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

#define   RD_DQS_STEP_SIZE  1


//
// Local function prototypes
//
static
UINT32
FindPerBitResults (
  PSYSHOST     host,
  UINT8        socket,
  UINT8        dimm,
  UINT8        rank,
  BErrorResult *ErrorResult
  );

void
RdDqDqsInit(
  PSYSHOST host,
  UINT8 socket
  );


/**

  @param host              - Pointer to sysHost
  @param socket            - Socket number
  @param dimm              - DIMM number
  @param rank              - Rank number
  @param ErrorResult       - include the error rwsult for this channel

  @retval Status

**/
static
UINT32
RunMprTest (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     dimm,
  UINT8     rank,
  UINT32    chBitmask,
  UINT32    bwSerr[MAX_CH][3]
  )
{
  UINT8   ch;
  UINT32  status;
  UINT8   DumArr[7] = {1,1,1,1,1,1,1};

  status = SUCCESS;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitmask & (1 << ch)) == 0) continue;

    CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << GetLogicalRank(host, socket, ch, dimm, rank), 0, 0));
  } // ch loop

  CHIP_FUNC_CALL(host, SetupIOTestMPR(host, socket, chBitmask, 1, 0, 0, 0));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitmask & (1 << ch)) == 0) continue;
    CHIP_FUNC_CALL(host, SetCpgcPatControl (host, socket, ch, BTBUFFER, BTBUFFER, BTBUFFER, 0, 0, 0));
  } // ch loop

  CHIP_FUNC_CALL(host, IO_Reset(host, socket));

  //
  // Run the test
  //
  CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

  // Collect test results
  status = CollectTestResults (host, socket, chBitmask, bwSerr);

  CHIP_FUNC_CALL(host, IO_Reset(host, socket));

  return status;
}

/**

  Detects DQ swizzling on board routing for NVMDIMM DIMMs

  @param host  - Pointer to sysHost

  @retval Status

**/
UINT32
DqSwizzleDiscovery (
  PSYSHOST host
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT32              chBitmask;
  UINT32              bwSerr[MAX_CH][3];
  UINT8               lane;
  UINT8               strobe;
  UINT8               bit;
  UINT8               b;
  UINT8               mask;
  UINT8               maxStrobe = 9;
  UINT8               result;
  UINT8               i;
  UINT8               foundSel;
  UINT8               swzSel = 0;
  UINT8               maxLanes = 8;
  UINT8               lanesPerBwSerr = 4;
  UINT16              mprPattern;
  UINT8               simFeedback = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  //
  // Return if this socket is disabled
  //
  socket = host->var.mem.currentSocket;

  if(CHIP_FUNC_CALL(host, CheckDqSwizzleSupport(host, socket)) == 0) return SUCCESS;

  OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, DQ_SWIZZLE_DISCOVERY, (UINT16)((socket << 8))));

  channelNvList = GetChannelNvList(host, socket);

  //Clear WDB
  SetPatternBuffers (host, socket, MEMINIT, 0);

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < 1; rank++) {

      //
      // Initialize to no ranks present
      //
      chBitmask = 0;
      for (ch = 0; ch < MAX_CH; ch++) {

        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        // Inidicate this rank is present on at least one channel
        chBitmask |= (1 << ch);

        // Initialize data structure
        for (i = 0; i < 36; i++) {
          (*dimmNvList)[dimm].dqSwz[i] = 0;
        }

        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, !(*dimmNvList)[dimm].aepDimmPresent));
#ifdef LRDIMM_SUPPORT
        host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentDramMode = MR3_MPR;
#endif
      }  // ch loop

      // Go to the next rank if this rank is not present on any channels
      if (!chBitmask) continue;

      //
      // Loop through bits 0-7 selecting one DQ to drive high
      //
      for (lane = 0; lane < 8; lane++) {

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((chBitmask & (1 << ch)) == 0) continue;

          // for x8 device bits[7:0] map to DQ[0:7]
          // for x4 device bits[7:4] map to DQ[0:3]
          mprPattern = BIT7 >> lane;

          // Use parallel MPR patterns for swizzle detection
          SetRankMPRPattern (host, socket, ch, dimm, rank, BIT11 | MR3_MPR, mprPattern);
        } // ch loop

        //
        // Run test
        //
        RunMprTest (host, socket, dimm, rank, chBitmask, bwSerr);

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((chBitmask & (1 << ch)) == 0) continue;
          rankList = GetRankNvList(host, socket, ch, dimm);
          dimmNvList = GetDimmNvList(host, socket, ch);

          if ((*dimmNvList)[dimm].x4Present && ((*dimmNvList)[dimm].aepDimmPresent == 0)) {
            if (lane >= 4) {
              continue;
            } else{
              maxLanes = 4;
              maxStrobe = MAX_STROBE;
              lanesPerBwSerr = 8;
              mask = 0xF;
            }
          } else {
            maxLanes = 8;
            maxStrobe = MAX_STROBE/2;
            lanesPerBwSerr = 4;
            mask = 0xFF;
          }

          // Simics workaround starts
          if (host->var.common.emulation & SIMICS_FLAG) {
            simFeedback = 1;
          }
          if (simFeedback) {

            /*
            // HSX test results with SRx4
            Dq Swizzle Discovery -- Started
            Checkpoint: Socket 0, 0xB7, 0x1E, 0x0000
            N0.C0.R0:  0 ...##... ...##... ...##... ...##... ...##... ...##... ...##... ...##... ...##...
            N0.C0.R0:  1 #......# #......# #......# #......# #......# #......# #......# #......# #......#
            N0.C0.R0:  2 ..#..#.. ..#..#.. ..#..#.. ..#..#.. ..#..#.. ..#..#.. ..#..#.. ..#..#.. ..#..#..
            N0.C0.R0:  3 .#....#. .#....#. .#....#. .#....#. .#....#. .#....#. .#....#. .#....#. .#....#.
            DQ lanes:     000000000011111111112222222222333333333344444444445555555555666666666677
                          012345678901234567890123456789012345678901234567890123456789012345678901
            N0.C0.D0.R0:  302103123021031230210312302103123021031230210312302103123021031230210312
            Dq Swizzle Discovery - 69ms
            */
            if (maxLanes <= 4) {
              UINT32 pattern = 0;
              switch(lane) {
                case 0:
                  pattern = 0x18181818;
                  break;
                case 1:
                  pattern = 0x81818181;
                  break;
                case 2:
                  pattern = 0x24242424;
                  break;
                case 3:
                  pattern = 0x42424242;
                  break;
              }
              bwSerr[ch][0] = pattern;
              bwSerr[ch][1] = pattern;
              bwSerr[ch][2] = (UINT8)pattern;
            } else {
              bwSerr[ch][0] = 0x01010101 << lane;
              bwSerr[ch][1] = 0x01010101 << lane;
              bwSerr[ch][2] = 0x01 << lane;
            }

//Simics workaround ends
          }
#ifdef SERIAL_DBG_MSG
          if (checkMsgLevel(host, SDBG_MAX)) {
            DisplayBwSerr(host, socket, ch, dimm, (*rankList)[rank].rankIndex, bwSerr[ch], lane);
          }
#endif
          // record passing lane - 3 bits per target lane
          // bits[1:0] are DQ swizzle within the nibble
          // bit[2] is nibble swap, if applicable
          for(strobe = 0; strobe < maxStrobe; strobe++) {


            result = (bwSerr[ch][strobe / lanesPerBwSerr] >> ((strobe % lanesPerBwSerr) * maxLanes)) & mask;
            foundSel = 0;
            for(i = 0; i < maxLanes; i++) {
              if (result & (1 << i)) {
                if (!foundSel) {
                  swzSel = i;
                  foundSel = 1;
                } else {
                  // Flag error if more than one DQ lane = 1 per strobe
                  host->var.mem.faultyPartsFlag[ch] |= ~0;
                  rankList = GetRankNvList(host, socket, ch, dimm);
                  (*rankList)[rank].faultyParts[0] |= FPT_TRAINING_FAILED;
                  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "More than one DQ lane = 1: 0x%x!\n", result));
                  EwlOutputType10(host, WARN_DQ_SWIZZLE_DISC, WARN_DQ_SWIZZLE_DISC_UNCORR, socket, ch, NO_DIMM, NO_RANK, EwlSeverityWarning, (UINT8)bwSerr[ch][strobe / lanesPerBwSerr], foundSel, maxLanes, strobe);
                  DisableChannel(host, socket, ch);
                }
              }
            } // i loop

            if (!foundSel) {
              // Flag error if no DQ lane = 1 per byte
              host->var.mem.faultyPartsFlag[ch] |= ~0;
              rankList = GetRankNvList(host, socket, ch, dimm);
              (*rankList)[rank].faultyParts[0] |= FPT_TRAINING_FAILED;
              MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "No DQ lanes = 1!\n"));
              EwlOutputType10(host, WARN_DQ_SWIZZLE_DISC, WARN_DQ_SWIZZLE_DISC_UNCORR, socket, ch, NO_DIMM, NO_RANK, EwlSeverityWarning, (UINT8)bwSerr[ch][strobe / lanesPerBwSerr], foundSel, maxLanes, strobe);
              DisableChannel(host, socket, ch);
            }

            // Save result for later use
            (*dimmNvList)[dimm].dqSwz[(strobe * maxLanes + lane) / 2] |= (swzSel << ((lane % 2) * 4));

          } // strobe loop
        } // ch loop
      } // lane loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((chBitmask & (1 << ch)) == 0) continue;
        // Disable MPR mode
        dimmNvList = GetDimmNvList(host, socket, ch);
        SetRankMPR (host, socket, ch, dimm, rank, 0);

        if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
          CHIP_FUNC_CALL(host, SetRankDAMprFnv(host, socket, ch, dimm, 0));
          CHIP_FUNC_CALL(host, SetRankDMprFnv(host, socket, ch, dimm, 0));
        }

        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, 0));
#ifdef LRDIMM_SUPPORT
        host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentDramMode = 0;
#endif

      } // ch loop
    } // rank loop
  } // dimm loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\n\n"));
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "START_DQ_SWIZZLE \n"));


  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       ""));
        for (bit = 0; bit < MAX_BITS; bit++) {
          if ((bit == 0) || (bit == (MAX_BITS / 2))) {
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "\n"));
            if (bit == (MAX_BITS / 2)){
              MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "\n"));
            }
            for(b = bit; b < (bit + (MAX_BITS / 2)); b++) {
              MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "  %2d", b));
            }
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "\n"));
          }
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         " %3d", ((*dimmNvList)[dimm].dqSwz[bit / 2] >> ((bit % 2) * 4)) & 7));

        } // bit loop

        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n"));
      } // rank loop
    } // dimm loop
  } // ch loop


  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n"));

  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));

  return SUCCESS;
} // DqSwizzleDiscovery

/**

  Perform main loop of READ DqDqs algorithm.

  @param host  - Pointer to sysHost

  @retval Status

**/
UINT32
ReadDqDqs (
  PSYSHOST  host
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               rankPresent;
  UINT16              errorStart[MAX_CH][MAX_STROBE];
  UINT16              errorEnd[MAX_CH][MAX_STROBE];
  UINT16              piPosition[MAX_CH][MAX_STROBE];
  UINT16              worstLeft;
  UINT16              worstRight;
  UINT32              status;
  UINT8               faultyPartsStatus;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  TErrorResult        ErrorResult[MAX_CH];

  if (~host->memFlows & MF_RD_DQS_EN) return SUCCESS;

  faultyPartsStatus = FPT_NO_ERROR;
  status = SUCCESS;

  //
  // Return if this socket is disabled
  //
  socket = host->var.mem.currentSocket;
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  RdDqDqsInit (host, socket);

  channelNvList = GetChannelNvList(host, socket);

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, RX_DQ_DQS_BASIC, (UINT16)((socket << 8)|(dimm << 4)|rank)));
      //
      // Initialize to no ranks present
      //
      rankPresent = 0;

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        //
        // Inidicate this rank is present on at least one channel
        //
        rankPresent |= (*rankList)[rank].enabled;

        SetRankMPR (host, socket, ch, dimm, rank, MR3_MPR);
#ifdef LRDIMM_SUPPORT
        host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentDramMode = MR3_MPR;
#endif // LRDIMM_SUPPORT

        //
        // Tell the iMC not to issue activates and precharges
        //
        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, !(*dimmNvList)[dimm].aepDimmPresent));
      } // ch loop

      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (!rankPresent) continue;

      status = FindPiCompareResult (host, socket, dimm, rank, ErrorResult);

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, 0));

        SetRankMPR (host, socket, ch, dimm, rank, 0);
#ifdef LRDIMM_SUPPORT
        host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentDramMode = 0;
#endif // LRDIMM_SUPPORT

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          DqDqsDebugDataSummarization (host, socket, ch, dimm, rank, &ErrorResult[ch], MAX_PHASE_IN_READ_ADJUSTMENT, 0, 0);
        }
#endif // SERIAL_DBG_MSG

        worstLeft = 0xFF;
        worstRight = 0xFF;

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
                           "Skipped Read DqDqs FindErrorGraphMin\n"));
            continue;
          }
          status = FindErrorGraphMin (host, socket, ch, dimm, rank, strobe, &ErrorResult[ch].DimmErrorResult[strobe][0],
                                      MAX_PHASE_IN_READ_ADJUSTMENT, &errorStart[ch][strobe],
                                      &errorEnd[ch][strobe], &piPosition[ch][strobe]);
          if (status == FAILURE) {
            //
            // Mark this part as faulty
            //
            host->var.mem.faultyPartsFlag[ch] |= (1 << strobe);

            (*rankList)[rank].faultyParts[strobe] |= FPT_RD_DQ_DQS_FAILED;

            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "Failed RdDqDqs\n"));

            faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);

            if (faultyPartsStatus == FPT_NO_ERROR) {
              // do we need to do anything here ?!?
            } else if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
              // if correctable  log the warning -> OutputWarning
              // careful for cases when warning gets promoted to error !!!  - log the strobe
              MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, strobe, NO_BIT,
                             "Failed RdDqDqs\n"));
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_RD_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, RxDqDqsDelay, DdrLevel, (UINT8)(errorStart[ch][strobe] - errorEnd[ch][strobe]));
            } else {
              // does anything need to be done here?
              MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "RdDqDqs training failure!!!\n"));
              //
              // Add this error to the warning log for both correctable and uncorrectable errors.
              //
              DisableChannel(host, socket, ch);
              EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_RD_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, RxDqDqsDelay, DdrLevel, (UINT8)(errorStart[ch][strobe] - errorEnd[ch][strobe]));
              status = FAILURE;
            }
          }

          if ((piPosition[ch][strobe] - errorEnd[ch][strobe]) < worstLeft) worstLeft = piPosition[ch][strobe] - errorEnd[ch][strobe];
          if ((errorStart[ch][strobe] - piPosition[ch][strobe]) < worstRight) worstRight = errorStart[ch][strobe] - piPosition[ch][strobe];

          //
          // BIOS writes the calculated values of PI setting
          //
          CHIP_FUNC_CALL(host, AddChipRxDqsOffset(host, socket, ch, dimm, rank, strobe, &piPosition[ch][strobe]));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqsDelay, GSM_UPDATE_CACHE, (INT16 *)&piPosition[ch][strobe]));
        } // strobe loop

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
        }
        // Display results
        DisplayErrorStartEnd(host, socket, ch, piPosition[ch], errorStart[ch], errorEnd[ch]);

        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "START_DATA_RX_DQS_BASIC\n"));
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                        "Pi = %d\n", piPosition[ch][strobe]));

        } // strobe loop

        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "STOP_DATA_RX_DQS_BASIC\n"));

        if (checkMsgLevel(host, SDBG_MAX)) {
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
        if (worstLeft > 4) {
          (*rankList)[rank].rxDQLeftSt = (UINT8)(worstLeft - 3);
        } else {
          (*rankList)[rank].rxDQLeftSt = (UINT8)worstLeft;
        }
        if (worstRight > 4) {
          (*rankList)[rank].rxDQRightSt = (UINT8)(worstRight - 3);
        } else {
          (*rankList)[rank].rxDQRightSt = (UINT8)worstRight;
        }
      } // ch loop
    } // rank loop
  } // dimm loop

  return status;
}

UINT32
ReadDqDqsPerBit (
  PSYSHOST host
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               nibble;
  UINT8               encodedNibble;
  UINT8               bit;
  UINT8               bitIdx;
  UINT8               msl;
  INT16               bitSkew;
  INT16               bitNibbleMin;
  INT16               bitSkewMax;
  UINT16              bitCenter[8];
  UINT8               rankPresent;
  PerBitPi            errorStart[MAX_CH];
  PerBitPi            errorEnd[MAX_CH];
  PerBitPi            piPosition[MAX_CH];
  UINT16              worstLeft;
  UINT16              worstRight;
  UINT32              status;
  UINT8               faultyPartsStatus;
  struct bitMarginCh  resultsBit;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  BErrorResult        ErrorResult[MAX_CH];

  if (~host->memFlows & MF_RD_DQS_EN) return SUCCESS;

  status = SUCCESS;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  RdDqDqsInit (host, socket);

  channelNvList = GetChannelNvList(host, socket);

  // Initialize LE and RE Margin results structure
  for (ch = 0; ch < MAX_CH; ch++) {
    for (bit = 0; bit < MAX_BITS; bit++) {
      resultsBit.bits[ch][bit].n = 0;
      resultsBit.bits[ch][bit].p = 0;
    }
  } // ch loop

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      //
      // Initialize to no ranks present
      //
      rankPresent = 0;

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        //
        // Inidicate this rank is present on at least one channel
        //
        rankPresent |= (*rankList)[rank].enabled;

        SetRankMPR (host, socket, ch, dimm, rank, MR3_MPR);
#ifdef LRDIMM_SUPPORT
        host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentDramMode = MR3_MPR;
#endif

        //
        // Tell the iMC not to issue activates and precharges
        //
        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, !(*dimmNvList)[dimm].aepDimmPresent));
      } // ch loop

      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (!rankPresent) continue;

      //
      // Data collecting
      //
      status = FindPerBitResults(host, socket, dimm, rank, ErrorResult);

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

        CHIP_FUNC_CALL(host, SetMprTrainMode (host, socket, ch, 0));
        SetRankMPR (host, socket, ch, dimm, rank, 0);
#ifdef LRDIMM_SUPPORT
        host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentDramMode = 0;
#endif

#ifdef SERIAL_DBG_MSG
        //Print Margins Here
#endif // SERIAL_DBG_MSG

        worstLeft = 0xFF;
        worstRight = 0xFF;

        for (bit = 0; bit < MAX_BITS; bit++) {

          nibble = bit / 4;
          if (nibble % 2){
            encodedNibble = (nibble + 17) / 2;
          } else {
            encodedNibble = nibble / 2;
          }

          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && (bit > 63)) continue;

          status = FindErrorGraphMin (host, socket, ch, dimm, rank, 0, &ErrorResult[ch].BitErrorResult[bit][0],
                                      MAX_PHASE_IN_READ_ADJUSTMENT_DQ, &errorStart[ch].PiPosition[bit],
                                      &errorEnd[ch].PiPosition[bit], &piPosition[ch].PiPosition[bit]);

          //LE and RE Margins
          resultsBit.bits[ch][bit].n = errorEnd[ch].PiPosition[bit];
          resultsBit.bits[ch][bit].p = errorStart[ch].PiPosition[bit];

          if (status == FAILURE) {

            //
            // Mark this part as faulty
            //
            host->var.mem.faultyPartsFlag[ch] |= (1 << encodedNibble);

            (*rankList)[rank].faultyParts[encodedNibble] |= FPT_RD_DQ_DQS_FAILED;

            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, encodedNibble, bit,
                           "Failed Read DqDqsPerBit\n"));

            faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);

            if (faultyPartsStatus == FPT_NO_ERROR) {
              // do we need to do anything here ?!?
            } else if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
              // if correctable  log the warning -> OutputWarning
              // careful for cases when warning gets promoted to error !!!  - log the strobe
              MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, encodedNibble, NO_BIT,
                             "RdDqDqs Correctable\n"));
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_RD_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, 0xFF, bit, RxDqsBitDelay, DdrLevel, (UINT8)(resultsBit.bits[ch][bit].p - resultsBit.bits[ch][bit].n));
              status = SUCCESS;
            } else {
              // does anything need to be done here?
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "RdDqDqs training failure!!!\n"));
              //
              // Add this error to the warning log for both correctable and uncorrectable errors.
              //
              DisableChannel(host, socket, ch);
              EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_RD_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, 0xFF, bit, RxDqsBitDelay, DdrLevel, (UINT8)(resultsBit.bits[ch][bit].p - resultsBit.bits[ch][bit].n));
              status = FAILURE;
            }
          } // if FAILURE


          if ((piPosition[ch].PiPosition[bit] - errorEnd[ch].PiPosition[bit]) < worstLeft) worstLeft = piPosition[ch].PiPosition[bit] - errorEnd[ch].PiPosition[bit];
          if ((errorStart[ch].PiPosition[bit] - piPosition[ch].PiPosition[bit]) < worstRight) worstRight = errorStart[ch].PiPosition[bit] - piPosition[ch].PiPosition[bit];

          // //
          // // BIOS writes the calculated values of PI setting
          // //
          // CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqsDelay, GSM_UPDATE_CACHE, (INT16 *)&piPosition[ch][strobe]));
        } // bit loop

#ifdef SERIAL_DBG_MSG
        //Print LE and RE Per Bit Deskew Margins for current rank
        if (checkMsgLevel(host, SDBG_MAX)) {
          DisplayREandLEMargins (host, socket, ch, dimm, rank, &resultsBit, RxDqsDelay);
        }
#endif // SERIAL_DBG_MSG

        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "dimm = %d, rank = %d\n", dimm, rank));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "         Per Bit Margin Center       Per Bit Skew\n"));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "N# = Nibble, BCx = Bit Center, BSx = Bit Skew , MSL = Most Skewed Lane\n\n"));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "N# BC0 BC1 BC2 BC3   NPi   BS0 BS1 BS2 BS3  MSL\n"));

        // Loop for each nibble/strobe.
        for (nibble = 0; nibble < MAX_STROBE; nibble++) {
          //
          // Skip if this is an ECC nibble when ECC is disabled
          // Note: unlike usual in training code, per-bit skew register are mapped linerly, so ECC bits are on the end
          //
          if ((!host->nvram.mem.eccEn) && (nibble > 15)) continue;
          if (nibble % 2){
            encodedNibble = (nibble + 17) / 2;
          } else {
            encodedNibble = nibble / 2;
          }
          bitNibbleMin = 127;   // Set to largest possible
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "%2d ", nibble));

          // Loop for all the bits in this nibble
          for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
            // Get the absolute bit index (0-72)
            bitIdx = (nibble * 4) + bit;

            // Find the center for this bit
            bitCenter[bit] = piPosition[ch].PiPosition[bitIdx];

            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "%3d ", bitCenter[bit]));

            // Check if the center of this bit is smaller than the smallest found so far
            if (bitCenter[bit] < bitNibbleMin) {
              bitNibbleMin = bitCenter[bit];
            }
          } // for (bit)

          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "  %3d   ", bitNibbleMin));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, encodedNibble, 0, DdrLevel, RxDqDqsDelay, GSM_UPDATE_CACHE, &bitNibbleMin));
          // Loop for all the bits in this nibble
          bitSkewMax = 0;
          msl = 0;
          for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
            bitSkew = bitCenter[bit] - bitNibbleMin;
            if (bitSkew > bitSkewMax) {
              bitSkewMax = bitSkew;
              msl = bit;
            }
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "%3d ", bitSkew));
            // call SetGDCR to update the appropriate register field
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, encodedNibble, bit, DdrLevel, RxDqsBitDelay, GSM_UPDATE_CACHE | GSM_WRITE_OFFSET,
                             &bitSkew));

          } // for (bit)

          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " %3d\n", msl));
        } // for nibble

      } // ch loop
    } // rank loop
  } // dimm loop

  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    DisplayResults(host, socket, RxDqDqsDelay);
    DisplayResults(host, socket, RxDqDelay);
    DisplayResults(host, socket, RxDqsPDelay);
    DisplayResults(host, socket, RxDqsNDelay);
    DisplayResults(host, socket, RxDqsBitDelay);
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  }
#endif // SERIAL_DBG_MSG
  CHIP_FUNC_CALL(host, ReadDqDqsCleanup(host, socket));

  return status;

} // ReadDqDqsPerBit


/**

  For each setting of RX DQS PI setting (64 options)
    Multicast the RX DQS PI setting to all CSRs
    BIOS clears errors
    BIOS waits until all reads have finished
    BIOS reads the results from BW_SERR CSRs

  @param host              - Pointer to sysHost
  @param socket              - Socket number
  @param dimm              - DIMM number
  @param rank              - Rank number
  @param ErrorResult       - include the error rwsult for this channel

  @retval Status

**/
UINT32
FindPiCompareResult (
  PSYSHOST     host,
  UINT8        socket,
  UINT8        dimm,
  UINT8        rank,
  TErrorResult *ErrorResult
  )
{
  UINT8               ch;
  UINT32              chBitmask;
  UINT8               strobe;
  INT16               piDelay;
  UINT8               piIndex;
  UINT32              status;
  UINT32              errResult;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  UINT8               DumArr[7] = {1,1,1,1,1,1,1};

  status    = SUCCESS;

  channelNvList = GetChannelNvList(host, socket);

  chBitmask = GetChBitmask (host, socket, dimm, rank);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << GetLogicalRank(host, socket, ch, dimm, rank), 0, 0));

    //
    // Clear dimm error results
    //
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      for (piDelay = 0; piDelay < MAX_PHASE_IN_READ_ADJUSTMENT; piDelay++) {
        ErrorResult[ch].DimmErrorResult[strobe][piDelay] = 0;
      } //piDelay loop
    } // strobe loop
  } // ch loop

  CHIP_FUNC_CALL(host, SetupIOTestMPR(host, socket, chBitmask, 1, 0, 0, 0));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    CHIP_FUNC_CALL(host, SetCpgcPatControl (host, socket, ch, BTBUFFER, BTBUFFER, BTBUFFER, DDR_CPGC_PATBUF_MUX0, DDR_CPGC_PATBUF_MUX1, DDR_CPGC_PATBUF_MUX2));
  } // ch loop

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                "RxDqDqs Pi Scanning...\n"));

  for (piDelay = 0; piDelay < MAX_PHASE_IN_READ_ADJUSTMENT; piDelay += RD_DQS_STEP_SIZE) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      //
      // Change the RX Dq Dqs PI Setting
      //
      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, RxDqsDelay, GSM_UPDATE_CACHE, &piDelay));

    } // ch loop

    //
    // Run the test
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

    //
    // Check the results
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList  = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      errResult = CHIP_FUNC_CALL(host, GetErrorResults (host, socket, ch));
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        for (piIndex = 0; piIndex < RD_DQS_STEP_SIZE; piIndex++) {
          // Any failure is a failure for the strobe
          if (errResult & (1 << (strobe * 2 - (strobe >= 9) * 17))) {
            ErrorResult[ch].DimmErrorResult[strobe][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 1;
          } else {
            ErrorResult[ch].DimmErrorResult[strobe][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 0;
          }
        } // piIndex loop
      } // strobe loop
    } // ch loop

    CHIP_FUNC_CALL(host, IO_Reset(host, socket));
  } // piDelay loop

  return status;
}


static
UINT32
FindPerBitResults (
  PSYSHOST     host,
  UINT8        socket,
  UINT8        dimm,
  UINT8        rank,
  BErrorResult *ErrorResult
  )

{
  UINT8               ch;
  UINT32              chBitmask;
  UINT8               bit;
  INT16               piDelay;
  UINT8               piIndex;
  UINT32              errResult;
  UINT8               chStatus;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  UINT8               DumArr[7] = {1,1,1,1,1,1,1};
  UINT32              bwSerr[MAX_CH][3];

  channelNvList = GetChannelNvList(host, socket);

  chBitmask = GetChBitmask (host, socket, dimm, rank);
  errResult = 0;
  chStatus = 0;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << GetLogicalRank(host, socket, ch, dimm, rank), 0, 0));

    //
    // Clear dimm error results
    //
    for (bit = 0; bit < MAX_BITS; bit++) {
      for (piDelay = 0; piDelay < MAX_PHASE_IN_READ_ADJUSTMENT_DQ; piDelay++) {
        ErrorResult[ch].BitErrorResult[bit][piDelay] = 0;
      } //piDelay loop
    } // bit loop
  } // ch loop

  CHIP_FUNC_CALL(host, SetupIOTestMPR(host, socket, chBitmask, 1, 0, 0, 0));

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    CHIP_FUNC_CALL(host, SetCpgcPatControl (host, socket, ch, BTBUFFER, BTBUFFER, BTBUFFER, 0xAAAAAA, 0xCCCCCC, 0xF0F0F0));
  } // ch loop

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                "RxDqDqs Pi Scanning...\n"));
  for (piDelay = 0; piDelay < MAX_PHASE_IN_READ_ADJUSTMENT_DQ; piDelay += RD_DQS_STEP_SIZE) {

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      //
      // Change the RX Dq Dqs PI Setting
      //
      CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, RxDqDqsDelay, GSM_UPDATE_CACHE, &piDelay));

    } // ch loop

    CHIP_FUNC_CALL(host, IO_Reset(host, socket));

    //
    // Run the test
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

    //
    // Check the results
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      chStatus = 1 << ch;
      bwSerr[ch][0] = 0;
      bwSerr[ch][1] = 0;
      bwSerr[ch][2] = 0;
      // Collect test results
      CollectTestResults (host, socket, chStatus, bwSerr);
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MAX)) {
        DisplayBwSerr(host,socket,ch, dimm, rank, bwSerr[ch], piDelay);
      }
#endif // SERIAL_DBG_MSG
      for (bit = 0; bit < MAX_BITS; bit++) {
        if ((!host->nvram.mem.eccEn) && (bit > 63)) continue;

        if (bit < 32) errResult = bwSerr[ch][0];
        else if (bit < 64) errResult = bwSerr[ch][1];
        else errResult = bwSerr[ch][2];


        for (piIndex = 0; piIndex < RD_DQS_STEP_SIZE; piIndex++) {
          // Any failure is a failure for the strobe
          // Assumption for SHL operation: if shift bit above 32, operation will become << (Bit%32) by CPU instruction set
          if (errResult & (1<<bit)) {
            ErrorResult[ch].BitErrorResult[bit][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 1;
          } else {
            ErrorResult[ch].BitErrorResult[bit][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 0;
          }
        } // piIndex loop
      } // bit loop
    } // ch loop

    CHIP_FUNC_CALL(host, IO_Reset(host, socket));
  } // piDelay loop

  return SUCCESS;
}

/**

  The function gets number of error samples and finds the middle of zero error

  @param host              - Pointer to sysHost
  @param socket              - Socket number
  @param ch                - Channel number
  @param dimm              - DIMM number
  @param rank              - Rank number
  @param strobe
  @param SdRamErrorSamples - Array with 64 sampes for each dqs pi setting
  @param ErrorSamplesSize  - Include how many sample we have in the array 72 or 64
  @param errorStart        - Pointer to location for error start
  @param errorEnd          - Pointer to location for error end

  @retval status

**/
UINT32
FindErrorGraphMin (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     strobe,
  UINT8     *SdRamErrorSamples,
  UINT16    ErrorSamplesSize,
  UINT16    *errorStart,
  UINT16    *errorEnd,
  UINT16    *piPosition
  )
{
  UINT16  ZeroSize;
  UINT16  i;
  UINT16  TempErrorEnd;
  UINT16  TempErrorStart;
  UINT32  status;

  TempErrorEnd    = ErrorSamplesSize;
  TempErrorStart  = ErrorSamplesSize;
  status          = SUCCESS;

  *errorEnd       = ErrorSamplesSize;
  *errorStart     = ErrorSamplesSize;

  //
  // If the first sample passed and the last sample failed
  //
  if (SdRamErrorSamples[0] == 0) {
    TempErrorEnd = 0;
  }
  //
  // Loop through each sample
  //
  for (i = 1; i < ErrorSamplesSize; i++) {
    //
    // Find the place of error start
    //
    if (TempErrorEnd == ErrorSamplesSize) {
      //
      // If this sample passed and the previous sample failed
      //
      if ((SdRamErrorSamples[i] == 0) && (SdRamErrorSamples[i - 1])) {
        TempErrorEnd = i;

        //
        // Check if this is the last sample and it passed
        //
        if (i == (ErrorSamplesSize - 1)) {
          TempErrorStart = i;

          //
          // Update error start and error end
          //
          if ((*errorStart - *errorEnd) < (TempErrorStart - TempErrorEnd)) {
            *errorEnd   = TempErrorEnd;
            *errorStart = TempErrorStart;

            //
            // Clean the temp variable
            //
            TempErrorEnd    = ErrorSamplesSize;
            TempErrorStart  = ErrorSamplesSize;
          }
        }
      }
    } else {
      //
      // TempErrorStart != ErrorSamplesSize // Find the place of error end
      //

      //
      // If this sample failed and the previous sample passed
      //
      if (SdRamErrorSamples[i] && (SdRamErrorSamples[i - 1] == 0)) {
        //
        // We found the first error sample
        //
        TempErrorStart = (i - 1);

        //
        // Update error start and error end
        //
        if ((*errorStart - *errorEnd) <= (TempErrorStart - TempErrorEnd)) {
          *errorEnd   = TempErrorEnd;
          *errorStart = TempErrorStart;
        }

        //
        // Clean the temp variable
        //
        TempErrorEnd    = ErrorSamplesSize;
        TempErrorStart  = ErrorSamplesSize;
      }
      //
      // Check if this is the last sample and it passed
      //
      if ((i == (ErrorSamplesSize - 1)) && (SdRamErrorSamples[i] == 0) && (SdRamErrorSamples[0])) {
        //
        // We found the first error sample
        //
        TempErrorStart = i;

        //
        // Update error start and error end
        //
        if ((*errorStart - *errorEnd) < (TempErrorStart - TempErrorEnd)) {
          *errorEnd   = TempErrorEnd;
          *errorStart = TempErrorStart;

          //
          // Clean the temp variable
          //
          TempErrorEnd    = ErrorSamplesSize;
          TempErrorStart  = ErrorSamplesSize;
        }
      }
    }
  } // ErrorSamplesSize loop

  //
  // in case of cycle data like #######EEEEEEEEEEEE ########.
  //
  if ((TempErrorEnd < ErrorSamplesSize) && (TempErrorStart == ErrorSamplesSize)) {
    //
    // If the first sample failed
    //
    if (SdRamErrorSamples[0]) {
      TempErrorStart = (ErrorSamplesSize - 1);
      //
      // the case is like the next: EEEEEEEEEEEE##############
      //
    } else {
      for (i = 1; i < ErrorSamplesSize; i++) {
        //
        // If this sample failed and the previous sample passed
        //
        if ((SdRamErrorSamples[i]) && (SdRamErrorSamples[i - 1] == 0)) {
          TempErrorStart = (i - 1);
          //
          // Break because we found the start of the errors
          //
          break;
        }
      } // ErrorSamplesSize loop
    }

    if ((*errorStart - *errorEnd) < ((TempErrorStart + ErrorSamplesSize) - TempErrorEnd)) {
      *errorEnd   = TempErrorEnd;
      *errorStart = TempErrorStart;
    }
  }
#ifdef FAULTY_PARTS_TRACKING_INJECT_2ND_ERROR
  if ((ch == 0) && (rank == 0) && (strobe == 7)) {
    *errorEnd = *errorStart;
  }
#endif // FAULTY_PARTS_TRACKING_INJECT_2ND_ERROR

  //
  // Result correction checking and find the Pi position
  //
  if (*errorEnd == ErrorSamplesSize || *errorStart == ErrorSamplesSize || *errorEnd == *errorStart) {

    status = FAILURE;
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                   "Dq/Dqs Pi not found. Using default\n"));
    *piPosition = 36;
    *errorStart = 42;
    *errorEnd   = 30;
  } else {
    if (*errorStart < *errorEnd) {
      ZeroSize                      = ((ErrorSamplesSize - (UINT8)*errorEnd) + (UINT8)*errorStart);
      *piPosition = (*errorEnd + (ZeroSize / 2)) % ErrorSamplesSize;
    } else {
      ZeroSize                      = (UINT8)(*errorStart - *errorEnd);
      *piPosition = *errorEnd + (ZeroSize / 2);
    }
  }

  return status;
}

/**

  Initialize the Rd DqDqs operation

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void
RdDqDqsInit (
  PSYSHOST host,
  UINT8    socket
  )
{
  //
  // Fill 1 cacheline of the WDB with a clock pattern
  //
  WriteTemporalPattern(host, socket, CHIP_FUNC_CALL(host, GetTemporalPatternChip(host)), 1);
} // RdDqDqsInit

#ifdef SERIAL_DBG_MSG

/**

  Display a summary of the results

  @param host            - Pointer to sysHost
  @param socket            - Socket number
  @param ch              - Channel number
  @param dimm            - DIMM number
  @param rank            - Rank number
  @param ErrorResult     - Array that include that data to display
  @param PiMaxNumber     - Array size. It can be 128 or 72 pi
  @param ReadMode        - if ReadMode is:
                             0 = "Read DQ/DQS"
                             1 = "Read Vref"
                             2 = "Write DQ/DQS"
                             3 = "Write Vref"
  @param AggressiveMode  - If true that caller is aggressive training

  @retval N/A

**/
void
DqDqsDebugDataSummarization (
  PSYSHOST     host,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  TErrorResult *ErrorResult,
  UINT8        PiMaxNumber,
  UINT8        ReadMode,
  UINT8        AggressiveMode
  )
{
  UINT8 i;
  UINT8 strobe;

  getPrintFControl (host);

  rcPrintf ((host, "\n"));
  if (AggressiveMode) {
    rcPrintf ((host, "Aggressive "));
  }
  if (ReadMode == 0) {
    rcPrintf ((host, "Read DQ/DQS "));
  } else if (ReadMode == 1){
    rcPrintf ((host, "Read Vref "));
  } else if (ReadMode == 2){
    rcPrintf ((host, "Write DQ/DQS "));
  } else if (ReadMode == 3){
    rcPrintf ((host, "Write Vref "));
  }

  rcPrintf ((host, "summary for socket:%d channel:%d dimm:%d rank:%d\n", socket, ch, dimm, rank));
  PrintLine(host, 85, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
  if (host->nvram.mem.eccEn) {
    rcPrintf ((host, "       0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17\n"));
  } else {
    rcPrintf ((host, "       0   1   2   3   4   5   6   7   9  10  11  12  13  14  15  16\n"));
  }
  for (i = 0; i < PiMaxNumber; i++) {
    rcPrintf ((host, "%d ", i));
    if (i < 10) {
      rcPrintf ((host, "  "));
    }

    if ((i < 100) && (i > 9)) {
      rcPrintf ((host, " "));
    }

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      rcPrintf ((host, "   "));
      if (ErrorResult->DimmErrorResult[strobe][i]) {
        rcPrintf ((host, "#"));
      } else {
        rcPrintf ((host, "."));
      }
    } // strobe loop

    setWhite (host);

    rcPrintf ((host, "\n"));
  } // i loop

  releasePrintFControl (host);
} // DqDqsDebugDataSummarization

/**

  Display a summary of the results

  @param host            - Pointer to sysHost
  @param socket            - Socket number
  @param ch              - Channel number

  @retval N/A

**/
void
DisplayErrorStartEnd (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT16    piPosition[MAX_STROBE],
  UINT16    errorStart[MAX_STROBE],
  UINT16    errorEnd[MAX_STROBE]
  )
{
  UINT8 strobe;

  if (!(checkMsgLevel(host, SDBG_MAX))) return;

  getPrintFControl (host);

  PrintLine(host, 76, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

  // Print error end (passing starts)
  rcPrintf ((host, "EE: "));
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    rcPrintf ((host, " %3d", errorEnd[strobe]));
  } // strobe loop

  // Print Pi Position (center point)
  rcPrintf ((host, "\nPP: "));
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    rcPrintf ((host, " %3d", piPosition[strobe]));
  } // strobe loop

  // Print error start (passing stops)
  rcPrintf ((host, "\nFE: "));
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    rcPrintf ((host, " %3d", errorStart[strobe]));
  } // strobe loop

  rcPrintf ((host, "\n\n"));

  releasePrintFControl (host);
} // DisplayErrorStartEnd
#endif //   SERIAL_DBG_MSG

/**

  Get's the resutls from the current test

  @param host      - Pointer to sysHost
  @param dimm      - DIMM number
  @param rank      - rank number of the DIMM

  @retval N/A

**/
void
GetResultsPassFail (
  PSYSHOST      host,
  UINT8         socket,
  UINT8         dimm,
  UINT8         rank,
  TErrorResult  *ErrorResult,
  UINT16        piDelay
  )
{
  UINT8               ch;
  UINT8               strobe;
  UINT8               status;
  UINT8               step;
  UINT32              feedBack;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;


    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    //SKX change for upper strobes in N1 registers
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      feedBack = CHIP_FUNC_CALL(host, GetDataTrainFeedback (host, socket, ch, strobe));
      if ((feedBack & 1) == 0) {
        status = 1;
      } else {
        status = 0;
      }

      for (step = 0; step < RD_DQS_STEP_SIZE; step++) {
        ErrorResult[ch].DimmErrorResult[strobe][piDelay + step] = status;
      }

    } // strobe loop
  } // ch loop
} // GetResultsPassFail

