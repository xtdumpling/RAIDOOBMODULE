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
 *      IMC and DDR modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"


#ifdef MARGIN_CHECK
//
// Local Prototypes
//
void   EvaluateResults (PSYSHOST host, UINT8 socket, UINT8 dimm, UINT8 rank, UINT8 sign, INT16 offset[MAX_CH][MAX_RANK_CH],
         UINT32 chStatus, UINT8 scope, GSM_GT group, GSM_LT level, UINT16 mode, UINT8 searchUp[MAX_CH][MAX_RANK_CH], UINT32 bwSerr[][3], struct bitMask *foundPass,
         struct bitMask *foundFail, void *marginData, UINT32 *ranksDone, UINT8 maxSteps);
UINT8  CheckAllPassFail(PSYSHOST host, UINT8 found[MAX_STROBE/2]);
void   GetStartingOffset (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, GSM_LT level, GSM_GT group, UINT8 sign, INT16 offset[MAX_CH][MAX_RANK_CH], UINT8 *maxSteps);
void   UpdateStartingOffset (PSYSHOST host, UINT8 socket, GSM_GT group, GSM_LT level, UINT8 scope, void *marginData);
UINT32 *CheckRange(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, INT16 offset, UINT8 sign, GSM_LT level, GSM_GT group,
         UINT8 foundFail[MAX_STROBE/2], UINT8 foundPass[MAX_STROBE/2], UINT8 scope, INT16 previousOffset, void *marginData, UINT32 beyondRange[3]);
void   SaveThisMargin(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 maxStrobe, INT16 offset, UINT8 sign,
         UINT8 scope, UINT8 foundFail[MAX_STROBE/2], UINT8 foundPass[MAX_STROBE/2], void *marginData);
void   SetAllFound (PSYSHOST host, UINT8 ch, UINT8 rank, UINT8 strobe, UINT8 bitMask, UINT8 found[MAX_CH][MAX_RANK_CH][MAX_STROBE/2]);
void   ClearAllFound (PSYSHOST host, UINT8 ch, UINT8 rank, UINT8 strobe, UINT8 found[MAX_CH][MAX_RANK_CH][MAX_STROBE/2]);
void   ClearBitResults (PSYSHOST host, UINT8 socket, struct bitMargin *resultsBit);


/**

  Finds margins for various parameters per rank

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
RankMarginTool (
  PSYSHOST host
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               dllTest;
  UINT16              maxDllLoops;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
#ifdef SERIAL_DBG_MSG
  UINT64_STRUCT       startTsc  = { 0, 0 };
  UINT64_STRUCT       endTsc    = { 0, 0 };
  UINT32              mrcLatency = 0;
#endif
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct bitMask      filter;
  struct bitMargin    resultsBit;
#ifdef SERIAL_DBG_MSG
  UINT8 i;
  UINT8 dataMask[MAX_STROBE/2];
  UINT8               strobe;
#endif // SERIAL_DBG_MSG
  struct rankMargin   resultsRxDq;
  struct rankMargin   resultsTxDq;
  struct rankMargin   resultsRxVref;
  struct rankMargin   resultsTxVref;
  struct rankMargin   resultsCmdVref;
  struct rankMargin   resultsCmd;
  UINT32              chBitmask;
  struct rankMargin   resultsCtl;
#ifdef LRDIMM_SUPPORT
  struct subRankMargin resultsBacksideSubRankRxDq;
  struct subRankMargin resultsBacksideSubRankTxDq;
  struct subRankMargin resultsBacksideSubRankRxVref;
  struct subRankMargin resultsBacksideSubRankTxVref;
  struct subRankMargin resultsBacksideSubRankCmd;
  struct subRankMargin resultsBacksideSubRankCtl;
  struct subRankMargin resultsBacksideSubRankCaVref;
  UINT8                subRank;
#endif

  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast ||
      (host->var.mem.subBootMode == ColdBootFast && !(host->setup.mem.options & RMT_COLD_FAST_BOOT))) return SUCCESS;

  if (~host->memFlows & MF_RANK_MARGIN_EN) return SUCCESS;
  if((host->setup.mem.options & MARGIN_RANKS_EN) == 0) return SUCCESS;

  //
  // Return if this socket is disabled
  //
  socket = host->var.mem.currentSocket;
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  if (host->var.common.emulation & SIMICS_FLAG) {
    return SUCCESS;
  }

  host->var.mem.runningRmt = 1;

  // Issue a minor check point
  OutputCheckpoint (host, STS_CHANNEL_TRAINING, STS_RMT, 0);

  CHIP_FUNC_CALL(host, SetConfigBeforeRmt(host, socket));

  channelNvList = GetChannelNvList(host, socket);

  //
  // Clear the filter bits to enable error checking on every bit
  //
  ClearFilter(host, socket, &filter);

  if (host->setup.mem.dllResetTestLoops) {
    maxDllLoops = host->setup.mem.dllResetTestLoops * 2;
  } else {
    maxDllLoops = 1;
  }

  for (dllTest = 0; dllTest < maxDllLoops; dllTest++) {

#ifdef LRDIMM_SUPPORT
    host->var.mem.currentSubRank = 0;
    host->var.mem.maxSubRank = 1;
#endif

    for (ch = 0; ch < MAX_CH; ch++) {
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          // Do not look at more than two ranks for the third DIMM
          if ((dimm > 1) && (rank > 1)) continue;

          // Initialize to 0
          resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          resultsCmd.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsCmd.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          resultsCtl.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsCtl.rank[ch][(*rankList)[rank].rankIndex].p = 0;
#ifdef LRDIMM_SUPPORT
          if (IsBacksideCmdMarginEnabled (host, socket)) {
            if ((host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)) {
              host->var.mem.maxSubRank = MAX_SUBRANK;
            }
            for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
              resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
            }
          }

          if ((CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)))&& (host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->setup.mem.enableBacksideRMT)) {
            if ((host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)) {
              host->var.mem.maxSubRank = MAX_SUBRANK;
            }
            for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
              resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
            }
          }
#endif
        } // rank loop
      } // dimm loop
    } // ch loop

#ifdef SERIAL_DBG_MSG
    ReadTsc64 (&startTsc);
#endif  // SERIAL_DBG_MSG

    if( (host->setup.mem.options & MARGIN_RANKS_EN) && (host->setup.mem.rmtPatternLength > 4000 )){
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "RMT Pattern length %d\n",
                      host->setup.mem.rmtPatternLength));
    }

    if ((host->setup.mem.options & MARGIN_RANKS_EN) && (host->setup.mem.rmtPatternLengthExt > 4000 )) {
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "CMD Pattern length %d\n",
                      host->setup.mem.rmtPatternLengthExt));
    }

    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Getting margins:\n"));
    DeNormalizeCCC(host, socket, DENORMALIZE);

    //
    // Setup CADB for ColdFastBoot margin tests
    //
    if (host->var.mem.subBootMode == ColdBootFast){
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          chBitmask = GetChBitmask (host, socket, dimm, rank);
          CHIP_FUNC_CALL(host, SetupIOTestBasicVA(host, socket, chBitmask, 1, NSOE, 0, 0, 10));
        }
      }
    }

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CMD\n"));
    GetMargins (host, socket, DdrLevel, CmdAll, MODE_VA_DESELECT, SCOPE_RANK, &filter, (void *)&resultsCmd,
                host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);

    if (host->setup.mem.optionsExt & CMD_VREF_EN) {
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CMDVref\n"));
      GetMargins (host, socket, DdrLevel, CmdVref, MODE_VA_DESELECT, SCOPE_RANK, &filter, (void *)&resultsCmdVref,
                host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);
    }

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CTL\n"));
    GetMargins (host, socket, DdrLevel, CtlAll, MODE_VA_DESELECT, SCOPE_RANK, &filter, (void *)&resultsCtl,
                host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);

    DeNormalizeCCC(host, socket, RENORMALIZE);

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxDq\n"));
    GetMargins (host, socket, DdrLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&resultsTxDq,
                host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxVref\n"));
    GetMargins (host, socket, DdrLevel, TxVref, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&resultsTxVref,
                host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);

    //
    // Do not recenter if we are testing DLL reset
    //
    host->var.mem.runningRmt = 0;
    if (host->setup.mem.dllResetTestLoops != 0) {
      //
      // Issue a DLL reset after the first half of testing
      //
      if(dllTest >= host->setup.mem.dllResetTestLoops) {
        IssueDllReset(host, socket);
      }
    }
    host->var.mem.runningRmt = 1;

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxDqs\n"));
    GetMargins (host, socket, DdrLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&resultsRxDq,
                host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxVref\n"));
    GetMargins (host, socket, DdrLevel, RxVref, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&resultsRxVref,
                host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);

#ifdef LRDIMM_SUPPORT

    // DDR4 RDIMM RCD Rev 2 backside Cmd, Ctl margins
    if (IsBacksideCmdMarginEnabled (host, socket)) {

      for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
        host->var.mem.currentSubRank = subRank;
        MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Get Backside Command margins:\n"));
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "BACKSIDE_CMD\n"));
        GetMargins (host, socket, LrbufLevel, CmdAll, MODE_VA_DESELECT, SCOPE_RANK, &filter, (void *)&(resultsBacksideSubRankCmd.subRank[subRank]),
                    host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "BACKSIDE_CTL\n"));
        GetMargins (host, socket, LrbufLevel, CtlAll, MODE_VA_DESELECT, SCOPE_RANK, &filter, (void *)&(resultsBacksideSubRankCtl.subRank[subRank]),
                    host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "BACKSIDE_CMD_VREF\n"));
        GetMargins (host, socket, LrbufLevel, CmdVref, MODE_VA_DESELECT, SCOPE_RANK, &filter, (void *)&(resultsBacksideSubRankCaVref.subRank[subRank]),
                    host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);
      }
      host->var.mem.currentSubRank = 0;
    }

    if ((host->nvram.mem.socket[socket].lrDimmPresent) && (host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->setup.mem.enableBacksideRMT)) {
      for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
        host->var.mem.currentSubRank = subRank;
        MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Get Backside Data margins:\n"));
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxDqs\n"));
        GetMargins (host, socket, LrbufLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&(resultsBacksideSubRankRxDq.subRank[subRank]),
                    host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxVref\n"));
        GetMargins (host, socket, LrbufLevel, RxVref, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&(resultsBacksideSubRankRxVref.subRank[subRank]),
                    host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxVref\n"));
        GetMargins (host, socket, LrbufLevel, TxVref, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&(resultsBacksideSubRankTxVref.subRank[subRank]),
                    host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxDqs\n"));
        GetMargins (host, socket, LrbufLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_RANK, &filter, (void *)&(resultsBacksideSubRankTxDq.subRank[subRank]),
                    host->setup.mem.rmtPatternLength, 1, 0, 0, WDB_BURST_LENGTH);
      }
      host->var.mem.currentSubRank = 0;
    }
#endif

#ifdef SERIAL_DBG_MSG
    ReadTsc64 (&endTsc);
    mrcLatency = TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_S);
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "\nRMT time            %d ms\n", mrcLatency));
    if (checkMsgLevel(host, SDBG_MEM_TRAIN))    getPrintFControl (host);
#endif //SERIAL_DBG_MSG

    if (host->setup.mem.dllResetTestLoops) {
      if (dllTest == 0) {
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "START_DLL_RESET_TEST_NO_DLL_RESET\n"));
      } else if (dllTest == host->setup.mem.dllResetTestLoops) {
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "START_DLL_RESET_TEST_DLL_RESET\n"));
      }
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "DLL test loop = %d\n", dllTest % host->setup.mem.dllResetTestLoops));
    }

    //
    // Display units for each parameter
    //
    if ((dllTest == 0) || (dllTest == host->setup.mem.dllResetTestLoops)) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\nRxDqs = 1/64th QCLK\nRxVref = 4.68 mV\nTxDq = 1/64th QCLK\nTxVref = 7.8 mV\nCmd = 1/64th QCLK\n"));
      if (host->setup.mem.optionsExt & CMD_VREF_EN) {
        if (!(CHIP_FUNC_CALL(host, CaVrefSelect(host)) & 0x8)) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CmdVref = 10 mV\nCmdVref FMC = 6.25 mV\n"));
        } else {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CmdVref = 6.25 mV\nCmdVref FMC = 6.25 mV\n"));
        }
      }
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Ctl = 1/64th QCLK\n\n"));
    }

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) {
      rcPrintf ((host, "START_RMT_N%d\n", socket));

      if (host->setup.mem.optionsExt & CMD_VREF_EN) {
        rcPrintf ((host, "\n             RxDqs-   RxDqs+     RxV-     RxV+    TxDq-    TxDq+     TxV-     TxV+     Cmd-     Cmd+    CmdV-    CmdV+     Ctl-     Ctl+"));
        PrintLine(host, 138, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      } else {
        rcPrintf ((host, "\n             RxDqs-   RxDqs+     RxV-     RxV+    TxDq-    TxDq+     TxV-     TxV+     Cmd-     Cmd+     Ctl-     Ctl+"));
        PrintLine(host, 118, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }

    }
#endif //SERIAL_DBG_MSG
    channelNvList = GetChannelNvList(host, socket);

    //
    // Loop for each channel
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      //
      // Loop for each dimm and each rank
      //
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        //
        // Skip if no DIMM present
        //
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
          //
          // Skip if no rank
          //
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          if (resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].n >
              resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].p) {
            resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].n = 0;
            resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          }
          if (resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].n >
              resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].p) {
            resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].n = 0;
            resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          }
          if (resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].n >
              resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].p) {
            resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].n = 0;
            resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          }
          if (resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].n >
              resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].p) {
            resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].n = 0;
            resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          }
          if (resultsCmd.rank[ch][(*rankList)[rank].rankIndex].n >
              resultsCmd.rank[ch][(*rankList)[rank].rankIndex].p) {
            resultsCmd.rank[ch][(*rankList)[rank].rankIndex].n = 0;
            resultsCmd.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          }
          if (host->setup.mem.optionsExt & CMD_VREF_EN) {
            if (resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].n >
                resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].p) {
              resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].n = 0;
              resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].p = 0;
            }
          }

          if (resultsCtl.rank[ch][(*rankList)[rank].rankIndex].n >
              resultsCtl.rank[ch][(*rankList)[rank].rankIndex].p) {
            resultsCtl.rank[ch][(*rankList)[rank].rankIndex].n = 0;
            resultsCtl.rank[ch][(*rankList)[rank].rankIndex].p = 0;
          }

          (*rankList)[rank].rxDQLeft = (UINT8)resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].n;
          (*rankList)[rank].rxDQRight = (UINT8)resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].p;
          (*rankList)[rank].txDQLeft = (UINT8)resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].n;
          (*rankList)[rank].txDQRight = (UINT8)resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].p;
          (*rankList)[rank].rxVrefLow = (UINT8)resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].n;
          (*rankList)[rank].rxVrefHigh = (UINT8)resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].p;
          (*rankList)[rank].txVrefLow = (UINT8)resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].n;
          (*rankList)[rank].txVrefHigh = (UINT8)resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].p;
          (*rankList)[rank].cmdLeft = (UINT8)resultsCmd.rank[ch][(*rankList)[rank].rankIndex].n;
          (*rankList)[rank].cmdRight = (UINT8)resultsCmd.rank[ch][(*rankList)[rank].rankIndex].p;
          if (host->setup.mem.optionsExt & CMD_VREF_EN) {
            (*rankList)[rank].cmdLow = (UINT8)(0 - resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].n);
            (*rankList)[rank].cmdHigh = (UINT8)resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].p;
          }

          (*rankList)[rank].ctlLeft = (UINT8)resultsCtl.rank[ch][(*rankList)[rank].rankIndex].n;
          (*rankList)[rank].ctlRight = (UINT8)resultsCtl.rank[ch][(*rankList)[rank].rankIndex].p;

          if (host->setup.mem.optionsExt & CMD_VREF_EN) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "   %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d\n",
              resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].n, resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].n, resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].n, resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].n, resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsCmd.rank[ch][(*rankList)[rank].rankIndex].n, resultsCmd.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].n, resultsCmdVref.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsCtl.rank[ch][(*rankList)[rank].rankIndex].n, resultsCtl.rank[ch][(*rankList)[rank].rankIndex].p));
          } else {
           MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "   %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d\n",
              resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].n, resultsRxDq.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].n, resultsRxVref.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].n, resultsTxDq.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].n, resultsTxVref.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsCmd.rank[ch][(*rankList)[rank].rankIndex].n, resultsCmd.rank[ch][(*rankList)[rank].rankIndex].p,
              resultsCtl.rank[ch][(*rankList)[rank].rankIndex].n, resultsCtl.rank[ch][(*rankList)[rank].rankIndex].p));
          }
        } // rank loop
      } // dimm loop
    } // ch loop

    if (host->setup.mem.optionsExt & PER_BIT_MARGINS) {
      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Get per bit margins:\n"));
      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxDqs\n"));
      ClearBitResults(host, socket, &resultsBit);
      GetMargins (host, socket, DdrLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                  host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
      DisplayPerBitMargins (host, socket, &resultsBit, RxDqsDelay, DdrLevel);

      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxDq\n"));
      ClearBitResults(host, socket, &resultsBit);
      GetMargins (host, socket, DdrLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                  host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
      DisplayPerBitMargins (host, socket, &resultsBit, TxDqDelay, DdrLevel);

      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxVref\n"));
      ClearBitResults(host, socket, &resultsBit);
      GetMargins (host, socket, DdrLevel, RxVref, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                  host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
      DisplayPerBitMargins (host, socket, &resultsBit, RxVref, DdrLevel);

      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxVref\n"));
      ClearBitResults(host, socket, &resultsBit);
      GetMargins (host, socket, DdrLevel, TxVref, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                  host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
      DisplayPerBitMargins (host, socket, &resultsBit, TxVref, DdrLevel);

    } // if per bit margins

#ifdef LRDIMM_SUPPORT
    //------------------------------------------
    // DDR4 RDIMM RCD Rev 2 backside Cmd, Ctl margins
    if (IsBacksideCmdMarginEnabled (host, socket)) {
      //
      // Display units for each parameter
      //
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n\nCmd = 1/32nd QCLK\nCtl = 1/32nd QCLK\nCmdVref = 10 mV\nCmdVref FMC = 6.25mV\n\n"));
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MINMAX)) {
        rcPrintf ((host, "\nRCD Rev2 BACKSIDE         Cmd-   Cmd+   Ctl-   Ctl+  CmdV-  CmdV+"));
        PrintLine(host, 70, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }
#endif //SERIAL_DBG_MSG
      //
      // Loop for each channel
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        //
        // Loop for each dimm and each rank
        //
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          //
          // Skip if not DDR4 RDIMM Register rev 2
          //
          if (!IsDdr4RegisterRev2(host, socket, ch, dimm) && !((*dimmNvList)[dimm].aepDimmPresent && (host->setup.mem.enableNgnBcomMargining))) continue;

          rankList = GetRankNvList(host, socket, ch, dimm);
          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
              if ((host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 0) && (subRank > 0)) continue;

              if (resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n >
                  resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p) {
                resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
            }

              if (resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n >
                  resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p) {
                resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              }

              if (resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n >
                  resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p) {
                resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              }

              if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1) {
                MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                               "SubRank %d:  %3d    %3d    %3d    %3d    %3d    %3d\n",
                  subRank,
                  resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p));
              } else {
                MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                               "             %3d    %3d    %3d    %3d    %3d    %3d\n",
                  resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankCmd.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankCtl.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankCaVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p));
              }
            } //subrank
          } //rank
        } //dimm
      } //ch

      if (host->setup.mem.optionsExt & PER_BIT_MARGINS) {

        for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
          host->var.mem.currentSubRank = subRank;
          if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)
            MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "Get Backside Sub-Rank %d per bit margins:\n", subRank));

          MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\nRCD Rev2 BS CMD per bit\n"));
          ClearBitResults(host, socket, &resultsBit);
          GetMargins (host, socket, LrbufLevel, CmdAll, MODE_VA_DESELECT | MODE_START_ZERO, SCOPE_BIT, &filter, (void *)&resultsBit,
            host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);
          DisplayPerBitMargins (host, socket, &resultsBit, CmdAll, LrbufLevel);

          MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\nRCD Rev2 BS CTL per bit\n"));
          ClearBitResults(host, socket, &resultsBit);
          GetMargins (host, socket, LrbufLevel, CtlAll, MODE_VA_DESELECT | MODE_START_ZERO, SCOPE_BIT, &filter, (void *)&resultsBit,
            host->setup.mem.rmtPatternLengthExt, 1, 0, 0, WDB_BURST_LENGTH);
          DisplayPerBitMargins (host, socket, &resultsBit, CtlAll, LrbufLevel);
        }
        host->var.mem.currentSubRank = 0;
      } //per bit
    } // DDR4 RDIMM RCD rev 2 backside
    //------------------------------------------

    if ((host->nvram.mem.socket[socket].lrDimmPresent) && (host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->setup.mem.enableBacksideRMT)) {
      //
      // Display units for each parameter
      //
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n\nRxDqs = 1/32nd QCLK\nRxVref = 7.8 mV\nTxDq = 1/32nd QCLK\nTxVref = 7.8 mV\n"));
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MINMAX)) {
        rcPrintf ((host, "\nLRDIMM BACKSIDE      RxDqs-   RxDqs+     RxV-     RxV+    TxDq-    TxDq+     TxV-     TxV+"));
        PrintLine(host, 91, SINGLE_LINE, CRLF_FLAG, CRLF_FLAG);
      }
#endif //SERIAL_DBG_MSG
      //
      // Loop for each channel
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        //
        // Loop for each dimm and each rank
        //
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          //
          // Skip if no DIMM present
          //
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          if (!CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm)))continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
            //
            // Skip if no rank
            //
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
              if ((host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 0) && (subRank > 0)) continue;
              if ((((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING) == SPD_3DS_TYPE) && (subRank > 0)) continue;
              if (resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n >
                  resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p) {
                resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              }

              if (resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n >
                  resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p) {
                resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              }

              if (resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n >
                  resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p) {
                resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              }

              if (resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n >
                  resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p) {
                resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n = 0;
                resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p = 0;
              }

              if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1) {
                MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                               "SubRank %d: %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d\n",
                  subRank,
                  resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p));
              } else {
                MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                               "           %3d      %3d      %3d      %3d      %3d      %3d      %3d      %3d\n",
                  resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankRxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankRxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankTxDq.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p,
                  resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].n,
                  resultsBacksideSubRankTxVref.subRank[subRank].rank[ch][(*rankList)[rank].rankIndex].p));
              }
            } //subrank
          } //rank
        } //dimm
      } //ch

      if (host->setup.mem.optionsExt & PER_BIT_MARGINS) {
        for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
          host->var.mem.currentSubRank = subRank;
          if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)
            MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "Get Backside Sub-Rank %d per bit margins:\n", subRank));

          MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxDqs\n"));
          ClearBitResults(host, socket, &resultsBit);
          GetMargins (host, socket, LrbufLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                      host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
          DisplayPerBitMargins (host, socket, &resultsBit, RxDqsDelay, LrbufLevel);

          MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxDq\n"));
          ClearBitResults(host, socket, &resultsBit);
          GetMargins (host, socket, LrbufLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                      host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
          DisplayPerBitMargins (host, socket, &resultsBit, TxDqDelay, LrbufLevel);

          MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "RxVref\n"));
          ClearBitResults(host, socket, &resultsBit);
          GetMargins (host, socket, LrbufLevel, RxVref, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                      host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
          DisplayPerBitMargins (host, socket, &resultsBit, RxVref, LrbufLevel);

          MemDebugPrint((host, SDBG_MEM_TRAIN, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "TxVref\n"));
          ClearBitResults(host, socket, &resultsBit);
          GetMargins (host, socket, LrbufLevel, TxVref, MODE_VIC_AGG, SCOPE_BIT, &filter, (void *)&resultsBit,
                      host->setup.mem.rmtPatternLength, 0, 0, 0, WDB_BURST_LENGTH);
          DisplayPerBitMargins (host, socket, &resultsBit, TxVref, LrbufLevel);
        }
        host->var.mem.currentSubRank = 0;
      } // if per bit margins
    } //ddr4 lrdimm

#endif //LRDIMM_SUPPORT

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) {
      rcPrintf ((host, "STOP_RMT_N%d\n", socket));
      releasePrintFControl (host);
    }
#endif  // SERIAL_DBG_MSG
  } // dllTest loop

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MINMAX) && (host->setup.mem.options & DISPLAY_EYE_EN)) {

    if (checkMsgLevel(host, SDBG_MINMAX) && (host->setup.mem.options & PER_NIBBLE_EYE_EN)) {

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        // Clear the dataMask
        for (i = 0; i < MAX_STROBE/2; i++) {
          dataMask[i] = 0;
        }
        // Select the nibble
        // strobe 0  - dq[3:0]
        // strobe 9  - dq[7:4]
        // strobe 1  - dq[11:8]
        // strobe 10 - dq[15:12]
        // etc...
        if (strobe >= 9) {
          dataMask[strobe - 9] = 0x0F;
          MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "** Eyes for DQ[%d:%d] **\n", (strobe-9)*8+7, (strobe-9)*8+4));
        } else {
          dataMask[strobe] = 0xF0;
          MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "** Eyes for DQ[%d:%d] **\n", strobe*8+3, strobe*8));
        }

        // Run the test
        EyeDiagram(host, socket, DdrLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
        EyeDiagram(host, socket, DdrLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);

#ifdef LRDIMM_SUPPORT
        if ((host->nvram.mem.socket[socket].lrDimmPresent) && (host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->setup.mem.enableBacksideRMT)) {
          for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
            host->var.mem.currentSubRank = subRank;
            //if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)
            //  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            //                 "\nDisplay Backside Sub-Rank %d Eye Diagrams\n", subRank));
            EyeDiagram(host, socket, LrbufLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
            EyeDiagram(host, socket, LrbufLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
          }
          host->var.mem.currentSubRank = 0;
        }

        //------------------------------------------
        // DDR4 RDIMM RCD Rev 2 backside Cmd, Ctl margins
        if (IsBacksideCmdMarginEnabled (host, socket)) {
          for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
            host->var.mem.currentSubRank = subRank;
            //if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)
            //  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            //                 "\nDisplay Backside Sub-Rank %d Eye Diagrams\n", subRank));
            EyeDiagram(host, socket, LrbufLevel, CmdAll, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
          }
          host->var.mem.currentSubRank = 0;
        }
#endif
      } // strobe loop

    } else { //not PER_NIBBLE_EYE_EN

      // Clear the dataMask
      for (i = 0; i < MAX_STROBE/2; i++) {
        dataMask[i] = 0;
      }

      EyeDiagram(host, socket, DdrLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
      //EyeDiagram(host, socket, DdrLevel, RxDqsPDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
      //EyeDiagram(host, socket, DdrLevel, RxDqsNDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
      EyeDiagram(host, socket, DdrLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);

#ifdef LRDIMM_SUPPORT
      if ((host->nvram.mem.socket[socket].lrDimmPresent) && (host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->setup.mem.enableBacksideRMT)) {
        for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
          host->var.mem.currentSubRank = subRank;
          //if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)
          //  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          //                 "\nDisplay Backside Sub-Rank %d Eye Diagrams\n", subRank));
          EyeDiagram(host, socket, LrbufLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
          EyeDiagram(host, socket, LrbufLevel, TxDqDelay, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
        }
        host->var.mem.currentSubRank = 0;
      }
      //------------------------------------------
      // DDR4 RDIMM RCD Rev 2 backside Cmd, Ctl margins
      if (IsBacksideCmdMarginEnabled (host, socket)) {
        for (subRank = 0; subRank < host->var.mem.maxSubRank; subRank++) {
          host->var.mem.currentSubRank = subRank;
          //if (host->nvram.mem.socket[socket].channelList[ch].encodedCSMode == 1)
          //  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          //                 "\nDisplay Backside Sub-Rank %d Eye Diagrams\n", subRank));
          EyeDiagram(host, socket, LrbufLevel, CmdAll, MODE_VIC_AGG, SCOPE_RANK, RMT_PATTERN_LENGTH, 0, 0, dataMask);
        }
        host->var.mem.currentSubRank = 0;
      }
#endif
    }
  }
#endif  // SERIAL_DBG_MSG
  host->var.mem.runningRmt = 0;

#ifdef SERIAL_DBG_MSG
  // Call the function to offset final training results -> customer request - BDX HSD s4987017.
  // Will also be calling RMT a second time after training offsets have been modified only on
  // a cold boot to help the consumer to recognize and quantify the change; callingTrngOffstCfgOnce 0 by default
  if ((host->var.mem.callingTrngOffstCfgOnce != 1) && (host->setup.mem.trainingResultOffsetFunctionEnable)) {
    rcPrintf ((host,"\n Training Result Offset Configuration"));
    host->var.mem.callingTrngOffstCfgOnce = 1;
    TrainingResultOffsetCfg(host);
  } //trainingResultOffsetFunctionEnable
#endif // SERIAL_DBG_MSG

  return SUCCESS;
} // RankMarginTool
#endif // MARGIN_CHECK


/**

  Clears bit margin results structure

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
VOID
ClearBitResults (
  PSYSHOST host,
  UINT8    socket,
  struct bitMargin  *resultsBit
  )
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               bit;
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  //
  // Clear margin data
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        // Do not look at more than two ranks for the third DIMM
        if ((dimm > 1) && (rank > 1)) continue;

        // Initialize to 0
        for (bit = 0; bit < 72; bit++) {
          resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].n = 0;
          resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p = 0;
        }
      } // rank loop
    } // dimm loop
  } // ch loop
} //ClearBitResults


/**

  Clears GetMargins filter

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param filter  - pointer to filter buffer

  @retval N/A

**/
void
ClearFilter (
  PSYSHOST        host,
  UINT8           socket,
  struct bitMask  *filter
  )
{
  UINT8               ch;
  UINT8               strobe;
  UINT8               dimm;
  UINT8               rank;
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  // Setup bitMask
  for (ch = 0; ch < MAX_CH; ch++) {
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        // Do not look at more than two ranks for the third DIMM
        if ((dimm > 1) && (rank > 1)) continue;

        for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
          filter->bits[ch][(*rankList)[rank].rankIndex][strobe] = 0;
        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

} // ClearFilter

/**

  Displays per bit margin results

  @param host  - Pointer to sysHost
  @param socket      - current socket
  @param resultsBit  - Data to compare against
  @param group       - Command group

  @retval N/A

**/
void
DisplayPerBitMargins (
  PSYSHOST          host,
  UINT8             socket,
  struct bitMargin  *resultsBit,
  GSM_GT            group,
  GSM_LT            level
  )
{
#ifdef SERIAL_DBG_MSG
  UINT8               ch;
  UINT8               bit;
  UINT8               b;
  UINT8               maxBits;
  UINT8               dimm;
  UINT8               rank;
  UINT8               skipRow;
  INT16               margin;
  INT16               marginStart = 0;
  INT16               marginStop = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  //
  // Check the debug message level
  //
  if (!checkMsgLevel(host, SDBG_MINMAX)) return;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Loop for each channel
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    //
    // Loop for each dimm and each rank
    //
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;

        MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "Per bit margins: "));
        if (group == RxDqsDelay) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "RxDqs\n"));
          marginStart = MAX_TIMING_OFFSET - 1;
          marginStop = -(MAX_TIMING_OFFSET - 1);
        } else if (group == RxDqsPDelay) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Rx DqsP\n"));
          marginStart = MAX_TIMING_OFFSET - 1;
          marginStop = -(MAX_TIMING_OFFSET - 1);
        } else if (group == RxDqsNDelay) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Rx DqsN\n"));
          marginStart = MAX_TIMING_OFFSET - 1;
          marginStop = -(MAX_TIMING_OFFSET - 1);

        } else if (group == CmdAll) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Cmd\n"));
          marginStart = MAX_TIMING_OFFSET - 1;
          marginStop = -(MAX_TIMING_OFFSET - 1);

        } else if (group == CtlAll) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "Ctl\n"));
          marginStart = MAX_TIMING_OFFSET - 1;
          marginStop = -(MAX_TIMING_OFFSET - 1);

        } else if (group == TxDqDelay) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "TxDq\n"));
          marginStart = MAX_TIMING_OFFSET - 1;
          marginStop = -(MAX_TIMING_OFFSET - 1);
        } else if (group == RxVref) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "RxVref\n"));
          marginStart = MAX_RD_VREF_OFFSET - 1;
          marginStop = -(MAX_RD_VREF_OFFSET - 1);
        } else if (group == TxVref) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "TxVref\n"));
          if (host->setup.mem.optionsExt & LRDIMM_BACKSIDE_VREF_EN) {
            if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
              marginStart = MAX_WR_VREF_OFFSET_DDRT - 1;
              marginStop = -(MAX_WR_VREF_OFFSET_DDRT - 1);
            } else {
              marginStart = MAX_WR_VREF_OFFSET_BACKSIDE - 1;
              marginStop = -(MAX_WR_VREF_OFFSET_BACKSIDE - 1);
            }
          } else {
            marginStart = MAX_WR_VREF_OFFSET - 1;
            marginStop = -(MAX_WR_VREF_OFFSET - 1);
          }
        }
        rcPrintf ((host, "     0------7 8-----15 16----23 24----31 32----39 40----47 48----55 56----63"));
        if (host->nvram.mem.eccEn) {
          rcPrintf ((host, " 64----71\n"));
        } else {
          rcPrintf ((host, "\n"));
          }
        for (margin = marginStart; margin >= marginStop; margin--) {
          //
          // Skip row if no failures
          //
          skipRow = 1;
          for (bit = 0; bit < MAX_BITS; bit++) {
            if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;
            if (margin >= 0) {
              if (margin >= resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p) {
                skipRow = 0;
                break;
              }
            } else {
              if (margin <= resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].n) {
                skipRow = 0;
                break;
              }
            }

          } // bit loop

          if (skipRow) continue;

          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "%3d ", margin));

          for (bit = 0; bit < MAX_BITS; bit++) {
            // Only display ECC bits if ECC is enabled
            if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;

            // Insert a space after every 8 bits
            if ((bit % 8) == 0) rcPrintf ((host, " "));

            // Check for pass or fail for each bit at this margin
            if (margin >= 0) {
              if (margin >= resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p) {
                MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "*"));
              } else {
                MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              " "));
              }
            } else {
              if (margin <= resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].n) {
                MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "*"));
              } else {
                MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              " "));
              }
            }
          } // bit loop

          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "\n"));
        } // margin loop

        if (group == RxDqsDelay) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "RxDqs"));
        } else if (group == RxDqsPDelay) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "RxDqsP"));
        }else if (group == RxDqsNDelay) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "RxDqsN"));
        } else if (group == TxDqDelay) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "TxDq"));
        } else if (group == RxVref) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "RxVref"));
        } else if (group == TxVref) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "TxVref"));
        } else if (group == CmdAll) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "Cmd"));
        } else if (group == CtlAll) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                            "Ctl"));
        }
        rcPrintf ((host, " - Per bit margins\n"));

        if (host->nvram.mem.eccEn == 0) {
          maxBits = MAX_BITS - 8;
        } else {
          maxBits = MAX_BITS;
        }
        for (bit = 0; bit < maxBits; bit++) {
          if ((bit == 0) || (bit == (maxBits / 2))) {
            for(b = bit; b < (bit + (maxBits / 2)); b++) {
              MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            "  %2d", b));
            }
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "\n"));
          }

          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        " %3d", resultsBit->bits[ch][(*rankList)[rank].rankIndex][bit].p));

          if ((bit == ((maxBits / 2) - 1)) || (bit == (maxBits - 1))) {
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "\n"));
            for(b = bit - ((maxBits / 2) - 1); b <= bit; b++) {
              MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            " %3d", resultsBit->bits[ch][(*rankList)[rank].rankIndex][b].n));
            }
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "\n\n"));
          }
        } // bit loop
        MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            "\n"));
      } // rank loop
    } // dimm loop
  } // ch loop
#endif // SERIAL_DBG_MSG
} // DisplayPerBitMargins

void
DisplayREandLEMargins (
  PSYSHOST            host,
  UINT8               socket,
  UINT8               ch,
  UINT8               dimm,
  UINT8               rank,
  struct bitMarginCh  *resultsBit,
  GSM_GT              group
  )
{
#ifdef SERIAL_DBG_MSG
  UINT8               bit;
  UINT8               b;
  UINT8               maxBits;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  //
  // Check the debug message level
  //
  if (!checkMsgLevel(host, SDBG_MEM_TRAIN)) return;

  dimmNvList = GetDimmNvList(host, socket, ch);

  //
  // Skip if no DIMM present
  //
  if ((*dimmNvList)[dimm].dimmPresent) {

    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n"));
    if (group == RxDqsDelay) {
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "RxDqs"));
    } else if (group == RxDqsPDelay) {
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "RxDqsP"));
    }else if (group == RxDqsNDelay) {
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "RxDqsN"));
    } else if (group == TxDqDelay) {
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "TxDq"));
    } else if (group == RxVref) {
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "RxVref"));
    } else if (group == TxVref) {
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "TxVref"));
    }
    rcPrintf ((host, " - Per bit margins\n"));

    if (host->nvram.mem.eccEn == 0) {
      maxBits = MAX_BITS - 8;
    } else {
      maxBits = MAX_BITS;
    }
    for (bit = 0; bit < maxBits; bit++) {
      if ((bit == 0) || (bit == (maxBits / 2))) {
        for(b = bit; b < (bit + (maxBits / 2)); b++) {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "  %2d", b));
        }
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n"));
      }

      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    " %3d", resultsBit->bits[ch][bit].p));

      if ((bit == ((maxBits / 2) - 1)) || (bit == (maxBits - 1))) {
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n"));
        for(b = bit - ((maxBits / 2) - 1); b <= bit; b++) {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        " %3d", resultsBit->bits[ch][b].n));
        }
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n\n"));
      }
    } // bit loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "\n"));
  }

#endif // SERIAL_DBG_MSG
} // DisplayREandLEMargins

/**

  Tests to see if all ranks across all memory controllers are done.

  @param RanksDone - Array of bits showing if ranks are done. If bitX = 1, the rank is done.

  @retval TRUE is ranks are all done
  @retval FALSE if any rank is not done.

**/
BOOLEAN
AreRanksDone (
  UINT32  RanksDone[MAX_IMC]
)
{
  UINT8 i;
  for (i = 0; i < MAX_IMC; i++) {
    if (RanksDone[i] != 0xFFFFFFFF) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Calculates positive and negative margin for the group provided

  @param host           - Pointer to sysHost
  @param socket         -  Processor socket to check
  @param level          - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group          - Parameter to be margined
                         RxDqsDelay    : Margin Rx DqDqs
                         TxDqDelay     : Margin Tx DqDqs
                         RxVref         : Margin Rx Vref
                         TxVref         : Margin Tx Vref
                         RecEnDelay    : Margin Receive Enable
                         WrLvlDelay    : Margin Write Level
                         CmdGrp0        : Margin CMD group 0
                         CmdGrp1        : Margin CMD group 1
                         CmdGrp2        : Margin CMD group 2
                         CmdAll         : Margin all CMD groups
  @param mode           - Test mode to use
                         MODE_XTALK            BIT0: Enable crosstalk (placeholder)
                         MODE_VIC_AGG          BIT1: Enable victim/aggressor
                         MODE_START_ZERO       BIT2: Start at margin 0
                         MODE_POS_ONLY         BIT3: Margin only the positive side
                         MODE_NEG_ONLY         BIT4: Margin only the negative side
                         MODE_DATA_MPR         BIT5: Enable MPR Data pattern
                         MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern
                         MODE_DATA_LFSR        BIT7: Enable Data LFSR
                         MODE_ADDR_LFSR        BIT8: Enable Address LFSR
                         MODE_ADDR_CMD0_SETUP  BIT9
                         MODE_CHECK_PARITY     BIT10: Enable parity checking
                         MODE_DESELECT         BIT11: Enable deselect patterns
                         MODE_VA_DESELECT      BIT12: Enable Victim - Aggressor deselect patterns
  @param scope          - Margin granularity
                         SCOPE_SOCKET      0: Margin per processor socket
                         SCOPE_CH        1: Margin per channel
                         SCOPE_DIMM      2: Margin per DIMM
                         SCOPE_RANK      3: Margin per rank
                         SCOPE_STROBE    4: Margin per strobe group
                         SCOPE_BIT       5: Margin per bit
  @param mask           - Mask of bits to exclude from testing
  @param marginData     - Pointer to the structure to store the margin results. The structure type varies based on scope.
  @param patternLength  - RankMarginTest Pattern length
  @param update         - Update starting margins so the next pass will run faster or not
  @param chMask         - Bit Mask of channels to not be used
  @param rankMask       - Bit Mask of ranks to not be used
  @param burstLength    - Number of cachelines to test for each write/read operation
                       It is the repeat count for the WR/RD subsequence

  @retval N/A
**/
void
GetMargins (
           PSYSHOST        host,
           UINT8           socket,
           GSM_LT          level,
           GSM_GT          group,
           UINT16          mode,
           UINT8           scope,
           struct bitMask  *mask,
           void            *marginData,
           UINT32          patternLength,
           UINT8           update,
           UINT8           chMask,
           UINT8           rankMask,
           UINT8           burstLength
           )
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               r;
  UINT8               strobe;
  UINT8               bit;
  UINT8               sign;
  UINT8               byteIndex;
  UINT8               maxSign;
  UINT8               maxSteps = 0;
  UINT8               faultyBits;
  UINT8               retryCount;
  UINT32              beyondRangeMask[MAX_CH][3];
  INT16               offset[MAX_CH][MAX_RANK_CH];
  INT16               previousOffset[MAX_CH][MAX_RANK_CH];
  INT16               restoreOffset[MAX_CH][MAX_RANK_CH];
  UINT32              chAllFail = 0;
  UINT32              chStatus;
  UINT8               searchUp[MAX_CH][MAX_RANK_CH];
  UINT32              ranksDone[MAX_IMC];
  UINT8               imc;
  UINT32              status;
  UINT32              bwSerr[MAX_CH][3];
  struct bitMask      foundPass;
  struct bitMask      foundFail;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct              baseMargin  *socketMarginCopy;
  struct              chMargin    *channelMarginCopy;
  struct rankMargin   *rankMarginCopy;
  struct strobeMargin *strobeMarginCopy;
  struct bitMargin    *bitMarginCopy;
  UINT16               cmdMinVal = MAX_CMD_MARGIN;
  UINT16               cmdMaxVal = 0;

  OutputExtendedCheckpoint((host, STS_GET_MARGINS, group, socket));

  disableAllMsg(host);

  channelNvList = GetChannelNvList(host, socket);

  if (mode & MODE_POS_ONLY) {
    // Only check the positive side
    sign = 0;
    maxSign = 1;
  } else if (mode & MODE_NEG_ONLY) {
    // Only check the negative side
    sign = 1;
    maxSign = 2;
  } else {
    // Check both positive and negative
    sign = 0;
    maxSign = 2;
  }

  if (CheckCMDGroup(group) || CheckCTLGroup(group)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      // Check if this channel is masked off
      if (chMask & (1 << ch)) continue;
      GetMarginsHook (host, socket, ch, 1);
    } // ch loop
  }

  // Margin both sides
  for (; sign < maxSign; sign++) {

    //
    // Make room for Backside Cmd hold margining
    //
    if (((group == CmdAll) || (group == CtlAll)) && (level == LrbufLevel) && sign) {
      BacksideShift(host, socket, RENORMALIZE);
    }

    // Clear pass/fail status
    ClearAllFound(host, SET_ALL, SET_ALL, SET_ALL, foundPass.bits);
    ClearAllFound(host, SET_ALL, SET_ALL, SET_ALL, foundFail.bits);

    // Initialize to no failures
    for (imc = 0; imc < MAX_IMC; imc++) {
      ranksDone[imc] = 0xFFFFFFFF;
    }

    // Get the offset to start with
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      // Check if this channel is masked off
      if (chMask & (1 << ch)) continue;

      imc = CHIP_FUNC_CALL(host, GetMCID(host, socket, ch));

      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

          // skip if this rank is disabled
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          // skip rank training if we don't need to do it. example cmd training on RDIMM where rank>0
          if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;

          // Do not test this rank if it is masked out
          if (rankMask & (1 << GetLogicalRank(host, socket, ch, dimm, rank))) continue;

          GetStartingOffset (host, socket, ch, dimm, rank, level, group, sign, offset, &maxSteps);
          if (CheckCMDGroup(group)) {
            GetSetCmdGroupDelayCore (host, socket, ch, dimm, level, group, GSM_READ_ONLY, &restoreOffset[ch][(*rankList)[rank].rankIndex], &cmdMinVal, &cmdMaxVal);
          } else if (CheckCTLGroup(group)) {
            GetSetCtlGroupDelayCore (host, socket, ch, dimm, level, group, GSM_READ_ONLY, &restoreOffset[ch][(*rankList)[rank].rankIndex], &cmdMinVal, &cmdMaxVal);
          } else {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, ALL_BITS, level, group, GSM_READ_ONLY, &restoreOffset[ch][(*rankList)[rank].rankIndex]));
          }
          if (mode & MODE_START_ZERO) {
            offset[ch][(*rankList)[rank].rankIndex] = 0;
          }
          //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
          //        "Starting Offset = %d, maxSteps = %d\n", offset[ch][(*rankList)[rank].rankIndex], maxSteps));

          // Indicate this rank is not done
          ranksDone[imc] &= ~(1 << ((*rankList)[rank].rankIndex + (CHIP_FUNC_CALL(host, GetMCCh(imc, ch) * 8))));

          previousOffset[ch][(*rankList)[rank].rankIndex] = 0;
          searchUp[ch][(*rankList)[rank].rankIndex] = 1;

          //
          // Ignore nibbles/bytes that have previously failed
          //
          byteIndex = 0;
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            if ((*rankList)[rank].faultyParts[strobe]) {
              if ((*dimmNvList)[dimm].x4Present) {
                if (strobe < 9) {
                  faultyBits = 0xF;
                } else {
                  faultyBits = 0xF0;
                }
              } else {
                faultyBits = 0xFF;
              }

              if (strobe < 9) {
                byteIndex = strobe;
              } else {
                byteIndex = strobe - 9;
              }

              SetAllFound(host, ch, (*rankList)[rank].rankIndex, byteIndex, faultyBits, foundPass.bits);
              SetAllFound(host, ch, (*rankList)[rank].rankIndex, byteIndex, faultyBits, foundFail.bits);
              mask->bits[ch][(*rankList)[rank].rankIndex][byteIndex] |= faultyBits;
            }
          } // strobe loop
        } // rank loop
      } // dimm loop
    } // ch loop

    // Loop through the read Vref points
    chStatus = 0;

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      getPrintFControl (host);

      rcPrintf ((host, "\nsign = %d\n", sign));
      rcPrintf ((host, "Data bits -> 0------7 8-----15 16----23 24----31 32----39 40----47 48----55 56----63"));
      if (host->nvram.mem.eccEn) {
        rcPrintf ((host, " 64----71\n"));
      } else {
        rcPrintf ((host, "\n"));
      }

      releasePrintFControl (host);
    }
#endif  // SERIAL_DBG_MSG
    // Loop until we either find an edge or reach the end of our range
    while (!AreRanksDone (ranksDone)) {

      if ((scope == SCOPE_SOCKET) || (scope == SCOPE_CH)) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          // Check if this channel is masked off
          if (chMask & (1 << ch)) continue;

          //
          // Clear the per bit mask registers
          //
          CHIP_FUNC_CALL(host, ClearBWErrorStatus (host, socket, ch));

          // Initialize error status to 0
          bwSerr[ch][0] = 0;
          bwSerr[ch][1] = 0;
          bwSerr[ch][2] = 0;
        } // ch loop
      }

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        if (scope == SCOPE_DIMM) {
          for (ch = 0; ch < MAX_CH; ch++) {
            if ((*channelNvList)[ch].enabled == 0) continue;
            // Check if this channel is masked off
            if (chMask & (1 << ch)) continue;

            //
            // Clear the per bit mask registers
            //
            CHIP_FUNC_CALL(host, ClearBWErrorStatus (host, socket, ch));

            // Initialize error status to 0
            bwSerr[ch][0] = 0;
            bwSerr[ch][1] = 0;
            bwSerr[ch][2] = 0;
          } // ch loop
        }

        // Loop for each rank
        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

          chStatus = 0xFF;

          chAllFail = chStatus;

          for (ch = 0; ch < MAX_CH; ch++) {
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            imc = CHIP_FUNC_CALL(host, GetMCID(host, socket, ch));
            // Check if this channel is masked off
            if (chMask & (1 << ch)) continue;

            // Continue if this rank is not enabled
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;

            // Do not test this rank if it is masked out
            if (rankMask & (1 << GetLogicalRank(host, socket, ch, dimm, rank))) continue;

            rankList = GetRankNvList(host, socket, ch, dimm);

            // Continue if this rank is done
            if (ranksDone[imc] & (1 << ((*rankList)[rank].rankIndex + (CHIP_FUNC_CALL(host, GetMCCh(imc, ch)) * 8)))) continue;

            dimmNvList = GetDimmNvList(host, socket, ch);

            // Clear the bit for this channel if this rank has not failed yet or has not yet passed
            if (CheckAllPassFail(host, foundFail.bits[ch][(*rankList)[rank].rankIndex]) ||
                CheckAllPassFail(host, foundPass.bits[ch][(*rankList)[rank].rankIndex])) {
              chStatus &= ~(1 << ch);
            } else {
              ranksDone[imc] |= (1 << ((*rankList)[rank].rankIndex + (CHIP_FUNC_CALL(host, GetMCCh(imc, ch) * 8))));
            }

            //
            // Use shorter test timeout for AEP because there is a risk of the test engine hanging
            //
            if ((*dimmNvList)[dimm].aepDimmPresent && CheckCMDGroup(group)) {
              host->var.mem.socket[socket].testTimeoutSize = 1;
            }
          } // ch loop

          // If there are no channels to test reset channel status and go to the next rank
          if (chStatus == chAllFail) {
            for (ch = 0; ch < MAX_CH; ch++) {
              if ((*channelNvList)[ch].enabled == 0) continue;
              // Check if this channel is masked off
              if (chMask & (1 << ch)) continue;

              // skip if this rank is disabled
              if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

              // skip rank training if we don't need to do it. example cmd training on RDIMM where rank>0
              if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;

              rankList = GetRankNvList(host, socket, ch, dimm);

              //
              // Restore the new offset to test
              //
              if (previousOffset[ch][(*rankList)[rank].rankIndex]) {
                RestoreOffset(host, socket, ch, dimm, rank, restoreOffset[ch][(*rankList)[rank].rankIndex], level, group);

                dimmNvList = GetDimmNvList(host, socket, ch);

                if (((CheckCMDGroup(group) || CheckCTLGroup(group)) && !(((*dimmNvList)[dimm].aepDimmPresent) && (group == CmdVref)))
                   || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref))) {
                  for (r = 0; r < MAX_RANK_CH; r++) {
                    previousOffset[ch][r] = 0;
                  }
                } else {
                  previousOffset[ch][(*rankList)[rank].rankIndex] = 0;
                }
              }
            } // ch loop

            chStatus = 0;
            continue;
          }

          // Set mask bits
          CHIP_FUNC_CALL(host, SetBwErrorMask(host, socket, dimm, rank, mask));

          for (ch = 0; ch < MAX_CH; ch++) {
            if ((*channelNvList)[ch].enabled == 0) continue;
            // Check if this channel is masked off
            if (chMask & (1 << ch)) continue;
            if ((~chStatus & (1 << ch)) == 0) continue;
            // skip if this rank is disabled
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            // skip rank training if we don't need to do it. example cmd training on RDIMM where rank>0
            if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;

            rankList = GetRankNvList(host, socket, ch, dimm);


            if ((scope == SCOPE_RANK) || (scope == SCOPE_STROBE)  || (scope == SCOPE_BIT)) {
              // Initialize error status to all fail
              bwSerr[ch][0] = (UINT32)-1;
              bwSerr[ch][1] = (UINT32)-1;
              bwSerr[ch][2] = (UINT32)-1;
            }

            // Set the new offset to test
            status = SetOffset(host, socket, ch, dimm, rank, offset[ch][(*rankList)[rank].rankIndex], level, group,
                               previousOffset[ch][(*rankList)[rank].rankIndex]);

            dimmNvList = GetDimmNvList(host, socket, ch);

            if (((CheckCMDGroup(group) || CheckCTLGroup(group)) && !(((*dimmNvList)[dimm].aepDimmPresent) && (group == CmdVref)))
                || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref))) {
              for (r = 0; r < MAX_RANK_CH; r++) {
                previousOffset[ch][r] = offset[ch][(*rankList)[rank].rankIndex];
              }
            } else {
              previousOffset[ch][(*rankList)[rank].rankIndex] = offset[ch][(*rankList)[rank].rankIndex];
            }

            // Check if we've reached the maximum or minimum in the allowable range
            CheckRange(host, socket, ch, dimm, rank, offset[ch][(*rankList)[rank].rankIndex], sign, level, group,
                                        foundFail.bits[ch][(*rankList)[rank].rankIndex], foundPass.bits[ch][(*rankList)[rank].rankIndex],
                                        scope, previousOffset[ch][(*rankList)[rank].rankIndex], marginData, beyondRangeMask[ch]);

            //
            // Skipping this check for groups that need to be restored to original value between margin steps.
            // This corrects an issue where these groups are not properly restored when CheckRange reaches a limit.
            //
            if (!(CheckCMDGroup(group) || CheckCTLGroup(group) || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref)))) {
              if (!(CheckAllPassFail(host, foundFail.bits[ch][(*rankList)[rank].rankIndex]) ||
                CheckAllPassFail(host, foundPass.bits[ch][(*rankList)[rank].rankIndex]))) {
                chStatus |= (1 << ch);          // disable channel
              }
            }
            if ((scope == SCOPE_RANK) || (scope == SCOPE_STROBE)  || (scope == SCOPE_BIT)) {
              //
              // Clear the per bit mask registers
              //
              CHIP_FUNC_CALL(host, ClearBWErrorStatus (host, socket, ch));
            }
          } // ch loop

          CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));

          retryCount = 0;

          do {
            // Run tests
            status = ExecuteTest (host, socket, ~chStatus, dimm, rank, group, mode, patternLength, burstLength);
            if (status == FAILURE){
              MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                             "ExecuteTest failed. Retrying...\n"));
              CHIP_FUNC_CALL(host, IO_Reset (host, socket));
              CHIP_FUNC_CALL(host, IODdrtReset(host, socket, dimm));
              CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
              JedecInitSequence (host, socket, CH_BITMASK);
              for (ch = 0; ch < MAX_CH; ch++) {
                if ((*channelNvList)[ch].enabled == 0) continue;
                CHIP_FUNC_CALL(host, RPQDrain(host, socket, ch, dimm, rank));
              } // ch loop
            } // if failure
          } while ((status == FAILURE) && (retryCount < 10));

          // Collect test results
          status = CollectTestResults (host, socket, ~chStatus, bwSerr);

          for (ch = 0; ch < MAX_CH; ch++) {
            if ((*channelNvList)[ch].enabled == 0) continue;

            dimmNvList = GetDimmNvList(host, socket, ch);

            bwSerr[ch][0] |= beyondRangeMask[ch][0];
            bwSerr[ch][1] |= beyondRangeMask[ch][1];
            bwSerr[ch][2] |= beyondRangeMask[ch][2];

            if ((*dimmNvList)[dimm].aepDimmPresent && CheckCMDGroup(group)) {
              host->var.mem.socket[socket].testTimeoutSize = 0;
            }
          } // ch loop

          if (CheckCMDGroup(group) || CheckCTLGroup(group) || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref))) {
            for (ch = 0; ch < MAX_CH; ch++) {
              if ((*channelNvList)[ch].enabled == 0) continue;
              // Check if this channel is masked off
              if (chMask & (1 << ch)) continue;
              if ((~chStatus & (1 << ch)) == 0) continue;
              rankList = GetRankNvList(host, socket, ch, dimm);

              //
              // Restore the new offset to test
              //
              if (previousOffset[ch][(*rankList)[rank].rankIndex]) {
                RestoreOffset(host, socket, ch, dimm, rank, restoreOffset[ch][(*rankList)[rank].rankIndex], level, group);

                dimmNvList = GetDimmNvList(host, socket, ch);
                if (((CheckCMDGroup(group) || CheckCTLGroup(group)) && !(((*dimmNvList)[dimm].aepDimmPresent) && (group == CmdVref)))
                   || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref))) {
                  for (r = 0; r < MAX_RANK_CH; r++) {
                    previousOffset[ch][r] = 0;
                  } // r loop
                } else {
                  previousOffset[ch][(*rankList)[rank].rankIndex] = 0;
                }
              }
            } // ch loop

            if (CheckCMDGroup(group) || CheckCTLGroup(group)) {
              if (status) {
                CHIP_FUNC_CALL(host, IO_Reset (host, socket));
                CHIP_FUNC_CALL(host, IODdrtReset(host, socket, dimm));
                CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
                JedecInitSequence (host, socket, CH_BITMASK);
                for (ch = 0; ch < MAX_CH; ch++) {
                  if ((*channelNvList)[ch].enabled == 0) continue;
                  CHIP_FUNC_CALL(host, RPQDrain(host, socket, ch, dimm, rank));
                }
              }
            }
            if (CheckCMDGroup(group) && status) {
              CHIP_FUNC_CALL(host, ClrPcheMiss (host, socket));
              CHIP_FUNC_CALL(host, SetAepTrainingMode (host, socket, LATE_CMD_CLK));
            }
          }

          if (status && (group == TxVref) && (host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->nvram.mem.txVrefDone) && (level == DdrLevel)) {
            CHIP_FUNC_CALL(host, IO_Reset (host, socket));
            CHIP_FUNC_CALL(host, IODdrtReset(host, socket, dimm));
            CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
            JedecInitSequence (host, socket, CH_BITMASK);
          }

          if ((scope == SCOPE_RANK) || (scope == SCOPE_STROBE)  || (scope == SCOPE_BIT)) {
            // Evaluate Test Results
            EvaluateResults (host, socket, dimm, rank, sign, offset, ~chStatus, scope, group, level, mode, searchUp, bwSerr, &foundPass,
                             &foundFail, marginData, ranksDone, maxSteps);
          }
        } // rank loop

        if (scope == SCOPE_DIMM) {
          // Evaluate Test Results
          EvaluateResults (host, socket, dimm, 0, sign, offset, ~chStatus, scope, group, level, mode, searchUp, bwSerr, &foundPass,
                           &foundFail, marginData, ranksDone, maxSteps);
        }
      } // dimm loop

      if ((scope == SCOPE_SOCKET) || (scope == SCOPE_CH)) {
        // Evaluate Test Results
        EvaluateResults (host, socket, 0, 0, sign, offset, ~chStatus, scope, group, level, mode, searchUp, bwSerr, &foundPass,
                         &foundFail, marginData, ranksDone, maxSteps);
      }

      if (AreRanksDone (ranksDone)) {
        if (CheckCMDGroup(group) || CheckCTLGroup(group) || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref))) {
          for (ch = 0; ch < MAX_CH; ch++) {
            if ((*channelNvList)[ch].enabled == 0) continue;
            if (chMask & (1 << ch)) continue;

            dimmNvList = GetDimmNvList(host, socket, ch);

            for (dimm = 0; dimm < MAX_DIMM; dimm++) {

              rankList = GetRankNvList(host, socket, ch, dimm);

              for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

                // skip if this rank is disabled
                if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

                // skip rank training if we don't need to do it. example cmd training on RDIMM where rank>0
                if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;

                // Do not test this rank if it is masked out
                if (rankMask & (1 << GetLogicalRank(host, socket, ch, dimm, rank))) continue;

                //
                // Restore the new offset to test
                //
                if (previousOffset[ch][(*rankList)[rank].rankIndex]) {
                  RestoreOffset(host, socket, ch, dimm, rank, restoreOffset[ch][(*rankList)[rank].rankIndex], level, group);

                  dimmNvList = GetDimmNvList(host, socket, ch);
                  if (((CheckCMDGroup(group) || CheckCTLGroup(group)) && !(((*dimmNvList)[dimm].aepDimmPresent) && (group == CmdVref)))
                     || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref))) {
                    for (r = 0; r < MAX_RANK_CH; r++) {
                      previousOffset[ch][r] = 0;
                    } // r loop
                  } else {
                    previousOffset[ch][(*rankList)[rank].rankIndex] = 0;
                  }
                }
              } // rank loop
            } // dimm loop
          } // ch loop

          if (CheckCMDGroup(group) || CheckCTLGroup(group)) {
            CHIP_FUNC_CALL(host, IO_Reset (host, socket));
            CHIP_FUNC_CALL(host, IODdrtResetAll(host, socket));
            CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
            JedecInitSequence (host, socket, CH_BITMASK);
          }
          if (CheckCMDGroup(group)) CHIP_FUNC_CALL(host, SetAepTrainingMode (host, socket, LATE_CMD_CLK));
        }
        if ((group == TxVref) && (host->nvram.mem.dramType != SPD_TYPE_DDR3) && (host->nvram.mem.txVrefDone) && (level == DdrLevel)) {
          CHIP_FUNC_CALL(host, IO_Reset (host, socket));
          CHIP_FUNC_CALL(host, IODdrtResetAll(host, socket));
          CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
          JedecInitSequence (host, socket, CH_BITMASK);
        }
      }
    } // offset loop
  } // sign loop

  //
  // Restore Backside Cmd delays to 0
  //
  if (((group == CmdAll) || (group == CtlAll)) && (level == LrbufLevel) && (sign == 2)) {
    BacksideShift(host, socket, DENORMALIZE);
  }

  //
  // clear bitwise error mask
  //
  CHIP_FUNC_CALL(host, ClearBwErrorMask(host, socket));

  if (CheckCMDGroup(group) || CheckCTLGroup(group)) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      // Check if this channel is masked off
      if (chMask & (1 << ch)) continue;
      CHIP_FUNC_CALL(host, GetMarginsHook (host, socket, ch, 0));
    } // ch loop
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // skip if this rank is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        // skip rank training if we don't need to do it. example cmd training on RDIMM where rank>0
        if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;

        // Do not test this rank if it is masked out
        if (rankMask & (1 << GetLogicalRank(host, socket, ch, dimm, rank))) continue;
        //
        // Restore the new offset to test
        //
        if (previousOffset[ch][(*rankList)[rank].rankIndex]) {
          RestoreOffset(host, socket, ch, dimm, rank, restoreOffset[ch][(*rankList)[rank].rankIndex], level, group);

          dimmNvList = GetDimmNvList(host, socket, ch);
          if (((CheckCMDGroup(group) || CheckCTLGroup(group)) && !(((*dimmNvList)[dimm].aepDimmPresent) && (group == CmdVref)))
             || ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref))) {
            for (r = 0; r < MAX_RANK_CH; r++) {
              previousOffset[ch][r] = 0;
            } // r loop
          } else {
            previousOffset[ch][(*rankList)[rank].rankIndex] = 0;
          }
        }
      } // rank loop
    } // dimm loop
  } // ch loop
  r = 1;
  //
  // Update starting offsets so the next pass will run faster
  //
  if (level == DdrLevel) {
    if (update) UpdateStartingOffset (host, socket, group, level, scope, marginData);

    //
    // For LRDIMM, just treat all host side ranks like rank 0, so copy rank 0 results to other ranks
    //

    if (host->nvram.mem.socket[socket].lrDimmPresent && (group == TxVref)) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          rank = 0;
          // skip if this rank is disabled
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          if (scope == SCOPE_STROBE) {
            for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
              if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group))) continue;
              strobeMarginCopy = marginData;
              for (strobe = 9; strobe < MAX_STROBE; strobe++) {
                (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].n = (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe - 9].n;
                (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].p = (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe - 9].p;
              } // strobe
            } // rank
          } // scope
        } // dimm loop
      } // ch loop
    }
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        rankList = GetRankNvList(host, socket, ch, dimm);
        for (rank = 1; rank < MAX_RANK_DIMM; rank++) {
          // skip if this rank is disabled
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          // skip rank training if we don't need to do it. example cmd training on RDIMM where rank>0
          if (CHIP_FUNC_CALL(host, CheckSkipRankTrain(host, socket, ch, dimm, rank, level, group)) == 0) continue;

          if (scope == SCOPE_RANK) {
            rankMarginCopy = marginData;
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,"Copy Rank from %d to %d, p = %d, n = %d\n",
                           (*rankList)[0].rankIndex,
                           (*rankList)[rank].rankIndex,
                           (*rankMarginCopy).rank[ch][(*rankList)[0].rankIndex].p,
                           (*rankMarginCopy).rank[ch][(*rankList)[0].rankIndex].n));
            (*rankMarginCopy).rank[ch][(*rankList)[rank].rankIndex].n = (*rankMarginCopy).rank[ch][(*rankList)[0].rankIndex].n;
            (*rankMarginCopy).rank[ch][(*rankList)[rank].rankIndex].p = (*rankMarginCopy).rank[ch][(*rankList)[0].rankIndex].p;
          } else if (scope == SCOPE_STROBE) {
            strobeMarginCopy = marginData;
            for (strobe = 0; strobe < MAX_STROBE; strobe++) {
              (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].n = (*strobeMarginCopy).strobe[ch][(*rankList)[0].rankIndex][strobe].n;
              (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].p = (*strobeMarginCopy).strobe[ch][(*rankList)[0].rankIndex][strobe].p;
              if (strobe > 8 && host->nvram.mem.socket[socket].lrDimmPresent && (group == TxVref)) {// duplicate the strobe values for power training for the Tx Vref case for each rank
                (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].n = (*strobeMarginCopy).strobe[ch][(*rankList)[0].rankIndex][strobe-9].n;
                (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].p = (*strobeMarginCopy).strobe[ch][(*rankList)[0].rankIndex][strobe-9].p;
              }
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,"Left %d, Right %d \n",
                             (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].p,
                             (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].n));
            }
          } else if (scope == SCOPE_BIT) {
            bitMarginCopy = marginData;
            for (bit = 0; bit < MAX_BITS; bit++) {
              (*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].n = (*bitMarginCopy).bits[ch][(*rankList)[0].rankIndex][bit].n;
              (*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].p = (*bitMarginCopy).bits[ch][(*rankList)[0].rankIndex][bit].p;
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, bit, "Left %d, Right %d \n",
                             (*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].p,
                             (*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].n));
            } // bit loop
          } // scope
        } // rank loop
      } // dimm loop
    } // ch loop
  }

  // Make sure if the margins are flipped, they are set to 0
  if (scope == SCOPE_SOCKET) {
    socketMarginCopy = marginData;
    if((*socketMarginCopy).n >= 0 && (*socketMarginCopy).p <= 0) {
      (*socketMarginCopy).n = 0;
      (*socketMarginCopy).p = 0;
    }
  } else {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if (scope == SCOPE_CH) {
        channelMarginCopy = marginData;
        if((*channelMarginCopy).channel[ch].n >= 0 && (*channelMarginCopy).channel[ch].p <= 0) {
          (*channelMarginCopy).channel[ch].n = 0;
          (*channelMarginCopy).channel[ch].p = 0;
        }
      } else {
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
            // skip if this rank is disabled
            if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
            if (scope == SCOPE_RANK || scope == SCOPE_DIMM) {
              rankMarginCopy = marginData;
              if((*rankMarginCopy).rank[ch][(*rankList)[rank].rankIndex].n >= 0 && (*rankMarginCopy).rank[ch][(*rankList)[rank].rankIndex].p <= 0) {
                (*rankMarginCopy).rank[ch][(*rankList)[rank].rankIndex].n = 0;
                (*rankMarginCopy).rank[ch][(*rankList)[rank].rankIndex].p = 0;
              }
            } else if (scope == SCOPE_STROBE) {
              strobeMarginCopy = marginData;
              for (strobe = 0; strobe < MAX_STROBE; strobe++) {
                if((*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].n >= 0 && (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].p <= 0) {
                  (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].n = 0;
                  (*strobeMarginCopy).strobe[ch][(*rankList)[rank].rankIndex][strobe].p = 0;
                }
              }
            } else if (scope == SCOPE_BIT) {
              bitMarginCopy = marginData;
              for (bit = 0; bit < MAX_BITS; bit++) {
                if((*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].n >= 0 && (*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].p <= 0) {
                  (*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].n = 0;
                  (*bitMarginCopy).bits[ch][(*rankList)[rank].rankIndex][bit].p = 0;
                }
              }
            } //scope
          } //rank
        } //dimm
      } // if/else scope = ch
    } //ch
  } // if/else scope = Socket

  if (CheckCMDGroup(group) || CheckCTLGroup(group)) {
    CHIP_FUNC_CALL(host, IO_Reset (host, socket));
    CHIP_FUNC_CALL(host, IODdrtResetAll(host, socket));
    CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
    JedecInitSequence (host, socket, CH_BITMASK);
  }

  //
  // Reset the IO
  //
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));

  restoreMsg(host);

} // GetMargins

/**

  Returns the starting offsets for the given test group

  @param host          - Pointer to sysHost
  @param socket        - Processor socket to check
  @param ch            - Current Channel
  @param dimm          - Dimm number (0-based)
  @param rank          - Rank number (0-based)
  @param level         - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group         - Test group
  @param sign          - The direction we are currently going (up, down, left or right)
  @param offset        - Pointer to offsets
  @param maxSteps      - Pointer to maximum number of steps

  @retval N/A

**/
void
GetStartingOffset (
                  PSYSHOST  host,
                  UINT8     socket,
                  UINT8     ch,
                  UINT8     dimm,
                  UINT8     rank,
                  GSM_LT    level,
                  GSM_GT    group,
                  UINT8     sign,
                  INT16     offset[MAX_CH][MAX_RANK_CH],
                  UINT8     *maxSteps
                  )
{
  struct  ddrRank       (*rankList)[MAX_RANK_DIMM];
  struct  dimmNvram     (*dimmNvList)[MAX_DIMM];

  rankList = GetRankNvList(host, socket, ch, dimm);
  dimmNvList = GetDimmNvList(host, socket, ch);
  // Get the offset to start with
#ifdef LRDIMM_SUPPORT
  if (level == LrbufLevel) {
    switch (group) {
    case RxVref:
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = -7;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = 7;
      }
      *maxSteps = 45;
      break;
    case TxVref:
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = -7;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = 7;
      }
      //*maxSteps = MAX_LRBUF_RD_VREF_OFFSET;
      if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
        *maxSteps = MAX_WR_VREF_OFFSET_BACKSIDE_DDRT;
      } else {
        *maxSteps = MAX_WR_VREF_OFFSET_BACKSIDE;
      }

      break;

    case RxDqsDelay:
    case RxDqsNDelay:
    case RxDqsPDelay:
    case TxDqDelay:
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = -5;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = 5;
      }
      *maxSteps = MAX_TIMING_OFFSET;
      break;
    case CmdAll:
    case CtlAll:
    case CmdVref:
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = -1;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = 1;
      }
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        *maxSteps = 127;
      }  else {
        *maxSteps = MAX_CMD_MARGIN;
      }
    default:
      break;
    } //switch group
  } else
#endif //LRDIMM
  {
    if (group == RxVref) {
      if (sign) {
        if ((host->var.mem.socket[socket].rdVrefLo == 0) || (host->var.mem.socket[socket].rdVrefLo >= MAX_RD_VREF_OFFSET)) {
          offset[ch][(*rankList)[rank].rankIndex] = -7;
        } else {
          offset[ch][(*rankList)[rank].rankIndex] = 0 - (INT8)host->var.mem.socket[socket].rdVrefLo;
        }
      } else {
        if ((host->var.mem.socket[socket].rdVrefHi == 0) || (host->var.mem.socket[socket].rdVrefHi >= MAX_RD_VREF_OFFSET)) {
          offset[ch][(*rankList)[rank].rankIndex] = 7;
        } else {
          offset[ch][(*rankList)[rank].rankIndex] = (INT8)host->var.mem.socket[socket].rdVrefHi;
        }
      }
      *maxSteps = MAX_RD_VREF_OFFSET;
    } else if (group == TxVref) {
      if (sign) {
        if ((host->var.mem.socket[socket].wrVrefLo == 0) || (host->var.mem.socket[socket].wrVrefLo >= MAX_RD_VREF_OFFSET)) {
          offset[ch][(*rankList)[rank].rankIndex] = -7;
        } else {
          offset[ch][(*rankList)[rank].rankIndex] = 0 - (INT8)host->var.mem.socket[socket].wrVrefLo;
        }
      } else {
        if ((host->var.mem.socket[socket].wrVrefHi == 0) || (host->var.mem.socket[socket].wrVrefHi >= MAX_RD_VREF_OFFSET)) {
          offset[ch][(*rankList)[rank].rankIndex] = 7;
        } else {
          offset[ch][(*rankList)[rank].rankIndex] = (INT8)host->var.mem.socket[socket].wrVrefHi;
        }
      }
      if (host->setup.mem.optionsExt & LRDIMM_BACKSIDE_VREF_EN) {
        *maxSteps = MAX_WR_VREF_OFFSET_BACKSIDE;
      } else {
        *maxSteps = MAX_WR_VREF_OFFSET;
      }
    } else if ((group == RxDqsDelay) || (group == RxDqsPDelay) || (group == RxDqsNDelay)){
      if (sign) {
        if ((*rankList)[rank].rxDQLeftSt >= MAX_TIMING_OFFSET) {
          (*rankList)[rank].rxDQLeftSt = MAX_TIMING_OFFSET - 5;
        }
        offset[ch][(*rankList)[rank].rankIndex] = (INT8)(0 - (INT8)(*rankList)[rank].rxDQLeftSt);
      } else {
        if ((*rankList)[rank].rxDQRightSt >= MAX_TIMING_OFFSET) {
          (*rankList)[rank].rxDQRightSt = MAX_TIMING_OFFSET - 5;
        }
        offset[ch][(*rankList)[rank].rankIndex] = (INT8)(*rankList)[rank].rxDQRightSt;
      }
      *maxSteps = MAX_TIMING_OFFSET;
    } else if (group == TxDqDelay) {
      if (sign) {
        if ((*rankList)[rank].txDQLeftSt >= MAX_TIMING_OFFSET) {
          (*rankList)[rank].txDQLeftSt = MAX_TIMING_OFFSET - 5;
        }
        offset[ch][(*rankList)[rank].rankIndex] = (INT8)(0 - (INT8)(*rankList)[rank].txDQLeftSt);
      } else {
        if ((*rankList)[rank].txDQRightSt >= MAX_TIMING_OFFSET) {
          (*rankList)[rank].txDQRightSt = MAX_TIMING_OFFSET - 5;
        }
        offset[ch][(*rankList)[rank].rankIndex] = (INT8)(*rankList)[rank].txDQRightSt;
      }
      *maxSteps = MAX_TIMING_OFFSET;
    } else if (CheckCMDGroup(group) && (group != CmdVref)){
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = -15;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = 15;
      }
      *maxSteps = MAX_CMD_MARGIN;
    } else if (group == CmdVref) {
      if (!(CHIP_FUNC_CALL(host, CaVrefSelect(host)) & 0x8) && (host->nvram.mem.dimmTypePresent == RDIMM) && !((*dimmNvList)[dimm].aepDimmPresent)) {
        if (sign) {
          offset[ch][(*rankList)[rank].rankIndex] = -5;
        } else {
          offset[ch][(*rankList)[rank].rankIndex] = 5;
        }
        *maxSteps = MAX_CMD_MARGIN;
      } else {
        if (sign) {
          if ((host->var.mem.socket[socket].cmdVrefLo == 0) || (host->var.mem.socket[socket].cmdVrefLo >= MEM_CHIP_POLICY_VALUE(host, maxCmdVref))){
            offset[ch][(*rankList)[rank].rankIndex] = -15;
          } else {
            offset[ch][(*rankList)[rank].rankIndex] = 0 - (INT8)host->var.mem.socket[socket].cmdVrefLo;
          }
        } else {
          if ((host->var.mem.socket[socket].cmdVrefHi == 0) || (host->var.mem.socket[socket].cmdVrefHi >= MEM_CHIP_POLICY_VALUE(host, maxCmdVref))){
            offset[ch][(*rankList)[rank].rankIndex] = 15;
          } else {
            offset[ch][(*rankList)[rank].rankIndex] = (INT8)host->var.mem.socket[socket].cmdVrefHi;
          }
        }
        *maxSteps = MEM_CHIP_POLICY_VALUE(host, maxCmdVref);
      }
    } else if (CheckCTLGroup(group)) {
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = -15;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = 15;
      }
      //*maxSteps = MAX_CMD_MARGIN;
      *maxSteps = 127;
    }
  }

} // GetStartingOffset

/**
  Returns the starting offset for the given test group

  @param host            - Pointer to sysHost
  @param socket          - Processor socket to check
  @param group           - Test group
  @param level           - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param scope           - Margin granularity
  @param marginData      - Pointer to the structure to store the margin results. The structure type varies based on scope.

  @retval N/A
**/
void
UpdateStartingOffset (
                     PSYSHOST  host,
                     UINT8     socket,
                     GSM_GT    group,
                     GSM_LT    level,
                     UINT8     scope,
                     void      *marginData
                     )
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT8               bit;
  INT16               pos;
  INT16               neg;
  struct rankMargin   *rankM;
  struct strobeMargin *strobeM;
  struct bitMargin    *bitM;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        pos = 0x7F;
        neg = -0x7F;

        if (scope == SCOPE_RANK) {
          rankM = marginData;

          if ((*rankM).rank[ch][(*rankList)[rank].rankIndex].p > 13) {
            pos = (*rankM).rank[ch][(*rankList)[rank].rankIndex].p - 3;
          } else {
            pos = 10;
          }
          if ((*rankM).rank[ch][(*rankList)[rank].rankIndex].n < -13) {
            neg = (*rankM).rank[ch][(*rankList)[rank].rankIndex].n + 3;
          } else {
            neg = -10;
          }
        } else if (scope == SCOPE_STROBE) {
          strobeM = marginData;

          //
          // Setup variables based on x4 or x8 DIMM
          //
          maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, group, level));

          for (strobe = 0; strobe < maxStrobe; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            if ((*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].n > (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].p)
              (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].n = 0;
            if ((*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].p > 13) {
              if (((*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].p - 3) < pos) {
                pos = (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].p - 3;
              }
            } else {
              pos = 10;
            }
            if ((*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].n < -13) {
              if (((*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].n + 3) > neg) {
                neg = (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].n + 3;
              }
            } else {
              neg = -10;
            }
          } // strobe loop
        } else if (scope == SCOPE_BIT) {
          bitM = marginData;
          rankList = GetRankNvList(host, socket, ch, dimm);

          for (bit = 0; bit < MAX_BITS; bit++) {
            if ((!host->nvram.mem.eccEn) && (bit > 63)) continue;

            if ((*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].p > 13) {
              if (((*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].p - 3) < pos) {
                pos = (*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].p - 3;
              }
            } else {
              pos = 10;
            }
            if ((*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].n < -13) {
              if (((*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].n + 3) > neg) {
                neg = (*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].n + 3;
              }
            } else {
              neg = -10;
            }
          } // bit loop
        } // scope

        if ((pos != 0x7F) && (neg != -0x7F)) {
          if (group == RxVref) {
            host->var.mem.socket[socket].rdVrefLo = (UINT8)(0 - neg);
            host->var.mem.socket[socket].rdVrefHi = (UINT8)pos;
          } else if (group == TxVref) {
            host->var.mem.socket[socket].wrVrefLo = (UINT8)(0 - neg);
            host->var.mem.socket[socket].wrVrefHi = (UINT8)pos;
          } else if ((group == RxDqsDelay) || (group == RxDqsPDelay) || (group == RxDqsNDelay)) {
            (*rankList)[rank].rxDQLeftSt = (UINT8)(0 - neg);
            (*rankList)[rank].rxDQRightSt = (UINT8)pos;
          } else if (group == TxDqDelay) {
            (*rankList)[rank].txDQLeftSt = (UINT8)(0 - neg);
            (*rankList)[rank].txDQRightSt = (UINT8)pos;
          } else if (group == CmdGrp0) {
            //offset[ch][(*rankList)[rank].rankIndex] = 0;
          }
        }
      } // rank loop
    } // dimm loop
  } // ch loop
} // UpdateStartingOffset

UINT8
CheckAllPassFail (
                 PSYSHOST  host,
                 UINT8     found[MAX_STROBE/2]
                 )
/**

  Checks to see if all the bits are set

  @param host        - Pointer to sysHost
  @param found       - Array of status bits

  @retval 0 - All set
  @retval 1 - Not all set

**/
{
  UINT8 strobe;

  for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
    if ((!host->nvram.mem.eccEn) && (strobe == 8)) continue;
    if (found[strobe] != 0xFF) return 1;
  } // strobe loop

  // Return 0 if all bits are set
  return 0;
} // CheckAllPassFail

void
SetAllFound (
            PSYSHOST  host,
            UINT8     ch,
            UINT8     rank,
            UINT8     strobe,
            UINT8     bitMask,
            UINT8     found[MAX_CH][MAX_RANK_CH][MAX_STROBE/2]
            )
/**
  Sets all the status bits

  @param host        - Pointer to sysHost
  @param ch          - Current Channel
  @param rank        - Current Rank
  @param strobe      - Current Strobe
  @param bitMask     - Value to save into the found array
  @param found       - Array of status bits

  @retval N/A

**/
{
  UINT8 maxCh;
  UINT8 maxRank;
  UINT8 startRank;
  UINT8 maxStrobe;
  UINT8 startStrobe;

  if (ch == SET_ALL) {
    ch = 0;
    maxCh = MAX_CH;
  } else {
    maxCh = ch + 1;
  }
  if (rank == SET_ALL) {
    startRank = 0;
    maxRank = MAX_RANK_CH;
  } else {
    startRank = rank;
    maxRank = rank + 1;
  }
  if (strobe == SET_ALL) {
    startStrobe = 0;
    maxStrobe = MAX_STROBE/2;
  } else {
    startStrobe = strobe;
    maxStrobe = strobe + 1;
  }

  for (; ch < maxCh; ch++) {
    for (rank = startRank; rank < maxRank; rank++) {
      for (strobe = startStrobe; strobe < maxStrobe; strobe++) {
        found[ch][rank][strobe] |= bitMask;
      } // strobe loop
    } // rank loop
  } // ch loop
} // SetAllFound

void
ClearAllFound (
              PSYSHOST  host,
              UINT8     ch,
              UINT8     rank,
              UINT8     strobe,
              UINT8     found[MAX_CH][MAX_RANK_CH][MAX_STROBE/2]
              )
/**

  Clears all the status bits

  @param host        - Pointer to sysHost
  @param ch          - Current Channel
  @param rank        - Current Rank
  @param strobe      - Current Strobe
  @param found       - Array of status bits

  @retval N/A

**/
{
  UINT8 maxCh;
  UINT8 maxRank;
  UINT8 startRank;
  UINT8 maxStrobe;
  UINT8 startStrobe;

  if (ch == SET_ALL) {
    ch = 0;
    maxCh = MAX_CH;
  } else {
    maxCh = ch + 1;
  }

  if (rank == SET_ALL) {
    startRank = 0;
    maxRank = MAX_RANK_CH;
  } else {
    startRank = rank;
    maxRank = rank + 1;
  }
  if (strobe == SET_ALL) {
    startStrobe = 0;
    maxStrobe = MAX_STROBE/2;
  } else {
    startStrobe = strobe;
    maxStrobe = strobe + 1;
  }

  for (; ch < maxCh; ch++) {
    for (rank = startRank; rank < maxRank; rank++) {

      for (strobe = startStrobe; strobe < maxStrobe; strobe++) {
        found[ch][rank][strobe] = 0;
      } // strobe loop
    } // rank loop
  } // ch loop
} // ClearAllFound

/**

  @param host            - Pointer to sysHost
  @param socket          - Processor socket to check
  @param ch              - Current Channel
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param offset          - Pointer to offset
  @param level           - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group           - Test group
  @param previousOffset  - Previous offset

  @retval SUCCESS
  @retval FAILURE

**/
UINT32
SetOffset(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT8     dimm,
         UINT8     rank,
         INT16     offset,
         GSM_LT    level,
         GSM_GT    group,
         INT16     previousOffset
         )
{
  UINT16  cmdMinVal = MAX_CMD_MARGIN;
  UINT16  cmdMaxVal = 0;
  UINT32  status = SUCCESS;
  INT16   piOffset;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SetOffset\n"));
  if ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref)){
    piOffset = offset - previousOffset;
    status = CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, ALL_BITS, level, group, GSM_FORCE_WRITE | GSM_WRITE_OFFSET
                              | GSM_UPDATE_CACHE, &piOffset));
  } else {
    switch (group) {
    case RxVref:
    case TxVref:
    case RxDqsDelay:
    case RxDqsPDelay:
    case RxDqsNDelay:
    case TxDqDelay:
    case RecEnDelay:
    case WrLvlDelay:
      status = CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, ALL_BITS, level, group, GSM_FORCE_WRITE | GSM_WRITE_OFFSET,
                                &offset));
      break;
    case CmdGrp0:
    case CmdGrp1:
    case CmdGrp2:
    case CmdGrp3:
    case CmdGrp4:
    case CmdGrp5:
    case CmdAll:
      piOffset = offset - previousOffset;
      status = GetSetCmdGroupDelayCore (host, socket, ch, dimm, level, group, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piOffset, &cmdMinVal, &cmdMaxVal);
      break;
    case CmdVref:
      piOffset = offset - previousOffset;
      status = GetSetCmdVrefCore (host, socket, ch, dimm, level, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piOffset);
      break;
    case CtlGrp0:
    case CtlGrp1:
    case CtlGrp2:
    case CtlGrp3:
    case CtlGrp4:
    case CtlGrp5:
    case CtlAll:
      piOffset = offset - previousOffset;
      status = GetSetCtlGroupDelayCore (host, socket, ch, dimm, level, group, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piOffset, &cmdMinVal, &cmdMaxVal);
      break;
    default:
      break;
    }
  }

  //
  // Reset the IO
  //
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));

  return status;
} // SetOffset

#ifdef SERIAL_DBG_MSG
/**

  Function to offset the final training results before entering the OS that would facilitate customer debug.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
TrainingResultOffsetCfg (
  PSYSHOST host
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  socket = host->var.mem.currentSocket;
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);

  // Change offsets to their actual values (-ve or +ve). Default = 0.
  // +ve starts at 0 --> for an offset of +7, enter 7; -ve starts at 100 --> for an offset of -5, enter 105
  if (host->setup.mem.offsetTxDq > 100) {
    host->setup.mem.offsetTxDq = 100 - host->setup.mem.offsetTxDq; //-ve
  } else if (host->setup.mem.offsetTxDq == 100) {
    host->setup.mem.offsetTxDq = 0;
  } //else offset is a +ve value as entered in setup

  if (host->setup.mem.offsetTxVref > 100) {
    host->setup.mem.offsetTxVref = 100 - host->setup.mem.offsetTxVref;
  } else if (host->setup.mem.offsetTxVref == 100) {
    host->setup.mem.offsetTxVref = 0;
  }

  if (host->setup.mem.offsetRxDq > 100) {
    host->setup.mem.offsetRxDq = 100 - host->setup.mem.offsetRxDq;
  } else if (host->setup.mem.offsetRxDq == 100) {
    host->setup.mem.offsetRxDq = 0;
  }

  if (host->setup.mem.offsetRxVref > 100) {
    host->setup.mem.offsetRxVref = 100 - host->setup.mem.offsetRxVref;
  } else if (host->setup.mem.offsetRxVref == 100) {
    host->setup.mem.offsetRxVref = 0;
  }

  if (host->setup.mem.offsetCmdAll > 100) {
    host->setup.mem.offsetCmdAll = 100 - host->setup.mem.offsetCmdAll;
  } else if (host->setup.mem.offsetCmdAll == 100) {
    host->setup.mem.offsetCmdAll = 0;
  }

  if (host->setup.mem.offsetCmdVref > 100) {
    host->setup.mem.offsetCmdVref = 100 - host->setup.mem.offsetCmdVref;
  } else if (host->setup.mem.offsetCmdVref == 100) {
    host->setup.mem.offsetCmdVref = 0;
  }

  if (host->setup.mem.offsetCtlAll > 100) {
    host->setup.mem.offsetCtlAll = 100 - host->setup.mem.offsetCtlAll;
  } else if (host->setup.mem.offsetCtlAll == 100) {
    host->setup.mem.offsetCtlAll = 0;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        //Change all parameters w.r.t. offsets across all ranks
        TrainingResultOffsetFunction (host, socket, ch, dimm, rank, host->setup.mem.offsetTxDq, DdrLevel, TxDqDelay);
        if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
          TrainingResultOffsetFunction (host, socket, ch, dimm, rank, host->setup.mem.offsetTxVref, DdrLevel, TxVref);
        };
        TrainingResultOffsetFunction (host, socket, ch, dimm, rank, host->setup.mem.offsetRxDq, DdrLevel, RxDqsDelay);
      }//rank
    }//dimm
  }//ch

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
      TrainingResultOffsetFunction (host, socket, ch, 0, 0, host->setup.mem.offsetTxVref, DdrLevel, TxVref);
    };

    //CMD/CTL offset functions called per ch
    TrainingResultOffsetFunction (host, socket, ch, 0, 0, host->setup.mem.offsetCmdAll, DdrLevel, CmdAll);
    TrainingResultOffsetFunction (host, socket, ch, 0, 0, host->setup.mem.offsetCmdVref, DdrLevel, CmdVref);
    TrainingResultOffsetFunction (host, socket, ch, 0, 0, host->setup.mem.offsetCtlAll, DdrLevel, CtlAll);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      //RxVref called per dimm
      TrainingResultOffsetFunction (host, socket, ch, dimm, 0, host->setup.mem.offsetRxVref, DdrLevel, RxVref);
    } //dimm
  } //ch

  //All offsets will be global and affect all ranks on all channels on all cpu's.
  rcPrintf ((host,"\n START_OS_OFFSET"));
  rcPrintf ((host,"\n Rank Margin Tool"));
  RankMarginTool(host);    //Call RMT a second time after training offsets have been modified.
  rcPrintf ((host,"\n STOP_OS_OFFSET"));

  rcPrintf ((host,"\n DisplayTrainResults"));
  DisplayTrainResults(host);

  rcPrintf ((host,"START_OS_OFFSET_LIMITS_TABLE\n"));
  rcPrintf ((host, "offsetTxDq = %d\n", host->setup.mem.offsetTxDq));
  rcPrintf ((host, "offsetRxDq = %d\n", host->setup.mem.offsetRxDq));
  rcPrintf ((host, "offsetTxVref = %d\n", host->setup.mem.offsetTxVref));
  rcPrintf ((host, "offsetRxVref = %d\n", host->setup.mem.offsetRxVref));
  rcPrintf ((host, "offsetCmdAll = %d\n", host->setup.mem.offsetCmdAll));
  rcPrintf ((host, "offsetCmdVref = %d\n", host->setup.mem.offsetCmdVref));
  rcPrintf ((host, "offsetCtlAll = %d\n", host->setup.mem.offsetCtlAll));
  rcPrintf ((host,"\n STOP_OS_OFFSET_LIMITS_TABLE"));

  return SUCCESS;
}//TrainingResultOffsetCfg

/**

  Function to offset the final training results before entering the OS that would facilitate customer debug.

  @param host            - Pointer to sysHost
  @param socket          - Processor socket to check
  @param ch              - Current Channel
  @param dimm            - Dimm number (0-based)
  @param rank            - Rank number (0-based)
  @param offset          - Pointer to offset
  @param level           - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group           - Test group

  @retval NULL
**/

void
TrainingResultOffsetFunction (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  INT16     offset,
  GSM_LT    level,
  GSM_GT    group
  )
{

 UINT16  cmdMinVal = MAX_CMD_MARGIN;
 UINT16  cmdMaxVal = 0;

  //Offsets can be programmed by calling the respective functions for each group
  switch (group) {
    case RxVref:
    case TxVref:
    case RxDqsDelay:
    case TxDqDelay:
      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, ALL_BITS, level, group, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &offset));
      break;
    case CmdAll:
      GetSetCmdGroupDelayCore (host, socket, ch, dimm, level, group, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &offset, &cmdMinVal, &cmdMaxVal);
      break;
    case CmdVref: //CmdVref is used for both CMD/CTL Vref
      GetSetCmdVrefCore (host, socket, ch, dimm, level, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &offset);
      break;
    case CtlAll:
      GetSetCtlGroupDelayCore (host, socket, ch, dimm, level, group,  GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &offset, &cmdMinVal, &cmdMaxVal);
      break;
    default:
      break;
  }//case

  //
  // Reset the IO
  //
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
} //TrainingResultOffsetFunction
#endif //SERIAL_DBG_MSG

UINT32 *
CheckRange(
          PSYSHOST  host,
          UINT8     socket,
          UINT8     ch,
          UINT8     dimm,
          UINT8     rank,
          INT16     offset,
          UINT8     sign,
          GSM_LT    level,
          GSM_GT    group,
          UINT8     foundFail[MAX_STROBE/2],
          UINT8     foundPass[MAX_STROBE/2],
          UINT8     scope,
          INT16     previousOffset,
          void      *marginData,
          UINT32    beyondRange[3]
          )
/**
  Check if we've reached the max or min for the current margin offset

  @param host           - Pointer to sysHost
  @param socket         - Processor socket to check
  @param ch             - Current Channel
  @param dimm           - Dimm number (0-based)
  @param rank           - Rank number (0-based)
  @param offset         - Pointer to offset
  @param sign           - The direction we are currently going (up, down, left or right)
  @param level          - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group          - Test group
  @param foundFail      - Bit mask marking passes
  @param foundPass      - Bit mask marking fails
  @param scope          - Margin granularity
  @param previousOffset - Previous offset
  @param marginData     - pointer to Margin data
  @param beyondRange    - pointer to the beyondRange array

  @retval Pointer to the beyondRange array

**/
{
  UINT8   strobe;
  UINT8   maxStrobe;
  UINT8   update;
  UINT8   loop;
  INT16   maxDelay = 0;
  INT16   minDelay = 0;
  UINT16  maxCmdDelay = 0;
  UINT16  minCmdDelay = 0;
  UINT16  usDelay = 0;
  INT16   delay = 0;
  INT16   piOffset;
  GSM_GT  group2;
  struct dimmNvram(*dimmNvList)[MAX_DIMM];

  beyondRange[0] = 0;
  beyondRange[1] = 0;
  beyondRange[2] = 0;

  if ((CheckCMDGroup(group) || CheckCTLGroup(group)) && (group != CmdVref)) {
    //
    // Calculate the new setting to be tested
    //
    minCmdDelay = MAX_CMD_MARGIN;
    maxCmdDelay = 0;
    if (CheckCMDGroup(group)) {
      GetSetCmdGroupDelayCore (host, socket, ch, dimm, level, group, GSM_READ_ONLY, &delay, &minCmdDelay, &maxCmdDelay);
    } else {
      GetSetCtlGroupDelayCore (host, socket, ch, dimm, level, group, GSM_READ_ONLY, &delay, &minCmdDelay, &maxCmdDelay);
    }

    //
    // Get min/max delay
    //
    if (level == LrbufLevel) {
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].aepDimmPresent) {
        minDelay = 0;
        maxDelay = 127;
      } else {
        if (sign) {
          minDelay = 0;
          maxDelay = 32;
        } else {
          minDelay = -1;
          maxDelay = 31;
        }
      }
    } else {
      minDelay = 0;
      maxDelay = MAX_CMD_MARGIN;
    }
    //
    // Flag to indicate if we need to update margins or not
    //
    update = 0;

    piOffset = offset - previousOffset;
    //
    // Check if the new setting is out of range
    //
    if (((INT16)(minCmdDelay + piOffset) <= minDelay) || ((maxCmdDelay + piOffset) >= maxDelay)) {
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        beyondRange[(strobe % 9) / 4] |= 0xF << (0x8 * ((strobe % 9) % 4) +0x4 * (strobe / 9));
      } // strobe loop
      //
      // Do not update if it has already failed
      //
      if (((foundFail[0] & 0xFF) != 0xFF) && ((foundPass[0] & 0xFF) == 0xFF)) {
        update = 1;

        for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
          foundFail[strobe] |= 0xFF;
        } // strobe loop
      }

      //
      // Update the margin if this is the first time it failed
      //
      if (update) {
        for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
          SaveThisMargin(host, socket, ch, dimm, rank, strobe, MAX_STROBE, offset, sign, scope, foundFail, foundPass, marginData);
        }
      }
    }

  } else if (CheckCMDGroup(group) && (group == CmdVref)) {
    dimmNvList = GetDimmNvList(host, socket, ch);
    if ((level == LrbufLevel && !((*dimmNvList)[dimm].aepDimmPresent)) || (level == DdrLevel && !(CHIP_FUNC_CALL(host, CaVrefSelect(host)) & 0x8) && (host->nvram.mem.dimmTypePresent == RDIMM))) {
      if (sign) {
        minDelay = -20;
        maxDelay = 1;
      } else {
        minDelay = -1;
        maxDelay = 20;
      }
    } else {
      minDelay = MEM_CHIP_POLICY_VALUE(host, minCmdVref);
      maxDelay = MEM_CHIP_POLICY_VALUE(host, maxCmdVref);
    }

    //
    // Calculate the new setting to be tested
    //
    minCmdDelay = MEM_CHIP_POLICY_VALUE(host, maxCmdVref);
    maxCmdDelay = 0;
    GetSetCmdVrefCore (host, socket, ch, dimm, level, GSM_READ_ONLY, &delay);
    //
    // Flag to indicate if we need to update margins or not
    //
    update = 0;
    piOffset = offset - previousOffset;
    //
    // Check if the new setting is out of range
    //
    if (((INT16)(delay + piOffset) <= minDelay) || ((delay + piOffset) >= maxDelay)) {
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        beyondRange[(strobe % 9) / 4] |= 0xF << (0x8 * ((strobe % 9) % 4) +0x4 * (strobe / 9));
      } // strobe loop

     //
     // Do not update if it has already failed
     //
      if (((foundFail[0] & 0xFF) != 0xFF) && ((foundPass[0] & 0xFF) == 0xFF)) {
        update = 1;
        for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
          foundFail[strobe] |= 0xFF;
        } // strobe loop
      }
     //
     // Update the margin if this is the first time it failed
     //
      if (update) {
        for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
          SaveThisMargin(host, socket, ch, dimm, rank, strobe, MAX_STROBE, offset, sign, scope, foundFail, foundPass, marginData);
        }
      }
    }
  } else {
    if ((group == RxDqsDelay) || (group == TxDqDelay) || (group == RecEnDelay) || (group == WrLvlDelay) ||
        (group == RxVref) || (group == TxVref) || (group == RxDqsPDelay) || (group == RxDqsNDelay)) {

      maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, group, level));

      //
      // Check each data group
      //
      for (strobe = 0; strobe < maxStrobe; strobe++) {
        if(group == RxDqsDelay && level == DdrLevel) {
          loop = 0;
          group2 = RxDqsPDelay;
        } else {
          loop = 1;
          group2 = group;
        }
        for(; loop < 2; loop++) {
          if(group == RxDqsDelay && level == DdrLevel && loop == 1) {
            group2 = RxDqsNDelay;
          }
          //
          // Flag to indicate if we need to update margins or not
          //
          update = 0;

          //
          // Calculate the new setting to be tested
          //
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, ALL_BITS, level, group2, GSM_READ_ONLY, &delay));
          delay = delay + offset;
          if ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group2 == TxVref)){
            delay = delay - previousOffset;
          }

          //
          // Get min/max delay
          //
          CHIP_FUNC_CALL(host, GetDataGroupLimits(host, level, group2, (UINT16 *)&minDelay, (UINT16 *)&maxDelay, &usDelay));
          CHIP_FUNC_CALL(host, UpdateDdrtGroupLimits(host, socket, ch, dimm, level, group2, (UINT16 *)&minDelay, (UINT16 *)&maxDelay));

          //
          // Check if the new setting is out of range
          //
          if ((delay <= minDelay) || (delay >= maxDelay)) {
            if (maxStrobe <= 9) {
              beyondRange[(strobe % 9) / 4] |= 0xFF << 0x8 * (strobe % 4);
            } else {
              beyondRange[(strobe % 9) / 4] |= 0xF << (0x8 * ((strobe % 9) % 4) + 0x4 * (strobe / 9));
            }
            if (strobe < 9) {
              if(maxStrobe <= 9) {  // Per byte
                if (((foundFail[strobe] & 0xFF) != 0xFF) && ((foundPass[strobe] & 0xFF) == 0xFF)) {
                  update = 1;
                  foundFail[strobe] = foundFail[strobe] | 0xFF;
                }
              } else { // Per nibble
                if (((foundFail[strobe] & 0x0F) != 0x0F) && ((foundPass[strobe] & 0x0F) == 0x0F)) {
                  update = 1;
                  foundFail[strobe] = foundFail[strobe] | 0x0F;
                }
              }
            } else {
              if (((foundFail[strobe - 9] & 0xF0) != 0xF0) && ((foundPass[strobe - 9] & 0xF0) == 0xF0)) {
                update = 1;
                foundFail[strobe - 9] = foundFail[strobe - 9] | 0xF0;
              }
            }
            //
            // Update the margin if this is the first time it failed
            //
            if (update) SaveThisMargin(host, socket, ch, dimm, rank, strobe, maxStrobe, offset, sign, scope, foundFail, foundPass, marginData);
          }
        }
      } // strobe loop
    }
  } // if CMD

  return &beyondRange[0];
} // CheckRange

/**
  Check if we've reached the max or min for the current offset

  @param host          - Pointer to sysHost
  @param socket        - Processor socket to check
  @param ch            - Current Channel
  @param dimm          - Dimm number (0-based)
  @param rank          - Rank number (0-based)
  @param strobe        - Current Strobe
  @param maxStrobe     - Maximum number of strobes
  @param offset        - Pointer to offset
  @param sign          - The direction we are currently going (up, down, left or right)
  @param scope         - Margin granularity
  @param marginData    - pointer to Margin data

**/
void
SaveThisMargin(
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT8     rank,
              UINT8     strobe,
              UINT8     maxStrobe,
              INT16     offset,
              UINT8     sign,
              UINT8     scope,
              UINT8     foundFail[MAX_STROBE/2],
              UINT8     foundPass[MAX_STROBE/2],
              void      *marginData
              )
{
  UINT8               bit;
  UINT8               Loop;
  struct baseMargin   *socketM;
  struct chMargin     *channelM;
  struct rankMargin   *rankM;
  struct strobeMargin *strobeM;
  struct bitMargin    *bitM;
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  if (scope == SCOPE_SOCKET) {
    socketM = marginData;

    // Save new offset at the failing point
    if (sign) (*socketM).n = offset;
    else (*socketM).p = offset;
  } else if (scope == SCOPE_CH) {
    channelM = marginData;

    // Save new offset at the failing point
    if (sign) (*channelM).channel[ch].n = offset;
    else (*channelM).channel[ch].p = offset;
  } else if (scope == SCOPE_DIMM) {
  } else if (scope == SCOPE_RANK) {
    rankM = marginData;
    rankList = GetRankNvList(host, socket, ch, dimm);

    // Save new offset at the failing point
    if (sign) (*rankM).rank[ch][(*rankList)[rank].rankIndex].n = offset;
    else (*rankM).rank[ch][(*rankList)[rank].rankIndex].p = offset;
  } else if (scope == SCOPE_STROBE) {
    strobeM = marginData;
    rankList = GetRankNvList(host, socket, ch, dimm);

    // Save new offset at the failing point
    if (sign) {
      (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].n = offset;
      if (maxStrobe <= 9 && strobe < 9) {
        (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe + 9].n = offset;
      }
    } else {
      (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe].p = offset;
      if (maxStrobe <= 9 && strobe < 9) {
        (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobe + 9].p = offset;
      }
    }
  } else if (scope == SCOPE_BIT) {
    bitM = marginData;
    rankList = GetRankNvList(host, socket, ch, dimm);

    bit = 0x8 * (strobe % 9) + 0x4 * (strobe / 9);
    Loop = 0x8 * (strobe % 9) + 0x4 * (strobe / 9) + 8 / (maxStrobe / 9);
    for (; bit < Loop; bit++) {
      // Save new margin at the failing point if there wasn't already a failure
      if ((foundFail[strobe%9] & (1 << (bit % 8))) == 0) {
        if (sign) (*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].n = offset;
        else (*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].p = offset;
      }
    } // bit loop
  }
} // SaveThisMargin

/**
  Restore Offset

  @param host          - Pointer to sysHost
  @param socket        - Processor socket to check
  @param ch            - Current Channel
  @param dimm          - Dimm number (0-based)
  @param rank          - Rank number (0-based)
  @param offset        - Offset to restore
  @param level         - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param group         - Test group

  @retval N/A
**/
void
RestoreOffset(
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     dimm,
             UINT8     rank,
             INT16     restoreOffset,
             GSM_LT    level,
             GSM_GT    group
             )
{
  INT16   zeroOffset = 0;
  UINT16  cmdMinVal = MAX_CMD_MARGIN;
  UINT16  cmdMaxVal = 0;

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "RestoreOffset\n"));

  if ((host->nvram.mem.dramType == SPD_TYPE_DDR3) && (group == TxVref)){
    CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, ALL_BITS, level, group, GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &restoreOffset));
  } else {
    switch (group) {
    case RxVref:
    case TxVref:
    case RxDqsDelay:
    case RxDqsPDelay:
    case RxDqsNDelay:
    case TxDqDelay:
    case RecEnDelay:
    case WrLvlDelay:
      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, ALL_BITS, level, group, GSM_WRITE_OFFSET | GSM_FORCE_WRITE,
                               &zeroOffset));
      break;
    case CmdGrp0:
    case CmdGrp1:
    case CmdGrp2:
    case CmdGrp3:
    case CmdGrp4:
    case CmdGrp5:
    case CmdAll:
      GetSetCmdGroupDelayCore (host, socket, ch, dimm, level, group, GSM_READ_ONLY, &zeroOffset, &cmdMinVal, &cmdMaxVal); // Get Current value
      zeroOffset = restoreOffset - zeroOffset;
      GetSetCmdGroupDelayCore (host, socket, ch, dimm, level, group, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &zeroOffset, &cmdMinVal, &cmdMaxVal);
      break;
    case CmdVref:
      GetSetCmdVrefCore (host, socket, ch, dimm, level, GSM_READ_ONLY, &zeroOffset);
      zeroOffset = restoreOffset - zeroOffset;
      GetSetCmdVrefCore (host, socket, ch, dimm, level, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &zeroOffset);
      break;
    case CtlGrp0:
    case CtlGrp1:
    case CtlGrp2:
    case CtlGrp3:
    case CtlGrp4:
    case CtlGrp5:
    case CtlAll:
      GetSetCtlGroupDelayCore (host, socket, ch, dimm, level, group, GSM_READ_ONLY, &zeroOffset, &cmdMinVal, &cmdMaxVal); // Get Current value
      zeroOffset = restoreOffset - zeroOffset;
      GetSetCtlGroupDelayCore (host, socket, ch, dimm, level, group, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &zeroOffset, &cmdMinVal, &cmdMaxVal);
      break;
    default:
      break;
    }
  }
  if (host->var.mem.socket[socket].hostRefreshStatus == 0) {
    CHIP_FUNC_CALL(host, IO_Reset(host, socket));
  }
} // RestoreOffset

/**
  Evaluates the results of the previous test

  @param host          - Pointer to sysHost
  @param socket        - Processor socket to check
  @param dimm          - Dimm number (0-based)
  @param rank          - Rank number (0-based)
  @param sign          - The direction we are currently going (up, down, left or right)
  @param offset        - Pointer to offsets
  @param chStatus      - Channel Status
  @param scope         - Margin granularity
  @param group         - Parameter to be margined
  @param level         - DDR or LRDIMM Host side, LRDIMM Backside, etc...
  @param mode          - Test mode to use
  @param searchUp      - Direction to search
  @param bwSerr        - Bit wise error status
  @param foundPass     - Bit mask marking passes
  @param foundFail     - Bit mask marking fails
  @param marginData    - pointer to Margin data
  @param ranksDone     - Array of bits showing if ranks are done. If bitX = 1, the rank is done.
  @param maxSteps      - Maximum Steps

  @retval N/A

**/
void
EvaluateResults (
                PSYSHOST        host,
                UINT8           socket,
                UINT8           dimm,
                UINT8           rank,
                UINT8           sign,
                INT16           offset[MAX_CH][MAX_RANK_CH],
                UINT32          chStatus,
                UINT8           scope,
                GSM_GT          group,
                GSM_LT          level,
                UINT16          mode,
                UINT8           searchUp[MAX_CH][MAX_RANK_CH],
                UINT32          bwSerr[MAX_CH][3],
                struct bitMask  *foundPass,
                struct bitMask  *foundFail,
                void            *marginData,
                UINT32          *ranksDone,
                UINT8           maxSteps
                )
{
  UINT8               ch;
  UINT8               d = 0;
  UINT8               maxDimm = MAX_DIMM;
  UINT8               startDimm = 0;
  UINT8               r = 0;
  UINT8               maxRank = MAX_RANK_DIMM;
  UINT8               startRank = 0;
  UINT8               strobe;
  UINT8               strobeIndex;
  UINT8               bit;
  UINT8               bitIndex;
  UINT8               bitMask;
  UINT32              status = SUCCESS;
  UINT32              errResult = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  if (scope == SCOPE_SOCKET) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // No need to check this channel if it is done
      if ((chStatus & (1 << ch)) == 0) continue;

      status |= bwSerr[ch][0] + bwSerr[ch][1];

      if (host->nvram.mem.eccEn) {
        status |= bwSerr[ch][2];
      }
    } // ch loop

    EvaluateStrobeStatus(host, socket, SET_ALL, SET_ALL, SET_ALL, SET_ALL, SET_ALL, status, offset[0][0], sign,
                         searchUp[0][0], foundPass->bits, foundFail->bits, marginData, scope, mode);
  } else if (scope == SCOPE_CH) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // No need to check this channel if it is done
      if ((chStatus & (1 << ch)) == 0) continue;

      status = bwSerr[ch][0] | bwSerr[ch][1];

      if (host->nvram.mem.eccEn) {
        status = status | bwSerr[ch][2];
      }

      EvaluateStrobeStatus(host, socket, ch, SET_ALL, SET_ALL, SET_ALL, SET_ALL, status, offset[ch][0], sign,
                           searchUp[ch][0], foundPass->bits, foundFail->bits, marginData, scope, mode);
    } // ch loop
  } else if (scope == SCOPE_DIMM) {
  } else if (scope == SCOPE_RANK) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // No need to check this channel if it is done
      if ((chStatus & (1 << ch)) == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
         DisplayBwSerr(host, socket, ch, dimm, rank, bwSerr[ch], offset[ch][(*rankList)[rank].rankIndex]);
      }
#endif  // SERIAL_DBG_MSG
      status = bwSerr[ch][0] | bwSerr[ch][1];

      if (host->nvram.mem.eccEn) {
        status = status | bwSerr[ch][2];
      }

      EvaluateStrobeStatus(host, socket, ch, dimm, rank, SET_ALL, SET_ALL, status,
                           offset[ch][(*rankList)[rank].rankIndex], sign, searchUp[ch][(*rankList)[rank].rankIndex],
                           foundPass->bits, foundFail->bits, marginData, scope, mode);
    } // ch loop
  } else if (scope == SCOPE_STROBE) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // No need to check this channel if it is done
      if ((chStatus & (1 << ch)) == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        DisplayBwSerr(host, socket, ch, dimm, (*rankList)[rank].rankIndex, bwSerr[ch], offset[ch][(*rankList)[rank].rankIndex]);
      }
#endif  // SERIAL_DBG_MSG
      for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
        if ((!host->nvram.mem.eccEn) && (strobe == 8)) continue;

        switch (strobe) {
          case 0:
            errResult = bwSerr[ch][0];
            break;

          case 4:
            errResult = bwSerr[ch][1];
            break;

          case 8:
            errResult = bwSerr[ch][2];
            break;
        }

        //
        // Find the index into the results data
        //
        strobeIndex = CHIP_FUNC_CALL(host, FindIndexResData (strobe));

        if (CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, group, level) == MAX_STROBE)) {

          //
          // x4 Lower nibble
          //
          status = (errResult >> (strobeIndex * 8)) & 0xF;
          EvaluateStrobeStatus(host, socket, ch, dimm, rank, strobe, 0xF, status, offset[ch][(*rankList)[rank].rankIndex],
                               sign, searchUp[ch][(*rankList)[rank].rankIndex], foundPass->bits, foundFail->bits, marginData,
                               scope, mode);

          //
          // x4 Upper nibble
          //
          status = (errResult >> (strobeIndex * 8)) & 0xF0;
          EvaluateStrobeStatus(host, socket, ch, dimm, rank, strobe + 9, 0xF0, status,
                               offset[ch][(*rankList)[rank].rankIndex], sign, searchUp[ch][(*rankList)[rank].rankIndex],
                               foundPass->bits, foundFail->bits, marginData, scope, mode);
        } else {
          // x8 Byte
          status = (errResult >> (strobeIndex * 8)) & 0xFF;
          EvaluateStrobeStatus(host, socket, ch, dimm, rank, strobe, 0xFF, status, offset[ch][(*rankList)[rank].rankIndex], sign, searchUp[ch][(*rankList)[rank].rankIndex],
                               foundPass->bits, foundFail->bits, marginData, scope, mode);
        }
      } // strobe loop
    } // ch loop
  } // scope == strobe
  else if (scope == SCOPE_BIT) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // No need to check this channel if it is done
      if ((chStatus & (1 << ch)) == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        DisplayBwSerr(host, socket, ch, dimm, (*rankList)[rank].rankIndex, bwSerr[ch], offset[ch][(*rankList)[rank].rankIndex]);
      }
#endif  // SERIAL_DBG_MSG
      for (bit = 0; bit < MAX_BITS; bit++) {
        if ((!host->nvram.mem.eccEn) && (bit > 63)) continue;

        switch (bit) {
          case 0:
            errResult = bwSerr[ch][0];
            break;

          case 32:
            errResult = bwSerr[ch][1];
            break;

          case 64:
            errResult = bwSerr[ch][2];
            break;
        } // switch

        //
        // Get strobe group
        //
        strobe = bit / 8;

        //
        // Find the index into the results data
        //
        strobeIndex = CHIP_FUNC_CALL(host, FindIndexResData (strobe));

        //
        // Index into error results
        //
        bitIndex = bit % 8;

        //
        // Mask for error results
        //
        bitMask = 1 << bitIndex;

        //
        // Get the status for the current bit
        //
        status = (errResult >> (strobeIndex * 8)) & bitMask;

        EvaluateStrobeStatus(host, socket, ch, dimm, rank, strobe, bitMask, status, offset[ch][(*rankList)[rank].rankIndex], sign, searchUp[ch][(*rankList)[rank].rankIndex],
                             foundPass->bits, foundFail->bits, marginData, scope, mode);
      } // bit loop
    } // ch loop
  } // scope == bit


  if ((scope == SCOPE_SOCKET) || (scope == SCOPE_CH)) {
    startDimm = 0;
    maxDimm = MAX_DIMM;
    startRank = 0;
    maxRank = MAX_RANK_DIMM;
  }

  if (scope == SCOPE_DIMM) {
    startDimm = dimm;
    maxDimm = dimm + 1;
    startRank = 0;
    maxRank = MAX_RANK_DIMM;
  }

  if ((scope == SCOPE_RANK) || (scope == SCOPE_STROBE)|| (scope == SCOPE_BIT)) {
    startDimm = dimm;
    maxDimm = dimm + 1;
    startRank = rank;
    maxRank = rank + 1;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    // No need to check this channel if it is done
    if ((chStatus & (1 << ch)) == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (d = startDimm; d < maxDimm; d++) {

      if ((*dimmNvList)[d].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, d);

      // Loop for each rank
      for (r = startRank; r < maxRank; r++) {

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, d, r, DONT_CHECK_MAPOUT)) continue;

        if (CheckAllPassFail(host, foundPass->bits[ch][(*rankList)[r].rankIndex])) {
          searchUp[ch][(*rankList)[r].rankIndex] = 0;
        } else {
          searchUp[ch][(*rankList)[r].rankIndex] = 1;
        }
      } // r loop
    } // d loop
  } // ch loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    // No need to check this channel if it is done
    if ((chStatus & (1 << ch)) == 0) continue;

    rankList = GetRankNvList(host, socket, ch, dimm);

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    // Increment offset if searching away from 0
    if (searchUp[ch][(*rankList)[rank].rankIndex]) {
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = offset[ch][(*rankList)[rank].rankIndex] - 1;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = offset[ch][(*rankList)[rank].rankIndex] + 1;
      }
    }
    // Decrement offset is searching toward 0
    else {
      if (sign) {
        offset[ch][(*rankList)[rank].rankIndex] = offset[ch][(*rankList)[rank].rankIndex] + 1;
      } else {
        offset[ch][(*rankList)[rank].rankIndex] = offset[ch][(*rankList)[rank].rankIndex] - 1;
      }
    }

    // Set global fail if we have exceeded the maximum number of steps
    if ((offset[ch][(*rankList)[rank].rankIndex] >= maxSteps) || (0 - offset[ch][(*rankList)[rank].rankIndex] >= maxSteps)) {
      EvalResSetRanksDone (host, socket, ch, dimm, rank, scope, ranksDone);
    }

    // Set global fail if we have searched all the way to 0
    //if (offset[ch][(*rankList)[rank].rankIndex] == 0) {
    //  *ranksDone |= (1 << ((*rankList)[rank].rankIndex + (ch * 8)));
    //}
  } // ch loop
} // EvaluateResults

void
EvaluateStrobeStatus (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT8     ch,
                     UINT8     dimm,
                     UINT8     rank,
                     UINT8     strobe,
                     UINT8     bitMask,
                     UINT32    status,
                     INT16     offset,
                     UINT8     sign,
                     UINT8     searchUp,
                     UINT8     foundPass[MAX_CH][MAX_RANK_CH][MAX_STROBE/2],
                     UINT8     foundFail[MAX_CH][MAX_RANK_CH][MAX_STROBE/2],
                     void      *marginData,
                     UINT8     scope,
                     UINT16    mode
                     )
/**
  Evaluate test results for one strobe group

  @param host          - Pointer to sysHost
  @param socket        - Processor socket to check
  @param ch            - Current Channel
  @param dimm          - Dimm number (0-based)
  @param rank          - Rank number (0-based)
  @param strobe        - Current Strobe
  @param bitMask       - Bit Mask of currently found passes
  @param status        - Status
  @param offset        - Pointer to offsets
  @param sign          - The direction we are currently going (up, down, left or right)
  @param searchUp      - Direction to search
  @param foundPass     - Bit mask marking passes
  @param foundFail     - Bit mask marking fails
  @param marginData    - pointer to Margin data
  @param scope         - Margin granularity
  @param mode          - Test mode to use

  @retval N/A

**/
{
  UINT8                 maxCh;
  UINT8                 maxDimm;
  UINT8                 maxRank;
  UINT8                 setAllStrobe;
  UINT8                 strobeByte = 0;
  UINT8                 i;
  UINT8                 bit;
  UINT8                 tmpBitMask;
  struct  ddrRank       (*rankList)[MAX_RANK_DIMM];
  struct  baseMargin    *socketM;
  struct  chMargin      *channelM;
  struct  rankMargin    *rankM;
  struct  strobeMargin  *strobeM;
  struct  bitMargin     *bitM;

  if (ch == SET_ALL) {
    ch = 0;
    maxCh = MAX_CH;
  } else {
    maxCh = ch + 1;
  }
  if (dimm == SET_ALL) {
    dimm = 0;
    maxDimm = MAX_DIMM;
  } else {
    maxDimm = dimm + 1;
  }
  if (rank == SET_ALL) {
    rank = 0;
    maxRank = MAX_RANK_DIMM;
  } else {
    maxRank = rank + 1;
  }
  if (strobe == SET_ALL) {
    strobe = 0;
    setAllStrobe = SET_ALL;
  } else {
    strobeByte = strobe;
    if (strobe > 8) strobe = strobe - 9;
    setAllStrobe = strobe;
  }

  // If there was a failure
  if (status) {
    for (; ch < maxCh; ch++) {
      for (; dimm < maxDimm; dimm++) {
        rankList = GetRankNvList(host, socket, ch, dimm);
        for (; rank < maxRank; rank++) {
          // If a pass has previously been found
          if (foundPass[ch][(*rankList)[rank].rankIndex][strobe] & bitMask) {
            // If we are searching up
            if (searchUp) {
              // Set all foundFail bits
              SetAllFound(host, ch, (*rankList)[rank].rankIndex, setAllStrobe, bitMask, foundFail);
            }
          } // If a previous pass has been found
        } // rank loop
      } // dimm loop
    } // ch loop
  } else {
    if (scope == SCOPE_SOCKET) {
      socketM = marginData;

      // Save new offset at the failing point
      if (sign) (*socketM).n = offset - 1;
      else (*socketM).p = offset + 1;
    } else if (scope == SCOPE_CH) {
      channelM = marginData;

      // Save new offset at the failing point
      if (sign) (*channelM).channel[ch].n = offset - 1;
      else (*channelM).channel[ch].p = offset + 1;
    } else if (scope == SCOPE_DIMM) {
    } else if (scope == SCOPE_RANK) {
      rankM = marginData;
      rankList = GetRankNvList(host, socket, ch, dimm);

      // Save new offset at the failing point
      if (sign) (*rankM).rank[ch][(*rankList)[rank].rankIndex].n = offset - 1;
      else (*rankM).rank[ch][(*rankList)[rank].rankIndex].p = offset + 1;
    } else if (scope == SCOPE_STROBE) {
      strobeM = marginData;
      rankList = GetRankNvList(host, socket, ch, dimm);

      if ((foundFail[ch][(*rankList)[rank].rankIndex][strobe] & bitMask) == 0) {
        // Save new offset at the failing point
        if (sign) {
          (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobeByte].n = offset - 1;
        } else {
          (*strobeM).strobe[ch][(*rankList)[rank].rankIndex][strobeByte].p = offset + 1;
        }

      }
    } else if (scope == SCOPE_BIT) {
      bitM = marginData;
      rankList = GetRankNvList(host, socket, ch, dimm);

      i = 0;
      tmpBitMask = bitMask;
      while (tmpBitMask != 1) {
        tmpBitMask >>= 1;
        i++;
      }

      bit = (strobe * 8) + i;

      if ((foundFail[ch][(*rankList)[rank].rankIndex][strobe] & (1 << i)) == 0) {
        // Save new offset at the failing point
        if (sign) (*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].n = offset - 1;
        else      (*bitM).bits[ch][(*rankList)[rank].rankIndex][bit].p = offset + 1;
      }
    }

    for (; ch < maxCh; ch++) {
      for (; dimm < maxDimm; dimm++) {
        rankList = GetRankNvList(host, socket, ch, dimm);
        for (; rank < maxRank; rank++) {
          // Set all foundPass bits
          SetAllFound(host, ch, (*rankList)[rank].rankIndex, setAllStrobe, bitMask, foundPass);
        } // rank loop
      } // dimm loop
    } // ch loop
  }
} // EvaluateStrobeStatus

UINT32
ExecuteTest (
            PSYSHOST  host,
            UINT8     socket,
            UINT32    ddrChEnMap,
            UINT8     dimm,
            UINT8     rank,
            GSM_GT    group,              // Don't care since CPGC runs only dq pattern
            UINT16    mode,
            UINT32    patternLength,
            UINT8     burstLength
            )
/**
  Executes an based memory test for advance trainings

  @param host:         Pointer to sysHost
  @param socket:       Processor socket to check
  @param ddrChEnMap    Bit-mask of enabled channels to test. 1 = Enabled, 0 = Skipped
  @param dimm:         Target DIMM to test
  @param rank:         Target Rank to test
  @param group:        Parameter to be margined
                         RxDqsDelay    : Margin Rx DqDqs
                         TxDqDelay     : Margin Tx DqDqs
                         RxVref         : Margin Rx Vref
                         TxVref         : Margin Tx Vref
                         RecEnDelay    : Margin Receive Enable
                         WrLvlDelay    : Margin Write Level
                         CmdGrp0        : Margin CMD group 0
                         CmdGrp1        : Margin CMD group 1
                         CmdGrp2        : Margin CMD group 2
                         CmdAll         : Margin all CMD groups
  @param mode:         Margin mode to use
                         MODE_XTALK            BIT0: Enable crosstalk (placeholder)
                         MODE_VIC_AGG          BIT1: Enable victim/aggressor
                         MODE_START_ZERO       BIT2: Start at margin 0
                         MODE_POS_ONLY         BIT3: Margin only the positive side
                         MODE_NEG_ONLY         BIT4: Margin only the negative side
                         MODE_DATA_MPR         BIT5: Enable MPR Data pattern
                         MODE_DATA_FIXED       BIT6: Enable Fixed Data pattern
                         MODE_DATA_LFSR        BIT7: Enable Data LFSR
                         MODE_ADDR_LFSR        BIT8: Enable Address LFSR
                         MODE_ADDR_CMD0_SETUP  BIT9
                         MODE_CHECK_PARITY     BIT10: Enable parity checking
                         MODE_DESELECT         BIT11: Enable deselect patterns
                         MODE_VA_DESELECT      BIT12: Enable Victim - Aggressor deselect patterns

  patternLength: Number of sequences to test (units of burstLength cache lines)
  burstLength:   Number of cachelines to test for each write/read operation
                 It is the repeat count for the WR/RD subsequence

  @retval N/A

**/
{
  UINT8 logRank = 0;
  UINT8 logSubRank = 0;
  //UINT8 vaLoopMax;
  struct channelNvram (*channelNvList)[MAX_CH];
  UINT8 ch;
  UINT32 status = SUCCESS;
#ifdef LRDIMM_SUPPORT
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  UINT8 sdramDensity = 0;
  UINT8 lrdimmDeviceSize = 0;
#endif //LRDIMM_SUPPORT

  channelNvList = GetChannelNvList(host, socket);
  if (ddrChEnMap != 0) {

    // Setup CPGC training pattern for specific group for all channels (parallel)
#ifdef LRDIMM_SUPPORT
    if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((ddrChEnMap & (1 << ch)) == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);
        if ((*dimmNvList)[dimm].lrRankMult < 2) continue;

        sdramDensity = (*dimmNvList)[dimm].SPDSDRAMBanks & 7;
        if (sdramDensity == SPD_2Gb)
          lrdimmDeviceSize = 1;   // 3'b001: LRDIMM with 2Gb devices (LR Rank's appended after A[14])
        else if (sdramDensity == SPD_4Gb || sdramDensity == SPD_8Gb)
          lrdimmDeviceSize = 2;   // 3'b010: LRDIMM with 4/8Gb devices (LR Rank's appended after A[15])

        //add a print to prevent lrdimmDeviceSize GCC build errors
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                      "LRDIMM Device Size %d\n", lrdimmDeviceSize));
        // TODO: CpgcSetLRDIMMMapping(host, socket, ch, logRank, lrdimmDeviceSize, (*dimmNvList)[dimm].lrRankMult);
      } // for ch
    } // if DDR3
#endif //LRDIMM_SUPPORT

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((ddrChEnMap & (1 << ch)) == 0) continue;
      // Convert physical DIMM/physical rank into VMSE logical rank
      logRank = GetLogicalRank(host, socket, ch, dimm, rank);
      if ((*channelNvList)[ch].encodedCSMode) logSubRank = (rank >> 1);
      if (CheckCMDGroup(group) || CheckCTLGroup(group)) {
        CHIP_FUNC_CALL(host, CpgcAdvTrainingSetup(host, socket, (1 << ch), logRank, 0, group, DDR_CPGC_ADV_CMD_PATTERN, CHIP_FUNC_CALL(host, PatternLengthToCpgcLoopCnt(patternLength))));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, NO_STROBE, NO_BIT,
                 "CMD/CTL patternlength = %d\n",CHIP_FUNC_CALL(host, PatternLengthToCpgcLoopCnt(patternLength))));
      } else {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, rank, NO_STROBE, NO_BIT,
                 "RMT patternlength = %d\n",CHIP_FUNC_CALL(host, PatternLengthToCpgcLoopCnt(patternLength) )));
        CHIP_FUNC_CALL(host, CpgcAdvTrainingSetup(host, socket, (1 << ch), logRank, logSubRank, group, DDR_CPGC_DQ_PATTERN, CHIP_FUNC_CALL(host, PatternLengthToCpgcLoopCnt(patternLength))));
      } // if check CMD CTL grp
    } //ch

    //
    // Set up the CADB if necessary
    //
    if (mode & (MODE_DESELECT | MODE_VA_DESELECT)) {
      CHIP_FUNC_CALL(host, EnableCADBDeselect(host, socket, ddrChEnMap, mode));
    }

    // Setup for Victim/Aggressor
    /*
    if (mode & MODE_VIC_AGG) {
      // Loop for 8 victim bits
      vaLoopMax = 8;
    } else if (mode & MODE_DATA_LFSR) {
      // Loop count depends on the WDB range. We always start from 0 so it really depends on the end point.
      vaLoopMax = WDB_END + 1;     // Use default only (wdbEnd) = 7
    } else {
      // Only one loop
      vaLoopMax = 1;
    }*/

    // Run CPGC training test for the selected channel(s)
    status = CHIP_FUNC_CALL(host, CpgcAdvTrainingExecTestGroup (host, socket, ddrChEnMap, 0, group));
  }

  return status;
} // ExecuteTest
