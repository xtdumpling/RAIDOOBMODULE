//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2017, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  File History
//
//  Rev. 1.04
//    Bug Fix : Fixed PERR when hot plug NVMe drive.
//    Reason  : Mask Link bandwidth change but not work. So if it is hot removal, do not log error to BMC/WHEA/GPNV
//    Auditor : Leon Xu
//    Date    : Aug/23/2017
//
//  Rev. 1.03
//    Bug Fix : Fixed "Correctable, Non-Fatal and Fatal" error reporting
//              flags are disabled on "Infiniband controller: Mellanox 
//              Technologies MT27700 Family [ConnectX-4]" when "PCI PERR/SERR Support"
//              is enabled. (Reported by HPE.)
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jul/11/2017
//
//  Rev. 1.02
//      Bug Fixed:  Mask out Surprised Down Error
//      Reason:     Make system no hang when removing NVMe
//      Auditor:    Donald Han
//      Date:       Jan/18/2017
//
//  Rev. 1.01
//   Bug Fixed:  Fixed that PERR could not work.. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Nov/11/2016
//  
//  Rev. 1.00
//      Bug Fixed:  Fix cannot log SERR/PERR. 
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Sep/09/2016
//
//**********************************************************************//
/** @file
  Implementation of the PCI Express Error Handler.

  Copyright (c) 2009-2016 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
#include "Token.h" // SMCPKG_SUPPORT
#include <Setup/IioUniversalData.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/WheaDefs.h>
#include <Cpu/CpuBaseLib.h>
#include <Protocol/ErrorHandlingProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <PciExpress.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/mpsyncdatalib.h>
#include "CommonErrorHandlerDefs.h"
#include "PcieErrorHandler.h"
#include "ErrorRecords.h"
#include "UncoreCommonIncludes.h"
#include "RcRegs.h"
#include <Fpga.h>
#include <Guid/FpgaSocketVariable.h>


STATIC EFI_STATUS
PcieGetAerErrors (
  IN OUT  PCIE_ERROR_SECTION *PcieErrorRec,
  IN      UINT8              Socket,
  IN      UINT8              Bus,
  IN      UINT8              Device,
  IN      UINT8              Function
);

STATIC EFI_STATUS
PcieSuppressUnsupportReqest (
  IN      UINT8              Socket,
  IN      UINT8              Bus,
  IN      UINT8              Device,
  IN      UINT8              Function,
  IN      BOOLEAN            Mask,
  IN      BOOLEAN            Clear
);


// Module data
EFI_ERROR_HANDLING_PROTOCOL           *mErrorHandlingProtocol;
EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
REPORTER_PROTOCOL                      mReporter;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
EFI_RAS_SYSTEM_TOPOLOGY               *mRasTopology;
SYSTEM_RAS_SETUP                      *mRasSetup;
SYSTEM_RAS_CAPABILITY                 *mRasCapability;
UINT32                                mSeverity = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;
EFI_SMM_BASE2_PROTOCOL                *mSmmBase2;
EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccess = NULL;
EFI_SMM_GPI_DISPATCH2_PROTOCOL        *mGpiDispatch;

BOOLEAN   mAllowClearingOfUEStatus          = TRUE;    // CV requirement
extern BOOLEAN   mNmiOnSerr;
extern BOOLEAN   mNmiOnPerr;
BOOLEAN   mPcieNonFatalErrDetected          = FALSE;
BOOLEAN   mPcieFatalErrDetected             = FALSE;
BOOLEAN   mAllowClearingOfPcieDeviceStatus  = TRUE;
BOOLEAN   mNumCorrectablePcieErrs           = 0;

////////////////////////////////////// COMMON ROUTINES /////////////////////////////////////////

/**
    This function obtains the PCI configuration space register offset of the specified device's
    PCI Express Capabilities register set.  Note: this is not the offset to the Advanced
    Error Reporting register set.

  @param[in] Socket      Device's socket number
  @param[in] Bus         Device's bus number.
  @param[in] Device      Device's device number.
  @param[in] Function    Device's function number.
  @param[in] Cap         Desired Capability structure

  @retval  CapabilityOffset.
**/
UINT8 
PciErrLibGetCapabilitiesOffset (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function,
  IN      UINT8   Cap
  )
{
  UINT16  PciPrimaryStatus;
  UINT8   CapabilityOffset = 0;
  UINT16  data16;
  UINT8   ItemCnt = 0;
  PciPrimaryStatus = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY) {
    CapabilityOffset = MmioRead8(MmPciAddress(Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_PTR));

    while(ItemCnt < MAX_CAPABILITIES) { // maximum possible number of items in list
      // format check, normal end of list is next offset = 0
      //   PCI 3.0 section 6.7 Capability List
      // "Each capability must be DWORD aligned. The bottom two bits of all pointers
      // (including the initial pointer at 34h) are reserved and must be implemented as 00b
      // although software must mask them to allow for future uses of these bits."

      if ((CapabilityOffset & DWORD_MSK) || (CapabilityOffset < CAP_OFFSET_BASE)) { // dword alignment; lower limit
        CapabilityOffset = 0;  // bad format or normal end of list, set not found
        break;          // terminate search
      }
      // CapOffset &= ~DWORD_MSK;  // not needed if b[1::0] must = 0
      // offset is good, get capabilities ID and next offset
      data16 = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, CapabilityOffset));
      if ((UINT8)(data16 & CAPABILITIES_MSK) == Cap) {
        break;  // requested ID found
      }

      // next item
      CapabilityOffset = (UINT8)(data16 >> 8);   // b[15::8] = next ptr
      ItemCnt++;
    }

  }
  return CapabilityOffset;
}

/**
  This function clears the specified device's primary and secondary (if appropriate) 
  legacy PCI status registers.
  
  @param[in] Socket      Device's socket number
  @param[in] Bus         Device's bus number.
  @param[in] Device      Device's device number.
  @param[in] Function    Device's function number.

  @retval  None.
**/
VOID
PciErrLibClearLegacyPciDeviceStatus (
  IN      UINT8	  Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16    StatusReg16;

  StatusReg16 = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));
  MmioWrite16(MmPciAddress(Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET), StatusReg16);

  if (PciErrLibIsPciBridgeDevice (Socket, Bus, Device, Function)) {
    StatusReg16 = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_BRIDGE_STATUS_REGISTER_OFFSET));

    MmioWrite16(MmPciAddress(Socket, Bus, Device, Function, PCI_BRIDGE_STATUS_REGISTER_OFFSET), StatusReg16);
  }
}

/**
  This function checks if a PCI-to-PCI bridge exists at the specified PCI address.
  
  @param[in] Socket      Device's socket number
  @param[in] Bus         Device's bus number.
  @param[in] Device      Device's device number.
  @param[in] Function    Device's function number.

  @retval  BOOLEAN       TRUE  - P2P present at the specified address.
                         FALSE - P2P not present at the specified address.
**/                 
BOOLEAN
PciErrLibIsPciBridgeDevice (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT32  Data32;

  Data32 = MmioRead32(MmPciAddress(Socket, Bus, Device, Function, PCI_REVISION_ID_OFFSET));
  Data32 &= 0xffffff00;
  if ((Data32 == EFI_PCI_BRIDGE_DEV_CLASS_CODE) || (Data32 == EFI_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE)) {
    return TRUE;
  }
  return FALSE;
}

/**
  This function obtains the extended PCI configuration space register offset for a
  specified Extended Capability for the specified PCI device.
  
  @param[in] Socket               Device's socket number
  @param[in] Bus                  Device's bus number.
  @param[in] Device               Device's device number.
  @param[in] Function             Device's function number.
  @param[in] ExtCapabilityId      ID of the desired Extended Capability.
  @param[in] VendoreSpecificId    vendor ID that is at offset ExtCapabilityoffset+4.

  @retval    Value                0         PCIe extended capability ID not found
                                  non-zero  Extended PCI configuration space offset of the specified Ext Capability block
**/  

UINT16 
PciErrLibGetExtendedCapabilitiesOffset (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function,
  IN      UINT16  ExtCapabilityId,
  IN      UINT16  VendorSpecificId
  )
{

  UINT16 ExCapOffset = 0;
  UINT16 ExItemCnt = 0;
  UINT32 data32;

  //  DMI AER Extended capabitity is in VSEC item at offset 0x148
  if (Bus == DMI_BUS_NUM && Device == DMI_DEV_NUM && ExtCapabilityId == PCIE_EXT_CAP_HEADER_AERC) {
    return 0x148;
  }

  if (PciErrLibGetCapabilitiesOffset(Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP)) {
    ExCapOffset = PCIE_ENHANCED_CAPABILITY_PTR;
    while (ExItemCnt < MAX_EX_CAPABILITIES) {  // maximum possible number of items
      // format check, normal end of list is 0
      //   PCIe 3.1 section 7.9.3 PCI Express Extended Capability Header
      // "For Extended Capabilities implemented in Configuration Space,
      // this offset is relative to the beginning of PCI compatible Configuration Space
      // and thus must always be either 000h (for terminating list of Capabilities)
      // or greater than 0FFh.
      // The bottom 2 bits of this offset are Reserved and must be implemented as 00b
      // although software must mask them to allow for future uses of these bits."
      if ((ExCapOffset & DWORD_MSK) || (ExCapOffset < EX_CAP_OFFSET_BASE)) {  // dword alignment; lower limit
        ExCapOffset = 0;  // if bad format or normal end of list, set not found
        break;            //  terminate search
      }

      // offset is good, get capabilities ID and next offset
      // capabilities ID check
      //data32 = PciExpressRead32(PCI_PCIE_ADDR(Bus, Device, Function, ExCapOffset));   // get ID
      data32 = MmioRead32(MmPciAddress(Socket, Bus, Device, Function, ExCapOffset));
      if ((data32 & EXCAPABILITIES_MSK) == ExtCapabilityId) {
        if (ExtCapabilityId != PCIE_EXT_CAP_HEADER_VSEC) {
          break;  // requested Extended ID found
        } else {  // check VSEC ID
         // if (PciExpressRead16(PCI_PCIE_ADDR(Bus, Device, Function, ExCapOffset + PCIE_EXT_VENDOR_SPECIFIC_HEADER_OFFSET)) == VendorSpecificId) {
          if ( MmioRead16(MmPciAddress(Socket, Bus, Device, Function, ExCapOffset + PCIE_EXT_VENDOR_SPECIFIC_HEADER_OFFSET))== VendorSpecificId) {
        	break;  // requested VSCE ID found
          }
        }
      }
      // next item
      ExCapOffset = (UINT16)(data32 >> 20);  // b[31::20] = next ptr
      ExItemCnt++;
    }
  }
  return ExCapOffset;

}

/**
  This function clears the legacy and standard PCI Express status registers for the
  specified PCI Express device.
  
  @param[in] Socket         Device's socket number
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    None
**/
VOID
PciErrLibClearPcieDeviceStatus (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8     CapabilitiesOffset;
  UINT16    StatusReg16;

  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);

  StatusReg16 = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)));

  if (mAllowClearingOfUEStatus == FALSE)  {
    StatusReg16 &= ~(B_PCIE_DEVICE_STATUS_NF_ERR);
  }
  
  MmioWrite16(MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)), StatusReg16);
  PciErrLibClearLegacyPciDeviceStatus (Socket, Bus, Device, Function);
}

/**
  This function will verify whether a port is PCIe root port
  
  @param[in] Socket         Device's socket number
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    BOOLEAN        True   if PCIe is root port.
                            False  if PCIe is not root port.
**/
BOOLEAN
PCIeIsRootPort(
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  BOOLEAN   RootPort;
  UINT8     PciHeaderType;

  RootPort = FALSE;

  PciHeaderType = MmioRead8(MmPciAddress(Socket, Bus, Device, Function, PCI_HEADER_TYPE_OFFSET));
  
  // In DMI mode, default is 00h indicating a conventional type 00h PCI header.
  // In PCIe mode, the default is 01h, corresponding to Type 1 for a PCIe root port.
  if ((PciHeaderType & HEADER_LAYOUT_CODE) == 1) {
    RootPort = TRUE;
  }

  return RootPort;
}

/**
  This function checks if a PCI or PCIe device exists at the specified PCI address.
  
  @param[in] Socket         Device's socket number
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    BOOLEAN        True   Device present at the specified address.
                            False  Device not present at the specified address.
**/
BOOLEAN
PciErrLibIsDevicePresent (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16    Data16;

  // Check if a device is present at this location...

  Data16 = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_VENDOR_ID_OFFSET));
  if (Data16 == 0xffff) {
    return FALSE;
  }
  return TRUE;
}

/**
  Hide or unhide FPGA device.

  @param[in]    Hide    To hide or unhide FPGA devices.
  @retval EFI_SUCCESS   The interrupt was handled successfully.
**/
EFI_STATUS
EFIAPI
HideFpgaDevices (
  BOOLEAN  Hide
  )
{
  CONST   UINT32                FpgaDevBits = (BIT5 | BIT3 | BIT2 | BIT1 | BIT0);
  UINT8                         Socket;
  UINT32                        PcuDevhide;
  UINT32                        UboxDevhide;

  DEBUG ((DEBUG_INFO, "HideFpgaDevices() enter.\n"));

  //
  // Loop thru all IIO stacks of all sockets that are present
  //
  DEBUG ((DEBUG_INFO, "FpgaSktActive = 0x%X.\n", mRasTopology->SystemInfo.FpgaSktActive));
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!(mRasTopology->SystemInfo.FpgaSktActive & (1 << Socket))) {
      continue;
    }

    UboxDevhide = mCpuCsrAccess->ReadCpuCsr (Socket, 0, DEVHIDE0_4_UBOX_MISC_REG);
    UboxDevhide = Hide ? UboxDevhide | FpgaDevBits : UboxDevhide & ~FpgaDevBits;  //UBOX Hide B(4) D0/1/2/3, F0 and Hide MCP0 VTD function @B(4) D5, F0
    mCpuCsrAccess->WriteCpuCsr (Socket, 0, DEVHIDE0_4_UBOX_MISC_REG, UboxDevhide);
    DEBUG ((DEBUG_INFO, "UBOX Data = 0x%X.\n", mCpuCsrAccess->ReadCpuCsr (Socket, 0, DEVHIDE0_4_UBOX_MISC_REG)));

    PcuDevhide = mCpuCsrAccess->ReadCpuCsr (Socket, 0, DEVHIDE_B4F0_PCU_FUN6_REG);
    PcuDevhide =  Hide ? PcuDevhide | FpgaDevBits : PcuDevhide & ~FpgaDevBits; //PCU Hide B(4) D0/1/2/3, F0 and Hide MCP0 VTD function @B(4) D5, F0
    mCpuCsrAccess->WriteCpuCsr (Socket, 0, DEVHIDE_B4F0_PCU_FUN6_REG, PcuDevhide);
    DEBUG ((DEBUG_INFO, "PCU Data = 0x%X.\n", mCpuCsrAccess->ReadCpuCsr (Socket, 0, DEVHIDE_B4F0_PCU_FUN6_REG)));
  }

  return EFI_SUCCESS;
}

BOOLEAN
PciErrLibIsDmiPort (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  return (Socket == 0 && Bus == DMI_BUS_NUM && Device == DMI_DEV_NUM && Function == DMI_FUNC_NUM);
}

BOOLEAN
IsHotRemoveUnderL1 (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16    CapOffset;
  UINT16    Sltsts;
  UINT16    Sltcon;
  UINT32    Sltcap;
  UINT32    Uncerrsts;
  BOOLEAN   HotPlugCap;
  BOOLEAN   PCC;
  BOOLEAN   PDS;

  // Get the capabilities offset
  CapOffset = PciErrLibGetExtendedCapabilitiesOffset (Socket, Bus,Device, Function, PCIE_EXT_CAP_HEADER_AERC, 0xff);

  Uncerrsts = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, CapOffset + PCIE_AER_UNCERRSTS_OFFSET));
  RASDEBUG((DEBUG_ERROR, "IsHotRemoveUnderL1: UNCERRSTS: 0x%x \n", Uncerrsts));

  //Check if it is CTO case.
  if (! (Uncerrsts & BIT14)) {
	return FALSE;
  }

  Sltcap = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, 0xA4));
  HotPlugCap = ( (Sltcap & BIT6) != 0);

  if (!HotPlugCap) {
	return FALSE;
  }


  Sltsts = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, 0xAA));
  Sltcon = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, 0xA8));
  RASDEBUG((DEBUG_ERROR, "IsHotRemoveUnderL1: Sltsts 0x%x, Sltcon 0x%x \n", Sltsts, Sltcon));

  PCC  = ( (Sltcon & BIT10) != 0);
  PDS  = ( (Sltsts & BIT6) != 0);

  if (PCC) {
	RASDEBUG((DEBUG_ERROR, "IsHotRemoveUnderL1: Is hot removal case (PCC = 1) \n"));
	return TRUE;
  } else if (!PDS) {
	RASDEBUG((DEBUG_ERROR, "IsHotRemoveUnderL1: Is hot removal case (PCC = 0) \n"));
	return TRUE;
  } else {
	return FALSE;
  }
}


////////////////////////////////////// ERROR HANDLING ROUTINES /////////////////////////////////////////
/**
  This function is the entry for processing PCH and IIO errors.
  
  @retval    None     
**/
VOID
ProcessErrors (
  VOID
 )
{
  if (mRasTopology->SystemInfo.FpgaSktActive) {
    HideFpgaDevices(FALSE);
  }

  ProcessIioErrors();

  if (mRasTopology->SystemInfo.FpgaSktActive) {
    HideFpgaDevices(TRUE);
  }

  ProcessPchErrors();

}

/**
  This function checks for, and handles, errors on standard PCI devices.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    BOOLEAN        TRUE     - An error was detected.
                            FALSE    - An error was not detected.
--*/
BOOLEAN
PciErrLibPciDeviceErrorHandler (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16    PrimaryStatus;
  UINT8     PciErrorType;
  UINT16    ErrorMap;
  BOOLEAN   ErrorFound;
  
  ErrorFound = FALSE;
  
  ErrorMap = EFI_PCI_STATUS_DETECTED_PARITY_ERROR | 
              EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR |
              EFI_PCI_STATUS_RECEIVED_TARGET_ABORT |
              EFI_PCI_STATUS_DATA_PARITY_ERROR_DETECTED;

  // The standard behavior is outlined below:
  // 1. #SERR generation is controlled thru CMD.SEE and CMD.PERE.
  // 2. #SERR generation can also occur if the device is capable of being a bus master 
  //    on the primary bus and can detect parity errors. Optionally, #SERR can also be
  //    triggered by setting a chipset-specific policy for bridges and bus masters for 
  //    target and master aborts...
  //

  //PrimaryStatus = PciExpressRead16 (PCI_PCIE_ADDR (Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));
  PrimaryStatus = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (PrimaryStatus & ErrorMap) {
    ErrorFound = TRUE;
    // Error signaled on the primary bus...
    if (PrimaryStatus & EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
      PciErrorType = SERROR_TYPE;
    } else {
      PciErrorType = PERROR_TYPE;
    }

    // Report the error...
    PcieLogReportError (Socket, Bus, Device, Function, PciErrorType);
    // Clear error status
    PciErrLibClearLegacyPciDeviceStatus (0, Bus, Device, Function);
  }

  return ErrorFound;
}

/**
  This function checks for errors on standard PCI devices.
    
  @param[in] Socket         Device's socket number
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    BOOLEAN        TRUE     - An error was detected.
                            FALSE    - An error was not detected.
--*/
BOOLEAN
PciErrLibIsPciDeviceError (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16    PrimaryStatus;
  UINT16    ErrorMap;
  BOOLEAN   ErrorFound;
  
  ErrorFound = FALSE;
  ErrorMap = EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR |  
              EFI_PCI_STATUS_DETECTED_PARITY_ERROR |
              EFI_PCI_STATUS_DATA_PARITY_ERROR_DETECTED |
              EFI_PCI_STATUS_RECEIVED_TARGET_ABORT;                                   

  PrimaryStatus = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));
  if (PrimaryStatus & ErrorMap) {
    ErrorFound = TRUE;
  }

  return ErrorFound;
}

/**
  This function checks the PCIe device status and the correctable error registers to see 
  if a PCIe Correctable error has occurred.

  @param[in] Socket         Device's socket number
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    BOOLEAN        TRUE  - A PCIe Correctable error was detected.
                            FALSE - A PCIe Correctable error was not detected.
--*/
BOOLEAN
PciErrLibIsCorrectablePcieDeviceError (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8     CapabilitiesOffset;
  UINT16    DeviceStatus;
  UINT16    DeviceControl;
  BOOLEAN   ErrorFound;
  PCIE_ERROR_SECTION PcieErrorRec;
  
  ErrorFound = FALSE;
  
  // Get the capabilities offset
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);

  // Check if the device status indicates that a CORRECTABLE error occurred

  DeviceStatus = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)));
  DeviceControl = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVCTL_OFFSET (Socket, Bus, Device, Function)));


  if (DeviceStatus & DeviceControl & B_PCIE_DEVICE_CONTROL_COR_ERR_EN) {

    DEBUG ((DEBUG_INFO, "Bus:0x%x, Device:0x%x, Func:0x%x has DeviceStatus:0x%x, DevControl:0x%x\n", Bus, Device, Function, DeviceStatus, DeviceControl));

    if (DeviceStatus & DeviceControl & B_PCIE_DEVICE_CONTROL_UNSUP_REQ_EN) {
      ErrorFound = TRUE;
    }
    else {
      PcieGetAerErrors (&PcieErrorRec, Socket, Bus, Device, Function);
      RASDEBUG ((DEBUG_INFO, "(PcieErrorRec.Corerrsts =  0x%x, mRasSetup->Pcie.PcieAerCorrErrBitMap = 0x%x\n", PcieErrorRec.Corerrsts, mRasSetup->Pcie.PcieAerCorrErrBitMap));
      if (PcieErrorRec.Corerrsts & mRasSetup->Pcie.PcieAerCorrErrBitMap) {
        ErrorFound = TRUE;
      }
    }
  }
  return ErrorFound;
}

/**
  This function handles PCIe Uncorrectable (Fatal and Non-Fatal) errors.
  An error is reported and the PCIe error detection status is updated.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibHandleUncorrectablePcieDeviceError (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8     CapabilitiesOffset;
  UINT16    PCIeCapabilities;
  UINT8     DeviceType;
  BOOLEAN   PCIeToPCIX;
  UINT16    SecondaryStatus;
  UINT16    ErrorMap;
  UINT8     PciErrorType;
  UINT16    DeviceStatus;
  
  // Get the capabilities offset
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);

  // If PCIe-to-PCI-X bridge, check secondary status register for a PCI error
  PCIeCapabilities = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_CAPABILITY_OFFSET));

  DeviceType = (UINT8)(PCIeCapabilities >> 4) & 0x0F;
  PCIeToPCIX = (DeviceType == 0x07) ? TRUE : FALSE;
  if (PCIeToPCIX) {
    ErrorMap = EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR      | EFI_PCI_STATUS_DETECTED_PARITY_ERROR |
               EFI_PCI_STATUS_DATA_PARITY_ERROR_DETECTED | EFI_PCI_STATUS_RECEIVED_TARGET_ABORT;

    SecondaryStatus = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_BRIDGE_STATUS_REGISTER_OFFSET));
    if ((SecondaryStatus & ErrorMap) != 0) {

      // Error signaled on the secondary bus
      if (SecondaryStatus & EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
        PciErrorType = SERROR_TYPE;
      } else {
        PciErrorType = PERROR_TYPE;
      }

      // Report the error
      PcieLogReportError (Socket, Bus, Device, Function, PciErrorType);

      // Clear the error bits
      MmioWrite16(MmPciAddress(Socket, Bus, Device, Function, PCI_BRIDGE_STATUS_REGISTER_OFFSET), ErrorMap);
      return;
    }
  }

  // Not a PCI error, report a FATAL PCIe error
  // Check if the device status indicates that a FATAL error or uncorrected error occurred
  DeviceStatus = MmioRead16(MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)));

  if ((DeviceStatus & B_PCIE_DEVICE_STATUS_FAT_ERR)) {

    // FATAL error reported
    if (mPciXPUnsupportedRequest != 01){
      PcieLogReportError (Socket, Bus, Device, Function, FATAL_ERROR_TYPE);
    }
  } else {
    if (mPciXPUnsupportedRequest != 01){
	    PcieLogReportError (Socket, Bus, Device, Function, NONFATAL_ERROR_TYPE);
    }
  }
}

/**
  This function handles PCIe Correctable errors.
  An error is reported if the correctable error threshold has not been reached.
  If the threshold has been reached, correctable errors are masked in the device.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibHandleCorrectablePcieDeviceError (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8   CapabilitiesOffset;
  UINT16  PcieCapabilities;
  UINT16  DeviceControl;
  UINT16  RootControl;

  // Check if the correctable-error reporting limit has already been reached.
  if (mNumCorrectablePcieErrs < PCI_ERROR_COUNT_MAX_THRESHOLD) {
    // Report the correctable error.
    PcieLogReportError (Socket, Bus, Device, Function, COR_ERROR_TYPE);
    mNumCorrectablePcieErrs++;
  }

  // Error limit has been reached.  Mask correctable errors in the device.
  if (mNumCorrectablePcieErrs >= PCI_ERROR_COUNT_MAX_THRESHOLD) {
    RASDEBUG ((DEBUG_INFO, "Pcie CE reached threshold,disabling error escalation from this device\n"));

    // Error limit has been reached.  Mask correctable errors in the device.
    CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
    DeviceControl = MmioRead16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVCTL_OFFSET (Socket, Bus, Device, Function)));
    DeviceControl &= ~(B_PCIE_DEVICE_CONTROL_COR_ERR_EN);
    MmioWrite16(MmPciAddress(Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP), DeviceControl);

    // Update the Root Control register if this is a root port.
    PcieCapabilities = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_CAPABILITY_OFFSET));
    if ((PcieCapabilities & 0xF0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
      RootControl = MmioRead16 (MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_ROOT_CONTROL_OFFSET));
      RootControl &= ~(B_PCIE_ROOT_CONTROL_COR_ERR_EN);
      MmioWrite16(MmPciAddress(Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_ROOT_CONTROL_OFFSET), RootControl);
    }
  }
}

/**
  This function determines if there is a device reporting an error on the specified 
  P2P bridge's secondary bus or on a subordinate bus.

  @oaram[in] Socket         P2P bridge's socket number.
  @param[in] Bus            P2P bridge's bus number.
  @param[in] Device         P2P bridge's device number.
  @param[in] Function       P2P bridge's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/
BOOLEAN
PciErrLibIsSubordinateDeviceError (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8     SecondaryBusNumber;
  UINT8     SubordinateBusNumber;

  if (PciErrLibIsPciBridgeDevice (Socket, Bus, Device, Function)) {
    SecondaryBusNumber = MmioRead8 (MmPciAddress (Socket,
    		                         Bus,
    		                         Device,
    		                         Function,
    		                         PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
    		                         ));

    SubordinateBusNumber = MmioRead8 (MmPciAddress (Socket,
    		                            Bus,
    		                            Device,
    		                            Function,
    		                            PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET
    		                            ));

    if (SecondaryBusNumber && SubordinateBusNumber) {
      for (Bus = SecondaryBusNumber; Bus <= SubordinateBusNumber; Bus++) {
        for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
          for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {
            if (PciErrLibIsDevicePresent (Socket, Bus, Device, Function)) {
              if (PciErrLibIsPcieDevice (Socket, Bus, Device, Function)) {
                if (PciErrLibIsFatalPcieDeviceError (Socket, Bus, Device, Function)      ||
                     PciErrLibIsNonfatalPcieDeviceError (Socket, Bus, Device, Function)   ||
                     PciErrLibIsCorrectablePcieDeviceError (Socket, Bus, Device, Function)) {
                  return TRUE;
                }
              } else {
                if (PciErrLibIsPciDeviceError (Socket, Bus, Device, Function)) {
                  return TRUE;
                }
              }
            }
          }
        }
      }
    }
  }
  return FALSE;
}

/**
  This function checks the SERR and PCIe device status to see if a Uncorrectable Non-Fatal
  PCIe error has occurred.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    BOOLEAN        TRUE   - A PCIe Uncorrectable Non-Fatal error was detected.
                            FALSE  - A PCIe Uncorrectable Non-Fatal error was not detected.
--*/
BOOLEAN
PciErrLibIsNonfatalPcieDeviceError (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  ) 
{
  UINT8     CapabilitiesOffset;
  UINT16    DeviceStatus;
  UINT16    DeviceControl;
  UINT16    PrimaryStatus;
  BOOLEAN   ErrorFound;

  ErrorFound = FALSE;
  
  // Get the capabilities offset
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);

  // Check if the device status indicates that a NON-FATAL error occurred
  DeviceStatus = MmioRead16(MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)));
  RASDEBUG ((DEBUG_INFO, "\t PciErrLibIsNonfatalPcieDeviceError: DeviceStatus: 0x%x, CapabilitiesOffset: 0%x, Offset: 0x%x\n", DeviceStatus, CapabilitiesOffset, GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)));

  if (DeviceStatus & B_PCIE_DEVICE_STATUS_NF_ERR) {
    // NON-FATAL error status set, now check if NON-FATAL error reporting is enabled.
    // NON-FATAL error reporting is enabled if SERR is enabled or NON-FATAL reporting is enabled
    // in the PCIe device control register.
	if (MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_COMMAND_OFFSET)) & EFI_PCI_COMMAND_SERR) {
		PrimaryStatus = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));
		RASDEBUG ((DEBUG_INFO, "\t PciErrLibIsNonfatalPcieDeviceError: PrimaryStatus: 0x%x\n", PrimaryStatus));

      // NON-FATAL status and NON-FATAL reporting enabled.
      // The PCIe status could be set even if the error was masked.
      // The PCIe status could be set even if there are not bits set in the AER status register.
      // Check SERR status to see if a FATAL/NON-FATAL error message was sent.
      if (PrimaryStatus & EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
        // SERR was signaled (FATAL or Non-Fatal error occurred)
        ErrorFound = TRUE;
      }
    }
    
    // The error could be an Unsupported Request error that occurred during PCI bus probe.
    // Even if we incorrectly ignore this error we will NMI (if appropriate) 
    // because of the Uncorrectable error check in PciErrLibIsErrOnPcieRootPort().
    if ((DeviceStatus & B_PCIE_DEVICE_STATUS_UNSUP_REQ) == 0) {
      // Not an unsupported request, so check if PCIe reporting is enabled
      DeviceControl = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVCTL_OFFSET (Socket, Bus, Device, Function)));

      if (DeviceControl & B_PCIE_DEVICE_CONTROL_NF_ERR_EN) {
        ErrorFound = TRUE;
      }
    }
  }

  RASDEBUG ((DEBUG_INFO, "\t PciErrLibIsNonfatalPcieDeviceError: ErrorFound: %d\n", ErrorFound));
  return ErrorFound;
}

/**
  This function checks the SERR and PCIe device status to see if a PCIe Uncorrectable Fatal
  error has occurred.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    BOOLEAN        TRUE   - A PCIe Uncorrectable Fatal error was detected.
                            FALSE  - A PCIe Uncorrectable Fatal error was not detected.
--*/
BOOLEAN
PciErrLibIsFatalPcieDeviceError (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8     CapabilitiesOffset;
  UINT16    DeviceStatus;
  UINT16    DeviceControl;
  UINT16    PrimaryStatus;
  BOOLEAN   ErrorFound;

  ErrorFound = FALSE;
  
  // Get the capabilities offset
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);

  // Check if the device status indicates that a FATAL error occurred
  DeviceStatus = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)));

  if (DeviceStatus & B_PCIE_DEVICE_STATUS_FAT_ERR) {
    // FATAL error status set, now check if FATAL error reporting is enabled.
    // FATAL error reporting is enabled if SERR is enabled or FATAL reporting is enabled
    // in the PCIe device control register.
    if (MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_COMMAND_OFFSET)) & EFI_PCI_COMMAND_SERR) {
    	PrimaryStatus = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

      // NON-FATAL status and NON-FATAL reporting enabled.
      // The PCIe status could be set even if the error was masked.
      // The PCIe status could be set even if there are not bits set in the AER status register.
      // Check SERR status to see if a FATAL/NON-FATAL error message was sent.
      if (PrimaryStatus & EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
        // SERR was signaled (FATAL or Non-Fatal error occurred)
        ErrorFound = TRUE;
      }
    } else {
      DeviceControl = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVCTL_OFFSET (Socket, Bus, Device, Function)));

      if (DeviceControl & B_PCIE_DEVICE_CONTROL_FAT_ERR_EN) {
        ErrorFound = TRUE;
      }
    }
  }
  return ErrorFound;
}

/**
  This function determines if the specified PCI or PCI-X device is reporting an error and handles the
  error if one is detected.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number.

  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/
BOOLEAN
PciErrLibNonPcieDeviceErrorHandler (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  BOOLEAN   ErrorDetected = FALSE;
  UINT16    PciSts;

  PciSts = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));
  RASDEBUG ((DEBUG_INFO, "PciErrLibNonPcieDeviceErrorHandler: 0x%PciSts\n", PciSts));

  if (PciSts & EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
	ErrorDetected = TRUE;
	RASDEBUG ((DEBUG_INFO, "PciErrLibNonPcieDeviceErrorHandler: set ErrorDetected = 1\n"));
	if (mAllowClearingOfPcieDeviceStatus) {
	  MmioWrite16 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET), (PciSts & EFI_PCI_STATUS_SIGNALED_SYSTEM_ERROR));
	}
  }

  return ErrorDetected;
}

/**
  This function determines if the specified device is reporting an error and handles the
  error if one is detected.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/
BOOLEAN
PciErrLibPcieDeviceErrorHandler (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  BOOLEAN   ErrorDetected = FALSE;

  // Check for an Uncorrectable PCIe error
  if (PciErrLibIsFatalPcieDeviceError (Socket, Bus, Device, Function) ||
       PciErrLibIsNonfatalPcieDeviceError (Socket, Bus, Device, Function)) {

    ErrorDetected = TRUE;

    // Do not report this device as having an error if this is a P2P bridge
    // and there is a subordinate device indicating an error occurred.
    if (!PciErrLibIsPciBridgeDevice (Socket, Bus, Device, Function) ||
        !PciErrLibIsSubordinateDeviceError (Socket, Bus, Device, Function)) {
      PciErrLibHandleUncorrectablePcieDeviceError (Socket, Bus, Device, Function);
    }
  } 

  // Check for a Correctable PCIe error
  if (PciErrLibIsCorrectablePcieDeviceError (Socket, Bus, Device, Function)) {
    ErrorDetected = TRUE;

    // Do not report this device as having an error if this is a P2P bridge
    // and there is a subordinate device indicating an error occurred.
    if (!PciErrLibIsPciBridgeDevice (Socket, Bus, Device, Function) ||
        !PciErrLibIsSubordinateDeviceError (Socket, Bus, Device, Function)) {

      PciErrLibHandleCorrectablePcieDeviceError (Socket, Bus, Device, Function);
    }
  } 
  if (ErrorDetected && mAllowClearingOfPcieDeviceStatus) {
    PciErrLibClearPcieDeviceStatus (Socket, Bus, Device, Function);
  } 
  return ErrorDetected;
}
/**
  This function checks if a specified device is a PCI Express device.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    BOOLEAN        TRUE   - PCIe device present at the specified address.
                            FALSE  - PCIe device not present at the specified address.
--*/
BOOLEAN
PciErrLibIsPcieDevice (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8   CapabilitiesOffset;

  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }
  return TRUE;
}

/**
  PCI Express error handler for ESB2 and BNB PCI Express Root Ports.

  @param[in] RootPortSocket         PCIe root port deviec's socket number.
  @param[in] RootPortBus            PCIe root port device's bus number.
  @param[in] RootPortDevice         PCIe root port device's device number.
  @param[in] RootPortFunction       PCIe root port device's function number. 
    
  @retval    BOOLEAN        TRUE   - An error was detected.
                            FALSE  - An error was not detected.
--*/
BOOLEAN
PciErrLibPcieRootPortErrorHandler (
  IN      UINT8   RootPortSocket,
  IN      UINT8   RootPortBus,
  IN      UINT8   RootPortDevice,
  IN      UINT8   RootPortFunction
  )
{
  CONST     UINT32 URBit = 0x10000;
  UINT8     Bus;
  UINT8     Device;
  UINT8     Function;
  BOOLEAN   ErrorDetected = FALSE;
  BOOLEAN   URDetected;
  UINT8     FuncExist = 0;
  UINT8     SecondaryBusNumber;
  UINT8     SubordinateBusNumber;
  PCIE_ERROR_SECTION PcieErrorRec;

  // Check for errors on subordinate devices
  SecondaryBusNumber = MmioRead8 (MmPciAddress(RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
  SubordinateBusNumber = MmioRead8 (MmPciAddress (RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));

  RASDEBUG ((DEBUG_INFO, "PciErrLibPcieRootPortErrorHandler: Sec Bus: %d, Sub Bus: %d\n", SecondaryBusNumber, SubordinateBusNumber));

  if (SecondaryBusNumber && SubordinateBusNumber) {
    PcieSuppressUnsupportReqest(RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction, TRUE, FALSE);
    for (Bus = SecondaryBusNumber; SecondaryBusNumber <= Bus && Bus <= SubordinateBusNumber; Bus++) {
      for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    	FuncExist = 0;
    	URDetected = FALSE;
        for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {
          if (PciErrLibIsDevicePresent (RootPortSocket, Bus, Device, Function)) {
        	RASDEBUG ((DEBUG_INFO, "\t PciErrLibPcieRootPortErrorHandler :  B: %x, D: %x, F:%x\n", Bus, Device, Function));
            PcieGetAerErrors (&PcieErrorRec, RootPortSocket, Bus, Device, Function);
            RASDEBUG ((DEBUG_INFO, "\t Existing : NFE: %x, CE: %x\n", PcieErrorRec.Uncerrsts, PcieErrorRec.Corerrsts));
            URDetected |= PcieErrorRec.Uncerrsts & URBit;
            PcieSuppressUnsupportReqest(RootPortSocket, Bus, Device, Function, TRUE, FALSE);
        // APTIOV_SERVER_OVERRIDE_RC_START : Fix PCIE error not logging if the errors occurs on Root PORT ALONE.
            ErrorDetected = PciErrLibIsPcieDevice (RootPortSocket, Bus, Device, Function) ? PciErrLibPcieDeviceErrorHandler (RootPortSocket, Bus, Device, Function) : PciErrLibPciDeviceErrorHandler (RootPortSocket, Bus, Device, Function);
        // APTIOV_SERVER_OVERRIDE_RC_END : Fix PCIE error not logging if the errors occurs on Root PORT ALONE.
          } else {
        	FuncExist = Function;
            break;
          }

        }// for (Function = 00; Function <= PCI_MAX_FUNC; Function++)

        for (Function = 0; Function < FuncExist; Function++) { // For existing function, clear UR status if it was not initailly logged.
        	PcieSuppressUnsupportReqest(RootPortSocket, Bus, Device, Function, FALSE, !URDetected); // Restore AER UR mask
            PcieGetAerErrors (&PcieErrorRec, RootPortSocket, Bus, Device, Function);
            RASDEBUG ((DEBUG_INFO, "\t UR Cleared. NFE: %x, CE: %x\n", PcieErrorRec.Uncerrsts, PcieErrorRec.Corerrsts));
        }

      }// for (Device = 00; Device <= PCI_MAX_DEVICE; Device++)
    } // for (Bus = SecondaryBusNumber; Bus <= SubordinateBusNumber; Bus++)
    PcieSuppressUnsupportReqest(RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction, FALSE, TRUE);
  } // if (SecondaryBusNumber && SubordinateBusNumber)
  
   // Check for a Root Port error only if we did not find any subordinate devices with an error
  if (!ErrorDetected) {
    RASDEBUG ((DEBUG_INFO, "\t Handling the error for RootPort\n"));

    // Check for an Uncorrectable PCIe error
    if (PciErrLibIsFatalPcieDeviceError (RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction) ||
      PciErrLibIsNonfatalPcieDeviceError (RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction)) {
      ErrorDetected = TRUE;
      PciErrLibHandleUncorrectablePcieDeviceError (RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction);      
    }

    // Check for a Correctable PCIe error
    if (PciErrLibIsCorrectablePcieDeviceError (RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction)) {
      ErrorDetected = TRUE;
      PciErrLibHandleCorrectablePcieDeviceError (RootPortSocket, RootPortBus, RootPortDevice, RootPortFunction);
    }
  }
  return ErrorDetected;
}

/**
  This function checks to see if PCIe error reporting is enabled in the specified 
  PCIe root port device.

  @param[in[ Socket         PCIe root port device's socket number.
  @param[in] Bus            PCIe root port device's bus number.
  @param[in] Device         PCIe root port device's device number.
  @param[in] Function       PCIe root port device's function number. 
    
  @retval    BOOLEAN        TRUE   - Error reporting is enabled.
                            FALSE  - Error reporting is not enabled or it is under OS control.
--*/
BOOLEAN
PciErrLibIsRootPortErrReportingEnabled (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  MISCCTRLSTS_1_IIO_PCIE_STRUCT    MiscCtrlSts1;
  UINT8                            CapabilitiesOffset;
  UINT16                           RootControl;
  BOOLEAN                          IsEnabled;

  IsEnabled = FALSE;
  
  if (PciErrLibIsDevicePresent (0, Bus, Device, Function)) {
    // Get the capabilities offset
    CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);

    RootControl = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_ROOT_CONTROL_OFFSET));

    if (RootControl & (B_PCIE_ROOT_CONTROL_FAT_ERR_EN | B_PCIE_ROOT_CONTROL_NF_ERR_EN | B_PCIE_ROOT_CONTROL_COR_ERR_EN)) {
      IsEnabled = TRUE;
    }

    MiscCtrlSts1.Data = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, ONLY_REGISTER_OFFSET (MISCCTRLSTS_1_IIO_PCIE_REG)));
    if ( (MiscCtrlSts1.Bits.override_system_error_on_pcie_correctable_error_enable == 1) || 
         (MiscCtrlSts1.Bits.override_system_error_on_pcie_non_fatal_error_enable == 1)  ||
         (MiscCtrlSts1.Bits.override_system_error_on_pcie_fatal_error_enable == 1) ) {
        IsEnabled = TRUE;
    }
  }

  return IsEnabled;
}

/**
  This function gets the capability structure.

  @param[in] CapBuf         a pointer to the capability array in the PCIE error record structure.
  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    Status
--*/
STATIC EFI_STATUS
PcieGetCapabilities (
  IN OUT  UINT8   *CapBuf,
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
)
{
  UINT8   PciPrimaryStatus;
  UINT8   CapabilityOffset;
  UINT8   CapId;
  UINT8   *CapPointer;
  UINT8   i;

  CapPointer = CapBuf;

  PciPrimaryStatus = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY) {    
    CapabilityOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
    for (i = 0; i < 60; i++) {
      CapId = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, CapabilityOffset + i));
      *CapPointer++  =  CapId;      
    }
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

/**
  This function gets the AER capability.
  
  @param[in] AerBuf         a pointer to the AER capability array in the PCIE error record structure.
  @param[in] Socket         PCIe devices' socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    Status
--*/
STATIC EFI_STATUS
PcieGetAer (
    IN OUT UINT8 *AerBuf,
    IN UINT8   Socket,
    IN UINT8   Bus,
    IN UINT8   Device,
    IN UINT8   Function
)
{
  UINT8                       PciPrimaryStatus;
  UINT16                      Offset;
  UINT8                       *AerPointer;
  UINT16                      Index;

  Index = 0;
  PciPrimaryStatus = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));


  if (PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY) {  
    Offset = PciErrLibGetExtendedCapabilitiesOffset (0, Bus, Device, Function, PCIE_EXT_CAP_HEADER_AERC, 0xff);
    if (Offset != 0) {
      // Copy AER to buffer
      AerPointer = (UINT8*)AerBuf;
      for (Index = 0; Index<AER_CAPABILITY_SIZE; Index++) {
    	*AerPointer++ = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, Offset + Index));
      }
      return EFI_SUCCESS;
    } else {
      return EFI_NOT_FOUND;
    }
  } else {
    return EFI_UNSUPPORTED;
  }
}

/**
  This function gets the slot number.
  
  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    Value          PcieSlotNum  - if applicable
                            0            - for Ports connected to devices that are either 
                                           integrated on the system board or integrated within 
                                           the same silicon as the Switch device or Root Port.
--*/
UINT16
GetSlotNum (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8   CapabilitiesOffset;
  UINT32  PcieSlotNum;

  PcieSlotNum = 0;
  
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
  if (CapabilitiesOffset) {
    PcieSlotNum = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + PCI_EXP_CAP_SLCAP));
    PcieSlotNum = PcieSlotNum >> 19;
  }
  
  return (UINT16) PcieSlotNum;
}

/**
  This function gets the port type.
  
  This value is only valid for Functions that implement a Type
  01h PCI Configuration Space header
  
  @param[in] Socket         PCIe deivce's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number. 
    
  @retval    Value          Port Type  - if applicable
                            0xff       - if no port type
--*/
UINT32
GetPortType (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8   CapabilitiesOffset;
  UINT16  PcieCapabilities;

  
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
  if (CapabilitiesOffset == 0) {
    return 0xff;
  }

  PcieCapabilities = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_CAPABILITY_OFFSET));

  return ((PcieCapabilities & 0xF0) >> 4);
}

/**
  This function gets the AER errors from the PCIE Extended Capabilities
  (Uncorrectable, correctable and RP errors)

  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number.

  @retval    Status
--*/
STATIC EFI_STATUS
PcieGetAerErrors (
  IN OUT  PCIE_ERROR_SECTION *PcieErrorRec,
  IN      UINT8              Socket,
  IN      UINT8              Bus,
  IN      UINT8              Device,
  IN      UINT8              Function
)
{
  UINT8     PciPrimaryStatus;
  UINT16    PcieCapabilities;
  UINT8     CapabilitiesOffset;
  UINT16    Offset;

  PcieErrorRec->Uncerrsts = 0;
  PcieErrorRec->Corerrsts = 0;
  PcieErrorRec->Rperrsts = 0;


  PciPrimaryStatus = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY) {
    Offset = PciErrLibGetExtendedCapabilitiesOffset (Socket, Bus,Device, Function, PCIE_EXT_CAP_HEADER_AERC, 0xff);
    if (Offset != 0) {
      PcieErrorRec->Uncerrsts = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_UNCERRSTS_OFFSET));

      PcieErrorRec->Corerrsts = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_CORERRSTS_OFFSET));

      // Get the capabilities offset and verify this is a root port
      CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);

      PcieCapabilities = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_CAPABILITY_OFFSET));

      if ((PcieCapabilities & 0xf0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
        PcieErrorRec->Rperrsts = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_ROOTERRSTS_OFFSET));

      }      
      return EFI_SUCCESS;
    } else {
      return EFI_NOT_FOUND;
    }
  } else {
    return EFI_UNSUPPORTED;
  }
}

/**
  This function gets suppress UR escalation and clear UR status if specified.

  @param[in] Socket         PCIe device's socket number.
  @param[in] Bus            PCIe device's bus number.
  @param[in] Device         PCIe device's device number.
  @param[in] Function       PCIe device's function number.
  @param[in] Mask           To set UR bit in UNCERRMSK
  @param[in] Clear          To clear UR bit in UNCERRSTS
  @retval    Status
--*/

STATIC EFI_STATUS
PcieSuppressUnsupportReqest (
  IN      UINT8              Socket,
  IN      UINT8              Bus,
  IN      UINT8              Device,
  IN      UINT8              Function,
  IN      BOOLEAN            Mask,
  IN      BOOLEAN            Clear
)
{
  UINT8     PciPrimaryStatus;
  UINT16    Offset;
  UINT32    Reg32;

  PciPrimaryStatus = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (!(PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY))
	return EFI_NOT_FOUND;

  Offset = PciErrLibGetExtendedCapabilitiesOffset (Socket, Bus,Device, Function, PCIE_EXT_CAP_HEADER_AERC, 0xff);

  if (!Offset)
	return EFI_NOT_FOUND;

  if (Clear){
    // Write UNCERRSTS[20] = 1 to clear AER UR.
    MmioWrite32(MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_UNCERRSTS_OFFSET), (1 << 20));
    MmioWrite32(MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_CORERRSTS_OFFSET), (1 << 13));

    // Write DEVSTS[3] = 1 to clear DEVSTS UR
    MmioWrite16(MmPciAddress (Socket, Bus, Device, Function, PCIE_DEVSTS_OFFSET), (1 << 3));
  }

  // Mask or unmask UR escalation.
  Reg32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_UNCERRMSK_OFFSET));
  Reg32 = Mask? Reg32 | (1 << 20) : Reg32 & ~(1 << 20);
  MmioWrite32(MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_UNCERRMSK_OFFSET), Reg32);

  // Mask or unmask Advisory Non-fatal error
  Reg32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_CORERRMSK_OFFSET));
  Reg32 = Mask? Reg32 | (1 << 13) : Reg32 & ~(1 << 13);
  MmioWrite32(MmPciAddress (Socket, Bus, Device, Function, Offset + PCIE_AER_CORERRMSK_OFFSET), Reg32);

  if (PCIeIsRootPort (Socket, Bus, Device, Function)){
	Reg32 = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, R_PCIE_PORT_RPEDMASK));
	Reg32 = Mask ? Reg32 | (1 << 1) : Reg32 & ~(1 << 1);
	MmioWrite32(MmPciAddress (Socket, Bus, Device, Function, R_PCIE_PORT_RPEDMASK), Reg32);
  }
  return EFI_SUCCESS;

}

/**
 This function finds the highest severity
  
  @param[in]  Sev       Error type detected. 
    
  @retval     Status
--*/
VOID 
ErrorSeverity (
  IN      OUT   UINT16 Sev
    )   
{
  switch (Sev) {
    case EFI_ACPI_5_0_ERROR_SEVERITY_FATAL:
      mSeverity = Sev;
      break;
    case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED:
      if (mSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_NONE) {
        mSeverity = Sev;
      }
      break;
    case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE:
      if ((mSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_NONE) || (mSeverity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED)) {
        mSeverity = Sev;  
      }
      break;
    default:
      RASDEBUG ((DEBUG_ERROR, "ErrorSeverity, Invalid error type: %x \n", Sev)); 
      break;
  }
  RASDEBUG ((DEBUG_ERROR, "ErrorSeverity, Severity: %x \n", Sev)); 
}

/**
  This function fills the PCIE error record fields logs the 
  appropriate type of error by calling mPlatformErrorHandlingProtocol.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PcieLogReportError (
  IN      UINT8                    Socket,
  IN      UINT8                    Bus,
  IN      UINT8                    Device,
  IN      UINT8                    Function,
  IN      UINT8                    ErrorType
  )
{
  UINT16                    ErrType;
  UINT32                    Data32;
  EFI_STATUS                Status;
  UINT64                    ValidBits;
  UEFI_PCIE_ERROR_RECORD    PciErrStruc;
  
  ZeroMem(&PciErrStruc, sizeof(UEFI_PCIE_ERROR_RECORD));

  // Build Error Record Header
  PciErrStruc.Header.SignatureStart = SIGNATURE_32('C','P','E','R');
  PciErrStruc.Header.Revision = 0x0204;
  PciErrStruc.Header.SignatureEnd = 0xffffffff;
  PciErrStruc.Header.SectionCount = 1;  // Only supporting one section per error record
  PciErrStruc.Header.ValidBits = 0;
  PciErrStruc.Header.RecordLength = sizeof (UEFI_PCIE_ERROR_RECORD);
  CopyMem (&PciErrStruc.Header.NotificationType, &gErrRecordNotifyPcieGuid, sizeof (EFI_GUID));
  RASDEBUG ((DEBUG_ERROR, "PcieLogReportError Error in device:  Bus = %x, Dev = %x, Func = %x, \n", Bus , Device, Function));
     // Fill error section
  PciErrStruc.Section.Version               = 0x0300;

  PciErrStruc.Section.CommandStatus         = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, PCI_COMMAND_OFFSET));
  PciErrStruc.Section.DeviceId.VendorId     = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_VENDOR_ID_OFFSET));
  PciErrStruc.Section.DeviceId.DeviceId     = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_DEVICE_ID_OFFSET));
  PciErrStruc.Section.DeviceId.ClassCode[0] = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_CLASSCODE_OFFSET));
  PciErrStruc.Section.DeviceId.ClassCode[1] = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_CLASSCODE_OFFSET + 1));
  PciErrStruc.Section.DeviceId.ClassCode[2] = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_CLASSCODE_OFFSET + 2));

  PciErrStruc.Section.DeviceId.FuncNum      = Function;
  PciErrStruc.Section.DeviceId.DevNum       = Device;
  PciErrStruc.Section.DeviceId.SegNum       = 0;
#if SMCPKG_SUPPORT
  PciErrStruc.Section.DeviceId.PriBus = Bus;
#else   
  PciErrStruc.Section.DeviceId.PriBus = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET));
#endif

  ValidBits = 0;
  Data32 = (PciErrStruc.Section.DeviceId.ClassCode[2] << 0x18) | (PciErrStruc.Section.DeviceId.ClassCode[1] << 0x10) | 
           (PciErrStruc.Section.DeviceId.ClassCode[0] << 0x08);
  if ((Data32 == EFI_PCI_BRIDGE_DEV_CLASS_CODE) ||
            (Data32 == EFI_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE)) {
    //Yes, it is PCI bridge
    //Fill bridge information
    PciErrStruc.Section.DeviceId.SecBus = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));   // Secondary Bus Number
    PciErrStruc.Section.BridgeControlSts.Bits.BrdgCntl = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_BRIDGE_CONTROL_REGISTER_OFFSET));         // Bridge Control
    PciErrStruc.Section.BridgeControlSts.Bits.BrdgSecondarySts = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_BRIDGE_STATUS_REGISTER_OFFSET));

    // Set Bridge information valid 
    ValidBits |= PCIE_ERROR_BRIDGE_CRL_STS_VALID;    
    PciErrStruc.Section.DeviceId.SlotNumber.SlotNum = GetSlotNum (Socket, Bus, Device, Function);
  }
  Status = PcieGetCapabilities (
              &(PciErrStruc.Section.CapStruct[0]),
              Socket,
              Bus,
              Device,
              Function
              );
  if (Status == EFI_SUCCESS) {
    PciErrStruc.Section.PortType = ((PciErrStruc.Section.CapStruct[2]) >> 4);
    ValidBits |= PCIE_ERROR_CAPABILITY_INFO_VALID | PCIE_ERROR_PORT_TYPE_VALID;
  }
  Status = PcieGetAer (
              &(PciErrStruc.Section.AerInfo[0]),
              Socket,
              Bus,
              Device,
              Function
              );
  if (Status == EFI_SUCCESS) {
    ValidBits |= PCIE_ERROR_AER_INFO_VALID;
  }
  Status = PcieGetAerErrors (
               &(PciErrStruc.Section),
               Socket,
               Bus,
               Device,
               Function
              );
  // Fill error descriptor
  PciErrStruc.Descriptor.Offset = sizeof (UEFI_ERROR_RECORD_HEADER) + sizeof (UEFI_ERROR_SECT_DESC);
  PciErrStruc.Descriptor.Length = sizeof (PCIE_ERROR_SECTION);
  PciErrStruc.Descriptor.ValidBits = 0;
  PciErrStruc.Descriptor.Flags = 1;   // Primary
  CopyMem (&PciErrStruc.Descriptor.SectionType, &gErrRecordPcieErrGuid, sizeof (EFI_GUID));

  //Check for hot removal under L1 case. If so, downgrad the severity to avoid NMI signaling.
  if (IsHotRemoveUnderL1 (Socket, Bus, Device, Function)) {
	RASDEBUG ((DEBUG_ERROR, "PcieLogReportError Error in device: Hot Removal L1 case. For sev to be corrected \n", Bus , Device, Function));
	ErrorType = INVALID_ERROR_TYPE;
#if SMCPKG_SUPPORT
	// Even I mask the bit of "PCI_link_bandwidth_Changed_mask(IIO dev[3:0] fn[0] offset[0x204] bit[0] )",
	// the status bit will still be set when I hot plug out the NVMe drive.
	// So do not log any error to BMC/WHEA/GPNV when it is a hot removal.
	return;
#endif
  }

  switch (ErrorType) {
    case SERROR_TYPE:
      ErrType             = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      break;

    case PERROR_TYPE:
      ErrType             = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      break;

    case COR_ERROR_TYPE:
      ErrType             = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
      break;

    case NONFATAL_ERROR_TYPE:
      mPcieNonFatalErrDetected = TRUE;
      ErrType             = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE ;
      break;

    case FATAL_ERROR_TYPE:
      mPcieFatalErrDetected   = TRUE;
      ErrType             = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      break;

    case INVALID_ERROR_TYPE:
      ErrType             = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;
      break;

    default:
      return;
  }
  RASDEBUG ((DEBUG_ERROR, "PcieLogReportError, ErrType = %x\n", ErrType)); 
  if (ErrType == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) {
    PciErrStruc.Header.Flags = HW_ERROR_FLAGS_RECOVERED;
  }
 
  PciErrStruc.Header.Severity = ErrType;
  PciErrStruc.Descriptor.Severity = ErrType;
  PciErrStruc.Section.ValidBits = ValidBits | PCIE_ERROR_VERSION_VALID | PCIE_ERROR_COMMAND_STATUS_VALID | PCIE_ERROR_DEVICE_ID_VALID;
  Status = mPlatformErrorHandlingProtocol->LogReportedError (&PciErrStruc.Header);
  ErrorSeverity (ErrType);
}


////////////////////////////////////// INITIALIZATION ROUTINES /////////////////////////////////////////

/**
  This function enables SERR and Parity error reporting in the P2P bridge's
  bridge control register.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibEnableBridgeControl (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8 Data8;

  Data8 = MmioRead8 (MmPciAddress (Socket, Bus, Device, Function, PCI_BRIDGE_CONTROL_REGISTER_OFFSET));

  // Enable SERR if selected in Setup
  if (mNmiOnSerr) {
    Data8 |= EFI_PCI_BRIDGE_CONTROL_SERR;

    // enable PERR if selected in Setup
    if (mNmiOnPerr) {
      Data8 |= EFI_PCI_BRIDGE_CONTROL_PARITY_ERROR_RESPONSE;
    } else {
 
      // if PERR is disabled in Setup, disable PERR alone
      Data8 &= ~EFI_PCI_BRIDGE_CONTROL_PARITY_ERROR_RESPONSE;
    }
  } else {

    // if SERR is disabled in setup, disable both SERR and PERR generation
    Data8 &= ~(EFI_PCI_BRIDGE_CONTROL_SERR | EFI_PCI_BRIDGE_CONTROL_PARITY_ERROR_RESPONSE);
  }

   MmioWrite8 (MmPciAddress (Socket, Bus, Device, Function, PCI_BRIDGE_CONTROL_REGISTER_OFFSET), Data8);

}

/**
  This function looks for the corresponding port on specified PCIe device using
  BDF and returns the value of its ECRC knob

  @oaram[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    UINT8          PCIe port ECRC knob value.
--*/
UINT8
PciPortEcrcEnabled (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8  Port;

  RASDEBUG ((DEBUG_INFO, "Checking ECRC enable for PCIe port on device %x:%x:%x\n", Bus, Device, Function));

  // Search the port from BDF
  for (Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
    if (mRasTopology->SystemInfo.SocketInfo[Socket].UncoreIioInfo.PciPortInfo[Port].Bus      == Bus      &&
        mRasTopology->SystemInfo.SocketInfo[Socket].UncoreIioInfo.PciPortInfo[Port].Device   == Device   &&
        mRasTopology->SystemInfo.SocketInfo[Socket].UncoreIioInfo.PciPortInfo[Port].Function == Function) {
      RASDEBUG ((DEBUG_INFO, "PCIe Port found as %x on device %x:%x:%x\n", Port, Bus, Device, Function));
      return mRasSetup->Iio.IioPciePortEcrcEn[Port];
    }
  }

  RASDEBUG ((DEBUG_INFO, "Not port found for PCIe device%x:%x:%x\n", Bus, Device, Function));
  // Port was not found
  return 0;

}

/**
  This function performs the following operations on the specified PCIe device:
    1.  Programs the Uncorrectable Error Mask register if applicable
    2.  Programs the Uncorrectable Error Severity register if applicable
    3.  Programs the Correctable Error Mask register if applicable
    4.  Clears the standard PCIe status registers
    5.  Disables legacy SERR reporting if appropriate (no AER registers)
    6.  Enables error reporting for root ports and regular devices

  @oaram[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number.

  @retval    None
--*/
VOID
PciErrLibEnablePcieELogCapabilities (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8       CapabilitiesOffset;
  UINT16      PcieCapabilities;
  UINT16      DeviceControl;
  UINT16      RootControl;
  UINT16      StatusReg16;
  BOOLEAN     RootPort;
  UINT16      AerCap, VendorId, DeviceId;
  UINT32      ErrorMask;
  UINT32      ErrorSev;
  UINT32      ErrorStatus;

  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (0, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
  if (CapabilitiesOffset == 0) {
    return ;
  }

  PcieCapabilities = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_CAPABILITY_OFFSET));

  if ((PcieCapabilities & 0xf0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    RootPort = TRUE;
  } else {
    RootPort = FALSE;
  }

  RASDEBUG ((DEBUG_ERROR, "Enabling PCIe error logging for %x:%x:%x\n", Bus, Device, Function));

  AerCap = PciErrLibGetExtendedCapabilitiesOffset (0, Bus, Device, Function, PCIE_EXT_CAP_HEADER_AERC, 0xff);
  if (AerCap) {

    RASDEBUG ((DEBUG_ERROR, "Programming AER capabilities for %x:%x:%x\n", Bus, Device, Function));
    VendorId = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_VENDOR_ID_OFFSET));
    DeviceId = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, PCI_DEVICE_ID_OFFSET));

    RASDEBUG ((DEBUG_INFO, "VendorId = 0x%x, DeviceId = 0x%x for %x:%x:%x\n", VendorId, DeviceId, Bus, Device, Function));

    // Clear Uncorrectable and Correctable status

    ErrorStatus = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, AerCap + PCIE_AER_UNCERRSTS_OFFSET));
    MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, AerCap + PCIE_AER_UNCERRSTS_OFFSET), ErrorStatus);
  	ErrorStatus = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, AerCap + PCIE_AER_CORERRSTS_OFFSET));
    MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, AerCap + PCIE_AER_CORERRSTS_OFFSET), ErrorStatus);

    // Program Correctable Error Mask
    ErrorMask = PCIEAER_CORRERR_MSK;
    if (mRasSetup->Pcie.PcieAerCorrErrEn == 1) {
      // Only (BIT0, BIT6, BIT7, BIT8, BIT12) needs to be changed, the rest are masked
      ErrorMask &= ~mRasSetup->Pcie.PcieAerCorrErrBitMap;
    }
    RASDEBUG ((DEBUG_ERROR, "Programming Correctable Error Mask to %x\n", ErrorMask));
    MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, AerCap + PCIE_AER_CORERRMSK_OFFSET), ErrorMask);

    // Program Uncorrectable Error Mask and Severity
    ErrorMask = PCIEAER_UNCORRERR_MSK;
    ErrorSev = 0;
    if (mRasSetup->Pcie.PcieAerAdNfatErrEn == 1) {
      // Only (BIT12, BIT15, BIT16, BIT20) needs to be changed, the rest are masked
      ErrorMask &= ~mRasSetup->Pcie.PcieAerAdNfatErrBitMap;
    }
    if (mRasSetup->Pcie.PcieAerNfatErrEn == 1) {
      // Only (BIT14, BIT21) needs to be changed, the rest are masked
      ErrorMask &= ~mRasSetup->Pcie.PcieAerNfatErrBitMap;
    }
    if (mRasSetup->Pcie.PcieAerFatErrEn == 1) {
      // Only (BIT4, BIT5, BIT13, BIT17, BIT18) needs to be changed, the rest are masked
      ErrorMask &= ~mRasSetup->Pcie.PcieAerFatErrBitMap;
      ErrorSev = mRasSetup->Pcie.PcieAerFatErrBitMap & PCIEAER_UNCORRERR_MSK;
    }
    if (PciPortEcrcEnabled (Socket, Bus, Device, Function) == 1) {
      // Only BIT19 corresponding ECRC, the rest are masked
      ErrorMask &= ~BIT19;
	  //b307207: ECRC error needs to be treated as fatal. 
      ErrorSev  |= BIT19; 
    }
    ErrorMask |= BIT20|BIT5; //UR must be masked to make it advisory non-fatal instead	//SMCPKG_SUPPORT

    //
    // WA for PCH Bug 1404326737 is not complete.  
    // For QAT, need to mask Uncorrectable Internal Error in AER Uncorrectable Error Mask
    //

    if ((VendorId == V_PCH_INTEL_VENDOR_ID) && (DeviceId == V_PCH_QAT_DEVICE_ID)) {
      RASDEBUG ((DEBUG_INFO, "QAT device is at %x:%x:%x\n", Bus, Device, Function));
      ErrorMask |= BIT22;  //Mask Uncorrectable Internal Error for QAT
    }

    RASDEBUG ((DEBUG_ERROR, "Programming Uncorrectable Error B %x D %x F %x\n", Bus, Device, Function));
    RASDEBUG ((DEBUG_ERROR, "Programming Uncorrectable Error Mask to %x\n", ErrorMask));
    RASDEBUG ((DEBUG_ERROR, "Programming Uncorrectable Error Severity to %x\n", ErrorSev));

    MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, AerCap + PCIE_AER_UNCERRMSK_OFFSET), ErrorMask);
    MmioWrite32 (MmPciAddress (Socket, Bus, Device, Function, AerCap + PCIE_AER_UNCERRSEV_OFFSET), ErrorSev);

  }

  // Clear the PCIe device status register
  // FInd a better way to check for DMI port
  StatusReg16 = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)));
  MmioWrite16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVSTS_OFFSET (Socket, Bus, Device, Function)), StatusReg16);

  // If this is a PCIe Root Port of the PCIe root complex, then configure its
  // Root Control Register also...
  if (RootPort) {
 	RootControl = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_ROOT_CONTROL_OFFSET));

    // Enable SERR on Fatal, Non-Fatal, and Correctable errors for error reporting.
    // An NMI will not be generated for correctable errors.
    if (mRasSetup->Pcie.PcieCorrErrEn == 1) {
      RootControl |= B_PCIE_ROOT_CONTROL_COR_ERR_EN;
    }
    if (mRasSetup->Pcie.PcieUncorrErrEn == 1) {
      RootControl |= B_PCIE_ROOT_CONTROL_NF_ERR_EN;
    }
    if (mRasSetup->Pcie.PcieFatalErrEn == 1) {
      RootControl |= B_PCIE_ROOT_CONTROL_FAT_ERR_EN;
    }
    RASDEBUG ((DEBUG_ERROR, "Programming Root Control to %x\n", RootControl));
    MmioWrite16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + PCIE_ROOT_CONTROL_OFFSET), RootControl);
  }

  // Enable Error logging in Device Control...

  DeviceControl = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVCTL_OFFSET (Socket, Bus, Device, Function)));

   
  if (mRasSetup->Pcie.PcieCorrErrEn == 1) {
    DeviceControl |= B_PCIE_DEVICE_CONTROL_COR_ERR_EN;
  }
  if (mRasSetup->Pcie.PcieUncorrErrEn == 1) {
    DeviceControl |= B_PCIE_DEVICE_CONTROL_NF_ERR_EN;
  }
  if (mRasSetup->Pcie.PcieFatalErrEn == 1) {
    DeviceControl |= B_PCIE_DEVICE_CONTROL_FAT_ERR_EN;
  }

  RASDEBUG ((DEBUG_ERROR, "Programming Device Control to %x at offset 0x%x\n", DeviceControl, GET_DEVCTL_OFFSET (Socket, Bus, Device, Function)));
  MmioWrite16 (MmPciAddress (Socket, Bus, Device, Function, CapabilitiesOffset + GET_DEVCTL_OFFSET (Socket, Bus, Device, Function)), DeviceControl);

}

/**
  This function initializes and enables error reporting in the specified PCI or PCIe
  device and all subordinate PCI or PCIe devices.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibEnableElogDeviceAndSubDevices (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT8 SecondaryBusNumber;
  UINT8 SubordinateBusNumber;

  //
  // Enable error logging in the current device...
  //
  PciErrLibEnableElogDevice (Socket, Bus, Device, Function);

  //
  // Enable error logging in the devices behind a bridge...
  //

  if (PciErrLibIsPciBridgeDevice (0, Bus, Device, Function)) {

    SecondaryBusNumber = MmioRead8 (
                           MmPciAddress (
                             Socket,
                             Bus,
                             Device,
                             Function,
                             PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                           ));

    SubordinateBusNumber = MmioRead8 (
                            MmPciAddress (
                              Socket,
                              Bus,
                              Device,
                              Function,
                              PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET
                            ));
#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
    if (SubordinateBusNumber >= SecondaryBusNumber) {
#else
    if ( SubordinateBusNumber > SecondaryBusNumber ) { 
#endif // #if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
      for (Bus = SecondaryBusNumber; Bus <= SubordinateBusNumber; Bus++) {
        for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
          for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {
            PciErrLibEnableElogDevice (Socket, Bus, Device, Function);
          } // Function
        } // Device
        //
        // if we have reach the latest bus number then break the loop in order 
        // to avoid a wrap up the counter. 
        if (Bus == 0xFF){
          break;
        }
      } // Bus
    } //Subordinate & Secondary Bus
  }// IsPcieBridgeDevice
}

/**
  This function initializes and enables error reporting in the specified PCI or PCIe device.

  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibEnableElogDevice (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  if (PciErrLibIsDevicePresent (Socket, Bus, Device, Function)) {
    RASDEBUG ((DEBUG_ERROR, "Enabling PCI error logging for %x:%x:%x\n", Bus, Device, Function));
  #if SMCPKG_SUPPORT  
  // Refer Grantley RC
     PciErrLibClearLegacyPciDeviceStatus (Socket, Bus, Device, Function);
     PciErrLibEnablePciSerrPerrGeneration (Socket, Bus, Device, Function);
  #endif   
    // Check if this is a PCIe device...
    if (PciErrLibIsPcieDevice (Socket, Bus, Device, Function)) {
        PciErrLibEnablePcieELogCapabilities (Socket, Bus, Device, Function);
     #if SMCPKG_SUPPORT == 0        
        PciErrLibClearLegacyPciDeviceStatus (Socket, Bus, Device, Function);
        PciErrLibEnablePciSerrPerrGeneration (Socket, Bus, Device, Function); 
     #endif   
    }

    if (PciErrLibIsPciBridgeDevice (Socket, Bus, Device, Function)) {
      // Enable SERR and PERR Error Logging in Bridge Control...
      // Serves both PCI and PCIe bridges...
      PciErrLibEnableBridgeControl (Socket, Bus, Device, Function);
    }
  }
}

/**
  This function enables SERR and Parity error reporting in the specified device's 
  legacy PCI command register.
  
  @param[in] Socket         Device's socket number.
  @param[in] Bus            Device's bus number.
  @param[in] Device         Device's device number.
  @param[in] Function       Device's function number. 
    
  @retval    None
--*/
VOID
PciErrLibEnablePciSerrPerrGeneration (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16  Data16;

  // Check if Assert on SERR is enabled in setup
  Data16 = MmioRead16(MmPciAddress(Socket, Bus, Device, Function, PCI_COMMAND_OFFSET));
  if (mNmiOnSerr) {
    Data16 |= EFI_PCI_COMMAND_SERR;
    //
    // Check if Assert NMI on PERR is enabled in setup
    //
    if (mNmiOnPerr) {
      Data16 |= EFI_PCI_COMMAND_PARITY_ERROR_RESPOND;
    } else {
      Data16 &= ~EFI_PCI_COMMAND_PARITY_ERROR_RESPOND;
    }
  } else {
    Data16 &= ~(EFI_PCI_COMMAND_SERR | EFI_PCI_COMMAND_PARITY_ERROR_RESPOND);
  }

  MmioWrite16 (MmPciAddress(Socket, Bus, Device, Function, PCI_COMMAND_OFFSET), Data16);
}

/**
  We need to clear any pending GPI SMI status to avoid a EOS.
  
  @retval    None
--*/
VOID
ClearGpiSmiStatus (
  VOID
  )
{
  UINT16  Data16;
  UINT16  PmBase;

  RASDEBUG ((DEBUG_ERROR, "<ClearGpiSmiStatus>\n"));
  // Get the PM Base Address
  PmBase = PciRead16 (PCI_LIB_ADDRESS (
            DEFAULT_PCI_BUS_NUMBER_PCH, // Bus
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_ACPI_BASE // Register
            ));

  // Base Address should be in bits 15:7 of PMBASE register 9.1.1 page 280, but 0x400 is bigger
  // so they use the whole 16 bit
  PmBase = (UINT16) (PmBase & 0xFF80);

  // caterr# connected to GPI 8
  Data16 = IoRead16 ((PmBase + R_ACPI_ALT_GP_SMI_STS));
  Data16 |= BIT8;
  IoWrite16 ((PmBase + R_ACPI_ALT_GP_SMI_STS), Data16);
}

//
// Interfaces between silicon and common module
//

BOOLEAN
PciErrIsSupported (
  VOID
  )
{
  UINT8 SystemErrorEn     = mRasSetup->SystemErrorEn;
  UINT8 IioErrorEn        = mRasSetup->Iio.IioErrorEn;
  UINT8 PcieErrorEn	      = mRasSetup->Pcie.PcieErrEn;
  RASDEBUG ((DEBUG_INFO, "<PciErrIsSupported> %d\n", (SystemErrorEn && (IioErrorEn || PcieErrorEn))));
  return (SystemErrorEn && (IioErrorEn || PcieErrorEn));
}

EFI_STATUS
PciErrEnableReporting (
    VOID
    ) 
{
  RASDEBUG ((DEBUG_INFO, "<PciErrEnableReporting>\n"));
  // Clear errors
  ClearIioErrors ();
  ClearPchErrors ();
  ClearGpiSmiStatus ();

  // Enable IO Hub(IOHub) Error Reporting (Like MCH, SNC etc). BE SURE THIS ONE CALLED FIRST
  EnableElogIoHub ();

  // Enable SMI Logic such that an SMI is generated upon any of the anticipated errors.
  EnableAsyncSmiLogic ();
  RASDEBUG ((DEBUG_ERROR, "EnableAsyncSmiLogic done \n"));
  return EFI_SUCCESS;
}

EFI_STATUS
PciErrDisableReporting (
  UINT32 DeviceType,
  UINT32 EventType,
  UINT32 *ExtData
  )
{
  RASDEBUG ((DEBUG_INFO, "<PciErrDisableReporting>\n"));
  //RASCM_TODO: Disable for iio/pcie is not implemented yet. It needs further discussion. 
  return EFI_UNSUPPORTED;
}


/**
  Checks for pending processor errors.

  @retval TRUE  - Pending error exists
          FALSE - No pending error exists

**/
BOOLEAN
PciErrCheckStatus (
  VOID
  )
{
//  RASDEBUG ((DEBUG_ERROR, "<PciErrCheckStatus>\n"));
  return   CheckForIioErrors();
}

EFI_STATUS
PciErrClearStatus (
  VOID
  ) 
{
  RASDEBUG ((DEBUG_INFO, "<PciErrClearStatus>\n"));
  //RASCM_TODO: We need a Pcie Clear Routine

  ClearIioErrors();
  ClearGpiSmiStatus ();
  ClearPchErrors();
  return EFI_SUCCESS;
}

EFI_STATUS
PciErrDetectAndHandle (
  UINT32    *CurrentSev,
  ClearOption      Clear
  ) 
{
  if(Clear == DoNotClearError) {
    //In this implementation of PCIe Error Reporting, Clear param AUTO is equal to ClearError. 
    //It does not support the DoNotClearError option.
    //RASCM_TODO: Later we can split the pcie clearing and handling flows to support this option.  
    return EFI_UNSUPPORTED;
  }
  
  RASDEBUG ((DEBUG_INFO, "<PciErrDetectAndHandle>\n"));

  mSeverity = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

  ProcessErrors();
    
  *CurrentSev = mSeverity;
  RASDEBUG ((DEBUG_INFO, "PciErrDetectAndHandle, mSeverity = %x \n", mSeverity)); 
  
  return EFI_SUCCESS;
}

/**
  Entry point for the PCI Express Error Handler. 

  This function initializes the error handling and enables PCI
  Express error sources.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.
**/
EFI_STATUS
EFIAPI
InitializePcieErrHandler (
  IN      EFI_HANDLE          ImageHandle,
  IN      EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                      Status;

  RASDEBUG ((DEBUG_ERROR, "InitializePcieErrHandler, in function 1  \n"));
  
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&mSmmBase2
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiErrorHandlingProtocolGuid,
                    NULL,
                    &mErrorHandlingProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformErrorHandlingProtocolGuid,
                    NULL,
                    &mPlatformErrorHandlingProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformRasPolicyProtocolGuid,
                    NULL,
                    &mPlatformRasPolicyProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  mRasTopology = mPlatformRasPolicyProtocol->EfiRasSystemTopology;
  mRasSetup = mPlatformRasPolicyProtocol->SystemRasSetup;
  mRasCapability = mPlatformRasPolicyProtocol->SystemRasCapability;
  
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmGpiDispatch2ProtocolGuid, 
                    NULL, 
                    &mGpiDispatch
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gSmst->SmmLocateProtocol (
                    &gEfiCpuCsrAccessGuid, 
                    NULL, 
                    &mCpuCsrAccess
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  InitPcieSiliconCap (&mRasCapability->SiliconRasCapability);
  InitErrDevInfo ();
  InitIOHInfo (); 

  if(PciErrIsSupported()) {

    PciErrEnableReporting();
    RASDEBUG ((DEBUG_ERROR, "PciErrEnableReporting,done  \n"));
    mReporter.IsSupported = PciErrIsSupported;
    mReporter.EnableReporting = PciErrEnableReporting;
    mReporter.DisableReporting = PciErrDisableReporting;
    mReporter.CheckStatus = PciErrCheckStatus;
    mReporter.ClearStatus = PciErrClearStatus;
    mReporter.DetectAndHandle = PciErrDetectAndHandle;
    
    // Register the Processor Error Handler
    Status = mErrorHandlingProtocol->RegisterHandler (&mReporter, LowPriority);
    // We only need signal NMI if IoMca is not enabled. Otherwise, MCE is signalled before NMI.
    if (!mRasSetup->Iio.IoMcaEn) {
      Status = mPlatformErrorHandlingProtocol->RegisterNotifier (PchNotifyOs, 0xff);
    }
    RASDEBUG ((DEBUG_ERROR, "InitializePcieErrHandler, done  \n"));
  } else {
    RASDEBUG ((DEBUG_INFO, "<PciErrIsNotSupported>\n"));
  }

  
  return Status;
}
