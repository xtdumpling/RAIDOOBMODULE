/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FlashMap.c

  Build GUIDed HOBs for platform specific flash map.

**/
#include <FlashLayout.h>
#include "FlashMap.h"

#include <Ppi/FlashMap.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Guid/FlashMapHob.h>
#include <Guid/SystemNvDataGuid.h>
#include <Guid/SystemNvDataHobGuid.h>
#include <Guid/FirmwareFileSystem3.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>

EFI_STATUS
EFIAPI
GetAreaInfo (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN PEI_FLASH_MAP_PPI            * This,
  IN  EFI_FLASH_AREA_TYPE         AreaType,
  IN  EFI_GUID                    *AreaTypeGuid,
  OUT UINT32                      *NumEntries,
  OUT EFI_FLASH_SUBAREA_ENTRY     **Entries
  );

EFI_GUID                            mFvBlockGuid      = EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL_GUID;

EFI_GUID                            mFfsGuid          = EFI_FIRMWARE_FILE_SYSTEM3_GUID;

EFI_GUID                            mSystemDataGuid   = EFI_SYSTEM_NV_DATA_HOB_GUID;

STATIC PEI_FLASH_MAP_PPI            mFlashMapPpi = { GetAreaInfo };

STATIC EFI_PEI_PPI_DESCRIPTOR       mPpiListFlashMap = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiFlashMapPpiGuid,
  &mFlashMapPpi
};

#define NULL_GUID { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

static EFI_FLASH_AREA_DATA          mFlashAreaData[]  = {
    /* FVMAIN region */
    {
        FLASH_REGION_FVMAIN_BASE,
        FLASH_REGION_FVMAIN_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_MAIN_BIOS,
        0, 0, 0,
        NULL_GUID,
    },
    /* OEM_FV region */
    {
        FLASH_REGION_OEM_FV_BASE,
        FLASH_REGION_OEM_FV_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_MAIN_BIOS,
        0, 0, 0,
        NULL_GUID,
    },
    /* FVWHEA region */
    {
        FLASH_REGION_FVWHEA_BASE,
        FLASH_REGION_FVWHEA_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_MAIN_BIOS,
        0, 0, 0,
        NULL_GUID,
    },
    /* MICROCODE_FV region */
    {
        FLASH_REGION_MICROCODE_FV_BASE,
        FLASH_REGION_MICROCODE_FV_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_GUID_DEFINED,
        0, 0, 0,
        EFI_SYSTEM_NV_DATA_HOB_GUID,
    },
    /* RUNTIME_UPDATABLE region */
    {
        FLASH_REGION_RUNTIME_UPDATABLE_BASE,
        FLASH_REGION_RUNTIME_UPDATABLE_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_GUID_DEFINED,
        0, 0, 0,
        EFI_SYSTEM_NV_DATA_HOB_GUID,
    },
    /* NV_FTW_SPARE region */
    {
        FLASH_REGION_NV_FTW_SPARE_BASE,
        FLASH_REGION_NV_FTW_SPARE_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_FTW_BACKUP,
        0, 0, 0,
        NULL_GUID,
    },
    /* BIOS_ACM_FV_HEADER region */
    {
        FLASH_REGION_BIOS_ACM_FV_HEADER_BASE,
        FLASH_REGION_BIOS_ACM_FV_HEADER_SIZE,
        EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_UNUSED,
        0, 0, 0,
        NULL_GUID,
    },
    /* BIOS_ACM region */
    {
        FLASH_REGION_BIOS_ACM_BASE,
        FLASH_REGION_BIOS_ACM_SIZE,
        EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_UNUSED,
        0, 0, 0,
        NULL_GUID,
    },
    /* SINIT_ACM region */
    {
        FLASH_REGION_SINIT_ACM_BASE,
        FLASH_REGION_SINIT_ACM_SIZE,
        EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_UNUSED,
        0, 0, 0,
        NULL_GUID,
    },
    /* FV_MRC{Normal,Recovery} region: need to be updated according to Normal or Recovery mode, now we make a place holder */
    {
        FLASH_REGION_FV_MRC_NORMAL_BASE,
        FLASH_REGION_FV_MRC_NORMAL_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_RECOVERY_BIOS,
        0, 0, 0,
        NULL_GUID,
    },
    /* FV_SECPEI region */
    {
        FLASH_REGION_FV_SECPEI_BASE,
        FLASH_REGION_FV_SECPEI_SIZE,
        EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
        EFI_FLASH_AREA_RECOVERY_BIOS,
        0, 0, 0,
        NULL_GUID,
    },

};

#define NUM_FLASH_AREA_DATA (sizeof (mFlashAreaData) / sizeof (mFlashAreaData[0]))

static EFI_HOB_FLASH_MAP_ENTRY_TYPE mFlashMapHobData[] = {

   /* RUNTIME_UPDATABLE.NV_VARIABLE_STORE Subregion */
   {
       EFI_HOB_TYPE_GUID_EXTENSION,
       sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),
       0,
       EFI_FLASH_MAP_HOB_GUID,
       0, 0, 0,
       EFI_FLASH_AREA_EFI_VARIABLES,
       { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
       1,
       {
           EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
           0,
           FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_VARIABLE_STORE_BASE,
           FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_VARIABLE_STORE_SIZE,
           { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
       },
   },
   /* RUNTIME_UPDATABLE.NV_FTW_WORKING Subregion */
   {
       EFI_HOB_TYPE_GUID_EXTENSION,
       sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE ),
       0,
       EFI_FLASH_MAP_HOB_GUID,
       0, 0, 0,
       EFI_FLASH_AREA_FTW_STATE,
       { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
       1,
       {
           EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
           0,
           FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_FTW_WORKING_BASE,
           FLASH_REGION_RUNTIME_UPDATABLE_SUBREGION_NV_FTW_WORKING_SIZE,
           { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
       },
   },
};

#define NUM_HOB_FLASH_MAP_DATA  (sizeof (mFlashMapHobData) / sizeof (mFlashMapHobData[0]))

/**

    GC_TODO: add routine description

    @param FileHandle  - GC_TODO: add arg description
    @param PeiServices - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
PeimInstallFlashMapPpi (
  IN EFI_PEI_FILE_HANDLE       *FileHandle,
  IN CONST EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_STATUS                    Status;
  //
  // Install FlashMap PPI
  //
  Status = PeiServicesInstallPpi (&mPpiListFlashMap);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "Flash Map PEIM Loaded\n"));

  return EFI_SUCCESS;
}

/**

  Build GUID HOBs for platform specific flash map.

  @param FfsHeader     Pointer this FFS file header.
  @param PeiServices   General purpose services available to every PEIM.

  @retval EFI_SUCCESS   Guid HOBs for platform flash map is built.
  @retval Otherwise     Failed to build the Guid HOB data.

**/
EFI_STATUS
PeimInitializeFlashMap (
  IN EFI_PEI_FILE_HANDLE       *FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_FLASH_AREA_HOB_DATA       FlashHobData;
  EFI_HOB_FLASH_MAP_ENTRY_TYPE  *Hob;
  EFI_BOOT_MODE                 BootMode;

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // Build flash area entries as GUIDed HOBs.
  //
  for (Index = 0; Index < NUM_FLASH_AREA_DATA; Index++) {
    SetMem (&FlashHobData, sizeof (EFI_FLASH_AREA_HOB_DATA), 0);

    FlashHobData.AreaType               = mFlashAreaData[Index].AreaType;
    FlashHobData.NumberOfEntries        = 1;
    FlashHobData.SubAreaData.Attributes = mFlashAreaData[Index].Attributes;
    FlashHobData.SubAreaData.Base       = (EFI_PHYSICAL_ADDRESS) (UINTN) mFlashAreaData[Index].Base;
    FlashHobData.SubAreaData.Length     = (EFI_PHYSICAL_ADDRESS) (UINTN) mFlashAreaData[Index].Length;


    switch (FlashHobData.AreaType) {
    case EFI_FLASH_AREA_RECOVERY_BIOS:
    case EFI_FLASH_AREA_MAIN_BIOS:
      CopyMem (
        &FlashHobData.AreaTypeGuid,
        &mFfsGuid,
        sizeof (EFI_GUID)
        );
      CopyMem (
        &FlashHobData.SubAreaData.FileSystem,
        &mFvBlockGuid,
        sizeof (EFI_GUID)
        );
      break;

    case EFI_FLASH_AREA_GUID_DEFINED:
      CopyMem (
        &FlashHobData.AreaTypeGuid,
        &mSystemDataGuid,
        sizeof (EFI_GUID)
        );
      CopyMem (
        &FlashHobData.SubAreaData.FileSystem,
        &mFvBlockGuid,
        sizeof (EFI_GUID)
        );
      break;

    default:
      break;
    }

    Hob = BuildGuidDataHob (
            &gEfiFlashMapHobGuid,
            &FlashHobData,
            sizeof (EFI_FLASH_AREA_HOB_DATA)
            );
    ASSERT (Hob);
  }

  for (Index = 0; Index < NUM_HOB_FLASH_MAP_DATA; Index++) {
    Status = PeiServicesCreateHob (
              EFI_HOB_TYPE_GUID_EXTENSION,
              (UINT16) (sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE)),
              &Hob
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    CopyMem (
      Hob,
      &mFlashMapHobData[Index],
      sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE)
      );
    if (mFlashMapHobData[Index].AreaType == EFI_FLASH_AREA_EFI_VARIABLES) {
      Hob->Entries[0].Base    = Hob->Entries[0].Base + EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH;
      Hob->Entries[0].Length  = Hob->Entries[0].Length - EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH;
    }
  }

  return EFI_SUCCESS;
}

/**

  Get data from the platform specific flash area map.

  @param PeiServices   General purpose services available to every PEIM.
  @param AreaType      Flash map area type.
  @param AreaTypeGuid  Guid for the flash map area type.
  NumEntries    On return, filled with the number of sub-areas with the same type.
  Entries       On return, filled with entry pointer to the sub-areas.

  @retval EFI_SUCCESS   The type of area exists in the flash map and data is returned.
  @retval EFI_NOT_FOUND The type of area does not exist in the flash map.

**/
EFI_STATUS
EFIAPI
GetAreaInfo (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN PEI_FLASH_MAP_PPI            * This,
  IN  EFI_FLASH_AREA_TYPE         AreaType,
  IN  EFI_GUID                    *AreaTypeGuid,
  OUT UINT32                      *NumEntries,
  OUT EFI_FLASH_SUBAREA_ENTRY     **Entries
  )
{
  EFI_PEI_HOB_POINTERS          Hob;
  EFI_HOB_FLASH_MAP_ENTRY_TYPE  *FlashMapEntry;

  Hob.Raw = GetHobList ();
  while (Hob.Raw != NULL) {
    Hob.Raw = GET_NEXT_HOB (Hob);
    Hob.Raw = GetNextHob (EFI_HOB_TYPE_GUID_EXTENSION, Hob.Raw);
    if (CompareGuid (&Hob.Guid->Name, &gEfiFlashMapHobGuid)) {
      FlashMapEntry = (EFI_HOB_FLASH_MAP_ENTRY_TYPE *) Hob.Raw;
      if ((AreaType != FlashMapEntry->AreaType) ||
          ((AreaType == EFI_FLASH_AREA_GUID_DEFINED) && !CompareGuid (AreaTypeGuid, &FlashMapEntry->AreaTypeGuid))
          ) {
        continue;
      }

      *NumEntries = FlashMapEntry->NumEntries;
      *Entries    = FlashMapEntry->Entries;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}
