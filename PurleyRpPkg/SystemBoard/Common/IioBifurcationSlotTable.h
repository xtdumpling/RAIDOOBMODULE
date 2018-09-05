//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file IioBifurcationSlotTable.h

Contains several definitions for table structures used for Bifurcation and slot configuration
on the different platforms.


**/

#ifndef IIOBIFURCATIONSLOTTABLE_H
#define IIOBIFURCATIONSLOTTABLE_H

#include "IioPlatformData.h"

#define QAT_ENABLED                0  // QAT is active-low
#define RISER_PRESENT              0
#define RISER_NOT_PRESENT          1
#define RISER_HP_EN                1
#define RISER_WINGED_IN            0
#define RISER_SLOT9_DISABLE        1

typedef struct {
  UINT8 Socket;
  UINT8 IouNumber;
  UINT8 Bifurcation;
} IIO_BIFURCATION_ENTRY;

typedef struct {
  UINT8 Socket;
  UINT8 IouNumber;
  UINT8 BroadwayAddress; // 0xff, no override bifurcation settings.
                           // 0-2 BW5 card can be present
} IIO_BROADWAY_ADDRESS_ENTRY;

typedef enum {
  Bw5_Addr_0 = 0,
  Bw5_Addr_1, 
  Bw5_Addr_2,
  Bw5_Addr_3,
  Bw5_Addr_Max
} BW5_ADDRESS;

typedef union {
  struct {
    UINT8 PresentSignal:1;
    UINT8 HPConf:1;
    UINT8 WingConf:1;
    UINT8 Slot9En:1;
  } Bits;
  UINT8 Data;
} PCIE_RISER_ID;

enum {
  Iio_PortA = 0,
  Iio_PortB = 1,
  Iio_PortC = 2,
  Iio_PortD = 3
};
typedef enum {
 Iio_Iou0 =0,
 Iio_Iou1,
 Iio_Iou2,
 Iio_Mcp0,
 Iio_Mcp1,
 Iio_IouMax
} IIO_IOUS;

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
  VPP_PORT_0 = 0,
  VPP_PORT_1,
  VPP_PORT_2,
  VPP_PORT_3
};
///
/// Platform Port/Socket assignments.
///

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

typedef struct {
  UINT8   PortIndex;
  UINT8   SlotNumber;     // 0xff if slot not implemented , Slot number if slot implemented
  BOOLEAN InterLockPresent;
  UINT8   SlotPowerLimitScale;
  UINT8   SlotPowerLimitValue;
  BOOLEAN HotPlugCapable;
  UINT8   VppPort; // 0xff if Vpp not enabled
  UINT8   VppAddress;
  BOOLEAN PcieSSDCapable;
  UINT8   PcieSSDVppPort; // 0xff if Vpp not enabled
  UINT8   PcieSSDVppAddress;
  BOOLEAN Hidden;
} IIO_SLOT_CONFIG_ENTRY;

//
// BW5 SMbus slave address
//
#define BW5_SMBUS_ADDRESS                 0x4C
#define PCA9555_COMMAND_CONFIG_PORT0_REG  0x06
#define PCA9555_COMMAND_INPUT_PORT0_REG   0x00
#define NUM_OF_RETRIES                    0x03
#define BW5_0_ID_MASK                     0x0007
#define BW5_1_ID_MASK                     0x0070
#define BW5_2_ID_MASK                     0x0700
#define BW5_3_ID_MASK                     0x7000
#define BW5_CONFIG_REG_MASK               0xFF
#define BW5_CARDS_PRESENT                 0x04
#define BW5_CARD_NOT_PRESENT              0x07


// Bifurcation read from MCU
#define BW5_BIFURCATE_x4x4x4x4  0
#define BW5_BIFURCATE_xxx8x4x4  1
#define BW5_BIFURCATE_x4x4xxx8  2
#define BW5_BIFURCATE_xxx8xxx8  3

typedef union {
  struct {
    UINT8 BifBits:3;
    UINT8 ExtPresent:1;
    UINT8 HotPlugEna:1;
    UINT8 Rsvd:2;
    UINT8 ExtId:1;
  } Bits;
  UINT8 Data;
} BW5_BIFURCATION_DATA_STRUCT;

#endif
