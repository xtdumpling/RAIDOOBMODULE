//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    Slot Table Update.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "UbaMainPei.h"
#include <Library/UbaSlotUpdateLib.h>
#include <IioPlatformData.h>

typedef enum {
  Iio_Socket0 = 0,
  Iio_Socket1,
  Iio_Socket2,
  Iio_Socket3,
  Iio_Socket4,
  Iio_Socket5,
  Iio_Socket6,
  Iio_Socket7
} IIO_SOCKETS;

typedef enum {
  Iio_Iou0 =0,
  Iio_Iou1,
  Iio_Iou2,
  Iio_Mcp0,
  Iio_Mcp1,
  Iio_IouMax
} IIO_IOUS;

typedef enum {
  Bw5_Addr_0 = 0,
  Bw5_Addr_1, 
  Bw5_Addr_2,
  Bw5_Addr_3,
  Bw5_Addr_Max
} BW5_ADDRESS;


UINT8
GetTypeNeonCityFPGAIOU0Setting (
  UINT8  IOU0Data
)
{
  //
  // Change bifurcation of Port1A-1B as xxx8 when QATGpio enabled.
  //
  IOU0Data = IIO_BIFURCATE_xxx8xxx8;
  return IOU0Data;
}

UINT8
GetTypeNeonCityFPGAIOU2Setting (
  UINT8  SkuPersonalityType,
  UINT8  IOU2Data
)
{
  //
  // PE3 for Socket0 is not an electrical channel compatible with PCIe
  // If BIOS detect non-FPGA SKU needs disable PE3
  //
  // |   CPU0   |   CPU1   | SKU |
  // -----------------------------
  // | Non-FPGA | Non-FPGA |  0  |
  // |   FPGA   |   FPGA   |  1  |
  // |   FPGA   | Non-FPGA |  2  |
  // | Non-FPGA |   FPGA   |  3  |
  //
  if ((SkuPersonalityType == 0) || (SkuPersonalityType == 3)) {
    IOU2Data = IIO_BIFURCATE_xxxxxxxx;
  }

  return IOU2Data;
}

static IIO_BROADWAY_ADDRESS_DATA_ENTRY   SlotTypeNeonCityFPGABroadwayTable[] = {
    {Iio_Socket1, Iio_Iou1, Bw5_Addr_2},
    {Iio_Socket1, Iio_Iou0, Bw5_Addr_1},
    {0xFF, Iio_IouMax, Bw5_Addr_Max},
};

static IIO_BROADWAY_ADDRESS_DATA_ENTRY   SlotTypeNeonCityFPGABroadwayTable_FPGA[] = {
    {Iio_Socket1, Iio_Iou1, Bw5_Addr_2},
    {Iio_Socket1, Iio_Iou2, Bw5_Addr_1},
    {0xFF, Iio_IouMax, Bw5_Addr_Max},
};

PLATFORM_SLOT_UPDATE_TABLE  TypeNeonCityFPGASlotTable = 
{
  PLATFORM_SLOT_UPDATE_SIGNATURE,
  PLATFORM_SLOT_UPDATE_VERSION,
  
  SlotTypeNeonCityFPGABroadwayTable,
  GetTypeNeonCityFPGAIOU0Setting,
  0
};

PLATFORM_SLOT_UPDATE_TABLE2  TypeNeonCityFPGASlotTable2 = 
{
  PLATFORM_SLOT_UPDATE_SIGNATURE,
  PLATFORM_SLOT_UPDATE_VERSION,
  
  SlotTypeNeonCityFPGABroadwayTable,
  GetTypeNeonCityFPGAIOU0Setting,
  0,
  GetTypeNeonCityFPGAIOU2Setting
};

PLATFORM_SLOT_UPDATE_TABLE2  TypeNeonCityFPGASlotTable2_FPGA = 
{
  PLATFORM_SLOT_UPDATE_SIGNATURE,
  PLATFORM_SLOT_UPDATE_VERSION,
  
  SlotTypeNeonCityFPGABroadwayTable_FPGA,
  GetTypeNeonCityFPGAIOU0Setting,
  0,
  GetTypeNeonCityFPGAIOU2Setting
};


/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
TypeNeonCityFPGAInstallSlotTableData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                         Status;

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformSlotDataGuid, 
                                 &TypeNeonCityFPGASlotTable, 
                                 sizeof(TypeNeonCityFPGASlotTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformSlotDataGuid2, 
                                 &TypeNeonCityFPGASlotTable2, 
                                 sizeof(TypeNeonCityFPGASlotTable2)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformSlotDataGuid2_1, 
                                 &TypeNeonCityFPGASlotTable2_FPGA, 
                                 sizeof(TypeNeonCityFPGASlotTable2_FPGA)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
