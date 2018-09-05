/** @file
  Implementation of the IOMCA Error Handler.

  Copyright (c) 2009-2017 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#include "IoMcaHandler.h"
#include <IndustryStandard/Pci30.h>
#include <Library/MmPciBaseLib.h>
#include <PciExpress.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>


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
#include "ErrorRecords.h"
#include "UncoreCommonIncludes.h"
#include "RcRegs.h"

#include <Iio/IioRegs.h>
#include <IIO_RAS.h>
#include <Library/PciLib.h>
#include <Setup/IioUniversalData.h>
#include <Library/ChipsetErrReporting.h>
#include <Library/MmPciBaseLib.h>


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
GetCapabilitiesOffset (
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
    CapabilityOffset = MmioRead8(MmPciAddress(Socket, Bus, Device, Function, PCI_CAPBILITY_POINTER_OFFSET));

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
GetExtendedCapabilitiesOffset (
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

  //  DMI AER Extended capability is in VSEC item at offset 0x148
  if (Bus == DMI_BUS_NUM && Device == DMI_DEV_NUM && ExtCapabilityId == PCIE_EXT_CAP_HEADER_AERC) {
    return 0x148;
  }

  if (GetCapabilitiesOffset(Socket, Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP)) {
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

BOOLEAN
IsHotRemoval (
  IN      UINT8   Socket,
  IN      UINT8   Bus,
  IN      UINT8   Device,
  IN      UINT8   Function
  )
{
  UINT16    Sltsts;
  UINT16    Sltcon;
  UINT32    Sltcap;
  BOOLEAN   HotPlugCap;
  BOOLEAN   PCC;
  BOOLEAN   PDS;

  Sltcap = MmioRead32 (MmPciAddress (Socket, Bus, Device, Function, 0xA4));

  HotPlugCap = ( (Sltcap & BIT6) != 0);

  if (!HotPlugCap) {
	return FALSE;
  }


  Sltsts = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, 0xAA));
  Sltcon = MmioRead16 (MmPciAddress (Socket, Bus, Device, Function, 0xA8));

  RASDEBUG((DEBUG_ERROR, "IsHotRemoval: Sltsts 0x%x, Sltcon 0x%x \n", Sltsts, Sltcon));

  PCC  = ( (Sltcon & BIT10) != 0);
  PDS  = ( (Sltsts & BIT6) != 0);

  if (PCC) {
	RASDEBUG((DEBUG_ERROR, "IsHotRemoval: Is hot removal case (PCC = 1) \n"));
	return TRUE;
  } else if (!PDS) {
	RASDEBUG((DEBUG_ERROR, "IsHotRemoval: Is hot removal case (PCC = 0) \n"));
	return TRUE;
  } else {
	return FALSE;
  }
}

EFI_STATUS
HandleIoMca (
  IN EMCA_MC_BANK_SIGNATURE * McBankSig
 )
{
  UINT16   CapOffset;
  UINT8    Segment;
  UINT8    Bus;
  UINT8    Dev;
  UINT8    Func;
  UINT16   DeviceStatus;
  UINT32   Uncerrsts;

  Segment = (McBankSig->McMisc >> 8) & 0xFF;
  Bus = (McBankSig->McMisc >> 24) & 0xFF;
  Dev = (McBankSig->McMisc >> 19) & 0x1F;
  Func = (McBankSig->McMisc >> 16) & 0x7;

  RASDEBUG((DEBUG_ERROR, "HandleIoMca: Misc: 0x%x, Segment: 0x%x, Bus: 0x%x, Dev: 0x%x, Func: 0x%x \n", McBankSig->McMisc, Segment, Bus, Dev, Func));
  // Get the capabilities offset
  CapOffset = GetCapabilitiesOffset (Segment, Bus, Dev, Func, EFI_PCI_CAPABILITY_ID_PCIEXP);

  // Check if the device status indicates that a NON-FATAL error occurred
  DeviceStatus = MmioRead16(MmPciAddress (Segment, Bus, Dev, Func, CapOffset + PCIE_DEVICE_STATUS_OFFSET));

  RASDEBUG((DEBUG_ERROR, "HandleIoMca: DEVSTS: 0x%x \n", DeviceStatus));

  if (! (DeviceStatus & B_PCIE_DEVICE_STATUS_NF_ERR) ) {
	RASDEBUG((DEBUG_ERROR, "HandleIoMca: NF status not found. \n"));
    return EFI_NOT_FOUND;
  }

  CapOffset = GetExtendedCapabilitiesOffset (Segment, Bus, Dev, Func, PCIE_EXT_CAP_HEADER_AERC, 0xff);
  if (CapOffset == 0) {
	RASDEBUG((DEBUG_ERROR, "HandleIoMca: AER Capability not found. \n"));
	return EFI_NOT_FOUND;
  }

  Uncerrsts = MmioRead32 (MmPciAddress (Segment, Bus, Dev, Func, CapOffset + PCIE_AER_UNCERRSTS_OFFSET));
  RASDEBUG((DEBUG_ERROR, "HandleIoMca: UNCERRSTS: 0x%x \n", Uncerrsts));

  if ( (Uncerrsts & BIT14) && IsHotRemoval (Segment, Bus, Dev, Func)) {
	MmioWrite32 (MmPciAddress (Segment, Bus, Dev, Func, CapOffset + PCIE_AER_UNCERRSTS_OFFSET), Uncerrsts);
	Uncerrsts = MmioRead32 (MmPciAddress (Segment, Bus, Dev, Func, CapOffset + PCIE_AER_UNCERRSTS_OFFSET));
	RASDEBUG((DEBUG_ERROR, "HandleIoMca: CTO found. After clearing UNCERRSTS: 0x%x \n", Uncerrsts));
	return EFI_SUCCESS;
  }

  RASDEBUG((DEBUG_ERROR, "HandleIoMca: Not a CTO case. \n"));
  return EFI_NOT_FOUND;
}
