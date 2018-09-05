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
  MemPointTest.h

@brief
  This file contains common definitions for the memory point test.
**/
#ifndef __MEM_POINT_TEST_H__
#define __MEM_POINT_TEST_H__

#ifdef SSA_FLAG

#include "ssabios.h"
#include "ChkRetStat.h"
#include "Platform.h"
#include "MemCfg.h"

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P)          (P)
#endif

#pragma pack(push, 1)

///
/// Rank combinations
///
typedef enum {
  Singles,                                ///< Single rank selection.
  Turnarounds,                            ///< Turnaround ranks selection.
  RankTestModeMax,                        ///< RANK_TEST_MODE enumeration maximum value.
  RankTestModeDelim = INT32_MAX           ///< RANK_TEST_MODE enumeration delimiter value.
} RANK_TEST_MODE;

///
/// rank-to-rank turnarounds test modes
///
typedef enum {
  AllRankTurnarounds,                     ///< Test all rank-to-rank turnarounds.
  OnlyInterDimmTurnarounds,               ///< Only test inter-DIMM rank-to-rank turnarounds.
  OnlyIntraDimmTurnarounds,               ///< Only test intra-DIMM rank-to-rank turnarounds.
  TurnaroundsTestModeMax,                 ///< TURNAROUNDS_TEST_MODE enumeration maximum value.
  TurnaroundsTestModeDelim = INT32_MAX    ///< TURNAROUNDS_TEST_MODE enumeration delimiter value.
} TURNAROUNDS_TEST_MODE;

///
/// rank-to-rank turnarounds pair modes
///
typedef enum {
  OrderDependent,                         ///< Need to test both A-B and B-A.
  OrderIndependent,                       ///< Only need to test A-B.
  TurnaroundsPairModeMax,                 ///< TURNAROUNDS_PAIR_MODE enumeration maximum value.
  TurnaroundsPairModeDelim = INT32_MAX    ///< TURNAROUNDS_PAIR_MODE enumeration delimiter value.
} TURNAROUNDS_PAIR_MODE;

///
/// DIMM initialization type
///
typedef enum {
  Jedec,                                  ///< JEDEC initialization is performed
  IoReset,                                ///< IO Reset initialization is performed
  DimmInitModeMax,                        ///< DIMM_INIT_MODE enumeration maximum value.
  DimmInitModeDelim = INT32_MAX           ///< DIMM_INIT_MODE enumeration delimiter value.
} DIMM_INIT_MODE;

///
/// lane failure status
///
typedef struct {
  // Using bit fields reduces the structure size from 8 bytes to 1 bytes.
  UINT8 AnyLaneFailed  : 1;
  UINT8 AllLanesFailed : 1;
  UINT8 Reserved       : 6;
} LANE_FAIL_STAT;
typedef LANE_FAIL_STAT LANE_FAIL_STATS[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];

typedef UINT8 RANK_COMBINATION_CNTS[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];

// memory point test test configuration
typedef struct {
  UINT32 SocketBitmask;
  UINT8 ControllerBitmasks[MAX_SOCKET_CNT];
  UINT8 ChannelBitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT];
  UINT8 ChannelValBitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT];
  // Note that the following field uses UINT8 instead of RANK_TEST_MODE to conserve space.
  UINT8 RankTestModes[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT8 DimmABitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT8 RankABitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_DIMM_CNT];
  UINT8 DimmBBitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  UINT8 RankBBitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_DIMM_CNT];
  UINT8 LaneValBitmasks[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_BUS_WIDTH / 8];
  UINT8 ChunkValBitmasks[MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT];
  TURNAROUNDS_TEST_MODE TurnaroundsTestMode;
  TURNAROUNDS_PAIR_MODE TurnaroundsPairMode;
} MEM_POINT_TEST_CONFIG;

// special values to use to test all populated entities
#define ALL_POPULATED_SOCKETS     0xFFFFFFFF
#define ALL_POPULATED_CONTROLLERS 0xFF
#define ALL_POPULATED_CHANNELS    0xFF
#define ALL_POPULATED_DIMMS       0xFF
#define ALL_POPULATED_RANKS       0xFF

// bitmask for all chunks in cacheline
#define ALL_CHUNKS_BITMASK        0xFF

// memory training error counter bit definitions.
typedef union {
  struct {
    UINT32 Count    : 31; ///< Error count, range is 0 to 2^31 - 1.
    UINT32 Overflow : 1;  ///< Error count overflow, 0 = no overflow, 1 = overflow.
  } Bits;
  UINT32 Data;
} ERROR_COUNT_32BITS;

#pragma pack(pop)

/**
  This is the definition for the result handler function provided by client.

  @param[in, out]  SsaServicesHandle     Pointer to SSA services.
  @param[in]       pSystemInfo           Pointer to system information structure.
  @param[in]       pClientTestData       Pointer to client test data.
  @param[in]       Socket                Socket tested.
  @param[in]       Controller            Controller tested.
  @param[in]       Channel               Channel tested.
  @param[in]       DimmRankA             First DIMM+Rank tested.
  @param[in]       DimmRankB             Second DIMM+Rank tested.
  @param[in]       RankCombinationIndex  Rank combination index tested.
  @param[in]       pLaneErrorStatus      Pointer to array of lane error status masks.  Number of array elements is SystemInfo.BusWidth/8
  @param[in]       ErrorCount            Channel error count.
  @param[out]      pTestStat             Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
typedef UINT8 (*MEM_POINT_TEST_RESULT_HANDLER)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN VOID *pClientTestData,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN DIMM_RANK_PAIR DimmRankA,
  IN DIMM_RANK_PAIR DimmRankB,
  IN UINT8 RankCombinationIndex,
  IN CONST UINT8 *pLaneErrorStatus,
  IN ERROR_COUNT_32BITS ErrorCount,
  OUT TEST_STATUS *pTestStat);

/**
@brief
  This function is used to set up the CPGC point test.

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
typedef UINT8 (*SETUP_MEM_POINT_TEST)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST VOID *pCpgcInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pCpgcPointTestCfg,
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
  This function is used to clean up the memory point test.

  If the IsLastCall input parameter is set to TRUE then this function will free
  the memory for the memory point test's private test status data structure and
  set the pointer to NULL, otherwise it will not.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IsLastCall         Flag indicating whether the call is the last.
  @param[in, out]  ppPointTestData    Pointer to pointer to specific memory point test data structure.
**/
typedef VOID (*CLEANUP_MEM_POINT_TEST)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN BOOLEAN IsLastCall,
  IN OUT VOID **ppPointTestData);

/**
  This function is used to get the DIMM/rank pairs for a given channel.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
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
typedef UINT8 (*GET_MEM_POINT_TEST_DIMM_RANKS)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN VOID *pPointTestData,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN OUT UINT8 *pPairCount,
  OUT DIMM_RANK_PAIR *pDimmRankPairs,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to set the rank combinations index to the given value.
  This function configures that hardware to run the point test for the given
  rank index.

  @param[in, out]  SsaServicesHandle     Pointer to SSA services.
  @param[in]       pSystemInfo           Pointer to system information structure.
  @param[in]       pMemCfg               Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg      Pointer to generic memory point test configuration structure.
  @param[in]       pVoidPointTestCfg     Pointer to specific memory point test configuration structure.
  @param[in]       pVoidPointTestData    Pointer to specific memory point test data structure.
  @param[in]       RankCombinationIndex  Rank combination index.
  @param[in]       UpdateHwSetting       Flag to indicate if need to apply the setting to register or not
  @param[out]      pTestStat             Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
typedef UINT8 (*SET_MEM_POINT_TEST_RANK_COMBINATION_INDEX)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pVoidPointTestCfg,
  IN VOID *pVoidPointTestData,
  IN UINT8 RankCombinationIndex,
  IN BOOLEAN UpdateHwSetting,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to set the flag indicating whether we're in the binary
  search state.  It is intended that the point test runs for a shorter period
  of time in the binary search state.

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
typedef UINT8 (*SET_MEM_POINT_TEST_IN_BINARY_SEARCH_STATE)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN VOID *pPointTestData,
  IN BOOLEAN InBinarySearchState,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to run a CPGC point test.

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
typedef UINT8 (*RUN_MEM_POINT_TEST)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN VOID *pPointTestData,
  OUT LANE_FAIL_STATS* pLaneFailStats,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to set default values in the given memory point test
  configuration.

  @param[in]   pSystemInfo        Pointer to system information structure.
  @param[in]   pMemPointTestCfg   Pointer to generic memory point test configuration structure.
**/
VOID SetMemPointTestCfgDefaults(
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  OUT MEM_POINT_TEST_CONFIG *pMemPointTestCfg);

#if SUPPORT_FOR_RDIMM || SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT
/**
  This function is used to initialize the given BacksideCmdBitmask value.

  The given pointer is indexed as pBacksideCmdBitmasks[Socket] and the bits
  within the bitmasks are assigned as 1 << ((Controller * MAX_CHANNEL_CNT *
  MAX_DIMM_CNT) + (Channel * MAX_DIMM_CNT) + Dimm).

  @param[in, out]  SsaServicesHandle     Pointer to SSA services.
  @param[in]       pSystemInfo           Pointer to system information structure.
  @param[in]       pMemCfg               Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg      Pointer to generic memory point test configuration structure.
  @param[in]       pMarginGroups         Pointer to array of margin group values.
  @param[in]       MarginGroupCount      Number of elements in pMarginGroups array.
  @param[out]      pBacksideCmdBitmasks  Pointer to array of bitmasks of whether DIMMs support backside command margining.
**/
VOID InitBacksideCmdBitmask(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST GSM_GT *pMarginGroups,
  IN UINT8 MarginGroupCount,
  OUT UINT16 *pBacksideCmdBitmasks);

/**
  This function remove DIMMs that do not support backside command margining
  from the given memory point test configuration.

  @param[in, out]  SsaServicesHandle     Pointer to SSA services.
  @param[in]       pSystemInfo           Pointer to system information structure.
  @param[in]       pMemCfg               Pointer to memory configuration structure.
  @param[in]       pBacksideCmdBitmasks  Pointer to array of bitmasks of whether DIMMs support backside command margining.
  @param[in, out]  pMemPointTestCfg      Pointer to generic memory point test configuration structure.
**/
VOID RemoveNonBacksideCmdDimms(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN UINT16 *pBacksideCmdBitmasks,
  IN OUT MEM_POINT_TEST_CONFIG *pMemPointTestCfg);

/**
  This function determines if any bits are set in the given backside command
  bitmasks.

  @param[in]  pBacksideCmdBitmasks  Pointer to array of bitmasks of whether DIMMs support backside command margining.

  @retval  0  no bits are set
  @retval  1  at least one bit is set
**/
BOOLEAN IsAnyBacksideCmdBitSet(
  IN UINT16 *pBacksideCmdBitmasks);
#endif // SUPPORT_FOR_RDIMM || SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT


#if SUPPORT_FOR_DDRT
/**
  This function remove DIMMs that are not DDRT (not able to do ERID test)
  from the given memory point test configuration.

  @param[in, out]  SsaServicesHandle     Pointer to SSA services.
  @param[in]       pSystemInfo           Pointer to system information structure.
  @param[in]       pMemCfg               Pointer to memory configuration structure.
  @param[in, out]  pMemPointTestCfg      Pointer to generic memory point test configuration structure.
**/
VOID RemoveNonDdrtDimms(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN OUT MEM_POINT_TEST_CONFIG *pMemPointTestCfg);
#endif // SUPPORT_FOR_DDRT
/**
  This function is used to check the point test configuration for correctness/consistency.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
*/
UINT8 ChkMemPointTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to log the memory point test configuration.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
**/
VOID LogMemPointTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg);

/**
  Function used to determine whether the given socket is enabled for testing.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsSocketTestingEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket);

/**
  Function used to determine whether the given controller is enabled for testing.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsControllerTestingEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller);

/**
  Function used to determine whether the given channel is enabled for testing.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsChannelTestingEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel);

/**
  Function used to determine whether the given DIMM is enabled for testing as DIMM A.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in]       Dimm               Zero based DIMM number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsDimmATestingEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 Dimm);

#if SUPPORT_FOR_TURNAROUNDS
/**
  Function used to determine whether the given DIMM is enabled for testing as DIMM B.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in]       Dimm               Zero based DIMM number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsDimmBTestingEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 Dimm);
#endif // SUPPORT_FOR_TURNAROUNDS

/**
  Function used to determine whether the given rank is enabled for testing as rank A.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in]       Dimm               Zero based DIMM number.
  @param[in]       Rank               Zero based physical rank number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsRankATestingEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 Dimm,
  IN UINT8 Rank);

#if SUPPORT_FOR_TURNAROUNDS
/**
  Function used to determine whether the given logical rank B is enabled for testing.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in]       Dimm               Zero based DIMM number.
  @param[in]       Rank               Zero based physical rank number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsRankBTestingEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 Dimm,
  IN UINT8 Rank);
#endif // SUPPORT_FOR_TURNAROUNDS

/**
  Function used to determine whether the given channel has any lanes enabled
  for validation.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsChannelValEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel);

/**
  Function used to determine whether the given controller has any channels with
  any lanes enabled for validation.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsControllerValEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller);

/**
  Function used to determine whether the given socket has any channels with
  any lanes enabled for validation.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.

  @retval  TRUE/FALSE
**/
BOOLEAN IsSocketValEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket);

#if SUPPORT_FOR_TURNAROUNDS
/**
  Function used to determine whether the given channel can perform
  rank-to-rank turnarounds, e.g., channels with a single rank cannot

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in]       IsDdrT             Flag indicating whether DDR-T devices are being tested.

  @retval  TRUE/FALSE
**/
BOOLEAN CanChannelDoTurnarounds(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN BOOLEAN IsDdrT);
#endif // SUPPORT_FOR_TURNAROUNDS

/**
  This function is used to get the number of rank combinations.

  @param[in, out]  SsaServicesHandle       Pointer to SSA services.
  @param[in]       pSystemInfo             Pointer to system information structure.
  @param[in]       pMemCfg                 Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg        Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel                 I/O level.
  @param[in]       IsDdrT                  Flag indicating whether DDR-T devices are being tested.
  @param[out]      pRankCombinationCnts    Pointer to per-channel array of rank combination counts. (Value may be NULL.)
  @param[out]      pMaxRankCombinationCnt  Pointer to maximum rank combination count. (Value may be NULL.)
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 GetRankCombinationsCnts(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN BOOLEAN IsDdrT,
  OUT RANK_COMBINATION_CNTS *pRankCombinationCnts,
  OUT UINT8 *pMaxRankCombinationCnt,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to get the dimm and rank pairs based on the rank
  iteration/combination index for a specified channel.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in]       RankCombinationIndex  Rank combination index variable. 
  @param[out]      pDimmRankA         Pointer to first zero based DIMM+rank pair numbers.
  @param[out]      pDimmRankB         Pointer to second zero based DIMM+rank pair numbers.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 GetDimmRankPerCombinationIndex(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 RankCombinationIndex,
  OUT DIMM_RANK_PAIR *pDimmRankA,
  OUT DIMM_RANK_PAIR *pDimmRankB);

/**
  This function is used to get the any-lane-failed status from the given lane
  failure status array.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pLaneFailStats     Pointer to lane failure status array.  Indexed as [Socket][Controller][Channel]

  @retval  FALSE  No lanes failed.
  @retval  TRUE   At least one lane failed.
**/
BOOLEAN GetAnyLaneFailed(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST LANE_FAIL_STATS *pLaneFailStats);

/**
  This helper function return the index of next tested channel based on
  the memory test config. The index is used to compress the per channel
  test data such as the current ranks and chunk mask.

  Note that calling this function with Socket=(MAX_SOCKET_CNT - 1),
  Controller=(MAX_CONTROLLER_CNT - 1), and Channel=(MAX_CHANNEL_CNT - 1) will
  return (TestedChannelCount - 1) where TestedChannelCount is the total number
  of tested channels.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.

  @retval  index
**/
UINT8 GetTestChnlIndex(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  UINT8 Socket,
  UINT8 Controller,
  UINT8 Channel);

#if SUPPORT_FOR_DDRT
/**
  This function determines whether the given memory point test configuration contains any
  DDR-T devices enabled for validation.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to memory point test configuration structure.

  @retval  TRUE   The memory configuration has at least one DDR-T device.
  @retval  FALSE  The memory configuration has no DDR-T devices.
**/
BOOLEAN HasDdrtDevicesValEnabled(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg);
#endif // SUPPORT_FOR_DDRT

#endif  // SSA_FLAG
#endif  // __MEM_POINT_TEST_H__

// end file MemPointTest.h
