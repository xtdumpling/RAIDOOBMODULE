/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IIoErrorHandler.c

Abstract:

  IIO based component hooks for event logging.

-------------------------------------------------------------------*/

#include "CommonErrorHandlerDefs.h"
#include <Protocol/SmmReadyToLock.h>
#include "PcieErrorHandler.h"
#include "IIoErrorHandler.h"
#include <Library/PciExpressLib.h>
#include <Library/SmmServicesTableLib.h>
#include "RcRegs.h"
#include "IIO_RAS.h"
#include "mca_msr.h"
#include <Setup/IioUniversalData.h>
#include <Protocol/IioUds.h>
#include <Protocol/IioSystem.h>
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Token.h>   //Register SW SMI to enable PCI error logging
#include <OemRasLib/OemRasLib.h>   //OEM Hooks Support
// APTIOV_SERVER_OVERRIDE_RC_END

UINT8   mPciXPUnsupportedRequest;
UINT8   mNextDevNum = 00;
SRIOV_UR_ERR_DEV_INFO    mErrDevIohInfo[MAX_NUM_ERR];
extern EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;

// APTIOV_SERVER_OVERRIDE_RC_START : Further SMI is not generated after 1 st SMI for PCI CE when using error counter mechanism
#define IIO_PCIE_XPCORERRCOUNTER_CLEAR        0xFFFF0000
// APTIOV_SERVER_OVERRIDE_RC_END : Further SMI is not generated after 1 st SMI for PCI CE when using error counter mechanism

////////////////////////////////////// COMMON ROUTINES /////////////////////////////////////////

/**
  Clears Extended IIO Errors.

  This function clears Extended IIO status registers.
  @param[in] Socket      The Socket numer
  @param[in] Bus         The Bus number.
  @param[in] Device      The Device number.
  @param[in] Function    The Function number.

  @retval  None.
**/
VOID
ClearIohExtPcieDeviceStatus(
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
)
{
  UINT32    Data32;
  UINT16    Data16;

  if (mRasSetup->Iio.IioErrRegistersClearEn == 0) {
    return;
  }
  
  Data32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRSTS_IIO_PCIE_REG)));
  MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRSTS_IIO_PCIE_REG)), Data32);

  if (mAllowClearingOfUEStatus)  {
    Data32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCERRSTS_IIO_PCIE_REG)));
    MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCERRSTS_IIO_PCIE_REG)), Data32);
  }

  Data32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCERRSTS_IIO_PCIE_REG))); //0.1.0
  MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCERRSTS_IIO_PCIE_REG)), Data32);

  Data32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (CORERRSTS_IIO_PCIE_REG))); //0.1.0
  MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (CORERRSTS_IIO_PCIE_REG)), Data32);

  Data32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRCOUNTER_IIO_PCIE_REG))); //0.1.0
  // APTIOV_SERVER_OVERRIDE_RC_START : Further SMI is not generated after 1 st SMI for PCI CE when using error counter mechanism
  MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRCOUNTER_IIO_PCIE_REG)), Data32 & IIO_PCIE_XPCORERRCOUNTER_CLEAR);
  // APTIOV_SERVER_OVERRIDE_RC_END : Further SMI is not generated after 1 st SMI for PCI CE when using error counter mechanism

  Data32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (RPERRSTS_IIO_PCIENTB_REG))); //0.3.0
  MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (RPERRSTS_IIO_PCIENTB_REG)), Data32);

  Data16 = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPGLBERRSTS_IIO_PCIE_REG)));
  MmioWrite16 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPGLBERRSTS_IIO_PCIE_REG)), Data16);
}

////////////////////////////////////// ERROR HANDLING ROUTINES /////////////////////////////////////////

/**

  This function check if a specific device is found.

  @param[in] Socket         The Socket number
  @param[in] Bus            The Bus number.
  @param[in] Device         The Device number.
  @param[in] Function       The Function number.
  @param[in] DevNumToAdd    The device number to add.

  @retval  Status           Return TRUE id the device is found.
**/
BOOLEAN
CheckForDeviceFound (
  IN      UINT8   Bus, 
  IN      UINT8   Device, 
  IN      UINT8   Function, 
  IN      UINT8   *DevNumToAdd 
)
{
  UINT8     i;
  BOOLEAN   FoundDev;

  FoundDev = FALSE;
  for (i = 0; i < MAX_NUM_ERR; i++) {
    if ((mErrDevIohInfo[i].BusNum == Bus) && (mErrDevIohInfo[i].Device == Device) && (mErrDevIohInfo[i].Function == Function)) {
      FoundDev = TRUE;
      *DevNumToAdd = i;
      return FoundDev;
    }
  }
  *DevNumToAdd = mNextDevNum;
  return FoundDev;
}

/**
  This function checks the XP Global Error Status register to see if one of the enabled errors
  has occurred.

  @param[in] Socket         PCIe root port device's socket number.
  @param[in] Bus            PCIe root port device's bus number.
  @param[in] Device         PCIe root port device's device number.
  @param[in] Function       PCIe root port device's function number. 

  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected, or the device is not a root port.
--*/
BOOLEAN
IioIsErrOnPcieRootPort (
  IN UINT8   Socket,
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
{
  UINT16    PcieCapabilities;
  UINT8     XpGlbErrSts;
  UINT8     XpCorErrSts;
  UINT8     XpUncErrSts;
  BOOLEAN   IsError;

  RASDEBUG ((DEBUG_INFO, "IioIsErrOnPcieRootPort  Bus = %x, Dev = %x, Func = %x, \n", Bus , Device, Function));

  IsError = FALSE;

  PcieCapabilities = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, R_PCIE_PORT_PXPCAP));
  if ((PcieCapabilities & 0xF0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {

    // Determine which errors are enabled
    XpGlbErrSts = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, R_PCIE_PORT_XPGLBERRSTS));
    XpCorErrSts = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, R_PCIE_PORT_XPCORERRSTS));
    XpUncErrSts = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, R_PCIE_PORT_XPUNCERRSTS));

    RASDEBUG ((XpGlbErrSts ? DEBUG_ERROR : DEBUG_INFO, "IioIsErrOnPcieRootPort XpGlbErrSts = %x, \n", XpGlbErrSts));
    RASDEBUG ((XpCorErrSts ? DEBUG_ERROR : DEBUG_INFO, "IioIsErrOnPcieRootPort XpCorErrSts = %x, \n", XpCorErrSts));
    RASDEBUG ((XpUncErrSts ? DEBUG_ERROR : DEBUG_INFO, "IioIsErrOnPcieRootPort XpUncErrSts = %x, \n", XpUncErrSts));


    if ( (XpCorErrSts &  B_PCIE_PORT_XPCORERRSTS_LBS) ||
         (XpUncErrSts &  V_PCIE_PORT_XPUNCERRSTS    ) ||
         (XpGlbErrSts & (B_PCIE_PORT_XPGLBERRSTS_NAER | B_PCIE_PORT_XPGLBERRSTS_FAER | B_PCIE_PORT_XPGLBERRSTS_CAER)) ) {
      IsError = TRUE;
    }
  }

  return IsError;
}


/**
  This function handles IIO PCI exp errors.

  @param[in] Ioh            The IOH number.

  @retval    Status.
**/
EFI_STATUS
IohPcieErrorHandler (
    IN      UINT8   Ioh,
    IN      UINT8   IioStack
  )
{

  UINT8                               Bus;
  UINT8                               Device;
  UINT8                               Function;
  UINT8                               Port;
  UINT8                               StackPerPort;
  CORERRMSK_IIO_PCIE_STRUCT           CorErrMsk;
  XPCORERRSTS_IIO_PCIE_STRUCT         XpCorErrSts;
  UNCERRMSK_IIO_PCIE_STRUCT           UncErrMsk;
  UNCERRSTS_IIO_PCIE_STRUCT           UncErrSts;
  LNKSTS_IIO_PCIE_STRUCT              LnkSts;
  BOOLEAN                             NmiGenerate;

  if(Ioh >= MAX_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }

  NmiGenerate = FALSE;
  mPciXPUnsupportedRequest = 00;
  RASDEBUG ((DEBUG_ERROR, "IohPcieErrorHandler start\n"));
  for(Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {

    StackPerPort = mIohInfo[Ioh].Port[Port].StackPerPort;
    
    if ((mIohInfo[Ioh].IioStackInfo[StackPerPort].StackValid != 1) && (StackPerPort != IioStack)) continue;

    Bus = mIohInfo[Ioh].Port[Port].Bus;
    Device = mIohInfo[Ioh].Port[Port].Device;
    Function = mIohInfo[Ioh].Port[Port].Function;

    // Check if error reporting is enabled for this device
    if (!PciErrLibIsRootPortErrReportingEnabled (Ioh, Bus, Device, Function)) {
      continue;
    }
    UncErrMsk.Data = MmioRead32 (MmPciAddress (Ioh, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCERRMSK_IIO_PCIE_REG)));
    UncErrSts.Data = MmioRead32 (MmPciAddress (Ioh, Bus, Device, Function, ONLY_REGISTER_OFFSET (UNCERRSTS_IIO_PCIE_REG)));
    CorErrMsk.Data = MmioRead32 (MmPciAddress (Ioh, Bus, Device, Function, ONLY_REGISTER_OFFSET (CORERRMSK_IIO_PCIE_REG)));
    XpCorErrSts.Data = MmioRead32 (MmPciAddress (Ioh, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRSTS_IIO_PCIE_REG)));

    if (XpCorErrSts.Data & BIT0) {
      LnkSts.Data = MmioRead16 (MmPciAddress (Ioh, Bus, Device, Function, ONLY_REGISTER_OFFSET (LNKSTS_IIO_PCIE_REG)));
      MmioWrite16 (MmPciAddress (Ioh, Bus, Device, Function, ONLY_REGISTER_OFFSET (LNKSTS_IIO_PCIE_REG)), LnkSts.Data);
    }

    if (IioIsErrOnPcieRootPort (Ioh, Bus, Device, Function)) {
      RASDEBUG ((DEBUG_ERROR, "  Error in root port:  Bus = %x, Dev = %x, Func = %x, \n", Bus , Device, Function));
      if (mPciXPUnsupportedRequest != 1) {
        PciErrLibPcieRootPortErrorHandler (Ioh, Bus, Device, Function);
      }
    }

      // Clear Extended error status registers
    if (!(mPcieFatalErrDetected || mPcieNonFatalErrDetected)) {
      ClearIohExtPcieDeviceStatus (Ioh, Bus, Device, Function);
    }

    if (mRasSetup->Iio.IoMcaEn == 0) {
      if ((NmiGenerate == TRUE) || mPcieFatalErrDetected || mPcieNonFatalErrDetected) {
        //Masking all global error reporting registers,without this there will be smi flood,
        //smi having high priority than NMI,NMI will never happen.
        MaskIIOErrorReporting ();
      }
    }
  }
  return EFI_SUCCESS;
}


////////////////////////////////////// INITIALIZATION ROUTINES /////////////////////////////////////////



/**
  This function masks PCI exp errors reporting.

  @retval    Status.
**/ 
EFI_STATUS
IohPcieMaskErrors(
  VOID
  )
{
  UINT8                        Port;
  UINT8                        Bus;
  UINT8                        Device;
  UINT8                        Function;
  UINT8                        RootBridgeLoop;
  UINT8                        IioStack;
  XPCORERRMSK_IIO_PCIE_STRUCT  XpCorErrMsk;
  XPUNCEDMASK_IIO_PCIE_STRUCT  XpUncErrDMsk;
  CORERRMSK_IIO_PCIE_STRUCT    CorErrMsk;


  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if (mIohInfo[RootBridgeLoop].Valid) {
      for (Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
        Bus = mIohInfo[RootBridgeLoop].Port[Port].Bus;
        Device = mIohInfo[RootBridgeLoop].Port[Port].Device;
        Function = mIohInfo[RootBridgeLoop].Port[Port].Function;
        if ((PciErrLibIsDevicePresent (0, Bus, Device, Function))
            && (PCIeIsRootPort(0, Bus, Device, Function))) { // Skip DMI connection
          XpCorErrMsk.Data = IIO_XPCORERR_MASK_DATA;
          MmioWrite32 (MmPciAddress(RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPCORERRMSK_IIO_PCIE_REG)), XpCorErrMsk.Data);
          XpUncErrDMsk.Data = IIO_XPUNCERRD_MASK_DATA;
          MmioWrite32 (MmPciAddress(RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (XPUNCEDMASK_IIO_PCIE_REG)), XpUncErrDMsk.Data);
          CorErrMsk.Data = IIO_AERCORERR_MASK_DATA;
          MmioWrite32 (MmPciAddress(RootBridgeLoop, Bus, Device, Function, ONLY_REGISTER_OFFSET (CORERRMSK_IIO_PCIE_REG)), CorErrMsk.Data);
        }
      }

      for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
    	MmioWrite32 (MmPciAddress(RootBridgeLoop, mIohInfo[RootBridgeLoop].IioStackInfo[IioStack].StackBus, IIO_STACK_RAS_DEV, IIO_STACK_RAS_FUNC, ONLY_REGISTER_OFFSET (LERRCTL_IIO_RAS_REG)), IIO_LERRCTL_MASK_DATA);
      }
    }
  }
  return EFI_SUCCESS;
}

/**
  re enumerate and update the topology Structure

  This function allocates memory for the RAS Topology Structure and populates it

  @param[out] **TopologyPtr   A pointer to the pointer for the RAS System Topology Structure

  @retval Status

**/
EFI_STATUS
IIOTopologyUpdateAtReadyToLock (
      OUT   EFI_RAS_SYSTEM_TOPOLOGY   **TopologyPtr
)
{
  EFI_RAS_SYSTEM_TOPOLOGY     *Topology;
  SYSTEM_INFO                 *SystemInfo;
  IIO_UDS                     *IioUdsData = NULL;
  EFI_IIO_UDS_PROTOCOL        *IioUds = NULL;
  UINT8                       Skt;
  UINT8                       PciPort;
  UINT8                       IioStack;
  UINT8                       Count;
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_IIO_SYSTEM_PROTOCOL     *IioSystem;

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &IioUds);
  ASSERT_EFI_ERROR (Status);
  IioUdsData = (IIO_UDS *)IioUds->IioUdsPtr;

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystem);
  ASSERT_EFI_ERROR (Status);

  Topology = *TopologyPtr;
  SystemInfo = &Topology->SystemInfo;

  for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
    if ((IioUdsData->SystemStatus.socketPresentBitMap & (1 << Skt)) == 0)
      continue;

    SystemInfo->SocketInfo[Skt].UncoreInfo.M3UpiCount = IioUdsData->PlatformData.CpuQpiInfo[Skt].TotM3Kti;
    SystemInfo->SocketInfo[Skt].UncoreInfo.M2PcieCount = 0;
    for (Count = 0; Count < 8; Count++) { // M2PciePresentBitMap = UINT8
      if ((IioUdsData->PlatformData.CpuQpiInfo[Skt].M2PciePresentBitmap & (1 << Count)) != 0) {
        SystemInfo->SocketInfo[Skt].UncoreInfo.M2PcieCount++;
      }
    }

    if (SystemInfo->SocketInfo[Skt].UncoreIioInfo.Valid != 0) {
      SystemInfo->SocketInfo[Skt].UncoreIioInfo.SocketId = IioUdsData->PlatformData.IIO_resource[Skt].SocketID;
      SystemInfo->SocketInfo[Skt].UncoreIioInfo.IIOBusBase = IioUdsData->PlatformData.IIO_resource[Skt].BusBase;
      for (PciPort = 0; PciPort < NUMBER_PORTS_PER_SOCKET; PciPort++) {
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].BusDevFun =
          (IioUdsData->PlatformData.IIO_resource[Skt].BusBase << 16) + (IioUdsData->PlatformData.IIO_resource[Skt].PcieInfo.PortInfo[PciPort].Device << 8) + IioUdsData->PlatformData.IIO_resource[Skt].PcieInfo.PortInfo[PciPort].Function; // Bus [23:16], Dev [15:8], Fun [7:0]
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].Bus = IioSystem->IioGlobalData->IioVar.IioVData.SocketPortBusNumber[Skt][PciPort];
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].Device = IioSystem->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PciPort].Device;
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].Function = IioSystem->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PciPort].Function;
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].StackPerPort = IioSystem->IioGlobalData->IioVar.IioVData.StackPerPort[Skt][PciPort];
     }
    // Copy over the IIO Stack info
      for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.IioStackInfo[IioStack].Valid = (IioUdsData->PlatformData.CpuQpiInfo[Skt].stackPresentBitmap & (1 << IioStack)) != 0;
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.IioStackInfo[IioStack].BusBase = IioUdsData->PlatformData.IIO_resource[Skt].StackRes[IioStack].BusBase;
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.IioStackInfo[IioStack].BusLimit = IioUdsData->PlatformData.IIO_resource[Skt].StackRes[IioStack].BusLimit;
      }
    }
  }
  return Status;
}

/**
  This function will enable IIO error reporting.

  @param[in] Protocol        Protocol unique ID.
  @param[in] Interface            Interface instance.
  @param[in] Handle      The handle on which the interface is installed..

  @retval    Status.
**/
   // APTIOV_SERVER_OVERRIDE_RC_START
EFI_STATUS 
IIOErrorReadyToLockCallback (
  IN      CONST EFI_GUID   *Protocol,
  IN      VOID             *Interface,
  IN      EFI_HANDLE        Handle
  )

{

  RASDEBUG ((DEBUG_INFO, "IIOErrorReadyToLockCallback: Inside the function\n"));
  IIOTopologyUpdateAtReadyToLock (&mPlatformRasPolicyProtocol->EfiRasSystemTopology);
  InitIOHInfo ();

  return EFI_SUCCESS;

}
//Register SW SMI to enable PCI error logging
EFI_STATUS
IIOErrorEnableCallBack (
  IN        EFI_HANDLE                      DispatchHandle,
  IN CONST  EFI_SMM_SW_REGISTER_CONTEXT     *RegisterContext,
  IN OUT    EFI_SMM_SW_CONTEXT              *SwContext,
  IN OUT    UINTN                           *CommBufferSize )
   // APTIOV_SERVER_OVERRIDE_RC_END
{
  UINT8       RootBridgeLoop;
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  // Loop over all RootBridges
  RASDEBUG ((DEBUG_INFO, "IIOErrorEnableCallBack: Inside the function\n"));
// APTIOV_SERVER_OVERRIDE_RC_START : To implement AMI functionality in Ready To Lock
//  IIOTopologyUpdateAtReadyToLock (&mPlatformRasPolicyProtocol->EfiRasSystemTopology);
//  InitIOHInfo ();
// APTIOV_SERVER_OVERRIDE_RC_END : To implement AMI functionality in Ready To Lock
  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if (mIohInfo[RootBridgeLoop].Valid) {
      Status = IIOErrorEnable (RootBridgeLoop);
      ASSERT_EFI_ERROR(Status);
     }
  }
  
  // Enable PCH Error reporting (Like ICH, SIOH etc).
  EnableElogPCH ();
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook support added for Enabling/Disabling PCIe Error Reporting Registers
  OemPciErrEnableDisableReporting(mPlatformRasPolicyProtocol);
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook support added for Enabling/Disabling PCIe Error Reporting Registers
  return Status;
}

/**
  This function initialize the error device info.

  @retval    None.
**/
VOID
InitErrDevInfo ()
{ 
  UINT8   i;

  for (i = 0; i < MAX_NUM_ERR; i++) {
    mErrDevIohInfo[i].BusNum = 00;
    mErrDevIohInfo[i].Device = 00;
    mErrDevIohInfo[i].Function = 00;
    mErrDevIohInfo[i].NmiSciFlag = 00;
    mErrDevIohInfo[i].UrMaskSet = 00;
    mErrDevIohInfo[i].HdrLog1 = 00;
		}
	}


/**
  This function is used to register the function that enable the IIO error reporting.
  
  @retval    Status.
**/
EFI_STATUS
EnableElogIoHub (
  VOID
  )
{
  EFI_STATUS    Status;
  UINT8         RootBridgeLoop;
  VOID          *Registration;
// APTIOV_SERVER_OVERRIDE_RC_START : Register SW SMI to enable PCI error logging
  EFI_SMM_SW_DISPATCH2_PROTOCOL   *SwDispatch = NULL;
  EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
  EFI_HANDLE                      SwHandle;
// APTIOV_SERVER_OVERRIDE_RC_END : Register SW SMI to enable PCI error logging
 
  for (RootBridgeLoop = 0; RootBridgeLoop < MAX_SOCKET; RootBridgeLoop++) {
    if (mIohInfo[RootBridgeLoop].Valid) {
      // This function clears the Iio smi mask bits and enable SMI_MSI enable bits.
      SetupIioCipIntrCtrl (RootBridgeLoop);
    }
  }

  // Setup call back after pci space initialized
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmReadyToLockProtocolGuid,
             // APTIOV_SERVER_OVERRIDE_RC_START : To implement AMI functionality in Ready To Lock
                    IIOErrorReadyToLockCallback,
             // APTIOV_SERVER_OVERRIDE_RC_END : To implement AMI functionality in Ready To Lock
                    &Registration
                    );

// APTIOV_SERVER_OVERRIDE_RC_START : Register SW SMI to enable PCI error logging
      Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &SwDispatch);
      ASSERT_EFI_ERROR (Status);
      DEBUG((DEBUG_INFO,"SmmLocateProtocol gEfiSmmSwDispatch2ProtocolGuid: Status: %r .... \n", Status));
      if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_LOAD, "Couldn't find SmgSwDispatch2 protocol: %r\n", Status));
          return Status;
      }

      //
      // Register the SW SMI to enable PCI error logging.
      //
      SwContext.SwSmiInputValue = PCIE_ELOG_SWSMI;
      Status = SwDispatch->Register (
                  SwDispatch,
                  IIOErrorEnableCallBack,
                  &SwContext,
                  &SwHandle);
      DEBUG((DEBUG_INFO,"Register IIOErrorEnableCallBack: Status: %r .... \n", Status));
      if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_LOAD, "Couldn't register the PCI ELOG SW SMI handler.Status: %r\n", Status));
          return Status;
      }
// APTIOV_SERVER_OVERRIDE_RC_END : Register SW SMI to enable PCI error logging

  RASDEBUG ((DEBUG_ERROR, "EnableElogIoHub, Status; %x   \n", Status));
  return Status;
}

/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
EFI_STATUS
IioDmiErrorEnable (
  VOID)
{
  UINT8 Socket=0;
  UINT8 Port=0;
  UINT8 Device,Function, Bus;
  UINT16  StatusReg16,AerRegisterOffset;
  UINT8 CapabilitiesOffset;
  UINT16 PcieCapabilities;
  BOOLEAN RootPort;
  UINT16 RootControl;
  UINT16 DeviceControl;

  Bus = mIohInfo[Socket].Port[Port].Bus;
  Device = mIohInfo[Socket].Port[Port].Device;
  Function = mIohInfo[Socket].Port[Port].Function;

  if (!PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
    return EFI_SUCCESS;
  }
 
  //clear legacy pcie device status
  StatusReg16 = MmioRead16 (MmPciAddress(Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));
  MmioWrite16 (MmPciAddress(Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET), StatusReg16);

  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function,EFI_PCI_CAPABILITY_ID_PCIEXP);
  if (CapabilitiesOffset == 0) {
    return EFI_SUCCESS;
  }

  PcieCapabilities = MmioRead16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_CAPABILITY_OFFSET));
  if ((PcieCapabilities & 0xF0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    RootPort = TRUE;
  } else {
    RootPort = FALSE;
  }
  //
  // Clear the PCIe device status register
  //
  StatusReg16 = MmioRead16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + R_DMI_DEVICE_STATUS_OFFSET));
  MmioWrite16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + R_DMI_DEVICE_STATUS_OFFSET), StatusReg16);

  //
  // Check for presence of AER 
  //
  AerRegisterOffset = PciErrLibGetExtendedCapabilitiesOffset(0, Bus, Device, Function,V_DMI_AER_CAPID,V_DMI_AER_VSEC_CAPID);
  if (AerRegisterOffset != 0) {
  //
  // Program AER mask registers for uncorrectable and correctable errors,if any
  //
  
  //
  // Program AER severity registers for uncorrectable,if any
  //

 
  //
  // Clear AER Status registers
  //
  ClearIohExtPcieDeviceStatus (Socket, Bus, Device, Function);
  }

  //PCIE basic error handling enable...
 
  //
  // Configure Root Control Register ..
  //
  if (RootPort) {
    RootControl = MmioRead16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_ROOT_CONTROL_OFFSET));
    RootControl |= (B_PCIE_ROOT_CONTROL_COR_ERR_EN | B_PCIE_ROOT_CONTROL_NF_ERR_EN | B_PCIE_ROOT_CONTROL_FAT_ERR_EN);
    MmioWrite16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_ROOT_CONTROL_OFFSET), RootControl);
  }

  //
  // Enable Error logging in Device Control...
  //
  DeviceControl = MmioRead16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + R_DMI_DEVICE_CONTROL_OFFSET));
  DeviceControl |= (B_PCIE_DEVICE_CONTROL_COR_ERR_EN| B_PCIE_DEVICE_CONTROL_NF_ERR_EN | B_PCIE_DEVICE_CONTROL_FAT_ERR_EN);
  MmioWrite16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + R_DMI_DEVICE_CONTROL_OFFSET), DeviceControl);

  
return EFI_SUCCESS;

}
