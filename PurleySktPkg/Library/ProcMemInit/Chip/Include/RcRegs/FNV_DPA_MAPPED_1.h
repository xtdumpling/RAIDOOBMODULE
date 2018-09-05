//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      Reference Code                                                    *
// *                                                                        *
// *      Copyright (c) 2007-2016, Intel Corporation.                       *
// *                                                                        *
// *      This software and associated documentation (if any) is furnished  *
// *      under a license and may only be used or copied in accordance      *
// *      with the terms of the license. Except as permitted by such        *
// *      license, no part of this software or documentation may be         *
// *      reproduced, stored in a retrieval system, or transmitted in any   *
// *      form or by any means without the express written consent of       *
// *      Intel Corporation.                                                *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *      This file contains Silicon register definitions.                  *
// *                                                                        *
// **************************************************************************
//
/* Date Stamp: 1/5/2016 */

#ifndef FNV_DPA_MAPPED_1_h
#define FNV_DPA_MAPPED_1_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_DPA_MAPPED_1_DEV 0                                                     */
/* FNV_DPA_MAPPED_1_FUN 1                                                     */

/* BM_BW_ADDR_L_0_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001000)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_0_FNV_DPA_MAPPED_1_REG 0x05014000
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_0_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_1_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001010)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_1_FNV_DPA_MAPPED_1_REG 0x05014010
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_1_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_2_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001020)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_2_FNV_DPA_MAPPED_1_REG 0x05014020
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_2_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_3_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001030)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_3_FNV_DPA_MAPPED_1_REG 0x05014030
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_3_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_4_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001040)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_4_FNV_DPA_MAPPED_1_REG 0x05014040
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_4_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_5_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001050)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_5_FNV_DPA_MAPPED_1_REG 0x05014050
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_5_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_6_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001060)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_6_FNV_DPA_MAPPED_1_REG 0x05014060
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_6_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_7_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001070)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_7_FNV_DPA_MAPPED_1_REG 0x05014070
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_7_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_8_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001080)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_8_FNV_DPA_MAPPED_1_REG 0x05014080
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_8_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_9_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001090)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_9_FNV_DPA_MAPPED_1_REG 0x05014090
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_9_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_10_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_10_FNV_DPA_MAPPED_1_REG 0x050140A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_10_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_11_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_11_FNV_DPA_MAPPED_1_REG 0x050140B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_11_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_12_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_12_FNV_DPA_MAPPED_1_REG 0x050140C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_12_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_13_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_13_FNV_DPA_MAPPED_1_REG 0x050140D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_13_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_14_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_14_FNV_DPA_MAPPED_1_REG 0x050140E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_14_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_15_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_15_FNV_DPA_MAPPED_1_REG 0x050140F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_15_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_16_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001100)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_16_FNV_DPA_MAPPED_1_REG 0x05014100
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_16_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_17_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001110)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_17_FNV_DPA_MAPPED_1_REG 0x05014110
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_17_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_18_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001120)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_18_FNV_DPA_MAPPED_1_REG 0x05014120
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_18_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_19_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001130)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_19_FNV_DPA_MAPPED_1_REG 0x05014130
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_19_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_20_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001140)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_20_FNV_DPA_MAPPED_1_REG 0x05014140
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_20_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_21_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001150)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_21_FNV_DPA_MAPPED_1_REG 0x05014150
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_21_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_22_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001160)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_22_FNV_DPA_MAPPED_1_REG 0x05014160
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_22_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_23_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001170)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_23_FNV_DPA_MAPPED_1_REG 0x05014170
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_23_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_24_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001180)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_24_FNV_DPA_MAPPED_1_REG 0x05014180
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_24_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_25_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001190)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_25_FNV_DPA_MAPPED_1_REG 0x05014190
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_25_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_26_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_26_FNV_DPA_MAPPED_1_REG 0x050141A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_26_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_27_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_27_FNV_DPA_MAPPED_1_REG 0x050141B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_27_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_28_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_28_FNV_DPA_MAPPED_1_REG 0x050141C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_28_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_29_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_29_FNV_DPA_MAPPED_1_REG 0x050141D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_29_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_30_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_30_FNV_DPA_MAPPED_1_REG 0x050141E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_30_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_31_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_31_FNV_DPA_MAPPED_1_REG 0x050141F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_31_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_32_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001200)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_32_FNV_DPA_MAPPED_1_REG 0x05014200
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_32_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_33_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001210)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_33_FNV_DPA_MAPPED_1_REG 0x05014210
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_33_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_34_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001220)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_34_FNV_DPA_MAPPED_1_REG 0x05014220
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_34_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_35_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001230)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_35_FNV_DPA_MAPPED_1_REG 0x05014230
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_35_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_36_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001240)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_36_FNV_DPA_MAPPED_1_REG 0x05014240
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_36_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_37_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001250)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_37_FNV_DPA_MAPPED_1_REG 0x05014250
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_37_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_38_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001260)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_38_FNV_DPA_MAPPED_1_REG 0x05014260
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_38_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_39_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001270)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_39_FNV_DPA_MAPPED_1_REG 0x05014270
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_39_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_40_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001280)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_40_FNV_DPA_MAPPED_1_REG 0x05014280
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_40_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_41_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001290)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_41_FNV_DPA_MAPPED_1_REG 0x05014290
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_41_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_42_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_42_FNV_DPA_MAPPED_1_REG 0x050142A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_42_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_43_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_43_FNV_DPA_MAPPED_1_REG 0x050142B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_43_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_44_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_44_FNV_DPA_MAPPED_1_REG 0x050142C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_44_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_45_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_45_FNV_DPA_MAPPED_1_REG 0x050142D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_45_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_46_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_46_FNV_DPA_MAPPED_1_REG 0x050142E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_46_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_47_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_47_FNV_DPA_MAPPED_1_REG 0x050142F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_47_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_48_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001300)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_48_FNV_DPA_MAPPED_1_REG 0x05014300
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_48_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_49_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001310)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_49_FNV_DPA_MAPPED_1_REG 0x05014310
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_49_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_50_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001320)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_50_FNV_DPA_MAPPED_1_REG 0x05014320
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_50_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_51_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001330)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_51_FNV_DPA_MAPPED_1_REG 0x05014330
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_51_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_52_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001340)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_52_FNV_DPA_MAPPED_1_REG 0x05014340
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_52_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_53_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001350)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_53_FNV_DPA_MAPPED_1_REG 0x05014350
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_53_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_54_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001360)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_54_FNV_DPA_MAPPED_1_REG 0x05014360
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_54_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_55_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001370)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_55_FNV_DPA_MAPPED_1_REG 0x05014370
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_55_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_56_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001380)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_56_FNV_DPA_MAPPED_1_REG 0x05014380
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_56_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_57_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001390)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_57_FNV_DPA_MAPPED_1_REG 0x05014390
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_57_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_58_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_58_FNV_DPA_MAPPED_1_REG 0x050143A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_58_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_59_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_59_FNV_DPA_MAPPED_1_REG 0x050143B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_59_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_60_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_60_FNV_DPA_MAPPED_1_REG 0x050143C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_60_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_61_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_61_FNV_DPA_MAPPED_1_REG 0x050143D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_61_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_62_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_62_FNV_DPA_MAPPED_1_REG 0x050143E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_62_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_63_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_63_FNV_DPA_MAPPED_1_REG 0x050143F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_63_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_64_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001400)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_64_FNV_DPA_MAPPED_1_REG 0x05014400
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_64_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_65_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001410)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_65_FNV_DPA_MAPPED_1_REG 0x05014410
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_65_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_66_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001420)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_66_FNV_DPA_MAPPED_1_REG 0x05014420
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_66_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_67_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001430)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_67_FNV_DPA_MAPPED_1_REG 0x05014430
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_67_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_68_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001440)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_68_FNV_DPA_MAPPED_1_REG 0x05014440
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_68_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_69_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001450)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_69_FNV_DPA_MAPPED_1_REG 0x05014450
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_69_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_70_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001460)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_70_FNV_DPA_MAPPED_1_REG 0x05014460
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_70_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_71_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001470)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_71_FNV_DPA_MAPPED_1_REG 0x05014470
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_71_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_72_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001480)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_72_FNV_DPA_MAPPED_1_REG 0x05014480
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_72_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_73_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001490)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_73_FNV_DPA_MAPPED_1_REG 0x05014490
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_73_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_74_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_74_FNV_DPA_MAPPED_1_REG 0x050144A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_74_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_75_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_75_FNV_DPA_MAPPED_1_REG 0x050144B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_75_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_76_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_76_FNV_DPA_MAPPED_1_REG 0x050144C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_76_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_77_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_77_FNV_DPA_MAPPED_1_REG 0x050144D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_77_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_78_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_78_FNV_DPA_MAPPED_1_REG 0x050144E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_78_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_79_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_79_FNV_DPA_MAPPED_1_REG 0x050144F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_79_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_80_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001500)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_80_FNV_DPA_MAPPED_1_REG 0x05014500
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_80_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_81_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001510)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_81_FNV_DPA_MAPPED_1_REG 0x05014510
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_81_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_82_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001520)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_82_FNV_DPA_MAPPED_1_REG 0x05014520
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_82_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_83_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001530)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_83_FNV_DPA_MAPPED_1_REG 0x05014530
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_83_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_84_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001540)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_84_FNV_DPA_MAPPED_1_REG 0x05014540
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_84_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_85_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001550)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_85_FNV_DPA_MAPPED_1_REG 0x05014550
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_85_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_86_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001560)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_86_FNV_DPA_MAPPED_1_REG 0x05014560
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_86_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_87_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001570)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_87_FNV_DPA_MAPPED_1_REG 0x05014570
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_87_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_88_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001580)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_88_FNV_DPA_MAPPED_1_REG 0x05014580
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_88_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_89_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001590)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_89_FNV_DPA_MAPPED_1_REG 0x05014590
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_89_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_90_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_90_FNV_DPA_MAPPED_1_REG 0x050145A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_90_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_91_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_91_FNV_DPA_MAPPED_1_REG 0x050145B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_91_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_92_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_92_FNV_DPA_MAPPED_1_REG 0x050145C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_92_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_93_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_93_FNV_DPA_MAPPED_1_REG 0x050145D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_93_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_94_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_94_FNV_DPA_MAPPED_1_REG 0x050145E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_94_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_95_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_95_FNV_DPA_MAPPED_1_REG 0x050145F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_95_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_96_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001600)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_96_FNV_DPA_MAPPED_1_REG 0x05014600
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_96_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_97_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001610)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_97_FNV_DPA_MAPPED_1_REG 0x05014610
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_97_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_98_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001620)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_98_FNV_DPA_MAPPED_1_REG 0x05014620
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_98_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_99_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001630)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_99_FNV_DPA_MAPPED_1_REG 0x05014630
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_99_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_100_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001640)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_100_FNV_DPA_MAPPED_1_REG 0x05014640
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_100_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_101_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001650)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_101_FNV_DPA_MAPPED_1_REG 0x05014650
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_101_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_102_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001660)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_102_FNV_DPA_MAPPED_1_REG 0x05014660
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_102_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_103_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001670)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_103_FNV_DPA_MAPPED_1_REG 0x05014670
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_103_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_104_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001680)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_104_FNV_DPA_MAPPED_1_REG 0x05014680
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_104_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_105_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001690)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_105_FNV_DPA_MAPPED_1_REG 0x05014690
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_105_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_106_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_106_FNV_DPA_MAPPED_1_REG 0x050146A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_106_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_107_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_107_FNV_DPA_MAPPED_1_REG 0x050146B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_107_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_108_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_108_FNV_DPA_MAPPED_1_REG 0x050146C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_108_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_109_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_109_FNV_DPA_MAPPED_1_REG 0x050146D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_109_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_110_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_110_FNV_DPA_MAPPED_1_REG 0x050146E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_110_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_111_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_111_FNV_DPA_MAPPED_1_REG 0x050146F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_111_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_112_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001700)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_112_FNV_DPA_MAPPED_1_REG 0x05014700
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_112_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_113_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001710)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_113_FNV_DPA_MAPPED_1_REG 0x05014710
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_113_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_114_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001720)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_114_FNV_DPA_MAPPED_1_REG 0x05014720
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_114_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_115_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001730)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_115_FNV_DPA_MAPPED_1_REG 0x05014730
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_115_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_116_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001740)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_116_FNV_DPA_MAPPED_1_REG 0x05014740
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_116_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_117_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001750)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_117_FNV_DPA_MAPPED_1_REG 0x05014750
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_117_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_118_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001760)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_118_FNV_DPA_MAPPED_1_REG 0x05014760
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_118_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_119_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001770)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_119_FNV_DPA_MAPPED_1_REG 0x05014770
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_119_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_120_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001780)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_120_FNV_DPA_MAPPED_1_REG 0x05014780
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_120_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_121_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001790)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_121_FNV_DPA_MAPPED_1_REG 0x05014790
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_121_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_122_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_122_FNV_DPA_MAPPED_1_REG 0x050147A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_122_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_123_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_123_FNV_DPA_MAPPED_1_REG 0x050147B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_123_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_124_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_124_FNV_DPA_MAPPED_1_REG 0x050147C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_124_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_125_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_125_FNV_DPA_MAPPED_1_REG 0x050147D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_125_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_126_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_126_FNV_DPA_MAPPED_1_REG 0x050147E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_126_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_127_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_127_FNV_DPA_MAPPED_1_REG 0x050147F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_127_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_128_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001800)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_128_FNV_DPA_MAPPED_1_REG 0x05014800
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_128_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_129_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001810)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_129_FNV_DPA_MAPPED_1_REG 0x05014810
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_129_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_130_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001820)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_130_FNV_DPA_MAPPED_1_REG 0x05014820
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_130_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_131_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001830)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_131_FNV_DPA_MAPPED_1_REG 0x05014830
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_131_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_132_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001840)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_132_FNV_DPA_MAPPED_1_REG 0x05014840
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_132_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_133_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001850)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_133_FNV_DPA_MAPPED_1_REG 0x05014850
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_133_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_134_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001860)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_134_FNV_DPA_MAPPED_1_REG 0x05014860
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_134_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_135_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001870)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_135_FNV_DPA_MAPPED_1_REG 0x05014870
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_135_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_136_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001880)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_136_FNV_DPA_MAPPED_1_REG 0x05014880
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_136_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_137_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001890)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_137_FNV_DPA_MAPPED_1_REG 0x05014890
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_137_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_138_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_138_FNV_DPA_MAPPED_1_REG 0x050148A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_138_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_139_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_139_FNV_DPA_MAPPED_1_REG 0x050148B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_139_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_140_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_140_FNV_DPA_MAPPED_1_REG 0x050148C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_140_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_141_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_141_FNV_DPA_MAPPED_1_REG 0x050148D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_141_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_142_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_142_FNV_DPA_MAPPED_1_REG 0x050148E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_142_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_143_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_143_FNV_DPA_MAPPED_1_REG 0x050148F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_143_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_144_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001900)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_144_FNV_DPA_MAPPED_1_REG 0x05014900
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_144_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_145_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001910)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_145_FNV_DPA_MAPPED_1_REG 0x05014910
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_145_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_146_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001920)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_146_FNV_DPA_MAPPED_1_REG 0x05014920
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_146_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_147_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001930)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_147_FNV_DPA_MAPPED_1_REG 0x05014930
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_147_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_148_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001940)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_148_FNV_DPA_MAPPED_1_REG 0x05014940
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_148_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_149_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001950)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_149_FNV_DPA_MAPPED_1_REG 0x05014950
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_149_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_150_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001960)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_150_FNV_DPA_MAPPED_1_REG 0x05014960
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_150_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_151_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001970)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_151_FNV_DPA_MAPPED_1_REG 0x05014970
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_151_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_152_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001980)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_152_FNV_DPA_MAPPED_1_REG 0x05014980
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_152_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_153_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001990)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_153_FNV_DPA_MAPPED_1_REG 0x05014990
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_153_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_154_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019A0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_154_FNV_DPA_MAPPED_1_REG 0x050149A0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_154_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_155_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019B0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_155_FNV_DPA_MAPPED_1_REG 0x050149B0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_155_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_156_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019C0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_156_FNV_DPA_MAPPED_1_REG 0x050149C0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_156_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_157_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019D0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_157_FNV_DPA_MAPPED_1_REG 0x050149D0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_157_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_158_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019E0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_158_FNV_DPA_MAPPED_1_REG 0x050149E0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_158_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_159_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019F0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_159_FNV_DPA_MAPPED_1_REG 0x050149F0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_159_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_160_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A00)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_160_FNV_DPA_MAPPED_1_REG 0x05014A00
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_160_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_161_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A10)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_161_FNV_DPA_MAPPED_1_REG 0x05014A10
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_161_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_162_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A20)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_162_FNV_DPA_MAPPED_1_REG 0x05014A20
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_162_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_163_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A30)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_163_FNV_DPA_MAPPED_1_REG 0x05014A30
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_163_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_164_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A40)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_164_FNV_DPA_MAPPED_1_REG 0x05014A40
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_164_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_165_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A50)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_165_FNV_DPA_MAPPED_1_REG 0x05014A50
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_165_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_166_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A60)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_166_FNV_DPA_MAPPED_1_REG 0x05014A60
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_166_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_167_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A70)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_167_FNV_DPA_MAPPED_1_REG 0x05014A70
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_167_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_168_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A80)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_168_FNV_DPA_MAPPED_1_REG 0x05014A80
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_168_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_169_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A90)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_169_FNV_DPA_MAPPED_1_REG 0x05014A90
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_169_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_170_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AA0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_170_FNV_DPA_MAPPED_1_REG 0x05014AA0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_170_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_171_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AB0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_171_FNV_DPA_MAPPED_1_REG 0x05014AB0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_171_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_172_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AC0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_172_FNV_DPA_MAPPED_1_REG 0x05014AC0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_172_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_173_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AD0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_173_FNV_DPA_MAPPED_1_REG 0x05014AD0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_173_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_174_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AE0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_174_FNV_DPA_MAPPED_1_REG 0x05014AE0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_174_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_175_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AF0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_175_FNV_DPA_MAPPED_1_REG 0x05014AF0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_175_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_176_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B00)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_176_FNV_DPA_MAPPED_1_REG 0x05014B00
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_176_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_177_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B10)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_177_FNV_DPA_MAPPED_1_REG 0x05014B10
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_177_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_178_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B20)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_178_FNV_DPA_MAPPED_1_REG 0x05014B20
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_178_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_179_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B30)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_179_FNV_DPA_MAPPED_1_REG 0x05014B30
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_179_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_180_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B40)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_180_FNV_DPA_MAPPED_1_REG 0x05014B40
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_180_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_181_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B50)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_181_FNV_DPA_MAPPED_1_REG 0x05014B50
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_181_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_182_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B60)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_182_FNV_DPA_MAPPED_1_REG 0x05014B60
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_182_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_183_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B70)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_183_FNV_DPA_MAPPED_1_REG 0x05014B70
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_183_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_184_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B80)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_184_FNV_DPA_MAPPED_1_REG 0x05014B80
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_184_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_185_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B90)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_185_FNV_DPA_MAPPED_1_REG 0x05014B90
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_185_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_186_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BA0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_186_FNV_DPA_MAPPED_1_REG 0x05014BA0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_186_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_187_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BB0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_187_FNV_DPA_MAPPED_1_REG 0x05014BB0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_187_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_188_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BC0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_188_FNV_DPA_MAPPED_1_REG 0x05014BC0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_188_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_189_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BD0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_189_FNV_DPA_MAPPED_1_REG 0x05014BD0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_189_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_190_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BE0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_190_FNV_DPA_MAPPED_1_REG 0x05014BE0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_190_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_191_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BF0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_191_FNV_DPA_MAPPED_1_REG 0x05014BF0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_191_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_192_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C00)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_192_FNV_DPA_MAPPED_1_REG 0x05014C00
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_192_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_193_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C10)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_193_FNV_DPA_MAPPED_1_REG 0x05014C10
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_193_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_194_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C20)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_194_FNV_DPA_MAPPED_1_REG 0x05014C20
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_194_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_195_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C30)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_195_FNV_DPA_MAPPED_1_REG 0x05014C30
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_195_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_196_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C40)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_196_FNV_DPA_MAPPED_1_REG 0x05014C40
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_196_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_197_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C50)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_197_FNV_DPA_MAPPED_1_REG 0x05014C50
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_197_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_198_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C60)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_198_FNV_DPA_MAPPED_1_REG 0x05014C60
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_198_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_199_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C70)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_199_FNV_DPA_MAPPED_1_REG 0x05014C70
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_199_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_200_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C80)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_200_FNV_DPA_MAPPED_1_REG 0x05014C80
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_200_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_201_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C90)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_201_FNV_DPA_MAPPED_1_REG 0x05014C90
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_201_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_202_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CA0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_202_FNV_DPA_MAPPED_1_REG 0x05014CA0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_202_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_203_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CB0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_203_FNV_DPA_MAPPED_1_REG 0x05014CB0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_203_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_204_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CC0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_204_FNV_DPA_MAPPED_1_REG 0x05014CC0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_204_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_205_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CD0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_205_FNV_DPA_MAPPED_1_REG 0x05014CD0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_205_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_206_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CE0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_206_FNV_DPA_MAPPED_1_REG 0x05014CE0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_206_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_207_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CF0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_207_FNV_DPA_MAPPED_1_REG 0x05014CF0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_207_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_208_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D00)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_208_FNV_DPA_MAPPED_1_REG 0x05014D00
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_208_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_209_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D10)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_209_FNV_DPA_MAPPED_1_REG 0x05014D10
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_209_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_210_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D20)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_210_FNV_DPA_MAPPED_1_REG 0x05014D20
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_210_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_211_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D30)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_211_FNV_DPA_MAPPED_1_REG 0x05014D30
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_211_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_212_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D40)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_212_FNV_DPA_MAPPED_1_REG 0x05014D40
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_212_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_213_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D50)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_213_FNV_DPA_MAPPED_1_REG 0x05014D50
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_213_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_214_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D60)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_214_FNV_DPA_MAPPED_1_REG 0x05014D60
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_214_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_215_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D70)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_215_FNV_DPA_MAPPED_1_REG 0x05014D70
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_215_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_216_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D80)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_216_FNV_DPA_MAPPED_1_REG 0x05014D80
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_216_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_217_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D90)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_217_FNV_DPA_MAPPED_1_REG 0x05014D90
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_217_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_218_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DA0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_218_FNV_DPA_MAPPED_1_REG 0x05014DA0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_218_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_219_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DB0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_219_FNV_DPA_MAPPED_1_REG 0x05014DB0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_219_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_220_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DC0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_220_FNV_DPA_MAPPED_1_REG 0x05014DC0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_220_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_221_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DD0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_221_FNV_DPA_MAPPED_1_REG 0x05014DD0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_221_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_222_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DE0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_222_FNV_DPA_MAPPED_1_REG 0x05014DE0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_222_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_223_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DF0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_223_FNV_DPA_MAPPED_1_REG 0x05014DF0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_223_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_224_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E00)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_224_FNV_DPA_MAPPED_1_REG 0x05014E00
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_224_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_225_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E10)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_225_FNV_DPA_MAPPED_1_REG 0x05014E10
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_225_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_226_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E20)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_226_FNV_DPA_MAPPED_1_REG 0x05014E20
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_226_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_227_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E30)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_227_FNV_DPA_MAPPED_1_REG 0x05014E30
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_227_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_228_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E40)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_228_FNV_DPA_MAPPED_1_REG 0x05014E40
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_228_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_229_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E50)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_229_FNV_DPA_MAPPED_1_REG 0x05014E50
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_229_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_230_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E60)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_230_FNV_DPA_MAPPED_1_REG 0x05014E60
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_230_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_231_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E70)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_231_FNV_DPA_MAPPED_1_REG 0x05014E70
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_231_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_232_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E80)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_232_FNV_DPA_MAPPED_1_REG 0x05014E80
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_232_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_233_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E90)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_233_FNV_DPA_MAPPED_1_REG 0x05014E90
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_233_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_234_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EA0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_234_FNV_DPA_MAPPED_1_REG 0x05014EA0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_234_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_235_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EB0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_235_FNV_DPA_MAPPED_1_REG 0x05014EB0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_235_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_236_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EC0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_236_FNV_DPA_MAPPED_1_REG 0x05014EC0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_236_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_237_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001ED0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_237_FNV_DPA_MAPPED_1_REG 0x05014ED0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_237_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_238_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EE0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_238_FNV_DPA_MAPPED_1_REG 0x05014EE0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_238_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_239_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EF0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_239_FNV_DPA_MAPPED_1_REG 0x05014EF0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_239_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_240_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F00)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_240_FNV_DPA_MAPPED_1_REG 0x05014F00
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_240_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_241_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F10)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_241_FNV_DPA_MAPPED_1_REG 0x05014F10
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_241_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_242_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F20)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_242_FNV_DPA_MAPPED_1_REG 0x05014F20
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_242_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_243_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F30)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_243_FNV_DPA_MAPPED_1_REG 0x05014F30
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_243_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_244_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F40)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_244_FNV_DPA_MAPPED_1_REG 0x05014F40
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_244_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_245_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F50)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_245_FNV_DPA_MAPPED_1_REG 0x05014F50
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_245_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_246_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F60)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_246_FNV_DPA_MAPPED_1_REG 0x05014F60
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_246_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_247_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F70)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_247_FNV_DPA_MAPPED_1_REG 0x05014F70
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_247_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_248_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F80)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_248_FNV_DPA_MAPPED_1_REG 0x05014F80
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_248_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_249_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F90)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_249_FNV_DPA_MAPPED_1_REG 0x05014F90
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_249_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_250_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FA0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_250_FNV_DPA_MAPPED_1_REG 0x05014FA0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_250_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_251_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FB0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_251_FNV_DPA_MAPPED_1_REG 0x05014FB0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_251_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_252_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FC0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_252_FNV_DPA_MAPPED_1_REG 0x05014FC0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_252_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_253_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FD0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_253_FNV_DPA_MAPPED_1_REG 0x05014FD0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_253_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_254_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FE0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_254_FNV_DPA_MAPPED_1_REG 0x05014FE0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_254_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_L_255_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FF0)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_ADDR_L_255_FNV_DPA_MAPPED_1_REG 0x05014FF0
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window.
 */
typedef union {
  struct {
    UINT32 dpa : 32;
    /* dpa - Bits[31:0], RW, default = 32'h00000000 
       DPA Address[37:6]
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_L_255_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_0_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001004)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_0_FNV_DPA_MAPPED_1_REG 0x05014004
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_0_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_1_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001014)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_1_FNV_DPA_MAPPED_1_REG 0x05014014
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_1_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_2_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001024)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_2_FNV_DPA_MAPPED_1_REG 0x05014024
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_2_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_3_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001034)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_3_FNV_DPA_MAPPED_1_REG 0x05014034
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_3_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_4_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001044)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_4_FNV_DPA_MAPPED_1_REG 0x05014044
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_4_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_5_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001054)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_5_FNV_DPA_MAPPED_1_REG 0x05014054
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_5_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_6_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001064)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_6_FNV_DPA_MAPPED_1_REG 0x05014064
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_6_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_7_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001074)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_7_FNV_DPA_MAPPED_1_REG 0x05014074
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_7_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_8_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001084)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_8_FNV_DPA_MAPPED_1_REG 0x05014084
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_8_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_9_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001094)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_9_FNV_DPA_MAPPED_1_REG 0x05014094
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_9_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_10_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_10_FNV_DPA_MAPPED_1_REG 0x050140A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_10_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_11_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_11_FNV_DPA_MAPPED_1_REG 0x050140B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_11_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_12_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_12_FNV_DPA_MAPPED_1_REG 0x050140C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_12_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_13_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_13_FNV_DPA_MAPPED_1_REG 0x050140D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_13_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_14_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_14_FNV_DPA_MAPPED_1_REG 0x050140E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_14_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_15_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_15_FNV_DPA_MAPPED_1_REG 0x050140F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_15_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_16_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001104)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_16_FNV_DPA_MAPPED_1_REG 0x05014104
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_16_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_17_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001114)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_17_FNV_DPA_MAPPED_1_REG 0x05014114
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_17_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_18_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001124)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_18_FNV_DPA_MAPPED_1_REG 0x05014124
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_18_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_19_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001134)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_19_FNV_DPA_MAPPED_1_REG 0x05014134
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_19_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_20_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001144)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_20_FNV_DPA_MAPPED_1_REG 0x05014144
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_20_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_21_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001154)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_21_FNV_DPA_MAPPED_1_REG 0x05014154
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_21_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_22_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001164)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_22_FNV_DPA_MAPPED_1_REG 0x05014164
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_22_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_23_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001174)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_23_FNV_DPA_MAPPED_1_REG 0x05014174
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_23_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_24_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001184)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_24_FNV_DPA_MAPPED_1_REG 0x05014184
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_24_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_25_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001194)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_25_FNV_DPA_MAPPED_1_REG 0x05014194
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_25_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_26_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_26_FNV_DPA_MAPPED_1_REG 0x050141A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_26_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_27_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_27_FNV_DPA_MAPPED_1_REG 0x050141B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_27_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_28_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_28_FNV_DPA_MAPPED_1_REG 0x050141C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_28_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_29_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_29_FNV_DPA_MAPPED_1_REG 0x050141D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_29_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_30_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_30_FNV_DPA_MAPPED_1_REG 0x050141E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_30_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_31_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_31_FNV_DPA_MAPPED_1_REG 0x050141F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_31_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_32_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001204)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_32_FNV_DPA_MAPPED_1_REG 0x05014204
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_32_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_33_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001214)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_33_FNV_DPA_MAPPED_1_REG 0x05014214
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_33_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_34_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001224)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_34_FNV_DPA_MAPPED_1_REG 0x05014224
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_34_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_35_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001234)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_35_FNV_DPA_MAPPED_1_REG 0x05014234
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_35_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_36_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001244)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_36_FNV_DPA_MAPPED_1_REG 0x05014244
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_36_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_37_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001254)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_37_FNV_DPA_MAPPED_1_REG 0x05014254
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_37_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_38_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001264)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_38_FNV_DPA_MAPPED_1_REG 0x05014264
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_38_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_39_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001274)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_39_FNV_DPA_MAPPED_1_REG 0x05014274
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_39_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_40_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001284)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_40_FNV_DPA_MAPPED_1_REG 0x05014284
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_40_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_41_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001294)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_41_FNV_DPA_MAPPED_1_REG 0x05014294
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_41_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_42_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_42_FNV_DPA_MAPPED_1_REG 0x050142A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_42_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_43_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_43_FNV_DPA_MAPPED_1_REG 0x050142B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_43_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_44_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_44_FNV_DPA_MAPPED_1_REG 0x050142C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_44_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_45_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_45_FNV_DPA_MAPPED_1_REG 0x050142D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_45_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_46_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_46_FNV_DPA_MAPPED_1_REG 0x050142E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_46_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_47_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_47_FNV_DPA_MAPPED_1_REG 0x050142F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_47_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_48_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001304)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_48_FNV_DPA_MAPPED_1_REG 0x05014304
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_48_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_49_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001314)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_49_FNV_DPA_MAPPED_1_REG 0x05014314
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_49_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_50_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001324)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_50_FNV_DPA_MAPPED_1_REG 0x05014324
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_50_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_51_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001334)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_51_FNV_DPA_MAPPED_1_REG 0x05014334
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_51_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_52_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001344)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_52_FNV_DPA_MAPPED_1_REG 0x05014344
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_52_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_53_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001354)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_53_FNV_DPA_MAPPED_1_REG 0x05014354
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_53_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_54_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001364)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_54_FNV_DPA_MAPPED_1_REG 0x05014364
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_54_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_55_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001374)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_55_FNV_DPA_MAPPED_1_REG 0x05014374
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_55_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_56_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001384)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_56_FNV_DPA_MAPPED_1_REG 0x05014384
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_56_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_57_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001394)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_57_FNV_DPA_MAPPED_1_REG 0x05014394
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_57_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_58_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_58_FNV_DPA_MAPPED_1_REG 0x050143A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_58_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_59_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_59_FNV_DPA_MAPPED_1_REG 0x050143B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_59_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_60_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_60_FNV_DPA_MAPPED_1_REG 0x050143C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_60_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_61_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_61_FNV_DPA_MAPPED_1_REG 0x050143D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_61_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_62_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_62_FNV_DPA_MAPPED_1_REG 0x050143E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_62_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_63_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_63_FNV_DPA_MAPPED_1_REG 0x050143F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_63_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_64_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001404)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_64_FNV_DPA_MAPPED_1_REG 0x05014404
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_64_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_65_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001414)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_65_FNV_DPA_MAPPED_1_REG 0x05014414
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_65_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_66_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001424)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_66_FNV_DPA_MAPPED_1_REG 0x05014424
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_66_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_67_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001434)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_67_FNV_DPA_MAPPED_1_REG 0x05014434
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_67_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_68_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001444)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_68_FNV_DPA_MAPPED_1_REG 0x05014444
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_68_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_69_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001454)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_69_FNV_DPA_MAPPED_1_REG 0x05014454
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_69_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_70_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001464)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_70_FNV_DPA_MAPPED_1_REG 0x05014464
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_70_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_71_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001474)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_71_FNV_DPA_MAPPED_1_REG 0x05014474
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_71_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_72_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001484)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_72_FNV_DPA_MAPPED_1_REG 0x05014484
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_72_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_73_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001494)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_73_FNV_DPA_MAPPED_1_REG 0x05014494
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_73_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_74_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_74_FNV_DPA_MAPPED_1_REG 0x050144A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_74_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_75_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_75_FNV_DPA_MAPPED_1_REG 0x050144B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_75_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_76_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_76_FNV_DPA_MAPPED_1_REG 0x050144C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_76_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_77_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_77_FNV_DPA_MAPPED_1_REG 0x050144D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_77_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_78_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_78_FNV_DPA_MAPPED_1_REG 0x050144E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_78_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_79_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_79_FNV_DPA_MAPPED_1_REG 0x050144F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_79_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_80_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001504)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_80_FNV_DPA_MAPPED_1_REG 0x05014504
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_80_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_81_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001514)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_81_FNV_DPA_MAPPED_1_REG 0x05014514
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_81_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_82_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001524)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_82_FNV_DPA_MAPPED_1_REG 0x05014524
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_82_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_83_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001534)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_83_FNV_DPA_MAPPED_1_REG 0x05014534
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_83_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_84_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001544)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_84_FNV_DPA_MAPPED_1_REG 0x05014544
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_84_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_85_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001554)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_85_FNV_DPA_MAPPED_1_REG 0x05014554
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_85_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_86_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001564)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_86_FNV_DPA_MAPPED_1_REG 0x05014564
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_86_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_87_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001574)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_87_FNV_DPA_MAPPED_1_REG 0x05014574
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_87_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_88_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001584)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_88_FNV_DPA_MAPPED_1_REG 0x05014584
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_88_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_89_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001594)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_89_FNV_DPA_MAPPED_1_REG 0x05014594
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_89_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_90_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_90_FNV_DPA_MAPPED_1_REG 0x050145A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_90_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_91_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_91_FNV_DPA_MAPPED_1_REG 0x050145B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_91_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_92_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_92_FNV_DPA_MAPPED_1_REG 0x050145C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_92_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_93_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_93_FNV_DPA_MAPPED_1_REG 0x050145D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_93_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_94_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_94_FNV_DPA_MAPPED_1_REG 0x050145E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_94_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_95_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_95_FNV_DPA_MAPPED_1_REG 0x050145F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_95_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_96_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001604)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_96_FNV_DPA_MAPPED_1_REG 0x05014604
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_96_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_97_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001614)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_97_FNV_DPA_MAPPED_1_REG 0x05014614
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_97_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_98_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001624)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_98_FNV_DPA_MAPPED_1_REG 0x05014624
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_98_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_99_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001634)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_99_FNV_DPA_MAPPED_1_REG 0x05014634
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_99_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_100_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001644)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_100_FNV_DPA_MAPPED_1_REG 0x05014644
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_100_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_101_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001654)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_101_FNV_DPA_MAPPED_1_REG 0x05014654
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_101_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_102_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001664)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_102_FNV_DPA_MAPPED_1_REG 0x05014664
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_102_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_103_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001674)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_103_FNV_DPA_MAPPED_1_REG 0x05014674
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_103_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_104_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001684)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_104_FNV_DPA_MAPPED_1_REG 0x05014684
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_104_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_105_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001694)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_105_FNV_DPA_MAPPED_1_REG 0x05014694
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_105_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_106_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_106_FNV_DPA_MAPPED_1_REG 0x050146A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_106_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_107_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_107_FNV_DPA_MAPPED_1_REG 0x050146B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_107_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_108_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_108_FNV_DPA_MAPPED_1_REG 0x050146C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_108_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_109_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_109_FNV_DPA_MAPPED_1_REG 0x050146D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_109_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_110_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_110_FNV_DPA_MAPPED_1_REG 0x050146E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_110_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_111_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_111_FNV_DPA_MAPPED_1_REG 0x050146F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_111_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_112_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001704)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_112_FNV_DPA_MAPPED_1_REG 0x05014704
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_112_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_113_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001714)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_113_FNV_DPA_MAPPED_1_REG 0x05014714
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_113_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_114_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001724)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_114_FNV_DPA_MAPPED_1_REG 0x05014724
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_114_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_115_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001734)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_115_FNV_DPA_MAPPED_1_REG 0x05014734
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_115_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_116_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001744)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_116_FNV_DPA_MAPPED_1_REG 0x05014744
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_116_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_117_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001754)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_117_FNV_DPA_MAPPED_1_REG 0x05014754
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_117_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_118_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001764)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_118_FNV_DPA_MAPPED_1_REG 0x05014764
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_118_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_119_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001774)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_119_FNV_DPA_MAPPED_1_REG 0x05014774
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_119_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_120_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001784)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_120_FNV_DPA_MAPPED_1_REG 0x05014784
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_120_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_121_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001794)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_121_FNV_DPA_MAPPED_1_REG 0x05014794
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_121_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_122_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_122_FNV_DPA_MAPPED_1_REG 0x050147A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_122_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_123_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_123_FNV_DPA_MAPPED_1_REG 0x050147B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_123_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_124_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_124_FNV_DPA_MAPPED_1_REG 0x050147C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_124_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_125_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_125_FNV_DPA_MAPPED_1_REG 0x050147D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_125_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_126_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_126_FNV_DPA_MAPPED_1_REG 0x050147E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_126_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_127_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_127_FNV_DPA_MAPPED_1_REG 0x050147F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_127_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_128_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001804)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_128_FNV_DPA_MAPPED_1_REG 0x05014804
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_128_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_129_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001814)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_129_FNV_DPA_MAPPED_1_REG 0x05014814
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_129_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_130_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001824)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_130_FNV_DPA_MAPPED_1_REG 0x05014824
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_130_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_131_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001834)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_131_FNV_DPA_MAPPED_1_REG 0x05014834
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_131_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_132_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001844)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_132_FNV_DPA_MAPPED_1_REG 0x05014844
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_132_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_133_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001854)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_133_FNV_DPA_MAPPED_1_REG 0x05014854
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_133_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_134_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001864)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_134_FNV_DPA_MAPPED_1_REG 0x05014864
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_134_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_135_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001874)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_135_FNV_DPA_MAPPED_1_REG 0x05014874
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_135_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_136_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001884)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_136_FNV_DPA_MAPPED_1_REG 0x05014884
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_136_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_137_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001894)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_137_FNV_DPA_MAPPED_1_REG 0x05014894
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_137_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_138_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_138_FNV_DPA_MAPPED_1_REG 0x050148A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_138_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_139_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_139_FNV_DPA_MAPPED_1_REG 0x050148B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_139_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_140_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_140_FNV_DPA_MAPPED_1_REG 0x050148C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_140_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_141_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_141_FNV_DPA_MAPPED_1_REG 0x050148D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_141_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_142_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_142_FNV_DPA_MAPPED_1_REG 0x050148E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_142_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_143_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_143_FNV_DPA_MAPPED_1_REG 0x050148F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_143_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_144_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001904)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_144_FNV_DPA_MAPPED_1_REG 0x05014904
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_144_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_145_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001914)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_145_FNV_DPA_MAPPED_1_REG 0x05014914
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_145_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_146_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001924)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_146_FNV_DPA_MAPPED_1_REG 0x05014924
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_146_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_147_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001934)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_147_FNV_DPA_MAPPED_1_REG 0x05014934
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_147_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_148_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001944)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_148_FNV_DPA_MAPPED_1_REG 0x05014944
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_148_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_149_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001954)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_149_FNV_DPA_MAPPED_1_REG 0x05014954
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_149_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_150_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001964)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_150_FNV_DPA_MAPPED_1_REG 0x05014964
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_150_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_151_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001974)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_151_FNV_DPA_MAPPED_1_REG 0x05014974
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_151_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_152_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001984)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_152_FNV_DPA_MAPPED_1_REG 0x05014984
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_152_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_153_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001994)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_153_FNV_DPA_MAPPED_1_REG 0x05014994
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_153_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_154_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019A4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_154_FNV_DPA_MAPPED_1_REG 0x050149A4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_154_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_155_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019B4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_155_FNV_DPA_MAPPED_1_REG 0x050149B4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_155_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_156_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019C4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_156_FNV_DPA_MAPPED_1_REG 0x050149C4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_156_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_157_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019D4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_157_FNV_DPA_MAPPED_1_REG 0x050149D4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_157_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_158_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019E4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_158_FNV_DPA_MAPPED_1_REG 0x050149E4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_158_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_159_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019F4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_159_FNV_DPA_MAPPED_1_REG 0x050149F4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_159_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_160_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A04)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_160_FNV_DPA_MAPPED_1_REG 0x05014A04
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_160_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_161_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A14)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_161_FNV_DPA_MAPPED_1_REG 0x05014A14
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_161_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_162_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A24)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_162_FNV_DPA_MAPPED_1_REG 0x05014A24
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_162_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_163_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A34)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_163_FNV_DPA_MAPPED_1_REG 0x05014A34
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_163_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_164_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A44)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_164_FNV_DPA_MAPPED_1_REG 0x05014A44
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_164_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_165_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A54)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_165_FNV_DPA_MAPPED_1_REG 0x05014A54
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_165_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_166_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A64)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_166_FNV_DPA_MAPPED_1_REG 0x05014A64
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_166_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_167_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A74)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_167_FNV_DPA_MAPPED_1_REG 0x05014A74
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_167_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_168_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A84)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_168_FNV_DPA_MAPPED_1_REG 0x05014A84
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_168_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_169_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A94)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_169_FNV_DPA_MAPPED_1_REG 0x05014A94
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_169_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_170_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AA4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_170_FNV_DPA_MAPPED_1_REG 0x05014AA4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_170_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_171_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AB4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_171_FNV_DPA_MAPPED_1_REG 0x05014AB4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_171_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_172_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AC4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_172_FNV_DPA_MAPPED_1_REG 0x05014AC4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_172_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_173_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AD4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_173_FNV_DPA_MAPPED_1_REG 0x05014AD4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_173_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_174_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AE4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_174_FNV_DPA_MAPPED_1_REG 0x05014AE4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_174_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_175_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AF4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_175_FNV_DPA_MAPPED_1_REG 0x05014AF4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_175_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_176_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B04)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_176_FNV_DPA_MAPPED_1_REG 0x05014B04
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_176_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_177_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B14)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_177_FNV_DPA_MAPPED_1_REG 0x05014B14
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_177_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_178_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B24)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_178_FNV_DPA_MAPPED_1_REG 0x05014B24
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_178_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_179_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B34)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_179_FNV_DPA_MAPPED_1_REG 0x05014B34
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_179_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_180_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B44)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_180_FNV_DPA_MAPPED_1_REG 0x05014B44
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_180_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_181_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B54)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_181_FNV_DPA_MAPPED_1_REG 0x05014B54
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_181_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_182_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B64)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_182_FNV_DPA_MAPPED_1_REG 0x05014B64
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_182_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_183_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B74)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_183_FNV_DPA_MAPPED_1_REG 0x05014B74
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_183_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_184_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B84)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_184_FNV_DPA_MAPPED_1_REG 0x05014B84
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_184_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_185_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B94)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_185_FNV_DPA_MAPPED_1_REG 0x05014B94
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_185_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_186_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BA4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_186_FNV_DPA_MAPPED_1_REG 0x05014BA4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_186_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_187_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BB4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_187_FNV_DPA_MAPPED_1_REG 0x05014BB4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_187_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_188_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BC4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_188_FNV_DPA_MAPPED_1_REG 0x05014BC4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_188_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_189_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BD4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_189_FNV_DPA_MAPPED_1_REG 0x05014BD4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_189_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_190_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BE4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_190_FNV_DPA_MAPPED_1_REG 0x05014BE4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_190_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_191_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BF4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_191_FNV_DPA_MAPPED_1_REG 0x05014BF4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_191_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_192_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C04)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_192_FNV_DPA_MAPPED_1_REG 0x05014C04
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_192_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_193_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C14)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_193_FNV_DPA_MAPPED_1_REG 0x05014C14
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_193_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_194_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C24)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_194_FNV_DPA_MAPPED_1_REG 0x05014C24
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_194_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_195_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C34)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_195_FNV_DPA_MAPPED_1_REG 0x05014C34
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_195_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_196_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C44)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_196_FNV_DPA_MAPPED_1_REG 0x05014C44
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_196_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_197_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C54)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_197_FNV_DPA_MAPPED_1_REG 0x05014C54
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_197_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_198_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C64)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_198_FNV_DPA_MAPPED_1_REG 0x05014C64
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_198_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_199_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C74)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_199_FNV_DPA_MAPPED_1_REG 0x05014C74
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_199_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_200_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C84)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_200_FNV_DPA_MAPPED_1_REG 0x05014C84
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_200_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_201_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C94)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_201_FNV_DPA_MAPPED_1_REG 0x05014C94
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_201_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_202_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CA4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_202_FNV_DPA_MAPPED_1_REG 0x05014CA4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_202_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_203_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CB4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_203_FNV_DPA_MAPPED_1_REG 0x05014CB4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_203_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_204_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CC4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_204_FNV_DPA_MAPPED_1_REG 0x05014CC4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_204_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_205_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CD4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_205_FNV_DPA_MAPPED_1_REG 0x05014CD4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_205_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_206_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CE4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_206_FNV_DPA_MAPPED_1_REG 0x05014CE4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_206_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_207_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CF4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_207_FNV_DPA_MAPPED_1_REG 0x05014CF4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_207_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_208_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D04)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_208_FNV_DPA_MAPPED_1_REG 0x05014D04
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_208_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_209_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D14)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_209_FNV_DPA_MAPPED_1_REG 0x05014D14
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_209_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_210_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D24)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_210_FNV_DPA_MAPPED_1_REG 0x05014D24
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_210_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_211_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D34)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_211_FNV_DPA_MAPPED_1_REG 0x05014D34
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_211_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_212_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D44)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_212_FNV_DPA_MAPPED_1_REG 0x05014D44
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_212_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_213_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D54)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_213_FNV_DPA_MAPPED_1_REG 0x05014D54
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_213_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_214_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D64)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_214_FNV_DPA_MAPPED_1_REG 0x05014D64
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_214_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_215_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D74)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_215_FNV_DPA_MAPPED_1_REG 0x05014D74
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_215_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_216_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D84)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_216_FNV_DPA_MAPPED_1_REG 0x05014D84
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_216_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_217_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D94)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_217_FNV_DPA_MAPPED_1_REG 0x05014D94
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_217_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_218_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DA4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_218_FNV_DPA_MAPPED_1_REG 0x05014DA4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_218_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_219_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DB4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_219_FNV_DPA_MAPPED_1_REG 0x05014DB4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_219_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_220_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DC4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_220_FNV_DPA_MAPPED_1_REG 0x05014DC4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_220_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_221_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DD4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_221_FNV_DPA_MAPPED_1_REG 0x05014DD4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_221_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_222_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DE4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_222_FNV_DPA_MAPPED_1_REG 0x05014DE4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_222_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_223_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DF4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_223_FNV_DPA_MAPPED_1_REG 0x05014DF4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_223_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_224_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E04)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_224_FNV_DPA_MAPPED_1_REG 0x05014E04
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_224_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_225_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E14)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_225_FNV_DPA_MAPPED_1_REG 0x05014E14
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_225_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_226_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E24)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_226_FNV_DPA_MAPPED_1_REG 0x05014E24
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_226_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_227_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E34)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_227_FNV_DPA_MAPPED_1_REG 0x05014E34
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_227_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_228_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E44)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_228_FNV_DPA_MAPPED_1_REG 0x05014E44
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_228_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_229_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E54)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_229_FNV_DPA_MAPPED_1_REG 0x05014E54
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_229_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_230_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E64)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_230_FNV_DPA_MAPPED_1_REG 0x05014E64
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_230_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_231_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E74)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_231_FNV_DPA_MAPPED_1_REG 0x05014E74
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_231_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_232_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E84)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_232_FNV_DPA_MAPPED_1_REG 0x05014E84
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_232_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_233_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E94)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_233_FNV_DPA_MAPPED_1_REG 0x05014E94
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_233_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_234_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EA4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_234_FNV_DPA_MAPPED_1_REG 0x05014EA4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_234_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_235_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EB4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_235_FNV_DPA_MAPPED_1_REG 0x05014EB4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_235_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_236_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EC4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_236_FNV_DPA_MAPPED_1_REG 0x05014EC4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_236_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_237_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001ED4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_237_FNV_DPA_MAPPED_1_REG 0x05014ED4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_237_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_238_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EE4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_238_FNV_DPA_MAPPED_1_REG 0x05014EE4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_238_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_239_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EF4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_239_FNV_DPA_MAPPED_1_REG 0x05014EF4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_239_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_240_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F04)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_240_FNV_DPA_MAPPED_1_REG 0x05014F04
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_240_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_241_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F14)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_241_FNV_DPA_MAPPED_1_REG 0x05014F14
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_241_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_242_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F24)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_242_FNV_DPA_MAPPED_1_REG 0x05014F24
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_242_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_243_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F34)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_243_FNV_DPA_MAPPED_1_REG 0x05014F34
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_243_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_244_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F44)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_244_FNV_DPA_MAPPED_1_REG 0x05014F44
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_244_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_245_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F54)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_245_FNV_DPA_MAPPED_1_REG 0x05014F54
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_245_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_246_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F64)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_246_FNV_DPA_MAPPED_1_REG 0x05014F64
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_246_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_247_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F74)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_247_FNV_DPA_MAPPED_1_REG 0x05014F74
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_247_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_248_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F84)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_248_FNV_DPA_MAPPED_1_REG 0x05014F84
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_248_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_249_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F94)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_249_FNV_DPA_MAPPED_1_REG 0x05014F94
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_249_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_250_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FA4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_250_FNV_DPA_MAPPED_1_REG 0x05014FA4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_250_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_251_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FB4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_251_FNV_DPA_MAPPED_1_REG 0x05014FB4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_251_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_252_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FC4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_252_FNV_DPA_MAPPED_1_REG 0x05014FC4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_252_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_253_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FD4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_253_FNV_DPA_MAPPED_1_REG 0x05014FD4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_253_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_254_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FE4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_254_FNV_DPA_MAPPED_1_REG 0x05014FE4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_254_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_ADDR_H_255_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FF4)                                                     */
/* Register default value:              0x00000018                            */
#define BM_BW_ADDR_H_255_FNV_DPA_MAPPED_1_REG 0x05014FF4
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the DPA Base Address of the Block Window and R/W Unlock 
 * Controls 
 */
typedef union {
  struct {
    UINT32 dpa : 5;
    /* dpa - Bits[4:0], RW, default = 5'h18 
       DPA Address[42:38]
     */
    UINT32 rsvd_5 : 11;
    /* rsvd_5 - Bits[15:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 win_size : 8;
    /* win_size - Bits[23:16], RW, default = 8'h0 
       Specifies # of cachelines to be transferred[br]
                       0x0 - 44 - Valid range [br]
                       0x45-ff - Rsvd
     */
    UINT32 rw_lock : 1;
    /* rw_lock - Bits[24:24], RW, default = 1'h0 
       Specifies if Window is setup for reads or writes[br]
                       0 - Window is set for all reads and only directory/metadata 
       writes (no regular writes permitted)[br] 
                       1 - Window is set for all reads and writes
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_ADDR_H_255_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_0_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001008)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_0_FNV_DPA_MAPPED_1_REG 0x05014008
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_0_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_1_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001018)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_1_FNV_DPA_MAPPED_1_REG 0x05014018
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_1_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_2_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001028)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_2_FNV_DPA_MAPPED_1_REG 0x05014028
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_2_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_3_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001038)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_3_FNV_DPA_MAPPED_1_REG 0x05014038
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_3_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_4_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001048)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_4_FNV_DPA_MAPPED_1_REG 0x05014048
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_4_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_5_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001058)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_5_FNV_DPA_MAPPED_1_REG 0x05014058
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_5_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_6_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001068)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_6_FNV_DPA_MAPPED_1_REG 0x05014068
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_6_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_7_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001078)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_7_FNV_DPA_MAPPED_1_REG 0x05014078
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_7_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_8_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001088)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_8_FNV_DPA_MAPPED_1_REG 0x05014088
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_8_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_9_FNV_DPA_MAPPED_1_REG supported on:                          */
/*       EKV (0x40001098)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_9_FNV_DPA_MAPPED_1_REG 0x05014098
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_9_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_10_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_10_FNV_DPA_MAPPED_1_REG 0x050140A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_10_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_11_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_11_FNV_DPA_MAPPED_1_REG 0x050140B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_11_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_12_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_12_FNV_DPA_MAPPED_1_REG 0x050140C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_12_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_13_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_13_FNV_DPA_MAPPED_1_REG 0x050140D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_13_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_14_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_14_FNV_DPA_MAPPED_1_REG 0x050140E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_14_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_15_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400010F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_15_FNV_DPA_MAPPED_1_REG 0x050140F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_15_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_16_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001108)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_16_FNV_DPA_MAPPED_1_REG 0x05014108
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_16_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_17_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001118)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_17_FNV_DPA_MAPPED_1_REG 0x05014118
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_17_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_18_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001128)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_18_FNV_DPA_MAPPED_1_REG 0x05014128
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_18_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_19_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001138)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_19_FNV_DPA_MAPPED_1_REG 0x05014138
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_19_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_20_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001148)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_20_FNV_DPA_MAPPED_1_REG 0x05014148
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_20_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_21_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001158)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_21_FNV_DPA_MAPPED_1_REG 0x05014158
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_21_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_22_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001168)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_22_FNV_DPA_MAPPED_1_REG 0x05014168
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_22_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_23_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001178)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_23_FNV_DPA_MAPPED_1_REG 0x05014178
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_23_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_24_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001188)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_24_FNV_DPA_MAPPED_1_REG 0x05014188
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_24_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_25_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001198)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_25_FNV_DPA_MAPPED_1_REG 0x05014198
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_25_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_26_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_26_FNV_DPA_MAPPED_1_REG 0x050141A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_26_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_27_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_27_FNV_DPA_MAPPED_1_REG 0x050141B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_27_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_28_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_28_FNV_DPA_MAPPED_1_REG 0x050141C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_28_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_29_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_29_FNV_DPA_MAPPED_1_REG 0x050141D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_29_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_30_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_30_FNV_DPA_MAPPED_1_REG 0x050141E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_30_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_31_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400011F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_31_FNV_DPA_MAPPED_1_REG 0x050141F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_31_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_32_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001208)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_32_FNV_DPA_MAPPED_1_REG 0x05014208
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_32_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_33_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001218)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_33_FNV_DPA_MAPPED_1_REG 0x05014218
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_33_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_34_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001228)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_34_FNV_DPA_MAPPED_1_REG 0x05014228
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_34_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_35_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001238)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_35_FNV_DPA_MAPPED_1_REG 0x05014238
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_35_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_36_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001248)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_36_FNV_DPA_MAPPED_1_REG 0x05014248
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_36_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_37_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001258)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_37_FNV_DPA_MAPPED_1_REG 0x05014258
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_37_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_38_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001268)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_38_FNV_DPA_MAPPED_1_REG 0x05014268
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_38_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_39_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001278)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_39_FNV_DPA_MAPPED_1_REG 0x05014278
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_39_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_40_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001288)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_40_FNV_DPA_MAPPED_1_REG 0x05014288
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_40_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_41_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001298)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_41_FNV_DPA_MAPPED_1_REG 0x05014298
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_41_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_42_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_42_FNV_DPA_MAPPED_1_REG 0x050142A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_42_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_43_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_43_FNV_DPA_MAPPED_1_REG 0x050142B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_43_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_44_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_44_FNV_DPA_MAPPED_1_REG 0x050142C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_44_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_45_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_45_FNV_DPA_MAPPED_1_REG 0x050142D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_45_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_46_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_46_FNV_DPA_MAPPED_1_REG 0x050142E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_46_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_47_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400012F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_47_FNV_DPA_MAPPED_1_REG 0x050142F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_47_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_48_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001308)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_48_FNV_DPA_MAPPED_1_REG 0x05014308
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_48_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_49_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001318)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_49_FNV_DPA_MAPPED_1_REG 0x05014318
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_49_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_50_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001328)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_50_FNV_DPA_MAPPED_1_REG 0x05014328
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_50_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_51_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001338)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_51_FNV_DPA_MAPPED_1_REG 0x05014338
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_51_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_52_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001348)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_52_FNV_DPA_MAPPED_1_REG 0x05014348
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_52_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_53_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001358)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_53_FNV_DPA_MAPPED_1_REG 0x05014358
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_53_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_54_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001368)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_54_FNV_DPA_MAPPED_1_REG 0x05014368
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_54_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_55_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001378)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_55_FNV_DPA_MAPPED_1_REG 0x05014378
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_55_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_56_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001388)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_56_FNV_DPA_MAPPED_1_REG 0x05014388
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_56_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_57_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001398)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_57_FNV_DPA_MAPPED_1_REG 0x05014398
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_57_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_58_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_58_FNV_DPA_MAPPED_1_REG 0x050143A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_58_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_59_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_59_FNV_DPA_MAPPED_1_REG 0x050143B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_59_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_60_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_60_FNV_DPA_MAPPED_1_REG 0x050143C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_60_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_61_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_61_FNV_DPA_MAPPED_1_REG 0x050143D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_61_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_62_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_62_FNV_DPA_MAPPED_1_REG 0x050143E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_62_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_63_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400013F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_63_FNV_DPA_MAPPED_1_REG 0x050143F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_63_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_64_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001408)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_64_FNV_DPA_MAPPED_1_REG 0x05014408
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_64_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_65_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001418)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_65_FNV_DPA_MAPPED_1_REG 0x05014418
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_65_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_66_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001428)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_66_FNV_DPA_MAPPED_1_REG 0x05014428
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_66_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_67_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001438)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_67_FNV_DPA_MAPPED_1_REG 0x05014438
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_67_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_68_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001448)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_68_FNV_DPA_MAPPED_1_REG 0x05014448
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_68_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_69_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001458)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_69_FNV_DPA_MAPPED_1_REG 0x05014458
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_69_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_70_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001468)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_70_FNV_DPA_MAPPED_1_REG 0x05014468
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_70_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_71_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001478)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_71_FNV_DPA_MAPPED_1_REG 0x05014478
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_71_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_72_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001488)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_72_FNV_DPA_MAPPED_1_REG 0x05014488
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_72_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_73_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001498)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_73_FNV_DPA_MAPPED_1_REG 0x05014498
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_73_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_74_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_74_FNV_DPA_MAPPED_1_REG 0x050144A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_74_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_75_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_75_FNV_DPA_MAPPED_1_REG 0x050144B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_75_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_76_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_76_FNV_DPA_MAPPED_1_REG 0x050144C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_76_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_77_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_77_FNV_DPA_MAPPED_1_REG 0x050144D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_77_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_78_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_78_FNV_DPA_MAPPED_1_REG 0x050144E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_78_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_79_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400014F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_79_FNV_DPA_MAPPED_1_REG 0x050144F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_79_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_80_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001508)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_80_FNV_DPA_MAPPED_1_REG 0x05014508
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_80_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_81_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001518)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_81_FNV_DPA_MAPPED_1_REG 0x05014518
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_81_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_82_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001528)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_82_FNV_DPA_MAPPED_1_REG 0x05014528
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_82_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_83_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001538)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_83_FNV_DPA_MAPPED_1_REG 0x05014538
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_83_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_84_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001548)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_84_FNV_DPA_MAPPED_1_REG 0x05014548
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_84_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_85_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001558)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_85_FNV_DPA_MAPPED_1_REG 0x05014558
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_85_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_86_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001568)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_86_FNV_DPA_MAPPED_1_REG 0x05014568
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_86_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_87_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001578)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_87_FNV_DPA_MAPPED_1_REG 0x05014578
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_87_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_88_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001588)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_88_FNV_DPA_MAPPED_1_REG 0x05014588
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_88_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_89_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001598)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_89_FNV_DPA_MAPPED_1_REG 0x05014598
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_89_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_90_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_90_FNV_DPA_MAPPED_1_REG 0x050145A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_90_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_91_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_91_FNV_DPA_MAPPED_1_REG 0x050145B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_91_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_92_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_92_FNV_DPA_MAPPED_1_REG 0x050145C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_92_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_93_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_93_FNV_DPA_MAPPED_1_REG 0x050145D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_93_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_94_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_94_FNV_DPA_MAPPED_1_REG 0x050145E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_94_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_95_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x400015F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_95_FNV_DPA_MAPPED_1_REG 0x050145F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_95_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_96_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001608)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_96_FNV_DPA_MAPPED_1_REG 0x05014608
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_96_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_97_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001618)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_97_FNV_DPA_MAPPED_1_REG 0x05014618
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_97_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_98_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001628)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_98_FNV_DPA_MAPPED_1_REG 0x05014628
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_98_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_99_FNV_DPA_MAPPED_1_REG supported on:                         */
/*       EKV (0x40001638)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_99_FNV_DPA_MAPPED_1_REG 0x05014638
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_99_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_100_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001648)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_100_FNV_DPA_MAPPED_1_REG 0x05014648
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_100_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_101_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001658)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_101_FNV_DPA_MAPPED_1_REG 0x05014658
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_101_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_102_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001668)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_102_FNV_DPA_MAPPED_1_REG 0x05014668
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_102_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_103_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001678)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_103_FNV_DPA_MAPPED_1_REG 0x05014678
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_103_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_104_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001688)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_104_FNV_DPA_MAPPED_1_REG 0x05014688
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_104_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_105_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001698)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_105_FNV_DPA_MAPPED_1_REG 0x05014698
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_105_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_106_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_106_FNV_DPA_MAPPED_1_REG 0x050146A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_106_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_107_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_107_FNV_DPA_MAPPED_1_REG 0x050146B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_107_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_108_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_108_FNV_DPA_MAPPED_1_REG 0x050146C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_108_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_109_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_109_FNV_DPA_MAPPED_1_REG 0x050146D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_109_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_110_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_110_FNV_DPA_MAPPED_1_REG 0x050146E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_110_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_111_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400016F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_111_FNV_DPA_MAPPED_1_REG 0x050146F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_111_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_112_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001708)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_112_FNV_DPA_MAPPED_1_REG 0x05014708
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_112_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_113_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001718)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_113_FNV_DPA_MAPPED_1_REG 0x05014718
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_113_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_114_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001728)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_114_FNV_DPA_MAPPED_1_REG 0x05014728
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_114_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_115_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001738)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_115_FNV_DPA_MAPPED_1_REG 0x05014738
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_115_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_116_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001748)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_116_FNV_DPA_MAPPED_1_REG 0x05014748
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_116_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_117_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001758)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_117_FNV_DPA_MAPPED_1_REG 0x05014758
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_117_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_118_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001768)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_118_FNV_DPA_MAPPED_1_REG 0x05014768
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_118_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_119_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001778)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_119_FNV_DPA_MAPPED_1_REG 0x05014778
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_119_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_120_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001788)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_120_FNV_DPA_MAPPED_1_REG 0x05014788
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_120_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_121_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001798)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_121_FNV_DPA_MAPPED_1_REG 0x05014798
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_121_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_122_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_122_FNV_DPA_MAPPED_1_REG 0x050147A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_122_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_123_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_123_FNV_DPA_MAPPED_1_REG 0x050147B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_123_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_124_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_124_FNV_DPA_MAPPED_1_REG 0x050147C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_124_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_125_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_125_FNV_DPA_MAPPED_1_REG 0x050147D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_125_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_126_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_126_FNV_DPA_MAPPED_1_REG 0x050147E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_126_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_127_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400017F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_127_FNV_DPA_MAPPED_1_REG 0x050147F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_127_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_128_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001808)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_128_FNV_DPA_MAPPED_1_REG 0x05014808
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_128_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_129_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001818)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_129_FNV_DPA_MAPPED_1_REG 0x05014818
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_129_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_130_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001828)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_130_FNV_DPA_MAPPED_1_REG 0x05014828
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_130_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_131_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001838)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_131_FNV_DPA_MAPPED_1_REG 0x05014838
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_131_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_132_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001848)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_132_FNV_DPA_MAPPED_1_REG 0x05014848
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_132_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_133_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001858)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_133_FNV_DPA_MAPPED_1_REG 0x05014858
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_133_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_134_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001868)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_134_FNV_DPA_MAPPED_1_REG 0x05014868
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_134_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_135_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001878)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_135_FNV_DPA_MAPPED_1_REG 0x05014878
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_135_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_136_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001888)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_136_FNV_DPA_MAPPED_1_REG 0x05014888
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_136_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_137_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001898)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_137_FNV_DPA_MAPPED_1_REG 0x05014898
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_137_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_138_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_138_FNV_DPA_MAPPED_1_REG 0x050148A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_138_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_139_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_139_FNV_DPA_MAPPED_1_REG 0x050148B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_139_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_140_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_140_FNV_DPA_MAPPED_1_REG 0x050148C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_140_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_141_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_141_FNV_DPA_MAPPED_1_REG 0x050148D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_141_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_142_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_142_FNV_DPA_MAPPED_1_REG 0x050148E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_142_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_143_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400018F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_143_FNV_DPA_MAPPED_1_REG 0x050148F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_143_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_144_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001908)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_144_FNV_DPA_MAPPED_1_REG 0x05014908
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_144_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_145_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001918)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_145_FNV_DPA_MAPPED_1_REG 0x05014918
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_145_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_146_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001928)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_146_FNV_DPA_MAPPED_1_REG 0x05014928
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_146_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_147_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001938)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_147_FNV_DPA_MAPPED_1_REG 0x05014938
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_147_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_148_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001948)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_148_FNV_DPA_MAPPED_1_REG 0x05014948
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_148_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_149_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001958)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_149_FNV_DPA_MAPPED_1_REG 0x05014958
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_149_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_150_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001968)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_150_FNV_DPA_MAPPED_1_REG 0x05014968
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_150_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_151_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001978)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_151_FNV_DPA_MAPPED_1_REG 0x05014978
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_151_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_152_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001988)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_152_FNV_DPA_MAPPED_1_REG 0x05014988
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_152_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_153_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001998)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_153_FNV_DPA_MAPPED_1_REG 0x05014998
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_153_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_154_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019A8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_154_FNV_DPA_MAPPED_1_REG 0x050149A8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_154_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_155_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019B8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_155_FNV_DPA_MAPPED_1_REG 0x050149B8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_155_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_156_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019C8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_156_FNV_DPA_MAPPED_1_REG 0x050149C8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_156_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_157_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019D8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_157_FNV_DPA_MAPPED_1_REG 0x050149D8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_157_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_158_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019E8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_158_FNV_DPA_MAPPED_1_REG 0x050149E8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_158_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_159_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x400019F8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_159_FNV_DPA_MAPPED_1_REG 0x050149F8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_159_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_160_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A08)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_160_FNV_DPA_MAPPED_1_REG 0x05014A08
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_160_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_161_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A18)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_161_FNV_DPA_MAPPED_1_REG 0x05014A18
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_161_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_162_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A28)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_162_FNV_DPA_MAPPED_1_REG 0x05014A28
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_162_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_163_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A38)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_163_FNV_DPA_MAPPED_1_REG 0x05014A38
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_163_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_164_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A48)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_164_FNV_DPA_MAPPED_1_REG 0x05014A48
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_164_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_165_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A58)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_165_FNV_DPA_MAPPED_1_REG 0x05014A58
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_165_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_166_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A68)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_166_FNV_DPA_MAPPED_1_REG 0x05014A68
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_166_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_167_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A78)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_167_FNV_DPA_MAPPED_1_REG 0x05014A78
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_167_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_168_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A88)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_168_FNV_DPA_MAPPED_1_REG 0x05014A88
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_168_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_169_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001A98)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_169_FNV_DPA_MAPPED_1_REG 0x05014A98
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_169_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_170_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AA8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_170_FNV_DPA_MAPPED_1_REG 0x05014AA8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_170_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_171_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AB8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_171_FNV_DPA_MAPPED_1_REG 0x05014AB8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_171_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_172_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AC8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_172_FNV_DPA_MAPPED_1_REG 0x05014AC8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_172_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_173_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AD8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_173_FNV_DPA_MAPPED_1_REG 0x05014AD8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_173_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_174_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AE8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_174_FNV_DPA_MAPPED_1_REG 0x05014AE8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_174_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_175_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001AF8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_175_FNV_DPA_MAPPED_1_REG 0x05014AF8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_175_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_176_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B08)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_176_FNV_DPA_MAPPED_1_REG 0x05014B08
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_176_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_177_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B18)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_177_FNV_DPA_MAPPED_1_REG 0x05014B18
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_177_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_178_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B28)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_178_FNV_DPA_MAPPED_1_REG 0x05014B28
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_178_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_179_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B38)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_179_FNV_DPA_MAPPED_1_REG 0x05014B38
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_179_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_180_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B48)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_180_FNV_DPA_MAPPED_1_REG 0x05014B48
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_180_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_181_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B58)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_181_FNV_DPA_MAPPED_1_REG 0x05014B58
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_181_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_182_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B68)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_182_FNV_DPA_MAPPED_1_REG 0x05014B68
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_182_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_183_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B78)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_183_FNV_DPA_MAPPED_1_REG 0x05014B78
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_183_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_184_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B88)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_184_FNV_DPA_MAPPED_1_REG 0x05014B88
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_184_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_185_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001B98)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_185_FNV_DPA_MAPPED_1_REG 0x05014B98
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_185_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_186_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BA8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_186_FNV_DPA_MAPPED_1_REG 0x05014BA8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_186_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_187_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BB8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_187_FNV_DPA_MAPPED_1_REG 0x05014BB8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_187_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_188_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BC8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_188_FNV_DPA_MAPPED_1_REG 0x05014BC8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_188_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_189_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BD8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_189_FNV_DPA_MAPPED_1_REG 0x05014BD8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_189_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_190_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BE8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_190_FNV_DPA_MAPPED_1_REG 0x05014BE8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_190_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_191_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001BF8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_191_FNV_DPA_MAPPED_1_REG 0x05014BF8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_191_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_192_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C08)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_192_FNV_DPA_MAPPED_1_REG 0x05014C08
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_192_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_193_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C18)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_193_FNV_DPA_MAPPED_1_REG 0x05014C18
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_193_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_194_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C28)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_194_FNV_DPA_MAPPED_1_REG 0x05014C28
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_194_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_195_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C38)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_195_FNV_DPA_MAPPED_1_REG 0x05014C38
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_195_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_196_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C48)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_196_FNV_DPA_MAPPED_1_REG 0x05014C48
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_196_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_197_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C58)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_197_FNV_DPA_MAPPED_1_REG 0x05014C58
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_197_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_198_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C68)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_198_FNV_DPA_MAPPED_1_REG 0x05014C68
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_198_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_199_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C78)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_199_FNV_DPA_MAPPED_1_REG 0x05014C78
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_199_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_200_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C88)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_200_FNV_DPA_MAPPED_1_REG 0x05014C88
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_200_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_201_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001C98)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_201_FNV_DPA_MAPPED_1_REG 0x05014C98
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_201_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_202_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CA8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_202_FNV_DPA_MAPPED_1_REG 0x05014CA8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_202_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_203_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CB8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_203_FNV_DPA_MAPPED_1_REG 0x05014CB8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_203_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_204_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CC8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_204_FNV_DPA_MAPPED_1_REG 0x05014CC8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_204_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_205_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CD8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_205_FNV_DPA_MAPPED_1_REG 0x05014CD8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_205_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_206_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CE8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_206_FNV_DPA_MAPPED_1_REG 0x05014CE8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_206_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_207_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001CF8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_207_FNV_DPA_MAPPED_1_REG 0x05014CF8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_207_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_208_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D08)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_208_FNV_DPA_MAPPED_1_REG 0x05014D08
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_208_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_209_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D18)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_209_FNV_DPA_MAPPED_1_REG 0x05014D18
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_209_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_210_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D28)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_210_FNV_DPA_MAPPED_1_REG 0x05014D28
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_210_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_211_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D38)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_211_FNV_DPA_MAPPED_1_REG 0x05014D38
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_211_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_212_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D48)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_212_FNV_DPA_MAPPED_1_REG 0x05014D48
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_212_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_213_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D58)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_213_FNV_DPA_MAPPED_1_REG 0x05014D58
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_213_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_214_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D68)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_214_FNV_DPA_MAPPED_1_REG 0x05014D68
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_214_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_215_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D78)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_215_FNV_DPA_MAPPED_1_REG 0x05014D78
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_215_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_216_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D88)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_216_FNV_DPA_MAPPED_1_REG 0x05014D88
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_216_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_217_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001D98)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_217_FNV_DPA_MAPPED_1_REG 0x05014D98
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_217_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_218_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DA8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_218_FNV_DPA_MAPPED_1_REG 0x05014DA8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_218_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_219_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DB8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_219_FNV_DPA_MAPPED_1_REG 0x05014DB8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_219_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_220_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DC8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_220_FNV_DPA_MAPPED_1_REG 0x05014DC8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_220_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_221_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DD8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_221_FNV_DPA_MAPPED_1_REG 0x05014DD8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_221_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_222_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DE8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_222_FNV_DPA_MAPPED_1_REG 0x05014DE8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_222_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_223_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001DF8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_223_FNV_DPA_MAPPED_1_REG 0x05014DF8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_223_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_224_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E08)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_224_FNV_DPA_MAPPED_1_REG 0x05014E08
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_224_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_225_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E18)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_225_FNV_DPA_MAPPED_1_REG 0x05014E18
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_225_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_226_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E28)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_226_FNV_DPA_MAPPED_1_REG 0x05014E28
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_226_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_227_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E38)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_227_FNV_DPA_MAPPED_1_REG 0x05014E38
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_227_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_228_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E48)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_228_FNV_DPA_MAPPED_1_REG 0x05014E48
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_228_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_229_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E58)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_229_FNV_DPA_MAPPED_1_REG 0x05014E58
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_229_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_230_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E68)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_230_FNV_DPA_MAPPED_1_REG 0x05014E68
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_230_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_231_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E78)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_231_FNV_DPA_MAPPED_1_REG 0x05014E78
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_231_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_232_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E88)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_232_FNV_DPA_MAPPED_1_REG 0x05014E88
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_232_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_233_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001E98)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_233_FNV_DPA_MAPPED_1_REG 0x05014E98
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_233_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_234_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EA8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_234_FNV_DPA_MAPPED_1_REG 0x05014EA8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_234_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_235_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EB8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_235_FNV_DPA_MAPPED_1_REG 0x05014EB8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_235_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_236_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EC8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_236_FNV_DPA_MAPPED_1_REG 0x05014EC8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_236_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_237_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001ED8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_237_FNV_DPA_MAPPED_1_REG 0x05014ED8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_237_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_238_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EE8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_238_FNV_DPA_MAPPED_1_REG 0x05014EE8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_238_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_239_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001EF8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_239_FNV_DPA_MAPPED_1_REG 0x05014EF8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_239_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_240_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F08)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_240_FNV_DPA_MAPPED_1_REG 0x05014F08
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_240_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_241_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F18)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_241_FNV_DPA_MAPPED_1_REG 0x05014F18
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_241_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_242_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F28)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_242_FNV_DPA_MAPPED_1_REG 0x05014F28
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_242_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_243_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F38)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_243_FNV_DPA_MAPPED_1_REG 0x05014F38
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_243_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_244_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F48)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_244_FNV_DPA_MAPPED_1_REG 0x05014F48
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_244_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_245_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F58)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_245_FNV_DPA_MAPPED_1_REG 0x05014F58
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_245_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_246_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F68)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_246_FNV_DPA_MAPPED_1_REG 0x05014F68
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_246_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_247_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F78)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_247_FNV_DPA_MAPPED_1_REG 0x05014F78
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_247_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_248_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F88)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_248_FNV_DPA_MAPPED_1_REG 0x05014F88
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_248_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_249_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001F98)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_249_FNV_DPA_MAPPED_1_REG 0x05014F98
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_249_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_250_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FA8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_250_FNV_DPA_MAPPED_1_REG 0x05014FA8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_250_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_251_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FB8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_251_FNV_DPA_MAPPED_1_REG 0x05014FB8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_251_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_252_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FC8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_252_FNV_DPA_MAPPED_1_REG 0x05014FC8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_252_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_253_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FD8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_253_FNV_DPA_MAPPED_1_REG 0x05014FD8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_253_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_254_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FE8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_254_FNV_DPA_MAPPED_1_REG 0x05014FE8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_254_FNV_DPA_MAPPED_1_STRUCT;


/* BM_BW_STATUS_255_FNV_DPA_MAPPED_1_REG supported on:                        */
/*       EKV (0x40001FF8)                                                     */
/* Register default value:              0x00000000                            */
#define BM_BW_STATUS_255_FNV_DPA_MAPPED_1_REG 0x05014FF8
/* Struct format extracted from XML file EKV\0.0.1.EKV.xml.
 * This register contains the Status of the current Block Access.
 */
typedef union {
  struct {
    UINT32 dpa_err : 1;
    /* dpa_err - Bits[0:0], RW1CV, default = 1'h0 
       1 = Invalid DPA Error or AIT Poison detected on read. Write command doesn't 
       update this status. 
     */
    UINT32 ngn_err : 1;
    /* ngn_err - Bits[1:1], RW1CV, default = 1'h0 
       1 = NGN uncorrectable read error. RMW for write does not affect this bit
     */
    UINT32 wr_err : 1;
    /* wr_err - Bits[2:2], RW1CV, default = 1'h0 
       1 = Write command sent when Block Window is set up for Read determined by 
       bm_bw_addr_h.rw_lock bit. The write was still allowed to complete and the write 
       data was accepted. 
     */
    UINT32 rd_err : 1;
    /* rd_err - Bits[3:3], ROV, default = 1'h0 
       RSVD 1 = Parity check on Block Window DPA failed.
     */
    UINT32 pm_err : 1;
    /* pm_err - Bits[4:4], RW1CV, default = 1'h0 
       1 = PM range of DPA space was locked.
     */
    UINT32 bw_disable_err : 1;
    /* bw_disable_err - Bits[5:5], RW1CV, default = 1'h0 
       1 = BW Access attempted when Block Window Read/Write Unlock is disabled.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} BM_BW_STATUS_255_FNV_DPA_MAPPED_1_STRUCT;


#endif /* FNV_DPA_MAPPED_1_h */
