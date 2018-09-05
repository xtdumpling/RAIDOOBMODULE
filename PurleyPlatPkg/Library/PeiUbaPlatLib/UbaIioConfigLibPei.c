//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  PeiUbaIioConfigLib implementation.

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
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Ppi/UbaCfgDb.h>
#include <Library/UbaIioConfigLib.h>

EFI_STATUS
PlatformIioConfigInit (
  IN OUT IIO_BIFURCATION_DATA_ENTRY       **BifurcationTable,
  IN OUT UINT8                            *BifurcationEntries,
  IN OUT IIO_SLOT_CONFIG_DATA_ENTRY       **SlotTable,
  IN OUT UINT8                            *SlotEntries
)
{ 
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PPI           *UbaConfigPpi = NULL;
  PLATFORM_IIO_CONFIG_UPDATE_TABLE  IioConfigTable;
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
  
  TableSize = sizeof(IioConfigTable);
  Status = UbaConfigPpi->GetData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid, 
                                 &IioConfigTable, 
                                 &TableSize
                                 );
                                 
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ASSERT (IioConfigTable.Signature == PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE);
  ASSERT (IioConfigTable.Version == PLATFORM_IIO_CONFIG_UPDATE_VERSION);
  
  *BifurcationTable = IioConfigTable.IioBifurcationTablePtr;
  *BifurcationEntries = (UINT8) (IioConfigTable.IioBifurcationTableSize/sizeof(IIO_BIFURCATION_DATA_ENTRY));
  
  *SlotTable = IioConfigTable.IioSlotTablePtr;
  *SlotEntries = (UINT8)(IioConfigTable.IioSlotTableSize/sizeof(IIO_SLOT_CONFIG_DATA_ENTRY));
  
  return Status;
}

EFI_STATUS
PlatformIioConfigInit2 (
  IN     UINT8                            SkuPersonalityType,
  IN OUT IIO_BIFURCATION_DATA_ENTRY       **BifurcationTable,
  IN OUT UINT8                            *BifurcationEntries,
  IN OUT IIO_SLOT_CONFIG_DATA_ENTRY       **SlotTable,
  IN OUT UINT8                            *SlotEntries
)
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  UBA_CONFIG_DATABASE_PPI           *UbaConfigPpi = NULL;
  PLATFORM_IIO_CONFIG_UPDATE_TABLE  IioConfigTable;
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
  
  TableSize = sizeof(IioConfigTable);
  if (SkuPersonalityType == 1) {
    Status = UbaConfigPpi->GetData (
                                   UbaConfigPpi, 
                                   &gPlatformIioConfigDataGuid_1, 
                                   &IioConfigTable,
                                   &TableSize
                                   );
  } else if (SkuPersonalityType == 2) {
    Status = UbaConfigPpi->GetData (
                                   UbaConfigPpi, 
                                   &gPlatformIioConfigDataGuid_2, 
                                   &IioConfigTable,
                                   &TableSize
                                   );
  } else if (SkuPersonalityType == 3) {
    Status = UbaConfigPpi->GetData (
                                   UbaConfigPpi, 
                                   &gPlatformIioConfigDataGuid_3, 
                                   &IioConfigTable,
                                   &TableSize
                                   );
  } else {
    Status = UbaConfigPpi->GetData (
                                   UbaConfigPpi, 
                                   &gPlatformIioConfigDataGuid, 
                                   &IioConfigTable,
                                   &TableSize
                                   );
  }                         
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ASSERT (IioConfigTable.Signature == PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE);
  ASSERT (IioConfigTable.Version == PLATFORM_IIO_CONFIG_UPDATE_VERSION);
  
  *BifurcationTable = IioConfigTable.IioBifurcationTablePtr;
  *BifurcationEntries = (UINT8) (IioConfigTable.IioBifurcationTableSize/sizeof(IIO_BIFURCATION_DATA_ENTRY));
  
  *SlotTable = IioConfigTable.IioSlotTablePtr;
  *SlotEntries = (UINT8)(IioConfigTable.IioSlotTableSize/sizeof(IIO_SLOT_CONFIG_DATA_ENTRY));

  return Status;
}

EFI_STATUS
PlatformUpdateIioConfig (
  IN  IIO_GLOBALS             *IioGlobalData
)
{
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PPI           *UbaConfigPpi = NULL;
  PLATFORM_IIO_CONFIG_UPDATE_TABLE  IioConfigTable;
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
  
  TableSize = sizeof(IioConfigTable);
  Status = UbaConfigPpi->GetData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid, 
                                 &IioConfigTable, 
                                 &TableSize
                                 );
                                 
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ASSERT (IioConfigTable.Signature == PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE);
  ASSERT (IioConfigTable.Version == PLATFORM_IIO_CONFIG_UPDATE_VERSION);
  
  Status = IioConfigTable.CallUpdate (IioGlobalData);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
