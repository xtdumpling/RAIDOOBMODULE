/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017   Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioPostLateInitialize.c

  Implement IIO initialization protocol to provide IIO initialization functions.
  This interfaces will be called by PciHostBridge driver in the PCI bus enumeration.

**/
#include <SysFunc.h>
#include <Library/OemIioInit.h>
#include <Library/IioUtilityLib.h>
#include <Library/IioInitLib.h>
#include <Library/IioInitDxeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcieCommonInitLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Protocol/IioSystem.h>
#include <Protocol/PciCallback.h>
#include <IioSetupDefinitions.h>
#include <RcRegs.h>
#include <SysHostChip.h>
#include "IioIoatInit.h"
#include "IioDevHide.h"

extern PCIE_LIB_TOPOLOGY_INF PcieTopology;

// APTIOV_SERVER_OVERRIDE_RC_START : Pcie Common Clock programming . 
PCIE_LIB_ROOTPORT_FEATURE_NODE FeatureTableTemplate[] ={
                                               {PCIE_LIB_FEATURE_COMMONCLOCK},
                                               {PCIE_LIB_FEATURE_ASPM},
                                               {PCIE_LIB_FEATURE_CTO},
                                               {PCIE_LIB_FEATURE_MAXPAYLOAD},
                                               {PCIE_LIB_FEATURE_EXTTAGFIELD},
                                             
                                               {PCIE_LIB_FEATURE_ATOMICOP},
                                               {PCIE_LIB_FEATURE_LTR},
                                               {PCIE_LIB_FEATURE_MRRS},
                                               {PCIE_LIB_FEATURE_MAX}};
// APTIOV_SERVER_OVERRIDE_RC_END : 
/**

    This function initializes FeatureTableTemplate list with the proper values for ASPM, LTR, AtomicOp,
    MaxPayload, CommonClock and ExtentedTAg Field for IIO Root ports and EndPoints.

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
InitLibParameters (
   IN IIO_GLOBALS  * IioGlobalData
  )
{
  UINT8                           IioIndex;
  UINT8                           PortIndex;
  UINT8                           Index;
  UINT8                           Stack;
  UINT8                           MaxStackPort;
  UINT8                           PortStackIndex;
  PCIE_LIB_ROOT_PORT_INF          *RootPortInfo;
  PCIE_LIB_ROOTPORT_FEATURE_NODE  *FeatureNode;
  EFI_STATUS                      Status;
  PCIE_LIB_ASPM_INF               *AspmInfo;
  PCIE_LIB_CTO_INF                *CtoInfo;
  PCIE_LIB_EXTTAGFIELD_INF        *ExtTagField;
  PCIE_LIB_ATOMICOP_INF           *AtomicOp;
  PCIE_LIB_LTR_INF                *Ltr;
  UINT8                           *Array;
  PCIE_LIB_MAXPAYLOAD_INF         *PlatformPayload;
  // APTIOV_SERVER_OVERRIDE_RC_START : Pcie Common Clock programming 
  PCIE_LIB_COMMONCLOCK_INF        *CommonClock;
  // APTIOV_SERVER_OVERRIDE_RC_END 
  PCIE_LIB_TOPOLOGY_INF           *PcieLibTopInf;
  PCIE_LIB_MRRS_INF               *Mrrs;

  //
  // Load HostBridgeList LUT as pointer!
  //
  IioGlobalData->IioVar.IioOutData.PtrPcieTopology.Address64bit =(UINT64) &PcieTopology;

  PcieLibTopInf =  (PCIE_LIB_TOPOLOGY_INF*)(IioGlobalData->IioVar.IioOutData.PtrPcieTopology.Address64bit);


  //
  // Init Hostbridge information
  //

  for ( IioIndex = 0; IioIndex < MaxIIO; IioIndex++ ) {

    Stack = 0;

    if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]) {
      PcieLibTopInf->HostBridgeList[(IioIndex * MAX_IIO_STACK) + Stack].Revision = PCIE_LIB_HOSTBRIDGE_REVISION_END;
      break;
    }
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, " Socket = %x!\n",IioIndex);
    PortIndex = 0;
    while (Stack < MAX_IIO_STACK){
        if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
          //
          // Skip this element if Stack is not present
          //
          Stack++;
          continue;
        }

        if (IioGlobalData->IioVar.IioVData.SocketStackBaseBusNumber[IioIndex][Stack] == IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[IioIndex][Stack] ){
          //
          // Skip this element if no extra buses assigned
          //
          PcieLibTopInf->HostBridgeList[(IioIndex * MAX_IIO_STACK) + Stack].Revision = 0xFF;
          Stack++;
          continue;
        }

        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, " HostBridge:[%d]!\n",(IioIndex * MAX_IIO_STACK) + Stack);

        PcieLibTopInf->HostBridgeList[(IioIndex * MAX_IIO_STACK) + Stack].TempBusMin = IioGlobalData->IioVar.IioVData.SocketStackBaseBusNumber[IioIndex][Stack] + 1;
        PcieLibTopInf->HostBridgeList[(IioIndex * MAX_IIO_STACK) + Stack].TempBusMax = IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[IioIndex][Stack];
        PcieLibTopInf->HostBridgeList[(IioIndex * MAX_IIO_STACK) + Stack].Revision = PCIE_LIB_HOSTBRIDGE_REVISION_1;

        //
        // Init Root port Information
        //
        RootPortInfo = PcieLibTopInf->HostBridgeList[(IioIndex * MAX_IIO_STACK) + Stack].RootPortList;
        //
        // Get the maximum port numbers in a Stack 0-3 for P-STACK and 1 for C-STACK
        //
        MaxStackPort = GetMaxPortPerStack(Stack);
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, " MaxPort:[%d]!\n",MaxStackPort);

        for (PortStackIndex = 0; PortStackIndex < MaxStackPort; PortStackIndex++) {
          //
          // Now get the PortIndex per socket 0-13
          //
          PCIESTACKPORTINDEX(PortStackIndex, Stack, PortIndex);
          if(RootPortInfo[PortStackIndex].Valid == 0){
             break;
          }
          //
          // Check if port is configured as VMD, if so we need to skip the pci scan process
          //
          if (IioGlobalData->IioVar.IioOutData.PciePortOwnership[PortStackIndex] == VMD_OWNERSHIP){
            continue;
          }

          IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, " -->Rootport:[%d] PortIndex [%d]!\n",PortStackIndex, PortIndex);

          RootPortInfo[PortStackIndex].Bus     = IioGlobalData->IioVar.IioVData.SocketStackBus[IioIndex][Stack];
          RootPortInfo[PortStackIndex].PortNum = PortIndex;
          IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, " -->Rootport Bus:[%x] Port[%x]!\n",RootPortInfo[PortStackIndex].Bus, PortStackIndex);
         //
         // Allocate Memory Pool for Universal Data Storage so that protocol can expose it
         //
         Status = gBS->AllocatePool ( EfiBootServicesData, sizeof (FeatureTableTemplate), (VOID **) &FeatureNode );
         ASSERT_EFI_ERROR (Status);

         RootPortInfo[PortStackIndex].FeatureList = FeatureNode;

         CopyMem(FeatureNode, FeatureTableTemplate, sizeof(FeatureTableTemplate));

         Index = 0;

         //
         // Init Feature node
         //
         while (FeatureNode[Index].Common.Type != PCIE_LIB_FEATURE_MAX) {
           IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, " -->FeatureNode:[%d].Type = %x!\n",Index, FeatureNode[Index].Common.Type);

           switch(FeatureNode[Index].Common.Type) {
             case PCIE_LIB_FEATURE_VPP:
                FeatureNode[Index].Common.Type = PCIE_LIB_FEATURE_SKIP; //Not support it
                break;

             case PCIE_LIB_FEATURE_CTO:
                CtoInfo = &FeatureNode[Index].Cto;

                if (IioGlobalData->SetupData.CompletionTimeoutGlobal != 2) {
                  if (IioGlobalData->SetupData.CompletionTimeoutGlobal == 1){
                    CtoInfo->Policy = PcieLibCompletionTO_Disabled;
                  } else {
                  CtoInfo->Policy = IioGlobalData->SetupData.CompletionTimeoutGlobalValue;
                  }
                } else {
                  Array = &(IioGlobalData->SetupData.CompletionTimeout[0]);
                  if (Array[IioIndex]) {
                    CtoInfo->Policy = PcieLibCompletionTO_Disabled;
                    
                  } else {
                    Array = &(IioGlobalData->SetupData.CompletionTimeoutValue[0]);
                    CtoInfo->Policy = Array[IioIndex];
                  }
                }
                break;

// APTIOV_SERVER_OVERRIDE_RC_START : Pcie Common Clock programming                 
             case PCIE_LIB_FEATURE_COMMONCLOCK : 
                 CommonClock = &FeatureNode[Index].CommonClock;
                 CommonClock->Flag = IioGlobalData->SetupData.PcieCommonClock[Index];
                 CommonClock->Type = PCIE_LIB_FEATURE_COMMONCLOCK;
                 IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "[CommonClock]->setup:%d\n",CommonClock->Flag);
                 break;
// APTIOV_SERVER_OVERRIDE_RC_END
             case PCIE_LIB_FEATURE_SLOTCONTROL:
                break;

             case PCIE_LIB_FEATURE_ASPM:
               AspmInfo = &FeatureNode[Index].Aspm;
               //
               // Verify if PCieASPMGlobal option is either disable or L1 
               // otherwise use PerPort logic 
               // 
               if (IioGlobalData->SetupData.PcieGlobalAspm != 1) {
                 AspmInfo->Policy = IioGlobalData->SetupData.PcieGlobalAspm;
               } else {
                 Array = IioGlobalData->SetupData.PcieAspm;
                 if (Array[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] != PCIE_ASPM_AUTO) {
                   AspmInfo->Policy =  Array[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex];
                 } else {
                   AspmInfo->Policy = PcieLibAspmAutoConfig;
                 }
               }
               break;

             case PCIE_LIB_FEATURE_MAXPAYLOAD:
                PlatformPayload = &FeatureNode[Index].MaxPayload;
                PlatformPayload->Flag = PCIE_LIB_MAXPAYLOAD_OVERRIDE;
                Array = &(IioGlobalData->SetupData.PcieMaxPayload[0]);
                if (Array[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex] != 2){
                  PlatformPayload->MaxPayload = Array[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex];
                } else {
                  PlatformPayload->MaxPayload = 1; // Set 256B by default
                }
                IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "[MaxPayload]->setup:%d\n",PlatformPayload->MaxPayload);
                break;

             case PCIE_LIB_FEATURE_EXTTAGFIELD:
                ExtTagField = &FeatureNode[Index].ExtendedTagField;
                ExtTagField->Flag = IioGlobalData->SetupData.PcieExtendedTagField;
                IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "[ExtTagField]->Flag %x \n",ExtTagField->Flag);
                break;

             case PCIE_LIB_FEATURE_ATOMICOP:
                AtomicOp = &FeatureNode[Index].AtomicOp;
                AtomicOp->Flag = IioGlobalData->SetupData.PCIe_AtomicOpReq;
                IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "[AtomicOp]->Flag %x \n",AtomicOp->Flag);
                break;

             case PCIE_LIB_FEATURE_LTR:
                Ltr = &FeatureNode[Index].Ltr;
                if ((IioGlobalData->SetupData.PCIe_LTR == 2) || (IioGlobalData->SetupData.PCIe_LTR == 1)){
                  Ltr->Flag = PCIE_LIB_LTR_ENABLE;
                } else {
                  Ltr->Flag = IioGlobalData->SetupData.PCIe_LTR;
                }
                IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "[Ltr]->Flag %x \n",Ltr->Flag);
                break;
             case PCIE_LIB_FEATURE_MRRS:
                Mrrs = &FeatureNode[Index].Mrrs;
                if (IioGlobalData->SetupData.PcieMaxReadRequestSize != 7) {
                  Mrrs->Flag = PCIE_LIB_MRRS_OVERRIDE;
                  Mrrs->MaxReadReqSize = IioGlobalData->SetupData.PcieMaxReadRequestSize;
                } else {
                  Mrrs->Flag = PCIE_LIB_MRRS_AUTO;
                }
                break;
             default:
                // can't handle this type, just skip
                break;
           } //End switch
          Index++;
         } //End While
      }//End PortIndex

      Stack++;

    } //End Stack
  } //End IioIndex

}

/**

    This function Execute any code need it before program Pcie Features in IIO
    Root ports and End Points

    @param Event - Pointer to PCIE_HOOK_EVENT
    @param Context - Pointer to VOID

    @retval None

**/
EFI_STATUS
EFIAPI
PciePlatformHookEvent (
  IN PCIE_HOOK_EVENT    Event,
  IN VOID               *Context
  )
{
  return EFI_SUCCESS;
}


/**

    This function calls Pcie Common Lib to set the proper values for ASPM, LTR, AtomicOp,
    MaxPayload, CommonClock and ExtentedTAg Field for IIO Root ports and EndPoints.

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
PcieLateCommonLibInit (
   IN IIO_GLOBALS       *IioGlobalData
  )
{

   InitLibParameters(IioGlobalData);

   // Example to call PcieInfoTreePaser
   PcieInfoTreePaser ((PCIE_LIB_TOPOLOGY_INF*)(IioGlobalData->IioVar.IioOutData.PtrPcieTopology.Address64bit));

}


/**

   Function that writes all the CSRs and MSRs defined in the SeCoE BIOS
   recommendations document to allow the ACM to later do and SCHECK and
   find the platform to be secure.

// ********************************************************************
// ******************   WARNING  **************************************
// *******  Settings within this function must not be changed *********
// *******  without reviewing the changes with SeCoE or the   *********
// *******  TXT BIOS owner.  Changes to this function will    *********
// *******  produce corresponding failures to the ACM Lock    *********
// *******  Configuration Checks and will prevent Trusted     *********
// *******  Boot.                                             *********
// ********************************************************************
// ********************************************************************

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/

VOID
TxtSecurePlatform(
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex
  )
{

  UINT16                                       PlkCtl;
  UINT32                                       MsDevFuncHide;
  ERRINJCON_IIO_PCIE_STRUCT                    ErrInjCon;
  IIO_DFX_LCK_CTL_CSR_IIO_DFX_GLOBAL_STRUCT    IioDfxLock;
  TSWCTL0_IIO_DFX_GLOBAL_STRUCT                TswCtl0;
  DMIRCBAR_IIO_PCIEDMI_STRUCT                  DmiRcBar;
  UINT8                                        Stack;
  UINT8                                        PortIndex;


  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "IioInit Secure the Platform (TXT)..\n");

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {

    if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
      continue;
    }

    // IIO LOCKS BELOW
    //
    // 4986493: Error Injection changes to enable Windows Hardware Error Architecture
    //          This change request supercedes the following HSDs: 4167177, 4168974, 4167708, 4986303
    //
    //
    // HSD 4167177: TXT BIOS Must Lock Error Injections CSRs
    //       Revised by HSD 4986493 to only lock DMI port error injection
    //
    if ((IioIndex == 0) && (Stack == IIO_CSTACK)) {
      ErrInjCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, ERRINJCON_IIO_PCIEDMI_REG);
      ErrInjCon.Bits.errinjdis = 1;
      IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, ERRINJCON_IIO_PCIEDMI_REG, ErrInjCon.Data);

      //
      // HSD 4166543: Implement Secure Memory Map Configuration as per SeCoE recommendation
      // ivt_eco: s4540054:  [legacy] DMIRCBAR registers that have *_LB fields are not lockable by LT-lock (or anything)
      //  [clone to s4030142] - permanent workaround
      //
      DmiRcBar.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, DMIRCBAR_IIO_PCIEDMI_REG);
      DmiRcBar.Bits.dmircbaren = 0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, DMIRCBAR_IIO_PCIEDMI_REG, DmiRcBar.Data);
    }
    //
    //  HSD: 5385744  ERRINJCON.ERRINJDIS should be set by BIOS in every boot to disable error injection
    //
    if(IioGlobalData->SetupData.WheaPcieErrInjEn == 0x00){
      if (IioGlobalData->IioVar.IioVData.SocketStackPersonality[IioIndex][Stack] != TYPE_DISABLED) {
        for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
          ErrInjCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, ERRINJCON_IIO_PCIE_REG);
          ErrInjCon.Bits.errinjdis = 1;
          ErrInjCon.Bits.cause_rcverr = 0;
          ErrInjCon.Bits.cause_ctoerr = 0;
          IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, ERRINJCON_IIO_PCIE_REG, ErrInjCon.Data);
        }
      }
    }  
    //
    // end HSD:5385744
    //

    //
    // HSD 4166541: Implement HSX/BDX Processor Security Locks as per SeCoE Recommendation
    //
    IioDfxLock.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, IIO_DFX_LCK_CTL_CSR_IIO_DFX_GLOBAL_REG);

    IioDfxLock.Data |= 0xFF;
    // HSD 4986116: Set IIO_DFX_LCK_CTL.ntblck bit
    IioDfxLock.Bits.ntblck = 1;
    // HSD 4928330: Set IIO_DFX_LCK_CTL.genviral_lck bit
    IioDfxLock.Bits.genviral_lck = 1;
    IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, IIO_DFX_LCK_CTL_CSR_IIO_DFX_GLOBAL_REG, IioDfxLock.Data);

    //IioDfxLock.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, IIO_DFX_LCK_CTL_CSR_IIO_DFX_GLOBAL_REG);
    //
    // HSD 4166541: Implement HSX/BDX Processor Security Locks as per SeCoE Recommendation
    //
    TswCtl0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, TSWCTL0_IIO_DFX_GLOBAL_REG);
    TswCtl0.Bits.me_disable = 1;
    TswCtl0.Bits.allow_ib_mmio_cfg = 0;
    TswCtl0.Bits.ignore_acs_p2p_ma_lpbk = 0;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, TSWCTL0_IIO_DFX_GLOBAL_REG, TswCtl0.Data);


  }  // End of for (Stack; Stack <MAX_IIO_STACK; Stack ++)


  if (IioIndex == SOCKET_0_INDEX){
  //
  // HSD 5370891: Security VT][SDL S3] ChipSec failures - Iio
  // Setting PLKCTL bit (bit0) Capability lock, down SSID and SVID registers
  //
  PlkCtl = IioPciExpressRead16(IioGlobalData, 0, 0, PCH_LBG_MROM0_PORT_DEVICE, PCH_LBG_MROM0_PORT_FUNCTION, PCH_LBG_MSUINT_MROM0_PLKCTL_REG);
  PlkCtl |= BIT0;
  IioPciExpressWrite16(IioGlobalData, 0, 0, PCH_LBG_MROM0_PORT_DEVICE, PCH_LBG_MROM0_PORT_FUNCTION, PCH_LBG_MSUINT_MROM0_PLKCTL_REG, PlkCtl);

  MsDevFuncHide = IioPciExpressRead32(IioGlobalData, 0, 0, PCH_LBG_MROM0_PORT_DEVICE, PCH_LBG_MROM0_PORT_FUNCTION, PCH_LBG_MSUINT_MROM0_MSDEVFUNCHIDE_REG);
  MsDevFuncHide |= BIT31;
  IioPciExpressWrite32(IioGlobalData, 0, 0, PCH_LBG_MROM0_PORT_DEVICE, PCH_LBG_MROM0_PORT_FUNCTION, PCH_LBG_MSUINT_MROM0_MSDEVFUNCHIDE_REG, MsDevFuncHide);
  } // IiOIndex == 0

}


/**

    This function does the final IIO initialization *common* in both DXE Init and Runtime Init (RAS).
    Intended to be used as either part of a callback, or during runtime, as a standalone function call.

    @param *IioGlobalData   - Pointer to Global Data Structure.

    @retval None

**/
VOID
IioCommonInitAfterResources (
  IN  IIO_GLOBALS  *IioGlobalData
)
{
  UINT8   IioIndex;

  for ( IioIndex = 0 ; IioIndex < MaxIIO ; IioIndex++ ){
    if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }

    //
    // init the IOAPIC after the MBAR allocation during the system PCI bus enumeration
    //
    IioIoApicInitBootEvent(IioGlobalData, IioIndex);

    if ((IioGlobalData->SetupData.LockChipset) && (!IioGlobalData->IioVar.IioVData.EVMode)) {
      TxtSecurePlatform(IioGlobalData, IioIndex);
    }

    //
    // init IOAT features here as the CB BAR needs to be programmed by the
    // system PCI bus driver
    //
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR,"Calling IoatInitBootEvent IioIndex: %d\n",IioIndex );
    IioIoatInitBootEvent(IioGlobalData, IioIndex);

    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "Calling IioDisableLinkPorts IioIndex: %d\n",IioIndex);
    // Disable Link Ports if no EndPoints behind
    IioDisableLinkPorts(IioGlobalData, IioIndex);
    //
    // HSD 4168370 Add EVMode Support to the HSX EP BIOS
    //
    if ((!IioGlobalData->IioVar.IioVData.EVMode)){
      IioMiscHide(IioGlobalData, IioIndex, IioAfterResources, IioDevFuncHideTable);  // Use uncore bus to hide devices
    }

    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR,"Calling NtbLateInit IioIndex: %d\n",IioIndex );
    NtbLateInit(IioGlobalData, IioIndex);
  }

  // Update SVID for IIO devices
  UpdateSsids (IioGlobalData);
}
/**

    This function does the final IIO initialization intended for callback invokation.

    @param RootBridgeHandle - Handle to RootBridge
    @param Address          - Pcie Address of the RootBridge
    @param PciPhase         - Pci Phase
    @param Context          - Contex of the function

    @retval None

**/
VOID
EFIAPI
IioInitAfterResources (
  IN  EFI_HANDLE                                   RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  Address,
  IN  EFI_PCI_ENUMERATION_PHASE                    PciPhase,
  IN  EFI_PCI_CALLBACK_CONTEXT                     *Context
  )
{
    EFI_STATUS Status;
    EFI_IIO_SYSTEM_PROTOCOL       *IioSystemProtocol = NULL;
    IIO_GLOBALS                   *IioGlobalData = NULL;

    ///
    /// Check whether this is real IioSystemProtocol notification, or just a SignalEvent
    ///

    Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
    if (EFI_ERROR(Status))
      return;

    IioGlobalData = IioSystemProtocol->IioGlobalData;

    //
    // If this callback routine on its ready to boot event, than avoid executing
    // multiple times (fix for s4030764)
    //
    if (IioGlobalData->IioVar.IioVData.CompletedReadyToBootEventServices == TRUE)
      return;

    IioBreakAtCheckPoint (IioGlobalData, STS_IIO_ON_READY_TO_BOOT, 0, 0xFF);

    IioCommonInitAfterResources(IioGlobalData);

    IioGlobalData->IioVar.IioVData.CompletedReadyToBootEventServices = TRUE;    //fix for s4030764
}

/**
  Initialize MultiCast as per validation requirement.
  (It is specific DualCast configuration using NTB as documented in the IIO spec.
  Regular system-wide Multicast configuration/policy is expected to be configured by OS/Device drivers.)


  @param[in]  IioGlobalData      Platform configuration info
  @param      IioIndex           Current socket.
  @param      PortIndex          Current port.

  @retval     MCAST_RET_SUCCESS Configured without any error
  @retval     Non-zero values   Other specific failures
**/
UINT8
EnableMultiCast (
  IN  IIO_GLOBALS           *IioGlobalData,
  IN  UINT8                 IioIndex,
  IN  UINT8                 PortIndex
  )
{
  EFI_STATUS              Status;
  UINT32                  i;
  UINT8                   Stack;
  UINT32                  Data32;
  EFI_PHYSICAL_ADDRESS    McastBaseAddress;
  EFI_ALLOCATE_TYPE       McastMemRangeType;
  UINTN                   McastRsvdMemSize;
  UINT8                   McastNumGrp;
  UINT8                   McastIndxPos;
  MCAST_BASE_N0_IIO_PCIE_STRUCT    McastBaseN0;
  MCAST_BASE_N1_IIO_PCIE_STRUCT    McastBaseN1;
  MCAST_BLK_ALL_N0_IIO_PCIE_STRUCT McastBlkAllN0;
  MCAST_CTRL_IIO_PCIE_STRUCT       McastCtrl;
  TCERRCTL_IIO_RAS_STRUCT          TcErrCtl;
  OTCERRSEV_IIO_RAS_STRUCT         OtcErrSev;
  TCERRST_IIO_RAS_STRUCT           TcErrSt;

  if ((IioGlobalData->IioVar.IioVData.CpuType != CPU_SKX)){
    return MCAST_ERR_NOT_SUPPORTED;
  }

  //
  // Read in configured number of groups and index position
  //
  McastNumGrp   = IioGlobalData->SetupData.McastNumGroup;
  McastIndxPos  = IioGlobalData->SetupData.McastIndexPosition;

  //
  // Allocate memory for Mcast test if not already allocated
  //
  if (IioGlobalData->IioVar.IioOutData.McastRsvdMemory != 0) {
    //
    // Already allocated, get the address
    //
    McastBaseAddress = IioGlobalData->IioVar.IioOutData.McastRsvdMemory;

  } else {
    //
    // Allocate memory for Mcast as per user config
    //
    if (IioGlobalData->SetupData.McastBaseAddrRegion == MCAST_MEM_AUTO) {
      //
      // Any location
      //
      McastMemRangeType = AllocateAnyPages;
      McastBaseAddress  = 0;

    } else if (IioGlobalData->SetupData.McastBaseAddrRegion == MCAST_MEM_BELOW_4G) {
      //
      // Force below 4G
      //
      McastMemRangeType = AllocateMaxAddress;
      McastBaseAddress  = 0xFFFFFFFF;          // Below 4G

    } else {

      return MCAST_ERR_MEM_ALLOC_FAIL;
    }

    //
    // Calculate size required as per number of groups (McastNumGroup) and size as per (McastIndexPosition)
    //
    McastRsvdMemSize = McastNumGrp * (UINTN)( 1 << McastIndxPos );

    //
    // Now allocate memory
    //
    Status = gBS->AllocatePages (
                     McastMemRangeType,
                     EfiBootServicesData,
                     EFI_SIZE_TO_PAGES( (UINTN)McastRsvdMemSize ),
                     &McastBaseAddress
                     );

    if (McastBaseAddress == 0) {
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\nMCAST: Memory Alloc Error\n");
      return MCAST_ERR_MEM_ALLOC_FAIL;

    } else {
      IioGlobalData->IioVar.IioOutData.McastRsvdMemory = McastBaseAddress;
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\nMCAST: Allocated Membase at 0x%x, Size 0x%x\n", IioGlobalData->IioVar.IioOutData.McastRsvdMemory, McastRsvdMemSize);
    }

  }

  //
  // Set MC Base Address and MC Index Position
  //


  McastBaseN0.Bits.mc_index_position = McastIndxPos;
  McastBaseN0.Bits.mc_base_address = (UINT32) (McastBaseAddress>>12);
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MCAST_BASE_N0_IIO_PCIE_REG, McastBaseN0.Data);

  McastBaseN1.Bits.mc_base_address = (UINT32) (McastBaseAddress >> 32);
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MCAST_BASE_N1_IIO_PCIE_REG, McastBaseN1.Data);

  //
  // Clear MCAST block all
  //

  McastBlkAllN0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MCAST_BLK_ALL_N0_IIO_PCIE_REG);
  McastBlkAllN0.Bits.mc_blk_all = 0;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MCAST_BLK_ALL_N0_IIO_PCIE_REG, McastBlkAllN0.Data);

  //
  //  MCAST Receive should be set for NTB
  //
  if ((IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == NTB_PORT_DEF_NTB_NTB ) ||
      (IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == NTB_PORT_DEF_NTB_RP )) {
    Data32 = 0;

    for( i = 0 ; i < McastNumGrp; i++ ) {
      Data32 |= 1 << i;
    }
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MCAST_RCV_N0_IIO_PCIE_REG, Data32);
  }

  //
  // Set MC Number of groups MC_NUM_GROUP and MC_ENABLE
  //

  McastCtrl.Bits.mc_num_group = (McastNumGrp == 0) ? (McastNumGrp) : (McastNumGrp -1);
  McastCtrl.Bits.mc_enable = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, MCAST_CTRL_IIO_PCIE_REG, McastCtrl.Data);

  if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) && (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX)) {
    //
    // 4929386: Cloned From SKX Si Bug Eco: OTC incorrectly logs error for a legal multicast region hit
    //
    Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];;
    TcErrCtl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRCTL_IIO_RAS_REG);
    if (TcErrCtl.Bits.otc_mabort != 0) {
      TcErrCtl.Bits.otc_mabort = 0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRCTL_IIO_RAS_REG, TcErrCtl.Data);
    }

    OtcErrSev.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCERRSEV_IIO_RAS_REG);
    if (OtcErrSev.Bits.otc_mabort != 1) {
      OtcErrSev.Bits.otc_mabort = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCERRSEV_IIO_RAS_REG, OtcErrSev.Data);
    }

    TcErrSt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRST_IIO_RAS_REG);
    if(TcErrSt.Bits.otc_mabort == 1) {
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRST_IIO_RAS_REG, TcErrSt.Data);  
    }    
  }

  //
  // Dump some of the mcast registers
  //
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "MCAST Socket:%x Port:%x Bus:%x\n", IioIndex, PortIndex, IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex]);
  for ( i = 0; i < 10; i++ ) {
    Data32 = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MCAST_CAP_HDR_IIO_PCIE_REG+i*4);

    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "MCAST Reg %x: 0x%x\n", i*4, Data32 );
  }

  return    MCAST_RET_SUCCESS;
}

/**

   This is the API for the EFI_IIO_SYSTEM_PROTOCOL interface which returns the details
   of the uplink port if connected to the platform

   @param PortDescriptor - Pointer to PORT_DESCRIPTOR data structure which contains the PCI address (Bus, Device, function)
   @param PortStatus     - Pointer to boolean which if TRUE indicates Uplink port is enabled else FALSE for disabled
   @param PortAttrib     - Pointer to PORT_ATTRIB data structure which gives the uplink ports width and speed

   @retval EFI_UNSUPPORTED - if system does not support the uplink port,
           EFI_DEVICE_ERROR - if internal error encountered,
           EFI_SUCCESS - for returning valid data

**/
EFI_STATUS
IioGetCpuUplinkPort (
    UINT8             IioIndex,
    PORT_DESCRIPTOR   *PortDescriptor,    //Bus, Device, function
    BOOLEAN           *PortStatus,        //TRUE if enabled else disabled
    PORT_ATTRIB       *PortAttrib         //width and speed
)
{
  EFI_STATUS Status;
  UINT8   PortIndex;
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  EFI_IIO_SYSTEM_PROTOCOL       *IioSystemProtocol = NULL;
  IIO_GLOBALS                   *IioGlobalData = NULL;
  VID_IIO_PCIE_STRUCT           VidIioPcie;
  LNKSTS_IIO_PCIE_STRUCT        LnkStsIioPcie;
  LNKCON_IIO_PCIE_STRUCT        LnkConIioPcie;

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  if (EFI_ERROR(Status))
    return Status;

  IioGlobalData = IioSystemProtocol->IioGlobalData;
  OemIioUplinkPortDetails(IioGlobalData, IioIndex, &PortIndex, &Bus, &Dev, &Func);
  if(IioIndex == 0xFF && PortIndex == 0xFF)
    return EFI_UNSUPPORTED;             //not uplink port provided in the platform

  //
  // Assume Legacy socket always present (where uplink port is)
  //
  if (PortIndex >= NUMBER_PORTS_PER_SOCKET)
    return EFI_DEVICE_ERROR;

  PortDescriptor->Bus       = Bus;
  PortDescriptor->Device    = Dev;
  PortDescriptor->Function  = Func;

  VidIioPcie.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, VID_IIO_PCIE_REG);

  if(VidIioPcie.Bits.vendor_identification_number == 0xFFFF)        //port not found! could be hidden
    return EFI_DEVICE_ERROR;

  LnkStsIioPcie.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);

  PortAttrib->PortWidth = (UINT8)LnkStsIioPcie.Bits.negotiated_link_width;   //width could be 1, 2, 4, 8, 16
  PortAttrib->PortSpeed = (UINT8)LnkStsIioPcie.Bits.current_link_speed;           //speed : 1=Gen1, 2=Gen2, 3=Gen3

  LnkConIioPcie.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG);

  *PortStatus = (LnkConIioPcie.Bits.link_disable) ? FALSE : TRUE;

  return EFI_SUCCESS;
}


/**

  Call back routine used to do all IIO post PCie port configuration, initialization

  @param IioGlobalData  - Pointer to IIO_GLOBALS

  @retval None
**/
EFI_STATUS
IioPostLateInitialize (
  IIO_GLOBALS  *IioGlobalData
  )
{
  EFI_STATUS                    Status;
  EFI_PCI_CALLBACK_PROTOCOL     *PciCallBackProtocol;
  UINT8                         IioIndex, PortIndex;

  //
  // Configure Multicast if enabled
  //
  if ( IioGlobalData->SetupData.MultiCastEnable == TRUE ) {
    for ( IioIndex = 0 ; IioIndex < MaxIIO ; IioIndex++ ) {
      if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex])
         continue;

      for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
        if( IioVData_ConfigurePciePort(IioGlobalData,IioIndex, PortIndex) == FALSE)  //check whether the PCIe port can be configured
           continue;
        if(PortIndex == 0){
           // any post PCIe init DMI settings are done below
          DmiPostInit(IioGlobalData, IioIndex);
         }
          EnableMultiCast (IioGlobalData, IioIndex, PortIndex);
        }
      }
  }



  // now set Pcie Features in IIO Root Ports and End Points as need it.
  PcieLateCommonLibInit(IioGlobalData);

  DumpIioPciePortPcieLinkStatus(IioGlobalData);

  Status = gBS->LocateProtocol (
                   &gEfiPciCallbackProtocolGuid,
                   NULL,
                   &PciCallBackProtocol
                   );
  ASSERT_EFI_ERROR(Status);

  Status = PciCallBackProtocol->RegisterPciCallback(
                    PciCallBackProtocol,
                    IioInitAfterResources,
                    EfiPciEnumerationResourceAssigned
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
