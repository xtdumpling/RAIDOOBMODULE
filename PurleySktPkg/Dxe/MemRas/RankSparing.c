/**
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
 **/
/**

 Copyright (c) 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.


 @file RankSparing.c

 This file implements Rank Sparing code

 **/

#include "RankSparing.h"
#include "AdddcSparing.h"
#include "SparingEngine.h"
#include "PatrolScrub.h"
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
#include <Library\AmiErrReportingLib\AmiErrReportingLib.h>
#include <OemRasLib\OemRasLib.h>
extern UEFI_MEM_ERROR_RECORD  *gMemRasMemoryErrorRecord;
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support
extern ADDDC_STATE_STRUCT  mAdddcStateStruct[MC_MAX_NODE][MAX_CH];
//
// Spare Rank Status
//

static
RANK_STATUS_TYPE mRankStatus[MC_MAX_NODE][MAX_MC_CH][MAX_RANK_CH];

/**

  Parse rank status.

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param Rank -- logical ran

  @retval none

 **/
VOID
EFIAPI
ParseRankState(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  IN UINT8 Rank
  )
{
  RANK_STATUS_TYPE state;
  UINT32 rankstate;

  state.Data = mRankStatus[McNodeId][Ch][Rank].Data;
  RAS_DEBUG((4, "MC:%d   Ch:%d  Rank:%d  ----  ", McNodeId, Ch, Rank));
  if (state.Bits.rank_exist) RAS_DEBUG((4, "rank exist,   "));

  if (state.Bits.rank_fail) RAS_DEBUG((4, "fail,   "));

  if (state.Bits.non_spare_rank) {
    RAS_DEBUG((4, "non-spare rank state:   "));
    rankstate = state.Bits.non_spare_status;
    switch (rankstate) {

    case NONSPARESTATUS_SCRUBNEXT:
      RAS_DEBUG((4, "scrub next"));
      break;
    case NONSPARESTATUS_SCRUBBING:
      RAS_DEBUG((4, "scrubbing"));
      break;
    case NONSPARESTATUS_DOSPARECOPY:
      RAS_DEBUG((4, "do spare copying"));
      break;
    case NONSPARESTATUS_SPARECOPYDONE:
      RAS_DEBUG((4, "spare copy done"));
      break;
    case NONSPARESTATUS_TRY_TAG:
      RAS_DEBUG((4, "try do tag"));
      break;
    default:
      RAS_DEBUG((4, "wrong state"));
      break;
    }
  }

  if (state.Bits.spare_rank) {
    RAS_DEBUG((4, "spare rank state:   "));
    rankstate = state.Bits.spare_rank_status;
    switch (rankstate) {
    case SPARERANKSTATUS_AVAILABLE:
      RAS_DEBUG((4, "available"));
      break;
    case SPARERANKSTATUS_SCRUBBING:
      RAS_DEBUG((4, "scrubbing"));
      break;
    case SPARERANKSTATUS_INPROGRESS:
      RAS_DEBUG((4, "do spare copying (in progress)"));
      break;
    case SPARERANKSTATUS_NOTAVAILALBE:
      RAS_DEBUG((4, "not available"));
      break;
    default:
      RAS_DEBUG((4, "wrong state"));
      break;
    }
  }

  RAS_DEBUG((4, "\n "));
}

/**

  get specific rank state

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param LogicalRank -- logical rank index.

  @retval rank state

  **/
RANK_STATUS_TYPE
EFIAPI
GetRankState(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  IN UINT8 LogicalRank
  )
{
  return mRankStatus[McNodeId][Ch][LogicalRank];
}

/**

  Checks if the given rank is a spare rank and returns true if it is

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param LogicalRank -- logical rank index.

  @retval spare rank exists or not

  **/
BOOLEAN
EFIAPI
IsSpareRankAvailable(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  IN UINT8 LogicalRank
  )
{
  return (mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_exist == 1);
}

/**

  set  rank new status. New status is  should downgrade, not upgrade here.

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param LogicalRank -- logical rank index.
  @param SpareStatus -- spare rank new status.

  @retval none

  **/
VOID
EFIAPI
SetRankState(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  IN UINT8 LogicalRank,
  IN UINT8 NewState
  )
{
  RAS_DEBUG((4, "SetRankState \n"));
  ParseRankState(McNodeId, Ch, LogicalRank);

  if (mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_exist &&
    mRankStatus[McNodeId][Ch][LogicalRank].Bits.spare_rank &&
    (mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_fail == 0)) {
    mRankStatus[McNodeId][Ch][LogicalRank].Bits.spare_rank_status = NewState;
  }

  if (mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_exist &&
    mRankStatus[McNodeId][Ch][LogicalRank].Bits.non_spare_rank) {
    mRankStatus[McNodeId][Ch][LogicalRank].Bits.non_spare_status = NewState;
  }

  ParseRankState(McNodeId, Ch, LogicalRank);

  return;
}

/**

  check if current rank is specific state of  rank.

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param LogicalRank -- logical rank.
  @param CheckState  -- check state

  @retval TRUE  -- available state of spare rank.
  @retval FALSE -- not available state of spare rank.

  **/
BOOLEAN
EFIAPI
CheckRankState(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  IN UINT8 LogicalRank,
  IN UINT8 CheckState
  )
{
  BOOLEAN IsCheckState = FALSE;

  if (mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_exist &&
    mRankStatus[McNodeId][Ch][LogicalRank].Bits.spare_rank &&
    (mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_fail == 0)) {
    if (mRankStatus[McNodeId][Ch][LogicalRank].Bits.spare_rank_status == CheckState) {
      IsCheckState = TRUE;
    }
  }

  if (mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_exist &&
    mRankStatus[McNodeId][Ch][LogicalRank].Bits.non_spare_rank) {
    if (mRankStatus[McNodeId][Ch][LogicalRank].Bits.non_spare_status == CheckState) {
      IsCheckState = TRUE;
    }
  }

  ParseRankState(McNodeId, Ch, LogicalRank);
  RAS_DEBUG((4, " check rank state : check state%d   status:%d\n", CheckState, IsCheckState));

  return IsCheckState;
}

/**

  check if current channel has specific state  rank,

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param state -- find state status
  @param pLogicalRank -- pointer to found rank

  @retval TRUE  -- found.
  @retval FALSE -- no found.
  @retval Status

  **/
BOOLEAN
EFIAPI
CheckRankStatePerCh(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  IN UINT8 state,
  IN OUT UINT8 * pFoundRank
  )
{
  BOOLEAN RankStatusExist = FALSE;
  UINT8 i;

  for (i = 0; i < MAX_RANK_CH; i++) {
    if (CheckRankState(McNodeId, Ch, i, state)) {
      RankStatusExist = TRUE;
      if (pFoundRank != NULL) {
        *pFoundRank = i;
      }
      break;
    }
  }

  RAS_DEBUG((4, "CheckRankStatePerCh --spare rank specific state exist per channel?   MC:%d   Channel:%d  State:%d  exist:%d (1-- true, 0-- false)  \n", McNodeId, Ch, state, RankStatusExist));

  return RankStatusExist;
}

/**

  check if current MC has specific state rank,

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param state -- find state status
  @param pLogicalRank -- pointer to found rank

  @retval TRUE  -- found.
  @retval FALSE -- no found.
  @retval Status

  **/
BOOLEAN
EFIAPI
CheckRankStatePerMC(
  IN UINT8 McNodeId,
  IN UINT8 state,
  IN OUT UINT8 * pFoundCh,
  IN OUT UINT8 * pFoundRank
  )
{
  BOOLEAN MCStatusExist = FALSE;
  UINT8 i;

  for (i = 0; i < MAX_MC_CH; i++) {
    MCStatusExist = CheckRankStatePerCh(McNodeId, i, state, pFoundRank);
    if (MCStatusExist) {
      if (pFoundCh) {
        *pFoundCh = i;
      }
      break;
    } //if
  } //for

  RAS_DEBUG((4, "CheckRankStatePerMC--rank specific state exist per MC?   MC:%d     State:%d  exist:%d  \n", McNodeId, state, MCStatusExist));

  return MCStatusExist;
}

/**

  get the lowest priority spare rank.

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param LogicalRank -- pointer to logical rank.

  @retval EFI_STATUS

  Note: invoker should call IsAvailSpareRankExist first.

  **/
VOID
EFIAPI
GetLowestPrioritySpareRank(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  OUT UINT8* pLogicalRank
  )
{
  UINT8 i;
  UINT8 LowestRank = 0;
  UINT32 LowestSeq = 0;
  UINT32 Seq;

  for (i = 0; i < MAX_RANK_CH; i++) {
    if (CheckRankState(McNodeId, Ch, i, SPARERANKSTATUS_AVAILABLE)) {
      Seq = mRankStatus[McNodeId][Ch][i].Bits.spare_rank_sequence;
      if (Seq >= LowestSeq) {
        LowestRank = i;
        LowestSeq = Seq;
      }
    }
  }

  *pLogicalRank = LowestRank;

  RAS_DEBUG((4, "GetLowestPrioritySpareRank--  MC:%d   Ch:%d  low priority rank:%d \n", McNodeId, Ch, LowestRank));

  return;
}

/**

  Update scratch pad register after spare rank is mark dirty.

  SCRATCHPAD2 in each DDR channel
  bits 0-7     one-hot encoding of channel's spare ranks, each bit position represents a logical rank id
  bits 8       sparing enabled

  @param NodeId  -- MC id
  @param Ch -- when value is 0xff, do whole MC; other do specified channel.
  @retval None

  **/
VOID
UpdateScratchPadReg(
  IN UINT8 McNodeId,
  IN UINT8 Ch
  )
{
  UINT8 Skt;
  UINT8 SktCh;
  UINT8 i;
  UINT32 ScratchPadValue;
  SCRATCHPAD2_MC_MAIN_STRUCT ChScratchPad2;

  ScratchPadValue = 0;
  for (i = 0; i < MAX_RANK_CH; i++) {
    if (CheckRankState(McNodeId, Ch, i, SPARERANKSTATUS_AVAILABLE)) {
      ScratchPadValue |= (1 << i);
    }
  }

  Skt = NODE_TO_SKT(McNodeId);
  SktCh = NODECH_TO_SKTCH(McNodeId, Ch);
  ChScratchPad2.Data = mCpuCsrAccess->ReadCpuCsr(Skt, SktCh, SCRATCHPAD2_MC_MAIN_REG);
  RAS_DEBUG((4, "UpdateScratchPadReg - SktCh=%d SCRATCHPAD2=%0x\n", SktCh, ChScratchPad2.Data));

  ChScratchPad2.Data &= 0xFFFFFF00; // clear LSB
  ChScratchPad2.Data |= ScratchPadValue;

  // if no more spare ranks available, clear enable flag
  if (ScratchPadValue == 0) {
    ChScratchPad2.Data &= ~(UINT32)(1 << MAX_RANK_CH);
  }

  mCpuCsrAccess->WriteCpuCsr(Skt, SktCh, SCRATCHPAD2_MC_MAIN_REG, ChScratchPad2.Data);
  RAS_DEBUG((4, "UpdateScratchPadReg - SktCh=%d SCRATCHPAD2=%x\n", SktCh, ChScratchPad2.Data));
  return;
}

/**

  Mark the rank as dirty flag.
  1. set mSpareRankStatus as RankSpareNotAvailable if it is a spare rank
  2. add it to mFailedRank

  @param McNodeId -- Memory controller ID
  @param Ch -- channel number.
  @param LogicalRank -- logical rank.

  @retval TRUE  -- available spare rank.
  @retval FALSE -- not an spare rank.
  @retval Status

  **/
VOID
EFIAPI
MarkRankDirtyFlag(
  IN UINT8 McNodeId,
  IN UINT8 Ch,
  IN UINT8 LogicalRank
  )
{
  mRankStatus[McNodeId][Ch][LogicalRank].Bits.rank_fail = 1;

  RAS_DEBUG((4, "MarkRankDirtyFlag -  MC:%d  channel:%d,  rank:%d is dirty\n", McNodeId, Ch, LogicalRank));
  return;
}

/**

  do spare copy operation for patrol scrub is disabled case.

  @param NodeId -- Memory controller ID
  @param Ch -- channel number.
  @param Rank -- logical rank.
  @param type -- identify error source type.

  @retval none

  **/
VOID
EFIAPI
DoSpareCopy(
  IN UINT8 NodeId,
  IN UINT8 Ch,
  IN UINT8 Rank,
  IN SPARE_COPY_TYPE type,
  IN SPARING_MODE SparingMode,
  IN UINT8  RegionNum
  )
{
  UINT32 errStatus;
  UINT8 NextSpareRank;

  RAS_DEBUG((1, " [Rank Sparing] DoSpareCopy Ch:%d\n", Ch));

  if (AcquireSparingEngine(NodeId) == EFI_SUCCESS) {

    if (type == EVENT_SPARE_COPY) {
      RASDEBUG((DEBUG_INFO, "[RAS event] CE event on ch:%d  rank:%d\n", Ch, Rank));
      if (!CheckRankStatePerCh(NodeId, Ch, SPARERANKSTATUS_AVAILABLE, NULL)) {
        RAS_DEBUG((1, "[RAS event] no available rank on channel:%d\n", Ch));
        return;
      }
    } else {
      RAS_DEBUG((1, "[RAS new] do spare copying, CE on ch:%d  rank:%d\n", Ch, Rank));
    }
    GetLowestPrioritySpareRank(NodeId, Ch, &NextSpareRank);
    //
    //do spare copy operation
    //
    StartSparing(NodeId, Rank, NextSpareRank, Ch, SparingMode, RegionNum);
    RASDEBUG((DEBUG_INFO, "DoSpareCopy: Start Sparing on NodeId:%d ch:%d  rank:%d\n", NodeId, Ch, Rank));
    //
    //update progress info
    //
    RASDEBUG((DEBUG_INFO, "\t DoSpareCopy: Calling update events progress and pending status\n"));
    GetOrSetErrorHandlingStatus(NodeId, GetEventPendingStatus, &errStatus);
    if (!errStatus) {
      errStatus = 1;
      GetOrSetErrorHandlingStatus(NodeId, UpdateEventPendingStatus, &errStatus); // Save the error bitmap for internal use
    }
    errStatus = 1;
    GetOrSetErrorHandlingStatus(NodeId, UpdateEventProgressStatus, &errStatus);

    SetRankState(NodeId, Ch, NextSpareRank, SPARERANKSTATUS_INPROGRESS);
    SetRankState(NodeId, Ch, Rank, NONSPARESTATUS_DOSPARECOPY);
  } else {
    RAS_DEBUG((1, "[RAS NEW] spare engine is busy!\n"));
  }
}

/**

  Initialize all rank sparing related data structures
  This routine should be called only once during boot

  @param None
  @retval Status

  **/
EFI_STATUS
EFIAPI
InitializeRankSparing()
{
  UINT8 nodeId;
  UINT8 ch;
  UINT8 rankIndex;
  UINT8 rank;

  //
  // It don't init non-spare rank info here, init it at error handler in case error happen.
  //
  for (nodeId = 0; nodeId < MC_MAX_NODE; nodeId++) {
    for (ch = 0; ch < MAX_MC_CH; ch++) {

      for (rankIndex = 0; rankIndex < (MAX_SPARE_RANK); rankIndex++) {
        mRankStatus[nodeId][ch][rankIndex].Data = 0;
      }
      for (rankIndex = 0; rankIndex < (MAX_SPARE_RANK); rankIndex++) {
        if (mSystemMemoryMap->Socket[NODE_TO_SKT(nodeId)].imcEnabled[NODE_TO_MC(nodeId)] &&
          mSystemMemoryMap->Socket[NODE_TO_SKT(nodeId)].ChannelInfo[NODECH_TO_SKTCH(nodeId, ch)].Enabled &&
          (mSystemMemoryMap->RasModesEnabled & RK_SPARE) &&
          (mSystemMemoryMap->Socket[NODE_TO_SKT(nodeId)].ChannelInfo[NODECH_TO_SKTCH(nodeId, ch)].SpareLogicalRank[rankIndex] != 0xFF)) {
          rank = mSystemMemoryMap->Socket[NODE_TO_SKT(nodeId)].ChannelInfo[NODECH_TO_SKTCH(nodeId, ch)].SpareLogicalRank[rankIndex];
          mRankStatus[nodeId][ch][rank].Data = 0;
          mRankStatus[nodeId][ch][rank].Bits.rank_exist = 1;
          mRankStatus[nodeId][ch][rank].Bits.spare_rank = 1;
          mRankStatus[nodeId][ch][rank].Bits.spare_rank_sequence = rankIndex;
          RAS_DEBUG((1, "[RAS]       spare rank from HOB   MC:%d   channel:%d   spare rank index:%d   logical rank:%d\n", nodeId, ch, rankIndex, rank));
        }
      }  //rank loop
    } // ch loop
  } // node loop
  return EFI_SUCCESS;
}

/**

 Check and handle ranks sparing done event

 @param NodeId    - Memory controller ID
 @param *IsEventHandled - ptr to a buffer to hold event handled status

 @retval status - EFI_SUCCESS if no failures
 @retval non-zero error code for failures

 IsEventHandled - TRUE -  event was handled
 FALSE - event not handled

 **/
EFI_STATUS
EFIAPI
CheckAndHandleRankSparingDone(
  IN UINT8 NodeId,
  OUT BOOLEAN *IsEventHandled
  )
{
  EFI_STATUS status = EFI_SUCCESS;
  UINT8 ch;
  BOOLEAN rankSpareInProgress;
  UINT32 errStatus;
  UINT8 spareProgressRank = 0;
  UINT8 FoundedCh;
  UINT8 FoundedPendingRank;
  UINT8 SourceCh;
  UINT8 SourceRank;
  UINT8 RegionNum = 0;

  *IsEventHandled = FALSE;
  //
  // Check if there is any rank sparing event already in progress
  // At a time only one rank sparing operation can be in progress
  // for a given node.
  //
  rankSpareInProgress = CheckRankStatePerMC(NodeId, SPARERANKSTATUS_INPROGRESS, &ch, &spareProgressRank);

  RAS_DEBUG((1, " [Rank Sparing] CheckAndHandleRankSparingDone in progress:%d\n", rankSpareInProgress));
  if (rankSpareInProgress) {
    //
    // Check if copy operation is complete
    //
    status = IsCopyComplete(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
    switch (status) {
    case EFI_SUCCESS:
      RAS_DEBUG((1, "[RAS pending event] spare copy complete! \n"));
      //
      //update status
      //
      SetRankState(NodeId, ch, spareProgressRank, SPARERANKSTATUS_NOTAVAILALBE);
      if (CheckRankStatePerMC(NodeId, NONSPARESTATUS_DOSPARECOPY, &SourceCh, &SourceRank)) {
        if (SourceCh != ch) {
          RAS_DEBUG((1, "error! the spare copy ch:%d is not equal to found ch:%d", ch, SourceCh));
        }
        SetRankState(NodeId, SourceCh, SourceRank, NONSPARESTATUS_SPARECOPYDONE);
      }
      //
      //update scratch pad register here
      //
      UpdateScratchPadReg(NodeId, SourceCh);
      //
      // Clear error count registers in CPU
      //
      ClearErrorsForThisRank(NodeId, ch, spareProgressRank);

      ReleaseSparingEngine(NodeId);

      RASDEBUG((DEBUG_INFO, "\t CheckAndHandleRankSparingDone: Calling update events progress and pending status\n"));
      errStatus = 0;
      GetOrSetErrorHandlingStatus(NodeId, UpdateEventProgressStatus, &errStatus);
      GetOrSetErrorHandlingStatus(NodeId, UpdateEventPendingStatus, &errStatus);
      //
      //check if any pending event on this MC. In case any pending event exist and available spare rank exist,
      //do spare copying again.
      //
      if (CheckRankStatePerMC(NodeId, NONSPARESTATUS_SCRUBNEXT, &FoundedCh, &FoundedPendingRank)) {
        RAS_DEBUG((1, "[RAS pending event] there is a pending CE event exist  Ch:%d   Rank:%d\n", FoundedCh, FoundedPendingRank));
        //
        //check if available rank exist
        //
        if (CheckPatrolScrubEnable()) {
          ScrubNextRank(NodeId, FoundedCh, FoundedPendingRank, NON_SPARE_CE_EVENT);
        } else {
          DoSpareCopy(NodeId, FoundedCh, FoundedPendingRank, EVENT_SPARE_COPY, RankSparing, RegionNum);
        }
      }
      *IsEventHandled = TRUE;
      status = EFI_SUCCESS;
      break;

    case EFI_NOT_READY:   // copy operation in progress
      RAS_DEBUG((1, "[RAS pending event] spare copy not complete! Copy in progress for Node[%d].CH[%d]\n", NodeId, ch));
      status = EFI_SUCCESS;
      *IsEventHandled = FALSE;
      break;

    default:              // Error in operation
      *IsEventHandled = FALSE;
      break;
    }
  }
  return status;
}

/**

  Check for new rank sparing events and handle

  @param NodeId    - Memory controller ID
  @param *IsEventHandled - ptr to a buffer to hold event handled status

  @retval status - EFI_SUCCESS if no failures
  @retval non-zero error code for failures

  @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

  **/
EFI_STATUS
EFIAPI
CheckAndHandleNewRankSparingEvent(
IN UINT8 NodeId,
OUT BOOLEAN * IsEventHandled
)
{
  EFI_STATUS status = EFI_SUCCESS;
  UINT8 ch;
  UINT8 rank;
  UINT8 RegionNum = 0;

  *IsEventHandled = FALSE;

  RAS_DEBUG((1, "[Rank Sparing] CheckAndHandleNewRankSparingEvent node=%d\n", NodeId));

  for (ch = 0; ch < MAX_MC_CH; ch++) {
    //
    // If this ch doesn't have spare rank, skip to next ch
    //
    if (!CheckRankStatePerCh(NodeId, ch, SPARERANKSTATUS_AVAILABLE, NULL)) {
      continue;
    }
    //
    // Check if there is any rank in this channel
    // that has overflow bit set
    //
    for (rank = 0; (rank < MAX_RANK_CH); rank++) {
      if (IsErrorExcdForThisRank(NodeId, ch, rank)) {
        break;
      }
    }

    if (rank != MAX_RANK_CH) {
      RAS_DEBUG((1, "[RAS CE new]  found a rank error on MC:%d   channel:%d,  rank:%d\n", NodeId, ch, rank));
      //
      // this error happen on spared rank, this is from patrol scrub on spare ranks.
      //
      if (CheckRankState(NodeId, ch, rank, SPARERANKSTATUS_SCRUBBING)) {
        RAS_DEBUG((1, "[RAS CE new]  this is an spare rank CE\n"));
        //
        // mark this rank as "dirty"
        //
        MarkRankDirtyFlag(NodeId, ch, rank);
        //
        // move to next rank scrub......
        //
        ScrubNextRank(NodeId, ch, rank, SPARE_CE);
      } else {
        RAS_DEBUG((1, "[RAS CE new]  this is an non-spare rank CE\n"));
        //
        // init non-spare rank.
        //
        if (CheckRankState(NodeId, ch, rank, NONSPARESTATUS_SCRUBNEXT) ||
          CheckRankState(NodeId, ch, rank, NONSPARESTATUS_SCRUBBING) ||
          CheckRankState(NodeId, ch, rank, NONSPARESTATUS_DOSPARECOPY)) {
          //
          //during overflow rank is actually do scrubbing,  spare copy , or wait, another overflow  come in again.
          //it is to prevent re-entry.
          //
          *IsEventHandled = TRUE;
          return status;
        }

        mRankStatus[NodeId][ch][rank].Data = 0;
        mRankStatus[NodeId][ch][rank].Bits.rank_exist = 1;
        mRankStatus[NodeId][ch][rank].Bits.non_spare_rank = 1;
        mRankStatus[NodeId][ch][rank].Bits.non_spare_status = NONSPARESTATUS_SCRUBNEXT;

        //
        //mark this rank as "dirty"
        //
        MarkRankDirtyFlag(NodeId, ch, rank);

        //
        //check patrol scrub if enabled.
        //
        if (CheckPatrolScrubEnable()) {
          //
          //move to next rank scrub......
          //
          ScrubNextRank(NodeId, ch, rank, NON_SPARE_CE);
        } else {
          DoSpareCopy(NodeId, ch, rank, NEW_SPARE_COPY, RankSparing, RegionNum);
        }
      } // CheckRankState
      *IsEventHandled = TRUE;

      return status;
    } // // (rank != MAX_RANK_CH)
  } // for(ch)
  return status;
}


/**
  Check if Rank Sparing is possible for this node and
  do Rank Sparing

  @param NodeId    - Memory controller ID
  @param EventType - NEW_EVENTS or EVENTS_IN_PROGRESS
  @param *IsEventHandled - ptr to buffer to hold event handled status

  @retval status - EFI_SUCCESS if no failures
  @retval non-zero error code for failures

  @retval IsEventHandled - TRUE -  event was handled
  @retval FALSE - event not handled

  **/

EFI_STATUS
EFIAPI
CheckAndHandleRankSparing(
IN UINT8 NodeId,
IN EVENT_TYPE EventType,
OUT BOOLEAN * IsEventHandled
)
{
  EFI_STATUS status = EFI_SUCCESS;

  RASDEBUG((DEBUG_INFO, "<CheckAndHandleRankSparing>\n"));
  if (mSystemMemoryMap->RasModesEnabled & RK_SPARE) {
// APTIOV_SERVER_OVERRIDE_RC_START : Oem Hooks Support
    if ((EventType == NEW_EVENTS) || (EventType == EVENTS_IN_PROGRESS)) {
        ZeroMem (gMemRasMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
        FillMemRecordBeforeHandling(NodeId, gMemRasMemoryErrorRecord, ERROR_EVENT_RANK_SPARING);
    }
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support
    switch (EventType) {
    case NEW_EVENTS:
      status = CheckAndHandleNewRankSparingEvent(NodeId, IsEventHandled);
     // APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
      OemReportStatusCode(SPARE_COPY_START, gMemRasMemoryErrorRecord);
     // APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support
      break;

    case EVENTS_IN_PROGRESS:
      status = CheckAndHandleRankSparingDone(NodeId, IsEventHandled);
     // APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
      OemReportStatusCode(SPARE_COPY_COMPLETE, gMemRasMemoryErrorRecord);
     // APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support
      break;

    default:
      status = EFI_UNSUPPORTED;     // Invalid event type
      break;
    }//Switch
  } else {
    RASDEBUG((DEBUG_INFO, "\t Rank Sparing Not Enabled Exiting\n"));
    *IsEventHandled = FALSE;
    status = EFI_SUCCESS;
  }
  RASDEBUG((DEBUG_INFO, "/ <CheckAndHandleRankSparing>\n"));
  return status;
}



