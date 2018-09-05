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
 * Copyright 2006 - 2016, Intel Corporation All Rights Reserved.
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

#ifndef _memhostchipcommon_h
#define _memhostchipcommon_h

#include "SysHostChipCommon.h"

#ifndef ASM_INC

// Define the different SKX package Sku's
#define SKX_SKU_LCC 0x0
#define SKX_SKU_HCC 0x2
#define SKX_SKU_XCC 0x3

#define LRDIMM_SUPPORT  1
#define PPR_SUPPORT     1
#define MREP_LOOPCOUNT  1 // core override, HSD 5331466 - cachelines=infinite, loopcount = 1
#define MREP_CACHELINES 0 //

// Define to include core code for Advanced Memtest feature
#define ADV_MEMTEST_SUPPORT  1

#define NODE_TO_SKT(node)       (node / MAX_IMC)
#define SKT_TO_NODE(socket, mc) ((socket << 1) | (mc & BIT0))
#define NODE_TO_MC(node)        (node % MAX_IMC)
#define SKTMC_TO_NODE(socket, mc)  ((socket * MAX_IMC) | (mc % MAX_IMC))
#define NODECH_TO_SKTCH(node, ch)    (((node % MAX_IMC)*MAX_MC_CH) + ch)

//
// MemDisplay
//
#define DISPLAY_RX_VREF_SUPPORT 1  // leave undefined if unsupported

//
// MemTraining
//
#define EYE_HEIGHT_SIM_DEFAULT  20
#define RX_VREF_TUNING          0               // fixed on SKX
#define DESKEW_DELAY            RxDqsPBitDelay  // Set this to either RxDqsBitDelay or RxDqsPBitDelay
#define MIN_EARLYVREF_EYE       6


#define tZQOPER_DDR4 512
#define tZQCS_DDR4 128


//MemoryAttributes
#define INDEP_VOL_REGION_DESC   0
#define PMEM_REGION_DESC        1
#define MIRROR_REGION_DESC      2
#define RESERVED_REGION_DESC    3
#define MMIO_REGION_DESC        4
#define UC_MMIO_REGION_DESC     5

//MemWriteLeveling
#define TX_DQ_DELAY_MAX 447;
#define TX_DQS_DELAY_MIN 64;
//
// Array sizes
//
#define IMC0 0
#define IMC1 1

#ifdef HWSW_CLIENT
#define SPD_ARRAY_SIZE  512
#endif //HWSW_CLIENT

//
//MAX_CHANNELS and DYNVAR_MAX were previously defined in MmrcProjectDefinitionsGenerated.h, but
// now are here and must be manually updated as needed depending on MMRC tool execution (they have been
// removed from automatic generation by the tool)
//
// Channels
//
#define MAX_CHANNELS                                      6

#define DYNVAR_MAX                                        51

#define MAX_IMC             2

#define MAX_MC_CH           3                               // Max channels per MC
#define MAX_CH              ((MAX_IMC)*(MAX_MC_CH))         // Max channels per socket
#define MEM_GRANULARITY  64
#define MC_MAX_NODE         (MAX_SOCKET * MAX_IMC)  // Max number of memory nodes
#define MAX_SYS_CH          (MAX_CH * MAX_SOCKET)   // Max channels in the system
#define MAX_AEP_CH          1
#define MAX_AEP_DIMMS       (MAX_SYS_CH * MAX_AEP_CH) // Max AEP DIMMs in the system
#define MAX_DIMM            2                       // Max DIMM per channel
#define MAX_UNIQUE_NGN_DIMM_INTERLEAVE 2            // Max number of unique interleaves for NGN DIMM
#ifdef SSA_FLAG
#define MAX_SDRAM_IN_DIMM   9                   // Maximum number of SDRAMs per DIMM when ECC is enabled.
#endif //SSA_FLAG
#define MAX_MEM_RISER       2                       // Max memory risers per socket
//SKX hack
#define MAX_DIMM3           3                   // Max DIMM per channel
#define MAX_BITS            72                  // Max number of data bits per rank
#define MAX_TECH            19                  // Number of entries in DRAM technology table
#define MAX_TECH_DDR4       12                  // Number of entries in DRAM technology table
#define MAX_TECH_AEP        8
#define MAX_DENSITY         4
#define MAX_RM              2
#define MAX_RIR             4                   // Number of Rank Interleave Register rules for DDR
#define MAX_RIR_DDRT        4                   // Number of Rank Interleave Register rules for NVMDIMM
#define MAX_RIR_WAYS        8                   // Number of interleave ways for RIR for DDR
#define MAX_RIR_DDRT_WAYS   2                   // Number of interleave ways for RIR for NVMDIMM
#define RIR_UNIT            29                  // RIR limit unit is 512MB aka 2^29
#define TAD_RULES           8                   // Number of TAD rule registers
#define MAX_TAD_WAYS        3                   // Number of interleave ways for TAD RULES
#define SAD_RULES          24                   // Number of SAD rule registers
#define SAD_RULES_10NM     16                   // Number of SAD rule registers in 10NM
#define MAX_SAD_RULES      24                   // Number of SAD rule registers
#define MAX_SAD_WAYS        8                   // Number of interleave ways for SAD RULES
#define SAD_ATTR_DRAM       0                   // DRAM attribute for SAD rule
#define SAD_NXM_ATTR        2                   // Attr value for an NXM SAD rule
#define SAD_UNIT            26                  // SAD limit unit is 64MB aka 2^26
#define MAX_PAD             3                   // Number of PAD registers
#define MAX_RAW_CARD        5                   // Number of supported Raw Cards
#define MAX_STROBE          18                  // Number of strobe groups
#define MAX_SPD_BYTE        256                 // Number of bytes in Serial EEPROM
#define MAX_SPD_BYTE_DDR4   512                 // Number of bytes in Serial EEPROM on DDR4
#define PARITY_INTERLEAVE   1                   // SAD interleave with merged HAs (pseudo-hemisphere mode)
#define MAX_SEEDS           10                  // Maximum
#define NMEM                0
#define FMEM                1
#define RIR_GRAN            512                 // RIR granularity in MB
#define SAD_GRAN            64                  // SAD granularity in MB
#define CRC_ALERT_DDR4      13000               // 13ns or 13k ps

#if   QR_DIMM_SUPPORT
#define MAX_RANK_DIMM       4                   // Max ranks per DIMM
#else
#define MAX_RANK_DIMM       2                   // Max ranks per DIMM
#endif
#define MAX_RANK_CH         8                   // Max ranks per channel
#define MAX_SPARE_RANK      2                   // Max number of spare ranks in a channel
#define MAX_BANK_RANK       16                  // Max banks per rank
#ifdef LRDIMM_SUPPORT
#define MAX_SUBRANK         2                   // Max subranks per logical rank
#endif
#define SPD_MODULE_PART     18                  // Number of bytes of module part - DDR3
#define SPD_MODULE_PART_DDR4 20                 // Number of bytes of module part - DDR4
#define MAX_PB_DESKEW_GROUP 4                   // Max Per-Bit Deskew Groups
#define SAD_RULES_ADDR_RANGE  4                 // Max IOT rules = 4, Total address limits (lower(4) entries each)
#define BITS_PER_NIBBLE 4
#define MAX_PARTIAL_MIRROR      4               //Maximum number of partial mirror regions that can be created

#define CONVERT_64MB_TO_4KB_GRAN  14
#define CONVERT_4KB_TO_64MB_GRAN 14
#define CONVERT_64MB_TO_GB_GRAN 4
#define CONVERT_64B_TO_64MB 20
#define CONVERT_B_TO_64MB 26
#define CONVERT_64MB_TO_B 26
#define CONVERT_64MB_TO_128MB_GRAN 1

#define GB_TO_MB_CONVERSION 1024
#define MAX_2LM_NM 512                           //Maximum near memory size in GB
#define MAX_2LM_NM_IN_MB  MAX_2LM_NM * GB_TO_MB_CONVERSION  //Maximum near memory size in MB
#define MIN_2LM_NM 4                             //Minimum near memory size in GB
#define MIN_2LM_NM_IN_MB MIN_2LM_NM * GB_TO_MB_CONVERSION  // Minimum near memory size in MB
#define  TX_DQ_START_OFFSET       32
#define  TX_DQ_START_OFFSET_2TO2  96

// Ctl FUBs
#define NUM_CTL_PLATFORM_GROUPS 4

// NVM DIMMs
#define MAX_MANUFACTURER_STRLEN 2
#define MAX_SERIALNUMBER_STRLEN 4
#define MAX_MODELNUMBER_STRLEN 20

#define MEM_TYPE_1LM        BIT0
#define MEM_TYPE_2LM        BIT1
#define MEM_TYPE_PMEM       BIT2
#define MEM_TYPE_PMEM_CACHE BIT3
#define MEM_TYPE_BLK_WINDOW BIT4
#define MEM_TYPE_CTRL       BIT5
#define MEM_TYPE_HBM        BIT6
#define MEM_TYPE_DDR4_FM    BIT7

#define MEM_TYPE_HBM_FLAT       (MEM_TYPE_HBM | MEM_TYPE_1LM)
#define MEM_TYPE_HBM_CACHE_DDR4 (MEM_TYPE_HBM | MEM_TYPE_2LM | MEM_TYPE_DDR4_FM)
#define MEM_TYPE_HBM_CACHE_DDRT (MEM_TYPE_HBM | MEM_TYPE_2LM)

// Memory Controller Target Interleave Granularity
#define MEM_CTRL_TGT_INTERLEAVE_64B         0
#define MEM_CTRL_TGT_INTERLEAVE_256B        1
#define MEM_CTRL_TGT_INTERLEAVE_4KB         2
#define MEM_CTRL_TGT_INTERLEAVE_1GB         3

#define BITMAP_CH0_CH1_CH2  ( ( BIT0 ) | (BIT1 ) | (BIT2) )
#define BITMAP_CH0_CH1      ( ( BIT0 ) | (BIT1 ) )
#define BITMAP_CH1_CH2      ( ( BIT1 ) | (BIT2 ) )
#define BITMAP_CH0_CH2      ( ( BIT0 ) | (BIT2 ) )
#define BITMAP_CH0       BIT0
#define BITMAP_CH1       BIT1
#define BITMAP_CH2       BIT2

#define CONVERT_64MB_TO_BYTE              64 * 1024 * 1024


#define MEM_MAP_LVL_NORMAL              0
#define MEM_MAP_LVL_IGNORE_CFGIN        1
#define MEM_MAP_LVL_IGNORE_CFGIN_IGNORE_NEW_DIMMS   2
#define MEM_MAP_VOL_MEM_MODE_UNDEFINED    2


#define MEM_MAP_STATE_RESOURCE_CALCULATION             0
#define MEM_MAP_STATE_RESOURCE_CALCULATION_FAILURE     1
#define MEM_MAP_STATE_RESOURCE_CALCULATION_DONE        2
#define MEM_MAP_STATE_PARTITION_FAILURE                3


//These are the error code that are used to handle Resource unavailability.
//The error codes begin at 0x20 to make sure it doesnt overlap with error codes defined for NGN dimms.
#define ERROR_RESOURCE_CALCULATION_COMPLETED           0x020
#define ERROR_RESOURCE_CALCULATION_FAILURE             0x021

//
// MC egress credit count values
//
#define MC_CREDIT_COUNT_1_CHANNEL_ENABLED    63
#define MC_CREDIT_COUNT_2_CHANNELS_ENABLED   31
#define MC_CREDIT_COUNT_3_CHANNELS_ENABLED   20

//
// SPD Defines
//-----------------------------------------------------------------------------

typedef struct {
  UINT8 chaTgt;
  UINT8 chaCh;
  UINT8 fmCh;
} EncodedInterleaveGranularity ;

typedef struct {
  UINT8 effBusDelaySR;
  UINT8 effBusDelayDR;
  UINT8 effBusDelayDD;
  UINT8 cas2RecEnDR;
  UINT8 cas2RecEnDD;
  UINT8 cas2DrvEnDR;
  UINT8 cas2DrvEnDD;
  UINT8 odtRdLeading;
  UINT8 odtRdTrailing;
  UINT8 odtWrLeading;
  UINT8 odtWrTrailing;
  UINT8 clkDiffDR;
  UINT8 clkDiffDD;
  UINT8 Gnt2Erid;
} TT_VARS, *PTT_VARS;


#define CMD_GRP_DELAY     0
#define CMD_SIGNAL_DELAY  1
#define CMDCTL_SIGNAL_DELAY   2

#define NORMAL_CADB_SETUP     0
#define RESTORE_CADB_PATTERN  1

#define UNMARGINED_CMD_EDGE   511

//Uncomment this line to enabled CA Loopback
//#define CA_LOOPBACK     1
#define NUM_CS                    8
#define NUM_CKE                   4
#define NUM_ODT                   4
#ifdef CA_LOOPBACK
#define NUM_CA_SIGNALS            25
#else
#define NUM_CA_SIGNALS            0
#endif
#define NUM_CTL_SIGNALS           (NUM_CS + NUM_CKE + NUM_ODT)
#define NUM_SIGNALS_TO_SWEEP_LB   (NUM_CTL_SIGNALS + NUM_CA_SIGNALS)

#define CH_BITMASK        0x3F         // For SKX: 0011 1111 (3F) when all channels are present on a socket ..as this is needed outside of BSSA also

//
// Per Bit De-Skew for IVT
//
#define MAX_PER_BIT_DESKEW_RANKS      4

//
// Xover Calib Samples
//

#define XOVER_CALIB_SAMPLE_COUNT      50

#define CAVREF                    64    //Intialize CAVREF for B0
#define MAX_RD_VREF_OFFSET        48
#define MAX_WR_VREF_OFFSET_DDRT   64
#define MAX_WR_VREF_OFFSET        31
#define MAX_WR_VREF_OFFSET_BACKSIDE 50
#define MAX_WR_VREF_OFFSET_BACKSIDE_DDRT 63
#define MAX_TIMING_OFFSET         31
#define MAX_FAN_STEPS             3
#define FAN_RIGHT                 0
#define FAN_LEFT                  1
#define FAN_CENTER                2
#define FAN_ENABLE                1
#define FAN_DISABLE               0

//
// Thermal equates
//
#define DRAM_TCASE_DEFAULT  (85 << 4)   // 85 degrees C
#define DRAM_TCASE_MAX      (95 << 4)   // 95 degrees C

#pragma pack(1)

//
// Platform structures
//
// --------------------------------------------------------------------
// MRC setup structures
// --------------------------------------------------------------------
//

//
// -----------------------------------------------------------------------------
//
// Host enumerated modes
//
#define CH_INDEPENDENT  0
#define FULL_MIRROR_1LM         BIT0
#define FULL_MIRROR_2LM         BIT1
#define CH_LOCKSTEP             BIT2
#define RK_SPARE                BIT3
#define CH_MIRROR               BIT4  // SKX_TODO - this will be deleted when mirror failover handling is implemented
#define PARTIAL_MIRROR_1LM      BIT5
#define PARTIAL_MIRROR_2LM      BIT6
#define STAT_VIRT_LOCKSTEP      BIT7
#define CH_ML                   (FULL_MIRROR_1LM | FULL_MIRROR_2LM | STAT_VIRT_LOCKSTEP)
#define CH_SL                   (RK_SPARE  | STAT_VIRT_LOCKSTEP)
#define CH_MS                   (RK_SPARE  | FULL_MIRROR_1LM | FULL_MIRROR_2LM)
#define CH_MLS                  (FULL_MIRROR_1LM | FULL_MIRROR_2LM | STAT_VIRT_LOCKSTEP | RK_SPARE)
#define CH_ALL_MIRROR           (FULL_MIRROR_1LM | FULL_MIRROR_2LM | PARTIAL_MIRROR_1LM | PARTIAL_MIRROR_2LM)

//
// Host extended RAS Modes
//
#define SDDC_EN             BIT0
#define DMNDSCRB_EN         BIT1
#define PTRLSCRB_EN         BIT2
#define ADDDC_ERR_INJ_EN    BIT3
#define MIRROR_SCRUB_EN     BIT5
#define ADDDC_EN            BIT6

// ADDDC region sizes for Static Virtual LS, ADDC
#define ADDDC_REGION_SIZE_RANK    1
#define ADDDC_REGION_SIZE_BANK    0

#define MEM_RANGE_INVALID              0
#define MEM_RANGE_PERSISTENT           BIT0
#define MEM_RANGE_NON_PERSISTENT       BIT1
#define MEM_RANGE_PART_PERSISTENT      BIT2
#define MEM_RANGE_PART_NON_PERSISTENT  BIT3
#define MEM_RANGE_MIRRORED             BIT4
#define MEM_RANGE_PART_MIRRORED        BIT5
#define MEM_RANGE_PART_NON_MIRRORED    BIT6
#define MEM_RANGE_NON_MIRRORED         BIT7
#define MEM_RANGE_RESERVED             BIT8
#define MEM_RANGE_PART_RESERVED        BIT9
#define MEM_RANGE_CTRL_REGION          BIT10
#define MEM_RANGE_PART_CTRL_REGION     BIT11
#define MEM_RANGE_BLK_WINDOW           BIT12
#define MEM_RANGE_PART_BLK_WINDOW      BIT13
//
// Host bit-mapped memSetup.options
//
//                   ; TEMPHIGH_EN enables support for 95 degree DIMMs
//                   ; PDWN_IBT_OFF_EN enables power down IBT off.
//                   ; PDWN_SR_CKE_MODE enables CKE to be tri-stated during register clock off power down self-refresh
//                   ; OPP_SELF_REF_EN enables the opportunistic self refresh mechanism
//                   ; MDLL_SHUT_DOWN_EN enables MDLL shutdown
//                   ; PAGE_POLICY: Clear for open page, set for closed page. Open page has better performance and
//                   ;  power usage in general. Close page may benefit some applications with poor locality
//                   ; ALLOW2XREF_EN enables 2X refresh if needed for extended operating temperature range (95degrees)
//                   ;  If TEMPHIGH_EN is also set, setting this bit will result in 2X refresh timing for the IMC
//                   ;  refresh control register
//                   ; MULTI_THREAD_MRC_EN enables multithreaded MRC. This reduces boot time for systems with multiple
//                   ;  processor sockets
//                   ; ADAPTIVE_PAGE_EN enables adaptive page mode. The memory controller will dynamically determine how long
//                   ;  to keep pages open to improve performance
//                   ; CMD_CLK_TRAINING_EN enables command to clock training step in BIOS
//                   ; SCRAMBLE_EN Set to enable data scrambling. This should always be enabled except for debug purposes.
//                   ; BANK_XOR_EN enables bank XOR memor mapping mode which is targeted at workloads with bank thrashing
//                   ;  caused by certain stride or page mappings
//                   ; RX_SENSE_AMP_EN < NOT CURRENTLY USED IN FUNCTIONAL CODE >
//                   ; DDR_RESET_LOOP enables infinite channel reset loop without retries for gathering of margin data
//                   ; NUMA_AWARE enables configuring memory interleaving appropriately for NUMA aware OS
//                   ; DISABLE_WMM_OPP_READ disables issuing read commands opportunistically during WMM
//                   ; ECC_CHECK_EN Enables ECC checking
//                   ; ECC_MIX_EN enables ECC in a system with mixed ECC and non-ECC memory in a channel by disabling ECC when
//                   ;  this configuration is detected.
//                   ; BALANCED_4WAY_EN < NOT CURRENTLY USED IN FUNCTIONAL CODE >
//                   ; SPLIT_BELOW_4GB_EN Forces memory address space below 4GB(or TOLM) to be split across CPU sockets when applicable
//                   ; RAS_TO_INDP_EN switches from lockstep or mirror mode to independenct channel mode when memory is present
//                   ;  on channel 2 and this is enabled
//                   ; DISABLE_CHANNEL_EN < NOT CURRENTLY USED IN FUNCTIONAL CODE >
//                   ; MARGIN_RANKS_EN Enables the rank margin tool
//                   ; MEM_OVERRIDE_EN enables use of inputMemTime inputs as hard overrides
//                   ; DRAMDLL_OFF_PD_EN < NOT CURRENTLY USED IN FUNCTIONAL CODE >
//                   ; MEMORY_TEST_EN enables execution of MemTest if on cold boot
//                   ; MEMORY_TEST_FAST_BOOT_EN enables the memory test when going through a fast boot path (fast warm, fast cold, etc)
//                   ; ATTEMPT_FAST_BOOT attempts to take a fast boot path if the NVRAM structure is good and the memory config
//                   ;  hasn't changed.  For example, on a warm boot, this will take the "fast warm" path through MRC which attempts
//                   ;  to make it as close as possible to the S3 path.
//                   ; SW_MEMORY_TEST_EN < NOT CURRENTLY USED IN FUNCTIONAL CODE >
//
#define TEMPHIGH_EN           BIT0
#define ATTEMPT_FAST_BOOT_COLD BIT1
#define PDWN_SR_CKE_MODE      BIT2
#define OPP_SELF_REF_EN       BIT3
#define MDLL_SHUT_DOWN_EN     BIT4
#define PAGE_POLICY           BIT5
#define ALLOW2XREF_EN         BIT6
#define MULTI_THREAD_MRC_EN   BIT7
#define ADAPTIVE_PAGE_EN      BIT8
#define CMD_CLK_TRAINING_EN   BIT9
#define SCRAMBLE_EN           BIT10  // for ddr4
#define SCRAMBLE_EN_DDRT      BIT11
#define DISPLAY_EYE_EN        BIT12
#define DDR_RESET_LOOP        BIT13
#define NUMA_AWARE            BIT14
#define DISABLE_WMM_OPP_READ  BIT15
#define RMT_COLD_FAST_BOOT    BIT16
#define ECC_CHECK_EN          BIT17
#define ECC_MIX_EN            BIT18
#define BALANCED_4WAY_EN      BIT19
#define CA_PARITY_EN          BIT20
#define SPLIT_BELOW_4GB_EN    BIT21
#define PER_NIBBLE_EYE_EN     BIT22
#define RAS_TO_INDP_EN        BIT23
#define DISABLE_CHANNEL_EN    BIT24
#define MARGIN_RANKS_EN       BIT25
#define MEM_OVERRIDE_EN       BIT26
#define DRAMDLL_OFF_PD_EN     BIT27
#define MEMORY_TEST_EN            BIT28
#define MEMORY_TEST_FAST_BOOT_EN  BIT29
#define ATTEMPT_FAST_BOOT         BIT30
#define SW_MEMORY_TEST_EN     BIT31
#define FASTBOOT_MASK         (BIT30 | BIT1)

//
// optionsExt
//
// TODO CORE Remove memflows redunancy in all other projects / setup options via memflows
#define SPD_CRC_CHECK             BIT0
#define RD_VREF_EN                BIT1
#define WR_VREF_EN                BIT2
#define PDA_EN                    BIT5
#define TURNAROUND_OPT_EN         BIT6
#define ROUND_TRIP_LATENCY_EN     BIT7
#define ALLOW_CORRECTABLE_ERROR   BIT8
#define TX_EQ_EN                  BIT10
#define IMODE_EN                  BIT11
#define CMD_NORMAL_EN             BIT12
#define PER_BIT_MARGINS           BIT13
#define DUTY_CYCLE_EN             BIT14
#define LRDIMM_BACKSIDE_VREF_EN   BIT15
#define CMD_VREF_EN               BIT16
#define MC_RON_EN                 BIT18
#define DRAM_RON_EN               BIT19
#define RX_ODT_EN                 BIT20
#define RTT_WR_EN                 BIT21
#define RX_CTLE_TRN_EN            BIT22
#define WR_CRC                    BIT23
#define NON_TGT_ODT_EN            BIT24
#define LRDIMM_WR_VREF_EN         BIT25
#define LRDIMM_RD_VREF_EN         BIT26
#define LRDIMM_TX_DQ_CENTERING    BIT27
#define LRDIMM_RX_DQ_CENTERING    BIT28
#define DIMM_ISOLATION_EN         BIT29

//
// optionsNgn
//
#define LOCK_NGN_CSR        BIT0
#define NGN_CMD_TIME        BIT1
#define NGN_ECC_CORR        BIT2
#define NGN_ECC_WR_CHK      BIT3
#define NGN_ECC_RD_CHK      BIT4
#define CR_MONETIZATION     BIT5
#define NGN_DEBUG_LOCK      BIT6
#define NGN_ARS_ON_BOOT     BIT7

//
// Host bit-mapped Memory Thermal Throttling Related options (mem.thermalThrottlingOptions)
//
#define MH_OUTPUT_EN          BIT0  // Enable for MEM_HOT output generation logic
#define MH_SENSE_EN           BIT1  // Enable for MEM_HOT sense logic
#define OLTT_EN               BIT2  // Enable open loop thermal throttling control
#define CLTT_EN               BIT3  // Enable closed loop thermal throttling control
#define CLTT_PECI_EN          BIT4  // Enable closed loop thermal throttling control w/PECI
#define MEMTRIP_EN            BIT5  // Enable MEMTRIP in ThermTrip tree

#define RDIMM             0
#define UDIMM             1
#define RDIMMandUDIMM     2
#define SODIMM            2
#define LRDIMM            9
#define QRDIMM            10
#define NVMDIMM           11

//
// Flag for B2P ENABLE_DRAM_PM & ENABLE_DRAM_THERMALS
//
#define DISABLE_DRAM_MPM                0
#define ENABLE_DRAM_PM                  BIT0
#define ENABLE_DRAM_THERMAL             BIT1

//
// --------------------------------------------------------------------
// MRC runtime structures
// --------------------------------------------------------------------
//
typedef union {
   struct{
      UINT16 freq:5;      // Frequency enum - DDR3_800, 1066, 1333, 1600, 1866, 2133
      UINT16 rsvd:2;      // Reserved for now
      UINT16 slot0:3;     // DIMM slot 0 enum - Not present, Empty, SR, DR, QR, LR
      UINT16 slot1:3;     // DIMM slot 1 enum - Not present, Empty, SR, DR, QR, LR
      UINT16 slot2:3;     // DIMM slot 2 enum - Not present, Empty, SR, DR, QR, LR
   } Bits;
   UINT16 Data;
} ODT_VALUE_INDEX;

typedef union {
   struct{
      UINT8 rttNom:3;      // Rtt_nom value matching JEDEC spec
      UINT8 rsvd_3:1;
      UINT8 rttWr:2;       // Rtt_park value matching JEDEC spec
      UINT8 rsvd_6:2;
   } Bits;
   UINT8 Data;
} ODT_VALUE_RTT_DDR3;

typedef struct odtValueStruct {
   UINT16 config;                            // ODT_VALUE_INDEX
   UINT8 mcOdt;                              // bit[0] = 0 for 50 ohms; 1 for 100 ohms
   //SKX hack
   UINT8 dramOdt[MAX_DIMM3][MAX_RANK_DIMM];   // ODT_VALUE_RTT_DDR3
} ODT_VALUE_STRUCT,*PODT_VALUE_STRUCT;

typedef union {
   struct{
      UINT16 rttNom:3;      // Rtt_nom value matching JEDEC spec
      UINT16 rttPrk:3;      // Rtt_park value matching JEDEC spec
      UINT16 rttWr:3;       // Rtt_wr value matching JEDEC spec
      UINT16 reserved:7;
   } Bits;
   UINT16 Data;
} ODT_VALUE_RTT_DDR4;

typedef struct ddr4OdtValueStruct {
   UINT16 config;                            // ODT_VALUE_INDEX
   UINT8 mcOdt;                              // bit[0] = 0 for 50 ohms; 1 for 100 ohms
   UINT8 mcVref;                             // Percent Vddq formula by MC spec
                                             // ReadVrefSel = (percent * 192 / Vdd) - 128
   UINT8 dramVref;                           // Percent Vddq as defined by JEDEC spec
   //SKX hack
   UINT16 dramOdt[MAX_DIMM3][MAX_RANK_DIMM];   // ODT_VALUE_RTT_DDR4
}DDR4_ODT_VALUE_STRUCT,**PPDDR4_ODT_VALUE_STRUCT ;

typedef union {
   struct{
      UINT16 dimmNum:2;    // Target DIMM number on the channel
      UINT16 rankNum:2;    // Target Rank number on the DIMM
      UINT16 rsvd:3;
      UINT16 slot0:3;      // DIMM slot 0 enum - Not present, Empty, SR, DR, QR, LR
      UINT16 slot1:3;      // DIMM slot 1 enum - Not present, Empty, SR, DR, QR, LR
      UINT16 slot2:3;      // DIMM slot 2 enum - Not present, Empty, SR, DR, QR, LR
   } Bits;
   UINT16 Data;
} ODT_ACT_INDEX;

typedef struct odtActStruct {
   UINT16 config;          // ODT_ACT_INDEX
   UINT16 actBits[2];      // Bits[3:0]  = D0_R[3:0]
                           // Bits[7:4]  = D1_R[3:0]
                           // Bits[11:8] = D2_R[3:0]
                           // Bit[15] = MCODT
} ODT_ACT_STRUCT, **PPODT_ACT_STRUCT;

//
// Define the WDB line. The WDB line is like the cache line.
//
#define MRC_WDB_LINES                 32
#define MRC_WDB_LINE_SIZE             64
#define MRC_WDB_PATTERN_P             0x55555555
#define MRC_WDB_PATTERN_N             0xAAAAAAAA
#define CADB_LINES                    16
#define NORMAL_MRS                    0           //NORMAL_MRS  - How writeCADBcmd usually works
#define LOAD_MRS                      BIT0        //LOAD_MRS - writeCADBcmd will load commands into cadb buffer but not execute
#define EXECUTE_MRS                   BIT1        //EXECUTE_MRS - writeCADBcmd will execute the stack of commands
#define CADB_CACHELINE_LIMIT          15

#define  MAX_CMD        3
#define  MAX_CLK        4

#define SIDE_A  0
#define SIDE_B  1

//
// Define in the Critical Section function on what to wait.
//
typedef enum {
  DoneAndRefDrained,
  Done,
  Immediate
} EWaitOn;

typedef enum {
  ssOne             = 0,
  ssTwo,
  ssThree,
  ssFour,
} TSubSequencesNumber;

//
// Module definitions
//
#define RE_LOGIC_DELAY_MAX_VALUE        2   // define receive enable logic delay max value
#define RE_EDGE_SIZE_THRESHOLD          3   // define the min of valid one edge size
#define RE_MAX_LOGIC_DELAY              7   // max logic delay value
#define RE_TRAINING_RESULT_HIGH_IN_BITS 32  // bit start of the training result
#define MAX_PHASE_IN_FINE_ADJUSTMENT    64
#define MAX_PHASE_IN_READ_ADJUSTMENT    72
#define MAX_PHASE_IN_READ_ADJUSTMENT_DQ 120 // larger range for added DQ 1/16 PI adjustments
#define RAS_ROW_BITS                    6   // (16 row bits) define the number of row bits from RAS command
#define RAS_LR_ROW_BITS                 1   // (18 row bits) define the number of row bits from RAS command
#define CAS_ROW_BITS                    0   // (10 column bits) for CAS command

#define TX_LOGIC_DELAY_MAX_VALUE        2   // define write max logic delay
#define ALL_CH                          0xFF
#define ALL_STROBES                     0xFF
#define ALL_BITS                        0xFF
#define MRC_ROUND_TRIP_MAX_SCR_B0_VALUE 47  // MAX Round Trip value scrambler can handle - Fixed in C0 (with Throttling)
#ifdef LRDIMM_SUPPORT
#define RE_LRDIMM_EDGE_SIZE_THRESHOLD_PHASE   32
#define RE_LRDIMM_EDGE_SIZE_THRESHOLD         0
#endif // LRDIMM

//SKX change
#define  TX_PER_BIT_SETTLE_TIME         0


#define   REC_EN_STEP_SIZE  1
#define   REC_EN_LOOP_COUNT 6
#define   REC_EN_PI_START   0
#define   REC_EN_PI_RANGE   128

#define   LRDIMM_BACKSIDE_PI_START   0
#define   LRDIMM_BACKSIDE_PI_RANGE   128
#define   LRDIMM_BACKSIDE_STEP_SIZE  2
#define   LRDIMM_BACKSIDE_READ_DELAY_START   -15
#define   LRDIMM_BACKSIDE_READ_DELAY_END      16
#define   LRDIMM_BACKSIDE_READ_DELAY_STEP_SIZE  1

#define   LRDIMM_BACKSIDE_WRITE_DELAY_START   -15
#define   LRDIMM_BACKSIDE_WRITE_DELAY_END      16
#define   LRDIMM_BACKSIDE_WRITE_DELAY_STEP_SIZE  1

#define   WR_LVL_STEP_SIZE  1
#define   WR_LVL_LOOP_COUNT 1
#define   WR_LVL_PI_START   128
#define   WR_LVL_PI_RANGE   128

#define   WR_DQ_STEP_SIZE  1
#define   WR_DQ_LOOP_COUNT 1
#define   WR_DQ_PI_START   0
#define   WR_DQ_PI_RANGE   128
//
// define the WDB line size.
//
#define MRC_WDB_BYTES_PER_TRANSFER    8
#define MRC_WDB_TRANSFERS_PER_LINE    8

#define SUB_SEQUENCES_NUMBER  4
#ifdef SSA_FLAG
#define MRC_WDB_NUM_MUX_SEEDS   3  // Number of WDB Mux
#define MRC_CADB_NUM_MUX_SEEDS  4  // Number of CADB Mux
#endif  //SSA_FLAG

#pragma pack()

#define CMD_STEP_SIZE         1
#define EARLY_CMD_STEP_SIZE   2
#define CTL_CLK_STEPSIZE      2

#define CTL_CLK_TEST_RANGE  256

#define MAX_CMD_MARGIN  255

#define PAR_TEST_START  0
#define PAR_TEST_RANGE  256
#define MAX_SAMPLES     128

// CMD Normalization
#define DCLKPITICKS 128
#define PITICKRANGE 511
#define EXTRAPIMARGIN 32
#define DENORMALIZE       -1
#define RENORMALIZE        1

//
// Timing mode Equates
//
#define TIMING_1N             0
#define TIMING_2N             2
#define TIMING_3N             3

#define MAX_GROUPS            4

#ifdef DEBUG_TURNAROUNDS

  #define TA_POR_T_RRDR     0
  #define TA_POR_T_RRDD     1
  #define TA_POR_T_WWDR     2
  #define TA_POR_T_WWDD     3
  #define TA_POR_T_WRDR     4
  #define TA_POR_T_WRDD     5
  #define TA_POR_T_RWSR     6
  #define TA_POR_T_RWDR     7
  #define TA_POR_T_RWDD     8
  #define TA_POR_T_CCD      9
  #define TA_POR_T_EODT     10
  #define TA_POR_TYPES      11     // Number of timing parameters

#endif  // DEBUG_TURNAROUNDS
//
// defines for Receive enable
//
#define   RT_GUARDBAND                4
#define   DECREMENT_ROUNDTRIP_VALUE   2 // DCLK
#define   FINE_DELAY_FEEDBACK_MSK     0x1FF
#define   RESULT_STROBE_INDEX         MAX_STROBE


//
// defines for mem ODT
//
#define   DDR_FREQ_LIMIT     DDR_2933

#define SKX_PCKG_TYPE 4 //CMD_CTL_DELAY_H

#define CHIP_IOGPDLY_PSECS SKX_PCKG_TYPE

// MemTiming.c

#endif // ASM_INC

// MemTraining.c
#define RD_WR_CMD_INIT_MARG 5
#define RX_TX_DQ_INIT_MARG  5

#pragma pack(1)
struct TADTable {
  UINT8   Enable;         // Rule enable
  UINT8   SADId;          // SAD Index
  UINT8   mode;           // Interleave Mode
  UINT8   socketWays;     // Socket Interleave ways for TAD
  UINT8   channelWays;    // Channel Interleave ways for TAD
  UINT32  Limit;          // Limit of the current TAD entry
  UINT32  ChannelAddressLow;  // Base channel address (in channel address space)
  UINT32  ChannelAddressHigh; // Base channel address (in channel address space)
};

struct SADTable {
  UINT8   Enable;         // Rule enable
  UINT8   type;           // Bit0: 1LM  Bit1: 2LM  Bit2: PMEM  Bit3: PMEM-cache  Bit4: BLK Window  Bit5: CSR/Mailbox/Ctrl region
  UINT8   granularity;    // Interleave granularities for current SAD entry.  Possible interleave granularity options depend on the SAD entry type.  Note that SAD entry type BLK Window and CSR/Mailbox/Ctrl region do not support any granularity options
  UINT32  Limit;          // Limit of the current SAD entry
  UINT8   ways;           // Interleave ways for SAD
  UINT8   channelInterBitmap[MAX_IMC];   //Bit map to denote which DDR4/NM channels are interleaved per IMC eg: 111b - Ch 2,1 & 0 are interleaved; 011b denotes Ch1 & 0 are interleaved
  UINT8   FMchannelInterBitmap[MAX_IMC];   //Bit map to denote which FM channels are interleaved per IMC eg: 111b - Ch 2,1 & 0 are interleaved; 011b denotes Ch1 & 0 are interleaved
  UINT8   imcInterBitmap;       //Bit map to denote which IMCs are interleaved from this socket.
  BOOLEAN local;          //0 - Remote 1- Local
  UINT8   IotEnabled;     // To indicate if IOT is enabled
  UINT8   mirrored;       //To Indicate the SAD is mirrored while enabling partial mirroring
  UINT8   Attr;
  UINT8   tgtGranularity;  // Target granularity
};

struct IMC {
  UINT8             imcEnabled[MAX_IMC];
  UINT8             imcNum;                                  // imc Number
  UINT16            memSize;                                // DDR4 memory size for this imc (64MB granularity)
#ifdef NVMEM_FEATURE_EN
  UINT16            NVmemSize;                              // NV Memory size of this ha
#endif
  UINT16            volSize;                                // Volatile size of the NVM dimms for this imc (64MB granularity)
  UINT16            NonVolSize;                             // Non-Volatile size of the NVM DIMMs for this iMC (64MB granularity)
  UINT16            perSize;                                // Persistent size of the NVM dimms for this imc (64MB granularity)
  UINT16            blkSize;                                // Block size of the NVM dimms for this imc (64MB granularity)
  UINT8             TADintList[TAD_RULES][MAX_TAD_WAYS];    // TAD interleave list for this socket
  UINT8             TADChnIndex[TAD_RULES][MAX_TAD_WAYS];   // Corresponding TAD channel indexes (per channel)
  UINT32            TADOffset[TAD_RULES][MAX_TAD_WAYS];     // Corresponding TAD offsets (per channel)
  struct TADTable   TAD[TAD_RULES];                         // TAD table
  UINT8             imcChannelListStartIndex;                // Index in channel list of first channel on this imc
};

//
// NGN DIMM Configuration Current Table
//
typedef struct {
  NGN_DIMM_PLATFORM_CFG_CURRENT_BODY_HOST body;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST interleave[MAX_UNIQUE_NGN_DIMM_INTERLEAVE];
  NGN_DIMM_INTERLEAVE_ID_HOST interleaveId[MAX_UNIQUE_NGN_DIMM_INTERLEAVE][MAX_CH*MAX_SOCKET];
} NGN_DIMM_CFG_CUR_HOST ;

//
// NGN DIMM Configuration Request Table
//
typedef struct {
  NGN_DIMM_PLATFORM_CFG_INPUT_BODY_HOST body;
  NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT_HOST partitionSize;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST interleave[MAX_UNIQUE_NGN_DIMM_INTERLEAVE];
  NGN_DIMM_INTERLEAVE_ID_HOST interleaveId[MAX_UNIQUE_NGN_DIMM_INTERLEAVE][MAX_CH*MAX_SOCKET];
} NGN_DIMM_CFG_REQ_HOST ;

//
// NGN DIMM Configuration Output Table
//
typedef struct {
  NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY_HOST body;
  NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT_HOST partitionSize;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST interleave[MAX_UNIQUE_NGN_DIMM_INTERLEAVE];
  NGN_DIMM_INTERLEAVE_ID_HOST interleaveId[MAX_UNIQUE_NGN_DIMM_INTERLEAVE][MAX_CH*MAX_SOCKET];
} NGN_DIMM_CFG_OUT_HOST ;

struct firmwareRev {
  UINT8  majorVersion;
  UINT8  minorVersion;
  UINT8  hotfixVersion;
  UINT16 buildVersion;
};

struct  prevBootDimmList {
  // NVM DIMM Information - Valid only if NVM Dimm Present
  UINT8           manufacturer[NGN_MAX_MANUFACTURER_STRLEN];  // Manufacturer
  UINT8           serialNumber[NGN_MAX_SERIALNUMBER_STRLEN];  // Serial Number
  UINT8           modelNumber[NGN_MAX_MODELNUMBER_STRLEN];    //Model Number
  UINT16          VendorID;           // Vendor ID for NVMDIMM Mgmt driver thru Nfit
  UINT16          DeviceID;           // Device ID for NVMDIMM Mgmt driver thru Nfit
  UINT16          rawCap;             // Raw Capacity
  UINT16          volCap;             // Volatile Capacity (2LM region)
  UINT16          nonVolCap;          // NonVolatile Capacity (PMEM/PMEM$ region + Blk region)
  UINT16          perCap;             // Persistent Capcity (PMEM/PMEM$). This size is not obtained from FNV. This is derived data.
  UINT16          blkCap;             // Block Capcity (BLK) This size is not obtained from FNV. This is derived data.
  UINT32          volRegionDPA;       // DPA start address of 2LM Region
  UINT32          perRegionDPA;       // DPA start address of PMEM Region
  struct firmwareRev firmwareRevision; // firmware Revision
};

struct  prevBootChannelList
{
  struct  prevBootDimmList dimmList[MAX_DIMM];
};

struct  prevBootSocket
{
  struct  prevBootChannelList channelList[MAX_CH];
};

struct prevBootNGNDimmCfg
{
  struct  prevBootSocket  socket[MAX_SOCKET];
};

struct dfxMemVars
{
  UINT8                 dfxDimmManagement;
  UINT8                 dfxPartitionDDRTDimm;
  UINT8                 dfxPartitionRatio[MAX_SOCKET * MAX_IMC];
  UINT8                 dfxMemInterleaveGranPMemNUMA;
  UINT8                 dfxMemInterleaveGranPMemUMA;
  UINT8                 dfxCfgMask2LM;
  UINT8                 dfxPerMemMode;                      // 0: Non Persistent Mode 1: Persistent Mode 2: Persistent mode Enable
  UINT8                 dfxHighAddrBitStart;                // valid range is 33-45
};

// HIGH_ADDR_EN enables extention of the MMIO hole to force memory to high address region
//#define HIGH_ADDR_EN        BIT0

#pragma pack()

//
// Chip specific for now until this parameter gets optimized
//
#define MAX_ODT_SETTINGS  20

//NGN error codes
#define NGN_CFG_CUR_REVISION_MISMATCH                 BIT0
#define NGN_CFG_CUR_CHECKSUM_FAILURE                  BIT1
#define NGN_CFG_CUR_INTERLEAVE_FAILURE                BIT2
#define NGN_CFG_REQ_REVISION_MISMATCH                 BIT4
#define NGN_CFG_REQ_CHECKSUM_FAILURE                  BIT5
#define NGN_CFG_REQ_INTERLEAVE_FAILURE                BIT6

//
// Chip specific section of struct memSetup
//
#define MEMSETUP_CHIP     \
  UINT8                 volMemMode;                      /* 0: 1LM  1: 2LM */                                                      \
  UINT8                 memInterleaveGran1LM;            /* Memory interleave mode for 1LM */                                      \
  UINT8                 memInterleaveGran2LM;            /* Memory interleave mode for 2LM */                                      \
  UINT8                 DdrtMemPwrSave;                                                                                            \
  UINT8                 pagetablealias;                  /* if set to 0, page table aliasing will occur on bit 1 else bit 2 */     \
  UINT8                 patrolScrubAddrMode;                                                                                       \
  UINT8                 memRankSparing;                  /* Memory Rank sparing Enable/Disable Map */                              \
  UINT8                 memMirroringMode;                /* Memory Mirroring mode */                                               \
  UINT8                 memMirorEnableMap;               /* Memory mirroring Enable/Disable Map */                                 \
  UINT16                memPartialMirrSize[MAX_SOCKET];  /* Partial Mirror size in 64MB Units */                                   \
  UINT32                migrationSpareMask;                                                                                        \
  UINT8                 SrefProgramming;                                                                                           \
  UINT8                 OppSrefEn;                                                                                                 \
  UINT8                 DdrtSrefEn;                                                                                                \
  UINT8                 MdllOffEn;                                                                                                 \
  UINT8                 PkgcSrefEn;                                                                                                \
  UINT8                 CkMode;                                                                                                    \
  UINT8                 CkeProgramming;                                                                                            \
  UINT8                 CkeIdleTimer;                                                                                              \
  UINT8                 ApdEn;                                                                                                     \
  UINT8                 PpdEn;                                                                                                     \
  UINT8                 DdrtCkeEn;                                                                                                 \
  UINT8                 ExtendedADDDCEn;                                                                                           \
  UINT8                 Blockgnt2cmd1cyc;                                                                                          \
  UINT8                 Disddrtopprd;                                                                                              \
  UINT8                 setTDPDIMMPower;                                                                                           \
  UINT8                 setSecureEraseAllDIMMs;                                                                                    \
  UINT8                 setSecureEraseSktCh[MAX_SOCKET][MAX_CH];                                                                   \
  UINT8                 crQosConfig;

#define SOCKETSETUP_CHIP   \

//
// Chip specific section of struct memVar
//
#define MEMVAR_CHIP     \
  UINT8                   volMemMode;           /* 0: 1LM  1:2LM 2:Undefined */                                           \
  UINT8                   imcInter;             /* Number of ways to interleave imc (1 or 2) */                           \
  UINT8                   maxIMC;                                                                                         \
  UINT32                  mmiohBase;            /* MMIOH base in 64MB granularity*/                                       \
  UINT32                  batteryBackedMem;     /* Amount of Battery Backed Memory in the system in 64MB units */         \
  UINT32                  nonBatteryBackedMem;  /* Amount of Non Battery Backed Memory in the system in 64MB units */     \
  UINT8                   hotPlugMode;          /* 1 = HOTPLUG OPERATION, 0 = NORMAL BOOT */                              \
  UINT8                   memNodeHP;            /* if hotPlugMode = 1, memNodeHP = hotplug nodeId */                      \
  UINT8                   sadEntry;             /* if hotPlugMode = 1, use this SAD */                                    \
  UINT8                   actionHP;             /* if hotPlugMode = 1, operation = MEM_NODE_INIT | MEM_MAP_UPDATE */      \
  UINT32                  milestoneHP;                                                                                    \
  UINT32                  memRiserPresent;      /* Bit mask of memory Riser populated on the system */                    \
  BOOLEAN                 EnableParallelTraining;                                                                         \
  BOOLEAN                 ExecuteThisRoutineInParallel;                                                                   \
  UINT8                   read2tckCL[MAX_CH];       /* increases to CL or CWL based on TCLK preamble */                   \
  UINT8                   write2tckCWL[MAX_CH];                                                                           \
  UINT8                   memInterleaveGran1LM;                                                                           \
  UINT8                   memInterleaveGran2LM;                                                                           \
  struct dfxMemVars       dfxMemVars;                                                                                     \
  UINT8                   setTDPDIMMPower;                                                                                \
  UINT8                   setSecureEraseAllDIMMs;                                                                         \
  UINT8                   setSecureEraseSktCh[MAX_SOCKET][MAX_CH];                                                        \
  UINT8                   ioInitdone[MAX_SOCKET];                                                                         \

//
// Chip specific section of struct Socket
//
#define SOCKET_CHIP     \
  UINT16            volSize;      /* Volatile size of the NVM dimms for this socket (64MB granularity) */                \
  UINT16            perSize;      /* Persistent size of the NVM dimms for this socket (64MB granularity) */              \
  UINT16            blkSize;      /* Block size of the NVM dimms for this socket (64MB granularity) */                   \
  struct SADTable   SAD[MAX_SAD_RULES];                                                                                  \
  UINT8             SADintList[SAD_RULES][MC_MAX_NODE];  /* SAD interleave list */                                       \
  struct IMC        imc[MAX_IMC];                                                                                        \
  UINT8             DdrtTrainingDone;                                                                                    \
  UINT8             fnvMbReady;                                                                                          \
  UINT8             fnvMediaReady;                                                                                       \
  UINT8             ddrClkData;                                                                                          \
  UINT8             ddrClkType;                                                                                          \
  UINT8             ddrFreqCheckFlag;                                                                                    \
  UINT8             maxFreq;                                                                                             \
  UINT8             clkSwapFixDis;   /* HSD4929953 */

//
// Chip specific section of struct ddrChannel
//
#define DDRCHANNEL_CHIP     \
  UINT16            nvMemSize;                                                                                           \
  UINT32            transientErrs;                                                                                       \


//
// Chip specific section of struct dimmDevice
//
#define DIMMDEVICE_CHIP     \
  UINT16                    volSize;    /* Volatile size for this NVM DIMM (64MB granularity) */                         \
  BOOLEAN                   ngnCfgCurPresent;                                                                            \
  BOOLEAN                   ngnCfgCurValid;                                                                              \
  NGN_DIMM_CFG_CUR_HOST     ngnCfgCur;                                                                                   \
  BOOLEAN                   ngnCfgReqPresent;                                                                            \
  BOOLEAN                   ngnCfgReqValid;                                                                              \
  NGN_DIMM_CFG_REQ_HOST     ngnCfgReq;                                                                                   \
  UINT8                     ngnErrorBitmap;   /* BIT0: CfgCur Revision Mismatch  BIT1: CfgCur Checksum Failure  BIT2: Too many Interleave in CfgCur  BIT4: CfgReq Revision Mismatch  BIT5: CfgReq Checksum Failure  BIT6: Too many Interleave in CfgReq */  \
  BOOLEAN                   ngnCfgOutPresent;                                                                            \
  NGN_DIMM_CFG_OUT_HOST     ngnCfgOut;                                                                                   \
  BOOLEAN                   newDimm;    /* Variable to identify if this dimm is newly populated in this boot. */         \

//
// Chip specific section of struct rankDevice
//
#define RANKDEVICE_CHIP     \

//
// Chip specific section of struct socketNvram
//
#define SOCKETNVRAM_CHIP     \
  UINTX               DynamicVars[MAX_CHANNELS][DYNVAR_MAX];              \
  UINT8               ddrtFreq;                                           \
  UINT8               cmdVrefTrainingDone;                                \
  UINT8               mcpPresent;

//
// Chip specific section of struct socketNvram
//
#define MEMNVRAM_CHIP        \

//
// Chip specific section of struct imcNvram
//
#define IMCNVRAM_CHIP     \
  UINT8               imcNodeId;               /*  System wide socket id for imc */                                 \
  UINT32              dimmVrefControl1;

//
// Chip specific section of struct channelNvram
//
#define CHANNELNVRAM_CHIP     \
  UINT8             sxpChFailed;               /* NVM memory in this channel had failed (MFO) */                   \
  UINT32            trainingMode[MAX_STROBE];  /* change for upper nibble */                                       \
  UINT32            ddrCRCmdControls3CmdN;                                                                         \
  UINT32            ddrCRCmdControls3CmdS;                                                                         \
  UINT32            ddrCRCmdControls3Ctl;                                                                          \
  UINT32            ddrCRCmdControls3Cke;                                                                          \
  UINT16            CurrentPlatform;                                                                               \
  UINT16            CurrentFrequency;                                                                              \
  UINT16            CurrentConfiguration;                                                                          \
  UINT16            CurrentDdrType;                                                                                \
  UINT32            ddrtDimm0BasicTiming;                                                                          \
  UINT32            ddrtDimm1BasicTiming;                                                                          \
  BUS_TYPE          fnvAccessMode;                                                                                 \
  UINT8             commandLatencyAddr;                                                                            \
  UINT8             aepWrCreditLimit;                                                                              \
  UINT32            fnvDqSwzRx[5];                                                                                 \
  UINT32            fnvDqSwzTx[5];                                                                                 \
  UINT32            cmdCsr[MAX_CMD_CSR];                                                                           \
  UINT32            cadbMuxPattern[MAX_CH];                                                                        \
  UINT8             twoXRefreshSetPerCh;  /* Saves the refreshRate value for each channel */                       \
  UINT8             EccModeMcDecs[MAX_RANK_CH];                                                                    \
  UINT8             EccModeMcMain[MAX_RANK_CH];                                                                    \
  UINT32            Plus1Failover[MAX_RANK_CH];                                                                    \
  UINT32            LinkRetryErrLimits;                                                                            \
  UINT32            LinkLinkFail;

//
// Chip specific section of struct dimmNvram
//
#define DIMMNVRAM_CHIP     \
  UINT8           manufacturer[NGN_MAX_MANUFACTURER_STRLEN];  /* Manufacturer */                                   \
  UINT8           serialNumber[NGN_MAX_SERIALNUMBER_STRLEN];  /* Serial Number */                                  \
  UINT8           modelNumber[NGN_MAX_MODELNUMBER_STRLEN]; /* Model Number */                                      \
  UINT16          VendorID;           /* Vendor ID for NVMDIMM Mgmt driver thru Nfit */                                 \
  UINT16          DeviceID;           /* Device ID for NVMDIMM Mgmt driver thru Nfit */                                 \
  UINT16          volCap;             /* Volatile Capacity (2LM region) */                                         \
  UINT16          nonVolCap;          /* NonVolatile Capacity (PMEM/PMEM$ region + Blk region) */                  \
  UINT16          perCap;             /* Persistent Capcity (PMEM/PMEM$). This size is not obtained from FNV. This is derived data. */  \
  UINT16          blkCap;             /* Block Capcity (BLK) This size is not obtained from FNV. This is derived data. */               \
  UINT32          volRegionDPA;       /* DPA start address of 2LM Region */                                        \
  UINT32          perRegionDPA;       /* DPA start address of PMEM Region */                                       \
  struct firmwareRev firmwareRevision; /* Firmware Revision */                                                     \
  UINT8           ApiVersion;         /* APTIOV_SERVER_OVERRIDE_RC */                                              \
  UINT8           dimmSku;            /* Dimm SKU info Bit0:MemorymodeEnabled 1: StoragemodeEnabled 2:AppDirectmodeEnabled 3:DieSparingCapable 4:SoftProgrammableSKU 5:MemorymodeEncryptionEnabled 6:AppDirectmodeEncryptionEnabled 7:StoragemodeEncrytionEnabled */    \
  UINT8           gnt2ERID;                                                                                        \
  UINT8           TDP;                /* Total dissipated power limit */                                           \
  UINT16          InterfaceFormatCode;                                                                             \
  UINT16          RevisionID;                                                                                      \
  UINT16          subSysVendorID;                                                                                  \
  UINT16          subSysDeviceID;                                                                                  \
  UINT16          subSysRevisionID;                                                                                \
  UINT16          VendorIDIdentifyDIMM;                                                                            \
  UINT16          DeviceIDIdentifyDIMM;                                                                            \
//
// Chip specific section of struct ddrRank
//
#define DDRRANK_CHIP     \

//
// Chip specific section of struct comTime
//
#define COMTIME_CHIP     \

//
// Chip specific section of the struct CPU_CSR_ACCESS_VAR
//
#define CPU_CSR_ACCESS_VAR_CHIP   \


//
// Power training defines
//
#define MAX_DRAMRON     3
#define MAX_MCODT       2
#define MAX_NONTGTODT   9
#define MAX_RTTWR       4
#define MAX_MCRON       3
#define MAX_TX_EQ       15
#define MAX_IMODE       8
#define MAX_CTLE        5
#define MAX_TCO_COMP    127
#define MIN_TCO_COMP    64

typedef struct _CHIP_ELY_CMD_CK_STRUCT
{
  INT16                               le[MAX_CH];
  INT16                               re[MAX_CH];
  DUMMY_REG                           tcdbp;
  DUMMY_REG                           trainingEn;
}CHIP_ERLY_CMD_CK_STRUCT, *PCHIP_ERLY_CMD_CK_STRUCT;

typedef struct _CHIP_GET_CMD_MGN_SWP_STRUCT
{
  UINT16              eyeSize[MAX_CH];
  UINT8               chEyemask;
  UINT8               chDonemask;
}CHIP_GET_CMD_MGN_SWP_STRUCT, *PCHIP_GET_CMD_MGN_SWP_STRUCT;

typedef struct _CHIP_PARITY_CHECKING_STRUCT
{
  UINT32                                    tCrapOrg;
  DUMMY_REG                                 tCrap;
  DUMMY_REG                                 ddrCRCtlControls;
  DUMMY_REG                                 ddr4cactl;
  DUMMY_REG                                 cmdParInjCtl;
  DUMMY_REG                                 mcSchedChknBit;
  UINT8                                     timingModeOrg;
}CHIP_PARITY_CHECKING_STRUCT, *PCHIP_PARITY_CHECKING_STRUCT;

typedef struct _CHIP_SET_NORMAL_CMD_TIMING_STRUCT
{
  DUMMY_REG                           tCrap;
  DUMMY_REG                           ddrtMiscDelay;
  UINT8                               timingMode;
}CHIP_SET_NORMAL_CMD_TIMING_STRUCT, *PCHIP_SET_NORMAL_CMD_TIMING_STRUCT;

typedef struct
{
  DUMMY_REG                           cpgcMiscODTCtl;
  DUMMY_REG                           dataControl0;
}EXECUTE_CTL_CLK_TEST_CHIP_STRUCT, *PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT;

typedef struct chipstruct
{
  DUMMY_REG                            dataControl0;
  DUMMY_REG                            dataControl0_1; //KV3
  DUMMY_REG                            dataControl2;

}CHIP_DDRIO_STRUCT, *PCHIP_DDRIO_STRUCT;

#define FLY_BY_CACHELINES   4
#define WR_FLY_BY_PATTERN   0x3CC300FF
#define TX_DQS_DQ_OFFSET    32
#define TX_DQS_DQ_OFFSET_2TO2  -32

#define CHIP_OFFSET_ARRAY_SIZE 9
#define CHIP_OFFSET_ARRAY_VALUES {-1, 0, 1, 2, 3, 4, 5, 6, 7}


typedef struct _CHIP_WL_CLEANUP_STRUCT
{
  DUMMY_REG                          ddrtDimm0BasicTiming;
  DUMMY_REG                          tCOTHP;
  DUMMY_REG                          tCOTHP2;
}CHIP_WL_CLEANUP_STRUCT, *PCHIP_WL_CLEANUP_STRUCT;

#define CHIP_GSM_CSN_SIGNALSTOSWEEP {PAR, CAS_N, A13, RAS_N, WE_N, A10, BA1, A0, BA0, A1, A3, A2, A4, A5, A6, A7, A8, A9, A12, A11, BG1, ACT_N, BG0, C0, C1, C2}
#define NUM_SIGNALS_TO_SWEEP 26

#define CHIP_CMD_CLK_TEST_START  128
#define CHIP_CMD_CLK_TEST_RANGE  160

#define CHIP_CMD_GRP_STOP CmdGrp2

#define CHIP_RNDTRP_DIFF_MPR_OFF_3N_TO_1N 4
#define CHIP_RNDTRP_DIFF_MPR_OFF_3N_TO_2N 2
#define CHIP_RNDTRP_DIFF_MPR_OFF_2N_TO_1N 2
#define CHIP_RNDTRP_DIFF_MPR_OFF_2N_TO_2N 0
#define CHIP_RNDTRP_DIFF_MPR_OFF_1N_TO_1N 0

#define INITIAL_CHIP_REC_EN_OFFSET 32
#define TEARDOWN_CHIP_REC_EN_OFFSET -INITIAL_CHIP_REC_EN_OFFSET

#define CHIP_GET_CTL_CLK_RES_MAX_STROBE MAX_STROBE

typedef enum {
    TYPE_SCF_BAR = 0,
    TYPE_PCU_BAR,
    TYPE_MEM_BAR0,
    TYPE_MEM_BAR1,
    TYPE_MEM_BAR2,
    TYPE_MEM_BAR3,
    TYPE_MEM_BAR4,
    TYPE_MEM_BAR5,
    TYPE_MEM_BAR6,
    TYPE_MEM_BAR7,
    TYPE_SBREG_BAR,
    TYPE_MAX_MMIO_BAR
} MMIO_BARS;

typedef struct {
  UINT8               ddrVoltageValue;
  UINT32              Vr12Encoding;
  UINT32              Vr125Encoding;
  UINT32              Vr135Encoding;
  UINT32              Vr1310Encoding;
#ifdef SERIAL_DBG_MSG
  char                *string_ptr;
#endif //MRC_DEBUG_PRINT
} VddValues;

//-----------------------------------------------------------------------------
// Test Modes (mode)
#define MODE_XTALK            BIT0  // Placeholder - This has not been implemented
#define MODE_VIC_AGG          BIT1
#define MODE_START_ZERO       BIT2
#define MODE_POS_ONLY         BIT3
#define MODE_NEG_ONLY         BIT4
#define MODE_DATA_MPR         BIT5
#define MODE_DATA_FIXED       BIT6
#define MODE_DATA_LFSR        BIT7
#define MODE_ADDR_LFSR        BIT8
#define MODE_ADDR_CMD0_SETUP  BIT9
#define MODE_CHECK_PARITY     BIT10
#define MODE_DESELECT         BIT11
#define MODE_VA_DESELECT      BIT12

// Scope of the margin
#define SCOPE_SOCKET      0
#define SCOPE_CH        1
#define SCOPE_DIMM      2
#define SCOPE_RANK      3
#define SCOPE_STROBE    4
#define SCOPE_BIT       5

// Vref centering method
#define NON_PER_BIT     0
#define PER_BIT         1

struct baseMargin {
  INT16 n;
  INT16 p;
};


// Output structures based on scope
#define  MAX_BITS    72

struct chMargin {
  struct baseMargin channel[MAX_CH];
};
struct dimmMargin {
  struct baseMargin dimm[MAX_CH][MAX_DIMM];
};
struct rankMargin {
  struct baseMargin rank[MAX_CH][MAX_RANK_CH];
};
struct strobeMargin {
  struct baseMargin strobe[MAX_CH][MAX_RANK_CH][MAX_STROBE];
};
struct bitMargin {
  struct baseMargin bits[MAX_CH][MAX_RANK_CH][MAX_BITS];
};
struct bitMarginCh {
  struct baseMargin bits[MAX_CH][MAX_BITS];
};

#ifdef LRDIMM_SUPPORT
struct subRankMargin {
  struct rankMargin subRank[MAX_SUBRANK];
};
#endif

// Mask of bits to ignore
typedef struct bitMask {
   UINT8 bits[MAX_CH][MAX_RANK_CH][MAX_STROBE/2];
} BITMASK_STRUCT, *PBITMASK_STRUCT;

#endif // _memhostchipcommon_h
