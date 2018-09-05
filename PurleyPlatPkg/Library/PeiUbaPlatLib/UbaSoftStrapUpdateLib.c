//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaSoftStrapUpdateLib implementation.

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

#include <Library/UbaSoftStrapUpdateLib.h>

EFI_STATUS
PlatformGetSoftStrapTable (
  IN  VOID                    **StrapTable
)
{
  EFI_STATUS                            Status;
  
  UBA_CONFIG_DATABASE_PPI         *UbaConfigPpi = NULL;
  PLATFORM_SOFT_STRAP_UPDATE_TABLE      StrapUpdateTable;
  UINTN                                 TableSize;

  Status = PeiServicesLocatePpi (
              &gUbaConfigDatabasePpiGuid, 
              0,
              NULL,
              &UbaConfigPpi
              );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  TableSize = sizeof(StrapUpdateTable);
  Status = UbaConfigPpi->GetData (
                                UbaConfigPpi,
                                &gPlatformSoftStrapConfigDataGuid,
                                &StrapUpdateTable,
                                &TableSize
                                );
  
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (StrapUpdateTable.Signature == PLATFORM_SOFT_STRAP_UPDATE_SIGNATURE);
  ASSERT (StrapUpdateTable.Version == PLATFORM_SOFT_STRAP_UPDATE_VERSION);

  *StrapTable = StrapUpdateTable.StrapPtr;
  
  return Status;
}


