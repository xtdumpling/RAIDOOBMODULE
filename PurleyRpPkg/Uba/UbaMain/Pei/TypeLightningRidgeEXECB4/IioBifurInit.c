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
  { Iio_Socket2, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket2, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket2, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket2, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket2, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket3, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket3, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket3, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket3, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket3, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
#if MAX_SOCKET > 4
  { Iio_Socket4, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket4, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket4, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket4, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket4, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket5, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket5, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket5, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket5, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket5, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket6, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket6, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket6, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket6, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket6, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket7, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket7, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket7, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket7, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket7, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
#endif
};

static IIO_SLOT_CONFIG_DATA_ENTRY   IioSlotTable[] = {
  // Port        |  Slot      | Inter  | Power Limit  | Power Limit | Hot       | Vpp        | Vpp         | PcieSSD | PcieSSD     | PcieSSD      | Hidden
  // Index       |            | lock   | Scale        |  Value      | Plug      | Port       | Addr        | Cap     | VppPort     | VppAddr      |
  /// Socket 0: Iuo2 Connected to SLOT 9 or SSDs: read values of QAT and riser.
  ///           Iuo0 Uplink
  ///           Iuo3 Connected to SLOT 1 or 2, and might be HP. Read values of QAT and riser.
  { PORT_1A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0 ,  0x4C          , HIDE    }, //Oculink
  { PORT_1B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1 ,  0x4C          , HIDE    }, //Oculink
  { PORT_1C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0 ,  0x4E          , HIDE    }, //Oculink
  { PORT_1D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1 ,  0x4E          , HIDE    }, //Oculink
  { PORT_2A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE }, // Uplink
  { PORT_3A_INDEX, 1          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  /// Socket 1: DMI no connected on all boards.
  ///           Iuo2 Slot 7 or 5 based on riser information. Might be HP.
  ///           Iuo0 Slot 5 or 7 based on riser information. Might be HP.
  ///           Iuo1, Not connected for RP. BW5 for ECB boards.
  { SOCKET_1_INDEX +
    PORT_0_INDEX,  NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
  { SOCKET_1_INDEX +                                                                                                                 
    PORT_1A_INDEX, 5          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_1_INDEX +                                                          
    PORT_2A_INDEX, 7          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_1_INDEX +                                                          
    PORT_3A_INDEX, 10         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,   0x40       , NOT_HIDE },
  { SOCKET_1_INDEX +                                                          
    PORT_3B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,   0x40       , HIDE    },
  { SOCKET_1_INDEX +                                                          
    PORT_3C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,   0x42       , HIDE    },
  { SOCKET_1_INDEX +                                                                                                    
    PORT_3D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,   0x42       , HIDE    },
  /// Socket 2: DMI connected to slot 14 on ECB, not connected on RP
  ///           Iuo2 Slot 6 or 8 based on riser information. Might be HP.
  ///           Iuo0 Slot 6 or 8 based on riser information. Might be HP.
  ///           Iuo1, Not connected for RP. BW5 for ECB boards.
  { SOCKET_2_INDEX +
    PORT_0_INDEX,  14        , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_2_INDEX +                                                                         
    PORT_1A_INDEX, 6         , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_2_INDEX +                                                                         
    PORT_2A_INDEX, 15        , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_2_INDEX +                                                                         
    PORT_3A_INDEX, 8         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  /// Socket 3: DMI not connected.
  ///           Iuo2 Not Connected for RP. BW5 for ECB.
  ///           Iuo0 Slot 4 or 3 based on riser information. Might be HP.
  ///           Iuo1 Slot 4 or 3 based on riser information. Might be HP.
  { SOCKET_3_INDEX +
    PORT_0_INDEX,  NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
  { SOCKET_3_INDEX +                                                                                                                   
    PORT_1A_INDEX, 19         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_3_INDEX +                                                                                                     
    PORT_2A_INDEX, 4          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_3_INDEX +                                                                                                                   
    PORT_3A_INDEX, 2          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
#if MAX_SOCKET > 4
  /// Socket 4: Iuo2 Connected to SLOT 9 or SSDs: read values of QAT and riser.
  ///           Iuo0 Uplink
  ///           Iuo3 Connected to SLOT 1 or 2, and might be HP. Read values of QAT and riser.
  { SOCKET_4_INDEX + 
    PORT_1A_INDEX, NO_SLT_IMP , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_4_INDEX +                                                                                                                                 
    PORT_2A_INDEX, NO_SLT_IMP , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE }, // Uplink
  { SOCKET_4_INDEX +                                                                                                                                 
    PORT_3A_INDEX, 1          , ENABLE   , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  /// Socket 5: DMI no connected on all boards.
  ///           Iuo2 Slot 7 or 5 based on riser information. Might be HP.
  ///           Iuo0 Slot 5 or 7 based on riser information. Might be HP.
  ///           Iuo1, Not connected for RP. BW5 for ECB boards.
  { SOCKET_5_INDEX +
    PORT_0_INDEX,  NO_SLT_IMP , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
  { SOCKET_5_INDEX +                                                                                                     
    PORT_1A_INDEX, 5          , ENABLE   , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_5_INDEX +                                                                                                     
    PORT_2A_INDEX, 7          , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_5_INDEX +                                                                                                                   
    PORT_3A_INDEX, 10         , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_5_INDEX +                                                                                                     
    PORT_3B_INDEX, 11         , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_5_INDEX +                                                                                                                   
    PORT_3C_INDEX, 12         , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_5_INDEX +                                                                                                     
    PORT_3D_INDEX, 13         , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  /// Socket 6: DMI connected to slot 14 on ECB, not connected on RP
  ///           Iuo2 Slot 6 or 8 based on riser information. Might be HP.
  ///           Iuo0 Slot 6 or 8 based on riser information. Might be HP.
  ///           Iuo1, Not connected for RP. BW5 for ECB boards.
  { SOCKET_6_INDEX +
    PORT_0_INDEX,  14         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_6_INDEX +                                                                                                                                  
    PORT_1A_INDEX, 6          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_6_INDEX +                                                                                                                                  
    PORT_3A_INDEX, 8          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_6_INDEX +                                                                                                                                  
    PORT_2A_INDEX, 15         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_6_INDEX +                                                                                                    
    PORT_2B_INDEX, 16         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_6_INDEX +                                                                                                                                  
    PORT_2C_INDEX, 17         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_6_INDEX +                                                                                                                                  
    PORT_2D_INDEX, 18         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  /// Socket 7: DMI not connected.                                                                                                                    
  ///           Iuo2 Not Connected for RP. BW5 for ECB.                                                                                             
  ///           Iuo0 Slot 4 or 3 based on riser information. Might be HP.
  ///           Iuo1 Slot 4 or 3 based on riser information. Might be HP.
  { SOCKET_7_INDEX +
    PORT_0_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  HIDE   },
  { SOCKET_7_INDEX +                                                                                                                              
    PORT_2A_INDEX, 3         , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_7_INDEX +                                                                                                                                 
    PORT_3A_INDEX, 4         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_7_INDEX +                                                                                                                                 
    PORT_1A_INDEX, 19        , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_7_INDEX +                                                                                                                                 
    PORT_1B_INDEX, 20        , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_7_INDEX +        
    PORT_1C_INDEX, 21        , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
  { SOCKET_7_INDEX +                                                                                                                                
    PORT_1D_INDEX, 22        , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
#endif
};

EFI_STATUS
UpdateLightningRidgeEXECB4IioConfig (
  IN  IIO_GLOBALS             *IioGlobalData
  )
{
  return EFI_SUCCESS;
}

PLATFORM_IIO_CONFIG_UPDATE_TABLE  TypeLightningRidgeEXECB4IioConfigTable = 
{
  PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE,
  PLATFORM_IIO_CONFIG_UPDATE_VERSION,
  
  IioBifurcationTable,
  sizeof(IioBifurcationTable),
  UpdateLightningRidgeEXECB4IioConfig,
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
TypeLightningRidgeEXECB4IioPortBifurcationInit (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                         Status;

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid, 
                                 &TypeLightningRidgeEXECB4IioConfigTable, 
                                 sizeof(TypeLightningRidgeEXECB4IioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid_1, 
                                 &TypeLightningRidgeEXECB4IioConfigTable, 
                                 sizeof(TypeLightningRidgeEXECB4IioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid_2, 
                                 &TypeLightningRidgeEXECB4IioConfigTable, 
                                 sizeof(TypeLightningRidgeEXECB4IioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformIioConfigDataGuid_3, 
                                 &TypeLightningRidgeEXECB4IioConfigTable, 
                                 sizeof(TypeLightningRidgeEXECB4IioConfigTable)
                                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}
