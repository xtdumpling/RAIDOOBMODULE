//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2016 Intel Corp.                             *
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
// *      This file contains platfrom specific routines which might need    *
// *      porting for a particular platform.                                *
// *                                                                        *
// **************************************************************************
**/
#ifndef _KTI_MISC_H_
#define _KTI_MISC_H_

#include "DataTypes.h"
#include "PlatformHost.h"

#define KTI_DEBUG_ERROR (1 << 0)
#define KTI_DEBUG_WARN  (1 << 1)
#define KTI_DEBUG_INFO0 (1 << 2)
#define KTI_DEBUG_INFO1 (1 << 3)

#ifdef SERIAL_DBG_MSG
#define KTI_DEBUG_LEVEL (KTI_DEBUG_ERROR | KTI_DEBUG_WARN | KTI_DEBUG_INFO0 | KTI_DEBUG_INFO1)
#else
#define KTI_DEBUG_LEVEL 0
#endif

//
// Separate this out so we can exclude them from compilation if not needed, to reduce some size
//
#if (KTI_DEBUG_LEVEL & KTI_DEBUG_ERROR)
#define KtiDebugPrintFatal(DbgInfo) OemDebugPrintKti DbgInfo
#else
#define KtiDebugPrintFatal(DbgInfo)
#endif

#if (KTI_DEBUG_LEVEL & KTI_DEBUG_WARN)
#define KtiDebugPrintWarn(DbgInfo)  OemDebugPrintKti DbgInfo
#else
#define KtiDebugPrintWarn(DbgInfo)
#endif

#if (KTI_DEBUG_LEVEL & KTI_DEBUG_INFO0)
#define KtiDebugPrintInfo0(DbgInfo) OemDebugPrintKti DbgInfo
#else
#define KtiDebugPrintInfo0(DbgInfo)
#endif

#if (KTI_DEBUG_LEVEL & KTI_DEBUG_INFO1)
#define KtiDebugPrintInfo1(DbgInfo) OemDebugPrintKti DbgInfo
#else
#define KtiDebugPrintInfo1(DbgInfo)
#endif


#if (KTI_DEBUG_LEVEL & KTI_DEBUG_ERROR)
#define KTI_ASSERT(assertion, majorCode, minorCode) \
  if (!(assertion)) { \
    OemDebugPrintKti (host, KTI_DEBUG_ERROR, "\n\n %s: %u   %s ", __FILE__, __LINE__, #assertion); \
    KtiAssert(host, majorCode, minorCode);\
  }
#else
#define KTI_ASSERT(assertion, majorCode, minorCode) \
  if (!(assertion)) { \
    KtiAssert(host, majorCode, minorCode);\
  }
#endif

/* KTI POST code - Major */
#define STS_DATA_STRUCTURE_INIT               0xA0  // Initialize KTI inuput structure default values
#define STS_COLLECT_EARLY_SYSTEM_INFO         0xA1  // Collect info such as SBSP, Boot Mode, Reset type etc
#define STS_SETUP_MINIMUM_PATH                0xA3  // Setup up minimum path between SBSP & other sockets
#define STS_PBSP_SYNC_UP                      0xA6  // Sync up with PBSPs
#define STS_TOPOLOGY_DISCOVERY                0xA7  // Topology discovery and route calculation
#define STS_PROGRAM_FINAL_ROUTE               0xA8  // Program final route
#define STS_PROGRAM_FINAL_IO_SAD              0xA9  // Program final IO SAD setting
#define STS_PROTOCOL_LAYER_SETTING            0xAA  // Protocol layer and other Uncore settings
#define STS_FULL_SPEED_TRANSITION             0xAB  // Transition links to full speed opeartion
#define STS_SYSTEM_COHERENCY_SETUP            0xAE  // Coherency Settings
#define STS_KTI_COMPLETE                      0xAF  // KTI is done

/* Subcodes for STS_KTI_COMPLETE */
#define MINOR_STS_KTI_COMPLETE                0x00
#define MINOR_STS_ABOUT_TO_RESET              0x42

/* Subcodes for STS_DATA_STRUCTURE_INIT */
#define MINOR_STS_DATA_INIT                   0x00

/* Subcodes for STS_PROGRAM_FINAL_ROUTE */
#define MINOR_STS_PROGRAM_FINAL_ROUTE_START   0xFF
#define MINOR_STS_PROGRAM_FINAL_FPGA_START    0x01

/* Subcodes for STS_COLLECT_EARLY_SYSTEM_INFO */
#define MINOR_STS_COLLECT_INFO                0x00

/* Subcodes for STS_SYSTEM_COHERENCY_SETUP */
#define MINOR_STS_SYSTEM_COHERENCY_START      0x00

/* Subcodes for STS_SETUP_MINIMUM_PATH */
#define MINOR_STS_ADD_SOCKET_TO_MIN_PATH_TREE 0x01
#define MINOR_STS_COLLECT_LEP                 0x02
#define MINOR_STS_CHK_PBSP_CHKIN              0x03
#define MINOR_STS_SET_PBSP_BOOT_PATH          0x04
#define MINOR_STS_SET_SBSP_CONFIG_PATH        0x05

/* Subcodes for STS_PBSP_SYNC_UP */
#define MINOR_STS_ISSUE_BOOT_GO               0x01

/* Subcodes for STS_TOPOLOGY_DISCOVERY */
#define MINOR_STS_CHK_TOPOLOGY                0x01
#define MINOR_STS_DETECT_RING                 0x02
#define MINOR_STS_CONSTRUCT_TOPOLOGY_TREE     0x03
#define MINOR_STS_CALCULATE_ROUTE             0x04

/* Subcodes for STS_PROGRAM_FINAL_IO_SAD */
#define MINOR_STS_ALLOCATE_CPU_RESOURCE       0x01
#define MINOR_STS_FILL_SAD_TGTLST             0x02
#define MINOR_STS_PROGRAM_CPU_SAD_ENTRIES     0x03

/* Subcodes for STS_PROTOCOL_LAYER_SETTING */
#define MINOR_STS_PROGRAM_RING_CRDT           0x01
#define MINOR_STS_KTI_MISC_SETUP              0x02

/* Subcodes for STS_FULL_SPEED_TRANSITION */
#define MINOR_STS_FULL_SPEED_START            0x00
#define MINOR_STS_PHY_LAYER_SETTING           0x01  // Phy layer settings
#define MINOR_STS_LINK_LAYER_SETTING          0x02  // Link layer settings


/* KTI Error code - Major */
#define ERR_BOOT_MODE                         0xD8
#define ERR_MINIMUM_PATH_SETUP                0xD9
#define ERR_TOPOLOGY_DISCOVERY                0xDA
#define ERR_SAD_SETUP                         0xDB
#define ERR_UNSUPPORTED_TOPOLOGY              0xDC
#define ERR_FULL_SPEED_TRANSITION             0xDD
#define ERR_S3_RESUME                         0xDE
#define ERR_SW_CHECK                          0xDF

/* Subcodes for ERR_BOOT_MODE */
#define MINOR_ERR_UNSUPPORTED_BOOT_MODE       0x01
#define MINOR_ERR_PBSP_CHKIN_FAILURE          0x02
// Bit[7] - Timeout bit  Bits[3:0] - Socket Number
#define MINOR_ERR_PBSP_S0_BUS_PROGRAM_FAILURE 0x80
#define MINOR_ERR_PBSP_S1_BUS_PROGRAM_FAILURE 0x81
#define MINOR_ERR_PBSP_S2_BUS_PROGRAM_FAILURE 0x82
#define MINOR_ERR_PBSP_S3_BUS_PROGRAM_FAILURE 0x83
#define MINOR_ERR_PBSP_S4_BUS_PROGRAM_FAILURE 0x84
#define MINOR_ERR_PBSP_S5_BUS_PROGRAM_FAILURE 0x85
#define MINOR_ERR_PBSP_S6_BUS_PROGRAM_FAILURE 0x86
#define MINOR_ERR_PBSP_S7_BUS_PROGRAM_FAILURE 0x87
#define MINOR_ERR_PBSP_S8_BUS_PROGRAM_FAILURE 0x88
#define MINOR_ERR_PBSP_S9_BUS_PROGRAM_FAILURE 0x89
#define MINOR_ERR_PBSP_S10_BUS_PROGRAM_FAILURE 0x8A
#define MINOR_ERR_PBSP_S11_BUS_PROGRAM_FAILURE 0x8B
#define MINOR_ERR_PBSP_S12_BUS_PROGRAM_FAILURE 0x8C
#define MINOR_ERR_PBSP_S13_BUS_PROGRAM_FAILURE 0x8D
#define MINOR_ERR_PBSP_S14_BUS_PROGRAM_FAILURE 0x8E
#define MINOR_ERR_PBSP_S15_BUS_PROGRAM_FAILURE 0x8F

/* Subcodes for ERR_MINIMUM_PATH_SETUP/ERR_TOPOLOGY_DISCOVERY */
#define MINOR_ERR_ADD_SOCKET_TO_TOPOLOGY_TREE 0x01
#define MINOR_ERR_TOPOLOGY_TREE               0x02
#define MINOR_ERR_INTERNAL_DATA_STRUCTURE     0x03
#define MINOR_ERR_NONCPU_PARENT_NODE          0x04
#define MINOR_ERR_INVALID_PORT_CONNECTION     0x05
#define MINOR_ERR_SANITY_CHECK                0x06

/* Subcodes for ERR_IO_SAD_SETUP */
#define MINOR_ERR_INSUFFICIENT_RESOURCE_SPACE 0x01

/* Subcodes for ERR_UNSUPPORTED_TOPOLOGY */
#define MINOR_ERR_INVALID_SOCKET_TYPE         0x01
#define MINOR_ERR_INVALID_CPU_SOCKET_ID       0x02
#define MINOR_ERR_CBO_COUNT_MISMATCH          0x03
#define MINOR_ERR_HA_COUNT_MISMATCH           0x04
#define MINOR_ERR_M3KTI_COUNT_MISMATCH        0x05
#define MINOR_ERR_SKU_MISMATCH                0x06
#define MINOR_ERR_LEGACY_PCH_MISMATCH         0x07
#define MINOR_ERR_MCP_LINK_COUNT_MISMATCH     0x08
#define MINOR_ERR_INVALID_TOPOLOGY            0x09
#define MINOR_ERR_TOPOLOGY_CHANGE             0x0A

/* Subcodes for ERR_FULL_SPEED_TRANSITION */
#define MINOR_ERR_LINK_SPEED_UNSUPPORTED      0x01
#define MINOR_ERR_MAX_CLK_RATIO_UNSUPPORTED   0x02
#define MINOR_ERR_KTI_ELEC_PARAM_NOT_FOUND    0x03
#define MINOR_ERR_KTI_ELEC_PARAM_INVALID      0x04
// Bit[7] - LBC Timeout bit  Bits[3:0] - Socket Number
#define MINOR_ERR_S0_TIMEOUT_LBC              0x80
#define MINOR_ERR_S1_TIMEOUT_LBC              0x81
#define MINOR_ERR_S2_TIMEOUT_LBC              0x82
#define MINOR_ERR_S3_TIMEOUT_LBC              0x83
#define MINOR_ERR_S4_TIMEOUT_LBC              0x84
#define MINOR_ERR_S5_TIMEOUT_LBC              0x85
#define MINOR_ERR_S6_TIMEOUT_LBC              0x86
#define MINOR_ERR_S7_TIMEOUT_LBC              0x87
#define MINOR_ERR_S8_TIMEOUT_LBC              0x88
#define MINOR_ERR_S9_TIMEOUT_LBC              0x89
#define MINOR_ERR_S10_TIMEOUT_LBC             0x8A
#define MINOR_ERR_S11_TIMEOUT_LBC             0x8B
#define MINOR_ERR_S12_TIMEOUT_LBC             0x8C
#define MINOR_ERR_S13_TIMEOUT_LBC             0x8D
#define MINOR_ERR_S14_TIMEOUT_LBC             0x8E
#define MINOR_ERR_S15_TIMEOUT_LBC             0x8F
// Bit[7] - IAR Timeout bit  Bits[3:0] - Socket Number
#define MINOR_ERR_S0_TIMEOUT_IAR              0x90
#define MINOR_ERR_S1_TIMEOUT_IAR              0x91
#define MINOR_ERR_S2_TIMEOUT_IAR              0x92
#define MINOR_ERR_S3_TIMEOUT_IAR              0x93
#define MINOR_ERR_S4_TIMEOUT_IAR              0x94
#define MINOR_ERR_S5_TIMEOUT_IAR              0x95
#define MINOR_ERR_S6_TIMEOUT_IAR              0x96
#define MINOR_ERR_S7_TIMEOUT_IAR              0x97
#define MINOR_ERR_S8_TIMEOUT_IAR              0x98
#define MINOR_ERR_S9_TIMEOUT_IAR              0x99
#define MINOR_ERR_S10_TIMEOUT_IAR             0x9A
#define MINOR_ERR_S11_TIMEOUT_IAR             0x9B
#define MINOR_ERR_S12_TIMEOUT_IAR             0x9C
#define MINOR_ERR_S13_TIMEOUT_IAR             0x9D
#define MINOR_ERR_S14_TIMEOUT_IAR             0x9E
#define MINOR_ERR_S15_TIMEOUT_IAR             0x9F

/* Subcodes for ERR_S3_RESUME */
#define MINOR_ERR_S3_RESUME_TOPOLOGY_MISMATCH 0x01
#define MINOR_ERR_S3_RESUME_SETUP_MISMATCH    0x02

/* Subcodes for ERR_SW_CHECK */
/* NOTE: All subcodes are uniques */
#define MINOR_ERR_SW_CHECK_1                  0x01
#define MINOR_ERR_SW_CHECK_2                  0x02
#define MINOR_ERR_SW_CHECK_3                  0x03
#define MINOR_ERR_SW_CHECK_4                  0x04
#define MINOR_ERR_SW_CHECK_5                  0x05
#define MINOR_ERR_SW_CHECK_6                  0x06
#define MINOR_ERR_SW_CHECK_7                  0x07
#define MINOR_ERR_SW_CHECK_8                  0x08
#define MINOR_ERR_SW_CHECK_9                  0x09
#define MINOR_ERR_SW_CHECK_A                  0x0A
#define MINOR_ERR_SW_CHECK_B                  0x0B
#define MINOR_ERR_SW_CHECK_C                  0x0C
#define MINOR_ERR_SW_CHECK_D                  0x0D
#define MINOR_ERR_SW_CHECK_E                  0x0E
#define MINOR_ERR_SW_CHECK_F                  0x0F
#define MINOR_ERR_SW_CHECK_10                 0x10
#define MINOR_ERR_SW_CHECK_11                 0x11
#define MINOR_ERR_SW_CHECK_12                 0x12
#define MINOR_ERR_SW_CHECK_13                 0x13
#define MINOR_ERR_SW_CHECK_14                 0x14
#define MINOR_ERR_SW_CHECK_15                 0x15
#define MINOR_ERR_SW_CHECK_16                 0x16
#define MINOR_ERR_SW_CHECK_17                 0x17
#define MINOR_ERR_SW_CHECK_18                 0x18
#define MINOR_ERR_SW_CHECK_19                 0x19
#define MINOR_ERR_SW_CHECK_1A                 0x1A
#define MINOR_ERR_SW_CHECK_1B                 0x1B
#define MINOR_ERR_SW_CHECK_1C                 0x1C
#define MINOR_ERR_SW_CHECK_1D                 0x1D
#define MINOR_ERR_SW_CHECK_1E                 0x1E
#define MINOR_ERR_SW_CHECK_1F                 0x1F
#define MINOR_ERR_SW_CHECK_20                 0x20
#define MINOR_ERR_SW_CHECK_21                 0x21
#define MINOR_ERR_SW_CHECK_22                 0x22
#define MINOR_ERR_SW_CHECK_23                 0x23
#define MINOR_ERR_SW_CHECK_24                 0x24
#define MINOR_ERR_SW_CHECK_25                 0x25
#define MINOR_ERR_SW_CHECK_26                 0x26
#define MINOR_ERR_SW_CHECK_27                 0x27
#define MINOR_ERR_SW_CHECK_28                 0x28
#define MINOR_ERR_SW_CHECK_29                 0x29
#define MINOR_ERR_SW_CHECK_2A                 0x2A
#define MINOR_ERR_SW_CHECK_2B                 0x2B
#define MINOR_ERR_SW_CHECK_2C                 0x2C
#define MINOR_ERR_SW_CHECK_2D                 0x2D
#define MINOR_ERR_SW_CHECK_2E                 0x2E
#define MINOR_ERR_SW_CHECK_2F                 0x2F
#define MINOR_ERR_SW_CHECK_30                 0x30
#define MINOR_ERR_SW_CHECK_31                 0x31
#define MINOR_ERR_SW_CHECK_32                 0x32
#define MINOR_ERR_SW_CHECK_33                 0x33
#define MINOR_ERR_SW_CHECK_34                 0x34
#define MINOR_ERR_SW_CHECK_35                 0x35
#define MINOR_ERR_SW_CHECK_36                 0x36
#define MINOR_ERR_SW_CHECK_37                 0x37
#define MINOR_ERR_SW_CHECK_38                 0x38
#define MINOR_ERR_SW_CHECK_39                 0x39
#define MINOR_ERR_SW_CHECK_3A                 0x3A
#define MINOR_ERR_SW_CHECK_3B                 0x3B
#define MINOR_ERR_SW_CHECK_3C                 0x3C
#define MINOR_ERR_SW_CHECK_3D                 0x3D
#define MINOR_ERR_SW_CHECK_3E                 0x3E
#define MINOR_ERR_SW_CHECK_3F                 0x3F
#define MINOR_ERR_SW_CHECK_40                 0x40
#define MINOR_ERR_SW_CHECK_41                 0x41
#define MINOR_ERR_SW_CHECK_42                 0x42
#define MINOR_ERR_SW_CHECK_43                 0x43
#define MINOR_ERR_SW_CHECK_44                 0x44
#define MINOR_ERR_SW_CHECK_45                 0x45
#define MINOR_ERR_SW_CHECK_46                 0x46
#define MINOR_ERR_SW_CHECK_47                 0x47
#define MINOR_ERR_SW_CHECK_48                 0x48
#define MINOR_ERR_SW_CHECK_49                 0x49
#define MINOR_ERR_SW_CHECK_4A                 0x4A
#define MINOR_ERR_SW_CHECK_4B                 0x4B
#define MINOR_ERR_SW_CHECK_4C                 0x4C
#define MINOR_ERR_SW_CHECK_4D                 0x4D
#define MINOR_ERR_SW_CHECK_4E                 0x4E
#define MINOR_ERR_SW_CHECK_4F                 0x4F
#define MINOR_ERR_SW_CHECK_50                 0x50
#define MINOR_ERR_SW_CHECK_51                 0x51
#define MINOR_ERR_SW_CHECK_52                 0x52
#define MINOR_ERR_SW_CHECK_53                 0x53
#define MINOR_ERR_SW_CHECK_54                 0x54
#define MINOR_ERR_SW_CHECK_55                 0x55
#define MINOR_ERR_SW_CHECK_56                 0x56
#define MINOR_ERR_SW_CHECK_57                 0x57
#define MINOR_ERR_SW_CHECK_58                 0x58
#define MINOR_ERR_SW_CHECK_59                 0x59
#define MINOR_ERR_SW_CHECK_5A                 0x5A
#define MINOR_ERR_SW_CHECK_5B                 0x5B
#define MINOR_ERR_SW_CHECK_5C                 0x5C
#define MINOR_ERR_SW_CHECK_5D                 0x5D
#define MINOR_ERR_SW_CHECK_5E                 0x5E
#define MINOR_ERR_SW_CHECK_5F                 0x5F
#define MINOR_ERR_SW_CHECK_60                 0x60
#define MINOR_ERR_SW_CHECK_61                 0x61
#define MINOR_ERR_SW_CHECK_62                 0x62
#define MINOR_ERR_SW_CHECK_63                 0x63
#define MINOR_ERR_SW_CHECK_64                 0x64
#define MINOR_ERR_SW_CHECK_65                 0x65
#define MINOR_ERR_SW_CHECK_66                 0x66
#define MINOR_ERR_SW_CHECK_67                 0x67
#define MINOR_ERR_SW_CHECK_68                 0x68
#define MINOR_ERR_SW_CHECK_69                 0x69
#define MINOR_ERR_SW_CHECK_6A                 0x6A
#define MINOR_ERR_SW_CHECK_6B                 0x6B
#define MINOR_ERR_SW_CHECK_6C                 0x6C
#define MINOR_ERR_SW_CHECK_6D                 0x6D
#define MINOR_ERR_SW_CHECK_6E                 0x6E
#define MINOR_ERR_SW_CHECK_6F                 0x6F
#define MINOR_ERR_SW_CHECK_70                 0x70
#define MINOR_ERR_SW_CHECK_71                 0x71
#define MINOR_ERR_SW_CHECK_72                 0x72
#define MINOR_ERR_SW_CHECK_73                 0x73
#define MINOR_ERR_SW_CHECK_74                 0x74
#define MINOR_ERR_SW_CHECK_75                 0x75
#define MINOR_ERR_SW_CHECK_76                 0x76
#define MINOR_ERR_SW_CHECK_77                 0x77
#define MINOR_ERR_SW_CHECK_78                 0x78
#define MINOR_ERR_SW_CHECK_79                 0x79
#define MINOR_ERR_SW_CHECK_7A                 0x7A
#define MINOR_ERR_SW_CHECK_7B                 0x7B
#define MINOR_ERR_SW_CHECK_7C                 0x7C


/* KTI Warning Code - available thru KTI output structure only */
#define WARN_KTI                              0xD7
//
// Phy/Link layer
//
#define WARN_LINK_SLOW_SPEED_MODE             0x01
#define WARN_UNSUPPORTED_LINK_SPEED           0x02
#define WARN_LINK_FAILURE                     0x03
#define WARN_PER_LINK_OPTION_MISMATCH         0x04
#define WARN_INVALID_LINK                     0x05

//
// All other layers
//
#define WARN_MMIOH_BASE_UNSUPPORTED           0x09
#define WARN_MMIOH_SIZE_UNSUPPORTED           0x0A
#define WARN_RESOURCE_NOT_REQUESTED_FOR_CPU   0x0C
#define WARN_RESOURCE_REQUEST_NOT_MET         0x0D
#define WARN_VGA_TARGET_SOC_NOT_PRESENT       0x0E
#define WARN_VGA_TARGET_STACK_NOT_PRESENT     0x0F
#define WARN_CPU_STEPPING_MISMATCH            0x10
#define WARN_TSC_SYNC_UNSUPPORTED             0x11

#endif // _KTI_MISC_H_
