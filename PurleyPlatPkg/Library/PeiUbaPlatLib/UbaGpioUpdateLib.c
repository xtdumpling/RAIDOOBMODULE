//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaGpioUpdateLib implementation.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <PiPei.h>
#include <Uefi/UefiSpec.h>

#include <Ppi/UbaCfgDb.h>

#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Library/UbaGpioUpdateLib.h>

EFI_STATUS
PlatformUpdateGpios (
  VOID
)
{
  EFI_STATUS                            Status;
  
  UBA_CONFIG_DATABASE_PPI               *UbaConfigPpi = NULL;
  PLATFORM_GPIO_UPDATE_TABLE            GpioTable;
  UINTN                                 TableSize;
  UINTN                                 Index;

  Status = PeiServicesLocatePpi (
              &gUbaConfigDatabasePpiGuid, 
              0,
              NULL,
              &UbaConfigPpi
              );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  TableSize = sizeof(GpioTable);
  Status = UbaConfigPpi->GetData (
                                UbaConfigPpi,
                                &gPlatformGpioConfigDataGuid,
                                &GpioTable,
                                &TableSize
                                );
  
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (GpioTable.Signature == PLATFORM_GPIO_UPDATE_SIGNATURE);
  ASSERT (GpioTable.Version == PLATFORM_GPIO_UPDATE_VERSION);

  for (Index = 0; GpioTable.Gpios[Index].Register != PLATFORM_END_OF_GPIO_LIST; Index++) {

    IoWrite32 (GpioTable.Gpios[Index].Register, GpioTable.Gpios[Index].Value);
  }
  
  return Status;
}


