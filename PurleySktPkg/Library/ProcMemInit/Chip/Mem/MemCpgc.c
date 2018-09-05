/**

Copyright (c) 2009 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MemCpgc.c

  Memory Initialization Module.

**/

//
// Include files
//
#include "SysFunc.h"
#include "SysFuncChip.h"
#include "MemFuncChip.h"
#include "SysHostChip.h"
#include "Cpgc.h"
#include "CpgcChip.h"
#include "MemProjectSpecific.h"
#include "MemApiSkx.h"

#ifdef _MSC_VER
#pragma warning(disable : 4702)
#endif //_MSC_VER

extern UINT8  tWL_DDR4[MAX_SUP_FREQ];
extern UINT8 columnBitsDDR4[MAX_TECH];     // Number of column address bits

//
// Array to indicate how to map the LFSR # with the Lane and UI
// The different LFSR selection on each Lane for a given UI
// There are 4 patterns used for advanced backside ddr training
//
#define DDR_CPGC_NUM_PATTERNS       4

static UINT32 CpgcAdvCmdPatternSetup (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 rank, UINT8 dqLoopCount);
static UINT32 CpgcCheckTransientErrs(PSYSHOST host,UINT8 socket, UINT8 ch, CPGC_ERROR_STATUS *cpgcErrorStatus);
static UINT32 CpgcAdvDqSetup (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 rank, UINT8 subRank, UINT8 dqLoopCount);
static INT16 GetMinDwl(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct);
static void CpgcDisableGlobalStart (PSYSHOST host, UINT8 socket, UINT8 ch);
static UINT32 CpgcFillWdbDqVa (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 mode);
static UINT32 CpgcGlobalTrainingSetup (PSYSHOST host, UINT8 socket, UINT8 ch, CPGC_GLOBAL_TRAINING_SETUP *cpgcGblTrainingSetup);

#ifdef ADV_MEMTEST_SUPPORT
static UINT32
CpgcMemTestGlobalSetupMATS (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     rank,
  UINT8     dqLoopCount,
  UINT8     testType,
  UINT8     mode,
  UINT8     seqDataInv[MT_MAX_SUBSEQ],
  UINT64_STRUCT patternQW[MT_PATTERN_DEPTH],
  UINT8     uiShl[MT_PATTERN_DEPTH],
  UINT8     numCacheLines,
  UINT8     patternDepth,
  UINT8     direction
  );

static VOID
CpgcMemTestRankSetupMATS (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    ddrChEnMap,
  UINT8     logRank,
  UINT8     logSubRank,
  UINT8     colAddressBits[MAX_CH],
  UINT8     rowAddressBits[MAX_CH],
  UINT8     bankAddressBits[MAX_CH],
  UINT8     mode,
  UINT8     direction,
  UINT8     bank,
  UINT8     baseBits,
  UINT8     seqDataInv[MT_MAX_SUBSEQ]
  );

#else // ADV_MEMTEST_SUPPORT

static UINT32
CpgcMemTestGlobalSetup (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     rank,
  UINT8     dqLoopCount,
  UINT32    pattern
  );

static void
CpgcMemTestRankSetup (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    ddrChEnMap,
  UINT8     logRank,
  UINT8     logSubRank,
  UINT8     colAddressBits[MAX_CH],
  UINT8     rowAddressBits[MAX_CH],
  UINT8     bankAddressBits[MAX_CH]
  );
#endif // ADV_MEMTEST_SUPPORT

static VOID
CpgcSetupAddrGen (
  PSYSHOST                                      host,
  UINT8                                         socket,
  UINT8                                         ch,
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00,
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10,
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0,
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0,
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0,
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0,
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0
  );

static void
CpgcSetupWdb (
  PSYSHOST                                      host,
  UINT8                                         socket,
  UINT8                                         ch,
  UINT8                                         wdbIncrRate,
  UINT8                                         wdbIncrScale,
  UINT8                                         wdbStartPntr,
  UINT8                                         wdbEndPntr,
  UINT8                                         setupPbwr,
  UINT8                                         muxCtl,
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00,
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10,
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0,
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0,
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0,
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0,
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0
  );

static void
FixupAddrBits (
  PSYSHOST      host,
  GSM_CSN       sig,
  PatCadbProg0  prog0[CADB_LINES],
  PatCadbProg1  prog1[CADB_LINES]
  );

static void
FixupCS (
  UINT8         cs,
  UINT8         subrank,
  PatCadbProg0  prog0[CADB_LINES],
  PatCadbProg1  prog1[CADB_LINES],
  UINT8         encodedCSMode
  );

static UINT32
GetPmonCounter(
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch
);

static UINT8
IsAepRank(
          PSYSHOST host,
          UINT8    socket,
          UINT8    ch,
          UINT8    logRank
);

static void
ProgCADBRow (
  PSYSHOST      host,
  UINT8         socket,
  UINT8         ch,
  UINT8         dimm,
  UINT8         rank,
  UINT8         bank,
  UINT16        data,
  UINT8         side,
  UINT8         pdaTag,
  UINT8         control,
  PatCadbProg0  *patCADBProg0,
  PatCadbProg1  *patCADBProg1
  );

static PatCadbProg0 CtlClkPattern0[CADB_LINES] = {
// addr, cid, bank, pdatag
  {0x0,   0x0, 0x0, 0x0}, // Row 0
  {0x0,   0x0, 0x0, 0x0}, // Row 1
  {0x0,   0x0, 0x0, 0x0}, // Row 2
  {0x0,   0x0, 0x0, 0x0}, // Row 3
  {0x0,   0x0, 0x0, 0x0}, // Row 4
  {0x0,   0x0, 0x0, 0x0}, // Row 5
  {0x0,   0x0, 0x0, 0x0}, // Row 6
  {0x0,   0x0, 0x0, 0x0}, // Row 7
  {0x0,   0x0, 0x0, 0x0}, // Row 8
  {0x0,   0x0, 0x0, 0x0}, // Row 9
  {0x0,   0x0, 0x0, 0x0}, // Row 10
  {0x0,   0x0, 0x0, 0x0}, // Row 11
  {0x0,   0x0, 0x0, 0x0}, // Row 12
  {0x0,   0x0, 0x0, 0x0}, // Row 13
  {0x0,   0x0, 0x0, 0x0}, // Row 14
  {0x0,   0x0, 0x0, 0x0}, // Row 15
};

static PatCadbProg1 CtlClkPattern1[CADB_LINES] = {
// CS, Control, ODT,  CKE, Par
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 0
  {0x0,   0,  0x0,  0x0, 0}, // Row 1
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 2
  {0x0,   0,  0x0,  0x0, 0}, // Row 3
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 4
  {0x0,   0,  0x0,  0x0, 0}, // Row 5
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 6
  {0x0,   0,  0x0,  0x0, 0}, // Row 7
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 8
  {0x0,   0,  0x0,  0x0, 0}, // Row 9
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 10
  {0x0,   0,  0x0,  0x0, 0}, // Row 11
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 12
  {0x0,   0,  0x0,  0x0, 0}, // Row 13
  {0x3FF, 0, 0x3F, 0x3F, 0}, // Row 14
  {0x0,   0,  0x0,  0x0, 0}, // Row 15
};

                                         // addr, cid, bank, pdatag
static PatCadbProg0 ParityPattern0[CADB_LINES] = {{0x0,   0x0, 0x1, 0x0}, // Row 0
                                                  {0x0,   0x0, 0x1, 0x0}, // Row 1
                                                  {0x0,   0x0, 0x1, 0x0}, // Row 2
                                                  {0x0,   0x0, 0x1, 0x0}, // Row 3
                                                  {0x0,   0x0, 0x1, 0x0}, // Row 4
                                                  {0x0,   0x0, 0x1, 0x0}, // Row 5
                                                  {0x0,   0x0, 0x1, 0x0}, // Row 6
                                                  {0x0,   0x0, 0x1, 0x0}, // Row 7
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 8
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 9
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 10
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 11
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 12
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 13
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 14
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 15
                                                 };

                                         // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 ParityPattern1[CADB_LINES] = {{0x3FF,     0xF, 0x0, 0x3F, 0}, // Row 0
                                                  {0x3FF,     0xF, 0x0, 0x3F, 0}, // Row 1
                                                  {0x3FF,     0xF, 0x0, 0x3F, 0}, // Row 2
                                                  {0x3FF,     0xF, 0x0, 0x3F, 0}, // Row 3
                                                  {0x3FE,     0xF, 0x0, 0x3F, 1}, // Row 4*
                                                  {0x3FF,     0xF, 0x0, 0x3F, 0}, // Row 5
                                                  {0x3FF,     0xF, 0x0, 0x3F, 0}, // Row 6
                                                  {0x3FF,     0xF, 0x0, 0x3F, 0}, // Row 7
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 8
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 9
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 10
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 11
                                                  {0x3FE,     0xF, 0x0, 0x3F, 0}, // Row 12*
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 13
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 14
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 15
                                                 };

                                            // addr, cid, bank, pdatag
static PatCadbProg0 ActivatePattern0[CADB_LINES] = {{BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 0
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 1
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 2
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 3
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 4*
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 5
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 6
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 7
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 8
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 9
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 10
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 11
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 12
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 13
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 14
                                                    {BIT14 | BIT15,   0x0, 0x0, 0x0}, // Row 15
                                                   };

                                            // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 ActivatePattern1[CADB_LINES] = {{0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 0
                                                    {0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 1
                                                    {0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 2
                                                    {0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 3
                                                    {0x3FE,     0x3, 0x0, 0x3F, 0}, // Row 4*
                                                    {0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 5
                                                    {0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 6
                                                    {0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 7
                                                    {0x3FF,     0xB, 0x0, 0x3F, 0}, // Row 8
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 9
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 10
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 11
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 12
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 13
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 14
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 15
                                                   };

                                            // addr, cid, bank, pdatag
static PatCadbProg0 PrechargePattern0[CADB_LINES] = {{0x400,   0x0, 0x0, 0x0}, // Row 0
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 1
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 2
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 3
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 4
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 5
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 6
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 7
                                                     {0x400,   0x0, 0x0, 0x0}, // Row 8
                                                     {0x0,   0x0, 0x0, 0x0}, // Row 9
                                                     {0x0,   0x0, 0x0, 0x0}, // Row 10
                                                     {0x0,   0x0, 0x0, 0x0}, // Row 11
                                                     {0x0,   0x0, 0x0, 0x0}, // Row 12
                                                     {0x0,   0x0, 0x0, 0x0}, // Row 13
                                                     {0x0,   0x0, 0x0, 0x0}, // Row 14
                                                     {0x0,   0x0, 0x0, 0x0}, // Row 15
                                                    };

                                            // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 PrechargePattern1[CADB_LINES] = {{0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 0
                                                     {0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 1
                                                     {0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 2
                                                     {0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 3
                                                     {0x3FE,     0xA, 0x0, 0x3F, 1}, // Row 4*
                                                     {0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 5
                                                     {0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 6
                                                     {0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 7
                                                     {0x3FF,     0xA, 0x0, 0x3F, 1}, // Row 8
                                                     {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 9
                                                     {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 10
                                                     {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 11
                                                     {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 12
                                                     {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 13
                                                     {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 14
                                                     {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 15
                                                    };

                                        // addr, cid, bank, pdatag
static PatCadbProg0 FnvGoodParity0[CADB_LINES] = {{0x0,   0x0, 0x0, 0x0}, // Row 0
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 1
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 2
                                                  {0x0,   0x0, 0x7, 0x0}, // Row 3*
                                                  {0x0,   0x0, 0x7, 0x0}, // Row 4*
                                                  {0x0,   0x0, 0x7, 0x0}, // Row 5*
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 6
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 7
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 8
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 9
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 10
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 11
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 12
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 13
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 14
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 15
                                                };

                                          // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 FnvGoodParity1[CADB_LINES] = {{0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 0
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 1
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 2
                                                  {0x3FF,     0x8, 0x0, 0x3F, 0}, // Row 3*
                                                  {0x3FE,     0x8, 0x0, 0x3F, 0}, // Row 4*
                                                  {0x3FF,     0x8, 0x0, 0x3F, 0}, // Row 5*
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 6
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 7
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 8
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 9
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 10
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 11
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 12
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 13
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 14
                                                  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 15
                                                };

                                            // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 DeselectPattern1[CADB_LINES] = {{0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 0
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 1
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 2
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 3
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 4
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 5
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 6
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 7
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 8
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 9
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 10
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 11
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 12
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 13
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 14
                                                    {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 15
                                                   };


  // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 GntPattern1[CADB_LINES] = {
  {0x3FE,     0xF, 0x0, 0x3F, 1}, // Row 0  //GNT
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 1
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 2
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 3
  {0x3FE,     0xF, 0x0, 0x3F, 1}, // Row 4  // GNT
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 5
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 6
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 7
  {0x3FE,     0xF, 0x0, 0x3F, 1}, // Row 8  // GNT
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 9
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 10
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 11
  {0x3FE,     0xF, 0x0, 0x3F, 1}, // Row 12  //GNT
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 13
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 14
  {0x3FF,     0xF, 0x0, 0x3F, 1}, // Row 15
};
                                         // addr, cid, bank, pdatag
static PatCadbProg0 AddressPattern0[CADB_LINES] = {{0x0,   0x0, 0x0, 0x0}, // Row 0
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 1
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 2
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 3
                                                  {0x0,   0x0, 0x8, 0x0}, // Row 4
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 5
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 6
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 7
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 8
                                                  {0x0,   0x0, 0x8, 0x0}, // Row 9
                                                  {0x0,   0x0, 0x8, 0x0}, // Row 10
                                                  {0x0,   0x0, 0x8, 0x0}, // Row 11
                                                  {0x0,   0x0, 0x0, 0x0}, // Row 12
                                                  {0x0,   0x0, 0x8, 0x0}, // Row 13
                                                  {0x0,   0x0, 0x8, 0x0}, // Row 14
                                                  {0x0,   0x0, 0x8, 0x0}, // Row 15
                                                 };

                                         // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 AddressPattern1[CADB_LINES] = {{0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 0
                                                  {0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 1
                                                  {0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 2
                                                  {0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 3
                                                  {0x3FE,     0x3, 0x0, 0x3F, 1}, // Row 4
                                                  {0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 5
                                                  {0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 6
                                                  {0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 7
                                                  {0x3FF,     0x3, 0x0, 0x3F, 1}, // Row 8
                                                  {0x3FF,     0x3, 0x0, 0x3F, 0}, // Row 9
                                                  {0x3FF,     0x3, 0x0, 0x3F, 0}, // Row 10
                                                  {0x3FF,     0x3, 0x0, 0x3F, 0}, // Row 11
                                                  {0x3FE,     0x3, 0x0, 0x3F, 0}, // Row 12
                                                  {0x3FF,     0x3, 0x0, 0x3F, 0}, // Row 13
                                                  {0x3FF,     0x3, 0x0, 0x3F, 0}, // Row 14
                                                  {0x3FF,     0x3, 0x0, 0x3F, 0}, // Row 15
                                                 };

                                         // addr, cid, bank, pdatag
static PatCadbProg0 AdvCmdPattern0[CADB_LINES] = {{0x00000,   0x0, 0x0, 0x0}, // Row 0
                                                  {0x02222,   0x0, 0x08, 0x0}, // Row 1
                                                  {0x11111,   0x2, 0x04, 0x0}, // Row 2
                                                  {0x13333,   0x2, 0x0C, 0x0}, // Row 3
                                                  {0x08888,   0x1, 0x02, 0x0}, // Row 4
                                                  {0x0AAAA,   0x1, 0x0A, 0x0}, // Row 5
                                                  {0x19999,   0x3, 0x06, 0x0}, // Row 6
                                                  {0x1BBBB,   0x3, 0x0E, 0x0}, // Row 7
                                                  {0x04444,   0x0, 0x01, 0x0}, // Row 8
                                                  {0x06666,   0x0, 0x09, 0x0}, // Row 9
                                                  {0x15555,   0x2, 0x05, 0x0}, // Row 10
                                                  {0x17777,   0x2, 0x0D, 0x0}, // Row 11
                                                  {0x0CCCC,   0x1, 0x03, 0x0}, // Row 12
                                                  {0x0EEEE,   0x1, 0x0B, 0x0}, // Row 13
                                                  {0x1DDDD,   0x3, 0x07, 0x0}, // Row 14
                                                  {0x1FFFF,   0x3, 0x0F, 0x0}, // Row 15
                                                 };

                                         // addr, cid, bank, pdatag
static PatCadbProg0 AdvCmdPattern2[CADB_LINES] = {{0x00000,   0x0, 0x0, 0x0}, // Row 0
                                                  {0x02222,   0x0, 0x08, 0x0}, // Row 1
                                                  {0x01111,   0x0, 0x04, 0x0}, // Row 2
                                                  {0x03333,   0x0, 0x0C, 0x0}, // Row 3
                                                  {0x08888,   0x0, 0x02, 0x0}, // Row 4
                                                  {0x0AAAA,   0x0, 0x0A, 0x0}, // Row 5
                                                  {0x09999,   0x0, 0x06, 0x0}, // Row 6
                                                  {0x0BBBB,   0x0, 0x0E, 0x0}, // Row 7
                                                  {0x04444,   0x0, 0x01, 0x0}, // Row 8
                                                  {0x06666,   0x0, 0x09, 0x0}, // Row 9
                                                  {0x05555,   0x0, 0x05, 0x0}, // Row 10
                                                  {0x07777,   0x0, 0x0D, 0x0}, // Row 11
                                                  {0x0CCCC,   0x0, 0x03, 0x0}, // Row 12
                                                  {0x0EEEE,   0x0, 0x0B, 0x0}, // Row 13
                                                  {0x0DDDD,   0x0, 0x07, 0x0}, // Row 14
                                                  {0x0FFFF,   0x0, 0x0F, 0x0}, // Row 15
                                                 };

                                         // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 AdvCmdPattern1[CADB_LINES] = {{0x0,     0x00, 0x0, 0x0, 0x0}, // Row 0
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 1
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 2
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 3
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 4
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 5
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 6
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 7
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 8
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 9
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 10
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 11
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 12
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 13
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 14
                                                  {0x0,     0x03, 0x0, 0x0, 0x0}, // Row 15
                                                 };

                                         // addr, cid, bank, pdatag
static PatCadbProg0 AggAdvCmdPattern0[CADB_LINES] = {{0x00000,   0x0, 0x0, 0x0}, // Row 0
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 1
                                                     {0x1DFBF,   0x7, 0x0F, 0x0}, // Row 2
                                                     {0x1FFFF,   0x7, 0x0F, 0x0}, // Row 3
                                                     {0x00000,   0x0, 0x00, 0x0}, // Row 4
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 5
                                                     {0x1DFBF,   0x7, 0x0F, 0x0}, // Row 6
                                                     {0x1FFFF,   0x7, 0x0F, 0x0}, // Row 7
                                                     {0x00000,   0x0, 0x00, 0x0}, // Row 8
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 9
                                                     {0x1DFBF,   0x7, 0x0F, 0x0}, // Row 10
                                                     {0x1FFFF,   0x7, 0x0F, 0x0}, // Row 11
                                                     {0x00000,   0x0, 0x00, 0x0}, // Row 12
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 13
                                                     {0x1DFBF,   0x7, 0x0F, 0x0}, // Row 14
                                                     {0x1FFFF,   0x7, 0x0F, 0x0}, // Row 15
                                                    };

                                         // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 AggAdvCmdPattern1[CADB_LINES] = {{0x0,     0x00, 0x0, 0x0, 0x0}, // Row 0
                                                     {0x0,     0x08, 0x0, 0x0, 0x0}, // Row 1
                                                     {0x0,     0x07, 0x0, 0x0, 0x0}, // Row 2
                                                     {0x0,     0x0F, 0x0, 0x0, 0x0}, // Row 3
                                                     {0x0,     0x00, 0x0, 0x0, 0x0}, // Row 4
                                                     {0x0,     0x08, 0x0, 0x0, 0x0}, // Row 5
                                                     {0x0,     0x07, 0x0, 0x0, 0x0}, // Row 6
                                                     {0x0,     0x0F, 0x0, 0x0, 0x0}, // Row 7
                                                     {0x0,     0x00, 0x0, 0x0, 0x0}, // Row 8
                                                     {0x0,     0x08, 0x0, 0x0, 0x0}, // Row 9
                                                     {0x0,     0x07, 0x0, 0x0, 0x0}, // Row 10
                                                     {0x0,     0x0F, 0x0, 0x0, 0x0}, // Row 11
                                                     {0x0,     0x00, 0x0, 0x0, 0x0}, // Row 12
                                                     {0x0,     0x08, 0x0, 0x0, 0x0}, // Row 13
                                                     {0x0,     0x07, 0x0, 0x0, 0x0}, // Row 14
                                                     {0x0,     0x0F, 0x0, 0x0, 0x0}, // Row 15
                                                    };

                                         // addr, cid, bank, pdatag
static PatCadbProg0 AggAdvCmdPattern2[CADB_LINES] = {{0x00000,   0x0, 0x0, 0x0}, // Row 0
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 1
                                                     {0x0DFBF,   0x0, 0x0F, 0x0}, // Row 2
                                                     {0x0FFFF,   0x0, 0x0F, 0x0}, // Row 3
                                                     {0x00000,   0x0, 0x00, 0x0}, // Row 4
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 5
                                                     {0x0DFBF,   0x0, 0x0F, 0x0}, // Row 6
                                                     {0x0FFFF,   0x0, 0x0F, 0x0}, // Row 7
                                                     {0x00000,   0x0, 0x00, 0x0}, // Row 8
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 9
                                                     {0x0DFBF,   0x0, 0x0F, 0x0}, // Row 10
                                                     {0x0FFFF,   0x0, 0x0F, 0x0}, // Row 11
                                                     {0x00000,   0x0, 0x00, 0x0}, // Row 12
                                                     {0x02040,   0x0, 0x00, 0x0}, // Row 13
                                                     {0x0DFBF,   0x0, 0x0F, 0x0}, // Row 14
                                                     {0x0FFFF,   0x0, 0x0F, 0x0}, // Row 15
                                                    };

static PatCadbProg0 ReadCommandsMRSMode0[CADB_LINES] = {{0x4000,   0x0, 0x0, 0x0}, // Row 0
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 1
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 2
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 3
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 4
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 5
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 6
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 7
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 8
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 9
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 10
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 11
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 12
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 13
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 14
                                                        {0x4000,   0x0, 0x0, 0x0}, // Row 15
                                                       };

                                         // CS  , Control, ODT,  CKE, Par
static PatCadbProg1 ReadCommandsMRSMode1[CADB_LINES] = {{0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 0
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 1
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 2
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 3
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 4*
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 5
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 6
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 7
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 8
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 9
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 10
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 11
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 12*
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 13
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 14
                                                        {0x3FE,     0x1, 0x0, 0x3F, 1}, // Row 15
                                                       };

// wdb chunks 0-3, 4-7
/*
const UINT32 CpgcWdbDqVaPat [8] =     { 0xfffe0100,           // wdb 0 for dq/va
                                        0xfffd0200,           // wdb 1 for dq/va
                                        0xfffb0400,           // wdb 2 for dq/va
                                        0xfff70800,           // wdb 3 for dq/va
                                        0xffef1000,           // wdb 4 for dq/va
                                        0xffdf2000,           // wdb 5 for dq/va
                                        0xffbf4000,           // wdb 6 for dq/va
                                        0xff7f8000,           // wdb 7 for dq/va
                                      };
 */
static const UINT8 CpgcWdbDqVaPat [8][8] =  {
                                        {0, 0x01, 0xFE, 0xFF, 0, 0x01, 0xFE, 0xFF}, //0xfffe0100,           // wdb 0 for dq/va
                                        {0, 0x02, 0xFD, 0xFF, 0, 0x02, 0xFD, 0xFF}, //0xfffd0200,           // wdb 1 for dq/va
                                        {0, 0x04, 0xFB, 0xFF, 0, 0x04, 0xFB, 0xFF}, //0xfffb0400,           // wdb 2 for dq/va
                                        {0, 0x08, 0xF7, 0xFF, 0, 0x08, 0xF7, 0xFF}, //0xfff70800,           // wdb 3 for dq/va
                                        {0, 0x10, 0xEF, 0xFF, 0, 0x10, 0xEF, 0xFF}, //0xffef1000,           // wdb 4 for dq/va
                                        {0, 0x20, 0xDF, 0xFF, 0, 0x20, 0xDF, 0xFF}, //0xffdf2000,           // wdb 5 for dq/va
                                        {0, 0x40, 0xBF, 0xFF, 0, 0x40, 0xBF, 0xFF}, //0xffbf4000,           // wdb 6 for dq/va
                                        {0, 0x80, 0x7F, 0xFF, 0, 0x80, 0x7F, 0xFF}  //0xff7f8000,           // wdb 7 for dq/va
                                      };

static const UINT8 CpgcWdbDqVaPat2 [8][8] =  {
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF}, //          // wdb 0 for dq/va
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF}, //          // wdb 1 for dq/va
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF}, //          // wdb 2 for dq/va
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF}, //          // wdb 3 for dq/va
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF}, //          // wdb 4 for dq/va
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF}, //          // wdb 5 for dq/va
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF}, //          // wdb 6 for dq/va
                                        {0, 0xEE, 0x11, 0xFF, 0, 0xEE, 0x11, 0xFF} //           // wdb 7 for dq/va
};

/*
*/
static void
CADBPointerWorkaround (
                       PatCadbProg0 prog0[],
                       PatCadbProg1 prog1[]
                       )
{
  PatCadbProg0 prog0local[CADB_LINES];
  PatCadbProg1 prog1local[CADB_LINES];
  UINT8 i;
  UINT8 j;

  for (i = 0; i < CADB_LINES; i++) {
    j = (i + 12) & 0xF;
    prog0local[j] = prog0[i];
    prog1local[j] = prog1[i];
  }

  for (i = 0; i < CADB_LINES; i++) {
    prog0[i] = prog0local[i];
    prog1[i] = prog1local[i];
  }
}

/*
      Write VA pattern in CADB
      Use basic VA pattern for CADB with 2 LFSRs. Rotation is manual
      Bit Order is  [CKE[3:0], ODT[3:0], CMD[2:0], CS[3:0], BA[2:0], MA[15:0]]
                      [59:56]  [51:48]  [42:40] [35:32]  [26:24]  [15:0]

      NOTE: CKE, ODT and CS are not used in functional mode and are ignored

       @param host         - Include all MRC global data.
       @param socket         - Memory Controller
       @param VicSpread      -
       @param VicBit            -
       @param LMNEn           - To enable LMN counter

     @retval N/A
*/
static void
SetupCADB(
         PSYSHOST  host,
         UINT8     socket,
         UINT8     ch,
         UINT8     VicSpread,
         UINT8     VicBit,
         UINT8     LMNEn
         )
{
  UINT8         r;
  UINT8         b;
  UINT8         lfsr0;
  UINT8         lfsr1;
  UINT8         bremap;
  UINT8         Fine;
  UINT16        seeds[3] = {0xEA1, 0xBEEF, 0xDEAD};
  UINT64_STRUCT data;
  UINT64_STRUCT datax;
  UINT64_STRUCT lfsr0_64;
  UINT64_STRUCT lfsr1_64;
  // struct RankCh (*rankPerCh)[MAX_RANK_CH];
  CPGC_PATCADBCLMUX1LMN_MCDDC_CTL_STRUCT patCadbClMux1Lmn;

  // Currently, always start writing at CADB row0.  Could add Start point in future.
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, 0);

  // Plan to use VicSpread of 7 bits
  // Walk through CADB rows, assigning bit for 1 VA pattern
  // rankPerCh = &host->nvram.mem.socket[socket].channelList[ch].rankPerCh;
  for (r = 0; r < MAX_RANK_CH; r++) {
    //<VM> - Chris, Should we check for validRankBitmask?
    //if (CheckRankAbsentDisabled(host, socket, ch, (*rankPerCh)[r].dimm, (*rankPerCh)[r].rank)) continue;

    lfsr0 = (r & 0x1);      // 0, 1, 0, 1 for r = 0,1,2 & 3
    lfsr1 = (r / 2);        // 0, 0, 1, 1 for r = 0,1,2 & 3

    // Assign Victirm/Aggressor Bits
    data.lo = 0;
    data.hi = 0;
    for (b = 0; b < 22; b++) { // b in range(22)
      Fine  = b % VicSpread;
      if (b >= 19) bremap = b + 21; // b = [40-42]
      else if (b >= 16) bremap = b + 8; // b = [24-26]
      else         bremap = b; // b = [0-15]
      lfsr0_64.lo = lfsr0;
      lfsr0_64.hi = 0;
      lfsr1_64.lo = lfsr1;
      lfsr1_64.hi = 0;
      if (Fine == VicBit)  datax = LShiftUINT64(lfsr0_64, bremap);
      else                 datax = LShiftUINT64(lfsr1_64, bremap);
      data.lo |= datax.lo;
      data.hi |= datax.hi;
    }

    // Write Row. CADB is auto incremented after every write
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBPROG0_MCDDC_CTL_REG, data.lo);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBPROG1_MCDDC_CTL_REG, data.hi);
  } // r loop

  //
  // Setup CADB in terms of LFSR selects, LFSR seeds, LMN constants and overall control
  //

  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, (LMNEn ? 0x2 : 0x22));
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCLMUX0LMN_MCDDC_CTL_REG, 0);
  patCadbClMux1Lmn.Data = 0;
  patCadbClMux1Lmn.Bits.sweep_freq = 1;
  patCadbClMux1Lmn.Bits.l_counter = 1;
  patCadbClMux1Lmn.Bits.m_counter = 1;
  patCadbClMux1Lmn.Bits.n_counter = 6;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCLMUX1LMN_MCDDC_CTL_REG, patCadbClMux1Lmn.Data);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCLMUX2LMN_MCDDC_CTL_REG, 0);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, seeds[0]);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, seeds[1]);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, seeds[2]);
} // SetupCADB

/**
     Programs all the key registers to define a CPCG test

      @param host       - Include all MRC global data.
      @param socket       - Memory Controller
      @param ChbitMask      - ch Bit mak for which test should be setup for.
      @param CmdPat         = [0: PatWrRd (Standard Write/Read Loopback),
                                     1: PatWr (Write Only),
                                     2: PatRd (Read Only),
                                     3: PatRdWrTA (ReadWrite Turnarounds),
                                     4: PatWrRdTA (WriteRead Turnarounds),
                                     5: PatODTTA (ODT Turnaround]
      @param NumCL          = Number of Cache lines
      @param LC             = Loop Count
      @param CPGCAddress    = Structure that stores start, stop and increment details for address
      @param SOE            = [0: Never Stop, 1: Stop on Any Lane, 2: Stop on All Byte, 3: Stop on All Lane]
      @param WDBPattern   - Structure that stores start, Stop, IncRate and Dqpat for pattern.
      @param EnCADB        - Enable test to write random deselect packages on bus to create xtalk/isi
      @param EnCKE          - Enable CKE power down by adding 64
      @param SubSeqWait    - # of Dclks to stall at the end of a sub-sequence

    @retval N/A
**/
void
SetupIOTest(
           PSYSHOST      host,
           UINT8         socket,
           UINT32        ChbitMask,
           UINT8         CmdPat,
           UINT16        NumCL,
           UINT8         LC,
           TCPGCAddress  *CPGCAddress,
           UINT8         SOE,
           TWDBPattern   *WDBPattern,
           UINT8         EnCADB,
           UINT8         EnCKE,
           UINT16        SubSeqWait
           )
{
  UINT8                                     ch;
  //UINT8                                     LCeff;
  UINT8                                     Mux0;
  UINT8                                     Reload;
  UINT8                                     mcId;
  //UINT8                                     Save;
  //UINT8                                     Wait;
  UINT32                                    LMNFreq[2];
  TCPGCAddress                              *a;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT          PatCadbCtl;
  CPGC_SEQCTL0_MC_MAIN_STRUCT               seqCtl0;
  CPGC_SUBSEQ0_CTL0_MC_MAIN_STRUCT          subSeqCtl0;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT  seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT  seqBaseAddrWrapHi0;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT           seqBAOCIC0;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT        seqBAIncCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT        seqBAIncCtl10;
  CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT         patWDBCLCtl;
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT      patWDBCLMuxCtl;
  CPGC_ERR_CTL_MCDDC_CTL_STRUCT             errCtl;
  CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_STRUCT      seqDmyRdAddr;
  CPGC_SEQ_DMYRDCTL0_MC_MAIN_STRUCT         seqDmyRdCtl;
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  UINT8                                     aepPresent;
  UINT8                                     dimm;
  UINT8                                     CmdPatOrig;
  UINT32                                    seqCtl0Orig;
#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SetupIOTest Starts\n"));
#endif

  //if ((LC - Log2x32(NumCL - 1) + 1) < 1)  LCeff = 1;
  //else                                    LCeff = LC - Log2x32(NumCL - 1) + 1;

  Reload = Log2x32(WDBPattern->IncRate - 1);
  //Save = Reload + Log2x32((WDBPattern->Stop - WDBPattern->Start - 1) & 0xFF);

  if (WDBPattern->IncRate > 31) WDBPattern->IncRate = Reload;
  else                          WDBPattern->IncRate += 32;

  //
  // Program Sequence
  //
  CmdPatOrig = CmdPat;
  seqCtl0.Data = 0;
  if (CmdPat == PatWrRd) {
    seqCtl0.Bits.subseq_start_pointer = 0;
    seqCtl0.Bits.subseq_end_pointer   = 1;
  } else if (CmdPat == PatWr) {
    seqCtl0.Bits.subseq_start_pointer = 0;
    seqCtl0.Bits.subseq_end_pointer   = 0;
  } else if (CmdPat == PatRd) {
    seqCtl0.Bits.subseq_start_pointer = 1;
    seqCtl0.Bits.subseq_end_pointer   = 1;
  } else if (CmdPat == PatRdWrTA) {
    seqCtl0.Bits.subseq_start_pointer = 0;
    seqCtl0.Bits.subseq_end_pointer   = 0;
  } else if (CmdPat == PatWrRdTA) {
    seqCtl0.Bits.subseq_start_pointer = 0;
    seqCtl0.Bits.subseq_end_pointer   = 7;
  } else if (CmdPat == PatODTTA) {
    seqCtl0.Bits.subseq_start_pointer = 0;
    seqCtl0.Bits.subseq_end_pointer   = 3;
  } else if (CmdPat == PatRdWrRd) {
      seqCtl0.Bits.subseq_start_pointer = 0;
      seqCtl0.Bits.subseq_end_pointer = 2;
  } else MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "\nSetupIOTest: Unknown Pattern\n"));

  seqCtl0.Bits.enable_dummy_reads   = host->var.mem.socket[socket].EnDumRd; // Enable Dummy Reads
  seqCtl0.Bits.global_control       = 1; // Enable Global Control
  seqCtl0.Bits.initialization_mode  = CPGC_Testing_Mode; // Test Mode
  //seqCtl0.Bits.loopcount            = LCeff;  // Loopcount
  seqCtl0.Bits.loopcount            = LC;  // Loopcount
  seqCtl0.Bits.start_test_delay     = 2;      // Delay the start of the test by 2 clocks

  // Inc address based on LC
  if (CmdPat == DimmTest) {
    seqCtl0.Bits.address_update_rate_mode = 1;
  }
  seqCtl0Orig = seqCtl0.Data;
  //
  // Program Sub Sequences
  //
  subSeqCtl0.Data = 0;
  // Reset Current Base address to start
  subSeqCtl0.Bits.rstcurr_ba_start = 1;

  //if (NumCL > 127) {
  //  // Assume Exponential number
  //  subSeqCtl0.Bits.num_cachelines = Log2x32(NumCL - 1);
  //} else {
  // Set Number of Cache Lines as linear number
  subSeqCtl0.Bits.num_cachelines = NumCL;
  subSeqCtl0.Bits.num_cachelines_scale = 1;
  //}

  for (ch = 0; ch < MAX_CH; ch++) {

    mcId = GetMCID(host, socket, ch);

    if (!(ChbitMask & (1 << ch))) {
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, 0);   // Clear global control
      continue;
    }

    CmdPat = CmdPatOrig;
    seqCtl0.Data = seqCtl0Orig;
    DdrtWriteFlyBy(host, socket, ch, &CmdPat, &seqCtl0.Data);
    //
    // Program CADB
    //
    if (EnCADB) {
      aepPresent = 0;
      dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
      for (dimm = 0; dimm < host->nvram.mem.socket[socket].channelList[ch].maxDimm; dimm++) {
        if((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if((*dimmNvList)[dimm].aepDimmPresent)  {
          aepPresent = 1 ;
          break;
        }
      }
      //
      // Enable CADB on deselect
      //
      PatCadbCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
      // A0 stepping workaround, s4929399
      if (aepPresent) {
        PatCadbCtl.Bits.enable_cadb_on_deselect = 0;
      } else {
        PatCadbCtl.Bits.enable_cadb_on_deselect = 1;
      }
      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, PatCadbCtl.Data);

      SetupCADB(host, socket, ch, 7, 8, 0);   // LMNEn=0
    }

    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
    // Clear Local Errors
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data | BIT2);

    if (SubSeqWait > 0xFF) SubSeqWait = 0xFF;
    subSeqCtl0.Bits.subseq_wait = SubSeqWait;

    if (CmdPat == PatWrRdTA) {
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BWr  << 20)); // Write CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRdWr  << 20)); // Read-Write CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ2_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRdWr  << 20)); // Read-Write CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ3_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRdWr  << 20)); // Read-Write CMD
    } else if (CmdPat == PatRdWrTA) {
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BWrRd << 20)); // Write-Read CMD
    } else if (CmdPat == PatODTTA) {
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BWr  << 20)); // Write CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRd  << 20)); // Read CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ2_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BWrRd<< 20)); // Write-Read CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ3_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BWr  << 20)); // Write CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ4_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRdWr<< 20)); // Read-Write CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ5_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRdWr  << 20)); // Read-Write CMD
    } else if (CmdPat == PatRdWrRd) {
        subSeqCtl0.Bits.savecurr_ba_start = 0;
        subSeqCtl0.Bits.rstcurr_ba_start = 0;
        subSeqCtl0.Bits.num_cachelines = 4;
        MemWritePciCfgMC(host, socket, mcId, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG + (0x50 * GetMCCh(mcId, ch)), subSeqCtl0.Data + (BRd << 20)); // Read CMD

        subSeqCtl0.Bits.savecurr_ba_start = 1;
        subSeqCtl0.Bits.rstcurr_ba_start = 0;
        subSeqCtl0.Bits.num_cachelines = 4;
        MemWritePciCfgMC(host, socket, mcId, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG + (0x50 * GetMCCh(mcId, ch)), subSeqCtl0.Data + (BWr << 20)); // Write CMD

        subSeqCtl0.Bits.savecurr_ba_start = 0;
        subSeqCtl0.Bits.rstcurr_ba_start = 1;
        subSeqCtl0.Bits.num_cachelines = 4;
        MemWritePciCfgMC(host, socket, mcId, CPGC_SUBSEQ2_CTL0_MC_MAIN_REG + (0x50 * GetMCCh(mcId, ch)), subSeqCtl0.Data + (BRd << 20)); // Read CMD

    } else {
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BWr  << 20)); // Write CMD
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRd  << 20)); // Read CMD
    }

    //
    // Program Sequence Address
    //
    a = CPGCAddress;

    seqBaseAddrStartLo0.Data              = 0;
    seqBaseAddrStartLo0.Bits.column_addr  = a->Start[3];
    seqBaseAddrStartLo0.Bits.bank_addr    = a->Start[1];
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_REG, 4, seqBaseAddrStartLo0.Data);

    seqBaseAddrStartHi0.Data            = 0;
    seqBaseAddrStartHi0.Bits.row_addr   = a->Start[2];
    seqBaseAddrStartHi0.Bits.rank_addr  = a->Start[0];
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_REG, 4, seqBaseAddrStartHi0.Data);

    seqBaseAddrWrapLo0.Data             = 0;
    seqBaseAddrWrapLo0.Bits.column_addr = a->Stop[3];
    seqBaseAddrWrapLo0.Bits.bank_addr   = a->Stop[1];
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_REG, 4, seqBaseAddrWrapLo0.Data);

    seqBaseAddrWrapHi0.Data           = 0;
    seqBaseAddrWrapHi0.Bits.row_addr  = a->Stop[2];
    seqBaseAddrWrapHi0.Bits.rank_addr = a->Stop[0];
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_REG, 4, seqBaseAddrWrapHi0.Data);

    // Do not bother with 23:13 as this is all DIMM testing
    seqBAOCIC0.Data = 0;
    seqBAOCIC0.Bits.column_addr_order = a->Order[3];
    seqBAOCIC0.Bits.row_addr_order    = a->Order[2];
    seqBAOCIC0.Bits.bank_addr_order   = a->Order[1];
    seqBAOCIC0.Bits.rank_addr_order   = a->Order[0];
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAOCIC0_MC_MAIN_REG, 4, seqBAOCIC0.Data);

    seqBAIncCtl00.Data = 0;
    seqBAIncCtl10.Data = 0;
    // Rank
    if (a->IncRate[0] > 31) {
      seqBAIncCtl10.Bits.rank_ba_updrate = Log2x32(a->IncRate[0] - 1);
    } else {
      seqBAIncCtl10.Bits.rank_ba_updrate = 128 + a->IncRate[0];
    }
    seqBAIncCtl10.Bits.rank_ba_inc = a->IncVal[0] & 0x7;

    // Bank
    if (a->IncRate[1] > 31) {
      seqBAIncCtl10.Bits.bank_ba_updrate = Log2x32(a->IncRate[1] - 1);
    } else {
      seqBAIncCtl10.Bits.bank_ba_updrate = 128 + a->IncRate[1];
    }
    seqBAIncCtl10.Bits.bank_ba_inc = a->IncVal[1] & 0x7;

    // Row
    if (a->IncRate[2] > 15) {
      seqBAIncCtl00.Bits.row_ba_updrate = Log2x32(a->IncRate[2] - 1);
    } else {
      seqBAIncCtl00.Bits.row_ba_updrate = 32 + a->IncRate[2];
    }
    seqBAIncCtl00.Bits.row_ba_inc = a->IncVal[2] & 0xFFF;

    // Col
    if (a->IncRate[3] > 31) {
      seqBAIncCtl00.Bits.column_ba_updrate = Log2x32(a->IncRate[3] - 1);
      seqBAIncCtl00.Bits.column_ba_updscale = 0;
    } else {
//      seqBAIncCtl00.Bits.column_ba_updrate = 128 + a->IncRate[3];
      seqBAIncCtl00.Bits.column_ba_updrate = a->IncRate[3];
      seqBAIncCtl00.Bits.column_ba_updscale = 1;
    }
    seqBAIncCtl00.Bits.column_ba_inc = a->IncVal[3] & 0x7F;

    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAINCCTL00_MC_MAIN_REG, 8, seqBAIncCtl00.Data);
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAINCCTL10_MC_MAIN_REG, 8, seqBAIncCtl10.Data);

    //
    // Program Write Data Buffer Related Entries
    //
    patWDBCLCtl.Data = 0;
    patWDBCLCtl.Bits.incr_rate  = WDBPattern->IncRate;
    patWDBCLCtl.Bits.start_pntr = WDBPattern->Start;
    patWDBCLCtl.Bits.end_pntr   = WDBPattern->Stop;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCLCTL_MCDDC_CTL_REG, patWDBCLCtl.Data);

    //MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_WR0_MCDDC_CTL_HSX_REG, 0x55555555);
    //MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_WR1_MCDDC_CTL_HSX_REG, 0xAAAAAAAA);

    // Enable LMN in either LMN mode or CADB -to create lots of supply noise
    Mux0 = ((WDBPattern->DQPat == LMNVa) || (WDBPattern->DQPat == CADB)) ? LMNMode : LFSRMode;

    // ECC, Select LFSR
    patWDBCLMuxCtl.Data = 0;
    patWDBCLMuxCtl.Bits.mux0 = Mux0;
    patWDBCLMuxCtl.Bits.mux1 = LFSRMode;
    patWDBCLMuxCtl.Bits.mux2 = LFSRMode;
    patWDBCLMuxCtl.Bits.ecc_datasrc_sel = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, patWDBCLMuxCtl.Data);

    // Currently, not planning to use the Inverstion Mask
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV_MCDDC_DP_REG, 0);

    // Write these registers when you first setup test
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG, 0x0EA10CA1);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG, 0xDEAD0A1E);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG, 0xBEEF0D08);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG, 0x0EA10CA1);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG, 0xDEAD0A1E);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG, 0xBEEF0D08);

    //
    // Program Error Checking
    //

    //
    // Enable selective_error_enable_chunk and selective_error_enable_cacheline, mask later
    // the bits we don't want to check.
    //
    errCtl.Data                         = 0;
    errCtl.Bits.stop_on_ntherr          = 1;
    errCtl.Bits.stop_on_errcontrol      = SOE;
    errCtl.Bits.selective_err_enb_chunk = 0xFF;
    errCtl.Bits.selective_err_enb_cl    = 0xFF;
    if (CmdPat == PatRdWrRd) errCtl.Bits.selective_err_enb_cl = 0xF0;
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_CTL_MCDDC_CTL_REG, errCtl.Data);

    //
    // Do not mask off errors
    //
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA0_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA1_MCDDC_CTL_REG, 0);
    //
    // Do not check ECC bits if ECC is disabled
    //
    if (host->nvram.mem.eccEn) {
      MemWritePciCfgEp (host, socket, ch, CPGC_ERR_ECC_MCDDC_CTL_REG, 0);
    } else {
      MemWritePciCfgEp (host, socket, ch, CPGC_ERR_ECC_MCDDC_CTL_REG, 0xFF);
    }

    //
    // Program Dummy Read
    //
    if (host->var.mem.socket[socket].EnDumRd) {
      // CPGC traffic only uses BA[1:0] - Mask BANK that will not be used
      seqDmyRdAddr.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG);
      seqDmyRdAddr.Bits.bank_mask0 = 0xFC;
      MemWritePciCfgMC (host, socket, ch, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG, seqDmyRdAddr.Data);

      // Rotated from 40nS to 200nS
      LMNFreq[0] = (40000/host->nvram.mem.socket[socket].QCLKps);
      LMNFreq[1] = (200000/host->nvram.mem.socket[socket].QCLKps);

      seqDmyRdCtl.Data = 0;
      seqDmyRdCtl.Bits.sweep_freq = 1;
      seqDmyRdCtl.Bits.l_counter = LMNFreq[0];
      seqDmyRdCtl.Bits.m_counter = LMNFreq[0];
      seqDmyRdCtl.Bits.n_counter = LMNFreq[1];
      // Chirp Freq from 5 to 25 MHz
      MemWritePciCfgMC_Ch (host, socket, mcId, CPGC_SEQ_DMYRDCTL0_MC_MAIN_REG, 4, seqDmyRdCtl.Data);
    } // if EnDumRd
  } // ch loop

  // Always do a ZQ Short before the beginning of a test
  //CpgcIssueZQ(host, socket, 0);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SetupIOTest Ends\n"));
#endif

} // SetupIOTest

/**

  Setup IO Test Basic VA

  @param host      - Pointer to sysHost
  @param socket    - Socket from which we are going to read the registers
  @param ChbitMask - ch Bit mask for which test should be setup for.
  @param LC        - Loop Count
  @param SOE       - [0: Never Stop, 1: Stop on Any Lane, 2:Stop on All Byte, 3: Stop on All Lane]
  @param EnCADB    - Enable test to write random deselect packages on bus to create xtalk/isi
  @param EnCKE     - Enable CKE power down by adding 64
  @param Spread    - Pattern Spread

  @retval N/A

**/
void
SetupIOTestBasicVA(
                  PSYSHOST  host,
                  UINT8     socket,
                  UINT32    ChbitMask,
                  UINT8     LC,
                  UINT8     SOE,
                  UINT8     EnCADB,
                  UINT8     EnCKE,
                  UINT32    Spread
                  )
{
  TCPGCAddress CPGCAddress = {{0, 0, 0, 0},    // Start
    {0, 0, 0, 1023}, // Stop
    {0, 0, 0, 0},    // Order
    {511, 0, 0, 0},  // IncRate
    {1, 0, 0, 1}};   // IncValue
  //                             IncRate,  Start, Stop, DQPat
  TWDBPattern  WDBPattern = {     16,     0,    9, BasicVA};

  WDBPattern.Stop = (Spread-1);

  SetupIOTest(host, socket, ChbitMask, PatWrRd, 8, LC, &CPGCAddress, SOE, \
              &WDBPattern, EnCADB, EnCKE, 0);

  host->var.mem.socket[socket].DQPatLC = LC - 8 + 1;
  if (host->var.mem.socket[socket].DQPatLC < 1) host->var.mem.socket[socket].DQPatLC = 1;
  host->var.mem.socket[socket].DQPat = BasicVA;
} // SetupIOTestBasicVA

/**

  Setup IO Test MPR

  @param host      - Pointer to sysHost
  @param socket    - Socket from which we are going to read the registers
  @param ChbitMask - ch Bit mask for which test should be setup for.
  @param LC        - Loop Count
  @param SOE       - [0: Never Stop, 1: Stop on Any Lane, 2:Stop on All Byte, 3: Stop on All Lane]
  @param EnCADB    - Enable test to write random deselect packages on bus to create xtalk/isi
  @param EnCKE     - Enable CKE power down by adding 64

  @retval N/A

**/
void
SetupIOTestMPR(
              PSYSHOST  host,
              UINT8     socket,
              UINT32    ChbitMask,
              UINT8     LC,
              UINT8     SOE,
              UINT8     EnCADB,
              UINT8     EnCKE
              )
{
  TCPGCAddress CPGCAddress = {{0, 0, 0, 0},    // Start
    {0, 0, 0, 1023}, // Stop
    {0, 0, 0, 0},    // Order
    {0, 0, 0, 0},    // IncRate
    {0, 0, 0, 1}};   // IncValue
//    {0, 0, 0, 0}};   // IncValue

  //                         IncRate,  Start, Stop,  DQPat
  //TWDBPattern  WDBPattern = { 32, 0, 9, BasicVA};
  TWDBPattern  WDBPattern = { 32, 0, 0, BasicVA};

//  SetupIOTest(host, socket, ChbitMask, PatRd, (1 << LC), LC, &CPGCAddress, SOE, &WDBPattern, EnCADB, EnCKE, 0);
  SetupIOTest(host, socket, ChbitMask, PatRd, 32, LC, &CPGCAddress, SOE, &WDBPattern, EnCADB, EnCKE, 0);

  host->var.mem.socket[socket].DQPatLC = 1;
  // if (host->var.mem.socket[socket].DQPatLC < 1) host->var.mem.socket[socket].DQPatLC = 1;
  host->var.mem.socket[socket].DQPat = BasicVA;
} // SetupIOTestMPR

/**

  Runs one or more CPGC tests (based on TestType) and returns ch errors

  @param host        - Pointer to sysHost
  @param socket      - Socket from which we are going to read the registers
  @param ChBitMask   - ch Bit mask for which test should be setup for.
  @param DQPat       - DQPat = [0: BasicVA, 1: SegmentWDB, 2: CADB, 3: TurnAround, 4: LMNVa]
  @param SeqLCs      - SeqLCs is an array of one or more loopcounts.
  @param ClearErrors - BOOLEAN to clear or not clear errors
  @param Mode        - mode allows for different types of modes for margining:

  @retval (UINT8)(ChDone & 0xF)

**/
UINT8
RunIOTest(
         PSYSHOST  host,
         UINT8     socket,
         UINT32    ChBitMask,
         UINT8     DQPat,
         UINT8     *SeqLCs,
         UINT8     ClearErrors,
         UINT16    Mode
         )
{
  UINT8   ch;
  UINT8   Reload;
  UINT8   NumTests, t;
  UINT8   mcId;
  UINT32  ChDone = 0;
  UINT32  TestRand;
  UINT32  TRand;
  CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT     patWDBCLCtl;
  CPGC_SUBSEQ0_CTL0_MC_MAIN_STRUCT      subSeqCtl0;
  CPGC_SEQCTL0_MC_MAIN_STRUCT           seqCtl0;
  //CPGC_GLOBALCTRL_MC_MAIN_STRUCT     globalCtrl;

// Runs one or more CPGC tests (based on TestType) and returns ch errors
// WDBIncRates is an array of one or more increments rates for the write data buffer
// SeqLCs is an array of one or more loopcounts.
// DQPat = [0: BasicVA, 1: SegmentWDB, 2: CADB, 3: TurnAround, 4: LMNVa]
// mode allows for different types of modes for margining:
//       mode is {Bit0: PhLock (keep all bytes within in ch in phase),
//               Bit1: Ch2Ch Data out of phase (LFSR seed), Bit 15:2: Reserved}

  UINT8 WDBIncRates[8] = {1, 1, 1, 1, 1, 1, 1, 1};
  UINT8 WDBStart[8]    = {0, 0, 0, 0, 0, 0, 0, 0};
  UINT8 WDBStop[8]     = {9, 9, 9, 9, 9, 9, 9, 9};
  UINT8 pollOnTestComplete = 1;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "RunIOTest Starts\n"));
#endif

  TestRand     = 0xBAD00451;
  NumTests     = 1;

  if (DQPat == SegmentWDB) {
    NumTests = 4;
    // WDBIncRates[3:0]  = [25, 25, 19, 10]
    // WDBStart[3:2]     = 10
    // WDBStop[3:2]      = 63

    WDBIncRates[3] = 10;
    WDBIncRates[2] = 19;
    WDBIncRates[1] = 25;
    WDBIncRates[0] = 25;

    WDBStart[3]    = 10;
    WDBStart[2]    = 10;
    WDBStop[3]     = 63;
    WDBStop[2]     = 63;
  } else if (DQPat == CADB) {
    NumTests = 7;
  } else if (DQPat == TurnAroundWR) {
    NumTests = 8;
  } else if (DQPat == TurnAroundODT) {
    NumTests = 4;
  }

  for (t = 0; t < NumTests; t++) {
    Reload = Log2x32 (WDBIncRates[t]-1);
    if (WDBIncRates[t] > 31) WDBIncRates[t] = Reload;
    else                     WDBIncRates[t] += 32 - 1;

    for (ch = 0; ch < MAX_CH; ch++) {
      if ( !((1 << ch) & ChBitMask )) continue;

      //
      // Check for SOE
      //
      MemReadPciCfgEp (host, socket, ch, CPGC_ERR_CTL_MCDDC_CTL_REG);

      if (Mode & BIT1) {
        // Circular shift by 9*ch
        TRand  = (TestRand >> (9 * ch)) | (TestRand << (32 - (9 * ch)));

        MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG, (0x0EA10CA1 ^ TRand));
        MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG, (0xDEAD0A1E ^ TRand));
        MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG, (0xBEEF0D08 ^ TRand));
        MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG, (0x0EA10CA1 ^ TRand));
        MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG, (0xDEAD0A1E ^ TRand));
        MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG, (0xBEEF0D08 ^ TRand));
      }

      if (DQPat == SegmentWDB) {
        patWDBCLCtl.Data = 0;
        patWDBCLCtl.Bits.incr_rate  = WDBIncRates[t];
        patWDBCLCtl.Bits.start_pntr = WDBStart[t];
        patWDBCLCtl.Bits.end_pntr   = WDBStop[t];
        MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCLCTL_MCDDC_CTL_REG, patWDBCLCtl.Data);

        mcId = GetMCID(host, socket, ch);

        seqCtl0.Data = MemReadPciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
        MemWritePciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG, 0x4);
        // Program desired loopcount
        seqCtl0.Bits.loopcount = ((SeqLCs[t] + 1) & 0x1F);
        // Set MRS Mode and start test
        MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
      } else if (DQPat == CADB) {
        SetupCADB(host, socket, ch, NumTests, t, 0);   // LMNEn=0
      }

      subSeqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50); // Reading Subseq
      mcId = GetMCID(host, socket, ch);
      seqCtl0.Data = MemReadPciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
      if (seqCtl0.Bits.loopcount == 0 || (subSeqCtl0.Bits.num_cachelines==0 && seqCtl0.Bits.loopcount==1)) {
        pollOnTestComplete = 0;
      }

    } // ch loop

    //
    // Start Test and Poll on completion
    //
    ChDone  = 0;

    IO_Reset(host, socket);

    //
    // Global_Start_Test
    //
    //
    // Start test
    //
    CpgcGlobalTestStart (host, socket, ChBitMask);

    if (pollOnTestComplete) {
      //
      // Poll test completion
      //
      CpgcPollGlobalTestDone (host, socket, ChBitMask);
    }
  } // t loop

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "RunIOTest Ends\n"));
#endif

  return(UINT8)(ChDone & 0xF);
} // RunIOTest

/**
  Programs CPGC to run on the selected physical ranks

  @param host              - Pointer to sysHost
  @param socket            - Socket from which we are going to read the registers
  @param ch                - Channel
  @param RankBitMask       - Bit mask of ranks to work on
  @param RankFeatureEnable - RankFeatureEnable is a bit mask that can enable CKE, Refresh or ZQ
  @param SubRankBitMask    - Bit mask of logical ranks

  @retval 0
  @retval (UINT8) (1 << ch)

**/
UINT8
SelectCPGCRanks(
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     RankBitMask,
               UINT8     RankFeatureEnable,
               UINT8     SubRankBitMask
               )
{
  UINT32                                    CRValue;
  UINT8                                     En;
  UINT8                                     dimm;
  UINT8                                     rank;
  UINT8                                     rankCount;
  UINT8                                     mcId;
  UINT8                                     trngTargetSelected;
  CPGC_SEQCTL0_MC_MAIN_STRUCT               seqCtl0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT  seqBaseAddrWrapHi0;
  CPGC_MISCREFCTL_MCDDC_CTL_STRUCT          miscRefCtl;
  CPGC_MISCZQCTL_MCDDC_CTL_STRUCT           miscZqCtl;
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT          miscCKECtl;
  CPGC_SEQ_LRRANKLG2PHYMAP0_MC_MAIN_STRUCT  seqLrRankLg2PhyMap0;
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  struct channelNvram                       (*channelNvList)[MAX_CH];
  UINT8                                     ddrtTarget = 0;
  MCMTR_MC_MAIN_STRUCT                      mcMtr;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT            cpgcGlobalCtrl;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT           ddrtMiscDelay;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SelectCPGCRanks Starts\n"));
#endif

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList    = GetDimmNvList(host, socket, ch);
  mcId          = GetMCID(host, socket, ch);

  // Programs CPGC to run on the selected physical ranks
  // RankFeatureEnable is a bit mask that can enable CKE, Refresh or ZQ
  // RankFeatureEnable[0] enables Refresh on all non-selected ranks
  // RankFeatureEnable[1] enables Refresh on all ranks
  // RankFeatureEnable[2] enables ZQ on all non-selected ranks
  // RankFeatureEnable[3] enables ZQ on all ranks
  // RankFeatureEnable[4] enables CKE on all non-selected ranks
  // RankFeatureEnable[5] enables CKE on all ranks

  // Check if nothing is selected
  if ((RankBitMask & 0xFF) == 0 ) {
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG, 4, 0);

    if (host->nvram.mem.socket[socket].channelList[ch].lrDimmPresent) {
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_LRRANKLG2PHYMAP0_MC_MAIN_REG, 4, 0);
    }

    // Disable ch by clearing global start bit in change config
    seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
    seqCtl0.Bits.global_control = 0;
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

    return 0;

  } else {
    // Normal case
    // Setup GPGC Test to iteration through appropriate ranks during test
    CRValue  = 0;
    rankCount = 0;

    // Prepare Rank Mapping and Max Rank
    //SKX change
    trngTargetSelected = 0;
    for (rank = 0; rank < MAX_RANK_CH; rank++) { //rank in range(4):
      if ( !((1 << rank) & RankBitMask)) continue;

      //Set TRNG_TARGET on selected NVMDIMM ranks; else clear TRNG_TARGET on selected DDR4 ranks
      //Note: SKX design assumes NVMDIMM and DDR4 ranks cannot be selected in the same CPGC sequence
      if ((*dimmNvList)[rank / MAX_RANK_DIMM].aepDimmPresent) {
        if (trngTargetSelected && (ddrtTarget == 0)) {
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_45);
        } else {
          ddrtTarget = 1;
        }
      } else {
        if (trngTargetSelected && (ddrtTarget == 1)) {
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_46);
        } else {
          ddrtTarget = 0;
        }
      }
      trngTargetSelected = 1;

      CRValue |= (rank << (4 * rankCount));
      rankCount += 1;
    } // rank loop

    // Select target rank type
    mcMtr.Data          = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
    cpgcGlobalCtrl.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
    ddrtMiscDelay.Data  = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
    if (ddrtTarget) {
      mcMtr.Bits.trng_target |= (1 << (ch % MAX_MC_CH));
      cpgcGlobalCtrl.Bits.ddrt_addr_mode |= (1 << (ch % MAX_MC_CH));
      ddrtMiscDelay.Bits.force_ddr4_mode = 0;
      ddrtMiscDelay.Bits.force_ddrt_mode = 1;
    } else {
      mcMtr.Bits.trng_target &= ~(1 << (ch % MAX_MC_CH));
      cpgcGlobalCtrl.Bits.ddrt_addr_mode &= ~(1 << (ch % MAX_MC_CH));
      ddrtMiscDelay.Bits.force_ddr4_mode = 1;
      ddrtMiscDelay.Bits.force_ddrt_mode = 0;
    }

    MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);
    MemWritePciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlobalCtrl.Data);
    MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);
    // Write New Rank Mapping and Max Rank
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG, 4, CRValue);


    dimmNvList  = GetDimmNvList(host, socket, ch);
    for (rank = 0; rank < MAX_RANK_CH; rank++) { //rank in range(4):
      if ( !((1 << rank) & RankBitMask)) continue;
      //
      // skip if not in encoded mode
      //
      if ((*channelNvList)[ch].encodedCSMode == 0) continue;

      // convert physical rank mask to dimm number 0/1 -> 0, 4/5 ->1, 6/7 ->2
      dimm = rank / 4;
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((IsLrdimmPresent(host, socket, ch, dimm) && (*dimmNvList)[dimm].aepDimmPresent == 0) || ((*channelNvList)[ch].encodedCSMode == 2)) {
        seqLrRankLg2PhyMap0.Data = 0;
        //
        // figure out where to append the subranks
        //
        // if the dimm has 16 row address bits [0:15] start appending the subrank after a[15]
        if (host->nvram.mem.dramType != SPD_TYPE_DDR3) {
          if ((*dimmNvList)[dimm].numRowBits == 17) {
            seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 4;
          } else if ((*dimmNvList)[dimm].numRowBits == 16) {
            seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 2;
          } else if ((*dimmNvList)[dimm].numRowBits == 15) {
            seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 1;
          }
          //add for 3DS support
          if ((*channelNvList)[ch].encodedCSMode == 2) {
            if ((*dimmNvList)[dimm].numRowBits  == 16) {
              seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 2;
            }
          }
        } else {
          if (((*dimmNvList)[dimm].numRowBits - ((*dimmNvList)[dimm].lrRankMult >> 1)) == 16) {
            seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 2;
          } else if (((*dimmNvList)[dimm].numRowBits - ((*dimmNvList)[dimm].lrRankMult >> 1)) == 15) {
            seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 1;
          }
        }

        seqLrRankLg2PhyMap0.Bits.lr0_mapping = SubRankBitMask;
        MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_LRRANKLG2PHYMAP0_MC_MAIN_REG, 4, seqLrRankLg2PhyMap0.Data);
      }
    } //rank

    seqBaseAddrWrapHi0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_REG, 4);
    seqBaseAddrWrapHi0.Bits.rank_addr = rankCount - 1;
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_REG, 4, seqBaseAddrWrapHi0.Data);

    // Make sure channel is enabled
    seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
    seqCtl0.Bits.global_control = 1;
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  }

  //<VM>- Need to convert RankFeatureEnable as an input parameter so we don't pass it all the time
  if (RankFeatureEnable != 0) {

    //
    // Enable Refresh and ZQ - 0's to the the desired ranks
    //
    // Refresh
    En = RankFeatureEnable & 0x3;
    miscRefCtl.Data = 0;
    if (En == 1) {
      miscRefCtl.Bits.ref_rankmask &= ~RankBitMask; // Enable all non-selected ranks
    } else if (En > 1) {
      miscRefCtl.Bits.ref_rankmask &= 0; // Enable all ranks
    }
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG, miscRefCtl.Data);

    //
    // ZQ
    //
    En = (RankFeatureEnable >> 2) & 0x3;
    miscZqCtl.Data = 0;
    miscZqCtl.Bits.always_do_zq = 0;
    miscZqCtl.Bits.zq_rankmask = 0xF;
    if (En == 1) {
      miscZqCtl.Bits.zq_rankmask &= ~RankBitMask; // Enable all non-selected ranks
    } else if (En > 1) {
      miscZqCtl.Bits.zq_rankmask &= 0; // Enable all ranks
    }
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCZQCTL_MCDDC_CTL_REG, miscZqCtl.Data);

    //
    // Enable CKE ranks - 1's to enable desired ranks
    //
    En = (RankFeatureEnable >> 4) & 0x3;
    miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    // Assume no override
    miscCKECtl.Bits.cke_on = 0xFF;
    if (En == 1) {
      // Enable all non-selected ranks
      miscCKECtl.Bits.cke_override = ~RankBitMask;
      miscCKECtl.Bits.cke_on = ~RankBitMask;
    } else if (En > 1) {
      // Enable all ranks.
      miscCKECtl.Bits.cke_override = 0xFF;
      miscCKECtl.Bits.cke_on = 0xFF;
    }
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
  } else {
    /* TODO: Check if we need to clear these
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCZQCTL_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, 0);
     */
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SelectCPGCRanks Ends\n"));
#endif

  return(UINT8) (1 << ch);
} // SelectGPGCRanks

/**

  this function writes to CADB via MRS mode

  @param host            - include all the MRC data
  @param socket             - Memory Controller
  @param ch              - ch to send command to
  @param rank            - CS to send the command to
  @param Command         - 0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP
  @param Bank            - MRS command to be sent
  @param Data            - Value to be sent
  @param Delay           - Delay in Dclocks

@retval UINT32 -  SUCCESS if the reset is succeded

**/
void
RPQDrainCadbMRSMode (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     dimm,
             UINT8     rank
             )
{
  UINT32                                readCount;
  UINT8                                 i;
  UINT32                                CRValueSave;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT      cpgcPatCadbCtlReg;
  CPGC_PATCADBMRS_MCDDC_CTL_STRUCT      patCADBMrs;
  CPGC_SEQCTL0_MC_MAIN_STRUCT           seqCtl0;
  CPGC_DDRT_MISC_CTL_MC_2LM_STRUCT      miscCtlMc2LM;
  PatCadbProg0                          ReadCommandsMRSModeLocal0[CADB_LINES];
  PatCadbProg1                          ReadCommandsMRSModeLocal1[CADB_LINES];
  struct channelNvram                   (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  //
  // B0 Fix for draining RPQ
  //
  if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                "B0 Fix for RPQ drain\n"));

    miscCtlMc2LM.Data = MemReadPciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG);
    miscCtlMc2LM.Bits.cpgc_rpq_force_drain = 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG, miscCtlMc2LM.Data);


    miscCtlMc2LM.Bits.cpgc_rpq_force_drain = 0;
    MemWritePciCfgEp (host, socket, ch, CPGC_DDRT_MISC_CTL_MC_2LM_REG, miscCtlMc2LM.Data);
  } else {
   //
   // A0 method for draining RPQ
   //
    readCount = GetPmonCounter(host,socket,ch);
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                "RPQ occupancy count is 0x%x\n", readCount));

    while (readCount != 0) {
      cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
      cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
      cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
      cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

      for (i = 0; i < CADB_LINES; i++) {
        ReadCommandsMRSModeLocal0[i] = ReadCommandsMRSMode0[i];
        ReadCommandsMRSModeLocal1[i] = ReadCommandsMRSMode1[i];
      }

      FixupCS(GetPhyRank(dimm, rank), 0, ReadCommandsMRSModeLocal0, ReadCommandsMRSModeLocal1, (*channelNvList)[ch].encodedCSMode);
      ProgramCADB (host, socket, ch, 0, CADB_LINES, ReadCommandsMRSModeLocal0, ReadCommandsMRSModeLocal1);

      // Issuing only one read at a time
      patCADBMrs.Data                     = 0;
      patCADBMrs.Bits.mrs_gap             = 4;
      patCADBMrs.Bits.mrs_start_pointer   = 0;
      patCADBMrs.Bits.mrs_end_pointer     = 0;

      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

      CRValueSave = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
      seqCtl0.Data                      = 0;
      seqCtl0.Bits.initialization_mode  = MRS_Mode;
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);


      //
      // Start test and clear errors
      // TODO: Global
      seqCtl0.Bits.local_clear_errs  = 1;
      seqCtl0.Bits.local_start_test  = 1;
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

      CpgcPollGlobalTestDone (host, socket, 1 << ch);

      //
      // Restore after MR command
      //
      seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
      seqCtl0.Bits.local_start_test = 0;
      seqCtl0.Bits.local_clear_errs = 1;
      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);


      MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, CRValueSave);


      // Check if more reads need to be issued
      readCount = GetPmonCounter(host,socket,ch);
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                    "RPQ occupancy count is 0x%x\n", readCount));
    }
  }
}

/**

  this function writes to CADB

  @param host            - include all the MRC data
  @param socket             - Memory Controller
  @param ch              - ch to send command to
  @param rank            - CS to send the command to
  @param Command         - 0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP
  @param Bank            - MRS command to be sent
  @param Data            - Value to be sent
  @param Delay           - Delay in Dclocks

@retval UINT32 -  SUCCESS if the reset is succeded

**/
UINT32
WriteCADBCmd (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     dimm,
             UINT8     rank,
             UINT8     Command,
             UINT8     Bank,
             UINT32    Data,
             UINT8     Delay
             )
{
  UINT8                                 count;
  UINT8                                 i;
  UINT8                                 currentRow;
  UINT32                                status = SUCCESS;
  UINT32                                CRValueSave;
  PatCadbProg0                          patCADBProg0;
  PatCadbProg1                          patCADBProg1;
  CPGC_PATCADBMRS_MCDDC_CTL_STRUCT      patCADBMrs;
  CPGC_SEQCTL0_MC_MAIN_STRUCT           seqCtl0;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT      cpgcPatCadbCtlReg;
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT      miscCKECtl;
  struct ddrRank                          (*rankList)[MAX_RANK_DIMM];
  struct channelNvram                   (*channelNvList)[MAX_CH];

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                 "WriteCADBCmd Starts: Command 0x%02x, Bank 0x%02x, Data 0x%04x\n", Command, Bank, Data));
#endif

  rankList = GetRankNvList(host, socket, ch, dimm);
  channelNvList = GetChannelNvList(host, socket);
  currentRow = 0;

  // Temporarily disable 3DS mode for MRS commands
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 0);
  }

  //
  // Pointer will be dynamically incremented after a write to CADB_PROG register
  //
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, 0);
#ifdef MRS_STACKING
  if ((host->var.mem.socket[socket].cadbMRSMode == NORMAL_MRS) || (host->var.mem.socket[socket].cadbMRSMode & EXECUTE_MRS))
#endif
  {
    cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
    cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
    cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
    cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);
  }

#ifdef MRS_STACKING
  if ((host->var.mem.socket[socket].cadbMRSMode == NORMAL_MRS) || (host->var.mem.socket[socket].cadbMRSMode & LOAD_MRS))
#endif
  {
    patCADBProg0.address = (0x1FFFF & Data);
    patCADBProg0.bank    = Bank;
    patCADBProg0.cid     = 0;
    if (IsLrdimmPresent(host, socket, ch, dimm) && ((*channelNvList)[ch].encodedCSMode == 1)) {
      // here we are in encoded quad CS mode
        patCADBProg0.cid     = (rank >> 1) | 0x6;
    }
    patCADBProg0.pdatag  = 0;

    if (Command == RCW_CMD) {
      patCADBProg1.cs      = ~((1 << (*rankList)[rank].CSIndex) + (1 << ((*rankList)[rank].CSIndex + 1)));
    } else {
      patCADBProg1.cs      = ~(1 << (*rankList)[rank].CSIndex);
    }
    if (Command == RCW_CMD) {
      patCADBProg1.control = 0;
    } else {
      patCADBProg1.control = Command;
    }

    // zero out CID0/1 as CS's bits
    if ((*channelNvList)[ch].encodedCSMode == 2) {
      patCADBProg1.cs = patCADBProg1.cs  & ~(0xc<<(dimm*4));
    }

    patCADBProg1.odt = 0;
    patCADBProg1.cke = 0;
    miscCKECtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
    if ((miscCKECtl.Bits.cke_override == 0) || (miscCKECtl.Bits.cke_override & miscCKECtl.Bits.cke_on)) {
      patCADBProg1.cke = 0x3F;
    }

    //
    // Assert ACT bit
    //
    patCADBProg1.control |= BIT3;

    //
    // Calculate parity
    //
    count = 0;

    // Count the 1's in CID bits if they are to be included in the parity calculation
    if ((*channelNvList)[ch].encodedCSMode == 2) {
      if (((*channelNvList)[ch].cidBitMap & BIT2) && (patCADBProg0.cid & BIT2))
        count++;
      if (((*channelNvList)[ch].cidBitMap & BIT1) && (patCADBProg0.cid & BIT1))
        count++;
      if (((*channelNvList)[ch].cidBitMap & BIT0) && (patCADBProg0.cid & BIT0))
        count++;
    }

    // Count the 1's in the address bits
    for(i = 0; i < 17; i++) {
      if (patCADBProg0.address & (1 << i)) {
        count++;
      }
    } // i loop

    // Count the 1's in the bank bits
    for(i = 0; i < 4; i++) {
      if (patCADBProg0.bank & (1 << i)) {
        count++;
      }
    } // i loop

    // Count the 1's in the control bits
    for(i = 0; i < 4; i++) {
      if (patCADBProg1.control & (1 << i)) {
        count++;
      }
    } // i loop

    if (count & BIT0) {
      patCADBProg1.par = 1;
    } else {
      patCADBProg1.par = 0;
    }
  }

#ifdef MRS_STACKING
  if (host->var.mem.socket[socket].cadbMRSMode & LOAD_MRS){
    ProgramCADB (host, socket, ch, host->var.mem.socket[socket].cadbMRSIndex[ch], host->var.mem.socket[socket].cadbMRSIndex[ch] + 1, &patCADBProg0, &patCADBProg1);
    if (host->var.mem.socket[socket].cadbMRSIndex[ch] >= CADB_CACHELINE_LIMIT) host->var.mem.socket[socket].cadbMRSMode = EXECUTE_MRS;
    host->var.mem.socket[socket].cadbMRSIndex[ch]++;
  }
  if (host->var.mem.socket[socket].cadbMRSMode == NORMAL_MRS)
#endif
  {
    if (host->var.mem.socket[socket].hostRefreshStatus && ((Command == RCW_CMD) || (Command == MRS_CMD) || (Command == ZQ_CMD))) {
      //
      // Insert deselect before RCW or MRS or ZQCL commands when host refresh is enabled to make sure
      // we do not violate tRFC
      //
      ProgramCADB (host, socket, ch, currentRow, currentRow + 1, &patCADBProg0, &DeselectPattern1[0]);
      currentRow = currentRow + 1;
    }
    ProgramCADB (host, socket, ch, currentRow, currentRow + 1, &patCADBProg0, &patCADBProg1);
    currentRow = currentRow + 1;
    if (host->var.mem.socket[socket].hostRefreshStatus && ((Command == RCW_CMD) || (Command == MRS_CMD) || (Command == ZQ_CMD))) {
      //
      // Add a deselect after RCW or MRS or ZQCL commands when host refresh is enabled to make sure
      // we do not violate tMOD
      //
      ProgramCADB (host, socket, ch, currentRow, currentRow + 1, &patCADBProg0, &DeselectPattern1[0]);
      currentRow = currentRow + 1;
    }
  }

#ifdef MRS_STACKING
  if ((host->var.mem.socket[socket].cadbMRSMode == NORMAL_MRS) || (host->var.mem.socket[socket].cadbMRSMode & EXECUTE_MRS))
#endif
  {
    //
    // Execute MRS Mode
    //
    if (Delay < 6) Delay = 6;

  patCADBMrs.Data                     = 0;
  if (host->var.mem.socket[socket].hostRefreshStatus == 1) {
    patCADBMrs.Bits.mrs_gap = 9;
  }
  else {
    patCADBMrs.Bits.mrs_gap = Delay;
  }
  patCADBMrs.Bits.mrs_start_pointer = 0;
#ifdef MRS_STACKING
  if(host->var.mem.socket[socket].cadbMRSIndex[ch] !=0) patCADBMrs.Bits.mrs_end_pointer     = host->var.mem.socket[socket].cadbMRSIndex[ch] - 1;
  else  patCADBMrs.Bits.mrs_end_pointer     = 0;
#else
    patCADBMrs.Bits.mrs_end_pointer     = currentRow - 1;
#endif
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

  //
  // Save before MR command
  //
  CRValueSave = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Data                      = 0;
  seqCtl0.Bits.initialization_mode  = MRS_Mode;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  //
  // Start test and clear errors
  //
  seqCtl0.Bits.local_clear_errs  = 1;
  seqCtl0.Bits.local_start_test  = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  //
  // Poll test completion
  //
  CpgcPollGlobalTestDone (host, socket, 1 << ch);

  //
  // Restore after MR command
  //
  seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Bits.local_start_test = 0;
  seqCtl0.Bits.local_clear_errs = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, CRValueSave);

#ifdef MRS_STACKING
  if (host->var.mem.socket[socket].cadbMRSIndex[ch] > CADB_CACHELINE_LIMIT) {
    host->var.mem.socket[socket].cadbMRSMode = LOAD_MRS;
    host->var.mem.socket[socket].cadbMRSIndex[ch] = 0;
  }
#endif
  }

  // Re-enable 3DS mode
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 1);
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, "WriteCADBCmd Ends\n"));
#endif

  return status;
} // WriteCADBCmd

/**

Routine Description:

  This function allows stacking of MRS commands in cadb buffer in writeCADBCmd function. Stacking commands in CADB
  is more efficient as the MRS are only ~32 clks apart instead of spread out by a bunch of CSR accesses

Arguments:
  host            - include all the MRC data
  socket          - Memory Controller
  ch              - ch to send command to
  dimm            - dimm to send command to
  Command         - NORMAL_MRS  - How writeCADBcmd usually works
                  - LOAD_MRS - writeCADBcmd will load commands into cadb buffer but not execute
                  - EXECUTE_MRS - writeCADBcmd will execute the stack of commands


Returns:
 UINT32 -  SUCCESS if the reset is succeded

--*/
void
MRSCmdStacking(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     Command
)
{
#ifdef MRS_STACKING
  host->var.mem.socket[socket].cadbMRSMode = Command;

  if (Command == LOAD_MRS) {
    host->var.mem.socket[socket].cadbMRSIndex[ch]=0;
  }
  if (Command == EXECUTE_MRS) {
    if(host->var.mem.socket[socket].cadbMRSIndex[ch] != 0) {  //Check for exactly 16 CADB commands
      //WriteCADBCmd(host, socket, ch, dimm, rank, Command, Bank, Data, Delay);
      WriteCADBCmd(host, socket, ch, dimm, 0, 0, 0, 0, 6); //Delay =2**6 Need to support NVMCTLR
    }
    host->var.mem.socket[socket].cadbMRSMode = NORMAL_MRS;                 // return to normal MRS mode
    host->var.mem.socket[socket].cadbMRSIndex[ch]=0;                           // reset index after execute
  }
#endif
} //MRSCmdStacking

void
WriteMRSPDA_WA(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     dram,
  UINT16    data,
  UINT8     bank
  )
{
  UINT8                             side;
  UINT8                             mcId;
  UINT8                             maxSide;
  UINT8                             currentRow;
  UINT8                             desel1 = 0;
  UINT8                             desel2 = 0;
  UINT8                             seq1 = 0;
  UINT16                            tempPrk;
  UINT32                            CRValueSave = 0;
  PatCadbProg0                      patCADBProg0[16];
  PatCadbProg1                      patCADBProg1[16];
  CPGC_PATCADBMRS_MCDDC_CTL_STRUCT  patCADBMrs;
  CPGC_SEQCTL0_MC_MAIN_STRUCT       seqCtl0;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT  cpgcPatCadbCtlReg;
  CPGC_PDA_MCDDC_DP_STRUCT          cpgcPDA;
  CPGC_PDA_SHADOW_MCDDC_CTL_STRUCT  cpgcPDAShadow;
  MCMTR_MC_MAIN_STRUCT              mcMtr;
  MCMTR_MC_MAIN_STRUCT              mcMtrSafe;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  struct channelNvram               (*channelNvList)[MAX_CH];

  if (host->var.mem.socket[socket].hostRefreshStatus == 1) {
    FixedDelay(host, 10);
  }

  channelNvList = GetChannelNvList(host, socket);
  rankList      = GetRankNvList(host, socket, ch, dimm);
  rankStruct    = GetRankStruct(host, socket, ch, dimm);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, dram, NO_BIT,
                 "DDR4 PDA write: bank 0x%02X, data 0x%04X\n", bank, data));
#endif

  //override PRK to WR
  tempPrk = (*rankStruct)[rank].MR5 & ~0x1c0; // zero out bits, 6,7,8 - RTT_PRK
  switch ((*rankList)[rank].RttWr >> DDR4_RTT_WR_ODT_SHIFT) {
    case 0:
      tempPrk |= DDR4_RTT_PARK_DIS<<6; // disable/off
      break;
    case 1:
      tempPrk |= DDR4_RTT_PARK_120<<6;   // PRK =120
      break;
    case 2:
      tempPrk |= DDR4_RTT_PARK_240<<6;   // PRK = 240
      break;
    case 3:
      tempPrk |= DDR4_RTT_PARK_INF<<6;  // PRK = disable
      break;
  }
  WriteMRS (host, socket, ch, dimm, rank, tempPrk, BANK5);

  // Temporarily disable 3DS mode for MRS commands
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 0);
  }

  // HSD 5330727 - PDA not driving DQ low when scheduler is set to DDRT target
  mcId = GetMCID(host, socket, ch);
  mcMtrSafe.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
  mcMtr.Data = mcMtrSafe.Data;
  mcMtr.Bits.trng_target = 0;
  MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);

  //
  // Pointer will be dynamically incremented after a write to CADB_PROG register
  //
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, 0);

  //
  // Save before MR command
  //
  CRValueSave = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Data                      = 0;
  seqCtl0.Bits.initialization_mode  = MRS_Mode;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  if ((host->nvram.mem.dimmTypePresent == RDIMM) && (bank != 7)) {
    maxSide = SIDE_B;
  } else {
    maxSide = SIDE_A;
  }

  cpgcPDA.Data = 0;
  cpgcPDAShadow.Data = 0;

  for (side = SIDE_A; side <= maxSide; side++) {
    //
    // Select the device(s) to be written to
    //
    if (dram == ALL_DRAMS) {
      cpgcPDA.Bits.device_select = 0;
    } else {
      cpgcPDA.Bits.device_select = ~(1 << dram) & ((1 << 18) - 1);
    }

    //
    // Select the rank to be written to
    //
    cpgcPDA.Bits.rank_select        = (*rankList)[rank].logicalRank;
    cpgcPDAShadow.Bits.rank_select  = (*rankList)[rank].logicalRank;

    MemWritePciCfgEp (host, socket, ch, CPGC_PDA_MCDDC_DP_REG, cpgcPDA.Data);
    MemWritePciCfgEp (host, socket, ch, CPGC_PDA_SHADOW_MCDDC_CTL_REG, cpgcPDAShadow.Data);

    currentRow = 0;
    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Placeholder for deselect
      //
      currentRow = currentRow + 1;
    }

    //
    // Program CADB row 0 (or row 1 if hostRefreshStatus is enabled) to enter PDA mode
    //
    ProgCADBRow (host, socket, ch, dimm, rank, BANK3, (*rankStruct)[rank].MR3 | MR3_PDA, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB row 1 (or row 2 if hostRefreshStatus is enabled) to enter Vref training mode
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data | BIT7, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB row 2 (or row 3 if hostRefreshStatus is enabled) to execute the PDA write
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data | BIT7, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB row 3 (or row 4 if hostRefreshStatus is enabled) to exit Vref training mode
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data & ~BIT7, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;
    seq1 = currentRow;

    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Placeholder for 2 deselects
      //
      currentRow = currentRow + 1;
      desel1 = currentRow;
      seq1 = currentRow;
      currentRow = currentRow + 1;
      desel2 = currentRow;
    }

    //
    // Program CADB row 2 and 3 to exit PDA mode because of HSD 5330475
    //
    ProgCADBRow(host, socket, ch, dimm, rank, BANK3, (*rankStruct)[rank].MR3, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    ProgCADBRow (host, socket, ch, dimm, rank, BANK3, (*rankStruct)[rank].MR3, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Placeholder for deselect
      //
      currentRow = currentRow + 1;
    }

    ProgramCADB (host, socket, ch, 0, currentRow, patCADBProg0, patCADBProg1);       // program actual CADB registers

    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Add a deselect before and after MRS commands when host controlled refreshes are enabled to make sure
      // we do not violate timings
      //
      ProgramCADB (host, socket, ch, 0, 1, &patCADBProg0[0], &DeselectPattern1[0]);
      ProgramCADB (host, socket, ch, desel1 - 1, desel1, &patCADBProg0[0], &DeselectPattern1[0]);
      ProgramCADB (host, socket, ch, desel2 - 1, desel2, &patCADBProg0[0], &DeselectPattern1[0]);
      ProgramCADB (host, socket, ch, currentRow - 1, currentRow, &patCADBProg0[0], &DeselectPattern1[0]);
    }

    //
    // Execute MRS Mode for first 2 entries
    //
    patCADBMrs.Data = 0;
    patCADBMrs.Bits.mrs_gap = 9;

    patCADBMrs.Bits.mrs_start_pointer = 0;
    patCADBMrs.Bits.mrs_end_pointer = seq1 - 1;
    MemWritePciCfgEp(host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

    // Start test and clear errors
    seqCtl0.Bits.local_clear_errs  = 1;
    seqCtl0.Bits.local_start_test  = 1;
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

    //
    // Poll test completion
    //
    CpgcPollGlobalTestDone (host, socket, 1 << ch);

    //
    // Execute MRS Mode for last 2 entries
    //
    patCADBMrs.Bits.mrs_start_pointer   = seq1;
    patCADBMrs.Bits.mrs_end_pointer     = currentRow - 1;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

    // change device_select to 0 to disable PDA for all DRAMS
    cpgcPDA.Bits.device_select = 0;
    MemWritePciCfgEp (host, socket, ch, CPGC_PDA_MCDDC_DP_REG, cpgcPDA.Data);

    // Start test and clear errors
    seqCtl0.Bits.local_clear_errs  = 1;
    seqCtl0.Bits.local_start_test  = 1;
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

    // Poll test completion
    CpgcPollGlobalTestDone (host, socket, 1 << ch);
  } // side loop

  //
  // Restore after MR command
  //
  seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Bits.local_start_test = 0;
  seqCtl0.Bits.local_clear_errs = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, CRValueSave);

  // HSD 5330727 - PBA not driving DQ low when scheduler is set to DDRT target
  // Restore original value
  MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtrSafe.Data);

  // Re-enable 3DS mode
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 1);
  }

  //Undo PRK override
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);

} // WriteMRSPDA_WA

/**

  this function writes to CADB

  @param host            - include all the MRC data
  @param socket          - Memory Controller
  @param ch              - ch to send command to
  @param dimm            - dimm to send command to
  @param rank            - CS to send the command to
  @param Command         - 0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP
  @param Bank            - MRS command to be sent
  @param Data            - Value to be sent

**/
void
WriteMRSPDA(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     dram,
  UINT16    data,
  UINT8     bank
  )
{
  UINT8                             side;
  UINT8                             mcId;
  UINT8                             maxSide;
  UINT8                             currentRow;
  UINT16                            tempPrk;
  UINT32                            CRValueSave = 0;
  PatCadbProg0                      patCADBProg0[7];
  PatCadbProg1                      patCADBProg1[7];
  CPGC_PATCADBMRS_MCDDC_CTL_STRUCT  patCADBMrs;
  CPGC_SEQCTL0_MC_MAIN_STRUCT       seqCtl0;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT  cpgcPatCadbCtlReg;
  CPGC_PDA_MCDDC_DP_STRUCT          cpgcPDA;
  CPGC_PDA_SHADOW_MCDDC_CTL_STRUCT  cpgcPDAShadow;
  MCMTR_MC_MAIN_STRUCT              mcMtr;
  MCMTR_MC_MAIN_STRUCT              mcMtrSafe;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  struct channelNvram               (*channelNvList)[MAX_CH];

  //
  // SKX TODO: PDA two part sequence WA being forced, This needs to be resolved as this violates NVDIMM spec
  // SKX Si HSD: 5353497
  //
  if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
    WriteMRSPDA_WA(host, socket, ch, dimm, rank, dram, data, bank);
    return;
  }

  if (host->var.mem.socket[socket].hostRefreshStatus == 1) {
    FixedDelay(host, 10);
  }

  channelNvList = GetChannelNvList(host, socket);
  rankList      = GetRankNvList(host, socket, ch, dimm);
  rankStruct    = GetRankStruct(host, socket, ch, dimm);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, dram, NO_BIT,
                 "DDR4 PDA write: bank 0x%02X, data 0x%04X\n", bank, data));
#endif

  // Override PRK to WR
  tempPrk = (*rankStruct)[rank].MR5 & ~0x1c0; // zero out bits, 6,7,8 - RTT_PRK
  switch ((*rankList)[rank].RttWr >> DDR4_RTT_WR_ODT_SHIFT) {
    case 0:
      tempPrk |= DDR4_RTT_PARK_DIS<<6; // disable/off
      break;
    case 1:
      tempPrk |= DDR4_RTT_PARK_120<<6;   // PRK =120
      break;
    case 2:
      tempPrk |= DDR4_RTT_PARK_240<<6;   // PRK = 240
      break;
    case 3:
      tempPrk |= DDR4_RTT_PARK_INF<<6;  // PRK = disable
      break;
  }
  WriteMRS (host, socket, ch, dimm, rank, tempPrk, BANK5);

  // Temporarily disable 3DS mode for MRS commands
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 0);
  }

  // HSD 5330727 - PDA not driving DQ low when scheduler is set to DDRT target
  mcId = GetMCID(host, socket, ch);
  mcMtrSafe.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
  mcMtr.Data = mcMtrSafe.Data;
  mcMtr.Bits.trng_target = 0;
  MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);

  //
  // Pointer will be dynamically incremented after a write to CADB_PROG register
  //
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, 0);

  //
  // Save before MR command
  //
  CRValueSave = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Data                      = 0;
  seqCtl0.Bits.initialization_mode  = MRS_Mode;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  if ((host->nvram.mem.dimmTypePresent == RDIMM) && (bank != 7)) {
    maxSide = SIDE_B;
  } else {
    maxSide = SIDE_A;
  }

  cpgcPDA.Data       = 0;
  cpgcPDAShadow.Data = 0;

  for (side = SIDE_A; side <= maxSide; side++) {
    //
    // Select the device(s) to be written to
    //
    if (dram == ALL_DRAMS) {
      cpgcPDA.Bits.device_select = 0;
    } else {
      cpgcPDA.Bits.device_select = ~(1 << dram) & ((1 << 18) - 1);
    }

    //
    // Select the rank to be written to
    //
    cpgcPDA.Bits.rank_select        = (*rankList)[rank].logicalRank;
    cpgcPDAShadow.Bits.rank_select  = (*rankList)[rank].logicalRank;

    MemWritePciCfgEp (host, socket, ch, CPGC_PDA_MCDDC_DP_REG, cpgcPDA.Data);
    MemWritePciCfgEp (host, socket, ch, CPGC_PDA_SHADOW_MCDDC_CTL_REG, cpgcPDAShadow.Data);

    currentRow = 0;
    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Placeholder for deselect
      //
      currentRow = currentRow + 1;
    }

    //
    // Program CADB row 0 (or row 1 if hostRefreshStatus is enabled) to enter PDA mode
    //
    ProgCADBRow (host, socket, ch, dimm, rank, BANK3, (*rankStruct)[rank].MR3 | MR3_PDA, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB row 1 (or row 2 if hostRefreshStatus is enabled) to enter Vref training mode
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data | BIT7, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB row 2 (or row 3 if hostRefreshStatus is enabled) to execute the PDA write
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data | BIT7, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB row 3 (or row 4 if hostRefreshStatus is enabled) to exit Vref training mode
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data & ~BIT7, side, ENTER_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB row 4 (or row 5 if hostRefreshStatus is enabled) to exit PDA mode
    //
    ProgCADBRow (host, socket, ch, dimm, rank, BANK3, (*rankStruct)[rank].MR3, side, EXIT_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Placeholder for deselect
      //
      currentRow = currentRow + 1;
    }

    ProgramCADB (host, socket, ch, 0, currentRow, patCADBProg0, patCADBProg1);       // program actual CADB registers

    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Add a deselect before and after MRS commands when host controlled refreshes are enabled to make sure
      // we do not violate timings
      //
      ProgramCADB (host, socket, ch, 0, 1, &patCADBProg0[0], &DeselectPattern1[0]);
      ProgramCADB (host, socket, ch, currentRow - 1, currentRow, &patCADBProg0[0], &DeselectPattern1[0]);
    }

    patCADBMrs.Data = 0;
      //
      // Use a larger gap when refreshes are enabled to ensure we do not violate refresh timings
      //
    patCADBMrs.Bits.mrs_gap = 9;

    patCADBMrs.Bits.mrs_start_pointer = 0;
    patCADBMrs.Bits.mrs_end_pointer = currentRow - 1;
    MemWritePciCfgEp(host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

    // Start test and clear errors
    seqCtl0.Bits.local_clear_errs  = 1;
    seqCtl0.Bits.local_start_test  = 1;
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

    //
    // Poll test completion
    //
    CpgcPollGlobalTestDone (host, socket, 1 << ch);

  } // side loop

  //
  // Restore after MR command
  //
  seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Bits.local_start_test = 0;
  seqCtl0.Bits.local_clear_errs = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, CRValueSave);

  // HSD 5330727 - PBA not driving DQ low when scheduler is set to DDRT target
  // Restore original value
  MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtrSafe.Data);

  // Re-enable 3DS mode
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 1);
  }

  //Undo PRK override
  WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);

} // WriteMRSPDA

void
WriteMR6(
PSYSHOST  host,
UINT8     socket,
UINT8     ch,
UINT8     dimm,
UINT8     rank,
UINT16    data,
UINT8     bank
)
/*++

this function writes to CADB

@param host            - include all the MRC data
@param socket          - Memory Controller
@param ch              - ch to send command to
@param dimm            - dimm to send command to
@param rank            - CS to send the command to
@param Bank            - MRS command to be sent
@param Data            - Value to be sent

--*/
{
  UINT8                                   side;
  UINT8                                   mcId;
  UINT8                                   maxSide;
  UINT8                                   currentRow;
  UINT16                                  tempPrk;
  UINT32                                  CRValueSave = 0;
  struct ddrRank(*rankList)[MAX_RANK_DIMM];
  struct rankDevice(*rankStruct)[MAX_RANK_DIMM];
  PatCadbProg0                            patCADBProg0[16];
  PatCadbProg1                            patCADBProg1[16];
  CPGC_PATCADBMRS_MCDDC_CTL_STRUCT        patCADBMrs;
  CPGC_SEQCTL0_MC_MAIN_STRUCT             seqCtl0;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT        cpgcPatCadbCtlReg;
  MCMTR_MC_MAIN_STRUCT                    mcMtr;
  MCMTR_MC_MAIN_STRUCT                    mcMtrSafe;
  struct channelNvram(*channelNvList)[MAX_CH];


  if (host->var.mem.socket[socket].hostRefreshStatus == 1) {
    FixedDelay(host, 10);
  }
  channelNvList = GetChannelNvList(host, socket);
  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct = GetRankStruct(host, socket, ch, dimm);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
    "DDR4 PDA write: bank 0x%02X, data 0x%04X\n", bank, data));
#endif

  //override PRK to WR
  tempPrk = (*rankStruct)[rank].MR5 & ~0x1c0; // zero out bits, 6,7,8 - RTT_PRK
  switch ((*rankList)[rank].RttWr >> DDR4_RTT_WR_ODT_SHIFT) {
  case 0:
    tempPrk |= DDR4_RTT_PARK_DIS << 6; // disable/off
    break;
  case 1:
    tempPrk |= DDR4_RTT_PARK_120 << 6;   // PRK =120
    break;
  case 2:
    tempPrk |= DDR4_RTT_PARK_240 << 6;   // PRK = 240
    break;
  case 3:
    tempPrk |= DDR4_RTT_PARK_INF << 6;  // PRK = disable
    break;
  }
  WriteMRS(host, socket, ch, dimm, rank, tempPrk, BANK5);

  // Temporarily disable 3DS mode for MRS commands
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 0);
  }

  // HSD 5330727 - PDA not driving DQ low when scheduler is set to DDRT target
  mcId = GetMCID(host, socket, ch);
  mcMtrSafe.Data = MemReadPciCfgMC(host, socket, mcId, MCMTR_MC_MAIN_REG);
  mcMtr.Data = mcMtrSafe.Data;
  mcMtr.Bits.trng_target = 0;
  MemWritePciCfgMC(host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);
  //
  // Pointer will be dynamically incremented after a write to CADB_PROG register
  //
  MemWritePciCfgEp(host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, 0);

  //
  // Save before MR command
  //
  CRValueSave = MemReadPciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Data = 0;
  seqCtl0.Bits.initialization_mode = MRS_Mode;
  MemWritePciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp(host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
  MemWritePciCfgEp(host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  if ((host->nvram.mem.dimmTypePresent == RDIMM) && (bank != 7)) {
    maxSide = SIDE_B;
  } else {
    maxSide = SIDE_A;
  }

  for (side = SIDE_A; side <= maxSide; side++) {
    currentRow = 0;
    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Placeholder for deselect on entry
      //
      currentRow = currentRow + 1;
    }

    //
    // Program CADB next row to execute (row 0 in case of normal or row 1 when hostRefreshStatus is enabled)
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data | BIT7, side, NO_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB next row to execute (row 1 in case of normal or row 2 when hostRefreshStatus is enabled)
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data | BIT7, side, NO_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    //
    // Program CADB next row to execute (row 2 in case of normal or row 3 when hostRefreshStatus is enabled)
    //
    ProgCADBRow(host, socket, ch, dimm, rank, bank, data & ~BIT7, side, NO_PDA, MRS_CMD, &patCADBProg0[currentRow], &patCADBProg1[currentRow]);
    currentRow = currentRow + 1;

    if (host->var.mem.socket[socket].hostRefreshStatus) {
      //
      // Placeholder for deselect on exit
      //
      currentRow = currentRow + 1;
    }

    ProgramCADB(host, socket, ch, 0, currentRow, patCADBProg0, patCADBProg1);

    if (host->var.mem.socket[socket].hostRefreshStatus) {

      //
      // Add a deselect after RCW or MRS commands when host controlled refreshes are enabled to make sure
      // we do not violate tMOD
      //
      ProgramCADB(host, socket, ch, 0, 1, &patCADBProg0[0], &DeselectPattern1[0]);
      ProgramCADB(host, socket, ch, currentRow - 1, currentRow, &patCADBProg0[0], &DeselectPattern1[0]);
    }

    //
    // Execute MRS Mode
    //
    patCADBMrs.Data = 0;
    patCADBMrs.Bits.mrs_gap = 9;

    patCADBMrs.Bits.mrs_start_pointer = 0;
    patCADBMrs.Bits.mrs_end_pointer = currentRow - 1;
    MemWritePciCfgEp(host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

    // Start test and clear errors
    seqCtl0.Bits.local_clear_errs = 1;
    seqCtl0.Bits.local_start_test = 1;
    MemWritePciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

    //
    // Poll test completion
    //
    CpgcPollGlobalTestDone(host, socket, 1 << ch);
  } // side loop

  //
  // Restore after MR command
  //
  seqCtl0.Data = MemReadPciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Bits.local_start_test = 0;
  seqCtl0.Bits.local_clear_errs = 1;
  MemWritePciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  MemWritePciCfgMC_Ch(host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, CRValueSave);

  // HSD 5330727 - PBA not driving DQ low when scheduler is set to DDRT target
  // Restore original value
  MemWritePciCfgMC(host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtrSafe.Data);

  // Re-enable 3DS mode
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 1);
  }

  //Undo PRK override
  WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
} // WriteMR6


#ifdef LRDIMM_SUPPORT

static INT16
GetMinDwl(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
)
{
  INT16                                   minDwl;
  INT16                                   cwlAdj;
  TCOTHP_MCDDC_CTL_STRUCT                 tCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT                tCOTHP2;

  tCOTHP.Data = WLChipCleanUpStruct->tCOTHP.Data;
  tCOTHP2.Data = WLChipCleanUpStruct->tCOTHP2.Data;

  minDwl = -2;

  if(host->nvram.mem.hostFlybyDone == 0) {
    cwlAdj = -3; // write absolute minimum CWL_ADJ of -3
  } else {
    // offset by minDwl (-2)
    cwlAdj = minDwl + (INT16)tCOTHP.Bits.t_cwl_adj - (INT16)tCOTHP2.Bits.t_cwl_adj_neg;

    // check for minimum value
    if (cwlAdj < -3) {
        cwlAdj = -3;
    }
  }

  return cwlAdj;
}

void
WritePBA (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     *controlWordDataPtr,
  UINT8     controlWordFunc,
  UINT8     controlWordAddr,
  UINT8     bufferAddr
  )
/*++

Routine Description:

  this function writes to CADB

Arguments:
  host            - include all the MRC data
  socket          - Memory Controller
  ch              - ch to send command to
  dimm            - dimm to send command to
  rank            - CS to send the command to
  controlWordDataPtr - Pointer to a 9 byte array of control word data
  controlWordFunc - Function index in the LR buffer
  controlWordAddr - Control Word Address
  bufferAddr      - Address of the individual buffer to target...or 'FF' for all buffers

Returns:
  N/A

--*/
{
  UINT8                                   mcId;
  UINT8                                   strobe;
  UINT32                                  CRValueSave = 0;
  struct dimmNvram                        (*dimmNvList)[MAX_DIMM];
  struct ddrRank                          (*rankList)[MAX_RANK_DIMM];
  struct channelNvram                     (*channelNvList)[MAX_CH];
  PatCadbProg0                            patCADBProg0[3];
  PatCadbProg1                            patCADBProg1[3];
  CPGC_PATCADBMRS_MCDDC_CTL_STRUCT        patCADBMrs;
  CPGC_PDA_MCDDC_DP_STRUCT                cpgcPDA;
  CPGC_PDA_SHADOW_MCDDC_CTL_STRUCT        cpgcPDAShadow;
  CPGC_SEQCTL0_MC_MAIN_STRUCT             seqCtl0;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT        cpgcPatCadbCtlReg;
  UINT16                                  tempData;
  UINT8                                   index;
  UINT8                                   maxIndex;
  MCMTR_MC_MAIN_STRUCT                    mcMtr;
  MCMTR_MC_MAIN_STRUCT                    mcMtrSafe;
  CHIP_WL_CLEANUP_STRUCT  WLChipCleanUpStruct, WLChipCleanUpStructOrg;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT          dataControl0;
  INT16                   CRAddDelay[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList  = GetDimmNvList(host, socket, ch);
  rankList    = GetRankNvList(host, socket, ch, dimm);


  dataControl0.Data = (*channelNvList)[ch].dataControl0;
  dataControl0.Bits.txon = 1;

  // override PRK to WR
  WriteLrbuf(host, socket, ch, dimm, 0, (*dimmNvList)[dimm].lrBuf_BC01, LRDIMM_F0, LRDIMM_BC02);

  GetChipTimingData(host, socket, ch, &WLChipCleanUpStruct);
  GetChipTimingData(host, socket, ch, &WLChipCleanUpStructOrg);
  MemSetLocal((UINT8 *)CRAddDelay, 0, sizeof(CRAddDelay));
  if(host->nvram.mem.hostFlybyDone == 0) {
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
    } // strobe loop

  }
  CRAddDelay[ch] = GetMinDwl(host, socket, ch, dimm, &WLChipCleanUpStructOrg);

  WriteChipWrADDDelays(host, socket, ch, 0, CRAddDelay, &WLChipCleanUpStruct);

  // HSD 5330727 - PBA not driving DQ low when scheduler is set to DDRT target
  mcId = GetMCID(host, socket, ch);
  mcMtrSafe.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
  mcMtr.Data = mcMtrSafe.Data;
  mcMtr.Bits.trng_target = 0;
  MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);

  //
  // Pointer will be dynamically incremented after a write to CADB_PROG register
  //
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, 0);

  //
  // Save before MR command
  //
  CRValueSave = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Data                      = 0;
  seqCtl0.Bits.initialization_mode  = MRS_Mode;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
  cpgcPatCadbCtlReg.Bits.enable_pda_doublelength = 1;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  cpgcPDA.Data = 0;
  cpgcPDAShadow.Data = 0;

  cpgcPDA.Bits.device_select = 0;
  //
  // Select the rank to be written to
  //
  cpgcPDA.Bits.rank_select        = (*rankList)[rank].logicalRank;
  cpgcPDAShadow.Bits.rank_select  = (*rankList)[rank].logicalRank;

  MemWritePciCfgEp (host, socket, ch, CPGC_PDA_MCDDC_DP_REG, cpgcPDA.Data);
  MemWritePciCfgEp (host, socket, ch, CPGC_PDA_SHADOW_MCDDC_CTL_REG, cpgcPDAShadow.Data);

  // Temporarily disable 3DS mode for MRS commands
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 0);
  }
  //
  // Program CADB row 0, BCW change function (func 0)
  //
  ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | (LRDIMM_BC7x << 4)), SIDE_A, NO_PDA, MRS_CMD, &patCADBProg0[0], &patCADBProg1[0]);
  //
  // Program CADB row 1 to enter PBA mode
  //
  ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | (LRDIMM_BC1x << 4) | ((*dimmNvList)[dimm].lrBuf_BC1x & LRDIMM_ONE_RANK_TIMING_MODE) | LRDIMM_PBA_MODE), SIDE_A, ENTER_PDA, MRS_CMD, &patCADBProg0[1], &patCADBProg1[1]);
  //
  // Program CADB row 2, BCW change function (input func)
  //
  ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | (LRDIMM_BC7x << 4) | controlWordFunc), SIDE_A, ENTER_PDA, MRS_CMD, &patCADBProg0[2], &patCADBProg1[2]);

  ProgramCADB (host, socket, ch, 0, 3, patCADBProg0, patCADBProg1);

  //
  // Execute MRS Mode
  //
  patCADBMrs.Data                     = 0;
  patCADBMrs.Bits.mrs_gap             = 5;
  patCADBMrs.Bits.mrs_start_pointer   = 0;
  patCADBMrs.Bits.mrs_end_pointer     = 2;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

  // Start test and clear errors
  seqCtl0.Bits.local_clear_errs  = 1;
  seqCtl0.Bits.local_start_test  = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  //
  // Poll test completion
  //
  CpgcPollGlobalTestDone (host, socket, 1 << ch);

  //
  // Execute MRS Mode (2 rows)
  //
  patCADBMrs.Data                     = 0;
  patCADBMrs.Bits.mrs_gap             = 5;
  patCADBMrs.Bits.mrs_start_pointer   = 0;
  patCADBMrs.Bits.mrs_end_pointer     = 1;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

  //PBA Mode enabled, now loop through all 9 buffers
  if (bufferAddr == ALL_DATABUFFERS) {
    index = 0;
    maxIndex = 9;
  } else if (bufferAddr > 9){
#ifdef DEBUG
      rcPrintf((host, "\nERROR: WritePBA () - IVALID PARAMETER\n"));
#endif //DEBUG
    return;
  } else {
    index = bufferAddr;
    maxIndex = bufferAddr +1;
  }

  for (; index < maxIndex; index++) {
    //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, index, NO_BIT,
    //             "#Debug DDR4 PBA write: Func 0x%02X, Addr 0x%02X,  Data 0x%02X\n", controlWordFunc, controlWordAddr, controlWordDataPtr[index]));

    if ((bufferAddr != ALL_DATABUFFERS) && (bufferAddr != index)) continue;
    cpgcPDA.Bits.device_select      = ~(3 << (index*2)) & ((1 << 18) - 1);
    //cpgcPDA.Bits.rank_select        = (*rankList)[rank].logicalRank;
    MemWritePciCfgEp (host, socket, ch, CPGC_PDA_MCDDC_DP_REG, cpgcPDA.Data);

    if ((controlWordAddr >= 0x10) || (controlWordFunc != 0)) {
      tempData = ((controlWordAddr & 0xF0) << 4) | controlWordDataPtr[index];
    } else {
      tempData = (controlWordAddr << 4) | controlWordDataPtr[index];
    }
    //
    // Program CADB row 0, BCW change function to get pdatag working properly
    //
    ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | (LRDIMM_BC7x << 4) | controlWordFunc), SIDE_A, ENTER_PDA, MRS_CMD, &patCADBProg0[0], &patCADBProg1[0]);
    //
    // Program CADB row 1 to write desired PBA command
    //
    ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | tempData), SIDE_A, ENTER_PDA, MRS_CMD, &patCADBProg0[1], &patCADBProg1[1]);

    ProgramCADB (host, socket, ch, 0, 2, patCADBProg0, patCADBProg1); // for 2 rows

    // Start test and clear errors
    seqCtl0.Bits.local_clear_errs  = 1;
    seqCtl0.Bits.local_start_test  = 1;
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

    //
    // Poll test completion
    //
    CpgcPollGlobalTestDone (host, socket, 1 << ch);
#ifdef PRINT_FUNC
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                   "DDR4 PBA write: buffer %d, bank 0x%02X, data 0x%04X\n", index, BANK7, (BIT12 | tempData)));
#endif
  } //buffer index

  //
  // PBA exit
  //
  cpgcPDA.Bits.device_select = 0;
  //cpgcPDA.Bits.rank_select        = (*rankList)[rank].logicalRank;
  MemWritePciCfgEp (host, socket, ch, CPGC_PDA_MCDDC_DP_REG, cpgcPDA.Data);
  //
  // Program CADB row 0, BCW change function (func 0)
  //
  ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | (LRDIMM_BC7x << 4)), SIDE_A, ENTER_PDA, MRS_CMD, &patCADBProg0[0], &patCADBProg1[0]);
  //
  // Program CADB row 1, BCW change function (func 0)
  //
  ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | (LRDIMM_BC7x << 4)), SIDE_A, ENTER_PDA, MRS_CMD, &patCADBProg0[1], &patCADBProg1[1]);
  //
  // Program CADB row 2 to exit PBA mode
  //
  ProgCADBRow (host, socket, ch, dimm, rank, BANK7, (BIT12 | (LRDIMM_BC1x << 4) | ((*dimmNvList)[dimm].lrBuf_BC1x & LRDIMM_ONE_RANK_TIMING_MODE)), SIDE_A, ENTER_PDA, MRS_CMD, &patCADBProg0[2], &patCADBProg1[2]);

  ProgramCADB (host, socket, ch, 0, 3, patCADBProg0, patCADBProg1);

  //
  // Execute MRS Mode
  //
  patCADBMrs.Data                     = 0;
  patCADBMrs.Bits.mrs_gap             = 5;
  patCADBMrs.Bits.mrs_start_pointer   = 0;
  patCADBMrs.Bits.mrs_end_pointer     = 2;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMRS_MCDDC_CTL_REG, patCADBMrs.Data);

  // Start test and clear errors
  seqCtl0.Bits.local_clear_errs  = 1;
  seqCtl0.Bits.local_start_test  = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);

  //
  // Poll test completion
  //
  CpgcPollGlobalTestDone (host, socket, 1 << ch);

  //
  // Restore after MR command
  //
  seqCtl0.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  seqCtl0.Bits.local_start_test = 0;
  seqCtl0.Bits.local_clear_errs = 1;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, CRValueSave);

  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_pda_doublelength = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  MemWritePciCfgEp(host, socket, ch, TCOTHP_MCDDC_CTL_REG, WLChipCleanUpStructOrg.tCOTHP.Data);
  MemWritePciCfgEp(host, socket, ch, TCOTHP2_MCDDC_CTL_REG, WLChipCleanUpStructOrg.tCOTHP2.Data);
  MemWritePciCfgEp(host, socket, ch, T_DDRT_DIMM0_BASIC_TIMING_MC_2LM_REG, WLChipCleanUpStructOrg.ddrtDimm0BasicTiming.Data);

  // HSD 5330727 - PBA not driving DQ low when scheduler is set to DDRT target
  // Restore original value
  MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtrSafe.Data);

  // Re-enable 3DS mode
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 1);
  }

  //Undo PRK override
  WriteLrbuf(host, socket, ch, dimm, 0, (*dimmNvList)[dimm].lrBuf_BC02, LRDIMM_F0, LRDIMM_BC02);

  // restore datacontrol0
  if(host->nvram.mem.hostFlybyDone == 0) {
    dataControl0.Data = (*channelNvList)[ch].dataControl0;
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // Skip if this is an ECC strobe when ECC is disabled
        //
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, 0, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
    } // strobe loop
  }
} // WritePBA
#endif //LRDIMM_SUPPORT

/**

  this function write the MR2 register for all the ranks

  @param host            - sysHost data structure
  @param socket          - Processor socket
  @param ch              - Channel to issue the ZQ to
  @param zqType          - Type of ZQ Calibration: Long or Short

@retval UINT32 - SUCCESS if the reset is succeded

**/
UINT32
CpgcIssueZQ (
            PSYSHOST  host,
            UINT8     socket,
            UINT8     ch,
            UINT8     zqType
            )
{
  UINT8               dimm;
  UINT8               rank;
  UINT8               delay = 1;
  UINT32              status = SUCCESS;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  TCRAP_MCDDC_CTL_STRUCT tCrap;

  channelNvList = GetChannelNvList(host, socket);

  if ((*channelNvList)[ch].enabled == 0) {
    return status;
  }

  tCrap.Data = MemReadPciCfgEp (host, socket, ch, TCRAP_MCDDC_CTL_REG);

  if (zqType == ZQ_SHORT) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Issue ZQCS\n"));
  } else {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Issue ZQCL\n"));
  }

  dimmNvList = GetDimmNvList(host, socket, ch);

  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if (CheckS3Jedec(host, socket, ch, dimm) == 0) continue;

    for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      if ((*dimmNvList)[dimm].aepDimmPresent) {
        //NVMDIMM_IF protocol does not support ZQCL or ZQCS in 3N mode
        //NVMCTLR will do internal calibration as part of power-up sequence
        if (tCrap.Bits.cmd_stretch != 3) {
          status = WriteCADBCmd(host, socket, ch, dimm, rank, ZQ_CMD, 0, (BIT7 | zqType), delay);
        } else {
          //Issue DB ZQCL by writing BC06 = 1
          WriteBC(host, socket, ch, dimm, 0, 1, LRDIMM_F0, LRDIMM_BC06);
        }
      } else {
        status = WriteCADBCmd(host, socket, ch, dimm, rank, ZQ_CMD, 0, (zqType << 10), delay);
      }
    } // rank loop
  } // dimm loop

  if (zqType == ZQ_LONG) {
    FixedDelay (host, 1);
  }

  return status;
} // CpgcIssueZQ

/**

  DDR advanced training CPGC setup

  @param host        - Pointer to sysHost
  @param socket      - Socket to train
  @param ddrChEnMap  - 4-bit bit mask of ddr channels to test - (1 = channel is selected to run tests)
  @param rank        - logical rank number (0-7)
  @param group       - GSM_GT value to store
  @param pattern     - ddr pattern (0-3)
  @param dqLoopCount - dq sequence loop count - number of times dq sequence is executed (2^(dqLoopCount - 1))

  @retval SUCCESS / FAILURE

**/
UINT32
CpgcAdvTrainingSetup (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT32    ddrChEnMap,
                     UINT8     rank,
                     UINT8     subRank,
                     GSM_GT    group,
                     UINT32    pattern,
                     UINT8     dqLoopCount
                     )
{

  UINT8               ch;
  UINT32              status;
  struct channelNvram (*channelNvList)[MAX_CH];

  MemDebugPrint((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, rank, NO_STROBE, NO_BIT,
                 "CpgcAdvTrainingSetup: ChEnMap = 0x%x, pattern = %d\n", ddrChEnMap, pattern));

  status = SUCCESS;
  channelNvList = GetChannelNvList(host, socket);

  //
  // CPGC setup
  //

  for (ch = 0; ch < MAX_CH; ch++) {

    // skip ddr channels not selected
    if (!(ddrChEnMap & (1 << ch))) continue;

    // skip disabled ddr channels
    if ((*channelNvList)[ch].enabled == 0) continue;

    switch (pattern & DDR_CPGC_PATTERN_MASK) {
    case DDR_CPGC_DQ_PATTERN:
      CpgcAdvDqSetup (host, socket, ch, rank, subRank, dqLoopCount);
      break;

    case DDR_CPGC_ADV_CMD_PATTERN:
      CpgcAdvCmdPatternSetup (host, socket, ch, rank, dqLoopCount);
      break;

      /*
      case DDR_CPGC_CMD_G2_PATTERN: // cmd group 2
        CpgcAdvCmdSetup2 (host, socket, ch, rank);
        break;

      case DDR_CPGC_CMD_G3MPR_PATTERN: // cmd group 3
        CpgcAdvCmdSetup3 (host, socket, ch, rank);
        break;

      case DDR_CPGC_MEM_TEST_PATTERN: // mem test pattern
        CpgcMemTestGlobalSetup (host, socket, ch, rank, dqLoopCount, pattern);
        break;
      */
    default:
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_47);
    }
  }

  return status;

} // CpgcAdvTrainingSetup

/**

  DDR DQ victim aggressor CPGC setup.

  @param host        - Pointer to sysHost
  @param socket      - Socket to train
  @param ch          - ddr channel to setup
  @param logRank     - logical rank number
  @param dqLoopCount - dq sequence loop count - number of times dq sequence is executed (2^(dqLoopCount - 1))
  @param victimBit   - dq victimBit

  @retval SUCCESS / FAILURE

**/
static UINT32
CpgcAdvDqSetup (
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch,
               UINT8     logRank,
               UINT8     subRank,
               UINT8     dqLoopCount
               )
{

  UINT32                                        status;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_GLOBAL_TRAINING_SETUP                    cpgcGblTrainingSetup;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, logRank, NO_STROBE, NO_BIT,
                 "CpgcAdvDqSetup\n"));

  status = SUCCESS;

  cpgcGblTrainingSetup.seqLoopCnt        = dqLoopCount;                      // test will end after (2^(n-1)) loops through the sub-sequence
  cpgcGblTrainingSetup.startDelay        = 0;                                // Delay start
  cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;                // Update based on the # of read and write cacheline
  cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0
  cpgcGblTrainingSetup.numCacheLines     = 5;                                // Number of Cachelines (2^N)
  if (IsAepRank(host, socket, ch, logRank)) {
    cpgcGblTrainingSetup.burstSeparation  = CPGC_SUBSEQ_WAIT_DDRT;            // (sub_seqwait) - need to add delay to drain credits if doing write / read subseq
  } else {
    cpgcGblTrainingSetup.burstSeparation  = CPGC_SUBSEQ_WAIT_DEFAULT;            // (sub_seqwait) - need to add delay to drain credits if doing write / read subseq
  }
  cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_ALL_CHUNK;       // monitor errors on any chunk
  cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_ALL_CL;          // monitor errors on any cacheline
  cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_STOP_ON_ALL_ERR;         // Stop on all lane error
  cpgcGblTrainingSetup.subseqEndPtr      = 1;                                // subseq end pointer
  cpgcGblTrainingSetup.useSubSeq1        = CPGC_BASE_READ_SUBSEQ;            // use sub seq1 (base read)
  cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
  cpgcGblTrainingSetup.back2backWrEn     = 1;                                // enable b2b writes
  cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
  cpgcGblTrainingSetup.refMask           = 0xFF;                             // disable cpgc self refresh
  cpgcGblTrainingSetup.zqMask            = 0xFF;                             // disable cpgc zq
  //if(x8) {
  cpgcGblTrainingSetup.x8Mode           = 0;
  //} else {
  // cpgcGblTrainingSetup.x8Mode           = 0;
  //}

  CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

  //
  // WDB programming
  //
  seqBaseAddrStartLo0.Data            = 0;
  seqBaseAddrStartHi0.Data            = 0;
  seqBaseAddrStartHi0.Bits.rank_addr  = 0;  // Use logical to physical rank mapping

  seqBaseAddrWrapLo0.Data             = 0;
  seqBaseAddrWrapLo0.Bits.column_addr = 0x1FF;
  seqBaseAddrWrapLo0.Bits.bank_addr   = 0x7; // was 0xf on HSX?

  seqBaseAddrWrapHi0.Data           = 0;
  seqBaseAddrWrapHi0.Bits.row_addr  = 0xFFFF;
  seqBaseAddrWrapHi0.Bits.rank_addr = 0;  // Use logical to physical rank mapping

  // Program logical to physical rank mapping
  SelectCPGCRanks(host, socket, ch, 1 << logRank, 0, subRank);

  //
  // Sequence BAIN control
  //
  seqBaincCtl00.Data = 0;
  seqBaincCtl00.Bits.column_ba_inc      = 1;
  seqBaincCtl00.Bits.column_ba_updrate  = 0;
  seqBaincCtl00.Bits.column_ba_updscale = 1;
  seqBaincCtl00.Bits.row_ba_inc         = 0;
  seqBaincCtl00.Bits.row_ba_updrate     = 0;
  seqBaincCtl00.Bits.row_ba_updscale    = 0;

  seqBaincCtl10.Data = 0;
  seqBaincCtl10.Bits.bank_ba_inc        = 4;
  seqBaincCtl10.Bits.bank_ba_updrate    = 0;
  seqBaincCtl10.Bits.bank_ba_updscale   = 0;
  seqBaincCtl10.Bits.rank_ba_inc        = 0;
  seqBaincCtl10.Bits.rank_ba_updrate    = 0;
  seqBaincCtl10.Bits.rank_ba_updscale   = 0;
  seqBaincCtl10.Bits.column_ba_inc_msb  = 0;

  //
  // Set wrap order
  //
  //TODO: fix this
  cpgcSeqBaoCic0.Data = 0;

  CpgcSetupWdb (host, socket, ch,
                5,                           // wdbIncrRate
                0,                           // wdbIncrScale
                0,                           // wdbStartPntr (victim bit 0)
                7,                           // wdbEndPntr (victim bit 0)
                1,                           // setupPbwr
                CPGC_WDB_MUX_CTL_LFSR24,     // mux control
                seqBaincCtl00,
                seqBaincCtl10,
                seqBaseAddrStartLo0,
                seqBaseAddrStartHi0,
                seqBaseAddrWrapLo0,
                seqBaseAddrWrapHi0,
                cpgcSeqBaoCic0
               );

  //
  // Program WDB patterns (dq/va pattern)
  //
  CpgcFillWdbDqVa (host, socket, ch, 0);

  //
  // Disable CADB
  //
  cpgcPatCadbCtlReg.Data = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  return status;

} // CpgcAdvDqSetup

/**

  DDR Advanced CMD Pattern CPGC setup.

  @param host        - Pointer to sysHost
  @param socket      - Socket to train
  @param ch          - ddr channel to setup
  @param rank        - logical rank number

  @retval SUCCESS / FAILURE

**/
static UINT32
CpgcAdvCmdPatternSetup (
                       PSYSHOST  host,
                       UINT8     socket,
                       UINT8     ch,
                       UINT8     rank,
                       UINT8     dqLoopCount
                       )
{

  UINT32                                        status;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_GLOBAL_TRAINING_SETUP                    cpgcGblTrainingSetup;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT           patCadbMuxCtl;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;
  struct dimmNvram                              (*dimmNvList)[MAX_DIMM];
  UINT8                                         aepPresent;
  UINT8                                         dimm;

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, rank, NO_STROBE, NO_BIT,
                 "CpgcAdvCmdPatternSetup\n"));
  //
  // Make sure pages are closed
  //
  FixedDelay(host, 50);

  status = SUCCESS;

  cpgcGblTrainingSetup.seqLoopCnt        = dqLoopCount;                                // test will end after (2^(n-1)) loops through the sub-sequence
  cpgcGblTrainingSetup.startDelay        = 0;                                // Delay start
  cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;                // Update based on the # of read and write cacheline
  cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0
  cpgcGblTrainingSetup.numCacheLines     = 5;                                // Number of Cachelines (2^N)
  cpgcGblTrainingSetup.burstSeparation  = CPGC_SUBSEQ_WAIT_DDRT;            // (sub_seqwait) - need to add delay to drain credits if doing write / read subseq
  cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_ALL_CHUNK;       // monitor errors on any chunk
  cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_ALL_CL;          // monitor errors on any cacheline
  cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_STOP_ON_ALL_ERR;         // Stop on all lane error
  cpgcGblTrainingSetup.subseqEndPtr      = 1;                                // subseq end pointer
  cpgcGblTrainingSetup.useSubSeq1        = CPGC_BASE_READ_SUBSEQ;            // use sub seq1 (base read)
  cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
  cpgcGblTrainingSetup.back2backWrEn     = 0;                                // enable b2b writes
  cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
  cpgcGblTrainingSetup.refMask           = 0xFF;                             // disable cpgc self refresh
  cpgcGblTrainingSetup.zqMask            = 0xFF;                             // disable cpgc zq
  cpgcGblTrainingSetup.x8Mode           = 0;

  CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

  // Program logical to physical rank mapping
  SelectCPGCRanks(host, socket, ch, 1 << rank, 0, 0);
  //
  // WDB programming
  //
  seqBaseAddrStartLo0.Data            = 0;
  seqBaseAddrStartHi0.Data            = 0;
  seqBaseAddrStartHi0.Bits.rank_addr  = 0;

  seqBaseAddrWrapLo0.Data             = 0;
  seqBaseAddrWrapLo0.Bits.column_addr = 0x1F;
  if (IsAepRank(host, socket, ch, rank) && host->var.common.cpuType == CPU_SKX) {
    seqBaseAddrWrapLo0.Bits.bank_addr = 0xC;
  } else {
    seqBaseAddrWrapLo0.Bits.bank_addr = 0xF;
  }
  seqBaseAddrWrapHi0.Data           = 0;
  seqBaseAddrWrapHi0.Bits.row_addr  = 0x1FFF;
  seqBaseAddrWrapHi0.Bits.rank_addr = 0;

  //
  // Sequence Base Address Increment control
  //
  seqBaincCtl00.Data = 0;
  seqBaincCtl00.Bits.column_ba_inc      = 43;
  seqBaincCtl00.Bits.column_ba_updrate  = 0;
  seqBaincCtl00.Bits.column_ba_updscale = 1;
  if (IsAepRank(host, socket, ch, rank)) {
    seqBaincCtl00.Bits.row_ba_inc       = 0;
  } else {
    seqBaincCtl00.Bits.row_ba_inc       = 1553;
  }
  seqBaincCtl00.Bits.row_ba_updrate     = 0;
  seqBaincCtl00.Bits.row_ba_updscale    = 1;

  seqBaincCtl10.Data = 0;
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    seqBaincCtl10.Bits.bank_ba_inc        = 1;
  } else {
    if (IsAepRank(host, socket, ch, rank) && host->var.common.cpuType == CPU_SKX) {
      seqBaincCtl10.Bits.bank_ba_inc      = 4;
    } else {
      seqBaincCtl10.Bits.bank_ba_inc      = 3;
    }
  }
  seqBaincCtl10.Bits.bank_ba_updrate    = 0;
  seqBaincCtl10.Bits.bank_ba_updscale   = 1;
  seqBaincCtl10.Bits.rank_ba_inc        = 0;
  seqBaincCtl10.Bits.rank_ba_updrate    = 0;
  seqBaincCtl10.Bits.rank_ba_updscale   = 0;
  seqBaincCtl10.Bits.column_ba_inc_msb  = 0;

  //
  // Set wrap order
  //
  cpgcSeqBaoCic0.Data = 0;

  CpgcSetupWdb (host, socket, ch,
                5,                           // wdbIncrRate
                0,                           // wdbIncrScale - Exponential
                0,                           // wdbStartPntr (victim bit 0)
                7,                           // wdbEndPntr (victim bit 0)
                1,                           // setupPbwr
                CPGC_WDB_MUX_CTL_LFSR24_PATBUF,     // mux control
                seqBaincCtl00,
                seqBaincCtl10,
                seqBaseAddrStartLo0,
                seqBaseAddrStartHi0,
                seqBaseAddrWrapLo0,
                seqBaseAddrWrapHi0,
                cpgcSeqBaoCic0
               );

  //
  // Program WDB patterns (dq/va pattern)
  //
  CpgcFillWdbDqVa (host, socket, ch, 1);

  //
  // Program WDB patterns
  //
  if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
    ProgramCADB (host, socket, ch, 0, CADB_LINES, AdvCmdPattern2, AdvCmdPattern1);
  } else {
    ProgramCADB (host, socket, ch, 0, CADB_LINES, AdvCmdPattern0, AdvCmdPattern1);
  }

  aepPresent = 0;
  dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
  for (dimm = 0; dimm < host->nvram.mem.socket[socket].channelList[ch].maxDimm; dimm++) {
    if((*dimmNvList)[dimm].dimmPresent == 0) continue;
    if((*dimmNvList)[dimm].aepDimmPresent)  {
      aepPresent = 1 ;
      break;
    }
  }
  //
  // Enable CADB on deselect
  //
  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  // A0 stepping workaround, s4929399
  if (aepPresent) {
    cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
  } else {
    cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 1;
  }
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
  if ((host->nvram.mem.dimmTypePresent == UDIMM) || (host->nvram.mem.dimmTypePresent == SODIMM)) {
    cpgcPatCadbCtlReg.Bits.enable_cadb_parityntiming = 1;
  } else {
    cpgcPatCadbCtlReg.Bits.enable_cadb_parityntiming = 0;
  }
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  //
  // Program CADB pattern buffer
  //
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, 0x5555);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, 0x3333);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, 0x0F0F);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX3PB_MCDDC_CTL_REG, 0x00FF);

  //
  // Program CADB mux control
  //
  patCadbMuxCtl.Data = 0;
  //
  // Enable LFSR
  //
  patCadbMuxCtl.Bits.mux0_control = 2;
  patCadbMuxCtl.Bits.mux1_control = 2;
  patCadbMuxCtl.Bits.mux2_control = 2;
  patCadbMuxCtl.Bits.mux3_control = 2;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, patCadbMuxCtl.Data);
  SetupPmon(host, socket, ch, 0xE0, 1);
  return status;

} // CpgcAdvCmdPatternSetup

/**

  Enable deselect patterns

  @param host        - Pointer to sysHost
  @param socket      - Socket to train
  @param ddrChEnMap  - DDR channels to setup
  @param mode        - MODE_DESELECT
                     - MODE_VA_DESELECT

  @retval SUCCESS / FAILURE

**/
void
EnableCADBDeselect (
                   PSYSHOST  host,
                   UINT8     socket,
                   UINT32    ddrChEnMap,
                   UINT16    mode
                   )
{
  UINT8                                   ch;
  struct channelNvram                     (*channelNvList)[MAX_CH];
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT        cpgcPatCadbCtlReg;
  CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT     patCadbMuxCtl;
  struct dimmNvram                        (*dimmNvList)[MAX_DIMM];
  UINT8                                   aepPresent;
  UINT8                                   dimm;

  channelNvList = GetChannelNvList(host, socket);


  for (ch = 0; ch < MAX_CH; ch++) {

    // skip ddr channels not selected
    if (!(ddrChEnMap & (1 << ch))) continue;

    // skip disabled ddr channels
    if ((*channelNvList)[ch].enabled == 0) continue;

    //
    // Program WDB patterns
    //
    if (mode & MODE_DESELECT) {
      if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
        ProgramCADB (host, socket, ch, 0, CADB_LINES, AdvCmdPattern2, AdvCmdPattern1);
      } else {
        ProgramCADB (host, socket, ch, 0, CADB_LINES, AdvCmdPattern0, AdvCmdPattern1);
      }
    } else {
      if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
        ProgramCADB (host, socket, ch, 0, CADB_LINES, AggAdvCmdPattern2, AggAdvCmdPattern1);
      } else {
        ProgramCADB (host, socket, ch, 0, CADB_LINES, AggAdvCmdPattern0, AggAdvCmdPattern1);
      }
    }

    aepPresent = 0;
    dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;
    for (dimm = 0; dimm < host->nvram.mem.socket[socket].channelList[ch].maxDimm; dimm++) {
      if((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if((*dimmNvList)[dimm].aepDimmPresent)  {
        aepPresent = 1 ;
        break;
      }
    }
    //
    // Enable CADB on deselect
    //
    cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
    // A0 stepping workaround, s4929399
    if (aepPresent) {
      cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
    } else {
      cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 1;
    }
    cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
    cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
    if ((host->nvram.mem.dimmTypePresent == UDIMM) || (host->nvram.mem.dimmTypePresent == SODIMM)) {
      cpgcPatCadbCtlReg.Bits.enable_cadb_parityntiming = 1;
    } else {
      cpgcPatCadbCtlReg.Bits.enable_cadb_parityntiming = 0;
    }
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

    //
    // Program CADB pattern buffer
    //
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, 0x5555);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, 0x3333);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, 0x0F0F);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX3PB_MCDDC_CTL_REG, 0x00FF);

    //
    // Program CADB mux control
    //
    patCadbMuxCtl.Data = 0;
    //
    // Enable LFSR
    //
    patCadbMuxCtl.Bits.mux0_control = 2;
    patCadbMuxCtl.Bits.mux1_control = 2;
    patCadbMuxCtl.Bits.mux2_control = 2;
    patCadbMuxCtl.Bits.mux3_control = 2;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, patCadbMuxCtl.Data);
  } // ch loop

} // EnableCADBDeselect

/**

  DDR CPGC fill wdb for dq/va pattern

  @param host    - Pointer to sysHost
  @param socket  - Socket to train
  @param ch      - ddr channel to setup

  @retval SUCCESS / FAILURE

**/
static UINT32
CpgcFillWdbDqVa (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT8     mode
                )
{

  UINT8     i;
  UINT8     wdbIndex;
  UINT32    status;
  TWdbLine  WdbLines[32];

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcFillWdbDqVa\n"));

  status = SUCCESS;

  for (wdbIndex = 0; wdbIndex < 8; wdbIndex++) {
    for (i = 0; i < MRC_WDB_LINE_SIZE; i++) {
      if ((mode == 1) && (host->nvram.mem.dramType == SPD_TYPE_DDR4)) {
        WdbLines[wdbIndex].WdbLine[i] = CpgcWdbDqVaPat2[wdbIndex][i / 8];
      } else {
        WdbLines[wdbIndex].WdbLine[i] = CpgcWdbDqVaPat[wdbIndex][i / 8];
      }
    } // i loop
  } // wdbIndex loop

  WDBFill (host, socket, ch, WdbLines, 8, 0);

  return status;

} // CpgcFillWdbDqVa

#ifndef ADV_MEMTEST_SUPPORT

/**

  CPGC based Memory test

  @param host                - Pointer to sysHost
  @param socket              - Memory controller number
  @param ddrChEnMap          - channel mask
  @param flags               - Read or Write and data pattern flags

  @retval n/a
**/
void
CpgcMemTest (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    ddrChEnMap,
  UINT32    flags
  )

{
  UINT32  rankChEnMap;
  UINT32  rankChEnOut;
  UINT8   logicalRank;
  UINT8   logicalSubRank;
  UINT8   ch;
  UINT8   columnBits[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   rowBits[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   bankBits[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   rankEnabled[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   dimmRank[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   rankChEnabled[MAX_RANK_CH] = {0};
  UINT8   logical2Physical[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   maxEnabledRank;
  UINT8   maxEnabledSubRank;
  UINT8   loopCount;
  BOOLEAN secondPass;
  UINT8   dimm;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  MemDebugPrint((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcMemTest: ddrChEnMap = 0x%x, flags = 0x%x\n", ddrChEnMap, flags));

  channelNvList = GetChannelNvList(host, socket);
  secondPass = FALSE;

  //
  // Phase 1: Global CPGC setup
  //

  // One loop (2^(n-1)) through the sequence (subseq numCacheLines set to test entire rank)
  loopCount = 1;
  maxEnabledSubRank = 0;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((ddrChEnMap & (1 << ch)) == 0) continue;

    CpgcMemTestGlobalSetup (host, socket, ch, 0, loopCount, DDR_CPGC_MEM_TEST_PATTERN | flags);
    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*channelNvList)[ch].encodedCSMode == 1) {
      maxEnabledSubRank = 1;
    } else if ((*channelNvList)[ch].encodedCSMode == 2) {
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if (maxEnabledSubRank < ((*dimmNvList)[dimm].dieCount - 1)) {
          maxEnabledSubRank = (*dimmNvList)[dimm].dieCount - 1;
        }
      } //dimm loop
    } //else
  } // ch loop

  //
  // Phase 2: Per logical rank setup and test
  //

  //
  // Find geometery for logical rank within channel.
  //
  MemRankGeometry (host, socket, columnBits, rowBits, bankBits, rankEnabled, dimmRank, rankChEnabled, &maxEnabledRank, logical2Physical);

  for (logicalSubRank = 0; logicalSubRank <= maxEnabledSubRank; logicalSubRank++) {
    for (logicalRank = 0; logicalRank <= maxEnabledRank; logicalRank++) {

      rankChEnMap = ddrChEnMap;

      for (ch = 0; ch < MAX_CH; ch++) {
        rankChEnMap &= ~((rankEnabled[logicalRank][ch] == 0) << ch);
      }

#ifdef LRDIMM_SUPPORT
      for (ch = 0; ch < MAX_CH; ch++) {
          if ((IsLrdimmPresent(host, socket, ch, dimmRank[logicalRank][ch]) && ((*channelNvList)[ch].encodedCSMode == 0) && logicalSubRank > 0)) {
              rankChEnMap &= ~(1 << ch);
          }
      } //ch
#endif
      //
      // Skip to next rank if no ranks enabled on any channel
      //
      if (rankChEnMap == 0) continue;

      CpgcMemTestRankSetup (host, socket, rankChEnMap, logicalRank, logicalSubRank, columnBits[logicalRank], rowBits[logicalRank], bankBits[logicalRank]);

      CpgcAdvTrainingExecTest (host, socket, rankChEnMap, DDR_CPGC_MEM_TEST_PATTERN);

      //
      // For READ test must check for failures
      //
      if ((flags & DDR_CPGC_MEM_TEST_READ) == DDR_CPGC_MEM_TEST_READ) {

        if (CheckTestResults (host, socket, rankChEnMap, &rankChEnOut, dimmRank, logical2Physical, logicalRank, ALL_BANKS, secondPass) == FAILURE) {
          secondPass = TRUE;
          logicalRank--;   // This reruns the rank that failed memtest
        } else {
          secondPass = FALSE;
          rankChEnMap = rankChEnOut;
          for (ch = 0; ch < MAX_CH; ch++) {
            rankEnabled[logicalRank][ch] = ((rankChEnOut >> ch) & 1);
          } //ch
        }
      } // if READ
    } // for logicalRank
  } // for logicalSubRank
  return;
} // CpgcMemTest

#else //not ADV_MEMTEST_SUPPORT

/**

  Executes a step of Advanced Memory test using CPGC engine and logs results

  @param host                - Pointer to sysHost
  @param socket              - Socket Id
  @param ddrChEnabled        - List of channels to be tested
  @param testType            - Memory test type
  @param mode                - Type of sequence MT_CPGC_WRITE, MT_CPGC_READ, or MT_CPGC_READ_WRITE
  @param seqDataInv[MT_MAX_SUBSEQ]    - Enables pattern inversion per subsequence
  @param patternQW[MT_PATTERN_DEPTH]  - 64-bit wide data pattern to use per UI
  @param uiShl[MT_PATTERN_DEPTH]      - Enables pattern rotation per UI
  @param numCacheLines                - Number of cachelines to use in WDB
  @param patternDepth                 - Number of UI to use in patternQW
  @param direction                    - Sequential address direction MT_ADDR_DIR_UP, MT_ADDR_DIR_DN

  @retval status - SUCCESS/FAILURE

**/
UINT32
CpgcMemTestMATS (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    ddrChEnMap,
  UINT8     testType,
  UINT8     mode,
  UINT8     seqDataInv[MT_MAX_SUBSEQ],
  UINT64_STRUCT patternQW[MT_PATTERN_DEPTH],
  UINT8     uiShl[MT_PATTERN_DEPTH],
  UINT8     numCacheLines,
  UINT8     patternDepth,
  UINT8     direction
  )
{
  UINT32  rankChEnMap;
  UINT32  rankChEnOut;
  UINT8   logicalRank;
  UINT8   logicalSubRank;
  UINT8   ch;
  UINT8   columnBits[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   rowBits[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   bankBits[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   rankEnabled[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   dimmRank[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   rankChEnabled[MAX_RANK_CH] = {0};
  UINT8   logical2Physical[MAX_RANK_CH][MAX_CH] = {{0}};
  UINT8   maxEnabledRank;
  UINT8   maxEnabledSubRank;
  UINT8   loopCount;
  UINT8   bank, baseBits;
  BOOLEAN secondPass;
  UINT8   dimm;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  UINT32 status = SUCCESS;

  MemDebugPrint((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcMemTestMATS: ddrChEnMap = 0x%x, mode = 0x%x\n", ddrChEnMap, mode));

  channelNvList = GetChannelNvList(host, socket);
  secondPass = FALSE;

  //
  // Phase 1: Global CPGC setup
  //
  loopCount = 1;
  maxEnabledSubRank = 0;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((ddrChEnMap & (1 << ch)) == 0) continue;

    CpgcMemTestGlobalSetupMATS (host, socket, ch, 0, loopCount, testType, mode, seqDataInv, patternQW, uiShl, numCacheLines, patternDepth, direction);

    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*channelNvList)[ch].encodedCSMode == 1) {
      maxEnabledSubRank = 1;
    } else if ((*channelNvList)[ch].encodedCSMode == 2) {
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if (maxEnabledSubRank < ((*dimmNvList)[dimm].dieCount - 1)) {
          maxEnabledSubRank = (*dimmNvList)[dimm].dieCount - 1;
        }
      } //dimm loop
    } //else
  } // ch loop

  //
  // Phase 2: Per logical rank setup and test
  //

  //
  // Find geometery for logical rank within channel.
  //
  MemRankGeometry (host, socket, columnBits, rowBits, bankBits, rankEnabled, dimmRank, rankChEnabled, &maxEnabledRank, logical2Physical);

  for (logicalSubRank = 0; logicalSubRank <= maxEnabledSubRank; logicalSubRank++) {
    for (logicalRank = 0; logicalRank <= maxEnabledRank; logicalRank++) {

      rankChEnMap = ddrChEnMap;
      for (ch = 0; ch < MAX_CH; ch++) {
        rankChEnMap &= ~((rankEnabled[logicalRank][ch] == 0) << ch);
#ifdef LRDIMM_SUPPORT
        if ((IsLrdimmPresent(host, socket, ch, 0) && ((*channelNvList)[ch].encodedCSMode == 0) && logicalSubRank > 0)) {
          rankChEnMap &= ~(1 << ch);
        }
#endif
      } //ch

      //
      // Skip to next rank if no ranks enabled on any channel
      //
      if (rankChEnMap == 0) continue;

      // Number of bank bits for HW sequence to use
      if (host->nvram.mem.dramType == SPD_TYPE_DDR3) {
        // Test DDR3 per bank address
        // This improves fault-tolerance capability for testType ADV_MT_TYPE_LTEST_SCRAM
        // that accumulates DQ errors to make decision on correctable vs. UC error
        baseBits = 3;
      } else {
        // Test DDR4 per pair of bank address with BG[1] interleave
        // This provides a good balance between fault-tolerance and performance with tCCD=4
        // Per-bank testing (i.e. baseBits=4) requires tCCD_L and a significant latency increase
        baseBits = 3;
      }
      // For each Bank address
      for (bank = 0; (bank < (1 << baseBits)) && rankChEnMap; bank++) {

        CpgcMemTestRankSetupMATS (host, socket, rankChEnMap, logicalRank, logicalSubRank,
                                  columnBits[logicalRank], rowBits[logicalRank], bankBits[logicalRank],
                                  mode, direction, bank, baseBits, seqDataInv);

        CpgcAdvTrainingExecTest (host, socket, rankChEnMap, DDR_CPGC_MEM_TEST_PATTERN);

        //
        // For READ test must check for failures
        //
        if (mode == MT_CPGC_READ_WRITE || mode == MT_CPGC_READ) {

          if (testType == ADV_MT_TYPE_LTEST_SCRAM) {

            if (CheckTestResults (host, socket, rankChEnMap, &rankChEnOut, dimmRank, logical2Physical, logicalRank, logicalSubRank, bank, secondPass) == FAILURE) {
              secondPass = TRUE;
              bank--;   // This reruns the bank that failed memtest to filter transient error
            } else {
              secondPass = FALSE;
              rankChEnMap = rankChEnOut;
              for (ch = 0; ch < MAX_CH; ch++) {
                rankEnabled[logicalRank][ch] = ((rankChEnOut >> ch) & 1);
              } //ch
            }

          } else {

            if (CheckTestResultsMATS (host, socket, rankChEnMap, dimmRank, logical2Physical, logicalRank, testType) == FAILURE) {
              OutputCheckpoint(host, STS_MEMBIST, SUB_MT_STEP9, 0);
              status = FAILURE;
            }
          }

        } // if READ
      } //for bank
    } // for logicalRank
  } // for logicalSubRank

  return status;
} // CpgcMemTestMATS


/**

  CPGC setup per rank for Advanced Memory test

  @param host              - Pointer to sysHost
  @param socket            - Memory controller to train
  @param ddrChEnMap        - ddr channels to setup
  @param logRank           - logical rank to setup
  @param colAddressBits    - number of DRAM column address bits to test
  @param rowAddressBits    - number of DRAM row address bits to test
  @param bankAddressBits   - number of DRAM bank address bits to test
  @param mode              - Type of sequence MT_CPGC_WRITE, MT_CPGC_READ, or MT_CPGC_READ_WRITE
  @param direction         - Sequential address direction MT_ADDR_DIR_UP, MT_ADDR_DIR_DN
  @param bank              - Base bank address number to test
  @param baseBits          - Number of bank address bits to skip on increment
  @param seqDataInv[MT_MAX_SUBSEQ] - Enables pattern inversion per subsequence

  @retval None

**/
static void
CpgcMemTestRankSetupMATS (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT32    ddrChEnMap,
                     UINT8     logRank,
                     UINT8     logSubRank,
                     UINT8     colAddressBits[MAX_CH],
                     UINT8     rowAddressBits[MAX_CH],
                     UINT8     bankAddressBits[MAX_CH],
                     UINT8     mode,
                     UINT8     direction,
                     UINT8     bank,
                     UINT8     baseBits,
                     UINT8     seqDataInv[MT_MAX_SUBSEQ]
                     )
{
  UINT8                                         ch;
  UINT8                                         cpgcColAddressBits;
  UINT8                                         cpgcRowAddressBits;
  UINT8                                         cpgcBankAddressBits;
  UINT8                                         numCacheLines;
  UINT8                                         mcId;
  INT32  matsBurstBits;
  struct channelNvram                           (*channelNvList)[MAX_CH];
  CPGC_SUBSEQ0_CTL0_MC_MAIN_STRUCT            cpgcSubseq0Ctl0Reg;
  CPGC_SUBSEQ1_CTL0_MC_MAIN_STRUCT            cpgcSubseq1Ctl0Reg;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT   seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT   seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT    seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT    seqBaseAddrWrapHi0;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT          seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT          seqBaincCtl10;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT             cpgcSeqBaoCic0;
  CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_STRUCT        seqDmyRdAddr;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT              cpgcGlobalCtrlReg;
  CPGC_SEQCTL0_MC_MAIN_STRUCT                 cpgcSeqCtl0Reg;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT             ddrtMiscDelay;
  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, logRank, NO_STROBE, NO_BIT,
                 "CpgcMemTestRankSetup: ddrChEnMap = 0x%x\n", ddrChEnMap));

  //
  // CPGC setup
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    mcId = GetMCID(host, socket, ch);
    //
    // skip ddr channels not selected or disabled ddr channels
    // Must disable global start so start test does not activate cpgc on disabled channel for this rank
    //
    if (!(ddrChEnMap & (1 << ch)) ||
        (host->nvram.mem.socket[socket].channelList[ch].enabled == 0) ||
        (((*channelNvList)[ch].encodedCSMode == 0) && (logSubRank != 0))) {
        CpgcDisableGlobalStart (host, socket, ch);
        continue;
    }

    MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, logRank, NO_STROBE, NO_BIT,
                   "CpgcMemTestRankSetup: colAddressBits = %d, rowAddressBits = %d, bankAddressBits = %d\n",
                   colAddressBits[ch], rowAddressBits[ch], bankAddressBits[ch]));

    //
    // Cpgc handles lower 3 bits of column
    //
    //
    // discover burst length setting
    //
    cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
    //
    // if Burst Length 4 do - 2 column bits, else -3
    //
    if (cpgcGlobalCtrlReg.Bits.use_bl4_col_addr == 1) {
      cpgcColAddressBits = colAddressBits[ch] - 2;
    } else {
      cpgcColAddressBits = colAddressBits[ch] - 3;
    }
    cpgcRowAddressBits = rowAddressBits[ch];
    cpgcBankAddressBits = bankAddressBits[ch];

    // Enable cacheline read/write bursts
    //matsBurstBits = MT_BURST_BITS;

    matsBurstBits = cpgcColAddressBits + cpgcBankAddressBits - baseBits;

    //
    // Adjust number of cachelines for size of rank (use 2^n) scale
    //
    //numCacheLines = cpgcColAddressBits + cpgcRowAddressBits + cpgcBankAddressBits - (UINT8)matsBurstBits;
    //numCacheLines = cpgcColAddressBits + cpgcRowAddressBits - (UINT8)matsBurstBits;
    numCacheLines = cpgcRowAddressBits;

    cpgcSeqCtl0Reg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(mcId, ch)));
    cpgcSeqCtl0Reg.Bits.loopcount = numCacheLines + 1;
    cpgcSeqCtl0Reg.Bits.address_update_rate_mode  = 0;
    MemWritePciCfgMC (host, socket, mcId, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(mcId, ch)), cpgcSeqCtl0Reg.Data);

    //Subsequence 0
    cpgcSubseq0Ctl0Reg.Data = MemReadPciCfgMC (host, socket, mcId, GetCpgcRegOffset (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG));
    cpgcSubseq0Ctl0Reg.Bits.num_cachelines = matsBurstBits;
    cpgcSubseq0Ctl0Reg.Bits.rstcurr_ba_start = 0;
    cpgcSubseq0Ctl0Reg.Bits.savecurr_ba_start = 1;
    cpgcSubseq0Ctl0Reg.Bits.num_cachelines_scale = 0;
    cpgcSubseq0Ctl0Reg.Bits.subseq_wait = 0;
    if (seqDataInv[0]) {
      cpgcSubseq0Ctl0Reg.Bits.inv_dataecc = 1;
    } else {
      cpgcSubseq0Ctl0Reg.Bits.inv_dataecc = 0;
    }
    MemWritePciCfgMC (host, socket, mcId, GetCpgcRegOffset (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG), cpgcSubseq0Ctl0Reg.Data);

    //Subsequence 1
    cpgcSubseq1Ctl0Reg.Data = MemReadPciCfgMC (host, socket, mcId, GetCpgcRegOffset (host, socket, ch, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG));
    cpgcSubseq1Ctl0Reg.Bits.num_cachelines = matsBurstBits;
    cpgcSubseq1Ctl0Reg.Bits.rstcurr_ba_start = 1;
    cpgcSubseq1Ctl0Reg.Bits.savecurr_ba_start = 0;
    cpgcSubseq1Ctl0Reg.Bits.num_cachelines_scale = 0;
    cpgcSubseq1Ctl0Reg.Bits.subseq_wait = 0;
    if (seqDataInv[1]) {
      cpgcSubseq1Ctl0Reg.Bits.inv_dataecc = 1;
    } else {
      cpgcSubseq1Ctl0Reg.Bits.inv_dataecc = 0;
    }
    MemWritePciCfgMC (host, socket, mcId, GetCpgcRegOffset (host, socket, ch, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG), cpgcSubseq1Ctl0Reg.Data);

    // Wrap at end of col, row, bank
    if (direction == MT_ADDR_DIR_UP) {
      seqBaseAddrStartLo0.Data             = 0;
      seqBaseAddrStartHi0.Data             = 0;
      //seqBaseAddrStartLo0.Bits.bank_addr   = 0;
      seqBaseAddrStartLo0.Bits.bank_addr   = bank;
      seqBaseAddrWrapLo0.Data              = 0;
      seqBaseAddrWrapHi0.Data              = 0;
      seqBaseAddrWrapLo0.Bits.column_addr  = (1 << cpgcColAddressBits) - 1;
      seqBaseAddrWrapLo0.Bits.bank_addr    = (1 << cpgcBankAddressBits) - 1;
      seqBaseAddrWrapHi0.Bits.row_addr     = (1 << cpgcRowAddressBits) - 1;
    } else {

      seqBaseAddrStartLo0.Data             = 0;
      seqBaseAddrStartHi0.Data             = 0;
      seqBaseAddrStartLo0.Bits.column_addr = (1 << cpgcColAddressBits) - 1;
      //seqBaseAddrStartLo0.Bits.bank_addr   = (1 << cpgcBankAddressBits) - 1;
      seqBaseAddrStartLo0.Bits.bank_addr   = (1 << cpgcBankAddressBits) - 1 - bank;
      seqBaseAddrStartHi0.Bits.row_addr    = (1 << cpgcRowAddressBits) - 1;
      seqBaseAddrWrapLo0.Data              = 0;
      seqBaseAddrWrapHi0.Data              = 0;
    }

    // Programs GPGC to run on the selected physical ranks
    SelectCPGCRanks(host, socket, ch, 1 << logRank, BIT3 | BIT1, logSubRank);
    ddrtMiscDelay.Data  = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
    ddrtMiscDelay.Bits.force_ddr4_mode = 0;
    ddrtMiscDelay.Bits.force_ddrt_mode = 0;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);

    //
    // Sequence BAIN control
    //
    // bank, row, column
    seqBaincCtl00.Data = 0;
    seqBaincCtl10.Data = 0;
    seqBaincCtl00.Bits.column_ba_updscale = 0;
    seqBaincCtl00.Bits.row_ba_updscale    = 0;
    seqBaincCtl10.Bits.bank_ba_updscale   = 0;
    seqBaincCtl10.Bits.rank_ba_inc        = 0;
    seqBaincCtl10.Bits.rank_ba_updrate    = 0;
    seqBaincCtl10.Bits.rank_ba_updscale   = 0;

    //seqBaincCtl00.Bits.column_ba_updrate  = 0;
    //seqBaincCtl00.Bits.row_ba_updrate     = cpgcColAddressBits;
    //seqBaincCtl10.Bits.bank_ba_updrate    = cpgcColAddressBits + cpgcRowAddressBits;

    seqBaincCtl10.Bits.bank_ba_updrate    = 0;
    seqBaincCtl00.Bits.column_ba_updrate  = cpgcBankAddressBits - baseBits;
    seqBaincCtl00.Bits.row_ba_updrate     = cpgcColAddressBits + cpgcBankAddressBits - baseBits;

    //if (direction == MT_ADDR_DIR_UP) {
    //  seqBaincCtl10.Bits.column_ba_inc_msb  = 0;
    //  seqBaincCtl00.Bits.column_ba_inc      = 1;
    //  seqBaincCtl00.Bits.row_ba_inc         = 1;
    //  seqBaincCtl10.Bits.bank_ba_inc        = 1;
    //} else {
    //  seqBaincCtl10.Bits.column_ba_inc_msb  = (UINT32)-1;
    //  seqBaincCtl00.Bits.column_ba_inc      = (UINT32)-1;
    //  seqBaincCtl00.Bits.row_ba_inc         = (UINT32)-1;
    //  seqBaincCtl10.Bits.bank_ba_inc        = (UINT32)-1;
    //}

    if (direction == MT_ADDR_DIR_UP) {
      seqBaincCtl10.Bits.column_ba_inc_msb  = 0;
      seqBaincCtl00.Bits.column_ba_inc      = 1;
      seqBaincCtl00.Bits.row_ba_inc         = 1;
      seqBaincCtl10.Bits.bank_ba_inc        = (1 << baseBits);
    } else {
      seqBaincCtl10.Bits.column_ba_inc_msb  = 0x3FF;
      seqBaincCtl00.Bits.column_ba_inc      = 0xFF;
      seqBaincCtl00.Bits.row_ba_inc         = 0xFFF;
      seqBaincCtl10.Bits.bank_ba_inc        = (UINT32)-(1 << baseBits);
    }

    cpgcSeqBaoCic0.Data = 0;
    cpgcSeqBaoCic0.Bits.column_addr_order = 0;
    cpgcSeqBaoCic0.Bits.row_addr_order = 1;
    cpgcSeqBaoCic0.Bits.bank_addr_order = 2;
    cpgcSeqBaoCic0.Bits.rank_addr_order = 3;

    //if (mode == MT_CPGC_READ_WRITE) {
    //cpgcSeqBaoCic0.Bits.column_bw_cryen = 1;
    //}

    CpgcSetupAddrGen (host, socket, ch,
                      seqBaincCtl00,
                      seqBaincCtl10,
                      seqBaseAddrStartLo0,
                      seqBaseAddrStartHi0,
                      seqBaseAddrWrapLo0,
                      seqBaseAddrWrapHi0,
                      cpgcSeqBaoCic0
                     );

    // GPGC traffic only uses BA[1:0] - Mask BANK that will not be used
    seqDmyRdAddr.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG);
    seqDmyRdAddr.Bits.bank_mask0 = 0xffff;
    MemWritePciCfgMC (host, socket, mcId, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG, seqDmyRdAddr.Data);

  } // ch

  return;
} // CpgcMemTestRankSetupMATS


/**
  WDB setup for Advanced Memory test

  CPGC address setup for Advanced memory test

  @param host             - Pointer to sysHost
  @param socket           - Memory controller to train
  @param ch               - ddr channel to setup
  @param mode                - Type of sequence MT_CPGC_WRITE, MT_CPGC_READ, or MT_CPGC_READ_WRITE
  @param seqDataInv[MT_MAX_SUBSEQ]    - Enables pattern inversion per subsequence
  @param patternQW[MT_PATTERN_DEPTH]  - 64-bit wide data pattern to use per UI
  @param uiShl[MT_PATTERN_DEPTH]      - Enables pattern rotation per UI
  @param numCacheLines                - Number of cachelines to use in WDB
  @param patternDepth                 - Number of UI to use in patternQW
  @param direction                    - Sequential address direction MT_ADDR_DIR_UP, MT_ADDR_DIR_DN

  @retval None

**/
static void
WriteWDBFixedPatternMATS(
                    PSYSHOST          host,
                    UINT8             socket,
                    UINT8             ch,
                    UINT8             mode,
                    UINT8             seqDataInv[MT_MAX_SUBSEQ],
                    UINT64_STRUCT     patternQW[MT_PATTERN_DEPTH],
                    UINT8             uiShl[MT_PATTERN_DEPTH],
                    UINT8             numCacheLines,
                    UINT8             patternDepth,
                    UINT8             direction
                    )
{
  UINT8             line;
  UINT8             i;
  UINT8             index;
  UINT8             temp;
  UINT32            *WdbLinePtr;
  TWdbLine          WdbLines[MRC_WDB_LINES];
  UINT64_STRUCT     tempQW[MT_PATTERN_DEPTH];
  UINT32    templo, temphi;

  // Check for error
  if (patternDepth > MT_PATTERN_DEPTH) {
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_48);
  }
  // Use local variable
  for (index = 0; index < MT_PATTERN_DEPTH; index++) {
    tempQW[index].lo = patternQW[index].lo;
    tempQW[index].hi = patternQW[index].hi;

    // Handle down direction
    if (direction == MT_ADDR_DIR_DN) {
      tempQW[index] = RotateLeftUINT64 (tempQW[index], uiShl[index] * 8 * (numCacheLines - 1));
    }
  }

  // For each cacheline (burst of 8 UI)
  for (line = 0; line < numCacheLines; line++) {

    WdbLinePtr = (UINT32 *) &WdbLines[line].WdbLine[0];
    for (i = 0; i < (MRC_WDB_LINE_SIZE / sizeof (UINT32)); i++) {

      // Chunk 0 gets pattern[0]
      // Chunk 1 gets pattern[1]
      // ...
      // Chunk N gets pattern[N MOD MT_PATTERN_DEPTH]

      // program 64-bit UI of line (8UI per cache line)
      index = (i / 2) % patternDepth;

      // Handle down direction
      if (line && (direction == MT_ADDR_DIR_DN)) {
        for (temp = 0; temp < patternDepth; temp++) {
          if (i == temp * 2) {
            tempQW[index] = RotateRightUINT64 (tempQW[index], uiShl[index] * 16);
            break;
          }
        }
      }
      if (i & 1) {
        // Most significant 32 bits of UI
        WdbLinePtr[i] = tempQW[index].hi;
      } else {
        // Least significant 32 bits of UI
        WdbLinePtr[i] = tempQW[index].lo;
      }
      // Rotate pattern left for each UI
      if (i % 2 == 1) {
        tempQW[index] = RotateLeftUINT64 (tempQW[index], uiShl[index]);
      }
    } // i loop
  } // line loop

  //
  // Print debug info... patterns expected on the bus
  //
  // subsequence 0
  if (mode == MT_CPGC_WRITE) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Subseq0 Write pattern:\n"));
  } else {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Subseq0 Read pattern:\n"));
  }
  // For each cacheline (burst of 8 UI)
  for (line = 0; line < numCacheLines; line++) {
    WdbLinePtr = (UINT32 *) &WdbLines[line].WdbLine[0];
    for (i = 0; i < (MRC_WDB_LINE_SIZE / sizeof (UINT32)); i++) {
      if ((i & 1) && (i > 0)) {
        if (seqDataInv[0]) {
          temphi = ~WdbLinePtr[i];
          templo = ~WdbLinePtr[i-1];
        } else {
          temphi = WdbLinePtr[i];
          templo = WdbLinePtr[i-1];
        }
        MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "CL%2d UI%d 0x%08x%08x\n", line, i/2, temphi, templo));
      }
    } // i loop
  } // line loop

  // subsequence 1
  if (mode == MT_CPGC_READ_WRITE) {
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Subseq1 Write pattern:\n"));

    // For each cacheline (burst of 8 UI)
    for (line = 0; line < numCacheLines; line++) {
      WdbLinePtr = (UINT32 *) &WdbLines[line].WdbLine[0];
      for (i = 0; i < (MRC_WDB_LINE_SIZE / sizeof (UINT32)); i++) {
        if ((i & 1) && (i > 0)) {
          if (seqDataInv[1]) {
            temphi = ~WdbLinePtr[i];
            templo = ~WdbLinePtr[i-1];
          } else {
            temphi = WdbLinePtr[i];
            templo = WdbLinePtr[i-1];
          }
          MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "CL%2d UI%d 0x%08x%08x\n", line, i/2, temphi, templo));
        }
      } // i loop
    } // line loop
  }

  //
  // Fill the WDB buffer with the write content.
  //
  WDBFill (host, socket, ch, WdbLines, numCacheLines, 0);

} // WriteWDBFixedPatternMATS


/**

  CPGC address setup for Advanced memory test

  @param host             - Pointer to sysHost
  @param socket           - Memory controller to train
  @param ch               - ddr channel to setup
  @param wdbIncrRate      - increment rate
  @param wdbIncrScale     - increment scale
  @param wdbStartPntr     - start pointer
  @param wdbEndPntr       - end pointer
  @param setupPbwr        - flag to indicate setup of Pb write
  @param muxCtl           - mux control (0 - LMN, 1 - pat buffer,  2 - LFSR242)
  @param seqBaincCtl00       - base addresses increment fields
  @param seqBaincCtl10       - base addresses increment fields
  @param seqBaseAddrStartLo0 - base address start fields
  @param seqBaseAddrStartHi0 - base address start fields
  @param seqBaseAddrWrapLo0  - base address wrap fields
  @param seqBaseAddrWrapHi0  - base address wrap fields
  @param cpgcSeqBaoCic0      - base address update rate, carry fields
  @param mode                - Type of sequence MT_CPGC_WRITE, MT_CPGC_READ, or MT_CPGC_READ_WRITE

  @retval None

**/
static void
CpgcSetupWdbMATS (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     wdbIncrRate,
  UINT8     wdbIncrScale,
  UINT8     wdbStartPntr,
  UINT8     wdbEndPntr,
  UINT8     setupPbwr,
  UINT8     muxCtl,
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT          seqBaincCtl00,
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT          seqBaincCtl10,
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT   seqBaseAddrStartLo0,
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT   seqBaseAddrStartHi0,
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT    seqBaseAddrWrapLo0,
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT    seqBaseAddrWrapHi0,
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT             cpgcSeqBaoCic0,
  UINT8     mode
  )
{
  UINT8 lfsr2code;
  UINT32 patBuf0rd;
  UINT32 patBuf1rd;
  UINT32 patBuf2rd;
  UINT32 patBuf0wr;
  UINT32 patBuf1wr;
  UINT32 patBuf2wr;
  CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_STRUCT cpgcPatWdbclMux0Pbrd;
  CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_STRUCT cpgcPatWdbclMux1Pbrd;
  CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_STRUCT cpgcPatWdbclMux2Pbrd;
  CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_STRUCT cpgcPatWdbclMux0Pbwr;
  CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_STRUCT cpgcPatWdbclMux1Pbwr;
  CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_STRUCT cpgcPatWdbclMux2Pbwr;
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT    cpgcPatWdbClMuxCtl;
  CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT       cpgcPatWdbClCtl;

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcSetupWdbMATS\n"));

  //
  // Program the fixed patterns to emulate normal functional mode. The 3 patterns emulates
  // counter counting from 0-7.
  //
  patBuf0rd = DDR_CPGC_PATBUF_MUX0;
  patBuf1rd = DDR_CPGC_PATBUF_MUX1;
  patBuf2rd = DDR_CPGC_PATBUF_MUX2;

  patBuf0wr = DDR_CPGC_PATBUF_MUX0;
  patBuf1wr = DDR_CPGC_PATBUF_MUX1;
  patBuf2wr = DDR_CPGC_PATBUF_MUX2;

  lfsr2code = 0;

  //
  // Program CPGC WDB Mux Selection
  //
  // select 32-bit buffer/LFSR24
  // replace ECC byte with ecc_datasrc_sel
  // select BL0 to be used as the ECC data byte for comparison
  //
  cpgcPatWdbClMuxCtl.Data = 0;
  cpgcPatWdbClMuxCtl.Bits.mux0 = muxCtl;
  cpgcPatWdbClMuxCtl.Bits.mux1 = muxCtl;
  cpgcPatWdbClMuxCtl.Bits.mux2 = muxCtl;

  cpgcPatWdbClMuxCtl.Bits.ecc_replace_byte = 1;
  cpgcPatWdbClMuxCtl.Bits.ecc_datasrc_sel = 0;
  cpgcPatWdbClMuxCtl.Bits.save_lfsr_seedrate = 0;
  cpgcPatWdbClMuxCtl.Bits.rld_lfsr_seedrate = 0;
  cpgcPatWdbClMuxCtl.Bits.cpgc_lfsr_2code_mode = lfsr2code;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, cpgcPatWdbClMuxCtl.Data);

  //
  // Program CPGC WDB Pointer Control
  //
  cpgcPatWdbClCtl.Data = 0;
  cpgcPatWdbClCtl.Bits.incr_rate = wdbIncrRate;
  cpgcPatWdbClCtl.Bits.incr_scale = wdbIncrScale;
  cpgcPatWdbClCtl.Bits.start_pntr = wdbStartPntr;
  cpgcPatWdbClCtl.Bits.end_pntr = wdbEndPntr;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCLCTL_MCDDC_CTL_REG, cpgcPatWdbClCtl.Data);

  //
  // Setup wr / rd buffers
  //
  // Program the fixed patterns to emulate normal functional mode. The 3 patterns emulates
  // counter counting from 0-7.
  //
  if (setupPbwr) {
    cpgcPatWdbclMux0Pbwr.Data = 0;
    cpgcPatWdbclMux0Pbwr.Bits.patbuffer = patBuf0wr;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG, cpgcPatWdbclMux0Pbwr.Data);

    cpgcPatWdbclMux1Pbwr.Data = 0;
    cpgcPatWdbclMux1Pbwr.Bits.patbuffer = patBuf1wr;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG, cpgcPatWdbclMux1Pbwr.Data);

    cpgcPatWdbclMux2Pbwr.Data = 0;
    cpgcPatWdbclMux2Pbwr.Bits.patbuffer = patBuf2wr;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG, cpgcPatWdbclMux2Pbwr.Data);
  }

  cpgcPatWdbclMux0Pbrd.Data = 0;
  cpgcPatWdbclMux0Pbrd.Bits.patbuffer = patBuf0rd;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG, cpgcPatWdbclMux0Pbrd.Data);

  cpgcPatWdbclMux1Pbrd.Data = 0;
  cpgcPatWdbclMux1Pbrd.Bits.patbuffer = patBuf1rd;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG, cpgcPatWdbclMux1Pbrd.Data);

  cpgcPatWdbclMux2Pbrd.Data = 0;
  cpgcPatWdbclMux2Pbrd.Bits.patbuffer = patBuf2rd;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG, cpgcPatWdbclMux2Pbrd.Data);

  //
  // Setup CPGC Address Generation
  //
  CpgcSetupAddrGen (host, socket, ch,
                    seqBaincCtl00,
                    seqBaincCtl10,
                    seqBaseAddrStartLo0,
                    seqBaseAddrStartHi0,
                    seqBaseAddrWrapLo0,
                    seqBaseAddrWrapHi0,
                    cpgcSeqBaoCic0
                   );
  return;
} // CpgcSetupWdbMATS


/**

  CPGC global setup for Advanced memory test

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ch          - ddr channel to setup
  @param rank        - Logical rank number
  @param dqLoopCount - CPGC sequence loop count - number of times sequence is executed (2^(dqLoopCount - 1))
  @param mode        - Type of sequence MT_CPGC_WRITE, MT_CPGC_READ, or MT_CPGC_READ_WRITE
  @param seqDataInv[MT_MAX_SUBSEQ]    - Enables pattern inversion per subsequence
  @param patternQW[MT_PATTERN_DEPTH]  - 64-bit wide data pattern to use per UI
  @param uiShl[MT_PATTERN_DEPTH]      - Enables pattern rotation per UI
  @param numCacheLines                - Number of cachelines to use in WDB
  @param patternDepth                 - Number of UI to use in patternQW
  @param direction                    - Sequential address direction MT_ADDR_DIR_UP, MT_ADDR_DIR_DN

  @retval SUCCESS

**/
static UINT32
CpgcMemTestGlobalSetupMATS (
                       PSYSHOST  host,
                       UINT8     socket,
                       UINT8     ch,
                       UINT8     rank,
                       UINT8     dqLoopCount,
                       UINT8     testType,
                       UINT8     mode,
                       UINT8     seqDataInv[MT_MAX_SUBSEQ],
                       UINT64_STRUCT patternQW[MT_PATTERN_DEPTH],
                       UINT8     uiShl[MT_PATTERN_DEPTH],
                       UINT8     numCacheLines,
                       UINT8     patternDepth,
                       UINT8     direction
                       )
{

  UINT32                                        status;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT            cpgcPatCadbCtlReg;
  CPGC_GLOBAL_TRAINING_SETUP                  cpgcGblTrainingSetup;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT          seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT          seqBaincCtl10;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT   seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT   seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT    seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT    seqBaseAddrWrapHi0;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT             cpgcSeqBaoCic0;

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, rank, NO_STROBE, NO_BIT,
                 "CpgcMemTestSetupMATS:\n  pattern[0] = 0x%08x%08x\n  pattern[1] = 0x%08x%08x\n",
                 patternQW[0].hi, patternQW[0].lo, patternQW[1].hi, patternQW[1].lo));

  status = SUCCESS;

  cpgcGblTrainingSetup.seqLoopCnt        = dqLoopCount;                      // test will end after (2^(n-1)) loops through the sequence
  cpgcGblTrainingSetup.startDelay        = 0;                                // Delay start
  cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;                // Update based on the # of read and write cacheline


  // Default to WRITE
  cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0

  // If MEM_TEST is read, set to READ
  if (mode == MT_CPGC_READ_WRITE || mode == MT_CPGC_READ) {
    cpgcGblTrainingSetup.rwMode          = CPGC_BASE_READ_SUBSEQ;            // Base Read or write - subseq0
  }

  cpgcGblTrainingSetup.numCacheLines     = 0;                                // Number of Cachelines (2^N)
  cpgcGblTrainingSetup.burstSeparation   = 0;                                // (sub_seqwait) - need to add delay to drain credits if doing write / read subseq
  cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_ALL_CHUNK;       // monitor errors on any chunk
  cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_ALL_CL;          // monitor errors on any cacheline
  if (testType == ADV_MT_TYPE_LTEST_SCRAM) {
    cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_STOP_ON_ALL_ERR;         // Stop on all lane error
  } else {
    cpgcGblTrainingSetup.stopOnErrCtl      = 1;                                // Stop on first error
  }

  if (mode == MT_CPGC_READ_WRITE) {
    cpgcGblTrainingSetup.subseqEndPtr    = 1;                                // subseq end pointer
    cpgcGblTrainingSetup.useSubSeq1      = CPGC_BASE_WRITE_SUBSEQ;
  } else {
    cpgcGblTrainingSetup.subseqEndPtr    = 0;                                // subseq end pointer
    cpgcGblTrainingSetup.useSubSeq1      = CPGC_NO_SUBSEQ1;                  // do not use sub seq1 (base read)
  }
  cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
  cpgcGblTrainingSetup.back2backWrEn     = 1;                                // enable b2b writes
  cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
  cpgcGblTrainingSetup.refMask           = 0;                                // enable cpgc self refresh
  cpgcGblTrainingSetup.zqMask            = 0;                                // enable cpgc zq
  cpgcGblTrainingSetup.x8Mode            = 0;

  CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

  //
  // Set WDB address order
  //
  seqBaincCtl00.Data = 0;
  seqBaincCtl10.Data = 0;
  cpgcSeqBaoCic0.Data = 0;
  seqBaseAddrStartLo0.Data = 0;
  seqBaseAddrStartHi0.Data = 0;
  seqBaseAddrWrapLo0.Data = 0;
  seqBaseAddrWrapHi0.Data = 0;


  CpgcSetupWdbMATS (host, socket, ch,
                0,                           // wdbIncrRate
                0,                           // wdbIncrScale
                0,                           // wdbStartPntr (victim bit 0)
                numCacheLines - 1,           // wdbEndPntr (victim bit 0)
                1,                           // setupPbwr
                CPGC_WDB_MUX_CTL_PATBUF,     // mux control
                seqBaincCtl00,               // column base address incr
                seqBaincCtl10,
                seqBaseAddrStartLo0,
                seqBaseAddrStartHi0,
                seqBaseAddrWrapLo0,
                seqBaseAddrWrapHi0,
                cpgcSeqBaoCic0,
                mode
               );

  //
  // Disable CADB
  //
  cpgcPatCadbCtlReg.Data = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  //
  // Program WDB data with given pattern
  //
  WriteWDBFixedPatternMATS(host, socket, ch, mode, seqDataInv, patternQW, uiShl, numCacheLines, patternDepth, direction);

  return status;

} // CpgcMemTestGlobalSetupMATS


/**

  Collect DDR advanced command CPGC error status

  @param host                - Pointer to sysHost
  @param socket              - Memory controller number
  @param ddrChEnMap          - channel mask
  @param cpgcErrorStatus[MAX_CH] - array of error status

  @retval   SUCCESS/FAILURE

**/
UINT32
CpgcAdvTrainingErrorStatusMATS (
  PSYSHOST          host,
  UINT8             socket,
  UINT32            ddrChEnMap,
  CPGC_ERROR_STATUS_MATS cpgcErrorStatus[MAX_CH]
  )
{

  UINT32 status;
  UINT8 mcId;
  UINT8 ch;
  CPGC_ERROR_STATUS                         cpgcErrorStatusLegacy[MAX_CH];
  CPGC_ERR_ADDR0_MCDDC_CTL_STRUCT           cpgcErrAddr0;
  CPGC_ERR_ADDR1_MCDDC_CTL_STRUCT           cpgcErrAddr1;
  CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_STRUCT    seqRankLg2PhyMap0;
  CPGC_SEQ_LRRANKLG2PHYMAP0_MC_MAIN_STRUCT  seqLrRankLg2PhyMap0;

  status = SUCCESS;

  for (ch = 0; ch < MAX_CH; ch++) {

    // skip inactive channels
    if (!(ddrChEnMap & (1 << ch))) continue;

    cpgcErrorStatus[ch].cpgcErrDat0S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA0_S_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrDat1S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA1_S_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrDat2S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA2_S_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrDat3S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA3_S_MCDDC_CTL_REG);
    if (host->nvram.mem.eccEn) {
      cpgcErrorStatus[ch].cpgcErrEccS  = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_ECC_S_MCDDC_CTL_REG);
    } else {
      cpgcErrorStatus[ch].cpgcErrEccS  = 0;
    }

    mcId = GetMCID(host, socket, ch);
    seqLrRankLg2PhyMap0.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_SEQ_LRRANKLG2PHYMAP0_MC_MAIN_REG + (4 * GetMCCh(mcId, ch)));
    cpgcErrorStatus[ch].cpgcSubrank = seqLrRankLg2PhyMap0.Bits.lr0_mapping;

    seqRankLg2PhyMap0.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG + (4 * GetMCCh(mcId, ch)));
    cpgcErrorStatus[ch].cpgcErrRank = seqRankLg2PhyMap0.Bits.rank0_mapping;

    cpgcErrorStatus[ch].cpgcErrChunk = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_CHUNK_MCDDC_CTL_REG);
    cpgcErrAddr0.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_ADDR0_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrRow = cpgcErrAddr0.Bits.row;
    cpgcErrAddr1.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_ADDR1_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrColumn = cpgcErrAddr1.Bits.column;
    cpgcErrorStatus[ch].cpgcErrBank = cpgcErrAddr1.Bits.bank;

    MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "CpgcAdvTrainingErrorStatusMATS: cpgcErr0 = 0x%x, cpgcErr1 = 0x%x, cpgcErr2 = 0x%x, cpgcErr3 = 0x%x, cpgcErrEccS = 0x%x\n",
                   cpgcErrorStatus[ch].cpgcErrDat0S,
                   cpgcErrorStatus[ch].cpgcErrDat1S,
                   cpgcErrorStatus[ch].cpgcErrDat2S,
                   cpgcErrorStatus[ch].cpgcErrDat3S,
                   cpgcErrorStatus[ch].cpgcErrEccS));

    cpgcErrorStatusLegacy[ch].cpgcErrDat0S = cpgcErrorStatus[ch].cpgcErrDat0S;
    cpgcErrorStatusLegacy[ch].cpgcErrDat1S = cpgcErrorStatus[ch].cpgcErrDat1S;
    cpgcErrorStatusLegacy[ch].cpgcErrDat2S = cpgcErrorStatus[ch].cpgcErrDat2S;
    cpgcErrorStatusLegacy[ch].cpgcErrDat3S = cpgcErrorStatus[ch].cpgcErrDat3S;
    cpgcErrorStatusLegacy[ch].cpgcErrEccS  = cpgcErrorStatus[ch].cpgcErrEccS;

    if (CpgcCheckTransientErrs(host, socket, ch, cpgcErrorStatusLegacy)) {
      // If transient error found print updated error status

      cpgcErrorStatus[ch].cpgcErrDat0S = cpgcErrorStatusLegacy[ch].cpgcErrDat0S;
      cpgcErrorStatus[ch].cpgcErrDat1S = cpgcErrorStatusLegacy[ch].cpgcErrDat1S;
      cpgcErrorStatus[ch].cpgcErrDat2S = cpgcErrorStatusLegacy[ch].cpgcErrDat2S;
      cpgcErrorStatus[ch].cpgcErrDat3S = cpgcErrorStatusLegacy[ch].cpgcErrDat3S;
      cpgcErrorStatus[ch].cpgcErrEccS  = cpgcErrorStatusLegacy[ch].cpgcErrEccS;

      MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "CpgcAdvTrainingErrorStatusMATS (Post Transient Check): cpgcErr0 = 0x%x, cpgcErr1 = 0x%x, cpgcErr2 = 0x%x, cpgcErr3 = 0x%x, cpgcErrEccS = 0x%x\n",
        cpgcErrorStatus[ch].cpgcErrDat0S,
        cpgcErrorStatus[ch].cpgcErrDat1S,
        cpgcErrorStatus[ch].cpgcErrDat2S,
        cpgcErrorStatus[ch].cpgcErrDat3S,
        cpgcErrorStatus[ch].cpgcErrEccS));
    }
  } // for

  return status;

} // CpgcAdvTrainingErrorStatusMATS

#endif //not ADV_MEMTEST_SUPPORT

#ifndef ADV_MEMTEST_SUPPORT
/**

  DDR Mem test / Mem Init CPGC setup.

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ch          - ddr channel to setup
  @param dqLoopCount - dq sequence loop count - number of times dq sequence is executed (2^(dqLoopCount - 1))
  @param pattern     - flag to indicate mem init vs. mem test and read vs. write and data pattern

  @retval SUCCESS / FAILURE

**/
static UINT32
CpgcMemTestGlobalSetup (
                       PSYSHOST  host,
                       UINT8     socket,
                       UINT8     ch,
                       UINT8     rank,
                       UINT8     dqLoopCount,
                       UINT32    pattern
                       )
{

  UINT32                                        status;
  UINT32                                        wdbData;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
  CPGC_GLOBAL_TRAINING_SETUP                    cpgcGblTrainingSetup;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;


  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, rank, NO_STROBE, NO_BIT,
                 "CpgcMemTestSetup: pattern = 0x%x\n", pattern));

  status = SUCCESS;

  cpgcGblTrainingSetup.seqLoopCnt        = dqLoopCount;                      // test will end after (2^(n-1)) loops through the sub-sequence
  cpgcGblTrainingSetup.startDelay        = 0;                                // Delay start
  cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;                // Update based on the # of read and write cacheline

  // Default to WRITE
  cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0

  // If MEM_TEST is read, set to READ
  if (pattern & DDR_CPGC_MEM_TEST_READ) {
    cpgcGblTrainingSetup.rwMode          = CPGC_BASE_READ_SUBSEQ;            // Base Read or write - subseq0
  }

  cpgcGblTrainingSetup.numCacheLines     = 0;                                // Number of Cachelines (2^N)
  cpgcGblTrainingSetup.burstSeparation   = 0;                                // (sub_seqwait) - need to add delay to drain credits if doing write / read subseq
  cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_ALL_CHUNK;       // monitor errors on any chunk
  cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_ALL_CL;          // monitor errors on any cacheline
  cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_STOP_ON_ALL_ERR;         // Stop on all lane error
  cpgcGblTrainingSetup.subseqEndPtr      = 0;                                // subseq end pointer
  cpgcGblTrainingSetup.useSubSeq1        = CPGC_NO_SUBSEQ1;                  // do not use sub seq1 (base read)
  cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
  cpgcGblTrainingSetup.back2backWrEn     = 1;                                // enable b2b writes
  cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
  cpgcGblTrainingSetup.refMask           = 0;                                // enable cpgc self refresh
  cpgcGblTrainingSetup.zqMask            = 0;                                // enable cpgc zq
  //if(x8) {
  cpgcGblTrainingSetup.x8Mode           = 0;
  //} else {
  // cpgcGblTrainingSetup.x8Mode           = 0;
  //}

  CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

  //
  // Set WDB address order
  //
  seqBaincCtl00.Data = 0;
  seqBaincCtl00.Bits.row_ba_updrate = 7;
  seqBaincCtl00.Bits.row_ba_inc = 1;
  seqBaincCtl00.Bits.column_ba_inc = 1;

  seqBaincCtl10.Data = 0;
  seqBaincCtl10.Bits.bank_ba_updrate = 16;
  seqBaincCtl10.Bits.bank_ba_inc = 1;

  seqBaseAddrStartLo0.Data = 0;
  seqBaseAddrStartHi0.Data = 0;

  seqBaseAddrWrapLo0.Data = 0;
  seqBaseAddrWrapLo0.Bits.bank_addr = 0xf;
  seqBaseAddrWrapLo0.Bits.column_addr = 0x7f;

  seqBaseAddrWrapHi0.Data = 0;
  seqBaseAddrWrapHi0.Bits.row_addr = 0x7fff;
  seqBaseAddrWrapHi0.Bits.rank_addr = rank;  // note: assume logical rank = 0; logical to physical mapping is done by CpgcMemTestRankSetup

  cpgcSeqBaoCic0.Data = 0;
  cpgcSeqBaoCic0.Bits.row_bw_cryen = 1;
  cpgcSeqBaoCic0.Bits.column_bw_cryen = 1;

  CpgcSetupWdb (host, socket, ch,
                0,                           // wdbIncrRate
                0,                           // wdbIncrScale
                0,                           // wdbStartPntr (victim bit 0)
                0,                           // wdbEndPntr (victim bit 0)
                1,                           // setupPbwr
                CPGC_WDB_MUX_CTL_PATBUF,     // mux control
                seqBaincCtl00,               // column base address incr
                seqBaincCtl10,
                seqBaseAddrStartLo0,
                seqBaseAddrStartHi0,
                seqBaseAddrWrapLo0,
                seqBaseAddrWrapHi0,
                cpgcSeqBaoCic0
               );

  //
  // Disable CADB
  //
  cpgcPatCadbCtlReg.Data = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  //
  // Program WDB data with constant 0's or 1's
  //
  wdbData = 0;

  if (pattern & DDR_CPGC_MEM_TEST_ALL0) {
    wdbData = 0;
  } else if (pattern & DDR_CPGC_MEM_TEST_A5) {
    wdbData = 0xA5A5A5A5;
  }

  // Invert pattern
  if (pattern & DDR_CPGC_MEM_TEST_INVERTPAT) {
    wdbData = ~wdbData;
  }

  WriteWDBFixedPattern(host, socket, ch, wdbData, 8, 0);

  return status;

} // CpgcMemTestGlobalSetup
#endif //not ADV_MEMTEST_SUPPORT

/**

  Setup WDB for training.

  @param host             - pointer to sysHost
  @param socket           - socket to train
  @param ch               - ddr channel
  @param wdbIncrRate      - increment rate
  @param wdbIncrScale     - increment scale
  @param wdbStartPntr     - start pointer
  @param wdbEndPntr       - end pointer
  @param setupPbwr        - flag to indicate setup of Pb write
  @param muxCtl           - mux control (0 - LMN, 1 - pat buffer,  2 - LFSR242)
  @param colBaInc         - column base addresses increment
  @param rowBaInc         - rwo base addresses increment
  @param bankBaInc        - bank base addresses increment
  @param baseAddressStart - base address start
  @param baseAddressWrap  - base address wrap

  @retval n/a

**/
static void
CpgcSetupWdb (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     wdbIncrRate,
             UINT8     wdbIncrScale,
             UINT8     wdbStartPntr,
             UINT8     wdbEndPntr,
             UINT8     setupPbwr,
             UINT8     muxCtl,
             CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00,
             CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10,
             CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0,
             CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0,
             CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0,
             CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0,
             CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0
             )
{
  UINT8 scheduler;
  UINT8 lfsr2code;
  UINT32 lfsrSeedOffset;
  UINT32 patBuf0;
  UINT32 patBuf1;
  UINT32 patBuf2;
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT    cpgcPatWdbClMuxCtl;
  CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT       cpgcPatWdbClCtl;
  CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_STRUCT cpgcPatWdbclMux0Pbrd;
  CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_STRUCT cpgcPatWdbclMux1Pbrd;
  CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_STRUCT cpgcPatWdbclMux2Pbrd;
  CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_STRUCT cpgcPatWdbclMux0Pbwr;
  CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_STRUCT cpgcPatWdbclMux1Pbwr;
  CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_STRUCT cpgcPatWdbclMux2Pbwr;

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcSetupWdb\n"));

  // TODO: update this. does not work the same for EP
  scheduler = ch % 2;

  //
  // Program the fixed patterns to emulate normal functional mode. The 3 patterns emulates
  // counter counting from 0-7.
  //
  patBuf0 = DDR_CPGC_PATBUF_MUX0;
  patBuf1 = DDR_CPGC_PATBUF_MUX1;
  patBuf2 = DDR_CPGC_PATBUF_MUX2;
  lfsr2code = 0;

  //
  // patbuffer is either fixed pattern buffer or LFSR24 seed depending on mux control
  //
  if (muxCtl == CPGC_WDB_MUX_CTL_LFSR24) {
    //
    // Program LFSR seeds. Seed are different for each scheduler. Also seeds change for each test run.
    //
    lfsrSeedOffset = host->var.mem.initialLFSRSeed++;
    patBuf0 = CPGC_LFSR0_INIT_SEED_HSX (scheduler) + lfsrSeedOffset;
    patBuf1 = CPGC_LFSR1_INIT_SEED_HSX (scheduler) + lfsrSeedOffset;
    patBuf2 = CPGC_LFSR2_INIT_SEED_HSX (scheduler) + lfsrSeedOffset;
  } else if (muxCtl == CPGC_WDB_MUX_CTL_LFSR24_PATBUF){
      lfsrSeedOffset = host->var.mem.initialLFSRSeed++;
      patBuf0 = CPGC_LFSR0_INIT_SEED_HSX (scheduler) + lfsrSeedOffset;
      patBuf1 = DDR_CPGC_PATBUF_MUX3;
      patBuf2 = CPGC_LFSR1_INIT_SEED_HSX (scheduler) + lfsrSeedOffset;
  }

  //
  // Program CPGC WDB Mux Selection
  //
  // select 32-bit buffer/LFSR24
  // replace ECC byte with ecc_datasrc_sel
  // select BL0 to be used as the ECC data byte for comparison
  //
  cpgcPatWdbClMuxCtl.Data = 0;
  if (muxCtl == CPGC_WDB_MUX_CTL_LFSR24_PATBUF) {
    cpgcPatWdbClMuxCtl.Bits.mux0 = CPGC_WDB_MUX_CTL_LFSR24;
    cpgcPatWdbClMuxCtl.Bits.mux1 = CPGC_WDB_MUX_CTL_PATBUF;
    cpgcPatWdbClMuxCtl.Bits.mux2 = CPGC_WDB_MUX_CTL_LFSR24;
  } else {
    cpgcPatWdbClMuxCtl.Bits.mux0 = muxCtl;
    cpgcPatWdbClMuxCtl.Bits.mux1 = muxCtl;
    cpgcPatWdbClMuxCtl.Bits.mux2 = muxCtl;
  }
  cpgcPatWdbClMuxCtl.Bits.ecc_replace_byte = 1;
  cpgcPatWdbClMuxCtl.Bits.ecc_datasrc_sel = 0;
  cpgcPatWdbClMuxCtl.Bits.save_lfsr_seedrate = 0;
  cpgcPatWdbClMuxCtl.Bits.rld_lfsr_seedrate = 0;
  cpgcPatWdbClMuxCtl.Bits.cpgc_lfsr_2code_mode = lfsr2code;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, cpgcPatWdbClMuxCtl.Data);

  //
  // Program CPGC WDB Pointer Control
  //
  cpgcPatWdbClCtl.Data = 0;
  cpgcPatWdbClCtl.Bits.incr_rate = wdbIncrRate;
  cpgcPatWdbClCtl.Bits.incr_scale = wdbIncrScale;
  cpgcPatWdbClCtl.Bits.start_pntr = wdbStartPntr;
  cpgcPatWdbClCtl.Bits.end_pntr = wdbEndPntr;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCLCTL_MCDDC_CTL_REG, cpgcPatWdbClCtl.Data);

  //
  // Setup wr / rd buffers
  //
  // Program the fixed patterns to emulate normal functional mode. The 3 patterns emulates
  // counter counting from 0-7.
  //
  if (setupPbwr) {
    cpgcPatWdbclMux0Pbwr.Data = 0;
    cpgcPatWdbclMux0Pbwr.Bits.patbuffer = patBuf0;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG, cpgcPatWdbclMux0Pbwr.Data);

    cpgcPatWdbclMux1Pbwr.Data = 0;
    cpgcPatWdbclMux1Pbwr.Bits.patbuffer = patBuf1;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBWR_MCDDC_DP_REG, cpgcPatWdbclMux1Pbwr.Data);

    cpgcPatWdbclMux2Pbwr.Data = 0;
    cpgcPatWdbclMux2Pbwr.Bits.patbuffer = patBuf2;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBWR_MCDDC_DP_REG, cpgcPatWdbclMux2Pbwr.Data);
  }

  cpgcPatWdbclMux0Pbrd.Data = 0;
  cpgcPatWdbclMux0Pbrd.Bits.patbuffer = patBuf0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG, cpgcPatWdbclMux0Pbrd.Data);

  cpgcPatWdbclMux1Pbrd.Data = 0;
  cpgcPatWdbclMux1Pbrd.Bits.patbuffer = patBuf1;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG, cpgcPatWdbclMux1Pbrd.Data);

  cpgcPatWdbclMux2Pbrd.Data = 0;
  cpgcPatWdbclMux2Pbrd.Bits.patbuffer = patBuf2;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG, cpgcPatWdbclMux2Pbrd.Data);

  //
  // Setup CPGC Address Generation
  //
  CpgcSetupAddrGen (host, socket, ch,
                    seqBaincCtl00,
                    seqBaincCtl10,
                    seqBaseAddrStartLo0,
                    seqBaseAddrStartHi0,
                    seqBaseAddrWrapLo0,
                    seqBaseAddrWrapHi0,
                    cpgcSeqBaoCic0
                   );
  return;
} // CpgcSetupWdb

/**

  Setup CPGC base address generation

  @param host             - pointer to sysHost
  @param socket           - socket to train
  @param ch               - ddr channel

  @retval n/a

**/
static VOID
CpgcSetupAddrGen (
                 PSYSHOST                                      host,
                 UINT8                                         socket,
                 UINT8                                         ch,
                 CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00,
                 CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10,
                 CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0,
                 CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0,
                 CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0,
                 CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0,
                 CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0
                 )
{

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcSetupAddrGen\n"));

  //
  // CPGC Address Generation
  //

  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_REG, 4, seqBaseAddrStartLo0.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_REG, 4, seqBaseAddrStartHi0.Data);

  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_REG, 4, seqBaseAddrWrapLo0.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_REG, 4, seqBaseAddrWrapHi0.Data);

  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAINCCTL00_MC_MAIN_REG, 8, seqBaincCtl00.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAINCCTL10_MC_MAIN_REG, 8, seqBaincCtl10.Data);

  //
  // Note: For the MPR read, the row/column addresses does not matter.
  // When the DRAM device is in that mode, all reads return the MPR pattern - alternating 0 and 1 on DQ lines.
  // That is regardless of what row/column addresses of the read command.
  //
  // Disable the carry enable for column address wrap. As it is eanbled by default.
  // If enabled, Row address will get incremented when column wrap address is hit.
  //
  //
  // Note: A0 bug cryen is broken. Disable cryen for all cases
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAOCIC0_MC_MAIN_REG, 4, cpgcSeqBaoCic0.Data);

  return;
} // CpgcSetupAddrGen

static UINT32
CpgcInitializeTransientErrChecking (
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch
  )
/*++

Routine Description:

  Initialize DDR advanced command CPGC error status based err counters to eliminate transient errors.

Arguments:

  host                - Pointer to sysHost
  socket              - Socket to train
  ch                  - Channel ID

Returns:

  SUCCESS / FAILURE

Notes:

    0 = no error
    1 = error


--*/
{
  UINT8     strobe;
  UINT32    status;
  CPGC_ERR_COUNTER0_MCDDC_CTL_STRUCT  cpgcErrCounter0;

  status = SUCCESS;

  for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
    cpgcErrCounter0.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_COUNTER0_MCDDC_CTL_REG + (8 * strobe));

    // Count bytes
    cpgcErrCounter0.Bits.ctl  = 2;
    cpgcErrCounter0.Bits.pntr = 2 * strobe;

    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_COUNTER0_MCDDC_CTL_REG + (8 * strobe), cpgcErrCounter0.Data);
  } // for strobe

  return status;
} // CpgcInitializeTransientErrChecking

/**

  Global training CPGC setup

  @param host                - Pointer to sysHost
  @param socket              - socket to train
  @param ch                  - ddr channel to setup
  @param seqLoopCnt          - Iterations of the complete Sequence loop
  @param startDelay          - Delay period (in DClks) for start sequence
  @param AddrUpdRateMode     - address update rate mode
  @param rwMode              - subsequence type for subseq0:  0000 - base read, 0001 base write
  @param numCacheLines       - number of cachelines to be transmitted
  @param burstSeparation     - # of DCLK cycles between completion of the current sub-sequence and beginning
                               the next sub-sequence
  @param errEnChunk          - Bit mask to enable any checking on any specific chunk within the Cache Line.
  @param errEnCl             - Bit mask to enable what Cachelines are checked within each group of 8 received.
  @param stopOnErrCtl        - Defines what type of err is will Stop a test:
                                 00: Never Stop On Any Err
                                 01: Stop on the Nth Any Lane Err
                                 10: Stop on All Byte Groups Err
                                 11: Stop on All Lanes Err
  @param subseqEndPtr        - specifies end of subsequence
  @param useSubSeq1          - flag to indicate init of subseq1 needed (0 - not used, 1 - base read, 2 - base writes)
  @param cpgcLinkTrain       - set cpgc_link_train bit
  @param back2backWrEn       - enable back2back writes
  @param mprControl          - enable / disable MPR
  @param refMask             - self refresh mask
  @param zqMask              - zq mask

  @retval SUCCESS / FAILURE

**/
static UINT32
CpgcGlobalTrainingSetup (
                        PSYSHOST                    host,
                        UINT8                       socket,
                        UINT8                       ch,
                        CPGC_GLOBAL_TRAINING_SETUP  *cpgcGblTrainingSetup
                        )
{

  UINT32                                status;
  UINT8                               startDelay;
  UINT8                               addrUpdRateMode;
  UINT8                               seqLoopCnt;
  UINT8                               rwMode;
  UINT8                               numCacheLines;
  UINT8                               burstSeparation;
  UINT8                               errEnChunk;
  UINT8                               errEnCl;
  UINT8                               stopOnErrCtl;
  UINT8                               subseqEndPtr;
  UINT8                               useSubSeq1;
  UINT8                               refMask;
  UINT8                               zqMask;
  UINT8                               mcId;
  CPGC_SEQCTL0_MC_MAIN_STRUCT         cpgcSeqCtl0Reg;
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT    cpgcMiscCkeCtlReg;
  CPGC_MISCREFCTL_MCDDC_CTL_STRUCT    cpgcMiscRefCtlReg;
  CPGC_MISCZQCTL_MCDDC_CTL_STRUCT     cpgcMiscZQCtlReg;
  CPGC_MISCODTCTL_MCDDC_CTL_STRUCT    cpgcMiscOdtCtlReg;
  CPGC_SUBSEQ0_CTL0_MC_MAIN_STRUCT    cpgcSubseq0Ctl0Reg;
  CPGC_SUBSEQ1_CTL0_MC_MAIN_STRUCT    cpgcSubseq1Ctl0Reg;
  CPGC_ERR_CTL_MCDDC_CTL_STRUCT       cpgcErrCtlReg;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT      cpgcGlobalCtrlReg;

  mcId = GetMCID(host, socket, ch);

  status = SUCCESS;

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcGlobalTrainingSetup\n"));

  startDelay          = cpgcGblTrainingSetup->startDelay;
  addrUpdRateMode     = cpgcGblTrainingSetup->addrUpdRateMode;
  seqLoopCnt          = cpgcGblTrainingSetup->seqLoopCnt;
  rwMode              = cpgcGblTrainingSetup->rwMode;
  numCacheLines       = cpgcGblTrainingSetup->numCacheLines;
  burstSeparation     = cpgcGblTrainingSetup->burstSeparation;
  errEnChunk          = cpgcGblTrainingSetup->errEnChunk;
  errEnCl             = cpgcGblTrainingSetup->errEnCl;
  stopOnErrCtl        = cpgcGblTrainingSetup->stopOnErrCtl;
  subseqEndPtr        = cpgcGblTrainingSetup->subseqEndPtr;
  useSubSeq1          = cpgcGblTrainingSetup->useSubSeq1;
  refMask             = cpgcGblTrainingSetup->refMask;
  zqMask              = cpgcGblTrainingSetup->zqMask;

  //
  // program scheduler (based on ch)
  //

  //
  // MC Controller Init - disable miscellaneous events
  // functional CKE logic, do not disable CKE powerdown,
  // disable refreshes for all ranks
  // disable ZQCal for all ranks
  //
  cpgcMiscCkeCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG);
  cpgcMiscRefCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG);
  cpgcMiscZQCtlReg.Data  = MemReadPciCfgEp (host, socket, ch, CPGC_MISCZQCTL_MCDDC_CTL_REG);
  cpgcMiscOdtCtlReg.Data  = MemReadPciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG);

  cpgcMiscCkeCtlReg.Bits.cke_override     = 0;
  cpgcMiscRefCtlReg.Bits.ref_rankmask     = refMask;
  cpgcMiscZQCtlReg.Bits.zq_rankmask       = zqMask;
  cpgcMiscOdtCtlReg.Bits.odt_override     = 0;
  cpgcMiscOdtCtlReg.Bits.mpr_train_ddr_on = 0;  //SKX change

  MemWritePciCfgEp (host, socket, ch, CPGC_MISCCKECTL_MCDDC_CTL_REG, cpgcMiscCkeCtlReg.Data);
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCREFCTL_MCDDC_CTL_REG, cpgcMiscRefCtlReg.Data);
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCZQCTL_MCDDC_CTL_REG, cpgcMiscZQCtlReg.Data);
  MemWritePciCfgEp (host, socket, ch, CPGC_MISCODTCTL_MCDDC_CTL_REG, cpgcMiscOdtCtlReg.Data);

  //
  // Write CPGC Registers
  //

  //
  // enables this sequencer to be controlled by global controls
  // REUT loopback mode, enter loopback.marker state
  // delay the start sequence, !! make training pref
  //

  cpgcSeqCtl0Reg.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  cpgcSeqCtl0Reg.Bits.global_control            = 1;
  cpgcSeqCtl0Reg.Bits.initialization_mode       = CPGC_Testing_Mode; // Test Mode
  cpgcSeqCtl0Reg.Bits.loopcount                 = seqLoopCnt;
  cpgcSeqCtl0Reg.Bits.address_update_rate_mode  = addrUpdRateMode;
  cpgcSeqCtl0Reg.Bits.subseq_start_pointer      = 0;
  cpgcSeqCtl0Reg.Bits.subseq_end_pointer        = subseqEndPtr;
  cpgcSeqCtl0Reg.Bits.start_test_delay          = startDelay;
  cpgcSeqCtl0Reg.Bits.link_wrtrn_enb            = 0;

  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, cpgcSeqCtl0Reg.Data);

  //
  // Subsequence scheduler Control:
  // exponential num cacheline scale
  // set base read /write mode
  //

  cpgcSubseq0Ctl0Reg.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50);
  cpgcSubseq0Ctl0Reg.Bits.subseq_type           = rwMode;
  cpgcSubseq0Ctl0Reg.Bits.num_cachelines        = numCacheLines;
  cpgcSubseq0Ctl0Reg.Bits.num_cachelines_scale  = 0;
  cpgcSubseq0Ctl0Reg.Bits.subseq_wait           = burstSeparation;
  cpgcSubseq0Ctl0Reg.Bits.rstcurr_ba_start      = 1; //reset address for next test
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50, cpgcSubseq0Ctl0Reg.Data);

  if (useSubSeq1 != CPGC_NO_SUBSEQ1) {
    cpgcSubseq1Ctl0Reg.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG, 0x50);
    cpgcSubseq1Ctl0Reg.Bits.subseq_type           = useSubSeq1;
    cpgcSubseq1Ctl0Reg.Bits.num_cachelines        = numCacheLines;
    cpgcSubseq1Ctl0Reg.Bits.num_cachelines_scale  = 0;
    cpgcSubseq1Ctl0Reg.Bits.subseq_wait           = burstSeparation;
    cpgcSubseq1Ctl0Reg.Bits.rstcurr_ba_start      = 1; //reset address for next test
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ1_CTL0_MC_MAIN_REG, 0x50, cpgcSubseq1Ctl0Reg.Data);
  }

  //
  // Error Control Registers
  //  stop on nth error (must be 1)
  //  do not stop on error
  //  configure monitor errors on all cachelines
  //  do not monitor errors on all chunks
  //
  cpgcErrCtlReg.Data = 0;
  cpgcErrCtlReg.Bits.stop_on_ntherr           = 1;
  cpgcErrCtlReg.Bits.stop_on_errcontrol       = 0;
  cpgcErrCtlReg.Bits.selective_err_enb_cl     = errEnCl;
  cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
  if (cpgcGlobalCtrlReg.Bits.use_bl4_col_addr) {
    //
    // Mask off the odd chunks if in burst length 4 mode
    //
    cpgcErrCtlReg.Bits.selective_err_enb_chunk  = errEnChunk & 3;
  } else {
    cpgcErrCtlReg.Bits.selective_err_enb_chunk  = errEnChunk;
  }
  cpgcErrCtlReg.Bits.stop_on_errcontrol       = stopOnErrCtl;
  cpgcErrCtlReg.Bits.cpgc_cntr_x8_mode        = cpgcGblTrainingSetup->x8Mode;
  MemWritePciCfgEp (host, socket, ch, CPGC_ERR_CTL_MCDDC_CTL_REG, cpgcErrCtlReg.Data);

  // This feature not enabled yet
  //CpgcInitializeTransientErrChecking (host, socket, ch);

  return status;
} // CpgcGlobalTrainingSetup

static UINT32
CpgcCheckTransientErrs (
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  CPGC_ERROR_STATUS *cpgcErrorStatus
  )
/*++

Routine Description:

  Adjust DDR advanced command CPGC error status based err counters to eliminate transient errors.

Arguments:

  host                - Pointer to sysHost
  socket              - Socket to train
  ch                  - Channel id
  cpgcErrorStatus     - error status for channel

Returns:

  SUCCESS / FAILURE

Notes:

    0 = no error
    1 = error


--*/
{
  UINT8     strobe;
  UINT32    status;
  CPGC_ERR_COUNTER0_S_MCDDC_CTL_STRUCT  cpgcErrCounter0_s;

  status = SUCCESS;

  for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
    cpgcErrCounter0_s.Data = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_COUNTER0_S_MCDDC_CTL_REG + (8 * strobe));

    // if count less than threahold then ignore errors
    if ((cpgcErrCounter0_s.Bits.status > 0) && (cpgcErrCounter0_s.Bits.status <= CPGC_ERR_THRESHOLD)) {
      status = FAILURE;
      // Count transient errors
      host->var.mem.socket[socket].channelList[ch].transientErrs++;

      // Clear errors for corresponding byte strobe
      if (strobe < (MAX_STROBE / 2) - 1) {
        // Data Bytes
        *(((UINT8 *)cpgcErrorStatus) + (strobe))     &= 0x0;
        *(((UINT8 *)cpgcErrorStatus) + (strobe + 8)) &= 0x0;
      } else {
        // ECC Bytes
        *(((UINT8 *)cpgcErrorStatus) + (strobe + 8)) &= 0x0;
        *(((UINT8 *)cpgcErrorStatus) + (strobe + 9)) &= 0x0;
      }
    }
  }
  return status;
} // CpgcCheckTransientErrs

/**

  Collect DDR advanced command CPGC error status

  @param host                - Pointer to sysHost
  @param socket              - Socket to train
  @param ddrChEnMap          - 4-bit bit mask of ddr channels to test - (1 = active channel)
  @param cpgcErrorStatus     - error status for each channel

  @retval SUCCESS / FAILURE

Notes:

    0 = no error
    1 = error

**/
UINT32
CpgcAdvTrainingErrorStatus (
                           PSYSHOST          host,
                           UINT8             socket,
                           UINT32            ddrChEnMap,
                           CPGC_ERROR_STATUS cpgcErrorStatus[MAX_CH]
                           )
{

  UINT32 status;
  UINT8 ch;

  status = SUCCESS;

  for (ch = 0; ch < MAX_CH; ch++) {

    // skip inactive channels
    if (!(ddrChEnMap & (1 << ch))) continue;

    cpgcErrorStatus[ch].cpgcErrDat0S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA0_S_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrDat1S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA1_S_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrDat2S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA2_S_MCDDC_CTL_REG);
    cpgcErrorStatus[ch].cpgcErrDat3S = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_DATA3_S_MCDDC_CTL_REG);
    if (host->nvram.mem.eccEn) {
      cpgcErrorStatus[ch].cpgcErrEccS  = MemReadPciCfgEp (host, socket, ch, CPGC_ERR_ECC_S_MCDDC_CTL_REG);
    } else {
      cpgcErrorStatus[ch].cpgcErrEccS  = 0;
    }

    MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "CpgcAdvTrainingErrorStatus: cpgcErr0 = 0x%x, cpgcErr1 = 0x%x, cpgcErr2 = 0x%x, cpgcErr3 = 0x%x, cpgcErrEccS = 0x%x\n",
                   cpgcErrorStatus[ch].cpgcErrDat0S,
                   cpgcErrorStatus[ch].cpgcErrDat1S,
                   cpgcErrorStatus[ch].cpgcErrDat2S,
                   cpgcErrorStatus[ch].cpgcErrDat3S,
                   cpgcErrorStatus[ch].cpgcErrEccS));

    // this features is broken for now
    // if (CpgcCheckTransientErrs (host, socket, ch, cpgcErrorStatus)) {
      // // If transient error found print updated error status
      // MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   // "CpgcAdvTrainingErrorStatus (Post Transient Check): cpgcErr0 = 0x%x, cpgcErr1 = 0x%x, cpgcErr2 = 0x%x, cpgcErr3 = 0x%x, cpgcErrEccS = 0x%x\n",
                   // cpgcErrorStatus[ch].cpgcErrDat0S,
                   // cpgcErrorStatus[ch].cpgcErrDat1S,
                   // cpgcErrorStatus[ch].cpgcErrDat2S,
                   // cpgcErrorStatus[ch].cpgcErrDat3S,
                   // cpgcErrorStatus[ch].cpgcErrEccS));
    // }
  } // for

  return status;

} // CpgcAdvTrainingErrorStatus

#ifndef ADV_MEMTEST_SUPPORT
/**

  DDR Mem test / Mem Init CPGC setup for logical rank.

  @param host              - Pointer to sysHost
  @param socket            - Memory controller to train
  @param ddrChEnMap        - ddr channels to setup
  @param logRank           - logical rank to setup
  @param colAddressBits    - number of DRAM column address bits to test
  @param rowAddressBits    - number of DRAM row address bits to test
  @param bankAddressBits   - number of DRAM bank address bits to test

   @retval n/a

**/
static void
CpgcMemTestRankSetup (
                     PSYSHOST  host,
                     UINT8     socket,
                     UINT32    ddrChEnMap,
                     UINT8     logRank,
                     UINT8     logSubRank,
                     UINT8     colAddressBits[MAX_CH],
                     UINT8     rowAddressBits[MAX_CH],
                     UINT8     bankAddressBits[MAX_CH]
                     )
{
  UINT8                                         ch;
  UINT8                                         cpgcColAddressBits;
  UINT8                                         cpgcRowAddressBits;
  UINT8                                         cpgcBankAddressBits;
  UINT8                                         numCacheLines;
  UINT8                                         mcId;
  struct channelNvram                           (*channelNvList)[MAX_CH];
  CPGC_SUBSEQ0_CTL0_MC_MAIN_STRUCT              cpgcSubseq0Ctl0Reg;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;
  CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_STRUCT          seqDmyRdAddr;
  CPGC_SUBSEQ0_OFFSETCTL0_MC_MAIN_STRUCT        subseq0_offsetctl0;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT                cpgcGlobalCtrlReg;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT               ddrtMiscDelay;
  channelNvList = GetChannelNvList(host, socket);

  MemDebugPrint((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, logRank, NO_STROBE, NO_BIT,
                 "CpgcMemTestRankSetup: ddrChEnMap = 0x%x\n", ddrChEnMap));

  //
  // CPGC setup
  //

  for (ch = 0; ch < MAX_CH; ch++) {
    mcId = GetMCID(host, socket, ch);
    //
    // skip ddr channels not selected or disabled ddr channels
    // Must disable global start so start test does not activate cpgc on disabled channel for this rank
    //
    if (!(ddrChEnMap & (1 << ch)) ||
        (host->nvram.mem.socket[socket].channelList[ch].enabled == 0) ||
        (((*channelNvList)[ch].encodedCSMode == 0) && (logSubRank != 0))) {
        CpgcDisableGlobalStart (host, socket, ch);
        continue;
    }

    MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, logRank, NO_STROBE, NO_BIT,
                  "CpgcMemTestRankSetup: colAddr = %d, rowAddr = %d, bankAddr = %d\n",
                  colAddressBits[ch], rowAddressBits[ch], bankAddressBits[ch]));

    //
    // Cpgc handles lower 3 bits of column
    //
    //
    // discover burst length setting
    //
    cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
    //
    // if Burst Length 4 do - 2 column bits, else -3
    //
    if (cpgcGlobalCtrlReg.Bits.use_bl4_col_addr == 1) {
      cpgcColAddressBits = colAddressBits[ch] - 2;
    } else {
      cpgcColAddressBits = colAddressBits[ch] - 3;
    }
    cpgcRowAddressBits = rowAddressBits[ch];
    cpgcBankAddressBits = bankAddressBits[ch];

    //
    // Adjust number of cachelines for size of rank (use 2^n) scale
    //
    numCacheLines = cpgcColAddressBits + cpgcRowAddressBits + cpgcBankAddressBits;

    cpgcSubseq0Ctl0Reg.Data = MemReadPciCfgMC (host, socket, mcId, GetCpgcRegOffset (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG));
    cpgcSubseq0Ctl0Reg.Bits.num_cachelines = numCacheLines;
    cpgcSubseq0Ctl0Reg.Bits.rstcurr_ba_start = 1;

    cpgcSubseq0Ctl0Reg.Bits.num_cachelines_scale = 0;
    MemWritePciCfgMC (host, socket, mcId, GetCpgcRegOffset (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG), cpgcSubseq0Ctl0Reg.Data);

    // Wrap at end of col, row, bank
    seqBaseAddrStartLo0.Data            = 0;
    seqBaseAddrStartHi0.Data            = 0;
//    seqBaseAddrStartHi0.Bits.rank_addr  = rank;
    seqBaseAddrStartHi0.Bits.rank_addr  = 0;

    seqBaseAddrWrapLo0.Data             = 0;
    seqBaseAddrWrapLo0.Bits.column_addr = (1 << cpgcColAddressBits) - 1;
    seqBaseAddrWrapLo0.Bits.bank_addr   = (1 << cpgcBankAddressBits) - 1;

    seqBaseAddrWrapHi0.Data           = 0;
    seqBaseAddrWrapHi0.Bits.row_addr  = (1 << cpgcRowAddressBits) - 1;
//    seqBaseAddrWrapHi0.Bits.rank_addr = rank;
    seqBaseAddrWrapHi0.Bits.rank_addr = 0;

//TODO: Check to see if there are correct define names for these BITx usages
    // Programs GPGC to run on the selected physical ranks
    // RankFeatureEnable is a bit mask that can enable CKE, Refresh or ZQ
    // RankFeatureEnable[0] enables Refresh on all non-selected ranks
    // RankFeatureEnable[1] enables Refresh on all ranks
    // RankFeatureEnable[2] enables ZQ on all non-selected ranks
    // RankFeatureEnable[3] enables ZQ on all ranks
    // RankFeatureEnable[4] enables CKE on all non-selected ranks
    // RankFeatureEnable[5] enables CKE on all ranks
    SelectCPGCRanks(host, socket, ch, 1 << logRank, BIT3 | BIT1, logSubRank);
    ddrtMiscDelay.Data  = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
    ddrtMiscDelay.Bits.force_ddr4_mode = 0;
    ddrtMiscDelay.Bits.force_ddrt_mode = 0;
    MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);
    //
    // Sequence BAIN control
    //
    seqBaincCtl00.Data = 0;
    seqBaincCtl00.Bits.column_ba_inc      = 1;
    seqBaincCtl00.Bits.column_ba_updrate  = 0;
    seqBaincCtl00.Bits.column_ba_updscale = 0;
    seqBaincCtl00.Bits.row_ba_inc         = 1;
    seqBaincCtl00.Bits.row_ba_updrate     = cpgcColAddressBits;
    seqBaincCtl00.Bits.row_ba_updscale    = 0;

    seqBaincCtl10.Data = 0;
    seqBaincCtl10.Bits.bank_ba_inc        = 1;
    seqBaincCtl10.Bits.bank_ba_updrate    = cpgcColAddressBits + cpgcRowAddressBits;
//    seqBaincCtl10.Bits.bank_ba_updscale   = 1;
    seqBaincCtl10.Bits.bank_ba_updscale   = 0;
    seqBaincCtl10.Bits.rank_ba_inc        = 0;
    seqBaincCtl10.Bits.rank_ba_updrate    = 0;
    seqBaincCtl10.Bits.rank_ba_updscale   = 0;
    seqBaincCtl10.Bits.column_ba_inc_msb  = 0;

    cpgcSeqBaoCic0.Data = 0;

    CpgcSetupAddrGen (host, socket, ch,
                      seqBaincCtl00,
                      seqBaincCtl10,
                      seqBaseAddrStartLo0,
                      seqBaseAddrStartHi0,
                      seqBaseAddrWrapLo0,
                      seqBaseAddrWrapHi0,
                      cpgcSeqBaoCic0
                     );

    // GPGC traffic only uses BA[1:0] - Mask BANK that will not be used
    seqDmyRdAddr.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG);
    seqDmyRdAddr.Bits.bank_mask0 = 0xffff;
    MemWritePciCfgMC (host, socket, mcId, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG, seqDmyRdAddr.Data);

    subseq0_offsetctl0.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_SUBSEQ0_OFFSETCTL0_MC_MAIN_REG);
    subseq0_offsetctl0.Bits.row_inc_enb = 1;
    subseq0_offsetctl0.Bits.col_inc_enb = 0;
    subseq0_offsetctl0.Bits.col_inc_order = 1;

    subseq0_offsetctl0.Bits.base_subseq_rptrate = 2;
    subseq0_offsetctl0.Bits.offadd_updrate = 2;
    MemWritePciCfgMC (host, socket, mcId, CPGC_SUBSEQ0_OFFSETCTL0_MC_MAIN_REG, subseq0_offsetctl0.Data);

  } // ch

  return;
} // CpgcMemTestRankSetup
#endif // !ADV_MEMTEST_SUPPORT

/**

  CPGC Clear Global Error Status

  @param host              - Pointer to sysHost
  @param socket            - Memory controller to train

  @retval SUCCESS

**/
static UINT32
CpgcClearGlobalErrorStatus (
                           PSYSHOST  host,
                           UINT8     socket
                           )
{
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT cpgcGlobalCtrlReg;

  cpgcGlobalCtrlReg.Data = MemReadPciCfgMain (host, socket, CPGC_GLOBALCTRL_MC_MAIN_REG);
  cpgcGlobalCtrlReg.Bits.clear_errs = 1;
  MemWritePciCfgMain (host, socket, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlobalCtrlReg.Data);

  return SUCCESS;
}

/**

  Loop based on PatternLength

  @param PatternLength - Number of loops

  @retval dqLoopCount

**/
UINT8
PatternLengthToCpgcLoopCnt(
                          UINT32 PatternLength
                          )
{
  UINT8 dqLoopCount;
  UINT32 j;
  int i;

  // Convert repeat into 2^(n - 1) - Round Up
  // PatternLength = 0 1 2 3 4 5 6 7 8 9  10 11 12 13 14 15 16 17 ...
  // RoundUp       = 1 1 2 4 4 8 8 8 8 16 16 16 16 16 16 16 16 32 ...
  // 2^n where n   = 0 0 1 2 2 3 3 3 3 4  4  4  4  4  4  4  4  5
  i = -1;
  j = 0;

  // repeat = 1, i=0 j=1+1=2, 1>=2 ?;
  // repeat = 2, i=0 j=1+1=2, 2>=2 ?; i=1 j=1+2=3, 2>=3 ?;
  // repeat = 3, i=0 j=1+1=2, 3>=2 ?; i=1 j=1+2=3, 3>=3 ?; i=2 j=5, 3>=5?
  // repeat = 4, i=0 j=1+1=2, 4>=2 ?; i=1 j=1+2=3, 4>=3 ?; i=2 j=5, 4>=5?
  // repeat = [7-5], i=0 j=1+1=2, [7-5]>=2 ?; i=1 j=1+2=3, [7-5]>=3 ?; i=2 j=5, [7-5]>=5?; i=3 j=9, [7-5]>=9?
  // repeat = 8, i=0 j=1+1=2, 8>=2 ?; i=1 j=1+2=3, 8>=3 ?; i=2 j=5, 8>=5?; i=3 j=9, 8>=9?
  // repeat = [15-9], i=0 j=1+1=2, [15-9]>=2 ?; i=1 j=1+2=3, [15-9]>=3 ?; i=2 j=5, [15-9]>=5?; i=3 j=9, [15-9]>=9; i=4 j=17, [15-9]>=17;
  // repeat = 16, i=0 j=1+1=2, 16>=2 ?; i=1 j=1+2=3, 16>=3 ?; i=2 j=5, 16>=5?; i=3 j=9, 16>=9; i=4 j=17, 16>=17;
  while (PatternLength >= j) {
    i++;
    j = 1 + (1 << i);               // This is equivalent to (1 + 2^i)
  }

  dqLoopCount = (UINT8)(i + 1);     // ~2^(n+1) ==> passed down to offset 2^(n-1)
  return dqLoopCount;
}

/**

  Set target victim bit for dq training in cpgc WDB

  @param host             - pointer to sysHost
  @param socket           - socket to train
  @param ddrCh            - ddr channel mask
  @param wdbVictimBit     - victim bit in wdb setup (0-7)

  @retval SUCCESS / FAILURE

**/
static UINT32
CpgcSetWdbVictimBit (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT32    ddrChEnMap,
                    UINT8     wdbVictimBit
                    )
{
  UINT32 status;
  UINT8 ch;
  CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT   cpgcPatWdbClCtl;

  status = SUCCESS;

  MemDebugPrint((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcSetWdbVictimBit: ddrChEnMap = 0x%x, victim bit = %d\n", ddrChEnMap, wdbVictimBit));

  //
  // Program CPGC WDB mux control on each channel
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    // skip unused ddr channels
    if (!(ddrChEnMap & (1 << ch))) continue;

    //
    // Program CPGC WDB Pointer Control to specified victim bit
    //
    cpgcPatWdbClCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATWDBCLCTL_MCDDC_CTL_REG);
    cpgcPatWdbClCtl.Bits.start_pntr = wdbVictimBit;
    cpgcPatWdbClCtl.Bits.end_pntr = wdbVictimBit;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCLCTL_MCDDC_CTL_REG, cpgcPatWdbClCtl.Data);
  } // ch loop

  return status;

} // CpgcSetWdbVictimBit

/**

  DDR advanced command CPGC execution

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ddrChEnMap  - 4-bit bit mask of ddr channels to test - (1 = channel is selected to run tests)
  @param pattern     - ddr pattern (0-3) ( 0 = dq, 1 = cmd group 1, 2 = cmd group 2, 3 = cmd group 3)

  @retval SUCCESS / FAILURE

**/
UINT32
CpgcAdvTrainingExecTest (
                        PSYSHOST  host,
                        UINT8     socket,
                        UINT32    ddrChEnMap,
                        UINT16    pattern
                        )
{

  UINT32 status;

  MemDebugPrint((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcAdvTrainingExecuteTest: ChEnMap = 0x%x, pattern = %d\n", ddrChEnMap, pattern));

  //
  // Notes:
  // Test are run in parallel on channels indicated by channel mask
  // In 1:1 lockstep only run tests on primary lockstep channels (ddr channels 0, 2)
  //

  status = SUCCESS;

  //
  // Start test
  //
  CpgcGlobalTestStart (host, socket, ddrChEnMap);

  //
  // Poll test completion
  //
  status = CpgcPollGlobalTestDone (host, socket, ddrChEnMap);

  return status;

} // CpgcAdvTrainingExecTest

UINT32
CpgcAdvTrainingExecTestGroup (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    ddrChEnMap,
  UINT8     pattern,
  GSM_GT    group
  )
/*++

  DDR advanced command CPGC execution

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ddrChEnMap  - 4-bit bit mask of ddr channels to test - (1 = channel is selected to run tests)
  @param pattern     - ddr pattern (0-3) ( 0 = dq, 1 = cmd group 1, 2 = cmd group 2, 3 = cmd group 3)

  @retval SUCCESS
  @retval FAILURE

--*/
{
 return CpgcAdvTrainingExecTest (host, socket, ddrChEnMap, 0);
} // CpgcAdvTrainingExecTestGroup

/**

  Polls for cpgc test done on ddr channels indicated with mask

  @param host                        - Pointer to sysHost
  @param socket                        - socket to train
  @param ddrChMask                   - ddr channel mask (1 = active channel)

  @retval SUCCESS / FAILURE


**/
UINT32
CpgcPollGlobalTestDone (
                       PSYSHOST  host,
                       UINT8     socket,
                       UINT32    ddrChMask
                       )

{
  UINT8                           ch;
  UINT8                           loopTestDoneStatus;
  UINT8                           mcDone;
  UINT8                           mcId;
  UINT8                           numChPerMC;
  UINT8                           status;
  UINT32                          startCount;
  UINT32                          timeOut;
  CPGC_GLOBALERR_MC_MAIN_STRUCT   cpgcGlobalErrReg;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT  cpgcGlobalCtrlReg;

  status = SUCCESS;
  mcDone = 0;

  if (host->var.mem.socket[socket].testTimeoutSize) {
    timeOut = 100000;
  } else {
    timeOut = 15000000;
  }

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!(ddrChMask & (1 << ch))) continue;

    mcId = GetMCID(host, socket, ch);
    numChPerMC = host->var.mem.numChPerMC;

    if (mcDone & (1 << mcId)) continue;

    startCount = GetCount(host);
    do {
      cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
      //
      // Timeout after a certain amount of time, and return an error.
      //
      if(GetDelay(host, startCount) > timeOut) {
        status = FAILURE;
        break;
      }
    } while (cpgcGlobalCtrlReg.Bits.clear_errs || cpgcGlobalCtrlReg.Bits.start_test);


    loopTestDoneStatus = 0;

    startCount = GetCount(host);

    //
    // Exit if test done on active channels
    //
    while (loopTestDoneStatus == 0) {

      loopTestDoneStatus = 1;

      //SKX change
      cpgcGlobalErrReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALERR_MC_MAIN_REG);

      if ((numChPerMC > 0) && (ch % numChPerMC == 0)) {
        loopTestDoneStatus = (UINT8)cpgcGlobalErrReg.Bits.channel_test_done_status_0;
      }
      if ((numChPerMC > 1) && (ch % numChPerMC == 1)) {
        loopTestDoneStatus &= cpgcGlobalErrReg.Bits.channel_test_done_status_1;
      }
      if ((numChPerMC > 2) && (ch % numChPerMC == 2)) {
        loopTestDoneStatus &= cpgcGlobalErrReg.Bits.channel_test_done_status_2;
      }

      //
      // Inidicate if this MC is done
      //
      if (loopTestDoneStatus == 1) {
        mcDone |= (1 << mcId);
      }

      //
      // Timeout after a certain amount of time, and return an error.
      //
      if(GetDelay(host, startCount) > timeOut) {
        status = FAILURE;
        break;
      }
    } // while


  } // ch loop

  MemDebugPrint ((host, SDBG_CPGC, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "CpgcPollGlobalTestDone: Exit -- Return "));

  if (status) {
    MemDebugPrint ((host, SDBG_CPGC, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "FAILURE\n"));
  } else {
    MemDebugPrint ((host, SDBG_CPGC, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "SUCCESS\n"));
  }

  return status;
} // CpgcPollGlobalTestDone

/**

  Disable global start control

  @param host                - Pointer to sysHost
  @param socket              - socket number
  @param ch                  - ddr channel number

  @retval n/a

**/
static void
CpgcDisableGlobalStart (
                       PSYSHOST  host,
                       UINT8     socket,
                       UINT8     ch
                       )
{
  CPGC_SEQCTL0_MC_MAIN_STRUCT   cpgcSeqCtl0Reg;

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcDisableGlobalStart\n"));

  cpgcSeqCtl0Reg.Data = MemReadPciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4);
  cpgcSeqCtl0Reg.Bits.global_control = 0;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, cpgcSeqCtl0Reg.Data);

  return;

} // CpgcDisableGlobalStart

/**

    CPGC Global Test Start

    @param host       - Pointer to sysHost
    @param socket     - socket number
    @param ddrChEnMap - bit mask of channels to use

    @retval SUCCESS

**/
UINT32
CpgcGlobalTestStart (
                    PSYSHOST  host,
                    UINT8     socket,
                    UINT32    ddrChEnMap
                    )
{
  UINT8                             ch;
  UINT8                             mcDone;
  UINT8                             mcId;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT    cpgcGlobalCtrlReg;

  mcDone = 0;

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!(ddrChEnMap & (1 << ch))) continue;

    mcId = GetMCID(host, socket, ch);

    if (mcDone & (1 << mcId)) continue;

    cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
    cpgcGlobalCtrlReg.Bits.start_test = 1;
    // clear errors
    cpgcGlobalCtrlReg.Bits.clear_errs = 1;
    MemWritePciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlobalCtrlReg.Data);

    //
    // Indicate the test has been started on this MC already
    //
    mcDone |= (1 << mcId);
  } // ch loop

  return SUCCESS;
}

/**

    CPGC Global Test Stop

    @param host       - Pointer to sysHost
    @param socket     - socket number
    @param ddrChEnMap - bit mask of channels to use

    @retval SUCCESS

**/
UINT32
CpgcGlobalTestStop (
                   PSYSHOST  host,
                   UINT8     socket,
                   UINT32    ddrChEnMap
                   )
{
  UINT8                             ch;
  UINT8                             mcDone;
  UINT8                             mcId;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT    cpgcGlobalCtrlReg;

  mcDone = 0;

  for (ch = 0; ch < MAX_CH; ch++) {
    if (!(ddrChEnMap & (1 << ch))) continue;

    mcId = GetMCID(host, socket, ch);

    if (mcDone & (1 << mcId)) continue;

    cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
    cpgcGlobalCtrlReg.Bits.stop_test = 1;
    // clear errors
    //cpgcGlobalCtrlReg.Bits.clear_errs = 1;
    MemWritePciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlobalCtrlReg.Data);
    //
    // Indicate the test has been started on this MC already
    //
    mcDone |= (1 << mcId);
  } // ch loop

  return SUCCESS;
}

/**

  DDR loopback control training

  @param host    - Pointer to sysHost
  @param socket  - Socket to train
  @param ch      - DDR channel to setup
  @param rank    - Logical rank number

  @retval SUCCESS / FAILURE

**/
UINT32
CpgcCtlClkPattern (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  GSM_CSN   signal
  )

{
  UINT32                                        status;
  CPGC_GLOBAL_TRAINING_SETUP                    cpgcGblTrainingSetup;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
  CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT           patCadbMuxCtl;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  struct channelNvram                           (*channelNvList)[MAX_CH];
  PatCadbProg0 CtlClkPattern0Buf[CADB_LINES];
  PatCadbProg1 CtlClkPattern1Buf[CADB_LINES];
  UINT16 cs;
  UINT32 i;
  UINT32   address;
  UINT8 bank, cid, cmd, par;


  MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "CpgcAdvCtlPattern\n"));
  channelNvList = GetChannelNvList(host, socket);


  status = SUCCESS;

  cpgcGblTrainingSetup.seqLoopCnt        = 1;                                // test will end after one loop through the sub-sequence
  cpgcGblTrainingSetup.startDelay        = 0;                                // Delay start
  cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;                // Update based on the # of read and write cacheline
  cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0
  cpgcGblTrainingSetup.numCacheLines     = 2;                                // Number of Cachelines (2^N)
  cpgcGblTrainingSetup.burstSeparation   = 0;                                // Burst separation based on training step
  cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_NO_CHUNK;        // Do not monitor errors on chunks
  cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_NO_CL;           // Do not monitor errors on cacheline
  cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_NEVER_STOP_ON_ERR;       // Do not Stop on all lane error
  cpgcGblTrainingSetup.subseqEndPtr      = 1;                                // subseq end pointer
  cpgcGblTrainingSetup.useSubSeq1        = CPGC_BASE_READ_SUBSEQ;            // use sub seq1 (base read)
  cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
  cpgcGblTrainingSetup.back2backWrEn     = 0;                                // enable b2b writes
  cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
  cpgcGblTrainingSetup.refMask           = 0xFF;                             // disable cpgc self refresh
  cpgcGblTrainingSetup.zqMask            = 0xFF;                             // disable cpgc zq
  cpgcGblTrainingSetup.x8Mode            = 0;
  CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

  //
  // WDB programming
  //
  seqBaseAddrStartLo0.Data            = 0;
  seqBaseAddrStartHi0.Data            = 0;
  seqBaseAddrStartHi0.Bits.rank_addr  = 0;

  seqBaseAddrWrapLo0.Data             = 0;
  seqBaseAddrWrapLo0.Bits.column_addr = 0x1FF;
  seqBaseAddrWrapLo0.Bits.bank_addr   = 0x7;

  seqBaseAddrWrapHi0.Data           = 0;
  seqBaseAddrWrapHi0.Bits.row_addr  = 0x1000;
  seqBaseAddrWrapHi0.Bits.rank_addr = 0;

  //

  //
  // Sequence Bank Address Increment control
  //
  seqBaincCtl00.Data = 0;
  seqBaincCtl00.Bits.column_ba_inc      = 0;
  seqBaincCtl00.Bits.column_ba_updrate  = 0;
  seqBaincCtl00.Bits.column_ba_updscale = 0;
  seqBaincCtl00.Bits.row_ba_inc         = 12;
  seqBaincCtl00.Bits.row_ba_updrate     = 0;
  seqBaincCtl00.Bits.row_ba_updscale    = 1;

  seqBaincCtl10.Data = 0;
  seqBaincCtl10.Bits.bank_ba_inc        = 4;
  seqBaincCtl10.Bits.bank_ba_updrate    = 0;
  seqBaincCtl10.Bits.bank_ba_updscale   = 1;
  seqBaincCtl10.Bits.rank_ba_inc        = 0;
  seqBaincCtl10.Bits.rank_ba_updrate    = 0;
  seqBaincCtl10.Bits.rank_ba_updscale   = 0;
  seqBaincCtl10.Bits.column_ba_inc_msb  = 0;

  //
  // Set wrap order
  //
  cpgcSeqBaoCic0.Data = 0;

  CpgcSetupWdb (host, socket, ch,
                   0,                             // wdbIncrRate
                   0,                             // wdbIncrScale
                   0,                             // wdbStartPntr
                   3,                             // wdbEndPntr
                   1,                             // setupPbwr
                   CPGC_WDB_MUX_CTL_LFSR24,       // mux = pattern buffer
                   seqBaincCtl00,
                   seqBaincCtl10,
                   seqBaseAddrStartLo0,
                   seqBaseAddrStartHi0,
                   seqBaseAddrWrapLo0,
                   seqBaseAddrWrapHi0,
                   cpgcSeqBaoCic0
                  );


  //
  // Program CADB entries
  //
  // Fixup CA pattern based on signal
  address = 0;
  bank = 0;
  cid = 0;
  cmd = 0x3;  // DDR3 ACT encoding causes address[16:14] to be used
  cs = 0x3FF;
  par = 0;
  switch(signal) {
    case CS2_N:
    case CS3_N:
    case CS6_N:
    case CS7_N:
      cs = (1 << (signal - CS0_N));
      break;
    case C0:
    case C1:
    case C2:
      cid = (1 << (signal - C0));
      break;
    case A0:
      address = BIT0;
      break;
    case A1:
      address = BIT1;
      break;
    case A2:
      address = BIT2;
      break;
    case A3:
      address = BIT3;
      break;
    case A4:
      address = BIT4;
      break;
    case A5:
      address = BIT5;
      break;
    case A6:
      address = BIT6;
      break;
    case A7:
      address = BIT7;
      break;
    case A8:
      address = BIT8;
      break;
    case A9:
      address = BIT9;
      break;
    case A10:
      address = BIT10;
      break;
    case A11:
      address = BIT11;
      break;
    case A12:
      address = BIT12;
      break;
    case A13:
      address = BIT13;
      break;
    case A14:
      address = BIT14;
      break;
    case A15:
      address = BIT15;
      break;
    case A16:
      address = BIT16;
      break;
    case A17:
      //SKX TODO for 16Gbit density
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_49);
    case BA0:
      bank = BIT0;
      break;
    case BA1:
      bank = BIT1;
      break;
    case BG0:
      bank = BIT2;
      break;
    case BG1:
      bank = BIT3;
      break;
    case PAR:
      par = 1;
      break;
    case ACT_N:
      cmd = BIT3;
      break;
    default:
      break;
  }
  for (i = 0; i < CADB_LINES; i++) {
    CtlClkPattern0Buf[i] = CtlClkPattern0[i];
    CtlClkPattern1Buf[i] = CtlClkPattern1[i];
    if ((i & 1) == 0) {
      CtlClkPattern0Buf[i].address = address;
      CtlClkPattern0Buf[i].bank = bank;
      CtlClkPattern0Buf[i].cid = cid;
      CtlClkPattern1Buf[i].cs = cs;
      CtlClkPattern1Buf[i].par = par;
      CtlClkPattern1Buf[i].control = cmd;
    }
  }

  ProgramCADB (host, socket, ch, 0, 2, CtlClkPattern0Buf, CtlClkPattern1Buf);

  //
  // Enable CADB
  //
  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 1;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  //
  // Program CADB pattern buffer
  //
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, 0x5555);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, 0x0000);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, 0x0000);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUX3PB_MCDDC_CTL_REG, 0x0000);

  (*channelNvList)[ch].cadbMuxPattern[0] = 0x5555;
  (*channelNvList)[ch].cadbMuxPattern[1] = 0;
  (*channelNvList)[ch].cadbMuxPattern[2] = 0;
  (*channelNvList)[ch].cadbMuxPattern[3] = 0;


  //
  // Program CADB mux control
  //
  patCadbMuxCtl.Data = 0;
  patCadbMuxCtl.Bits.mux0_control = 1;
  patCadbMuxCtl.Bits.mux1_control = 1;
  patCadbMuxCtl.Bits.mux2_control = 1;
  patCadbMuxCtl.Bits.mux3_control = 1;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, patCadbMuxCtl.Data);
  return status;
}



/**

  DDR parity based command training

  @param host    - Pointer to sysHost
  @param socket  - Socket to train
  @param ch      - DDR channel to setup
  @param rank    - Logical rank number
  @param sig     - Signal to test

  @retval SUCCESS / FAILURE

**/
UINT32
CpgcAdvCmdParity (
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 UINT8     rank,
                 UINT8     subrank,
                 GSM_CSN   sig,
                 UINT8     mode
                 )
{
  UINT32                                        i;
  UINT8                                         c;
  UINT8                                         chBitmask = 0;
  UINT32                                        status;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
  CPGC_GLOBAL_TRAINING_SETUP                    cpgcGblTrainingSetup;
  CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT           patCadbMuxCtl;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;
  struct channelNvram                           (*channelNvList)[MAX_CH];
  PatCadbProg0 ParityPattern0Local[CADB_LINES];
  PatCadbProg1 ParityPattern1Local[CADB_LINES];
  PatCadbProg0 ActivatePattern0Local[CADB_LINES];
  PatCadbProg1 ActivatePattern1Local[CADB_LINES];
  PatCadbProg0 PrechargePattern0Local[CADB_LINES];
  PatCadbProg1 PrechargePattern1Local[CADB_LINES];
  PatCadbProg0 FnvGoodParity0Local[CADB_LINES];
  PatCadbProg1 FnvGoodParity1Local[CADB_LINES];
  PatCadbProg0 AddressPattern0Local[CADB_LINES];
  PatCadbProg1 AddressPattern1Local[CADB_LINES];
  PatCadbProg1 DeselectPattern1Local[CADB_LINES];
  PatCadbProg1 GntPattern1Local[CADB_LINES];

  MemDebugPrint((host, SDBG_CPGC, socket, ch, NO_DIMM, rank, NO_STROBE, NO_BIT,
                 "CpgcAdvCmdParity\n"));
  channelNvList = GetChannelNvList(host, socket);

  // Temporarily disable 3DS mode for MRS commands
  if (((*channelNvList)[ch].encodedCSMode == 2) && (host->nvram.mem.threeDsModeDisabled == 0)) {
    Toggle3DSMode(host, socket, ch, 0);
  }

  if ((*channelNvList)[ch].cadbMuxPattern[0] != 0x5555 || (*channelNvList)[ch].cadbMuxPattern[1] != 0x3333 ||
      (*channelNvList)[ch].cadbMuxPattern[2] != 0x0F0F || (*channelNvList)[ch].cadbMuxPattern[3] != 0x00FF) {
    for (c = 0; c < MAX_CH; c++) {
      if ((*channelNvList)[c].enabled == 0) continue;
      chBitmask |= 1 << c;
      //
      // Program CADB pattern buffer
      //
      MemWritePciCfgEp (host, socket, c, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG, 0x5555);
      MemWritePciCfgEp (host, socket, c, CPGC_PATCADBMUX1PB_MCDDC_CTL_REG, 0x3333);
      MemWritePciCfgEp (host, socket, c, CPGC_PATCADBMUX2PB_MCDDC_CTL_REG, 0x0F0F);
      MemWritePciCfgEp (host, socket, c, CPGC_PATCADBMUX3PB_MCDDC_CTL_REG, 0x00FF);
      //
      // Update Cached Values
      //
      (*channelNvList)[c].cadbMuxPattern[0] = 0x5555;
      (*channelNvList)[c].cadbMuxPattern[1] = 0x3333;
      (*channelNvList)[c].cadbMuxPattern[2] = 0x0F0F;
      (*channelNvList)[c].cadbMuxPattern[3] = 0x00FF;

      //
      // Program CADB mux control
      //
      patCadbMuxCtl.Data = 0;
      patCadbMuxCtl.Bits.mux0_control = 1;
      patCadbMuxCtl.Bits.mux1_control = 1;
      patCadbMuxCtl.Bits.mux2_control = 1;
      patCadbMuxCtl.Bits.mux3_control = 1;
      MemWritePciCfgEp (host, socket, c, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, patCadbMuxCtl.Data);

      status = CpgcAdvCmdParity(host, socket, c, rank, 0, DESELECT, mode);
    }
    ExecuteCmdSignalTest (host, socket, chBitmask);
    status = CpgcAdvCmdParity(host, socket, ch, rank, 0, sig, mode);
    return status;
  }

  for (i = 0; i < CADB_LINES; i++) {
    ParityPattern0Local[i] = ParityPattern0[i];
    ParityPattern1Local[i] = ParityPattern1[i];
    ActivatePattern0Local[i] = ActivatePattern0[i];
    ActivatePattern1Local[i] = ActivatePattern1[i];
    PrechargePattern0Local[i] = PrechargePattern0[i];
    PrechargePattern1Local[i] = PrechargePattern1[i];
    AddressPattern0Local[i] = AddressPattern0[i];
    AddressPattern1Local[i] = AddressPattern1[i];
    FnvGoodParity0Local[i] = FnvGoodParity0[i];
    FnvGoodParity1Local[i] = FnvGoodParity1[i];
    DeselectPattern1Local[i] = DeselectPattern1[i];
    GntPattern1Local[i] = GntPattern1[i];
  }

  status = SUCCESS;

  if ((mode == NORMAL_CADB_SETUP) || (mode != RESTORE_CADB_PATTERN)) {
    if (sig == GNT) {
      cpgcGblTrainingSetup.seqLoopCnt        = 0;                                // test will end after one loop through the sub-sequence //?? Incorrect comment
    } else {
      cpgcGblTrainingSetup.seqLoopCnt        = 1;                                // test will end after one loop through the sub-sequence
    }
    cpgcGblTrainingSetup.startDelay        = 0;                                // Delay start
    cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;                // Update based on the # of read and write cacheline
    cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0
    cpgcGblTrainingSetup.numCacheLines     = 2;                                // Number of Cachelines (2^N)
    cpgcGblTrainingSetup.burstSeparation   = 0;                                // Burst separation based on training step
    cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_ALL_CHUNK;       // monitor errors on any chunk
    cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_ALL_CL;          // monitor errors on any cacheline
    cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_STOP_ON_ALL_ERR;         // Stop on all lane error
    cpgcGblTrainingSetup.subseqEndPtr      = 1;                                // subseq end pointer
    cpgcGblTrainingSetup.useSubSeq1        = CPGC_BASE_READ_SUBSEQ;            // use sub seq1 (base read)
    cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
    cpgcGblTrainingSetup.back2backWrEn     = 0;                                // enable b2b writes
    cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
    cpgcGblTrainingSetup.refMask           = 0xFF;                             // disable cpgc self refresh
    cpgcGblTrainingSetup.zqMask            = 0xFF;                             // disable cpgc zq
    //if(x8) {
    cpgcGblTrainingSetup.x8Mode           = 0;
    //} else {
    // cpgcGblTrainingSetup.x8Mode           = 0;
    //}

    CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

    //
    // WDB programming
    //
    seqBaseAddrStartLo0.Data            = 0;
    seqBaseAddrStartHi0.Data            = 0;
    seqBaseAddrStartHi0.Bits.rank_addr  = rank;

    seqBaseAddrWrapLo0.Data             = 0;
    seqBaseAddrWrapLo0.Bits.column_addr = 0x1FF;
    seqBaseAddrWrapLo0.Bits.bank_addr   = 0x7;

    seqBaseAddrWrapHi0.Data           = 0;
    seqBaseAddrWrapHi0.Bits.row_addr  = 0x1000;
    seqBaseAddrWrapHi0.Bits.rank_addr = rank;

    //
    // Sequence Bank Address Increment control
    //
    seqBaincCtl00.Data = 0;
    seqBaincCtl00.Bits.column_ba_inc      = 0;
    seqBaincCtl00.Bits.column_ba_updrate  = 0;
    seqBaincCtl00.Bits.column_ba_updscale = 0;
    seqBaincCtl00.Bits.row_ba_inc         = 12;
    seqBaincCtl00.Bits.row_ba_updrate     = 0;
    seqBaincCtl00.Bits.row_ba_updscale    = 1;

    seqBaincCtl10.Data = 0;
    seqBaincCtl10.Bits.bank_ba_inc        = 4;
    seqBaincCtl10.Bits.bank_ba_updrate    = 0;
    seqBaincCtl10.Bits.bank_ba_updscale   = 1;
    seqBaincCtl10.Bits.rank_ba_inc        = 0;
    seqBaincCtl10.Bits.rank_ba_updrate    = 0;
    seqBaincCtl10.Bits.rank_ba_updscale   = 0;
    seqBaincCtl10.Bits.column_ba_inc_msb  = 0;

    //
    // Set wrap order
    //
    cpgcSeqBaoCic0.Data = 0;

    CpgcSetupWdb (host, socket, ch,
                  0,                             // wdbIncrRate
                  0,                             // wdbIncrScale
                  0,                             // wdbStartPntr
                  3,                             // wdbEndPntr
                  1,                             // setupPbwr
                  CPGC_WDB_MUX_CTL_LFSR24,       // mux = pattern buffer
                  seqBaincCtl00,
                  seqBaincCtl10,
                  seqBaseAddrStartLo0,
                  seqBaseAddrStartHi0,
                  seqBaseAddrWrapLo0,
                  seqBaseAddrWrapHi0,
                  cpgcSeqBaoCic0
                 );

  } // if NORMAL_CADB_SETUP
  //
  // CADB entries for ddr cmd group 1
  //

  switch (sig) {
  case PAR:
    FixupCS(rank, subrank, ParityPattern0Local, ParityPattern1Local, (*channelNvList)[ch].encodedCSMode);
    CADBPointerWorkaround(ParityPattern0Local, ParityPattern1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, ParityPattern0Local, ParityPattern1Local);
    break;
  case ACT_N:
    FixupCS(rank, subrank, ActivatePattern0Local, ActivatePattern1Local,(*channelNvList)[ch].encodedCSMode);
    CADBPointerWorkaround(ActivatePattern0Local, ActivatePattern1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, ActivatePattern0Local, ActivatePattern1Local);
    break;
  case RAS_N:
  case CAS_N:
  case WE_N:
  case BA0:
  case BA1:
  case BA2:
  case BG0:
  case BG1:
  case A0:
  case A1:
  case A2:
  case A3:
  case A4:
  case A5:
  case A6:
  case A7:
  case A8:
  case A9:
  case A10:
  case A11:
  case A12:
  case A13:
  case A14:
  case A15:
  case A16:
  case A17:
  case C0:
  case C1:
  case C2:
  case CS2_N:
  case CS6_N:
  case CS3_N:
  case CS7_N:
    FixupCS(rank, subrank, AddressPattern0Local, AddressPattern1Local, (*channelNvList)[ch].encodedCSMode);
    FixupAddrBits (host, sig, AddressPattern0Local, AddressPattern1Local);
    CADBPointerWorkaround(AddressPattern0Local, AddressPattern1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, AddressPattern0Local, AddressPattern1Local);
    break;
  case PRECHARGE:  // Precharge
    FixupCS(rank, subrank, PrechargePattern0Local, PrechargePattern1Local, (*channelNvList)[ch].encodedCSMode);
    CADBPointerWorkaround(PrechargePattern0Local, PrechargePattern1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, PrechargePattern0Local, PrechargePattern1Local);
    break;
  case FNV_GOOD_PARITY:
    FixupCS(rank, subrank, FnvGoodParity0Local, FnvGoodParity1Local, (*channelNvList)[ch].encodedCSMode);
    CADBPointerWorkaround(FnvGoodParity0Local, FnvGoodParity1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, FnvGoodParity0Local, FnvGoodParity1Local);
    break;
  case DESELECT:  // Deselect
    CADBPointerWorkaround(PrechargePattern0Local, DeselectPattern1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, PrechargePattern0Local, DeselectPattern1Local);
    break;
  case GNT:
    FixupCS(rank+1, 0, ActivatePattern0Local, GntPattern1Local, (*channelNvList)[ch].encodedCSMode); // GNT is on the 2nd CS per dimm
    CADBPointerWorkaround(ActivatePattern0Local, GntPattern1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, ActivatePattern0Local, GntPattern1Local);
    break;
  default:        // Deselect
    CADBPointerWorkaround(PrechargePattern0Local, DeselectPattern1Local);
    ProgramCADB (host, socket, ch, 0, CADB_LINES, PrechargePattern0Local, DeselectPattern1Local);
    break;
  }

  if ((mode == NORMAL_CADB_SETUP) || (mode != RESTORE_CADB_PATTERN)) {
    //
    // Enable CADB
    //
    cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
    cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = 0;
    cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 1;
    if (sig == GNT) {
      cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 0;
    } else {
      cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = 1;
    }
    if (host->nvram.mem.dimmTypePresent == UDIMM) {
      cpgcPatCadbCtlReg.Bits.enable_cadb_parityntiming = 1;
    }
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);
  }

  return status;

} // CpgcAdvCmdParity

/**

  Fix up the CS field for programming the CADB

  @param cs      - Chip select
  @param pattern - Data to program

**/
static void
FixupCS (
        UINT8         cs,
        UINT8         subrank,
        PatCadbProg0  prog0[CADB_LINES],
        PatCadbProg1  prog1[CADB_LINES],
        UINT8         encodedCSMode
        )
{
  UINT8 cadbLine;
  UINT8 dimm;

  for (cadbLine = 0; cadbLine < CADB_LINES; cadbLine++) {
    if (prog1[cadbLine].cs != 0x3FF) {
      prog1[cadbLine].cs =  (~(1 << cs)) & 0x3FF;
    }
    if (encodedCSMode == 2) {
      dimm = (cs / 4) * 4;
      prog1[cadbLine].cs = prog1[cadbLine].cs & ~(0xc<<(dimm));
      prog1[cadbLine].cs |= (subrank << (dimm +2)); // program subranks CID0/CID1
      prog0[cadbLine].cid = subrank;  // program CID2 if its needed
    }
  }
} // FixupCS

/**

  Fix up the address bit field for programming the CADB

  @param sig     - Signal to test
  @param pattern - Data to program

**/
static void
FixupAddrBits (
              PSYSHOST      host,
              GSM_CSN       sig,
              PatCadbProg0  prog0[CADB_LINES],
              PatCadbProg1  prog1[CADB_LINES]
              )
{
  UINT8 cadbLine;
  UINT8 pattern[CADB_LINES] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1};

  for (cadbLine = 0; cadbLine < CADB_LINES; cadbLine++) {
    if ((sig >= A0) && (sig <= A17)) {
      prog0[cadbLine].address = pattern[cadbLine] << (sig - A0);
    } else {
      prog0[cadbLine].address  = 0;
    }

    if ((sig >= BA0) && (sig <= BA2)) {
      prog0[cadbLine].bank  = pattern[cadbLine] << (sig - BA0);
    } else {
      prog0[cadbLine].bank  = 0;
    }

    if ((sig >= BG0) && (sig <= BG1)) {
      prog0[cadbLine].bank  = (pattern[cadbLine] << (sig - BG0)) << 2;
    } else {
      prog0[cadbLine].bank  &= 3;
    }

    if ((sig >= C0) && (sig <= C2)) {
      prog0[cadbLine].cid  = (pattern[cadbLine] << (sig - C0));
    } else {
      prog0[cadbLine].cid  = 0;
    }

    if (sig == RAS_N) {
      prog1[cadbLine].control  = pattern[cadbLine] << 2;
    } else {
      prog1[cadbLine].control  &= ~BIT2;
    }

    if (sig == CAS_N) {
      prog1[cadbLine].control  = pattern[cadbLine] << 1;
    } else {
      prog1[cadbLine].control  &= ~BIT1;
    }

    if (sig == WE_N) {
      prog1[cadbLine].control  = pattern[cadbLine];
    } else {
      prog1[cadbLine].control  &= ~BIT0;
    }

    // CS#/CIDs only trained during earlycmdclk for 3DS DIMMs
    if ((sig == CS2_N) || (sig == CS3_N) || (sig == CS6_N) || (sig == CS7_N)) {
      prog1[cadbLine].cs &= ((~(1 << (sig - CS0_N))) & 0x3FF); // Clears Target Chip Select
      prog1[cadbLine].cs |= ((pattern[cadbLine]) << (sig - CS0_N)); // Programs Pattern on Target Chip Select
      if (sig == CS2_N || sig == CS6_N) {
        prog1[cadbLine].cs &= ((~(1 << (sig - CS0_N + 1))) & 0x3FF); // Clears CS3 or CS7
      } else if (sig == CS3_N || sig == CS7_N) {
        prog1[cadbLine].cs &= ((~(1 << (sig - CS0_N - 1))) & 0x3FF); // Clears CS2 or CS6
      }
    }

  } // cadbLine loop
} // FixupAddrBits


/**

  Loads the CADB

  @param host    - Pointer to sysHost
  @param socket  - Socket to train
  @param ch      - DDR channel to setup
  @param rank    - Logical rank number
  @param pattern - Data to program

  @retval SUCCESS / FAILURE

**/
void
ProgramCADB (
            PSYSHOST      host,
            UINT8         socket,
            UINT8         ch,
            UINT8         startLine,
            UINT8         numLines,
            PatCadbProg0  prog0[CADB_LINES],
            PatCadbProg1  prog1[CADB_LINES]
            )
{
  UINT8                                   cadbLine;
  CPGC_PATCADBPROG0_MCDDC_CTL_STRUCT  patCADBProg0;
  CPGC_PATCADBPROG1_MCDDC_CTL_STRUCT  patCADBProg1;


    //
    // Set the write pointer to 0
    //
    MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBWRPNTR_MCDDC_CTL_REG, startLine);

    //
    // Program 16 lines in the CADB
    //
    for (cadbLine = startLine; cadbLine < numLines; cadbLine++) {
      patCADBProg0.Data          = 0;
      patCADBProg0.Bits.address  = prog0[cadbLine - startLine].address;
      patCADBProg0.Bits.cid      = prog0[cadbLine - startLine].cid;
      patCADBProg0.Bits.bank     = prog0[cadbLine - startLine].bank;
      patCADBProg0.Bits.pdatag   = prog0[cadbLine - startLine].pdatag;

      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBPROG0_MCDDC_CTL_REG, patCADBProg0.Data);

      patCADBProg1.Data          = 0;
      patCADBProg1.Bits.cs       = prog1[cadbLine - startLine].cs;
      patCADBProg1.Bits.control  = prog1[cadbLine - startLine].control;
      patCADBProg1.Bits.odt      = prog1[cadbLine - startLine].odt;
      patCADBProg1.Bits.cke      = prog1[cadbLine - startLine].cke;
      patCADBProg1.Bits.par      = prog1[cadbLine - startLine].par;

      MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBPROG1_MCDDC_CTL_REG, patCADBProg1.Data);
    } // cadbLine loop
} // ProgramCADB

/**

  This function returns the CH corrected offset for each used CPGC register, and corrects for HA0/1 if needed

  @param host                - Pointer to sysHost
  @param socket              - Socket number
  @param ch                  - Channel Number
  @param regOffset           - Base register offset

  @retval New register Offset based on CH number

**/
UINT32
GetCpgcRegOffset (
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 UINT32    regOffset
                 )
{
  //SKX change
  ch = ch % host->var.mem.numChPerMC;

  switch (regOffset) {
  //case CPGC_SEQ_BASEADDRSTART0_MC_MAINEXT_REG:
  //case CPGC_SEQ_BASEADDRWRAP0_MC_MAINEXT_REG:
  case CPGC_SEQ_BAOCIC0_MC_MAIN_REG:
  case CPGC_SEQCTL0_MC_MAIN_REG:
  case CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG:
    return(regOffset + (4 * ch));
    break;
  case CPGC_SEQ_BAINCCTL00_MC_MAIN_REG:
  case CPGC_SEQ_BAINCCTL10_MC_MAIN_REG:
    return(regOffset + (8 * ch));
    break;
  case CPGC_SUBSEQ0_CTL0_MC_MAIN_REG:
  case CPGC_SUBSEQ1_CTL0_MC_MAIN_REG:
    return(regOffset + (0x50 * ch));
    break;
  case CPGC_GLOBALERR_MC_MAIN_REG:
  case CPGC_GLOBALCTRL_MC_MAIN_REG:
    return(regOffset);
    break;
  default:
    MemDebugPrint((host, SDBG_MIN, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "ERROR: GetCpgcRegOffset - Invalid offset 0x%8X\n", regOffset));
    RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_50);
    return(regOffset);
  }
} // GetCpgcRegOffset

/**

  Populates the provided CADB row

  @param host    - Pointer to sysHost
  @param socket  - Socket to train
  @param ch      - DDR channel to setup
  @param rank    - Logical rank number
  @param bank    - MRS to write to
  @param data    - Data to write
  @param side    - Side of the register (A or B)
  @param pdaTag  - PDA

**/
static void
ProgCADBRow (
            PSYSHOST      host,
            UINT8         socket,
            UINT8         ch,
            UINT8         dimm,
            UINT8         rank,
            UINT8         bank,
            UINT16        data,
            UINT8         side,
            UINT8         pdaTag,
            UINT8         control,
            PatCadbProg0  *patCADBProg0,
            PatCadbProg1  *patCADBProg1
            )
{
  UINT8             i;
  UINT8             count;
  struct dimmNvram  (*dimmNvList)[MAX_DIMM];
  struct ddrRank    (*rankList)[MAX_RANK_DIMM];
  struct channelNvram  (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  dimmNvList = GetDimmNvList(host, socket, ch);
  rankList = GetRankNvList(host, socket, ch, dimm);

  //
  // Mirror address/bank bits
  //
  if ((rank & BIT0) && (*dimmNvList)[dimm].SPDAddrMapp) {
    data  = MirrorAddrBits (host, data);
    bank  = MirrorBankBits (host, bank);
  }

  //
  // Invert address/bank bits
  //
  if (side == SIDE_B) {
    data  = InvertAddrBits (data);
    bank  = InvertBankBits (bank);
  }

  (*patCADBProg0).address = data;
  (*patCADBProg0).bank    = bank;
  (*patCADBProg0).pdatag  = pdaTag;
  (*patCADBProg0).cid     = 0;
  if (IsLrdimmPresent(host, socket, ch, dimm) && (*channelNvList)[ch].encodedCSMode) {
    // here we are in encoded quad CS mode
    (*patCADBProg0).cid     = (rank >> 1) | 0x6;
  }

  (*patCADBProg1).cs      = ~(1 << (*rankList)[rank].CSIndex) & ((1 << 11) - 1);
  // zero out CID0/1 as CS's bits
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    (*patCADBProg1).cs = (*patCADBProg1).cs   & ~(0xc<<(dimm*4));
  }

  (*patCADBProg1).control = control;

  //
  // Assert ACT bit
  //
  (*patCADBProg1).control |= BIT3;

  //
  // Calculate parity
  //
  count = 0;

  // Count the 1's in CID bits if they are to be included in the parity calculation
  if ((*channelNvList)[ch].encodedCSMode == 2) {
    if (((*channelNvList)[ch].cidBitMap & BIT2) && ((*patCADBProg0).cid & BIT2))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT1) && ((*patCADBProg0).cid & BIT1))
      count++;
    if (((*channelNvList)[ch].cidBitMap & BIT0) && ((*patCADBProg0).cid & BIT0))
      count++;
  }

  // Count the 1's in the address bits
  for (i = 0; i < 17; i++) {
    if ((*patCADBProg0).address & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the bank bits
  for (i = 0; i < 4; i++) {
    if ((*patCADBProg0).bank & (1 << i)) {
      count++;
    }
  } // i loop

  // Count the 1's in the control bits
  for (i = 0; i < 4; i++) {
    if ((*patCADBProg1).control & (1 << i)) {
      count++;
    }
  } // i loop

  if (count & BIT0) {
    (*patCADBProg1).par = 1;
  } else {
    (*patCADBProg1).par = 0;
  }

  (*patCADBProg1).cke = 0x3F;
  (*patCADBProg1).odt = 0;

} // ProgCADBRow


//-----------------------------------------------------------------------------------------------
/**

  This function loads the WDB using PCI accesses

  @param host              - Pointer to sysHost
  @param socket              - Socket number
  @param ch                - Channel number
  @param NumberOfWdbLines  - Number of cachelines to load
  @param WdbLines          - Pointer to data to write to the WDB

  @retval N/A

**/
static void
WdbPreLoadChunk (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT32    *wdbLines
                )
{
  // Setting rdwr_pntr and rdwr_subpntr to 0
  // cache line 0 and chunk 0
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_RDWR_PNTR_MCDDC_DP_REG, 0);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_WR0_MCDDC_DP_REG, *(wdbLines));
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_WR1_MCDDC_DP_REG, *(wdbLines + 1));


} // WdbPreLoadChunk

//-----------------------------------------------------------------------------------------------
/**

Arguments:

  host        - Pointer to the system host (root) structure
  ch          - DDR Channel ID

  @retval 0 - success
  @retval 1 - failure

**/
void
CpgcSetup64 (
            PSYSHOST host,
            UINT8  socket,
            UINT8  ch,
            UINT8  dimm,
            UINT32 dpa,
            UINT8  mode,
            UINT8  uiBitMask,
            UINT64_STRUCT *data
            )

{
  UINT8   c;
  UINT8   mcId;
  UINT8   logRank;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  CPGC_SEQCTL0_MC_MAIN_STRUCT                   seqCtl0;
  CPGC_SUBSEQ0_CTL0_MC_MAIN_STRUCT              subSeqCtl0;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               seqBAOCIC0;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBAIncCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBAIncCtl10;
  CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT             patWDBCLCtl;
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT          patWDBCLMuxCtl;
  CPGC_ERR_CTL_MCDDC_CTL_STRUCT                 errCtl;
  AMAP_MC_MAIN_STRUCT                           amap;
  MCMTR_MC_MAIN_STRUCT                          mcMtr;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT               ddrtMiscDelay;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT                cpgcGlobalCtrl;

  // Disable CPGC on other channels
  for (c = 0; c < MAX_CH; c++) {
    if (c != ch) {
      MemWritePciCfgMC_Ch (host, socket, c, CPGC_SEQCTL0_MC_MAIN_REG, 4, 0);   // Clear global control
    }
  }

  dimmNvList = GetDimmNvList(host, socket, ch);
  mcId = GetMCID(host, socket, ch);
  logRank = GetLogicalRank(host, socket, ch, dimm, 0);

  WdbPreLoadChunk (host, socket, ch, (UINT32 *)data);

  //
  // Program Sequence
  //
  seqCtl0.Data = 0;
  seqCtl0.Bits.subseq_start_pointer = 0;
  seqCtl0.Bits.subseq_end_pointer   = 0;
  seqCtl0.Bits.enable_dummy_reads   = 0;
  seqCtl0.Bits.global_control       = 1;
  seqCtl0.Bits.initialization_mode  = 1;
  seqCtl0.Bits.loopcount            = 1;
  seqCtl0.Bits.start_test_delay     = 2;      // Delay the start of the test by 2 clocks

  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data);
  // SKX TODO: can this CSR write be combined?
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQCTL0_MC_MAIN_REG, 4, seqCtl0.Data | BIT2);

  //
  // Program Sub Sequences
  //
  subSeqCtl0.Data = 0;
  subSeqCtl0.Bits.rstcurr_ba_start = 1;
  subSeqCtl0.Bits.num_cachelines = 1;
  subSeqCtl0.Bits.num_cachelines_scale = 1;
  subSeqCtl0.Bits.subseq_wait = 0;

  // Choose read vs. write command
  if (mode == READ_ACCESS) {
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BRd  << 20)); // Read CMD
  } else {
    MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SUBSEQ0_CTL0_MC_MAIN_REG, 0x50, subSeqCtl0.Data + (BWr  << 20)); // Write CMD
  }

  //
  // Program Sequence Address
  //

  /*
  Address mapping:
              Bank      Row       Column
  CPGC CSR    3:0       16:0      17:0
  NVMDIMM DPA    42:39     38:22   RFU,RFU,21:6

  Note: NVMCTLR DPA CSR aperture is assumed to fit within 32 bit address space,
        so this function does not support access to DPA 4GB and above.
  */

  seqBaseAddrStartLo0.Data              = 0;
  seqBaseAddrStartLo0.Bits.column_addr  = (dpa >> 6) & 0xFFFF;
  seqBaseAddrStartLo0.Bits.bank_addr    = 0;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_REG, 4, seqBaseAddrStartLo0.Data);
  seqBaseAddrStartHi0.Data              = 0;
  seqBaseAddrStartHi0.Bits.row_addr     = (dpa >> 22) & 0x3FF;
  seqBaseAddrStartHi0.Bits.rank_addr    = 0;                // Use CPGC_SEQ_RANKLG2PHYMAP0 to select physical rank
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_REG, 4, seqBaseAddrStartHi0.Data);

  seqBaseAddrWrapLo0.Data               = 0;
  seqBaseAddrWrapLo0.Bits.column_addr   = seqBaseAddrStartLo0.Bits.column_addr;
  seqBaseAddrWrapLo0.Bits.bank_addr     = seqBaseAddrStartLo0.Bits.bank_addr;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_REG, 4, seqBaseAddrWrapLo0.Data);
  seqBaseAddrWrapHi0.Data               = 0;
  seqBaseAddrWrapHi0.Bits.row_addr      = seqBaseAddrStartHi0.Bits.row_addr;
  seqBaseAddrWrapHi0.Bits.rank_addr     = 0;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_REG, 4, seqBaseAddrWrapHi0.Data);

  seqBAOCIC0.Data = 0;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAOCIC0_MC_MAIN_REG, 4, seqBAOCIC0.Data);

  seqBAIncCtl00.Data = 0;
  seqBAIncCtl10.Data = 0;
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAINCCTL00_MC_MAIN_REG, 8, seqBAIncCtl00.Data);
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_BAINCCTL10_MC_MAIN_REG, 8, seqBAIncCtl10.Data);

  //
  // Program Write Data Buffer Related Entries
  //
  patWDBCLCtl.Data = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCLCTL_MCDDC_CTL_REG, patWDBCLCtl.Data);

  // Select LFSR
  patWDBCLMuxCtl.Data = 0;
  patWDBCLMuxCtl.Bits.mux0 = LFSRMode;
  patWDBCLMuxCtl.Bits.mux1 = LFSRMode;
  patWDBCLMuxCtl.Bits.mux2 = LFSRMode;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, patWDBCLMuxCtl.Data);

  // Currently, not planning to use the Inverstion Mask
  //MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV_MCDDC_DP_REG, 0);

  if (mode == READ_ACCESS) {
    //
    // Program Error Checking
    //
    errCtl.Data                         = 0;
    errCtl.Bits.stop_on_ntherr          = 1;
    errCtl.Bits.stop_on_errcontrol      = 1;
    errCtl.Bits.selective_err_enb_chunk = uiBitMask;
    errCtl.Bits.selective_err_enb_cl    = 0xFF;
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_CTL_MCDDC_CTL_REG, errCtl.Data);

    //
    // Do not mask off errors
    //
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA0_MCDDC_CTL_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA1_MCDDC_CTL_REG, 0);
  }

  // Select target rank type
  amap.Data = MemReadPciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG);
  mcMtr.Data = MemReadPciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG);
  ddrtMiscDelay.Data = MemReadPciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG);
  cpgcGlobalCtrl.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);

  if ((*dimmNvList)[dimm].aepDimmPresent) {
    mcMtr.Bits.trng_target |= (1 << (ch % MAX_MC_CH));
    cpgcGlobalCtrl.Bits.ddrt_addr_mode |= (1 << (ch % MAX_MC_CH));
    ddrtMiscDelay.Bits.force_ddr4_mode = 0;
    ddrtMiscDelay.Bits.force_ddrt_mode = 1;
  } else {
    mcMtr.Bits.trng_target &= ~(1 << (ch % MAX_MC_CH));
    cpgcGlobalCtrl.Bits.ddrt_addr_mode &= ~(1 << (ch % MAX_MC_CH));
    ddrtMiscDelay.Bits.force_ddr4_mode = 1;
    ddrtMiscDelay.Bits.force_ddrt_mode = 0;
  }
  MemWritePciCfgEp (host, socket, ch, AMAP_MC_MAIN_REG, amap.Data);
  MemWritePciCfgMC (host, socket, mcId, MCMTR_MC_MAIN_REG, mcMtr.Data);
  MemWritePciCfgEp (host, socket, ch, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);
  MemWritePciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlobalCtrl.Data);

  // Physical Rank Mapping
  MemWritePciCfgMC_Ch (host, socket, ch, CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG, 4, logRank);
}

void
SetupPatternSequencer (
  PSYSHOST      host,
  UINT8         socket,
  UINT8         ch,
  SequencerMode sequencerMode,
  UINT32        *sequencerPattern
  )
/*++

Routine Description:

  Setup Data Pattern Configuration and Data Pattern Sequencer registers

Arguments:

  host                - Pointer to sysHost
  socket              - Memory controller number
  ch                  - channel
  sequencerMode       - SequencerMode struct
  sequencerPattern    - Pointer to array of pattern sequences
                        that are to be programmed

Returns:

  n/a
--*/
{
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT  patWDBCLMuxCtl;

  // ECC, Bit Buffer
  patWDBCLMuxCtl.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG);
  patWDBCLMuxCtl.Bits.mux0  = sequencerMode.Bits.seqMode0;
  patWDBCLMuxCtl.Bits.mux1  = sequencerMode.Bits.seqMode1;
  patWDBCLMuxCtl.Bits.mux2  = sequencerMode.Bits.seqMode2;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, patWDBCLMuxCtl.Data);

  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG, sequencerPattern[0]);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX1_PBRD_MCDDC_DP_REG, sequencerPattern[1]);
  MemWritePciCfgEp (host, socket, ch, CPGC_PATWDBCL_MUX2_PBRD_MCDDC_DP_REG, sequencerPattern[2]);
} //SetupPatternSequencer

/**
  Clears the bitwise error mask registers

  @param host:         Pointer to sysHost
  @param socket:       Processor socket to clear
  @param ch:           Channel to clear

**/
void
ClearBwErrorMask(
                 PSYSHOST        host,
                 UINT8           socket
                 )
{
  UINT8             ch;
  UINT8             dimm;
  UINT8             rank;
  struct bitMask    filter;

  //
  // Clear the filter bits to enable error checking on every bit
  //
  ClearFilter(host, socket, &filter);

  // Setup bitMask
  for (ch = 0; ch < MAX_CH; ch++) {
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      // Loop for each rank
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {

        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        SetBwErrorMask(host, socket, dimm, rank, &filter);
      } // rank loop
    } // dimm loop
  } // ch loop
} // ClearBwErrorMask

/**
  Sets the IOSAV_CH_X_BW_SERR registers to mask off data bits from reporting failures

  @param host:         Pointer to sysHost
  @param socket:         Processor socket to mask
  @param dimm:         DIMM to mask
  @param rank:         Rank to mask
  @param bitMask:      Bits to be masked

**/
void
SetBwErrorMask(
              PSYSHOST        host,
              UINT8           socket,
              UINT8           dimm,
              UINT8           rank,
              struct bitMask  *mask
              )
{
  UINT8                 ch;
  UINT32                csrReg;
  struct  channelNvram  (*channelNvList)[MAX_CH];
  struct  ddrRank       (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    rankList = GetRankNvList(host, socket, ch, dimm);

    csrReg = (UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][0];
    csrReg |= ((UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][1] << 8);
    csrReg |= ((UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][2] << 16);
    csrReg |= ((UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][3] << 24);

    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA0_MCDDC_CTL_REG, csrReg);

    csrReg = (UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][4];
    csrReg |= ((UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][5] << 8);
    csrReg |= ((UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][6] << 16);
    csrReg |= ((UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][7] << 24);

    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_DATA1_MCDDC_CTL_REG, csrReg);

    csrReg = (UINT32)mask->bits[ch][(*rankList)[rank].rankIndex][8];
    MemWritePciCfgEp (host, socket, ch, CPGC_ERR_ECC_MCDDC_CTL_REG, csrReg);

  } // ch loop
} // SetBwErrorMask

static void
FillWdbPprPattern (
    PSYSHOST  host,
    UINT8     socket,
    UINT8     ch,
    UINT8     dimm,
    UINT32    dramMask
 )
/*++

  Setup WDB pattern for PPR

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ch          - ddr channel to setup
  @param dimm        - DIMM id
  @param dram        - dram that is target of PPR sequence

  @retval None

--*/
{
  UINT8                                         i;
  UINT8                                         maxBits;
  TWdbLine                                      WdbLines[MRC_WDB_LINES];
  CPGC_PATWDB_INV_MCDDC_DP_STRUCT               patwdbInvReg;
  struct dimmNvram                              (*dimmNvList)[MAX_DIMM];
  UINT8                                         dram;

  dimmNvList    = GetDimmNvList(host, socket, ch);

  if ((*dimmNvList)[dimm].x4Present) {
    maxBits = 4;
  } else {
    maxBits = 8;
  }

  //
  // Program WDB data for PPR pattern
  //
  for (i = 0; i < MRC_WDB_LINE_SIZE; i++) {
    WdbLines[0].WdbLine[i] = 0xFF;
    for (dram = 0; dram < MAX_STROBE; dram++) {
      if ((~dramMask) & (1 << dram)) continue;

      if (((dram * maxBits) / 8) == (i % 8)) {
        //
        // Insert '0' pattern for target DRAM
        //
        if (maxBits == 4) {
          WdbLines[0].WdbLine[i] ^= (0xF << (4 * (dram % 2)));
        } else {
          WdbLines[0].WdbLine[i] = 0;
        }
      }
    }
  }

  WDBFill (host, socket, ch, WdbLines, 1, 0);

  //
  // ECC bits 0..7 mirror DQ0..7
  // We need to invert ECC if x8 and DRAM 0/8 or if x4 and DRAM0/16 or DRAM1/17 is the target
  //
  if (host->nvram.mem.eccEn) {
    patwdbInvReg.Data = 0;
    for (dram = 0; dram < MAX_STROBE; dram++) {
      if ((~dramMask) & (1 << dram)) continue;
      if ((maxBits == 4) && (((dram / 2) == 0) || ((dram / 2) == 8))) {
        patwdbInvReg.Bits.eccinv_or_dcenb ^= (0xF << (4 * (dram % 2)));
      } else if ((maxBits == 8) && ((dram == 0) || (dram == 8))) {
        patwdbInvReg.Bits.eccinv_or_dcenb ^= 0xFF;
      }
    }
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV_MCDDC_DP_REG, patwdbInvReg.Data);
  } // if ecc en
}

UINT32
CpgcSetupPprPattern (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     subRank,
  UINT32    dramMask,
  UINT8     bank,
  UINT32    row
  )
/*++

  CPGC setup for PPR Sequence

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ch          - ddr channel to setup
  @param dimm        - DIMM id
  @param rank        - rank id
  @param subRank     - sub rank ID
  @param dramMask    - bitmap of dram that is target of PPR sequence
  @param bank        - bank id
  @param row         - row id

  @retval SUCCESS
  @retval FAILURE

--*/
{
  UINT32                                        status = SUCCESS;
  CPGC_GLOBAL_TRAINING_SETUP                    cpgcGblTrainingSetup;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;

  cpgcGblTrainingSetup.seqLoopCnt        = 1;                      // test will end after (2^(n-1)) loops through the sub-sequence
  cpgcGblTrainingSetup.startDelay        = 0;                      // Delay start
  cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;      // Update based on the # of read and write cacheline

  // Default to WRITE
  cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0

  cpgcGblTrainingSetup.numCacheLines     = 0;                                // Number of Cachelines (2^N)
  cpgcGblTrainingSetup.burstSeparation   = 0;                                // (sub_seqwait) - need to add delay to drain credits if doing write / read subseq
  cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_ALL_CHUNK;       // monitor errors on any chunk
  cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_ALL_CL;          // monitor errors on any cacheline
  cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_STOP_ON_ALL_ERR;         // Stop on all lane error
  cpgcGblTrainingSetup.subseqEndPtr      = 0;                                // subseq end pointer
  cpgcGblTrainingSetup.useSubSeq1        = CPGC_NO_SUBSEQ1;                  // do not use sub seq1 (base read)
  cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
  cpgcGblTrainingSetup.back2backWrEn     = 1;                                // enable b2b writes
  cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
  cpgcGblTrainingSetup.refMask           = 0xFF;                             // disable cpgc self refresh
  cpgcGblTrainingSetup.zqMask            = 0xFF;                             // disable cpgc zq
  cpgcGblTrainingSetup.x8Mode            = 0;

  CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

  //
  // Set WDB address order
  //
  seqBaincCtl00.Data = 0;
  seqBaincCtl10.Data = 0;

  seqBaseAddrStartLo0.Data = 0;
  seqBaseAddrStartLo0.Bits.bank_addr = bank;

  seqBaseAddrStartHi0.Data = 0;
  seqBaseAddrStartHi0.Bits.row_addr = row;

  seqBaseAddrWrapLo0.Data = 0;
  seqBaseAddrWrapLo0.Bits.bank_addr = 0xf;
  seqBaseAddrWrapLo0.Bits.column_addr = 0x7f;

  seqBaseAddrWrapHi0.Data = 0;
  seqBaseAddrWrapHi0.Bits.row_addr = 0x1ffff;
  seqBaseAddrWrapHi0.Bits.rank_addr = 0;

  SelectCPGCRanks(host, socket, ch, 1 << GetLogicalRank (host, socket, ch, dimm, rank), 0, subRank);

  cpgcSeqBaoCic0.Data = 0;

  CpgcSetupWdb (host, socket, ch,
                0,                           // wdbIncrRate
                0,                           // wdbIncrScale
                0,                           // wdbStartPntr (victim bit 0)
                0,                           // wdbEndPntr (victim bit 0)
                1,                           // setupPbwr
                CPGC_WDB_MUX_CTL_PATBUF,     // mux control
                seqBaincCtl00,               // column base address incr
                seqBaincCtl10,
                seqBaseAddrStartLo0,
                seqBaseAddrStartHi0,
                seqBaseAddrWrapLo0,
                seqBaseAddrWrapHi0,
                cpgcSeqBaoCic0
               );

  //
  // Disable CADB
  //
  cpgcPatCadbCtlReg.Data = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  //
  // Program WDB data for PPR pattern
  //
  FillWdbPprPattern (host, socket, ch, dimm, dramMask);

  return status;
}


UINT32
CpgcPprTestGlobalSetup (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     subRank,
  UINT32    dramMask,
  UINT8     bank,
  UINT32    row,
  UINT32    pattern
  )
/*++

  DDR Mem test / Mem Init CPGC setup.

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param ch          - ddr channel to setup
  @param dimm        - DIMM id
  @param rank        - rank id
  @param subRank     - sub rank ID
  @param dram        - dram that is target of PPR sequence
  @param bank        - bank id
  @param row         - row id
  @param pattern     - flag to indicate mem init vs. mem test and read vs. write and data pattern

  @retval SUCCESS
  @retval FAILURE

--*/
{

  UINT32                                        status;
  UINT32                                        wdbData;
  UINT8                                         mcId;
  CPGC_GLOBAL_TRAINING_SETUP                    cpgcGblTrainingSetup;
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT            seqBaincCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT            seqBaincCtl10;
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT     seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT     seqBaseAddrStartHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT      seqBaseAddrWrapLo0;
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT      seqBaseAddrWrapHi0;
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT               cpgcSeqBaoCic0;

  struct dimmNvram                              (*dimmNvList)[MAX_DIMM];
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT                cpgcGlobalCtrlReg;
  MemDebugPrint((host, SDBG_CPGC, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
                 "CpgcPprTestSetup: pattern = 0x%x\n", pattern));

  status = SUCCESS;

  dimmNvList    = GetDimmNvList(host, socket, ch);

  cpgcGblTrainingSetup.seqLoopCnt        = 1;                      // test will end after (2^(n-1)) loops through the sub-sequence
  cpgcGblTrainingSetup.startDelay        = 0;                                // Delay start
  cpgcGblTrainingSetup.addrUpdRateMode   = CPGC_UPDATE_BY_CL;                // Update based on the # of read and write cacheline

  // Default to WRITE
  cpgcGblTrainingSetup.rwMode            = CPGC_BASE_WRITE_SUBSEQ;           // Base Read or write - subseq0

  // If MEM_TEST is read, set to READ
  if ((pattern & (DDR_CPGC_MEM_TEST_READ | DDR_CPGC_MEM_TEST_WRITE)) == DDR_CPGC_MEM_TEST_READ) {
    cpgcGblTrainingSetup.rwMode          = CPGC_BASE_READ_SUBSEQ;            // Base Read or write - subseq0
  }

  //
  // Calculate # of cache lines required from the # of column bits we need
  // to sweep in the row
  // Number of Cachelines (2^N)
  //
  mcId = GetMCID(host, socket, ch);
  cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, socket, mcId, CPGC_GLOBALCTRL_MC_MAIN_REG);
  if (cpgcGlobalCtrlReg.Bits.use_bl4_col_addr == 1) {
    cpgcGblTrainingSetup.numCacheLines     =  columnBitsDDR4[(*dimmNvList)[dimm].techIndex] - 2;
  } else {
    cpgcGblTrainingSetup.numCacheLines     =  columnBitsDDR4[(*dimmNvList)[dimm].techIndex] - 3;
  }

  cpgcGblTrainingSetup.burstSeparation   = 0;                                // (sub_seqwait) - need to add delay to drain credits if doing write / read subseq
  cpgcGblTrainingSetup.errEnChunk        = CPGC_ERR_MONITOR_ALL_CHUNK;       // monitor errors on any chunk
  cpgcGblTrainingSetup.errEnCl           = CPGC_ERR_MONITOR_ALL_CL;          // monitor errors on any cacheline
  cpgcGblTrainingSetup.stopOnErrCtl      = CPGC_ERR_STOP_ON_ALL_ERR;         // Stop on all lane error
  cpgcGblTrainingSetup.subseqEndPtr      = 0;                                // subseq end pointer
  if ((pattern & (DDR_CPGC_MEM_TEST_READ | DDR_CPGC_MEM_TEST_WRITE)) == (DDR_CPGC_MEM_TEST_READ | DDR_CPGC_MEM_TEST_WRITE)) {
    cpgcGblTrainingSetup.useSubSeq1        = CPGC_BASE_READ_SUBSEQ;          // use sub seq1 (base read
  } else {
    cpgcGblTrainingSetup.useSubSeq1        = CPGC_NO_SUBSEQ1;                // do not use sub seq1 (base read)
  }
  cpgcGblTrainingSetup.cpgcLinkTrain     = 0;                                // do not use cpgc link train
  cpgcGblTrainingSetup.back2backWrEn     = 1;                                // enable b2b writes
  cpgcGblTrainingSetup.mprControl        = 0;                                // MPR disable
  cpgcGblTrainingSetup.refMask           = 0;                                // enable cpgc self refresh
  cpgcGblTrainingSetup.zqMask            = 0;                                // enable cpgc zq
  cpgcGblTrainingSetup.x8Mode            = 0;

  CpgcGlobalTrainingSetup (host, socket, ch, &cpgcGblTrainingSetup);

  //
  // Set WDB address order
  //
  seqBaincCtl00.Data = 0;
  seqBaincCtl10.Data = 0;
  seqBaincCtl00.Bits.column_ba_inc = 1;

  seqBaseAddrStartLo0.Data = 0;
  seqBaseAddrStartLo0.Bits.bank_addr = bank;

  seqBaseAddrStartHi0.Data = 0;
  seqBaseAddrStartHi0.Bits.row_addr = row;

  seqBaseAddrWrapLo0.Data = 0;
  seqBaseAddrWrapLo0.Bits.bank_addr = 0xf;
  seqBaseAddrWrapLo0.Bits.column_addr = 0x7f;

  seqBaseAddrWrapHi0.Data = 0;
  seqBaseAddrWrapHi0.Bits.row_addr = 0x1ffff;
  seqBaseAddrWrapHi0.Bits.rank_addr = 0;

  cpgcSeqBaoCic0.Data = 0;
  cpgcSeqBaoCic0.Bits.column_bw_cryen = 1;

  SelectCPGCRanks(host, socket, ch, 1 << GetLogicalRank (host, socket, ch, dimm, rank), 0, subRank);

  cpgcSeqBaoCic0.Data = 0;


  CpgcSetupWdb (host, socket, ch,
                0,                           // wdbIncrRate
                0,                           // wdbIncrScale
                0,                           // wdbStartPntr (victim bit 0)
                0,                           // wdbEndPntr (victim bit 0)
                1,                           // setupPbwr
                CPGC_WDB_MUX_CTL_PATBUF,     // mux control
                seqBaincCtl00,               // column base address incr
                seqBaincCtl10,
                seqBaseAddrStartLo0,
                seqBaseAddrStartHi0,
                seqBaseAddrWrapLo0,
                seqBaseAddrWrapHi0,
                cpgcSeqBaoCic0
               );

  //
  // Disable CADB
  //
  cpgcPatCadbCtlReg.Data = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);

  //
  // Program WDB data with constant 0's or 1's
  //
  if ((~pattern) & DDR_CPGC_PPR_TEST) {
    wdbData = 0;
    if (pattern & DDR_CPGC_MEM_TEST_ALL0) {
      wdbData = 0;
    } else if (pattern & DDR_CPGC_MEM_TEST_A5) {
      wdbData = 0xA5A5A5A5;
    }
    // Invert pattern
    if (pattern & DDR_CPGC_MEM_TEST_INVERTPAT) {
      wdbData = ~wdbData;
    }

    WriteWDBFixedPattern(host, socket, ch, wdbData, 8, 0);
  } else {
    // PPR test pattern
    if (pattern & DDR_CPGC_MEM_TEST_INVERTPAT) {
      FillWdbPprPattern (host, socket, ch, dimm, ~dramMask);
    } else {
      FillWdbPprPattern (host, socket, ch, dimm, dramMask);
    }
  }

  return status;

} // CpgcPprTestGlobalSetup


UINT32
CpgcPprCleanup (
  PSYSHOST  host,
  UINT8     socket,
  UINT32    chBitMask
  )
/*++

  Clean-up after PPR sequence is done

  @param host        - Pointer to sysHost
  @param socket      - Memory controller to train
  @param chBitMask   - Bit map of channels to mask off
--*/
{
  UINT8 ch;
  struct channelNvram  (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    //Check if this channel has been masked off as each BSSA Call is per MC
    if ((chBitMask & (1 << ch)) == 0) continue;
    if ((*channelNvList)[ch].enabled == 0) continue;
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV_MCDDC_DP_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV0_MCDDC_DP_REG, 0);
    MemWritePciCfgEp (host, socket, ch, CPGC_PATWDB_INV1_MCDDC_DP_REG, 0);
  } // ch loop

  return SUCCESS;
}

/**
  This function determines if cpgc is targeting an NVMDIMM Rank

  @param host:         Pointer to sysHost
  @param socket:       Socket Id
  @param ch:           DDR channel number within the processor socket

  @retval 1:           Rank belongs to AEP Dimm.
  @retval 0:           Rank belongs to DDR4 Dimm
**/

static UINT8
IsAepRank(
          PSYSHOST host,
          UINT8    socket,
          UINT8    ch,
          UINT8    logRank
)
{
  UINT8            dimm;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  dimm = logRank / MAX_RANK_DIMM;
  return ((*dimmNvList)[dimm].aepDimmPresent);
}

void
SetupPmon(
PSYSHOST        host,
UINT8           socket,
UINT8           ch,
UINT8           eventSelect,
UINT8           unitMask
)
{
    PMONCNTRCFG_0_MCDDC_CTL_STRUCT  pmonCntrCfg;
    PMONCNTR_0_N0_MCDDC_CTL_STRUCT  pmonCntr;

    pmonCntr.Data = 0;
    MemWritePciCfgEp (host, socket, ch, PMONCNTR_0_N0_MCDDC_CTL_REG, pmonCntr.Data);

    MemWritePciCfgEp (host, socket, ch, PMONCNTR_4_N0_MCDDC_CTL_REG, pmonCntr.Data);


    pmonCntrCfg.Data = 0;
    pmonCntrCfg.Bits.eventselect = eventSelect;
    pmonCntrCfg.Bits.unitmask = unitMask;
    pmonCntrCfg.Bits.counterenable = 1; // enable counter
    MemWritePciCfgEp (host, socket, ch, PMONCNTRCFG_0_MCDDC_CTL_REG, pmonCntrCfg.Data);

    MemWritePciCfgEp (host, socket, ch, PMONCNTRCFG_4_MCDDC_CTL_REG, pmonCntrCfg.Data);

}

static UINT32
GetPmonCounter(
               PSYSHOST  host,
               UINT8     socket,
               UINT8     ch
)
{
  UINT32                          count;
  PMONCNTRCFG_4_MCDDC_CTL_STRUCT  pmonCntrCfg;

  pmonCntrCfg.Data = MemReadPciCfgEp (host, socket, ch, PMONCNTRCFG_4_MCDDC_CTL_REG);
  pmonCntrCfg.Bits.counterreset = 1;
  MemWritePciCfgEp (host, socket, ch, PMONCNTRCFG_4_MCDDC_CTL_REG, pmonCntrCfg.Data);


  // read counter
  count = MemReadPciCfgEp (host, socket, ch, PMONCNTR_4_N0_MCDDC_CTL_REG);
  return count;
}

UINT32
GetTemporalPatternChip(
  PSYSHOST  host
)
{
  UINT32  temporalPattern;
  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    temporalPattern = CLOCK_PATTERN_DDR4;
  } else {
    temporalPattern = CLOCK_PATTERN_DDR3;
  }
  return temporalPattern;
}

UINT8
isHynix2013(
PSYSHOST host,
UINT8    socket,
UINT8    ch
)
{
  UINT8 dimm;
  CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_STRUCT cpgcSeqRanklg2phymap;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  cpgcSeqRanklg2phymap.Data = MemReadPciCfgMC_Ch(host, socket, ch, CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG, 4);
  dimm = (UINT8) cpgcSeqRanklg2phymap.Bits.rank0_mapping / MAX_RANK_DIMM;

  if (((*dimmNvList)[dimm].SPDDramMfgId == MFGID_HYNIX) && (((*dimmNvList)[dimm].SPDOtherOptFeatures & SPD_PPR_MASK) == 0) && (IsLrdimmPresent(host, socket, ch, dimm) == 0)) {
    return 1;
  } else {
    return 0;
  }
}
UINT8
isDdrtDimm(
           PSYSHOST host,
           UINT8    socket,
           UINT8    ch
)
{
  CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_STRUCT cpgcSeqRanklg2phymap;

  cpgcSeqRanklg2phymap.Data = MemReadPciCfgMC_Ch(host, socket, ch, CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG, 4);
  return (IsAepRank(host, socket, ch, (UINT8) cpgcSeqRanklg2phymap.Bits.rank0_mapping));
}

void
DdrtWriteFlyBy(
PSYSHOST host,
UINT8    socket,
UINT8    ch,
UINT8    *cmd,
UINT32   *seqCtl0
)
{
  CPGC_SEQCTL0_MC_MAIN_STRUCT seqCtl0data;

  seqCtl0data.Data = *seqCtl0;

  if (*cmd == PatRdWrRd) {
    if(isDdrtDimm(host, socket, ch) || isHynix2013(host, socket, ch)) {
      *cmd = PatWrRd;
      seqCtl0data.Bits.subseq_end_pointer = 1;
      *seqCtl0 = seqCtl0data.Data;
    }
  }
}

VOID
DisableCADBAlwaysOnMode(
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch
)
{
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;
 
  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG);
  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on = 0;
  MemWritePciCfgEp (host, socket, ch, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);	
}