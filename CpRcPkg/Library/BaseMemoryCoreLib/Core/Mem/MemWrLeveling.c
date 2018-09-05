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
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "Cpgc.h"

//
// Local function prototypes
//

static void
WriteLevelingEnd (
                 PSYSHOST host,
                 UINT8    socket
                 );

static void
TxPiSample (
           PSYSHOST host,
           UINT8    socket,
           UINT8    dimm,
           UINT8    rank
           );


/**

  Perform Write Leveling training

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
WriteLeveling (
              PSYSHOST host
              )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT8               rankPresent;
#ifdef SERIAL_DBG_MSG
  UINT16              delay;
#endif // SERIAL_DBG_MSG
  UINT16              risingEdge[MAX_CH][MAX_STROBE];
  UINT16              centerPoint[MAX_CH][MAX_STROBE];
  UINT16              fallingEdge[MAX_CH][MAX_STROBE];
  UINT16              pulseWidth[MAX_CH][MAX_STROBE];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct Socket       (*varSocket)[MAX_SOCKET];
  UINT8               faultyPartsStatus;
#ifdef IDT_LRBUF_WA
  UINT8               cwData;
  UINT32              lrbufData;
#endif  //  IDT_LRBUF_WA

  if (~host->memFlows & MF_WR_LVL_EN) return SUCCESS;

  socket = host->var.mem.currentSocket;

  //
  // Removed scaling support per Core HSD 4757065. It should not be necessary until 12Gb and 16Gb devices become available.
  // Current 8Gb devices can return non-zero capability in the MPR location, and a SPD byte is not defined to discovery DRAM capability.
  //
  // Get t_wr scaling info from DRAM 0 using MPR read mechanism
  //
  //TwrScaling( host);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteLeveling Starts\n"));
#endif

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  faultyPartsStatus = FPT_NO_ERROR;

  channelNvList = GetChannelNvList(host, socket);
  varSocket        = &host->var.mem.socket;

#ifdef IDT_LRBUF_WA
  // IDT Workaround start
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    if (((*channelNvList)[ch].lrDimmPresent) && (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_IDT_DVID) &&
            ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_RID_B0)) {

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                        "LRDIMM IDT Sighting 200004 - Start WL\n"));
          // sighting 200004
          ReadLrbufSmb(host, socket, ch, dimm, 0x0020, &lrbufData);

          (*dimmNvList)[dimm].idtF3RC6 = lrbufData;

          cwData = ((UINT8) (lrbufData >> 24) & 0x0f) | BIT0;
          WriteRcLrbuf(host, socket, ch, dimm, 0, cwData, 3, 6);

          ReadLrbufSmb(host, socket, ch, dimm, 0x6008, &lrbufData);

          (*dimmNvList)[dimm].idtSMBF6RC8 = lrbufData;

          ReadLrbufSmb(host, socket, ch, dimm, 0x1028, &lrbufData);

          lrbufData = (lrbufData & 0x3f) + 0x20;
          cwData = ((UINT8) (*dimmNvList)[dimm].idtSMBF6RC8 & 0xc0) | (UINT8) lrbufData;

          WriteRcLrbufExtended(host, socket, ch, dimm, 0, cwData, 0x6008);

        } // IDT_B0
      } // dimm loop
    } // lrDimmPresent
  } // ch loop
#endif    // IDT_LRBUF_WA

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < (*varSocket)[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, WRITE_LEVELING_BASIC, (UINT16)((socket << 8)|(dimm << 4)|rank)));
      //
      // Initialize to no ranks present
      //
      rankPresent = 0;

      for (ch = 0; ch < MAX_CH; ch++) {
        // Go to next channel if this one is disabled
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);

        // Continue if this rank is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        //
        // Inidicate this rank is present on at least one channel
        //
        rankPresent |= (*rankList)[rank].enabled;

        if ((*dimmNvList)[dimm].x4Present) {
          if (host->nvram.mem.eccEn) {
            host->var.mem.piSettingStopFlag[ch] = 0x3FFFF;
          } else {
            host->var.mem.piSettingStopFlag[ch] = 0x1FEFF;
          }
        } else {
          if (host->nvram.mem.eccEn) {
            host->var.mem.piSettingStopFlag[ch] = 0x1FF;
          } else {
            host->var.mem.piSettingStopFlag[ch] = 0x0FF;
          }
        } // if x4
      } // ch loop

      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (!rankPresent) continue;

      //
      // Change Rtt_nom to Rtt_wr and enable DRAM Write Leveling mode
      //
#ifdef LRDIMM_SUPPORT
      host->var.mem.InOvrRttPrkMode = 1;
#endif
      OverrideRttNom(host, socket, dimm, rank);

      //
      // Reset the IO
      //
      //CHIP_FUNC_CALL(host, IO_Reset (host, socket));

      // Sample all DQS Pi's
      TxPiSample (host, socket, dimm, rank);

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList  = GetDimmNvList(host, socket, ch);

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "\nSummary: Write Leveling Pi"));
          rcPrintf ((host, "\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
          PrintSampleArray (host, socket, ch, dimm, 0);
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG

        for (strobe = 0; strobe < maxStrobe; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          EvaluatePiSettingResults (host, socket, ch, dimm, rank, strobe, risingEdge, centerPoint, fallingEdge, pulseWidth, FPT_PI_WRITE_TYPE);

          if (host->var.mem.faultyPartsFlag[ch] & (1 << strobe)) {

            faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);

            if (faultyPartsStatus == FPT_NO_ERROR) {
              // do we need to do anything here ?!?
            } else if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
              // if correctable log the warning -> OutputWarning
              // careful for cases when warning gets promoted to error !!!  - log the strobe
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_WR_LEVEL, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, WrLvlDelay, DdrLevel, (UINT8)pulseWidth[ch][strobe]);
            } else {
              host->var.mem.piSettingStopFlag[ch] |= (1 << strobe);
              DisableChannel(host, socket, ch);
              //
              // Add this error to the warning log for both correctable and uncorrectable errors.
              //
              EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_WR_LEVEL, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, WrLvlDelay, DdrLevel, (UINT8)pulseWidth[ch][strobe]);
            }
          }

          risingEdge[ch][strobe] = risingEdge[ch][strobe] - 64;

          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_UPDATE_CACHE, (INT16 *)&risingEdge[ch][strobe]));
          if (!(*dimmNvList)[dimm].x4Present) {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqsDelay, GSM_UPDATE_CACHE, (INT16 *)&risingEdge[ch][strobe]));
          }
        } // strobe loop

#ifdef SERIAL_DBG_MSG
        // Display results
        DisplayEdges(host, socket, ch, pulseWidth[ch], fallingEdge[ch], risingEdge[ch], centerPoint[ch], maxStrobe);

        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "START_DATA_WR_LVL_BASIC\n"));
        }
        for (strobe = 0; strobe < maxStrobe; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&delay));
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "WrLevel Delay = %d\n", delay));
        } // strobe loop
        if (checkMsgLevel(host, SDBG_MAX)) {
          rcPrintf ((host, "STOP_DATA_WR_LVL_BASIC\n"));
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
      } // ch loop

      //
      // Restore Rtt_nom and disable DRAM Write Leveling mode
      //
#ifdef LRDIMM_SUPPORT
      host->var.mem.InOvrRttPrkMode = 0;
#endif
      RestoreRttNom(host, socket, dimm, rank);

    } // rank loop
  } // dimm loop

#ifdef IDT_LRBUF_WA
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if (((*channelNvList)[ch].lrDimmPresent) && (host->nvram.mem.dramType == SPD_TYPE_DDR3)) {
      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        // check for IDT B0 iMB
        if (((*dimmNvList)[dimm].lrbufDVid == LRBUF_IDT_DVID) &&
            ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_RID_B0)) {
          // Sighting 200004
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                        "LRDIMM IDT Sighting 200004 - End WL\n"));

          lrbufData = (*dimmNvList)[dimm].idtSMBF6RC8;
          WriteLrbufSmb(host, socket, ch, dimm, 2, 0x6008, &lrbufData);

          lrbufData = (*dimmNvList)[dimm].idtF3RC6;
          cwData = ((UINT8) (lrbufData >> 24) & 0x0f);
          WriteRcLrbuf(host, socket, ch, dimm, 0, cwData, 3, 6);

        } // IDT_B0
      } // dimm loop
    } // lrDimmPresent
  } // ch loop
#endif // IDT_LRBUF_WA
  WriteLevelingEnd (host, socket);
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteLeveling Ends\n"));
#endif

  return SUCCESS;
} // WriteLeveling

/**

  Clean up after Write Leveling training

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static
void
WriteLevelingEnd (
                 PSYSHOST host,
                 UINT8    socket
                 )
{
  UINT8               dimm;
  UINT8               rank;

  //
  // Enable all DIMM output buffers
  //

  //
  // Loop for each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Loop for each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      //
      // Change the Write level mode to 0 in the end of the algorithm
      //
        CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, CLEAR_MODE));

    } // rank loop
  } // dimm loop

  //
  // reset the IO
  //
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
} // WriteLevelingEnd

/**

  Samples all DQS Pi's

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param dimm  - DIMM number
  @param rank   - Rank number
  @param nibble - 0 for the low nibble only, 1 for the high nibble, 2 for both nibbles

  @retval N/A

**/
static
void
TxPiSample (
           PSYSHOST host,
           UINT8    socket,
           UINT8    dimm,
           UINT8    rank
           )
{
  UINT8                   ch;
  UINT32                  chBitmask;
  UINT8                   strobe;
  UINT8                   dWord;
  INT16                   piDelay;
  UINT16                  piDelayEnd;
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  struct TrainingResults  (*trainRes)[MAX_CH][MAX_STROBE];
  CHIP_DDRIO_STRUCT       dataControl;

  trainRes      = &host->var.mem.trainRes;

  CHIP_FUNC_CALL(host, TxPiSampleChipInit(&dataControl));

  chBitmask = GetChBitmask (host, socket, dimm, rank);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ( !((1 << ch) & chBitmask)) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    if (rank >= (*dimmNvList)[dimm].numRanks) continue;
    CHIP_FUNC_CALL(host, GetChipLogicalRank(host, socket, ch, dimm, rank, &dataControl));
    //
    // Clear training results
    //
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      for (dWord = 0; dWord < 4; dWord++) {
        (*trainRes)[ch][strobe].results[dWord] = 0;
      } // dWord loop
    } // strobe loop

    CHIP_FUNC_CALL(host, SetChipTxPiSampleTrainingMode(host, socket, ch, &dataControl));
  } // ch loop

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                 "Write Leveling Pi Scanning...\n"));

  CHIP_FUNC_CALL(host, GetChipTiPiSamplepiDelayStartandEnd(host, socket, &piDelay, &piDelayEnd));

  for (; piDelay < piDelayEnd; piDelay += WR_LVL_STEP_SIZE) {

    //
    // Change the TX DQS PI Setting
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ( !((1 << ch) & chBitmask)) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      if (rank >= (*dimmNvList)[dimm].numRanks) continue;

      //
      // Set training mode and rank to all channels
      //
      CHIP_FUNC_CALL(host, SetChipTxPiSampleTrainingTXDQSStrobesI(host, socket, ch, dimm, rank, piDelay, &dataControl));

      FixedDelay(host, 1);

      CHIP_FUNC_CALL(host, IO_Reset(host, socket)); // Need reset before enabling training mode.

      FixedDelay(host, 1);

      CHIP_FUNC_CALL(host, SetChipTxPiSampleTrainingTXDQSStrobesII(host, socket, ch, dimm, &dataControl));
    } // ch loop

    //
    // Clear Errors and wait to collect results
    //
    FixedDelay(host, 1);

    CHIP_FUNC_CALL(host, IO_Reset(host, socket)); // Need reset after enabling training mode.

    //
    // Clear Errors and wait to collect results
    //
    FixedDelay(host, 1);
    CHIP_FUNC_CALL(host, GetResultsWL(host, socket, dimm, rank, piDelay));

  } // piDelay inner loop

  return;
} // TxPiSample

/**

  Changes Rtt Nom to Rtt Wr

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param dimm  - DIMM number
  @param rank  - Rank number

**/
void
OverrideRttNom (
               PSYSHOST host,
               UINT8    socket,
               UINT8    dimm,
               UINT8    rank
               )
{
  UINT8               ch;
  UINT16              MR1;
  UINT8               imcRank;
  UINT8               d;
  UINT8               r;
  UINT8               odtPin;
  UINT32              wrOdtTableData;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
  struct odtActStruct *odtActPtr;
#ifdef  LRDIMM_SUPPORT
  UINT8               cwData;
#endif  //  LRDIMM_SUPPORT

  channelNvList = GetChannelNvList(host, socket);

  //
  // Enter WL mode and override Rtt_nom
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    rankStruct = GetRankStruct(host, socket, ch, dimm);

    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

#ifdef  LRDIMM_SUPPORT
    if((CHIP_FUNC_CALL(host, ShouldChipEnableDDR4LRDIMMHostSideTraining(host, socket, ch, dimm, rank))) &&
    ((*rankStruct)[rank].CurrentLrdimmTrainingMode != LRDIMM_DWL_TRAINING_MODE) &&
    (host->var.mem.InPbaWaMode == 0)) {
      //
      // Set the current rank within the BC08 register.
      //
      WriteLrbuf(host, socket, ch, dimm, 0, rank, LRDIMM_F0, LRDIMM_BC08);
      //
      // all other dimms in the channel must have their host interface DQ/DQS drivers disabled
      //
      for (d = 0; d < (*channelNvList)[ch].maxDimm; d++) {
        if (!(CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, d)))) continue;
        cwData = (*channelNvList)[ch].dimmList[d].lrBuf_BC03;
        if (d != dimm) {
          cwData |= LRDIMM_HOSTDQDQSDRIVERS_DISABLE;
        }
        WriteLrbuf(host, socket, ch, d, 0, cwData, LRDIMM_F0, LRDIMM_BC03);
      } // d loop
      //
      // Enable LRDIMM host side training mode
      //
      WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_HWL_TRAINING_MODE, LRDIMM_F0, LRDIMM_BC0C);
      (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_HWL_TRAINING_MODE;

#ifndef COSIM_EN
#endif
      //
      // Copy RTT_WR to RTT_PARK
      //
      if ((*channelNvList)[ch].dimmList[dimm].lrBuf_BC01 != 0) {
        cwData = (*channelNvList)[ch].dimmList[dimm].lrBuf_BC01;
        WriteLrbuf(host, socket, ch, dimm, 0, cwData, LRDIMM_F0, LRDIMM_BC02);
      }
    } else
#endif  //LRDIMM_SUPPORT
    {
      //
      // for DDR4, override RTT_PARK with RTT_WR value (unless RTT_WR is disabled)
      //
      CHIP_FUNC_CALL(host, SetChipRTTPark(host, socket, ch, dimm, rank));
      // Get the current settings for MR1
      MR1 = (*rankStruct)[rank].MR1;

#ifdef  LRDIMM_SUPPORT
      if (host->var.mem.InPbaWaMode == 0)
#endif
      {
        //
        // Enable Write Leveling mode
        // Includes wait for tMOD before enabling ODT
        //
        MR1 = MR1 | MR1_WLE;
        if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
          MR1 |= 1;
        }
        MR1 &= 0xfff9;
        WriteMRS (host, socket, ch, dimm, rank, MR1, BANK1);
      }
    }

    // Lookup ODT activations for this config
    odtActPtr = LookupOdtAct(host, socket, ch, dimm, rank);

    wrOdtTableData = 0;

    // Set encodings for ODT signals in IMC registers
    for (d = 0; d < (*channelNvList)[ch].maxDimm; d++) {
      for (r = 0; r < host->var.mem.socket[socket].maxRankDimm; r++) {

        // Skip if no rank
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[d].rankList[r].enabled == 0) {
          continue;
        }

        // Get ODT signal associated with this rank
        odtPin = host->nvram.mem.socket[socket].channelList[ch].dimmList[d].rankList[r].ODTIndex;
        imcRank = d * MAX_RANK_DIMM + r;

        // Set ODT signals for target write
        if ((odtActPtr !=  NULL) && (odtActPtr->actBits[WR_TARGET] & (1 << imcRank))) {
          wrOdtTableData |= (1 << odtPin);
        }

      } // matrix rank
    } // matrix dimm

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "ODT Override: 0x%x\n", wrOdtTableData));

    CHIP_FUNC_CALL(host, AssertChipODTOveride(host, socket, ch, wrOdtTableData));
  } // ch loop

  // Wait for tWLDQSEN to be met
  FixedDelay(host, 1);

} // OverrideRttNom

/*++

  Changes Rtt Nom to Rtt Wr

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param dimm    - DIMM number
  @param rank    - Rank number

  @retval N/A

--*/
void
RestoreRttNom (
              PSYSHOST host,
              UINT8    socket,
              UINT8    dimm,
              UINT8    rank
              )
{
  UINT8               ch;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
#ifdef  LRDIMM_SUPPORT
  UINT8               d;
  UINT8               cwData;
  UINT8               rankBitMask;
#endif  //  LRDIMM_SUPPORT
  channelNvList = GetChannelNvList(host, socket);

  //
  // Restore Rtt Nom setting and exit WL mode
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    rankStruct  = GetRankStruct(host, socket, ch, dimm);

    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    //
    // Deassert ODT overrides
    //
    CHIP_FUNC_CALL(host, DeAssertChipODTOveride(host, socket, ch));
    // Wait for ODTLoff (CWL-2)
    FixedDelay(host, 1);


#ifdef  LRDIMM_SUPPORT
    if(CHIP_FUNC_CALL(host, ShouldChipDisableDDR4LRDIMMWriteLeveling(host, socket, ch, dimm, rank))){
      //
      // Disable LRDIMM host side training mode
      //
      WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
      (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_NORMAL_MODE;
      //
      // Restore to RTT_PARK
      //
      cwData = (*channelNvList)[ch].dimmList[dimm].lrBuf_BC02;
      WriteLrbuf(host, socket, ch, dimm, 0, cwData, LRDIMM_F0, LRDIMM_BC02);
      //
      // restore host interface DQ/DQS driver settings for all dimms in the channel
      //
      for (d = 0; d < (*channelNvList)[ch].maxDimm; d++) {
        if (!(CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, d)))) continue;
        cwData = (*channelNvList)[ch].dimmList[d].lrBuf_BC03;
        WriteLrbuf(host, socket, ch, d, 0, cwData, LRDIMM_F0, LRDIMM_BC03);
      }
      //
      //Per IDT DDR4 Data Buffer A0 sightings report version 2013_01_21_1349
      //sighting number 100003
      //Program BC07 to select the proper rank again.
      //
      //rank present bit mask
      rankBitMask = 0;
      switch ((*channelNvList)[ch].dimmList[dimm].numDramRanks) {
        case 1:
          rankBitMask = 0xE;
          break;

        case 2:
          rankBitMask = 0xC;
          break;

        case 4:
          rankBitMask = 0x0;
          break;
      }
      cwData = rankBitMask;
      WriteLrbuf(host, socket, ch, dimm, 0, cwData, LRDIMM_F0, LRDIMM_BC07);


    } else
#endif
    {

#ifdef  LRDIMM_SUPPORT
      if (host->var.mem.InPbaWaMode == 0)
#endif
      {
        //
        // Restore Rtt_nom, keeping WL mode enabled
        // Use separate MR1 write for Native QR DIMMs because ODT is strapped high on Ranks 1 and 3
        // Assume wait for tMOD includes ODTLoff (CWL-2)
        //

        WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1 | MR1_WLE, BANK1);

        //
        // Restore MR1 to disable WL mode
        //
        WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);
      }

      CHIP_FUNC_CALL(host, DoChipCompatibleRTT_PARKRestore(host, socket, ch, dimm, rank));
    }

  } // ch loop
} // RestoreRttNom


/*++

  this function execute the write leveling Cleanup.
  Center TxDQS-CLK timing

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval SUCCESS

--*/
UINT32
WriteLevelingCleanUp (
  PSYSHOST  host
  )
{
  UINT8                   socket;
  UINT8                   ch;
  UINT8                   dimm;
  UINT8                   rank;
  UINT8                   logRank;
  UINT8                   rankPresent;
  UINT8                   strobe;
  UINT8                   maxStrobe;
  UINT8                   offset;
  UINT32                  chBitmask;
  UINT8                   DumArr[7]    = {1,1,1,1,1,1,1};
  UINT8                   AllGood;
  UINT8                   AllGoodLoops;
  UINT8                   DqOffset;
  UINT8                   faultyPartsStatus;
  const INT16             DqOffsets[11] = {0, -4, 4, -8, 8, -12, 12, -20, 20, -40, 40};
  INT16                   Offsets[CHIP_OFFSET_ARRAY_SIZE] = CHIP_OFFSET_ARRAY_VALUES;
  INT16                   ByteOff[MAX_CH][MAX_RANK_CH][MAX_STROBE]; // Passing offset for each ch/byte.
  INT16                   ByteSum[MAX_CH]; // Sum of passing offsets for a ch
  INT8                    ByteCount[MAX_CH]; // Sum of passing offsets for a ch
  INT16                   TargetOffset = 0;
  UINT32                  strobeMask[MAX_CH];
  UINT32                  Result;
  UINT32                  errResult;
  UINT32                  SkipMe;
  UINT32                  strobePass[MAX_CH]; // Bit mask indicating which ch/byte has passed
  UINT32                  strobeFail[MAX_CH]; // Bit mask indicating which ch/byte fails to pass
  INT16                   txDqs;
  INT16                   txDq;
  INT16                   CRAddDelay[MAX_CH];
  INT16                   GlobalByteOff = 0;
  INT16                   LocalOffset = 0;
  UINT8                   chStatus;
  UINT32                  bwSerr[MAX_CH][3];
  UINT8                   bit;
  UINT8                   Done = 0;
  UINT8                   Offset;
#ifdef  LRDIMM_SUPPORT
  INT16                   lower;
  INT16                   upper;
  INT16                   center;
#endif
  struct channelNvram     (*channelNvList)[MAX_CH];
  struct ddrRank          (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  CHIP_WL_CLEANUP_STRUCT  WLChipCleanUpStruct;

  //
  // Setup CPGC Pattern
  // Use 0x00FFC33C pattern to keep DQ-DQS simple but detect any failures
  // Same Pattern as NHM/WSM
  //
  //                  //  // Rank,  Bank,  Row,   Col
  TCPGCAddress CPGCAddress =
  {{ 0, 0, 0xFFFF, 0 },    // Start
   { 0, 4, 0, 0x1F }, // Stop
   { 0, 0, 0, 0 },    // Order
   { 0, 0, 0, 0 },    // IncRate
   { 0, 4, 0, 1 } };   // IncValue, change to BG interleave

  TWDBPattern WDBPattern  = {0,  0,  3,  BasicVA};

  MemSetLocal ((UINT8 *)strobeFail, 0, sizeof (strobeFail));

  if (~host->memFlows & MF_WR_FLYBY_EN) return SUCCESS;

  //
  // Return if this socket is disabled
  //
  socket = host->var.mem.currentSocket;

  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  Offset = CHIP_FUNC_CALL(host, GetChipWriteLevelingCleanUpOffset(host));

  channelNvList = GetChannelNvList(host, socket);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteLevelingCleanUp Starts\n"));
#endif

  MemSetLocal((UINT8 *)CRAddDelay, 0, sizeof(CRAddDelay));
  //
  // Put the pattern into the WDB
  //
  WriteTemporalPattern(host, socket, WR_FLY_BY_PATTERN, FLY_BY_CACHELINES);

  for (ch = 0; ch < MAX_CH; ch++) {
    ByteSum[ch] = 0;
    ByteCount[ch] = 0;
    strobeMask[ch] = 0;
  } // ch loop

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, WR_FLY_BY, (UINT16)((SUB_WR_FLY_BY_TRAINING << 12)|(socket << 8)|(dimm << 4)|rank)));

      //
      // Initialize to no ranks present
      //
      rankPresent = 0;

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);
        rankList = GetRankNvList(host, socket, ch, dimm);

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;
        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);

        //
        // Set strobe bit masks
        //
        if ((*dimmNvList)[dimm].x4Present) {
          strobeMask[ch] = 0x3FFFF;
        } else {
          strobeMask[ch] = 0x1FF;
        }

        //
        // Clear out the ECC bits if ECC is disabled
        //
        if (host->nvram.mem.eccEn == 0) {
          strobeMask[ch] &= 0x1FEFF;
        }

        maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));

        for (strobe = 0; strobe < maxStrobe; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          if ((*rankList)[rank].faultyParts[strobe]) {
            strobeMask[ch] &= ~(1 << strobe);
          }
        } // strobe loop

        //
        // Inidicate this rank is present on at least one channel
        //
        rankPresent = 1;

        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, 0));

        strobePass[ch] = 0;
        strobeFail[ch] = strobeMask[ch];
      } // ch loop

      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (!rankPresent) continue;

      // Setup IO test CmdPat=PatWrRd, NumCL=4, LC=4, REUTAddress, SOE=3,
      // WDBPattern, EnCADB=0, EnCKE=0, SubSeqWait=0 )
      chBitmask = GetChBitmask (host, socket, dimm, rank);

      WDBPattern.IncRate = 0;
      CHIP_FUNC_CALL(host, SetupIOTest (host, socket, chBitmask, PatRdWrRd, FLY_BY_CACHELINES, 1, &CPGCAddress, NSOE, &WDBPattern, 0, 0, CPGC_SUBSEQ_WAIT_DDRT));

      CHIP_FUNC_CALL(host, SetupChipPatternSequencer(host, socket, ch, dimm, rank));

      //
      // Sweep through the cycle offsets until we find a value that passes
      //

      for (offset = 0; offset < (sizeof (Offsets)) / sizeof (INT16); offset++) {
        //
        // Program new delay offsets to DQ/DQS timing:
        //
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
          "Current DQS offset delay is %d DClks (DQS offset index=%d)\n", Offsets[offset], offset));

        chBitmask = GetChBitmask(host, socket, dimm, rank);
        for (ch = 0; ch < MAX_CH; ch++) {
          if ( !((1 << ch) & chBitmask )) continue;

          dimmNvList = GetDimmNvList(host, socket, ch);
          if (rank >= (*dimmNvList)[dimm].numRanks) continue;

          CHIP_FUNC_CALL(host, SetChipDQDQSTiming(host, socket, ch, Offsets[offset], &CRAddDelay[ch], &GlobalByteOff, &WLChipCleanUpStruct));

          //Write Tx DQ/DQS Flyby delays
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "Write Tx DQ/DQS Flyby delay: GlobalByteOff = %d, CRAddDelay = %d\n", GlobalByteOff, CRAddDelay[ch]));
          maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));

          for (strobe = 0; strobe < maxStrobe; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            //read cache
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));

            txDqs = txDqs + GlobalByteOff;

            //write csr
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE, (INT16 *)&txDqs));
            if (maxStrobe == 9) {
              //write csr
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 0, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE, (INT16 *)&txDqs));
            }
            if (host->var.mem.xoverModeVar == XOVER_MODE_2TO2) {
              txDq = txDqs + TX_DQS_DQ_OFFSET_2TO2 + Offset;
            } else {
              txDq = txDqs + TX_DQS_DQ_OFFSET - TX_PER_BIT_SETTLE_TIME;
            }
            //read csr, write csr
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_CSR | GSM_FORCE_WRITE, (INT16 *)&txDq));
            if (maxStrobe == 9) {
              //read cache, write csr
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqDelay, GSM_READ_CSR | GSM_FORCE_WRITE, (INT16 *)&txDq));
            }
            //read csr
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY | GSM_READ_CSR, (INT16 *)&txDqs));

          } // strobe loop

          // Write Wr ADD Delays
          CHIP_FUNC_CALL(host, WriteChipWrADDDelays(host, socket, ch, Offsets[offset], CRAddDelay, &WLChipCleanUpStruct));
        } // ch loop

        if (chBitmask == 0) {
          Done = 0;
        } else {
          //
          // Run Test across all DqOffsets points
          //
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "\t\tDqs Index\tDqs Offset (DClks)\tDqOffset (PI)\tPrev Pass\tCurrent Error\tstrobePass\n"));
#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MAX)) {
            PrintLine(host, 115, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
          }
#endif // SERIAL_DBG_MSG

          for (DqOffset = 0; DqOffset < (sizeof (DqOffsets)) / sizeof (INT16); DqOffset++) {
            // Update Offset
            for (ch = 0; ch < MAX_CH; ch++) {
              if ( !((1 << ch) & chBitmask )) continue;
              dimmNvList = GetDimmNvList(host, socket, ch);
              if (rank >= (*dimmNvList)[dimm].numRanks) continue;
              maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));

              for (strobe = 0; strobe < maxStrobe; strobe++) {
                //
                // Skip if this is an ECC strobe when ECC is disabled
                //
                if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

                //read csr
                CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY | GSM_READ_CSR, (INT16 *)&txDqs));

                if (host->var.mem.xoverModeVar == XOVER_MODE_2TO2) {
                  txDq = txDqs + TX_DQS_DQ_OFFSET_2TO2 + (INT16) DqOffsets[DqOffset] + Offset;
                } else {
                  txDq = txDqs + TX_DQS_DQ_OFFSET - TX_PER_BIT_SETTLE_TIME + (INT16) DqOffsets[DqOffset];
                }
                //read csr, write csr
                CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_CSR | GSM_FORCE_WRITE, &txDq));
                if (maxStrobe == 9) {
                  //read csr, write csr
                  CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqDelay, GSM_READ_CSR | GSM_FORCE_WRITE, (INT16 *)&txDq));
                }
              } // strobe loop
            } // ch loop

            //
            // Reset the IO
            //
            CHIP_FUNC_CALL(host, IO_Reset (host, socket));

            // Run Test
            //DQPat = BasicVA, DumArr, ClearErrors = 1, mode = 0
            CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

            // Update results for all ch/bytes
            Done = 1;

            // Update results for all ch/bytes
            for (ch = 0; ch < MAX_CH; ch++) {
              if (!((1 << ch) & chBitmask)) continue;

              rankList = GetRankNvList(host, socket, ch, dimm);

              chStatus = 1 << ch;
              bwSerr[ch][0] = 0;
              bwSerr[ch][1] = 0;
              bwSerr[ch][2] = 0;
              CollectTestResults (host, socket, chStatus, bwSerr);
              // Read out per byte error results and check for any byte error
              Result = 0;
              maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));
              for (strobe = 0; strobe < maxStrobe; strobe++) {

                bit = (strobe - 9 * (strobe >= 9))*8 + 4*(strobe >= 9);

                if (bit < 32) errResult = bwSerr[ch][0];
                else if (bit < 64) errResult = bwSerr[ch][1];
                else errResult = bwSerr[ch][2];
//              if (errResult & (1 << (strobe * 2 - (strobe >= 9) * 17))) {
                if (errResult & (1 << (bit))) {
                  Result |= 1 << strobe;
                }
              } // strobe loop

              SkipMe = (Result & strobeMask[ch]) | strobePass[ch];

              for (strobe = 0; strobe < maxStrobe; strobe++) {
                //
                // Skip if this is an ECC strobe when ECC is disabled
                //
                if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

                // If this byte has failed or previously passed, nothing to do
                if (SkipMe & (1 << strobe)) continue;

                strobePass[ch] |= (1 << strobe);
                ByteOff[ch][(*rankList)[rank].rankIndex][strobe] = Offsets[offset];
                ByteSum[ch] += Offsets[offset];
                ByteCount[ch] = ByteCount[ch] + 1;
              } // strobe loop

              if (SkipMe == strobePass[ch] ){
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "\t%2d\t\t%2d\t\t\t%2d\t\tYES\t\t0x%05x\t\t0x%05x\n", Offsets[offset], offset, DqOffsets[DqOffset], Result, strobePass[ch] ));
              } else {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                  "\t%2d\t\t%2d\t\t\t%2d\t\tNO\t\t0x%05x\t\t0x%05x\n", Offsets[offset], offset, DqOffsets[DqOffset], Result, strobePass[ch] ));
              }

              strobeFail[ch] &= ~strobePass[ch];

              if (strobePass[ch] != strobeMask[ch]) Done = 0;
            } // ch loop
            // Jump out of the for DqOffset loop if everybody is passing
            if (Done == 1) break;
          } // DqOffset loop
        } // chBitmask
        // Jump out of the for offset loop if everybody is passing
        if (Done == 1) break;
      } // offset loop
    } // rank loop
  } // dimm loop

  //
  // Evaluate the data
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    CHIP_FUNC_CALL(host, GetChipTimingData(host, socket, ch, &WLChipCleanUpStruct));

    // Caclulate the average offset, rounding up
    // Apply that offset to the global MC CRAddDelay register
    if (ByteCount[ch]) {
      TargetOffset = (ByteSum[ch] + (ByteCount[ch]/2)) / ByteCount[ch];
    }

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, WR_FLY_BY, (UINT16)((SUB_WR_FLY_BY_EVALUATE << 12)|(socket << 8)|(dimm << 4)|rank)));

        if (Done == 0) {
          maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));
          for (strobe = 0; strobe < maxStrobe; strobe++) {

            if (strobeFail[ch] & (1 << strobe)){
              host->var.mem.faultyPartsFlag[ch] |= (1 << strobe);
              (*rankList)[rank].faultyParts[strobe] |= FPT_WR_FLY_BY_FAILED;

              faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);

              if (faultyPartsStatus == FPT_NO_ERROR) {
                // do we need to do anything here ?!?
              } else if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
                // if correctable  log the warning -> OutputWarning
                // careful for cases when warning gets promoted to error !!!  - log the strobe
                MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, strobe, NO_BIT,
                             "Failed Write Fly By\n"));
                EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_WR_FLYBY, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, WrLvlDelay, DdrLevel, 0xFF);
              } else {
                // does anything need to be done here?
                MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                              "Write Fly By training failure!!!\n"));
                //
                // Add this error to the warning log for both correctable and uncorrectable errors.
                //
                DisableChannel(host, socket, ch);
                EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_WR_FLYBY, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, WrLvlDelay, DdrLevel, 0xFF);
              }
            }
          } // strobe loop
        } // if Done == 0

        AllGood = 0;
        AllGoodLoops = 0;

        while (AllGood == 0) {

          CHIP_FUNC_CALL(host, UpdateGlobalOffsets(host, ch, TargetOffset, &CRAddDelay[ch], &GlobalByteOff));
#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MAX)) {
            PrintLine(host, 80, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
          }
#endif //SERIAL_DBG_MSG

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "Current values: GlobalByteOff = %d DClks, CRAddDelay[%d] = %d DClks\n", GlobalByteOff, ch, CRAddDelay[ch]));

          // Refine TargetOffset to make sure it works for all byte lanes
          AllGood = 1;
          maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "Refining TargetOffset for all byte lanes...\n"));
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\t\tTgtOffset\tByteOff\n"));
#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MAX)) {
            PrintLine(host, 80, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
          }
#endif //SERIAL_DBG_MSG
          for (strobe = 0; strobe < maxStrobe; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
            if ((*rankList)[rank].faultyParts[strobe]) continue;

            LocalOffset = GlobalByteOff + (128 * (ByteOff[ch][(*rankList)[rank].rankIndex][strobe] - TargetOffset));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
            // 447 on TXDQS should give enough room for DQ/DQS margins
            if ((txDqs + LocalOffset) > 447) {
              AllGood = 0;
              AllGoodLoops += 1;
              TargetOffset += 1;
              break;
            }
            if ((txDqs + LocalOffset) < 40) {
              AllGood = 0;
              AllGoodLoops += 1;
              TargetOffset -= 1;
              break;
            }
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "%d DClks\t%d DClks\n", TargetOffset, ByteOff[ch][(*rankList)[rank].rankIndex][strobe]));
          } // strobe loop

          // Avoid an infinite loop
          if (AllGoodLoops > 3) {
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                           "TargetOffset refining failed \n"));
            EwlOutputType2(host, WARN_WR_LEVEL, WARN_WR_FLYBY_UNCORR, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, WrLvlDelay, DdrLevel, 0xFF);

            //
            // This is a fatal error condition so map out the entire channel.
            //
            DisableChannel(host, socket, ch);
            AllGood = 1;
          }
        } // while AllGood
      } // rank loop
    } // dimm loop

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        // Program the final settings to the DQ bytes and update MrcData
        maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\t\tTxDq (PI)\tTxDqs (PI)\tPI Offset\n"));
#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          PrintLine(host, 80, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
        }
#endif //SERIAL_DBG_MSG
        for (strobe = 0; strobe < maxStrobe; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          if ((*rankList)[rank].faultyParts[strobe]) continue;

          LocalOffset = GlobalByteOff + (128 * (ByteOff[ch][(*rankList)[rank].rankIndex][strobe] - TargetOffset));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
          txDqs = txDqs + LocalOffset;
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&txDqs));
          if (maxStrobe == 9) {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqsDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&txDqs));
          }

          if (host->var.mem.xoverModeVar == XOVER_MODE_2TO2) {
            txDq = txDqs + TX_DQS_DQ_OFFSET_2TO2 + Offset;
          } else {
            txDq = txDqs + TX_DQS_DQ_OFFSET;
          }
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&txDq));
          if (maxStrobe == 9) {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&txDq));
          }
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "%3d\t\t%3d\t\t%3d\n", txDq, txDqs, LocalOffset));
        } // strobe loop
#ifdef  LRDIMM_SUPPORT
        // LRDIMM XTALK OPTIMIZATION
        if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))){
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "Tx Skew Adjust\n"));
          for (strobe = 0; strobe < 9; strobe++) {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&lower));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&upper));
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "Before (Lower nibble) = %2d (Upper nibble) = %2d\n", lower, upper));
            center = (lower+upper)/2;
            lower = center;
            upper = center;
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&lower));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqsDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&upper));
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "After  (Lower nibble) = %2d (Upper nibble) = %2d\n", lower, upper));

            if (host->var.mem.xoverModeVar == XOVER_MODE_2TO2) {
              lower += TX_DQS_DQ_OFFSET_2TO2;
              upper += TX_DQS_DQ_OFFSET_2TO2;
            } else {
              lower += TX_DQS_DQ_OFFSET;
              upper += TX_DQS_DQ_OFFSET;
            }
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&lower));
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, (INT16 *)&upper));
          }
        }
#endif
      } // rank loop
    } // dimm loop

    // Update MC Delay
    CHIP_FUNC_CALL(host, UpdateChipMCDelay(host, socket, ch, channelNvList, CRAddDelay, &WLChipCleanUpStruct));
  } // ch loop

  CHIP_FUNC_CALL(host, DoFinalChipSpecificWLCleanUp(host, socket));

  //
  // Reset the IO
  //
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));

  host->nvram.mem.hostFlybyDone = 1;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    DisplayResults(host, socket, TxDqsDelay);
  }
#endif

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WriteLevelingCleanUp Ends\n"));
#endif

  return SUCCESS;
} // WriteLevelingCleanUp
