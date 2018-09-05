//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SystemBoardBifurcationSlotTables.h

  System Board platform PCIe IIO default Bifurcation and SLOT assignments.

**/

#ifndef SYSTEMBOARD_BIFURCATION_SLOT_H
#define SYSTEMBOARD_BIFURCATION_SLOT_H

/// EP Reference platform and Electrical Characterization Board (Neon City)
//   EP RPECB
#include "IioBifurcationSlotTable.h"
// Platform
// Socket        Iou       Default Bifurcation
static IIO_BIFURCATION_ENTRY EP_RP_BifurcationTable[] =
{
  // Neon City IIO bifurcation table (Based on Neon City Block Diagram rev 0.6)
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8x4x4 },
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};

static IIO_BROADWAY_ADDRESS_ENTRY EP_RP_BroadwayTable[] = {
    {Iio_Socket0, Iio_Iou2, Bw5_Addr_0 },
    {Iio_Socket1, Iio_Iou1, Bw5_Addr_2},
    {Iio_Socket1, Iio_Iou0, Bw5_Addr_1 },
};

/// Reference Platform  Electrical Characterization board (Neon City)
/// slot assignments, hot plug configuration and Power limits.
static IIO_SLOT_CONFIG_ENTRY EP_RP_SlotTable[] = {
    // Port        |  Slot      | Inter      | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD     | PcieSSD       | Hidden
    // Index       |            | lock       | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort     | VppAddr       |
    { PORT_1A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0  , 0x4C          , HIDE    },//Oculink
    { PORT_1B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1  , 0x4C          , HIDE    },//Oculink
    { PORT_1C_INDEX,  1         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_2A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    // Slot 2 supports HP:  PCA9555 (CPU0) Addres 0x40, SCH (Rev 0.604) P 118 (MRL in J65)
    { PORT_3A_INDEX,  2         , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , ENABLE  , VPP_PORT_0    , 0x40         , NOT_HIDE },
    { PORT_3B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x40         , HIDE    },
    { PORT_3C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x42         , HIDE    },
    { PORT_3D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x42         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_0_INDEX ,   6        , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    // Slot 4 supports HP: PCA9554 (CPU1) Address 0x40, SCH (Rev 0.604) P 121 (MRL in J287)
    { SOCKET_1_INDEX +
      PORT_1A_INDEX,   4        , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_1   , 0x40         , ENABLE  , VPP_PORT_0    , 0x40         , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_1B_INDEX, NO_SLT_IMP , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x40         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_1C_INDEX, NO_SLT_IMP , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x42         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_1D_INDEX, NO_SLT_IMP , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x42         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_2A_INDEX,  8         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_1   , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x44         , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_2B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x44         , HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                   
      PORT_2C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                   
      PORT_2D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_3A_INDEX,  5         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +                                                                                                                                   
      PORT_3C_INDEX,  7         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    // Note: On Neon  City, Slot 3 is assigned to PCH's PCIE port
};

// Platform
// Socket        Iou       Default Bifurcation
static IIO_BIFURCATION_ENTRY EP_ECB_BifurcationTable[] =
{
  // Neon City IIO bifurcation table (Based on Neon City Block Diagram rev 0.6)
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

/// Reference Platform  Electrical Characterization board (Neon City)
/// slot assignments, hot plug configuration and Power limits.
static IIO_SLOT_CONFIG_ENTRY EP_ECB_SlotTable[] = {
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

/// EX Reference Platform and Electrical Characterization Boards *Lightning Ridge family.
//// EX RP ECB
static IIO_BIFURCATION_ENTRY EX_RPECB_BifurcationTable[] = {
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
};

static IIO_BROADWAY_ADDRESS_ENTRY EX_ECB_BroadwayTable[] = {
    {Iio_Socket1, Iio_Iou2, Bw5_Addr_1 },
    {Iio_Socket2, Iio_Iou1, Bw5_Addr_2 },
    {Iio_Socket3, Iio_Iou0, Bw5_Addr_3 },
};


static IIO_SLOT_CONFIG_ENTRY EX_ECB_SlotTable[] = {
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
    { PORT_3A_INDEX, 1          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
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
    /// Socket 4: Iuo2 Connected to SLOT 9 or SSDs: read values of QAT and riser.
    ///           Iuo0 Uplink
    ///           Iuo3 Connected to SLOT 1 or 2, and might be HP. Read values of QAT and riser.
    { SOCKET_4_INDEX + 
      PORT_1A_INDEX, NO_SLT_IMP , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_4_INDEX +                                                                                                                                 
      PORT_2A_INDEX, NO_SLT_IMP , DISABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE }, // Uplink
    { SOCKET_4_INDEX +                                                                                                                                 
      PORT_3A_INDEX, 1          , ENABLE   , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , ENABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
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
};

static IIO_SLOT_CONFIG_ENTRY EX_RP_SlotTable[] = {
    // Port        |  Slot      | Inter  | Power Limit | Power Limit | Hot      | Vpp           | Vpp         | PcieSSD | PcieSSD     | PcieSSD       | Hidden
    // Index       |            | lock   | Scale       |  Value      | Plug     | Port          | Addr        | Cap     | VppPort     | VppAddr       |
    /// Socket 0: Iuo2 Connected to SLOT 9 or SSDs: read values of QAT and riser.
    ///           Iuo0 Uplink
    ///           Iuo3 Connected to SLOT 1 or 2, and might be HP. Read values of QAT and riser.
    { PORT_1A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,  0x4C        , HIDE    }, //Oculink
    { PORT_1B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,  0x4C        , HIDE    }, //Oculink
    { PORT_1C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,  0x4E        , HIDE    }, //Oculink
    { PORT_1D_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,  0x4E        , HIDE    }, //Oculink
    { PORT_2A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE }, // Uplink
    { PORT_3A_INDEX, 1          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   ,  0x40        , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    /// Socket 1: DMI no connected on all boards.
    ///           Iuo2 Slot 7 or 5 based on riser information. Might be HP.
    ///           Iuo0 Slot 5 or 7 based on riser information. Might be HP.
    ///           Iuo1, Not connected for RP.
    { SOCKET_1_INDEX +
      PORT_0_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
    { SOCKET_1_INDEX +                                                                           
      PORT_1A_INDEX, 5          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_2A_INDEX, 7          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +                                                                          
      PORT_3A_INDEX, 10         , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
    /// Socket 2: DMI Port, not connected on RP
    ///           Iuo2 Slot 6 or 8 based on riser information. Might be HP.
    ///           Iuo1 Slot 6 or 8 based on riser information. Might be HP.
    ///           Iuo0, Not connected for RP.
    { SOCKET_2_INDEX +
      PORT_0_INDEX,  NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
    { SOCKET_2_INDEX +                                                                                                   
      PORT_1A_INDEX, 6          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_2_INDEX +                                                                                                   
      PORT_3A_INDEX, 8          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_2_INDEX +                                                                                                   
      PORT_2A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
    /// Socket 3: DMI not connected.
    ///           Iuo2 Not Connected for RP.
    ///           Iuo0 Slot 4 or 3 based on riser information. Might be HP.
    ///           Iuo1 Slot 4 or 3 based on riser information. Might be HP.
    { SOCKET_3_INDEX +
      PORT_0_INDEX , NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
    { SOCKET_3_INDEX +                                                                                                   
      PORT_2A_INDEX, 3          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_3_INDEX +                                                                                                   
      PORT_3A_INDEX, 4          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_3_INDEX +                                                                                                   
      PORT_1A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
};

/// EP STHI Server with graphics platform (Opal City)

static IIO_BIFURCATION_ENTRY EP_STHI_BifurcationTable[] =
{
  // Opal City (Based on Opal City Block Diagram rev 0.42)
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8xxx8 }, /// To be changed to x4x4x4x4 if QAT is present.
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },

  { Iio_Socket1, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxx8x4x4 },
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};


/// Opal City slot assignments, hot plug configuration and Power limits.
///
static IIO_SLOT_CONFIG_ENTRY EP_STHI_SlotTable[] = {
    // Port        |  Slot      | Inter   | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD      | PcieSSD      | Hidden
    // Index       |            | lock    | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort      | VppAddr      |
    { PORT_1A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE }, // Uplink
    { PORT_1B_INDEX, 3          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_1C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE }, /// SATA or OCULINK
    { PORT_2A_INDEX, 6          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_3A_INDEX, 4          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_0_INDEX,  NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    }, /// DMI not used.
    { SOCKET_1_INDEX +                                                                                                                                
      PORT_1A_INDEX, 2          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +                                                                                                                                
      PORT_2A_INDEX, 5          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +                                                                                                                                
      PORT_3A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +                                                                                                                                
      PORT_3C_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    }, /// Port 3C-3d not used
};

// Platform
// Socket        Iou       Default Bifurcation
static IIO_BIFURCATION_ENTRY EP_DVP_BifurcationTable[] =
{
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8xxx8 },
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

/// DVP
static IIO_BROADWAY_ADDRESS_ENTRY EP_DVP_BroadwayTable[] = {
    { Iio_Socket0, Iio_Iou2, Bw5_Addr_0 },
    { Iio_Socket1, Iio_Iou0, Bw5_Addr_1 },
    { Iio_Socket1, Iio_Iou1, Bw5_Addr_2 }
};

/// EPDVP slot assignments, hot plug config and Power limits.
//// TODO: This is NEON CITY information update when EvDvp information is available.
//// Current EPDVP configuration still have socket1 IUO1 bifurcated as xxx8xxx8
static IIO_SLOT_CONFIG_ENTRY EP_DVP_SlotTable[] = {
    // Port        |  Slot      | Inter   | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD      | PcieSSD      | Hidden
    // Index       |            | lock    | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort      | VppAddr      |
    { PORT_1A_INDEX, 1          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { PORT_1C_INDEX, 1          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
   { PORT_2A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE }, // Uplink
    { PORT_3A_INDEX, 2          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                                                                          
      PORT_0_INDEX,  6          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                                                         
      PORT_1A_INDEX, 4          , ENABLE  , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_1   , 0x40         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                                                                          
      PORT_2A_INDEX, 8          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_1   , 0x42         , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                                                  
      PORT_3A_INDEX, 5          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                                                                                                                  
      PORT_3C_INDEX, 7          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
};

// Platform
// Socket        Iou       Default Bifurcation
static IIO_BIFURCATION_ENTRY WolfPassBifurcationTable[] =
{
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_x4x4xxx8 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxx8x4x4 },
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};


static IIO_SLOT_CONFIG_ENTRY WolfPassSlotTable[] = {
    /// TODO: Get HP info from Wolf pass. Based on Block diagram rev 0.2
    // Port        |  Slot       | Inter   | Power Limit | Power Limit | Hot      | Vpp          | Vpp          | PcieSSD | PcieSSD      | PcieSSD      | Hidden
    // Index       |             | lock    | Scale       |  Value      | Plug     | Port         | Addr         | Cap     | VppPort      | VppAddr      |
    { PORT_1A_INDEX, 1           , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,  0x40        ,  NOT_HIDE },
    { PORT_1B_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,  0x40        ,  HIDE    },
    { PORT_1C_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,  0x42        ,  HIDE    },
    { PORT_1D_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,  0x42        ,  HIDE    },
    { PORT_2A_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE }, /// Uplink
    { PORT_3A_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE }, /// I/O Module
    { PORT_3C_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,  0x4C        ,  HIDE    }, //Oculink
    { PORT_3D_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,  0x4C        ,  HIDE    }, //Oculink
    { SOCKET_1_INDEX +                     
      PORT_0_INDEX,  7           , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                                                            
      PORT_1A_INDEX, 3           , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                     
      PORT_1C_INDEX, 6           , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
    { SOCKET_1_INDEX +                     
      PORT_2A_INDEX, 4           , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,   0x44       ,  NOT_HIDE },
    { SOCKET_1_INDEX +                                                                                                                                  
      PORT_2B_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,   0x44       ,  HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                  
      PORT_2C_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,   0x46       ,  HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                  
      PORT_2D_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,   0x46       ,  HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                  
      PORT_3A_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0   ,   0x4C       ,  HIDE    }, // Oculink
    { SOCKET_1_INDEX +                                                                                                                                  
      PORT_3B_INDEX, NO_SLT_IMP  , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1   ,   0x4C       ,  HIDE    }, // Oculink
    { SOCKET_1_INDEX +                                                 
      PORT_3C_INDEX, 8           , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE  , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX ,  NOT_HIDE },
};


// SKX+FPGA
static IIO_BIFURCATION_ENTRY EP_FPGAS0S1_BifurcationTable[] = {
  // Neon City FPGA IIO bifurcation table (Based on Neon City FPGA Block Diagram rev 1)
  // Iio_Iou0 is not include in SKX+FPGA
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxx8x4x4 },
  // BIOS must bifucarte MCP0 link as 2x8 (FPGA port).
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  // Iio_Iou0 is not include in SKX+FPGA
  { Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  // BIOS must bifucarte MCP1 link as 2x8 (FPGA port).
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};

/// Reference Platform  Electrical Characterization board (Neon City FPGA)
/// slot assignments, hot plug configuration and Power limits.
static IIO_SLOT_CONFIG_ENTRY EP_FPGAS0S1_SlotTable[] = {
    // Port        |  Slot      | Inter      | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD     | PcieSSD       | Hidden
    // Index       |            | lock       | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort     | VppAddr       |
    { PORT_2A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_3A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0  , 0x4C          , HIDE    },//Oculink
    { PORT_3B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1  , 0x4C          , HIDE    },//Oculink
    { PORT_3C_INDEX, 1     	    , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_0_INDEX , 6	        , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_2A_INDEX, 8	         ,DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x44         , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_2B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x44         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_2C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_2D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_3A_INDEX, 4          , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_3B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_3C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_3D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    // Note: On Neon  City, Slot 3 is assigned to PCH's PCIE port
	// Note: On Neon City SKX+FPGA, Slots 2,5 and 7 are assigned to FPGA.
};

///
/// Skt0==FPGA && Skt1==Non-FPGA
///
static IIO_BIFURCATION_ENTRY EP_FPGAS0_BifurcationTable[] = {
  // Neon City FPGA IIO bifurcation table (Based on Neon City FPGA Block Diagram rev 1)
  // Iio_Iou0 is not include in SKX+FPGA
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxx8x4x4 },
  // BIOS must bifucarte MCP0 link as 2x8 (FPGA port).
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },

};

/// Reference Platform  Electrical Characterization board (Neon City FPGA)
/// slot assignments, hot plug configuration and Power limits.
static IIO_SLOT_CONFIG_ENTRY EP_FPGAS0_SlotTable[] = {
    // Port        |  Slot      | Inter      | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD     | PcieSSD       | Hidden
    // Index       |            | lock       | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort     | VppAddr       |
    { PORT_2A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_3A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0  , 0x4C          , HIDE    },//Oculink
    { PORT_3B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1  , 0x4C          , HIDE    },//Oculink
    { PORT_3C_INDEX, 1     	    , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_0_INDEX ,   6        , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    // Slot 4 supports HP: PCA9554 (CPU1) Address 0x40, SCH (Rev 0.604) P 121 (MRL in J287)
    { SOCKET_1_INDEX +
      PORT_1A_INDEX,   4        , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_1   , 0x40         , ENABLE  , VPP_PORT_0    , 0x40         , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_1B_INDEX, NO_SLT_IMP , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x40         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_1C_INDEX, NO_SLT_IMP , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x42         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_1D_INDEX, NO_SLT_IMP , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x42         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_2A_INDEX,  8         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_1   , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x44         , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_2B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x44         , HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                   
      PORT_2C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                   
      PORT_2D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_3A_INDEX,  5         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +                                                                                                                                   
      PORT_3C_INDEX,  7         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },

};

///
/// Skt0==Non-FPGA && Skt1==FPGA
///
static IIO_BIFURCATION_ENTRY EP_FPGAS1_BifurcationTable[] = {
  // Neon City FPGA IIO bifurcation table (Based on Neon City FPGA Block Diagram rev 1)
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8x4x4 },
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  // Iio_Iou0 is not include in SKX+FPGA
  { Iio_Socket1, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Iou2, IIO_BIFURCATE_xxxxxx16 },
  // BIOS must bifucarte MCP1 link as 2x8 (FPGA port).
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};

/// Reference Platform  Electrical Characterization board (Neon City FPGA)
/// slot assignments, hot plug configuration and Power limits.
static IIO_SLOT_CONFIG_ENTRY EP_FPGAS1_SlotTable[] = {
    // Port        |  Slot      | Inter      | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD     | PcieSSD       | Hidden
    // Index       |            | lock       | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort     | VppAddr       |
    { PORT_1A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0  , 0x4C          , HIDE    },//Oculink
    { PORT_1B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1  , 0x4C          , HIDE    },//Oculink
    { PORT_1C_INDEX,  1         , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_2A_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    // Slot 2 supports HP:  PCA9555 (CPU0) Addres 0x40, SCH (Rev 0.604) P 118 (MRL in J65)
    { PORT_3A_INDEX,  2         , ENABLE     , PWR_SCL_MAX , PWR_VAL_MAX , ENABLE  , VPP_PORT_0   , 0x40         , ENABLE  , VPP_PORT_0    , 0x40         , NOT_HIDE },
    { PORT_3B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x40         , HIDE    },
    { PORT_3C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x42         , HIDE    },
    { PORT_3D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x42         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_0_INDEX , 6	        , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_2A_INDEX, 8	         ,DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x44         , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_2B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x44         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_2C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_0    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_2D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , ENABLE  , VPP_PORT_1    , 0x46         , HIDE    },
    { SOCKET_1_INDEX +
      PORT_3A_INDEX, 4		 	, DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
	  PORT_3B_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_3C_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
	{ SOCKET_1_INDEX +
	  PORT_3D_INDEX, NO_SLT_IMP , DISABLE    , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    // Note: On Neon  City, Slot 3 is assigned to PCH's PCIE port
	// Note: On Neon City SKX+FPGA, Slots 2,5 and 7 are assigned to FPGA.
};

/// EP STHI FPGA Server with graphics platform (Opal City STHI FPGA)
/// Skt0==FPGA && Skt1==FPGA
static IIO_BIFURCATION_ENTRY EP_STHI_FPGAS0S1_BifurcationTable[] =
{
  // Opal City FPGA (Based on Opal City STHI FPGA Block Diagram rev 1.0)
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8xxx8 }, /// To be changed to xxx8x4x4 if QAT is present.
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  // Skt1 Iio_Iou0/1/2 is not include in SKX+FPGA
  // BIOS must bifucarte MCP0 link as 2x8 (FPGA port).
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};

///
/// Skt0==FPGA && Skt1==Non-FPGA
///
static IIO_BIFURCATION_ENTRY EP_STHI_FPGAS0_BifurcationTable[] =
{
  // Opal City FPGA (Based on Opal City STHI FPGA Block Diagram rev 1.0)
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8xxx8 }, /// To be changed to xxx8x4x4 if QAT is present.
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  // Skt1 Iio_Iou0/1/2 is not include in SKX+FPGA
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};

///
/// Skt0==Non-FPGA && Skt1==FPGA
///
static IIO_BIFURCATION_ENTRY EP_STHI_FPGAS1_BifurcationTable[] =
{
  // Opal City FPGA (Based on Opal City STHI FPGA Block Diagram rev 1.0)
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8xxx8 }, /// To be changed to xxx8x4x4 if QAT is present.
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  // Skt1 Iio_Iou0/1/2 is not include in SKX+FPGA
  // BIOS must bifucarte MCP0 link as 2x8 (FPGA port).
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};

///
/// Skt0==Non-FPGA && Skt1==Non-FPGA
///
static IIO_BIFURCATION_ENTRY EP_STHI_FPGA_BifurcationTable[] =
{
  // Opal City FPGA (Based on Opal City STHI FPGA Block Diagram rev 1.0)
  { Iio_Socket0, Iio_Iou0, IIO_BIFURCATE_xxx8xxx8 }, /// To be changed to xxx8x4x4 if QAT is present.
  { Iio_Socket0, Iio_Iou1, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Iou2, IIO_BIFURCATE_xxx8xxx8 },
  { Iio_Socket0, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket0, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
  // Skt1 Iio_Iou0/1/2 is not include in SKX+FPGA
  // BIOS must bifucarte MCP0 link as 2x8 (FPGA port).
  { Iio_Socket1, Iio_Mcp0, IIO_BIFURCATE_xxxxxx16 },
  { Iio_Socket1, Iio_Mcp1, IIO_BIFURCATE_xxxxxx16 },
};


/// Opal City FPGA slot assignments, hot plug configuration and Power limits.
///
static IIO_SLOT_CONFIG_ENTRY EP_STHI_FPGA_SlotTable[] = {
    // Port        |  Slot      | Inter   | Power Limit | Power Limit | Hot     | Vpp          | Vpp          | PcieSSD | PcieSSD      | PcieSSD      | Hidden
    // Index       |            | lock    | Scale       |  Value      | Plug    | Port         | Addr         | Cap     | VppPort      | VppAddr      |
    { PORT_1A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE }, // Uplink
    { PORT_1B_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_1C_INDEX, 3          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_2A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_3A_INDEX, 4          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { PORT_3C_INDEX, 6          , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , NOT_HIDE },
    { SOCKET_1_INDEX +
      PORT_0_INDEX,  NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    }, /// DMI not used.
    { SOCKET_1_INDEX +                                                                                                                                
      PORT_1A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                
      PORT_2A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
    { SOCKET_1_INDEX +                                                                                                                                
      PORT_3A_INDEX, NO_SLT_IMP , DISABLE , PWR_SCL_MAX , PWR_VAL_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , DISABLE , VPP_PORT_MAX , VPP_ADDR_MAX , HIDE    },
};


#endif	// SYSTEMBOARD_BIFURCATION_SLOT_H
