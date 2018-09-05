//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/05/2017
//
//*****************************************************************************
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
static
void
FindPiSetting (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     dimm,
              UINT8     rank,
              UINT16    risingEdge[MAX_CH][MAX_STROBE],
              UINT16    centerPoint[MAX_CH][MAX_STROBE],
              UINT16    fallingEdge[MAX_CH][MAX_STROBE],
              UINT16    pulseWidth[MAX_CH][MAX_STROBE]
              );
static
void
PISettingAdjustment (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     dimm,
                    UINT8     rank
                    );

static
void
FindRoundTripLatency (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT8     dimm,
                     UINT8     rank,
                     UINT16    centerPoint[MAX_CH][MAX_STROBE]
                     );

void
GetResults (
  PSYSHOST host,
  UINT8 socket,
  UINT8 dimm,
  UINT8 rank,
  UINT16 piDelay
  );

void
FindMaxMinLogic (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT8     dimm,
                UINT8     rank,
                UINT8     MaxSdramInDimm,
                UINT8     *MaxLogicDelay,
                UINT8     *MinLogicDelay
                );



/**

  Perform Receive Enable Training

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
ReceiveEnable (
              PSYSHOST  host
              )
{
  UINT8                             socket;
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             logRank = 0;
  UINT8                             strobe = 0;
#ifdef SERIAL_DBG_MSG
  INT16                             piDelay;
#endif // SERIAL_DBG_MSG
  UINT8                             numRanks;
  UINT8                             temp;
  UINT8                             rtError;
  UINT8                             ioCompDiff;
  UINT8                             MaxLogicDelay;
  UINT8                             MinLogicDelay;
  UINT8                             rankPresent;
  UINT16                            risingEdge[MAX_CH][MAX_STROBE];
  UINT16                            centerPoint[MAX_CH][MAX_STROBE];
  UINT16                            fallingEdge[MAX_CH][MAX_STROBE];
  UINT16                            pulseWidth[MAX_CH][MAX_STROBE];
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct RankCh                     (*rankPerCh)[MAX_RANK_CH];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT32                            tRRD_s_org[MAX_CH];
#ifdef LRDIMM_SUPPORT
  UINT8                             controlWordData;
#endif // LRDIMM_SUPPORT

  socket = host->var.mem.currentSocket;

  //
  // Return if socket is disable or No DIMMs found or RECVEN training disabled
  //
  if ((~host->memFlows & MF_REC_EN_EN) || (host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;


  ioCompDiff = MEM_CHIP_POLICY_VALUE(host, mrcRoundTripIoComp) - MEM_CHIP_POLICY_VALUE(host, mrcRoundTripIoCompStart);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "ReceiveEnable Starts\n"));
#endif // PRINT_FUNC

  for (ch = 0; ch < MAX_CH; ch++) {
    tRRD_s_org[ch] = 0;
  }

  //
  // Turn on MPR pattern for all ranks
  //
  SetAllRankMPR (host, socket, MR3_MPR);

  CHIP_FUNC_CALL(host, ReceiveEnableInitAllRanks (host, socket));

  channelNvList = GetChannelNvList(host, socket);

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    numRanks = CHIP_FUNC_CALL(host, NumRanksToTrain(host, socket, host->var.mem.socket[socket].maxRankDimm));
    for (rank = 0; rank < numRanks; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, RECEIVE_ENABLE_BASIC, (UINT16)((socket << 8)|(dimm << 4)|rank)));
      //
      // Initialize to no ranks present
      //
      rankPresent = 0;

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);
        rankStruct    = GetRankStruct(host, socket, ch, dimm);

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;
        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);

        CHIP_FUNC_CALL(host, ReceiveEnableInitPerCh (host, socket, ch, dimm, rank));

        CHIP_FUNC_CALL(host, RankRoundTrip (host, socket, ch, dimm, rank));
        CHIP_FUNC_CALL(host, SetActToActPeriod(host, socket, ch, tRRD_s_org));
        //
        // Indicate this rank is present on at least one channel
        //
        rankPresent = 1;

        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, 0));

        if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
          //
          // Enable Receive Enable Mode
          //

          WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR4 | MR4_RE, BANK4);


          CHIP_FUNC_CALL(host, SetLongPreamble( host, socket, ch, TRUE));

#ifdef  LRDIMM_SUPPORT
          if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))){
            //F0BC1x = 00010000b
            // enable read preamble training mode in the buffer
            (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x | LRDIMM_RD_PREAMBLE_TRAINING_MODE;
             controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
            WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);
          }
#endif // LRDIMM_SUPPORT
        } // DDR4

        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, !(*dimmNvList)[dimm].aepDimmPresent));
      } // ch loop

      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (!rankPresent) continue;

      CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, RECEIVE_ENABLE_BASIC));

      //
      // Find the DQS high phase
      //
      FindPiSetting (host, socket, dimm, rank, risingEdge, centerPoint, fallingEdge, pulseWidth);

#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        getPrintFControl (host);
      }
#endif // SERIAL_DBG_MSG
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        //
        // Preparation for back in time
        //
        rankPerCh = &(*channelNvList)[ch].rankPerCh;

        //
        // Write the PI setting to MC
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_UPDATE_CACHE,
                           (INT16 *)&centerPoint[ch][strobe]));
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "Rec En Delay %d\n", centerPoint[ch][strobe]));
        } // strobe loop

        CHIP_FUNC_CALL(host, DecIoLatency( host, socket, ch, dimm, rank, centerPoint));

        CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));
        CHIP_FUNC_CALL(host, SetIOLatency (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency));
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "Round trip latency = %d\nIO latency = %d\n",
                       (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip, (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency));
      } // ch loop

      //
      // Find out the round trip latency
      //
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                     "\nRound trip latency\n"));
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        releasePrintFControl (host);
      }
#endif // SERIAL_DBG_MSG
      FindRoundTripLatency (host, socket, dimm, rank, centerPoint);

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        rankPerCh = &(*channelNvList)[ch].rankPerCh;

        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, 0));
        //
        // Find diff IO latency
        //
        FindMaxMinLogic (host, socket, ch, dimm, rank, MAX_STROBE, &MaxLogicDelay, &MinLogicDelay);

        //
        // Setup the new Pi setting
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          centerPoint[ch][strobe] = centerPoint[ch][strobe] + 32;
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_UPDATE_CACHE,
                           (INT16 *)&centerPoint[ch][strobe]));
        } // strobe loop

        FindMaxMinLogic (host, socket, ch, dimm, rank, MAX_STROBE, &MaxLogicDelay, &MinLogicDelay);

#ifdef  LRDIMM_SUPPORT
        if (CHIP_FUNC_CALL(host, IsLrdimmPresent(host, socket, ch, dimm))&& (host->nvram.mem.dramType != SPD_TYPE_DDR3)) {
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
#endif // LRDIMM_SUPPORT
        //
        // If smallest logic-delay 2 or more error in procedure
        //
        if (MinLogicDelay > 2) {
          (*rankList)[rank].faultyParts[0] |= FPT_REC_ENABLE_FAILED;
          MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "ERROR: smallest logic-delay > 2, it is %d\n", MinLogicDelay));
          DisableChannel(host, socket, ch);
#if SMCPKG_SUPPORT
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_RD_RCVEN, socket, ch, dimm, NO_RANK, EwlSeverityWarning, NO_STROBE, NO_BIT, RecEnDelay, DdrLevel, 0xFF);
#else
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_RD_RCVEN, socket, ch, NO_DIMM, NO_RANK, EwlSeverityWarning, NO_STROBE, NO_BIT, RecEnDelay, DdrLevel, 0xFF);
#endif

        }

        /*
        //
        // At the end, if lowest logic-delay is more then 0, reduce IO-latency by MinLogicDelay, and decrement all logic-delays by MinLogicDelay
        //
        if (MinLogicDelay > 0) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "MinLogicDelay = \n", MinLogicDelay));
          (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency = (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency - MinLogicDelay;

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            //
            // Do not check if this part is faulty
            //
            if ((*rankList)[rank].faultyParts[strobe] != 0) {
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "FAULTY_PARTS_TRACKING: skipped Rec Enable MinLogicDelay\n"));
              continue;
            }

            tmpLogicDelay = GetDDRIO(host, socket, ch, dimm, rank, strobe, 0, DDRIO_DDR, DDRIO_REC_EN_DELAY, CACHED, 0);
            tmpLogicDelay  = tmpLogicDelay - (MinLogicDelay * 64);
            SetDDRIO (host, socket, ch, dimm, rank, strobe, 0, DDRIO_DDR, DDRIO_REC_EN_DELAY, CACHED, 1, tmpLogicDelay);
          } // strobe loop
        }

        //
        // TODO: Check if we need to program RTL here
        //
        CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));
        CHIP_FUNC_CALL(host, SetIOLatency (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency));
         */

#ifdef SERIAL_DBG_MSG
        //
        // Summarization
        //
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          if (host->var.mem.earlyCtlClkSerialDebugFlag) {
            rcPrintf ((host, "\n Early CTL CLK Receive Enable Summary\n"));
          } else {
            rcPrintf ((host, "\n Receive Enable Summary\n"));
          }
          rcPrintf ((host, "------------------------\n"));
          if (host->var.mem.earlyCtlClkSerialDebugFlag) {
            rcPrintf ((host, "START_DATA_EARLY_CTL_CLK_REC_EN\n"));
          } else {
            rcPrintf ((host, "START_DATA_REC_EN_BASIC\n"));
          }
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY, &piDelay));
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT, "Pi setting = %d\n", piDelay));
          } // strobe loop

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "IO Latency = %d\n", (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency));
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                        "Round Trip = %d\n", (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip + ioCompDiff));
          if (host->var.mem.earlyCtlClkSerialDebugFlag) {
            rcPrintf ((host, "STOP_DATA_EARLY_CTL_CLK_REC_EN\n"));
          } else {
            rcPrintf ((host, "STOP_DATA_REC_EN_BASIC\n"));
          }
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
      } // ch loop

      //
      // Notify IO to move out of RCVEN training mode
      //
      CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, CLEAR_MODE));

      if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

          dimmNvList = GetDimmNvList(host, socket, ch);
          rankStruct    = GetRankStruct(host, socket, ch, dimm);

          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

          //
          // Enable Receive Enable Mode
          //

          WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR4, BANK4);


          CHIP_FUNC_CALL(host, SetLongPreamble( host, socket, ch, FALSE));
        } // ch loop
      } // if DDR4
#ifdef SERIAL_DBG_MSG
      releasePrintFControl (host);
#endif  // SERIAL_DBG_MSG
    } // rank loop
  } // dimm loop

  //
  // Set IO compensation delay to the final value
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, 0));
    rtError = 0;

    rankPerCh = &(*channelNvList)[ch].rankPerCh;
    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);

        temp = (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip;

        // If RT value + IO comp diff <= 55, then update RT = RT + IO comp diff
        if (temp + ioCompDiff <= MEM_CHIP_POLICY_VALUE(host, mrcRoundTripMax)) {
          temp = temp + ioCompDiff;
          (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip = temp;
          CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, logRank, temp));
        }

        // Otherwise indicate error
        else {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "Round Trip delay %d exceeds limit %d\n",
                         temp + ioCompDiff, MEM_CHIP_POLICY_VALUE(host, mrcRoundTripDefault)));
          EwlOutputType2(host, WARN_RD_RCVEN, WARN_ROUNDTRIP_EXCEEDED, socket, ch, dimm, rank, EwlSeverityWarning, NO_STROBE, NO_BIT, RecEnDelay, DdrLevel, 0xFF);
          rtError = 1;
        }
      } // rank loop
    } // dimm loop
    CHIP_FUNC_CALL(host, SetIOLatencyComp (host, socket, ch, rtError, tRRD_s_org));
  } // ch loop

  CHIP_FUNC_CALL(host, ReceiveEnableCleanup(host, socket));

  //
  // Set nodata_trn_en
  //
  CHIP_FUNC_CALL(host, ProgramNoDataTrnEn(host, socket, 1));

  //
  // Turn off MPR pattern for all ranks
  //
  SetAllRankMPR (host, socket, 0);

  //
  // Set SenseAmpDelay and ODT Delay and durations
  //
  CHIP_FUNC_CALL(host, SetSenseAmpODTDelays(host, socket));

  //
  // Reset DDR IO
  //
  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "ReceiveEnable Ends\n"));
#endif

  return SUCCESS;
} // ReceiveEnable


/**

  Find the edges of the signal

  @param host              - Pointer to sysHost
  @param socket              - Socket number
  @param dimm              - DIMM number
  @param rank              - Rank number
  @param risingEdge        - Pointer to save the rising edge
  @param centerPoint       - Pointer to save the center point
  @param fallingEdge       - Pointer to save the falling edge
  @param pulseWidth        - Pointer to save the pulse width

  @retval N/A

**/
static
void
FindPiSetting (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     dimm,
              UINT8     rank,
              UINT16    risingEdge[MAX_CH][MAX_STROBE ],
              UINT16    centerPoint[MAX_CH][MAX_STROBE],
              UINT16    fallingEdge[MAX_CH][MAX_STROBE],
              UINT16    pulseWidth[MAX_CH][MAX_STROBE]
              )
{
  UINT8               ch;
  UINT8               strobe;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  UINT8               faultyPartsStatus;

  faultyPartsStatus = FPT_NO_ERROR;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Starting by sampling all the SDRAM present
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*dimmNvList)[dimm].dimmPresent == 0) {
      host->var.mem.piSettingStopFlag[ch] = 0;
      continue;
    }

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) {
      host->var.mem.piSettingStopFlag[ch] = 0;
      continue;
    }

    if (host->nvram.mem.eccEn) {
      host->var.mem.piSettingStopFlag[ch] = 0x3FFFF;
    } else {
      host->var.mem.piSettingStopFlag[ch] = 0x1FEFF;
    }
  } // ch loop

  PISettingAdjustment (host, socket, dimm, rank);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      getPrintFControl (host);
      if (host->var.mem.earlyCtlClkSerialDebugFlag) {
        rcPrintf ((host, "\nSummary: Early Ctl Clk Receive Enable Pi\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
      } else {
        rcPrintf ((host, "\nSummary: Receive Enable Pi\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
      }
      PrintSampleArray (host, socket, ch, dimm, 1);
      releasePrintFControl (host);
    }
#endif // SERIAL_DBG_MSG
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      EvaluatePiSettingResults (host, socket, ch, dimm, rank, strobe, risingEdge, centerPoint, fallingEdge, pulseWidth,
                                FPT_PI_READ_TYPE);

      if (host->var.mem.faultyPartsFlag[ch] & (1 << strobe)) {
        //
        // Add this error to the warning log for both correctable and uncorrectable errors.
        //
        faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);

        if (faultyPartsStatus == FPT_NO_ERROR) {
          // do we need to do anything here ?!?
        } else if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
          // if correctable  log the warning -> OutputWarning
          // careful for cases when warning gets promoted to error !!!
          EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_RD_RCVEN, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, RecEnDelay, DdrLevel, (UINT8)pulseWidth[ch][strobe]);
        } else {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "RecEn training failure!!!\n"));
          //
          // Add this error to the warning log for both correctable and uncorrectable errors.
          //
          host->var.mem.piSettingStopFlag[ch] |= (1 << strobe);
          DisableChannel(host, socket, ch);
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_RD_RCVEN, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, RecEnDelay, DdrLevel, (UINT8)pulseWidth[ch][strobe]);
        }
      }
    } // strobe loop

#ifdef SERIAL_DBG_MSG
    DisplayEdges(host, socket, ch, pulseWidth[ch], fallingEdge[ch], risingEdge[ch], centerPoint[ch], MAX_STROBE);
#endif // SERIAL_DBG_MSG
  } // ch loop

  return;
} // FindPiSetting

/**

  This function change 128 times the pi setting for each rank channel.

  @param host              - Pointer to sysHost
  @param socket              - Socket number
  @param dimm              - DIMM number
  @param rank              - Rank number

  @retval N/A

**/
static
void
PISettingAdjustment (
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
  UINT16                  fineDelay;
  UINT16                  fineDelayEnd;
  struct channelNvram     (*channelNvList)[MAX_CH];
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  struct TrainingResults  (*trainRes)[MAX_CH][MAX_STROBE];
  UINT8                   DumArr[7] = {1,1,1,1,1,1,1};

  channelNvList = GetChannelNvList(host, socket);
  trainRes      = &host->var.mem.trainRes;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    //
    // Clear training results
    //
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      for (dWord = 0; dWord < 4; dWord++) {
        (*trainRes)[ch][strobe].results[dWord] = 0;
      } // dWord loop
    } // strobe loop
  } // ch loop

  chBitmask = GetChBitmask (host, socket, dimm, rank);

  SetupRecEnFineTest(host, socket, chBitmask);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                 "RecEn Pi Scanning:\n"));

  fineDelayEnd = REC_EN_PI_START + REC_EN_PI_RANGE;

  for (fineDelay = REC_EN_PI_START; fineDelay < fineDelayEnd; fineDelay += REC_EN_STEP_SIZE) {

    //
    // Change the Receive Enable PI Setting
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      // Continue to the next rank if this one is disabled
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, RecEnDelay, GSM_UPDATE_CACHE,
                       (INT16 *)&fineDelay));
    } // ch loop

    //
    // Send a burst of 7 read commands back to back (4 DCLK apart) using MC_SPECIAL_COMMAND CRs
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

    GetResults(host, socket, dimm, rank, fineDelay);

  } // fineDelay loop

  return;
} // PISettingAdjustment

/**

  This function gets the test engine ready to run tests for Receive Enable Fine training

  @param host              - Pointer to sysHost
  @param socket              - Socket number
  @param chBitmask - Bitmask of channels to test

  @retval N/A

**/
void
SetupRecEnFineTest (
                   PSYSHOST                            host,
                   UINT8                               socket,
                   UINT32                              chBitmask
                   )
{
  UINT8               loopCount;
  UINT8               numCl;
  //                                Rank, Bank, Row, Col
  TCPGCAddress        CPGCAddress = {{0,   0,    0,   0},   // Start
                                     {0,   4,    0,   1023}, // Stop
                                     {0,   0,    0,   0},    // Order
                                     {0,   0,    0,   0},    // IncRate
                                     {0,   4,    0,   1}};   // IncValue
                               // IncRate, Start, Stop, DQPat
  TWDBPattern         WDBPattern  = {32,       0,   9, BasicVA}; // Should not matter what this is. We do not compare data.

  CHIP_FUNC_CALL(host, SetupRecEnFineParam(host, &CPGCAddress, &WDBPattern, &loopCount, &numCl));
  // CmdPat=PatRd, NumCL=8, LC=7, CPGCAddress, SOE=0,
  // WDBPattern, EnCADB=0, EnCKE=0, SubSeqWait=8
  CHIP_FUNC_CALL(host, SetupIOTest(host, socket, chBitmask, PatRd, numCl, loopCount, &CPGCAddress, 0, &WDBPattern, 0, 0, 4));


} // SetupRecEnFineTest

/**

 For each data module, a 128 bit vector of zeros and ones holding the tests results is
 evaluated which is a concatenation of the two reads from the feedback CRs.
 BIOS locates the middle of the longest consecutive stream of ones (when the vector is evaluated as cyclic,
 this location in the vector indicates the PI setting and logic delay for that data module).

  @param host              - Pointer to sysHost
  @param socket            - Current Socket
  @param ch                - Channel number
  @param dimm              - Dimm number on ch
  @param rank              - Rank on dimm
  @param strobe            - Strobe number
  @param risingEdge        - Pointer to save the rising edge
  @param centerPoint       - Pointer to save the center point
  @param fallingEdge       - Pointer to save the falling edge
  @param pulseWidth        - Pointer to save the pulse width
  @param step              - The PI Step to work on:
                               FPT_PI_READ_TYPE FPT_PI_WRITE_TYPE
                               FPT_PI_LRDIMM_READ_TYPE
                               FPT_PI_LRDIMM_WRITE_TYPE
                               FPT_PI_LRDIMM_RD_MRD_TYPE
                               FPT_PI_LRDIMM_WR_MRD_TYPE

  @retval N/A

**/
void
EvaluatePiSettingResults (
                         PSYSHOST  host,
                         UINT8     socket,
                         UINT8     ch,
                         UINT8     dimm,
                         UINT8     rank,
                         UINT8     strobe,
                         UINT16    risingEdge[MAX_CH][MAX_STROBE],
                         UINT16    centerPoint[MAX_CH][MAX_STROBE],
                         UINT16    fallingEdge[MAX_CH][MAX_STROBE],
                         UINT16    pulseWidth[MAX_CH][MAX_STROBE],
                         UINT8     step
                         )
{
  UINT8                   encodedStrobe;
  UINT16                  piSetting;
  UINT16                  piStart;
  UINT32                  Register;
  UINT32                  Bit;
  UINT8                   CyclicCase;
  UINT16                  OneCount;
  UINT16                  OneStart;
  UINT16                  NumberOfCycle;
  UINT16                  MaxOneCount;
  UINT8                   dWord;
  struct ddrRank          (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  struct TrainingResults  (*trainRes)[MAX_CH][MAX_STROBE];

  Register  = 0;
  OneStart  = 0;

  Bit                     = BIT0;
  OneCount                = 0;
  MaxOneCount             = 0;
  CyclicCase              = 0;
  trainRes                = &host->var.mem.trainRes;

  risingEdge[ch][strobe]  = 0;
  fallingEdge[ch][strobe] = 0;
  pulseWidth[ch][strobe]  = 0;

  dimmNvList = GetDimmNvList(host, socket, ch);
  rankList = GetRankNvList(host, socket, ch, dimm);

  if ((*dimmNvList)[dimm].x4Present) {
    if (strobe % 2) {
      encodedStrobe = strobe + 9 - ((strobe + 1) / 2);
    } else {
      encodedStrobe = strobe;
    }
  } else {
    encodedStrobe = strobe;
  }

  //
  // Start at bit 0
  //
  CHIP_FUNC_CALL(host, SetupPiSettingsParams(host, socket, step,  &piSetting, &piStart, &NumberOfCycle, &trainRes));

  while (host->var.mem.piSettingStopFlag[ch] & (1 << strobe)) {

    dWord = CHIP_FUNC_CALL(host, GetTrainingResultIndex(step, piSetting, piStart));

    //
    // Get the proper training results
    //
    if ((piSetting % 32) == 0) {
      Register  = (*trainRes)[ch][strobe].results[dWord];
      Bit       = BIT0;
    }
    //
    // algo start
    //
    if (Register & Bit) {
      //
      // this index is 1
      //
      if (OneCount == 0) {
        //
        // This first one so save the starting point
        //
        OneStart = piSetting;
      }

      OneCount++;
    } else {
      //
      // this index is 0
      //
      if (OneCount > 0) {
        //
        // We just moved from a one to a zero so check if that was the largest stream of one's
        //
        if (MaxOneCount < OneCount) {
          //
          // TBD  if i have two max value equals
          //
          MaxOneCount             = OneCount;
          risingEdge[ch][strobe]  = OneStart;
          fallingEdge[ch][strobe] = piSetting;
        }
        //
        // Start counting again at 0
        //
        OneCount = 0;
      }
    }

    //
    // Go to the next bit
    //
    piSetting++;
    Bit = Bit << 1;

    //
    // Check if we're at the end of the cycle
    //
    if (piSetting == NumberOfCycle) {
      if (OneCount > 0) {
        //
        // Cyclic case continue to scan
        // Start back over with bit 0 if the last bit is a 1
        //
        piSetting = CHIP_FUNC_CALL(host, StartBackOver (step, piStart));
        CyclicCase  = 1;
      } else {
        //
        // stop the scan
        // We've checked all bits so save the results
        //
        host->var.mem.piSettingStopFlag[ch] &= ~(1 << strobe);
        pulseWidth[ch][strobe]  = fallingEdge[ch][strobe] - risingEdge[ch][strobe];
        centerPoint[ch][strobe] = ((pulseWidth[ch][strobe] / 2) + risingEdge[ch][strobe]) % (NumberOfCycle - piStart);
      }
    }
    //
    // Check if this is a cyclic case and if we are at the end of the one's
    //
    if ((CyclicCase == 1) && (OneCount == 0)) {
      host->var.mem.piSettingStopFlag[ch] &= ~(1 << strobe);
      if (risingEdge[ch][strobe] > fallingEdge[ch][strobe]) {
        pulseWidth[ch][strobe]  = ((NumberOfCycle - risingEdge[ch][strobe]) + fallingEdge[ch][strobe]) - piStart;
        centerPoint[ch][strobe] = (((pulseWidth[ch][strobe] / 2) + risingEdge[ch][strobe])) % (NumberOfCycle - piStart);
      } else {
        //
        // last bit is 1 and first bit after cycle is 0 bit 127 = 1 and bit 0 = 0
        //
        pulseWidth[ch][strobe]  = fallingEdge[ch][strobe] - risingEdge[ch][strobe];
        centerPoint[ch][strobe] = ((pulseWidth[ch][strobe] / 2) + risingEdge[ch][strobe]) % (NumberOfCycle - piStart);
      }
    }
  } // while loop

#ifdef FAULTY_PARTS_TRACKING_INJECT_ERROR
  if ((ch == 0) && (dimm == 0) && (rank == 1) && (strobe == 5)) {
    //_asm jmp $;
    pulseWidth[ch][strobe]  = 1;
    centerPoint[ch][strobe] = ((pulseWidth[ch][strobe] / 2) + risingEdge[ch][strobe]) % (NumberOfCycle - piStart);
  }
#endif // FAULTY_PARTS_TRACKING_INJECT_ERROR

  //
  // If the pulseWidth (ones stream) is smaller than threshold need to sample again
  //
  if ((step == FPT_PI_READ_TYPE) || (step == FPT_PI_WRITE_TYPE)) {
    if (pulseWidth[ch][strobe] <= RE_EDGE_SIZE_THRESHOLD) {

      //
      // This is a fatal error condition so map out the entire channel
      //
      host->var.mem.faultyPartsFlag[ch] |= (1 << encodedStrobe);
      if (step == FPT_PI_READ_TYPE) {
        (*rankList)[rank].faultyParts[encodedStrobe] |= FPT_REC_ENABLE_FAILED;

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, encodedStrobe, NO_BIT,
                       "Failed RecEn Pi\n"));
      }
      if (step == FPT_PI_WRITE_TYPE) {
        (*rankList)[rank].faultyParts[encodedStrobe] |= FPT_WR_LEVELING_FAILED;
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, encodedStrobe, NO_BIT,
                       "Failed WL Pi\n"));
        // Evaluate here for correctable error  ... possibly disable channel
      }
    }
  }

#ifdef LRDIMM_SUPPORT
  //
  // HSD4929028: Error checking for LRDIMM
  //
  if ((step == FPT_PI_LRDIMM_READ_TYPE) ||
      (step == FPT_PI_LRDIMM_WRITE_TYPE)){
    if (pulseWidth[ch][strobe] <= RE_LRDIMM_EDGE_SIZE_THRESHOLD_PHASE){
      host->var.mem.faultyPartsFlag[ch] |= (1 << encodedStrobe);
      (*rankList)[rank].faultyParts[encodedStrobe] |= FPT_LRDIMM_TRAINING_FAILED;
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, encodedStrobe, NO_BIT,
                   "Failed LRDIMM Pi\n"));
    }
  }
  if ((step == FPT_PI_LRDIMM_RD_MRD_TYPE) ||
      (step == FPT_PI_LRDIMM_WR_MRD_TYPE)){
    if (pulseWidth[ch][strobe] <= RE_LRDIMM_EDGE_SIZE_THRESHOLD){
      host->var.mem.faultyPartsFlag[ch] |= (1 << encodedStrobe);
      (*rankList)[rank].faultyParts[encodedStrobe] |= FPT_LRDIMM_TRAINING_FAILED;
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, encodedStrobe, NO_BIT,
                   "Failed LRDIMM Pi\n"));
    }
  }
#endif //Lrdimm_support
}

/**

  Once the DQS high phase has been found (for each DRAM) the next stage is to find out the round trip latency,
  by locating the preamble cycle. This is achieved by trying smaller and smaller roundtrip
  values until the strobe sampling is done on the preamble cycle.

  @param host              - Pointer to sysHost
  @param socket              - Socket number
  @param dimm              - DIMM number
  @param rank              - Rank number
  @param centerPoint       - Pointer to center point

  @retval N/A

**/
static
void
FindRoundTripLatency (
                     PSYSHOST                            host,
                     UINT8                               socket,
                     UINT8                               dimm,
                     UINT8                               rank,
                     UINT16                              centerPoint[MAX_CH][MAX_STROBE]
                     )
{
  UINT8               ch;
  UINT32              chBitmask;
  UINT8               strobe;
  UINT8               logRank = 0;
  UINT8               stopFlag;
  UINT8               haveZero;
  UINT16              tmpPiDelay;
  UINT32              feedBack = 0;
  UINT32              zeroFlag;
  UINT32              allZeros[MAX_CH];
  UINT32              ShiftIndex;
  UINT32              Register;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct RankCh       (*rankPerCh)[MAX_RANK_CH];
  UINT8               aux_strobe;
  UINT8               DumArr[7] = {1,1,1,1,1,1,1};
  UINT16              maxLimit = 0;
  UINT16              minLimit = 0;
  UINT16              usDelay = 0;

  Register      = 0;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    allZeros[ch] = 0;
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    //
    // Get the logical rank #
    //
    logRank = GetLogicalRank(host, socket, ch, dimm, rank);

    if (host->nvram.mem.eccEn) {
      allZeros[ch] = 0x3FFFF;
    } else {
      allZeros[ch] = 0x1FEFF;
    }
  } // ch loop

  chBitmask = GetChBitmask (host, socket, dimm, rank);
  CHIP_FUNC_CALL(host, GetDataGroupLimits(host, DdrLevel, RecEnDelay, &minLimit, &maxLimit, &usDelay)); // move this out of loops

  stopFlag = 0;

  while (stopFlag == 0) {
    //
    // Send a burst of 16 read commands back to back (4 DCLK apart)
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

    stopFlag = 1;

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((chBitmask & (1 << ch)) == 0) continue;

      //
      // Continue if this rank is not present
      //
      rankList = GetRankNvList(host, socket, ch, dimm);
      dimmNvList = GetDimmNvList(host, socket, ch);

      // Continue to the next rank if this one is disabled
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      rankPerCh = &(*channelNvList)[ch].rankPerCh;
      dimmNvList  = GetDimmNvList(host, socket, ch);

      haveZero    = 0;
      zeroFlag    = 0;

      //
      // BIOS reads result from IO CRs and extract the relevant bit for each data module.
      //
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // Skip if this is an ECC strobe when ECC is disabled
        //
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        ShiftIndex = centerPoint[ch][strobe] % RE_TRAINING_RESULT_HIGH_IN_BITS;

        //
        // Read the correct register to work on
        //
        feedBack = CHIP_FUNC_CALL(host, ReadTrainFeedback (host, socket, ch, strobe));

        //if (feedBack >= (UINT8)(1 << (REC_EN_LOOP_COUNT - 1))) {
        if (feedBack >= 1) {
          Register = 1 << ShiftIndex;
        } else {
          Register = 0;
        }
        if ((*rankList)[rank].faultyParts[strobe] != 0) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "Skip RecEn haveZero & zeroFlag\n"));
          // do nothing here we will fool the system that the faulty part is working later when we see other strobes to be working too
        } else {
          //
          // Test if DQS value is 0 or 1
          //
          if ((Register & (1 << ShiftIndex)) == 0) {
            //
            // SDRAM i index value is 0
            //
            haveZero = 1;
            zeroFlag |= 1 << strobe;

            // if some other strobe is ok lets append the faulty strobe as working too - to advance the algorithm
            for (aux_strobe = 0; aux_strobe < MAX_STROBE; aux_strobe++) {
              if ((*rankList)[rank].faultyParts[aux_strobe] != 0) {
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, aux_strobe, NO_BIT,
                               "Adding faulty strobe RecEn\n"));
                zeroFlag |= 1 << aux_strobe;
              }
            } // aux_strobe loop
          }
        }
      } // strobe loop

      if (haveZero) {
        if (zeroFlag != allZeros[ch]) {
          stopFlag = 0;

          //
          // increase the Rank # IO latency by 2 and continue to next iteration.
          //
          (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency = (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency + 2;
          CHIP_FUNC_CALL(host, SetIOLatency (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency));
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "IO Latency = %d\nzeroFlag = 0x%x, allZeros[%d] = 0x%x\n",
                         (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency, zeroFlag, ch, allZeros[ch]));

          //  If some are zero, on next iteration increase their RX RcvEnable Logic Delay settings by 2
          //
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            if (zeroFlag & (1 << strobe)) {
              //
              // Increase logic delay by 2
              //
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY,
                               (INT16 *)&tmpPiDelay));
              tmpPiDelay = tmpPiDelay + (2 * 64);

              //
              // TODO: Check against a max pi delay instead of of max logic delay
              //
              if (tmpPiDelay > maxLimit) {
                tmpPiDelay = maxLimit;
                MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                               "ERROR: Rec En Logic Delay is %d > %d *64!!!\n", tmpPiDelay, RE_MAX_LOGIC_DELAY));
                EwlOutputType2(host, WARN_RD_RCVEN, WARN_RCVEN_PI_DELAY_EXCEEDED, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, RecEnDelay, DdrLevel, 0xFF);
                DisableChannel(host, socket, ch);
              }

              centerPoint[ch][strobe] = tmpPiDelay;
              CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_UPDATE_CACHE,
                               (INT16 *)&tmpPiDelay));
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                             "Rec En Delay = %d\n", tmpPiDelay));
            } // ZeroFlag
          } // strobe loop
        } // IterationStop
        else {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "Round trip latency: Found all zeros\n"));
          //
          // if all are 0 stop the loop
          // No need to test this channel anymore
          //
          chBitmask &= ~(1 << ch);
        }
      } else {
        //
        // If all bits are one, decrement roundtrip value by DECREMENT_ROUNDTRIP_VALUE, and continue to next iteration.
        //
        stopFlag = 0;
        if ((*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip < DECREMENT_ROUNDTRIP_VALUE) {
          //
          // No need to test this channel anymore
          //
          chBitmask &= ~(1 << ch);
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "WARNING: Unable to find round trip latency!!!\n"));
          DisableChannel(host, socket, ch);
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_RD_RCVEN, socket, ch, dimm, rank, EwlSeverityWarning, NO_STROBE, NO_BIT, RecEnDelay, DdrLevel, 0xFF);
        } else {
          (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip -= DECREMENT_ROUNDTRIP_VALUE;
        }
        CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "Round trip latency: No zeros found\nRound trip latency = %d\n",
                       (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip));
      }
    } // ch loop
  } // while loop

  return;
} // FindRoundTripLatency

/**

  This function find the logic delay max and logic delay min

  @param host            - Pointer to sysHost
  @param socket            - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param rank            - Rank number
  @param MaxSdramInDimm  - Number of sdram in the current DIMM
  MaxLogicDelay   - Return logic delay max value
  MinLogicDelay   - Return logic delay min value

  @retval N/A

**/
void
FindMaxMinLogic (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT8     dimm,
                UINT8     rank,
                UINT8     MaxSdramInDimm,
                UINT8     *MaxLogicDelay,
                UINT8     *MinLogicDelay
                )
{
  UINT8   strobe;
  UINT16  tmpLogicDelay;

  *MaxLogicDelay  = 0;
  *MinLogicDelay  = 16;

  for (strobe = 0; strobe < MaxSdramInDimm; strobe++) {
    //
    // Get this logic delay
    //
    CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY, (INT16 *)&tmpLogicDelay));
    tmpLogicDelay = tmpLogicDelay / 64;

    //
    // Check if this delay is less than the minimum delay
    //
    if ((UINT8)tmpLogicDelay < *MinLogicDelay) {
      *MinLogicDelay = (UINT8)tmpLogicDelay;
    }

    //
    // Check if this delay is more than the maximum delay
    //
    if (*MaxLogicDelay < (UINT8)tmpLogicDelay) {
      *MaxLogicDelay = (UINT8)tmpLogicDelay;
    }
  } // strobe loop
} // FindMaxMinLogic

#ifdef SERIAL_DBG_MSG
/**

  This function print the sample array

  @param host      - Pointer to sysHost
  @param socket      - Socket number
  @param ch        - Channel number
  @param dimm      - DIMM number
  @param ReadMode  - 5 = LRDIMM Write MWD Delay
                     4 = LRDIMM Read DQ DQS
                     3 = LRDIMM Read Recieve Enable Phase
                     2 = LRDIMM Write DWL
                     1 = Read DqDqs
                     0 = Write DqDqs

  @retval N/A

**/
void
PrintSampleArray (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT8    dimm,
                 UINT8    ReadMode
                 )
{
  UINT8   strobe;
  UINT8   maxStrobe;
  UINT32  piSetting;
  struct TrainingResults (*trainRes)[MAX_CH][MAX_STROBE];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  trainRes    = &host->var.mem.trainRes;
#ifdef LRDIMM_SUPPORT
  if (ReadMode == PRINT_ARRAY_TYPE_LRDIMM_MDQ_WR_DELAY) {
    trainRes    = &host->var.mem.lrMwdTrainRes;
  }
  if (ReadMode == PRINT_ARRAY_TYPE_LRDIMM_MDQ_RD_DELAY) {
    trainRes    = &host->var.mem.lrMrdTrainRes;
  }
  if (ReadMode == PRINT_ARRAY_TYPE_LRDIMM_RCV_EN_PHASE) {
    trainRes    = &host->var.mem.lrTrainRes;
  }
  if (ReadMode == PRINT_ARRAY_TYPE_LRDIMM_WL_PHASE) {
    trainRes    = &host->var.mem.lrDwlTrainRes;
  }
#endif // LRDIMM_SUPPORT

  dimmNvList  = GetDimmNvList(host, socket, ch);

  if ((ReadMode == 0) && !(*dimmNvList)[dimm].x4Present) {
    PrintLine(host, 39, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
    if (host->nvram.mem.eccEn) {
      rcPrintf ((host, "       0   1   2   3   4   5   6   7   8\n"));
    } else {
      rcPrintf ((host, "       0   1   2   3   4   5   6   7\n"));
    }
    maxStrobe = MAX_STROBE / 2;
  } else {
    PrintLine(host, 85, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
    if (host->nvram.mem.eccEn) {
      rcPrintf ((host, "       0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17\n"));
    } else {
      rcPrintf ((host, "       0   1   2   3   4   5   6   7   9  10  11  12  13  14  15  16\n"));
    }
    maxStrobe = MAX_STROBE;
  }

  for (piSetting = 0; piSetting < (MAX_PHASE_IN_FINE_ADJUSTMENT * 2); piSetting++) {
    if ((ReadMode > 1) && (piSetting % 2)) continue;
    if (((ReadMode == PRINT_ARRAY_TYPE_LRDIMM_MDQ_RD_DELAY) || (ReadMode == PRINT_ARRAY_TYPE_LRDIMM_MDQ_WR_DELAY)) && (piSetting % 4)) continue;
    if ((ReadMode == PRINT_ARRAY_TYPE_LRDIMM_MDQ_RD_DELAY) || (ReadMode == PRINT_ARRAY_TYPE_LRDIMM_MDQ_WR_DELAY)) {
      rcPrintf ((host, "%d ", (((INT32)piSetting / 4) - 15)));

      if ((((INT32)piSetting/4 - 15) >= 0) && (((INT32)piSetting/4 -15) < 10)) {
        rcPrintf ((host, "  "));
      }

      if ((((INT32)piSetting/4 -15) < 100) && (((INT32)piSetting/4 -15) > 9)) {
        rcPrintf ((host, " "));
      }

      if ((((INT32)piSetting/4 -15) < 0) && (((INT32)piSetting/4 -15) > -10)) {
        rcPrintf ((host, " "));
      }

    } else {
      rcPrintf ((host, "%d ", piSetting));

      if (piSetting < 10) {
        rcPrintf ((host, "  "));
      }

      if ((piSetting < 100) && (piSetting > 9)) {
        rcPrintf ((host, " "));
      }
    }
    for (strobe = 0; strobe < maxStrobe; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      if (((1 << piSetting) & (*trainRes)[ch][strobe].results[piSetting / 32]) != 0) {
        rcPrintf ((host, "   1"));
      } else {
        rcPrintf ((host, "   0"));
      }
    } // strobe loop

    rcPrintf ((host, "\n"));
  } // piSetting loop
} // PrintSampleArray


/**

  Prints the edges found

  @param host      - Pointer to sysHost
  @param socket        - Current Socket
  @param ch        - Channel number
  @param pulseWidth    - Pointer to save the pulse width
  @param fallingEdge   - Pointer to save the falling edge
  @param risingEdge    - Pointer to save the rising edge
  @param centerPoint   - Pointer to save the center point
  @param maxStrobe     - Maximum number of strobes

  @retval N/A

**/
void
DisplayEdges (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT16    pulseWidth[MAX_STROBE],
             UINT16    fallingEdge[MAX_STROBE],
             UINT16    risingEdge[MAX_STROBE],
             UINT16    centerPoint[MAX_STROBE],
             UINT8     maxStrobe
             )
{
  UINT8 strobe;

  if (!(checkMsgLevel(host, SDBG_MAX))) return;

  getPrintFControl (host);

  PrintLine(host, 76, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

  // Print rising edges
  rcPrintf ((host, "RE: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
  if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    rcPrintf ((host, " %3d", risingEdge[strobe]));
  } // strobe loop

  // Print center points
  rcPrintf ((host, "\nCP: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
  if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    rcPrintf ((host, " %3d", centerPoint[strobe]));
  } // strobe loop

  // Print falling edges
  rcPrintf ((host, "\nFE: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
  if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    rcPrintf ((host, " %3d", fallingEdge[strobe]));
  } // strobe loop

  // Print pulse width
  rcPrintf ((host, "\nPW: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
  if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    rcPrintf ((host, " %3d", pulseWidth[strobe]));
  } // strobe loop
  rcPrintf ((host, "\n\n"));

  releasePrintFControl (host);
} // DisplayEdges


/**

  Prints the edges found for LRDIMMs

  @param host      - Pointer to sysHost
  @param socket        - Current Socket
  @param ch        - Channel number
  @param pulseWidth    - Pointer to save the pulse width
  @param fallingEdge   - Pointer to save the falling edge
  @param risingEdge    - Pointer to save the rising edge
  @param centerPoint   - Pointer to save the center point
  @param maxStrobe     - Maximum number of strobes

  @retval N/A

**/
void
DisplayLrdimmMrdEdges (
                      PSYSHOST  host,
                      UINT8     socket,
                      UINT8     ch,
                      UINT16    pulseWidth[MAX_STROBE],
                      UINT16    fallingEdge[MAX_STROBE],
                      UINT16    risingEdge[MAX_STROBE],
                      UINT16    centerPoint[MAX_STROBE],
                      UINT8     maxStrobe
                      )
{
  UINT8 strobe;

  if (!(checkMsgLevel(host, SDBG_MAX))) return;

  getPrintFControl (host);

  PrintLine(host, 76, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

  // Print rising edges
  rcPrintf ((host, "RE: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
    rcPrintf ((host, " %3d", ((INT32)risingEdge[strobe]/4 -15)));
  } // strobe loop

  // Print center points
  rcPrintf ((host, "\nCP: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
    rcPrintf ((host, " %3d", ((INT32)centerPoint[strobe]/4 -15)));
  } // strobe loop

  // Print falling edges
  rcPrintf ((host, "\nFE: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
    rcPrintf ((host, " %3d", ((INT32)fallingEdge[strobe]/4 -15)));
  } // strobe loop

  // Print pulse width
  rcPrintf ((host, "\nPW: "));
  for (strobe = 0; strobe < maxStrobe; strobe++) {
    rcPrintf ((host, " %3d", ((INT32)pulseWidth[strobe]/4)));
  } // strobe loop
  rcPrintf ((host, "\n\n"));

  releasePrintFControl (host);
} // DisplayLrdimmMrdEdges
#endif // SERIAL_DBG_MSG


/**

  Get's the resutls from the current test

  @param host      - Pointer to sysHost
  @param socket    - Current Socket
  @param dimm      - DIMM number
  @param rank      - rank number of the DIMM
  @param piDelay   - CLK adjustment setting

  @retval N/A

**/
void
GetResults (
           PSYSHOST  host,
           UINT8     socket,
           UINT8     dimm,
           UINT8     rank,
           UINT16    piDelay
           )
{
  UINT8               ch;
  UINT8               strobe;
  UINT32              csrReg;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    for (strobe = 0; strobe < RESULT_STROBE_INDEX; strobe++) {
      csrReg = CHIP_FUNC_CALL(host, ReadTrainFeedback (host, socket, ch, strobe));

      CHIP_FUNC_CALL(host, RecEnTrainFeedback (host, socket, ch, strobe, csrReg, piDelay));
    } // strobe loop
  } // ch loop
} // GetResults

/**

  Optimizes round trip latency

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
RoundTripOptimize (
                  PSYSHOST  host
                  )
{
  UINT8                             socket;
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             logRank;
  UINT8                             strobe;
  UINT16                            minDelay;
  UINT16                            maxDelay;
  UINT8                             clk;
  INT16                             piDelay[MAX_CH];
  INT16                             curDelay;
  UINT8                             ckEnabled[MAX_CLK];
  UINT8                             ctlIndex[MAX_CLK];
  UINT8                             worstMargin;
  UINT8                             eyeWidth[MAX_CH][MAX_RANK_CH][MAX_STROBE];
  UINT8                             goodRT[MAX_CH][MAX_RANK_CH];
  UINT8                             goodIO[MAX_CH][MAX_RANK_CH];
  UINT8                             orgRT[MAX_CH][MAX_RANK_CH];
  UINT8                             orgIO[MAX_CH][MAX_RANK_CH];
  UINT8                             rankDone[MAX_CH][MAX_RANK_CH];
  UINT8                             keepGoing;
  UINT8                             failed;
  UINT8                             minIoLatency;
  UINT8                             rtGuardBand;
  UINT8                             orgrunningRmt;
  UINT16                            minLimit;
  UINT16                            usDelay;
  UINT16                            maxCkDelay;
  UINT16                            maxCmdDelay;
  UINT16                            maxCtlDelay;
  UINT16                            maxRecEnDelay;
  UINT16                            maxTxDqDelay;
  UINT16                            maxTxDqsDelay;
  struct bitMask                    filter;
  struct strobeMargin               timeMargin;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct RankCh                     (*rankPerCh)[MAX_RANK_CH];

  if (~host->memFlows & MF_RT_OPT_EN)  return SUCCESS;

  if (~host->setup.mem.optionsExt & ROUND_TRIP_LATENCY_EN) return SUCCESS;

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) return SUCCESS;

  MemSetLocal ((UINT8 *)piDelay, 0, sizeof (piDelay));
  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // Get the min/max limits
  //
  CHIP_FUNC_CALL(host, GetCmdGroupLimits (host, socket, DdrLevel, CkAll, &minLimit, &maxCkDelay));
  CHIP_FUNC_CALL(host, GetCmdGroupLimits (host, socket, DdrLevel, CmdAll, &minLimit, &maxCmdDelay));
  CHIP_FUNC_CALL(host, GetCmdGroupLimits (host, socket, DdrLevel, CtlAll, &minLimit, &maxCtlDelay));
  CHIP_FUNC_CALL(host, GetDataGroupLimits (host, DdrLevel, RecEnDelay, &minLimit, &maxRecEnDelay, &usDelay));
  CHIP_FUNC_CALL(host, GetDataGroupLimits (host, DdrLevel, TxDqsDelay, &minLimit, &maxTxDqsDelay, &usDelay));
  CHIP_FUNC_CALL(host, GetDataGroupLimits (host, DdrLevel, TxDqDelay, &minLimit, &maxTxDqDelay, &usDelay));
  
  //
  // Save Orginal runningRmt setting before set to 1
  //
  orgrunningRmt = host->var.mem.runningRmt;
  
  //
  // Set runningRmt = 1 so that we margin all ranks for host RxDqsDelay. Previously, we only trained rank 0.
  //
  host->var.mem.runningRmt = 1;

  rtGuardBand = RT_GUARDBAND;
  minIoLatency = MEM_CHIP_POLICY_VALUE(host, minIoLatency);

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {

    // Go to next channel if this is disabled
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      // Check next DIMM if this one is not present or is mapped out
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      // Loop for each rank
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, ROUND_TRIP_OPTIMIZE, (UINT16)((socket << 8)|(dimm << 4)| rank)));

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n = 0;
          timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p = 0;
        } // strobe loop

        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);

        orgRT[ch][(*rankList)[rank].rankIndex] = CHIP_FUNC_CALL(host, GetRoundTrip (host, socket, ch, logRank));
        orgIO[ch][(*rankList)[rank].rankIndex] = CHIP_FUNC_CALL(host, GetIOLatency (host, socket, ch, logRank));
        goodRT[ch][(*rankList)[rank].rankIndex] = orgRT[ch][(*rankList)[rank].rankIndex];
        goodIO[ch][(*rankList)[rank].rankIndex] = orgIO[ch][(*rankList)[rank].rankIndex];
      } // rank loop
    } // dimm loop

    //
    // Delaying DDRIO settings by 20 pi
    //
    minDelay = MAX_CMD_MARGIN;
    maxDelay = 0;
    piDelay[ch] = 20;
    for (clk = 0; clk < MAX_CLK; clk++) {
      ckEnabled[clk] = 0;
      ctlIndex[clk] = 0;
    } // clk loop

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        clk = (*rankList)[rank].ckIndex;
        ctlIndex[clk] = (*rankList)[rank].ctlIndex;
        ckEnabled[clk] = 1;
      } // rank loop
    } // dimm loop


    //
    // Check whether piDelay can be accommodated by all signals
    // else, adjust piDelay so that none of the signals rail
    //
    for (clk = 0; clk < MAX_CLK; clk++) {
      if(ckEnabled[clk] == 0) continue;

      CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex[clk] + CtlGrp0, GSM_READ_ONLY, &curDelay, &minDelay, &maxDelay));
      if ((maxDelay + piDelay[ch]) > maxCtlDelay) {
        piDelay[ch] = maxCtlDelay - maxDelay;
      }
    } // clk loop

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_READ_ONLY, &curDelay, &minDelay, &maxDelay));
    if ((maxDelay + piDelay[ch]) > maxCmdDelay) {
      piDelay[ch] = maxCmdDelay - maxDelay;
    }

    // check RCVEN, TXDQ, TXDQS
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY, &curDelay));
          if ((curDelay + piDelay[ch]) > maxRecEnDelay) {
            piDelay[ch] = maxRecEnDelay - curDelay;
          }
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, &curDelay));
          if ((curDelay + piDelay[ch]) > maxTxDqsDelay) {
            piDelay[ch] = maxTxDqsDelay - curDelay;
          }
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_ONLY, &curDelay));
          if ((curDelay + piDelay[ch]) > maxTxDqDelay) {
            piDelay[ch] = maxTxDqDelay - curDelay;
          }
        } // strobe loop
      } // rank loop
    } // dimm loop

    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Number of offset PI ticks %2d\n",piDelay[ch]));

    // offset by 20 pi ticks or maximum value based on trained values
    for (clk = 0; clk < MAX_CLK; clk++) {
      if(ckEnabled[clk] == 0) continue;

      CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, clk, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay[ch]));

      CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex[clk] + CtlGrp0, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay[ch], &minDelay, &maxDelay));
    } // clk loop

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay[ch], &minDelay, &maxDelay));

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay[ch]));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay[ch]));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay[ch]));
        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  JedecInitSequence (host, socket, CH_BITMASK);

  //
  // Clear the filter bits to enable error checking on every bit
  //
  ClearFilter(host, socket, &filter);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Get initial eye widths\n"));

  //
  // Get initial Rx timing width
  //
  GetMargins (host, socket, DdrLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_STROBE, &filter, &timeMargin, TRAIN_RD_DQS_PL, 0, 0, 0,
              WDB_BURST_LENGTH);

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    rcPrintf ((host, "             RT  IOL    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17\n"));
    PrintLine(host, 93, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
  }
#endif // SERIAL_DBG_MSG
  worstMargin = 0xFF;

  //
  // Calculate eye widths
  //
  for (ch = 0; ch < MAX_CH; ch++) {

    // Go to next channel if this is disabled
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      // Check next DIMM if this one is not present or is mapped out
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      // Loop for each rank
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "%2d  %2d  :",orgRT[ch][(*rankList)[rank].rankIndex], orgIO[ch][(*rankList)[rank].rankIndex]));
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          eyeWidth[ch][(*rankList)[rank].rankIndex][strobe] = (UINT8)(timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p -
                                                                      timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n);
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "%3d ", eyeWidth[ch][(*rankList)[rank].rankIndex][strobe]));

          //
          // Track the worst margin
          //
          if (worstMargin > eyeWidth[ch][(*rankList)[rank].rankIndex][strobe]) {
            worstMargin = eyeWidth[ch][(*rankList)[rank].rankIndex][strobe];
          }
        } // strobe loop
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n"));

        //
        // Do not try to optimize if IO latency is below the minimum
        //
        if (orgIO[ch][(*rankList)[rank].rankIndex] <= minIoLatency) {
          rankDone[ch][(*rankList)[rank].rankIndex] = 1;
        } else {
          rankDone[ch][(*rankList)[rank].rankIndex] = 0;
        }
      } // rank loop
    } // dimm loop
  } // ch loop

  //
  // If the starting margins are too low skip this step
  //
  if (worstMargin <= rtGuardBand) {
    keepGoing = 0;
  } else {
    keepGoing = 1;
  }

  //
  // Reduce round trip on each rank until we find the best round trip setting
  //
  while (keepGoing) {

    keepGoing = 0;

    for (ch = 0; ch < MAX_CH; ch++) {

      // Go to next channel if this is disabled
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        // Check next DIMM if this one is not present or is mapped out
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);

        // Loop for each rank
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

          // Continue to the next rank if this one is disabled
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          //
          // Do not adjust this rank if it is done
          //
          if (rankDone[ch][(*rankList)[rank].rankIndex]) continue;
          //
          // Get the logical rank #
          //
          logRank = GetLogicalRank(host, socket, ch, dimm, rank);

          CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, logRank, goodRT[ch][(*rankList)[rank].rankIndex] - 2));
          CHIP_FUNC_CALL(host, SetIOLatency (host, socket, ch, logRank, goodIO[ch][(*rankList)[rank].rankIndex] - 2));
          //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
          //              "         %d : %d\n", goodRT[ch][(*rankList)[rank].rankIndex] - 2, goodIO[ch][(*rankList)[rank].rankIndex] - 2));

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n = 0;
            timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p = 0;
          } // strobe loop
        } // rank loop
      } // dimm loop
    } // ch loop

    //
    // Clear the filter bits to enable error checking on every bit
    //
    ClearFilter(host, socket, &filter);

    CHIP_FUNC_CALL(host, IO_Reset (host, socket));

    //
    // Get Rx timing width
    //
    GetMargins (host, socket, DdrLevel, RxDqsDelay, MODE_VIC_AGG, SCOPE_STROBE, &filter, &timeMargin, TRAIN_RD_DQS_PL, 0, 0, 0,
                WDB_BURST_LENGTH);

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\nCurrent Test Results:\n"));
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) {
      rcPrintf ((host, "             RT  IOL    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17\n"));
      PrintLine(host, 93, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
    }
#endif // SERIAL_DBG_MSG


    //
    // Calculate eye widths
    //
    for (ch = 0; ch < MAX_CH; ch++) {

      // Go to next channel if this is disabled
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        // Check next DIMM if this one is not present or is mapped out
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);

        // Loop for each rank
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

          // Continue to the next rank if this one is disabled
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          if (rankDone[ch][(*rankList)[rank].rankIndex]) continue;

         MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            "%2d  %2d  :",goodRT[ch][(*rankList)[rank].rankIndex] - 2, goodIO[ch][(*rankList)[rank].rankIndex] - 2));
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
            if (timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n > timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p) {
              timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n = 0;
              timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p = 0;
            }
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "%3d ", timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p -
                           timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n));

          } // strobe loop
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\n"));

        } // rank loop
      } // dimm loop
    } // ch loop

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n"));

    //
    // Check eye widths
    //
    for (ch = 0; ch < MAX_CH; ch++) {

      // Go to next channel if this is disabled
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      for (dimm = 0; dimm < MAX_DIMM; dimm++) {

        // Check next DIMM if this one is not present or is mapped out
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);

        // Loop for each rank
        for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

          // Continue to the next rank if this one is disabled
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          if (rankDone[ch][(*rankList)[rank].rankIndex]) continue;

          failed = 0;

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
            if ((eyeWidth[ch][(*rankList)[rank].rankIndex][strobe] - rtGuardBand) >
            (timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p -
                timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n)) {

              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                             "Fail: eyeWidth = %d, current width = %d\n", eyeWidth[ch][(*rankList)[rank].rankIndex][strobe],
                             timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].p -
                             timeMargin.strobe[ch][(*rankList)[rank].rankIndex][strobe].n));
              failed = 1;
              break;
            }
          } // strobe loop

          if (failed) {
            rankDone[ch][(*rankList)[rank].rankIndex] = 1;
          } else {
            //
            // These values passed so save them
            //
            goodRT[ch][(*rankList)[rank].rankIndex] -= 2;
            goodIO[ch][(*rankList)[rank].rankIndex] -= 2;

            //
            // Do not try to optimize if IO latency is 0 or less
            //
            if (goodIO[ch][(*rankList)[rank].rankIndex] <= minIoLatency) {
              rankDone[ch][(*rankList)[rank].rankIndex] = 1;
            } else {
              keepGoing = 1;
            }
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                           "New RT value passed saving. RT = %d, IO = %d, keepGoing = %d\n", goodRT[ch][(*rankList)[rank].rankIndex],
                           goodIO[ch][(*rankList)[rank].rankIndex], keepGoing));
          }
        } // rank loop
      } // dimm loop
    } // ch loop
  } // while loop

  MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n              org RT : new RT  org IOL : new IOL\n"));
  for (ch = 0; ch < MAX_CH; ch++) {

    // Go to next channel if this is disabled
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    rankPerCh = &(*channelNvList)[ch].rankPerCh;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      // Check next DIMM if this one is not present or is mapped out
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      // Loop for each rank
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);

        (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip = goodRT[ch][(*rankList)[rank].rankIndex];
        CHIP_FUNC_CALL(host, SetRoundTrip (host, socket, ch, logRank, goodRT[ch][(*rankList)[rank].rankIndex]));
        (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency  = goodIO[ch][(*rankList)[rank].rankIndex];
        CHIP_FUNC_CALL(host, SetIOLatency (host, socket, ch, logRank, goodIO[ch][(*rankList)[rank].rankIndex]));
        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
            "   %2d   :   %2d       %2d   :   %2d\n", orgRT[ch][(*rankList)[rank].rankIndex], goodRT[ch][(*rankList)[rank].rankIndex], orgIO[ch][(*rankList)[rank].rankIndex], goodIO[ch][(*rankList)[rank].rankIndex]));

      } // rank loop
    } // dimm loop

    for (clk = 0; clk < MAX_CLK; clk++) {
      ckEnabled[clk] = 0;
      ctlIndex[clk] = 0;
    } // clk loop

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        clk = (*rankList)[rank].ckIndex;
        ctlIndex[clk] = (*rankList)[rank].ctlIndex;
        ckEnabled[clk] = 1;
      } // rank loop
    } // dimm loop

    //
    //Reverting DDRIO Pi Delay offset
    //
    piDelay[ch] = -piDelay[ch];
    for (clk = 0; clk < MAX_CLK; clk++) {
      if(ckEnabled[clk] == 0) continue;

      CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, clk, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay[ch]));

      CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex[clk] + CtlGrp0, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay[ch], &minDelay, &maxDelay));
    } // clk loop

    CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelay[ch], &minDelay, &maxDelay));

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay[ch]));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay[ch]));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay[ch]));
        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

  CHIP_FUNC_CALL(host, IO_Reset (host, socket));
  JedecInitSequence (host, socket, CH_BITMASK);
  
  //
  // Restore runningRmt value
  //
  host->var.mem.runningRmt = orgrunningRmt;
  return SUCCESS;
} // RoundTripOptimize
