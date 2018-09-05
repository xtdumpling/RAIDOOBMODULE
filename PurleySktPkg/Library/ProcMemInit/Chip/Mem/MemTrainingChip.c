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
#include "SysFuncChip.h"
#include "MemProjectSpecific.h"
#include "RcRegs.h"
#include "MemApiSkx.h"
#include "MemFuncChip.h"

UINT8
CmdVrefQuick (
  PSYSHOST  host,
  UINT8     socket,
  GSM_GT    group
  )
/*++

Routine Description:

  Quick (1D) centering of Cmd Vref

Arguments:

  host    - Pointer to sysHost
  socket  - Socket number
  group   - CmdVref

Returns:

  Success or failure

--*/
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT16              mode;
  UINT32              patternLength;
  INT16               compLow,compHigh;
  INT16               aepoffset;
  struct bitMask      filter;
  struct rankMargin   resultsRank;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  UINT8               rankPerChannel;
  INT16               ch_offset[MAX_CH];
  channelNvList = GetChannelNvList(host, socket);

  patternLength = 32;
  // mode = MODE_VIC_AGG;
  mode = MODE_VA_DESELECT;

  // Setup bitMask
  for (ch = 0; ch < MAX_CH; ch++) {
    for (rankPerChannel = 0; rankPerChannel < MAX_RANK_CH; rankPerChannel++ ) {
      resultsRank.rank[ch][rankPerChannel].n = 0;
      resultsRank.rank[ch][rankPerChannel].p = 0;
      //offset[rankPerChannel] = 0;
    }
  } // ch loop

  //
  // Clear the filter bits to enable error checking on every bit
  //
  ClearFilter(host, socket, &filter);

  GetMargins (host, socket, DdrLevel, group, mode, SCOPE_RANK, &filter, (void *)&resultsRank, patternLength, 0, 0, 0,
              WDB_BURST_LENGTH);

  SetAepTrainingMode (host,socket, CMD_VREF_CENTERING);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    compLow = -UNMARGINED_CMD_EDGE;
    compHigh = UNMARGINED_CMD_EDGE;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      rankList = GetRankNvList(host, socket, ch, dimm);
      for (rank = 0; rank < (*channelNvList)[ch].dimmList[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        if (resultsRank.rank[ch][(*rankList)[rank].rankIndex].n > resultsRank.rank[ch][(*rankList)[rank].rankIndex].p) {
          resultsRank.rank[ch][(*rankList)[rank].rankIndex].n = 0;
          resultsRank.rank[ch][(*rankList)[rank].rankIndex].p = 0;
        }
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                      "Margin: %4d to %4d\n",
                      resultsRank.rank[ch][(*rankList)[rank].rankIndex].n, resultsRank.rank[ch][(*rankList)[rank].rankIndex].p));

        //if NVMDIMM, set VREF in DIMM
        if ((*channelNvList)[ch].dimmList[dimm].aepDimmPresent) {
          aepoffset = (resultsRank.rank[ch][(*rankList)[rank].rankIndex].n + resultsRank.rank[ch][(*rankList)[rank].rankIndex].p)/2;
          // Set new VREF per NVMDIMM
          GetSetCmdVref (host, socket, ch, dimm, DdrLevel, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &aepoffset);
        } else { // if any other kind of dimm store the results for composite channel
          if (resultsRank.rank[ch][(*rankList)[rank].rankIndex].n > compLow) {
            compLow = resultsRank.rank[ch][(*rankList)[rank].rankIndex].n;
          }
          if (resultsRank.rank[ch][(*rankList)[rank].rankIndex].p < compHigh) {
            compHigh = resultsRank.rank[ch][(*rankList)[rank].rankIndex].p;
          }
        }
      } // rank loop

    } // dimm loop

    //
    // Set new VREF per channel
    //
    if ((compLow != -UNMARGINED_CMD_EDGE) || (compHigh != UNMARGINED_CMD_EDGE)){

      ch_offset[ch] = (compLow + compHigh) / 2;
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Composite Low = %d - Composite High = %d final_offset = %d\n", compLow, compHigh, ch_offset[ch] ));

      GetSetCmdVref (host, socket, ch, 0, DdrLevel, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &ch_offset[ch]);
    }
  } // ch loop

  SetAepTrainingMode (host,socket, DISABLE_TRAINING_STEP);
  host->nvram.mem.socket[socket].cmdVrefTrainingDone = 1;
  //
  // Reset the IO
  //
  IO_Reset (host, socket);
  JedecInitSequence (host, socket, CH_BITMASK);

   /*
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    getPrintFControl (host);
    rcPrintf ((host, "START_DATA_CMD_VREF_CENTERING\n"));
  }
#endif  // SERIAL_DBG_MSG

  // Apply new center  SKX may be different
  for (ch = 0; ch < MAX_CH; ch++) {
  //ch = ch & BIT1;  // VCM need this to fix centering since the channel is out of range at this point

     MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Applying offset of %d...\n",final_offset[ch/2] ));
     GetSetCmdVref (host, socket, ch, GSM_WRITE_OFFSET | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &final_offset[ch/2]);

  }

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) {
    rcPrintf ((host, "STOP_DATA_CMD_VREF_CENTERING\n"));
    releasePrintFControl (host);
  }
#endif  // SERIAL_DBG_MSG
  */
  return SUCCESS;
} // CmdVrefQuick

UINT8
FindIndexResData (
                  UINT8 strobe
                 )
{
  UINT8 strobeIndex;
  strobeIndex = strobe;
  while (strobeIndex > 3) {
    strobeIndex -= 4;
  }
  return strobeIndex;
} //FindIndexResData


/**

  This function implements Sense Amp Offset training.
  SenseAmp offset cancellation
  Test VOC/senseamp settings 0 to 31 and look for transition

  @param host    - Pointer to sysHost

  @retval UINT32  - if it succeded return 0

**/
UINT32
SenseAmpOffset (
               PSYSHOST  host
               )
{
  UINT8                                 socket;
  UINT8                                 ch;
  UINT8                                 dimm;
  UINT8                                 rank;
  UINT8                                 strobe;
  UINT8                                 strobeWA;
  UINT8                                 bit;
  UINT32                                status;
  INT16                                 firstZero[MAX_CH][MAX_STROBE][MAX_BITS];
  INT16                                 lastOne[MAX_CH][MAX_STROBE][MAX_BITS];
  INT16                                 sampOffset;
  INT16                                 vref;
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct dimmNvram                      (*dimmNvList)[MAX_DIMM];
  struct ddrRank                        (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                     (*rankStruct)[MAX_RANK_DIMM];
  DATATRAINFEEDBACKN0_0_MCIO_DDRIO_A0_STRUCT dataTrainFeedback;
  RXOFFSETN0RANK0_0_MCIO_DDRIO_STRUCT        rxOffset;
  RXOFFSETN0RANK0_0_MCIO_DDRIO_STRUCT        rxOffsetTmp;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT         dataControl0;
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT         dataControl2;
  DATACONTROL4N0_0_MCIO_DDRIO_STRUCT         dataControl4;
#ifdef DEBUG_SENSEAMP
  UINT8                                 loopCnt = 0;
#endif

#ifdef SERIAL_DBG_MSG
  UINT64_STRUCT             startTsc  = { 0, 0};
  UINT64_STRUCT             endTsc = { 0, 0};
#endif  // SERIAL_DBG_MSG

  if (~host->memFlows & MF_SENSE_AMP_EN) return SUCCESS;

#ifdef SERIAL_DBG_MSG
  //
  // Start time
  //
  ReadTsc64 (&startTsc);
#endif // SERIAL_DBG_MSG

  socket = host->var.mem.currentSocket;
  OutputCheckpoint(host, STS_CHANNEL_TRAINING, SENSE_AMP, socket);
  channelNvList = GetChannelNvList(host, socket);
  status = 0;

#ifdef DEBUG_SENSEAMP
  for (loopCnt = 0; loopCnt < 100; loopCnt++) {
    OutputCheckpoint(host, STS_CHANNEL_TRAINING, loopCnt+1, socket);
#endif
  for (ch = 0; ch < MAX_CH; ch++) {

    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);

    //
    // Set Rtt Park to disabled
    //
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankStruct  = GetRankStruct(host, socket, ch, dimm);

      for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

#ifdef  LRDIMM_SUPPORT
        if (IsLrdimmPresent(host, socket, ch, dimm)) {
          WriteLrbuf(host, socket, ch, dimm, 0, DDR4_RTT_PARK_DIS, LRDIMM_F0, LRDIMM_BC02);
        } else
#endif
        {
          WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5 & ~DDR4_RTT_PRK_ODT_MASK, BANK5);
        }
      } // rank loop
    } // dimm loop

#ifdef PRINT_FUNC
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nSenseAmpOffset Starts\n"));
#endif

    // enable senseamp training mode
    // SKX HSD 4927892
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      // datacontrol2
      dataControl2.Data                 = (*channelNvList)[ch].dataControl2[strobe];
      dataControl2.Bits.rxvocselqnnnh   = 0x1f;
      dataControl2.Bits.forcerxon       = 1;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2.Data);

      // datacontrol4
      dataControl4.Data               = (*channelNvList)[ch].dataControl4[strobe];
      dataControl4.Bits.rxvrefsel     = 0; //HSD 4927283 should be 8 initially
      dataControl4.Bits.vreftonblbus  = 0;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), dataControl4.Data);

      // datacontrol0
      dataControl0.Data                       = (*channelNvList)[ch].dataControl0;  // going to add strobe soon
      dataControl0.Bits.senseamptrainingmode  = 0;
      dataControl0.Bits.forceodton            = 1;
      dataControl0.Bits.ddrcrforceodton       = 1;
      dataControl0.Bits.saoffsettraintxon     = 1;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);

      for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
        firstZero[ch][strobe][bit] = 99;
        lastOne[ch][strobe][bit] = 99;
      } // bit loop
    } // nibble loop


#ifdef SERIAL_DBG_MSG
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\nSampOffset Training\n"));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "              "));
    for (strobe = 0; strobe < MAX_STROBE ; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "%2d %2d ", (strobe+9)%18,strobe));
    } // strobe loop

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n       SA     "));
    for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8))) continue;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "01 23 01 23 "));
    } // strobe loop
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));

#endif // SERIAL_DBG_MSG

    for (sampOffset = 4; sampOffset < 27; sampOffset++) {

      // should not need dimm loop here
      //for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        dimm = 0;
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        //for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
        //
        // This only needs to get set for rank 0
        //
        rank = 0;
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          // Program Offset and Propagate new value from RF
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          rxOffset.Data = (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe];
          rxOffset.Bits.saoffset0 = sampOffset;
          rxOffset.Bits.saoffset1 = sampOffset;
          rxOffset.Bits.saoffset2 = sampOffset;
          rxOffset.Bits.saoffset3 = sampOffset;

          MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG), rxOffset.Data);
        } // strobe loop
      //} // dimm loop

      // Propagate delay values (without a read command)
      dataControl0.Bits.readrfrd              = 1;
      dataControl0.Bits.readrfwr              = 0;
      dataControl0.Bits.readrfrank            = 0;
      dataControl0.Bits.senseamptrainingmode  = 1;

      // SKX change for upper nibble
      for (strobe = 0; strobe < MAX_STROBE ; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      } // strobe loop

      IO_Reset(host, socket);

      dataControl0.Bits.readrfrd    = 0;
      // SKX change for upper nibble
      for (strobe = 0; strobe < MAX_STROBE ; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      } // strobe loop


      FixedDelay(host, 1);

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "VOC %2d ", sampOffset));

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        dataTrainFeedback.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATATRAINFEEDBACKN0_0_MCIO_DDRIO_REG));


        for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
          //SKX note: CTE model requires a change to match HW polarity
          //SKX HSD 4928076 swizzle SA feedback, this will recorded the feedback in the appropriate nibble
          if (bit<2){
            strobeWA = (strobe+9)%18;
          } else {
            strobeWA = strobe;
          }
          // HSD
          if (dataTrainFeedback.Bits.datatrainfeedback & (1 << bit)) {
            lastOne[ch][strobeWA][bit] = sampOffset;
          } else {
            if (firstZero[ch][strobeWA][bit] == 99) {
              firstZero[ch][strobeWA][bit] = sampOffset;
            }
          }

          //Display in bits
          if ((1 << bit) & dataTrainFeedback.Bits.datatrainfeedback) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "1"));
          } else {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "0"));
          }
          if (bit & BIT0) {
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," "));
          }
        } // bit
      } // strobe loop

      FixedDelay(host, 1);

      dataControl0.Bits.senseamptrainingmode = 0;
      // SKX change for upper nibble
      for (strobe = 0; strobe < MAX_STROBE ; strobe++) {
        if ((!host->nvram.mem.eccEn) && (strobe == 8)) continue;
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      } // strobe loop


      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n"));

    } // sampOffset loop
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\nBitSAmp\n             bit:  0  1  2  3\n"));

    // Calculate and Program Offsets and display per bit SenseAmp Offset
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Nibble %2d: ", strobe));

      rxOffset.Data = 0;
      for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
        // Find vref center, add 1 for Round Up
        vref = (firstZero[ch][strobe][bit] + lastOne[ch][strobe][bit]) / 2;

        // Check for saturation conditions
        // to make sure we are as close as possible to vdd/2 (750mv)
        if (firstZero[ch][strobe][bit] == 99) {   // never saw a 0
          vref = 31;
        }
        if (lastOne[ch][strobe][bit] == 99) {     // never saw a 1
          vref = 0;
        }

        switch (bit) {
        case 0:
          rxOffset.Bits.saoffset0 = vref;
          break;
        case 1:
          rxOffset.Bits.saoffset1 = vref;
          break;
        case 2:
          rxOffset.Bits.saoffset2 = vref;
          break;
        case 3:
          rxOffset.Bits.saoffset3 = vref;
          break;
        } // switch

        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " %2d", vref));

        if (bit == 3) {

          for (dimm = 0; dimm < MAX_DIMM; dimm++) {
            if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

            rankList = GetRankNvList(host, socket, ch, dimm);
            for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
              if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

              rxOffsetTmp.Data = (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe];
              rxOffsetTmp.Bits.saoffset0 = 0;
              rxOffsetTmp.Bits.saoffset1 = 0;
              rxOffsetTmp.Bits.saoffset2 = 0;
              rxOffsetTmp.Bits.saoffset3 = 0;
              rxOffsetTmp.Data |= rxOffset.Data;
              (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe] = rxOffsetTmp.Data;
              MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG),
                                (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe]);
            } // rank loop
          } // dimm loop

          rxOffset.Data = 0;
        } // if bit == 3
      } // bit loop

      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n"));
    } // strobe loop

    // Propagate delay values (without a read command)
    dataControl0.Data            = (*channelNvList)[ch].dataControl0;
    dataControl0.Bits.readrfrd   = 1;
    dataControl0.Bits.readrfwr   = 0;
    dataControl0.Bits.readrfrank = 0;
    // SKX change for upper nibble
    for (strobe = 0; strobe < MAX_STROBE ; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
    } // strobe loop

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n"));

    for (strobe = 0; strobe < MAX_STROBE; strobe++) { // set both nibbles
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      // Clean up after test
      // Disable force Sense amp
      // restore original datacontrol values
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl4[strobe]);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl0);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl2[strobe]);
    } // strobe loop

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\n"));
#ifdef PRINT_FUNC
    MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "\nSenseAmpOffset Ends\n"));
#endif

    //
    // Restore Rtt Park
    //


    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList    = GetRankNvList(host, socket, ch, dimm);
      rankStruct  = GetRankStruct(host, socket, ch, dimm);

      for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
      } // rank loop
    } // dimm loop
  } // ch loop

  IO_Reset(host, socket);

#ifdef DEBUG_SENSEAMP
  } // loopCnt
#endif
#ifdef SERIAL_DBG_MSG
  //
  // End time
  //
  ReadTsc64 (&endTsc);
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SenseAmpOffset - %dms\n", TimeDiff (host, startTsc, endTsc, TDIFF_UNIT_MS)));
#endif // SERIAL_DBG_MSG

  return status;
}

#define GRP_MUL_NONE  0   // Map one rank to each group
#define GRP_MUL_2_1   1   // Map two ranks to each group (0,2 and 1,3 to each group on QR)
#define GRP_MUL_FULL  2   // Map all DIMM ranks to one group (Not supported)
#define PPDS_GROUP_X4 BIT15 // Indicate that this group is x4 vs. x8


/**

  This function applies RfOn setting to given channel.

  @param host    - Pointer to sysHost
  @param socket  - Socket
  @param ch      - Channel
  @param rfOn    - rfOn setting to apply

@retval UINT32 - previous value

**/
UINT32
SetRfOn (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT32    rfOn
)
{
  struct channelNvram                   (*channelNvList)[MAX_CH];
  UINT8                                 strobe;
  UINT32                                retVal;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT    dataControl0;

  channelNvList = GetChannelNvList(host, socket);

  dataControl0.Data = (*channelNvList)[ch].dataControl0;
  retVal = dataControl0.Bits.rfon;
  dataControl0.Bits.rfon = rfOn;
    //SKX change
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
    } // strobe loop

  return retVal;
} // SetRfOn

/**

  This function applies RfOn setting to given channel.

  @param host    - Pointer to sysHost
  @param socket  - Socket
  @param ch      - Channel

@retval none

**/

/**

  This function applies the given CKE override

  @param host    - Pointer to sysHost
  @param socket  - Socket
  @param ch      - Channel
  @param ckeMask    - CKE settings to apply

@retval none

**/
void
SetCkeOverride (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     ckeMask
)
{
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT  miscCKECtl;

  // Override CKE
  miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
  miscCKECtl.Bits.cke_override = ckeMask;
  miscCKECtl.Bits.cke_on = ckeMask;
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);

} // SetCkeOverride

UINT32 CheckDimmType(
    PSYSHOST host
    )
{
  return SUCCESS;
}

/**

 Determine if we train a given rank for a given group

  @param host    - Pointer to sysHost
  @param socket  - Socket
  @param ch      - Channel
  @param dimm    - Dimm
  @param rank    - Rank
  @param level   - DdrLevel or LrbufLevel
  @group         - Tx/RxVref, Tx/RxDq/Dqs, etc
  @retval        - 1 = skip, 0 = train

**/
UINT8
CheckSkipRankTrain(
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank,
  GSM_LT   level,
  GSM_GT   group
  )
{
  UINT8 status=0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  
  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);

  if (rank > 0) {
#ifdef  LRDIMM_SUPPORT
    // only train rank0 for frontside TXVREF on LRDIMMs
    if (host->var.mem.runningRmt == 0){
      if (IsLrdimmPresent(host, socket, ch, dimm) && (level == DdrLevel) && ((group == TxVref) || (group == RxVref) ||
      (group == TxDqDelay) || (group == RxDqsPDelay) || (group == RxDqsNDelay) || (group == RxDqsDelay))) status = 1;
    }
    // do not margin subranks for LRDIMM in either encoded mode or direct mapped
    // commented out for now, EV wants margins on backside for all ranks
    //if (IsLrdimmPresent(host, socket, ch, dimm) && (level == LrbufLevel) && ((rank > 1) || (host->var.mem.currentSubRank == 1))) status = 1;
#endif
    // 3DS, skip logical ranks
    if (((*channelNvList)[ch].encodedCSMode == 2) && (rank > 1)) status = 1;

    // only train rank0 for CMD training on RDIMMs
    if (host->nvram.mem.dimmTypePresent == RDIMM && CheckCMDGroup(group)
                && (!host->var.mem.runningRmt) && (level == DdrLevel)) status = 1;

  }

  // only do backside Cmd margining on Register Rev2
  if ((CheckCMDGroup(group) || CheckCTLGroup(group) || (group == CmdVref)) && (level == LrbufLevel)
    && (!IsDdr4RegisterRev2(host, socket, ch, dimm) && !(((*dimmNvList)[dimm].aepDimmPresent) && (host->setup.mem.enableNgnBcomMargining)))) status = 1;

  if (!IsLrdimmPresent(host, socket, ch, dimm) && (level == LrbufLevel) && (!CheckCMDGroup(group)) && (!CheckCTLGroup(group))) status = 1;
 // restoreMsg (host);
 // if (status) MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, "Skip = %d GROUP=%d \n", status, group));
 // disableAllMsg (host);
  return status;
}

/**

Routine Description:

  Checks to see if the TxDqDqs is too close to the zero encoding
  Adjusts TxDqDqs if this is the case for both directions

Arguments:

  host      - Pointer to sysHost
  socket    - current socket
  direction - DENORMALIZE (-1) or RENORMALIZE (1)

Returns:

**/

UINT32
WLPushOut(
  PSYSHOST  host,
  UINT8     socket
)

{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               adjust = 0;
  INT16               piDelay;
  UINT16              minDelay;
  UINT16              maxDelay;
  struct channelNvram (*channelNvList)[MAX_CH];
  CHIP_WL_CLEANUP_STRUCT  WLChipCleanUpStruct;
  INT16                   CRAddDelay[MAX_CH];

  // Output current direction  Revise output statement
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "WriteLeveling Pushout\n"));

  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  " BEFORE WL pushout values\n"));
#ifdef SERIAL_DBG_MSG
  DisplayResults(host, socket, TxDqsDelay);
  DisplayResults(host, socket, TxDqDelay);
#endif
  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    minDelay      = PITICKRANGE;
    maxDelay      = 0;
    adjust        = 0;

    // Get current CWL
    GetChipTimingData(host, socket, ch, &WLChipCleanUpStruct);
    MemSetLocal((UINT8 *)CRAddDelay, 0, sizeof(CRAddDelay));
    ReadChipWrADDDelays(host, socket, ch, CRAddDelay, &WLChipCleanUpStruct);

    // Read results from TxDqDqs
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        // If rank enabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY,
                         (INT16 *)&piDelay);
          UpdateMinMaxInt(piDelay, (INT16 *)&minDelay, (INT16 *)&maxDelay);

          GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_ONLY,
                         (INT16 *)&piDelay);
          UpdateMinMaxInt(piDelay, (INT16 *)&minDelay, (INT16 *)&maxDelay);
        } // strobe loop
      } // rank loop
    } // dimm loop

    piDelay       = 0;
    //CWL by 1 or 2 clock and TX PI's by 128 or 256
    if ((CRAddDelay[ch] > -2) && (maxDelay <= (PITICKRANGE-2*DCLKPITICKS-EXTRAPIMARGIN/2))) { //511-256=255
      CRAddDelay[ch] = CRAddDelay[ch] - 2;
      piDelay = 2*DCLKPITICKS;
      adjust = 1;
    } else if ((CRAddDelay[ch] > -3) && (maxDelay <= (PITICKRANGE-DCLKPITICKS-EXTRAPIMARGIN/2))) { //511-128 = 383
      CRAddDelay[ch] = CRAddDelay[ch] - 1;
      piDelay = DCLKPITICKS;
      adjust = 1;
    }

    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Pushout to CWL_ADJ=%d and TXDq/Dqs by +%d\n",CRAddDelay[ch],piDelay));
    if (adjust == 1){
      // move CWL_ADJ

      UpdateChipMCDelay(host, socket, ch, channelNvList, CRAddDelay, &WLChipCleanUpStruct);

      // Write TxDqDqs per strobe
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
          // If rank enabled
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          for(strobe = 0; strobe < MAX_STROBE; strobe++) {
            //
            // Skip if this is an ECC strobe when ECC is disabled
            //
            if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

            GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay);

            GetSetDataGroup(host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE,
                           (INT16 *)&piDelay);
          } // strobe loop
        } // rank loop
      } // dimm loop
    } // adjust True
  } // Ch loop

  IO_Reset (host, socket);

  return SUCCESS;
} // WLPushOut

/**

  When PXC is enabled, DDRIO TX DQ/DQS limit must be enforced.

  @param host         - Pointer to sysHost
  @param socket       - Socket
  @param ch           - Channel

@retval none

**/
void enforceTXDQDQSLimit(
                         PSYSHOST  host,
                         UINT8 socket,
                         UINT8 ch
                    )
{
  UINT8                     dimm;
  UINT8                     rank;
  UINT8                     strobe;
  UINT8                     adjustCWL = 0;
  INT8                      curCWL_ADJ;
  INT16                     txdqDelay;
  INT16                     txdqsDelay;
  INT16                     minDelay = PITICKRANGE;
  INT16                     maxDelay = 0;

  TCOTHP_MCDDC_CTL_STRUCT   tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT  tCOTHP2;

  struct dimmNvram          (*dimmNvList)[MAX_DIMM];
  struct channelNvram       (*channelNvList)[MAX_CH];

  dimmNvList = GetDimmNvList(host, socket, ch);
  channelNvList = GetChannelNvList(host, socket);

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
      for(strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // Skip if this is an ECC strobe when ECC is disabled
        //
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_CSR|GSM_READ_ONLY, (INT16 *)&txdqDelay);
        UpdateMinMaxInt(txdqDelay, &minDelay, &maxDelay);
        GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_CSR|GSM_READ_ONLY, (INT16 *)&txdqsDelay);
        UpdateMinMaxInt(txdqsDelay, &minDelay, &maxDelay);
        //
        // either txdqdelay or txdqsdelay is  < 1 LD; add 2 logic delay to all
        //
        if (((txdqDelay < 64)||(txdqsDelay < 64)) && (adjustCWL == 0)) {
          adjustCWL = 1;
        }
      } // strobe loop
    } // rank loop
  } // dimm loop

  if (adjustCWL == 1) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Enforced 2 logic delay in PXC\n"));

    // check if there is overflow for write, throw an error mesg
    if (maxDelay > (PITICKRANGE-DCLKPITICKS)) {
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Out of range!! maxDelay = 0x%x, maxLimit = 0x%x\n",  (maxDelay+DCLKPITICKS), PITICKRANGE));
    }

    // Get current CWL
    tCOTHP.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
    curCWL_ADJ = (INT8)tCOTHP.Bits.t_cwl_adj;
    tCOTHP2.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG);
    curCWL_ADJ = curCWL_ADJ - (INT8)tCOTHP2.Bits.t_cwl_adj_neg;

    // subtract 1 to adjust
    curCWL_ADJ -= 1;

    if (curCWL_ADJ < -3) {
      // throw an error mesg
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "t_cwl_adj_neg value overflows\n"));
    } else if ((curCWL_ADJ >= -3) && (curCWL_ADJ < 0)) {
      tCOTHP.Bits.t_cwl_adj = 0;
      tCOTHP2.Bits.t_cwl_adj_neg = ABS(curCWL_ADJ);
      MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, tCOTHP.Data);
      MemWritePciCfgEp (host, socket, ch, TCOTHP2_MCDDC_CTL_REG, tCOTHP2.Data);
    } else if (curCWL_ADJ >= 0) {
      tCOTHP.Bits.t_cwl_adj = curCWL_ADJ;
      MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, tCOTHP.Data);
    }
    (*channelNvList)[ch].tcothp = tCOTHP.Data;

    txdqDelay = txdqsDelay = DCLKPITICKS; // add 2 logic delay

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        for(strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          //
          // add 2 logic delay to all
          //
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, (INT16 *)&txdqDelay);
          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, (INT16 *)&txdqsDelay);
        } // strobe loop
      } // rank loop
    } // dimm loop
  } // adjustCWL
} //enforceTXDQDQSLimit

