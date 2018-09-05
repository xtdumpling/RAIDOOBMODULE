//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This file implement runtime library functions for creating Whea 
  error logs. 

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <Library/WheaErrorLib.h>

extern EFI_GUID gEfiWheaPlatformNonStandardErrorSectionGuid;

#define EFI_PCI_BRIDGE_DEV_CLASS_CODE               0x06040000
#define EFI_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE   0x06040100
#define AER_CAPABILITY_SIZE                         0x38

//
// PCIe Enhanced Capabilities...
//
#define PCIE_ENHANCED_CAPABILITY_PTR                0x0100

//
// Common Enhanced Capability Headers...
//

//
// Invalid header, used for terminating list...
//
#define PCIE_EXT_CAP_HEADER_INVALID                 0xFFFF

//
//Advanced Error Reporting Enhanced Capability Header...
//
#define PCIE_EXT_CAP_HEADER_AERC                    0x0001

#pragma optimize("", off)

//
// Data definitions
//

EFI_GUID
ZeroGuid    = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

EFI_STATUS
WheaGenElogMemory(
    IN UINT16                  ErrorType,
    IN MEMORY_DEV_INFO         *MemInfo,
    IN GENERIC_ERROR_STATUS    *ErrStsBlk
  )
/*++

--*/
{
  GENERIC_ERROR_DATA_ENTRY    *NextErrEntry;
  PLATFORM_MEMORY_ERROR_DATA_UEFI_231  *NextErrData;

  if (ErrStsBlk == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // As per Microsoft, Can report one error data entry at a time.
  // If bit 3 of BlockStatus is set, it merely indicates that more 
  //  than one error was detected. It doesn’t mean that more than one 
  //  error is reported in the status block. Think of it in the same 
  //  way as the overflow bit in the MCA status registers.
  //
  ZeroMem((VOID *)ErrStsBlk, sizeof(GENERIC_ERROR_STATUS) + ErrStsBlk->ErrDataSize);

  if (ErrorType == GEN_ERR_SEV_PLATFORM_MEMORY_FATAL) {

    // If already more than one error data log, set multiple
    //
// APTIOV_SERVER_OVERRIDE_RC_START : Commented the code to set Multiple Error Valid Bit
      //if (ErrStsBlk->BlockStatus.UeValid != 1) {
        ErrStsBlk->BlockStatus.UeValid = 1;
    //  } else {
     //   ErrStsBlk->BlockStatus.MultipleUeValid = 1;
    //  }
    } else {
    //  if (ErrStsBlk->BlockStatus.CeValid != 1) {
        ErrStsBlk->BlockStatus.CeValid = 1;
  //    } else {
   //     ErrStsBlk->BlockStatus.MultipleCeValid = 1;
   //   }
// APTIOV_SERVER_OVERRIDE_RC_END : Commented the code to set Multiple Error Valid Bit
  }

  // Calculate offset for next error data log
  //
  NextErrEntry = (GENERIC_ERROR_DATA_ENTRY *) ((UINTN)ErrStsBlk + sizeof(GENERIC_ERROR_STATUS) + 
                  ErrStsBlk->ErrDataSize);
  NextErrData = (PLATFORM_MEMORY_ERROR_DATA_UEFI_231 *) ((UINTN)NextErrEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));

  // Set Error status block data
  //
  ErrStsBlk->RawDataOffset = 0;                   // No Raw data for our platform memory
  ErrStsBlk->RawDataSize = 0;
  ErrStsBlk->BlockStatus.NumErrorDataEntry++;

  // Adjust Error status block error data size to add new error log
  //
  if (MemInfo->UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
      ErrStsBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PLATFORM_MEMORY_ERROR_DATA_UEFI_231);
  } else {
    ErrStsBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PLATFORM_MEMORY_ERROR_DATA);
  }

  // Set error section type GUID
  //
  CopyMem(&NextErrEntry->SectionType, &gEfiWheaPlatformMemoryErrorSectionGuid, sizeof(EFI_GUID));
  if (MemInfo->UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
      NextErrEntry->Revision = GENERIC_ERROR_SECTION_REVISION_UEFI231;
  } else {
    NextErrEntry->Revision = GENERIC_ERROR_SECTION_REVISION;
  }

  if (ErrorType == GEN_ERR_SEV_PLATFORM_MEMORY_FATAL) {
    ErrStsBlk->Severity       = GENERIC_ERROR_FATAL;
    NextErrEntry->Severity    = GENERIC_ERROR_FATAL;
  } else if (ErrorType == GEN_ERR_SEV_PLATFORM_MEMORY_RECOVERABLE){
    ErrStsBlk->Severity       = GENERIC_ERROR_RECOVERABLE;
    NextErrEntry->Severity    = GENERIC_ERROR_RECOVERABLE;
  } else {
    ErrStsBlk->Severity       = GENERIC_ERROR_CORRECTED;
    NextErrEntry->Severity    = GENERIC_ERROR_CORRECTED;
  }

  // Get FRU data from Platform hook
  //
  WheaGetMemoryFruInfo(MemInfo, &NextErrEntry->FruId, &NextErrEntry->FruString[0], &NextErrData->Card);
  if (!CompareGuid(&NextErrEntry->FruId, &ZeroGuid)) {
      NextErrEntry->SecValidMask.FruIdValid = 1;
  }

  if(NextErrEntry->FruString[0] != 0) {
      NextErrEntry->SecValidMask.FruStringValid = 1;
  }

  NextErrEntry->SectionFlags.Primary = 1; //0;         // default for our platforms
  if (MemInfo->UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
      NextErrEntry->DataSize = sizeof(PLATFORM_MEMORY_ERROR_DATA_UEFI_231);
  } else {
    NextErrEntry->DataSize = sizeof(PLATFORM_MEMORY_ERROR_DATA);
  }

  // Set valid Error data fields
  //
// APTIOV_SERVER_OVERRIDE_RC_START : Support added for Mode1 Memory Error Reporting
  if (MemInfo->ValidBits)  {
        NextErrData->ValidFields = MemInfo->ValidBits;
  } else {
        NextErrData->ValidFields = 0;
// APTIOV_SERVER_OVERRIDE_RC_END : Support added for Mode1 Memory Error Reporting
  }

  // Set Error status as error in memory device
  //
  NextErrData->ErrorStatus.Type = ErrorMemStorage;

  NextErrData->PhysicalAddress  = MemInfo->PhyAddr;
// APTIOV_SERVER_OVERRIDE_RC_START : Support added for Mode1 Memory Error Reporting
  NextErrData->PhysicalAddressMask = MemInfo->PhyAddrMask;
  NextErrData->BitPosition = MemInfo->BitPosition;
// APTIOV_SERVER_OVERRIDE_RC_END : Support added for Mode1 Memory Error Reporting
  NextErrData->Node       = MemInfo->Node;
  NextErrData->ModuleRank = MemInfo->Dimm;
  NextErrData->Device     = MemInfo->Device;
  NextErrData->Bank       = MemInfo->Bank;
  NextErrData->Row        = MemInfo->Row;           // Module or Rank#
  NextErrData->Column     = MemInfo->Column;
  NextErrData->ErrorType  = (UINT8) MemInfo->ErrorType;

  if (MemInfo->UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    NextErrData->RankNumber         = (UINT8) MemInfo->Rank;
    NextErrData->SmBiosCardHandle   = MemInfo->SmBiosCardHandle;    // Type 16 handle;
    NextErrData->SmBiosModuleHandle = MemInfo->SmBiosModuleHandle;  // Type 17 handle;
  }

  return EFI_SUCCESS;
}

STATIC EFI_STATUS
PcieGetCapabilities (
    IN OUT PCIE_CAPABILITY *CapBuf,
    IN UINT8   Segment,
    IN UINT8   Bus,
    IN UINT8   Device,
    IN UINT8   Function
);

STATIC EFI_STATUS
PcieGetAer (
    IN OUT PCIE_AER *AerBuf,
    IN UINT8   Segment,
    IN UINT8   Bus,
    IN UINT8   Device,
    IN UINT8   Function
) ;

EFI_STATUS
WheaGenElogPcieRootDevBridge(
    IN UINT16                      ErrorType,
    IN PCIE_PCI_DEV_INFO           *ErrPcieDev,
    IN GENERIC_ERROR_STATUS        *ErrStsBlk
  )
/*++
 
--*/
{
  GENERIC_ERROR_DATA_ENTRY    *NextErrEntry;
  PCIE_ERROR_DATA             *NextErrData;

  UINT8 Data8;
  UINT16 Data16;
  UINT32 Data32;

  Data8  = 0;
  Data16 = 0;
  Data32 = 0;

  if (ErrorType != GEN_ERR_SEV_PCIE_CORRECTED &&
      ErrorType != GEN_ERR_SEV_PCIE_FATAL &&
      ErrorType != GEN_ERR_SEV_PCIE_RECOVERABLE
      )
  {
    return EFI_NOT_FOUND;
  }

  if (ErrStsBlk == NULL) {
      return EFI_NOT_FOUND;
  }

  //
  // As per Microsoft, Can report one error data entry at a time.
  // If bit 3 of BlockStatus is set, it merely indicates that more 
  //  than one error was detected. It doesn’t mean that more than one 
  //  error is reported in the status block. Think of it in the same 
  //  way as the overflow bit in the MCA status registers.
  //
  ZeroMem((VOID *)ErrStsBlk, sizeof(GENERIC_ERROR_STATUS) + ErrStsBlk->ErrDataSize);

  // Calculate offset for next error data log
  //
  NextErrEntry = (GENERIC_ERROR_DATA_ENTRY *) ((UINTN)ErrStsBlk + sizeof(GENERIC_ERROR_STATUS) + 
                    ErrStsBlk->ErrDataSize);
  NextErrData = (PCIE_ERROR_DATA *) ((UINTN)NextErrEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));

  // Set Error status block data
  //
  ErrStsBlk->RawDataOffset = 0;                   // No Raw data for our platform memory
  ErrStsBlk->RawDataSize = 0;
  ErrStsBlk->BlockStatus.NumErrorDataEntry++;

  // Adjust Error status block error data size to add new error log
  //
  ErrStsBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PCIE_ERROR_DATA);

  // If already more than one error data log, set multiple
  //
  if ( ErrorType == GEN_ERR_SEV_PCIE_RECOVERABLE || ErrorType == GEN_ERR_SEV_PCIE_CORRECTED) {
// APTIOV_SERVER_OVERRIDE_RC_START : Commented the code to set Multiple Error Valid Bit
//      if (ErrStsBlk->BlockStatus.CeValid != 1) {
      ErrStsBlk->BlockStatus.CeValid = 1;
//      } else {
//        ErrStsBlk->BlockStatus.MultipleCeValid = 1;
//      }
    ErrStsBlk->Severity = GENERIC_ERROR_CORRECTED;
    NextErrEntry->Severity = GENERIC_ERROR_CORRECTED;
  } else {
//      if (ErrStsBlk->BlockStatus.UeValid != 1) {
      ErrStsBlk->BlockStatus.UeValid = 1;
//      } else {
//        ErrStsBlk->BlockStatus.MultipleUeValid = 1;
//      }
// APTIOV_SERVER_OVERRIDE_RC_END : Commented the code to set Multiple Error Valid Bit
    ErrStsBlk->Severity = GENERIC_ERROR_FATAL;
    NextErrEntry->Severity = GENERIC_ERROR_FATAL;
  }

  NextErrEntry->Revision = GENERIC_ERROR_SECTION_REVISION;
  NextErrEntry->SectionFlags.Primary = 1;//0;         // default for our platforms
  NextErrEntry->DataSize = sizeof(PCIE_ERROR_DATA);
    
  // Set error section type GUID
  //
  CopyMem(&NextErrEntry->SectionType, &gEfiWheaPcieErrorSectionGuid, sizeof(EFI_GUID));

  // Set valid Error data fields
  //
  NextErrData->ValidFields = PCIE_ERROR_PORT_TYPE_VALID | PCIE_ERROR_VERSION_VALID \
                                | PCIE_ERROR_COMMAND_STATUS_VALID | PCIE_ERROR_DEVICE_ID_VALID;

  //
  // Populate PCIe error information. 
  // NOTE: SHOULD NOT CLEAR ANY ERROR STATUS. CALLER WIL CLEAR ERROR STATUS
  //
  NextErrData->PortType = 4; //??? // root port
  NextErrData->Version = PCIE_SPECIFICATION_SUPPORTED;

  Data16 = PciExpressRead16( EFI_PCI_WHEA_ADDRESS(
               ErrPcieDev->Segment,
               ErrPcieDev->Bus,
               ErrPcieDev->Device,
               ErrPcieDev->Function,
               PCI_COMMAND_OFFSET
              )
           );

  Data32 = (UINT32)Data16<<16;
  Data16 = PciExpressRead16( EFI_PCI_WHEA_ADDRESS(
               ErrPcieDev->Segment,
               ErrPcieDev->Bus,
               ErrPcieDev->Device,
               ErrPcieDev->Function,
               PCI_PRIMARY_STATUS_OFFSET
              )
           );

  NextErrData->CommandStatus = Data32|Data16; // read command & status reg
  Data16 = PciExpressRead16( EFI_PCI_WHEA_ADDRESS(
               ErrPcieDev->Segment,
               ErrPcieDev->Bus,
               ErrPcieDev->Device,
               ErrPcieDev->Function,
               PCI_VENDOR_ID_OFFSET
             )
           );
  NextErrData->DevBridge.VendorId = Data16;

  Data16 = PciExpressRead16( EFI_PCI_WHEA_ADDRESS(
               ErrPcieDev->Segment,
               ErrPcieDev->Bus,
               ErrPcieDev->Device,
               ErrPcieDev->Function,
               PCI_DEVICE_ID_OFFSET
               )
           );
  NextErrData->DevBridge.DeviceId = Data16;

/*
  Data16 = PciExpressRead16( EFI_PCI_WHEA_ADDRESS(
               ErrPcieDev->Segment,
               ErrPcieDev->Bus,
               ErrPcieDev->Device,
               ErrPcieDev->Function,
               PCI_CLASSCODE_OFFSET
               )

           );
  NextErrData->DevBridge.ClassCode = Data16;
*/

  NextErrData->DevBridge.Function = ErrPcieDev->Function;
  NextErrData->DevBridge.Device = ErrPcieDev->Device;
  NextErrData->DevBridge.Segment = ErrPcieDev->Segment;
  NextErrData->DevBridge.PrimaryOrDeviceBus = ErrPcieDev->Bus;

  // Check if PCI bridge
  //
  Data32 = PciExpressRead32( EFI_PCI_WHEA_ADDRESS (
               ErrPcieDev->Segment,
               ErrPcieDev->Bus,
               ErrPcieDev->Device,
               ErrPcieDev->Function,
               PCI_REVISION_ID_OFFSET
               )
           );

  NextErrData->DevBridge.ClassCode[0] = (UINT8)(Data32 >> 0x18);
  NextErrData->DevBridge.ClassCode[1] = (UINT8)(Data32 >> 0x10);
  NextErrData->DevBridge.ClassCode[2] = (UINT8)(Data32 >> 0x8);
  

  Data32 &= 0xFFFFFF00;

  if ( (Data32 == EFI_PCI_BRIDGE_DEV_CLASS_CODE) ||
          (Data32 == EFI_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE ) ) {

    //
    //Yes, it is PCI bridge
    //Fill bridge information
    //
    Data8 = PciExpressRead8( EFI_PCI_WHEA_ADDRESS (
                ErrPcieDev->Segment,
                ErrPcieDev->Bus,
                ErrPcieDev->Device,
                ErrPcieDev->Function,
                PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET
                )
         );

    NextErrData->DevBridge.PrimaryOrDeviceBus = Data8;

    Data8 = PciExpressRead8( EFI_PCI_WHEA_ADDRESS (
                ErrPcieDev->Segment,
                ErrPcieDev->Bus,
                ErrPcieDev->Device,
                ErrPcieDev->Function,
                PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                )
         );

    NextErrData->DevBridge.SecondaryBus = Data8;

    Data16 = PciExpressRead16( EFI_PCI_WHEA_ADDRESS(
                 ErrPcieDev->Segment,
                 ErrPcieDev->Bus,
                 ErrPcieDev->Device,
                 ErrPcieDev->Function,
                 PCI_BRIDGE_CONTROL_REGISTER_OFFSET
                 )
         );

    Data32 = (UINT32) Data16<<16;

    Data16 = PciExpressRead16( EFI_PCI_WHEA_ADDRESS(
                 ErrPcieDev->Segment,
                 ErrPcieDev->Bus,
                 ErrPcieDev->Device,
                 ErrPcieDev->Function,
                 PCI_BRIDGE_STATUS_REGISTER_OFFSET
                 )
         );

    NextErrData->BridgeControlStatus = Data32| Data16;

    // Set Bridge information valid 
    //
    NextErrData->ValidFields |= PCIE_ERROR_BRIDGE_CRL_STS_VALID;
  } //Pci Bridge

  NextErrData->SerialNo = 0;
  PcieGetCapabilities(
      &(NextErrData->Capability),
      ErrPcieDev->Segment,
      ErrPcieDev->Bus,
      ErrPcieDev->Device,
      ErrPcieDev->Function
      );

  NextErrData->ValidFields |= PCIE_ERROR_CAPABILITY_INFO_VALID;
  PcieGetAer(
      &(NextErrData->AerInfo),
      ErrPcieDev->Segment,
      ErrPcieDev->Bus,
      ErrPcieDev->Device,
      ErrPcieDev->Function
      );

  NextErrData->ValidFields |= PCIE_ERROR_AER_INFO_VALID;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS
PcieGetCapabilities (
    IN OUT  PCIE_CAPABILITY *CapBuf,
    IN UINT8   Segment,
    IN UINT8   Bus,
    IN UINT8   Device,
    IN UINT8   Function
)
/*++

--*/
{
  UINT8   PciPrimaryStatus;
  UINT8   CapabilityOffset;
  UINT8   CapId;
  UINT8  *CapPointer;

  CapPointer = CapBuf->PcieCap;

  PciPrimaryStatus = PciExpressRead8( EFI_PCI_WHEA_ADDRESS(Segment, Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY) {
    CapabilityOffset = PciExpressRead8( EFI_PCI_WHEA_ADDRESS(Segment,Bus, Device, Function, PCI_CAPBILITY_POINTER_OFFSET));
    do {
      CapId = PciExpressRead8( EFI_PCI_WHEA_ADDRESS(Segment,Bus, Device, Function, CapabilityOffset));

      *CapPointer++  =  CapId;
      CapabilityOffset = PciExpressRead8( EFI_PCI_WHEA_ADDRESS(Segment,Bus, Device, Function, CapabilityOffset + 1));
    } while (CapabilityOffset != 0);

    return EFI_SUCCESS;
  } else {
      return EFI_UNSUPPORTED;
  }
}

typedef struct {
  UINT32                ExtCapId:16;
  UINT32                ChanelVersion:4;
  UINT32                CapabilityOffset:12;
} PCIE_EXT_CAP;


STATIC EFI_STATUS
PcieGetAer (
    IN OUT PCIE_AER *AerBuf,
    IN UINT8   Segment,
    IN UINT8   Bus,
    IN UINT8   Device,
    IN UINT8   Function
)
/*++

--*/
{

  UINT8                       PciPrimaryStatus;
  UINT16                      CapabilityOffset;
  UINT16                      CurCapabilityOffset = 00;
  PCIE_EXT_CAP                *ExtCapability;
  UINT32                      ExtCapabilityData;
  UINT8                       *AerPointer;
  UINT16                      ExtCapId =0;
  UINT16                      Index=0;

  PciPrimaryStatus = PciExpressRead8( EFI_PCI_WHEA_ADDRESS(Segment,Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if (PciPrimaryStatus & EFI_PCI_STATUS_CAPABILITY) {
    ExtCapabilityData = PciExpressRead32( EFI_PCI_WHEA_ADDRESS(Segment,Bus, Device, Function, PCIE_ENHANCED_CAPABILITY_PTR));

    ExtCapability =(PCIE_EXT_CAP  *)&ExtCapabilityData;
    CapabilityOffset = (UINT16)ExtCapability->CapabilityOffset;
    if (CapabilityOffset == 0 )
      return EFI_UNSUPPORTED;
    CurCapabilityOffset =PCIE_ENHANCED_CAPABILITY_PTR;

    do {
      ExtCapId = (UINT16)ExtCapability->ExtCapId;

      if (ExtCapId == PCIE_EXT_CAP_HEADER_AERC)
        break;

      ExtCapabilityData = PciExpressRead32( EFI_PCI_WHEA_ADDRESS(Segment,Bus, Device, Function, CapabilityOffset));
      ExtCapability =(PCIE_EXT_CAP  *)&ExtCapabilityData;
      CurCapabilityOffset = CapabilityOffset;
      CapabilityOffset = (UINT16)ExtCapability->CapabilityOffset;

    } while (CapabilityOffset != 0 && ExtCapability->ExtCapId != PCIE_EXT_CAP_HEADER_INVALID);

    if (ExtCapId == PCIE_EXT_CAP_HEADER_AERC) {
      //
      // Copy AER to buffer
      //
      AerPointer = (UINT8*)AerBuf->PcieAer;
      for(Index =0; Index<AER_CAPABILITY_SIZE; Index++) {
        *AerPointer++ = PciExpressRead8( EFI_PCI_WHEA_ADDRESS(Segment,Bus, Device, Function, CurCapabilityOffset + Index));
      }
      return EFI_SUCCESS;
    } else {
      return EFI_NOT_FOUND;
    }
  } else {
    return EFI_UNSUPPORTED;
  }

}

EFI_STATUS
WheaGenElogPciDev(
    IN UINT16                      ErrorType,
    IN PCIE_PCI_DEV_INFO           *ErrPcieDev,
    IN GENERIC_ERROR_STATUS        *ErrStsBlk
  )
/*++

--*/
{
  GENERIC_ERROR_DATA_ENTRY    *NextErrEntry;
  PCI_PCIX_DEVICE_ERROR_DATA  *NextErrData;

  UINT8                       Data8;
  UINT16                      Data16;
  UINT32                      Data32;

  Data8  = 0;
  Data16 = 0;
  Data32 = 0;

  if (ErrorType !=GEN_ERR_SEV_PCI_DEV_CORRECTED  &&
      ErrorType != GEN_ERR_SEV_PCI_DEV_FATAL &&
      ErrorType != GEN_ERR_SEV_PCI_DEV_RECOVERABLE ) {
    return EFI_NOT_FOUND;
  }

  if (ErrStsBlk == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // As per Microsoft, Can report one error data entry at a time.
  // If bit 3 of BlockStatus is set, it merely indicates that more 
  //  than one error was detected. It doesn’t mean that more than one 
  //  error is reported in the status block. Think of it in the same 
  //  way as the overflow bit in the MCA status registers.
  //
  ZeroMem((VOID *)ErrStsBlk, sizeof(GENERIC_ERROR_STATUS) + ErrStsBlk->ErrDataSize);

  // Calculate offset for next error data log
  //
  NextErrEntry = (GENERIC_ERROR_DATA_ENTRY *) ((UINTN)ErrStsBlk + sizeof(GENERIC_ERROR_STATUS) + ErrStsBlk->ErrDataSize);
  NextErrData = (PCI_PCIX_DEVICE_ERROR_DATA *) ((UINTN)NextErrEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));

  // Set Error status block data
  //
  ErrStsBlk->RawDataOffset = 0;     // No Raw data for our platform memory

  ErrStsBlk->RawDataSize = 0;

  ErrStsBlk->BlockStatus.NumErrorDataEntry++;

  // Adjust Error status block error data size to add new error log
  //
  ErrStsBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PCI_PCIX_DEVICE_ERROR_DATA);

  // If already more than one error data log, set multiple
  //
  if (ErrorType == GEN_ERR_SEV_PCI_DEV_RECOVERABLE || ErrorType == GEN_ERR_SEV_PCI_DEV_CORRECTED) {
// APTIOV_SERVER_OVERRIDE_RC_START : Commented the code to set Multiple Error Valid Bit
//    if (ErrStsBlk->BlockStatus.CeValid != 1) {
      ErrStsBlk->BlockStatus.CeValid = 1;
//    } else {
//      ErrStsBlk->BlockStatus.MultipleCeValid = 1;
//    }
    ErrStsBlk->Severity = GENERIC_ERROR_CORRECTED;
    NextErrEntry->Severity = GENERIC_ERROR_CORRECTED;
  } else {
//    if (ErrStsBlk->BlockStatus.UeValid != 1) {
      ErrStsBlk->BlockStatus.UeValid = 1;
//    } else {
//      ErrStsBlk->BlockStatus.MultipleUeValid = 1;
//    }
// APTIOV_SERVER_OVERRIDE_RC_END : Commented the code to set Multiple Error Valid Bit
    ErrStsBlk->Severity = GENERIC_ERROR_FATAL;
    NextErrEntry->Severity = GENERIC_ERROR_FATAL;
  }

  NextErrEntry->Revision = GENERIC_ERROR_SECTION_REVISION;
  NextErrEntry->SectionFlags.Primary = 1;//0;   // default for our platforms
  NextErrEntry->DataSize = sizeof(PCI_PCIX_DEVICE_ERROR_DATA);

  // Set error section type GUID
  //
  CopyMem(&NextErrEntry->SectionType, &gEfiWheaPciDevErrorSectionGuid, sizeof(EFI_GUID));

  // Set valid Error data fields
  //
  //Palsamy check pci device valid fields
  //
// APTIOV_SERVER_OVERRIDE_RC_START : Filling required PCI/PCIx Device Error Valid Bits.
  NextErrData->ValidFields = PCI_PCIX_DEVICE_ID_INFO_VALID;
// APTIOV_SERVER_OVERRIDE_RC_END : Filling required PCI/PCIx Device Error Valid Bits.

  NextErrData->DeviceId.Segment=ErrPcieDev->Segment;
  NextErrData->DeviceId.Bus= ErrPcieDev->Bus;
  NextErrData->DeviceId.Device = ErrPcieDev->Device;
  NextErrData->DeviceId.Function=ErrPcieDev->Function;

  Data16 = PciExpressRead16(
                EFI_PCI_WHEA_ADDRESS(
                    ErrPcieDev->Segment,
                    ErrPcieDev->Bus,
                    ErrPcieDev->Device,
                    ErrPcieDev->Function,
                    PCI_VENDOR_ID_OFFSET
                    )
                );
  NextErrData->DeviceId.VendorId = Data16;

  Data16 = PciExpressRead16(
                EFI_PCI_WHEA_ADDRESS(
                    ErrPcieDev->Segment,
                    ErrPcieDev->Bus,
                    ErrPcieDev->Device,
                    ErrPcieDev->Function,
                    PCI_DEVICE_ID_OFFSET
                    )
                );
  NextErrData->DeviceId.DeviceId = Data16;
// APTIOV_SERVER_OVERRIDE_RC_START

  // Modified the code to read configuration space in 64 bit boundary as otherwise it asserts.
  #if 0
  Data16 = PciExpressRead16(
                EFI_PCI_WHEA_ADDRESS(
                    ErrPcieDev->Segment,
                    ErrPcieDev->Bus,
                    ErrPcieDev->Device,
                    ErrPcieDev->Function,
                    PCI_CLASSCODE_OFFSET
                    )
                );
  NextErrData->DeviceId.ClassCode = Data16;
  #endif
  Data32 = PciExpressRead32( EFI_PCI_WHEA_ADDRESS (
               ErrPcieDev->Segment,
               ErrPcieDev->Bus,
               ErrPcieDev->Device,
               ErrPcieDev->Function,
               PCI_REVISION_ID_OFFSET
               )
           );

  NextErrData->DeviceId.ClassCode = (UINT16)(Data32 >> 0x10);

//  Filling required PCI/PCIx Device Error Valid Bits.
//  NextErrData->ValidFields |= PCI_PCIX_BUS_ERROR_BUS_ID_VALID; //|PCI_PCIX_BUS_ERROR_BUS_ADDRESS_VALID;

// APTIOV_SERVER_OVERRIDE_RC_END
  //
  // TODO:
  //

  NextErrData->ErrorStatus = 0;
  NextErrData->IOResgisters =0;
  NextErrData->MemoryRegisters =0;
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done for proper calculation of Lenght for PCI/PCIx Device Error
  NextErrData->RegisterData[0] =0;
  NextErrData->RegisterData[1] =0;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done for proper calculation of Lenght for PCI/PCIx Device Error
  NextErrData->ValidFields |= 0;

  return EFI_SUCCESS;
}

EFI_STATUS
WheaGenElogProcessor(
    IN UINT16                  ErrorType,
    IN CPU_DEV_INFO            *CpuInfo,
    IN GENERIC_ERROR_STATUS    *ErrStsBlk
  )
/*++

--*/
{
  GENERIC_ERROR_DATA_ENTRY      *NextErrEntry;
  PROCESSOR_GENERIC_ERROR_DATA  *NextErrData;

  if (ErrStsBlk == NULL) {
    return EFI_NOT_FOUND;
  }

  if (ErrorType == GEN_ERR_SEV_CPU_GENERIC_FATAL || ErrorType == GEN_ERR_SEV_CPU_SPECIFIC_FATAL) {

    // If already more than one error data log, set multiple
    //
    if (ErrStsBlk->BlockStatus.UeValid != 1) {
      ErrStsBlk->BlockStatus.UeValid = 1;
    } else {
      ErrStsBlk->BlockStatus.MultipleUeValid = 1;
    }
  } else {
    if (ErrStsBlk->BlockStatus.CeValid != 1) {
      ErrStsBlk->BlockStatus.CeValid = 1;
    } else {
      ErrStsBlk->BlockStatus.MultipleCeValid = 1;
    }
  }

  // Calculate offset for next error data log
  //
  NextErrEntry = (GENERIC_ERROR_DATA_ENTRY *) ((UINTN)ErrStsBlk + sizeof(GENERIC_ERROR_STATUS) + 
                  ErrStsBlk->ErrDataSize);
  NextErrData = (PROCESSOR_GENERIC_ERROR_DATA *) ((UINTN)NextErrEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));

  // Set Error status block data
  //
  ErrStsBlk->RawDataOffset = 0;
  ErrStsBlk->RawDataSize = 0;
  ErrStsBlk->BlockStatus.NumErrorDataEntry++;

  // Adjust Error status block error data size to add new error log
  //
  ErrStsBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PROCESSOR_GENERIC_ERROR_DATA);

  // Set error section type GUID
  //
  CopyMem(&NextErrEntry->SectionType, &gEfiWheaProcessorGenericErrorSectionGuid, sizeof(EFI_GUID));
  NextErrEntry->Revision = GENERIC_ERROR_SECTION_REVISION;

  if (ErrorType == GEN_ERR_SEV_CPU_GENERIC_FATAL) {
    ErrStsBlk->Severity       = GENERIC_ERROR_FATAL;
    NextErrEntry->Severity    = GENERIC_ERROR_FATAL;
  } else {
    ErrStsBlk->Severity       = GENERIC_ERROR_CORRECTED;
    NextErrEntry->Severity    = GENERIC_ERROR_CORRECTED;
  }

  NextErrEntry->SectionFlags.Primary = 1; //0;         // default for our platforms
  NextErrEntry->DataSize = sizeof(PROCESSOR_GENERIC_ERROR_DATA);

  // Set valid Error data fields
  //
  NextErrData->ValidFields = GEN_ERR_PROC_TYPE_VALID |GEN_ERR_PROC_ISA_VALID | GEN_ERR_PROC_OPERATION_VALID | \
                             GEN_ERR_PROC_ERROR_TYPE_VALID | GEN_ERR_PROC_ID_VALID;
//
// TODO : Need to port other parameters of PROCESSOR_GENERIC_ERROR_DATA
//
    NextErrData->Type       = CpuInfo->ProcessorType;
    NextErrData->Isa        = CpuInfo->ProcessorISA;
    NextErrData->ErrorType  = CpuInfo->ProcessorErrorType;
    NextErrData->Operation  = CpuInfo->Operation;
    NextErrData->ApicId     = CpuInfo->ProcessorApicId;
    return EFI_SUCCESS;
}

EFI_STATUS
WheaGenElogIohDev (
    IN UINT16                      ErrorType,
    IN PCIE_PCI_DEV_INFO           *ErrIohDev,
    IN GENERIC_ERROR_STATUS        *ErrStsBlk,
    IN UINT32                      ErrorSource
  )
/*++

Routine Description:

  IOH Core Whea Error Log routine
 
--*/
{
  GENERIC_ERROR_DATA_ENTRY    *NextErrEntry;
  IOH_ERROR_DATA              *NextErrData;

  UINT8 Data8;
  UINT16 Data16;
  UINT32 Data32;

  Data8  = 0;
  Data16 = 0;
  Data32 = 0;

  if (ErrorType != GEN_ERR_SEV_IOH_CORRECTED &&
      ErrorType != GEN_ERR_SEV_IOH_FATAL &&
      ErrorType != GEN_ERR_SEV_IOH_RECOVERABLE
     ) {
    return EFI_NOT_FOUND;
  }

  //
  // As per Microsoft, Can report one error data entry at a time.
  // If bit 3 of BlockStatus is set, it merely indicates that more 
  //  than one error was detected. It doesn’t mean that more than one 
  //  error is reported in the status block. Think of it in the same 
  //  way as the overflow bit in the MCA status registers.
  //
  ZeroMem((VOID *)ErrStsBlk, sizeof(GENERIC_ERROR_STATUS) + ErrStsBlk->ErrDataSize);

  //
  // Calculate offset for next error data log
  //
  NextErrEntry = (GENERIC_ERROR_DATA_ENTRY *) ((UINTN)ErrStsBlk + sizeof(GENERIC_ERROR_STATUS) + 
                  ErrStsBlk->ErrDataSize);
  NextErrData = (IOH_ERROR_DATA *) ((UINTN)NextErrEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));

  //
  // Set Error status block data
  //
  ErrStsBlk->RawDataOffset = 0;                   // No Raw data for our platform memory
  ErrStsBlk->RawDataSize = 0;
  ErrStsBlk->BlockStatus.NumErrorDataEntry++;

  //
  // Adjust Error status block error data size to add new error log
  //
  ErrStsBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PCIE_ERROR_DATA);

  //
  // If already more than one error data log, set multiple
  //
  if ( ErrorType == GEN_ERR_SEV_IOH_RECOVERABLE || ErrorType == GEN_ERR_SEV_IOH_CORRECTED) {
    if (ErrStsBlk->BlockStatus.CeValid != 1) {
      ErrStsBlk->BlockStatus.CeValid = 1;
    } else {
      ErrStsBlk->BlockStatus.MultipleCeValid = 1;
    }
    ErrStsBlk->Severity = GENERIC_ERROR_CORRECTED;
    NextErrEntry->Severity = GENERIC_ERROR_CORRECTED;
  } else {
    if (ErrStsBlk->BlockStatus.UeValid != 1) {
      ErrStsBlk->BlockStatus.UeValid = 1;
    } else {
      ErrStsBlk->BlockStatus.MultipleUeValid = 1;
    }
    ErrStsBlk->Severity = GENERIC_ERROR_FATAL;
    NextErrEntry->Severity = GENERIC_ERROR_FATAL;
  }
  NextErrEntry->Revision = GENERIC_ERROR_SECTION_REVISION;
  NextErrEntry->SectionFlags.Primary = 1;//0;         // default for our platforms
  NextErrEntry->DataSize = sizeof(PCIE_ERROR_DATA);

  //    
  // Set error section type GUID
  //
  CopyMem(&NextErrEntry->SectionType, &gEfiWheaPcieErrorSectionGuid, sizeof(EFI_GUID));

  //
  // Set valid Error data fields
  //
  NextErrData->ValidFields = PCIE_ERROR_PORT_TYPE_VALID | PCIE_ERROR_VERSION_VALID \
                              | PCIE_ERROR_COMMAND_STATUS_VALID | PCIE_ERROR_DEVICE_ID_VALID;

  //
  // Specify which primary register reported the error...
  //
  NextErrData->ErrorSource = ErrorSource;
  
  //
  // Populate IOH error information. 
  // NOTE: We should not clear the error status, this will be performed by the caller
  //
  NextErrData->PortType = 0; //??? // port type from capabilities
  NextErrData->Version = PCIE_SPECIFICATION_SUPPORTED;


  Data16 = PciExpressRead16 (
             EFI_PCI_WHEA_ADDRESS(
                 ErrIohDev->Segment,
                 ErrIohDev->Bus,
                 ErrIohDev->Device,
                 ErrIohDev->Function,
                 PCI_COMMAND_OFFSET
                 )
             );

  Data32 = (UINT32)Data16 << 16;
  Data16 = PciExpressRead16 (
             EFI_PCI_WHEA_ADDRESS(
                 ErrIohDev->Segment,
                 ErrIohDev->Bus,
                 ErrIohDev->Device,
                 ErrIohDev->Function,
                 PCI_PRIMARY_STATUS_OFFSET
                 )
             );

  NextErrData->CommandStatus = Data32 | Data16; // read command & status reg
  Data16 = PciExpressRead16 (
             EFI_PCI_WHEA_ADDRESS(
                 ErrIohDev->Segment,
                 ErrIohDev->Bus,
                 ErrIohDev->Device,
                 ErrIohDev->Function,
                 PCI_VENDOR_ID_OFFSET
                 )
             );
  NextErrData->DevBridge.VendorId = Data16;

  Data16 = PciExpressRead16 (
             EFI_PCI_WHEA_ADDRESS(
                 ErrIohDev->Segment,
                 ErrIohDev->Bus,
                 ErrIohDev->Device,
                 ErrIohDev->Function,
                 PCI_DEVICE_ID_OFFSET
                 )
             );
  NextErrData->DevBridge.DeviceId = Data16;

  Data32 = PciExpressRead32 (
             EFI_PCI_WHEA_ADDRESS(
                 ErrIohDev->Segment,
                 ErrIohDev->Bus,
                 ErrIohDev->Device,
                 ErrIohDev->Function,
                 PCI_REVISION_ID_OFFSET
                 )
             );
  NextErrData->DevBridge.ClassCode[0] = (UINT8)(Data32 >> 0x18);
  NextErrData->DevBridge.ClassCode[1] = (UINT8)(Data32 >> 0x10);
  NextErrData->DevBridge.ClassCode[2] = (UINT8)(Data32 >> 0x8);

  NextErrData->DevBridge.Function = ErrIohDev->Function;
  NextErrData->DevBridge.Device   = ErrIohDev->Device;
  NextErrData->DevBridge.Segment  = ErrIohDev->Segment;

  //
  // Check if PCI bridge
  //
  Data32 = PciExpressRead32 (
             EFI_PCI_WHEA_ADDRESS (
                 ErrIohDev->Segment,
                 ErrIohDev->Bus,
                 ErrIohDev->Device,
                 ErrIohDev->Function,
                 PCI_REVISION_ID_OFFSET
                 )
             );
  Data32 &= 0xFFFFFF00;

  if ( (Data32 == EFI_PCI_BRIDGE_DEV_CLASS_CODE) ||
       (Data32 == EFI_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE ) ) {
    //
    // Yes, it is PCI bridge
    // Fill bridge information
    //
    Data8 = PciExpressRead8 (
              EFI_PCI_WHEA_ADDRESS (
                  ErrIohDev->Segment,
                  ErrIohDev->Bus,
                  ErrIohDev->Device,
                  ErrIohDev->Function,
                  PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET
                  )
              );
    NextErrData->DevBridge.PrimaryOrDeviceBus = Data8;

    Data8 = PciExpressRead8 (
              EFI_PCI_WHEA_ADDRESS (
                  ErrIohDev->Segment,
                  ErrIohDev->Bus,
                  ErrIohDev->Device,
                  ErrIohDev->Function,
                  PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
                  )
              );
    NextErrData->DevBridge.SecondaryBus = Data8;

    Data16 = PciExpressRead16 (
               EFI_PCI_WHEA_ADDRESS (
                   ErrIohDev->Segment,
                   ErrIohDev->Bus,
                   ErrIohDev->Device,
                   ErrIohDev->Function,
                   PCI_BRIDGE_CONTROL_REGISTER_OFFSET
                   )
             );

    Data32 = (UINT32) Data16 << 16;

    Data16 = PciExpressRead16 (
               EFI_PCI_WHEA_ADDRESS(
                   ErrIohDev->Segment,
                   ErrIohDev->Bus,
                   ErrIohDev->Device,
                   ErrIohDev->Function,
                   PCI_BRIDGE_STATUS_REGISTER_OFFSET
                   )
             );

  
    NextErrData->BridgeControlStatus = Data32 | Data16;

    //
    // Set Bridge information valid 
    //
    NextErrData->ValidFields |= PCIE_ERROR_BRIDGE_CRL_STS_VALID;
    //
    // Pci Bridge
    //
  }

  NextErrData->SerialNo = 0;

   /* PcieGetCapabilities(
        &(NextErrData->Capability),
        ErrPcieDev->Segment,
        ErrPcieDev->Bus,
        ErrPcieDev->Device,
        ErrPcieDev->Function
        );

    NextErrData->ValidFields |= PCIE_ERROR_CAPABILITY_INFO_VALID;
    PcieGetAer(
        &(NextErrData->AerInfo),
        ErrPcieDev->Segment,
        ErrPcieDev->Bus,
        ErrPcieDev->Device,
        ErrPcieDev->Function
        );

    NextErrData->ValidFields |= PCIE_ERROR_AER_INFO_VALID;
  */

  return EFI_SUCCESS;
}

EFI_STATUS
WheaGenElogNonStandardDev(
    IN UINT16                              ErrorType,
    IN NON_STANDARD_DEV_ERROR_INFO         *ErrNonStdDevInfo,
    IN GENERIC_ERROR_STATUS                *ErrStsBlk
  )
/*++

Routine Description:

  Non Standard Core Whea Error Log routine
 
--*/
{
    GENERIC_ERROR_DATA_ENTRY             *NextErrEntry;
    PLATFORM_NON_STANDARD_DEV_ERROR_DATA *NextErrData;

    if (ErrStsBlk == NULL) {
      return EFI_NOT_FOUND;
    }
    //
	//As per Microsoft, Can report one error data entry at a time.
	//If bit 3 of BlockStatus is set, it merely indicates that more 
	//than one error was detected. It doesn’t mean that more than one 
	//error is reported in the status block. Think of it in the same 
	//way as the overflow bit in the MCA status registers.
	//
    ZeroMem((VOID *)ErrStsBlk, sizeof(GENERIC_ERROR_STATUS) + ErrStsBlk->ErrDataSize);

    if (ErrorType == GEN_ERR_SEV_NON_STANDARD_DEV_FATAL) {
    // If already more than one error data log, set multiple
      if (ErrStsBlk->BlockStatus.UeValid != 1) {
        ErrStsBlk->BlockStatus.UeValid = 1;
      } else {
        ErrStsBlk->BlockStatus.MultipleUeValid = 1;
      }
    } else {
      if (ErrStsBlk->BlockStatus.CeValid != 1) {
        ErrStsBlk->BlockStatus.CeValid = 1;
      } else {
        ErrStsBlk->BlockStatus.MultipleCeValid = 1;
      }
    }

// Calculate offset for next error data log
    NextErrEntry = (GENERIC_ERROR_DATA_ENTRY *) ((UINTN)ErrStsBlk + sizeof(GENERIC_ERROR_STATUS) + 
                    ErrStsBlk->ErrDataSize);
    NextErrData = (PLATFORM_NON_STANDARD_DEV_ERROR_DATA *) ((UINTN)NextErrEntry + sizeof(GENERIC_ERROR_DATA_ENTRY));

// Set Error status block data
    ErrStsBlk->RawDataOffset = 0;                   // No Raw data for our platform memory
    ErrStsBlk->RawDataSize = 0;
    ErrStsBlk->BlockStatus.NumErrorDataEntry++;

// Adjust Error status block error data size to add new error log
    ErrStsBlk->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + sizeof(PLATFORM_NON_STANDARD_DEV_ERROR_DATA);

// Set error section type GUID
    CopyMem(&NextErrEntry->SectionType, &gEfiWheaPlatformNonStandardErrorSectionGuid, sizeof(EFI_GUID));

	NextErrEntry->Revision = GENERIC_ERROR_SECTION_REVISION_UEFI231;

    if (ErrorType == GEN_ERR_SEV_NON_STANDARD_DEV_FATAL) {
      ErrStsBlk->Severity       = GENERIC_ERROR_FATAL;
      NextErrEntry->Severity    = GENERIC_ERROR_FATAL;
    } else if (ErrorType == GEN_ERR_SEV_NON_STANDARD_DEV_RECOVERABLE){
      ErrStsBlk->Severity       = GENERIC_ERROR_RECOVERABLE;
      NextErrEntry->Severity    = GENERIC_ERROR_RECOVERABLE;
    } else {
      ErrStsBlk->Severity       = GENERIC_ERROR_CORRECTED;
      NextErrEntry->Severity    = GENERIC_ERROR_CORRECTED;
    }

    NextErrEntry->SectionFlags.Primary = 1; //0;         // default for our platforms
    NextErrEntry->DataSize = sizeof(PLATFORM_NON_STANDARD_DEV_ERROR_DATA);

	NextErrData->ErrorType  = ErrNonStdDevInfo->ErrorType;
	NextErrData->PortType  = (UINT16)ErrNonStdDevInfo->PortType;

    return EFI_SUCCESS;
}