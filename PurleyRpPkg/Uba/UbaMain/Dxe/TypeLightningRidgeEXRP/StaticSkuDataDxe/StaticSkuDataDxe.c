//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA static sku data update dxe driver.

  Copyright (c) 2013 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "StaticSkuDataDxe.h"

/**
  The Driver Entry Point.
  
  The function is the driver Entry point.
  
  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
StaticSkuConfigDataDxeEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                              Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;

  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  Status = InstallMpTableData (UbaConfigProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = InstallPirqData (UbaConfigProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  Status = InstallAcpiFixupTableData (UbaConfigProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  return Status;
}
