//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.05
//      Bug Fixed:  Add tRWSR w/a setup option for Samsung DIMMs.
//      Reason:     Improve memory RX margin
//      Auditor:    Stephen Chen
//      Date:       Aug/18/2017
//
//  Rev. 1.04
//      Bug Fixed:  Add tCCD_L Relaxation setup option for specific DIMMs.
//      Reason:     Requested by Micron, code reference from Jian.
//      Auditor:    Jacker Yeh
//      Date:       Mar/17/2017
//
//  Rev. 1.03
//      Bug Fixed:  Add NVDIMM Vender ID.
//      Reason:     
//      Auditor:    Leon Xu
//      Date:       Feb/25/2017
//
//  Rev. 1.02
//      Bug Fixed:  Add NVDIMM function
//      Reason:     
//      Auditor:    Leon Xu
//      Date:       Feb/17/2017
//
//  Rev. 1.01
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/7/2016
//
//  Rev. 1.00
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
//
//*****************************************************************************

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
  Copyright 2006 - 2016 Intel Corporation All Rights Reserved.

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
  MemHost.h

@brief
       Memory detection and initialization compliant with JEDEC specification.

 ************************************************************************/

#ifndef _memhost_h
#define _memhost_h
#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysRegs.h"
#include "MemRegs.h"
#include "MemDefaults.h"
#include "NGNDimmPlatformCfgData.h"
#include "MrcCommonTypes.h"
#include "MemHostChipCommon.h"
#include "KtiSi.h"
#include "Cpgc.h"

#define MAX_DIMMTABLEINDEX       (MAX_CH * MAX_DIMM)


#define MAXFIELDVAL(bitfield)     (bitfield = 0xffffffff)

//EFI_GUID definition locations for different BDAT/BSSA cases
#if defined(SSA_FLAG) && defined(MINIBIOS_BUILD)                              //Case1: For the MiniBIOS: BSSA with/without BDAT
#include "MrcSsaServices.h"
#elif !defined(SSA_FLAG) && defined(MINIBIOS_BUILD)  //Case2: For the MiniBIOS: BDAT without BSSA
#elif !defined(MINIBIOS_BUILD)                                               //Case3: For the Full BIOS: BDAT with/without BSSA (vice-versa)
//#include <Library\ProcessorStartup\ProcessorStartup.h>
#include <PiPei.h>
#endif //defined(SSA_FLAG) && defined(MINIBIOS_BUILD)

#ifdef MEMMAPSIM_BUILD
#define MMS_TABLE_TYPE_AEP      0
#define MMS_TABLE_TYPE_ID       1
#define MMS_TABLE_TYPE_CD       2
#define MMS_TABLE_TYPE_DP       3
#define MMS_TABLE_TYPE_BW       4
#define MMS_TABLE_TYPE_INVALID  5
#define MMS_TABLE_TYPES         MMS_TABLE_TYPE_INVALID
typedef struct PMFTableInfo {
  UINT32  headerRevision ;
  UINT32  headerOffset ;
  UINT32  headerLength ;
  UINT8   bodyRevision ;
  UINT32  bodyOffset ;
  UINT32  bodyLength ;
} PMFTableInfo ;
#endif
//
// IMC Vendor and Device Ids
//
#define IMC_VID     0x8086
#define IMC_DID     0x3CA8
#define ALL_DVID    0xFFFFFFFF

// Debug Build code
// These should be disabled for all normal builds and only enable on demand for debugging
//#define DEBUG_TURNAROUNDS         1
#define DEBUG_PERFORMANCE_STATS   1
//#define DEBUG_RDRAND              1
//#define DEBUG_SENSEAMP            1

#ifdef DEBUG_PERFORMANCE_STATS
#define  MAX_NOZONE     20
#endif  // DEBUG_PERFORMANCE_STATS

#define PGT_TIMER_ENABLE  1
#define PGT_TIMER_DISABLE 0

// Define the different HSX package Sku's
#define HSX_SKU_HCC 0
#define HSX_SKU_MCC 1
#define HSX_SKU_LCC 2

//
// DDR3 frequencies 800 - 2666
// DDR4 frequencies 1333 - 4200
//
#define MAX_SUP_FREQ        28                  // 26 frequencies are supported (800, 1067, 1333, 1600, 1867, 2133, 2400, 2666, 2933,
                                                //                               3200, 3400, 3467, 3600, 3733, 3800, 4000, 4200, 4266, 4400)
#define DDR_800  0   // tCK(ns)=2.5
#define DDR_1000 1   // tCK(ns)=2.0
#define DDR_1066 2   // tCK(ns)=1.875
#define DDR_1200 3   // tCK(ns)=1.667
#define DDR_1333 4   // tCK(ns)=1.5
#define DDR_1400 5   // tCK(ns)=1.429
#define DDR_1600 6   // tCK(ns)=1.25
#define DDR_1800 7   // tCK(ns)=1.11
#define DDR_1866 8   // tCK(ns)=1.07
#define DDR_2000 9   // tCK(ns)=1.0
#define DDR_2133 10  // tCK(ns)=0.9375
#define DDR_2200 11  // tCK(ns)=0.909
#define DDR_2400 12  // tCK(ns)=0.833
#define DDR_2600 13  // tCK(ns)=0.769
#define DDR_2666 14  // tCK(ns)=0.750
#define DDR_2800 15  // tCK(ns)=0.714
#define DDR_2933 16  // tCK(ns)=0.682
#define DDR_3000 17  // tCK(ns)=0.667
#define DDR_3200 18  // tCK(ns)=0.625
#define DDR_3400 19  // tCK(ns)=0.588
#define DDR_3466 20  // tCK(ns)=0.577
#define DDR_3600 21  // tCK(ns)=0.556
#define DDR_3733 22  // tCK(ns)=0.536
#define DDR_3800 23  // tCK(ns)=0.526
#define DDR_4000 24  // tCK(ns)=0.5
#define DDR_4200 25  // tCK(ns)=0.476
#define DDR_4266 26  // tCK(ns)=0.469
#define DDR_4400 27  // tCK(ns)=0.455

#define FREQUENCY_MTB_OFFSET            1000000
#define FREQUENCY_FTB_OFFSET            1000
#define DDR_800_TCK_MIN                 2500000
#define DDR_1000_TCK_MIN                2000000
#define DDR_1066_TCK_MIN                1875000
#define DDR_1200_TCK_MIN                1667000
#define DDR_1333_TCK_MIN                1500000
#define DDR_1400_TCK_MIN                1429000
#define DDR_1600_TCK_MIN                1250000
#define DDR_1800_TCK_MIN                1110000
#define DDR_1866_TCK_MIN                1071000
#define DDR_2000_TCK_MIN                1000000
#define DDR_2133_TCK_MIN                938000
#define DDR_2200_TCK_MIN                909000
#define DDR_2400_TCK_MIN                833000
#define DDR_2600_TCK_MIN                769000
#define DDR_2666_TCK_MIN                750000
#define DDR_2800_TCK_MIN                714000
#define DDR_2933_TCK_MIN                682000
#define DDR_3000_TCK_MIN                667000
#define DDR_3200_TCK_MIN                625000
#define DDR_3400_TCK_MIN                588300
#define DDR_3466_TCK_MIN                576900
#define DDR_3600_TCK_MIN                555600
#define DDR_3733_TCK_MIN                535800
#define DDR_3800_TCK_MIN                526400
#define DDR_4000_TCK_MIN                500000
#define DDR_4200_TCK_MIN                476200
#define DDR_4266_TCK_MIN                468900
#define DDR_4400_TCK_MIN                454600

// page size equates for tFAW table
#define PAGE_SIZE_1K    0
#define PAGE_SIZE_2K    1
#define PAGE_SIZE_HALFK 2

// JEDEC manufacturer IDs from JEP-106
#define MFGID_AENEON    0x5705
#define MFGID_QIMONDA   0x5105
#define MFGID_NEC       0x1000
#define MFGID_IDT       0xB300
#define MFGID_TI        0x9700
#define MFGID_HYNIX     0xAD00
#define MFGID_MICRON    0x2C00
#define MFGID_INFINEON  0xC100
#define MFGID_SAMSUNG   0xCE00
#define MFGID_TEK       0x3D00
#define MFGID_KINGSTON  0x9801
#define MFGID_ELPIDA    0xFE02
#define MFGID_SMART     0x9401
#define MFGID_AGILENT   0xC802
#define MFGID_NANYA     0x0B03
#define MFGID_INPHI     0xB304
#define MFGID_MONTAGE   0x3206
#define MFGID_RAMBUS    0x9D06
#define MFGID_INTEL     0x8900
#define MFGID_VIKING    0x4001

#define MFGID_INNODISK  0xF106  // SMCPKG_SUPPORT+
#define MFGID_NETLIST	0x1603	// SMCPKG_SUPPORT+
#define MFGID_AGIGA     0x3400  // SMCPKG_SUPPORT+

#define IDT_SPD_REGREV_B0          0x20
#define IDT_SPD_REGREV_C0          0x30
#define MONTAGE_SPD_REGREV_B1      0xB1
#define INPHI_SPD_REGREV_B0        0x01
#define TI_SPD_REGREV_A6           0x0B

#define MICRON_SPD_DRAMREV_I       0x49

#define IDT_SPD_REGREV_GEN2_B0     0x50
#define INPHI_SPD_REGREV_GEN2_C0   0x10
#define RAMBUS_SPD_REGREV_GEN2_B1  0x21
#define MONTAGE_SPD_REGREV_GEN2_A0 0xA0
#define MONTAGE_SPD_REGREV_GEN2_H0 0x80


#ifdef LRDIMM_SUPPORT
// This flag is for Inphi LRDIMM workarounds (Published)
#define INPHI_LRBUF_WA     1

#define LRBUF_INPHI_DVID      0x004018C7
#define LRBUF_INPHI_RID_A0    0x00
#define LRBUF_INPHI_RID_A1    0x01
#define LRBUF_INPHI_RID_B0    0x10

// This flag is for IDT LRDIMM workarounds (Published)
#define IDT_LRBUF_WA       1

#ifdef  IDT_LRBUF_WA
#define LRBUF_IDT_DVID          0xd230b380
#define LRBUF_IDT_RID_B0        IDT_SPD_REGREV_B0
#define LRBUF_IDT_SPD_REGVEN    MFGID_IDT
#define LRBUF_IDT_SPD_REGREV_B0 IDT_SPD_REGREV_B0
#define LRBUF_IDT_DB_RID_A0     0x10
#define LRBUF_IDT_DB_RID_GPB0   0xB0

// START IDT_Greendale_A2_1866_Sighting_102001 section 2 of 2
#define LRBUF_IDTGD_DVID        0xd231b380
#define LRBUF_IDTGD_RID_A2      0x12
// END IDT_Greendale_A2_1866_Sighting_102001 section 2 of 2
#endif // IDT_LRBUF_WA

//This flag is for Montage LRDIMM workarounds (Published)
#define MONTAGE_LRBUF_WA    1
#define LRBUF_MONTAGE_RID_A1      0xA1
#define LRBUF_MONTAGE_RID_GEN2_A0 0x10
#define LRBUF_MONTAGE_RID_GEN2_A1 0x11

#endif // LRDIMM_SUPPORT

//
// RC08 - Input/Output Configuration Control Word
//
#define QxC_2_0_enabled                    0
#define QxC_1_0_enabled_QxC_2_disabled     BIT0
#define QxC_2_1_disabled_QxC_0_enabled     BIT1
#define QxC_2_0_disabled                   BIT0 | BIT1
#define QxPAR_disabled                     BIT2  // Feature is enabled when Bit 2 is clear
#define DA17Input_QxA17Output_disabled     BIT3  // Feature is enabled with Bit 3 is clear


//
// ODT equates used to program mode registers
//
#define  MCODT_50        0
#define  MCODT_100       1

#define  MCODT_ACT       1
#define  ODT_ACT         1
#define  NO_ACT          0

#define DRAM_ODT_DIS      0
#define RTT_WR_ODT_60     BIT9
#define RTT_WR_ODT_120    BIT10
#define RTT_WR_ODT_MASK   (BIT10 | BIT9)
#define RTT_NOM_ODT_60    BIT2
#define RTT_NOM_ODT_120   BIT6
#define RTT_NOM_ODT_40    (BIT6 | BIT2)
#define RTT_NOM_ODT_20    BIT9
#define RTT_NOM_ODT_30    (BIT9 | BIT2)
#define RTT_NOM_ODT_MASK  (BIT9 | BIT6 | BIT2)

#define  RTT_WR_DIS     0
#define  RTT_WR_INF     0
#define  RTT_WR_60      BIT0
#define  RTT_WR_120     BIT1

#define  RTT_NOM_DIS     0
#define  RTT_NOM_INF     0
#define  RTT_NOM_60       BIT0
#define  RTT_NOM_120      BIT1
#define  RTT_NOM_40       BIT1 | BIT0
#define  RTT_NOM_20       BIT2
#define  RTT_NOM_48_DDR4  BIT2 | BIT0
#define  RTT_NOM_30       BIT2 | BIT0
#define  RTT_NOM_80_DDR4  BIT2 | BIT1
#define  RTT_NOM_34_DDR4  BIT2 | BIT1 | BIT0
#define  RTT_NOM_80       BIT2 | BIT1 | BIT0

#define DDR4_VREF_RANGE1_OFFSET   64
//JEDEC value
#define DDR4_VREF_RANGE2_OFFSET   41

#define DB_DRAM_VREF_RANGE2_OFFSET  23
#define CODE_ODT     0
#define DECODE_ODT   1

#define ODT_45_OHMS   45
#define ODT_50_OHMS   50
#define ODT_55_OHMS   55
#define ODT_100_OHMS  100

// DDR4 Rtt_wr
#define  DDR4_RTT_WR_DIS    0
#define  DDR4_RTT_WR_120    BIT0
#define  DDR4_RTT_WR_240    BIT1
#define  DDR4_RTT_WR_80     BIT2
#define  DDR4_RTT_WR_60     (BIT2 + BIT0) // Placeholder
#define  DDR4_RTT_WR_INF    (BIT1 | BIT0)

#define  RTTWR_DIS    0
#define  RTTWR_120    120
#define  RTTWR_240    240
#define  RTTWR_80     80
#define  RTTWR_60     60
#define  RTTWR_INF    999

// DDR4 Rtt_nom
#define  DDR4_RTT_NOM_DIS   0
#define  DDR4_RTT_NOM_INF   0
#define  DDR4_RTT_NOM_60    BIT0
#define  DDR4_RTT_NOM_120   BIT1
#define  DDR4_RTT_NOM_40    (BIT1 | BIT0)
#define  DDR4_RTT_NOM_240   BIT2
#define  DDR4_RTT_NOM_48    (BIT2 | BIT0)
#define  DDR4_RTT_NOM_80    (BIT2 | BIT1)
#define  DDR4_RTT_NOM_34    (BIT2 | BIT1 | BIT0)

// DDR4 Rtt_park
#define  DDR4_RTT_PARK_DIS   0
#define  DDR4_RTT_PARK_INF   0
#define  DDR4_RTT_PARK_60    BIT0
#define  DDR4_RTT_PARK_120   BIT1
#define  DDR4_RTT_PARK_40    (BIT1 | BIT0)
#define  DDR4_RTT_PARK_240   BIT2
#define  DDR4_RTT_PARK_48    (BIT2 | BIT0)
#define  DDR4_RTT_PARK_80    (BIT2 | BIT1)
#define  DDR4_RTT_PARK_34    (BIT2 | BIT1 | BIT0)

// ODT equates used to program mode registers
#define DDR4_RTT_WR_ODT_SHIFT 9
#define DDR4_RTT_WR_ODT_MASK  (BIT11 | BIT10 | BIT9)
#define DDR4_RTT_PRK_ODT_SHIFT 6
#define DDR4_RTT_PRK_ODT_MASK  (BIT8 | BIT7 | BIT6)
#define DDR4_RTT_NOM_ODT_SHIFT 8
#define DDR4_RTT_NOM_ODT_MASK  (BIT10 | BIT9 | BIT8)

#define DDR4_DRAM_RON_34 0
#define DDR4_DRAM_RON_48 1

#define DDR4_RON_34 34
#define DDR4_RON_40 40
#define DDR4_RON_48 48

#define ODT_RD_CYCLE_LEADING  0
#define ODT_RD_CYCLE_TRAILING 1

#define ODT_WR_CYCLE_LEADING  0
#define ODT_WR_CYCLE_TRAILING 0
#define DDR4_LRDIMM_ODT_WR_CYCLE_LEADING  0
#define DDR4_LRDIMM_ODT_WR_CYCLE_TRAILING 1

// Timing
#define SR_MODE                 0   // Same rank
#define DR_MODE                 1   // Different rank/same DIMM
#define DD_MODE                 2   // Different DIMM

#define RTT_CHANGE_SKEW   1

typedef enum {
  t_cke,
  t_xp,
  t_odt_oe,
  t_xsdll
}WORK_AROUND_TYPE;

//
// MemEarlyRid
//
#define   GNT2ERID_START  24

//
// The following bits enable the flow when set.
//
#define MF_X_OVER_EN        BIT0
#define MF_SENSE_AMP_EN     BIT1
#define MF_E_CMDCLK_EN      BIT2
#define MF_REC_EN_EN        BIT3
#define MF_RD_DQS_EN        BIT4
#define MF_WR_LVL_EN        BIT5
#define MF_WR_FLYBY_EN      BIT6
#define MF_WR_DQ_EN         BIT7
#define MF_CMDCLK_EN        BIT8
#define MF_RD_ADV_EN        BIT9
#define MF_WR_ADV_EN        BIT10
#define MF_RD_VREF_EN       BIT11
#define MF_WR_VREF_EN       BIT12
#define MF_RT_OPT_EN        BIT13
#define MF_RX_DESKEW_EN     BIT14 // Deskew are enabled/disabled together.
#define MF_TX_DESKEW_EN     BIT14 // Deskew are enabled/disabled together.
#define MF_TX_EQ_EN         BIT15
#define MF_IMODE_EN         BIT16
#define MF_EARLY_RID_EN     BIT17
#define MF_DQ_SWIZ_EN       BIT18
#define MF_LRBUF_RD_EN      BIT19
#define MF_LRBUF_WR_EN      BIT20
#define MF_RANK_MARGIN_EN   BIT21
#define MF_E_WR_VREF_EN     BIT22
#define MF_E_RD_VREF_EN     BIT23
#define MF_L_RD_VREF_EN     BIT24
#define MF_MEMINIT_EN       BIT25
#define MF_FNVSIMICSSIM_EN  BIT26
#define MF_NORMAL_MODE_EN   BIT27
#define MF_CMD_VREF_EN      BIT28
#define MF_L_WR_VREF_EN     BIT29
#define MF_MEMTEST_EN       BIT30
#define MF_E_CTLCLK_EN      BIT31

//
// memFlowsExt
//
#define MF_EXT_RX_CTLE_EN         BIT0
#define MF_EXT_PXC_EN             BIT1
#define MF_EXT_CMD_NORM_EN        BIT2
#define MF_EXT_LRDIMM_BKSIDE_EN   BIT3
#define MF_EXT_CLK_EN             BIT4
#define MF_EXT_DETECT_DIMM        BIT5
#define MF_EXT_CHECK_POR          BIT6
#define MF_EXT_INIT_DIMM_RANK     BIT7
#define MF_EXT_THROTTLING_EARLY   BIT8
#define MF_EXT_THROTTLING         BIT9
#define MF_EXT_POST_TRAINING      BIT10
#define MF_EXT_E_CONFIG           BIT11
#define MF_EXT_L_CONFIG           BIT12
#define MF_EXT_BIOS_SSA_RMT_EN    BIT13

#define MF_EXT_MCODT_EN           BIT14
#define MF_EXT_MCRON_EN           BIT15
#define MF_EXT_DIMMRON_EN         BIT16
#define MF_EXT_RTTWRT_EN          BIT17
#define MF_EXT_NONTGTODT_EN       BIT18
#define MF_EXT_TCO_COMP_EN        BIT19


//
// Get Set Mode (GSM) defines
//
//
// Mode Parameters
//
#define GSM_READ_CSR      BIT0      // Set to force the read from hardware. Clear to read form cache.
#define GSM_READ_ONLY     BIT1      // Set to skip the write flow.
#define GSM_WRITE_OFFSET  BIT2      // Set to use the "value" input as an offset. Set to use the "value" input as an absolute.
#define GSM_FORCE_WRITE   BIT3      // Set to force a write even if the data to be written matches the cached value.
#define GSM_UPDATE_CACHE  BIT4      // Set to update cached value with the read or write value
#define GSM_COMBINE_EDGES BIT5      // Set to use value1 as left/low side edge and value2 as right/high side edge
                                    // Clear to use value1 as center and don't use value2

//
// RTL Initialization Mode
//
#define RTL_INITIALIZE_ONCE   0
#define RTL_RE_INITIALIZE     1


///
/// MRC status
///
typedef enum {
  MRC_STATUS_SUCCESS,
  MRC_STATUS_STROBE_NOT_PRESENT,
  MRC_STATUS_RANK_NOT_PRESENT,
  MRC_STATUS_CH_NOT_PRESENT,
  MRC_STATUS_LEVEL_NOT_SUPPORTED,
  MRC_STATUS_GROUP_NOT_SUPPORTED,
  MRC_STATUS_SIGNAL_NOT_SUPPORTED,
  MRC_STATUS_GSM_LIMIT
} MRC_STATUS;

///
/// External signal names
///
typedef enum {
  RAS_N, CAS_N, WE_N,
  BA0, BA1, BA2,
  A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, A17,
  CS0_N, CS1_N, CS2_N, CS3_N, CS4_N, CS5_N, CS6_N, CS7_N, CS8_N, CS9_N,
  CKE0, CKE1, CKE2, CKE3, CKE4, CKE5,
  ODT0, ODT1, ODT2, ODT3, ODT4, ODT5,
  PAR, ALERT_N,
  BG0, BG1, ACT_N, C0, C1, C2,
  CK0, CK1, CK2, CK3, CK4, CK5,
  FNV_GOOD_PARITY, DESELECT, PRECHARGE, GNT,   // these are actually commands as opposed to signals
  gsmCsnDelim = INT32_MAX
} GSM_CSN;

typedef struct {
  GSM_GT  group;    ///< Command group number
  INT16   value;    ///< Offset selected by mode GSM_WRITE_OFFSET
} GSM_CGVAL;

typedef struct {
  GSM_GT  group;    ///< Command group number
  INT16   le;       ///< Left edge
  INT16   re;       ///< Right edge
} GSM_CGEDGE;

typedef struct {
  GSM_CSN signal;   ///< Command signal name
  INT16   value;    ///< Offset selected by mode GSM_WRITE_OFFSET
} GSM_CSVAL;

typedef struct {
  GSM_CSN signal;   ///< Command signal name
  INT16   le;       ///< Left edge
  INT16   re;       ///< Right edge
} GSM_CSEDGE;

typedef struct {
  GSM_CSN signal;   ///< Command signal name
  INT16   le;       ///< Left edge
  INT16   re;       ///< Right edge
  UINT8   toSample;
} GSM_CSEDGE_CTL;

typedef struct {
  GSM_CSN   signal;
  char      *signalStr;
} SIGNAL_STRING;

typedef struct {
  GSM_GT    group;
  char      *groupStr;
} GROUP_STRING;

typedef struct {
   INT16 left;
   INT16 right;
} SIGNAL_EYE;

typedef struct {
  UINT8    reqData;
  UINT16   freqValue;
} REQ_DATA_FREQ;

typedef struct {
  UINT16   freqValue;
  INT32    minTck;
} MIN_TCK;

typedef struct {
  GSM_GT  group;
  UINT16  minVal;
  UINT16  maxVal;
  UINT16  usDelay;
  char   *groupStr;
} GSM_LIMIT;

#define NORMAL_CADB_SETUP     0
#define RESTORE_CADB_PATTERN  1
#define UNMARGINED_CMD_EDGE   511

#define RECEIVE_ENABLE_BASIC  0
#define WRITE_LEVELING_BASIC  1
#define WR_FLY_BY             2
#define RX_DQ_DQS_BASIC       3
#define TX_DQ_BASIC           4
#define LATE_CMD_CLK          5
#define TRAIN_RD_DQS          6
#define TRAIN_WR_DQS          7
#define TRAIN_RD_VREF         8
#define TRAIN_WR_VREF         9
#define CROSSOVER_CALIBRATION 10
#define SENSE_AMP             11
#define EARLY_CMD_CLK         12
#define PER_BIT_DESKEW_RX     13
#define PER_BIT_DESKEW_TX     14
#define RX_VREF_TRAINING      15
#define ROUND_TRIP_OPTIMIZE   19
#define RX_BACKSIDE_PHASE_TRAINING  20
#define RX_BACKSIDE_CYCLE_TRAINING  21
#define RX_BACKSIDE_DELAY_TRAINING  22
#define TX_BACKSIDE_FINE_WL_TRAINING  23
#define TX_BACKSIDE_COARSE_WL_TRAINING  24
#define DISABLE_SENSE_AMP     25
#define EARLY_CTL_CLK         26
#define TX_EQ_TRAINING        27
#define T_TX_EQ               101
#define T_IMODE               102
#define T_CTLE                103
#define T_MC_RON              104
#define T_RTT_WR              105
#define T_MC_ODT              106
#define T_NT_ODT              107
#define T_DRAM_RON            108
#define T_RX_ODT              109
#define T_TCO_COMP            110
#define TX_BACKSIDE_DELAY_TRAINING  28
#define IMODE_TRAINING        29
#define TRAIN_WR_VREF_LRDIMM  30
#define CMD_VREF_CENTERING    36
#define DQ_SWIZZLE_DISCOVERY  37
#define EARLY_RID_FINE        38
#define EARLY_RID_COARSE      39
#define EARLY_RID_FINAL       40
#define MEM_TEST              43
#define PACKAGE_DELAY_CCC     44
#define E_TRAIN_RD_VREF       49
#define E_TRAIN_WR_VREF       50
#define TRAIN_RD_VREF_LRDIMM  51
#define TX_LRDIMM_DQ_CENTERING   52
#define RX_LRDIMM_DQ_CENTERING   53
#define PPR_FLOW              54
#define   PPR_PRE_TEST        55
#define   PPR_SEQ_BEGIN       56
#define   PPR_POST_TEST       57
#define CLK_TRAINING          58
#define BACKSIDE_TRAINING     59
#define PXC_TRAINING          60

#define CLEAR_MODE            0xFF
//
//  Training states defines
//
#define NOZONE              0
#define DIMM_DETECT         1
#define CHECK_DIMM_RANKS    2
#define GATHER_SPD          3
#define EARLY_CONFIG        4
#define JEDEC_INIT          5
#define CMD_CLK             6
#define REC_EN              7
#define RDDQDQS             8
#define WR_LVL              9
#define WRDQDQS             10
#define WR_FLYBY            11
#define CLOCK_INIT          12
#define RDDQDQS_ADV         13
#define WRDQDQS_ADV         14
#define RANK_MARGIN_TOOL    15
#define MEM_MAPPING         16
#define RAS_CONFIG          17
#define EYE_DIAGRAM         18
#define XOVER_CALIB         19
#define BIT_DESKEW_RX       20
#define RD_VREF             21
#define NORMAL_MODE         22
#define CMD_CLK_EARLY       23
#define BIT_DESKEW_TX       24
#define WR_VREF             25
#define RT_OPT              26
#define SENSE_AMPLIFIER     27
#define DDRIO_INIT          28
#define CTL_CLK_EARLY       29
#ifdef LRDIMM_SUPPORT
#define LRDIMM_RX           30
#define LRDIMM_TX           31
#define WR_VREF_LRDIMM      32
#endif
#define CMD_VREF_CEN        33
#define TRAIN_TX_EQ         34
#define TRAIN_IMODE         35
#define TRAIN_CTLE          36
#define TRAIN_MC_RON        37
#define TRAIN_RTT_WR        38
#define TRAIN_RX_ODT        39
#define TRAIN_DRAM_RON      40
#define SWIZZLE_DISCOVERY   41
#define E_WR_VREF           42
#define E_RD_VREF           43
#define RD_VREF_LRDIMM      44
#define TX_DQ_CENTERING_LRDIMM  45
#define RX_DQ_CENTERING_LRDIMM  46
#define E_RID               47
#define LATE_CONFIG         48
#define MEM_BIST            49
#define MEM_INIT            50
#define POST_PKG_RPR        51

#define MAX_PHASE           2                   // MAX_PHASE


//
//  Data to be tracked
//
#define PCI_ACCESS          0
#define JEDEC_COUNT         2
#define FIXED_DELAY         3
#define POLLING_COUNT       6
#define VREF_MOVE           7
#define CPGC_COUNT          8
#define DURATION_TIME       9



//
// DRAM timing equates
//
#define tDLLK   512
#define ZQ_SHORT  0
#define ZQ_LONG   1
#define tZQOPER 300
#define tZQinit 512 // define the tZQinit as define in jedec spec
#define tMRD    8   // HSD 5370282
#define tCCD_S_DEFINE 4

//
// MemTest definitions
//
#define MEMTEST 0
#define MEMINIT 1
#define WCRDT   32
#define RCRDT   22

//
// Advanced MemTest definitions
//
#define ADV_MT_TYPE_XMATS8     0
#define ADV_MT_TYPE_XMATS16    1
#define ADV_MT_TYPE_XMATS32    2
#define ADV_MT_TYPE_XMATS64    3
#define ADV_MT_TYPE_WCMATS8    4
#define ADV_MT_TYPE_WCMCH8     5
#define ADV_MT_TYPE_GNDB64     6
#define ADV_MT_TYPE_MARCHCM64  7
#define ADV_MT_TYPE_LTEST_SCRAM  8
#define ADV_MT_TYPE_LINIT_SCRAM  9

//
// JEDEC MRS definitions
//
#define BANK0   0
#define BANK1   BIT0
#define BANK2   BIT1
#define BANK3   BIT0 + BIT1
#define BANK4   BIT2
#define BANK5   BIT2 + BIT0
#define BANK6   BIT2 + BIT1
#define BANK7   BIT2 + BIT1 + BIT0

#define RDIMM_RC00     0x00
#define RDIMM_RC01     0x01
#define RDIMM_RC02     0x02
#define RDIMM_RC03     0x03
#define RDIMM_RC04     0x04
#define RDIMM_RC05     0x05
#define RDIMM_RC06     0x06
#define RDIMM_RC08     0x08
#define RDIMM_RC09     0x09
#define RDIMM_RC0A     0x0A
#define RDIMM_RC0B     0x0B
#define RDIMM_RC0C     0x0C
#define RDIMM_RC0D     0x0D
#define RDIMM_RC0E     0x0E
#define RDIMM_RC0F     0x0F
#define RDIMM_RC1x     0x10
#define RDIMM_RC2x     0x20
#define RDIMM_RC3x     0x30
#define RDIMM_RC4x     0x40
#define RDIMM_RC5x     0x50
#define RDIMM_RC6x     0x60
#define RDIMM_RC8x     0x80
#define RDIMM_RC9x     0x90
#define RDIMM_RCAx     0xA0
#define RDIMM_RCBx     0xB0

#define LRDIMM_BC00    0x00
#define LRDIMM_BC01    0x01
#define LRDIMM_BC02    0x02
#define LRDIMM_BC03    0x03
#define LRDIMM_BC04    0x04
#define LRDIMM_BC05    0x05
#define LRDIMM_BC06    0x06
#define LRDIMM_BC07    0x07
#define LRDIMM_BC08    0x08
#define LRDIMM_BC09    0x09
#define LRDIMM_BC0A    0x0A
#define LRDIMM_BC0B    0x0B
#define LRDIMM_BC0C    0x0C
#define LRDIMM_BC0E    0x0E

#define LRDIMM_BC0x    0x00
#define LRDIMM_BC1x    0x10
#define LRDIMM_BC2x    0x20
#define LRDIMM_BC3x    0x30
#define LRDIMM_BC4x    0x40
#define LRDIMM_BC5x    0x50
#define LRDIMM_BC6x    0x60
#define LRDIMM_BC7x    0x70
#define LRDIMM_BC8x    0x80
#define LRDIMM_BC9x    0x90
#define LRDIMM_BCAx    0xA0
#define LRDIMM_BCBx    0xB0
#define LRDIMM_BCCx    0xC0
#define LRDIMM_BCDx    0xD0
#define LRDIMM_BCEx    0xE0
#define LRDIMM_BCFx    0xF0
#define LRDIMM_F0      0x0
#define LRDIMM_F1      0x1
#define LRDIMM_F2      0x2
#define LRDIMM_F3      0x3
#define LRDIMM_F4      0x4
#define LRDIMM_F5      0x5
#define LRDIMM_F6      0x6
#define LRDIMM_F7      0x7
#define LRDIMM_F8      0x8
#define LRDIMM_F9      0x9

#define LRDIMM_BCAx_VALID   BIT0
#define LRDIMM_BCBx_VALID   BIT1
#define LRDIMM_BCDx_VALID   BIT2

//F0 BC03 bit definitions
#define LRDIMM_HOSTDQDQSDRIVERS_DISABLE BIT3

//F0 BC06 bit definitions
#define LRDIMM_ZQCL    0x01
#define LRDIMM_ZQCS    0x02

//F0 BC0A bit definitions
#define LRDIMM_BC0A_DDR4_1600  0x0
#define LRDIMM_BC0A_DDR4_1866  0x1
#define LRDIMM_BC0A_DDR4_2133  0x2
#define LRDIMM_BC0A_DDR4_2400  0x3
#define LRDIMM_BC0A_DDR4_2666  0x4
#define LRDIMM_BC0A_DDR4_3200  0x5

//F0 BC0C bit definitions
#define LRDIMM_NORMAL_MODE 0x0
#define LRDIMM_MREP_TRAINING_MODE 0x1
#define LRDIMM_DWL_TRAINING_MODE  0x4
#define LRDIMM_HWL_TRAINING_MODE  0x5
#define LRDIMM_MRD_TRAINING_MODE  0x6
#define LRDIMM_MWD_TRAINING_MODE  0x7

//F0BC1x bit definitions
#define LRDIMM_PBA_MODE 0x1
#define LRDIMM_MPR_OVERRIDE_MODE 0x2
#define LRDIMM_PDA_MODE 0x4
#define LRDIMM_ONE_RANK_TIMING_MODE 0x8
#define LRDIMM_RD_PREAMBLE_TRAINING_MODE 0x10

#define LFSR_ORDER   23
#define LFSR_POLY    0x840001



// Define the maximum number of day's between cold-boots
#define MEM_MAX_COLD_BOOT_DAYS      (90)  // Approximately 3 months

//#define FAULTY_PARTS_TRACKING_INJECT_ERROR    (1)
//#define FAULTY_PARTS_TRACKING_INJECT_2ND_ERROR    (1)

#define FPT_OK                      (0x00)
#define FPT_REC_ENABLE_FAILED       (BIT0)
#define FPT_RD_DQ_DQS_FAILED        (BIT1)
#define FPT_WR_LEVELING_FAILED      (BIT2)
#define FPT_WR_FLY_BY_FAILED        (BIT3)
#define FPT_WR_DQ_DQS_FAILED        (BIT4)
#define FPT_MEM_BIST_FAILED         (BIT5)
#define FPT_LRDIMM_TRAINING_FAILED  (BIT6)
#define FPT_TRAINING_FAILED         (BIT7)

#define FPT_PI_READ_TYPE            (BIT0)
#define FPT_PI_WRITE_TYPE           (BIT1)
#define FPT_PI_LRDIMM_READ_TYPE     (BIT2)
#define FPT_PI_LRDIMM_WRITE_TYPE    (BIT3)
#define FPT_PI_LRDIMM_RD_MRD_TYPE   (BIT4)
#define FPT_PI_LRDIMM_WR_MRD_TYPE   (BIT5)

#define PRINT_ARRAY_TYPE_LRDIMM_WL_PHASE     2
#define PRINT_ARRAY_TYPE_LRDIMM_RCV_EN_PHASE 3
#define PRINT_ARRAY_TYPE_LRDIMM_MDQ_RD_DELAY 4
#define PRINT_ARRAY_TYPE_LRDIMM_MDQ_WR_DELAY 5

#define FPT_NO_ERROR                (0x00)
#define FPT_CORRECTABLE_ERROR       (BIT0)
#define FPT_NO_CORRECTABLE_ERROR    (BIT1)

#define MAX_BITS_IN_BYTE      8                     // BITS per byte
#define DDRIO_DDR             0

#define BITS_PER_NIBBLE 4
//
// Command Clock
//
#define PER_GROUP             0
#define COMPOSITE_GROUPS      1

//
// Cross over Mode
//
#define XOVER_MODE_AUTO   0
#define XOVER_MODE_2TO2   1
#define XOVER_MODE_1TO1   2

//
// Cross over calibration
//
#define XOVER_CALIB_DIS       0
#define XOVER_CALIB_EN        1
#define XOVER_CALIB_AUTO      2

//
// Sense amp calibration
//
#define SENSE_AMP_DISABLE     0
#define SENSE_AMP_EN          1
#define SENSE_AMP_AUTO        2

//
// Common Core dummy defines
//

#ifndef MAX_MC_CH
#define MAX_MC_CH                2               // Max channels per MC
#endif
#ifndef MAX_CLUSTERS
#define MAX_CLUSTERS             1               // Maximum number of clusters supported
#endif

#ifndef MAX_EDC
#define MAX_EDC                  1               // Maximum number of EDC supported
#endif

#define CATCHALL_TIMEOUT    100000               // 100 ms

#pragma pack(push, 1)

extern const UINT16 rankSize[MAX_TECH];          ///< Rank size in 64 MB units
extern const UINT16 rankSizeDDR4[MAX_TECH];      ///< Rank size in 64 MB units
extern const UINT16 rankSizeAEP[MAX_TECH_AEP];


//
// Features Equates
//
#define DOUBLE_REFI       BIT1
#define EXTENDED_TEMP     BIT2
#define X4_PRESENT        BIT3
#define X8_PRESENT        BIT4
#define X4_AND_X8_PRESENT BIT5
#define ASR_SUPPORT       BIT6
#define X16_PRESENT       BIT7

//
// Mode register definitions
//
#define MR0_B4       BIT1  // Burst 4
#define MR0_ONTHEFLY BIT0  // On-the-fly Burst
#define MR0_RBT      BIT3  // Read Burst Type
#define MR0_TM       BIT7  // Mode (normal, test)
#define MR0_DLL      BIT8  // DLL reset
#define MR0_PPD      BIT12 // Precharge Power Down
#define MR1_DLL      BIT0  // DLL Enable
#define MR1_WLE      BIT7  // Write Leveling Enable
#define MR1_TDQS     BIT11 // TDQS Enable
#define MR1_QOFF     BIT12 // Output Buffer Enable
#define MR2_ASR      BIT6  // Auto Self Refresh
#define MR2_SRT      BIT7  // Self Refresh Temperature Range
#define MR2_ASR_DDR4 BIT6 | BIT7 //Auto Self Refresh
#define MR3_MPR      BIT2  // Multi Purpose Register
#define MR3_PDA      BIT4  // Per DRAM Addressability
#define MR4_RE       BIT10 // Read Preamble Training Mode

#define NO_PDA    0
#define ENTER_PDA 1
#define EXIT_PDA  2

#define ALL_DRAMS 0xFF  // Indicates to write to all DRAMs when in PDA mode
#define ALL_DATABUFFERS 0xFF // Indicates to write to all Buffers in PBA Mode
struct dataGroup {
  UINT8 rxVrefCenter;
#ifdef MARGIN_CHECK
  UINT8 txVrefCenter;
#endif
};

///
/// Rand per channel information
//
struct RankCh {
  UINT8             dimm;                             ///< DIMM this rank belongs to
  UINT8             rank;                             ///< Rank on the DIMM (0-3)
  UINT8             Roundtrip;
  UINT8             IOLatency;
  UINT8             RtIoPad;
  UINT16            RtIoMargin;
};

//
// Workarounds Equates
//
#define WA_SCRAMBLER_DIS    BIT0        // Disable scrambler for this iMC
#define WA_NO_MORE_RCW      BIT1        // Disable any more WriteRC and/or WriteRCLrBuf


typedef enum {
INVALID_BUS,
SMBUS,
EMRS,
CPGC,
SAD,
} BUS_TYPE;

#define MEM_TYPE_1LM        BIT0
#define MEM_TYPE_2LM        BIT1
#define MEM_TYPE_PMEM       BIT2
#define MEM_TYPE_PMEM_CACHE BIT3
#define MEM_TYPE_BLK_WINDOW BIT4
#define MEM_TYPE_CTRL       BIT5

#define BITMAP_CH0_CH1_CH2  ( ( BIT0 ) | (BIT1 ) | (BIT2) )
#define BITMAP_CH0_CH1      ( ( BIT0 ) | (BIT1 ) )
#define BITMAP_CH1_CH2      ( ( BIT1 ) | (BIT2 ) )
#define BITMAP_CH0_CH2      ( ( BIT0 ) | (BIT2 ) )
#define BITMAP_CH0       BIT0
#define BITMAP_CH1       BIT1
#define BITMAP_CH2       BIT2

#define DDRT_CTRL_SIZE 2  //128MB of NVMCTLR CSR/Mailbox/BLK CTRL region per NVMDIMM_IF dimm (in 64MB granularity)
#define DDRT_BLK_WINDOW_SIZE 1  //(in 64MB granularity)
#define DDRT_CTRL_DPA_START 0
#define DDRT_BLK_WINDOW_DPA_START DDRT_CTRL_SIZE
#define DDRT_VOL_DPA_START 4  //Volatile region starts above the control regions and BLK RW windows(which is 256MB in size including reserved fields)

#define MEM_MAP_LVL_NORMAL              0
#define MEM_MAP_LVL_IGNORE_CFGIN        1
#define MEM_MAP_LVL_IGNORE_CFGIN_IGNORE_NEW_DIMMS   2

#define MEM_MAP_VOL_MEM_MODE_UNDEFINED                 2

#define MEM_MAP_STATE_RESOURCE_CALCULATION             0
#define MEM_MAP_STATE_RESOURCE_CALCULATION_FAILURE     1
#define MEM_MAP_STATE_RESOURCE_CALCULATION_SUCCESS     2
#define MEM_MAP_STATE_PARTITION_FAILURE                3

//These are the error code that are used to handle Resource unavailability.
//The error codes begin at 0x20 to make sure it doesnt overlap with error codes defined for NGN dimms.
#define ERROR_RESOURCE_CALCULATION_COMPLETED           0x020
#define ERROR_RESOURCE_CALCULATION_FAILURE             0x021


//NVMDIMM training mode
#define DISABLE_TRAINING_MODE 0
#define ENABLE_TRAINING_MODE  1
#define DISABLE_TRAINING_STEP 2

//CL supported range
#define HIGH_CL_RANGE 1

//
// defines for mem ODT
//
#define  SLOT_NOT_PRESENT  0
#define  EMPTY_DIMM        1
#define  SR_DIMM           2
#define  DR_DIMM           3
#define  QR_DIMM           4
#define  LR_DIMM           5
#define  AEP_DIMM_TYPE     6

#define  DIMM0          0
#define  DIMM1          1
#define  DIMM2          2

#define  RANK0          0
#define  RANK1          1
#define  RANK2          2
#define  RANK3          3

//
// Defines for GetMargins inputs
//
#define WDB_BURST_LENGTH      32
#define WDB_END               7
#define LFSR0_SEED            0x0C6C6C
#define LFSR1_SEED            0x05A5A5
#define LFSR2_SEED            0

#ifdef SERIAL_DBG_MSG
#define MemDebugPrint(dbgInfo)  debugPrintMem dbgInfo
#define MspDebugPrint(dbgInfo)  debugPrintMsp dbgInfo
#define MmrcDebugPrint(dbgInfo)
#define OutputExtendedCheckpoint(dbgInfo)
#else
#define MemDebugPrint(dbgInfo)
#define MspDebugPrint(dbgInfo)
#define MmrcDebugPrint(dbgInfo)
#define OutputExtendedCheckpoint(dbgInfo) OutputCheckpoint dbgInfo
#endif

#ifdef MEM_NVDIMM_EN
#define NOT_NVDIMM                 0
#define NVDIMM_VENDOR_NOT_FOUND    1

#define NVDIMM_SUCCESS 0
#define NVDIMM_ERROR 1
#define NVDIMM_NODATA 2
#define NVDIMM_RESTORE 3

#define STATUS_RESTORE_NEEDED      (1 << 0)
#define STATUS_RESTORE_INPROGRESS  (1 << 1)
#define STATUS_RESTORE_SUCCESSFUL  (1 << 2)
#define STATUS_ARMED               (1 << 3)

#define ERROR_DETECT               (1 << 8)
#define ERROR_RESTORE              (1 << 9)
#define ERROR_ARM                  (1 << 10)
#define ERROR_SAVE                 (1 << 11)

#endif  // MEM_NVDIMM_EN

typedef enum
{
  MRC_PF_NULL,                // All policy flags turned off.
  MRC_PF_COLD         = BIT0, // Execute MRC function on cold reset.
  MRC_PF_FAST         = BIT1, // Execute MRC function on cold reset when S3 data is present.
  MRC_PF_WARM         = BIT2, // Execute MRC function on warm reset.
  MRC_PF_S3           = BIT3, // Execute MRC function on S3 exit.
  //MRC_PF_FULL_MRC     = BIT4,   // Execute MRC function when in Full MRC mode.
  //MRC_PF_MINI_MRC     = BIT5,   // Execute MRC function when in Mini-MRC mode.
  MRC_PF_ALL          = 0xF   // All policy flags turned off.
} PFSelectorType;

typedef enum
{
  MRC_MP_NULL,            // All policy flags turned off
  MRC_MP_SERIAL     = BIT0, // Execute function when in serial mode
  MRC_MP_PARALLEL   = BIT1, // Execute function when in parallel mode
  MRC_MP_LOOP       = BIT2, // Execute function for each socket when in serial mode
  MRC_MP_BOTH       = MRC_MP_SERIAL | MRC_MP_PARALLEL, // Execute function in both modes
  MRC_MP_BOTH_LOOP  = MRC_MP_SERIAL | MRC_MP_PARALLEL | MRC_MP_LOOP, // Execute function in both modes and loop
} MPSelectorType;

//
// TRR defines
//
#define PTRR_MODE   BIT0
#define TRR_MODE_A  BIT1
#define TRR_IMMUNE  BIT2
#ifdef TRR_MODE_B_SUPPORT
#define TRR_MODE_B  BIT3
#endif //TRR_MODE_B_SUPPORT

struct DimmPOREntryStruct {
  UINT32 procType;
  UINT8  socketType;
  UINT8  dramType;
  UINT8  spc;
  UINT8  dpc;
  UINT8  dimmType;
  UINT8  vdd;
  UINT8  numRanks;
  UINT8  porFreq;
  UINT8  stretchFreq;
};

#define DRV_GAP            (1 * MAX_PHASE_IN_FINE_ADJUSTMENT)  // define DRV_GAP default value.
#define IO_DIFF            (1 * MAX_PHASE_IN_FINE_ADJUSTMENT)  // define the IO diff value.
#define DRV_GAP_RD_WR      (1 * MAX_PHASE_IN_FINE_ADJUSTMENT)  // define the DRV GAP read write value.
#define TA_PAD             0
#define LR_RANK_MULT_TA    2

#define PERCENTAGE_TOLERANCE 95
#define POWERTRAINING_DEFAULT_SETTING -1

#define GET_MARGINS        0
#define TER_MARGINS        1
#define BER_MARGINS        2
#define MAX_SETTINGS       20
//#define MAX_ODT_SETTINGS   81 // 3*3 per DIMM --> 9x9 per CH --> 81x6 per mixed config NEEDS REDUCTION
#define MAX_GROUPS         4
#define MAX_PARAMS         4
#define PER_CH_PER_STROBE  0
#define PER_RANK           1
#define PER_STROBE         2
#define PER_BYTE           3
#define PER_CH             4
#define PER_MC             5
#define PER_CH_PER_DIMM    6
#define PER_CH_PER_BYTE    7
#define DEFAULT_PARAM      9
#define AVERAGE_FILTER     0
#define POWER_TRENDLINE    1
#define MAX_MC_ODT         2
#define MAX_RTT_PARK       8
#define MAX_RTT_NOM        8

#define DONT_CHECK_MAPOUT 0
#define CHECK_MAPOUT      BIT0

// Define the mode parameter
#define  CMD_CLK_DELAY_UPDATED      1

struct ioGroup {
  UINT8 num;
  UINT8 side;
};

struct signalIOGroup {
  GSM_CSN sig;
  GSM_GT  platformGroup;
  struct ioGroup group;
};

typedef union {
struct  {
   UINT32 channelindex:3;
   UINT32 rsvd_14_3:12;
   UINT32 selector:1;
   UINT32 cmdval:16;
}Bits;
UINT32 Data;
}CMD_WRITE_PWR_BW_SCALE_DATA;

typedef union {
struct  {
   UINT32 type   : 1;
   UINT32 scale  : 3;
   UINT32 rsvd_27:28;
}Bits;
UINT32 Data;
}MEMORY_PM_SCALE_DATA;

typedef union {
struct  {
   UINT32 channelindex:3;
   UINT32 rsvd_3:1;
   UINT32 adjustFactor:8;
   UINT32 rsvd_upper:20;
}Bits;
UINT32 Data;
}CMD_WRITE_DRAM_PBM_THRT_ADJUST_DATA;

typedef union {
struct  {
   UINT32 dimmpresent:16;
   UINT32 rsvd_8:8;
   UINT32 channelindex:8;
}Bits;
UINT32 Data;
}CMD_DDR_DIMM_PRESENT;

typedef union {
struct  {
   UINT32 Countertype:1;
   UINT32 counterbitvector:16;
   UINT32 rsvd_15:15;
}Bits;
UINT32 Data;
}CMD_DRAM_PM_RESET_CTR;

typedef union {
struct  {
   UINT32 Countertype:1;
   UINT32 counterbitvector:16;
   UINT32 rsvd_15:15;
}Bits;
UINT32 Data;
}CMD_DRAM_PM_START_CTR;

typedef union {
struct  {
   UINT32 Countertype:1;
   UINT32 counterbitvector:16;
   UINT32 rsvd_15:15;
}Bits;
UINT32 Data;
}CMD_DRAM_PM_STOP_CTR;

typedef union {
struct  {
   UINT32 Countertype:1;
   UINT32 counternumber2read:4;
   UINT32 rsvd_27:27;
}Bits;
UINT32 Data;
}CMD_DRAM_PM_READ_CTR;

typedef union {
struct  {
   UINT32 raplmode:2;
   UINT32 rsvd_30:30;
}Bits;
UINT32 Data;
}CMD_DRAM_PM_RAPL_MODE;

typedef union {
struct  {
   UINT32 vr0chmask:4;
   UINT32 vr1chmask:4;
   UINT32 rsvd_24:24;
}Bits;
UINT32 Data;
}CMD_DRAM_PM_CH_VR_MAP;

typedef union {
struct  {
   UINT32 imc0chmask:4;
   UINT32 imc1chmask:4;
   UINT32 rsvd_24:24;
}Bits;
UINT32 Data;
}CMD_B273548_WA_CHANNELS;

typedef struct  {
  UINT8  RAPL3SigmaPolicy;      ///< BIT1 = Page Policy (1=close Page / 0=open page), BIT0 = CKE (1=slow / 0=fast)
  UINT16 RAPLRanksPresentData[MAX_CH];    ///<  15:0= ranks present
  UINT16 RAPLDRAMPmWrPwrCOEFFValConst[0xF];
  UINT16 RAPLDRAMPmWrPwrCOEFFValPower[0xF];
  UINT8  RAPLMode;        ///< 0 = Estimated, 1=VR Measured
  UINT8  RAPLDramAdjVal;
  UINT16 RAPLDramPwrInfoMaxVal;
  UINT16 RAPLDramPwrInfoTdpVal;
  UINT16 RAPLDramPwrInfoMinVal;
  UINT16 RAPLWritePwrScaleSval[0xF];
  UINT16 RAPLWritePwrScaleMval;
  UINT8  RAPLDdrEnergyScaleFactor;
}DramRaplDataStruct;

typedef struct {
  UINT16 DramRaplDataDramPmWritePowerCoeffCval;
  UINT16 DramRaplDataDramMaxPwr;
  UINT16 DramRaplDataDramTdp;
  UINT16 DramRaplDataDramMinPwrClosedPage;
  UINT16 DramRaplDataDramWritePwrScaleClosedPage;
  UINT16 DramRaplDataDramMaxPwrOpenPage;
  UINT16 DramRaplDataDramTdpOpenPage;
  UINT16 DramRaplDataDramMinPwrOpenPage;
  UINT16 DramRaplDataDramWritePwrScaleOpenPage;
  UINT32 DramRaplDataDramRefreshRateSlope;
  UINT16 DramRaplDataDramSelfRefreshPower;
}DramRaplLUTDataStruct;

struct IoGroupDelayStruct {
  UINT8   ch;                         ///< Channel
  UINT8   iog;                        ///< IO Group #
  UINT8   side;                       ///< Side A or B
  UINT8   delay[CHIP_IOGPDLY_PSECS];       ///< Delay in Pico Seconds HSX 15C 10C 6C
};

///
/// Common MemThrot structure
///
struct DimmTTRowEntryStruct {
  UINT8  TTDDRType;
  UINT8  TTDimmType;
  UINT8  TTDramDensity;
  UINT8  TTSpdByte7;
  UINT8  TTFrequency;
  UINT8  TTAdjDimmInstalled;
  UINT8  TTDPC;
  UINT8  TTStaticData;
  UINT8  TTPower;
};

///
/// Common MemWeight Structure
///
struct DimmWTRowEntryStruct {
  UINT8  WTDimmType;
  UINT8  WTLvAndDramDensity;
  UINT8  WTSpdByte7;
  UINT8  WTSignalLoading;
  UINT8  WTDieCount;
  UINT8  WTDimmPos;
  UINT8  WTFrequency;
  UINT32 WTPmCmdPwrData;
  UINT16 DramPmWritePowerCoeff;
  UINT8  DramMaxPwr;
  UINT8  DramTdp;
  UINT8  DramMinPwrClosedPage;
  UINT16 DramWritePwrScaleClosedPage;
  UINT8  DramMaxPwrOpenPage;
  UINT8  DramTdpOpenPage;
  UINT8  DramMinPwrOpenPage;
  UINT16 DramWritePwrScaleOpenPage;
  UINT32 DramRefreshRateSlope;
  UINT16 DramSelfRefreshPower;
};

///
/// Common between RcRegsh
///
typedef union {
   struct{
      UINT32 emulation_type:8;
                            /* 00h = HW
                            4 = Simics */
      UINT32 rsvd_8:8;
      UINT32 comm_buf:8;
                        /* comminication buffer between BIOS and emulation environment */
      UINT32 rsvd_24:8;
   } Bits;
   UINT32 Data;
} EMULATION_INFO_UBOX_CFG_STRUCT, CSR_EMULATION_FLAG_IIO_PCIEDMI_STRUCT;

typedef struct {
  UINT8  stackPresentBitmap[MAX_SOCKET]; ///< bitmap of present stacks per socket
  UINT8  StackBus[MAX_SOCKET][MAX_IIO_STACK];  ///< Bus of each stack
  UINT8  SocketFirstBus[MAX_SOCKET];
  UINT8  Socket10nmUboxBus0[MAX_SOCKET];         //10nm CPU use only
  UINT8  SocketLastBus[MAX_SOCKET];
  UINT8  segmentSocket[MAX_SOCKET];
  UINT8  cpuType;
  UINT8  stepping;
  UINT32 socketPresentBitMap;
  UINT32 FpgaPresentBitMap;
  UINT32 mmCfgBase;
  UINT8  maxCh;
  UINT8  maxIMC;
  UINT8  numChPerMC;
  UINT8  imcEnabled[MAX_SOCKET][MAX_IMC];
  UINT8  mcId[MAX_SOCKET][MAX_CH];
  CPU_CSR_ACCESS_VAR_CHIP                 ///< Chip hook to enable CPU_CSR_ACCESS_VAR fields
} CPU_CSR_ACCESS_VAR;

#pragma pack(pop)
#define POWERTRENDLINE {0,100,200,300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900} // power level

///
/// (MPT_MT - MemeoryPowerTraining_MarginType)param type for power training steps
///
typedef enum {
  GetMargin   = 0,
  TerMargin   = 1,
  BerMargin   = 2
} MPT_MT;

///
/// (MPT_PT - MemeoryPowerTraining_ParamType)param type for power training steps
///
typedef enum {
  PerChPerByte   = 0,
  PerRank        = 1,
  PerStrobe      = 2,
  PerCh          = 3,
  PerMC          = 4
} MPT_PT;

///
/// (MPT_P - MemeoryPowerTraining_Param)param  for power training steps
///
typedef enum {
  traindramron    = 0,
  trainmcodt      = 1,
  trainnontgtodt  = 2,
  trainrttwr      = 3,
  trainmcron      = 4,
  traintxeq       = 5,
  trainimode      = 6,
  trainctle       = 7,
  traintcocomp    = 8,
} MPT_P;

#define IMC0 0
#define IMC1 1
struct vicAggr {
  UINT8         vicAggrLookup[MAX_CH][MAX_BITS][2]; // [x][y][0]= primary aggressor for that bit (index 0 based); [x][y][1]= secondary aggressor for that bit (index 0 based)
  UINT8         bitOrder[MAX_CH][MAX_BITS]; // order in which worst margins are treated as potential victim
  UINT8         MT[MAX_CH][MAX_BITS][7]; //[x][y][0] = DQ lane; [x][y][1]= victim; [x][y][2]= aggr0; [x][y][3]= aggr1; [x][y][4]=victim status; [x][y][5]=aggressor 0 status;[x][y][6]=aggressor 1 status;
  INT16         marginValue[MAX_CH][MAX_BITS]; // margin value for that bit (index 0 based)
  INT16         skewValue[MAX_CH][MAX_BITS];   // skew value for that bit (index 0 based)
  INT16         nibbleMaxSkew[MAX_CH][MAX_BITS/4][2];  // [x][z][0] = maxNibbleSkewBit(skew bit value); [x][z][1] = maxNibbleSkew(skew value)
};

#define DQ0 0
#define DQ1 1
#define DQ2 2
#define DQ3 3
#define DQ4 4
#define DQ5 5
#define DQ6 6
#define DQ7 7
#define DQ8 8
#define DQ9 9
#define DQ10 10
#define DQ11 11
#define DQ12 12
#define DQ13 13
#define DQ14 14
#define DQ15 15
#define DQ16 16
#define DQ17 17
#define DQ18 18
#define DQ19 19
#define DQ20 20
#define DQ21 21
#define DQ22 22
#define DQ23 23
#define DQ24 24
#define DQ25 25
#define DQ26 26
#define DQ27 27
#define DQ28 28
#define DQ29 29
#define DQ30 30
#define DQ31 31
#define DQ32 32
#define DQ33 33
#define DQ34 34
#define DQ35 35
#define DQ36 36
#define DQ37 37
#define DQ38 38
#define DQ39 39
#define DQ40 40
#define DQ41 41
#define DQ42 42
#define DQ43 43
#define DQ44 44
#define DQ45 45
#define DQ46 46
#define DQ47 47
#define DQ48 48
#define DQ49 49
#define DQ50 50
#define DQ51 51
#define DQ52 52
#define DQ53 53
#define DQ54 54
#define DQ55 55
#define DQ56 56
#define DQ57 57
#define DQ58 58
#define DQ59 59
#define DQ60 60
#define DQ61 61
#define DQ62 62
#define DQ63 63
#define DQ64 64
#define DQ65 65
#define DQ66 66
#define DQ67 67
#define DQ68 68
#define DQ69 69
#define DQ70 70
#define DQ71 71


//
// PPR Types
//
typedef enum {
  PprTypeSoft    = 0,
  PprTypeHard    = 1
} PPR_TYPE;

//
// PPR Status
//
#define PPR_STS_SUCCESS         0x00
#define PPR_STS_ADDR_VALID      0x01
#define PPR_STS_FAILED          0x02

//
// PPR Guard Keys
//
#define PPR_GUARD_KEY0  0xCFF
#define PPR_GUARD_KEY1  0x7FF
#define PPR_GUARD_KEY2  0xBFF
#define PPR_GUARD_KEY3  0x3FF

#define DRAM_UNKNOWN    0xFF

///
/// Memory Timings
///
typedef enum {
  tREFI = 0,
  tRFC,
  tRAS,
  tCWL,
  tCL,
  tRP,
  tRCD,
  tRRD,
  tRRD_S,
  tRRD_L,
  tWTR,
  tWTR_S,
  tWTR_L,
  tCCD,
  tCCD_S,
  tCCD_L,
  tRTP,
  tCKE,
  tFAW,
  tWR,
  tPRPDEN,
  tXP,
  tZQCS,
  tSTAB,
  tMOD,
  //tMRD,
  tREFIx9,
  tXPDLL,
  tXSOFFSET,
  tRRDR,
  tRRDD,
  tRC,
  tRDA,
  tWRA,
  tWRPRE,
  tRDPDEN,
  tWRPDEN,
  numMemTimingTypes,
} MEM_TIMING_TYPE;

typedef struct {
  MEM_TIMING_TYPE     timing;
  UINT32              minVal;
  UINT32              maxVal;
#ifdef SERIAL_DBG_MSG
  char                *timing_string_ptr;  ///< Output string describing this task (potentially output to debug serial port).
#endif //MRC_DEBUG_PRINT
} memTimingMinMaxType;

#pragma pack(push, 1)
//
// -----------------------------------------------------------------------------
///
/// Common DRAM timings
///
struct comTime {
  UINT8   nCL;
  UINT8   nWR;
  UINT8   nRCD;
  UINT8   nAL;
  UINT8   nRP;
  UINT8   nRC;
  UINT8   nWTR;
  UINT8   nWTR_L;
  UINT8   nRAS;
  UINT8   nRTP;
  UINT8   nFAW;
  UINT8   nRRD;
  UINT8   nRRD_L;
  UINT8   nWL;
  UINT8   nRTR;
  UINT8   nMDL;
  UINT8   nMDD;
  UINT16  nRFC;
  UINT16  nXS;
#if SMCPKG_SUPPORT
  UINT16  nCCD_L;
#endif
  UINT16  tAA;
  UINT16  tCL;
  UINT16  tRCD;
  UINT16  tRP;
  UINT16  tRC;
  UINT16  tRFC;
  UINT16  tAL;
  UINT16  tRRD;
  UINT16  tRRD_L;
  UINT16  tRAS;
  UINT16  tRTP;
  UINT16  tWR;
  UINT16  tWL;
  UINT16  tWTR;
  UINT16  tCCD;
  UINT16  tCCD_WR;
  UINT16  tCCD_L;
  UINT16  tCCD_WR_L;
  UINT32  casSup;
  UINT8   casSupRange;
  COMTIME_CHIP    ///< Chip hook to enable comTime fields
}; //struct comTime

struct lrbufRefresh {
   UINT8 refreshStagger;
   UINT16 nRFC;
};

//
// NVRAM structures for S3 state
//

///
/// DIMM rank info
/// List indexed by rank number
///
struct ddrRank {
  UINT8   enabled;                    ///< 0 = disabled,  1 = enabled
  UINT8   rankIndex;
  UINT8   phyRank;                    ///< Physical Rank #(3:0)
  UINT8   logicalRank;                ///< Logical Rank number (0 - 7)
  UINT8   ckIndex;                    ///< Index to the clock for this rank
  UINT8   ctlIndex;                   ///< Index to the control group for this rank
  UINT8   CKEIndex;
  UINT8   ODTIndex;
  UINT8   CSIndex;
  UINT8   devTagInfo;
  UINT16  RttWr;
  UINT16  RttPrk;
  UINT16  RttNom;
  UINT16  rankSize;                   ///< Units of 64 MB
  UINT16  remSize;                    ///< Units of 64 MB
#ifdef NVMEM_FEATURE_EN
  UINT16  NVrankSize;                   ///< Units of 64 MB
#endif
  UINT8   rxDQLeft;
  UINT8   rxDQRight;
  UINT8   txDQLeft;
  UINT8   txDQRight;
  UINT8   cmdLeft;
  UINT8   cmdRight;
  UINT8   cmdLow;
  UINT8   cmdHigh;
  UINT8   ctlLeft;
  UINT8   ctlRight;
  UINT8   rxVrefLow;
  UINT8   rxVrefHigh;
  UINT8   txVrefLow;
  UINT8   txVrefHigh;
  UINT8   rxDQLeftSt;
  UINT8   rxDQRightSt;
  UINT8   txDQLeftSt;
  UINT8   txDQRightSt;
  UINT8   rxVrefLowSt;
  UINT8   rxVrefHighSt;
  UINT8   txVrefLowSt;
  UINT8   txVrefHighSt;
  UINT8   faultyParts[MAX_STROBE];
#ifdef LRDIMM_SUPPORT
  UINT8   lrbufRxVref[MAX_STROBE];      ///< Actual value of backside RxVref
  UINT32  lrbufTxVref[MAX_STROBE];      ///< Actual value of backside TxVref
  UINT8   lrBuf_FxBC2x3x[MAX_STROBE];
  UINT8   lrBuf_FxBC4x5x[MAX_STROBE];
  UINT8   lrBuf_FxBC8x9x[MAX_STROBE];
  UINT8   lrBuf_FxBCAxBx[MAX_STROBE];
  UINT8   lrBuf_FxBCCxEx[MAX_STROBE/2];
  UINT8   lrBuf_FxBCDxFx[MAX_STROBE/2];
#endif
  DDRRANK_CHIP    ///< Chip hook to enable ddrRank fields
};  //struct ddrRank

///
/// DIMM information stored in NVRAM
//
struct dimmNvram {
  UINT8           dimmPresent;        ///< 1 = DIMM present in this slot
  UINT16          lrbufswizzle;
  INT32           minTCK;             ///< minimum tCK for this DIMM (SPD_MIN_TCK)
  UINT8           ftbTCK;             ///< fine offset for tCK
  UINT16          tCL;
  UINT16          tRCD;
  UINT16          tRP;
  UINT8           mapOut[MAX_RANK_DIMM]; ///< 1 = User requested rank made non-Present
  UINT8           numRanks;           ///< Number of logical ranks on this DIMM
  UINT8           numDramRanks;       ///< Number of physical DRAM ranks on this DIMM
  UINT8           techIndex;          ///< Index into DIMM technology table
  UINT8           aepTechIndex;       ///< Index into NVM DIMM technology table
  UINT8           fmcType;            ///< Far Memory Controller Type
  UINT8           fmcRev;             ///< Far Memory Controller Rev Type
  UINT8           SPDRawCard;         ///< Raw Card Number
  UINT8           SPDThermRefsh;      ///< SDRAM Thermal and Refresh Options
#ifdef PPR_SUPPORT
  UINT8           SPDOtherOptFeatures;///< SDRAM Other Optional features
#endif // PPR_SUPPORT
  UINT8           SPDThermSensor;     ///< Module Thermal Sensor
  UINT8           SPDThermHeatSprdr;  ///< Module Thermal Head Spreader Solution
  UINT8           SPDControlWords[2]; ///< Array of register control words
  UINT8           SPDAddrMapp;        ///< Address Mapping from Edge connector to DRAM
  UINT8           SPDRegRev;          ///< Register Revision
  UINT8           SPDODCtl;           ///< Register Output Drive Strength for Control
  UINT8           SPDODCk;            ///< Register Output Drive Strength for Clock
#ifdef LRDIMM_SUPPORT
  UINT8           spdLrBuf_F0_RC2_3;
  UINT8           spdLrBuf_F0_RC4_5;
  UINT8           spdLrBuf_F1_RC8_11;
  UINT8           spdLrBuf_F1_RC12_13;
  UINT8           spdLrBuf_F1_RC14_15;
  UINT8           spdLrBuf_MDQ_DS_ODT;
  UINT8           spdLrBuf_DRAM_QODT_ACT[4];
  UINT8           spdLrBuf_MR1_2_RTT;
  UINT8           spdLrBuf_150_MIN_MOD_DELAY;
  UINT8           spdLrBuf_150_MAX_MOD_DELAY;
  UINT8           SPDLrbufDramVrefdqR0; ///< DRAM VrefDQ for Package Rank 0
  UINT8           SPDLrbufDramVrefdqR1; ///< DRAM VrefDQ for Package Rank 1
  UINT8           SPDLrbufDramVrefdqR2; ///< DRAM VrefDQ for Package Rank 2
  UINT8           SPDLrbufDramVrefdqR3; ///< DRAM VrefDQ for Package Rank 3
  UINT8           SPDLrbufDbVrefdq; ///< LR Data Buffer VrefDQ for DRAM Interface
  UINT8           SPDLrbufDbDsRttLe1866; ///< LR Data Buffer MDQ Drive Strength and RTT for data rate <= 1866
  UINT8           SPDLrbufDbDsRttGt1866Le2400; ///< LR Data Buffer MDQ Drive Strength and RTT for data rate > 1866 and <= 2400
  UINT8           SPDLrbufDbDsRttGt2400Le3200; ///< LR Data Buffer MDQ Drive Strength and RTT for data rate > 2400 and <= 3200
  UINT8           SPDLrbufDramDs; ///< LR Buffer DRAM Drive Strength (for data rates <1866, 1866 < data rate < 2400, and 2400 < data rate < 3200)
  UINT8           SPDLrbufDramOdtWrNomLe1866; ///< LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate <= 1866
  UINT8           SPDLrbufDramOdtWrNomGt1866Le2400; ///< LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 1866 and <= 2400
  UINT8           SPDLrbufDramOdtWrNomGt2400Le3200; ///< LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 2400 and <= 3200
  UINT8           SPDLrbufDramOdtParkLe1866; ///< LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate <= 1866
  UINT8           SPDLrbufDramOdtParkGt1866Le2400; ///< LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 1866 and <= 2400
  UINT8           SPDLrbufDramOdtParkGt2400Le3200; ///< LR Buffer DRAM ODT (RTT_WR and RTT_NOM) for data rate > 2400 and <= 3200
  UINT8           lrBuf_F0RC14;
  UINT8           rcLrFunc;
  UINT8           lrBuf_BC00;
  UINT8           lrBuf_BC01;
  UINT8           lrBuf_BC02;
  UINT8           lrBuf_BC03;
  UINT8           lrBuf_BC04;
  UINT8           lrBuf_BC05;
  UINT8           lrBuf_BC0A;
  UINT8           lrBuf_BC1x;
  UINT8           lrBuf_F6BC4x;
  UINT8           lrBuf_F5BC4x[MAX_STROBE/2];
#ifdef    IDT_LRBUF_WA
  UINT32          idtSMBF6RC8;
  UINT32          idtF3RC6;
#endif    // IDT_LRBUF_WA
#endif    // LRDIMM_SUPPORT
  UINT8           XMPOrg;             ///< XMP organization and configuration
  UINT8           XMPRev;
  UINT8           mtbDividend;
  UINT32          XMPChecksum;
  UINT8           x4Present;          ///< Set if this is a x4 rank
  UINT8           keyByte;
  UINT8           keyByte2;           ///< Logical module type (for MRC compat)
  UINT8           aepDimmPresent;     ///< Is an NVM DIMM present?
  UINT8           actKeyByte2;        ///< Actual module type reported by SPD
  UINT8           SPDModuleOrg;       ///< Logical number of DRAM ranks and device width
  UINT8           actSPDModuleOrg;    ///< Actual number of DRAM ranks and device width
  UINT8           SPDDeviceType;      ///< Primary SDRAM Device Type
  UINT8           SPDSecondaryDeviceType; ///< Secondary SDRAM Device Type
  UINT8           numBanks;           ///< Number of banks
  UINT8           numBankGroups;      ///< Number of bank groups
  UINT8           sdramCapacity;      ///< SDRAM Capacity
  UINT8           numRowBits;         ///< Number of row address bits
  UINT8           numColBits;         ///< Number of column address bits
  UINT8           SPDSDRAMBanks;      ///< Logical number of banks and DRAM density
  UINT8           actSPDSDRAMBanks;   ///< Actual number of banks and DRAM density
  UINT8           actSPDSdramAddr;    ///< Actual number of DRAM rows and columns
  UINT8           dieCount;           ///< Number of DRAM dies per package
  UINT8           lrRankMult;         ///< Rank multiplication factor
  UINT8           SPDMemBusWidth;     ///< Width of the Primary bus and extension
  UINT8           dimmTs;             ///< Module Thermal Sensor
  UINT8           sdramType;
  UINT8           dimmHs;             ///< Heat spreader info
  UINT8           dimmAttrib;         ///< Module attributes
  UINT8           mtbDiv;             ///< medium time base divisor (SPD_MTB_DIV)
  UINT8           SPDftb;             ///< fine time base (SPD_FTB)
  UINT32          dimmMemTech;        ///< Used to program DIMMMTR
  UINT16          dramTcaseMax;       ///< Maximum tCase for each DIMM
  UINT16          SPDRegVen;          ///< Register Vendor ID
  UINT16          SPDMMfgId;          ///< Module Mfg Id from SPD
  UINT8           SPDMMfLoc;          ///< Module Mfg Location from SPD
  UINT16          SPDModDate;         ///< Module Manufacturing Date
  UINT16          SPDDramMfgId;       ///< DRAM Mfg Id
  UINT8           SPDDramRev;         ///< DRAM Rev Id
  UINT8           SPDModSN[4];        ///< Module Serial Number
  UINT8           SPDModPart[SPD_MODULE_PART];     ///< Module Part Number
  UINT8           SPDModPartDDR4[SPD_MODULE_PART_DDR4];      ///< Module Part Number DDR4
  UINT8           SPDModRevCode;      ///< Module Revision Code
  UINT8           SPDOptionalFeature; ///< DDR4 optional feature
  UINT16          SPDCrcDDR4;         ///< 16-bit CRC DDR4
//APTIOV_SERVER_OVERRIDE_RC_START  
  UINT16          MaxTdp;
//APTIOV_SERVER_OVERRIDE_RC_END
#ifdef MEM_NVDIMM_EN
  UINT32          nvDimmType;         ///< Used to index into the nv manufature array  Index valures 1 to NVDIMM vender operations table  0=not NVDIMM
  UINT32          nvDimmStatus;
  // Bit            Description
  //  0             NVDIMM controller failure
  //  1             NVDIMM restore failed
  //  2             NVDIMM restore retry
  //  3             NVDIMM backup failed
  //  4             NVDIMM erase failed
  //  5             NVDIMM erase retry
  //  6             NVDIMM arm failed
  //  7             No backup energy source detected
  //  8             Backup energy source charge failed
  //  9             NVDIMM uncorrectable memory error
  // 10             NVDIMM correctable memory memory error threshold
  // 11-15          Reserved
  UINT16          nvDimmErrorType;
  UINT8           nvDimmEnergyType;     // 0: 12V aux power; 1: dedicated backup energy source; 2: no backup energy source
  UINT16          funcdesc;
  UINT8           nvCtrlSmbAddr;        //SMBUS CONTROL ADDRESS
#endif  //MEM_NVDIMM_EN
  UINT32          lrbufDVid;          ///< LR Buffer Device and Vendor ID (from buffer not SPD)
  UINT8           lrbufRid;           ///< LR Buffer Revision ID (from buffer not SPD)
  UINT8           lrbufGen;           ///< LR Buffer Gen
  UINT8           SPDIntelSN[5];      ///< Intel DIMM serial number
  struct ddrRank  rankList[MAX_RANK_DIMM];
  UINT8           lrbufPersBytes[SPD_LR_PERS_BYTES_TOTAL];      ///< LRDIMM personality bytes
  UINT8           SPDmtb;             ///< DDR4 medium time base (SPD_TB_DDR4)
  UINT8           maxTCK;             ///< DDR4 maximum tCK for this DIMM
  UINT8           maxftbTCK;          ///< DDR4 max fine offset for tCK
  UINT8           ftbTRC;             ///< DDR4 fine offset for tRC
  UINT8           ftbTRP;             ///< DDR4 fine offset for tRP
  UINT8           ftbTRCD;            ///< DDR4 fine offset for tRCD
  UINT8           ftbTAA;             ///< DDR4 fine offset for tAA
  UINT8           SPDSpecRev;         ///< Revision of the SPD spec for this data
  UINT16          gnt2erid;
  UINT8           rcCache[16];
  UINT8           rcxCache[16];
  UINT8           lrDimmPresent;
  UINT8           rcClk;
  UINT8           rcCmd;
  UINT8           rcCtl;
  UINT16          rcVref;
#ifdef MEMMAPSIM_BUILD
  CHAR            *pmfFileName;
  PMFTableInfo    pmfTableInfo[MMS_TABLE_TYPES];
#endif
  UINT32          fnvioControl;
  UINT8           dqSwz[36];
  UINT16          rawCap;             ///< Raw Capacity
  DIMMNVRAM_CHIP    ///< Chip hook to enable dimmNvram fields
}; //struct dimmNvram

///
/// XMP Memroy Timings
///
struct memTiming {
  UINT8   nCL;  ///< 0, 5-11 where 0 = Auto
  UINT8   nRP;  ///< 0-15 where 0 = Auto
  UINT8   nRCD; ///< 0-15 where 0 = Auto
  UINT8   nRRD;
  UINT8   nRRD_L;
  UINT8   nWTR;
  UINT8   nRAS;
  UINT8   nRTP;
  UINT8   nWR;
  UINT8   nFAW;
  UINT8   nCWL;
  UINT8   nRC;
  UINT8   nCMDRate;
  UINT8   ddrFreqLimit;
  UINT16  vdd;
  UINT8   ucVolt;
  UINT32  casSup;
  UINT16  tREFI;
  UINT16  nRFC;
  UINT16  ddrFreq;
};

#define MAX_CMD_CSR 16
#define MAX_SIDE    2

///
/// Channel information stored in NVRAM
///
typedef struct channelNvram {
  UINT8             enabled;            ///< 0 = channel disabled, 1 = channel enabled
  UINT8             features;           ///< Bit mask of features to enable or disable
  UINT8             maxDimm;            ///< Number of DIMM
  UINT8             numRanks;           ///< Number of ranks on this channel
  UINT8             numQuadRanks;       ///< Number of QR DIMMs on this channel
  UINT8             timingMode;         ///< Command timing mode(1N, 2N, or 3N)
  UINT8             trainTimingMode;    ///< Command timing mode(1N, 2N, or 3N)
  UINT8             ckeMask;            ///< CKE signals to assert during IOSAV mode
  UINT8             chFailed;           ///< ddr4 memory in this channel had failed (MFO)
  UINT8             ngnChFailed;        ///< NGN memory in this channel had failed (MFO)
  UINT32            dataControl0;
  UINT32            dataControl1[MAX_STROBE];       ///< need to save for each strobe
  UINT32            dataControl2[MAX_STROBE];
  UINT32            dataControl4[MAX_STROBE];
  UINT32            dataControl3[MAX_STROBE];
  UINT32            ddrCRClkControls;
  struct comTime    common;             ///< Common timings for this channel
  struct memTiming  standard;
  struct dimmNvram  dimmList[MAX_DIMM];
  struct RankCh     rankPerCh[MAX_RANK_CH];
  UINT32            dimmVrefControl1;
  UINT32            dimmVrefControlFnv1;
  UINT32            dimmVrefControlFnv1Sa; // SA fub
  UINT32            ddrCrCmdPiCodingFnv;
  UINT32            ddrCrCmdPiCodingFnv2;
  UINT32            clkCsr;
  UINT32            txGroup0[MAX_RANK_CH][MAX_STROBE];
  UINT32            txGroup1[MAX_RANK_CH][MAX_STROBE];
  UINT32            txTco;
  UINT32            txXtalk[MAX_STROBE];        ///< Channels and bits are in each struct
  UINT32            rxGroup0[MAX_RANK_CH][MAX_STROBE];
  UINT32            rxGroup1[MAX_RANK_CH][MAX_STROBE];
  UINT32            rxOffset[MAX_RANK_CH][MAX_STROBE];
  UINT32            rxVrefCtrl[MAX_STROBE];
  UINT8             encodedCSMode;
  UINT8             cidBitMap;
  UINT8             txVrefSafe[MAX_RANK_CH];
  UINT8             dimmRevType;
#ifdef LRDIMM_SUPPORT
  UINT8             txVrefSafeLrbufLevel[MAX_RANK_CH];
#endif
  UINT8             txVrefCache[MAX_RANK_CH][MAX_STROBE]; ///< Cached value of txVref (this might not be the programmed value)
  UINT8             txVref[MAX_RANK_CH][MAX_STROBE];      ///< Actual current value of txVref
  UINT32            ddrCRCmdTrainingCmdN;
  UINT32            ddrCRCmdTrainingCmdS;
  UINT32            ddrCRCmdControls3CmdSFnv;
  UINT32            ddrCRCmdTrainingCmdSFnv;
  UINT32            ddrCRCtlTraining;
  UINT32            ddrCRCkeTraining;
  UINT32            ddrCRClkTraining;
  UINT32            ddrCRClkRanksUsed;
  UINT32            dataOffsetTrain[MAX_STROBE];
  UINT8             lrDimmPresent;
  UINT8             idtLrDimmPresent;
  UINT8             lrRankMultEnabled;
  UINT8             lrdimmExtAddrMode;
  UINT8             v150OnlyDimmPresent;    ///< A 1.5v Only DIMM exists on this channel
  UINT8             v120NotSupported;       ///< DDR4 DIMM on this channel does not support 1.2v
  UINT32            rankErrCountStatus;
  UINT32            rankErrCountInfo[MAX_RANK_CH/2];
  UINT32            rankErrThresholdInfo[MAX_RANK_CH/2];
  UINT8             ddrtEnabled;
  UINT8             spareInUse;
  UINT8             oldSparePhysicalRank;
  UINT8             spareDimm[MAX_RANK_CH/2];
  UINT8             spareRank[MAX_RANK_CH/2];
  UINT16            spareRankSize[MAX_RANK_CH/2];
  UINT8             spareLogicalRank[MAX_SPARE_RANK];
  UINT8             sparePhysicalRank[MAX_SPARE_RANK];
  UINT8             minRoundTrip;       ///< minimum roundtrip value per channel among all the valid ranks (0-7)
  UINT8             maxRoundTrip;       ///< maximum roundtrip value per channel among all the valid ranks (0-7)
  UINT32            roundtrip0;         ///< Even though roundtrip and IO latencies
  UINT32            roundtrip1;         ///<  are stored at the rank level, the register
  UINT32            ioLatency0;         ///<  is at the channel level.  Store a copy of
  UINT32            ioLatency1;         ///<  the register here so that only 1 PCI write
                                        //  per channel is required on S3 resume.
  UINT32            tcrwp;              ///< TCRWP_MCDDC_CTL_STRUCT
  UINT32            tcrap;              ///< TCRAP_MCDDC_CTL_STRUCT
  UINT32            tcothp;             ///< TCOTHP_MCDDC_CTL_STRUCT
  UINT32            tcothp2;
  UINT32            tclrdp;
  UINT32            ddrtDimmBasicTiming; //Commented out due to HSD5330978
  UINT32            TCMr0Shadow;
  UINT32            TCMr2Shadow;
  UINT32            TCMr4Shadow;
  UINT32            TCMr5Shadow;
  UINT8             txPerBitDeskew[MAX_RANK_CH][MAX_STROBE][4];
  UINT32            ddrCRCmdControls2CmdN;
  UINT32            ddrCRCmdControls2CmdS;
  UINT32            ddrCRCtlControlsCkeRanksUsed;
  UINT32            ddrCRCtlControlsRanksUsed;
  UINT8             tCCDAdder;
  CHANNELNVRAM_CHIP     ///< Chip hook to enable channelNvram fields
} CHANNEL_NVRAM_STRUCT, *PCHANNEL_NVRAM_STRUCT;

///
/// IMC information stored in NVRAM
///
struct imcNvram {
  UINT8               enabled;          ///< 0 = imc disabled, 1 = imc enabled
  UINT32              scrubMask;        ///< Scrub mask
  UINT32              scrubMask2;       ///< Scrub mask2
  UINT8               EmcaLtCtlMcMainExt;
  UINT32              ExRasConfigHaCfg;
  UINT32              SmiSpareCtlMcMainExt;
  UINT8               spdPageAddr;
  UINT32              ddrCRCompCtl0;
  IMCNVRAM_CHIP       ///< Chip hook to enable imcNvram fields
}; //struct imcNvram

///
/// Socket information stored in NVRAM
///
struct socketNvram {
  UINT8               enabled;
  INT32               minTCK;           ///< minimum tCK for this DIMM
  UINT8               ddrFreq;          ///< DDR Frequency of this socket
  UINT16              ddrFreqMHz;       ///< DDR Frequency of this socket in MHz
  UINT16              QCLKps;           ///< Qclk period in pS
  UINT8               cmdClkTrainingDone;
  UINT8               ddrVoltage;       ///< Voltage of this socket
  UINT8               lvDimmPresent;    ///< 1.35v DIMM
  UINT8               ulvDimmPresent;   ///< 1.25v DIMM
  UINT8               lrDimmPresent;
  UINT8               aepDimmPresent;
  UINT8               lrInitDone;
  UINT16              imcDid;           ///< IMC device ID
  UINT32              smbCmd0;
  UINT32              smbCmd1;
  UINT8               maxDimmPop;       ///< Maximum number of DIMM populated on a channel for a socket
  UINT8               wa;               ///< Bit field for workarounds
  UINT8               txVrefCenter;
  UINT8               ddr4SpdPageEn;
#ifdef  QR_DIMM_SUPPORT
  UINT8               qrPresent;
#endif  // QR_DIMM_SUPPORT
  UINT8               txVrefCache;                ///< Current value of txVref
  struct channelNvram channelList[MAX_CH];
  struct imcNvram     imc[MAX_IMC];
  UINT64_STRUCT       procPpin;         ///< Processor PPIN number
  UINT32              smiCtrlUboxMisc;
  UINT8               idtLrDimmPresent;
  UINT16              dramType;
  UINT8               disallowXorInterleave;
  UINT8               DimmWithoutThermalSensorFound;
  UINT8               refreshRate;
  UINT32              serialPacketNum;
  UINT32              serialUncompressedBytes;
  UINT32              serialCompressedBytes;
  SOCKETNVRAM_CHIP    ///< Chip hook to enable socketNvram fields
}; //struct socketNvram

//
// -----------------------------------------------------------------------------
//
// oemHooksSetup STRUCT 4t    ;  OEM BIOS-specific hooks
//
// oemCallback DWORD ?  ; OEM scratch data for BIOS callback function (optional)
//
// oemHooksSetup    ENDS
//
struct oemHooksSetup {
  UINT32  oemCallback;
};

//
// -----------------------------------------------------------------------------
//
// ddrChannelSetup STRUCT 4t    ;  Channel setup structure declaration
//
// enabled  BYTE ?   ; Channel enable switch:
//                   ; 0 = channel disabled
//                   ; 1 = channel enabled
//
// options  BYTE ?   ; Bit-mapped options:
//
// numDimmSlots BYTE ? ; Number of Dimm slots per channel
//                   ; Valid options are 1, 2 or 3
//                   ; MAX_DIMM is defined in mrcplatform.h.  This option can be no larger than MAX_DIMM.
//                   ; It overrides MAX_DIMM when it is smaller.
//
// ddrChannelSetup    ENDS
//
// -----------------------------------------------------------------------------
//

///
/// DIMM enable/disable information
///
struct ddrDimmSetup {
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  UINT8             Disable;
#endif
  UINT8             mapOut[MAX_RANK_DIMM];    ///< Set for each DIMM to be mapped out
};

///
/// Channel setup structure declaration
///
struct ddrChannelSetup {
  UINT8               enabled;            ///< Channel enable switch
  UINT8               options;            ///< Bit-mapped options
  UINT8               numDimmSlots;       ///< Number of DIMM slots per channel
  UINT8               batterybacked;
  UINT8               rankmask;
  struct ddrDimmSetup dimmList[MAX_DIMM];
  struct smbDevice    vrefDcp;            ///< Remove this field with DDR3 support
};

//
// -----------------------------------------------------------------------------
//
// Node bit-mapped options
//
// ddrSocketSetup STRUCT 4t    ;  Socket setup structure declaration
//
// enabled  BYTE ?   ; imc enable switch:
//                   ; 0 = imc disabled
//                   ; 1 = imc enabled
//
// options  BYTE ?   ; Bit-mapped options per socket:
//
// vrefDefaultValue BYTE ? ; Default DCP value per socket for DIMM Vref = Vddq/2
//
// vrefDcp smbDevice <>    ; Defines override of DCP SMBus device and address
//                         ; compId = DCP_ISL9072X or DCP_AD5247
//                         ; strapAddress
//                         ; busSegment
//
// ddrSocketSetup    ENDS
//
// -----------------------------------------------------------------------------
//

///
/// Socket setup structure declaration
///
struct ddrSocketSetup {
  UINT8                   enabled;             ///< imc enable switch
  UINT8                   options;             ///< Bit-mapped options per socket
  UINT8                   ddrVddLimit;         ///< Voltage of this socket
  struct ddrChannelSetup  ddrCh[MAX_CH];
  UINT8                   imcEnabled[MAX_IMC];
  SOCKETSETUP_CHIP                             ///< Chip hook to enable ddrSocketSetup fields
}; //struct ddrSocketSetup

///
/// PPR DRAM Address
///
typedef struct {
  UINT8     dimm;
  UINT8     rank;
  UINT8     subRank;
  UINT32    dramMask;
  UINT8     bank;
  UINT32    row;
} PPR_ADDR;

///
/// PPR Address
///
typedef struct {
  UINT8     pprAddrStatus;
  UINT8     socket;
  UINT8     mc;
  UINT8     ch;
  PPR_ADDR  pprAddr;
} PPR_ADDR_MRC_SETUP;

// HIGH_ADDR_EN enables extention of the MMIO hole to force memory to high address region
#define HIGH_ADDR_EN        BIT0
#define CR_MIXED_SKU        BIT2  //used to enable(1)- halt on mixed sku discovery and disable(0) - warn on mixed sku discovery

struct dfxMemSetup
{
  UINT8                 dfxDimmManagement;
  UINT8                 dfxPartitionDDRTDimm;
  UINT8                 dfxPartitionRatio[16];              // 16 for MAX_SOCKET 8 and 2 MC per socket
  UINT8                 dfxCfgMask2LM;                      // 0: Normal  1: Aggressive
  UINT8                 dfxMemInterleaveGranPMemNUMA;
  UINT8                 dfxMemInterleaveGranPMemUMA;
  UINT8                 dfxMaxNodeInterleave;
  UINT32                dfxOptions;
  UINT8                 dfxHighAddrBitStart;
  UINT8                 dfxLowMemChannel;                  // Optional channel to use for low memory if channel interleave = 1. 0 - Auto, 1 - ch 0, 2 - ch 1, etc.
  UINT8                 dfxPerMemMode;                     // 0: Non Persistent Mode 1: Persistent Mode 2: Persistent mode Enable
  UINT8                 dfxLoadDimmMgmtDriver;
};

//
// memSetup   STRUCT 4t   ; Host setup structure declaration
//
// mode     BYTE  ?  ; Enumerated host mode:
//                   ; CH_INDEPENDENT
//                   ; CH_MIRROR mirrors channel address space between channel
//                   ; CH_LOCK lockstep between channel 0 and 1 or 2 and 3
//                   ; RK_SPARE rank sparing
//
// ddrFreqLimit BYTE ?  ; Forces a DDR frequency slower than the common tCK detected via SPD.
//                   ; A DDR frequency faster than the common frequency is a config error.
//                   ; Options are 0=AUTO, 1=DDR_800, 3=DDR_1066, 5=DDR_1333, 7=DDR_1600, 9=DDR_1866, 11=DDR_2133, 13=DDR2400
//
// chInter BYTE ?    ; Channel interleave setting
//                   ; Valid options are 1, 2, 3, or 4 way interleave
//                   ; Other value defaults to 4 way interleave
//
// rankInter BYTE ?  ; Rank interleave setting
//                   ; Valid options are 1, 2, 4, or 8 way interleave
//                   ; Other value defaults to 8 way interleave
//
//
// spareErrTh  WORD ?   ; Spare error threshold
//                      ; Contains threshold to initiate channel sparing
//                      ; Valid settings are 1-15 when SPARE_EN = 1
//
// patrolScrubDuration DWORD ? ; Specifies the number of hours it takes for patrol scrub to scrub all system memory
//
//
// serialDebugMsgLvl BYTE ? ; Specifies what level of debug messages to display
//                         ; 0 = none, 1 = minimal status, 2, = all
//
// MemTestLoops WORD ; Number of MemTest loops to run. 0 = AUTO which means only 1 test will run if MemTest is enabled.
//                   ; This option is not valid if MemTest is disabled.
//
// dimmTypeSupport BYTE ? ; Type of DDR3 DIMM supported by this platform
//                   ; Valid options are 0 - RDIMM, 1 - UDIMM, 2 - Either
//                   ; RDIMMs and UDIMMs can not be populated together at the same time
//                   ; The MRC will halt with a fatal error if x16 UDIMMs and this option is set to 2.  Platform routing is not
//                   ; compatible.
//
// pcTimeoutCounter BYTE ? ; Sets MC_PAGETABLE_PARAMS1 bits[7:0] when ADAPTIVE_PAGE_EN==0
//                         ; 0 = Auto (2 for DP, 3 for UP)
//                         ; Non-zero value specifies timeout counter MSB
//
// vrefStepSize BYTE ?     ; Amount to inc/dec DIMM Vref value for each step
//
// vrefAbsMaxSteps BYTE ?  ; Max number of steps for DIMM Vref margin +/-10% of Vddq
//
// vrefOpLimitSteps BYTE ? ; Max number of steps for DIMM Vref margin +/-5% of Vddq/2
//
// ckeThrottling  byte ?  ; CKE Power managment mode
//                        ; 0 = Disabled
//                              pdwn_mode_apd = 0
//                              pdwn_mode_ppd = 0
//                              pdwn_mode_slow_exit = 0
//                        ; 1 = APD Enabled, PPD Disabled
//                              pdwn_mode_apd = 1
//                              pdwn_mode_ppd = 0
//                              pdwn_mode_slow_exit = 0
//                        ; 2 = APD Disabled, PPDF Enabled
//                              pdwn_mode_apd = 0
//                              pdwn_mode_ppd = 1
//                              pdwn_mode_slow_exit = 0
//                        ; 3 = APD Disabled, PPDS Enabled
//                              pdwn_mode_apd = 0
//                              pdwn_mode_ppd = 1
//                              pdwn_mode_slow_exit = 1
//                        ; 4 = APD Enabled, PPDF Enabled
//                              pdwn_mode_apd = 1
//                              pdwn_mode_ppd = 1
//                              pdwn_mode_slow_exit = 0
//                        ; 5 = APD Enabled, PPDS Enabled
//                              pdwn_mode_apd = 1
//                              pdwn_mode_ppd = 1
//                              pdwn_mode_slow_exit = 1
// vrefDelayUs WORD ?      ; Number of us to delay after changing DIMM Vref
//
// options DWORD  ?  ; Bit-mapped host options (defined above in memSetup.options section)
//
// oemHooks oemHooksSetup <> ; Setup structure containing external hooks for OEM BIOS
//
// imc ddrSocketSetup   2t DUP (<>) ; Setup structures for socket 0 and socket 1
//                                     ; See ddrSocketSetup description (above)
//
//
// memSetup      ENDS
//

//
// advMemTestOptions
//
#define ADV_MT_XMATS8     (1 << ADV_MT_TYPE_XMATS8)
#define ADV_MT_XMATS16    (1 << ADV_MT_TYPE_XMATS16)
#define ADV_MT_XMATS32    (1 << ADV_MT_TYPE_XMATS32)
#define ADV_MT_XMATS64    (1 << ADV_MT_TYPE_XMATS64)
#define ADV_MT_WCMATS8    (1 << ADV_MT_TYPE_WCMATS8)
#define ADV_MT_WCMCH8     (1 << ADV_MT_TYPE_WCMCH8)
#define ADV_MT_GNDB64     (1 << ADV_MT_TYPE_GNDB64)
#define ADV_MT_MARCHCM64  (1 << ADV_MT_TYPE_MARCHCM64)
#define ADV_MT_LTEST_SCRAM     (1 << ADV_MT_TYPE_LTEST_SCRAM)
#define ADV_MT_LINIT_SCRAM     (1 << ADV_MT_TYPE_LINIT_SCRAM)

///
/// Host setup structure declaration
///
struct memSetup {
  UINT32                options;
  UINT32                optionsExt;
  UINT32                optionsNgn;
  UINT8                 bclk;                            ///< BCLK frequency (100, 133, 145,..) 0 means use the default
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  UINT8                 MemMapOut;
#endif  //#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  UINT8                 imcBclk;                         ///< IMC BCLK frequency (Auto, 100, 133)
  UINT8                 enforcePOR;
  UINT8                 RASmodeEx;
  UINT8                 ddrFreqLimit;
  UINT8                 ddrVccu;
  UINT8                 chInter;
  UINT8                 dimmTypeSupport;
  UINT8                 vrefStepSize;                    ///< Amount to inc/dec DIMM Vref value for each step
  UINT8                 vrefAbsMaxSteps;                 ///< Max number of steps for DIMM Vref margin +/-10% of Vddq
  UINT8                 vrefOpLimitSteps;                ///< Max number of steps for DIMM Vref margin +/-5% of Vddq/2
  UINT8                 pdwnCkMode;                      ///< Power Down Clock Modes for UDIMM or RDIMM
  UINT8                 MemPwrSave;
  UINT8                 ckeThrottling;
  UINT8                 olttPeakBWLIMITPercent;          ///< (value/100) * 255 / max number of dimms per channel = DIMM_TEMP_THRT_LMT THRT_HI
  UINT8                 thermalThrottlingOptions;        ///< Bitmapped field for Thermal Throttling Modes (defined in mem.thermalThrottlingOptions section)
  UINT8                 memhotOutputOnlyOpt;             ///<  Thermal Throttling O/P bits - (High | Mid | Low); 0= Memhot output disabled,1 = Memhot on High,2 = Memhot on High|Mid, 3 = Memhot on High|Mid|Low
  UINT8                 dramraplen;                      ///< Initialize DRAM RAPL
  UINT8                 dramraplbwlimittf;               ///< Allows custom tuning of BW_LIMIT_TF when DRAM RAPL is enabled
  UINT8                 CmsEnableDramPm;
  UINT8                 dramraplRefreshBase;             ///< Allows custom tuning of Power scaling by Refresh rate in units of 0.1x when DRAM RAPL is enabled
  UINT8                 logParsing;                      ///< if set to 1, additional information will appear in log for easier parsing.
  UINT8                 altitude;                        ///< System Altitude for Memory Thermal Throttling Calculations
  UINT8                 forceRankMult;                   ///< Force LRDIMM rank multiplication factor
  UINT8                 lrdimmModuleDelay;               ///< LRDIMM Module Delay Auto/Disabled
  UINT8                 lrdimm_x4asx8;                   ///< Option to run x4 LRDIMM as x8 for power savings
  UINT8                 rxVrefTraining;                  ///< Enable disable RX Vref Training
  UINT8                 perBitDeSkew;                    ///< Enable disable per Bit DeSkew Training
  UINT8                 iotMemBufferRsvtn;               ///< reserves a block of memory as IOT trace buffer for each CPU socket
#if SMCPKG_SUPPORT
  UINT8                 tCCDLRelaxation;
  UINT8                 tRWSREqualization;
#endif
#ifdef SSA_FLAG
  UINT8                   enableBiosSsaLoader;             ///< Enables BIOS SSA EV loader
  UINT8                   enableBiosSsaRMT;                 ///< Enables BIOS SSA Stitched Mode (RMT)
  UINT8                   enableBiosSsaRMTonFCB;            // Enables BIOS SSA Stitched Mode (RMT) on FCB
  UINT8                   biosSsaPerBitMargining;           ///< BSSA RMT Per-Bit Margining
  UINT8                   biosSsaDisplayTables;              ///< BSSA RMT Display Tables
  UINT8                   biosSsaPerDisplayPlots;           ///<  BSSA RMT Display Plots
  UINT8                   biosSsaLoopCount;
  UINT8                   biosSsaBacksideMargining;        ///< BSSA RMT Backside Margining
  UINT8                   biosSsaEarlyReadIdMargining;    ///< BSSA RMT Early Read ID Margining
  UINT8                   biosSsaStepSizeOverride;           ///< BSSA RMT Step Size Override
  UINT8                   biosSsaRxDqs;
  UINT8                   biosSsaRxVref;
  UINT8                   biosSsaTxDq;
  UINT8                   biosSsaTxVref;
  UINT8                   biosSsaCmdAll;
  UINT8                   biosSsaCmdVref;
  UINT8                   biosSsaCtlVref;
  UINT8                   biosSsaCtlAll;
  UINT8                   biosSsaEridDelay;
  UINT8                   biosSsaEridVref;
  UINT8                   biosSsaDebugMessages;           ///< BSSA RMT Debug Messages
#endif //SSA_FLAG
  UINT8                 enforceThreeMonthTimeout;        ///< Option to disable 3 month timeout to re-train memory
#ifdef LRDIMM_SUPPORT
  UINT8                 enableBacksideRMT;               ///< Enable Backside RMT
  UINT8                 enableBacksideCMDRMT;            ///< Enable Backside CMD RMT
#endif
  UINT8                 enableNgnBcomMargining;          ///< Enable NVMDIMM BCOM margining support
  UINT8                 trainingResultOffsetFunctionEnable;
  INT16                 offsetTxDq; ///<Dq Write
  INT16                 offsetRxDq; ///<Dq Read
  INT16                 offsetTxVref;
  INT16                 offsetRxVref;
  INT16                 offsetCmdAll;
  INT16                 offsetCmdVref;
  INT16                 offsetCtlAll;
  UINT8                 setMCODT;                        ///< setMCODT: 0 = 50ohms, 1 = 100ohms, 2 = AUTO
#ifdef MARGIN_CHECK
  UINT32                rmtPatternLength;
  UINT32                rmtPatternLengthExt;
#endif  // MARGIN_CHECK
  UINT32                patrolScrubDuration;
  UINT8                 DieSparing;
  UINT8                 DieSparingAggressivenessLevel; ///<NGN Die sparing aggressiveness (0...255), default 128
  UINT8                 ADDDCEn;
  UINT8                 NgnAddressRangeScrub;
  UINT8                 NgnHostAlertAit;      ///<Signal Poison or Viral upon receiving Address Indirection Table Error
  UINT8                 NgnHostAlertDpa;      ///<Signal Poison or Viral upon receiving DIMM Physical Address Error
  UINT8                 advMemTestEn;         ///< Global enable of advanced CPGC MemTest step
  UINT32                advMemTestOptions;    ///< Bit fields to enable different advanced MemTest options
  UINT16                memTestLoops;                    ///< Number of MemTests to execute
  UINT16                scrambleSeedLow;                 ///< Low 16 bits of the data scrambling seed
  UINT16                scrambleSeedHigh;                ///< High 16 bits of the data scrambling seed
  UINT8                 ADREn;
#if SMCPKG_SUPPORT
  UINT8                 SmcGlobalResetEn;
  UINT8                 SmcGracefulShutdownTriggerADR;
  UINT8                 SmcADRCompleteTimeout;
  UINT8                 SmcHideADR;
  UINT8                 SmcSetNVDIMM2WT;
#endif
  UINT8                 LegacyADRModeEn;
#ifdef NVMEM_FEATURE_EN
  UINT8                 ADRDataSaveMode;
#endif
  UINT8                 check_pm_sts;
  UINT8                 check_platform_detect;
  UINT8                 mcBgfThreshold;                  ///< The HA to MC BGF threshold is used for scheduling MC request in bypass condition.
  UINT16                normOppIntvl;                    ///< Normal operation interval.
  UINT8                 SpdSmbSpeed;                     ///< DDR4 SMB speed
  struct oemHooksSetup  oemHooks;
  struct ddrSocketSetup socket[MAX_SOCKET];
  struct memTiming      inputMemTime;
  UINT32                XMPChecksum[MAX_SOCKET][MAX_CH][MAX_DIMM];
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  UINT32                meRequestedSize;                 ///< Size requested for ME UMA in 1MB units
#endif // ME_SUPPORT_FLAG
  UINT8                 phaseShedding;                   ///< Enable/Disable memory VR phase shedding feature
  UINT8                 DimmTempStatValue;               ///< DIMMTEMPSTAT
  UINT8                 customRefreshRate;               ///< Allows custom tuning of Refresh rate from 2.0x to 4.0x in units of 0.1x
  UINT8                 HRMFO_Threshold;
  UINT8                 partialmirror;
  UINT8                 partialmirrorsad0;
  UINT16                partialmirrorsize[MAX_PARTIAL_MIRROR]; ///<size of each partial mirror to be created in order
  UINT8                 partialMirrorUEFI;
  UINT64                partialmirrorpercent;
  UINT8                 partialmirrorsts;
  UINT8                 dllResetTestLoops;               ///< Number of times to loop through RMT to test the DLL Reset
  UINT32                memFlows;                        ///<Enable(1)/Disable(0) memory training steps in MRC flow
  UINT32                memFlowsExt;                     ///<Enable(1)/Disable(0) memory training steps in MRC flow
  UINT8                 writePreamble;
  UINT8                 readPreamble;
  UINT8                 RASmode;
  UINT8                 socketInter;                     ///< Number of ways to interleave sockets/socketes (1,2, or 4)
  UINT8                 rankInter;
  UINT8                 DramRaplExtendedRange;           ///< Enable extended range for DRAM RAPL
  UINT8                 electricalThrottling;            ///<ET Auto/Enabled/Disabled
  UINT16                spareErrTh;
  UINT8                 leakyBktLo;                      ///< Leaky bucket low mask position
  UINT8                 leakyBktHi;                      ///< Leaky bucket high mask position
  UINT8                 spareRanks;
  UINT8                 lockstepEnableX4;
  UINT16                numSparTrans;                    ///< Number of sparing transactions interval
#ifdef MEM_NVDIMM_EN
  UINT8                 interNVDIMMS;
  UINT8                 restoreNVDIMMS;                  ///< BIOS will perform NVDIMM Restore operation
  UINT8                 eraseArmNVDIMMS;                 ///< BIOS will perform NVDIMM erase & ARM operations
#endif  //MEM_NVDIMM_EN
  UINT8                 dramMaint;                       ///< Enables automatic DRAM maintenance settings
  UINT8                 dramMaintTRRMode;
  UINT8                 dramMaintMode;
  UINT8                 DramMaintenanceTest;
  UINT8                 DMTDir;
  UINT8                 DMTInv;
  UINT32                DMTRep;
  UINT32                DMTIter;
  UINT8                 DMTSwizzleEn;
  UINT8                 DMRefreshEn;
  UINT8                 ceccWaChMask;                    ///< Bit mask of CH on which CECC WA is applied (0 - WA not applied, 1 - WA applied).
  UINT8                 MemDownConfig;          ///< if set to 1, then the platform has Memorydown configuration
#ifdef PPR_SUPPORT
  UINT8                 pprType;                ///< PPR type - Hard / Soft / Disabed
  UINT8                 pprErrInjTest;          ///< Enable/disable support for Error injection test using Cscripts
  PPR_ADDR_MRC_SETUP    pprAddrSetup[MAX_PPR_ADDR_ENTRIES];
#endif // PPR_SUPPORT
  UINT8                 imcInter;                                                                                                  \
  UINT8                 oneRankTimingModeEn;  ///< Enable/Disable support for JEDEC RCD v2.0+ One Rank Timing Mode
  struct dfxMemSetup    dfxMemSetup;                                                                                               \
  MEMSETUP_CHIP            ///< Chip hook to enable memSetup fields
}; //struct memSetup

struct memNvram {
  UINT8           DataGood;                   ///< Set to one if valid data is present in this structure
  UINT8           RASmode;                    ///< RAS mode (lockstep, mirror, sparing)
  UINT8           RASmodeEx;                  ///< Extended RAS mode (patrol scrub)
  UINT8           ratioIndex;                 ///< Index into the DDR3 ratio table
  UINT8           eccEn;                      ///< Set if ECC will be enabled
  UINT8           dimmTypePresent;            ///< Type of DIMMs populated (RDIMM,UDIMM,SODIMM)
  UINT8           aepDimmPresent;             ///< Is an Apache Pass DIMM present in the system?
  UINT16          dramType;                   ///< DDR3 or DDR4 (from keybyte in SPD)
  UINT32          scrambleSeed;               ///< Data scrambling seed
  UINT16          lastBootDate;               ///< Last Full MRC Training Date
  UINT32          socketBitMap;               ///< CPU present mask
  UINT8           ExtendedADDDCEn;
  struct memSetup savedSetupData;
  struct socketNvram socket[MAX_SOCKET];
  UINT8           XMPProfilesSup;
  UINT8           XMPProfilesRevision;
  struct          memTiming  currentMemTime;
  struct          memTiming  profileMemTime[2];
  UINT8           Force1LMOnly;               ///<Variable used to force the system to 1LM mode when license check fails.
  INT16           crc16;
  UINT8           WrCRC;
  UINT8           txVrefDone;
  UINT8           hostFlybyDone;
  UINT8           threeDsModeDisabled;        ///<Variable to track if 3DS mode is enabled/disabled for CPGC
  INT16           normalizationFactor[MAX_SOCKET][MAX_CH];
  MEMNVRAM_CHIP   ///< Chip hook to enable memNvram fields
};

//#pragma pack(pop)

struct  sysNvramRuntime {
  UINT32  scrubAddrLo[MAX_SOCKET];  ///< Scrub AddressLo
  UINT32  scrubAddrHi[MAX_SOCKET];  ///< Scrub AddressHI
  UINT8   chFailed[MAX_SOCKET][MAX_CH];
  UINT8   devTagInfo[MAX_SOCKET][MAX_CH][MAX_DIMM][MAX_RANK_DIMM];
  UINT8   spareInUse[MAX_SOCKET][MAX_CH];
  UINT8   oldSparePhysicalRank[MAX_SOCKET][MAX_CH];
};

typedef struct _MEMORY_CFG_DATA_RUNTIME {
  struct sysNvramRuntime nvram;
  UINT64_STRUCT  TempByte;     ///< To adjust Padding Bits added for gEfiMemoryConfigDataGuid
} MEMORY_CFG_DATA_RUNTIME;

///
/// Error result struct used in read/write dq/dqs algo.
///
typedef struct {
  UINT8 DimmErrorResult[MAX_STROBE][(MAX_PHASE_IN_FINE_ADJUSTMENT * 2)];  ///< need to be good for read and write.
} TErrorResult, *PTErrorResult;
///
/// Error result struct used in read/write dq/dqs algo.
///
typedef struct {
  UINT8 BitErrorResult[MAX_BITS][(MAX_PHASE_IN_FINE_ADJUSTMENT * 2)];  ///< needs to be good for read and write.
} BErrorResult;
///
/// Per bit PI structure
///
typedef struct {
  UINT16 PiPosition[MAX_BITS];  ///< need to be good for read and write.
} PerBitPi;


///
/// Rank info
///
struct rankDevice {
  UINT16  MR0;  ///< MR0 value for this rank
  UINT16  MR1;  ///< MR1 value for this rank
  UINT16  MR2;  ///< MR2 value for this rank
  UINT16  MR3;  ///< MR3 value for this rank
  UINT16  MR4;  ///< MR4 value for this rank
  UINT16  MR5;  ///< MR5 value for this rank
  UINT16  MR6[MAX_STROBE];  ///< MR6 value for this rank
#ifdef LRDIMM_SUPPORT
  UINT8   CurrentLrdimmTrainingMode;
  UINT8   CurrentDramMode;
  UINT16  CurrentMpr0Pattern;
  UINT8   lrbufRxVrefCache[MAX_STROBE];
  UINT32  lrbufTxVrefCache[MAX_STROBE];
  UINT8   cachedLrBuf_FxBC2x3x[MAX_STROBE];
  UINT8   cachedLrBuf_FxBC4x5x[MAX_STROBE];
  UINT8   cachedLrBuf_FxBC8x9x[MAX_STROBE];
  UINT8   cachedLrBuf_FxBCAxBx[MAX_STROBE];
  UINT8   cachedLrBuf_FxBCCxEx[MAX_STROBE/2];
  UINT8   cachedLrBuf_FxBCDxFx[MAX_STROBE/2];
#endif
  RANKDEVICE_CHIP        ///< Chip hook to enable rankDevice fields
}; //struct rankDevice

///
/// DIMM info
/// List ordered by proximity to host (far to near)
///
struct dimmDevice {
  INT32               minTCK;     ///< minimum tCK for this DIMM (SPD_MIN_TCK)
#ifdef SERIAL_DBG_MSG
  UINT16              tCL;
  UINT16              tRCD;
  UINT16              tRP;
#endif  // SERIAL_DBG_MSG
#ifdef NVMEM_FEATURE_EN
  UINT16              NVmemSize;
#endif
  UINT16              memSize;    ///< Memory size for this DIMM (64MB granularity)
  struct smbDevice    spd;        ///< SPD device
  struct rankDevice   rankStruct[MAX_RANK_DIMM];
#ifdef LRDIMM_SUPPORT
  UINT8   oneRankTimingModeLrbuf_FxBC2x3x[MAX_STROBE];
  UINT8   oneRankTimingModeLrbuf_FxBC4x5x[MAX_STROBE];
  UINT8   oneRankTimingModeLrbuf_FxBC8x9x[MAX_STROBE];
  UINT8   oneRankTimingModeLrbuf_FxBCAxBx[MAX_STROBE];
  UINT8   oneRankTimingModeLrbuf_FxBCCxEx[MAX_STROBE/2];
  UINT8   oneRankTimingModeLrbuf_FxBCDxFx[MAX_STROBE/2];
  UINT8   originalRank0Lrbuf_FxBC2x3x[MAX_RANK_DIMM][MAX_STROBE];
  UINT8   originalRank0Lrbuf_FxBC4x5x[MAX_RANK_DIMM][MAX_STROBE];
  UINT8   originalRank0Lrbuf_FxBC8x9x[MAX_RANK_DIMM][MAX_STROBE];
  UINT8   originalRank0Lrbuf_FxBCAxBx[MAX_RANK_DIMM][MAX_STROBE];
  UINT8   originalRank0Lrbuf_FxBCCxEx[MAX_RANK_DIMM][MAX_STROBE/2];
  UINT8   originalRank0Lrbuf_FxBCDxFx[MAX_RANK_DIMM][MAX_STROBE/2];
#endif
  DIMMDEVICE_CHIP       ///< Chip hook to enable dimmDevice fields
}; //struct dimmDevice


typedef struct {
  UINT8 WdbLine[MRC_WDB_LINE_SIZE];
} TWdbLine;

typedef struct {
  UINT32  address;
  UINT8   cid;
  UINT8   bank;
  UINT8   pdatag;
} PatCadbProg0;

typedef struct {
  UINT16  cs;
  UINT8   control;
  UINT8   odt;
  UINT8   cke;
  UINT8   par;
} PatCadbProg1;

///
/// TT channel info
///
struct ddrChannel {
  UINT8             mcId;               ///<Memory controller number
  UINT8             numDimmSlots;       ///<Number of DIMM slots per channel
  UINT16            memSize;            ///<Memory size for this channel (64MB granularity)
  UINT16            ddr4RemSize;        ///<Remaining DDR4 Memory size for this channel
  UINT16            volSize;            ///<Volatile size of the NVM dimms in this channel
  UINT16            NonVolSize;         ///<Non Volatile size of the NVM dimms in this channel
  UINT16            volRemSize;         ///<Remaining Volatile size of the NVM dimms in this channel
  UINT16            perSize;            ///<Pmem size of the NVM dimms in this channel
  UINT16            perRemSize;         ///<Remaining Pmem size of the NVM dimms in this channel
  UINT16            blkSize;            ///<Block size of the NVM dimms in this channel
  UINT16            blkWinSize;         ///<Block window size of the NVM dimms in this channel
  UINT16            blkWinRemSize;      ///<Remaining block window size of the NVM dimms in this channel
  UINT16            ctrlSize;           ///<Ctrl region size of the NVM dimms in this channel
  UINT16            ctrlRemSize;        ///<Remaining Ctrl region size of the NVM dimms in this channel
#if defined (MEM_NVDIMM_EN) || defined (NVMEM_FEATURE_EN)
  UINT16            NVmemSize;          ///< NVDIMM channel memory size
  UINT16            NVmemRemSize;
#endif  //MEM_NVDIMM_EN
  UINT16            remSize;            ///< Size not yet mapped in units of 64 MB
  TWdbLine          WdbLines[MRC_WDB_LINES];
  PatCadbProg0      cadb0[CADB_LINES];
  PatCadbProg1      cadb1[CADB_LINES];
  struct dimmDevice dimmList[MAX_DIMM];
  UINT16            rirLimit[MAX_RIR];  ///< Rank Interleave Register Limits
  UINT8             rirWay[MAX_RIR];    ///< Rank Interleave Register Ways
  UINT8             rirValid[MAX_RIR];  ///< RIR valid bit
  UINT32            rirOffset[MAX_RIR][MAX_RIR_WAYS];
  UINT8             rirIntList[MAX_RIR][MAX_RIR_WAYS];
  UINT16            rirDDRTLimit[MAX_RIR_DDRT];
  UINT8             rirDDRTWay[MAX_RIR_DDRT];
  UINT8             rirDDRTValid[MAX_RIR_DDRT];
  UINT32            rirDDRTOffset[MAX_RIR_DDRT][MAX_RIR_WAYS];
  UINT8             rirDDRTIntList[MAX_RIR_DDRT][MAX_RIR_WAYS];
  SIGNAL_EYE        ctlTrainingResults[NUM_CTL_PLATFORM_GROUPS];
#ifdef LRDIMM_SUPPORT
  UINT8             chOneRankTimingModeEnable;
#endif
  DDRCHANNEL_CHIP        ///< Chip hook to enable ddrChannel fields
}; //struct ddrChannel


///
/// List ordered by rank size
///
struct rankSort {
  UINT8   socketNum;///< Node this rank is on
  UINT8   chNum;    ///< Channel this rank is on
  UINT8   dimmNum;  ///< DIMM this rank is on
  UINT8   rankNum;  ///< Index into rank list
  UINT8   rankID;   ///< Rank ID for this rank
  UINT16  rankSize; ///< Units of 64 MB
};

///
/// List ordered by socket size
///
struct socketSort {
  UINT8   socketNum;    ///< Socket number
  UINT8   haNum;        ///< HA number
  UINT8   haNodeId;     ///< HA socket id
  UINT16  socketSize;   ///< Units of 64 MB
};

///
/// List ordered by channel size
///
struct channelSort {
  UINT8   chNum;          ///< Index into channel list
  UINT8   socketNum;      ///< Socket number
  UINT16  chSize;         ///< Units of 64 MB
};

///
/// socket info
///
struct Socket {
  UINT8             imcEnabled[MAX_IMC];
  UINT8             csrUnlock;
  UINT8             fatalError;
  UINT8             majorCode;
  UINT8             minorCode;
#if SMCPKG_SUPPORT			//SMCPKG_SUPPORT++
  UINT32            logData;
#endif
  UINT8             maxRankDimm;  ///< Maximum number or ranks supported per DIMM
  UINT32            memSize;      ///< DDR4 memory size for this socket (64MB granularity)
  UINT32            remSize;      ///< Remaining memory size for this socket (64MB granularity)
//AP DEBUG can this be moved to hostchip KNL specific reflect where this is use
  UINT32            baseAddrLo[MAX_IMC];   ///< Memory Start address bits 31:0  for this imc
  UINT32            baseAddrHi[MAX_IMC];   ///< Memory Start address bits 47:32 for this imc
  UINT32            maxAddrLo[MAX_IMC];    ///< Memory Max address bits 31:0  for this imc
  UINT32            maxAddrHi[MAX_IMC];    ///< Memory Max address bits 47:32 for this imc
  UINT8             rdVrefLo;
  UINT8             rdVrefHi;
  UINT8             wrVrefLo;
  UINT8             wrVrefHi;
  struct ddrChannel channelList[MAX_CH];
  UINT32            WdbPatternUid;
  UINT32            WdbBaseAddress;           ///< This field define the WDB base address as define in cache configuration. For write to WDB the cache mode should be WC.
  UINT32            CompCtl0;                 ///< CompCtl0 CR
  UINT32            DimmVref;                 ///< DimmVref CR
  UINT32            MiscControl0;             ///< MiscCOntrol0 CR
  UINT8             DQPat;                    ///< Global Variables storing the current DQPat REUT Test
  UINT8             DQPatLC;                  ///< Global Variables storing the current DQPat Loopcount
  BOOLEAN           EnLA;                       ///< Enable/Disable Logic Analizer
  BOOLEAN           EnDumRd;                    ///< Enable/Disable Logic Analizer
  UINT8             socketSubBootMode;
  UINT8             hostRefreshStatus;
  UINT64_STRUCT     procPpin;
  UINT8             options;      ///< SPARE_EN
  UINT32            RequiredMemoryBase;
#ifdef MRS_STACKING
  UINT8                   cadbMRSMode;          ///< Mode of operation (LOAD / EXECUTE / NORMAL)
  UINT8                   cadbMRSIndex[MAX_CH];
#endif
#ifdef NVMEM_FEATURE_EN
  UINT32            NVmemSize;      ///< Memory size for this node
  UINT32            TotalInterleavedMemSize;      ///< DDR4 memory size for this socket (64MB granularity)
  UINT32            TotalInterleavedNVMemSize; /// < Actual NVMEM interleaved.
  UINT32            NvMemSadIdxBitMap; /// < Bit map of the sadIdx number to indicate if the sad belongs to NVMEM range.
  #endif // NVMEM_FEATURE_EN
  UINT8             smbCntlDti[2]; ///< Cached value for the Device Type bit field in the SMBCNTL register
  UINT8             cmdVrefLo;                                                                                           \
  UINT8             cmdVrefHi;                                                                                           \
  UINT8             testTimeoutSize;
  SOCKET_CHIP       ///< Chip hook to enable Socket fields
}; //struct Socket

//
// SMBCNTL Register smb_dti bit definitions
//
#define DTI_TSOD            0x03  // '0011' specifies TSOD
#define DTI_EEPROM          0x0A  // '1010' specifies EEPROM's
#define DTI_WP_EEPROM       0x06  // '0110' specifies a write-protect operation for an EEPROM
#define DTI_LRDIMM          0x0B  // '1011' specifies LRDIMM buffer
#define DTI_DCP_FIVE        0x05  // '0101' is used for DDR3 Vref control on the reference platform
#define DTI_DCP_SEVEN       0x07  // '0111' could be used as an alternate for Vref control
#define DTI_FNV             0x0B  // '1011' specifies NVMCTLR device

//
// DDR4 SPD Page addressing
//
#define SPD_EE_PAGE_SELECT_0      6 // Page 0 - lower 256 EEPROM bytes
#define SPD_EE_PAGE_SELECT_1      7 // Page 1 - upper 256 EEPROM bytes
#define SPD_EE_PAGE_SIZE          256


#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
struct perfData {
  UINT32  pciCount;
  UINT32  testCount;
  UINT32  jedecCount;
  UINT32  fixedDelay;
  UINT32  pollingCount;
  UINT32  vrefCount;
  UINT32  cpgcCount;
  UINT32  durationTime;
};
#endif  //  SERIAL_DBG_MSG
#endif  //  DEBUG_PERFORMANCE_STATS
///
/// Sub-boot state internal to MRC (8-15 are definable).  The 2 main boot types and paths through QPIRC/MRC - NormalBoot and S3Resume.
/// Within NormalBoot and S3Resume, the sub-boot type can be cold, warm, fast warm, fast cold, and ADR resume.  These are populated
/// at the beginning of MRC so they are not applicable for QPIRC.
///
typedef enum SubBootMode
{
  ColdBoot     = 8,   // Normal path through MRC with full mem detection, init, training, etc.
  WarmBoot     = 9,   // Warm boot path through MRC. Some functionality can be skipped for speed.
  WarmBootFast = 10,  // Fast warm boot path uses the NVRAM structure to skip as much MRC
                            // code as possible to try to get through MRC fast. Should be as close
                            //    as possible to the S3 flow.
  ColdBootFast = 11,  // Fast cold boot path uses the NVRAM structure to skip as much MRC
                            //    code as possible on a cold boot.
  AdrResume    = 12,  // ADR flow can skip most of MRC (i.e. take the S3 path) for DIMMs that
                      //    are in self-refresh.  But the DIMMs that are not in self-refresh
                      //    must go through more of MRC.
#ifdef MEM_NVDIMM_EN
  NvDimmResume = 13   // NvDimm flow is similar to Adr Batterybackup, but the DIMMs need
                      //    Rx & Mx registers intialized.
#else
  Reserved13 = 13
#endif  //MEM_NVDIMM_EN
} SubBootMode;

//
// XMP Identification String
//
#define XMP_ID  0x0C4A

#ifdef SERIAL_DBG_MSG
struct Interleaves {
  UINT8   ways;
  UINT16  channel[MAX_SOCKET * MAX_CH];
};
#endif // SERIAL_DBG_MSG
///
/// define the Training_Result_UP/DOWN CRs struct.
///
struct TrainingResults {
  UINT32  results[4];
};

#ifdef LRDIMM_SUPPORT
struct lrMrecTrainingResults {
  UINT8  results;
};
#endif // LRDIMM_SUPPORT

///
/// Advanced Memtest Error log structure
///
typedef struct {
  UINT32 cpgcErrDat0S;
  UINT32 cpgcErrDat1S;
  UINT32 cpgcErrDat2S;
  UINT32 cpgcErrDat3S;
  UINT32 cpgcErrEccS;
  UINT32 cpgcErrChunk;
  UINT32 cpgcErrColumn;
  UINT32 cpgcErrRow;
  UINT32 cpgcErrBank;
  UINT32 cpgcErrRank;
  UINT32 cpgcSubrank;
} CPGC_ERROR_STATUS_MATS;

struct advMemtestErrorEntry {
  UINT32  code;
  UINT32  data;
  UINT32  checkpoint;
  UINT32  count;
  CPGC_ERROR_STATUS_MATS  advMemtestErrorInfo;
};

#define MAX_ADV_MT_LOG  16
struct advMemtestErrorLog {
  UINT32                      index;
  struct advMemtestErrorEntry log[MAX_ADV_MT_LOG];
};

#define MEM_CHIP_POLICY_DEF(x)          host->var.mem.memChipPolicy.x
#define MEM_CHIP_POLICY_VALUE(host, x)  host->var.mem.memChipPolicy.x
#define CHIP_FUNC_CALL(host, x)         x


typedef struct {
  UINT8                     maxVrefSettings;                        // MAX_VREF_SETTINGS
  UINT8                     earlyVrefStepSize;                      // EARLY_VREF_STEP_SIZE
  UINT8                     minCmdVref;                             // MIN_CMD_VREF
  UINT8                     maxCmdVref;                             // MAX_CMD_VREF
  UINT8                     minIoLatency;                           // MIN_IO_LATENCY
  UINT16                    cas2DrvenMaxGap;                        // CAS2DRVEN_MAXGAP
  UINT8                     mrcRoundTripIoComp;                     // MRC_ROUND_TRIP_IO_COMPENSATION;
  UINT8                     mrcRoundTripIoCompStart;                // MRC_ROUND_TRIP_IO_COMP_START;
  UINT8                     mrcRoundTripMax;                        // MRC_ROUND_TRIP_MAX_VALUE;
  UINT8                     mrcRoundTripDefault;                    // MRC_ROUND_TRIP_DEFAULT_VALUE
  UINT32                    SrMemoryDataStorageDispatchPipeCsr;     // #define SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR  (BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG)  // UBOX scratchpad CSR13
  UINT32                    SrMemoryDataStorageCommandPipeCsr;      // #define SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR   (BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG)  // UBOX scratchpad CSR14
  UINT32                    SrMemoryDataStorageDataPipeCsr;         // #define SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR      (BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG)  // UBOX scratchpad CSR15
  UINT32                    SrBdatStructPtrCsr;                     // SR_BDAT_STRUCT_PTR_CSR
  UINT32                    BiosStickyScratchPad0;                  // BIOSSCRATCHPAD0_UBOX_MISC_REG
  UINT8                     PerBitMarginDefault;                    // PER_BIT_MARGIN_DEFAULT
  UINT8                     RxOdtDefault;                           // RX_ODT_DEFAULT
  UINT8                     RmtColdFastBootDefault;                 // RMT_COLD_FAST_BOOT_DEFAULT
  UINT8                     FastBootDefault;                        // FAST_BOOT_DEFAULT
} MEM_CHIP_POLICY;

///
/// TT host info
///
struct memVar {
#ifdef SERIAL_DBG_MSG
  UINT8                   varStructInit;    /// To indicate if the VAR struct has been initialized to setup values
#endif
  UINT8                   firstJEDECDone;   ///< Flag to indicate the first JEDEC Init has executed
  UINT8                   memBCLK;          ///< Memory BCLK speed
  UINT8                   currentSocket;    ///< Current socket being initialized
  UINT8                   pipeSyncIndex;    ///<
// APTIOV_SERVER_OVERRIDE_RC_START : Added to skip LastBoot Error Handler for cold boot.
  BOOLEAN                 IsColdBoot;
// APTIOV_SERVER_OVERRIDE_RC_END : Added to skip LastBoot Error Handler for cold boot.
  SubBootMode             subBootMode;      ///< WarmBoot, WarmBootFast, etc.
  UINT8                   wipeMemory;       ///< Write 0 to all memory to clean secrets
  UINT8                   skipMemoryInit;   ///< Skip memory init based on certain conditions.
  UINT8                   ddrFreqLimit;     ///< Set to limit frequency by the user
  UINT8                   ddrVccu;          ///< Set to specify DDR VCCU
  UINT8                   chInter;          ///< Number of ways to interleave channels (1,2,3, or 4)
  UINT8                   timeBase;
  UINT8                   callingTrngOffstCfgOnce; ///<to prevent looping inside RMT
  UINT16                  freqTable[MAX_SUP_FREQ];
  UINT8                   RASModesEx;       ///< Bit map of additional supported RAS modes BIT2 = patrol scrub (PTRLSCRB_EN)
  UINT8                   RASCapable;       ///< 1 for RAS capable imc(s).
  UINT8                   clkTrainingDone;
  UINT8                   numChPerMC;       ///< Number of Channels per HA
  UINT8                   earlyCmdClkExecuted;
  UINT8                   checkMappedOutRanks;
#ifdef SERIAL_DBG_MSG
  UINT8                   serialDebugMsgLvl;
  UINT8                   earlyCtlClkSerialDebugFlag;
  UINT64_STRUCT           startTsc;
#endif // SERIAL_DBG_MSG
  UINT8                   SADNum;
  UINT32                  memSize;          ///< Total physical memory size
#ifdef NVMEM_FEATURE_EN
  UINT32                  NVmemSize;          ///< Total physical memory size
  UINT32                  TotalInterleavedMemSize;      ///< DDR4 memory size for this socket (64MB granularity)
  UINT32                  TotalInterleavedNVMemSize; /// < Actual NVMEM interleaved.
#endif // NVMEM_FEATURE_EN
  UINT32                  QCLKPeriod;       ///< QCLK Period in pico seconds
  UINT32                  options;          ///< PTRLSCRB_EN, DMNDSCRB_EN, etc
  UINT32                  lowMemBase;       ///< Mem base in 64MB units for below 4GB mem.
  UINT32                  lowMemSize;       ///< Mem size in 64MB units for below 4GB mem.
  UINT32                  highMemBase;      ///< Mem base in 64MB units for above 4GB mem.
  UINT32                  highMemSize;      ///< Mem size in 64MB units for above 4GB mem.
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  UINT32                  meStolenSize;     ///< Size allocated for ME UMA in 1MB units
#endif //ME_SUPPORT_FLAG
  UINT32                  hwLFSRSeeds[12][MAX_SEEDS];
  UINT32                  initialLFSRSeed;
  UINT32                  piSettingStopFlag[MAX_CH];
  UINT32                  faultyPartsFlag[MAX_CH];
  UINT8                   iotDisabled[MAX_SOCKET];
  UINT64_STRUCT           baseIOTPhysicalAddress[SAD_RULES_ADDR_RANGE];  ///<Four base address limits of the IOT SAD address entries
  UINT32                  odtValueTablePtr;  ///< struct odtValueStruct *
  UINT32                  odtActTablePtr;    ///< struct odtActStruct *
  UINT8                   imodeEnabled;
  UINT8                   txEqExecuted;
  UINT8                   imodeExecuted;
  UINT8                   pxcEnabled;
  UINT8                   MemMapDegradeLevel;       ///<0 - MEM_MAP_LVL_NORMAL   1 : MEM_MAP_LVL_IGNORE_CFGIN     2 :MEM_MAP_LVL_IGNORE_NEW_DIMMS
  UINT8                   MemMapState;              ///<0 - MEM_MAP_STATE_RESOURCE_CALCULATION    1-MEM_MAP_STATE_RESOURCE_CALCULATION_SUCCESS    2-MEM_MAP_STATE_RESOURCE_FAILURE
  UINT8                   powerGoodReset;
  struct Socket           socket[MAX_SOCKET];                        ///< Per socket structure
#ifdef PPR_SUPPORT
  UINT8                   pprStatus[MAX_PPR_ADDR_ENTRIES];
  UINT8                   softPprDone[MAX_SOCKET];
#endif // PPR_SUPPORT
#ifdef LRDIMM_SUPPORT
  struct TrainingResults  lrTrainRes[MAX_CH][MAX_STROBE];
  struct lrMrecTrainingResults lrMrecTrainRes[MAX_CH][MAX_STROBE];
  struct TrainingResults  lrMrdTrainRes[MAX_CH][MAX_STROBE];
  struct TrainingResults  lrDwlTrainRes[MAX_CH][MAX_STROBE];
  struct lrMrecTrainingResults lrCwlTrainRes[MAX_CH][MAX_STROBE];
  struct TrainingResults  lrMwdTrainRes[MAX_CH][MAX_STROBE];
  UINT8                   maxSubRank;
  UINT8                   currentSubRank;
  UINT8                   InPbaWaMode;
  UINT8                   InOvrRttPrkMode;
  UINT8                   txVrefBacksideDone;
#endif // LRDIMM_SUPPORT
  struct TrainingResults  trainRes[MAX_CH][MAX_STROBE];
#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  UINT32                  currentMemPhase;
  UINT8                   noZoneIndex;
  UINT8                   noZoneActive;
  struct perfData         all;
  struct perfData         dimmDetect;
  struct perfData         chkDimmRanks;
  struct perfData         gatherSPD;
  struct perfData         earlyConfig;
  struct perfData         ddrioInit;
  struct perfData         jedecInit;
  struct perfData         cmdClk;
  struct perfData         cmdClkEarly;
  struct perfData         ctlClkEarly;
  struct perfData         xoverCalib;
  struct perfData         recEn;
  struct perfData         rdDqDqs;
  struct perfData         wrLvl;
  struct perfData         wrDqDqs;
  struct perfData         EarlyRdVref;
  struct perfData         EarlyWrVref;
  struct perfData         wrFlyBy;
  struct perfData         clockInit;
  struct perfData         EarlyRid;
  struct perfData         rxDqAdv;
  struct perfData         txDqAdv;
  struct perfData         rankMarginTool;
  struct perfData         memMapping;
  struct perfData         rasConfig;
  struct perfData         eyeDiagram;
  struct perfData         perBitDeskewRx;
  struct perfData         perBitDeskewTx;
  struct perfData         normalMode;
  struct perfData         rdVref;
  struct perfData         wrVref;
  struct perfData         cmdVref;
  struct perfData         swizzleDiscovery;
#ifdef LRDIMM_SUPPORT
  struct perfData         wrVrefLrdimm;
  struct perfData         rdVrefLrdimm;
  struct perfData         rxLrdimmDqCentering;
  struct perfData         txLrdimmDqCentering;
#endif
  struct perfData         rtOpt;
  struct perfData         txEqTrn;
  struct perfData         trainTxEq;
  struct perfData         trainImode;
  struct perfData         trainRXCTLE;
  struct perfData         trainMcRon;
  struct perfData         trainRttWr;
  struct perfData         trainRxOdt;
  struct perfData         trainDramRon;
#ifdef LRDIMM_SUPPORT
  struct perfData         LrdimmRx;
  struct perfData         LrdimmTx;
#endif
  struct perfData         lateConfig;
  struct perfData         memTest;
  struct perfData         memInit;
#ifdef PPR_SUPPORT
  struct perfData         ppr;
#endif // PPR_SUPPORT
  struct perfData         noZone[MAX_NOZONE];
#endif  //  SERIAL_DBG_MSG
#endif  //  DEBUG_PERFORMANCE_STATS

  UINT8                   firstPass;
  UINT8                   LrTrainingDone[MAX_SOCKET];
  UINT8                   previousBootError;
  UINT8                   dateCheckedOnFirstSocket;  ///<Flag to check that the MRC Training Date gets checked on only the first socket
  UINT8                   OdtWrCycleTrailing;
  UINT8                   xoverModeVar;     ///< xover mode (1 = xover2:2, 0 = native 1:1)
#ifdef BDAT_SUPPORT
#ifdef SSA_FLAG
  UINT8                   SsaHeapFlag;
  UINT32                  bssaBdatSize;                                 ///<total size for all the saveToBdat() calls (made for each different HOB)
  UINT8                   bssaNumberHobs[MAX_NUMBER_SSA_BIOS_RESULTS];  ///<same GUID multiple HOBs (<60kB)
  EFI_GUID                bssaBdatGuid[MAX_NUMBER_SSA_BIOS_RESULTS];    ///<track all GUIDs used with saveToBdat()
  UINT8                   bssaNumberDistinctGuids;                      ///<size of array
#endif  //SSA_FLAG
#endif //BDAT_SUPPORT

  UINT8                   doHAInterleave;
  UINT8                   OpRASmode;        ///< Operational RAS mode
  UINT8                   socketInter;      ///< Number of ways to interleave sockets/socketes (1,2,4, or 8)
  UINT8                   rankInter;        ///< 1, 2, 4, or 8 way interleave
  UINT8                   RASModes;         ///< Bit map of supported RAS modes BIT0 = Lockstep and Mirror support, BIT1 = Spare support
  UINT16                  spareErrTh;       ///< Number of correctable errors before triggerint spare copy
  UINT8                   VLSPrevBoot;
  UINT32                  lowMEMemBase;     ///< ME Memory base.
  UINT32                  mccpubusno;
  UINT8                   SADintList[SAD_RULES][MC_MAX_NODE];  ///< SAD interleave list
  struct SADTable         SAD[SAD_RULES];                       ///< SAD table
  UINT8                   DMTestPass[MAX_SOCKET];
  UINT32                  rtDefaultValue;
#ifdef MARGIN_CHECK
  UINT8                   runningRmt;   ///< Flag to check that the RankMarginTool is currently being running
#endif
  UINT32                  lastCheckpoint[MAX_SOCKET];
#ifdef  ADV_MEMTEST_SUPPORT
  struct advMemtestErrorLog advMemtestErrors;
#endif
  UINT8                   nvramDecompressionFailed;
  UINT8                   notRunningFromCT; ///< PostPackageRepairMain called from the CallTable (CT)
  UINT32                  chBitMask;
  MEM_CHIP_POLICY         memChipPolicy;
  MEMVAR_CHIP            ///< Chip hook to enable memVar fields
};  // struct memVar

#pragma pack(pop)

#endif // _memhost_h
