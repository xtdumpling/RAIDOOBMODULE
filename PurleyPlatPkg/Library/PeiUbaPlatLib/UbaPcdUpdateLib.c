//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaPcdUpdateLib implementation.

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
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>

#include <Library/UbaPcdUpdateLib.h>

EFI_STATUS
PlatformUpdatePcds (
  VOID
)
{
  EFI_STATUS                            Status;
  UBA_CONFIG_DATABASE_PPI               *UbaConfigPpi = NULL;
  
  PLATFORM_PCD_UPDATE_TABLE             PcdUpdateTable;
  UINTN                                 Size;

  Status = PeiServicesLocatePpi (
              &gUbaConfigDatabasePpiGuid, 
              0,
              NULL,
              &UbaConfigPpi
              );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Size = sizeof(PcdUpdateTable);
  Status = UbaConfigPpi->GetData (
                              UbaConfigPpi,
                              &gPlatformPcdConfigDataGuid,
                              &PcdUpdateTable,
                              &Size
                              );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (PcdUpdateTable.Signature == PLATFORM_PCD_UPDATE_SIGNATURE);
  ASSERT (PcdUpdateTable.Version == PLATFORM_PCD_UPDATE_VERSION);

  Status = PcdUpdateTable.CallUpdate ();
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}


