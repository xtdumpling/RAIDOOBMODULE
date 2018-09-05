//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
 FPGA Fv Data library

 Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
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

/**
  Returns the Raw image from the FV

  @param[in] NameGuid   - Guid of the File Image
  @param[out] Buffer    - Buffer
  @param[out] Size      - Buffer Size

  @retval EFI_NOT_FOUND
  @retval EFI_SUCCESS

 **/
EFI_STATUS
FpgaGetRawImage (
  IN EFI_GUID       *NameGuid,
  IN OUT VOID       **Buffer,
  IN OUT UINTN      *Size
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINT32                        AuthenticationStatus;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || HandleCount == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    &Fv
                    );
    if (EFI_ERROR(Status)) {
      return EFI_LOAD_ERROR;
    }

    //
    // Try a raw file
    //
    *Buffer = NULL;
    *Size = 0;
    Status = Fv->ReadSection (
                   Fv,
                   NameGuid,
                   EFI_SECTION_RAW,
                   0,
                   Buffer,
                   Size,
                   &AuthenticationStatus
                   );
    if (!EFI_ERROR(Status)) {
      break;
    }
  }

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  if (NULL != HandleBuffer) {
    gBS->FreePool(HandleBuffer);
  }

  return EFI_SUCCESS;
}
