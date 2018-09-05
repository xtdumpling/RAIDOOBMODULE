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
// *      Copyright (c) 2007-2017, Intel Corporation.                       *
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
/* Date Stamp: 3/23/2017 */

#ifndef FPGA_CCI_h
#define FPGA_CCI_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FPGA_CCI_DEV 16                                                            */
/* FPGA_CCI_FUN 0                                                             */

/* PCIE_00_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x40080000)                                                    */
/* Register default value:              0x09CB8086                            */
#define PCIE_00_FPGA_CCI_REG 0x14004000
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * PCIE Device ID and Vendor ID
 */
typedef union {
  struct {
    UINT32 vendorid : 16;
    /* vendorid - Bits[15:0], RO, default = 16'h8086 
       Vendor ID
     */
    UINT32 deviceid : 16;
    /* deviceid - Bits[31:16], RO, default = 16'h09CB 
       Device ID
     */
  } Bits;
  UINT32 Data;
} PCIE_00_FPGA_CCI_STRUCT;


/* PCIE_04_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x40080004)                                                    */
/* Register default value:              0x00100000                            */
#define PCIE_04_FPGA_CCI_REG 0x14004004
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * PCIE Status and Command
 */
typedef union {
  struct {
    UINT32 command : 16;
    /* command - Bits[15:0], RO, default = 16'h0 
       Command
     */
    UINT32 status : 16;
    /* status - Bits[31:16], RO, default = 16'h10 
       Status
     */
  } Bits;
  UINT32 Data;
} PCIE_04_FPGA_CCI_STRUCT;


/* PCIE_08_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x40080008)                                                    */
/* Register default value:              0x08800000                            */
#define PCIE_08_FPGA_CCI_REG 0x14004008
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * PCIE Class code and Revision ID
 */
typedef union {
  struct {
    UINT32 revisionid : 8;
    /* revisionid - Bits[7:0], RO, default = 8'h0 
       Revision ID
     */
    UINT32 classcode : 24;
    /* classcode - Bits[31:8], RO, default = 24'h088000 
       Class code
     */
  } Bits;
  UINT32 Data;
} PCIE_08_FPGA_CCI_STRUCT;


/* PCIE_0C_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x4008000C)                                                    */
/* Register default value:              0x00000010                            */
#define PCIE_0C_FPGA_CCI_REG 0x1400400C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * PCIE Latency Timer and CL size
 */
typedef union {
  struct {
    UINT32 clsize : 8;
    /* clsize - Bits[7:0], RO, default = 8'h10 
       Cache line size
     */
    UINT32 latencytimer : 8;
    /* latencytimer - Bits[15:8], RO, default = 8'h0 
       Latency Timer
     */
    UINT32 headertype : 8;
    /* headertype - Bits[23:16], RO, default = 8'h0 
       Header Type
     */
    UINT32 bist : 8;
    /* bist - Bits[31:24], RO, default = 8'h0 
       BIST
     */
  } Bits;
  UINT32 Data;
} PCIE_0C_FPGA_CCI_STRUCT;


/* PCIE_10_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x40080010)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE_10_FPGA_CCI_REG 0x14004010
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Base Address Register #0
 */
typedef union {
  struct {
    UINT32 bar0 : 32;
    /* bar0 - Bits[31:0], RO, default = 32'h00000000 
       Base Address Register #0
     */
  } Bits;
  UINT32 Data;
} PCIE_10_FPGA_CCI_STRUCT;


/* PCIE_2C_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x4008002C)                                                    */
/* Register default value:              0x00008086                            */
#define PCIE_2C_FPGA_CCI_REG 0x1400402C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Subsystem and Subsystem Vendor ID
 */
typedef union {
  struct {
    UINT32 subsystemvendorid : 16;
    /* subsystemvendorid - Bits[15:0], RO, default = 16'h8086 
       Subsystem Vendor ID
     */
    UINT32 subsystemid : 16;
    /* subsystemid - Bits[31:16], RO, default = 16'h0 
       Subsystem ID
     */
  } Bits;
  UINT32 Data;
} PCIE_2C_FPGA_CCI_STRUCT;


/* PCIE_3C_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x4008003C)                                                    */
/* Register default value:              0x00000100                            */
#define PCIE_3C_FPGA_CCI_REG 0x1400403C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Latency and Interrupt
 */
typedef union {
  struct {
    UINT32 interruptline : 8;
    /* interruptline - Bits[7:0], RO, default = 8'h0 
       Interrupt Line
     */
    UINT32 interruptpin : 8;
    /* interruptpin - Bits[15:8], RO, default = 8'h01 
       Interrupt Pin
     */
    UINT32 mingnt : 8;
    /* mingnt - Bits[23:16], RO, default = 8'h0 
       Minimum Grant
     */
    UINT32 maxlat : 8;
    /* maxlat - Bits[31:24], RO, default = 8'h0 
       Maximum Latency
     */
  } Bits;
  UINT32 Data;
} PCIE_3C_FPGA_CCI_STRUCT;


/* KTI_SCRATCH0_FPGA_CCI_REG supported on:                                    */
/*       SKXP (0x40080210)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_SCRATCH0_FPGA_CCI_REG 0x14004210
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI scratchpad register 0
 */
typedef union {
  struct {
    UINT32 scratchpad : 32;
    /* scratchpad - Bits[31:0], RW, default = 32'h0 
       Scratchpad register 0
     */
  } Bits;
  UINT32 Data;
} KTI_SCRATCH0_FPGA_CCI_STRUCT;


/* KTI_SCRATCH1_FPGA_CCI_REG supported on:                                    */
/*       SKXP (0x40080214)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_SCRATCH1_FPGA_CCI_REG 0x14004214
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI scratchpad register 1
 */
typedef union {
  struct {
    UINT32 scratchpad : 32;
    /* scratchpad - Bits[31:0], RW, default = 32'h0 
       Scratchpad register 1
     */
  } Bits;
  UINT32 Data;
} KTI_SCRATCH1_FPGA_CCI_STRUCT;


/* KTI_STKYSCRATCH0_FPGA_CCI_REG supported on:                                */
/*       SKXP (0x40080218)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STKYSCRATCH0_FPGA_CCI_REG 0x14004218
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI sticky scratchpad register 0
 */
typedef union {
  struct {
    UINT32 stickyscratchpad : 32;
    /* stickyscratchpad - Bits[31:0], RWS, default = 32'h0 
       Sticky scratchpad register 0
     */
  } Bits;
  UINT32 Data;
} KTI_STKYSCRATCH0_FPGA_CCI_STRUCT;


/* KTI_STKYSCRATCH1_FPGA_CCI_REG supported on:                                */
/*       SKXP (0x4008021C)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STKYSCRATCH1_FPGA_CCI_REG 0x1400421C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI sticky scratchpad register 1
 */
typedef union {
  struct {
    UINT32 stickyscratchpad : 32;
    /* stickyscratchpad - Bits[31:0], RWS, default = 32'h0 
       Sticky scratchpad register 1
     */
  } Bits;
  UINT32 Data;
} KTI_STKYSCRATCH1_FPGA_CCI_STRUCT;


/* PEI_LOCK_FPGA_CCI_REG supported on:                                        */
/*       SKXP (0x4008022C)                                                    */
/* Register default value:              0x00000000                            */
#define PEI_LOCK_FPGA_CCI_REG 0x1400422C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * PEI Lock
 */
typedef union {
  struct {
    UINT32 peilock : 1;
    /* peilock - Bits[0:0], RW-L, default = 1'h0 
       Controls write access to PEI region locked registers (1 - Lock, 0 - Unlock). 
       This register is Red region locked, so SMBUS PrivReg0[0] needs to be set in 
       order to enable write access to it.  
     */
    UINT32 reserved1 : 31;
    /* reserved1 - Bits[31:1], RsvdZ, default = 31'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PEI_LOCK_FPGA_CCI_STRUCT;


/* DRAM_RULE0_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080230)                                                    */
/* Register default value:              0x00000002                            */
#define DRAM_RULE0_FPGA_CCI_REG 0x14004230
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Coherency configuration control 0
 */
typedef union {
  struct {
    UINT32 ruleenable : 1;
    /* ruleenable - Bits[0:0], RW-L, default = 1'h0 
       0, disable this rule
       1, enable this rule
     */
    UINT32 reserved1 : 1;
    /* reserved1 - Bits[1:1], RsvdP, default = 1'h1 
       Reserved (Xeon Interleave_Mode, unsupported on FPGA.  FPGA only interleaves on 
       address bits {8,7,6}) 
     */
    UINT32 reserved2 : 2;
    /* reserved2 - Bits[3:2], RsvdP, default = 2'h0 
       Reserved (Xeon Attr, unsupported on FPGA.  FPGA only generates transactions to 
       DRAM, not MMCFG space) 
     */
    UINT32 reserved4 : 3;
    /* reserved4 - Bits[6:4], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 addrlimit : 20;
    /* addrlimit - Bits[26:7], RW-L, default = 20'h0 
       This field corresponds to ADDR[45:26] of DRAM top address limit (Lower limit is 
       0) 
     */
    UINT32 reserved27 : 5;
    /* reserved27 - Bits[31:27], RsvdZ, default = 5'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE0_FPGA_CCI_STRUCT;


/* INTERLEAVE_LIST0_FPGA_CCI_REG supported on:                                */
/*       SKXP (0x40080234)                                                    */
/* Register default value:              0x00000000                            */
#define INTERLEAVE_LIST0_FPGA_CCI_REG 0x14004234
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * DRAM Interleave list 0
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST0_FPGA_CCI_STRUCT;


/* DRAM_RULE1_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080238)                                                    */
/* Register default value:              0x00000002                            */
#define DRAM_RULE1_FPGA_CCI_REG 0x14004238
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Coherency configuration control 1
 */
typedef union {
  struct {
    UINT32 ruleenable : 1;
    /* ruleenable - Bits[0:0], RW-L, default = 1'h0 
       0, disable this rule
       1, enable this rule
     */
    UINT32 reserved1 : 1;
    /* reserved1 - Bits[1:1], RsvdP, default = 1'h1 
       Reserved (Xeon Interleave_Mode, unsupported on FPGA.  FPGA only interleaves on 
       address bits {8,7,6}) 
     */
    UINT32 reserved2 : 2;
    /* reserved2 - Bits[3:2], RsvdP, default = 2'h0 
       Reserved (Xeon Attr, unsupported on FPGA.  FPGA only generates transactions to 
       DRAM, not MMCFG space) 
     */
    UINT32 reserved4 : 3;
    /* reserved4 - Bits[6:4], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 addrlimit : 20;
    /* addrlimit - Bits[26:7], RW-L, default = 20'h0 
       This field corresponds to ADDR[45:26] of DRAM top address limit. Must be 
       strictly greater than previous rule even if this rule is disabled. 
     */
    UINT32 reserved27 : 5;
    /* reserved27 - Bits[31:27], RsvdZ, default = 5'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} DRAM_RULE1_FPGA_CCI_STRUCT;


/* INTERLEAVE_LIST1_FPGA_CCI_REG supported on:                                */
/*       SKXP (0x4008023C)                                                    */
/* Register default value:              0x00000000                            */
#define INTERLEAVE_LIST1_FPGA_CCI_REG 0x1400423C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * DRAM Interleave list 1
 */
typedef union {
  struct {
    UINT32 package0 : 4;
    /* package0 - Bits[3:0], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package1 : 4;
    /* package1 - Bits[7:4], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package2 : 4;
    /* package2 - Bits[11:8], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package3 : 4;
    /* package3 - Bits[15:12], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package4 : 4;
    /* package4 - Bits[19:16], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package5 : 4;
    /* package5 - Bits[23:20], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package6 : 4;
    /* package6 - Bits[27:24], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
    UINT32 package7 : 4;
    /* package7 - Bits[31:28], RW-L, default = 4'h0 
       Node ID of interleave list target
     */
  } Bits;
  UINT32 Data;
} INTERLEAVE_LIST1_FPGA_CCI_STRUCT;


/* FPGA_BUSNO_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080240)                                                    */
/* Register default value:              0x00000000                            */
#define FPGA_BUSNO_FPGA_CCI_REG 0x14004240
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Bus number configuration
 */
typedef union {
  struct {
    UINT32 busnumb0 : 8;
    /* busnumb0 - Bits[7:0], RW-L, default = 8'h0 
       FPGA base bus decode (FPGA only decodes one segment - 32 busses)
     */
    UINT32 reserved8 : 23;
    /* reserved8 - Bits[30:8], RsvdZ, default = 23'h0 
       Reserved
     */
    UINT32 busnumbprog : 1;
    /* busnumbprog - Bits[31:31], RW-L, default = 1'h0 
       Set by BIOS when BusNumb0 is valid
     */
  } Bits;
  UINT32 Data;
} FPGA_BUSNO_FPGA_CCI_STRUCT;


/* LLPR_SMRR_BASE_FPGA_CCI_REG supported on:                                  */
/*       SKXP (0x40080250)                                                    */
/* Register default value:              0x00000000                            */
#define LLPR_SMRR_BASE_FPGA_CCI_REG 0x14004250
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Base address for SMRR (TSEG) protection
 */
typedef union {
  struct {
    UINT32 reserved0 : 12;
    /* reserved0 - Bits[11:0], RsvdZ, default = 12'h0 
       Reserved
     */
    UINT32 baseaddr : 20;
    /* baseaddr - Bits[31:12], RW-L, default = 20'h0 
       SMRR memory range base address
       (Follows subset register definition to IA32_SMRR_PHYSBASE)
     */
  } Bits;
  UINT32 Data;
} LLPR_SMRR_BASE_FPGA_CCI_STRUCT;


/* LLPR_SMRR_MASK_FPGA_CCI_REG supported on:                                  */
/*       SKXP (0x40080254)                                                    */
/* Register default value:              0x00000000                            */
#define LLPR_SMRR_MASK_FPGA_CCI_REG 0x14004254
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Mask for SMRR (TSEG) protection
 */
typedef union {
  struct {
    UINT32 reserved0 : 11;
    /* reserved0 - Bits[10:0], RsvdZ, default = 11'h0 
       Reserved
     */
    UINT32 valid : 1;
    /* valid - Bits[11:11], RW-L, default = 1'h0 
       LLPR_SMRR rule is valid when bit is set
     */
    UINT32 physmask : 20;
    /* physmask - Bits[31:12], RW-L, default = 20'h0 
       SMRR memory range mask which determines the range of region being mapped
     */
  } Bits;
  UINT32 Data;
} LLPR_SMRR_MASK_FPGA_CCI_STRUCT;


/* LLPR_SMRR2_BASE_FPGA_CCI_REG supported on:                                 */
/*       SKXP (0x40080258)                                                    */
/* Register default value:              0x00000000                            */
#define LLPR_SMRR2_BASE_FPGA_CCI_REG 0x14004258
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Base address for SMRR2 (TSEG) protection
 */
typedef union {
  struct {
    UINT32 reserved0 : 12;
    /* reserved0 - Bits[11:0], RsvdZ, default = 12'h0 
       Reserved
     */
    UINT32 baseaddr : 20;
    /* baseaddr - Bits[31:12], RW-L, default = 20'h0 
       SMRR2 memory range base address
       (Follows subset register definition to IA32_SMRR2_PHYSBASE)
     */
  } Bits;
  UINT32 Data;
} LLPR_SMRR2_BASE_FPGA_CCI_STRUCT;


/* LLPR_SMRR2_MASK_FPGA_CCI_REG supported on:                                 */
/*       SKXP (0x4008025C)                                                    */
/* Register default value:              0x00000000                            */
#define LLPR_SMRR2_MASK_FPGA_CCI_REG 0x1400425C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Mask for SMRR2 (TSEG) protection
 */
typedef union {
  struct {
    UINT32 reserved0 : 11;
    /* reserved0 - Bits[10:0], RsvdZ, default = 11'h0 
       Reserved
     */
    UINT32 valid : 1;
    /* valid - Bits[11:11], RW-L, default = 1'h0 
       LLPR_SMRR2 rule is valid when bit is set
     */
    UINT32 physmask : 20;
    /* physmask - Bits[31:12], RW-L, default = 20'h0 
       SMRR2 memory range mask which determines the range of region being mapped
     */
  } Bits;
  UINT32 Data;
} LLPR_SMRR2_MASK_FPGA_CCI_STRUCT;


/* LLPR_MESEG_BASE_FPGA_CCI_REG supported on:                                 */
/*       SKXP (0x40080260)                                                    */
/* Register default value:              0x00000000                            */
#define LLPR_MESEG_BASE_FPGA_CCI_REG 0x14004260
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Base address for MESEG protection. Similar definition to MESEG_BASE.
 */
typedef union {
  struct {
    UINT32 mebase : 27;
    /* mebase - Bits[26:0], RW-L, default = 27'h0 
       corresponds to A[45:19] of base address memory range allocated to ME
     */
    UINT32 reserved27 : 5;
    /* reserved27 - Bits[31:27], RsvdZ, default = 5'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} LLPR_MESEG_BASE_FPGA_CCI_STRUCT;


/* LLPR_MESEG_LIMIT_FPGA_CCI_REG supported on:                                */
/*       SKXP (0x40080264)                                                    */
/* Register default value:              0x00000000                            */
#define LLPR_MESEG_LIMIT_FPGA_CCI_REG 0x14004264
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Limit address for MESEG protection. Similar definition to MESEG_LIMIT.
 */
typedef union {
  struct {
    UINT32 melimit : 27;
    /* melimit - Bits[26:0], RW-L, default = 27'h0 
       Corresponds to A[45:19] of limit address memory range allocated to ME.
       Minimum granularity is 1MB
     */
    UINT32 reserved27 : 4;
    /* reserved27 - Bits[30:27], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 enable : 1;
    /* enable - Bits[31:31], RW-L, default = 1'h0 
       Enable LLPR_MESEG rule
     */
  } Bits;
  UINT32 Data;
} LLPR_MESEG_LIMIT_FPGA_CCI_STRUCT;


/* CIPUCTL_FPGA_CCI_REG supported on:                                         */
/*       SKXP (0x40080278)                                                    */
/* Register default value:              0x00000008                            */
#define CIPUCTL_FPGA_CCI_REG 0x14004278
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * CCI PU control register
 */
typedef union {
  struct {
    UINT32 enautoflush : 1;
    /* enautoflush - Bits[0:0], RW, default = 1'h0 
       Enable cache auto flush
     */
    UINT32 flushinterval : 8;
    /* flushinterval - Bits[8:1], RW, default = 8'h4 
       controls the flush interval
     */
    UINT32 reserved9 : 23;
    /* reserved9 - Bits[31:9], RW, default = 23'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} CIPUCTL_FPGA_CCI_STRUCT;


/* CIPUSTAT0_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x4008027C)                                                    */
/* Register default value:              0xC0001702                            */
#define CIPUSTAT0_FPGA_CCI_REG 0x1400427C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * CCI PU-RD status (even channel)
 */
typedef union {
  struct {
    UINT32 reserved0 : 1;
    /* reserved0 - Bits[0:0], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 tginitdone : 1;
    /* tginitdone - Bits[1:1], RO, default = 1'h1 
       Tag initialization done
     */
    UINT32 retryreqt4 : 1;
    /* retryreqt4 - Bits[2:2], RO, default = 1'h0 
       Retry request in T4
     */
    UINT32 rbstallrxwr : 1;
    /* rbstallrxwr - Bits[3:3], RO, default = 1'h0 
       RB requested a WR stall
     */
    UINT32 rbstallrxrd : 1;
    /* rbstallrxrd - Bits[4:4], RO, default = 1'h0 
       RB requested a RD stall
     */
    UINT32 chflushdone : 1;
    /* chflushdone - Bits[5:5], RO, default = 1'h0 
       Cache flush done
     */
    UINT32 reserved6 : 2;
    /* reserved6 - Bits[7:6], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 txreqdone : 1;
    /* txreqdone - Bits[8:8], RO, default = 1'h1 
       TX request done
     */
    UINT32 tagwrdone : 1;
    /* tagwrdone - Bits[9:9], RO, default = 1'h1 
       TAG write done
     */
    UINT32 cachewrdone : 1;
    /* cachewrdone - Bits[10:10], RO, default = 1'h1 
       Cache write done
     */
    UINT32 wrfencepend : 1;
    /* wrfencepend - Bits[11:11], RO, default = 1'h0 
       WrFence pending to be completed
     */
    UINT32 taglookupdone : 1;
    /* taglookupdone - Bits[12:12], RO, default = 1'h1 
       TAG lookup done
     */
    UINT32 stallt4 : 1;
    /* stallt4 - Bits[13:13], RO, default = 1'h0 
       pipeline stall in T4
     */
    UINT32 stallt3 : 1;
    /* stallt3 - Bits[14:14], RO, default = 1'h0 
       pipeline stall in T3
     */
    UINT32 stallt2 : 1;
    /* stallt2 - Bits[15:15], RO, default = 1'h0 
       pipeline stall in T2
     */
    UINT32 stallt1 : 1;
    /* stallt1 - Bits[16:16], RO, default = 1'h0 
       pipeline stall in T1
     */
    UINT32 sreqt4 : 3;
    /* sreqt4 - Bits[19:17], RO, default = 3'h0 
       pipeline sreqT4[2:0]
     */
    UINT32 sreqt3 : 3;
    /* sreqt3 - Bits[22:20], RO, default = 3'h0 
       pipeline sreqT3[2:0]
     */
    UINT32 sreqt2 : 3;
    /* sreqt2 - Bits[25:23], RO, default = 3'h0 
       pipeline sreqT2[2:0]
     */
    UINT32 sreqt1 : 3;
    /* sreqt1 - Bits[28:26], RO, default = 3'h0 
       pipeline sreqT1[2:0]
     */
    UINT32 puhdrvalid : 1;
    /* puhdrvalid - Bits[29:29], RO, default = 1'h0 
       Header valid
     */
    UINT32 allwrreqcomp : 1;
    /* allwrreqcomp - Bits[30:30], RO, default = 1'h1 
       All WR requests completed
     */
    UINT32 allreqcomp : 1;
    /* allreqcomp - Bits[31:31], RO, default = 1'h1 
       All requests in CCI completed
     */
  } Bits;
  UINT32 Data;
} CIPUSTAT0_FPGA_CCI_STRUCT;


/* CIPUSTAT1_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080280)                                                    */
/* Register default value:              0xC0001702                            */
#define CIPUSTAT1_FPGA_CCI_REG 0x14004280
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * CCI PU-WR status (even channel)
 */
typedef union {
  struct {
    UINT32 reserved0 : 1;
    /* reserved0 - Bits[0:0], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 tginitdone : 1;
    /* tginitdone - Bits[1:1], RO, default = 1'h1 
       Tag initialization done
     */
    UINT32 retryreqt4 : 1;
    /* retryreqt4 - Bits[2:2], RO, default = 1'h0 
       Retry request in T4
     */
    UINT32 rbstallrxwr : 1;
    /* rbstallrxwr - Bits[3:3], RO, default = 1'h0 
       RB requested a WR stall
     */
    UINT32 rbstallrxrd : 1;
    /* rbstallrxrd - Bits[4:4], RO, default = 1'h0 
       RB requested a RD stall
     */
    UINT32 chflushdone : 1;
    /* chflushdone - Bits[5:5], RO, default = 1'h0 
       Cache flush done
     */
    UINT32 reserved6 : 2;
    /* reserved6 - Bits[7:6], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 txreqdone : 1;
    /* txreqdone - Bits[8:8], RO, default = 1'h1 
       TX request done
     */
    UINT32 tagwrdone : 1;
    /* tagwrdone - Bits[9:9], RO, default = 1'h1 
       TAG write done
     */
    UINT32 cachewrdone : 1;
    /* cachewrdone - Bits[10:10], RO, default = 1'h1 
       Cache write done
     */
    UINT32 wrfencepend : 1;
    /* wrfencepend - Bits[11:11], RO, default = 1'h0 
       WrFence pending to be completed
     */
    UINT32 taglookupdone : 1;
    /* taglookupdone - Bits[12:12], RO, default = 1'h1 
       TAG lookup done
     */
    UINT32 stallt4 : 1;
    /* stallt4 - Bits[13:13], RO, default = 1'h0 
       pipeline stall in T4
     */
    UINT32 stallt3 : 1;
    /* stallt3 - Bits[14:14], RO, default = 1'h0 
       pipeline stall in T3
     */
    UINT32 stallt2 : 1;
    /* stallt2 - Bits[15:15], RO, default = 1'h0 
       pipeline stall in T2
     */
    UINT32 stallt1 : 1;
    /* stallt1 - Bits[16:16], RO, default = 1'h0 
       pipeline stall in T1
     */
    UINT32 sreqt4 : 3;
    /* sreqt4 - Bits[19:17], RO, default = 3'h0 
       pipeline sreqT4[2:0]
     */
    UINT32 sreqt3 : 3;
    /* sreqt3 - Bits[22:20], RO, default = 3'h0 
       pipeline sreqT3[2:0]
     */
    UINT32 sreqt2 : 3;
    /* sreqt2 - Bits[25:23], RO, default = 3'h0 
       pipeline sreqT2[2:0]
     */
    UINT32 sreqt1 : 3;
    /* sreqt1 - Bits[28:26], RO, default = 3'h0 
       pipeline sreqT1[2:0]
     */
    UINT32 puhdrvalid : 1;
    /* puhdrvalid - Bits[29:29], RO, default = 1'h0 
       Header valid
     */
    UINT32 allwrreqcomp : 1;
    /* allwrreqcomp - Bits[30:30], RO, default = 1'h1 
       All WR requests completed
     */
    UINT32 allreqcomp : 1;
    /* allreqcomp - Bits[31:31], RO, default = 1'h1 
       All requests in CCI completed
     */
  } Bits;
  UINT32 Data;
} CIPUSTAT1_FPGA_CCI_STRUCT;


/* CIPUSTAT2_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080284)                                                    */
/* Register default value:              0xC0001702                            */
#define CIPUSTAT2_FPGA_CCI_REG 0x14004284
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * CCI PU-RD status (odd channel) (Reserved when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 reserved0 : 1;
    /* reserved0 - Bits[0:0], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 tginitdone : 1;
    /* tginitdone - Bits[1:1], RO, default = 1'h1 
       Tag initialization done
     */
    UINT32 retryreqt4 : 1;
    /* retryreqt4 - Bits[2:2], RO, default = 1'h0 
       Retry request in T4
     */
    UINT32 rbstallrxwr : 1;
    /* rbstallrxwr - Bits[3:3], RO, default = 1'h0 
       RB requested a WR stall
     */
    UINT32 rbstallrxrd : 1;
    /* rbstallrxrd - Bits[4:4], RO, default = 1'h0 
       RB requested a RD stall
     */
    UINT32 chflushdone : 1;
    /* chflushdone - Bits[5:5], RO, default = 1'h0 
       Cache flush done
     */
    UINT32 reserved6 : 2;
    /* reserved6 - Bits[7:6], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 txreqdone : 1;
    /* txreqdone - Bits[8:8], RO, default = 1'h1 
       TX request done
     */
    UINT32 tagwrdone : 1;
    /* tagwrdone - Bits[9:9], RO, default = 1'h1 
       TAG write done
     */
    UINT32 cachewrdone : 1;
    /* cachewrdone - Bits[10:10], RO, default = 1'h1 
       Cache write done
     */
    UINT32 wrfencepend : 1;
    /* wrfencepend - Bits[11:11], RO, default = 1'h0 
       WrFence pending to be completed
     */
    UINT32 taglookupdone : 1;
    /* taglookupdone - Bits[12:12], RO, default = 1'h1 
       TAG lookup done
     */
    UINT32 stallt4 : 1;
    /* stallt4 - Bits[13:13], RO, default = 1'h0 
       pipeline stall in T4
     */
    UINT32 stallt3 : 1;
    /* stallt3 - Bits[14:14], RO, default = 1'h0 
       pipeline stall in T3
     */
    UINT32 stallt2 : 1;
    /* stallt2 - Bits[15:15], RO, default = 1'h0 
       pipeline stall in T2
     */
    UINT32 stallt1 : 1;
    /* stallt1 - Bits[16:16], RO, default = 1'h0 
       pipeline stall in T1
     */
    UINT32 sreqt4 : 3;
    /* sreqt4 - Bits[19:17], RO, default = 3'h0 
       pipeline sreqT4[2:0]
     */
    UINT32 sreqt3 : 3;
    /* sreqt3 - Bits[22:20], RO, default = 3'h0 
       pipeline sreqT3[2:0]
     */
    UINT32 sreqt2 : 3;
    /* sreqt2 - Bits[25:23], RO, default = 3'h0 
       pipeline sreqT2[2:0]
     */
    UINT32 sreqt1 : 3;
    /* sreqt1 - Bits[28:26], RO, default = 3'h0 
       pipeline sreqT1[2:0]
     */
    UINT32 puhdrvalid : 1;
    /* puhdrvalid - Bits[29:29], RO, default = 1'h0 
       Header valid
     */
    UINT32 allwrreqcomp : 1;
    /* allwrreqcomp - Bits[30:30], RO, default = 1'h1 
       All WR requests completed
     */
    UINT32 allreqcomp : 1;
    /* allreqcomp - Bits[31:31], RO, default = 1'h1 
       All requests in CCI completed
     */
  } Bits;
  UINT32 Data;
} CIPUSTAT2_FPGA_CCI_STRUCT;


/* CIPUSTAT3_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080288)                                                    */
/* Register default value:              0xC0001702                            */
#define CIPUSTAT3_FPGA_CCI_REG 0x14004288
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * CCI PU-WR status (odd channel) (Reserved when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 reserved0 : 1;
    /* reserved0 - Bits[0:0], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 tginitdone : 1;
    /* tginitdone - Bits[1:1], RO, default = 1'h1 
       Tag initialization done
     */
    UINT32 retryreqt4 : 1;
    /* retryreqt4 - Bits[2:2], RO, default = 1'h0 
       Retry request in T4
     */
    UINT32 rbstallrxwr : 1;
    /* rbstallrxwr - Bits[3:3], RO, default = 1'h0 
       RB requested a WR stall
     */
    UINT32 rbstallrxrd : 1;
    /* rbstallrxrd - Bits[4:4], RO, default = 1'h0 
       RB requested a RD stall
     */
    UINT32 chflushdone : 1;
    /* chflushdone - Bits[5:5], RO, default = 1'h0 
       Cache flush done
     */
    UINT32 reserved6 : 2;
    /* reserved6 - Bits[7:6], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 txreqdone : 1;
    /* txreqdone - Bits[8:8], RO, default = 1'h1 
       TX request done
     */
    UINT32 tagwrdone : 1;
    /* tagwrdone - Bits[9:9], RO, default = 1'h1 
       TAG write done
     */
    UINT32 cachewrdone : 1;
    /* cachewrdone - Bits[10:10], RO, default = 1'h1 
       Cache write done
     */
    UINT32 wrfencepend : 1;
    /* wrfencepend - Bits[11:11], RO, default = 1'h0 
       WrFence pending to be completed
     */
    UINT32 taglookupdone : 1;
    /* taglookupdone - Bits[12:12], RO, default = 1'h1 
       TAG lookup done
     */
    UINT32 stallt4 : 1;
    /* stallt4 - Bits[13:13], RO, default = 1'h0 
       pipeline stall in T4
     */
    UINT32 stallt3 : 1;
    /* stallt3 - Bits[14:14], RO, default = 1'h0 
       pipeline stall in T3
     */
    UINT32 stallt2 : 1;
    /* stallt2 - Bits[15:15], RO, default = 1'h0 
       pipeline stall in T2
     */
    UINT32 stallt1 : 1;
    /* stallt1 - Bits[16:16], RO, default = 1'h0 
       pipeline stall in T1
     */
    UINT32 sreqt4 : 3;
    /* sreqt4 - Bits[19:17], RO, default = 3'h0 
       pipeline sreqT4[2:0]
     */
    UINT32 sreqt3 : 3;
    /* sreqt3 - Bits[22:20], RO, default = 3'h0 
       pipeline sreqT3[2:0]
     */
    UINT32 sreqt2 : 3;
    /* sreqt2 - Bits[25:23], RO, default = 3'h0 
       pipeline sreqT2[2:0]
     */
    UINT32 sreqt1 : 3;
    /* sreqt1 - Bits[28:26], RO, default = 3'h0 
       pipeline sreqT1[2:0]
     */
    UINT32 puhdrvalid : 1;
    /* puhdrvalid - Bits[29:29], RO, default = 1'h0 
       Header valid
     */
    UINT32 allwrreqcomp : 1;
    /* allwrreqcomp - Bits[30:30], RO, default = 1'h1 
       All WR requests completed
     */
    UINT32 allreqcomp : 1;
    /* allreqcomp - Bits[31:31], RO, default = 1'h1 
       All requests in CCI completed
     */
  } Bits;
  UINT32 Data;
} CIPUSTAT3_FPGA_CCI_STRUCT;


/* EXTSCRATCH0_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x40080290)                                                    */
/* Register default value:              0x00000000                            */
#define EXTSCRATCH0_FPGA_CCI_REG 0x14004290
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Extended scratchpad register0
 */
typedef union {
  struct {
    UINT32 extscratch : 32;
    /* extscratch - Bits[31:0], RW, default = 32'h0 
       Extended scratchpad register
     */
  } Bits;
  UINT32 Data;
} EXTSCRATCH0_FPGA_CCI_STRUCT;


/* EXTSCRATCH1_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x40080294)                                                    */
/* Register default value:              0x00000000                            */
#define EXTSCRATCH1_FPGA_CCI_REG 0x14004294
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Extended scratchpad register1
 */
typedef union {
  struct {
    UINT32 extscratch : 32;
    /* extscratch - Bits[31:0], RW, default = 32'h0 
       Extended scratchpad register
     */
  } Bits;
  UINT32 Data;
} EXTSCRATCH1_FPGA_CCI_STRUCT;


/* EXTSCRATCH2_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x40080298)                                                    */
/* Register default value:              0x00000000                            */
#define EXTSCRATCH2_FPGA_CCI_REG 0x14004298
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Extended scratchpad register2
 */
typedef union {
  struct {
    UINT32 extscratch : 32;
    /* extscratch - Bits[31:0], RW, default = 32'h0 
       Extended scratchpad register
     */
  } Bits;
  UINT32 Data;
} EXTSCRATCH2_FPGA_CCI_STRUCT;


/* EXTSCRATCH3_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x4008029C)                                                    */
/* Register default value:              0x00000000                            */
#define EXTSCRATCH3_FPGA_CCI_REG 0x1400429C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Extended scratchpad register3
 */
typedef union {
  struct {
    UINT32 extscratch : 32;
    /* extscratch - Bits[31:0], RW, default = 32'h0 
       Extended scratchpad register
     */
  } Bits;
  UINT32 Data;
} EXTSCRATCH3_FPGA_CCI_STRUCT;


/* KTI_LL_LCP_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080300)                                                    */
/* Register default value:              0x000FF000                            */
#define KTI_LL_LCP_FPGA_CCI_REG 0x14004300
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link layer capability
 */
typedef union {
  struct {
    UINT32 ktiportver : 4;
    /* ktiportver - Bits[3:0], RO, default = 4'h0 
       Version of KTI port: KTI 1.0
     */
    UINT32 reserved4 : 4;
    /* reserved4 - Bits[7:4], RO, default = 4'h0 
       Reserved
     */
    UINT32 crcmode : 2;
    /* crcmode - Bits[9:8], RO, default = 2'h0 
       CRC Mode supported: CRC16 only
     */
    UINT32 reserved10 : 2;
    /* reserved10 - Bits[11:10], RO, default = 2'h0 
       Reserved
     */
    UINT32 llretryqwrapval : 8;
    /* llretryqwrapval - Bits[19:12], RO, default = 8'hff 
       Link layer retry queue wrap value
     */
    UINT32 reserved12 : 12;
    /* reserved12 - Bits[31:20], RO, default = 12'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LCP_FPGA_CCI_STRUCT;


/* KTI_LL_LS_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080304)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_LL_LS_FPGA_CCI_REG 0x14004304
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link layer status
 */
typedef union {
  struct {
    UINT32 numvalentiesinretryq : 8;
    /* numvalentiesinretryq - Bits[7:0], RO, default = 8'h0 
       Number of valid entries in the retry queue
     */
    UINT32 rxlinkstate : 2;
    /* rxlinkstate - Bits[9:8], RO, default = 2'h0 
       RX link state:
       0: IDLE
       1: NORMAL
       2: RETRY_START
     */
    UINT32 txlinkstate : 3;
    /* txlinkstate - Bits[12:10], RO, default = 3'h0 
       TX link state:
       0: IDLE
       1: NORMAL
       2: RETRY_START
       3: RETRY_ROLLBACK
       4: ROLLBACK_DONE
       5: REPLAY_START
       6: RETRY_REPLAY
       7: RETRY_DONE
     */
    UINT32 llsentparaminit : 1;
    /* llsentparaminit - Bits[13:13], RO, default = 1'h0 
       Link layer has received flit and sent out the parameter init flit
     */
    UINT32 curcrcmode : 2;
    /* curcrcmode - Bits[15:14], RO, default = 2'h0 
       Current CRC mode: CRC16 only
     */
    UINT32 linkinitdone : 1;
    /* linkinitdone - Bits[16:16], RO, default = 1'h0 
       Link initialization done
     */
    UINT32 reserved17 : 15;
    /* reserved17 - Bits[31:17], RO, default = 15'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LS_FPGA_CCI_STRUCT;


/* KTI_LL_LP0_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080308)                                                    */
/* Register default value:              0x000000F0                            */
#define KTI_LL_LP0_FPGA_CCI_REG 0x14004308
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link received parameter0
 */
typedef union {
  struct {
    UINT32 ktiver : 4;
    /* ktiver - Bits[3:0], RO, default = 4'h0 
       KTI Version
     */
    UINT32 skutype : 4;
    /* skutype - Bits[7:4], RO, default = 4'hf 
       SKU Type
     */
    UINT32 portnum : 5;
    /* portnum - Bits[12:8], RO, default = 5'h0 
       Port number
     */
    UINT32 reserved13 : 1;
    /* reserved13 - Bits[13:13], RO, default = 1'h0 
       Reserved
     */
    UINT32 ltagent : 1;
    /* ltagent - Bits[14:14], RO, default = 1'h0 
       LT Agent
     */
    UINT32 firmwareagent : 1;
    /* firmwareagent - Bits[15:15], RO, default = 1'h0 
       Firmware Agent
     */
    UINT32 basenodeid : 4;
    /* basenodeid - Bits[19:16], RO, default = 4'h0 
       Base Node ID
     */
    UINT32 reserved20 : 4;
    /* reserved20 - Bits[23:20], RO, default = 4'h0 
       Reserved
     */
    UINT32 rxllrwrapval : 8;
    /* rxllrwrapval - Bits[31:24], RO, default = 8'h0 
       RX LLR Wrap Value
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LP0_FPGA_CCI_STRUCT;


/* KTI_LL_LP1_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x4008030C)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_LL_LP1_FPGA_CCI_REG 0x1400430C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link received parameter1
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 32'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LP1_FPGA_CCI_STRUCT;


/* KTI_LL_TX_CRD_FPGA_CCI_REG supported on:                                   */
/*       SKXP (0x40080310)                                                    */
/* Register default value:              0x39800000                            */
#define KTI_LL_TX_CRD_FPGA_CCI_REG 0x14004310
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link layer TX credit status register
 */
typedef union {
  struct {
    UINT32 txvnacrdavail : 8;
    /* txvnacrdavail - Bits[7:0], RO, default = 8'h0 
       TX VNA credits available (each bit correspond to one of 8 VC)
     */
    UINT32 txvn0crdavail : 8;
    /* txvn0crdavail - Bits[15:8], RO, default = 8'h0 
       TX VN0 credits available (each bit correspond to one of 8 VC)
     */
    UINT32 rxvnacrdavail : 8;
    /* rxvnacrdavail - Bits[23:16], RO, default = 8'h80 
       RX VNA credits available (each bit correspond to one of 8 VC)
     */
    UINT32 rxvn0crdavail : 8;
    /* rxvn0crdavail - Bits[31:24], RO, default = 8'h39 
       RX VN0 credits available (each bit correspond to one of 8 VC)
     */
  } Bits;
  UINT32 Data;
} KTI_LL_TX_CRD_FPGA_CCI_STRUCT;


/* KTI_LL_LES_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080314)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_LL_LES_FPGA_CCI_REG 0x14004314
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link layer error status
 */
typedef union {
  struct {
    UINT32 numfailedretry : 4;
    /* numfailedretry - Bits[3:0], RO, default = 4'h0 
       Number of failed retry
     */
    UINT32 reserved4 : 4;
    /* reserved4 - Bits[7:4], RO, default = 4'h0 
       Reserved
     */
    UINT32 numphyinit : 3;
    /* numphyinit - Bits[10:8], RO, default = 3'h0 
       Number of phy init
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[11:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reserved12 : 20;
    /* reserved12 - Bits[31:12], RO, default = 20'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LES_FPGA_CCI_STRUCT;


/* KTI_LL_LCL_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080318)                                                    */
/* Register default value:              0x00000300                            */
#define KTI_LL_LCL_FPGA_CCI_REG 0x14004318
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link Layer Control
 */
typedef union {
  struct {
    UINT32 linkrst : 1;
    /* linkrst - Bits[0:0], RW1S, default = 1'h0 
       Link layer reset
     */
    UINT32 debugen : 1;
    /* debugen - Bits[1:1], RWSL, default = 1'h0 
       Enable sending of debug flit (Not supported)
     */
    UINT32 initstall : 1;
    /* initstall - Bits[2:2], RWSL, default = 1'h0 
       Stall credit return after init
     */
    UINT32 txholdcrd : 1;
    /* txholdcrd - Bits[3:3], RWSL, default = 1'h0 
       Hold credits (Not supported)
     */
    UINT32 retrytimeoutval : 3;
    /* retrytimeoutval - Bits[6:4], RWSL, default = 3'h0 
       Retry Timeout Value:
       000 - 8K flits
       001 - 4K flits
       010 - 2K flits
       011 - 1K flits
       100 - 1/2K flits
       101 - 1/4K flits
       110 - 1/8K flits
       111 - Disabled
     */
    UINT32 reserved7 : 1;
    /* reserved7 - Bits[7:7], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 maxretry : 4;
    /* maxretry - Bits[11:8], RWSL, default = 4'h3 
       Number of failed retries to trigger Phy Reset
     */
    UINT32 maxphyrstbefinitabort : 2;
    /* maxphyrstbefinitabort - Bits[13:12], RWSL, default = 2'h0 
       Max phy reset before initialization aborts
       00 - disable initialization abort (infinite reinit)
       01 - 1
       10 - 2
       11 - 4
     */
    UINT32 reserved14 : 18;
    /* reserved14 - Bits[31:14], RsvdZ, default = 18'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LCL_FPGA_CCI_STRUCT;


/* KTI_LL_LCCL_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x4008031C)                                                    */
/* Register default value:              0x00904880                            */
#define KTI_LL_LCCL_FPGA_CCI_REG 0x1400431C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link Layer Credit Control
 */
typedef union {
  struct {
    UINT32 vnamax : 8;
    /* vnamax - Bits[7:0], RWSL, default = 8'h80 
       VNA credits: up to 128 filits
     */
    UINT32 reserved8 : 3;
    /* reserved8 - Bits[10:8], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 vn0snp : 1;
    /* vn0snp - Bits[11:11], RWSL, default = 1'h1 
       VN0 SNP credit: only 1 is supported
     */
    UINT32 reserved12 : 2;
    /* reserved12 - Bits[13:12], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 vn0rsp : 1;
    /* vn0rsp - Bits[14:14], RWSL, default = 1'h1 
       VN0 RSP credit: only 1 is supported
     */
    UINT32 reserved15 : 5;
    /* reserved15 - Bits[19:15], RsvdZ, default = 5'h0 
       Reserved
     */
    UINT32 vn0ncb : 1;
    /* vn0ncb - Bits[20:20], RWSL, default = 1'h1 
       VN0 NCB credit: only 1 is supported
     */
    UINT32 reserved21 : 2;
    /* reserved21 - Bits[22:21], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 vn0ncs : 1;
    /* vn0ncs - Bits[23:23], RWSL, default = 1'h1 
       VN0 NCS credit: only 1 is supported
     */
    UINT32 reserved24 : 8;
    /* reserved24 - Bits[31:24], RsvdZ, default = 8'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LCCL_FPGA_CCI_STRUCT;


/* KTI_LL_LDCL_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x40080320)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_LL_LDCL_FPGA_CCI_REG 0x14004320
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link Layer DC
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 32'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_LL_LDCL_FPGA_CCI_STRUCT;


/* KTI_TX_LP0_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080324)                                                    */
/* Register default value:              0x000000F0                            */
#define KTI_TX_LP0_FPGA_CCI_REG 0x14004324
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link Transmit Parameter0
 */
typedef union {
  struct {
    UINT32 ktiversion : 4;
    /* ktiversion - Bits[3:0], RW-L, default = 4'h0 
       KTI Version
     */
    UINT32 skutype : 4;
    /* skutype - Bits[7:4], RW-L, default = 4'hf 
       SKU Type
     */
    UINT32 portnumb : 5;
    /* portnumb - Bits[12:8], RW-L, default = 5'h0 
       Port number
     */
    UINT32 reserved13 : 1;
    /* reserved13 - Bits[13:13], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 ltagent : 1;
    /* ltagent - Bits[14:14], RW-L, default = 1'h0 
       LT Agent
     */
    UINT32 firmwareagent : 1;
    /* firmwareagent - Bits[15:15], RW-L, default = 1'h0 
       Firmware Agent
     */
    UINT32 reserved16 : 8;
    /* reserved16 - Bits[23:16], RsvdZ, default = 8'h0 
       Reserved
     */
    UINT32 txeseqmax : 8;
    /* txeseqmax - Bits[31:24], RW-L, default = 8'h0 
       TX LLR Wrap Value
     */
  } Bits;
  UINT32 Data;
} KTI_TX_LP0_FPGA_CCI_STRUCT;


/* KTI_TX_LP1_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080328)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_TX_LP1_FPGA_CCI_REG 0x14004328
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Link Transmit Parameter1
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 32'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_TX_LP1_FPGA_CCI_STRUCT;


/* KTI_TIMER_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x4008032C)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_TIMER_FPGA_CCI_REG 0x1400432C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Private KTI Timer
 */
typedef union {
  struct {
    UINT32 clkcnt : 32;
    /* clkcnt - Bits[31:0], RO, default = 32'h0 
       Counts number of clocks
     */
  } Bits;
  UINT32 Data;
} KTI_TIMER_FPGA_CCI_STRUCT;


/* KTI_STAT0_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080330)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT0_FPGA_CCI_REG 0x14004330
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI TX CRC/Retry count
 */
typedef union {
  struct {
    UINT32 numtxcrc : 32;
    /* numtxcrc - Bits[31:0], RO, default = 32'h0 
       Number of TX CRC (from RX retry request) detected
     */
  } Bits;
  UINT32 Data;
} KTI_STAT0_FPGA_CCI_STRUCT;


/* KTI_STAT1_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080334)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT1_FPGA_CCI_REG 0x14004334
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI RX CRC/Retry count
 */
typedef union {
  struct {
    UINT32 numrxcrc : 32;
    /* numrxcrc - Bits[31:0], RO, default = 32'h0 
       Number of RX CRC (from TX retry request) detected
     */
  } Bits;
  UINT32 Data;
} KTI_STAT1_FPGA_CCI_STRUCT;


/* KTI_STAT10_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080358)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT10_FPGA_CCI_REG 0x14004358
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI error count
 */
typedef union {
  struct {
    UINT32 errcount : 8;
    /* errcount - Bits[7:0], RO, default = 8'h0 
       Number of KTI errors detected
     */
    UINT32 parerrcount : 8;
    /* parerrcount - Bits[15:8], RO, default = 8'h0 
       Number of parity errors detected
     */
    UINT32 pendsnprespcount : 8;
    /* pendsnprespcount - Bits[23:16], RO, default = 8'h0 
       Number of pending snoop responses
     */
    UINT32 pendccireqcount : 8;
    /* pendccireqcount - Bits[31:24], RO, default = 8'h0 
       Number of CCI pending requests
     */
  } Bits;
  UINT32 Data;
} KTI_STAT10_FPGA_CCI_STRUCT;


/* KTI_STAT11_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x4008035C)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT11_FPGA_CCI_REG 0x1400435C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI error indication
 */
typedef union {
  struct {
    UINT32 error : 1;
    /* error - Bits[0:0], RO, default = 1'h0 
       KTI error detected
     */
    UINT32 parerror : 1;
    /* parerror - Bits[1:1], RO, default = 1'h0 
       KTI parity error detected
     */
    UINT32 snppendor : 1;
    /* snppendor - Bits[2:2], RO, default = 1'h0 
       Snoop response is pending
     */
    UINT32 ccipendor : 1;
    /* ccipendor - Bits[3:3], RO, default = 1'h0 
       CCI completion is pending
     */
    UINT32 reqpendor : 1;
    /* reqpendor - Bits[4:4], RO, default = 1'h0 
       UPI request is pending
     */
    UINT32 rxcrcerror : 1;
    /* rxcrcerror - Bits[5:5], RO, default = 1'h0 
       Rx link CRC error detected
     */
    UINT32 txcrcerror : 1;
    /* txcrcerror - Bits[6:6], RO, default = 1'h0 
       Tx link CRC error detected
     */
    UINT32 reseverd7 : 1;
    /* reseverd7 - Bits[7:7], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 rxresetor : 1;
    /* rxresetor - Bits[8:8], RO, default = 1'h0 
       Rx PHY inband reset received
     */
    UINT32 reserved9 : 3;
    /* reserved9 - Bits[11:9], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 txresetor : 1;
    /* txresetor - Bits[12:12], RO, default = 1'h0 
       Tx PHY inband reset sent
     */
    UINT32 reserved13 : 3;
    /* reserved13 - Bits[15:13], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 rxresetcount : 8;
    /* rxresetcount - Bits[23:16], RO, default = 8'h0 
       Rx inband reset count
     */
    UINT32 txresetcount : 8;
    /* txresetcount - Bits[31:24], RO, default = 8'h0 
       Tx inband reset count
     */
  } Bits;
  UINT32 Data;
} KTI_STAT11_FPGA_CCI_STRUCT;


/* KTI_STAT12_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080360)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT12_FPGA_CCI_REG 0x14004360
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI RX cycle count
 */
typedef union {
  struct {
    UINT32 rxcyclesel : 32;
    /* rxcyclesel - Bits[31:0], RO, default = 32'h0 
       Number of RX cycles
     */
  } Bits;
  UINT32 Data;
} KTI_STAT12_FPGA_CCI_STRUCT;


/* KTI_STAT13_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080364)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT13_FPGA_CCI_REG 0x14004364
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI TX cycle count
 */
typedef union {
  struct {
    UINT32 txcyclesel : 32;
    /* txcyclesel - Bits[31:0], RO, default = 32'h0 
       Number of TX cycles
     */
  } Bits;
  UINT32 Data;
} KTI_STAT13_FPGA_CCI_STRUCT;


/* KTI_STAT14_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x40080368)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT14_FPGA_CCI_REG 0x14004368
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI RX Data count
 */
typedef union {
  struct {
    UINT32 rxdatacnt : 32;
    /* rxdatacnt - Bits[31:0], RO, default = 32'h0 
       Number of RX cache line transferred
     */
  } Bits;
  UINT32 Data;
} KTI_STAT14_FPGA_CCI_STRUCT;


/* KTI_STAT15_FPGA_CCI_REG supported on:                                      */
/*       SKXP (0x4008036C)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_STAT15_FPGA_CCI_REG 0x1400436C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI TX Data count
 */
typedef union {
  struct {
    UINT32 txdatacnt : 32;
    /* txdatacnt - Bits[31:0], RO, default = 32'h0 
       Number of TX cache line transferred
     */
  } Bits;
  UINT32 Data;
} KTI_STAT15_FPGA_CCI_STRUCT;


/* KTI_CTRL0_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080370)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_CTRL0_FPGA_CCI_REG 0x14004370
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Private Link Layer Control
 */
typedef union {
  struct {
    UINT32 sampleen : 1;
    /* sampleen - Bits[0:0], RW-L, default = 1'h0 
       Sample enable: start counter at rising transition
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[23:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rxlinkthrottle : 2;
    /* rxlinkthrottle - Bits[25:24], RW-L, default = 2'h0 
       RX link throttle
       00 - no throttle
       01 - throttle to 3/4
       10 - throttle to 1/2
       11 - throttle to 1/4
     */
    UINT32 txlinkthrottle : 2;
    /* txlinkthrottle - Bits[27:26], RW-L, default = 2'h0 
       TX link throttle
       00 - no throttle
       01 - throttle to 3/4
       10 - throttle to 1/2
       11 - throttle to 1/4
     */
    UINT32 rxcrcerren : 2;
    /* rxcrcerren - Bits[29:28], RW-L, default = 2'h0 
       RX CRC error injection
       00 - disable random RX CRC error injection
       01 - inject RX CRC error average every 256 clks
       10 - inject RX CRC error average every 64K clks
       11 - inject RX CRC error average 16 per second
     */
    UINT32 txcrcen : 2;
    /* txcrcen - Bits[31:30], RW-L, default = 2'h0 
       TX CRC error injection
       00 - disable random TX crc error injection
       01 - inject TX crc error average every 256 clks
       10 - inject TX crc error average every 64K clks
       11 - inject TX crc error average 16 per second
     */
  } Bits;
  UINT32 Data;
} KTI_CTRL0_FPGA_CCI_STRUCT;


/* KTI_CTRL6_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x40080388)                                                    */
/* Register default value:              0x01000000                            */
#define KTI_CTRL6_FPGA_CCI_REG 0x14004388
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Node ID
 */
typedef union {
  struct {
    UINT32 cpunodeid0 : 4;
    /* cpunodeid0 - Bits[3:0], RW-L, default = 4'h0 
       CPU Node ID0
     */
    UINT32 cpunodeid1 : 4;
    /* cpunodeid1 - Bits[7:4], RW-L, default = 4'h0 
       CPU Node ID1
     */
    UINT32 cpunodeid2 : 4;
    /* cpunodeid2 - Bits[11:8], RW-L, default = 4'h0 
       CPU Node ID2
     */
    UINT32 cpunodeid3 : 2;
    /* cpunodeid3 - Bits[13:12], RW-L, default = 2'h0 
       CPU Node ID3
     */
    UINT32 reserved14 : 10;
    /* reserved14 - Bits[23:14], RsvdZ, default = 10'h0 
       Reserved
     */
    UINT32 fpganodeid : 4;
    /* fpganodeid - Bits[27:24], RO, default = 4'h1 
       FPGA Node ID
     */
    UINT32 srcaddrintvmode : 4;
    /* srcaddrintvmode - Bits[31:28], RW-L, default = 4'h0 
       Source address interleave mode
     */
  } Bits;
  UINT32 Data;
} KTI_CTRL6_FPGA_CCI_STRUCT;


/* KTI_CTRL7_FPGA_CCI_REG supported on:                                       */
/*       SKXP (0x4008038C)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_CTRL7_FPGA_CCI_REG 0x1400438C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Protocol Control
 */
typedef union {
  struct {
    UINT32 sampleen : 1;
    /* sampleen - Bits[0:0], RW-L, default = 1'h0 
       1: clear cycle count and begin sample
       0: stop sample
     */
    UINT32 rstcnt : 1;
    /* rstcnt - Bits[1:1], RW-L, default = 1'h0 
       Clear protocol timer and cycle count
     */
    UINT32 reserved2 : 14;
    /* reserved2 - Bits[15:2], RsvdP, default = 14'h0 
       Reserved
     */
    UINT32 rxcyclesel : 5;
    /* rxcyclesel - Bits[20:16], RW-L, default = 5'h0 
       Select RX cycle type to count
     */
    UINT32 rsvd : 3;
    /* rsvd - Bits[23:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 txcyclesel : 5;
    /* txcyclesel - Bits[28:24], RW-L, default = 5'h0 
       Select TX cycle type to count
     */
    UINT32 reserved29 : 3;
    /* reserved29 - Bits[31:29], RsvdP, default = 3'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_CTRL7_FPGA_CCI_STRUCT;


/* KTI_ERR_STAT0_FPGA_CCI_REG supported on:                                   */
/*       SKXP (0x40080390)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_ERR_STAT0_FPGA_CCI_REG 0x14004390
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Link Layer Error 0
 */
typedef union {
  struct {
    UINT32 rxcycleerr : 1;
    /* rxcycleerr - Bits[0:0], RW1C, default = 1'h0 
       RX received unsupported cycles
     */
    UINT32 txretryerr : 1;
    /* txretryerr - Bits[1:1], RW1C, default = 1'h0 
       TX retry is out of range
     */
    UINT32 txtimeouterr : 1;
    /* txtimeouterr - Bits[2:2], RW1C, default = 1'h0 
       TX timeout error
     */
    UINT32 rxtimeouterr : 1;
    /* rxtimeouterr - Bits[3:3], RW1C, default = 1'h0 
       RX timeout error
     */
    UINT32 rxcsrerr : 1;
    /* rxcsrerr - Bits[4:4], RW1C, default = 1'h0 
       RX CSR cycle write to msg reserved space
     */
    UINT32 reiniterr : 1;
    /* reiniterr - Bits[5:5], RW1C, default = 1'h0 
       TX reinit failed after multiple phy reset
     */
    UINT32 rxvnaerr : 1;
    /* rxvnaerr - Bits[6:6], RW1C, default = 1'h0 
       RX received VNA flits without VNA credits
     */
    UINT32 txvnaerr : 1;
    /* txvnaerr - Bits[7:7], RW1C, default = 1'h0 
       TX send VNA flits without VNA credits
     */
    UINT32 rxvnacrderr : 1;
    /* rxvnacrderr - Bits[8:8], RW1C, default = 1'h0 
       RX receive VNA credits more than TX_VNA_MAX (CPU VNA credits)
     */
    UINT32 txvnacrderr : 1;
    /* txvnacrderr - Bits[9:9], RW1C, default = 1'h0 
       TX send VNA credits greater than RX_VNA_MAX (FPGA VNA credits)
     */
    UINT32 rxvn0crderr : 1;
    /* rxvn0crderr - Bits[10:10], RW1C, default = 1'h0 
       RX received more than 1 VN0 credit
     */
    UINT32 txvn0crderr : 1;
    /* txvn0crderr - Bits[11:11], RW1C, default = 1'h0 
       TX sent more than 1 VN0 credit
     */
    UINT32 rxvn0err : 1;
    /* rxvn0err - Bits[12:12], RW1C, default = 1'h0 
       RX received VN0 flit without VN0 credit
     */
    UINT32 txvn0err : 1;
    /* txvn0err - Bits[13:13], RW1C, default = 1'h0 
       TX sent VN0 flit without VN0 credit
     */
    UINT32 rxvn1crderr : 1;
    /* rxvn1crderr - Bits[14:14], RW1C, default = 1'h0 
       RX receives VN1 credit
     */
    UINT32 txvn1crderr : 1;
    /* txvn1crderr - Bits[15:15], RW1C, default = 1'h0 
       TX sent VN1 credit
     */
    UINT32 rxvn1err : 1;
    /* rxvn1err - Bits[16:16], RW1C, default = 1'h0 
       RX sent VN1 flit
     */
    UINT32 txvn1err : 1;
    /* txvn1err - Bits[17:17], RW1C, default = 1'h0 
       TX sent VN1 flit
     */
    UINT32 rxackerr : 1;
    /* rxackerr - Bits[18:18], RW1C, default = 1'h0 
       RX Eseq value less than 0
     */
    UINT32 txackerr : 1;
    /* txackerr - Bits[19:19], RW1C, default = 1'h0 
       TX Eseq value less than 0
     */
    UINT32 rxeseqerr : 1;
    /* rxeseqerr - Bits[20:20], RW1C, default = 1'h0 
       RX Eseq value greater than RX_Eseq_Max (CPU retry buffer depth)
     */
    UINT32 txeseqerr : 1;
    /* txeseqerr - Bits[21:21], RW1C, default = 1'h0 
       TX Eseq value greater than TX_Eseq_Max (FPGA retry buffer depth)
     */
    UINT32 rxllctrlerr : 1;
    /* rxllctrlerr - Bits[22:22], RW1C, default = 1'h0 
       RX received unknown LLCTRL flit
     */
    UINT32 rxdebugerr : 1;
    /* rxdebugerr - Bits[23:23], RW1C, default = 1'h0 
       RX received debug flit
     */
    UINT32 rxpoisonerr : 1;
    /* rxpoisonerr - Bits[24:24], RW1C, default = 1'h0 
       RX received poisoned flit
     */
    UINT32 reserved25 : 7;
    /* reserved25 - Bits[31:25], RW1C, default = 7'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_STAT0_FPGA_CCI_STRUCT;


/* KTI_ERR_EN0_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x40080394)                                                    */
/* Register default value:              0x01FFFFFF                            */
#define KTI_ERR_EN0_FPGA_CCI_REG 0x14004394
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Link Layer Error Enable 0
 */
typedef union {
  struct {
    UINT32 llerren0 : 25;
    /* llerren0 - Bits[24:0], RW, default = 25'h1ffffff 
       0 - disable corresponding bit in ERR_STAT0
       1 - enable corresponding error detection
     */
    UINT32 reserved25 : 7;
    /* reserved25 - Bits[31:25], RW, default = 7'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_EN0_FPGA_CCI_STRUCT;


/* KTI_ERR_STAT1_FPGA_CCI_REG supported on:                                   */
/*       SKXP (0x40080398)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_ERR_STAT1_FPGA_CCI_REG 0x14004398
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Link Layer Error 1
 */
typedef union {
  struct {
    UINT32 rxoddrtidfifoparerr : 1;
    /* rxoddrtidfifoparerr - Bits[0:0], RW1C, default = 1'h0 
       RX odd RTID fifo parity error
     */
    UINT32 rxevenrtidfifoparerr : 1;
    /* rxevenrtidfifoparerr - Bits[1:1], RW1C, default = 1'h0 
       RX even RTID fifo parity error
     */
    UINT32 rxfifoparerr : 1;
    /* rxfifoparerr - Bits[2:2], RW1C, default = 1'h0 
       RX fifo parity error
     */
    UINT32 txfifoparerr : 1;
    /* txfifoparerr - Bits[3:3], RW1C, default = 1'h0 
       TX fifo parity error
     */
    UINT32 txoddcycleparerr : 1;
    /* txoddcycleparerr - Bits[4:4], RW1C, default = 1'h0 
       TX odd cycle fifo parity error
     */
    UINT32 txevencycleparerr : 1;
    /* txevencycleparerr - Bits[5:5], RW1C, default = 1'h0 
       TX even cycle fifo parity error
     */
    UINT32 txevendataparerr : 1;
    /* txevendataparerr - Bits[6:6], RW1C, default = 1'h0 
       TX even data fifo parity error
     */
    UINT32 txodddataparerr : 1;
    /* txodddataparerr - Bits[7:7], RW1C, default = 1'h0 
       TX odd data fifo parity error
     */
    UINT32 rxodddataparerr : 1;
    /* rxodddataparerr - Bits[8:8], RW1C, default = 1'h0 
       RX odd data fifo parity error
     */
    UINT32 rxoddcycleparerr : 1;
    /* rxoddcycleparerr - Bits[9:9], RW1C, default = 1'h0 
       RX odd cycle fifo parity error
     */
    UINT32 rxevendataparerr : 1;
    /* rxevendataparerr - Bits[10:10], RW1C, default = 1'h0 
       RX even data fifo parity error
     */
    UINT32 rxevencycleparerr : 1;
    /* rxevencycleparerr - Bits[11:11], RW1C, default = 1'h0 
       RX even cycle fifo parity error
     */
    UINT32 rxoddrtiderr : 1;
    /* rxoddrtiderr - Bits[12:12], RW1C, default = 1'h0 
       RX odd RTID fifo overflow/underflow
     */
    UINT32 rxevenrtiderr : 1;
    /* rxevenrtiderr - Bits[13:13], RW1C, default = 1'h0 
       RX even RTID fifo overflow/underflow
     */
    UINT32 txoddcycleerr : 1;
    /* txoddcycleerr - Bits[14:14], RW1C, default = 1'h0 
       TX odd cycle fifo overflow/underflow
     */
    UINT32 txevencycleerr : 1;
    /* txevencycleerr - Bits[15:15], RW1C, default = 1'h0 
       TX even cycle fifo overflow/underflow
     */
    UINT32 txevendataerr : 1;
    /* txevendataerr - Bits[16:16], RW1C, default = 1'h0 
       TX even data fifo overflow/underflow
     */
    UINT32 txodddataerr : 1;
    /* txodddataerr - Bits[17:17], RW1C, default = 1'h0 
       TX odd data fifo overflow/underflow
     */
    UINT32 rxodddataerr : 1;
    /* rxodddataerr - Bits[18:18], RW1C, default = 1'h0 
       RX odd data fifo overflow/underflow
     */
    UINT32 rxoddcycleerr : 1;
    /* rxoddcycleerr - Bits[19:19], RW1C, default = 1'h0 
       RX odd cycle fifo overflow/underflow
     */
    UINT32 rxevendataerr : 1;
    /* rxevendataerr - Bits[20:20], RW1C, default = 1'h0 
       RX even data fifo overflow/underflow
     */
    UINT32 rxevencycleerr : 1;
    /* rxevencycleerr - Bits[21:21], RW1C, default = 1'h0 
       RX even cycle fifo overflow/underflow
     */
    UINT32 rxfifoerr : 1;
    /* rxfifoerr - Bits[22:22], RW1C, default = 1'h0 
       RX fifo overflow/underflow
     */
    UINT32 reserved23 : 9;
    /* reserved23 - Bits[31:23], RW1C, default = 9'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_STAT1_FPGA_CCI_STRUCT;


/* KTI_ERR_EN1_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x4008039C)                                                    */
/* Register default value:              0x007FFFFF                            */
#define KTI_ERR_EN1_FPGA_CCI_REG 0x1400439C
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Link Layer Error Enable 1
 */
typedef union {
  struct {
    UINT32 llerren1 : 23;
    /* llerren1 - Bits[22:0], RW, default = 23'h7fffff 
       0 - disable corresponding bit in ERR_STAT1
       1 - enable corresponding error detection
     */
    UINT32 reserved17 : 9;
    /* reserved17 - Bits[31:23], RW, default = 9'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_EN1_FPGA_CCI_STRUCT;


/* KTI_ERR_STAT2_FPGA_CCI_REG supported on:                                   */
/*       SKXP (0x400803A0)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_ERR_STAT2_FPGA_CCI_REG 0x140043A0
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Protocol Layer Error (Reserved[31:16] when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 rspqerr0 : 1;
    /* rspqerr0 - Bits[0:0], RW1C, default = 1'h0 
       response queue fifo overflow/underflow error (Ch0)
     */
    UINT32 reqqerr0 : 1;
    /* reqqerr0 - Bits[1:1], RW1C, default = 1'h0 
       request queue fifo overflow/underflow error (Ch0)
     */
    UINT32 reloopqerr0 : 1;
    /* reloopqerr0 - Bits[2:2], RW1C, default = 1'h0 
       reloop queue fifo overflow/underflow error (Ch0)
     */
    UINT32 ccicmperr0 : 1;
    /* ccicmperr0 - Bits[3:3], RW1C, default = 1'h0 
       sending cci cmp while cci_cmp is 0 (Ch0)
     */
    UINT32 validclrerr0 : 1;
    /* validclrerr0 - Bits[4:4], RW1C, default = 1'h0 
       clearing request_valid when it's 0 (Ch0)
     */
    UINT32 validseterr0 : 1;
    /* validseterr0 - Bits[5:5], RW1C, default = 1'h0 
       setting request_valid when it's 1 (Ch0)
     */
    UINT32 rxdniderr0 : 1;
    /* rxdniderr0 - Bits[6:6], RW1C, default = 1'h0 
       received cycle with DNID!=FPGA_NID (Ch0)
     */
    UINT32 memrangeerr0 : 1;
    /* memrangeerr0 - Bits[7:7], RW1C, default = 1'h0 
       received request with address out of memory range (Ch0)
     */
    UINT32 respqparerr0 : 1;
    /* respqparerr0 - Bits[8:8], RW1C, default = 1'h0 
       response queue fifo parity error (Ch0)
     */
    UINT32 reqqparerr0 : 1;
    /* reqqparerr0 - Bits[9:9], RW1C, default = 1'h0 
       request queue fifo parity error (Ch0)
     */
    UINT32 reloopqparerr0 : 1;
    /* reloopqparerr0 - Bits[10:10], RW1C, default = 1'h0 
       reloop queue fifo parity error (Ch0)
     */
    UINT32 cmpqparerr0 : 1;
    /* cmpqparerr0 - Bits[11:11], RW1C, default = 1'h0 
       completion queue parity error (Ch0)
     */
    UINT32 intfredirectparerr0 : 2;
    /* intfredirectparerr0 - Bits[13:12], RW1C, default = 2'h0 
       Interface redirection patity error (Ch0) (Reserved when SINGLE_CHANNEL)
     */
    UINT32 reserved14 : 2;
    /* reserved14 - Bits[15:14], RW1C, default = 2'h0 
       Reserved
     */
    UINT32 rspqerr1 : 1;
    /* rspqerr1 - Bits[16:16], RW1C, default = 1'h0 
       response queue fifo overflow/underflow error (Ch1)
     */
    UINT32 reqqerr1 : 1;
    /* reqqerr1 - Bits[17:17], RW1C, default = 1'h0 
       request queue fifo overflow/underflow error (Ch1)
     */
    UINT32 reloopqerr1 : 1;
    /* reloopqerr1 - Bits[18:18], RW1C, default = 1'h0 
       reloop queue fifo overflow/underflow error (Ch1)
     */
    UINT32 ccicmperr1 : 1;
    /* ccicmperr1 - Bits[19:19], RW1C, default = 1'h0 
       sending cci cmp while cci_cmp is 0 (Ch1)
     */
    UINT32 validclrerr1 : 1;
    /* validclrerr1 - Bits[20:20], RW1C, default = 1'h0 
       clearing request_valid when it's 0 (Ch1)
     */
    UINT32 validseterr1 : 1;
    /* validseterr1 - Bits[21:21], RW1C, default = 1'h0 
       setting request_valid when it's 1 (Ch1)
     */
    UINT32 rxdniderr1 : 1;
    /* rxdniderr1 - Bits[22:22], RW1C, default = 1'h0 
       received cycle with DNID!=FPGA_NID (Ch1)
     */
    UINT32 memrangeerr1 : 1;
    /* memrangeerr1 - Bits[23:23], RW1C, default = 1'h0 
       received request with address out of memory range (Ch1)
     */
    UINT32 respqparerr1 : 1;
    /* respqparerr1 - Bits[24:24], RW1C, default = 1'h0 
       response queue fifo parity error (Ch1)
     */
    UINT32 reqqparerr1 : 1;
    /* reqqparerr1 - Bits[25:25], RW1C, default = 1'h0 
       request queue fifo parity error (Ch1)
     */
    UINT32 reloopqparerr1 : 1;
    /* reloopqparerr1 - Bits[26:26], RW1C, default = 1'h0 
       reloop queue fifo parity error (Ch1)
     */
    UINT32 cmpqparerr1 : 1;
    /* cmpqparerr1 - Bits[27:27], RW1C, default = 1'h0 
       completion queue parity error (Ch1)
     */
    UINT32 intfredirectparerr1 : 2;
    /* intfredirectparerr1 - Bits[29:28], RW1C, default = 2'h0 
       Interface redirection patity error (Ch1)
     */
    UINT32 reserved30 : 2;
    /* reserved30 - Bits[31:30], RW1C, default = 2'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_STAT2_FPGA_CCI_STRUCT;


/* KTI_ERR_EN2_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x400803A4)                                                    */
/* Register default value:              0x0FFF0FFF                            */
#define KTI_ERR_EN2_FPGA_CCI_REG 0x140043A4
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI Protocol Layer Error Enable (Reserved[31:16] when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 llerrench0 : 12;
    /* llerrench0 - Bits[11:0], RW, default = 12'hfff 
       0 - disable corresponding bit in ERR_STAT2
       1 - enable corresponding error detection
     */
    UINT32 reserved12 : 4;
    /* reserved12 - Bits[15:12], RW, default = 4'h0 
       Reserved
     */
    UINT32 llerrench1 : 12;
    /* llerrench1 - Bits[27:16], RW, default = 12'hfff 
       0 - disable corresponding bit in ERR_STAT2
       1 - enable corresponding error detection
     */
    UINT32 reserved28 : 4;
    /* reserved28 - Bits[31:28], RW, default = 4'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_EN2_FPGA_CCI_STRUCT;


/* KTI_ERR_STAT3_FPGA_CCI_REG supported on:                                   */
/*       SKXP (0x400803A8)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_ERR_STAT3_FPGA_CCI_REG 0x140043A8
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Cache Controller Channel 0 Error
 */
typedef union {
  struct {
    UINT32 ccirdevictparerr : 1;
    /* ccirdevictparerr - Bits[0:0], RW1C, default = 1'h0 
       cci_pu_rd evict fifo parity error
     */
    UINT32 ccirdevicterr : 1;
    /* ccirdevicterr - Bits[1:1], RW1C, default = 1'h0 
       cci_pu_rd evict fifo overflow/underflow
     */
    UINT32 holdramparerr : 1;
    /* holdramparerr - Bits[2:2], RW1C, default = 1'h0 
       hram parity error
     */
    UINT32 compramparerr : 1;
    /* compramparerr - Bits[3:3], RW1C, default = 1'h0 
       completion ram parity error
     */
    UINT32 wrrxfifoparerr : 1;
    /* wrrxfifoparerr - Bits[4:4], RW1C, default = 1'h0 
       wrrx fifo parity error
     */
    UINT32 wrrxfifoerr : 1;
    /* wrrxfifoerr - Bits[5:5], RW1C, default = 1'h0 
       wrrx fifo overflow/underflow
     */
    UINT32 grantidowriteerr : 1;
    /* grantidowriteerr - Bits[6:6], RW1C, default = 1'h0 
       Grant ID overwrite error
     */
    UINT32 grantidconferr : 1;
    /* grantidconferr - Bits[7:7], RW1C, default = 1'h0 
       Grant ID Conflict error
     */
    UINT32 rxchconferr : 1;
    /* rxchconferr - Bits[8:8], RW1C, default = 1'h0 
       Rx channel conflict error
     */
    UINT32 ccicomp0inviderr : 1;
    /* ccicomp0inviderr - Bits[9:9], RW1C, default = 1'h0 
       cci completion port0 with an invalid TID
     */
    UINT32 ccicomp1inviderr : 1;
    /* ccicomp1inviderr - Bits[10:10], RW1C, default = 1'h0 
       cci completion port1 with an invalid TID
     */
    UINT32 rdcompinprt1err : 1;
    /* rdcompinprt1err - Bits[11:11], RW1C, default = 1'h0 
       illegal read completion on cci port1
     */
    UINT32 wridorwrramparerr : 1;
    /* wridorwrramparerr - Bits[12:12], RW1C, default = 1'h0 
       write ID fifo or write ram parity error
     */
    UINT32 wrtxhdrfifoparerr : 1;
    /* wrtxhdrfifoparerr - Bits[13:13], RW1C, default = 1'h0 
       write Tx header fifo parity error
     */
    UINT32 rdtxhdrfifoparerr : 1;
    /* rdtxhdrfifoparerr - Bits[14:14], RW1C, default = 1'h0 
       read Tx header fifo parity error
     */
    UINT32 wridfifoerr : 1;
    /* wridfifoerr - Bits[15:15], RW1C, default = 1'h0 
       write ID fifo overflow/underflow
     */
    UINT32 wrtxhdrfifoerr : 1;
    /* wrtxhdrfifoerr - Bits[16:16], RW1C, default = 1'h0 
       write Tx header fifo overflow/underflow
     */
    UINT32 rdtxhdrfifoerr : 1;
    /* rdtxhdrfifoerr - Bits[17:17], RW1C, default = 1'h0 
       read Tx header fifo overflow/underlow
     */
    UINT32 ccicnf0overwriteerr : 1;
    /* ccicnf0overwriteerr - Bits[18:18], RW1C, default = 1'h0 
       cci conflict pipe 0 overwrite an entry
     */
    UINT32 ccicnf0reademptyerr : 1;
    /* ccicnf0reademptyerr - Bits[19:19], RW1C, default = 1'h0 
       cci conflict pipe 0 cleared an empty entry
     */
    UINT32 ccicnf1overwriteerr : 1;
    /* ccicnf1overwriteerr - Bits[20:20], RW1C, default = 1'h0 
       cci conflict pipe 1 overwrite an entry
     */
    UINT32 ccicnf1reademptyerr : 1;
    /* ccicnf1reademptyerr - Bits[21:21], RW1C, default = 1'h0 
       cci conflict pipe 1 cleared an empty entry
     */
    UINT32 cciwrevictparerr : 1;
    /* cciwrevictparerr - Bits[22:22], RW1C, default = 1'h0 
       cci_pu_wr evict fifo parity error
     */
    UINT32 cciwrevicterr : 1;
    /* cciwrevicterr - Bits[23:23], RW1C, default = 1'h0 
       cci_pu_wr evict fifo overflow/underflow
     */
    UINT32 rdreissuefifoparerr : 1;
    /* rdreissuefifoparerr - Bits[24:24], RW1C, default = 1'h0 
       read reissue fifo parity error
     */
    UINT32 wrreissuefifoparerr : 1;
    /* wrreissuefifoparerr - Bits[25:25], RW1C, default = 1'h0 
       write reissue fifo parity error
     */
    UINT32 rdreissuefifoerr : 1;
    /* rdreissuefifoerr - Bits[26:26], RW1C, default = 1'h0 
       read reissue fifo overflow/underflow
     */
    UINT32 wrreissuefifoerr : 1;
    /* wrreissuefifoerr - Bits[27:27], RW1C, default = 1'h0 
       write reissue fifo overflow/underflow
     */
    UINT32 reqmonfifoparerr : 1;
    /* reqmonfifoparerr - Bits[28:28], RW1C, default = 1'h0 
       request monitor fifo parity error
     */
    UINT32 reqmonrespfifoerr : 1;
    /* reqmonrespfifoerr - Bits[29:29], RW1C, default = 1'h0 
       request monitor Rx response fifo overlow/underflow
     */
    UINT32 reqmonrdfifoerr : 1;
    /* reqmonrdfifoerr - Bits[30:30], RW1C, default = 1'h0 
       request monitor Read request fifo overlow/underflow
     */
    UINT32 reserved31 : 1;
    /* reserved31 - Bits[31:31], RW1C, default = 1'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_STAT3_FPGA_CCI_STRUCT;


/* KTI_ERR_EN3_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x400803AC)                                                    */
/* Register default value:              0x7FFFFFFF                            */
#define KTI_ERR_EN3_FPGA_CCI_REG 0x140043AC
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Cache Controller Channel 0 Error Enable
 */
typedef union {
  struct {
    UINT32 ccierrench0 : 31;
    /* ccierrench0 - Bits[30:0], RW, default = 31'h7fffffff 
       0 - disable corresponding bit in ERR_STAT3
       1 - enable corresponding error detection
     */
    UINT32 reserved31 : 1;
    /* reserved31 - Bits[31:31], RW, default = 1'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_EN3_FPGA_CCI_STRUCT;


/* KTI_ERR_STAT4_FPGA_CCI_REG supported on:                                   */
/*       SKXP (0x400803B0)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_ERR_STAT4_FPGA_CCI_REG 0x140043B0
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Cache Controller Channel 1 Error (Reserved when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 ccirdevictparerr : 1;
    /* ccirdevictparerr - Bits[0:0], RW1C, default = 1'h0 
       cci_pu_rd evict fifo parity error
     */
    UINT32 ccirdevicterr : 1;
    /* ccirdevicterr - Bits[1:1], RW1C, default = 1'h0 
       cci_pu_rd evict fifo overflow/underflow
     */
    UINT32 holdramparerr : 1;
    /* holdramparerr - Bits[2:2], RW1C, default = 1'h0 
       hram parity error
     */
    UINT32 compramparerr : 1;
    /* compramparerr - Bits[3:3], RW1C, default = 1'h0 
       completion ram parity error
     */
    UINT32 wrrxfifoparerr : 1;
    /* wrrxfifoparerr - Bits[4:4], RW1C, default = 1'h0 
       wrrx fifo parity error
     */
    UINT32 wrrxfifoerr : 1;
    /* wrrxfifoerr - Bits[5:5], RW1C, default = 1'h0 
       wrrx fifo overflow/underflow
     */
    UINT32 grantidowriteerr : 1;
    /* grantidowriteerr - Bits[6:6], RW1C, default = 1'h0 
       Grant ID overwrite error
     */
    UINT32 grantidconferr : 1;
    /* grantidconferr - Bits[7:7], RW1C, default = 1'h0 
       Grant ID Conflict error
     */
    UINT32 rxchconferr : 1;
    /* rxchconferr - Bits[8:8], RW1C, default = 1'h0 
       Rx channel conflict error
     */
    UINT32 ccicomp0inviderr : 1;
    /* ccicomp0inviderr - Bits[9:9], RW1C, default = 1'h0 
       cci completion port0 with an invalid TID
     */
    UINT32 ccicomp1inviderr : 1;
    /* ccicomp1inviderr - Bits[10:10], RW1C, default = 1'h0 
       cci completion port1 with an invalid TID
     */
    UINT32 rdcompinprt1err : 1;
    /* rdcompinprt1err - Bits[11:11], RW1C, default = 1'h0 
       illegal read completion on cci port1
     */
    UINT32 wridorwrramparerr : 1;
    /* wridorwrramparerr - Bits[12:12], RW1C, default = 1'h0 
       write ID fifo or write ram parity error
     */
    UINT32 wrtxhdrfifoparerr : 1;
    /* wrtxhdrfifoparerr - Bits[13:13], RW1C, default = 1'h0 
       write Tx header fifo parity error
     */
    UINT32 rdtxhdrfifoparerr : 1;
    /* rdtxhdrfifoparerr - Bits[14:14], RW1C, default = 1'h0 
       read Tx header fifo parity error
     */
    UINT32 wridfifoerr : 1;
    /* wridfifoerr - Bits[15:15], RW1C, default = 1'h0 
       write ID fifo overflow/underflow
     */
    UINT32 wrtxhdrfifoerr : 1;
    /* wrtxhdrfifoerr - Bits[16:16], RW1C, default = 1'h0 
       write Tx header fifo overflow/underflow
     */
    UINT32 rdtxhdrfifoerr : 1;
    /* rdtxhdrfifoerr - Bits[17:17], RW1C, default = 1'h0 
       read Tx header fifo overflow/underlow
     */
    UINT32 ccicnf0overwriteerr : 1;
    /* ccicnf0overwriteerr - Bits[18:18], RW1C, default = 1'h0 
       cci conflict pipe 0 overwrite an entry
     */
    UINT32 ccicnf0reademptyerr : 1;
    /* ccicnf0reademptyerr - Bits[19:19], RW1C, default = 1'h0 
       cci conflict pipe 0 cleared an empty entry
     */
    UINT32 ccicnf1overwriteerr : 1;
    /* ccicnf1overwriteerr - Bits[20:20], RW1C, default = 1'h0 
       cci conflict pipe 1 overwrite an entry
     */
    UINT32 ccicnf1reademptyerr : 1;
    /* ccicnf1reademptyerr - Bits[21:21], RW1C, default = 1'h0 
       cci conflict pipe 1 cleared an empty entry
     */
    UINT32 cciwrevictparerr : 1;
    /* cciwrevictparerr - Bits[22:22], RW1C, default = 1'h0 
       cci_pu_wr evict fifo parity error
     */
    UINT32 cciwrevicterr : 1;
    /* cciwrevicterr - Bits[23:23], RW1C, default = 1'h0 
       cci_pu_wr evict fifo overflow/underflow
     */
    UINT32 rdreissuefifoparerr : 1;
    /* rdreissuefifoparerr - Bits[24:24], RW1C, default = 1'h0 
       read reissue fifo parity error
     */
    UINT32 wrreissuefifoparerr : 1;
    /* wrreissuefifoparerr - Bits[25:25], RW1C, default = 1'h0 
       write reissue fifo parity error
     */
    UINT32 rdreissuefifoerr : 1;
    /* rdreissuefifoerr - Bits[26:26], RW1C, default = 1'h0 
       read reissue fifo overflow/underflow
     */
    UINT32 wrreissuefifoerr : 1;
    /* wrreissuefifoerr - Bits[27:27], RW1C, default = 1'h0 
       write reissue fifo overflow/underflow
     */
    UINT32 reqmonfifoparerr : 1;
    /* reqmonfifoparerr - Bits[28:28], RW1C, default = 1'h0 
       request monitor fifo parity error
     */
    UINT32 reqmonrespfifoerr : 1;
    /* reqmonrespfifoerr - Bits[29:29], RW1C, default = 1'h0 
       request monitor Rx response fifo overlow/underflow
     */
    UINT32 reqmonrdfifoerr : 1;
    /* reqmonrdfifoerr - Bits[30:30], RW1C, default = 1'h0 
       request monitor Read request fifo overlow/underflow
     */
    UINT32 reserved31 : 1;
    /* reserved31 - Bits[31:31], RW1C, default = 1'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_STAT4_FPGA_CCI_STRUCT;


/* KTI_ERR_EN4_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x400803B4)                                                    */
/* Register default value:              0x7FFFFFFF                            */
#define KTI_ERR_EN4_FPGA_CCI_REG 0x140043B4
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Cache Controller Channel 1 Error Enable (Reserved when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 ccierrench1 : 31;
    /* ccierrench1 - Bits[30:0], RW, default = 31'h7fffffff 
       0 - disable corresponding bit in ERR_STAT4
       1 - enable corresponding error detection
     */
    UINT32 reserved31 : 1;
    /* reserved31 - Bits[31:31], RW, default = 1'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_EN4_FPGA_CCI_STRUCT;


/* KTI_ERR_STAT5_FPGA_CCI_REG supported on:                                   */
/*       SKXP (0x400803B8)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_ERR_STAT5_FPGA_CCI_REG 0x140043B8
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Local Tag/Cache Error (Reserved[31:16] when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 cacheparerr0 : 1;
    /* cacheparerr0 - Bits[0:0], RW1C, default = 1'h0 
       cache ram parity error (Ch0)
     */
    UINT32 remoddtagparerr0 : 1;
    /* remoddtagparerr0 - Bits[1:1], RW1C, default = 1'h0 
       remote odd tag ram parity error (Ch0)
     */
    UINT32 remoddtagmulthiterr0 : 1;
    /* remoddtagmulthiterr0 - Bits[2:2], RW1C, default = 1'h0 
       remote odd tag hit more than 1-way (Ch0)
     */
    UINT32 remoddtagwrcnflterr0 : 1;
    /* remoddtagwrcnflterr0 - Bits[3:3], RW1C, default = 1'h0 
       remote odd cci tag write address conflict with protocol write (Ch0)
     */
    UINT32 reserved4 : 1;
    /* reserved4 - Bits[4:4], RW1C, default = 1'h0 
       reserved 
     */
    UINT32 remeventagparerr0 : 1;
    /* remeventagparerr0 - Bits[5:5], RW1C, default = 1'h0 
       remote even tag ram parity error (Ch0)
     */
    UINT32 remeventagmulthiterr0 : 1;
    /* remeventagmulthiterr0 - Bits[6:6], RW1C, default = 1'h0 
       remote even tag hit more than 1-way (Ch0)
     */
    UINT32 remeventagwrcnflterr0 : 1;
    /* remeventagwrcnflterr0 - Bits[7:7], RW1C, default = 1'h0 
       remote even cci tag write address conflict with protocol write (Ch0)
     */
    UINT32 reserved8 : 1;
    /* reserved8 - Bits[8:8], RW1C, default = 1'h0 
       reserved 
     */
    UINT32 locoddtagparerr0 : 1;
    /* locoddtagparerr0 - Bits[9:9], RW1C, default = 1'h0 
       local odd tag ram parity error (Ch0)
     */
    UINT32 locoddtagmulthiterr0 : 1;
    /* locoddtagmulthiterr0 - Bits[10:10], RW1C, default = 1'h0 
       local odd tag hit more than 1-way (Ch0)
     */
    UINT32 locoddtagwrcnflterr0 : 1;
    /* locoddtagwrcnflterr0 - Bits[11:11], RW1C, default = 1'h0 
       local odd cci tag write address conflict with protocol write (Ch0)
     */
    UINT32 reserved12 : 1;
    /* reserved12 - Bits[12:12], RW1C, default = 1'h0 
       reserved 
     */
    UINT32 loceventagparerr0 : 1;
    /* loceventagparerr0 - Bits[13:13], RW1C, default = 1'h0 
       local even tag ram parity error (Ch0)
     */
    UINT32 loceventagmulthiterr0 : 1;
    /* loceventagmulthiterr0 - Bits[14:14], RW1C, default = 1'h0 
       local even tag hit more than 1-way (Ch0)
     */
    UINT32 loceventagwrcnflterr0 : 1;
    /* loceventagwrcnflterr0 - Bits[15:15], RW1C, default = 1'h0 
       local even cci tag write address conflict with protocol write (Ch0)
     */
    UINT32 cacheparerr : 1;
    /* cacheparerr - Bits[16:16], RW1C, default = 1'h0 
       cache ram parity error (Ch1)
     */
    UINT32 remoddtagparerr1 : 1;
    /* remoddtagparerr1 - Bits[17:17], RW1C, default = 1'h0 
       remote odd tag ram parity error (Ch1)
     */
    UINT32 remoddtagmulthiterr1 : 1;
    /* remoddtagmulthiterr1 - Bits[18:18], RW1C, default = 1'h0 
       remote odd tag hit more than 1-way (Ch1)
     */
    UINT32 remoddtagwrcnflterr1 : 1;
    /* remoddtagwrcnflterr1 - Bits[19:19], RW1C, default = 1'h0 
       remote odd cci tag write address conflict with protocol write (Ch1)
     */
    UINT32 reserved20 : 1;
    /* reserved20 - Bits[20:20], RW1C, default = 1'h0 
       reserved 
     */
    UINT32 remeventagparerr1 : 1;
    /* remeventagparerr1 - Bits[21:21], RW1C, default = 1'h0 
       remote even tag ram parity error (Ch1)
     */
    UINT32 remeventagmulthiterr1 : 1;
    /* remeventagmulthiterr1 - Bits[22:22], RW1C, default = 1'h0 
       remote even tag hit more than 1-way (Ch1)
     */
    UINT32 remeventagwrcnflterr1 : 1;
    /* remeventagwrcnflterr1 - Bits[23:23], RW1C, default = 1'h0 
       remote even cci tag write address conflict with protocol write (Ch1)
     */
    UINT32 reserved24 : 1;
    /* reserved24 - Bits[24:24], RW1C, default = 1'h0 
       reserved 
     */
    UINT32 locoddtagparerr1 : 1;
    /* locoddtagparerr1 - Bits[25:25], RW1C, default = 1'h0 
       local odd tag ram parity error (Ch1)
     */
    UINT32 locoddtagmulthiterr1 : 1;
    /* locoddtagmulthiterr1 - Bits[26:26], RW1C, default = 1'h0 
       local odd tag hit more than 1-way (Ch1)
     */
    UINT32 locoddtagwrcnflterr1 : 1;
    /* locoddtagwrcnflterr1 - Bits[27:27], RW1C, default = 1'h0 
       local odd cci tag write address conflict with protocol write (Ch1)
     */
    UINT32 reserved28 : 1;
    /* reserved28 - Bits[28:28], RW1C, default = 1'h0 
       reserved 
     */
    UINT32 loceventagparerr1 : 1;
    /* loceventagparerr1 - Bits[29:29], RW1C, default = 1'h0 
       local even tag ram parity error (Ch1)
     */
    UINT32 loceventagmulthiterr1 : 1;
    /* loceventagmulthiterr1 - Bits[30:30], RW1C, default = 1'h0 
       local even tag hit more than 1-way (Ch1)
     */
    UINT32 loceventagwrcnflterr1 : 1;
    /* loceventagwrcnflterr1 - Bits[31:31], RW1C, default = 1'h0 
       local even cci tag write address conflict with protocol write (Ch1)
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_STAT5_FPGA_CCI_STRUCT;


/* KTI_ERR_EN5_FPGA_CCI_REG supported on:                                     */
/*       SKXP (0x400803BC)                                                    */
/* Register default value:              0xEEEFEEEF                            */
#define KTI_ERR_EN5_FPGA_CCI_REG 0x140043BC
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * Local Tag/Cache Error Enable (Reserved[31:16] when SINGLE_CHANNEL)
 */
typedef union {
  struct {
    UINT32 cacheerren0 : 1;
    /* cacheerren0 - Bits[0:0], RW, default = 1'h1 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 remoddtagerren0 : 3;
    /* remoddtagerren0 - Bits[3:1], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 reserved4 : 1;
    /* reserved4 - Bits[4:4], RW, default = 1'h0 
       Reserved
     */
    UINT32 remeventagerren0 : 3;
    /* remeventagerren0 - Bits[7:5], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 reserved8 : 1;
    /* reserved8 - Bits[8:8], RW, default = 1'h0 
       Reserved
     */
    UINT32 locoddtagerren0 : 3;
    /* locoddtagerren0 - Bits[11:9], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 reserved12 : 1;
    /* reserved12 - Bits[12:12], RW, default = 1'h0 
       Reserved
     */
    UINT32 loceventagerren0 : 3;
    /* loceventagerren0 - Bits[15:13], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 cacheerren1 : 1;
    /* cacheerren1 - Bits[16:16], RW, default = 1'h1 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 remoddtagerren1 : 3;
    /* remoddtagerren1 - Bits[19:17], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 reserved20 : 1;
    /* reserved20 - Bits[20:20], RW, default = 1'h0 
       Reserved
     */
    UINT32 remeventagerren1 : 3;
    /* remeventagerren1 - Bits[23:21], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 reserved24 : 1;
    /* reserved24 - Bits[24:24], RW, default = 1'h0 
       Reserved
     */
    UINT32 locoddtagerren1 : 3;
    /* locoddtagerren1 - Bits[27:25], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
    UINT32 reserved28 : 1;
    /* reserved28 - Bits[28:28], RW, default = 1'h0 
       Reserved
     */
    UINT32 loceventagerren1 : 3;
    /* loceventagerren1 - Bits[31:29], RW, default = 3'h7 
       0 - disable corresponding bit in ERR_STAT5
       1 - enable corresponding error detection
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_EN5_FPGA_CCI_STRUCT;


/* KTI_FERR_FPGA_CCI_REG supported on:                                        */
/*       SKXP (0x400803C8)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_FERR_FPGA_CCI_REG 0x140043C8
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI first error
 */
typedef union {
  struct {
    UINT32 errorstatus : 32;
    /* errorstatus - Bits[31:0], RW1C, default = 32'h0 
       Holds the Status of the Error Register that was triggered first
     */
  } Bits;
  UINT32 Data;
} KTI_FERR_FPGA_CCI_STRUCT;


/* KTI_NERR_FPGA_CCI_REG supported on:                                        */
/*       SKXP (0x400803CC)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_NERR_FPGA_CCI_REG 0x140043CC
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI next error
 */
typedef union {
  struct {
    UINT32 errorstatus : 32;
    /* errorstatus - Bits[31:0], RW1C, default = 32'h0 
       Holds the status of the Error Register that was triggered second
     */
  } Bits;
  UINT32 Data;
} KTI_NERR_FPGA_CCI_STRUCT;


/* KTI_ERR_INFO_FPGA_CCI_REG supported on:                                    */
/*       SKXP (0x400803D0)                                                    */
/* Register default value:              0x00000000                            */
#define KTI_ERR_INFO_FPGA_CCI_REG 0x140043D0
/* Struct format extracted from XML file 1\0_16_0_CFG.xml.
 * KTI parity error counter
 */
typedef union {
  struct {
    UINT32 ktiperrcnt : 26;
    /* ktiperrcnt - Bits[25:0], RO, default = 26'h0 
       Number of parity errors detected in KTI controller
     */
    UINT32 nerrorsourceid : 3;
    /* nerrorsourceid - Bits[28:26], RO, default = 3'h0 
       Indicates the Error Register that was triggered second
       0 - KTI Link Layer Error 0
       1 - KTI Link Layer Error 1
       2 - KTI Protocol Layer Error
       3 - Cache Controller Error
       4 - Local Tag/Cache Error
       5 - Snoop Filter Error (only for HA)
     */
    UINT32 ferrorsourceid : 3;
    /* ferrorsourceid - Bits[31:29], RO, default = 3'h0 
       Indicates the Error Register that was triggered first
       0 - KTI Link Layer Error 0
       1 - KTI Link Layer Error 1
       2 - KTI Protocol Layer Error
       3 - Cache Controller Error
       4 - Local Tag/Cache Error
       5 - Snoop Filter Error (only for HA)
     */
  } Bits;
  UINT32 Data;
} KTI_ERR_INFO_FPGA_CCI_STRUCT;


#endif /* FPGA_CCI_h */
