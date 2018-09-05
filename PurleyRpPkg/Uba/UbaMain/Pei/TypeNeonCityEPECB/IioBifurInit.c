//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    IIO Config Update.

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
#include <Library/UbaIioConfigLib.h>
#include <IioPlatformData.h>
#include <Iio/IioRegs.h>

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
  VPP_PORT_0 = 0,
  VPP_PORT_1,
  VPP_PORT_2,
  VPP_PORT_3
} VPP_PORT;

#define ENABLE            1
#define DISABLE           0
#define NO_SLT_IMP        0xFF
#define SLT_IMP           1
#define HIDE              1
#define NOT_HIDE          0
#define VPP_PORT_0        0
#define VPP_PORT_1        1
#define VPP_PORT_MAX      0xFF
#define VPP_ADDR_MAX      0xFF
#define PWR_VAL_MAX       0xFF
#define PWR_SCL_MAX       0xFF

static IIO_BIFURCATION_DATA_ENTRY   IioBifurcationTable[] = 
{
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};

static IIO_SLOT_CONFIG_DATA_ENTRY   IioSlotTable[] = {
  // Port        |  Slot      | Inter   | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD      | PcieSSD     | Hidden
  // Index       |            | lock    | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort      | VppAddr     |
  { PORT_1A_INDEX,  1         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   , 0x40        , NOT_HIDE  },
  { PORT_1B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x40        , HIDE     },
  { PORT_1C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   , 0x42        , HIDE     },
  { PORT_1D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x42        , HIDE     },
  { PORT_2A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  // Slot 2 supports HP:  PCA9555 (CPU0) Address 0x40, SCH (Rev 0.604) P 118 (MRL in J65)
  { PORT_3A_INDEX,  3         , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , ENABLE  , VPP_PORT_0   , 0x44         , NOT_HIDE },
  { PORT_3B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x44         , HIDE    },
  { PORT_3C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   , 0x46         , HIDE    },
  { PORT_3D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x46         , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_0_INDEX,  NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   , 0x4C         , HIDE    }, // Oculink
  // Slot 4 supports HP: PCA9554 (CPU1) Address 0x40, SCH (Rev 0.604) P 121 (MRL in J287)                                                           
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_1A_INDEX,  5         , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_1   , 0x40         , ENABLE  , VPP_PORT_0   , 0x40         , NOT_HIDE },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_1B_INDEX, NO_SLT_IMP , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x40         , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_1C_INDEX, NO_SLT_IMP , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   , 0x42         , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_1D_INDEX, NO_SLT_IMP , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x42         , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_2A_INDEX,  7         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_2B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_2C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                  
    PORT_2D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_3A_INDEX,  6         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   , 0x44         , NOT_HIDE },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_3B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x44         , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                  
    PORT_3C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   , 0x46         , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                                
    PORT_3D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   , 0x46         , HIDE    },
};

EFI_STATUS
UpdateNeonCityEPECBIioConfig (
  IN  IIO_GLOBALS             *IioGlobalData
  )
{
  return EFI_SUCCESS;
}

PLATFORM_IIO_CONFIG_UPDATE_TABLE  TypeNeonCityEPECBIioConfigTable = 
{
  PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE,
  PLATFORM_IIO_CONFIG_UPDATE_VERSION,
  
  IioBifurcationTable,
  sizeof(IioBifurcationTable),
  UpdateNeonCityEPECBIioConfig,
  IioSlotTable,
  sizeof(IioSlotTable)
  
};

/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
TypeNeonCityEPECBIioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                         Status;

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid, 
                                 &TypeNeonCityEPECBIioConfigTable, 
                                 sizeof(TypeNeonCityEPECBIioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid_1, 
                                 &TypeNeonCityEPECBIioConfigTable, 
                                 sizeof(TypeNeonCityEPECBIioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid_2, 
                                 &TypeNeonCityEPECBIioConfigTable, 
                                 sizeof(TypeNeonCityEPECBIioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid_3, 
                                 &TypeNeonCityEPECBIioConfigTable, 
                                 sizeof(TypeNeonCityEPECBIioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
