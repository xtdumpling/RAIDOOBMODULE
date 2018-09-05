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
  CpgcPointTest.h

@brief
  This file contains definitions for the CPGC point test.
**/
#ifndef __CPGC_POINT_TEST_H__
#define __CPGC_POINT_TEST_H__

#ifdef SSA_FLAG

#include "ssabios.h"
#include "ChkRetStat.h"
#include "MemPointTest.h"

#pragma pack(push, 1)

// number of user defined DqDB pattern cachelines
#define USER_DEFINED_DQDB_PATTERN_CACHELINE_CNT 2

// number of user defined CADB pattern rows
#define USER_DEFINED_CADB_PATTERN_ROW_CNT 16

// maximum number of subsequences
#define MAX_SUBSEQ_CNT 10

// DDRT subseq wait time
#define DDRT_SUBSEQ_WAIT 0xC8


///
/// Memory traffic mode.
///
typedef enum {
  LoopbackTrafficMode,                    ///< This traffic mode produces a simple burst of N cacheline writes followed by a burst on N cacheline reads where N is the corresponding BurstLength parameter value.
  WritesAndReadsTrafficMode,              ///< This traffic mode produces a repeating series of the following sequence of operations: four bursts of write operations followed by four bursts of read operations.
  WriteOnlyTrafficMode,                   ///< This traffic mode produces a series of bursts of write operations.
  ReadOnlyTrafficMode,                    ///< This traffic mode produces a repeating series of bursts of read operations.
  IdleTrafficMode,                        ///< No traffic will be generated for the given channel.
  Wr2WrAndRd2RdTrafficMode,               ///< This traffic mode produces write-to-write AND read-to-read rank-to-rank turn-around operations.
  Rd2WrTrafficMode,                       ///< This traffic mode produces read-to-write rank-to-rank turn-around operations.
  Wr2RdTrafficMode,                       ///< This traffic mode produces write-to-read rank-to-rank turn-around operations.
  TrafficModeMax,                         ///< TRAFFIC_MODE enumeration maximum value.
  TrafficModeDelim = INT32_MAX            ///< This values ensures that different environments use the same size for enum values.
} TRAFFIC_MODE;

///
/// Memory addressing mode.
///
typedef enum {
  LinearAddrMode,                         ///< Within a burst, the addresses will increment linearly.
  ActiveAddrMode,                         ///< Within a burst, the addresses will increment non-linearly. The idea is to make all of the address signals wiggle.
  AddrModeMax,                            ///< ADDR_MODE enumeration maximum value.
  AddrModeDelim = INT32_MAX               ///< This values ensures that different environments use the same size for enum values.
} ADDR_MODE;

///
/// CADB modes.
///
typedef enum {
  NormalCadbMode,                         ///< Normal CADB mode.
  OnDeselectCadbMode,                     ///< On deselect CADB mode.
  AlwaysOnCadbMode,                       ///< Always on CADB mode.
  CadbModeMax,                            ///< CADB_MODE enumeration maximum value.
  CadbModeDelim = INT32_MAX               ///< This values ensures that different environments use the same size for enum values.
} CADB_MODE;

//////////////////////////////////////////////////////////////////////////
// CPGC Address scheme. Same struct was defined in MRC\include\cpgc.h 
//////////////////////////////////////////////////////////////////////////

typedef enum {
  RankAddrField,
  BankAddrField,
  RowAddrField,
  ColAddrField,
  CpgcAddrFieldMax,                       ///< CPGC_ADDR_FIELD enumeration maximum value.
  CpgcAddrFieldDelim = INT32_MAX          ///< This values ensures that different environments use the same size for enum values.
} CPGC_ADDR_FIELD;

typedef struct {
  UINT16 Start[CpgcAddrFieldMax];                   // (4, uint16)    // Rank, Bank, Row, Col
  UINT32 Wrap[CpgcAddrFieldMax];                    // (4, uint16)    // Rank, Bank, Row, Col
  UINT8  Order[CpgcAddrFieldMax];                   // [4, uint8)     // Rank, Bank, Row, Col
  ADDR_INC_MODE IncMode[CpgcAddrFieldMax];          // (4, unit32)    // Rank, Bank, Row, Col
  UINT32 IncRate[CpgcAddrFieldMax];                 // (4, unit32)    // Rank, Bank, Row, Col
  INT16 IncVal[CpgcAddrFieldMax];                   // (4, unit8)     // Rank, Bank, Row, Col
  BOOLEAN CarryEnable[CpgcAddrFieldMax];            // (4, unit8)     // Rank, Bank, Row, Col
} CPGC_ADDRESS;

//////////////////////////////////////////////////////////////////////////
/// CPGC sequencer
//////////////////////////////////////////////////////////////////////////
typedef struct {
  UINT8 SubSequenceStart;
  UINT8 SubSequenceEnd;
  UINT8 SubSequenceType[MAX_SUBSEQ_CNT];
  BOOLEAN ResetBaseAddr[MAX_SUBSEQ_CNT];
} CPGC_SEQUENCER;

/// CPGC point test test configuration
typedef struct {
  CPGC_STOP_MODE CpgcStopMode;
  TRAFFIC_MODE TrafficModes[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  ADDR_MODE AddrModes[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT16 BurstLengths[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT32 LoopCounts[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT32 BinarySearchLoopCounts[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT16 StartDelays[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT16 InterSubseqWaits[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  BOOLEAN EnableUserDefinedDqdbPatterns;
  UINT8 UserDefinedDqdbPatternLengths[MAX_CHANNEL_CNT];
  UINT8 UserDefinedDqbdPatternRotationCnts[MAX_CHANNEL_CNT];
  UINT8 UserDefinedDqdbPatterns[MAX_CHANNEL_CNT][USER_DEFINED_DQDB_PATTERN_CACHELINE_CNT][CHUNKS_PER_CACHELINE][DQDB_PATTERN_WIDTH / 8];
  DQDB_INC_RATE_MODE DqdbIncRateModes[MAX_CHANNEL_CNT];
  UINT8 DqdbIncRates[MAX_CHANNEL_CNT];
  DQDB_UNISEQ_MODE DqdbUniseqModes[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_DQDB_UNISEQ_CNT];
  UINT32 DqdbUniseqSeeds[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_DQDB_UNISEQ_CNT];
  UINT16 DqdbLmnValues[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_DQDB_LMN_UNISEQ_CNT][MAX_DQDB_LMN_VALUES_CNT];
  CADB_MODE CadbModes[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  BOOLEAN EnableUserDefinedCadbPatterns;
  UINT8 UserDefinedCadbPatterns[MAX_CHANNEL_CNT][USER_DEFINED_CADB_PATTERN_ROW_CNT][CADB_PATTERN_WIDTH / 8];
  CADB_UNISEQ_MODE CadbUniseqModes[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_CADB_UNISEQ_CNT];
  UINT32 CadbUniseqSeeds[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_CADB_UNISEQ_CNT];
  UINT16 CadbLmnValues[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_CADB_LMN_UNISEQ_CNT][MAX_CADB_LMN_VALUES_CNT];
  BOOLEAN EnableErid;
  UINT32 EridPatternSeeds[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_ERID_SIGNAL_CNT];
  BOOLEAN EnableTimeout;
} CPGC_POINT_TEST_CONFIG;

#if MAX_LOGICAL_RANK_CNT == 1
  #define MAX_RANK_ITER 1
#else
  #define MAX_RANK_ITER (MAX_LOGICAL_RANK_CNT * (MAX_LOGICAL_RANK_CNT - 1))
#endif

#pragma pack(pop)

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
  OUT TEST_STATUS *pTestStat);

/**
@brief
  This function logs the CPGC information.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pCpgcInfo          Pointer to CPGC information structure.
**/
VOID LogCpgcInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN MRC_SERVER_CPGC_INFO *pCpgcInfo);

/**
@brief
  This function sets default values in the given CPGC point test configuration.

  @param[out]  pTestCfg           Pointer to CPGC point test configuration structure.
**/
VOID SetCpgcPointTestCfgDefaults(
  OUT CPGC_POINT_TEST_CONFIG *pTestCfg);

/**
@brief
  This function is used to check the CPGC point test configuration for
  correctness/consistency.

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
  OUT TEST_STATUS *pTestStat);

/**
@brief
  This function is used to log the CPGC point test configuration.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pCpgcPointTestCfg  Pointer to test configuration structure.
**/
VOID LogCpgcPointTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST CPGC_POINT_TEST_CONFIG *pCpgcPointTestCfg);

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
  @param[in]       IsDdrT              Flag indicating whether DDR-T devices are being tested.
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
  IN OUT VOID **ppPointTestData,
  OUT TEST_STATUS *pTestStat);

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
  IN UINT8 TestedChnlCnt);

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
  OUT TEST_STATUS *pTestStat);

/**
@brief
  This function is used to set the rank combination index to the given value.
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
  @param[out]      pTestStat             Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 SetCpgcPointTestRankCombinationIndex(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN VOID *pPointTestData,
  IN UINT8 RankCombinationIndex,
  IN BOOLEAN UpdateHwSetting,
  OUT TEST_STATUS *pTestStat);

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
  @param[in]       pPointTestCfg        Pointer to specific memory point test configuration structure.
  @param[in]       pPointTestData       Pointer to specific memory point test data structure.
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
  IN CONST VOID *pPointTestCfg,
  IN VOID *pPointTestData,
  IN BOOLEAN InBinarySearchState,
  OUT TEST_STATUS *pTestStat);

/**
@brief
  This function is used to run a CPGC point test.

  This function conforms to the RUN_MEM_POINT_TEST typedef in MemPointTest.h.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in]       pPointTestData     Pointer to specific memory point test data structure.
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
  IN CONST VOID *pPointTestCfg,
  IN VOID *pPointTestData,
  OUT LANE_FAIL_STATS *pLaneFailStats,
  OUT TEST_STATUS *pTestStat);

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
  IN OUT VOID **ppPointTestData);

/**
@brief
  This function rotates the given DqDB chunk value to the left the given number
  of bits.  This function returns the rotated value.

  @param[in]  Pattern   Pattern to rotate.
  @param[in]  BitCount  Number of bit positions to rotate.

  @retval  Rotated pattern.
**/
UINT64 RotateDqdbChunk(
  IN UINT64 Pattern,
  IN UINT8 BitCount);

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
  OUT UINT64* pBfr);

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
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

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
  OUT TEST_STATUS *pTestStat);

#endif // SSA_FLAG
#endif // __CPGC_POINT_TEST_H__

// end file CpgcPointTest.h
