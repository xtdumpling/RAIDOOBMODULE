//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaSlotUpdateLib implementation.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>
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
#include <Library/UbaSlotUpdateLib.h>

EFI_STATUS
PlatformGetSlotTableData (
  IN OUT IIO_BROADWAY_ADDRESS_DATA_ENTRY  **BroadwayTable,
  IN OUT UINT8                            *IOU2Setting,
  IN OUT UINT8                            *FlagValue
)
{ 
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PPI           *UbaConfigPpi = NULL;
  PLATFORM_SLOT_UPDATE_TABLE        IioSlotTable;
  UINTN                             TableSize;
  
  Status = PeiServicesLocatePpi (
              &gUbaConfigDatabasePpiGuid, 
              0,
              NULL,
              &UbaConfigPpi
              );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  TableSize = sizeof(IioSlotTable);
  Status = UbaConfigPpi->GetData (
                                 UbaConfigPpi, 
                                 &gPlatformSlotDataGuid, 
                                 &IioSlotTable, 
                                 &TableSize
                                 );
                                 
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ASSERT (IioSlotTable.Signature == PLATFORM_SLOT_UPDATE_SIGNATURE);
  ASSERT (IioSlotTable.Version == PLATFORM_SLOT_UPDATE_VERSION);
  
  *BroadwayTable = IioSlotTable.BroadwayTablePtr;
  *IOU2Setting   = IioSlotTable.GetIOU2Setting (*IOU2Setting);
  *FlagValue      = IioSlotTable.FlagValue;

  return Status;
}

EFI_STATUS
PlatformGetSlotTableData2 (
  IN OUT IIO_BROADWAY_ADDRESS_DATA_ENTRY  **BroadwayTable,
  IN OUT UINT8                            *IOU0Setting,
  IN OUT UINT8                            *FlagValue,
  IN OUT UINT8                            *IOU2Setting,
  IN     UINT8                            SkuPersonalityType
)
{ 
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PPI           *UbaConfigPpi = NULL;
  PLATFORM_SLOT_UPDATE_TABLE2       IioSlotTable;
  UINTN                             TableSize;
  
  Status = PeiServicesLocatePpi (
              &gUbaConfigDatabasePpiGuid, 
              0,
              NULL,
              &UbaConfigPpi
              );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  TableSize = sizeof(IioSlotTable);
  if ((SkuPersonalityType == 1) || (SkuPersonalityType == 3)) {
    Status = UbaConfigPpi->GetData (
                                 UbaConfigPpi, 
                                 &gPlatformSlotDataGuid2_1, 
                                 &IioSlotTable, 
                                 &TableSize
                                 );
  } else {
    Status = UbaConfigPpi->GetData (
                                 UbaConfigPpi, 
                                 &gPlatformSlotDataGuid2, 
                                 &IioSlotTable, 
                                 &TableSize
                                 );
  }
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ASSERT (IioSlotTable.Signature == PLATFORM_SLOT_UPDATE_SIGNATURE);
  ASSERT (IioSlotTable.Version == PLATFORM_SLOT_UPDATE_VERSION);
  
  *BroadwayTable = IioSlotTable.BroadwayTablePtr;
  *IOU0Setting   = IioSlotTable.GetIOU0Setting (*IOU0Setting);
  *FlagValue     = IioSlotTable.FlagValue;
  *IOU2Setting   = IioSlotTable.GetIOU2Setting (SkuPersonalityType, *IOU2Setting);

  return Status;
}
