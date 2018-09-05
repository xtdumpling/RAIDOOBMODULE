//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file

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
#include <Protocol/UbaCfgDb.h>
#include <Library/UbaUsbOcUpdateLib.h>

EFI_STATUS
PlatformGetUsbOcMappings (
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb20OverCurrentMappings,
  IN OUT   PCH_USB_OVERCURRENT_PIN   **Usb30OverCurrentMappings,
  IN OUT   PCH_USB20_AFE             **Usb20AfeParams
)
{
  EFI_STATUS                            Status;
  UBA_CONFIG_DATABASE_PROTOCOL          *UbaConfigProtocol = NULL;
  PLATFORM_USBOC_UPDATE_TABLE           UsbOcUpdateTable;
  UINTN                                 TableSize;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  TableSize = sizeof(UsbOcUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                UbaConfigProtocol,
                                &gDxePlatformUbaOcConfigDataGuid,
                                &UsbOcUpdateTable,
                                &TableSize
                                );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (UsbOcUpdateTable.Signature == PLATFORM_USBOC_UPDATE_SIGNATURE);
  ASSERT (UsbOcUpdateTable.Version == PLATFORM_USBOC_UPDATE_VERSION);

  UsbOcUpdateTable.CallUsbOcUpdate ( Usb20OverCurrentMappings,
                                     Usb30OverCurrentMappings,
                                     Usb20AfeParams
                                   );

  return Status;
}
