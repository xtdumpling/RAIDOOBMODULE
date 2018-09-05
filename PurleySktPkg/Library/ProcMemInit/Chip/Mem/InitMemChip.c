/**
 This file contains an 'Intel Pre-EFI Module' and is licensed
 for Intel CPUs and Chipsets under the terms of your license
 agreement with Intel or your vendor.  This file may be
 modified by the user, subject to additional terms of the
 license agreement
**/

/*************************************************************************
 *
 * Memory Reference Code
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
 *      IMC and DDR modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysFuncChip.h"
#include "MemFuncChip.h"
#include "SysHostChip.h"
#include "Pipe.h"
#include "MemPOR.h"
#include "FnvAccess.h"
#include "Mmrc.h"
#include "CpgcChip.h"
#include "KtiSetupDefinitions.h"
#include "CpuCsrAccessDefine.h"
#include "MemAddrMap.h"

// APTIOV_SERVER_OVERRIDE_RC_START :  Wrapper for OEM to override TurnAroundTimings
#include <AmiMrcHooksWrapperLib\AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END :  Wrapper for OEM to override TurnAroundTimings

//
// DCLK ratios
//
const UINT8   DCLKs_100[MAX_SUP_FREQ] = {8, 10, 10, 12, 12, 14, 16, 18, 18, 20, 20, 22, 24, 26, 26, 28, 28, 30, 32, 34, 35, 36, 37, 38, 40, 42, 43, 44};
                                 //1333  1600  1866  2133  2400  2666  2933  3200
static const UINT8   DMFC[] =             { 7,    6,    5,    4,    3,    2,    1,    0};
static const UINT8   PcodeMaxRatio133[] = {10,   12,   14,   16,   18,   20,   22,   24};
                                 //1000  1200  1400  1600  1800  2000  2200  3200
static const UINT8   PcodeMaxRatio100[] = {10,   12,   14,   16,   18,   20,   22,   32};
extern const UINT8   DCLKs[MAX_SUP_FREQ];
extern const UINT8   BCLKs[MAX_SUP_FREQ];
//sref_idle_timer, opp_sref_en, mdll_off_en, ck_mode, ddrt_idle_cmd
static const UINT32 SREF_LL0_AUTO[5] = {0xFFFFF, 0, 1, 2, 1};
static const UINT32 SREF_LL1_AUTO[5] = {0x0E800, 1, 1, 2, 1};
static const UINT32 SREF_LL2_AUTO[5] = {0x0C800, 1, 1, 2, 1};
static const UINT32 SREF_LL3_AUTO[5] = {0xF, 0, 1, 2, 1};
static const UINT32 SREF_UCR_AUTO[5] = {0xF, 0, 1, 2, 1};

//                           {DDRT_CKE, PPD, APD, CKE idle timer}
static const UINT8 CKE_LL0_AUTO[4] = {0x80, 0, 0, 0};
static const UINT8 CKE_LL1_AUTO[4] = {0x60, 0, 1, 1};
static const UINT8 CKE_LL2_AUTO[4] = {0x30, 0, 1, 1};
static const UINT8 CKE_LL3_AUTO[4] = {0x30, 0, 1, 1};

extern UINT8  lrdimmExtraOdtDelay[MAX_SUP_FREQ];
extern const UINT8 tPARRECOVERY[MAX_SUP_FREQ];
//
// Local Prototypes
//
UINT32 InitMem(PSYSHOST host);
UINT32 InitADR(PSYSHOST host);
UINT32 EarlyConfig(PSYSHOST host);
UINT32 LateConfig(PSYSHOST host);
UINT32 SetupSVLandScrambling(PSYSHOST);
void   ExitSR(PSYSHOST host, UINT8 socket);
UINT32 PreTrainingInit(PSYSHOST host);
UINT32 PostTrainingInit(PSYSHOST host);
void   CheckLogicDelayTimingMode(PSYSHOST host, UINT8 socket);
UINT32 MemLate(PSYSHOST host);
void   CheckSecrets(PSYSHOST host);
UINT32 SwitchToNormalMode(PSYSHOST host);
void   PowerManagementSetup(PSYSHOST host, UINT8 socket);
// NVMDIMM stuff (under construction)
VOID   EarlyDdrtConfig(PSYSHOST host, UINT8 socket);
void   DisableDDRTEridParityErrorLogging(PSYSHOST host, UINT8 socket);
void   ClearIMCErrors(PSYSHOST host);
UINT32 DDRResetLoop(PSYSHOST host);
// DRAM Maintenance
void   DramMaintenance(PSYSHOST host, UINT8 socket);
void   ConfigureTRRMode(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 mode);
UINT32 RunMmrc (PSYSHOST host);
void   SetupCapWrCrcErrorFlow(PSYSHOST host, UINT8 socket);
void   PmSelfRefreshProgramming(PSYSHOST host, UINT8 socket, UINT8 ch);
void   PmCkeProgramming(PSYSHOST host, UINT8 socket, UINT8 ch);
void   ProgramErid2DataValid(PSYSHOST host, UINT8 socket);
void   DdrtCapErrorWA(PSYSHOST host, UINT8 socket, UINT8 ch);
void   ddrtS3Resume(PSYSHOST host, UINT8 socket);


#ifdef _MSC_VER
#pragma warning(disable : 4740)
#endif

#define NO_POST_CODE  0xFF
#define DEFAULT_DDRT_READ_CREDIT 0x28
// Define RUN_MMRC_TASK to take MMRC path...
#define RUN_MMRC_TASK

static CallTableEntryType CallTableMaster[] = {
  // The functions are executed in the following order, as the policy flag dictates.
  // Mrctask, post_code, policyFlag, iteration, debug_string
  {DispatchSlaves,      NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Dispatch Slaves")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Get Boot Mode")},
  {DetermineBootMode,   NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Select Boot Mode")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Send Boot Mode")},
  {InitStructuresLate,  NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Init Structures Late") },
#if ENHANCED_WARNING_LOG == 0
  {PlatformMemInitWarning, NO_POST_CODE,      0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Promote Warning Exception List")},
#endif
  {InitThrottlingEarly, NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Initialize Throttling Early")},
  {DetectDIMMConfig,    STS_DIMM_DETECT,      0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Detect DIMM Configuration")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Get Slave Data")},
  {CheckPORCompat,      NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Check POR Compatibility")},
  {InitDdrClocks,       STS_CLOCK_INIT,       0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Initialize DDR Clocks")},
  {CheckStatus,         NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_SERIAL,    CALL_TABLE_STRING("Check Status")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Send Status")},
  {ConfigureXMP,        NO_POST_CODE,         0,          MRC_PF_COLD,  MRC_MP_BOTH,      CALL_TABLE_STRING("Configure XMP")},
  {CheckVdd,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Set Vdd")},
  {CheckDimmRanks,      STS_RANK_DETECT,      0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Check DIMM Ranks")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Send Data")},
  {InitMem,             NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Initialize Memory")},
  // The functions are executed in the following order, as the policy flag dictates.
  // Mrctask, post_code, OEM command, policyFlag, iteration, debug_string
  {GatherSPDData,       STS_SPD_DATA,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Gather SPD Data")},
#ifdef MEM_NVDIMM_EN
  {CoreNVDIMMStatus,    NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Core NVDIMM Status")},
#endif //MEM_NVDIMM_EN
  {EarlyConfig,         STS_GLOBAL_EARLY,     0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early Configuration")},
#ifdef RUN_MMRC_TASK
  {RunMmrc,             STS_DDRIO_INIT,       EARLY_INIT, MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DDRIO Initialization (MMRC)")},
#else
  {InitDdrioInterface,  STS_DDRIO_INIT,       EARLY_INIT, MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DDRIO Initialization")},
#endif
  {InitDdrioInterfaceLate,  STS_DDRIO_INIT,   LATE_INIT,   MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DDRIO Initialization Late")},
  {PreTrainingInit,         NO_POST_CODE, 0,               MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Pre-Training Initialization") },
  {EarlyCtlClk,         STS_CHANNEL_TRAINING, EARLY_CTL_CLK, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early CTL/CLK Training")},
  {EarlyCmdClk,         STS_CHANNEL_TRAINING, EARLY_CMD_CLK, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early CMD/CLK Training")},
#ifdef LRDIMM_SUPPORT
#if defined COSIM_EN || !defined YAM_ENV
  {Ddr4LrdimmBacksideRxPhase, STS_CHANNEL_TRAINING, RX_BACKSIDE_PHASE_TRAINING, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Phase Training(RX)")},
  {Ddr4LrdimmBacksideRxCycle, STS_CHANNEL_TRAINING, RX_BACKSIDE_CYCLE_TRAINING, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Cycle Training(RX)")},
  {Ddr4LrdimmBacksideRxDelay, STS_CHANNEL_TRAINING, RX_BACKSIDE_DELAY_TRAINING, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Delay Training(RX)")},
#endif
#endif
  {ReceiveEnable,       STS_CHANNEL_TRAINING, RECEIVE_ENABLE_BASIC, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Receive Enable")},
//  {ReadDqDqs,           STS_CHANNEL_TRAINING, RX_DQ_DQS_BASIC,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Dq/Dqs Basic")},
  {ReadDqDqsPerBit,     STS_CHANNEL_TRAINING, RX_DQ_DQS_BASIC,  MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Dq/Dqs Basic Per Bit")},
#if defined COSIM_EN || !defined YAM_ENV
  {DqSwizzleDiscovery,  STS_CHANNEL_TRAINING, DQ_SWIZZLE_DISCOVERY, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Dq Swizzle Discovery")},
#endif
#ifdef LRDIMM_SUPPORT
#if defined COSIM_EN || !defined YAM_ENV
  {Ddr4LrdimmBacksideTxFineWL, STS_CHANNEL_TRAINING, TX_BACKSIDE_FINE_WL_TRAINING,  MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Fine WL Training(TX)")},
  {Ddr4LrdimmBacksideTxCoarseWL, STS_CHANNEL_TRAINING, TX_BACKSIDE_COARSE_WL_TRAINING, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Coarse WL Training(TX)")},
  {Ddr4LrdimmBacksideTxDelay, STS_CHANNEL_TRAINING, TX_BACKSIDE_DELAY_TRAINING,  MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Delay Training(TX)")},
#endif
#endif
  {WriteLeveling,       STS_CHANNEL_TRAINING, WRITE_LEVELING_BASIC, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Write Leveling")},
  {WriteLevelingCleanUp,STS_CHANNEL_TRAINING, WR_FLY_BY,  MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Write Fly By")},
  //{WrDqDqs,             STS_CHANNEL_TRAINING, TX_DQ_BASIC,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Dq Basic")},
  {WriteDqDqsPerBit,    STS_CHANNEL_TRAINING, TX_DQ_BASIC, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Dq/Dqs Basic Per Bit")},
  {PostPackageRepairMain, STS_CHANNEL_TRAINING, PPR_FLOW, MRC_PF_ALL,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("PPR Flow")},
  {WrEarlyVrefCentering,STS_CHANNEL_TRAINING, E_TRAIN_WR_VREF,   MRC_PF_COLD, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Wr Early Vref Centering")},
  {RdEarlyVrefCentering,STS_CHANNEL_TRAINING, E_TRAIN_RD_VREF,   MRC_PF_COLD, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Rd Early Vref Centering")},
  {CmdVrefCentering,    STS_CHANNEL_TRAINING, CMD_VREF_CENTERING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("CMD Vref Centering")},
  {LateCmdClk,          STS_CHANNEL_TRAINING, LATE_CMD_CLK,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Late Cmd/Clk Training")},
  {EarlyReadID,         STS_CHANNEL_TRAINING, EARLY_RID_FINE,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early Read ID")},
#ifdef LRDIMM_SUPPORT
  {RxLRDIMMDqCentering,STS_CHANNEL_TRAINING, RX_LRDIMM_DQ_CENTERING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rd LRDIMM DQ Centering")},
  {WrLRDIMMVrefCentering,STS_CHANNEL_TRAINING, TRAIN_WR_VREF_LRDIMM,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Wr LRDIMM backside VREF Training")},
  {RdLRDIMMVrefCentering,STS_CHANNEL_TRAINING, TRAIN_RD_VREF_LRDIMM,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rd LRDIMM backside VREF Training")},
#endif
  {TrainMcOdt,          STS_CHANNEL_TRAINING, T_MC_ODT,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("McOdt")},
  {TrainDramRon,        STS_CHANNEL_TRAINING, T_DRAM_RON, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DramRon")},
  {TrainMcRon,          STS_CHANNEL_TRAINING, T_MC_RON,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("McRon")},
  {TrainTxEq,           STS_CHANNEL_TRAINING, T_TX_EQ,    MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Eq")},
  {TrainImode,          STS_CHANNEL_TRAINING, T_IMODE,    MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Imode")},
  {TrainCTLE,           STS_CHANNEL_TRAINING, T_CTLE,     MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("CTLE")},
  {TxPerBitDeskew,      STS_CHANNEL_TRAINING, PER_BIT_DESKEW_TX,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Per Bit Deskew Training")},
  {RxPerBitDeskew,      STS_CHANNEL_TRAINING, PER_BIT_DESKEW_RX,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Per Bit Deskew Training")},
  {WrVrefCentering,     STS_CHANNEL_TRAINING, TRAIN_WR_VREF,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Wr Vref Centering")},
  {RdVrefCentering,     STS_CHANNEL_TRAINING, TRAIN_RD_VREF,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rd Vref Centering")},
  {WrAdvancedCentering, STS_CHANNEL_TRAINING, TRAIN_WR_DQS,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Dq Adv Training")},
  {RdAdvancedCentering, STS_CHANNEL_TRAINING, TRAIN_RD_DQS,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Dq/Dqs Adv Training")},
  {PXCTraining,         STS_CHANNEL_TRAINING, PXC_TRAINING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("PXC Training")},
  {RoundTripOptimize,   STS_CHANNEL_TRAINING, ROUND_TRIP_OPTIMIZE,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Round Trip Optimization")},
#ifdef   SERIAL_DBG_MSG
  {DisplayTrainResults, NO_POST_CODE,         0,          MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Display Training Results")},
#endif // SERIAL_DBG_MSG
  {PostTrainingInit,    NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Post-Training Initialization")},
#ifdef MARGIN_CHECK
  {RankMarginTool,      STS_CHANNEL_TRAINING, STS_RMT,    MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rank Margin Tool")},
#endif
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Pre SSA Get Data")},
#ifdef  BDAT_SUPPORT
  {FillBDATStructure,   NO_POST_CODE,         0,          MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Fill BDAT Structure")},
#endif  // BDAT_SUPPORT
#ifdef SSA_FLAG
  {SSAInit,             SSA_API_INIT,         SSA_API_INIT_START,   MRC_PF_ALL,   MRC_MP_BOTH,  CALL_TABLE_STRING("BIOS SSA Initialization")},
#endif  // SSA_FLAG
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Post SSA Send Data")},
#ifdef MEM_NVDIMM_EN
  { CoreRestoreNVDIMMs, NO_POST_CODE, 0, MRC_PF_ALL, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Core Restore NVDIMMs") },
  { CoreArmNVDIMMs, NO_POST_CODE, 0,  MRC_PF_ALL, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Core Arm NVDIMMs") },
#endif  //MEM_NVDIMM_EN
  {LateConfig,          STS_CHANNEL_TRAINING, STS_MINOR_END_TRAINING,   MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Late Configuration")},
  {InitThrottling,      STS_INIT_THROTTLING,  0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Initialize Throttling")},
  {AdvancedMemTest,     STS_MEMBIST,          0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Advanced MemTest")},
  {MemTest,             STS_MEMBIST,          0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("MemTest")},
  {SetupSVLandScrambling, NO_POST_CODE,       0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Setup SVL and Scrambling")},
  {MemInit,             STS_MEMINIT,          0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("MemInit")},
  {CheckRASSupportAfterMemInit,  NO_POST_CODE,0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Check Ras Support After MemInit")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Get NVRAM Data")},
  {InitMemoryMap,       STS_DDR_MEMMAP,       0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Initialize Memory Map")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Sync Both threads")},
  {SwitchToNormalMode,  STS_CHANNEL_TRAINING, STS_MINOR_NORMAL_MODE,   MRC_PF_ALL,   MRC_MP_BOTH_LOOP,      CALL_TABLE_STRING("Switch to Normal Mode")},
  {InitADR,             NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Initialize ADR")},
  {SetRASConfig,        STS_RAS_CONFIG,       0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Set RAS Configuration")},
  {MemLate,             NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Memory Late")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Final Error Sync")},
//  {InitSADInterfaceDataStruct, NO_POST_CODE,         0,   MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Memory Late")},
#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  {PrintAllStats,       NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Print All Stats")},
#endif
#endif  // DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  {DIMMInfo,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("DIMM Information")},
#endif // SERIAL_DBG_MSG
  {DDRResetLoop,        NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("DDR Reset Loop")},
};

static UINT8 CallTableMasterSize = sizeof(CallTableMaster) / sizeof(CallTableEntryType);

static CallTableEntryType CallTableSlave[] = {
  // The functions are executed in the following order, as the policy flag dictates.
  // Mrctask,         Major POST Code, Minor POST Code,      OEM command, policyFlag,     MP flags,          Debug String
  {SlaveAck,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Slave Acknoledge")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Send Boot Mode")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Get Boot Mode")},
  {InitStructuresLate,  NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Init Structures Late") },
#if ENHANCED_WARNING_LOG == 0
  {PlatformMemInitWarning, NO_POST_CODE,      0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Promote Warning Exception List")},
#endif
  {InitThrottlingEarly, NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Initialize Throttling Early")},
  {DetectDIMMConfig,    STS_DIMM_DETECT,      0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Detect DIMM Configuration")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Send Slave Data")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Get Status")},
  {WaitForConfigureXMP, NO_POST_CODE,         0,          MRC_PF_COLD,  MRC_MP_PARALLEL,  CALL_TABLE_STRING("Wait for Configure XMP")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Get Data")},
  {InitMem,             NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Initialize Memory")},

  // The functions are executed in the following order, as the policy flag dictates.
  // Mrctask, post_code, OEM command, policyFlag, iteration, debug_string
  {GatherSPDData,       STS_SPD_DATA,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Gather SPD Data")},
#ifdef MEM_NVDIMM_EN
  { CoreNVDIMMStatus, NO_POST_CODE, 0, MRC_PF_ALL, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Core NVDIMM Status") },
#endif //MEM_NVDIMM_EN
  {EarlyConfig,         STS_GLOBAL_EARLY,     0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early Configuration")},
#ifdef RUN_MMRC_TASK
  {RunMmrc,             STS_DDRIO_INIT,       EARLY_INIT, MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DDRIO Initialization (MMRC)")},
#else
  {InitDdrioInterface,  STS_DDRIO_INIT,       EARLY_INIT, MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DDRIO Initialization")},
#endif
  {InitDdrioInterfaceLate,  STS_DDRIO_INIT,   LATE_INIT,  MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DDRIO Initialization Late")},
  {PreTrainingInit,     NO_POST_CODE,         0,          MRC_PF_ALL,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Pre-Training Initialization")},
  {EarlyCtlClk,         STS_CHANNEL_TRAINING, EARLY_CTL_CLK,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early CTL/CLK Training")},
  {EarlyCmdClk,         STS_CHANNEL_TRAINING, EARLY_CMD_CLK,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early CMD/CLK Training")},
#ifdef LRDIMM_SUPPORT
  {Ddr4LrdimmBacksideRxPhase, STS_CHANNEL_TRAINING, RX_BACKSIDE_PHASE_TRAINING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Phase Training(RX)")},
  {Ddr4LrdimmBacksideRxCycle, STS_CHANNEL_TRAINING, RX_BACKSIDE_CYCLE_TRAINING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Cycle Training(RX)")},
  {Ddr4LrdimmBacksideRxDelay, STS_CHANNEL_TRAINING, RX_BACKSIDE_DELAY_TRAINING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Delay Training(RX)")},
#endif
  {ReceiveEnable,       STS_CHANNEL_TRAINING, RECEIVE_ENABLE_BASIC,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Receive Enable")},
  //{ReadDqDqs,           STS_CHANNEL_TRAINING, RX_DQ_DQS_BASIC,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Dq/Dqs Basic")},
  {ReadDqDqsPerBit,     STS_CHANNEL_TRAINING, RX_DQ_DQS_BASIC,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Dq/Dqs Basic Per Bit")},
  {DqSwizzleDiscovery,  STS_CHANNEL_TRAINING, DQ_SWIZZLE_DISCOVERY,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Dq Swizzle Discovery")},
#ifdef LRDIMM_SUPPORT
  {Ddr4LrdimmBacksideTxFineWL, STS_CHANNEL_TRAINING, TX_BACKSIDE_FINE_WL_TRAINING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Fine WL Training(TX)")},
  {Ddr4LrdimmBacksideTxCoarseWL, STS_CHANNEL_TRAINING, TX_BACKSIDE_COARSE_WL_TRAINING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Coarse WL Training(TX)")},
  {Ddr4LrdimmBacksideTxDelay, STS_CHANNEL_TRAINING, TX_BACKSIDE_DELAY_TRAINING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Lrdimm Backside Delay Training(TX)")},
#endif
  {WriteLeveling,       STS_CHANNEL_TRAINING, WRITE_LEVELING_BASIC,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Write Leveling")},
  {WriteLevelingCleanUp, STS_CHANNEL_TRAINING, WR_FLY_BY, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Write Fly By")},
  //{WrDqDqs,             STS_CHANNEL_TRAINING, TX_DQ_BASIC,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Dq Basic")},
  {WriteDqDqsPerBit,    STS_CHANNEL_TRAINING, TX_DQ_BASIC,  MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Dq/Dqs Basic Per Bit")},
  {PostPackageRepairMain, STS_CHANNEL_TRAINING, PPR_FLOW, MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("PPR Flow")},
  {WrEarlyVrefCentering,STS_CHANNEL_TRAINING, E_TRAIN_WR_VREF, MRC_PF_COLD, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Wr Early Vref Centering")},
  {RdEarlyVrefCentering,STS_CHANNEL_TRAINING, E_TRAIN_RD_VREF, MRC_PF_COLD, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Rd Early Vref Centering")},
  {CmdVrefCentering,    STS_CHANNEL_TRAINING, CMD_VREF_CENTERING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("CMD Vref Centering")},
  {LateCmdClk,          STS_CHANNEL_TRAINING, LATE_CMD_CLK,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Late Cmd/Clk Training")},
  {EarlyReadID,         STS_CHANNEL_TRAINING, EARLY_RID_FINE,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Early Read ID")},
#ifdef LRDIMM_SUPPORT
  {RxLRDIMMDqCentering,STS_CHANNEL_TRAINING, RX_LRDIMM_DQ_CENTERING,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rd LRDIMM DQ Centering")},
  {WrLRDIMMVrefCentering,STS_CHANNEL_TRAINING, TRAIN_WR_VREF_LRDIMM,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Wr LRDIMM backside VREF Training")},
  {RdLRDIMMVrefCentering,STS_CHANNEL_TRAINING, TRAIN_RD_VREF_LRDIMM,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rd LRDIMM backside VREF Training")},
#endif
  {TrainMcOdt,          STS_CHANNEL_TRAINING, T_MC_ODT,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("McOdt")},
  {TrainDramRon,        STS_CHANNEL_TRAINING, T_DRAM_RON, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("DramRon")},
  {TrainMcRon,          STS_CHANNEL_TRAINING, T_MC_RON,   MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("McRon")},
  {TrainTxEq,           STS_CHANNEL_TRAINING, T_TX_EQ,    MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Eq")},
  {TrainImode,          STS_CHANNEL_TRAINING, T_IMODE,    MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Imode")},
  {TrainCTLE,           STS_CHANNEL_TRAINING, T_CTLE,     MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("CTLE")},
  {TxPerBitDeskew,      STS_CHANNEL_TRAINING, PER_BIT_DESKEW_TX, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Per Bit Deskew Training")},
  {RxPerBitDeskew,      STS_CHANNEL_TRAINING, PER_BIT_DESKEW_RX, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Per Bit Deskew Training")},
  {WrVrefCentering,     STS_CHANNEL_TRAINING, TRAIN_WR_VREF, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Wr Vref Centering")},
  {RdVrefCentering,     STS_CHANNEL_TRAINING, TRAIN_RD_VREF, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rd Vref Centering")},
  {WrAdvancedCentering, STS_CHANNEL_TRAINING, TRAIN_WR_DQS, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Tx Dq Adv Training")},
  {RdAdvancedCentering, STS_CHANNEL_TRAINING, TRAIN_RD_DQS, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rx Dq/Dqs Adv Training")},
  {PXCTraining,         STS_CHANNEL_TRAINING, PXC_TRAINING, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("PXC Training")},
  {RoundTripOptimize,   STS_CHANNEL_TRAINING, ROUND_TRIP_OPTIMIZE, MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Round Trip Optimization")},
#ifdef   SERIAL_DBG_MSG
  {DisplayTrainResults, NO_POST_CODE,         0,          MRC_PF_COLD,  MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Display Training Results")},
#endif// SERIAL_DBG_MSG
  {PostTrainingInit,    NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Post-Training Initialization")},
#ifdef MARGIN_CHECK
  {RankMarginTool,      STS_CHANNEL_TRAINING, STS_RMT,    MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Rank Margin Tool")},
#endif
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Pre SSA Send Slave Data")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Post SSA Get Master Data")},
#ifdef MEM_NVDIMM_EN
  { CoreRestoreNVDIMMs, NO_POST_CODE, 0, MRC_PF_ALL, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Core Restore NVDIMMs") },
  { CoreArmNVDIMMs, NO_POST_CODE, 0,  MRC_PF_ALL, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Core Arm NVDIMMs") },
#endif  //MEM_NVDIMM_EN
  {LateConfig,          STS_CHANNEL_TRAINING, STS_MINOR_END_TRAINING, MRC_PF_ALL, MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Late Configuration")},
  {InitThrottling,      STS_INIT_THROTTLING,  0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Initialize Throttling")},
  {AdvancedMemTest,     STS_MEMBIST,          0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Advanced MemTest")},
  {MemTest,             STS_MEMBIST,          0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("MemTest")},
  {SetupSVLandScrambling, NO_POST_CODE,       0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP, CALL_TABLE_STRING("Enable Misc Settings")},
  {MemInit,             STS_MEMINIT,          0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("MemInit")},
  {CheckRASSupportAfterMemInit, NO_POST_CODE, 0,          MRC_PF_ALL,   MRC_MP_BOTH_LOOP,  CALL_TABLE_STRING("Check Ras Support After MemInit") },
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Send NVRAM Data")},
  {PipeSync,           NO_POST_CODE,         0,           MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Sync Both threads")},
  {SwitchToNormalMode,  STS_CHANNEL_TRAINING, STS_MINOR_NORMAL_MODE, MRC_PF_ALL,   MRC_MP_BOTH_LOOP,      CALL_TABLE_STRING("Switch to Normal Mode")},
  {PipeSync,            NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_PARALLEL,  CALL_TABLE_STRING("Final Error Sync")},
#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  {PrintAllStats,       NO_POST_CODE,         0,          MRC_PF_ALL,   MRC_MP_BOTH,      CALL_TABLE_STRING("Print All Stats")},
#endif
#endif  // DEBUG_PERFORMANCE_STATS
};

static UINT8 CallTableSlaveSize = sizeof(CallTableSlave) / sizeof(CallTableEntryType);

static VddValues   VddValuesTable[] = {
  {SPD_VDD_120, MAILBOX_BIOS_120V_RAMP_ENCODING_VR12, MAILBOX_BIOS_120V_RAMP_ENCODING_VR12_5, MAILBOX_BIOS_120V_RAMP_ENCODING_VR13_5, MAILBOX_BIOS_120V_RAMP_ENCODING_VR13_10, CALL_TABLE_STRING("1.20V\n")},
  {0xFF,        0xFF,                                 0xFF                                  , 0xFF                                  , 0xFF                                   , CALL_TABLE_STRING("Unknown VDD\n")},
};

static UINT8 VddValuesTableSize = sizeof(VddValuesTable) / sizeof(VddValues);

// -------------------------------------
// Code section
// -------------------------------------

/**
  Provide the caller with pointers to call tables and call table sizes

  @param host              - Pointer to SysHost structure
  @param MasterCallTable   - Pointer to pointer to the master call table
  @param MasterTableSize   - Pointer to master table size
  @param SlaveCallTable    - Pointer to pointer to the slave call table
  @param SlaveTableSize    - Pointer to the slave table size

  retval    MRC_STATUS_SUCCESS
**/
MRC_STATUS
GetCallTablePtrs (
  PSYSHOST              host,
  CallTableEntryType    **MasterCallTable,
  UINT8                 *MasterTableSize,
  CallTableEntryType    **SlaveCallTable,
  UINT8                 *SlaveTableSize
  )
{
  *MasterCallTable = CallTableMaster;
  *MasterTableSize = CallTableMasterSize;
  *SlaveCallTable = CallTableSlave;
  *SlaveTableSize = CallTableSlaveSize;

  return MRC_STATUS_SUCCESS;
}



/**
  This function implements a MRC reset loop using scratchpad registers

  @param host:         Pointer to sysHost

  @retval SUCCESS:  Memory initialization completed successfully.
**/
UINT32
DDRResetLoop (
             PSYSHOST                  host
             )
{
  UINT32        spad5;
  UINT32        spad0;
  UINT16        tempCounter;

  if (host->setup.mem.options & DDR_RESET_LOOP) {
    //
    // Check bit 2 to see if we need to keep cycling.
    //
    spad5 = ReadCpuPciCfgEx (host, 0, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG);
    spad0 = ReadCpuPciCfgEx (host, 0, 0, BIOSSCRATCHPAD0_UBOX_MISC_REG);
    //
    // If the user has requested MRC cycling and this is the first
    // time through the loop, set BIT2 of SPAD0 here to keep cycling
    // in the future.  Someone can then clear this bit with ITP or
    // external tools to stop cycling and proceed to system boot.
    //
    if ((spad5 & 0xFFFF) == 0) {
      spad0 |= BIT2;
      WriteCpuPciCfgEx (host, 0, 0, BIOSSCRATCHPAD0_UBOX_MISC_REG, spad0);
    }
    if (spad0 & BIT2) {
      //
      // Increment the loop count.
      //
      tempCounter = (UINT16) spad5;
      tempCounter++;
      spad5 = (spad5 & 0xFFFF0000) + tempCounter;
      WriteCpuPciCfgEx (host, 0, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG, spad5);
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "DDR LC: %d\n", tempCounter));
      host->var.common.resetRequired |= POST_RESET_WARM;
      return  FAILURE;
    } else {
      //
      // Clear out the counter since we're done cycling.
      //
      spad5 = ReadCpuPciCfgEx (host, 0, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG);
      WriteCpuPciCfgEx (host, 0, 0, BIOSSCRATCHPAD5_UBOX_MISC_REG, (spad5 & 0xFFFF0000));
    }
  } // if DDR_RESET_LOOP

  return SUCCESS;
} // DDRResetLoop

/**

  This routine gets memory ready to be written and read

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT32
InitMem (
        PSYSHOST host
        )
{
  UINT8                           socket;
  MC_INIT_STATE_G_MC_MAIN_STRUCT  mcInitStateG;

  socket = host->var.mem.currentSocket;
  GetCpuCsrAccessVar_RC(host, &host->var.CpuCsrAccessVarHost);

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  // Clear WorkAround register prior to starting
  host->nvram.mem.socket[socket].wa = 0;

  //
  // Force CS to be driven during traning
  //
  mcInitStateG.Data = MemReadPciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG);
  mcInitStateG.Bits.cs_oe_en = 0xF;
  MemWritePciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);


  ClearIMCErrors(host);

  return SUCCESS;
} // InitMem

void
UpdateFnvDdrioCache(PSYSHOST host,
              UINT8    socket,
              UINT8    ch
)
{
  UINT8                                dimm;
  UINT8                                rank = 0;
  INT16                                zeroOffset = 0;
  struct dimmNvram                     (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  for(dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
    GetSetCmdVref(host, socket, ch, dimm, DdrLevel, GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &zeroOffset);
    GetSetDataGroup(host, socket, ch, dimm, rank, ALL_STROBES, ALL_BITS, LrbufLevel, TxVref, GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &zeroOffset);
  }
}

void
DisableScrambling(PSYSHOST host,
                  UINT8    socket,
                  UINT8    ch
)
{
  UINT8                                   dimm;
  struct dimmNvram                        (*dimmNvList)[MAX_DIMM];
  DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_STRUCT   txSeedLo;
  DA_RD_SCR_LFSR_FNV_DA_UNIT_0_STRUCT     rxSeed;

  dimmNvList = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
    txSeedLo.Data = ReadFnvCfg(host, socket, ch, dimm, DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_REG);
    rxSeed.Data = ReadFnvCfg(host, socket, ch, dimm, DA_RD_SCR_LFSR_FNV_DA_UNIT_0_REG);

    txSeedLo.Bits.en = 0;
    rxSeed.Bits.en   = 0;

    WriteFnvCfg(host, socket, ch, dimm, DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_REG, txSeedLo.Data);
    WriteFnvCfg(host, socket, ch, dimm, DA_RD_SCR_LFSR_FNV_DA_UNIT_0_REG, rxSeed.Data);
  }
}

void
InvalidateDdrtPrefetchCache(PSYSHOST host,
                UINT8    socket,
                UINT8    ch
)
{
  UINT8                                dimm;
  struct dimmNvram                     (*dimmNvList)[MAX_DIMM];
  DN_SPARE_CTRL_FNV_D_UNIT_0_STRUCT    dSpareCtrlFnv;
  D_DDRT_TRAINING_OTHER_EN_FNV_D_UNIT_0_STRUCT ddrtTrainingOtherEn;

  dimmNvList = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].fmcType == FMC_FNV_TYPE) {
      dSpareCtrlFnv.Data = ReadFnvCfg(host, socket, ch, dimm, DN_SPARE_CTRL_FNV_D_UNIT_0_REG);
      dSpareCtrlFnv.Bits.spare_1 |= BIT15;
      WriteFnvCfg(host, socket, ch, dimm, DN_SPARE_CTRL_FNV_D_UNIT_0_REG, dSpareCtrlFnv.Data);

      dSpareCtrlFnv.Bits.spare_1 &= ~BIT15;
      WriteFnvCfg(host, socket, ch, dimm, DN_SPARE_CTRL_FNV_D_UNIT_0_REG, dSpareCtrlFnv.Data);
    } else { // EKV
      ddrtTrainingOtherEn.Data = ReadFnvCfg(host, socket, ch, dimm, D_DDRT_TRAINING_OTHER_EN_FNV_D_UNIT_0_REG);
      ddrtTrainingOtherEn.Bits.pche_inv_set0_only = 1;
      WriteFnvCfg(host, socket, ch, dimm, D_DDRT_TRAINING_OTHER_EN_FNV_D_UNIT_0_REG, ddrtTrainingOtherEn.Data);
    }
  }
}

void
ProgramDdrtGnt2Erid(PSYSHOST host,
                UINT8    socket,
                UINT8    ch
)
{
  UINT8                                dimm;
  struct dimmNvram                     (*dimmNvList)[MAX_DIMM];
  DA_DDRT_LATENCY_FNV_DA_UNIT_0_STRUCT ddrtLatency;

  dimmNvList = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      ddrtLatency.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_LATENCY_FNV_DA_UNIT_0_REG);
      ddrtLatency.Bits.tcl_gnt_erid = (*dimmNvList)[dimm].gnt2ERID;
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_LATENCY_FNV_DA_UNIT_0_REG, ddrtLatency.Data);
  }
}

void
ProgramtCLtCWLAdd(PSYSHOST host,
                  UINT8    socket,
                  UINT8    ch
)
{
  UINT8                          dimm;
  UINT8                          dimmPair;
  UINT8                          tclAdd;
  UINT8                          tcwlAdd;
  UINT8                          tCWL;
  struct dimmNvram               (*dimmNvList)[MAX_DIMM];
  DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_STRUCT ddrtDqBcomCtrl;
  DA_DDRIO_INIT_CONTROL_FNV_DA_UNIT_0_STRUCT ddrioInitControl;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

      dimmPair = BIT0 ^ dimm;
      if (IsLrdimmPresent(host, socket, ch, dimmPair)) {
        tclAdd  = DdrtCASLatencyAdderLRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
        tcwlAdd = DdrtCASWriteLatencyAdderLRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
        tCWL = DdrtCASWriteLatencyLRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
      } else {
        tclAdd = DdrtCASLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
        if ((*dimmNvList)[dimm].fmcType == FMC_EKV_TYPE) {
          tcwlAdd = EkvDdrtCASWriteLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
          tCWL = EkvDdrtCASWriteLatencyRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
        } else {
          tcwlAdd = DdrtCASWriteLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
          tCWL = DdrtCASWriteLatencyRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
        }
      }
      ddrtDqBcomCtrl.Data   = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG);
      ddrioInitControl.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRIO_INIT_CONTROL_FNV_DA_UNIT_0_REG);
      ddrtDqBcomCtrl.Bits.tcl_add = tclAdd;
      ddrtDqBcomCtrl.Bits.tcwl_add = tcwlAdd;
      // PlaceHolder
      if (tCWL + tcwlAdd <= 12){
        ddrioInitControl.Bits.ddrtio_par_flopen = 0;
      } else {
        ddrioInitControl.Bits.ddrtio_par_flopen = 1;
      }
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG, ddrtDqBcomCtrl.Data);
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRIO_INIT_CONTROL_FNV_DA_UNIT_0_REG, ddrioInitControl.Data);
    } //dimm
}
/**

  This routine gets things ready for DDR training

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT32
PreTrainingInit (
                PSYSHOST host
                )
{
  UINT8                          ch;
  UINT8                          socket = host->var.mem.currentSocket;
  struct channelNvram            (*channelNvList)[MAX_CH];
  CPGC_INORDER_MCDDC_CTL_STRUCT  cpgcInOrder;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);


  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    cpgcInOrder.Data = MemReadPciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG);
    cpgcInOrder.Bits.wpq_inorder_en = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG, cpgcInOrder.Data);

ProgramtCLtCWLAdd(host, socket, ch);
    ProgramDdrtGnt2Erid(host, socket, ch);
    DisableScrambling(host, socket, ch);
    UpdateFnvDdrioCache(host, socket, ch);
    InvalidateDdrtPrefetchCache(host, socket, ch);
  } // ch loop

  InitStartingOffset (host, socket);

  if (host->var.common.bootMode == NormalBoot && host->var.mem.subBootMode == ColdBootFast) {
    CHIP_FUNC_CALL(host, ToggleBcomFlopEn(host, socket, 0));
  }
  return SUCCESS;
} // PreTrainingInit

/**

  This routine is run immediately after DDR training

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT32
PostTrainingInit (
                 PSYSHOST host
                 )
{
  UINT8                                     ch;
  UINT8                                     mcId;
  UINT8                                     strobe;
  MCMTR_MC_MAIN_STRUCT                      mcMtr;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT        dataControl0;
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT        dataControl2;
  AMAP_MC_MAIN_STRUCT                       imcAMAPReg;
  UINT8 socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0)) return SUCCESS;

  IO_Reset(host, socket);

  // HSD 4928504
  //
  // Disable ALL Channels and return if no DIMMs (moved from late config)
  //
  if (host->nvram.mem.socket[socket].maxDimmPop == 0) {

    for (mcId = 0; mcId < MAX_IMC; mcId++) {
      mcMtr.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);

      for (ch = 0; ch < MAX_CH; ch++) {
        if (mcId != GetMCID(host, socket, ch)) continue;
        //
        // MC0.mcMtr.Bits.chn_disable channel 2 - 0 & MC1.mcMtr.Bits.chn_disable channel 5 - 3
        //
        mcMtr.Bits.chn_disable |= BIT0 << (ch % MAX_MC_CH);
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n Disable Channel \n"));
      } // ch loop

      //
      // One channel must remain enabled on each MC
      //
      mcMtr.Bits.chn_disable &= ~(BIT0);
      MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);

    } // mcId loop
    return SUCCESS;
  }

  //
  // Update dimmmtr.rank_disable setting on mapout results after training
  //
  CheckRankPopLocal(host, socket);

  if (host->setup.mem.optionsExt & TURNAROUND_OPT_EN) {
    //
    // Program turnaround timings
    //
    TurnAroundTimings (host, socket);
    
// APTIOV_SERVER_OVERRRIDE_START : Wrapper for OEM to override TurnAroundTimings
    AmiTurnAroundTimingsWrapper (host, socket);
// APTIOV_SERVER_OVERRIDE_END : Wrapper for OEM to override TurnAroundTimings

#ifdef  DEBUG_TURNAROUNDS
    //
    // Check turnaround timings and report via scratchpad
    //
    CheckTurnAroundTimings (host, socket);
#endif  //  DEBUG_TURNAROUNDS
  }

  // HSD 4928504
  //
  // Disable Unused Channels (moved from late config)
  //
  if (host->setup.mem.options & DISABLE_CHANNEL_EN) {
    for (mcId = 0; mcId < MAX_IMC; mcId++) {
      mcMtr.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);

      for (ch = 0; ch < MAX_CH; ch++) {
       if ((*channelNvList)[ch].enabled)  continue;
       if (mcId != GetMCID(host, socket, ch)) continue;
       //
       // MC0.mcMtr.Bits.chn_disable channel 2 - 0 & MC1.mcMtr.Bits.chn_disable channel 5 - 3
       //
       MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n Disable Channel \n"));
       mcMtr.Bits.chn_disable |= BIT0 << (ch % MAX_MC_CH);
      } // ch loop

      //
      // One channel must remain enabled on each MC
      //

      if (mcMtr.Bits.chn_disable == (BIT2 | BIT1 | BIT0)) {
        mcMtr.Bits.chn_disable &= ~(BIT0);
      }

      MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);

    } // mcId loop
  }

  // HSD 4928504
  //
  // TXPION, RXPION, FORCERXON (moved from memLate)
  //

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dataControl0.Data = (*channelNvList)[ch].dataControl0;
    dataControl0.Bits.internalclockson = 1; // HSD 5332375
    dataControl0.Bits.txpion = 0;
    dataControl0.Bits.rxpion = 0;
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      dataControl2.Data = (*channelNvList)[ch].dataControl2[strobe];
      dataControl2.Bits.forcerxon = 0;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2.Data);
      (*channelNvList)[ch].dataControl2[strobe] = dataControl2.Data;
    } // strobe loop
    (*channelNvList)[ch].dataControl0 = dataControl0.Data;
  } // ch loop

  //HSD: 4929590
  //Program  fine_grain_bank_interleaving in AMAP register
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    imcAMAPReg.Data = MemReadPciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG);
    //HSD 5330963 - Always for all configs
    imcAMAPReg.Bits.fine_grain_bank_interleaving = 0x1;
    MemWritePciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG,imcAMAPReg.Data);
  }

  return SUCCESS;
} // PostTrainingInit

/**

  This routine gets memory ready to be written and read for NVMDIMM

  @param host  - Pointer to sysHost
  @param socket  - Processor to initialize

  @retval N/A

**/
VOID
EarlyDdrtConfig (
                PSYSHOST                  host,
                UINT8                     socket
                )
{
  DDRT_TIMING_MCDDC_CTL_STRUCT              ddrtCtlTiming;
  DDRT_TIMING_MC_2LM_STRUCT                 ddrtTiming;
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimm0BasicTiming;
  DDRT_DATAPATH_DELAY2_MCDDC_DP_STRUCT      ddrtDataPathDelay2;
  T_PERSIST_TIMING_MC_2LM_STRUCT            persistTiming;
  DDRT_CREDIT_LIMIT_MC_2LM_STRUCT           ddrtCreditLimit;
  T_DDRT_RD_CNSTRNT_MC_2LM_STRUCT           ddrtRdCnstrnt;
  T_DDRT_WR_CNSTRNT_MC_2LM_STRUCT           ddrtWrCnstrnt;
  T_DDRT_GNT2RW_CNSTRNT_MC_2LM_STRUCT       ddrtGnt2rwCnstrnt;
  T_DDRT_RW2GNT_CNSTRNT_MC_2LM_STRUCT       ddrtRw2gntCnstrnt;
  T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_STRUCT      ddrtGnt2gntCnstrnt;
  T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimm1BasicTiming;
  T_DDRT_PD_MC_2LM_STRUCT                   ddrtPd;
  T_DDRT_PD2_MC_2LM_STRUCT                  ddrtPd2;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT           ddrtMiscDelay;
  DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_STRUCT  ddrtMajorModeTh2;
  CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT          cpgcDdrtMiscCtl;
  DDRT_ERROR_MC_2LM_STRUCT                  errSignals;
  DDRT_ECC_MODE_MC_2LM_STRUCT               ddrtEccMode;
  MODE_DELAYS_MC_2LM_STRUCT                 modeDelays;
  DDRT_DEFEATURE_MC_2LM_STRUCT              ddrtDefeature;
  DDRT_MISC_CTL_MC_2LM_STRUCT               ddrtMiscCtl;
  UINT8                                     ch;
  UINT8                                     dimm;
  UINT8                                     tempData;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  CHN_TEMP_CFG_MCDDC_CTL_STRUCT             ChnTempCfgReg;
  DIMM_TEMP_EV_OFST_0_MCDDC_CTL_STRUCT      dimmTempEvOfst;


#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "EarlyDdrtConfig Starts\n"));
#endif
  if (host->nvram.mem.socket[socket].aepDimmPresent == 0) return;

  if (CheckSteppingEqual(host, CPU_SKX, A0_REV)) {
    // 4929731
    DisableDDRTEridParityErrorLogging(host, socket);
  }
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    ChnTempCfgReg.Data = MemReadPciCfgEp(host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG);
    ChnTempCfgReg.Bits.oltt_en = 0;
    ChnTempCfgReg.Bits.bw_limit_thrt_en = 0;
    ChnTempCfgReg.Bits.thrt_allow_isoch = 1;
    MemWritePciCfgEp(host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG, ChnTempCfgReg.Data);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      dimmTempEvOfst.Data = MemReadPciCfgEp(host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)));
      dimmTempEvOfst.Bits.ev_2x_ref_templo_en = 0;
      MemWritePciCfgEp(host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempEvOfst.Data);
    }

    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;

    ddrtCtlTiming.Data = MemReadPciCfgEp (host, socket, ch, DDRT_TIMING_MCDDC_CTL_REG);
    ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd = 13;
    ddrtCtlTiming.Bits.ddrtwr_to_ddr4wr = 13;
    ddrtCtlTiming.Bits.ddrtwr_to_ddr4rd = 7;
    ddrtCtlTiming.Bits.ddrtrd_to_ddr4wr = 16;
    MemWritePciCfgEp (host, socket, ch, DDRT_TIMING_MCDDC_CTL_REG, ddrtCtlTiming.Data);

    ddrtTiming.Data = MemReadPciCfgEp (host, socket, ch, DDRT_TIMING_MC_2LM_REG);
    ddrtTiming.Bits.rid2dealloc = 6;
    MemWritePciCfgEp (host, socket, ch, DDRT_TIMING_MC_2LM_REG, ddrtTiming.Data);

    if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1600) {
      tempData = 0;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_1866) {
      tempData = 1;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2133) {
      tempData = 2;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2400) {
      tempData = 3;
    } else if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2666) {
      tempData = 4;
    } else {
      tempData = 5;
    }

    host->nvram.mem.socket[socket].ddrtFreq = (UINT8) tempData;
    ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG);
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl = (*channelNvList)[ch].common.nWL - 9;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG, ddrtDimm0BasicTiming.Data);

    // HSD 4929677
    ddrtDataPathDelay2.Data = MemReadPciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG);
    if ((host->setup.mem.optionsExt & WR_CRC) && (ddrtDimm0BasicTiming.Bits.t_ddrt_twl > 7)) {
      ddrtDataPathDelay2.Bits.en_wr_ddrt_mode = 3;
    } else {
      ddrtDataPathDelay2.Bits.en_wr_ddrt_mode = 1;
    }
    MemWritePciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG, ddrtDataPathDelay2.Data);

    ddrtDimm1BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG);
    MemWritePciCfgEp (host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG, ddrtDimm1BasicTiming.Data);

    persistTiming.Data = MemReadPciCfgEp (host, socket, ch, T_PERSIST_TIMING_MC_2LM_REG);
    persistTiming.Bits.t_gnt_dq = 2;
    persistTiming.Bits.t_persist_delay = 0;
    MemWritePciCfgEp (host, socket, ch, T_PERSIST_TIMING_MC_2LM_REG, persistTiming.Data);

    ddrtCreditLimit.Data = MemReadPciCfgEp (host, socket, ch, DDRT_CREDIT_LIMIT_MC_2LM_REG);
    ddrtCreditLimit.Bits.ddrt_rd_credit = 0;
    ddrtCreditLimit.Bits.ddrt_wr_credit = 0;
    MemWritePciCfgEp (host, socket, ch, DDRT_CREDIT_LIMIT_MC_2LM_REG, ddrtCreditLimit.Data);

    errSignals.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG);
    errSignals.Bits.ignore_ddrt_ecc_error = 1;
    errSignals.Bits.ignore_ddrt_err0 = 1;
    errSignals.Bits.ignore_ddrt_err1 = 1;
    errSignals.Bits.ignore_ddr4_error = 1;
    errSignals.Bits.ignore_erid_parity_error = 1;
    MemWritePciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG, errSignals.Data);

    ddrtRdCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_RD_CNSTRNT_MC_2LM_REG);
    ddrtRdCnstrnt.Bits.t_ddrt_rdrd_dd = 6;
    ddrtRdCnstrnt.Bits.t_ddrt_wrrd_dd = 6;
    ddrtRdCnstrnt.Bits.t_ddrt_rdrd_s = 6;
    ddrtRdCnstrnt.Bits.t_ddrt_wrrd_s = 6;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_RD_CNSTRNT_MC_2LM_REG, ddrtRdCnstrnt.Data);

    ddrtWrCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_WR_CNSTRNT_MC_2LM_REG);
    ddrtWrCnstrnt.Bits.t_ddrt_rdwr_dd = 6;
    ddrtWrCnstrnt.Bits.t_ddrt_rdwr_s = 0x1f;
    ddrtWrCnstrnt.Bits.t_ddrt_wrwr_dd = 10;
    ddrtWrCnstrnt.Bits.t_ddrt_wrwr_s = 1;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_WR_CNSTRNT_MC_2LM_REG, ddrtWrCnstrnt.Data);

    ddrtGnt2rwCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_GNT2RW_CNSTRNT_MC_2LM_REG);
    ddrtGnt2rwCnstrnt.Bits.t_ddrt_gntrd_dd = 0xF;
    ddrtGnt2rwCnstrnt.Bits.t_ddrt_gntrd_s = 0xF;
    //
    // changed the values from 0x1 to 15 for turnaround optimization
    //
    ddrtGnt2rwCnstrnt.Bits.t_ddrt_gntwr_dd = 15;
    ddrtGnt2rwCnstrnt.Bits.t_ddrt_gntwr_s = 15;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_GNT2RW_CNSTRNT_MC_2LM_REG, ddrtGnt2rwCnstrnt.Data);

    ddrtRw2gntCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_RW2GNT_CNSTRNT_MC_2LM_REG);
    ddrtRw2gntCnstrnt.Bits.t_ddrt_rdgnt_dd = 0xF;
    ddrtRw2gntCnstrnt.Bits.t_ddrt_rdgnt_s = 0x1F;
    ddrtRw2gntCnstrnt.Bits.t_ddrt_wrgnt_dd = 7;
    ddrtRw2gntCnstrnt.Bits.t_ddrt_wrgnt_s = 7;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_RW2GNT_CNSTRNT_MC_2LM_REG, ddrtRw2gntCnstrnt.Data);

    ddrtGnt2gntCnstrnt.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG);
    ddrtGnt2gntCnstrnt.Bits.t_ddrt_gntgnt_dd = 7;
    ddrtGnt2gntCnstrnt.Bits.t_ddrt_gntgnt_s = 2;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG, ddrtGnt2gntCnstrnt.Data);

    ddrtPd.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_PD_MC_2LM_REG);
    ddrtPd.Bits.t_ddrt_rdpden = 1;
    ddrtPd.Bits.t_ddrt_wrpden = 1;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_PD_MC_2LM_REG, ddrtPd.Data);

    ddrtPd2.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_PD2_MC_2LM_REG);
    ddrtPd2.Bits.t_ddrt_gntpden = 1;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_PD2_MC_2LM_REG, ddrtPd2.Data);

    ddrtMajorModeTh2.Data = MemReadPciCfgEp (host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_REG);
    ddrtMajorModeTh2.Bits.gnt_exit = 1;
    ddrtMajorModeTh2.Bits.pwr_enter = 1;
    ddrtMajorModeTh2.Bits.pwr_exit = 1;
    MemWritePciCfgEp (host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_REG, ddrtMajorModeTh2.Data);

    ddrtMiscDelay.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
    ddrtMiscDelay.Bits.rpq_rid_to_credit_ret = 4;
    ddrtMiscDelay.Bits.wpq_rid_to_fwr = 4;
    ddrtMiscDelay.Bits.wpq_dealloc_to_credit_ret = 2;
//    ddrtMiscDelay.Bits.wpq_rid_to_rt_ufill = 2;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);

    cpgcDdrtMiscCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG);
    cpgcDdrtMiscCtl.Bits.cpgc_max_credit = 0x20;
    cpgcDdrtMiscCtl.Bits.disable_max_credit_check = 0;
    cpgcDdrtMiscCtl.Bits.enable_erid_return  = 1;
    cpgcDdrtMiscCtl.Bits.multi_credit_on = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG, cpgcDdrtMiscCtl.Data);

    ddrtEccMode.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ECC_MODE_MC_2LM_REG);
    ddrtEccMode.Bits.em = 0x1F;
    MemWritePciCfgEp (host, socket, ch, DDRT_ECC_MODE_MC_2LM_REG, ddrtEccMode.Data);

    modeDelays.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, MODE_DELAYS_MC_2LM_REG);
    modeDelays.Bits.ddr4_to_ddrt_delay = 0x20;
    modeDelays.Bits.ddrt_to_ddr4_delay = 0x20;
    MemWritePciCfgEp (host, socket, ch, MODE_DELAYS_MC_2LM_REG, modeDelays.Data);

    // 4929208: MC RPQ command starves when read credit remains 1 and BIDS Clears out the counters
    ddrtDefeature.Data = MemReadPciCfgEp (host, socket, ch, DDRT_DEFEATURE_MC_2LM_REG);
    // no stepping check, as the RTL bug is not going to be fixed until GO
    ddrtDefeature.Bits.ignore_erid_parity_error = 1;
    ddrtDefeature.Data |= (BIT25+BIT24);
    //4929239: PowerOnSafe Setting: DDRT_DEFEATURE.block_gnt2cmd_1cyc should be set to 1
    if (host->setup.mem.Blockgnt2cmd1cyc == 0) {
      ddrtDefeature.Bits.block_gnt2cmd_1cyc = 1; //PO Safe value
    } else {
      ddrtDefeature.Bits.block_gnt2cmd_1cyc = 0; //POR value
    }
    MemWritePciCfgEp (host, socket, ch, DDRT_DEFEATURE_MC_2LM_REG, ddrtDefeature.Data);

    // HSD 5332469 - Back 2 Back writes for DDRT
    if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
      ddrtMiscCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRT_MISC_CTL_MC_2LM_REG);
      ddrtMiscCtl.Data |= BIT26;
      if (CheckSteppingGreaterThan(host, CPU_SKX, B0_REV_SKX)) {
        ddrtMiscCtl.Data |= BIT30;
      }
      MemWritePciCfgEp (host, socket, ch, DDRT_MISC_CTL_MC_2LM_REG, ddrtMiscCtl.Data);
    }
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "EarlyDdrtConfig Ends\n"));
#endif
}

void
CADBChickenBit (
                PSYSHOST host,
                UINT8    socket
)
{
  UINT8                               ch;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT patCadbMuxCtl;
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT   chknBit;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //
    // Program CADB pattern buffer
    //
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, 0x5555);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, 0x0000);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, 0x0000);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX3PB_MCDDC_CTL_REG, 0x0000);

    (*channelNvList)[ch].cadbMuxPattern[0] = 0x5555;
    (*channelNvList)[ch].cadbMuxPattern[1] = 0;
    (*channelNvList)[ch].cadbMuxPattern[2] = 0;
    (*channelNvList)[ch].cadbMuxPattern[3] = 0;


    //
    // Program CADB mux control
    //
    patCadbMuxCtl.Data = 0;
    patCadbMuxCtl.Bits.mux0_control = 1;
    patCadbMuxCtl.Bits.mux1_control = 1;
    patCadbMuxCtl.Bits.mux2_control = 1;
    patCadbMuxCtl.Bits.mux3_control = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, patCadbMuxCtl.Data);
  }


  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    chknBit.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
    chknBit.Bits.def_1 = 1;
    MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, chknBit.Data);
  }
}

/**

  This routine gets memory ready to be written and read

  @param host  - Pointer to sysHost
  @param socket  - Processor to initialize

  @retval N/A

**/
UINT32
EarlyConfig (
            PSYSHOST  host
            )
{
  UINT8                               socket;
  UINT8                               ch;
  UINT8                               dimm;
  UINT8                               rank;
  UINT8                               strobe;
  UINT8                               x4Mode;
  UINT8                               nCLOrg;
  UINT8                               casSupBase;
  UINT8                               mcId;
  UINT8                               maxCL;
  UINT8                               ddrtOnMc0 = 0;
  UINT8                               ddrtOnMc1 = 0;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
  struct ddrRank                      (*rankList)[MAX_RANK_DIMM];
  RXOFFSETN0RANK0_0_MCIO_DDRIO_STRUCT rxOffset;
  MCMTR_MC_MAIN_STRUCT                mcMtr;
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT   chknBit;
  MCMNT_CHKN_BIT_MCDDC_CTL_STRUCT     mcmntChknBit;
  MCMNT_CHKN_BIT2_MCDDC_CTL_STRUCT    mcMntChknBit2;
  TCLRDP_MCDDC_CTL_STRUCT             pagetbl;
  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT    scrambleConfig;
  MCSCRAMBLE_SEED_SEL_MCDDC_DP_STRUCT scrambleSeedSel;
  DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_STRUCT ddrCRClkRanksEnabled;
  MC_INIT_STATE_G_MC_MAIN_STRUCT         mcInitStateG;
  LINK_CFG_READ_1_MCDDC_DP_STRUCT     linkCfg;
#ifdef LRDIMM_SUPPORT
  UINT32                              rm2xx;
#endif
  PMONUNITCTRL_MCDDC_CTL_STRUCT       pmonUnitCtrl;
  MCMAIN_CHKN_BITS2_MC_MAIN_STRUCT    mcMainChknBits2;
  MCSCRAMBLE_SEED_SEL_DDRT_MCDDC_DP_STRUCT   scrambleSeedSelDdrt;
  VSSHIORVREFCONTROL_CTL_MCIO_DDRIOEXT_STRUCT vsshioVrefControl;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  if (host->setup.mem.options & SCRAMBLE_EN) {
    if ((host->var.common.bootMode == NormalBoot) &&
        ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == WarmBoot))) {

      //
      // Verify the value is in range if not use default.
      //
      if (host->setup.mem.scrambleSeedLow > MAX_SCRAMBLE_SEED_LOW){
        host->nvram.mem.scrambleSeed = (host->setup.mem.scrambleSeedHigh << 16) | SCRAMBLE_SEED_LOW;
      } else {
        host->nvram.mem.scrambleSeed = (host->setup.mem.scrambleSeedHigh << 16) | host->setup.mem.scrambleSeedLow;
      }
    }
  } // if scrambling

  channelNvList = GetChannelNvList(host, socket);

  if ((host->var.common.bootMode != S3Resume)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      OutputCheckpoint(host, 0xb3, 0x11, 0);
      ddrCRClkRanksEnabled.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
      ddrCRClkRanksEnabled.Bits.ranken = 0x0;
      MemWritePciCfgEp(host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksEnabled.Data);
      IO_Reset(host, socket);
    }
  }

  // HSD 5370163 - Disable byte enable parity check for full writes
  // For B0/L0/B1 steppings
  if (CheckSteppingGreaterThan (host, CPU_SKX, A2_REV_SKX) && CheckSteppingLessThan(host, CPU_SKX, H0_REV_SKX)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      linkCfg.Data = MemReadPciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG);
      linkCfg.Bits.read_data |= BIT12;
      MemWritePciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG, linkCfg.Data);
    } // ch
  } // stepping

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    mcMainChknBits2.Data = MemReadPciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS2_MC_MAIN_REG);
    // WA HSD 4929795, should be fixed in B0
    if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
      mcMainChknBits2.Bits.dis_pcommit_perf = 1;
    }
    // 5370967 - <H0> RankSpare + eccHarasser failing w/UC TOR TO
    if (CheckSteppingGreaterThan (host, CPU_SKX, A2_REV_SKX)) {
      mcMainChknBits2.Data |=  BIT7;  // defeature
    }
    MemWritePciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS2_MC_MAIN_REG, mcMainChknBits2.Data);

    mcMtr.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
    //
    // Enable ECC if required
    //
    if (host->nvram.mem.eccEn) {
      mcMtr.Bits.ecc_en = 1;
    }

    //
    // Enter CPGC training mode
    //
    mcMtr.Bits.trng_mode = 1;
    MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);
  } // mcId loop

  //
  // Enable the DCLK
  //
  mcInitStateG.Data = MemReadPciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG);
  if (host->var.common.bootMode != S3Resume){
    mcInitStateG.Bits.dclk_enable = 1;
  }
  mcInitStateG.Bits.reset_io    = 0;
  MemWritePciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);

  //
  // Check if there are secrets in memory
  //
  CheckSecrets(host);

  //
  // Check if a DDRT DIMM is present on any MC
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((ch < 3) && (ddrtOnMc0 == 0) && ((*channelNvList)[ch].ddrtEnabled == 1)) {
      ddrtOnMc0 = 1;
    } else if ((ch > 2) && (ddrtOnMc1 == 0) && ((*channelNvList)[ch].ddrtEnabled == 1)) {
      ddrtOnMc1 = 1;
    }
  }

  //
  // en_ddrt_pc6_clkstp_fsm - This bit must be set *on all channels* if the MC has any DDRT DIMM present (5332213)
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    mcmntChknBit.Data = MemReadPciCfgEp (host, socket, ch, MCMNT_CHKN_BIT_MCDDC_CTL_REG);
    if (((ch < 3) && (ddrtOnMc0 == 1)) || ((ch > 2) && (ddrtOnMc1 == 1))) {
      mcmntChknBit.Bits.en_ddrt_pc6_clkstp_fsm = 1;
    } else {
      mcmntChknBit.Bits.en_ddrt_pc6_clkstp_fsm = 0; //Clear bit if MC does not have any DDRT DIMM populated
    }
    MemWritePciCfgEp (host, socket, ch, MCMNT_CHKN_BIT_MCDDC_CTL_REG, mcmntChknBit.Data);
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Insure Scrambling Disable for Training
    //
    scrambleConfig.Data = MemReadPciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG);      // this clears all the enables

    // Same 16-bit value used in lower and upper seed select
    if (scrambleConfig.Bits.seed_lock == 0) {
      scrambleSeedSel.Bits.scrb_lower_seed_sel = host->nvram.mem.scrambleSeed & 0xffff;
      scrambleSeedSel.Bits.scrb_upper_seed_sel = host->nvram.mem.scrambleSeed >> 16;
      MemWritePciCfgEp (host, socket, ch, MCSCRAMBLE_SEED_SEL_MCDDC_DP_REG, scrambleSeedSel.Data);

      scrambleSeedSelDdrt.Bits.scrb_lower_seed_sel_ddrt = host->nvram.mem.scrambleSeed;
      scrambleSeedSelDdrt.Bits.scrb_upper_seed_sel_ddrt = host->nvram.mem.scrambleSeed;
      MemWritePciCfgEp (host, socket, ch, MCSCRAMBLE_SEED_SEL_DDRT_MCDDC_DP_REG, scrambleSeedSelDdrt.Data);
    }
    scrambleConfig.Data = 0;            // clear all the enable bits
    scrambleConfig.Bits.seed_lock = 1;  // this may have been locked already, but we need it locked in either case
    MemWritePciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG, scrambleConfig.Data);

    //
    // Disable 2 cycle bypass before any CPGC activity
    //
    chknBit.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
    chknBit.Bits.dis_2cyc_byp = 1;
    chknBit.Bits.cmd_oe_alwy_on = 1;
    chknBit.Bits.ovrd_odt_to_io = 1;

    //SKX HSD: 5331257 / 5370233
    chknBit.Bits.dis_blck_pm = 0x0;
    if (((*channelNvList)[ch].maxDimm == 2) && ((*channelNvList)[ch].encodedCSMode != 2) && CheckSteppingLessThan(host, CPU_SKX, B1_REV_SKX) && (host->nvram.mem.RASmode & RK_SPARE) != RK_SPARE){
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((IsLrdimmPresent(host, socket, ch, 0) && (*dimmNvList)[0].aepDimmPresent == 0) && (IsLrdimmPresent(host, socket, ch, 1) && (*dimmNvList)[1].aepDimmPresent == 0)) {
        chknBit.Bits.dis_blck_pm = 0x1;
      } // both LRDIMM non AEP
    } // 2 DPC and not 3ds and A/B/L stepping

    chknBit.Bits.dis_rdimm_par_gen = 1; // disable parity generation by default
    MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, chknBit.Data);

    mcmntChknBit.Data = MemReadPciCfgEp (host, socket, ch, MCMNT_CHKN_BIT_MCDDC_CTL_REG);
    mcmntChknBit.Bits.dis_ck_gate_mnts = 0;
    if ((*channelNvList)[ch].ddrtEnabled  && ((*channelNvList)[ch].maxDimm == 1) && CheckSteppingLessThan (host, CPU_SKX, H0_REV_SKX)) {
      mcmntChknBit.Bits.dis_cke_off_durg_thr = 1;
    }
    MemWritePciCfgEp (host, socket, ch, MCMNT_CHKN_BIT_MCDDC_CTL_REG, mcmntChknBit.Data);

    //
    // 4929010: Cloned From SKX Si Bug Eco: CKE chaging when MC2GDCompUpdate Happens
    //
    mcMntChknBit2.Data = MemReadPciCfgEp (host, socket, ch, MCMNT_CHKN_BIT2_MCDDC_CTL_REG);
    mcMntChknBit2.Bits.dis_cke_pri_rcb_gate = 1;
    MemWritePciCfgEp (host, socket, ch, MCMNT_CHKN_BIT2_MCDDC_CTL_REG, mcMntChknBit2.Data);
  } //ch

  if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          rankList = &(*dimmNvList)[dimm].rankList;
          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          //
          // Construct CKE mask
          //
          (*channelNvList)[ch].ckeMask |= 1 << (*rankList)[rank].CKEIndex;

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {

            rxOffset.Data = 0;
            // RxDqsPsel = 1 means OTHER amplifier output.
            // All Bytes, Nibble 0 = always 0
            // All Bytes, Nibble 1 = 1 if x8
            // All Bytes, Nibble 1 = 0 if x4
            if ((*dimmNvList)[dimm].x4Present || (strobe < 9)) {
              rxOffset.Bits.rxdqspsel = 0;
            } else {
              rxOffset.Bits.rxdqspsel = 1;
            }

            (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe] = rxOffset.Data;
            MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG), rxOffset.Data);
          } // strobe loop
        } // rank loop
      } // dimm loop

      //
      // tRCD must be at least 5
      //
      if ((*channelNvList)[ch].common.nRCD < 5) {
        (*channelNvList)[ch].common.nRCD = 5;
      }

      //
      // tRP must be at least 5
      //
      if ((*channelNvList)[ch].common.nRP < 5) {
        (*channelNvList)[ch].common.nRP = 5;
      }

      //
      // tWTR_S must be at least 2
      //
      if ((*channelNvList)[ch].common.nWTR < 2) {
        (*channelNvList)[ch].common.nWTR = 2;
      }
      //
      // tWTR_L must be at least 4
      //
      if ((*channelNvList)[ch].common.nWTR_L < 4) {
        (*channelNvList)[ch].common.nWTR_L = 4;
      }

      //
      // tRAS must be at least 10
      //
      if ((*channelNvList)[ch].common.nRAS < 10) {
        (*channelNvList)[ch].common.nRAS = 10;
      }

      //
      // tRAS must be at most 40
      //
      if ((*channelNvList)[ch].common.nRAS > 63) {
        (*channelNvList)[ch].common.nRAS = 63;
      }

      //
      // tRTP must be at least 4
      //
      if ((*channelNvList)[ch].common.nRTP < 4) {
        (*channelNvList)[ch].common.nRTP = 4;
      }

      //
      // tRRD must be at least 4
      //
      if ((*channelNvList)[ch].common.nRRD < 4) {
        (*channelNvList)[ch].common.nRRD = 4;
      }

    } // ch loop
  } // S3

  for (ch = 0; ch < MAX_CH; ch++) {

    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Initialize to 0
    //
    x4Mode      = 0;

    dimmNvList  = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < host->var.mem.socket[socket].channelList[ch].numDimmSlots; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      //
      // Write the DIMM Memory Technology information to DIMMMTR
      //
      MemWritePciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4), (UINT32) (*dimmNvList)[dimm].dimmMemTech);

      //
      // Check if this is a x4 DIMM
      //
      if ((*dimmNvList)[dimm].x4Present) {
        x4Mode |= (1 << dimm);
      }
    } // dimm loop

    //
    // Set bits indicating which DIMMs are x4 if any
    //
    if (x4Mode) {
      MemWritePciCfgEp (host, socket, ch, X4MODESEL_MCDDC_DP_REG, x4Mode);
    }

    if (host->var.common.bootMode == NormalBoot) {

        //
        // Make sure the selected CAS Latency is supported by each DIMM on this channel
        //
        casSupBase = 7;
        maxCL = 36;
        if ((*channelNvList)[ch].common.casSupRange == HIGH_CL_RANGE) {
          casSupBase = 23;
          maxCL = 52;
        }

        nCLOrg = (*channelNvList)[ch].common.nCL;


      //If Read 2tCK Preamble add 1 to CL
      //SKX TODO: This will need updating for the new proposed CL table in Jedec that adds
      //          some of the intermediate CL settings
      host->var.mem.read2tckCL[ch] = 0;
      if ((host->setup.mem.readPreamble > 0) && (host->nvram.mem.socket[socket].ddrFreq >= DDR_2400)){
        (*channelNvList)[ch].common.nCL++;
        host->var.mem.read2tckCL[ch]++;
      }

      // make sure CL is supported, if not, increment to next setting
        while (!((1 << ((*channelNvList)[ch].common.nCL - casSupBase)) & (*channelNvList)[ch].common.casSup)) {
          (*channelNvList)[ch].common.nCL++;

        if ((*channelNvList)[ch].common.nCL > maxCL) {
          // Use the original version of nCL if we can't find a good one
          (*channelNvList)[ch].common.nCL = nCLOrg;
          host->var.mem.read2tckCL[ch] = 0;
          MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "tCL could not be found in the list of supported tCL settings found in the SPD\n"));
          break;
        }
      } // (while)
    } // S3 resume --> actually this if is for NormalBoot

    if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {
      // Save the default standard timings
      (*channelNvList)[ch].standard.nRRD = (*channelNvList)[ch].common.nRRD;
      (*channelNvList)[ch].standard.nRRD_L = (*channelNvList)[ch].common.nRRD_L;
      (*channelNvList)[ch].standard.nWTR = (*channelNvList)[ch].common.nWTR;
      (*channelNvList)[ch].standard.nRP  = (*channelNvList)[ch].common.nRP;
      (*channelNvList)[ch].standard.nRAS = (*channelNvList)[ch].common.nRAS;
      (*channelNvList)[ch].standard.nRCD = (*channelNvList)[ch].common.nRCD;
      (*channelNvList)[ch].standard.nRC  = (*channelNvList)[ch].common.nRC;
      (*channelNvList)[ch].standard.nRTP = (*channelNvList)[ch].common.nRTP;
      (*channelNvList)[ch].standard.nRFC = (*channelNvList)[ch].common.nRFC;
      (*channelNvList)[ch].standard.nWR  = (*channelNvList)[ch].common.nWR;
      (*channelNvList)[ch].standard.nCL  = (*channelNvList)[ch].common.nCL;
      (*channelNvList)[ch].standard.nFAW = (*channelNvList)[ch].common.nFAW;
    } // S3

    //
    // Check for manual overrides
    //
    if (host->setup.mem.options & MEM_OVERRIDE_EN) {
      if (host->setup.mem.inputMemTime.nRRD != 0) {
        (*channelNvList)[ch].common.nRRD = host->setup.mem.inputMemTime.nRRD;
      }

      if (host->setup.mem.inputMemTime.nRRD_L != 0) {
        (*channelNvList)[ch].common.nRRD_L = host->setup.mem.inputMemTime.nRRD_L;
      }

      if (host->setup.mem.inputMemTime.nWTR != 0) {
        (*channelNvList)[ch].common.nWTR = host->setup.mem.inputMemTime.nWTR;
      }

      if (host->setup.mem.inputMemTime.nRP != 0) {
        (*channelNvList)[ch].common.nRP = host->setup.mem.inputMemTime.nRP;
      }

      if (host->setup.mem.inputMemTime.nRAS != 0) {
        (*channelNvList)[ch].common.nRAS = host->setup.mem.inputMemTime.nRAS;
      }

      if (host->setup.mem.inputMemTime.nRCD != 0) {
        (*channelNvList)[ch].common.nRCD = host->setup.mem.inputMemTime.nRCD;
      }

      if (host->setup.mem.inputMemTime.nRC != 0) {
        (*channelNvList)[ch].common.nRC = host->setup.mem.inputMemTime.nRC;
      }

      if (host->setup.mem.inputMemTime.nRTP != 0) {
        (*channelNvList)[ch].common.nRTP = host->setup.mem.inputMemTime.nRTP;
      }

      if (host->setup.mem.inputMemTime.nRFC != 0) {
        (*channelNvList)[ch].common.nRFC = host->setup.mem.inputMemTime.nRFC;
      }

      if (host->setup.mem.inputMemTime.nWR != 0) {
        (*channelNvList)[ch].common.nWR = host->setup.mem.inputMemTime.nWR;
      }

      if (host->setup.mem.inputMemTime.nCL != 0) {
        (*channelNvList)[ch].common.nCL = host->setup.mem.inputMemTime.nCL;
      }

      if (host->setup.mem.inputMemTime.nFAW != 0) {
        (*channelNvList)[ch].common.nFAW = host->setup.mem.inputMemTime.nFAW;
      }

      if (host->setup.mem.inputMemTime.nCWL != 0) {
        (*channelNvList)[ch].common.nWL = host->setup.mem.inputMemTime.nCWL;
      }
    } // MEM_OVERRIDE_EN

    //
    // Program memory timings for this ch
    //
    ProgramTimings (host, socket, ch);

#ifdef   SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      getPrintFControl(host);
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Mem Timings:\n"));

        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "WR_CRC=%d, TCLK readPreamble=%d, TCLK writePreamble=%d\n",
                       (host->setup.mem.optionsExt & WR_CRC), host->setup.mem.readPreamble+1, host->setup.mem.writePreamble+1));

        //MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        //               "CL adder from 2tclk readPreamble=%d, CWL adder from 2tclk writePreamble=%d\n",
        //               host->var.mem.read2tckCL[ch], host->var.mem.write2tckCWL[ch]));
        //
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tCCD     = %d (+4)\n", (*channelNvList)[ch].common.tCCD));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tCCD_L   = %d (+4)\n", (*channelNvList)[ch].common.tCCD_L));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tCCD_WR  = %d (+4)\n", (*channelNvList)[ch].common.tCCD_WR));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tCCD_WR_L= %d (+4)\n", (*channelNvList)[ch].common.tCCD_WR_L));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tCWL     = %d\n", (*channelNvList)[ch].common.nWL));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tCL      = %d\n", (*channelNvList)[ch].common.nCL));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRP      = %d\n", (*channelNvList)[ch].common.nRP));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRCD     = %d\n", (*channelNvList)[ch].common.nRCD));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRRD     = %d\n", (*channelNvList)[ch].common.nRRD));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRRD_L   = %d\n", (*channelNvList)[ch].common.nRRD_L));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tWTR     = %d\n", (*channelNvList)[ch].common.nWTR));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRAS     = %d\n", (*channelNvList)[ch].common.nRAS));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRTP     = %d\n", (*channelNvList)[ch].common.nRTP));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tWR      = %d\n", (*channelNvList)[ch].common.nWR));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tFAW     = %d\n", (*channelNvList)[ch].common.nFAW));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRC      = %d\n", (*channelNvList)[ch].common.nRC));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "tRFC     = %d\n", (*channelNvList)[ch].common.nRFC));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "casSup   = 0x%x\n", (*channelNvList)[ch].common.casSup));
      releasePrintFControl(host);
    }
#endif


    //
    // Set ODT Matrix
    //
    SetOdtMatrix (host, socket, ch);

    //
    // Set ODT timing parameters
    //
    SetOdtTiming (host, socket, ch);

    //
    //Enable Page Table Aliasing
    //
    pagetbl.Data = MemReadPciCfgEp (host, socket, ch, TCLRDP_MCDDC_CTL_REG);
    pagetbl.Bits.en_pgta2 = 0;


    //
    // for LRDIMM in encoded mode, set subrank timing mode settings
    //
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((IsLrdimmPresent(host, socket, ch, dimm) || (*channelNvList)[ch].encodedCSMode == 2)) {
        if ((*channelNvList)[ch].encodedCSMode == 2) {
          //Enable 3DS support
          pagetbl.Bits.en_3ds = 1;
          switch ((*dimmNvList)[dimm].dieCount) {
            case 2:
              rm2xx = 1;
              break;

            case 3:
            case 4:
              rm2xx = 2;
              break;

            case 5:
            case 6:
            case 7:
            case 8:
              rm2xx = 3;
              break;

            default:
              rm2xx = 0;
              break;
          }
          switch (dimm) {
            case 0:
              pagetbl.Bits.rm2x0 = rm2xx;
              break;

            case 1:
              pagetbl.Bits.rm2x1 = rm2xx;
              break;

            case 2:
              pagetbl.Bits.rm2x2 = rm2xx;
              break;
          }
        } else {
          switch (dimm) {
            case 0:
              if ((*dimmNvList)[dimm].lrRankMult > 1) pagetbl.Bits.rm2x0 = 1;
              break;

            case 1:
              if ((*dimmNvList)[dimm].lrRankMult > 1) pagetbl.Bits.rm2x1 = 1;
              break;

            case 2:
              if ((*dimmNvList)[dimm].lrRankMult > 1) pagetbl.Bits.rm2x2 = 1;
              break;
          }
        }
        //
        // initialize F0BC1x variable
        //
        (*dimmNvList)[dimm].lrBuf_BC1x = 0;

        // HSD 4929072
        // set en_pgta2=1 for 1DPC Direct mapped LRDIMM
        if (((*channelNvList)[ch].encodedCSMode == 0) && ((*channelNvList)[ch].maxDimm == 1) && ((*dimmNvList)[dimm].aepDimmPresent == 0)) pagetbl.Bits.en_pgta2 = 1;
      } // lrdimm
    } //dimm loop

    MemWritePciCfgEp (host, socket, ch, TCLRDP_MCDDC_CTL_REG, pagetbl.Data);
    //
    // Reset counters
    //
    pmonUnitCtrl.Data = 0;
    pmonUnitCtrl.Bits.resetcounterconfigs = 1;
    pmonUnitCtrl.Bits.resetcounters = 1;

    MemWritePciCfgEp (host, socket, ch, PMONUNITCTRL_MCDDC_CTL_REG, pmonUnitCtrl.Data);

  } //ch loop

  // HSD CLONE SKX Sighting: Big variations on Comp-generated codes if Ch0 or Ch3 is not initialized
  for (ch = 0; ch < MAX_CH; ch++) {
    if (((*channelNvList)[ch].enabled == 1) || ((ch != 0) && (ch != 3))) continue;
    vsshioVrefControl.Data = MemReadPciCfgEp(host, socket, ch, VSSHIORVREFCONTROL_CTL_MCIO_DDRIOEXT_REG);
    // 1866, 2133
    if (host->nvram.mem.socket[socket].ddrFreq <= DDR_2133) {
      vsshioVrefControl.Bits.vsshiorvrefctl = 0x005f8225;
    } else { // > 2133
      vsshioVrefControl.Bits.vsshiorvrefctl = 0x005f8625;
    }
    MemWritePciCfgEp (host, socket, ch, VSSHIORVREFCONTROL_CTL_MCIO_DDRIOEXT_REG, vsshioVrefControl.Data);
  }

  // SKX hooks for NVMDIMM
  EarlyDdrtConfig (host, socket);
  EarlyFnvConfigAccess (host, socket);
  CADBChickenBit(host, socket);

  return SUCCESS;

} // EarlyConfig


UINT32
PopulateDynamicVariables (
                          PSYSHOST host
                         )
/*
  This routine is responsible for populating all dynamic variables in the spreadsheet with
  actual values to be programmed into the registers. Any translation required between values
  and register settings must happen here too. For example, if we have a DIMM with CAS latency
  of 11, but the memory controller's register needs to be programmed to "2" for TCL of 11, this
  routine would populate the dynamic variable for CAS latency to 2.
*/
{
  UINT8                       socket;
  UINT8                       channel;
  UINT16                      vlow = 600;
  UINT16                      vhigh = 1184;
  struct channelNvram         (*channelNvList)[MAX_CH];
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  struct ddr4OdtValueStruct   *ddr4OdtValuePtr;

  socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);
  for (channel = 0; channel < MAX_CH; channel++) {
    if ((*channelNvList)[channel].enabled == 0) continue;

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "\n: Create Dynamic Variables for Populated Channels %d:\n", channel));

    dimmNvList = GetDimmNvList(host, socket, channel);

    //Dynamic variables: SKIP. Used to skip execution for line
    //host->nvram.mem.socket[socket].DynamicVars[channel][SKIP] = 0;

    // Dynamic variable: RXVREF
    ddr4OdtValuePtr = LookupDdr4OdtValue(host, socket, channel);
    if (ddr4OdtValuePtr == NULL) {
      RC_ASSERT(ddr4OdtValuePtr != NULL, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_35);
      continue;
    }

    // Selects the Vref voltage value coming out of internal Vref generator.
    // The absolute Vref values are calculated as below:
    host->nvram.mem.socket[socket].DynamicVars[channel][RXVREF] = ((((ddr4OdtValuePtr->mcVref * 1200)/100-vlow) * 127) / (vhigh-vlow)) + 1; //+1 to round up all remainders
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "RXVREF = 0x%02x\n", host->nvram.mem.socket[socket].DynamicVars[channel][RXVREF]));

    //Dynamic variables: IS_UDIMM
    if (host->nvram.mem.dimmTypePresent == UDIMM) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_UDIMM] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_UDIMM] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_UDIMM = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_UDIMM]));


    //Dynamic variables: IS_NOT_UDIMM
    if (host->nvram.mem.dimmTypePresent == UDIMM) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NOT_UDIMM] = 0;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NOT_UDIMM] = 1;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_NOT_UDIMM = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_NOT_UDIMM]));


    //Dynamic variables: IS_X8_MODE
    if ((*dimmNvList)[0].x4Present == 0) {  //SKX has two Dimms per channel
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_X8_MODE_SLOT0] = 1;
     } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_X8_MODE_SLOT0] = 0;
    }

    if ((*dimmNvList)[1].x4Present == 0) {  //SKX has two Dimms per channel
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_X8_MODE_SLOT1] = 1;
     } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_X8_MODE_SLOT1] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_X8_MODE = Slot 0: %d Slot 1: %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_X8_MODE_SLOT0], host->nvram.mem.socket[socket].DynamicVars[channel][IS_X8_MODE_SLOT1]));

/*
    //Dynamic variables: IS_GREAT_THAN_A_STEPPING
     if (host->var.common.stepping > A0_REV_SKX) {
       host->nvram.mem.socket[socket].DynamicVars[channel][IS_GREAT_THAN_A_STEPPING] = 1;
     } else {
       host->nvram.mem.socket[socket].DynamicVars[channel][IS_GREAT_THAN_A_STEPPING] = 0;
     }
     MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CPU Stepping = %d\n", host->var.common.stepping));
     MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "IS_GREAT_THAN_A_STEPPING = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_GREAT_THAN_A_STEPPING]));

    //Dynamic variables: IS_EQUAL_TO_B_STEPPING
    if (host->var.common.stepping == B0_REV_SKX) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_EQUAL_TO_B_STEPPING] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_EQUAL_TO_B_STEPPING] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "IS_EQUAL_TO_B_STEPPING = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_EQUAL_TO_B_STEPPING]));

    //Dynamic variables: IS_LESS_THAN_C_STEPPING
    if (host->var.common.stepping < C0_REV_SKX) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_LESS_THAN_C_STEPPING] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_LESS_THAN_C_STEPPING] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "IS_LESS_THAN_C_STEPPING = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_LESS_THAN_C_STEPPING]));
*/

    //Dynamic variables: IS_NGN_DIMM_SLOT0 ( if Slot0 has an NGN DIMM (enable CKE1 RX) )
    if ( (*dimmNvList)[0].aepDimmPresent == 1 ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_DIMM_SLOT0] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_DIMM_SLOT0] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_NGN_DIMM_SLOT0 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_DIMM_SLOT0]));


    //Dynamic variables: IS_NGN_DIMM_SLOT1 ( if Slot1 has an NGN DIMM (enable CKE3 RX) )
    if( (*dimmNvList)[1].aepDimmPresent == 1 ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_DIMM_SLOT1] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_DIMM_SLOT1] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_NGN_DIMM_SLOT1 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_DIMM_SLOT1]));

    //Dynamic variables: IS_DIMM_SLOT1 ( if Slot1 has a DIMM (enable CKE2 TX) )
    if ( (*dimmNvList)[1].dimmPresent == 1 ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DIMM_SLOT1] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DIMM_SLOT1] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_DIMM_SLOT1 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_DIMM_SLOT1]));


    //Dynamic variables: IS_NO_NGN_DIMM_ANYSLOT ( if no NGN DIMM in either Slot0 or Slot1 )
    //if ( (*dimmNvList)[0].aepDimmPresent == 1  ||  (*dimmNvList)[1].aepDimmPresent == 1 ) {  //keyByte represents SPD Byte 2, We set aepDimmPresent to 1 if reading byte 2 returns 13
    if ( (*dimmNvList)[0].keyByte > SPD_TYPE_DDR4 || (*dimmNvList)[1].keyByte > SPD_TYPE_DDR4 ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NO_NGN_DIMM_ANYSLOT] = 0;
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_ANYSLOT] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NO_NGN_DIMM_ANYSLOT] = 1;
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_NGN_ANYSLOT] = 0;

    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_NO_NGN_DIMM_ANYSLOT = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_NO_NGN_DIMM_ANYSLOT]));

    //Dynamic variables: IS_DDR4_DIMM_SLOT0 ( if Slot0 has a DDR4 DIMM (enable CKE1 TX) )
    if( (*dimmNvList)[0].keyByte == SPD_TYPE_DDR4 &&
      !((*dimmNvList)[0].actKeyByte2 == SPD_UDIMM && (*dimmNvList)[0].numRanks == 1) &&
      !((*dimmNvList)[0].actKeyByte2 == SPD_RDIMM && (*dimmNvList)[0].numRanks == 1)
        ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT0] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT0] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT0 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT0]));


    //Dynamic variables: IS_DDR4_DIMM_SLOT1 ( if Slot1 has a DDR4 DIMM (enable CKE3 TX) )
    if ( (*dimmNvList)[1].keyByte == SPD_TYPE_DDR4 &&
       !((*dimmNvList)[1].actKeyByte2 == SPD_UDIMM && (*dimmNvList)[1].numRanks == 1) &&
       !((*dimmNvList)[1].actKeyByte2 == SPD_RDIMM && (*dimmNvList)[1].numRanks == 1)
        ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT1] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT1] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT1 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SR_RDIMM_SLOT1]));


    //Dynamic variables: IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT0 ( if Slot0 has any DDR4 DIMM other than single-rank UDIMM (enable ODT1 TX) )
    if( (*dimmNvList)[0].keyByte == SPD_TYPE_DDR4 && !( (*dimmNvList)[0].actKeyByte2 == SPD_UDIMM && (*dimmNvList)[0].numRanks == 1 ) ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT0] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT0] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT0 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT0]));


    //Dynamic variables: IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT1 ( if Slot1 has any DDR4 DIMM other than single-rank UDIMM (enable ODT3 TX) )
    if( (*dimmNvList)[1].keyByte == SPD_TYPE_DDR4 && !( (*dimmNvList)[1].actKeyByte2 == SPD_UDIMM && (*dimmNvList)[1].numRanks == 1 ) ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT1] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT1] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT1 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_DDR4_DIMM_BUT_NOT_SRANK_UDIMM_SLOT1]));


    //Dynamic variables: IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT0 ( if Slot0 has dual or quad-rank DIMM, or NGN DIMM (enable CS#1/GNT#0 TX) )
    if ( (*dimmNvList)[0].numRanks == 2 || (*dimmNvList)[0].numRanks == 4 || (*dimmNvList)[0].aepDimmPresent == 1 ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT0] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT0] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT0 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT0]));


    //Dynamic variables: IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT1 ( if Slot1 has dual or quad-rank DIMM, or NGN DIMM (enable CS#5/GNT#1 TX) )
    if ( (*dimmNvList)[1].numRanks == 2 || (*dimmNvList)[1].numRanks == 4 || (*dimmNvList)[1].aepDimmPresent == 1 ) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT1] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT1] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT1 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_DRANK_OR_QRANK_OR_NGN_DIMM_SLOT1]));

    //Dynamic variables: IS_3DS_DIMM_ANYSLOT ( if any DIMM is 3DS (enable C2 TX) )
    //if ( (*channelNvList)[channel].encodedCSMode == 2 ) {          //When reading from SDRAM Package Type SPD Byte 6. If [1:0] Package Type is 2, we set encodedCSMode to 2.
    if ( ((*dimmNvList)[0].SPDDeviceType & 0x3 ) == 0x2 || ((*dimmNvList)[1].SPDDeviceType & 0x3 ) == 0x2 ||
        (((*dimmNvList)[0].dimmPresent == 1 && (*dimmNvList)[0].aepDimmPresent == 1) ||
         ((*dimmNvList)[1].dimmPresent == 1 && (*dimmNvList)[1].aepDimmPresent == 1))) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_3DS_DIMM_OR_DDRT_DIMM_ANYSLOT] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_3DS_DIMM_OR_DDRT_DIMM_ANYSLOT] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_3DS_DIMM_ANYSLOT = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_3DS_DIMM_OR_DDRT_DIMM_ANYSLOT]));


    //Dynamic variables: IS_QRANK_OR_3DS_DIMM_SLOT0 ( if Slot0 has a quad-rank or 3DS DIMM (enable CS#3, 2 TX) )
    if ( (*dimmNvList)[0].numRanks == 4 || ((*dimmNvList)[0].SPDDeviceType & 0x3) == 0x2 || ((*dimmNvList)[0].dimmPresent == 1 && (*dimmNvList)[0].aepDimmPresent == 1)) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_QRANK_OR_3DS_DIMM_OR_DDRT_DIMM_SLOT0] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_QRANK_OR_3DS_DIMM_OR_DDRT_DIMM_SLOT0] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_QRANK_OR_3DS_DIMM_SLOT0 = %d\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_QRANK_OR_3DS_DIMM_OR_DDRT_DIMM_SLOT0]));


    //Dynamic variables: IS_QRANK_OR_3DS_DIMM_SLOT1 ( if Slot1 has a quad-rank or 3DS DIMM (enable CS#7, 6 TX) )
    if ( (*dimmNvList)[1].numRanks == 4 || ((*dimmNvList)[1].SPDDeviceType & 0x3) == 0x2 || ((*dimmNvList)[1].dimmPresent == 1 && (*dimmNvList)[1].aepDimmPresent == 1)) {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_QRANK_OR_3DS_DIMM_OR_DDRT_DIMM_SLOT1] = 1;
    } else {
        host->nvram.mem.socket[socket].DynamicVars[channel][IS_QRANK_OR_3DS_DIMM_OR_DDRT_DIMM_SLOT1] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "IS_QRANK_OR_3DS_DIMM_SLOT1 = %d\n\n", host->nvram.mem.socket[socket].DynamicVars[channel][IS_QRANK_OR_3DS_DIMM_OR_DDRT_DIMM_SLOT1]));
  } // channel loop

  return SUCCESS;
}

UINT32
RunMmrc (
         PSYSHOST host
        )
{
  UINT32                    status = SUCCESS;
  UINT8                     socket;
  UINT8                     channel;
  UINT8                     i;
  struct channelNvram     (*channelNvList)[MAX_CH];

  socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);
  //
  // For debug, enable register access printout.
  //
  // host->var.mem.serialDebugMsgLvl |= SDBG_REG_ACCESS;
  //
  // Save the host structure so certain functions can use it without
  // it being passed as a parameter. This saves code size in phone/tablet segments.
  //
  SaveMrcHostStructureAddress (host);
  //
  // Translate the bootmode and subbootmode to MMRC-known bootmodes.
  //
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast) {
    host->var.common.mmrcBootMode = S3;
  } else {
    if (host->var.mem.subBootMode == ColdBootFast) {
      host->var.common.mmrcBootMode = FB;
    } else if (host->var.mem.subBootMode == WarmBoot) {
      host->var.common.mmrcBootMode = S0;
    } else if (((SubBootMode)(host->var.common.bootMode)) == ColdBoot) {
      host->var.common.mmrcBootMode = S5;
    } else if (host->var.mem.subBootMode == AdrResume) {
      host->var.common.mmrcBootMode = S3_ADR;
    } else {
      //
      // Default to cold boot.
      //
      host->var.common.mmrcBootMode = S5;
    }
  }

  //
  // Needs to be populated with actual values.
  //
  for (channel = 0; channel < MAX_CH; channel++) {

    if (CheckSteppingGreaterThan(host, CPU_SKX, B1_REV_SKX)) {
      (*channelNvList)[channel].CurrentPlatform = P_H0;
    } else if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
      (*channelNvList)[channel].CurrentPlatform = P_B0;
    } else {
      (*channelNvList)[channel].CurrentPlatform = P_A0;
    }

    switch (host->nvram.mem.socket[socket].ddrFreq) {
      case DDR_800:
        (*channelNvList)[channel].CurrentFrequency = F_800;    //CS: need check why it can not be set to F_800
        break;
      case DDR_1066:
        (*channelNvList)[channel].CurrentFrequency = F_1066;
        break;
      case DDR_1333:
        (*channelNvList)[channel].CurrentFrequency = F_1333;
        break;
      case DDR_1600:
        (*channelNvList)[channel].CurrentFrequency = F_1600;
        break;
      case DDR_1866:
        (*channelNvList)[channel].CurrentFrequency = F_1866;
        break;
      case DDR_2133:
        (*channelNvList)[channel].CurrentFrequency = F_2133;
        break;
      case DDR_2400:
        (*channelNvList)[channel].CurrentFrequency = F_2400;
        break;
      case DDR_2666:
        (*channelNvList)[channel].CurrentFrequency = F_2666;
        break;
      case DDR_3200:
        (*channelNvList)[channel].CurrentFrequency = F_3200;
        break;
      default:
        (*channelNvList)[channel].CurrentFrequency = F_1600;
        break;
    }
    //
    // Add support for other CPU types here (CNX, EX-CPUs if sharing the same MRC, etc).
    //
    switch (host->var.common.cpuType) {
      case CPU_SKX:
        (*channelNvList)[channel].CurrentConfiguration = C_SKX;
        break;
      default:
        (*channelNvList)[channel].CurrentConfiguration = C_SKX;
        break;
    }


    (*channelNvList)[channel].CurrentDdrType = T_DDR4;
    //
    // Since CurrentX variables are setup as BITx, need to convert BITX into x for the
    // variable that MMRC needs to do its platform comparisons in the CreatePfctSel.
    //
    // For example, BIT3 needs to be converted to 3. "3" is also used for array lookups
    // as well instead of BIT3.
    //
    for (i = 0; i < 32; i++) {
      if (((*channelNvList)[channel].CurrentPlatform & (1 << i)) != 0) {
        (*channelNvList)[channel].CurrentPlatform = i;
      }
      if (((*channelNvList)[channel].CurrentFrequency & (1 << i)) != 0) {
        (*channelNvList)[channel].CurrentFrequency = i;
      }
      if (((*channelNvList)[channel].CurrentConfiguration & (1 << i)) != 0) {
        (*channelNvList)[channel].CurrentConfiguration = i;
      }
      if (((*channelNvList)[channel].CurrentDdrType & (1 << i)) != 0) {
        (*channelNvList)[channel].CurrentDdrType = i;
      }
    }
    MemDebugPrint((host, SDBG_MAX, socket, channel, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Platform %d, Frequency %d, Configuration %d, Type %d\n",
      (*channelNvList)[channel].CurrentPlatform,
      (*channelNvList)[channel].CurrentFrequency,
      (*channelNvList)[channel].CurrentConfiguration,
      (*channelNvList)[channel].CurrentDdrType
      ));
  }
  //
  // Disable any sort of parallel operation since MMRC will only be used
  // serially on SKX. Parallel operation involved MMRC training algos which
  // setup channels to run CPGC in parallel.
  //
  host->var.mem.EnableParallelTraining = FALSE;
  PopulateDynamicVariables (host);
  //
  // Now that MMRC knows the current platform, frequency, configuration, and type,
  // invoke the entry point to process the spreadsheet data.
  //
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "Calling MmrcEntry()\n"));

  status = (UINT8) MmrcEntry (host);
#ifdef SERIAL_DBG_MSG
  //
  // Turn off debug register access printouts.
  //
  host->var.mem.serialDebugMsgLvl &= ~SDBG_REG_ACCESS;
#endif
  DumpAllRegisters();

  return status;
}



/**

  Report VDD Error

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
ReportVddError (
               PSYSHOST host,
               UINT8    socket
               )
{
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Cant program VDD!\n"));
  OutputError (host, ERR_SET_VDD, ERR_UNKNOWN_VR_MODE, socket, 0xFF, 0xFF, 0xFF);
}

/**

  Program Vddq

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param SvidEncodingValue - VRM12 defined VR Ramp Encoding Value

  @retval N/A

**/
void
SetVdd (
       PSYSHOST host,
       UINT8    socket
       )
{
  UINT8                               ch;
  UINT16                              userVdd;
  UINT8                               status;
  UINT32                              PcodeMailboxStatus = 0;
  UINT32                              ActiveVrMask = 0;
  UINT32                              VrMode = 0;
  UINT32                              VRData;
  UINT32                              svidEncoding = 0x00;
  UINT8                               VrCount;
  UINT8                               Count;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  BIOSSCRATCHPAD1_UBOX_MISC_STRUCT    biosScratchPad1;
  MC_INIT_STATE_G_MC_MAIN_STRUCT      mcInitStateG;
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT    miscCKECtl;
  UINT32 VrAddress[] = {
    MAILBOX_BIOS_DRAM0_VR_ADDR,
    MAILBOX_BIOS_DRAM1_VR_ADDR,
    MAILBOX_BIOS_DRAM2_VR_ADDR,
    MAILBOX_BIOS_DRAM3_VR_ADDR
  };
  VddValues        *vddptr;

  channelNvList = GetChannelNvList(host, socket);

  OutputExtendedCheckpoint((host, STS_CLOCK_INIT, SUB_SET_CLK_VDD, socket));
  //
  // Only assert reset for cold boot cases
  //
  if (((host->var.common.bootMode == NormalBoot) &&
      ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast)))) {
    //
    // Assert reset
    //
    mcInitStateG.Data             = MemReadPciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG);
    // Clear ddr_reset to assert reset
    mcInitStateG.Bits.ddr_reset   = 0;
    mcInitStateG.Bits.reset_io    = 0;
    MemWritePciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);
  }

  // Loop for each channel
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    miscCKECtl.Bits.cke_override = 0xFF;
    miscCKECtl.Bits.cke_on = 0;
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
  } // ch loop

  //
  // Read the Active DRAM VR's from the Mailbox Data Register
  //
  PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, (UINT32) MAILBOX_BIOS_CMD_ACTIVE_VR_MASK, 0);
  if (PcodeMailboxStatus == 0) ActiveVrMask = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
  else ReportVddError(host, socket);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   ":: Read the Active DRAM VR's from the Mailbox Data Register: %x\n", ActiveVrMask));

  // Set Command
  VRData = (MAILBOX_BIOS_DATA_VR_REG_PROTOCOL_ID << 16) | MAILBOX_BIOS_VR_CMD_GET_REG;

  //
  // Make sure the requested Vdd makes sense
  //
  userVdd = host->setup.mem.inputMemTime.vdd;
  if (userVdd != 0) {
    if((userVdd < MIN_DDR4_VDD) || (userVdd > MAX_DDR4_VDD)) {
      userVdd = 0;
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "User requesed a Vdd setting that is out of range.\n"));
    }
  }

  for (Count = 0 ; Count < sizeof(VrAddress)/sizeof(UINT32) ; Count++) {

    //
    // If VR is active, program requested voltage
    //
    if (ActiveVrMask & (BIT0 << Count)) {
      //
      // Use MAILBOX_BIOS_CMD_VR_INTERFACE commandto read protocol ID register 0x05 for VR Mode
      //
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, (UINT32) (MAILBOX_BIOS_CMD_VR_INTERFACE | ( VrAddress[Count] << 8)), VRData);
      if (PcodeMailboxStatus == 0) VrMode = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      else if ( PcodeMailboxStatus == MAILBOX_BIOS_ERROR_CODE_BAD_VR_ADDR) {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n:: B2P responds MAILBOX_BIOS_ERROR_CODE_BAD_VR_ADDR for 0x%x\n", VrAddress[Count] ));
        break;
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n:: B2P error code = 0x%x\n", PcodeMailboxStatus));
        ReportVddError(host, socket);
      }

      //
      // Setup Encoding value based on VrMode and Selected Voltage
      //
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "VR%d DDR Voltage: ", Count));

      if (userVdd != 0) {
        //
        // Use the user provided Vdd if it is not 0
        //
        if ((VrMode == VR_MODE_12) || (VrMode == VR_MODE_13_5)) {
          svidEncoding = ((userVdd - 250) / 5) + 1;
        } else {
          svidEncoding = ((userVdd - 500) / 10) + 1;
        }
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "%d.%d\n", userVdd / 1000, userVdd % 1000));
      } else {

        //
        // Setup Encoding value based on VrMode and Selected Voltage
        //
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "DDR Voltage: "));

        vddptr = &VddValuesTable[0];
        for (VrCount = 0 ; VrCount < VddValuesTableSize ; VrCount++) {
          vddptr = &VddValuesTable[VrCount];
          if (vddptr->ddrVoltageValue == host->nvram.mem.socket[socket].ddrVoltage) break;
        }

        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, vddptr->string_ptr));

        if (vddptr->ddrVoltageValue == 0xFF) ReportVddError(host, socket);
        else if (VrMode == VR_MODE_12) svidEncoding = vddptr->Vr12Encoding;
        else if (VrMode == VR_MODE_12_5) svidEncoding = vddptr->Vr125Encoding;
        else if (VrMode == VR_MODE_13_5) svidEncoding = vddptr->Vr135Encoding;
        else if (VrMode == VR_MODE_13_10) svidEncoding = vddptr->Vr1310Encoding;
      }

      //
      // Set appropriate Vdd
      //
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n:: VrAddress[%d] = %x, write MAILBOX_BIOS_CMD_VDD_RAMP,  svidEncoding = 0x%x\n", Count, VrAddress[Count], svidEncoding));
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, (UINT32) (MAILBOX_BIOS_CMD_VDD_RAMP | (VrAddress[Count] << 8)), svidEncoding);
      if (PcodeMailboxStatus != 0) {
        ActiveVrMask &= ~(BIT0 << Count);
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "VR%d SVID Command Failed\n", Count));
      }
    }
  } // count loop

  if ((ActiveVrMask & (BIT3 | BIT2 | BIT1 | BIT0)) == 0) {
    //
    // No SVID detected, call a hook to see if a platform specific Set VDD function is available
    //
    status = CoreSetVdd(host, socket);
    if (status == FAILURE) {
      //
      // If failure, change the host structure to reflect it's running at default (v1.2)
      //
      host->nvram.mem.socket[socket].ddrVoltage = SPD_VDD_120;
      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n:: A platform specific Set VDD function is failure. Now it's running at default (v1.2).\n"));

    }
  }

  //
  // set base voltage into bios scratchpad1 register bits 1-0 (zero out rest of bits for now)
  //  0 = Not set yet (read before MRC initializes the value)
  //  1 = 1.5V
  //  2 = 1.35V
  //  3 = Reserved for 1.2x setting
  //
  biosScratchPad1.Data = ReadCpuPciCfgEx (host, socket, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG);
  biosScratchPad1.Data = biosScratchPad1.Data & ~(BIT1+BIT0);

  if (host->nvram.mem.socket[socket].ddrVoltage == SPD_VDD_120) {
    biosScratchPad1.Data = biosScratchPad1.Data | 3;
  } else if (host->nvram.mem.socket[socket].ddrVoltage == SPD_VDD_135) {
    biosScratchPad1.Data = biosScratchPad1.Data | 2;
  } else if (host->nvram.mem.socket[socket].ddrVoltage == SPD_VDD_150) {
    biosScratchPad1.Data = biosScratchPad1.Data | 1;
  } else {
    biosScratchPad1.Data = biosScratchPad1.Data | 0;
  }

  WriteCpuPciCfgEx (host, socket, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG, biosScratchPad1.Data);

  //
  // Initialization complete
  //
} // SetVdd


#define LT_ESTS   0xFED30008
#define LT_E2STS  0xFED308F0
/**

  Checks for secrets in memory

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
CheckSecrets (
             PSYSHOST host
             )
{
  CAPID0_PCU_FUN3_STRUCT capId0;

  capId0.Data = host->var.common.procCom[0].capid0;
  if (capId0.Bits.lt_sx_en) {
    if (((*(UINT32 *) LT_ESTS) & BIT6)) {
      host->var.mem.wipeMemory = 1;
      host->var.mem.skipMemoryInit = 0;
    } else if (((*(UINT32 *) LT_E2STS) & BIT1)) {
      host->var.mem.wipeMemory = 1;
      host->var.mem.skipMemoryInit = 0;
    } else {
      host->var.mem.wipeMemory = 0;
    }
  }
} // CheckSecrets


/**

  Reset the DDR channels for the given socket number (or per MC for BSSA)

  host      - Pointer to sysHost
  socket    - Socket Id
  chBitMask - Bitmask of channels to reset

  @retval N/A

**/
void
ResetAllDdrChannels (
                    PSYSHOST host,
  UINT8     socket,
  UINT32    chBitMask
                    )
{
  UINT8   mcId;
  UINT8   ch;
  UINT8   mcStart = 0xF;
  UINT8   mcEnd = 0;
  UINT8   nv_ckeMask;
  struct channelNvram (*channelNvList)[MAX_CH];
  MC_INIT_STATE_G_MC_MAIN_STRUCT     mcInitStateG;
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT   miscCKECtl;

  //
  // Bypass for the S3 resume path
  //
  if ((host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast)) {

    return;
  }

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nReset All Channels\n"));

#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  countTrackingData(host, JEDEC_COUNT, 1);
#endif
#endif // DEBUG_PERFORMANCE_STATS

  channelNvList = GetChannelNvList(host, socket);

  //Here, from the Channel Bit Mask, we calculate the number of MC's present //For non-BSSA case, this will always be 2 for SKX
  //Possibilities in HSX: Ch 0,1,2,3 - MC0    OR    Ch 0,1 - MC0 and Ch 2,3 - MC1
  //Possibilities in SKX: Ch 0,1,2 - MC0    OR    Ch 3,4,5 - MC1
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((chBitMask & (1 << ch)) == 0) continue;

   //mcId = 0 or 1; if 0, then mcStart = 0 ... GetMCID - Returns the memory controller ID
    mcId = GetMCID(host, socket, ch); //Get MC channel belongs to
    if(mcStart > mcId) {  //always set to 0
      mcStart = mcId; // 0 or 1
    }
    if (mcEnd < mcId) {  //0 OR will be set to 1 --> if we have 2 MC's (HSX)
      mcEnd = mcId;
    }
  } //ch loop

  // De-assert CKE
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((chBitMask & (1 << ch)) == 0) continue;

    miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    miscCKECtl.Bits.cke_override = (*channelNvList)[ch].ckeMask;
    miscCKECtl.Bits.cke_on = 0;
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
  } // ch loop



  {
    //
    // Assert reset
    //
    mcInitStateG.Data = MemReadPciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG);

    //Setting the reset bits for each MC
    for (mcId = mcStart; mcId <= mcEnd; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
      //
      // DDR_RESET & RESET_IO is controlled from HA0
      //
      // Clear ddr_reset to assert reset
      mcInitStateG.Bits.ddr_reset   = 0;
      mcInitStateG.Bits.reset_io    = 0;
      MemWritePciCfgMC (host, socket, mcId, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);
    } // mcId loop

    //
    // Reset should be held for JEDEC mandated 200us, but a 2 usec
    // fixed delay in MRC should be plenty for stable power DDR reset flow.
    //
    if (host->var.mem.firstJEDECDone) {
      FixedDelay(host, 2);
    } else {
      FixedDelay (host, 200);
      host->var.mem.firstJEDECDone = 1;
    }
  }

  //
  // De-assert reset
  //
  //
  // DDR_RESET & RESET_IO is controlled from HA0
  //
  //Setting the bits for each MC
  for (mcId = mcStart; mcId <= mcEnd; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
    mcInitStateG.Bits.ddr_reset = 1;
    MemWritePciCfgMC (host, socket, mcId, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);
  } // mcId loop

  //
  // Reset deassertion to CKE assertion (JEDEC mandated 500us)
  //
  FixedDelay (host, 500);

  if ((host->nvram.mem.dramType == SPD_TYPE_DDR3) || (host->nvram.mem.dimmTypePresent != RDIMM)) {
    // Loop for each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((chBitMask & (1 << ch)) == 0) continue;
      nv_ckeMask =  (*channelNvList)[ch].ckeMask;
      // Assert CKE
      miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
      miscCKECtl.Bits.cke_override = (*channelNvList)[ch].ckeMask;
      miscCKECtl.Bits.cke_on = nv_ckeMask;
      MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
    } // ch loop

    //
    // wait the 20 nano sec tCKSRX
    //
    FixedDelay (host, 1);
  } else {
    //
    // wait 5 usec for tStab
    //
    FixedDelay (host, 5);
  }

} // ResetAllDdrChannels

/**

This function applies the given ZQ mask

@param host      - Pointer to sysHost
@param socket    - Socket
@param ch        - Channel
@param ZQMask    - ZQ mask to apply

@retval none

**/
VOID
SetZQMask (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     ZQMask
  )
{
  CPGC_MISCZQCTL_MCDDC_CTL_STRUCT  miscZqCtl;

  miscZqCtl.Data = MemReadPciCfgEp(host, socket, ch, CPGC_MISCZQCTL_MCDDC_CTL_REG);
  //
  // ZQ
  //
  miscZqCtl.Bits.always_do_zq = 0; // ZQ operation will take place at the normal programmed period after a refresh occurs as long as the DIMM/rank does not appear in the DimmRankPairs array
  miscZqCtl.Bits.zq_rankmask = ZQMask;
  MemWritePciCfgEp(host, socket, ch, CPGC_MISCZQCTL_MCDDC_CTL_REG, miscZqCtl.Data);

  return;
} // SetZQMask

/**

This function applies the given Refresh mask

@param host      - Pointer to sysHost
@param socket    - Socket
@param ch        - Channel
@param refMask   - Refresh mask to apply

@retval none

**/
VOID
SetRefreshMask (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     refMask
  )
{
  CPGC_MISCREFCTL_MCDDC_CTL_STRUCT  miscRefCtl;

  miscRefCtl.Data = MemReadPciCfgEp(host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG);
  //
  // Enable Refresh - 0's to the the desired ranks
  //
  miscRefCtl.Bits.ref_rankmask = refMask; // Enable all non-selected ranks
  MemWritePciCfgEp(host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG, miscRefCtl.Data);

  return;
} // SetRefreshMask

/**

This function applies the given CKE override

@param host      - Pointer to sysHost
@param socket    - Socket
@param ch        - Channel
@param ckeMask   - CKE settings to apply
@param assertCke - Flag to ASSERT/DEASERT CKE

@retval none

**/
VOID
SetCkeMask (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     ckeMask,
  UINT8     assertCke
)
{
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT  miscCKECtl;

  // Override CKE
  miscCKECtl.Data = MemReadPciCfgEp(host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
  miscCKECtl.Bits.cke_override |= ckeMask ; //Keep all the prev overrides and enable current overrides also.
  if (assertCke){
    miscCKECtl.Bits.cke_on |= ckeMask; //Keep all the prev overrides and enable current overrides also.
  } else {
    miscCKECtl.Bits.cke_on &= ~ckeMask;
  }
  MemWritePciCfgEp(host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);

  return;
} // SetCkeMask

/**

This function asserts CKE on each channel and unmasks periodic refresh

@param host    - Pointer to sysHost
@param socket  - Socket

@retval none

**/
VOID
ExitSelfRefreshCriticalSection(
  PSYSHOST  host,
  UINT8     socket
)
{
  UINT8                               ch;
  struct channelNvram(*channelNvList)[MAX_CH];
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT  miscCKECtl;
  CPGC_MISCREFCTL_MCDDC_CTL_STRUCT  miscRefCtl;
  UINT64_STRUCT                     startTsc = { 0, 0 };
  UINT64_STRUCT                     endTsc = { 0, 0 };
  channelNvList = GetChannelNvList(host, socket);

  //
  // Ensure that subordinate functions are cached
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
  } // ch loop
  //
  // Enable periodic refresh engine
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    miscCKECtl.Bits.refresh_enable = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
  } // ch loop


  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    // Set up CKE overrides
    miscCKECtl.Data = MemReadPciCfgEp(host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    miscCKECtl.Bits.cke_override |= 0xFF;
    miscCKECtl.Bits.cke_on |= 0xFF;

    // Clear refresh mask
    miscRefCtl.Data = MemReadPciCfgEp(host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG);
    miscRefCtl.Bits.ref_rankmask = 0;

    // Exit Self-refresh state by taking CKE high
    MemWritePciCfgEp(host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);

    // Guarantee tXS of 360ns in case CSR access is too fast
    ReadTsc64(&startTsc);
    do {
      ReadTsc64(&endTsc);
    } while (TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_NS) < 360);

    // Begin periodic auto-refresh (timing critical)
    // Note that one refresh command should already be queued and will occur immediately
    MemWritePciCfgEp(host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG, miscRefCtl.Data);

  } // ch loop

  //
  // Indicate that host periodic auto-refresh is now enabled
  //
  host->var.mem.socket[socket].hostRefreshStatus = 1;
}

UINT8
ExitSelfRefreshCriticalSectionEndMarker()
/*
This function needs to immediately follow the
ExitSelfRefresh() funtion.  This provides an end label that C
can use to read the entire function (ExitSelfRefresh) into
cache before executing it.
*/
{
  UINT8 status;
  status = 0;
  return status;
}

/*
  Handles the S3 exit flow. After the DIMM configuration is
  restored, BIOS can perform the following steps to bring the
  DRAM out of self-refresh and enabling the refresh operations:
  1. Enable DCLK at DDR interface and wait for tSTAB (~6usec)
     for clock to be stable.
  2. Set up  commands:
     A. NOP with tXS wait
     B. Refresh with tRFC wait
     C. ZQCL with tZQoper wait
  3. Set rank disable at DIMMMTR_x.RANK_DISABLE for any rank
     that is populated but need to be disabled from all
     channels.
  4. Set CKE via MC_INIT_STATE_Cx.CKE_ON[5:0] for all
     occupied channels (up to four CSR writes).
  5. Wait tRFC+10ns (but subtract the amount of time took for
     the last CSR write from step #4) via subsequence control
  7. Poll for done
  8. Enable automatic refresh via setting REFRESH_ENABLE in
     MC_INIT_STATE_G

  Due to the timing constraint of the above step 4-8 which may affect the DRAM refresh
  operation without violating 9xTREFI panic refresh requirement, these steps (4-8) must
  be pre-loaded into the LLC during the Non-Evicted Memory Mode (NEM); otherwise, the
  fetch delay from the Flash device may take a long time and violate the refresh timing.
*/
void
ExitSelfRefresh (
                PSYSHOST host,
                UINT8 socket
                )
{
  UINT8                             ch;
  struct channelNvram               (*channelNvList)[MAX_CH];
#ifdef MEM_NVDIMM_EN
  UINT8                             dimm;
  UINT8                             rank;
  UINT16                            MR6;
  UINT32                            lrbufData;
  struct   dimmNvram                (*dimmNvList)[MAX_DIMM];
  struct   rankDevice               (*rankStruct)[MAX_RANK_DIMM];
  struct   ddrRank                  (*rankList)[MAX_RANK_DIMM];
#endif  //MEM_NVDIMM_EN

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Exit Self Refresh\n"));

  channelNvList = GetChannelNvList(host, socket);

  //
  // Setup engine with commands needed for SR exit.
  // 1. NOP with tXS wait
  // 2. ZQCL with tZQoper wait
  // 3. Refresh with tRFC wait
  //

  //
  // Bring the IO out of reset on S3 path before enabling refresh
  // and switching to normal mode.
  //
#ifdef MEM_NVDIMM_EN
  if (host->var.mem.subBootMode != NvDimmResume) {
#endif
    //
    // Assert that refresh engine is not enabled
    // TODO: move assert inside IO_Reset function and verify hostRefreshStatus is set on normal path
    //
    RC_ASSERT((host->var.mem.socket[socket].hostRefreshStatus == 0), ERR_RC_INTERNAL, 0);
    IO_Reset(host, socket);
#ifdef MEM_NVDIMM_EN
  }
#endif

  //Initialize the masks before starting the refresh engine
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Deassert CKE
    //
    SetCkeMask(host, socket, ch, 0xFF, 0);

    // Disable Refresh
    SetRefreshMask(host, socket, ch, 0xFF);

    // Disable ZQCALs
    SetZQMask(host, socket, ch, 0xFF);
  } // ch loop

#ifdef MEM_NVDIMM_EN
  if (host->var.mem.subBootMode == NvDimmResume) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        if ((*dimmNvList)[dimm].nvDimmType) {

          //
          // Note: RCD PLL settings must be written while CKE is low to prevent violations on CK input to DRAM.
          // This step must be done via SMBus interface because the RCD is in CKE PD state after NVDIMM restore flow
          // and will ignore RCW writes until CKE goes high.
          //

          // SMBus offset 0xD byte contains RC0B in bits[7:4] and RC0A in bits[3:0]
          lrbufData = ((*dimmNvList)[dimm].rcCache[RDIMM_RC0B] << 4) | ((*dimmNvList)[dimm].rcCache[RDIMM_RC0A] & 0x0F);
          WriteLrbufSmb(host, socket, ch, dimm, 1, 0x0D, &lrbufData);

          // SMBus offset 0x12 byte contains RC3x in bits[7:0]
          lrbufData = (*dimmNvList)[dimm].rcxCache[RDIMM_RC3x >> 4];
          WriteLrbufSmb (host, socket, ch, dimm, 1, 0x12, &lrbufData);

        } // nvDimmType
      } // dimm loop
    } // ch loop
  } // if NvDimmResume
#endif  //MEM_NVDIMM_EN

  disableAllMsg(host);

  //
  // Assert CKE on each channel and unmask periodic refresh
  // Note that this function must be prefilled into cache before calling.
  //
  ExitSelfRefreshCriticalSection(host, socket);

  //
  // Restore output messages after auto refresh is enabled
  //
  restoreMsg(host);

#ifdef MEM_NVDIMM_EN
  if (host->var.mem.subBootMode == NvDimmResume) {  // Initialize MRx rgisters for NVDIMMs

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        if ((*dimmNvList)[dimm].nvDimmType) {

          DoRegisterInitDDR4(host, socket, ch, dimm, 0);
          DoRegisterInitDDR4_CKE(host, socket, ch, dimm, 0);

          rankList = GetRankNvList(host, socket, ch, dimm);
          rankStruct = GetRankStruct(host, socket, ch, dimm);
          for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            MR6 = GetVrefRange(host, (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex]);
#ifdef LRDIMM_SUPPORT
            if (IsLrdimmPresent(host, socket, ch, dimm)) {
              MR6 = (UINT8)(*rankList)[rank].lrbufTxVref[0];
            }
#endif
            MR6 |= (((GetTccd_L(host, socket, ch) - 4) & 7) << 10);

            //
            // Note that NVDIMM microcontroller will leave DRAM in DLL-off state, so the
            // DLL-off to DLL-on flow must be used to write MR1, MR0 prior to other MRS
            //
            WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);
            WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);
            //
            // Follow MRS programming order in the DDR4 SDRAM spec
            //
            WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR3, BANK3);
            WriteMR6(host, socket, ch, dimm, rank, MR6, BANK6);
            WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
            WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR4, BANK4);
            WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR2, BANK2);
          } // rank loop
        } // if nvDimmType
      } // dimm loop

      DoZQ(host, socket, ch, ZQ_LONG);

      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        if ((*dimmNvList)[dimm].nvDimmType) {

          for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            //
            // Only restore Tx Vref on rank 0 for LRDIMMs
            //
            if ((rank > 0) & (IsLrdimmPresent(host, socket, ch, dimm))) continue;
            RestoreOffset(host, socket, ch, dimm, rank, 0, DdrLevel, TxVref);
          } // rank loop
        } // if nvDimmType
      } // dimm loop
    } // ch loop
  } // if NvDimmResume
#endif // MEM_NVDIMM_EN
} // ExitSelfRefresh

/**
   Determine the min and max values of roundtrip per channel for ddr4 dimms in qclks

   @param host  - Pointer to sysHost
   @param socket  - Socket Id

   @retval N/A
**/
void GetMinMaxRoundtrip(
                        PSYSHOST  host,
                        UINT8     socket
                        )
{
  UINT8           ch;
  UINT8           dimm;
  UINT8           rank;
  UINT8           minRoundtrip = 0xFF;
  UINT8           maxRoundtrip = 0x0;
  UINT8           tempRoundtrip;
  BOOLEAN         dimmPresentCh = FALSE;

  struct channelNvram           (*channelNvList)[MAX_CH];
  struct dimmNvram              (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList  = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        // only ddr4 dimms
        if (!((*dimmNvList)[dimm].aepDimmPresent)) {
          // if you come here there is atleast 1 ddr4 dimm present per channel
          dimmPresentCh = TRUE;

          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            tempRoundtrip = GetRoundTrip (host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank));

            if (tempRoundtrip < minRoundtrip) {
              minRoundtrip = tempRoundtrip;
            }

            if (tempRoundtrip > maxRoundtrip) {
              maxRoundtrip = tempRoundtrip;
            }
          } // rank
        } // ddr4 dimm present
    } // dimm

    // set the min and max values of roundtrip per channel for ddr4 dimms in qclks
    if (dimmPresentCh == TRUE) {  // atleast 1 ddr4 dimm present per channel
      (*channelNvList)[ch].minRoundTrip = minRoundtrip;
      (*channelNvList)[ch].maxRoundTrip = maxRoundtrip;

      // clear the ddr4 dimm present per channel for next channel and reset min and max values
      dimmPresentCh = FALSE;
      minRoundtrip = 0xFF;
      maxRoundtrip = 0x0;
    }
  } // ch
} // GetMinMaxRoundtrip

/**
  Determine the slot number that needs to be assigned per channel

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param dimm  - DIMM number (0-based)
  @param data  - t_ddrt_gnt2erid (NVMDIMM)/cmpl_program_delay(ddr4)
  @retval slot/error - the slot number that is assigned per channel/error
**/
UINT8 GetSlotNumber(
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    UINT8     dimm,
                    UINT8     data
                    )
{
  UINT8                     slotNumber;
  UINT8                     slot;
  BOOLEAN                   found = FALSE;
  UINT8                     error = 0xFF;

  struct  channelNvram      (*channelNvList)[MAX_CH];
  struct  dimmNvram         (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList  = GetDimmNvList(host, socket, ch);

  // loop through all slots( should be value between 0 and 5, based on register def) to find the slot number that matches
  for (slot = 0; slot < 6; slot++) {
    if ((*dimmNvList)[dimm].aepDimmPresent) { // NVMDIMM dimm
      // (ddrt_timing.du_slot_number + t_ddrt_dimmN.basic_timing_t_sxp_gnt2erid + 6) mod 6 = (2 * current channel number), where N = 0 or 1
      slotNumber = (slot + data + 6) % 6;
      if (slotNumber == (2 * (ch%3))) {
        found = TRUE;
        break;
      }
    } else if (!((*dimmNvList)[dimm].aepDimmPresent)) { // ddr4 dimm
      // (slot + max of roundtrip value of current channel in qclks - cmpl_to_data_delay.cmpl_program_delay - 1) mod 6 = (2 * current channel number)
      // (bl_egress_credit.du_slot_number + (max of roundtripM.rt_rankN - 1)/2 - cmpl_to_data_delay.cmpl_program_delay - 1) mod 6 = (2 * current channel number), where M = 0 or 1, and N = 0 to 7
      slotNumber = (slot + (((*channelNvList)[ch].maxRoundTrip - 1)/2) - data - 1) % 6;
      if (slotNumber == (2 * (ch%3))) {
        found = TRUE;
        break;
      }
    }
  }

  if (found == TRUE) {
    return slot;
  } else {
    return error;
  }
} // GetSlotNumber
/**

  Initialize channel specific memory controller registers after training and before normal mode

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
LateConfig (
           PSYSHOST host
           )
{
  UINT8                                     socket;
  UINT8                                     mcId;
  UINT8                                     ch;
  UINT8                                     chNum;
  UINT8                                     dimm;
  UINT8                                     rank;
  UINT8                                     tempData;
  UINT8                                     slotNumber;
  UINT8                                     logRank;
  UINT8                                     extraCmplDelayRank;
  UINT8                                     diffCmplProgramDelay;
  UINT8                                     mbStatus = 0;
  UINT8                                     setPowerMbStatus = 0;
  UINT32                                    status;
  UINT32                                    ddrtchnlpopulation;
  UINT32                                    ddr4chnlpopulation;

  struct channelNvram                       (*channelNvList)[MAX_CH];
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  struct rankDevice                         (*rankStruct)[MAX_RANK_DIMM];
  struct powerManagement                    powerManagement;
  IDLETIME_MCDDC_CTL_STRUCT                 IdleTime;
  MCMTR_MC_MAIN_STRUCT                      mcMtr;
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT          miscCKECtl;
  CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT          cpgcDdrtMiscCtl;
  T_DDRT_PWRUP_MC_2LM_STRUCT                ddrtPwUpMrc2Lm;
  DDRT_ERROR_MC_2LM_STRUCT     errSignals;
  CMPL_TO_DATA_DELAY_MCDDC_DP_STRUCT        cmplDataDelay;
  DDRT_TIMING_MC_2LM_STRUCT                 ddrtTiming;
  BL_EGRESS_CREDIT_MCDDC_CTL_STRUCT         blEgressCredit;
  BLEGR_CRDT_CTL_MCDDC_CTL_STRUCT           blEgrCrdtCtl;
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimmBasicTiming;
  CPGC_INORDER_MCDDC_CTL_STRUCT             cpgcInOrder;
  DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_STRUCT ddrCRCtlCompOffsetCmdN;
  DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_STRUCT ddrCRCtlCompOffsetCmdS;
  DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_STRUCT  ddrCRCtlCompOffsetCke;
  DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_STRUCT  ddrCRCtlCompOffsetCtl;
  MEMORY_M2MEM_MAIN_STRUCT                     m2mMem;
  TIMEOUT_M2MEM_MAIN_STRUCT                    m2mTimeout;
  DDRT_MAJOR_MODE_THRESHOLD3_MC_2LM_STRUCT     majorModeThreshold3;
  DDRT_DATAPATH_DELAY2_MCDDC_DP_STRUCT      ddrtDataPathDelay2;
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimm0BasicTiming;
  DDRT_CLK_GATING_MC_2LM_STRUCT             ddrtClkGating;
  DDRT_ERR_LOG_CTL_MC_2LM_STRUCT            ddrtErrorLogCtrl;
  MCASCCONTROL_MC_MAIN_STRUCT               mcasControl;
  MCGLBRSPCNTL_MC_MAIN_STRUCT               mcGlbrsptCntl;
  RSP_FUNC_MCCTRL_ERR_INJ_MCDDC_CTL_STRUCT  rspFuncMcctrlErrInj;
  DDRT_DATAPATH_DELAY_MCDDC_DP_STRUCT       ddrtDatapathDelay;
  DDRT_TIMING_MCDDC_CTL_STRUCT              ddrtCtlTiming;
  T_DDRT_PD_MC_2LM_STRUCT                   ddrtPd;
  T_DDRT_PD2_MC_2LM_STRUCT                  ddrtPd2;
  UINT32                                    TimeOutLockVal;
  ddrtDimmBasicTiming.Data = 0x0;

  socket = host->var.mem.currentSocket;

  OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, STS_RMT, (UINT16)((SUB_LATE_CONFIG << 8)|socket)));
  //
  // Return if this socket is disabled
  //
  if (host->nvram.mem.socket[socket].enabled == 0) return SUCCESS;

  if (host->nvram.mem.socket[socket].maxDimmPop == 0) return SUCCESS;

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
    // 4928666  imcX_m2mem_memory should be set based on populated channels.
    m2mMem.Data = MemReadPciCfgMC (host, socket, mcId, MEMORY_M2MEM_MAIN_REG);
    m2mMem.Bits.ddr4chnlpopulation = ddr4chnlpopulation = 0;
    m2mMem.Bits.ddrtchnlpopulation = ddrtchnlpopulation = 0;
    channelNvList = GetChannelNvList(host, socket);
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      chNum = mcId * MAX_MC_CH + ch;    // Change channel format from 0-2 to 0-5
      if ((*channelNvList)[chNum].enabled == 0) continue;
      if ((*channelNvList)[chNum].ddrtEnabled)
        ddrtchnlpopulation |= (1 << ch);
      else
        ddr4chnlpopulation |= (1 << ch);
    }
    m2mMem.Bits.ddrtchnlpopulation = ddrtchnlpopulation;
    m2mMem.Bits.ddr4chnlpopulation = ddr4chnlpopulation;
    MemWritePciCfgMC (host, socket, mcId, MEMORY_M2MEM_MAIN_REG, m2mMem.Data);

    m2mTimeout.Data = MemReadPciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
    TimeOutLockVal = m2mTimeout.Bits.timeoutlock;
    m2mTimeout.Bits.timeoutlock = 0;
    MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);

    // 4928667  imcX_m2mem_timeout should be enabled
    m2mTimeout.Data = MemReadPciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
    m2mTimeout.Bits.timeouten = 1;
    //m2mTimeout.Bits.timeoutbase = 3;  // Leave it to default for now
    //m2mTimeout.Bits.timeoutmult = 2;  // Leave it to default for now
    MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);

    m2mTimeout.Data = MemReadPciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
    m2mTimeout.Bits.timeoutlock = TimeOutLockVal;
    MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);

  }

  channelNvList = GetChannelNvList(host, socket);

  //
  // NVMCTLR hook after DDR training
  //
  if (!(host->var.common.bootMode == NormalBoot && host->var.mem.subBootMode == WarmBootFast)) {
    LateFnvConfigAccess (host, socket);
  }
  ProgramErid2DataValid(host, socket);
  //Set delay from wpq ERID to underfill indication on read return / per Ch.
  //Done after training to get Correct RTL
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      GetWpqRidtoRt ( host, socket, ch, dimm);
    } // dimm loop

  } // ch loop

  //
  // HSD5330883: Need to program imcX_cY_ddrt_datapath_delay2.rrd_gnt2erid = imcX_cY_t_ddrt_dimmZ_basic_timing.t_ddrt_gnt2erid + 1
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;

    ddrtPd.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_PD_MC_2LM_REG);
    ddrtPd.Bits.t_ddrt_rdpden = 2;
    ddrtPd.Bits.t_ddrt_wrpden = Saturate(31, GettWR(host, socket) + 4 + 1);
    MemWritePciCfgEp (host, socket, ch, T_DDRT_PD_MC_2LM_REG, ddrtPd.Data);

    //
    // t_ddrt_gntpden = (DDRT roundtrip / 2) + 16
    //
    ddrtPd2.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_PD2_MC_2LM_REG);
    ddrtPd2.Bits.t_ddrt_gntpden = Saturate(31, (GetRoundTrip (host, socket, ch, GetLogicalRank(host, socket, ch, 0, 0)) / 2) + 16);
    MemWritePciCfgEp (host, socket, ch, T_DDRT_PD2_MC_2LM_REG, ddrtPd2.Data);
    //
    // HSD5370588: CLONE from skylake_server: [CR B0] Powerdown exit to command timing register programmed incorrectly
    // This register is currently programmed to 8.  BIOS needs to program it to 15 on all channels with a DDRT DIMM installed.
    //
    ddrtPwUpMrc2Lm.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_PWRUP_MC_2LM_REG);
    ddrtPwUpMrc2Lm.Bits.t_ddrt_xp = 15;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_PWRUP_MC_2LM_REG, ddrtPwUpMrc2Lm.Data);

    ddrtDimm0BasicTiming.Data = 0;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      if (dimm == 0) {
        ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG);
      } else {
        ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG);
      }
    }

    ddrtDataPathDelay2.Data = MemReadPciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG);
    ddrtDataPathDelay2.Bits.rrd_gnt2erid = ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid + 1;
    MemWritePciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG, ddrtDataPathDelay2.Data);
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "t_ddrt_gnt2erid: %d rrd_gnt2erid: %d\n", ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid, ddrtDataPathDelay2.Bits.rrd_gnt2erid));
  } //Ch

  //
  // HSD5332462: DDRT_TIMING.ddrtrd_to_ddr4rd value change
  //             ddrtrd_to_ddr4rd = (rrd_gnt2erid + en_rd_ddrt_mode + delay.rd + 4) - (rt_rank_0 / 2)
  //
  GetMinMaxRoundtrip(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
    if ((CheckSteppingLessThan (host, CPU_SKX, B1_REV_SKX)) && ((*channelNvList)[ch].maxDimm == 2)) {
      dimmNvList = GetDimmNvList(host, socket, ch);
      ddrtDataPathDelay2.Data = MemReadPciCfgEp(host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG);
      ddrtDataPathDelay2.Bits.en_rd_ddrt_mode = 0;
      MemWritePciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG, ddrtDataPathDelay2.Data);
      ddrtDatapathDelay.Data = MemReadPciCfgEp(host, socket, ch, DDRT_DATAPATH_DELAY_MCDDC_DP_REG);
      ddrtCtlTiming.Data = MemReadPciCfgEp(host, socket, ch, DDRT_TIMING_MCDDC_CTL_REG);
      ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd = ddrtDataPathDelay2.Bits.rrd_gnt2erid + ddrtDataPathDelay2.Bits.en_rd_ddrt_mode + ddrtDatapathDelay.Bits.rd + 4;
      if (ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd > ((UINT32)(*channelNvList)[ch].minRoundTrip / 2)) {
        ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd -= ((UINT32)(*channelNvList)[ch].minRoundTrip / 2);
      } else {
        ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd = 0;
      }
      //
      // 5370152: CLONE SKX Sighting: (CR) TOR TO/ECC w/ Retries on No Data Packets
      //

      ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd += 8;

      MemWritePciCfgEp(host, socket, ch, DDRT_TIMING_MCDDC_CTL_REG, ddrtCtlTiming.Data);
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "rrd_gnt2erid: %2d, en_rd_ddrt_mode: %2d, delay.rd: %2d, rt_rank0: %2d, ddrtrd_to_ddr4rd: %2d\n",
        ddrtDataPathDelay2.Bits.rrd_gnt2erid,
        ddrtDataPathDelay2.Bits.en_rd_ddrt_mode,
        ddrtDatapathDelay.Bits.rd,
        (*channelNvList)[ch].minRoundTrip,
        ddrtCtlTiming.Bits.ddrtrd_to_ddr4rd));
    }
  } //Ch

  //
  // HSD5330977: Request to enable the DDRT DFX clk so as to get logging information
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
    ddrtClkGating.Data = MemReadPciCfgEp (host, socket, ch, DDRT_CLK_GATING_MC_2LM_REG);
    ddrtErrorLogCtrl.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERR_LOG_CTL_MC_2LM_REG);
    ddrtClkGating.Bits.en_ddrt_dfd_clk =  1;
    ddrtErrorLogCtrl.Bits.mask =  0x20;
    MemWritePciCfgEp (host, socket, ch, DDRT_CLK_GATING_MC_2LM_REG, ddrtClkGating.Data);
    MemWritePciCfgEp (host, socket, ch, DDRT_ERR_LOG_CTL_MC_2LM_REG, ddrtErrorLogCtrl.Data);
  } //Ch
#if defined COSIM_EN || !defined YAM_ENV
  // Temporary workaround for CR PO
  if (host->nvram.mem.aepDimmPresent == 0) {
    //
    // Wait for NVMCTLR to indicate that Media interface is ready
    // SKX TODO: Remove Cold Boot Check after Simics Bug has been fixed
    if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {
      FnvPollingBootStatusRegister (host, socket, FNV_MEDIA_DONE);
    }

    //
    // Poll for Credit Ready
    //
    if ((host->var.common.bootMode == NormalBoot) && ((host->var.mem.subBootMode == WarmBoot) || (host->var.mem.subBootMode == WarmBootFast))) {
      FnvPollingBootStatusRegister (host, socket, FNV_CREDIT_READY);
    }
  }
#endif

  if( ( host->setup.mem.setTDPDIMMPower == POWER_10W || host->setup.mem.setTDPDIMMPower == POWER_12W ||
        host->setup.mem.setTDPDIMMPower == POWER_15W || host->setup.mem.setTDPDIMMPower == POWER_18W ) ) {

    host->var.mem.setTDPDIMMPower = host->setup.mem.setTDPDIMMPower;
  } else {
       MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\nWarning!!! Invalid option value for Set TDP DIMM power limit.\n"));
       host->var.mem.setTDPDIMMPower = POWER_DEFAULT;
  }

  channelNvList = &host->nvram.mem.socket[socket].channelList;
  for (ch = 0; ch < MAX_CH; ch++) {
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
      if (!((*dimmNvList)[dimm].aepDimmPresent)) continue;
      powerManagement.enable = 1;
      powerManagement.TDP = 0;
      powerManagement.peakPowerBudget = 0;
      powerManagement.averagePowerBudget = 0;
      // Temporary workaround for CR PO
      if (host->nvram.mem.aepDimmPresent == 0) {

        if (host->var.mem.setTDPDIMMPower != (*dimmNvList)[dimm].TDP) {

          status = GetPowerManagementPolicy (host,socket,ch,dimm, &powerManagement, &mbStatus);

          if (status == SUCCESS) {

            if (powerManagement.enable == 0) {
              powerManagement.enable = 1;
            }
            powerManagement.TDP = host->var.mem.setTDPDIMMPower;
            status = SetPowerManagementPolicy(host,socket,ch,dimm, &powerManagement, &setPowerMbStatus);

            if(status != SUCCESS) {
              MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
              //log data format:
              //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
              //Byte 1 - FW MB Opcode,Byte 2 FW MB Sub-Opcode,Byte 3 - FW MB Status Code
              EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
              DisableChannel(host, socket, ch);
            }

          } else {
            MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
            //log data format:
            //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
            //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
            EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
            DisableChannel(host, socket, ch);
          }
        }
      }
    }
  }


  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    ddrCRCtlCompOffsetCmdN.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCmdS.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCke.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCmdN.Bits.iodirectionvalid = 1;
    ddrCRCtlCompOffsetCmdS.Bits.iodirectionvalid = 1;
    ddrCRCtlCompOffsetCke.Bits.iodirectionvalid = 1;
    ddrCRCtlCompOffsetCtl.Bits.iodirectionvalid = 1;

    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCmdN.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCmdS.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCke.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCtl.Data);

  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    errSignals.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG);
    if ((*channelNvList)[ch].ddrtEnabled) {
      cpgcDdrtMiscCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG);
      if ((*dimmNvList)[0].aepDimmPresent == 1)  errSignals.Bits.ignore_ddrt_err0 = 0;
      if ((*dimmNvList)[1].aepDimmPresent == 1)  errSignals.Bits.ignore_ddrt_err1 = 0;
      errSignals.Bits.ignore_ddr4_error = 0;
      errSignals.Bits.ignore_erid_parity_error = 0;
      errSignals.Bits.ignore_ddrt_ecc_error = 0;
      cpgcDdrtMiscCtl.Bits.multi_credit_on = 0;
      MemWritePciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG, cpgcDdrtMiscCtl.Data);

      majorModeThreshold3.Data = MemReadPciCfgEp (host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD3_MC_2LM_REG);
      majorModeThreshold3.Bits.ddrt_critical_starve = 0x03;
      if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
        majorModeThreshold3.Bits.ddr4_critical_starve = 0x03;
      }
      MemWritePciCfgEp(host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD3_MC_2LM_REG, majorModeThreshold3.Data);
    } else {
      errSignals.Bits.ignore_erid_parity_error = 1;
      errSignals.Bits.ignore_ddr4_error = 1;
      errSignals.Bits.ignore_ddrt_ecc_error = 1;
      errSignals.Bits.ignore_ddrt_err0 = 1;
      errSignals.Bits.ignore_ddrt_err1 = 1;
    }
    MemWritePciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG, errSignals.Data);
  }

  //
  // These registers need to be cleared before normal mode
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    cpgcInOrder.Data = MemReadPciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG);
    cpgcInOrder.Bits.wpq_inorder_en = 0;
    MemWritePciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG, cpgcInOrder.Data);
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //
    // Enable adaptive page policy  (We set it to disabled in programTimings)
    //
    if ((host->nvram.mem.RASmodeEx & ADDDC_EN) != ADDDC_EN) {

      if (host->setup.mem.options & ADAPTIVE_PAGE_EN) {
        IdleTime.Data = MemReadPciCfgEp(host, socket, ch, IDLETIME_MCDDC_CTL_REG);
        IdleTime.Bits.adapt_pg_clse = 1;
        MemWritePciCfgEp(host, socket, ch, IDLETIME_MCDDC_CTL_REG, IdleTime.Data);
      }
    } else {
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n Force Closed Page Mode due to ADDDC Workaround"));
    }
  } // ch loop

#ifdef MEM_NVDIMM_EN
  if (!(host->var.mem.subBootMode == NvDimmResume))
  {
#endif
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if ((host->nvram.mem.dimmTypePresent == RDIMM) && ((*dimmNvList)[dimm].aepDimmPresent == 0)) {
          //
          // Disable register SMBUS per BIOS sighting #4930001
          //
          WriteRC(host, socket, ch, dimm, 0, 0x1, RDIMM_RC2x);
        }
#ifdef  LRDIMM_SUPPORT
        if (IsLrdimmPresent(host, socket, ch, dimm)) {
          WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
          (*dimmNvList)[dimm].rcLrFunc = 0;
        } //lrDimmPresent
#endif // LRDIMM_SUPPORT
      } //dimm
    } // ch loop
    // Disable any further RCW's
    host->nvram.mem.socket[socket].wa = host->nvram.mem.socket[socket].wa | WA_NO_MORE_RCW;
#ifdef MEM_NVDIMM_EN
  }
#endif

  //
  // Configure C/A Parity
  //
  SetupCapWrCrcErrorFlow(host, socket);
#ifdef MEM_NVDIMM_EN
  if (!(host->var.mem.subBootMode == NvDimmResume)) {
#endif

  if ((host->setup.mem.options & CA_PARITY_EN) && (host->nvram.mem.dimmTypePresent == RDIMM)) {
    EnableParityChecking(host, socket);
  } else {
    DisableParityChecking(host, socket);
  }
#ifdef MEM_NVDIMM_EN
  }
#endif


  //
  // If we're coming out of S3, enable CKE_ON to bring DIMMs out
  // of self-refresh before enabling refresh and turning on normal
  // mode.
  //
#ifdef MEM_NVDIMM_EN
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast || host->var.mem.subBootMode == ColdBootFast || host->var.mem.subBootMode == NvDimmResume) {
#else
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast || host->var.mem.subBootMode == ColdBootFast) {
#endif

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      MemWritePciCfgEp (host, socket, ch, TCMR0SHADOW_MCDDC_CTL_REG, (*channelNvList)[ch].TCMr0Shadow);
      MemWritePciCfgEp (host, socket, ch, TCMR2SHADOW_MCDDC_CTL_REG, (*channelNvList)[ch].TCMr2Shadow);
      MemWritePciCfgEp (host, socket, ch, TCMR4SHADOW_MCDDC_CTL_REG, (*channelNvList)[ch].TCMr4Shadow);
      MemWritePciCfgEp (host, socket, ch, TCMR5SHADOW_MCDDC_CTL_REG, (*channelNvList)[ch].TCMr5Shadow);
    } // ch loop
  }

  if ((host->var.common.bootMode == NormalBoot && host->var.mem.subBootMode == ColdBootFast)) {
    IO_Reset(host, socket);
  }

  if (((host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) == STAT_VIRT_LOCKSTEP)) {
    // HSD 5331451
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n Set on-the-fly-burst per rank\n", socket));

    channelNvList = &host->nvram.mem.socket[socket].channelList;

    if (host->var.common.bootMode == NormalBoot) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rankStruct  = &host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct;

          for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            (*rankStruct)[rank].MR0 = (*rankStruct)[rank].MR0 | MR0_ONTHEFLY;
            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);

            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, " MR0=on-the-fly-burst\n", socket));

          } // rank loop
        } // DIMM loop
      } // ch loop
    } // NormalBoot
  } // Is sVLS enabled

  if( (host->nvram.mem.RASmodeEx & ADDDC_EN) == ADDDC_EN) {
    channelNvList = &host->nvram.mem.socket[socket].channelList;


    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      if (((*channelNvList)[ch].features & X4_PRESENT) != X4_PRESENT) {
         MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "No X4 dimms in channel\n"));
         continue;
      }

      mcMtr.Data = MemReadPciCfgMC(host, socket, (ch / MAX_MC_CH), MCMTR_MC_MAIN_REG);
      mcMtr.Bits.close_pg = 1;
      MemWritePciCfgMC(host, socket, (ch / MAX_MC_CH), MCMTR_MC_MAIN_REG, mcMtr.Data);

      if (host->var.common.bootMode == NormalBoot) {
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          rankStruct  = &host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct;

          for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            (*rankStruct)[rank].MR0 = (*rankStruct)[rank].MR0 | MR0_ONTHEFLY;
            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR0, BANK0);

            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, " MR0=on-the-fly-burst\n", socket));
          } // rank loop
        } // dimm loop
      } // if (host->var.common.bootMode == NormalBoot)
    } // ch loop
  } // if LS enabled in setup
  // HSD 5331451 - end

  //
  // enable wrcrc, this needs to be done before refresh is enabled
  //
  if (!((host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast))) {
    CheckAndProgramDqSwizzleAep(host, socket);
  }

  if ((host->setup.mem.optionsExt & WR_CRC) && (!(host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP))) {
    EnableWrCRC(host, socket);
  }

#ifdef MEM_NVDIMM_EN
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast || host->var.mem.subBootMode == NvDimmResume) {
#else
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast) {
#endif
    //
    // Exit self refresh
    //
    ExitSR(host, socket);
    if ((host->var.common.bootMode == NormalBoot && host->var.mem.subBootMode == WarmBootFast)) {
      FnvPollingBootStatusRegister(host, socket, FNV_CSR_UNLOCK);
    }
    ddrtS3Resume(host, socket);

#ifdef MEM_NVDIMM_EN
    if (host->var.mem.subBootMode == NvDimmResume)  {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          if ((host->nvram.mem.dimmTypePresent == RDIMM) && ((*dimmNvList)[dimm].aepDimmPresent == 0)) {
            //
            // Disable register SMBUS per BIOS sighting #4930001
            //
            WriteRC(host, socket, ch, dimm, 0, 0x1, RDIMM_RC2x);
          }
#ifdef  LRDIMM_SUPPORT
          if (IsLrdimmPresent(host, socket, ch, dimm)) {
            WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
            (*dimmNvList)[dimm].rcLrFunc = 0;
          } //lrDimmPresent
#endif // LRDIMM_SUPPORT
        } //dimm
      } // ch loop
      // Disable any further RCW's
      host->nvram.mem.socket[socket].wa = host->nvram.mem.socket[socket].wa | WA_NO_MORE_RCW;
      if ((host->setup.mem.options & CA_PARITY_EN) && (host->nvram.mem.dimmTypePresent == RDIMM)) {
        EnableParityChecking(host, socket);
      }
      else {
        DisableParityChecking(host, socket);
      }
    }
#endif
  } // if (S3Resume)
  else {
    //
    // Assert CKE on each channel and enable refresh
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
      miscCKECtl.Bits.refresh_enable = 1;
      MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
    }
    host->var.mem.socket[socket].hostRefreshStatus = 1;
  }

  //
  // program slot assignment values for Early Completion
  //
  GetMinMaxRoundtrip(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    cmplDataDelay.Data = MemReadPciCfgEp (host, socket, ch, CMPL_TO_DATA_DELAY_MCDDC_DP_REG);
    ddrtTiming.Data = MemReadPciCfgEp (host, socket, ch, DDRT_TIMING_MC_2LM_REG);
    blEgressCredit.Data = MemReadPciCfgEp (host, socket, ch, BL_EGRESS_CREDIT_MCDDC_CTL_REG);
    blEgrCrdtCtl.Data = MemReadPciCfgEp (host, socket, ch, BLEGR_CRDT_CTL_MCDDC_CTL_REG);

    // Calculate the difference between min and max value(qclks) of roundtrip delay of all DDR4 ranks in dclks and set cmpl_program_delay
    diffCmplProgramDelay = ( (*channelNvList)[ch].maxRoundTrip - (*channelNvList)[ch].minRoundTrip )/2;

    // E.g., if the difference is 2 dclks, then cmpl_to_data_delay. cmpl_program_delay = 2 + 1(base value) = 3 and the completion to data delay is 6 + 3 = 9
    // Note, max difference allowed can be 3 dclk, but cmpl_program_delay has a max value of 4 (3 + 1(base value) = 4). Hopefully we don't hit the real max of 3.
    cmplDataDelay.Bits.cmpl_program_delay = diffCmplProgramDelay + 0x5;

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
         "Difference between min and max value(qclks) of roundtrip delay of all DDR4 ranks in dclks= %d \n", diffCmplProgramDelay));
    // this warning is needed
    if (diffCmplProgramDelay >= 3) {
      OutputWarning (host, WARN_RT_DIFF_EXCEED, WARN_RT_DIFF_MINOR_EXCEED, socket, ch, NO_DIMM, 0xFF);
    }

    // Set mcmtr.enable_slot_use = 1
    mcMtr.Data = MemReadPciCfgMC (host, socket, GetMCID(host, socket, ch), MCMTR_MC_MAIN_REG);
    mcMtr.Bits.enable_slot_use = 1;
    MemWritePciCfgMC (host, socket, GetMCID(host, socket, ch), MCMTR_MC_MAIN_REG, mcMtr.Data);

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
         "mcmtr.enable_slot_use is set \n"));

    if (diffCmplProgramDelay > 0) {
      // Set cmpl_to_data_delay.extra_cmpl_delay_en = 1
      cmplDataDelay.Bits.extra_cmpl_delay_en = 1;

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
           "extra_cmpl_delay_en is set \n"));
    }

    dimmNvList  = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if (!((*dimmNvList)[dimm].aepDimmPresent)) {  // ddr4 dimm present
        // Program bl_egress_credit.du_slot_number (DDR4) according to the formula
        // i. (bl_egress_credit.du_slot_number + RdCAS to early completion delay) mod 6 = (2 * current channel number)
        // ii. "RdCAS to early completion delay" =  "Roundtrip latency" (in dclk) - (cmpl_to_data_delay.cmpl_program_delay + 5)
        // iii. "Roundtrip latency" = (max of roundtripM.rt_rankN - 1)/2 + 4, where M = 0, 1, and N = 0 to 7
        // iv.  (bl_egress_credit.du_slot_number + (max of roundtripM.rt_rankN - 1)/2 + 4 - (cmpl_to_data_delay.cmpl_program_delay + 5)) mod 6 = (2 * current channel number)
        tempData = (UINT8)cmplDataDelay.Bits.cmpl_program_delay;
        slotNumber = GetSlotNumber(host, socket, ch, dimm, tempData);
        if (slotNumber != 0xFF) {
          blEgressCredit.Bits.du_slot_number = slotNumber;
        } // else throw an error
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
             "slotnumber: %d\n", slotNumber));
      } else if ((*dimmNvList)[dimm].aepDimmPresent) { // NVMDIMM present
        // Program ddrt_timing.du_slot_number (NVMDIMM) according to the formula
        // i. (ddrt_timing.du_slot_number + GNT to early completion delay) mod 6 = (2 * current channel number)
        // ii.    "GNT to early completion delay" = "GNT to ERID delay" + 6
        // iii.   "GNT to ERID delay" = t_ddrt_dimm0.basic_timing_t_sxp_gnt2erid or t_ddrt_dimm1_basic_timing.t_sxp_gnt2erid (depending on which NVM DIMM slot is populated)
        // iv.    the value of 6 is ERID to early completion delay, which is a fixed number due to hardware implementation
        ddrtDimmBasicTiming.Data = MemReadPciCfgEp (host, socket, ch, (T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG + (dimm * 4)));
        tempData = (UINT8)ddrtDimmBasicTiming.Bits.t_ddrt_gnt2erid;
        slotNumber = GetSlotNumber(host, socket, ch, dimm, tempData);
        if (slotNumber != 0xFF) {
          ddrtTiming.Bits.du_slot_number = slotNumber;
        }  // else throw an error
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
             "slotnumber: %d\n", slotNumber));
      }

      // if cmpl_to_data_delay.extra_cmpl_delay_en is set to 1 . Only DDR4 ranks are needed for this set. NVMDIMM ranks are irrelavent here.
      if ((diffCmplProgramDelay > 0) && (!((*dimmNvList)[dimm].aepDimmPresent))) {
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          // Get the logical rank #
          logRank = GetLogicalRank(host, socket, ch, dimm, rank);

          // Program cmpl_to_data_delay.extra_cmpl_delay_rankN = (max of roundtripX.rt_rankY - roundtripM.rt_rankN) / 2, where X = 0, 1, Y = 0 to 7, M = 0 or 1, and N = 0 to 7
          extraCmplDelayRank = ((*channelNvList)[ch].maxRoundTrip - GetRoundTrip (host, socket, ch, logRank)) / 2;

          if (logRank == 0) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank0 = (UINT8)extraCmplDelayRank;
          } else if (logRank == 1) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank1 = (UINT8)extraCmplDelayRank;
          } else if (logRank == 2) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank2 = (UINT8)extraCmplDelayRank;
          } else if (logRank == 3) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank3 = (UINT8)extraCmplDelayRank;
          } else if (logRank == 4) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank4 = (UINT8)extraCmplDelayRank;
          } else if (logRank == 5) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank5 = (UINT8)extraCmplDelayRank;
          } else if (logRank == 6) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank6 = (UINT8)extraCmplDelayRank;
          } else if (logRank == 7) {
            cmplDataDelay.Bits.extra_cmpl_delay_rank7 = (UINT8)extraCmplDelayRank;
          }
        } // rank loop
      }
    } // dimm loop

    //
    // 4929211: Cloned From SKX Si Bug Eco: [NVMDIMM EPO] SKX MC Err Flow fails when NVMCTLR asserts NVMDIMM ERR# pin
    //
    blEgrCrdtCtl.Bits.enable_tgr_cr = 1;

    MemWritePciCfgEp (host, socket, ch, BLEGR_CRDT_CTL_MCDDC_CTL_REG, blEgrCrdtCtl.Data);
    MemWritePciCfgEp (host, socket, ch, CMPL_TO_DATA_DELAY_MCDDC_DP_REG, cmplDataDelay.Data);
    MemWritePciCfgEp (host, socket, ch, DDRT_TIMING_MC_2LM_REG, ddrtTiming.Data);
    MemWritePciCfgEp (host, socket, ch, BL_EGRESS_CREDIT_MCDDC_CTL_REG, blEgressCredit.Data);
  } // ch loop

  //
  // HSD 5332732 SC TOR_TO and out of credits
  //
  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

      MemWritePciCfgMC (host, socket, mcId, MCASC0LDVALHI_MC_MAIN_REG, 0x500);
      MemWritePciCfgMC (host, socket, mcId, MCASC0LDVALLO_MC_MAIN_REG, 0x10000);

      mcasControl.Data = (UINT16)MemReadPciCfgMC (host, socket, mcId, MCASCCONTROL_MC_MAIN_REG);
      mcasControl.Bits.asc0en = 1;
      MemWritePciCfgMC (host, socket, mcId, MCASCCONTROL_MC_MAIN_REG, mcasControl.Data);

      mcGlbrsptCntl.Data = MemReadPciCfgMC (host, socket, mcId, MCGLBRSPCNTL_MC_MAIN_REG);
      mcGlbrsptCntl.Bits.glbrsp0starttrigsel = 0x2;
      mcGlbrsptCntl.Bits.glbrsp0stopsel = 0x7;
      MemWritePciCfgMC (host, socket, mcId, MCGLBRSPCNTL_MC_MAIN_REG, mcGlbrsptCntl.Data);
      for (ch = 0; ch < MAX_MC_CH; ch++) {
        chNum = mcId * MAX_MC_CH + ch;    // Change channel format from 0-2 to 0-5
        rspFuncMcctrlErrInj.Data = MemReadPciCfgEp (host, socket, chNum, RSP_FUNC_MCCTRL_ERR_INJ_MCDDC_CTL_REG);
        rspFuncMcctrlErrInj.Bits.wr_crdt_thr_sel = 1;
        MemWritePciCfgEp (host, socket, chNum, RSP_FUNC_MCCTRL_ERR_INJ_MCDDC_CTL_REG, rspFuncMcctrlErrInj.Data);
      }
    }
  }

  return SUCCESS;
} // LateConfig


#ifdef SSA_FLAG
UINT32
SSAInit (
  PSYSHOST host
  )
/*++

  Initialize the SSA API

  @param host  - Pointer to sysHost

  @retval SUCCESS

--*/
{
#ifdef BDAT_SUPPORT
  UINT8     ResultIdx;

  //Initializing the BSSA BDAT variables - BiosSaveToBdat()
  host->var.mem.bssaBdatSize      = 0;
#endif //BDAT_SUPPORT

  if ((~host->memFlowsExt & MF_EXT_BIOS_SSA_RMT_EN) && (host->setup.mem.enableBiosSsaRMT)) return SUCCESS;

#ifdef BDAT_SUPPORT
  for (ResultIdx = 0; ResultIdx < MAX_NUMBER_SSA_BIOS_RESULTS; ResultIdx++) {
    host->var.mem.bssaNumberHobs[ResultIdx] = 0;
  }

  host->var.mem.bssaNumberDistinctGuids = 0;
#endif //BDAT_SUPPORT

  // Enabling the BSSA module and loader is dependent on:
  // 1. The Oem Hook returning TRUE. OEMs should add their own jumper detect function.
  // 2. Loader setup knob being enabled.
  // For 'Stitched SSA': Depends just on it's setup knob being enabled.
  // *SSA_OEM_HOOK* defined for only the procmeminit library class

#ifdef SSA_OEM_HOOK
  if (((OemDetectPhysicalPresenceSSA(host) == TRUE) && (host->setup.mem.enableBiosSsaLoader)) || (host->setup.mem.enableBiosSsaRMT)) {
    SsaBiosInitialize (host);                        //Callng InstallPpi, Initializes the API
  }
#endif //SSA_OEM_HOOK

#ifdef SSA_LOADER_FLAG
#ifdef  SSA_OEM_HOOK
  if ((OemDetectPhysicalPresenceSSA(host) == TRUE) && (host->setup.mem.enableBiosSsaLoader)) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Calling the BIOS SSA API\n"));

    OutputCheckpoint (host, SSA_API_INIT, SSA_API_INIT_EV_LOADER, 0);  // Issue a SSA check point to invoke the EV loader
  }
#endif //SSA_OEM_HOOK
#endif //SSA_LOADER_FLAG

  return SUCCESS;
} //SSAInit
#endif  //SSA_FLAG

/**

  Enables CMD/ADDR Parity

  @param host    - Pointer to sysHost
  @param socket  - Processor socket

  @retval N/A

**/
void
EnableCAParityRuntime (
               PSYSHOST  host,
               UINT8     socket
               )
{
  UINT8                 ch;
  UINT8                               mcId;
  UINT8                               dimm;
  UINT8                               aepDimmCountImc[MAX_IMC]={0};
  struct  channelNvram                (*channelNvList)[MAX_CH];
  struct  dimmNvram                   (*dimmNvList)[MAX_DIMM];
  SCRUBMASK_MC_MAIN_STRUCT            scrub_mask;
  DDR4_CA_CTL_MCDDC_DP_STRUCT         ddr4cactl;
  ERF_DDR4_CMD_REG2_MCDDC_CTL_STRUCT  erfDdr4CmdReg;
  MC0_DP_CHKN_BIT_MCDDC_DP_STRUCT     dp_chkn;
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT   mcSchedChknBit;
#ifdef  LRDIMM_SUPPORT
  DDR4_CBIT_PARITY_MCDDC_CTL_STRUCT   ddr4cbitParity;
#endif


  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Enabling C/A Parity\n"));

  // count NVMDIMMs
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    mcId = GetMCID(host, socket, ch);
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent){
        aepDimmCountImc[mcId]++;
      }
    } // dimm
  } // ch

  //SKX change: separate CSRs per ch
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    ddr4cactl.Data = MemReadPciCfgEp(host, socket, ch, DDR4_CA_CTL_MCDDC_DP_REG);

    //
    // Program DDR4 tPar Recovery
    //
    ddr4cactl.Bits.tpar_recov_ch0 = tPARRECOVERY[host->nvram.mem.ratioIndex];

    //
    // Program RDIMM/LRDIMM Present on channel
    //
    if (host->nvram.mem.dimmTypePresent == RDIMM) {
      ddr4cactl.Bits.erf_regpart0 = 1;
    }
    MemWritePciCfgEp(host, socket, ch, DDR4_CA_CTL_MCDDC_DP_REG, ddr4cactl.Data);

    //program Per bit inclusion control for ddr4 Cbits in command/address parity generation.
    ddr4cbitParity.Data = MemReadPciCfgEp(host, socket, ch, DDR4_CBIT_PARITY_MCDDC_CTL_REG);

    // for encoded QuadCS mode with 2 ranks per CS
    // for direct mapped QR or DR
    ddr4cbitParity.Bits.enable = 0;
    if ((*channelNvList)[ch].encodedCSMode == 2) {
      ddr4cbitParity.Bits.enable = (*channelNvList)[ch].cidBitMap;
    }
    MemWritePciCfgEp(host, socket, ch, DDR4_CBIT_PARITY_MCDDC_CTL_REG, ddr4cbitParity.Data);

    //SKX change: separate CSRs per ch
    dp_chkn.Data= MemReadPciCfgEp (host, socket, ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG);
    dp_chkn.Bits.dis_rdimm_par_chk = 0;
    dp_chkn.Bits.en_rdimm_par_err_log = 1;  // Enable RDIMM parity error logging
    if (host->nvram.mem.socket[socket].cmdClkTrainingDone == 0) {
      dp_chkn.Bits.en_rdimm_par_err_log = 0;
    }
    MemWritePciCfgEp (host, socket, ch, MC0_DP_CHKN_BIT_MCDDC_DP_REG, dp_chkn.Data);

    //4928166 MRC: MC Register settings to ensure Parity engines in MC are not enabled
    //DDR4 and NVMDIMM seems to have different behavior. For NVMDIMM parity needs to be disabled before CMD\CTL sweep
    //So even though function is called EnableParity for NVMDIMM we are really disabling parity
    mcSchedChknBit.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
    if (host->nvram.mem.socket[socket].cmdClkTrainingDone) {
      mcSchedChknBit.Bits.dis_ddrt_par_gen = 0;
      mcSchedChknBit.Bits.dis_rdimm_par_gen = 0;
    } else {
      mcSchedChknBit.Bits.dis_ddrt_par_gen = 1;
      mcSchedChknBit.Bits.dis_rdimm_par_gen = 1;
    }
    MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, mcSchedChknBit.Data);
  } // ch loop

  // WA HSD 4929163: From Si Bug Eco - ECC error in MRS command during CAP error FSM locks up the Patrol FSM
  // Should be fixed in B0
  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
      // HSD 5331580
      if((aepDimmCountImc[mcId] == 0) && ((host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) != STAT_VIRT_LOCKSTEP)){
        scrub_mask.Data = MemReadPciCfgMC (host, socket, mcId, SCRUBMASK_MC_MAIN_REG);
        scrub_mask.Bits.no_stop_on_ecc = 1;
        MemWritePciCfgMC(host, socket, mcId, SCRUBMASK_MC_MAIN_REG, scrub_mask.Data);
      }
    } //mcId loop

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      mcId = GetMCID(host, socket, ch);
      if(aepDimmCountImc[mcId] > 0) {
        //
        // Writes RC6 to 6 to clear Parity Error
        //
        erfDdr4CmdReg.Data       = 0x0;
        erfDdr4CmdReg.Bits.ad    = 0x66;
        erfDdr4CmdReg.Bits.bank  = 0x7;
        erfDdr4CmdReg.Bits.en    = 0x1;
        if (host->nvram.mem.dimmTypePresent == RDIMM) {
          erfDdr4CmdReg.Bits.rdimm = 1;
        } else {
          erfDdr4CmdReg.Bits.rdimm = 0;
        }
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG2_MCDDC_CTL_REG, erfDdr4CmdReg.Data);

        // zero out en bits for 0,1,3,4,5,6,7
        //0
        erfDdr4CmdReg.Data= MemReadPciCfgEp (host, socket, ch, ERF_DDR4_CMD_REG0_MCDDC_CTL_REG);
        erfDdr4CmdReg.Bits.en    = 0x0;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG0_MCDDC_CTL_REG, erfDdr4CmdReg.Data);
        //1
        erfDdr4CmdReg.Data= MemReadPciCfgEp (host, socket, ch, ERF_DDR4_CMD_REG1_MCDDC_CTL_REG);
        erfDdr4CmdReg.Bits.en    = 0x0;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG1_MCDDC_CTL_REG, erfDdr4CmdReg.Data);
        //3
        erfDdr4CmdReg.Data= MemReadPciCfgEp (host, socket, ch, ERF_DDR4_CMD_REG3_MCDDC_CTL_REG);
        erfDdr4CmdReg.Bits.en    = 0x0;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG3_MCDDC_CTL_REG, erfDdr4CmdReg.Data);
        //4
        erfDdr4CmdReg.Data= MemReadPciCfgEp (host, socket, ch, ERF_DDR4_CMD_REG4_MCDDC_CTL_REG);
        erfDdr4CmdReg.Bits.en    = 0x0;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG4_MCDDC_CTL_REG, erfDdr4CmdReg.Data);
        //5
        erfDdr4CmdReg.Data= MemReadPciCfgEp (host, socket, ch, ERF_DDR4_CMD_REG5_MCDDC_CTL_REG);
        erfDdr4CmdReg.Bits.en    = 0x0;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG5_MCDDC_CTL_REG, erfDdr4CmdReg.Data);
        //6
        erfDdr4CmdReg.Data= MemReadPciCfgEp (host, socket, ch, ERF_DDR4_CMD_REG6_MCDDC_CTL_REG);
        erfDdr4CmdReg.Bits.en    = 0x0;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG6_MCDDC_CTL_REG, erfDdr4CmdReg.Data);
        //7
        erfDdr4CmdReg.Data= MemReadPciCfgEp (host, socket, ch, ERF_DDR4_CMD_REG7_MCDDC_CTL_REG);
        erfDdr4CmdReg.Bits.en    = 0x0;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG7_MCDDC_CTL_REG, erfDdr4CmdReg.Data);
      } //NVMDIMM
    } // ch
  } //stepping
} // EnableCAParityRuntime

/**

  Exits self refresh

  @param host    - Pointer to sysHost
  @param socket  - Processor socket

  @retval N/A

**/
void
ExitSR (
       PSYSHOST  host,
       UINT8     socket
       )
{
  UINT8                     ch;
  volatile UINT32           data = 0;
  VOID                      (*functionPtr)(PSYSHOST host, UINT8 socket);
  UINT8                     (*endFunctionPtr)();
  UINT32                    **dataPtr;
  UINT32                    **endDataPtr;
  TCRFTP_MCDDC_CTL_STRUCT   tcrftp;
  UINT32                    tcrftpOrg[MAX_CH];
  struct  channelNvram      (*channelNvList)[MAX_CH];
  UINT32                    tczqcalOrg[MAX_CH];
  TCZQCAL_MCDDC_CTL_STRUCT  tczqcal;
  channelNvList = GetChannelNvList(host, socket);
  //
  // First read the entire function into cache so that there are
  // no flash reads during the S3 exit flow.  A flash read takes
  // too long and will violate timing parameters.
  //
  functionPtr = ExitSelfRefreshCriticalSection;
  endFunctionPtr = ExitSelfRefreshCriticalSectionEndMarker;
  dataPtr = (UINT32 **)(&functionPtr);
  endDataPtr = (UINT32 **)(&endFunctionPtr);
  //
  // Check to make sure the compiler didn't reorganize the end
  // function marker to someplace before the critical function.
  // If it did, then set the endFunctionPtr to a place 1000 hex
  // bytes after the start of the function.  This is more than
  // enough bytes to read the entire function into processor
  // cache.
  if (*endDataPtr <= *dataPtr) {
    *endDataPtr = *dataPtr + 0x1000;
  }

  while (*dataPtr < *endDataPtr) {
    data = **dataPtr;
    (*dataPtr)++;
  }

  if (data) data = 0; // To prevent GCC compiler from warning.(unused-but-set-variable)

  if (host->var.mem.subBootMode == AdrResume) {
    CoreReleaseADRClamps(host, socket);
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    tcrftpOrg[ch] = 0;
    tczqcalOrg[ch] = 0;
    if ((*channelNvList)[ch].enabled == 0) continue;
    tcrftp.Data          = MemReadPciCfgEp (host, socket, ch, TCRFTP_MCDDC_CTL_REG);
    tcrftpOrg[ch]        = tcrftp.Data;
    tcrftp.Bits.t_refi >>= 1; // divide by 2 during ExitSelfRefresh
    MemWritePciCfgEp (host, socket, ch, TCRFTP_MCDDC_CTL_REG, tcrftp.Data);
    tczqcal.Data         = MemReadPciCfgEp (host, socket, ch, TCZQCAL_MCDDC_CTL_REG);
    tczqcalOrg[ch]       = tczqcal.Data;
    tczqcal.Bits.zqcsperiod = 2;
    MemWritePciCfgEp (host, socket, ch, TCZQCAL_MCDDC_CTL_REG, tczqcal.Data);
  } // ch loop

  //
  // At this point, the entire function - ExitSelfRefresh - has
  // been read into processor cache.  Now we can execute it.
  //
  ExitSelfRefresh(host, socket);

  //
  // Enable ZQCS
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    SetZQMask(host, socket, ch, 0x00);
  } // ch loop

  // Guarantee multiple ZQCS commands on S3 resume and fast warm reset paths.(The ZQCL inside ExitSelfRefresh function is only executed for NVDIMM)
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast) {
    FixedDelay(host, 16000);
  }

  //
  // Restore tREFI and ZQCAL
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    MemWritePciCfgEp(host, socket, ch, TCRFTP_MCDDC_CTL_REG, tcrftpOrg[ch]);
    MemWritePciCfgEp(host, socket, ch, TCZQCAL_MCDDC_CTL_REG, tczqcalOrg[ch]);
  } // ch loop
} // ExitSR

#ifdef  LRDIMM_SUPPORT
#ifdef  SERIAL_DBG_MSG
void DumpLrbuf (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     dimm
               )
{
  UINT32 lrbufData;
  UINT16 offset;

  if (checkMsgLevel(host, SDBG_MAX)) {

    getPrintFControl(host);
    // debug... dump all MB config space via SMBus reads
    rcPrintf ((host, "\nN%d.C%d.D%d - LRDIMM config dump:\n", socket, ch, dimm));
    rcPrintf ((host, "Byte offset: 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F"));

    for (offset = 0; offset < 0xFF; offset = offset + 4) {

      if (SUCCESS == ReadLrbufSmb (host, socket, ch, dimm, offset, &lrbufData)) {
        if (offset % 16 == 0) {
          rcPrintf ((host, "\n       0x%03x:", offset));
        }
        rcPrintf ((host, " 0x%02x 0x%02x 0x%02x 0x%02x",
                  lrbufData & 0xFF, (lrbufData >> 8) & 0xFF, (lrbufData >> 16) & 0xFF, (lrbufData >> 24) & 0xFF));
      } else {
         rcPrintf ((host, "Failure: LRDIMM SMBus read at offset 0x%03x\n", offset));
      }
    }
    rcPrintf ((host, "\n\n"));

    releasePrintFControl(host);
  }
}
#endif  // SERIAL_DBG_MSG
#endif  //LRDIMM_SUPPORT

/**

  Switches to normal mode

  @param host    - Pointer to sysHost
  @param socket  - Socket to switch

**/
UINT32
SwitchToNormalMode (
  PSYSHOST  host
  )
{
  UINT8                                         socket = host->var.mem.currentSocket;
  UINT8                                         ch;
  UINT8                                         strobe;
  UINT8                                         mcId;
  UINT8                                         rank;
  UINT8                                         rankNum;
  UINT8                                         maxChannel;
  UINT8                                         curChannel;
  UINT8                                         memMode;
  UINT8                                         AepDimmPresentOnSocket;
  UINT64_STRUCT                                 msrData;
  struct channelNvram                           (*channelNvList)[MAX_CH];
  RPQAGE_MCDDC_CTL_STRUCT                       rpqAge;
  DEFEATURES0_M2MEM_MAIN_STRUCT                 m2mDefeatures0;
  DEFEATURES1_M2MEM_MAIN_STRUCT                 m2mDefeatures1;
  SBDEFEATURES0_MC_2LM_STRUCT                   SbDefeatures0;
  CKE_LL0_MCDDC_CTL_STRUCT                      cke_ll0;
  CKE_LL1_MCDDC_CTL_STRUCT                      cke_ll1;
  CKE_LL2_MCDDC_CTL_STRUCT                      cke_ll2;
  CKE_LL3_MCDDC_CTL_STRUCT                      cke_ll3;
  BIOS_RESET_CPL_PCU_FUN1_STRUCT                biosResetCpl;
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT             chknBit;
  MCSCHED_CHKN_BIT2_MCDDC_CTL_STRUCT            chknBit2;
  MCSCHED_CHKN_BIT3_MCDDC_CTL_STRUCT            chknBit3;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT               mcChknBits;
  MCMTR_MC_MAIN_STRUCT                          mcMtr;
  MC_INIT_STATE_G_MC_MAIN_STRUCT                mcInitStateG;
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT              miscCKECtl;
  CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT              cpgcDdrtMiscCtl;
  UINT8                                         dimm;
  struct dimmNvram                              (*dimmNvList)[MAX_DIMM];
  BOOLEAN                                       NgnDebugLock = TRUE;
  DA_ECC_ENABLE_FNV_DA_UNIT_0_STRUCT            daEccEnable;
  DN_ECC_ENABLE_FNV_D_UNIT_0_STRUCT             dnEccEnable;
  FNV_DEBUG_LOCK_FNV_DFX_MISC_0_STRUCT          fnvDebugLock;
  DIMMTEMPSTAT_0_MCDDC_CTL_STRUCT               DimmTempStat;
  BL_EGRESS_CREDIT_MCDDC_CTL_STRUCT             blEgressCredit;
  DDRT_RETRY_TIMER2_MC_2LM_STRUCT               ddrtRetryTimer2;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT               ddrtMiscDelay;
  MCBGFTUNE_MC_MAIN_STRUCT                      mcbgfTune;
  DDRT_RETRY_CTL_MC_2LM_STRUCT                  ddrtRetryCtl;
  DDRT_RETRY_TIMER_MC_2LM_STRUCT                ddrtRetryTimer;
  DDRT_RETRY_LINK_FAIL_MC_2LM_STRUCT            ddrtRetryLinkFail;
  DDRT_CREDIT_LIMIT_MC_2LM_STRUCT               ddrtCreditLimit;
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT            dataControl2;        //data
  DATACONTROL3N0_0_MCIO_DDRIO_STRUCT            dataControl3;        //data
  DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_STRUCT    ddrCRCmdNControls;   //CMDN
  DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_STRUCT    ddrCRCmdSControls;   //CMDS
  DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_STRUCT     ddrCRCkeControls;    //CKE
  DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_STRUCT     ddrCRCtlControls;    //CTL
  DDRCRCLKCONTROLS_MCIO_DDRIOEXT_STRUCT         ddrCRClkControls;    //CLK
  DDRCRCOMPCTL2_MCIO_DDRIOEXT_STRUCT            ddrCompCtl2;         //COMP
  DDRT_DEFEATURE_MC_2LM_STRUCT                  ddrtDefeature;
  DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_STRUCT  ddrCrDimmCmdControl1; // NVMCTLR CMD
  DDRCRCLKCONTROLS_CH_FNV_DDRIO_DAT7_CH_STRUCT  ddrCrDimmClkControl1; // NVMCTLR CLK
  DDRCRCOMPCTL2_CH_FNV_DDRIO_COMP_STRUCT        ddrCompCtl2Fnv;
  DATACONTROL3N0_0_FNV_DDRIO_COMP_STRUCT        dataControl3CompFnv;      // NVMCTLR data
  BL_EGRESS_CREDIT_MCDDC_CTL_STRUCT             blEgressCredit1;

  DDRT_MCA_CTL_MC_2LM_STRUCT                    ddrtMcaCtl;
  UINT32                                        tempData;
  DDRT_MISC_CTL_MC_2LM_STRUCT                   ddrtMiscCtl;
#if defined COSIM_EN || !defined YAM_ENV
  DRDP_WCRD_CONFIG_FNV_D_UNIT_0_STRUCT          drdpWcrdConfig;
  DA_WRITE_CREDIT_FNV_DA_UNIT_0_STRUCT          daWriteCredit;
  D_READ_CREDIT_FNV_D_UNIT_0_STRUCT             dReadCredit;
  DN_EMASK_FNV_D_UNIT_0_STRUCT                  dnEmask;
  DA_EMASK_FNV_DA_UNIT_0_STRUCT                 daEmask;
  D_FUNC_DEFEATURE1_FNV_D_UNIT_0_STRUCT         dFuncDefeature1;
#endif
  DDRT_STARVE_CTL_MC_2LM_STRUCT                 ddrtStarve;
  WRTRKRALLOC_M2MEM_MAIN_STRUCT                 WrTrkrAlloc;
  DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_STRUCT      ddrtMajorModeTh2;
  CREDITS_M2MEM_MAIN_STRUCT                     creditsM2mem;
  TCDBP_MCDDC_CTL_STRUCT                        tcdbp;
  TCOTHP_MCDDC_CTL_STRUCT                       tcothp;
  TCOTHP2_MCDDC_CTL_STRUCT                      tcothp2;
  DDRT_DATAPATH_DELAY2_MCDDC_DP_STRUCT          ddrtDataPathDelay2;
  T_DDRT_RD_CNSTRNT_MC_2LM_STRUCT               ddrtRdCnstrnt;
  T_DDRT_WR_CNSTRNT_MC_2LM_STRUCT               ddrtWrCnstrnt;
  T_DDRT_GNT2RW_CNSTRNT_MC_2LM_STRUCT           ddrtGnt2rwCnstrnt;
  T_DDRT_RW2GNT_CNSTRNT_MC_2LM_STRUCT           ddrtRw2gntCnstrnt;
  T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_STRUCT          ddrtGnt2gntCnstrnt;
  MSC_NGN_PLL_CTRL1_FNV_DFX_MISC_0_STRUCT       mscngnpllctrl1;
  MODE_STARVE_CYCLE_THRESHOLD_MC_2LM_STRUCT     ddrtModeStrvCycleTh;
  DDRT_MAJOR_MODE_THRESHOLD1_MC_2LM_STRUCT      ddrtMajorModeTh1;
  MODE_DDRT_CMD_STARVE_THRESHOLD_MC_2LM_STRUCT  ddrtCmdStarveThld;
  DDRT_MM_DDR4_CMD_THRESH_MCDDC_CTL_STRUCT      ddrtCmdThresh;
  MODE_DELAYS_MC_2LM_STRUCT                     modeDelays;
  SPARING_CONTROL_THRESH_MC_MAIN_STRUCT         SparingCtlThr;
  T_PERSIST_TIMING_MC_2LM_STRUCT                persistTiming;
  UINT8                                         EnabledChannelCount[MAX_IMC];
  LINK_CFG_READ_1_MCDDC_DP_STRUCT               linkCfgRead;
  DATACONTROL0N0_0_FNV_DDRIO_COMP_STRUCT        dataControl0fnv;
  REVISION_MFG_ID_FNV_DFX_MISC_0_STRUCT         revisionmfg;
  UINT8                                         McCreditCount[MAX_IMC];
  IDLETIME_MCDDC_CTL_STRUCT                     IdleTime;
  DDRT_FNV_INTR_CTL_MCDDC_DP_STRUCT             ddrtIntrCtl;
  UINT32                                        PcodeMailboxStatus = 0;
  UINT32                                        PCUData = 0;
  UINT8                                         i;

  // Continue if this socket is not present
  if (host->nvram.mem.socket[socket].enabled == 0) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);
  SetVLSModePerChannel(host, socket);
  SetVLSRegionPerChannel(host, socket);

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
      //comp
    mcChknBits.Data = MemReadPciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS_MC_MAIN_REG);

    // HSD 5331110 Disable Periodic Rcomp during memory training
    if ((CheckSteppingGreaterThan (host, CPU_SKX, A2_REV_SKX)) || (host->nvram.mem.socket[socket].aepDimmPresent==0)) {
      mcChknBits.Bits.dis_rcomp  = 0;
    }

    // HSD 5332135, 5371129 - set chkn bit for 3DS dimms and  LD64 LRDIMM failing when in Ch2/Ch5
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      if ((*channelNvList)[(mcId * MAX_MC_CH) + ch].enabled == 0) continue;
      if (((*channelNvList)[(mcId * MAX_MC_CH) + ch].encodedCSMode == 2) || ((*channelNvList)[(mcId * MAX_MC_CH) + ch].lrDimmPresent && (host->nvram.mem.socket[socket].ddrFreq == DDR_2666))) {
        mcChknBits.Bits.increase_rcomp = 1;
      }
    }

    MemWritePciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS_MC_MAIN_REG, mcChknBits.Data);
  }

    for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        dataControl2.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG));
        dataControl2.Bits.enearlywriterankqnnnh = 0;
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2.Data);
      } // strobe loop
    } // ch loop

  // 4930051: switching to normal mode, please set lvlshft_holden=1 for all data,cmd,ctl,clk fubs for SKX and NVMCTLR
  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
      //comp
    ddrCompCtl2.Data = MemReadPciCfgMC (host, socket, mcId, DDRCRCOMPCTL2_MCIO_DDRIOEXT_REG);
    ddrCompCtl2.Bits.lvlshft_holden  = 0;
    MemWritePciCfgMC (host, socket, mcId, DDRCRCOMPCTL2_MCIO_DDRIOEXT_REG, ddrCompCtl2.Data);
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //CMDN
    ddrCRCmdNControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRCmdNControls.Bits.lvlshft_holden  = 0;
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdNControls.Data);

    //CMDS
    ddrCRCmdSControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCmdSControls.Bits.lvlshft_holden  = 0;
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdSControls.Data);

    //CKE
    ddrCRCkeControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG);
    ddrCRCkeControls.Bits.lvlshft_holden  = 0;
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG, ddrCRCkeControls.Data);

    //CTL
    ddrCRCtlControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG);
    ddrCRCtlControls.Bits.lvlshft_holden  = 0;
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlControls.Data);

    //CLK
    ddrCRClkControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG);
    ddrCRClkControls.Bits.lvlshft_holden  = 0;
    MemWritePciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG, ddrCRClkControls.Data);

    // set default value for idle page reset value
    IdleTime.Data = MemReadPciCfgEp (host, socket, ch, IDLETIME_MCDDC_CTL_REG);
    IdleTime.Bits.idle_page_rst_val = IDLE_PAGE_RST_VAL;
    MemWritePciCfgEp (host, socket, ch, IDLETIME_MCDDC_CTL_REG, IdleTime.Data);

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      dataControl3.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG));
      dataControl3.Bits.lvlshft_holden = 0;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG), dataControl3.Data);
    }

    // NVMDIMM section
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

      // cmd
      ddrCrDimmCmdControl1.Data = host->nvram.mem.socket[socket].channelList[ch].dimmVrefControlFnv1;
      ddrCrDimmCmdControl1.Bits.lvlshft_holden = 0;

      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_NA_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmCmdControl1.Data, 0, FNV_IO_WRITE, 0xF);
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_NB_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmCmdControl1.Data, 0, FNV_IO_WRITE, 0xF);
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCMDCONTROLS_SB_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmCmdControl1.Data, 0, FNV_IO_WRITE, 0xF);

        // clk
      GetSetFnvIO (host, socket, ch, dimm, DDRCRCLKCONTROLS_CH_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmClkControl1.Data, 0, FNV_IO_READ, 0xF);
      ddrCrDimmClkControl1.Bits.lvlshft_holden = 0;
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCLKCONTROLS_CH_FNV_DDRIO_DAT7_CH_REG, &ddrCrDimmClkControl1.Data, 0, FNV_IO_WRITE, 0xF);

      // comp
      GetSetFnvIO (host, socket, ch, dimm, DDRCRCOMPCTL2_CH_FNV_DDRIO_COMP_REG, &ddrCompCtl2Fnv.Data, 0, FNV_IO_READ, 0xF);
      ddrCompCtl2Fnv.Bits.lvlshft_holden = 0;
      GetSetFnvIO(host, socket, ch, dimm, DDRCRCOMPCTL2_CH_FNV_DDRIO_COMP_REG, &ddrCompCtl2Fnv.Data, 0, FNV_IO_WRITE, 0xF);

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        GetSetFnvIO (host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_FNV_DDRIO_COMP_REG), &dataControl3CompFnv.Data, 0, FNV_IO_READ, 0xF);
        dataControl3CompFnv.Bits.lvlshft_holden = 0;
        GetSetFnvIO(host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_FNV_DDRIO_COMP_REG), &dataControl3CompFnv.Data, 0, FNV_IO_WRITE, 0xF);
      } // strobe
    } // dimm
  } // ch
  // // end 4930051


#if defined COSIM_EN || !defined YAM_ENV
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    (*channelNvList)[ch].fnvAccessMode = SMBUS;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ( (*dimmNvList)[dimm].dimmPresent == 0 ) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      //
      // Enable ECC on NVMCTLR
      // Program Write and Read Credits for NVMCTLR
      //
      daEccEnable.Data     = ReadFnvCfg (host, socket, ch, dimm, DA_ECC_ENABLE_FNV_DA_UNIT_0_REG);
      dnEccEnable.Data     = ReadFnvCfg (host, socket, ch, dimm, DN_ECC_ENABLE_FNV_D_UNIT_0_REG);
      drdpWcrdConfig.Data  = ReadFnvCfg (host, socket, ch, dimm, DRDP_WCRD_CONFIG_FNV_D_UNIT_0_REG);
      daWriteCredit.Data   = ReadFnvCfg (host, socket, ch, dimm, DA_WRITE_CREDIT_FNV_DA_UNIT_0_REG);
      dReadCredit.Data     = ReadFnvCfg (host, socket, ch, dimm, D_READ_CREDIT_FNV_D_UNIT_0_REG);
      daEmask.Data         = ReadFnvCfg (host, socket, ch, dimm, DA_EMASK_FNV_DA_UNIT_0_REG);
      dnEmask.Data         = ReadFnvCfg (host, socket, ch, dimm, DN_EMASK_FNV_D_UNIT_0_REG);
      dFuncDefeature1.Data = ReadFnvCfg (host, socket, ch, dimm, D_FUNC_DEFEATURE1_FNV_D_UNIT_0_REG);

      if (host->setup.mem.optionsNgn & NGN_ECC_CORR) {
        daEccEnable.Bits.ecc_corr_en = 1;
      } else{
        daEccEnable.Bits.ecc_corr_en = 0;
      }

      if (host->setup.mem.optionsNgn & NGN_ECC_WR_CHK) {
        daEccEnable.Bits.ecc_wr_chk_en = 1;
      } else {
        daEccEnable.Bits.ecc_wr_chk_en = 0;
      }

      if (host->setup.mem.optionsNgn & NGN_ECC_RD_CHK) {
        dnEccEnable.Bits.ecc_rd_chk_en = 1;
      } else {
        dnEccEnable.Bits.ecc_rd_chk_en = 0;
      }

      WriteFnvCfg (host, socket, ch, dimm, DA_ECC_ENABLE_FNV_DA_UNIT_0_REG, daEccEnable.Data);
      WriteFnvCfg (host, socket, ch, dimm, DN_ECC_ENABLE_FNV_D_UNIT_0_REG, dnEccEnable.Data);

      //
      // Reset Write Credit
      //
      drdpWcrdConfig.Bits.wcrd_cfg = 0;
      WriteFnvCfg (host, socket, ch, dimm, DRDP_WCRD_CONFIG_FNV_D_UNIT_0_REG, drdpWcrdConfig.Data);

      //
      // Trigger Write Credit Load
      //
      dFuncDefeature1.Bits.wcrd_init = 0;
      WriteFnvCfg (host, socket, ch, dimm, D_FUNC_DEFEATURE1_FNV_D_UNIT_0_REG, dFuncDefeature1.Data);

      dFuncDefeature1.Bits.wcrd_init = 1;
      WriteFnvCfg (host, socket, ch, dimm, D_FUNC_DEFEATURE1_FNV_D_UNIT_0_REG, dFuncDefeature1.Data);

      //
      // Program Write Credits and Read Credits
      //
      drdpWcrdConfig.Bits.wcrd_cfg = (*channelNvList)[ch].aepWrCreditLimit;
      daWriteCredit.Bits.val = (*channelNvList)[ch].aepWrCreditLimit;
      dReadCredit.Bits.val = DEFAULT_DDRT_READ_CREDIT; // default
      WriteFnvCfg (host, socket, ch, dimm, DRDP_WCRD_CONFIG_FNV_D_UNIT_0_REG, drdpWcrdConfig.Data);
      WriteFnvCfg (host, socket, ch, dimm, DA_WRITE_CREDIT_FNV_DA_UNIT_0_REG, daWriteCredit.Data);
      WriteFnvCfg (host, socket, ch, dimm, D_READ_CREDIT_FNV_D_UNIT_0_REG, dReadCredit.Data);

      //
      // Unmask Write Credit Check
      //
      dnEmask.Bits.wr_credit = 0;
      daEmask.Bits.wr_credit = 0;
      WriteFnvCfg (host, socket, ch, dimm, DN_EMASK_FNV_D_UNIT_0_REG, dnEmask.Data);
      WriteFnvCfg (host, socket, ch, dimm, DA_EMASK_FNV_DA_UNIT_0_REG, daEmask.Data);
    }
  }

  if (!(host->setup.mem.optionsNgn & NGN_DEBUG_LOCK)) {
    NgnDebugLock = FALSE;
  }
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ( (*dimmNvList)[dimm].dimmPresent == 0 ) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      if (NgnDebugLock == TRUE) {
        fnvDebugLock.Data = ReadFnvCfg(host, socket, ch, dimm, FNV_DEBUG_LOCK_FNV_DFX_MISC_0_REG);
        fnvDebugLock.Bits.dbg_en = 1;     // Write 1 to enable
        WriteFnvCfg (host, socket, ch, dimm, FNV_DEBUG_LOCK_FNV_DFX_MISC_0_REG, fnvDebugLock.Data);
        fnvDebugLock.Data = ReadFnvCfg(host, socket, ch, dimm, FNV_DEBUG_LOCK_FNV_DFX_MISC_0_REG);
        if (fnvDebugLock.Bits.dwe == 1) { // Check debug enabled by HW
          rcPrintf ((host, "\n: NGN unlocked for debug S%d.CH%d.D%d:\n", socket, ch, dimm));
        }
        fnvDebugLock.Bits.lock = 1;       // // Lock the register
        WriteFnvCfg (host, socket, ch, dimm, FNV_DEBUG_LOCK_FNV_DFX_MISC_0_REG, fnvDebugLock.Data);
      }
    }
  }
#endif //YAM_ENV

  //
  // Determine how many channels enabled on each MC
  //
  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    EnabledChannelCount[mcId] = 0;
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      if ((*channelNvList)[ch + (MAX_MC_CH * mcId)].enabled == 1) {
        EnabledChannelCount[mcId]++;
      }
    }
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;

    DdrtCapErrorWA(host, socket, ch);
    ddrtMiscDelay.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
    ddrtMiscDelay.Bits.force_ddr4_mode = 0;
    ddrtMiscDelay.Bits.force_ddrt_mode = 0;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);

    //HSD4929486, 4929487: Cloned From SKX Si Bug Eco: deadlock when non-TGR partial is starved and only TGR-reserved BL EGR remains
    if (CheckSteppingLessThan(host, CPU_SKX, A2_REV_SKX)) {
      ddrtMajorModeTh2.Data = MemReadPciCfgEp(host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_REG);
      ddrtMajorModeTh2.Bits.pwmm_starv_cntr_prescaler = 0;
      MemWritePciCfgEp(host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD2_MC_2LM_REG, ddrtMajorModeTh2.Data);
    }
    ddrtCreditLimit.Data = MemReadPciCfgEp (host, socket, ch, DDRT_CREDIT_LIMIT_MC_2LM_REG);
    ddrtCreditLimit.Bits.ddrt_rd_credit = DEFAULT_DDRT_READ_CREDIT; // default
    ddrtCreditLimit.Bits.ddrt_wr_credit = (*channelNvList)[ch].aepWrCreditLimit;
    //
    // 5370451: skx B0 or later needs to set min_rd_credit_partials to 1
    //
    if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
      ddrtCreditLimit.Bits.min_rd_credit_partials = 1;
    }
    MemWritePciCfgEp (host, socket, ch, DDRT_CREDIT_LIMIT_MC_2LM_REG, ddrtCreditLimit.Data);

    // 4929208: MC RPQ command starves when read credit remains 1 and BIDS Clears out the counters
    // no stepping check, as the RTL bug is not going to be fixed until GO
    ddrtDefeature.Data = MemReadPciCfgEp (host, socket, ch, DDRT_DEFEATURE_MC_2LM_REG);
    ddrtDefeature.Bits.ignore_erid_parity_error = 0;
    ddrtDefeature.Data &= ~(BIT25+BIT24);
    if (ddrtCreditLimit.Bits.ddrt_rd_credit <= 3)
      ddrtDefeature.Data |= BIT25;
    if (ddrtCreditLimit.Bits.ddrt_wr_credit <= 3)
      ddrtDefeature.Data |= BIT24;

    //
    // HSD 5332628: GNTGNT_S = 4 when all channels of MC populated and Channel2 is AEP
    //
    if ((EnabledChannelCount[ch / MAX_MC_CH] == 3) && (ch % MAX_MC_CH == 2)) {
      ddrtGnt2gntCnstrnt.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG);
      ddrtGnt2gntCnstrnt.Bits.t_ddrt_gntgnt_s = 4;
      MemWritePciCfgEp (host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG, ddrtGnt2gntCnstrnt.Data);
    }

    //
    //HSD4929074 Memory tile performance recommended and target CSR values for BIOS programming for ww20_2015 spreadsheet
    //
    ddrtRdCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_RD_CNSTRNT_MC_2LM_REG);
    ddrtRdCnstrnt.Bits.t_ddrt_rdrd_s = 0x0;
    ddrtRdCnstrnt.Bits.t_ddrt_rdrd_dd = 0x0;
    ddrtRdCnstrnt.Bits.t_ddrt_wrrd_s = 0x02;
    ddrtRdCnstrnt.Bits.t_ddrt_wrrd_dd = 0x02;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_RD_CNSTRNT_MC_2LM_REG, ddrtRdCnstrnt.Data);

    ddrtWrCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_WR_CNSTRNT_MC_2LM_REG);
    ddrtWrCnstrnt.Bits.t_ddrt_rdwr_s = 0x0;
    ddrtWrCnstrnt.Bits.t_ddrt_rdwr_dd = 0x13;
    ddrtWrCnstrnt.Bits.t_ddrt_wrwr_s = 0x02;
    //
    // HSD5332782: t_ddrt_wrwr_dd = (t_cwl + t_cwl_adj - t_cwl_adj_neg + 1) - (en_wr_ddrt_mode + 2)
    //
    tcdbp.Data              = MemReadPciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG);
    tcothp.Data             = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
    tcothp2.Data            = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
    ddrtDataPathDelay2.Data = MemReadPciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG);
    ddrtWrCnstrnt.Bits.t_ddrt_wrwr_dd = tcdbp.Bits.t_cwl + tcothp.Bits.t_cwl_adj - tcothp2.Bits.t_cwl_adj_neg + 1;
    if (ddrtWrCnstrnt.Bits.t_ddrt_wrwr_dd > (ddrtDataPathDelay2.Bits.en_wr_ddrt_mode + 2)) {
      ddrtWrCnstrnt.Bits.t_ddrt_wrwr_dd -= (ddrtDataPathDelay2.Bits.en_wr_ddrt_mode + 2);
    } else {
      ddrtWrCnstrnt.Bits.t_ddrt_wrwr_dd = 0;
    }
    MemWritePciCfgEp (host, socket, ch, T_DDRT_WR_CNSTRNT_MC_2LM_REG, ddrtWrCnstrnt.Data);

    ddrtGnt2rwCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_GNT2RW_CNSTRNT_MC_2LM_REG);
    ddrtGnt2rwCnstrnt.Bits.t_ddrt_gntrd_s = 0x0;
    ddrtGnt2rwCnstrnt.Bits.t_ddrt_gntrd_dd = 0x0;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_GNT2RW_CNSTRNT_MC_2LM_REG, ddrtGnt2rwCnstrnt.Data);

    ddrtRw2gntCnstrnt.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_RW2GNT_CNSTRNT_MC_2LM_REG);
    ddrtRw2gntCnstrnt.Bits.t_ddrt_rdgnt_s = 0x0;
    ddrtRw2gntCnstrnt.Bits.t_ddrt_rdgnt_dd = 0x2;

    if (host->nvram.mem.socket[socket].ddrFreq >= DDR_2400){
      ddrtRw2gntCnstrnt.Bits.t_ddrt_wrgnt_dd = 0x04;
      ddrtRw2gntCnstrnt.Bits.t_ddrt_wrgnt_s = 0x04;
    }

    if (IsLrdimmPresent(host, socket, ch, 0) && ((*channelNvList)[ch].maxDimm == 2)) {
      ddrtRw2gntCnstrnt.Bits.t_ddrt_wrgnt_dd = 0x07;
    }

    MemWritePciCfgEp (host, socket, ch, T_DDRT_RW2GNT_CNSTRNT_MC_2LM_REG, ddrtRw2gntCnstrnt.Data);

    if ((CheckSteppingLessThan(host, CPU_SKX, B1_REV_SKX)) && ((*channelNvList)[ch].maxDimm == 2)) {
      ddrtGnt2gntCnstrnt.Data = (UINT16)MemReadPciCfgEp(host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG);
      ddrtGnt2gntCnstrnt.Bits.t_ddrt_gntgnt_dd += 8;
      MemWritePciCfgEp(host, socket, ch, T_DDRT_GNT2GNT_CNSTRNT_MC_2LM_REG, ddrtGnt2gntCnstrnt.Data);
    }

    ddrtModeStrvCycleTh.Data = MemReadPciCfgEp (host, socket, ch, MODE_STARVE_CYCLE_THRESHOLD_MC_2LM_REG);
    ddrtModeStrvCycleTh.Bits.ddrt = 0x32;
    ddrtModeStrvCycleTh.Bits.ddr4 = 0x32;
    MemWritePciCfgEp (host, socket, ch, MODE_STARVE_CYCLE_THRESHOLD_MC_2LM_REG, ddrtModeStrvCycleTh.Data);

    ddrtMajorModeTh1.Data = MemReadPciCfgEp (host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD1_MC_2LM_REG);
    ddrtMajorModeTh1.Bits.wmm_enter = 0x12;
    ddrtMajorModeTh1.Bits.wmm_exit = 0x06;
    ddrtMajorModeTh1.Bits.min_rmm_cycles = 0x1E;
    ddrtMajorModeTh1.Bits.min_wmm_cycles = 0x3C;
    MemWritePciCfgEp (host, socket, ch, DDRT_MAJOR_MODE_THRESHOLD1_MC_2LM_REG, ddrtMajorModeTh1.Data);

    modeDelays.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, MODE_DELAYS_MC_2LM_REG);
    modeDelays.Bits.ddr4_to_ddrt_delay = 0x04;
    modeDelays.Bits.ddrt_to_ddr4_delay = 0x04;
    if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
      modeDelays.Bits.wmm_gnt2gnt_delay = 0x13;
      modeDelays.Bits.reqs = 0x16;
    }
    MemWritePciCfgEp (host, socket, ch, MODE_DELAYS_MC_2LM_REG, modeDelays.Data);

    ddrtCmdStarveThld.Data =  MemReadPciCfgEp (host, socket, ch, MODE_DDRT_CMD_STARVE_THRESHOLD_MC_2LM_REG);
    ddrtCmdStarveThld.Bits.reads = 0x1E;
    ddrtCmdStarveThld.Bits.writes = 0x18;
    ddrtCmdStarveThld.Bits.partials = 0x10;
    ddrtCmdStarveThld.Bits.gnts = 0x26;
    MemWritePciCfgEp (host, socket, ch, MODE_DDRT_CMD_STARVE_THRESHOLD_MC_2LM_REG, ddrtCmdStarveThld.Data);

    if(CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)){
      ddrtCmdThresh.Data =  MemReadPciCfgEp (host, socket, ch, DDRT_MM_DDR4_CMD_THRESH_MCDDC_CTL_REG);
      ddrtCmdThresh.Bits.ddr4_rd_cmd_threshold = 0x1A;
      ddrtCmdThresh.Bits.ddr4_wr_cmd_threshold = 0x1A;
      MemWritePciCfgEp (host, socket, ch, DDRT_MM_DDR4_CMD_THRESH_MCDDC_CTL_REG, ddrtCmdThresh.Data);
    }
    // End Performance Timings

    // 5330588: EKV sends up interrupt packet with mismatching RID in metadata and MC logs MCA for ERID UC
    // No stepping check, as applies for all SKX steppings
    ddrtDefeature.Data |= BIT17;

    MemWritePciCfgEp (host, socket, ch, DDRT_DEFEATURE_MC_2LM_REG, ddrtDefeature.Data);

    //
    // 5332747: Need to clear imcX_cX_ddrt_err_log_1st after training.
    //
    MemWritePciCfgEp (host, socket, ch, DDRT_ERR_LOG_1ST_MC_2LM_REG,  0);
    MemWritePciCfgEp (host, socket, ch, DDRT_ERR_LOG_NEXT_MC_2LM_REG, 0);
  } // ch loop

  //
  // program the per channel MC-M2M NVMDIMM RPQ credits to 39
  //
  if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {
    for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
      for (ch = 0; ch < MAX_MC_CH; ch++) {
        if ((*channelNvList)[(mcId * MAX_MC_CH) + ch].enabled == 0) continue;
        if ((*channelNvList)[(mcId * MAX_MC_CH) + ch].ddrtEnabled == 0) continue;
        creditsM2mem.Data = MemReadPciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG);
        /*
         NVMDIMM RPQ 3:0 = 0100
         Channel  5:4 =
                        channel 0 - 00
                        channel 1 - 01
                        channel 2 - 10
        */
        creditsM2mem.Bits.crdttype = 0x4 | (ch << 4);
        creditsM2mem.Bits.crdtwren = 0;
        creditsM2mem.Bits.crdtcnt = DEFAULT_DDRT_READ_CREDIT - 1;
        MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
        creditsM2mem.Bits.crdtwren = 1;
        MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
        creditsM2mem.Bits.crdtwren = 0;
        MemWritePciCfgMC (host, socket, mcId, CREDITS_M2MEM_MAIN_REG, creditsM2mem.Data);
      }
    }
  }

  //
  // 4929281: SKX Egress credit programming
  // Calculate enabled channels count for MC credit programming,
  // and then calculate the credit count
  //
  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    EnabledChannelCount[mcId] = 0;
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      if ((*channelNvList)[ch + (MAX_MC_CH * mcId)].enabled == 1) {
        EnabledChannelCount[mcId]++;
      }
    }
    switch (EnabledChannelCount[mcId]) {
      case (1):
        McCreditCount[mcId] = MC_CREDIT_COUNT_1_CHANNEL_ENABLED;
        break;
      case (2):
        McCreditCount[mcId] = MC_CREDIT_COUNT_2_CHANNELS_ENABLED;
        break;
      case (3):
        McCreditCount[mcId] = MC_CREDIT_COUNT_3_CHANNELS_ENABLED;
        break;
    }
  }

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0)
      continue;

      WrTrkrAlloc.Data = MemReadPciCfgMC (host, socket, mcId, WRTRKRALLOC_M2MEM_MAIN_REG);
      WrTrkrAlloc.Bits.sttrkrpwrthresh = 11;
      MemWritePciCfgMC (host, socket, mcId, WRTRKRALLOC_M2MEM_MAIN_REG, WrTrkrAlloc.Data);
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    rankNum = 0;
    //
    // Enable 2 cycle bypass after CPGC activity is done
    //
    chknBit.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
    chknBit.Bits.dis_2cyc_byp = 0;
    dimmNvList = GetDimmNvList(host, socket, ch);

    // if DIMM 0 and 1 are not NVMDIMM
    if (((*dimmNvList)[0].aepDimmPresent == 0) && ((*dimmNvList)[1].aepDimmPresent == 0)) {
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          rankNum++;
        }
      }
      if (rankNum > 4) {
        if(host->setup.mem.PpdEn == 1) {
          chknBit.Bits.dis_2cyc_byp = 1;
        } else {
          chknBit.Bits.dis_2cyc_byp = 0;
          cke_ll0.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL0_MCDDC_CTL_REG);
          cke_ll1.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL1_MCDDC_CTL_REG);
          cke_ll2.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL2_MCDDC_CTL_REG);
          cke_ll3.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL3_MCDDC_CTL_REG);
          cke_ll0.Bits.ppd_en = 0;
          cke_ll1.Bits.ppd_en = 0;
          cke_ll2.Bits.ppd_en = 0;
          cke_ll3.Bits.ppd_en = 0;
          MemWritePciCfgEp (host, socket, ch, CKE_LL0_MCDDC_CTL_REG, cke_ll0.Data);
          MemWritePciCfgEp (host, socket, ch, CKE_LL1_MCDDC_CTL_REG, cke_ll1.Data);
          MemWritePciCfgEp (host, socket, ch, CKE_LL2_MCDDC_CTL_REG, cke_ll2.Data);
          MemWritePciCfgEp (host, socket, ch, CKE_LL3_MCDDC_CTL_REG, cke_ll3.Data);
         }
      }
    }

    chknBit.Bits.cmd_oe_alwy_on = 0;
    MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, chknBit.Data);

    //
    // s4929454: Cloned From SKX Si Bug Eco: ACTSafe stuck at 0 leads to blocked read
    // A0 workaround
    //
    if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
      chknBit2.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG);
      chknBit2.Bits.dis_safe_pri_rcb_gate = 1;
      chknBit2.Bits.dis_gs_pri_rcb_gate = 1;
      chknBit2.Bits.dis_pgt_pri_rcb_gate = 1;
      MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG, chknBit2.Data);
    }

    if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
      chknBit3.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT3_MCDDC_CTL_REG);
      chknBit3.Data |= 0x80;
      MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT3_MCDDC_CTL_REG, chknBit3.Data);
    }

    //
    // Program Read Pending Queue Age Counters
    //
    rpqAge.Data = MemReadPciCfgEp (host, socket, ch, RPQAGE_MCDDC_CTL_REG);
    rpqAge.Bits.cpugtcount = CPU_GT_COUNT;
    rpqAge.Bits.iocount = IO_COUNT;
    MemWritePciCfgEp (host, socket, ch, RPQAGE_MCDDC_CTL_REG, rpqAge.Data);


    //
    // clear the multi credit, disable_max_credit_check for NVMDIMM
    //
    if ((*channelNvList)[ch].ddrtEnabled) {
      dimmNvList = GetDimmNvList(host, socket, ch);
      cpgcDdrtMiscCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG);
      cpgcDdrtMiscCtl.Bits.multi_credit_on = 0;
      cpgcDdrtMiscCtl.Bits.disable_max_credit_check = 0;
      cpgcDdrtMiscCtl.Bits.enable_erid_return  = 0;
      MemWritePciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG, cpgcDdrtMiscCtl.Data);

      persistTiming.Data = MemReadPciCfgEp (host, socket, ch, T_PERSIST_TIMING_MC_2LM_REG);
      persistTiming.Bits.t_persist_delay = 0x7e; // default
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        mscngnpllctrl1.Data = ReadFnvCfg(host, socket, ch, dimm, MSC_NGN_PLL_CTRL1_FNV_DFX_MISC_0_REG);
        // if 667
        if (mscngnpllctrl1.Bits.freqcur == 0xa) {
          persistTiming.Bits.t_persist_delay = 0x40;
        }
      } // dimm
      MemWritePciCfgEp (host, socket, ch, T_PERSIST_TIMING_MC_2LM_REG, persistTiming.Data);
    }
    //
    // 4929408: PowerOn Safe Settings in the DDRT_MISC_CTL register
    //
    ddrtMiscCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRT_MISC_CTL_MC_2LM_REG);
    if (host->setup.mem.Disddrtopprd == 0 || (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX) &&
        CheckSteppingLessThan (host, CPU_SKX, H0_REV_SKX)) ) {
      // PO Safe
      ddrtMiscCtl.Bits.dis_ddrt_opp_rd = 1;
      ddrtMiscCtl.Bits.dis_ddrt_opp_ufill_rd = 1;
    } else {
      // POR
      ddrtMiscCtl.Bits.dis_ddrt_opp_rd = 0;
      ddrtMiscCtl.Bits.dis_ddrt_opp_ufill_rd = 0;
    }
    if (CheckSteppingGreaterThan(host, CPU_SKX, B0_REV_SKX)) {
      ddrtMiscCtl.Data &= ~BIT30;
    }
    MemWritePciCfgEp (host, socket, ch, DDRT_MISC_CTL_MC_2LM_REG, ddrtMiscCtl.Data);
    //
    // 4929410: MC_2LMDDRT_CRNODE_CH0_MC0_CR_DDRT_MCA_CTL.dis_erid_par should be set to 0 before normal mode
    // 4929409: Cloned From SKX Si Bug Eco: DDRT_SCHED_ERROR MCA can incorrectly fire
    //
    ddrtMcaCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRT_MCA_CTL_MC_2LM_REG);
    ddrtMcaCtl.Bits.dis_erid_par = 0;
    ddrtMcaCtl.Bits.dis_ddrt_scheduler_mismatch = 1;
    ddrtMcaCtl.Bits.dis_pending_intr_error = 1;
    MemWritePciCfgEp (host, socket, ch, DDRT_MCA_CTL_MC_2LM_REG, ddrtMcaCtl.Data);

    //
    // 4929053  BIOS shoud set reissue_err_ack CSR bit as W/A to finish NVMDIMM Error Flow
    //
    ddrtRetryCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRT_RETRY_CTL_MC_2LM_REG);
    ddrtRetryCtl.Bits.reissue_err_ack = 1;
    ddrtRetryCtl.Bits.force_timer_non_ddrt_mode = 0x48;  // HSD 4929270
    MemWritePciCfgEp (host, socket, ch, DDRT_RETRY_CTL_MC_2LM_REG, ddrtRetryCtl.Data);

    //
    // HSD 4929270 - NVMDIMM Retry FSM shows slow progress with DDR4 WPQ has transactions during Retry flow
    //
    ddrtRetryTimer2.Data = MemReadPciCfgEp (host, socket, ch, DDRT_RETRY_TIMER2_MC_2LM_REG);
    ddrtRetryTimer2.Bits.train_reset_time = 0x20;
    MemWritePciCfgEp (host, socket, ch, DDRT_RETRY_TIMER2_MC_2LM_REG, ddrtRetryTimer2.Data);

    //
    // 4929246  MC_2LMDDRT_CRNODE_CHX_MCY_CR_DDRT_RETRY_TIMER.disable_retry_timer should be set to 0
    //
    ddrtRetryTimer.Data = MemReadPciCfgEp (host, socket, ch, DDRT_RETRY_TIMER_MC_2LM_REG);
    ddrtRetryTimer.Bits.disable_retry_timer = 0;
    MemWritePciCfgEp (host, socket, ch, DDRT_RETRY_TIMER_MC_2LM_REG, ddrtRetryTimer.Data);

    //
    // 4929247  MC_2LMDDRT_CRNODE_CH0_MC0_CR_DDRT_RETRY_LINK_FAIL.threshold should be set to 0x10
    //
    ddrtRetryLinkFail.Data = MemReadPciCfgEp (host, socket, ch, DDRT_RETRY_LINK_FAIL_MC_2LM_REG);
    ddrtRetryLinkFail.Bits.threshold = 0x10;
    MemWritePciCfgEp (host, socket, ch, DDRT_RETRY_LINK_FAIL_MC_2LM_REG, ddrtRetryLinkFail.Data);

    //
    // 4929211: Cloned From SKX Si Bug Eco: [NVMDIMM EPO] SKX MC Err Flow fails when NVMCTLR asserts NVMDIMM ERR# pin
    // 4929281: SKX Egress credit programming
    //
    blEgressCredit.Data = MemReadPciCfgEp (host, socket, ch, BL_EGRESS_CREDIT_MCDDC_CTL_REG);
    if (host->var.mem.volMemMode == VOL_MEM_MODE_2LM) {
      blEgressCredit.Bits.min_ddr4_egrcrdt = 3;
    } else {
      blEgressCredit.Bits.min_ddr4_egrcrdt = 2;
    }
    blEgressCredit.Bits.ddr4_egcrdt = McCreditCount[ch / MAX_MC_CH];
    blEgressCredit.Bits.ddrt_egcrdt = McCreditCount[ch / MAX_MC_CH];
    MemWritePciCfgEp (host, socket, ch, BL_EGRESS_CREDIT_MCDDC_CTL_REG, blEgressCredit.Data);

    //
    // 4930151: Cloned From SKX Si Bug Eco: Change value of bl_egress_credit.ddrt_egrcrdt_low to 3
    // this value holds for all steppings, not a WA based on sighting info
    //
    blEgressCredit1.Data = MemReadPciCfgEp (host, socket, ch, BL_EGRESS_CREDIT_MCDDC_CTL_REG);
    blEgressCredit1.Bits.ddrt_egrcrdt_low = 3;
    MemWritePciCfgEp (host, socket, ch, BL_EGRESS_CREDIT_MCDDC_CTL_REG, blEgressCredit1.Data);

    //
    // s4929272: Cloned From SKX Si Bug Eco: patrol deallocated by SbDeallocAll
    //   Set each MC channel's SbDefeatures0.NumSbEntries to 126
    //
    SbDefeatures0.Data = MemReadPciCfgEp (host, socket, ch, SBDEFEATURES0_MC_2LM_REG);
    SbDefeatures0.Bits.numsbentries = 126;
    MemWritePciCfgEp (host, socket, ch, SBDEFEATURES0_MC_2LM_REG, SbDefeatures0.Data);
  } // ch loop

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
    m2mDefeatures0.Data = MemReadPciCfgMC (host, socket, mcId, DEFEATURES0_M2MEM_MAIN_REG);

    //s24929342 Need to set EgrBypDis and IngBypDis to 0

    m2mDefeatures0.Bits.ingbypdis = 0;
    if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {
      m2mDefeatures0.Bits.egrbypdis = 0;
    } else {
      m2mDefeatures0.Bits.egrbypdis = 1;
    }

    if (CheckSteppingLessThan(host, CPU_SKX, B1_REV_SKX)) {
      //5332548: Cloned From SKX Si Bug Eco: Cloned Bug: bypassed prefetch can deallocate demand read ak credit prematurely
      if ((host->setup.kti.XptPrefetchEn == KTI_ENABLE) && ( (host->var.mem.volMemMode == VOL_MEM_MODE_1LM) && ( (host->var.mem.socket[socket].volSize > 0) || (host->var.mem.socket[socket].perSize > 0)) ) ) {
        m2mDefeatures0.Bits.ingbypdis = 1;
      }
    }
    //
    // s5372310 Heavy ECC Injection = TOR TO or HA unexpected response
    //  Always set wait4bothhalves
    //
    m2mDefeatures0.Bits.wait4bothhalves = 1;

    //
    // 4929076  Cloned From SKX Si Bug Eco: MC NVMDIMM ECC Error on No-Data packet during NM Correction can cause NM access to be marked as fatal
    //
    if (CheckSteppingEqual(host, CPU_SKX, A0_REV)) {
      m2mDefeatures0.Bits.ddr4maxretries = 6;
      m2mDefeatures0.Bits.ddrtmaxretries = 6;
    } else {
      m2mDefeatures0.Bits.ddr4maxretries = 4;
      m2mDefeatures0.Bits.ddrtmaxretries = 4;
    }
    MemWritePciCfgMC (host, socket, mcId, DEFEATURES0_M2MEM_MAIN_REG, m2mDefeatures0.Data);


    //[SKX B0 PO] RankSparing silent data corruption
    if (CheckSteppingEqual(host, CPU_SKX, B0_REV_SKX) || CheckSteppingEqual(host, CPU_SKX, L0_REV_SKX)|| CheckSteppingEqual(host, CPU_SKX, B1_REV_SKX)) {
      SparingCtlThr.Data = MemReadPciCfgMC (host, socket, mcId, SPARING_CONTROL_THRESH_MC_MAIN_REG);
      SparingCtlThr.Bits.addr_skip_en = 0;
      MemWritePciCfgMC (host, socket, mcId, SPARING_CONTROL_THRESH_MC_MAIN_REG, SparingCtlThr.Data);
    }

    //
    // 4929079  Cloned From SKX Si Bug Eco: SKX m2mem: ErrUcNmCacheSrb incorrectly fires for poison writes leading to them being issued as MirrScrubWr(MEMWR) causing REJ_DURING_RETRY_2
    // 4929266: Cloned From SKX Si Bug Eco: SKX m2mem: NMCACHE_CHECK to RD_REQ_PRI jump happens with no RetryGo leading to a hang
    // 5331744: CLONE from skylake_server: BIOS setting fatalonnmcacherducdata but not fatalonnmcachepwrucdata for 303282
    //
    if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
      m2mDefeatures1.Data = MemReadPciCfgMC (host, socket, mcId, DEFEATURES1_M2MEM_MAIN_REG);
      m2mDefeatures1.Bits.uconnmcacherducdata = 1;
      m2mDefeatures1.Bits.uconnmcachepwrucdata = 1;

      //
      // s4929254 Cloned From SKX Si Bug Eco: 2LM X Sparing : during init quisce flow Fill is incorrectly getting blocked from allocation into Egress
      // If SKX A0 and Patrol Scrub enabled, set Defeatures1[EgrEco[25]] = 1
      //
      if (host->nvram.mem.RASmodeEx & PTRLSCRB_EN) {
        m2mDefeatures1.Bits.egreco |= 0x02;
      }
      MemWritePciCfgMC (host, socket, mcId, DEFEATURES1_M2MEM_MAIN_REG, m2mDefeatures1.Data);
    }

    m2mDefeatures1.Data = MemReadPciCfgMC (host, socket, mcId, DEFEATURES1_M2MEM_MAIN_REG);
    m2mDefeatures1.Bits.ingeco = 0x06;
    //
    // s5370294: Set ingeco to 0 in 1-way imc interleave mode
    //
    if(CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)){
      if (host->var.mem.socket[socket].imc[mcId].TAD[0].Enable){
        for (i = 0; i < TAD_RULES; i++) {
          if (!host->var.mem.socket[socket].imc[mcId].TAD[i].Enable) break;
          if (host->var.mem.socket[socket].imc[mcId].TAD[i].socketWays == 1){
            m2mDefeatures1.Bits.ingeco = 0;
          }
        }
      }
    }
    // HSD 5372273 BIOS needs to set IngEco.bit22 only when both prefetch and ingress bypass are enabled.
    if (CheckSteppingGreaterThan(host, CPU_SKX, B1_REV_SKX)){
      m2mDefeatures0.Data = MemReadPciCfgMC (host, socket, mcId, DEFEATURES0_M2MEM_MAIN_REG);
      if ((m2mDefeatures0.Bits.ingbypdis == 0) && ((host->setup.kti.XptPrefetchEn == KTI_ENABLE) || (host->setup.kti.KtiPrefetchEn == KTI_ENABLE))) {
        m2mDefeatures1.Bits.ingeco |= 0x40;
      }
    }
    MemWritePciCfgMC (host, socket, mcId, DEFEATURES1_M2MEM_MAIN_REG, m2mDefeatures1.Data);
  }//mcid loop

  //HSD: 4929680 / 533028 SKX MC : Increase wrcrdt_threshold until SKX B0 (Only for NVMDIMM)
  //Should be removed in B0
  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
      ddrtStarve.Data = MemReadPciCfgEp (host, socket, ch, DDRT_STARVE_CTL_MC_2LM_REG);
      ddrtStarve.Bits.wrcrdt_threshold = 0x03;
      ddrtStarve.Bits.tgr_threshold = 0x04;
      ddrtStarve.Bits.timer = 0x0b;
      MemWritePciCfgEp (host, socket, ch, DDRT_STARVE_CTL_MC_2LM_REG, ddrtStarve.Data);
    }
  }


  //HSD 5332578 Implement Workaround for SKX Bug ECO 306283 To Allow Memory Access After Viral
  // Need to enable this for all steppings (HSD 5371212)
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    linkCfgRead.Data = MemReadPciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG);
    linkCfgRead.Bits.read_data |= BIT9;
    MemWritePciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG, linkCfgRead.Data);
  }
  //End HSD: 5332578

  // HSD 5370910/ 5372337: <B1c> memic + cap fails w/ M2M TO after traffic stops
  AepDimmPresentOnSocket = 0;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < host->var.mem.socket[socket].channelList[ch].numDimmSlots; dimm++){
      if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
        AepDimmPresentOnSocket = 1;
      } // AEP DIMM present
    } // dimm loop
  } // ch loop
  if ((host->var.mem.volMemMode == VOL_MEM_MODE_1LM) && (!(host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP)) && (AepDimmPresentOnSocket == 0)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      linkCfgRead.Data = MemReadPciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG);
      linkCfgRead.Bits.read_data |= BIT11;
      MemWritePciCfgEp (host, socket, ch, LINK_CFG_READ_1_MCDDC_DP_REG, linkCfgRead.Data);
    }
    WriteBios2PcuMailboxCommand (host, socket, (UINT32) MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE, MAILBOX_BIOS_NOMCSTOPONDDR4ERR);
  }
  // HSD 5370910

  //HSD 5332703-EKV S0 IO power optimization requires DDRT training update
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    ddrtIntrCtl.Data = 0;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

      // 5370437: [CR B0] BIOS needs to enable SMI & ERR pin signaling on DDRT interrupts by programming MC_RRD_CRNODE_CH0_MC0_CR_DDRT_FNV_INTR_CTL register
      if (dimm == 0) {
        // Enable SMI & ERRO signalling for DDRT priority lo & hi interrupt
        ddrtIntrCtl.Bits.slot0_lo_smi_en = 1;
        ddrtIntrCtl.Bits.slot0_hi_smi_en = 1;
        ddrtIntrCtl.Bits.slot0_lo_err0_en = 1;
        ddrtIntrCtl.Bits.slot0_hi_err0_en = 1;
      } else if (dimm == 1) {
        // Enable SMI & ERRO signalling for DDRT priority lo & hi interrupt
        ddrtIntrCtl.Bits.slot1_lo_smi_en = 1;
        ddrtIntrCtl.Bits.slot1_hi_smi_en = 1;
        ddrtIntrCtl.Bits.slot1_lo_err0_en = 1;
        ddrtIntrCtl.Bits.slot1_hi_err0_en = 1;
      }
      revisionmfg.Data = ReadFnvCfg(host, socket, ch, dimm, REVISION_MFG_ID_FNV_DFX_MISC_0_REG);
      if (revisionmfg.Bits.revision_id == 0x10) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          GetSetFnvIO(host, socket, ch, dimm, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG, &dataControl0fnv.Data, 0, FNV_IO_READ, 0xF);
          dataControl0fnv.Bits.txpion = 0;
          dataControl0fnv.Bits.rxpion = 0;
          GetSetFnvIO(host, socket, ch, dimm, UpdateIoRegisterCh(host, strobe, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG), &dataControl0fnv.Data, 0, FNV_IO_WRITE, 0xF);
        } // strobe loop
      }
    } // dimm loop
    if (ddrtIntrCtl.Data != 0) {
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "ddrtIntrCtl.Data = %d \n", ddrtIntrCtl.Data));
      MemWritePciCfgEp (host, socket, ch, DDRT_FNV_INTR_CTL_MCDDC_DP_REG, ddrtIntrCtl.Data);
    }
  }  // ch loop

  EnableTsod(host, socket);

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
    //
    // Get MCMTR
    //
    mcMtr.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);

    //default is disabled
    mcMtr.Bits.bank_xor_enable = 0;

    mcMtr.Bits.trng_target = 0;
    MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);

    //
    // Allow CS to float when a command is not being issued
    //
    mcInitStateG.Data = MemReadPciCfgMC (host, socket, mcId, MC_INIT_STATE_G_MC_MAIN_REG);
    mcInitStateG.Bits.cs_oe_en = 0;
    MemWritePciCfgMC (host, socket, mcId, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);

    mcMtr.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
    //
    // Enter normal mode
    //
    mcMtr.Bits.normal     = 1;
    mcMtr.Bits.trng_mode  = 3;

    //
    // Set to closed page mode
    //
    if (host->setup.mem.options & PAGE_POLICY) {
      mcMtr.Bits.close_pg = 1;
    }
    MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);
  } // mcId loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //
    // Clear the CKE override
    //
    miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    miscCKECtl.Bits.cke_override = 0;
    miscCKECtl.Bits.cke_on = 0;
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);

    //
    // Initialize per dimm throttling and power related registers
    //
    dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      //
      // Program a constant temperature in the DIMMTEMPSTAT register
      // Expectation is that BIOS programs temperature between temp_mid and temp_hi
      //
      DimmTempStat.Data = MemReadPciCfgEp (host, socket, ch, (DIMMTEMPSTAT_0_MCDDC_CTL_REG + (dimm * 4)));
      DimmTempStat.Bits.ev_asrt_tempoemhi = 1;
      DimmTempStat.Bits.ev_asrt_tempoemlo = 1;
      DimmTempStat.Bits.ev_asrt_tempmid = 1;
      MemWritePciCfgEp (host, socket, ch, (DIMMTEMPSTAT_0_MCDDC_CTL_REG + (dimm * 4)), DimmTempStat.Data);
    }
  }

 for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    maxChannel = 0;
    curChannel = 0;
    mcbgfTune.Data = MemReadPciCfgMC(host, socket, mcId, MCBGFTUNE_MC_MAIN_REG);

    for (ch = 0; ch < MAX_MC_CH; ch++) {
      if ((*channelNvList)[ch + (mcId * MAX_MC_CH)].enabled == 0) continue;
      curChannel = ch;
      maxChannel++;
    }

   //HSD 5370510: for 2-channel configuration (within an IMC) includes CH2 (or CH5 for IMC1)
    if (CheckSteppingEqual(host, CPU_SKX, B0_REV_SKX) || CheckSteppingEqual(host, CPU_SKX, L0_REV_SKX)) {
      if ((curChannel == 2) && (maxChannel == 2)) {
        mcbgfTune.Bits.mccbgfd_en_3ch = 1;
        mcbgfTune.Bits.mccbgfd_en_2d = 0;
      }
    }

    //For 3-channel configurations (within an IMC)
    if (maxChannel == MAX_MC_CH) {
      mcbgfTune.Bits.mccbgfd_en_3ch = 1;
      mcbgfTune.Bits.mccbgfd_en_2d = 0;
    } // If maxChannel == 3

    MemWritePciCfgMC(host, socket, mcId, MCBGFTUNE_MC_MAIN_REG, mcbgfTune.Data);

  } // mcId loop

  //
  // Configure power management features
  //
  PowerManagementSetup(host, socket);
  tempData = 0;
  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) {
      //
      // Disable Pcode RCOMP Bit[20:19]
      //
      tempData |= (BIT0 << (mcId + 19));
    }
  } // mcId loop

  if (tempData > 0) {
    // Read PCU_MISC_CONFIG data
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG, 0);
    if (PcodeMailboxStatus == 0) {
      PCUData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);

      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "MboxStatus: %d PCU_MISC_CONFIG = 0x%x\n", PcodeMailboxStatus, PCUData));
    }

    //
    // Disable Pcode RCOMP Bit[20:19]
    //
    PCUData |= tempData;

    // Write back PCU_MISC_CONFIG data
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, (UINT32) MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG, PCUData);

    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG, 0);

    if (PcodeMailboxStatus == 0) {
      PCUData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);

      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "MBoxStatus: %d PCU_MISC_CONFIG = 0x%x\n", PcodeMailboxStatus, PCUData));
    }
  } // if (tempData > 0)

  biosResetCpl.Data = ReadCpuPciCfgEx (host, socket, 0, BIOS_RESET_CPL_PCU_FUN1_REG);
  biosResetCpl.Bits.rst_cpl2 = 1;
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n:InitMEM - BIOS_RESET_CPL: Set CPL2 on #S%d\n", socket));
  WriteCpuPciCfgEx (host, socket, 0, BIOS_RESET_CPL_PCU_FUN1_REG, biosResetCpl.Data);


  //
  // Workaround for 5332810: New definition of MSR 0x62 to merge multiple 1LM+Pmem workarounds
  //
  msrData.hi = 0;
  msrData.lo = 0;

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    memMode = CheckMemModes(host, socket, mcId);

    //
    // if Pmem enabled and 2LM disabled
    //
    if((memMode & MEM_TYPE_PMEM) && ((memMode & MEM_TYPE_2LM) == 0)) {
      msrData.lo |= (1 << mcId);
    }

  } // mcId loop

  if (!(host->var.common.emulation & SIMICS_FLAG)) {
    WriteMsrPipe(host, socket, 0x62, msrData);
  }

  return SUCCESS;
} // SwitchToNormalMode

/**

  Configure power management features

  @param host    - Pointer to sysHost
  @param socket  - Processor socket

  @retval N/A

**/
void
PowerManagementSetup (
                     PSYSHOST  host,
                     UINT8     socket
                     )
{
  UINT8                                       ch;
  struct  channelNvram                        (*channelNvList)[MAX_CH];
  DYNAMIC_PERF_POWER_CTL_PCU_FUN2_STRUCT      dynamicPerfPowerCtl;
  DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_STRUCT ddrCRCmdControls1CmdS;
  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return;

  channelNvList = GetChannelNvList(host, socket);

  // HSD 4930240
  dynamicPerfPowerCtl.Data = ReadCpuPciCfgEx(host, socket, 0, DYNAMIC_PERF_POWER_CTL_PCU_FUN2_REG);
  dynamicPerfPowerCtl.Bits.imc_apm_override_enable = 1;
  WriteCpuPciCfgEx(host, socket, 0, DYNAMIC_PERF_POWER_CTL_PCU_FUN2_REG, dynamicPerfPowerCtl.Data);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

     PmSelfRefreshProgramming(host, socket, ch);
     PmCkeProgramming(host, socket, ch);

  } // ch loop

  if (host->setup.mem.OppSrefEn == 0) {
    for (ch = 0; ch < MAX_CH; ch++) {
      ddrCRCmdControls1CmdS.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_REG);
      ddrCRCmdControls1CmdS.Bits.selectrefresh = 1;
      MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls1CmdS.Data);
    } // ch loop
  }

} // PowerManagementSetup


/**

  Gets the size of the test to be run

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)

  @retval maxMtr

**/
UINT32
GetTestSize (
            PSYSHOST host,
            UINT8    socket,
            UINT8    dimm,
            UINT8    rank
            )
{
  UINT8 ch;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  MAXMTR_MC_MAIN_STRUCT           maxMtr;
  DIMMMTR_0_MC_MAIN_STRUCT        dimmMtr;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Initiazlize to 0
  //
  maxMtr.Data = 0;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    //
    // Skip if no DIMM present
    //
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, dimm, rank, CHECK_MAPOUT)) continue;

    dimmMtr.Data = (*dimmNvList)[dimm].dimmMemTech;

    //
    // Get the maximum number of rows
    //
    if (maxMtr.Bits.maxnumrow < dimmMtr.Bits.ra_width) {
      maxMtr.Bits.maxnumrow = dimmMtr.Bits.ra_width;
    }
    //
    // Get the maximum number of columns
    //
    if (maxMtr.Bits.maxnumcol < dimmMtr.Bits.ca_width) {
      maxMtr.Bits.maxnumcol = dimmMtr.Bits.ca_width;
    }
  } // ch loop

  return maxMtr.Data;

} // GetTestSize



/**

  Clears IMC error registers

  @param host      - Pointer to sysHost

  @retval N/A

**/
void
ClearIMCErrors (
               PSYSHOST host
               )
{
} // ClearIMCErrors

void
ResetSystemEx (
            PSYSHOST host,
            UINT8    resetType
            )
/**


  Execute a platform reset

  @param host      - Pointer to sysHost
  @param resetType - Reset type

  @retval N/A

**/
{

    //
    // Reset the system
    //
    OutPort8 (host, 0xcf9, resetType);

} // ResetSystem

/**

  Programming required late in the memory initialization process

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT32
MemLate (
        PSYSHOST host
        )
{
  UINT8                             socket;
  UINT8                             mcId;
  MCMTR_MC_MAIN_STRUCT              mcMtr;
  DEVHIDE7_2_UBOX_MISC_STRUCT       devHide;
  DEFEATURES0_M2MEM_MAIN_STRUCT     m2mDefeatures0;
  SYSFEATURES0_M2MEM_MAIN_STRUCT    sysFeatures0;
  TIMEOUT_M2MEM_MAIN_STRUCT         m2mTimeout;
  TIMEOUT_M2MEM_MAIN_STRUCT         m2mTimeoutOrig;

  if (host->setup.mem.options & PAGE_POLICY) {
    for (socket = 0; socket < MAX_SOCKET; socket++) {
      // Continue if this socket is not present
      if (host->nvram.mem.socket[socket].enabled == 0) continue;

      for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {

        if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

        // Set to closed page mode
        mcMtr.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
        mcMtr.Bits.close_pg = 1;

        MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);
      } // mcId loop
    } // socket loop
  } // if closed page

  //
  // HSD 5332293 Disable m2mem timeout if both ingress bypass and prefetch are enabled.
  //
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->nvram.mem.socket[socket].enabled == 0) continue;

    for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

      m2mDefeatures0.Data = MemReadPciCfgMC (host, socket, mcId, DEFEATURES0_M2MEM_MAIN_REG);
      if (CheckSteppingLessThan (host, CPU_SKX, H0_REV_SKX)) {
        sysFeatures0.Data = MemReadPciCfgMC(host, socket, mcId, SYSFEATURES0_M2MEM_MAIN_REG);

        if ((m2mDefeatures0.Bits.ingbypdis == 0) && (sysFeatures0.Bits.prefdisable == 0)) {
          m2mTimeoutOrig.Data = m2mTimeout.Data = MemReadPciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG);
          // clear lock
          m2mTimeout.Bits.timeoutlock = 0 ;
          MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);

          // clear timeouten
          m2mTimeout.Bits.timeouten = 0;
          MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);

          // restore the lock if its there
          m2mTimeout.Bits.timeoutlock = m2mTimeoutOrig.Bits.timeoutlock;
          MemWritePciCfgMC (host, socket, mcId, TIMEOUT_M2MEM_MAIN_REG, m2mTimeout.Data);
        }
      } //CheckSteppingLessThan

      //
      // HSD 5370704: PWR prescrub on hitting uncorrectable doesnt poison the line which could
      // eventually lead to data corruption
      //
      m2mDefeatures0.Bits.badchnlfirstforpwr = 1;
      MemWritePciCfgMC (host, socket, mcId, DEFEATURES0_M2MEM_MAIN_REG, m2mDefeatures0.Data);
    } // mcId
  } // socket

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (host->nvram.mem.socket[socket].enabled == 0) {
      continue;
    }
    devHide.Data = ReadCpuPciCfgEx (host, socket, 0, DEVHIDE7_2_UBOX_MISC_REG);
    devHide.Bits.disfn |= BIT22;
    WriteCpuPciCfgEx (host, socket, 0, DEVHIDE7_2_UBOX_MISC_REG, devHide.Data);

    devHide.Data = ReadCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F7_PCU_FUN6_REG);
    devHide.Bits.disfn |= BIT22;
    WriteCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F7_PCU_FUN6_REG, devHide.Data);

    devHide.Data = ReadCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F7_VCU_FUN2_REG);
    devHide.Bits.disfn |= BIT22;
    WriteCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F7_VCU_FUN2_REG, devHide.Data);

    devHide.Data = ReadCpuPciCfgEx (host, socket, 0, DEVHIDE0_2_UBOX_MISC_REG);
    devHide.Bits.disfn |= (BIT22 + BIT23);
    WriteCpuPciCfgEx (host, socket, 0, DEVHIDE0_2_UBOX_MISC_REG, devHide.Data);

    devHide.Data = ReadCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F0_PCU_FUN6_REG);
    devHide.Bits.disfn |= (BIT22 + BIT23);
    WriteCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F0_PCU_FUN6_REG, devHide.Data);

    devHide.Data = ReadCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F0_VCU_FUN2_REG);
    devHide.Bits.disfn |= (BIT22 + BIT23);
    WriteCpuPciCfgEx (host, socket, 0, DEVHIDE_B2F0_VCU_FUN2_REG, devHide.Data);
  } // socket loop

  //
  // Indicate data is good
  //
  host->nvram.mem.DataGood = 1;

  return SUCCESS;
} // MemLate

/**

  Set the bit indicating memory initialization is complete

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
SetMemDone (
           PSYSHOST host
           )
{
  UINT8                             socket;
  UINT8                             ch;
  UINT8                             mcId;
  struct channelNvram               (*channelNvList)[MAX_CH];

  MC_INIT_STATE_G_MC_MAIN_STRUCT    mcInitStateG;
  PMONUNITCTRL_MCDDC_CTL_STRUCT     pmonUnitCtrl;
  PMONCNTRCFG_4_MCDDC_CTL_STRUCT pmonCntrCfg;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Continue if this socket is not present
    //
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;

    channelNvList = GetChannelNvList(host, socket);

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      //
      // Reset counters
      //
      pmonUnitCtrl.Data = 0;
      pmonUnitCtrl.Bits.resetcounterconfigs = 1;
      pmonUnitCtrl.Bits.resetcounters = 1;
      MemWritePciCfgEp (host, socket, ch, PMONUNITCTRL_MCDDC_CTL_REG, pmonUnitCtrl.Data);

      //
      // Enable error counter
      //
      pmonCntrCfg.Data = 0;
      pmonCntrCfg.Bits.eventselect = 0x9;
      pmonCntrCfg.Bits.counterenable = 1;
      MemWritePciCfgEp (host, socket, ch, PMONCNTRCFG_4_MCDDC_CTL_REG, pmonCntrCfg.Data);

    } // ch loop

    //
    // Set MRC_DONE
    //
    for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
      if(host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

      mcInitStateG.Data = MemReadPciCfgMC (host, socket, mcId, MC_INIT_STATE_G_MC_MAIN_REG);
      mcInitStateG.Bits.mrc_done  = 1;
      MemWritePciCfgMC (host, socket, mcId, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);
    }

  } // socket loop
} // SetMemDone

/**

  Returns the physical rank number

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT8
GetPhyRank (
           UINT8 dimm,
           UINT8 rank
           )
{
  return(dimm * 4) + rank;
} // GetPhyRank


/**

    Initialize ADR

    @param host - Pointer to SysHost

    @retval SUCCESS

**/
UINT32
InitADR(
       PSYSHOST  host
       )
{
  volatile UINT32            *pointer;
  PM_ADR_MCDDC_CTL_STRUCT    pmAdrCtrl;
  struct channelNvram        (*channelNvList)[MAX_CH];
  struct dimmNvram           (*dimmNvList)[MAX_DIMM];
  UINT8                      socket;
  UINT8                      ch;
  UINT8                      dimm;
  PCU_BIOS_SPARE2_PCU_FUN3_STRUCT           pcuBiosSpare2Reg;

  // Clear ADR_RST_STS on all boots regardless of if ADR is enabled
  // or not to make sure ADR_COMPLETE works correct.
  if (!host->var.common.resetRequired) {
#ifdef SERIAL_DBG_MSG
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "No Pending Reset, clearing the ADR status bit\n"));
#endif // SERIAL_DBG_MSG
    pointer = (UINT32 *)(host->var.common.pwrmBase + SSB_PM_STS); // PCH_PM_STS
    *pointer = ADR_RST_STS;
  }

  for (socket = 0; socket < MAX_SOCKET; socket++) { //HSD s5371498
    channelNvList = GetChannelNvList(host, socket);
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled) {
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          if ( (host->setup.mem.ADREn) && (((host->var.mem.volMemMode == VOL_MEM_MODE_2LM) || (host->var.mem.volMemMode == VOL_MEM_MODE_1LM)) && ((*dimmNvList)[dimm].aepDimmPresent)) ){
            pmAdrCtrl.Data = MemReadPciCfgEp (host, socket, ch, PM_ADR_MCDDC_CTL_REG);
            pmAdrCtrl.Bits.num_pwr_fail_cmds = 1;
            pmAdrCtrl.Bits.ddrt_adr_en = 1;
            MemWritePciCfgEp (host, socket, ch, PM_ADR_MCDDC_CTL_REG, pmAdrCtrl.Data);

  #ifdef SERIAL_DBG_MSG

            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "%d:%d   Set ddrt_adr_en = 1\n", socket, ch));

  #endif // SERIAL_DBG_MSG
          }
        }
      }
    } // ch loop
  } // socket loop

  if (host->setup.mem.ADREn) {

#ifdef SERIAL_DBG_MSG

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Enabling ADR\n"));

#endif // SERIAL_DBG_MSG

   // Note: All the chipset initialization code has been done in Chipset Package.
   // Only the CPU/Pcode related settings are done here.
    // Legacy ADR Enable/Disable
    if (!(host->setup.mem.LegacyADRModeEn)) {
      for (socket = 0; socket < MAX_SOCKET; socket++) {
        if (host->var.common.socketPresentBitMap & (BIT0 << socket)) {
          pcuBiosSpare2Reg.Data = ReadCpuPciCfgEx(host, socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG);
          pcuBiosSpare2Reg.Data |= BIT18;     //when set, pcode will flush IIO cache.
          WriteCpuPciCfgEx(host, socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG, pcuBiosSpare2Reg.Data);
        }
      }
    }
#ifdef SERIAL_DBG_MSG
    if (host->setup.mem.LegacyADRModeEn){
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "ADR Enabled (LegacyMode)"));
    } else {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "ADR Enabled (Non-LegacyMode)"));
    }
#endif // SERIAL_DBG_MSG
  }

  return SUCCESS;
}

/**

  Returns the DDR frequency supported by this config

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param vdd   - Vdd (1.5v or 1.35V)

  @retval Supported DDR frequency

**/
UINT8
GetPORDDRFreq (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     vdd
              )
{
  UINT8 dimm;
  UINT8 totalRanks = 0;
  UINT8 porTableIndex;
  UINT8 ddrFreq = NOT_SUP;
  UINT8 dimmType;
  UINT8 socketType;
  UINT8 aepPresent = 0;
  UINT16 freqTableLength;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct DimmPOREntryStruct *freqTablePtr;

  freqTablePtr = &freqTable[0];
  freqTableLength = sizeof(freqTable)/sizeof(freqTable[0]);
  CoreLookupFreqTable(host, &freqTablePtr, &freqTableLength);

  channelNvList = GetChannelNvList(host, socket);

  dimmNvList = GetDimmNvList(host, socket, ch);

  //
  // Default to RDIMM or UDIMM
  //
  dimmType = host->nvram.mem.dimmTypePresent;

  // Loop for each DIMM
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

    // Skip if no DIMM present
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    totalRanks += (*dimmNvList)[dimm].numDramRanks;

    // change to LRDIMM
    if (IsLrdimmPresent(host, socket, ch, dimm)){
      dimmType = LRDIMM;
    }
    if ((*dimmNvList)[dimm].aepDimmPresent) {
      aepPresent = 1;
    }
  } // dimm loop

  // change to NVMDIMM
  if (aepPresent) dimmType = NVMDIMM;

  //
  // Check socket type
  //
  if ((host->nvram.mem.dimmTypePresent == UDIMM) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
    socketType = SOCKET_HEDT;
  } else {
    socketType = host->var.common.socketType;
  }

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Memory Configuration: Vdd = %d, procType = 0x%x, socketType = %d, dramType = %d, spc = %d, dpc = %d, dimmType = %d, numRanks = %d\n",
      vdd,
      host->var.common.cpuFamily,
      socketType,
      host->nvram.mem.dramType,
      host->var.mem.socket[socket].channelList[ch].numDimmSlots,
      (*channelNvList)[ch].maxDimm,
      dimmType,
      totalRanks));

  for (porTableIndex = 0; porTableIndex < freqTableLength; porTableIndex++) {
    if ((freqTablePtr[porTableIndex].procType == host->var.common.cpuFamily) &&
        (freqTablePtr[porTableIndex].socketType == socketType) &&
        (freqTablePtr[porTableIndex].dramType == host->nvram.mem.dramType) &&
        (freqTablePtr[porTableIndex].spc == host->var.mem.socket[socket].channelList[ch].numDimmSlots) &&
        (freqTablePtr[porTableIndex].dpc == (*channelNvList)[ch].maxDimm) &&
        (freqTablePtr[porTableIndex].dimmType == dimmType) &&
        (freqTablePtr[porTableIndex].numRanks >= totalRanks) && // could make this exact in the future, if POR table included all possible rank count combinations
        (freqTablePtr[porTableIndex].vdd == vdd)) {

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          " Table Index: %d\n", porTableIndex));
      if (host->setup.mem.enforcePOR == ENFORCE_POR_EN) {
        // POR setting
        ddrFreq = freqTablePtr[porTableIndex].porFreq;
      } else if (host->setup.mem.enforcePOR == ENFORCE_STRETCH_EN) {
        // Stretch Goal
        ddrFreq = freqTablePtr[porTableIndex].stretchFreq;
      }
      break;
    }
  } // porTableIndex

  //
  // Check if we found a valid config
  //
  if (porTableIndex == freqTableLength) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Configuration not in POR table!\n"));
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Memory Configuration: Vdd = %d, procType = 0x%x, socketType = %d, dramType = %d, spc = %d, dpc = %d, dimmType = %d, numRanks = %d\n",
      vdd,
      host->var.common.cpuFamily,
      socketType,
      host->nvram.mem.dramType,
      host->var.mem.socket[socket].channelList[ch].numDimmSlots,
      (*channelNvList)[ch].maxDimm,
      dimmType,
      totalRanks));
  }

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "GetPORDDRFreq returns ddrfreq = %d\n", ddrFreq));

  return ddrFreq;
} // GetPORDDRFreq

/**

  Checks if the requested Vdd is supported

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param ch    - Channel number (0-based)
  @param vdd   - Vdd (1.5v or 1.35V)

  @retval Supported DDR frequency

**/
UINT8
CheckPORDDRVdd (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     vdd
               )
{
  UINT8 porTableIndex;
  UINT8 dimmType;
  UINT8 status = FAILURE;
  UINT8 maxRanks = 0;
  UINT8 dimm = 0;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct DimmPOREntryStruct *freqTablePtr;
  UINT16 freqTableLength;

  freqTablePtr = &freqTable[0];
  freqTableLength = sizeof(freqTable)/sizeof(freqTable[0]);
  OemLookupFreqTable(host, &freqTablePtr, &freqTableLength);

  dimmNvList = GetDimmNvList(host, socket, ch);
  channelNvList = GetChannelNvList(host, socket);

  // Loop for each DIMM
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {

    // Skip if no DIMM present
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    if (maxRanks < (*dimmNvList)[dimm].numDramRanks) {
      maxRanks = (*dimmNvList)[dimm].numDramRanks;
    }
  } // dimm loop

  //
  // Default to RDIMM or UDIMM
  //
  dimmType = host->nvram.mem.dimmTypePresent;

  //
  // Check for LR-DIMM
  //
  if (host->nvram.mem.socket[socket].lrDimmPresent) {
    dimmType = LRDIMM;
  }

  for (porTableIndex = 0; porTableIndex < freqTableLength; porTableIndex++) {
    if ((freqTablePtr[porTableIndex].procType == host->var.common.cpuFamily) &&
        (freqTablePtr[porTableIndex].socketType == host->var.common.socketType) &&
        (freqTablePtr[porTableIndex].spc == host->var.mem.socket[socket].channelList[ch].numDimmSlots) &&
        (freqTablePtr[porTableIndex].dpc == (*channelNvList)[ch].maxDimm) &&
        (freqTablePtr[porTableIndex].numRanks >= maxRanks) &&
        (freqTablePtr[porTableIndex].dimmType == dimmType)) {

      //
      // Found the current configuration in the table. Check to see if there is a DDR frequency available.
      //
      if ((host->setup.mem.enforcePOR == ENFORCE_POR_EN) && (freqTablePtr[porTableIndex].porFreq == NOT_SUP)) {
        // Break if there is no valid POR frequency for this config
        break;
      } else if ((host->setup.mem.enforcePOR == ENFORCE_STRETCH_EN)  && (freqTablePtr[porTableIndex].stretchFreq == NOT_SUP)) {
        // Break if there is no valid stretch frequency for this config
        break;
      }

      //
      // Check if the desired Vdd is supported with this config. Break if yes. Keep looking if no.
      //
      if(freqTablePtr[porTableIndex].vdd & vdd) {
        status = SUCCESS;
        break;
      }
    }
  } // porTableIndex

  return status;
} // CheckPORDDRVdd


/**

  this function writes to CADB

  @param host            - include all the MRC data
  @param socket             - Memory Controller
  @param ch              - ch to send command to
  @param rank            - CS to send the command to

**/
void
ReadMprCADBSeq (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     dimm,
               UINT8     rank,
               UINT8     side,
               UINT8     mprPage,
               UINT8     mprLoc
               )
{
  UINT8                                   count;
  UINT8                                   i;
  UINT32                                  CRValueSave = 0;
  UINT8                                   bankMR3;
  UINT16                                  dataMR3;
  UINT8                                   bank;
  UINT16                                  data;
  UINT8                                   slot;
  struct dimmNvram                        (*dimmNvList)[MAX_DIMM];
  struct ddrRank                          (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                       (*rankStruct)[MAX_RANK_DIMM];
  PatCadbProg0                            patCADBProg0[10];
  PatCadbProg1                            patCADBProg1[10];
  CPGC_PATCADBMRS_MCDDC_CTL_STRUCT        patCADBMrs;
  CPGC_SEQCTL0_MC_MAIN_STRUCT             seqCtl0;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT        cpgcPatCadbCtlReg;
  struct channelNvram                     (*channelNvList)[MAX_CH];
  // CS  , Control, ODT,  CKE, Par
  PatCadbProg1 DeselectPattern1[CADB_LINES] = { { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 0
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 1
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 2
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 3
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 4
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 5
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 6
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 7
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 8
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 9
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 10
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 11
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 12
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 13
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 14
                                                { 0x3FF, 0xF, 0x0, 0x3F, 1 }, // Row 15
                                              };

  if (host->var.mem.socket[socket].hostRefreshStatus == 1) {
    FixedDelay(host, 10);
  }

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);
  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  //
  // Pointer will be dynamically incremented after a write to CADB_PROG register
  //
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, 0);
  //
  // Save before MR command
  //
  CRValueSave = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Data                      = 0;
  seqCtl0.Bits.initialization_mode  = MRS_Mode;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  // Command 0 - MR3 write to side B if present, or side A if not
  slot = 0;
  if (host->var.mem.socket[socket].hostRefreshStatus) {
    //
    // Placeholder for deselect on entry
    //
    slot = slot + 1;
  }

  dataMR3 = (*rankStruct)[rank].MR3 | MR3_MPR | mprPage;
  bankMR3 = BANK3;
  //
  // Mirror address/bank bits
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    dataMR3  = MirrorAddrBits (host, dataMR3);
    bankMR3  = MirrorBankBits (host, bankMR3);
  }
  //
  // Invert address/bank bits
  //
  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    dataMR3  = InvertAddrBits (dataMR3);
    bankMR3  = InvertBankBits (bankMR3);
  }

  patCADBProg0[slot].address = dataMR3;
  patCADBProg0[slot].bank    = bankMR3;
  patCADBProg0[slot].pdatag  = NO_PDA;
  patCADBProg0[slot].cid     = 0;
  if ((IsLrdimmPresent(host, socket, ch, dimm) && (*channelNvList)[ch].encodedCSMode == 1)) {
    // here we are in encoded quad CS mode
    patCADBProg0[slot].cid     = (rank >> 1) | 0x6;
  }
  patCADBProg1[slot].cs      = ~(1 << (*rankList)[rank].CSIndex) & ((1 << 11) - 1);
  patCADBProg1[slot].control = MRS_CMD;

  //
  // Assert ACT bit
  //
  patCADBProg1[slot].control |= BIT3;

  //
  // Calculate parity
  //
  count = 0;

  // Count the 1's in CID bits if they are to be included in the parity calculation
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    if (((*channelNvList)[ch].cidBitMap & BIT2) && (patCADBProg0[slot].cid & BIT2))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT1) && (patCADBProg0[slot].cid & BIT1))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT0) && (patCADBProg0[slot].cid & BIT0))
      count++;
  }

  // Count the 1's in the address bits
  for (i = 0; i < 17; i++) {
    if (patCADBProg0[slot].address & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the bank bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg0[slot].bank & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the control bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg1[slot].control & (1 << i)) {
      count++;
    }
  } // i loop

  if (count & BIT0) {
    patCADBProg1[slot].par = 1;
  } else {
    patCADBProg1[slot].par = 0;
  }

  patCADBProg1[slot].cke = 0x3F;
  patCADBProg1[slot].odt = 0;

  patCADBProg0[slot].address = dataMR3;
  patCADBProg0[slot].bank    = bankMR3;
  patCADBProg0[slot].pdatag  = NO_PDA;
  patCADBProg0[slot].cid     = 0;
  if ((IsLrdimmPresent(host, socket, ch, dimm) && (*channelNvList)[ch].encodedCSMode == 1)) {
    // here we are in encoded quad CS mode
    patCADBProg0[slot].cid     = (rank >> 1) | 0x6;
  }
  patCADBProg1[slot].cs      = ~(1 << (*rankList)[rank].CSIndex) & ((1 << 11) - 1);
  patCADBProg1[slot].control = MRS_CMD;

  //
  // Assert ACT bit
  //
  patCADBProg1[slot].control |= BIT3;

  // Command 1 - MR3 write to side A
  slot = slot + 1;
  dataMR3 = (*rankStruct)[rank].MR3 | MR3_MPR | mprPage;
  bankMR3 = BANK3;

  //
  // Mirror address/bank bits
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    dataMR3  = MirrorAddrBits (host, dataMR3);
    bankMR3  = MirrorBankBits (host, bankMR3);
  }

  patCADBProg0[slot].address = dataMR3;
  patCADBProg0[slot].bank    = bankMR3;
  patCADBProg0[slot].pdatag  = NO_PDA;
  patCADBProg0[slot].cid     = 0;
  if (IsLrdimmPresent(host, socket, ch, dimm) && (*channelNvList)[ch].encodedCSMode == 1) {
    // here we are in encoded quad CS mode
    patCADBProg0[slot].cid     = (rank >> 1) | 0x6;
  }
  patCADBProg1[slot].cs      = ~(1 << (*rankList)[rank].CSIndex) & ((1 << 11) - 1);
  patCADBProg1[slot].control = MRS_CMD;

  //
  // Assert ACT bit
  //
  patCADBProg1[slot].control |= BIT3;

  //
  // Calculate parity
  //
  count = 0;

  // Count the 1's in CID bits if they are to be included in the parity calculation
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    if (((*channelNvList)[ch].cidBitMap & BIT2) && (patCADBProg0[slot].cid & BIT2))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT1) && (patCADBProg0[slot].cid & BIT1))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT0) && (patCADBProg0[slot].cid & BIT0))
      count++;
  }

  // Count the 1's in the address bits
  for (i = 0; i < 17; i++) {
    if (patCADBProg0[slot].address & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the bank bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg0[slot].bank & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the control bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg1[slot].control & (1 << i)) {
      count++;
    }
  } // i loop

  if (count & BIT0) {
    patCADBProg1[slot].par = 1;
  } else {
    patCADBProg1[slot].par = 0;
  }

  patCADBProg1[slot].cke = 0x3F;
  patCADBProg1[slot].odt = 0;

  // Command 2 - RD from given side
  slot = slot + 1;
  data = 0;
  bank = mprLoc;

  //
  // Mirror address/bank bits
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    data  = MirrorAddrBits (host, data);
    bank  = MirrorBankBits (host, bank);
  }

  //
  // Invert address/bank bits
  //
  if (side == SIDE_B) {
    data  = InvertAddrBits (data);
    bank  = InvertBankBits (bank);
  }

  patCADBProg0[slot].address = data;
  patCADBProg0[slot].bank    = bank;
  patCADBProg0[slot].pdatag  = NO_PDA;
  patCADBProg0[slot].cid     = 0;
  if (IsLrdimmPresent(host, socket, ch, dimm) && (*channelNvList)[ch].encodedCSMode == 1) {
    // here we are in encoded quad CS mode
    patCADBProg0[slot].cid     = (rank >> 1) | 0x6;
  }
  patCADBProg1[slot].cs      = ~(1 << (*rankList)[rank].CSIndex) & ((1 << 11) - 1);
  patCADBProg1[slot].control = RD_CMD;

  //
  // Assert ACT bit
  //
  patCADBProg1[slot].control |= BIT3;

  //
  // Calculate parity
  //
  count = 0;

  // Count the 1's in CID bits if they are to be included in the parity calculation
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    if (((*channelNvList)[ch].cidBitMap & BIT2) && (patCADBProg0[slot].cid & BIT2))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT1) && (patCADBProg0[slot].cid & BIT1))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT0) && (patCADBProg0[slot].cid & BIT0))
      count++;
  }

  // Count the 1's in the address bits
  for (i = 0; i < 17; i++) {
    if (patCADBProg0[slot].address & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the bank bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg0[slot].bank & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the control bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg1[slot].control & (1 << i)) {
      count++;
    }
  } // i loop

  if (count & BIT0) {
    patCADBProg1[slot].par = 1;
  } else {
    patCADBProg1[slot].par = 0;
  }

  patCADBProg1[slot].cke = 0x3F;
  patCADBProg1[slot].odt = 0;

  // Command 3 - MR3 write to side B if present, or side A if not
  slot = slot + 1;
  dataMR3  = (*rankStruct)[rank].MR3;
  bankMR3 = BANK3;

  //
  // Mirror address/bank bits
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    dataMR3  = MirrorAddrBits (host, dataMR3);
    bankMR3  = MirrorBankBits (host, bankMR3);
  }

  //
  // Invert address/bank bits
  //
  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    dataMR3  = InvertAddrBits (dataMR3);
    bankMR3  = InvertBankBits (bankMR3);
  }

  patCADBProg0[slot].address = dataMR3;
  patCADBProg0[slot].bank    = bankMR3;
  patCADBProg0[slot].pdatag  = NO_PDA;
  patCADBProg0[slot].cid     = 0;

  patCADBProg1[slot].cs      = ~(1 << (*rankList)[rank].CSIndex) & ((1 << 11) - 1);
  patCADBProg1[slot].control = MRS_CMD;

  //
  // Assert ACT bit
  //
  patCADBProg1[slot].control |= BIT3;

  //
  // Calculate parity
  //
  count = 0;

  // Count the 1's in CID bits if they are to be included in the parity calculation
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    if (((*channelNvList)[ch].cidBitMap & BIT2) && (patCADBProg0[slot].cid & BIT2))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT1) && (patCADBProg0[slot].cid & BIT1))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT0) && (patCADBProg0[slot].cid & BIT0))
      count++;
  }

  // Count the 1's in the address bits
  for (i = 0; i < 17; i++) {
    if (patCADBProg0[slot].address & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the bank bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg0[slot].bank & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the control bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg1[slot].control & (1 << i)) {
      count++;
    }
  } // i loop

  if (count & BIT0) {
    patCADBProg1[slot].par = 1;
  } else {
    patCADBProg1[slot].par = 0;
  }

  patCADBProg1[slot].cke = 0x3F;
  patCADBProg1[slot].odt = 0;

  // Command 4 - MR3 write to side A
  slot = slot + 1;
  dataMR3  = (*rankStruct)[rank].MR3;
  bankMR3 = BANK3;

  //
  // Mirror address/bank bits
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    dataMR3  = MirrorAddrBits (host, dataMR3);
    bankMR3  = MirrorBankBits (host, bankMR3);
  }

  patCADBProg0[slot].address = dataMR3;
  patCADBProg0[slot].bank    = bankMR3;
  patCADBProg0[slot].pdatag  = NO_PDA;
  patCADBProg0[slot].cid     = 0;
  if (IsLrdimmPresent(host, socket, ch, dimm) && (*channelNvList)[ch].encodedCSMode) {
    // here we are in encoded quad CS mode
    patCADBProg0[slot].cid     = (rank >> 1) | 0x6;
  }
  patCADBProg1[slot].cs      = ~(1 << (*rankList)[rank].CSIndex) & ((1 << 11) - 1);
  patCADBProg1[slot].control = MRS_CMD;

  //
  // Assert ACT bit
  //
  patCADBProg1[slot].control |= BIT3;

  //
  // Calculate parity
  //
  count = 0;

  // Count the 1's in CID bits if they are to be included in the parity calculation
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    if (((*channelNvList)[ch].cidBitMap & BIT2) && (patCADBProg0[slot].cid & BIT2))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT1) && (patCADBProg0[slot].cid & BIT1))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT0) && (patCADBProg0[slot].cid & BIT0))
      count++;
  }

  // Count the 1's in the address bits
  for (i = 0; i < 17; i++) {
    if (patCADBProg0[slot].address & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the bank bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg0[slot].bank & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the control bits
  for (i = 0; i < 4; i++) {
    if (patCADBProg1[slot].control & (1 << i)) {
      count++;
    }
  } // i loop

  if (count & BIT0) {
    patCADBProg1[slot].par = 1;
  } else {
    patCADBProg1[slot].par = 0;
  }

  patCADBProg1[slot].cke = 0x3F;
  patCADBProg1[slot].odt = 0;

  if (host->var.mem.socket[socket].hostRefreshStatus) {
    //
    // Placeholder for deselect on exit
    //
    slot = slot + 1;
  }
  // Program commands into CADB

  ProgramCADB (host, socket, ch, 0, slot + 1, patCADBProg0, patCADBProg1);

  if (host->var.mem.socket[socket].hostRefreshStatus) {
    //
    // Add a deselect
    //
    ProgramCADB(host, socket, ch, 0, 1, &patCADBProg0[0], &DeselectPattern1[0]);
    ProgramCADB(host, socket, ch, slot, slot + 1, &patCADBProg0[0], &DeselectPattern1[0]);
  }

  //
  // Execute MRS Mode
  //
  patCADBMrs.Data                     = 0;
  if (host->var.mem.socket[socket].hostRefreshStatus == 1) {
    patCADBMrs.Bits.mrs_gap = 9;
  }
  else {
    patCADBMrs.Bits.mrs_gap = 5;
  }
  patCADBMrs.Bits.mrs_start_pointer = 0;
  patCADBMrs.Bits.mrs_end_pointer = slot;
  MemWritePciCfgEp(host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

  // Start test and clear errors
  seqCtl0.Bits.local_clear_errs  = 1;
  seqCtl0.Bits.local_start_test  = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  //
  // Poll test completion
  //
  CpgcPollGlobalTestDone (host, socket, 1 << ch);

  //
  // Restore after MR command
  //
  seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Bits.local_start_test = 0;
  seqCtl0.Bits.local_clear_errs = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, CRValueSave);

} // ReadMprCADBSeq

/**

  Reads MPR page/location from given rank and updates associated MprData per DRAM device

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param MprData - each byte is updated with UI[0-7] data per DRAM device

**/
void
ReadMprRankDdr4 (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT8     dimm,
                UINT8     rank,
                UINT8     mprPage,
                UINT8     mprLoc,
                UINT8     MprData[MAX_STROBE]
                )
{
  MR_READ_RESULT_DQ12TO0_MCDDC_DP_STRUCT  mrReadResultDq12to0;
  MR_READ_RESULT_DQ28TO16_MCDDC_DP_STRUCT mrReadResultDq28to16;
  MR_READ_RESULT_DQ44TO32_MCDDC_DP_STRUCT mrReadResultDq44to32;
  MR_READ_RESULT_DQ60TO48_MCDDC_DP_STRUCT mrReadResultDq60to48;
  MR_READ_RESULT_DQ68TO64_MCDDC_DP_STRUCT mrReadResultDq68to64;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];


  dimmNvList    = GetDimmNvList(host, socket, ch);

  ReadMprCADBSeq (host, socket, ch, dimm, rank, SIDE_A, mprPage, mprLoc);

  // Get side A results
  mrReadResultDq12to0.Data  = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ12TO0_MCDDC_DP_REG);
  mrReadResultDq28to16.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ28TO16_MCDDC_DP_REG);
  mrReadResultDq44to32.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ44TO32_MCDDC_DP_REG);
  mrReadResultDq60to48.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ60TO48_MCDDC_DP_REG);
  mrReadResultDq68to64.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ68TO64_MCDDC_DP_REG);

  MprData[0] = (UINT8)((mrReadResultDq12to0.Data  >> 0)  & 0xFF);
  MprData[1] = (UINT8)((mrReadResultDq12to0.Data  >> 16) & 0xFF);
  MprData[2] = (UINT8)((mrReadResultDq28to16.Data >> 0)  & 0xFF);
  MprData[3] = (UINT8)((mrReadResultDq28to16.Data >> 16) & 0xFF);
  MprData[4] = (UINT8)((mrReadResultDq44to32.Data >> 0)  & 0xFF);
  MprData[5] = (UINT8)((mrReadResultDq44to32.Data >> 16) & 0xFF);
  MprData[6] = (UINT8)((mrReadResultDq60to48.Data >> 0)  & 0xFF);
  MprData[7] = (UINT8)((mrReadResultDq60to48.Data >> 16) & 0xFF);
  MprData[8] = (UINT8)((mrReadResultDq68to64.Data >> 0)  & 0xFF);

  if ((*dimmNvList)[dimm].x4Present) {

    MprData[9]  = (UINT8)((mrReadResultDq12to0.Data  >> 8)  & 0xFF);
    MprData[10] = (UINT8)((mrReadResultDq12to0.Data  >> 24) & 0xFF);
    MprData[11] = (UINT8)((mrReadResultDq28to16.Data >> 8)  & 0xFF);
    MprData[12] = (UINT8)((mrReadResultDq28to16.Data >> 24) & 0xFF);
    MprData[13] = (UINT8)((mrReadResultDq44to32.Data >> 8)  & 0xFF);
    MprData[14] = (UINT8)((mrReadResultDq44to32.Data >> 24) & 0xFF);
    MprData[15] = (UINT8)((mrReadResultDq60to48.Data >> 8)  & 0xFF);
    MprData[16] = (UINT8)((mrReadResultDq60to48.Data >> 24) & 0xFF);
    MprData[17] = (UINT8)((mrReadResultDq68to64.Data >> 8)  & 0xFF);

  } //x4

  if (host->nvram.mem.dimmTypePresent == RDIMM) {

    ReadMprCADBSeq (host, socket, ch, dimm, rank, SIDE_B, mprPage, mprLoc);

    // Get side B results
    mrReadResultDq12to0.Data  = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ12TO0_MCDDC_DP_REG);
    mrReadResultDq28to16.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ28TO16_MCDDC_DP_REG);
    mrReadResultDq44to32.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ44TO32_MCDDC_DP_REG);
    mrReadResultDq60to48.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ60TO48_MCDDC_DP_REG);
    mrReadResultDq68to64.Data = MemReadPciCfgEp (host, socket, ch, MR_READ_RESULT_DQ68TO64_MCDDC_DP_REG);

    // Update results
    // TODO: implement side B discovery of DRAM mapping
    MprData[4] = (UINT8)((mrReadResultDq44to32.Data >> 0)  & 0xFF);
    MprData[5] = (UINT8)((mrReadResultDq44to32.Data >> 16) & 0xFF);
    MprData[6] = (UINT8)((mrReadResultDq60to48.Data >> 0)  & 0xFF);
    MprData[7] = (UINT8)((mrReadResultDq60to48.Data >> 16) & 0xFF);

    if ((*dimmNvList)[dimm].x4Present) {

      MprData[13] = (UINT8)((mrReadResultDq44to32.Data >> 8)  & 0xFF);
      MprData[14] = (UINT8)((mrReadResultDq44to32.Data >> 24) & 0xFF);
      MprData[15] = (UINT8)((mrReadResultDq60to48.Data >> 8)  & 0xFF);
      MprData[16] = (UINT8)((mrReadResultDq60to48.Data >> 24) & 0xFF);

    } //x4
  } // RDIMM

} // ReadMprRankDdr4

#ifdef LRDIMM_SUPPORT
/**

  Reads MPR page/location from given rank and updates associated MprData per DRAM device

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param MprData - each byte is updated with UI[0-7] data per DRAM device

**/
void
ReadBcwDimmDdr4 (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT8     dimm,
                UINT8     bcwFunc,
                UINT8     bcwByte,
                UINT8     bcwNibble,
                UINT8     MprData[MAX_STROBE]
                )
{
  MR_READ_RESULT_DQ12TO0_MCDDC_DP_STRUCT  mrReadResultDq12to0;
  MR_READ_RESULT_DQ28TO16_MCDDC_DP_STRUCT mrReadResultDq28to16;
  MR_READ_RESULT_DQ44TO32_MCDDC_DP_STRUCT mrReadResultDq44to32;
  MR_READ_RESULT_DQ60TO48_MCDDC_DP_STRUCT mrReadResultDq60to48;
  MR_READ_RESULT_DQ68TO64_MCDDC_DP_STRUCT mrReadResultDq68to64;
  DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_STRUCT daDdrtTrainingRc06;
  DA_DDRT_TRAINING_RC4X_FNV_DA_UNIT_0_STRUCT daDdrtTrainingRc4x;
  DA_DDRT_TRAINING_RC6X_FNV_DA_UNIT_0_STRUCT daDdrtTrainingRc6x;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  UINT8 controlWordData;
  DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_STRUCT daDdrtDqBcomCtrl;

  dimmNvList    = GetDimmNvList(host, socket, ch);

  // Montage requires BCW function select inside the BCW Read protocol and does not use BC7x for BCW Read
  // Write BC7x = 0 to be on safe side.
  WriteLrbuf(host, socket, ch, dimm, 0, 0, 0, LRDIMM_BC7x);

  if (!(*dimmNvList)[dimm].aepDimmPresent) {

    // CW Source selection
    controlWordData = (bcwFunc << 5) | BIT4 | bcwByte;
    WriteRC(host, socket, ch, dimm, 0, controlWordData, RDIMM_RC4x);

    // CW Source selection
    controlWordData = bcwNibble << 4;
    WriteRC (host, socket, ch, dimm, 0, controlWordData, RDIMM_RC6x);

    // CW Read operation
    controlWordData = 4;    //CMD 4 - CW Read Operation
    WriteRC (host, socket, ch, dimm, 0, controlWordData, RDIMM_RC06);

  } else {

    //
    // Enable Far Memory Controller to automatically respond with REQ# on read command
    //
    daDdrtDqBcomCtrl.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG);
    if ((*dimmNvList)[dimm].fmcType == FMC_FNV_TYPE) {
      daDdrtDqBcomCtrl.Data |= BIT12;
    } else {
      daDdrtDqBcomCtrl.Bits.bcw_mpr_rd_req_en = 1;
    }
    WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG, daDdrtDqBcomCtrl.Data);

    daDdrtTrainingRc4x.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC4X_FNV_DA_UNIT_0_REG);
    daDdrtTrainingRc4x.Bits.ad_12_8 = BIT4 | bcwByte;
    daDdrtTrainingRc4x.Bits.func = bcwFunc;
    WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC4X_FNV_DA_UNIT_0_REG, daDdrtTrainingRc4x.Data);

    daDdrtTrainingRc6x.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC6X_FNV_DA_UNIT_0_REG);
    daDdrtTrainingRc6x.Bits.ad_7_0 = bcwNibble << 4;
    WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC6X_FNV_DA_UNIT_0_REG, daDdrtTrainingRc6x.Data);

    daDdrtTrainingRc06.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_REG);
    daDdrtTrainingRc06.Bits.misc_ctrl = 4;
    WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_REG, daDdrtTrainingRc06.Data);
  }
  if ((*dimmNvList)[dimm].aepDimmPresent) {

    //
    // Read the data
    //
    FnvBcwRead (host, socket, ch, dimm, 0, MprData);

    //
    // Restore FNV settings
    //
    daDdrtTrainingRc06.Bits.misc_ctrl = 0;
    WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_REG, daDdrtTrainingRc06.Data);
    if ((*dimmNvList)[dimm].fmcType == FMC_FNV_TYPE) {
      daDdrtDqBcomCtrl.Data &= ~BIT12;
    } else {
      daDdrtDqBcomCtrl.Bits.bcw_mpr_rd_req_en = 0;
    }
    WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG, daDdrtDqBcomCtrl.Data);

  } else {
    //
    // Read the data
    //
    ReadMprCADBSeq(host, socket, ch, dimm, 0, SIDE_A, 0, 0);

    // Get side A results
    mrReadResultDq12to0.Data = MemReadPciCfgEp(host, socket, ch, MR_READ_RESULT_DQ12TO0_MCDDC_DP_REG);
    mrReadResultDq28to16.Data = MemReadPciCfgEp(host, socket, ch, MR_READ_RESULT_DQ28TO16_MCDDC_DP_REG);
    mrReadResultDq44to32.Data = MemReadPciCfgEp(host, socket, ch, MR_READ_RESULT_DQ44TO32_MCDDC_DP_REG);
    mrReadResultDq60to48.Data = MemReadPciCfgEp(host, socket, ch, MR_READ_RESULT_DQ60TO48_MCDDC_DP_REG);
    mrReadResultDq68to64.Data = MemReadPciCfgEp(host, socket, ch, MR_READ_RESULT_DQ68TO64_MCDDC_DP_REG);

    MprData[0] = (UINT8)((mrReadResultDq12to0.Data >> 0) & 0xFF);
    MprData[1] = (UINT8)((mrReadResultDq12to0.Data >> 16) & 0xFF);
    MprData[2] = (UINT8)((mrReadResultDq28to16.Data >> 0) & 0xFF);
    MprData[3] = (UINT8)((mrReadResultDq28to16.Data >> 16) & 0xFF);
    MprData[4] = (UINT8)((mrReadResultDq44to32.Data >> 0) & 0xFF);
    MprData[5] = (UINT8)((mrReadResultDq44to32.Data >> 16) & 0xFF);
    MprData[6] = (UINT8)((mrReadResultDq60to48.Data >> 0) & 0xFF);
    MprData[7] = (UINT8)((mrReadResultDq60to48.Data >> 16) & 0xFF);
    MprData[8] = (UINT8)((mrReadResultDq68to64.Data >> 0) & 0xFF);

    if ((*dimmNvList)[dimm].x4Present) {

      MprData[9] = (UINT8)((mrReadResultDq12to0.Data >> 8) & 0xFF);
      MprData[10] = (UINT8)((mrReadResultDq12to0.Data >> 24) & 0xFF);
      MprData[11] = (UINT8)((mrReadResultDq28to16.Data >> 8) & 0xFF);
      MprData[12] = (UINT8)((mrReadResultDq28to16.Data >> 24) & 0xFF);
      MprData[13] = (UINT8)((mrReadResultDq44to32.Data >> 8) & 0xFF);
      MprData[14] = (UINT8)((mrReadResultDq44to32.Data >> 24) & 0xFF);
      MprData[15] = (UINT8)((mrReadResultDq60to48.Data >> 8) & 0xFF);
      MprData[16] = (UINT8)((mrReadResultDq60to48.Data >> 24) & 0xFF);
      MprData[17] = (UINT8)((mrReadResultDq68to64.Data >> 8) & 0xFF);

    } //x4
  }

  // Disable DB function override
  WriteLrbuf(host, socket, ch, dimm, 0, 0, 0, LRDIMM_BC7x);

} // ReadBcwRankDdr4
#endif //LRDIMM_SUPPORT


/**

  Returns the memory controller ID

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval MC ID

**/
UINT8
GetMCID (
        PSYSHOST  host,
        UINT8     socket,
        UINT8     ch
        )
{
  return host->var.mem.socket[socket].channelList[ch].mcId;
} // GetMCID

/**

  Returns the memory controller ID

  @param host    - Pointer to sysHost
  @param socket  - Socket Id
  @param ch      - Channel number

  @retval MC ID

**/
UINT8
GetMCCh (
  UINT8     mcId,
  UINT8     ch
        )
{
  return (ch % MAX_MC_CH);
} // GetMCCh

/**

    Check to see if Rank for each socket is populated

    @param host - Pointer to SysHost

    @retval N/A

**/
void
CheckRankPop (
             PSYSHOST  host
             )
{
  UINT8 socket;

  for (socket = 0; socket < MAX_SOCKET; socket++) {

    CheckRankPopLocal(host, socket);

  } // socket loop
} // CheckRankPop


/**

    Check to see if Rank for specified socket is populated

    @param host   - Pointer to SysHost
    @param socket - Current socket

    @retval N/A

**/
void
CheckRankPopLocal (
                  PSYSHOST  host,
                  UINT8     socket
                  )
{
  UINT8                           ch;
  UINT8                           rank;
  UINT8                           dimm;
  UINT32                          dimmAmap;
  UINT8                           chRankEnabled;
  struct dimmNvram                (*dimmNvList)[MAX_DIMM];
  DIMMMTR_0_MC_MAIN_STRUCT        dimmMtr;
  AMAP_MC_MAIN_STRUCT             imcAMAPReg;

  if (host->nvram.mem.socket[socket].enabled == 0) return;

  for (ch = 0; ch < MAX_CH; ch++) {

    dimmNvList = GetDimmNvList(host, socket, ch);
    dimmAmap = 0;
    chRankEnabled = 0;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      for (rank = 0; rank < (*dimmNvList)[dimm].numDramRanks; rank++) {

        if ((*dimmNvList)[dimm].mapOut[rank] == 0) {
          chRankEnabled++;
        }
      } // rank loop
    } // dimm loop

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if (chRankEnabled == 0) {
        dimmMtr.Data = MemReadPciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4));
      } else {
        dimmMtr.Data = (*dimmNvList)[dimm].dimmMemTech;
      }

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (((*dimmNvList)[dimm].dimmPresent == 0) || (rank >= (*dimmNvList)[dimm].numDramRanks) || (chRankEnabled == 0)) {

          dimmMtr.Bits.rank_disable |= 1 << rank;

          if (((*dimmNvList)[dimm].dimmPresent) && (*dimmNvList)[dimm].mapOut[rank] == 1){
            // Disable Patrol Scrub on mapped out ranks
            dimmAmap |= (1 << (dimm * 8 + rank));

            // Update AMAP register
            imcAMAPReg.Data = MemReadPciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG);
            imcAMAPReg.Bits.dimm0_pat_rank_disable |= (UINT8)(dimmAmap & 0xff);
            imcAMAPReg.Bits.dimm1_pat_rank_disable |= (UINT8)((dimmAmap >> 8) & 0xff);
            MemWritePciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG,imcAMAPReg.Data);
          }
        }
      } //rank loop

      (*dimmNvList)[dimm].dimmMemTech = dimmMtr.Data;
      MemWritePciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4), (UINT32) dimmMtr.Data);

      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                     "dimmMtr: 0x%08x\n", (*dimmNvList)[dimm].dimmMemTech));
    } //dimm loop

    if (chRankEnabled == 0) {
      host->nvram.mem.socket[socket].channelList[ch].enabled = 0;
    }
  } // ch loop
} // CheckRankPopLocal

UINT8
GetMaxFreq (
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8                  i;
  UINT8                  maxDdrFreq;
  CAPID1_PCU_FUN3_STRUCT capid1;

  capid1.Data = ReadCpuPciCfgEx(host, socket, PCU_INSTANCE_0, CAPID1_PCU_FUN3_REG);
  if (capid1.Bits.dmfc & BIT3) {
    maxDdrFreq = 0xFF;
  } else {
    //
    // Convert fuse value
    //
    for (i = 0; i < 8; i++) {
      if (capid1.Bits.dmfc == DMFC[i]) break;
    }

    if ((!(host->var.common.emulation & SIMICS_FLAG)) && (i != 8)) {
      if (host->var.mem.socket[socket].ddrClkType) {
        if (host->var.mem.socket[socket].maxFreq != PcodeMaxRatio100[i]) {
          MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Could not find valid freq ratio: dmfc: %d Clk Type: %d max ratio: %d Expected max ratio: %d\n",
                        capid1.Bits.dmfc, host->var.mem.socket[socket].ddrClkType, host->var.mem.socket[socket].maxFreq, PcodeMaxRatio100[i]));
          OutputWarning (host, WARN_DIMM_COMPAT, WARN_CHANNEL_CONFIG_NOT_SUPPORTED, socket, 0xFF, 0xFF, 0xFF);
        }
      } else {
        if (host->var.mem.socket[socket].maxFreq != PcodeMaxRatio133[i]) {
          MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Could not find valid freq ratio: dmfc: %d Clk Type: %d max ratio: %d Expected max ratio: %d\n",
                        capid1.Bits.dmfc, host->var.mem.socket[socket].ddrClkType, host->var.mem.socket[socket].maxFreq, PcodeMaxRatio133[i]));
          OutputWarning (host, WARN_DIMM_COMPAT, WARN_CHANNEL_CONFIG_NOT_SUPPORTED, socket, 0xFF, 0xFF, 0xFF);
        }
      }
    }
    if (host->var.mem.socket[socket].ddrClkType) {
      switch (host->var.mem.socket[socket].maxFreq) {
        case (10):
          maxDdrFreq = DDR_1000;
          break;

        case (12):
          maxDdrFreq = DDR_1200;
          break;

        case (14):
          maxDdrFreq = DDR_1400;
          break;

        case (16):
          maxDdrFreq = DDR_1600;
          break;

        case (18):
          maxDdrFreq = DDR_1800;
          break;

        case (20):
          maxDdrFreq = DDR_2000;
          break;

        case (22):
          maxDdrFreq = DDR_2200;
          break;

        case (28):
          maxDdrFreq = DDR_2800;
          break;

        case (32):
          maxDdrFreq = DDR_3200;
          break;

        default:
          maxDdrFreq = DDR_2400;
          MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Could not find valid freq ratio: Capping frequency at 2400\n"));
          break;
      }
    } else {
      switch (host->var.mem.socket[socket].maxFreq) {
        case (10):
          maxDdrFreq = DDR_1333;
          break;

        case (12):
          maxDdrFreq = DDR_1600;
          break;

        case (14):
          maxDdrFreq = DDR_1866;
          break;

        case (16):
          maxDdrFreq = DDR_2133;
          break;

        case (18):
          maxDdrFreq = DDR_2400;
          break;

        case (20):
          maxDdrFreq = DDR_2666;
          break;

        case (22):
          maxDdrFreq = DDR_2933;
          break;

        case (24):
          maxDdrFreq = DDR_3200;
          break;

        default:
          maxDdrFreq = DDR_2400;
          MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Could not find valid freq ratio: Capping frequency at 2400\n"));
          break;
      }
    }

    if (i != 8) {
      if (host->var.mem.socket[socket].ddrClkType) {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "dmfc: %d Clk Type: %d max ratio: %d Expected max ratio: %d\n",
                       capid1.Bits.dmfc, host->var.mem.socket[socket].ddrClkType, host->var.mem.socket[socket].maxFreq, PcodeMaxRatio100[i]));
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "dmfc: %d Clk Type: %d max ratio: %d Expected max ratio: %d\n",
                       capid1.Bits.dmfc, host->var.mem.socket[socket].ddrClkType, host->var.mem.socket[socket].maxFreq, PcodeMaxRatio133[i]));
      }
    }//i
  }

  return maxDdrFreq;
}

UINT8
GetDCLKs(
  UINT8 index,
  UINT8 bclk
)
{
  if (bclk == 133) {
    return (DCLKs[index]);
  }
  return (DCLKs_100[index]);
}

VOID
GetDclkRatio (
  PSYSHOST  host,
  UINT8     socket,
  UINT8    *req_type,
  UINT8    *req_data
  )
{
  UINT32 mailboxStatus;
  UINT32 mailboxData;
  UINT8  maxFreq;
  MC_BIOS_REQ_PCU_FUN1_STRUCT   mcBiosReq;

  //
  // read the current MC freq via mailbox command
  //
  if (host->var.mem.socket[socket].ddrFreqCheckFlag == 0) {   //Ckeck if it is the first time
    mailboxStatus = WriteBios2PcuMailboxCommand(host, socket, MAILBOX_BIOS_CMD_READ_MC_FREQ, 0x0);
    if (mailboxStatus == 0) {  // success
      mailboxData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      *req_data = (UINT8) mailboxData & 0x3F;
      *req_type = (UINT8) ((mailboxData & 0x40) >> 6);
      maxFreq   = (UINT8) ((mailboxData & 0x1F80) >> 7);
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n Current DCLK: %d; reqType = %d\n", *req_data, *req_type));
      host->var.mem.socket[socket].ddrFreqCheckFlag = 1;
      host->var.mem.socket[socket].ddrClkData = *req_data;
      host->var.mem.socket[socket].ddrClkType = *req_type;
      host->var.mem.socket[socket].maxFreq    = maxFreq;
    } else {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Could not able to get Current DCLK and reqType from Pcode mailbox!\n"));
      OutputError (host, ERR_READ_MC_FREQ, ERR_NOT_ABLE_READ_MC_FREQ, socket, 0xFF, 0xFF, 0xFF);
    }
  } else {
    *req_data = host->var.mem.socket[socket].ddrClkData;
    *req_type = host->var.mem.socket[socket].ddrClkType;
  }
  // HSD 4930317 - publish Memory Frequency
  mcBiosReq.Data = ReadCpuPciCfgEx (host, socket, 0, MC_BIOS_REQ_PCU_FUN1_REG);
  mcBiosReq.Bits.req_data = *req_data;
  mcBiosReq.Bits.req_type = *req_type;
  WriteCpuPciCfgEx (host, socket, 0, MC_BIOS_REQ_PCU_FUN1_REG, mcBiosReq.Data);
} // GetDclkRatio

VOID
GetDclkBclk (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     *DCLKRatio,
  UINT8     *bclk
  )
{
}

VOID
SetDclkRatio (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     req_type,
  UINT8     req_data
  )
{
  UINT32 mailboxStatus;
  UINT32 mcBiosReq;

  //
  // set the desired DCLK ratio
  //
  mcBiosReq = ((req_type << 0x6) | req_data);

  //Set MC Fivr Alternate Voltage bit for > 2400
  if (host->nvram.mem.socket[socket].ddrFreq > DDR_2400) {
    mcBiosReq |= MAILBOX_BIOS_MC_FIVR_ALT_VOLT;
  }

  //
  // set MC freq via mailbox command
  //
  mailboxStatus = WriteBios2PcuMailboxCommand(host, socket, MAILBOX_BIOS_CMD_SET_MC_FREQ, mcBiosReq);

  if (mailboxStatus == MAILBOX_BIOS_ERROR_CODE_UNSUPPORTED_MC_FREQ) {
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "PCODE mailbox to Set MC Frequency failed!\n"));
    OutputError (host, ERR_SET_MC_FREQ, ERR_UNSUPPORTED_MC_FREQ, socket, 0xFF, 0xFF, 0xFF);
  } else if (mailboxStatus == MAILBOX_BIOS_ERROR_CODE_NO_ERROR) {
    host->var.mem.socket[socket].ddrClkData = req_data;
    host->var.mem.socket[socket].ddrClkType = req_type;
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "PCODE mailbox to Set MC Frequency succeeded!\n"));
  } else {
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "PCODE mailbox to Set MC Frequency failed: Boot with previous values!\n"));
  }

} // SetDclkRatio

VOID
SetChannelDisable (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  )
{
} // SetChannelDisable

UINT32
IsRankDisabled (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT16    rank
  )
{
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  DIMMMTR_0_MC_MAIN_STRUCT      dimmMtr;

  dimmNvList    = GetDimmNvList(host, socket, ch);
  dimmMtr.Data = (*dimmNvList)[dimm].dimmMemTech;

  // Skip ranks that are disabled
  if (dimmMtr.Bits.rank_disable & (1 << rank)) return 1;
  return 0;
}

VOID
SetDimmPop (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm
  )
{
  DIMMMTR_0_MC_MAIN_STRUCT dimmMtr;

  //
  // Disable DIMM so no maintenance operations go to it
  //
  dimmMtr.Data = MemReadPciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4));
  dimmMtr.Bits.dimm_pop = 1;
  MemWritePciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4), dimmMtr.Data);
}


VOID
DimmInfoDisplayProc (
  PSYSHOST  host,
  UINT8     socket
  )
{
  //
  // Display processor abbreviation, stepping and socket
  //

  if (host->var.common.cpuFamily == CPU_FAMILY_SKX) {
    rcPrintf ((host, "SKX", socket));
  } else {
    rcPrintf ((host, "?", socket));
  }
  rcPrintf ((host, " "));

  switch (host->var.common.stepping) {
    case A0_REV_SKX:
      rcPrintf ((host, " A0"));
      break;

    case A2_REV_SKX:
      rcPrintf ((host, " A2"));
      break;

    case B0_REV_SKX:
      rcPrintf ((host, " B0"));
      break;

    case B1_REV_SKX:
      rcPrintf ((host, " B1"));
      break;

    case H0_REV_SKX:
      rcPrintf ((host, " H0"));
      break;

    case L0_REV_SKX:
     rcPrintf ((host, " L0"));
     break;

    case M0_REV_SKX:
     rcPrintf ((host, " M0"));
     break;

    case U0_REV_SKX:
     rcPrintf ((host, " U0"));
    break;

    default:
      rcPrintf ((host, "Unknown"));
  }

  if (host->var.common.socketType == SOCKET_HEDT) {
    rcPrintf ((host, " - Socket HEDT\n"));
  } else if (host->var.common.socketType == SOCKET_2S) {
    rcPrintf ((host, " - Socket 2S\n"));
  } else if (host->var.common.socketType == SOCKET_4S) {
    rcPrintf ((host, " - Socket 4S\n"));
  }
}


UINT8
GetLvmode (
  PSYSHOST  host,
  UINT8     socket
  )
{
  DDRCRCOMPCTL0_MCIO_DDRIOEXT_STRUCT ddrCRCompCtl0;

  ddrCRCompCtl0.Data = MemReadPciCfgEp (host, socket, 0, DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG);
  return (UINT8)ddrCRCompCtl0.Bits.lvmode;
}


VOID
DimmInfoDisplayProcFeatures (
  PSYSHOST  host
  )
{
  UINT8                                 socket;
  UINT8                                 ch;
  struct channelNvram                   (*channelNvList)[MAX_CH];

  MCMTR_MC_MAIN_STRUCT              mcMtr;
  IDLETIME_MCDDC_CTL_STRUCT             idleTime;
  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT      scrambleConfig;
  SCRUBCTL_MC_MAIN_STRUCT               imcSCRUBCTL;

  idleTime.Data = 0;
  scrambleConfig.Data = 0;
  imcSCRUBCTL.Data = 0;
  mcMtr.Data = 0;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0))  continue;

    channelNvList = GetChannelNvList(host, socket);

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      idleTime.Data = MemReadPciCfgEp (host, socket, ch, IDLETIME_MCDDC_CTL_REG);
      scrambleConfig.Data = MemReadPciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG);
      break;
    } // ch loop

    imcSCRUBCTL.Data = MemReadPciCfgMain (host, socket, SCRUBCTL_MC_MAIN_REG);
    // Assume ecc_en symmetry between MC
    mcMtr.Data = MemReadPciCfgMC (host, socket, 0, MCMTR_MC_MAIN_REG);
    break;
  } // socket loop

  // ECC Support
  rcPrintf ((host, "ECC Checking        |            |            |            |            |"));
  if (host->nvram.mem.eccEn) {
    rcPrintf ((host, "     On     |\n"));
  } else {
    rcPrintf ((host, "     Off    |\n"));
  }

  // Scrubbing
  rcPrintf ((host, "Patrol/Demand Scrub |            |            |            |            |"));
  if (imcSCRUBCTL.Bits.scrub_en) {
    rcPrintf ((host, "     On     |\n"));
  } else {
    rcPrintf ((host, "     Off    |\n"));
  }

  // RAS Mode
  rcPrintf ((host, "RAS Mode            |            |            |            |            |"));
  if (host->nvram.mem.RASmode == 0) {
    rcPrintf ((host, "   Indep    |\n"));
  } else if (host->nvram.mem.RASmode == FULL_MIRROR_1LM) {
    rcPrintf ((host, "   1LM Mirror   |\n"));
  } else if (host->nvram.mem.RASmode == FULL_MIRROR_2LM) {
      rcPrintf ((host, "  2LM Mirror   |\n"));
  } else if (host->nvram.mem.RASmode == STAT_VIRT_LOCKSTEP) {
    rcPrintf ((host, "  Lockstep  |\n"));
  } else if (host->nvram.mem.RASmode == RK_SPARE) {
    rcPrintf ((host, " Rank Spare |\n"));
  } else if (host->nvram.mem.RASmode == ADDDC_EN) {
    rcPrintf ((host, " ADDDC Spare |\n"));
  } else if (host->nvram.mem.RASmode == SDDC_EN) {
    rcPrintf ((host, " SDDC Spare |\n"));
  } else if (host->nvram.mem.RASmode == CH_SL) {
    rcPrintf ((host, "    SP/LK   |\n"));
  } else if (host->nvram.mem.RASmode == CH_MS) {
    rcPrintf ((host, "    MR/SP   |\n"));
  }

  // Sparing Per Channel

  // Paging Policy
  rcPrintf ((host, "Paging Policy       |            |            |            |            |"));
  if (mcMtr.Bits.close_pg) {
    rcPrintf ((host, "   Closed   |\n"));
  } else if (idleTime.Bits.adapt_pg_clse) {
    rcPrintf ((host, " Adapt Open |\n"));
  } else {
    rcPrintf ((host, "    Open    |\n"));
  }

  // Scrambling
  rcPrintf ((host, "Data Scrambling     |            |            |            |            |"));
  if (scrambleConfig.Bits.rx_enable) {
    rcPrintf ((host, "     On     |\n"));
  } else {
    rcPrintf ((host, "     Off    |\n"));
  }

  // NUMA

}

void
SetChTwr (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     nWR
  )
{
  TCRAP_MCDDC_CTL_STRUCT      tCrap;

  tCrap.Data = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
  tCrap.Bits.t_wr = nWR;
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tCrap.Data);
} // SetChTwr


UINT8
GetCurrentTimingMode (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  )
{
  TCRAP_MCDDC_CTL_STRUCT      tCrap;

  tCrap.Data = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
  return (UINT8)tCrap.Bits.cmd_stretch;
} // GetCurrentTimingMode

UINT8
SetTimingMode (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     cmdTiming
  )
{
  TCRAP_MCDDC_CTL_STRUCT      tCrap;

  tCrap.Data = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
  tCrap.Bits.cmd_stretch = cmdTiming;
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tCrap.Data);
  return cmdTiming;
} // SetTimingMode


VOID
SetDataTiming (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     nCL,
  UINT8     nCWL
  )
{
  TCDBP_MCDDC_CTL_STRUCT  tcdbp;

  tcdbp.Data = MemReadPciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG);
  tcdbp.Bits.t_cwl = nCWL;
  tcdbp.Bits.t_cl = nCL;
  MemWritePciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG, tcdbp.Data);
} // SetDataTiming

VOID
SetEncodedCsMode (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  )
{
  UINT8 dimm;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  if ((*channelNvList)[ch].encodedCSMode) {
    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      MemWritePciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4), (UINT32) (*dimmNvList)[dimm].dimmMemTech);
    } //dimm
  } // encodedCSMode
} //SetEncodedCsMode

void
AdjustRoundtripForMpr(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT16    mprMode
  )
{
}

VOID
SetConfigBeforeRmt(
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8               ch;

  //
  //Set RfOn = 0 before RMT per sighting #4987510
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    SetRfOn(host, socket, ch, 0);
  } // ch loop
}

/**
  MemoryInitDdr()

  This routine performs the Memory Init on all the DIMMs present on MCs using patrol scrub
  engine and returns the operation result in the supplied MEM_IT_STATUS_STRUC array.

  NOTE: The memory Init is performed in Flat Memory Mode and All2All Memory Model.

  @param  Hhst         INPUT: Ptr to SYSHOST, system host (root) structure
  @param  socket       INPUT: Socket number

  @retval None

**/

VOID
MemoryInitDdr (
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8               ch;
  UINT32              ddrChEnabled;
  struct channelNvram (*channelNvList)[MAX_CH];

  // Create the Channel Mask
  ddrChEnabled = 0;
  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    ddrChEnabled |= 1 << ch;
  }

  //
  // Step 1: Write all 0's pattern
  //
#ifdef ADV_MEMTEST_SUPPORT
  MemInitOpt (host, socket, ddrChEnabled);
#else //ADV_MEMTEST_SUPPORT
  CpgcMemTest (host, socket, ddrChEnabled, DDR_CPGC_MEM_TEST_WRITE | DDR_CPGC_MEM_TEST_ALL0);
#endif  //ADV_MEMTEST_SUPPORT

} // MemoryInitDdr


VOID
PmSelfRefreshProgramming (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  )
{
  UINT8                             mcId;
  UINT8                             chId;
  UINT8                             tempMdllValue;
  UINT8                             tempCkModeValue;
  UINT32                            tempSrefTimer;
  UINT32                            mailboxData;
  UINT32                            pcodeMailboxStatus = 0;
  struct channelNvram               (*channelNvList)[MAX_CH];
  SREF_LL0_MCDDC_CTL_STRUCT         sref_ll0;
  SREF_LL1_MCDDC_CTL_STRUCT         sref_ll1;
  SREF_LL2_MCDDC_CTL_STRUCT         sref_ll2;
  SREF_LL3_MCDDC_CTL_STRUCT         sref_ll3;
  SREF_STATIC_MCDDC_CTL_STRUCT      sref_static;
  MCMNT_CHKN_BIT2_MCDDC_CTL_STRUCT  mcmnt2_chkn_bit2;

  channelNvList = GetChannelNvList(host, socket);

  mcId = ch / MAX_MC_CH;
  chId = ch % MAX_MC_CH; // Channel Id per MC

  mcmnt2_chkn_bit2.Data = MemReadPciCfgEp (host, socket, ch, MCMNT_CHKN_BIT2_MCDDC_CTL_REG);
  sref_ll0.Data = MemReadPciCfgEp (host, socket, ch, SREF_LL0_MCDDC_CTL_REG);
  sref_ll1.Data = MemReadPciCfgEp (host, socket, ch, SREF_LL1_MCDDC_CTL_REG);
  sref_ll2.Data = MemReadPciCfgEp (host, socket, ch, SREF_LL2_MCDDC_CTL_REG);
  sref_ll0.Data &= ~(BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT20 | 0xfffff);
  sref_ll1.Data &= ~(BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT20 | 0xfffff);
  sref_ll2.Data &= ~(BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT20 | 0xfffff);

  host->setup.mem.SrefProgramming = 1;

  if(host->setup.mem.SrefProgramming == 1) {
    if (host->nvram.mem.dimmTypePresent == UDIMM){
      host->setup.mem.CkMode = 0;
    }

    if(host->setup.mem.CkMode == 0){
      host->setup.mem.MdllOffEn = 0;
    }

    if(host->setup.mem.CkMode == 2){
      mcmnt2_chkn_bit2.Bits.dis_cke_pri_rcb_gate = 1;
      mcmnt2_chkn_bit2.Bits.dis_ref_pri_rcb_gate = 1;
    }

    //
    // Self Refresh idle timer = tRFC + tZQCS + 100
    //
    tempSrefTimer = (*channelNvList)[ch].common.nRFC + tZQCS_DDR4 + 100;

    sref_ll0.Data |= ((2 << 26) | (host->setup.mem.CkMode << 24) | (host->setup.mem.MdllOffEn <<22) | (host->setup.mem.OppSrefEn <<20 ) | tempSrefTimer);
    sref_ll1.Data |= ((2 << 26) | (host->setup.mem.CkMode << 24) | (host->setup.mem.MdllOffEn <<22) | (host->setup.mem.OppSrefEn <<20 ) | tempSrefTimer);
    sref_ll2.Data |= ((2 << 26) | (host->setup.mem.CkMode << 24) | (host->setup.mem.MdllOffEn <<22) | (host->setup.mem.OppSrefEn <<20 ) | tempSrefTimer);

    mailboxData = ((1 << 26) | (host->setup.mem.CkMode << 24) | (host->setup.mem.MdllOffEn <<22) | (host->setup.mem.OppSrefEn << 20) | tempSrefTimer);

  } else { //Use Auto values from table.

    if (host->nvram.mem.dimmTypePresent == UDIMM){
      //if udimm mdlloffen and ckmode will be 0.
      tempMdllValue = 0;
      tempCkModeValue = 0;
    } else { // CLK STOP ENABLED
      tempMdllValue = 1;
      tempCkModeValue = 2;
      mcmnt2_chkn_bit2.Bits.dis_cke_pri_rcb_gate = 1;
      mcmnt2_chkn_bit2.Bits.dis_ref_pri_rcb_gate = 1;
    }

    sref_ll0.Data |= ((2 << 26) | (tempCkModeValue << 24) | (tempMdllValue << 22) | (SREF_LL0_AUTO[1] << 20) | (SREF_LL0_AUTO[0]));
    sref_ll1.Data |= ((2 << 26) | (tempCkModeValue << 24) | (tempMdllValue << 22) | (SREF_LL1_AUTO[1] << 20) | (SREF_LL1_AUTO[0]));
    sref_ll2.Data |= ((2 << 26) | (tempCkModeValue << 24) | (tempMdllValue << 22) | (SREF_LL2_AUTO[1] << 20) | (SREF_LL2_AUTO[0]));
    mailboxData = ((1 << 26) | (tempCkModeValue << 24) | (tempMdllValue << 22) | (SREF_UCR_AUTO[1] << 20) | (SREF_UCR_AUTO[0]));

  }//end of Auto Values

  if (host->setup.mem.OppSrefEn == 0) {
    sref_ll0.Bits.opp_sref_en = 0;
    sref_ll1.Bits.opp_sref_en = 0;
    sref_ll2.Bits.opp_sref_en = 0;
    sref_ll3.Bits.opp_sref_en = 0;
  }

  // Write the register value into the Register
  MemWritePciCfgEp (host, socket, ch, SREF_LL0_MCDDC_CTL_REG, sref_ll0.Data);
  MemWritePciCfgEp (host, socket, ch, SREF_LL1_MCDDC_CTL_REG, sref_ll1.Data);
  MemWritePciCfgEp (host, socket, ch, SREF_LL2_MCDDC_CTL_REG, sref_ll2.Data);
  MemWritePciCfgEp (host, socket, ch, MCMNT_CHKN_BIT2_MCDDC_CTL_REG, mcmnt2_chkn_bit2.Data);

  //Program SREF_PKGC Mailbox register here.
  pcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, (UINT32) (MAILBOX_BIOS_CMD_SREF_PKGC | (mcId << 8) | (chId << 12)), mailboxData);

  if (pcodeMailboxStatus != 0) {
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "SREF_PKGC UCR Command Failed\n"));
  }

  sref_ll3.Data = mailboxData;
  MemWritePciCfgEp(host, socket, ch, SREF_LL3_MCDDC_CTL_REG, sref_ll3.Data);
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "opp_sref_en: %d\n", sref_ll3.Bits.opp_sref_en));
  //pkgc_sref_en

  sref_static.Data = MemReadPciCfgEp (host, socket, ch, SREF_STATIC_MCDDC_CTL_REG);
  sref_static.Data &= ~BIT30;

  if(host->setup.mem.PkgcSrefEn == 1) {
    sref_static.Data |= BIT30; //set PkgcSrefEn. This is default unless otherwise changed in setup.
  }

  MemWritePciCfgEp (host, socket, ch, SREF_STATIC_MCDDC_CTL_REG, sref_static.Data);

} // PmSelfRefreshProgramming


VOID PmCkeProgramming(
PSYSHOST host,
UINT8 socket,
UINT8 ch)
{

  CKE_LL0_MCDDC_CTL_STRUCT cke_ll0;
  CKE_LL1_MCDDC_CTL_STRUCT cke_ll1;
  CKE_LL2_MCDDC_CTL_STRUCT cke_ll2;
  CKE_LL3_MCDDC_CTL_STRUCT cke_ll3;
  UINT32 DclkCycle;
  UINT32 tempCkeTimer=0;
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  struct channelNvram         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);

  DclkCycle =  GettCK(host, socket); //Dclk cycles/ns * 10000

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "SRI DclkCycle %x\n", DclkCycle));

  cke_ll0.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL0_MCDDC_CTL_REG);
  cke_ll1.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL1_MCDDC_CTL_REG);
  cke_ll2.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL2_MCDDC_CTL_REG);
  cke_ll3.Data = MemReadPciCfgEp (host, socket, ch, CKE_LL3_MCDDC_CTL_REG);


  cke_ll0.Data &= ~(BIT24 | BIT9 | BIT8 | 0xff);
  cke_ll1.Data &= ~(BIT24 | BIT9 | BIT8 | 0xff);
  cke_ll2.Data &= ~(BIT24 | BIT9 | BIT8 | 0xff);
  cke_ll3.Data &= ~(BIT24 | BIT9 | BIT8 | 0xff);

  if (CheckSteppingLessThan(host, CPU_SKX, H0_REV_SKX) && host->nvram.mem.aepDimmPresent) {
    host->setup.mem.CkeProgramming = 1;
    host->setup.mem.PpdEn = 0;
    host->setup.mem.DdrtCkeEn = 0;
  }

  if(host->setup.mem.CkeProgramming == 1){//Manual

    if((host->setup.mem.ApdEn == 1) && (host->setup.mem.PpdEn == 1)){
      host->setup.mem.PpdEn = 0; //Both APD and PPD cannot be set to 1. Warning will be issued in set up. One of the bits will be forced to 0.
    }

    tempCkeTimer = ((host->setup.mem.CkeIdleTimer *DclkCycle)/10000);//number of Dclks

    cke_ll0.Data |= ((host->setup.mem.DdrtCkeEn << 24) | (host->setup.mem.PpdEn << 9) | (host->setup.mem.ApdEn <<8) | tempCkeTimer);
    cke_ll1.Data |= ((host->setup.mem.DdrtCkeEn << 24) | (host->setup.mem.PpdEn << 9) | (host->setup.mem.ApdEn <<8) | tempCkeTimer);
    cke_ll2.Data |= ((host->setup.mem.DdrtCkeEn << 24) | (host->setup.mem.PpdEn << 9) | (host->setup.mem.ApdEn <<8) | tempCkeTimer);
    cke_ll3.Data |= ((host->setup.mem.DdrtCkeEn << 24) | (host->setup.mem.PpdEn << 9) | (host->setup.mem.ApdEn <<8) | tempCkeTimer);

  }
  else{ //AUTO
                    //DDRT_CKE               PPD                        APD                     CKE idle timer
    cke_ll0.Data |= ((CKE_LL0_AUTO[3] << 24) | (CKE_LL0_AUTO[2] << 9) | (CKE_LL0_AUTO[1] << 8) | CKE_LL0_AUTO[0]);
    cke_ll1.Data |= ((CKE_LL1_AUTO[3] << 24) | (CKE_LL1_AUTO[2] << 9) | (CKE_LL1_AUTO[1] << 8) | CKE_LL1_AUTO[0]);
    cke_ll2.Data |= ((CKE_LL2_AUTO[3] << 24) | (CKE_LL2_AUTO[2] << 9) | (CKE_LL2_AUTO[1] << 8) | CKE_LL2_AUTO[0]);
    cke_ll3.Data |= ((CKE_LL3_AUTO[3] << 24) | (CKE_LL3_AUTO[2] << 9) | (CKE_LL3_AUTO[1] << 8) | CKE_LL3_AUTO[0]);

  }

  // HSD 4929072
  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    if (((*channelNvList)[ch].maxDimm == 2) && ((*channelNvList)[ch].encodedCSMode != 2)) {
      // if DIMM 0 and 1 are LRDIMM and not NVMDIMM
      if (((*dimmNvList)[0].aepDimmPresent == 0 && IsLrdimmPresent(host, socket, ch, 0)) && ((*dimmNvList)[1].aepDimmPresent == 0 && IsLrdimmPresent(host, socket, ch, 1))){
        cke_ll1.Bits.ppd_en = 0;
        cke_ll2.Bits.ppd_en = 0;
        cke_ll3.Bits.ppd_en = 0;
        cke_ll1.Bits.apd_en = 1;
        cke_ll2.Bits.apd_en = 1;
        cke_ll3.Bits.apd_en = 1;
      } // LRDIMM non NVMDIMM check
    } // 2DPC
  } // CheckSteppingLessThan

    // HSD 5331542
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    if (cke_ll1.Bits.ppd_en == 1) cke_ll1.Bits.apd_en = 1;
    if (cke_ll2.Bits.ppd_en == 1) cke_ll2.Bits.apd_en = 1;
    if (cke_ll3.Bits.ppd_en == 1) cke_ll3.Bits.apd_en = 1;
  }

  MemWritePciCfgEp (host, socket, ch, CKE_LL0_MCDDC_CTL_REG, cke_ll0.Data);
  MemWritePciCfgEp (host, socket, ch, CKE_LL1_MCDDC_CTL_REG, cke_ll1.Data);
  MemWritePciCfgEp (host, socket, ch, CKE_LL2_MCDDC_CTL_REG, cke_ll2.Data);
  MemWritePciCfgEp (host, socket, ch, CKE_LL3_MCDDC_CTL_REG, cke_ll3.Data);


  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "SRI cke_ll0.Data %x\n", cke_ll0.Data));

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "SRI cke_ll0.Data %x\n", cke_ll0.Data));
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "SRI cke_ll1.Data %x\n", cke_ll1.Data));
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "SRI cke_ll2.Data %x\n", cke_ll2.Data));
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "SRI cke_ll3.Data %x\n", cke_ll3.Data));

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "SRI tempCkeTimer %x\n", tempCkeTimer));
}


/**

  For certain DCLK to UCLK settings we need to add bubbles

  @param host    - Pointer to sysHost
  @param socket  - Socket to switch

**/
void
ProgramBgfTable (
    PSYSHOST  host,
    UINT8     socket
    )
{
  //HSD 4928684
  UINT8                             mcId;
  INT16                             d_ratio;
  INT16                             u_ratio;
  INT16                             uclk;
  MCBGF_U2D_OVER0_MC_MAIN_STRUCT    u2d_over0;
  MCBGF_U2D_OVER1_MC_MAIN_STRUCT    u2d_over1;
  MCBGF_U2D_OVER2_MC_MAIN_STRUCT    u2d_over2;
  MCBGF_U2D_OVER3_MC_MAIN_STRUCT    u2d_over3;

  //setup table bits
  u2d_over0.Data = 0;
  u2d_over1.Data = 0;
  u2d_over2.Data = 0;
  u2d_over3.Data = 0;

  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    switch (host->nvram.mem.socket[socket].ddrFreq) {
      case DDR_1600:
        u2d_over0.Bits.override900      = 14;
        u2d_over0.Bits.override1000     = 10;
        u2d_over1.Bits.override1100     = 11;
        u2d_over0.Bits.en_override900   = 1;
        u2d_over0.Bits.en_override1000  = 1;
        u2d_over1.Bits.en_override1100  = 1;
        break;

      case DDR_1800:

        u2d_over0.Bits.override1000     = 15;
        u2d_over1.Bits.override1100     = 17;
        u2d_over1.Bits.override1200     = 12;
        u2d_over1.Bits.override1300     = 13;
        u2d_over0.Bits.en_override1000  = 1;
        u2d_over1.Bits.en_override1100  = 1;
        u2d_over1.Bits.en_override1200  = 1;
        u2d_over1.Bits.en_override1300  = 1;
        break;

      case DDR_1866:
        u2d_over1.Bits.override1100     = 17;
        u2d_over1.Bits.override1200     = 12;
        u2d_over1.Bits.override1300     = 13;
        u2d_over1.Bits.en_override1100  = 1;
        u2d_over1.Bits.en_override1200  = 1;
        u2d_over1.Bits.en_override1300  = 1;
        break;

      case DDR_2000:
        u2d_over1.Bits.override1100     = 17;
        u2d_over1.Bits.override1200     = 18;
        u2d_over1.Bits.override1300     = 13;
        u2d_over1.Bits.override1400     = 14;
        u2d_over1.Bits.en_override1100  = 1;
        u2d_over1.Bits.en_override1200  = 1;
        u2d_over1.Bits.en_override1300  = 1;
        u2d_over1.Bits.en_override1400  = 1;
        break;

      case DDR_2133:
        u2d_over1.Bits.override1200     = 18;
        u2d_over1.Bits.override1300     = 20;
        u2d_over1.Bits.override1400     = 14;
        u2d_over2.Bits.override1500     = 15;
        u2d_over1.Bits.en_override1200  = 1;
        u2d_over1.Bits.en_override1300  = 1;
        u2d_over1.Bits.en_override1400  = 1;
        u2d_over2.Bits.en_override1500  = 1;
        break;

      case DDR_2200:
        u2d_over1.Bits.override1300     = 20;
        u2d_over1.Bits.override1400     = 14;
        u2d_over2.Bits.override1500     = 15;
        u2d_over2.Bits.override1600     = 16;
        u2d_over1.Bits.en_override1300  = 1;
        u2d_over1.Bits.en_override1400  = 1;
        u2d_over2.Bits.en_override1500  = 1;
        u2d_over2.Bits.en_override1600  = 1;
        break;

      case DDR_2400:
        u2d_over1.Bits.override1400     = 21;
        u2d_over2.Bits.override1500     = 15;
        u2d_over2.Bits.override1600     = 16;
        u2d_over2.Bits.override1700     = 17;
        u2d_over1.Bits.en_override1400  = 1;
        u2d_over2.Bits.en_override1500  = 1;
        u2d_over2.Bits.en_override1600  = 1;
        u2d_over2.Bits.en_override1700  = 1;
        break;

      case DDR_2600:
        u2d_over2.Bits.override1500     = 23;
        u2d_over2.Bits.override1600     = 24;
        u2d_over2.Bits.override1700     = 17;
        u2d_over2.Bits.override1800     = 18;
        u2d_over3.Bits.override1900     = 19;
        u2d_over2.Bits.en_override1500  = 1;
        u2d_over2.Bits.en_override1600  = 1;
        u2d_over2.Bits.en_override1700  = 1;
        u2d_over2.Bits.en_override1800  = 1;
        u2d_over3.Bits.en_override1900  = 1;

        break;

      case DDR_2666:
        u2d_over2.Bits.override1500     = 23;
        u2d_over2.Bits.override1600     = 24;
        u2d_over2.Bits.override1700     = 17;
        u2d_over2.Bits.override1800     = 18;
        u2d_over3.Bits.override1900     = 19;
        u2d_over2.Bits.en_override1500  = 1;
        u2d_over2.Bits.en_override1600  = 1;
        u2d_over2.Bits.en_override1700  = 1;
        u2d_over2.Bits.en_override1800  = 1;
        u2d_over3.Bits.en_override1900  = 1;
        break;

      case DDR_2800:
        u2d_over2.Bits.override1600     = 24;
        u2d_over2.Bits.override1700     = 26;
        u2d_over2.Bits.override1800     = 18;
        u2d_over3.Bits.override1900     = 19;
        u2d_over3.Bits.override2000     = 20;
        u2d_over2.Bits.en_override1600  = 1;
        u2d_over2.Bits.en_override1700  = 1;
        u2d_over2.Bits.en_override1800  = 1;
        u2d_over3.Bits.en_override1900  = 1;
        u2d_over3.Bits.en_override2000  = 1;
        break;

      case DDR_2933:
        u2d_over2.Bits.override1700     = 26;
        u2d_over2.Bits.override1800     = 27;
        u2d_over3.Bits.override1900     = 19;
        u2d_over3.Bits.override2000     = 20;
        u2d_over3.Bits.override2100     = 21;
        u2d_over2.Bits.en_override1700  = 1;
        u2d_over2.Bits.en_override1800  = 1;
        u2d_over3.Bits.en_override1900  = 1;
        u2d_over3.Bits.en_override2000  = 1;
        u2d_over3.Bits.en_override2100  = 1;
        break;

      default:
        break;
    }
  }

  else if (CheckSteppingEqual(host, CPU_SKX, B0_REV_SKX) || CheckSteppingEqual(host, CPU_SKX, L0_REV_SKX)) {
    switch (host->nvram.mem.socket[socket].ddrFreq) {

      case DDR_1800:
        d_ratio = ((1800 / 2) * 3) / 100;
        break;

      case DDR_1866:
        d_ratio = ((1866 / 2) * 3) / 100;
        break;

      case DDR_2000:
        d_ratio = ((2000 / 2) * 3) / 100;
        break;

      case DDR_2133:
        d_ratio = ((2133 / 2) * 3) / 100;
        break;

      case DDR_2200:
        d_ratio = ((2200 / 2) * 3) / 100;
        break;

      case DDR_2400:
        d_ratio = ((2400 / 2) * 3) / 100;
        break;

      case DDR_2600:
        d_ratio = ((2600 / 2) * 3) / 100;
        break;

      case DDR_2666:
        d_ratio = ((2666 / 2) * 3) / 100;
        break;

      case DDR_2800:
        d_ratio = ((2800 / 2) * 3) / 100;
        break;

      case DDR_2933:
        d_ratio = ((2933 / 2) * 3) / 100;
        break;

      default:
        d_ratio = 0;
        break;
    }
    for (uclk = 1200; uclk <= 2000; uclk = uclk + 100) {
      u_ratio = (uclk * 3) / 100;
      if (((d_ratio * 3) / 2) > u_ratio) {
        switch (uclk) {
          case 1200:
            u2d_over1.Bits.override1200     = (INT16) ((d_ratio * 2) - u_ratio);
            u2d_over1.Bits.en_override1200  = 1;
            break;

          case 1300:
            u2d_over1.Bits.override1300     = (d_ratio * 2) - u_ratio;
            u2d_over1.Bits.en_override1300  = 1;
            break;

          case 1400:
            u2d_over1.Bits.override1400     = (d_ratio * 2) - u_ratio;
            u2d_over1.Bits.en_override1400  = 1;
            break;

          case 1500:
            u2d_over2.Bits.override1500     = (d_ratio * 2) - u_ratio;
            u2d_over2.Bits.en_override1500  = 1;
            break;

          case 1600:
            u2d_over2.Bits.override1600     = (d_ratio * 2) - u_ratio;
            u2d_over2.Bits.en_override1600  = 1;
            break;

          case 1700:
            u2d_over2.Bits.override1700     = (d_ratio * 2) - u_ratio;
            u2d_over2.Bits.en_override1700  = 1;
            break;

          case 1800:
            u2d_over2.Bits.override1800     = (d_ratio * 2) - u_ratio;
            u2d_over2.Bits.en_override1800  = 1;
            break;

          case 1900:
            u2d_over3.Bits.override1900     = (d_ratio * 2) - u_ratio;
            u2d_over3.Bits.en_override1900  = 1;
            break;

          case 2000:
            u2d_over3.Bits.override2000     = (d_ratio * 2) - u_ratio;
            u2d_over3.Bits.en_override2000  = 1;
            break;

          default:
            break;
        }
      }
    }
  }

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Programming the DCLK/UCLK bubble generator \n"));

  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    MemWritePciCfgMC (host, socket, mcId, MCBGF_U2D_OVER0_MC_MAIN_REG, u2d_over0.Data);
    MemWritePciCfgMC (host, socket, mcId, MCBGF_U2D_OVER1_MC_MAIN_REG, u2d_over1.Data);
    MemWritePciCfgMC (host, socket, mcId, MCBGF_U2D_OVER2_MC_MAIN_REG, u2d_over2.Data);
    MemWritePciCfgMC (host, socket, mcId, MCBGF_U2D_OVER3_MC_MAIN_REG, u2d_over3.Data);
  }
} //ProgramBgfTable


/**

  Setup Command Address Parity and Write CRC error flows

  @param host    - Pointer to sysHost
  @param socket  - Socket to switch

**/
void
SetupCapWrCrcErrorFlow (
    PSYSHOST  host,
    UINT8     socket
    )
{
  UINT8                                   rank;
  UINT8                                   dimm;
  UINT8                                   ch;
  UINT8                                   dimmIsolation;
  UINT8                                   vmseErrorLatency;
  UINT8                                   max_roundTripLatency;
  UINT8                                   cur_roundTripLatency;
  struct channelNvram                     (*channelNvList)[MAX_CH];
  struct dimmNvram                        (*dimmNvList)[MAX_DIMM];
  ERF_DDR4_CMD_REG0_MCDDC_CTL_STRUCT      erfDdr4CmdReg0;
  DDR4_CA_CTL_MCDDC_DP_STRUCT             ddr4cactl;
  DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_STRUCT  ddrCRCmdSControls;
  VMSE_ERROR_MCDDC_DP_STRUCT              vmseerrordata;
  TCOTHP_MCDDC_CTL_STRUCT                 tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT                tCOTHP2;
  MCWDB_CHKN_BIT_MCDDC_DP_STRUCT          disDdrioEarly;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //
    // setup vmse_err_latency
    //
    vmseerrordata.Data = MemReadPciCfgEp (host, socket, ch , VMSE_ERROR_MCDDC_DP_REG);

    // Find Max Round Trip
    max_roundTripLatency = 0;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ( (*dimmNvList)[dimm].dimmPresent == 0 ) continue;
      for (rank = 0;  rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        cur_roundTripLatency = GetRoundTrip(host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank));
        if (max_roundTripLatency < cur_roundTripLatency) max_roundTripLatency = cur_roundTripLatency;
      } // rank
    }  // dimm

    vmseErrorLatency = (*channelNvList)[ch].common.nWL + 5 + (((max_roundTripLatency+1)/2 - (*channelNvList)[ch].common.nCL)) + (UINT8)((CRC_ALERT_DDR4/(host->nvram.mem.socket[socket].QCLKps * 2) + 1));

    // SVL
    if ((host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) == STAT_VIRT_LOCKSTEP) {
      if (CheckSteppingLessThan (host, CPU_SKX, H0_REV_SKX)) {
        tCOTHP.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
        tCOTHP2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
        disDdrioEarly.Data = MemReadPciCfgEp (host, socket, ch, MCWDB_CHKN_BIT_MCDDC_DP_REG);
        vmseErrorLatency = (*channelNvList)[ch].common.nWL + ((UINT8)tCOTHP.Bits.t_cwl_adj - (UINT8)tCOTHP2.Bits.t_cwl_adj_neg) - 6;
        if (disDdrioEarly.Bits.dis_ddrio_earlywdata == 0) {
          vmseErrorLatency = vmseErrorLatency - 1;
        }
      }
    } 

    vmseerrordata.Bits.vmse_err_latency = vmseErrorLatency;
    if (vmseErrorLatency > (UINT32)(17 + (*channelNvList)[ch].common.nWTR + (*channelNvList)[ch].common.nWL + ((max_roundTripLatency+1)/2))) {
        MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "vmse_err_latency is greater than the maximum value"));
    }

    MemWritePciCfgEp (host, socket, ch , VMSE_ERROR_MCDDC_DP_REG, vmseerrordata.Data);

    //
    // HSD 5332037 - set rxvref = 0, prevents inadvertent alert during pkgC
    //
    if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX) && ((host->setup.mem.options & CA_PARITY_EN) || (host->setup.mem.optionsExt & WR_CRC))) {
      ddrCRCmdSControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG);
      ddrCRCmdSControls.Bits.rxvref = 0;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdSControls.Data);
    }

    ddr4cactl.Data = MemReadPciCfgEp(host, socket, ch, DDR4_CA_CTL_MCDDC_DP_REG);
    // 4928669  Register : imcX_cX_ddr4_ca_ctl    Field: erf_en0 should be set to 1
    // (if trainingdone & dimmisolation & (WR_CRC | CAP))
    if ((host->nvram.mem.socket[socket].cmdClkTrainingDone) && ((host->setup.mem.options & CA_PARITY_EN) || (host->setup.mem.optionsExt & WR_CRC))) {
      ddr4cactl.Bits.erf_en0 = 1;  // enable error flow
    } else {
      ddr4cactl.Bits.erf_en0 = 0;
    }
    MemWritePciCfgEp(host, socket, ch, DDR4_CA_CTL_MCDDC_DP_REG, ddr4cactl.Data);

    // if dimm isolation is enabled
    if (host->setup.mem.optionsExt & DIMM_ISOLATION_EN){
      dimmIsolation = 1;
    } else {
      dimmIsolation = 0;
    }

    //
    // HSD 5331580
    // If CAP and SVL are enabled disable dimm isolation
    if (((host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) == STAT_VIRT_LOCKSTEP) && (host->setup.mem.options & CA_PARITY_EN) && CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
      dimmIsolation = 0;
    }

    // nothing is enabled, default setting for REG2
    if (((~host->setup.mem.options & CA_PARITY_EN) && (~host->setup.mem.optionsExt & WR_CRC)) || (dimmIsolation == 0)) {
      erfDdr4CmdReg0.Data         = 0;
      erfDdr4CmdReg0.Bits.ad      = 0x66;
      erfDdr4CmdReg0.Bits.bank    = 7;
      erfDdr4CmdReg0.Bits.rcw     = 0;
      erfDdr4CmdReg0.Bits.rdimm   = 1;
      erfDdr4CmdReg0.Bits.en      = 1;
      MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG2_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

    // something is enabled
    } else {
      // Reg 0 setup
      erfDdr4CmdReg0.Data         = 0;
      erfDdr4CmdReg0.Bits.ad      = 4;
      erfDdr4CmdReg0.Bits.bank    = 3;
      erfDdr4CmdReg0.Bits.rcw     = 0;
      erfDdr4CmdReg0.Bits.rdimm   = 1;
      // only enable if no NVMDIMM DIMMs and CA_PARITY_EN is enabled
      if ((host->setup.mem.options & CA_PARITY_EN) && (dimmIsolation == 1)) erfDdr4CmdReg0.Bits.en   = 1;
      MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG0_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

        // Reg 1 setup
      erfDdr4CmdReg0.Data         = 0;
      erfDdr4CmdReg0.Bits.ad      = 0x64;
      erfDdr4CmdReg0.Bits.bank    = 7;
      erfDdr4CmdReg0.Bits.rcw     = 0;
      erfDdr4CmdReg0.Bits.rdimm   = 1;
      if ((host->setup.mem.options & CA_PARITY_EN) && (dimmIsolation == 1)) erfDdr4CmdReg0.Bits.en   = 1;
      MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG1_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

        // Reg 2 setup
      erfDdr4CmdReg0.Data         = 0;
      erfDdr4CmdReg0.Bits.ad      = 0x1400;
      erfDdr4CmdReg0.Bits.bank    = 0;
      erfDdr4CmdReg0.Bits.rcw     = 5;
      erfDdr4CmdReg0.Bits.rdimm   = 1;
      if ((host->setup.mem.options & CA_PARITY_EN) && (dimmIsolation == 1)) erfDdr4CmdReg0.Bits.en   = 1;
      MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG2_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

        // Reg 3 setup
      erfDdr4CmdReg0.Data         = 0;
      erfDdr4CmdReg0.Bits.ad      = 0x66;
      erfDdr4CmdReg0.Bits.bank    = 7;
      erfDdr4CmdReg0.Bits.rcw     = 0;
      erfDdr4CmdReg0.Bits.rdimm   = 1;
      if ((host->setup.mem.options & CA_PARITY_EN) && (dimmIsolation == 1)) erfDdr4CmdReg0.Bits.en   = 1;
      MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG3_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

      // HSD 4930067 - WRCRC flow broken for A0, ZBB for B0 HSD 5370272
      //if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
        erfDdr4CmdReg0.Data         = 0;
        erfDdr4CmdReg0.Bits.ad      = 0;
        erfDdr4CmdReg0.Bits.bank    = 3;
        erfDdr4CmdReg0.Bits.rcw     = 0;
        erfDdr4CmdReg0.Bits.rdimm   = 1;
        if ((host->setup.mem.options & CA_PARITY_EN) && (dimmIsolation == 1)) erfDdr4CmdReg0.Bits.en   = 1;
        MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG4_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);
      // } else {
          // // Reg 4 setup
        // erfDdr4CmdReg0.Data         = 0;
        // erfDdr4CmdReg0.Bits.ad      = 5;
        // erfDdr4CmdReg0.Bits.bank    = 3;
        // erfDdr4CmdReg0.Bits.rcw     = 0;
        // erfDdr4CmdReg0.Bits.rdimm   = 0;
        // if ((host->setup.mem.optionsExt & WR_CRC) && (dimmIsolation == 1) && ((*channelNvList)[ch].ddrtEnabled == 0)) erfDdr4CmdReg0.Bits.en      = 1;
        // MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG4_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

          // // Reg 5 setup
        // erfDdr4CmdReg0.Data         = 0;
        // erfDdr4CmdReg0.Bits.ad      = 0x400;
        // erfDdr4CmdReg0.Bits.bank    = 7;
        // erfDdr4CmdReg0.Bits.rcw     = 5;
        // erfDdr4CmdReg0.Bits.rdimm   = 0;
        // if ((host->setup.mem.optionsExt & WR_CRC) && (dimmIsolation == 1) && ((*channelNvList)[ch].ddrtEnabled == 0)) erfDdr4CmdReg0.Bits.en      = 1;
        // MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG5_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

          // // Reg 6 setup
        // erfDdr4CmdReg0.Data         = 0;
        // erfDdr4CmdReg0.Bits.ad      = 0;
        // erfDdr4CmdReg0.Bits.bank    = 5;
        // erfDdr4CmdReg0.Bits.rcw     = 0;
        // erfDdr4CmdReg0.Bits.rdimm   = 0;
        // if ((host->setup.mem.optionsExt & WR_CRC) && (dimmIsolation == 1) && ((*channelNvList)[ch].ddrtEnabled == 0)) erfDdr4CmdReg0.Bits.en      = 1;
        // MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG6_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);

          // // Reg 7 setup
        // erfDdr4CmdReg0.Data         = 0;
        // erfDdr4CmdReg0.Bits.ad      = 0;
        // erfDdr4CmdReg0.Bits.bank    = 3;
        // erfDdr4CmdReg0.Bits.rcw     = 0;
        // erfDdr4CmdReg0.Bits.rdimm   = 0;
        // if ((((host->setup.mem.optionsExt & WR_CRC) && ((*channelNvList)[ch].ddrtEnabled == 0)) || (host->setup.mem.options & CA_PARITY_EN)) && (dimmIsolation == 1)) erfDdr4CmdReg0.Bits.en   = 1;
        // MemWritePciCfgEp(host, socket, ch, ERF_DDR4_CMD_REG7_MCDDC_CTL_REG, erfDdr4CmdReg0.Data);
      // }
    }
  } // ch
} //SetupCapWrCrcErrorFlow

void
ProgramErid2DataValid(
                      PSYSHOST host,
                      UINT8    socket
)
{
  UINT8                                     ch;
  UINT8                                     dimm;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimm0BasicTiming;
  DDRT_DATAPATH_DELAY_MCDDC_DP_STRUCT       ddrtDatapathDelay;
  ROUNDTRIP0_MCDDC_DP_STRUCT                roundtrip0;
  DDRT_DATAPATH_DELAY2_MCDDC_DP_STRUCT      ddrtDataPathDelay2;
  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
    ddrtDatapathDelay.Data    = MemReadPciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY_MCDDC_DP_REG);
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      switch(dimm) {
        case 0:
          ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG);
          roundtrip0.Data = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP0_MCDDC_DP_REG);
          break;
        case 1:
        default:
          ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG);
          roundtrip0.Data = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP1_MCDDC_DP_REG);
          break;
      } // switch
      // Read to Data Valid = Roundtrip[Q Clks]/2 - Trained Grant2Erid[D Clks] - 4 [D Clks]
      ddrtDatapathDelay.Bits.erid_to_rdvalid_delay = (roundtrip0.Bits.rt_rank0/2) - ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid - 4;
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                     "erid2datavalid = %d\n", ddrtDatapathDelay.Bits.erid_to_rdvalid_delay));
/*
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        //HSD 5330417: Increase completion delay
        ddrtGnt2data.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_MR0_FNV_DA_UNIT_0_REG);
        tempRoundtrip = GetRoundTrip(host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank)) / 2;
        if (tempRoundtrip < (ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid + 14)) {
          tclGntdata = (UINT8)ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid + 14 - tempRoundtrip;
          SetRoundTrip(host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank), (2 * (tempRoundtrip + tclGntdata)));
          //map tclgntdata to the register DCLK values
          if (tclGntdata < 16) tclGntdata = tclGntdata - 9;
          else if ((tclGntdata > 16) && !(tclGntdata % 2) && (tclGntdata <= 24)) tclGntdata = tclGntdata - (tclGntdata - 18) / 2 - 10;
          else if ((tclGntdata > 16) && (tclGntdata % 2) && (tclGntdata < 24)) tclGntdata = tclGntdata - (tclGntdata - 17) / 2 - 4;
          else MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "tclGntdata exceeds maximum value of 24: %d\n", tclGntdata));
          ddrtGnt2data.Bits.tcl_gnt_data_0 = tclGntdata & 0x01;
          ddrtGnt2data.Bits.tcl_gnt_data = (tclGntdata >> 1) & 0x07;
          WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_MR0_FNV_DA_UNIT_0_REG, ddrtGnt2data.Data);
        }
      } //rank loop
*/
    } // dimm loop
    //
    // HSD5331313: 2lm Scoreboard rejects MemInvXtoI multiple times
    //

    ddrtDatapathDelay.Bits.rd = ddrtDatapathDelay.Bits.erid_to_rdvalid_delay + 4 + 1;
    ddrtDataPathDelay2.Data = MemReadPciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG);
    ddrtDataPathDelay2.Bits.erid_to_retry_delay = ddrtDatapathDelay.Bits.erid_to_rdvalid_delay + 1;
    ddrtDataPathDelay2.Bits.erid_to_earlyretry_delay = ddrtDataPathDelay2.Bits.erid_to_retry_delay - 2;
    MemWritePciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY2_MCDDC_DP_REG, ddrtDataPathDelay2.Data);

    MemWritePciCfgEp (host, socket, ch, DDRT_DATAPATH_DELAY_MCDDC_DP_REG, ddrtDatapathDelay.Data);
  } // ch loop
}

void
DisableDDRTEridParityErrorLogging(PSYSHOST host,
                                  UINT8    socket
)
{
  UINT64_STRUCT msrReg;
  msrReg = ReadMsrPipe(host, socket, MSR_MC_CTL);
  msrReg.lo |= BIT23;
  WriteMsrPipe(host, socket, MSR_MC_CTL, msrReg);
}

UINT32
SetupSVLandScrambling (
                       PSYSHOST host
)
{
  UINT8                                         socket;
  UINT8                                         ch, SktCh;
  UINT8                                         mc;
  UINT32                                        VirtualLockstepEn;
  struct channelNvram                           (*channelNvList)[MAX_CH];
  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT              scrambleConfig;
  SPARING_CONTROL_MC_MAIN_STRUCT                SprngCtrl;
  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);

  if ((host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) == STAT_VIRT_LOCKSTEP) {
    for(mc = 0; mc< MAX_IMC; mc++) {
      VirtualLockstepEn = 0;
      for (ch = 0; ch < MAX_MC_CH; ch++) {
        SktCh= NODECH_TO_SKTCH(mc,ch);

        if ((*channelNvList)[SktCh].enabled == 0) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\nChannel not enabled, no SVL \n"));
          continue;            // Channel not enabled, no SVL
        }

        if ( (host->nvram.mem.socket[socket].channelList[SktCh].dimmList[0].aepDimmPresent)
            || (host->nvram.mem.socket[socket].channelList[SktCh].dimmList[1].aepDimmPresent)){
          continue; // Cannot enable SVL on a channel with AEP dimm
        }

        SprngCtrl.Data = MemReadPciCfgMC (host, socket, mc, SPARING_CONTROL_MC_MAIN_REG);
        if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {
          SprngCtrl.Bits.virtual_lockstep_en = 1;
        } else {
          VirtualLockstepEn = VirtualLockstepEn | 1<<(ch);
          SprngCtrl.Bits.virtual_lockstep_en = VirtualLockstepEn;
        }
        MemWritePciCfgMC (host, socket, mc, SPARING_CONTROL_MC_MAIN_REG, SprngCtrl.Data);
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "IMC[%d].SPARING_CONTROL.virtual_lockstep_en = %d\n", mc, SprngCtrl.Bits.virtual_lockstep_en));
      }
    }
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //
    // Enable Scrambling
    //
    if (host->setup.mem.options & SCRAMBLE_EN) {

      scrambleConfig.Data = MemReadPciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG);
      scrambleConfig.Bits.rx_enable = 1;
      scrambleConfig.Bits.tx_enable = 1;
      scrambleConfig.Bits.ch_enable = 1;

      MemWritePciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG, scrambleConfig.Data);
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\nScrambling Enabled\n"));
    }
  }

  return SUCCESS;
}

void
InitRtlOffsetChip(
  UINT8     rtlOffset[2]
)
{
  rtlOffset[0] = RTL_OFFSET_0;
  rtlOffset[1] = RTL_OFFSET_1;
  return;
}
;

void
DdrtCapErrorWA(PSYSHOST host,
UINT8 socket,
UINT8 ch)
{
  TCOTHP_MCDDC_CTL_STRUCT tcothp;

  tcothp.Data = MemReadPciCfgEp(host, socket, ch, TCOTHP_MCDDC_CTL_REG);
  tcothp.Bits.t_cs_oe = 0;
  tcothp.Bits.t_odt_oe = 0;
  MemWritePciCfgEp(host, socket, ch, TCOTHP_MCDDC_CTL_REG, tcothp.Data);
}

void
ddrtS3Resume(PSYSHOST host,
UINT8 socket
)
{
  UINT8  ch;
  struct channelNvram (*channelNvList)[MAX_CH];

  if ((host->var.common.bootMode == NormalBoot)) {
    return;
  }
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 0) continue;
    JedecInitDdrChannel(host, socket, ch);
  }
}
