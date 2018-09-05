//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
 FPGA Fv Data library

 Copyright (c) 2015-2016, Intel Corporation. All rights reserved.<BR>
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 **/

#include "FpgaFvDataLibInternal.h"
#include <Library/FpgaFvDataLib.h>

// Internal Functions

UINTN
GetSectionSize (
  IN  EFI_SECTION_TYPE              SectionType,
  IN  VOID                          *Data
  )
{
  UINT32                            SectionSize;
  UINTN                             SectionHeaderSize;
  EFI_COMMON_SECTION_HEADER         *SectionData;

  SectionHeaderSize = sizeof (EFI_COMMON_SECTION_HEADER);
  SectionData = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)Data - SectionHeaderSize);
  if (SectionData->Type != SectionType) {
    SectionHeaderSize = sizeof (EFI_COMMON_SECTION_HEADER2);
    SectionData = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)Data - SectionHeaderSize);
  }
  ASSERT (SectionData->Type == SectionType);

  if (IS_SECTION2(SectionData)) {
    SectionSize = SECTION2_SIZE(SectionData);
  } else {
    SectionSize = SECTION_SIZE(SectionData);
  }

  return SectionSize - SectionHeaderSize;
}


/**
  Returns the Raw image from the FV

  @param NameGuid  - Guid of the File Image
  @param Buffer    - Buffer
  @param Size      - Buffer Size

  @retval EFI_NOT_FOUND
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
FpgaGetRawImage (
  IN EFI_GUID              *NameGuid,
  IN OUT VOID              **Buffer,
  IN OUT UINTN             *Size
  )
{
  EFI_STATUS                  Status;
  UINTN                       Instance;
  EFI_PEI_FV_HANDLE           VolumeHandle;
  EFI_PEI_FILE_HANDLE         FvFileHandle;

  VolumeHandle = NULL;
  Instance = 0;

  for (Instance = 0; ; Instance++) {
    //
    // Traverse all firmware volume instances.
    //
    Status = PeiServicesFfsFindNextVolume (Instance, &VolumeHandle);
      if (EFI_ERROR (Status)) {
      break;
    }
    FvFileHandle = NULL;
    Status = PeiServicesFfsFindFileByName (NameGuid, VolumeHandle, &FvFileHandle);
      if (EFI_ERROR (Status)) {
      continue;
    }
    //
    // Search RAW section.
    //
    Status = PeiServicesFfsFindSectionData (EFI_SECTION_RAW, FvFileHandle, Buffer);
      if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Got it!
    //
    *Size = GetSectionSize (EFI_SECTION_RAW, *Buffer);
    DEBUG ((EFI_D_ERROR, "RAW data size = %X\n", *Size));
    break;
  }

  return Status;
}
