//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA FPK configuration library

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <Base.h>
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UbaCfgDb.h>
#include <Library/UbaFpkConfigLib.h>

/**
  Retrieves FPK config struct from UBA database

  @retval EFI_SUCCESS           Config struct is retrieved.
  @retval EFI_NOT_FOUND         UBA protocol, platform or data not found.
  @retval EFI_INVALID_PARAMETER If PlatformFpkConfigStruct is NULL.
**/
EFI_STATUS
FpkConfigGetConfigStruct (
  OUT PLATFORM_FPK_CONFIG_STRUCT *PlatformFpkConfigStruct
  )
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength = sizeof (*PlatformFpkConfigStruct);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformFpkConfigDataGuid,
                                    PlatformFpkConfigStruct,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (PlatformFpkConfigStruct->Signature == PLATFORM_FPK_CONFIG_SIGNATURE);
  ASSERT (PlatformFpkConfigStruct->Version   == PLATFORM_FPK_CONFIG_VERSION);

  return EFI_SUCCESS;
}
