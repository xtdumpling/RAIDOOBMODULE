/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2015, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file AcpiBdatmHook.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


extern struct SystemMemoryMapHob   *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;
extern EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;

#ifdef BDAT_SUPPORT
#include <Acpi/Bdat.h>
EFI_GUID gEfiMemoryMapDataHobBdatGuid  = BDAT_MEMORY_DATA_4B_GUID;
EFI_GUID gEfiRmtDataHobBdatGuid        = RMT_SCHEMA_5_GUID;
#endif

#ifndef MAX_HOB_ENTRY_SIZE
#define MAX_HOB_ENTRY_SIZE  60*1024
#endif

#ifdef BDAT_SUPPORT
static
INT16 Crc16 (char *ptr, int count)
{
  INT16 crc, i;
  crc = 0;
  while (--count >= 0)
  {
    crc = crc ^ (INT16)(int)*ptr++ << 8;
    for (i = 0; i < 8; ++i)
    {
      if (crc & 0x8000)
      {
        crc = crc << 1 ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    }
  }
  return (crc & 0xFFFF);
}

/**

    Update the BDAT ACPI table: Multiple instances of the BDAT DATA HOB are placed into one contiguos memory range

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchBdatAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  )
/*++

  Routine Description:

    Update the BDAT ACPI table: Multiple instances of the BDAT DATA HOB are placed into one contiguos memory range

  Arguments:

    *TableHeader   - The table to be set

  Returns:

    EFI_SUCCESS -  Returns Success

--*/
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_PHYSICAL_ADDRESS          Address, *AddressPointer;
  UINTN                         idx;
  UINTN                         CurrentHobSize;
  UINTN                         RemainingHobSizeMemSchema, RemainingHobSizeRmtSchema;
  EFI_HOB_GUID_TYPE             *GuidHob      = NULL;
  VOID                          *HobData      = NULL;
  UINT8                         checksum;
  EFI_TIME                      EfiTime;
  BDAT_STRUCTURE                *bdatHeaderStructPtr;
  UINT32                        *SchemasAddressLocationArray;
  UINT8                         socket;
  UINTN                         totalBDATstructureSize = 0;
  UINT16                        NumberExtraSchemas;
  UINT32                        BdatStructureSize;
#ifdef SSA_FLAG
  EFI_GUID                      gEfiMemoryMapDataHobBdatBssaGuid  = {0};
  UINT16                        NumberBssaSchemas = 0;
  UINT32                        GuidIdx;
  UINT32                        HobIdx;
  UINT32                        CurrentSchema;
  UINT32                        PreviousSchemaSize;
  UINT32                        RemainingHobSizeBssaSchema;
  BDAT_SCHEMA_HEADER_STRUCTURE  *bssaSchemaHeaderPtr;
#endif //SSA_FLAG
  EFI_BDAT_ACPI_DESCRIPTION_TABLE *BdatAcpiTable;

  Address = 0xffffffff;
  BdatAcpiTable = (EFI_BDAT_ACPI_DESCRIPTION_TABLE *)Table;
  NumberExtraSchemas = 1;
#ifdef SSA_FLAG
  DEBUG ((EFI_D_ERROR, "PatchBdatAcpiTable Started\n"));
  for (GuidIdx = 0; GuidIdx < mSystemMemoryMap->bssaNumberDistinctGuids; GuidIdx++) {
    NumberBssaSchemas += mSystemMemoryMap->bssaNumberHobs[GuidIdx];  //No. of HOBS per GUID added up for all GUIDs created from calls to saveToBdat()
    DEBUG ((EFI_D_ERROR, "GuidIdx = %d, total num hobs: %d\n", GuidIdx,  mSystemMemoryMap->bssaNumberHobs[GuidIdx]));
  }

  RemainingHobSizeBssaSchema = mSystemMemoryMap->bssaBdatSize + (NumberBssaSchemas*sizeof(BDAT_SCHEMA_HEADER_STRUCTURE)); //Total size of all HOBs created by SaveToBdat() + NumberBssaSchemas*headerPerSchema
  NumberExtraSchemas += NumberBssaSchemas; //total HOBs sizes added up
  DEBUG ((EFI_D_ERROR, "NumberBssaSchemas = %d, total schema size: %d\n", NumberBssaSchemas,  RemainingHobSizeBssaSchema));
#endif //SSA_FLAG
  BdatStructureSize = sizeof(BDAT_STRUCTURE) + (NumberExtraSchemas * sizeof(UINT32));


#ifdef SSA_FLAG
  totalBDATstructureSize = BdatStructureSize + sizeof (BDAT_MEMORY_DATA_STRUCTURE) + sizeof (BDAT_RMT_STRUCTURE) + RemainingHobSizeBssaSchema;
#else
  totalBDATstructureSize = BdatStructureSize + sizeof (BDAT_MEMORY_DATA_STRUCTURE) + sizeof (BDAT_RMT_STRUCTURE);
#endif //SSA_FLAG
  DEBUG ((EFI_D_ERROR, "Total BDAT size: %d\n", totalBDATstructureSize));

//Allocating RealTime Memory for BDAT. A block large enough to copy both structs + BSSA Bdat; so that they are "contiguos".
  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   EFI_SIZE_TO_PAGES (totalBDATstructureSize),
                   &Address
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  bdatHeaderStructPtr = (BDAT_STRUCTURE *)Address;
  AddressPointer = (EFI_PHYSICAL_ADDRESS *)Address;

  ZeroMem ((VOID *)AddressPointer, totalBDATstructureSize); // The memory location where the HOB's are to be copied to

  //
  // Create BIOS Data Signature
  //
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[0] = 'B';
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[1] = 'D';
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[2] = 'A';
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[3] = 'T';
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[4] = 'H';
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[5] = 'E';
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[6] = 'A';
  bdatHeaderStructPtr->BdatHeader.BiosDataSignature[7] = 'D';
  //
  // Structure size
  //
  bdatHeaderStructPtr->BdatHeader.BiosDataStructSize = (UINT32) totalBDATstructureSize;
  //
  // Primary Version
  //
  bdatHeaderStructPtr->BdatHeader.PrimaryVersion =  BDAT_PRIMARY_VER;
  //
  // Secondary Version
  //
  bdatHeaderStructPtr->BdatHeader.SecondaryVersion =  BDAT_SECONDARY_VER;
  //
  // CRC16 value of the BDAT_STRUCTURE
  //
  bdatHeaderStructPtr->BdatHeader.Crc16 = Crc16 ((char *)bdatHeaderStructPtr, BdatStructureSize);//Check if this CRC is OK
  bdatHeaderStructPtr->BdatSchemas.SchemaListLength = 1 + NumberExtraSchemas;
  bdatHeaderStructPtr->BdatSchemas.Reserved = 0;
  bdatHeaderStructPtr->BdatSchemas.Reserved1 = 0;
  //Initialize the Time parameters in the SCHEMA_LIST_STRUCTURE
  Status = gRT->GetTime (&EfiTime, NULL);
  if (!EFI_ERROR (Status)) {
    bdatHeaderStructPtr->BdatSchemas.Year =   EfiTime.Year;
    bdatHeaderStructPtr->BdatSchemas.Month =  EfiTime.Month;
    bdatHeaderStructPtr->BdatSchemas.Day =    EfiTime.Day;
    bdatHeaderStructPtr->BdatSchemas.Hour =   EfiTime.Hour;
    bdatHeaderStructPtr->BdatSchemas.Minute = EfiTime.Minute;
    bdatHeaderStructPtr->BdatSchemas.Second = EfiTime.Second;
  }


  //Saving the "Schema Offsets" from the BDAT base in RT Memory into the Schemas Array in BDAT_STRUCT"; AddressPointer -> Base Address of RT Memory Allocated
  SchemasAddressLocationArray = bdatHeaderStructPtr->BdatSchemas.Schemas;
  SchemasAddressLocationArray[0] = BdatStructureSize;  //*(x+1)=x[1]. Offset of BDAT_STRUCTURE start from BDAT base
  SchemasAddressLocationArray[1] = SchemasAddressLocationArray[0] + sizeof(BDAT_MEMORY_DATA_STRUCTURE);  //Offset of BDAT_MEMORY_DATA_STRUCTURE start from BDAT base
  //For the next Schema, Offset would be = baseAddress of BDAT_STRUCTURE + sizeof(BDAT_STRUCTURE) + sizeof(BDAT_MEMORY_DATA_STRUCTURE for Schema1)

  //
  // Update BDAT ACPI table
  //
  BdatAcpiTable->BdatGas.Address = (UINT64)Address;
  DEBUG ((EFI_D_ERROR, "BdatRegionAddress = %x\n", (UINT64)Address));

  //Copy BDAT base address to ScratchPad5
  for(socket = 0; socket < MAX_SOCKET; socket++) {
    if(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[socket].Valid) {
      mCpuCsrAccess->WriteCpuCsr(socket, 0, BIOSNONSTICKYSCRATCHPAD5_UBOX_MISC_REG, (UINT32)Address);
    }
  }

  //
  // Copy Bdat structure from HostGenerator to Reserved memory
  //

  RemainingHobSizeMemSchema = sizeof(BDAT_MEMORY_DATA_STRUCTURE);
  RemainingHobSizeRmtSchema = sizeof(BDAT_RMT_STRUCTURE);

  //
  // Get first BDAT Memory Data HOB
  //
  GuidHob = GetFirstGuidHob (&gEfiMemoryMapDataHobBdatGuid);  //GuidHob -> The first HOB instance of the matched GUID

  //We first copy the BDAT_STRUCT into RunTime Memory; then move the AddressPointer to the end of BDAT_STRUCT; to start Copying the Memory Schema from the HOB
  Address = Address + BdatStructureSize; //Incrementing ACPI Memory address by CurrentHobSize size to maintain contiguos memory for all instances of the HOB
  //DEBUG ((EFI_D_ERROR, "Bdat address after BDAT_STRUCTURE = %x\n", (UINT64)Address));

  AddressPointer = (EFI_PHYSICAL_ADDRESS *)Address;
#ifdef SSA_FLAG
  HobIdx = 0;
#endif

  DEBUG ((EFI_D_ERROR, "Remaining HobSize MemSchema: %d\n", RemainingHobSizeMemSchema));
  while (RemainingHobSizeMemSchema) {
    ASSERT(GuidHob != NULL);
    if (GuidHob == NULL) {
      return EFI_NOT_FOUND;
    }
    HobData = GET_GUID_HOB_DATA(GuidHob);  //HobData-> A pointer to the Base address of the data buffer in a HOB
    CurrentHobSize = GET_GUID_HOB_DATA_SIZE (GuidHob);  //Size of that matched instance of the  Data HOB.. HOB 0/1/2
//Prinitng out schemaID and size the 1st time; later calls - ID
#ifdef SSA_FLAG
    if (HobIdx == 0) {
      bssaSchemaHeaderPtr = (BDAT_SCHEMA_HEADER_STRUCTURE *)HobData;
      DEBUG ((EFI_D_ERROR, "Getting schema %g. Size: %d\n", &(bssaSchemaHeaderPtr->SchemaId), bssaSchemaHeaderPtr->DataSize));
    } else {
      DEBUG ((EFI_D_ERROR, "Getting hob: %d\n", HobIdx));
    }
    HobIdx++;
#endif
    //
    // Trim the HOB data padding so there is no overflow on the buffer
    //
    if (CurrentHobSize > RemainingHobSizeMemSchema){
      CurrentHobSize = RemainingHobSizeMemSchema;
    }

    if (CurrentHobSize < MAX_HOB_ENTRY_SIZE){
      RemainingHobSizeMemSchema = 0;
    } else {
      RemainingHobSizeMemSchema -= CurrentHobSize;
    }

    //
    // Get next BDAT data hob, if none left, results NULL
    //
    GuidHob = GET_NEXT_HOB (GuidHob); // Increment to next HOB
    GuidHob = GetNextGuidHob (&gEfiMemoryMapDataHobBdatGuid, GuidHob);  // Now search for next instance of the BDAT HOB

    //Copy Bdat structure from HOB's to Reserved memory
    CopyMem ((VOID *)AddressPointer, (VOID *)HobData, CurrentHobSize);

    Address = Address + CurrentHobSize; //Incrementing ACPI Memory address by CurrentHobSize size to maintain contiguos memory for all instances of the HOB
    AddressPointer = (EFI_PHYSICAL_ADDRESS *)Address;
  } //while loop

  //
  // Get first BDAT RMT Data HOB
  //
  GuidHob = GetFirstGuidHob (&gEfiRmtDataHobBdatGuid);
#ifdef SSA_FLAG
  HobIdx = 0;
#endif
  DEBUG ((EFI_D_ERROR, "Remaining HobSize RmtSchema: %d\n", RemainingHobSizeRmtSchema));
  while (RemainingHobSizeRmtSchema) {
    ASSERT(GuidHob != NULL);
    if (GuidHob == NULL) {
      return EFI_NOT_FOUND;
    }
    HobData = GET_GUID_HOB_DATA (GuidHob);
    CurrentHobSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
#ifdef SSA_FLAG
    if (HobIdx == 0) {
      bssaSchemaHeaderPtr = (BDAT_SCHEMA_HEADER_STRUCTURE *)HobData;
      DEBUG ((EFI_D_ERROR, "Getting schema %g. Size: %d\n", &(bssaSchemaHeaderPtr->SchemaId), bssaSchemaHeaderPtr->DataSize));
    } else {
      DEBUG ((EFI_D_ERROR, "Getting hob: %d\n", HobIdx));
    }
    HobIdx++;
#endif
    //
    // Trim the HOB data padding so there is no overflow on the buffer
    //
    if (CurrentHobSize > RemainingHobSizeRmtSchema){
      CurrentHobSize = RemainingHobSizeRmtSchema;
    }

    if (CurrentHobSize < MAX_HOB_ENTRY_SIZE){
      RemainingHobSizeRmtSchema = 0;
    } else {
      RemainingHobSizeRmtSchema -= CurrentHobSize;
    }

    //
    // Get next BDAT data hob, if none left, results NULL
    //
    GuidHob = GET_NEXT_HOB (GuidHob);
    GuidHob = GetNextGuidHob (&gEfiRmtDataHobBdatGuid, GuidHob);

    CopyMem ((VOID *)AddressPointer, (VOID *)HobData, CurrentHobSize);

    Address = Address + CurrentHobSize;
    AddressPointer = (EFI_PHYSICAL_ADDRESS *)Address;
  } //while loop

#ifdef SSA_FLAG
  //
  //Saving to RT Memory BDAT Data received from HOBs generated due to call/calls to SaveToBdat()
  //
  PreviousSchemaSize = sizeof(BDAT_RMT_STRUCTURE);
  CurrentSchema = 2;
  for (GuidIdx = 0; GuidIdx < mSystemMemoryMap->bssaNumberDistinctGuids; GuidIdx++) {
    gEfiMemoryMapDataHobBdatBssaGuid  = mSystemMemoryMap->bssaBdatGuid[GuidIdx]; //get first GUID instance
    GuidHob = GetFirstGuidHob (&gEfiMemoryMapDataHobBdatBssaGuid);
    DEBUG ((EFI_D_ERROR, "Getting BIOS SSA result with GUID  %g\n", &gEfiMemoryMapDataHobBdatBssaGuid));
    for (HobIdx = 0; HobIdx < mSystemMemoryMap->bssaNumberHobs[GuidIdx]; HobIdx++) { //looping through all HOBs linked to that GUID
      ASSERT(GuidHob != NULL);
      if (GuidHob == NULL) {
        return EFI_NOT_FOUND;
      }
      HobData = GET_GUID_HOB_DATA (GuidHob);
      CurrentHobSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
      DEBUG ((EFI_D_ERROR, "Initial HOB size  %d; remaining HOB size %d\n", CurrentHobSize, RemainingHobSizeBssaSchema));
      //Setting the header first
      if (RemainingHobSizeBssaSchema < sizeof(BDAT_SCHEMA_HEADER_STRUCTURE)) {
        //Nothing we can do, break execution
        DEBUG ((EFI_D_ERROR, "Not enough space to add schema header to BIOS SSA result\n"));
        RemainingHobSizeBssaSchema = 0;
        break;
      }
      //Each HOB has a header added to it (BDAT_SCHEMA_HEADER_STRUCTURE)
      bssaSchemaHeaderPtr = (BDAT_SCHEMA_HEADER_STRUCTURE *)Address;
      bssaSchemaHeaderPtr->SchemaId = gEfiMemoryMapDataHobBdatBssaGuid;
      RemainingHobSizeBssaSchema -= sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
      Address = Address + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
      AddressPointer = (EFI_PHYSICAL_ADDRESS *)Address;
      //CRC16 value of the BDAT_SCHEMA_HEADER_STRUCTURE
      bssaSchemaHeaderPtr->Crc16 = Crc16 ((char *)bssaSchemaHeaderPtr, sizeof(BDAT_SCHEMA_HEADER_STRUCTURE));

      GuidHob = GET_NEXT_HOB (GuidHob); // Increment to next HOB
      GuidHob = GetNextGuidHob (&gEfiMemoryMapDataHobBdatBssaGuid, GuidHob);  // Now search for next instance of the BDAT HOB

      SchemasAddressLocationArray[CurrentSchema] = SchemasAddressLocationArray[CurrentSchema  -1] + PreviousSchemaSize;
      CurrentSchema++;

      if (RemainingHobSizeBssaSchema < CurrentHobSize) {
        DEBUG ((EFI_D_ERROR, "Not enough space to add complete BIOS SSA result\n"));
        CurrentHobSize = RemainingHobSizeBssaSchema;
      }
      //HOB size won't overflow a UINT32.
      bssaSchemaHeaderPtr->DataSize = (UINT32)CurrentHobSize + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
      DEBUG ((EFI_D_ERROR, "Setting schema %g size to %d\n", &(bssaSchemaHeaderPtr->SchemaId), bssaSchemaHeaderPtr->DataSize));
      //HOB size won't overflow a UINT32.
      PreviousSchemaSize = (UINT32)CurrentHobSize + sizeof(BDAT_SCHEMA_HEADER_STRUCTURE);
      DEBUG ((EFI_D_ERROR, "Copying %d bytes to 0x%x\n", CurrentHobSize, AddressPointer));

     //Copy HOB to RT Memory
      CopyMem ((VOID *)AddressPointer, (VOID *)HobData, (UINT32)CurrentHobSize);
      //HOB size won't overflow a UINT32.
      DEBUG ((EFI_D_ERROR, "HOB size  %d; remaining HOB size %d\n", CurrentHobSize, RemainingHobSizeBssaSchema));
      RemainingHobSizeBssaSchema -= (UINT32)CurrentHobSize;
      Address = Address + CurrentHobSize; //Incrementing ACPI Memory address by CurrentHobSize size to maintain contiguos memory for all instances of the HOB
      AddressPointer = (EFI_PHYSICAL_ADDRESS *)Address;
      if (RemainingHobSizeBssaSchema == 0) {
        break;
      }
    }
    if (RemainingHobSizeBssaSchema == 0) {
      break;
    }
  }
#endif //SSA_FLAG

  //
  // Update checksum
  //
  BdatAcpiTable->Header.Checksum = 0;
  checksum = 0;
  for(idx = 0; idx < sizeof(EFI_BDAT_ACPI_DESCRIPTION_TABLE); idx++) {
    checksum = checksum + (UINT8) (((UINT8 *)(BdatAcpiTable))[idx]);
  }
  BdatAcpiTable->Header.Checksum = (UINT8) (0 - checksum);

  return Status;
}
#endif
