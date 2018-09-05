//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This is an implementation of the Whea Support protocol.

  Copyright (c) 2007 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "WheaSupport.h"


// WHEA Support driver instance data structure
STATIC
WHEA_SUPPORT_INST mWheaInst = 
{
  EFI_WHEA_SUPPORT_INST_SIGNATURE,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  0,
  NULL,
  0,
  NULL,
  0,
  NULL,
  FALSE,
  0,
  {NULL}
};

STATIC
EFI_ACPI_TABLE_PROTOCOL         *mAcpiTbl = NULL;
STATIC
BOOLEAN                         mReadyToBoot = FALSE;
UINT8                           mEinjEntries = 00;
#define EINJ5_ENTRIES           0x09
#define EINJ4_ENTRIES           0x08
//
// WHEA Support protocol member functions
//

STATIC
EFI_STATUS
AddErrorSource (
  IN EFI_WHEA_SUPPORT_PROTOCOL          *This,
  IN WHEA_ERROR_TYPE                    Type,
  IN UINTN                              Flags,
  IN BOOLEAN                            EnableError,
  OUT UINT16                            *SourceId,
  IN UINTN                              NoOfRecords,
  IN UINTN                              MaxSections,
  IN OUT VOID                           *SourceData
  )
{
  WHEA_SUPPORT_INST                     *WheaInst;
  EFI_ACPI_WHEA_ERROR_SOURCE_TABLE      *Hest;
  UINT8                                 *NextSrc;
  UINTN                                 SrcLen;
  GENERIC_HW_ERROR_SOURCE                       *GenSrc;
  EFI_ACPI_WHEA_GENERIC_ERROR_SOURCE_ENTRY      *GenSrcEntry;
  UINTN                                         MaxErrorStatusSize;
  PCIE_AER_SOURCE_COMMON                        *PcieCommon;
  EFI_ACPI_WHEA_XPF_MCE_SOURCE_ENTRY                *MceSrc;
  EFI_ACPI_WHEA_XPF_CMC_SOURCE_ENTRY            *CmcSrc;   

//
// Get WHEA support protocol instance structure data
//
  if (This == NULL) {
    return EFI_NOT_FOUND; 
  }
  WheaInst = WHEA_SUPPORT_INST_FROM_THIS(This);
  Hest = WheaInst->Hest;
  NextSrc = (UINT8 *)Hest + Hest->Header.Length;
  SrcLen = 0;

//
// Check Error source type being added.
//
  switch (Type) {
    case GenericHw :
      SrcLen = sizeof(EFI_ACPI_WHEA_GENERIC_ERROR_SOURCE_ENTRY);
      GenSrc = (GENERIC_HW_ERROR_SOURCE *)SourceData;
      GenSrcEntry = (EFI_ACPI_WHEA_GENERIC_ERROR_SOURCE_ENTRY *)NextSrc;
      //
      // Check if we have enough space in global ErrorStatusBlock & then
      // Allocate space for Gen HW Error Status block; size required is given 
      // in SourceData->ErrorStatusSize.
      //
      // Find maximum Error status block size required
      MaxErrorStatusSize = GenSrc->MaxErrorDataSize;    // Raw data size
      if (GenSrc->ErrorStatusSize > MaxErrorStatusSize) 
      {
        MaxErrorStatusSize = GenSrc->ErrorStatusSize;    // Error Status size
      }
      if ((WheaInst->CurErrorStatusBlockSize + MaxErrorStatusSize + sizeof(UINT64)) > MAX_ERROR_STATUS_SIZE) {
       // No more space in the buffer - remote failure case
        return EFI_OUT_OF_RESOURCES;
      }
      // Now allocate and adjust global ErrorStatusBlock
      GenSrc->ErrorStatusAddress.Address = (UINT64) (UINTN)WheaInst->ErrorStatusBlock;
      *(UINT64 *)WheaInst->ErrorStatusBlock = (UINT64) (UINTN)((UINT8 *)WheaInst->ErrorStatusBlock + sizeof(UINT64));
      GenSrc->ErrorStatusAddress.AddressSpaceId = 0;      // System memory
      GenSrc->ErrorStatusAddress.RegisterBitWidth = 0x40; //0;    
      GenSrc->ErrorStatusAddress.RegisterBitOffset = 0;
      GenSrc->ErrorStatusAddress.AccessSize = 4; // qword access

      WheaInst->ErrorStatusBlock = (VOID *)((UINT8 *)WheaInst->ErrorStatusBlock + MaxErrorStatusSize + sizeof(UINT64));
      WheaInst->CurErrorStatusBlockSize += (MaxErrorStatusSize + sizeof(UINT64));

      // Ok, now add the source to HEST :)
      GenSrcEntry->Type = Type;
      *SourceId = (UINT16)Hest->ErrorEntryCount;
      GenSrcEntry->SourceId = (UINT16)Hest->ErrorEntryCount;
      GenSrcEntry->RelatedSourceId = GenSrc->RelatedSourceId;
      GenSrcEntry->Flags = (UINT8)Flags;
      GenSrcEntry->IsEnable = (EnableError==TRUE)? 1 : 0;
      GenSrcEntry->NoOfRecords = (UINT32)NoOfRecords;
      GenSrcEntry->MaxSecPerRecord = (UINT32)MaxSections;
      GenSrcEntry->MaxErrorDataSize = GenSrc->MaxErrorDataSize;
      gBS->CopyMem(&GenSrcEntry->ErrorStatusAddress, &GenSrc->ErrorStatusAddress, sizeof(EFI_ACPI_GAS));
      gBS->CopyMem(&GenSrcEntry->NotifyMethod, &GenSrc->NotifyMethod, sizeof(HW_NOTIFY_METHOD));
      GenSrcEntry->ErrorStatusSize = GenSrc->ErrorStatusSize;
      break;

    case XpfMce:
      SrcLen = sizeof(EFI_ACPI_WHEA_XPF_MCE_SOURCE_ENTRY);
      MceSrc = (EFI_ACPI_WHEA_XPF_MCE_SOURCE_ENTRY *)NextSrc;
      MceSrc->Type = Type;
      *SourceId = (UINT16)Hest->ErrorEntryCount;
      MceSrc->SourceId = (UINT16)Hest->ErrorEntryCount;
      MceSrc->CfgWriteEnable = 0;
      MceSrc->Flags = (UINT8)Flags;
      MceSrc->IsEnable = (EnableError==TRUE)? 1 : 0;
      MceSrc->NoOfRecords = (UINT32)NoOfRecords;
      MceSrc->MaxSecPerRecord = (UINT32)MaxSections;
      gBS->CopyMem(&MceSrc->MceSource, SourceData, sizeof(XPF_MCE_SOURCE));
      break;
    case XpfCmc:
      SrcLen = sizeof(EFI_ACPI_WHEA_XPF_CMC_SOURCE_ENTRY);
      CmcSrc = (EFI_ACPI_WHEA_XPF_CMC_SOURCE_ENTRY *)NextSrc;
      CmcSrc->Type = Type;
      *SourceId = (UINT16)Hest->ErrorEntryCount;
      CmcSrc->SourceId = (UINT16)Hest->ErrorEntryCount;
      CmcSrc->CfgWriteEnable = 0;
      CmcSrc->Flags = (UINT8)Flags;
      CmcSrc->IsEnable = (EnableError==TRUE)? 1 : 0;
      CmcSrc->NoOfRecords = (UINT32)NoOfRecords;
      CmcSrc->MaxSecPerRecord = (UINT32)MaxSections;
      gBS->CopyMem(&CmcSrc->CmcSource, SourceData, sizeof(XPF_CMC_SOURCE));
      break;
    case XpfNmi:
      // *** not required for now.. os will have it's native handler.
      return EFI_UNSUPPORTED;
    
    case PcieRootAer:
    case PcieDeviceAer:
    case PcieBridgeAer:
      PcieCommon = (PCIE_AER_SOURCE_COMMON *)NextSrc;
      PcieCommon->Type = Type;
      *SourceId = (UINT16)Hest->ErrorEntryCount;
      PcieCommon->SourceId = (UINT16)Hest->ErrorEntryCount;
      PcieCommon->CfgWriteEnable = 0;
      PcieCommon->Flags = (UINT8)Flags;
      PcieCommon->IsEnable = (EnableError==TRUE)? 1 : 0;
      PcieCommon->NoOfRecords = (UINT32)NoOfRecords;
      PcieCommon->MaxSecPerRecord = (UINT32)MaxSections;
      
        switch (Type) {
          case PcieRootAer:
            SrcLen = sizeof (PCIE_ROOT_AER_SOURCE);
            break;
          case PcieDeviceAer:
            SrcLen = sizeof (PCIE_DEVICE_AER_SOURCE);
            break;
          case PcieBridgeAer:
          default:
            SrcLen = sizeof (PCIE_BRIDGE_AER_SOURCE);
               break;
        }
      gBS->CopyMem((UINT8 *)(UINTN)PcieCommon + sizeof(PCIE_AER_SOURCE_COMMON), SourceData, SrcLen);
      SrcLen += sizeof(PCIE_AER_SOURCE_COMMON);
      break;
    default:
      return EFI_UNSUPPORTED;
  }
  
  // Ok, adjust the HEST size & Error entry count;
  Hest->Header.Length += (UINT32)SrcLen;
  Hest->ErrorEntryCount++;
  return EFI_SUCCESS;
}

//
// Add an last boot error data log to WHEA for error that happend on last boot.
//
STATIC
EFI_STATUS
AddBootErrorLog (
  IN EFI_WHEA_SUPPORT_PROTOCOL          *This,
  IN UINT8                              ErrorCondtion,
  IN UINT32                             ErrorSevirity,
  OPTIONAL IN EFI_GUID                  *FruID, 
  OPTIONAL IN CHAR8                     *FruDescription,
  IN EFI_GUID                           *ErrorType, 
  IN UINT32                             ErrorDataSize, 
  OPTIONAL IN UINT8                     *ErrorData
  )
{
  return EFI_SUCCESS;
}

//
// This funtion will install serialization instruction for error injection method for an error type (e.g. memory UE).
// If error injection method already exist for the error type, the old method will be replced with new one.
//
STATIC
EFI_STATUS
InstallErrorInjectionMethod (
  IN EFI_WHEA_SUPPORT_PROTOCOL          *This,
  IN UINTN                              ErrorType,
  IN UINTN                              InstCount,
  IN VOID                               *InstEntry
  )
{
  WHEA_SUPPORT_INST                     *WheaInst;

//
// Get WHEA support protocol instance structure data
//
  if (This == NULL) {
    return EFI_NOT_FOUND; 
  }
  WheaInst = WHEA_SUPPORT_INST_FROM_THIS(This);

  ASSERT (mEinjEntries <= 10);
  WheaInst->InstalledErrorInj = ErrorType;
  WheaInst->Einj->InstructionEntryCount = (UINT32)mEinjEntries;
  gBS->CopyMem((VOID *)&WheaInst->Einj->InstructionEntry[0], InstEntry, \
    mEinjEntries * sizeof(EFI_ACPI_WHEA_SERIALIZATION_ACTION));
  if(mEinjEntries == EINJ5_ENTRIES)
  WheaInst->Einj->Header.Length = sizeof(EFI_ACPI_WHEA_ERROR_INJECTION_TABLE);
  else
    WheaInst->Einj->Header.Length = sizeof(EFI_ACPI_WHEA_ERROR_INJECTION_TABLE) - sizeof(EFI_ACPI_WHEA_SERIALIZATION_ACTION);    
  return EFI_SUCCESS;
}

//
// Tis function will get the current error injection capability installed in a bitmap.
//
STATIC
EFI_STATUS
GetErrorInjectionCapability (
  IN EFI_WHEA_SUPPORT_PROTOCOL          *This,
  OUT UINTN                             *InjectCapability
  )
{
  WHEA_SUPPORT_INST                     *WheaInst;

//
// Get WHEA support protocol instance structure data
//
  if (This == NULL) {
    return EFI_NOT_FOUND; 
  }
  WheaInst = WHEA_SUPPORT_INST_FROM_THIS(This);
  *InjectCapability = WheaInst->InstalledErrorInj;
  return EFI_SUCCESS;
}

//
// Returns the Error log Address Range allocated for WHEA
//
STATIC
EFI_STATUS
GetElar (
  IN EFI_WHEA_SUPPORT_PROTOCOL          *This,
  OUT UINTN                             *ElarSize,
  OUT VOID                              **LogAddress
  )
{
  WHEA_SUPPORT_INST                     *WheaInst;

//
// Get WHEA support protocol instance structure data
//
  if (This == NULL) {
    return EFI_NOT_FOUND; 
  }

  WheaInst = WHEA_SUPPORT_INST_FROM_THIS(This);
  *ElarSize = WheaInst->ErrorLogAddressRangeSize;
  *LogAddress = WheaInst->ErrorLogAddressRange;

  if (WheaInst->ErrorLogAddressRangeSize == 0 ||
    WheaInst->ErrorLogAddressRange == NULL) {
    return EFI_BAD_BUFFER_SIZE;
  }

  return EFI_SUCCESS;
}

//
// This installs the serialization actions for accessing Error Record persitant Storage.
//
STATIC
EFI_STATUS
InstallErrorRecordMethod (
  IN EFI_WHEA_SUPPORT_PROTOCOL          *This,
  IN UINTN                              InstCount,
  IN VOID                               *InstEntry
  )
{
  WHEA_SUPPORT_INST                     *WheaInst;

//
// Get WHEA support protocol instance structure data
//
  if (This == NULL) {
    return EFI_NOT_FOUND; 
  }
  WheaInst = WHEA_SUPPORT_INST_FROM_THIS(This);

  ASSERT (InstCount <= 16);
  WheaInst->Erst->InstructionEntryCount = (UINT32)InstCount;
  gBS->CopyMem((VOID *)&WheaInst->Erst->InstructionEntry[0], InstEntry, \
    InstCount * sizeof(EFI_ACPI_WHEA_SERIALIZATION_ACTION));
  WheaInst->Erst->Header.Length = sizeof(EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
InstallWheaTables (
  WHEA_SUPPORT_INST                 *WheaInst
  )
{
  EFI_STATUS            Status;
  UINTN                 TableKey;

  if (WheaInst == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (WheaInst->TablesInstalled) {
    return EFI_ALREADY_STARTED;
  }
  if (mAcpiTbl) {
    DEBUG((EFI_D_ERROR, "WheaSupport. Install Whea Tables \n"));
    TableKey = 0;
    Status = mAcpiTbl->InstallAcpiTable(mAcpiTbl,WheaInst->Hest, WheaInst->Hest->Header.Length,&TableKey);
    TableKey = 0;
    Status |= mAcpiTbl->InstallAcpiTable (mAcpiTbl, WheaInst->Bert, WheaInst->Bert->Header.Length, &TableKey);
    TableKey = 0;
    Status |= mAcpiTbl->InstallAcpiTable (mAcpiTbl, WheaInst->Erst, WheaInst->Erst->Header.Length, &TableKey);
    TableKey = 0;
    Status |= mAcpiTbl->InstallAcpiTable (mAcpiTbl, WheaInst->Einj, WheaInst->Einj->Header.Length, &TableKey);
    ASSERT_EFI_ERROR (Status); 

    WheaInst->TablesInstalled = TRUE;

    gBS->FreePool(WheaInst->Hest);
    gBS->FreePool(WheaInst->Bert);
    gBS->FreePool(WheaInst->Erst);
    gBS->FreePool(WheaInst->Einj);
  }
  return EFI_SUCCESS;
}

VOID
EFIAPI
CheckInstallTables (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                    Status;
  
  mReadyToBoot = TRUE;  
  Status = InstallWheaTables((WHEA_SUPPORT_INST *)Context);
  if (EFI_ERROR(Status) && (Status != EFI_ALREADY_STARTED)) {
    ASSERT_EFI_ERROR (Status);
  }
}

VOID
EFIAPI
CheckAcpiSupport (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                    Status;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &mAcpiTbl);
  
  ASSERT_EFI_ERROR (Status);
  if (mReadyToBoot) {
    Status = InstallWheaTables((WHEA_SUPPORT_INST *)Context);
    if (EFI_ERROR(Status) && (Status != EFI_ALREADY_STARTED)) {
      ASSERT_EFI_ERROR (Status);
    }
  }
}

//
// Driver entry point
//
EFI_STATUS
EFIAPI
InstallWheaSupport (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description: 

  Entry point of the Whea support driver. 

Arguments:  

  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

Returns:  

  EFI_SUCCESS:              Driver initialized successfully
  EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

--*/
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  STATIC EFI_EVENT          AcpiEvent;
  EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL *IsWhea;

 //
  // Check if platform support WHEA
  //
  IsWhea = NULL;
  Status = gBS->LocateProtocol (&gEfiIsPlatformSupportWheaProtocolGuid, NULL, &IsWhea);
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }
  
  if (!IsWhea->IsPlatformSupportWhea(IsWhea)) {
    return EFI_UNSUPPORTED;
  }

  InitWheaPlatformHookLib();

// Allocate memory space for WHEA ACPI tables and initilize them - EfiLibAllocateZeroPool
  mWheaInst.Hest = (EFI_ACPI_WHEA_ERROR_SOURCE_TABLE *)AllocateZeroPool (MAX_HEST_SIZE);
  ASSERT (mWheaInst.Hest != NULL);
  mWheaInst.Erst = (EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE *)AllocateZeroPool (MAX_ERST_SIZE);
  ASSERT (mWheaInst.Erst != NULL);
  mWheaInst.Einj = (EFI_ACPI_WHEA_ERROR_INJECTION_TABLE *)AllocateZeroPool (MAX_EINJ_SIZE);
  ASSERT (mWheaInst.Einj != NULL);
  mWheaInst.Bert = (EFI_ACPI_WHEA_BOOT_ERROR_TABLE *)AllocateZeroPool (MAX_BERT_SIZE);
  ASSERT (mWheaInst.Bert != NULL);

  mWheaInst.Hest->Header.Signature          = EFI_ACPI_WHEA_HEST_TABLE_SIGNATURE;
  mWheaInst.Hest->Header.Length             = sizeof(EFI_ACPI_WHEA_ERROR_SOURCE_TABLE);
  mWheaInst.Hest->ErrorEntryCount           = 0;
  mWheaInst.Hest->Header.Revision           = EFI_WHEA_TABLE_REVISION;
  mWheaInst.Hest->Header.Checksum           = 0;
  UpdateAcpiTableIds(&mWheaInst.Hest->Header);

  mWheaInst.Erst->Header.Signature          = EFI_ACPI_WHEA_ERST_TABLE_SIGNATURE;
  mWheaInst.Erst->Header.Length             = sizeof(EFI_ACPI_WHEA_ERROR_SERIALIZATION_TABLE);
  mWheaInst.Erst->SerializationHeaderSize   = sizeof(EFI_ACPI_WHEA_ERROR_SERIALIZATION_HEADER); //12;
  mWheaInst.Erst->InstructionEntryCount     = 0;
  mWheaInst.Erst->Header.Revision           = EFI_WHEA_TABLE_REVISION;
  mWheaInst.Erst->Header.Checksum           = 0;
  UpdateAcpiTableIds(&mWheaInst.Erst->Header);

  mWheaInst.Bert->Header.Signature          = EFI_ACPI_WHEA_BERT_TABLE_SIGNATURE;
  mWheaInst.Bert->Header.Length             = sizeof(EFI_ACPI_WHEA_BOOT_ERROR_TABLE);
  mWheaInst.Bert->Header.Revision           = EFI_WHEA_TABLE_REVISION;
  mWheaInst.Bert->Header.Checksum           = 0;
  UpdateAcpiTableIds(&mWheaInst.Bert->Header);

  mWheaInst.Einj->Header.Signature          = EFI_ACPI_WHEA_EINJ_TABLE_SIGNATURE;

  //  
  // Added new entry in ACPI 5 WHEA EINJ table for SET_ERROR_TYPE_WITH_ADDRESS
  //
  if(IsWhea->WheaAcpi5Support == TRUE)  {
  mWheaInst.Einj->Header.Length             = sizeof(EFI_ACPI_WHEA_ERROR_INJECTION_TABLE);
  mEinjEntries = EINJ5_ENTRIES;
  }
  else  {
    mWheaInst.Einj->Header.Length             = sizeof(EFI_ACPI_WHEA_ERROR_INJECTION_TABLE) - sizeof(EFI_ACPI_WHEA_SERIALIZATION_ACTION);
    mEinjEntries = EINJ4_ENTRIES;
  }
// APTIOV_SERVER_OVERRIDE_RC_START : Changed the length for error logging to work with WHEAHCT tool.
  mWheaInst.Einj->InjectionHeaderSize       = sizeof(EFI_ACPI_WHEA_ERROR_INJECTION_HEADER) - sizeof(EFI_ACPI_DESCRIPTION_HEADER); 
// APTIOV_SERVER_OVERRIDE_RC_END : Changed the length for error logging to work with WHEAHCT tool.

  mWheaInst.Einj->InstructionEntryCount     = mEinjEntries;
  mWheaInst.Einj->Header.Revision           = EFI_WHEA_TABLE_REVISION;
  mWheaInst.Einj->Header.Checksum           = 0;
  UpdateAcpiTableIds(&mWheaInst.Einj->Header);

// Allocate memory space Error status blocks
// Allocate memort space for Errorlog Address range
  Status = gBS->AllocatePool (EfiReservedMemoryType, (MAX_ERROR_STATUS_SIZE + MAX_ERROR_LOG_RANGE_SIZE \
                                + MAX_BOOT_ERROR_LOG_SIZE), &mWheaInst.ErrorStatusBlock);
  ASSERT_EFI_ERROR (Status);
  
  gBS->SetMem (mWheaInst.ErrorStatusBlock, (MAX_ERROR_STATUS_SIZE + MAX_ERROR_LOG_RANGE_SIZE \
                                + MAX_BOOT_ERROR_LOG_SIZE),0);

  mWheaInst.CurErrorStatusBlockSize     = 0;        // Current usage
  mWheaInst.ErrorStatusBlockSize        = MAX_ERROR_STATUS_SIZE;
  mWheaInst.ErrorLogAddressRange        = (VOID *)(((UINT8 *)mWheaInst.ErrorStatusBlock) + MAX_ERROR_STATUS_SIZE);
  mWheaInst.ErrorLogAddressRangeSize    = MAX_ERROR_LOG_RANGE_SIZE;

// Boot error log space
  mWheaInst.BootErrorRegionLen          = MAX_BOOT_ERROR_LOG_SIZE;
  mWheaInst.BootErrorRegion             = (VOID *)(((UINT8 *)mWheaInst.ErrorLogAddressRange) + MAX_ERROR_LOG_RANGE_SIZE);
  mWheaInst.Bert->BootErrorRegionLen    = (UINT32)mWheaInst.BootErrorRegionLen;
  mWheaInst.Bert->BootErrorRegion       = (UINT64) (UINTN)mWheaInst.BootErrorRegion;

// Initialize Member functions
  mWheaInst.WheaSupport.AddErrorSource                  = AddErrorSource;
  mWheaInst.WheaSupport.AddBootErrorLog                 = AddBootErrorLog;
  mWheaInst.WheaSupport.InstallErrorInjectionMethod     = InstallErrorInjectionMethod;
  mWheaInst.WheaSupport.GetErrorInjectionCapability     = GetErrorInjectionCapability;
  mWheaInst.WheaSupport.GetElar                         = GetElar;
  mWheaInst.WheaSupport.InstallErrorRecordMethod        = InstallErrorRecordMethod;

// Install protocol
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiWheaSupportProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mWheaInst.WheaSupport
                  );
  ASSERT_EFI_ERROR (Status);

//
// Setup Notification for installing Whea ACPI tables
//

// Register the ready to boot event.
 
  mAcpiTbl = NULL;
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &mAcpiTbl);

  Status = EfiCreateEventReadyToBootEx (
            TPL_NOTIFY,
            CheckInstallTables,
            &mWheaInst,
            &AcpiEvent
            );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
