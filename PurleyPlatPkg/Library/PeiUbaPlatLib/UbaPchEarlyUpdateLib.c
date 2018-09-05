/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UbaPchEarlyUpdateLib.c

Abstract:

--*/

#include <PiPei.h>
#include <Uefi/UefiSpec.h>

#include <Ppi/UbaCfgDb.h>

#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Library/UbaPchEarlyUpdateLib.h>

EFI_STATUS
PlatformPchLanConfig (
  IN SYSTEM_CONFIGURATION         *SystemConfig
)
{
  EFI_STATUS                            Status;
  
  UBA_CONFIG_DATABASE_PPI         *UbaConfigPpi = NULL;
  PLATFORM_PCH_EARLY_UPDATE_TABLE       PchEarlyUpdateTable;
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

  TableSize = sizeof(PchEarlyUpdateTable);
  Status = UbaConfigPpi->GetData (
                                UbaConfigPpi,
                                &gPlatformPchEarlyConfigDataGuid,
                                &PchEarlyUpdateTable,
                                &TableSize
                                );
  
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (PchEarlyUpdateTable.Signature == PLATFORM_PCH_EARLY_UPDATE_SIGNATURE);
  ASSERT (PchEarlyUpdateTable.Version == PLATFORM_PCH_EARLY_UPDATE_VERSION);

  Status = PchEarlyUpdateTable.ConfigLan (SystemConfig);
  
  return Status;
}

EFI_STATUS
PlatformInitLateHook (
  IN SYSTEM_CONFIGURATION         *SystemConfig
)
{
  EFI_STATUS                            Status;
  
  UBA_CONFIG_DATABASE_PPI         *UbaConfigPpi = NULL;
  PLATFORM_PCH_EARLY_UPDATE_TABLE       PchEarlyUpdateTable;
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

  TableSize = sizeof(PchEarlyUpdateTable);
  Status = UbaConfigPpi->GetData (
                                UbaConfigPpi,
                                &gPlatformPchEarlyConfigDataGuid,
                                &PchEarlyUpdateTable,
                                &TableSize
                                );
  
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (PchEarlyUpdateTable.Signature == PLATFORM_PCH_EARLY_UPDATE_SIGNATURE);
  ASSERT (PchEarlyUpdateTable.Version == PLATFORM_PCH_EARLY_UPDATE_VERSION);

  if (PchEarlyUpdateTable.InitLateHook == NULL) {
    return EFI_NOT_FOUND;
  }
  
  Status = PchEarlyUpdateTable.InitLateHook (SystemConfig);
  
  return Status;
}


