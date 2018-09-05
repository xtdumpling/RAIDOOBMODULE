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


@file PatrolScrub.c

Patrol scrub support

**/

#include "PatrolScrub.h"
#include "RankSparing.h"
#include "SparingEngine.h"
#include "DramDeviceCorrection.h"


typedef struct {
  AMAP_MC_MAIN_STRUCT                 AMAPReg;
}CH_AMAP_STRUCT2;

typedef struct {
  SCRUBCTL_MC_MAIN_STRUCT              ScrubCtlReg;
  SCRUBADDRESSLO_MC_MAIN_STRUCT        ScrubAddrLoReg;
  SCRUBADDRESSHI_MC_MAIN_STRUCT        ScrubAddrHiReg;
  SCRUBADDRESS2LO_MCMAIN_STRUCT        ScrubAddrLo2Reg;
  SCRUBADDRESS2HI_MCMAIN_STRUCT        ScrubAddrHi2Reg;
  UINT32                               ScrubInterval;
  CH_AMAP_STRUCT2                      AMAPReg[MAX_MC_CH];
  
  SSRSTATETYPE                         SSRState;
  UINT8                                ScrubSpareCh;
  UINT8                                ScrubSpareRank;
} PATROL_SCRUB_STATUS_STRUC;

static
PATROL_SCRUB_STATUS_STRUC mPatrolScrubStatus[MC_MAX_NODE];

static
UINT8 mPatrolScrubFlag;

#define POISON_SCRUB_INTERVAL         0x2   // TODO: set this to max possible fastest scrub interval

BOOLEAN
EFIAPI
CheckPatrolScrubEnable(
)
/**

 Check if patrol scrub is enabled or not.

 @param none

 @retval TRUE -- patrol scrub is enabled.
 @retval FALSE -- patrol scrub is disabled.

 **/
{
  if (mPatrolScrubFlag){
    return TRUE;
  }
  else{
    return FALSE;
  }
}

/**

  Enable Patrol scrub engine to start from the last Address.

  @param NodeId - Memory controller ID

  @retval EFI_SUCCESS - always

  **/
EFI_STATUS
EFIAPI
ReEnablePatrolScrubEngine(
IN UINT8 Node
)
{
  UINT8 Socket = NODE_TO_SKT(Node);
  UINT8 Mc = NODE_TO_MC(Node);
  SCRUBCTL_MC_MAIN_STRUCT      ScrubCtlReg;
  
  //
  // Disable scrubbing bgefore enbaling the scrub address registers.
  //
  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  if (ScrubCtlReg.Bits.scrub_en == 1) {
    RAS_DEBUG((1, "ReEnablePatrolScrubEngine Scrub_enbale bit set. to be cleared before update. ScrubCtlReg.Data : %x \n", ScrubCtlReg.Data));
    ScrubCtlReg.Bits.scrub_en = 0;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);
  }
  //
  // Patrol scrub was enabled before. Re-enable it now and restore old register values
  //
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG, mPatrolScrubStatus[Node].ScrubAddrLoReg.Data);
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, mPatrolScrubStatus[Node].ScrubAddrHiReg.Data);
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESS2LO_MCMAIN_REG, (UINT32)mPatrolScrubStatus[Node].ScrubAddrLo2Reg.Data);
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESS2HI_MCMAIN_REG, (UINT32)mPatrolScrubStatus[Node].ScrubAddrHi2Reg.Data);

  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, mPatrolScrubStatus[Node].ScrubCtlReg.Data);

  RAS_DEBUG((4, "[PatrolScrub]Restore ScrubCtr:%x  ScrubHi:%x   ScrubLo:%x \n",mPatrolScrubStatus[Node].ScrubCtlReg.Data,
    mPatrolScrubStatus[Node].ScrubAddrHiReg.Data,mPatrolScrubStatus[Node].ScrubAddrLoReg.Data )); 

  //  }
  return EFI_SUCCESS;
}


/**

  Disable Patrol scrub engine and save patrol registers

  @param NodeId - Memory controller ID


  @retval EFI_SUCCESS - Always

  **/
EFI_STATUS
EFIAPI
DisablePatrolScrubEngine(
  IN UINT8 Node
){
  UINT8                        Socket = NODE_TO_SKT(Node);
  UINT8                        Mc     = NODE_TO_MC(Node);
  SCRUBCTL_MC_MAIN_STRUCT      ScrubCtlReg;

  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  if (ScrubCtlReg.Bits.scrub_en == 1) {
    RAS_DEBUG((4, "DisablePatrolScrubEngine : node = %x \n", Node));
    
    //
    //  disable scrubbing
    //
    ScrubCtlReg.Bits.scrub_en = 0;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);
  }
  
  return EFI_SUCCESS;
}


/**

  Save Patrol scrub engine registers

  @param NodeId - Memory controller ID


  @retval EFI_SUCCESS - Always

  **/
EFI_STATUS
EFIAPI
SavePatrolScrubEngine(
  IN UINT8 Node
)
{
  UINT8                        Socket = NODE_TO_SKT(Node);
  UINT8                        Mc     = NODE_TO_MC(Node);
  UINT8                        ChOnMc;
  SCRUBCTL_MC_MAIN_STRUCT      ScrubCtlReg;
  
  //
  // Save registers and disable scrubbing
  //
  mPatrolScrubStatus[Node].ScrubAddrLoReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG);
  mPatrolScrubStatus[Node].ScrubAddrHiReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);
  mPatrolScrubStatus[Node].ScrubAddrLo2Reg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESS2LO_MCMAIN_REG);
  mPatrolScrubStatus[Node].ScrubAddrHi2Reg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESS2HI_MCMAIN_REG);  
  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  mPatrolScrubStatus[Node].ScrubCtlReg.Data    =  ScrubCtlReg.Data;
  mPatrolScrubStatus[Node].ScrubInterval       = ScrubCtlReg.Bits.scrubinterval;

    for(ChOnMc = 0; ChOnMc < MAX_MC_CH; ChOnMc ++){
      mPatrolScrubStatus[Node].AMAPReg[ChOnMc].AMAPReg.Data =  mCpuCsrAccess->ReadCpuCsr(Socket,NODECH_TO_SKTCH(Mc,ChOnMc), AMAP_MC_MAIN_REG); 
    }

  RAS_DEBUG((4, "[PatrolScrub]Save ScrubCtr:%x  ScrubHi:%x   ScrubLo:%x \n",ScrubCtlReg.Data,mPatrolScrubStatus[Node].ScrubAddrHiReg.Data,mPatrolScrubStatus[Node].ScrubAddrLoReg.Data ));  
  return EFI_SUCCESS;
}

/**

    Update AMAP register before scrubbing non-spare ranks or
    after doing spare copy operation.
    AMAP register is from spare rank and failed rank which is map out current address map.

    @param NodeId  -- MC id
    @param Ch -- specified channel.
    @retval None

    **/
static
VOID
EFIAPI
UpdateAMAP(
IN  UINT8 Socket,
IN  UINT8 Mc,
IN  UINT8 ScrubSpareCh
)
{
  UINT32                     DimmAmap;
  UINT8                      i;
  UINT8                      physicalRank;
  UINT8                      Dimm;
  UINT8                      RankPerDimm;
  AMAP_MC_MAIN_STRUCT        AMAPReg;
  UINT8                      Node = SKTMC_TO_NODE(Socket, Mc);

  DimmAmap = 0;

  for (i = 0; i < MAX_RANK_CH; i++){
    //
    //scan spare rank array 
    //
    if (CheckRankState(Node, ScrubSpareCh, i, SPARERANKSTATUS_AVAILABLE)){
      physicalRank = GetLogicalToPhysicalRankId(Node, ScrubSpareCh, i);
      Dimm = physicalRank / 4;
      RankPerDimm = physicalRank % 4;
      DimmAmap |= (1 << (Dimm * 8 + RankPerDimm));
    }

    //
    //scan spare failed rank.
    //
    if (GetRankState(Node, ScrubSpareCh, i).Bits.rank_exist  &&
      GetRankState(Node, ScrubSpareCh, i).Bits.spare_rank  &&
      GetRankState(Node, ScrubSpareCh, i).Bits.rank_fail) {
      physicalRank = GetLogicalToPhysicalRankId(Node, ScrubSpareCh, i);
      Dimm = physicalRank / 4;
      RankPerDimm = physicalRank % 4;
      DimmAmap |= (1 << (Dimm * 8 + RankPerDimm));
    }

    //
    //scan non-spare failed rank which has done spare copy.
    //
    if (GetRankState(Node, ScrubSpareCh, i).Bits.rank_exist   &&
      GetRankState(Node, ScrubSpareCh, i).Bits.non_spare_rank &&
      (GetRankState(Node, ScrubSpareCh, i).Bits.rank_fail)) {
      if (CheckRankState(Node, ScrubSpareCh, i, NONSPARESTATUS_SPARECOPYDONE)){
        physicalRank = GetLogicalToPhysicalRankId(Node, ScrubSpareCh, i);
        Dimm = physicalRank / 4;
        RankPerDimm = physicalRank % 4;
        DimmAmap |= (1 << (Dimm * 8 + RankPerDimm));
      }
    }
  }//for

  //
  // Update AMAP to enable patrolling any rank
  //
  AMAPReg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, NODECH_TO_SKTCH(Node, ScrubSpareCh), AMAP_MC_MAIN_REG);
  RAS_DEBUG((4, "UpdateAMAP -  before AMAP value Channel:%d  reg:%lx   OrData:%lx\n", ScrubSpareCh, AMAPReg.Data, DimmAmap));

  AMAPReg.Bits.dimm0_pat_rank_disable = (UINT8)(DimmAmap & 0xff);
  AMAPReg.Bits.dimm1_pat_rank_disable = (UINT8)((DimmAmap >> 8) & 0xff);
  mCpuCsrAccess->WriteCpuCsr(Socket,  NODECH_TO_SKTCH(Node, ScrubSpareCh), AMAP_MC_MAIN_REG, AMAPReg.Data);

  RAS_DEBUG((4, "UpdateAMAP -  current AMAP value Channel:%d  reg:%lx\n", ScrubSpareCh, AMAPReg.Data));

}



/**

  do scrubbing on spare rank.

  @param NodeId  - Memory controller ID
  @param Ch - Failed Channel number
  @param Rank - Failed rank number
  @param ScrubSource -NON_SPARE_CE,SPARE_UC,SPARE_CE
  @retval None

  **/
static
VOID
EFIAPI
DoRankScrubbing(
IN UINT8 NodeId,
IN UINT8 Ch,
IN UINT8 Rank,
IN UINT8 ScrubSource
)
{
  UINT8                               NextSpareRank;
  UINT32                              SparePhysicalRank;
  UINT32                              SpareDimm;
  UINT32                              SpareRankPerDimm;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT     mcMainChknReg;
  AMAP_MC_MAIN_STRUCT                 AMAPReg;
  UINT8                               Socket = NODE_TO_SKT(NodeId);
  UINT8                               Mc = NODE_TO_MC(NodeId);
  UINT32                              PcodeMailboxData=0;
  EFI_STATUS                          Status;
  SCRUBADDRESSHI_MC_MAIN_STRUCT       ScrubAddrHiReg;
  SCRUBCTL_MC_MAIN_STRUCT             ScrubCtlReg;
  
  //
  //save spare scrub info.
  //
  mPatrolScrubStatus[NodeId].SSRState = SCRUBBING_SPARERANK;
  mPatrolScrubStatus[NodeId].ScrubSpareCh = Ch;
  GetLowestPrioritySpareRank(NodeId, Ch, &NextSpareRank);


  mPatrolScrubStatus[NodeId].ScrubSpareRank = NextSpareRank;

  //
  //update the source CE state.
  //
  SetRankState(NodeId, Ch, NextSpareRank, SPARERANKSTATUS_SCRUBBING);
  if ((ScrubSource == NON_SPARE_CE) || ((ScrubSource == NON_SPARE_CE_EVENT))){
    SetRankState(NodeId, Ch, Rank, NONSPARESTATUS_SCRUBBING);
  }
  
  //
  //stop scrubbing and save patrol scrubbing register.
  //
  SavePatrolScrubEngine(NodeId);
  DisablePatrolScrubEngine(NodeId);

  //switch to legacy mode.
  SwitchAddressModeToLegacy (NodeId);

  //
  //set patrol inteval to PCU.
  //
  PcodeMailboxData  = Mc; // BIT0 MC_INDEX (0 or 1)
  PcodeMailboxData  |= POISON_SCRUB_INTERVAL << 8;
  Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MAILBOX_BIOS_CMD_MC_PATROL_SCRUB_INTERVAL, PcodeMailboxData);
  ASSERT_EFI_ERROR(Status);

  
  //
  // if Spare rank available, then patrol spare rank
  // update ScrubSpareRankDdrCh to indicate the current DDR CH that is
  // patrolled
  //
  mcMainChknReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG);
  mcMainChknReg.Bits.defeature_12 = 1;      // Enable patrol to stop after rank is scrubbed
  mcMainChknReg.Bits.dis_ptrl_err_log = 1;  // Disable patrol from MCA logging errors when scrubbing spare rank
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG, mcMainChknReg.Data);

  //
  // Update AMAP to disable all other ranks except the specific rank.
  //
  AMAPReg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, NODECH_TO_SKTCH(NodeId, Ch), AMAP_MC_MAIN_REG);
  AMAPReg.Bits.dimm0_pat_rank_disable = 0x0f;
  AMAPReg.Bits.dimm1_pat_rank_disable = 0x0f;
  AMAPReg.Bits.dimm2_pat_rank_disable = 0x0f;
  SparePhysicalRank = GetLogicalToPhysicalRankId(NodeId, Ch, NextSpareRank);
  SpareDimm = SparePhysicalRank / 4;
  SpareRankPerDimm = SparePhysicalRank % 4;

  if (SpareDimm == 0){
    AMAPReg.Bits.dimm0_pat_rank_disable &= (~(1 << SpareRankPerDimm));
  }

  if (SpareDimm == 1){
    AMAPReg.Bits.dimm1_pat_rank_disable &= (~(1 << SpareRankPerDimm));
  }

  if (SpareDimm == 2){
    AMAPReg.Bits.dimm2_pat_rank_disable &= (~(1 << SpareRankPerDimm));
  }

  RAS_DEBUG((4, "UpdateAMAP for scrubbing -   AMAP value Channel:%d  reg:%lx\n", Ch, AMAPReg.Data));
  mCpuCsrAccess->WriteCpuCsr(Socket, NODECH_TO_SKTCH(NodeId, Ch), AMAP_MC_MAIN_REG, AMAPReg.Data); 
  
  //
  // Change patrol scrub address to scrub specified spare rank
  //       
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG, 0);
  
  ScrubAddrHiReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);
  ScrubAddrHiReg.Bits.chnl = Ch;
  ScrubAddrHiReg.Bits.rank = GetLogicalToPhysicalRankId(NodeId, Ch, NextSpareRank);
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHiReg.Data); 

  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  ScrubCtlReg.Bits.scrub_en = 1;
  ScrubCtlReg.Bits.startscrub = 1;
  ScrubCtlReg.Bits.scrubinterval = POISON_SCRUB_INTERVAL;
  ScrubCtlReg.Bits.stop_on_cmpl = 1;
  ScrubCtlReg.Bits.stop_on_err = 1;
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);

  RAS_DEBUG((1, "[RAS scrubbing end] Scrubbing spare rank  Skt:%d Mc:%d  Ch:%d  Rank:%d \n", Socket, Mc, Ch, NextSpareRank));
}


/**

  Handles patrol scrub one rank by one rank for specifid node and chanel. Assume there must be an available spare rank exist on this channel.
  In case ScrubSource is NON_SPARE_CE, it is CE happen on non-spare rank, it need scrub spare rank first, in case SSR is busy with sparecopying or scrub spare rank,
  log an event.
  In case ScrubSource is SPARE_UC, or SPARE_CE, it scrub spare rank, no need check SSR status for it must be scrubbing spare rank.
  Invoker make sure this channel has available spare rank exist.

  @param NodeId  - Memory controller ID
  @param Ch - Failed Channel number
  @param Rank - Failed rank number
  @param ScrubSource -NON_SPARE_CE,SPARE_UC,SPARE_CE
  @retval None

  **/
VOID
EFIAPI
ScrubNextRank(
IN UINT8 NodeId,
IN UINT8 Ch,
IN UINT8 Rank,
IN UINT8 ScrubSource
)
{
  UINT8                               FoundedCh;
  UINT8                               FoundedPendingRank;
  UINT8                               SourceRank;

  RAS_DEBUG((1, " [RAS scrubbing] ScrubNextRank Ch:%d\n", Ch));

  if (!CheckRankStatePerCh(NodeId, Ch, SPARERANKSTATUS_AVAILABLE, NULL)){

    //
    //note: for NON_SPARE_CE source, it can't go here, at invoker, it will check available spare rank.
    //     
    RAS_DEBUG((1, " [RAS scrubbing] No available spare rank for Ch:%d\n", Ch));

    //
    //for user case: if scrubing spare rank CE or UC, but no available spare rank exist,
    //start SSR whatever.
    // 
    ReEnablePatrolScrubEngine(NodeId);

    //
    //update the non spare rank which state is NONSPARESTATUS_SCRUBNEXT.
    //
    if (ScrubSource == NON_SPARE_CE_EVENT){
      SetRankState(NodeId, Ch, Rank, NONSPARESTATUS_TRY_TAG);

      //RAS_DEBUG((1," [RAS scrubbing] start device tag Node:%d  Ch:%d  Rank:%d\n",
      //    NodeId, Ch,R ank));

      //HandleSddcSparingForPendingEvent(NodeId, Ch, Rank);
    }

    if ((ScrubSource == SPARE_UC) || (ScrubSource == SPARE_CE))  {
      if (CheckRankStatePerCh(NodeId, Ch, NONSPARESTATUS_SCRUBNEXT, &SourceRank)){
        SetRankState(NodeId, Ch, SourceRank, NONSPARESTATUS_TRY_TAG);

        // RAS_DEBUG((1," [RAS scrubbing] start device tag Node:%d  Ch:%d  Rank:%d\n",
        //    NodeId, Ch, SourceRank));

        //HandleSddcSparingForPendingEvent(NodeId, Ch, SourceRank);
      } else{
        RAS_DEBUG((1, " [RAS scrubbing] error! No found pending source rank\n"));
        return;
      }
    }

    //
    //check if any left event here.
    //
    if (CheckRankStatePerMC(NodeId, NONSPARESTATUS_SCRUBNEXT, &FoundedCh, &FoundedPendingRank)){
      RAS_DEBUG((1, "[RAS scurb next] there is a pending CE event exist  Ch:%d   Rank:%d\n", FoundedCh, FoundedPendingRank));
      //   
      //check if available rank exist in this MC.   
      //
      ScrubNextRank(NodeId, FoundedCh, FoundedPendingRank, NON_SPARE_CE_EVENT);

    }

    return;
  }


  if ((ScrubSource == SPARE_CE) || (ScrubSource == SPARE_UC))  {
    if (mPatrolScrubStatus[NodeId].SSRState != SCRUBBING_SPARERANK){
      RAS_DEBUG((1, "[RAS scrubbing] SSR state:%d is wrong during scrub next rank 0\n", mPatrolScrubStatus[NodeId].SSRState));
      return;
    }

    if ((Ch != mPatrolScrubStatus[NodeId].ScrubSpareCh) || (Rank != mPatrolScrubStatus[NodeId].ScrubSpareRank)){
      RAS_DEBUG((1, "[RAS scrubbing] SSR state not match CE/US passdown\n"));
      return;
    }
    RAS_DEBUG((1, "[RAS scrubbing]  spare rank CE/UC  Mc:%d  Ch:%d  Rank:%d \n", NodeId, Ch, Rank));
  }

  if (ScrubSource == NON_SPARE_CE_EVENT){
    RAS_DEBUG((1, "[RAS scrubbing]  non-spare rank event  CE Mc:%d  Ch:%d  Rank:%d \n", NodeId, Ch, Rank));
  }

  if (ScrubSource == NON_SPARE_CE){
    //
    //in case multi overflow happen on one MC, and if SSR is busy with scrubbing or spare copy, 
    //just go out,  at  spare copy end, it will re-enter.
    //
    if (mPatrolScrubStatus[NodeId].SSRState != SCRUBBING_NON_SPARERANK){
      RAS_DEBUG((1, "[RAS scrubbing]  SSR is busy, go out. Skt:%d  Mc:%d  Ch:%d  Rank:%d  wait for SSR state:%d \n",
      NodeId, 0, Ch, Rank, mPatrolScrubStatus[NodeId].SSRState));
      return;
    }

    RAS_DEBUG((1, "[RAS scrubbing]  non-spare rank CE Skt:%d Mc:%d  Ch:%d  Rank:%d \n", NodeId, 0, Ch, Rank));
  }

  DoRankScrubbing(NodeId, Ch, Rank, ScrubSource);

  return;
}


static
EFI_STATUS
HandlePatrolCompleteEventForNode(
IN UINT8 Node
)
/**

 Handles patrol complete event for a node

 @param NodeId  - Memory controller ID

 @retval EFI_SUCCESS / Error code

 **/
{
  EFI_STATUS                          status = EFI_SUCCESS;
  UINT8                               SourceRank;
  UINT8                               DestRank;
  UINT8                               Ch = mPatrolScrubStatus[Node].ScrubSpareCh;
  UINT32                              errStatus;
  SCRUBCTL_MC_MAIN_STRUCT             ScrubCtlReg;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT     mcMainChknReg;
  UINT8                               RegionNum = 0;
  UINT32                              PcodeMailboxData = 0;
  UINT8                               TempDimm,TempRankOnDimm;
  AMAP_MC_MAIN_STRUCT                 AMAPReg;  

  UINT8 Socket = NODE_TO_SKT(Node);
  UINT8 Mc = NODE_TO_MC(Node);

  RAS_DEBUG((1, "[RAS patrol complete]HandlePatrolCompleteEventForNode - Enter for Skt:%d Mc:%d\n", Socket, Mc));

  //
  //get failed rank info
  //
  if (!CheckRankStatePerCh(Node, Ch, NONSPARESTATUS_SCRUBBING, &SourceRank)){
    RAS_DEBUG((1, "[RAS patrol complete] Error! didn't find the scrubbing non spare rank:%d\n"));
  }
  RAS_DEBUG((1, "[RAS patrol complete] CE failed rank:%d\n", SourceRank));
  
  //
  //get scrubbed rank info
  //
  CheckRankStatePerCh(Node, Ch, SPARERANKSTATUS_SCRUBBING, &DestRank);
  if (mPatrolScrubStatus[Node].ScrubSpareRank != DestRank){
    RAS_DEBUG((1, "ERROR! spare desti rank is not equal to SSR rank\n"));
  }
  RAS_DEBUG((1, "[RAS patrol complete] patrol scrub complete rank:%d\n", DestRank));

  //
  //update rank info
  //
  SetRankState(Node, Ch, DestRank, SPARERANKSTATUS_INPROGRESS);
  SetRankState(Node, Ch, SourceRank, NONSPARESTATUS_DOSPARECOPY);

  //
  //clear patrol scrub stop_on_cpml status first, and disable scrub.
  //update scrub info and restore other register before do spare copying.
  //
  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  ScrubCtlReg.Bits.ptl_cmpl = 0;
  ScrubCtlReg.Bits.stop_on_cmpl = 0;
  ScrubCtlReg.Bits.stop_on_err = 0;
  ScrubCtlReg.Bits.scrub_en = 0;
  ScrubCtlReg.Bits.scrubinterval = mPatrolScrubStatus[Node].ScrubInterval;
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);


  mcMainChknReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG);
  mcMainChknReg.Bits.defeature_12 = 0;      // Disable patrol to stop after rank is scrubbed
  mcMainChknReg.Bits.dis_ptrl_err_log = 0;  // enable patrol from MCA logging errors when scrubbing spare rank
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG, mcMainChknReg.Data);

  //
  //restore PCU setting for patrol scrub. 
  //
  SwitchAddressModeToSystem (Node);
  
  PcodeMailboxData  = Mc; // BIT0 MC_INDEX (0 or 1)
  PcodeMailboxData  |= mPatrolScrubStatus[Node].ScrubInterval << 8;
  status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MAILBOX_BIOS_CMD_MC_PATROL_SCRUB_INTERVAL, PcodeMailboxData);
  ASSERT_EFI_ERROR(status);


  //
  //update AMAP
  //

   
  AMAPReg.Data = mPatrolScrubStatus[Node].AMAPReg[Ch].AMAPReg.Data ;
  TempDimm = DestRank / 4;
  TempRankOnDimm = DestRank % 4;

  if (TempDimm == 0){
    AMAPReg.Bits.dimm0_pat_rank_disable &= (~(1 << TempRankOnDimm));
  }
  if (TempDimm == 1){
    AMAPReg.Bits.dimm1_pat_rank_disable &= (~(1 << TempRankOnDimm));
  }  
  
  TempDimm = SourceRank / 4;
  TempRankOnDimm = SourceRank % 4;
  if (TempDimm == 0){
    AMAPReg.Bits.dimm0_pat_rank_disable |= (1 << TempRankOnDimm);
  }
  if (TempDimm == 1){
    AMAPReg.Bits.dimm1_pat_rank_disable |= (1 << TempRankOnDimm);
  }    
  mCpuCsrAccess->WriteCpuCsr(Socket,NODECH_TO_SKTCH(Mc,Ch), AMAP_MC_MAIN_REG,AMAPReg.Data); 
  
  //
  //enable patrol scrub engine before sparing copy to prevent saved data is crushed by AcquireSparingEngine.
  //
  ReEnablePatrolScrubEngine(Node);


  //
  //update SSRState.
  //
  mPatrolScrubStatus[Node].SSRState = SCRUBBING_NON_SPARERANK;


  
  //
  //do spare copy operation
  //
  RAS_DEBUG((1, "[RAS patrol complete] do spare copying\n"));

  if (AcquireSparingEngine(Node) == EFI_SUCCESS){
    StartSparing(Node, SourceRank, DestRank, Ch, RankSparing, RegionNum);
  }

  //
  //update progress info
  //
  GetOrSetErrorHandlingStatus(Node, GetEventPendingStatus, &errStatus);
  if (!errStatus) {
    errStatus = 1;
    GetOrSetErrorHandlingStatus(Node, UpdateEventPendingStatus, &errStatus); // Save the error bitmap for internal use
  }
  errStatus = 1;
  GetOrSetErrorHandlingStatus(Node, UpdateEventProgressStatus, &errStatus);
  
  RAS_DEBUG((1, "[RAS patrol complete] end!\n"));
  return status;
}



/**

  Handles patrol UC event for a node

  @param NodeId  - Memory controller ID

  @retval EFI_SUCCESS / Error code

  **/
static
EFI_STATUS
HandlePatrolUCEventForNode(
IN  UINT8 NodeId
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  RAS_DEBUG((1, "[RAS patrol UC]  this is an spare rank UC\n"));

  if (mPatrolScrubStatus[NodeId].SSRState == SCRUBBING_SPARERANK){
    //
    //mark this rank as "dirty"
    //
    MarkRankDirtyFlag(NodeId, mPatrolScrubStatus[NodeId].ScrubSpareCh, mPatrolScrubStatus[NodeId].ScrubSpareRank);

    //
    //move to next rank scrub
    //
    ScrubNextRank(NodeId, mPatrolScrubStatus[NodeId].ScrubSpareCh, mPatrolScrubStatus[NodeId].ScrubSpareRank, SPARE_UC);
  }

  return Status;
}



EFI_STATUS
EFIAPI
CheckAndHandlePatrolEvent(
IN EFI_HANDLE        DispatchHandle,
IN CONST VOID        *DispatchContext, // optional
IN OUT   VOID        *CommBuffer,      // optional
IN OUT   UINTN       *CommBufferSize   // optional
)
/**

  Checks if patrol has completed on any node and handles it

  @param DispatchHandle - Dispatch handle (unused for now!)
  @param Dispatch Context - Context information (unused for now!)
  @param CommBuffer     - Buffer used for communication between caller/callback function (unused for now!)
  @param CommBufferSize - size of communication buffer (unused for now!)

  @retval EFI_SUCCESS / Error code

  **/
{
  UINT8                       Socket;
  UINT8                       Mc;
  UINT8                       Node;
  EFI_STATUS                  status = EFI_SUCCESS;
  SCRUBCTL_MC_MAIN_STRUCT     scrubCtlReg;


  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      if (!mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]){
        continue;
      }

      Node = SKTMC_TO_NODE(Socket, Mc);
      scrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
      if (scrubCtlReg.Bits.ptl_cmpl){
        RAS_DEBUG((1, "[RAS patrol]Patrol non_spare memory complete event start! \n"));
        HandlePatrolCompleteEventForNode(Node);
      }

      if (scrubCtlReg.Bits.ptl_stopped){
        RAS_DEBUG((1, "[RAS patrol]Patrol scrub spare rank UC event start \n"));
        HandlePatrolUCEventForNode(Node);
      }

    } // for Mc

  } // for Socket

  return status;
}


EFI_STATUS
InstallPatrolSmiHandler(
VOID
)
/**

 Install patrol SMI handler if required

 @param None

 @retval EFI_SUCCESS / Error code

 **/
{
  UINT8       rankSpareEnabled;
  EFI_STATUS  status = EFI_SUCCESS;
  EFI_HANDLE  imageHandle;

  //
  //each node has the same value of RasModesEnabled
  //Judge whole system RAS configuration here.
  //Judge each node if it support spare mode later.
  //
  rankSpareEnabled = mSystemMemoryMap->RasModesEnabled & RK_SPARE;

  //
  //check if support patrol scrub.
  //
  mPatrolScrubFlag = mSystemMemoryMap->ExRasModesEnabled & PTRLSCRB_EN;


  //
  if (rankSpareEnabled && CheckPatrolScrubEnable()) {
    //
    // We need to handle patrol  SMI event, install handler
    //
    imageHandle = NULL;
    status = gSmst->SmiHandlerRegister(
      CheckAndHandlePatrolEvent,
      NULL,
      &imageHandle
      );

    RASDEBUG((EFI_D_INFO, "[RAS Enable] Patrol scrub SMI handler installed\n"));
  }

  return status;
}

