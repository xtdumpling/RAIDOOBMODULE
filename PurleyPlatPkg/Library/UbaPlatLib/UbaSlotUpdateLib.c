//
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

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UbaSlotUpdateLib.h>
#include <Protocol/UbaCfgDb.h>

EFI_STATUS
PlatformGetSlotTableData (
  IN OUT IIO_BROADWAY_ADDRESS_DATA_ENTRY  **BroadwayTable,
  IN OUT UINT8                            *IOU2Setting,
  IN OUT UINT8                            *FlagValue
  
)
{ 
  EFI_STATUS                        Status;
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  PLATFORM_SLOT_UPDATE_TABLE        IioSlotUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  DataLength = sizeof(IioSlotUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                   UbaConfigProtocol, 
                                   &gPlatformSlotDataDxeGuid, 
                                   &IioSlotUpdateTable, 
                                   &DataLength
                                   );
                                 
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ASSERT (IioSlotUpdateTable.Signature == PLATFORM_SLOT_UPDATE_SIGNATURE);
  ASSERT (IioSlotUpdateTable.Version   == PLATFORM_SLOT_UPDATE_VERSION);
  
  *BroadwayTable = IioSlotUpdateTable.BroadwayTablePtr;
  *IOU2Setting   = IioSlotUpdateTable.GetIOU2Setting (*IOU2Setting);
  *FlagValue      = IioSlotUpdateTable.FlagValue;
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
  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                             DataLength = 0;
  PLATFORM_SLOT_UPDATE_TABLE2       IioSlotUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  DataLength = sizeof(IioSlotUpdateTable);
  if ((SkuPersonalityType == 1) || (SkuPersonalityType == 3)) {
    Status = UbaConfigProtocol->GetData (
                                     UbaConfigProtocol, 
                                     &gPlatformSlotDataDxeGuid2_1, 
                                     &IioSlotUpdateTable, 
                                     &DataLength
                                     );
  } else {
    Status = UbaConfigProtocol->GetData (
                                     UbaConfigProtocol, 
                                     &gPlatformSlotDataDxeGuid2, 
                                     &IioSlotUpdateTable, 
                                     &DataLength
                                     );
  }
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  ASSERT (IioSlotUpdateTable.Signature == PLATFORM_SLOT_UPDATE_SIGNATURE);
  ASSERT (IioSlotUpdateTable.Version   == PLATFORM_SLOT_UPDATE_VERSION);

  *BroadwayTable = IioSlotUpdateTable.BroadwayTablePtr;
  *IOU0Setting   = IioSlotUpdateTable.GetIOU0Setting (*IOU0Setting);
  *FlagValue     = IioSlotUpdateTable.FlagValue;
  *IOU2Setting   = IioSlotUpdateTable.GetIOU2Setting (SkuPersonalityType, *IOU2Setting);

  return Status;
}