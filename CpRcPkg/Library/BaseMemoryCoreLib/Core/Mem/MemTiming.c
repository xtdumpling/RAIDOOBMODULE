//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.02
//      Bug Fix:    Change Z11C MICRON DIMM detection rule.
//      Reason:     DIE REV "I" is Z11C DIMM, on die ECC DIMM
//      Auditor:    Kasber Chen
//      Date:       JUN/12/2017
//
//  Rev. 1.01
//      Bug Fix:    Detect MICRON, tCCD_L >= 0x2b and enable tCCD_L workaround
//      Reason:     Refer Grantley SVN_4980
//      Auditor:    Kasber Chen
//      Date:       JUN/03/2017
//
//  Rev. 1.00
//      Bug Fix:    Add tCCD_L Relaxation setup option for specific DIMMs.
//      Reason:     Requested by Micron, code reference from Jian.
//      Auditor:    Jacker Yeh
//      Date:       Mar/17/2017
//
//****************************************************************************
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
 *      IMC and DDR4 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"

                                      //   800   1000   1066   1200   1333   1400   1600   1800   1866   2000   2133  2200  2400  2600  2666  2800  2933  3000  3200  3400  3467  3600  3733  3800  4000  4200  4266  4400
const UINT32  preciseTCK[MAX_SUP_FREQ] = {25000, 20000, 18750, 16667, 15000, 14286, 12500, 11111, 10714, 10000, 9375, 9091, 8333, 7692, 7500, 7143, 6818, 6667, 6250, 5883, 5769, 5556, 5358, 5264, 5000, 4762, 4689, 4546};
const UINT8   tWL_DDR3[MAX_SUP_FREQ]  = { 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11};
const UINT8   tWL_DDR4[MAX_SUP_FREQ]  = { 9, 9, 9, 9, 9, 11, 11, 11, 12, 12, 14, 14, 16, 16, 18, 18, 18, 18, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
const UINT8   tMRD_DDR3               = 1;
const UINT8   tMRD_DDR4               = 4;
const UINT8   tPARRECOVERY[MAX_SUP_FREQ] = { 0, 0, 0, 0, 104, 105, 105, 122, 122, 138, 139, 156, 157, 157, 157, 158, 158, 158, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159};
const UINT8   maxRefAllowed[MAX_DENSITY] = { 12, 9, 5, 3 };
const UINT8   PiTicks  = 64;

//
// Local Prototypes
//
UINT16 GetMaxClkDiffDD(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 mode);
UINT8  Cas2RecEnGap(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 mode);
UINT8  Cas2DrvEnGap(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 mode);
void   EffectiveBusDelay(PSYSHOST host, UINT8 socket, UINT8 ch, UINT32 *effBusDelaySR, UINT32 *effBusDelayDR, UINT32 *effBusDelayDD);
UINT8  CkDiffGap(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 mode);

void
RestoreTimings (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch
             )
/*++

  Restores trained timing values register

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number

  @retval N/A

--*/
{
  UINT8                                       dimm;
  UINT8                                       rank;
  UINT8                                       strobe;
  UINT8                                       ctlIndex;
  UINT8                                       clk;
  UINT8                                       bit;
  UINT8                                       maxBit = 4;
  INT16                                       zeroValue = 0;
  UINT16                                      cmdMinVal;
  UINT16                                      cmdMaxVal;
  UINT16                                      ctlMinVal;
  UINT16                                      ctlMaxVal;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
#ifdef LRDIMM_SUPPORT
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
#endif

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Restore Timings\n"));

  CHIP_FUNC_CALL(host, SetRestoreTimings(host, socket, ch));

  //
  // Restore CMD timings
  //
  CHIP_FUNC_CALL(host, GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_FORCE_WRITE + GSM_WRITE_OFFSET, &zeroValue, &cmdMinVal, &cmdMaxVal));

  //
  // Restore CTL timings
  //
  for (ctlIndex = 0; ctlIndex < NUM_CTL_PLATFORM_GROUPS; ctlIndex++) {
    CHIP_FUNC_CALL(host, GetSetCtlGroupDelay (host, socket, ch, ctlIndex + CtlGrp0, GSM_FORCE_WRITE + GSM_WRITE_OFFSET, &zeroValue, &ctlMinVal, &ctlMaxVal));
  }

  //
  // Restore CLK timings
  //
  for (clk = 0; clk < 4; clk++) {
    CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, clk, GSM_FORCE_WRITE + GSM_WRITE_OFFSET, &zeroValue));
  }

  //
  // Restore all timings on fast boot.
  //
  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
#ifdef LRDIMM_SUPPORT
    rankStruct  = GetRankStruct(host, socket, ch, dimm);
#endif
    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // Skip if this is an ECC strobe when ECC is disabled
        //
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        CHIP_FUNC_CALL(host, GetSetTxDelay (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET,
                       &zeroValue));
        CHIP_FUNC_CALL(host, GetSetRxDelay (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqsDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET,
                       &zeroValue));
        CHIP_FUNC_CALL(host, GetSetRxVref (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxVref, GSM_FORCE_WRITE | GSM_WRITE_OFFSET,
                      &zeroValue));
        for (bit = 0; bit < maxBit; bit++) {
          if (DESKEW_DELAY == RxDqsBitDelay) {
            CHIP_FUNC_CALL(host, GetSetRxDelayBit (host, socket, ch, dimm, rank, strobe, bit, DdrLevel, RxDqsBitDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET,
              &zeroValue));
          } else {
            CHIP_FUNC_CALL(host, GetSetRxDelayBit (host, socket, ch, dimm, rank, strobe, bit, DdrLevel, RxDqsPBitDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET,
              &zeroValue));
            CHIP_FUNC_CALL(host, GetSetRxDelayBit (host, socket, ch, dimm, rank, strobe, bit, DdrLevel, RxDqsNBitDelay, GSM_FORCE_WRITE | GSM_WRITE_OFFSET,
              &zeroValue));
          }
        }
        CHIP_FUNC_CALL(host, CheckTxVrefSupport(host, socket, ch, dimm, rank, strobe, &zeroValue));
        CHIP_FUNC_CALL(host, WriteFastBootTimings(host, socket, ch, dimm, rank, strobe));
#ifdef LRDIMM_SUPPORT
        // restore LRDIMM cache
        if (IsLrdimmPresent(host, socket, ch, dimm)){
          (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBC2x3x[strobe];
          (*rankStruct)[rank].cachedLrBuf_FxBC4x5x[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBC4x5x[strobe];
          (*rankStruct)[rank].cachedLrBuf_FxBC8x9x[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe];
          (*rankStruct)[rank].cachedLrBuf_FxBCAxBx[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBCAxBx[strobe];
          (*rankStruct)[rank].lrbufTxVrefCache[strobe] = (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrbufTxVref[strobe];
          if (strobe < MAX_STROBE/2) {
            (*rankStruct)[rank].cachedLrBuf_FxBCCxEx[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBCCxEx[strobe];
            (*rankStruct)[rank].cachedLrBuf_FxBCDxFx[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBCDxFx[strobe];
            (*rankStruct)[rank].lrbufRxVrefCache[strobe] = (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrbufRxVref[strobe];
          }
        }
#endif
      } // strobe loop
    } // rank loop
  } // dimm loop

  //
  // Other Timing Parameters
  //
  CHIP_FUNC_CALL(host, RestoreTimingsChip (host, socket, ch));
}

#if SMCPKG_SUPPORT
UINT8
SpdGetTccd_L(
PSYSHOST  host,
UINT8     socket,
UINT8     ch
)
/*++

This function returns the value of tCCD from SPD

@param host    - Point to sysHost
@param socket  - Socket Id
@param ch      - Channel number

@retval UINT32 Value of tCCD

--*/
{
  UINT32              tCCDLocal;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  tCCDLocal = (*channelNvList)[ch].common.nCCD_L;

  CheckBounds(host, tCCD, &tCCDLocal);
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "tCCD Relaxion Enabled -> SpdGetTccd_L : %02xh\n",tCCDLocal ));
  return  (UINT8) tCCDLocal;
} // SpdGetTccd_L
#endif

UINT8
GetTccd_L(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
  )
/*++

  Gets tCCD

  @param host    - Point to sysHost
  @param socket  - Socket numer

  @retval tCCD

--*/
{
  UINT32  tCCDLocal = 4;
  UINT16  tCCDTime;
  struct channelNvram(*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
#if SMCPKG_SUPPORT
  if(host->setup.mem.tCCDLRelaxation){
      for(tCCDTime = 0; tCCDTime < MAX_DIMM; tCCDTime++){
          if((*channelNvList)[ch].dimmList[tCCDTime].SPDDramMfgId == MFGID_MICRON){
              MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "MICRON DIE REV = %x.\n", (*channelNvList)[ch].dimmList[tCCDTime].SPDDramRev));
              if((*channelNvList)[ch].dimmList[tCCDTime].SPDDramRev == 0x49)  //DIE REV = "I"
                  return SpdGetTccd_L(host, socket, ch);
          }
      }
  }
#endif
  if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
    // Enforce tCCD_L based on freq
    if (host->nvram.mem.socket[socket].ddrFreqMHz <= 800) {
      tCCDTime = 62500; // 6.25 ns
    } else if (host->nvram.mem.socket[socket].ddrFreqMHz < 1200) {
      tCCDTime = 53550; // 5.355 ns
    } else {
      tCCDTime = 50000; // 5 ns
    }
    tCCDLocal = (UINT8)((tCCDTime + (GettCK(host, socket) - 100)) / GettCK(host, socket));
  }

  tCCDLocal = tCCDLocal + (*channelNvList)[ch].tCCDAdder;
  CheckBounds(host, tCCD, &tCCDLocal);

  return (UINT8)tCCDLocal;

} // GetTccd_L


void
GetTTVar (
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
         )
/*++

  Gets the variables needed to calculate turnaround times

  @param host    - Point to sysHost
  @param socket  - Socket numer
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval N/A

--*/
{
  UINT32                        effBusDelaySR;
  UINT32                        effBusDelayDR;
  UINT32                        effBusDelayDD;
  ODT_TIMING                    odtTiming;


  //
  // Get the effective bus delays for same rank (SR), different rank same DIMM (DR) and different DIMM (DD)
  //
  EffectiveBusDelay(host, socket, ch, &effBusDelaySR, &effBusDelayDR, &effBusDelayDD);
  (*ttVars).effBusDelaySR = (UINT8)effBusDelaySR;
  (*ttVars).effBusDelayDR = (UINT8)effBusDelayDR;
  (*ttVars).effBusDelayDD = (UINT8)effBusDelayDD;

  (*ttVars).cas2RecEnDR = Cas2RecEnGap(host, socket, ch, DR_MODE);
  (*ttVars).cas2RecEnDD = Cas2RecEnGap(host, socket, ch, DD_MODE);

  (*ttVars).cas2DrvEnDR = Cas2DrvEnGap(host, socket, ch, DR_MODE);
  (*ttVars).cas2DrvEnDD = Cas2DrvEnGap(host, socket, ch, DD_MODE);

  CHIP_FUNC_CALL(host, GetOdtTiming(host, socket, ch, &odtTiming));
  (*ttVars).odtRdLeading  = odtTiming.odtRdLeading;
  (*ttVars).odtRdTrailing = odtTiming.odtRdTrailing;
  (*ttVars).odtWrLeading  = odtTiming.odtWrLeading;
  (*ttVars).odtWrTrailing = odtTiming.odtWrTrailing;

  (*ttVars).clkDiffDR = CkDiffGap(host, socket, ch, DR_MODE);
  (*ttVars).clkDiffDD = CkDiffGap(host, socket, ch, DD_MODE);
} // GetTTVar

UINT8
CalctWWDR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT8     minWWDR,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the write to write turnaround time for different ranks on the same DIMM

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tWWDR

--*/
{
  UINT8               tWWDR;
  CHIP_FUNC_CALL(host, DramSpecCalctWWDR(host, socket, ch, ttVars, &tWWDR));

  //
  // Check for min WWDR
  //
  if (tWWDR < minWWDR) {
    tWWDR = minWWDR;
  }

  CHIP_FUNC_CALL(host, EnforceWWDR(host, ch, &tWWDR));
  if (host->setup.mem.optionsExt & WR_CRC) { tWWDR += 1; }

  if (tWWDR < 3) {
    tWWDR = 3;
  }

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "tWWDR = %d, minWWDR = %d\n", tWWDR, minWWDR));

  return tWWDR;
} // CalctWWDR

UINT8
CalctWWDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT8     minWWDD,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the write to write turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tWWDD

--*/
{
  UINT8 tWWDD = 0;
  tWWDD = (*ttVars).cas2DrvEnDD;

  CHIP_FUNC_CALL(host, DramSpecCalctWWDD(host, socket, ch, ttVars, &tWWDD));

  //
  // Check for min WWDD
  //
  if (tWWDD < minWWDD) {
    tWWDD = minWWDD;
  }

  CHIP_FUNC_CALL(host, EnforceWWDD(host, ch, &tWWDD));
  if (host->setup.mem.optionsExt & WR_CRC) { tWWDD += 1; }

  if (tWWDD < 3) {
    tWWDD = 3;
  }

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "tWWDD = %d, minWWDD = %d\n", tWWDD, minWWDD));
  return tWWDD;
} // CalctWWDD

UINT8
CalctRWSR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT32    specMin,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the read to write turnaround time for the same rank

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval rRWSR

--*/
{
  UINT8                                 tRWSR;
  struct channelNvram                   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  tRWSR = (*ttVars).effBusDelaySR;

  CHIP_FUNC_CALL(host, DramSpecCalctRWSR(host, socket, ch, ttVars, &tRWSR));

  if (((*channelNvList)[ch].lrDimmPresent) && (tRWSR < specMin)) {
    tRWSR = (UINT8)specMin;
  }
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "specMin = %d, tRWSR = %d\n", specMin, tRWSR));
  return tRWSR;
} // CalctRWSR

UINT8
CalctRWDR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT32    specMin,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the read to write turnaround time for different ranks on the same DIMM

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tRWDR

--*/
{
  UINT8                                 tRWDR;
  struct channelNvram                   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  tRWDR = (*ttVars).effBusDelayDR;

  if (((*ttVars).odtRdTrailing > 1) || ((*ttVars).odtWrLeading > 1)) {
    tRWDR += 1;
  }

  CHIP_FUNC_CALL(host, DramSpecCalctRWDR(host, socket, ch, ttVars, &tRWDR));

  if (((*channelNvList)[ch].lrDimmPresent) && (tRWDR < specMin)) {
    tRWDR = (UINT8)specMin;
  }
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "specMin = %d, tRWDR = %d\n", specMin, tRWDR));

  return tRWDR;
} // CalctRWDR

UINT8
CalctRWDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the read to write turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tRWDD

--*/
{
  UINT8                                 tRWDD;

  tRWDD = (*ttVars).effBusDelayDD;

  CHIP_FUNC_CALL(host, DramSpecCalctRWDD(host, socket, ch, ttVars, &tRWDD));

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 " tRWDD = %d\n", tRWDD));

  return tRWDD;
} // CalctRWDD

UINT8
CalctWRDR(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the write to read turnaround time for different ranks on the same DIMM

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tWRDR

--*/
{
  UINT8 tWRDR;

  CHIP_FUNC_CALL(host, DramSpecCalctWRDR(host, socket, ch, ttVars, &tWRDR));
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "tWRDR = %d\n", tWRDR));

  return tWRDR;
} // CalctWRDR

UINT8
CalctWRDD(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         TT_VARS   *ttVars
         )
/*++

  This function calculates the write to read turnaround time for different DIMMs

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param ttVars  - Structure that holds the variables

  @retval tWRDD

--*/
{
  UINT8 tWRDD;

  CHIP_FUNC_CALL(host, DramSpecCalctWRDD(host, socket, ch, ttVars, &tWRDD));

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "tWRDD = %d\n", tWRDD));

  return tWRDD;
} // CalctWRDD

/*++

  CAS2RCVEN: The time from sending CAS command @ IO till RCV_EN is asserted per this command.
  It should be calculated per byte as
  CAS2RCVEN(rank, byte) = 1/2 * (round_trip_latency(rank) + RxRcvEnLogicDelay(rank, byte)
        - IO_latency(rank) + RcvPIsetting(rank, byte) / 64) - const
  The const is the latency from pads to MC. It doesn't affect the further calculations because these will be the differences
  of CAS2RCVEN of different ranks same byte.
  The gap between two data transfers is determined by two limits - drive period and ODT period. The limiting factor is
  different for different rank and for different DIMM.
  R2RDR_GAP: On different ranks same DIMM the ODT value doesn't change in the turnaround, so the limiters are derived from
  need to keep non-overlap between the two drivers. For this purpose 0.6 DCLK cycle is enough. The final equation is
            tRRDR-D0 = max(Byte = 0 to 7, ECC) |CAS2RCVEN(R0, byte) - CAS2RCVEN(R1, byte)| + 0.25
            tRRDR-D2 = max(Byte = 0 to 7, ECC) |CAS2RCVEN(R2, byte) - CAS2RCVEN(R3, byte)| + 0.25
            tRRDR = round-up (max (tRRDR-D0, tRRDR-D2)) -1

  @param host    - Point to sysHost
  @param socket  - Socket number
  @param ch      - Channel number
  @param Mode    - Timing Mode:
                     SR_MODE
                     DR_MODE
                     DD_MODE

  @retval  gap

--*/
UINT8
Cas2RecEnGap(
            PSYSHOST  host,
            UINT8     socket,
            UINT8     ch,
            UINT8     mode
            )
{
  UINT8               dimm;
  UINT8               rank;
  UINT8               logRank;
  UINT8               strobe;
  INT16               recEnPi;
  UINT16              cas2Rcven;
  UINT16              maxGap = 0;
  UINT16              strobeMin[MAX_STROBE];
  UINT16              strobeMax[MAX_STROBE];
  UINT16              gap;
  UINT16              gapDimm = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  gap = 0;

  if (mode == DD_MODE) {
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      strobeMin[strobe] = 0xFFF;
      strobeMax[strobe] = 0;
    } // strobe loop
  }


  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    if (mode == DR_MODE) {
      gapDimm = 0;

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        strobeMin[strobe] = 0xFFF;
        strobeMax[strobe] = 0;
      } // strobe loop
    }

#ifdef DEBUG_TURNAROUNDS
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\nCAS To Receive Enable Data %d DIMM %d\n", ch, dimm));
    if (mode == DD_MODE) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Different DIMM\n"));
    } else {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Different Rank\n"));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "           | rank 0 | rank 1 | rank 2 | rank 3\n", dimm));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "-----------------------------------------------\n"));
#endif

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;


#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Strobe %2d:", strobe));
#endif

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY, &recEnPi));
        cas2Rcven = (CHIP_FUNC_CALL(host, GetRoundTrip (host, socket, ch, logRank)) * MAX_PHASE_IN_FINE_ADJUSTMENT) -
                    (CHIP_FUNC_CALL(host, GetIOLatency (host, socket, ch, logRank)) * MAX_PHASE_IN_FINE_ADJUSTMENT) + recEnPi;

#ifdef DEBUG_TURNAROUNDS
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " |  %3d ", cas2Rcven));
#endif

        CHIP_FUNC_CALL(host, UpdateMinMax(cas2Rcven, &strobeMin[strobe], &strobeMax[strobe]));
      } // rank loop

#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n"));
#endif

    } // strobe loop

    if (mode == DR_MODE) {

      maxGap = 0;
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if (maxGap < (strobeMax[strobe] - strobeMin[strobe])) {
          maxGap = strobeMax[strobe] - strobeMin[strobe];
        }
      } // strobe loop

      //
      // An extra DCLK is required if the diff is > 39 ticks
      //
      if (maxGap >= 39) {
        gapDimm = maxGap - 39;
      } else {
        gapDimm = 0;
      }

      //
      // Account for separation between commands (convert pi ticks to DCLK)
      //
      if ((gapDimm % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) > 0) {
        gapDimm = (gapDimm / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + 1;
      } else {
        gapDimm = gapDimm / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2);
      }
    }


#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                     "gapDimm = %d\n", gapDimm));
#endif

      if (gap < gapDimm) gap = gapDimm;
  } // dimm loop

  if (mode == DD_MODE) {
    //
    // Account for separation between commands (convert pi ticks to DCLK)
    //
    maxGap = 0;
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if (maxGap < (strobeMax[strobe] - strobeMin[strobe])) {
        maxGap = strobeMax[strobe] - strobeMin[strobe];
      }
    } // strobe loop

    CHIP_FUNC_CALL(host, CalcFinalGap(&gap, maxGap, gapDimm));
  }


#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                     "cas2RecEnGap = %d\n", gap));
#endif

  return(UINT8)gap;
} // Cas2RecEnGap

UINT8
Cas2DrvEnGap(
            PSYSHOST  host,
            UINT8     socket,
            UINT8     ch,
            UINT8     mode
            )
{
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  INT16               piDelay;
  UINT16              gap = 0;
  UINT16              gapDimm = 0;
  UINT16              cas2DrvEn;
  UINT16              maxGap = 0;
  UINT16              strobeMin[MAX_STROBE];
  UINT16              strobeMax[MAX_STROBE];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  if (mode == DD_MODE) {
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      strobeMin[strobe] = 0xFFF;
      strobeMax[strobe] = 0;
    } // strobe loop
  }

  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

#ifdef DEBUG_TURNAROUNDS
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\nCAS To Drive Enable Data %d DIMM %d\n", ch, dimm));
    if (mode == DD_MODE) {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Different DIMM\n"));
    } else {
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "Different Rank\n"));
    }
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "           | rank 0 | rank 1 | rank 2 | rank 3\n", dimm));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "-----------------------------------------------\n"));
#endif

    if (mode == DR_MODE) {
      gapDimm = 0;

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        strobeMin[strobe] = 0xFFF;
        strobeMax[strobe] = 0;
      } // strobe loop
    }


    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "Strobe %2d:", strobe));
#endif

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, &piDelay));
        cas2DrvEn = piDelay;

#ifdef DEBUG_TURNAROUNDS
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " |  %3d ", cas2DrvEn));
#endif

        CHIP_FUNC_CALL(host, UpdateMinMax(cas2DrvEn, &strobeMin[strobe], &strobeMax[strobe]));
      } // rank loop

#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\n"));
#endif

    } // strobe loop
    if (mode == DR_MODE) {

      maxGap = 0;
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if (maxGap < (strobeMax[strobe] - strobeMin[strobe])) {
          maxGap = strobeMax[strobe] - strobeMin[strobe];
        }
      } // strobe loop

      if (maxGap >= MEM_CHIP_POLICY_VALUE(host, cas2DrvenMaxGap)) {
        gapDimm = maxGap - MEM_CHIP_POLICY_VALUE(host, cas2DrvenMaxGap);
      } else {
        gapDimm = 0;
      }

      //
      // Account for separation between commands (convert pi ticks to DCLK)
      //
      if ((gapDimm % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) > 0) {
        gapDimm = (gapDimm / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + 1;
      } else {
        gapDimm = gapDimm / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2);
      }
    }

#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                     "gapDimm = %d\n", gapDimm));
#endif

      if (gap < gapDimm) gap = gapDimm;
  } // dimm loop

  if (mode == DD_MODE) {
    maxGap = 0;
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if (maxGap < (strobeMax[strobe] - strobeMin[strobe])) {
        maxGap = strobeMax[strobe] - strobeMin[strobe];
      }
    } // strobe loop

    if (maxGap >= MEM_CHIP_POLICY_VALUE(host, cas2DrvenMaxGap)) {
      maxGap = maxGap - MEM_CHIP_POLICY_VALUE(host, cas2DrvenMaxGap);
    } else {
      maxGap = 0;
    }

    if ((maxGap % (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) > 0) {
      gap = (maxGap / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2)) + 1;
    } else {
      gap = maxGap / (MAX_PHASE_IN_FINE_ADJUSTMENT * 2);
    }

  }

#ifdef DEBUG_TURNAROUNDS
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "cas2DrvEnGap = %d\n", gap));
#endif

  return(UINT8)gap;
} // Cas2DrvEnGap

void
EffectiveBusDelay(
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 UINT32    *effBusDelaySR,
                 UINT32    *effBusDelayDR,
                 UINT32    *effBusDelayDD
                 )
/*++

  Effective Bus Delay

  @param host          - Point to sysHost
  @param socket        - Socket number
  @param ch            - Channel number
  @param effBusDelaySR - effective bus delay for same rank (SR)
  @param effBusDelayDR - effective bus delay for different rank
                         same DIMM (DR)
  @param effBusDelayDD - effective bus delay for different DIMM
                         (DD)

  @retval N/A

--*/
{
  UINT8                   dimm;
  UINT8                   rank;
  UINT8                   logRank;
  UINT8                   strobe;
  UINT8                   Nmode;
  INT16                   RcvEnPi;
  INT16                   TxDqsPi;
  UINT8                   preamble;
  INT16                   CwlAdj;
  UINT16                  IntDelta;
  UINT16                  RTL;
  UINT16                  CL;
  UINT32                  EffectiveBusRoundTrip[MAX_DIMM][MAX_RANK_CH][MAX_STROBE];
  UINT32                  EffBusRT = 0;
  UINT32                  TxDelay[MAX_DIMM][MAX_RANK_CH][MAX_STROBE];
  UINT32                  TxDelayTemp = 0;
  UINT32                  IOComp;
  UINT32                  IOlat;
  UINT32                  longPreambleEnable;
  struct channelNvram     (*channelNvList)[MAX_CH];
  struct dimmNvram        (*dimmNvList)[MAX_DIMM];
  struct ddrRank          (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  *effBusDelaySR = 0;
  *effBusDelayDR = 0;
  *effBusDelayDD = 0;

  CHIP_FUNC_CALL(host, EffectiveBusDelayInit(host, socket, ch, &CwlAdj, &IOComp, &IntDelta, &longPreambleEnable));

  preamble    =  (UINT8)(PiTicks * (longPreambleEnable + 1));
  CL          = ((*channelNvList)[ch].common.nCL) * PiTicks * 2;
  IOlat       = ((*channelNvList)[ch].ioLatency0);
  Nmode       = ((*channelNvList)[ch].timingMode);

  if (Nmode > 0) {
    Nmode = Nmode - 1;
  }
  Nmode = Nmode * 2 * PiTicks;

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {

    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    rankList = GetRankNvList(host, socket, ch, dimm);

    for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
      //
      // Get the logical rank #
      //
      logRank = GetLogicalRank(host, socket, ch, dimm, rank);

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe] = 0;
        TxDelay[dimm][(*rankList)[rank].rankIndex][strobe] = 0;
      } // strobe loop

      //
      // Get round trip latency for each channel
      //
      RTL = CHIP_FUNC_CALL(host, GetRoundTrip(host, socket, ch, logRank)) * PiTicks;

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // Skip if this is an ECC strobe when ECC is disabled
        //
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RecEnDelay, GSM_READ_ONLY, &RcvEnPi));
        CHIP_FUNC_CALL(host, GetSetDataGroup (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqsDelay, GSM_READ_ONLY, &TxDqsPi));
        EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe] = RTL - CL - (UINT16)IOComp - ((UINT16)PiTicks * ((IOlat >> (4 * logRank)) & 0xF)) - \
                                                                           Nmode + RcvEnPi + IntDelta + preamble;

#ifdef DEBUG_TURNAROUNDS
        //
        //  Debug Code Start
        //
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "**************************************************************\n", RTL));
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,"\n"));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " RTL - CL - IOComp - IOLat - Nmode + RcvEnPi + IntDelta + preamble\n"));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "    0x%X - 0x%X - 0x%X", RTL, CL, IOComp));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " - 0x%X",  ((UINT16)PiTicks*((IOlat >> (4*logRank)) & 0xF)) ));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " - 0x%X + 0x%X", Nmode, RcvEnPi));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " + 0x%X + 0x%X", IntDelta, preamble));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " = 0x%X\n", EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe]));
        //
        //  Debug Code End
        //
#endif //DEBUG_TURNAROUNDS

        TxDelay[dimm][(*rankList)[rank].rankIndex][strobe] = (UINT16)(TxDqsPi) + CwlAdj;// - (PiTicks / 2);

#ifdef DEBUG_TURNAROUNDS
        //
        //  Debug Code Start
        //
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " TxDqsPi + CwlAdj = TxDelay\n"));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "  0x%X", TxDqsPi));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " + 0x%X", CwlAdj));
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       " = 0x%X\n", TxDelay[dimm][(*rankList)[rank].rankIndex][strobe]));
        //
        //  Debug Code End
        //
#endif //DEBUG_TURNAROUNDS

      } // strobe loop
    } // rank loop
  } // dimm loop

  //
  // Calculate effective bus delay for same rank (SR)
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    rankList = GetRankNvList(host, socket, ch, dimm);

    for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        if ((EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe] -
             TxDelay[dimm][(*rankList)[rank].rankIndex][strobe]) > *effBusDelaySR) {

          *effBusDelaySR = EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe] -
                           TxDelay[dimm][(*rankList)[rank].rankIndex][strobe];

#ifdef DEBUG_TURNAROUNDS
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "EffBusRT %d, TxDelayTemp %d, effBusDelaySR = %d\n",
                         EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe],
                         TxDelay[dimm][(*rankList)[rank].rankIndex][strobe], *effBusDelaySR));
#endif

        }
      } // strobe loop
    } // rank loop
  } // dimm loop

  //
  // Calculate effective bus delay for different rank same DIMM (DR)
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      EffBusRT = 0;
      TxDelayTemp = 0xFFF;

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        if (EffBusRT < EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe]) {
          EffBusRT = EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe];
        }
        if (TxDelayTemp > TxDelay[dimm][(*rankList)[rank].rankIndex][strobe]) {
          TxDelayTemp = TxDelay[dimm][(*rankList)[rank].rankIndex][strobe];
        }

      } // rank loop

      if ((EffBusRT - TxDelayTemp) > *effBusDelayDR) {
        *effBusDelayDR = EffBusRT - TxDelayTemp;

#ifdef DEBUG_TURNAROUNDS
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, strobe, NO_BIT,
                       "EffBusRT %d, TxDelayTemp %d, effBusDelayDR = %d\n",
                       EffBusRT, TxDelayTemp, *effBusDelayDR));
#endif

      }
    } // strobe loop
  } // dimm loop

  //
  // Calculate effective bus delay for different DIMM (DD)
  //
  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

    EffBusRT = 0;
    TxDelayTemp = 0xFFF;

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        if (EffBusRT < EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe]) {
          EffBusRT = EffectiveBusRoundTrip[dimm][(*rankList)[rank].rankIndex][strobe];
        }
        if (TxDelayTemp > TxDelay[dimm][(*rankList)[rank].rankIndex][strobe]) {
          TxDelayTemp = TxDelay[dimm][(*rankList)[rank].rankIndex][strobe];
        }
      } // rank loop
    } // dimm loop

    if ((EffBusRT - TxDelayTemp) > *effBusDelayDD) {
      *effBusDelayDD = EffBusRT - TxDelayTemp;

#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, strobe, NO_BIT,
                     "EffBusRT %d, TxDelayTemp %d, effBusDelayDD = %d\n",
                     EffBusRT, TxDelayTemp, *effBusDelayDD));
#endif

    }
  } // strobe loop


#ifdef DEBUG_TURNAROUNDS
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "EffBusRT %d, TxDelayTemp %d, effBusDelayDD = %d\n",
                 EffBusRT, TxDelayTemp, *effBusDelayDD));
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "**************************************************************\n"));

  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "effBusDelaySR = %d -> %d\n", *effBusDelaySR, (*effBusDelaySR + (2 * PiTicks) - 1) / (2 * PiTicks)));
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "effBusDelayDR = %d -> %d\n", *effBusDelayDR, (*effBusDelayDR + (2 * PiTicks) - 1) / (2 * PiTicks)));
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "effBusDelayDD = %d -> %d\n", *effBusDelayDD, (*effBusDelayDD + (2 * PiTicks) - 1) / (2 * PiTicks)));
#endif

  *effBusDelaySR = (*effBusDelaySR + (2 * PiTicks) - 1) / (2 * PiTicks);
  *effBusDelayDR = (*effBusDelayDR + (2 * PiTicks) - 1) / (2 * PiTicks);
  *effBusDelayDD = (*effBusDelayDD + (2 * PiTicks) - 1) / (2 * PiTicks);

  return;
} // EffectiveBusDelay

UINT8
CkDiffGap(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT8     mode
         )
/*++



  @param @param host        - Pointer to sysHost
  @param @param socket      - Socket number
  @param @param ch          - Channel number
  @param Mode               - Timing Mode:
                                SR_MODE
                                DR_MODE
                                DD_MODE

  @retval Maximum delay between clocks across DIMMs in Pi ticks

  --*/
{

  if (GetMaxClkDiffDD(host, socket, ch, mode) > 39) {
    return 1;
  } else {
    return 0;
  }
} // CkDiffGap

UINT16
GetMaxClkDiffDD(
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     mode
               )
/*++

  Finds the maximum clock difference between DIMMs in a channel

  @param host        - Pointer to sysHost
  @param socket      - Socket number
  @param ch          - Channel number
  @param Mode        - Timing Mode:
                         SR_MODE
                         DR_MODE
                         DD_MODE

  @retval Maximum delay between clocks across DIMMs in Pi ticks

--*/
{
  UINT8               dimm;
  UINT8               rank;
  UINT8               clocksFound = 0;
  UINT8               clkIndex;
  INT16               clkDelay[MAX_RANK_CH];
  UINT16              diff = 0;
  UINT16              diffTemp = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);

  //
  // No need to ckeck for differences between DIMMs if there are not multiple DIMMs
  //
  if ((*channelNvList)[ch].maxDimm < 2) return 0;

  for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

    if (mode == DR_MODE) {
      clocksFound = 0;
    }

    rankList = GetRankNvList(host, socket, ch, dimm);
    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
      CHIP_FUNC_CALL(host, GetSetClkDelay (host, socket, ch, (*rankList)[rank].ckIndex, GSM_READ_ONLY, &clkDelay[clocksFound]));
      for (clkIndex = 0; clkIndex < clocksFound; clkIndex++) {
          if (clkDelay[clocksFound] > clkDelay[clkIndex]) {
            diffTemp = clkDelay[clocksFound] - clkDelay[clkIndex];
          } else {
            diffTemp = clkDelay[clkIndex] - clkDelay[clocksFound];
          }

          if (diffTemp > (128 - diffTemp)) {
            diffTemp = 128 - diffTemp;
          }

          if (diffTemp > diff) {
            diff = diffTemp;
          }
      } // clkIndex Loop

#ifdef DEBUG_TURNAROUNDS
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                     "CLK %d Pi Delay = %d\n", (*rankList)[rank].ckIndex, clkDelay[clocksFound]));
#endif

      if (mode == DR_MODE) {

#ifdef DEBUG_TURNAROUNDS
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                       "diff = %d\n", diff));
#endif

      }
      clocksFound++;
    } // rank loop
  } // dimm loop


  if (mode == DD_MODE) {

#ifdef DEBUG_TURNAROUNDS
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "Diff = %d\n", diff));
#endif

  }

  return diff;
} // GetMaxClkDiffDD
