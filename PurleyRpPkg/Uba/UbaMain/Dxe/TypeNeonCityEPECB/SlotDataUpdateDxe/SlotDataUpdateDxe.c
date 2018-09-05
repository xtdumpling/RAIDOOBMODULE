//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    Slot Data Update.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "SlotDataUpdateDxe.h"

UINT8
GetTypeNeonCityEPECBIOU0Setting (
  UINT8  IOU0Data
)
{
  return IOU0Data;
}

UINT8
GetTypeNeonCityEPECBIOU2Setting (
  UINT8  SkuPersonalityType,
  UINT8  IOU2Data
)
{
  return IOU2Data;
}

PLATFORM_SLOT_UPDATE_TABLE  TypeNeonCityEPECBSlotTable = 
{
  PLATFORM_SLOT_UPDATE_SIGNATURE,
  PLATFORM_SLOT_UPDATE_VERSION,
  
  NULL,
  GetTypeNeonCityEPECBIOU0Setting,
  0
};

PLATFORM_SLOT_UPDATE_TABLE2  TypeNeonCityEPECBSlotTable2 = 
{
  PLATFORM_SLOT_UPDATE_SIGNATURE,
  PLATFORM_SLOT_UPDATE_VERSION,
  
  NULL,
  GetTypeNeonCityEPECBIOU0Setting,
  0,
  GetTypeNeonCityEPECBIOU2Setting
};

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
SlotDataUpdateEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                               Status;
  UBA_CONFIG_DATABASE_PROTOCOL             *UbaConfigProtocol = NULL;

  DEBUG((EFI_D_ERROR, "UBA:SlotDataUpdate-TypeNeonCityEPECB\n"));
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformSlotDataDxeGuid, 
                                     &TypeNeonCityEPECBSlotTable, 
                                     sizeof(TypeNeonCityEPECBSlotTable)
                                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol, 
                                     &gPlatformSlotDataGuid2, 
                                     &TypeNeonCityEPECBSlotTable2, 
                                     sizeof(TypeNeonCityEPECBSlotTable2)
                                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
