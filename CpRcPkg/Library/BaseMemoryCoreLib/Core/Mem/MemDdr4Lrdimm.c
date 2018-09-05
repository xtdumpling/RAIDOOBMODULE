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
 *      IMC and DDR4 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "SysFunc.h"
#include "Cpgc.h"

#ifdef  LRDIMM_SUPPORT

// Program Desired Resulst here
// Rising Edge RE
UINT16 BCW2x[MAX_STROBE/2] = {64, 64, 64, 64, 64, 64, 64, 64, 64};
UINT16 BCW3x[MAX_STROBE/2] = {64, 64, 64, 64, 64, 64, 64, 64, 64};
UINT16 BCWAx[MAX_STROBE/2] = {72, 72, 72, 72, 72, 72, 72, 72, 72};
UINT16 BCWBx[MAX_STROBE/2] = {72, 72, 72, 72, 72, 72, 72, 72, 72};
INT16 BCCX[MAX_STROBE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
INT16 BCDX[MAX_STROBE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
UINT16 BC4X5X[MAX_STROBE] = {63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};
UINT16 BC8X9X[MAX_STROBE] = {63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};

//
// Local function prototypes
//
static
UINT32
MDQReceiveEnablePhaseTraining(
                             PSYSHOST  host,
                             UINT8     socket
                             );

static
UINT32
DramInterfaceWlTraining(
                       PSYSHOST  host,
                       UINT8     socket
                       );

static
void
MDQCoarseWlTraining(
                   PSYSHOST  host,
                   UINT8     socket
                   );

static
UINT32
MDQWriteDelayTraining(
                     PSYSHOST  host,
                     UINT8     socket
                     );

static
void
MDQReceiveEnableCycleTraining(
                             PSYSHOST  host,
                             UINT8     socket
                             );

static
UINT32
MDQReadDelayTraining(
                    PSYSHOST  host,
                    UINT8     socket
                    );

static
UINT32
Ddr4LrdimmBacksideRxOneRankTimingMode(
                                     PSYSHOST  host,
                                     UINT8     socket
                                     );

static
UINT32
Ddr4LrdimmBacksideDisableOneRankTimingMode(
                                          PSYSHOST  host,
                                          UINT8     socket
                                          );

static
UINT32
Ddr4LrdimmBacksideTxOneRankTimingMode(
                                     PSYSHOST  host,
                                     UINT8     socket
                                     );

UINT32
SetLrdimmMrdResults (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    UINT8     dimm,
                    UINT8     rank,
                    UINT16    centerPoint[MAX_STROBE]
                    );

UINT32
SetLrdimmMwdResults (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    UINT8     dimm,
                    UINT8     rank,
                    UINT16    centerPoint[MAX_STROBE]
                    );

void
GetLrdimmTrainResults (
                      PSYSHOST  host,
                      UINT8     socket,
                      UINT8     dimm,
                      UINT8     rank,
                      UINT16    piDelay,
                      UINT8     step
                      );

void
GetLrdimmMrecResults (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT8     dimm,
                     UINT8     rank,
                     INT8      cycleSetting
                     );

void
GetLrdimmCwlResults (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     dimm,
                    UINT8     rank,
                    INT8      cycleDelay
                    );

void
GetLrdimmMrdMwdResults (
                       PSYSHOST  host,
                       UINT8     socket,
                       UINT8     dimm,
                       UINT8     rank,
                       UINT8     delaySetting,
                       UINT8     step
                       );

void
SetupLrdimmReadTest(
                   PSYSHOST  host,
                   UINT8     socket,
                   UINT32    chBitmask,
                   UINT8     NumCL,
                   UINT8     loopCount
                   );

void
SetupLrdimmReadWriteTest(
                        PSYSHOST  host,
                        UINT8     socket,
                        UINT32    chBitmask,
                        UINT8     NumCL,
                        UINT8     loopCount
                        );

static
void
LrdimmCycleTrainingInit (
                        PSYSHOST host,
                        UINT8    socket,
                        UINT32   chBitmask,
                        UINT8    dimm,
                        UINT8    rank,
                        UINT8    step
                        );

void
LrdimmExitRankTraining (
                       PSYSHOST host,
                       UINT8    socket,
                       UINT32   chBitmask,
                       UINT8    dimm,
                       UINT8    rank
                       );

static
void
LrdimmMrdInit (
              PSYSHOST host,
              UINT8    socket,
              UINT32   chBitmask,
              UINT8    dimm,
              UINT8    rank
              );

static
void
LrdimmMwdInit (
              PSYSHOST host,
              UINT8    socket,
              UINT32   chBitmask,
              UINT8    dimm,
              UINT8    rank
              );

void
PrintMrecResults (
                 PSYSHOST host,
                 UINT8 socket,
                 UINT8 ch,
                 UINT8 dimm,
                 UINT8 ReadMode
                 );

void
SetBufferMPRPattern (
                    PSYSHOST host,
                    UINT8    socket,
                    UINT8    ch,
                    UINT8    dimm,
                    UINT8    rank,
                    UINT8    *pattern
                    );


//
// code starts here
//
void
SkipDdr4LrdimmBacksideRxDelay(
                              PSYSHOST host,
                              UINT8 socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT32                            chBitmask;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "COSIM_WA: SkipDdr4LrdimmBacksideRxDelay\n"));

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;

        SetLrdimmMrdResults(host, socket, ch, dimm, rank, BC4X5X);
      } // ch
    } // rank
  } // dimm
}

void
SkipDdr4LrdimmBacksideTxDelay(
                              PSYSHOST host,
                              UINT8 socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT32                            chBitmask;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "COSIM_WA: SkipDdr4LrdimmBacksideTxDelay\n"));

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;

        SetLrdimmMwdResults(host, socket, ch, dimm, rank, BC8X9X);
      } // ch
    } // rank
  } // dimm
}

void
SkipDdr4LrdimmBacksideRxCycle(
                              PSYSHOST host,
                              UINT8 socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             strobe;
  UINT32                            chBitmask;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  UINT8                             *controlWordDataPtr;
  UINT8                             codedNibbleCycleSetting;
  UINT8                             controlWordAddr;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "COSIM_WA: SkipDdr4LrdimmBacksideRxCycle\n"));

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;
        rankList   = GetRankNvList(host, socket, ch, dimm);

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if (BCCX[strobe] < 0) {
            codedNibbleCycleSetting = 4 + (UINT8)ABS(BCCX[strobe]);
          } else {
            codedNibbleCycleSetting = (UINT8)BCCX[strobe];
          }
          (*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9] &= (0xF0 >> (4 * (strobe / 9)));
          (*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9] |= (codedNibbleCycleSetting << (4 * (strobe / 9)));
        } // strobe

        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBCCxEx[0]);
        controlWordAddr = LRDIMM_BCCx + ((rank >> 1) * 0x20);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, (rank & BIT0) , controlWordAddr, ALL_DATABUFFERS);
      } // ch
    } // rank
  } // dimm
}

void
SkipDdr4LrdimmBacksideTxCoarse(
                              PSYSHOST host,
                              UINT8 socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             strobe;
  UINT32                            chBitmask;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  UINT8                             *controlWordDataPtr;
  UINT8                             codedNibbleCycleSetting;
  UINT8                             controlWordAddr;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "COSIM_WA: SkipDdr4LrdimmBacksideTxCoarse\n"));

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;
        rankList   = GetRankNvList(host, socket, ch, dimm);

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if (BCDX[strobe] < 0) {
            codedNibbleCycleSetting = 4 + (UINT8)ABS(BCDX[strobe]);
          } else {
            codedNibbleCycleSetting = (UINT8)BCDX[strobe];
          }
          (*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9] &= (0xF0 >> (4 * (strobe / 9)));
          (*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9] |= (codedNibbleCycleSetting << (4 * (strobe / 9)));
        } // strobe

        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBCDxFx[0]);
        controlWordAddr = LRDIMM_BCDx + ((rank >> 1) * 0x20);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, (rank & BIT0) , controlWordAddr, ALL_DATABUFFERS);
      } // ch
    } // rank
  } // dimm
}

void
SkipDdr4LrdimmBacksideRxPhase(
                              PSYSHOST host,
                              UINT8 socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             strobe;
  UINT32                            chBitmask;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordFunc;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "COSIM_WA: SkipDdr4LrdimmBacksideRxPhase\n"));

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;
        rankStruct = GetRankStruct(host, socket, ch, dimm);
        rankList   = GetRankNvList(host, socket, ch, dimm);

        for (strobe = 0; strobe < MAX_STROBE/2; strobe++) {
          (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe]     = (UINT8) (((BCW2x[strobe]/2) + 32) % 64);
          (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe + 9] = (UINT8) (((BCW3x[strobe]/2) + 32) % 64);
          (*rankList)[rank].lrBuf_FxBC2x3x[strobe]             = (UINT8) (((BCW2x[strobe]/2) + 32) % 64);
          (*rankList)[rank].lrBuf_FxBC2x3x[strobe + 9]         = (UINT8) (((BCW3x[strobe]/2) + 32) % 64);

        } //strobe

        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC2x3x[0];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC2x, ALL_DATABUFFERS);
        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC2x3x[9];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC3x, ALL_DATABUFFERS);
      } // ch
    } // rank
  } // dimm
} // SkipDdr4LrdimmBacksideRxPhase

void
SkipDdr4LrdimmBacksideTxFine(
                              PSYSHOST host,
                              UINT8 socket
)
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             strobe;
  UINT32                            chBitmask;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordFunc;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "COSIM_WA: SkipDdr4LrdimmBacksideTxFine\n"));

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {

      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;
        rankStruct = GetRankStruct(host, socket, ch, dimm);
        rankList   = GetRankNvList(host, socket, ch, dimm);

        for (strobe = 0; strobe < MAX_STROBE/2; strobe++) {
          (*rankStruct)[rank].cachedLrBuf_FxBCAxBx[strobe]     = (UINT8) ((BCWAx[strobe]/2) % 64);
          (*rankStruct)[rank].cachedLrBuf_FxBCAxBx[strobe + 9] = (UINT8) ((BCWBx[strobe]/2) % 64);
          (*rankList)[rank].lrBuf_FxBCAxBx[strobe]             = (UINT8) ((BCWAx[strobe]/2) % 64);
          (*rankList)[rank].lrBuf_FxBCAxBx[strobe + 9]         = (UINT8) ((BCWBx[strobe]/2) % 64);

        } //strobe

        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBCAxBx[0];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BCAx, ALL_DATABUFFERS);
        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBCAxBx[9];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BCBx, ALL_DATABUFFERS);
      } // ch
    } // rank
  } // dimm
} // SkipDdr4LrdimmBacksideTxFine
/**

  Perform DDR4 LRDIMM Backside Read Training (RX)

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
Ddr4LrdimmBacksideRxPhase (
                          PSYSHOST  host
                          )
{
  UINT8                             socket;
  UINT32                            status = SUCCESS;
  struct channelNvram               (*channelNvList)[MAX_CH];
  UINT8                             ch;
  UINT8                             dimm;

  socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);
  //
  // Reasons to just return with SUCCESS...
  //
  if ((~host->memFlows & MF_LRBUF_RD_EN) ||               /* Training step is disabled by the SIMICS */
      (host->nvram.mem.socket[socket].enabled == 0) ||     /* Socket is disable */
      (host->nvram.mem.socket[socket].maxDimmPop == 0) ||  /* No DIMM present */
      (host->nvram.mem.dramType == SPD_TYPE_DDR3) ||       /* No DDR4 dimms present */
      (host->nvram.mem.socket[socket].lrDimmPresent == 0)) /* No LRDIMM present */ {

#ifdef COSIM_EN
    SkipDdr4LrdimmBacksideRxPhase(host, socket);
#endif
    return SUCCESS;
  }

  //
  // HSX B0 workaround, BIOS sighting #4168616
  //
  CHIP_FUNC_CALL(host, SetDisable2cycBypass (host, socket, 0));

  //
  //MDQ Receive Enable Phase Training (MREP)
  //
  MDQReceiveEnablePhaseTraining(host, socket);

  //
  // HSX B0 workaround, BIOS sighting #4168616
  //
  CHIP_FUNC_CALL(host, SetDisable2cycBypass (host, socket, 1));

  // Set function control nibble back to 0
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((*channelNvList)[ch].dimmList[dimm].rcLrFunc != 0) {
        WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
        (*channelNvList)[ch].dimmList[dimm].rcLrFunc = 0;
      }
    } // ch
  } //dimm

  return status;
} //Ddr4LrdimmBacksideRx


/**

  Perform DDR4 LRDIMM Backside Read Training (RX)

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
Ddr4LrdimmBacksideRxCycle (
                          PSYSHOST  host
                          )
{
  UINT8                             socket;
  UINT32                            status = SUCCESS;
  struct channelNvram               (*channelNvList)[MAX_CH];
  UINT8                             ch;
  UINT8                             dimm;

  socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);

  //
  // Reasons to just return with SUCCESS...
  //
  if ((~host->memFlows & MF_LRBUF_RD_EN) ||                 /* Training step is disabled by the SIMICS */
      (host->nvram.mem.socket[socket].enabled == 0) ||       /* No Socket present */
      (host->nvram.mem.socket[socket].maxDimmPop == 0) ||    /* No DIMM present */
      (host->nvram.mem.dramType == SPD_TYPE_DDR3)   ||       /* No DDR4 dimms present */
      (host->nvram.mem.socket[socket].lrDimmPresent == 0)) /* No LRDIMM present */ {

#ifdef COSIM_EN
  SkipDdr4LrdimmBacksideRxCycle(host, socket);
#endif
    return SUCCESS;
  }

  //
  //MDQ Receive Enable Cycle Training (MREC)
  //
  MDQReceiveEnableCycleTraining(host, socket);

  // Set function control nibble back to 0
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((*channelNvList)[ch].dimmList[dimm].rcLrFunc != 0) {
        WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
        (*channelNvList)[ch].dimmList[dimm].rcLrFunc = 0;
      }
    } // ch
  } //dimm

  return status;
} //Ddr4LrdimmBacksideRxCycle


/**

  Perform DDR4 LRDIMM Backside Read Training (RX)

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
Ddr4LrdimmBacksideRxDelay (
                          PSYSHOST  host
                          )
{
  UINT8                             socket;
  UINT32                            status = SUCCESS;
  struct channelNvram               (*channelNvList)[MAX_CH];
  UINT8                             ch;
  UINT8                             dimm;

  socket = host->var.mem.currentSocket;
  channelNvList = GetChannelNvList(host, socket);

  //
  // Reasons to just return with SUCCESS...
  //
  if ((~host->memFlows & MF_LRBUF_RD_EN) ||                 /* Training step is disabled by the SIMICS */
      (host->nvram.mem.socket[socket].enabled == 0) ||       /* No Socket present */
      (host->nvram.mem.socket[socket].maxDimmPop == 0) ||    /* No DIMM present */
      (host->nvram.mem.dramType == SPD_TYPE_DDR3)   ||       /* No DDR4 dimms present */
      (host->nvram.mem.socket[socket].lrDimmPresent == 0))   /* No LRDIMM present */ {

#ifdef COSIM_EN
    SkipDdr4LrdimmBacksideRxDelay(host, socket);
#endif
    return SUCCESS;
  }

  //
  // MDQ Read Delay Training (MRD)
  //
  MDQReadDelayTraining(host, socket);

  Ddr4LrdimmBacksideRxOneRankTimingMode(host, socket);

  // Set function control nibble back to 0
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((*channelNvList)[ch].dimmList[dimm].rcLrFunc != 0) {
        WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
        (*channelNvList)[ch].dimmList[dimm].rcLrFunc = 0;
      }
    } // ch
  } //dimm

  return status;
} //Ddr4LrdimmBacksideRxDelay


/**

  Perform LRDIMM Backside One Rank Timing Mode Read calculations
  and enable the support.

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
UINT32
Ddr4LrdimmBacksideRxOneRankTimingMode(
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             backsideRank;
  UINT8                             backsideRankMax;
  UINT32                            chBitmask;
  UINT8                             strobe;
  INT16                             rxCycle, averageRxCycle;
  INT16                             rxPhase, averageRxPhase;
  INT16                             rxDelay;
  INT16                             averageRecieveEnable, averageReadDelay;
  struct ddrChannel                 (*channelList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct dimmDevice                 (*chdimmList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordData;


  channelList = &host->var.mem.socket[socket].channelList;
  //
  // Create a bit mask of channels that have an LRDIMM with at least 1 rank present for dimm 0, rank 0
  //
  chBitmask = GetChBitmaskLrdimm (host, socket, 0, 0);
  for (ch = 0; ch < MAX_CH; ch++) {
    //
    // Initialize the chOneRankTimingModeEnable variable
    //
    (*channelList)[ch].chOneRankTimingModeEnable = ONE_RANK_TIMING_MODE_DISABLED;
    if (!((1 << ch) & chBitmask )) continue;
    (*channelList)[ch].chOneRankTimingModeEnable = host->setup.mem.oneRankTimingModeEn;
  } //ch
  //
  // For each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Create a bit mask of channels that have at least 1 rank present for this dimm and rank 0
    //
    chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
    //
    // For each channel
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if (!((1 << ch) & chBitmask )) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);

      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                  "lrbufGen = %u\n", (*dimmNvList)[dimm].lrbufGen));

      if ((*dimmNvList)[dimm].lrbufGen == 0) {  //One Rank Timing Moded only supported on newer RCD revision, check the next dimm
        (*channelList)[ch].chOneRankTimingModeEnable = ONE_RANK_TIMING_MODE_DISABLED;
      }
      // Montage Gen2 LRBUF W/A
      if (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE)&& ((*dimmNvList)[dimm].lrbufGen == 0x01) && ((*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_GEN2_A0)) {
        (*channelList)[ch].chOneRankTimingModeEnable = ONE_RANK_TIMING_MODE_DISABLED;
      }
    } //ch
  } //dimm

#ifdef SERIAL_DBG_MSG
  for (ch = 0; ch < MAX_CH; ch++) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "One Rank Timing Mode(1=enabled): %d \n", (*channelList)[ch].chOneRankTimingModeEnable));

  } //ch
#endif

  // For each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Create a bit mask of channels that have at least 1 rank present for this dimm and rank 0
    //
    chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
    //
    // For each channel
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if (!((1 << ch) & chBitmask )) continue;
      if ((*channelList)[ch].chOneRankTimingModeEnable == ONE_RANK_TIMING_MODE_DISABLED) continue;

      chdimmList  = &host->var.mem.socket[socket].channelList[ch].dimmList;
      dimmNvList = GetDimmNvList(host, socket, ch);
      rankList = GetRankNvList(host, socket, ch, dimm);
      //
      // Save off the original rank 0 values first
      //
      if ((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) {
        backsideRankMax = (*dimmNvList)[dimm].numDramRanks - 1;
      } else {
        backsideRankMax = 0;
      }
      for (backsideRank = 0; backsideRank <= backsideRankMax; backsideRank++) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          (*chdimmList)[dimm].originalRank0Lrbuf_FxBC2x3x[backsideRank][strobe] = (*rankList)[backsideRank].lrBuf_FxBC2x3x[strobe];
          (*chdimmList)[dimm].originalRank0Lrbuf_FxBC4x5x[backsideRank][strobe] = (*rankList)[backsideRank].lrBuf_FxBC4x5x[strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*chdimmList)[dimm].originalRank0Lrbuf_FxBCCxEx[backsideRank][strobe] = (*rankList)[backsideRank].lrBuf_FxBCCxEx[strobe];
          }
        } //strobe
      } // backsideRank loop

      //
      // For each strobe
      //
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // find the average backside Recieve Enable (RX phase + cycle) and Read Delay.
        //
        averageRecieveEnable = 0;
        averageReadDelay = 0;
        //
        // For each rank
        //
        for (backsideRank = 0; backsideRank < (*dimmNvList)[dimm].numDramRanks; backsideRank++) {
          rxPhase = (INT16)((*rankList)[backsideRank].lrBuf_FxBC2x3x[strobe]);
          rxCycle = (INT16)((((*rankList)[backsideRank].lrBuf_FxBCCxEx[strobe % 9]) >> (4 * (strobe / 9))) & 0x0F);
          if (rxCycle > 2) rxCycle = -1 * (rxCycle & 0x3);
          rxCycle += 2; // add 2 just to avoid negative values
          averageRecieveEnable += ((rxCycle * 64) + rxPhase);
          rxDelay = (INT16)((*rankList)[backsideRank].lrBuf_FxBC4x5x[strobe]);
          if (rxDelay > 15) {
            rxDelay = -1 * (rxDelay - 16);
          }
          averageReadDelay += rxDelay;
        } //backsideRank
        averageRecieveEnable = averageRecieveEnable / (*dimmNvList)[dimm].numDramRanks;
        averageRxCycle = (averageRecieveEnable / 64) - 2; // subtract 2 to remove the previous adjustment for negative numbers
        averageRxPhase = averageRecieveEnable % 64;
        averageReadDelay = averageReadDelay / (*dimmNvList)[dimm].numDramRanks;

        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC2x3x[strobe] = (UINT8)averageRxPhase;
        if (averageRxCycle < 0) {
          averageRxCycle = ABS(averageRxCycle) + 4;
        }
        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCCxEx[strobe % 9] &= (UINT8)(0xF0 >> (4 * (strobe / 9)));
        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCCxEx[strobe % 9] |= (UINT8)(averageRxCycle << (4 * (strobe / 9)));

        if (averageReadDelay < 0) {
          averageReadDelay = ABS(averageReadDelay) + 16;
        }
        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC4x5x[strobe] = (UINT8)averageReadDelay;
      } //strobe
      //
      // Store One Rank Timing mode values into rank 0
      //
      if ((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) {
        backsideRankMax = (*dimmNvList)[dimm].numDramRanks - 1;
      } else {
        backsideRankMax = 0;
      }
      rankStruct = GetRankStruct(host, socket, ch, dimm);
      for (backsideRank = 0; backsideRank <= backsideRankMax; backsideRank++) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC2x3x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC2x3x[strobe];
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC4x5x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC4x5x[strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*rankStruct)[backsideRank].cachedLrBuf_FxBCCxEx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCCxEx[strobe];
          }
          (*rankList)[backsideRank].lrBuf_FxBC2x3x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC2x3x[strobe];
          (*rankList)[backsideRank].lrBuf_FxBC4x5x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC4x5x[strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*rankList)[backsideRank].lrBuf_FxBCCxEx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCCxEx[strobe];
          }
        } // strobe loop

        //
        // program new rank 0 values and enable One Rank Timing Mode
        //
        //
        // Write F[0]BC2x/F[0]BC3x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC2x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC3x, ALL_DATABUFFERS);
        //
        // Write F[0]BC4x/F[0]BC5x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC4x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC5x, ALL_DATABUFFERS);
        //
        // Write F0 BCCxEx
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCCxEx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCCx, ALL_DATABUFFERS);
      } // backsideRank loop
      //
      // Write to F0 BC1x, enabling One Rank Timing Mode
      //
      (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x | LRDIMM_ONE_RANK_TIMING_MODE;
      controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
      WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);

    } //ch
  } //dimm

  return SUCCESS;
} //Ddr4LrdimmBacksideRxOneRankTimingMode


/**

  Disable LRDIMM Backside One Rank Timing Mode

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
UINT32
Ddr4LrdimmBacksideDisableOneRankTimingMode(
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT32                            chBitmask;
  UINT8                             strobe;
  struct ddrChannel                 (*channelList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct dimmDevice                 (*chdimmList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordData;
  UINT8                             backsideRank;
  UINT8                             backsideRankMax;


  channelList = &host->var.mem.socket[socket].channelList;
  //
  // For each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Create a bit mask of channels that have at least 1 rank present for this dimm and rank 0
    //
    chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
    //
    // For each channel
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if (!((1 << ch) & chBitmask )) continue;
      if ((*channelList)[ch].chOneRankTimingModeEnable == ONE_RANK_TIMING_MODE_DISABLED) continue;

      chdimmList  = &host->var.mem.socket[socket].channelList[ch].dimmList;
      dimmNvList = GetDimmNvList(host, socket, ch);
      rankList = GetRankNvList(host, socket, ch, dimm);
      //
      // restore original rank 0
      //
      if ((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) {
        backsideRankMax = (*dimmNvList)[dimm].numDramRanks - 1;
      } else {
        backsideRankMax = 0;
      }
      rankStruct = GetRankStruct(host, socket, ch, dimm);
      for (backsideRank = 0; backsideRank <= backsideRankMax; backsideRank++) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC2x3x[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBC2x3x[backsideRank][strobe];
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC4x5x[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBC4x5x[backsideRank][strobe];
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC8x9x[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBC8x9x[backsideRank][strobe];
          (*rankStruct)[backsideRank].cachedLrBuf_FxBCAxBx[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBCAxBx[backsideRank][strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*rankStruct)[backsideRank].cachedLrBuf_FxBCCxEx[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBCCxEx[backsideRank][strobe];
            (*rankStruct)[backsideRank].cachedLrBuf_FxBCDxFx[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBCDxFx[backsideRank][strobe];
          }
          (*rankList)[backsideRank].lrBuf_FxBC2x3x[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBC2x3x[backsideRank][strobe];
          (*rankList)[backsideRank].lrBuf_FxBC4x5x[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBC4x5x[backsideRank][strobe];
          (*rankList)[backsideRank].lrBuf_FxBC8x9x[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBC8x9x[backsideRank][strobe];
          (*rankList)[backsideRank].lrBuf_FxBCAxBx[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBCAxBx[backsideRank][strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*rankList)[backsideRank].lrBuf_FxBCCxEx[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBCCxEx[backsideRank][strobe];
            (*rankList)[backsideRank].lrBuf_FxBCDxFx[strobe] = (*chdimmList)[dimm].originalRank0Lrbuf_FxBCDxFx[backsideRank][strobe];
          }
        } // strobe loop

        //
        // program rank 0 values and disable One Rank Timing Mode
        //
        //
        // Write F[0]BC2x/F[0]BC3x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC2x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC3x, ALL_DATABUFFERS);
        //
        // Write F[0]BC4x/F[0]BC5x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC4x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC5x, ALL_DATABUFFERS);
        //
        // Write F0 BCCxEx
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCCxEx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCCx, ALL_DATABUFFERS);
        //
        // Write F[0]BC8x/F[0]BC9x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC8x9x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC8x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC8x9x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC9x, ALL_DATABUFFERS);
        //
        // Write F[0]BCAx/F[0]BCBx
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCAxBx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCAx, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCAxBx[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCBx, ALL_DATABUFFERS);
        //
        // F0BCDx for backside rank 0
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCDxFx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCDx, ALL_DATABUFFERS);
      } // backsideRank loop
      //
      // Write to F0 BC1x, disabling One Rank Timing Mode
      //
      (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x & ~LRDIMM_ONE_RANK_TIMING_MODE;
      controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
      WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);
    } //ch
  } //dimm

  return SUCCESS;
} //Ddr4LrdimmBacksideDisableOneRankTimingMode


/**

  Perform LRDIMM Backside One Rank Timing Mode Write
  calculations and enable the support.

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
UINT32
Ddr4LrdimmBacksideTxOneRankTimingMode(
  PSYSHOST  host,
  UINT8     socket
  )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             backsideRank;
  UINT8                             backsideRankMax;
  UINT32                            chBitmask;
  UINT8                             strobe;
  INT16                             txCycle, averageTxCycle;
  INT16                             txPhase, averageTxPhase;
  INT16                             txDelay;
  INT16                             averageWriteLeveling, averageWriteDelay;
  struct ddrChannel                 (*channelList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct dimmDevice                 (*chdimmList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordData;


  channelList = &host->var.mem.socket[socket].channelList;
  //
  // For each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Create a bit mask of channels that have at least 1 rank present for this dimm and rank 0
    //
    chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
    //
    // For each channel
    //
    for (ch = 0; ch < MAX_CH; ch++) {
      if (!((1 << ch) & chBitmask )) continue;
      if ((*channelList)[ch].chOneRankTimingModeEnable == ONE_RANK_TIMING_MODE_DISABLED) continue;

      chdimmList  = &host->var.mem.socket[socket].channelList[ch].dimmList;
      dimmNvList = GetDimmNvList(host, socket, ch);
      rankList = GetRankNvList(host, socket, ch, dimm);
      //
      // Save off the original Tx rank 0 values first
      //
      if ((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) {
        backsideRankMax = (*dimmNvList)[dimm].numDramRanks - 1;
      } else {
        backsideRankMax = 0;
      }
      for (backsideRank = 0; backsideRank <= backsideRankMax; backsideRank++) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          (*chdimmList)[dimm].originalRank0Lrbuf_FxBC8x9x[backsideRank][strobe] = (*rankList)[backsideRank].lrBuf_FxBC8x9x[strobe];
          (*chdimmList)[dimm].originalRank0Lrbuf_FxBCAxBx[backsideRank][strobe] = (*rankList)[backsideRank].lrBuf_FxBCAxBx[strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*chdimmList)[dimm].originalRank0Lrbuf_FxBCDxFx[backsideRank][strobe] = (*rankList)[backsideRank].lrBuf_FxBCDxFx[strobe];
          }
        } // strobe loop
      } // backsideRank loop

      //
      // For each strobe
      //
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // find the average backside Dram Write Leveling (RX phase + cycle) and Write Delay.
        //
        averageWriteLeveling = 0;
        averageWriteDelay = 0;
        //
        // For each rank
        //
        for (backsideRank = 0; backsideRank < (*dimmNvList)[dimm].numDramRanks; backsideRank++) {
          txPhase = (INT16)((*rankList)[backsideRank].lrBuf_FxBCAxBx[strobe]);
          txCycle = (INT16)((((*rankList)[backsideRank].lrBuf_FxBCDxFx[strobe % 9]) >> (4 * (strobe / 9))) & 0x0F);
          if (txCycle > 2) txCycle = -1 * (txCycle & 0x3);
          txCycle += 2; // add 2 just to avoid negative values
          averageWriteLeveling += ((txCycle * 64) + txPhase);
          txDelay = (INT16)((*rankList)[backsideRank].lrBuf_FxBC8x9x[strobe]);
          if (txDelay > 15) {
            txDelay = -1 * (txDelay - 16);
          }
          averageWriteDelay += txDelay;
        } //backsideRank
        averageWriteLeveling = averageWriteLeveling / (*dimmNvList)[dimm].numDramRanks;
        averageTxCycle = (averageWriteLeveling / 64) - 2; // subtract 2 to remove the previous adjustment for negative numbers
        averageTxPhase = averageWriteLeveling % 64;
        averageWriteDelay = averageWriteDelay / (*dimmNvList)[dimm].numDramRanks;

        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCAxBx[strobe] = (UINT8)averageTxPhase;
        if (averageTxCycle < 0) {
          averageTxCycle = ABS(averageTxCycle) + 4;
        }
        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCDxFx[strobe % 9] &= (UINT8)(0xF0 >> (4 * (strobe / 9)));
        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCDxFx[strobe % 9] |= (UINT8)(averageTxCycle << (4 * (strobe / 9)));

        if (averageWriteDelay < 0) {
          averageWriteDelay = ABS(averageWriteDelay) + 16;
        }
        (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC8x9x[strobe] = (UINT8)averageWriteDelay;

      } //strobe
      //
      // Store One Rank Timing mode values into rank 0
      //
      if ((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) {
        backsideRankMax = (*dimmNvList)[dimm].numDramRanks - 1;
      } else {
        backsideRankMax = 0;
      }
      rankStruct = GetRankStruct(host, socket, ch, dimm);
      for (backsideRank = 0; backsideRank <= backsideRankMax; backsideRank++) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC2x3x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC2x3x[strobe];
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC4x5x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC4x5x[strobe];
          (*rankStruct)[backsideRank].cachedLrBuf_FxBC8x9x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC8x9x[strobe];
          (*rankStruct)[backsideRank].cachedLrBuf_FxBCAxBx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCAxBx[strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*rankStruct)[backsideRank].cachedLrBuf_FxBCCxEx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCCxEx[strobe];
            (*rankStruct)[backsideRank].cachedLrBuf_FxBCDxFx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCDxFx[strobe];
          }
          (*rankList)[backsideRank].lrBuf_FxBC2x3x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC2x3x[strobe];
          (*rankList)[backsideRank].lrBuf_FxBC4x5x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC4x5x[strobe];
          (*rankList)[backsideRank].lrBuf_FxBC8x9x[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBC8x9x[strobe];
          (*rankList)[backsideRank].lrBuf_FxBCAxBx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCAxBx[strobe];
          if (strobe < (MAX_STROBE / 2)) {
            (*rankList)[backsideRank].lrBuf_FxBCCxEx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCCxEx[strobe];
            (*rankList)[backsideRank].lrBuf_FxBCDxFx[strobe] = (*chdimmList)[dimm].oneRankTimingModeLrbuf_FxBCDxFx[strobe];
          }
        } // strobe loop

        //
        // program new rank 0 values and enable One Rank Timing Mode
        //
        //
        // Write F[0]BC2x/F[0]BC3x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC2x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC2x3x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC3x, ALL_DATABUFFERS);
        //
        // Write F[0]BC4x/F[0]BC5x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC4x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC4x5x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC5x, ALL_DATABUFFERS);
        //
        // Write F0 BCCx
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCCxEx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCCx, ALL_DATABUFFERS);
        //
        // Write F[0]BC8x/F[0]BC9x
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC8x9x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC8x, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBC8x9x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BC9x, ALL_DATABUFFERS);
        //
        // Write F[0]BCAx/F[0]BCBx
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCAxBx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCAx, ALL_DATABUFFERS);

        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCAxBx[9]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCBx, ALL_DATABUFFERS);
        //
        // Write F0 BCDx
        //
        controlWordDataPtr = &((*rankList)[backsideRank].lrBuf_FxBCDxFx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, backsideRank, LRDIMM_BCDx, ALL_DATABUFFERS);
      } // backsideRank loop

      //
      // Write to F0 BC1x, enabling One Rank Timing Mode
      //
      (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x | LRDIMM_ONE_RANK_TIMING_MODE;
      controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
      WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);

    } //ch
  } //dimm

  return SUCCESS;
} //Ddr4LrdimmBacksideTxOneRankTimingMode

/**

  Perform LRDIMM Backside Training (TX)

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
Ddr4LrdimmBacksideTxFineWL (
                           PSYSHOST  host
                           )
{
  UINT8                             socket;
  UINT32                            status = SUCCESS;
  struct channelNvram               (*channelNvList)[MAX_CH];
  UINT8                             ch;
  UINT8                             dimm;

  socket = host->var.mem.currentSocket;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Reasons to just return with SUCCESS...
  //
  if ((~host->memFlows & MF_LRBUF_WR_EN) ||                 /* Training step is disabled by the SIMICS */
      (host->nvram.mem.socket[socket].enabled == 0) ||       /* No Socket present */
      (host->nvram.mem.socket[socket].maxDimmPop == 0) ||    /* No DIMM present */
      (host->nvram.mem.dramType == SPD_TYPE_DDR3)   ||       /* No DDR4 dimms present */
      (host->nvram.mem.socket[socket].lrDimmPresent == 0)) /* No LRDIMM present */ {

#ifdef COSIM_EN
    SkipDdr4LrdimmBacksideTxFine(host, socket);
#endif
    return SUCCESS;
  }

  //
  // If One Rank Timing mode is enabled, disable it and restore multi-rank mode for the purposes of backside training
  //
  if (host->setup.mem.oneRankTimingModeEn) {
    Ddr4LrdimmBacksideDisableOneRankTimingMode(host, socket);
  }

  //
  // DRAM Interface WL (DWL)
  //
  DramInterfaceWlTraining(host, socket);

  //
  // Set function control nibble back to 0
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((*channelNvList)[ch].dimmList[dimm].rcLrFunc != 0) {
        WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
        (*channelNvList)[ch].dimmList[dimm].rcLrFunc = 0;
      }
    } // ch
  } //dimm

  return status;
} //Ddr4LrdimmBacksideTxFineWL

/**

  Perform LRDIMM Backside Training (TX)

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
Ddr4LrdimmBacksideTxCoarseWL (
                             PSYSHOST  host
                             )
{
  UINT8                             socket;
  UINT32                            status = SUCCESS;
  UINT8                             ch;
  UINT8                             dimm;
  struct channelNvram               (*channelNvList)[MAX_CH];

  socket = host->var.mem.currentSocket;

  //
  // Reasons to just return with SUCCESS...
  //
  if ((~host->memFlows & MF_LRBUF_WR_EN) ||                 /* Training step is disabled by the SIMICS */
      (host->nvram.mem.socket[socket].enabled == 0) ||       /* No Socket present */
      (host->nvram.mem.socket[socket].maxDimmPop == 0) ||    /* No DIMM present */
      (host->nvram.mem.dramType == SPD_TYPE_DDR3)   ||       /* No DDR4 dimms present */
      (host->nvram.mem.socket[socket].lrDimmPresent == 0)) /* No LRDIMM present */ {

#ifdef COSIM_EN
    SkipDdr4LrdimmBacksideTxCoarse(host, socket);
#endif
    return SUCCESS;
  }

  //
  // MDQS Coarse WL step (CWL)
  //
  MDQCoarseWlTraining(host, socket);

  channelNvList = GetChannelNvList(host, socket);
  // Set function control nibble back to 0
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if ((*channelNvList)[ch].dimmList[dimm].rcLrFunc != 0) {
        WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
        (*channelNvList)[ch].dimmList[dimm].rcLrFunc = 0;
      }
    } // ch
  } //dimm
  return status;
} //Ddr4LrdimmBacksideTxCoarseWL

/**

  Perform LRDIMM Backside Training (TX)

  @param host  - Pointer to sysHost

  @retval status

**/
UINT32
Ddr4LrdimmBacksideTxDelay (
                          PSYSHOST  host
                          )
{
  UINT8                             socket;
  UINT32                            status = SUCCESS;
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  struct channelNvram               (*channelNvList)[MAX_CH];

  socket = host->var.mem.currentSocket;
  //
  // Reasons to just return with SUCCESS...
  //
  if ((~host->memFlows & MF_LRBUF_WR_EN) ||                 /* Training step is disabled by the SIMICS */
      (host->nvram.mem.socket[socket].enabled == 0) ||       /* No Socket present */
      (host->nvram.mem.socket[socket].maxDimmPop == 0) ||    /* No DIMM present */
      (host->nvram.mem.dramType == SPD_TYPE_DDR3)   ||       /* No DDR4 dimms present */
      (host->nvram.mem.socket[socket].lrDimmPresent == 0))   /* No LRDIMM present */ {

#if COSIM_EN
    SkipDdr4LrdimmBacksideTxDelay(host, socket);
#endif
    return SUCCESS;
  }

  //
  // MDQS-MDQ Write Delay Training step (MWD)
  //
  MDQWriteDelayTraining(host, socket);

  Ddr4LrdimmBacksideTxOneRankTimingMode(host, socket);

  channelNvList = GetChannelNvList(host, socket);
  // Set function control nibble back to 0
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        // for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          // //
          // //Update the "cached" register values from the nvram "actual" register values
          // //
          // (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBC2x3x[strobe];
          // (*rankStruct)[rank].cachedLrBuf_FxBC4x5x[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBC4x5x[strobe];
          // (*rankStruct)[rank].cachedLrBuf_FxBC8x9x[strobe] =  (*channelNvList)[ch].dimmList[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe];
        // } // strobe
        if ((*channelNvList)[ch].dimmList[dimm].rcLrFunc != 0) {
          WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
          (*channelNvList)[ch].dimmList[dimm].rcLrFunc = 0;
        }
      } // ch
    } //rank
  } //dimm

  return status;
} //Ddr4LrdimmBacksideTxDelay


/**

  Set F[x]BC4x and F[x]BC5x to the respective settings that are in the center of the passing region.

  @param host              - Pointer to sysHost
  @param ch                - Channel number
  @param dimm              - Dimm number
  @param rank              - Rank number
  @param strobe            - Strobe number

  @retval N/A

**/
UINT32
SetLrdimmMrdResults (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    UINT8     dimm,
                    UINT8     rank,
                    UINT16    centerPoint[MAX_STROBE]
                    )
{
  UINT32                            status = SUCCESS;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             mdqsReadDelay = 0;
  UINT8                             strobe;
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordFunc;

  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((centerPoint[strobe]/4) < 15) {
      mdqsReadDelay = (UINT8)(((centerPoint[strobe]/4) ^ 31) & 0x1F);
    } else {
      mdqsReadDelay = (UINT8)(((centerPoint[strobe]/4) - 15) & 0x1F);
    }
    //
    // store the data to write for later use
    //
    (*rankList)[rank].lrBuf_FxBC4x5x[strobe] = mdqsReadDelay;
    (*rankStruct)[rank].cachedLrBuf_FxBC4x5x[strobe] = (*rankList)[rank].lrBuf_FxBC4x5x[strobe];
  } //strobe
  controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC4x5x[0];
  controlWordFunc = rank;
  WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC4x, ALL_DATABUFFERS);
  controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC4x5x[9];
  controlWordFunc = rank;
  WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC5x, ALL_DATABUFFERS);

  return status;
} //SetLrdimmMrdResults


/**

  Set F[x]BC8x and F[x]BC9x to the respective settings that are in the center of the passing region.

  @param host              - Pointer to sysHost
  @param ch                - Channel number
  @param dimm              - Dimm number
  @param rank              - Rank number
  @param strobe            - Strobe number

  @retval N/A

**/
UINT32
SetLrdimmMwdResults (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     ch,
                    UINT8     dimm,
                    UINT8     rank,
                    UINT16    centerPoint[MAX_STROBE]
                    )
{
  UINT32                            status = SUCCESS;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             mdqsWriteDelay = 0;
  UINT8                             strobe;
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordFunc;

  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  for (strobe = 0; strobe < MAX_STROBE; strobe++) {
    if ((centerPoint[strobe]/4) < 15) {
      mdqsWriteDelay = (UINT8)(((centerPoint[strobe]/4) ^ 31) & 0x1F);
    } else {
      mdqsWriteDelay = (UINT8)(((centerPoint[strobe]/4) - 15) & 0x1F);
    }
    //
    // store the data to write for later use
    //
    (*rankList)[rank].lrBuf_FxBC8x9x[strobe] = mdqsWriteDelay;
    (*rankStruct)[rank].cachedLrBuf_FxBC8x9x[strobe] = (*rankList)[rank].lrBuf_FxBC8x9x[strobe];
  } //strobe
  controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC8x9x[0];
  controlWordFunc = rank;
  WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC8x, ALL_DATABUFFERS);
  controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC8x9x[9];
  controlWordFunc = rank;
  WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC9x, ALL_DATABUFFERS);

  return status;
} //SetLrdimmMwdResults


/**

  Get's the results from the current test

  @param host      - Pointer to sysHost
  @param socket    - socket number
  @param dimm      - DIMM number
  @param rank      - rank number of the DIMM
  @param piDelay   - CLK adjustment setting for this test
  @param step      - which training step is this called from, using same value as training mode for convenience

  @retval N/A

**/
void
GetLrdimmTrainResults (
                      PSYSHOST  host,
                      UINT8     socket,
                      UINT8     dimm,
                      UINT8     rank,
                      UINT16    piDelay,
                      UINT8     step
                      )
{
  UINT8               ch;
  UINT8               strobe;
  UINT8               swizzleStrobe;
  UINT8               dWord;
  UINT32              lrdimmFeedBack;
  struct TrainingResults  (*lrTrainRes)[MAX_CH][MAX_STROBE];
  UINT8               DumArr[7] = {0,0,0,0,0,0,0};
  UINT32              chBitmask;
  UINT32              databit;
  UINT32              stepSizeBits = (1 << LRDIMM_BACKSIDE_STEP_SIZE) - 1;

  chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);

  if (step == LRDIMM_MREP_TRAINING_MODE) {
    lrTrainRes      = &host->var.mem.lrTrainRes;
  } else {
    lrTrainRes      = &host->var.mem.lrDwlTrainRes;
  }

  dWord = (UINT8)((piDelay - LRDIMM_BACKSIDE_PI_START) / 32);

  if (step == LRDIMM_MREP_TRAINING_MODE) {
    //
    // ii.  Execute the CPGC MPR RD sequence
    //
    // Send a burst of 7 read commands back to back (4 DCLK apart) using MC_SPECIAL_COMMAND CRs
    //
    CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));
    //
    // iii. As the CPGC test is still running, after a TBD wait time, read the results of the DQ sampling in the DDRIO.
    FixedDelay(host, 1);
  }
  //
  // enter sense amp training mode here to latch results
  //
  CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, SENSE_AMP));
  FixedDelay(host, 1);

  //
  // Exit sense amp training mode
  //
  CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, DISABLE_SENSE_AMP));

  if (step == LRDIMM_MREP_TRAINING_MODE) {
    //
    //iv. Stop the CPGC test in prep for the next phase setting.
    //
    CHIP_FUNC_CALL(host, CpgcGlobalTestStop (host, socket, chBitmask));
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask)) continue;

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      swizzleStrobe = CHIP_FUNC_CALL(host, CheckBacksideSwizzle(host, socket, ch, dimm, strobe));
      lrdimmFeedBack = CHIP_FUNC_CALL(host, LrDimmReadTrainFeedback (host, socket, ch, swizzleStrobe, 0xFF));
      
      //
      // Add full results to a larger results structure (populate odd ticks same as the even, so I can reuse same results routines
      // as host receive enable)
      //

      if (lrdimmFeedBack) { // mapping a clock cycle so any bit of 4 passing will be logged as a 1
        databit = (stepSizeBits << (piDelay % 32)); // 3 is used to fill 2 locations in the results array
        (*lrTrainRes)[ch][strobe].results[dWord] |= databit;
      }

    } // strobe loop
  } //ch loop
} // GetLrdimmTrainingResults


#ifdef  MONTAGE_LRBUF_WA
//
//M88DDR4 RCD/DB Sighting Report, sighting 1302101
// montage M88DDR4DB01-A1 mrec workaround for DB A1
//
void
ReadBCWAsynByDq (
                PSYSHOST host,
                UINT8    socket,
                UINT8    ch,
                UINT8    dimm,
                UINT8    rank,
                UINT8    controlWordData[MAX_STROBE/2],
                UINT8    controlWordFunc,
                UINT8    controlWordAddr
                )
/**

    Read BC Work Around By DQ

    @param host                          - Pointer to SysHost
    @param socket                        - Processor Socket
    @param ch                            - Channel on socket
    @param dimm                          - DIMM on channel
    @param rank                          - Rank on DIMM
    @param controlWordData[MAX_STROBE/2] - Control Word Data
    @param controlWordFunc               - Control Word Function
    @param controlWordAddr               - Control Word Address

    @retval N/A

**/
{
  UINT8   bit;
  UINT8   strobe;
  UINT32  lrDimmFeedback;
  UINT8   i;

  for (i = 0; i < 9; i++) {
    controlWordData[i] = 0;
  }

  lrDimmFeedback = 0;
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);

  WriteLrbuf(host, socket, ch, dimm, 0, 0x37, LRDIMM_F7, LRDIMM_BC5x);
  WriteLrbuf(host, socket, ch, dimm, 0, 2, LRDIMM_F7, LRDIMM_BC6x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xb7, LRDIMM_F7, LRDIMM_BC5x);
  WriteLrbuf(host, socket, ch, dimm, 0, 2, LRDIMM_F7, LRDIMM_BC6x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);

  WriteLrbuf(host, socket, ch, dimm, 0, controlWordAddr, LRDIMM_F7, LRDIMM_BC5x);
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

  for (bit = 0; bit < 8; bit++) {
    if (controlWordFunc > 7) {
      WriteLrbuf(host, socket, ch, dimm, 0, 1<<4|bit<<1|1<<0, LRDIMM_F6, LRDIMM_BC6x);
    } else {
      WriteLrbuf(host, socket, ch, dimm, 0, controlWordFunc<<5|bit<<1|1<<0, LRDIMM_F6, LRDIMM_BC6x);
    }

    FixedDelay(host, 10);

    // this should still work for SKX. We take bit0 feedback of the lower strobes and shove it into the other 7 bit positions
    for (strobe = 0; strobe < (MAX_STROBE/2); strobe++) {
      lrDimmFeedback = CHIP_FUNC_CALL(host, LrDimmReadTrainFeedback (host, socket, ch, strobe, bit));
      controlWordData[strobe] = controlWordData[strobe]| (UINT8)lrDimmFeedback;
    }
  }

  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
  WriteLrbuf(host, socket, ch, dimm, 0, 0, LRDIMM_F6, LRDIMM_BC6x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0x37, LRDIMM_F7, LRDIMM_BC5x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0, LRDIMM_F7, LRDIMM_BC6x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xb7, LRDIMM_F7, LRDIMM_BC5x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0, LRDIMM_F7, LRDIMM_BC6x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));
}

void MRECWorkaround2Intel (
                          PSYSHOST  host,
                          UINT8     socket,
                          UINT8     ch,
                          UINT8     dimm,
                          UINT8     rank,
                          INT8      cycleSetting
                          )
{
  UINT8                         strobe;
  struct lrMrecTrainingResults  (*lrMrecTrainRes)[MAX_CH][MAX_STROBE];
  struct rankDevice             (*rankStruct)[MAX_RANK_DIMM];
  UINT8                         controlWordData [MAX_STROBE/2];
  UINT8                         TempMprdata[8][MAX_STROBE/2];
  UINT8                         Mprdata[MAX_STROBE];
  UINT8                         trainingmode;
  UINT8                         i,j;
  UINT8                         offset, function;
  UINT8                         F6BC4X[9];

  lrMrecTrainRes = &host->var.mem.lrMrecTrainRes;
  rankStruct  = GetRankStruct(host, socket, ch, dimm);

  trainingmode = (*rankStruct)[rank].CurrentLrdimmTrainingMode;
  //exit MRD mode
  WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
  (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_NORMAL_MODE;

  for (i = 0; i < 8; i++) {
    if (i > 3) {
      function = 6;
      offset = (i - 4) << 4;
    } else {
      function = 5;
      offset = i << 4;
    }
    ReadBCWAsynByDq(host,socket,ch,dimm, 0, TempMprdata[i], function, offset);
  }

  for (i = 0; i < MAX_STROBE; i++) {
    Mprdata[i] = 0;
    for (j = 0; j < 8; j++) {
      if (i < 9) {
        Mprdata[i] = Mprdata[i] | ((TempMprdata[j][i] & 0x01) << j) ;
      } else {
        Mprdata[i] = Mprdata[i] | (((TempMprdata[j][i-9]>>4) & 0x01) << j);
      }
    }
  }

  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
  WriteLrbuf(host, socket, ch, dimm, 0, 0xac, LRDIMM_F7, LRDIMM_BC5x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0x68, LRDIMM_F7, LRDIMM_BC6x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

  ReadBCWAsynByDq(host, socket, ch, dimm, 0, F6BC4X, LRDIMM_F6, LRDIMM_BC4x);

  for (strobe = 0; strobe < (MAX_STROBE/2); strobe++) {
    F6BC4X[strobe] = F6BC4X[strobe] & 0x1f;
  }

  WriteLrbufPBA(host, socket, ch, dimm, rank, F6BC4X, LRDIMM_F6, LRDIMM_BC4x, ALL_DATABUFFERS);

  ReadBCWAsynByDq(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F6, LRDIMM_BC5x);

  for (strobe = 0; strobe < (MAX_STROBE/2); strobe++) {
    if (controlWordData[strobe] == Mprdata[strobe]) {
      (*lrMrecTrainRes)[ch][strobe].results = (*lrMrecTrainRes)[ch][strobe].results | (1 << (cycleSetting + 2));
    }
  }

  for (strobe = 0; strobe < (MAX_STROBE/2); strobe++) {
    F6BC4X[strobe] = (F6BC4X[strobe] & 0x1f) | 0x80;
  }

  WriteLrbufPBA(host, socket, ch, dimm, rank, F6BC4X, LRDIMM_F6, LRDIMM_BC4x, ALL_DATABUFFERS);

  ReadBCWAsynByDq(host,socket,ch,dimm, 0, controlWordData, LRDIMM_F6, LRDIMM_BC5x);

  for (strobe = 0; strobe < (MAX_STROBE/2); strobe++) {
    if (controlWordData[strobe] == Mprdata[strobe+9]) {
      (*lrMrecTrainRes)[ch][strobe+9].results = (*lrMrecTrainRes)[ch][strobe+9].results | (1 << (cycleSetting + 2));
    }
  }

  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
  WriteLrbuf(host, socket, ch, dimm, 0, 0xac, LRDIMM_F7, LRDIMM_BC5x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0x08, LRDIMM_F7, LRDIMM_BC6x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
  CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

  if (trainingmode != LRDIMM_NORMAL_MODE) {
    WriteLrbuf(host, socket, ch, dimm, 0, trainingmode, LRDIMM_F0, LRDIMM_BC0C);
    (*rankStruct)[rank].CurrentLrdimmTrainingMode = trainingmode;
  }
}
#endif // MONTAGE_LRBUF_WA


/**

  Get's the results from the current test

  @param host      - Pointer to sysHost
  @param dimm      - DIMM number
  @param rank      - rank number of the DIMM

  @retval N/A

**/
void
GetLrdimmMrecResults (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT8     dimm,
                     UINT8     rank,
                     INT8      cycleSetting
                     )
{
  UINT8               ch;
  UINT8               strobe;
  UINT8               swizzleStrobe = 0;
  UINT32              lrdimmFeedBack;
#ifdef MONTAGE_LRBUF_WA
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
#endif // MONTAGE_LRBUF_WA
  struct lrMrecTrainingResults  (*lrMrecTrainRes)[MAX_CH][MAX_STROBE];
  UINT8               DumArr[7] = {1,1,1,1,1,1,1};
  UINT32              chBitmask;

  chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
  lrMrecTrainRes      = &host->var.mem.lrMrecTrainRes;

  //
  // enter sense amp training mode
  //
  CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, SENSE_AMP));
  //
  //i.  Execute the CPGC MPR RD sequence
  //
  CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask)) continue;

#ifdef MONTAGE_LRBUF_WA
    dimmNvList = GetDimmNvList (host, socket, ch);

    //
    //M88DDR4 RCD/DB Sighting Report, sighting 1302101
    // montage M88DDR4DB01-A1 mrec workaround for DB A1
    //
    if (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE &&
        (*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_A1) && cycleSetting > -2) {
      MRECWorkaround2Intel(host, socket, ch, dimm, rank, cycleSetting);
    } else
#endif // MONTAGE_LRBUF_WA
    {
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // Skip if this is an ECC strobe when ECC is disabled
        //
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

#ifdef MONTAGE_LRBUF_WA
        if (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE && (*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_A1) && cycleSetting == -2) {
          lrdimmFeedBack = 0;
        } else 
#endif // MONTAGE_LRBUF_WA
        {
            swizzleStrobe = CHIP_FUNC_CALL(host, CheckBacksideSwizzle(host, socket, ch, dimm, strobe));
            lrdimmFeedBack = CHIP_FUNC_CALL(host, LrDimmReadTrainFeedback(host, socket, ch, swizzleStrobe, 0xFF));
        }
        if (lrdimmFeedBack) {
          (*lrMrecTrainRes)[ch][strobe].results = (*lrMrecTrainRes)[ch][strobe].results | (1 << (cycleSetting + 2));
        }
      } // strobe loop
    }
  } // ch loop
  //
  // Exit sense amp training mode
  //
  CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, DISABLE_SENSE_AMP));
} // GetLrdimmMrecResults


/**

  Get's the results from the current test

  @param host      - Pointer to sysHost
  @param dimm      - DIMM number
  @param rank      - rank number of the DIMM

  @retval N/A

**/
void
GetLrdimmCwlResults (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT8     dimm,
                    UINT8     rank,
                    INT8      cycleDelay
                    )
{
  UINT8               ch;
  UINT8               strobe;
  UINT8               swizzleStrobe;
  UINT32              lrdimmFeedBack;
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct lrMrecTrainingResults  (*lrCwlTrainRes)[MAX_CH][MAX_STROBE];
  INT8                resultBitPosition;

  lrCwlTrainRes      = &host->var.mem.lrCwlTrainRes;
  channelNvList = GetChannelNvList(host, socket);
  resultBitPosition = cycleDelay + 2; //(-2, -1, 0, 1, 2 maps to bit 0, 1, 2, 3, 4)

  //
  // enter sense amp training mode here to latch results
  //
  CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, SENSE_AMP));
  FixedDelay(host, 1);


  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

#ifdef MONTAGE_LRBUF_WA
    dimmNvList = GetDimmNvList (host, socket, ch);
#endif

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      swizzleStrobe = CHIP_FUNC_CALL(host, CheckBacksideSwizzle(host, socket, ch, dimm, strobe));
#ifdef MONTAGE_LRBUF_WA
      if (((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE && (*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_A1) && cycleDelay == -2) {
        lrdimmFeedBack = 0;
      } else 
#endif // MONTAGE_LRBUF_WA
      {
        lrdimmFeedBack = CHIP_FUNC_CALL(host, LrDimmReadTrainFeedback (host, socket, ch, swizzleStrobe, 0xFF));
      }
      if (lrdimmFeedBack == 0xF) {
        (*lrCwlTrainRes)[ch][strobe].results = (*lrCwlTrainRes)[ch][strobe].results | (1 << resultBitPosition);
      }
    } // strobe loop
  } // ch loop
  //
  // Exit sense amp training mode
  //
  CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, DISABLE_SENSE_AMP));
} // GetLrdimmCwlResults


/**

  Get's the results from the current test

  @param host              - Pointer to sysHost
  @param dimm              - DIMM number
  @param rank              - rank number of the DIMM
  @param delaySetting      - Encoded Delay MDQS setting (per F[3:0]BC4x register definition in JEDEC Spec)
  @param step              - which training step is this called from, using same value as training mode for convenience

  @retval N/A

**/
void
GetLrdimmMrdMwdResults (
                       PSYSHOST  host,
                       UINT8     socket,
                       UINT8     dimm,
                       UINT8     rank,
                       UINT8     delaySetting,
                       UINT8     step
                       )
{
  UINT8               ch;
  UINT8               strobe;
  UINT8               swizzleStrobe;
  UINT8               dWord;
  UINT32              lrdimmFeedBack;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct TrainingResults  (*lrTrainRes)[MAX_CH][MAX_STROBE];
  UINT8               resultsCodedDelay;

  if (step == LRDIMM_MRD_TRAINING_MODE) {
    lrTrainRes      = &host->var.mem.lrMrdTrainRes;
  } else {
    lrTrainRes      = &host->var.mem.lrMwdTrainRes;
  }
  channelNvList = GetChannelNvList(host, socket);

  //
  // The encoded read delay is such that values 0 to 15d map to settings 0 to +15,
  // and values 16 to 31 map to values 0 to -15 respectively.  However, to find
  // the widest passing region, it is more convient to map the results such that
  // values 0 to 15 maps to settings -15 to 0 and values 16 to 31 maps to values
  // 1 to 16.
  if (delaySetting < 16) {
    resultsCodedDelay = delaySetting + 15;
  } else {
    resultsCodedDelay = delaySetting ^ 31;
  }
  //
  // Putting the results per nibble in a 128bit structure, so every 4 bits = 1 entry
  // and 32 / 4 = 8, thus divide by 8 to get the proper dword for this entry
  //
  dWord = (UINT8)(resultsCodedDelay / 8);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    // Continue to the next rank if this one is disabled
    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      swizzleStrobe = CHIP_FUNC_CALL(host, CheckBacksideSwizzle(host, socket, ch, dimm, strobe));
      lrdimmFeedBack = LrDimmReadTrainFeedback (host, socket, ch, swizzleStrobe, 0xFF);
      if (lrdimmFeedBack == 0xF) {
        (*lrTrainRes)[ch][strobe].results[dWord] = (*lrTrainRes)[ch][strobe].results[dWord] | (0xF << ((resultsCodedDelay % 8) * 4));
      }
    } // strobe loop
  } // ch loop
} // GetLrdimmMrdMwdResults


/**

  Initialize CPGC for LRDIMM Recieve Training


  @param host   - Pointer to sysHost
  @param socket - socket number
  @param chBitmask - mask of channels to be used in this CPGC test

**/
void
SetupLrdimmReadTest(
                   PSYSHOST  host,
                   UINT8     socket,
                   UINT32    chBitmask,
                   UINT8     NumCL,
                   UINT8     loopCount
                   )
{

  TCPGCAddress        CPGCAddress = {{0, 0, 0, 0},   // Start
    {0, 4, 0, 0x1F}, // Stop
    {0, 0, 0, 0},    // Order
    {0, 0, 0, 0},    // IncRate
    {0, 4, 0, 1}};   // IncValue
  // IncRate, Start, Stop, DQPat
  TWDBPattern         WDBPattern  = {1,       0,   0, BasicVA}; // Should not matter what this is. We do not compare data.


  // CmdPat=PatRd, NumCL=input, LC=input, CPGCAddress, SOE=0,
  // WDBPattern, EnCADB=0, EnCKE=0, SubSeqWait=0
  CHIP_FUNC_CALL(host, SetupIOTest(host, socket, chBitmask, PatRd, NumCL, loopCount, &CPGCAddress, 0, &WDBPattern, 0, 0, 0));

} //SetupLrdimmReadTest

/**

  Initialize CPGC for LRDIMM Write Training


  @param host   - Pointer to sysHost
  @param socket - socket number
  @param chBitmask - mask of channels to be used in this CPGC test

**/
void
SetupLrdimmReadWriteTest(
                        PSYSHOST  host,
                        UINT8     socket,
                        UINT32    chBitmask,
                        UINT8     NumCL,
                        UINT8     loopCount
                        )
{
  UINT8     subseqwait=0;
  UINT8     ch;
  struct    channelNvram          (*channelNvList)[MAX_CH];

  //
  // Setup CPGC Pattern
  //
  TCPGCAddress CPGCAddress = {{0, 0, 0, 0},    // Start
    {0, 7, 0, 0x1F}, // Stop
    {0, 0, 0, 0},    // Order
    {0, 0, 0, 0},    // IncRate
    {0, 0, 0, 1}};   // IncValue
  //                       IncRate Start, Stop, DQPat
  TWDBPattern WDBPattern  = {0,  0,  1,  BasicVA};

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    if ((*channelNvList)[ch].ddrtEnabled) {
      subseqwait = 100;
      break;
    }
  }
  // CmdPat=PatRd, NumCL=input, LC=input, CPGCAddress, SOE=0,
  // WDBPattern, EnCADB=0, EnCKE=0, SubSeqWait=0/100
  CHIP_FUNC_CALL(host, SetupIOTest(host, socket, chBitmask, PatWrRd, NumCL, loopCount, &CPGCAddress, NSOE, &WDBPattern, 0, 0, subseqwait));
} //SetupLrdimmReadWriteTest

/**

  Initialize LRDIMM MDQ Receive Enable Cycle Training Parameters

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static
void
LrdimmCycleTrainingInit (
                        PSYSHOST host,
                        UINT8    socket,
                        UINT32   chBitmask,
                        UINT8   dimm,
                        UINT8    rank,
                        UINT8    step
                        )
{
  UINT8                             ch;
  UINT8                             pattern[8] = {0, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0xFF};
  struct channelNvram               (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask)) continue;
    if ((*channelNvList)[ch].enabled == 0) continue;

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

    //
    //Set the rank within the BC08 register.
    //
    WriteLrbuf(host, socket, ch, dimm, 0, rank, LRDIMM_F0, LRDIMM_BC08);

    WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
    //
    // 5. Set BC0C to enable cycle training mode.
    //    Write to BC0C
    //
    WriteLrbuf(host, socket, ch, dimm, 0, step, LRDIMM_F0, LRDIMM_BC0C);
    host->var.mem.socket[socket].channelList[ch].dimmList[dimm].rankStruct[rank].CurrentLrdimmTrainingMode = step;
    //
    // 2. Set the Buffer MPR pattern to a non-periodic time varying pattern
    //   (i.e. 0x2B, or 00101011)
    //
    SetBufferMPRPattern(host, socket, ch, dimm, rank, pattern);
  } //ch
} // LrdimmCycleTrainingInit


/**

  Initialize LRDIMM MPR Buffer with provided pattern

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - channel number
  @param dimm    - dimm number
  @param rank    - rank number
  @param pattern - pointer to an 8 byte array containing pattern values to be written.

  @retval N/A

**/
void
SetBufferMPRPattern (
                    PSYSHOST host,
                    UINT8    socket,
                    UINT8    ch,
                    UINT8    dimm,
                    UINT8    rank,
                    UINT8    *pattern
                    )
{
  UINT8                             index;

  for (index = 0; index < 4; index++) {
    WriteLrbuf(host, socket, ch, dimm, 0, pattern[index], LRDIMM_F5, (LRDIMM_BC0x + (index << 4)));
  }
  for (index = 4; index < 8; index++) {
    WriteLrbuf(host, socket, ch, dimm, 0, pattern[index], LRDIMM_F6, (LRDIMM_BC0x + ((index - 4) << 4)));
  }
}


/**

  Restore LRDIMM parameters changed during training steps

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param dimm    - dimm number (0-based)
  @param rank    - rank number (0-based)

  @retval N/A

**/
void
LrdimmExitRankTraining (
                       PSYSHOST host,
                       UINT8    socket,
                       UINT32   chBitmask,
                       UINT8    dimm,
                       UINT8    rank
                       )
{
  UINT8                             ch;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             controlWordData;

  channelNvList = GetChannelNvList(host, socket);

  SetTrainingMode (host, socket, dimm, rank, CLEAR_MODE);

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask )) continue;
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList (host, socket, ch);
    rankStruct  = GetRankStruct(host, socket, ch, dimm);

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
    //
    //Send MRS commands to place DRAM's out of MPR mode
    //
    SetRankMPR (host, socket, ch, dimm, rank, 0);
    if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
      CHIP_FUNC_CALL(host, SetRankDAMprFnv(host, socket, ch, dimm, 0));
      CHIP_FUNC_CALL(host, SetRankDMprFnv(host, socket, ch, dimm, 0));
    }
    (*rankStruct)[rank].CurrentDramMode = 0;
    //
    //Clear training mode.
    //   Write 0 to BC0C
    //
    WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
    (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_NORMAL_MODE;
    //
    //Disable: read preamble training mode, MPR override, and PBA/PDA mode in the buffer: F0BC1x = 00000000b
    //
    (*dimmNvList)[dimm].lrBuf_BC1x = (*dimmNvList)[dimm].lrBuf_BC1x & LRDIMM_ONE_RANK_TIMING_MODE;
    controlWordData = (*dimmNvList)[dimm].lrBuf_BC1x;
    WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, LRDIMM_F0, LRDIMM_BC1x);

  } //ch
} // LrdimmExitRankTraining


/**

  Initialize LRDIMM MDQ Receive Enable Cycle Training Parameters

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static
void
LrdimmMrdInit (
              PSYSHOST host,
              UINT8    socket,
              UINT32   chBitmask,
              UINT8    dimm,
              UINT8    rank
              )
{
  UINT8                             ch;
  UINT8                             index;
  UINT8                             pattern[8] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask)) continue;
    if ((*channelNvList)[ch].enabled == 0) continue;

    rankStruct  = GetRankStruct(host, socket, ch, dimm);

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
    //
    //enable only this rank with BC07
    //
    WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
    //
    // 6. Set BC0C to enable MRD training mode.
    //    Write 6 to BC0C
    //
    WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_MRD_TRAINING_MODE, LRDIMM_F0, LRDIMM_BC0C);
    (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_MRD_TRAINING_MODE;
    //
    //3. Set the comparison pattern in the Buffer registers (F5BC0x through
    //   F5BC3x and F6BC0x through F6BC3x) to the default (i.e. 10101010)
    //
    for (index = 0; index < 4; index++) {
      WriteLrbuf(host, socket, ch, dimm, 0, pattern[index], LRDIMM_F5, (LRDIMM_BC0x + (index << 4)));
    }
    for (index = 4; index < 8; index++) {
      WriteLrbuf(host, socket, ch, dimm, 0, pattern[index], LRDIMM_F6, (LRDIMM_BC0x + ((index - 4) << 4)));
    }
    //
    //2.  Set the DRAM in MPR mode, with expected pattern programmed in the DRAM's
    //    (i.e. the default 0101 pattern).
    SetRankMPRPattern (host, socket, ch, dimm, rank, MR3_MPR, 0x55);
    (*rankStruct)[rank].CurrentDramMode = MR3_MPR;
    (*rankStruct)[rank].CurrentMpr0Pattern = 0x55;
  } //ch
} // LrdimmMrdInit


/**

  Initialize LRDIMM MDQ-MDQS Write Delay Training Parameters

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
static
void
LrdimmMwdInit (
              PSYSHOST host,
              UINT8    socket,
              UINT32   chBitmask,
              UINT8    dimm,
              UINT8    rank
              )
{
  UINT8                             ch;
  UINT8                             index;
  UINT8                             pattern[8] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!((1 << ch) & chBitmask)) continue;
    if ((*channelNvList)[ch].enabled == 0) continue;

    rankStruct  = GetRankStruct(host, socket, ch, dimm);

    if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
    //
    //enable only this rank with BC07
    //
    WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
    //
    // 2. Setup the Buffer Training Config CW to drive per-bit feedback on
    //    the DQ lanes.
    WriteLrbuf(host, socket, ch, dimm, 0, 1, LRDIMM_F6, LRDIMM_BC4x);
    //
    // 5. Set BC0C to enable MWD training mode.
    //    Write 7 to BC0C
    //
    WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_MWD_TRAINING_MODE, LRDIMM_F0, LRDIMM_BC0C);
    (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_MWD_TRAINING_MODE;
    //
    //3. Set the comparison pattern in the Buffer registers (F5BC0x through
    //   F5BC3x and F6BC0x through F6BC3x) to the default (i.e. 10101010)
    //
    for (index = 0; index < 4; index++) {
      WriteLrbuf(host, socket, ch, dimm, 0, pattern[index], LRDIMM_F5, (LRDIMM_BC0x + (index << 4)));
    }
    for (index = 4; index < 8; index++) {
      WriteLrbuf(host, socket, ch, dimm, 0, pattern[index], LRDIMM_F6, (LRDIMM_BC0x + ((index - 4) << 4)));
    }
  } //ch
} // LrdimmMwdInit


/**

  Perform LRDIMM MDQ Receive Enable Phase Training (MREP)

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
UINT32
MDQReceiveEnablePhaseTraining(
                             PSYSHOST  host,
                             UINT8     socket
                             )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT32                            chBitmask;
  UINT8                             logRank = 0;
  UINT8                             logSubRank = 0;
  UINT32                            status = SUCCESS;
  UINT8                             strobe;
  UINT8                             dWord;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  struct TrainingResults            (*lrTrainRes)[MAX_CH][MAX_STROBE];
  UINT8                             nibblePhaseSetting;
  UINT16                            risingEdge[MAX_CH][MAX_STROBE];
  UINT16                            centerPoint[MAX_CH][MAX_STROBE];
  UINT16                            fallingEdge[MAX_CH][MAX_STROBE];
  UINT16                            pulseWidth[MAX_CH][MAX_STROBE];
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordFunc;
  UINT8                             faultyPartsStatus;

  status = SUCCESS;
  channelNvList = GetChannelNvList(host, socket);
  lrTrainRes  = &host->var.mem.lrTrainRes;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDR4 LRDIMM MDQ RecEn Phase Training\n"));
  CHIP_FUNC_CALL(host, SetAepTrainingMode (host,socket, RX_BACKSIDE_PHASE_TRAINING));
  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, RX_BACKSIDE_PHASE_TRAINING, (UINT16)((socket << 8)|(dimm << 4)|rank)));
      //
      // Create a bit mask of channels that have at least 1 rank present for this dimm and rank
      //
      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (chBitmask == 0) continue;

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;

        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);
        if ((*channelNvList)[ch].encodedCSMode) {
          logSubRank = (rank >> 1);
        } else {
          logSubRank = 0;
        }
        //
        // a) Adjust the CPGC test for the target rank
        //
        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, logSubRank));
        //
        // Clear training results variable for this channel
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          for (dWord = 0; dWord < 4; dWord++) {
            (*lrTrainRes)[ch][strobe].results[dWord] = 0;
          } // dWord loop
        } // strobe loop
      } // ch loop

      //
      // 1. Setup CPGC to execute "infinite" back to back sequence of MPR RD's.
      // This needs to be an "infinite sequence" since the phase is being compared.
      // CPGC enables this with a cacheline_num setting of 0 in the subsequence.
      // The CPGC test can be stopped after the DQ feedback has been read.
      // This is why we can't use this mode for cycle training to find the preamble.
      //
      // input parameters for this function are 8 for Number of cache lines and 0 for loop count
      //
      SetupLrdimmReadTest(host, socket, chBitmask, MREP_CACHELINES, MREP_LOOPCOUNT);
      //
      //3. Setup HSX DDRIO in senseamp training mode, for sampling of DQ lanes.
      //   NOTE: using rank0 VOC for all LRDIMM training
      //   NOTE: will enable and disable sense amp training mode just before and after test to get MC to latch results
      //
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask )) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        rankStruct = GetRankStruct(host, socket, ch, dimm);
        CHIP_FUNC_CALL(host, SetMprTrainMode(host, socket, ch, !(*dimmNvList)[dimm].aepDimmPresent));
        //
        //enable only this rank with BC07
        //
        WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
        //
        //4. Set BC0C to enable MREP training mode.
        //   Write 1 to BC0C
        //
        WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_MREP_TRAINING_MODE, LRDIMM_F0, LRDIMM_BC0C);
        (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_MREP_TRAINING_MODE;
        if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
          SetRankMPRPatternDA(host, socket, ch, dimm, rank, MR3_MPR, 0x55);
        } else {
          SetRankMPRPattern (host, socket, ch, dimm, rank, MR3_MPR, 0x55);
        }
        (*rankStruct)[rank].CurrentDramMode = MR3_MPR;
        (*rankStruct)[rank].CurrentMpr0Pattern = 0x55;
      } // ch loop

      //
      //5.  For each back-side rank [x]:
      //
      // Find the DQ rising edge
      //
      //
      // b) For nibble phase settings of 0 to 64, do the following:
      //
      //MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
      //              "LRDIMM MDQ RecEn Phase Training - Pi Scanning:\n"));

      for (nibblePhaseSetting = LRDIMM_BACKSIDE_PI_START; nibblePhaseSetting < LRDIMM_BACKSIDE_PI_RANGE; nibblePhaseSetting += LRDIMM_BACKSIDE_STEP_SIZE) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask)) continue;

          //
          //i.  Set F[x]BC2x and F[x]BC3x to phase setting in all buffers
          //    x = backside rank 0:3
          WriteLrbuf(host, socket, ch, dimm, 0, (nibblePhaseSetting >> 1), rank, LRDIMM_BC2x);
          WriteLrbuf(host, socket, ch, dimm, 0, (nibblePhaseSetting >> 1), rank, LRDIMM_BC3x);
        } // ch loop

        //
        // For NVM dimms change infinite read to infinite GNT
        //
        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask)) continue;
          dimmNvList = GetDimmNvList(host, socket, ch);
          if ((*dimmNvList)[dimm].aepDimmPresent) {
            CHIP_FUNC_CALL(host, CpgcAdvCmdParity (host, socket, ch, CHIP_FUNC_CALL(host, GetPhyRank(dimm, rank)), 0, GNT, NORMAL_CADB_SETUP));
          }
        }

        //
        // read results from training results registers
        //
        GetLrdimmTrainResults(host, socket, dimm, rank, nibblePhaseSetting, LRDIMM_MREP_TRAINING_MODE);
      } //nibblePhaseSetting loop
      LrdimmExitRankTraining(host, socket, chBitmask, dimm, rank);

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);

        if ((*dimmNvList)[dimm].dimmPresent == 0) {
          host->var.mem.piSettingStopFlag[ch] = 0;
          continue;
        }

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) {
          host->var.mem.piSettingStopFlag[ch] = 0;
          continue;
        }

        if (host->nvram.mem.eccEn) {
          host->var.mem.piSettingStopFlag[ch] = 0x3FFFF;
        } else {
          host->var.mem.piSettingStopFlag[ch] = 0x1FEFF;
        }
      } // ch loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;
        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);

        rankStruct = GetRankStruct(host, socket, ch, dimm);

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "\nSummary: LRDIMM MDQ RcvEn Phase Training\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
          PrintSampleArray (host, socket, ch, dimm, PRINT_ARRAY_TYPE_LRDIMM_RCV_EN_PHASE);
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          EvaluatePiSettingResults (host, socket, ch, dimm, rank, strobe, risingEdge, centerPoint, fallingEdge, pulseWidth,
                                    FPT_PI_LRDIMM_READ_TYPE);

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
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_RCVEN_PHASE_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, RecEnDelay, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            } else {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, strobe, NO_BIT,
                             "LRDIMM training failure!!!\n"));
              //
              // Add this error to the warning log for both correctable and uncorrectable errors.
              //
              host->var.mem.piSettingStopFlag[ch] |= (1 << strobe);
              DisableChannel(host, socket, ch);
              EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_RCVEN_PHASE_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, RecEnDelay, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            }
          }
        } // strobe loop
#ifdef SERIAL_DBG_MSG
        DisplayEdges(host, socket, ch, pulseWidth[ch], fallingEdge[ch], risingEdge[ch], centerPoint[ch], MAX_STROBE);
#endif // SERIAL_DBG_MSG

        //
        //c)  Set F[x]BC2x and F[x]BC3x to the respective settings that resulted in the
        //    first DQ feedback value transitioning from 0 to 1, first rising edge.
        //    An additional offset must be applied to center in the preamble.
        //    The offset should be 32/64 tCK for HSX (1 tCK preamble setting in the DRAM)
        //
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, "Rx Skew Adjust\n"));
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // store the data to write for later use
          //x
          //save off the "true" value in the cached variable

          //Adding a code offset of 5 to trained MREP setting per HSD 5372218
          if (((*dimmNvList)[dimm].SPDRegVen == MFGID_IDT) && ((*dimmNvList)[dimm].lrbufRid == LRBUF_IDT_DB_RID_GPB0)) {
            (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe] =  (UINT8) (((risingEdge[ch][strobe]/2) + 32 + 5) % 64);
          }
          else {
            (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe] =  (UINT8) (((risingEdge[ch][strobe]/2) + 32) % 64);
          }
          (*rankList)[rank].lrBuf_FxBC2x3x[strobe] = (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe];

        }//strobe
        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC2x3x[0];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC2x, ALL_DATABUFFERS);
        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBC2x3x[9];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BC3x, ALL_DATABUFFERS);

        CHIP_FUNC_CALL(host, SetMprTrainMode(host, socket, ch, 0));
      } // ch loop
    } //rank
  } //dimm

  CHIP_FUNC_CALL(host, CadbRcvenTrainingCleanup(host, socket));
  CHIP_FUNC_CALL(host, SetAepTrainingMode (host,socket, DISABLE_TRAINING_STEP));
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  return status;
} //MDQReceiveEnablePhaseTraining


/**

  Perform LRDIMM MDQ Receive Enable Cycle Training (MREC)

  The following option is a change relative to the host-side training algorithm
  for RCVEN, in that there is no search for the preamble on the strobe. Instead,
  a functional read (MRD training mode) needs to be tested to determine when the
  cycle timing is correct. This means there is some dependency on the DQ-DQS
  timing to ensure that the data is correctly captured, even though the DQ-DQS
  timing has not yet been trained (similar scenario to TX WL Coarse).

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval N/A

**/
void
MDQReceiveEnableCycleTraining(
                             PSYSHOST  host,
                             UINT8     socket
                             )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT32                            chBitmask;
  UINT8                             logRank = 0;
  UINT8                             logSubRank = 0;
  UINT8                             strobe;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  struct lrMrecTrainingResults      (*lrMrecTrainRes)[MAX_CH][MAX_STROBE];
  INT8                              nibbleCycleSetting;
  UINT8                             codedNibbleCycleSetting = 0;
#ifdef SERIAL_DBG_MSG
  UINT8                             maxStrobe;
  INT8                              cycleSetting;
#endif // SERIAL_DBG_MSG
  UINT8                             controlWordAddr;
  UINT8                             controlWordData;
  UINT8                             *controlWordDataPtr;
  INT8                              mdqsReadDelay = 0;
  UINT8                             rankBitMask;
  UINT8                             faultyPartsStatus;


  channelNvList = GetChannelNvList(host, socket);
  lrMrecTrainRes  = &host->var.mem.lrMrecTrainRes;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDR4 LRDIMM MDQ RecEn Cycle Training\n"));
  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, RX_BACKSIDE_CYCLE_TRAINING, (UINT16)((socket << 8)|(dimm << 4)|rank)));

      faultyPartsStatus = FPT_NO_ERROR;
      //
      // Create a bit mask of channels that have at least 1 rank present for this dimm and rank
      //
      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (chBitmask == 0) continue;

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

#ifdef MONTAGE_LRBUF_WA
        //M88DDR4 RCD/DB Sighting Report, sighting 1302101
        // montage M88DDR4DB01-A1 mrec workaround for DB A1
        dimmNvList = GetDimmNvList(host, socket, ch);
        if ((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE &&
            (*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_A1) {
          CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
          WriteLrbuf(host, socket, ch, dimm, 0, 0xb0, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x0e, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);

          WriteLrbuf(host, socket, ch, dimm, 0, 0xb1, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x08, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xb0, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x2e, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);

          WriteLrbuf(host, socket, ch, dimm, 0, 0xb0, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x0e, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xb1, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x09, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);

          WriteLrbuf(host, socket, ch, dimm, 0, 0xb0, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x2e, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);

          WriteLrbuf(host, socket, ch, dimm, 0, 0xb0, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x0e, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
          CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));
        }
#endif // MONTAGE_LRBUF_WA

        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);
        if ((*channelNvList)[ch].encodedCSMode) {
          logSubRank = (rank >> 1);
        } else {
          logSubRank = 0;
        }
        //
        //6.  For each back-side rank [x]:
        //
        // a) Adjust the CPGC test for the target rank
        //
        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, logSubRank));
        //
        // Clear training results variable for this channel
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          (*lrMrecTrainRes)[ch][strobe].results = 0;
        } // strobe loop
      } // ch loop

      //
      // Early LRDIMM MDQ Recieve Enable Cycle training step init
      //
      LrdimmCycleTrainingInit(host, socket, chBitmask, dimm, rank, LRDIMM_MRD_TRAINING_MODE);
      //
      // 1. Setup CPGC sequence with a single cacheline RD. RCVEN in buffer will
      //    start, and the duration will be the preamble + BL/2.
      //
      //    parameters are 1 for Number of cache lines and 1 for loop count
      //
      SetupLrdimmReadTest(host, socket, chBitmask, 1, 1);

      for (ch = 0; ch < MAX_CH; ch++) {
        if(!((1 << ch) & chBitmask)) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        rankStruct = GetRankStruct(host, socket, ch, dimm);
        CHIP_FUNC_CALL(host, SetMprTrainMode(host, socket, ch, !(*dimmNvList)[dimm].aepDimmPresent));
        //
        // setup LRDIMM feedback as per bit (F6BC4X, perbit feedback)
        WriteLrbuf(host, socket, ch, dimm, 0, 1, LRDIMM_F6, LRDIMM_BC4x);
        //
        //3.  Set the DRAM in MPR mode, and write the same pattern to the DRAM MPR registers
        //    (as is set in the buffer MPR register)
        //
        SetRankMPRPattern (host, socket, ch, dimm, rank, MR3_MPR, 0x2B);
        SetRankMPRPatternDA (host, socket, ch, dimm, rank, MR3_MPR, 0x2B);
        (*rankStruct)[rank].CurrentDramMode = MR3_MPR;
        (*rankStruct)[rank].CurrentMpr0Pattern = 0x2B;
        //}
      } // ch

      CHIP_FUNC_CALL(host, ToggleTxOnFnv(host, socket, chBitmask, dimm, 1));
      //
      //6.  For each back-side rank [x]:
      //
      // b) For nibble cycle settings of -1 to 2, do the following:
      //
      //MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
      //              "LRDIMM MDQ RecEn Cycle Training \n"));

      for (nibbleCycleSetting = -2; nibbleCycleSetting <= 2; nibbleCycleSetting += 1) {

        //Spec 0.92 compliant changes
        if (nibbleCycleSetting < 0) {
          codedNibbleCycleSetting = 4 + (UINT8)ABS(nibbleCycleSetting);
        } else {
          codedNibbleCycleSetting = (UINT8)nibbleCycleSetting;
        }
        controlWordData = (codedNibbleCycleSetting | (codedNibbleCycleSetting << 4));
        // F0BCCx for backside rank 0, F1BCCx for backside rank 1, F0BCEx for backside rank 2, and F1BCEx for backside rank 3
        controlWordAddr = LRDIMM_BCCx + ((rank >> 1) * 0x20);

        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask)) continue;

          dimmNvList = GetDimmNvList (host, socket, ch);

#ifdef MONTAGE_LRBUF_WA
          //Skip WriteLrbuf if montage, A1 and cycle setting = -2
          if ((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE && (*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_A1 && nibbleCycleSetting == -2) continue;
#endif // MONTAGE_LRBUF_WA
          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, (rank & BIT0) , controlWordAddr);
        } //ch
        SetupLrdimmReadTest(host, socket, chBitmask, 1, 1);
        //
        //ii. For several settings of MDQS read delay (F[x]BC4x and F[x]BC5x)
        //    (i.e. default, +/- 4/64 tCK, +/- 8/64 tCK)
        //
        for (mdqsReadDelay = -8; mdqsReadDelay <= 8; mdqsReadDelay += 4) {
          if (mdqsReadDelay < 0) {
            controlWordData = (UINT8) (ABS(mdqsReadDelay) | BIT4);
          } else {
            controlWordData = (UINT8) mdqsReadDelay;
          }
          for (ch = 0; ch < MAX_CH; ch++) {
            if (!((1 << ch) & chBitmask)) continue;

            WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, rank, LRDIMM_BC4x);
            WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, rank, LRDIMM_BC5x);

            WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
            //
          }
          //ii. Check the DQ feedback from the Buffer comparator, if there's
          //    a pass in the nibble (for any MDQS read delay setting), that
          //    nibble has the correct cycle setting.
          //
          GetLrdimmMrecResults(host, socket, dimm, rank, nibbleCycleSetting);
        } //read delay
      } // nibble cycle

      LrdimmExitRankTraining(host, socket, chBitmask, dimm, rank);
      CHIP_FUNC_CALL(host, ToggleTxOnFnv(host, socket, chBitmask, dimm, 0));
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        rankList = GetRankNvList(host, socket, ch, dimm);
        rankStruct  = GetRankStruct(host, socket, ch, dimm);
        //
        //c.  Set F[x]BC2x and F[x]BC3x to the respective settings that
        //    resulted in at least one passing test per nibble:
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          //
          //Find best coded nibble cycle setting for this phase.
          //The encoded test results assume test run 0 is for -8, test run 1 is for -4, ...,test run 4 is for +8
          //
          switch ((*lrMrecTrainRes)[ch][strobe].results) {
          case BIT0:
            nibbleCycleSetting = -2;
            break;

          case BIT1:
            nibbleCycleSetting = -1;
            break;

          case BIT2:
            nibbleCycleSetting = 0;
            break;

          case BIT3:
            nibbleCycleSetting = 1;
            break;

          case BIT4:
            nibbleCycleSetting = 2;
            break;

          case 0:
          default:
            //
            // Failure, mark this part as faulty
            //
            (*rankList)[rank].faultyParts[strobe] |= FPT_LRDIMM_TRAINING_FAILED;
            if ((*lrMrecTrainRes)[ch][strobe].results) {
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                            "LRDIMM MDQS RecEn Cycle Training - FOUND MULTIPLE PASSING READ CYCLES - bit map = %d\n", (*lrMrecTrainRes)[ch][strobe].results));
            } else {
              MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                            "LRDIMM MDQS RecEn Cycle Training - FAILED PASSING READ CYCLE\n"));
            }
            nibbleCycleSetting = -1; //DEFAULT
            break;
          }

            //spec 0.92 compliant code here
          if (nibbleCycleSetting < 0) {
            codedNibbleCycleSetting = 4 + (UINT8)ABS(nibbleCycleSetting);
          } else {
            codedNibbleCycleSetting = (UINT8)nibbleCycleSetting;
          }
          (*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9] &= (0xF0 >> (4 * (strobe / 9)));
          (*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9] |= (codedNibbleCycleSetting << (4 * (strobe / 9)));
          (*rankStruct)[rank].cachedLrBuf_FxBCCxEx[strobe % 9] = (*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9];
        } // strobe

        // 0.92 spec compliant code
        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBCCxEx[0]);
        controlWordAddr = LRDIMM_BCCx + ((rank >> 1) * 0x20);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, (rank & BIT0) , controlWordAddr, ALL_DATABUFFERS);
      } // ch loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);

#ifdef MONTAGE_LRBUF_WA
        //M88DDR4 RCD/DB Sighting Report, sighting 1302101
        // montage M88DDR4DB01-A1 mrec workaround for DB A1
        if ((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE &&
            (*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_A1) {
          CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
          WriteLrbuf(host, socket, ch, dimm, 0, 0x0f, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x0c, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
          CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));

          FixedDelay(host, 10);

          CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS));
          WriteLrbuf(host, socket, ch, dimm, 0, 0x0f, LRDIMM_F7, LRDIMM_BC5x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0x04, LRDIMM_F7, LRDIMM_BC6x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcd, LRDIMM_F7, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, 0xcc, LRDIMM_F7, LRDIMM_BC4x);
          CHIP_FUNC_CALL(host, MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS));
        }
#endif // MONTAGE_LRBUF_WA

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "\nSummary: LRDIMM RecEn Cycle\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
          PrintMrecResults (host, socket, ch, dimm, 0);
          releasePrintFControl (host);
        }
        // Print rising edges
        if ((checkMsgLevel(host, SDBG_MAX))) {
          if (!(*dimmNvList)[dimm].x4Present) {
            maxStrobe = MAX_STROBE / 2;
          } else {
            maxStrobe = MAX_STROBE;
          }

          getPrintFControl (host);

          PrintLine(host, 76, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

          rcPrintf ((host, "RE: "));
          for (strobe = 0; strobe < maxStrobe; strobe++) {
            //if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
            // Just print out the final cycle settings

            if ((*lrMrecTrainRes)[ch][strobe].results & BIT0) {
              cycleSetting = -2;
            } else if ((*lrMrecTrainRes)[ch][strobe].results & BIT1) {
              cycleSetting = -1;
            } else if ((*lrMrecTrainRes)[ch][strobe].results & BIT2) {
              cycleSetting = 0;
            } else if ((*lrMrecTrainRes)[ch][strobe].results & BIT3) {
              cycleSetting = 1;
            } else if ((*lrMrecTrainRes)[ch][strobe].results & BIT4) {
              cycleSetting = 2;
            } else {
              //
              // Do not log the warning if this is an ECC strobe when ECC is disabled
              //
              if (((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) == 0) {
                EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_RD_RCVEN, socket, ch, dimm, rank, EwlSeverityWarning, strobe, (*lrMrecTrainRes)[ch][strobe].results, RecEnDelayCycle,LrbufLevel, 0xFF);
                cycleSetting = -1;
              } else {
                cycleSetting = 0;
              }
            }
            rcPrintf ((host, " %3d", cycleSetting));
          } // strobe loop
          rcPrintf ((host, "\n"));

          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG

        faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);
        if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
          EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_RCVEN_CYCLE_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, NO_STROBE, NO_BIT, RecEnDelayCycle, LrbufLevel, 0xFF);
        } else if (faultyPartsStatus == FPT_NO_CORRECTABLE_ERROR){
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_RCVEN_CYCLE_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, NO_STROBE, NO_BIT, RecEnDelayCycle, LrbufLevel, 0xFF);
          // Disable the channel with faulty ranks
          DisableChannel(host, socket, ch);
        }
        CHIP_FUNC_CALL(host, SetMprTrainMode(host, socket, ch, 0));
      } // ch loop
    } //rank
    for (ch = 0; ch < MAX_CH; ch++) {
     chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
     if (!((1 << ch) & chBitmask)) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      rankBitMask = 0;
      switch ((*dimmNvList)[dimm].numDramRanks) {
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
      WriteLrbuf(host, socket, ch, dimm, 0, rankBitMask, LRDIMM_F0, LRDIMM_BC07);
    } //ch
  } //dimm
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  return;
} //MDQReceiveEnableCycleTraining


/**

  Perform LRDIMM MDQ Read Delay Training (MRD)

  This training step leverages the comparison capability of the buffer
  MRD Training Mode to measure the margins and determine the best
  Rx strobe (MDQS) delay timing relative to the MDQ signals.

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
UINT32
MDQReadDelayTraining(
                    PSYSHOST  host,
                    UINT8     socket
                    )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             dWord;
  UINT32                            chBitmask;
  UINT8                             logRank = 0;
  UINT8                             logSubRank = 0;
  UINT32                            status = SUCCESS;
  UINT8                             strobe;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  struct TrainingResults            (*lrMrdTrainRes)[MAX_CH][MAX_STROBE];
  UINT8                             DumArr[7] = {1,1,1,1,1,1,1};
  UINT8                             controlWordData;
  INT8                              mdqsReadDelay = 0;
  UINT16                            risingEdge[MAX_CH][MAX_STROBE];
  UINT16                            centerPoint[MAX_CH][MAX_STROBE];
  UINT16                            fallingEdge[MAX_CH][MAX_STROBE];
  UINT16                            pulseWidth[MAX_CH][MAX_STROBE];
  UINT8                             rankBitMask;
  UINT8                             faultyPartsStatus;

  status = SUCCESS;
  channelNvList = GetChannelNvList(host, socket);
  lrMrdTrainRes = &host->var.mem.lrMrdTrainRes;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDR4 LRDIMM MDQ Read Delay Training\n"));
  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      //
      // Create a bit mask of channels that have at least 1 rank present for this dimm and rank
      //
      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      if (chBitmask == 0) continue;

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);

        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);
        if ((*channelNvList)[ch].encodedCSMode) {
          logSubRank = (rank >> 1);
        } else {
          logSubRank = 0;
        }

        CHIP_FUNC_CALL(host, SetMprTrainMode(host, socket, ch, !(*dimmNvList)[dimm].aepDimmPresent));
        //
        // a. Adjust the CPGC test for the target rank
        //
        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, logSubRank));
        //
        // Clear training results variable for this channel
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          for (dWord = 0; dWord < 4; dWord++) {
            (*lrMrdTrainRes)[ch][strobe].results[dWord] = 0;
          }
        } // strobe loop
      } // ch loop

      //
      // Early LRDIMM MDQ Read Delay training step init
      //
      LrdimmMrdInit (host, socket, chBitmask, dimm, rank);

      //
      // 1. Setup CPGC sequence for a single cacheline MPR read.
      //
      SetupLrdimmReadTest(host, socket, chBitmask, 1, 1);

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;
        rankStruct = GetRankStruct(host, socket, ch, dimm);
        //
        //4. Setup the Buffer Training Config CW to drive per-bit feedback on
        //   the DQ lanes.
        WriteLrbuf(host, socket, ch, dimm, 0, 1, LRDIMM_F6, LRDIMM_BC4x);
        //
        // 6. Set BC0C to enable MRD training mode.
        //    Write 6 to BC0C
        //
        WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_MRD_TRAINING_MODE, LRDIMM_F0, LRDIMM_BC0C);
        (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_MRD_TRAINING_MODE;
      } // ch
      //
      //6.  For each back-side rank [x]:
      //
      //MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
      //              "LRDIMM MDQ Read Delay Training \n"));
      //
      // b. Sweep F[x]BC4x and F[x]BC5x from -15/64 to +15/64 in 1/64
      //    increments), for each setting do the following:
      //
      for (mdqsReadDelay = LRDIMM_BACKSIDE_READ_DELAY_START; mdqsReadDelay <= LRDIMM_BACKSIDE_READ_DELAY_END; mdqsReadDelay += LRDIMM_BACKSIDE_READ_DELAY_STEP_SIZE) {
        if (mdqsReadDelay < 0) {
          controlWordData = (UINT8) (ABS(mdqsReadDelay) | BIT4);
        } else {
          controlWordData = (UINT8) mdqsReadDelay;
        }
        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask)) continue;

          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, rank, LRDIMM_BC4x);
          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, rank, LRDIMM_BC5x);
        } //ch
        //
        //i.  Execute the CPGC MPR RD sequence
        //
        CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));
        //
        // 5. Setup HSX DDRIO in senseamp training mode, for sampling of DQ lanes.
        //
        CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, SENSE_AMP));


        //
        //ii. After the CPGC test completes, read the results of the DQ sampling
        //    in the DDRIO, and extract the per nibble composite eye (AND the results).
        //
        GetLrdimmMrdMwdResults(host, socket, dimm, rank, controlWordData, LRDIMM_MRD_TRAINING_MODE);

        CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, DISABLE_SENSE_AMP));

      } // end read delay sweep
      //
      //c.  Set F[x]BC4x and F[x]BC5x to the respective settings that are in the center of the passing region.
      //
      LrdimmExitRankTraining(host, socket, chBitmask, dimm, rank);

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        if ((*dimmNvList)[dimm].dimmPresent == 0) {
          host->var.mem.piSettingStopFlag[ch] = 0;
          continue;
        }

        if (host->nvram.mem.eccEn) {
          host->var.mem.piSettingStopFlag[ch] = 0x3FFFF;
        } else {
          host->var.mem.piSettingStopFlag[ch] = 0x1FEFF;
        }
      } // ch loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        //
        //c.  Set F[x]BC4x and F[x]BC5x to the respective settings that are in the center of the passing region.
        //
#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "\nSummary: LRDIMM Rd MDQS Delay\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
          PrintSampleArray (host, socket, ch, dimm, PRINT_ARRAY_TYPE_LRDIMM_MDQ_RD_DELAY);
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          status = SUCCESS;
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          //TODO: qualify duty cycle pass / fail
          EvaluatePiSettingResults (host, socket, ch, dimm, rank, strobe, risingEdge, centerPoint, fallingEdge, pulseWidth,
                                    FPT_PI_LRDIMM_RD_MRD_TYPE);
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
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_READ_DELAY_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            } else {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, strobe, NO_BIT,
                             "LRDIMM training failure!!!\n"));
              //
              // Add this error to the warning log for both correctable and uncorrectable errors.
              //
              host->var.mem.piSettingStopFlag[ch] |= (1 << strobe);
              DisableChannel(host, socket, ch);
              EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_READ_DELAY_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            }
          }
        } // strobe loop
#ifdef SERIAL_DBG_MSG
        DisplayLrdimmMrdEdges(host, socket, ch, pulseWidth[ch], fallingEdge[ch], risingEdge[ch], centerPoint[ch], MAX_STROBE);
#endif // SERIAL_DBG_MSG

        SetLrdimmMrdResults(host, socket, ch, dimm, rank, centerPoint[ch]);

        CHIP_FUNC_CALL(host, SetMprTrainMode(host, socket, ch, 0));
      } // ch loop
    } //rank

    for (ch = 0; ch < MAX_CH; ch++) {
     chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
     if (!((1 << ch) & chBitmask)) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      rankBitMask = 0;
      switch ((*dimmNvList)[dimm].numDramRanks) {
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
      WriteLrbuf(host, socket, ch, dimm, 0, rankBitMask, LRDIMM_F0, LRDIMM_BC07);
    } //ch
  } //dimm
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  return status;
} //MDQReadDelayTraining


/**

  Perform LRDIMM DRAM Interface WL (DWL):

  This training step leverages the buffer DWL Training Mode to determine the
  best Tx strobe (MDQS) strobe alignment to the DRAM clock.

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
UINT32
DramInterfaceWlTraining(
                       PSYSHOST  host,
                       UINT8     socket
                       )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT32                            chBitmask;
  UINT8                             rankBitMask;
  UINT32                            status = SUCCESS;
  UINT8                             strobe;
  UINT8                             dWord;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  struct TrainingResults            (*lrDwlTrainRes)[MAX_CH][MAX_STROBE];
  UINT16                            risingEdge[MAX_CH][MAX_STROBE];
  UINT16                            centerPoint[MAX_CH][MAX_STROBE];
  UINT16                            fallingEdge[MAX_CH][MAX_STROBE];
  UINT16                            pulseWidth[MAX_CH][MAX_STROBE];
  UINT8                             *controlWordDataPtr;
  UINT8                             controlWordFunc;
  UINT8                             pi;
  UINT8                             faultyPartsStatus;

  status = SUCCESS;
  channelNvList = GetChannelNvList(host, socket);
  lrDwlTrainRes      = &host->var.mem.lrDwlTrainRes;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDR4 LRDIMM DRAM WL Training\n"));
  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, TX_BACKSIDE_FINE_WL_TRAINING, (UINT16)((socket << 8)|(dimm << 4)|rank)));
      //
      // Create a bit mask of channels that have at least 1 rank present for this dimm and rank
      //
      chBitmask = GetChBitmaskLrdimm(host, socket, dimm, rank);
      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (chBitmask == 0) continue;

      for (ch = 0; ch < MAX_CH; ch++) {

        //
        // Clear training results variable for this channel
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          for (dWord = 0; dWord < 4; dWord++) {
            (*lrDwlTrainRes)[ch][strobe].results[dWord] = 0;
          } // dWord loop
        } // strobe loop
      } // ch loop
      //

      SetupLrdimmReadTest(host, socket, chBitmask, 1, 1);

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        rankStruct = GetRankStruct(host, socket, ch, dimm);

        //
        // setup LRDIMM feedback as per bit (F6BC4X, perbit feedback)
        //
        WriteLrbuf(host, socket, ch, dimm, 0, 1, LRDIMM_F6, LRDIMM_BC4x);
        //
        //2. Set the buffer into DWL training mode in the BC0C register.
        //   Once this is done, the buffer will send continuous stream of strobes
        //   to the DRAM, with the timing associated with the selected rank's WL
        //   control words. The DRAM will send DQ feedback indicating the CK sample
        //   in the DRAM. This DQ feedback will be propogated to the host.
        //
        //   Write 4 to BC0C
        //
        WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_DWL_TRAINING_MODE, LRDIMM_F0, LRDIMM_BC0C);
        (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_DWL_TRAINING_MODE;
      } //ch
      //
      //3.  For each back-side rank [x]:
      //
      // a. Set the target DRAMs in WL mode, with adjustments to RTT_PARK
      //    (i.e. copy RTT_WR to RTT_PARK, need to make sure host is overriding
      //    the ODT signals in the same way as for Host interface WL) as needed.
      //    Leave all other rank DRAM's in normal mode.
      //
      OverrideRttNom(host, socket, dimm, rank);
      CHIP_FUNC_CALL(host, ToggleInternalClocksOnFnv(host, socket, chBitmask, dimm, 1));
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        //
        //b. Set the rank within the BC08 and BC07 registers
        //
        WriteLrbuf(host, socket, ch, dimm, 0, rank, LRDIMM_F0, LRDIMM_BC08);
        WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
      } // ch

      //MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
      //              "LRDIMM DRAM Interface WL - Pi Scanning:\n"));
      //
      //c. Sweep F[x]BCAx and F[x]BCBx Phase Control in 1/64 increments),
      //   for each setting do the following:
      //
      for (pi = LRDIMM_BACKSIDE_PI_START; pi < LRDIMM_BACKSIDE_PI_RANGE; pi += LRDIMM_BACKSIDE_STEP_SIZE) {
        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask)) continue;
          // set all strobes to the pi setting
          WriteLrbuf(host, socket, ch, dimm, 0, pi >> 1, rank, LRDIMM_BCAx);
          WriteLrbuf(host, socket, ch, dimm, 0, pi >> 1, rank, LRDIMM_BCBx);

          //
          // Inphi A1 WA
          //
          dimmNvList = GetDimmNvList(host, socket, ch);
          if (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) &&
              ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A1)) {
            InphiPIWA(host, socket, ch, dimm);
          }

          WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
        } //ch
        //
        //a. Capture the DQ feedback values in the host
        //
        GetLrdimmTrainResults(host, socket, dimm, rank, pi, LRDIMM_DWL_TRAINING_MODE);
      }
      //
      // fix ODT
      //
      CHIP_FUNC_CALL(host, ToggleInternalClocksOnFnv(host, socket, chBitmask, dimm, 0));
      RestoreRttNom(host, socket, dimm, rank);
      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        rankStruct    = GetRankStruct(host, socket, ch, dimm);
        //
        //set DRAMS back to normal mode
        //   Write 0 to BC0C
        //
        WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
        (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_NORMAL_MODE;
      } // ch loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        if ((*dimmNvList)[dimm].dimmPresent == 0) {
          host->var.mem.piSettingStopFlag[ch] = 0;
          continue;
        }

        rankList = GetRankNvList(host, socket, ch, dimm);

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) {
          host->var.mem.piSettingStopFlag[ch] = 0;
          continue;
        }

        if (host->nvram.mem.eccEn) {
          host->var.mem.piSettingStopFlag[ch] = 0x3FFFF;
        } else {
          host->var.mem.piSettingStopFlag[ch] = 0x1FEFF;
        }
      } // ch loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);

        // Continue to the next rank if this one is disabled
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        rankStruct    = GetRankStruct(host, socket, ch, dimm);

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "\nSummary: LRDIMM DRAM WL Phase\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
          PrintSampleArray (host, socket, ch, dimm, PRINT_ARRAY_TYPE_LRDIMM_WL_PHASE);
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          //
          //d. Set F[x]BCAx and F[x]BCBx Phase Control to the respective
          //   settings that are aligned to the rising edge of the CK.
          //
          EvaluatePiSettingResults (host, socket, ch, dimm, rank, strobe, risingEdge, centerPoint, fallingEdge, pulseWidth,
                                    FPT_PI_LRDIMM_WRITE_TYPE);

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
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_WL_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            } else {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, strobe, NO_BIT,
                             "LRDIMM training failure!!!\n"));
              //
              // Add this error to the warning log for both correctable and uncorrectable errors.
              //
              host->var.mem.piSettingStopFlag[ch] |= (1 << strobe);
              DisableChannel(host, socket, ch);
              EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_WL_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            }
          }
        } // strobe loop
#ifdef SERIAL_DBG_MSG
        DisplayEdges(host, socket, ch, pulseWidth[ch], fallingEdge[ch], risingEdge[ch], centerPoint[ch], MAX_STROBE);
#endif // SERIAL_DBG_MSG

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // store the data to write for later use
          //
          (*rankList)[rank].lrBuf_FxBCAxBx[strobe] = (((risingEdge[ch][strobe]) % 128) >> 1);
          (*rankStruct)[rank].cachedLrBuf_FxBCAxBx[strobe] = (*rankList)[rank].lrBuf_FxBCAxBx[strobe];
        } // strobe
        //
        //d. Set F[x]BCAx and F[x]BCBx Phase Control to the respective settings
        //   that are aligned to the rising edge of the CK.
        //
        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBCAxBx[0];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BCAx, ALL_DATABUFFERS);
        controlWordDataPtr = &(*rankList)[rank].lrBuf_FxBCAxBx[9];
        controlWordFunc = rank;
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, controlWordFunc, LRDIMM_BCBx, ALL_DATABUFFERS);
        //
        // Inphi A1 WA
        //
        if (((*dimmNvList)[dimm].SPDRegVen == MFGID_INPHI) &&
            ((*dimmNvList)[dimm].lrbufRid == LRBUF_INPHI_RID_A1)) {
          InphiPIWA(host, socket, ch, dimm);
        }
        WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
      } // ch loop
    } //rank


    for (ch = 0; ch < MAX_CH; ch++) {
     chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
     if (!((1 << ch) & chBitmask)) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      rankBitMask = 0;
      switch ((*dimmNvList)[dimm].numDramRanks) {
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
      WriteLrbuf(host, socket, ch, dimm, 0, rankBitMask, LRDIMM_F0, LRDIMM_BC07);
    } // ch

  } //dimm
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  return status;
} //DramInterfaceWlTraining


/**

  Perform LRDIMM DRAM Interface WL (DWL):

  This training step leverages the comparison capability of the buffer MWD
  Training Mode to determine the best Tx strobe (MDQS) cycle alignment for
  the associated CWL setting and DIMM routing.

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
void
MDQCoarseWlTraining(
                   PSYSHOST  host,
                   UINT8     socket
                   )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT32                            chBitmask;
  UINT8                             logRank = 0;
  UINT8                             logSubRank = 0;
  UINT8                             strobe;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  struct lrMrecTrainingResults      (*lrCwlTrainRes)[MAX_CH][MAX_STROBE];
#ifdef SERIAL_DBG_MSG
  UINT8                             maxStrobe;
#endif // SERIAL_DBG_MSG
  UINT8                             controlWordData;
  UINT8                             controlWordAddr;
  UINT8                             *controlWordDataPtr;
  INT8                              cycleDelay;
  UINT8                             codedDelayCycleSetting = 0;
  UINT8                             DumArr[7] = {1,1,1,1,1,1,1};
  UINT8                             pattern[12] = {0, 0xFF, 0xFF, 0, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0xFF,0};
  UINT8                             rankBitMask;
  UINT8                             faultyPartsStatus;

  channelNvList = GetChannelNvList(host, socket);
  lrCwlTrainRes = &host->var.mem.lrCwlTrainRes;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDR4 LRDIMM DRAM Coarse WL Training\n"));
  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      OutputExtendedCheckpoint((host, STS_CHANNEL_TRAINING, TX_BACKSIDE_COARSE_WL_TRAINING, (UINT16)((socket << 8)|(dimm << 4)|rank)));

      faultyPartsStatus = FPT_NO_ERROR;
      //
      // Create a bit mask of channels that have at least 1 rank present for this dimm and rank
      //
      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (chBitmask == 0) continue;

      MemSetLocal((UINT8 *) lrCwlTrainRes, 0, sizeof(*lrCwlTrainRes));
      //
      //4. Setup CPGC to execute single cacheline write/read loopback test
      //
      SetupLrdimmReadWriteTest(host, socket, chBitmask, 1, 1);

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;
        //
        //2. Program the Buffer Training Configuration Control Word
        //   for per-bit feedback (F6BC4x).
        //
        WriteLrbuf(host, socket, ch, dimm, 0, 1, LRDIMM_F6, LRDIMM_BC4x);
        //
        //Set the rank within the BC08 register.
        //
        WriteLrbuf(host, socket, ch, dimm, 0, rank, LRDIMM_F0, LRDIMM_BC08);
        WriteLrbuf(host, socket, ch, dimm, 0, (0xF & ~(1 << rank)), LRDIMM_F0, LRDIMM_BC07);
        //
        //7. For each back-side rank [x]:
        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);
        if ((*channelNvList)[ch].encodedCSMode) {
          logSubRank = (rank >> 1);
        } else {
          logSubRank = 0;
        }
        //
        // a. Adjust the CPGC test for the target rank.
        //
        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, logSubRank));
      } // ch
      //MemDebugPrint((host, SDBG_MAX, socket, NO_CH, dimm, rank, NO_STROBE, NO_BIT,
      //              "LRDIMM MDQS Coarse WL - Cycle Scanning:\n"));
      //
      // b. Sweep F[x]BCAx and F[x]BCBx cycle delays from 2 down to -1.
      //    For each setting do the following:
      //
      for (cycleDelay = 2; cycleDelay >= -2; cycleDelay--) {
        //Spec 0.92 compliant code here
        if (cycleDelay < 0) {
          codedDelayCycleSetting = 4 + (UINT8)ABS(cycleDelay);
        } else {
          codedDelayCycleSetting = (UINT8)cycleDelay;
        }
        controlWordData = (codedDelayCycleSetting | (codedDelayCycleSetting << 4));

        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask)) continue;

          dimmNvList = GetDimmNvList(host, socket, ch);
          rankList = GetRankNvList(host, socket, ch, dimm);
          rankStruct  = GetRankStruct(host, socket, ch, dimm);

#ifdef MONTAGE_LRBUF_WA
          //Skip WriteLrbuf if montage, A1 and cycle setting = -2
          if ((*dimmNvList)[dimm].SPDRegVen == MFGID_MONTAGE && (*dimmNvList)[dimm].lrbufRid == LRBUF_MONTAGE_RID_A1 && cycleDelay == -2) continue;
#endif // MONTAGE_LRBUF_WA

          // F0BCDx for backside rank 0, F1BDCx for backside rank 1, F0BCFx for backside rank 2, and F1BCFx for backside rank 3
          controlWordAddr = LRDIMM_BCDx + ((rank >> 1) * 0x20);
          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, (rank & BIT0) , controlWordAddr);
          //
          // change the buffer pattern for each cycle
          //
          SetBufferMPRPattern(host, socket, ch, dimm, rank, &pattern[cycleDelay + 2]);
          //
          //5. Set the buffer into MWD training mode in the BC0C register.
          //   Write 7 to BC0C
          //
          WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_MWD_TRAINING_MODE, LRDIMM_F0, LRDIMM_BC0C);
          (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_MWD_TRAINING_MODE;
        } //ch
        //
        // c. Per nibble, determine the passing cycle setting
        // (and store away in the results structure for later).
        //
        CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));
        //
        //
        GetLrdimmCwlResults(host, socket, dimm, rank, cycleDelay);

        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask )) continue;

          rankStruct  = GetRankStruct(host, socket, ch, dimm);

          //
          //set DRAMS back to normal mode
          //   Write 0 to BC0C
          //
          WriteLrbuf(host, socket, ch, dimm, 0, LRDIMM_NORMAL_MODE, LRDIMM_F0, LRDIMM_BC0C);
          (*rankStruct)[rank].CurrentLrdimmTrainingMode = LRDIMM_NORMAL_MODE;
          //
          // dummy MRS write
          //
          WriteMRS (host, socket, ch, dimm, rank,                       0, BANK3);   // MR3 = 0
        } // ch loop

        CHIP_FUNC_CALL(host, IODdrtReset(host, socket, dimm));
        CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));

      } // cycleDelay loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        rankList = GetRankNvList(host, socket, ch, dimm);
        rankStruct  = GetRankStruct(host, socket, ch, dimm);
        dimmNvList = GetDimmNvList(host, socket, ch);

#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "\nSummary: LRDIMM MDQS Coarse WL Cycle\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
          PrintMrecResults(host, socket, ch, dimm, 1);
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG

// Gather per strobe results
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
          //Spec v0.92 compliant code here
          if ((*lrCwlTrainRes)[ch][strobe].results & BIT0) {
            codedDelayCycleSetting = 6 << (4 * (strobe / 9));
          } else if ((*lrCwlTrainRes)[ch][strobe].results & BIT1) {
            codedDelayCycleSetting = 5 << (4 * (strobe / 9));
          } else if ((*lrCwlTrainRes)[ch][strobe].results & BIT2) {
            codedDelayCycleSetting = 0 << (4 * (strobe / 9));
          } else if ((*lrCwlTrainRes)[ch][strobe].results & BIT3) {
            codedDelayCycleSetting = 1 << (4 * (strobe / 9));
          } else if ((*lrCwlTrainRes)[ch][strobe].results & BIT4) {
            codedDelayCycleSetting = 2 << (4 * (strobe / 9));
          } else {
          //
          // Failure, mark this part as faulty
          //
          (*rankList)[rank].faultyParts[strobe] |= FPT_LRDIMM_TRAINING_FAILED;
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "LRDIMM MDQS Coarse WL - FAILED WRITE CYCLE\n"));
            codedDelayCycleSetting = 0;
          }
          (*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9] &= 0xF0 >> (4 * (strobe / 9));
          (*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9] |= codedDelayCycleSetting;
          (*rankStruct)[rank].cachedLrBuf_FxBCDxFx[strobe % 9] = (*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9];
        } //strobe

#ifdef SERIAL_DBG_MSG
        // Print rising edges
        if ((checkMsgLevel(host, SDBG_MAX))) {
          if (!(*dimmNvList)[dimm].x4Present) {
            maxStrobe = MAX_STROBE / 2;
          } else {
            maxStrobe = MAX_STROBE;
          }

          getPrintFControl (host);

          PrintLine(host, 76, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);

          rcPrintf ((host, "RE: "));
          for (strobe = 0; strobe < maxStrobe; strobe++) {
              //0.92 compliant results
              if ((((*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9] >> (4 * (strobe / 9))) & 0xF) > 4) {
                rcPrintf ((host, " %3d", 4-(((*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9] >> (4 * (strobe / 9))) & 0xF)));
              } else {
                rcPrintf ((host, " %3d", (((*rankList)[rank].lrBuf_FxBCDxFx[strobe % 9] >> (4 * (strobe / 9))) & 0xF)));
              }
          } // strobe loop
          rcPrintf ((host, "\n"));

          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG

        // Make sure CWL is 0
        controlWordAddr = LRDIMM_BCDx + ((rank >> 1) * 0x20);
        controlWordData = 0;
        WriteLrbuf (host, socket, ch, dimm, 0, controlWordData, (rank & BIT0), controlWordAddr);

        // F0BCDx for backside rank 0, F1BCDx for backside rank 1, F0BCFx for backside rank 2, and F1BCFx for backside rank 3
        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBCDxFx[0]);
        controlWordAddr = LRDIMM_BCDx + ((rank >> 1) * 0x20);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, (rank & BIT0) , controlWordAddr, ALL_DATABUFFERS);

        faultyPartsStatus = EvaluateFaultyParts(host, socket, ch);
        if (faultyPartsStatus == FPT_CORRECTABLE_ERROR) {
          EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_COARSE_WL_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, 0xFF);
        } else if (faultyPartsStatus == FPT_NO_CORRECTABLE_ERROR){
          EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_COARSE_WL_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, 0xFF);
          DisableRank (host, socket, ch, dimm, rank);
          //
          // if we are in encoded mode, disable ranks that share this rank (TODO: fix up for 3DS)
          //
          if ((*channelNvList)[ch].encodedCSMode) {
            DisableRank (host, socket, ch, dimm, ((rank + 2) & 0x3));
          }
        }
      } // ch loop
    } //rank
    for (ch = 0; ch < MAX_CH; ch++) {
     chBitmask = GetChBitmaskLrdimm (host, socket, dimm, 0);
     if (!((1 << ch) & chBitmask)) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      rankBitMask = 0;
      switch ((*dimmNvList)[dimm].numDramRanks) {
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
      WriteLrbuf(host, socket, ch, dimm, 0, rankBitMask, LRDIMM_F0, LRDIMM_BC07);

    } //ch

  } //dimm
  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  return;
} //MDQCoarseWlTraining

/**

  Perform LRDIMM MDQ MDQS Write Delay Training (MWD)

  This training step leverages the comparison capability of the buffer
  MWD Training Mode to determine the best Tx data (MDQ) phase alignment
  to the strobes (MDQS).

  @param host   - Pointer to sysHost
  @param socket - socket number

  @retval status

**/
UINT32
MDQWriteDelayTraining(
                     PSYSHOST  host,
                     UINT8     socket
                     )
{
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             dWord;
  UINT32                            chBitmask;
  UINT8                             logRank = 0;
  UINT8                             logSubRank = 0;
  UINT32                            status = SUCCESS;
  UINT8                             strobe;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct TrainingResults            (*lrMwdTrainRes)[MAX_CH][MAX_STROBE];
  UINT8                             DumArr[7] = {1,1,1,1,1,1,1};
  UINT8                             controlWordData;
  INT8                              mdqsWriteDelay = 0;
  UINT16                            risingEdge[MAX_CH][MAX_STROBE];
  UINT16                            centerPoint[MAX_CH][MAX_STROBE];
  UINT16                            fallingEdge[MAX_CH][MAX_STROBE];
  UINT16                            pulseWidth[MAX_CH][MAX_STROBE];
  UINT8                             faultyPartsStatus;

  status = SUCCESS;
  channelNvList = GetChannelNvList(host, socket);
  lrMwdTrainRes = &host->var.mem.lrMwdTrainRes;

  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DDR4 LRDIMM MDQ Write Delay Training\n"));
  //
  // Train each DIMM
  //
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Train each rank
    //
    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      //
      // Create a bit mask of channels that have atleast 1 rank present for this dimm and rank
      //
      chBitmask = GetChBitmaskLrdimm (host, socket, dimm, rank);
      //
      // Go to the next rank if this rank is not present on any channels
      //
      if (chBitmask == 0) continue;

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);
        if ((*channelNvList)[ch].encodedCSMode) {
          logSubRank = (rank >> 1);
        } else {
          logSubRank = 0;
        }
        //
        // a. Adjust the CPGC test for the target rank
        //
        CHIP_FUNC_CALL(host, SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, logSubRank));
        //
        // Clear training results variable for this channel
        //
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          for (dWord = 0; dWord < 4; dWord++) {
            (*lrMwdTrainRes)[ch][strobe].results[dWord] = 0;
          }
        } // strobe loop
      } // ch loop

      //
      // Early LRDIMM MDQ-MDQS Write Delay training step init
      //
      LrdimmMwdInit (host, socket, chBitmask, dimm, rank);

      //
      //4. Setup CPGC to execute single cacheline write/read loopback test
      //
      SetupLrdimmReadWriteTest(host, socket, chBitmask, 1, 1);

      //
      //6.  For each back-side rank [x]:
      //
      // b. Sweep F[x]BC8x and F[x]BC9x from -15/64 to +15/64 in 1/64
      //    increments), for each setting do the following:
      //
      for (mdqsWriteDelay = LRDIMM_BACKSIDE_WRITE_DELAY_START; mdqsWriteDelay <= LRDIMM_BACKSIDE_WRITE_DELAY_END; mdqsWriteDelay += LRDIMM_BACKSIDE_WRITE_DELAY_STEP_SIZE) {
        if (mdqsWriteDelay < 0) {
          controlWordData = (UINT8) (ABS(mdqsWriteDelay) | BIT4);
        } else {
          controlWordData = (UINT8) mdqsWriteDelay;
        }
        for (ch = 0; ch < MAX_CH; ch++) {
          if (!((1 << ch) & chBitmask)) continue;
          dimmNvList = GetDimmNvList(host, socket, ch);

          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, rank, LRDIMM_BC8x);
          WriteLrbuf(host, socket, ch, dimm, 0, controlWordData, rank, LRDIMM_BC9x);
        } //ch
        //
        //i.  Run the CPGC write/read loopback test
        //
        CHIP_FUNC_CALL(host, RunIOTest(host, socket, chBitmask, BasicVA, DumArr, 1, 0));
        //
        // 3. Setup HSX DDRIO in senseamp training mode, for sampling of DQ lanes.
        //
        CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, SENSE_AMP));

        //
        //ii. After the CPGC test completes, read the results of the DQ sampling
        //    in the DDRIO, and extract the per nibble composite eye (AND the results).
        //
        GetLrdimmMrdMwdResults(host, socket, dimm, rank, controlWordData, LRDIMM_MWD_TRAINING_MODE);

        CHIP_FUNC_CALL(host, SetTrainingMode (host, socket, dimm, rank, DISABLE_SENSE_AMP));

      } // end read delay sweep
      //
      //c.  Set F[x]BC4x and F[x]BC5x to the respective settings that are in the center of the passing region.
      //
      LrdimmExitRankTraining(host, socket, chBitmask, dimm, rank);

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        if ((*dimmNvList)[dimm].dimmPresent == 0) {
          host->var.mem.piSettingStopFlag[ch] = 0;
          continue;
        }

        if (host->nvram.mem.eccEn) {
          host->var.mem.piSettingStopFlag[ch] = 0x3FFFF;
        } else {
          host->var.mem.piSettingStopFlag[ch] = 0x1FEFF;
        }
      } // ch loop

      for (ch = 0; ch < MAX_CH; ch++) {
        if (!((1 << ch) & chBitmask)) continue;

        dimmNvList = GetDimmNvList(host, socket, ch);

        //
        //c.  Set F[x]BC8x and F[x]BC9x to the respective settings that are in the center of the passing region.
        //
#ifdef SERIAL_DBG_MSG
        if (checkMsgLevel(host, SDBG_MAX)) {
          getPrintFControl (host);
          rcPrintf ((host, "\nSummary: LRDIMM MDQ Wr Delay\nS%d, Ch%d, DIMM%d, Rank%d\n", socket, ch, dimm, rank));
          PrintSampleArray (host, socket, ch, dimm, PRINT_ARRAY_TYPE_LRDIMM_MDQ_WR_DELAY);
          releasePrintFControl (host);
        }
#endif // SERIAL_DBG_MSG
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          //
          // Skip if this is an ECC strobe when ECC is disabled
          //
          status = SUCCESS;
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          //TODO: qualify duty cycle pass / fail
          EvaluatePiSettingResults (host, socket, ch, dimm, rank, strobe, risingEdge, centerPoint, fallingEdge, pulseWidth,
                                    FPT_PI_LRDIMM_WR_MRD_TYPE);

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
              EwlOutputType2(host, WARN_FPT_CORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_WRITE_DELAY_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            } else {
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, strobe, NO_BIT,
                             "LRDIMM training failure!!!\n"));
              //
              // Add this error to the warning log for both correctable and uncorrectable errors.
              //
              host->var.mem.piSettingStopFlag[ch] |= (1 << strobe);
              DisableChannel(host, socket, ch);
              EwlOutputType2(host, WARN_FPT_UNCORRECTABLE_ERROR, WARN_FPT_MINOR_LRDIMM_WRITE_DELAY_TRAINING, socket, ch, dimm, rank, EwlSeverityWarning, strobe, NO_BIT, GsmGtDelim, LrbufLevel, (UINT8)pulseWidth[ch][strobe]);
            }
          }
        } // strobe loop
#ifdef SERIAL_DBG_MSG
        DisplayLrdimmMrdEdges(host, socket, ch, pulseWidth[ch], fallingEdge[ch], risingEdge[ch], centerPoint[ch], MAX_STROBE);
#endif // SERIAL_DBG_MSG
        SetLrdimmMwdResults(host, socket, ch, dimm, rank, centerPoint[ch]);
      } // ch loop
    } //rank
  } //dimm

  CHIP_FUNC_CALL(host, FifoTrainReset(host, socket));
  return status;
} //MDQWriteDelayTraining


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
PrintMrecResults (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT8    dimm,
                 UINT8    ReadMode
                 )
{
  UINT8   strobe;
  UINT8   maxStrobe;
  INT8    CycleSetting;
  struct lrMrecTrainingResults (*lrMrecTrainRes)[MAX_CH][MAX_STROBE];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  lrMrecTrainRes    = &host->var.mem.lrMrecTrainRes;
  if (ReadMode == 1) {
    lrMrecTrainRes    = &host->var.mem.lrCwlTrainRes;
  }

  dimmNvList  = GetDimmNvList(host, socket, ch);

  if ((ReadMode == 0) && !(*dimmNvList)[dimm].x4Present) {
    PrintLine(host, 39, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
    rcPrintf ((host, "       0   1   2   3   4   5   6   7   8\n"));
    maxStrobe = MAX_STROBE / 2;
  } else {
    PrintLine(host, 85, SINGLE_LINE, NOCRLF_FLAG, CRLF_FLAG);
    rcPrintf ((host, "       0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17\n"));
    maxStrobe = MAX_STROBE;
  }

  for (CycleSetting = -2; CycleSetting < 3; CycleSetting++) {
    rcPrintf ((host, "%2d  ", CycleSetting));

    for (strobe = 0; strobe < maxStrobe; strobe++) {
      rcPrintf ((host, "   "));
      if (((1 << (CycleSetting + 2)) & (*lrMrecTrainRes)[ch][strobe].results ) != 0) {
        rcPrintf ((host, "1"));
      } else {
        rcPrintf ((host, "0"));
      }
    } // strobe loop

    rcPrintf ((host, "\n"));
  } // CycleSetting loop
} // PrintMrecResults

#endif // SERIAL_DBG_MSG

void
InphiPIWA (
  PSYSHOST host,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm
)
{
  FixedDelay(host, 1);
  WriteLrbuf(host, socket, ch, dimm, 0, 0x03, LRDIMM_F7, LRDIMM_BC4x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0x03, LRDIMM_F0, LRDIMM_BC8x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0x00, LRDIMM_F0, LRDIMM_BC8x);
  WriteLrbuf(host, socket, ch, dimm, 0, 0x00, LRDIMM_F7, LRDIMM_BC4x);
  FixedDelay(host, 1);
} // InphiPIWA

#endif // LRDIMM_SUPPORT
