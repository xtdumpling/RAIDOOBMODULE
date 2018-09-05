//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/
#ifndef  _CPU_PCI_ACCESS_H_
#define  _CPU_PCI_ACCESS_H_

#include "DataTypes.h"



//
// CPU Types; this needs to be contiguous to assist in table look up
//
#define MAX_CPU_TYPES        1

//
// CPU Index for MC function look-up
//
#define MAX_CPU_INDEX        1


//
// Box Types; this needs to be contiguous to assist in table look up
//
#define  BOX_CHA_MISC        0
#define  BOX_CHA_PMA         1
#define  BOX_CHA_CMS         2
#define  BOX_CHABC           3
#define  BOX_PCU             4
#define  BOX_VCU             5
#define  BOX_M2MEM           6
#define  BOX_MC              7
#define  BOX_MCIO            8
#define  BOX_KTI             9
#define  BOX_M3KTI           10
#define  BOX_MCDDC           11
#define  BOX_M2UPCIE         12
#define  BOX_IIO_PCIE_DMI    13
#define  BOX_IIO_PCIE        14
#define  BOX_IIO_PCIE_NTB    15
#define  BOX_IIO_CB          16
#define  BOX_IIO_VTD         17
#define  BOX_IIO_DFX         18
#define  BOX_UBOX            19
#define  BOX_FPGA            20
#define  MAX_BOX_TYPES       21


//
// Maximum Number of Instances supported by each box type. Note that if the number of instances
// are same for all supported CPUs, then we will have only one #define here (i.e MAX_ALL_XXXXX)
//
#define  MAX_SKX_CHA         28

#define  MAX_SKX_M2PCIE      5

#define  MAX_ALL_CBOBC       1

#define  MAX_SKX_M3KTI       2

#define  MAX_SKX_KTIAGENT    3

#define  MAX_SKX_M2MEM       2

#define  MAX_ALL_M2PCIE      1
#define  MAX_ALL_UBOX        1
#define  MAX_ALL_IIO         4
#define  MAX_ALL_PCU         1
#define  MAX_ALL_VCU         1

#define  MAX_ALL_IIO_CB          1  // 1 instance per CB function block
#define  MAX_ALL_IIO_PCIE_DMI    1  // 0:0:0
#define  MAX_ALL_IIO_PCIE_NTB    3  // 4 instances in PCIE_NTB (0:3:0/1/2/3)
#define  MAX_ALL_IIO_DFX         21 // 4 instances per M/PSTACK + 1 Cstack
#define  MAX_ALL_IIO_DFX_DMI     4  // 4 instances in C stack
#define  MAX_ALL_IIO_DFX_VTD     6  // 6 instances in IIO_DFX block across C/P/MCP stacks
#define  MAX_ALL_IIO_DFX_VTD_DMI 1  // 1 instances in IIO_DFX block across C stack
#define  MAX_ALL_IIO_PCIE        21 // 4 instances per M/PSTACK + 1 Cstack


//
// MAX instances of MC functional
// blocks for different CPU Sub-types.
// These #defines are not used in
// the code.  They are included here to
// aid understanding the Instance -> function mapping
// implementation in CpuPciAccess.c
//
#define  MAX_SKX_1HA_MCMAIN         1
#define  MAX_SKX_1HA_MCCTL          4
#define  MAX_SKX_1HA_MCDP           4
#define  MAX_SKX_1HA_MCDECS         4
#define  MAX_SKX_1HA_MCDDRIOGBC     1
#define  MAX_SKX_1HA_MCDDRIOEXTGBC  1
#define  MAX_SKX_1HA_MCDDRIO        4
#define  MAX_SKX_1HA_MCDDRIOEXT     4
#define  MAX_SKX_1HA_MCDDRIOMCC     4
#define  MAX_SKX_1HA_MCDDRIOEXTMCC  4

#define  MAX_SKX_2HA_MCMAIN         2
#define  MAX_SKX_2HA_MCMAINEXT      2
#define  MAX_SKX_2HA_MCCTL          4
#define  MAX_SKX_2HA_MCDP           4
#define  MAX_SKX_2HA_MCDECS         4
#define  MAX_SKX_2HA_MCDDRIO        4
#define  MAX_SKX_2HA_MCDDRIOEXT     4
#define  MAX_SKX_2HA_MCDDRIOMCC     4
#define  MAX_SKX_2HA_MCDDRIOEXTMCC  4

#define  MAX_SKX_4HA_MCMAIN         2
#define  MAX_SKX_4HA_MCMAINEXT      2
#define  MAX_SKX_4HA_MCCTL          4
#define  MAX_SKX_4HA_MCDP           4
#define  MAX_SKX_4HA_MCDECS         4
#define  MAX_SKX_4HA_MCDDRIO        4
#define  MAX_SKX_4HA_MCDDRIOEXT     4
#define  MAX_SKX_4HA_MCDDRIOMCC     4
#define  MAX_SKX_4HA_MCDDRIOEXTMCC  4


#define  MAX_BOX_INSTANCES      8


//
// Functional Blocks supported by each box type; there is a 1:1 maping between these definitons and
// PCI function supported by a box. But the valude of the define itself doesn't have any relationship
// with the actual PCI Function number. That translation will be done by the helper routine for
// different supported CPU types.
// Note: These values need  to be contiguos for each box type to assist in table look up
//
#define  CHA_MISC            0

#define  CHA_PMA             0

#define  CHA_CMS             0

#define  CHABC_SAD           0
#define  CHABC_SAD1          1
#define  CHABC_UTIL          2
#define  CHABC_PMA           3

#define  PCU_FUN0            0
#define  PCU_FUN1            1
#define  PCU_FUN2            2
#define  PCU_FUN3            3
#define  PCU_FUN4            4
#define  PCU_FUN5            5
#define  PCU_FUN6            6
#define  PCU_FUN7            7

#define  VCU_FUN0            0
#define  VCU_FUN1            1
#define  VCU_FUN2            2
#define  VCU_FUN3            3
#define  VCU_FUN4            4
#define  VCU_FUN5            5
#define  VCU_FUN6            6
#define  VCU_FUN7            7

#define  M2MEM_MAIN          0
#define  M2MEM_CMS           1

#define  MC_MAIN             0
#define  MC_2LM              1  //SKX:TODO come back.  Maybe be own box?
#define  MC_MAINEXT          3
#define  MAX_MC_FUNC_BLKS    4
#define  MAX_INST_IN_MC      4

#define  MCIO_DDRIO          0
#define  MCIO_DDRIOEXT       1

#define  KTI_LLPMON          0
#define  KTI_REUT            1
#define  KTI_CIOPHYDFX       2
#define  KTI_LLDFX           3

#define  KTI0_DEV_NUM        8
#define  KTI1_DEV_NUM        9
#define  KTI2_DEV_NUM       10

#define  M3KTI_MAIN          0
#define  M3KTI_PMON          1
#define  M3KTI_PMON1         2
#define  M3KTI_CMS           3

#define  MCDDC_CTL           0
#define  MCDDC_DP            1

#define  M2UPCIE_MAIN        0
#define  M2UPCIE_PMON        1
#define  M2UPCIE_CMS         2

#define  IIO_PCIEDMI         0

#define  IIO_PCIE            0

#define  IIO_PCIENTB         0

#define  IIO_CB_FUNC0        0
#define  IIO_CB_FUNC1        1
#define  IIO_CB_FUNC2        2
#define  IIO_CB_FUNC3        3
#define  IIO_CB_FUNC4        4
#define  IIO_CB_FUNC5        5
#define  IIO_CB_FUNC6        6
#define  IIO_CB_FUNC7        7

#define  IIO_VTD             0
#define  IIO_RAS             2
#define  IIO_IOAPIC          3
#define  IIO_VMD             4
#define  IIO_PERFMON         5

#define  IIO_DFX             0
#define  IIO_DFX_DMI         1
#define  IIO_DFX_GLOBAL      2
#define  IIO_DFX_GLOBAL_DMI  3
#define  IIO_DFX_VTD         4
#define  IIO_DFX_VTD_DMI     5


#define  UBOX_CFG            0
#define  UBOX_DFX            1
#define  UBOX_MISC           2


#define  MCDDC_DESC          2
#define  MAX_MCDDC_FUNC_BLKS 4
#define  MAX_INST_IN_MCDDC   8

//#define  MCIO_DDRIOGBC       0
//#define  MCIO_DDRIOEXTGBC    1
#define  MCIO_DDRIO          0
#define  MCIO_DDRIOEXT       1
#define  MCIO_DDRIOMCC       4  //SKX:look into this
#define  MCIO_DDRIOEXTMCC    5  //SKX:look into this
#define  MAX_MCIO_FUNC_BLKS  6
#define  MAX_INST_IN_MCIO    6

#define MEM_MAIN             ((BOX_MC << 8)    | (MC_MAIN))
#define MEM_MAINEXT          ((BOX_MC << 8)    | (MC_MAINEXT))
#define MEM_CTL              ((BOX_MCDDC << 8) | (MCDDC_CTL))
#define MEM_DP               ((BOX_MCDDC << 8) | (MCDDC_DP))
#define MEM_DESC             ((BOX_MCDDC << 8) | (MCDDC_DESC))
#define MEM_DDRIOGBC         ((BOX_MCIO << 8)  | (MCIO_DDRIOGBC))
#define MEM_DDRIOEXTGBC      ((BOX_MCIO << 8)  | (MCIO_DDRIOEXTGBC))
#define MEM_DDRIO            ((BOX_MCIO << 8)  | (MCIO_DDRIO))
#define MEM_DDRIOEXT         ((BOX_MCIO << 8)  | (MCIO_DDRIOEXT))
#define MEM_DDRIOMCC         ((BOX_MCIO << 8)  | (MCIO_DDRIOMCC))
#define MEM_DDRIOEXTMCC      ((BOX_MCIO << 8)  | (MCIO_DDRIOEXTMCC))

//
#define PCU_INSTANCE_0 0

//
// Legacy PCI address for CSRs
// Note: 
//     These addresses are used when syshost structure is not yet initialized and CSR read/warite functions are not yet usable.
//     They need to be manually updated when register location is changed to match the register header file.
//     Keep such usage of CSRs at minimun and only when necessary.
//
#define SKX_LEGACY_CSR_MMCFG_RULE_N0_CHABC_SAD1_REG    PCI_REG_ADDR(1,29,1,0xc0)  // 1:29:1:C0   MMCFG_RULE in CHA

//
// Format of CSR register offset passed to helper functions.
// This must be kept in sync with the CSR XML parser tool that generates CSR offset definitions in the CSR header files.
//
typedef union {
  struct  {
    UINT32 offset : 12;         // bits <11:0>
    UINT32 size : 3;          // bits <14:12>
    UINT32 pseudo : 1;          // bit  <15>
    UINT32 funcblk : 8;          // bits <23:16>
    UINT32 boxtype : 8;          // bits <31:24>
  } Bits;
  UINT32 Data;
} CSR_OFFSET;


//
// Format of CSR register offset passed to helper functions.
// This must be kept in sync with the CSR XML parser tool that generates CSR offset definitions in the CSR header files.
//
#define PCI_REG_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xff) | (((Function) & 0x07) << 8) | (((Device) & 0x1f) << 11) | (((Bus) & 0xff) << 16))
#define PCIE_REG_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

#endif   // _CPU_PCI_ACCESS_H_
