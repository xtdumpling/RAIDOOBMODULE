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
 *      IMC and DDR4 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "SysHostChip.h"
#include "SysFuncChip.h"
#include "RcRegs.h"
#include "Cpgc.h"
#include "CpgcChip.h"
#include "FnvAccess.h"
#include "MemProjectSpecific.h"
#include "MemFuncChip.h"
#include "MemApiSkx.h"
#include "Ddr4OdtValueTable.h"
#include "MemHostChip.h"

// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

#define  MCODT_50       0
#define  MCODT_100      1

#define  MCODT_ACT      1
#define  ODT_ACT        1
#define  NO_ACT         0

#define  READDQDQS_EXTRA_MARGIN   -4

//#include "Ddr4OdtValueTable.h"
#include "Ddr4OdtActivationTable.h"

extern UINT8 DCLKs[MAX_SUP_FREQ];
extern UINT8 BCLKs[MAX_SUP_FREQ];
extern UINT8 bclkfreq[2];

// PRIVATE Prototypes
BOOLEAN DidChipClearAEPDimmParityErrors(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, PCHIP_CLEAR_PARITY_RESULTS_STRUCT chipClearParityResStruct);
BOOLEAN ShouldChipClearPtyAltWriteMR5(PSYSHOST host);
void DoChipSimClearParityResultsAltPath(PSYSHOST host, UINT8 socket);
void PerformChipSpecificNormalCMDTimingSetup(PSYSHOST host, UINT8 socket, UINT8 ch, PCHIP_SET_NORMAL_CMD_TIMING_STRUCT chipSetNormalCMDTimingStruct);
UINT8 SetMinGoodInit (PSYSHOST host);
static UINT16 SupportedFirmware[NUM_FW_REVISION][NUM_FW_REVISION_FIELDS] = {{ MAJOR_VERSION_CLK_MOVEMENT, MINOR_VERSION_CLK_MOVEMENT, HOTFIX_CLK_MOVEMENT, BUILD_CLK_MOVEMENT },
                                                                            { MAJOR_VERSION_BCOM_MARGINING, MINOR_VERSION_BCOM_MARGINING, HOTFIX_BCOM_MARGINING, BUILD_BCOM_MARGINING }};
VOID
ReceiveEnableInitChip (
  PSYSHOST    host,                             // Pointer to the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch
  );


#define DDRT_SUPPORTED_FREQUENCIES 5
//                                                              1600 1866 2133 2400 2666
UINT8 DdrtCASLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES]            = {10, 12, 14, 15, 15};
UINT8 DdrtCASLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES]       = { 6,  6,  6,  6,  6};
UINT8 DdrtCASWriteLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES]       = { 9, 10, 11, 12, 14};
UINT8 DdrtCASWriteLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES]  = { 7,  7,  7,  7,  9};
UINT8 DdrtCASLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES]           = {11, 12, 14, 15, 15};
UINT8 DdrtCASLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES]      = { 4,  5,  5,  5,  5};
UINT8 DdrtCASWriteLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES]      = { 9, 10, 11, 12, 14};
UINT8 DdrtCASWriteLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES] = { 7,  7,  7,  6,  8};
UINT8 DdrtCASWriteLatencyRDIMMFMC[DDRT_SUPPORTED_FREQUENCIES]    = { 9, 10, 11, 14, 16};
//                                                                  1600 1866 2133 2400 2666
UINT8 EkvDdrtCASLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES]            = {10, 12, 14, 15, 15};
UINT8 EkvDdrtCASLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES]       = { 6,  6,  6,  6,  6};
UINT8 EkvDdrtCASWriteLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES]       = { 9, 10, 14, 16, 18};
UINT8 EkvDdrtCASWriteLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES]  = { 7,  7,  6,  6,  7};
UINT8 EkvDdrtCASLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES]           = {11, 12, 14, 15, 15};
UINT8 EkvDdrtCASLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES]      = { 4,  5,  5,  5,  5};
UINT8 EkvDdrtCASWriteLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES]      = { 9, 10, 11, 12, 18};
UINT8 EkvDdrtCASWriteLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES] = { 7,  7,  7,  6,  6};

extern PatCadbProg0 PrechargePattern0[CADB_LINES];
extern PatCadbProg1 DeselectPattern1[CADB_LINES];
static UINT32 NonStickyScratchPads[16] = {BIOSNONSTICKYSCRATCHPAD0_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD1_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD3_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD4_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD5_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD6_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD7_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD8_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD9_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD12_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG,
                                   BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG};

static UINT32 StickyScratchPads[8] = {BIOSSCRATCHPAD0_UBOX_MISC_REG,
                               BIOSSCRATCHPAD1_UBOX_MISC_REG,
                               BIOSSCRATCHPAD2_UBOX_MISC_REG,
                               BIOSSCRATCHPAD3_UBOX_MISC_REG,
                               BIOSSCRATCHPAD4_UBOX_MISC_REG,
                               BIOSSCRATCHPAD5_UBOX_MISC_REG,
                               BIOSSCRATCHPAD6_UBOX_MISC_REG,
                               BIOSSCRATCHPAD7_UBOX_MISC_REG};


UINT8
GetCmdMarginsSweepErrorResFeedback(
GSM_CSN signal,
UINT8   dimm,
INT16   piIndex,
UINT8   CurError
) {
  UINT8 errorResult = 0;
  return errorResult;
}

VOID
SetCpgcPatControl (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     mux0,
  UINT8     mux1,
  UINT8     mux2,
  UINT32    patbuf0,
  UINT32    patbuf1,
  UINT32    patbuf2
)
{
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT  patWDBCLMuxCtl;

  patWDBCLMuxCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG);
  patWDBCLMuxCtl.Bits.mux0 = mux0;
  patWDBCLMuxCtl.Bits.mux1 = mux1;
  patWDBCLMuxCtl.Bits.mux2 = mux2;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG,    patWDBCLMuxCtl.Data);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG, patbuf0);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG, patbuf1);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG, patbuf2);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG, patbuf0);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG, patbuf1);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG, patbuf2);
}

UINT32
GetErrorResults (
  PSYSHOST   host,
  UINT8      socket,
  UINT8      ch
)
{
  return MemReadPciCfgEp (host, socket, ch, CPGC_ERR_BYTE_MCDDC_CTL_REG);
}

UINT8
GetWrDqDqsOffset (
  PSYSHOST   host
)
{

  return 0;
}

VOID
SetMprTrainMode (
  PSYSHOST    host,
  UINT8       socket,
  UINT8       ch,
  UINT8       enable
)
{
  CPGC_MISCODTCTL_MCDDC_CTL_STRUCT   cpgcMiscODTCtl;
  cpgcMiscODTCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG);
  cpgcMiscODTCtl.Bits.mpr_train_ddr_on = enable;
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG, cpgcMiscODTCtl.Data);
}

UINT8
GetChipParamType (
  PSYSHOST    host,
  UINT8       Param
)
{
  UINT8 ParamType = DEFAULT_PARAM;

  switch(Param){
  case traindramron:
    ParamType        = PER_RANK;
    break;
  case trainmcodt:
    ParamType       = PER_MC;
    break;
  case trainnontgtodt:
    ParamType       = PER_CH_PER_DIMM;
    break;
  case trainrttwr:
    ParamType       = PER_RANK;
    break;
  case trainmcron:
    ParamType       = PER_MC;
    break;
  case traintxeq:
    ParamType       = PER_BYTE;
    break;
  case trainimode:
    ParamType       = PER_CH_PER_BYTE;
    break;
  case trainctle:
    ParamType       = PER_CH_PER_BYTE;
    break;
  default:
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "GetChipParamType called unknown parameter type\n"));
    break;
  }
  return ParamType;
}


UINT32
GetStickyScratchpad (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     scratchpad
  )
{
  return ReadCpuPciCfgEx (host, socket, 0, StickyScratchPads[scratchpad]);
} // GetStickyScratchpad

void
SetStickyScratchpad (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     scratchpad,
  UINT32    data
  )
{
  WriteCpuPciCfgEx (host, socket, 0, StickyScratchPads[scratchpad], data);
} // SetStickyScratchpad

UINT32
GetNonStickyScratchpad (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     scratchpad
  )
{
  return ReadCpuPciCfgEx (host, socket, 0, NonStickyScratchPads[scratchpad]);
} // GetNonStickyScratchpad

void
SetNonStickyScratchpad (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     scratchpad,
  UINT32    data
  )
{
  WriteCpuPciCfgEx (host, socket, 0, NonStickyScratchPads[scratchpad], data);
} // SetNonStickyScratchpad

UINT32
  GetDataTrainFeedback (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     strobe
  )
{
  return MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG));
}

void
SetActToActPeriod(
           PSYSHOST        host,
           UINT8           socket,
           UINT8           ch,
           UINT32          tRRD_s_org[MAX_CH]
)
{
  TCRAP_MCDDC_CTL_STRUCT tcrap;

  tcrap.Data       = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);

  if (tRRD_s_org[ch] == 0){
    tRRD_s_org[ch]   = tcrap.Data;
  }

  tcrap.Bits.t_rrd = 4;
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tcrap.Data);
} // SetActToActPeriod

void
SetIOLatencyComp(
                  PSYSHOST  host,
                  UINT8     socket,
                  UINT8     ch,
                  UINT8     rtError,
                  UINT32    tcrapOrg[MAX_CH]
)
{
    IOLATENCY1_MCDDC_DP_STRUCT        ioLatency1;
    struct channelNvram               (*channelNvList)[MAX_CH];

    channelNvList = GetChannelNvList(host, socket);

    // If not error flag, set IO Latency Compensation for normal operation
    if (!rtError) {
      ioLatency1.Data = MemReadPciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG);
      ioLatency1.Bits.io_lat_io_comp = MRC_ROUND_TRIP_IO_COMPENSATION;
      MemWritePciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG, ioLatency1.Data);
      (*channelNvList)[ch].ioLatency1 = ioLatency1.Data;
    } else {
      DisableChannel(host, socket, ch);
    }

    MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tcrapOrg[ch]);
} // SetIOLatencyComp

void
ReceiveEnableCleanup(
  PSYSHOST host,
  UINT8    socket
)
{
} // ReceiveEnableCleanup

void
SetLongPreamble(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  BOOLEAN  set
)
{

  UINT8                             strobe;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT dataControl0;
  struct channelNvram                (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  // TODO: Added parenthesis to clear GCC issue, but SIMICS_FLAG is no longer used in Skylake,
  // so this code needs to be revisited to replace it with a flag that is still used'
  if (!(host->var.common.emulation & SIMICS_FLAG)) {
    dataControl0.Data = (*channelNvList)[ch].dataControl0;
    dataControl0.Bits.longpreambleenable = 1;

    // SKX change for upper nibble
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      if (set == TRUE) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      } else {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl0);
      } // if set
    } // strobe loop
  } // skip training
} // SetLongPreamble

void
DecIoLatency(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  UINT16   centerPoint[MAX_CH][MAX_STROBE]
)
{
  UINT8    LogicDelayIsOne;
  UINT8    strobe;
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct RankCh  (*rankPerCh)[MAX_RANK_CH];
  struct channelNvram (*channelNvList)[MAX_CH];

  LogicDelayIsOne = 0;

  channelNvList = GetChannelNvList(host, socket);
  rankPerCh = &(*channelNvList)[ch].rankPerCh;
  rankList  = GetRankNvList(host, socket, ch, dimm);

  //
  // If all logic-delays are 1, decrement IO_latency by 1, change all logic-delays to 0, no change in round-trip
  //

  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    //
    // Skip if this is an ECC strobe when ECC is disabled
    //
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) {
      //
      // Increment this for the non present strobe
      //
      LogicDelayIsOne++;
      continue;
    }
    if (centerPoint[ch][strobe] >= MAX_PHASE_IN_FINE_ADJUSTMENT) {
      LogicDelayIsOne++;
    }
  } // strobe loop

  //
  // If all logic-delays are 1, decrement IO_latency by 1, change all logic-delays to 0, no change in round-trip
  //
  if (LogicDelayIsOne == MAX_STROBE) {
    (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency -= 1;
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      centerPoint[ch][strobe] = centerPoint[ch][strobe] - 64;
      GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_UPDATE_CACHE,
                      (INT16 *)&centerPoint[ch][strobe]);
    } // strobe
  } // LogicDelayIsOne
} // DecIoLatency

void
SetupRecEnFineParam(
                     PSYSHOST host,
                     TCPGCAddress *CPGCAddress,
                     TWDBPattern  *WDBPattern,
                     UINT8 *loopCount,
                     UINT8 *numCl
)
{
  *loopCount = REC_EN_LOOP_COUNT+1;
  *numCl     = 32;
} // SetupRecEnFineParam

void
SetupPiSettingsParams(
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    step,
                 UINT16   *piSetting,
                 UINT16   *piStart,
                 UINT16   *NumberOfCycle,
                 struct TrainingResults  (**trainRes)[MAX_CH][MAX_STROBE]
)
{
  if (step == FPT_PI_WRITE_TYPE) {
    *piSetting     = WR_LVL_PI_START;
    *piStart       = WR_LVL_PI_START;
    *NumberOfCycle = WR_LVL_PI_START + WR_LVL_PI_RANGE;
#ifdef LRDIMM_SUPPORT
  } else if (step == FPT_PI_LRDIMM_READ_TYPE) {
    *piSetting     = LRDIMM_BACKSIDE_PI_START;
    *piStart       = LRDIMM_BACKSIDE_PI_START;
    *NumberOfCycle = LRDIMM_BACKSIDE_PI_START + LRDIMM_BACKSIDE_PI_RANGE;
    *trainRes       = &host->var.mem.lrTrainRes;
  } else if (step == FPT_PI_LRDIMM_WRITE_TYPE) {
    *piSetting     = LRDIMM_BACKSIDE_PI_START;
    *piStart       = LRDIMM_BACKSIDE_PI_START;
    *NumberOfCycle = LRDIMM_BACKSIDE_PI_START + LRDIMM_BACKSIDE_PI_RANGE;
    *trainRes       = &host->var.mem.lrDwlTrainRes;
  } else if (step == FPT_PI_LRDIMM_RD_MRD_TYPE) {
    *piSetting     = LRDIMM_BACKSIDE_PI_START;
    *piStart       = LRDIMM_BACKSIDE_PI_START;
    *NumberOfCycle = LRDIMM_BACKSIDE_PI_START + LRDIMM_BACKSIDE_PI_RANGE;
    *trainRes       = &host->var.mem.lrMrdTrainRes;
  } else if (step == FPT_PI_LRDIMM_WR_MRD_TYPE) {
    *piSetting     = LRDIMM_BACKSIDE_PI_START;
    *piStart       = LRDIMM_BACKSIDE_PI_START;
    *NumberOfCycle = LRDIMM_BACKSIDE_PI_START + LRDIMM_BACKSIDE_PI_RANGE;
    *trainRes       = &host->var.mem.lrMwdTrainRes;
#endif  //LRDIMM_SUPPORT
  } else {
    *piSetting     = REC_EN_PI_START;
    *piStart       = REC_EN_PI_START;
    *NumberOfCycle = REC_EN_PI_START + REC_EN_PI_RANGE;
  }
} // SetupPiSettings

UINT8
GetTrainingResultIndex(
                       UINT8  step,
                       UINT16 piSetting,
                       UINT16 piStart
)
{
  UINT8 dWord;

  if (step == FPT_PI_WRITE_TYPE) {
    dWord = (UINT8)((piSetting - WR_LVL_PI_START) / 32);
#ifdef LRDIMM_SUPPORT
  } else if ((step == FPT_PI_LRDIMM_READ_TYPE) || (step == FPT_PI_LRDIMM_WRITE_TYPE)) {
    dWord = (UINT8)((piSetting - LRDIMM_BACKSIDE_PI_START) / 32);
#endif
  } else {
    dWord = (UINT8)((piSetting - REC_EN_PI_START) / 32);
  }

  return dWord;
} // GetTrainingResultIndex

UINT32
ReadTrainFeedback (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch,
                   UINT8    strobe
)
{
  UINT32 feedback;
  // SKX change
  feedback =  MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG)) & 0x1FF;
  return feedback;

} // ReadTrainFeedback

void
RecEnTrainFeedback (
                    PSYSHOST host,
                    UINT8    socket,
                    UINT8    ch,
                    UINT8    strobe,
                    UINT32   csrReg,
                    UINT16   piDelay
)
{
  UINT8  step;
  UINT8  dWord;
  UINT8  minGood;
  struct TrainingResults  (*trainRes)[MAX_CH][MAX_STROBE];

  trainRes = &host->var.mem.trainRes;
  minGood  = SetMinGoodInit(host);
  dWord    = (UINT8)((piDelay - REC_EN_PI_START) / 32);

  if ((csrReg & 0x1FF) >= minGood) {
    for (step = 0; step < REC_EN_STEP_SIZE; step++) {
      (*trainRes)[ch][strobe].results[dWord] = (*trainRes)[ch][strobe].results[dWord] | (1 << ((piDelay + step) % 32));
    } // step loop
  }

} // RecEnTrainFeedback

UINT16
StartBackOver (
                UINT8  step,
                UINT16 piStart
)
{
  UINT16 piSetting;

  if (step == FPT_PI_WRITE_TYPE) {
    piSetting = WR_LVL_PI_START;
  } else {
    piSetting = REC_EN_PI_START;
  }

  return piSetting;
} // StartBackOver

void
SetInitRoundTrip(
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT8    dimm,
                 UINT8    rank
)
{
  UINT8  rtDelay;
  UINT8  logRank;
  struct ddrRank        (*rankList)[MAX_RANK_DIMM];
  struct RankCh         (*rankPerCh)[MAX_RANK_CH];
  struct channelNvram   (*channelNvList)[MAX_CH];
  struct dimmNvram      (*dimmNvList)[MAX_DIMM];

  rankList = GetRankNvList(host, socket, ch, dimm);
  channelNvList = GetChannelNvList(host, socket);
  rankPerCh = &(*channelNvList)[ch].rankPerCh;
  dimmNvList = GetDimmNvList(host, socket, ch);

  //
  // Get the logical rank #
  //
  logRank = GetLogicalRank(host, socket, ch, dimm, rank);

  // Default ROUNDT_LAT = HW_ROUNDT_LAT_DEFAULT_VALUE + 2* tCL
  if (!(*dimmNvList)[dimm].aepDimmPresent) {
    rtDelay = (UINT8) host->var.mem.rtDefaultValue + 2*((*channelNvList)[ch].common.nCL);
  } else {
    rtDelay = (UINT8) host->var.mem.rtDefaultValue + 2*(DdrtCASLatencyRDIMM[host->nvram.mem.socket[socket].ddrtFreq]) + 2*(DdrtCASLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq]);
  }
  //MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n Initial RT = %d\n", rtDelay));

  (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip = rtDelay;
  SetRoundTrip (host, socket, ch, logRank, rtDelay);

  (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency = MRC_IO_LATENCY_DEFAULT_VALUE;
  SetIOLatency (host, socket, ch, logRank, MRC_IO_LATENCY_DEFAULT_VALUE);
} // SetInitRoundTrip

void
SetIoLatComp(
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch
)
{
  struct channelNvram             (*channelNvList)[MAX_CH];
  IOLATENCY1_MCDDC_DP_STRUCT      ioLatency1;

  channelNvList = GetChannelNvList(host, socket);
  // IO Latency Compensation starting point
  ioLatency1.Data = MemReadPciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG);
  ioLatency1.Bits.io_lat_io_comp = MRC_ROUND_TRIP_IO_COMP_START;
  MemWritePciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG, ioLatency1.Data);
  (*channelNvList)[ch].ioLatency1 = ioLatency1.Data;
} // SetIoLatComp

UINT8
SetMinGoodInit (
                PSYSHOST host
)
{
  return (UINT8)(1 << (REC_EN_LOOP_COUNT - 1));
} // SetMinGoodInit

//
// MemDDR4Lrdimm project specific
//

UINT32
LrDimmReadTrainFeedback (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch,
                   UINT8    strobe,
                   UINT8    bit
)
{
  UINT32 csrReg;
  UINT32 feedback;

  // SKX HSD 4928153 DQ swizzle, normally this would be a straight up read but on SKX the bits are swizzled
  // nibble0 bits 0,1 are located in nibble1 bits 0,1 and vise-versa. Bits 2,3 for nibble0 and nibble1 are where they are supposed to be.
  //csrReg   = ((MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG)) & 0xc) |
  //        (MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe+9)%18, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG)) & 0x3));
  csrReg = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG));

  if (bit == 0xFF) {
    feedback = (csrReg & 0xF);
  } else {
    feedback = ((csrReg&0x01) << bit);
  }
  return feedback;
} // LrDimmReadTrainFeedback

UINT8
IsLrdimmPresent (
                    PSYSHOST host,
                    UINT8    socket,
                    UINT8    ch,
                    UINT8    dimm
                    )
{
   return (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].lrDimmPresent);
} // IsLrdimmPresent

void
SetDisable2cycBypass (
                       PSYSHOST host,
                       UINT8    socket,
                       UINT8    data
)
{
  UINT8 ch;
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT    chknBit;
  struct channelNvram                  (*channelNvList)[MAX_CH];

  chknBit.Data = 0;

  channelNvList = GetChannelNvList(host, socket);


  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    chknBit.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
    chknBit.Bits.dis_2cyc_byp = data;
    MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, chknBit.Data);
  } //ch
} // Disable2cycBypass

//
// memODT project specific
//
static UINT32  rdOdtTableRegs[MAX_DIMM3] = {
                                       RD_ODT_TBL0_MCDDC_CTL_REG,
                                       RD_ODT_TBL1_MCDDC_CTL_REG,
                                       RD_ODT_TBL2_MCDDC_CTL_REG
                                    };

static UINT32  wrOdtTableRegs[MAX_DIMM3] = {
                                       WR_ODT_TBL0_MCDDC_CTL_REG,
                                       WR_ODT_TBL1_MCDDC_CTL_REG,
                                       WR_ODT_TBL2_MCDDC_CTL_REG
                                    };

UINT16
SetOdtConfigInOdtValueIndex (
                              PSYSHOST host,
                              UINT8    socket
)
{
  ODT_VALUE_INDEX config;
  // Construct config index
  config.Data = 0;
  //SKX hack
  config.Bits.slot2 = EMPTY_DIMM;
  config.Bits.freq = GetClosestFreq(host, socket);

  // Check for frequencies above 2933
   if(config.Bits.freq > DDR_2933) {
     config.Bits.freq = DDR_2933;
   }

   // Check for frequencies below 1866
  if(config.Bits.freq < DDR_1866) {
    config.Bits.freq = DDR_1866;
  }

  return config.Data;
} // SetOdtConfigInOdtValueIndex


//---------------------------------------------------------------
/**

  Program ODT Timing

  @param host        - Pointer to sysHost
  @param socket        - Socket number
  @param ch          - Channel number (0-based)

  @retval N/A

**/
void
SetOdtTiming (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  )
{
  RD_ODT_TBL2_MCDDC_CTL_STRUCT rdOdtTiming;
  WR_ODT_TBL2_MCDDC_CTL_STRUCT wrOdtTiming;


  //
  // Write the Read and Write ODT Activates to the Matrix registers
  //
  rdOdtTiming.Data = MemReadPciCfgEp (host, socket, ch, RD_ODT_TBL2_MCDDC_CTL_REG);
  wrOdtTiming.Data = MemReadPciCfgEp (host, socket, ch, WR_ODT_TBL2_MCDDC_CTL_REG);

  rdOdtTiming.Bits.extraleadingodt = ODT_RD_CYCLE_LEADING;
  rdOdtTiming.Bits.extratrailingodt = ODT_RD_CYCLE_TRAILING;

  wrOdtTiming.Bits.extra_leading_odt = ODT_WR_CYCLE_LEADING;
  if (host->nvram.mem.socket[socket].channelList[ch].lrDimmPresent) {
    wrOdtTiming.Bits.extra_leading_odt = DDR4_LRDIMM_ODT_WR_CYCLE_LEADING;
    wrOdtTiming.Bits.extra_trailing_odt = DDR4_LRDIMM_ODT_WR_CYCLE_TRAILING;
  } else {
    wrOdtTiming.Bits.extra_trailing_odt = ODT_WR_CYCLE_TRAILING;
  }

  //If Read 2tCK Preamble Add 1 or 2 to ODT pulse width
  if(host->var.mem.read2tckCL[ch] > 0){
    if ((host->var.mem.read2tckCL[ch] + rdOdtTiming.Bits.extratrailingodt) < 4 ) {
      rdOdtTiming.Bits.extratrailingodt += host->var.mem.read2tckCL[ch];
    } else {
      DisableChannel(host, socket, ch);
      OutputWarning (host, WARN_MEMORY_TRAINING, WARN_ODT_TIMING_OVERFLOW, socket, ch, NO_DIMM, NO_RANK);
    }
  }
  //If Write 2tCK Preamble Add 1 or 2 to ODT pulse width
  if(host->var.mem.write2tckCWL[ch] > 0){
    if ((host->var.mem.write2tckCWL[ch] + wrOdtTiming.Bits.extra_trailing_odt) < 4 ){
      wrOdtTiming.Bits.extra_trailing_odt += host->var.mem.write2tckCWL[ch];
    } else {
      DisableChannel(host, socket, ch);
      OutputWarning (host, WARN_MEMORY_TRAINING, WARN_ODT_TIMING_OVERFLOW, socket, ch, NO_DIMM, NO_RANK);
    }
  }

  //For WRCRC
  if (host->setup.mem.optionsExt & WR_CRC) {
    wrOdtTiming.Data = MemReadPciCfgEp (host, socket, ch, wrOdtTableRegs[2]);
    if (wrOdtTiming.Bits.extra_trailing_odt < 3) {
      wrOdtTiming.Bits.extra_trailing_odt++;
    } else {
      DisableChannel(host, socket, ch);
      OutputWarning (host, WARN_MEMORY_TRAINING, WARN_ODT_TIMING_OVERFLOW, socket, ch, NO_DIMM, NO_RANK);
    }
    MemWritePciCfgEp (host, socket, ch, wrOdtTableRegs[2], wrOdtTiming.Data);
  }

  //
  // Write the Read and Write ODT Activates to the Matrix registers
  //
  MemWritePciCfgEp (host, socket, ch, RD_ODT_TBL2_MCDDC_CTL_REG, rdOdtTiming.Data);
  MemWritePciCfgEp (host, socket, ch, WR_ODT_TBL2_MCDDC_CTL_REG, wrOdtTiming.Data);
} // SetOdtTiming

void
SetDdr3OdtMatrix (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch,
                   UINT8    dimm,
                   UINT8    rank
)
{
} // SetDdr3OdtMatrix

//---------------------------------------------------------------
struct odtValueStruct *LookupOdtValue (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  )
{
   // Return pointer
   return NULL;
} // LookupOdtValue

void
GetOdtActTablePointers (
                         PSYSHOST host,
                         UINT32   *odtActTableSize,
                         struct odtActStruct **odtActTablePtr
)
{
   *odtActTablePtr = ddr4OdtActTable;
   *odtActTableSize = sizeof(ddr4OdtActTable);
} // GetOdtActTablePointers


void
SetOdtActMatrixRegs (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch,
                   UINT8    dimm,
                   UINT32   rdOdtTableData,
                   UINT32   wrOdtTableData
)
{
  //
  // Write the Read and Write ODT Activates to the Matrix registers
  //
  MemWritePciCfgEp (host, socket, ch, rdOdtTableRegs[dimm], rdOdtTableData);
  MemWritePciCfgEp (host, socket, ch, wrOdtTableRegs[dimm], wrOdtTableData);
} // OdtActMatrixRegs

//
// SKX specific functions can maybe move to somewere else (functions not use by KNL/HSX)
//

void
SetSenseAmpODTDelays (
              PSYSHOST  host,
              UINT8     socket
              )
{
  UINT8           ch,dimm,rank,strobe,longpreamble;
  INT16           delay;
  UINT32          Senseampdelay;
  UINT32          SenseAmpDuration;
  INT16           MaxRcven;
  INT16           MinRcven;
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct dimmNvram                      (*dimmNvList)[MAX_DIMM];
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT    dataControl0;
  DATACONTROL1N0_0_MCIO_DDRIO_STRUCT    dataControl1;

  channelNvList = GetChannelNvList(host, socket);
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n Starting Senseamp and ODT delay and duration calculations \n"));
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "------------------------------------------------------------ \n"));
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dataControl0.Data = (*channelNvList)[ch].dataControl0;
    longpreamble = dataControl0.Bits.longpreambleenable & 0x1;
    dimmNvList = GetDimmNvList(host, socket, ch);

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      // clear out min and max values
      MaxRcven=0;
      MinRcven=0xfff;
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        //for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY, &delay);
          //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT, "##Debug %d \n",delay));
          if (delay < MinRcven) {
            MinRcven = delay;
          }
          if (delay > MaxRcven) {
            MaxRcven = delay;
          }
        } //rank

      } //dimm

      // calculate delays
      // Receive Enable -long preamble - 1 (extra hold time req)
      Senseampdelay = (16 + MinRcven/64 - (2*longpreamble) -1)%16;
      // Receive Enable +1(Extend pulse 1 early) +2(for pre/postamble) +long preamble +1(roundup)
      SenseAmpDuration = MaxRcven/64 - MinRcven/64 + 1 + 2 + (2*longpreamble) + 1;

      if (SenseAmpDuration>7) {
        SenseAmpDuration=7;
      }
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, strobe, NO_BIT,
      "Longpreamble = %d, MinRcven=%3d, MaxRcven=%3d: Senseampdelay=%d SenseAmpDuration=%d\n",longpreamble,MinRcven,MaxRcven,Senseampdelay,SenseAmpDuration));

      //
      //program new values
      //

      //read cached values
      dataControl1.Data = (*channelNvList)[ch].dataControl1[strobe];

      //modify with new values
      dataControl1.Bits.odtdelay          = Senseampdelay;
      dataControl1.Bits.senseampdelay     = Senseampdelay;
      dataControl1.Bits.odtduration       = SenseAmpDuration;
      dataControl1.Bits.senseampduration  = SenseAmpDuration;

      //write new values
      (*channelNvList)[ch].dataControl1[strobe] =  dataControl1.Data;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL1N0_0_MCIO_DDRIO_REG), dataControl1.Data);

    } //strobe
  } //ch
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n\n"));
}  // SetSenseAmpODTDelays


//
// MemEarlyRid project specific
//

void
ClearEridLfsrErr (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch
)
{
  CPGC_ERID_LFSR_ERR_MC_2LM_STRUCT    errResult;

  errResult.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_ERR_MC_2LM_REG);
  errResult.Bits.status = 0;
  errResult.Bits.count = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_ERID_LFSR_ERR_MC_2LM_REG, errResult.Data);
} // ClearEridLfsrErr

void
GetSetSxpGnt2Erid (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT8    dimm,
                 UINT8    mode,
                 INT16   *data
)
{
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimm0BasicTiming;

  struct channelNvram                       (*channelNvList)[MAX_CH];
  channelNvList = GetChannelNvList(host, socket);

  if (mode & GSM_READ_ONLY) {
    switch(dimm) {

      case 0:
        ddrtDimm0BasicTiming.Data = (*channelNvList)[ch].ddrtDimm0BasicTiming;
        *data = (INT16)ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid;
        break;

      case 1:
        ddrtDimm0BasicTiming.Data = (*channelNvList)[ch].ddrtDimm1BasicTiming;
        *data = (INT16)ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid;
        break;

      default:
        break;
    } // switch dimm
  } else {
    switch(dimm) {

      case 0:
        ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG);
        ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid = *data;
        MemWritePciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG, ddrtDimm0BasicTiming.Data);
        (*channelNvList)[ch].ddrtDimm0BasicTiming = ddrtDimm0BasicTiming.Data;
        break;

      case 1:
        ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG);
        ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid = *data;
        MemWritePciCfgEp (host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG, ddrtDimm0BasicTiming.Data);
        (*channelNvList)[ch].ddrtDimm1BasicTiming = ddrtDimm0BasicTiming.Data;
        break;

      default:
        break;
    } // switch dimm
  }
} // GetSetSxpGnt2Erid

void
GetWpqRidtoRt (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT8    dimm
)
{
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimm0BasicTiming;
  T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_STRUCT   ddrtDimm1BasicTiming;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT           ddrtMiscDelay;
  ROUNDTRIP0_MCDDC_DP_STRUCT                ddrtRtRank0;
  ROUNDTRIP1_MCDDC_DP_STRUCT                ddrtRtRank4;

  ddrtMiscDelay.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);

  switch(dimm) {

    case 0:
      ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG);
      ddrtRtRank0.Data = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP0_MCDDC_DP_REG);
      ddrtMiscDelay.Bits.wpq_rid_to_rt_ufill = (ddrtRtRank0.Bits.rt_rank0 >> 1) - ddrtDimm0BasicTiming.Bits.t_ddrt_gnt2erid - 5;
      MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);
      break;

    case 1:
      ddrtDimm1BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM1_BASIC_TIMING_MC_2LM_REG);
      ddrtRtRank4.Data = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP1_MCDDC_DP_REG);
      ddrtMiscDelay.Bits.wpq_rid_to_rt_ufill = (ddrtRtRank4.Bits.rt_rank4 >> 1) - ddrtDimm1BasicTiming.Bits.t_ddrt_gnt2erid - 5;
      MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);
      break;

    default:
      break;
  } // switch dimm
} // SetWpqRidtoRt

UINT16
GetDdrCrDdrt(
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch
)
{
  return (UINT16)MemReadPciCfgEp (host, socket, ch, DDRCRDDRTTRAINRESULT_MCIO_DDRIOEXT_REG);
} // GetDdrCrDdrtResult

UINT8
GetDdrtTrainResult(
                    PSYSHOST host,
                    UINT8    socket,
                    UINT8    ch,
                    UINT8    dimm
)
{
  DDRCRDDRTTRAINRESULT_MCIO_DDRIOEXT_STRUCT ddrtTrainResult;

  ddrtTrainResult.Data = GetDdrCrDdrt( host, socket, ch);

  return (UINT8)(ddrtTrainResult.Bits.evenoddsamples >> (dimm * 4));
} // GetDdrCrDdrtResult

void
GetEridCombineResult(
                      PSYSHOST host,
                      UINT8    socket,
                      UINT8    ch,
                      UINT8    *eridCombinedResult
)
{
  CPGC_ERID_LFSR_ERR_MC_2LM_STRUCT    errResult;

  errResult.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_ERR_MC_2LM_REG);
  if ((errResult.Bits.status & 0x7F) == 0) {
    *eridCombinedResult = 1;
  } // if status
} // GetEridCombineResult

void
SetEridEnable (
                      PSYSHOST host,
                      UINT8    socket,
                      UINT8    ch,
                      UINT8    data
)
{
  DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_STRUCT clkRanksUsed;

  // The host DDRIO will be in an Early RID training mode (DDRTTraining=1) that captures sam-ples on every other DCLK,
  // and the PI for the sampling delay in the host DDRIO will be swept.
  clkRanksUsed.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
  clkRanksUsed.Bits.ddrttraining = data;
  MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, clkRanksUsed.Data);

} // SetEridEnable

void
SetupCoerseEridTraining (
                      PSYSHOST host,
                      UINT8    socket,
                      UINT8    ch,
                      UINT8    dimm
)
{
  CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT    cpgcDdrtMiscCtl;
  //DDRT_CREDIT_LIMIT_MC_2LM_STRUCT     ddrtCreditLimit;
  CPGC_ERID_LFSR_INIT0_MC_2LM_STRUCT  cpgcEridLfsrInit0;
  CPGC_ERID_LFSR_INIT1_MC_2LM_STRUCT  cpgcEridLfsrInit1;
  CPGC_ERID_LFSR_CTL_MC_2LM_STRUCT    cpgcEridLfsrCtl;
  CPGC_ERID_LFSR_ERR_MC_2LM_STRUCT    cpgcEridLfsrErr;

  cpgcDdrtMiscCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG);
  cpgcDdrtMiscCtl.Bits.cpgc_max_credit = 0x28;  //training doc has value = 32
  cpgcDdrtMiscCtl.Bits.multi_credit_on = 1;
  cpgcDdrtMiscCtl.Bits.enable_erid_return  = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG, cpgcDdrtMiscCtl.Data);

  //ddrtCreditLimit.Data = MemReadPciCfgEp (host, socket, ch, DDRT_CREDIT_LIMIT_MC_2LM_REG);
  //ddrtCreditLimit.Bits.ddrt_rd_credit = 0;
  //ddrtCreditLimit.Bits.ddrt_wr_credit = 0;
  //MemWritePciCfgEp (host, socket, ch, DDRT_CREDIT_LIMIT_MC_2LM_REG, ddrtCreditLimit.Data);

  cpgcEridLfsrInit0.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_INIT0_MC_2LM_REG);
  cpgcEridLfsrInit0.Bits.seed = 0x05A5A5;
  MemWritePciCfgEp (host, socket, ch, CPGC_ERID_LFSR_INIT0_MC_2LM_REG, cpgcEridLfsrInit0.Data);

  cpgcEridLfsrInit1.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_INIT1_MC_2LM_REG);
  cpgcEridLfsrInit1.Bits.seed = 0x0C6C6C;
  MemWritePciCfgEp (host, socket, ch, CPGC_ERID_LFSR_INIT1_MC_2LM_REG, cpgcEridLfsrInit1.Data);

  cpgcEridLfsrCtl.Data =  MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_CTL_MC_2LM_REG);
  cpgcEridLfsrCtl.Bits.enable_erid_lfsr_training = 1;
  cpgcEridLfsrCtl.Bits.mode0 = 0;
  cpgcEridLfsrCtl.Bits.mode1 = 0;
  cpgcEridLfsrCtl.Bits.stop_on_err0 = 0;
  cpgcEridLfsrCtl.Bits.stop_on_err1 = 0;
  cpgcEridLfsrCtl.Bits.reset0 = 0;
  cpgcEridLfsrCtl.Bits.reset1 = 0;
  cpgcEridLfsrCtl.Bits.load0 = 0;
  cpgcEridLfsrCtl.Bits.load1 = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_ERID_LFSR_CTL_MC_2LM_REG, cpgcEridLfsrCtl.Data);

  cpgcEridLfsrErr.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_ERR_MC_2LM_REG);
  cpgcEridLfsrErr.Bits.mask = 0x80;
  MemWritePciCfgEp (host, socket, ch, CPGC_ERID_LFSR_ERR_MC_2LM_REG, cpgcEridLfsrErr.Data);
} // SetupCoerseEridTraining

void
DisableEridLfsrMode (
                      PSYSHOST host,
                      UINT8    socket,
                      UINT8    ch
)
{
  CPGC_ERID_LFSR_CTL_MC_2LM_STRUCT    cpgcEridLfsrCtl;
  //
  // Disable ERID LFSR training mode
  //
  cpgcEridLfsrCtl.Data =  MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_CTL_MC_2LM_REG);
  cpgcEridLfsrCtl.Bits.enable_erid_lfsr_training = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_ERID_LFSR_CTL_MC_2LM_REG, cpgcEridLfsrCtl.Data);
} // DisableEridLfsrMode

void
SetupCpgcWdbBuffErid (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch
)
{

  UINT32             sequencerPattern[3];
  SequencerMode      sequencerMode;

  sequencerMode.Data = 0;
  sequencerMode.Bits.seqMode0 = BTBUFFER;
  sequencerMode.Bits.seqMode1 = BTBUFFER;
  sequencerMode.Bits.seqMode2 = BTBUFFER;
  sequencerMode.Bits.eccDataSourceSel = 1;

  sequencerPattern[0] = DDR_CPGC_PATBUF_MUX0;
  sequencerPattern[1] = DDR_CPGC_PATBUF_MUX1;
  sequencerPattern[2] = DDR_CPGC_PATBUF_MUX2;

  SetupPatternSequencer (host, socket, ch, sequencerMode, sequencerPattern);

} // SetupCpgcWdbBuffErid

void
CollectTestResultsErid (
                         PSYSHOST host,
                         UINT8    socket,
                         UINT8    ch,
                         UINT8    dimm,
                         UINT16   offset,
                         UINT8    index,
                         UINT16   eridOffset,
                         UINT8    *gnt2eridPassed
)
{
  // Collect test results
  CPGC_ERID_LFSR_ERR_MC_2LM_STRUCT    errResult;
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];

  dimmNvList     = GetDimmNvList(host, socket, ch);
  errResult.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERID_LFSR_ERR_MC_2LM_REG);
  if ((errResult.Bits.status & 0x7F) == 0) {

    // Mark passing channels
    *gnt2eridPassed = 1;

    // Update result
    (*dimmNvList)[dimm].gnt2erid = (*dimmNvList)[dimm].gnt2erid + offset;

  } else {
    *gnt2eridPassed = 0;
  }
} // CollectTestResultsErid

//------------------------------------------------------------------------------------

void
TxPiSampleChipInit(
  PCHIP_DDRIO_STRUCT dataControl
  )
{
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT   dataControl0;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT   dataControl0_1; //KV3
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT   dataControl2;

  dataControl0.Data = 0;
  dataControl2.Data = 0;
  dataControl0.Bits.wltrainingmode  = 1;
  dataControl0.Bits.readrfwr        = 1;
  dataControl0.Bits.txpion          = 1;
  dataControl2.Bits.forcebiason     = 1;
  dataControl0_1.Data = 0; //KV3
  dataControl0_1.Bits.wltrainingmode  = 1; //KV3

  dataControl->dataControl0.Data = dataControl0.Data;
  dataControl->dataControl2.Data = dataControl2.Data;
  dataControl->dataControl0_1.Data = dataControl0_1.Data; //KV3
}

void
GetChipLogicalRank(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank,
  PCHIP_DDRIO_STRUCT dataControl
  )
{
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT   dataControl0;

  dataControl0.Data = dataControl->dataControl0.Data;
  dataControl0.Bits.readrfrank = GetLogicalRank(host, socket, ch, dimm, rank);
  dataControl->dataControl0.Data = dataControl0.Data;
}

void
SetChipTxPiSampleTrainingMode(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_DDRIO_STRUCT
  dataControl
  )
{
  UINT8 strobe;
  struct channelNvram     (*channelNvList)[MAX_CH];
  channelNvList = GetChannelNvList(host, socket);

  // SKX change for upper nibble
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    //
    // Set training mode and rank to all channels
    //
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl->dataControl2.Data | (*channelNvList)[ch].dataControl2[strobe]);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl->dataControl0.Data | (*channelNvList)[ch].dataControl0);
  } // strobe loop
}

void
GetChipTiPiSamplepiDelayStartandEnd(
  PSYSHOST host,
  UINT8 socket,
  INT16* piDelayStart,
  UINT16* piDelayEnd
  )
{
  *piDelayEnd = WR_LVL_PI_START + WR_LVL_PI_RANGE;
  *piDelayStart = WR_LVL_PI_START;
}


void
SetChipTxPiSampleTrainingTXDQSStrobesI(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank,
  INT16 piDelay,
  PCHIP_DDRIO_STRUCT dataControl
  )
{
  UINT8 strobe;
  struct channelNvram     (*channelNvList)[MAX_CH];
  channelNvList = GetChannelNvList(host, socket);

  // SKX change for upper nibble
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    //
    // Set training mode and rank to all channels
    //
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl->dataControl0_1.Data | (*channelNvList)[ch].dataControl0); //KV3
  } // strobe loop

  GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, TxDqsDelay, GSM_UPDATE_CACHE, (INT16 *)&piDelay);
}

/**

  Chip specific setup for TxPiSample TXDQS Strobe training, part 2

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param ch  - Processor socket Channel number
  @param dimm  - Processor socket channel dimm number
  @param dataControl0data - training mode data

  @retval N/A

**/
void
SetChipTxPiSampleTrainingTXDQSStrobesII(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  PCHIP_DDRIO_STRUCT dataControl
  )
{
  UINT8 strobe;
  struct channelNvram     (*channelNvList)[MAX_CH];
  channelNvList = GetChannelNvList(host, socket);

  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    //
    // Set training mode and rank to all channels
    //
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl->dataControl0.Data | (*channelNvList)[ch].dataControl0);
  } // strobe loop
}

void
EnableChipRecieveEnableMode(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 strobe,
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct
  )
{
  MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), executeCtlCLKTestChipStruct->dataControl0.Data);
}

void
EnableChipRecieveEnableModeII(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 strobe,
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct
  )
{
  struct channelNvram  (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl0);
}

void
PerformChipODTMPRDimmActivationStep(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct
  )
{
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];
  CPGC_MISCODTCTL_MCDDC_CHIP_STRUCT     cpgcMiscODTCtl;

  dimmNvList  = GetDimmNvList(host, socket, ch);

  cpgcMiscODTCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG);
  if (!(*dimmNvList)[dimm].aepDimmPresent) {
    cpgcMiscODTCtl.Bits.mpr_train_ddr_on = 1;
  }
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG, cpgcMiscODTCtl.Data);
  executeCtlCLKTestChipStruct->cpgcMiscODTCtl.Data = cpgcMiscODTCtl.Data;
}

void
DisableChipChlRecEnOffsetTrainingMode(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PEXECUTE_CTL_CLK_TEST_CHIP_STRUCT executeCtlCLKTestChipStruct
  )
{
  CPGC_MISCODTCTL_MCDDC_CHIP_STRUCT     cpgcMiscODTCtl;

  cpgcMiscODTCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG);
  cpgcMiscODTCtl.Bits.mpr_train_ddr_on = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG, cpgcMiscODTCtl.Data);
  executeCtlCLKTestChipStruct->cpgcMiscODTCtl.Data = cpgcMiscODTCtl.Data;
}

/**

  Chip specific code path to determine if DDR4 LRDIM Host side training should be enabled. Disables legacy host side training if it has been enabled

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param ch  - Processor socket Channel number
  @param dimm  - Processor socket channel dimm number
  @param rank - Processor socket channel dimm rank number

  @retval BOOLEAN - true if the chip should disable DDR4 LRDIM Host side training

**/
BOOLEAN
ShouldChipEnableDDR4LRDIMMHostSideTraining(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank
  )
{
  if (IsLrdimmPresent(host, socket, ch, dimm)){
    return TRUE;
  }
  return FALSE;
}

/**

  Chip specific code path to support each platforms individual set of RTT_NOM values

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param ch  - Processor socket Channel number
  @param dimm  - Processor socket channel dimm number
  @param rank - Processor socket channel dimm rank number

  @retval N/A

**/
void
SetChipRTTPark(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank
  )
{
  UINT16              MR5;
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  if ((host->nvram.mem.dramType == SPD_TYPE_DDR4) && (*rankList)[rank].RttWr & DDR4_RTT_WR_ODT_MASK) {
    //
    // Get the current settings for MR5
    //
    MR5 = (*rankStruct)[rank].MR5;
    MR5 &= ~DDR4_RTT_PRK_ODT_MASK;
    // Set Rtt Park to the same value as Rtt Wr
    switch ((*rankList)[rank].RttWr >> DDR4_RTT_WR_ODT_SHIFT) {

    case DDR4_RTT_WR_80:
      MR5 |= (DDR4_RTT_NOM_80 << DDR4_RTT_PRK_ODT_SHIFT);
      break;

    case DDR4_RTT_WR_120:
      MR5 |= (DDR4_RTT_NOM_120 << DDR4_RTT_PRK_ODT_SHIFT);
      break;

    case DDR4_RTT_WR_240:
      MR5 |= (DDR4_RTT_NOM_240 << DDR4_RTT_PRK_ODT_SHIFT);
      break;

    case DDR4_RTT_WR_INF:
      MR5 |= (DDR4_RTT_NOM_INF << DDR4_RTT_PRK_ODT_SHIFT);
      break;

    default:
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_63);
      break;
    } // switch
    WriteMRS (host, socket, ch, dimm, rank, MR5, BANK5);
  } // ddr4
}

/**

  Chip specific code path to Disable any legacy LRDRIM WriteLeveling and return false, or return true if should disable DDR4 write leveling.

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param ch  - Processor socket Channel number
  @param dimm  - Processor socket channel dimm number
  @param rank - Processor socket channel dimm rank number

  @retval BOOLEAN - true if the chip should disable DDR4 write leveling

**/
BOOLEAN
ShouldChipDisableDDR4LRDIMMWriteLeveling(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank
  )
{
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];

  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  if(IsLrdimmPresent(host, socket, ch, dimm) &&
    ((*rankStruct)[rank].CurrentLrdimmTrainingMode != LRDIMM_DWL_TRAINING_MODE) &&
    (host->var.mem.InPbaWaMode == 0))
  {return TRUE;}

  return FALSE;
}

/**

  Chip specific code path.This chip only supports DDR4, so it has a different code path for DDR4 that requires RTT_PARK restored. DDR3 does not need this.

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param ch  - Processor socket Channel number
  @param dimm  - Processor socket channel dimm number
  @param rank - Processor socket channel dimm rank number

  @retval BOOLEAN - true if the chip should disable DDR4 write leveling

**/
void
DoChipCompatibleRTT_PARKRestore(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank
  )
{
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];

  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  //
  // Restore RTT_PARK
  //
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
}



/*++

Routine Description:

  Get's the results from the current test

Arguments:

  host      - Pointer to sysHost
  dimm      - DIMM number
  rank      - rank number of the DIMM

Returns:

  N/A

--*/
void
GetResultsWL (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm,
  UINT8     rank,
  UINT16    piDelay
  )
{
  UINT8               ch;
  UINT8               strobe;
  UINT8               dWord;
  UINT8               step;
  UINT32              csrReg;
  UINT16                  piDelayStart;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct TrainingResults  (*trainRes)[MAX_CH][MAX_STROBE];

  trainRes      = &host->var.mem.trainRes;
  channelNvList = GetChannelNvList(host, socket);

  if ((host->nvram.mem.socket[socket].lrDimmPresent) && (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {
    piDelayStart = 128;
  } else {
    piDelayStart = WR_LVL_PI_START;
  }

  dWord = (UINT8)((piDelay - piDelayStart) / 32);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    //SKX change for upper strobes in N1 registers
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {

      if (!(*dimmNvList)[dimm].x4Present && (strobe >= 9)) continue;

      csrReg = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG));

      if ((csrReg & 0x1FF) >= 16) {
        for (step = 0; step < WR_LVL_STEP_SIZE; step++) {
          (*trainRes)[ch][strobe].results[dWord] = (*trainRes)[ch][strobe].results[dWord] | (1 << ((piDelay + step) % 32));
        }
      }

    } // strobe loop
  } // ch loop
} // GetResults

/**

  this function execute the write leveling Cleanup.
  Center TxDQS-CLK timing

  @param host    - Pointer to sysHost
  @param socket  - Socket number

**/
void
UpdateGlobalOffsets (
  PSYSHOST  host,
  UINT8     ch,
  INT16     TargetOffset,
  INT16     *CRAddDelay,
  INT16     *GlobalByteOff
  )
{
  // Calculate offsets
  *GlobalByteOff = 0;
  if (TargetOffset > 7) {
    *CRAddDelay = 7;
    *GlobalByteOff = 128 * (TargetOffset - 7);
  } else if (TargetOffset < -3) {
    *CRAddDelay = -3;
    *GlobalByteOff = 128 * (TargetOffset + 3);
  } else {
    *CRAddDelay = TargetOffset;
  }
} // UpdateGlobalOffsets

UINT8  GetChipWriteLevelingCleanUpOffset(PSYSHOST  host)
{
  //BoilerPlate
  return 0;
}

/**

  Chip specific code path. differences in the sequencer setup were moved to hooks

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param ch  - Processor socket Channel number
  @param dimm  - Processor socket channel dimm number
  @param rank - Processor socket channel dimm rank number

  @retval - VOID

**/
void
SetupChipPatternSequencer(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank
  )
{
  UINT32 chBitmask;
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT  patWDBCLMuxCtl;

  chBitmask = GetChBitmask (host, socket, dimm, rank);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ( !((1 << ch) & chBitmask )) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    if (rank >= (*dimmNvList)[dimm].numRanks) continue;
    // ECC, Bit Buffer
    patWDBCLMuxCtl.Data = 0;
    patWDBCLMuxCtl.Bits.mux0 = BTBUFFER;
    patWDBCLMuxCtl.Bits.mux1 = BTBUFFER;
    patWDBCLMuxCtl.Bits.mux2 = BTBUFFER;
    patWDBCLMuxCtl.Bits.ecc_datasrc_sel = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, patWDBCLMuxCtl.Data);

    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG, 0xAAAAAA);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG, 0xCCCCCC);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG, 0xF0F0F0);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG, 0xAAAAAA);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG, 0xCCCCCC);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG, 0xF0F0F0);
  } // ch loop
}

/**

  Chip specific code path to initialize "GlobalByteOff"

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param CRAddDelay  -
  @param GlobalByteOff  -

  @retval - VOID

**/
void
SetChipDQDQSTiming(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  INT16 offsetValue,
  INT16* CRAddDelay,
  INT16* GlobalByteOff,
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
  )
{
  WLChipCleanUpStruct->tCOTHP.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
  WLChipCleanUpStruct->tCOTHP2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
  WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG);
  *GlobalByteOff = 0;
  UpdateGlobalOffsets(host, ch, offsetValue, CRAddDelay, GlobalByteOff);
}

void
WriteChipWrADDDelays(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  INT16 offsetValue,
  INT16* CRAddDelay,
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
  )
{
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT ddrtDimm0BasicTiming;
  TCOTHP_MCDDC_CTL_STRUCT tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT tCOTHP2;

  tCOTHP.Data = WLChipCleanUpStruct->tCOTHP.Data;
  tCOTHP2.Data = WLChipCleanUpStruct->tCOTHP2.Data;
  ddrtDimm0BasicTiming.Data = WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data;

  if (CRAddDelay[ch] < 0) {
    if (CRAddDelay[ch] < -3) {
      CRAddDelay[ch] = -3;
    }
    tCOTHP.Bits.t_cwl_adj = 0;
    tCOTHP2.Bits.t_cwl_adj_neg = ABS(CRAddDelay[ch]);
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj = 0;
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj_neg = ABS(CRAddDelay[ch]);
  } else {
    if (CRAddDelay[ch] > 7) {
        CRAddDelay[ch] = 7;
    }
    tCOTHP.Bits.t_cwl_adj = CRAddDelay[ch];
    tCOTHP2.Bits.t_cwl_adj_neg = 0;
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj = CRAddDelay[ch];
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj_neg = 0;
  }

  WLChipCleanUpStruct->tCOTHP.Data = tCOTHP.Data;
  WLChipCleanUpStruct->tCOTHP2.Data = tCOTHP2.Data;
  WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data = ddrtDimm0BasicTiming.Data;
  MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, WLChipCleanUpStruct->tCOTHP.Data);
  MemWritePciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG, WLChipCleanUpStruct->tCOTHP2.Data);
  MemWritePciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG, WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data);
}


void
ReadChipWrADDDelays(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  INT16* CRAddDelay,
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
  )
{
  TCOTHP_MCDDC_CTL_STRUCT tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT tCOTHP2;

  tCOTHP.Data = WLChipCleanUpStruct->tCOTHP.Data;
  tCOTHP2.Data = WLChipCleanUpStruct->tCOTHP2.Data;

  CRAddDelay[ch] = (INT16)tCOTHP.Bits.t_cwl_adj - (INT16)tCOTHP2.Bits.t_cwl_adj_neg;
}

/**

  Chip specific code path to getChipTimingData

  @param host  - Pointer to the system host (root) structure
  @param socket  - processor socket ID
  @param ch  - Processor socket Channel number

  @retval - VOID

**/
void
GetChipTimingData(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
  )
{
  WLChipCleanUpStruct->tCOTHP.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
  WLChipCleanUpStruct->tCOTHP2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
  WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG);
}

void
UpdateChipMCDelay(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  struct channelNvram (*channelNvList)[MAX_CH],
  INT16* CRAddDelay,
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
  )
{
  T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_STRUCT ddrtDimm0BasicTiming;
  TCOTHP_MCDDC_CTL_STRUCT tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT tCOTHP2;

  tCOTHP.Data = WLChipCleanUpStruct->tCOTHP.Data;
  tCOTHP2.Data = WLChipCleanUpStruct->tCOTHP2.Data;
  ddrtDimm0BasicTiming.Data = WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data;

  if (CRAddDelay[ch] < 0) {
    tCOTHP.Bits.t_cwl_adj = 0;
    tCOTHP2.Bits.t_cwl_adj_neg = ABS(CRAddDelay[ch]);

    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj = 0;
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj_neg = ABS(CRAddDelay[ch]);

  } else {
    tCOTHP.Bits.t_cwl_adj = CRAddDelay[ch];
    tCOTHP2.Bits.t_cwl_adj_neg = 0;
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj = CRAddDelay[ch];
    ddrtDimm0BasicTiming.Bits.t_ddrt_twl_adj_neg = 0;
  }

  WLChipCleanUpStruct->tCOTHP.Data = tCOTHP.Data;
  WLChipCleanUpStruct->tCOTHP2.Data = tCOTHP2.Data;
  WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data = ddrtDimm0BasicTiming.Data;
  MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, WLChipCleanUpStruct->tCOTHP.Data);
  MemWritePciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG, WLChipCleanUpStruct->tCOTHP2.Data);
  MemWritePciCfgEp (host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG, WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data);

  (*channelNvList)[ch].tcothp = WLChipCleanUpStruct->tCOTHP.Data;
  (*channelNvList)[ch].tcothp2 = WLChipCleanUpStruct->tCOTHP2.Data;
// Commented out due to HSD5330978
  (*channelNvList)[ch].ddrtDimm0BasicTiming = WLChipCleanUpStruct->ddrtDimm0BasicTiming.Data;
}

UINT32
CheckChipByteError(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm)
{
  UINT32                  Result;
  UINT8                   strobe;
  UINT8                   bit;
  UINT32                  errResult;
  UINT8                   maxStrobe;
  UINT8                   chStatus;
  UINT32                  bwSerr[MAX_CH + 1][3];

  chStatus = 1 << ch;
  bwSerr[ch][0] = 0;
  bwSerr[ch][1] = 0;
  bwSerr[ch][2] = 0;

  CollectTestResults (host, socket, chStatus, bwSerr);
  // Read out per byte error results and check for any byte error
  //          errResult = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_BYTE_MCDDC_CTL_REG);
  Result = 0;
  maxStrobe = GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel);

  for (strobe = 0; strobe < maxStrobe; strobe++) {

    bit = (strobe - 9 * (strobe >= 9))*8 + 4*(strobe >= 9);

    if (bit < 32) errResult = bwSerr[ch][0];
    else if (bit < 64) errResult = bwSerr[ch][1];
    else errResult = bwSerr[ch][2];
    //            if (errResult & (1 << (strobe * 2 - (strobe >= 9) * 17))) {
    if (errResult & (1 << (bit))) {
      Result |= 1 << strobe;
    }
  }
  return Result;
}

void
AssertChipODTOveride(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT32 wrOdtTableData
  )
{
  CPGC_MISCODTCTL_MCDDC_CTL_STRUCT cpgcMiscODTCtl;

  cpgcMiscODTCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG);
  cpgcMiscODTCtl.Bits.odt_override  = wrOdtTableData;
  cpgcMiscODTCtl.Bits.odt_on        = wrOdtTableData;
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG, cpgcMiscODTCtl.Data);
}

void
DeAssertChipODTOveride(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch
  )
{
  CPGC_MISCODTCTL_MCDDC_CTL_STRUCT cpgcMiscODTCtl;

  cpgcMiscODTCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG);
  cpgcMiscODTCtl.Bits.odt_override  = 0;
  cpgcMiscODTCtl.Bits.odt_on        = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG, cpgcMiscODTCtl.Data);
}

void
DoFinalChipSpecificWLCleanUp(
  PSYSHOST host,
  UINT8 socket
  )
{

  UINT8 ch;
  struct channelNvram  (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV_MCDDC_DP_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV0_MCDDC_DP_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV1_MCDDC_DP_REG, 0);
  } // ch loop

}

//---------------------------------------------------------------------------------------------------

#define PER_GROUP         0
#define COMPOSITE_GROUPS  1

/**

  Exectues command/clock training

  @param host      - Pointer to sysHost

  @retval N/A

**/
UINT32
ChipLateCmdClk(
          PSYSHOST  host
          )
{
  UINT8                               socket;

  if (~host->memFlows & MF_CMDCLK_EN){
    //
    // Indicate CMD training is done
    //
    host->nvram.mem.socket[host->var.mem.currentSocket].cmdClkTrainingDone = 1;
    return SUCCESS;
  }

  //
  // Return if this socket is disabled
  //
  socket = host->var.mem.currentSocket;
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  host->var.mem.checkMappedOutRanks = 0;


  // Do not run late command clock if socket is in 3N Timing
  if (host->nvram.mem.socket[socket].channelList[0].timingMode == TIMING_3N) return SUCCESS;

  //
  // Configure NVMCTLR to enable Early CMD training mode
  //
  SetAepTrainingMode (host,socket, LATE_CMD_CLK);

  //
  // Final Composite Margin & CK Alignment
  //
  PerCMDGroup(host, host->var.mem.currentSocket, COMPOSITE_GROUPS);

  //
  // Configure NVMCTLR to disable Early CMD training mode
  //
#ifdef GENERIC_FNV_TRAINING_MODE
  JedecInitSequence (host, socket, CH_BITMASK);
  SetAepTrainingMode (host,socket, DISABLE_TRAINING_STEP);
#else
  SetAepTrainingModeSMB(host, socket, DISABLE_TRAINING_STEP);
#endif

  //
  // Indicate CMD training is done
  //
  host->nvram.mem.socket[host->var.mem.currentSocket].cmdClkTrainingDone = 1;

  //
  // Enable PPDF in next JEDEC INIT
  //
  IO_Reset (host, host->var.mem.currentSocket);
  JedecInitSequence (host, host->var.mem.currentSocket, CH_BITMASK);

  NormalizeCCC(host, host->var.mem.currentSocket);

  return SUCCESS;
} // ChipLateCmdClk

void
ChipEnableCheckParity(
  PSYSHOST host,
  UINT16* mode
  )
{
  *mode |= MODE_CHECK_PARITY;
}

void
ChipSimEnableNmlCMDTrng(
  PSYSHOST host,
  UINT8 socket
  )
{
  if ((host->memFlows & (MF_E_CMDCLK_EN | MF_REC_EN_EN | MF_LRBUF_RD_EN | MF_LRBUF_WR_EN))) {
    SetNormalCMDTiming(host, socket);
  }
}

void ChipEarlyCmdCKSvHook(
  PSYSHOST host,
  UINT8 socket,
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct
  )
{
  //Boiler plate
}

BOOLEAN
ShouldChipSkipElyCMDTrAsPrtyNotSprtd(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  )
{
  if (~host->memFlows & MF_E_CMDCLK_EN) {

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Skipping Early Cmd/Clk Training\n"));

    return TRUE;
  }
  return FALSE;
}

void
TrnOnChipCMDADDOpEn(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct
  )
{
  TCDBP_MCDDC_CTL_STRUCT                    tcdbp;

  tcdbp.Data= MemReadPciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG);
  tcdbp.Bits.cmd_oe_on = 1;
  chipErlyCmdCKStruct->tcdbp.Data = tcdbp.Data;
  MemWritePciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG, chipErlyCmdCKStruct->tcdbp.Data);
}

void
EnableChipFNV(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct
  )
{
  SetAepTrainingMode (host, socket, EARLY_CMD_CLK);
} // EnableChipFNV

void
DisableChipFNV(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct
  )
{
  UINT8                               dimm = 0;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
#ifndef GENERIC_FNV_TRAINING_MODE
  DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_STRUCT  daTrainingEn;
  D_DDRT_TRAINING_EN_FNV_D_UNIT_0_STRUCT    dTrainingEn;
#endif

  channelNvList = GetChannelNvList(host, socket);

#ifdef GENERIC_FNV_TRAINING_MODE
  JedecInitSequence (host, socket, CH_BITMASK);
  SetAepTrainingMode (host,socket, DISABLE_TRAINING_STEP);
#else
  dTrainingEn.Data = 0;
  daTrainingEn.Data = 0;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

      WriteFnvCfgSmb(host, socket, ch, dimm, DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_REG, daTrainingEn.Data);
      WriteFnvCfgSmb(host, socket, ch, dimm, D_DDRT_TRAINING_EN_FNV_D_UNIT_0_REG, dTrainingEn.Data);
    } // dimm loop
  } // ch loop
#endif
}

void
TrnOffChipCMDADDOpEn(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_ERLY_CMD_CK_STRUCT chipErlyCmdCKStruct
  )
{
  TCDBP_MCDDC_CTL_STRUCT  tcdbp;

  tcdbp.Data = MemReadPciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG);
  tcdbp.Bits.cmd_oe_on = 0;
  chipErlyCmdCKStruct->tcdbp.Data = tcdbp.Data;
  MemWritePciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG, chipErlyCmdCKStruct->tcdbp.Data);
}

void
GetCmdMarginsSweepInit(
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct
  )
{
  UINT8 ch;

  chipGetCmdMgnSwpStruct->chEyemask = 0;
  chipGetCmdMgnSwpStruct->chDonemask = 0;

  for (ch = 0; ch < MAX_CH; ch++) {
    chipGetCmdMgnSwpStruct->eyeSize[ch] = 0;
  }
}

void
UpdateCmdMarginMasks(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  INT16 piIndex,
  INT16 offset,
  UINT8 errorResult[MAX_CH][PAR_TEST_RANGE],
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct
  )
{
  UINT8 step;

  if (errorResult[ch][piIndex]) {
    chipGetCmdMgnSwpStruct->eyeSize[ch] = 0;
  } else {
    // errorResult[ch][piIndex] = 0;
    chipGetCmdMgnSwpStruct->eyeSize[ch] += offset;
  }

  if (chipGetCmdMgnSwpStruct->eyeSize[ch] > 64) {
     chipGetCmdMgnSwpStruct->chEyemask &= ~(1 << ch);
  }
  if (((chipGetCmdMgnSwpStruct->chEyemask && (1 << ch)) == 0) && (errorResult[ch][piIndex] == 1)) {
     chipGetCmdMgnSwpStruct->chDonemask &= ~(1 << ch);
  }

  //
  // Fill in results for skipped Pi delays
  //
  for (step = 0; step < offset; step++) {
    if ((piIndex + step) >= PAR_TEST_RANGE) continue;

    errorResult[ch][piIndex + step] = errorResult[ch][piIndex];
  }
} // UpdateCmdMarginMasks

UINT8
CheckChDoneMask(
  PSYSHOST host,
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct
  )
{
  if(chipGetCmdMgnSwpStruct->chDonemask == 0) {
    return 0;
  } else {
    return 1;
  }
} // CheckChDoneMask

BOOLEAN
ShouldChipClearCMDMGNParityError(
  PSYSHOST host,
  UINT32 status
  )
{
  if ((status) || ((host->nvram.mem.dimmTypePresent == UDIMM) || (host->nvram.mem.dimmTypePresent == SODIMM))) {
    return TRUE;
  }
  return FALSE;
}

void
ChipClearDimmErrRslts(
  UINT8 ch,
  UINT8 errorResult[MAX_CH][CHIP_CMD_CLK_TEST_RANGE],
  UINT32* chBitmask,
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct
  )
{
  INT16 piIndex;

  for (piIndex = 0; piIndex < CHIP_CMD_CLK_TEST_RANGE; piIndex++) {
    errorResult[ch][piIndex] = 1;
  } //piIndex loop

  // Indicate rank present and enabled on this ch
  *chBitmask |= 1 << ch;
  chipGetCmdMgnSwpStruct->chEyemask |= 1 << ch;
  chipGetCmdMgnSwpStruct->chDonemask |= 1 << ch;
}

/**
  Collects the results of the previous parity based test

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to check

  @retval status:       Non-zero if any failure was detected
**/
UINT32
ChipCollectParityResults (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT32    chToCheck,
                     UINT8     dimm,
                     UINT32    bwSerr[MAX_CH][3]
                     )
{
  UINT8                       ch;
  UINT8                       err;
  UINT32                      status = 0;
  struct channelNvram         (*channelNvList)[MAX_CH];
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  ALERTSIGNAL_MCDDC_DP_STRUCT alertSignal;
  DDRT_ERROR_MC_2LM_STRUCT    errSignals;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    // No need to check this channel if it's bit isn't set
    if ((chToCheck & (1 << ch))) {

      err = 0;
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        errSignals.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG);
        switch (dimm) {
          case 0:
            err = (UINT8)errSignals.Bits.observed_ddrt_err0_assert;
            break;
          case 1:
            err = (UINT8)errSignals.Bits.observed_ddrt_err1_assert;
            break;
        }
      } else {
        alertSignal.Data = (UINT8)MemReadPciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG);
        err = alertSignal.Bits.seen;
      }

      status |= err;
      if (err) {
        bwSerr[ch][0] = 0xFFFFFFFF;
        bwSerr[ch][1] = 0xFFFFFFFF;
        if (host->nvram.mem.eccEn) {
          bwSerr[ch][2] = 0xFF;
        } else {
          bwSerr[ch][2] = 0;
        }
      } // aepDimmPresent
    } // chToCheck
  } // ch loop

  return  status;
} // CollectParityResults

BOOLEAN
DidChipClearAEPDimmParityErrors(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  PCHIP_CLEAR_PARITY_RESULTS_STRUCT chipClearParityResStruct
  )
{
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  DDRT_ERROR_MC_2LM_STRUCT    errSignals;

  dimmNvList = GetDimmNvList(host, socket, ch);

  if ((*dimmNvList)[dimm].aepDimmPresent) {
    errSignals.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG);
    switch (dimm) {
    case 0:
      errSignals.Bits.observed_ddrt_err0_assert = 0;
      break;
    case 1:
      errSignals.Bits.observed_ddrt_err1_assert = 0;
      break;
    }
    chipClearParityResStruct->errSignals.Data = errSignals.Data;
    MemWritePciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG, chipClearParityResStruct->errSignals.Data);
    return TRUE;
  } //else {

  return FALSE; //Default to test for if no aepDimm is present.
}

/**
  Clears the results of the previous parity based test

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to check

  @retval status:       Non-zero if any failure was detected
**/
void
ChipClearParityResults (
                   PSYSHOST  host,
                   UINT8     socket,
                   UINT32    chToCheck,
                   UINT8     dimm
                   )
{
  UINT8                       ch;
  UINT32                      tCrapOrg[MAX_CH];
  struct channelNvram         (*channelNvList)[MAX_CH];
  ALERTSIGNAL_MCDDC_DP_STRUCT alertSignal;
  CHIP_CLEAR_PARITY_RESULTS_STRUCT chipClearParityResStruct;
  TCRAP_MCDDC_CTL_STRUCT      tCrap;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    tCrapOrg[ch] = 0;
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Set 3N timing and save original setting
    //
    tCrapOrg[ch] = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
    tCrap.Data = tCrapOrg[ch];
    tCrap.Bits.cmd_stretch  = (*channelNvList)[ch].trainTimingMode;
    chipClearParityResStruct.tCrap.Data  = tCrap.Data;
    MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, chipClearParityResStruct.tCrap.Data);
  } // ch loop

  IO_Reset (host, socket);
  JedecInitSequence (host, socket, CH_BITMASK);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Clear parity error status in the memory controller
    //
    if(!DidChipClearAEPDimmParityErrors(host, socket, ch, dimm, &chipClearParityResStruct)){
      alertSignal.Data = (UINT8)MemReadPciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG);
      alertSignal.Bits.seen = 1;
      MemWritePciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG, (UINT32)alertSignal.Data);
    }
    //
    // Restore original CMD timing
    //
    MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tCrapOrg[ch]);
    //  }
  } // ch loop
} // ClearParityResults

BOOLEAN
ShouldChipClearPtyAltWriteMR5(
  PSYSHOST host
  )
{
  if ((host->nvram.mem.dimmTypePresent == UDIMM) || (host->nvram.mem.dimmTypePresent == SODIMM))
    return TRUE;
  else
    return FALSE;
}

void
DoChipSimClearParityResultsAltPath(
  PSYSHOST host,
  UINT8 socket
  )
{
    JedecInitSequence (host, socket, CH_BITMASK);
}

BOOLEAN
DoesChipDetectCSorLRRnkDimm(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch
  )
{
  struct channelNvram         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  if ((*channelNvList)[ch].encodedCSMode == 1)
    return TRUE;
  else
    return FALSE;
}

void
ChipEnableParity(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_PARITY_CHECKING_STRUCT chipEnableParityCheckingStruct
  )
{
}

void
ChipSet3NTimingandSaveOrigSetting(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_PARITY_CHECKING_STRUCT chipEnableParityCheckingStruct
  )
{
  struct channelNvram         (*channelNvList)[MAX_CH];
  TCRAP_MCDDC_CTL_STRUCT                    tCrap;

  channelNvList = GetChannelNvList(host, socket);

  chipEnableParityCheckingStruct->tCrapOrg = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
  tCrap.Data = chipEnableParityCheckingStruct->tCrapOrg;
  tCrap.Bits.cmd_stretch  = (*channelNvList)[ch].trainTimingMode;
  chipEnableParityCheckingStruct->tCrap.Data = tCrap.Data;
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, chipEnableParityCheckingStruct->tCrap.Data);
}

void
ChipRestoreOrigCMDtiming(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_PARITY_CHECKING_STRUCT chipEnableParityCheckingStruct
  )
{
  MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, chipEnableParityCheckingStruct->tCrapOrg);
}


BOOLEAN
DoesChipSupportParityChecking(
  PSYSHOST host
  )
{
  return TRUE;  //Parity checking is supported on all supported DIMM types
}

/**
  Clears the results of the previous parity based test

  @param host:         Pointer to sysHost
  @param socket:       Processor socket to check
  @param rank:         Rank to clear
  @retval status:       Non-zero if any failure was detected
**/
void
ChipClearParityResultsAlt (
                      PSYSHOST  host,
                      UINT8     socket,
                      UINT32    chToCheck,
                      UINT8     rank,
                      UINT8     dimm
                      )
{
  UINT8                       ch;
  UINT32                      tCrapOrg[MAX_CH];
  struct channelNvram         (*channelNvList)[MAX_CH];
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  struct rankDevice           (*rankStruct)[MAX_RANK_DIMM];
  ALERTSIGNAL_MCDDC_DP_STRUCT alertSignal;
  DDRT_ERROR_MC_2LM_STRUCT     errSignals;
  TCRAP_MCDDC_CTL_STRUCT      tCrap;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    tCrapOrg[ch] = 0;
    if ((*channelNvList)[ch].enabled == 0) continue;

    // No need to check this channel if it's bit isn't set
    if ((chToCheck & (1 << ch))) {
      //
      // Set 3N timing and save original setting
      //
      tCrap.Data = tCrapOrg[ch] = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);
      tCrap.Bits.cmd_stretch  = (*channelNvList)[ch].trainTimingMode;
      MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tCrap.Data);

      //
      // Skip if no DIMM present
      //
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankStruct  = GetRankStruct(host, socket, ch, dimm);

      //
      // Skip if no rank
      //
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      if (host->nvram.mem.dimmTypePresent == UDIMM) {
        WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
      } else {
        if (rank == 0) { //RDIMM is only RANK0
          WriteRC (host, socket, ch, dimm, rank, 6, RDIMM_RC06);
          WriteRC (host, socket, ch, dimm, rank, BIT0, RDIMM_RC0E);
        }
      }

      FixedDelay(host, 1);
    }
  } // ch loop

  IO_Reset (host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if (chToCheck & (1 << ch)) {

      //
      // Clear parity error status in the memory controller
      //
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        errSignals.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG);
        switch (dimm) {
          case 0:
            errSignals.Bits.observed_ddrt_err0_assert = 0;
            break;
          case 1:
            errSignals.Bits.observed_ddrt_err1_assert = 0;
            break;
        }
        MemWritePciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG, errSignals.Data);

      } else {
        alertSignal.Data = (UINT8)MemReadPciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG);
        alertSignal.Bits.seen = 1;
        MemWritePciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG, (UINT32)alertSignal.Data);
      }
      //
      // Restore original CMD timing
      //
      MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, tCrapOrg[ch]);
    }
  } // ch loop
} // ClearParityResultsAlt

void
ChipSpecificParityCheckDisable (
    PSYSHOST    host,                                             // Pointer to the system host (root) structure
    UINT8       socket                                            // Socket Id
  )
{
}

void
ChipSpecificParityCheckDisablePerCh (
  PSYSHOST                     host,
  UINT8                        socket,
  UINT8                        ch,
  PCHIP_PARITY_CHECKING_STRUCT chipDisableParityCheckingStruct
  )
{
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT         mcSchedChknBit;

  //4928166 MRC: MC Register settings to ensure Parity engines in MC are not enabled
  mcSchedChknBit.Data = MemReadPciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
  mcSchedChknBit.Bits.dis_ddrt_par_gen = 0;
  chipDisableParityCheckingStruct->mcSchedChknBit.Data = mcSchedChknBit.Data;
  MemWritePciCfgEp(host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, chipDisableParityCheckingStruct->mcSchedChknBit.Data);
}

void
ChipSpecificErrorEnable(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_PARITY_CHECKING_STRUCT chipDisableParityCheckingStruct
  )
{
  DDR4_CA_CTL_MCDDC_DP_STRUCT               ddr4cactl;
  CMD_PAR_INJ_CTL_MCDDC_CTL_STRUCT          cmdParInjCtl;

  //4928166 MRC: MC Register settings to ensure Parity engines in MC are not enabled
  ddr4cactl.Data = MemReadPciCfgEp(host, socket, ch, DDR4_CA_CTL_MCDDC_DP_REG);
  ddr4cactl.Bits.erf_en0 = 0;
  chipDisableParityCheckingStruct->ddr4cactl.Data = ddr4cactl.Data;
  MemWritePciCfgEp(host, socket, ch, DDR4_CA_CTL_MCDDC_DP_REG, chipDisableParityCheckingStruct->ddr4cactl.Data);

  cmdParInjCtl.Data = MemReadPciCfgEp(host, socket, ch, CMD_PAR_INJ_CTL_MCDDC_CTL_REG);
  cmdParInjCtl.Bits.en_ddr4 = 0;
  cmdParInjCtl.Bits.en_ddrt_ui0 = 0;
  cmdParInjCtl.Bits.en_ddrt_ui1 = 0;
  chipDisableParityCheckingStruct->cmdParInjCtl.Data = cmdParInjCtl.Data;
  MemWritePciCfgEp(host, socket, ch, CMD_PAR_INJ_CTL_MCDDC_CTL_REG, chipDisableParityCheckingStruct->cmdParInjCtl.Data);
}

void
PerformChipSpecificNormalCMDTimingSetup(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_SET_NORMAL_CMD_TIMING_STRUCT chipSetNormalCMDTimingStruct
  )
{
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT ddrtMiscDelay;
  TCRAP_MCDDC_CTL_STRUCT                    tCrap;

  ddrtMiscDelay.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
  tCrap.Data = chipSetNormalCMDTimingStruct->tCrap.Data;
  if (tCrap.Bits.cmd_stretch == TIMING_2N) {
    ddrtMiscDelay.Bits.enable_ddrt_2n_timing = 1;
  } else {
    ddrtMiscDelay.Bits.enable_ddrt_2n_timing = 0;
  }
  chipSetNormalCMDTimingStruct->ddrtMiscDelay.Data = ddrtMiscDelay.Data;
  MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, chipSetNormalCMDTimingStruct->ddrtMiscDelay.Data);
}

BOOLEAN
ShouldChipSetNormalCMDTiming(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  PCHIP_SET_NORMAL_CMD_TIMING_STRUCT chipSetNormalCMDTimingStruct
  )
{
  struct channelNvram         (*channelNvList)[MAX_CH];
  TCRAP_MCDDC_CTL_STRUCT                    tCrap;

  channelNvList = GetChannelNvList(host, socket);

  tCrap.Data = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);

  if (tCrap.Bits.cmd_stretch != (*channelNvList)[ch].timingMode) {
    tCrap.Bits.cmd_stretch  = (*channelNvList)[ch].timingMode;
    chipSetNormalCMDTimingStruct->tCrap.Data = tCrap.Data;
    MemWritePciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG, chipSetNormalCMDTimingStruct->tCrap.Data);

    PerformChipSpecificNormalCMDTimingSetup(host, socket, ch, chipSetNormalCMDTimingStruct);
    if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  return FALSE;
}

//Functions that are specific to SKX MemCmdClk.c along with associated types and defines
GSM_CSN   SKXCtlToSweep[NUM_SIGNALS_TO_SWEEP_LB] = {
  CS0_N, CS1_N, CS2_N, CS3_N, CS4_N, CS5_N, CS6_N, CS7_N,
  CKE0, CKE1, CKE2, CKE3,
  ODT0, ODT1, ODT2, ODT3,
#ifdef CA_LOOPBACK
  C2, A17, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, BA0, BA1, BG0, BG1, PAR, ACT_N
#endif
};


struct skxCmdEye {
   INT16 left;
   INT16 right;
};

#define SKX_CTL_CLK_TEST_RANGE  256

extern UINT8 CSFeedbackCh0Rank0 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CSFeedbackCh0Rank1 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CSFeedbackCh0Rank2 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CSFeedbackCh0Rank4 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CSFeedbackCh0Rank5 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CSFeedbackCh0Rank6 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CKFeedbackCh0Rank0 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CKFeedbackCh0Rank1 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CKFeedbackCh0Rank4 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CKFeedbackCh0Rank5 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 ODTFeedbackCh0Rank0 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 ODTFeedbackCh0Rank1 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 ODTFeedbackCh0Rank4 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 ODTFeedbackCh0Rank5 [SKX_CTL_CLK_TEST_RANGE];
extern UINT8 CAFeedbackCh0 [SKX_CTL_CLK_TEST_RANGE];

BOOLEAN
ShouldChipPerformEarlyCtlClk(
  PSYSHOST host
  )
{
  // Execute Loopback for RDIMM, LRDIMM, NVMDIMM configs
  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    EarlyCtlClkLoopback(host);
    host->var.mem.checkMappedOutRanks = 0;
    return FALSE;
  }
  return TRUE;
}


VOID
ChipCtlClkFeedBack (
  PSYSHOST  host,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     sign,
  INT16     *ctlMargins
)
{
}

void
PerformChipLRDIMMEarlyCtlClk(
  PSYSHOST host,
  UINT8 socket
  )
{
  UINT8 ch;
  UINT8 dimm;
#ifdef LRDIMM_SUPPORT
  UINT8               controlWordData;
#endif
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);

#ifdef LRDIMM_SUPPORT

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if (IsLrdimmPresent(host, socket, ch, dimm) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
          //F0BC1x = 00010010b
          // enable MPR override mode in the buffer
          //
          (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x | LRDIMM_MPR_OVERRIDE_MODE;
          controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);
      } //lrdimm
    } //dimm
  } //ch
#endif
}

void
DisableChipLRDIMMMPROverrideMode(
  PSYSHOST host,
  UINT8 socket
  )
{
  UINT8 ch;
  UINT8 dimm;
#ifdef LRDIMM_SUPPORT
  UINT8               controlWordData;
#endif

  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);

#ifdef LRDIMM_SUPPORT

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if (IsLrdimmPresent(host, socket, ch, dimm) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
          //
          // disable MPR override mode in the buffer
          //
          (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x & ~LRDIMM_MPR_OVERRIDE_MODE;
          controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);
      } //lrdimm
    } //dimm
  } //ch
#endif
}

void
EnableChipLRDIMMPreambleTrMode(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  )
{
#ifdef LRDIMM_SUPPORT
  UINT8                                 controlWordData;
#endif
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;

#ifdef  LRDIMM_SUPPORT
  if (IsLrdimmPresent(host, socket, ch, dimm)) {
    //F0BC1x = 00010000b
    // enable read preamble training mode in the buffer
    (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x | LRDIMM_RD_PREAMBLE_TRAINING_MODE;
    controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
    WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);
  }
#endif
}

void
DisableChipLRDIMMPreambleTrMode(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  )
{
#ifdef LRDIMM_SUPPORT
  UINT8  controlWordData;
#endif
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
#ifdef  LRDIMM_SUPPORT
  if (IsLrdimmPresent(host, socket, ch, dimm) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
    //F0BC1x = 00010000b
    // disable read preamble training mode in the buffer
    (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x & ~LRDIMM_RD_PREAMBLE_TRAINING_MODE;
    controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
    WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);

    // IDT WA for GP B0
    if (((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) && ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_DB_RID_GPB0)) {
      WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
    }
    // END IDT WA for GP B0
  }
#endif
}

void
StepChipCtlCKIOLatency(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank,
  UINT8 logRank
  )
{
  UINT8                                 ioLatency;
  //
  // Add 1 to IO latency
  //
  ioLatency = GetIOLatency (host, socket, ch, logRank) + 1;
  SetIOLatency (host, socket, ch, logRank, ioLatency);
}

void
RestoreChipCtlCKIOLatency(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT8 rank,
  UINT8 logRank
  )
{
  UINT8                                 ioLatency;
  //
  // Restore IO latency
  //
  ioLatency = GetIOLatency (host, socket, ch, logRank) - 1;
  SetIOLatency (host, socket, ch, logRank, ioLatency);
}


UINT8
CheckParityWorkaroundChipSpecific(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm,
  UINT16 dateCode
  )
{
  //Boiler plate
  return 0;
}

void
SetChipGetCtlClkResLowerNibbleStrobeFeedback(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  UINT8 strobe,
  UINT32* csrReg,
  UINT32* strobeFeedback
  )
{
  //Let compiler optimize if possible.
  UINT32 _strobeFeedback = *strobeFeedback;
  //
  // Common accross HSX, SKX and KNL
  //
  *csrReg = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG));
  //
  // Lower nibble
  //
  if ((*csrReg & 0x1FF) > 0) {
    _strobeFeedback |= 1 << strobe;
    *strobeFeedback = _strobeFeedback;
  }
}

void
SetChipGetCtlClkResUpperNibbleStrobeFeedback(
UINT8 strobe,
UINT32 csrReg,
  UINT32* strobeFeedback
  )
{
  //Boiler plate
}

UINT32
ChipAggressiveCMDTest(
  PSYSHOST  host,
  UINT8     socket
  )
{
  //Boiler plate
  return 0;
}

/**

  Handles any register interaction on GetMargins entry/exit.

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param entry   - TRUE when entering GetMargins, FALSE when exiting.

**/
UINT32
GetMarginsHook (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  BOOLEAN   entry
)
{
  TCDBP_MCDDC_CTL_STRUCT  tcdbp;
  tcdbp.Data = MemReadPciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG);
  tcdbp.Bits.cmd_oe_on = entry;
  MemWritePciCfgEp (host, socket, ch, TCDBP_MCDDC_CTL_REG, tcdbp.Data);
  return SUCCESS;
}

//
//InitMem.c
//
void
SetRankTerminationMask(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch
  )
{
  UINT8   dimm;
  UINT8   i;
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  MC_TERM_RNK_MSK_MCDDC_CTL_STRUCT rankMask;

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);

  //
  // Look at each DIMM
  //
  rankMask.Data = 0;
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    rankList = GetRankNvList(host, socket, ch, dimm);
    for (i = 0; i < (*dimmNvList)[dimm].numRanks; i++) {
      if ((*rankList)[i].enabled == 0) continue;

      if ((i < 2) && IsTermRank(host, socket, ch, dimm, i)) {
        rankMask.Data |= 1 << ((dimm * MAX_RANK_DIMM) + i);
      }
    } // rank loop

    //
    // Make sure at least one rank is set per DIMM
    //
    if ((rankMask.Data & (0xF << (dimm * MAX_RANK_DIMM))) == 0) {
      rankMask.Data |= 1 << (dimm * MAX_RANK_DIMM);
    }
  } // dimm loop

  //
  // Set a termination rank for each DIMM
  //
  if (rankMask.Data == 0) rankMask.Data = 1;

  //
  // Channel PPDS idle counter
  //
  rankMask.Bits.ch_ppds_idle_timer  = PPDS_IDLE_TIMER;

  //
  // Update the CSR
  //
  MemWritePciCfgEp (host, socket, ch, MC_TERM_RNK_MSK_MCDDC_CTL_REG, rankMask.Data);

  //MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
  //              "TermRnkMsk = 0x%x\n", rankMask.Data));
}

/**

  Checks if DQ swizzling is supported

  @param host    - Pointer to sysHost

**/
UINT8
CheckDqSwizzleSupport (
  PSYSHOST  host,
  UINT8     socket
)
{
  UINT8                         aepdimms = 0;
  UINT8                         ch;
  struct channelNvram           (*channelNvList)[MAX_CH];

  // check for NVMDIMM DIMMs present SKX TO: replace this with NVMDIMM per socket variable
  channelNvList = &host->nvram.mem.socket[socket].channelList;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled == 1) {
      aepdimms = 1;
    }
  }

  // run DqSwizzleDiscovery if WR_CRC enabled
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0) || (~host->memFlows & MF_DQ_SWIZ_EN) ||
      (host->nvram.mem.eccEn == 0) ||
      ((!(host->setup.mem.optionsExt & WR_CRC)) && (aepdimms == 0)) || (host->nvram.mem.dramType != SPD_TYPE_DDR4) || (host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP)) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Skipping Swizzle Discovery.\n"));
    return 0;
  }
  return 1;
}

/**

  Checks DQ swizzling and programs registers to account for it

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param entry   - TRUE when entering GetMargins, FALSE when exiting.

**/
void
CheckAndProgramDqSwizzle (
  PSYSHOST  host,
  UINT8     socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT32                            crcDqSwz[5];
  UINT32                            crcDqSwzeven[5];
  UINT8                             i;
  UINT8                             bit0;
  UINT8                             bit1;
  UINT8                             temp;
  UINT32                            enable_nibble_swizzle;
  DDR4_WRCRC0_CTL_MCDDC_DP_STRUCT   wrcrc0;
  DDR4_WRCRC1_CTL_MCDDC_DP_STRUCT   wrcrc1;
  DDR4_WRCRC2_CTL_MCDDC_DP_STRUCT   wrcrc2;
  DDR4_WRCRC3_CTL_MCDDC_DP_STRUCT   wrcrc3;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent) continue;
        // Program results

      MemWritePciCfgEp (host, socket, ch, DDR4_WRCRC_RANK_CTL_MCDDC_DP_REG, 0xee44);

      for (i = 0; i < 4; i++) {
        crcDqSwz[i] = 0;
        crcDqSwzeven[i] = 0;
      }
      for (i = 0; i < 64; i++) {
        crcDqSwz[i/16] |= (((*dimmNvList)[dimm].dqSwz[i/2] >> ((i % 2)*4)) & 3) << ((i % 16) * 2);
      }
      //special mapping for bits 63-72, plus enable bits
      //set the enable bits for this swizzle map
      enable_nibble_swizzle = 0;
      for (i = 0; i < 72; i=i+8) {
        bit0 = ((*dimmNvList)[dimm].dqSwz[i / 2] >> ((i % 2) * 4)) & 7;
        if(bit0 > 3) {
          enable_nibble_swizzle |= (1 << (i/8));
        }
      }
      if (dimm == 0) {
        wrcrc0.Data =  MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC0_CTL_MCDDC_DP_REG);
        wrcrc1.Data = MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC1_CTL_MCDDC_DP_REG);
        wrcrc0.Bits.enable_nibble_swizzle = enable_nibble_swizzle;
        wrcrc1.Bits.enable_nibble_swizzle = enable_nibble_swizzle;
        MemWritePciCfgEp(host, socket, ch, DDR4_WRCRC0_CTL_MCDDC_DP_REG, wrcrc0.Data);
        MemWritePciCfgEp(host, socket, ch, DDR4_WRCRC1_CTL_MCDDC_DP_REG, wrcrc1.Data);
      } else {
        wrcrc2.Data = MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC2_CTL_MCDDC_DP_REG);
        wrcrc3.Data = MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC3_CTL_MCDDC_DP_REG);
        wrcrc2.Bits.enable_nibble_swizzle = enable_nibble_swizzle;
        wrcrc3.Bits.enable_nibble_swizzle = enable_nibble_swizzle;
        MemWritePciCfgEp(host, socket, ch, DDR4_WRCRC2_CTL_MCDDC_DP_REG, wrcrc2.Data);
        MemWritePciCfgEp(host, socket, ch, DDR4_WRCRC3_CTL_MCDDC_DP_REG, wrcrc3.Data);
      }
      if (dimm == 0) {
        crcDqSwz[4] = (UINT16)MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC0_CTL_MCDDC_DP_REG);
        crcDqSwzeven[4] = (UINT16)MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC1_CTL_MCDDC_DP_REG);
      } else {
          crcDqSwz[4] = (UINT16)MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC2_CTL_MCDDC_DP_REG);
          crcDqSwzeven[4] = (UINT16)MemReadPciCfgEp(host, socket, ch, DDR4_WRCRC3_CTL_MCDDC_DP_REG);
      }
      for (i = 64; i < 72; i++) {
        crcDqSwz[i/16] |= (((*dimmNvList)[dimm].dqSwz[i/2] >> ((i % 2)*4)) & 3) << (((i+8) % 16) * 2);
      }

        for (i = 0; i < 64; i=i+2) {
          bit0 = ((crcDqSwz[i / 16] >> ((i % 16) * 2)) & 3);
          bit1 = ((crcDqSwz[(i + 1) / 16] >> (((i + 1) % 16) * 2)) & 3);
          temp = bit1;
          bit1 = bit0;
          bit0 = temp;
          crcDqSwzeven[i / 16] |= bit0 << ((i % 16) * 2);
          crcDqSwzeven[(i +1) / 16] |= bit1 << (((i + 1) % 16) * 2);
      }

        for (i = 64; i < 72; i=i+2) {
          bit0 = ((crcDqSwz[i / 16] >> (((i + 8) % 16) * 2)) & 3);
          bit1 = ((crcDqSwz[(i + 1) / 16] >> (((i + 9) % 16) * 2)) & 3);
          temp = bit1;
          bit1 = bit0;
          bit0 = temp;
          crcDqSwzeven[i / 16] |= bit0 << (((i + 8) % 16) * 2);
          crcDqSwzeven[i / 16] |= bit1 << (((i + 9) % 16) * 2);
      }

      for (i = 0; i < 5; i++) {
        if (dimm == 0) {
          MemWritePciCfgEp (host, socket, ch, (DDR4_WRCRC0_DQ15_DQ0_MCDDC_DP_REG + (i * 4)), crcDqSwz[i]);
          MemWritePciCfgEp (host, socket, ch, (DDR4_WRCRC1_DQ15_DQ0_MCDDC_DP_REG + (i * 4)), crcDqSwzeven[i]);
        } else {
          if (i == 0) {  // w/a for CSR gap between DDR4_WRCRC2_DQ47_DQ32_MCDDC_DP_REG and DDR4_WRCRC2_DQ63_DQ48_MCDDC_DP_REG
            MemWritePciCfgEp (host, socket, ch, DDR4_WRCRC2_DQ15_DQ0_MCDDC_DP_REG, crcDqSwz[0]);
            MemWritePciCfgEp (host, socket, ch, DDR4_WRCRC2_DQ31_DQ16_MCDDC_DP_REG, crcDqSwz[1]);
            MemWritePciCfgEp (host, socket, ch, DDR4_WRCRC2_DQ47_DQ32_MCDDC_DP_REG, crcDqSwz[2]);
            MemWritePciCfgEp (host, socket, ch, DDR4_WRCRC2_DQ63_DQ48_MCDDC_DP_REG, crcDqSwz[3]);
            MemWritePciCfgEp (host, socket, ch, DDR4_WRCRC2_CTL_MCDDC_DP_REG, crcDqSwz[4]);
          }
          MemWritePciCfgEp (host, socket, ch, (DDR4_WRCRC3_DQ15_DQ0_MCDDC_DP_REG + (i * 4)), crcDqSwzeven[i]);
        } // dimm == 1
      } // i loop
    } // dimm loop
  } // ch loop
} // CheckAndProgramDqSwizzle

/**

  Checks DQ swizzling and programs registers to account for it

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param entry   - TRUE when entering GetMargins, FALSE when exiting.

**/
void
CheckAndProgramDqSwizzleAep (
  PSYSHOST  host,
  UINT8     socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT32                            rxDqSwz[5];
  UINT32                            txDqSwz[5];
  UINT8                             i;
  UINT8                             j;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        // Program results

        for (i = 0; i < 5; i++) {
          txDqSwz[i] = 0;
          rxDqSwz[i] = 0;
        }

        for (i = 0; i < 72; i++) {
          txDqSwz[i/16] |= (((*dimmNvList)[dimm].dqSwz[i/2] >> ((i % 2)*4)) & 3) << ((i % 16) * 2);
        }

        for (j = 0; j < 5; j++) {
          for (i = 0; i < 16; i++) {
            rxDqSwz[j] |= (i % 4) << ((((txDqSwz[j] >> (i * 2)) & (BIT0 | BIT1)) * 2) + ((i/4) * 8));
          } // i loop
        } // j loop
        txDqSwz[4] &= 0x0000FFFF;
        rxDqSwz[4] &= 0x0000FFFF;

        // Program Swizzle NVMCTLR Registers
      if (host->memFlows & MF_DQ_SWIZ_EN) {
        for (i = 0; i < 5; i++) {
          WriteFnvCfg(host, socket, ch, dimm, (DA_DDRT_TX_DQ_SWZ0_FNV_DA_UNIT_0_REG + (i*4)), txDqSwz[i]);
          WriteFnvCfg(host, socket, ch, dimm, (DA_DDRT_RX_DQ_SWZ0_FNV_DA_UNIT_0_REG + (i*4)), rxDqSwz[i]);
        }
      }
    } // dimm loop
  } // ch loop
} // CheckAndProgramDqSwizzleAep

//---------------------------------------------------------------------------------------------------

void
WriteRCFnv(
           PSYSHOST host,
           UINT8    socket,
           UINT8    ch,
           UINT8    dimm,
           UINT8    controlWordData,
           UINT8    controlWordAddr
)
{
  struct channelNvram(*channelNvList)[MAX_CH];
  BUS_TYPE temp;

  channelNvList = GetChannelNvList(host, socket);
  temp = (*channelNvList)[ch].fnvAccessMode;
  (*channelNvList)[ch].fnvAccessMode = EMRS;

  switch (controlWordAddr) {
    case RDIMM_RC0E:
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC0E_FNV_DA_UNIT_0_REG, controlWordData);
      break;
    default:
      break;
  }

  (*channelNvList)[ch].fnvAccessMode = temp;
}


/**

  Routine Description: Enables/Disables Training Mode for NVMCTLR

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param mode    - training mode

  @retval SUCCESS
**/
UINT32
SetAepTrainingMode (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    mode
                 )
{
  UINT8                                       ch;
  UINT8                                       dimm;
  BOOLEAN                                     ddrtEnabled = FALSE;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
  DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_STRUCT    daTrainingEn;
  D_DDRT_TRAINING_EN_FNV_D_UNIT_0_STRUCT      dTrainingEn;
  D_FUNC_DEFEATURE2_FNV_D_UNIT_0_STRUCT       dFuncDefeature2;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled) {
      ddrtEnabled = TRUE;
      break;
    }
  }
  if (ddrtEnabled == FALSE) {
    return SUCCESS;
  }
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "NVMDIMM training mode = %d\n", mode));

  daTrainingEn.Data = 0;
  dTrainingEn.Data = 0;

  daTrainingEn.Bits.tm_en = 1;
  dTrainingEn.Bits.tm_en = 1;
  daTrainingEn.Bits.disable_rid_feedback = 1;
  dTrainingEn.Bits.disable_rid_feedback = 1;

  switch (mode){
    case DISABLE_TRAINING_MODE:
      // Exit Training Mode
      daTrainingEn.Data = 0;
      dTrainingEn.Data = 0;
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " Exit Training Mode\n"));
      break;
    case EARLY_CMD_CLK:
      daTrainingEn.Bits.early_cmd_ck_trn  = 1;
      break;
    case EARLY_RID_FINE:
      daTrainingEn.Bits.basic_erid_trn = 1;
      daTrainingEn.Bits.disable_rid_feedback = 0;
      dTrainingEn.Bits.disable_rid_feedback = 0;
      break;
    case EARLY_RID_COARSE:
      daTrainingEn.Bits.disable_rid_feedback = 0;
      dTrainingEn.Bits.disable_rid_feedback = 0;
      break;
    case CMD_VREF_CENTERING:
      daTrainingEn.Bits.adv_cmd_ck_trn  = 1;
      dTrainingEn.Bits.adv_cmd_ck_trn  = 1;
      break;
    case LATE_CMD_CLK:
      daTrainingEn.Bits.adv_cmd_ck_trn  = 1;
      dTrainingEn.Bits.adv_cmd_ck_trn  = 1;
      break;
#ifdef LRDIMM_SUPPORT
    case RX_BACKSIDE_PHASE_TRAINING:
      daTrainingEn.Bits.recen_dq_dqs_mpr_trn = 1;
      break;
#endif
    case DISABLE_TRAINING_STEP:
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " Training Step Disabled\n"));
      break;
    case ENABLE_TRAINING_MODE:
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Training Mode Enabled\n"));
      break;
    default:
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Training Step Not Supported\n"));
      break;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      // 4928304  MRC: d_func_defeature2 needs to be programmed for err# assertion in adv cmd-ck training
      if ((mode == CMD_VREF_CENTERING) || (mode == LATE_CMD_CLK) || (mode == DISABLE_TRAINING_STEP)) {
        dFuncDefeature2.Data = ReadFnvCfg(host, socket, ch, dimm, D_FUNC_DEFEATURE2_FNV_D_UNIT_0_REG);
        if ((mode == CMD_VREF_CENTERING) || (mode == LATE_CMD_CLK))
          dFuncDefeature2.Bits.err_pche_miss = 1;
        else
          dFuncDefeature2.Bits.err_pche_miss = 0;
        WriteFnvCfg(host, socket, ch, dimm, D_FUNC_DEFEATURE2_FNV_D_UNIT_0_REG, dFuncDefeature2.Data);
      }
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_REG, daTrainingEn.Data);
      WriteFnvCfg(host, socket, ch, dimm, D_DDRT_TRAINING_EN_FNV_D_UNIT_0_REG, dTrainingEn.Data);
    }
  }

  return SUCCESS;
}


/**

  Routine Description: Clears Prefetch Cache miss for NVMCTLR

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval SUCCESS
**/
UINT32
ClrPcheMiss (
                 PSYSHOST host,
                 UINT8    socket
                 )
{
#ifdef DEBUG_LATECMDCLK

  UINT8                                       ch;
  UINT8                                       dimm;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
  D_DDRT_FUNC_STS_FNV_D_UNIT_0_STRUCT         dDdrtFuncSts;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      dDdrtFuncSts.Data = ReadFnvCfg(host, socket, ch, dimm, D_DDRT_FUNC_STS_FNV_D_UNIT_0_REG);
      if(dDdrtFuncSts.Bits.pche_miss) {
        dDdrtFuncSts.Bits.pche_miss = 0;
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT," Missed lookup to Prefetch Cache during training\n"));
      }
      WriteFnvCfg(host, socket, ch, dimm, D_DDRT_FUNC_STS_FNV_D_UNIT_0_REG, dDdrtFuncSts.Data);
    }
  }
#endif

  return SUCCESS;
}


/**

  Routine Description: Enables/Disables Training Mode for NVMCTLR, SMBUS version

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param mode    - training mode

  @retval SUCCESS
**/
UINT32
SetAepTrainingModeSMB (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    mode
                 )
{
  UINT8                                       ch;
  UINT8                                       dimm;
  BOOLEAN                                     ddrtEnabled = FALSE;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
  DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_STRUCT    daTrainingEn;
  D_DDRT_TRAINING_EN_FNV_D_UNIT_0_STRUCT      dTrainingEn;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled) {
      ddrtEnabled = TRUE;
      break;
    }
  }
  if (ddrtEnabled == FALSE) {
    return SUCCESS;
  }

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "NVMDIMM training mode = %d\n", mode));

  daTrainingEn.Data = 0;
  dTrainingEn.Data = 0;

  daTrainingEn.Bits.tm_en = 1;
  dTrainingEn.Bits.tm_en = 1;
  daTrainingEn.Bits.disable_rid_feedback = 1;
  dTrainingEn.Bits.disable_rid_feedback = 1;

  switch (mode){
    case DISABLE_TRAINING_MODE:
      // Exit Training Mode
      daTrainingEn.Data = 0;
      dTrainingEn.Data = 0;
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " Exit Training Mode\n"));
      break;
    case EARLY_CMD_CLK:
      daTrainingEn.Bits.early_cmd_ck_trn  = 1;
      break;
    case EARLY_RID_FINE:
      daTrainingEn.Bits.basic_erid_trn = 1;
      daTrainingEn.Bits.disable_rid_feedback = 0;
      dTrainingEn.Bits.disable_rid_feedback = 0;
      break;
    case EARLY_RID_COARSE:
      daTrainingEn.Bits.disable_rid_feedback = 0;
      dTrainingEn.Bits.disable_rid_feedback = 0;
      break;
    case CMD_VREF_CENTERING:
    case LATE_CMD_CLK:
      daTrainingEn.Bits.adv_cmd_ck_trn  = 1;
      dTrainingEn.Bits.adv_cmd_ck_trn  = 1;
      break;
#ifdef LRDIMM_SUPPORT
    case RX_BACKSIDE_PHASE_TRAINING:
      daTrainingEn.Bits.recen_dq_dqs_mpr_trn = 1;
      break;
#endif
    case DISABLE_TRAINING_STEP:
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " Training Step Disabled\n"));
      break;
    case ENABLE_TRAINING_MODE:
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Training Mode Enabled\n"));
      break;
    default:
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Training Step Not Supported\n"));
      break;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        WriteFnvCfgSmb(host, socket, ch, dimm, DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_REG, daTrainingEn.Data);
        WriteFnvCfgSmb(host, socket, ch, dimm, D_DDRT_TRAINING_EN_FNV_D_UNIT_0_REG, dTrainingEn.Data);
      }
  }

  return SUCCESS;
}

/**

  This resets the NVMCTLR DDRTIO FIFO Pointers

  @param host    - Pointer to sysHost
  @param socket    - Socket number

  @retval N/A

**/
void
IODdrtReset (
             PSYSHOST host,
             UINT8    socket,
             UINT8    dimm
             )
{
  UINT8                                          ch;
  struct channelNvram                            (*channelNvList)[MAX_CH];
  struct dimmNvram                               (*dimmNvList)[MAX_DIMM];
  MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_STRUCT fnvioCtl;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

    fnvioCtl.Data = (*dimmNvList)[dimm].fnvioControl;
    fnvioCtl.Bits.mc2ddrttrainreset = 1;
    WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG, fnvioCtl.Data);
    fnvioCtl.Bits.mc2ddrttrainreset = 0;
    WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG, fnvioCtl.Data);
    (*dimmNvList)[dimm].fnvioControl = fnvioCtl.Data;
  } // ch
} // IODdrtReset

void
IODdrtResetAll(
  PSYSHOST host,
  UINT8 socket
  )
{
  UINT8 dimm;

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    IODdrtReset(host, socket, dimm);
  } // dimm
} // IODdrtResetAll


/**

This resets the FNV DDRTIO FIFO Pointers

@param host    - Pointer to sysHost
@param socket    - Socket number

@retval N/A

**/
void
IODdrtResetPerCh(
PSYSHOST host,
UINT8    socket,
UINT8    ch,
UINT8    dimm
)
{
  struct channelNvram(*channelNvList)[MAX_CH];
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_STRUCT fnvioCtl;

  channelNvList = GetChannelNvList(host, socket);
  if ((*channelNvList)[ch].enabled == 0) return;
  dimmNvList = GetDimmNvList(host, socket, ch);
  if ((*dimmNvList)[dimm].dimmPresent == 0) return;
  if ((*dimmNvList)[dimm].aepDimmPresent == 0) return;

  fnvioCtl.Data = (*dimmNvList)[dimm].fnvioControl;
  fnvioCtl.Bits.mc2ddrttrainreset = 1;
  WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG, fnvioCtl.Data);
  fnvioCtl.Bits.mc2ddrttrainreset = 0;
  WriteFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG, fnvioCtl.Data);
  (*dimmNvList)[dimm].fnvioControl = fnvioCtl.Data;
} // IODdrtReset

/**

  Check if CTL loopback mode should be used

  @param host    - Pointer to sysHost

  @retval N/A

**/
BOOLEAN
CheckCTLLoopback (
             PSYSHOST host
             )
{
  if (host->nvram.mem.dimmTypePresent == RDIMM) {
    return TRUE;
  } else {
    return FALSE;
  }
} // CheckCTLLoopback

/**

  This gets the parity error status during CTL loopback training

  @param host    - Pointer to sysHost
  @param socket    - Socket number

  @retval N/A

**/
UINT8
ChipGetParityErrorStatus (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    dimm
             )
{
  UINT8                       err = 0;
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  ALERTSIGNAL_MCDDC_DP_STRUCT alertSignal;
  DDRT_ERROR_MC_2LM_STRUCT    errSignals;

  dimmNvList = GetDimmNvList(host, socket, ch);
  if ((*dimmNvList)[dimm].aepDimmPresent) {
    //
    // Clear parity error status in the memory controller
    // 4928893  MRC: For Early CMD CLK, ERR# bits need to be cleared before read
    //
    errSignals.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG);
    switch (dimm) {
      case 0:
        errSignals.Bits.observed_ddrt_err0_assert = 0;
        break;
      case 1:
        errSignals.Bits.observed_ddrt_err1_assert = 0;
        break;
    }
    MemWritePciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG, errSignals.Data);

    //
    // Read Alert Feedback Sticky Bit
    //
    errSignals.Data = MemReadPciCfgEp (host, socket, ch, DDRT_ERROR_MC_2LM_REG);
    switch (dimm) {
      case 0:
        err = (UINT8)errSignals.Bits.observed_ddrt_err0_assert;
        break;
      case 1:
        err = (UINT8)errSignals.Bits.observed_ddrt_err1_assert;
        break;
    }

  } else {
    //
    // Clear parity error status in the memory controller
    //
    alertSignal.Data = (UINT8)MemReadPciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG);
    alertSignal.Bits.seen = 1;
    MemWritePciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG, (UINT32)alertSignal.Data);

    //
    // Read Alert Feedback Sticky Bit
    //
    alertSignal.Data = (UINT8)MemReadPciCfgEp (host, socket, ch, ALERTSIGNAL_MCDDC_DP_REG);
    err = alertSignal.Bits.seen;
  }

  return err;
} // ChipGetParityErrorStatus

/**

  This sets the CTL loopback mode in the buffer

  @param host    - Pointer to sysHost
  @param socket    - Socket number

  @retval N/A

**/
void
ChipSetCTLLoopback (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    dimm
             )
{
  UINT32              lrbufData;
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  if ((*dimmNvList)[dimm].aepDimmPresent) {

    /* training_mode - Bits[2:0], RW, default = 3'h0
      Training mode selection[br]
               000 = Normal operating [br]
               001 = CK-CA training[br]
               010 = CS0_n loopback[br]
               011 = GNT loopback (CS1_n loopback)[br]
               100 = CKE0 loopback[br]
               101 = reserved (CKE1 loopback; NVMDIMM REQ)[br]
               110 = ODT0 loopback[br]
               111 = ERID loopback (ODT1 loopback, NVMDIMM ERR)
    */
    WriteFnvCfgSmb(host, socket, ch, dimm, DA_DDRT_TRAINING_RC0C_FNV_DA_UNIT_0_REG, (*dimmNvList)[dimm].rcCache[RDIMM_RC0C]);

  } else {

    /*
    RC0C: Training Control Word
      000 = Normal operating mode
      001 = Clock-to-CA training mode1
      010 = DCS0_n loopback mode1
      011 = DCS1_n loopback mode1
      100 = DCKE0 loopback mode1
      101 = DCKE1 loopback mode1
      110 = DODT0 loopback mode1
      111 = DODT1 loopback mode1
    */
    lrbufData = (*dimmNvList)[dimm].rcCache[RDIMM_RC0C];

    // Inphi and IDT Gen 2 RCD Clock-to-CA loopback workaround
    if ( ((((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) && ((*dimmNvList)[dimm].SPDRegRev == INPHI_SPD_REGREV_GEN2_C0)) ||
          (((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) && ((*dimmNvList)[dimm].SPDRegRev == IDT_SPD_REGREV_GEN2_B0)))
          && ((*dimmNvList)[dimm].rcCache[RDIMM_RC0D] & 0x1) && (lrbufData == 0x1)) {

      // configure RC0D = 0 = Dual Rank Mode for when RC0C = 1 = Clock-to-CA training mode
      lrbufData |= (((*dimmNvList)[dimm].rcCache[RDIMM_RC0D] & 0xc) << 4);

    } else {
      lrbufData |= ((*dimmNvList)[dimm].rcCache[RDIMM_RC0D] << 4);
    }

    WriteLrbufSmb (host, socket, ch, dimm, 1, 0x0e, &lrbufData);
  }
} // ChipSetCTLLoopback

void
DisplayCCCResults(
  PSYSHOST  host,
  UINT8     socket
)
/*++
  Displays the training results

  @param host:         Pointer to sysHost
  @param socket:       Processor socket

  @retval N/A
--*/
{
  UINT8               ch;
  UINT8               index;
  UINT8               ioGroup;
  INT16               delay;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_CMD_CTL\n"));

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "            "));
  for (index = 0; index < 7; index++) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %2d  ", index));
  } // index loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    // CMDN
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CMDN"));
    for (ioGroup = 0; ioGroup < 7; ioGroup++) {

      GetSetCmdDelay (host, socket, ch, ioGroup, SIDE_A, GSM_READ_ONLY, &delay); //side = 0
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %3d ", delay));
    } // ioGroup loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));

     //CMDS
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CMDS"));
    for (ioGroup = 7; ioGroup < 14; ioGroup++) {

      GetSetCmdDelay (host, socket, ch, ioGroup, SIDE_A, GSM_READ_ONLY, &delay); //side = 1
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %3d ", delay));
    } // ioGroup loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));

    // CKE
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"CKE "));
    for (ioGroup = 0; ioGroup < 5; ioGroup++) {

      GetSetCmdDelay (host, socket, ch, ioGroup, SIDE_B, GSM_READ_ONLY, &delay); //side = 1
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %3d ", delay));
    } // ioGroup loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));

    // CTL
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"CTL "));
    for (ioGroup = 7; ioGroup < 12; ioGroup++) {

      GetSetCmdDelay (host, socket, ch, ioGroup, SIDE_B, GSM_READ_ONLY, &delay); //side = 0
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %3d ", delay));
    } // ioGroup loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));

  } // ch loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\nSTART_DATA_CLK\n"));

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "       "));
  for (index = 0; index < 4; index++) {
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "  %3d ", index));
  } // index loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  ""));
    for (ioGroup = 0; ioGroup < 4; ioGroup++) {

      GetSetClkDelay (host, socket, ch, ioGroup, GSM_READ_ONLY, &delay);
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "  %3d ", delay));
    } // ioGroup loop

    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n"));
  } // ch loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n"));
} // DisplayCCCResults

void
CheckTwrMargins(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     tWR,
  UINT8     *DisCh
)
{
} // CheckTwrMargins

void
NormalizeLockstepTiming(
  PSYSHOST  host,
  UINT8     socket
)
{
} // NormalizeLockstepTiming

UINT8
GetVocVrefFactor (
  PSYSHOST  host
)
{
  return 2; // conversion factor of 1 to 1
} // GetVocVrefFactor

UINT32
SetMaxDelta (
  PSYSHOST  host,
  UINT32    maxAllowedDelta
)
{
  return maxAllowedDelta;
} // SetMaxDelta

/**

  This gets the Tx Eq settings list

  @param host    - Pointer to sysHost
  @param socket    - Socket number

  @retval N/A

**/
UINT8
GetTxEqSettingsList (
             PSYSHOST host,
             UINT8    socket,
             INT16    settingList[MAX_ODT_SETTINGS][MAX_PARAMS]
             )
{
  UINT8 loop;

  for (loop = 0; loop < MAX_TX_EQ; loop++) {
    settingList[loop][0] = loop*2;
  }

  return MAX_TX_EQ;
} // GetTxEqSettingsList

/**

  This gets the TCO COMP settings list - Stub for SKX

  @param host    - Pointer to sysHost
  @param socket    - Socket number
  @param settingLIst - pointer to list of settings

  @retval numTcoCompSettings - number of TCO COMP settings

**/
UINT8
GetTcoCompSettingsList(
  PSYSHOST host,
  UINT8    socket,
  INT16    settingList[MAX_SETTINGS][MAX_PARAMS]
  )
{
  return 0;
}

/**

  This programs nodata_trn_en

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param value   - Value to program nodata_trn_en

  @retval N/A

**/
void
ProgramNoDataTrnEn (
             PSYSHOST host,
             UINT8    socket,
             UINT8    value
             )
{
  UINT8                                             ch;
  UINT8                                             dimm;
  struct channelNvram                               (*channelNvList)[MAX_CH];
  struct dimmNvram                                  (*dimmNvList)[MAX_DIMM];
  DA_DDRT_TRAINING_DEFEATURE0_FNV_DA_UNIT_0_STRUCT  ddrtTrainingDefeature;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        ddrtTrainingDefeature.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_DEFEATURE0_FNV_DA_UNIT_0_REG);
        ddrtTrainingDefeature.Bits.nodata_trn_en = value;
        WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_DEFEATURE0_FNV_DA_UNIT_0_REG, ddrtTrainingDefeature.Data);
    } // dimm loop
  } // ch loop

} // ProgramNoDataTrnEn


/**

  This programs CaVrefSelect

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @retval N/A

**/
UINT8
CaVrefSelect (
               PSYSHOST host
)
{
  UINT8 controlWordData;

  //HSD 5330295
  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    controlWordData = 0;
  } else {
    controlWordData = 8;
  }

  return controlWordData;
} //CaVrefSelect

UINT8
SkipReceiveEnable (
  PSYSHOST host
  )
/*++

  RCVEN is normally run twice - once in Early CTL/CLK and again after Early CMD/CLK.
  Return success if running RCVEN the second time.

  @param host            - Pointer to sysHost

  @retval   0 - RCVEN has not been run
            1 - RCVEN has been run once already

--*/
{
  return 0;
} //SkipReceiveEnable

VOID
ReceiveEnableInitPerCh (
    PSYSHOST    host,
    UINT8       socket,
    UINT8       ch,
    UINT8       dimm,
    UINT8       rank
    )
{
}

VOID
ReceiveEnableInitAllRanks (
  PSYSHOST host,
  UINT8    socket
  )
{
  UINT8                             ch;
  struct channelNvram               (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    ReceiveEnableInitChip ( host,  socket,  ch);
  } // ch loop

  //
  // Clear nodata_trn_en
  //
  ProgramNoDataTrnEn(host, socket, 0);
}

VOID
ReceiveEnableInitChip (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch
  )
/*++

  Initialize channel for Recieve Enable training.
  This initialization is needed each time training is performed on a new rank.

  @param host            - Pointer to sysHost
  @param socket          - Socket number

  @retval  N/A

--*/
{
  UINT8                             dimm;
  UINT8                             rank;
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      //
      // Set initial Roundtrip Latency values
      //
      SetInitRoundTrip(host, socket, ch, dimm, rank);
    } // rank loop
  } // dimm loop
  SetIoLatComp( host, socket, ch);
} // ReceiveEnableInitChip

VOID
RankRoundTrip (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
/*++

  Initialize channel for Recieve Enable training.
  This initialization is needed each time training is performed on a new rank.

  @param host            - Pointer to sysHost
  @param socket          - Socket number
  @param ch              - Channel number (0-based)
  @param dimm            - DIMM number
  @param rank            - Rank number

  @retval  N/A

--*/
{
} // RankRoundTrip

void
EarlyCmdSignalTest(
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chBitmask
  )
/*++

Routine Description:

  Start and stop Early CMD/CLK tests

Arguments:

  host        - Pointer to sysHost
  socket      - Socket number
  chBitmask   - Channels to execute test on

Returns:

  N/A

--*/
{

 ExecuteCmdSignalTest (host, socket, chBitmask);

} // EarlyCmdSignalTest

UINT32
EarlyCmdSignalStop(
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chBitmask,
  UINT8     dimm,
  UINT32    bwSerr[MAX_CH][3]
  )
/*++

Routine Description:

  Stop Early CMD/CLK tests

Arguments:

  host        - Pointer to sysHost
  socket      - Socket number
  chBitmask   - Channels to execute test on

Returns:

  N/A

--*/
{
  return 0;
} // EarlyCmdSignalStop
/**

  Checks if Rx Vref tuning is required

  @param host    - Pointer to sysHost

  @retval BOOLEAN - true if the chip should enable RxVref tuning

 **/
BOOLEAN
ShouldChipEnableRxVrefTuning (
    PSYSHOST host
)
{
  return FALSE;
}

VOID
RxVrefTuning (
              PSYSHOST  host,
              UINT8     socket,
              GSM_GT    group
)
{
     //HSX only
}
/**

  This initializes the imode setting list

  @param host    - Pointer to sysHost
  @param socket    - Socket number
  @param settingLIst - pointer to list of settings


  @retval numImodeSettings - number of Imode settings

**/
UINT8
GetImodeSettingsList (
  PSYSHOST host,
  UINT8    socket,
  INT16    settingList[MAX_ODT_SETTINGS][MAX_PARAMS]
)
{
  UINT8 numSettings = 0;
  UINT8 loop;
  INT16 SweepRange[9] = {{0}, {1}, {3}, {4}, {5}, {7}, {12}, {13}, {15}};

  numSettings = 9; // Number of points characterised.
  for (loop = 0; loop < numSettings; loop++) {
    settingList[loop][0] = SweepRange[loop];
  }

  return numSettings;
} // GetImodeSettingsList

/**

  This functions is for cleaning up any chipset bugs related to ReadDqDqsPerBit training

  @param host     - Pointer to sysHost
  @param socket   - Socket number

  @retval - None

**/
void
ReadDqDqsCleanup(
  PSYSHOST host,
  UINT8 socket
)
{
  UINT8   ch;
  UINT8   dimm;
  UINT8   rank;
  UINT8   strobe;
  UINT8   numOfNibbles;
  INT16   compositeRxDqDelay;
  UINT32  sumRxDqDqsDelay, averageRxDqDqsDelay;
  INT16   rxDqDqsPi, rxDqPi, rxDqsPi, delta;
  struct  dimmNvram     (*dimmNvList)[MAX_DIMM];
  struct channelNvram   (*channelNvList)[MAX_CH];

  //
  // HSD 4929008 -DQ can't be different per rank for same nibble setting
  // Currently all steppings
  //
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Starting_ReadDqDqsCleanup\n"));
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    for (strobe = 0; strobe < MAX_STROBE/2; strobe++) {
      sumRxDqDqsDelay = 0;
      numOfNibbles = 0;
      compositeRxDqDelay = 0;
      averageRxDqDqsDelay = 0;

      // find average RxDqDqs delay to get final DQ setting
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          dimmNvList = GetDimmNvList(host, socket, ch);
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

          // read the combined Dq/Dqs PI setting
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqDqsDelay, GSM_READ_ONLY, &rxDqDqsPi);
          sumRxDqDqsDelay  = sumRxDqDqsDelay  + rxDqDqsPi;
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, RxDqDqsDelay, GSM_READ_ONLY, &rxDqDqsPi);
          sumRxDqDqsDelay  = sumRxDqDqsDelay  + rxDqDqsPi;
          numOfNibbles = numOfNibbles + 2;
        }
      }
      averageRxDqDqsDelay = sumRxDqDqsDelay  / numOfNibbles;
      averageRxDqDqsDelay = averageRxDqDqsDelay + READDQDQS_EXTRA_MARGIN;
      if  (averageRxDqDqsDelay > 71) {
        compositeRxDqDelay = 0;
      } else if (averageRxDqDqsDelay > 63) {
        compositeRxDqDelay = 1;
      } else if (averageRxDqDqsDelay > 55) {
        compositeRxDqDelay = 2;
      } else if (averageRxDqDqsDelay > 47) {
        compositeRxDqDelay = 3;
      } else if (averageRxDqDqsDelay > 39) {
        compositeRxDqDelay = 4;
      } else {
        compositeRxDqDelay = 5;
      }

      // set final DQ delay
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          dimmNvList = GetDimmNvList(host, socket, ch);
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqDelay, GSM_READ_ONLY, &rxDqPi);
          delta = compositeRxDqDelay - rxDqPi;
          rxDqsPi = 8*delta;

          // write the final DQ and DQS settings
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqDelay, GSM_UPDATE_CACHE, &compositeRxDqDelay);
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqsDelay,  GSM_UPDATE_CACHE | GSM_WRITE_OFFSET, &rxDqsPi);

          GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, RxDqDelay, GSM_READ_ONLY, &rxDqPi);
          delta = compositeRxDqDelay - rxDqPi;
          rxDqsPi = 8*delta;
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, RxDqDelay, GSM_UPDATE_CACHE, &compositeRxDqDelay);
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, RxDqsDelay,  GSM_UPDATE_CACHE | GSM_WRITE_OFFSET, &rxDqsPi);
        } // rank
      } // dimm
    } // strobe
  } // ch
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    DisplayResults(host, socket, RxDqDqsDelay);
    DisplayResults(host, socket, RxDqDelay);
    DisplayResults(host, socket, RxDqsPDelay);
    DisplayResults(host, socket, RxDqsNDelay);
    DisplayResults(host, socket, RxDqsBitDelay);
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  }
#endif
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Finished_ReadDqDqsCleanup\n"));
} //ReadDqDqsCleanup

/**

  This functions enables TxOn in datacontrol0

  @param host     - Pointer to sysHost
  @param socket   - Socket number

  @retval - None

**/
void
ToggleTxOnFnv(
              PSYSHOST host,
              UINT8    socket,
              UINT32   chBitmask,
              UINT8    dimm,
              UINT8    value
)
{
  UINT8                             ch;
  UINT8                             strobe;
  DATACONTROL0N0_0_FNV_DDRIO_COMP_STRUCT fnvDataControl0;
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask)) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      GetSetFnvIO (host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG), &fnvDataControl0.Data, 0, FNV_IO_READ, 0xF);
      fnvDataControl0.Bits.txon = value;
      GetSetFnvIO (host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG), &fnvDataControl0.Data, 0, FNV_IO_WRITE, 0xF);
    } // strobe
  } // ch
}

/**

  This functions enables InternalClocks On in datacontrol0

  @param host     - Pointer to sysHost
  @param socket   - Socket number

  @retval - None

**/
void
ToggleInternalClocksOnFnv(
                       PSYSHOST host,
                       UINT8    socket,
                       UINT32   chBitmask,
                       UINT8    dimm,
                       UINT8    value
)
{
  UINT8                             ch;
  UINT8                             strobe;
  DATACONTROL0N0_0_FNV_DDRIO_COMP_STRUCT fnvDataControl0;
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask)) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      GetSetFnvIO (host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG), &fnvDataControl0.Data, 0, FNV_IO_READ, 0xF);
      fnvDataControl0.Bits.internalclockson = value;
      GetSetFnvIO (host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG), &fnvDataControl0.Data, 0, FNV_IO_WRITE, 0xF);
    } // strobe
  } // ch
}

/**

  Get DIMM slot specific latency to be added to round trip

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Ch number
  @param dimm    - Dimm number

  @retval None

**/
UINT8
GetChipAdjForRTL(
    PSYSHOST host,
    UINT8    socket,
    UINT8    ch,
    UINT8    dimm
)
{
  return 0;
}

/**

  Get page table timer status

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Ch number

  @retval TRUE if enabled / FALSE if disabled

**/
BOOLEAN
IsChipPageTableTimerEnabled(
    PSYSHOST host,
    UINT8    socket,
    UINT8    ch
)
{
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT chknBit;

  chknBit.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);

  return (chknBit.Bits.dis_pgt_tmr == 0);
}

/**

  Enable/Disable page table timer

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Ch number
  @param state   - Enable / Disable

  @retval none

**/
void
SetChipPageTableTimer(
    PSYSHOST host,
    UINT8    socket,
    UINT8    ch,
    UINT8    state
)
{
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT   chknBit;

  chknBit.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
  if (state == PGT_TIMER_DISABLE) {
    chknBit.Bits.dis_pgt_tmr = 1;
  } else {
    chknBit.Bits.dis_pgt_tmr = 0;
  }
  MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, chknBit.Data);
}

/**

  This gets  CTLE settings list

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval - number of CTLE settings

**/
UINT8
GetCtleSettingsList (
             PSYSHOST host,
             UINT8    socket,
             INT16    settingList[MAX_ODT_SETTINGS][MAX_PARAMS]
             )
{
  //
  // Override default settingList with chip specific values if required
  //
  INT16  ctleSettings[MAX_CTLE][MAX_PARAMS] = {
    {4,1,0},
    {4,1,1},
    {4,1,2},
    {4,2,0},
    {4,2,1}
  };

  MemCopy ((UINT8 *)settingList, (UINT8 *)ctleSettings, sizeof(ctleSettings));

  return MAX_CTLE;
} // GetCtleSettingsList

/**

  This gets MC_RON settings list

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval - number of MC_RON settings

**/
UINT8
GetMcRonSettingsList (
             PSYSHOST host,
             UINT8    socket,
             INT16    settingList[MAX_ODT_SETTINGS][MAX_PARAMS]
             )
{
  //
  // Override default settingList with chip specific values if required
  //
  INT16 mcRonSettingList[MAX_SETTINGS][MAX_PARAMS] = {{TX_RON_33},{TX_RON_30},{TX_RON_28}};
  MemCopy ((UINT8 *)settingList, (UINT8 *)mcRonSettingList, sizeof(mcRonSettingList));
  return MAX_MCRON;
} // GetMcRonSettingsList

void
RPQDrain(
         PSYSHOST host,
         UINT8    socket,
         UINT8    ch,
         UINT8    dimm,
         UINT8    rank
)
{
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  DDRT_DEFEATURE_MC_2LM_STRUCT              ddrtDefeature;
  DN_SPARE_CTRL_FNV_D_UNIT_0_STRUCT         dSpareCtrlFnv;
  D_DDRT_TRAINING_OTHER_EN_FNV_D_UNIT_0_STRUCT ddrtTrainingOtherEn;

  dimmNvList = GetDimmNvList(host, socket, ch);
  if ((*dimmNvList)[dimm].aepDimmPresent) {

    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                "\nCalling RPQ Draining Sequence\n"));

    //
    // 1. Invalidate Prefetch Cache
    //
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
    //
    // 2. Send Read commands to clear the RPQ. We will send 1 command at a time until the rpq is empty.
    //
    RPQDrainCadbMRSMode(host, socket, ch, dimm, rank);


    // 2. Tell MC to clear Req Counter
    // Program after 32 read commands
    ddrtDefeature.Data = MemReadPciCfgEp (host, socket, ch, DDRT_DEFEATURE_MC_2LM_REG);
    ddrtDefeature.Bits.ignore_ddrt_req = 1;
    MemWritePciCfgEp (host, socket, ch, DDRT_DEFEATURE_MC_2LM_REG, ddrtDefeature.Data);


    ddrtDefeature.Bits.ignore_ddrt_req = 0;
    MemWritePciCfgEp (host, socket, ch, DDRT_DEFEATURE_MC_2LM_REG, ddrtDefeature.Data);
  }
}

void
GetDdrtTiming (
            PSYSHOST host,
            UINT8    socket,
            UINT8    ch,
            UINT8    dimm,
            UINT8    *CASTemp,
            UINT8    *CWLTemp
)
{
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  UINT8 dimmPair;

  dimmNvList = GetDimmNvList(host, socket, ch);
  dimmPair = (BIT0 ^ dimm);
  if (IsLrdimmPresent(host, socket, ch, dimmPair)) {
    *CASTemp = DdrtCASLatencyLRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
    *CWLTemp = DdrtCASWriteLatencyLRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
  } else {
    if ((*dimmNvList)[dimm].fmcType == FMC_EKV_TYPE) {
      *CASTemp = EkvDdrtCASLatencyRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
      *CWLTemp = EkvDdrtCASWriteLatencyRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
    } else {
      *CASTemp = DdrtCASLatencyRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
      *CWLTemp = DdrtCASWriteLatencyRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
    }
  }
}

void
GetDdrtTimingBufferDelay (
            PSYSHOST host,
            UINT8    socket,
            UINT8    ch,
            UINT8    dimm,
            UINT8    *tCLAdd,
            UINT8    *tCWLAdd
)
{
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  UINT8 dimmPair;

  dimmNvList = GetDimmNvList(host, socket, ch);
  dimmPair = (BIT0 ^ dimm);
  if (IsLrdimmPresent(host, socket, ch, dimmPair)) {
    *tCLAdd  = DdrtCASLatencyAdderLRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
    *tCWLAdd = DdrtCASWriteLatencyAdderLRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
  } else {
    if ((*dimmNvList)[dimm].fmcType == FMC_EKV_TYPE) {
      *tCLAdd = EkvDdrtCASLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
      *tCWLAdd = EkvDdrtCASWriteLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
    } else {
      *tCLAdd  = DdrtCASLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
      *tCWLAdd = DdrtCASWriteLatencyAdderRDIMM[host->nvram.mem.socket[socket].ddrtFreq];
    }
  }
}
void
DisableEridReturn(
            PSYSHOST host,
            UINT8    socket
)
{
  UINT8 ch;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT    cpgcDdrtMiscCtl;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    cpgcDdrtMiscCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG);
    cpgcDdrtMiscCtl.Bits.enable_erid_return  = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG, cpgcDdrtMiscCtl.Data);
  }
}

void
FifoTrainReset (
                PSYSHOST host,
                UINT8    socket
)
{
  UINT8 ch;
  UINT8 dimm;
  struct channelNvram                                     (*channelNvList)[MAX_CH];
  struct dimmNvram                                        (*dimmNvList)[MAX_DIMM];
  DA_CXFIFO_TRAININGRESET_FNV_DA_UNIT_1_STRUCT            fifoTrainReset;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      fifoTrainReset.Data = ReadFnvCfg (host, socket, ch, dimm, DA_CXFIFO_TRAININGRESET_FNV_DA_UNIT_1_REG);
      fifoTrainReset.Bits.cxfifo_trainreset = 1;
      WriteFnvCfg (host, socket, ch, dimm, DA_CXFIFO_TRAININGRESET_FNV_DA_UNIT_1_REG, fifoTrainReset.Data);
      fifoTrainReset.Bits.cxfifo_trainreset = 0;
      WriteFnvCfg (host, socket, ch, dimm, DA_CXFIFO_TRAININGRESET_FNV_DA_UNIT_1_REG, fifoTrainReset.Data);
    } // dimm
  } // ch loop
}

UINT8
CheckSktId (
            PSYSHOST host,
            UINT8    socket
)
{
  if (host->var.common.MemRasFlag && (socket != host->var.common.inComingSktId)) return 1;
  else return 0;
}

UINT8
CheckMemRas (
             PSYSHOST host
)
{
  if ((host->var.common.socketId == host->nvram.common.sbspSocketId) && (host->var.common.MemRasFlag == 0)) return 1;
  else return 0;
}

void
LoadCapId (
            PSYSHOST host,
            UINT8    socket
)
{
  //
  // load local CAPID0 thru CAPID6 CSRs to host->var.common.procCom
  //
  host->var.common.procCom[socket].capid0 = ReadCpuPciCfgEx (host, socket, 0, CAPID0_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid1 = ReadCpuPciCfgEx (host, socket, 0, CAPID1_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid2 = ReadCpuPciCfgEx (host, socket, 0, CAPID2_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid3 = ReadCpuPciCfgEx (host, socket, 0, CAPID3_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid4 = ReadCpuPciCfgEx (host, socket, 0, CAPID4_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid5 = ReadCpuPciCfgEx (host, socket, 0, CAPID5_PCU_FUN3_REG);
  host->var.common.procCom[socket].capid6 = ReadCpuPciCfgEx (host, socket, 0, CAPID6_PCU_FUN3_REG);
} // LoadCapId

/**

  Apply settings after AdvancedMemTest ends

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Ch number
  @param scrambleConfigOrg    Original scrambler config

  @retval None

**/
VOID
PostAdvancedMemTestRestoreScrambler (
  PSYSHOST    host,
  UINT8       socket,
  UINT8       ch,
  UINT32      scrambleConfigOrg
  )
{

  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT          scrambleConfig;

  // Restore scrambler
  scrambleConfig.Data = scrambleConfigOrg;
  MemWritePciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG, scrambleConfig.Data);

}

UINT8
isCmdVrefTrainingDone (PSYSHOST host,
                       UINT8    socket
)
{
  return (host->nvram.mem.socket[socket].cmdVrefTrainingDone);
}

void
ToggleBcomFlopEn(PSYSHOST host,
           UINT8    socket,
           UINT8    value
)
{
  UINT8                                     ch;
  UINT8                                     dimm;
  DA_DDRIO_CMDEN_FNV_DA_UNIT_0_STRUCT       ddrioCmdEn;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].fmcType != FMC_EKV_TYPE) continue;

      ddrioCmdEn.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRIO_CMDEN_FNV_DA_UNIT_0_REG);
      ddrioCmdEn.Bits.fnvbcomflopen = value;
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRIO_CMDEN_FNV_DA_UNIT_0_REG, ddrioCmdEn.Data);
    }
  }
}

void
ToggleBcom(PSYSHOST host,
           UINT8    socket,
           UINT8    value
)
{
  UINT8                                     ch;
  UINT8                                     dimm;
  DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_STRUCT ddrtDqBcomCtrl;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].fmcType != FMC_FNV_TYPE) continue;

      ddrtDqBcomCtrl.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG);
      ddrtDqBcomCtrl.Bits.en = value;
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG, ddrtDqBcomCtrl.Data);
    }
  }
}

/*++

  Enables Write CRC

  @param host    - Pointer to sysHost

  @retval N/A

--*/
void
EnableWrCRC (
  PSYSHOST host,
  UINT8    socket
)
{
  UINT8                                   ch;
  UINT8                                   strobe;
  UINT8                                   rank;
  UINT8                                   dimm;
  IOLATENCY1_MCDDC_DP_STRUCT              ioLatency1;
  DATACONTROL4N0_0_MCIO_DDRIO_STRUCT      dataControl4;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  struct dimmNvram                       (*dimmNvList)[MAX_DIMM];
  struct rankDevice                      (*rankStruct)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  //
  // Account for DQ swizzling if necessary
  //
  CheckAndProgramDqSwizzle(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ( (*dimmNvList)[dimm].dimmPresent == 0 ) continue;
      rankStruct = GetRankStruct(host, socket, ch, dimm);
      for (rank = 0;  rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        (*rankStruct)[rank].MR2 |= BIT12;// WrCRC Bit

        // if not S3Resume and not fastwarmboot
        if ((host->var.mem.subBootMode != WarmBootFast) && (host->var.common.bootMode != S3Resume)){
          WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR2, BANK2);
        }
      } // rank
    }  // dimm

    ioLatency1.Data = MemReadPciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG);
    ioLatency1.Bits.cr_wrcrc_enable = 1;
    MemWritePciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG, ioLatency1.Data);

    //per strobe
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      dataControl4.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG));
      dataControl4.Bits.writecrcenable = 1;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), dataControl4.Data);
    } // strobe loop

  } // ch loop

}

/**

  Apply settings before AdvancedMemTest starts

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Ch number

  @retval UINT32 scrambleConfigOrg

**/
UINT32
PreAdvancedMemTest (
  PSYSHOST    host,
  UINT8       socket,
  UINT8       ch
)
{
  UINT32              scrambleConfigOrg;
  CPGC_INORDER_MCDDC_CTL_STRUCT             cpgcInOrder;
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT     mcschedChx;
  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT          scrambleConfig;

  // Disable scrambler
  scrambleConfig.Data = scrambleConfigOrg = MemReadPciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG);
  scrambleConfig.Bits.rx_enable = 0;
  scrambleConfig.Bits.tx_enable = 0;
  scrambleConfig.Bits.ch_enable = 0;
  MemWritePciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG, scrambleConfig.Data);

  cpgcInOrder.Data = MemReadPciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG);
  cpgcInOrder.Bits.wpq_inorder_en = 1;
  MemWritePciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG, cpgcInOrder.Data);

  mcschedChx.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
  mcschedChx.Bits.dis_pgt_tmr = 1;
  MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, mcschedChx.Data);

  return scrambleConfigOrg;
}

/**

  Apply settings after AdvancedMemTest ends

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Ch number
  @param scrambleConfigOrg    Original scrambler config

  @retval None

**/
VOID
PostAdvancedMemTest (
  PSYSHOST    host,
  UINT8       socket,
  UINT8       ch,
  UINT32      scrambleConfigOrg
  )
{

  CPGC_INORDER_MCDDC_CTL_STRUCT             cpgcInOrder;
  MCSCHED_CHKN_BIT_MCDDC_CTL_STRUCT     mcschedChx;
  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT          scrambleConfig;

  // Disable scrambler
  scrambleConfig.Data = scrambleConfigOrg;
  MemWritePciCfgEp (host, socket, ch, MCSCRAMBLECONFIG_MCDDC_DP_REG, scrambleConfig.Data);

  cpgcInOrder.Data = MemReadPciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG);
  cpgcInOrder.Bits.wpq_inorder_en = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_INORDER_MCDDC_CTL_REG, cpgcInOrder.Data);

  mcschedChx.Data = MemReadPciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG);
  mcschedChx.Bits.dis_pgt_tmr = 0;
  MemWritePciCfgEp (host, socket, ch, MCSCHED_CHKN_BIT_MCDDC_CTL_REG, mcschedChx.Data);

}

void
ChipDeassertCKE(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 ckeMask) {
#ifdef MEM_NVDIMM_EN

  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT miscCKECtl;
  miscCKECtl.Data = MemReadPciCfgEp(host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
  miscCKECtl.Bits.cke_override |= ckeMask;
  miscCKECtl.Bits.cke_on &= miscCKECtl.Bits.cke_override;
  miscCKECtl.Bits.cke_on &= ~ckeMask;
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Deasert CKE cke_override=0x%x, cke_on=0x%x\n", miscCKECtl.Bits.cke_override, miscCKECtl.Bits.cke_on));
  MemWritePciCfgEp(host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
#endif
}

void
ProgramDAFnvMprPattern (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT16    pattern
)
{
  DA_DDRT_MPR3_PAGE0_FNV_DA_UNIT_0_STRUCT mprPage0;

  mprPage0.Bits.mpr0 = (UINT8)(pattern & 0xFF);
  mprPage0.Bits.mpr1 = (UINT8)(pattern >> 8);
  mprPage0.Bits.mpr2 = 0x0F;
  mprPage0.Bits.mpr3 = 0;
  WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_MPR3_PAGE0_FNV_DA_UNIT_0_REG, mprPage0.Data);
}

void
ProgramDFnvMprPattern (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT16    pattern
  )
{
  D_DDRT_TRAINING_MPR_FNV_D_UNIT_0_STRUCT mprPage0;

  mprPage0.Bits.mpr0 = (UINT8)(pattern & 0xFF);
  mprPage0.Bits.mpr1 = (UINT8)(pattern >> 8);
  mprPage0.Bits.mpr2 = 0x0F;
  mprPage0.Bits.mpr3 = 0;
  WriteFnvCfg(host, socket, ch, dimm, D_DDRT_TRAINING_MPR_FNV_D_UNIT_0_REG, mprPage0.Data);
}

void
SetRankDAMprFnv (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT16    mprMode
  )
{
  WriteFnvCfg (host, socket, ch, dimm, DA_DDRT_TRAINING_MR3_FNV_DA_UNIT_0_REG, mprMode);
}

void
SetRankDMprFnv (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT16    mprMode
  )
{

  struct dimmNvram                (*dimmNvList)[MAX_DIMM];

  dimmNvList    = GetDimmNvList(host, socket, ch);
  if ((*dimmNvList)[dimm].aepDimmPresent) {
    WriteFnvCfg (host, socket, ch, dimm, D_DDRT_TRAINING_MR3_FNV_D_UNIT_0_REG, mprMode);
  }
} // SetRankMprFnv

void
EnforceWWDR(
            PSYSHOST host,
            UINT8    ch,
            UINT8    *tWWDR
)
{
  //
  // s4928481: Cloned From SKX Si Bug Eco: TxDqPiClkSel changing through MCO corrupts TX FIFO pointers during PXC mode
  //
  //if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    if (host->var.mem.pxcEnabled & (1<<ch)) {
      if (*tWWDR < ENFORCE_WWDR_PXC_EN) {
        *tWWDR = ENFORCE_WWDR_PXC_EN;
      }
    } else {
      if (*tWWDR < ENFORCE_WWDR_PXC_DIS) {
        *tWWDR = ENFORCE_WWDR_PXC_DIS;
      }
    }
  //}
}

void
EnforceWWDD(
            PSYSHOST host,
            UINT8    ch,
            UINT8    *tWWDD
)
{
  //
  // s4928481: Cloned From SKX Si Bug Eco: TxDqPiClkSel changing through MCO corrupts TX FIFO pointers during PXC mode
  //
  //if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    if (host->var.mem.pxcEnabled & (1<<ch)) {
      if (*tWWDD < ENFORCE_WWDD_PXC_EN) {
        *tWWDD = ENFORCE_WWDD_PXC_EN;
      }
    } else {
      if (*tWWDD < ENFORCE_WWDD_PXC_DIS) {
        *tWWDD = ENFORCE_WWDD_PXC_DIS;
      }
    }
  //}
}

UINT8
NumRanksToTrain (
  PSYSHOST host,
  UINT8 socket,
  UINT8 numRanksDefault
  )
{
  return numRanksDefault;
}

UINT32
FindPiCompareResultChip(
            PSYSHOST     host,
            UINT8        socket,
      UINT8        ch,
            UINT8        dimm,
            UINT8        rank,
            TErrorResult *ErrorResult,
      UINT8        rankPresent
)
{
  return FindPiCompareResult (host, socket, dimm, rank, ErrorResult);
}

/**

  Routine Description: Check to see if the frequency needs to be overridden

  @param host    - Pointer to sysHost
  @param ddrFreq - DDR frequency

  @retval N/A
**/
void
ChipOverrideFreq (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8*   ddrFreq
                 )
{

} // ChipOverrideFreq

/**

  Routine Description: Checks if lockstep is enabled

  @param host    - Pointer to sysHost

  @retval N/A
**/
UINT8
ChipLockstepEn (
                 PSYSHOST host
                 )
{
  if(host->nvram.mem.RASmode & STAT_VIRT_LOCKSTEP) {
    return 1;
  } else {
    return 0;
  }
} // ChipLockstepEn

/**

  Routine Description: Checks if mirroring is enabled

  @param host    - Pointer to sysHost

  @retval N/A
**/
UINT8
ChipMirrorEn (
                 PSYSHOST host
                 )
{
  if(host->nvram.mem.RASmode & CH_ALL_MIRROR) {
    return 1;
  } else {
    return 0;
  }
} // ChipMirrorEn

/**

  This function runs two cadb tests to correct cadb pointer behavior. Test patterns will have Chip Selects asserted.
  1. program a separate cadb pattern compared to Backside Receive Enable Phase.
  2. program desired cadb pattern that counts upwards from 0x0 to 0xF

  Pointer will parked at 12
  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval None

**/
void
CadbRcvenTrainingCleanup  (
  PSYSHOST host,
  UINT8    socket
)
{
/*
  UINT8                               ch;
  UINT8                               chBitmask = 0;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT patCadbMuxCtl;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT    cpgcPatCadbCtlReg;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].lrDimmPresent == 0) continue;

    chBitmask |= 1 << ch;
    //
    // Program CADB pattern buffer
    //
    (*channelNvList)[ch].cadbMuxPattern[0] = 0x5555;
    (*channelNvList)[ch].cadbMuxPattern[1] = 0;
    (*channelNvList)[ch].cadbMuxPattern[2] = 0;
    (*channelNvList)[ch].cadbMuxPattern[3] = 0;

    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[0]);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[1]);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[2]);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX3PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[3]);

#ifdef YAM_ENV
    C_delay_d_clk(10);
#endif

  //
  // Program CADB mux control
  //
    patCadbMuxCtl.Data = 0;
    patCadbMuxCtl.Bits.mux0_control = 1;
    patCadbMuxCtl.Bits.mux1_control = 1;
    patCadbMuxCtl.Bits.mux2_control = 1;
    patCadbMuxCtl.Bits.mux3_control = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, patCadbMuxCtl.Data);

#ifdef YAM_ENV
    C_delay_d_clk(10);
#endif

    ProgramCADB (host, socket, ch, 0, CADB_LINES, PrechargePattern0, DeselectPattern1);

    cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
    cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
    cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 1;
    cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);
  }

  ExecuteCmdSignalTest (host, socket, chBitmask);

  for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((*channelNvList)[ch].lrDimmPresent == 0) continue;

      (*channelNvList)[ch].cadbMuxPattern[0] = 0x5555;
      (*channelNvList)[ch].cadbMuxPattern[1] = 0x3333;
      (*channelNvList)[ch].cadbMuxPattern[2] = 0x0F0F;
      (*channelNvList)[ch].cadbMuxPattern[3] = 0x00FF;

      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[0]);
      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[1]);
      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[2]);
      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX3PB_MCDDC_CTL_REG, (*channelNvList)[ch].cadbMuxPattern[3]);
      //
      // Update Cached Values
      //
#ifdef YAM_ENV
      C_delay_d_clk(10);
#endif

      //
      // Program CADB mux control
      //
      patCadbMuxCtl.Data = 0;
      patCadbMuxCtl.Bits.mux0_control = 1;
      patCadbMuxCtl.Bits.mux1_control = 1;
      patCadbMuxCtl.Bits.mux2_control = 1;
      patCadbMuxCtl.Bits.mux3_control = 1;
      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, patCadbMuxCtl.Data);
#ifdef YAM_ENV
      C_delay_d_clk(10);
#endif
  }

  ExecuteCmdSignalTest (host, socket, chBitmask);
*/
}

/**

  This function parses through the KTI var structure to detect if MCP is valid. Future Reference code will use mcpPresent

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval None

**/
void
CheckMCPPresent (
                 PSYSHOST host,
                 UINT8    socket
)
{
  UINT8 stack;
  host->nvram.mem.socket[socket].mcpPresent = 0;
  for (stack = 0; stack < MAX_IIO_STACK; stack++) {
    if (host->var.kti.CpuInfo[socket].CpuRes.StackRes[stack].Personality == TYPE_MCP) {
      host->nvram.mem.socket[socket].mcpPresent = 1;
    }
  }
}

void
Toggle3DSMode(
               PSYSHOST host,
               UINT8    socket,
               UINT8    ch,
               UINT8    value
)
{
  UINT8            dimm;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if ((*dimmNvList)[dimm].aepDimmPresent == 1) continue;
    if (value) {
      MemWritePciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4), (UINT32) (*dimmNvList)[dimm].dimmMemTech);
    } else {
      // zero out ddr4_3dsnumranks_cs (24:23) and hdrl (21:21)
      MemWritePciCfgEp (host, socket, ch, DIMMMTR_0_MC_MAIN_REG + (dimm * 4), (UINT32) ((*dimmNvList)[dimm].dimmMemTech & ~(BIT24 | BIT23 | BIT21)));
    }
  }
}

void
DdrtBufferReset (
                PSYSHOST host,
                UINT8    socket,
                UINT8    ch,
                UINT8    dimm
)
{
  DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_STRUCT   daDdrtRc06;

  FixedDelay (host, 500);
  daDdrtRc06.Data = 0;
  daDdrtRc06.Bits.misc_ctrl = 1;
  WriteFnvCfgSmb(host, socket, ch, dimm, DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_REG, daDdrtRc06.Data);
  daDdrtRc06.Bits.misc_ctrl = 0;
  WriteFnvCfgSmb(host, socket, ch, dimm, DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_REG, daDdrtRc06.Data);

} // DdrtBufferReset

UINT8
GetMaxStrobe(
            PSYSHOST  host,
            UINT8     socket,
            UINT8     ch,
            UINT8     dimm,
            GSM_GT    group,
            GSM_LT    level
            )
/*++
  Get the maximum number of data groups based on DIMM and group type

  @param host  - Pointer to sysHost
  @param socket  - Socket
  @param ch    - Channel
  @param dimm  - DIMM
  @param group - Group type
  @param level - Level type

  @retval Max strobe

--*/
{
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];

  dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;

#ifdef LRDIMM_SUPPORT
  if ((IsLrdimmPresent(host, socket, ch, dimm)) && (group == TxVref) && (level == DdrLevel)) {
    return MAX_STROBE / 2;
  }
  if ((IsLrdimmPresent(host, socket, ch, dimm)) && (group == TxVref) && (level == LrbufLevel)) {
    return MAX_STROBE;
  }
  if ((IsLrdimmPresent(host, socket, ch, dimm)) && (group == RxVref) && (level == LrbufLevel)) {
    return MAX_STROBE / 2;
  }
#endif
  //
  // Setup variables based on x4 or x8 DIMM or chipset specific
  //
  if ((group == TxEq) || (group == TxImode) || (group == RxEq) || (group == RxCtleC) || (group == RxCtleR) ||
    ((group == RxVref) && (level == DdrLevel))) {
     return MAX_STROBE / 2;
  } else if ((*dimmNvList)[dimm].x4Present ||
      (group == RxDqsPDelay) || (group == RxDqsNDelay) || (group == RxDqDqsDelay) ||
      (group == RxDqDelay) || (group == RxDqsDelay) || (group == RecEnDelay) ||
      (group == TxDqDelay) || (group == RxVref) || (group == RxEq) ||
      (group == TxEq) || ((group == TxVref) && (host->nvram.mem.dramType == SPD_TYPE_DDR3))) {
    return MAX_STROBE;
  } else {
    return MAX_STROBE / 2;
  }
} // GetMaxStrobe

void
UpdateDdrtGroupLimits (
                       PSYSHOST host,
                       UINT8    socket,
                       UINT8    ch,
                       UINT8    dimm,
                       GSM_LT   level,
                       GSM_GT   group,
                       UINT16   *minLimit,
                       UINT16   *maxLimit
)
{
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  if ((*dimmNvList)[dimm].aepDimmPresent == 1 && level == LrbufLevel && group == TxVref) {
    *minLimit = 0;
    *maxLimit = 0x7f;
  }
}

void
FnvCWLWorkaround (
                  PSYSHOST host,
                  UINT8    socket,
                  UINT8    ch,
                  UINT8    dimm
)
{
  UINT8 CWLTemp;
  DA_DDRT_TRAINING_MR2_FNV_DA_UNIT_0_STRUCT mr2;
  struct dimmNvram(*dimmNvList)[MAX_DIMM];

  //
  // 1. 2 DPC configs
  // 2. RDIMM Pair
  // 3. 2400 Freq
  //
  dimmNvList = GetDimmNvList(host, socket, ch);
  if ((*dimmNvList)[dimm].fmcRev == FMC_FNV_TYPE) {
    if ((dimm != 0) && (host->nvram.mem.socket[socket].ddrFreq == DDR_2400) && (!IsLrdimmPresent(host, socket, ch, 0))) {
      mr2.Data = ReadFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_MR2_FNV_DA_UNIT_0_REG);
      CWLTemp = DdrtCASWriteLatencyRDIMMFMC[host->nvram.mem.socket[socket].ddrtFreq];
      if (CWLTemp < 14) {
        CWLTemp = CWLTemp - 9;
      } else {
        CWLTemp = (CWLTemp - ((CWLTemp - 14) / 2) - 10);
      }
      mr2.Bits.tcwl = CWLTemp;
      WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_MR2_FNV_DA_UNIT_0_REG, mr2.Data);
    }
  }
}

void
AddChipRxDqsOffset (
                    PSYSHOST host,
                    UINT8    socket,
                    UINT8    ch,
                    UINT8    dimm,
                    UINT8    rank,
                    UINT8    strobe,
                    UINT16   *offset
)
{
}

/**

Offsets CMD, CTL, CLK based on specified offset

@param host      - Pointer to sysHost
@param socket    - Socket Id
@param offset    - Desired offset

@retval N/A

**/
void
OffsetCCC(
          PSYSHOST  host,
          UINT8     socket,
          INT16     offset,
          UINT8     jedecinit
)
{
  UINT8                                       ch;
  UINT8                                       dimm;
  UINT8                                       rank;
  UINT8                                       clk;
  UINT8                                       ckEnabled[MAX_CH][MAX_CLK];
  UINT16                                      minVal = MAX_CMD_MARGIN;
  UINT16                                      maxVal = 0;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct ddrRank                              (*rankList)[MAX_RANK_DIMM];

  if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {

    channelNvList = GetChannelNvList(host, socket);

    for (ch = 0; ch < MAX_CH; ch++) {
      for (clk = 0; clk < MAX_CLK; clk++) {
        ckEnabled[ch][clk] = 0;
      }
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);
        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          clk = (*rankList)[rank].ckIndex;
          ckEnabled[ch][clk] = 1;
        }
      }
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // Offset all CMD and CTL groups
      GetSetCmdGroupDelay(host, socket, ch, CmdAll, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &offset, &minVal, &maxVal);
      GetSetCtlGroupDelay(host, socket, ch, CtlAll, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &offset, &minVal, &maxVal);

      // Offset all enabled clocks
      for (clk = 0; clk < MAX_CLK; clk++) {
        if (ckEnabled[ch][clk] == 0) continue;
        GetSetClkDelay(host, socket, ch, clk, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &offset);
      }
    }
    // Moved CLKs so need JedecInit
    if (jedecinit) {
      ToggleBcom(host, socket, 1);
      JedecInitSequence (host, socket, CH_BITMASK);
    }
  } // B0
} // OffsetCCC

void
CheckValidFirmwareVersion(
                          PSYSHOST       host,
                          UINT8          socket,
                          UINT8          ch,
                          UINT8          dimm,
                          FNVINFORMATION FnvInformation
)
{
  UINT8 firmwareRequired;

  if (host->setup.mem.enableNgnBcomMargining) {
    firmwareRequired = BCOM_MARGINING_FW;
  } else {
    firmwareRequired = CLK_MOVEMENT_FW;
  }

  if ((SupportedFirmware[firmwareRequired][MAJOR_VERSION_INDEX] > FnvInformation->fwr.majorVersion) ||
    SupportedFirmware[firmwareRequired][MINOR_VERSION_INDEX] > FnvInformation->fwr.minorVersion ||
    SupportedFirmware[firmwareRequired][HOTFIX_INDEX] > FnvInformation->fwr.hotfixVersion ||
    SupportedFirmware[firmwareRequired][BUILD_INDEX] > FnvInformation->fwr.buildVersion) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
      "FW not supported. Please update NVMCTLR FW to atleast %02d.%02d.%02d.%04x\n",
      SupportedFirmware[firmwareRequired][MAJOR_VERSION_INDEX],
      SupportedFirmware[firmwareRequired][MINOR_VERSION_INDEX],
      SupportedFirmware[firmwareRequired][HOTFIX_INDEX],
      SupportedFirmware[firmwareRequired][BUILD_INDEX]));
    if (firmwareRequired == CLK_MOVEMENT_FW) {
      OutputWarning(host, WARN_FW_OUT_OF_DATE, WARN_FW_CLK_MOVEMENT, socket, ch, dimm, NO_RANK);
    } else {
      OutputWarning(host, WARN_FW_OUT_OF_DATE, WARN_FW_BCOM_MARGINING, socket, ch, dimm, NO_RANK);
    }
  }
}

/**

This routine initializes the memory size fields in the structures
for DIMMs, Channels, and Nodes.  The sizes are calculated from the sizes in
the rank structures.

@param host  - Pointer to sysHost

@retval void

**/
void DisplayFnvInfo(PSYSHOST host)
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  struct memNvram *nvramMem;
  struct channelNvram(*channelNvList)[MAX_CH];
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  struct fnvInformation fnvInfoList;
  UINT8  mbStatus = 0;
  UINT32 status = SUCCESS;

  nvramMem = &host->nvram.mem;
  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (nvramMem->socket[sckt].enabled == 0) continue;
    channelNvList = &nvramMem->socket[sckt].channelList;

    // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;

        //Skip if this is not a DDRT dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        status = GetFnvInfo(host, sckt, ch, dimm, &fnvInfoList, &mbStatus);

        if (status == SUCCESS) {

#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MAX)) {
            getPrintFControl(host);
          }
#endif // SERIAL_DBG_MSG

          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
            "Firmware Revision: %02x.%02x.%02x.%04x Firmware Type: 0x%x\n",
            fnvInfoList.fwr.majorVersion, fnvInfoList.fwr.minorVersion, fnvInfoList.fwr.hotfixVersion, fnvInfoList.fwr.buildVersion, fnvInfoList.fwrType));

          if (!(host->var.common.emulation & SIMICS_FLAG)) {
            CheckValidFirmwareVersion(host, sckt, ch, dimm, &fnvInfoList);
          }
#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MAX)) {
            releasePrintFControl(host);
          }
#endif // SERIAL_DBG_MSG
       }
      }//dimm
    }//ch
  } //socket
}

/**
  Populate the MEM_PLATFORM_POLICY_SIM structure in SysHost

  @param  host  - Pointer to SysHost structure

  @retval MRC_STATUS_SUCCESS
**/
static MRC_STATUS
PopulateMemChipPolicySim (
  PSYSHOST host
  )
{
  MRC_STATUS    status = MRC_STATUS_SUCCESS;

  return status;
}

/**
  Populate the MEM_PLATFORM_POLICY structure in SysHost

  @param  host  - Pointer to SysHost structure

  @retval MRC_STATUS_SUCCESS
**/
MRC_STATUS
PopulateMemChipPolicy (
  PSYSHOST host
  )
{
  MRC_STATUS    status = MRC_STATUS_SUCCESS;

  MEM_CHIP_POLICY_DEF(maxVrefSettings)                       = MAX_VREF_SETTINGS;
  MEM_CHIP_POLICY_DEF(earlyVrefStepSize)                     = EARLY_VREF_STEP_SIZE;
  MEM_CHIP_POLICY_DEF(minCmdVref)                            = MIN_CMD_VREF;
  MEM_CHIP_POLICY_DEF(maxCmdVref)                            = MAX_CMD_VREF;
  MEM_CHIP_POLICY_DEF(cas2DrvenMaxGap)                       = CAS2DRVEN_MAXGAP;
  MEM_CHIP_POLICY_DEF(minIoLatency)                          = MIN_IO_LATENCY;
  MEM_CHIP_POLICY_DEF(mrcRoundTripIoComp)                    = MRC_ROUND_TRIP_IO_COMPENSATION;
  MEM_CHIP_POLICY_DEF(mrcRoundTripIoCompStart)               = MRC_ROUND_TRIP_IO_COMP_START;
  MEM_CHIP_POLICY_DEF(mrcRoundTripMax)                       = MRC_ROUND_TRIP_MAX_VALUE;
  MEM_CHIP_POLICY_DEF(mrcRoundTripDefault)                   = MRC_ROUND_TRIP_DEFAULT_VALUE;
  MEM_CHIP_POLICY_DEF(SrMemoryDataStorageDispatchPipeCsr)    = SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR;
  MEM_CHIP_POLICY_DEF(SrMemoryDataStorageCommandPipeCsr)     = SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR;
  MEM_CHIP_POLICY_DEF(SrMemoryDataStorageDataPipeCsr)        = SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR;
  MEM_CHIP_POLICY_DEF(SrBdatStructPtrCsr)                    = SR_BDAT_STRUCT_PTR_CSR;
  MEM_CHIP_POLICY_DEF(BiosStickyScratchPad0)                 = BIOSSCRATCHPAD0_UBOX_MISC_REG;
  MEM_CHIP_POLICY_DEF(PerBitMarginDefault)                   = PER_BIT_MARGIN_DEFAULT;
  MEM_CHIP_POLICY_DEF(RxOdtDefault)                          = RX_ODT_DEFAULT;
  MEM_CHIP_POLICY_DEF(RmtColdFastBootDefault)                = RMT_COLD_FAST_BOOT_DEFAULT;
  MEM_CHIP_POLICY_DEF(FastBootDefault)                       = FAST_BOOT_DEFAULT;

  status = PopulateMemChipPolicySim(host);

  return status;
}

/**
  Return pointer to the ODT value table. The table is an array of
  ODT value structures.
  Also returns the size of the array.

  @param  host      - Pointer to SysHost structure
  @param  tablePtr  - Pointer to caller's ODT table pointer
  @param  tableSize - Pointer to caller's ODT table size variable.

  @retval MRC_STATUS_SUCCESS if table is found
**/
MRC_STATUS
GetDdr4OdtValueTable (
  PSYSHOST                    host,
  struct ddr4OdtValueStruct   **tablePtr,
  UINT16                      *tableSize
  )
{
  MRC_STATUS    status = MRC_STATUS_SUCCESS;

  *tablePtr = &ddr4OdtValueTable[0];
  *tableSize = sizeof(ddr4OdtValueTable)/sizeof(ddr4OdtValueTable[0]);

  return status;
}

/**
  Indicate if a register offset is in the list of offsets to skip for register tracing

  @param  host      - Pointer to sysHost structure
  @param  RegOffset - The register offset to check

  @retval TRUE if the register offset is in the list to skip
          FALSE otherwise
**/
BOOLEAN
InCsrTraceRegsSkipList (
  PSYSHOST      host,
  UINT32        RegOffset
  )
{
  if ((RegOffset == SR_POST_CODE_CSR) ||
      (RegOffset == SR_BIOS_SERIAL_DEBUG_CSR) ||
      (RegOffset == SR_PRINTF_SYNC_CSR)) {
    return TRUE;
  }
  return FALSE;
}

UINT8
CheckS3Jedec(PSYSHOST host,
UINT8 socket,
UINT8 ch,
UINT8 dimm
)
{
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  if (host->var.common.bootMode == NormalBoot) {
    return 1;
  } else {
    if ((*dimmNvList)[dimm].aepDimmPresent) {
      return 1;
    } else {
      return 0;
    }
  }
}

UINT8
CheckBacksideSwizzle(
PSYSHOST host,
UINT8    socket,
UINT8    ch,
UINT8    dimm,
UINT8    strobe
)
{
  struct dimmNvram(*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  if ((*dimmNvList)[dimm].aepDimmPresent) {
    if ((*dimmNvList)[dimm].lrbufswizzle & (1 << (strobe % 9))) {
      if (strobe > 8) {
        return strobe - 9;
      } else {
        return strobe + 9;
      }
    } else {
      return strobe;
    }
  } else{
    return strobe;
  }
}