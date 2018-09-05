//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Add tCCD_L Relaxation setup option for specific DIMMs.
//      Reason:     Requested by Micron, code reference from Jian.
//      Auditor:    Jacker Yeh
//      Date:       Mar/17/2017
//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

   Memory Reference Code

   ESS - Enterprise Silicon Software

   INTEL CONFIDENTIAL

@copyright
   Copyright 2006 - 2015 Intel Corporation All Rights Reserved.

   The source code contained or described herein and all documents
   related to the source code ("Material") are owned by Intel
   Corporation or its suppliers or licensors. Title to the Material
   remains with Intel Corporation or its suppliers and licensors.
   The Material contains trade secrets and proprietary and confidential
   information of Intel or its suppliers and licensors. The Material
   is protected by worldwide copyright and trade secret laws and treaty
   provisions.  No part of the Material may be used, copied, reproduced,
   modified, published, uploaded, posted, transmitted, distributed, or
   disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other
   intellectual property right is granted to or conferred upon you
   by disclosure or delivery of the Materials, either expressly,
   by implication, inducement, estoppel or otherwise. Any license
   under such intellectual property rights must be express and
   approved by Intel in writing.

@file
  MemRegs.h

@brief
  This file contains memory detection and initialization for
  IMC and DDR3 modules compliant with JEDEC specification.

**/

#ifndef _memregs_h
#define _memregs_h

//
// #define UNCORE_CR_UNCORE_MC_CFG_CONTROL   0x2E3
//
//
// Serial Presence Detect bytes (JEDEC revision 1.0)
//
#define SPD_SIZE              0     // Bytes used, Device size, CRC coverage
#define SPD_REVISION          1     // SPD Encoding Revision
#define SPD_KEY_BYTE          2     // DRAM Device Type  //BYTE 2 - keyByte
#define SPD_TYPE_DDR3         0x0B  // DDR3 SDRAM
#define SPD_TYPE_DDR4         0x0C  // DDR4 SDRAM
#define SPD_TYPE_AEP          0x0D  // NVMDIMM SDRAM
#define SPD_KEY_BYTE2         3     // Module Type and Thickness (RDIMM or UDIMM)  //BYTE 3 keyByte2
  #define SPD_RDIMM           1     // Module type is RDIMM
  #define SPD_UDIMM           2     // Module type is UDIMM
  #define SPD_SODIMM          3     // Module type is SODIMM
  #define SPD_MICRO_DIMM      4     // Module type is Micro-DIMM
  #define SPD_LRDIMM_DDR4     4     // Module type is LRDIMM (DDR4)
  #define SPD_MINI_RDIMM      5     // Module type is Mini-RDIMM
  #define SPD_MINI_UDIMM      6     // Module type is Mini-UDIMM
  #define SPD_MINI_CDIMM      7     // Module type is Mini-CDIMM
  #define SPD_ECC_SO_UDIMM    8     // Module type is 72b-SO-UDIMM
  #define SPD_ECC_SO_RDIMM    9     // Module type is 72b-SO-RDIMM
  #define SPD_ECC_SO_RDIMM_DDR4  8  // Module type is 72b-SO-RDIMM
  #define SPD_ECC_SO_UDIMM_DDR4  9  // Module type is 72b-SO-UDIMM
  #define SPD_ECC_SO_CDIMM    10    // Module type is 72b-SO-CDIMM
  #define SPD_LRDIMM          11    // Module type is LRDIMM
#ifdef SSA_FLAG
  #define SPD_16b_SO_DIMM     12    // Module type is 16b_SO_DIMM
  #define SPD_32b_SO_DIMM     13    // Module type is 32b_SO_DIMM
#endif  //SSA_FLAG
  #define SPD_NVM_DIMM        13    // JEDEC (place holder)
  #define SPD_UDIMM_ECC       18    // Module type is UDIMM-ECC
#define SPD_SDRAM_BANKS       4     // SDRAM Density and number of internal banks
  #define SPD_1Gb             2     // Total SDRAM Capacity 1 Gigabits
  #define SPD_2Gb             3     // Total SDRAM Capacity 2 Gigabits
  #define SPD_4Gb             4     // Total SDRAM Capacity 4 Gigabits
  #define SPD_8Gb             5     // Total SDRAM Capacity 8 Gigabits
  #define SPD_16Gb            6     // Total SDRAM Capacity 16 Gigabits
  #define SPD_32Gb            7     // Total SDRAM Capacity 32 Gigabits
  #define SPD_64Gb            8     // Total SDRAM Capacity 64 Gigabits
  #define SPD_128Gb           9     // Total SDRAM Capacity 128 Gigabits
#define SPD_SDRAM_ADDR        5     // Number of Row and Column address bits
  #define SPD_ROW_12          0     // 12 row bits
  #define SPD_ROW_13          1     // 13 row bits
  #define SPD_ROW_14          2     // 14 row bits
  #define SPD_ROW_15          3     // 15 row bits
  #define SPD_ROW_16          4     // 16 row bits
  #define SPD_ROW_17          5     // 17 row bits
  #define SPD_ROW_18          6     // 18 row bits
  #define SPD_COL_9           0     // 9 colum bits
  #define SPD_COL_10          1     // 10 colum bits
  #define SPD_COL_11          2     // 11 colum bits
  #define SPD_COL_12          3     // 12 colum bits
#define SPD_VDD_SUPPORT       6     // Vdd DIMM supports
  #define SPD_VDD_150         0     // Module Supports 1.50V
  #define SPD_VDD_135         BIT1  // Module Supports 1.35V
  #define SPD_VDD_125         BIT2  // Module Supports 1.25V
#define SPD_MODULE_ORG        7     // Number of Ranks and SDRAM device width
  #define DEVICE_WIDTH_X4     0     // SDRAM device width = 4 bits
  #define DEVICE_WIDTH_X8     1     // SDRAM device width = 8 bits
  #define DEVICE_WIDTH_X16    2     // SDRAM device width = 16 bits
  #define SPD_NUM_RANKS_1   0
  #define SPD_NUM_RANKS_2   1
  #define SPD_NUM_RANKS_4   3
  #define SPD_NUM_RANKS_8   4
#define SPD_MEM_BUS_WID     8     // Width of SDRAM memory bus
#define SPD_FTB             9     // Timebase for fine grain timing calculations
#define SPD_MTB_DIVEND      10    // Medium Time Base Dividend
#define SPD_MTB_DIVISOR     11    // Medium Time Base Divisor
#define SPD_MIN_TCK         12    // Minimum cycle time (at max CL)
  #define SPD_TCKMIN_800      20    // tCK(MTB)=20, tCK(ns)=2.5
  #define SPD_TCKMIN_1066     15    // tCK(MTB)=15, tCK(ns)=1.875
  #define SPD_TCKMIN_1333     12    // tCK(MTB)=12, tCK(ns)=1.5
  #define SPD_TCKMIN_1600     10    // tCK(MTB)=10, tCK(ns)=1.25
  #define SPD_TCKMIN_1866     9     // tCK(MTB)=9, tCK(ns)=1.07
  #define SPD_TCKMIN_2133     8     // tCK(MTB)=8, tCK(ns)=0.9375
  #define SPD_TCKMIN_2400     7     // tCK(MTB)=7, tCK(ns)=.833
#define SPD_CAS_LT_SUP_LSB  14    // CAS Latencies Supported, Least Significant Byte
#define SPD_CAS_LT_SUP_MSB  15    // CAS Latencies Supported, Most Significant Byte
#define SPD_MIN_TAA         16    // Minimum CAS Latency Time (tAAmin)
#define SPD_MIN_TWR         17    // Minimum Write Recovery Time
#define SPD_MIN_TRCD        18    // Minimum RAS to CAS delay
#define SPD_MIN_TRRD        19    // Minimum Row active to row active delay
#define SPD_MIN_TRP         20    // Minimum Row Precharge time
#define SPD_EXT_TRC_TRAS    21    // Upper nibbles for min tRAS and tRC
#define SPD_MIN_TRAS        22    // Minimum Active to Precharge time
#define SPD_MIN_TRC         23    // Minimum Active to Active/Refresh time
#define SPD_MIN_TRFC_LSB    24    // Minimum Refresh Recovery time least-significant byte
#define SPD_MIN_TRFC_MSB    25    // Minimum Refresh Recovery time most-significant byte
#define SPD_MIN_TWTR        26    // Minimum Internal Write to Read command delay
#define SPD_MIN_TRTP        27    // Minimum Internal Read to Precharge command delay
#define SPD_UN_TFAW         28    // Upper Nibble for tFAW
#define SPD_MIN_TFAW        29    // Minimum Four Activate Window Delay Time (tFAWmin)
#define SPD_OD_SUP          30    // SDRAM Output Drivers Supported
#define SPD_RFSH_OPT        31    // SDRAM Refresh Options
  #define ETR                 BIT0  // Bit location for Extended Temp Range
  #define ETRR                BIT1  // Bit location for Extended Temp Refresh Rate
  #define ASR                 BIT2  // Bit location for Automatic Self Refresh
  #define ODTS                BIT3  // Bit location for On-die Thermal Sensor
#define SPD_DIMM_TS         32    // Module Temperature Sensor
#define SPD_SDRAM_TYPE      33    // SDRAM device type
#define SPD_FTB_TCK         34    // Fine Offset for SDRAM tCK
#define SPD_FTB_TAA         35    // Fine Offset for SDRAM tAA
#define SPD_FTB_TRCD        36    // Fine Offset for SDRAM tRCD
#define SPD_FTB_TRP         37    // Fine Offset for SDRAM tRP
#define SPD_FTB_TRC         38    // Fine Offset for SDRAM tRC
#define SPD_OPT_FEAT        41    // SDRAM Optional Features
  #define SPD_PTRR          BIT7  // Indicates if the DIMM is pTRR compliant

// Begin module specific section
#define SPD_MODULE_NH       60    // Module Nominal Height
#define SPD_MODULE_MT       61    // Module Maximum Thickness
#define SPD_REF_RAW_CARD    62    // Reference Raw Card Used
 #define RAW_CARD_A         0
 #define RAW_CARD_B         1
 #define RAW_CARD_C         2
 #define RAW_CARD_D         3
 #define RAW_CARD_E         4
 #define RAW_CARD_F         5
 #define RAW_CARD_G         6
 #define RAW_CARD_H         7
 #define RAW_CARD_J         8
 #define RAW_CARD_K         9
 #define RAW_CARD_L         10
 #define RAW_CARD_M         11
 #define RAW_CARD_N         12
 #define RAW_CARD_P         13
 #define RAW_CARD_R         14
 #define RAW_CARD_T         15
 #define RAW_CARD_U         16
 #define RAW_CARD_V         17
 #define RAW_CARD_W         18
 #define RAW_CARD_Y         19
 #define RAW_CARD_AA        20
 #define RAW_CARD_AB        21
 #define RAW_CARD_AC        22
 #define RAW_CARD_AD        23
 #define RAW_CARD_AE        24
 #define RAW_CARD_AF        25
 #define RAW_CARD_AG        26
 #define RAW_CARD_AH        27
 #define RAW_CARD_AJ        28
 #define RAW_CARD_AK        29
 #define RAW_CARD_AL        30
 #define RAW_CARD_ZZ        0x1F
 #define RAW_CARD_REV_MASK  (BIT6 + BIT5)
 #define RAW_CARD_EXT       BIT7
 #define RAW_CARD_AM        (RAW_CARD_EXT + 0)
 #define RAW_CARD_AN        (RAW_CARD_EXT + 1)
 #define RAW_CARD_AP        (RAW_CARD_EXT + 2)
 #define RAW_CARD_AR        (RAW_CARD_EXT + 3)
 #define RAW_CARD_AT        (RAW_CARD_EXT + 4)
 #define RAW_CARD_AU        (RAW_CARD_EXT + 5)
 #define RAW_CARD_AV        (RAW_CARD_EXT + 6)
 #define RAW_CARD_AW        (RAW_CARD_EXT + 7)
 #define RAW_CARD_AY        (RAW_CARD_EXT + 8)
 #define RAW_CARD_BA        (RAW_CARD_EXT + 9)
 #define RAW_CARD_BB        (RAW_CARD_EXT + 10)
 #define RAW_CARD_BC        (RAW_CARD_EXT + 11)
 #define RAW_CARD_BD        (RAW_CARD_EXT + 12)
 #define RAW_CARD_BE        (RAW_CARD_EXT + 13)
 #define RAW_CARD_BF        (RAW_CARD_EXT + 14)
 #define RAW_CARD_BG        (RAW_CARD_EXT + 15)
 #define RAW_CARD_BH        (RAW_CARD_EXT + 16)
 #define RAW_CARD_BJ        (RAW_CARD_EXT + 17)
 #define RAW_CARD_BK        (RAW_CARD_EXT + 18)
 #define RAW_CARD_BL        (RAW_CARD_EXT + 19)
 #define RAW_CARD_BM        (RAW_CARD_EXT + 20)
 #define RAW_CARD_BN        (RAW_CARD_EXT + 21)
 #define RAW_CARD_BP        (RAW_CARD_EXT + 22)
 #define RAW_CARD_BR        (RAW_CARD_EXT + 23)
 #define RAW_CARD_BT        (RAW_CARD_EXT + 24)
 #define RAW_CARD_BU        (RAW_CARD_EXT + 25)
 #define RAW_CARD_BV        (RAW_CARD_EXT + 26)
 #define RAW_CARD_BW        (RAW_CARD_EXT + 27)
 #define RAW_CARD_BY        (RAW_CARD_EXT + 28)
 #define RAW_CARD_CA        (RAW_CARD_EXT + 29)
 #define RAW_CARD_CB        (RAW_CARD_EXT + 30)

// UDIMM specific bytes
// Applicable when Module Type (key byte 3) = 2, 3, 4, 6, or 8

#define SPD_ADDR_MAP_FECTD  63    // Address Mapping from Edge Connector to DRAM

// RDIMM specific bytes
// Applicable when Module Type (key byte 3) = 1, 5, or 9

#define SPD_RDIMM_ATTR      63    // RDIMM module attributes
#define SPD_DIMM_HS         64    // Module Heat Spreader Solution
#define SPD_REG_VEN_LSB     65    // Register Vendor ID LSB
#define SPD_REG_VEN_MSB     66    // Register Vendor ID MSB
#define SPD_REG_REV         67    // Register Revision
#define SPD_CNTL_0          69    // Register Control Word 0 & 1
#define SPD_CNTL_1          70    // Register Control Word 2 & 3
#define SPD_CNTL_2          71    // Register Control Word 4 & 5
#define SPD_CNTL_3          72    // Register Control Word 6 & 7   (reserved)
#define SPD_CNTL_4          73    // Register Control Word 8 & 9   (reserved)
#define SPD_CNTL_5          74    // Register Control Word 10 & 11 (reserved)
#define SPD_CNTL_6          75    // Register Control Word 12 & 13 (reserved)
#define SPD_CNTL_7          76    // Register Control Word 14 & 15 (reserved)

// LRDIMM specific bytes
// Applicable when Module Type (key byte 3) = 0xB
// Based on DDR3 SPD 1.0 Document Release 2.1 draft, dated May 27, 2011

#define SPD_LRDIMM_ATTR       63    // LRDIMM module attributes
#define SPD_LRBUF_REV         64    // LR Buffer Revision
#define SPD_LRBUF_VEN_LSB     65    // LR Buffer Vendor ID LSB
#define SPD_LRBUF_VEN_MSB     66    // LR Buffer Vendor ID MSB
#define SPD_LR_F0_RC2_3       67    // LR Buffer Function 0, Control Word 2 & 3
#define SPD_LR_F0_RC4_5       68    // LR Buffer Function 0, Control Word 4 & 5
#define SPD_LR_F1_RC8_11      69    // LR Buffer Function 1, Control Word 8 & 11
#define SPD_LR_F1_RC12_13     70    // LR Buffer Function 1, Control Word 12 & 13
#define SPD_LR_F1_RC14_15     71    // LR Buffer Function 1, Control Word 14 & 15

// Speed bin 0 = 800 & 1066
#define SPD_LR_SB0_MDQ_DS_ODT       72    // LR Buffer Function 3, Control Word 8 & 9
#define SPD_LR_SB0_DR01_QODT_ACT    73    // LR Buffer Function 3 & 4, Control Word 10 & 11
#define SPD_LR_SB0_DR23_QODT_ACT    74    // LR Buffer Function 5 & 6, Control Word 10 & 11
#define SPD_LR_SB0_DR45_QODT_ACT    75    // LR Buffer Function 7 & 8, Control Word 10 & 11
#define SPD_LR_SB0_DR67_QODT_ACT    76    // LR Buffer Function 9 & 10, Control Word 10 & 11
#define SPD_LR_SB0_MR1_2_RTT        77    // LR Buffer SMBus offsets 0xC0 - 0xC7

// Speed bin 1 = 1333 & 1600
#define SPD_LR_SB1_MDQ_DS_ODT       78    // LR Buffer Function 3, Control Word 8 & 9
#define SPD_LR_SB1_DR01_QODT_ACT    79    // LR Buffer Function 3 & 4, Control Word 10 & 11
#define SPD_LR_SB1_DR23_QODT_ACT    80    // LR Buffer Function 5 & 6, Control Word 10 & 11
#define SPD_LR_SB1_DR45_QODT_ACT    81    // LR Buffer Function 7 & 8, Control Word 10 & 11
#define SPD_LR_SB1_DR67_QODT_ACT    82    // LR Buffer Function 9 & 10, Control Word 10 & 11
#define SPD_LR_SB1_MR1_2_RTT        83    // LR Buffer SMBus offsets 0xC0 - 0xC7

// Speed bin 2 = 1866 & 2133
#define SPD_LR_SB2_MDQ_DS_ODT       84    // LR Buffer Function 3, Control Word 8 & 9
#define SPD_LR_SB2_DR01_QODT_ACT    85    // LR Buffer Function 3 & 4, Control Word 10 & 11
#define SPD_LR_SB2_DR23_QODT_ACT    86    // LR Buffer Function 5 & 6, Control Word 10 & 11
#define SPD_LR_SB2_DR45_QODT_ACT    87    // LR Buffer Function 7 & 8, Control Word 10 & 11
#define SPD_LR_SB2_DR67_QODT_ACT    88    // LR Buffer Function 9 & 10, Control Word 10 & 11
#define SPD_LR_SB2_MR1_2_RTT        89    // LR Buffer SMBus offsets 0xC0 - 0xC7

#define SPD_LR_150_MIN_MOD_DELAY    90    // LR DIMM minimum DQ Read propagation delay at 1.5V
#define SPD_LR_150_MAX_MOD_DELAY    91    // LR DIMM maximum DQ Read propagation delay at 1.5V
#define SPD_LR_135_MIN_MOD_DELAY    92    // LR DIMM minimum DQ Read propagation delay at 1.35V
#define SPD_LR_135_MAX_MOD_DELAY    93    // LR DIMM maximum DQ Read propagation delay at 1.35V
#define SPD_LR_12x_MIN_MOD_DELAY    94    // LR DIMM minimum DQ Read propagation delay at 1.2xV
#define SPD_LR_12x_MAX_MOD_DELAY    95    // LR DIMM maximum DQ Read propagation delay at 1.2xV

#define SPD_LR_PERS_BYTE_0          102   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_1          103   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_2          104   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_3          105   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_4          106   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_5          107   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_6          108   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_7          109   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_8          110   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_9          111   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_10         112   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_11         113   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_12         114   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_13         115   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTE_14         116   // LR DIMM Personality Byte
#define SPD_LR_PERS_BYTES_TOTAL     15    // LR DIMM Total number of Personality Bytes

// End module specific section

#define SPD_MMID_LSB        117   // Module Manufacturer ID Code, Least Significant Byte
#define SPD_MMID_MSB        118   // Module Manufacturer ID Code, Mostst Significant Byte
#define SPD_MM_LOC          119   // Module Manufacturing Location
#define SPD_MM_DATE         120   // Module Manufacturing Date 120-121
#define SPD_MODULE_SN       122   // Module Serial Number 122-125
#define SPD_CRC_LSB         126   // LSB of 16-bit CRC
#define SPD_CRC_MSB         127   // MSB of 16-bit CRC

#define SPD_MODULE_PN       128   // Module Part Number 128-145
#define SPD_MODULE_RC       146   // Module Revision Code 146-147
#define SPD_DRAM_MIDC_LSB   148   // DRAM Manufacturer ID Code, Least Significant Byte
#define SPD_DRAM_MIDC_MSB   149   // DRAM Manufacturer ID Code, Most Significant Byte
#ifdef  MEM_NVDIMM_EN
#define SPD_NVDIMM_ID_N     174   // If NVDIMM value will be 'N'
#define SPD_NVDIMM_ID_V     175   // If NVDIMM value will be 'V'
#endif  //MEM_NVDIMM_EN
#define SPD_BYTE_200        200   // Fixed value 0xBE

//
// DDR4 Specific Bytes
//
#define SPD_SDRAM_TYPE_DDR4   6     // Primary SDRAM Device Type (DDR4)
  #define SPD_SIGNAL_LOADING  (BIT0 | BIT1) // Signal Loading Mask
  #define SPD_3DS_TYPE        2     //   Encoding for Single load stack (3DS)
  #define SPD_DIE_COUNT       (BIT4 | BIT5 | BIT6) // Die Count Mask
#define SPD_OPT_FEAT_DDR4     7     // SDRAM Optional Features (DDR4)
  #define SPD_MAC_MASK        BIT0 | BIT1 | BIT2  // Mask for Maximum Active Count field
  #define SPD_TRR_IMMUNE      BIT3  // Indicates this DIMM does not require DRAM Maintenance
#define SPD_RFSH_OPT_DDR4     8     // SDRAM Refresh Options (DDR4)
#define SPD_OTH_OP_FEAT_DDR4  9     // SDRAM Optional features (DDR4)
  #define SPD_PPR_MASK        (BIT7 | BIT6) // Mask for PPR capability
    #define SPD_PPR_NOT_SUP   0     // PPR not supported
    #define SPD_PPR_HARD_1R   BIT6  // Hard PPR with 1 row per BG
  #define SPD_SOFT_PPR        BIT5  // Soft PPR capability
#define SPD_SECONDARY_SDRAM_TYPE_DDR4   10    // Secondary SDRAM Device Type (DDR4)
  #define SPD_HALF_SIZE_SECOND_RANK BIT2
  #define SPD_QUARTER_SIZE_SECOND_RANK BIT3
#define SPD_VDD_DDR4          11    // Vdd DIMM supports (DDR4)
  #define SPD_VDD_120         3     // Module operable and endurant 1.20V
#define SPD_MODULE_ORG_DDR4   12    // Number of Package Ranks and SDRAM device width (DDR4)
#define SPD_MEM_BUS_WID_DDR4  13    // Width of SDRAM memory bus
#define SPD_DIMM_TS_DDR4      14    // Module Thermal Sensor
#define SPD_TB_DDR4           17    // Timebase [3:2] MTB, [1:0] FTB
#define SPD_MIN_TCK_DDR4      18    // Minimum cycle time
  #define SPD_TCKMIN_DDR4_1600      10  // tCK(MTB)=10, tCK(ns)=1.25
  #define SPD_TCKMIN_DDR4_1866      9   // tCK(MTB)=9, tCK(ns)=1.071
  #define SPD_TCKMIN_DDR4_2133      8   // tCK(MTB)=8, tCK(ns)=.938
  #define SPD_TCKMIN_DDR4_2400      7   // tCK(MTB)=7, tCK(ns)=.833
#define SPD_MAX_TCK_DDR4      19    // Maximum cycle time
#define SPD_CAS_LT_SUP_1_DDR4 20    // CAS Latencies Supported, first byte
#define SPD_CAS_LT_SUP_2_DDR4 21    // CAS Latencies Supported, second byte
#define SPD_CAS_LT_SUP_3_DDR4 22    // CAS Latencies Supported, third byte
#define SPD_CAS_LT_SUP_4_DDR4 23    // CAS Latencies Supported, fourth byte
#define SPD_MIN_TAA_DDR4      24    // Minimum CAS Latency Time (tAAmin)
#define SPD_MIN_TRCD_DDR4     25    // Minimum RAS to CAS delay
#define SPD_MIN_TRP_DDR4      26    // Minimum Row Precharge time
#define SPD_EXT_TRC_TRAS_DDR4 27    // Upper nibbles for min tRAS and tRC
#define SPD_MIN_TRAS_DDR4     28    // Minimum Active to Precharge time
#define SPD_MIN_TRC_DDR4      29    // Minimum Active to Active/Refresh time
#define SPD_MIN_TRFC1_LSB_DDR4 30    // Minimum Refresh Recovery time least-significant byte
#define SPD_MIN_TRFC1_MSB_DDR4 31    // Minimum Refresh Recovery time most-significant byte
#define SPD_MIN_TRFC2_LSB_DDR4 32    // Minimum Refresh Recovery time least-significant byte
#define SPD_MIN_TRFC2_MSB_DDR4 33    // Minimum Refresh Recovery time most-significant byte
#define SPD_MIN_TRFC3_LSB_DDR4 34    // Minimum Refresh Recovery time least-significant byte
#define SPD_MIN_TRFC3_MSB_DDR4 35    // Minimum Refresh Recovery time most-significant byte
#define SPD_TFAW_UPPER_DDR4   36    // Upper nibble for tFAW
#define SPD_MIN_TFAW_DDR4     37    // Minimum For Active Window Delay Time (tFAW)
#define SPD_MIN_TRRDS_DDR4    38    // Minimum Active to Active Delay Time tRRD_S Different Bank Group
#define SPD_MIN_TRRDL_DDR4    39    // Minimum Active to Active Delay Time tRRD_L Same Bank Group
#if SMCPKG_SUPPORT
#define SPD_MIN_TCCDL_DDR4    40    // Minimum CAS to CAS Delay Time tCCD_L Same Bank Group
#define SPD_FTB_TCCDL_DDR4    117   // Fine offset for tCCD_L
#endif
#define SPD_FTB_TRRDL_DDR4    118   // Fine offset for tRRD_L
#define SPD_FTB_TRRDS_DDR4    119   // Fine offset for tRRD_S
#define SPD_FTB_TRC_DDR4      120   // Fine offset for TRC
#define SPD_FTB_TRP_DDR4      121   // Fine offset for TRP
#define SPD_FTB_TRCD_DDR4     122   // Fine offset for TRCD
#define SPD_FTB_TAA_DDR4      123   // Fine offset for TAA
#define SPD_FTB_MAX_TCK_DDR4  124   // Fine offset for max TCK
#define SPD_FTB_MIN_TCK_DDR4  125   // Fine offset for min TCK

#define SPD_CRC_LSB_BLOCK1    254   // LSB of 16-bit CRC
#define SPD_CRC_MSB_BLOCK1    255   // MSB of 16-bit CRC

#define SPD_MMID_LSB_DDR4     320   // Module Manufacturer ID Code, Least Significant Byte
#define SPD_MMID_MSB_DDR4     321   // Module Manufacturer ID Code, Most Significant Byte
#define SPD_MM_LOC_DDR4       322   // Module Manufacturing Location
#define SPD_MM_DATE_YR_DDR4   323   // Module Manufacturing Date Year
#define SPD_MM_DATE_WK_DDR4   324   // Module Manufacturing Date Week
#define SPD_MODULE_SN_DDR4    325   // Module Serial Number 325-328
#define SPD_MODULE_PN_DDR4    329   // Module Part Number 329-348
#define SPD_MODULE_RC_DDR4    349   // Module Revision Code
#define SPD_DRAM_MIDC_LSB_DDR4 350  // DRAM Manufacturer ID Code, Least Significant Byte
#define SPD_DRAM_MIDC_MSB_DDR4 351  // DRAM Manufacturer ID Code, Most Significant Byte
#define SPD_DRAM_REV_DDR4     352   // DRAM Revision ID
#define SPD_CRC_LSB_DDR4      382   // LSB of 16-bit CRC
#define SPD_CRC_MSB_DDR4      383   // MSB of 16-bit CRC

// Begin DDR4 module specific section
#define SPD_MODULE_NH_DDR4    128    // Module Nominal Height
#define SPD_MODULE_MT_DDR4    129    // Module Maximum Thickness
#define SPD_REF_RAW_CARD_DDR4 130    // Reference Raw Card Used

// UDIMM specific bytes
// Applicable when Module Type (key byte 3) = 2
#define SPD_ADDR_MAP_FECTD_DDR4 131    // Address Mapping from Edge Connector to DRAM

// RDIMM specific bytes
// Applicable when Module Type (key byte 3) = 1
#define SPD_RDIMM_ATTR_DDR4    131   // RDIMM module attributes
#define SPD_DIMM_HS_DDR4       132   // Module Heat Spreader Solution
#define SPD_REG_VEN_LSB_DDR4   133   // Register Vendor ID LSB
#define SPD_REG_VEN_MSB_DDR4   134   // Register Vendor ID MSB
#define SPD_REG_REV_DDR4       135   // Register Revision
#define SPD_ADD_MAPPING_DDR4   136   // Address mapping from Reg to DRAM
#define SPD_REG_OD_CTL_DDR4    137   // Register Output Drive Strength for Control
#define SPD_REG_OD_CK_DDR4     138   // Register Output Drive Strength for Clock

//DIMM revision types
#define RCD_REV_1 0
#define RCD_REV_2 1

// LRDIMM specific bytes
// Applicable when Module Type (key byte 3) = 0x4
#define SPD_LRDIMM_ATTR_DDR4   131   // LRDIMM module attributes
#define SPD_LRBUF_HS_DDR4      132   // LR Buffer Heat Spreader Solution
#define SPD_LRBUF_VEN_LSB_DDR4 133   // LR Buffer Vendor ID LSB
#define SPD_LRBUF_VEN_MSB_DDR4 134   // LR Buffer Vendor ID MSB
#define SPD_LRBUF_REV_DDR4     135   // LR Buffer Register Revision
#define SPD_LRBUF_ADD_MAP_DDR4 136   // LR Buffer Address Mapping from Register to DRAM
#define SPD_LRBUF_DB_REV_DDR4  139   // LR Buffer Data Buffer Revision
#define SPD_LRBUF_DRAM_VREFDQ_R0_DDR4 140 // LR Buffer DRAM VrefDQ for Package Rank 0
#define SPD_LRBUF_DRAM_VREFDQ_R1_DDR4 141 // LR Buffer DRAM VrefDQ for Package Rank 1
#define SPD_LRBUF_DRAM_VREFDQ_R2_DDR4 142 // LR Buffer DRAM VrefDQ for Package Rank 2
#define SPD_LRBUF_DRAM_VREFDQ_R3_DDR4 143 // LR Buffer DRAM VrefDQ for Package Rank 3
#define SPD_LRBUF_DB_VREFDQ_DDR4 144 // LR Data Buffer VrefDQ for DRAM Interface
#define SPD_LRBUF_DB_DS_RTT_LE1866_DDR4 145 // LR Data Buffer MDQ Drive Strength and RTT for data rate <= 1866
#define SPD_LRBUF_DB_DS_RTT_GT1866_LE2400_DDR4 146 // LR Data Buffer MDQ Drive Strength and RTT for data rate > 1866 and <= 2400
#define SPD_LRBUF_DB_DS_RTT_GT2400_LE3200_DDR4 147 // LR Data Buffer MDQ Drive Strength and RTT for data rate > 2400 and <= 3200
#define SPD_LRBUF_DRAM_DS_DDR4 148   // LR Buffer DRAM Drive Strength (for data rates <= 1866, 1866 < data rate <= 2400, and 2400 < data rate <= 3200)
#define SPD_LRBUF_DRAM_ODT_WR_NOM_LE1866_DDR4 149 // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate <= 1866
#define SPD_LRBUF_DRAM_ODT_WR_NOM_GT1866_LE2400_DDR4 150 // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 1866 and <= 2400
#define SPD_LRBUF_DRAM_ODT_WR_NOM_GT2400_LE3200_DDR4 151 // LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 2400 and <= 3200
#define SPD_LRBUF_DRAM_ODT_PARK_LE1866_DDR4 152 // LR Buffer DRAM ODT (RTT_PARK) for data rate <= 1866
#define SPD_LRBUF_DRAM_ODT_PARK_GT1866_LE2400_DDR4 153 // LR Buffer DRAM ODT (RTT_PARK) for data rate > 1866 and <= 2400
#define SPD_LRBUF_DRAM_ODT_PARK_GT2400_LE3200_DDR4 154 // LR Buffer DRAM ODT (RTT_PARK) for data rate > 2400 and <= 3200

//
// End DDR4 Specific Bytes
//

//
// NVM Dimm Specific Bytes
//

#define SPD_AEP_DEV_DENSITY_R1                      132
#define SPD_AEP_DEV_TYPE_R1                         134
#define SPD_AEP_MOD_ORGANIZATION_R1                 136
#define SPD_AEP_MOD_BUS_WIDTH_R1                    137
#define SPD_AEP_THERMAL_SENSOR_R1                   138
#define SPD_AEP_WRITE_CREDIT_R1                     200
#define SPD_AEP_GNT2ERID_1333_1600_R1               216
#define SPD_AEP_GNT2ERID_1866_2133_R1               217
#define SPD_AEP_GNT2ERID_2400_2666_R1               218
#define SPD_AEP_GNT2ERID_3200_R1                    219

//
// AEP Dimm Specific Bytes Rev 2
//

#define SPD_AEP_DEV_DENSITY                         176
#define SPD_AEP_DEV_TYPE                            178
#define SPD_AEP_MOD_ORGANIZATION                    180
#define SPD_AEP_MOD_BUS_WIDTH                       181
#define SPD_AEP_THERMAL_SENSOR                      182
#define SPD_AEP_LRBUF_DB_VREFDQ                     144
#define SPD_AEP_LRBUF_DS_RTT_LE1866                 145
#define SPD_AEP_LRBUF_DS_RTT_GT1866_LE2400          146
#define SPD_AEP_LRBUF_DS_RTT_GT2400_LE3200          147
#define SPD_AEP_LRBUF_DRAM_DS                       148
#define SPD_AEP_LRBUF_DRAM_ODT_WR_NOM_LE1866        149
#define SPD_AEP_LRBUF_DRAM_ODT_WR_NOM_GT1866_LE2400 150
#define SPD_AEP_LRBUF_DRAM_ODT_WR_NOM_GT2400_LE3200 151
#define SPD_AEP_LRBUF_DRAM_ODT_PARK_LE1866          152
#define SPD_AEP_LRBUF_DRAM_ODT_PARK_GT1866_LE2400   153
#define SPD_AEP_LRBUF_DRAM_ODT_PARK_GT2400_LE3200   154
#define SPD_AEP_BACKSIDE_SWIZZLE_0                  234
#define SPD_AEP_BACKSIDE_SWIZZLE_1                  233
#define SPD_AEP_WRITE_CREDIT                        170
#define SPD_AEP_GNT2ERID_1333_1600                  186
#define SPD_AEP_GNT2ERID_1866_2133                  187
#define SPD_AEP_GNT2ERID_2400_2666                  188
#define SPD_AEP_GNT2ERID_3200                       189
#define SPD_AEP_LSB_FMC_TYPE                        196
#define SPD_AEP_FMC_REV                             198
#define FMC_FNV_TYPE                               0x79
#define FMC_EKV_TYPE                               0x7A
#define FMC_BWV_TYPE                               0x7B

#define SPD_VENDORID_BYTE0 320
#define SPD_VENDORID_BYTE1 321
#define SPD_DEVICEID_BYTE0 192
#define SPD_DEVICEID_BYTE1 193
#define SPD_RID 349
#define SPD_SUBSYSTEM_VENDORID_BYTE0 194
#define SPD_SUBSYSTEM_VENDORID_BYTE1 195
#define SPD_SUBSYSTEM_DEVICEID_BYTE0 196
#define SPD_SUBSYSTEM_DEVICEID_BYTE1 197
#define SPD_SUBSYSTEM_REVISIONID 198

#define SPD_XMP_ID            176 // XMP Id String
#define SPD_XMP_ORG           178 // XMP Organization and Configuration
#define SPD_XMP_REV           179 // XMP Revision
#define SPD_XMP_MTB_DIVEND1   180 // XMP Medium Timebase Dividend Profile 1
#define SPD_XMP_MTB_DIVISOR1  181 // XMP Medium Timebase Divisor Profile 1
#define SPD_XMP_MTB_DIVEND2   182 // XMP Medium Timebase Dividend Profile 2
#define SPD_XMP_MTB_DIVISOR2  183 // XMP Medium Timebase Divisor Profile 2
#define SPD_XMP_VDD           185 // XMP Module VDD Voltage Level
#define SPD_XMP_TCK           186 // XMP Minimum cycle time (at max CL)
#define SPD_XMP_TAA           187 // XMP Minimum CAS Latency Time (tAAmin)
#define SPD_XMP_CAS_SUP_LSB   188 // XMP CAS Latencies Supported, Least Significant Byte
#define SPD_XMP_CAS_SUP_MSB   189 // XMP CAS Latencies Supported, Most Significant Byte
#define SPD_XMP_TCWL          190 // XMP Minimum CAS Write Latency Time(tCWLmin)
#define SPD_XMP_TRP           191 // XMP Minimum Row Precharge time (tRPmin)
#define SPD_XMP_TRCD          192 // XMP Minimum RAS to CAS delay (tRCDmin)
#define SPD_XMP_TWR           193 // XMP Minimum Write Recovery Time
#define SPD_XMP_TRC_TRAS      194 // XMP Upper nibbles for min tRAS and tRC
#define SPD_XMP_TRAS          195 // XMP Minimum Active to Precharge time
#define SPD_XMP_TRC           196 // XMP Minimum Active to Active/Refresh time
#define SPD_XMP_TREFI_LSB     197 // XMP Maximum tREFI Time Least Significant Byte
#define SPD_XMP_TREFI_MSB     198 // XMP Maximum tREFI Time Most Significant Byte
#define SPD_XMP_TRFC_LSB      199 // XMP Minimum Refresh Recovery Delay Time Least Significant Byte
#define SPD_XMP_TRFC_MSB      200 // XMP Minimum Refresh Recovery Delay Time Most Significant Byte
#define SPD_XMP_TRTP          201 // XMP Minimum Internal Read to Precharge command delay
#define SPD_XMP_TRRD          202 // XMP Minimum Row active to row active delay
#define SPD_XMP_UN_TFAW       203 // XMP Upper Nibble for tFAW
#define SPD_XMP_TFAW          204 // XMP Minimum Four Activate Window Delay Time (tFAWmin)
#define SPD_XMP_TWTR          205 // XMP Minimum Internal Write to Read command delay
#define SPD_XMP_CMD           208 // XMP System CMD Rate Mode
#define SPD_XMP_ASR_PERF      209 // XMP SDRAM Auto Self Refresh Performance
#define SPD_XMP_MC_VOLT       210 // XMP Memory Controller Voltage Level
#define SPD_XMP_FTB_TCK       211 // XMP Fine Offset for Minimum Cycle Time (tCK)
#define SPD_XMP_FTB_TAA       212 // XMP Fine Offset for Minimum CAS Latency Time (tAA)
#define SPD_XMP_FTB_TRP       213 // XMP Fine Offset for Minimum Row Precharge Delay Time (tRP)
#define SPD_XMP_FTB_TRCD      214 // XMP Fine Offset for Minimum RAS to CAS Delay Time (tRCD)
#define SPD_XMP_FTB_TRC       215 // XMP Fine Offset for Minimum Active to Active/Refresh Delay Time (tRC)

//
// DDR4 XMP Offsets
//
#define SPD_XMP_ID_DDR4       384 // XMP Id String
#define SPD_XMP_ORG_DDR4      386 // XMP Organization and Configuration
#define SPD_XMP_REV_DDR4      387 // XMP Revision
#define SPD_XMP_TB_PROFILE_1  388 // XMP Timebase for Pofile 1 (MTB of 125ps, and FTP of 1ps)
#define SPD_XMP_TB_PROFILE_2  389 // XMP Timebase for Pofile 2 (MTB of 125ps, and FTP of 1ps)
#define SPD_XMP_VDD_DDR4      393 // XMP Module VDD Voltage Level
#define SPD_XMP_TCK_DDR4      396 // XMP Minimum cycle time (at max CL)
#define SPD_XMP_CAS_SUP_1     397 // XMP CAS Latencies Supported, First Byte
#define SPD_XMP_CAS_SUP_2     398 // XMP CAS Latencies Supported, Second Byte
#define SPD_XMP_CAS_SUP_3     399 // XMP CAS Latencies Supported, Third Byte
#define SPD_XMP_CAS_SUP_4     400 // XMP CAS Latencies Supported, Forth Byte
#define SPD_XMP_TAA_DDR4      401 // XMP Minimum CAS Latency Time (tAAmin)
#define SPD_XMP_TRCD_DDR4     402 // XMP Minimum RAS to CAS delay (tRCDmin)
#define SPD_XMP_TRP_DDR4      403 // XMP Minimum Row Precharge time (tRPmin)
#define SPD_XMP_TRC_TRAS_DDR4 404 // XMP Upper nibbles for min tRAS and tRC
#define SPD_XMP_TRAS_DDR4     405 // XMP Minimum Active to Precharge time
#define SPD_XMP_TRC_DDR4      406 // XMP Minimum Active to Active/Refresh time
#define SPD_XMP_TRFC1_LSB     407 // XMP Minimum Refresh Recovery Delay 1 Time Least Significant Byte
#define SPD_XMP_TRFC1_MSB     408 // XMP Minimum Refresh Recovery Delay 1 Time Most Significant Byte
#define SPD_XMP_TRFC2_LSB     409 // XMP Minimum Refresh Recovery Delay 2 Time Least Significant Byte
#define SPD_XMP_TRFC2_MSB     410 // XMP Minimum Refresh Recovery Delay 2 Time Most Significant Byte
#define SPD_XMP_TRFC4_LSB     411 // XMP Minimum Refresh Recovery Delay 4 Time Least Significant Byte
#define SPD_XMP_TRFC4_MSB     412 // XMP Minimum Refresh Recovery Delay 4 Time Most Significant Byte
#define SPD_XMP_UN_TFAW_DDR4  413 // XMP Upper Nibble for tFAW
#define SPD_XMP_TFAW_DDR4     414 // XMP Minimum Four Activate Window Delay Time (tFAWmin)
#define SPD_XMP_TRRDS_DDR4    415 // XMP Minimum Row Active to Row Active Delay Time, same bank group
#define SPD_XMP_TRRDL_DDR4    416 // XMP Minimum Row Active to Row Active Delay Time, different bank group
#define SPD_XMP_FTB_TRRDL     425 // XMP Fine Offset for Minimum Row Active to Row Active Delay Time, same bank group
#define SPD_XMP_FTB_TRRDS     426 // XMP Fine Offset for Minimum Row Active to Row Active Delay Time, different bank group
#define SPD_SMP_FTB_TRC_DDR4  427 // XMP Fine Offset for Minimum Active/Refresh Delay Time (tRC)
#define SPD_XMP_FTB_TRP_DDR4  428 // XMP Fine Offset for Minimum Row Precharge Delay Time (tRP)
#define SPD_XMP_FTB_TRCD_DDR4 429 // XMP Fine Offset for Minimum RAS to CAS Delay Time (tRCD)
#define SPD_XMP_FTB_TAA_DDR4  430 // XMP Fine Offset for Minimum CAS Latency Time (tAA)
#define SPD_XMP_FTB_TCK_DDR4  431 // XMP Fine Offset for Minimum Cycle Time (tCK)


#define SPD_INTEL_ID          250 // Intel DIMM serial number
#define SPD_INTEL_ID_DDR4     507 // Intel DIMM serial number DDR4

//
// Mobile Platform Memory Module Thermal Sensor C-spec (JC-42.4 November 2004)
//
#define MTS_CAPABILITY  0
#define MTS_CONFIG      1
  #define MTS_CFG_EVENT_MODE  BIT0
  #define MTS_CFG_EVENT_POL   BIT1
  #define MTS_CFG_TCRIT_ONLY  BIT2
  #define MTS_CFG_EVENT_CTRL  BIT3

#define MTS_ALARM_UPPER 2
#define MTS_ALARM_LOWER 3
#define MTS_CRITICAL    4
#define MTS_TEMP        5
#define MTS_MFGID       6
#define MTS_DID_RID     7

#endif // _memregs_h
