/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
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
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "Cpgc.h"

//------------------------------------------------------------------------------------------
// Local defines
GSM_CSN signalsToSweep[NUM_SIGNALS_TO_SWEEP] = CHIP_GSM_CSN_SIGNALSTOSWEEP;

/*
 * Test code
INT16   le0[NUM_SIGNALS_TO_SWEEP]             = {-53, -57, -57, -59, -51, -53, -47, -46, -63, -64, -43, -40, -52, -49, -58, -60, -47, -43, -44, -42, -59, -61, -64, -62};
INT16   re0[NUM_SIGNALS_TO_SWEEP]             = { 40,  36,  36,  34,  42,  40,  46,  47,  30,  29,  50,  53,  41,  44,  35,  33,  46,  50,  49,  51,  34,  32,  29,  31};
INT16   le1[NUM_SIGNALS_TO_SWEEP]             = {-30, -37, -40, -39, -26, -33, -29, -26, -44, -44, -22, -20, -32, -29, -34, -40, -24, -23, -18, -22, -40, -41, -48, -42};
INT16   re1[NUM_SIGNALS_TO_SWEEP]             = { 63,  56,  53,  54,  67,  60,  64,  67,  49,  49,  71,  73,  61,  64,  59,  53,  69,  70,  75,  71,  53,  52,  45,  51};
 */

//------------------------------------------------------------------------------------------
#define PER_GROUP         0
#define COMPOSITE_GROUPS  1
#define NO_SIGNAL         0xff

//
// Local Prototypes
//
UINT32 CollectParityResults(PSYSHOST host, UINT8 socket, UINT32 chToCheck, UINT8 dimm, UINT32 bwSerr[MAX_CH][3]);
void   ClearParityResults(PSYSHOST host, UINT8 socket, UINT32 chToCheck, UINT8 dimm);
void   ClearParityResultsAlt(PSYSHOST host, UINT8 socket, UINT32 chToCheck, UINT8 rank, UINT8 dimm);
void   ClearBwSerr(UINT32 bwSerr[MAX_CH][3]);
void   OffsetGroups(PSYSHOST host, UINT8 socket, UINT8 ch, GSM_GT group, INT16 offset);
void   ExecuteCtlClkTest(PSYSHOST host, UINT8 socket, UINT32 chBitmask, UINT8 dimm, UINT8 rank, UINT8 testFlags);
void   GetCtlClkResults(PSYSHOST host, UINT8 socket, UINT8 dimm, UINT8 rank, UINT32 strobeFeedback[MAX_CH]);
void   GetCtlMarginsSweep(PSYSHOST host, UINT8 socket, UINT8 dimm, UINT8 rank, GSM_CSN sig[MAX_CH], INT16 *le, INT16 *re);
void   GetChipBwSerrErrors(UINT32 errMask, UINT8 ch, UINT8 errorResult[MAX_CH][CHIP_CMD_CLK_TEST_RANGE], INT16 piIndex, INT16 offset, PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct);

UINT32
LateCmdClk(
  PSYSHOST  host
  )
/*++

  Exectues command/clock training

  @param host      - Pointer to sysHost

  @retval SUCCESS

--*/
{
  return CHIP_FUNC_CALL(host, ChipLateCmdClk(host));
}

UINT32
AggressiveCMDTest(
  PSYSHOST  host,
  UINT8     socket
  )

/**

  Aggressive CMD test to get a per platform group centering of CMD

  @param host - Pointer to sysHost

  @retval N/A

**/
{
  return CHIP_FUNC_CALL(host, ChipAggressiveCMDTest(host,socket));
}


UINT32
PerCMDGroup(
  PSYSHOST  host,
  UINT8     socket,
  UINT16    testCase
  )
{
  UINT8                               ch;
  UINT8                               dimm;
  UINT8                               rank;
  //UINT8                               vIndex;
  UINT16                              mode;
  GSM_GT                              group;
  GSM_GT                              groupStart;
  GSM_GT                              groupStop;
  UINT32                              status = SUCCESS;
  UINT32                              patternLength = 1;
#ifdef SERIAL_DBG_MSG
  char                                strBuf0[128];
#endif  // SERIAL_DBG_MSG
  //struct rankMargin                   resultsRank[3];
  struct rankMargin                   resultsRank;
  struct bitMask                      filter;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
  struct ddrRank                      (*rankList)[MAX_RANK_DIMM];
  GSM_CGEDGE                          groupEdgeData[MAX_CH][MAX_RANK_CH][MAX_CMD];

  // Skip socket if not enabled
  if (host->nvram.mem.socket[socket].enabled == 0) return status;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Enable normal CMD timing
  //
  SetNormalCMDTiming(host, socket);

/*
  //
  // Code to test the SetCombinedCmdGroup function
  //
  ch = 0;
        //
        // Load the eye data
        //
        for (signal = 0; signal < NUM_SIGNALS_TO_SWEEP; signal++) {
          signalEdgeData[0][signal].signal = signalsToSweep[sig];
          signalEdgeData[0][signal].le = le0[signal];
          signalEdgeData[0][signal].re = re0[signal];
          signalEdgeData[4][signal].signal = signalsToSweep[signal];
          signalEdgeData[4][signal].le = le1[signal];
          signalEdgeData[4][signal].re = re1[signal];
        } // signal loop
  //
  // Call the combine function
  //
  status = (UINT32)CHIP_FUNC_CALL(host, SetCombinedCmdGroup(host, socket, ch, DdrLevel, GSM_COMBINE_EDGES, CMD_SIGNAL_DELAY, NUM_SIGNALS_T0_SWEEP, signalEdgeData));
 */

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MEM_TRAIN)) {
    getPrintFControl (host);
  }
#endif // SERIAL_DBG_MSG
  if(testCase == COMPOSITE_GROUPS) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "START_CMD_CLK_PER_GROUP_FINAL\n"));
    //numPoints = 3;
    //stepSize = 12;
    mode = MODE_VA_DESELECT;

  } else {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "START_CMD_CLK_PER_GROUP\n"));
    //numPoints = 1;
    //stepSize = 1;
    mode = MODE_DESELECT;
  }

  patternLength = 32;

  //
  // Clear the filter bits to enable error checking on every bit
  //
  ClearFilter(host, socket, &filter);

  CHIP_FUNC_CALL(host, ChipEnableCheckParity(host, &mode));

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MEM_TRAIN)) {
    releasePrintFControl (host);
  }
#endif // SERIAL_DBG_MSG

  CHIP_FUNC_CALL(host, IO_Reset (host, socket));

  if(testCase == COMPOSITE_GROUPS) {
    groupStart = CmdAll;
    groupStop = CmdAll;
  } else {
    groupStart = CmdGrp0;
    groupStop = CHIP_CMD_GRP_STOP;
  }

  for (group = groupStart; group <= groupStop; group++) {
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Calling GetMargins for %s\n", CHIP_FUNC_CALL(host, GetPlatformGroupStr(host, group, (char *)&strBuf0))));
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        rankList = GetRankNvList(host, socket, ch, dimm);
        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          //for (vIndex = 0; vIndex < 3; vIndex++) {
          //  resultsRank[vIndex].rank[ch][(*rankList)[rank].rankIndex].n = 0;
          //  resultsRank[vIndex].rank[ch][(*rankList)[rank].rankIndex].p = 0;
          //}
          //for (vIndex = 0; vIndex < 3; vIndex++) {
            resultsRank.rank[ch][(*rankList)[rank].rankIndex].n = 0;
            resultsRank.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          //}
        } // rank loop
      } // dimm loop
    } // ch loop

    //
    // Get CMD margins for current rank and group
    //
    //if(mode == MODE_DESELECT) {
      GetMargins (host, socket, DdrLevel, group, mode, SCOPE_RANK, &filter, (void *)&resultsRank, patternLength, 0, 0, 0,
                  WDB_BURST_LENGTH);
    //} else {
    //  patternLength = 32, stepsize = 12, numsteps = 3
    //  GetMultiVref (host, socket, DdrLevel, group, mode, SCOPE_RANK, &filter, (void *)resultsRank, patternLength, 0, 0, stepSize, numPoints);
    //}

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

#if 0
          //
          // Get the total eye width
          //
          eyeWidth = resultsRank[vIndexCenter].rank[ch][(*rankList)[rank].rankIndex].p -
                     resultsRank[vIndexCenter].rank[ch][(*rankList)[rank].rankIndex].n;
          //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
          //              "EW = R0 + L0 = %d\n", eyeWidth));
          offset = eyeHeight * ((2 * resultsRank[vIndexCenter].rank[ch][(*rankList)[rank].rankIndex].p) +
                                (2 * resultsRank[vIndexCenter].rank[ch][(*rankList)[rank].rankIndex].n));
          //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
          //              "Center = EH * (2*R0 + 2*L0) = %d\n", offset));
          offset += ((eyeWidth / 3) + ((2 * eyeHeight) / 3)) *
                    (resultsRank[vIndexCenter + 1].rank[ch][(*rankList)[rank].rankIndex].p +
                     resultsRank[vIndexCenter - 1].rank[ch][(*rankList)[rank].rankIndex].p +
                     resultsRank[vIndexCenter + 1].rank[ch][(*rankList)[rank].rankIndex].n +
                     resultsRank[vIndexCenter - 1].rank[ch][(*rankList)[rank].rankIndex].n);
          //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
          //              "Center += (1/3EW + 2/3EH) * (R+1 + R-1 + L+1 + L-1) = %d\n", offset));
          /*
          offset += (((2 * eyeWidth) / 3) + (eyeHeight / 3)) *
                    (results[vIndexCenter + 2].strobe[ch][(*rankList)[rank].rankIndex][strobe].p +
                     results[vIndexCenter - 2].strobe[ch][(*rankList)[rank].rankIndex][strobe].p +
                     results[vIndexCenter + 2].strobe[ch][(*rankList)[rank].rankIndex][strobe].n +
                     results[vIndexCenter - 2].strobe[ch][(*rankList)[rank].rankIndex][strobe].n);
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                        "Center += (2/3EW + 1/3EH) * (R+2 + R-2 + L+2 + L-2) = %d\n", offset));
          offset += eyeWidth *
                    (results[vIndexCenter + 3].strobe[ch][(*rankList)[rank].rankIndex][strobe].p +
                     results[vIndexCenter - 3].strobe[ch][(*rankList)[rank].rankIndex][strobe].p +
                     results[vIndexCenter + 3].strobe[ch][(*rankList)[rank].rankIndex][strobe].n +
                     results[vIndexCenter - 3].strobe[ch][(*rankList)[rank].rankIndex][strobe].n);
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                        "Center += EW * (R+3 + R-3 + L+3 + L-3) = %d\n", offset));
           */
          offset = offset / (((4 * eyeWidth) + (4 * eyeHeight)) / 2);
          //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
          //              "Center = Center / (4*EW + 4*EH) / 2 = %d\n", offset));
#endif
          //
          // Convert results from absolute pi values to offset values and save the results
          //
          if(testCase == COMPOSITE_GROUPS) {
            groupEdgeData[ch][(*rankList)[rank].rankIndex][0].group = CmdAll;
            groupEdgeData[ch][(*rankList)[rank].rankIndex][0].le = resultsRank.rank[ch][(*rankList)[rank].rankIndex].n;
            groupEdgeData[ch][(*rankList)[rank].rankIndex][0].re = resultsRank.rank[ch][(*rankList)[rank].rankIndex].p;
          } else {
            groupEdgeData[ch][(*rankList)[rank].rankIndex][group - CmdGrp0].group = group;
            groupEdgeData[ch][(*rankList)[rank].rankIndex][group - CmdGrp0].le = resultsRank.rank[ch][(*rankList)[rank].rankIndex].n;
            groupEdgeData[ch][(*rankList)[rank].rankIndex][group - CmdGrp0].re = resultsRank.rank[ch][(*rankList)[rank].rankIndex].p;
          }

        } // rank loop
      } // dimm loop
    } // ch loop
  } // group loop

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n"));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Evaluate results and write them
    //
    if(testCase == COMPOSITE_GROUPS) {
      status = (UINT32)CHIP_FUNC_CALL(host, SetCombinedCmdGroup(host, socket, ch, DdrLevel, GSM_COMBINE_EDGES, CMD_GRP_DELAY, 1,
                                           groupEdgeData[ch]));
    } else {
      status = (UINT32)CHIP_FUNC_CALL(host, SetCombinedCmdGroup(host, socket, ch, DdrLevel, GSM_COMBINE_EDGES, CMD_GRP_DELAY, MAX_CMD,
                                           groupEdgeData[ch]));
    }
  } // ch loop

  if(testCase == COMPOSITE_GROUPS) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "STOP_CMD_CLK_PER_GROUP_FINAL\n"));
  } else {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "STOP_CMD_CLK_PER_GROUP\n"));
  }

  // Execute JEDEC reset/init sequence
  disableAllMsg (host);
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  JedecInitSequence (host, socket, CH_BITMASK);
  restoreMsg (host);

  return SUCCESS;
} // PerCMDGroup


/**

  Exectues early command/clock training

  @param host - Pointer to sysHost

  @retval N/A

**/
UINT32
EarlyCmdClk(
  PSYSHOST  host
  )
{
  UINT8                               socket;
  UINT8                               signal;
  GSM_CSN                             signaldecode;
  UINT8                               ch = 0;
  UINT8                               dimm = 0;
  UINT8                               rank;
  UINT8                               rankEnabled;
  struct channelNvram                 (*channelNvList)[MAX_CH];
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
  struct ddrRank                      (*rankList)[MAX_RANK_DIMM];
  GSM_CSEDGE                          signalEdgeData[MAX_CH][MAX_RANK_CH][NUM_SIGNALS_TO_SWEEP];
  CHIP_ERLY_CMD_CK_STRUCT             chipErlyCmdCKStruct;

  MemSetLocal ((UINT8 *)&chipErlyCmdCKStruct, 0, sizeof (chipErlyCmdCKStruct));

  socket = host->var.mem.currentSocket;

  host->var.mem.checkMappedOutRanks = 1;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, EARLY_CMD_CLK, socket));
  channelNvList = GetChannelNvList(host, socket);


  //
  // Make sure all populated DIMMs support parity properly
  //
  if(CHIP_FUNC_CALL(host, ShouldChipSkipElyCMDTrAsPrtyNotSprtd(host, socket, ch, dimm))){
    return SUCCESS;
  }

#ifdef SERIAL_DBG_MSG
  getPrintFControl (host);
#endif // SERIAL_DBG_MSG
  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "START_PARITY_CMD_CLK\n"));

  // Chip Workaround
  CHIP_FUNC_CALL(host, OffsetCCC(host, socket, 64, 1));

  //
  // Enable normal CMD timing
  //
  SetNormalCMDTiming(host, socket);
  CHIP_FUNC_CALL(host, ToggleBcomFlopEn(host, socket, 1));
  CHIP_FUNC_CALL(host, ToggleBcom(host, socket, 0));
  CHIP_FUNC_CALL(host, ChipEarlyCmdCKSvHook(host, socket, &chipErlyCmdCKStruct));
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Turn on command/address output enable
    //
    CHIP_FUNC_CALL(host, TrnOnChipCMDADDOpEn(host, socket, ch, &chipErlyCmdCKStruct));

    // Disable 3DS mode
    if ((*channelNvList)[ch].encodedCSMode == 2) {
      CHIP_FUNC_CALL(host, Toggle3DSMode(host, socket, ch, 0));
      //Tracks 3DS mode to skip some CSR
      host->nvram.mem.threeDsModeDisabled = 1;
    }
  } // ch loop

  CHIP_FUNC_CALL(host, IO_Reset (host, socket));

  //
  // Enable parity checking at the DIMM
  //
  EnableParityChecking(host, socket);

  //
  // Configure NVMCTLR to enable Early CMD training mode if necessary
  //
  CHIP_FUNC_CALL(host, EnableChipFNV(host, socket, ch, &chipErlyCmdCKStruct));

  //
  // Loop to train PAR, ACT_N then the address signals
  //
  for (signal = 0; signal < NUM_SIGNALS_TO_SWEEP; signal++) {

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      signaldecode = signalsToSweep[signal];
       // for NVM DIMMs, convert the CID0/1 to S2/S3
      if (signaldecode == C0){
        if ((dimm == 0) || (dimm == 2)) {
          signaldecode = CS2_N; //dimm0 CID0
        } else {
          signaldecode = CS6_N; //dimm1 CID0
        }
      } else if (signaldecode == C1){
        if ((dimm == 0) || (dimm == 2)) {
          signaldecode = CS3_N; //dimm0 CID1
        } else {
          signaldecode = CS7_N; //dimm1 CID1
        }
      }

      for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

        rankEnabled = 0;

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          dimmNvList = GetDimmNvList(host, socket, ch);
          if (rank >= (*dimmNvList)[dimm].numRanks) continue;

          rankEnabled = 1;

          break;
        } // ch loop

        //
        // Continue to the next rank if this one is not populated
        //
        if (rankEnabled == 0) continue;

        //
        // Get CMD margins for current rank and signal
        //
        if ((host->nvram.mem.dimmTypePresent != UDIMM) && (host->nvram.mem.dimmTypePresent != SODIMM) && (rank > 0)) {
          for (ch = 0; ch < MAX_CH; ch++) {
            if ((*channelNvList)[ch].enabled == 0) continue;
            rankList = GetRankNvList(host, socket, ch, dimm);
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            dimmNvList = GetDimmNvList(host, socket, ch);
            if (rank >= (*dimmNvList)[dimm].numRanks) continue;
            chipErlyCmdCKStruct.le[ch] = signalEdgeData[ch][(*rankList)[0].rankIndex][signal].le;
            chipErlyCmdCKStruct.re[ch] = signalEdgeData[ch][(*rankList)[0].rankIndex][signal].re;
          } //ch loop
        } else {
          GetCmdMarginsSweep(host, socket, dimm, rank, signaldecode, chipErlyCmdCKStruct.le, chipErlyCmdCKStruct.re);
        }

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          dimmNvList = GetDimmNvList(host, socket, ch);
          if (rank >= (*dimmNvList)[dimm].numRanks) continue;

          //
          // Convert results from absolute pi values to offset values and save the results
          //
          signalEdgeData[ch][(*rankList)[rank].rankIndex][signal].signal = signaldecode;
          signalEdgeData[ch][(*rankList)[rank].rankIndex][signal].le = chipErlyCmdCKStruct.le[ch];
          signalEdgeData[ch][(*rankList)[rank].rankIndex][signal].re = chipErlyCmdCKStruct.re[ch];

        } // ch loop

      } // rank loop
    } // dimm loop

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n"));
  } // signal loop

  //
  // Configure NVMCTLR to disable Early CMD training mode if necessary
  //
#ifdef GENERIC_FNV_TRAINING_MODE
  JedecInitSequence (host, socket, CH_BITMASK);
  CHIP_FUNC_CALL(host, SetAepTrainingMode (host,socket, DISABLE_TRAINING_STEP));
#else
  CHIP_FUNC_CALL(host, SetAepTrainingModeSMB (host,socket, DISABLE_TRAINING_STEP));
#endif

  DisableParityChecking(host, socket);

  //
  // clear to insure we take the right path for setting final PI values
  //
  host->var.mem.earlyCmdClkExecuted = 0;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Evaluate results and write them
    //
    CHIP_FUNC_CALL(host, SetCombinedCmdGroup(host, socket, ch, DdrLevel, GSM_COMBINE_EDGES, CMDCTL_SIGNAL_DELAY, NUM_SIGNALS_TO_SWEEP,
                                         signalEdgeData[ch]));

    //
    // Turn off command/address output enable
    //
    CHIP_FUNC_CALL(host, TrnOffChipCMDADDOpEn(host, socket, ch, &chipErlyCmdCKStruct));

    // Restore 3DS mode
    if ((*channelNvList)[ch].encodedCSMode == 2) {
      CHIP_FUNC_CALL(host, Toggle3DSMode(host, socket, ch, 1));
      host->nvram.mem.threeDsModeDisabled = 0;
    }
  } // ch loop

  //
  // Set to indicate early CMD/CLK training has execute
  //
  host->var.mem.earlyCmdClkExecuted = 1;

  // Chip WorkAround
  CHIP_FUNC_CALL(host, OffsetCCC(host, socket, -64, 0));


  // Execute JEDEC reset/init sequence
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  disableAllMsg (host);
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  CHIP_FUNC_CALL(host, ToggleBcom(host, socket, 1));
  CHIP_FUNC_CALL(host, ToggleBcomFlopEn(host, socket, 0));
  JedecInitSequence (host, socket, CH_BITMASK);
  restoreMsg (host);

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    CHIP_FUNC_CALL(host, DisplayCCCResults(host, socket));
  }
  releasePrintFControl (host);
#endif // SERIAL_DBG_MSG

  host->var.mem.checkMappedOutRanks = 0;

  return SUCCESS;
} // EarlyCmdClk


//------------------------------------------------------------------------------------------
struct cmdEye {
   INT16 left;
   INT16 right;
};


/**

  Finds if a give CID or CS is trained

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param dimm        - DIMM number
  @param signal      - Signal

  @retval N/A

**/
UINT32
isCmdCtlTrained(
            PSYSHOST  host,
            UINT8     socket,
            UINT8     ch,
            UINT8     dimm,
            GSM_CSN   signal
)
{
  UINT8           status;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);

  status = EARLY_CMD_CLK;

  // CTL signals use earlyCTLCLK for RDIMM or NVMDIMM
  if ((host->nvram.mem.dimmTypePresent == RDIMM) || (*dimmNvList)[dimm].aepDimmPresent){
    switch(signal) {
      case CKE0:
      case CKE1:
      case CKE2:
      case CKE3:
      case ODT0:
      case ODT1:
      case ODT2:
      case ODT3:
      case CS0_N:
      case CS1_N:
      case CS4_N:
      case CS5_N:
        status = EARLY_CTL_CLK;
        break;
      default:
        break;
    }
  }

  //NVMDIMM use earlyCmdClk
  if ((*dimmNvList)[dimm].aepDimmPresent) {
    switch(signal) {
      case CS2_N:
      case CS3_N:
      case CS6_N:
      case CS7_N:
      case C0:
      case C1:
      case C2:
        status = EARLY_CMD_CLK;
        break;
      default:
        break;
    }
  }

  // Standard LRDIMM trains C0 with earlyCtlClk, NOT NVMDIMM or 3DS
  if (((CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))|| ((*dimmNvList)[dimm].numRanks == 4)) && ((*dimmNvList)[dimm].aepDimmPresent == 0) && (*channelNvList)[ch].encodedCSMode != 2)){
    switch(signal) {
      case C0:
      case C1:
      case CS2_N:
      case CS3_N:
        if ((*dimmNvList)[dimm].numRanks == 2){
          status = 0; // don't train DR LRDIMM
        } else {
          status = EARLY_CTL_CLK;
        }
        break;
      case CS6_N:
      case CS7_N:
        status = EARLY_CTL_CLK;
        break;
      case C2:
        status = 0;
        break;
      default:
        break;
    }
  }

  // 3DS LRDIMM or RDIMM, carefull this a channel level so we could have 3DS with NVMDIMM
  if (((*channelNvList)[ch].encodedCSMode == 2) && ((*dimmNvList)[dimm].aepDimmPresent == 0)) {
    //always train these for 3DS

    if (((signal == C1) || (signal == CS3_N) || (signal == CS7_N)) && (((*channelNvList)[ch].cidBitMap & BIT1) == 0)){
      status = 0; // Do not train it
    } // else it will

    if ((signal == C2) && (((*channelNvList)[ch].cidBitMap & BIT2) == 0)){
      status = 0; // Do not train it
    }
  }

  // SKIP CID's or upper CS#  if RDIMM / UDIMM (NOT LRDIMM and NOT NVMDIMM and NOT 3DS)
  if ((CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))== 0) && ((*dimmNvList)[dimm].aepDimmPresent == 0) && ((*channelNvList)[ch].encodedCSMode != 2) &&
    ((*dimmNvList)[dimm].numRanks < 4)) {
    switch(signal) {
      case C0:
      case C1:
      case C2:
      case CS2_N:
      case CS3_N:
      case CS6_N:
      case CS7_N:
        status = 0;
        break;
      default:
        break;
    }
  }
  return status;
}


/**

  Finds command margins for the given signal

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param dimm        - DIMM number
  @param rank        - rank number
  @param signal      - Signal
  @param *le         - Left edge
  @param *re         - Right edge

  @retval N/A

**/
void
GetCmdMarginsSweep(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm,
  UINT8     rank,
  GSM_CSN   signal,
  INT16     *le,
  INT16     *re
  )
{
  UINT8               ch;
  UINT8               executeTest;
  INT16               piIndex;
  INT16               offset;
  INT16               orgDelay[MAX_CH];
  UINT32              rankMask[MAX_CH];
  UINT8               errorResult[MAX_CH][PAR_TEST_RANGE];
  UINT16              piPosition[MAX_CH];
  UINT32               chBitmask = 0;
  UINT32              bwSerr[MAX_CH][3];
  UINT32              status = 0;
#ifdef SERIAL_DBG_MSG
  char                strBuf0[128];
#endif // SERIAL_DBG_MSG
  struct cmdEye       curEye[MAX_CH];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  CHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct;

  CHIP_FUNC_CALL(host, GetCmdMarginsSweepInit(&chipGetCmdMgnSwpStruct));

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    rankMask[ch] = 0;
    orgDelay[ch] = 0;

    if ((*channelNvList)[ch].enabled == 0) continue;

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);
    if (rank >= (*dimmNvList)[dimm].numRanks) continue;

    // need to initialize these regardless if signal is trained.
    //curEye[ch].left = -UNMARGINED_CMD_EDGE;
    //curEye[ch].right = UNMARGINED_CMD_EDGE;
    re[ch] = UNMARGINED_CMD_EDGE; // UNMARGINED_CMD_EDGE = 511
    le[ch] = -UNMARGINED_CMD_EDGE;

    // Skip CIDS if not NVMDIMM
    if (isCmdCtlTrained(host, socket, ch, dimm, signal) != (UINT32)EARLY_CMD_CLK ) continue;

    // Initialize ch rank mask
    rankMask[ch] = 1;

    //
    // Get the original delay
    //
    CHIP_FUNC_CALL(host, GetSetSignal (host, socket, ch, DdrLevel, signal, GSM_READ_ONLY, &orgDelay[ch]));

    for (piIndex = 0; piIndex < PAR_TEST_RANGE; piIndex++) {
      errorResult[ch][piIndex] = 1;
    } //piIndex loop

    //
    // Setup for parity test
    //
    CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host,GetPhyRank(dimm, rank)), 0, signal, NORMAL_CADB_SETUP));

    chipGetCmdMgnSwpStruct.eyeSize[ch] = 0;
    chipGetCmdMgnSwpStruct.chEyemask |= 1 << ch;
    chipGetCmdMgnSwpStruct.chDonemask |= 1 << ch;

    // Indicate rank present and enabled on this ch
    chBitmask |= 1 << ch;
  } // ch loop

  offset = EARLY_CMD_STEP_SIZE;

  //
  // Sweep Pi delays
  //  The following code assumes PAR_TEST_START is always <= the absolute minimum CMD setting
  //  and PAR_TEST_RANGE + PAR_TEST_START is always < the maximum CMD setting
  //
  for (piIndex = PAR_TEST_START; piIndex < (PAR_TEST_RANGE + PAR_TEST_START); piIndex += offset) {

    if (chBitmask == 0) continue;

    // Shift timings on each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((rankMask[ch] == 0)) continue;

      CHIP_FUNC_CALL(host, GetSetSignal (host, socket, ch, DdrLevel, signal, GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &piIndex));
      //
      //Call RestoreEarlyCmdPatterns or CpgAdvCmdParity depending on chip and depending on if the flow is RC_SIM or not
      //
       CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(dimm, rank)), 0, signal, RESTORE_CADB_PATTERN));
    } // ch loop

    //
    // Generate an IO Reset. This is required because the pi delays changed.
    //
    CHIP_FUNC_CALL(host, IO_Reset (host, socket));

    //
    // Run concurrent tests
    //
    CHIP_FUNC_CALL(host, EarlyCmdSignalTest (host, socket, chBitmask));

    ClearBwSerr(bwSerr);

    //
    // Check for errors
    //
    status = CollectParityResults (host, socket, chBitmask, dimm, bwSerr);
    status |= CHIP_FUNC_CALL(host, EarlyCmdSignalStop   (host, socket, chBitmask, dimm, bwSerr));

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((rankMask[ch] == 0)) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      if (rank >= (*dimmNvList)[dimm].numRanks) continue;

      if (bwSerr[ch][0] || bwSerr[ch][1] || bwSerr[ch][2]) {
        errorResult[ch][piIndex] = 1;
      } else {
        errorResult[ch][piIndex] = 0;
      }


      CHIP_FUNC_CALL(host, UpdateCmdMarginMasks(host, socket, ch, piIndex, offset, errorResult, &chipGetCmdMgnSwpStruct));
    } // ch loop

    //
    // Clear errors if a parity error occurred
    //
    if (CHIP_FUNC_CALL(host, ShouldChipClearCMDMGNParityError(host, status))) {

      //
      // Restore initial setting
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((rankMask[ch] == 0)) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        if (rank >= (*dimmNvList)[dimm].numRanks) continue;

        CHIP_FUNC_CALL(host, GetSetSignal (host, socket, ch, DdrLevel, signal, GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &orgDelay[ch]));
      } // ch loop

      //
      // Generate an IO Reset. This is required because the pi delays changed.
      //
      CHIP_FUNC_CALL(host, IO_Reset (host, socket));

      //
      // Clear parity error status in the iMC and at the DIMM
      //
      disableAllMsg (host);
      ClearParityResultsAlt(host, socket, chBitmask, rank, dimm);
      restoreMsg (host);

      //
      // Restore current offset
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((rankMask[ch] == 0)) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        if (rank >= (*dimmNvList)[dimm].numRanks) continue;

        if (signal == PAR) {
          //
          // Setup the test
          //
          CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(dimm, rank)), 0, signal, NORMAL_CADB_SETUP));
        }
      } // ch loop
    } // if status

    //
    // Issue a precharge unless testing PAR
    //
    executeTest = 0;
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((rankMask[ch] == 0)) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      if (rank >= (*dimmNvList)[dimm].numRanks) continue;

      //
      // No need to do anything if NVMDIMM is not present and this is a parity signal
      //
      if(((*channelNvList)[ch].ddrtEnabled == 0) && (signal == PAR)) continue;

      if ((*dimmNvList)[dimm].aepDimmPresent == 0) {
        if (signal == PAR) {
          //
          // Do nothing for PAR Signal
          //
          CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(dimm, rank)), 0, DESELECT, NORMAL_CADB_SETUP));
        } else {
          //
          // Setup the test for a precharge
          //
          CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(dimm, rank)), 0, PRECHARGE, NORMAL_CADB_SETUP));
          executeTest = 1;
        }
      } else {
        if (status) {
          //
          //NVMCTLR with Good Parity
          //
          CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(dimm, rank)), 0, FNV_GOOD_PARITY, NORMAL_CADB_SETUP));
          executeTest = 1;
        } else {
          //
          //Deselect Pattern
          //
          CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(dimm, rank)), 0, DESELECT, NORMAL_CADB_SETUP));
        }
      }
    } // ch loop

    // Issue precharge
    if(executeTest) ExecuteCmdSignalTest (host, socket, chBitmask);

    if (CHIP_FUNC_CALL(host, CheckChDoneMask(host, &chipGetCmdMgnSwpStruct)) == 0) {
      piIndex = PAR_TEST_RANGE;
    }
  } // piIndex loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((rankMask[ch] == 0)) continue;
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "%5s -> ", CHIP_FUNC_CALL(host, GetSignalStr(signal, (char *)&strBuf0))));
    for (piIndex = 0; piIndex < PAR_TEST_RANGE; piIndex++) {

      dimmNvList = GetDimmNvList(host, socket, ch);
      if (rank >= (*dimmNvList)[dimm].numRanks) continue;

      if (errorResult[ch][piIndex]) {
        // Fail
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "*"));
      } else {
        // Pass
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "0"));
      }
    } // piIndex loop

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n"));
  } // ch loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((rankMask[ch] == 0)) continue;

    status = FindErrorGraphMin (host, socket, ch, dimm, rank, 0, &errorResult[ch][0], PAR_TEST_RANGE,
                                (UINT16 *)&curEye[ch].right, (UINT16 *)&curEye[ch].left, (UINT16 *)&piPosition[ch]);

    if ((status == FAILURE) || (curEye[ch].left == curEye[ch].right)){
      curEye[ch].left = 0;
      curEye[ch].right = 0;
    }


    //
    // Offset results relative to the starting point of the sweep
    //
    curEye[ch].right = curEye[ch].right - orgDelay[ch];
    curEye[ch].left = curEye[ch].left - orgDelay[ch];

    //
    // Convert edges to be CLK based
    //
    //curEye[ch].right = 0 - curEye[ch].right;
    //curEye[ch].left = 0 - curEye[ch].left;

  } // ch loop

  //
  // Restore initial pi delay
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((rankMask[ch] == 0)) continue;

    // Update return parameters
    re[ch] = curEye[ch].right;
    le[ch] = curEye[ch].left;

    if ((re[ch] - le[ch]) <= 0) {
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                    "ERROR: Signal %s, No eye found.\n", CHIP_FUNC_CALL(host, GetSignalStr(signal, (char *)&strBuf0))));
      le[ch] = 0;
      re[ch] = 0;
    }
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "le = %d - re = %d\n", le[ch], re[ch]));

    //
    // Restore signal under test to its initial setting
    //
    CHIP_FUNC_CALL(host, GetSetSignal (host, socket, ch, DdrLevel, signal, GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &orgDelay[ch]));
  } // ch loop

} // GetCmdMarginsSweep

/**

  Finds command margins for the given command group

  @param host        - Pointer to sysHost
  @param socket        - Socket number

  @retval N/A

**/
void
ExecuteCmdSignalTest(
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chBitmask
  )
{

    //
    // Start test
    //
    CHIP_FUNC_CALL(host, CpgcGlobalTestStart (host, socket, chBitmask));

    //
    // Poll test completion
    //
    CHIP_FUNC_CALL(host, CpgcPollGlobalTestDone (host, socket, chBitmask));

} // ExecuteCmdSignalTest

/**

  Finds command margins for the given group

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param dimm        - DIMM number
  @param rank        - rank number
  @param group       - group to sweep
  @param *le         - Left edge
  @param *re         - Right edge

  @retval N/A

**/
void
OffsetGroups(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  GSM_GT    group,
  INT16     offset
  )
{
  UINT16 minVal;
  UINT16 maxVal;

  //
  // Check if we are moving all CMD and CTL groups
  //
  if (group == CmdCtlAll) {
    minVal = MAX_CMD_MARGIN;
    maxVal = 0;

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &offset, &minVal, &maxVal));

    minVal = MAX_CMD_MARGIN;
    maxVal = 0;

    CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, CtlAll, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &offset, &minVal, &maxVal));
  }

  //
  // Check if we are moving individual CMD groups
  //
  if (CheckCMDGroup(group)) {
    minVal = MAX_CMD_MARGIN;
    maxVal = 0;

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, group, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &offset, &minVal, &maxVal));
  }

  //
  // Check if we are moving individual CTL groups
  //
  if (CheckCTLGroup(group)) {
    minVal = MAX_CMD_MARGIN;
    maxVal = 0;

    CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, group, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &offset, &minVal, &maxVal));
  }
} // OffsetGroups

//------------------------------------------------------------------------------------------
/**

  Updates the composite edges

  @param le      - Left edge
  @param re      - Right edge
  @param *compLe - Composite left edge
  @param *compRe - Composite right edge

  @retval N/A

**/
void
UpdateEdges(
  INT16 le,
  INT16 re,
  INT16 *compLe,
  INT16 *compRe
  )
{
  if (le > *compLe) *compLe = le;
  if (re < *compRe) *compRe = re;
} // UpdateEdges


//-----------------------------------------------------------------------------------
/**

  Updates minimum and maximum variables based on an input

  @param val     - New value to evaluate
  @param minVal  - Current minimum value
  @param maxVal  - Current maximum value

  @retval N/A

**/
void
UpdateMinMaxInt(
  INT16 val,
  INT16 *minVal,
  INT16 *maxVal
  )
{
  if (val < *minVal) *minVal = val;
  if (val > *maxVal) *maxVal = val;
} // UpdateMinMaxInt

/**

  Updates hi and low values based on a value and a limit

  @param val     - Value to evaluate
  @param limit   - Limit
  @param *loVal  - Low value
  @param *hiVal  - High value
  @retval N/A

**/
void
UpdateIoDelayLimit(
  UINT16  val,
  UINT16  limit,
  UINT16  *loVal,
  UINT16  *hiVal
  )
{
  if (val < *loVal) *loVal = val;

  if (limit - val < *hiVal) {
    *hiVal = limit - val;
  }
} // UpdateIoDelayLimit

UINT32
CollectParityResults (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chToCheck,
  UINT8 dimm, //DBG
  UINT32    bwSerr[MAX_CH][3]
  )
/*++
  Collects the results of the previous parity based test

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to check

  @retval status:       Non-zero if any failure was detected
--*/
{
  return CHIP_FUNC_CALL(host, ChipCollectParityResults(host, socket, chToCheck, dimm, bwSerr));
}

/**
  Clears bwSerr

  @param host:         Pointer to sysHost
  @param bwSerr:       Error status

  @retval status:       Non-zero if any failure was detected
**/
void
ClearBwSerr (
  UINT32    bwSerr[MAX_CH][3]
  )
{
  UINT8 ch;

  for (ch = 0; ch < MAX_CH; ch++) {
    bwSerr[ch][0] = 0;
    bwSerr[ch][1] = 0;
    bwSerr[ch][2] = 0;
  } // ch loop
} // ClearBwSerr

void
ClearParityResults (
  PSYSHOST  host,
  UINT8     socket,
  UINT32     chToCheck,
  UINT8 dimm
  )
/*++
  Clears the results of the previous parity based test

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to check

  @retval status:       Non-zero if any failure was detected
--*/
{
  CHIP_FUNC_CALL(host, ChipClearParityResults (host, socket, chToCheck, dimm));
}

void
ClearParityResultsAlt (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chToCheck,
  UINT8     rank,
  UINT8     dimm
  )
/*++
  Clears the results of the previous parity based test

  @param host:         Pointer to sysHost
  @param socket:       Processor socket to check
  @param rank:         Rank to clear
  @retval status:       Non-zero if any failure was detected
--*/
{
  CHIP_FUNC_CALL(host, ChipClearParityResultsAlt (host, socket, chToCheck, rank, dimm));
}

//------------------------------------------------------------------------------------------

/**

  Returns the logical rank number

  @param host   - Pointer to sysHost
  @param socket - Processor socket to check
  @param ch     - channel number
  @param dimm  - DIMM number
  @param rank  - rank number

  @retval Logical rank number

**/
UINT8
GetLogicalRank(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank
  )
{
  UINT8 logRank;

  if (CHIP_FUNC_CALL(host, DoesChipDetectCSorLRRnkDimm(host, socket, ch))) {
    if (dimm == 2) {
      logRank = 6 + (rank & 1);
    } else {
      logRank = (dimm << 2) + (rank & 1);
    }
  } else {
    logRank = (dimm << 2) + rank;
    if (dimm == 2) {     // CS# 8 or 9
      logRank = logRank - 2;
    }
  }

  return logRank;

} // GetLogicalRank

/**

  Enables parity checking on the DIMM

  @param host:         Pointer to sysHost
  @param socket:       Processor socket to check

**/
void
EnableParityChecking(
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8                       ch;
  UINT8                       dimm;
  UINT8                       rank;
  struct channelNvram         (*channelNvList)[MAX_CH];
  struct dimmNvram            (*dimmNvList)[MAX_DIMM];
  struct rankDevice           (*rankStruct)[MAX_RANK_DIMM];
  CHIP_PARITY_CHECKING_STRUCT chipEnableParityCheckingStruct;

  channelNvList = GetChannelNvList(host, socket);

  if(!CHIP_FUNC_CALL(host, DoesChipSupportParityChecking(host)))return;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, EARLY_CMD_CLK, (UINT16)((SUB_ENABLE_PARITY << 8)|socket)));
    // Enable parity
    CHIP_FUNC_CALL(host, ChipEnableParity(host, socket, ch, &chipEnableParityCheckingStruct));

#ifdef MEM_NVDIMM_EN
    if ((host->var.common.bootMode == NormalBoot) &&
      ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast) || (host->var.mem.subBootMode == NvDimmResume)) &&
      (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
#else
    if ((host->var.common.bootMode == NormalBoot) &&
      ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast)) &&
      (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
#endif
      //
      // Set 3N timing and save original setting
      //
      CHIP_FUNC_CALL(host, ChipSet3NTimingandSaveOrigSetting(host, socket, ch, &chipEnableParityCheckingStruct));

      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        rankStruct  = GetRankStruct(host, socket, ch, dimm);

        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          if ((host->nvram.mem.dimmTypePresent == UDIMM)||(host->nvram.mem.dimmTypePresent == SODIMM)) {
            //
            // Enable parity checking in the DRAMs
            //
            if (host->nvram.mem.socket[socket].ddrFreq < DDR_2400) {
              (*rankStruct)[rank].MR5 = (*rankStruct)[rank].MR5 | BIT0 | BIT9;
            } else {
              (*rankStruct)[rank].MR5 = (*rankStruct)[rank].MR5 | BIT1 | BIT9;
            }
            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
          } else {
            if ((rank == 0)) {
              //
              // Enable parity checking in the register
              //
              if (host->nvram.mem.socket[host->var.mem.currentSocket].cmdClkTrainingDone == 0) {
                if ((*dimmNvList)[dimm].aepDimmPresent) {
                  CHIP_FUNC_CALL(host, WriteRCFnv(host, socket, ch, dimm, BIT0, RDIMM_RC0E));
                } else {
                  WriteRC (host, socket, ch, dimm, rank, BIT0, RDIMM_RC0E);
                }
              } else {
                if ((*dimmNvList)[dimm].aepDimmPresent) {
                  CHIP_FUNC_CALL(host, WriteRCFnv(host, socket, ch, dimm, BIT0 | BIT2 | BIT3, RDIMM_RC0E));
                } else {
                  WriteRC (host, socket, ch, dimm, rank, BIT0 | BIT2 | BIT3, RDIMM_RC0E);
                }
              }
            }
          }
        } // rank loop
      } // dimm loop

      //
      // Restore original CMD timing
      //
      CHIP_FUNC_CALL(host, ChipRestoreOrigCMDtiming(host, socket, ch, &chipEnableParityCheckingStruct));
    }
  } // ch loop

  CHIP_FUNC_CALL(host, EnableCAParityRuntime(host, socket));
} // EnableParityChecking

/**

  Disables parity checking on the DIMM

  @param host:         Pointer to sysHost
  @param socket:       Processor socket to check

**/
void
DisableParityChecking(
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  CHIP_PARITY_CHECKING_STRUCT       chipDisableParityCheckingStruct;

  if(!CHIP_FUNC_CALL(host, DoesChipSupportParityChecking(host)))return;

  channelNvList = GetChannelNvList(host, socket);

  CHIP_FUNC_CALL(host, ChipSpecificParityCheckDisable(host, socket));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    CHIP_FUNC_CALL(host, ChipSpecificParityCheckDisablePerCh(host, socket, ch, &chipDisableParityCheckingStruct));
#ifdef MEM_NVDIMM_EN
    if ((host->var.common.bootMode == NormalBoot) &&
      ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast) || (host->var.mem.subBootMode == NvDimmResume)) &&
      (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
#else
    if ((host->var.common.bootMode == NormalBoot) &&
        ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast)) &&
        (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
#endif
      //
      // Set 3N timing and save original setting
      //
      CHIP_FUNC_CALL(host, ChipSet3NTimingandSaveOrigSetting(host, socket, ch, &chipDisableParityCheckingStruct));

      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        rankStruct  = GetRankStruct(host, socket, ch, dimm);

        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          if ((host->nvram.mem.dimmTypePresent == UDIMM) || (host->nvram.mem.dimmTypePresent == SODIMM)) {

            if (host->nvram.mem.socket[socket].ddrFreq < DDR_2400) {
              (*rankStruct)[rank].MR5 = (*rankStruct)[rank].MR5 & ~(BIT0 | BIT9);
            } else {
              (*rankStruct)[rank].MR5 = (*rankStruct)[rank].MR5 & ~(BIT1 | BIT9);
            }

            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
          } else {
            if (rank == 0){
              if ((*dimmNvList)[dimm].aepDimmPresent) {
                CHIP_FUNC_CALL(host, WriteRCFnv(host, socket, ch, dimm, 0, RDIMM_RC0E));
              } else {
                WriteRC (host, socket, ch, dimm, rank, 0, RDIMM_RC0E);
              }
            }
          }
        } // rank loop
      } // dimm loop

      //
      // Restore original CMD timing
      //
      CHIP_FUNC_CALL(host, ChipRestoreOrigCMDtiming(host, socket, ch, &chipDisableParityCheckingStruct));
    }

    CHIP_FUNC_CALL(host, ChipSpecificErrorEnable(host, socket, ch, &chipDisableParityCheckingStruct));

  } // ch loop
} // DisableParityChecking

/**

  Enables normal CMD timing

  @param host      - Pointer to sysHost
  @param socket    - Processor socket

  @retval N/A

**/
void
SetNormalCMDTiming(
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8                   ch;
  UINT8                   dimm;
  UINT8                   rank;
  UINT8                   rtDiff;
  UINT8                   logRank;
  struct channelNvram     (*channelNvList)[MAX_CH];
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  struct ddrRank          (*rankList)[MAX_RANK_DIMM];
  struct RankCh           (*rankPerCh)[MAX_RANK_CH];
  CHIP_SET_NORMAL_CMD_TIMING_STRUCT chipSetNormalCMDTimingStruct;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    rankPerCh = &(*channelNvList)[ch].rankPerCh;

    //
    // Set normal CMD timing
    //
    if(CHIP_FUNC_CALL(host, ShouldChipSetNormalCMDTiming(host, socket, ch, &chipSetNormalCMDTimingStruct))){

      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Setting cmd timing to %d\n", (*channelNvList)[ch].timingMode));

      //
      // Determine Roundtrip difference
      //
      switch ((*channelNvList)[ch].timingMode) {
        case TIMING_1N:
          if ((*channelNvList)[ch].trainTimingMode == TIMING_2N) rtDiff = CHIP_RNDTRP_DIFF_MPR_OFF_2N_TO_1N;
          else rtDiff = CHIP_RNDTRP_DIFF_MPR_OFF_3N_TO_1N;
          break;
        case TIMING_2N:
          if ((*channelNvList)[ch].trainTimingMode == TIMING_2N) rtDiff = CHIP_RNDTRP_DIFF_MPR_OFF_2N_TO_2N;
          else rtDiff = CHIP_RNDTRP_DIFF_MPR_OFF_3N_TO_2N;
          break;
        default:
          rtDiff = CHIP_RNDTRP_DIFF_MPR_OFF_1N_TO_1N;
          break;
      } // switch timingMode

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          logRank = GetLogicalRank(host, socket, ch, dimm, rank);

          (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip =
            (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip - rtDiff;

          MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "Setting RTL to %d\n", (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));

          CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));

        } // rank loop
      } // dimm loop
    } // if timing needs to be changed
  } // ch loop
} // SetNormalCMDTiming

/**
  Routine Description:
  Normalize all PI's to reduce latency

  @param host   - Pointer to sysHost
  @param socket - Processor socket

**/
UINT32
NormalizeCCC(
  PSYSHOST  host,
  UINT8     socket
)

{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               clk;
  INT16               piDelay = 0;
  UINT16              minDelay;
  UINT16              maxDelay;
  UINT8               ckEnabled[MAX_CLK];
  UINT8               ctlIndex[MAX_CLK];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  if (!(host->memFlowsExt & MF_EXT_CMD_NORM_EN)) return SUCCESS;
  if (!(host->setup.mem.optionsExt & CMD_NORMAL_EN)) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,    "\n"));
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "Normalizing Cmd/Ctl/Clk\n"));

  //
  //Push out TX PI's to maximize CMD norm benefit
  //
  CHIP_FUNC_CALL(host, WLPushOut(host,socket));

  // Show the current RecEn and TxDqDqs values (Comment out for upload)
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\n BEFORE Normalization settings...\n"));
    DisplayResults(host, socket, TxDqsDelay);
    DisplayResults(host, socket, TxDqDelay);
    CHIP_FUNC_CALL(host, DisplayCCCResults(host, socket));
  }
#endif

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    minDelay      = 255;
    maxDelay      = 0;

    for (clk = 0; clk < MAX_CLK; clk++) {
      ckEnabled[clk]  = 0;
      ctlIndex[clk]   = 0;
    } // clk loop

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        clk             = (*rankList)[rank].ckIndex;
        ctlIndex[clk]   = (*rankList)[rank].ctlIndex;
        ckEnabled[clk]  = 1;
      } // rank loop
    } // dimm loop

    //
    // Read all delays to find the minimum
    //
    for (clk = 0; clk < MAX_CLK; clk++) {
      if(ckEnabled[clk] == 0) continue;
      CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex[clk] + CtlGrp0, GSM_READ_ONLY, &piDelay, &minDelay, &maxDelay));
    } // clk loop

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_READ_ONLY, &piDelay, &minDelay, &maxDelay));

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY,
                         (INT16 *)&piDelay));
          UpdateMinMaxInt(piDelay-EXTRAPIMARGIN, (INT16 *)&minDelay, (INT16 *)&maxDelay);

          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_ONLY,
                         (INT16 *)&piDelay));
          UpdateMinMaxInt(piDelay-EXTRAPIMARGIN, (INT16 *)&minDelay, (INT16 *)&maxDelay);
        } // strobe loop
      } // rank loop
    } // dimm loop

    //
    // Normalize to zero by subtracting the minimum
    //
    piDelay = -minDelay;
    host->nvram.mem.normalizationFactor[socket][ch] = piDelay;

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Normalization values: Min PI = %-4d, Max PI = %-4d, normalization = %-4d ticks\n",
      minDelay, maxDelay, piDelay));

    // unlikely corner case check this means we probably don't have enough margin to finish training.
    if  (minDelay < 16 ) {
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "ERROR: Could not normalize > 16 PI ticks, expecting >100\n"));
    }

    for (clk = 0; clk < MAX_CLK; clk++) {
      if(ckEnabled[clk] == 0) continue;
      CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, clk, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay));
      CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex[clk] + CtlGrp0, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minDelay, &maxDelay));
    } // clk loop

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minDelay, &maxDelay));

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                         (INT16 *)&piDelay));
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                         (INT16 *)&piDelay));
        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

  disableAllMsg (host);
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  JedecInitSequence (host, socket, CH_BITMASK); // after moving CMD/CTL/CLK
  restoreMsg (host);

  //
  // Re-run Receive Enable training
  //
  disableAllMsg (host);
  ReceiveEnable(host);
  restoreMsg (host);

  //
  //  Check all settings after normalization (might take out)
  //
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\n AFTER normalize settings...\n"));
    DisplayResults(host, socket, TxDqsDelay);
    DisplayResults(host, socket, TxDqDelay);
    CHIP_FUNC_CALL(host, DisplayCCCResults(host, socket));
  }
#endif

  return SUCCESS;
} // NormalizeCCC


/**
  Routine Description:
  DeNormalize all PI's so CMD/CTL's can be margined

  Arguments:

    host      - Pointer to sysHost
    socket    - current socket
    direction - DENORMALIZE (-1) or RENORMALIZE (1)

**/
UINT32
DeNormalizeCCC(
  PSYSHOST  host,
  UINT8     socket,
  INT8      direction
)
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               clk;
  INT16               piDelay;
  UINT8               ckEnabled[MAX_CLK];
  UINT8               ctlIndex[MAX_CLK];
  UINT16              minDelay;
  UINT16              maxDelay;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  if (!(host->memFlowsExt & MF_EXT_CMD_NORM_EN)) return SUCCESS;
  if (!(host->setup.mem.optionsExt & CMD_NORMAL_EN)) return SUCCESS;


  //  Added conditional to identify the type of normalization being done
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,    "\n"));
  if (direction == DENORMALIZE){
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "DeNormalize Cmd/Ctl/Clk\n"));
  } else {
    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "ReNormalize Cmd/Ctl/Clk\n"));
  }

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    piDelay = direction * host->nvram.mem.normalizationFactor[socket][ch];

    minDelay = 255;
    maxDelay = 0;

    //  Check the value and see if it is the same value as the normalization factor
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Cmd normalization Factor %d\tDirection %d\n", host->nvram.mem.normalizationFactor[socket][ch], direction));

    for (clk = 0; clk < MAX_CLK; clk++) {
      ckEnabled[clk] = 0;
      ctlIndex[clk] = 0;
    } // clk loop

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        clk             = (*rankList)[rank].ckIndex;
        ctlIndex[clk]   = (*rankList)[rank].ctlIndex;
        ckEnabled[clk]  = 1;
      } // rank loop
    } // dimm loop

    for (clk = 0; clk < MAX_CLK; clk++) {
      if(ckEnabled[clk] == 0) continue;

      CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, clk, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay));

      CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex[clk] + CtlGrp0, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minDelay, &maxDelay));
    } // clk loop

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay, &minDelay, &maxDelay));

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                         (INT16 *)&piDelay));

          CHIP_FUNC_CALL(host, GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                         (INT16 *)&piDelay));
        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

  disableAllMsg (host);
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  JedecInitSequence (host, socket, CH_BITMASK); // after moving CMD/CTL/CLK
  restoreMsg (host);

  //
  // Re-run Receive Enable training (messes up LRDIMMs when normalizing cmd/ctl)
  //
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "Re-run Receive Enable training\n"));
  disableAllMsg (host);
  ReceiveEnable(host);
  RoundTripOptimize (host);
  restoreMsg (host);

  //
  //  Check to see if the settings are restored
  //
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\nShowing current settings...\n"));

    DisplayResults(host, socket, RecEnDelay);
    DisplayResults(host, socket, TxDqsDelay);
    DisplayResults(host, socket, TxDqDelay);
    CHIP_FUNC_CALL(host, DisplayCCCResults(host, socket));
  }
#endif

  return SUCCESS;
} // DeNormalizeCCC



/**

  Checks to see of supplied group is a member of the CMD Group.

  @param group - Group item to check

  @retval 1 - Is a member of CMD Group
  @retval 0 - Is not a member of CMD Group

**/
UINT8
CheckCMDGroup(
  GSM_GT  group
  )
{
  if ((group == CmdAll) || (group == CmdGrp0) || (group == CmdGrp1) || (group == CmdGrp2) ||  (group == CmdGrp3) || (group == CmdGrp4) || (group == CmdGrp5) || (group == CmdVref)) {
    return 1;
  } else {
    return 0;
  }
} // CheckCMDGroup

/**

  Checks to see of supplied group is a member of the CTL Group.

  @param group - Group item to check

  @retval 1 - Is a member of CTL Group
  @retval 0 - Is not a member of CTL Group

**/
UINT8
CheckCTLGroup(
  GSM_GT  group
  )
{
  if ((group == CtlAll) || (group == CtlGrp0) || (group == CtlGrp1) || (group == CtlGrp2) || (group == CtlGrp3) || (group == CtlGrp4)) {
    return 1;
  } else {
    return 0;
  }
} // CheckCTLGroup


#define SETUP_FOR_RE_TEST BIT0
#define TEAR_DOWN_RE_TEST BIT1
#define EXECUTE_RE_TEST   BIT2



GSM_CSN   CtlToSweep[NUM_SIGNALS_TO_SWEEP_LB] = {
  CS0_N, CS1_N, CS2_N, CS3_N, CS4_N, CS5_N, CS6_N, CS7_N,
  CKE0, CKE1, CKE2, CKE3,
  ODT0, ODT1, ODT2, ODT3,
#ifdef CA_LOOPBACK
  C2, A17, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, BA0, BA1, BG0, BG1, PAR, ACT_N
#endif
};

/**

  Exectues early control/clock training with Loopback mode

  @param host - Pointer to sysHost

  @retval N/A

**/
UINT32
EarlyCtlClkLoopback(
  PSYSHOST  host
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               rankPresent;
  UINT8               controlWordData = 0;
  UINT8               signal;
  UINT8               index;
  GSM_CSN             signalToSweep[MAX_CH];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  INT16               le[MAX_CH];
  INT16               re[MAX_CH];
//  UINT32              lrbufData;
  GSM_CSEDGE_CTL      signalEdgeData[MAX_CH][MAX_RANK_CH][NUM_SIGNALS_TO_SWEEP_LB];
  UINT32              status = SUCCESS;
#ifdef SERIAL_DBG_MSG
  char                strBuf0[128];
#endif //SERIAL_DBG_MSG
  UINT8               sweepSomething;

  socket = host->var.mem.currentSocket;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "EarlyCtlClkLoopback Starts\n"));
#endif

  channelNvList = GetChannelNvList(host, socket);
  index = 0;

  // Chip Workaround
  CHIP_FUNC_CALL(host, OffsetCCC(host, socket, 64, 1));

  //
  // Enable parity checking
  //
  EnableParityChecking(host, socket);

  CHIP_FUNC_CALL(host, ToggleBcom(host, socket, 0));

  for (ch = 0; ch < MAX_CH; ch++){
    for (rank = 0; rank < MAX_RANK_CH; rank++){
      for (signal = 0; signal < NUM_SIGNALS_TO_SWEEP_LB; signal++){
        signalEdgeData[ch][rank][signal].toSample = 0; // Data structure containing CS, CKE, and ODT values
      }
    }
  }//ch loop

  //
  // Test every rank on every Dimm
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, EARLY_CTL_CLK, (UINT16)((socket << 8)|(dimm << 4)|rank)));
      rankPresent = 0;

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        //
        // Indicate this rank is present on at least one channel
        //
        rankPresent = 1;
        break;
      } // ch loop

      //
      // Continue to the next rank if this one is not populated
      //
      if (rankPresent == 0) continue;

      //
      // S2#/S3# or CID0,1,2 for LRDIMM and 3DS are handled when rank=0
      //
      if (rank >= 2) continue;

      //
      //RC0C,DA[3:0] : CS -- 001x, CKE -- 010x, ODT -- 011x, where x is rank.
      //Normal --- 0000, CK to CA -- 0001
      //
      // CS  -> signal 0
      // CKE -> signal 1
      // ODT -> signal 2
      // GNT -> signal 3
      // C0  -> signal 4
      // C1  -> signal 5
      // C2  -> signal 6
      // A17  -> signal 7
      // etc...
      //
      for (signal = 0; signal < 6 + NUM_CA_SIGNALS; signal++) {

        //
        //Once in Loopback mode, Dimm will no longer accept MRS commands until JEDEC Init is called
        //
        sweepSomething = 0;
        for (ch = 0; ch < MAX_CH; ch++) {
          signalToSweep[ch] = NO_SIGNAL; // initialize to no signal
          if ((*channelNvList)[ch].enabled == 0) continue;

          dimmNvList = GetDimmNvList(host, socket, ch);
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          // Convert signal to controlWordData CS/CKE/ODT
          if (signal < 3) {
            // CTL signal loopback
            controlWordData = ((signal + 1) * 2) + rank;

          } else if (signal == 3) {
            // GNT loopback for NVM DIMMs
            if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
            controlWordData = 3;

          } else if (signal == 4) {
            // C0/S2# loopback for LRDIMM, Encoded QuadCS, or Encoded 3DS mode
            // skip if rank =1, NOT LRDIMM or if NVMDIMM
            if (rank || (isCmdCtlTrained(host, socket, ch, dimm, C0) != (UINT32)EARLY_CTL_CLK )) continue;
            //if (rank || (*dimmNvList)[dimm].aepDimmPresent || (IsLrdimmPresent(host, socket, ch, dimm)==0) ) continue;
            controlWordData = 1;

          } else if (signal == 5) {
            // C1/S3# loopback for Encoded 3DS mode and diecount > 2
            // skip if rank = 1, Direct or encoded LRDIMM, skip if NOT LRDIMM, or if NVMDIMM
            if (rank || (isCmdCtlTrained(host, socket, ch, dimm, C1) != (UINT32)EARLY_CTL_CLK )) continue;
            //if (rank || (*dimmNvList)[dimm].aepDimmPresent || (*dimmNvList)[dimm].dieCount < 3) continue;
            controlWordData = 1;

          } else if (signal == 6) {
            // C2 loopback for Encoded 3DS mode and diecount > 4
            // Signal is shared, so all DIMMs need to use largest dieCount on the channel
            if (rank || (isCmdCtlTrained(host, socket, ch, dimm, C2) != (UINT32)EARLY_CTL_CLK )) continue;
            //if (rank || (*dimmNvList)[dimm].aepDimmPresent || (*dimmNvList)[dimm].dieCount < 5 ) continue;
            controlWordData = 1;

          } else if (signal == 7) {
            // A17 loopback
            // Add support for 16Gb density, should be in earlycmdclk
            continue;

          } else {
            // Loopback all other CA signals
            if (rank) continue;
            controlWordData = 1;
          }

          switch (signal){
            case 0:
              index = (dimm*4+rank);
              break;
            case 1:
              index = (dimm*2+rank+NUM_CS);
              break;
            case 2:
              index = (dimm*2+rank+NUM_CS+NUM_CKE);
              break;
            case 3:
              //CS1 used for GNT
              index = (dimm*4+rank+1);
              break;
            case 4:
              //CS2 used for C0
              index = (dimm*4+rank+2);
              break;
            case 5:
              //CS3 used for C1
              index = (dimm*4+rank+3);
              break;
            default:
              index = signal - 6 + NUM_CTL_SIGNALS;
              break;
          }//signal conditional

          signalToSweep[ch] = CtlToSweep[index];
          sweepSomething = 1;

        } // ch loop

        if (!sweepSomething) continue;

        // Use SMBus access change loopback mode
        // Optional DDR4 method: use JedecInit to reset RCD state. Not compatible with NVMDIMM
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

          dimmNvList = GetDimmNvList(host, socket, ch);
          (*dimmNvList)[dimm].rcCache[RDIMM_RC0C] = controlWordData;
          CHIP_FUNC_CALL(host, ChipSetCTLLoopback(host, socket, ch, dimm));
        } //ch

        GetCtlMarginsSweep(host, socket, dimm, rank, signalToSweep, le, re);

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          if (signalToSweep[ch] == NO_SIGNAL) continue;

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "Loop %d, Signal %s,  left %d,  right %d\n", signal, CHIP_FUNC_CALL(host, GetSignalStr(signalToSweep[ch], (char *)&strBuf0)), le[ch], re[ch]));

          rankList = GetRankNvList(host, socket, ch, dimm);
          //
          // Convert results from absolute pi values to offset values and save the results
          //
          signalEdgeData[ch][(*rankList)[rank].rankIndex][index].signal = signalToSweep[ch];
          signalEdgeData[ch][(*rankList)[rank].rankIndex][index].le = le[ch];
          signalEdgeData[ch][(*rankList)[rank].rankIndex][index].re = re[ch];
          signalEdgeData[ch][(*rankList)[rank].rankIndex][index].toSample = 1;

        } // ch loop
      }//signal loop
    }//rank loop

    //Restore Normal Operating mode after sweeping CS/CKE/ODT for DIMM
    controlWordData = 0;
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      (*dimmNvList)[dimm].rcCache[RDIMM_RC0C] = controlWordData;
      CHIP_FUNC_CALL(host, ChipSetCTLLoopback(host, socket, ch, dimm));
    } //ch
  }//dimm loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Evaluate results and write them
    //
    status = (UINT32)CHIP_FUNC_CALL(host, SetCombinedCtlGroup(host, socket, ch, NUM_SIGNALS_TO_SWEEP_LB, signalEdgeData[ch]));

  }//ch loop

  // Chip Workaround
  CHIP_FUNC_CALL(host, OffsetCCC(host, socket, -64, 1));

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    CHIP_FUNC_CALL(host, DisplayCCCResults(host, socket));
  }
#endif // SERIAL_DBG_MSG

  //
  // Disable parity checking
  //
  DisableParityChecking(host, socket);
  CHIP_FUNC_CALL(host, ToggleBcom(host, socket, 1));

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "EarlyCtlClkLoopback Ends\n"));
#endif
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  return status;
} // EarlyCtlClkLoopback


/**

  Finds control margins for the given signal

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param dimm        - DIMM number
  @param rank        - rank number
  @param signal      - Signal
  @param *le         - Left edge
  @param *re         - Right edge

  @retval N/A

**/
void
GetCtlMarginsSweep(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm,
  UINT8     rank,
  GSM_CSN   signal[MAX_CH],
  INT16     *le,
  INT16     *re
  )
{
  UINT8               ch;
  INT16               piIndex;
  UINT16              i;
  UINT8               alertFeedback[MAX_CH][CTL_CLK_TEST_RANGE];
  INT16               orgDelay[MAX_CH];
  UINT32              rankMask[MAX_CH];
  UINT8               alertCentering[MAX_CH];
  UINT32              chBitmask = 0;
  struct cmdEye       curEye[MAX_CH];
  struct channelNvram (*channelNvList)[MAX_CH];
#ifdef SERIAL_DBG_MSG
  char                strBuf0[128];
#endif
  UINT16              piPosition[MAX_CH];
  UINT32              status = 0;
  UINT8               err = 0;
  UINT16              eyeWidth;
  //ALERTSIGNAL_MCDDC_DP_STRUCT alertSignal;
  //DDRT_ERROR_MC_2LM_STRUCT     errSignals;
  //MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
  //               "GetCtlMarginsSweep\n"));

  MemSetLocal ((UINT8 *)alertCentering, 0, sizeof (alertCentering));
  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {

    rankMask[ch] = 0;
    orgDelay[ch] = 0;

    if ((*channelNvList)[ch].enabled == 0) continue;

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    re[ch] = UNMARGINED_CMD_EDGE; // UNMARGINED_CMD_EDGE = 511
    le[ch] = -UNMARGINED_CMD_EDGE;

    //if (isCmdCtlTrained(host, socket, ch, dimm, signal[ch]) != (UINT32)EARLY_CTL_CLK ) continue;
    if (signal[ch] == NO_SIGNAL) continue;

    // Initialize ch rank mask
    rankMask[ch] = 1; // rank is enabled

    curEye[ch].left = 0;
    curEye[ch].right = 0;

    //
    // Get the original delay
    //

    CHIP_FUNC_CALL(host, GetSetSignal (host, socket, ch, DdrLevel, signal[ch], GSM_READ_ONLY, &orgDelay[ch]));

    // Clear dimm error results
    // support larger step sizes
    for (piIndex = 0; piIndex < CTL_CLK_TEST_RANGE; piIndex++) {
      alertFeedback[ch][piIndex] = 0; // Clear
    } //piIndex loop

    // Indicate rank present and enabled on this ch
    chBitmask |= 1 << ch;

    //
    //Set CADB Pattern for each Channel
    //
    CHIP_FUNC_CALL(host, CpgcCtlClkPattern (host, socket, ch, signal[ch]));
  }// ch loop

  //
  // Run concurrent tests
  //
  CHIP_FUNC_CALL(host, CpgcGlobalTestStart (host, socket, chBitmask));


  // Alternative method mod 128 + 128
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((rankMask[ch] == 0)) continue;

    alertCentering[ch] = CHIP_FUNC_CALL(host, ChipGetParityErrorStatus(host, socket, ch, dimm));
  } //ch loop

  //
  // Assert that CTL_CLK_TEST_RANGE is evenly divisible by CTL_CLK_STEPSIZE
  //
  if (CTL_CLK_TEST_RANGE % CTL_CLK_STEPSIZE) {
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_70);
  }
  //
  // Sweep Pi delays
  //
  for (piIndex = 0; piIndex < CTL_CLK_TEST_RANGE; piIndex += CTL_CLK_STEPSIZE) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((rankMask[ch] == 0)) continue;

      //
      //Set Pi Value
      //

      CHIP_FUNC_CALL(host, GetSetSignal (host, socket, ch, DdrLevel, signal[ch], GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &piIndex));

    } //ch loop

    //
    // Generate an IO Reset. This is required because the pi delays changed.
    //
    CHIP_FUNC_CALL(host, IO_Reset (host, socket));

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((rankMask[ch] == 0)) continue;
      err = CHIP_FUNC_CALL(host, ChipGetParityErrorStatus(host, socket, ch, dimm));

      if (alertCentering[ch]) {
        alertFeedback[ch][piIndex] = !err;
      } else {
        alertFeedback[ch][piIndex] = err;
      }

      // Fill unused feedback entries
      for (i = 1; i < CTL_CLK_STEPSIZE; i++) {
        alertFeedback[ch][piIndex+i] = alertFeedback[ch][piIndex];
      }

    } //ch loop
  }//piIndex loop

  //
  // Stop Test
  //
  CHIP_FUNC_CALL(host, CpgcGlobalTestStop (host, socket, chBitmask));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((rankMask[ch] == 0)) continue;
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "Sweep results for %s\n", CHIP_FUNC_CALL(host, GetSignalStr(signal[ch], (char *)&strBuf0))));
    for (piIndex = 0; piIndex < CTL_CLK_TEST_RANGE; piIndex++) {

      if (alertFeedback[ch][piIndex]) {
        // '1'
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "1"));
      } else {
        // '0'
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "0"));
      }
    } // piIndex loop

    // Right -> Index End
    // Left -> Index Beginning
    status = FindErrorGraphMin (host, socket, ch, dimm, rank, 0, &alertFeedback[ch][0], CTL_CLK_TEST_RANGE,
                                (UINT16 *)&curEye[ch].right, (UINT16 *)&curEye[ch].left, (UINT16 *)&piPosition[ch]);

    if ((status == FAILURE) || (curEye[ch].left == curEye[ch].right)){
      curEye[ch].left = 0;
      curEye[ch].right = 0;
    }

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n"));
  }//ch loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((rankMask[ch] == 0)) continue;

    //
    // Disable CADB always on mode before DisableChannel
    //
    CHIP_FUNC_CALL(host, DisableCADBAlwaysOnMode(host, socket, ch));
    
    // Update return parameters
    re[ch] = curEye[ch].right;
    le[ch] = curEye[ch].left;

    // apply minimum eye width check here
    eyeWidth = ABS((re[ch] - le[ch]));
    if (eyeWidth <= 40) {
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
        "Eye width is too small: %d\n", eyeWidth));
      EwlOutputType3(host, WARN_MEMORY_TRAINING, WARN_CTL_CLK_LOOPBACK_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, CtlAll, DdrLevel, signal[ch], (UINT8)(eyeWidth));
      DisableChannel(host, socket, ch);
    }

    if (re[ch] < le[ch]) {
      // wrap around case
      re[ch] += 256;
    }
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "le = %d - re = %d\n", le[ch], re[ch]));
    //
    // Restore signal under test to its initial setting
    //
    CHIP_FUNC_CALL(host, GetSetSignal (host, socket, ch, DdrLevel, signal[ch], GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &orgDelay[ch]));

  } //ch loop

}//GetCtlMarginsSweep

/**

  Exectues early control/clock training

  @param host - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
EarlyCtlClk(
  PSYSHOST  host
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               imc = 0;
  UINT8               numRanks;
  UINT32              chBitmask;
  UINT8               dimm;
  UINT8               rank;
  UINT8               sign;
  UINT8               rtlOffset[2];
  UINT8               rtlIndex;
  UINT8               ctlIndex;
  UINT8               ctlEnabled;
  UINT32              strobeFeedback[2][MAX_CH];
  UINT32              strobeCompareFeedback;
  UINT32              origMemFlows;
  UINT8               roundTripLatency[MAX_CH];
  INT16               offset;
  INT16               currentOffset[MAX_CH];
  UINT16              minDelay;
  UINT16              maxDelay;
  INT16               ctlMargins[MAX_CH][MAX_RANK_CH][2];
  INT16               ctlOffset;
  INT16               ctlLeft;
  INT16               ctlRight;
  UINT8               rankPresent;
  UINT32              ranksDone[MAX_IMC];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  if (~host->memFlows & MF_E_CTLCLK_EN) return SUCCESS;

  host->var.mem.checkMappedOutRanks = 1;

  //
  // Return if this socket is disabled
  //
  socket = host->var.mem.currentSocket;
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);

  //if(!ShouldChipPerformEarlyCtlClk(host))
  //  return SUCCESS;

  // Execute Loopback for RDIMM, LRDIMM, NVM DIMM configs
  if (CHIP_FUNC_CALL(host, CheckCTLLoopback(host))) {
    EarlyCtlClkLoopback(host);
    host->var.mem.checkMappedOutRanks = 0;
    return SUCCESS;
  }

  CHIP_FUNC_CALL(host, PerformChipLRDIMMEarlyCtlClk(host, socket));
  minDelay = 255;
  maxDelay = 0;

  CHIP_FUNC_CALL(host, InitRtlOffsetChip(rtlOffset));
  //
  // Initial Rec En training
  //
#ifdef SERIAL_DBG_MSG
  host->var.mem.earlyCtlClkSerialDebugFlag = 1;
#endif
  // force receive enable to run if using earlyctlclk rcven method
  origMemFlows = host->memFlows;
  host->memFlows |= MF_REC_EN_EN;
  ReceiveEnable(host);

  //restore memflows
  host->memFlows= origMemFlows;
#ifdef SERIAL_DBG_MSG
  host->var.mem.earlyCtlClkSerialDebugFlag = 0;
#endif

  for (ch = 0; ch < MAX_CH; ch++) {
    for (rank = 0; rank < MAX_RANK_CH; rank++) {
      ctlMargins[ch][rank][0] = 0;
      ctlMargins[ch][rank][1] = 0;
    } // rank loop
  } // ch loop

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train ranks
    //
    numRanks = CHIP_FUNC_CALL(host, NumRanksToTrain(host, socket, host->var.mem.socket[socket].maxRankDimm));
    for (rank = 0; rank < numRanks; rank++) {

      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, EARLY_CTL_CLK, (UINT16)((socket << 8)|(dimm << 4)|rank)));

      //
      // Initialize to no ranks present
      //
      rankPresent = 0;
      chBitmask = GetChBitmask (host, socket, dimm, rank);

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        //
        // Indicate this rank is present on at least one channel
        //
        rankPresent = 1;

        CHIP_FUNC_CALL(host, GetOriginalRtlChip(host, socket, ch, dimm, rank, RTL_INITIALIZE_ONCE, &roundTripLatency[ch]));
      } // ch loop

      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (!rankPresent) continue;

      //
      //Set feedback comparison value depending on if ECC
      //
      if ((!host->nvram.mem.eccEn)){
        strobeCompareFeedback = 0x1feff;
      } else {
        strobeCompareFeedback = 0x3ffff;
      }

      //
      // Search CTL from the center out to find both edges
      //
      for (sign = 0; sign < 2; sign++) {

        if (sign) {
          offset = -CTL_CLK_STEPSIZE;
        } else {
          offset = CTL_CLK_STEPSIZE;
        }

        // Initialize to no failures
        for (imc = 0; imc < MAX_IMC; imc++) {
          ranksDone[imc] = 0xFFFFFFFF;
        }
        chBitmask = GetChBitmask (host, socket, dimm, rank);

        //
        // Setup the test but do not execute it
        //
        ExecuteCtlClkTest(host, socket, chBitmask, dimm, rank, SETUP_FOR_RE_TEST);

        for (ch = 0; ch < MAX_CH; ch++) {
          currentOffset[ch] = 0;
          imc = CHIP_FUNC_CALL(host, GetMCID(host, socket, ch));
          if ((*channelNvList)[ch].enabled == 0) continue;
          if ((chBitmask & (1 << ch)) == 0) continue;

          rankList = GetRankNvList(host, socket, ch, dimm);

          // Indicate this rank is not done
          ranksDone[imc] &= ~(1 << ((*rankList)[rank].rankIndex + (CHIP_FUNC_CALL(host, GetMCCh(imc, ch) * 8))));

          strobeFeedback[0][ch] = 0;
          strobeFeedback[1][ch] = 0;
          CHIP_FUNC_CALL(host, GetOriginalRtlChip(host, socket, ch, dimm, rank, RTL_RE_INITIALIZE, &roundTripLatency[ch]));
        } // ch loop

        // Loop until we either find an edge or reach the end of our range
#ifndef DE_SKU
        while ((ranksDone[IMC0] & ranksDone[IMC1]) != 0xFFFFFFFF) {
#else
        while (ranksDone[IMC0] != 0xFFFFFFFF) {
#endif  //DE_SKU
          for (ch = 0; ch < MAX_CH; ch++) {
            if ((chBitmask & (1 << ch)) == 0) continue;

            //
            // Offset CTL
            //
            CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, CtlAll, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &offset, &minDelay, &maxDelay));

            currentOffset[ch] += offset;
          } // ch loop


          CHIP_FUNC_CALL(host, IO_Reset (host, socket));

          //OutputCheckpoint (host, 0xBE, (UINT8)currentOffset[ch - 1], 0);

          //
          // Get strobe levels at two RTL settings
          //
          for (rtlIndex = 0; rtlIndex < 2; rtlIndex++) {

            for (ch = 0; ch < MAX_CH; ch++) {
              if ((chBitmask & (1 << ch)) == 0) continue;

              CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank),
                  roundTripLatency[ch] + rtlOffset[rtlIndex] + CHIP_FUNC_CALL(host, GetChipAdjForRTL(host, socket, ch, dimm))));
            } // ch loop

            //
            // Execute the CTL-CLK test
            //
            ExecuteCtlClkTest(host, socket, chBitmask, dimm, rank, EXECUTE_RE_TEST);

            GetCtlClkResults(host, socket, dimm, rank, strobeFeedback[rtlIndex]);
          } // rtlIndex loop

          //
          // Check results. If RTL offset 0 is not all zero's or RTL offset 2 is not all 1's then this is a failure
          //
          for (ch = 0; ch < MAX_CH; ch++) {
            imc = CHIP_FUNC_CALL(host, GetMCID(host, socket, ch));
            if ((chBitmask & (1 << ch)) == 0) continue;

            rankList = GetRankNvList(host, socket, ch, dimm);

            // For debug only
            //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            //              "PI %3d Feedback = [%5x,%5x]\n", currentOffset[ch], strobeFeedback[0][ch], strobeFeedback[1][ch]));

            if (((strobeFeedback[0][ch] != 0) || (strobeFeedback[1][ch] != strobeCompareFeedback)) || (currentOffset[ch] < -127) || (currentOffset[ch] > 127)) {
              //
              // Edge found
              //
              ctlMargins[ch][(*rankList)[rank].rankIndex][sign] = currentOffset[ch];

              //
              // Indicate this rank is done
              //
              ranksDone[imc] |= 1 << ((*rankList)[rank].rankIndex + (CHIP_FUNC_CALL(host, GetMCCh(imc, ch) * 8)));
              chBitmask &= ~(1 << ch);
            }

            //
            // Restore RTL
            //
            CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank), roundTripLatency[ch]));

            if ((currentOffset[ch] < -128) || (currentOffset[ch] > 128)) {
              chBitmask &= ~(1 << ch);
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "No edge found\n"));
            }
          } // ch loop

          if (chBitmask == 0){
            ranksDone[IMC0] = 0xFFFFFFFF;
 #ifndef DE_SKU
            ranksDone[IMC1] = 0xFFFFFFFF;
 #endif
          }

        } // while ranksDone

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          currentOffset[ch] = 0 - currentOffset[ch];

          //
          // Restore CTL pi Delay
          //
          CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, CtlAll, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &currentOffset[ch], &minDelay, &maxDelay));
        } // ch loop

        disableAllMsg (host);
        CHIP_FUNC_CALL(host, IO_Reset (host, socket));
        chBitmask = GetChBitmask (host, socket, dimm, rank);
        //
        // Tear down the test and do not execute it
        //
        ExecuteCtlClkTest(host, socket, chBitmask, dimm, rank, TEAR_DOWN_RE_TEST);
        JedecInitSequence (host, socket, CH_BITMASK);
        restoreMsg (host);
      } // sign loop
    } // rank loop
  } // dimm loop

  //
  // Calculate average CTL across ranks
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);
      numRanks = CHIP_FUNC_CALL(host, NumRanksToTrain(host, socket, host->var.mem.socket[socket].maxRankDimm));
      for (rank = 0; rank < numRanks; rank++) {

      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                      "le = %3d - re = %3d  width = %3d\n", ctlMargins[ch][(*rankList)[rank].rankIndex][1], ctlMargins[ch][(*rankList)[rank].rankIndex][0],ctlMargins[ch][(*rankList)[rank].rankIndex][0]-ctlMargins[ch][(*rankList)[rank].rankIndex][1]));
      } // rank loop
    } // dimm loop
  } // ch loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    for (ctlIndex = 0; ctlIndex < 5; ctlIndex++) {

      ctlLeft = -0xff;
      ctlRight = 0xff;
      ctlEnabled = 0;

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        rankList = GetRankNvList(host, socket, ch, dimm);
        numRanks = CHIP_FUNC_CALL(host, NumRanksToTrain(host, socket, host->var.mem.socket[socket].maxRankDimm));
        for (rank = 0; rank < numRanks; rank++) {

          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          if ((*rankList)[rank].ctlIndex != ctlIndex) continue;

          ctlEnabled = 1;

          if (ctlLeft < ctlMargins[ch][(*rankList)[rank].rankIndex][1]) {
            ctlLeft = ctlMargins[ch][(*rankList)[rank].rankIndex][1];
          }

          if (ctlRight > ctlMargins[ch][(*rankList)[rank].rankIndex][0]) {
            ctlRight = ctlMargins[ch][(*rankList)[rank].rankIndex][0];
          }
        } // rank loop
      } // dimm loop

      if (ctlEnabled) {
        ctlOffset = (ctlRight + ctlLeft) / 2;
        CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex + CtlGrp0, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &ctlOffset, &minDelay, &maxDelay));
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Ctl group %2d, left edge = %3d - right edge = %3d offset == %3d width =%3d\n", ctlIndex, ctlLeft, ctlRight, ctlOffset, ctlRight-ctlLeft));
        host->var.mem.socket[socket].channelList[ch].ctlTrainingResults[ctlIndex].left = ctlLeft;
        host->var.mem.socket[socket].channelList[ch].ctlTrainingResults[ctlIndex].right = ctlRight;
      } // ctlEnabled

    } // ctlIndex loop
  } // ch loop

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    CHIP_FUNC_CALL(host, DisplayCCCResults(host, socket));
  }
#endif
  CHIP_FUNC_CALL(host, DisableChipLRDIMMMPROverrideMode(host, socket));
  host->var.mem.checkMappedOutRanks = 0;

  return SUCCESS;
} // EarlyCtlClk

/**

  Exectues control/clock point test

  @param host      - Pointer to sysHost
  @param socket    - Processor socket
  @param chBitmask - Bitmask of channels to test
  @param dimm      - DIMM to test
  @param rank      - Rank to test
  @param testFlags - Flags

  @retval N/A

**/
void
ExecuteCtlClkTest(
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chBitmask,
  UINT8     dimm,
  UINT8     rank,
  UINT8     testFlags
  )
{
  UINT8                                 ch;
  UINT8                                 strobe;
  UINT8                                 logRank;
  INT16                                 recEnOffset;
  UINT8                                 DumArr[7] = {1,1,1,1,1,1,1};
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct rankDevice                     (*rankStruct)[MAX_RANK_DIMM];
  EXECUTE_CTL_CLK_TEST_CHIP_STRUCT      executeCtlCLKTestChipStruct;

  channelNvList = GetChannelNvList(host, socket);

  if (testFlags & SETUP_FOR_RE_TEST) {
    //
    // Receive enable offset
    //
    recEnOffset = INITIAL_CHIP_REC_EN_OFFSET;

    //
    // Turn on MPR pattern for all ranks
    //
    SetAllRankMPR (host, socket, MR3_MPR);

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((chBitmask & (1 << ch)) == 0) continue;

      rankStruct  = GetRankStruct(host, socket, ch, dimm);

      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      //
      // Get the logical rank #
      //
      logRank = GetLogicalRank(host, socket, ch, dimm, rank);

      CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, 0));

      if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
        //
        // Enable Receive Enable Mode
        //
        (*rankStruct)[rank].MR0 &= ~MR0_DLL;
        WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR4 | MR4_RE, BANK4);
        (*rankStruct)[rank].MR0 |= MR0_DLL;

        if (!(host->var.common.emulation & (SOFT_SDV_FLAG | VP_FLAG | SIMICS_FLAG))) {
          executeCtlCLKTestChipStruct.dataControl0.Data = (*channelNvList)[ch].dataControl0;
          //executeCtlCLKTestChipStruct.dataControl0.Bits.longpreambleenable = 1;
          //TODO: Fix this
          executeCtlCLKTestChipStruct.dataControl0.Data |= BIT31;

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            CHIP_FUNC_CALL(host, EnableChipRecieveEnableMode(host, socket, ch, strobe, &executeCtlCLKTestChipStruct));
          } // strobe loop
        }

        CHIP_FUNC_CALL(host, EnableChipLRDIMMPreambleTrMode(host, socket, ch, dimm));
      }

      CHIP_FUNC_CALL(host, PerformChipODTMPRDimmActivationStep(host, socket, ch, dimm, &executeCtlCLKTestChipStruct));

      //
      // Chip specific IO latency step (Add 1 or subtract 1 from/to IO latency)
      //
      CHIP_FUNC_CALL(host, StepChipCtlCKIOLatency(host, socket, ch, dimm, rank, logRank));

      //
      // Add offset to rec en (currently chip specific initial value)
      //
      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, RecEnDelay, GSM_UPDATE_CACHE | GSM_WRITE_OFFSET,
                       &recEnOffset));

    } // ch loop

    CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, RECEIVE_ENABLE_BASIC));

    SetupRecEnFineTest(host, socket, chBitmask);
  } // Setup test

  if (testFlags & EXECUTE_RE_TEST) {
    //
    // Send a burst of 7 read commands back to back (4 DCLK apart) using MC_SPECIAL_COMMAND CRs
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));
  }

  if (testFlags & TEAR_DOWN_RE_TEST) {
    //
    // Receive enable offset
    //
    recEnOffset = TEARDOWN_CHIP_REC_EN_OFFSET;

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // Continue to the next rank if this one is disabled
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      CHIP_FUNC_CALL(host, DisableChipChlRecEnOffsetTrainingMode(host, socket, ch, &executeCtlCLKTestChipStruct));

      CHIP_FUNC_CALL(host, DisableChipLRDIMMPreambleTrMode(host, socket, ch, dimm));
      if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
        rankStruct    = GetRankStruct(host, socket, ch, dimm);

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        //
        // Enable Receive Enable Mode
        //
        (*rankStruct)[rank].MR0 &= ~MR0_DLL;
        WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR4, BANK4);
        (*rankStruct)[rank].MR0 |= MR0_DLL;

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          //
          //Enable Recieve Enable Mode using settings from the Channel NVlist
          //
          CHIP_FUNC_CALL(host, EnableChipRecieveEnableModeII(host, socket, ch, strobe, &executeCtlCLKTestChipStruct));

        } // strobe loop
      } // if DDR4

      //
      // Get the logical rank #
      //
      logRank = GetLogicalRank(host, socket, ch, dimm, rank);

      //
      // Restore IO latency
      //
      CHIP_FUNC_CALL(host, RestoreChipCtlCKIOLatency(host, socket, ch, dimm, rank, logRank));

      //
      // Restore rec en
      //
      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, RecEnDelay, GSM_UPDATE_CACHE | GSM_WRITE_OFFSET,
                       &recEnOffset));
    } // ch loop

    //
    // Notify IO to move out of RCVEN training mode
    //
    CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, CLEAR_MODE));

    //
    // Turn off MPR pattern for all ranks
    //
    SetAllRankMPR (host, socket, 0);
  }
} // ExecuteCtlClkTest

/**

  Get's the resutls from the current test

  @param host      - Pointer to sysHost
  @param dimm      - DIMM number
  @param rank      - rank number of the DIMM

  @retval N/A

**/
void
GetCtlClkResults (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm,
  UINT8     rank,
  UINT32    strobeFeedback[MAX_CH]
  )
{
  UINT8               ch;
  UINT8               strobe;
  UINT32              csrReg;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    strobeFeedback[ch] = 0;
    //Platforms require varying logic change for upper nibble
    for (strobe = 0; strobe < CHIP_GET_CTL_CLK_RES_MAX_STROBE; strobe++) {

      //Skip if non ecc
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      //
      //Platform specific step for lower nibble
      //
      CHIP_FUNC_CALL(host, SetChipGetCtlClkResLowerNibbleStrobeFeedback(host, socket, ch, strobe, &csrReg, &strobeFeedback[ch]));

      //Platform specific step for upper nibble
      CHIP_FUNC_CALL(host, SetChipGetCtlClkResUpperNibbleStrobeFeedback(strobe, csrReg, &strobeFeedback[ch]));
    } // strobe loop
  } // ch loop
} // GetCtlClkResults

/**

  Check to see if this DIMM properly supports parity

  @param host      - Pointer to sysHost
  @param dimm      - DIMM number

  @retval N/A

**/
UINT8
CheckParityWorkaround (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm
  )
{
  UINT8             parityWorkaround = 0;
  UINT16            dateCode;
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  dateCode = (*dimmNvList)[dimm].SPDModDate >> 8;
  dateCode |= (*dimmNvList)[dimm].SPDModDate << 8;

  parityWorkaround = CHIP_FUNC_CALL(host, CheckParityWorkaroundChipSpecific(host, socket, ch, dimm, dateCode));

  return parityWorkaround;
} // CheckParityWorkaround

void
GetChipBwSerrErrors (
  UINT32                       errMask,
  UINT8                        ch,
  UINT8                        errorResult[MAX_CH][CHIP_CMD_CLK_TEST_RANGE],
  INT16                        piIndex,
  INT16                        offset,
  PCHIP_GET_CMD_MGN_SWP_STRUCT chipGetCmdMgnSwpStruct
  )
{
  if (errMask) {
    chipGetCmdMgnSwpStruct->eyeSize[ch] = 0;
  } else {
    errorResult[ch][piIndex] = 0;
    chipGetCmdMgnSwpStruct->eyeSize[ch] += offset;
  }

  if (chipGetCmdMgnSwpStruct->eyeSize[ch] > 64) {
    chipGetCmdMgnSwpStruct->chEyemask &= ~(1 << ch);
  }
  if (((chipGetCmdMgnSwpStruct->chEyemask && (1 << ch)) == 0) && (errorResult[ch][piIndex] == 1)) {
    chipGetCmdMgnSwpStruct->chDonemask &= ~(1 << ch);
  }
}
