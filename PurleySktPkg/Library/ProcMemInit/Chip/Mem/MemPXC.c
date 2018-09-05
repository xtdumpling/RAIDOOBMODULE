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
 *      This file contains PXC init, algo
 *
 ************************************************************************/
#include "SysFunc.h"
#include "XCCVictimAggressorTable.h"
#include "RcRegs.h"
#include "MemApiSkx.h"
#include "MemFuncChip.h"

typedef struct { 
  INT16         marginData[MAX_CH][MAX_RANK_CH][MAX_BITS];
} pxcMarginData;

//#define PXCTHRESHOLD 13  // this is just an initial value to start with, but later can be tuned based on EV feedback

#define PXCTHRESHOLD 30  // temp value for debug purpose

#define LATEPICLK_SKEW_DELTA 65
#define INSUFFICIENT_MARGIN -1
#define PXCMARGIN_IMPROVEMENT 2
#define PXC_ENABLE 1
#define PXC_DISABLE 0
#define DQ_LANE 0
#define VICTIM 1
#define AGGRESSOR_0 2
#define AGGRESSOR_1 3
#define VICTIM_STATUS 4   // status for victim 
#define AGGR0_STATUS 5   // status for aggressor 0
#define AGGR1_STATUS 6   // status for aggressor 1

#define STATUS_DISABLED 0x0
#define STATUS_VICTIM 0x1
#define STATUS_AGGRESSOR 0x2

#define VICTIM_BIT 1
#define AGGRESSOR_BIT 2
#define UNASSIGNED 0xFF
#define NOT_PROCESSED_BIT 0xAA

#define PXC_TARGET_RANK 0  // index 0 based value; rank should be 0 for A0 

#define PXC_DEBUG

// Local function prototypes
UINT8 checkIsVictim(UINT8 ch, UINT8 victimAggrBit, UINT8 startBit, struct vicAggr *vicAggrChkBit);
UINT8 checkIsVictimAggr(UINT8 ch, UINT8 victimAggrBit, UINT8 startBit, struct vicAggr *vicAggrChkBit);
void updateBitStatus(UINT8 ch, UINT8 bit, UINT8 startBit, UINT8 victimAggr, UINT8 overrideSts, struct vicAggr *vicAggrBitSts);
void sortVictimsMargins(UINT8 ch, UINT8 startBit, struct vicAggr *vicAggrSort);
void PXCProgramAggressors(PSYSHOST host, UINT8 socket, UINT8 ch, struct vicAggr *vicAggrProg);
void PXCAttackerSelect(PSYSHOST host, UINT8 socket, UINT8 ch);
void PXCSetup(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 PXCEnDisable);
void PXCAttackerSelect(PSYSHOST host, UINT8 socket, UINT8 ch);
void PXCProgramAggressors(PSYSHOST host, UINT8 socket, UINT8 ch, struct vicAggr *vicAggrProg);
UINT32 PXCTraining(PSYSHOST host);

/**

  This function implements the PXC algo.

  @param host            - Pointer to sysHost

 @retval SUCCESS

**/
UINT32 PXCTraining(
                   PSYSHOST  host
                   )
{
  UINT8               ch;
  UINT8               socket;
  UINT8               nibble;
  UINT8               encodedNibble;
  UINT8               checkThreshold = 0;
  UINT8               bit;
  UINT8               maxNibbleSkewBit = 0; 
  UINT8               maxBits;
  UINT8               b;
  UINT8               byte;
  UINT8               victimDq;
  UINT8               order, index;
  UINT8               dqBit = 0;
  UINT8               swapLPC = 0;
  UINT8               newVictimBit = 0;
  UINT8               chBitMask = 0x3F; // default enable all channels
  INT16               minBitDelay, minBitDelayNew;
  INT16               maxNibbleSkew;
  INT16               bitDelay;
  INT16               tempDiff;  
  INT16               bitDelayData[MAX_CH][MAX_RANK_CH][MAX_BITS];
  UINT32              patternLength;
  UINT32              assignment1Value = 0;
  UINT32              assignment0Value = 0;
  UINT32              attack0selN0Value = 0;
  UINT32              attack1selN0Value = 0;
                      // frequencies  800   1000   1066   1200   1333   1400   1600   1800   1866   2000   2133  2200  2400  2600  2666  2800  2933  3000  3200
                      // ps         19.53  15.62  14.64  13.02  11.72  11.16   9.76   8.68   8.36   7.81   7.32  7.10  6.51  6.00  5.85  5.58  5.32  5.20  4.88
  UINT8               bitDelay2ps[MAX_SUP_FREQ] = {20, 16, 15, 13, 12, 11, 10, 9, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5};

  pxcMarginData  pxcMargins;

  struct vicAggr vicAggrCurr;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct bitMargin    resultsBit;
  struct bitMask      filter;

  vicAggrCurr = vicAggrXCC;

  socket = host->var.mem.currentSocket;
  OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, PXC_TRAINING, host->var.mem.currentSocket));

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  //
  // Return if PXC step is disabled 
  //
  if (~host->memFlowsExt & MF_EXT_PXC_EN) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);
  patternLength = 64;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "START_PXC_TRAINING\n"));


  // Collect Tx Dq per bit margins. 


  // Fill Filter Structure and initialize margin results structure
  for (ch = 0; ch < MAX_CH; ch++) {
    for (bit = 0; bit < MAX_BITS; bit++) {
      resultsBit.bits[ch][PXC_TARGET_RANK][bit].n = 0;
      resultsBit.bits[ch][PXC_TARGET_RANK][bit].p = 0;
      filter.bits[ch][PXC_TARGET_RANK][bit/8] = 0;
    }
  } // Ch loop

  // Get per bit margins
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "PatternLength: %d\n", patternLength));
  GetMargins (host, socket, DdrLevel, TxDqDelay, MODE_XTALK, SCOPE_BIT, &filter, (void *)&resultsBit, patternLength,
              0, 0, (UINT8)~(1<< PXC_TARGET_RANK), WDB_BURST_LENGTH);

  // check whether all bits margin value is > threshold value, enable required channels along with programming default attackers
  //
  // Loop for each channel
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) {
      chBitMask &= ~(1 << ch); // use the chBitMask for disabled channels
      continue;      
    }
    // enforce TXDQ/DQS limits
    enforceTXDQDQSLimit(host, socket, ch);
    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, 0, PXC_TARGET_RANK, CHECK_MAPOUT)) continue;  // need to refine this
    //
    // need to make sure whether target rank is populated
    // 
    if ((*channelNvList)[ch].numRanks <= PXC_TARGET_RANK) {  
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, "PXC target rank is not present in populated ranks\n"));
    }

    for (bit = 0; bit < MAX_BITS; bit++) {
      // Only display ECC bits if ECC is enabled
      if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;

      // margins > 13(threshold value), disable PXC training
      if ( (resultsBit.bits[ch][PXC_TARGET_RANK][bit].p - resultsBit.bits[ch][PXC_TARGET_RANK][bit].n) > PXCTHRESHOLD ) {
        checkThreshold = 1; // till now all bits are > threshold value
        continue;
      } else {
        checkThreshold = 0;
        break;
      }
    } // bit loop
    
    // check per ch
    if (checkThreshold == 1) {
      chBitMask &= ~(1 << ch); // use the chBitMask for disabled channels
      checkThreshold = 0; // clear the flag
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, "Margin above threshold value. Disabling PXC for CH\n"));                  
    } else {  // enable PXC and program aggressors
      // enable PXC; set pidelta and send the data 2QClks early
      PXCSetup(host, socket, ch, PXC_ENABLE);  
      // set default values for attacker 0 & attacker 1
      PXCAttackerSelect(host, socket, ch); 
      // enable the ch, to use this in WA
      host->var.mem.pxcEnabled |= (1<<ch);  
    }
  } // ch loop

  // Only display ECC bits if ECC is enabled
  if (host->nvram.mem.eccEn == 0) {
    maxBits = MAX_BITS - 8;
  } else {
    maxBits = MAX_BITS;
  }
  // calculate margin and bit delay values
  //
  // Loop for each channel
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitMask & (1<<ch)) == 0) continue;
    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, 0, PXC_TARGET_RANK, CHECK_MAPOUT)) continue;

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, 
                  "Margin Before applying PXC\n"));

    for (bit = 0; bit < maxBits; bit++) {
      if ((bit == 0) || (bit == (maxBits / 2))) {
        for(b = bit; b < (bit + (maxBits / 2)); b++) {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "  %2d", b));
        } // bit loop
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n"));
      }
      pxcMargins.marginData[ch][PXC_TARGET_RANK][bit] = (resultsBit.bits[ch][PXC_TARGET_RANK][bit].p - resultsBit.bits[ch][PXC_TARGET_RANK][bit].n);
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    " %3d", pxcMargins.marginData[ch][PXC_TARGET_RANK][bit]));

      if ((bit == ((maxBits / 2) - 1)) || (bit == (maxBits - 1))) {
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n\n"));
      }
    } // bit loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, 
                  "Bit Delay\n"));
    for (nibble = 0; nibble < MAX_STROBE; nibble++) {
      if ((!host->nvram.mem.eccEn) && ((nibble == 8) || (nibble == 17))) continue;
      //
      // Skip if this is an ECC nibble when ECC is disabled
      // Note: unlike usual in training code, per-bit skew register are mapped linerly, so ECC bits are on the end
      //
      if ((!host->nvram.mem.eccEn) && (nibble > 15)) continue;
      if (nibble%2){
        encodedNibble = (nibble + 17)/2;
      } else {
        encodedNibble = nibble/2;
      }

      if (nibble == 9) {
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                      "\n\n"));
      }

      if ( (nibble == 0) || (nibble == 9)) {
        for (bit = 0; bit < ((MAX_STROBE/2) *4); bit++) {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                        "  %3d", ((nibble * 4) + bit)));
        } // bit loop
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                      "\n"));
      }

      for (bit = 0; bit < BITS_PER_NIBBLE; bit++) {
        GetSetDataGroup (host, socket, ch, 0, PXC_TARGET_RANK, encodedNibble, bit, DdrLevel, TxDqBitDelay, GSM_READ_CSR|GSM_READ_ONLY, (INT16 *)&bitDelay);
        bitDelayData[ch][PXC_TARGET_RANK][((nibble * 4) + bit)] = bitDelay;
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                      "  %3d", bitDelayData[ch][PXC_TARGET_RANK][((nibble * 4) + bit)]));
      } // bit loop
    } // nibble loop
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                  "\n\n"));      
  } // ch loop

  // copy margin and skew values(find out min and max skew values per nibble)
  //
  // Loop for each channel
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitMask & (1<<ch)) == 0) continue;
    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, 0, PXC_TARGET_RANK, CHECK_MAPOUT)) continue;
#ifdef PXC_DEBUG
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, 
                  "margin table\n"));
#endif //PXC_DEBUG
    for (bit = 0; bit < MAX_BITS; bit++) {
      // Only display ECC bits if ECC is enabled
      if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;
      vicAggrCurr.marginValue[ch][bit] = pxcMargins.marginData[ch][PXC_TARGET_RANK][bit];
#ifdef PXC_DEBUG
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    " %3d", vicAggrCurr.marginValue[ch][bit]));
#endif //PXC_DEBUG
      // init bitOrder(this is used in sort to distinguish whether bit is processed or not)
      vicAggrCurr.bitOrder[ch][bit] = NOT_PROCESSED_BIT;

      // fill the MT with default/disabled values
      vicAggrCurr.MT[ch][bit][DQ_LANE] = UNASSIGNED;    
      vicAggrCurr.MT[ch][bit][VICTIM] = UNASSIGNED;
      vicAggrCurr.MT[ch][bit][AGGRESSOR_0] = UNASSIGNED;
      vicAggrCurr.MT[ch][bit][AGGRESSOR_1] = UNASSIGNED;
      vicAggrCurr.MT[ch][bit][VICTIM_STATUS] = STATUS_DISABLED;  // 0x0  = disabled; 0x1 = victim; 0x2 = aggressor
      vicAggrCurr.MT[ch][bit][AGGR0_STATUS] = STATUS_DISABLED;  // 0x0  = disabled; 0x1 = victim; 0x2 = aggressor
      vicAggrCurr.MT[ch][bit][AGGR1_STATUS] = STATUS_DISABLED;  // 0x0  = disabled; 0x1 = victim; 0x2 = aggressor
    } // bit loop
#ifdef PXC_DEBUG
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                  "\n"));
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, 
                  "skew table - LatePIClk denoted by ()\n"));
#endif //PXC_DEBUG
    for (bit = 0; bit < MAX_BITS; bit+=4) {
      // Only display ECC bits if ECC is enabled
      if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;
      // find the min bit delay
      minBitDelay = bitDelayData[ch][PXC_TARGET_RANK][bit];
      for (nibble = bit; nibble < (bit+4); nibble++) {
        if (bitDelayData[ch][PXC_TARGET_RANK][nibble] < minBitDelay) {
          minBitDelay = bitDelayData[ch][PXC_TARGET_RANK][nibble];
        }
      } // nibble loop
      // find the max skew value per nibble
      maxNibbleSkew = (UINT8)(bitDelayData[ch][PXC_TARGET_RANK][bit] - minBitDelay);
      maxNibbleSkewBit = bit;
      for (nibble = bit; nibble < (bit+4); nibble++) {
        vicAggrCurr.skewValue[ch][nibble] = (bitDelayData[ch][PXC_TARGET_RANK][nibble] - minBitDelay);
        if (vicAggrCurr.skewValue[ch][nibble] > maxNibbleSkew) {
          maxNibbleSkew = vicAggrCurr.skewValue[ch][nibble];
          maxNibbleSkewBit = nibble;
        }
      } // nibble loop
      // store the maxNibbleSkew per nibble and latepiclk bit
      vicAggrCurr.nibbleMaxSkew[ch][bit/4][0] = maxNibbleSkewBit; // latepiclk bit 
      vicAggrCurr.nibbleMaxSkew[ch][bit/4][1] = maxNibbleSkew;
#ifdef PXC_DEBUG
      for (nibble = bit; nibble < (bit+4); nibble++) {
        if (nibble == maxNibbleSkewBit) {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " (%2d)", maxNibbleSkew));
        } else {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, " %3d", vicAggrCurr.skewValue[ch][nibble]));
        }
      }
#endif //PXC_DEBUG

    } // bit loop
#ifdef PXC_DEBUG        
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                  "\n"));
#endif //PXC_DEBUG
  } // ch loop

  // process each byte vector
  //
  // Loop for each channel
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitMask & (1<<ch)) == 0) continue;
    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, 0, PXC_TARGET_RANK, CHECK_MAPOUT)) continue;  
#ifdef PXC_DEBUG
    MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\nOrder and analyse by margin\n"));
#endif //PXC_DEBUG
    for (bit = 0; bit < MAX_BITS; bit+=8) {
#ifdef PXC_DEBUG
#ifdef SERIAL_DBG_MSG
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT,
                  "byte vector:       DQ Lane     Victim     status     skew     skew_delta     aggressor0     status     aggressor1     status     margin\n"));
      PrintLine(host, 145, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
#endif //SERIAL_DBG_MSG
#endif //PXC_DEBUG
      // Only display ECC bits if ECC is enabled
      if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;

      sortVictimsMargins(ch, bit, &vicAggrCurr);

      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                    "\n"));
      order = 0;
      for (index = 0; index < 8; index++) {
        for (byte = bit; byte < (bit+8); byte++) {
          // delta btw 2 latepiclk bits > 65 or done processing byte vector
          tempDiff = (vicAggrCurr.nibbleMaxSkew[ch][bit/4][1] > vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][1]? (vicAggrCurr.nibbleMaxSkew[ch][bit/4][1]- vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][1]):(vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][1]-vicAggrCurr.nibbleMaxSkew[ch][bit/4][1]));
          if ( (tempDiff * bitDelay2ps[host->nvram.mem.socket[socket].ddrFreq] > LATEPICLK_SKEW_DELTA) || (order == 8)) {
            break; // exit byte loop
            break; // exit index loop
          }

          if (vicAggrCurr.bitOrder[ch][byte] == order) {
            order++; // point to next order
            // mark the victim bit
            victimDq = byte;
            vicAggrCurr.MT[ch][byte][DQ_LANE] = byte;  // DQ lane 

#ifdef PXC_DEBUG
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "byte vector before rules:     %3d         %3d     %10s    %3d         %3d             %3d    %10s         %3d    %10s      %3d", vicAggrCurr.MT[ch][byte][DQ_LANE], (vicAggrCurr.MT[ch][byte][VICTIM]== UNASSIGNED? 0 : vicAggrCurr.MT[ch][byte][VICTIM]), ((vicAggrCurr.MT[ch][byte][VICTIM_STATUS]  == STATUS_DISABLED)? "Disabled": ((vicAggrCurr.MT[ch][byte][VICTIM_STATUS] == STATUS_VICTIM)?"Victim":"Aggressor")),
                          vicAggrCurr.skewValue[ch][byte], tempDiff, (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] == UNASSIGNED? vicAggrCurr.vicAggrLookup[ch][byte][0]:vicAggrCurr.MT[ch][byte][AGGRESSOR_0]), ((vicAggrCurr.MT[ch][byte][AGGR0_STATUS] == STATUS_DISABLED)? "Disabled": ((vicAggrCurr.MT[ch][byte][AGGR0_STATUS] == STATUS_VICTIM)?"Victim":"Aggressor")),  
                          (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] == UNASSIGNED? vicAggrCurr.vicAggrLookup[ch][byte][1]:vicAggrCurr.MT[ch][byte][AGGRESSOR_1]), ((vicAggrCurr.MT[ch][byte][AGGR1_STATUS] == STATUS_DISABLED)? "Disabled": ((vicAggrCurr.MT[ch][byte][AGGR1_STATUS] == STATUS_VICTIM)?"Victim":"Aggressor")), vicAggrCurr.marginValue[ch][byte]));
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                          "\n"));
#endif //PXC_DEBUG

			if (checkIsVictimAggr(ch, victimDq, bit, &vicAggrCurr) == SUCCESS) {
              continue;
			} 
	        // For each aggressor, if the aggressor is already assigned as a victim from a previous iteration, then bypass all remaining steps for that aggres-sor.            
            if ( (victimDq == vicAggrCurr.vicAggrLookup[ch][byte][0]) || (victimDq == vicAggrCurr.vicAggrLookup[ch][byte][1])) {
              continue;
            } // else we have bit to be processed

            // victim is latepiclk bit, use the latepiclk bit as primary aggressor and select primary aggressor frm lookup table as victim
            if (victimDq == vicAggrCurr.nibbleMaxSkew[ch][bit/4][0]) {
              // swap primary aggressor and potential victim
              newVictimBit = vicAggrCurr.vicAggrLookup[ch][victimDq][0];	

              // check newVictimBit is already a victim or aggressor in the MT table
              if (checkIsVictimAggr(ch, newVictimBit, bit, &vicAggrCurr) == SUCCESS) {
                newVictimBit = vicAggrCurr.vicAggrLookup[ch][victimDq][1];
              }

              // the aggressor being swapped is also the LatePiClk bit, then select the other aggressor from the table to be the victim. 
              if (newVictimBit == vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][0]) {
                newVictimBit = vicAggrCurr.vicAggrLookup[ch][victimDq][1];
              }

              if (vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] == UNASSIGNED) {
                vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] = victimDq;  // latepiclk bit as primary aggressor
                vicAggrCurr.MT[ch][newVictimBit][VICTIM] = newVictimBit; // update the victim
              }
              if (vicAggrCurr.MT[ch][victimDq][AGGRESSOR_0] == UNASSIGNED) {
                vicAggrCurr.MT[ch][victimDq][AGGRESSOR_0] = victimDq; 
                vicAggrCurr.MT[ch][victimDq][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                vicAggrCurr.MT[ch][victimDq][VICTIM] = newVictimBit; // new victim bit
              }
              updateBitStatus(ch, newVictimBit, bit, VICTIM_BIT, 0, &vicAggrCurr); // update table for victim
              updateBitStatus(ch, victimDq, bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for aggr

              // swap was performed b/w new victim bit and latepiclk
              swapLPC = 1;

              //If the victim has a single aggressor (i.e., both aggressors in the table are the same value), and the aggressor is also the LatePiClk bit, then do not assign the aggressor. 
              if (vicAggrCurr.vicAggrLookup[ch][victimDq][0] == vicAggrCurr.vicAggrLookup[ch][victimDq][1]) {
                if (vicAggrCurr.vicAggrLookup[ch][victimDq][0] == vicAggrCurr.nibbleMaxSkew[ch][bit/4][0]) {
				  // do not assign the aggressor
                }
              }
            } else if (victimDq == vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][0]) {
              // swap primary aggressor and potential victim
              newVictimBit = vicAggrCurr.vicAggrLookup[ch][victimDq][0];	

              // check newVictimBit is already a victim or aggressor in the MT table
              if (checkIsVictimAggr(ch, newVictimBit, bit, &vicAggrCurr) == SUCCESS) {
                newVictimBit = vicAggrCurr.vicAggrLookup[ch][victimDq][1];
              }

              // the aggressor being swapped is also the LatePiClk bit, then select the other aggressor from the table to be the victim. 
              if (newVictimBit == vicAggrCurr.nibbleMaxSkew[ch][bit/4][0]) {
                newVictimBit = vicAggrCurr.vicAggrLookup[ch][victimDq][1];
              }

              if (vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] == UNASSIGNED) {
                vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] = victimDq;  // latepiclk bit as primary aggressor
                vicAggrCurr.MT[ch][newVictimBit][VICTIM] = newVictimBit; // update the victim
              }  
              if (vicAggrCurr.MT[ch][victimDq][AGGRESSOR_0] == UNASSIGNED) {
                vicAggrCurr.MT[ch][victimDq][AGGRESSOR_0] = victimDq;
                vicAggrCurr.MT[ch][victimDq][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                vicAggrCurr.MT[ch][victimDq][VICTIM] = newVictimBit; // new victim bit
              }
              updateBitStatus(ch, newVictimBit, bit, VICTIM_BIT, 0, &vicAggrCurr); // update table for victim
              updateBitStatus(ch, victimDq, bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for aggr

              // swap was performed b/w new victim bit and latepiclk
              swapLPC = 1;

              // the victim has a single aggressor (i.e., both aggressors in the table are the same value), and the aggressor is also the LatePiClk bit, then do not assign the aggressor. 
              if (vicAggrCurr.vicAggrLookup[ch][victimDq][0] == vicAggrCurr.vicAggrLookup[ch][victimDq][1]) {
                if (vicAggrCurr.vicAggrLookup[ch][victimDq][0] == vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][0]) {
                  // do not assign the aggressor
                }
              }
            }
            // compute victim is how many PI ticks earlier than wrt latepiclk in nibble 0   // may be need to check the swapLPC here, as the newVictimBit is different from victimDq
            if (victimDq < (bit + 4)) {
              tempDiff = ((vicAggrCurr.skewValue[ch][byte] > vicAggrCurr.nibbleMaxSkew[ch][bit/4][1])? (vicAggrCurr.skewValue[ch][byte]-vicAggrCurr.nibbleMaxSkew[ch][bit/4][1]): (vicAggrCurr.nibbleMaxSkew[ch][bit/4][1]-vicAggrCurr.skewValue[ch][byte]));
              if (tempDiff > 6) {
                // do not assign the aggressor
              } else if ((tempDiff > 4) && (tempDiff <= 6)) {
                // enable aggressor 0
                if (swapLPC == 1) {
                  // do not enable aggressor as the first/aggr 0 is enabled
                } else {
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = byte;	// assign itself							
                  } else {                         
                    if (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] == UNASSIGNED) {
                      vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0];	// enable first/aggr 0							
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                      updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                    }
                  }
                }
              } else if (tempDiff <= 4) {
                // enable both aggressor 0 & aggressor 1
                if (swapLPC ==1) {
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1] = byte;
                  } else {
                    if (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] == UNASSIGNED) {
                      vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1];  // enable aggressor 1 as the first /aggr 0 is enabled while swapping
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                      updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                    }
                  }
                } else {
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = byte;
                  } else {
                    if (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] == UNASSIGNED) {
                      vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0];  
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                      updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                    }
                  }
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][byte][AGGRESSOR_1] = byte;
                  } else {
                    if (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] == UNASSIGNED) {
                      vicAggrCurr.MT[ch][byte][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1];  
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                      updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                    }
                  }
                }
              }
            } else { // compute victim is how many PI ticks earlier than wrt latepiclk in nibble 1
              tempDiff = ((vicAggrCurr.skewValue[ch][byte] > vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][1])? (vicAggrCurr.skewValue[ch][byte]-vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][1]): (vicAggrCurr.nibbleMaxSkew[ch][(bit/4)+1][1]-vicAggrCurr.skewValue[ch][byte]));                  
              if (tempDiff > 6) {
                // do not assign the aggressor
              } else if ((tempDiff > 4) && (tempDiff <= 6)) {
                // enable aggressor 0
                if (swapLPC == 1) {
                  // do not enable aggressor as the first/aggr 0 is enabled
                } else {
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = byte;
                  } else {
                    if (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] == UNASSIGNED) {
                      vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0];	// enable first/aggr 0							
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                      updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                    }
                  }
                }
              } else if (tempDiff <= 4) {
				// enable both aggressor 0 & aggressor 1
                if (swapLPC == 1) {
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1] = byte;
                  } else { 
                    if (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] == UNASSIGNED) {
                      vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1];  // enable aggressor 1 as the first /aggr 0 is enabled while swapping
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                      updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                    }
                  }
                } else {
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = byte;
                  } else { 
                      if (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] == UNASSIGNED) {
                        vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0];  
                        vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                        vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                        vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                        vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][0]; 
                        vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][0]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                        updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][0], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                      }
                    }
                  if (checkIsVictim(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, &vicAggrCurr) == SUCCESS) {
                    vicAggrCurr.MT[ch][byte][AGGRESSOR_1] = byte;
                  } else {
                    if (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] == UNASSIGNED) {
                      vicAggrCurr.MT[ch][byte][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1];  
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][VICTIM_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR0_STATUS] = STATUS_AGGRESSOR; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGRESSOR_1] = vicAggrCurr.vicAggrLookup[ch][byte][1]; 
                      vicAggrCurr.MT[ch][vicAggrCurr.vicAggrLookup[ch][byte][1]][AGGR1_STATUS] = STATUS_AGGRESSOR; 
                      updateBitStatus(ch, vicAggrCurr.vicAggrLookup[ch][byte][1], bit, AGGRESSOR_BIT, 0, &vicAggrCurr); // update table for enabled aggr on other bits
                    }
                  }
                }
              } //
            } // else

            // aggressor enabled above has a skew to victim of > 8 PI ticks, disable that aggressor by setting it back to its initial value. 
            if (swapLPC == 1) {
              if (vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] != UNASSIGNED) {  // aggressor 0
                if ( ((vicAggrCurr.skewValue[ch][newVictimBit] > vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0]])? (vicAggrCurr.skewValue[ch][newVictimBit] - vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0]]):(vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0]]-vicAggrCurr.skewValue[ch][newVictimBit])) > 8) {
                  vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] = UNASSIGNED; // set to initial value
                }
              }						
              if (vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1] != UNASSIGNED) {  // aggressor 1
                if ( ((vicAggrCurr.skewValue[ch][newVictimBit] > vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1]])? (vicAggrCurr.skewValue[ch][newVictimBit] - vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1]]):(vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1]]-vicAggrCurr.skewValue[ch][newVictimBit])) > 8) {
                  vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_1] = UNASSIGNED; // set to initial value
                }
              }						
            } else {
              if (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] != UNASSIGNED) { // aggressor 0
                if ( ((vicAggrCurr.skewValue[ch][byte] > vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][byte][AGGRESSOR_0]])? (vicAggrCurr.skewValue[ch][byte] - vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][byte][AGGRESSOR_0]]):(vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][byte][AGGRESSOR_0]]-vicAggrCurr.skewValue[ch][byte])) > 8) {
                  vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = UNASSIGNED; // set to initial value
                }
              }
              if (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] != UNASSIGNED) { // aggressor 1
                if ( ((vicAggrCurr.skewValue[ch][byte] > vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][byte][AGGRESSOR_1]])? (vicAggrCurr.skewValue[ch][byte] - vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][byte][AGGRESSOR_1]]):(vicAggrCurr.skewValue[ch][vicAggrCurr.MT[ch][byte][AGGRESSOR_1]]-vicAggrCurr.skewValue[ch][byte])) > 8) {
                  vicAggrCurr.MT[ch][byte][AGGRESSOR_1] = UNASSIGNED; // set to initial value
                }
              }
            }

            // only one aggressor in the lookup table is duplicated, enable only primary aggressor
            if (swapLPC == 1) {
              if (vicAggrCurr.vicAggrLookup[ch][newVictimBit][0] == vicAggrCurr.vicAggrLookup[ch][newVictimBit][1]) {
                if (vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] == UNASSIGNED) {
                  vicAggrCurr.MT[ch][newVictimBit][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][newVictimBit][0]; // enable primary aggressor
                }
              }
            } else {
              if (vicAggrCurr.vicAggrLookup[ch][byte][0] == vicAggrCurr.vicAggrLookup[ch][byte][1]) {
                if (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] == UNASSIGNED) {
                  vicAggrCurr.MT[ch][byte][AGGRESSOR_0] = vicAggrCurr.vicAggrLookup[ch][byte][0] ; // enable primary aggressor
                }
              }
            }

#ifdef PXC_DEBUG
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "byte vector after rules:      %3d         %3d     %10s    %3d         %3d             %3d    %10s         %3d    %10s      %3d", vicAggrCurr.MT[ch][byte][DQ_LANE], (swapLPC == 1? newVictimBit: vicAggrCurr.MT[ch][byte][DQ_LANE]), ((vicAggrCurr.MT[ch][byte][VICTIM_STATUS] == STATUS_DISABLED)? "Disabled": ((vicAggrCurr.MT[ch][byte][VICTIM_STATUS] == STATUS_VICTIM)?"Victim":"Aggressor")),
                          vicAggrCurr.skewValue[ch][byte], tempDiff, (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] == UNASSIGNED? vicAggrCurr.vicAggrLookup[ch][byte][0]:vicAggrCurr.MT[ch][byte][AGGRESSOR_0]), ((vicAggrCurr.MT[ch][byte][AGGR0_STATUS] == STATUS_DISABLED)? "Disabled": ((vicAggrCurr.MT[ch][byte][AGGR0_STATUS] == STATUS_VICTIM)?"Victim":"Aggressor")),  
                          (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] == UNASSIGNED? vicAggrCurr.vicAggrLookup[ch][byte][1]:vicAggrCurr.MT[ch][byte][AGGRESSOR_1]), ((vicAggrCurr.MT[ch][byte][AGGR1_STATUS] == STATUS_DISABLED)? "Disabled": ((vicAggrCurr.MT[ch][byte][AGGR1_STATUS] == STATUS_VICTIM)?"Victim":"Aggressor")), vicAggrCurr.marginValue[ch][byte]));
            MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                          "\n"));
#endif //PXC_DEBUG
            if (swapLPC == 1) {
              // once an aggressor is enabled duing swap, both of its assignment 0 and assignment 1 should be same value(update for aggr1, as aggr0 is already updated while swapping)
              vicAggrCurr.MT[ch][victimDq][AGGRESSOR_1] = victimDq; 
              vicAggrCurr.MT[ch][victimDq][AGGR1_STATUS] = STATUS_AGGRESSOR; 
              // clear swapLPC flag
              swapLPC = 0;
            }

          } // order
        } // byte
      } // index

      // print the final results before programming attacker(0/1) select
#ifdef SERIAL_DBG_MSG
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                    "\n"));
      MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT,
                    "byte vector results:     DQ_Victim     Assignment 0     Assignment 1\n"));                    
      PrintLine(host, 78, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
#endif //SERIAL_DBG_MSG
      for (byte = bit; byte < (bit+8); byte++) {
        if (vicAggrCurr.MT[ch][byte][AGGRESSOR_0] != UNASSIGNED) {
          assignment0Value = vicAggrCurr.MT[ch][byte][AGGRESSOR_0];
        } else {
          attack0selN0Value =  MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte/8, ATTACK0SELN0_0_MCIO_DDRIO_REG));
          dqBit = (byte%8);
          assignment0Value = (((attack0selN0Value & (dqBit << (3 * dqBit))) >> (3*dqBit)) );
          if (assignment0Value <= 7) {
            assignment0Value += bit;
          }
        }
        if (vicAggrCurr.MT[ch][byte][AGGRESSOR_1] != UNASSIGNED) {
          assignment1Value = vicAggrCurr.MT[ch][byte][AGGRESSOR_1];
        } else {
          attack1selN0Value = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte/8, ATTACK1SELN0_0_MCIO_DDRIO_REG));
          dqBit = (byte%8);
          assignment1Value = (((attack1selN0Value & (dqBit << (3 * dqBit))) >> (3*dqBit)) );				
          if (assignment1Value <= 7) {
            assignment1Value += bit;
          }
        }

        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "                                     %3d           %3d              %3d", vicAggrCurr.MT[ch][byte][DQ_LANE], assignment0Value, assignment1Value));
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                      "\n"));
      } // byte loop
    } // bit loop
  } // ch loop
  
  //
  // program the aggressors 
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitMask & (1<<ch)) == 0) continue;
    // set aggressors which are enabled for attacker 0 & attacker 1
    PXCProgramAggressors(host, socket, ch, &vicAggrCurr); 
  } // ch loop

  // Collect Tx Dq per bit margins. 

  // Fill Filter Structure and initialize margin results structure
  for (ch = 0; ch < MAX_CH; ch++) {
    for (bit = 0; bit < MAX_BITS; bit++) {
      resultsBit.bits[ch][PXC_TARGET_RANK][bit].n = 0;
      resultsBit.bits[ch][PXC_TARGET_RANK][bit].p = 0;
      filter.bits[ch][PXC_TARGET_RANK][bit/8] = 0;
    }
  } // Ch loop
  // Get per bit margins
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "PatternLength: %d\n", patternLength));
  GetMargins (host, socket, DdrLevel, TxDqDelay, MODE_XTALK, SCOPE_BIT, &filter, (void *)&resultsBit, patternLength,
              0, 0, (UINT8)~(1<< PXC_TARGET_RANK), WDB_BURST_LENGTH);

  // calculate margin after running PXC
  //
  // Loop for each channel
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitMask & (1<<ch)) == 0) continue;
    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, 0, PXC_TARGET_RANK, CHECK_MAPOUT)) continue;

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, 
                   "Margin After applying PXC\n"));

    for (bit = 0; bit < maxBits; bit++) {
	  // Only display ECC bits if ECC is enabled
      if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;

      if ((bit == 0) || (bit == (maxBits / 2))) {
        for(b = bit; b < (bit + (maxBits / 2)); b++) {
          MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "  %2d", b));
        }
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n"));
      } // bit loop

      pxcMargins.marginData[ch][PXC_TARGET_RANK][bit] = (resultsBit.bits[ch][PXC_TARGET_RANK][bit].p - resultsBit.bits[ch][PXC_TARGET_RANK][bit].n);
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     " %3d", pxcMargins.marginData[ch][PXC_TARGET_RANK][bit]));

      if ((bit == ((maxBits / 2) - 1)) || (bit == (maxBits - 1))) {
        MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "\n\n"));
      }
    } // bit loop
  } // ch loop

  // compare with original margins (before PXC) and new margins (after PXC) to determine whether to disable PXC or leave it enabled
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((chBitMask & (1<<ch)) == 0) continue;
    //
    // Skip if no rank
    //
    if (CheckRank(host, socket, ch, 0, PXC_TARGET_RANK, CHECK_MAPOUT)) continue;

    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, 
                   "margin improvement after applying PXC\n"));

    minBitDelayNew = pxcMargins.marginData[ch][PXC_TARGET_RANK][0];
    minBitDelay = vicAggrCurr.marginValue[ch][0];

    for (bit = 1; bit < MAX_BITS; bit++) {
      // Only display ECC bits if ECC is enabled
      if ((host->nvram.mem.eccEn == 0) && (bit > 63)) continue;

      if (pxcMargins.marginData[ch][PXC_TARGET_RANK][bit] < minBitDelayNew ) {
        minBitDelayNew = pxcMargins.marginData[ch][PXC_TARGET_RANK][bit];
      }

      if (vicAggrCurr.marginValue[ch][bit] < minBitDelay ) {
        minBitDelay = vicAggrCurr.marginValue[ch][bit];
      }
    } // bit loop

	tempDiff = ((minBitDelayNew > minBitDelay)? (minBitDelayNew - minBitDelay): INSUFFICIENT_MARGIN);
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT,
                    "original value: %3d; new value: %3d; diff value: %3d", minBitDelay, minBitDelayNew, (minBitDelayNew-minBitDelay)));
    if (tempDiff > PXCMARGIN_IMPROVEMENT) {
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, "Margin Improvement Check: PXC Training Enabled\n"));
      continue;
    } else if ((tempDiff == INSUFFICIENT_MARGIN) || (tempDiff <= PXCMARGIN_IMPROVEMENT)) {
      // check whether there is margin improvement, no improvement by 2 ticks disable the pxc on that channel
      MemDebugPrint((host, SDBG_MEM_TRAIN, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n"));
            
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, PXC_TARGET_RANK, NO_STROBE, NO_BIT, "Margin Improvement Check: PXC Training Disabled\n"));
      PXCSetup(host, socket, ch, PXC_DISABLE);  // disable PXC  
      // mark pxc is disabled
      host->var.mem.pxcEnabled &= ~(1<<ch);
    } 
  } // ch loop
  

  return SUCCESS;
}  //PXCTraining


// chip level functions
/**

  This function applies a new PXC setup values(enables or disables the PXC training).

  @param host       - Pointer to sysHost
  @param socket     - Socket
  @param ch         - Channel
  @param earlyWData - send the data 2QClks early by setting to 0
  @param PXCValue   - Enable/disable the PXC 

@retval none

**/
void PXCSetup(
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     PXCEnDisable
             )
{
  UINT8                                 strobe;
  struct channelNvram                   (*channelNvList)[MAX_CH];
  DATACONTROL1N0_0_MCIO_DDRIO_STRUCT    dataControl1;
  MCWDB_CHKN_BIT_MCDDC_DP_STRUCT        disDdrioEarly;

  channelNvList = GetChannelNvList(host, socket);

  disDdrioEarly.Data = MemReadPciCfgEp (host, socket, ch, MCWDB_CHKN_BIT_MCDDC_DP_REG);

  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    //
    // Skip if this is an ECC strobe when ECC is disabled
    //
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

    //read cached values
    dataControl1.Data = (*channelNvList)[ch].dataControl1[strobe];

    // enable/disable PXC
    if (PXCEnDisable == 1) {
      dataControl1.Bits.xtalkpienable = 1;
    } else if (PXCEnDisable == 0) {
      dataControl1.Bits.xtalkpienable = 0;
    }

    // set delta to 2
    dataControl1.Bits.xtalkpidelta = 0x2;

    // set pisign to 1
    dataControl1.Bits.xtalkpisign = 1;

    //write new values
    (*channelNvList)[ch].dataControl1[strobe] =  dataControl1.Data;
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL1N0_0_MCIO_DDRIO_REG), dataControl1.Data);
  } //strobe

  //send the data 2QClks early by setting to 0
  if (PXCEnDisable == 1) {
    disDdrioEarly.Bits.dis_ddrio_earlywdata = 0;
  } else if (PXCEnDisable == 0) {
    disDdrioEarly.Bits.dis_ddrio_earlywdata = 1;  // disable by setting to 1(default)    
  }

  MemWritePciCfgEp (host, socket, ch, MCWDB_CHKN_BIT_MCDDC_DP_REG, disDdrioEarly.Data);
} //PXCSetup

/**

  This function sets default attackers for both attacker0select and attacker1select.

  @param host       - Pointer to sysHost
  @param socket     - Socket
  @param ch         - Channel

@retval none

**/
void PXCAttackerSelect(
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch
             )
{
  UINT8                                 dqBit;
  UINT8                                 byte;
  ATTACK0SELN0_0_MCIO_DDRIO_STRUCT      attackr0SelectN0;
  ATTACK1SELN0_0_MCIO_DDRIO_STRUCT      attackr1SelectN0;
  ATTACK0SELN1_0_MCIO_DDRIO_STRUCT      attackr0SelectN1;
  ATTACK1SELN1_0_MCIO_DDRIO_STRUCT      attackr1SelectN1;

#ifdef PXC_DEBUG
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                  "Program default values for ATTACK0/1SELNO/N1\n"));
#endif //PXC_DEBUG
  // program default values for attackr0 and attackr1
  for (byte = 0; byte < (MAX_STROBE/2); byte++) {
    attackr0SelectN0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN0_0_MCIO_DDRIO_REG));
    attackr1SelectN0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN0_0_MCIO_DDRIO_REG));
    attackr0SelectN1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN1_0_MCIO_DDRIO_REG));
    attackr1SelectN1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN1_0_MCIO_DDRIO_REG));

    for (dqBit = 0; dqBit <= 7; dqBit++) {
      // program same value for both Nibble 0 and Nibble 1
      attackr0SelectN0.Data |= (dqBit << (3*dqBit));
      attackr1SelectN0.Data |= (dqBit << (3*dqBit));
      attackr0SelectN1.Data |= (dqBit << (3*dqBit));
      attackr1SelectN1.Data |= (dqBit << (3*dqBit));
    }

    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN0_0_MCIO_DDRIO_REG), attackr0SelectN0.Data);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN0_0_MCIO_DDRIO_REG), attackr1SelectN0.Data);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN1_0_MCIO_DDRIO_REG), attackr0SelectN1.Data);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN1_0_MCIO_DDRIO_REG), attackr1SelectN1.Data);
  }
} //PXCAttackerSelect

/**

  This function sets aggressors which are enabled for both attacker0select and attacker1select.

  @param host         - Pointer to sysHost
  @param socket       - Socket
  @param ch           - Channel
  @param vicAggrProg  - Pointer to vicAggr structure

@retval none

**/
void PXCProgramAggressors(
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              struct vicAggr *vicAggrProg
             )
{
  UINT8                                 dqBit;
  UINT8                                 byte;
  ATTACK0SELN0_0_MCIO_DDRIO_STRUCT      attackr0SelectN0;
  ATTACK1SELN0_0_MCIO_DDRIO_STRUCT      attackr1SelectN0;
  ATTACK0SELN1_0_MCIO_DDRIO_STRUCT      attackr0SelectN1;
  ATTACK1SELN1_0_MCIO_DDRIO_STRUCT      attackr1SelectN1;

#ifdef PXC_DEBUG
    MemDebugPrint((host, SDBG_MEM_TRAIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
                  "Program assignment values for ATTACK0/1SELNO/N1\n"));
#endif //PXC_DEBUG

  // program values for attackr0 and attackr1
  for (byte = 0; byte < (MAX_STROBE/2); byte++) {
    // read the current values of attackr0 and attackr1
	attackr0SelectN0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN0_0_MCIO_DDRIO_REG));
    attackr1SelectN0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN0_0_MCIO_DDRIO_REG));
    attackr0SelectN1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN1_0_MCIO_DDRIO_REG));
    attackr1SelectN1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN1_0_MCIO_DDRIO_REG));

    for (dqBit = 0; dqBit <= 7; dqBit++) {
      // program same value for both Nibble 0 and Nibble 1
      if (vicAggrProg->MT[ch][(byte*8)+dqBit][AGGRESSOR_0] != UNASSIGNED) {
        attackr0SelectN0.Data &= ~(0x7 << (3*dqBit));
        attackr0SelectN0.Data |= ((vicAggrProg->MT[ch][byte*8+dqBit][AGGRESSOR_0]%8) << (3*dqBit));
        attackr0SelectN1.Data &= ~(0x7 << (3*dqBit));
        attackr0SelectN1.Data |= ((vicAggrProg->MT[ch][byte*8+dqBit][AGGRESSOR_0]%8) << (3*dqBit));
      }    
      if (vicAggrProg->MT[ch][(byte*8)+dqBit][AGGRESSOR_1] != UNASSIGNED) {
        attackr1SelectN0.Data &= ~(0x7 << (3*dqBit));
        attackr1SelectN0.Data |= ((vicAggrProg->MT[ch][byte*8+dqBit][AGGRESSOR_1]%8) << (3*dqBit));
        attackr1SelectN1.Data &= ~(0x7 << (3*dqBit));
        attackr1SelectN1.Data |= ((vicAggrProg->MT[ch][byte*8+dqBit][AGGRESSOR_1]%8) << (3*dqBit));
      }
    } // dqBit

    // program new set of values for attackr0 and attackr1 
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN0_0_MCIO_DDRIO_REG), attackr0SelectN0.Data);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN0_0_MCIO_DDRIO_REG), attackr1SelectN0.Data);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK0SELN1_0_MCIO_DDRIO_REG), attackr0SelectN1.Data);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, byte, ATTACK1SELN1_0_MCIO_DDRIO_REG), attackr1SelectN1.Data);
  } //byte
} //PXCProgramAggressors

/**

  This function sorts margin values and marks order to process them in ascending order.

  @param ch              - Channel
  @param startBit        - start bit position
  @param vicAggrSort     - structure contains all details about victim aggressor table

 @retval none

**/
void 
sortVictimsMargins(
                   UINT8 ch,
                   UINT8 startBit, 
                   struct vicAggr *vicAggrSort 
                   ) 
{
  UINT8          bit;
  UINT8          loop;
  UINT8          order;
  INT16          minMargin;
  UINT8          minNibble = 0;

  order = 0;
	
  for (loop = 0; loop < 8; loop++) {
    minMargin = 0xFF;
    for (bit = startBit; bit < (startBit+8); bit++) {
      if (vicAggrSort->bitOrder[ch][bit] != NOT_PROCESSED_BIT) continue;

      if (vicAggrSort->marginValue[ch][bit] < minMargin) {
        minMargin = vicAggrSort->marginValue[ch][bit];
        minNibble = bit;
      }
    }
    if (minNibble >= MAX_BITS) {
      continue;
    }
    vicAggrSort->bitOrder[ch][minNibble] = order++; 
  }
}  //sortVictimsMargins

/**

  This function updates bit(can be either victim/aggressor) status of entire lookup table.

  @param ch              - Channel
  @param bit             - bit who's status needs to be updated
  @param startBit        - start bit position
  @param victimAggr      - whether bit is a victim or aggressor
  @param overrideSts     - to override the existing status (default set to not override(0-disable)) 
  @param vicAggrBitSts   - structure contains all details about victim aggressor table

 @retval none

**/
void 
updateBitStatus(
                   UINT8 ch,
                   UINT8 bit,
                   UINT8 startBit,
                   UINT8 victimAggr, 
                   UINT8 overrideSts,
                   struct vicAggr *vicAggrBitSts
                   ) 
{
  UINT8            index;

  for (index = startBit; index < (startBit+8); index++)
  {
    if (victimAggr == 1) { // victim bit
      if (bit == vicAggrBitSts->vicAggrLookup[ch][index][0]) { // victim column
        if ( (vicAggrBitSts->MT[ch][index][VICTIM] == UNASSIGNED) || (overrideSts == 1)) {
          vicAggrBitSts->MT[ch][index][VICTIM] = bit;
        }
        if (vicAggrBitSts->MT[ch][index][VICTIM_STATUS] == STATUS_DISABLED) { // victim status
          vicAggrBitSts->MT[ch][index][VICTIM_STATUS] = STATUS_VICTIM;  // update the disabled->victim status 
        }
      }
      if (bit == vicAggrBitSts->vicAggrLookup[ch][index][0]) { // aggr 0 column
        if ( (vicAggrBitSts->MT[ch][index][AGGRESSOR_0] == UNASSIGNED) || (overrideSts == 1)) {
          vicAggrBitSts->MT[ch][index][AGGRESSOR_0] = bit;
        }
        if (vicAggrBitSts->MT[ch][index][AGGR0_STATUS] == STATUS_DISABLED) { // aggr0 status
          vicAggrBitSts->MT[ch][index][AGGR0_STATUS] = STATUS_VICTIM;  // update the disabled->victim status 
        }
      }
      if (bit == vicAggrBitSts->vicAggrLookup[ch][index][1]) { // aggr 1 column
        if ((vicAggrBitSts->MT[ch][index][AGGRESSOR_1] == UNASSIGNED) || (overrideSts == 1)) { 
          vicAggrBitSts->MT[ch][index][AGGRESSOR_1] = bit;
        }
        if (vicAggrBitSts->MT[ch][index][AGGR1_STATUS] == STATUS_DISABLED) { // aggr1 status
          vicAggrBitSts->MT[ch][index][AGGR1_STATUS] = STATUS_VICTIM;  // update the disabled->victim status 
        }
      }
    } else if (victimAggr == 2) { // aggressor bit
      if (bit == vicAggrBitSts->vicAggrLookup[ch][index][0]) { // aggr 0 column
        if ((vicAggrBitSts->MT[ch][index][AGGRESSOR_0] == UNASSIGNED) || (overrideSts == 1)) {
          vicAggrBitSts->MT[ch][index][AGGRESSOR_0] = bit;
        }
        if (vicAggrBitSts->MT[ch][index][AGGR0_STATUS] == STATUS_DISABLED) { // aggr0 status
          vicAggrBitSts->MT[ch][index][AGGR0_STATUS] = STATUS_AGGRESSOR;  // update the disabled->aggr0 status 
        }
      }
      if (bit == vicAggrBitSts->vicAggrLookup[ch][index][1]) { // aggr 1 column
        if ((vicAggrBitSts->MT[ch][index][AGGRESSOR_1] == UNASSIGNED) || (overrideSts == 1)) {
          vicAggrBitSts->MT[ch][index][AGGRESSOR_1] = bit;
        }
        if (vicAggrBitSts->MT[ch][index][AGGR1_STATUS] == STATUS_DISABLED) { // aggr1 status
          vicAggrBitSts->MT[ch][index][AGGR1_STATUS] = STATUS_AGGRESSOR;  // update the disabled->aggr1 status 
        }
      }		
    }
  } // index
} // updateBitStatus

/**

  This function checks whether bit passed is already a victim or aggressor from the table.

  @param ch              - Channel
  @param victimAggrBit   - whether bit is a victim or aggressor
  @param startBit        - start bit position
  @param vicAggrChkBit   - structure contains all details about victim aggressor table

 @retval SUCCESS/FAILURE

**/
UINT8 
checkIsVictimAggr(
                  UINT8 ch,
                  UINT8 victimAggrBit,
                  UINT8 startBit,
                  struct vicAggr *vicAggrChkBit
                 ) 
{
  UINT8            index;
  UINT8            foundBitFlag = 0;

  for (index = startBit; index < (startBit+8); index++)
  {
    // If the victim bit was an aggressor in a previous iteration, it cannot be as-signed as a victim.  Move to the next bit in the byte vector. 
    if ( (victimAggrBit == vicAggrChkBit->MT[ch][index][AGGRESSOR_0]) || (victimAggrBit == vicAggrChkBit->MT[ch][index][AGGRESSOR_1]) ) {
      foundBitFlag = 1;
    }
    // If the victim bit was a victim in a previous iteration, it cannot be reevaluated as a victim. Move to next bit in the byte vector. 
    if (victimAggrBit == vicAggrChkBit->MT[ch][index][VICTIM]) {
      foundBitFlag = 1;
    }
  } // index loop

  if (foundBitFlag == 1) {
    return SUCCESS;
  } else {
	return FAILURE;
  }
} //checkIsVictimAggr

/**

  This function checks whether bit passed is already a victim from the table.

  @param ch              - Channel
  @param victimAggrBit   - whether bit is a victim
  @param startBit        - start bit position
  @param victimAggressor - structure contains all details about victim aggressor table

 @retval SUCCESS/FAILURE

**/
UINT8 
checkIsVictim(
                  UINT8 ch,
                  UINT8 victimAggrBit,
                  UINT8 startBit,
                  struct vicAggr *vicAggrChkBit
                 ) 
{
  UINT8            index;

  for (index = startBit; index < (startBit+8); index++)
  {
    // If the victim bit was a victim in a previous iteration, it cannot be reevaluated as a victim. Move to next bit in the byte vector. 
    if (victimAggrBit == vicAggrChkBit->MT[ch][index][VICTIM]) {
      return SUCCESS;
    }
  } // index loop

  return FAILURE;
} //checkIsVictim








