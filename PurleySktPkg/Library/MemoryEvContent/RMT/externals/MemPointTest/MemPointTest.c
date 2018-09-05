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
  MemPointTest.c

@brief
  This file contains functions for the generic memory point test.
**/
#ifdef SSA_FLAG

#ifdef __STUB__
#include <assert.h>
#endif // __STUB__

#include "ssabios.h"
#include "bssalogging.h"
#include "BitMask.h"
#include "ChkRetStat.h"
#include "LogByteArrayAsHex.h"
#include "MemPointTest.h"

// flag to enable function trace debug messages
#define ENBL_FUNCTION_TRACE_DEBUG_MSGS (0)

// flag to enable verbose log messages
#define ENBL_VERBOSE_LOG_MSGS (0)

// flag to enable verbose error messages
#define ENBL_VERBOSE_ERROR_MSGS (0)

#define min(a, b) (((a) < (b)) ? (a) : (b))

/**
  This function is used to get normalized dimm and rank bit mask for the given channel. 
  The normalization is to combine the dimm and rank into one bit mask
  so that it can be much easier to iterate through when we count or 
  generate the rank combination.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in]       A_or_B             Single character indicating DIMM A or DIMM B.

  @retval  DIMM bitmask.
**/
UINT32 GetNormalizedDimmRankBitMask(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN char A_or_B)
{
  UINT8 DimmBitmask, RankBitmask;
  UINT32 NormalizedBitmask = 0;
  UINT8 Dimm;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "GetNormalizedDimmRankBitMask(Socket=%u Controller=%u Channel=%u A_or_B=%c)\n",
    Socket, Controller, Channel, A_or_B);
#endif

#ifdef __STUB__
  // confirm design assumption that all values will fit in packed 32-bit variable
  assert((MAX_DIMM_CNT * MAX_RANK_CNT) < 32);
#endif // __STUB__

  // start with the corresponding bitmask from the memory point test configuration
  if (A_or_B == 'A') {
    DimmBitmask = pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel];
  }
  else {
    DimmBitmask = pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel];
  }

  // factor in the bitmask from the physical memory configuration
  DimmBitmask &= pMemCfg->DimmBitmasks[Socket][Controller][Channel];

  for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++){
    if ((DimmBitmask & (0x1 << Dimm)) == 0) continue;

    // look at the available rank in the dimm
    // start with the corresponding bitmask from the memory point test configuration
    if (A_or_B == 'A') {
      RankBitmask = pMemPointTestCfg->RankABitmasks[Socket][Controller][Channel][Dimm];
    }
    else {
      RankBitmask = pMemPointTestCfg->RankBBitmasks[Socket][Controller][Channel][Dimm];
    }

    // factor in the Rankbitmask from the physical memory configuration
    RankBitmask &= (1 << pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]) - 1;

    NormalizedBitmask |= (RankBitmask << (Dimm * MAX_RANK_CNT));
  } // Dimm

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (GetNormalizedDimmRankBitMask NormalizedBitmask=0x%X)\n", NormalizedBitmask);
#endif

  return NormalizedBitmask;
} //  end function GetNormalizedDimmRankBitMask

/**
  This function is used to get the number of single rank combination/iteration or 
  set the dimm/rank pair to the specified rank iteration.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in, out]  pRankCombinationIndex  Pointer to Rank combination index variable. 
  @param[in]       IsGetDimmRankPair  Flag to indicate if get the combination count (FALSE) or get the Dimm/Rank pair per combination index (TRUE).
  @param[out]      pDimmRankA         Pointer to first zero based DIMM+rank pair numbers.
  @param[out]      pDimmRankB         Pointer to second zero based DIMM+rank pair numbers.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 DimmRankIterationSingles(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN OUT UINT8 *pRankCombinationIndex,
  IN BOOLEAN IsGetDimmRankPair,
  OUT DIMM_RANK_PAIR *pDimmRankA,
  OUT DIMM_RANK_PAIR *pDimmRankB)
{
  UINT32 NormalizedDimmRankBitmask_A;
  UINT32 NormalizedDimmRankBitmask_B;
  UINT8 i;
  UINT8 iterationCnt = 0;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "DimmRankIterationSingles(Socket=%u Controller=%u Channel=%u RankCombinationIndex=%u "
    "IsGetDimmRankPair=%u)\n",
    Socket, Controller, Channel, *pRankCombinationIndex, IsGetDimmRankPair);
#endif

  bssa_memset(pDimmRankA, 0, sizeof(*pDimmRankA));
  bssa_memset(pDimmRankB, 0, sizeof(*pDimmRankB));

  NormalizedDimmRankBitmask_A = GetNormalizedDimmRankBitMask(SsaServicesHandle, pMemCfg, pMemPointTestCfg, Socket, Controller, Channel, 'A');
  NormalizedDimmRankBitmask_B = GetNormalizedDimmRankBitMask(SsaServicesHandle, pMemCfg, pMemPointTestCfg, Socket, Controller, Channel, 'B');

  // for the single rank test mode, we just need to iterate the normalized dimm/rank bit maks and
  // find all the non-zero bit.
  for (i = 0; i < MAX_DIMM_CNT * MAX_RANK_CNT; i++){
    if (NormalizedDimmRankBitmask_A & (0x1 << i)){
      iterationCnt++;
    }

    // check if the current iteration index match the specified combination index  
    if ((IsGetDimmRankPair) && (iterationCnt == (*pRankCombinationIndex + 1))){ // RankCombinationIndex is zero base!
      
      pDimmRankA->Dimm = i / MAX_RANK_CNT;
      pDimmRankA->Rank = i - (i / MAX_RANK_CNT) * MAX_RANK_CNT;
#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  (DimmRankIterationSingles iteration:%d DimmA=%u RankA=%u DimmB=%u RankB=%u)\n", iterationCnt, pDimmRankA->Dimm, pDimmRankA->Rank, pDimmRankB->Dimm, pDimmRankB->Rank);
#endif
        return 0;
    }
  } // i

  if (!IsGetDimmRankPair) {
    // we are count the number of combination 
    *pRankCombinationIndex = iterationCnt;
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  (DimmRankIterationSingles RankCombinationCnt%u)\n", *pRankCombinationIndex);
#endif

  return 0;
} // end function DimmRankIterationSingles



#if SUPPORT_FOR_TURNAROUNDS
/**
  This function is used to get the number of turnaround rank combination/iteration or 
  set the dimm/rank pair to the specified rank iteration.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.  
  @param[in]       Socket             Zero based socket number.
  @param[in]       Controller         Zero based controller number.
  @param[in]       Channel            Zero based channel number.
  @param[in, out]  pRankCombinationIndex  Pointer to Rank combination index variable. 
  @param[in]       IsGetDimmRankPair  Flag to indicate if get the combination count (FALSE) or get the Dimm/Rank pair per combination index (TRUE).
  @param[out]      pDimmRankA         Pointer to first zero based DIMM+rank pair numbers.
  @param[out]      pDimmRankB         Pointer to second zero based DIMM+rank pair numbers.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 DimmRankIterationTurnaround(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN OUT UINT8 *pRankCombinationIndex,
  IN BOOLEAN IsGetDimmRankPair,
  OUT DIMM_RANK_PAIR *pDimmRankA,
  OUT DIMM_RANK_PAIR *pDimmRankB)
{
  UINT32 NormalizedDimmRankBitmask_A;
  UINT32 NormalizedDimmRankBitmask_B;
  UINT8 i, j;
  UINT8 iterationCnt = 0;
  UINT32 NormalizedDimmRankBitmask_B_filtered;
//#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  DIMM_RANK_PAIR Temp_DimmRankA;
  DIMM_RANK_PAIR Temp_DimmRankB;
  bssa_memset(&Temp_DimmRankA, 0, sizeof(Temp_DimmRankA));
  bssa_memset(&Temp_DimmRankB, 0, sizeof(Temp_DimmRankB));
//#endif
#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  if (!IsGetDimmRankPair){
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "DimmRankIterationTurnaround(Socket=%u Controller=%u Channel=%u "
    "IsGetDimmRankPair=%u ",
    Socket, Controller, Channel, *pRankCombinationIndex, IsGetDimmRankPair);
  } else {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "DimmRankIterationTurnaround(Socket=%u Controller=%u Channel=%u RankCombinationIndex=%u "
      "IsGetDimmRankPair=%u)\n",
      Socket, Controller, Channel, *pRankCombinationIndex, IsGetDimmRankPair);
  }
#endif

  bssa_memset(pDimmRankA, 0, sizeof(*pDimmRankA));
  bssa_memset(pDimmRankB, 0, sizeof(*pDimmRankB));

  NormalizedDimmRankBitmask_A = GetNormalizedDimmRankBitMask(SsaServicesHandle, pMemCfg, pMemPointTestCfg, Socket, Controller, Channel, 'A');
  NormalizedDimmRankBitmask_B = GetNormalizedDimmRankBitMask(SsaServicesHandle, pMemCfg, pMemPointTestCfg, Socket, Controller, Channel, 'B');

  // for the intra-dimm turnaround test mode, we need to iterate the normalized dimm/rank bit mask A
  // and find all the non-zero bit in the normalized dimm/rank bit mask B.
  for (i = 0; i < MAX_DIMM_CNT * MAX_RANK_CNT; i++){
    if (NormalizedDimmRankBitmask_A & (0x1 << i)){
      
      // find the DimmB/RankB that match the "intra-dimm" criteria  

      if (pMemPointTestCfg->TurnaroundsTestMode == OnlyIntraDimmTurnarounds) {
        // Case: turnaround for the ranks in the same dimm
        // Only include the dimm/rank bits in the same dimm as dimm A
        NormalizedDimmRankBitmask_B_filtered = NormalizedDimmRankBitmask_B & (((0x1 << MAX_RANK_CNT) - 1) << (i / MAX_RANK_CNT));

      } else if (pMemPointTestCfg->TurnaroundsTestMode == OnlyInterDimmTurnarounds){
        // Case: turnaround for the ranks in the different dimm
        // Exclude all the dimm/rank bits not in the same dimm as dimm A
        NormalizedDimmRankBitmask_B_filtered = NormalizedDimmRankBitmask_B & (~(((0x1 << MAX_RANK_CNT) - 1) << (i / MAX_RANK_CNT))); 
      } else {
        NormalizedDimmRankBitmask_B_filtered = NormalizedDimmRankBitmask_B;
      }

      // Exclude rank that is the same the dimm/rank A
      NormalizedDimmRankBitmask_B_filtered &= ~(0x1 << i);

      for (j = 0; j < MAX_DIMM_CNT * MAX_RANK_CNT; j++){

        // Exclude dimms with different memory technology, they are not allowed do turnaround like the DDR4/DDRT
        if (pMemCfg->MemoryTech[Socket][Controller][Channel][i / MAX_RANK_CNT] != pMemCfg->MemoryTech[Socket][Controller][Channel][j / MAX_RANK_CNT]) continue;

        if (NormalizedDimmRankBitmask_B_filtered & (0x1 << j)){
          iterationCnt++;

//#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
//          Temp_DimmRankA.Dimm = i / MAX_RANK_CNT;
//          Temp_DimmRankA.Rank = i - (i / MAX_RANK_CNT) * MAX_RANK_CNT;
//          Temp_DimmRankB.Dimm = j / MAX_RANK_CNT;
//          Temp_DimmRankB.Rank = j - (j / MAX_RANK_CNT) * MAX_RANK_CNT;
//          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
//            "  (Iteration:%d DimmA=%u RankA=%u DimmB=%u RankB=%u)\n", iterationCnt, Temp_DimmRankA.Dimm, Temp_DimmRankA.Rank, Temp_DimmRankB.Dimm, Temp_DimmRankB.Rank);
//#endif

          // check if the current iteration index match the specified combination index  
          if ((IsGetDimmRankPair) && (iterationCnt == (*pRankCombinationIndex + 1))){ // RankCombinationIndex is zero base!
            pDimmRankA->Dimm = i / MAX_RANK_CNT;
            pDimmRankA->Rank = i - (i / MAX_RANK_CNT) * MAX_RANK_CNT;
            pDimmRankB->Dimm = j / MAX_RANK_CNT;
            pDimmRankB->Rank = j - (j / MAX_RANK_CNT) * MAX_RANK_CNT;
//#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  (DimmRankIterationTurnaround iteration:%d DimmA=%u RankA=%u DimmB=%u RankB=%u)\n", iterationCnt, pDimmRankA->Dimm, pDimmRankA->Rank, pDimmRankB->Dimm, pDimmRankB->Rank);
//#endif
            return 0;
          }

        // if the rank-to-rank turnarounds pair mode is order dependent (i.e., both A-B and B-A)
          if (pMemPointTestCfg->TurnaroundsPairMode == OrderDependent){
            // swap the dimm/rank A and dimm/rank B  
            iterationCnt++;
//#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
//            Temp_DimmRankA.Dimm = i / MAX_RANK_CNT;
//            Temp_DimmRankA.Rank = i - (i / MAX_RANK_CNT) * MAX_RANK_CNT;
//            Temp_DimmRankB.Dimm = j / MAX_RANK_CNT;
//            Temp_DimmRankB.Rank = j - (j / MAX_RANK_CNT) * MAX_RANK_CNT;
//            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
//              "  (Iteration:%d DimmA=%u RankA=%u DimmB=%u RankB=%u)\n", iterationCnt, Temp_DimmRankA.Dimm, Temp_DimmRankA.Rank, Temp_DimmRankB.Dimm, Temp_DimmRankB.Rank);
//#endif
            // check if the current iteration index match the specified combination index  
            if ((IsGetDimmRankPair) && (iterationCnt == (*pRankCombinationIndex + 1))){ // RankCombinationIndex is zero base!
              pDimmRankA->Dimm = j / MAX_RANK_CNT;
              pDimmRankA->Rank = j - (j / MAX_RANK_CNT) * MAX_RANK_CNT;
              pDimmRankB->Dimm = i / MAX_RANK_CNT;
              pDimmRankB->Rank = i - (i / MAX_RANK_CNT) * MAX_RANK_CNT;
//#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "  (DimmRankIterationTurnaround iteration:%d DimmA=%u RankA=%u DimmB=%u RankB=%u)\n", iterationCnt, pDimmRankA->Dimm, pDimmRankA->Rank, pDimmRankB->Dimm, pDimmRankB->Rank);
//#endif
              return 0;
            }
          }
        }
      } // j
    }// for each available Dimm/Rank bit
  } // i

 
  if (!IsGetDimmRankPair) {
    // we are count the number of combination 
    *pRankCombinationIndex = iterationCnt;
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "   RankCombinationCnt:%u)\n", *pRankCombinationIndex);
#endif

  return 0;
} // end function DimmRankIterationTurnaround
#endif // SUPPORT_FOR_TURNAROUNDS

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
  OUT DIMM_RANK_PAIR *pDimmRankB)
{
#if SUPPORT_FOR_TURNAROUNDS
  // IF this channel is configured to do rank-to-rank turnarounds but
  // can't do so
  if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] == Turnarounds) {

    DimmRankIterationTurnaround(SsaServicesHandle,pSystemInfo, pMemCfg,pMemPointTestCfg,
      Socket, Controller, Channel, &RankCombinationIndex, TRUE, 
      pDimmRankA,pDimmRankB);
  }
  else
#endif // SUPPORT_FOR_TURNAROUNDS
  {
    DimmRankIterationSingles(SsaServicesHandle,pSystemInfo, pMemCfg,pMemPointTestCfg,
      Socket, Controller, Channel, &RankCombinationIndex, TRUE,
       pDimmRankA,pDimmRankB);
  }
  return 0;
}

/**
  This function is used to set default values in the given memory point test
  configuration.

  @param[in]   pSystemInfo        Pointer to system information structure.
  @param[out]  pMemPointTestCfg   Pointer to generic memory point test configuration structure.
**/
VOID SetMemPointTestCfgDefaults(
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  OUT MEM_POINT_TEST_CONFIG *pMemPointTestCfg)
{
  UINT8 Socket, Controller, Channel, Dimm, ByteGroup;

  // start by setting everything to 0
  bssa_memset(pMemPointTestCfg, 0, sizeof(*pMemPointTestCfg));

  pMemPointTestCfg->SocketBitmask = ALL_POPULATED_SOCKETS;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    pMemPointTestCfg->ControllerBitmasks[Socket] = ALL_POPULATED_CONTROLLERS;

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      pMemPointTestCfg->ChannelBitmasks[Socket][Controller] = ALL_POPULATED_CHANNELS;
      pMemPointTestCfg->ChannelValBitmasks[Socket][Controller] = ALL_POPULATED_CHANNELS;

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] = Singles;
        pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] = ALL_POPULATED_DIMMS;
        pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel] = ALL_POPULATED_DIMMS;

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++){
          pMemPointTestCfg->RankABitmasks[Socket][Controller][Channel][Dimm] = ALL_POPULATED_RANKS;
          pMemPointTestCfg->RankBBitmasks[Socket][Controller][Channel][Dimm] = ALL_POPULATED_RANKS;
        } // end for each DIMM
      } // end for each channel
    } // end for each controller
  } // end for each socket

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      // FOR each byte group:
      for (ByteGroup = 0; ByteGroup < (pSystemInfo->BusWidth / 8); ByteGroup++) {
#ifdef __STUB__
        assert(ByteGroup < (sizeof(pMemPointTestCfg->LaneValBitmasks[0][0]) /
          sizeof(pMemPointTestCfg->LaneValBitmasks[0][0][0])));
#endif // __STUB__
        // IF there's a mismatch THEN at least avoid a corruption
        if (ByteGroup >= (sizeof(pMemPointTestCfg->LaneValBitmasks[0][0]) /
          sizeof(pMemPointTestCfg->LaneValBitmasks[0][0][0]))) {
          break;
        }

        pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup] = 0xFF;
      } // end for each byte group

      pMemPointTestCfg->ChunkValBitmasks[Controller][Channel] = ALL_CHUNKS_BITMASK;
    } // end for each channel
  } // end for each controller
} // end function SetMemPointTestCfgDefaults

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
  OUT UINT16 *pBacksideCmdBitmasks)
{
  UINT8 Socket, Controller, Channel, Dimm, Rank = 0;
  GSM_LT IoLevel = LrbufLevel;
  UINT8 Idx;
  BOOLEAN Enable;
  UINT8 LaneMasks[MAX_BUS_WIDTH / 8];
  INT16 MinOffset, MaxOffset;
  UINT16 Delay, StepUnit;
  UINT16 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "InitBacksideCmdBitmask(MarginGroupCount=%u)\n", MarginGroupCount);
#endif

#ifdef __STUB__
  // confirm design assumption that all values will fit in packed 16-bit variable
  assert((MAX_CONTROLLER_CNT * MAX_CHANNEL_CNT * MAX_DIMM_CNT) < 16);
#endif // __STUB__

  bssa_memset(LaneMasks, 0xFF, sizeof(LaneMasks));

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // start with an empty bitmask
    pBacksideCmdBitmasks[Socket] = 0;

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          // IF this DIMM is not being tested THEN skip it
          if (!IsDimmATestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
            pMemPointTestCfg, Socket, Controller, Channel,Dimm)) {
            continue;
          }

          // IF any of the margin parameter groups are not supported on the
          // backside THEN backside command margining isn't done

          Enable = TRUE;

          // FOR each command margin group:
          for (Idx = 0; Idx < MarginGroupCount; Idx++) {
            if (pMarginGroups[Idx] == GsmGtMax) {
              continue;
            }

            // NOTE: We're calling GetMarginParamLimits here without calling
            // InitMarginParam first, but since we're only interested in
            // whether the device supports backside command margining (i.e.,
            // the return code) there should be no problem.
            if (SsaServicesHandle->SsaMemoryConfig->GetMarginParamLimits(
              SsaServicesHandle, Socket, Controller, Channel, Dimm, Rank,
              LaneMasks, IoLevel, pMarginGroups[Idx], &MinOffset, &MaxOffset,
              &Delay, &StepUnit) == UnsupportedValue) {
              Enable = FALSE;
              break;
            }
          } // end for each command margin group

          if (Enable) {
            Bitmask = 1 << ((Controller * MAX_CHANNEL_CNT * MAX_DIMM_CNT) +
              (Channel * MAX_DIMM_CNT) + Dimm);

            pBacksideCmdBitmasks[Socket] |= Bitmask;
          }
        } // end for each DIMM
      } // end for each channel
    } // end for each controller
  } // end for each socket

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "(InitBacksideCmdBitmask");
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      " BacksideCmdBitmask=0x%04X", pBacksideCmdBitmasks[Socket]);
  }
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    ")\n", MarginGroupCount);
#endif
} // end function InitBacksideCmdBitmask

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
  IN OUT MEM_POINT_TEST_CONFIG *pMemPointTestCfg)
{
  UINT8 Socket, Controller, Channel, Dimm;
  BOOLEAN IsSocketToBeTested;
  BOOLEAN IsControllerToBeTested;
  BOOLEAN IsChannelToBeTested;
  UINT16 Bitmask;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is not to be tested THEN skip it
    if ((pMemPointTestCfg->SocketBitmask & (1 << Socket)) == 0) {
      continue;
    }

    IsSocketToBeTested = FALSE;

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not to be tested THEN skip it
      if ((pMemPointTestCfg->ControllerBitmasks[Socket] &
        (1 << Controller)) == 0) {
        continue;
      }

      IsControllerToBeTested = FALSE;

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not to be tested THEN skip it
        if ((pMemPointTestCfg->ChannelBitmasks[Socket][Controller] &
          (1 << Channel)) == 0) {
          continue;
        }

        IsChannelToBeTested = FALSE;

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          // IF this DIMM is not to be tested THEN skip it
          if ((pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] &
            (1 << Dimm)) == 0) {
            continue;
          }

          Bitmask = 1 << ((Controller * MAX_CHANNEL_CNT * MAX_DIMM_CNT) +
            (Channel * MAX_DIMM_CNT) + Dimm);

          // IF the DIMM supports backside command margining
          if (pBacksideCmdBitmasks[Socket] & Bitmask) {
            IsSocketToBeTested = TRUE;
            IsControllerToBeTested = TRUE;
            IsChannelToBeTested = TRUE;
          }
          else {
            // IF the bitmask is currently the special "all" value
            if (pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] ==
              ALL_POPULATED_DIMMS) {
              // start with the bitmask of what's actually populated
              pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] = \
                pMemCfg->DimmBitmasks[Socket][Controller][Channel];
            }

            // remove the DIMM
            pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] &= \
              ~(1 << Dimm);
          }
        } // end for each DIMM

        // IF it turns out that there were no ranks to test on this channel
        if (!IsChannelToBeTested) {
          // IF the bitmask is currently the special "all" value
          if (pMemPointTestCfg->ChannelBitmasks[Socket][Controller] ==
            ALL_POPULATED_CHANNELS) {
            // start with the bitmask of what's actually populated
            pMemPointTestCfg->ChannelBitmasks[Socket][Controller] = \
              pMemCfg->ChannelBitmasks[Socket][Controller];
          }

          // remove the channel
          pMemPointTestCfg->ChannelBitmasks[Socket][Controller] &= \
            ~(1 << Channel);
        }
      } // end for each channel

      // IF it turns out that there were no ranks to test on this controller
      if (!IsControllerToBeTested) {
        // IF the bitmask is currently the special "all" value
        if (pMemPointTestCfg->ControllerBitmasks[Socket] ==
          ALL_POPULATED_CONTROLLERS) {
          // start with the bitmask of what's actually populated
          pMemPointTestCfg->ControllerBitmasks[Socket] = \
            pMemCfg->ControllerBitmasks[Socket];
        }

        // remove the controller
        pMemPointTestCfg->ControllerBitmasks[Socket] &= ~(1 << Controller);
      }
    } // end for each controller

    // IF it turns out that there were no ranks to test on this socket
    if (!IsSocketToBeTested) {
      // IF the bitmask is currently the special "all" value
      if (pMemPointTestCfg->SocketBitmask == ALL_POPULATED_SOCKETS) {
        // start with the bitmask of what's actually populated
        pMemPointTestCfg->SocketBitmask = pSystemInfo->SocketBitMask;
      }

      // remove the socket
      pMemPointTestCfg->SocketBitmask &= ~(1 << Socket);
    }
  } // end for each socket
} // end function RemoveNonBacksideCmdDimms

/**
  This function determines if any bits are set in the given backside command
  bitmasks.

  @param[in]  pBacksideCmdBitmasks  Pointer to array of bitmasks of whether DIMMs support backside command margining.

  @retval  0  no bits are set
  @retval  1  at least one bit is set
**/
BOOLEAN IsAnyBacksideCmdBitSet(
  IN UINT16 *pBacksideCmdBitmasks)
{
  UINT8 Socket;
  BOOLEAN IsAnyBitSet = FALSE;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    if (pBacksideCmdBitmasks[Socket] != 0) {
      IsAnyBitSet = TRUE;
      break;
    }
  } // end for each socket

  return IsAnyBitSet;
} // end function IsAnyBacksideCmdBitSet

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
  IN OUT MEM_POINT_TEST_CONFIG *pMemPointTestCfg)
{
  UINT8 Socket, Controller, Channel, Dimm;
  BOOLEAN IsSocketToBeTested;
  BOOLEAN IsControllerToBeTested;
  BOOLEAN IsChannelToBeTested;
  MEMORY_TECHNOLOGY MemoryTech;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is not to be tested THEN skip it
    if ((pMemPointTestCfg->SocketBitmask & (1 << Socket)) == 0) {
      continue;
    }

    IsSocketToBeTested = FALSE;

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not to be tested THEN skip it
      if ((pMemPointTestCfg->ControllerBitmasks[Socket] &
        (1 << Controller)) == 0) {
        continue;
      }

      IsControllerToBeTested = FALSE;

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not to be tested THEN skip it
        if ((pMemPointTestCfg->ChannelBitmasks[Socket][Controller] &
          (1 << Channel)) == 0) {
          continue;
        }

        IsChannelToBeTested = FALSE;

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          // IF this DIMM is not to be tested THEN skip it
          if ((pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] &
            (1 << Dimm)) == 0) {
            continue;
          }

          MemoryTech = pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm];

          // IF the DIMM supports backside command margining
          if (MemoryTech == SsaMemoryDdrT) {
            IsSocketToBeTested = TRUE;
            IsControllerToBeTested = TRUE;
            IsChannelToBeTested = TRUE;
          }
          else {
            // IF the bitmask is currently the special "all" value
            if (pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] ==
              ALL_POPULATED_DIMMS) {
              // start with the bitmask of what's actually populated
              pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] = \
                pMemCfg->DimmBitmasks[Socket][Controller][Channel];
            }

            // remove the DIMM
            pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] &= \
              ~(1 << Dimm);
          }
        } // end for each DIMM

        // IF it turns out that there were no ranks to test on this channel
        if (!IsChannelToBeTested) {
          // IF the bitmask is currently the special "all" value
          if (pMemPointTestCfg->ChannelBitmasks[Socket][Controller] ==
            ALL_POPULATED_CHANNELS) {
            // start with the bitmask of what's actually populated
            pMemPointTestCfg->ChannelBitmasks[Socket][Controller] = \
              pMemCfg->ChannelBitmasks[Socket][Controller];
          }

          // remove the channel
          pMemPointTestCfg->ChannelBitmasks[Socket][Controller] &= \
            ~(1 << Channel);
        }
      } // end for each channel

      // IF it turns out that there were no ranks to test on this controller
      if (!IsControllerToBeTested) {
        // IF the bitmask is currently the special "all" value
        if (pMemPointTestCfg->ControllerBitmasks[Socket] ==
          ALL_POPULATED_CONTROLLERS) {
          // start with the bitmask of what's actually populated
          pMemPointTestCfg->ControllerBitmasks[Socket] = \
            pMemCfg->ControllerBitmasks[Socket];
        }

        // remove the controller
        pMemPointTestCfg->ControllerBitmasks[Socket] &= ~(1 << Controller);
      }
    } // end for each controller

    // IF it turns out that there were no ranks to test on this socket
    if (!IsSocketToBeTested) {
      // IF the bitmask is currently the special "all" value
      if (pMemPointTestCfg->SocketBitmask == ALL_POPULATED_SOCKETS) {
        // start with the bitmask of what's actually populated
        pMemPointTestCfg->SocketBitmask = pSystemInfo->SocketBitMask;
      }

      // remove the socket
      pMemPointTestCfg->SocketBitmask &= ~(1 << Socket);
    }
  } // end for each socket
} // end function RemoveNonBacksideCmdDimms

#endif // SUPPORT_FOR_DDRT
/**
  This function is used to check the given memory point test configuration for
  correctness/consistency.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 ChkMemPointTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller, Channel, Dimm;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "ChkMemPointTestCfg()\n");
#endif

  // IF the "all populated" value isn't specified
  if (pMemPointTestCfg->SocketBitmask != ALL_POPULATED_SOCKETS) {
    // make sure all sockets enabled for testing are populated
    if ((pMemPointTestCfg->SocketBitmask & pSystemInfo->SocketBitMask) !=
      pMemPointTestCfg->SocketBitmask) {
#if ENBL_VERBOSE_ERROR_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "ERROR: Requested SocketBitmask=0x%X populated SocketBitmask=0x%X\n",
        pMemPointTestCfg->SocketBitmask, pSystemInfo->SocketBitMask);
#endif
      ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
      return 1;
    }
  } // end if the "all populated" value isn't specified

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is enabled for testing:
    if (IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
      pMemPointTestCfg, Socket)) {
      // IF the "all populated" value isn't specified
      if (pMemPointTestCfg->ControllerBitmasks[Socket] !=
        ALL_POPULATED_CONTROLLERS) {
        // IF all controllers enabled for testing are not populated
        if ((pMemPointTestCfg->ControllerBitmasks[Socket] &
          pMemCfg->ControllerBitmasks[Socket]) !=
          pMemPointTestCfg->ControllerBitmasks[Socket]) {
#if ENBL_VERBOSE_ERROR_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "ERROR: Socket=%d requested ControllerBitmask=0x%X "
            "populated ControllerBitmask=0x%X\n",
            Socket, pMemPointTestCfg->ControllerBitmasks[Socket],
            pMemCfg->ControllerBitmasks[Socket]);
#endif
          ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
          return 1;
        }
      } // end if the "all populated" value isn't specified

      // FOR each controller:
      for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
        // IF this controller is enabled for testing:
        if (IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket, Controller)) {
          // IF the "all populated" value isn't specified
          if (pMemPointTestCfg->ChannelBitmasks[Socket][Controller] !=
            ALL_POPULATED_CHANNELS) {
            // IF all channels enabled for testing are not populated
            if ((pMemPointTestCfg->ChannelBitmasks[Socket][Controller] &
              pMemCfg->ChannelBitmasks[Socket][Controller]) !=
              pMemPointTestCfg->ChannelBitmasks[Socket][Controller]) {
#if ENBL_VERBOSE_ERROR_MSGS
              NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                "ERROR: Socket=%d Controller=%d requested ChannelBitmask=0x%X "
                "populated ChannelBitmask=0x%X\n",
                Socket, Controller,
                pMemPointTestCfg->ChannelBitmasks[Socket][Controller],
                pMemCfg->ChannelBitmasks[Socket][Controller]);
#endif
              ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
              return 1;
            }
          } // end if the "all populated" value isn't specified

          if ((pMemPointTestCfg->ChannelValBitmasks[Socket][Controller] &
            pMemPointTestCfg->ChannelBitmasks[Socket][Controller]) !=
            pMemPointTestCfg->ChannelValBitmasks[Socket][Controller]) {
#if ENBL_VERBOSE_ERROR_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "ERROR: Socket=%d Controller=%d requested ChannelValBitmask=0x%X "
              "and ChannelBitmask=0x%X\n",
              Socket, Controller,
              pMemPointTestCfg->ChannelValBitmasks[Socket][Controller],
              pMemPointTestCfg->ChannelBitmasks[Socket][Controller]);
#endif
            ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
            return 1;
          }

          // FOR each channel:
          for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
            // IF this channel is enabled for testing:
            if (IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo,
              pMemCfg, pMemPointTestCfg, Socket, Controller, Channel)) {
              // IF the "all populated" value isn't specified
              if (pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] !=
                ALL_POPULATED_DIMMS) {
                // IF all DIMM A's enabled for testing are not populated
                if ((pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] &
                  pMemCfg->DimmBitmasks[Socket][Controller][Channel]) !=
                  pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel]) {
#if ENBL_VERBOSE_ERROR_MSGS
                  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                    "ERROR: Socket=%d Controller=%d Channel=%d "
                    "requested DimmABitmasks=0x%X populated DimmBitmasks=0x%X\n",
                    Socket, Controller, Channel,
                    pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel],
                    pMemCfg->DimmBitmasks[Socket][Controller][Channel]);
#endif
                  ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
                  return 1;
                }
              } // end if the "all populated" value isn't specified

              if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] >=
                RankTestModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
                NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                  "ERROR: Socket=%d Controller=%d Channel=%d "
                  "requested RankCombinations (%u) is out of range (0 <= val < %u)\n",
                  Socket, Controller, Channel,
                  pMemPointTestCfg->RankTestModes[Socket][Controller][Channel],
                  RankTestModeMax);
#endif
                ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
                return 1;
              }

#if SUPPORT_FOR_TURNAROUNDS
              // IF rank-to-rank turnaround testing is specified:
              if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] ==
                Turnarounds) {
                // IF the "all populated" value isn't specified
                if (pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel] !=
                  ALL_POPULATED_DIMMS) {
                  // IF all DIMM B's enabled for testing are not populated
                  if ((pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel] &
                    pMemCfg->DimmBitmasks[Socket][Controller][Channel]) !=
                    pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel]) {
#if ENBL_VERBOSE_ERROR_MSGS
                    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                      "ERROR: Socket=%d Controller=%d Channel=%d "
                      "requested DimmBBitmasks=0x%X populated DimmBitmasks=0x%X\n",
                      Socket, Controller, Channel,
                      pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel],
                      pMemCfg->DimmBitmasks[Socket][Controller][Channel]);
#endif
                    ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
                    return 1;
                  }
                } // end if the "all populated" value isn't specified
              } // end if rank-to-rank turnaround testing is specified
#endif // SUPPORT_FOR_TURNAROUNDS

              // FOR each DIMM:
              for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
                // IF this DIMM is enabled for testing as DIMM A:
                if (IsDimmATestingEnabled(SsaServicesHandle, pSystemInfo,
                  pMemCfg, pMemPointTestCfg, Socket, Controller, Channel,
                  Dimm)) {
                    // IF the "all populated" value isn't specified
                    if (pMemPointTestCfg->RankABitmasks[Socket][Controller][Channel][Dimm] !=
                      ALL_POPULATED_DIMMS) {
                      // IF all rank A's enabled for testing are not populated
                      if (pMemPointTestCfg->RankABitmasks[Socket][Controller][Channel][Dimm] >
                        ((1 << pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]) - 1)) {
#if ENBL_VERBOSE_ERROR_MSGS
                        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                          "ERROR: Socket=%d Controller=%d Channel=%d Dimm=%d "
                          "requested RankABitmasks=0x%X populated rank count=%u\n",
                          Socket, Controller, Channel, Dimm,
                          pMemPointTestCfg->RankABitmasks[Socket][Controller][Channel][Dimm],
                          pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]);
#endif
                        ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
                        return 1;
                      }
                    } // end if the "all populated" value isn't specified
                } // end if this DIMM is enabled for testing as DIMM A

#if SUPPORT_FOR_TURNAROUNDS
                // IF this DIMM is enabled for testing as DIMM B:
                if (IsDimmBTestingEnabled(SsaServicesHandle, pSystemInfo,
                  pMemCfg, pMemPointTestCfg, Socket, Controller, Channel,
                  Dimm)) {
                  // IF the "all populated" value isn't specified
                  if (pMemPointTestCfg->RankBBitmasks[Socket][Controller][Channel][Dimm] !=
                    ALL_POPULATED_DIMMS) {
                    // IF all rank B's enabled for testing are not populated
                    if (pMemPointTestCfg->RankBBitmasks[Socket][Controller][Channel][Dimm] >
                      ((1 << pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]) - 1)) {
#if ENBL_VERBOSE_ERROR_MSGS
                      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
                        "ERROR: Socket=%d Controller=%d Channel=%d Dimm=%d "
                        "requested RankBBitmasks=0x%X populated rank count=%u\n",
                        Socket, Controller, Channel, Dimm,
                        pMemPointTestCfg->RankBBitmasks[Socket][Controller][Channel][Dimm],
                        pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]);
#endif
                      ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
                      return 1;
                    }
                  } // end if the "all populated" value isn't specified
                } // end if this DIMM is enabled for testing as DIMM B
#endif // SUPPORT_FOR_TURNAROUNDS
              } // end for each DIMM
            } // end if this channel is enabled for testing
          } // end for each channel
        } // end if this controller is enabled for testing
      } // end for each controller
    } // end if this socket is enabled for testing
  } // end for each socket

#if SUPPORT_FOR_TURNAROUNDS
  if (pMemPointTestCfg->TurnaroundsTestMode >= TurnaroundsTestModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: Requested TurnaroundsTestMode (%u) is out of range (0 <= val < %u)\n",
      pMemPointTestCfg->TurnaroundsTestMode, TurnaroundsTestModeMax);
#endif
    ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  if (pMemPointTestCfg->TurnaroundsPairMode >= TurnaroundsPairModeMax) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: Requested TurnaroundsPairMode (%u) is out of range (0 <= val < %u)\n",
      pMemPointTestCfg->TurnaroundsPairMode, TurnaroundsPairModeMax);
#endif
    ProcError(SsaServicesHandle, NotAvailable, __FILE__, __LINE__, pTestStat);
    return 1;
  }
#endif // SUPPORT_FOR_TURNAROUNDS

  return 0;
} // end function ChkMemPointTestCfg

/**
  This function is used to log the memory point test configuration.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
**/
VOID LogMemPointTestCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg)
{
  UINT8 Socket, Controller, Channel, Dimm;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "LogMemPointTestCfg()\n");
#endif

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n"
    "Memory Point Test Configuration:\n");
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  SocketBitmask=0x%X\n", pMemPointTestCfg->SocketBitmask);
  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    Socket=%u\n", Socket);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "    ControllerBitmask=0x%X\n",
      pMemPointTestCfg->ControllerBitmasks[Socket]);

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      Controller=%u\n", Controller);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      ChannelBitmask=0x%X\n",
        pMemPointTestCfg->ChannelBitmasks[Socket][Controller]);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      ChannelValBitmask=0x%X\n",
        pMemPointTestCfg->ChannelValBitmasks[Socket][Controller]);

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "        Channel=%u\n", Channel);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "          RankTestMode=%u\n",
          pMemPointTestCfg->RankTestModes[Socket][Controller][Channel]);
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "          DimmABitmask=0x%X\n",
          pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel]);

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "          Dimm=%u\n", Dimm);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "            RankABitmask=0x%X\n",
            pMemPointTestCfg->RankABitmasks[Socket][Controller][Channel][Dimm]);
        } // end for each DIMM

        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "          DimmBBitmask=0x%X\n",
          pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel]);

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "          Dimm=%u\n", Dimm);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "            RankBBitmask=0x%X\n",
            pMemPointTestCfg->RankBBitmasks[Socket][Controller][Channel][Dimm]);
        } // end for each DIMM
      } // end for each channel
    } // end for each controller
  } // end for each socket

  // FOR each controller:
  for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Controller=%u\n", Controller);
    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    Channel=%u\n", Channel);
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      LaneValBitmask=0x");
      LogByteArrayAsHex(SsaServicesHandle,
        pMemPointTestCfg->LaneValBitmasks[Controller][Channel],
        min((pSystemInfo->BusWidth / 8),
        (sizeof(pMemPointTestCfg->LaneValBitmasks[0][0]) /
        sizeof(pMemPointTestCfg->LaneValBitmasks[0][0][0]))));
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "\n");

      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "      ChunkValBitmask=0x%02X\n",
        pMemPointTestCfg->ChunkValBitmasks[Controller][Channel]);
    } // end for each channel
  } // end for each controller

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  TurnaroundsTestMode=%u\n", pMemPointTestCfg->TurnaroundsTestMode);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  TurnaroundsPairMode=%u\n", pMemPointTestCfg->TurnaroundsPairMode);

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n");
} // end function LogMemPointTestCfg

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
  IN UINT8 Socket)
{
  BOOLEAN RetVal = TRUE;
  UINT32 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsSocketTestingEnabled(Socket=%u)\n", Socket);
#endif

  Bitmask = pMemPointTestCfg->SocketBitmask & pSystemInfo->SocketBitMask;

  // IF the socket is not enabled for testing
  if ((Bitmask & (1 << Socket)) == 0) {
    RetVal = FALSE;
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsSocketTestingEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsSocketTestingEnabled

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
  IN UINT8 Controller)
{
  BOOLEAN RetVal;
  UINT8 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsControllerTestingEnabled(Socket=%u Controller=%u)\n",
    Socket, Controller);
#endif

  RetVal = IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
    pMemPointTestCfg, Socket);

  if (RetVal) {
    Bitmask = pMemPointTestCfg->ControllerBitmasks[Socket] & \
      pMemCfg->ControllerBitmasks[Socket];

    // IF the controller is not enabled for testing
    if ((Bitmask & (1 << Controller)) == 0) {
      RetVal = FALSE;
    }
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsControllerTestingEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsControllerTestingEnabled

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
  IN UINT8 Channel)
{
  BOOLEAN RetVal;
  UINT8 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsChannelTestingEnabled(Socket=%u Controller=%u Channel=%u)\n",
    Socket, Controller, Channel);
#endif

  RetVal = IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, Socket, Controller);

  if (RetVal) {
    Bitmask = pMemPointTestCfg->ChannelBitmasks[Socket][Controller] & \
      pMemCfg->ChannelBitmasks[Socket][Controller];

    // IF the channel is not enabled for testing
    if ((Bitmask & (1 << Channel)) == 0) {
      RetVal = FALSE;
    }
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsChannelTestingEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsChannelTestingEnabled

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
  IN UINT8 Dimm)
{
  BOOLEAN RetVal;
  UINT8 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsDimmATestingEnabled(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
    Socket, Controller, Channel, Dimm);
#endif

  RetVal = IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, Socket, Controller, Channel);

  if (RetVal) {
    Bitmask = pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] & \
      pMemCfg->DimmBitmasks[Socket][Controller][Channel];

    // IF the DIMM is not enabled for testing
    if ((Bitmask & (1 << Dimm)) == 0) {
      RetVal = FALSE;
    }
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsDimmATestingEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsDimmATestingEnabled

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
  IN UINT8 Dimm)
{
  BOOLEAN RetVal;
  UINT8 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsDimmBTestingEnabled(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
    Socket, Controller, Channel, Dimm);
#endif

  if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] == Turnarounds) {
    RetVal = IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
      pMemPointTestCfg, Socket, Controller, Channel);
  }
  else {
    RetVal = FALSE;
  }

  if (RetVal) {
    Bitmask = pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel] & \
      pMemCfg->DimmBitmasks[Socket][Controller][Channel];

    // IF the DIMM is not enabled for testing
    if ((Bitmask & (1 << Dimm)) == 0) {
      RetVal = FALSE;
    }
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsDimmBTestingEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsDimmBTestingEnabled
#endif // SUPPORT_FOR_TURNAROUNDS

/**
  Function used to determine whether the given logical rank A is enabled for testing.

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
  IN UINT8 Rank)
{
  BOOLEAN RetVal;
  UINT8 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsRankATestingEnabled(Socket=%u Controller=%u Channel=%u Dimm=%u Rank=%u)\n",
    Socket, Controller, Channel, Dimm, Rank);
#endif

  RetVal = IsDimmATestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, Socket, Controller, Channel, Dimm);

  if (RetVal) {
    // get the bitmask of rank A's available for testing
    Bitmask = pMemPointTestCfg->RankABitmasks[Socket][Controller][Channel][Dimm] & \
      ((1 << pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]) - 1);

    // IF the logical rank is not available for testing
    if ((Bitmask & (1 << Rank)) == 0) {
      RetVal = FALSE;
    }
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsRankATestingEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsRankATestingEnabled

#if SUPPORT_FOR_TURNAROUNDS
/**
  Function used to determine whether the given physical rank is enabled for testing as rank B.

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
  IN UINT8 Rank)
{
  BOOLEAN RetVal;
  UINT8 Bitmask;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsRankBTestingEnabled(Socket=%u Controller=%u Channel=%u Dimm=%u Rank=%u)\n",
    Socket, Controller, Channel, Dimm, Rank);
#endif

  RetVal = IsDimmBTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, Socket, Controller, Channel, Dimm);

  if (RetVal) {
    // get the bitmask of rank B's available for testing
    Bitmask = pMemPointTestCfg->RankBBitmasks[Socket][Controller][Channel][Dimm] & \
      ((1 << pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]) - 1);

    // IF the logical rank is not available for testing
    if ((Bitmask & (1 << Rank)) == 0) {
      RetVal = FALSE;
    }
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsRankBTestingEnabled Retval=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsRankBTestingEnabled
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
  IN UINT8 Channel)
{
  BOOLEAN RetVal;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsChannelValEnabled(Socket=%u Controller=%u Channel=%u)\n",
    Socket, Controller, Channel);
#endif

  RetVal = IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, Socket, Controller, Channel);

  if (RetVal) {
    if ((pMemPointTestCfg->ChannelValBitmasks[Socket][Controller] &
      (1 << Channel)) == 0) {
      RetVal = FALSE;
    }
  }

  // IF the channel is enabled for testing
  if (RetVal) {
    UINT8 ByteGroup;

    // preset the return value
    RetVal = FALSE;

    // FOR each byte group:
    for (ByteGroup = 0; ByteGroup < (pSystemInfo->BusWidth / 8); ByteGroup++) {
#ifdef __STUB__
      assert(ByteGroup < (sizeof(pMemPointTestCfg->LaneValBitmasks[0][0]) /
        sizeof(pMemPointTestCfg->LaneValBitmasks[0][0][0])));
#endif // __STUB__
      // IF there's a mismatch THEN at least avoid a corruption
      if (ByteGroup >= (sizeof(pMemPointTestCfg->LaneValBitmasks[0][0]) /
        sizeof(pMemPointTestCfg->LaneValBitmasks[0][0][0]))) {
        break;
      }

      // IF any lane in this byte group is enabled for validation
      if (pMemPointTestCfg->LaneValBitmasks[Controller][Channel][ByteGroup]) {
        // the channel is enabled for validation
        RetVal = TRUE;

        // exit the byte group for loop now
        break;
      }
    } // end for each byte group
  } // end if the channel is enabled for testing

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsChannelValEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function IsChannelValEnabled

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
  IN UINT8 Controller)
{
  UINT8 Channel;
  BOOLEAN RetVal;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsControllerValEnabled(Socket=%u Controller=%u)\n",
    Socket, Controller);
#endif

  RetVal = IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, Socket, Controller);

  // IF the controller is enabled for testing
  if (RetVal) {
    RetVal = FALSE;

    // FOR each channel:
    for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
      RetVal = IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller, Channel);

      // IF the channel is enabled for validation THEN exit the for loop
      if (RetVal) {
        break;
      }
    } // end for each channel
  } // end if the controller is enabled for testing

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsControllerValEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end if IsControllerValEnabled

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
  IN UINT8 Socket)
{
  UINT8 Controller;
  BOOLEAN RetVal;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "IsSocketValEnabled(Socket=%u)\n", Socket);
#endif

  RetVal = IsSocketTestingEnabled(SsaServicesHandle, pSystemInfo,
    pMemPointTestCfg, Socket);

  // IF the socket is enabled for testing
  if (RetVal) {
    RetVal = FALSE;

    // FOR each channel:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      RetVal = IsControllerValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller);

      // IF the controller is enabled for validation THEN exit the for loop
      if (RetVal) {
        break;
      }
    } // end for each controller
  } // end if the socket is enabled for testing

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (IsSocketValEnabled RetVal=%u)\n", RetVal);
#endif

  return RetVal;
}

/**
  This function is used to get the number of rank combinations for a given channel.

  @param[in, out]  SsaServicesHandle       Pointer to SSA services.
  @param[in]       pSystemInfo             Pointer to system information structure.
  @param[in]       pMemCfg                 Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg        Pointer to generic memory point test configuration structure.
  @param[in]       Socket                  Zero based socket number.
  @param[in]       Controller              Zero based controller number.
  @param[in]       Channel                 Zero based channel number.

  @retval  RankCombinationCnt
**/
UINT8 GetChnlRankCombinationsCnt(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel)
{
  DIMM_RANK_PAIR DimmRankA, DimmRankB;
  UINT8 RankCombinationsCnt;

#if SUPPORT_FOR_TURNAROUNDS
        // IF this channel is configured to do rank-to-rank turnarounds but
        // can't do so
        if (pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] == Turnarounds) {

          DimmRankIterationTurnaround(SsaServicesHandle,pSystemInfo, pMemCfg,pMemPointTestCfg,
            Socket, Controller, Channel, &RankCombinationsCnt, FALSE, &DimmRankA, &DimmRankB);
        }
        else
#endif // SUPPORT_FOR_TURNAROUNDS
        {
          DimmRankIterationSingles(SsaServicesHandle,pSystemInfo, pMemCfg,pMemPointTestCfg,
            Socket, Controller, Channel, &RankCombinationsCnt, FALSE, &DimmRankA, &DimmRankB);
        }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  (GetChnlRankCombinationsCnt Socket=%u Controller=%u Channel=%u "
          "RankCombinationsCnt=%u)\n",
          Socket, Controller, Channel, RankCombinationsCnt);
#endif

  return RankCombinationsCnt;
} // end function GetChnlRankCombinationsCnt

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
  IN BOOLEAN IsDdrT)
{
  BOOLEAN RetVal = FALSE;
  UINT8 RankCombinationsCnt;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CanChannelDoTurnarounds(IoLevel=%u Socket=%u Controller=%u Channel=%u IsDdrT=%u)\n",
    IoLevel, Socket, Controller, Channel, IsDdrT);
#endif

  RankCombinationsCnt = GetChnlRankCombinationsCnt(SsaServicesHandle, pSystemInfo, pMemCfg, 
    pMemPointTestCfg, Socket, Controller, Channel);

  if (RankCombinationsCnt > 0 ) {
    RetVal = TRUE;
  }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (CanChannelDoTurnarounds RetVal=%u)\n", RetVal);
#endif

  return RetVal;
} // end function CanChannelDoTurnarounds
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
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller, Channel;
  UINT8 RankCombinationsCnt;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "GetRankCombinationsCnts(IoLevel=%u IsDdrT=%u)\n", IoLevel, IsDdrT);
#endif

  if (pRankCombinationCnts) {
    bssa_memset(pRankCombinationCnts, 0, sizeof(*pRankCombinationCnts));
  }

  if (pMaxRankCombinationCnt) {
    *pMaxRankCombinationCnt = 0;
  }

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not enabled for testing THEN skip it
        if (!IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, Socket ,Controller, Channel)) {
          continue;
        }

        RankCombinationsCnt = GetChnlRankCombinationsCnt(SsaServicesHandle, pSystemInfo, pMemCfg, 
          pMemPointTestCfg, Socket, Controller, Channel);

        if (pRankCombinationCnts) {
          (*pRankCombinationCnts)[Socket][Controller][Channel] = \
            RankCombinationsCnt;
        }

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  (GetRankCombinationsCnts Socket=%u Controller=%u Channel=%u "
          "RankCombinationsCnt=%u)\n",
          Socket, Controller, Channel, RankCombinationsCnt);
#endif

        if (pMaxRankCombinationCnt) {
          if (RankCombinationsCnt > *pMaxRankCombinationCnt) {
            *pMaxRankCombinationCnt = RankCombinationsCnt;
          }
        }
      } // end for each channel
    } // end for each controller
  } // end for each socket

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  if (pMaxRankCombinationCnt) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  (GetRankCombinationsCnts MaxRankCombinationCnt=%u)\n",
      *pMaxRankCombinationCnt);
  }
#endif

  return 0;
} // end function GetRankCombinationsCnts

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
  IN CONST LANE_FAIL_STATS *pLaneFailStats)
{
  UINT8 Socket, Controller, Channel;
  BOOLEAN AnyLaneFailed = FALSE;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "GetAnyLaneFailed()\n");
#endif

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

        // IF any lane failed on this channel:
        if ((*pLaneFailStats)[Socket][Controller][Channel].AnyLaneFailed) {
          AnyLaneFailed = TRUE;
          break;
        }
      } // end for each channel

      if (AnyLaneFailed) {
        break;
      }
    } // end for each controller

    if (AnyLaneFailed) {
      break;
    }
  } // end for each socket

  return AnyLaneFailed;
} // end function GetAnyLaneFailed

/**
  This helper function returns the index of the given channel based on the
  memory test config.  The index is used to compress the per channel test data.

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
  UINT8 Channel)
{
  UINT8 SocketIdx, ControllerIdx, ChannelIdx;
  UINT8 Idx = 0;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "GetTestChnlIndex(Socket=%u Controller=%u Channel=%u)\n", Socket, Controller, Channel);
#endif

  // FOR each socket:
  for (SocketIdx = 0; SocketIdx <= Socket; SocketIdx++) {
    // FOR each controller:
    for (ControllerIdx = 0; ControllerIdx < MAX_CONTROLLER_CNT; ControllerIdx++) {
      // FOR each channel:
      for (ChannelIdx = 0; ChannelIdx < MAX_CHANNEL_CNT; ChannelIdx++) {
        // IF this channel is enabled for testing
        if (IsChannelTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
          pMemPointTestCfg, SocketIdx, ControllerIdx, ChannelIdx)){
          Idx++;
        }

        // IF this is the channel we're looking for THEN exit to for loop now
        if ((SocketIdx == Socket) && (ControllerIdx == Controller) &&
          (ChannelIdx == Channel)) {
          break;
        }
      } // end for each channel

      // IF this is the controller we're looking for THEN exit to for loop now
      if ((SocketIdx == Socket) && (ControllerIdx == Controller)) {
        break;
      }
    } // end for each controller

    // IF this is the socket we're looking for THEN exit to for loop now
    if (SocketIdx == Socket) {
      break;
    }
  } // end for each socket

  Idx = Idx - 1; // zero base

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  (GetTestChnlIndex Idx=%u)\n", Idx);
#endif

  return Idx;
} // end function GetTestChnlIndex

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
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg)
{
  UINT8 Socket, Controller, Channel, Dimm;
  BOOLEAN HasDdrtDevices = FALSE;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is not to be tested THEN skip it
    if ((pMemPointTestCfg->SocketBitmask & (1 << Socket)) == 0) {
      continue;
    }

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not populated THEN skip it
      if ((pMemPointTestCfg->ControllerBitmasks[Socket] & (1 << Controller)) == 0) {
        continue;
      }

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not populated THEN skip it
        if ((pMemPointTestCfg->ChannelValBitmasks[Socket][Controller] &
          (1 << Channel)) == 0) {
            continue;
        }

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          // IF this DIMM is not populated THEN skip it
          if (((pMemPointTestCfg->DimmABitmasks[Socket][Controller][Channel] | pMemPointTestCfg->DimmBBitmasks[Socket][Controller][Channel]) &
            (1 << Dimm)) == 0) {
              continue;
          }

          if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] ==
            SsaMemoryDdrT) {
              HasDdrtDevices = TRUE;
              break;
          }
        } // end for each DIMM

        if (HasDdrtDevices) {
          break;
        }
      } // end for each channel

      if (HasDdrtDevices) {
        break;
      }
    } // end for each controller

    if (HasDdrtDevices) {
      break;
    }
  } // end for each socket

  return HasDdrtDevices;
} // end function HasDdrtDevices
#endif // SUPPORT_FOR_DDRT

#endif  // SSA_FLAG

// end file MemPointTest.c
