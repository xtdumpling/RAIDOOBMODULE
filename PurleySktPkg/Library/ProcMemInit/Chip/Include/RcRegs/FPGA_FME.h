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

#ifndef FPGA_FME_h
#define FPGA_FME_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FPGA_FME_DEV 16                                                            */
/* FPGA_FME_FUN 0                                                             */

/* FME_DFH_N0_FPGA_FME_REG supported on:                                      */
/*       SKXP (0x00000000)                                                    */
/* Register default value:              0x10000001                            */
#define FME_DFH_N0_FPGA_FME_REG 0x00000000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME version
 */
typedef union {
  struct {
    UINT32 cciversion : 12;
    /* cciversion - Bits[11:0], RO, default = 12'h1 
       CCI-P version #
     */
    UINT32 cciminorrev : 4;
    /* cciminorrev - Bits[15:12], RO, default = 4'h0 
       CCI-P minor revision
     */
    UINT32 nextdfhoffset : 16;
    /* nextdfhoffset - Bits[31:16], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte Offset
     */
  } Bits;
  UINT32 Data;
} FME_DFH_N0_FPGA_FME_STRUCT;


/* FME_DFH_N1_FPGA_FME_REG supported on:                                      */
/*       SKXP (0x00000004)                                                    */
/* Register default value:              0x200001000                           */
#define FME_DFH_N1_FPGA_FME_REG 0x00000004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME version
 */
typedef union {
  struct {
    UINT32 nextdfhoffset : 8;
    /* nextdfhoffset - Bits[7:0], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte Offset
     */
    UINT32 reserved40 : 20;
    /* reserved40 - Bits[27:8], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 featuretype : 4;
    /* featuretype - Bits[31:28], RO, default = 4'h1 
       Feature Type = AFU
     */
  } Bits;
  UINT32 Data;
} FME_DFH_N1_FPGA_FME_STRUCT;


/* FME_AFU_ID_L_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000008)                                                    */
/* Register default value:              0x82FE38F0F9E17764                    */
#define FME_AFU_ID_L_N0_FPGA_FME_REG 0x00000008
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME AFU identifier_L
 */
typedef union {
  struct {
    UINT32 afuidlow : 32;
    /* afuidlow - Bits[31:0], RO, default = 64'h82FE38F0F9E17764 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME AFU ID low 
     */
  } Bits;
  UINT32 Data;
} FME_AFU_ID_L_N0_FPGA_FME_STRUCT;


/* FME_AFU_ID_L_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000000C)                                                    */
/* Register default value:              0x82FE38F0F9E17764                    */
#define FME_AFU_ID_L_N1_FPGA_FME_REG 0x0000000C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME AFU identifier_L
 */
typedef union {
  struct {
    UINT32 afuidlow : 32;
    /* afuidlow - Bits[31:0], RO, default = 64'h82FE38F0F9E17764 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME AFU ID low 
     */
  } Bits;
  UINT32 Data;
} FME_AFU_ID_L_N1_FPGA_FME_STRUCT;


/* FME_AFU_ID_H_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000010)                                                    */
/* Register default value:              0xBFAF2AE94A5246E3                    */
#define FME_AFU_ID_H_N0_FPGA_FME_REG 0x00000010
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME AFU identifier_H
 */
typedef union {
  struct {
    UINT32 afuidhigh : 32;
    /* afuidhigh - Bits[31:0], RO, default = 64'hBFAF2AE94A5246E3 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME AFU ID high
     */
  } Bits;
  UINT32 Data;
} FME_AFU_ID_H_N0_FPGA_FME_STRUCT;


/* FME_AFU_ID_H_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000014)                                                    */
/* Register default value:              0xBFAF2AE94A5246E3                    */
#define FME_AFU_ID_H_N1_FPGA_FME_REG 0x00000014
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME AFU identifier_H
 */
typedef union {
  struct {
    UINT32 afuidhigh : 32;
    /* afuidhigh - Bits[31:0], RO, default = 64'hBFAF2AE94A5246E3 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME AFU ID high
     */
  } Bits;
  UINT32 Data;
} FME_AFU_ID_H_N1_FPGA_FME_STRUCT;


/* FME_NEXT_AFU_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000018)                                                    */
/* Register default value:              0x00000000                            */
#define FME_NEXT_AFU_N0_FPGA_FME_REG 0x00000018
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME next AFU
 */
typedef union {
  struct {
    UINT32 nextafudfhoffset : 24;
    /* nextafudfhoffset - Bits[23:0], RO, default = 24'h0 
       Next AFU DFH Byte offset=0. End of AFU linked list. The Port Capability 
       registers points to the Port and User AFU linked list. 
     */
    UINT32 reserved24 : 8;
    /* reserved24 - Bits[31:24], RsvdZ, default = 40'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_NEXT_AFU_N0_FPGA_FME_STRUCT;


/* FME_NEXT_AFU_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000001C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_NEXT_AFU_N1_FPGA_FME_REG 0x0000001C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME next AFU
 */
typedef union {
  struct {
    UINT32 reserved24 : 32;
    /* reserved24 - Bits[31:0], RsvdZ, default = 40'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_NEXT_AFU_N1_FPGA_FME_STRUCT;


/* FME_SCRATCHPAD0_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00000028)                                                    */
/* Register default value:              0x00000000                            */
#define FME_SCRATCHPAD0_N0_FPGA_FME_REG 0x00000028
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME scratchpad register
 */
typedef union {
  struct {
    UINT32 scratchpad : 32;
    /* scratchpad - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Scratchpad register
     */
  } Bits;
  UINT32 Data;
} FME_SCRATCHPAD0_N0_FPGA_FME_STRUCT;


/* FME_SCRATCHPAD0_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x0000002C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_SCRATCHPAD0_N1_FPGA_FME_REG 0x0000002C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME scratchpad register
 */
typedef union {
  struct {
    UINT32 scratchpad : 32;
    /* scratchpad - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Scratchpad register
     */
  } Bits;
  UINT32 Data;
} FME_SCRATCHPAD0_N1_FPGA_FME_STRUCT;


/* FAB_CAPABILITY_N0_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00000030)                                                    */
/* Register default value:              0x26027100                            */
#define FAB_CAPABILITY_N0_FPGA_FME_REG 0x00000030
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FAB capability register
 */
typedef union {
  struct {
    UINT32 fabricversion : 8;
    /* fabricversion - Bits[7:0], RO, default = 8'h0 
       Fabric version ID. Identifies protocol changes made to the interface,
       example adding User mode interrupts. Current value=0
     */
    UINT32 socketid : 1;
    /* socketid - Bits[8:8], RO, default = 1'h1 
       Socket ID
       BDX - driven over NOA pin
       SKX - programmed via SMBus
     */
    UINT32 reserved9 : 3;
    /* reserved9 - Bits[11:9], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 pci0link : 1;
    /* pci0link - Bits[12:12], RO, default = 1'h1 
       PCIe0 link available - Yes/No
       This is not a PCIe link status register.
     */
    UINT32 pcie1link : 1;
    /* pcie1link - Bits[13:13], RO, default = 1'h1 
       PCIe1 link available - Yes/No
       This is not a PCIe link status register.
     */
    UINT32 coherentlink : 1;
    /* coherentlink - Bits[14:14], RO, default = 1'h1 
       Coherent (QPI/UPI) link available - Yes/No
       This is not a QPI/UPI link status register.
     */
    UINT32 reserved15 : 1;
    /* reserved15 - Bits[15:15], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 iommuavaliability : 1;
    /* iommuavaliability - Bits[16:16], RO, default = 1'h0 
       0 - IOMMU is not available
       1 - IOMMU is available
     */
    UINT32 numports : 3;
    /* numports - Bits[19:17], RO, default = 3'h1 
       Number of ports implemented
     */
    UINT32 reserved20 : 4;
    /* reserved20 - Bits[23:20], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 addresswidth : 6;
    /* addresswidth - Bits[29:24], RO, default = 6'h26 
       Address bus width in bits
     */
    UINT32 reserved30 : 2;
    /* reserved30 - Bits[31:30], RsvdZ, default = 2'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FAB_CAPABILITY_N0_FPGA_FME_STRUCT;


/* FAB_CAPABILITY_N1_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00000034)                                                    */
/* Register default value:              0x00000000                            */
#define FAB_CAPABILITY_N1_FPGA_FME_REG 0x00000034
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FAB capability register
 */
typedef union {
  struct {
    UINT32 cachesize : 12;
    /* cachesize - Bits[11:0], RO, default = 12'h0 
       Cache size in KB
     */
    UINT32 cacheassociativity : 4;
    /* cacheassociativity - Bits[15:12], RO, default = 4'h0 
       Cache associativity
       0 - direct mapped cache
     */
    UINT32 reserved48 : 15;
    /* reserved48 - Bits[30:16], RsvdZ, default = 15'h0 
       Reserved
     */
    UINT32 lockbit : 1;
    /* lockbit - Bits[31:31], RW1S, default = 1'h0 
       BIOS can read or write to this bit. Once set to 1, the value is latched
     */
  } Bits;
  UINT32 Data;
} FAB_CAPABILITY_N1_FPGA_FME_STRUCT;


/* PORT0_OFFSET_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000038)                                                    */
/* Register default value:              0x00000000                            */
#define PORT0_OFFSET_N0_FPGA_FME_REG 0x00000038
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port0 offset
 */
typedef union {
  struct {
    UINT32 portbyteoffset : 24;
    /* portbyteoffset - Bits[23:0], RO, default = 24'h0 
       CCI-P Port 0 byte offset
     */
    UINT32 reserved24 : 8;
    /* reserved24 - Bits[31:24], RsvdZ, default = 8'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT0_OFFSET_N0_FPGA_FME_STRUCT;


/* PORT0_OFFSET_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000003C)                                                    */
/* Register default value:              0x10000002                            */
#define PORT0_OFFSET_N1_FPGA_FME_REG 0x0000003C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port0 offset
 */
typedef union {
  struct {
    UINT32 barid : 3;
    /* barid - Bits[2:0], RO, default = 3'h2 
       BAR #
     */
    UINT32 reserved35 : 20;
    /* reserved35 - Bits[22:3], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 afuaccessctrl : 1;
    /* afuaccessctrl - Bits[23:23], RW, default = 1'h0 
       This bit sets the AFU access type:
       0 - Memory requests from AFU will be mapped to PF domain. AFU MMIO access 
       permission set to PF access. VF access to AFU is illegal. 
       1 - Memory requests from AFU will be mapped to VF domain. AFU MMIO access 
       permission set to VF access. PF accesses to AFU are illegal. 
     */
    UINT32 reserved56 : 4;
    /* reserved56 - Bits[27:24], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 portimplemented : 1;
    /* portimplemented - Bits[28:28], RO, default = 1'h1 
       Port implemented?
       SW reads this to determine if Port 0 is available.
       0 - Not implemented
       1 - implemented
     */
    UINT32 reserved61 : 3;
    /* reserved61 - Bits[31:29], RsvdZ, default = 3'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT0_OFFSET_N1_FPGA_FME_STRUCT;


/* PORT1_OFFSET_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000040)                                                    */
/* Register default value:              0x00080000                            */
#define PORT1_OFFSET_N0_FPGA_FME_REG 0x00000040
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port1 offset
 */
typedef union {
  struct {
    UINT32 portbyteoffset : 24;
    /* portbyteoffset - Bits[23:0], RO, default = 24'h80000 
       CCI-P Port 1 byte offset
     */
    UINT32 reserved24 : 8;
    /* reserved24 - Bits[31:24], RsvdZ, default = 8'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT1_OFFSET_N0_FPGA_FME_STRUCT;


/* PORT1_OFFSET_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000044)                                                    */
/* Register default value:              0x00000002                            */
#define PORT1_OFFSET_N1_FPGA_FME_REG 0x00000044
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port1 offset
 */
typedef union {
  struct {
    UINT32 barid : 3;
    /* barid - Bits[2:0], RO, default = 3'h2 
       BAR #
     */
    UINT32 reserved35 : 20;
    /* reserved35 - Bits[22:3], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 afuaccessctrl : 1;
    /* afuaccessctrl - Bits[23:23], RW, default = 1'h0 
       This bit sets the AFU access type:
       0 - Memory requests from AFU will be mapped to PF domain. AFU MMIO access 
       permission set to PF access. VF access to AFU is illegal. 
       1 - Memory requests from AFU will be mapped to VF domain. AFU MMIO access 
       permission set to VF access. PF accesses to AFU are illegal. 
     */
    UINT32 reserved56 : 4;
    /* reserved56 - Bits[27:24], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 portimplemented : 1;
    /* portimplemented - Bits[28:28], RO, default = 1'h0 
       Port implemented?
       SW reads this to determine if Port 0 is available.
       0 - Not implemented
       1 - implemented
     */
    UINT32 reserved61 : 3;
    /* reserved61 - Bits[31:29], RsvdZ, default = 3'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT1_OFFSET_N1_FPGA_FME_STRUCT;


/* PORT2_OFFSET_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000048)                                                    */
/* Register default value:              0x00100000                            */
#define PORT2_OFFSET_N0_FPGA_FME_REG 0x00000048
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port2 offset
 */
typedef union {
  struct {
    UINT32 portbyteoffset : 24;
    /* portbyteoffset - Bits[23:0], RO, default = 24'h100000 
       CCI-P Port 2 byte offset
     */
    UINT32 reserved24 : 8;
    /* reserved24 - Bits[31:24], RsvdZ, default = 8'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT2_OFFSET_N0_FPGA_FME_STRUCT;


/* PORT2_OFFSET_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000004C)                                                    */
/* Register default value:              0x00000002                            */
#define PORT2_OFFSET_N1_FPGA_FME_REG 0x0000004C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port2 offset
 */
typedef union {
  struct {
    UINT32 barid : 3;
    /* barid - Bits[2:0], RO, default = 3'h2 
       BAR #
     */
    UINT32 reserved35 : 20;
    /* reserved35 - Bits[22:3], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 afuaccessctrl : 1;
    /* afuaccessctrl - Bits[23:23], RW, default = 1'h0 
       This bit sets the AFU access type:
       0 - Memory requests from AFU will be mapped to PF domain. AFU MMIO access 
       permission set to PF access. VF access to AFU is illegal. 
       1 - Memory requests from AFU will be mapped to VF domain. AFU MMIO access 
       permission set to VF access. PF accesses to AFU are illegal. 
     */
    UINT32 reserved56 : 4;
    /* reserved56 - Bits[27:24], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 portimplemented : 1;
    /* portimplemented - Bits[28:28], RO, default = 1'h0 
       Port implemented?
       SW reads this to determine if Port 0 is available.
       0 - Not implemented
       1 - implemented
     */
    UINT32 reserved61 : 3;
    /* reserved61 - Bits[31:29], RsvdZ, default = 3'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT2_OFFSET_N1_FPGA_FME_STRUCT;


/* PORT3_OFFSET_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000050)                                                    */
/* Register default value:              0x00180000                            */
#define PORT3_OFFSET_N0_FPGA_FME_REG 0x00000050
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port3 offset
 */
typedef union {
  struct {
    UINT32 portbyteoffset : 24;
    /* portbyteoffset - Bits[23:0], RO, default = 24'h180000 
       CCI-P Port 3 byte offset
     */
    UINT32 reserved24 : 8;
    /* reserved24 - Bits[31:24], RsvdZ, default = 8'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT3_OFFSET_N0_FPGA_FME_STRUCT;


/* PORT3_OFFSET_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000054)                                                    */
/* Register default value:              0x00000002                            */
#define PORT3_OFFSET_N1_FPGA_FME_REG 0x00000054
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Port3 offset
 */
typedef union {
  struct {
    UINT32 barid : 3;
    /* barid - Bits[2:0], RO, default = 3'h2 
       BAR #
     */
    UINT32 reserved35 : 20;
    /* reserved35 - Bits[22:3], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 afuaccessctrl : 1;
    /* afuaccessctrl - Bits[23:23], RW, default = 1'h0 
       This bit sets the AFU access type:
       0 - Memory requests from AFU will be mapped to PF domain. AFU MMIO access 
       permission set to PF access. VF access to AFU is illegal. 
       1 - Memory requests from AFU will be mapped to VF domain. AFU MMIO access 
       permission set to VF access. PF accesses to AFU are illegal. 
     */
    UINT32 reserved56 : 4;
    /* reserved56 - Bits[27:24], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 portimplemented : 1;
    /* portimplemented - Bits[28:28], RO, default = 1'h0 
       Port implemented?
       SW reads this to determine if Port 0 is available.
       0 - Not implemented
       1 - implemented
     */
    UINT32 reserved61 : 3;
    /* reserved61 - Bits[31:29], RsvdZ, default = 3'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PORT3_OFFSET_N1_FPGA_FME_STRUCT;


/* FAB_STATUS_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00000058)                                                    */
/* Register default value:              0x00000000                            */
#define FAB_STATUS_N0_FPGA_FME_REG 0x00000058
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FAB status register
 */
typedef union {
  struct {
    UINT32 upilinkstatus : 4;
    /* upilinkstatus - Bits[3:0], RO, default = 4'h0 
       UPI link status
       [3] Link initialization complete
       [2] TX link in L0 state
       [1] RX link in L0 state
       [0] Error captured in KTI_FERR register
     */
    UINT32 reserved4 : 4;
    /* reserved4 - Bits[7:4], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 pcie0linkstatus : 1;
    /* pcie0linkstatus - Bits[8:8], RO, default = 1'h0 
       PCIe0 link up
     */
    UINT32 reserved9 : 3;
    /* reserved9 - Bits[11:9], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 pcie1linkstatus : 1;
    /* pcie1linkstatus - Bits[12:12], RO, default = 1'h0 
       PCIe1 link up
     */
    UINT32 reserved13 : 19;
    /* reserved13 - Bits[31:13], RsvdZ, default = 51'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FAB_STATUS_N0_FPGA_FME_STRUCT;


/* FAB_STATUS_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x0000005C)                                                    */
/* Register default value:              0x00000000                            */
#define FAB_STATUS_N1_FPGA_FME_REG 0x0000005C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FAB status register
 */
typedef union {
  struct {
    UINT32 reserved13 : 32;
    /* reserved13 - Bits[31:0], RsvdZ, default = 51'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FAB_STATUS_N1_FPGA_FME_STRUCT;


/* BITSTREAM_ID_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000060)                                                    */
/* Register default value:              0x00000000                            */
#define BITSTREAM_ID_N0_FPGA_FME_REG 0x00000060
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Bitstream ID
 */
typedef union {
  struct {
    UINT32 githash : 32;
    /* githash - Bits[31:0], RO, default = 32'h0 
       GIT repository hash (8 less significant digits)
     */
  } Bits;
  UINT32 Data;
} BITSTREAM_ID_N0_FPGA_FME_STRUCT;


/* BITSTREAM_ID_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000064)                                                    */
/* Register default value:              0x00000000                            */
#define BITSTREAM_ID_N1_FPGA_FME_REG 0x00000064
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Bitstream ID
 */
typedef union {
  struct {
    UINT32 hssiid : 4;
    /* hssiid - Bits[3:0], RO, default = 4'h0 
       HSSI configuration identifier:
       0 - No HSSI
       1 - PCIe-RP
       2 - Ethernet
     */
    UINT32 reserved36 : 12;
    /* reserved36 - Bits[15:4], RO, default = 12'h0 
       Reserved
     */
    UINT32 verdebug : 4;
    /* verdebug - Bits[19:16], RO, default = 4'h0 
       Bitstream version debug number
     */
    UINT32 verpatch : 4;
    /* verpatch - Bits[23:20], RO, default = 4'h0 
       Bitstream version patch number
     */
    UINT32 verminor : 4;
    /* verminor - Bits[27:24], RO, default = 4'h0 
       Bitstream version minor number
     */
    UINT32 vermajor : 4;
    /* vermajor - Bits[31:28], RO, default = 4'h0 
       Bitstream version major number
     */
  } Bits;
  UINT32 Data;
} BITSTREAM_ID_N1_FPGA_FME_STRUCT;


/* BITSTREAM_MD_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00000068)                                                    */
/* Register default value:              0x00000000                            */
#define BITSTREAM_MD_N0_FPGA_FME_REG 0x00000068
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Bitstream Meta data
 */
typedef union {
  struct {
    UINT32 synthseed : 4;
    /* synthseed - Bits[3:0], RO, default = 4'h0 
       Seed number used for synthesis flow
     */
    UINT32 synthday : 8;
    /* synthday - Bits[11:4], RO, default = 8'h0 
       Synthesis date (day number - 2 digits)
     */
    UINT32 synthmonth : 8;
    /* synthmonth - Bits[19:12], RO, default = 8'h0 
       Synthesis date (month number - 2 digits)
     */
    UINT32 synthyear : 8;
    /* synthyear - Bits[27:20], RO, default = 8'h0 
       Synthesis date (year number - 2 digits)
     */
    UINT32 reserved : 4;
    /* reserved - Bits[31:28], RO, default = 36'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} BITSTREAM_MD_N0_FPGA_FME_STRUCT;


/* BITSTREAM_MD_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000006C)                                                    */
/* Register default value:              0x00000000                            */
#define BITSTREAM_MD_N1_FPGA_FME_REG 0x0000006C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Bitstream Meta data
 */
typedef union {
  struct {
    UINT32 reserved : 32;
    /* reserved - Bits[31:0], RO, default = 36'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} BITSTREAM_MD_N1_FPGA_FME_STRUCT;


/* GENPROTRANGE2_BASE_N0_FPGA_FME_REG supported on:                           */
/*       SKXP (0x00000070)                                                    */
/* Register default value:              0x000F0000                            */
#define GENPROTRANGE2_BASE_N0_FPGA_FME_REG 0x00000070
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Protected range base address
 */
typedef union {
  struct {
    UINT32 reserved0 : 16;
    /* reserved0 - Bits[15:0], RsvdZ, default = 16'h0 
       Reserved
     */
    UINT32 addr : 4;
    /* addr - Bits[19:16], RW-L, default = 4'hf 
       base address [19:16] of memory range to protect from inbound DMA access.
       Expectation is this region is used to protect PAM region access from 0xa_0000 to 
       0xf_0000 
     */
    UINT32 reserved20 : 12;
    /* reserved20 - Bits[31:20], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} GENPROTRANGE2_BASE_N0_FPGA_FME_STRUCT;


/* GENPROTRANGE2_BASE_N1_FPGA_FME_REG supported on:                           */
/*       SKXP (0x00000074)                                                    */
/* Register default value:              0x00000000                            */
#define GENPROTRANGE2_BASE_N1_FPGA_FME_REG 0x00000074
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Protected range base address
 */
typedef union {
  struct {
    UINT32 reserved20 : 32;
    /* reserved20 - Bits[31:0], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} GENPROTRANGE2_BASE_N1_FPGA_FME_STRUCT;


/* GENPROTRANGE2_LIMIT_N0_FPGA_FME_REG supported on:                          */
/*       SKXP (0x00000078)                                                    */
/* Register default value:              0x00000000                            */
#define GENPROTRANGE2_LIMIT_N0_FPGA_FME_REG 0x00000078
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Protected range limit address
 */
typedef union {
  struct {
    UINT32 reserved0 : 16;
    /* reserved0 - Bits[15:0], RsvdZ, default = 16'h0 
       Reserved
     */
    UINT32 addr : 4;
    /* addr - Bits[19:16], RW-L, default = 4'h0 
       limit address [19:16] of memory range to protect from inbound DMA access.
       Expectation is this region is used to protect PAM region access from 0xa_0000 - 
       0xf_0000 
     */
    UINT32 reserved20 : 11;
    /* reserved20 - Bits[30:20], RsvdZ, default = 11'h0 
       Reserved
     */
    UINT32 enable : 1;
    /* enable - Bits[31:31], RW-L, default = 1'h0 
       Enable GENPROTRANGE check
     */
  } Bits;
  UINT32 Data;
} GENPROTRANGE2_LIMIT_N0_FPGA_FME_STRUCT;


/* GENPROTRANGE2_LIMIT_N1_FPGA_FME_REG supported on:                          */
/*       SKXP (0x0000007C)                                                    */
/* Register default value:              0x00000000                            */
#define GENPROTRANGE2_LIMIT_N1_FPGA_FME_REG 0x0000007C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Protected range limit address
 */
typedef union {
  struct {
    UINT32 reserved32 : 32;
    /* reserved32 - Bits[31:0], RsvdZ, default = 32'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} GENPROTRANGE2_LIMIT_N1_FPGA_FME_STRUCT;


/* DXE_LOCK_N0_FPGA_FME_REG supported on:                                     */
/*       SKXP (0x00000080)                                                    */
/* Register default value:              0x00000000                            */
#define DXE_LOCK_N0_FPGA_FME_REG 0x00000080
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * DXE Lock
 */
typedef union {
  struct {
    UINT32 dxeearlylock : 1;
    /* dxeearlylock - Bits[0:0], RW-L, default = 1'h0 
       Determines write access to the DXE region CSRs (1 - CSR region is locked, 0 - it 
       is open for write access). Red unlock region master CSR must be set in order to 
       write this CSR.  PrivReg0[0] in SMBUS space is red unlock master CSR. 
     */
    UINT32 reserved1 : 31;
    /* reserved1 - Bits[31:1], RsvdZ, default = 63'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} DXE_LOCK_N0_FPGA_FME_STRUCT;


/* DXE_LOCK_N1_FPGA_FME_REG supported on:                                     */
/*       SKXP (0x00000084)                                                    */
/* Register default value:              0x00000000                            */
#define DXE_LOCK_N1_FPGA_FME_REG 0x00000084
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * DXE Lock
 */
typedef union {
  struct {
    UINT32 reserved1 : 32;
    /* reserved1 - Bits[31:0], RsvdZ, default = 63'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} DXE_LOCK_N1_FPGA_FME_STRUCT;


/* HSSI_CTRL_N0_FPGA_FME_REG supported on:                                    */
/*       SKXP (0x00000088)                                                    */
/* Register default value:              0x00000000                            */
#define HSSI_CTRL_N0_FPGA_FME_REG 0x00000088
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * HSSI Control
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RW, default = 32'h0 
       Data
     */
  } Bits;
  UINT32 Data;
} HSSI_CTRL_N0_FPGA_FME_STRUCT;


/* HSSI_CTRL_N1_FPGA_FME_REG supported on:                                    */
/*       SKXP (0x0000008C)                                                    */
/* Register default value:              0x00000000                            */
#define HSSI_CTRL_N1_FPGA_FME_REG 0x0000008C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * HSSI Control
 */
typedef union {
  struct {
    UINT32 address : 16;
    /* address - Bits[15:0], RW, default = 16'h0 
       Address
     */
    UINT32 command : 16;
    /* command - Bits[31:16], RW, default = 16'h0 
       Command:
       0x0 - No request
       0x08 - SW register RD request
       0x10 - SW register WR request
       0x40 - Auxiliar bus RD request
       0x80 - Auxiliar bus WR request
     */
  } Bits;
  UINT32 Data;
} HSSI_CTRL_N1_FPGA_FME_STRUCT;


/* HSSI_STAT_N0_FPGA_FME_REG supported on:                                    */
/*       SKXP (0x00000090)                                                    */
/* Register default value:              0x00000000                            */
#define HSSI_STAT_N0_FPGA_FME_REG 0x00000090
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * HSSI Status
 */
typedef union {
  struct {
    UINT32 data : 32;
    /* data - Bits[31:0], RO, default = 32'h0 
       Data
     */
  } Bits;
  UINT32 Data;
} HSSI_STAT_N0_FPGA_FME_STRUCT;


/* HSSI_STAT_N1_FPGA_FME_REG supported on:                                    */
/*       SKXP (0x00000094)                                                    */
/* Register default value:              0x00000000                            */
#define HSSI_STAT_N1_FPGA_FME_REG 0x00000094
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * HSSI Status
 */
typedef union {
  struct {
    UINT32 acknowledge : 1;
    /* acknowledge - Bits[0:0], RO, default = 1'h0 
       Acknowledge
     */
    UINT32 spare : 1;
    /* spare - Bits[1:1], RO, default = 1'h0 
       Spare
     */
    UINT32 reserved34 : 30;
    /* reserved34 - Bits[31:2], RsvdZ, default = 30'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} HSSI_STAT_N1_FPGA_FME_STRUCT;


/* IOMMU_CTRL_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00000098)                                                    */
/* Register default value:              0x00000000                            */
#define IOMMU_CTRL_N0_FPGA_FME_REG 0x00000098
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * IOMMU Control
 */
typedef union {
  struct {
    UINT32 prefetchdisablec0 : 1;
    /* prefetchdisablec0 - Bits[0:0], RW, default = 1'h0 
       Disables the IOMMU prefetcher for the C0 channel.
     */
    UINT32 prefetchdisablec1 : 1;
    /* prefetchdisablec1 - Bits[1:1], RW, default = 1'h0 
       Disables the IOMMU prefetcher for the C1 channel.
     */
    UINT32 partialwrdisable : 1;
    /* partialwrdisable - Bits[2:2], RW, default = 1'h0 
       Disables IOMMU partial cache line writes. When disabled the IOMMU status memory 
       writes will be full cache lines. 
     */
    UINT32 reserved3 : 1;
    /* reserved3 - Bits[3:3], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 countersel : 2;
    /* countersel - Bits[5:4], RW, default = 2'h0 
       2'h3: Reserved
       2'h2: Number of 1G page translation responses.
       2'h1: Number of 2M page translation responses.
       2'h0: Number of 4K page translation responses.
       See 'DebugCounters' field in IOMMU_STAT register for counter read values
     */
    UINT32 reserved6 : 26;
    /* reserved6 - Bits[31:6], RsvdZ, default = 26'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} IOMMU_CTRL_N0_FPGA_FME_STRUCT;


/* IOMMU_CTRL_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x0000009C)                                                    */
/* Register default value:              0x00000000                            */
#define IOMMU_CTRL_N1_FPGA_FME_REG 0x0000009C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * IOMMU Control
 */
typedef union {
  struct {
    UINT32 capecapdefeature : 32;
    /* capecapdefeature - Bits[31:0], RW, default = 32'h0 
       Connected to the IOMMU SIP Capabilities/Extended Capabilities Register. See 
       IOMMU SIP Integration Guide for detailed description of this register. 
     */
  } Bits;
  UINT32 Data;
} IOMMU_CTRL_N1_FPGA_FME_STRUCT;


/* IOMMU_STAT_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x000000A0)                                                    */
/* Register default value:              0x00000000                            */
#define IOMMU_STAT_N0_FPGA_FME_REG 0x000000A0
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * IOMMU Status
 */
typedef union {
  struct {
    UINT32 te : 1;
    /* te - Bits[0:0], RO, default = 1'h0 
       Translation Enable bit from IOMMU SIP
     */
    UINT32 drainreqinprog : 1;
    /* drainreqinprog - Bits[1:1], RO, default = 1'h0 
       Drain request in progress
     */
    UINT32 invstate : 3;
    /* invstate - Bits[4:2], RO, default = 3'h0 
       Invalidation current state
     */
    UINT32 respbufferstatec0 : 3;
    /* respbufferstatec0 - Bits[7:5], RO, default = 3'h0 
       C0 Response Buffer current state
     */
    UINT32 respbufferstatec1 : 3;
    /* respbufferstatec1 - Bits[10:8], RO, default = 3'h0 
       C1 Response Buffer current state
     */
    UINT32 lastreqid : 4;
    /* lastreqid - Bits[14:11], RO, default = 4'h0 
       Last request ID to IOMMU SIP
     */
    UINT32 lastrespid : 4;
    /* lastrespid - Bits[18:15], RO, default = 4'h0 
       Last IOMMU SIP response ID value
     */
    UINT32 lastrespstatus : 3;
    /* lastrespstatus - Bits[21:19], RO, default = 3'h0 
       Last IOMMU SIP response status value
     */
    UINT32 transactionbuffernotemptyc0 : 1;
    /* transactionbuffernotemptyc0 - Bits[22:22], RO, default = 1'h0 
       C0 Transaction Buffer is not empty
     */
    UINT32 transactionbuffernotemptyc1 : 1;
    /* transactionbuffernotemptyc1 - Bits[23:23], RO, default = 1'h0 
       C1 Transaction Buffer is not empty
     */
    UINT32 requestfifonotemptyc0 : 1;
    /* requestfifonotemptyc0 - Bits[24:24], RO, default = 1'h0 
       C0 Request FIFO is not empty
     */
    UINT32 requestfifonotemptyc1 : 1;
    /* requestfifonotemptyc1 - Bits[25:25], RO, default = 1'h0 
       C1 Request FIFO is not empty
     */
    UINT32 respfifonotemptyc0 : 1;
    /* respfifonotemptyc0 - Bits[26:26], RO, default = 1'h0 
       C0 Response FIFO is not empty
     */
    UINT32 respfifonotemptyc1 : 1;
    /* respfifonotemptyc1 - Bits[27:27], RO, default = 1'h0 
       C1 Response FIFO is not empty
     */
    UINT32 respfifooverflowc0 : 1;
    /* respfifooverflowc0 - Bits[28:28], RO, default = 1'h0 
       C0 Response FIFO overflow detected
     */
    UINT32 respfifooverflowc1 : 1;
    /* respfifooverflowc1 - Bits[29:29], RO, default = 1'h0 
       C1 Response FIFO overflow detected
     */
    UINT32 transactionbufferoverflowc0 : 1;
    /* transactionbufferoverflowc0 - Bits[30:30], RO, default = 1'h0 
       C0 Transaction Buffer overflow detected
     */
    UINT32 transactionbufferoverflowc1 : 1;
    /* transactionbufferoverflowc1 - Bits[31:31], RO, default = 1'h0 
       C1 Transaction Buffer overflow detected
     */
  } Bits;
  UINT32 Data;
} IOMMU_STAT_N0_FPGA_FME_STRUCT;


/* IOMMU_STAT_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x000000A4)                                                    */
/* Register default value:              0x00000000                            */
#define IOMMU_STAT_N1_FPGA_FME_REG 0x000000A4
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * IOMMU Status
 */
typedef union {
  struct {
    UINT32 requestfifooverflow : 1;
    /* requestfifooverflow - Bits[0:0], RO, default = 1'h0 
       Request FIFO overflow detected
     */
    UINT32 memwrinprog : 1;
    /* memwrinprog - Bits[1:1], RO, default = 1'h0 
       IOMMU memory write in progress. Indicates an IOMMU generated interrupt or IOMMU 
       status write is in progress, but no response has been received. 
     */
    UINT32 memrdinprog : 1;
    /* memrdinprog - Bits[2:2], RO, default = 1'h0 
       IOMMU memory read in progress. Indicates read has been requested, but no 
       completion has been detected. 
     */
    UINT32 reserved35 : 1;
    /* reserved35 - Bits[3:3], RO, default = 1'h0 
       Reserved
     */
    UINT32 debugcounters : 16;
    /* debugcounters - Bits[19:4], RO, default = 16'h0 
       Value of counter selected by IOMMU_CTRL[5:4]
     */
    UINT32 reserved52 : 12;
    /* reserved52 - Bits[31:20], RO, default = 12'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} IOMMU_STAT_N1_FPGA_FME_STRUCT;


/* THERM_MNGM_DFH_N0_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00001000)                                                    */
/* Register default value:              0x10000001                            */
#define THERM_MNGM_DFH_N0_FPGA_FME_REG 0x00001000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Thermal Management DFH
 */
typedef union {
  struct {
    UINT32 featureid : 12;
    /* featureid - Bits[11:0], RO, default = 12'h1 
       Feature Id
     */
    UINT32 featurerev : 4;
    /* featurerev - Bits[15:12], RO, default = 4'h0 
       Feature Revision
     */
    UINT32 nextdfhbyteoffset : 16;
    /* nextdfhbyteoffset - Bits[31:16], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset
     */
  } Bits;
  UINT32 Data;
} THERM_MNGM_DFH_N0_FPGA_FME_STRUCT;


/* THERM_MNGM_DFH_N1_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00001004)                                                    */
/* Register default value:              0x600001000                           */
#define THERM_MNGM_DFH_N1_FPGA_FME_REG 0x00001004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Thermal Management DFH
 */
typedef union {
  struct {
    UINT32 nextdfhbyteoffset : 8;
    /* nextdfhbyteoffset - Bits[7:0], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset
     */
    UINT32 reserved40 : 20;
    /* reserved40 - Bits[27:8], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 featuretype : 4;
    /* featuretype - Bits[31:28], RO, default = 4'h3 
       Feature Type = Private Feature
     */
  } Bits;
  UINT32 Data;
} THERM_MNGM_DFH_N1_FPGA_FME_STRUCT;


/* TMP_THRESHOLD_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00001008)                                                    */
/* Register default value:              0x645F5F5A                            */
#define TMP_THRESHOLD_N0_FPGA_FME_REG 0x00001008
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Temperature Threshold
 */
typedef union {
  struct {
    UINT32 tempthreshold1 : 7;
    /* tempthreshold1 - Bits[6:0], RW-L, default = 7'h5a 
       7-bit unsigned integer.
       Max value clamped at 100 C; i.e. if the SW tries to write value greater than 100 
       C, HW will automatically default to 100 C. Set and locked by BIOS 
     */
    UINT32 tempthreshold1enab : 1;
    /* tempthreshold1enab - Bits[7:7], RW-L, default = 1'h0 
       Temperature Threshold1 enable/disable
     */
    UINT32 tempthreshold2 : 7;
    /* tempthreshold2 - Bits[14:8], RW-L, default = 7'h5F 
       7-bit unsigned integer.
       Max value clamped at 100 C; i.e. if the SW tries to write value greater than 100 
       C, HW will automatically default to 100 C. Set and locked by BIOS 
     */
    UINT32 tempthreshold2enab : 1;
    /* tempthreshold2enab - Bits[15:15], RW-L, default = 1'h0 
       Temperature Threshold2 enable/disable
     */
    UINT32 prochotsetpoint : 7;
    /* prochotsetpoint - Bits[22:16], RsvdZ, default = 7'h5F 
       Defeatured
       Proc Hot set point: Max value clamped at Therm Trip threshold; if the SW tries 
       to write a larger value, HW will automatically clamp it. 
     */
    UINT32 reserved23 : 1;
    /* reserved23 - Bits[23:23], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 thermtripthreshold : 7;
    /* thermtripthreshold - Bits[30:24], RO, default = 7'h64 
       7-bit unsigned integer.
       Default value = 95C
       Defined in Blue bitstream. Cannot be modified by BIOS.
     */
    UINT32 reserved31 : 1;
    /* reserved31 - Bits[31:31], RsvdZ, default = 1'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} TMP_THRESHOLD_N0_FPGA_FME_STRUCT;


/* TMP_THRESHOLD_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x0000100C)                                                    */
/* Register default value:              0x00000000                            */
#define TMP_THRESHOLD_N1_FPGA_FME_REG 0x0000100C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Temperature Threshold
 */
typedef union {
  struct {
    UINT32 threshold1status : 1;
    /* threshold1status - Bits[0:0], RO, default = 1'h0 
       Set when temperature reaches Threshold1. Will trigger AP1/AP2 selected by 
       Threshold1 policy configuration. 
     */
    UINT32 threshold2status : 1;
    /* threshold2status - Bits[1:1], RO, default = 1'h0 
       Set when temperature reaches Threshold2. Will trigger AP6 state.
     */
    UINT32 reserved34 : 1;
    /* reserved34 - Bits[2:2], RsvdP, default = 1'h0 
       Defeatured
       Set when temperature reaches Proc Hot set point.
       SKX - FPGA will not drive Proc Hot. CPU will instead drive Proc Hot. Package 
       changes to make Proc Hot an input to FPGA are under review. 
       BDX - should follow the SKX flow. FPGA should not drive Proc Hot.
     */
    UINT32 thermtripstatus : 1;
    /* thermtripstatus - Bits[3:3], RO, default = 1'h0 
       Set when temperature reaches Therm trip threshold. Hardware will drive the Therm 
       trip signal which reports a thermal event to the BMC, and may cause system 
       shutdown. This is a fatal event that will cause a platform shutdown. 
     */
    UINT32 reserved36 : 4;
    /* reserved36 - Bits[7:4], RsvdZ, default = 4'h0 
       Reserved
     */
    UINT32 valmodeforce : 1;
    /* valmodeforce - Bits[8:8], RsvdP, default = 1'h0 
       Defeatured
       Validation mode- Force Proc Hot.
       Locked by BIOS.
     */
    UINT32 valmodetherm : 1;
    /* valmodetherm - Bits[9:9], RW-L, default = 1'h0 
       Validation mode - Therm trip Hot.
       Locked by BIOS.
     */
    UINT32 reserved42 : 2;
    /* reserved42 - Bits[11:10], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 thresholdpolicy : 1;
    /* thresholdpolicy - Bits[12:12], RW-L, default = 1'h0 
       Threshold 1 policy
       0 - AP2 state (90% throttle)
       1 - AP1 state (50% throttle)
     */
    UINT32 reserved45 : 19;
    /* reserved45 - Bits[31:13], RsvdZ, default = 19'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} TMP_THRESHOLD_N1_FPGA_FME_STRUCT;


/* TMP_RDSENSOR_FMT1_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00001010)                                                    */
/* Register default value:              0x00000000                            */
#define TMP_RDSENSOR_FMT1_N0_FPGA_FME_REG 0x00001010
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Thermal Sensor FMT1
 */
typedef union {
  struct {
    UINT32 fpgatemp : 7;
    /* fpgatemp - Bits[6:0], RO, default = 7'h0 
       Temperature Reading. Reads out FPGA temperature in Celsius.
     */
    UINT32 reserved7 : 1;
    /* reserved7 - Bits[7:7], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 numbtempreads : 16;
    /* numbtempreads - Bits[23:8], RO, default = 16'h0 
       Temperature reading sequence numbers. This sequentially increments on each 
       reading from the FPGAs thermal sensor. During normal operation, you should see 
       this value increment. If it stops incrementing, it indicates a failure. 
     */
    UINT32 tempvalid : 1;
    /* tempvalid - Bits[24:24], RO, default = 1'h0 
       When set it indicates that temperature reading is valid. There is no way for SW 
       to disable or clear the temperature readings. 
     */
    UINT32 reserved25 : 7;
    /* reserved25 - Bits[31:25], RsvdZ, default = 7'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} TMP_RDSENSOR_FMT1_N0_FPGA_FME_STRUCT;


/* TMP_RDSENSOR_FMT1_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00001014)                                                    */
/* Register default value:              0x00000000                            */
#define TMP_RDSENSOR_FMT1_N1_FPGA_FME_REG 0x00001014
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Thermal Sensor FMT1
 */
typedef union {
  struct {
    UINT32 tempthermalsensor : 10;
    /* tempthermalsensor - Bits[9:0], RO, default = 10'h0 
       Debug mode
       Reads out raw temperature code from thermal sensor. This is provided for debug 
       only. SW should use the temperature provided in bits[6:0]. 
     */
    UINT32 reserved42 : 22;
    /* reserved42 - Bits[31:10], RsvdZ, default = 22'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} TMP_RDSENSOR_FMT1_N1_FPGA_FME_STRUCT;


/* TMP_RDSENSOR_FMT2_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00001018)                                                    */
/* Register default value:              0x00000000                            */
#define TMP_RDSENSOR_FMT2_N0_FPGA_FME_REG 0x00001018
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Thermal Sensor FMT2
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       TBD
     */
  } Bits;
  UINT32 Data;
} TMP_RDSENSOR_FMT2_N0_FPGA_FME_STRUCT;


/* TMP_RDSENSOR_FMT2_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x0000101C)                                                    */
/* Register default value:              0x00000000                            */
#define TMP_RDSENSOR_FMT2_N1_FPGA_FME_REG 0x0000101C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Thermal Sensor FMT2
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       TBD
     */
  } Bits;
  UINT32 Data;
} TMP_RDSENSOR_FMT2_N1_FPGA_FME_STRUCT;


/* PWR_MNGM_DFH_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00002000)                                                    */
/* Register default value:              0x10000002                            */
#define PWR_MNGM_DFH_N0_FPGA_FME_REG 0x00002000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Power Management DFH
 */
typedef union {
  struct {
    UINT32 featureid : 12;
    /* featureid - Bits[11:0], RO, default = 12'h2 
       Feature Id
     */
    UINT32 featurerev : 4;
    /* featurerev - Bits[15:12], RO, default = 4'h0 
       Feature Revision
     */
    UINT32 nextdfhbyteoffset : 16;
    /* nextdfhbyteoffset - Bits[31:16], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset
     */
  } Bits;
  UINT32 Data;
} PWR_MNGM_DFH_N0_FPGA_FME_STRUCT;


/* PWR_MNGM_DFH_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00002004)                                                    */
/* Register default value:              0x600001000                           */
#define PWR_MNGM_DFH_N1_FPGA_FME_REG 0x00002004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Power Management DFH
 */
typedef union {
  struct {
    UINT32 nextdfhbyteoffset : 8;
    /* nextdfhbyteoffset - Bits[7:0], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset
     */
    UINT32 reserved40 : 20;
    /* reserved40 - Bits[27:8], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 featuretype : 4;
    /* featuretype - Bits[31:28], RO, default = 4'h3 
       Feature Type = Private Feature
     */
  } Bits;
  UINT32 Data;
} PWR_MNGM_DFH_N1_FPGA_FME_STRUCT;


/* PM_STATUS_N0_FPGA_FME_REG supported on:                                    */
/*       SKXP (0x00002008)                                                    */
/* Register default value:              0x00040000                            */
#define PM_STATUS_N0_FPGA_FME_REG 0x00002008
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Power Status register
 */
typedef union {
  struct {
    UINT32 pwrconsumed : 18;
    /* pwrconsumed - Bits[17:0], RO, default = 18'h0 
       PCU will write the FPGA power consumption value
     */
    UINT32 latencytolerance : 1;
    /* latencytolerance - Bits[18:18], RO, default = 1'h1 
       FPGA Latency Tolerance Reporting (LTR)
       This is the time taken for the CPU to resume from Package C6 state.
       This affect the memory access latency seen by AFU.
       This a read only bit that reflects the state of the OVERCLK[1] pin.
       FIU should take a Logical AND of all AFU LTR bits defined in
       PORT_CONTROL register.
     */
    UINT32 reserved19 : 13;
    /* reserved19 - Bits[31:19], RsvdZ, default = 45'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PM_STATUS_N0_FPGA_FME_STRUCT;


/* PM_STATUS_N1_FPGA_FME_REG supported on:                                    */
/*       SKXP (0x0000200C)                                                    */
/* Register default value:              0x00000000                            */
#define PM_STATUS_N1_FPGA_FME_REG 0x0000200C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Power Status register
 */
typedef union {
  struct {
    UINT32 reserved19 : 32;
    /* reserved19 - Bits[31:0], RsvdZ, default = 45'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PM_STATUS_N1_FPGA_FME_STRUCT;


/* PM_THRESHOLD_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00002010)                                                    */
/* Register default value:              0x00007F7F                            */
#define PM_THRESHOLD_N0_FPGA_FME_REG 0x00002010
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Power Thresholds
 */
typedef union {
  struct {
    UINT32 threshold1 : 7;
    /* threshold1 - Bits[6:0], RW, default = 7'h7F 
       Power Threshold 1 in Watts
     */
    UINT32 reserved7 : 1;
    /* reserved7 - Bits[7:7], RW, default = 1'h0 
       Reserved
     */
    UINT32 threshold2 : 7;
    /* threshold2 - Bits[14:8], RW, default = 7'h7F 
       Power Threshold 2 in Watts
     */
    UINT32 reserved15 : 1;
    /* reserved15 - Bits[15:15], RW, default = 1'h0 
       Reserved
     */
    UINT32 threshold1status : 1;
    /* threshold1status - Bits[16:16], RO, default = 1'h0 
       Indicates if the power exceeds Threshold 1. Will trigger AP1 state
     */
    UINT32 threshold2status : 1;
    /* threshold2status - Bits[17:17], RO, default = 1'h0 
       Indicates if the power exceeds Threshold 2. Will trigger AP2 state
     */
    UINT32 reserved18 : 14;
    /* reserved18 - Bits[31:18], RsvdZ, default = 46'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PM_THRESHOLD_N0_FPGA_FME_STRUCT;


/* PM_THRESHOLD_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00002014)                                                    */
/* Register default value:              0x00000000                            */
#define PM_THRESHOLD_N1_FPGA_FME_REG 0x00002014
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Power Thresholds
 */
typedef union {
  struct {
    UINT32 reserved18 : 32;
    /* reserved18 - Bits[31:0], RsvdZ, default = 46'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PM_THRESHOLD_N1_FPGA_FME_STRUCT;


/* GLBL_PERF_DFH_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003000)                                                    */
/* Register default value:              0x10000003                            */
#define GLBL_PERF_DFH_N0_FPGA_FME_REG 0x00003000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Global Performance DFH
 */
typedef union {
  struct {
    UINT32 featureid : 12;
    /* featureid - Bits[11:0], RO, default = 12'h3 
       Feature Id
     */
    UINT32 featurerev : 4;
    /* featurerev - Bits[15:12], RO, default = 4'h0 
       Feature Revision
     */
    UINT32 nextdfhbyteoffset : 16;
    /* nextdfhbyteoffset - Bits[31:16], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset
     */
  } Bits;
  UINT32 Data;
} GLBL_PERF_DFH_N0_FPGA_FME_STRUCT;


/* GLBL_PERF_DFH_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003004)                                                    */
/* Register default value:              0x600001000                           */
#define GLBL_PERF_DFH_N1_FPGA_FME_REG 0x00003004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * Global Performance DFH
 */
typedef union {
  struct {
    UINT32 nextdfhbyteoffset : 8;
    /* nextdfhbyteoffset - Bits[7:0], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset
     */
    UINT32 reserved40 : 20;
    /* reserved40 - Bits[27:8], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 featuretype : 4;
    /* featuretype - Bits[31:28], RO, default = 4'h3 
       Feature Type = Private Feature
     */
  } Bits;
  UINT32 Data;
} GLBL_PERF_DFH_N1_FPGA_FME_STRUCT;


/* FPMON_CACHE_CTL_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00003008)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CACHE_CTL_N0_FPGA_FME_REG 0x00003008
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Cache Control
 */
typedef union {
  struct {
    UINT32 resetcounters : 1;
    /* resetcounters - Bits[0:0], RsvdZ, default = 1'h0 
       Defeatured. Reset Counters. When set to 1, the counter values will be set to 0
     */
    UINT32 reserved1 : 7;
    /* reserved1 - Bits[7:1], RsvdZ, default = 7'h0 
       Reserved
     */
    UINT32 freezecounters : 1;
    /* freezecounters - Bits[8:8], RW, default = 1'h0 
       Freeze if set to 1 then the counters will be frozen.
     */
    UINT32 reserved9 : 7;
    /* reserved9 - Bits[15:9], RsvdZ, default = 7'h0 
       Reserved
     */
    UINT32 cacheeventcode : 4;
    /* cacheeventcode - Bits[19:16], RW, default = 4'h0 
       Select the cache event to be read out on both the cache counter registers.
       0x0 - Cache read hit
       0x1 - Cache write hit
       0x2 - Cache read miss
       0x3 - Cache write miss
       0x4 - Reserved
       0x5 - Hold requests
       0x6 - Cache Data write port contention
       0x7 - Cache Tag write port contention
       0x8 - Tx request stall
       0x9 - Rx stalls
       0xA - Cache evictions
     */
    UINT32 ccichsel : 1;
    /* ccichsel - Bits[20:20], RW, default = 1'h0 
       0 - RD channel
       1 - WR channel
     */
    UINT32 reserved21 : 11;
    /* reserved21 - Bits[31:21], RsvdZ, default = 43'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FPMON_CACHE_CTL_N0_FPGA_FME_STRUCT;


/* FPMON_CACHE_CTL_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x0000300C)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CACHE_CTL_N1_FPGA_FME_REG 0x0000300C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Cache Control
 */
typedef union {
  struct {
    UINT32 reserved21 : 32;
    /* reserved21 - Bits[31:0], RsvdZ, default = 43'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FPMON_CACHE_CTL_N1_FPGA_FME_STRUCT;


/* FPMON_CACHE_CTR0_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00003010)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CACHE_CTR0_N0_FPGA_FME_REG 0x00003010
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Cache Counter 0
 */
typedef union {
  struct {
    UINT32 cachecountereven : 32;
    /* cachecountereven - Bits[31:0], RO, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Cache Counter for even addresses. Reads out the cache event selected by 
       FPMON_CACHE_CTL. 
     */
  } Bits;
  UINT32 Data;
} FPMON_CACHE_CTR0_N0_FPGA_FME_STRUCT;


/* FPMON_CACHE_CTR0_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00003014)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CACHE_CTR0_N1_FPGA_FME_REG 0x00003014
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Cache Counter 0
 */
typedef union {
  struct {
    UINT32 cachecountereven : 16;
    /* cachecountereven - Bits[15:0], RO, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Cache Counter for even addresses. Reads out the cache event selected by 
       FPMON_CACHE_CTL. 
     */
    UINT32 reserved48 : 12;
    /* reserved48 - Bits[27:16], RsvdZ, default = 12'h0 
       Reserved
     */
    UINT32 cacheeventcode : 4;
    /* cacheeventcode - Bits[31:28], RO, default = 4'h0 
       Cache Event being reported
     */
  } Bits;
  UINT32 Data;
} FPMON_CACHE_CTR0_N1_FPGA_FME_STRUCT;


/* FPMON_CACHE_CTR1_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00003018)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CACHE_CTR1_N0_FPGA_FME_REG 0x00003018
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Cache Counter 1
 */
typedef union {
  struct {
    UINT32 cachecounterodd : 32;
    /* cachecounterodd - Bits[31:0], RO, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Cache Counter for odd addresses. Reads out the cache event selected by 
       FPMON_CACHE_CTL. For a CCI SINGLE_CHANNEL configuration, this register is 
       deprecated. 
     */
  } Bits;
  UINT32 Data;
} FPMON_CACHE_CTR1_N0_FPGA_FME_STRUCT;


/* FPMON_CACHE_CTR1_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x0000301C)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CACHE_CTR1_N1_FPGA_FME_REG 0x0000301C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Cache Counter 1
 */
typedef union {
  struct {
    UINT32 cachecounterodd : 16;
    /* cachecounterodd - Bits[15:0], RO, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Cache Counter for odd addresses. Reads out the cache event selected by 
       FPMON_CACHE_CTL. For a CCI SINGLE_CHANNEL configuration, this register is 
       deprecated. 
     */
    UINT32 reserved48 : 12;
    /* reserved48 - Bits[27:16], RsvdZ, default = 12'h0 
       Reserved
     */
    UINT32 cacheeventcode : 4;
    /* cacheeventcode - Bits[31:28], RO, default = 4'h0 
       Cache Event being reported
     */
  } Bits;
  UINT32 Data;
} FPMON_CACHE_CTR1_N1_FPGA_FME_STRUCT;


/* FPMON_FAB_CTL_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003020)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_FAB_CTL_N0_FPGA_FME_REG 0x00003020
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON FAB Control
 */
typedef union {
  struct {
    UINT32 resetcounters : 1;
    /* resetcounters - Bits[0:0], RsvdZ, default = 1'h0 
       Defeatured. Reset Counters. When set to 1, the counter values will be set to 0
     */
    UINT32 reserved1 : 7;
    /* reserved1 - Bits[7:1], RsvdZ, default = 7'h0 
       Reserved
     */
    UINT32 freezecounters : 1;
    /* freezecounters - Bits[8:8], RW, default = 1'h0 
       Freeze. If set to 1 then the counters will be frozen
     */
    UINT32 reserved9 : 7;
    /* reserved9 - Bits[15:9], RsvdZ, default = 7'h0 
       Reserved
     */
    UINT32 fabriceventcode : 4;
    /* fabriceventcode - Bits[19:16], RW, default = 4'h0 
       Fabric Event Code:
       0x0 - PCIe0 reads 
       0x1 - PCIe0 writes
       0x2 - PCIe1 reads 
       0x3 - PCIe1 writes
       0x4 - UPI reads   
       0x5 - UPI writes  
       0x6 - MMIO reads  
       0x7 - MMIO writes 
     */
    UINT32 cciportid : 2;
    /* cciportid - Bits[21:20], RW, default = 2'h0 
       CCI-P Port Id. When filtering enabled, it counts only those requests originating 
       from Port Id specified in this field. 
       0x0 - Port 0
       0x1 - Port 1
       0x2 - Reserved
       0x3 - Reserved
     */
    UINT32 reserved22 : 1;
    /* reserved22 - Bits[22:22], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 cciportfilter : 1;
    /* cciportfilter - Bits[23:23], RW, default = 1'h0 
       CCI-P Port Filter Enable/Disable When 1 it enables port Id filtering.
     */
    UINT32 reserved24 : 8;
    /* reserved24 - Bits[31:24], RsvdZ, default = 40'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FPMON_FAB_CTL_N0_FPGA_FME_STRUCT;


/* FPMON_FAB_CTL_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003024)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_FAB_CTL_N1_FPGA_FME_REG 0x00003024
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON FAB Control
 */
typedef union {
  struct {
    UINT32 reserved24 : 32;
    /* reserved24 - Bits[31:0], RsvdZ, default = 40'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FPMON_FAB_CTL_N1_FPGA_FME_STRUCT;


/* FPMON_FAB_CTR_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003028)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_FAB_CTR_N0_FPGA_FME_REG 0x00003028
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Event Counter
 */
typedef union {
  struct {
    UINT32 fabriceventcounter : 32;
    /* fabriceventcounter - Bits[31:0], RO, default = 60'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Fabric event counter
     */
  } Bits;
  UINT32 Data;
} FPMON_FAB_CTR_N0_FPGA_FME_STRUCT;


/* FPMON_FAB_CTR_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x0000302C)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_FAB_CTR_N1_FPGA_FME_REG 0x0000302C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Event Counter
 */
typedef union {
  struct {
    UINT32 fabriceventcounter : 28;
    /* fabriceventcounter - Bits[27:0], RO, default = 60'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Fabric event counter
     */
    UINT32 fabriceventcode : 4;
    /* fabriceventcode - Bits[31:28], RO, default = 4'h0 
       Fabric event code being reported
     */
  } Bits;
  UINT32 Data;
} FPMON_FAB_CTR_N1_FPGA_FME_STRUCT;


/* FPMON_CLK_CTR_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003030)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CLK_CTR_N0_FPGA_FME_REG 0x00003030
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Clock Counter
 */
typedef union {
  struct {
    UINT32 clockcounter : 32;
    /* clockcounter - Bits[31:0], RO, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Clk_16UI (AFU interface clock) counter. This is a free running counter, that 
       cannot be frozen or reset by SW. 
     */
  } Bits;
  UINT32 Data;
} FPMON_CLK_CTR_N0_FPGA_FME_STRUCT;


/* FPMON_CLK_CTR_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003034)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_CLK_CTR_N1_FPGA_FME_REG 0x00003034
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON Clock Counter
 */
typedef union {
  struct {
    UINT32 clockcounter : 32;
    /* clockcounter - Bits[31:0], RO, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Clk_16UI (AFU interface clock) counter. This is a free running counter, that 
       cannot be frozen or reset by SW. 
     */
  } Bits;
  UINT32 Data;
} FPMON_CLK_CTR_N1_FPGA_FME_STRUCT;


/* FPMON_VTD_CTL_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003038)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_VTD_CTL_N0_FPGA_FME_REG 0x00003038
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * VTd event control
 */
typedef union {
  struct {
    UINT32 resetcounters : 1;
    /* resetcounters - Bits[0:0], RW, default = 1'h0 
       When set to 1, the counter values will be set to 0
     */
    UINT32 reserved1 : 7;
    /* reserved1 - Bits[7:1], RsvdZ, default = 7'h0 
       Reserved
     */
    UINT32 freezecounters : 1;
    /* freezecounters - Bits[8:8], RW, default = 1'h0 
       When set to 1, the counter values will be frozen
     */
    UINT32 reserved9 : 7;
    /* reserved9 - Bits[15:9], RsvdZ, default = 7'h0 
       Reserved
     */
    UINT32 vtdtlbeventcode : 4;
    /* vtdtlbeventcode - Bits[19:16], RW, default = 4'h0 
       VTd and TLB event code:
       0x0 C0 AFU0 Memory Read Transaction Count
       0x1 C0 AFU1 Memory Read Transaction Count
       0x2 C1 AFU0 Memory Write Transaction Count
       0x3 C1 AFU1 Memory Write Transaction Count
       0x4 C0 AFU0 DevTLB Hit Count
       0x5 C0 AFU1 DevTLB Hit Count
       0x6 C1 AFU0 DevTLB Hit Count
       0x7 C1 AFU1 DevTLB Hit Count
     */
    UINT32 reserved20 : 12;
    /* reserved20 - Bits[31:20], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FPMON_VTD_CTL_N0_FPGA_FME_STRUCT;


/* FPMON_VTD_CTL_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x0000303C)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_VTD_CTL_N1_FPGA_FME_REG 0x0000303C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * VTd event control
 */
typedef union {
  struct {
    UINT32 reserved20 : 32;
    /* reserved20 - Bits[31:0], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FPMON_VTD_CTL_N1_FPGA_FME_STRUCT;


/* FPMON_VTD_CTR_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003040)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_VTD_CTR_N0_FPGA_FME_REG 0x00003040
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * VTd event counter
 */
typedef union {
  struct {
    UINT32 vtdeventcounter : 32;
    /* vtdeventcounter - Bits[31:0], RO, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       VTd event counter
     */
  } Bits;
  UINT32 Data;
} FPMON_VTD_CTR_N0_FPGA_FME_STRUCT;


/* FPMON_VTD_CTR_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00003044)                                                    */
/* Register default value:              0x00000000                            */
#define FPMON_VTD_CTR_N1_FPGA_FME_REG 0x00003044
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * VTd event counter
 */
typedef union {
  struct {
    UINT32 vtdeventcounter : 16;
    /* vtdeventcounter - Bits[15:0], RO, default = 48'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       VTd event counter
     */
    UINT32 reserved48 : 12;
    /* reserved48 - Bits[27:16], RsvdZ, default = 12'h0 
       Reserved
     */
    UINT32 vtdeventcode : 4;
    /* vtdeventcode - Bits[31:28], RO, default = 4'h0 
       VTd Event being reported
     */
  } Bits;
  UINT32 Data;
} FPMON_VTD_CTR_N1_FPGA_FME_STRUCT;


/* GLBL_ERR_DFH_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00004000)                                                    */
/* Register default value:              0x10001004                            */
#define GLBL_ERR_DFH_N0_FPGA_FME_REG 0x00004000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON FAB Control
 */
typedef union {
  struct {
    UINT32 featureid : 12;
    /* featureid - Bits[11:0], RO, default = 12'h4 
       Feature Id
     */
    UINT32 featurerevision : 4;
    /* featurerevision - Bits[15:12], RO, default = 4'h1 
       Feature Revision
     */
    UINT32 nextdfhbyteoffset : 16;
    /* nextdfhbyteoffset - Bits[31:16], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset.
     */
  } Bits;
  UINT32 Data;
} GLBL_ERR_DFH_N0_FPGA_FME_STRUCT;


/* GLBL_ERR_DFH_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00004004)                                                    */
/* Register default value:              0x600001000                           */
#define GLBL_ERR_DFH_N1_FPGA_FME_REG 0x00004004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FPMON FAB Control
 */
typedef union {
  struct {
    UINT32 nextdfhbyteoffset : 8;
    /* nextdfhbyteoffset - Bits[7:0], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset.
     */
    UINT32 reserved : 20;
    /* reserved - Bits[27:8], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 featuretype : 4;
    /* featuretype - Bits[31:28], RO, default = 4'h3 
       Feature Type = Private Feature
     */
  } Bits;
  UINT32 Data;
} GLBL_ERR_DFH_N1_FPGA_FME_STRUCT;


/* FME_ERROR0_MASK_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00004008)                                                    */
/* Register default value:              0x00000000                            */
#define FME_ERROR0_MASK_N0_FPGA_FME_REG 0x00004008
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Error Mask register 0
 */
typedef union {
  struct {
    UINT32 errormask : 20;
    /* errormask - Bits[19:0], RW, default = 20'h0 
       Error Mask - 0, disabled; 1, enabled
     */
    UINT32 reserved20 : 12;
    /* reserved20 - Bits[31:20], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_ERROR0_MASK_N0_FPGA_FME_STRUCT;


/* FME_ERROR0_MASK_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x0000400C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_ERROR0_MASK_N1_FPGA_FME_REG 0x0000400C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Error Mask register 0
 */
typedef union {
  struct {
    UINT32 reserved20 : 32;
    /* reserved20 - Bits[31:0], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_ERROR0_MASK_N1_FPGA_FME_STRUCT;


/* FME_ERROR0_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00004010)                                                    */
/* Register default value:              0x00000000                            */
#define FME_ERROR0_N0_FPGA_FME_REG 0x00004010
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Error Status register 0
 */
typedef union {
  struct {
    UINT32 fabricerr : 1;
    /* fabricerr - Bits[0:0], RW1C, default = 1'h0 
       Fabric error
     */
    UINT32 fabricfifouoflow : 1;
    /* fabricfifouoflow - Bits[1:1], RW1C, default = 1'h0 
       Fabric fifo under/overflow error
     */
    UINT32 kticdcparityerr : 2;
    /* kticdcparityerr - Bits[3:2], RW1C, default = 2'h0 
       KTI CDC Parity Error
     */
    UINT32 iommuparityerr : 1;
    /* iommuparityerr - Bits[4:4], RW1C, default = 1'h0 
       IOMMU Parity error
     */
    UINT32 afuaccessmodeerr : 1;
    /* afuaccessmodeerr - Bits[5:5], RW1C, default = 1'h0 
       AFU PF/VF access mismatch detected
     */
    UINT32 mbperr : 1;
    /* mbperr - Bits[6:6], RW1C, default = 1'h0 
       Indicates an MBP event
     */
    UINT32 pcie0cdcparerr : 5;
    /* pcie0cdcparerr - Bits[11:7], RW1C, default = 5'h0 
       PCIe0 CDC Parity Error
     */
    UINT32 pcie1cdcparerr : 5;
    /* pcie1cdcparerr - Bits[16:12], RW1C, default = 5'h0 
       PCIe1 CDC Parity Error
     */
    UINT32 cvlcdcparerr0 : 3;
    /* cvlcdcparerr0 - Bits[19:17], RW1C, default = 3'h0 
       CVL CDC Parity Error
     */
    UINT32 reserved20 : 12;
    /* reserved20 - Bits[31:20], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_ERROR0_N0_FPGA_FME_STRUCT;


/* FME_ERROR0_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00004014)                                                    */
/* Register default value:              0x00000000                            */
#define FME_ERROR0_N1_FPGA_FME_REG 0x00004014
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Error Status register 0
 */
typedef union {
  struct {
    UINT32 reserved20 : 32;
    /* reserved20 - Bits[31:0], RsvdZ, default = 44'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_ERROR0_N1_FPGA_FME_STRUCT;


/* PCIE0_ERROR_MASK_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00004018)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE0_ERROR_MASK_N0_FPGA_FME_REG 0x00004018
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe0 Error Mask register
 */
typedef union {
  struct {
    UINT32 errormask : 10;
    /* errormask - Bits[9:0], RW, default = 10'h0 
       Error Mask - 0, disabled; 1, enabled
     */
    UINT32 reserved10 : 22;
    /* reserved10 - Bits[31:10], RsvdZ, default = 54'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PCIE0_ERROR_MASK_N0_FPGA_FME_STRUCT;


/* PCIE0_ERROR_MASK_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x0000401C)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE0_ERROR_MASK_N1_FPGA_FME_REG 0x0000401C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe0 Error Mask register
 */
typedef union {
  struct {
    UINT32 reserved10 : 32;
    /* reserved10 - Bits[31:0], RsvdZ, default = 54'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PCIE0_ERROR_MASK_N1_FPGA_FME_STRUCT;


/* PCIE0_ERROR_N0_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x00004020)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE0_ERROR_N0_FPGA_FME_REG 0x00004020
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe0 Error Status register
 */
typedef union {
  struct {
    UINT32 formattypeerr : 1;
    /* formattypeerr - Bits[0:0], RW1C, default = 1'h0 
       TLP format/type error
     */
    UINT32 mwaddrerr : 1;
    /* mwaddrerr - Bits[1:1], RW1C, default = 1'h0 
       TLP MW address error
     */
    UINT32 mwlengtherr : 1;
    /* mwlengtherr - Bits[2:2], RW1C, default = 1'h0 
       TLP MW length error
     */
    UINT32 mraddrerr : 1;
    /* mraddrerr - Bits[3:3], RW1C, default = 1'h0 
       TLP MR address error
     */
    UINT32 mrlengtherr : 1;
    /* mrlengtherr - Bits[4:4], RW1C, default = 1'h0 
       TLP MR length error
     */
    UINT32 comptagerr : 1;
    /* comptagerr - Bits[5:5], RW1C, default = 1'h0 
       TLP CPL tag error
     */
    UINT32 compstaterr : 1;
    /* compstaterr - Bits[6:6], RW1C, default = 1'h0 
       TLP CPL status error
     */
    UINT32 comptimeouterr : 1;
    /* comptimeouterr - Bits[7:7], RW1C, default = 1'h0 
       TLP CPL timeout
     */
    UINT32 parityerr : 1;
    /* parityerr - Bits[8:8], RW1C, default = 1'h0 
       CCI bridge parity error
     */
    UINT32 rxpoisontlperr : 1;
    /* rxpoisontlperr - Bits[9:9], RW1C, default = 1'h0 
       Received a TLP with EP bit set
     */
    UINT32 reserved10 : 22;
    /* reserved10 - Bits[31:10], RsvdZ, default = 52'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PCIE0_ERROR_N0_FPGA_FME_STRUCT;


/* PCIE0_ERROR_N1_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x00004024)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE0_ERROR_N1_FPGA_FME_REG 0x00004024
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe0 Error Status register
 */
typedef union {
  struct {
    UINT32 reserved10 : 30;
    /* reserved10 - Bits[29:0], RsvdZ, default = 52'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
    UINT32 vfnumb : 1;
    /* vfnumb - Bits[30:30], RO, default = 1'h0 
       If FunctTypeErr = 1, this field contains the VF number that generated the error
     */
    UINT32 functtypeerr : 1;
    /* functtypeerr - Bits[31:31], RO, default = 1'h0 
       Indicates if the error was originated by a Physical (0) or Virtual (1) function 
     */
  } Bits;
  UINT32 Data;
} PCIE0_ERROR_N1_FPGA_FME_STRUCT;


/* PCIE1_ERROR_MASK_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00004028)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE1_ERROR_MASK_N0_FPGA_FME_REG 0x00004028
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe1 Error Mask register
 */
typedef union {
  struct {
    UINT32 errormask : 10;
    /* errormask - Bits[9:0], RW, default = 10'h0 
       Error Mask - 0, disabled; 1, enabled
     */
    UINT32 reserved10 : 22;
    /* reserved10 - Bits[31:10], RsvdZ, default = 54'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PCIE1_ERROR_MASK_N0_FPGA_FME_STRUCT;


/* PCIE1_ERROR_MASK_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x0000402C)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE1_ERROR_MASK_N1_FPGA_FME_REG 0x0000402C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe1 Error Mask register
 */
typedef union {
  struct {
    UINT32 reserved10 : 32;
    /* reserved10 - Bits[31:0], RsvdZ, default = 54'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PCIE1_ERROR_MASK_N1_FPGA_FME_STRUCT;


/* PCIE1_ERROR_N0_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x00004030)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE1_ERROR_N0_FPGA_FME_REG 0x00004030
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe1 Error Status register
 */
typedef union {
  struct {
    UINT32 formattypeerr : 1;
    /* formattypeerr - Bits[0:0], RW1C, default = 1'h0 
       TLP format/type error
     */
    UINT32 mwaddrerr : 1;
    /* mwaddrerr - Bits[1:1], RW1C, default = 1'h0 
       TLP MW address error
     */
    UINT32 mwlengtherr : 1;
    /* mwlengtherr - Bits[2:2], RW1C, default = 1'h0 
       TLP MW length error
     */
    UINT32 mraddrerr : 1;
    /* mraddrerr - Bits[3:3], RW1C, default = 1'h0 
       TLP MR address error
     */
    UINT32 mrlengtherr : 1;
    /* mrlengtherr - Bits[4:4], RW1C, default = 1'h0 
       TLP MR length error
     */
    UINT32 comptagerr : 1;
    /* comptagerr - Bits[5:5], RW1C, default = 1'h0 
       TLP CPL tag error
     */
    UINT32 compstaterr : 1;
    /* compstaterr - Bits[6:6], RW1C, default = 1'h0 
       TLP CPL status error
     */
    UINT32 comptimeouterr : 1;
    /* comptimeouterr - Bits[7:7], RW1C, default = 1'h0 
       TLP CPL timeout
     */
    UINT32 parityerr : 1;
    /* parityerr - Bits[8:8], RW1C, default = 1'h0 
       CCI bridge parity error
     */
    UINT32 rxpoisontlperr : 1;
    /* rxpoisontlperr - Bits[9:9], RW1C, default = 1'h0 
       Received a TLP with EP bit set
     */
    UINT32 reserved10 : 22;
    /* reserved10 - Bits[31:10], RsvdZ, default = 54'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PCIE1_ERROR_N0_FPGA_FME_STRUCT;


/* PCIE1_ERROR_N1_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x00004034)                                                    */
/* Register default value:              0x00000000                            */
#define PCIE1_ERROR_N1_FPGA_FME_REG 0x00004034
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * PCIe1 Error Status register
 */
typedef union {
  struct {
    UINT32 reserved10 : 32;
    /* reserved10 - Bits[31:0], RsvdZ, default = 54'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} PCIE1_ERROR_N1_FPGA_FME_STRUCT;


/* FME_FIRST_ERROR_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00004038)                                                    */
/* Register default value:              0x00000000                            */
#define FME_FIRST_ERROR_N0_FPGA_FME_REG 0x00004038
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME First Error register
 */
typedef union {
  struct {
    UINT32 errorregstatus : 32;
    /* errorregstatus - Bits[31:0], RO, default = 60'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Holds 60 LSBs from the Error register that was triggered first
     */
  } Bits;
  UINT32 Data;
} FME_FIRST_ERROR_N0_FPGA_FME_STRUCT;


/* FME_FIRST_ERROR_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x0000403C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_FIRST_ERROR_N1_FPGA_FME_REG 0x0000403C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME First Error register
 */
typedef union {
  struct {
    UINT32 errorregstatus : 28;
    /* errorregstatus - Bits[27:0], RO, default = 60'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Holds 60 LSBs from the Error register that was triggered first
     */
    UINT32 errorregid : 4;
    /* errorregid - Bits[31:28], RO, default = 4'h0 
       Indicates the Error Register that was triggered first
       0 - FME0 Error
       1 - PCIe0 Error
       2 - PCIe1 Error
     */
  } Bits;
  UINT32 Data;
} FME_FIRST_ERROR_N1_FPGA_FME_STRUCT;


/* FME_NEXT_ERROR_N0_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00004040)                                                    */
/* Register default value:              0x00000000                            */
#define FME_NEXT_ERROR_N0_FPGA_FME_REG 0x00004040
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Next Error register
 */
typedef union {
  struct {
    UINT32 errorregstatus : 32;
    /* errorregstatus - Bits[31:0], RO, default = 60'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Holds 60 LSBs from the Error register that was triggered second
     */
  } Bits;
  UINT32 Data;
} FME_NEXT_ERROR_N0_FPGA_FME_STRUCT;


/* FME_NEXT_ERROR_N1_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00004044)                                                    */
/* Register default value:              0x00000000                            */
#define FME_NEXT_ERROR_N1_FPGA_FME_REG 0x00004044
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Next Error register
 */
typedef union {
  struct {
    UINT32 errorregstatus : 28;
    /* errorregstatus - Bits[27:0], RO, default = 60'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Holds 60 LSBs from the Error register that was triggered second
     */
    UINT32 errorregid : 4;
    /* errorregid - Bits[31:28], RO, default = 4'h0 
       Indicates the Error Register that was triggered second
       0 - FME0 Error
       1 - PCIe0 Error
       2 - PCIe1 Error
     */
  } Bits;
  UINT32 Data;
} FME_NEXT_ERROR_N1_FPGA_FME_STRUCT;


/* RAS_NOFAT_ERR_MASK_N0_FPGA_FME_REG supported on:                           */
/*       SKXP (0x00004048)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_NOFAT_ERR_MASK_N0_FPGA_FME_REG 0x00004048
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS Non-fatal Error Mask register
 */
typedef union {
  struct {
    UINT32 errormask : 13;
    /* errormask - Bits[12:0], RW, default = 13'h0 
       Error Mask - 0, disabled; 1, enabled
     */
    UINT32 reserved13 : 19;
    /* reserved13 - Bits[31:13], RsvdZ, default = 51'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_NOFAT_ERR_MASK_N0_FPGA_FME_STRUCT;


/* RAS_NOFAT_ERR_MASK_N1_FPGA_FME_REG supported on:                           */
/*       SKXP (0x0000404C)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_NOFAT_ERR_MASK_N1_FPGA_FME_REG 0x0000404C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS Non-fatal Error Mask register
 */
typedef union {
  struct {
    UINT32 reserved13 : 32;
    /* reserved13 - Bits[31:0], RsvdZ, default = 51'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_NOFAT_ERR_MASK_N1_FPGA_FME_STRUCT;


/* RAS_NOFAT_ERR_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00004050)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_NOFAT_ERR_N0_FPGA_FME_REG 0x00004050
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS Non-fatal Error Status register
 */
typedef union {
  struct {
    UINT32 tempthreshap1 : 1;
    /* tempthreshap1 - Bits[0:0], RO, default = 1'h0 
       Temperature threshold triggered AP1
     */
    UINT32 tempthreshap2 : 1;
    /* tempthreshap2 - Bits[1:1], RO, default = 1'h0 
       Temperature threshold triggered AP2
     */
    UINT32 pcieerror : 1;
    /* pcieerror - Bits[2:2], RO, default = 1'h0 
       indicates an error has occurred in pcie.  
     */
    UINT32 portfatalerr : 1;
    /* portfatalerr - Bits[3:3], RO, default = 1'h0 
       indicates a fatal error has occurred in the the AFU port.
     */
    UINT32 prochot : 1;
    /* prochot - Bits[4:4], RO, default = 1'h0 
       indicates a ProcHot event
     */
    UINT32 afuaccessmodeerr : 1;
    /* afuaccessmodeerr - Bits[5:5], RO, default = 1'h0 
       indicates an AFU PF/VF access mismatch 
     */
    UINT32 injectedwarningerr : 1;
    /* injectedwarningerr - Bits[6:6], RO, default = 1'h0 
       Injected Warning Error
     */
    UINT32 pciepoisonerr : 1;
    /* pciepoisonerr - Bits[7:7], RO, default = 1'h0 
       indicates a Poison error from any of PCIe ports
     */
    UINT32 gbcrcerror : 1;
    /* gbcrcerror - Bits[8:8], RO, default = 1'h0 
       indicates green bitstream crc error, classified catastrophic.
       Currently not supported, will be supported in future.
     */
    UINT32 tempthreshap6 : 1;
    /* tempthreshap6 - Bits[9:9], RO, default = 1'h0 
       Temperature threshold triggered AP6
     */
    UINT32 powerthreshap1 : 1;
    /* powerthreshap1 - Bits[10:10], RO, default = 1'h0 
       Power threshold triggered AP1
     */
    UINT32 powerthreshap2 : 1;
    /* powerthreshap2 - Bits[11:11], RO, default = 1'h0 
       Power threshold triggered AP2
     */
    UINT32 mbperr : 1;
    /* mbperr - Bits[12:12], RO, default = 1'h0 
       indicates a MBP event
     */
    UINT32 reserved9 : 19;
    /* reserved9 - Bits[31:13], RsvdZ, default = 51'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_NOFAT_ERR_N0_FPGA_FME_STRUCT;


/* RAS_NOFAT_ERR_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00004054)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_NOFAT_ERR_N1_FPGA_FME_REG 0x00004054
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS Non-fatal Error Status register
 */
typedef union {
  struct {
    UINT32 reserved9 : 32;
    /* reserved9 - Bits[31:0], RsvdZ, default = 51'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_NOFAT_ERR_N1_FPGA_FME_STRUCT;


/* RAS_CATFAT_ERROR_MASK_N0_FPGA_FME_REG supported on:                        */
/*       SKXP (0x00004058)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_CATFAT_ERROR_MASK_N0_FPGA_FME_REG 0x00004058
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS BlueBS Error Mask register
 */
typedef union {
  struct {
    UINT32 errormask : 12;
    /* errormask - Bits[11:0], RW, default = 12'h0 
       Error Mask - 0, disabled; 1, enabled
     */
    UINT32 reserved12 : 20;
    /* reserved12 - Bits[31:12], RsvdZ, default = 52'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_CATFAT_ERROR_MASK_N0_FPGA_FME_STRUCT;


/* RAS_CATFAT_ERROR_MASK_N1_FPGA_FME_REG supported on:                        */
/*       SKXP (0x0000405C)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_CATFAT_ERROR_MASK_N1_FPGA_FME_REG 0x0000405C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS BlueBS Error Mask register
 */
typedef union {
  struct {
    UINT32 reserved12 : 32;
    /* reserved12 - Bits[31:0], RsvdZ, default = 52'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_CATFAT_ERROR_MASK_N1_FPGA_FME_STRUCT;


/* RAS_CATFAT_ERROR_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00004060)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_CATFAT_ERROR_N0_FPGA_FME_REG 0x00004060
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS BlueBS Error Status register
 */
typedef union {
  struct {
    UINT32 ktilinkfatalerr : 1;
    /* ktilinkfatalerr - Bits[0:0], RO, default = 1'h0 
       KTI link layer error detected.
     */
    UINT32 tagcchfatalerr : 1;
    /* tagcchfatalerr - Bits[1:1], RO, default = 1'h0 
       tag-n-cache error detected.
     */
    UINT32 ccifatalerr : 1;
    /* ccifatalerr - Bits[2:2], RO, default = 1'h0 
       CCI error detected.
     */
    UINT32 ktiprotofatalerr : 1;
    /* ktiprotofatalerr - Bits[3:3], RO, default = 1'h0 
       KTI protocol error detected.
     */
    UINT32 dramfatalerr : 1;
    /* dramfatalerr - Bits[4:4], RO, default = 1'h0 
       Fatal DRAM error detected
     */
    UINT32 iommufatalerr : 1;
    /* iommufatalerr - Bits[5:5], RO, default = 1'h0 
       IOMMU fatal parity error detected.
     */
    UINT32 fabricfatalerr : 1;
    /* fabricfatalerr - Bits[6:6], RO, default = 1'h0 
       Fabric fatal error detected
     */
    UINT32 pciepoisonerr : 1;
    /* pciepoisonerr - Bits[7:7], RO, default = 1'h0 
       indicates a Poison error from any of PCIe ports
     */
    UINT32 injectedfatalerr : 1;
    /* injectedfatalerr - Bits[8:8], RO, default = 1'h0 
       Injected Fatal Error
     */
    UINT32 crccatasterr : 1;
    /* crccatasterr - Bits[9:9], RO, default = 1'h0 
       Catastrophic CRC error detected
     */
    UINT32 thermcatasterr : 1;
    /* thermcatasterr - Bits[10:10], RO, default = 1'h0 
       Catastrophic thermal runaway event
     */
    UINT32 injectedcatasterr : 1;
    /* injectedcatasterr - Bits[11:11], RO, default = 1'h0 
       Injected Catastrophic Error
     */
    UINT32 reserved11 : 20;
    /* reserved11 - Bits[31:12], RsvdZ, default = 52'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_CATFAT_ERROR_N0_FPGA_FME_STRUCT;


/* RAS_CATFAT_ERROR_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00004064)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_CATFAT_ERROR_N1_FPGA_FME_REG 0x00004064
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS BlueBS Error Status register
 */
typedef union {
  struct {
    UINT32 reserved11 : 32;
    /* reserved11 - Bits[31:0], RsvdZ, default = 52'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_CATFAT_ERROR_N1_FPGA_FME_STRUCT;


/* RAS_ERROR_INJ_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00004068)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_ERROR_INJ_N0_FPGA_FME_REG 0x00004068
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS Error injection register
 */
typedef union {
  struct {
    UINT32 catasterror : 1;
    /* catasterror - Bits[0:0], RW, default = 1'h0 
       Set this bit to enact a Catastrophic error 
     */
    UINT32 fatalerror : 1;
    /* fatalerror - Bits[1:1], RW, default = 1'h0 
       Set this bit to enact a Fatal error 
     */
    UINT32 nofatalerror : 1;
    /* nofatalerror - Bits[2:2], RW, default = 1'h0 
       Set this bit to enact a Non-fatal error 
     */
    UINT32 reserved0 : 29;
    /* reserved0 - Bits[31:3], RsvdZ, default = 61'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_ERROR_INJ_N0_FPGA_FME_STRUCT;


/* RAS_ERROR_INJ_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x0000406C)                                                    */
/* Register default value:              0x00000000                            */
#define RAS_ERROR_INJ_N1_FPGA_FME_REG 0x0000406C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * RAS Error injection register
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 61'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} RAS_ERROR_INJ_N1_FPGA_FME_STRUCT;


/* FME_PR_DFH_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00005000)                                                    */
/* Register default value:              0x10001005                            */
#define FME_PR_DFH_N0_FPGA_FME_REG 0x00005000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration DFH
 */
typedef union {
  struct {
    UINT32 featureid : 12;
    /* featureid - Bits[11:0], RO, default = 12'h5 
       Feature Id
     */
    UINT32 featurerevision : 4;
    /* featurerevision - Bits[15:12], RO, default = 4'h1 
       Feature Revision
     */
    UINT32 nextdfhbyteoffset : 16;
    /* nextdfhbyteoffset - Bits[31:16], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset.
     */
  } Bits;
  UINT32 Data;
} FME_PR_DFH_N0_FPGA_FME_STRUCT;


/* FME_PR_DFH_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00005004)                                                    */
/* Register default value:              0x600001000                           */
#define FME_PR_DFH_N1_FPGA_FME_REG 0x00005004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration DFH
 */
typedef union {
  struct {
    UINT32 nextdfhbyteoffset : 8;
    /* nextdfhbyteoffset - Bits[7:0], RO, default = 24'h1000 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Next DFH Byte offset.
     */
    UINT32 reserved : 20;
    /* reserved - Bits[27:8], RsvdZ, default = 20'h0 
       Reserved
     */
    UINT32 featuretype : 4;
    /* featuretype - Bits[31:28], RO, default = 4'h3 
       Feature Type = Private Feature
     */
  } Bits;
  UINT32 Data;
} FME_PR_DFH_N1_FPGA_FME_STRUCT;


/* FME_PR_CTRL_N0_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x00005008)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_CTRL_N0_FPGA_FME_REG 0x00005008
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Control
 */
typedef union {
  struct {
    UINT32 prreset : 1;
    /* prreset - Bits[0:0], RW, default = 1'h0 
       This bit is used to clear a timeout error caused due to previous PR Operation. 
       PR Reset Protocol:
       1. SW initiates a PR reset by writing 1 to this bit
       2. Upon receiving a PR reset, HW clears its states and acks the reset by setting 
       FME_PR_CTRL[4] (PR ResetAck bit).  
       3. SW after asserting PR reset should poll on PR ResetAck bit to go high. Once 
       ResetAck bit is set by HW, the SW should de-assert the PR Reset bit and start a 
       new PR operation 
     */
    UINT32 reserved1 : 3;
    /* reserved1 - Bits[3:1], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 prresetack : 1;
    /* prresetack - Bits[4:4], RO, default = 1'h0 
       Indicates a HW ack to the SW initiated PR Reset. This bit is valid only when PR 
       Reset bit initiated by SW is 1.  
       PR reset state machine is kicked off when PR Reset bit is set by the SW. Upon 
       successful completion of PR reset sequence, the HW sets PR ResetAck bit. 
     */
    UINT32 reserved5 : 3;
    /* reserved5 - Bits[7:5], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 prregionid : 2;
    /* prregionid - Bits[9:8], RW, default = 2'h0 
       0x0 - Select PR 0 for reconfiguration
       0x1 - select PR 1 for reconfiguration
       0x2 - Reserved
       0x3 - Reserved
     */
    UINT32 reserved10 : 2;
    /* reserved10 - Bits[11:10], RsvdZ, default = 2'h0 
       Reserved
     */
    UINT32 prstartrequest : 1;
    /* prstartrequest - Bits[12:12], RW1S, default = 1'h0 
       SW writes 1 to request access for PR service.
       Hardware clears this bit after PR is complete or if PR error occurs or if
       PR service timeout is reached. The timeout value is hardcoded at
       compile time.
     */
    UINT32 prdatapushcomplete : 1;
    /* prdatapushcomplete - Bits[13:13], RW1S, default = 1'h0 
       SW writes 1 to this bit to notify bitstream push complete after pushing the last 
       data to FME_PR_DATA. 
       Hardware clears this bit if PR is complete or if PR fails or if timeout is 
       reached 
     */
    UINT32 prkind : 1;
    /* prkind - Bits[14:14], RW, default = 1'h0 
       0 - Load Customer Green bitstream. 1 - Load Intel Green bitstream. This bit has 
       to be configured before asserting PRStartRequest. Changes to this bit after 
       asserting PRStartRequest will be ignored and might result in Denial of PR 
       service  
     */
    UINT32 reserved13 : 17;
    /* reserved13 - Bits[31:15], RsvdZ, default = 17'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_PR_CTRL_N0_FPGA_FME_STRUCT;


/* FME_PR_CTRL_N1_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x0000500C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_CTRL_N1_FPGA_FME_REG 0x0000500C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Control
 */
typedef union {
  struct {
    UINT32 cfgdata : 32;
    /* cfgdata - Bits[31:0], RW, default = 32'h0 
       Config data
     */
  } Bits;
  UINT32 Data;
} FME_PR_CTRL_N1_FPGA_FME_STRUCT;


/* FME_PR_STATUS_N0_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00005010)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_STATUS_N0_FPGA_FME_REG 0x00005010
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Status
 */
typedef union {
  struct {
    UINT32 numbcredits : 9;
    /* numbcredits - Bits[8:0], RO, default = 9'h0 
       This is used to track number of available entries in HW queue inside the PR 
       engine. SW must check number of credits is greater than 0 before it writes to 
       PR_DATA. 
     */
    UINT32 reserved9 : 7;
    /* reserved9 - Bits[15:9], RsvdZ, default = 7'h0 
       Reserved
     */
    UINT32 prstatus : 1;
    /* prstatus - Bits[16:16], RO, default = 1'h0 
       1 - PR operation failed
       This bit is valid only when PR Start request bit = 0
       When starting a new PR operation, SW must read this bit and clear errors from 
       previous PR. HW will auto clear this bit when FME_PR_ERROR[5:0] = 0x0 
     */
    UINT32 reserved17 : 3;
    /* reserved17 - Bits[19:17], RsvdZ, default = 3'h0 
       Reserved
     */
    UINT32 alteraprcrtlrstatus : 3;
    /* alteraprcrtlrstatus - Bits[22:20], RO, default = 3'h0 
       0 - power-up or nreset asserted
       1 - PR_ERROR was triggered
       2 - CRC_ERROR was triggered
       3 - Incompatible bitstream error detected
       4 - PR operation in progress
       5 - PR operation passed
       6 - Reserved
       7 - Reserved
       This field is used for additional debug information only. Regular PR flow does 
       not require this 
     */
    UINT32 reserved23 : 1;
    /* reserved23 - Bits[23:23], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 prhoststatus : 4;
    /* prhoststatus - Bits[27:24], RO, default = 4'h0 
       0x0 - Idle State, waiting for PR start
       0x1 - Received PR start
       0x2 - Doing PR
       0x3 - Doing PR
       0x4 - Doing PR
       0x5 - Doing PR
       0x6 - Doing PR
       0x7 - PR complete
       0x8 - PR Error
     */
    UINT32 reserved28 : 4;
    /* reserved28 - Bits[31:28], RsvdZ, default = 4'h0 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_PR_STATUS_N0_FPGA_FME_STRUCT;


/* FME_PR_STATUS_N1_FPGA_FME_REG supported on:                                */
/*       SKXP (0x00005014)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_STATUS_N1_FPGA_FME_REG 0x00005014
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Status
 */
typedef union {
  struct {
    UINT32 tbd : 32;
    /* tbd - Bits[31:0], RO, default = 32'h0 
       Security Block Status fields 32b (TBD)
     */
  } Bits;
  UINT32 Data;
} FME_PR_STATUS_N1_FPGA_FME_STRUCT;


/* FME_PR_DATA_N0_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x00005018)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_DATA_N0_FPGA_FME_REG 0x00005018
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Data
 */
typedef union {
  struct {
    UINT32 prdata : 32;
    /* prdata - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       PR data from the raw-binary file for partial reconfiguration.
       SW must check number of Credits is greated than 0 and PR Grant=1 before it 
       writes to PR Data register. Violating this may cause a PR error. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_DATA_N0_FPGA_FME_STRUCT;


/* FME_PR_DATA_N1_FPGA_FME_REG supported on:                                  */
/*       SKXP (0x0000501C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_DATA_N1_FPGA_FME_REG 0x0000501C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Data
 */
typedef union {
  struct {
    UINT32 prdata : 32;
    /* prdata - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       PR data from the raw-binary file for partial reconfiguration.
       SW must check number of Credits is greated than 0 and PR Grant=1 before it 
       writes to PR Data register. Violating this may cause a PR error. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_DATA_N1_FPGA_FME_STRUCT;


/* FME_PR_ERROR_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00005020)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_ERROR_N0_FPGA_FME_REG 0x00005020
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Error Status
 */
typedef union {
  struct {
    UINT32 hostinitoperationerror : 1;
    /* hostinitoperationerror - Bits[0:0], RW1C, default = 1'h0 
       1 - Previous PR errors detected by PR Host
       SW writes 1 to clear this bit
       Before asserting PR Start request, SW must check FME_PR_STATUS [16] to record 
       the result of previous PR operation. If this bit is set, previous PR failed due 
       to one or more errors. SW must read FME_PR_ERROR [5:0] and clear the bits which 
       are set. Once the bits in FME_PR_ERROR [5:0] are cleared, HW will auto clear 
       FME_PR_STATUS [16], enabling start of next PR operation. 
       Not clearing the error bits set in FME_PR_ERROR [5:1] and starting another PR 
       operation will result in PR error with FME_PR_ERROR [0] getting asserted. 
     */
    UINT32 ipinitcrcerror : 1;
    /* ipinitcrcerror - Bits[1:1], RW1C, default = 1'h0 
       1 - CRC error detected by PR IP
       SW writes 1 to clear this bit
     */
    UINT32 ipinitincompatiblebitstream : 1;
    /* ipinitincompatiblebitstream - Bits[2:2], RW1C, default = 1'h0 
       1 - Incompatible PR bitstream detected by PR IP
       SW writes 1 to clear this bit
     */
    UINT32 ipinitprotocolerror : 1;
    /* ipinitprotocolerror - Bits[3:3], RW1C, default = 1'h0 
       1 - PR data push protocol violated
       SW writes 1 to clear this bit
     */
    UINT32 hostinitfifooverflow : 1;
    /* hostinitfifooverflow - Bits[4:4], RW1C, default = 1'h0 
       1 - PR data fifo overflow error detected
       SW writes 1 to clear this bit
     */
    UINT32 reserved5 : 1;
    /* reserved5 - Bits[5:5], RsvdZ, default = 1'h0 
       Reserved
     */
    UINT32 secureloadfailed : 1;
    /* secureloadfailed - Bits[6:6], RW1C, default = 1'h0 
       1- Protected Exchange PR load failed. SW writes 1 to clear this bit
     */
    UINT32 reserved7 : 25;
    /* reserved7 - Bits[31:7], RsvdZ, default = 57'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_PR_ERROR_N0_FPGA_FME_STRUCT;


/* FME_PR_ERROR_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00005024)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_ERROR_N1_FPGA_FME_REG 0x00005024
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME Partial Reconfiguration Error Status
 */
typedef union {
  struct {
    UINT32 reserved7 : 32;
    /* reserved7 - Bits[31:0], RsvdZ, default = 57'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} FME_PR_ERROR_N1_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH0_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00005028)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH0_N0_FPGA_FME_REG 0x00005028
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH0
 */
typedef union {
  struct {
    UINT32 publichash0 : 32;
    /* publichash0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [63:0]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH0_N0_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH0_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x0000502C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH0_N1_FPGA_FME_REG 0x0000502C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH0
 */
typedef union {
  struct {
    UINT32 publichash0 : 32;
    /* publichash0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [63:0]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH0_N1_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH1_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00005030)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH1_N0_FPGA_FME_REG 0x00005030
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH1
 */
typedef union {
  struct {
    UINT32 publichash1 : 32;
    /* publichash1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [127:64]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH1_N0_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH1_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00005034)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH1_N1_FPGA_FME_REG 0x00005034
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH1
 */
typedef union {
  struct {
    UINT32 publichash1 : 32;
    /* publichash1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [127:64]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH1_N1_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH2_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00005038)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH2_N0_FPGA_FME_REG 0x00005038
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH2
 */
typedef union {
  struct {
    UINT32 publichash2 : 32;
    /* publichash2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [191:128]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH2_N0_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH2_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x0000503C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH2_N1_FPGA_FME_REG 0x0000503C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH2
 */
typedef union {
  struct {
    UINT32 publichash2 : 32;
    /* publichash2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [191:128]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH2_N1_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH3_N0_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00005040)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH3_N0_FPGA_FME_REG 0x00005040
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH3
 */
typedef union {
  struct {
    UINT32 publichash3 : 32;
    /* publichash3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [255:192]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH3_N0_FPGA_FME_STRUCT;


/* FME_PR_PUB_HASH3_N1_FPGA_FME_REG supported on:                             */
/*       SKXP (0x00005044)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PUB_HASH3_N1_FPGA_FME_REG 0x00005044
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Public HASH3
 */
typedef union {
  struct {
    UINT32 publichash3 : 32;
    /* publichash3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Public Hash [255:192]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PUB_HASH3_N1_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH0_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00005048)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH0_N0_FPGA_FME_REG 0x00005048
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH0
 */
typedef union {
  struct {
    UINT32 privatehash0 : 32;
    /* privatehash0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [63:0]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH0_N0_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH0_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x0000504C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH0_N1_FPGA_FME_REG 0x0000504C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH0
 */
typedef union {
  struct {
    UINT32 privatehash0 : 32;
    /* privatehash0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [63:0]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH0_N1_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH1_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00005050)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH1_N0_FPGA_FME_REG 0x00005050
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH1
 */
typedef union {
  struct {
    UINT32 privatehash1 : 32;
    /* privatehash1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [127:64]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH1_N0_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH1_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00005054)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH1_N1_FPGA_FME_REG 0x00005054
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH1
 */
typedef union {
  struct {
    UINT32 privatehash1 : 32;
    /* privatehash1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [127:64]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH1_N1_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH2_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00005058)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH2_N0_FPGA_FME_REG 0x00005058
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH2
 */
typedef union {
  struct {
    UINT32 privatehash2 : 32;
    /* privatehash2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [191:128]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH2_N0_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH2_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x0000505C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH2_N1_FPGA_FME_REG 0x0000505C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH2
 */
typedef union {
  struct {
    UINT32 privatehash2 : 32;
    /* privatehash2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [191:128]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH2_N1_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH3_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00005060)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH3_N0_FPGA_FME_REG 0x00005060
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH3
 */
typedef union {
  struct {
    UINT32 privatehash3 : 32;
    /* privatehash3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [255:192]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH3_N0_FPGA_FME_STRUCT;


/* FME_PR_PRIV_HASH3_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x00005064)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_PRIV_HASH3_N1_FPGA_FME_REG 0x00005064
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Private HASH3
 */
typedef union {
  struct {
    UINT32 privatehash3 : 32;
    /* privatehash3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Private Hash [255:192]
       Note that this register is updated over SMBUS after blue bitstream load and 
       locked at boot time. ALI Green bitstream loader can only read this register to 
       retrieve the hash before a PR load. 
     */
  } Bits;
  UINT32 Data;
} FME_PR_PRIV_HASH3_N1_FPGA_FME_STRUCT;


/* FME_PR_LICENSE0_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00005068)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE0_N0_FPGA_FME_REG 0x00005068
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 0
 */
typedef union {
  struct {
    UINT32 license0 : 32;
    /* license0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [63:0]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE0_N0_FPGA_FME_STRUCT;


/* FME_PR_LICENSE0_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x0000506C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE0_N1_FPGA_FME_REG 0x0000506C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 0
 */
typedef union {
  struct {
    UINT32 license0 : 32;
    /* license0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [63:0]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE0_N1_FPGA_FME_STRUCT;


/* FME_PR_LICENSE1_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00005070)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE1_N0_FPGA_FME_REG 0x00005070
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 1
 */
typedef union {
  struct {
    UINT32 license1 : 32;
    /* license1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [127:64]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE1_N0_FPGA_FME_STRUCT;


/* FME_PR_LICENSE1_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00005074)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE1_N1_FPGA_FME_REG 0x00005074
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 1
 */
typedef union {
  struct {
    UINT32 license1 : 32;
    /* license1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [127:64]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE1_N1_FPGA_FME_STRUCT;


/* FME_PR_LICENSE2_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00005078)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE2_N0_FPGA_FME_REG 0x00005078
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 2
 */
typedef union {
  struct {
    UINT32 license2 : 32;
    /* license2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [191:128]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE2_N0_FPGA_FME_STRUCT;


/* FME_PR_LICENSE2_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x0000507C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE2_N1_FPGA_FME_REG 0x0000507C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 2
 */
typedef union {
  struct {
    UINT32 license2 : 32;
    /* license2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [191:128]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE2_N1_FPGA_FME_STRUCT;


/* FME_PR_LICENSE3_N0_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00005080)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE3_N0_FPGA_FME_REG 0x00005080
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 3
 */
typedef union {
  struct {
    UINT32 license3 : 32;
    /* license3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [255:192]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE3_N0_FPGA_FME_STRUCT;


/* FME_PR_LICENSE3_N1_FPGA_FME_REG supported on:                              */
/*       SKXP (0x00005084)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_LICENSE3_N1_FPGA_FME_REG 0x00005084
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR License 3
 */
typedef union {
  struct {
    UINT32 license3 : 32;
    /* license3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR License [255:192]
     */
  } Bits;
  UINT32 Data;
} FME_PR_LICENSE3_N1_FPGA_FME_STRUCT;


/* FME_PR_SESKEY0_N0_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00005088)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY0_N0_FPGA_FME_REG 0x00005088
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 0
 */
typedef union {
  struct {
    UINT32 sessionkey0 : 32;
    /* sessionkey0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [63:0]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY0_N0_FPGA_FME_STRUCT;


/* FME_PR_SESKEY0_N1_FPGA_FME_REG supported on:                               */
/*       SKXP (0x0000508C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY0_N1_FPGA_FME_REG 0x0000508C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 0
 */
typedef union {
  struct {
    UINT32 sessionkey0 : 32;
    /* sessionkey0 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [63:0]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY0_N1_FPGA_FME_STRUCT;


/* FME_PR_SESKEY1_N0_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00005090)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY1_N0_FPGA_FME_REG 0x00005090
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 1
 */
typedef union {
  struct {
    UINT32 sessionkey1 : 32;
    /* sessionkey1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [127:64]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY1_N0_FPGA_FME_STRUCT;


/* FME_PR_SESKEY1_N1_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00005094)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY1_N1_FPGA_FME_REG 0x00005094
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 1
 */
typedef union {
  struct {
    UINT32 sessionkey1 : 32;
    /* sessionkey1 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [127:64]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY1_N1_FPGA_FME_STRUCT;


/* FME_PR_SESKEY2_N0_FPGA_FME_REG supported on:                               */
/*       SKXP (0x00005098)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY2_N0_FPGA_FME_REG 0x00005098
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 2
 */
typedef union {
  struct {
    UINT32 sessionkey2 : 32;
    /* sessionkey2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [191:128]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY2_N0_FPGA_FME_STRUCT;


/* FME_PR_SESKEY2_N1_FPGA_FME_REG supported on:                               */
/*       SKXP (0x0000509C)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY2_N1_FPGA_FME_REG 0x0000509C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 2
 */
typedef union {
  struct {
    UINT32 sessionkey2 : 32;
    /* sessionkey2 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [191:128]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY2_N1_FPGA_FME_STRUCT;


/* FME_PR_SESKEY3_N0_FPGA_FME_REG supported on:                               */
/*       SKXP (0x000050A0)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY3_N0_FPGA_FME_REG 0x000050A0
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 3
 */
typedef union {
  struct {
    UINT32 sessionkey3 : 32;
    /* sessionkey3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [255:192]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY3_N0_FPGA_FME_STRUCT;


/* FME_PR_SESKEY3_N1_FPGA_FME_REG supported on:                               */
/*       SKXP (0x000050A4)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_SESKEY3_N1_FPGA_FME_REG 0x000050A4
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Session Key 3
 */
typedef union {
  struct {
    UINT32 sessionkey3 : 32;
    /* sessionkey3 - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Session Key [255:192]
     */
  } Bits;
  UINT32 Data;
} FME_PR_SESKEY3_N1_FPGA_FME_STRUCT;


/* FME_PR_INTFC_ID_L_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x000050A8)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_INTFC_ID_L_N0_FPGA_FME_REG 0x000050A8
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Interface ID Low
 */
typedef union {
  struct {
    UINT32 interfaceidl : 32;
    /* interfaceidl - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Interface ID low
     */
  } Bits;
  UINT32 Data;
} FME_PR_INTFC_ID_L_N0_FPGA_FME_STRUCT;


/* FME_PR_INTFC_ID_L_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x000050AC)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_INTFC_ID_L_N1_FPGA_FME_REG 0x000050AC
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Interface ID Low
 */
typedef union {
  struct {
    UINT32 interfaceidl : 32;
    /* interfaceidl - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Interface ID low
     */
  } Bits;
  UINT32 Data;
} FME_PR_INTFC_ID_L_N1_FPGA_FME_STRUCT;


/* FME_PR_INTFC_ID_H_N0_FPGA_FME_REG supported on:                            */
/*       SKXP (0x000050B0)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_INTFC_ID_H_N0_FPGA_FME_REG 0x000050B0
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Interface ID High
 */
typedef union {
  struct {
    UINT32 interfaceidh : 32;
    /* interfaceidh - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Interface ID high
     */
  } Bits;
  UINT32 Data;
} FME_PR_INTFC_ID_H_N0_FPGA_FME_STRUCT;


/* FME_PR_INTFC_ID_H_N1_FPGA_FME_REG supported on:                            */
/*       SKXP (0x000050B4)                                                    */
/* Register default value:              0x00000000                            */
#define FME_PR_INTFC_ID_H_N1_FPGA_FME_REG 0x000050B4
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * FME PR Interface ID High
 */
typedef union {
  struct {
    UINT32 interfaceidh : 32;
    /* interfaceidh - Bits[31:0], RW, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       FME PR Interface ID high
     */
  } Bits;
  UINT32 Data;
} FME_PR_INTFC_ID_H_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR0_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008000)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR0_N0_FPGA_FME_REG 0x00008000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry0 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR0_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR0_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008004)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR0_N1_FPGA_FME_REG 0x00008004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry0 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR0_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT0_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008008)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT0_N0_FPGA_FME_REG 0x00008008
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry0 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT0_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT0_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000800C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT0_N1_FPGA_FME_REG 0x0000800C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry0 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT0_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR1_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008010)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR1_N0_FPGA_FME_REG 0x00008010
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry1 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR1_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR1_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008014)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR1_N1_FPGA_FME_REG 0x00008014
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry1 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR1_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT1_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008018)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT1_N0_FPGA_FME_REG 0x00008018
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry1 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT1_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT1_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000801C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT1_N1_FPGA_FME_REG 0x0000801C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry1 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT1_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR2_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008020)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR2_N0_FPGA_FME_REG 0x00008020
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry2 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR2_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR2_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008024)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR2_N1_FPGA_FME_REG 0x00008024
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry2 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR2_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT2_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008028)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT2_N0_FPGA_FME_REG 0x00008028
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry2 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT2_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT2_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000802C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT2_N1_FPGA_FME_REG 0x0000802C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry2 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT2_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR3_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008030)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR3_N0_FPGA_FME_REG 0x00008030
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry3 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR3_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR3_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008034)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR3_N1_FPGA_FME_REG 0x00008034
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry3 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR3_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT3_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008038)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT3_N0_FPGA_FME_REG 0x00008038
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry3 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT3_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT3_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000803C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT3_N1_FPGA_FME_REG 0x0000803C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry3 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT3_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR4_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008040)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR4_N0_FPGA_FME_REG 0x00008040
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry4 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR4_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR4_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008044)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR4_N1_FPGA_FME_REG 0x00008044
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry4 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR4_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT4_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008048)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT4_N0_FPGA_FME_REG 0x00008048
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry4 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT4_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT4_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000804C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT4_N1_FPGA_FME_REG 0x0000804C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry4 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT4_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR5_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008050)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR5_N0_FPGA_FME_REG 0x00008050
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry5 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR5_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR5_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008054)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR5_N1_FPGA_FME_REG 0x00008054
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry5 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR5_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT5_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008058)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT5_N0_FPGA_FME_REG 0x00008058
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry5 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT5_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT5_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000805C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT5_N1_FPGA_FME_REG 0x0000805C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry5 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT5_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR6_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008060)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR6_N0_FPGA_FME_REG 0x00008060
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry6 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR6_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR6_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008064)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR6_N1_FPGA_FME_REG 0x00008064
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry6 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR6_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT6_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008068)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT6_N0_FPGA_FME_REG 0x00008068
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry6 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT6_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT6_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000806C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT6_N1_FPGA_FME_REG 0x0000806C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry6 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT6_N1_FPGA_FME_STRUCT;


/* MSIX_ADDR7_N0_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008070)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR7_N0_FPGA_FME_REG 0x00008070
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry7 address
 */
typedef union {
  struct {
    UINT32 msgaddrlow : 32;
    /* msgaddrlow - Bits[31:0], RW, default = 32'h0 
       Message Address Lower bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR7_N0_FPGA_FME_STRUCT;


/* MSIX_ADDR7_N1_FPGA_FME_REG supported on:                                   */
/*       SKXP (0x00008074)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_ADDR7_N1_FPGA_FME_REG 0x00008074
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry7 address
 */
typedef union {
  struct {
    UINT32 msgaddrupp : 32;
    /* msgaddrupp - Bits[31:0], RW, default = 32'h0 
       Message Address Upper bits
     */
  } Bits;
  UINT32 Data;
} MSIX_ADDR7_N1_FPGA_FME_STRUCT;


/* MSIX_CTLDAT7_N0_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x00008078)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_CTLDAT7_N0_FPGA_FME_REG 0x00008078
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry7 control and data
 */
typedef union {
  struct {
    UINT32 msgdata : 32;
    /* msgdata - Bits[31:0], RW, default = 32'h0 
       Message Data
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT7_N0_FPGA_FME_STRUCT;


/* MSIX_CTLDAT7_N1_FPGA_FME_REG supported on:                                 */
/*       SKXP (0x0000807C)                                                    */
/* Register default value:              0x00000001                            */
#define MSIX_CTLDAT7_N1_FPGA_FME_REG 0x0000807C
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX table entry7 control and data
 */
typedef union {
  struct {
    UINT32 msgcontrol : 32;
    /* msgcontrol - Bits[31:0], RW, default = 32'h1 
       Message Control
     */
  } Bits;
  UINT32 Data;
} MSIX_CTLDAT7_N1_FPGA_FME_STRUCT;


/* MSIX_PBA_N0_FPGA_FME_REG supported on:                                     */
/*       SKXP (0x00009000)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_PBA_N0_FPGA_FME_REG 0x00009000
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX PBA
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} MSIX_PBA_N0_FPGA_FME_STRUCT;


/* MSIX_PBA_N1_FPGA_FME_REG supported on:                                     */
/*       SKXP (0x00009004)                                                    */
/* Register default value:              0x00000000                            */
#define MSIX_PBA_N1_FPGA_FME_REG 0x00009004
/* Struct format extracted from XML file 1\0_16_0_windows0.xml.
 * MSIX PBA
 */
typedef union {
  struct {
    UINT32 reserved0 : 32;
    /* reserved0 - Bits[31:0], RsvdZ, default = 64'h0 
       This field was split. It's value spans this field and the field of similar name 
       in the next/preceding struct. 
       Reserved
     */
  } Bits;
  UINT32 Data;
} MSIX_PBA_N1_FPGA_FME_STRUCT;


#endif /* FPGA_FME_h */
