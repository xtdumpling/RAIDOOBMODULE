/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
  Copyright (c) 2013-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  CpgcPointTest.c

@brief
  This file contains code for a downloadable DLL that performs a 1D margin test.
**/
#ifdef SSA_FLAG

#define BSSA_LOGGING_LEVEL BSSA_LOGGING_INTERNAL_DEBUG_LEVEL

#if defined(__STUB__) || defined(HEADLESS_MRC)
#include <assert.h>
#endif // __STUB__

#include "ssabios.h"
#include "bssalogging.h"
#include "BitMask.h"
#include "ChkRetStat.h"
#include "GetActualMallocSize.h"
#include "GetMemPoolSize.h"
#include "LogByteArrayAsHex.h"
#include "MemPointTest.h"
#include "CpgcPointTest.h"

// flag to enable BIOS-SSA debug messages
#define ENBL_BIOS_SSA_API_DEBUG_MSGS (0)

// flag to enable function trace debug messages
#define ENBL_FUNCTION_TRACE_DEBUG_MSGS (0)

// flag to enable verbose log messages
#define ENBL_VERBOSE_LOG_MSGS (0)

// flag to enable verbose error messages
#define ENBL_VERBOSE_ERROR_MSGS (0)

// flag to enable memory pool debug log messages
#define ENBL_MEM_POOL_DEBUG_LOG_MSGS (0)

// flag to enable DDR-T debug messages
#define ENBL_DDRT_DEBUG_LOG_MSGS (0)

// flag to enable DqDB debug messages
#define ENBL_DQDB_DEBUG_LOG_MSGS (0)

// flag to enable timeout debug messages
#define ENBL_TIMEOUT_DEBUG_LOG_MSGS (0)

#pragma pack(push, 1)

//
// shorthand CPGC Subsequence types
//
#ifndef BRd
#define BRd    (BaseReadSubseqType)
#endif // BRd
#ifndef BWr
#define BWr    (BaseWriteSubseqType)
#endif // BWr
#ifndef BRdWr
#define BRdWr  (BaseReadWriteSubseqType)
#endif // BRdWr
#ifndef BWrRd
#define BWrRd  (BaseWriteReadSubseqType)
#endif // BWrRd
#ifndef ORd
#define ORd    (OffsetReadSubseqType)
#endif // ORd
#ifndef OWr
#define OWr    (OffsetWriteSubseqType)
#endif // OWr


// memory point test data
typedef struct {
  // current DIMM and rank A/B values
  DIMM_RANK_PAIR CrntDimmRankA;
  DIMM_RANK_PAIR CrntDimmRankB;
  // current chunk validation mask, we need it for the GetDqErrorStatus()
  UINT8 CrntChunkSelectMask;
} CPGC_PER_CHNL_POINT_TEST_DATA;

// memory point test data
typedef struct {
  // test status logging level
  UINT8 TestStatusLogLevel;

  // the maximum number of rank combinations
  UINT8 MaxRankCombinationsCnt;

  // dynamically allocated array of per channel rank and chunk mask status
  // indexed based on the tested channel index starting from socket->controller->channel 
  UINT8 TestedChnlCnt;
  CPGC_PER_CHNL_POINT_TEST_DATA *pChannelTestData;

  // the number of rank combinations per channel
  RANK_COMBINATION_CNTS RankCombinationsCnts;

  // the current rank combination index
  UINT8 RankCombinationsIndex;

  // lane failure status; indexed as [Socket][Controller][Channel]
  LANE_FAIL_STATS LaneFailStats;

  // flag indicating that at we're in the binary search state
  BOOLEAN InBinarySearchState;

  // function pointer for result handler
  MEM_POINT_TEST_RESULT_HANDLER ResultHandler;

  // pointer to client data - used during callbacks to client
  VOID *pClientTestData;

  // values saved from call to SetupCpgcPointTest function
  GSM_LT IoLevel;
  BOOLEAN IsDdrT;

#if SUPPORT_FOR_TURNAROUNDS
  // address schemas for turnaround traffic
  CPGC_ADDRESS TurnaroundCpgcAddr0; // for Wr2WrAndRd2Rd and the first part of Wr2Rd
  CPGC_ADDRESS TurnaroundCpgcAddr1; // for Rd2Wr and the second part of Wr2Rd
#endif // SUPPORT_FOR_TURNAROUNDS
} CPGC_POINT_TEST_DATA;

typedef union {
  UINT32 Uint32[2];
  UINT64 Uint64;
} UNION_64;

#pragma pack(pop)

//
// Sequencer set up for different traffic mode
//
CONST CPGC_SEQUENCER SequencersByTrafficMode[TrafficModeMax] = {
  // SubseqenceStart, SubsequenceStop, SubsequenceType[MAX_SUBSEQ_CNT], ResetBaseAdr[MAX_SUBSEQ_CNT]
  {0, 1, {  BWr,   BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, TRUE, TRUE}}, // LoopbackTrafficMode
  {0, 7, {  BWr,   BWr, BWr, BWr, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE, FALSE, FALSE, FALSE,  TRUE, FALSE, FALSE, FALSE, TRUE, TRUE}}, // WritesAndReadsTrafficMode
  {0, 0, {  BWr,   BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, TRUE, TRUE}}, // WriteOnlyTrafficMode
  {0, 0, {  BRd,   BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, TRUE, TRUE}}, // ReadOnlyTrafficMode
  {0, 0, {  BWr,   BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, TRUE, TRUE}}, // IdleTrafficMode -> No traffic will be generated for the given channel.
  {0, 1, {  BWr,   BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, TRUE, TRUE}}, // Wr2WrAndRd2RdTrafficMode
  {0, 0, {BWrRd,   BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, TRUE, TRUE}}, // Rd2WrTrafficMode
  {0, 1, {  BWr, BRdWr, BRd, BRd, BRd, BRd, BRd, BRd, BRd, BRd},{TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE, TRUE, TRUE}}, // Wr2RdTrafficMode
};

/**
@brief
  This function rotates the given 64-bit value to the left the given number of
  bits.  This function returns the rotated value.

  @param[in]  Pattern   Pattern to rotate.
  @param[in]  BitCount  Number of bit positions to rotate.
**/
UINT64 RotateDqdbChunk(
  IN UINT64 Pattern,
  IN UINT8 BitCount)
{
  UINT64 Shifted, Wrapped;

  if (BitCount >= 64) {
    BitCount %= 64;
  }

  // the shifted portion
  Shifted = Pattern << BitCount;

  // the wrapped around portion
  Wrapped = Pattern >> (64 - BitCount);

  return Shifted | Wrapped;
} // end function RotateDqdbChunk

/**
@brief
  This function is used to set the given bits in the given buffer according to
  the given temporal pattern.

  @param[in]   SpatialMask         Spatial mask of bits to set in buffer.
  @param[in]   TemporalPattern     Temporal pattern.
  @param[in]   TemporalPatternLen  Temporal pattern length.
  @param[out]  pBfr                Pointer to buffer.
**/
VOID SetSpatialPattern(
  IN UINT64 SpatialMask,
  IN UINT8 TemporalPattern,
  IN UINT8 TemporalPatternLen,
  OUT UINT64* pBfr)
{
  UINT8 BfrIdx;
  // FOR each temporal pattern element:
  for (BfrIdx = 0; BfrIdx < TemporalPatternLen; BfrIdx++) {
    // IF this temporal pattern element is set:
    if ((TemporalPattern & (1 << BfrIdx)) != 0) {
      // set the given bits in the corresponding buffer element
      pBfr[BfrIdx] |= SpatialMask;
    }
    else {
      // clear the given bits in the corresponding buffer element
      pBfr[BfrIdx] &= ~SpatialMask;
    }
  }
} // end function SetSpatialPattern

/**
@brief
This function is used to initialize the ERID pattern.

@param[in, out]  SsaServicesHandle  Pointer to SSA services.
@param[in]       pSystemInfo        Pointer to system information structure.
@param[in]       pCpgcInfo          Pointer to CPGC information structure.
@param[in]       pMemCfg            Pointer to memory configuration structure.
@param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
@param[in]       pCpgcPointTestCfg  Pointer to CPGC point test configuration structure.
@param[out]      pTestStat           Pointer to return status in results metadata structure.

@retval  0  success
@retval  1  error occurred
**/
static VOID InitEridPattern(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg)
{
  UINT8 Socket, Controller, Channel;
  ERID_SIGNAL Signal;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitEridPattern()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        // IF ERID test is enabled.
        if (!pCpgcPointTestCfg->EnableErid) {
          continue;
        }

        // FOR each Early-Read-ID signal:
        for (Signal = 0; Signal < EridSignalMax; Signal++) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SetEridPattern(Socket=%u Controller=%u Channel=%u Signal=%u\n",
            Socket, Controller, Channel, Signal);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "    PatternMode=%u EnableReload=%u EnableReset=%u Seed=0x%X)\n",
            EridLfsrMode, 0, 0, pCpgcPointTestCfg->EridPatternSeeds[Controller][Channel][Signal]);
#endif
          SsaServicesHandle->SsaMemoryDdrtConfig->SetEridPattern(
            SsaServicesHandle, Socket, Controller, Channel, Signal, EridLfsrMode, 
            0, 0, pCpgcPointTestCfg->EridPatternSeeds[Controller][Channel][Signal]);
        } // end of each Early-Read-ID signal
      } // end for each channel
    } // end for each controller
  } // end for each socket
} // end function InitEridPattern

/**
@brief
This function is used to initialize the ERID validation masks.

Note that only channels selected for testing are set up.

@param[in, out]  SsaServicesHandle  Pointer to SSA services.
@param[in]       pSystemInfo        Pointer to system information structure.
@param[in]       pMemCfg            Pointer to memory configuration structure.
@param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
**/
static VOID InitEridValidation(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg)
{
  UINT8 Socket, Controller, Channel;
  UINT8 EridErrCheckingMask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitEridValidation()\n");
#endif

  // IF ERID test is not enabled.
  if (!pCpgcPointTestCfg->EnableErid) {
    return;
  }

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        EridErrCheckingMask = 0;

        // convert the lane validation mask to ERID error checking mask.
        // even and odd bits correspond to ERID0 and 1
        // bit 0 corresponding to even ERID UIs
        if (pMemPointTestCfg->LaneValBitmasks[Controller][Channel][0] & 0x1){
          EridErrCheckingMask |= 0x55;
        }
        // bit 1 corresponding to odd ERID UIs
        // bit 7 is parity bit, we need to skip it.
        if (pMemPointTestCfg->LaneValBitmasks[Controller][Channel][0] & 0x2){
          EridErrCheckingMask |= 0x2A;
        }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetEridValidationMask(Socket=%u Controller=%u Channel=%u EridMask=0x%X\n",
          Socket, Controller, Channel,
          EridErrCheckingMask);
#endif
        SsaServicesHandle->SsaMemoryDdrtConfig->SetEridValidationMask(
          SsaServicesHandle, Socket, Controller, Channel,
          EridErrCheckingMask);
      } // end for each channel
    } // end for each controller
  } // end for each socket
} // end function InitEridValidation

/**
@brief
  This function is used to initialize the DqDB pattern and unisequencers.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pCpgcInfo          Pointer to CPGC information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg  Pointer to CPGC point test configuration structure.
  @param[in]       InitDqdbContent    Flag indicating whether to initialize the DQDB contents.
  @param[out]      pTestStat          Pointer to return status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 InitDqdb(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MRC_SERVER_CPGC_INFO *pCpgcInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  IN BOOLEAN InitDqdbContent,
  OUT TEST_STATUS *pTestStat)
{
  SSA_STATUS RetStat;
  UINT8 Socket, Controller, Channel, Uniseq;
  UINT8 CachelineCount;
  UINT32 BfrSize;
  UINT64* pPattern;
  UINT8 RotationIdx, RotationCnt;
  UINT8 Chunk;
  UINT64 VictimMask, AggressorMask, NeitherMask, BothMask;
  UINT8 StartIndex = 0, EndIndex;
  UINT8 LDataSel = 0;
  BOOLEAN EnableFreqSweep = FALSE;
  UINT8 DcInvMask[9];
  UINT8 StartCachelineIndex;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "CpgcPointTest.InitDqdb()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        // IF the user defined DqDB pattern is enabled
        if (pCpgcPointTestCfg->EnableUserDefinedDqdbPatterns) {
          EndIndex = (pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] * \
            (pCpgcPointTestCfg->UserDefinedDqbdPatternRotationCnts[Channel] + 1)) - 1;
        }
        // ELSE use the BKM "victim/aggressor" pattern
        else {
          EndIndex = 7;
        }

        // IF DQDB content initialization is specified
        if (InitDqdbContent) {
          CachelineCount = EndIndex + 1;

          BfrSize = CachelineCount * CHUNKS_PER_CACHELINE * sizeof(UINT64);
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
          GetMemPoolSize(SsaServicesHandle);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  Malloc(Size=%u) for DqDB buffer\n", BfrSize);
#endif
          pPattern = (UINT64*)SsaServicesHandle->SsaCommonConfig->Malloc(
            SsaServicesHandle, BfrSize);
          if (pPattern == NULL) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: Could not allocate memory for DqDB pattern buffer.\n");
#endif
            ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
            return 1;
          }
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "    pBfr=%p\n", pPattern);
          GetMemPoolSize(SsaServicesHandle);
#endif
          // IF the user defined DqDB pattern is enabled
          if (pCpgcPointTestCfg->EnableUserDefinedDqdbPatterns) {
            // copy the pattern into the temporary space
            bssa_memcpy(pPattern, pCpgcPointTestCfg->UserDefinedDqdbPatterns[Channel],
              (pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] *
              CHUNKS_PER_CACHELINE * sizeof(UINT64)));

            // FOR each DqDB pattern rotation:
            for (RotationIdx = 1;
              RotationIdx <= pCpgcPointTestCfg->UserDefinedDqbdPatternRotationCnts[Channel];
              RotationIdx++) {
                // FOR each chunk of the pattern
                for (Chunk = 0; Chunk < (pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] * CHUNKS_PER_CACHELINE); Chunk++) {
                  // rotate the pattern n bits to the left
                  pPattern[(RotationIdx * pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] * CHUNKS_PER_CACHELINE) + Chunk] = \
                    RotateDqdbChunk(*((UINT64*)
                    pCpgcPointTestCfg->UserDefinedDqdbPatterns[Channel][Chunk / CHUNKS_PER_CACHELINE][Chunk % CHUNKS_PER_CACHELINE]),
                    RotationIdx);

#if ENBL_DQDB_DEBUG_LOG_MSGS
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                    "  RotationIdx=%u CachelineIdx=%u Chunk=%u PatternIdx=%u\n",
                    RotationIdx, (Chunk / CHUNKS_PER_CACHELINE),
                    (Chunk % CHUNKS_PER_CACHELINE),
                    ((RotationIdx * pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] * CHUNKS_PER_CACHELINE) + Chunk));
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "  OriginalPattern=0x");
                  LogByteArrayAsHex(SsaServicesHandle,
                    pCpgcPointTestCfg->UserDefinedDqdbPatterns[Channel][Chunk / CHUNKS_PER_CACHELINE][Chunk % CHUNKS_PER_CACHELINE],
                    sizeof(UINT64));
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, " RotatedPattern=0x");
                  LogByteArrayAsHex(SsaServicesHandle,
                    (UINT8*) &pPattern[(RotationIdx * pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] * CHUNKS_PER_CACHELINE) + Chunk],
                    sizeof(UINT64));
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "\n");
#endif // ENBL_DQDB_DEBUG_LOG_MSGS
                } // end for each chunk of the pattern
            } // end for each DqDB pattern rotation
          }
          // ELSE use the BKM "victim/aggressor" pattern
          else {
            bssa_memset(pPattern, 0, BfrSize);

            RotationCnt   = CachelineCount - 1;
            VictimMask    = 0x0101010101010101;
            AggressorMask = 0xFEFEFEFEFEFEFEFE;

            // FOR each pattern rotation (+1):
            for (RotationIdx = 0; RotationIdx <= RotationCnt; RotationIdx++) {
              if (RotationIdx != 0) {
                VictimMask = RotateDqdbChunk(VictimMask, 1);
                AggressorMask = RotateDqdbChunk(AggressorMask, 1);
              }

              // get the mask of lanes that aren't set in either mask
              NeitherMask = ~(VictimMask | AggressorMask);

              // get the mask of lanes that are set in both masks
              BothMask = VictimMask & AggressorMask;

              // eliminate the lanes that are set in both masks from the victim
              // and aggressor masks
              VictimMask ^= BothMask;
              AggressorMask ^= BothMask;

              if (VictimMask) {
                SetSpatialPattern(VictimMask, 0xF0, CHUNKS_PER_CACHELINE,
                  &pPattern[RotationIdx * CHUNKS_PER_CACHELINE]);
              }
              if (AggressorMask) {
                SetSpatialPattern(AggressorMask, 0xAA, CHUNKS_PER_CACHELINE,
                  &pPattern[RotationIdx * CHUNKS_PER_CACHELINE]);
              }
              if (NeitherMask) {
                SetSpatialPattern(NeitherMask, 0xCC, CHUNKS_PER_CACHELINE,
                  &pPattern[RotationIdx * CHUNKS_PER_CACHELINE]);
              }
            } // end for each pattern rotation (+1)
          } // end else use the BKM "victim/aggressor" pattern

#if ENBL_DQDB_DEBUG_LOG_MSGS
          {
            UINT8 CachelineIndex;

            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  DqDB pattern:\n");
            // FOR each cacheline:
            for (CachelineIndex = 0; CachelineIndex < CachelineCount; CachelineIndex++) {
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "    Cacheline=%u\n", CachelineIndex);
              // FOR each chunk:
              for (Chunk = 0; Chunk < CHUNKS_PER_CACHELINE; Chunk++) {
                NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                  "      Chunk=%u: 0x%08X%08X\n", Chunk,
                  ((UNION_64*) &(pPattern[(CachelineIndex * CHUNKS_PER_CACHELINE) + Chunk]))->Uint32[1],
                  ((UNION_64*) &(pPattern[(CachelineIndex * CHUNKS_PER_CACHELINE) + Chunk]))->Uint32[0]);
              } // end for each chunk
            } // end for each cacheline
          }
#endif // ENBL_DQDB_DEBUG_LOG_MSGS

          StartCachelineIndex = 0;
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DQDB_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SetDqdbPattern(Socket=%u Controller=%u Channel=%u\n",
            Socket, Controller, Channel);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "    CachelineCnt=%u StartCachelineIndex=%u)\n",
            CachelineCount, StartCachelineIndex);
#endif // ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DQDB_DEBUG_LOG_MSGS
          SsaServicesHandle->SsaMemoryServerConfig->SetDqdbPattern(
            SsaServicesHandle, Socket, Controller, Channel, pPattern,
            CachelineCount, StartCachelineIndex);

#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
          GetMemPoolSize(SsaServicesHandle);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  Free(pBfr=%p)\n", pPattern);
#endif
          SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle, pPattern);
          pPattern = NULL;
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
          GetMemPoolSize(SsaServicesHandle);
#endif
        } // end if DQDB content initialization is specified

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetDqdbIndexes(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    StartIndex=%u EndIndex=%u IncRateMode=%u IncRate=%u)\n",
          StartIndex, EndIndex,
          pCpgcPointTestCfg->DqdbIncRateModes[Channel],
          pCpgcPointTestCfg->DqdbIncRates[Channel]);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetDqdbIndexes(
          SsaServicesHandle, Socket, Controller, Channel, StartIndex, EndIndex,
          pCpgcPointTestCfg->DqdbIncRateModes[Channel],
          pCpgcPointTestCfg->DqdbIncRates[Channel]);

        // FOR each DqDB unisequencer:
        for (Uniseq = 0; Uniseq < MAX_DQDB_UNISEQ_CNT; Uniseq++) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SetDqdbUniseqMode(Socket=%u Controller=%u Channel=%u Uniseq=%u Mode=%d)\n",
            Socket, Controller, Channel, Uniseq,
            pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq]);
#endif
          RetStat = SsaServicesHandle->SsaMemoryServerConfig->SetDqdbUniseqMode(
            SsaServicesHandle, Socket, Controller, Channel, Uniseq,
            pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq]);
          if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
            return 1;
          }

          // IF the unisequencer mode is "LMN"
          if ((Uniseq < MAX_CADB_LMN_UNISEQ_CNT) &&
            (pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq] == LmnDqdbUniseqMode)) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetDqdbUniseqLmn(Socket=%u Controller=%u Channel=%u Uniseq=%u L=%d M=%d N=%d LDataSel=0)\n",
              Socket, Controller, Channel, Uniseq,
              pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][0],
              pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][1],
              pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][2]);
#endif
            RetStat = SsaServicesHandle->SsaMemoryServerConfig->SetDqdbUniseqLmn(
              SsaServicesHandle, Socket, Controller, Channel, Uniseq,
              pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][0],
              pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][1],
              pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][2],
              LDataSel, EnableFreqSweep);
            if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
              return 1;
            }
          }
          // ELSE the unisequencer mode is "LFSR" or "buffer"
          else {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetDqdbUniseqWrSeed(Socket=%u Controller=%u Channel=%u Uniseq=%u Seed=0x%06X)\n",
              Socket, Controller, Channel, Uniseq,
              pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);
#endif
            SsaServicesHandle->SsaMemoryServerConfig->SetDqdbUniseqWrSeed(
              SsaServicesHandle, Socket, Controller, Channel, Uniseq,
              pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetDqdbUniseqRdSeed(Socket=%u Controller=%u Channel=%u Uniseq=%u Seed=0x%06X)\n",
              Socket, Controller, Channel, Uniseq,
              pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);
#endif
            SsaServicesHandle->SsaMemoryServerConfig->SetDqdbUniseqRdSeed(
              SsaServicesHandle, Socket, Controller, Channel, Uniseq,
              pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);
          } // end else the mode is "LFSR" or "buffer"
        } // end for each DqDB unisequencer

        // init/hardcode the ECC source to byte 0, DC_INV mask to 0
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetDqdbEccDataSource(Socket=%u Controller=%u Channel=%u EccStatSource=%u)\n",
          Socket, Controller, Channel, ByteGroup0);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetDqdbEccDataSource(
          SsaServicesHandle, Socket, Controller, Channel, ByteGroup0);

        bssa_memset(&DcInvMask[0], 0, 9);
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetDqdbInvertDcConfig(Socket=%u Controller=%u Channel=%u\n"
          "    LaneMasks=0x",
          Socket, Controller, Channel);
        LogByteArrayAsHex(SsaServicesHandle, DcInvMask, sizeof(DcInvMask));
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "   Mode=%u DcPolarity=%u ShiftRateExponent=%u)\n",
          InvertMode, 0, pCpgcInfo->MaxDqdbInvertDcShiftRateExponentVal);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetDqdbInvertDcConfig(
          SsaServicesHandle, Socket, Controller, Channel, (const UINT8*)DcInvMask,
          InvertMode, 0,0);
      } // end for each channel
    } // end for each controller
  } // end for each socket

  return 0;
} // end function InitDqdb

/**
@brief
  This function is a helper function that is used to set the logical rank map
  for a give DIMM/rank pairs

  @param[in, out]  SsaServicesHandle   Pointer to SSA services.
  @param[in]       Socket              Zero based socket number.
  @param[in]       Controller          Zero based controller number.
  @param[in]       Channel             Zero based channel number.
  @param[in]       RankCombinationMode Singles/Turnaround
  @param[in]       DimmRankA           Dimm/Rank pair A
  @param[in]       DimmRankB           Dimm/Rank pair B
**/
static VOID SetRankMap(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN RANK_TEST_MODE RankTestMode,
  IN DIMM_RANK_PAIR DimmRankA,
  IN DIMM_RANK_PAIR DimmRankB)
{
  UINT8 PairCount;
  DIMM_RANK_PAIR DimmRankPairs[8];

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.SetRankMap(Socket=%u Controller=%u Channel=%u RankTestMode=%u",
    Socket, Controller, Channel, RankTestMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    " DimmA=%u RankA=%u DimmB=%u RankB=%u)\n",
    DimmRankA.Dimm, DimmRankA.Rank, DimmRankB.Dimm, DimmRankB.Rank);
#endif

#if SUPPORT_FOR_TURNAROUNDS
  // IF this channel is configured to do rank-to-rank turnaround testing
  if (RankTestMode == Turnarounds) {
    PairCount = 2;
    DimmRankPairs[0].Dimm = DimmRankA.Dimm;
    DimmRankPairs[0].Rank = DimmRankA.Rank;
    DimmRankPairs[1].Dimm = DimmRankB.Dimm;
    DimmRankPairs[1].Rank = DimmRankB.Rank;
  }
  // ELSE this channel is configured to do single rank testing
  else
#endif // SUPPORT_FOR_TURNAROUNDS
  {
    PairCount = 1;
    DimmRankPairs[0].Dimm = DimmRankA.Dimm;
    DimmRankPairs[0].Rank = DimmRankA.Rank;
  }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
  {
    UINT8 Idx;

    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  SetSeqRankMap(Socket=%u Controller=%u Channel=%u PairCount=%u",
      Socket, Controller, Channel, PairCount);

    for (Idx = 0; Idx < PairCount; Idx++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "\n    Idx=%u Dimm=%u Rank=%u",
        Idx, DimmRankPairs[Idx].Dimm, DimmRankPairs[Idx].Rank);
    }

    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      ")\n");
  }
#endif
  SsaServicesHandle->SsaMemoryServerConfig->SetSeqRankMap(
    SsaServicesHandle, Socket, Controller, Channel, PairCount, DimmRankPairs);

} // end function SetRankMap

/**
@brief
  This function is used to set the logical ranks based on the given test data.

  @param[in, out]  SsaServicesHandle   Pointer to SSA services.
  @param[in]       pSystemInfo         Pointer to system information structure.
  @param[in]       pMemCfg             Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg    Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg   Pointer to CPGC memory point test configuration structure.
  @param[in]       pCpgcPointTestData  Pointer to CPGC point test data structure.
  @param[out]      pTestStat           Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 SetCrntRanks(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  IN CPGC_POINT_TEST_DATA *pCpgcPointTestData,
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller, Channel;
  DIMM_RANK_PAIR CrntDimmRankA, CrntDimmRankB;
  UINT8 TestedChnlIdx;
  BOOLEAN EnableDdr4; // flag for DDR4 or DDR-T

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.SetCrntRanks()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
            continue;
        }
        // get the index of tested channel.
        TestedChnlIdx = GetTestChnlIndex(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel);

        //NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        //  "CpgcPointTest.SetCrntRanks(TestChnlIdx=%u)\n", TestedChnlIdx);

        CrntDimmRankA = pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA;
        CrntDimmRankB = pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankB;

#if SUPPORT_FOR_TURNAROUNDS
        // IF this channel is configured to do rank-to-rank turnaround testing
        if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] ==
          Turnarounds) {
          // make sure the DIMM+rank pairs are different
          if ((CrntDimmRankA.Dimm == CrntDimmRankB.Dimm) &&
            (CrntDimmRankA.Rank == CrntDimmRankB.Rank)) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: Rank-to-rank turnaround DIMM+rank pairs are the same.\n");
#endif
            ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
            return 1;
          }
       } // end if this channel is configured to do rank-to-rank turnaround testing
#endif // SUPPORT_FOR_TURNAROUNDS

        // code to set up the MC DDR4/DDR-T switch
        // Because DDR-T and DDR4 can't be mixed, assuming the code that generate and
        // checking the mem_cfg already ensure that there is no mix, we just need to
        // check the DimmRank4.
        if (pMemCfg->MemoryTech[Socket][Controller][Channel][CrntDimmRankA.Dimm] != SsaMemoryDdrT) {
          // DDR4
          EnableDdr4 = TRUE;
        }
        else {
          // DDR-T
          EnableDdr4 = FALSE;
        }

          // Set up the MC address scheme as DDR4 or DDR-T
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SetAddrConfig(Socket=%u Controller=%u Channel=%u EnableDdr4=%u)\n",
            Socket, Controller, Channel, EnableDdr4);
#endif
          SsaServicesHandle->SsaMemoryServerConfig->SetAddrConfig(
            SsaServicesHandle, Socket, Controller, Channel, EnableDdr4);

          SetRankMap(SsaServicesHandle, Socket, Controller, Channel,
            pMemPointTestCfg->RankTestModes[Socket][Controller][Channel],
            CrntDimmRankA, CrntDimmRankB);
      } // end for each channel
    } // end for each controller
  } // end for each socket
  return 0;
} // end function SetCrntRanks

/**
@brief
  This function is used to set a subsequence configuration.

  @param[in, out]  SsaServicesHandle                   Pointer to SSA services.
  @param[in]       Socket                              Zero based socket number.
  @param[in]       Controller                          Zero based memory controller number.
  @param[in]       Channel                             Zero based memory channel number.
  @param[in]       Subseq                              Zero based subsequence number.
  @param[in]       BurstLengthMode                     Burst length mode.
  @param[in]       BurstLength                         Burst length.
  @param[in]       InterSubseqWait                     Inter-subsequence wait in dclks.
  @param[in]       SubseqType                          Subsequence type.
  @param[in]       Subseq                              Zero based subsequence number.
  @param[in]       EnableResetCurrentBaseAddrToStart   Specifies whether the current base address is reset to the start address with this subsequence.
**/
static VOID SetSubseqConfig(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 Subseq,
  IN BURST_LENGTH_MODE BurstLengthMode,
  IN UINT32 BurstLength,
  IN UINT32 InterSubseqWait,
  IN SUBSEQ_TYPE SubseqType,
  IN BOOLEAN EnableResetCurrentBaseAddrToStart)
{
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  SetSubseqConfig(Socket=%u Controller=%u Channel=%u Subseq=%u\n",
    Socket, Controller, Channel, Subseq);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    BurstLengthMode=%u BurstLength=%u InterSubseqWait=%u SubseqType=%u\n",
    BurstLengthMode, BurstLength, InterSubseqWait, SubseqType);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    EnableSaveCurrentBaseAddrToStart=%u EnableResetCurrentBaseAddrToStart=%u\n",
    FALSE, EnableResetCurrentBaseAddrToStart);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    EnableAddrInversion=%u EnableDataInversion=%u)\n",
    FALSE, FALSE);
#endif
  SsaServicesHandle->SsaMemoryServerConfig->SetSubseqConfig(
    SsaServicesHandle, Socket, Controller, Channel, Subseq, BurstLengthMode,
    BurstLength, InterSubseqWait, SubseqType, FALSE,
    EnableResetCurrentBaseAddrToStart, FALSE, FALSE);
} // end function SetSubseqConfig

/**
@brief
  This function is used to set up address start, wrap and increment value.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based memory controller number.
  @param[in]       Channel            Zero based memory channel number.
  @param[in]       pAddrScheme        Specifies the rank, bank, row and col address scheme.
**/
static VOID SetSeqAddr(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN CONST CPGC_ADDRESS *pAddrScheme)
{
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  SetSeqStartAddr(Socket=%u Controller=%u Channel=%u\n",
    Socket, Controller, Channel);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    Rank=0x%X Bank=0x%X Row=0x%X Col=0x%X)\n",
    pAddrScheme->Start[RankAddrField], pAddrScheme->Start[BankAddrField],
    pAddrScheme->Start[RowAddrField], pAddrScheme->Start[ColAddrField]);
#endif
  SsaServicesHandle->SsaMemoryServerConfig->SetSeqStartAddr(
    SsaServicesHandle, Socket, Controller, Channel,
    (UINT8)pAddrScheme->Start[RankAddrField],
    (UINT8)pAddrScheme->Start[BankAddrField],
    pAddrScheme->Start[RowAddrField],
    pAddrScheme->Start[ColAddrField]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  SetSeqWrapAddr(Socket=%u Controller=%u Channel=%u\n",
    Socket, Controller, Channel);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    Rank=0x%X Bank=0x%X Row=0x%X Col=0x%X)\n",
    pAddrScheme->Wrap[RankAddrField], pAddrScheme->Wrap[BankAddrField],
    pAddrScheme->Wrap[RowAddrField], pAddrScheme->Wrap[ColAddrField]);
#endif
  SsaServicesHandle->SsaMemoryServerConfig->SetSeqWrapAddr(
    SsaServicesHandle, Socket, Controller, Channel,
    (UINT8)pAddrScheme->Wrap[RankAddrField],
    (UINT8)pAddrScheme->Wrap[BankAddrField],
    pAddrScheme->Wrap[RowAddrField],
    (UINT16)pAddrScheme->Wrap[ColAddrField]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  SetSeqAddrInc(Socket=%u Controller=%u Channel=%u\n",
    Socket, Controller, Channel);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    RankIncMode=%u RankIncRate=%u RankIncVal=%d\n",
    pAddrScheme->IncMode[RankAddrField], pAddrScheme->IncRate[RankAddrField],
    pAddrScheme->IncVal[RankAddrField]);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    BankIncMode=%u BankIncRate=%u BankIncVal=%d\n",
    pAddrScheme->IncMode[BankAddrField], pAddrScheme->IncRate[BankAddrField],
    pAddrScheme->IncVal[BankAddrField]);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    RowIncMode=%u RowIncRate=%u RowIncVal=%d\n",
    pAddrScheme->IncMode[RowAddrField], pAddrScheme->IncRate[RowAddrField],
    pAddrScheme->IncVal[RowAddrField]);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    ColIncMode=%u ColIncRate=%u ColIncVal=%d)\n",
    pAddrScheme->IncMode[ColAddrField], pAddrScheme->IncRate[ColAddrField],
    pAddrScheme->IncVal[ColAddrField]);
#endif
  SsaServicesHandle->SsaMemoryServerConfig->SetSeqAddrInc(
    SsaServicesHandle, Socket, Controller, Channel,
    pAddrScheme->IncMode[RankAddrField],
    pAddrScheme->IncRate[RankAddrField],
    (INT8)pAddrScheme->IncVal[RankAddrField],
    pAddrScheme->IncMode[BankAddrField],
    pAddrScheme->IncRate[BankAddrField],
    (INT8)pAddrScheme->IncVal[BankAddrField],
    pAddrScheme->IncMode[RowAddrField],
    pAddrScheme->IncRate[RowAddrField],
    pAddrScheme->IncVal[RowAddrField],
    pAddrScheme->IncMode[ColAddrField],
    pAddrScheme->IncRate[ColAddrField],
    pAddrScheme->IncVal[ColAddrField]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  SetSeqAddrConfig(Socket=%u Controller=%u Channel=%u\n",
    Socket, Controller, Channel);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    EnableRankWrapCarry=%u EnableBankWrapCarry=%u EnableRowWrapCarry=%u\n",
    pAddrScheme->CarryEnable[RankAddrField],
    pAddrScheme->CarryEnable[BankAddrField],
    pAddrScheme->CarryEnable[RowAddrField]);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    EnableColWrapCarry=%u  EnableRankAddrInvert=%u EnableBankAddrInvert=%u\n",
    pAddrScheme->CarryEnable[ColAddrField], FALSE, FALSE);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    EnableRowAddrInvert=%u EnableColAddrInvert=%u  AddrInvertRate=%u\n",
    FALSE, FALSE, 0);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    RankAddrOrder=%u BankAddrOrder=%u RowAddrOrder=%u ColAddrOrderRank=%u)\n",
    pAddrScheme->Order[RankAddrField], pAddrScheme->Order[BankAddrField],
    pAddrScheme->Order[RowAddrField], pAddrScheme->Order[ColAddrField]);
#endif
  SsaServicesHandle->SsaMemoryServerConfig->SetSeqAddrConfig(
    SsaServicesHandle, Socket, Controller, Channel,
    pAddrScheme->CarryEnable[RankAddrField],
    pAddrScheme->CarryEnable[BankAddrField],
    pAddrScheme->CarryEnable[RowAddrField],
    pAddrScheme->CarryEnable[ColAddrField],
    FALSE, FALSE, FALSE, FALSE, // disable address invert
    0,                           // address invert rate
    pAddrScheme->Order[RankAddrField],
    pAddrScheme->Order[BankAddrField],
    pAddrScheme->Order[RowAddrField],
    pAddrScheme->Order[ColAddrField]); // address order
} // end function SetSeqAddr

/**
@brief
  This function is used to find the minimum row address of all the populated
  DIMM in a given channel.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       Socket             Zero-based Socket number.
  @param[in]       Controller         Zero based memory controller number.
  @param[in]       Channel            Zero based memory channel number.
  @param[in]       DefaultRowSize     The default row size for AEP dimm.

  @retval  Minimum row address size.
**/
static UINT32 GetMinRowSize(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_CFG *pMemCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT32 DefaultRowSize)
{
  UINT8 DimmBitmask;
  UINT8 Dimm;
  MRC_DIMM_INFO DimmInfoBuffer;
  UINT32 MinRowSize = 0xFFFFFFFF; // initialize minimum row size to maximum value

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.GetMinRowSize(Socket=%u Controller=%u Channel=%u DefaultRowSize=%u)\n",
    Socket, Controller, Channel, DefaultRowSize);
#endif
  DimmBitmask = pMemCfg->DimmBitmasks[Socket][Controller][Channel];

  // FOR each DIMM:
  for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
    // IF this DIMM is not populated THEN skip it
    if ((DimmBitmask & (1 << Dimm)) == 0) {
      continue;
    }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  GetDimmInfo(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
      Socket, Controller, Channel, Dimm);
#endif
    SsaServicesHandle->SsaMemoryConfig->GetDimmInfo(SsaServicesHandle,
      Socket, Controller, Channel, Dimm, &DimmInfoBuffer);

    // The row size of the AEp/DDRT dimm was 1. We will skip it
    if (DimmInfoBuffer.MemoryTech == SsaMemoryDdrT) continue;

    // IF this DIMM's row size is less than the current minimum
    if (DimmInfoBuffer.RowSize < MinRowSize) {
      // this is the new minimum
      MinRowSize = DimmInfoBuffer.RowSize;
    }
  } // end for each DIMM

  if (MinRowSize == 0xFFFFFFFF){ // all dimms on this channel were AEP/DDRT dimm
    MinRowSize = DefaultRowSize;
  }
#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.GetMinRowSize(Socket=%u Controller=%u Channel=%u) DefaultRowSize=%u MinRowSize=%u:\n",
    Socket, Controller, Channel, DefaultRowSize, MinRowSize);
#endif

  return MinRowSize;
} // end function GetMinRowSize

/**
@brief
  This function is used to set up the point test sequence, subsequence and addresses.

  Note that only channels selected for testing are set up.

  @param[in, out]  SsaServicesHandle   Pointer to SSA services.
  @param[in]       pSystemInfo         Pointer to system information structure.
  @param[in]       pCpgcInfo           Pointer to CPGC information structure.
  @param[in]       pMemCfg             Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg    Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg   Pointer to CPGC point test configuration structure.
  @param[in]       pCpgcPointTestData  Pointer to CPGC point test data structure.
**/
static VOID InitTestSeq(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  IN CPGC_POINT_TEST_DATA *pCpgcPointTestData)
{
  UINT8 Socket, Controller, Channel;
  UINT8 TestedChnlIdx;
  BOOLEAN EnableGlobalControl;
  RANK_TEST_MODE RankTestMode = Singles;
  TRAFFIC_MODE TrafficMode;
  UINT8 BurstLengthExponent;
  BURST_LENGTH_MODE BurstLengthMode;
  UINT32 BurstLength, TmpBurstLength;
  UINT32 InterSubseqWait;
  UINT8 SubseqEndIndex = 0;
  UINT8 SubseqIndex;
  UINT8 SubSequenceType;
  CPGC_ADDRESS* pCpgcAddressScheme;
  CONST CPGC_SEQUENCER* pCpgcSequencerSetting;

  //
  // Address schemes
  //
  CPGC_ADDRESS SingleRankLinearCpgcAddr = {  // single rank, linear address test (for dq signals)
    //                 Rank,              Bank,              Row,               Col
    /* Start       */ {0,                 0,                 0,                 0},
    /* Wrap        */ {0,                 0xC,               0x1000 ,           0x7F}, // the row wrap address will be runtime updated based on the DIMM info
    /* Order       */ {3,                 0,                 2,                 1},    // carry increment order = bank->col->row
    /* IncMode     */ {LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode},
    /* IncRate     */ {0,                 0,                 0,                 0},
    /* IncVal      */ {0,                 4,                 0,                 0},
    /* CarryEnable */ {FALSE,             TRUE,              FALSE,             TRUE} // row address carry will not increase rank address
  };

  // DDR4 Cmd/CmdVref/Ctl test address scheme
  CPGC_ADDRESS SingleRankActiveCpgcAddr = { // single rank, active address test (for command/address signals)
    //                 Rank,              Bank,              Row,               Col
    /* Start       */ {0,                 0,                 0,                 0},
    /* Wrap        */ {0,                 0xF,               0x1FFF,            0x1F},  // the row wrap address will be runtime updated based on the DIMM info
    /* Order       */ {0,                 0,                 0,                 0},
    /* IncMode     */ {LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode},
    /* IncRate     */ {0,                 0,                 0,                 0},
    /* IncVal      */ {0,                 3,                 1553,              43},
    /* CarryEnable */ {FALSE,             FALSE,             FALSE,             FALSE}
  };

  // DDRT Cmd/CmdVref/Ctl  address scheme is slightly different from DDR4
  CPGC_ADDRESS SingleRankActiveCpgcAddr_DDRT = { // single rank, active address test (for command/address signals)
    //                 Rank,              Bank,              Row,               Col
    /* Start       */ {0,                 0,                 0,                 0},
    /* Wrap        */ {0,                 0xC,               0x1FFF,            0x1F},  // the row wrap address will be runtime updated based on the DIMM info
    /* Order       */ {0,                 0,                 0,                 0},
    /* IncMode     */ {LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode},
    /* IncRate     */ {0,                 0,                 0,                 0},
    /* IncVal      */ {0,                 4,                 1553,                 43},
    /* CarryEnable */ {FALSE,             FALSE,             FALSE,             FALSE}
  };

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitTestSeq()\n");
#endif

  //
  // initialize the sequencer, address scheme
  //
  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this channel is not enabled for testing THEN skip it
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        RankTestMode = pMemPointTestCfg->RankTestModes[Socket][Controller][Channel];
        TrafficMode = pCpgcPointTestCfg->TrafficModes[Controller][Channel];

#if SUPPORT_FOR_TURNAROUNDS
        // IF traffic mode is not idle AND this channel is configured to do
        // rank-to-rank turnaround testing AND this channel can't do
        // rank-to-rank turnarounds (this can happen as a result of the fact
        // that the RankTestMode is per-socket whereas the TrafficMode is only
        // per-controller)
        if ((TrafficMode != IdleTrafficMode) &&
          (RankTestMode == Turnarounds) &&
          !CanChannelDoTurnarounds(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, pCpgcPointTestData->IoLevel, Socket, Controller,
          Channel, pCpgcPointTestData->IsDdrT)) {
#if ENBL_VERBOSE_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  Overriding Socket=%u Controller=%u Channel=%u from TrafficMode=%u to %u\n",
            Socket, Controller, Channel, TrafficMode, IdleTrafficMode);
#endif
          // locally override the traffic mode to idle
          TrafficMode = IdleTrafficMode;
        }
#endif // SUPPORT_FOR_TURNAROUNDS

        // IF the traffic mode is idle
        if (TrafficMode == IdleTrafficMode) {
          EnableGlobalControl = FALSE;
          SubseqEndIndex = 0;
        }
        // ELSE the traffic mode is not idle
        else {
          EnableGlobalControl = TRUE;

          //
          // set up the subsequences and sequence
          //
          BurstLength = pCpgcPointTestCfg->BurstLengths[Controller][Channel];

          BurstLengthExponent = GetHighestBitSetInMask(BurstLength, 32);

          // IF the burst length is a power of 2
          if ((UINT32)(1 << BurstLengthExponent) == BurstLength) {
            BurstLengthMode = ExponentialBurstLengthMode;
            BurstLength = BurstLengthExponent;
          }
          // ELSE the burst length is not a power of 2
          else {
            BurstLengthMode = LinearBurstLengthMode;
          }

          // get the index of tested channel.
          TestedChnlIdx = GetTestChnlIndex(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel);

          // Override sub-seq wait time for DDRT device
          if (pMemCfg->MemoryTech[Socket][Controller][Channel][pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA.Dimm] != SsaMemoryDdrT) {
            InterSubseqWait = pCpgcPointTestCfg->InterSubseqWaits[Controller][Channel];
          } else {
            InterSubseqWait = DDRT_SUBSEQ_WAIT;
          }

          // select the sequencer setting
          pCpgcSequencerSetting = \
            &SequencersByTrafficMode[pCpgcPointTestCfg->TrafficModes[Controller][Channel]];

          SubseqEndIndex = pCpgcSequencerSetting->SubSequenceEnd;

          // get the subsequence type of the 2nd subsequence
          SubSequenceType = pCpgcSequencerSetting->SubSequenceType[1];

          for (SubseqIndex = pCpgcSequencerSetting->SubSequenceStart;
            SubseqIndex <= SubseqEndIndex; SubseqIndex++) {
            TmpBurstLength = BurstLength;

            // IF this sequence is for the Wr2RdTrafficMode
            if (SubSequenceType == BRdWr) {
              if (SubseqIndex == 0) {
                // the burst length for the first part needs to be half of that
                // of the second part (the value is an 2n exponent)
                if (BurstLengthMode == LinearBurstLengthMode) {
                  TmpBurstLength = BurstLength / 2;
                }
                else {
                  TmpBurstLength = BurstLength - 1;
                }
              }
            } // end if this sequence is for the Wr2RdTrafficMode

            SetSubseqConfig(SsaServicesHandle, Socket, Controller, Channel,
              SubseqIndex, BurstLengthMode, TmpBurstLength, InterSubseqWait,
              pCpgcSequencerSetting->SubSequenceType[SubseqIndex],
              pCpgcSequencerSetting->ResetBaseAddr[SubseqIndex]);
          } // end for each subsequence

          //
          // Set up the address scheme
          //
          if (pCpgcPointTestCfg->AddrModes[Controller][Channel] == LinearAddrMode) {
#if SUPPORT_FOR_TURNAROUNDS
            // IF the rank combination is turnarounds
            if (RankTestMode == Turnarounds) {
              if (TrafficMode == Wr2WrAndRd2RdTrafficMode) {
                pCpgcAddressScheme = &pCpgcPointTestData->TurnaroundCpgcAddr0;
              }
              else {
                pCpgcAddressScheme = &pCpgcPointTestData->TurnaroundCpgcAddr1;
              }
            }
            // ELSE singles test
            else
#endif // SUPPORT_FOR_TURNAROUNDS
            {
              pCpgcAddressScheme = &SingleRankLinearCpgcAddr;
            }
          }
          else {
            // Active address mode for command/address test
            if (pMemCfg->MemoryTech[Socket][Controller][Channel][pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA.Dimm] != SsaMemoryDdrT) {
              pCpgcAddressScheme = &SingleRankActiveCpgcAddr;
            } else {
              pCpgcAddressScheme = &SingleRankActiveCpgcAddr_DDRT;
            }
          }

          // update the row wrapper address based on the DIMMs
          pCpgcAddressScheme->Wrap[RowAddrField] = GetMinRowSize(
            SsaServicesHandle, pMemCfg, Socket, Controller, Channel, pCpgcAddressScheme->Wrap[RowAddrField]) - 1;

          SetSeqAddr(SsaServicesHandle, Socket, Controller, Channel,
            pCpgcAddressScheme);
        } // end else the traffic mode is not idle

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetSeqConfig(Socket=%u Controller=%u Channel=%u StartDelay=%u\n",
          Socket, Controller, Channel,
          pCpgcPointTestCfg->StartDelays[Controller][Channel]);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    SubseqStartIndex=%u SubseqEndIndex=%u LoopCount=0x%X InitMode=%u\n",
          0, SubseqEndIndex, pCpgcPointTestCfg->LoopCounts[Controller][Channel],
          ReutCpgcInitMode);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableGlobalControl=%u EnableConstantWriteStrobe=%u EnableDummyReads=%u\n",
          EnableGlobalControl, FALSE, FALSE);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    AddrUpdateRateMode=%u)\n",
          CachelineAddrUpdateRate);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetSeqConfig(
          SsaServicesHandle, Socket, Controller, Channel,
          pCpgcPointTestCfg->StartDelays[Controller][Channel], 0,
          SubseqEndIndex, pCpgcPointTestCfg->LoopCounts[Controller][Channel],
          ReutCpgcInitMode, EnableGlobalControl, FALSE, FALSE,
          CachelineAddrUpdateRate);
      } // end for each channel
    } // end for each controller

    // Turn off the "global binding" for the channel not participate to the test.
    // The MRC turn on all the "global binding" because it run DDR4/DDRT test
    // simultaneously. Since we don't test them together, we need to turn off
    // the "global binding". Otherwise, it caused hung or cpgc engine stop prematurely
    // when mixed channel with DDRT only and other channel with DDR4 dimms. 
    // for example: ch0 slot0 ddr4, ch2 slot0 ddrt.

    if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
      pMemPointTestCfg, Socket))continue;

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
            continue;
        }

        EnableGlobalControl = FALSE;
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetSeqConfig(Socket=%u Controller=%u Channel=%u StartDelay=%u\n",
          Socket, Controller, Channel,
          4);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    SubseqStartIndex=%u SubseqEndIndex=%u LoopCount=0x%X InitMode=%u\n",
          0, 0, 16,
          ReutCpgcInitMode);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableGlobalControl=%u EnableConstantWriteStrobe=%u EnableDummyReads=%u\n",
          EnableGlobalControl, FALSE, FALSE);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  AddrUpdateRateMode=%u)\n",
          CachelineAddrUpdateRate);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetSeqConfig(
          SsaServicesHandle, Socket, Controller, Channel,
          4, 0,0, 16,
          ReutCpgcInitMode, EnableGlobalControl, FALSE, FALSE,
          CachelineAddrUpdateRate);
      } // end for each channel
    } // end for each controller
  } // end for each socket
} // end function InitTestSeq

/**
@brief
  This function is used to initialize the CPGC engine and MC.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IsFirstCall         Flag indicating whether the call is the first.
**/
static VOID InitCpgcEngine(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN BOOLEAN IsFirstCall)
{
  UINT8 Socket, Controller, Channel;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitCpgcEngine()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

//      // IF this is the first call to this function
//      if (IsFirstCall) {
//        // set training mode here
//#if ENBL_BIOS_SSA_API_DEBUG_MSGS
//        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
//          "  InitCpgc(Socket=%u Controller=%u)\n",
//          Socket, Controller);
//#endif
//        SsaServicesHandle->SsaMemoryConfig->InitCpgc(SsaServicesHandle, Socket,
//          Controller, Setup);
//      } // end if this is the first call to this function

#if 0
      // Set credit wait mode
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  SetCreditWaitConfig(Socket=%u Controller=%u Enable=%u)\n",
        Socket, Controller, FALSE);
#endif
      SsaServicesHandle->SsaMemoryServerConfig->SetCreditWaitConfig(
        SsaServicesHandle, Socket, Controller, FALSE);
#endif // if 0
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }
#if 0
        // Set the WPQ in order
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetWpqInOrderConfig(Socket=%u Controller=%u Channel=%u Enable=%u)\n",
          Socket, Controller, Channel, TRUE);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetWpqInOrderConfig(
          SsaServicesHandle, Socket, Controller, Channel, TRUE);

        // Disable Dummy read
        // Hardcode to support DDR4 
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetDummyReadBankMask(Socket=%u Controller=%u Channel=%u BankMask=0x%X)\n",
          Socket, Controller, Channel, 0xFFFF);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetDummyReadBankMask(
          SsaServicesHandle, Socket, Controller, Channel, 0xFFFF);

        // Set refresh ctrl
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetRefreshOverride(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnablePanicRefreshOnly=%u PairCount=%u)\n",
          FALSE, 0);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetRefreshOverride(
          SsaServicesHandle, Socket, Controller, Channel, FALSE, 0, NULL);

        // Set ZQ cal ctrl
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetZQCalOverride(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableZQCalAfterRefresh=%u PairCount=%u)\n",
          FALSE, 0);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetZQCalOverride(
          SsaServicesHandle, Socket, Controller, Channel, FALSE, 0, NULL);

        // Set ODT override ctrl
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetOdtOverride(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableMprDdrTrain=%u ValCount=%u)\n",
          FALSE, 0);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetOdtOverride(
          SsaServicesHandle, Socket, Controller, Channel, FALSE, 0, NULL, NULL);
#endif // If 0

        // Set CKE override ctrl
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetCkeOverride(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    ValCount=%u)\n",
          0);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetCkeOverride(
          SsaServicesHandle, Socket, Controller, Channel, 0, NULL, NULL);

#if 0
        // Set multiple write credit mode
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetMultipleWrCreditConfig(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    Enable=%u)\n",
          FALSE);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetMultipleWrCreditConfig(
          SsaServicesHandle, Socket, Controller, Channel, FALSE);
#endif // If 0
      } // end for each channel
    } // end for each controller
  } // end for each socket
} // end function InitCpgcEngine

/**
@brief
  This function is used to initialize the CPGC stop mode.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg  Pointer to CPGC point test configuration structure.
**/
static VOID InitCpgcStopMode(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg)
{
  UINT8 Socket, Controller, Channel;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitCpgcStopMode()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        // Set the stop mode
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetStopMode(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    StopMode=%u StopOnNthErrorCount=%u)\n",
          pCpgcPointTestCfg->CpgcStopMode, 0);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetStopMode(
          SsaServicesHandle, Socket, Controller, Channel, pCpgcPointTestCfg->CpgcStopMode, 0);
      } // end for each channel
    } // end for each controller
  } // end for each socket
} // end function InitCpgcStopMode

/**
@brief
  This function is used to initialize the CADB controller.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg   Pointer to CPGC point test configuration structure.
  @param[in]       pCpgcPointTestData  Pointer to CPGC point test data structure.

**/
static VOID InitCadbCtrl(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  IN CONST CPGC_POINT_TEST_DATA *pCpgcPointTestData)
{
  UINT8 Socket, Controller, Channel;
  BOOLEAN EnableAlwaysOn, EnableOnDeselect;
  BOOLEAN EnableParityNTiming = FALSE;
  BOOLEAN EnableOnePassAlwaysOn = FALSE;
  BOOLEAN EnablePdaDoubleLength = FALSE;
  CADB_MODE CadbMode;
  UINT8 TestedChnlIdx;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitCadbCtrl()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        CadbMode = pCpgcPointTestCfg->CadbModes[Controller][Channel];

        // get the index of tested channel.
        TestedChnlIdx = GetTestChnlIndex(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel);

        // override the Cadb mode if the tested dimm is a DDRT device
        if (pMemCfg->MemoryTech[Socket][Controller][Channel][pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA.Dimm] == SsaMemoryDdrT) {
          CadbMode = NormalCadbMode;
        }

        switch (CadbMode) {
          case NormalCadbMode:
          default:
            EnableAlwaysOn   = FALSE;
            EnableOnDeselect = FALSE;
            break;
          case OnDeselectCadbMode:
            EnableAlwaysOn   = FALSE;
            EnableOnDeselect = TRUE;
            break;
          case AlwaysOnCadbMode:
            EnableAlwaysOn   = TRUE;
            EnableOnDeselect = FALSE;
            break;
        }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetCadbConfig(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableAlwaysOn=%u EnableOnDeselect=%u EnableParityNTiming=%u\n",
          EnableAlwaysOn, EnableOnDeselect, EnableParityNTiming);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableOnePassAlwaysOn=%u EnablePdaDoubleLength=%u)\n",
          EnableOnePassAlwaysOn, EnablePdaDoubleLength);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetCadbConfig(
          SsaServicesHandle, Socket, Controller, Channel,
          EnableAlwaysOn, EnableOnDeselect,
          EnableParityNTiming, EnableOnePassAlwaysOn, EnablePdaDoubleLength);

      } // end for each channel
    } // end for each controller
  } // end for each socket
} // end function InitCadbCtrl

/**
@brief
  This function is used to initialize the CADB pattern generation programming.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pVoidPointTestCfg  Pointer to specific memory point test configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 InitCadbPattern(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pVoidPointTestCfg,
  OUT TEST_STATUS *pTestStat)
{
  CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg = (CONST CPGC_POINT_TEST_CONFIG *) pVoidPointTestCfg;

  UINT8 Socket, Controller, Channel, Uniseq;
  BOOLEAN EnableAlwaysOn, EnableOnDeselect;
  BOOLEAN EnableParityNTiming = FALSE;
  BOOLEAN EnableOnePassAlwaysOn = FALSE;
  BOOLEAN EnablePdaDoubleLength = FALSE;
  UINT8 RetStat;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitCadbPattern()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        switch (pCpgcPointTestCfg->CadbModes[Controller][Channel]) {
          case NormalCadbMode:
          default:
            EnableAlwaysOn   = FALSE;
            EnableOnDeselect = FALSE;
            break;
          case OnDeselectCadbMode:
            EnableAlwaysOn   = FALSE;
            EnableOnDeselect = TRUE;
            break;
          case AlwaysOnCadbMode:
            EnableAlwaysOn   = TRUE;
            EnableOnDeselect = FALSE;
            break;
        }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetCadbConfig(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableAlwaysOn=%u EnableOnDeselect=%u EnableParityNTiming=%u\n",
          EnableAlwaysOn, EnableOnDeselect, EnableParityNTiming);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    EnableOnePassAlwaysOn=%u EnablePdaDoubleLength)\n",
          EnableOnePassAlwaysOn, EnablePdaDoubleLength);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetCadbConfig(
          SsaServicesHandle, Socket, Controller, Channel,
          EnableAlwaysOn, EnableOnDeselect,
          EnableParityNTiming, EnableOnePassAlwaysOn, EnablePdaDoubleLength);

        // IF this channel isn't generating CADB patterns THEN skip it
        if (pCpgcPointTestCfg->CadbModes[Controller][Channel] == NormalCadbMode) {
          continue;
        }

        // Program the CADB content. Only when the CPGC in CMD/CTL test.
        // The AddressMode should be in "ActiveAddrMode" when in CMD/CTL test
        if (pCpgcPointTestCfg->AddrModes[Controller][Channel] == ActiveAddrMode) {
          if (pCpgcPointTestCfg->EnableUserDefinedCadbPatterns) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetCadbPattern(Socket=%u Controller=%u Channel=%u RowCount=%u StartRowIndex=%u)\n",
              Socket, Controller, Channel, USER_DEFINED_CADB_PATTERN_ROW_CNT, 0);
#endif
            SsaServicesHandle->SsaMemoryServerConfig->SetCadbPattern(
              SsaServicesHandle, Socket, Controller, Channel,
              (UINT64*)pCpgcPointTestCfg->UserDefinedCadbPatterns,
              USER_DEFINED_CADB_PATTERN_ROW_CNT, 0);
          }
          // ELSE use the BKM CADB pattern
          else {
            // BKM CADB pattern
            /*assignment_dict = {"MA0":"LFSR1", "MA1":"LFSR0","MA2":"LFSR3","MA3":"LFSR2","MA4":"LFSR1",
              "MA5":"LFSR0", "MA6":"LFSR3","MA7":"LFSR2","MA8":"LFSR1","MA9":"LFSR0", 
              "MA10":"LFSR3","MA11":"LFSR2","MA12":"LFSR1","MA13":"LFSR0","MA14":"LFSR3",
              "MA15":"LFSR2","MA16":"LFSR1", "C0": "LFSR2", "C1":"LFSR1", "C2":"LFSR0",
              "BA0":"LFSR3", "BA1":"LFSR2", "BG0":"LFSR1","BG1":"LFSR0",
              "WE":"LFSR3", "RAS":"LFSR1", "CAS":"LFSR2", "ACT":"LFSR0"}
            while the rest will be 0s.*/
           /* UINT64 BkmCadbPattern[CADB_PATTERN_ROW_CNT] = {
              0x000000000000,
              0x800008402222,
              0x400004211111,
              0xc0000c613333,
              0x200002108888,
              0xa0000a50aaaa,
              0x600006319999,
              0xe0000e71bbbb,
              0x100001004444,
              0x900009406666,
              0x500005215555,
              0xd0000d617777,
              0x30000310cccc,
              0xb0000b50eeee,
              0x70000731dddd,
              0xf0000f71ffff,
            };*/
            //// MRC AdvCmdPattern0/1
            //UINT64 BkmCadbPattern[CADB_PATTERN_ROW_CNT] = {
            //  0x0000000000000000,
            //  0x0000300008002222,
            //  0x0000300004211111,
            //  0x000030000c213333,
            //  0x0000300002108888,
            //  0x000030000a10aaaa,
            //  0x0000300006319999,
            //  0x000030000e31bbbb,
            //  0x0000300001004444,
            //  0x0000300009006666,
            //  0x0000300005215555,
            //  0x000030000d217777,
            //  0x000030000310cccc,
            //  0x000030000b10eeee,
            //  0x000030000731dddd,
            //  0x000030000f31ffff,
            //};

            // MRC AggAdvCmdPattern0/1
            // 100
            UINT64 BkmCadbPattern[CADB_PATTERN_ROW_CNT] = {
              0x0000000000000000,
              0x0000800000002040,
              0x000070000f71dfdf,
              0x0000f0000f71ffff,
              0x0000000000000000,
              0x0000800000002040,
              0x000070000f71dfdf,
              0x0000f0000f71ffff,
              0x0000000000000000,
              0x0000800000002040,
              0x000070000f71dfdf,
              0x0000f0000f71ffff,
              0x0000000000000000,
              0x0000800000002040,
              0x000070000f71dfdf,
              0x0000f0000f71ffff,
            };

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetCadbPattern(Socket=%u Controller=%u Channel=%u RowCount=%u StartRowIndex=%u)\n",
              Socket, Controller, Channel, CADB_PATTERN_ROW_CNT, 0);
#endif
            SsaServicesHandle->SsaMemoryServerConfig->SetCadbPattern(
              SsaServicesHandle, Socket, Controller, Channel,
              (UINT64*)&BkmCadbPattern, CADB_PATTERN_ROW_CNT, 0);
          } // end else use the BKM CADB pattern

          // FOR each CADB unisequencer:
          for (Uniseq = 0; Uniseq < MAX_CADB_UNISEQ_CNT; Uniseq++) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetCadbUniseqMode(Socket=%u Controller=%u Channel=%u Uniseq=%u DeselectMode=%u)\n",
              Socket, Controller, Channel, Uniseq, 
              pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Uniseq]);
#endif
            RetStat = SsaServicesHandle->SsaMemoryServerConfig->SetCadbUniseqMode(
              SsaServicesHandle, Socket, Controller, Channel, Uniseq,
              pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Uniseq]);
            if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
              return 1;
            }

            // IF the unisequencer mode is "LMN"
            if ((Uniseq < MAX_CADB_LMN_UNISEQ_CNT) &&
              (pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Uniseq] ==
              LmnCadbUniseqMode)) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  SetCadbUniseqLmn(Socket=%u Controller=%u Channel=%u Uniseq=%u\n",
                Socket, Controller, Channel, Uniseq);
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "    L=%u M=%u N=%u LDataSel=%u EnableFreqSweep=%u)\n",
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][0],
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][1],
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][2],
                0, FALSE);
#endif
              RetStat = SsaServicesHandle->SsaMemoryServerConfig->SetCadbUniseqLmn(
                SsaServicesHandle, Socket, Controller, Channel, Uniseq,
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][0],
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][1],
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][2],
                0, FALSE);
              if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
                return 1;
              }
            }
            // ELSE the unisequencer mode is "LFSR" or "buffer"
            else {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  SetCadbUniseqSeed(Socket=%u Controller=%u Channel=%u Uniseq=%u\n",
                Socket, Controller, Channel, Uniseq);
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "    DeselectSeed=0x%X)\n",
                pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][Uniseq]);
#endif
              SsaServicesHandle->SsaMemoryServerConfig->SetCadbUniseqSeed(
                SsaServicesHandle, Socket, Controller, Channel, Uniseq,
                pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][Uniseq]);
            } // end else the unisequencer mode is "LFSR" or "buffer"
          } // end for each CADB unisequencer
        } // if in ActiveAddrMode
      } // end for each channel
    } // end for each controller
  } // end for each socket

  return 0;
} // end function InitCadbPattern

/**
@brief
  This function is used to initialize the error counters and validation masks.

  Note that only channels selected for testing are set up.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
**/
static VOID InitValidation(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CPGC_POINT_TEST_DATA *pCpgcPointTestData)
{
  UINT8 Socket, Controller, Channel;
  UINT8 TestedChnlIdx;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.InitValidation()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetErrorCounterMode(Socket=%u Controller=%u Channel=%u Counter=%u\n",
          Socket, Controller, Channel, 0);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    Mode=%u ModeIndex=%u)\n",
          ChannelErrorCounterMode, 0);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetErrorCounterMode(
          SsaServicesHandle, Socket, Controller, Channel, 0,
          ChannelErrorCounterMode, 0);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetLaneValidationMask(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    LaneBitmasks=0x");
        LogByteArrayAsHex(SsaServicesHandle,
          pMemPointTestCfg->LaneValBitmasks[Controller][Channel],
          (pSystemInfo->BusWidth / 8));
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, ")\n");
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetLaneValidationMask(
          SsaServicesHandle, Socket, Controller, Channel,
          (UINT8*)&pMemPointTestCfg->LaneValBitmasks[Controller][Channel][0]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetCachelineValidationMask(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    CachelineMask=0x%02X)\n",
          0xFF);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetCachelineValidationMask(
          SsaServicesHandle, Socket, Controller, Channel, 0xFF);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetChunkValidationMask(Socket=%u Controller=%u Channel=%u\n",
          Socket, Controller, Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    ChunkMask=0x%02X)\n",
          pMemPointTestCfg->ChunkValBitmasks[Controller][Channel]);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetChunkValidationMask(
          SsaServicesHandle, Socket, Controller, Channel,
          pMemPointTestCfg->ChunkValBitmasks[Controller][Channel]);

        // save the current chunk  mask to point test data, it will be used later in error checking
        TestedChnlIdx = GetTestChnlIndex(SsaServicesHandle, pSystemInfo,
          pMemCfg, pMemPointTestCfg, Socket, Controller, Channel);
        pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntChunkSelectMask = \
          pMemPointTestCfg->ChunkValBitmasks[Controller][Channel];
      } // end for each channel
    } // end for each controller
  } // end for each socket
} // end function InitValidation

/**
@brief
  This function is used to run a point test.

  @param[in, out]  SsaServicesHandle   Pointer to SSA services.
  @param[in]       pSystemInfo         Pointer to system information structure.
  @param[in]       pMemCfg             Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg    Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg   Pointer to CPGC point test configuration structure.
  @param[in]       pCpgcPointTestData  Pointer to CPGC point test data structure.
  @param[out]      pTestStat           Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 RunPointTest(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  IN CPGC_POINT_TEST_DATA *pCpgcPointTestData,
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller, Channel;
  BOOLEAN ClearErrors;
  BOOLEAN StartTest;
  BOOLEAN StopTest;
  UINT8 TestDoneMask;
  BOOLEAN SocketAnyLaneFailed, ControllerAnyLaneFailed;
  UINT8 LaneErrorStatus[MAX_BUS_WIDTH / 8];
  INT8 ByteGroup;
  UINT8 Counter = 0;
  UINT32 Count = 0;
  BOOLEAN Overflow = FALSE;
  ERROR_COUNT_32BITS ErrCnt;
  UINT8 ChannelBitmask;
  UINT8 TestedChnlIdx;
  UINT16 EridCount;
  UINT8 EridStatus;
  SSA_SEQ_STATE SeqState;
  UINT32 CurrentLoopCounts[MAX_CHANNEL_CNT];
  UINT32 CurrentCachelines[MAX_CHANNEL_CNT];
  UINT8 NonProgressCount[MAX_CHANNEL_CNT];
  UINT8 MaxNonProgressCount = 200;
  BOOLEAN ChannelTimedOut[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  BOOLEAN TimeoutOccurred = FALSE;
  DIMM_RANK_PAIR DimmRankA = {0,0}, DimmRankB = {0,0};

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CpgcPointTest.RunPointTest()\n");
#endif

#if ENBL_VERBOSE_LOG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  Clearing errors and starting the DFx engine(s).\n");
#endif

  // IF we're testing an early read ID margin parameter THEN clear the early
  // read ID status
  if (pCpgcPointTestCfg->EnableErid) {
    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // FOR each controller:
      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
        // FOR each channel:
        for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
          // IF this channel is not enabled for testing THEN skip it
          if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel)) {
            continue;
          }

          // PT need to check if AEP DIMM is available
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  ClearEridErrorStatus(Socket=%u Controller=%u Channel=%u)\n",
            Socket, Controller, Channel);
#endif
          SsaServicesHandle->SsaMemoryDdrtConfig->ClearEridErrorStatus(
            SsaServicesHandle, Socket, Controller, Channel);
        } // end for each channel
      } // end for each controller
    } // end for each socket
  } // end if we're testing an early read ID margin parameter

  // ------------------------------------------------------------------------
  // Start the test.
  // ------------------------------------------------------------------------
  ClearErrors = TRUE;
  StartTest = TRUE;
  StopTest = FALSE;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not enabled for testing THEN skip it
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  SetGlobalTestControl(Socket=%u Controller=%u\n",
        Socket, Controller);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    ClearErrors=%u StartTest=%u StopTest=%u)\n",
        ClearErrors, StartTest, StopTest);
#endif
      SsaServicesHandle->SsaMemoryServerConfig->SetGlobalTestControl(
        SsaServicesHandle, Socket, Controller, ClearErrors, StartTest, StopTest);
    } // end for each controller
  } // end for each socket

  // ------------------------------------------------------------------------
  // Wait for the test to complete
  // ------------------------------------------------------------------------
  // Note that we only wait for controllers that are enabled for validation.
#if ENBL_VERBOSE_LOG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  Waiting for the DFx engine(s) to complete.\n");
#endif

  // clear the time out flags
  bssa_memset(ChannelTimedOut, 0, sizeof(ChannelTimedOut));

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

      // IF the "all populated" value is specified
      if (pMemPointTestCfg->ChannelBitmasks[Socket][Controller] ==
        ALL_POPULATED_CONTROLLERS) {
        ChannelBitmask = pMemCfg->ChannelBitmasks[Socket][Controller];
      }
      else {
        ChannelBitmask = pMemPointTestCfg->ChannelBitmasks[Socket][Controller];
      }

      bssa_memset(&CurrentLoopCounts, 0, sizeof(CurrentLoopCounts));
      bssa_memset(&CurrentCachelines, 0, sizeof(CurrentCachelines));
      bssa_memset(&NonProgressCount, 0, sizeof(NonProgressCount));

      TestDoneMask = 0;
      // DO-WHILE all channels for this controller are not done (or timed out):
      do {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
        if (pCpgcPointTestData->TestStatusLogLevel >= 7) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  GetTestDone(Socket=%u Controller=%u ChannelBitmask=0x%X)\n",
            Socket, Controller, ChannelBitmask);
        }
#endif
        SsaServicesHandle->SsaMemoryServerConfig->GetTestDone(
          SsaServicesHandle, Socket, Controller, ChannelBitmask, &TestDoneMask);
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
        if (pCpgcPointTestData->TestStatusLogLevel >= 7) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "    TestDoneMask=0x%X\n", TestDoneMask);
        }
#endif
        // IF all channels for this controller are done
        if (TestDoneMask == ChannelBitmask) {
          // exit the while loop
          break;
        }

        // IF timeout checking is enabled
        if (pCpgcPointTestCfg->EnableTimeout) {
          // FOR each channel:
          for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
            // IF this channel is not enabled for testing THEN skip it
            if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller, Channel)) {
                continue;
            }

            // IF this channel's test-done was to be checked and this channel
            // didn't complete and hasn't already been identified as timed out
            if ((ChannelBitmask & (1 << Channel)) && 
              ((TestDoneMask & (1 << Channel)) == 0) &&
              !ChannelTimedOut[Socket][Controller][Channel]) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
                if (pCpgcPointTestData->TestStatusLogLevel >= 7) {
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                    "  GetSeqState(Socket=%u Controller=%u Channel=%u)\n",
                    Socket, Controller, Channel);
                }
#endif
                // get the current loop count
                SsaServicesHandle->SsaMemoryServerConfig->GetSeqState(
                  SsaServicesHandle, Socket, Controller, Channel, &SeqState);
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
                if (pCpgcPointTestData->TestStatusLogLevel >= 7) {
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                    "    LoopCount=%u Cacheline=%u\n",
                    SeqState.LoopCount, SeqState.Cacheline);
                }
#endif
                // IF the CPGC engine has started (ignore start delays) but isn't progressing
                if ((SeqState.LoopCount != 0) &&
                  (SeqState.LoopCount == CurrentLoopCounts[Channel]) &&
                  (SeqState.Cacheline == CurrentCachelines[Channel])) {
                  // increment the consecutive non-progress counter
                  NonProgressCount[Channel] += 1;

#if ENBL_VERBOSE_LOG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                    "CPGC engine not progressing (Socket=%u Controller=%u Channel=%u Count=%u).\n",
                    Socket, Controller, Channel, NonProgressCount[Channel]);
#endif
                  // IF the consecutive non-progress counter exceeds the maximum value
                  if (NonProgressCount[Channel] > MaxNonProgressCount) {
                    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                      "ERROR: CPGC engine hung (Socket=%u Controller=%u Channel=%u).\n",
                      Socket, Controller, Channel);

                    ChannelTimedOut[Socket][Controller][Channel] = TRUE;
                    TimeoutOccurred = TRUE;

                    // explicitly stop the CPGC engine
                    ClearErrors = FALSE;
                    StartTest = FALSE;
                    StopTest = TRUE;
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
                    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                      "  SetLocalTestControl(Socket=%u Controller=%u Channel=%u\n",
                      Socket, Controller, Channel);
                    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                      "    ClearErrors=%u StartTest=%u StopTest=%u)\n",
                      ClearErrors, StartTest, StopTest);
#endif
                    SsaServicesHandle->SsaMemoryServerConfig->SetLocalTestControl(
                      SsaServicesHandle, Socket, Controller, Channel, ClearErrors, StartTest, StopTest);

                    // remove the channel from the bitmask
                    ChannelBitmask &= ~(1 << Channel);
                  } // end if the consecutive non-progress counter exceeds the maximum value
                }
                // ELSE the CPGC engine is progressing
                else {
                  // clear the consecutive non-progress counter
                  NonProgressCount[Channel] = 0;
                }

                // save the current values
                CurrentLoopCounts[Channel] = SeqState.LoopCount;
                CurrentCachelines[Channel] = SeqState.Cacheline;
            } // end if this channel's test-done was to be checked...
          } // end for each channel
        } // end if timeout checking is enabled
      } while (TestDoneMask != ChannelBitmask); // end do-while all channels...
    } // end for each controller
  } // end for each socket

  // ------------------------------------------------------------------------
  // Get the error status.
  // ------------------------------------------------------------------------
  // Note that we only get error status and counts and write results for
  // controllers that are enabled for validation.
#if ENBL_VERBOSE_LOG_MSGS || ENBL_TIMEOUT_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  Checking error status.\n");
#endif
  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    SocketAnyLaneFailed = FALSE;

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

      // set default values
      ControllerAnyLaneFailed = FALSE;

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        // IF this channel is idle THEN skip it
        if (pCpgcPointTestCfg->TrafficModes[Controller][Channel] ==
          IdleTrafficMode) {
          continue;
        }

#if SUPPORT_FOR_TURNAROUNDS
        // IF this channel is configured to do rank-to-rank turnaround testing
        // AND this channel can't do rank-to-rank turnarounds (this can happen
        // as a result of the fact that the RankTestMode is per-socket whereas
        // the TrafficMode is only per-controller) THEN skip it
        if ((pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] == Turnarounds) &&
          !CanChannelDoTurnarounds(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, pCpgcPointTestData->IoLevel, Socket, Controller,
          Channel, pCpgcPointTestData->IsDdrT)) {
          continue;
        }
#endif // SUPPORT_FOR_TURNAROUNDS

        // get test channel index
        TestedChnlIdx = GetTestChnlIndex(SsaServicesHandle, pSystemInfo,
          pMemCfg, pMemPointTestCfg, Socket, Controller, Channel);

        // IF this channel has lanes enabled for validation:
        if (IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          // IF timeout checking is enabled and this channel timed out
          if (pCpgcPointTestCfg->EnableTimeout &&
            ChannelTimedOut[Socket][Controller][Channel]) {
            // poison the error results
            pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AnyLaneFailed = TRUE;
            pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AllLanesFailed = TRUE;
            bssa_memcpy(LaneErrorStatus, pMemPointTestCfg->LaneValBitmasks[Controller][Channel], sizeof(LaneErrorStatus));
            ErrCnt.Bits.Count = 0;
            ErrCnt.Bits.Overflow = TRUE;
          }
          // ELSE the channel didn't time out
          else {
            // preset the lane fail status
            pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AnyLaneFailed = FALSE;
            pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AllLanesFailed = TRUE;

            // PT currently I assume that there is no need to check ERID and DQ error status simultaneously 
            // the error status either DQ status or EIRD status, NOT both.
            // This allows us to reuse the Lane failing status structure for ERID which occupy the first 
            // two lanes.

            // IF we're not testing Early Read ID
            if (!pCpgcPointTestCfg->EnableErid) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  GetLaneErrorStatus(Socket=%u Controller=%u Channel=%u ChunkValBitmask=0x%X)\n",
                Socket, Controller, Channel,
                pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntChunkSelectMask);
#endif
              SsaServicesHandle->SsaMemoryServerConfig->GetLaneErrorStatus(
                SsaServicesHandle, Socket, Controller, Channel,
                pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntChunkSelectMask,
                &LaneErrorStatus[0]);
            }
            // ELSE we're testing Early Read ID
            else {
              bssa_memset(LaneErrorStatus, 0, sizeof(LaneErrorStatus));
              // ERID
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  GetEridErrorStatus(Socket=%u Controller=%u Channel=%u)\n",
                Socket, Controller, Channel);
#endif
              SsaServicesHandle->SsaMemoryDdrtConfig->GetEridErrorStatus(
                SsaServicesHandle, Socket, Controller, Channel, &EridStatus, &EridCount);
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "    EridStatus=%u EridCount=%u\n",
                EridStatus, EridCount);
#endif
              // convert the error status back to lane status
              // even and odd bits correspond to ERID0 and 1
              if (EridStatus & 0x55) {
                LaneErrorStatus[0] |= 1;       // Bit 0
              }
              if (EridStatus & 0xAA) {
                LaneErrorStatus[0] |= 1 << 1;  // Bit 1
              }

              // Update error count info
              Count = EridCount;
            } // end else we're testing Early Read ID

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "    LaneErrorStatus=0x");
            for (ByteGroup = ((pSystemInfo->BusWidth / 8) - 1); ByteGroup >= 0; ByteGroup--) {
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "%02X", LaneErrorStatus[ByteGroup]);
            }
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "\n");
#endif
            // FOR each byte group:
            for (ByteGroup = 0; ByteGroup < (pSystemInfo->BusWidth / 8); ByteGroup++) {
              if (LaneErrorStatus[ByteGroup] != 0) {
                pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AnyLaneFailed = TRUE;
                break;
              }
            } // end for each byte group

            // IF we're testing Early Read ID
            if (pCpgcPointTestCfg->EnableErid) {
#if __STUB__
              assert(MAX_ERID_SIGNAL_CNT <= 8);
#endif // __STUB__
              if ((LaneErrorStatus[0] &
                pMemPointTestCfg->LaneValBitmasks[Controller][Channel][0] &
                ((1 << MAX_ERID_SIGNAL_CNT) - 1)) !=
                (pMemPointTestCfg->LaneValBitmasks[Controller][Channel][0] &
                ((1 << MAX_ERID_SIGNAL_CNT) - 1))) {
                pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AllLanesFailed = FALSE;
              }
            }
            // ELSE we're not testing Early Read ID
            else {
              // FOR each byte group:
              for (ByteGroup = 0; ByteGroup < (pSystemInfo->BusWidth / 8); ByteGroup++) {
                if ((LaneErrorStatus[ByteGroup] &
                  pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup]) !=
                  pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup]) {
                  pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AllLanesFailed = FALSE;
                  break;
                }
              } // end for each byte group
            } // end else we're not testing Early Read ID

            // ERID error count already retrieved along with status in previous call
            // IF we're not testing Early Read ID
            if (!pCpgcPointTestCfg->EnableErid) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  GetErrorCount(Socket=%u Controller=%u Channel=%u Counter=%u)\n",
                Socket, Controller, Channel, Counter);
#endif
              SsaServicesHandle->SsaMemoryServerConfig->GetErrorCount(
                SsaServicesHandle, Socket, Controller, Channel, Counter, &Count, &Overflow);
            } // end if we're not testing Early Read ID
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "    Count=%u Overflow=%u\n",
              Count, Overflow);
#endif
            ErrCnt.Bits.Count = Count;
            ErrCnt.Bits.Overflow = Overflow;
          } // end else the channel didn't time out

          if (pCpgcPointTestData->TestStatusLogLevel >= 5) {

             DimmRankA = pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA;
             DimmRankB = pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankB;

            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "\n"
              "  Socket=%u Controller=%u Channel=%u DimmA=%u RankA=%u\n",
              Socket, Controller, Channel, DimmRankA.Dimm, DimmRankA.Rank);

            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  DimmB=%u RankB=%u RankCombinationsIndex=%u\n"
              "  LaneErrStat=0x",
              DimmRankB.Dimm, DimmRankB.Rank, pCpgcPointTestData->RankCombinationsIndex);

            for (ByteGroup = ((pSystemInfo->BusWidth / 8) - 1); ByteGroup >= 0; ByteGroup--) {
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "%02X", LaneErrorStatus[ByteGroup]);
            }

            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              " ErrCnt=%u Overflow=%u\n",
              ErrCnt.Bits.Count, ErrCnt.Bits.Overflow);
          }

          if ((((ErrCnt.Bits.Count != 0) || (ErrCnt.Bits.Overflow == TRUE)) &&
            (pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AnyLaneFailed == FALSE)) ||
            (((ErrCnt.Bits.Count == 0) && (ErrCnt.Bits.Overflow == FALSE)) &&
            (pCpgcPointTestData->LaneFailStats[Socket][Controller][Channel].AnyLaneFailed == TRUE))) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: Error count and status don't agree.\n");
#endif
            // PT Don't return error for now. The Simics and RcSim don't have the right
            // CPGC model which can cause error cnt and status not agree.
            // There is a run time variable can be used to determine whether is in Simics,
            // however, the variable is part of the MRC host structure which the EV content 
            // can not access unless we create an API for it.
            // The RcSim can be determined at the compiling.
//            ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
//            return 1;
          }

          if ((ErrCnt.Bits.Count != 0) || ErrCnt.Bits.Overflow) {
            ControllerAnyLaneFailed = TRUE;
          }

          // IF we're not in the binary search state and there is a result
          // handler
          if (!pCpgcPointTestData->InBinarySearchState &&
            pCpgcPointTestData->ResultHandler) {
            // pass the results back upstream
            if (pCpgcPointTestData->ResultHandler(
              SsaServicesHandle, pSystemInfo,
              pCpgcPointTestData->pClientTestData, Socket, Controller, Channel,
              pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA,
              pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankB,
              pCpgcPointTestData->RankCombinationsIndex, LaneErrorStatus,
              ErrCnt, pTestStat)) {
              return 1;
            }
          } // end if we're not in the binary search state and ...
        }
        // ELSE this channel is active but not validated
        else {
// PT where will below status go?
          // we still need to record a result to capture it's rank info
          bssa_memset(LaneErrorStatus, 0, sizeof(LaneErrorStatus));
          ErrCnt.Bits.Count = 0;
          ErrCnt.Bits.Overflow = FALSE;
        }
      } // end for each channel

      if (ControllerAnyLaneFailed) {
        SocketAnyLaneFailed = TRUE;
      }
    } // end for each controller
  } // end for each socket

  return (TimeoutOccurred) ? 1 : 0;
} // end function RunPointTest

/**
@brief
  This function checks the given CPGC information values against the
  corresponding preprocessor definition values.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pCpgcInfo          Pointer to CPGC information structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 ChkCpgcInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN MRC_SERVER_CPGC_INFO *pCpgcInfo,
  OUT TEST_STATUS *pTestStat)
{
  if ((pCpgcInfo->MaxNumberDqdbUniseqs > MAX_DQDB_UNISEQ_CNT) ||
    (pCpgcInfo->MaxNumberCadbRows > CADB_PATTERN_ROW_CNT) ||
    (pCpgcInfo->MaxNumberCadbUniseqs > MAX_CADB_UNISEQ_CNT)) {
#if ENBL_VERBOSE_ERROR_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "ERROR: CpgcInfo value(s) exceed preprocessor value(s).\n");
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "MaxNumberDqdbUniseqs=%u MAX_DQDB_UNISEQ_CNT=%u\n"
        "MaxNumberCadbRows=%u CADB_PATTERN_ROW_CNT=%u\n",
        pCpgcInfo->MaxNumberDqdbUniseqs, MAX_DQDB_UNISEQ_CNT,
        pCpgcInfo->MaxNumberCadbRows, CADB_PATTERN_ROW_CNT);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "MaxNumberCadbUniseqs=%u MAX_CADB_UNISEQ_CNT=%u\n",
        pCpgcInfo->MaxNumberCadbUniseqs, MAX_CADB_UNISEQ_CNT);
#endif
    ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  return 0;
} // end function ChkCpgcInfo

/**
@brief
  This function logs the CPGC information.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pCpgcInfo          Pointer to CPGC information structure.
**/
VOID LogCpgcInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN MRC_SERVER_CPGC_INFO *pCpgcInfo)
{
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n"
    "CPGC Information:\n");
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberDqdbCachelines=%u\n", pCpgcInfo->MaxNumberDqdbCachelines);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbIncRateExponentVal=%u\n", pCpgcInfo->MaxDqdbIncRateExponentVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbIncRateLinearVal=%u\n", pCpgcInfo->MaxDqdbIncRateLinearVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberDqdbUniseqs=%u\n", pCpgcInfo->MaxNumberDqdbUniseqs);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbUniseqSeedVal=0x%X\n", pCpgcInfo->MaxDqdbUniseqSeedVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbUniseqLVal=0x%X\n", pCpgcInfo->MaxDqdbUniseqLVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbUniseqMVal=0x%X\n", pCpgcInfo->MaxDqdbUniseqMVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbUniseqNVal=0x%X\n", pCpgcInfo->MaxDqdbUniseqNVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbUniseqSeedSaveRateVal=0x%X\n", pCpgcInfo->MaxDqdbUniseqSeedSaveRateVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDqdbInvertDcShiftRateExponentVal=0x%X\n", pCpgcInfo->MaxDqdbInvertDcShiftRateExponentVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberCadbRows=%u\n", pCpgcInfo->MaxNumberCadbRows);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberCadbUniseqs=%u\n", pCpgcInfo->MaxNumberCadbUniseqs);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxCadbUniseqSeedVal=0x%X\n", pCpgcInfo->MaxCadbUniseqSeedVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxCadbUniseqLVal=0x%X\n", pCpgcInfo->MaxCadbUniseqLVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxCadbUniseqMVal=0x%X\n", pCpgcInfo->MaxCadbUniseqMVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxCadbUniseqNVal=0x%X\n", pCpgcInfo->MaxCadbUniseqNVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDummyReadLVal=0x%X\n", pCpgcInfo->MaxDummyReadLVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDummyReadMVal=0x%X\n", pCpgcInfo->MaxDummyReadMVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxDummyReadNVal=0x%X\n", pCpgcInfo->MaxDummyReadNVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxStartDelayVal=0x%X\n", pCpgcInfo->MaxStartDelayVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  IsLoopCountExponential=%u\n", pCpgcInfo->IsLoopCountExponential);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxLoopCountVal=0x%X\n", pCpgcInfo->MaxLoopCountVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberSubseqs=%u\n", pCpgcInfo->MaxNumberSubseqs);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxBurstLengthExponentVal=0x%X\n", pCpgcInfo->MaxBurstLengthExponentVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxBurstLengthLinearVal=0x%X\n", pCpgcInfo->MaxBurstLengthLinearVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxInterSubseqWaitVal=0x%X\n", pCpgcInfo->MaxInterSubseqWaitVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxOffsetAddrUpdateRateVal=0x%X\n", pCpgcInfo->MaxOffsetAddrUpdateRateVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxAddrInvertRateVal=0x%X\n", pCpgcInfo->MaxAddrInvertRateVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxRankAddrIncRateExponentVal=0x%X\n", pCpgcInfo->MaxRankAddrIncRateExponentVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxRankAddrIncRateLinearVal=0x%X\n", pCpgcInfo->MaxRankAddrIncRateLinearVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MinRankAddrIncVal=%d\n", pCpgcInfo->MinRankAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxRankAddrIncVal=%d\n", pCpgcInfo->MaxRankAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxBankAddrIncRateExponentVal=0x%X\n", pCpgcInfo->MaxBankAddrIncRateExponentVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxBankAddrIncRateLinearVal=0x%X\n", pCpgcInfo->MaxBankAddrIncRateLinearVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MinBankAddrIncVal=%d\n", pCpgcInfo->MinBankAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxBankAddrIncVal=%d\n", pCpgcInfo->MaxBankAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxRowAddrIncRateExponentVal=0x%X\n", pCpgcInfo->MaxRowAddrIncRateExponentVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxRowAddrIncRateLinearVal=0x%X\n", pCpgcInfo->MaxRowAddrIncRateLinearVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MinRowAddrIncVal=%d\n", pCpgcInfo->MinRowAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxRowAddrIncVal=%d\n", pCpgcInfo->MaxRowAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxColAddrIncRateExponentVal=0x%X\n", pCpgcInfo->MaxColAddrIncRateExponentVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxColAddrIncRateLinearVal=0x%X\n", pCpgcInfo->MaxColAddrIncRateLinearVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MinColAddrIncVal=%d\n", pCpgcInfo->MinColAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxColAddrIncVal=%d\n", pCpgcInfo->MaxColAddrIncVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberRankMapEntries=%u\n", pCpgcInfo->MaxNumberRankMapEntries);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberBankMapEntries=%u\n", pCpgcInfo->MaxNumberBankMapEntries);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberRowAddrSwizzleEntries=%u\n", pCpgcInfo->MaxNumberRowAddrSwizzleEntries);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxRowAddrSwizzleVal=%u\n", pCpgcInfo->MaxRowAddrSwizzleVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxStopOnNthErrorCountVal=0x%X\n", pCpgcInfo->MaxStopOnNthErrorCountVal);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MaxNumberErrorCounters=%u\n", pCpgcInfo->MaxNumberErrorCounters);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n");
} // end function LogCpgcInfo

/**
@brief
  This function is used to set default values in the given CPGC point test
  configuration.

  @param[out]  pCpgcPointTestCfg  Pointer to CPGC point test configuration structure.
**/
VOID SetCpgcPointTestCfgDefaults(
  OUT CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg)
{
  UINT8 Controller, Channel, Uniseq, LmnIdx;

  // start by setting everything to 0
  bssa_memset(pCpgcPointTestCfg, 0, sizeof(*pCpgcPointTestCfg));

  pCpgcPointTestCfg->CpgcStopMode = DoNotStopCpgcStopMode;

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      pCpgcPointTestCfg->TrafficModes[Controller][Channel]           = LoopbackTrafficMode;
      pCpgcPointTestCfg->AddrModes[Controller][Channel]              = LinearAddrMode;
      pCpgcPointTestCfg->BurstLengths[Controller][Channel]           = 128;
      pCpgcPointTestCfg->LoopCounts[Controller][Channel]             = 15;
      pCpgcPointTestCfg->BinarySearchLoopCounts[Controller][Channel] = 10;
      pCpgcPointTestCfg->StartDelays[Controller][Channel]            = 0;
      pCpgcPointTestCfg->InterSubseqWaits[Controller][Channel]       = 0;
    } // end for each channel
  } // end for each controller

  pCpgcPointTestCfg->EnableUserDefinedDqdbPatterns = FALSE;

  // FOR each channel:
  for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
    pCpgcPointTestCfg->DqdbIncRateModes[Channel] = ExponentialDqdbIncRateMode;
    pCpgcPointTestCfg->DqdbIncRates[Channel] = 4;
  } // end for each channel

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      // FOR each DqDB unisequencer:
      for (Uniseq = 0; Uniseq < MAX_DQDB_UNISEQ_CNT; Uniseq++) {
        pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq] = LfsrDqdbUniseqMode;
      } // end for each WDB unisequencer

      pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][0] = 0x123456;
      pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][1] = 0xABCDEF;
      pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][2] = 0x876543;

      // FOR each DqDB LMN unisequencer:
      for (Uniseq = 0; Uniseq < MAX_DQDB_LMN_UNISEQ_CNT; Uniseq++) {
        // FOR each DqDB LMN value:
        for (LmnIdx = 0; LmnIdx < MAX_DQDB_LMN_VALUES_CNT; LmnIdx++) {
          pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][LmnIdx] = 1;
        } // end for each DqDB LMN value
      } // end for each DqDB LMN unisequencer

      pCpgcPointTestCfg->CadbModes[Controller][Channel] = NormalCadbMode;
    } // end for each channel
  } // end for each controller

  pCpgcPointTestCfg->EnableUserDefinedDqdbPatterns = FALSE;

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      // FOR each CADB unisequencer:
      for (Uniseq = 0; Uniseq < MAX_CADB_UNISEQ_CNT; Uniseq++) {
        pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Uniseq] = LfsrCadbUniseqMode;
      } // end for each CADB unisequencer

      pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][0] = 0x0EA1;
      pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][1] = 0xBEEF;
      pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][2] = 0xDEAD;
      pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][3] = 0x29AB;

      // FOR each CADB LMN unisequencer:
      for (Uniseq = 0; Uniseq < MAX_CADB_LMN_UNISEQ_CNT; Uniseq++) {
        // FOR each CADB LMN value:
        for (LmnIdx = 0; LmnIdx < MAX_CADB_LMN_VALUES_CNT; LmnIdx++) {
          pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][LmnIdx] = 1;
        } // end for each CADB LMN value
      } // end for each CADB LMN unisequencer
    } // end for each channel
  } // end for each controller

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      pCpgcPointTestCfg->EridPatternSeeds[Controller][Channel][EridSignal0] = 0x5A5A5;
      pCpgcPointTestCfg->EridPatternSeeds[Controller][Channel][EridSignal1] = 0xC6C6C;
    } // end for each channel
  } // end for each controller
} // end function SetCpgcPointTestCfgDefaults

/**
@brief
  This function is used to check the CPGC point test configuration for
  correctness/consistency. It also fills in the given memory configuration
  structure.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pCpgcInfo          Pointer to CPGC information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg  Pointer to CPGC point test configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 ChkCpgcPointTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MRC_SERVER_CPGC_INFO *pCpgcInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller, Channel, Uniseq;
  UINT16 BurstLength;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "ChkCpgcPointTestCfg()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing:
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        if (pCpgcPointTestCfg->TrafficModes[Controller][Channel] >= TrafficModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: TrafficMode value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->TrafficModes[Controller][Channel], TrafficModeMax);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        if (pCpgcPointTestCfg->AddrModes[Controller][Channel] > AddrModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: AddrMode value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->AddrModes[Controller][Channel], AddrModeMax);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        BurstLength = pCpgcPointTestCfg->BurstLengths[Controller][Channel];

        // IF the burst length exceeds the maximum linear value
        if (BurstLength > pCpgcInfo->MaxBurstLengthLinearVal) {
          UINT8 BurstLengthExponent;

          BurstLengthExponent = GetHighestBitSetInMask(BurstLength, 32);

          // IF the burst length is not a power of 2
          if (((UINT32) 1 << BurstLengthExponent) != BurstLength) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: BurstLength value (%u) exceeds maximum linear value (%u) and is not a power of 2.\n",
              BurstLength, pCpgcInfo->MaxBurstLengthLinearVal);
#endif
            ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
            return 1;
          }

          if (BurstLengthExponent > pCpgcInfo->MaxBurstLengthExponentVal) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: BurstLength value (%u) exceeds maximum exponential value (%u).\n",
              BurstLength, (1 << pCpgcInfo->MaxBurstLengthExponentVal));
#endif
            ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
            return 1;
          }
        } // end if the burst length exceeds the maximum linear value

        if (pCpgcPointTestCfg->LoopCounts[Controller][Channel] > pCpgcInfo->MaxLoopCountVal) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: LoopCount value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->LoopCounts[Controller][Channel], pCpgcInfo->MaxLoopCountVal);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        if (pCpgcPointTestCfg->BinarySearchLoopCounts[Controller][Channel] > pCpgcInfo->MaxLoopCountVal) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: BinarySearchLoopCount value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->BinarySearchLoopCounts[Controller][Channel], pCpgcInfo->MaxLoopCountVal);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        if (pCpgcPointTestCfg->StartDelays[Controller][Channel] > pCpgcInfo->MaxStartDelayVal) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: StartDelay value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->StartDelays[Controller][Channel], pCpgcInfo->MaxStartDelayVal);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        if (pCpgcPointTestCfg->InterSubseqWaits[Controller][Channel] > pCpgcInfo->MaxInterSubseqWaitVal) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: InterSubseqWait value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->InterSubseqWaits[Controller][Channel], pCpgcInfo->MaxInterSubseqWaitVal);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        // IF user defined DqDB patterns are enabled
        if (pCpgcPointTestCfg->EnableUserDefinedDqdbPatterns) {
          if ((pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] == 0) ||
            (pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] > USER_DEFINED_DQDB_PATTERN_CACHELINE_CNT)) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: UserDefinedDqdbPatternLength value (%u) is out of range (0 < val < %u).\n",
              pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel], USER_DEFINED_DQDB_PATTERN_CACHELINE_CNT);
#endif
            ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
            return 1;
          }

          if ((pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel] *
            (pCpgcPointTestCfg->UserDefinedDqbdPatternRotationCnts[Channel] + 1)) >
            pCpgcInfo->MaxNumberDqdbCachelines) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: Product of UserDefinedDqdbPatternLength and "
                "UserDefinedDqbdPatternRotationCnt+1 (%u and %u) values is out of range (1 <= val <= %u).\n",
                pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel],
                pCpgcPointTestCfg->UserDefinedDqbdPatternRotationCnts[Channel],
                pCpgcInfo->MaxNumberDqdbCachelines);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
          }
        } // end if user defined DqDB patterns are enabled

        if (pCpgcPointTestCfg->DqdbIncRateModes[Channel] > DqdbIncRateModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: DqdbIncRateMode value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->DqdbIncRateModes[Channel], DqdbIncRateModeMax);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        // IF the DqDB increment rate mode is linear
        if (pCpgcPointTestCfg->DqdbIncRateModes[Channel] == LinearDqdbIncRateMode) {
          if (pCpgcPointTestCfg->DqdbIncRates[Channel] > pCpgcInfo->MaxDqdbIncRateLinearVal) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: DqdbIncRate value (%u) is out of range (0 <= val <= %u).\n",
              pCpgcPointTestCfg->DqdbIncRates[Channel], pCpgcInfo->MaxDqdbIncRateLinearVal);
#endif
            ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
            return 1;
          }
        }
        // ELSE the DqDB increment rate mode is exponential
        else {
          if (pCpgcPointTestCfg->DqdbIncRates[Channel] > pCpgcInfo->MaxDqdbIncRateExponentVal) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: DqdbIncRate value (%u) is out of range (0 <= val <= %u).\n",
              pCpgcPointTestCfg->DqdbIncRates[Channel], pCpgcInfo->MaxDqdbIncRateExponentVal);
#endif
            ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
            return 1;
          }
        } // end else the DqDB increment rate mode is exponential

        // FOR each DqDB unisequencer:
        for (Uniseq = 0; Uniseq < MAX_DQDB_UNISEQ_CNT; Uniseq++) {
          if (pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq] > DqdbUniseqModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: DqdbUniseqMode value (%u) is out of range (0 <= val < %u).\n",
              pCpgcPointTestCfg->DqdbUniseqModes[Channel], DqdbUniseqModeMax);
#endif
            ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
            return 1;
          }

#if SUPPORT_FOR_TURNAROUNDS
          // IF the Wr2Rd turnaround traffic mode is specified
          if (pCpgcPointTestCfg->TrafficModes[Controller][Channel] == Wr2RdTrafficMode) {
            // only the LFSR unisequencer mode is supported
            if (pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq] != LfsrDqdbUniseqMode) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: DqdbUniseqMode value (%u) must be %s when TrafficMode=%u.\n",
                pCpgcPointTestCfg->DqdbUniseqModes[Channel], LfsrDqdbUniseqMode, Wr2RdTrafficMode);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }
          } //  end if the Wr2Rd turnaround traffic mode is specified
#endif // SUPPORT_FOR_TURNAROUNDS

          // IF the unisequencer is in LMN mode
          if (pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq] == LmnDqdbUniseqMode) {
            if (Uniseq != 0) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: Only DqdbUniseq 0 supports LmnDqdbUniseqMode.\n");
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }

            if (pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][0] > pCpgcInfo->MaxDqdbUniseqLVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: DqdbLmnValue L value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][0], pCpgcInfo->MaxDqdbUniseqLVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }

            if (pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][1] > pCpgcInfo->MaxDqdbUniseqMVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: DqdbLmnValue M value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][1], pCpgcInfo->MaxDqdbUniseqMVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }

            if (pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][2] > pCpgcInfo->MaxDqdbUniseqNVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: DqdbLmnValue N value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][2], pCpgcInfo->MaxDqdbUniseqNVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }
          }
          // ELSE the the unisequencer is not in LMN mode
          else {
            if (pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq] > pCpgcInfo->MaxDqdbUniseqSeedVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: DqdbUniseqSeed value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq], pCpgcInfo->MaxDqdbUniseqSeedVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }
          } // end else the the unisequencer is not in LMN mode
        } // end for each DqDB unisequencer:

        if (pCpgcPointTestCfg->CadbModes[Controller][Channel] > CadbModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: CadbMode value (%u) is out of range (0 <= val < %u).\n",
            pCpgcPointTestCfg->CadbModes[Controller][Channel], CadbModeMax);
#endif
          ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
          return 1;
        }

        // FOR each CADB unisequencer:
        for (Uniseq = 0; Uniseq < MAX_CADB_UNISEQ_CNT; Uniseq++) {
          if (pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Uniseq] > CadbUniseqModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: CadbUniseqMode value (%u) is out of range (0 <= val < %u).\n",
              pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Uniseq], CadbUniseqModeMax);
#endif
            ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
            return 1;
          }

          // IF the unisequencer is in LMN mode
          if ((Uniseq < MAX_CADB_LMN_UNISEQ_CNT) &&
            (pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Uniseq] == LmnCadbUniseqMode)) {
            if (pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][0] > pCpgcInfo->MaxCadbUniseqLVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: CadbLmnValue L value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][0], pCpgcInfo->MaxCadbUniseqLVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }

            if (pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][1] > pCpgcInfo->MaxCadbUniseqMVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: CadbLmnValue M value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][1], pCpgcInfo->MaxCadbUniseqMVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }

            if (pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][2] > pCpgcInfo->MaxCadbUniseqNVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: CadbLmnValue N value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][2], pCpgcInfo->MaxCadbUniseqNVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }
          }
          // ELSE the the unisequencer is not in LMN mode
          else {
            if (pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][Uniseq] > pCpgcInfo->MaxCadbUniseqSeedVal) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: CadbUniseqSeed value (%u) is out of range (0 <= val <= %u).\n",
                pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][Uniseq], pCpgcInfo->MaxCadbUniseqSeedVal);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
            }
          } // end else the the unisequencer is not in LMN mode
        } // end for each CADB unisequencer

#if SUPPORT_FOR_TURNAROUNDS
        // IF rank-to-rank turnaround testing is specified
        if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] == Turnarounds) {
          // IF the traffic mode is not valid for rank-to-rank turnaround testing
          if ((pCpgcPointTestCfg->TrafficModes[Controller][Channel] != Wr2WrAndRd2RdTrafficMode) &&
            (pCpgcPointTestCfg->TrafficModes[Controller][Channel] != Rd2WrTrafficMode) &&
            (pCpgcPointTestCfg->TrafficModes[Controller][Channel] != Wr2RdTrafficMode) &&
            (pCpgcPointTestCfg->TrafficModes[Controller][Channel] != IdleTrafficMode)) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: TrafficMode value (%u) is out of range for RankCombination=Turnarounds.\n",
                pCpgcPointTestCfg->TrafficModes[Controller][Channel]);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
          }
        }
        // ELSE singles testing is specified
        else
#endif // SUPPORT_FOR_TURNAROUNDS
        {
          // IF the traffic mode is not valid for singles testing
          if ((pCpgcPointTestCfg->TrafficModes[Controller][Channel] == Wr2WrAndRd2RdTrafficMode) ||
            (pCpgcPointTestCfg->TrafficModes[Controller][Channel] == Rd2WrTrafficMode) ||
            (pCpgcPointTestCfg->TrafficModes[Controller][Channel] == Wr2RdTrafficMode)) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: TrafficMode value (%u) is out of range for RankCombination=Singles.\n",
                pCpgcPointTestCfg->TrafficModes[Controller][Channel]);
#endif
              ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
              return 1;
          }
        } // end else singles testing is specified
      } // end for each channel
    } // end for each controller
  } // end for each socket

  return 0;
} // end function ChkCpgcPointTestCfg

/**
@brief
  This function is used to log the CPGC point test configuration.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pCpgcPointTestCfg  Pointer to test configuration structure.
**/
VOID LogCpgcPointTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg)
{
  UINT8 Controller, Channel, Cacheline, Chunk, Uniseq, Row, Idx, Signal;

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n"
    "CPGC Point Test Configuration:\n");
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  CpgcStopMode=%u\n", pCpgcPointTestCfg->CpgcStopMode);

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Controller=%u\n", Controller);
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    Channel=%u\n", Channel);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      TrafficMode=%u\n",
        pCpgcPointTestCfg->TrafficModes[Controller][Channel]);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      AddrMode=%u\n",
        pCpgcPointTestCfg->AddrModes[Controller][Channel]);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      BurstLength=%u\n",
        pCpgcPointTestCfg->BurstLengths[Controller][Channel]);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      LoopCount=%u\n",
        pCpgcPointTestCfg->LoopCounts[Controller][Channel]);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      BinarySearchLoopCount=%u\n",
        pCpgcPointTestCfg->BinarySearchLoopCounts[Controller][Channel]);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      StartDelay=%u\n",
        pCpgcPointTestCfg->StartDelays[Controller][Channel]);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      InterSubseqWait=%u\n",
        pCpgcPointTestCfg->InterSubseqWaits[Controller][Channel]);
    } // end for each channel
  } // end for each controller

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  EnableUserDefinedDqdbPatterns=%u\n", pCpgcPointTestCfg->EnableUserDefinedDqdbPatterns);

  // FOR each channel:
  for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Channel=%u\n", Channel);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    UserDefinedDqdbPatternLength=%u\n", pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel]);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    UserDefinedDqbdPatternRotationCnts=%u\n", pCpgcPointTestCfg->UserDefinedDqbdPatternRotationCnts[Channel]);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    UserDefinedDqdbPattern:\n");

    for (Cacheline = 0; Cacheline < USER_DEFINED_DQDB_PATTERN_CACHELINE_CNT; Cacheline++) {
      for (Chunk = 0; Chunk < CHUNKS_PER_CACHELINE; Chunk++) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      Cacheline=%u Chunk=%d 0x", Cacheline, Chunk);
        LogByteArrayAsHex(SsaServicesHandle,
          pCpgcPointTestCfg->UserDefinedDqdbPatterns[Channel][Cacheline][Chunk],
          (DQDB_PATTERN_WIDTH / 8));
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "\n");
      } // end for each chunk
    } // end for each cacheline

    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "      DqdbIncRateMode=%u\n",
      pCpgcPointTestCfg->DqdbIncRateModes[Channel]);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "      DqdbIncRate=%u\n",
      pCpgcPointTestCfg->DqdbIncRates[Channel]);
  } // end for each channel

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Controller=%u\n", Controller);
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    Channel=%u\n", Channel);
      for (Uniseq = 0; Uniseq < MAX_DQDB_UNISEQ_CNT; Uniseq++) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      DqdbUniseqModes[%d]=%u\n",
          Uniseq, pCpgcPointTestCfg->DqdbUniseqModes[Controller][Channel][Uniseq]);
      }
      for (Uniseq = 0; Uniseq < MAX_DQDB_UNISEQ_CNT; Uniseq++) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      DqdbUniseqSeeds[%d]=0x%X\n",
          Uniseq, pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);
      }
      for (Uniseq = 0; Uniseq < MAX_DQDB_LMN_UNISEQ_CNT; Uniseq++) {
        for (Idx = 0; Idx < MAX_DQDB_LMN_VALUES_CNT; Idx++) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "      DqdbLmnValues[%d][%d]=%u\n",
            Uniseq, Idx, pCpgcPointTestCfg->DqdbLmnValues[Controller][Channel][Uniseq][Idx]);
        }
      }

      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      CadbMode=%u\n",
        pCpgcPointTestCfg->CadbModes[Controller][Channel]);
    } // end for each channel
  } // end for each controller

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  EnableUserDefinedCadbPatterns=%u\n", pCpgcPointTestCfg->EnableUserDefinedCadbPatterns);

  // FOR each channel:
  for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Channel=%u\n", Channel);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    UserDefinedCadbPattern:\n");

    for (Row = 0; Row < USER_DEFINED_CADB_PATTERN_ROW_CNT; Row++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      Row=%d 0x", Row);
      LogByteArrayAsHex(SsaServicesHandle,
        pCpgcPointTestCfg->UserDefinedCadbPatterns[Channel][Row],
        (CADB_PATTERN_WIDTH / 8));
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "\n");
    } // end for each chunk
  } // end for each channel

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Controller=%u\n", Controller);
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    Channel=%u\n", Channel);
      for (Idx = 0; Idx < MAX_CADB_UNISEQ_CNT; Idx++) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      CadbUniseqModes[%d]=%u\n",
          Idx, pCpgcPointTestCfg->CadbUniseqModes[Controller][Channel][Idx]);
      }
      for (Idx = 0; Idx < MAX_CADB_UNISEQ_CNT; Idx++) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      CadbUniseqSeeds[%d]=0x%X\n",
          Idx, pCpgcPointTestCfg->CadbUniseqSeeds[Controller][Channel][Idx]);
      }
      for (Uniseq = 0; Uniseq < MAX_CADB_LMN_UNISEQ_CNT; Uniseq++) {
        for (Idx = 0; Idx < MAX_CADB_LMN_VALUES_CNT; Idx++) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "      CadbLmnValues[%d][%d]=%u\n",
            Uniseq, Idx, pCpgcPointTestCfg->CadbLmnValues[Controller][Channel][Uniseq][Idx]);
        }
      }
    } // end for each channel
  } // end for each controller

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "      EnableErid=%u\n", pCpgcPointTestCfg->EnableErid);
  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Controller=%u\n", Controller);
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    Channel=%u\n", Channel);
      //  FOR each signal:
      for (Signal = 0; Signal < EridSignalMax; Signal++) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    Signal=%u\n", Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      EridPatternSeeds[%u][%u][%u]=0x%X\n",
          Controller, Channel, Signal,
          pCpgcPointTestCfg->EridPatternSeeds[Controller][Channel][Signal]);
      } // end for each signal
    } // end for each channel
  } // end for each controller

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "      EnableTimeout=%u\n", pCpgcPointTestCfg->EnableTimeout);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n");
} // end function LogCpgcPointTestCfg

/**
@brief
  This function is used to set up the CPGC point test.

  This function conforms to the SETUP_MEM_POINT_TEST function typedef in
  MemPointTest.h.

  If the IsFirstCall input parameter is set to TRUE then this function will
  allocate memory for the CpgcPointTest's private test status data structure,
  otherwise it will treat the ppPointTestData parameter as an input.

  If the InitDqdbContent input parameter is set to TRUE then this function will
  initialize the contents of the WDB, otherwise, it won't.

  @param[in, out]  SsaServicesHandle   Pointer to SSA services.
  @param[in]       pSystemInfo         Pointer to system information structure.
  @param[in]       pCpgcInfo           Pointer to CPGC information structure.
  @param[in]       pMemCfg             Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg    Pointer to generic memory point test configuration structure.
  @param[in]       pCpgcPointTestCfg   Pointer to CPGC point test configuration structure.
  @param[in]       ResultHandler       Pointer to result handler function.
  @param[in]       pClientTestData     Pointer to client test data structure.
  @param[in]       IoLevel             I/O level.
  @param[in]       IsDdrT              Flag indicating whether testing is for DDR-T devices.
  @param[in]       TestStatusLogLevel  Test status logging level.
  @param[in]       IsFirstCall         Flag indicating whether the call is the first.
  @param[in]       InitDqdbContent     Flag indicating whether to initialize the DQDB contents.
  @param[in, out]  ppPointTestData     Pointer to pointer to specific memory point test data
  @param[out]      pTestStat           Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 SetupCpgcPointTest(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MRC_SERVER_CPGC_INFO *pCpgcInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  IN MEM_POINT_TEST_RESULT_HANDLER ResultHandler,
  IN VOID *pClientTestData,
  IN GSM_LT IoLevel,
  IN BOOLEAN IsDdrT,
  IN UINT8 TestStatusLogLevel,
  IN BOOLEAN IsFirstCall,
  IN BOOLEAN InitDqdbContent,
  IN OUT VOID **ppVoidPointTestData,
  OUT TEST_STATUS *pTestStat)
{
  CPGC_POINT_TEST_DATA *pPointTestData;
  UINT8 TestedChnlCnt;

#if SUPPORT_FOR_TURNAROUNDS
  CPGC_ADDRESS TurnaroundCpgcAddr0 = { // rank-to-rank turnaround test
    //                 Rank,              Bank,              Row,               Col
    /* Start       */ {0,                 0,                 0,                 0},
    /* Wrap        */ {1,                 0,                 0,                 0x7F}, // the row wrap address will be runtime updated based on the DIMM info
    /* Order       */ {0,                 0,                 0,                 0},
    /* IncMode     */ {ExponentialAddrIncMode, LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode},
    /* IncRate     */ {0,                 0,                 0,                 1},   // R0:A0, R1:A0, R0:A1, R1:A1, R0:A2, R1:A2, ..
    /* IncVal      */ {1,                 0,                 0,                 1},
    /* CarryEnable */ {FALSE,             FALSE,             FALSE,             FALSE}
  };
  CPGC_ADDRESS TurnaroundCpgcAddr1 = { // rank-to-rank turnaround test
    //                 Rank,              Bank,              Row,               Col
    /* Start       */ {0,                 0,                 0,                 0},
    /* Wrap        */ {1,                 0,                 0,                 0x7F}, // the row wrap address will be runtime updated based on the DIMM info
    /* Order       */ {0,                 0,                 0,                 0},
    /* IncMode     */ {LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode, LinearAddrIncMode},
    /* IncRate     */ {1,                 0,                 0,                 3},   // R0:A0, R1:A0, R0:A1, R1:A1, R0:A2, R1:A2, ..
    /* IncVal      */ {1,                 0,                 0,                 1},
    /* CarryEnable */ {FALSE,             FALSE,             FALSE,             FALSE}
  };
#endif // SUPPORT_FOR_TURNAROUNDS

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "SetupCpgcPointTest(IoLevel=%u IsDdrT=%u TestStatusLogLevel=%u ",
    IoLevel, IsDdrT, TestStatusLogLevel);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsFirstCall=%u InitDqdbContent=%u)\n",
    IsFirstCall, InitDqdbContent);
#endif

  // IF this is the first call to this function
  if (IsFirstCall) {
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    GetMemPoolSize(SsaServicesHandle);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Malloc(Size=%u) for CPGC point test data\n", sizeof(*pPointTestData));
#endif
    pPointTestData = (CPGC_POINT_TEST_DATA *)SsaServicesHandle->SsaCommonConfig->Malloc(
      SsaServicesHandle, sizeof(*pPointTestData));
    if (pPointTestData == NULL) {
#if ENBL_VERBOSE_ERROR_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "ERROR: Could not allocate memory for point test data.\n");
#endif
      ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
      return 1;
    }
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    pBfr=%p\n", pPointTestData);
    GetMemPoolSize(SsaServicesHandle);
#endif

    *ppVoidPointTestData = pPointTestData;

    bssa_memset(pPointTestData, 0, sizeof(*pPointTestData));
  }
  // ELSE this is not the first call to this function
  else {
    // use the previously allocated buffer
    pPointTestData = (CPGC_POINT_TEST_DATA *)(*ppVoidPointTestData);
  }

  pPointTestData->TestStatusLogLevel = TestStatusLogLevel;
  pPointTestData->ResultHandler = ResultHandler;
  pPointTestData->pClientTestData = pClientTestData;
  pPointTestData->IoLevel = IoLevel;
  pPointTestData->IsDdrT = IsDdrT;
#if SUPPORT_FOR_TURNAROUNDS
  bssa_memcpy(&pPointTestData->TurnaroundCpgcAddr0, &TurnaroundCpgcAddr0,
    sizeof(TurnaroundCpgcAddr0));
  bssa_memcpy(&pPointTestData->TurnaroundCpgcAddr1, &TurnaroundCpgcAddr1,
    sizeof(TurnaroundCpgcAddr1));
#endif // SUPPORT_FOR_TURNAROUNDS

  // get the count of enabled channels (add one because index is zero base)
  TestedChnlCnt = GetTestChnlIndex(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, (MAX_SOCKET_CNT - 1), (MAX_CONTROLLER_CNT - 1),
    (MAX_CHANNEL_CNT - 1)) + 1;

  // IF this is not the first call to this function and the number of tested
  // channels has increased
  if (!IsFirstCall && (pPointTestData->pChannelTestData != NULL) &&
    (TestedChnlCnt > pPointTestData->TestedChnlCnt)) {
    // release the existing buffer
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
      GetMemPoolSize(SsaServicesHandle);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Free(pBfr=%p)\n", pPointTestData->pChannelTestData);
#endif
    SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle,
      pPointTestData->pChannelTestData);
    pPointTestData->pChannelTestData = NULL;
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    GetMemPoolSize(SsaServicesHandle);
#endif
  } // end if this is not the first call to this function and ...

  // save the number of tested channels value
  pPointTestData->TestedChnlCnt = TestedChnlCnt;

  // IF we need to allocate a buffer for the channel test data
  if ((pPointTestData->TestedChnlCnt != 0) &&
    (pPointTestData->pChannelTestData == NULL)) {
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    GetMemPoolSize(SsaServicesHandle);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Malloc(Size=%u) for CPGC per channel test data array\n",
      pPointTestData->TestedChnlCnt * sizeof(CPGC_PER_CHNL_POINT_TEST_DATA));
#endif
    pPointTestData->pChannelTestData = (CPGC_PER_CHNL_POINT_TEST_DATA *) \
      SsaServicesHandle->SsaCommonConfig->Malloc(SsaServicesHandle,
      (pPointTestData->TestedChnlCnt * sizeof(CPGC_PER_CHNL_POINT_TEST_DATA)));
    if (pPointTestData->pChannelTestData == NULL) {
#if ENBL_VERBOSE_ERROR_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "ERROR: Could not allocate memory for per channel test data\n");
#endif
      ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
      return 1;
    }
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    pBfr=%p\n", pPointTestData->pChannelTestData);
    GetMemPoolSize(SsaServicesHandle);
#endif

    bssa_memset(pPointTestData->pChannelTestData, 0,
      pPointTestData->TestedChnlCnt * sizeof(CPGC_PER_CHNL_POINT_TEST_DATA));
  } // end if we need to allocate a buffer for the channel test data

  if (GetRankCombinationsCnts(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, IoLevel, IsDdrT, &pPointTestData->RankCombinationsCnts,
    &pPointTestData->MaxRankCombinationsCnt, pTestStat)) {
    return 1;
  }

  // initialize the CPGC engine/MC control
  InitCpgcEngine(SsaServicesHandle, pSystemInfo, pMemCfg, pMemPointTestCfg,
    IsFirstCall);

  // initialize stop mode
  InitCpgcStopMode(SsaServicesHandle, pSystemInfo, pMemCfg, pMemPointTestCfg,
    pCpgcPointTestCfg);

  // initialize the DqDB pattern and control
  if (InitDqdb(SsaServicesHandle, pSystemInfo, pCpgcInfo, pMemCfg,
    pMemPointTestCfg, pCpgcPointTestCfg, InitDqdbContent, pTestStat)) {
    return 1;
  }

  // initialize the ERID pattern and control
  InitEridPattern(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, pCpgcPointTestCfg);

  // initialize the CADB pattern
  if (InitCadbPattern(SsaServicesHandle, pSystemInfo,
    pMemCfg, pMemPointTestCfg, pCpgcPointTestCfg, pTestStat)) {
    return 1;
  }

  // initialize the error counters and validation masks
  InitValidation(SsaServicesHandle, pSystemInfo, pMemCfg, pMemPointTestCfg,
    pPointTestData);

  // initialize the ERID error counters and validation masks
  InitEridValidation(SsaServicesHandle, pSystemInfo, pMemCfg, pMemPointTestCfg,
    pCpgcPointTestCfg);

  return 0;
} // end function SetupCpgcPointTest

/**
@brief
  This function is used to get the size of required CpgcPointTest dynamic memory.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pCpgcPointTestCfg  Pointer to CPGC point test configuration structure.
  @param[in]       TestedChnlCnt      Number of channels being tested.

  @retval  dynamic memory size
**/
UINT32 GetCpgcPointTestDynamicMemSize(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg,
  IN UINT8 TestedChnlCnt)
{
  UINT8 Channel;
  UINT8 CachelineCount = 0;
  UINT32 MemSize = 0;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "GetCpgcPointTestDynamicMemSize(TestedChnlCnt=%u)\n", TestedChnlCnt);
#endif

  // IF the user defined DqDB pattern is enabled
  if (pCpgcPointTestCfg->EnableUserDefinedDqdbPatterns) {
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      // IF the current maximum is less than this channel's value
      if (CachelineCount < pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel]) {
        // this channel's value is the new maximum
        CachelineCount = pCpgcPointTestCfg->UserDefinedDqdbPatternLengths[Channel];
      }
    } // end for each channel
  }
  // ELSE use the BKM "victim/aggressor" pattern
  else {
    CachelineCount = 8;
  } // end else use the BKM "victim/aggressor" pattern

  // for initializing the DQDB contents
  MemSize = GetActualMallocSize(CachelineCount * CHUNKS_PER_CACHELINE * sizeof(UINT64));

  // for "pPointTestData"
  MemSize += GetActualMallocSize(sizeof(CPGC_POINT_TEST_DATA));

  // for "pPointTestData->pChannelTestData"
  MemSize += GetActualMallocSize(TestedChnlCnt * sizeof(CPGC_PER_CHNL_POINT_TEST_DATA));

  // a bit more for good measure
  MemSize += 100;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "  MemSize=%u\n", MemSize);
#endif

  return MemSize;
} // end function GetCpgcPointTestDynamicMemSize

/**
@brief
  This function is used to get the DIMM/rank pairs for a given channel.

  This function conforms to the GET_MEM_POINT_TEST_DIMM_RANKS typedef in
  MemPointTest.h.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in]       pPointTestData     Pointer to specific memory point test data structure.
  @param[in]       Socket             Zero based CPU socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in, out]  pPairCount         Number of entries in the DimmRankPairs array.  Input=number of elements in pDimmRankPairs.  Output=number of populated elements in pDimmRankPairs.
  @param[out]      pDimmRankPairs     Array of DIMM and physical rank pair values.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 GetCpgcPointTestDimmRanks(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN VOID *pVoidPointTestData,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN OUT UINT8 *pPairCount,
  OUT DIMM_RANK_PAIR *pDimmRankPairs,
  OUT TEST_STATUS *pTestStat)
{
  CPGC_POINT_TEST_DATA *pPointTestData = (CPGC_POINT_TEST_DATA*) pVoidPointTestData;
  UINT8 PairCountInput = *pPairCount;
  UINT8 TestedChnlIdx;

  UNREFERENCED_PARAMETER(SsaServicesHandle);

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "GetCpgcPointTestDimmRanks(Socket=%u Controller=%u Channel=%u)\n",
    Socket, Controller, Channel);
#endif

  // IF the channel is not enabled for testing
  if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, Socket, Controller, Channel)) {
    *pPairCount = 0;
  }
  // ELSE the channel is enabled for testing
  else {
    TestedChnlIdx = GetTestChnlIndex(SsaServicesHandle, pSystemInfo, pMemCfg,
      pMemPointTestCfg, Socket, Controller, Channel);

    // IF there are sufficient DIMM/rank pairs
    if (PairCountInput >= 1) {
      *pPairCount = 1;
      bssa_memcpy(&pDimmRankPairs[0],
        &pPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA,
        sizeof(DIMM_RANK_PAIR));

#if SUPPORT_FOR_TURNAROUNDS
      if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] == Turnarounds) {
        // IF there are sufficient DIMM/rank pairs
        if (PairCountInput >= 2) {
          *pPairCount = 2;
          bssa_memcpy(&pDimmRankPairs[1],
            &pPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankB,
            sizeof(DIMM_RANK_PAIR));
        }
        // ELSE there are insufficient DIMM/rank pairs
        else {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: Insufficient DIMM/rank pairs\n");
#endif
          ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
          return 1;
        }
      } // end if this channel is doing rank-to-rank turnarounds
#endif // SUPPORT_FOR_TURNAROUNDS
    }
    // ELSE there are insufficient DIMM/rank pairs
    else {
#if ENBL_VERBOSE_ERROR_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "ERROR: Insufficient DIMM/rank pairs\n");
#endif
      ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
      return 1;
    }
  } // end else the channel is enabled for testing

  return 0;
} // end function GetCpgcPointTestDimmRanks

/**
@brief
  This function is used to update the pointtest data with the "current" dimm/rank pair 
  based on the the rank iteration index.
  This function configures that hardware to run the point test for the given
  rank index.

  This function conforms to the SET_MEM_POINT_TEST_RANK_COMBINATION_INDEX
  typedef in MemPointTest.h.

  @param[in, out]  SsaServicesHandle     Pointer to SSA services.
  @param[in]       pSystemInfo           Pointer to system information structure.
  @param[in]       pMemCfg               Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg      Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg         Pointer to specific memory point test configuration structure.
  @param[in]       pPointTestData        Pointer to specific memory point test data structure.
  @param[in]       RankCombinationIndex  Rank combination index.
  @param[in]       UpdateHwSetting       Flag to indicate if need to apply the setting to register or not
                                         FALSE: only update on the rankCombinartionIndex update the 
                                         "Current" dimm/rank pairs of the point test data.
                                         TRUE: will retrieve the "current" dimm/rank paris and program them
                                         to HW registers.
                                         NOTE: the function need to be called at least twice. The first
                                         with this parameter as FALSE, then later called with this parameter 
                                         as TRUE.
  @param[out]      pTestStat             Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 SetCpgcPointTestRankCombinationIndex(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pVoidPointTestCfg,
  IN VOID *pVoidPointTestData,
  IN UINT8 RankCombinationIndex,
  IN BOOLEAN UpdateHwSetting,
  OUT TEST_STATUS *pTestStat)
{
  CPGC_POINT_TEST_CONFIG* pCpgcPointTestCfg = (CPGC_POINT_TEST_CONFIG*) pVoidPointTestCfg;
  CPGC_POINT_TEST_DATA* pCpgcPointTestData = (CPGC_POINT_TEST_DATA*) pVoidPointTestData;
  UINT8 Socket, Controller, Channel;
  UINT8 ChnlRankCombinationIndex;
  UINT8 TestedChnlIdx;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "SetCpgcPointTestRankCombinationIndex(RankCombinationIndex=%u, UpdateHwSetting=%u)\n",
    RankCombinationIndex, UpdateHwSetting);
#endif

  if (UpdateHwSetting){
    // program the rank setting 
    if (SetCrntRanks(SsaServicesHandle, pSystemInfo, pMemCfg,
      pMemPointTestCfg, pCpgcPointTestCfg, pCpgcPointTestData, pTestStat)) {
        return 1;
    }
    // Reprogram the test seq before we need to change the subseq_wait time when switch DDR4 and DDRT dimms.
    // To use the SetSubSeqConfig() API to modify the subseq_wait time, we need to know the burst length,
    // subseq_type, tec. Those info is available only in the InitTestSeq(). Thus we need to call InitTestSeq()
    // here.
    InitTestSeq(SsaServicesHandle, pSystemInfo, pMemCfg, pMemPointTestCfg, pCpgcPointTestCfg, pCpgcPointTestData);

    // Modify the CadbMode. If DDRT we need to set to NormalMode.
    // At the point test level, it doesn't know the margin parameter so it can't selectively skip this call. 
    // Ideally, only when in Cmd/CmdVref/Ctl test case we need to call it.
    InitCadbCtrl(SsaServicesHandle, pSystemInfo, pMemCfg, pMemPointTestCfg, pCpgcPointTestCfg, pCpgcPointTestData);
  }// UpdateHwSetting
  else {
    if (GetRankCombinationsCnts(SsaServicesHandle, pSystemInfo, pMemCfg,
      pMemPointTestCfg, pCpgcPointTestData->IoLevel, pCpgcPointTestData->IsDdrT,
      &pCpgcPointTestData->RankCombinationsCnts,
      &pCpgcPointTestData->MaxRankCombinationsCnt, pTestStat)) {
      return 1;
    }

    pCpgcPointTestData->RankCombinationsIndex = RankCombinationIndex;

    // FOR each socket
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // FOR each controller:
      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
        // FOR each channel:
        for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
          // IF this channel is not enabled for testing THEN skip it
          if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel)) {
            continue;
          }

          TestedChnlIdx = GetTestChnlIndex(SsaServicesHandle, pSystemInfo,
            pMemCfg, pMemPointTestCfg, Socket, Controller, Channel);

          //NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          //  "CpgcPointTest.SetCpgcPointTestRankCombinationIndex(Socket:%u controller:%u channel: %u TestChnlIdx=%u)\n", Socket, Controller, Channel, TestedChnlIdx);

          // IF this channel has more than one rank combination
          if (pCpgcPointTestData->RankCombinationsCnts[Socket][Controller][Channel] > 0) {
            // limit the rank iteration index to the maximum number supported by
            // the given channel
            ChnlRankCombinationIndex = RankCombinationIndex % \
              pCpgcPointTestData->RankCombinationsCnts[Socket][Controller][Channel];

            GetDimmRankPerCombinationIndex(SsaServicesHandle,pSystemInfo, pMemCfg,pMemPointTestCfg,
              Socket, Controller, Channel, ChnlRankCombinationIndex, 
              &pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankA,
              &pCpgcPointTestData->pChannelTestData[TestedChnlIdx].CrntDimmRankB);

          } // end if this channel has more than one rank combination
        } // end for each channel
      } // end for each controller
    } // end for each socket
  } // else not update HW, but update the CrntDimmRankA/B of the point test data.
  return 0;
} // end function SetCpgcPointTestRankCombinationIndex

/**
@brief
  This function is used to set the flag indicating whether we're in the binary
  search state.  It is intended that the point test runs for a shorter period
  of time in the binary search state.

  This function conforms to the SET_MEM_POINT_TEST_IN_BINARY_SEARCH_STATE
  typedef in MemPointTest.h.

  @param[in, out]  SsaServicesHandle    Pointer to SSA services.
  @param[in]       pSystemInfo          Pointer to system information structure.
  @param[in]       pMemCfg              Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg     Pointer to generic memory point test configuration structure.
  @param[in]       pVoidPointTestCfg    Pointer to specific memory point test configuration structure.
  @param[in]       pVoidPointTestData   Pointer to specific memory point test data structure.
  @param[in]       InBinarySearchState  Flag indicating whether we're in the binary search state.
  @param[out]      pTestStat            Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 SetCpgcPointTestInBinarySearchState(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pVoidPointTestCfg,
  IN VOID *pVoidPointTestData,
  IN BOOLEAN InBinarySearchState,
  OUT TEST_STATUS *pTestStat)
{
  CONST CPGC_POINT_TEST_CONFIG* pCpgcPointTestCfg = (const CPGC_POINT_TEST_CONFIG*) pVoidPointTestCfg;
  CPGC_POINT_TEST_DATA *pPointTestData = (CPGC_POINT_TEST_DATA*) pVoidPointTestData;
  UINT8 Socket, Controller, Channel;
  UINT32 LoopCount;
 
  UNREFERENCED_PARAMETER(pTestStat);

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "SetCpgcPointTestInBinarySearchState(InBinarySearchState=%u)\n", InBinarySearchState);
#endif
  pPointTestData->InBinarySearchState = InBinarySearchState;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        // IF this channel is idle THEN skip it
        if (pCpgcPointTestCfg->TrafficModes[Controller][Channel] ==
          IdleTrafficMode) {
          continue;
        }

        LoopCount = (InBinarySearchState) ? \
          pCpgcPointTestCfg->BinarySearchLoopCounts[Controller][Channel] : \
          pCpgcPointTestCfg->LoopCounts[Controller][Channel];

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetLoopCount(Socket=%u Controller=%u Channel=%u LoopCount=%u)\n",
          Socket, Controller, Channel, LoopCount);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetLoopCount(
          SsaServicesHandle, Socket, Controller, Channel, LoopCount);
      } // end for each channel
    } // end for each controller
  } // end for each socket

  return 0;
} // end function SetCpgcPointTestInBinarySearchState

/**
@brief
  This function is used to run a CPGC point test.

  This function conforms to the RUN_MEM_POINT_TEST typedef in MemPointTest.h.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pVoidPointTestCfg  Pointer to specific memory point test configuration structure.
  @param[in]       pVoidPointTestData Pointer to specific memory point test data structure.
  @param[out]      pLaneFailStats     Pointer to lane failure status array.  Indexed as [Socket][Controller][Channel].  (Value may be NULL.)
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 RunCpgcPointTest(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pVoidPointTestCfg,
  IN VOID *pVoidPointTestData,
  OUT LANE_FAIL_STATS *pLaneFailStats,
  OUT TEST_STATUS *pTestStat)
{
  CONST CPGC_POINT_TEST_CONFIG* pCpgcPointTestCfg = (const CPGC_POINT_TEST_CONFIG*) pVoidPointTestCfg;
  CPGC_POINT_TEST_DATA *pPointTestData = (CPGC_POINT_TEST_DATA*) pVoidPointTestData;
  UINT8 Socket;
#if SUPPORT_FOR_TURNAROUNDS
  UINT8 Controller, Channel;
  UINT8 Uniseq;
  UINT8 SubseqStartIndex, SubseqEndIndex;
  UINT32 DqdbUniseqSeed;
  UINT32 LoopCount;
  BOOLEAN EnableGlobalControl;
  TRAFFIC_MODE TrafficMode;
  BOOLEAN IsAnyWr2Rd = FALSE;
#endif // #if SUPPORT_FOR_TURNAROUNDS

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "RunCpgcPointTest()\n");
#endif

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF the socket is not enabled for testing THEN skip it
    if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
      pMemPointTestCfg, Socket)) continue;

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  ResetAepFifoTrain(Socket=%u)\n", Socket);
#endif
    SsaServicesHandle->SsaMemoryDdrtConfig->ResetAepFifoTrain(
      SsaServicesHandle, Socket);
  } // end for each socket

#if SUPPORT_FOR_TURNAROUNDS
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // check if any channel's traffic mode is write-to-read rank-to-rank turnarounds
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        // IF this channel's traffic mode is write-to-read rank-to-rank turnarounds
        if (pCpgcPointTestCfg->TrafficModes[Controller][Channel] == Wr2RdTrafficMode) {
          IsAnyWr2Rd = TRUE;
          break;
        }
      } // end for each channel

      if (IsAnyWr2Rd) {
        break;
      }
    } // end for each controller

    if (IsAnyWr2Rd) {
      break;
    }
  } // end for each socket

  // IF at least one channel is configured for write-to-read rank-to-rank
  // turnarounds
  if (IsAnyWr2Rd) {
    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // FOR each controller:
      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
        // FOR each channel:
        for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
          // IF this channel is not enabled for testing THEN skip it
          if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel)) {
            continue;
          }

          TrafficMode = pCpgcPointTestCfg->TrafficModes[Controller][Channel];

          // IF this channel's traffic mode is write-to-read rank-to-rank turnarounds
          if (TrafficMode == Wr2RdTrafficMode) {
            // setup to perform the initial write operation
            SubseqStartIndex = 0;
            SubseqEndIndex = 0;
            EnableGlobalControl = TRUE;

            // set the address schema for the first part
            SetSeqAddr(SsaServicesHandle, Socket, Controller, Channel,
              &pPointTestData->TurnaroundCpgcAddr0);

            // restore the write seed
            // FOR each DqDB unisequencer:
            for (Uniseq = 0; Uniseq < MAX_DQDB_UNISEQ_CNT; Uniseq++) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "SetDqdbUniseqWrSeed(Socket=%u Controller=%u Channel=%u Uniseq=%u Seed=0x%06X)\n",
                Socket, Controller, Channel, Uniseq,
                pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);
#endif
              SsaServicesHandle->SsaMemoryServerConfig->SetDqdbUniseqWrSeed(
                SsaServicesHandle, Socket, Controller, Channel, Uniseq,
                pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);
            } // end for each DqQD unisequencer
          }
          // ELSE IF the traffic mode is Idle THEN skip it
          else if (TrafficMode == IdleTrafficMode) {
            continue;
          }
          else {
            // temporarily disable global control
            SubseqStartIndex = 0;
            SubseqEndIndex = SequencersByTrafficMode[TrafficMode].SubSequenceEnd;
            EnableGlobalControl = FALSE;
          }

          // only one loop for the first part
          LoopCount = 1;

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "SetSeqConfig(Socket=%u Controller=%u Channel=%u StartDelay=%u\n",
            Socket, Controller, Channel, pCpgcPointTestCfg->StartDelays[Controller][Channel]);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SubseqStartIndex=%u SubseqEndIndex=%u LoopCount=0x%X InitMode=%u\n",
            SubseqStartIndex, SubseqEndIndex, LoopCount, ReutCpgcInitMode);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  EnableGlobalControl=%u EnableConstantWriteStrobe=%u EnableDummyReads=%u\n",
            EnableGlobalControl, FALSE, FALSE);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  AddrUpdateRateMode=%u)\n",
            CachelineAddrUpdateRate);
#endif
          SsaServicesHandle->SsaMemoryServerConfig->SetSeqConfig(
            SsaServicesHandle, Socket, Controller, Channel,
            pCpgcPointTestCfg->StartDelays[Controller][Channel],
            SubseqStartIndex, SubseqEndIndex, LoopCount, ReutCpgcInitMode,
            EnableGlobalControl, FALSE, FALSE, CachelineAddrUpdateRate);
        } // end for each channel
      } // end for each controller
    } // end for each socket

    // perform the initial write operation
    if (RunPointTest(SsaServicesHandle, pSystemInfo, pMemCfg,
      pMemPointTestCfg, pCpgcPointTestCfg, pPointTestData, pTestStat)) {
      return 1;
    }

    EnableGlobalControl = TRUE;

    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // FOR each controller:
      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
        // FOR each channel:
        for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
          // IF this channel is not enabled for testing THEN skip it
          if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel)) {
            continue;
          }

          TrafficMode = pCpgcPointTestCfg->TrafficModes[Controller][Channel];

          // IF this channel's traffic mode is write-to-read rank-to-rank turnarounds
          if (TrafficMode == Wr2RdTrafficMode) {
            // setup to perform the base-read-write operations
            SubseqStartIndex = 1;
            SubseqEndIndex = 1;

            // set the address schema for the second part
            SetSeqAddr(SsaServicesHandle, Socket, Controller, Channel,
              &pPointTestData->TurnaroundCpgcAddr1);

            // FOR each DqDB unisequencer:
            for (Uniseq = 0; Uniseq < MAX_DQDB_UNISEQ_CNT; Uniseq++) {
              // read the current DQDB write seed
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "GetDqdbUniseqWrSeed(Socket=%u Controller=%u Channel=%u Uniseq=%u)\n",
                Socket, Controller, Channel, Uniseq,
                pCpgcPointTestCfg->DqdbUniseqSeeds[Controller][Channel][Uniseq]);
#endif
              SsaServicesHandle->SsaMemoryServerConfig->GetDqdbUniseqWrSeed(
                SsaServicesHandle, Socket, Controller, Channel, Uniseq,
                &DqdbUniseqSeed);
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  Seed=0x%06X\n", DqdbUniseqSeed);
#endif

              // set the current DQDB write seed
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "SetDqdbUniseqWrSeed(Socket=%u Controller=%u Channel=%u Uniseq=%u Seed=0x%06X)\n",
                Socket, Controller, Channel, Uniseq, DqdbUniseqSeed);
#endif
              SsaServicesHandle->SsaMemoryServerConfig->SetDqdbUniseqWrSeed(
                SsaServicesHandle, Socket, Controller, Channel, Uniseq,
                DqdbUniseqSeed);
            } // end for each DqQD unisequencer
          }
          // ELSE IF the traffic mode is Idle THEN skip it
          else if (TrafficMode == IdleTrafficMode) {
            continue;
          }
          else {
            // re-enable global control
            SubseqStartIndex = 0;
            SubseqEndIndex = SequencersByTrafficMode[TrafficMode].SubSequenceEnd;
          }

          // full loop count for the second part
          LoopCount = pCpgcPointTestCfg->LoopCounts[Controller][Channel];

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "SetSeqConfig(Socket=%u Controller=%u Channel=%u StartDelay=%u\n",
            Socket, Controller, Channel, pCpgcPointTestCfg->StartDelays[Controller][Channel]);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SubseqStartIndex=%u SubseqEndIndex=%u LoopCount=0x%X InitMode=%u\n",
            SubseqStartIndex, SubseqEndIndex, LoopCount, ReutCpgcInitMode);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  EnableGlobalControl=%u EnableConstantWriteStrobe=%u EnableDummyReads=%u\n",
            EnableGlobalControl, FALSE, FALSE);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  AddrUpdateRateMode=%u)\n",
            CachelineAddrUpdateRate);
#endif
          SsaServicesHandle->SsaMemoryServerConfig->SetSeqConfig(
            SsaServicesHandle, Socket, Controller, Channel,
            pCpgcPointTestCfg->StartDelays[Controller][Channel],
            SubseqStartIndex, SubseqEndIndex, LoopCount, ReutCpgcInitMode,
            EnableGlobalControl, FALSE, FALSE, CachelineAddrUpdateRate);
        } // end for each channel
      } // end for each controller
    } // end for each socket
  } // end if at least one channel is configured for write-to-read ...
#endif // SUPPORT_FOR_TURNAROUNDS

  // initialize the lane failure status array
  bssa_memset(pPointTestData->LaneFailStats, 0,  sizeof(pPointTestData->LaneFailStats));

  // run a point test with the current rank combination
  if (RunPointTest(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, pCpgcPointTestCfg, pPointTestData, pTestStat)) {
    return 1;
  }

  if (pLaneFailStats != NULL) {
    bssa_memcpy(pLaneFailStats, &pPointTestData->LaneFailStats, sizeof(*pLaneFailStats));
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) continue;

        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  Socket=%u Controller=%u Channel=%u AnyLaneFailed=%u AllLanesFailed=%u\n",
          Socket, Controller, Channel,
          pPointTestData->LaneFailStats[Socket][Controller][Channel].AnyLaneFailed,
          pPointTestData->LaneFailStats[Socket][Controller][Channel].AllLanesFailed);
      } // end for each channel
    } // end for each controller
  } // end for each socket
#endif

  return 0;
} // end function RunCpgcPointTest

/**
@brief
  This function is used to clean up the CPGC point test.

  This function conforms to the CLEANUP_MEM_POINT_TEST function typedef in
  MemPointTest.h.

  If the IsLastCall input parameter is set to TRUE then this function will free
  the memory for the CpgcPointTest's private test status data structure and set
  the pointer to NULL, otherwise it will not.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IsLastCall         Flag indicating whether the call is the last.
  @param[in, out]  ppPointTestData    Pointer to pointer to specific memory point test data structure.
**/
VOID CleanupCpgcPointTest(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN BOOLEAN IsLastCall,
  IN OUT VOID **ppVoidPointTestData)
{
  CPGC_POINT_TEST_DATA *pPointTestData = *((CPGC_POINT_TEST_DATA**) ppVoidPointTestData);
  //UINT8 Socket, Controller;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CleanupCpgcPointTest(IsLastCall=%u)\n", IsLastCall);
#endif

//  // IF this is the last call to this function
//  if (IsLastCall) {
//    // cleanup the CPGC engine(s)
//    // FOR each socket:
//    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
//      // FOR each controller:
//      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
//        // IF this channel is not enabled for testing THEN skip it
//        if (!IsControllerTestingEnabled(pSystemInfo, pMemCfg, pMemPointTestCfg,
//          Socket, Controller)) continue;
//
//#if ENBL_BIOS_SSA_API_DEBUG_MSGS
//        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
//          "InitCpgc(Socket=%u Controller=%u SetupCleanup=Cleanup)\n",
//          Socket, Controller);
//#endif
//        SsaServicesHandle->SsaMemoryConfig->InitCpgc(SsaServicesHandle, Socket,
//          Controller, Cleanup);
//      } // end for each controller
//    } // end for each socket
//  } // end if this is the last call to this function

  // IF this is the last call to this function
  if (IsLastCall) {
    if (pPointTestData != NULL) {
      if (pPointTestData->pChannelTestData != NULL) {
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
        GetMemPoolSize(SsaServicesHandle);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  Free(pBfr=%p)\n", pPointTestData->pChannelTestData);
#endif
        SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle,
          pPointTestData->pChannelTestData);
#if ENBL_MALLOC_DEBUG_LOG_MSGS
        GetMemPoolSize(SsaServicesHandle);
#endif
      }

#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
      GetMemPoolSize(SsaServicesHandle);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  Free(pBfr=%p)\n", pPointTestData);
#endif
      SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle,
        pPointTestData);
      *ppVoidPointTestData = NULL;
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
      GetMemPoolSize(SsaServicesHandle);
#endif
    }
  } // end if this is the last call to this function
} // end function CleanupCpgcPointTest

#endif // SSA_FLAG

// end file CpgcPointTest.c
