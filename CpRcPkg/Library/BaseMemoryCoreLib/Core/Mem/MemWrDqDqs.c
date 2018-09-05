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
#include "Cpgc.h"

//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif



//
// Local function prototypes
//
static
void
WrDqDqsInit (
            PSYSHOST        host,
            UINT8           socket
            );

static
UINT32
DqDqsDataCollecting (
                    PSYSHOST        host,
                    UINT8           socket,
                    UINT8           dimm,
                    UINT8           rank,
                    TErrorResult    *ErrorResult
                    );


static
UINT32
FindPerBitResults (
  PSYSHOST     host,
  UINT8        socket,
  UINT8        dimm,
  UINT8        rank,
  BErrorResult *ErrorResult
  );

//
// Internal definitions
//
#define MAX_CYCLE_DELAY 2 // define the max value of cycle delay
// Define the max number of sample in this algorithm
//
#define WRITE_DQ_DQS_MAX_SAMPLE (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)

/**

  Perform Write DqDqs Training

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
WrDqDqs (
        PSYSHOST host
        )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               Offset;
  UINT32              chBitmask;
  UINT8               dimm;
  UINT8               rank;
  UINT8               logRank = 0;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT8               rankPresent;
  UINT16              errorStart[MAX_CH][MAX_STROBE];
  UINT16              errorEnd[MAX_CH][MAX_STROBE];
  UINT16              piPosition[MAX_CH][MAX_STROBE];
  UINT8               worstLeft;
  UINT8               worstRight;
#ifdef SERIAL_DBG_MSG
  UINT16              delay;
#endif // SERIAL_DBG_MSG
  UINT32              status;
  UINT16              txDqs;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  TErrorResult        ErrorResult[MAX_CH];
  UINT8               faultyPartsStatus;
  UINT8               problemStrobe = 0;

  TCPGCAddress CPGCAddress = {{0, 0, 0, 0}, // Start
                             {0, 0xF, 0, 0x1F}, // Stop
                             {0, 0, 0, 0},    // Order
                             {0, 0, 0, 0},    // IncRate
                             {0, 4, 0, 1}};   // IncValue

  TWDBPattern WDBPattern  = {0,  0,  0,  BasicVA};

  if (~host->memFlows & MF_WR_DQ_EN) return SUCCESS;

  socket = host->var.mem.currentSocket;
  Offset = CHIP_FUNC_CALL(host, GetWrDqDqsOffset(host));

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  faultyPartsStatus = FPT_NO_ERROR;

  status = SUCCESS;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WrDqDqs Starts\n"));
#endif

  WrDqDqsInit (host, socket);

  channelNvList = GetChannelNvList(host, socket);

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
    CPGCAddress.Stop[1] = 0x7; // Bank wrap address
    CPGCAddress.IncVal[1] = 4; // Bank increment value
  }

  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, TX_DQ_BASIC, (UINT16)((socket << 8)|(dimm << 4)|rank)));
      //
      // Initialize to no ranks present
      //
      rankPresent = 0;

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList  = GetDimmNvList(host, socket, ch);
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;
        //
        // Indicate this rank is present on at least one channel
        //
        rankPresent |= (*rankList)[rank].enabled;
        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank (host, socket, ch, dimm, rank);

        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, 0));

      } // ch loop

      chBitmask = GetChBitmask (host, socket, dimm, rank);

      CHIP_FUNC_CALL(host, SetupIOTest (host, socket, chBitmask, PatWrRd, 100, 1, &CPGCAddress, NSOE, &WDBPattern, 0, 0, 0));

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((chBitmask & (1 << ch)) == 0) continue;
        CHIP_FUNC_CALL(host, SetCpgcPatControl (host, socket, ch, BTBUFFER, BTBUFFER, BTBUFFER, DDR_CPGC_PATBUF_MUX0, DDR_CPGC_PATBUF_MUX1, DDR_CPGC_PATBUF_MUX2));
      } // ch loop

      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (!rankPresent) continue;

      //
      // Data collecting
      //
      status = DqDqsDataCollecting (host, socket, dimm, rank, ErrorResult);

      //
      // Data prosessing for each channel.
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList  = GetDimmNvList(host, socket, ch);
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          DqDqsDebugDataSummarization (host, socket, ch, dimm, rank, &ErrorResult[ch], WRITE_DQ_DQS_MAX_SAMPLE, 2, 0);
        }
#endif // SERIAL_DBG_MSG

        maxStrobe = MAX_STROBE;

        worstLeft = 0xFF;
        worstRight = 0xFF;

        status = SUCCESS;

        for (strobe = 0; strobe < maxStrobe; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          if ((*rankList)[rank].faultyParts[strobe] != 0) {
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "skipped Write DqDqs FindErrorGraphMin\n"));
            continue;
          }
          status = status | FindErrorGraphMin (host, socket, ch, dimm, rank, strobe, &ErrorResult[ch].DimmErrorResult[strobe][0],
                                               WRITE_DQ_DQS_MAX_SAMPLE, &errorStart[ch][strobe],
                                               &errorEnd[ch][strobe], &piPosition[ch][strobe]);
          errorStart[ch][strobe] = errorStart[ch][strobe] + WR_DQ_PI_START;
          errorEnd[ch][strobe] = errorEnd[ch][strobe] + WR_DQ_PI_START;
          piPosition[ch][strobe] = piPosition[ch][strobe] + WR_DQ_PI_START;

          //
          // Add in Tx Dqs offset
          //
          if ((*dimmNvList)[dimm].x4Present) {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
          } else {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe % 9, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
          }
          if (host->var.mem.xoverModeVar == XOVER_MODE_2TO2) {
            errorStart[ch][strobe] = (UINT16)(txDqs - TX_DQ_START_OFFSET_2TO2 + Offset) + errorStart[ch][strobe];
            errorEnd[ch][strobe] = (UINT16)(txDqs - TX_DQ_START_OFFSET_2TO2) + Offset + errorEnd[ch][strobe];
            piPosition[ch][strobe] = (UINT16)(txDqs - TX_DQ_START_OFFSET_2TO2) + Offset + piPosition[ch][strobe];
          } else {
            errorStart[ch][strobe] = (UINT16)(txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME) + errorStart[ch][strobe];
            errorEnd[ch][strobe] = (UINT16)(txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME) + errorEnd[ch][strobe];
            piPosition[ch][strobe] = (UINT16)(txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME) + piPosition[ch][strobe];
          }

          if (status == FAILURE) {
            //
            // Mark this part as faulty
            //
            host->var.mem.faultyPartsFlag[ch] |= (1 << strobe);

            (*rankList)[rank].faultyParts[strobe] |= FPT_WR_DQ_DQS_FAILED;

            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "Failed Write DqDqs FindErrorGraphMin\n"));

            faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);

            if (faultyPartsStatus == FPT_NO_ERROR) {
              // do we need to do anything here ?!?
              status = SUCCESS;
            } else if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
              // if correctable log the warning -> OutputWarning
              // careful for cases when warning gets promoted to error !!!  - log the strobe
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_WR_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, TxDqsDelay, DdrLevel, (UINT8)(errorStart[ch][strobe] - errorEnd[ch][strobe]));
              status = SUCCESS;
            } else {
              // no need to warning/disable channel - it is done out of the loop
              problemStrobe = strobe;
              status = FAILURE;
              break;
            }
          }

          if ((piPosition[ch][strobe] > errorEnd[ch][strobe]) && (piPosition[ch][strobe] - errorEnd[ch][strobe]) < worstLeft) {
            worstLeft = (UINT8)(piPosition[ch][strobe] - errorEnd[ch][strobe]);
          } else if ((errorEnd[ch][strobe] > piPosition[ch][strobe]) && (piPosition[ch][strobe] + (128 - errorEnd[ch][strobe])) < worstLeft) {
            worstLeft = (UINT8)(piPosition[ch][strobe] + (128 - errorEnd[ch][strobe]));
          }
          if ((piPosition[ch][strobe] > errorStart[ch][strobe]) && ((128 - piPosition[ch][strobe]) + errorStart[ch][strobe]) < worstRight) {
            worstRight = (UINT8)((128 - piPosition[ch][strobe]) + errorStart[ch][strobe]);
          } else if ((errorStart[ch][strobe] > piPosition[ch][strobe]) && (errorStart[ch][strobe] - piPosition[ch][strobe]) < worstRight) {
            worstRight = (UINT8)(errorStart[ch][strobe] - piPosition[ch][strobe]);
          }

          //
          // Program Pi
          //
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE, (INT16 *)&piPosition[ch][strobe]));
        } // strobe loop

        if (status) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "WrDq/Dqs Failure! (status = %d)\n", status));
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_WR_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, problemStrobe, NO_BIT, TxDqsDelay, DdrLevel, (UINT8)(errorStart[ch][problemStrobe] - errorEnd[ch][problemStrobe]));


          //
          // This is a fatal error condition so map out the entire channel.
          //
          DisableChannel(host, socket, ch);
        } // if status

#ifdef SERIAL_DBG_MSG
        // Display results
        DisplayErrorStartEnd(host, socket, ch, piPosition[ch], errorStart[ch], errorEnd[ch]);

        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "START_DATA_TX_DQ_BASIC\n"));
        }
        for (strobe = 0; strobe < maxStrobe; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_ONLY, (INT16 *)&delay));
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                        "TxDqDqs: Pi = %d\n", delay));
        } // strobe loop

        if (checkMsgLevel(host, SDBG_MAX)) {
          rcPrintf ((host, "STOP_DATA_TX_DQ_BASIC\n"));
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
        if (worstLeft > 4) {
          (*rankList)[rank].txDQLeftSt = worstLeft - 3;
        } else {
          (*rankList)[rank].txDQLeftSt = worstLeft;
        }
        if (worstRight > 4) {
          (*rankList)[rank].txDQRightSt = worstRight - 3;
        } else {
          (*rankList)[rank].txDQRightSt = worstRight;
        }
      } // ch loop
    } // rank loop
  } // dimm loop

  CHIP_FUNC_CALL(host, IO_Reset(host, socket));

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "WrDqDqs Ends\n"));
#endif

  return SUCCESS;
} // WrDqDqs

UINT32
WriteDqDqsPerBit (
  PSYSHOST host
  )
{
  UINT8                             socket;
  UINT8                             ch;
  UINT32                            chBitmask;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             logRank = 0;
  UINT8                             nibble;
  UINT8                             encodedNibble;
  UINT8                             bit, bitIdx, msl;
  INT16                             bitSkew, bitNibbleMin, bitSkewMax;
  UINT16                            bitCenter[8];
  UINT8                             rankPresent;
  PerBitPi                          errorStart[MAX_CH];
  PerBitPi                          errorEnd[MAX_CH];
  PerBitPi                          piPosition[MAX_CH];
  UINT32                            status;
  UINT16                            txDqs;
  UINT8                             faultyPartsStatus;
  UINT8                             problemEyeSize = 0;
  UINT8                             problemStrobe = 0;
  struct bitMarginCh                resultsBit;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  BErrorResult                      ErrorResult[MAX_CH];
  TCPGCAddress CPGCAddress = {{0, 0, 0, 0}, // Start
                             {0, 0xF, 0, 0x1F}, // Stop
                             {0, 0, 0, 0},    // Order
                             {0, 0, 0, 0},    // IncRate
                             {0, 4, 0, 1}};   // IncValue

  TWDBPattern WDBPattern  = {0,  0,  0,  BasicVA};

  if (~host->memFlows & MF_WR_DQ_EN) return SUCCESS;

  socket = host->var.mem.currentSocket;

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  status = SUCCESS;

  WrDqDqsInit (host, socket);

  channelNvList = GetChannelNvList(host, socket);
  // Initialize LE and RE Margin results structure
  for (ch = 0; ch < MAX_CH; ch++) {
    for (bit = 0; bit < MAX_BITS; bit++) {
      resultsBit.bits[ch][bit].n = 0;
      resultsBit.bits[ch][bit].p = 0;
    }
  } // Ch loop

  if (host->nvram.mem.dramType == SPD_TYPE_DDR3){
    CPGCAddress.Stop[1] = 0x7; // Bank wrap address
    CPGCAddress.IncVal[1] = 4; // Bank increment value
  }

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
        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);

        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, 0));

      } // ch loop

      chBitmask = GetChBitmask (host, socket, dimm, rank);

      CHIP_FUNC_CALL(host, SetupIOTest (host, socket, chBitmask, PatWrRd, 32, 1, &CPGCAddress, NSOE, &WDBPattern, 0, 0, CPGC_SUBSEQ_WAIT_DDRT)); //FNV restricted to 32

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((chBitmask & (1 << ch)) == 0) continue;
        CHIP_FUNC_CALL(host, SetCpgcPatControl (host, socket, ch, BTBUFFER, BTBUFFER, BTBUFFER, DDR_CPGC_PATBUF_MUX0, DDR_CPGC_PATBUF_MUX1, DDR_CPGC_PATBUF_MUX2));
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

        status = SUCCESS;

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

          if ((*rankList)[rank].faultyParts[encodedNibble] != 0) {
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, encodedNibble, NO_BIT,
                           "skipped Write DqDqs FindErrorGraphMin\n"));
            continue;
          }

          status = FindErrorGraphMin (host, socket, ch, dimm, rank, 0, &ErrorResult[ch].BitErrorResult[bit][0],
                                      WRITE_DQ_DQS_MAX_SAMPLE, &errorStart[ch].PiPosition[bit],
                                      &errorEnd[ch].PiPosition[bit], &piPosition[ch].PiPosition[bit]);

          errorStart[ch].PiPosition[bit] = errorStart[ch].PiPosition[bit] + WR_DQ_PI_START;
          errorEnd[ch].PiPosition[bit] = errorEnd[ch].PiPosition[bit] + WR_DQ_PI_START;
          piPosition[ch].PiPosition[bit] = piPosition[ch].PiPosition[bit] + WR_DQ_PI_START;

          //
          // Add in Tx Dqs offset
          //
          if ((*dimmNvList)[dimm].x4Present) {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, encodedNibble, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
          } else {
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, encodedNibble % 9, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
          }

          errorStart[ch].PiPosition[bit] = (UINT16)(txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME) + errorStart[ch].PiPosition[bit];
          errorEnd[ch].PiPosition[bit] = (UINT16)(txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME) + errorEnd[ch].PiPosition[bit];
          piPosition[ch].PiPosition[bit] = (UINT16)(txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME) + piPosition[ch].PiPosition[bit];

          //LE and RE Margins
          resultsBit.bits[ch][bit].n = errorEnd[ch].PiPosition[bit];
          resultsBit.bits[ch][bit].p = errorStart[ch].PiPosition[bit];

          if (status == FAILURE) {
            //
            // Mark this part as faulty
            //
            host->var.mem.faultyPartsFlag[ch] |= (1 << encodedNibble);

            (*rankList)[rank].faultyParts[encodedNibble] |= FPT_WR_DQ_DQS_FAILED;

            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, encodedNibble, bit,
                           "Failed Write DqDqsPerBit\n"));

            faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);

            if (faultyPartsStatus == FPT_NO_ERROR) {
              // do we need to do anything here ?!?
              status = SUCCESS;
            } else if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
              // if correctable log the warning -> OutputWarning
              // careful for cases when warning gets promoted to error !!!  - log the strobe
              MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, dimm, rank, encodedNibble, NO_BIT,
                             "WrDqDqs Correctable\n"));
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_WR_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, encodedNibble, bit, TxDqDelay, DdrLevel, (UINT8)(errorStart[ch].PiPosition[bit] - errorEnd[ch].PiPosition[bit]));
              status = SUCCESS;
            } else {
              // no need to warning/disable channel - it is done out of the loop
              problemEyeSize = (UINT8)(errorStart[ch].PiPosition[bit] - errorEnd[ch].PiPosition[bit]);
              problemStrobe = encodedNibble;
              status = FAILURE;
              break;
            }
          }
        } // bit loop

#ifdef SERIAL_DBG_MSG
        //Print LE and RE Per Bit Deskew Margins for current rank
        if (checkMsgLevel(host, SDBG_MAX)) {
          DisplayREandLEMargins (host, socket, ch, dimm, rank, &resultsBit, TxDqDelay);
        }
#endif // SERIAL_DBG_MSG

        if (status) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                         "WrDq/Dqs Training Failure! (status = %d)\n", status));
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_WR_DQ_DQS, socket, ch, dimm, rank, EwlSeverityWarning, problemStrobe, NO_BIT, TxDqsDelay, DdrLevel, problemEyeSize);

          //
          // This is a fatal error condition so map out the entire channel.
          //
          DisableChannel(host, socket, ch);
        } // if status

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
          bitNibbleMin = 512;   // Set to largest possible
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
          } // bit loop

          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "  %3d   ", bitNibbleMin));
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, encodedNibble, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE, &bitNibbleMin));
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
            CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, encodedNibble, bit, DdrLevel, TxDqBitDelay, GSM_UPDATE_CACHE | GSM_WRITE_OFFSET,
                             &bitSkew));

          } // bit loop
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " %3d\n", msl));
        } // nibble loop
      } //ch loop
    } // rank loop
  } // dimm loop
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    DisplayResults(host, socket, TxDqDelay);
    DisplayResults(host, socket, TxDqBitDelay);
  }
#endif // SERIAL_DBG_MSG
  CHIP_FUNC_CALL(host, IO_Reset(host, socket));
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));

  return status;

} //WriteDqDqsPerBit

/**

  Initialize Wr DQ/DQs parameters

  @param host                - Pointer to sysHost
  @param socket                - Socket number

  @retval N/A

**/
static
void
WrDqDqsInit (
            PSYSHOST        host,
            UINT8           socket
            )
{
  UINT8               ch;
  UINT8               bit;
  UINT8               line;
  UINT8               chunk;
  UINT8               maxChunk;
  UINT8               maxBits;
  UINT32              pattern = 0;
  UINT32              *WdbLinePtr;
  TWdbLine            WdbLines[MRC_WDB_LINES];
  struct channelNvram (*channelNvList)[MAX_CH];

  maxBits = 8;

  //
  // Fill the WDB buffer with the write content.
  //
  for (bit = 0; bit < maxBits; bit++) {

    if (bit & 0x1) {
      pattern = MRC_WDB_PATTERN_N; //take back to jason SKX and BDX work with a checkerboard pattern
    } else {
      pattern = MRC_WDB_PATTERN_P;
    }

    //
    // Get the cacheline this bit belongs to
    //
    line = bit / 8;

    //
    // Get the byte pair (they call it chunk, but it's not). There are 32 byte pairs
    //
    chunk = (bit * 2) % 16;
    maxChunk = chunk + 2;

    WdbLinePtr = (UINT32 *) &WdbLines[line].WdbLine[0];
    for (; chunk < maxChunk; chunk++) {
      WdbLinePtr[chunk] = pattern; // We need to iterate through all 64 of these
    } // chunk loop
  } // bit loop

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    WDBFill (host, socket, ch, WdbLines, 1, 0);
  } // ch loop

} // WrDqDqsInit


/**

  This function collects all the data from DqDqs algorithm

  @param host                - Pointer to sysHost
  @param socket                - Socket number
  @param dimm                - DIMM number
  @param rank                - Rank number
  ErrorResult         - Error result return data

  @retval 0 - successful
  @retval Other - failure

**/
static
UINT32
DqDqsDataCollecting (
                    PSYSHOST        host,
                    UINT8           socket,
                    UINT8           dimm,
                    UINT8           rank,
                    TErrorResult    *ErrorResult
                    )
{
  UINT8               ch;
  UINT8               Offset;
  UINT32              chBitmask;
  INT16               piDelay;
  UINT8               piIndex;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT32              errResult;
  UINT32              status;
  UINT16              txDqs;
  UINT16              txDq;
  UINT8               DumArr[7] = {1,1,1,1,1,1,1};
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  status    = SUCCESS;

  Offset = CHIP_FUNC_CALL(host, GetWrDqDqsOffset(host));

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0)  continue;

    dimmNvList  = GetDimmNvList(host, socket, ch);
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    //
    // Clear dimm error results
    //
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      for (piDelay = 0; piDelay < MAX_PHASE_IN_FINE_ADJUSTMENT * 2; piDelay++) {
        ErrorResult[ch].DimmErrorResult[strobe][piDelay] = 0;
      } // piDelay loop
    } // strobe loop
  } // ch loop

  chBitmask = GetChBitmask (host, socket, dimm, rank);

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                 "TxDqDqs Pi Scanning...\n"));

  for (piDelay = WR_DQ_PI_START; piDelay < WR_DQ_PI_START + WR_DQ_PI_RANGE; piDelay += WR_DQ_STEP_SIZE) {

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList  = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      //
      // Set the new Pi.
      //
      maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));

      for (strobe = 0; strobe < maxStrobe; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
        if (host->var.mem.xoverModeVar == XOVER_MODE_2TO2) {
          txDq = txDqs - TX_DQ_START_OFFSET_2TO2 + piDelay + Offset;
        } else {
          txDq = txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME + piDelay;
        }
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE, (INT16 *)&txDq));
        if (!(*dimmNvList)[dimm].x4Present) {
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE, (INT16 *)&txDq));
        }
      } // strobe loop
    } // ch loop

    //
    // Clear Errors and Run Test
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList  = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      errResult = CHIP_FUNC_CALL(host, GetErrorResults (host, socket, ch));

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        for (piIndex = 0; piIndex < WR_DQ_STEP_SIZE; piIndex++) {
          // Any failure is a failure for the strobe
          if (errResult & (1 << (strobe * 2 - (strobe >= 9) * 17))) {
            ErrorResult[ch].DimmErrorResult[strobe][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 1;
          } else {
            ErrorResult[ch].DimmErrorResult[strobe][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 0;
          }
        } // piIndex loop
      } // strobe loop
    } // ch loop
  } // piDelay loop

  return status;
} // DqDqsDataCollecting


static
UINT32
FindPerBitResults (
  PSYSHOST        host,
  UINT8           socket,
  UINT8           dimm,
  UINT8           rank,
  BErrorResult *ErrorResult
  )
/*++

  This function collects all the data from DqDqs algorithm

  @param host                - Pointer to sysHost
  @param socket                - Socket number
  @param dimm                - DIMM number
  @param rank                - Rank number
  ErrorResult         - Error result return data

  @retval 0 - successful
  @retval Other - failure

--*/
{
  UINT8               ch;
  UINT32              chBitmask;
  UINT8               bit;
  INT16               piDelay;
  UINT8               piIndex;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT32              errResult;
  UINT32              status;
  UINT16              txDqs;
  UINT16              txDq;
  UINT8               chStatus;
  UINT32              bwSerr[MAX_CH][3];
  UINT8               DumArr[7] = {1,1,1,1,1,1,1};
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  status    = SUCCESS;

  channelNvList = GetChannelNvList(host, socket);

  chBitmask = GetChBitmask (host, socket, dimm, rank);
  errResult = 0;
  chStatus = 0;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0)  continue;

    dimmNvList  = GetDimmNvList(host, socket, ch);
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

    //
    // Clear dimm error results
    //
    for (bit = 0; bit < MAX_BITS; bit++) {
      for (piDelay = 0; piDelay < MAX_PHASE_IN_FINE_ADJUSTMENT * 2; piDelay++) {
        ErrorResult[ch].BitErrorResult[bit][piDelay] = 0;
      } // piDelay loop
    } // bit loop
  } // ch loop

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
                "TxDqDqs Pi Scanning...\n"));

  for (piDelay = WR_DQ_PI_START; piDelay < WR_DQ_PI_START + WR_DQ_PI_RANGE; piDelay += WR_DQ_STEP_SIZE) {

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList  = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;

      //
      // Set the new Pi.
      //
      maxStrobe = CHIP_FUNC_CALL(host, GetMaxStrobe(host, socket, ch, dimm, WrLvlDelay, DdrLevel));

      for (strobe = 0; strobe < maxStrobe; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, (INT16 *)&txDqs));
        txDq = txDqs - TX_DQ_START_OFFSET - TX_PER_BIT_SETTLE_TIME + piDelay;
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE, (INT16 *)&txDq));
        if (!(*dimmNvList)[dimm].x4Present) {
          CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe + 9, 0, DdrLevel, TxDqDelay, GSM_UPDATE_CACHE, (INT16 *)&txDq));
        }
      } // strobe loop
    } // ch loop

    //
    // Clear Errors and Run Test
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList  = GetDimmNvList(host, socket, ch);
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT) || (rank >= (*dimmNvList)[dimm].numRanks)) continue;
      chStatus = 1 << ch;
      bwSerr[ch][0] = 0;
      bwSerr[ch][1] = 0;
      bwSerr[ch][2] = 0;
      // Collect test results
      status = CollectTestResults (host, socket, chStatus, bwSerr);
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


        for (piIndex = 0; piIndex < WR_DQ_STEP_SIZE; piIndex++) {
          // Any failure is a failure for the bit
          // Assumption for SHL operation: if shift bit above 32, operation will become << (Bit%32) by CPU instruction set
          if (errResult & (1<<bit)) {
            ErrorResult[ch].BitErrorResult[bit][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 1;
          } else {
            ErrorResult[ch].BitErrorResult[bit][(piDelay % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + piIndex] = 0;
          }
        } // piIndex loop
      } // bit loop
    } // ch loop
  } // piDelay loop

  return status;
} // FindPerBitResults
