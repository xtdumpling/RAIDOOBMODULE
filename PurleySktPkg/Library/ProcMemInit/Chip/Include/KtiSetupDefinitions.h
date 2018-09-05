//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
 * KTI Reference Code
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
 *      This file contains KTIRC setup option value definitions.
 *
 ************************************************************************/

#ifndef  _kti_setup_definitions_h
#define  _kti_setup_definitions_h

//
// Values for enable/disable options
//
#define KTI_DISABLE 0
#define KTI_ENABLE  1
#define KTI_AUTO    2

//
// Link speed
//
#define SPEED_REC_96GT     0
#define SPEED_REC_104GT    1
#define MAX_KTI_LINK_SPEED 2
#define FREQ_PER_LINK      3

//
// Link Speed Modes
//
#define KTI_LINK_SLOW_SPEED_MODE 0
#define KTI_LINK_FULL_SPEED_MODE 1

//
// Degrade Precedence
//
#define TOPOLOGY_PRECEDENCE 0
#define FEATURE_PRECEDENCE  1

//
// VNA Credit override global setup variable values
//
#define  KTI_LL_VNA_SETUP_PER_LINK 0xff
#define  KTI_LL_VNA_SETUP_MAX      0x7f
#define  KTI_LL_VNA_SETUP_MIN      0x0

//
// Resource allocation settings
//
#define BUS_RATIO_MIN  0
#define BUS_RATIO_MAX  8
#define BUS_RATIO_DFLT 1

//
// IIO Uniphy disable options
//
#define IIO_UNIPHY_DIS_DO_NOT_DISABLE      0
#define IIO_UNIPHY_DIS_DISABLE_AND_CLKGATE 1
#define IIO_UNIPHY_DIS_DISABLE_NO_CLKGATE  2

//
// Legacy VGA allocation defaults
//
#define LEGACY_VGA_SOCKET_DFLT 0
#define LEGACY_VGA_SOCKET_MIN  0
#define LEGACY_VGA_SOCKET_MAX  3
#define LEGACY_VGA_STACK_DFLT  0
#define LEGACY_VGA_STACK_MIN   0
#define LEGACY_VGA_STACK_MAX   6

//
// KTI debug print Levels
//
#define KTI_DEBUG_PRINT_FATAL_ONLY         0x1
#define KTI_DEBUG_PRINT_WARNING_ONLY       0x2
#define KTI_DEBUG_PRINT_INFO_SUMMARY_ONLY  0x4
#define KTI_DEBUG_PRINT_INFO_DETAILED_ONLY 0x8
#define KTI_DEBUG_PRINT_ALL                0xf

//
// CRC modes
//
#define CRC_MODE_16BIT         0
#define CRC_MODE_ROLLING_32BIT 1
#define CRC_MODE_AUTO          2

//
// Hot plug topology settings
//
#define HOT_PLUG_TOPOLOGY_4S 0
#define HOT_PLUG_TOPOLOGY_8S 1

//
// IODC setup options
//
#define IODC_DISABLE                  0
#define IODC_AUTO                          1
#define IODC_EN_REM_INVITOM_PUSH           2
#define IODC_EN_REM_INVITOM_ALLOCFLOW      3
#define IODC_EN_REM_INVITOM_ALLOC_NONALLOC 4
#define IODC_EN_REM_INVITOM_AND_WCILF      5

//
// HA OSB settings
//
#define OSB_AD_VNA_THR_SI_MIN  0
#define OSB_AD_VNA_THR_SI_DFLT 0
#define OSB_AD_VNA_THR_SI_MAX  31
#define OSB_SNP_THR_SI_MIN     0
#define OSB_SNP_THR_SI_DFLT    1
#define OSB_SNP_THR_SI_MAX     7

//
// Bias FWD modes
//
#define BIAS_FWD_MODE0 0
#define BIAS_FWD_MODE1 1
#define BIAS_FWD_MODE2 2
#define BIAS_FWD_MODE3 3
#define BIAS_FWD_MODE4 4
#define BIAS_FWD_AUTO  5

//
// System Degrade Modes
//
#define DEGRADE_TOPOLOGY_1S        0 // Reduce the system to 1S
#define DEGRADE_SUPPORTED_TOPOLOGY 1 // Reduce the system to supported topology
#define DEGRADE_TOPOLOGY_AS_IS     2 // Leave the topology as is

//
// Values for DfxDualLinksInterleavingMode
//
#define DFX_DUAL_LINK_INTLV_MODE_CHA_INTLV   1
#define DFX_DUAL_LINK_INTLV_MODE_DISABLE_D2C 0

#endif // _kti_setup_definitions_h
