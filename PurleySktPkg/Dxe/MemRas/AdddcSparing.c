/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AdddcSparing.c

Abstract:

  Implements double Bank

--*/

#include <Protocol/MemRasProtocol.h>
#include "MemRas.h"
#include "AdddcSparing.h"
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks Support
#include <Library\AmiErrReportingLib\AmiErrReportingLib.h>
#include <OemRasLib\OemRasLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support

extern SDDC_PLUSONE_STATUS   mSddcPlusOneStatus[MC_MAX_NODE][MAX_MC_CH][MAX_RANK_CH];
extern SPARING_ENGINE_STATUS mSpareEngineStatus[MC_MAX_NODE];
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
extern UEFI_MEM_ERROR_RECORD  *gMemRasMemoryErrorRecord;
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support
UINT8                        mMaxRegion;


#define SPARE_DEVICE_NUMBER   17
#define NUMBER_OF_BANK_GROUPS 4
#define REGION_NUM_AUTO       0xFF

UINT32 mAdddcRegionoffset = ADDDC_REGION0_CONTROL_MC_MAIN_REG;


SPARING_EVENT_STRUCT mAdddcSparingEventStruct[MC_MAX_NODE][MAX_CH];
ADDDC_STATE_STRUCT   mAdddcStateStruct[MC_MAX_NODE][MAX_CH];
UINT16               mSkipRankBitmap[MC_MAX_NODE][MAX_MC_CH];  //Ranks to Skip during selection of Non-Failed Rank

extern SDDC_PLUSONE_STATUS mSddcPlusOneStatus[MC_MAX_NODE][MAX_MC_CH][MAX_RANK_CH];

//Index1 will give the Rank for which we are finding the Farthest rank and Index 2 will choose the Farthest Rank in
//descending order of preference. If we want the Rank Farthest away for Rank 0 the first Attempt will be to try
// FarthestRankArray[0][0] this will be 3 and that is the farthest rank from 0. If 3 is not available we choose 2,1 next
UINT8 FarthestRankArray[MAX_RANK_DIMM][MAX_RANK_DIMM] = {{3,2,1,0},
                                                         {3,2,0,1},
                                                         {0,1,3,2},
                                                         {0,1,2,3}};
UINT32  mRankErrorThreshold;

VOID
InitializeAdddcErrInj(
    )
{
  UINT8  Socket;
  UINT8  Mc;
  UINT8  SktCh;
  UINT8  ddrCh;
  ADDDC_ERR_INJ_MCDDC_DP_STRUCT AdddcErrInj;

  for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Mc = 0; Mc < MAX_IMC; Mc++) {

      if (mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]) {
        for(ddrCh = 0; ddrCh < MAX_MC_CH; ddrCh++) {
          SktCh =  NODECH_TO_SKTCH(Mc, ddrCh);
          if (mSystemMemoryMap->Socket[Socket].ChannelInfo[SktCh].Enabled) {
            AdddcErrInj.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr( Socket, SktCh, ADDDC_ERR_INJ_MCDDC_DP_REG);
            AdddcErrInj.Bits.primary0 = 1;
            AdddcErrInj.Bits.buddy0   = 1;
            AdddcErrInj.Bits.primary1 = 1;
            AdddcErrInj.Bits.buddy1   = 1;
            mCpuCsrAccess->WriteCpuCsr( Socket, SktCh, ADDDC_ERR_INJ_MCDDC_DP_REG, AdddcErrInj.Data );
          }
        }// ddrCh loop
      } // If IMC enabled
    }  //IMC loop
  } //Socket loop
  return;
}


/**

    Initialize all ADDDC related channel level data structures for all enabled nodes
    after mirror failover happened.

    @param None

    @retval EFI_STATUS

**/
EFI_STATUS
InitializeAdddcAfterMirror(
    IN UINT8  McIndex
    )
{
  UINT8  Socket;
  UINT8  Mc;
  UINT8  NodeId;
  UINT8  ddrCh;
  RASDEBUG((DEBUG_INFO,"<InitializeAdddcAfterMirror>\n"));
  if(mSystemMemoryMap->ExRasModesEnabled & ADDDC_EN) {
    for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
      for (Mc = 0; Mc < MAX_IMC; Mc++) {

        if(Mc != McIndex) continue; // Only initialize adddc on MC where mirror failover happened
        NodeId = SKT_TO_NODE(Socket, Mc);
        if (mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]) {
          for(ddrCh = 0; ddrCh < MAX_MC_CH; ddrCh++) {
             mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;
          }// ddrCh loop
        }// If IMC enabled
      }  //IMC loop
    }  //Socket loop
  }
  return EFI_SUCCESS;
}


/**

    Initialize all ADDDC related channel level data structures for all enabled nodes
    After boot, this routine should be called after the node is Online/Offline operation

    @param None

    @retval EFI_STATUS

**/
EFI_STATUS
InitializeAdddcMR(
    )
{
  UINT8  Socket;
  UINT8  Mc;
  UINT8  NodeId;
  UINT8  ddrCh;
  UINT8  index;
  BOOLEAN MirrorEnabledFlag;

  MirrorEnabledFlag = FALSE;
  mRankErrorThreshold = 0;
  switch (mSystemMemoryMap->SystemRasType) {
    case ADVANCED_RAS:
    case FPGA_RAS:
      mMaxRegion = MAX_REGION_EX;
      break;

    case STANDARD_RAS:
    case CORE_RAS:
      mMaxRegion = MAX_REGION_EP;
      break;

    default:
      return EFI_UNSUPPORTED;
  }
  

  if(mSystemMemoryMap->ExRasModesEnabled & ADDDC_ERR_INJ_EN) {
    RASDEBUG((DEBUG_INFO,"Adddc error injection enabled \n"));
    InitializeAdddcErrInj();
  }

  if (mMemRas->SystemMemInfo->RasModesEnabled & CH_ALL_MIRROR) {
    MirrorEnabledFlag = TRUE;
  }

  if(mSystemMemoryMap->ExRasModesEnabled & ADDDC_EN) {
    RASDEBUG((DEBUG_INFO,"Adddc enabled \n"));
    for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
      for (Mc = 0; Mc < MAX_IMC; Mc++) {
        NodeId = SKT_TO_NODE(Socket, Mc);
        if (mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]) {
          for(ddrCh = 0; ddrCh < MAX_MC_CH; ddrCh++) {

            if (mSystemMemoryMap->Socket[NODE_TO_SKT(NodeId)].ChannelInfo[NODECH_TO_SKTCH(NodeId, ddrCh)].Enabled) {
              mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion = mMaxRegion;
              mAdddcSparingEventStruct[NodeId][ddrCh].NumStrikes = 0;
              mAdddcSparingEventStruct[NodeId][ddrCh].SddcPlusOneDone = FALSE;

              for(index=0;index < mMaxRegion;index++) {
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].BuddyBank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].BuddyBankGrp = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].BuddyRank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].BuddySubRank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].ChNum = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].FailedDevice = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].PrimaryBank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].PrimaryBankGrp = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].PrimaryRank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].PrimarySubRank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].RegionNum = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].ReverseRegion = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].SocketNum = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].StrikeNum = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Region[index].Valid = 0;
              }

              for(index=0;index < MAX_STRIKE;index++) {
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].Bank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].BankGroup = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].Device = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].Dimm = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].PrevStrike = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].Rank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].SubRank = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].RegionNum = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].ReverseRemapPrevStrike = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].StrikeNum = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].StrikeType = 0;
                mAdddcSparingEventStruct[NodeId][ddrCh].Strike[index].Valid = 0;
              }

               mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;

               if (MirrorEnabledFlag) {
                 mAdddcStateStruct[NodeId][ddrCh] = AdddcUnavailable;
               }

            }
          }// ddrCh loop
        } // If IMC enabled
      }  //IMC loop
    } //Socket loop
  }
  return EFI_SUCCESS;
}

/**

    Call to find if the current Rank or Bank was used previously as a buddy for another rank

    @param NodeId       - Memory controller ID
    @param ddrCh        - Channel ID
    @param StrikeNum    - Strike Number
    @param BankValid    - Used to indicate if we are identifying bank or rank

    @retval BOOLEAN     - TRUE if previous rank/bank buddy FALSE if not used as previous rank/bank buddy

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
BOOLEAN
AnyPreviousBuddy(
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 BankValid,
    OUT UINT8 *PrevRegion,
    OUT UINT8 *PrevStrike
    )
{
  UINT8 Bank;
  UINT8 Rank;
  UINT8 BankGroup;
  BOOLEAN status=FALSE;
  UINT8 RegionIndex;

  if(BankValid) {
    Bank      = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;
    BankGroup = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].BankGroup;
    Rank      = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

    for(RegionIndex=0;RegionIndex < mMaxRegion;RegionIndex++) {
      if (mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].Valid) {
      if ((mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].BuddyRank) == Rank) {
          if ( ((mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].BuddyBank)    == Bank) &&
               ((mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].BuddyBankGrp) == BankGroup) ){
            *PrevRegion = RegionIndex;
            *PrevStrike = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].StrikeNum;
            status = TRUE;
            break;
          }
        }
      }
    }
  }
  else {
    Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;
    for(RegionIndex=0;RegionIndex < mMaxRegion;RegionIndex++) {
      if (mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].Valid) {
        if ((mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].BuddyRank) == Rank) {
          *PrevRegion = RegionIndex;
          *PrevStrike = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionIndex].StrikeNum;
          status = TRUE;
          break;
        }
      }
    }
  }
  return status;
}

/**
    Any Previous strike was a bank or a rank

    @param NodeId          - Memory controller ID
    @param ddrCh           - Channel ID
    @param StrikeNum       - The strike Number that we are currently on.
    @param Gran            - if gran is 1, we are looking for a previous bank strike and if its 0 then a rank strike

    @retval status - Boolean for true or false

**/

BOOLEAN
AnyPreviousStrike (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 Gran,
    OUT UINT8 *PrevStrike
    )
{
  UINT8 Bank;
  UINT8 Rank;
  UINT8 StrikeIndex;
  BOOLEAN status=FALSE;
  //if Bank Granularity check if there was a previous strike that was a bank
  RASDEBUG((DEBUG_INFO,"<AnyPreviousStrike>\n"));
  RASDEBUG((DEBUG_INFO,"Current Strike Number %d\n",StrikeNum));

  if(Gran == BANKVALID) {
    Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;

    for (StrikeIndex=0;StrikeIndex<=(StrikeNum-1);StrikeIndex++) {

      if((mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeIndex].StrikeType) == BANK_STRIKE) {
        *PrevStrike = StrikeIndex;
        RASDEBUG((DEBUG_INFO,"Identified previous Bank strike %d\n",*PrevStrike));
        status = TRUE;
        break;
      }
    }
  }
  //if Rank Granularity check if there was a previous strike that was a Rank
  else if (Gran == RANKVALID) {
    Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;
    for(StrikeIndex=0;StrikeIndex<=(StrikeNum-1);StrikeIndex++) {
      if((mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeIndex].StrikeType) == RANK_STRIKE) {
        *PrevStrike = StrikeIndex;
        RASDEBUG((DEBUG_INFO,"Identified previous Rank strike %d\n",*PrevStrike));
        status = TRUE;
        break;
      }
    }
  }

  RASDEBUG((DEBUG_INFO," / <AnyPreviousStrike>\n"));
  return status;
}

//Check if any Previous Strike matches the current RANK/BANK/DEVICE according to the granularity input
BOOLEAN
AnyPreviousStrikeMatch (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 Gran,
    OUT UINT8 *PrevStrike
    )
{
  UINT8 Bank;
  UINT8 Rank;
  UINT8 StrikeIndex;
  BOOLEAN status=FALSE;

  if(Gran == BANKVALID) {
    Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;
    for(StrikeIndex=0;StrikeIndex<=(StrikeNum-1);StrikeIndex++) {
      if (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeIndex].StrikeType == BANK_STRIKE){
        if ( (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeIndex].Bank) == (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank) &&
             (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeIndex].BankGroup) == (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].BankGroup) ) {
          *PrevStrike = StrikeIndex;
          status = TRUE;
          break;
        }
      }
    }
  }

  else if (Gran == RANKVALID) {
    Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;
    for(StrikeIndex=0;StrikeIndex<=(StrikeNum-1);StrikeIndex++) {
      if (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeIndex].StrikeType == RANK_STRIKE){
        if ((mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeIndex].Rank) == (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank)) {
          *PrevStrike = StrikeIndex;
          status = TRUE;
          break;
        }
      }
    }
  }
  return status;
}

/**

    Clear adddc status structure. To check if there was a copy complete SMI and clear the statuses

    @param NodeId          - Memory controller ID
    @param ddrCh           - Channel ID
    @param StrikeNum       - The strike Number that we are currently on.

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
ClearAdddcStatusStruct(IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    OUT BOOLEAN *IsEventHandled
    )
{
  UINT8       Rank;
  EFI_STATUS  status;
  UINT32      errStatus;

  RASDEBUG((DEBUG_INFO,"<ClearAdddcStatusStruct>\n"));
  status = IsCopyComplete( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));

  //copy completed for node and channel
  if(status == EFI_SUCCESS) {
    Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

    status = ReleaseSparingEngine( NodeId );
    RASDEBUG((DEBUG_INFO, "\t ADDDC Spare copy done handled successfully\n"));

    //Increment Number of strikes after the strike is handled successfully
    RASDEBUG((DEBUG_INFO,"\t Increment Number of Strikes \n"));
    mAdddcSparingEventStruct[NodeId][ddrCh].NumStrikes++;

    mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;
    errStatus = 0;
    RASDEBUG((DEBUG_INFO, "\t Update Event Progress and Pending status\n"));
    GetOrSetErrorHandlingStatus( NodeId, UpdateEventProgressStatus, &errStatus );
    GetOrSetErrorHandlingStatus( NodeId, UpdateEventPendingStatus, &errStatus );
    *IsEventHandled = TRUE;
  }
  else if (status == EFI_NOT_READY) {
    RASDEBUG((DEBUG_INFO, "\t Adddc spare copy still in progress\n"));
    status = EFI_SUCCESS;
    *IsEventHandled = FALSE;
  }
  else {
    *IsEventHandled = FALSE;
    RASDEBUG((DEBUG_INFO, "\t Bank DDDC spare copy Error!! Released Sparing engine\n"));
  }
  RASDEBUG((DEBUG_INFO,"</ClearAdddcStatusStruct>\n"));
  status = EFI_SUCCESS;
  return status;
}

BOOLEAN
CheckRankAvailabletoUpgrade (
                  IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 Rank
                  )

{
  UINT8     RegionIndex;
  UINT8     NumAdddcRegion;
  UINT8     Skt;

  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT    AdddcCtrRegion0;
  RASDEBUG((DEBUG_INFO, "<CheckRankAvailabletoUpgrade>\n"));

  NumAdddcRegion = 0;
  Skt = NODE_TO_SKT(NodeId);
  for (RegionIndex = 0; RegionIndex < mMaxRegion; RegionIndex++) {

     AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(Skt, NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionIndex*4));
     if (AdddcCtrRegion0.Bits.region_enable == 1) { //Only check enabled regions
       if (AdddcCtrRegion0.Bits.region_size == 0) { //If it is a Bank Strike (region_size = 0; translates to a Bank Strike)
         if ((UINT8)AdddcCtrRegion0.Bits.failed_cs == Rank) {
           RASDEBUG((DEBUG_INFO, "\t  region:%d Rank:%d \n",RegionIndex,Rank));
           NumAdddcRegion++;
         }
       }
     }
  }

  if(NumAdddcRegion>1)  { //More than 1 Bank VLS regions in the same Rank
    RASDEBUG((DEBUG_INFO, "\t More than 1 adddc regions in this Rank\n"));
    return FALSE;
  }

  return TRUE;
}

/**

    Upgrade Bank failure to Rank failure when there are two banks failed in same rank

    @param NodeId    - Memory controller ID
    @param ddrCh     - Channel ID
    @param Rank      - Failed Rank
    @param RegionNum - Region to be upgraded

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
UpgradeBank2Rank (IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 StrikeNum,
                  IN UINT8 RegionNum,
                  OUT BOOLEAN *IsEventHandled
                  )
{
  EFI_STATUS          status = EFI_SUCCESS;
  UINT8               Bank;
  UINT8               Rank;
  ADDDC_STATE_STRUCT  AdddcCurrentState;
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT    AdddcCtrRegion;

  AdddcCurrentState = mAdddcStateStruct[NodeId][ddrCh];

  RASDEBUG((DEBUG_INFO, "<UpgradeBank2Rank>\n"));
  Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

  /*If the Rank that needs to be upgraded to VLS has more than one
   * Bank VLS , BIOS does not support reversing more than one region.
   * */
  if (! CheckRankAvailabletoUpgrade(NodeId, ddrCh, Rank)) {
    if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
      StartSddcPlusOne  (NodeId, ddrCh, Rank, IsEventHandled);
    else
      RASDEBUG((DEBUG_INFO, "\t Inconsistent state attempting plusone when adddc in progress\n"));
    return status;
  }

  switch(AdddcCurrentState) {
    case(NoAdddcInProgress):
      //
      //Step1: call BANKADDCMR with the reverse Boolean set to true.
      Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;

      mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].ReverseRegion=TRUE;
      RASDEBUG((DEBUG_INFO, "\t About to reverse Bank Region %d\n",RegionNum));
      DoSpareCopy(NodeId, ddrCh, Rank, NEW_SPARE_COPY, BankAdddcSparingMR, RegionNum);

      mAdddcStateStruct[NodeId][ddrCh] = UB2RReverseInProgress;
      *IsEventHandled = TRUE;
    break;

    case(UB2RReverseInProgress) :
      status = IsCopyComplete( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
      if(status == EFI_SUCCESS) {
        //copy completed for node and channel
        Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;
        Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].ReverseRegion=FALSE;
        spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG);
        if (spareCtlReg.Bits.reverse && spareCtlReg.Bits.adddc_sparing){
          RASDEBUG((DEBUG_INFO, "\t Clearing the reverse bit\n"));
          spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG);
          spareCtlReg.Bits.reverse = 0;
          mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

          //Once reversed clear the region enable of that region
          AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr( NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionNum*4));
          AdddcCtrRegion.Bits.region_enable = 0;
          mCpuCsrAccess->WriteCpuCsr(NODE_TO_SKT(NodeId),  NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionNum*4), AdddcCtrRegion.Data);
        }
        status = ReleaseSparingEngine( NodeId );
        RASDEBUG((DEBUG_INFO, "\t Reverse Successful\n"));
        RASDEBUG((DEBUG_INFO, "\t About to upgrade region %d to Rank\n",RegionNum));
        DoSpareCopy(NodeId, ddrCh, Rank, NEW_SPARE_COPY, RankAdddcSparingMR, RegionNum);

        mAdddcStateStruct[NodeId][ddrCh] = UB2RRAdddcInProgress;
        *IsEventHandled = TRUE;

      }
      else if (status == EFI_NOT_READY) {
        RASDEBUG((DEBUG_INFO, "\t ADDDC spare copy still in progress\n"));
        status = EFI_SUCCESS;
        *IsEventHandled = FALSE;
      }
      else {
        *IsEventHandled = FALSE;
        RASDEBUG((DEBUG_INFO, "\t Bank DDDC spare copy Error!! Released Sparing engine\n"));
      }

    break;

    case(UB2RRAdddcInProgress):
    RASDEBUG((DEBUG_INFO, "\t Everything successful clearing adddc structs\n"));
        ClearAdddcStatusStruct(NodeId,ddrCh,StrikeNum,IsEventHandled);
        break;

    default:
      //add assert here we should never reach this case
      break;
  }
  RASDEBUG((DEBUG_INFO, "</ UpgradeBank2Rank>\n"));
  return status;
}

/**

  Verify if the Rank passed is available to be used as a Non Failed(Buddy) Rank for ADDDC operation

  @param Socket         Socket to which the Non Failed Rank belongs
  @param ddrCh          Ch to which the Non Failed Rank belongs
  @param dimm           Dimm to which the Rank belongs to
  @param rank           Rank

  @retval TRUE/FALSE  Return TRUE if Rank is available else FALSE

**/

BOOLEAN
CheckRankAvailable (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 dimm,
    IN UINT8 rank
    )
{

  UINT8                         Socket;
  CORRERRCNT_0_MCDDC_DP_STRUCT  corrErrCntReg;
  UINT32                        corrErrCntRegBaseAddr = CORRERRCNT_0_MCDDC_DP_REG;
  UINT8                         physicalRank;
  UINT8                         SktCh;

  Socket = NODE_TO_SKT(NodeId);
  SktCh  = NODECH_TO_SKTCH(NodeId,ddrCh);
  RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"\t CheckRankAvailable[Node:%d][Ch:%d][dimm:%d][rank:%d] \n",NodeId,SktCh,dimm,rank));
  physicalRank = rank & 0x3;
  //If the Rank is not enabled or already mapped out, return RANK unavailable
  if ((mSystemMemoryMap->Socket[Socket].ChannelInfo[SktCh].DimmInfo[dimm].rankList[physicalRank].enabled == 0) || \
      (mMemRas->pSysHost->nvram.mem.socket[Socket].channelList[SktCh].dimmList[dimm].mapOut[physicalRank] == 1)) {
    return FALSE;
  }

  //Verify rank is healthy. If not healthy, return as RANK Unavailable
  corrErrCntReg.Data = mCpuCsrAccess->ReadCpuCsr( Socket, SktCh, corrErrCntRegBaseAddr + (rank/2) * 4 );
  if( ((rank % 2) == 0 ) && corrErrCntReg.Bits.cor_err_cnt_0 != 0 )
    return FALSE;
  if( ((rank % 2) == 1 ) && corrErrCntReg.Bits.cor_err_cnt_1 != 0 )
    return FALSE;

  //Finally check if the Rank is present in the Skip Rank Bit map
  if (mSkipRankBitmap[NodeId][ddrCh] & 1<<rank) {
    return FALSE;
  }
  RASDEBUG(( DEBUG_INFO,"\t Rank [Node:%d][Ch:%d][dimm:%d][rank:%d] available\n",NodeId,SktCh,dimm,rank));
  return TRUE;
}

/**

  Change spare error threshold that was pre-configured for a rank

  @param socket                    - Socket number
  @param ch                      - Channel number (0-based)
  @param Rank                    -  Rank whose threshold value should be changed

  @retval N/A

**/
static void
UpdateErrorThresholdForAdddcRegion (
                  UINT8    Skt,
                  UINT8    SktCh,
                  UINT8    rank
                  )
{
  UINT32                            ErrThreshold;
  CORRERRTHRSHLD_0_MCDDC_DP_STRUCT  imcCORRERRTHRSHLD01;

  /*If Rank error threshold is uninitialized initialize it one time only*/
  if(mRankErrorThreshold == 0) {
    imcCORRERRTHRSHLD01.Data = mCpuCsrAccess->ReadCpuCsr(Skt, SktCh, CORRERRTHRSHLD_0_MCDDC_DP_REG + ((rank >> 1) * 4));
    mRankErrorThreshold = imcCORRERRTHRSHLD01.Bits.cor_err_th_0;
  }
  if (mRankErrorThreshold > 0 ){
    ErrThreshold = (mRankErrorThreshold/2) + (mRankErrorThreshold%2);
    imcCORRERRTHRSHLD01.Data = mCpuCsrAccess->ReadCpuCsr(Skt, SktCh, CORRERRTHRSHLD_0_MCDDC_DP_REG + ((rank >> 1) * 4));
    if (rank%2) {
      imcCORRERRTHRSHLD01.Bits.cor_err_th_1 = ErrThreshold;
    } else {
      imcCORRERRTHRSHLD01.Bits.cor_err_th_0 = ErrThreshold;
    }
    mCpuCsrAccess->WriteCpuCsr(Skt, SktCh, CORRERRTHRSHLD_0_MCDDC_DP_REG + ((rank >> 1) * 4), imcCORRERRTHRSHLD01.Data);
  }
  return;
}

/*
 * This function is only used to identify there is a potential buddy rank available once we do a reverse
 * */

/**

  This function is only used to identify there is a potential buddy rank available once we reverse an existing adddc region during reverse and remap rank

  @param NodeId         Node to which the Failed Rank belongs
  @param ddrCh          Ch to which the Failed Rank belongs
  @param dimm           region number of the region being reversed

  @retval EFI_STATUS  Return SUCCESS if Rank is available else EFI_FAILED

**/

EFI_STATUS
IsRankAvailableToReverse (
    IN  UINT8   NodeId,
    IN  UINT8   ddrCh,
    IN  UINT8   RegionNum
    )
{
  UINT8                                   StrikeNum;
  UINT8                                   Dimm;
  UINT8                                   FailedDimm;
  UINT8                                   Skt;
  UINT8                                   RegionIndex;
  UINT8                                   RankIndex;
  UINT8                                   FailedRank;
  UINT8                                   FailedSubRank;
  UINT8                                   BuddyRankAssign;
  DIMMMTR_0_MC_MAIN_STRUCT                dimmMtr;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT    AdddcCtrRegion0;
  EFI_STATUS                              status;

  status = EFI_SUCCESS;

  RASDEBUG(( DEBUG_INFO," <IsRankAvailableToReverse> \n"));

  Skt = NODE_TO_SKT(NodeId);
  if (!(mMemRas->pSysHost->var.common.socketPresentBitMap &  (1 << Skt)))
    return EFI_INVALID_PARAMETER;

  StrikeNum     = mAdddcSparingEventStruct[NodeId][ddrCh].NumStrikes;
  FailedRank    = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank;
  FailedSubRank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimarySubRank;
  FailedDimm    = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Dimm;

  for (RegionIndex = 0; RegionIndex < mMaxRegion; RegionIndex++) {
    if (RegionIndex == RegionNum) continue; //bypass the current failed region.

    AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(Skt, NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionIndex*4));
    if (AdddcCtrRegion0.Bits.region_enable == 1) { //Only check enabled regions
      if (AdddcCtrRegion0.Bits.region_size == 0) { //If it is a Bank Strike (region_size = 0; translates to a Bank Strike)
        if ((UINT8)AdddcCtrRegion0.Bits.failed_cs == FailedRank) { //If the other region's failed rank matches the current region's failed rank.

          //special case when the VLS is established within Banks of same region.
          //In this special case only ONE VLS can exist we move to sddcplus one if that is done
          if ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs == FailedRank) {
            return EFI_UNSUPPORTED;
          }

          return status; //Buddy Rank identified, we can now return.
        } else { //Mismatch occured, so let's mark both the ranks part of VLS for BuddyRank consideration further down the road.
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs); //Other Region's BuddyRank
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.failed_cs); //Other Region's FailedRank
        }
      }
      else { //Previous VLS is between Ranks, so let's mark both the ranks part of VLS for BuddyRank consideration further down the road.
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs); //Other Region's BuddyRank
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.failed_cs); //Other Region's FailedRank
      }
    }
  }

  for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
    if (Dimm == FailedDimm) continue;
    dimmMtr.Data = mCpuCsrAccess->ReadCpuCsr( Skt, NODECH_TO_SKTCH(NodeId, ddrCh), DIMMMTR_0_MC_MAIN_REG + (Dimm * 4));
    if ((dimmMtr.Bits.dimm_pop == 1) && (dimmMtr.Bits.ddr3_width == 0)) { //Verify DIMM is present, and it is a x4 type.
      for(RankIndex=0;RankIndex<MAX_RANK_DIMM;RankIndex++){
        //The Non Failed Rank has to be on the other dimm.  Get the value of BIT2 and perform 1's complement on that bit to select other dimm
        BuddyRankAssign = ( (FailedRank & BIT2)^BIT2) | RankIndex;
        if (CheckRankAvailable (NodeId, ddrCh, Dimm, BuddyRankAssign)) {
          RASDEBUG(( DEBUG_INFO," <IsRankAvailableToReverse> : Yes \n"));
          return status; //Buddy Rank available, return success
        }
      }
    } //Verify DIMM is present
  } // Dimm loop

  //
  //If there is no other dimm or no useable rank in other DIMM then find a RANK in the same DIMM that is farthest away from current rank
  //
  for(RankIndex=0;RankIndex<MAX_RANK_DIMM;RankIndex++){
    BuddyRankAssign = FarthestRankArray[FailedRank][RankIndex];
    BuddyRankAssign = (FailedRank & BIT2) | BuddyRankAssign; //BIT2 will ensure we are using the same dimm as failed rank
    if (CheckRankAvailable (NodeId, ddrCh, FailedDimm, BuddyRankAssign)) {
      RASDEBUG(( DEBUG_INFO," <IsRankAvailableToReverse> : Yes \n"));
        return status; //Buddy Rank identified, we can now return.
     }
  }
  //
  //  If we get here, then we are unable to find a BuddyRank.
  //
  status = EFI_NOT_FOUND;
  RASDEBUG(( DEBUG_INFO," <IsRankAvailableToReverse> : No \n"));
  return status;
}




/**

  RankBuddy Selection function needed for ADDDC

  @param NodeId         Failing NodeId
  @param ddrCh          Failing DDR Channel
  @param RegionNum      Virtual Region used for LockStep 
  @param SkipBankBitmap Output Bitmap identifying which Banks to NOT consider when searching for a Bank Buddy.

  @retval EFI_SUCCESS:              Buddy Rank successfully identified
  @retval EFI_NOT_FOUND:            Failed to identify a Buddy Rank
  @retval EFI_INVALID_PARAMETER:    Invalid input parameter

**/


EFI_STATUS
FindNonFailedRankBank (
    IN  UINT8   NodeId,
    IN  UINT8   ddrCh,
    IN  UINT8   RegionNum
    )
{
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT        AdddcCtrRegion0;
  DIMMMTR_0_MC_MAIN_STRUCT                    dimmMtr;
  EFI_STATUS                                  status;
  UINT8                                       StrikeNum;
  UINT8                                       Dimm;
  UINT8                                       FailedDimm;
  UINT8                                       Skt;
  UINT8                                       RegionIndex;
  UINT8                                       RankIndex;
  UINT8                                       FailedRank;
  UINT8                                       FailedSubRank;
  UINT8                                       BuddyRankAssign;
  UINT8                                       FailedBank;
  UINT8                                       FailedBankGrp;
  BOOLEAN                                     MIRROR_ENABLED = FALSE;
  status = EFI_SUCCESS;

  RASDEBUG(( DEBUG_INFO," <FindNonFailedRankBank> \n"));

  Skt = NODE_TO_SKT(NodeId);
  if (!(mMemRas->pSysHost->var.common.socketPresentBitMap &  (1 << Skt)))
    return EFI_INVALID_PARAMETER;

  if (mMemRas->SystemMemInfo->RasModesEnabled & CH_ALL_MIRROR) {
    MIRROR_ENABLED = TRUE;
  }

  StrikeNum     = mAdddcSparingEventStruct[NodeId][ddrCh].NumStrikes;
  FailedBank    = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryBank;
  FailedBankGrp = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryBankGrp;
  FailedRank    = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank;
  FailedSubRank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimarySubRank;
  FailedDimm    = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Dimm;
  RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"\tFailedRank:%d\n",FailedRank));
  RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"\t FailedDimm:%d\n",FailedDimm));
  //
  // Try and match BuddyRank values from a *different* region, if one exists. (1st Buddy Rank Check)
  //
  for (RegionIndex = 0; RegionIndex < mMaxRegion; RegionIndex++) {
    if (RegionIndex == RegionNum) continue; //bypass the current failed region.

    AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(Skt, NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionIndex*4));
    if (AdddcCtrRegion0.Bits.region_enable == 1) { //Only check enabled regions
      RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"\t Other ADDDC Regions are active \n"));
      if (AdddcCtrRegion0.Bits.region_size == 0) { //If it is a Bank Strike (region_size = 0; translates to a Bank Strike)
        if ((UINT8)AdddcCtrRegion0.Bits.failed_cs == FailedRank) { //If the other region's failed rank matches the current region's failed rank.

          //special case when the VLS is established within Banks of same rank.
          //In this special case only ONE VLS can exist we move to sddcplus one if that is done
          if ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs == FailedRank) {
            return EFI_UNSUPPORTED;
          }

          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddySubRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          if(! MIRROR_ENABLED) {
            mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank     = FailedBank ^ BIT0;
          } else {
            mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank     = FailedBank;
          }
          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBankGrp  = FailedBankGrp;
          RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"A Buddy Rank (%d) has been identified.\n", mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank));
          return status; //Buddy Rank identified, we can now return.
        } else { //Mismatch occured, so let's mark both the ranks part of VLS for BuddyRank consideration further down the road.
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs); //Other Region's BuddyRank
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.failed_cs); //Other Region's FailedRank
        }
      }
      else { //Previous VLS is between Ranks, so let's mark both the ranks part of VLS for BuddyRank consideration further down the road.
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs); //Other Region's BuddyRank
          mSkipRankBitmap[NodeId][ddrCh] |= 1 << ((UINT8)AdddcCtrRegion0.Bits.failed_cs); //Other Region's FailedRank
      }
    }
  }

  //
  // Identify whether another DIMM is available on the same channel to see if we can find a Buddy Rank there. (2nd Buddy Rank Check)
  //

  for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
    if (Dimm == FailedDimm) continue;

    dimmMtr.Data = mCpuCsrAccess->ReadCpuCsr( Skt, NODECH_TO_SKTCH(NodeId, ddrCh), DIMMMTR_0_MC_MAIN_REG + (Dimm * 4));
    if ((dimmMtr.Bits.dimm_pop == 1) && (dimmMtr.Bits.ddr3_width == 0)) { //Verify DIMM is present, and it is a x4 type.
      //
      //Note that DIMM0 will always use rank/chip_select 0 thru 3, and DIMM1 will always use rank/chip_select 4 thru 7
      //Therefore, performing a One's complement of BIT2 of the Rank/Chip_select essentially selects the "other DIMM" of that same channel.
      //
      RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"\t Presence of other dimm identified\n"));
      for(RankIndex=0;RankIndex<MAX_RANK_DIMM;RankIndex++){
        //The Non Failed Rank has to be on the other dimm.  Get the value of BIT2 and perform 1's complement on that bit to select other dimm
        BuddyRankAssign = ( (FailedRank & BIT2)^BIT2) | RankIndex;
        RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"\t Trying Buddy Rank %d\n",BuddyRankAssign));
        if (CheckRankAvailable (NodeId, ddrCh, Dimm, BuddyRankAssign)) {
          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank     = BuddyRankAssign;
          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddySubRank  = FailedSubRank;
          if(! MIRROR_ENABLED) {
            mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank     = FailedBank ^ BIT0;
          } else {
            mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank     = FailedBank;
          }
          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBankGrp  = FailedBankGrp;
          RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"A Buddy Rank (%d) and Buddy Bank (%d) have been identified.\n", mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank, mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank));
          return status; //Buddy Rank identified, we can now return.
        }
      }

    } //Verify DIMM is present

  } // Dimm loop

  //
  //If there is no other dimm or no useable rank in other DIMM then find a RANK in the same DIMM that is farthest away from current rank
  //
  for(RankIndex=0;RankIndex<MAX_RANK_DIMM;RankIndex++){
    RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"\t No other Dimms or the Other dimm is unuseable\n"));
    BuddyRankAssign = FarthestRankArray[FailedRank][RankIndex];
    BuddyRankAssign = (FailedRank & BIT2) | BuddyRankAssign; //BIT2 will ensure we are using the same dimm as failed rank
    if (CheckRankAvailable (NodeId, ddrCh, FailedDimm, BuddyRankAssign)) {
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank     = BuddyRankAssign;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddySubRank  = FailedSubRank;
        if(! MIRROR_ENABLED) {
          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank     = FailedBank ^ BIT0;
        } else {
          mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank     = FailedBank;
        }
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBankGrp  = FailedBankGrp;
        RASDEBUG((DEBUG_ERROR | DEBUG_INFO,"A Buddy Rank (%d) and Buddy Bank (%d) have been identified.\n", mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank, mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyBank));
        return status; //Buddy Rank identified, we can now return.
     }
  }
  //
  //  If we get here, then we are unable to find a BuddyRank.
  //
  status = EFI_NOT_FOUND;
  RASDEBUG((DEBUG_ERROR,"FAIL: Failed to allocate a BuddyRank!\n"));
  RASDEBUG(( DEBUG_INFO," <FindNonFailedRankBank> \n"));
  return status;
}


/**

    Reverse the Bank VLS established WITHIN a Rank before Upgrading the rank to plus one

    @param NodeId           - Memory controller ID
    @param ddrCh            - Channel ID
    @param tStrikeNum       - The strike Number that we are currently on.
    @param PrevStrikeNUm    - The previous strike which used the current strike Rank as buddy and to be reversed
    @param PrevRegionNum    - The previous region which used the current strike Rank as buddy

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
ReverseAndUpgradePlusOne (
                  IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 StrikeNum,
                  IN UINT8 PrevStrikeNum,
                  IN UINT8 PrevRegionNum,
                  OUT BOOLEAN *IsEventHandled
                  )
{
  EFI_STATUS          status = EFI_SUCCESS;
  UINT8               Rank;
  ADDDC_STATE_STRUCT  AdddcCurrentState;
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT    AdddcCtrRegion;

  AdddcCurrentState = mAdddcStateStruct[NodeId][ddrCh];

  RASDEBUG((DEBUG_INFO, "<ReverseAndUpgradePlusOne>\n"));
  Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

  switch(AdddcCurrentState) {
    case(NoAdddcInProgress):

        mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].ReverseRegion=TRUE;
        mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].ReverseRemapPrevStrike = PrevStrikeNum;

        //ReverseBankADDDC on Previous Region
        DoSpareCopy(NodeId, ddrCh, Rank, NEW_SPARE_COPY, BankAdddcSparingMR, PrevRegionNum);

        mAdddcStateStruct[NodeId][ddrCh] = RUPReverseInProgress;
        *IsEventHandled = TRUE;
        //DO SDDCPLUSONE if no region is available.

      break;

    case(RUPReverseInProgress):
      status = IsCopyComplete( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
      if(status == EFI_SUCCESS) {
        //copy completed for node and channel
        Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

        status = ReleaseSparingEngine( NodeId );
        ASSERT (status == EFI_SUCCESS);
        RASDEBUG((DEBUG_INFO, "ReverseAndUpgradePlusOne - Reverse Done. Initiating sddc on Rank:%d \n",Rank));

        //Clear reverse region before initiating another Bank ADDDC operation
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].ReverseRegion=FALSE;
        spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG);
        if (spareCtlReg.Bits.reverse && spareCtlReg.Bits.adddc_sparing){
          RASDEBUG((DEBUG_INFO, "\t Clearing the reverse bit\n"));
          spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG);
          spareCtlReg.Bits.reverse = 0;
          mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

          //Once reversed clear the region enable of that region
          AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr( NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + PrevRegionNum*4));
          AdddcCtrRegion.Bits.region_enable = 0;
          mCpuCsrAccess->WriteCpuCsr(NODE_TO_SKT(NodeId),  NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + PrevRegionNum*4), AdddcCtrRegion.Data);
        }
        mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;
        StartSddcPlusOne  (NodeId, ddrCh, Rank, IsEventHandled);
        *IsEventHandled = TRUE;

      }
      else if (status == EFI_NOT_READY) {
        RASDEBUG((DEBUG_INFO, "\t ReverseAndUpgradePlusOne - adddc reverse spare still in progress\n"));
        status = EFI_SUCCESS;
        *IsEventHandled = FALSE;
      }
      else {
        *IsEventHandled = FALSE;
        RASDEBUG((DEBUG_INFO, "\t ReverseAndUpgradePlusOne - Bank DDDC spare copy Error!! Released Sparing engine\n"));
      }

    break;
  }
  RASDEBUG((DEBUG_INFO, "</ReverseAndUpgradePlusOne>\n"));
  return status;

}


/**

    Reverse and remap the Bank that was previously used as a buddy because it has a strike

    @param NodeId          - Memory controller ID
    @param ddrCh           - Channel ID
    @param CurrentStrikeNum - The strike Number that we are currently on.
    @param PrevStrikeNUm    - The previous strike which used the current strike Rank as buddy and to be reversed
    @param PrevRegionNum    - The previous region which used the current strike Rank as buddy

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
ReverseRemapBank (IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 CurrentStrikeNum,
                  IN UINT8 PrevStrikeNum,
                  IN UINT8 PrevRegionNum,
                  OUT BOOLEAN *IsEventHandled
                  )
{
  EFI_STATUS          status = EFI_SUCCESS;
  UINT8               Bank;
  UINT8               Rank;
  ADDDC_STATE_STRUCT  AdddcCurrentState;
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT    AdddcCtrRegion;

  AdddcCurrentState = mAdddcStateStruct[NodeId][ddrCh];

  RASDEBUG((DEBUG_INFO, "<ReverseRemapBank> \n"));
  Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].Bank;
  Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].Rank;

  switch(AdddcCurrentState) {
    case(NoAdddcInProgress):
      //Save the information of previous strike that is being reversed in the current strike
      mAdddcSparingEventStruct[NodeId][ddrCh].Strike[CurrentStrikeNum].ReverseRemapPrevStrike = PrevStrikeNum;

      //Since ADDDC region will be reversed, mark the Rank that was originally used as NonFailed Rank as Unuseable
      mSkipRankBitmap[NodeId][ddrCh] |= 1 << mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].PrimaryRank;
      mSkipRankBitmap[NodeId][ddrCh] |= 1 << mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].BuddyRank;
      if ((mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion > 0) && (IsRankAvailableToReverse(NodeId, ddrCh,PrevRegionNum) == EFI_SUCCESS)){

        mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].ReverseRegion=TRUE;

        //ReverseBankDDDC on Previous Region
        DoSpareCopy(NodeId, ddrCh, Rank, NEW_SPARE_COPY, BankAdddcSparingMR, PrevRegionNum);

        mAdddcStateStruct[NodeId][ddrCh] = RRMBReverseInProgress;
        *IsEventHandled = TRUE;
        }
        //DO SDDCPLUSONE if no region is available.
        else {
          if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
            StartSddcPlusOne  (NodeId, ddrCh, Rank, IsEventHandled);
        }

      break;

    case(RRMBReverseInProgress):
      status = IsCopyComplete( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
      if(status == EFI_SUCCESS) {
        //copy completed for node and channel
        Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].Rank;
        spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr( NODE_TO_SKT(NodeId), NODE_TO_SKT(NodeId), SPARING_CONTROL_MC_MAIN_REG);
        if (spareCtlReg.Bits.reverse && spareCtlReg.Bits.adddc_sparing){
          RASDEBUG((DEBUG_INFO, "\t Clearing the reverse bit\n"));
          spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG);
          spareCtlReg.Bits.reverse = 0;
          mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

          //Once reversed clear the region enable of that region
          AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr( NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + PrevRegionNum*4));
          AdddcCtrRegion.Bits.region_enable = 0;
          mCpuCsrAccess->WriteCpuCsr(NODE_TO_SKT(NodeId),  NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + PrevRegionNum*4), AdddcCtrRegion.Data);
        }
        status = ReleaseSparingEngine( NodeId );
        RASDEBUG((DEBUG_INFO, "\tReverseRemapBank - Reverse Done. Attempting 1st Bank ADDDC on Strike:%d Region:%d\n",PrevStrikeNum,PrevRegionNum));

        //Clear reverse region before initiating another Bank ADDDC operation
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].ReverseRegion=FALSE;
        //Do BankADDDC for the previous strike that will find and assign a new buddy. But we reuse region number
        mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;
        BankADDDCMR(NodeId,ddrCh, PrevStrikeNum, PrevRegionNum, IsEventHandled);

        mAdddcStateStruct[NodeId][ddrCh] = RRMB1SetInProgress;
        *IsEventHandled = TRUE;

      }
      else if (status == EFI_NOT_READY) {
        RASDEBUG((DEBUG_INFO, "\tReverseRemapBank - spare copy still in progress\n"));
        status = EFI_SUCCESS;
        *IsEventHandled = FALSE;
      }
      else {
        *IsEventHandled = FALSE;
        RASDEBUG((DEBUG_INFO, "\tReverseRemapBank - Bank DDDC spare copy Error!! Released Sparing engine\n"));
      }

    break;

    case(RRMB1SetInProgress):
        status = IsCopyComplete( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
        if(status == EFI_SUCCESS) {
          //copy completed for node and channel

          status = ReleaseSparingEngine( NodeId );
          RASDEBUG((DEBUG_INFO, "\tReverseRemapBank - First Bank ADDDC Done. Attempting 2nd Bank ADDDC on Strike:%d \n",CurrentStrikeNum));

          //Do BankADDDC for the Current strike - This will find the new region and its buddy
          mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;
          BankADDDCMR(NodeId, ddrCh, CurrentStrikeNum, REGION_NUM_AUTO, IsEventHandled);

          mAdddcStateStruct[NodeId][ddrCh] = RRMB2SetInProgress;
          *IsEventHandled = TRUE;
        }
        else if (status == EFI_NOT_READY) {
          RASDEBUG((DEBUG_INFO, "\tReverseRemapBank - spare copy still in progress\n"));
          status = EFI_SUCCESS;
          *IsEventHandled = FALSE;
        }
        else {
          *IsEventHandled = FALSE;
          RASDEBUG((DEBUG_INFO, "\tReverseRemapBank - Bank adddc spare copy Error!! Released Sparing engine\n"));
        }

     break;

    case(RRMB2SetInProgress):
        ClearAdddcStatusStruct(NodeId,ddrCh,CurrentStrikeNum,IsEventHandled);
     break;

  }
  RASDEBUG((DEBUG_INFO, "</ReverseRemapBank>\n"));
  return status;

}

/**

    Reverse and remap the rank that was previously used as a buddy because it has a strike

    @param NodeId          - Memory controller ID
    @param ddrCh           - Channel ID
    @param CurrentStrikeNum - The strike Number that we are currently on.
    @param PrevStrikeNUm    - The previous strike which used the current strike Rank as buddy and to be reversed
    @param PrevRegionNum    - The previous region which used the current strike Rank as buddy

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
ReverseRemapRank (IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 CurrentStrikeNum,
                  IN UINT8 PrevStrikeNum,
                  IN UINT8 PrevRegionNum,
                  OUT BOOLEAN *IsEventHandled
                  )
{
  EFI_STATUS          status = EFI_SUCCESS;
  UINT8               Bank;
  UINT8               Rank;
  ADDDC_STATE_STRUCT  AdddcCurrentState;
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT    AdddcCtrRegion;

  RASDEBUG((DEBUG_INFO, "<ReverseRemapRank>\n"));
  AdddcCurrentState = mAdddcStateStruct[NodeId][ddrCh];

  Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].PrimaryBank;
  Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].PrimaryRank;

  switch(AdddcCurrentState) {
    case(NoAdddcInProgress):
       //Save the information of previous strike that is being reversed in the current strike
       mAdddcSparingEventStruct[NodeId][ddrCh].Strike[CurrentStrikeNum].ReverseRemapPrevStrike = PrevStrikeNum;

        /*Since ADDDC region will be reversed, mark the Rank that was originally used as NonFailed Rank as Unuseable
        when selecting a Non-Failed Rank*/
        mSkipRankBitmap[NodeId][ddrCh] |= 1 << mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].PrimaryRank;
        mSkipRankBitmap[NodeId][ddrCh] |= 1 << mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].BuddyRank;

        //Only reverse if there is another region available and another Rank available for creating Rank VLS
        if ((mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion > 0) && (IsRankAvailableToReverse(NodeId, ddrCh,PrevRegionNum) == EFI_SUCCESS)){

          mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].ReverseRegion=TRUE;

          //ReverseRankDDDC on current Region
          DoSpareCopy(NodeId, ddrCh, Rank, NEW_SPARE_COPY, RankAdddcSparingMR, PrevRegionNum);

          mAdddcStateStruct[NodeId][ddrCh] = RRMRReverseInProgress;
          *IsEventHandled = TRUE;
        }
        //DO SDDCPLUSONE if no region is available.
        else {
          if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
            StartSddcPlusOne  (NodeId, ddrCh, Rank, IsEventHandled);
        }
    break;

    case(RRMRReverseInProgress):
      status = IsCopyComplete( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
      if(status == EFI_SUCCESS) {
        //copy completed for node and channel
        Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].Rank;
        spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr( NODE_TO_SKT(NodeId), NODE_TO_SKT(NodeId), SPARING_CONTROL_MC_MAIN_REG);
        if (spareCtlReg.Bits.reverse && spareCtlReg.Bits.adddc_sparing){
          RASDEBUG((DEBUG_INFO, "\t Clearing the reverse bit\n"));
          spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG);
          spareCtlReg.Bits.reverse = 0;
          mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId), SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

          //Once reversed clear the region enable of that region
          AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr( NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + PrevRegionNum*4));
          AdddcCtrRegion.Bits.region_enable = 0;
          mCpuCsrAccess->WriteCpuCsr(NODE_TO_SKT(NodeId),  NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + PrevRegionNum*4), AdddcCtrRegion.Data);
        }
        status = ReleaseSparingEngine( NodeId );
        RASDEBUG((DEBUG_INFO, "\t ReverseRemapRank -Reverse Rank done handled successfully\n"));
        RASDEBUG((DEBUG_INFO, "\t ReverseRemapRank -Initiate first Rank Sparing\n"));
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[PrevRegionNum].ReverseRegion=FALSE;
        //Do RankADDDC for the previous strike that will find and assign a new buddy. But Re-use region number
        mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;
        RankADDDCMR(NodeId,ddrCh, PrevStrikeNum,PrevRegionNum,IsEventHandled);

        mAdddcStateStruct[NodeId][ddrCh] = RRMR1SetInProgress;
        *IsEventHandled = TRUE;
      }
      else if (status == EFI_NOT_READY) {
        RASDEBUG((DEBUG_INFO, "\t  - ReverseRemapRank -spare copy still in progress\n"));
        status = EFI_SUCCESS;
        *IsEventHandled = FALSE;
      }
      else {
        *IsEventHandled = FALSE;
        RASDEBUG((DEBUG_INFO, "\t ReverseRemapRank -Spare copy Error!! Released Sparing engine\n"));
      }

    break;

    case(RRMR1SetInProgress):
       status = IsCopyComplete( NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
        if(status == EFI_SUCCESS) {
          //copy completed for node and channel
          Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].Rank;

          status = ReleaseSparingEngine( NodeId );
          RASDEBUG((DEBUG_INFO, "\t ReverseRemapRank -First spare copy copy done handled successfully\n"));
          RASDEBUG((DEBUG_INFO, "\t ReverseRemapRank -Initiate second Rank Sparing\n"));
          //Do RankADDDC for the Current Strike - This will find a new region and a new buddy
          mAdddcStateStruct[NodeId][ddrCh] = NoAdddcInProgress;
          BankADDDCMR(NodeId, ddrCh, CurrentStrikeNum, REGION_NUM_AUTO, IsEventHandled);

          mAdddcStateStruct[NodeId][ddrCh] = RRMR2SetInProgress;
          *IsEventHandled = TRUE;
        }
        else if (status == EFI_NOT_READY) {
          RASDEBUG((DEBUG_INFO, "\t ReverseRemapRank - ADDDC spare copy still in progress\n"));
          status = EFI_SUCCESS;
          *IsEventHandled = FALSE;
        }
        else {
          *IsEventHandled = FALSE;
          RASDEBUG((DEBUG_INFO, "\t ReverseRemapRank - Bank DDDC spare copy Error!! Released Sparing engine\n"));
        }
     break;

    case(RRMR2SetInProgress):
        ClearAdddcStatusStruct(NodeId,ddrCh,CurrentStrikeNum,IsEventHandled);
    break;
  }
  RASDEBUG((DEBUG_INFO, "</ReverseRemapRank>\n"));
  return status;
}


/**

    Call BankAdddc for MR

    @param NodeId    - Memory controller ID
    @param ddrCh     - Channel ID
    @param Rank      - Failed Rank
    @param *IsEventHandled - ptr to event handled status

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
BankADDDCMR (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 RegionNumInput,
    OUT BOOLEAN *IsEventHandled
    )
{
  EFI_STATUS                                  status = EFI_SUCCESS;
  UINT8                                       RegionNum = 0;
  UINT8                                       Bank;
  UINT8                                       Rank;
  UINT8                                       PrimaryRank;
  UINT8                                       NonFailedRank;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT        AdddcCtrRegion0;
  ADDDC_STATE_STRUCT  AdddcCurrentState;

  RASDEBUG((DEBUG_INFO, "<BankADDDCMR>\n"));
  AdddcCurrentState = mAdddcStateStruct[NodeId][ddrCh];
  RASDEBUG((DEBUG_INFO, "\t BankADDDCMR: Strike Number %d\n",StrikeNum));
  Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;
  Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

  if(mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion > 0) {
    if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress) {
      //The regions are assigned from 0 - (MAX_REGION - 1)
      if (RegionNumInput == REGION_NUM_AUTO) {
        RegionNum =  mMaxRegion - mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion;
        mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion -= 1;
      } else {
        if(RegionNumInput < MAX_REGION_EX){
          RegionNum =  RegionNumInput;
        }
        else  {
          RASDEBUG((DEBUG_ERROR, "\t Attempting to read invalid region!!\n"));
          status = EFI_INVALID_PARAMETER;
          ASSERT_EFI_ERROR(status);
          return status;
        }
      }

      if (RegionNum < MAX_REGION_EX) {
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].RegionNum = RegionNum;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].StrikeNum = StrikeNum;
        mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionNum = RegionNum;

        AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionNum*4) );
        if (AdddcCtrRegion0.Bits.region_enable != 0) {
          RASDEBUG((DEBUG_INFO, "\t BankADDDCMR: Region Wrongly Acquired %d\n", RegionNum));
          status = EFI_NOT_FOUND;
          ASSERT (status);
          IsEventHandled = FALSE;
          return status;
        }
        RASDEBUG((DEBUG_INFO, "\t BankADDDCMR: Region Number %d\n",RegionNum));

        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].Valid = 1;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank     = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimarySubRank  = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].SubRank;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryBank     = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryBankGrp  = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].BankGroup;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].FailedDevice    = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Device;

        //The following call will populate buddy rank structure mAdddcSparingEventStruct[ddrch]->Region.BuddyRank
        status = FindNonFailedRankBank(NodeId, ddrCh, RegionNum);

        //The only time an unsupported status is returned is if we attempt two Bank VLS on single Rank Single DIMM system.
        if( (status == EFI_UNSUPPORTED) || (status == EFI_NOT_FOUND) ){
            return status;
        }

        /*Before initiating a Bank or Rank sparing operation that creates an adddc region, reducing the
         * error thresholds for the ranks that will be part of the adddc region to half their original value */

        PrimaryRank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank;
        NonFailedRank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank;
        if(mMemRas->pSysHost->var.common.stepping == H0_REV_SKX) {
          UpdateErrorThresholdForAdddcRegion(NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh),PrimaryRank);
          UpdateErrorThresholdForAdddcRegion(NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh),NonFailedRank);
        }
        DoSpareCopy(NodeId, ddrCh, Rank, NEW_SPARE_COPY, BankAdddcSparingMR, RegionNum);

        //this is only set if current status was noadddc in progress
        mAdddcStateStruct[NodeId][ddrCh] = BAdddcInProgress;
      }
      *IsEventHandled = TRUE;
    }
    //Inconsistent adddc state
    else {
      DEBUG((DEBUG_INFO, "Inconsistent ADDDC state \n"));
      status = EFI_INVALID_PARAMETER;
      return status;
    }
  }
  //DO SDDCPLUSONE if no region is available.
  else {
    DEBUG((DEBUG_INFO, "No More regions available - go to Sddcplusone \n"));
    if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
      StartSddcPlusOne  (NodeId, ddrCh, Rank, IsEventHandled);
  }
  RASDEBUG((DEBUG_INFO, "</BankADDDCMR>\n"));
  return status;
}

/**
    Call RankADDDCMR

    @param NodeId    - Memory controller ID
    @param ddrCh     - Channel ID
    @param Rank      - Failed Rank
    @param *IsEventHandled - ptr to event handled status

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
RankADDDCMR (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 RegionNumInput,
    OUT BOOLEAN *IsEventHandled
    )
{
  EFI_STATUS                                  status = EFI_SUCCESS;
  UINT8                                       RegionNum = 0;
  UINT8                                       Bank;
  UINT8                                       Rank;
  UINT8                                       PrimaryRank;
  UINT8                                       NonFailedRank;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT        AdddcCtrRegion0;
  ADDDC_STATE_STRUCT  AdddcCurrentState;

  RASDEBUG((DEBUG_INFO,"<RankADDDCMR>\n"));
  AdddcCurrentState = mAdddcStateStruct[NodeId][ddrCh];

  Bank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;
  Rank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

  if(mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion > 0) {
    if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress) {
      //The regions are assigned from 0 - (MAX_REGION - 1)
      if (RegionNumInput == REGION_NUM_AUTO) {
        RegionNum =  mMaxRegion - mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion;
        mAdddcSparingEventStruct[NodeId][ddrCh].NumAvlRegion -= 1;
      } else {
        if(RegionNumInput < MAX_REGION_EX){
          RegionNum =  RegionNumInput;
        }
        else  {
          RASDEBUG((DEBUG_ERROR, "\t Attempting to read invalid region !!\n"));
          status = EFI_INVALID_PARAMETER;
          ASSERT_EFI_ERROR(status);
          return status;
        }
        RegionNum =  RegionNumInput;
      }
      if (RegionNum < MAX_REGION_EX) {
        RASDEBUG((DEBUG_INFO, "\tRankADDDCMR:Acquired region %d\n",RegionNum));
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].RegionNum = RegionNum;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].StrikeNum = StrikeNum;
        mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionNum = RegionNum;

        AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionNum*4) );
        if (AdddcCtrRegion0.Bits.region_enable != 0) {
          RASDEBUG((DEBUG_INFO, "\tRankADDDCMR: Region Wrongly Acquired %d\n", RegionNum));
          IsEventHandled = FALSE;
          return status;
        }

        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].Valid = 1;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimarySubRank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].SubRank;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryBank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryBankGrp = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].BankGroup;
        mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].FailedDevice = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Device;

        mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Valid = 1;

        //The following call will populate buddy rank structure
        status = FindNonFailedRankBank(NodeId, ddrCh, RegionNum);

        if(status == EFI_UNSUPPORTED) {
            return status;
        }
        /*Before initiating a Bank or Rank sparing operation that creates an adddc region, reducing the
         * error thresholds for the ranks that will be part of the adddc region to half their original value */
        PrimaryRank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank;
        NonFailedRank = mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank;
        if(mMemRas->pSysHost->var.common.stepping == H0_REV_SKX) {
          UpdateErrorThresholdForAdddcRegion(NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh),PrimaryRank);
          UpdateErrorThresholdForAdddcRegion(NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh),NonFailedRank);
        }
        DoSpareCopy(NodeId, ddrCh, Rank, NEW_SPARE_COPY, RankAdddcSparingMR, RegionNum);
        mAdddcStateStruct[NodeId][ddrCh] = RAdddcInProgress;
      }
      *IsEventHandled = TRUE;
    }
    //Region Number is Invalid.
    else {
      status = EFI_INVALID_PARAMETER;
      return status;
    }
  }

  //Do SddcPlusOne if no region is available
  else {
    if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
      StartSddcPlusOne  (NodeId, ddrCh ,Rank, IsEventHandled);
  }
  RASDEBUG((DEBUG_INFO,"</RankADDDCMR>\n"));
  return status;
}


/**

    Check Failed Granularity info - For now this always returns the gran as Bank level.
    This Call also populates the Strike Structure

    @param NodeId    - Memory controller ID
    @param  ddrCh - Failed Channel
    @param  StrikeNum - The Strike Number of the current strike

    @retval None

**/

VOID
PopulateFailedDevInfo(
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN PFAILED_DIMM_INFO pFailedDimmInfo
    )
{

  RASDEBUG((DEBUG_INFO,"<PopulateFailedDevInfo>\n"));

  RASDEBUG((DEBUG_INFO,"\t Populating Failed device information for Strike :%d\n",StrikeNum));

  if(pFailedDimmInfo->ValidBits & 01 ) {

    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].StrikeType  = BANK_STRIKE;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionType  = pFailedDimmInfo->RegionType;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionSubType  = pFailedDimmInfo->RegionSubType;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank        = pFailedDimmInfo->FailedRank;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].SubRank     = pFailedDimmInfo->FailedSubRank;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank        = pFailedDimmInfo->FailedBank;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].BankGroup   = pFailedDimmInfo->FailedBG;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Device      = pFailedDimmInfo->FailedDev;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Dimm        = (pFailedDimmInfo->FailedRank & BIT2)>>2;
    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Valid       = 1;
  }
  RASDEBUG((DEBUG_INFO,"</ PopulateFailedDevInfo>\n"));
  return;
}

/**

    Check and Handle ADDDC MR Event

    @param NodeId    - Memory controller ID
    @param *IsEventHandled - ptr to event handled status

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
CheckandHandleNewAdddcMREvent(
    IN  UINT8 NodeId,
    OUT BOOLEAN *IsEventHandled
 )
{
  EFI_STATUS   status = EFI_SUCCESS;
  UINT8        ddrCh = 0;
  UINT8        Rank  = 0;
  UINT8        FailedRank;
  UINT8        StrikeNum = 0;
  BOOLEAN      ErrorRankFound = FALSE;
  UINT8        PrevRegion;
  UINT8        PrevStrike;
  UINT8        RegionNum;
  UINT8        Ch;
  FAILED_DIMM_INFO FailedDimmInfo;
  CORRERRORSTATUS_MCDDC_DP_STRUCT corrErrStsReg;

  RASDEBUG((DEBUG_INFO,"<CheckandHandleNewAdddcMREvent>\n"));

  if(NodeId >= MC_MAX_NODE) {
    return EFI_INVALID_PARAMETER;
  }

  for(ddrCh = 0; ddrCh < MAX_MC_CH; ddrCh++ ) {
    StrikeNum = 0;

    //Do not attempt to read and clear rank error counters if the channel does not qualify for adddc (x8 dimms)
    if((mSystemMemoryMap->Socket[NODE_TO_SKT(NodeId)].ChannelInfo[NODECH_TO_SKTCH(NodeId, ddrCh)].Features & X8_PRESENT))
      continue;

    //Do not attempt to read and clear rank error counters If the channel has already exceeded the maximum possible strikes
    if(mAdddcSparingEventStruct[NodeId][ddrCh].NumStrikes >= MAX_STRIKE) {
      RASDEBUG((DEBUG_INFO,"\t Reached maximum supported strikes quit! \n"));
      continue;
    }

    for( Rank = 0; Rank < MAX_RANK_CH ; Rank++ ) {
      if(IsErrorExcdForThisRank( NodeId, ddrCh, Rank)) {
        corrErrStsReg.Data = mCpuCsrAccess->ReadCpuCsr( NODE_TO_SKT(NodeId), NODECH_TO_SKTCH(NodeId, ddrCh), CORRERRORSTATUS_MCDDC_DP_REG );
        RASDEBUG((DEBUG_INFO,"\t NodeId:%d, DdrCh:%d, err_overflow_stat:%x \n",NodeId,ddrCh,corrErrStsReg.Bits.err_overflow_stat));
        ErrorRankFound = TRUE;
      }
      if (ErrorRankFound == TRUE)
        break;
    }//for Rank
    if (ErrorRankFound == TRUE)
      break;
  }//for Ch

  if(!ErrorRankFound) {
    *IsEventHandled = FALSE;
    return status;
  }
  // find exact failed rank on the ddrc ch.
  status = GetFailedDimmErrorInfo(NodeId, ddrCh, 00, Rank, &FailedDimmInfo);
  if(status != EFI_SUCCESS) {
      // incorrect failed rank from retry read error log register.
      return status;
  }

  /*A0 WA 1: : 4930278: Cloned From SKX Si Bug Eco: Sparing in one channel causes RD block
   in another channel due to common sparing_in_progress signal used in all 3 chnls*/
  //A0 WA 1: Identify if adddcevents took place for any other channel
  if(mMemRas->pSysHost->var.common.stepping < B0_REV_SKX) {
    RASDEBUG((DEBUG_INFO,"\t A0 Stepping Checking for WA\n"));
    for(Ch=0;Ch<MAX_MC_CH;Ch++) {
      if(Ch ==  ddrCh)
        continue;
      if (mAdddcSparingEventStruct[NodeId][Ch].NumStrikes > 0) {
        RASDEBUG((DEBUG_INFO,"\t ADDDC on other channel. ADDDC Is only possible on 1 Channel per IMC on A0 returning!! \n"));
        return status;
      }
    }
  }

  StrikeNum = mAdddcSparingEventStruct[NodeId][ddrCh].NumStrikes;
  RASDEBUG((DEBUG_INFO,"\t PopulateFailedDevInfo[NodeId:%d][ddrCh:%d][StrikeNum:%d] \n",NodeId,ddrCh,StrikeNum));
  PopulateFailedDevInfo(NodeId, ddrCh, StrikeNum, &FailedDimmInfo);
  FailedRank = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank;

  //We can only do sddc once on a Rank if it was marked done for that Rank we return
  if(mSddcPlusOneStatus[NodeId][ddrCh][FailedRank] == SddcPlusOneDone) {
    *IsEventHandled = TRUE;
    return status;
  }

    //If Bank Granularity check if there was no previous strike and do just BankADDDC
    if (StrikeNum < 1) { //First strike
      BankADDDCMR(NodeId, ddrCh, StrikeNum, REGION_NUM_AUTO, IsEventHandled);
      *IsEventHandled = TRUE;
      return status;
    }
    else {   //Not First strike
      if (mSystemMemoryMap->SystemRasType == ADVANCED_RAS) {
        //Was any Previous Strike RANK
        if (AnyPreviousStrike(NodeId, ddrCh, StrikeNum, RANKVALID, &PrevStrike)) {
            RASDEBUG((DEBUG_INFO,"\t Previous Strike was RANK \n"));
          //Current Bank belongs to a RANK that had a previous strike
          if (AnyPreviousStrikeMatch(NodeId, ddrCh, StrikeNum, RANKVALID, &PrevStrike)) {
              RASDEBUG((DEBUG_INFO,"\t Previous Strike was on same RANK \n"));
              if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
                StartSddcPlusOne (NodeId, ddrCh, FailedRank, IsEventHandled);
              return status;
          }
          //No RANK level strike on same RANK before
          else if (AnyPreviousBuddy(NodeId, ddrCh,StrikeNum, RANKVALID, &PrevRegion, &PrevStrike)) { //Current failed bank.rank was previously used as buddy rank
              RASDEBUG((DEBUG_INFO,"\t Failed Bank belongs to a Rank previously used as Buddy Reverse! \n"));
              ReverseRemapRank (NodeId, ddrCh, StrikeNum, PrevStrike, PrevRegion, IsEventHandled);
              *IsEventHandled = TRUE;
              return status;
          }
        } //Current Strike Bank - Any Prev Strike Rank

        //Check if any previous strike was a bank
        if (AnyPreviousStrike(NodeId, ddrCh, StrikeNum, BANKVALID, &PrevStrike)) {
            RASDEBUG((DEBUG_INFO,"\t Previous strike was on a Bank \n"));
            //The Previous Bank Strike belongs to the Same rank as Current Bank Strike
            if(mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank == mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].Rank) {
                RASDEBUG((DEBUG_INFO,"\t Previous strike was on same Rank as Current Rank \n"));
                //Current and Previous strike was same Rank and Same Bank return and handle the case in sddcplusone
                RASDEBUG((DEBUG_INFO,"\t Previous strike was on a Bank RegionType =  %d \n", mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionType));
                if((mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Bank == mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].Bank)
                    && (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].BankGroup == mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].BankGroup)
                    && (mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionType & (1 << FailedRankInLSMode))) {

                  RegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].RegionNum;
                  //We cannot initiate sddcplusone for second strike on same bank if a VLS was established between banks of the same Rank
                  if(mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank == mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank) {
                    return status;
                  }

                  RASDEBUG((DEBUG_INFO,"\t Previous strike was on same Bank as Current Bank\n"));
                  if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
                    StartSddcPlusOne (NodeId, ddrCh, FailedRank, IsEventHandled);
                  return status;
                }

                //Current and Previous strike same device.
                else if(mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Device == mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].Device) {

                  RASDEBUG((DEBUG_INFO,"\t Previous strike was on same device upgrade!!\n"));
                  RegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].RegionNum;

                  //current and previous strike same device but we are on a channel with single DIMM of single Rank
                  if(mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank == mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank) {
                    ReverseAndUpgradePlusOne(NodeId, ddrCh, StrikeNum,PrevStrike,RegionNum,IsEventHandled);
                  }
                  //Upgrade to Rank
                  else{
                    RegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].RegionNum;
                    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].StrikeType = RANK_STRIKE;
                    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].StrikeType = RANK_STRIKE;
                    mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionNum = RegionNum;
                    UpgradeBank2Rank (NodeId, ddrCh,StrikeNum, RegionNum, IsEventHandled);
                  }
                  *IsEventHandled = TRUE;
                  return status;
                }

            }
            //Ensure current rank was not previously used as a nonfailed rank
            if (AnyPreviousBuddy(NodeId, ddrCh,StrikeNum,RANKVALID,&PrevRegion, &PrevStrike)) {
              RegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].RegionNum;
              //We cannot initiate sddcplusone for strike on buddy if a VLS was established between banks of the same Rank
              if(mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].PrimaryRank == mAdddcSparingEventStruct[NodeId][ddrCh].Region[RegionNum].BuddyRank) {
                return status;
              }

              RASDEBUG((DEBUG_INFO,"\t Previously used as a buddy reverse \n"));
              RASDEBUG((DEBUG_INFO,"\t Previous region number:%d \n",PrevRegion));
              ReverseRemapBank(NodeId, ddrCh, StrikeNum, PrevStrike, PrevRegion, IsEventHandled);
              *IsEventHandled = TRUE;
              return status;
            }

        }
        //Decision to create another Bank VLS if region is available
        BankADDDCMR(NodeId, ddrCh, StrikeNum, REGION_NUM_AUTO, IsEventHandled);

      } //ADVANCED SKU
      else {
        //Previously there was a Bank Strike
        if (AnyPreviousStrike(NodeId, ddrCh, StrikeNum, BANKVALID, &PrevStrike)){
          //Check Current Strike and any Previous strike was on same Bank
          if(mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].Rank == mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrike].Rank) {
            if (AnyPreviousStrikeMatch(NodeId, ddrCh, StrikeNum, BANKVALID, &PrevStrike)){
              if(mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress)
                StartSddcPlusOne (NodeId, ddrCh, FailedRank, IsEventHandled);
              return status;
            }
            //Current failed rank was previously used as a non failed Rank
            if (AnyPreviousBuddy(NodeId, ddrCh,StrikeNum,RANKVALID,&PrevRegion, &PrevStrike)) {
                ReverseRemapBank(NodeId, ddrCh, StrikeNum, PrevStrike, PrevRegion, IsEventHandled);
                *IsEventHandled = TRUE;
                return status;
            }
          }
          BankADDDCMR(NodeId, ddrCh, StrikeNum, REGION_NUM_AUTO, IsEventHandled);
        } else {
          //No Bank strikes before, attempt do create a bank VLS if region is available
          BankADDDCMR(NodeId, ddrCh, StrikeNum, REGION_NUM_AUTO, IsEventHandled);
        }
      } //NOT ADVANCED SKU
    } //StrikeNum >= 1

  return status;
}


/**

    Handle Adddc In progress event

    @param NodeId    - Memory controller ID
    @param *IsEventHandled - ptr to event handled status

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
HandleInProgressAdddcMREvent(
    IN  UINT8 NodeId,
    OUT BOOLEAN *IsEventHandled
 )
{
  UINT8 StrikeNum;
  UINT8 RegionNum;
  UINT8 PrevStrikeNum;
  UINT8 PrevRegionNum;
  UINT8 ddrCh;
  BOOLEAN     ChFound=FALSE;
  EFI_STATUS  status = EFI_SUCCESS;
  ADDDC_STATE_STRUCT AdddcCurrentState;

  for(ddrCh = 0;ddrCh<MAX_MC_CH;ddrCh++) {
    if (mAdddcStateStruct[NodeId][ddrCh] != NoAdddcInProgress) {
      ChFound = TRUE;
      break;
    }
  }

  if(ChFound) {
    AdddcCurrentState = mAdddcStateStruct[NodeId][ddrCh];
    StrikeNum = mAdddcSparingEventStruct[NodeId][ddrCh].NumStrikes;

    switch(AdddcCurrentState) {

      case (BAdddcInProgress):
        status = ClearAdddcStatusStruct(NodeId,ddrCh,StrikeNum,IsEventHandled);
      break;

      case (RAdddcInProgress):
        status = ClearAdddcStatusStruct(NodeId,ddrCh,StrikeNum,IsEventHandled);
      break;

      case (UB2RReverseInProgress):
        RegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].RegionNum;
        status = UpgradeBank2Rank (NodeId, ddrCh, StrikeNum, RegionNum, IsEventHandled);
      break;

      case (UB2RRAdddcInProgress):
        status = ClearAdddcStatusStruct(NodeId,ddrCh,StrikeNum,IsEventHandled);
      break;

      //Reverse and remap Bank in progress
      case (RRMBReverseInProgress):
      case (RRMB1SetInProgress):
      case (RRMB2SetInProgress):

        PrevStrikeNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].ReverseRemapPrevStrike;
        PrevRegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].RegionNum;
        status = ReverseRemapBank(NodeId,ddrCh,StrikeNum,PrevStrikeNum,PrevRegionNum,IsEventHandled);
      break;

      //Reverse and remap Rank in progress
      case (RRMRReverseInProgress):
      case (RRMR1SetInProgress):
      case (RRMR2SetInProgress):

        PrevStrikeNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].ReverseRemapPrevStrike;
        PrevRegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].RegionNum;
        status = ReverseRemapRank(NodeId,ddrCh,StrikeNum,PrevStrikeNum,PrevRegionNum,IsEventHandled);
      break;

      case (RUPReverseInProgress):
        PrevStrikeNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[StrikeNum].ReverseRemapPrevStrike;
        PrevRegionNum = mAdddcSparingEventStruct[NodeId][ddrCh].Strike[PrevStrikeNum].RegionNum;
        status = ReverseAndUpgradePlusOne(NodeId,ddrCh,StrikeNum,PrevStrikeNum,PrevRegionNum,IsEventHandled);
        break;

      default:
         status = EFI_UNSUPPORTED;     // Invalid event type
         break;
    }
  }
  return status;
}


/**
  Check if Adddc Sparing is possible for this node and update Event Handled accordingly

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
CheckAndHandleAdddcSparing(
  IN UINT8 NodeId,
  IN EVENT_TYPE EventType,
  OUT BOOLEAN * IsEventHandled
  )
{
  EFI_STATUS status = EFI_SUCCESS;
  RASDEBUG((DEBUG_INFO,"<CheckAndHandleAdddcSparing>\n"));

  if (mSystemMemoryMap->ExRasModesEnabled & ADDDC_EN){
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks Support
    if ((EventType == NEW_EVENTS) || (EventType == EVENTS_IN_PROGRESS)) {
        ZeroMem (gMemRasMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
        FillMemRecordBeforeHandling(NodeId, gMemRasMemoryErrorRecord, ERROR_EVENT_ADDDC);
    }
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support
    switch (EventType) {
    case NEW_EVENTS:
       RASDEBUG((DEBUG_INFO,"CheckAndHandleAdddcSparing: New Events\n"));
       status = CheckandHandleNewAdddcMREvent(NodeId, IsEventHandled);
       // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hooks Support
       OemReportStatusCode(ADDDC_START, gMemRasMemoryErrorRecord);
       // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hooks Support
       break;

    case EVENTS_IN_PROGRESS:
      RASDEBUG((DEBUG_INFO,"CheckAndHandleAdddcSparing: Events in Progress\n"));
       status = HandleInProgressAdddcMREvent(NodeId, IsEventHandled);
       // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hooks Support
       OemReportStatusCode(ADDDC_COMPLETE, gMemRasMemoryErrorRecord);
       // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hooks Support
       break;

    default:
       RASDEBUG((DEBUG_INFO,"CheckAndHandleAdddcSparing: Something terribly wrong\n"));
       status = EFI_UNSUPPORTED;     // Invalid event type
       break;
    } //Switch
  } else {
    RASDEBUG((DEBUG_INFO,"\t ADDDC Not Enabled Exiting\n"));
    *IsEventHandled = FALSE;
    status = EFI_SUCCESS;
  }

  RASDEBUG((DEBUG_INFO,"</ CheckAndHandleAdddcSparing>\n"));
  return status;
}

/**
    Checks if a rank can support adddc sparing

    @param NodeId - Memory Node
    @param ddrCh  - DDR CH id
    @param Rank   - Rank Id
    @param Bank   - Bank Id

    @retval TRUE if specified rank supports ADDDC sparing
    @retval FALSE if specified rank doesn't support adddc sparing

**/

BOOLEAN
IsAdddcAvailable(
    IN UINT8  NodeId,
    IN UINT8  ddrCh
 )
{
  return ((mSystemMemoryMap->ExRasModesEnabled & ADDDC_EN) && (mAdddcStateStruct[NodeId][ddrCh] == NoAdddcInProgress));
}

BOOLEAN
EFIAPI
IsRankInPlusOne(
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 rank
    )
{
  UINT8   AdddcRegion;
  UINT8   VlsRank = 0xff;
  BOOLEAN RankInPlusOne = FALSE;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT      AdddcCtrRegion0;

  UINT8   Socket;
  UINT8   SktCh ;
  Socket  = NODE_TO_SKT(NodeId);
  SktCh   = NODECH_TO_SKTCH(NodeId, ddrCh);

  for( AdddcRegion=0; AdddcRegion<mMaxRegion; AdddcRegion++) {
    AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, (ADDDC_REGION0_CONTROL_MC_MAIN_REG + AdddcRegion * 4));
    if (AdddcCtrRegion0.Bits.region_enable == 1){
      if ( (rank == (UINT8)AdddcCtrRegion0.Bits.failed_cs ) ){
        VlsRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
      } else if (rank == (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs) {
        VlsRank = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
      }
    }
  }

  if ( (mSddcPlusOneStatus[NodeId][ddrCh][rank]    == SddcPlusOneInProgress) ||
       (mSddcPlusOneStatus[NodeId][ddrCh][rank]    == SddcPlusOneDone) )
  {
    RankInPlusOne = TRUE;
  }
  if (VlsRank < MAX_RANK_CH) {
    if ((mSddcPlusOneStatus[NodeId][ddrCh][VlsRank] == SddcPlusOneInProgress) ||
        (mSddcPlusOneStatus[NodeId][ddrCh][VlsRank] == SddcPlusOneDone) ) {
      RankInPlusOne = TRUE;
    }
  }
  return RankInPlusOne;
}

/*
 * If Any adddc operaion is in progress or sddc operation is completed on a Rank
 * we are free to clear the error counters on that Rank
*/
BOOLEAN
EFIAPI
GetSpareInProgressState(
    IN        UINT8   NodeId,
    IN        UINT8   ddrCh,
    IN        UINT8   rank
    )
{
  //UINT8   rank;
  BOOLEAN CopyInProgress = FALSE;

  if ( (mAdddcStateStruct[NodeId][ddrCh] == BAdddcInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RAdddcInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == UB2RReverseInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == UB2RRAdddcInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RRMBReverseInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RRMB1SetInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RRMB2SetInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RRMRReverseInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RRMR1SetInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RRMR2SetInProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == SDDCPlus1InProgress) ||
       (mAdddcStateStruct[NodeId][ddrCh] == RUPReverseInProgress) )
       {
        CopyInProgress = TRUE;
  }

  //If Any rank in the channel has an sddc operation in progress
    if ( IsRankInPlusOne(NodeId,ddrCh,rank) ) {
         CopyInProgress = TRUE;
    }
  return CopyInProgress;
}

