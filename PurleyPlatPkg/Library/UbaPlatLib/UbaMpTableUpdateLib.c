//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaMpTableUpdateLib implementation.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
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
#include <Library/UbaMpTableUpdateLib.h>

#include <Protocol/UbaCfgDb.h>

EFI_STATUS
PlatformGetMpTableDataPointer (
  IN  DEVICE_DATA                       **DeviceData,
  IN  DEVICE_UPDATE_DATA                **DeviceUpdateData
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_MP_UPDATE_TABLE                MpUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength  = sizeof (MpUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformMpTableConfigDataGuid,
                                    &MpUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (MpUpdateTable.Signature == PLATFORM_MP_TABLE_UPDATE_SIGNATURE);
  ASSERT (MpUpdateTable.Version == PLATFORM_MP_TABLE_UPDATE_VERSION);

  *DeviceData       = MpUpdateTable.DeviceDataPtr;
  *DeviceUpdateData = MpUpdateTable.DeviceUpdateDataPtr;

  return EFI_SUCCESS;
}


