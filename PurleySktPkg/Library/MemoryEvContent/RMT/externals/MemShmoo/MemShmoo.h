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
  Copyright (c) 2013-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  MemShmoo.h

@brief
  This file contains definitions for memory shmoo algorithms.
**/
#ifndef __MEM_SHMOO_H__
#define __MEM_SHMOO_H__

#ifdef SSA_FLAG

#include "ssabios.h"

#include "ChkRetStat.h"
#include "MemPointTest.h"

#pragma pack(push, 1)

/// shmoo direction
typedef enum {
  LoDirection,                            ///< low side
  HiDirection,                            ///< high side
  HiThenLoDirection,                      ///< high then low side
  LoThenHiDirection,                      ///< low then high side
  ShmooDirectionMax,                      ///< SHMOO_DIRECTION enumeration maximum value.
  ShmooDirectionDelim = INT32_MAX         ///< SHMOO_DIRECTION enumeration delimiter value.
} SHMOO_DIRECTION;

/// lane/rank/channel shmoo stop mode
typedef enum {
  AnyShmooStopMode,                       //< stop once any lane/rank/channel fails
  AllShmooStopMode,                       //< stop once all lanes/ranks/channels fail
  DoNoStopShmooStopMode,                  //< do not stop
  ShmooStopModeMax,                       ///< SHMOO_STOP_MODE enumeration maximum value.
  ShmooStopModeDelim = INT32_MAX          ///< SHMOO_STOP_MODE enumeration delimiter value.
} SHMOO_STOP_MODE;

/// memory shmoo test configuration
typedef struct {
  GSM_LT IoLevel;                           //< margin parameter I/O level
  GSM_GT MarginGroup;                       //< margin parameter group
  BOOLEAN IsDdrT;                           //< flag indicating whether testing is for DDR-T devices
  SHMOO_DIRECTION ShmooDirection;           //< shmoo direction
  UINT8 StepSize;                           //< step size
  UINT8 IncrementalStepSize;                //< incremental step size
  UINT8 GuardBandSize;                      //< guard band size

  SHMOO_STOP_MODE LaneShmooStopMode;        //< lane shmoo stop mode
  SHMOO_STOP_MODE RankShmooStopMode;        //< rank shmoo stop mode
  SHMOO_STOP_MODE ChannelShmooStopMode;     //< channel shmoo stop mode
  SHMOO_STOP_MODE ControllerShmooStopMode;  //< controller shmoo stop mode
  SHMOO_STOP_MODE SocketShmooStopMode;      //< socket shmoo stop mode

  BOOLEAN EnableOptimizedShmoo;             //< flag to enable optimized shmooing

  BOOLEAN EnableIncrementalStepping;        //< flag to enable incremental stepping

  BOOLEAN EnableDimmInitAfterErr;           //< flag to enable DIMM initialization after errors are detected
  DIMM_INIT_MODE DimmInitMode;              //< DIMM initialization mode

  BOOLEAN DisableInitMarginParam;          //< flag to disable the calling of the InitMarginParam function

  BOOLEAN StopChannelMarginOnStopCond;     //< Indicate whether to put channels nominal while stop condition satisfied
  UINT8 ChannelValCompletionBitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT]; //< to stop the val of channels once it meets the stop condition 

#if SUPPORT_FOR_EXTENDED_RANGE
  BOOLEAN EnableExtendedRange;             //< flag to enable extended range feature
#endif // SUPPORT_FOR_EXTENDED_RANGE
} MEM_SHMOO_CONFIG;

#pragma pack(pop)

/**
  This function sets default values in the given MemShmoo test configuration.

  @param[out]  pTestCfg           Pointer to MemShmoo test configuration structure.
**/
VOID SetMemShmooTestCfgDefaults(
  OUT MEM_SHMOO_CONFIG *pTestCfg);

/**
  This is the definition for the margin parameter change handler function
  provided by the client.  The function will be called by the MemShmoo code
  after the margin parameter offset is adjusted (and before a point test is run
  at that new offset).

  @param[in, out]  pClientTestData   Pointer to client test data.
  @param[in]       IoLevel           I/O level.
  @param[in]       MarginGroup       Margin group.
  @param[in]       TestingOffset     Margin parameter offset being tested.
**/
typedef VOID (*MARGIN_PARAM_CHG_HANDLER)(
  IN OUT VOID *pClientTestData,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup,
  IN INT16  TestingOffset);

/**
  This is the definition for the pre-DIMM-initialization handler function
  provided by the client.  The function will be called by the MemShmoo code
  immediately before a DIMM initialization is performed.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pClientTestCfg     Pointer to client test configuration structure.
  @param[in, out]  pClientTestData    Pointer to client test data structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
typedef UINT8 (*PRE_DIMM_INIT_HANDLER)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST VOID *pClientTestCfg,
  IN OUT VOID *pClientTestData,
  OUT TEST_STATUS *pTestStat);

/**
  This is the definition for the post-DIMM-initialization handler function
  provided by the client.  The function will be called by the MemShmoo code
  immediately after a DIMM initialization is performed.

  @param[in, out]  SsaServicesHandle      Pointer to SSA services.
  @param[in]       pSystemInfo            Pointer to system information structure.
  @param[in]       pMemCfg                Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg       Pointer to generic memory point test configuration structure.
  @param[in]       pClientTestCfg         Pointer to client test configuration structure.
  @param[in, out]  pClientTestData        Pointer to client test data structure.
  @param[in]       IsLastRankCombination  Flag indicating that this is the last rank combination.
  @param[out]      pTestStat              Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
typedef UINT8 (*POST_DIMM_INIT_HANDLER)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pClientTestCfg,
  IN OUT VOID *pClientTestData,
  IN BOOLEAN IsLastRankCombination,
  OUT TEST_STATUS *pTestStat);

/**
  This is the definition for the pre-RunPointTest handler function provided by
  the client.  The function will be called by the MemShmoo code immediately
  before a PointTest is performed.

  @param[in, out]  SsaServicesHandle      Pointer to SSA services.
  @param[in]       pSystemInfo            Pointer to system information structure.
  @param[in]       pMemCfg                Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg       Pointer to generic memory point test configuration structure.
  @param[in]       pClientTestCfg         Pointer to client test configuration structure.
  @param[in, out]  pClientTestData        Pointer to client test data structure.
  @param[out]      pTestStat              Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
typedef UINT8 (*PRE_RUN_POINT_TEST_HANDLER)(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pClientTestCfg,
  IN OUT VOID *pClientTestData,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to check the memory shmoo configuration for
  correctness/consistency.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pTestCfg           Pointer to memory shmoo test configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 ChkMemShmooTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_SHMOO_CONFIG *pTestCfg,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to log the memory shmoo test configuration.

  @param[in, out]   SsaServicesHandle  Pointer to SSA services.
  @param[in]        pTestCfg           Pointer to memory shmoo test configuration structure.
**/
VOID LogMemShmooTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_SHMOO_CONFIG *pTestCfg);

/**
  This function is used to set up the memory shmoo test.

  @param[in, out]  SsaServicesHandle    Pointer to SSA services.
  @param[in]       pSystemInfo          Pointer to system information structure.
  @param[in]       pMemCfg              Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg         Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg     Pointer to generic memory point test configuration structure.
  @param[in]       GetPointTestDimmRanks  Pointer to memory point test function for getting the DIMM/rank pairs for a channel.
  @param[in]       SetPointTestRankCombinationsIndex  Pointer to specific memory point test function for setting the rank combination index.
  @param[in]       RunPointTest         Pointer to specific memory point test function to run point test.
  @param[in]       MarginParamChgHandler  Pointer to client's margin parameter change handler function.
  @param[in]       PreDimmInitHandler   Pointer to client's pre-DIMM-initialization handler function.
  @param[in]       PostDimmInitHandler  Pointer to client's post-DIMM-initialization handler function.
  @param[in]       PreRunPointTestHandler  Pointer to client's pre-RunPointTest handler function.
  @param[in]       pClientTestCfg       Pointer to client test configuration.
  @param[in, out]  pClientTestData      Pointer to client test data.
  @param[in]       TestStatusLogLevel   Test status logging level.
  @param[out]      ppMemShmooData       Pointer to pointer to memory shmoo test data
  @param[out]      pTestStat            Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 SetupMemShmoo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GET_MEM_POINT_TEST_DIMM_RANKS GetPointTestDimmRanks,
  IN SET_MEM_POINT_TEST_IN_BINARY_SEARCH_STATE SetPointTestInBinarySearchState,
  IN RUN_MEM_POINT_TEST RunPointTest,
  IN MARGIN_PARAM_CHG_HANDLER MarginParamChgHandler,
  IN PRE_DIMM_INIT_HANDLER PreDimmInitHandler,
  IN POST_DIMM_INIT_HANDLER PostDimmInitHandler,
  IN PRE_RUN_POINT_TEST_HANDLER PreRunPointTestHandler,
  IN CONST VOID *pClientTestCfg,
  IN OUT VOID *pClientTestData,
  IN UINT8 TestStatusLogLevel,
  OUT VOID **ppMemShmooData,
  OUT TEST_STATUS *pTestStat);

#if SUPPORT_FOR_EXTENDED_RANGE
/**
  This function is used to get the maximum of the minimum offsets for the given
  channel from the given shmoo test data.

  @param[in]  pSystemInfo        Pointer to system information structure.
  @param[in]  pVoidMemShmooData  Pointer to memory shmoo test data structure.
  @param[in]  Socket             Zero-based socket.
  @param[in]  Controller         Zero-based controller.
  @param[in]  Channel            Zero-based channel.

  @retval  Minimum shmoo parameter offset.
**/
INT16 GetMemShmooMaxOfMinOffset(
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST VOID *pVoidMemShmooData,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel);

/**
  This function is used to get the minimum of the maximum offsets for the given
  channel from the given shmoo test data.

  @param[in]  pSystemInfo        Pointer to system information structure.
  @param[in]  pVoidMemShmooData  Pointer to memory shmoo test data structure.
  @param[in]  Socket             Zero-based socket.
  @param[in]  Controller         Zero-based controller.
  @param[in]  Channel            Zero-based channel.

  @retval  Maximum shmoo parameter offset.
**/
INT16 GetMemShmooMinOfMaxOffset(
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST VOID *pVoidMemShmooData,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel);
#endif // SUPPORT_FOR_EXTENDED_RANGE

/**
  This function is used to get the number of active channels from the given
  shmoo test data.

  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Number of active channels.
**/
UINT8 GetMemShmooActiveChnlCnt(
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the minimum shmoo parameter offset from the
  given shmoo test data.

  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Minimum shmoo parameter offset.
**/
INT16 GetMemShmooMinOffset(
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the maximum shmoo parameter offset from the
  given shmoo test data.

  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Maximum shmoo parameter offset.
**/
INT16 GetMemShmooMaxOffset(
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the shmoo parameter step unit from the
  given shmoo test data.

  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter step unit.
**/
UINT16 GetMemShmooStepUnit(
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the shmoo parameter settling delay from the
  given shmoo test data.

  @param[in]   pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter settling delay.
**/
UINT16 GetMemShmooSettleDelay(
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the minimum shmoo parameter offset from the
  given shmoo test data per memory technology.

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Minimum shmoo parameter offset.
**/
INT16 GetMemShmooMinOffsetByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the maximum shmoo parameter offset from the
  given shmoo test data per memory technology.

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Maximum shmoo parameter offset.
**/
INT16 GetMemShmooMaxOffsetByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the shmoo parameter step unit from the
  given shmoo test data per memory technology.

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter step unit.
**/
UINT16 GetMemShmooStepUnitByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the shmoo parameter settling delay from the
  given shmoo test data per memory technology

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]   pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter settling delay.
**/
UINT16 GetMemShmooSettleDelayByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the flag indicating whether we're in the binary
  search state.

  @param[in]   pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Flag indicating whether we're in the binary search state.
**/
BOOLEAN GetInBinarySearchState(
  IN CONST VOID *pMemShmooData);

/**
  This function is used to get the size of the required MemShmoo dynamic memory.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       MinOffset          Minimum offset value.
  @param[in]       MaxOffset          Maximum offset value.

  @retval  dynamic memory size
**/
UINT32 GetMemShmooDynamicMemSize(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN INT16 MinOffset,
  IN INT16 MaxOffset);

/**
  This function is used to run a find edge test.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pVoidMemShmooData  Pointer to memory shmoo test data.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in]       ShmooDir           Shmoo direction.
  @param[in]       InnerLimitOffset   Inner shmoo parameter offset not to test beyond.
  @param[in, out]  pTestingOffset     Pointer to shmoo parameter offset where we're testing.
  @param[in, out]  pCurrentOffset     Pointer to current shmoo parameter offset.
  @param[out]      pLastPassFound     Pointer to flag indicating whether a last pass offset offset was found.
  @param[out]      pLastPassOffset    Pointer to last pass offset.
  @param[out]      pFirstFailFound    Pointer to flag indicating whether a first fail offset offset was found.
  @param[out]      pFirstFailOffset   Pointer to first fail offset.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 FindEdge(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT VOID *pVoidMemShmooData,
  IN OUT VOID *pPointTestData,
  IN SHMOO_DIRECTION ShmooDir,
  IN INT16 InnerLimitOffset,
  IN OUT INT16 *pTestingOffset,
  IN OUT INT16 *pCurrentOffset,
  OUT BOOLEAN *pLastPassFound,
  OUT INT16 *pLastPassOffset,
  OUT BOOLEAN *pFirstFailFound,
  OUT INT16 *pFirstFailOffset,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to run a margin 1D test.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to specific memory point test configuration structure.
  @param[in, out]  pMemShmooData      Pointer to memory shmoo test data structure.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 RunMargin1D(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT VOID *pMemShmooData,
  IN OUT VOID *pPointTestData,
  OUT TEST_STATUS *pTestStat);

/**
  This function is used to clean up the memory shmoo test.  The given
  pMemShmooData buffer will be freed and the pointer will be set to NULL.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to specific memory point test configuration structure.
  @param[in, out]  ppMemShmooData     Pointer to pointer to memory shmoo test data
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 CleanupMemShmoo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN OUT VOID **ppMemShmooData,
  OUT TEST_STATUS *pTestStat);

#endif  // SSA_FLAG
#endif  // __MEM_SHMOO_H__

// end file MemShmoo.h
