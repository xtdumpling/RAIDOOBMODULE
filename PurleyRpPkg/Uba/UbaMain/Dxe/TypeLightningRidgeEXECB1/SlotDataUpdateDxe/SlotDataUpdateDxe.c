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
GetTypeLightningRidgeEXECB1IOU0Setting (
  UINT8  IOU0Data
)
{
  //
  // Change bifurcation of Port1A-1B as x4x4 when QATGpio enabled.
  //
  IOU0Data = IIO_BIFURCATE_xxx8x4x4;
  return IOU0Data;
}

UINT8
GetTypeLightningRidgeEXECB1IOU2Setting (
  UINT8  SkuPersonalityType,
  UINT8  IOU2Data
)
{
  return IOU2Data;
}

static IIO_BROADWAY_ADDRESS_DATA_ENTRY   SlotTypeLightningRidgeEXECB1BroadwayTable[] = {
    {Iio_Socket1, Iio_Iou2, Bw5_Addr_1 },
    {Iio_Socket2, Iio_Iou1, Bw5_Addr_2 },
    {Iio_Socket3, Iio_Iou0, Bw5_Addr_3 },
};


PLATFORM_SLOT_UPDATE_TABLE  TypeLightningRidgeEXECB1SlotTable = 
{
  PLATFORM_SLOT_UPDATE_SIGNATURE,
  PLATFORM_SLOT_UPDATE_VERSION,
  
  SlotTypeLightningRidgeEXECB1BroadwayTable,
  GetTypeLightningRidgeEXECB1IOU0Setting,
  1
};

PLATFORM_SLOT_UPDATE_TABLE2  TypeLightningRidgeEXECB1SlotTable2 = 
{
  PLATFORM_SLOT_UPDATE_SIGNATURE,
  PLATFORM_SLOT_UPDATE_VERSION,

  SlotTypeLightningRidgeEXECB1BroadwayTable,
  GetTypeLightningRidgeEXECB1IOU0Setting,
  1,
  GetTypeLightningRidgeEXECB1IOU2Setting
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

  DEBUG((EFI_D_ERROR, "UBA:SlotDataUpdate-TypeLightningRidgeEXECB1\n"));
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
                                     &TypeLightningRidgeEXECB1SlotTable, 
                                     sizeof(TypeLightningRidgeEXECB1SlotTable)
                                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol, 
                                     &gPlatformSlotDataGuid2, 
                                     &TypeLightningRidgeEXECB1SlotTable2, 
                                     sizeof(TypeLightningRidgeEXECB1SlotTable2)
                                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
