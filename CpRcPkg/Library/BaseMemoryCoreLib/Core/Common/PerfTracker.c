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
* Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
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

#include "SysFunc.h"
#include "Pipe.h"


#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG

//
// Local Prototypes
//
void   countData(PSYSHOST host, struct perfData *perfDataPtr, UINT8 mode, UINT32 data);
void   PrintStats(PSYSHOST host, struct perfData *perfDataPtr);
void   ClearPerfData (PSYSHOST host, struct perfData *perfDataPtr);
BOOLEAN perfDataNonZero (PSYSHOST host, struct perfData *perfDataPtr);

/**

  Counts this PCI access

  @param host -  Pointer to sysHost
  @param mode -    PCI_ACCESS          0
                   JEDEC_COUNT         2
                   FIXED_DELAY         3
                   POLLING_COUNT       6
                   VREF_MOVE           7
  @param data -    data to add to count

  @retval N/A

**/
void
countTrackingData(
                 PSYSHOST  host,
                 UINT8     mode,
                 UINT32    data
                 )
{
  struct  perfData  *perfDataPtr;


  //
  // Always count in the "all" structure
  //
  perfDataPtr = &host->var.mem.all;
  countData(host, perfDataPtr, mode, data);

  switch (host->var.mem.currentMemPhase) {
    case JEDEC_INIT:
      perfDataPtr = &host->var.mem.jedecInit;
      countData(host, perfDataPtr, mode, data);
      break;

    case RANK_MARGIN_TOOL:
      perfDataPtr = &host->var.mem.rankMarginTool;
      countData(host, perfDataPtr, mode, data);
      break;

    case CMD_CLK_EARLY:
      perfDataPtr = &host->var.mem.cmdClkEarly;
      countData(host, perfDataPtr, mode, data);
      break;

    case TRAIN_TX_EQ:
      perfDataPtr = &host->var.mem.txEqTrn;
      countData(host, perfDataPtr, mode, data);
      break;

    case WR_VREF:
      perfDataPtr = &host->var.mem.wrVref;
      countData(host, perfDataPtr, mode, data);
      break;

    case RD_VREF:
      perfDataPtr = &host->var.mem.rdVref;
      countData(host, perfDataPtr, mode, data);
      break;

    case DIMM_DETECT:
      perfDataPtr = &host->var.mem.dimmDetect;
      countData(host, perfDataPtr, mode, data);
      break;

    case CHECK_DIMM_RANKS:
      perfDataPtr = &host->var.mem.chkDimmRanks;
      countData(host, perfDataPtr, mode, data);
      break;

    case GATHER_SPD:
      perfDataPtr = &host->var.mem.gatherSPD;
      countData(host, perfDataPtr, mode, data);
      break;

    case EARLY_CONFIG:
      perfDataPtr = &host->var.mem.earlyConfig;
      countData(host, perfDataPtr, mode, data);
      break;


    case DDRIO_INIT:
      perfDataPtr = &host->var.mem.ddrioInit;
      countData(host, perfDataPtr, mode, data);
      break;

    case CMD_CLK:
      perfDataPtr = &host->var.mem.cmdClk;
      countData(host, perfDataPtr, mode, data);
      break;

    case CTL_CLK_EARLY:
      perfDataPtr = &host->var.mem.ctlClkEarly;
      countData(host, perfDataPtr, mode, data);
      break;

    case XOVER_CALIB:
      perfDataPtr = &host->var.mem.xoverCalib;
      countData(host, perfDataPtr, mode, data);
      break;

    case REC_EN:
      perfDataPtr = &host->var.mem.recEn;
      countData(host, perfDataPtr, mode, data);
      break;

    case RDDQDQS:
      perfDataPtr = &host->var.mem.rdDqDqs;
      countData(host, perfDataPtr, mode, data);
      break;

    case SWIZZLE_DISCOVERY:
      perfDataPtr = &host->var.mem.swizzleDiscovery;
      countData(host, perfDataPtr, mode, data);
      break;

    case WR_LVL:
      perfDataPtr = &host->var.mem.wrLvl;
      countData(host, perfDataPtr, mode, data);
      break;

    case WRDQDQS:
      perfDataPtr = &host->var.mem.wrDqDqs;
      countData(host, perfDataPtr, mode, data);
      break;

    case E_RD_VREF:
      perfDataPtr = &host->var.mem.EarlyRdVref;
      countData(host, perfDataPtr, mode, data);
      break;

    case E_WR_VREF:
      perfDataPtr = &host->var.mem.EarlyWrVref;
      countData(host, perfDataPtr, mode, data);
      break;

    case WR_FLYBY:
      perfDataPtr = &host->var.mem.wrFlyBy;
      countData(host, perfDataPtr, mode, data);
      break;

    case CLOCK_INIT:
      perfDataPtr = &host->var.mem.clockInit;
      countData(host, perfDataPtr, mode, data);
      break;

    case E_RID:
      perfDataPtr = &host->var.mem.EarlyRid;
      countData(host, perfDataPtr, mode, data);
      break;

    case RDDQDQS_ADV:
      perfDataPtr = &host->var.mem.rxDqAdv;
      countData(host, perfDataPtr, mode, data);
      break;

    case WRDQDQS_ADV:
      perfDataPtr = &host->var.mem.txDqAdv;
      countData(host, perfDataPtr, mode, data);
      break;

    case NORMAL_MODE:
      perfDataPtr = &host->var.mem.normalMode;
      countData(host, perfDataPtr, mode, data);
      break;

    case MEM_MAPPING:
      perfDataPtr = &host->var.mem.memMapping;
      countData(host, perfDataPtr, mode, data);
      break;

    case RAS_CONFIG:
      perfDataPtr = &host->var.mem.rasConfig;
      countData(host, perfDataPtr, mode, data);
      break;

    case EYE_DIAGRAM:
      perfDataPtr = &host->var.mem.eyeDiagram;
      countData(host, perfDataPtr, mode, data);
      break;

    case BIT_DESKEW_RX:
      perfDataPtr = &host->var.mem.perBitDeskewRx;
      countData(host, perfDataPtr, mode, data);
      break;

    case BIT_DESKEW_TX:
      perfDataPtr = &host->var.mem.perBitDeskewTx;
      countData(host, perfDataPtr, mode, data);
      break;

    case CMD_VREF_CEN:
      perfDataPtr = &host->var.mem.cmdVref;
      countData(host, perfDataPtr, mode, data);
      break;

#ifdef LRDIMM_SUPPORT
    case WR_VREF_LRDIMM:
      perfDataPtr = &host->var.mem.wrVrefLrdimm;
      countData(host, perfDataPtr, mode, data);
      break;

    case RD_VREF_LRDIMM:
      perfDataPtr = &host->var.mem.rdVrefLrdimm;
      countData(host, perfDataPtr, mode, data);
      break;

    case RX_DQ_CENTERING_LRDIMM:
      perfDataPtr = &host->var.mem.rxLrdimmDqCentering;
      countData(host, perfDataPtr, mode, data);
      break;

    case TX_DQ_CENTERING_LRDIMM:
      perfDataPtr = &host->var.mem.txLrdimmDqCentering;
      countData(host, perfDataPtr, mode, data);
      break;
#endif

    case RT_OPT:
      perfDataPtr = &host->var.mem.rtOpt;
      countData(host, perfDataPtr, mode, data);
      break;

#ifdef LRDIMM_SUPPORT
    case LRDIMM_RX:
      perfDataPtr = &host->var.mem.LrdimmRx;
      countData(host, perfDataPtr, mode, data);
      break;

    case LRDIMM_TX:
      perfDataPtr = &host->var.mem.LrdimmTx;
      countData(host, perfDataPtr, mode, data);
      break;
#endif

    case TRAIN_IMODE:
      perfDataPtr = &host->var.mem.trainImode;
      countData(host, perfDataPtr, mode, data);
      break;

    case TRAIN_CTLE:
      perfDataPtr = &host->var.mem.trainRXCTLE;
      countData(host, perfDataPtr, mode, data);
      break;

    case TRAIN_MC_RON:
      perfDataPtr = &host->var.mem.trainMcRon;
      countData(host, perfDataPtr, mode, data);
      break;

    case TRAIN_RTT_WR:
      perfDataPtr = &host->var.mem.trainRttWr;
      countData(host, perfDataPtr, mode, data);
      break;

    case TRAIN_RX_ODT:
      perfDataPtr = &host->var.mem.trainRxOdt;
      countData(host, perfDataPtr, mode, data);
      break;

    case  TRAIN_DRAM_RON:
      perfDataPtr = &host->var.mem.trainDramRon;
      countData(host, perfDataPtr, mode, data);
      break;

    case LATE_CONFIG:
      perfDataPtr = &host->var.mem.lateConfig;
      countData(host, perfDataPtr, mode, data);
      break;

    case MEM_BIST:
      perfDataPtr = &host->var.mem.memTest;
      countData(host, perfDataPtr, mode, data);
      break;

    case MEM_INIT:
      perfDataPtr = &host->var.mem.memInit;
      countData(host, perfDataPtr, mode, data);
      break;

#ifdef PPR_SUPPORT
    case POST_PKG_RPR:
      perfDataPtr = &host->var.mem.ppr;
      countData(host, perfDataPtr, mode, data);
      break;
#endif // PPR_SUPPORT

    //NOZONE
    default:
      if (host->var.mem.noZoneIndex < MAX_NOZONE) {
        perfDataPtr = &host->var.mem.noZone[host->var.mem.noZoneIndex];
        countData(host, perfDataPtr, mode, data);
      }
      break;
  }
}


/**

  Counts this data access

  @param host -  Pointer to sysHost
  @param perfDataPtr - Pointer to current performance pointer data structure
  @param mode -    PCI_ACCESS          0
                   JEDEC_COUNT         2
                   FIXED_DELAY         3
                   POLLING_COUNT       6
                   VREF_MOVE           7
               CPGC_COUNT          8
                   DURATION_TIME       9
  @param data -    data to add to count
  @retval N/A

**/
void
countData(
         PSYSHOST  host,
         struct    perfData  *perfDataPtr,
         UINT8     mode,
         UINT32    data
         )
{

  switch (mode) {
  case PCI_ACCESS:
    (*perfDataPtr).pciCount += data;
    break;

  case JEDEC_COUNT:
    (*perfDataPtr).jedecCount += data;
    break;

  case FIXED_DELAY:
    (*perfDataPtr).fixedDelay += data;
    break;

  case POLLING_COUNT:
    (*perfDataPtr).pollingCount += data;
    break;

  case VREF_MOVE:
    (*perfDataPtr).vrefCount += data;
    break;

  case CPGC_COUNT:
    (*perfDataPtr).cpgcCount += data;
    break;

  case DURATION_TIME:
    (*perfDataPtr).durationTime += data;
    break;
  }
} // countData

struct CheckpointToPhase {
  UINT32  majorCheckPoint;
  UINT32  minorCheckPoint;
  UINT32  phase;
};

#define phaseTableSize sizeof(phaseTable)/sizeof(phaseTable[0])
const struct CheckpointToPhase phaseTable[] = {
  { STS_DIMM_DETECT,                                    0,  DIMM_DETECT },
  { STS_CLOCK_INIT,                                     0,  CLOCK_INIT },
  { STS_SPD_DATA,                                       0,  GATHER_SPD },
  { STS_GLOBAL_EARLY,                                   0,  EARLY_CONFIG },
#ifndef RCSIM
  { STS_JEDEC_INIT,                                     0,  JEDEC_INIT },
#endif
  { STS_DDRIO_INIT,                                     0,  DDRIO_INIT },
  { STS_DDRIO_INIT,                            EARLY_INIT,  DDRIO_INIT },
  { STS_DDRIO_INIT,                             LATE_INIT,  DDRIO_INIT },
  { STS_CHANNEL_TRAINING,           CROSSOVER_CALIBRATION,  XOVER_CALIB },
  { STS_CHANNEL_TRAINING,                       SENSE_AMP,  SENSE_AMPLIFIER },
  { STS_CHANNEL_TRAINING,            RECEIVE_ENABLE_BASIC,  REC_EN },
  { STS_CHANNEL_TRAINING,                 RX_DQ_DQS_BASIC,  RDDQDQS },
  { STS_CHANNEL_TRAINING,            DQ_SWIZZLE_DISCOVERY,  SWIZZLE_DISCOVERY },
  { STS_CHANNEL_TRAINING,            WRITE_LEVELING_BASIC,  WR_LVL },
  { STS_CHANNEL_TRAINING,                     TX_DQ_BASIC,  WRDQDQS },
  { STS_CHANNEL_TRAINING,                 E_TRAIN_WR_VREF,  E_WR_VREF },
  { STS_CHANNEL_TRAINING,                 E_TRAIN_RD_VREF,  E_RD_VREF },
  { STS_CHANNEL_TRAINING,                   TRAIN_RD_VREF,  RD_VREF },
  { STS_CHANNEL_TRAINING,                    LATE_CMD_CLK,  CMD_CLK },
  { STS_CHANNEL_TRAINING,                  EARLY_RID_FINE,  E_RID },
  { STS_CHANNEL_TRAINING,              CMD_VREF_CENTERING,  CMD_VREF_CEN },
  { STS_CHANNEL_TRAINING,                   EARLY_CMD_CLK,  CMD_CLK_EARLY },
  { STS_CHANNEL_TRAINING,                   EARLY_CTL_CLK,  CTL_CLK_EARLY },
  { STS_CHANNEL_TRAINING,                    TRAIN_RD_DQS,  RDDQDQS_ADV },
  { STS_CHANNEL_TRAINING,                    TRAIN_WR_DQS,  WRDQDQS_ADV },
  { STS_CHANNEL_TRAINING,                         STS_RMT,  RANK_MARGIN_TOOL },
  { STS_CHANNEL_TRAINING,           STS_MINOR_NORMAL_MODE,  NORMAL_MODE },
  { STS_DDR_MEMMAP,                                     0,  MEM_MAPPING },
  { STS_RAS_CONFIG,                                     0,  RAS_CONFIG },
  { STS_CHANNEL_TRAINING,                       WR_FLY_BY,  WR_FLYBY },
  { STS_CHANNEL_TRAINING,               PER_BIT_DESKEW_RX,  BIT_DESKEW_RX },
  { STS_CHANNEL_TRAINING,               PER_BIT_DESKEW_TX,  BIT_DESKEW_TX },
  { STS_CHANNEL_TRAINING,                   TRAIN_WR_VREF,  WR_VREF },
  { STS_CHANNEL_TRAINING,             ROUND_TRIP_OPTIMIZE,  RT_OPT },
#ifdef LRDIMM_SUPPORT
  { STS_CHANNEL_TRAINING,      RX_BACKSIDE_PHASE_TRAINING,  LRDIMM_RX },
  { STS_CHANNEL_TRAINING,      RX_BACKSIDE_CYCLE_TRAINING,  LRDIMM_RX },
  { STS_CHANNEL_TRAINING,      RX_BACKSIDE_DELAY_TRAINING,  LRDIMM_RX },
  { STS_CHANNEL_TRAINING,    TX_BACKSIDE_FINE_WL_TRAINING,  LRDIMM_TX },
  { STS_CHANNEL_TRAINING,  TX_BACKSIDE_COARSE_WL_TRAINING,  LRDIMM_TX },
  { STS_CHANNEL_TRAINING,      TX_BACKSIDE_DELAY_TRAINING,  LRDIMM_TX },
  { STS_CHANNEL_TRAINING,            TRAIN_WR_VREF_LRDIMM,  WR_VREF_LRDIMM },
  { STS_CHANNEL_TRAINING,            TRAIN_RD_VREF_LRDIMM,  RD_VREF_LRDIMM },
  { STS_CHANNEL_TRAINING,          TX_LRDIMM_DQ_CENTERING,  TX_DQ_CENTERING_LRDIMM },
  { STS_CHANNEL_TRAINING,          RX_LRDIMM_DQ_CENTERING,  RX_DQ_CENTERING_LRDIMM },
#endif
  { STS_RANK_DETECT,                                    0,  CHECK_DIMM_RANKS },
  { STS_CHANNEL_TRAINING,                  IMODE_TRAINING,  TRAIN_IMODE },
  { STS_CHANNEL_TRAINING,                         T_IMODE,  TRAIN_IMODE },
  { STS_CHANNEL_TRAINING,                          T_CTLE,  TRAIN_CTLE },
  { STS_CHANNEL_TRAINING,                        T_MC_RON,  TRAIN_MC_RON },
  { STS_CHANNEL_TRAINING,                        T_RTT_WR,  TRAIN_RTT_WR },
  { STS_CHANNEL_TRAINING,                        T_MC_ODT,  TRAIN_RX_ODT },
  { STS_CHANNEL_TRAINING,                      T_DRAM_RON,  TRAIN_DRAM_RON },
  { STS_CHANNEL_TRAINING,                  TX_EQ_TRAINING,  TRAIN_TX_EQ },
  { STS_CHANNEL_TRAINING,                         T_TX_EQ,  TRAIN_TX_EQ },
  { STS_CHANNEL_TRAINING,          STS_MINOR_END_TRAINING,  LATE_CONFIG },
  { STS_MEMBIST,                                        0,  MEM_BIST },
  { STS_MEMINIT,                                        0,  MEM_INIT },
#ifdef PPR_SUPPORT
  { STS_CHANNEL_TRAINING,                        PPR_FLOW, POST_PKG_RPR },
#endif // PPR_SUPPORT
};

/**

  Sets the current memory phase

  @param host -  Pointer to sysHost
  @param phase - Current memory phase

  @retval N/A

**/
void
SetMemPhase(
           PSYSHOST  host,
           UINT32    phase
           )
{
  host->var.mem.currentMemPhase = phase;
}

/**

  Sets the current memory phase

  @param host -  Pointer to sysHost
  @param majorCheckPoint  - Major Check Point to set
  @param minorCheckPoint  - Minor Check Point to set

  @retval N/A

**/
void
SetMemPhaseCP(
             PSYSHOST  host,
             UINT32    majorCheckPoint,
             UINT32    minorCheckPoint
             )
{
  UINT8   index;

  for (index = 0; index < phaseTableSize; index++) {
    if ((phaseTable[index].majorCheckPoint == majorCheckPoint) && (phaseTable[index].minorCheckPoint == minorCheckPoint)) {
      SetMemPhase (host, phaseTable[index].phase);
      host->var.mem.noZoneActive = 0;
      break;
    }
  }
} // SetMemPhaseCP

/**

  Clears the current memory phase

  @param host -  Pointer to sysHost
  @param phase - Current memory phase to clear

  @retval N/A

**/
void
ClearMemPhase(
             PSYSHOST  host,
             UINT32    phase
             )
{

  host->var.mem.currentMemPhase = 0;

  //
  // Go to the next "no zone" if no zone is defined
  //
  if (host->var.mem.noZoneActive == 0) {
    if ((host->var.mem.noZoneIndex + 1) < MAX_NOZONE) {
      host->var.mem.noZoneIndex = host->var.mem.noZoneIndex + 1;
      if (checkMsgLevel(host, SDBG_MAX)) {
        rcPrintf ((host, "Entering no zone %d\n", host->var.mem.noZoneIndex));
      }
    }
    host->var.mem.noZoneActive = 1;
  }

}

/**

  Sets the current memory phase

  @param host -  Pointer to sysHost
  @param majorCheckPoint  - Major Check Point to clear
  @param minorCheckPoint  - Minor Check Point to clear

  @retval N/A

**/
void
ClearMemPhaseCP(
               PSYSHOST  host,
               UINT32    majorCheckPoint,
               UINT32    minorCheckPoint
               )
{
  UINT8   index;

  for (index = 0; index < phaseTableSize; index++) {
    if ((phaseTable[index].majorCheckPoint == majorCheckPoint) && (phaseTable[index].minorCheckPoint == minorCheckPoint)) {
      ClearMemPhase (host, phaseTable[index].phase);
      break;
    }
  }
} // ClearMemPhaseCP

#ifdef  DEBUG_PERFORMANCE_STATS
/**

  Compares the current memory phase

  @param host -  Pointer to sysHost
  @param phase - Current memory phase
  @retval UINT8 bit test result

**/
UINT8
IsMemPhase(PSYSHOST host, UINT32 phase )
{
  
  if (phase ==  host->var.mem.currentMemPhase )
     return TRUE;
  else 
     return FALSE;
}
#endif

/**

  Print statistics

  @param host  - Pointer to sysHost

  @retval N/A

**/
UINT32
PrintAllStats (
              PSYSHOST  host
              )
{
  UINT8 socket;
  UINT8 noZoneIndex;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  if (checkMsgLevel(host, SDBG_MINMAX)) {
    getPrintFControl (host);

    rcPrintf ((host, "Performance statistics for socket %d\n", socket));
    rcPrintf ((host, "MRC Phase         |    PCI   | Polling |JEDEC | FixedDelay | Vref  | CPCG  | Duration |\n"));
    rcPrintf ((host, "                  |   Count  |  Count  |      |    Time    | Count | Count |   Time   |\n"));
    PrintLine(host, 92, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

    rcPrintf ((host, "Total Stats       "));
    PrintStats (host, &host->var.mem.all);

    if (perfDataNonZero(host, &host->var.mem.dimmDetect)) {
      rcPrintf ((host, "DetectDimmConfig  "));
      PrintStats (host, &host->var.mem.dimmDetect);
    }

    if (perfDataNonZero(host, &host->var.mem.chkDimmRanks)) {
      rcPrintf ((host, "CheckDimmRanks    "));
      PrintStats (host, &host->var.mem.chkDimmRanks);
    }

    if (perfDataNonZero(host, &host->var.mem.gatherSPD)) {
      rcPrintf ((host, "GatherSPDData     "));
      PrintStats (host, &host->var.mem.gatherSPD);
    }

    if (perfDataNonZero(host, &host->var.mem.earlyConfig)) {
      rcPrintf ((host, "ChannelEarlyConfig"));
      PrintStats (host, &host->var.mem.earlyConfig);
    }

    if (perfDataNonZero(host, &host->var.mem.jedecInit)) {
      rcPrintf ((host, "JedecInitSequence "));
      PrintStats (host, &host->var.mem.jedecInit);
    }

    if (perfDataNonZero(host, &host->var.mem.ddrioInit)) {
      rcPrintf ((host, "InitDdrioInterface"));
      PrintStats (host, &host->var.mem.ddrioInit);
    }

    if (perfDataNonZero(host, &host->var.mem.cmdClkEarly)) {
      rcPrintf ((host, "CmdClockEarly     "));
      PrintStats (host, &host->var.mem.cmdClkEarly);
    }

    if (perfDataNonZero(host, &host->var.mem.ctlClkEarly)) {
      rcPrintf ((host, "CtlClockEarly     "));
      PrintStats (host, &host->var.mem.ctlClkEarly);
    }

    if (perfDataNonZero(host, &host->var.mem.cmdClk)) {
      rcPrintf ((host, "CmdClockTraining  "));
      PrintStats (host, &host->var.mem.cmdClk);
    }

    if (perfDataNonZero(host, &host->var.mem.xoverCalib)) {
      rcPrintf ((host, "X-Over Calib      "));
      PrintStats (host, &host->var.mem.xoverCalib);
    }

    if (perfDataNonZero(host, &host->var.mem.recEn)) {
      rcPrintf ((host, "Receive Enable    "));
      PrintStats (host, &host->var.mem.recEn);
    }

    if (perfDataNonZero(host, &host->var.mem.rdDqDqs)) {
      rcPrintf ((host, "Rd Dq/Dqs         "));
      PrintStats (host, &host->var.mem.rdDqDqs);
    }

    if (perfDataNonZero(host, &host->var.mem.swizzleDiscovery)) {
      rcPrintf ((host, "Swizzle Discovery "));
      PrintStats (host, &host->var.mem.swizzleDiscovery);
    }

    if (perfDataNonZero(host, &host->var.mem.wrLvl)) {
      rcPrintf ((host, "Write Leveling    "));
      PrintStats (host, &host->var.mem.wrLvl);
    }

    if (perfDataNonZero(host, &host->var.mem.wrDqDqs)) {
      rcPrintf ((host, "Wr Dq/Dqs         "));
      PrintStats (host, &host->var.mem.wrDqDqs);
    }

    if (perfDataNonZero(host, &host->var.mem.EarlyRdVref)) {
      rcPrintf ((host, "Early Rd Vref     "));
      PrintStats (host, &host->var.mem.EarlyRdVref);
    }

    if (perfDataNonZero(host, &host->var.mem.EarlyWrVref)) {
      rcPrintf ((host, "Early Wr Vref     "));
      PrintStats (host, &host->var.mem.EarlyWrVref);
    }

    if (perfDataNonZero(host, &host->var.mem.wrFlyBy)) {
      rcPrintf ((host, "Wr Fly-By         "));
      PrintStats (host, &host->var.mem.wrFlyBy);
    }

    if (perfDataNonZero(host, &host->var.mem.clockInit)) {
      rcPrintf ((host, "Clock Init        "));
      PrintStats (host, &host->var.mem.clockInit);
    }

    if (perfDataNonZero(host, &host->var.mem.EarlyRid)) {
      rcPrintf ((host, "Early Rd ID       "));
      PrintStats (host, &host->var.mem.EarlyRid);
    }

    if (perfDataNonZero(host, &host->var.mem.rxDqAdv)) {
      rcPrintf ((host, "Rx DqDqs Adv      "));
      PrintStats (host, &host->var.mem.rxDqAdv);
    }

    if (perfDataNonZero(host, &host->var.mem.txDqAdv)) {
      rcPrintf ((host, "Tx DqDqs Adv      "));
      PrintStats (host, &host->var.mem.txDqAdv);
    }

    if (perfDataNonZero(host, &host->var.mem.rankMarginTool)) {
      rcPrintf ((host, "Rk Margin Tool    "));
      PrintStats (host, &host->var.mem.rankMarginTool);
    }

    if (perfDataNonZero(host, &host->var.mem.normalMode)) {
      rcPrintf ((host, "Normal Mode       "));
      PrintStats (host, &host->var.mem.normalMode);
    }

    if (perfDataNonZero(host, &host->var.mem.memMapping)) {
      rcPrintf ((host, "Memory Mapping    "));
      PrintStats (host, &host->var.mem.memMapping);
    }

    if (perfDataNonZero(host, &host->var.mem.rasConfig)) {
      rcPrintf ((host, "RAS Config        "));
      PrintStats (host, &host->var.mem.rasConfig);
    }

    if (perfDataNonZero(host, &host->var.mem.eyeDiagram)) {
      rcPrintf ((host, "Eye Diagram       "));
      PrintStats (host, &host->var.mem.eyeDiagram);
    }

    if (perfDataNonZero(host, &host->var.mem.perBitDeskewRx)) {
      rcPrintf ((host, "Rx Per Bit Deskew "));
      PrintStats (host, &host->var.mem.perBitDeskewRx);
    }

    if (perfDataNonZero(host, &host->var.mem.perBitDeskewTx)) {
      rcPrintf ((host, "Tx Per Bit Deskew "));
      PrintStats (host, &host->var.mem.perBitDeskewTx);
    }

#ifdef LRDIMM_SUPPORT
    if (perfDataNonZero(host, &host->var.mem.wrVrefLrdimm)) {
      rcPrintf ((host, "Wr Vref LRDIMM    "));
      PrintStats (host, &host->var.mem.wrVrefLrdimm);
    }
#endif

    if (perfDataNonZero(host, &host->var.mem.rdVref)) {
      rcPrintf ((host, "Rd Vref           "));
      PrintStats (host, &host->var.mem.rdVref);
    }

    if (perfDataNonZero(host, &host->var.mem.wrVref)) {
      rcPrintf ((host, "Wr Vref           "));
      PrintStats (host, &host->var.mem.wrVref);
    }

    if (perfDataNonZero(host, &host->var.mem.cmdVref)) {
      rcPrintf ((host, "Cmd Vref          "));
      PrintStats (host, &host->var.mem.cmdVref);
    }

    if (perfDataNonZero(host, &host->var.mem.rtOpt)) {
      rcPrintf ((host, "Round Trip Opt    "));
      PrintStats (host, &host->var.mem.rtOpt);
    }

    if (perfDataNonZero(host, &host->var.mem.txEqTrn)) {
      rcPrintf ((host, "TX EQ             "));
      PrintStats (host, &host->var.mem.txEqTrn);
    }

    if (perfDataNonZero(host, &host->var.mem.trainImode)) {
      rcPrintf ((host, "IMODE             "));
      PrintStats (host, &host->var.mem.trainImode);
    }

    if (perfDataNonZero(host, &host->var.mem.trainRXCTLE)) {
      rcPrintf ((host, "CTLE              "));
      PrintStats (host, &host->var.mem.trainRXCTLE);
    }

    if (perfDataNonZero(host, &host->var.mem.trainMcRon)) {
      rcPrintf ((host, "MCRON             "));
      PrintStats (host, &host->var.mem.trainMcRon);
    }

    if (perfDataNonZero(host, &host->var.mem.trainRttWr)) {
      rcPrintf ((host, "RTTWR             "));
      PrintStats (host, &host->var.mem.trainRttWr);
    }

    if (perfDataNonZero(host, &host->var.mem.trainRxOdt)) {
      rcPrintf ((host, "RXODT             "));
      PrintStats (host, &host->var.mem.trainRxOdt);
    }

    if (perfDataNonZero(host, &host->var.mem.trainDramRon)) {
      rcPrintf ((host, "DRAMRON           "));
      PrintStats (host, &host->var.mem.trainDramRon);
    }

#ifdef LRDIMM_SUPPORT
    if (perfDataNonZero(host, &host->var.mem.LrdimmRx)) {
      rcPrintf ((host, "LRDIMM RX         "));
      PrintStats (host, &host->var.mem.LrdimmRx);
    }

    if (perfDataNonZero(host, &host->var.mem.LrdimmTx)) {
      rcPrintf ((host, "LRDIMM TX         "));
      PrintStats (host, &host->var.mem.LrdimmTx);
    }
#endif

    if (perfDataNonZero(host, &host->var.mem.lateConfig)) {
      rcPrintf ((host, "LateConfig        "));
      PrintStats (host, &host->var.mem.lateConfig);
    }

    if (perfDataNonZero(host, &host->var.mem.memTest)) {
      rcPrintf ((host, "MEMTEST           "));
      PrintStats (host, &host->var.mem.memTest);
    }

    if (perfDataNonZero(host, &host->var.mem.memInit)) {
      rcPrintf ((host, "MEMINIT           "));
      PrintStats (host, &host->var.mem.memInit);
    }

    for (noZoneIndex = 0; (noZoneIndex <= host->var.mem.noZoneIndex) && (noZoneIndex < MAX_NOZONE); noZoneIndex++) {
      if(perfDataNonZero(host, &host->var.mem.noZone[noZoneIndex])) {
        rcPrintf ((host, "No Zone %2d        ", noZoneIndex));
        PrintStats (host, &host->var.mem.noZone[noZoneIndex]);
      }
    }

    rcPrintf ((host, "\n"));

    releasePrintFControl (host);
  }

  // Flush the serial buffer
  SerialSendBuffer(host, 1);

  return SUCCESS;
} // PrintStats

/**

  Print statistics

  @param host  - Pointer to sysHost
  @param perfDataPtr - Pointer to current performance pointer data structure

  @retval N/A

**/
void
PrintStats (
           PSYSHOST host,
           struct perfData *perfDataPtr
           )
{
  if (checkMsgLevel(host, SDBG_MINMAX)) {

    //
    // Number of PCI accesses
    //
    rcPrintf ((host, "|  %8d", (*perfDataPtr).pciCount));

    //
    // Number of PCI access while polling
    //
    rcPrintf ((host, "| %7d ", (*perfDataPtr).pollingCount));

    //
    // Number of JEDEC inits
    //
    rcPrintf ((host, "| %5d", (*perfDataPtr).jedecCount));

    //
    // Total time in the FixedDelay function
    //
    if ((*perfDataPtr).fixedDelay > 5000) {
      rcPrintf ((host, "|  %6dms  ", (*perfDataPtr).fixedDelay / 1000));
    } else {
      rcPrintf ((host, "|  %6dus  ", (*perfDataPtr).fixedDelay));
    }

    //
    // Number of Vref movements
    //
    rcPrintf ((host, "| %6d ", (*perfDataPtr).vrefCount));

    //
    // Number of CPGC tests
    //
    rcPrintf ((host, "| %5d ", (*perfDataPtr).cpgcCount));

    //
    // Duration time
    //
    rcPrintf ((host, "| %6dms ", (*perfDataPtr).durationTime));

    //
    // End the table
    //
    rcPrintf ((host, "|\n"));

    //
    // Clear all tracking data
    //
    (*perfDataPtr).testCount        = 0;
    (*perfDataPtr).pciCount         = 0;
    (*perfDataPtr).jedecCount       = 0;
    (*perfDataPtr).fixedDelay       = 0;
    (*perfDataPtr).pollingCount     = 0;
    (*perfDataPtr).vrefCount        = 0;
    (*perfDataPtr).cpgcCount        = 0;
    (*perfDataPtr).durationTime     = 0;
  }
} // PrintStats


/**

  Check if the performance data has data

  @param host  - Pointer to sysHost
  @param perfDataPtr - Pointer to current performance pointer data structure

  @retval TRUE  -> performance data present 
          FALSE -> performance data is all clear

**/
BOOLEAN
perfDataNonZero (
           PSYSHOST host,
           struct perfData *perfDataPtr
           )
{
  if(((*perfDataPtr).testCount       == 0) &&
     ((*perfDataPtr).pciCount        == 0) &&
     ((*perfDataPtr).jedecCount      == 0) &&
     ((*perfDataPtr).fixedDelay      == 0) &&
     ((*perfDataPtr).pollingCount    == 0) &&
     ((*perfDataPtr).vrefCount       == 0) &&
     ((*perfDataPtr).cpgcCount       == 0) &&
     ((*perfDataPtr).durationTime    == 0)
     ) return FALSE;
    else return TRUE;
  
} // perfDataNonZero

/**

  Clear the perfData structures

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
ClearAllPerfData (
                 PSYSHOST host
                 )
{
  UINT8           noZoneIndex;
  struct perfData *perfDataPtr;

  perfDataPtr = &host->var.mem.all;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.dimmDetect;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.chkDimmRanks;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.gatherSPD;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.earlyConfig;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.jedecInit;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.ddrioInit;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.cmdClkEarly;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.ctlClkEarly;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.cmdClk;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.xoverCalib;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.recEn;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.rdDqDqs;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.wrLvl;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.wrDqDqs;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.wrFlyBy;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.clockInit;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.EarlyRid;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.rxDqAdv;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.txDqAdv;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.rankMarginTool;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.normalMode;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.memMapping;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.rasConfig;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.eyeDiagram;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.perBitDeskewRx;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.perBitDeskewTx;
  ClearPerfData(host, perfDataPtr);

#ifdef LRDIMM_SUPPORT
  perfDataPtr = &host->var.mem.wrVrefLrdimm;
  ClearPerfData(host, perfDataPtr);
#endif

  perfDataPtr = &host->var.mem.rdVref;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.wrVref;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.cmdVref;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.rtOpt;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.txEqTrn;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.trainImode;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.trainRXCTLE;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.trainMcRon;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.trainRttWr;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.trainRxOdt;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.trainDramRon;
  ClearPerfData(host, perfDataPtr);

#ifdef LRDIMM_SUPPORT
  perfDataPtr = &host->var.mem.LrdimmRx;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.LrdimmTx;
  ClearPerfData(host, perfDataPtr);

#endif // LRDIMM_SUPPORT

  perfDataPtr = &host->var.mem.lateConfig;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.memTest;
  ClearPerfData(host, perfDataPtr);

  perfDataPtr = &host->var.mem.memInit;
  ClearPerfData(host, perfDataPtr);

  for (noZoneIndex = 0; noZoneIndex < MAX_NOZONE; noZoneIndex++) {
    perfDataPtr = &host->var.mem.noZone[noZoneIndex];
    ClearPerfData(host, perfDataPtr);
  }

} // ClearAllPerfData

/**

  Clear perfData structure

  @param host        - Pointer to sysHost
  @param perfDataPtr - Pointer to the structure to clear

  @retval N/A

**/
void
ClearPerfData (
              PSYSHOST host,
              struct perfData *perfDataPtr
              )
{
  (*perfDataPtr).testCount     = 0;
  (*perfDataPtr).pciCount      = 0;
  (*perfDataPtr).jedecCount    = 0;
  (*perfDataPtr).fixedDelay    = 0;
  (*perfDataPtr).pollingCount  = 0;
  (*perfDataPtr).vrefCount     = 0;
  (*perfDataPtr).cpgcCount     = 0;
  (*perfDataPtr).durationTime  = 0;
} // ClearPerfData

#endif   // SERIAL_DBG_MSG
#endif   // DEBUG_PERFORMANCE_STATS


