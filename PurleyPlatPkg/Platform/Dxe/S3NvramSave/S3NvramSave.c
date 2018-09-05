/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file S3NvramSave.c


**/

#include "S3NvramSave.h"
#include <Protocol/PlatformType.h>
#include <Protocol/Decompress.h>
#include <Library/CompressLib.h>
#include <Protocol/VariableLock.h>


/**

  This function will retrieve the S3 data from HOBs produced by MRC
  and will save it to NVRAM if the data is absent or different from
  the previously saved data.

  @param VOID

  @retval VOID

**/
VOID
SaveS3StructToNvram (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           BufferSize;
  UINTN                           CurrentHobSize;
  UINTN                           RemainingHobSize;
  UINTN                           S3ChunkSize;
  CHAR16                          EfiMemoryConfigVariable[] = L"MemoryConfig0";
  EFI_HOB_GUID_TYPE               *GuidHob      = NULL;
  VOID                            *HobData      = NULL;
  VOID                            *VariableData = NULL;
  
  UINTN                           CompressedDataSize;
  UINT32                          ScratchSize;
  VOID                            *CompressedData = NULL;
  VOID                            *Scratch = NULL;
  EFI_DECOMPRESS_PROTOCOL         *Decompress = NULL;
  VOID                            *CompressedVariableData = NULL;
  UINTN                           CompressedBufferSize;
  EDKII_VARIABLE_LOCK_PROTOCOL    *VariableLock = NULL;

  RemainingHobSize = sizeof (struct sysNvram);
  DEBUG((EFI_D_INFO, "Entering SaveS3StructToNvram(): sysNvram size to be saved to NVRAM(bytes): %d\n", RemainingHobSize));
  
  //
  // Get first S3 data HOB
  //
  GuidHob = GetFirstGuidHob (&gEfiMemoryConfigDataHobGuid);
  // APTIOV_SERVER_OVERRIDE_RC_START : Return if gEfiMemoryConfigData Hob is not found
  if (GuidHob == NULL) {
    return ;
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : Return if gEfiMemoryConfigData Hob is not found

  Status = gBS->LocateProtocol (&gEfiDecompressProtocolGuid, NULL, (VOID **) &Decompress);
  if(EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return;
  }
  
  Status = gBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid, NULL, (VOID **)&VariableLock);
  DEBUG((DEBUG_INFO, "[SaveMemoryConfigEntryPoint] Locate Variable Lock protocol - %r\n", Status));
  ASSERT_EFI_ERROR(Status); 

  while (RemainingHobSize) {
    ASSERT (GuidHob);
    if (GuidHob == NULL) {
      return;
    }
    HobData = GET_GUID_HOB_DATA(GuidHob); 
    CurrentHobSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
    
    // Trim the Hob data padding so there is no overflow on the buffer
    //
    if(CurrentHobSize > RemainingHobSize){
      CurrentHobSize = RemainingHobSize;
    }


    if (CurrentHobSize < MAX_HOB_ENTRY_SIZE){
      RemainingHobSize = 0;
    } else {
      RemainingHobSize -= CurrentHobSize;
    }
     DEBUG((EFI_D_INFO, "   Remaining Hob Size(bytes) now is: %d\n", RemainingHobSize));
    //
    // Use the HOB data to save Memory Configuration Data
    //
    BufferSize = CurrentHobSize;
    Status = gBS->AllocatePool ( EfiBootServicesData,
                      BufferSize,
                      (VOID**)&VariableData
                      );

    ASSERT (VariableData != NULL); 
    S3ChunkSize = MAX_HOB_ENTRY_SIZE / 8;
    DEBUG((EFI_D_INFO, "   S3ChunkSize Hob Size(bytes): %d\n", S3ChunkSize));

    while (CurrentHobSize) {
      if (S3ChunkSize > CurrentHobSize) {
        S3ChunkSize = CurrentHobSize;
      }
      BufferSize = S3ChunkSize;
      CompressedDataSize = 0;
      ScratchSize = 0;
      Status = gRT->GetVariable (
                    EfiMemoryConfigVariable,
                    &gEfiMemoryConfigDataGuid,
                    NULL,
                    &CompressedDataSize,
                    NULL
                    );
                    
      if(Status == EFI_BUFFER_TOO_SMALL) {
        Status = gBS->AllocatePool ( EfiBootServicesData,
                        CompressedDataSize,
                        (VOID**)&CompressedData
                        );
        ASSERT (Status == EFI_SUCCESS); 
      }

      if(!EFI_ERROR (Status))
      {
    Status = gRT->GetVariable (
                    EfiMemoryConfigVariable,
                    &gEfiMemoryConfigDataGuid,
                    NULL,
                         &CompressedDataSize,
                         CompressedData
                         );

        if (!EFI_ERROR (Status)) {
         Status = Decompress->GetInfo (
                         Decompress,
                         CompressedData,
                         (UINT32)CompressedDataSize,
                         (UINT32*)&BufferSize,
                         &ScratchSize
                         );
        }

        if (!EFI_ERROR (Status)) {
          Status = gBS->AllocatePool ( EfiBootServicesData,
                         ScratchSize,
                         (VOID**)&Scratch
                         );
        }

        if (!EFI_ERROR (Status)) {
          Status = Decompress->Decompress (
                          Decompress,
                          CompressedData,
                          (UINT32)CompressedDataSize,
                           VariableData,
                           (UINT32)BufferSize,
                           Scratch,
                           ScratchSize
                           );
        }

        if (EFI_ERROR (Status)) {
          DEBUG((EFI_D_ERROR, "Getting variables error: 0x%x\n", Status));
          ASSERT (Status == EFI_SUCCESS); 
        }

        if(Scratch != NULL) {
          gBS->FreePool (Scratch);
          Scratch = NULL;
        }
      }

      if(CompressedData != NULL) {
        gBS->FreePool (CompressedData);
        CompressedData = NULL;
      }

      if ( (EFI_ERROR(Status)) || (CompareMem (HobData, VariableData, S3ChunkSize) != 0) ) {
        Status = gBS->AllocatePool ( EfiBootServicesData,
                          BufferSize,
                          (VOID**)&CompressedVariableData
                    );
        ASSERT (CompressedVariableData != NULL); 
        if (Status == EFI_SUCCESS) {
          CompressedBufferSize = BufferSize;
          Status = Compress(HobData, S3ChunkSize, CompressedVariableData, &CompressedBufferSize);
          if (Status == EFI_BUFFER_TOO_SMALL){
            gBS->FreePool(CompressedVariableData);
            Status = gBS->AllocatePool(EfiBootServicesData,
                    CompressedBufferSize,
                    (VOID**)&CompressedVariableData
                    );
            ASSERT (CompressedVariableData != NULL);
            Status = Compress(HobData, S3ChunkSize, CompressedVariableData, &CompressedBufferSize);
          }
          if(Status == EFI_SUCCESS) {
            Status = gRT->SetVariable (
                    EfiMemoryConfigVariable,
                    &gEfiMemoryConfigDataGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                           CompressedBufferSize,
                           CompressedVariableData
                    );
          }
          if(CompressedVariableData != NULL) {
            gBS->FreePool(CompressedVariableData);
            CompressedVariableData = NULL;
          }
      }
    
        if (EFI_ERROR (Status)) {
          DEBUG((EFI_D_ERROR, "Set variable error. Status: 0x%x\n", Status));
          ASSERT_EFI_ERROR (Status);
        }
      }
      //
      // Lock the Memory Config Variable
      //
      Status = VariableLock->RequestToLock(VariableLock, EfiMemoryConfigVariable, &gEfiMemoryConfigDataGuid);
      ASSERT_EFI_ERROR(Status);
      HobData = (UINT8 *) (HobData) + S3ChunkSize;
    
      CurrentHobSize -= S3ChunkSize;
      EfiMemoryConfigVariable[12]++;  // Increment number in the string
    }
    //
    // Get next S3 Config data hob, if none left, results NULL
    //
    GuidHob = GET_NEXT_HOB (GuidHob); // Increment to next HOB
    GuidHob = GetNextGuidHob (&gEfiMemoryConfigDataHobGuid, GuidHob);  // Now search for next MemConfig HOB

    if(VariableData != NULL) {
    gBS->FreePool(VariableData);
        VariableData = NULL;
    }
  }

  return;
}


/**

  This function is used to compare the saved S3 structure from NVRAM
  with the S3 structure populated on the current boot.  This will 
  determine if the structure needs to be saved to NVRAM again or not.

  @param IN VOID  *CurrentBootStruct - Pointer to current boot NVRAM structure
  @param IN VOID  *SavedBootStruct - Pointer to buffer of the saved NVRAM structure
                                     from a previous boot

  @retval BOOLEAN  -  TRUE  - The structures match
              @retval FALSE - There are differences

**/
BOOLEAN
S3NvramCompareStructs (
  IN VOID                               *CurrentBootStruct,
  IN VOID                               *SavedBootStruct
  )
{
  EFI_STATUS                    Status;
  struct sysNvram               *SavedS3    = SavedBootStruct;
  struct sysNvram               *CurrentS3  = CurrentBootStruct;
  UINTN                         CompareValue;
  EFI_PLATFORM_TYPE_PROTOCOL    *mPlatformType;
  BOOLEAN                       StructsMatch = TRUE;
  
  Status = gBS->LocateProtocol (
                &gEfiPlatformTypeProtocolGuid,
                NULL,
                &mPlatformType
                );
  DEBUG((EFI_D_INFO, "S3NvramCompareStructs: Set variable status: %r \n",   Status));
  ASSERT_EFI_ERROR (Status);
  
  //
  // TODO:  This will be updated to check more vital parts of the 
  // NVRAM structure instead of the entire structure.  For now, a compare of
  // the entire NVRAM structure is used
  //
  CompareValue = CompareMem ((VOID*)CurrentS3, (VOID*)SavedS3, sizeof (struct sysNvram));
  DEBUG((EFI_D_INFO, "S3NvramCompareStructs: Set variable CompareValue: %x \n",  CompareValue));

  //
  // SKX_TODO: Do we need to skip saving this structure a 2nd time to avoid garbage collection for STHI boards
  //  
  if (CompareValue != 0)
    StructsMatch = FALSE;

  return StructsMatch;
}


EFI_STATUS
EFIAPI
S3NvramSaveEntry ( 
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  ) 
/**

  This is the main entry point of the S3 NVRAM Save module.

  @param ImageHandle  -  Handle for the image of this driver.
  @param SystemTable  -  Pointer to the EFI System Table.

  @retval EFI_SUCCESS  -  Module launched successfully.

**/
{
  EFI_STATUS Status = EFI_SUCCESS;
  
  //
  // Save the s3 strututre from MRC into NVRAM if needed
  //
  SaveS3StructToNvram();
  
  return Status;
  
}
