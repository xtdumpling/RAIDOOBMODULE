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
 * Copyright 2006 - 2017 Intel Corporation All Rights Reserved.
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

#ifndef  _mem_defaults_h
#define  _mem_defaults_h

#define AUTO                  0

#define TEMPHIGH_EN           BIT0
#define PDWN_MODE_APD         BIT1
#define PDWN_MODE_PPD         BIT2
#define PDWN_MODE_SLOW_EXIT   BIT3
#define PDWN_MODE_IBT_OFF_EN  BIT4
#define PDWN_IDLE_CNTR        0x40

//
// RAS Mode
//
#define MIRROR_EN             BIT0    // Mirror Mode
#define LOCKSTEP_EN           BIT1    // Lockstep Mode

#define CH_ALL_MIRROR         (FULL_MIRROR_1LM | FULL_MIRROR_2LM | PARTIAL_MIRROR_1LM | PARTIAL_MIRROR_2LM)

#define LOCK_NGN_CSR_DISABLE      0
#define LOCK_NGN_CSR_ENABLE       1
#define LOCK_NGN_CSR_AUTO         LOCK_NGN_CSR_ENABLE   // Setupdefault for lockNgnCsr

#define NGN_CMD_1N                0
#define NGN_CMD_2N                1
#define NGN_CMD_TIME_AUTO         NGN_CMD_1N             // Setupdefault for ngnCmdTime

#define NGN_ECC_CORR_DISABLE      0
#define NGN_ECC_CORR_ENABLE       1
#define NGN_ECC_CORR_AUTO         NGN_ECC_CORR_ENABLE   // Setupdefault for ngnEccCorr

#define NGN_ECC_WR_CHK_DISABLE    0
#define NGN_ECC_WR_CHK_ENABLE     1
#define NGN_ECC_WR_CHK_AUTO       NGN_ECC_WR_CHK_ENABLE // Setupdefault for ngnEccWrChk

#define NGN_ECC_RD_CHK_DISABLE    0
#define NGN_ECC_RD_CHK_ENABLE     1
#define NGN_ECC_RD_CHK_AUTO       NGN_ECC_RD_CHK_ENABLE // Setupdefault for ngnEccRdChk

#define CR_MONETIZATION_DISABLE   0
#define CR_MONETIZATION_ENABLE    1
#define CR_MONETIZATION_AUTO      CR_MONETIZATION_DISABLE

#define NGN_DEBUG_LOCK_DISABLE     0
#define NGN_DEBUG_LOCK_ENABLE      1
#define NGN_DEBUG_LOCK_AUTO        LOCK_NGN_CSR_DISABLE // Setupdefault for ngnDebugLock

//
// MC_ODT
//
#define MC_ODT_50_OHMS        0
#define MC_ODT_100_OHMS       1
#define MC_ODT_AUTO           2
#define MC_ODT_DEFAULT        MC_ODT_50_OHMS

//
// ALLOW_CORRECTABLE_ERRORS
//
#define ALLOW_CORRECTABLE_ERROR_DISABLE     0
#define ALLOW_CORRECTABLE_ERROR_ENABLE      1
#define ALLOW_CORRECTABLE_ERROR_AUTO        ALLOW_CORRECTABLE_ERROR_ENABLE
#define ALLOW_CORRECTABLE_ERROR_DEFAULT     ALLOW_CORRECTABLE_ERROR_AUTO  // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// Memory Power Savings Mode
//
#define MEM_PWR_SAV_DISABLE   0       // Disable the PPM control and force this to be off all the time
#define MEM_PWR_SAV_SLOW      1       // PPDS with processor control of parameters
#define MEM_PWR_SAV_FAST      2       // PPDF with processor control of parameters
#define MEM_PWR_SAV_USER      3       // Disable processor control of parameters, set values manually
#define MEM_PWR_SAV_AUTO      4
#define MEM_PWR_SAV_DEFAULT_DDR4   MEM_PWR_SAV_USER
#define MEM_PWR_SAV_DEEP      1
#define MEM_PWR_SAV_SHALLOW   2
#define MEM_PWR_SAV_DEFAULT_DDRT   MEM_PWR_SAV_DEEP

//
// CKE Throttling
//
#define CKE_MODE_OFF          0       // CKE Throttling Disabled
#define CKE_APD               1       // Advanced Power Down Enabled, Pre-charge Power Down Disabled
#define CKE_PPDF              2       // Advanced Power Down Disabled, Pre-charge Power Down Fast Enabled
#define CKE_PPDS              3       // Advanced Power Down Disabled, Pre-charge Power Slow Enabled
#define CKE_APD_PPDF          4       // Advanced Power Down Enabled, Pre-charge Power Down Enabled
#define CKE_APD_PPDS          5       // Advanced Power Down Enabled, Pre-charge Power Slow Enabled
#define CKE_MODE_AUTO         6
#define CKE_MODE_DEFAULT   CKE_APD

//
// CK/CK# OFF for Self Refresh
//
#define CK_SELF_REF_DRIVEN    0x00    // CK driven during self refresh
#define CK_SELF_REF_TRI_STATE 0x1     // CK tri-stated during self refresh
#define CK_SELF_REF_LOW       0x2     // CK pulled low during self refresh
#define CK_SELF_REF_HIGH      0x3     // CK pulled high during self refresh
#define CK_SELF_REF_AUTO      0x4     // setting depends on the DIMM type

//
// Opportunistic Self-Refresh
//
#define OPP_SR_DIS            0
#define OPP_SR_EN             1
#define OPP_SR_AUTO           2
#define OPP_SR_DEFAULT        OPP_SR_EN

//
// Opportunistic Reads in WMM
//
#define OPP_RD_WMM_DIS            0
#define OPP_RD_WMM_EN             1
#define OPP_RD_WMM_AUTO           2
#define OPP_RD_WMM_DEFAULT        OPP_RD_WMM_EN   // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// MDLL Shut Down Enable
//
#define MDLL_SDEN_DIS         0
#define MDLL_SDEN_EN          1
#define MDLL_SDEN_AUTO        2
#define MDLL_SDEN_DEFAULT     MDLL_SDEN_DIS

//
// MEMHOT Throttling Mode
//
#define MEMHOT_DIS            0
#define MEMHOT_OUTPUT_ONLY    1
#define MEMHOT_INPUT_ONLY     2
#define MEMHOT_INPUT_OUTPUT_EN 3

//
// MEMHOT_OUTPUT_ONLY options - hsd s5370942
//
#define MEMHOT_OUTPUT_ONLY_DIS            0
#define MEMHOT_OUTPUT_ONLY_HIGH           1//4 - Default option
#define MEMHOT_OUTPUT_ONLY_HIGH_MED       2//6
#define MEMHOT_OUTPUT_ONLY_HIGH_MED_LOW   3//7

//
// Brickland/Grantley/Grangeville/Purley Platform Memory Subsystem OLTT/CLTT
// BIOS MRC Implementation White Paper
// August 2014
// Revision 1.6
//
#define CLTT_THRT_MID_DEFAULT 255
#define CLTT_THRT_HI_DEFAULT 26   // = 255 * 10% * 1 rounded up
#define CLTT_THRT_HI_DDRT_DEFAULT 4    //HSD s5371498
#define BW_LIMIT_TF_DDRT_DEFAULT  600  //HSD s5371498
#define DIMMTEMP_THRT_CRIT_DEFAULT 1  // 0.1GB/s/DIMM
#define CLTT_THRT_CRIT_DEFAULT 3  // 0.1GB/s/DIMM

#define OLTT_THRT_MID_DEFAULT 255
#define OLTT_THRT_HI_DEFAULT 204   // = 255 * 80% * 1 rounded up

#define CLTT_TEMP_LO_SINGLE_REFRESH_DEFAULT 82
#define CLTT_TEMP_MID_SINGLE_REFRESH_DEFAULT 82
#define CLTT_TEMP_HI_SINGLE_REFRESH_DEFAULT 100

#define CLTT_TEMP_LO_DOUBLE_REFRESH_DEFAULT 84
#define CLTT_TEMP_MID_DOUBLE_REFRESH_DEFAULT 93
#define CLTT_TEMP_HI_DOUBLE_REFRESH_DEFAULT 100

#define TEMP_HI_DIS_MEMTRIP_DEFAULT 100
#define TEMP_HI_EN_MEMTRIP_DEFAULT 105

#define OLTT_TEMP_LO_DEFAULT 82
#define OLTT_TEMP_MID_DEFAULT 82
#define OLTT_TEMP_HI_DEFAULT 255

#define TT_MAX_WORST_CASE_DIMM_TEMP_OFFSET 15

//
// Dram Power Table Default Values
//
#define UDIMM_MAX_POWER_DEFAULT 93
#define UDIMM_DRAM_TDP_DEFAULT 47
#define UDIMM_MIN_PWR_CLOSED_PAGE_DEFAULT 15
#define UDIMM_WRITE_PWR_SCALE_CLOSED_PAGE_DEFAULT 943
#define UDIMM_MAX_PWR_OPEN_PAGE_DEFAULT 88
#define UDIMM_TDP_OPEN_PAGE_DEFAULT 49
#define UDIMM_MIN_PWR_OPEN_PAGE_DEFAULT 18
#define UDIMM_WRITE_PWR_SCALE_OPEN_PAGE_DEFAULT 1066
#define UDIMM_REFRESH_RATE_SLOPE_DEFAULT 1663
#define UDIMM_SELF_REFRESH_POWER_DEFAULT 584

#define LRDIMM_MAX_POWER_DEFAULT 212
#define LRDIMM_DRAM_TDP_DEFAULT 141
#define LRDIMM_MIN_PWR_CLOSED_PAGE_DEFAULT 52
#define LRDIMM_WRITE_PWR_SCALE_CLOSED_PAGE_DEFAULT 466
#define LRDIMM_MAX_PWR_OPEN_PAGE_DEFAULT 203
#define LRDIMM_TDP_OPEN_PAGE_DEFAULT 157
#define LRDIMM_MIN_PWR_OPEN_PAGE_DEFAULT 75
#define LRDIMM_WRITE_PWR_SCALE_OPEN_PAGE_DEFAULT 506
#define LRDIMM_REFRESH_RATE_SLOPE_DEFAULT 6650
#define LRDIMM_SELF_REFRESH_POWER_DEFAULT 2332

#define RDIMM_MAX_POWER_DEFAULT 191
#define RDIMM_DRAM_TDP_DEFAULT 116
#define RDIMM_MIN_PWR_CLOSED_PAGE_DEFAULT 49
#define RDIMM_WRITE_PWR_SCALE_CLOSED_PAGE_DEFAULT 710
#define RDIMM_MAX_PWR_OPEN_PAGE_DEFAULT 182
#define RDIMM_TDP_OPEN_PAGE_DEFAULT 137
#define RDIMM_MIN_PWR_OPEN_PAGE_DEFAULT 72
#define RDIMM_WRITE_PWR_SCALE_OPEN_PAGE_DEFAULT 776
#define RDIMM_REFRESH_RATE_SLOPE_DEFAULT 6650
#define RDIMM_SELF_REFRESH_POWER_DEFAULT 2369

#define DRAM_RAPL_REFRESH_BASE_DEFAULT 10

//
// DRAM RAPL Extended Range Options
//
#define DRAM_RAPL_EXTENDED_RANGE_DISABLE 0
#define DRAM_RAPL_EXTENDED_RANGE_ENABLE 1
#define DRAM_RAPL_EXTENDED_RANGE_DEFAULT DRAM_RAPL_EXTENDED_RANGE_ENABLE

//
// Platform specific value to be programmed into the TSOD CONFIG register
//
//#define OEM_MTS_CONFIG_VALUE    MTS_CFG_TCRIT_ONLY + MTS_CFG_EVENT_CTRL
#define OEM_MTS_CONFIG_VALUE    0

//
// Electrical Throttling Options
//
#define ET_DISABLE  0
#define ET_ENABLE   1
#define ET_AUTO     2
// TODO: Make sure this is disabled for PO

//
// High Temperature Enable
//
#define HTE_DISABLE  0
#define HTE_ENABLE   1
#define HTE_AUTO     2
#define HTE_DEFAULT  HTE_DISABLE

//
// Allow 2X Refresh enable
//
#define A2R_DISABLE  0
#define A2R_ENABLE   1
#define A2R_AUTO     2
#define A2R_DEFAULT  A2R_DISABLE

//
// CustomRefreshRate
//
#define Cust_Refresh_Rate_MIN  20
#define Cust_Refresh_Rate_MAX  40
#define Cust_Refresh_Rate_Default  Cust_Refresh_Rate_MIN

//
// Altitude
//
#define UNKNOWN   0
#define ALT_300M  1
#define ALT_900M  2
#define ALT_1500M 3
#define ALT_3000M 4
//
// Page Policy
//
#define CLOSED_PAGE_DIS       0       // Closed Page mode disabled (Open page enabled)
#define CLOSED_PAGE_EN        1       // Closed Page mode enabled
#define OPEN_PAGE_ADAPTIVE    2       // Adaptive Open page mode enabled
#define CLOSED_PAGE_AUTO      3
#define CLOSED_PAGE_DEFAULT   OPEN_PAGE_ADAPTIVE

// Memory Type
#define RDIMM_ONLY            0       // RDIMMs only
#define UDIMM_ONLY            1       // UDIMMs (and SODIMMs) only
#define RDIMMandUDIMM         2       // Both RDIMMs and UDIMMs supported
#define MEM_TYPE_AUTO         RDIMMandUDIMM

// DDR3 Frequency
#define DDR3_FREQ_AUTO        AUTO
#define DDR3_FREQ_DEFAULT     DDR3_FREQ_AUTO

// DDR3 Voltage
#define DDR3_VDD_AUTO         0
#define DDR3_VDD_150          1
#define DDR3_VDD_135          2
#define DDR3_VDD_DEFAULT      DDR3_VDD_AUTO

// ECC Support
#define ECC_DIS               0       // Disable ECC support
#define ECC_EN                1       // Enable ECC support
#define ECC_AUTO              ECC_EN
#define ECC_DEFAULT           ECC_EN  // Changing the default here requires a change in the help string of the respective BIOS knob.

//Page Table Aliasing -  option of choosing which bit to use for page table aliasing - either bit 1 or 2.
#define PAGE_TBL_AL_SET0               0       // if set to 0, alias will occur on bit 1.
#define PAGE_TBL_AL_SET1               1       // If set to 1, alias will occur with bit 2
#define PAGE_TBL_AL_AUTO               2
#define PAGE_TBL_AL_DEFAULT            PAGE_TBL_AL_SET0

// LRDIMM Rank Multiplication
#define RANK_MULT_AUTO      0     // Choose RM factor based on #DPC
#define RANK_MULT_EN        1     // Force RM enabled

// Log Parsing - option of enabling additional information in the log to make parsing it more generic and easier in MT
#define LOG_PARSING_DIS     0     // Disabled
#define LOG_PARSING_EN      1     // Enabled

// LRDIMM Module Delay
#define MODULE_DELAY_AUTO   0       // Boundary check SPD MD and use if good, else defaults
#define MODULE_DELAY_DIS    1       // Do NOT use SPD MD. Use Defaults

// Memory test enable
#define MEM_TEST_DIS       0          // Disable memory test
#define MEM_TEST_EN        1          // Enable memory test
#define MEM_TEST_AUTO      2
#define MEM_TEST_DEFAULT   MEM_TEST_EN  // Changing the default here requires a change in the help string of the respective BIOS knob.

// Software Memory test enable
#define SW_MEM_TEST_DIS    0          // Disable memory test
#define SW_MEM_TEST_EN     1          // Enable memory test
#define SW_MEM_TEST_AUTO   SW_MEM_TEST_DIS

// Memory test enable for fast boot scenarios
#define MEM_TEST_FAST_BOOT_DIS      0      // Disable memory test during fast boot
#define MEM_TEST_FAST_BOOT_EN       1      // Enable memory test during fast boot
#define MEM_TEST_FAST_BOOT_AUTO     2
#define MEM_TEST_FAST_BOOT_DEFAULT  MEM_TEST_FAST_BOOT_DIS  // Changing the default here requires a change in the help string of the respective BIOS knob.

// Do we attempt a fast boot path through MRC?
#define FAST_BOOT_DIS         0       // Disable fast boot (i.e. take the normal cold boot path through MRC)
#define FAST_BOOT_EN          1       // Enable fast boot (i.e. skip parts of MRC when possible)
#define FAST_BOOT_AUTO        2
#define FAST_BOOT_DEFAULT     FAST_BOOT_EN  // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// Fast Cold Boot
//
#define FAST_BOOT_COLD_DIS         0       // Disable fast boot (i.e. take the normal cold boot path through MRC)
#define FAST_BOOT_COLD_EN          1       // Enable fast boot (i.e. skip parts of MRC when possible)
#define FAST_BOOT_COLD_AUTO        2
#define FAST_BOOT_COLD_DEFAULT     FAST_BOOT_COLD_EN   // Changing the default here requires a change in the help string of the respective BIOS knob.

#define RMT_COLD_FAST_BOOT_DIS     0
#define RMT_COLD_FAST_BOOT_EN      1
#define RMT_COLD_FAST_BOOT_AUTO    2
#define RMT_COLD_FAST_BOOT_DEFAULT RMT_COLD_FAST_BOOT_DIS  // Changing the default here requires a change in the help string of the respective BIOS knob.

//
//Memory test loops
//
#define MEM_TEST_LOOPS_DEFAULT      1          // Number of MemTests to execute

//
// Advanced Memory Test algorithms using CPGC
//
#define ADV_MEM_TEST_DIS            0
#define ADV_MEM_TEST_EN             1
#define ADV_MEM_TEST_DEFAULT        ADV_MEM_TEST_DIS

// DDR Cycling Support
#define DDR_CYCLING_DIS       0       // Disable DDR Cycling
#define DDR_CYCLING_EN        1       // Enable DDR Cycling

//
// Refresh parameter timing constraints
//
#define OREFNI                63
#define REF_HI_WM             8
#define REF_PANIC_WM          9

//
// ZQ Calibration Timing Parameter
//
#define ZQCS_PERIOD           128

//
// Self-refresh idle counter
//
#define SREF_IDLE_CNTR        0x3e800
//
// WDB Watermarks
//
#define WMM_ENTRY             0x24   // HSD 4928321
#define WMM_EXIT              0x12
#define STARVATION_COUNTER    0x120

//
// NUMA Mode
//
#define NUMA_DIS              0
#define NUMA_EN               1
#define NUMA_AUTO             2
#define NUMA_DEFAULT          NUMA_EN
#define SOCKET_1WAY           1
#define SOCKET_2WAY           2
#define SOCKET_4WAY           4
#define SOCKET_INTER_AUTO     SOCKET_2WAY

#define DIMM_MGMT_BIOS_SETUP      0
#define DIMM_MGMT_CR_MGMT_DRIVER  1
#define DIMM_MGMT_AUTO            DIMM_MGMT_CR_MGMT_DRIVER

//DDRT partition ratios
#define RATIO_20_80_0   0
#define RATIO_80_20_0   1
#define RATIO_50_50_0   2
#define RATIO_60_0_40   3
#define RATIO_40_10_50  4
#define RATIO_0_40_60   5
#define RATIO_0_30_70   6
#define RATIO_100_0_0   7
#define RATIO_0_100_0   8
#define RATIO_0_0_100   9
#define RATIO_30_60_10  10
#define RATIO_70_0_30   11

//HighAddressBitStartPosition
#define HIGH_ADDR_START_BIT_POSITION_33 33
#define HIGH_ADDR_START_BIT_POSITION_34 34
#define HIGH_ADDR_START_BIT_POSITION_35 35
#define HIGH_ADDR_START_BIT_POSITION_36 36
#define HIGH_ADDR_START_BIT_POSITION_37 37
#define HIGH_ADDR_START_BIT_POSITION_38 38
#define HIGH_ADDR_START_BIT_POSITION_39 39
#define HIGH_ADDR_START_BIT_POSITION_40 40
#define HIGH_ADDR_START_BIT_POSITION_41 41
#define HIGH_ADDR_START_BIT_POSITION_42 42
#define HIGH_ADDR_START_BIT_POSITION_43 43
#define HIGH_ADDR_START_BIT_POSITION_44 44
#define HIGH_ADDR_START_BIT_POSITION_45 45

//
// partition DDRT DIMM
//
#define PARTITION_DDRT_DIMM_DIS               0
#define PARTITION_DDRT_DIMM_EN                1

#define SCK_GRAN_256B     1
#define CH_GRAN_256B      1

//
// Secure Erase Unit
//
#define DISABLE_SECURE_ERASE 0
#define ENABLE_SECURE_ERASE  1

#define MAX_AEP_DIMM_SETUP 48 //(MAX_CH * MAX_SOCKET * MAX_AEP_CH)

//
// Volatile Memory Mode
//
#define VOL_MEM_MODE_1LM  0
#define VOL_MEM_MODE_2LM  1
#define VOL_MEM_MODE_AUTO 2
//
// Persistent Memory Mode
//
#define NON_PER_MEM_MODE    0
#define PER_MEM_MODE        1
#define PER_MEM_CACHE_MODE  2
// Temporary workaround for CR PO
//#define MEM_MODE_AUTO       NON_PER_MEM_MODE
#define MEM_MODE_AUTO       PER_MEM_MODE

//
// Block Memory Mode
//
#define NON_BLOCK_MEM_MODE   0
#define BLOCK_MEM_MODE       1

//
// Force Persistent Memory Mode Region
//
#define NON_PER_MEM_REGION   0
#define PER_MEM_REGION       1

//
// Force Block Memory Mode Region
//
#define NON_BLK_MEM_REGION   0
#define BLK_MEM_REGION       1

//
// Interleave Granularity 1LM
//
#define MEM_INT_GRAN_1LM_AUTO                 0
#define MEM_INT_GRAN_1LM_256B_256B            1
#define MEM_INT_GRAN_1LM_64B_64B              2
#define MEM_INT_GRAN_1LM_DEFAULT              MEM_INT_GRAN_1LM_256B_256B

//
// Interleave Granularity 2LM
//
#define MEM_INT_GRAN_2LM_AUTO                 0
#define MEM_INT_GRAN_2LM_NM_4KB_256B_FM_4KB   1
#define MEM_INT_GRAN_2LM_NM_4KB_64B_FM_256B   2
#define MEM_INT_GRAN_2LM_NM_4KB_64B_FM_4KB    3
#define MEM_INT_GRAN_2LM_DEFAULT              MEM_INT_GRAN_2LM_NM_4KB_256B_FM_4KB

//
// Interleave Granularity PMEM
//
#define MEM_INT_GRAN_PMEM_NUMA_AUTO           0
#define MEM_INT_GRAN_PMEM_NUMA_4KB_4KB        1
#define MEM_INT_GRAN_PMEM_NUMA_4KB_256B       2
#define MEM_INT_GRAN_PMEM_NUMA_DEFAULT        MEM_INT_GRAN_PMEM_NUMA_4KB_4KB

//
// Config Mask for 2LM
//
#define CFG_MASK_2LM_NORMAL                   0
#define CFG_MASK_2LM_AGGRESSIVE               1

//
// IMC Interleave
//
#define IMC_1WAY               1
#define IMC_2WAY               2
#define IMC_INTER_AUTO         AUTO
#define IMC_INTER_DEFAULT      IMC_2WAY

//
// Channel Interleave
//
#define CH_1WAY               1
#define CH_2WAY               2
#define CH_3WAY               3
#define CH_4WAY               4
#define CH_INTER_AUTO         AUTO
#define CH_INTER_DEFAULT      CH_3WAY

//
// Rank Interleave
//
#define RANK_1WAY             1
#define RANK_2WAY             2
#define RANK_4WAY             4
#define RANK_8WAY             8
#define RANK_INTER_AUTO       AUTO
#define RANK_INTER_DEFAULT    RANK_4WAY

//
// Power Management
//
#define POWER_10W               10
#define POWER_12W               12
#define POWER_15W               15
#define POWER_18W               18
#define POWER_DEFAULT           POWER_12W

//
// Socket Interleave Below 4G
//
#define SOCKET_INTLV_BELOW_4G_DIS 0
#define SOCKET_INTLV_BELOW_4G_EN  1

//
// Set System Time Opcode
//
#define SET_SYSTEM_TIME_OPCODE  0x7

//
// Set System Time sub-opcode
//
#define SET_SYSTEM_TIME_SUB_OPCODE 0x0

//
// Identify DIMM opcode and sub-opcode
//
#define IDENTIFY_DIMM_OPCODE_SUB_OPCODE 0x1

//
// get power management policy opcode
//
#define GET_POWER_MANAGEMENT_POLICY_OPCODE 0x4

//
// get/set power management policy sub-opcode
//
#define GET_SET_POWER_MANAGEMENT_POLICY_SUB_OPCODE 0x2

//
// Set power management policy opcode
//
#define SET_POWER_MANAGEMENT_POLICY_OPCODE 0x5

//
// Get Partition Info opcode
//
#define GET_PARTITION_INFO_OPCODE 0x6

//
// Set,Get Partition Info sub-opcode
//
#define GET_PARTITION_INFO_SUB_OPCODE 0x2
//
// Get Platform Config data opcode
//
#define GET_PLATFORM_CONFIG_DATA_OPCODE 0x6
//
// Get Platform Config data sub-opcode
//
#define GET_PLATFORM_CONFIG_DATA_SUB_OPCODE 0x1
//
// Set Die Sparing Policy opcode
//
#define SET_DIE_SPARING_POLICY_OPCODE 0x5
//
// Set Die Sparing Policy sub-opcode
//
#define SET_DIE_SPARING_POLICY_SUB_OPCODE 0x3

//
// Get security state opcode
//
#define GET_SECURITY_STATE_OPCODE 0x2

//
// Get security state sub-opcode
//
#define GET_SECURITY_STATE_SUB_OPCODE 0x0

//
// Secure Erase Unit opcode
//
#define SECURE_ERASE_UNIT_OPCODE 0x3

//
// Secure Erase Unit sub-opcode
//
#define SECURE_ERASE_UNIT_SUB_OPCODE 0xF5

//
// Set Config Lock down opcode
//
#define SET_CONFIG_LOCK_DOWN_OPCODE  0x07
//
// Set Config Lock down sub-opcode
//
#define SET_CONFIG_LOCK_DOWN_SUB_OPCODE  0x05

//
// RAS Mode
//
#define RAS_MODE_DISABLE        0
#define MIRROR_1LM_ENABLED      1
#define MIRROR_2LM_ENABLED      2
#define LOCKSTEP_ENABLED        3
#define SPARING_DISABLED        0
#define SPARING_ENABLED         1
#define ONE_RANK_SPARE          1
#define TWO_RANK_SPARE          2

/*#define FAILOVER_DIR_SCRUB    0
#define FAILOVER_BADCHNL_FIRST  1
#define FAILOVER_SWAP_MAPOUT    2*/

#define FAILOVER_DISABLED       0
#define FAILOVER_DIR_SCRUB      1
#define FAILOVER_BADCHNL_FIRST  2
#define FAILOVER_HIGHER_REL     3
#define FAILOVER_SWAP_MAPOUT    4 // De-PORed

// Index of the internal ThresholdLog2 type to read/write for FAILOVER_SAWP_MAPOUT error.
#define  LINK_TRANSIENT_ERR_TYPE  0
#define  ECC_CORRECTED_ERR_TYPE   1
#define  UNCORRECTABLE_ERR_TYPE   2
#define  MIRRSCRUBRD_NOERR_TYPE   3
#define  MIRRSCRUBRD_ERR_TYPE     4

#define PMIRROR_1LM    1
#define PMIRROR_2LM    2
#define PMIRROR_DISABLED   0
#define PMIRROR_ENABLED    1
#define MAX_PMIRR_SIZE          0xFFFF //Max multiplier for 64MB partial mirrors
#define MAX_PMIRR_PERCENT_SIZE  5000  //Maximum percentage that can be mirrored is 50%
#define MAX_PARTIAL_MIRROR      4  //Maximum number of partial mirror regions that can be created

#define NGN_DIE_SPARING_EN                               1
#define NGN_DIE_SPARING_DIS                              0
#define NGN_DIE_SPARING_AGGRESSIVENESS_LEVEL_DEFAULT     128

#define NGN_ADDRESS_RANGE_SCRUB_EN                       1
#define NGN_ADDRESS_RANGE_SCRUB_DIS                      0

#define NGN_HOST_ALERT_POISON_EN                     0
#define NGN_HOST_ALERT_VIRAL_EN                      1

//
//  XMP Support
//
#define XMP_AUTO              0
#define XMP_MANUAL            1
#define XMP_PROFILE_1         2
#define XMP_PROFILE_2         3

//
//  BGF Threshold
//
#define BGF_THRESHOLD         4

//
// Scrambling support
//
#define SCRAMBLE_DISABLE        0       // Disable Data Scrambling
#define SCRAMBLE_ENABLE         1       // Enable Data Scrambling
#define SCRAMBLE_AUTO           2
#define SCRAMBLE_DEFAULT        SCRAMBLE_ENABLE   // Changing the default here requires a change in the help string of the respective BIOS knob.
#define SCRAMBLE_SEED_LOW       41003
#define SCRAMBLE_SEED_HIGH      54165
#define MAX_SCRAMBLE_SEED_LOW   65535
#define MAX_SCRAMBLE_SEED_HIGH  65535

//
// Page Policy and Timing parameters
//
#define IDLE_PAGE_RST_VAL      8
#define IDLE_PAGE_RST_VAL_MAX 63
#define WIN_SIZE              64
#define PPC_TH                6
#define OPC_TH                6

//
// RDIMM Timing Control 2
//
#define RDIMM_TIMING_CNTL2_TCKOFF 0x7F
#define RDIMM_TIMING_CNTL2_TCKEV_DDR_2133 4
#define RDIMM_TIMING_CNTL2_TCKEV_DDR_2666 6
#define RDIMM_TIMING_CNTL2_TCKEV 8

//
// Partial Write Starvation Counter
//
#define WPQ_IS                37    // HSD 4928321
#define WPQ_IS_ISOC           37    // HSD 4928321

//
// Channel PPDS idle counter
//
#define PPDS_IDLE_TIMER       0x200

//
// Rank Margin Tool
//
#define RMT_DIS               0
#define RMT_EN                1
#define RMT_AUTO              2
#define RMT_DEFAULT           RMT_EN  // Changing the default here requires a change in the help string of the respective BIOS knob.

#define BACKSIDE_CMD_RMT      RMT_DIS

#define RMT_PATTERN_LENGTH    32767
#define RMT_PATTERN_LENGTH_CMD_CTL_VREF    32767

//
// NGN BCOM Margining for RMT and BSSA
//
#define NGN_BCOM_MARGINING  RMT_DIS

//
// Pattern length for advanced training steps
//
#define TRAIN_RD_DQS_PL       64
#define TRAIN_WR_DQS_PL       64
#define TRAIN_RD_DQS_DL       256

//
// Minimum timing check
//
#define MIN_TIMING_LIMIT      5

//
// Multi-Threaded MRC
//
#define MULTI_THREADED_DIS    0
#define MULTI_THREADED_EN     1
#define MULTI_THREADED_AUTO   2
#define MULTI_THREADED_DEFAULT MULTI_THREADED_EN   // Changing the default here requires a change in the help string of the respective BIOS knob.
//
// DRAM Ron
//
#define DRAM_RON_DISABLE      0
#define DRAM_RON_ENABLE       1
#define DRAM_RON_AUTO         2
#define DRAM_RON_DEFAULT      DRAM_RON_ENABLE

//
// RX ODT
//
#define RX_ODT_DISABLE        0
#define RX_ODT_ENABLE         1
#define RX_ODT_AUTO           2
#define RX_ODT_DEFAULT        RX_ODT_DISABLE

//
// MC RON
//
#define MC_RON_AUTO           0
#define MC_RON_DISABLE        1
#define MC_RON_ENABLE         2
#define MC_RON_DEFAULT        MC_RON_ENABLE

//
// Tx Equalization calibration
//
#define TX_EQ_DISABLE         0
#define TX_EQ_ENABLE          1
#define TX_EQ_AUTO            2
#define TX_EQ_DEFAULT         TX_EQ_ENABLE

//
// iMode training
//
#define IMODE_DISABLE         0
#define IMODE_ENABLE          1
#define IMODE_AUTO            2
#define IMODE_DEFAULT         IMODE_ENABLE

//
// RXCTLE training
//
#define RX_CTLE_DISABLE      0
#define RX_CTLE_ENABLE       1
#define RX_CTLE_AUTO         2
#define RX_CTLE_DEFAULT      RX_CTLE_ENABLE

//
// RPQAGE defaults
//
#define IO_COUNT              0x40
//#define CPU_GT_COUNT          0x40
// HSD4929074
#define CPU_GT_COUNT          0x100

//
// CMD Normalization
//
#define CMD_NORMAL_DISABLE          0
#define CMD_NORMAL_ENABLE           1
#define CMD_NORMAL_AUTO             2
#define CMD_NORMAL_DEFAULT          CMD_NORMAL_ENABLE

//
// Round Trip Latency
//
#define ROUND_TRIP_LATENCY_DISABLE  0
#define ROUND_TRIP_LATENCY_ENABLE   1
#define ROUND_TRIP_LATENCY_AUTO     2
#define ROUND_TRIP_LATENCY_DEFAULT  ROUND_TRIP_LATENCY_ENABLE

//
// CMD/CLK Training
//
#define CMD_CLK_DIS           0
#define CMD_CLK_EN            1
#define CMD_CLK_AUTO          2
#define CMD_CLK_DEFAULT       CMD_CLK_EN

//
// Rx Vref Training
//
#define RX_VREF_DISABLE       0
#define RX_VREF_ENABLE        1
#define RX_VREF_AUTO          2
#define RX_VREF_DEFAULT       RX_VREF_ENABLE

//
// Tx Vref Training
//
#define TX_VREF_DISABLE       0
#define TX_VREF_ENABLE        1
#define TX_VREF_AUTO          2
#define TX_VREF_DEFAULT       TX_VREF_ENABLE

//
// CMD Vref Training
//
#define CMD_VREF_DISABLE       0
#define CMD_VREF_ENABLE        1
#define CMD_VREF_DEFAULT       CMD_VREF_ENABLE

//
// LRDIMM Backside Vref Training
//
#define LRDIMM_BACKSIDE_VREF_DISABLE         0
#define LRDIMM_BACKSIDE_VREF_ENABLE          1
#define LRDIMM_BACKSIDE_VREF_AUTO            2
#define LRDIMM_BACKSIDE_VREF_DEFAULT         LRDIMM_BACKSIDE_VREF_ENABLE

#define LRDIMM_WR_VREF_DISABLE               0
#define LRDIMM_WR_VREF_ENABLE                1
#define LRDIMM_WR_VREF_AUTO                  2
#define LRDIMM_WR_VREF_DEFAULT               LRDIMM_WR_VREF_ENABLE  //Permanently disabled for HSX, applies to BDX only.

#define LRDIMM_RD_VREF_DISABLE               0
#define LRDIMM_RD_VREF_ENABLE                1
#define LRDIMM_RD_VREF_AUTO                  2
#define LRDIMM_RD_VREF_DEFAULT               LRDIMM_RD_VREF_ENABLE  //Permanently disabled for HSX, applies to BDX only.

//
// LRDIMM Baside RX DQ Centering
//
#define LRDIMM_RX_DQ_CENTERING_DISABLE       0
#define LRDIMM_RX_DQ_CENTERING_ENABLE        1
#define LRDIMM_RX_DQ_CENTERING_AUTO          2
#define LRDIMM_RX_DQ_CENTERING_DEFAULT       LRDIMM_RX_DQ_CENTERING_ENABLE

//
// PDA
//
#define PDA_DISABLE           0
#define PDA_ENABLE            1
#define PDA_AUTO              2
#define PDA_DEFAULT           PDA_ENABLE

//
// Turnaround Time Optimization
//
#define TURNAROUND_DISABLE    0
#define TURNAROUND_ENABLE     1
#define TURNAROUND_AUTO       2
#define TURNAROUND_DEFAULT    TURNAROUND_ENABLE

//
// IOT MEMORY BUFFER RESERVATION
//
#define IOT_BUFFER_DEFAULT    0  // 1 = 64MB, 2= 128MB, so on till 256 = 16384MB (16GB)

//
// Per Bit DeSkew Training
//
#define BIT_DESKEW_DISABLE   0
#define BIT_DESKEW_ENABLE    1
#define BIT_DESKEW_AUTO      2
#define BIT_DESKEW_DEFAULT   BIT_DESKEW_ENABLE

//
// Enable Backside RMT
//
#define BACKSIDE_RMT_DISABLE     0
#define BACKSIDE_RMT_ENABLE      1
#define BACKSIDE_RMT_AUTO        2
#define BACKSIDE_RMT_DEFAULT     BACKSIDE_RMT_ENABLE  // Changing the default here requires a change in the help string of the respective BIOS knob.

#ifdef SSA_FLAG
//
// Enabling the BIOS SSA loader
//
#define BIOS_SSA_LOADER_DISABLE           0
#define BIOS_SSA_LOADER_ENABLE            1
#define BIOS_SSA_LOADER_AUTO              2
#define BIOS_SSA_LOADER_DEFAULT           BIOS_SSA_LOADER_DISABLE

//
// Enabling the BIOS SSA Stitched Mode (RMT)
//
#define BIOS_SSA_RMT_DISABLE                 0
#define BIOS_SSA_RMT_ENABLE                  1
#define BIOS_SSA_RMT_AUTO                    2
#define BIOS_SSA_RMT_DEFAULT                 BIOS_SSA_RMT_DISABLE

//
// Enabling the BIOS SSA Stitched Mode (RMT) on FAST COLD BOOT(FCB)
//
#define BIOS_SSA_RMT_FCB_DISABLE                 0
#define BIOS_SSA_RMT_FCB_ENABLE                  1
#define BIOS_SSA_RMT_FCB_AUTO                    2
#define BIOS_SSA_RMT_FCB_DEFAULT                 BIOS_SSA_RMT_FCB_DISABLE

// *Sub-menu with Configuration options for the BIOS SSA Rank Margin Tool*

//Generic defaults
#define BIOS_SSA_RMT_DEFAULT_VALUE                                1
#define BIOS_SSA_LOOP_CNT_DEFAULT                                16

//
// BSSA RMT Per-Bit Margining
//
#define BIOS_SSA_PER_BIT_MARGINING_DISABLE              0
#define BIOS_SSA_PER_BIT_MARGINING_ENABLE               1
#define BIOS_SSA_PER_BIT_MARGINING_AUTO                 2
#define BIOS_SSA_PER_BIT_MARGINING_DEFAULT              BIOS_SSA_PER_BIT_MARGINING_ENABLE

//
// BSSA RMT Display Tables
//
#define BIOS_SSA_DISPLAY_TABLE_DISABLE                  0
#define BIOS_SSA_DISPLAY_TABLE_ENABLE                   1
#define BIOS_SSA_DISPLAY_TABLE_AUTO                     2
#define BIOS_SSA_DISPLAY_TABLE_DEFAULT                  BIOS_SSA_DISPLAY_TABLE_ENABLE

//
// BSSA RMT Display Plots
//
#define BIOS_SSA_DISPLAY_PLOTS_DISABLE                  0
#define BIOS_SSA_DISPLAY_PLOTS_ENABLE                   1
#define BIOS_SSA_DISPLAY_PLOTS_AUTO                     2
#define BIOS_SSA_DISPLAY_PLOTS_DEFAULT                  BIOS_SSA_DISPLAY_PLOTS_ENABLE

//
// BSSA RMT Backside Margining
//
#define BIOS_SSA_BACKSIDE_MARGINING_DISABLE             0
#define BIOS_SSA_BACKSIDE_MARGINING_ENABLE              1
#define BIOS_SSA_BACKSIDE_MARGINING_AUTO                2
#define BIOS_SSA_BACKSIDE_MARGINING_DEFAULT             BIOS_SSA_BACKSIDE_MARGINING_DISABLE

//
// BSSA RMT Early Read ID Margining
//
#define BIOS_SSA_EARLY_READ_ID_DISABLE                  0
#define BIOS_SSA_EARLY_READ_ID_ENABLE                   1
#define BIOS_SSA_EARLY_READ_ID_AUTO                     2
#define BIOS_SSA_EARLY_READ_ID_DEFAULT                  BIOS_SSA_EARLY_READ_ID_DISABLE

//
// BSSA RMT Step Size Override
//
#define BIOS_SSA_STEP_SIZE_OVERRIDE_DISABLE             0
#define BIOS_SSA_STEP_SIZE_OVERRIDE_ENABLE              1
#define BIOS_SSA_STEP_SIZE_OVERRIDE_AUTO                2
#define BIOS_SSA_STEP_SIZE_OVERRIDE_DEFAULT             BIOS_SSA_STEP_SIZE_OVERRIDE_DISABLE

//
// BSSA RMT Debug Messages
//
#define BIOS_SSA_RMT_DEBUG_MSGS_DISABLE                  2
#define BIOS_SSA_RMT_DEBUG_MSGS_ENABLE                   5 //values are 2 for disabled, and 5 for enabled.
#define BIOS_SSA_RMT_DEBUG_MSGS_AUTO                     9
#define BIOS_SSA_RMT_DEBUG_MSGS_DEFAULT                  BIOS_SSA_RMT_DEBUG_MSGS_DISABLE
#endif //SSA_FLAG

//
// Option to offset the final memory training results
//
#define TRNG_RESULT_OFFSET_DISABLE     0
#define TRNG_RESULT_OFFSET_ENABLE      1
#define TRNG_RESULT_OFFSET_DEFAULT     TRNG_RESULT_OFFSET_DISABLE

//
// RCV Enable after CMD/CLK Training
//
#define RCV_EN_DIS            0
#define RCV_EN_EN             1
#define RCV_EN_AUTO           RCV_EN_EN

//
// Duty Cycle Training
//
#define DUTY_CYCLE_DISABLE 0
#define DUTY_CYCLE_ENABLE  1
#define DUTY_CYCLE_AUTO    DUTY_CYCLE_ENABLE

//
// Patrol scrubbing
//
#define PATROL_SCRUB_DURATION_DEFAULT 24
#define PATROL_SCRUB_DURATION_MAX 24
#define PATROL_SCRUB_DIS      0
#define PATROL_SCRUB_EN       1
#define PATROL_SCRUB_REVERSE_ADDR   0
#define PATROL_SCRUB_SPA            1
//
// Demand Scrub
//
#define DEMAND_SCRUB_DIS      0
#define DEMAND_SCRUB_EN       1

//
// Leaky Bucket Default Values
//
#define LEAKY_BUCKET_LO       0x28
#define LEAKY_BUCKET_HI       0x29

//
// Phase Shedding Control
//
#define PHASE_SHEDDING_DIS    0
#define PHASE_SHEDDING_EN     1
#define PHASE_SHEDDING_AUTO   2
#define PHASE_SHEDDING_DEFAULT PHASE_SHEDDING_DIS

//
// Multi-Vref algorithm defaults
//
#define RX_MULTI_VREF_POINTS  7
#define TX_MULTI_VREF_POINTS  7
#define MAX_VREF_POINTS       10    // Maximum number of points possible
#define RX_MULTI_VREF_STEP    4
#define TX_MULTI_VREF_STEP    6
#define WEIGHT_COEF_A         0
#define WEIGHT_COEF_B         8
#define WEIGHT_COEF_C         100

//
// Enforce POR
//
#define ENFORCE_POR_EN        0
#define ENFORCE_STRETCH_EN    1
#define ENFORCE_POR_DIS       2
#define ENFORCE_POR_AUTO      3
//SKX change for now
#define ENFORCE_POR_DEFAULT   ENFORCE_POR_EN  // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// C/A Parity
//
#define CA_PARITY_DISABLE     0
#define CA_PARITY_ENABLE      1
#define CA_PARITY_AUTO        CA_PARITY_ENABLE  // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// Write CRC
//
#define WR_CRC_DISABLE           0
#define WR_CRC_ENABLE            1
#define WR_CRC_AUTO              WR_CRC_DISABLE   // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// DIMM Isolation
//
#define DIMM_ISOLATION_DISABLE    0
#define DIMM_ISOLATION_ENABLE     1
#define DIMM_ISOLATION_AUTO       DIMM_ISOLATION_ENABLE   // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// Alternate Address Map
//
#define ALTERNATE_ADDRESS_MAP_DIS     0
#define ALTERNATE_ADDRESS_MAP_EN      1
#define ALTERNATE_ADDRESS_MAP_AUTO    2
#define ALTERNATE_ADDRESS_MAP_DEFAULT ALTERNATE_ADDRESS_MAP_EN

//
// Display Eye Diagrams
//
#define EYE_DIAGRAM_DIS       0
#define EYE_DIAGRAM_EN        1
#define EYE_DIAGRAM_AUTO      EYE_DIAGRAM_DIS

//
// Per Bit Margin Data
//
#define PER_BIT_MARGIN_DISABLE       0
#define PER_BIT_MARGIN_ENABLE        1
#define PER_BIT_MARGIN_AUTO          2
#define PER_BIT_MARGIN_DEFAULT       PER_BIT_MARGIN_DISABLE   // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// Normal Operation Interval
//
#define NORMAL_OPPERATION_INTERVAL 1024

//
// DIMMTEMP / DimmTemp default
//
#define DIMMTEMP_DEFAULT    0x5C

//
// BCLK Freq
//
#define BCLK_133    133

//
// SMBUS Clk Period
//
#define SMB_CLK_400K                0
#define SMB_CLK_1M                  1
#define SMB_CLK_100K                2
#define SMB_CLK_DEFAULT             SMB_CLK_400K  // Changing the default here requires a change in the help string of the respective BIOS knob.

//
// DLL Reset Test
//
#define DLL_RESET_TEST_LOOPS        0

//
// Read/WritePreamble TCLK
//
#define PREAMBLE_1TCLK              0
#define PREAMBLE_2TCLK              1
#define WRITE_PREAMBLE_DEFAULT      PREAMBLE_1TCLK
#define READ_PREAMBLE_DEFAULT       PREAMBLE_1TCLK


//
//
//
#define RX_VREF_PER_BIT        0xF
#define RX_VREF_PER_NIBBLE     0x8
#define RX_VREF_SETTING        RX_VREF_PER_BIT
#define RX_VREF_PER_BIT_BUS    0x8
#define RX_VREF_PER_NIBBLE_BUS 0xF
#define RX_VREF_TO_NIBBLE_BUS  RX_VREF_PER_BIT_BUS
//
// mem flows
//
#define MEM_FLOWS                   0xFFFFFFFF

//
// DDRT Transcation
//
#define DDRT_XACTOR_DEFAULT 1

//
// ADR related options
//
#define ADR_BBU     1
#define ADR_NVDIMM  2

#define SKX_A0_WORKAROUND 1

//
// PPR definitions
//
#define PPR_TYPE_HARD     1
#define PPR_TYPE_SOFT     2
#define PPR_DISABLED      0
#define PPR_TYPE_AUTO     PPR_DISABLED  // Changing the default here requires a change in the help string of the respective BIOS knob.

#define PPM_AUTO     0xFF

//
// NGN boot time ARS
//
#define NGN_ARS_ON_BOOT_DIS       0
#define NGN_ARS_ON_BOOT_EN        1
#define NGN_ARS_ON_BOOT_AUTO      NGN_ARS_ON_BOOT_DIS

//
// CR QoS Configuration
//
#define CR_QOS_DEFAULT                              0
#define CR_QOS_FASTGO_DISABLE                       1
#define CR_QOS_FASTGO_DISABLE_MLC_SQ_THRESHOLD_5    2
#define CR_QOS_FASTGO_DISABLE_MLC_SQ_THRESHOLD_6    3
#define CR_QOS_FASTGO_DISABLE_MLC_SQ_THRESHOLD_8    4
#define CR_QOS_FASTGO_DISABLE_MLC_SQ_THRESHOLD_10   5

//
// CR Mixed SKU
//
#define CR_MIXED_SKU_DIS        0
#define CR_MIXED_SKU_EN         1
#define CR_MIXED_SKU_AUTO       CR_MIXED_SKU_EN

//
// One Rank Timing Mode Option
//
#define ONE_RANK_TIMING_MODE_DISABLED 0
#define ONE_RANK_TIMING_MODE_ENABLED  1
#define ONE_RANK_TIMING_MODE_DEFAULT  ONE_RANK_TIMING_MODE_ENABLED

//
// Load NGN DIMM Management Drivers
//
#define LOAD_NGN_DIMM_MGMT_DRIVER_DIS     0
#define LOAD_NGN_DIMM_MGMT_DRIVER_EN      1
#define LOAD_NGN_DIMM_MGMT_DRIVER_AUTO    LOAD_NGN_DIMM_MGMT_DRIVER_EN

//
// IMC BCLK
//
#define IMC_BCLK_100          100
#define IMC_BCLK_133          133
#define IMC_BCLK_AUTO         0

//
// SPD CRC Check
//
#define SPD_CRC_CHECK_DIS     0
#define SPD_CRC_CHECK_EN      1
#define SPD_CRC_CHECK_AUTO    2
#define SPD_CRC_CHECK_DEFAULT SPD_CRC_CHECK_DIS

#endif   // _mem_platform_h
