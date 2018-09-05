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
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#ifndef _memhostchip_h
#define _memhostchip_h

#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysRegs.h"
#include "MemRegs.h"
#include "MemDefaults.h"
//#include "MmrcProjectDefinitionsGenerated.h"
#include "KtiSi.h"
#include "NGNDimmPlatformCfgData.h"

#ifndef ASM_INC

//
// IMC Vendor and Device Ids
//
#define IMC_VID     0x8086
#define IMC_DID     0x3CA8
#define ALL_DVID    0xFFFFFFFF

// Define the different SKX package Sku's
#define SKX_SKU_LCC 0x0
#define SKX_SKU_HCC 0x2
#define SKX_SKU_XCC 0x3

//
// MemTraining
//
#define MAX_VREF_SETTINGS       20
#define EARLY_VREF_STEP_SIZE    2

//
// InitMemChip
//
#define MIN_DDR4_VDD  900
#define MAX_DDR4_VDD  1850


//SKX changes
#define MAX_CMD_GROUP0    6
#define MAX_CMD_GROUP1    3
#define MAX_CMD_GROUP2    4
#define MAX_CMD_GROUPALL  13

//SKX changes
#define MAX_CTL_GROUP0  4
#define MAX_CTL_GROUP1  2
#define MAX_CTL_GROUP2  3
#define MAX_CTL_GROUP3  2
#define MAX_CTL_GROUPALL  11

#define MAX_CMDCTL_GROUPALL  (MAX_CMD_GROUPALL + MAX_CTL_GROUPALL)
#define CAS2DRVEN_MAXGAP  28

#define MIN_CMD_VREF              0
#define MAX_CMD_VREF              63

#pragma pack(1)
struct ChannelTestGroup
{
  UINT8                       channelBitMask;
  UINT32                      maxMtr;
};
#pragma pack()

#define MRC_ROUND_TRIP_DEFAULT_VALUE    39  // Round trip default starting value
#define MRC_ROUND_TRIP_MAX_VALUE        88  // Round trip max value according mas
#define MRC_IO_LATENCY_DEFAULT_VALUE    4   // IO latency default value according mas
#define MRC_ROUND_TRIP_IO_COMPENSATION  18 //KV114  // Roundtrip - IO compensation
#define MRC_ROUND_TRIP_IO_COMPENSATION_EMU 17 // Roundtrip - IO compensation for Emulation (temp WA)
#define MRC_ROUND_TRIP_IO_COMP_START    18 //KV114  // Roundtrip - IO compensation starting point

//
// defines for Receive enable
//
#define   MIN_IO_LATENCY              0

// MemTiming.c
// Min Timings (HSD 5370733) B2B Turnarounds
#define MIN_WWDR    2 
#define MIN_WWDD    2 

// enforce due to PXC (HSD 5370733)
#define ENFORCE_WWDR_PXC_EN 3
#define ENFORCE_WWDR_PXC_DIS 2

#define ENFORCE_WWDD_PXC_EN 3
#define ENFORCE_WWDD_PXC_DIS 2

#endif // ASM_INC

//
// Platform Assigned CSRs for PIPE/SOCKET/MRC MILESTONES, Pipe.h
//
#define SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_MMIO SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR
#define SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR  (BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG)  // UBOX scratchpad CSR13
#define SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_MMIO  SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR
#define SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR   (BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG)  // UBOX scratchpad CSR14
#define SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR_MMIO     SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR
#define SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR      (BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG)  // UBOX scratchpad CSR15


typedef union {
struct  {
   UINT32 selector:1;
   UINT32 dimmindex:4;
   UINT32 rsvd_8:11;
   UINT32 datavalue:16;
}Bits;
UINT32 Data;
}CMD_DRAM_PM_WRITE_PWR_COEFF;


#define MAX_RIR             4                   // Number of Rank Interleave Register rules for DDR
#define MAX_RIR_DDRT        4                   // Number of Rank Interleave Register rules for NVMDIMM
#define MAX_RIR_WAYS        8                   // Number of interleave ways for RIR for DDR
#define MAX_RIR_DDRT_WAYS   2                   // Number of interleave ways for RIR for NVMDIMM
#define TAD_RULES           8                   // Number of TAD rule registers
#define MAX_TAD_WAYS        3                   // Number of interleave ways for TAD RULES
#define SAD_RULES          24                   // Number of SAD rule registers
#define MAX_SAD_RULES      24                   // Number of SAD rule registers
#define MAX_SAD_WAYS        8                   // Number of interleave ways for SAD RULES
#define SAD_ATTR_DRAM       0                   // DRAM attribute for SAD rule
#define SAD_NXM_ATTR        2                   // Attr value for an NXM SAD rule
#define MAX_PAD             3                   // Number of PAD registers
#define MAX_RAW_CARD        5                   // Number of supported Raw Cards
#define MAX_STROBE          18                  // Number of strobe groups
#define MAX_SPD_BYTE        256                 // Number of bytes in Serial EEPROM
#define MAX_SPD_BYTE_DDR4   512                 // Number of bytes in Serial EEPROM on DDR4
#define PARITY_INTERLEAVE   1                   // SAD interleave with merged HAs (pseudo-hemisphere mode)
#define MAX_SEEDS           10                  // Maximum


//
// Panic Up/Down Multiplier
//
#define MIN_CAP 2 // 2 nF
#define CHARGED_VOLTAGE 24   // 24 mV
#define VSSHI 356   // Vsshi = 0.356 V (according to Vsshi target code = 0x25)
#define VCCP 1300   // Vccp = fixed value of 1.3 V
// Format 3.13 is a fixed point format in which the decimal point is assumed to be after 3 digits from the left. Dividing this whole number by 2^(13) gives the corresponding value in integer
// format.  Consider an example of converting 1100001000100011 in 3.13 format to integer format (unit is volts) (1100001000100011)2 = (49699)10; 49699 /2^(13){8192} = 6.0667724609375 volts
#define VCCP_CONVERSION_FACTOR 8192 // VCCP conversion factor is used when actual VCCP is read (and that it's 2^13 -- as it's written in the spec)
#define RDQ 90    // Rdq = 90 Ohm
#define PANICCOMPUPMULT 0x7F // max value for panic up multiplier
#define PANICCOMPDNMULT 0x7F // max value for panic down multiplier

//
// idle_cmd_to_cke_drops_timer value
//
#define IDLE_CMD_CKE_DROPS_TIMER 0xFF

#endif // _memhostchip_h
