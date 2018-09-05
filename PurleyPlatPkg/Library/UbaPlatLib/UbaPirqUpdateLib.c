//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaPirqDataUpdateLib implementation

  Copyright (c) 2008 - 2014, Intel Corporation. All rights reserved.<BR>
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

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UbaPirqUpdateLib.h>

#include <Protocol/UbaCfgDb.h>

EFI_STATUS
PlatformGetPirqDataPointer (
  IN  PLATFORM_PIRQ_DATA                **PirqData
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_PIRQ_UPDATE_TABLE              PirqUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength  = sizeof (PirqUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformPirqConfigDataGuid,
                                    &PirqUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (PirqUpdateTable.Signature == PLATFORM_PIRQ_UPDATE_SIGNATURE);
  ASSERT (PirqUpdateTable.Version == PLATFORM_PIRQ_UPDATE_VERSION);

  *PirqData = PirqUpdateTable.PirqDataPtr;

  return EFI_SUCCESS;
}


