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
  MemShmoo.c

@brief
  This file contains code for the memory shmoo algorithms.
**/
#ifdef SSA_FLAG

#define BSSA_LOGGING_LEVEL BSSA_LOGGING_INTERNAL_DEBUG_LEVEL

#include "ssabios.h"
#include "bssalogging.h"
#include "ChkRetStat.h"
#include "GetActualMallocSize.h"
#include "GetMemPoolSize.h"
#include "LogByteArrayAsHex.h"
#include "LogSpecificity.h"
#include "Platform.h"
#include "MarginParam.h"
#include "MemPointTest.h"
#include "MemShmoo.h"

#ifndef ABS
#define ABS(a) (((a) < 0) ? (-(a)) : (a))
#endif

// flag to enable BIOS-SSA debug messages
#define ENBL_BIOS_SSA_API_DEBUG_MSGS (0)

// flag to enable *ShmooStopMode debug messages
#define ENBL_FUNCTION_TRACE_DEBUG_MSGS (0)

// flag to enable verbose log messages
#define ENBL_VERBOSE_LOG_MSGS (0)

// flag to enable verbose error messages
#define ENBL_VERBOSE_ERROR_MSGS (0)

// flag to enable memory pool debug log messages
#define ENBL_MEM_POOL_DEBUG_LOG_MSGS (0)

// flag to enable debug log messages
#define ENBL_DEBUG_LOG_MSGS (0)

// flag to enable fixup debug messages
#define ENBL_FIXUP_DEBUG_LOG_MSGS (0)

// flag to enable extended range debug messages
#define ENBL_EXTENDED_RANGE_DEBUG_LOG_MSGS (0)

// flag to enable DDR-T debug messages
#define ENBL_DDRT_DEBUG_LOG_MSGS (0)

// flag to enable debug of offset status indexing
#define ENBL_DEBUG_OFFSET_STAT_IDX (0)

#pragma pack(push, 1)

// shmoo state
typedef enum {
  DoneShmooState = 0, // this entry must be 0 to make is easy to initialize the test status buffer
  BinarySearchShmooState,
  StopConditionShmooState,
  LastPassShmooState,
  GuardBandShmooState
} SHMOO_STATE;

// offset test status
typedef enum {
  NotTested = 0, // this entry must be 0 to make is easy to initialize the test status buffer
  NoErrsDetected,
  ErrsDetected
} OFFSET_TEST_STAT;

// shmoo data
typedef struct {
  // test status logging level
  UINT8 TestStatusLogLevel;

  // margin parameter specificity
  MARGIN_PARAM_SPECIFICITY Specificity;

  // margin parameter settling delay
  UINT16 SettleDelay;

  // margin parameter settling delays for memory tech (DDR4 and DDRT)
  UINT16 SettleDelayByMemoryTech[2];

  // limits of margin parameter offset (aligned to step size)
  INT16 MinOffset;
  INT16 MaxOffset;

  // limits of margin parameter offset (aligned to step size) for memory tech (DDR4 and DDRT)
  INT16 MinOffsetByMemoryTech[2];   // 0 is DDR4, 1 is DDRT
  INT16 MaxOffsetByMemoryTech[2];

#if SUPPORT_FOR_EXTENDED_RANGE
  // current/minimum/maximum offset values by lane
  // values are converted from INT16 to UINT8 to save space
  UINT8 MinOffsets[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_BUS_WIDTH];
  UINT8 MaxOffsets[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_BUS_WIDTH];
#endif // SUPPORT_FOR_EXTENDED_RANGE

  // margin parameter step unit
  UINT16 StepUnit;

  // margin parameter step units for both ddr4 and ddrt types
  UINT16 StepUnitByMemoryTech[2];

  // number of channels that are not idle (all sockets/controllers)
  UINT8 ActiveChnlCnt;

  // number of channels being validated (all sockets/controllers)
  UINT8 ValChnlCnt;

  // shmoo state
  SHMOO_STATE ShmooState;

  // dynamically allocated buffer of per-offset test status
  // indexed as [CurrentOffset - MinOffset] / StepSize
  UINT32 OffsetTestStatBfrSize;
  UINT8* pOffsetTestStat; // uses values from OFFSET_TEST_STAT enum

  // dynamically allocated array of lane failure status
  // indexed as [RankIterIdx][Socket][Controller][Channel]
  UINT32 LaneFailStatsBfrSize;
  LANE_FAIL_STATS *pLaneFailStats;

  // pointer to memory point test function for getting the DIMM/rank pairs for a channel
  GET_MEM_POINT_TEST_DIMM_RANKS GetPointTestDimmRanks;

  // pointer to point test function for setting in-binary-search-state
  SET_MEM_POINT_TEST_IN_BINARY_SEARCH_STATE SetPointTestInBinarySearchState;

  // pointer to memory point test function for running point test
  RUN_MEM_POINT_TEST RunPointTest;

  // pointer to client's margin parameter change handler function
  MARGIN_PARAM_CHG_HANDLER MarginParamChgHandler;

  // pointer to client's pre-DIMM-initialization handler function
  PRE_DIMM_INIT_HANDLER PreDimmInitHandler;

  // pointer to client's post-DIMM-initialization handler function
  POST_DIMM_INIT_HANDLER PostDimmInitHandler;

  // pointer to client's pre-RunPointTest handler function
  PRE_RUN_POINT_TEST_HANDLER PreRunPointTestHandler;

  // pointers to client configuration and data - used during callbacks to client
  CONST VOID *pClientTestCfg;
  VOID *pClientTestData;
} MEM_SHMOO_TEST_DATA;

#pragma pack(pop)

#if ENBL_DEBUG_OFFSET_STAT_IDX
/**
  This function is used to range check the given index into the dynamically
  allocated offset test status array.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pMemShmooData      Pointer to memory shmoo test data structure.
  @param[in]       LineNumber         Line number.
  @param[in]       Idx                Index.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 ChkOffsetTestStatIdx(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN UINT32 LineNumber,
  IN UINT32 Idx,
  OUT TEST_STATUS *pTestStat)
{
  if (Idx > (pMemShmooData->OffsetTestStatBfrSize - 1)) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: Index value (%u) is out of range (0 <= val < %u).\n",
      Idx, pMemShmooData->OffsetTestStatBfrSize);
    ProcError(SsaServicesHandle, Failure, __FILE__, LineNumber, pTestStat);
    return 1;
  }

  return 0;
} // end function ChkOffsetTestStatIdx
#endif // ENBL_DEBUG_OFFSET_STAT_IDX

/**
  This function sets default values in the given MemShmoo test configuration.

  @param[out]  pTestCfg           Pointer to MemShmoo test configuration structure.
**/
VOID SetMemShmooTestCfgDefaults(
  OUT MEM_SHMOO_CONFIG *pTestCfg)
{
  bssa_memset(pTestCfg, 0, sizeof(*pTestCfg));
  pTestCfg->IoLevel                   = DdrLevel;
  pTestCfg->MarginGroup               = RecEnDelay;
  pTestCfg->StepSize                  = 1;
  pTestCfg->IncrementalStepSize       = 16;
  pTestCfg->GuardBandSize             = 0;
  pTestCfg->EnableOptimizedShmoo      = TRUE;
  pTestCfg->LaneShmooStopMode         = AllShmooStopMode;
  pTestCfg->RankShmooStopMode         = AllShmooStopMode;
  pTestCfg->ChannelShmooStopMode      = AllShmooStopMode;
  pTestCfg->ControllerShmooStopMode   = AllShmooStopMode;
  pTestCfg->SocketShmooStopMode       = AllShmooStopMode;
  pTestCfg->EnableIncrementalStepping = FALSE;
  pTestCfg->EnableDimmInitAfterErr    = FALSE;
  pTestCfg->DimmInitMode              = Jedec;
#if SUPPORT_FOR_EXTENDED_RANGE
  pTestCfg->EnableExtendedRange       = TRUE;
#endif // SUPPORT_FOR_EXTENDED_RANGE
} // end function SetMemShmooTestCfgDefaults

/**
  This function is used to check the memory shmoo configuration for correctness/
  consistency.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pTestCfg           Pointer to memory shmoo configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 ChkMemShmooTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_SHMOO_CONFIG *pTestCfg,
  OUT TEST_STATUS *pTestStat)
{
  if (pTestCfg->IoLevel >= GsmLtMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: IoLevel value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->IoLevel, GsmLtMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->MarginGroup >= GsmGtMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: MarginGroup value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->MarginGroup, GsmGtMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->ShmooDirection >= ShmooDirectionMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: ShmooDirection value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->ShmooDirection, ShmooDirectionMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->StepSize == 0) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: StepSize value (%u) is out of range (0 < val).\n",
      pTestCfg->StepSize);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->LaneShmooStopMode >= ShmooStopModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: LaneShmooStopMode value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->LaneShmooStopMode, ShmooStopModeMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->RankShmooStopMode >= ShmooStopModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: RankShmooStopMode value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->RankShmooStopMode, ShmooStopModeMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->ChannelShmooStopMode >= ShmooStopModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: ChannelShmooStopMode value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->ChannelShmooStopMode, ShmooStopModeMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->ControllerShmooStopMode >= ShmooStopModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: ControllerShmooStopMode value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->ControllerShmooStopMode, ShmooStopModeMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->SocketShmooStopMode >= ShmooStopModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: SocketShmooStopMode value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->SocketShmooStopMode, ShmooStopModeMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pTestCfg->DimmInitMode >= DimmInitModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: DimmInitMode value (%u) is out of range (0 <= val < %u).\n",
      pTestCfg->DimmInitMode, DimmInitModeMax);
#endif
    ProcError(SsaServicesHandle, UnsupportedValue, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  return 0;
} // end function ChkMemShmooTestCfg

/**
  This function is used to log the memory shmoo test configuration.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pTestCfg           Pointer to memory shmoo test configuration structure.
**/
VOID LogMemShmooTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_SHMOO_CONFIG *pTestCfg)
{
#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "LogMemShmooTestCfg()\n");
#endif

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n"
    "Memory Shmoo Test Configuration:\n");
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  IoLevel=%u (%s)\n", pTestCfg->IoLevel, GetIoLevelStr(pTestCfg->IoLevel));
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MarginGroup=%u (%s)\n", pTestCfg->MarginGroup, GetMarginGroupStr(pTestCfg->MarginGroup));
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  ShmooDirection=%u\n", pTestCfg->ShmooDirection);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  StepSize=%u\n", pTestCfg->StepSize);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  GuardBandSize=%u\n", pTestCfg->GuardBandSize);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  LaneShmooStopMode=%u\n", pTestCfg->LaneShmooStopMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  RankShmooStopMode=%u\n", pTestCfg->RankShmooStopMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  ChannelShmooStopMode=%u\n", pTestCfg->ChannelShmooStopMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  ControllerShmooStopMode=%u\n", pTestCfg->ControllerShmooStopMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  SocketShmooStopMode=%u\n", pTestCfg->SocketShmooStopMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  EnableOptimizedShmoo=%u\n", pTestCfg->EnableOptimizedShmoo);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  EnableIncrementalStepping=%u\n", pTestCfg->EnableIncrementalStepping);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  EnableDimmInitAfterErr=%u\n", pTestCfg->EnableDimmInitAfterErr);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  DimmInitMode=%u\n", pTestCfg->DimmInitMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  DisableInitMarginParam=%u\n", pTestCfg->DisableInitMarginParam);
#if SUPPORT_FOR_EXTENDED_RANGE
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  EnableExtendedRange=%u\n", pTestCfg->EnableExtendedRange);
#endif // SUPPORT_FOR_EXTENDED_RANGE
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n");
} // end function LogMemShmooTestCfg

/**
  This function is used to get the information about the channels.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[out]      pVoidMemShmooData  Pointer to memory shmoo test data structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static VOID GetChnlInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  OUT VOID *pVoidMemShmooData,
  OUT TEST_STATUS *pTestStat)
{
  MEM_SHMOO_TEST_DATA *pMemShmooData = (MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  UINT8 Socket, Controller, Channel;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.GetChnlInfo()\n");
#endif

  pMemShmooData->ActiveChnlCnt = 0; // initialize accumulator
  pMemShmooData->ValChnlCnt = 0; // initialize accumulator

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

        (pMemShmooData->ActiveChnlCnt)++;

        // IF this channel has no lanes enabled for validation THEN skip it
        if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        (pMemShmooData->ValChnlCnt)++;
      } // end for each channel
    } // end for each controller
  } // end for each socket

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  ActiveChnlCnt=%u ValChnlCnt=%u\n",
    pMemShmooData->ActiveChnlCnt, pMemShmooData->ValChnlCnt);
#endif
} // end function GetChnlInfo

/**
  This function is used to get information about the margin parameter.

  Note that the margin parameter is only applied to logical ranks that are
  enabled for validation.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[out]      pVoidMemShmooData  Pointer to memory shmoo test data structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 GetMarginParamInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  OUT VOID *pVoidMemShmooData,
  OUT TEST_STATUS *pTestStat)
{
  MEM_SHMOO_TEST_DATA *pMemShmooData = (MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  SSA_STATUS RetStat;
  UINT8 Socket, Controller, Channel, Dimm, Rank;
  UINT8 RankCount;
  UINT8 LaneBitmasks[MAX_BUS_WIDTH / 8];
  INT16 TmpMinOffset, TmpMaxOffset;
  UINT16 TmpSettleDelay;
  UINT8 i; // index of memoryTech  0:ddr4; 1 ddrt
#if SUPPORT_FOR_EXTENDED_RANGE
  UINT8 TmpOffset;
  UINT8 StrobeWidth = 8;
  UINT8 ByteGroup, Strobe, Lane;
  UINT8 ByteGroupBitmask;
#endif // SUPPORT_FOR_EXTENDED_RANGE

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS || ENBL_FIXUP_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.GetMarginParamInfo()\n");
#endif

#if SUPPORT_FOR_SERIALIZATION
  // initialize to minimum values
  pMemShmooData->MinOffset = 0;
  pMemShmooData->MaxOffset = 0;
  bssa_memset(pMemShmooData->MinOffsetByMemoryTech, 0x0, sizeof(pMemShmooData->MinOffsetByMemoryTech));
  bssa_memset(pMemShmooData->MaxOffsetByMemoryTech, 0x0, sizeof(pMemShmooData->MaxOffsetByMemoryTech));
#else
  #if SUPPORT_FOR_EXTENDED_RANGE
    // IF extended range testing is enabled THEN we will test to the outermost limits
    if (pMemShmooCfg->EnableExtendedRange) {
       // initialize to minimum values
      pMemShmooData->MinOffset = 0;
      pMemShmooData->MaxOffset = 0;
      bssa_memset(pMemShmooData->MinOffsetByMemoryTech, 0x0, sizeof(pMemShmooData->MinOffsetByMemoryTech));
      bssa_memset(pMemShmooData->MaxOffsetByMemoryTech, 0x0, sizeof(pMemShmooData->MaxOffsetByMemoryTech));

      // initialize the (unsigned) minimum and maximum values to the largest
      // possible values
      bssa_memset(pMemShmooData->MinOffsets, 0xFF, sizeof(pMemShmooData->MinOffsets));
      bssa_memset(pMemShmooData->MaxOffsets, 0xFF, sizeof(pMemShmooData->MaxOffsets));
    }
    // ELSE we only test to the innermost limits
    else
  #endif // SUPPORT_FOR_EXTENDED_RANGE
    {
      pMemShmooData->MinOffset = -30000; // initialize to large negative value
      pMemShmooData->MaxOffset = 30000; // initialize to large positive value
      bssa_memset(pMemShmooData->MinOffsetByMemoryTech, -30000, sizeof(pMemShmooData->MinOffsetByMemoryTech));
      bssa_memset(pMemShmooData->MaxOffsetByMemoryTech, 30000, sizeof(pMemShmooData->MaxOffsetByMemoryTech));
    }
#endif // !SUPPORT_FOR_SERIALIZATION

  pMemShmooData->SettleDelay = 0; // initialize to smallest positive value
  bssa_memset(pMemShmooData->SettleDelayByMemoryTech, 0, sizeof(pMemShmooData->SettleDelayByMemoryTech));

  // FOR each socket
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel has no lanes enabled for validation THEN skip it
        if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++){
          // IF this DIMM is not enabled for testing THEN skip it
#if SUPPORT_FOR_TURNAROUNDS
          if (!IsDimmATestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel, Dimm) &&
            ((pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] != Turnarounds) ||
            !IsDimmBTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel, Dimm))) {
            continue;
          }
#else
          if (!IsDimmATestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel, Dimm)) {
            continue;
          }
#endif // !SUPPORT_FOR_TURNAROUNDS

#if SUPPORT_FOR_EXTENDED_RANGE
          // IF the margin parameter is strobe specific AND extended range testing is enabled
          if ((pMemShmooData->Specificity & StrobeSpecific) &&
            pMemShmooCfg->EnableExtendedRange) {
            StrobeWidth = GetMarginParamStrobeWidth(SsaServicesHandle, pMemCfg,
              pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup, Socket,
              Controller, Channel, Dimm);
          } // end if the margin parameter is strobe specific
#endif // SUPPORT_FOR_EXTENDED_RANGE

          RankCount = pMemCfg->RankCounts[Socket][Controller][Channel][Dimm];

          // FOR each rank:
          for (Rank = 0; Rank < RankCount; Rank++) {
            // IF this rank is not enabled for testing THEN skip it
#if SUPPORT_FOR_TURNAROUNDS
            if ((!IsRankATestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller, Channel, Dimm, Rank) &&
              ((pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] != Turnarounds) ||
              !IsRankBTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller, Channel, Dimm, Rank)))) {
              continue;
            }
#else
            if (!IsRankATestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller, Channel, Dimm, Rank)) {
              continue;
            }
#endif // !SUPPORT_FOR_TURNAROUNDS

#if SUPPORT_FOR_EXTENDED_RANGE
            // FOR each lane:
            for (Lane = 0; Lane < pSystemInfo->BusWidth; Lane++) {
              // reset the lane bit masks
              bssa_memset(LaneBitmasks, 0, sizeof(LaneBitmasks));

              // IF the margin parameter is lane specific AND extended range testing is enabled
              if ((pMemShmooData->Specificity & LaneSpecific) &&
                pMemShmooCfg->EnableExtendedRange) {
                // set the lane bitmasks to just this lane
                ByteGroup = Lane / 8;

                LaneBitmasks[ByteGroup] = (1 << (Lane % 8)) & \
                  pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup];

                // IF this lane isn't being validated THEN skip it
                if ((LaneBitmasks[ByteGroup] & (1 << (Lane % 8))) == 0) {
                  continue;
                }
              }
              // ELSE IF the margin parameter is strobe specific AND extended range testing is enabled
              else if ((pMemShmooData->Specificity & StrobeSpecific) &&
                pMemShmooCfg->EnableExtendedRange) {
                // IF this lane is not the first of a strobe group THEN skip it
                if ((Lane % StrobeWidth) != 0) {
                  continue;
                }

                // set the lane bitmasks to just this strobe group
                Strobe = Lane / StrobeWidth;

                ByteGroup = Strobe / (8 / StrobeWidth);

                ByteGroupBitmask = (1 << StrobeWidth) - 1;

                ByteGroupBitmask <<= (Strobe % (8 / StrobeWidth)) * StrobeWidth;

                LaneBitmasks[ByteGroup] = ByteGroupBitmask & \
                  pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup];

                // IF no lane in the strobe group is being validated THEN skip it
                if ((LaneBitmasks[ByteGroup] & ByteGroupBitmask) == 0) {
                  continue;
                }
              }
              // ELSE the margin parameter is neither lane nor strobe specific
              // OR extended range testing is not enabled
              else {
                // IF this lane is not the first of this device THEN skip remaining lanes
                if (Lane != 0) {
                  break;
                }
#endif // SUPPORT_FOR_EXTENDED_RANGE

                bssa_memcpy(LaneBitmasks,
                  pMemPointTestCfg->LaneValBitmasks[Controller][Channel],
                  sizeof(LaneBitmasks));

#if SUPPORT_FOR_EXTENDED_RANGE
              } // end else the margin parameter is neither lane nor strobe specific ...
#endif // SUPPORT_FOR_EXTENDED_RANGE

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_FIXUP_DEBUG_LOG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  GetMarginParamLimits(\n"
                "    Socket=%u Controller=%u Channel=%u Dimm=%u Rank=%u\n"
                "    LaneBitmasks=0x",
                Socket, Controller, Channel, Dimm, Rank);
              LogByteArrayAsHex(SsaServicesHandle, LaneBitmasks,
                (pSystemInfo->BusWidth / 8));
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                " IoLevel=%u MarginGroup=%u)\n",
                pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup);
#endif
              RetStat = SsaServicesHandle->SsaMemoryConfig->GetMarginParamLimits(
                SsaServicesHandle, Socket, Controller, Channel, Dimm, Rank,
                LaneBitmasks, pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup,
                &TmpMinOffset, &TmpMaxOffset, &TmpSettleDelay, &pMemShmooData->StepUnit);
              // IF the margin parameter isn't supported for this device THEN skip it
              if (RetStat == UnsupportedValue) {
                continue;
              }
              if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
                return 1;
              }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_FIXUP_DEBUG_LOG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "    TmpMinOffset=%d TmpMaxOffset=%d TmpSettleDelay=%u\n",
                TmpMinOffset, TmpMaxOffset, TmpSettleDelay);
#endif

              // check for new min-offset, max-offset, and delay values
#if SUPPORT_FOR_SERIALIZATION
              if (TmpMinOffset < pMemShmooData->MinOffset) {
                pMemShmooData->MinOffset = TmpMinOffset;
              }
              if (TmpMaxOffset > pMemShmooData->MaxOffset) {
                pMemShmooData->MaxOffset = TmpMaxOffset;
              }

              // update the min/max per memory tech
              if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT) {
                // DDR4
                if (TmpMinOffset < pMemShmooData->MinOffsetByMemoryTech[0]) {
                  pMemShmooData->MinOffsetByMemoryTech[0] = TmpMinOffset;
                }
                if (TmpMaxOffset > pMemShmooData->MaxOffsetByMemoryTech[0]) {
                  pMemShmooData->MaxOffsetByMemoryTech[0] = TmpMaxOffset;
                }
              } else {
                // DDRT
                if (TmpMinOffset < pMemShmooData->MinOffsetByMemoryTech[1]) {
                  pMemShmooData->MinOffsetByMemoryTech[1] = TmpMinOffset;
                }
                if (TmpMaxOffset > pMemShmooData->MaxOffsetByMemoryTech[1]) {
                  pMemShmooData->MaxOffsetByMemoryTech[1] = TmpMaxOffset;
                }
              } // MemoryTech
#elif SUPPORT_FOR_EXTENDED_RANGE
              // IF extended range testing is enabled THEN we will test to the
              // outermost limits
              if (pMemShmooCfg->EnableExtendedRange) {
                if (TmpMinOffset < pMemShmooData->MinOffset) {
                  pMemShmooData->MinOffset = TmpMinOffset;
                }
                if (TmpMaxOffset > pMemShmooData->MaxOffset) {
                  pMemShmooData->MaxOffset = TmpMaxOffset;
                }

                // update the min/max per memory tech
                if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT) {
                  // DDR4
                  if (TmpMinOffset < pMemShmooData->MinOffsetByMemoryTech[0]) {
                    pMemShmooData->MinOffsetByMemoryTech[0] = TmpMinOffset;
                  }
                  if (TmpMaxOffset > pMemShmooData->MaxOffsetByMemoryTech[0]) {
                    pMemShmooData->MaxOffsetByMemoryTech[0] = TmpMaxOffset;
                  }
                } else {
                  // DDRT
                  if (TmpMinOffset < pMemShmooData->MinOffsetByMemoryTech[1]) {
                    pMemShmooData->MinOffsetByMemoryTech[1] = TmpMinOffset;
                  }
                  if (TmpMaxOffset > pMemShmooData->MaxOffsetByMemoryTech[1]) {
                    pMemShmooData->MaxOffsetByMemoryTech[1] = TmpMaxOffset;
                  }
                } // MemoryTech

                TmpOffset = (UINT8) ABS(TmpMinOffset);
                // IF this offset is inside the current minimum
                if (pMemShmooData->MinOffsets[Socket][Controller][Channel][Lane] > TmpOffset) {
                  // this is the new minimum
                  pMemShmooData->MinOffsets[Socket][Controller][Channel][Lane] = TmpOffset;
                }
                TmpOffset = (UINT8) TmpMaxOffset;
                // IF this offset is inside the current maximum
                if (pMemShmooData->MaxOffsets[Socket][Controller][Channel][Lane] > TmpOffset) {
                  // this is the new maximum
                  pMemShmooData->MaxOffsets[Socket][Controller][Channel][Lane] = TmpOffset;
                }
              }
              // ELSE we only test to the innermost limits
              else
              {
                if (TmpMinOffset > pMemShmooData->MinOffset) {
                  pMemShmooData->MinOffset = TmpMinOffset;
                }
                if (TmpMaxOffset < pMemShmooData->MaxOffset) {
                  pMemShmooData->MaxOffset = TmpMaxOffset;
                }
                // update the min/max per memory tech
                if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT) {
                  // DDR4
                  if (TmpMinOffset > pMemShmooData->MinOffsetByMemoryTech[0]) {
                    pMemShmooData->MinOffsetByMemoryTech[0] = TmpMinOffset;
                  }
                  if (TmpMaxOffset < pMemShmooData->MaxOffsetByMemoryTech[0]) {
                    pMemShmooData->MaxOffsetByMemoryTech[0] = TmpMaxOffset;
                  }
                } else {
                  // DDRT
                  if (TmpMinOffset > pMemShmooData->MinOffsetByMemoryTech[1]) {
                    pMemShmooData->MinOffsetByMemoryTech[1] = TmpMinOffset;
                  }
                  if (TmpMaxOffset < pMemShmooData->MaxOffsetByMemoryTech[1]) {
                    pMemShmooData->MaxOffsetByMemoryTech[1] = TmpMaxOffset;
                  }
                } // MemoryTech
              }
#else // !SUPPORT_FOR_EXTENDED_RANGE && !SUPPORT_FOR_SERIALIZATION
              if (TmpMinOffset > pMemShmooData->MinOffset) {
                pMemShmooData->MinOffset = TmpMinOffset;
              }
              if (TmpMaxOffset < pMemShmooData->MaxOffset) {
                pMemShmooData->MaxOffset = TmpMaxOffset;
              }
              // update the min/max per memory tech
              if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT) {
                // DDR4
                if (TmpMinOffset > pMemShmooData->MinOffsetByMemoryTech[0]) {
                  pMemShmooData->MinOffsetByMemoryTech[0] = TmpMinOffset;
                }
                if (TmpMaxOffset < pMemShmooData->MaxOffsetByMemoryTech[0]) {
                  pMemShmooData->MaxOffsetByMemoryTech[0] = TmpMaxOffset;
                }
              } else {
                // DDRT
                if (TmpMinOffset > pMemShmooData->MinOffsetByMemoryTech[1]) {
                  pMemShmooData->MinOffsetByMemoryTech[1] = TmpMinOffset;
                }
                if (TmpMaxOffset < pMemShmooData->MaxOffsetByMemoryTech[1]) {
                  pMemShmooData->MaxOffsetByMemoryTech[1] = TmpMaxOffset;
                }
              } // MemoryTech
#endif // !SUPPORT_FOR_EXTENDED_RANGE && !SUPPORT_FOR_SERIALIZATION
              if (TmpSettleDelay > pMemShmooData->SettleDelay) {
                pMemShmooData->SettleDelay = TmpSettleDelay;
              }
              // update the min/max per memory tech
              if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT) {
                // DDR4
                pMemShmooData->SettleDelayByMemoryTech[0] = TmpSettleDelay;
                pMemShmooData->StepUnitByMemoryTech[0] = pMemShmooData->StepUnit;
              } else {
                // DDRT
                pMemShmooData->SettleDelayByMemoryTech[1] = TmpSettleDelay;
                pMemShmooData->StepUnitByMemoryTech[1] = pMemShmooData->StepUnit;
              } // MemoryTech
#if SUPPORT_FOR_EXTENDED_RANGE
            } // end for each lane
#endif // SUPPORT_FOR_EXTENDED_RANGE

            // IF the margin parameter is not rank specific THEN exit the rank for loop
            if ((pMemShmooData->Specificity & RankSpecific) == 0) {
              break;
            }
          } // end for each rank

          // IF the margin parameter is not rank specific THEN exit the DIMM for loop
          if ((pMemShmooData->Specificity & RankSpecific) == 0) {
            break;
          }
        } // end for each DIMM

        // IF the margin parameter is not channel specific THEN exit the channel for loop
        if ((pMemShmooData->Specificity & ChannelSpecific) == 0) {
          break;
        }
      } // end for each channel

      // IF the margin parameter is not controller specific THEN exit the controller for loop
      if ((pMemShmooData->Specificity & ControllerSpecific) == 0) {
        break;
      }
    } // end for each controller

    // IF the margin parameter is not socket specific THEN exit the socket for loop
    if ((pMemShmooData->Specificity & SocketSpecific) == 0) {
      break;
    }
  } // end for each socket

  // align the min/max values on the step size
  pMemShmooData->MinOffset /= pMemShmooCfg->StepSize;
  pMemShmooData->MinOffset *= pMemShmooCfg->StepSize;
  pMemShmooData->MaxOffset /= pMemShmooCfg->StepSize;
  pMemShmooData->MaxOffset *= pMemShmooCfg->StepSize;

  for (i = 0; i <= 1; i++){
    pMemShmooData->MinOffsetByMemoryTech[i] /= pMemShmooCfg->StepSize;
    pMemShmooData->MinOffsetByMemoryTech[i] *= pMemShmooCfg->StepSize;
    pMemShmooData->MaxOffsetByMemoryTech[i] /= pMemShmooCfg->StepSize;
    pMemShmooData->MaxOffsetByMemoryTech[i] *= pMemShmooCfg->StepSize;
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS || ENBL_FIXUP_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  MinOffset=%d MaxOffset=%d SettleDelay=%u\n",
    pMemShmooData->MinOffset, pMemShmooData->MaxOffset, pMemShmooData->SettleDelay);
#endif
  return 0;
} // end function GetMarginParamInfo

/**
  This function is used to get the incremental steppings if required based on the 
  margin param sensitivity. The input param decideds whether to count IncrementalSteppings

  @param[in, out]  SsaServicesHandle         Pointer to SSA services
  @param[in]       NewOffset                 Pointer to the new offset
  @param[in]       pCurrentOffset            Pointer to current offset
  @param[in]       pMemShmooCfg              Pointer to memory shmoo test configuration structure
  @param[in]       EnableIncrementalStepping Flag to enable incremental stepping.
  @param[out]      StepCount                 Pointer to number of steps to take while changing the margin param from current
  @param[out]      LastStepSize              Pointer to size of last step if the offset difference is not an integral number of steps
  @param[out]      IncrementalStepSize       Pointer to step size var in smaller increments

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 FindIncrementalStep(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN INT16 *NewOffset,
  IN INT16 *pCurrentOffset,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN BOOLEAN EnableIncrementalStepping,
  OUT UINT8 *StepCount,
  OUT INT16 *LastStepSize,
  OUT INT16 *IncrementalStepSize) 
{
  UINT8 OffsetDiff;

  // for sensitive parameters make multiple steps in smaller increments
  if (EnableIncrementalStepping) {
    if (*NewOffset > *pCurrentOffset) {
      *IncrementalStepSize = pMemShmooCfg->IncrementalStepSize;
    }
    else {
      *IncrementalStepSize = -1 * pMemShmooCfg->IncrementalStepSize;
    }

    OffsetDiff = ((UINT8) ABS(*NewOffset - *pCurrentOffset));

    *StepCount = OffsetDiff / pMemShmooCfg->IncrementalStepSize;

    // IF the offset difference is not an integral number of incremental steps
    if (OffsetDiff % pMemShmooCfg->IncrementalStepSize) {
      (*StepCount)++;
      *LastStepSize = OffsetDiff % pMemShmooCfg->IncrementalStepSize;
      if (*NewOffset < *pCurrentOffset) {
        *LastStepSize *= -1;
      }
    }
    // ELSE the offset difference is an integral number of incremental steps
    else {
      *LastStepSize = *IncrementalStepSize;
    }
  }
  // ELSE incremental stepping is not enabled
  else {
    // move the full distance in one step
    *IncrementalStepSize = *LastStepSize = *NewOffset - *pCurrentOffset;
    *StepCount = 1;
  }
  return 0;
} // end function FindIncrementalStep

/**
  This function is used to set the margin parameter to the given offset at the channel level.
  This function also waits the given settling delay amount before returning.

  Note that the margin parameter is only applied to logical ranks that are
  enabled for validation.

  @param[in, out]  SsaServicesHandle   Pointer to SSA services.
  @param[in]       Socket              Socket number to set margin param
  @param[in]       Controller          Controller number to set margin param
  @param[in]       Channel             Channel number to set margin param
  @param[in]       pSystemInfo         Pointer to system information structure.
  @param[in]       pMemCfg             Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg        Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg    Pointer to generic memory point test configuration structure.
  @param[in]       pVoidMemShmooData   Pointer to memory shmoo test data structure.
  @param[in]       pCurrentOffset      Pointer to current offset.
  @param[in]       NewOffset           New offset.
  @param[in]       StepCount           Number of steps to take while changing the margin param from current based on IncrementalStepSize
  @param[in]       LastStepSize        Size of last step if the offset difference is not an integral number of incremental steps
  @param[in]       IncrementalStepSize Step size for sensitive parameters in smaller increments
  @param[in, out]  pPointTestData      Pointer to specific memory point test data structure.
  @param[out]      pTestStat           Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 SetMarginParamPerChannel(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST UINT8 Socket,
  IN CONST UINT8 Controller,
  IN CONST UINT8 Channel,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pVoidMemShmooData,
  IN INT16 *pCurrentOffset,
  IN INT16 NewOffset,
  IN UINT8 StepCount,
  IN INT16 LastStepSize,
  IN INT16 IncrementalStepSize,
  IN OUT VOID *pPointTestData,
  OUT TEST_STATUS *pTestStat)
{
  SSA_STATUS RetStat;
  UINT8 Dimm, Rank;
  UINT8 LaneBitmasks[MAX_BUS_WIDTH / 8];
  MEM_SHMOO_TEST_DATA *pMemShmooData = (MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  UINT8 PairIdx;
  UINT8 PairCount;
  DIMM_RANK_PAIR DimmRankPairs[2];
  INT16 TmpNewOffset, TmpCurrentOffset;
#if SUPPORT_FOR_EXTENDED_RANGE
  UINT8 StrobeWidth = 8;
  UINT8 ByteGroup, Strobe, Lane;
  UINT8 ByteGroupBitmask;
  INT16 MinOffset, MaxOffset;
  BOOLEAN LaneFound;
  UINT8 StepSize = pMemShmooCfg->StepSize;
#endif // SUPPORT_FOR_EXTENDED_RANGE

  if (*pCurrentOffset != NewOffset) {
    // IF this channel has no lanes enabled for validation THEN skip it
    if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
      pMemPointTestCfg, Socket, Controller, Channel)) {
      return 0;
    }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "MemShmoo.SetMarginParamPerChannel(CurrentOffset=%d, NewOffset=%d)\n",
      *pCurrentOffset, NewOffset);
#else
    // IF the new offset is different from the current offset
    if (*pCurrentOffset != NewOffset) {
      if (pMemShmooData->TestStatusLogLevel >= 4) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "\nMoving IoLevel=%u (%s) MarginGroup=%u (%s) from %d to %d for socket=%d, controller=%d and channel=%d\n",
          pMemShmooCfg->IoLevel, GetIoLevelStr(pMemShmooCfg->IoLevel),
          pMemShmooCfg->MarginGroup, GetMarginGroupStr(pMemShmooCfg->MarginGroup),
          *pCurrentOffset, NewOffset, Socket, Controller, Channel);
        }
      } // end if the new offset is different from the current offset
#endif

    PairCount = 2;
    if (pMemShmooData->GetPointTestDimmRanks(SsaServicesHandle, pSystemInfo,
      pMemCfg, pMemPointTestCfg, pPointTestData, Socket, Controller,
      Channel, &PairCount, &DimmRankPairs[0], pTestStat)) {
      return 1;
    }

    // FOR each DIMM/rank pair:
    for (PairIdx = 0; PairIdx < PairCount; PairIdx++) {
      Dimm = DimmRankPairs[PairIdx].Dimm;
      Rank = DimmRankPairs[PairIdx].Rank;

      TmpCurrentOffset = *pCurrentOffset;
      TmpNewOffset = NewOffset;

  #if SUPPORT_FOR_EXTENDED_RANGE
      // IF the margin parameter is strobe specific AND extended range testing is enabled
      if ((pMemShmooData->Specificity & StrobeSpecific) &&
        pMemShmooCfg->EnableExtendedRange) {
        StrobeWidth = GetMarginParamStrobeWidth(SsaServicesHandle,
          pMemCfg, pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup,
          Socket, Controller, Channel, Dimm);
      } // end if the margin parameter is strobe specific
  #endif // SUPPORT_FOR_EXTENDED_RANGE

  #if SUPPORT_FOR_EXTENDED_RANGE
      // FOR each lane:
      for (Lane = 0; Lane < pSystemInfo->BusWidth; Lane++) {
        // IF the margin parameter is lane specific AND extended range testing is enabled
        if ((pMemShmooData->Specificity & LaneSpecific) &&
          pMemShmooCfg->EnableExtendedRange) {
        }
        // ELSE IF the margin parameter is strobe specific AND extended range testing is enabled
        else if ((pMemShmooData->Specificity & StrobeSpecific) &&
          pMemShmooCfg->EnableExtendedRange) {
          // IF this lane is not the first of a strobe group THEN skip it
          if ((Lane % StrobeWidth) != 0) {
            continue;
          }
        }
        // ELSE the margin parameter is neither lane nor strobe specific
        // OR extended range testing is not enabled
        else {
          // IF this lane is not the first of this device THEN skip remaining lanes
          if (Lane != 0) {
            break;
          }
        }

        // reset the lane bit masks
        bssa_memset(LaneBitmasks, 0, sizeof(LaneBitmasks));

        // reset the temporary offset values
        TmpCurrentOffset = *pCurrentOffset;
        TmpNewOffset = NewOffset;

        // get this lane's minimum and maximum offset values
        MinOffset = (INT16) pMemShmooData->MinOffsets[Socket][Controller][Channel][Lane] * -1;
        MaxOffset = (INT16) pMemShmooData->MaxOffsets[Socket][Controller][Channel][Lane];

        // IF the margin parameter is lane specific AND extended range testing is enabled
        if ((pMemShmooData->Specificity & LaneSpecific) &&
          pMemShmooCfg->EnableExtendedRange) {
          // set the lane bitmasks to just this lane
          ByteGroup = Lane / 8;

          ByteGroupBitmask = (1 << (Lane % 8));

          LaneBitmasks[ByteGroup] = ByteGroupBitmask & \
            pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup];
        }
        // ELSE IF the margin parameter is strobe specific AND extended range testing is enabled
        else if ((pMemShmooData->Specificity & StrobeSpecific) &&
          pMemShmooCfg->EnableExtendedRange) {
          // set the lane bitmasks to just this strobe group
          Strobe = Lane / StrobeWidth;

          ByteGroup = Strobe / (8 / StrobeWidth);

          ByteGroupBitmask = (1 << StrobeWidth) - 1;

          ByteGroupBitmask <<= (Strobe % (8 / StrobeWidth)) * StrobeWidth;

          LaneBitmasks[ByteGroup] = ByteGroupBitmask & \
            pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup];
        }
        // ELSE the margin parameter is neither lane nor strobe specific
        // OR extended range testing is not enabled
        else {
          bssa_memcpy(LaneBitmasks,
            pMemPointTestCfg->LaneValBitmasks[Controller][Channel],
            (pSystemInfo->BusWidth / 8));
        } // end else the margin parameter is neither lane nor strobe specific

        // ensure that at least one lane is set
        LaneFound = FALSE;
        for (ByteGroup = 0; ByteGroup < (pSystemInfo->BusWidth / 8); ByteGroup++) {
          if (LaneBitmasks[ByteGroup] != 0) {
            LaneFound = TRUE;
            break;
          }
        }
        // IF no lanes are set THEN skip the operation
        if (!LaneFound) {
          continue;
        }

        // IF extended range testing is enabled
        if (pMemShmooCfg->EnableExtendedRange) {
          // IF both the current and the new offset are out of range on
          // the same side for this device+lanes
          if (((TmpCurrentOffset < MinOffset) &&
            (TmpNewOffset < MinOffset)) ||
            ((TmpCurrentOffset > MaxOffset) &&
            (TmpNewOffset > MaxOffset))) {
              // skip this device+lanes
              continue;
          }
          // IF both the current and the new offset are out of range on
          // opposite sides for this device+lanes
          if (((TmpCurrentOffset < MinOffset) &&
            (TmpNewOffset > MaxOffset)) ||
            ((TmpCurrentOffset > MaxOffset) &&
            (TmpNewOffset < MinOffset))) {
              // IF the current offset is below this device+lanes' minimum value
              if (TmpCurrentOffset < MinOffset) {
                // adjust the offsets to move from/to the limits (aligning on step size)
                TmpCurrentOffset = (MinOffset / StepSize) * StepSize;
                TmpNewOffset = (MaxOffset / StepSize) * StepSize;
              }
              // ELSE the current offset is above this device+lanes' maximum value
              else
              {
                TmpCurrentOffset = (MaxOffset / StepSize) * StepSize;
                TmpNewOffset = (MinOffset / StepSize) * StepSize;
              }
          }
          // ELSE IF the current offset is below this device+lanes' minimum value
          else if (TmpCurrentOffset < MinOffset) {
            // adjust the offset to move to the limit (aligning on step size)
            TmpCurrentOffset = (MinOffset / StepSize) * StepSize;
          }
          // ELSE IF the current offset is above this device+lanes' maximum value
          else if (TmpCurrentOffset > MaxOffset) {
            TmpCurrentOffset = (MaxOffset / StepSize) * StepSize;
          }
          // ELSE IF the new offset is below this device+lanes' minimum value
          else if (TmpNewOffset < MinOffset) {
            TmpNewOffset = (MinOffset / StepSize) * StepSize;
          }
          // ELSE IF the new offset is above this device+lanes' maximum value
          else if (TmpNewOffset > MaxOffset) {
            TmpNewOffset = (MaxOffset / StepSize) * StepSize;
          }
        } // end if extended range testing is enabled

        // IF there is no change THEN skip it
        if (TmpCurrentOffset == TmpNewOffset) {
          continue;
        }
  #else // !SUPPORT_FOR_EXTENDED_RANGE
        // reset the lane bit masks
        bssa_memset(LaneBitmasks, 0, sizeof(LaneBitmasks));

        bssa_memcpy(LaneBitmasks,
          pMemPointTestCfg->LaneValBitmasks[Controller][Channel],
          (pSystemInfo->BusWidth / 8));
  #endif // !SUPPORT_FOR_EXTENDED_RANGE

  #if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_EXTENDED_RANGE_DEBUG_LOG_MSGS
  #if ENBL_EXTENDED_RANGE_DEBUG_LOG_MSGS
        // IF either offset is different from the requested value THEN
        // ExtendedRange is in play
        if ((TmpCurrentOffset != *pCurrentOffset) ||
          (TmpNewOffset != NewOffset)) {
  #endif // ENBL_EXTENDED_RANGE_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SetMarginParamOffset(\n"
            "    Socket=%u Controller=%u Channel=%u Dimm=%u Rank=%u\n"
            "    LaneBitmasks=0x",
            Socket, Controller, Channel, Dimm, Rank);
          LogByteArrayAsHex(SsaServicesHandle, LaneBitmasks,
            (pSystemInfo->BusWidth / 8));
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            " IoLevel=%u MarginGroup=%u\n"
            "    CurrentOffset=%d NewOffset=%d)\n",
            pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup,
            TmpCurrentOffset, TmpNewOffset);
  #if ENBL_EXTENDED_RANGE_DEBUG_LOG_MSGS
        } // end if either offset is different from the requested value
  #endif // ENBL_EXTENDED_RANGE_DEBUG_LOG_MSGS
  #endif // ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_EXTENDED_RANGE_DEBUG_LOG_MSGS
        RetStat = SsaServicesHandle->SsaMemoryConfig->SetMarginParamOffset(
          SsaServicesHandle, Socket, Controller, Channel, Dimm, Rank,
          LaneBitmasks, pMemShmooCfg->IoLevel,
          pMemShmooCfg->MarginGroup, TmpCurrentOffset, TmpNewOffset);
        // IF the margin parameter isn't supported for this device THEN skip it
        if (RetStat == UnsupportedValue) {
          continue;
        }
        if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__,
          pTestStat)) {
          return 1;
        }

  #if SUPPORT_FOR_EXTENDED_RANGE
      } // end for each lane
  #endif // SUPPORT_FOR_EXTENDED_RANGE

      // IF the margin parameter is not rank specific THEN exit the rank for loop
      if ((pMemShmooData->Specificity & RankSpecific) == 0) {
        break;
      }
    } // end for each DIMM/rank pair
  }
  return 0;
}

/**
  This function is used to set the margin parameter to the given offset.  This
  function also waits the given settling delay amount before returning.  The
  given current offset variable is updated to the new offset.

  Note that the margin parameter is only applied to logical ranks that are
  enabled for validation.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in]       pVoidMemShmooData  Pointer to memory shmoo test data structure.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in, out]  pCurrentOffset     Pointer to current offset.
  @param[in]       NewOffset          New offset.
  @param[in]       EnableIncrementalStepping  Flag to enable incremental stepping.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 SetMarginParam(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pVoidMemShmooData,
  IN OUT VOID *pPointTestData,
  IN OUT INT16 *pCurrentOffset,
  IN INT16 NewOffset,
  IN BOOLEAN EnableIncrementalStepping,
  OUT TEST_STATUS *pTestStat)
{
//  SSA_STATUS RetStat;
  UINT8 Socket, Controller, Channel;//, Dimm, Rank;
//  UINT8 LaneBitmasks[MAX_BUS_WIDTH / 8];
  MEM_SHMOO_TEST_DATA *pMemShmooData = (MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  INT16 IncrementalStepSize;
  INT16 LastStepSize;
  UINT8 StepNumber, StepCount;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.SetMarginParam(CurrentOffset=%d, NewOffset=%d EnableIncrementalStepping=%u)\n",
    *pCurrentOffset, NewOffset, EnableIncrementalStepping);
#else
  // IF the new offset is different from the current offset
  if (*pCurrentOffset != NewOffset) {
    /*if (pMemShmooData->TestStatusLogLevel >= 4) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "\nMoving IoLevel=%u (%s) MarginGroup=%u (%s) from %d to %d\n",
        pMemShmooCfg->IoLevel, GetIoLevelStr(pMemShmooCfg->IoLevel),
        pMemShmooCfg->MarginGroup, GetMarginGroupStr(pMemShmooCfg->MarginGroup),
        *pCurrentOffset, NewOffset);
    }*/
  } // end if the new offset is different from the current offset
#endif

  // IF the new offset is different from the current offset
  if (*pCurrentOffset != NewOffset) {
    FindIncrementalStep(SsaServicesHandle, &NewOffset, pCurrentOffset, pMemShmooCfg, EnableIncrementalStepping, 
      &StepCount, &LastStepSize, &IncrementalStepSize);

    // FOR each step:
    for (StepNumber = 0; StepNumber < StepCount; StepNumber++) {
      // IF this is the last step
      if (StepNumber == (StepCount - 1)) {
        // use the last step size
        NewOffset = *pCurrentOffset + LastStepSize;
      }
      else {
        // use the incremental step size
        NewOffset = *pCurrentOffset + IncrementalStepSize;
      }

      // FOR each socket:
      for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
        // FOR each controller:
        for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
          // FOR each channel:
          for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
            
            if(SetMarginParamPerChannel(SsaServicesHandle, Socket, Controller, Channel, pSystemInfo, pMemCfg,
              pMemShmooCfg, pMemPointTestCfg, pVoidMemShmooData, pCurrentOffset, 
              NewOffset, StepCount, LastStepSize, IncrementalStepSize, pPointTestData, pTestStat)) {
                return 1;
            }

            // IF the margin parameter is not channel specific THEN exit the channel for loop
            if ((pMemShmooData->Specificity & ChannelSpecific) == 0) {
              break;
            }
          } // end for each channel

          // IF the margin parameter is not controller specific THEN exit the controller for loop
          if ((pMemShmooData->Specificity > ControllerSpecific) == 0) {
            break;
          }
        } // end for each controller

        // IF the margin parameter is not socket specific THEN exit the socket for loop
        if ((pMemShmooData->Specificity & SocketSpecific) == 0) {
          break;
        }
      } // end for each socket

      // wait for the margin parameter to settle
      if (pMemShmooData->SettleDelay != 0) {
        SsaServicesHandle->SsaCommonConfig->Wait(SsaServicesHandle,
          pMemShmooData->SettleDelay);
      }

      *pCurrentOffset = NewOffset;
    } // end for each step
  } // end if the new offset is different from the current offset

  return 0;
} // end function SetMarginParam

/**
  This function is used to set Margin Param to nominal for the perticular channel

  @param[in, out]  SsaServicesHandle Pointer to SSA services.
  @param[in]       Socket            Socket number to set margin param
  @param[in]       Controller        Controller number to set margin param
  @param[in]       Channel           Channel number to set margin param
  @param[in]       pSystemInfo       Pointer to system information structure.
  @param[in]       pMemCfg           Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg      Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg  Pointer to generic memory point test configuration structure.
  @param[in]       pMemShmooData     Pointer to memory shmoo test data structure.
  @param[in, out]  pPointTestData    Pointer to specific memory point test data structure.
  @param[in, out]  pCurrentOffset    Pointer to current offset.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 SetMarginParamNominalForChannel(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  IN INT16 *pCurrentOffset,
  OUT TEST_STATUS *pTestStat)
{
  INT16 NewOffset = 0;
  INT16 IncrementalStepSize;
  INT16 LastStepSize;
  UINT8 StepNumber, StepCount;
  if (*pCurrentOffset != NewOffset) {
    FindIncrementalStep(SsaServicesHandle, &NewOffset, pCurrentOffset, pMemShmooCfg, pMemShmooCfg->EnableIncrementalStepping, 
      &StepCount, &LastStepSize, &IncrementalStepSize);

    for (StepNumber = 0; StepNumber < StepCount; StepNumber++) {
      // IF this is the last step
      if (StepNumber == (StepCount - 1)) {
        // use the last step size
        NewOffset = *pCurrentOffset + LastStepSize;
      }
      else {
        // use the incremental step size
        NewOffset = *pCurrentOffset + IncrementalStepSize;
      }
      if(SetMarginParamPerChannel(SsaServicesHandle, Socket, Controller, Channel, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pMemShmooData, pCurrentOffset, 
        NewOffset, StepCount, LastStepSize, IncrementalStepSize, pPointTestData, pTestStat)){
          return 1;
      }
    }
  }
  return 0;
} // end function SetMarginParamNominalForChannel

/**
  This function is used to check whether the shmoo stop modes have been
  satisfied.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pMemShmooData      Pointer to memory shmoo test data structure.
  @param[out]      pShmooStopModesSatisfied  Pointer to shmoo stop modes satisfied status.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 ChkShmooStopModesSatisfied(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN CONST MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  OUT BOOLEAN *pShmooStopModesSatisfied,
  IN INT16 *pCurrentOffset,
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller, Channel;
  BOOLEAN AnySocketSatisfied, AllSocketsSatisfied;
  BOOLEAN AnyControllerSatisfied, AllControllersSatisfied;
  BOOLEAN AnyChannelSatisfied, AllChannelsSatisfied;
  BOOLEAN AnyRankSatisfied, AllRanksSatisfied;
  LANE_FAIL_STAT LaneFailStat;
  UINT8 RankCombinationIndex;
  UINT8 MaxRankCombinationsCnt;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.ChkShmooStopModesSatisfied()\n");
#endif
  *pShmooStopModesSatisfied = FALSE;

  if (GetRankCombinationsCnts(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, pMemShmooCfg->IoLevel, pMemShmooCfg->IsDdrT, NULL,
    &MaxRankCombinationsCnt, pTestStat)) {
    return 1;
  }

  // IF any of the shmoo stop modes are do-not-stop THEN the shmoo stop modes
  // are never satisfied
  if ((pMemShmooCfg->LaneShmooStopMode == DoNoStopShmooStopMode) ||
    (pMemShmooCfg->RankShmooStopMode == DoNoStopShmooStopMode) ||
    (pMemShmooCfg->ChannelShmooStopMode == DoNoStopShmooStopMode) ||
    (pMemShmooCfg->ControllerShmooStopMode == DoNoStopShmooStopMode) ||
    (pMemShmooCfg->SocketShmooStopMode == DoNoStopShmooStopMode)) {
    return FALSE;
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "ChkShmooStopModesSatisfied()\n");
#endif
  AnySocketSatisfied = FALSE;
  AllSocketsSatisfied = TRUE;
  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is not enabled for testing THEN skip it
    if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
      pMemPointTestCfg, Socket)) {
      continue;
    }

#if ENBL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Socket=%u\n", Socket);
#endif
    AnyControllerSatisfied = FALSE;
    AllControllersSatisfied = TRUE;
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not enabled for testing THEN skip it
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

#if ENBL_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  Controller=%u\n", Controller);
#endif
      AnyChannelSatisfied = FALSE;
      AllChannelsSatisfied = TRUE;
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel has no lanes enabled for validation THEN skip it
        if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller, Channel)) {
          continue;
        }

#if ENBL_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      Channel=%u\n", Channel);
#endif
        AnyRankSatisfied = FALSE;
        AllRanksSatisfied = TRUE;
        // FOR each rank combination:
        for (RankCombinationIndex = 0;
          RankCombinationIndex < MaxRankCombinationsCnt;
          RankCombinationIndex++) {
#if ENBL_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "        RankCombinationIndex=%u\n", RankCombinationIndex);
#endif
          // IF this is not the first iteration THEN exit the rank combination for loop now
          if (RankCombinationIndex != 0) {
            break;
          }

          // get the lane failure status from the memory point test
          LaneFailStat = \
            pMemShmooData->pLaneFailStats[RankCombinationIndex][Socket][Controller][Channel];

#if ENBL_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "          AnyLaneFailed=%u AllLanesFailed=%u\n",
            LaneFailStat.AnyLaneFailed, LaneFailStat.AllLanesFailed);
#endif
          if (pMemShmooCfg->LaneShmooStopMode == AnyShmooStopMode) {
            if (LaneFailStat.AnyLaneFailed) {
              AnyRankSatisfied = TRUE;
            }
            else {
              AllRanksSatisfied = FALSE;
            }
          }
          // ELSE the lane shmoo-stop-mode is "All"
          else {
            if (LaneFailStat.AllLanesFailed) {
              AnyRankSatisfied = TRUE;
            }
            else {
              AllRanksSatisfied = FALSE;
            }
          } // end else the lane shmoo-stop-mode is "All"

#if ENBL_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "          AnyRankSatisfied=%u AllRanksSatisfied=%u\n",
            AnyRankSatisfied, AllRanksSatisfied);
#endif
        } // end for each rank combination

        if (pMemShmooCfg->RankShmooStopMode == AnyShmooStopMode) {
          if (AnyRankSatisfied) {
            AnyChannelSatisfied = TRUE;
            if(pMemShmooCfg->StopChannelMarginOnStopCond && !(pMemShmooCfg->EnableOptimizedShmoo)
              && (pMemShmooCfg->ChannelShmooStopMode != DoNoStopShmooStopMode)) {
              if (SetMarginParamNominalForChannel(SsaServicesHandle, Socket, Controller, Channel, pSystemInfo, pMemCfg, pMemShmooCfg,
                pMemPointTestCfg, pMemShmooData, pPointTestData, pCurrentOffset, pTestStat)) {
                  return 1;
              }
              pMemShmooCfg->ChannelValCompletionBitmasks[Socket][Controller] |= (0x1 << Channel);
              pMemPointTestCfg->ChannelValBitmasks[Socket][Controller] &= ~(0x1 << Channel);
#if ENBL_DEBUG_LOG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              " Channel Validation Completed and moved to nominalSocket=%d, Controller=%d, Channel=%d\n",
              Socket, Controller, Channel);
#endif 
            }
          }
          else {
            AllChannelsSatisfied = FALSE;
          }
        }
        // ELSE the rank shmoo-stop-mode is "All"
        else {
          if (AllRanksSatisfied) {
            AnyChannelSatisfied = TRUE;
            if(pMemShmooCfg->StopChannelMarginOnStopCond && !(pMemShmooCfg->EnableOptimizedShmoo)
              && (pMemShmooCfg->ChannelShmooStopMode != DoNoStopShmooStopMode)) {
              if(SetMarginParamNominalForChannel(SsaServicesHandle, Socket, Controller, Channel, pSystemInfo, pMemCfg,pMemShmooCfg,
                pMemPointTestCfg, pMemShmooData, pPointTestData, pCurrentOffset, pTestStat)) {
                  return 1;
              }
              pMemShmooCfg->ChannelValCompletionBitmasks[Socket][Controller] |= (0x1 << Channel);
              pMemPointTestCfg->ChannelValBitmasks[Socket][Controller] &= ~(0x1 << Channel);
#if ENBL_DEBUG_LOG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              " Channel Validation Completed and moved to nominalSocket=%d, Controller=%d, Channel=%d\n",
              Socket, Controller, Channel);
#endif
            }
          }
          else {
            AllChannelsSatisfied = FALSE;
          }
        } // end else the rank shmoo-stop-mode is "All"

#if ENBL_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "        AnyChannelSatisfied=%u AllChannelsSatisfied=%u\n",
          AnyChannelSatisfied, AllChannelsSatisfied);
#endif
      } // end for each channel

      if (pMemShmooCfg->ChannelShmooStopMode == AnyShmooStopMode) {
        if (AnyChannelSatisfied) {
          AnyControllerSatisfied = TRUE;
        }
        else {
          AllControllersSatisfied = FALSE;
        }
      }
      // ELSE the channel shmoo-stop-mode is "All"
      else {
        if (AllChannelsSatisfied) {
          AnyControllerSatisfied = TRUE;
        }
        else {
          AllControllersSatisfied = FALSE;
        }
      } // end else the channel shmoo-stop-mode is "All"

#if ENBL_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      AnyControllerSatisfied=%u AllControllersSatisfied=%u\n",
        AnyControllerSatisfied, AllControllersSatisfied);
#endif
    } // end for each controller

    if (pMemShmooCfg->ControllerShmooStopMode == AnyShmooStopMode) {
      if (AnyControllerSatisfied) {
        AnySocketSatisfied = TRUE;
      }
      else {
        AllSocketsSatisfied = FALSE;
      }
    }
    // ELSE the controller shmoo-stop-mode is "All"
    else {
      if (AllControllersSatisfied) {
        AnySocketSatisfied = TRUE;
      }
      else {
        AllSocketsSatisfied = FALSE;
      }
    } // end else the controller shmoo-stop-mode is "All"

#if ENBL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    AnySocketSatisfied=%u AllSocketsSatisfied=%u\n",
      AnySocketSatisfied, AllSocketsSatisfied);
#endif
  } // end for each socket

  if (pMemShmooCfg->SocketShmooStopMode == AnyShmooStopMode) {
    if (AnySocketSatisfied) {
      *pShmooStopModesSatisfied = TRUE;
    }
  }
  // ELSE the socket shmoo-stop-mode is "All"
  else {
    if (AllSocketsSatisfied) {
      *pShmooStopModesSatisfied = TRUE;
    }
  } // end else the socket shmoo-stop-mode is "All"

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  ShmooStopModesSatisfied=%u\n",
    *pShmooStopModesSatisfied);
#endif

  return 0;
} // end function ChkShmooStopModesSatisfied

/**
  This function is used to perform DIMM initialization after an error is detected.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pMemShmooData      Pointer to memory shmoo test data.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in, out]  pCurrentOffset     Pointer to current shmoo parameter offset.
  @param[in]       IsLastRankCombo    Flag indicating whether this is the last rank combination.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 DimmInitAfterErr(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  IN OUT INT16 *pCurrentOffset,
  IN BOOLEAN IsLastRankCombo,
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller;
#if SUPPORT_FOR_DDRT
  UINT8 Channel, Dimm;
  UINT8 PairCount;
  DIMM_RANK_PAIR DimmRankPairs[2];
#endif // SUPPORT_FOR_DDRT

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.DimmInitAfterErr(CurrentOffset=%d IsLastRankCombo=%u)\n",
    *pCurrentOffset, IsLastRankCombo);
#endif
  // IF JEDEC initialization is selected
  if (pMemShmooCfg->DimmInitMode == Jedec) {
    INT16 NewOffset;
    BOOLEAN EnableIncrementalStepping;

    if (pMemShmooData->PreDimmInitHandler) {
      if (pMemShmooData->PreDimmInitHandler(SsaServicesHandle, pSystemInfo,
        pMemShmooData->pClientTestCfg, pMemShmooData->pClientTestData,
        pTestStat)) {
        return 1;
      }
    }

    // set the margin parameter back to nominal prior to performing the JEDEC init
    NewOffset = 0;
    EnableIncrementalStepping = FALSE; // incremental stepping not required here
    if (SetMarginParam(SsaServicesHandle, pSystemInfo,
      pMemCfg, pMemShmooCfg, pMemPointTestCfg,
      pMemShmooData, pPointTestData, pCurrentOffset, NewOffset,
      EnableIncrementalStepping, pTestStat)) {
      return 1;
    }
  } // end if JEDEC initialization is selected

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF the margin parameter is controller specific
      if (pMemShmooData->Specificity & ControllerSpecific) {
        // IF the controller is not enabled for validation THEN skip it
        if (!IsControllerValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller)) {
          continue;
        }
      }
      // ELSE the margin parameter is not controller specific
      else {
        // IF the controller is not enabled for testing THEN skip it
        if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo,
          pMemCfg, pMemPointTestCfg, Socket, Controller)) {
          continue;
        }
      }

      // do an I/O reset for either DIMM initialization type
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  IoReset(Socket=%u Controller=%u)\n",
        Socket, Controller);
#endif
      SsaServicesHandle->SsaMemoryConfig->IoReset(SsaServicesHandle,
        Socket, Controller);
    } // end for each controller
  } // end for each socket


#if SUPPORT_FOR_DDRT
      // do an AEP IO reset if the dimm is AEP dimm.
      // FOR each socket:
      for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
        // IF the socket is not enabled for testing THEN skip it
        if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
          pMemPointTestCfg, Socket)) {
          continue;
        }

        // FOR each controller:
        for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
            // FOR each channel:
            for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
              // IF the margin parameter is channel specific
              if (pMemShmooData->Specificity & ChannelSpecific) {
                // IF the channel is not enabled for validation THEN skip it
                if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
                  pMemPointTestCfg, Socket, Controller, Channel)) {
                  continue;
                }
              }
              else {
                // IF the channel is not enabled for testing THEN skip it
                if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
                  pMemPointTestCfg, Socket, Controller, Channel)) {
                  continue;
                }
              }

              PairCount = 2;
              if (pMemShmooData->GetPointTestDimmRanks(SsaServicesHandle, pSystemInfo,
                pMemCfg, pMemPointTestCfg, pPointTestData, Socket, Controller,
                Channel, &PairCount, &DimmRankPairs[0], pTestStat)) {
                return 1;
              }

              for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
                // IF this DIMM isn't being tested THEN skip it
                if (((PairCount < 1) || (Dimm != DimmRankPairs[0].Dimm)) &&
                  ((PairCount < 2) || (Dimm != DimmRankPairs[1].Dimm))) {
                  continue;
                }

                // IF this DIMM isn't DDR-T THEN skip it
                if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT) {
                    continue;
                }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
                NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                  "  AepIoReset(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
                  Socket, Controller, Channel, Dimm);
#endif
                SsaServicesHandle->SsaMemoryDdrtConfig->AepIoReset(
                  SsaServicesHandle, Socket, Controller, Channel, Dimm);
              } // end for each DIMM
            } // end for each channel
          } // end for each controller
      // do FIFO train reset
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  ResetAepFifoTrain(Socket=%u)\n", Socket);
#endif
      SsaServicesHandle->SsaMemoryDdrtConfig->ResetAepFifoTrain(
        SsaServicesHandle, Socket);
      } // end for each socket
#endif // SUPPORT_FOR_DDRT

   // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // FOR each controller:
      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
          // IF the margin parameter is controller specific
          if (pMemShmooData->Specificity & ControllerSpecific) {
            // IF the controller is not enabled for validation THEN skip it
            if (!IsControllerValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller)) {
              continue;
            }
          }
          // ELSE the margin parameter is not controller specific
          else {
            // IF the controller is not enabled for testing THEN skip it
            if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo,
              pMemCfg, pMemPointTestCfg, Socket, Controller)) {
              continue;
            }
          }
      // IF JEDEC initialization is selected
      if (pMemShmooCfg->DimmInitMode == Jedec) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  JedecReset(Socket=%u Controller=%u)\n",
          Socket, Controller);
#endif
        SsaServicesHandle->SsaMemoryConfig->JedecReset(
          SsaServicesHandle, Socket, Controller);
      } // end if JEDEC initialization is selected
    } // end for each controller
  } // end for each socket

#if SUPPORT_FOR_DDRT
  // IF we're testing DDR/DDRT devices and the margin parameter is command/control
  if ((pMemShmooCfg->MarginGroup == CmdAll) ||
    (pMemShmooCfg->MarginGroup == CmdVref) ||
    (pMemShmooCfg->MarginGroup == CtlAll)) {
    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // IF the socket is not enabled for testing THEN skip it
      if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
        pMemPointTestCfg, Socket)) {
        continue;
      }

      // FOR each controller:
      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
        // FOR each channel:
        for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
          // IF the margin parameter is channel specific
          if (pMemShmooData->Specificity & ChannelSpecific) {
            // IF the channel is not enabled for validation THEN skip it
            if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller, Channel)) {
              continue;
            }
          }
          else {
            // IF the channel is not enabled for testing THEN skip it
            if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller, Channel)) {
              continue;
            }
          }

          PairCount = 2;
          if (pMemShmooData->GetPointTestDimmRanks(SsaServicesHandle, pSystemInfo,
            pMemCfg, pMemPointTestCfg, pPointTestData, Socket, Controller,
            Channel, &PairCount, &DimmRankPairs[0], pTestStat)) {
            return 1;
          }

          for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
            // IF this DIMM isn't being tested THEN skip it
            if (((PairCount < 1) || (Dimm != DimmRankPairs[0].Dimm)) &&
              ((PairCount < 2) || (Dimm != DimmRankPairs[1].Dimm))) {
              continue;
            }

            // IF this DIMM isn't DDR-T THEN skip it
            if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT) {
              continue;
            }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  DrainAepRpq(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
              Socket, Controller, Channel, Dimm);
#endif
            SsaServicesHandle->SsaMemoryDdrtConfig->DrainAepRpq(
              SsaServicesHandle, Socket, Controller, Channel, Dimm);
          } // end for each DIMM
        } // end for each channel
      } // end for each controller

      // IF the margin parameter isn't control. the margin parameter is Cmd or CmdVref
      if (pMemShmooCfg->MarginGroup != CtlAll) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  ClearAepPrefetchCache(Socket=%u)\n", Socket);
#endif
        SsaServicesHandle->SsaMemoryDdrtConfig->ClearAepPrefetchCache(
          SsaServicesHandle, Socket);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetAepDimmTrainingMode(Socket=%u Mode=%u)\n",
          Socket, FnvTrainModeLateCmdCk);
#endif
        SsaServicesHandle->SsaMemoryDdrtConfig->SetAepDimmTrainingMode(
          SsaServicesHandle, Socket, FnvTrainModeLateCmdCk);
      } // end if the margin parameter isn't control
    } // end for each socket
  } // end if we're testing DDR devices and the margin parameter is command/control
#endif // SUPPORT_FOR_DDRT

  // IF JEDEC initialization is selected
  if (pMemShmooCfg->DimmInitMode == Jedec) {
    if (pMemShmooData->PostDimmInitHandler) {
      if (pMemShmooData->PostDimmInitHandler(SsaServicesHandle, pSystemInfo,
        pMemCfg, pMemPointTestCfg, pMemShmooData->pClientTestCfg,
        pMemShmooData->pClientTestData, IsLastRankCombo, pTestStat)) {
        return 1;
      }
    }
  } // end if JEDEC initialization is selected

  return 0;
} // end function DimmInitAfterErr

/**
  This function is used to run a point test.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pMemShmooData      Pointer to memory shmoo test data.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in]       TestingOffset      Pointer to shmoo parameter offset where we're to test.
  @param[in, out]  pCurrentOffset     Pointer to current shmoo parameter offset.
  @param[out]      pAnyLaneFailed     Pointer to any lane failed status
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 RunPointTest(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  IN INT16 TestingOffset,
  IN OUT INT16 *pCurrentOffset,
  OUT BOOLEAN *pAnyLaneFailed,
  OUT TEST_STATUS *pTestStat)
{
#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.RunPointTest(CurrentOffset=%d TestingOffset=%d)\n",
    *pCurrentOffset, TestingOffset);
#endif

  // IF there is no lane failure status buffer THEN allocate one
  if (pMemShmooData->pLaneFailStats == NULL) {
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    GetMemPoolSize(SsaServicesHandle);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Malloc(Size=%u) for lane failure status\n",
      pMemShmooData->LaneFailStatsBfrSize);
#endif
    pMemShmooData->pLaneFailStats = \
      (LANE_FAIL_STATS *) SsaServicesHandle->SsaCommonConfig->Malloc(
      SsaServicesHandle, pMemShmooData->LaneFailStatsBfrSize);
    if (pMemShmooData->pLaneFailStats == NULL) {
#if ENBL_VERBOSE_ERROR_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "ERROR: Could not allocate memory for lane failure status\n");
#endif
      ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
      return 1;
    }
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    pBfr=%p\n", pMemShmooData->pLaneFailStats);
    GetMemPoolSize(SsaServicesHandle);
#endif
  }

  // set the margin parameter to the testing offset
  if (SetMarginParam(SsaServicesHandle, pSystemInfo,
    pMemCfg, pMemShmooCfg, pMemPointTestCfg,
    pMemShmooData, pPointTestData, pCurrentOffset, TestingOffset,
    pMemShmooCfg->EnableIncrementalStepping, pTestStat)) {
    return 1;
  }

  // IF there is a margin parameter change handler
  if (pMemShmooData->MarginParamChgHandler) {
    // pass the new offset back upstream
    pMemShmooData->MarginParamChgHandler(
      pMemShmooData->pClientTestData, pMemShmooCfg->IoLevel,
      pMemShmooCfg->MarginGroup, TestingOffset);
  } // end if there is a margin parameter change handler

  // give the client an opportunity to do the same
  if (pMemShmooData->PreRunPointTestHandler != NULL) {
    if (pMemShmooData->PreRunPointTestHandler(SsaServicesHandle, pSystemInfo,
      pMemCfg, pMemPointTestCfg, pMemShmooData->pClientTestCfg,
      pMemShmooData->pClientTestData, pTestStat)) {
      return 1;
    }
  }

  // run a point test at the current offset
  if (pMemShmooData->RunPointTest(
    SsaServicesHandle, pSystemInfo, pMemCfg, pMemPointTestCfg,
    pPointTestCfg, pPointTestData, &pMemShmooData->pLaneFailStats[0],
    pTestStat)) {
    return 1;
  }

  *pAnyLaneFailed = GetAnyLaneFailed(SsaServicesHandle,
    pSystemInfo, pMemCfg, pMemPointTestCfg, &pMemShmooData->pLaneFailStats[0]);

  if (*pAnyLaneFailed) {
    // IF DIMM initialization after an error is detected is enabled
    if (pMemShmooCfg->EnableDimmInitAfterErr) {
      BOOLEAN IsLastRankCombo = TRUE;

      if (DimmInitAfterErr(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, pCurrentOffset, IsLastRankCombo, pTestStat)) {
        return 1;
      }
    } // end if DIMM initialization after an error is detected is enabled
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (Offset=%d AnyLaneFailed=%u)\n", TestingOffset, *pAnyLaneFailed);
#endif

  return 0;
} // end function RunPointTest

/**
  This function implements the binary-search state.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pMemShmooData      Pointer to memory shmoo test data.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in]       ShmooDir           Shmoo direction.
  @param[in]       InnerLimitOffset   Inner shmoo parameter offset not to test beyond.
  @param[in]       OuterLimitOffset   Outer shmoo parameter offset not to test beyond.
  @param[in, out]  pTestingOffset     Pointer to shmoo parameter offset where we're testing.
  @param[in, out]  pCurrentOffset     Pointer to current shmoo parameter offset.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 BinarySearch(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  IN INT16 InnerLimitOffset,
  IN INT16 OuterLimitOffset,
  IN OUT INT16 *pTestingOffset,
  IN OUT INT16 *pCurrentOffset,
  OUT TEST_STATUS *pTestStat)
{
  INT16 BSInnerLimitOffset, BSOuterLimitOffset;
  BOOLEAN BSInnerLimitTested, BSOuterLimitTested;
  INT16 NewOffset;
  UINT32 Idx;
  OFFSET_TEST_STAT OffsetTestStat;
  BOOLEAN AnyLaneFailed;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.BinarySearch(InnerLimitOffset=%d OuterLimitOffset=%d CurrentOffset=%d)\n",
    InnerLimitOffset, OuterLimitOffset, *pCurrentOffset);
#endif

  if (pMemShmooData->TestStatusLogLevel >= 3) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "\nSearching for pass/fail boundary\n");
  }

  if (pMemShmooData->SetPointTestInBinarySearchState(SsaServicesHandle,
    pSystemInfo, pMemCfg, pMemPointTestCfg, pPointTestCfg, pPointTestData,
    TRUE, pTestStat)) {
    return 1;
  }

  BSInnerLimitOffset = InnerLimitOffset;
  BSOuterLimitOffset = OuterLimitOffset;
  BSInnerLimitTested = FALSE;
  BSOuterLimitTested = FALSE;

  // start halfway between the inner and outer limits
  NewOffset = InnerLimitOffset + ((OuterLimitOffset - InnerLimitOffset) / 2);

  // align to the step size
  NewOffset /= pMemShmooCfg->StepSize;
  NewOffset *= pMemShmooCfg->StepSize;

  // WHILE the inner and outer limits are not adjacent or the limits haven't been tested:
  while ((ABS(BSOuterLimitOffset - BSInnerLimitOffset) > pMemShmooCfg->StepSize) ||
    !BSInnerLimitTested || !BSOuterLimitTested) {
    *pTestingOffset = NewOffset;

    Idx = (*pTestingOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

    OffsetTestStat = (OFFSET_TEST_STAT) pMemShmooData->pOffsetTestStat[Idx];

#if ENBL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  get Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
      *pTestingOffset, Idx, OffsetTestStat, __LINE__);
#endif

    // IF the offset was not already tested
    if (OffsetTestStat == NotTested) {
      // run a point test at the testing offset
      if (RunPointTest(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, *pTestingOffset, pCurrentOffset, &AnyLaneFailed,
        pTestStat)) {
        return 1;
      }

#if ENBL_DEBUG_OFFSET_STAT_IDX
      if (ChkOffsetTestStatIdx(SsaServicesHandle, pMemShmooData, __LINE__, Idx,
        pTestStat)) {
        return 1;
      }
#endif // ENBL_DEBUG_OFFSET_STAT_IDX

      pMemShmooData->pOffsetTestStat[Idx] = (UINT8) \
        (AnyLaneFailed) ? ErrsDetected : NoErrsDetected;

#if ENBL_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  set Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
        *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif
    }
    // ELSE this point was already tested
    else {
      // get the system any-lane-failed status
      AnyLaneFailed = (OffsetTestStat == ErrsDetected) ? TRUE : FALSE;
    } // end else this point was already tested

    if (*pTestingOffset == BSOuterLimitOffset) {
      BSOuterLimitTested = TRUE;
    }

    if (*pTestingOffset == BSInnerLimitOffset) {
      BSInnerLimitTested = TRUE;
    }

    // IF errors were detected on any lane on any channel
    if (AnyLaneFailed) {
      // the tested offset is the new outer limit
      BSOuterLimitOffset = *pTestingOffset;
    }
    // ELSE no errors were detected on any lane of any channel
    else {
      // the tested offset is the new inner limit
      BSInnerLimitOffset = *pTestingOffset;
    }

    // test halfway between the inner and outer limits
    NewOffset = BSInnerLimitOffset + ((BSOuterLimitOffset - BSInnerLimitOffset) / 2);

    // align to the step size
    NewOffset /=  pMemShmooCfg->StepSize;
    NewOffset *=  pMemShmooCfg->StepSize;

    // IF the offset to test is the inner limit and the inner limit's already been tested
    if ((NewOffset == BSInnerLimitOffset) && BSInnerLimitTested) {
      // test the outer limit instead
      NewOffset = BSOuterLimitOffset;
    }

    // IF the offset to test is the outer limit and the outer limit's already been tested
    if ((NewOffset == BSOuterLimitOffset) && BSOuterLimitTested) {
      // test the inner limit instead
      NewOffset = BSInnerLimitOffset;
    }

#if ENBL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  NewOffset=%d\n", NewOffset);
#endif
  } // end while the inner and outer limits are not adjacent or...

  // always end with the current offset at the last-pass offset
  *pTestingOffset =  BSInnerLimitOffset;

  // clear the per-offset test status (the binary search has a shorter test duration)
  bssa_memset(pMemShmooData->pOffsetTestStat, 0, pMemShmooData->OffsetTestStatBfrSize);

#if ENBL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  memset(%p, 0, %u)\n",
    pMemShmooData->pOffsetTestStat, pMemShmooData->OffsetTestStatBfrSize);
#endif

  if (pMemShmooData->SetPointTestInBinarySearchState(SsaServicesHandle,
    pSystemInfo, pMemCfg, pMemPointTestCfg, pPointTestCfg, pPointTestData,
    FALSE, pTestStat)) {
    return 1;
  }

  pMemShmooData->ShmooState = StopConditionShmooState;

  return 0;
} // end function BinarySearch

/**
  This function implements the find-stop-condition state.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pMemShmooData      Pointer to memory shmoo test data.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in]       ShmooDir           Shmoo direction.
  @param[in]       OuterLimitOffset   Outer shmoo parameter offset not to test beyond.
  @param[in, out]  pTestingOffset     Pointer to shmoo parameter offset where we're testing.
  @param[in, out]  pCurrentOffset     Pointer to current shmoo parameter offset.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 FindStopCondition(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  IN SHMOO_DIRECTION ShmooDir,
  IN INT16 OuterLimitOffset,
  IN OUT INT16 *pTestingOffset,
  IN OUT INT16 *pCurrentOffset,
  OUT TEST_STATUS *pTestStat)
{
  UINT32 Idx;
  INT16 NewOffset, LastPassOffset;
  BOOLEAN AnyLaneFailed;
  BOOLEAN ShmooStopModesSatisfied;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.FindStopCondition(ShmooDir=%d OuterLimitOffset=%d TestingOffset=%d CurrentOffset=%d)\n",
    ShmooDir, OuterLimitOffset, *pTestingOffset, *pCurrentOffset);
#endif

  if (pMemShmooData->TestStatusLogLevel >= 3) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "\nSearching for stop condition\n");
  }

  // start at the testing offset
  NewOffset = *pTestingOffset;

  if (ShmooDir == HiDirection) {
    LastPassOffset = *pTestingOffset - pMemShmooCfg->StepSize;

    if (LastPassOffset < pMemShmooData->MinOffset) {
      LastPassOffset = pMemShmooData->MinOffset;
    }
  }
  else {
    LastPassOffset = *pTestingOffset + pMemShmooCfg->StepSize;

    if (LastPassOffset > pMemShmooData->MaxOffset) {
      LastPassOffset = pMemShmooData->MaxOffset;
    }
  }

  // run a point test at the testing offset
  if (RunPointTest(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
    pPointTestData, *pTestingOffset, pCurrentOffset, &AnyLaneFailed,
    pTestStat)) {
    return 1;
  }

  Idx = (*pTestingOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

#if ENBL_DEBUG_OFFSET_STAT_IDX
  if (ChkOffsetTestStatIdx(SsaServicesHandle, pMemShmooData, __LINE__, Idx,
    pTestStat)) {
    return 1;
  }
#endif // ENBL_DEBUG_OFFSET_STAT_IDX

  pMemShmooData->pOffsetTestStat[Idx] = (UINT8) \
    (AnyLaneFailed) ? ErrsDetected : NoErrsDetected;

  if (!AnyLaneFailed) {
    LastPassOffset = *pTestingOffset;
  }
#if ENBL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  set Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
    *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif

  ShmooStopModesSatisfied = FALSE;
  // WHILE we have not satisfied the shmoo stop modes:
  while (!ShmooStopModesSatisfied) {
    if (ChkShmooStopModesSatisfied(SsaServicesHandle,
      pSystemInfo, pMemCfg, pMemShmooCfg, pMemPointTestCfg, pPointTestCfg,
      pMemShmooData, pPointTestData, &ShmooStopModesSatisfied, pCurrentOffset, pTestStat)) {
      return 1;
    }

    if (ShmooStopModesSatisfied) {
      break;
    }

      // IF we're at the outer limit THEN we're done
    if (*pTestingOffset == OuterLimitOffset) {
      break;
    }

    // move away from nominal
    if (ShmooDir == HiDirection) {
      NewOffset += pMemShmooCfg->StepSize;
    }
    else {
      NewOffset -= pMemShmooCfg->StepSize;
    }

    *pTestingOffset = NewOffset;

    // run a point test at the testing offset
    if (RunPointTest(SsaServicesHandle, pSystemInfo, pMemCfg,
      pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
      pPointTestData, *pTestingOffset, pCurrentOffset, &AnyLaneFailed,
      pTestStat)) {
      return 1;
    }

    Idx = (*pTestingOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

#if ENBL_DEBUG_OFFSET_STAT_IDX
    if (ChkOffsetTestStatIdx(SsaServicesHandle, pMemShmooData, __LINE__, Idx,
      pTestStat)) {
      return 1;
    }
#endif // ENBL_DEBUG_OFFSET_STAT_IDX

    pMemShmooData->pOffsetTestStat[Idx] = (UINT8) \
      (AnyLaneFailed) ? ErrsDetected : NoErrsDetected;

    if (!AnyLaneFailed) {
      LastPassOffset = *pTestingOffset;
    }
#if ENBL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  set Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
      *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif
  } // end while we have not satisfied the shmoo stop modes

  // set the testing offset to the last pass offset
  *pTestingOffset = LastPassOffset;

  pMemShmooData->ShmooState = LastPassShmooState;

  return 0;
} // end function FindStopCondition

/**
  This function implements the find-last-pass state.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pMemShmooData      Pointer to memory shmoo test data.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in]       ShmooDir           Shmoo direction.
  @param[in]       InnerLimitOffset   Inner shmoo parameter offset not to test beyond.
  @param[in, out]  pConsecutiveAllPassCnt  Pointer to count of consecutive all-pass offsets.
  @param[in, out]  pTestingOffset     Pointer to shmoo parameter offset where we're testing.
  @param[in, out]  pCurrentOffset     Pointer to current shmoo parameter offset.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 FindLastPass(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  IN SHMOO_DIRECTION ShmooDir,
  IN INT16 InnerLimitOffset,
  IN OUT UINT8 *pConsecutiveAllPassCnt,
  IN OUT INT16 *pTestingOffset,
  IN OUT INT16 *pCurrentOffset,
  OUT TEST_STATUS *pTestStat)
{
  UINT32 Idx;
  OFFSET_TEST_STAT OffsetTestStat;
  BOOLEAN AnyLaneFailed;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.FindLastPass(ShmooDir=%d InnerLimitOffset=%d CurrentOffset=%d)\n",
    ShmooDir, InnerLimitOffset, *pCurrentOffset);
#endif

  if (pMemShmooData->TestStatusLogLevel >= 3) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "\nSearching for last pass offset\n");
  }

  // pre-set for case where we find no last-pass
  pMemShmooData->ShmooState = DoneShmooState;

  Idx = (*pTestingOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

  OffsetTestStat = (OFFSET_TEST_STAT) pMemShmooData->pOffsetTestStat[Idx];

#if ENBL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  get Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
    *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif

  // IF no errors were detected at the last offset tested during the
  // check-stop-modes-satisfied state
  if (OffsetTestStat == (UINT8) NoErrsDetected) {
    *pConsecutiveAllPassCnt = 1;
    pMemShmooData->ShmooState = GuardBandShmooState;
  }
  else
  {
    // WHILE we haven't tested at the inner limiting offset
    while (*pTestingOffset != InnerLimitOffset) {
      // IF the current offset was not already tested
      if (OffsetTestStat == NotTested) {
        // run a point test at the testing offset
        if (RunPointTest(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
          pPointTestData, *pTestingOffset, pCurrentOffset, &AnyLaneFailed,
          pTestStat)) {
          return 1;
        }

#if ENBL_DEBUG_OFFSET_STAT_IDX
        if (ChkOffsetTestStatIdx(SsaServicesHandle, pMemShmooData, __LINE__, Idx,
          pTestStat)) {
          return 1;
        }
#endif // ENBL_DEBUG_OFFSET_STAT_IDX

        pMemShmooData->pOffsetTestStat[Idx] = (UINT8) \
          (AnyLaneFailed) ? ErrsDetected : NoErrsDetected;

#if ENBL_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  set Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
          *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif
      }
      else {
        // get the system any-lane-failed status
        AnyLaneFailed = (OffsetTestStat == ErrsDetected) ? TRUE : FALSE;
      }

      // IF no errors were found on any channel THEN we're done
      if (!AnyLaneFailed) {
        *pConsecutiveAllPassCnt = 1;
        pMemShmooData->ShmooState = GuardBandShmooState;
        break;
      }

      // move towards nominal
      if (ShmooDir == HiDirection) {
        // IF there's no more room THEN we're done
        if ((*pTestingOffset - pMemShmooCfg->StepSize) < InnerLimitOffset) {
          break;
        }

        *pTestingOffset -= pMemShmooCfg->StepSize;
      }
      else {
        // IF there's no more room THEN we're done
        if ((*pTestingOffset + pMemShmooCfg->StepSize) > InnerLimitOffset) {
          break;
        }

        *pTestingOffset += pMemShmooCfg->StepSize;
      }

      Idx = (*pTestingOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

      OffsetTestStat = (OFFSET_TEST_STAT) pMemShmooData->pOffsetTestStat[Idx];

#if ENBL_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  get Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
        *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif
    } // end while we haven't tested at the inner limiting offset
  } // end else errors were detected at the last offset tested during the
    // check-stop-modes-satisfied state

  return 0;
} // end function FindLastPass

/**
  This function implements the check-guard-band state.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pMemShmooData      Pointer to memory shmoo test data.
  @param[in, out]  pPointTestData     Pointer to specific memory point test data structure.
  @param[in]       ShmooDir           Shmoo direction.
  @param[in]       InnerLimitOffset   Inner shmoo parameter offset not to test beyond.
  @param[in, out]  pConsecutiveAllPassCnt  Pointer to count of consecutive all-pass offsets.
  @param[in, out]  pTestingOffset     Pointer to shmoo parameter offset where we're testing.
  @param[in, out]  pCurrentOffset     Pointer to current shmoo parameter offset.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
static UINT8 ChkGuardBand(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_SHMOO_CONFIG *pMemShmooCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN CONST VOID *pPointTestCfg,
  IN OUT MEM_SHMOO_TEST_DATA *pMemShmooData,
  IN OUT VOID *pPointTestData,
  IN SHMOO_DIRECTION ShmooDir,
  IN INT16 InnerLimitOffset,
  IN OUT UINT8 *pConsecutiveAllPassCnt,
  IN OUT INT16 *pTestingOffset,
  IN OUT INT16 *pCurrentOffset,
  OUT TEST_STATUS *pTestStat)
{
  UINT32 Idx;
  OFFSET_TEST_STAT OffsetTestStat;
  BOOLEAN AnyLaneFailed;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "MemShmoo.ChkGuardBand(ShmooDir=%d InnerLimitOffset=%d CurrentOffset=%d)\n",
    ShmooDir, InnerLimitOffset, *pCurrentOffset);
#endif

  if (pMemShmooData->TestStatusLogLevel >= 3) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "\nChecking guard band\n");
  }

  // WHILE we haven't tested at the inner limiting offset
  while (*pTestingOffset != InnerLimitOffset) {
    // IF the guard band size has been met or exceeded THEN we're done
    if (*pConsecutiveAllPassCnt > pMemShmooCfg->GuardBandSize) {
      break;
    }

    // move towards nominal
    if (ShmooDir == HiDirection) {
      // IF there's no more room THEN we're done
      if ((*pTestingOffset - pMemShmooCfg->StepSize) < InnerLimitOffset) {
        break;
      }

      *pTestingOffset -= pMemShmooCfg->StepSize;
    }
    else {
      // IF there's no more room THEN we're done
      if ((*pTestingOffset + pMemShmooCfg->StepSize) > InnerLimitOffset) {
        break;
      }

      *pTestingOffset += pMemShmooCfg->StepSize;
    }

    Idx = (*pTestingOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

    OffsetTestStat = (OFFSET_TEST_STAT) pMemShmooData->pOffsetTestStat[Idx];

#if ENBL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  get Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
      *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif
    // IF the current offset was already tested
    if (OffsetTestStat != NotTested) {
      // get the system any-lane-failed status
      AnyLaneFailed = (OffsetTestStat == ErrsDetected) ? TRUE : FALSE;
    }
    else {
      // run a point test at the testing offset
      if (RunPointTest(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, *pTestingOffset, pCurrentOffset, &AnyLaneFailed,
        pTestStat)) {
        return 1;
      }

#if ENBL_DEBUG_OFFSET_STAT_IDX
      if (ChkOffsetTestStatIdx(SsaServicesHandle, pMemShmooData, __LINE__, Idx,
        pTestStat)) {
        return 1;
      }
#endif // ENBL_DEBUG_OFFSET_STAT_IDX

      pMemShmooData->pOffsetTestStat[Idx] = (UINT8) \
        (AnyLaneFailed) ? ErrsDetected : NoErrsDetected;

#if ENBL_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  set Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
        *pTestingOffset, Idx, pMemShmooData->pOffsetTestStat[Idx], __LINE__);
#endif
    } // end else the current offset was not already tested

    // IF errors were found on any channel
    if (AnyLaneFailed) {
      // reset the counter
      *pConsecutiveAllPassCnt = 0;
    }
    else {
      *pConsecutiveAllPassCnt++;
    }
  } // end while we haven't tested at the inner limiting offset

  pMemShmooData->ShmooState = DoneShmooState;

  return 0;
} // end function ChkGuardBand

/**
  This function is used to set up the memory shmoo test.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       GetPointTestDimmRanks  Pointer to memory point test function for getting the DIMM/rank pairs for a channel.
  @param[in]       SetPointTestInBinarySearchState  Pointer to specific memory point test function for setting in-binary-search-state.
  @param[in]       RunPointTest       Pointer to specific memory point test function to run point test.
  @param[in]       MarginParamChgHandler  Pointer to client's result handler function.
  @param[in]       PreDimmInitHandler  Pointer to client's pre-DIMM-initialization handler function.
  @param[in]       PostDimmInitHandler  Pointer to client's post-DIMM-initialization handler function.
  @param[in]       PreRunPointTestHandler  Pointer to client's pre-RunPointTest handler function.
  @param[in]       pClientTestCfg     Pointer to client test configuration.
  @param[in, out]  pClientTestData    Pointer to client test data.
  @param[in]       TestStatusLogLevel  Test status logging level.
  @param[out]      ppVoidMemShmooData  Pointer to pointer to memory shmoo test data
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

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
  OUT VOID **ppVoidMemShmooData,
  OUT TEST_STATUS *pTestStat)
{
  MEM_SHMOO_TEST_DATA *pMemShmooData;
  UINT8 Socket;
  SSA_STATUS RetStat;
  SETUP_CLEANUP SetupCleanup;
#if SUPPORT_FOR_DDRT
  UINT8 Controller, Channel;
  FNV_TRAINING_MODE TrainingMode;
  BOOLEAN EnableTraining = TRUE;
#endif // SUPPORT_FOR_DDRT

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "SetupMemShmoo(TestStatusLogLevel=%u)\n", TestStatusLogLevel);
#endif

#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
  GetMemPoolSize(SsaServicesHandle);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  Malloc(Size=%u) for memory shmoo test data\n", sizeof(*pMemShmooData));
#endif
  *ppVoidMemShmooData = SsaServicesHandle->SsaCommonConfig->Malloc(
    SsaServicesHandle, sizeof(*pMemShmooData));
  if (*ppVoidMemShmooData == NULL) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: Could not allocate memory for memory shmoo test status\n");
#endif
    ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
    return 1;
  }
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    pBfr=%p\n", *ppVoidMemShmooData);
  GetMemPoolSize(SsaServicesHandle);
#endif

  pMemShmooData = *((MEM_SHMOO_TEST_DATA**) ppVoidMemShmooData);

  bssa_memset(pMemShmooData, 0, sizeof(*pMemShmooData));

  pMemShmooData->TestStatusLogLevel = TestStatusLogLevel;
  pMemShmooData->GetPointTestDimmRanks = GetPointTestDimmRanks;
  pMemShmooData->SetPointTestInBinarySearchState = SetPointTestInBinarySearchState;
  pMemShmooData->RunPointTest = RunPointTest;
  pMemShmooData->MarginParamChgHandler = MarginParamChgHandler;
  pMemShmooData->PreDimmInitHandler = PreDimmInitHandler;
  pMemShmooData->PostDimmInitHandler = PostDimmInitHandler;
  pMemShmooData->PreRunPointTestHandler = PreRunPointTestHandler;
  pMemShmooData->pClientTestCfg = pClientTestCfg;
  pMemShmooData->pClientTestData = pClientTestData;

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_FIXUP_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  GetMarginParamSpecificity(IoLevel=%u MarginGroup=%u)\n",
    pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup);
#endif
  RetStat = SsaServicesHandle->SsaMemoryConfig->GetMarginParamSpecificity(
    SsaServicesHandle, pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup,
    &pMemShmooData->Specificity);
  if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
    return 1;
  }
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_FIXUP_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "    Specificity=0x%X (",
    pMemShmooData->Specificity);
  LogSpecificity(SsaServicesHandle, pMemShmooData->Specificity);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, ")\n");
#endif

  GetChnlInfo(SsaServicesHandle, pSystemInfo, pMemCfg, pMemShmooCfg,
    pMemPointTestCfg, pMemShmooData, pTestStat);

#if SUPPORT_FOR_DDRT
  // IF we're testing DDR devices
  if (pMemShmooCfg->IsDdrT) {
    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // IF the margin parameter is socket specific
      if (pMemShmooData->Specificity & SocketSpecific) {
        // IF the socket is not enabled for validation THEN skip it
        if (!IsSocketValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }
      // ELSE the margin parameter is not socket specific
      else {
        // IF the socket is not enabled for testing THEN skip it
        if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }

      // IF the margin parameter is command timing or voltage or Early Read ID timing or voltage
      if ((pMemShmooCfg->MarginGroup == CmdAll) ||
        (pMemShmooCfg->MarginGroup == CmdVref) ||
        (pMemShmooCfg->MarginGroup == EridDelay) ||
        (pMemShmooCfg->MarginGroup == EridVref)) {
        // IF the margin parameter is command timing or voltage
        if ((pMemShmooCfg->MarginGroup == CmdAll) ||
          (pMemShmooCfg->MarginGroup == CmdVref)) {
          TrainingMode = FnvTrainModeLateCmdCk;
        }
        // ELSE the margin parameter is Early Read ID timing or voltage
        else {
          TrainingMode = FnvTrainModeEarlyRidCoarse;
        }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetAepDimmTrainingMode(Socket=%u Mode=%u)\n",
          Socket, TrainingMode);
#endif
        SsaServicesHandle->SsaMemoryDdrtConfig->SetAepDimmTrainingMode(
          SsaServicesHandle, Socket, TrainingMode);
      } // end if the margin parameter is command timing or voltage or ...

      // IF the margin parameter is Early Read ID timing or voltage
      if ((pMemShmooCfg->MarginGroup == EridDelay) ||
        (pMemShmooCfg->MarginGroup == EridVref)) {
        // FOR each controller:
        for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
          pMemShmooCfg->ChannelValCompletionBitmasks[Socket][Controller] = 0;
          // FOR each channel:
          for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
            // IF the margin parameter is channel specific
            if (pMemShmooData->Specificity & ChannelSpecific) {
              // IF the controller is not enabled for validation THEN skip it
              if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
                pMemPointTestCfg, Socket, Controller, Channel)) {
                continue;
              }
            }
            // ELSE the margin parameter is not channel specific
            else {
              // IF the controller is not enabled for testing THEN skip it
              if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo,
                pMemCfg, pMemPointTestCfg, Socket, Controller, Channel)) {
                continue;
              }
            }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetEridTrainingConfig(Socket=%u Controller=%u Channel=%u EnableTraining=%u)\n",
              Socket, Controller, Channel, EnableTraining);
#endif
            SsaServicesHandle->SsaMemoryDdrtConfig->SetEridTrainingConfig(
              SsaServicesHandle, Socket, Controller, Channel, EnableTraining);
          } // end for each channel
        } // end for each controller
      } // end if the margin parameter is Early Read ID timing or voltage
    } // end for each socket
  } // end if we're testing DDR devices
#endif // SUPPORT_FOR_DDRT

  // setup the margin parameter
  // IF calling the InitMarginParam is not disabled
  if (!pMemShmooCfg->DisableInitMarginParam) {
    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // IF the margin parameter is socket specific
      if (pMemShmooData->Specificity & SocketSpecific) {
        // IF the socket is not enabled for validation THEN skip it
        if (!IsSocketValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }
      // ELSE the margin parameter is not socket specific
      else {
        // IF the socket is not enabled for testing THEN skip it
        if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }

      SetupCleanup = Setup;
#if SUPPORT_FOR_RDIMM || SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT
      // NOTE: Backside command margining setup/cleanup is different for high
      // and low sides so we're piggybacking the shmoo direction in the
      // Setup/Cleanup parameter.
      if ((pMemShmooCfg->IoLevel == LrbufLevel) &&
        ((pMemShmooCfg->MarginGroup == CmdAll) ||
        (pMemShmooCfg->MarginGroup == CmdVref) ||
        (pMemShmooCfg->MarginGroup == CtlAll))) {
        SetupCleanup |= pMemShmooCfg->ShmooDirection << 1;
      }
#endif // SUPPORT_FOR_RDIMM || SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  InitMarginParam(Socket=%u IoLevel=%u MarginGroup=%u SetupCleanup=%u)\n",
        Socket, pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup, SetupCleanup);
#endif
      RetStat = SsaServicesHandle->SsaMemoryConfig->InitMarginParam(
        SsaServicesHandle, Socket, pMemShmooCfg->IoLevel,
        pMemShmooCfg->MarginGroup, SetupCleanup);
      if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
        return 1;
      }
    } // end for each socket
  } // end if calling the InitMarginParam is not disabled

  if (GetMarginParamInfo(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemShmooCfg, pMemPointTestCfg, pMemShmooData, pTestStat)) {
    return 1;
  }

  pMemShmooData->OffsetTestStatBfrSize = \
    (((pMemShmooData->MaxOffset - pMemShmooData->MinOffset) /
    pMemShmooCfg->StepSize) + 1) * sizeof(UINT8);

#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
  GetMemPoolSize(SsaServicesHandle);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  Malloc(Size=%u) for offset test status\n",
    pMemShmooData->OffsetTestStatBfrSize);
#endif
  pMemShmooData->pOffsetTestStat = SsaServicesHandle->SsaCommonConfig->Malloc(
    SsaServicesHandle, pMemShmooData->OffsetTestStatBfrSize);
  if (pMemShmooData->pOffsetTestStat == NULL) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: Could not allocate memory for offset test status\n");
#endif
    ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
    return 1;
  }
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    pBfr=%p\n", pMemShmooData->pOffsetTestStat);
  GetMemPoolSize(SsaServicesHandle);
#endif

#if ENBL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  memset(%p, 0, %u)\n",
    pMemShmooData->pOffsetTestStat, pMemShmooData->OffsetTestStatBfrSize);
#endif

  bssa_memset(pMemShmooData->pOffsetTestStat, 0, pMemShmooData->OffsetTestStatBfrSize);

  pMemShmooData->LaneFailStatsBfrSize = sizeof(LANE_FAIL_STATS);
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
  GetMemPoolSize(SsaServicesHandle);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  Malloc(Size=%u) for lane failure status\n",
    pMemShmooData->LaneFailStatsBfrSize);
#endif
  pMemShmooData->pLaneFailStats = \
    (LANE_FAIL_STATS *) SsaServicesHandle->SsaCommonConfig->Malloc(
    SsaServicesHandle, pMemShmooData->LaneFailStatsBfrSize);
  if (pMemShmooData->pLaneFailStats == NULL) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: Could not allocate memory for lane failure status\n");
#endif
    ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
    return 1;
  }
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "    pBfr=%p\n", pMemShmooData->pLaneFailStats);
  GetMemPoolSize(SsaServicesHandle);
#endif

  return 0;
} // end function SetupMemShmoo

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
  IN UINT8 Channel)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  UINT8 Lane;
  UINT8 Offset;
  UINT8 SmallestOffset = 0xFF; // initialize to largest possible value

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // find the MinOffsets value with the smallest magnitude
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // FOR each lane:
  for (Lane = 0; Lane < pSystemInfo->BusWidth; Lane++) {
    Offset = pMemShmooData->MinOffsets[Socket][Controller][Channel][Lane];

    // IF this offset is inside the current minimum value
    if (SmallestOffset > Offset) {
      SmallestOffset = Offset;
    }
  } // end for each lane

  return ((INT16) SmallestOffset) * -1;
} // end function GetMemShmooMaxOfMinOffset

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
  IN UINT8 Channel)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  UINT8 Lane;
  UINT8 Offset;
  UINT8 SmallestOffset = 0xFF; // initialize to largest possible value

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // find the MaxOffsets value with the smallest magnitude
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // FOR each lane:
  for (Lane = 0; Lane < pSystemInfo->BusWidth; Lane++) {
    Offset = pMemShmooData->MaxOffsets[Socket][Controller][Channel][Lane];

    // IF this offset is inside the current minimum value
    if (SmallestOffset > Offset) {
      SmallestOffset = Offset;
    }
  } // end for each lane

  return (INT16) SmallestOffset;
} // end function GetMemShmooMinOfMaxOffset
#endif // SUPPORT_FOR_EXTENDED_RANGE

/**
  This function is used to get the number of active channels from the given
  shmoo test data.

  @param[in]  pVoidMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Number of active channels.
**/
UINT8 GetMemShmooActiveChnlCnt(
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->ActiveChnlCnt;
}

/**
  This function is used to get the minimum offset from the given shmoo test
  data.

  @param[in]  pVoidMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Minimum shmoo parameter offset.
**/
INT16 GetMemShmooMinOffset(
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->MinOffset;
}

/**
  This function is used to get the maximum offset from the given shmoo test
  data.

  @param[in]  pVoidMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Maximum shmoo parameter offset.
**/
INT16 GetMemShmooMaxOffset(
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->MaxOffset;
} // end function GetMaxOffset

/**
  This function is used to get the shmoo parameter step unit from the
  given shmoo test data.

  @param[in]  pVoidMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter step unit.
**/
UINT16 GetMemShmooStepUnit(
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->StepUnit;
} // end function GetMemShmooStepUnit

/**
  This function is used to get the shmoo parameter settling delay from the
  given shmoo test data.

  @param[in]  pVoidMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter settling delay.
**/
UINT16 GetMemShmooSettleDelay(
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->SettleDelay;
}

/**
  This function is used to get the minimum shmoo parameter offset from the
  given shmoo test data per memory technology.

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Minimum shmoo parameter offset.
**/
INT16 GetMemShmooMinOffsetByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->MinOffsetByMemoryTech[IsDdrt];
}

/**
  This function is used to get the maximum shmoo parameter offset from the
  given shmoo test data per memory technology.

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Maximum shmoo parameter offset.
**/
INT16 GetMemShmooMaxOffsetByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->MaxOffsetByMemoryTech[IsDdrt];
} // end function GetMaxOffsetByMemoryTech


/**
  This function is used to get the shmoo parameter step unit from the
  given shmoo test data per memory technology.

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]  pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter step unit.
**/
UINT16 GetMemShmooStepUnitByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->StepUnitByMemoryTech[IsDdrt];
} // end function GetMemShmooStepUnitByMemoryTech

/**
  This function is used to get the shmoo parameter settling delay from the
  given shmoo test data per memory technology

  @param[in]   IsDdrt  TRUE: DDRT, FALSE: DDR4
  @param[in]   pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Shmoo parameter settling delay.
**/
UINT16 GetMemShmooSettleDelayByMemoryTech(
  IN CONST BOOLEAN IsDdrt,
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

  return pMemShmooData->SettleDelayByMemoryTech[IsDdrt];
}
/**
  This function is used to get the flag indicating whether we're in the binary
  search state.

  @param[in]   pMemShmooData  Pointer to memory shmoo test data structure.

  @retval  Flag indicating whether we're in the binary search state.
**/
BOOLEAN GetInBinarySearchState(
  IN CONST VOID *pVoidMemShmooData)
{
  CONST MEM_SHMOO_TEST_DATA *pMemShmooData = \
    (CONST MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  BOOLEAN InBinarySearchState;

  if (pMemShmooData->ShmooState == BinarySearchShmooState) {
    InBinarySearchState = TRUE;
  }
  else {
    InBinarySearchState = FALSE;
  }

  return InBinarySearchState;
} // end function GetInBinarySearchState

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
  IN INT16 MaxOffset)
{
  UINT32 MemSize;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "GetMemShmooDynamicMemSize(MinOffset=%d MaxOffset=%d)\n",
    MinOffset, MaxOffset);
#endif

  // for "pMemShmooData"
  MemSize = GetActualMallocSize(sizeof(MEM_SHMOO_TEST_DATA));

  // for "pMemShmooData->pLaneFailStats"
  MemSize += GetActualMallocSize(sizeof(LANE_FAIL_STATS));

  // for "pMemShmooData->pOffsetTestStat"
  MemSize += GetActualMallocSize((((MaxOffset - MinOffset) /
    pMemShmooCfg->StepSize) + 1) * sizeof(UINT8));

  // a bit more for good measure
  MemSize += 100;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "  MemSize=%u\n", MemSize);
#endif

  return MemSize;
} // end function GetMemShmooDynamicMemSize

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
  OUT TEST_STATUS *pTestStat)
{
  MEM_SHMOO_TEST_DATA *pMemShmooData = (MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;
  INT16 OuterLimitOffset;
  INT16 TmpOffset, NewOffset;
  OFFSET_TEST_STAT OffsetTestStat;
  UINT8 ConsecutiveAllPassCnt = 0;
  UINT32 Idx;
  UINT8 Socket, Controller;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "FindEdge(ShmooDir=%u InnerLimitOffset=%d TestingOffset=%d CurrentOffset=%d)\n",
    ShmooDir, InnerLimitOffset, *pTestingOffset, *pCurrentOffset);
#endif

  *pLastPassFound = FALSE;
  *pLastPassOffset = 0;
  *pFirstFailFound = FALSE;
  *pFirstFailOffset = 0;

  // clear the per-offset test status
  bssa_memset(pMemShmooData->pOffsetTestStat, 0, pMemShmooData->OffsetTestStatBfrSize);

#if ENBL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  memset(%p, 0, %u)\n",
    pMemShmooData->pOffsetTestStat, pMemShmooData->OffsetTestStatBfrSize);
#endif

  // the outer limit depends on the shmoo direction
  OuterLimitOffset = (ShmooDir == HiDirection) ? pMemShmooData->MaxOffset : pMemShmooData->MinOffset;
  // align the outer limit to the step size
  OuterLimitOffset /=  pMemShmooCfg->StepSize;
  OuterLimitOffset *=  pMemShmooCfg->StepSize;

  if (pMemShmooCfg->EnableOptimizedShmoo) {
    pMemShmooData->ShmooState = BinarySearchShmooState;
  }
  else {
    pMemShmooData->ShmooState = StopConditionShmooState;
  }

  // WHILE there's more shmoo states to do:
  while (pMemShmooData->ShmooState != DoneShmooState) {
#if ENBL_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  ShmooState=%u\n", pMemShmooData->ShmooState);
#endif

    switch (pMemShmooData->ShmooState) {
    case BinarySearchShmooState:
      if (BinarySearch(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, InnerLimitOffset, OuterLimitOffset, pTestingOffset,
        pCurrentOffset, pTestStat)) {
        return 1;
      }
      break;
    case StopConditionShmooState:
      if (FindStopCondition(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, ShmooDir, OuterLimitOffset, pTestingOffset,
        pCurrentOffset, pTestStat)) {
        return 1;
      }
      break;
    case LastPassShmooState:
      if (FindLastPass(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, ShmooDir, InnerLimitOffset, &ConsecutiveAllPassCnt,
        pTestingOffset, pCurrentOffset, pTestStat)) {
        return 1;
      }
      break;
    case GuardBandShmooState:
      if (ChkGuardBand(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, ShmooDir, InnerLimitOffset, &ConsecutiveAllPassCnt,
        pTestingOffset, pCurrentOffset, pTestStat)) {
        return 1;
      }
      break;
    default:
      ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
      return 1;
    }
  } // end while there's more shmoo states to do

  if (ShmooDir == HiDirection) {
    for (TmpOffset = InnerLimitOffset;
      TmpOffset <= OuterLimitOffset;
      TmpOffset += pMemShmooCfg->StepSize) {
      Idx = (TmpOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

      OffsetTestStat = (OFFSET_TEST_STAT) pMemShmooData->pOffsetTestStat[Idx];

#if ENBL_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  get Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
        TmpOffset, Idx, OffsetTestStat, __LINE__);
#endif

      if (OffsetTestStat == NoErrsDetected) {
        *pLastPassOffset = TmpOffset;
        *pLastPassFound = TRUE;
      }
      else if (OffsetTestStat == ErrsDetected) {
        *pFirstFailOffset = TmpOffset;
        *pFirstFailFound = TRUE;
        break;
      }
    }
  }
  else {
    for (TmpOffset = InnerLimitOffset;
      TmpOffset >= OuterLimitOffset;
      TmpOffset -= pMemShmooCfg->StepSize) {
      Idx = (TmpOffset - pMemShmooData->MinOffset) / pMemShmooCfg->StepSize;

      OffsetTestStat = (OFFSET_TEST_STAT) pMemShmooData->pOffsetTestStat[Idx];

#if ENBL_DEBUG_LOG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  get Offset=%d Idx=%u OffsetTestStat=%u (Line=%d)\n",
        TmpOffset, Idx, OffsetTestStat, __LINE__);
#endif

      if (OffsetTestStat == NoErrsDetected) {
        *pLastPassOffset = TmpOffset;
        *pLastPassFound = TRUE;
      }
      else if (OffsetTestStat == ErrsDetected) {
        *pFirstFailOffset = TmpOffset;
        *pFirstFailFound = TRUE;
        break;
      }
    }
  }


  // restore all the remaining Margin param offset to nominal
  NewOffset = 0;
  if (SetMarginParam(SsaServicesHandle, pSystemInfo, pMemCfg, pMemShmooCfg,
    pMemPointTestCfg, pMemShmooData, pPointTestData,
    pCurrentOffset, NewOffset, pMemShmooCfg->EnableIncrementalStepping,
    pTestStat)) {
    return 1;
  }

  // restore the channel validation mask if the chnl validation ChannelValCompletionBitmasks
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) { 
      pMemPointTestCfg->ChannelValBitmasks[Socket][Controller] |= pMemShmooCfg->ChannelValCompletionBitmasks[Socket][Controller];
    }
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  LastPassFound=%u LastPassOffset=%d FirstFailFound=%u FirstFailOffset=%d\n",
    *pLastPassFound, *pLastPassOffset, *pFirstFailFound, *pFirstFailOffset);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  TestingOffset=%d CurrentOffset=%d)\n",
    *pTestingOffset, *pCurrentOffset);
#endif

  return 0;
} // end function FindEdge

/**
  This function is used to run a margin 1D test.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemShmooCfg       Pointer to memory shmoo test configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       pPointTestCfg      Pointer to specific memory point test configuration structure.
  @param[in, out]  pVoidMemShmooData  Pointer to memory shmoo test data structure.
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
  IN OUT VOID *pVoidMemShmooData,
  IN OUT VOID *pPointTestData,
  OUT TEST_STATUS *pTestStat)
{
  SHMOO_DIRECTION ShmooDirs[2];
  UINT8 ShmooDirCnt;
  UINT8 ShmooDirIdx;
  INT16 TestingOffset = 0;
  INT16 CurrentOffset = 0;
  INT16 InnerLimitOffset;
  BOOLEAN HiSideLastPassFound = FALSE;
  INT16 HiSideLastPassOffset = 0;
  BOOLEAN LoSideLastPassFound = FALSE;
  INT16 LoSideLastPassOffset = 0;
  BOOLEAN HiSideFirstFailFound = FALSE;
  INT16 HiSideFirstFailOffset = 0;
  BOOLEAN LoSideFirstFailFound = FALSE;
  INT16 LoSideFirstFailOffset = 0;
  MEM_SHMOO_TEST_DATA *pMemShmooData = (MEM_SHMOO_TEST_DATA*) pVoidMemShmooData;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "RunMargin1D()\n");
#endif

  if (pMemShmooData->TestStatusLogLevel >= 4) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "MinOffset=%d MaxOffset=%d\n",
      pMemShmooData->MinOffset, pMemShmooData->MaxOffset);
  }

  // ------------------------------------------------------------------------
  // get the ordered list of directions to test
  switch (pMemShmooCfg->ShmooDirection) {
  case HiDirection:
    ShmooDirs[0] = HiDirection;
    ShmooDirCnt = 1;
    break;
  case LoDirection:
    ShmooDirs[0] = LoDirection;
    ShmooDirCnt = 1;
    break;
  default:
    ShmooDirs[0] = HiDirection;
    ShmooDirs[1] = LoDirection;
    ShmooDirCnt = 2;
    break;
  case LoThenHiDirection:
    ShmooDirs[0] = LoDirection;
    ShmooDirs[1] = HiDirection;
    ShmooDirCnt = 2;
    break;
  }

  // ------------------------------------------------------------------------
  // FOR all shmoo directions:
  for (ShmooDirIdx = 0; ShmooDirIdx < ShmooDirCnt; ShmooDirIdx++) {
    SHMOO_DIRECTION ShmooDir = ShmooDirs[ShmooDirIdx];

    if (ShmooDir == HiDirection) {
      InnerLimitOffset = 0;
    }
    else {
      InnerLimitOffset = pMemShmooCfg->StepSize * -1;

      // take care of case where the minimum offset is very small or zero
      if (InnerLimitOffset < pMemShmooData->MinOffset) {
        InnerLimitOffset = 0;
      }
    }

    // IF the optimized shmoo isn't enabled and we're not testing at the
    // inner limit offset
    if ((!pMemShmooCfg->EnableOptimizedShmoo) &&
      (InnerLimitOffset != TestingOffset)) {
      // start at the inner limit offset
      TestingOffset = InnerLimitOffset;
    }

    if (ShmooDir == HiDirection) {
      if (FindEdge(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, ShmooDir, InnerLimitOffset, &TestingOffset,
        &CurrentOffset, &HiSideLastPassFound, &HiSideLastPassOffset,
        &HiSideFirstFailFound, &HiSideFirstFailOffset, pTestStat)) {
        return 1;
      }
    }
    else {
      if (FindEdge(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemShmooCfg, pMemPointTestCfg, pPointTestCfg, pMemShmooData,
        pPointTestData, ShmooDir, InnerLimitOffset, &TestingOffset,
        &CurrentOffset, &LoSideLastPassFound, &LoSideLastPassOffset,
        &LoSideFirstFailFound, &LoSideFirstFailOffset, pTestStat)) {
        return 1;
      }
    }
  } // end for all shmoo directions

  if (pMemShmooData->TestStatusLogLevel >= 2) {
    // IF we tested the high side
    if (pMemShmooCfg->ShmooDirection != LoDirection) {
      if (HiSideLastPassFound) {
        if (HiSideFirstFailFound) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "Hi side last pass offset = %d\n", HiSideLastPassOffset);
        }
        else {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "Hi side last pass offset = %d (first fail not found)\n",
            HiSideLastPassOffset);
        }
      }
      else {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "Hi side last pass offset = not found\n");
      }
    } // end if we tested the high side

    // IF we tested the low side
    if (pMemShmooCfg->ShmooDirection != HiDirection) {
      if (LoSideLastPassFound) {
        if (LoSideFirstFailFound) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "Lo side last pass offset = %d\n", LoSideLastPassOffset);
        }
        else {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "Lo side last pass offset = %d (first fail not found)\n",
            LoSideLastPassOffset);
        }
      }
      else {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "Lo side last pass offset = not found\n");
      }
    } // end if we tested the low side
  } // pMemShmooData->LogTestStatus

  return 0;
} // end function RunMargin1D

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
  OUT TEST_STATUS *pTestStat)
{
  MEM_SHMOO_TEST_DATA *pMemShmooData = *((MEM_SHMOO_TEST_DATA**) ppMemShmooData);
  UINT8 Socket;
  SSA_STATUS RetStat;
  SETUP_CLEANUP SetupCleanup;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CleanupMemShmoo()\n");
#endif

  // cleanup the margin parameter
  // IF calling the InitMarginParam is not disabled
  if (!pMemShmooCfg->DisableInitMarginParam) {
    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // IF the margin parameter is socket specific
      if (pMemShmooData->Specificity & SocketSpecific) {
        // IF the socket is not enabled for validation THEN skip it
        if (!IsSocketValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }
      // ELSE the margin parameter is not socket specific
      else {
        // IF the socket is not enabled for testing THEN skip it
        if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }

      SetupCleanup = Cleanup;
#if SUPPORT_FOR_RDIMM || SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT
      // NOTE: Backside command margining setup/cleanup is different for high
      // and low sides so we're piggybacking the shmoo direction in the
      // Setup/Cleanup parameter.
      if ((pMemShmooCfg->IoLevel == LrbufLevel) &&
        ((pMemShmooCfg->MarginGroup == CmdAll) ||
        (pMemShmooCfg->MarginGroup == CmdVref) ||
        (pMemShmooCfg->MarginGroup == CtlAll))) {
        SetupCleanup |= pMemShmooCfg->ShmooDirection << 1;
      }
#endif // SUPPORT_FOR_RDIMM || SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  InitMarginParam(Socket=%u IoLevel=%u MarginGroup=%u SetupCleanup=%u)\n",
        Socket, pMemShmooCfg->IoLevel, pMemShmooCfg->MarginGroup, SetupCleanup);
#endif
      RetStat = SsaServicesHandle->SsaMemoryConfig->InitMarginParam(
        SsaServicesHandle, Socket, pMemShmooCfg->IoLevel,
        pMemShmooCfg->MarginGroup, SetupCleanup);
      if (ChkRetStat(SsaServicesHandle, RetStat, __FILE__, __LINE__, pTestStat)) {
        return 1;
      }
    } // end for each socket
  } // end if calling the InitMarginParam is not disabled

#if SUPPORT_FOR_DDRT
  // IF we're testing DDR-T devices
  if (pMemShmooCfg->IsDdrT) {
    // FOR each socket:
    for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
      // IF the margin parameter is socket specific
      if (pMemShmooData->Specificity & SocketSpecific) {
        // IF the socket is not enabled for validation THEN skip it
        if (!IsSocketValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }
      // ELSE the margin parameter is not socket specific
      else {
        // IF the socket is not enabled for testing THEN skip it
        if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
          pMemPointTestCfg, Socket)) {
          continue;
        }
      }

      // IF the margin parameter is command or Early Read ID
      // PT place the FNV training mode back to late cmd ck training mode which
      // is the leftover state after legacy RMT test.
      if ((pMemShmooCfg->MarginGroup == CmdAll) ||
        (pMemShmooCfg->MarginGroup == CmdVref) ||
        (pMemShmooCfg->MarginGroup == EridDelay) ||
        (pMemShmooCfg->MarginGroup == EridVref)) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetAepDimmTrainingMode(Socket=%u Mode=%u)\n",
          Socket, FnvTrainModeLateCmdCk);
#endif
        SsaServicesHandle->SsaMemoryDdrtConfig->SetAepDimmTrainingMode(
          SsaServicesHandle, Socket, FnvTrainModeLateCmdCk);
      }

      // IF the margin parameter is Early Read ID
      if ((pMemShmooCfg->MarginGroup == EridDelay) ||
        (pMemShmooCfg->MarginGroup == EridVref)) {
        UINT8 Controller, Channel;
        BOOLEAN EnableTraining = FALSE;

        // FOR each controller:
        for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
          // FOR each channel:
          for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
            // IF the margin parameter is channel specific
            if (pMemShmooData->Specificity & ChannelSpecific) {
              // IF the controller is not enabled for validation THEN skip it
              if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
                pMemPointTestCfg, Socket, Controller, Channel)) {
                continue;
              }
            }
            // ELSE the margin parameter is not channel specific
            else {
              // IF the controller is not enabled for testing THEN skip it
              if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo,
                pMemCfg, pMemPointTestCfg, Socket, Controller, Channel)) {
                continue;
              }
            }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetEridTrainingConfig(Socket=%u Controller=%u Channel=%u EnableTraining=%u)\n",
              Socket, Controller, Channel, EnableTraining);
#endif
            SsaServicesHandle->SsaMemoryDdrtConfig->SetEridTrainingConfig(
              SsaServicesHandle, Socket, Controller, Channel, EnableTraining);
          } // end for each channel
        } // end for each controller

        // FOR each controller:
        for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
          // IF the margin parameter is controller specific
          if (pMemShmooData->Specificity & ControllerSpecific) {
            // IF the controller is not enabled for validation THEN skip it
            if (!IsControllerValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
              pMemPointTestCfg, Socket, Controller)) {
              continue;
            }
          }
          // ELSE the margin parameter is not controller specific
          else {
            // IF the controller is not enabled for testing THEN skip it
            if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo,
              pMemCfg, pMemPointTestCfg, Socket, Controller)) {
              continue;
            }
          }

          // do an I/O reset for either DIMM initialization type
#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  IoReset(Socket=%u Controller=%u)\n",
            Socket, Controller);
#endif
          SsaServicesHandle->SsaMemoryConfig->IoReset(SsaServicesHandle,
            Socket, Controller);
        } // end for each controller
      } // end if the margin parameter is Early Read ID
    } // end for each socket
  } // end if we're testing DDR-T devices

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF the socket is not enabled for testing THEN skip it
    if (!IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
      pMemPointTestCfg, Socket)) {
      continue;
    }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS || ENBL_DDRT_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  ResetAepFifoTrain(Socket=%u)\n", Socket);
#endif
    SsaServicesHandle->SsaMemoryDdrtConfig->ResetAepFifoTrain(
      SsaServicesHandle, Socket);
  } // end for each socket
#endif // SUPPORT_FOR_DDRT

  if (pMemShmooData != NULL) {
    if (pMemShmooData->pLaneFailStats != NULL)
    {
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
      GetMemPoolSize(SsaServicesHandle);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  Free(pBfr=%p)\n", pMemShmooData->pLaneFailStats);
#endif
      SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle,
        pMemShmooData->pLaneFailStats);
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
      GetMemPoolSize(SsaServicesHandle);
#endif

      pMemShmooData->pLaneFailStats = NULL;
    }

    if (pMemShmooData->pOffsetTestStat != NULL) {
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
      GetMemPoolSize(SsaServicesHandle);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  Free(pBfr=%p)\n", pMemShmooData->pOffsetTestStat);
#endif
      SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle,
        pMemShmooData->pOffsetTestStat);
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
      GetMemPoolSize(SsaServicesHandle);
#endif

      pMemShmooData->pOffsetTestStat = NULL;
    }

#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    GetMemPoolSize(SsaServicesHandle);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Free(pBfr=%p)\n", pMemShmooData);
#endif
    SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle,
      pMemShmooData);
#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
    GetMemPoolSize(SsaServicesHandle);
#endif

    *ppMemShmooData = NULL;
  }

  return 0;
} // end function CleanupMemShmoo

#endif  // SSA_FLAG

// end file MemShmoo.c
