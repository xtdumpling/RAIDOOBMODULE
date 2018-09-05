/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file DramDeviceCorrection.c

  Implements DRAM device correction functionality

**/

#include <Protocol/MemRasProtocol.h>
#include "MemRas.h"
#include "SparingEngine.h"
#include "DramDeviceCorrection.h"
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
#include <Library/TimerLib.h>
#include <Library\AmiErrReportingLib\AmiErrReportingLib.h>
#include <OemRasLib\OemRasLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support

extern  MEMRAS_S3_PARAM           mMemrasS3Param;
extern  SPARING_EVENT_STRUCT      mAdddcSparingEventStruct[MC_MAX_NODE][MAX_CH];
extern  UINT8                     mMaxRegion;
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook Support
extern UEFI_MEM_ERROR_RECORD      *gMemRasMemoryErrorRecord;
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook Support
#define MAX_MICRO_SECOND_DELAY    10
//
// Sddc Plus One status
//
SDDC_PLUSONE_STATUS mSddcPlusOneStatus[MC_MAX_NODE][MAX_MC_CH][MAX_RANK_CH];

/**

    Initialize all Sddc plus one  related data structures
    This routine should be called only once during boot

    @param None

    @retval Status

**/
EFI_STATUS
EFIAPI
InitializeSddcPlusOne(
 )
{
  UINT8 nodeId;
  UINT8 ddrCh;
  UINT8 rank;
 
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
  //
  // Initialize all sparing related data structures
  // here
  //
  if (mSystemMemoryMap->ExRasModesEnabled & SDDC_EN) {
    for(nodeId = 0; nodeId < MC_MAX_NODE; nodeId++) {
      for(ddrCh = 0; ddrCh < MAX_MC_CH; ddrCh++) {
        for(rank=0; rank< MAX_RANK_CH; rank++) {
          mSddcPlusOneStatus[nodeId][ddrCh][rank] = SddcPlusOneAvailable;
        }
      }
    }
  }
  return EFI_SUCCESS;
}

/**

    Handle SDDC Plus One done event

    @param NodeId    - Memory controller ID
    @param *IsEventHandled - ptr to a buffer to hold event handled status

    @retval status - EFI_SUCCESS if no failures
    @retval non-zero error code for failures

    IsEventHandled - TRUE -  event was handled
                     FALSE - event not handled

**/

EFI_STATUS
EFIAPI
HandleSddcPlusOneEventDone(
    IN  UINT8    NodeId,
    OUT BOOLEAN *IsEventHandled
)
{
  UINT8       DdrCh = 0;
  EFI_STATUS  Status  = EFI_SUCCESS;
  BOOLEAN     PendingSddcChFound = FALSE;
  UINT32      errStatus;
  UINT8       Rank = 0;
  for(DdrCh = 0; DdrCh < MAX_MC_CH; DdrCh++) {
    for(Rank =0; Rank < MAX_RANK_CH; Rank++){
      if  (mSddcPlusOneStatus[NodeId][DdrCh][Rank] == SddcPlusOneInProgress ) {
        PendingSddcChFound = TRUE;
        break;
      }
    }
    if(PendingSddcChFound) { //ch already found exit loop and handle error
      break;
    }
  }

  if((Rank>=MAX_RANK_CH) || (DdrCh>=MAX_MC_CH)) {
    RASDEBUG((DEBUG_INFO, "No SddcPlusOne in Progress \n"));
    return Status;
  }

  if (PendingSddcChFound) {

    Status = IsCopyComplete(NODE_TO_SKT(NodeId), NODE_TO_MC(NodeId));
    switch( Status ){
    case EFI_SUCCESS:
      RAS_DEBUG((1, "[RAS pending event] spare copy complete! \n"));

      Status = ReleaseSparingEngine( NodeId );
      DEBUG((DEBUG_INFO, "\t SDDC Spare copy done handled successfully\n"));

      //Increment Number of strikes after the strike for channel after handled successfully
      DEBUG((DEBUG_INFO,"\t Increment Number of Strikes \n"));
      mAdddcSparingEventStruct[NodeId][DdrCh].NumStrikes++;

      errStatus = 0;
      RASDEBUG((DEBUG_INFO,"\t HandleSddcPlusOneEventDone: Calling update events progress and pending status\n"));
      GetOrSetErrorHandlingStatus(NodeId, UpdateEventProgressStatus, &errStatus );
      GetOrSetErrorHandlingStatus(NodeId, UpdateEventPendingStatus, &errStatus );

      mSddcPlusOneStatus[NodeId][DdrCh][Rank] = SddcPlusOneDone;
      *IsEventHandled = TRUE;
      Status = EFI_SUCCESS;
      break;

    case EFI_NOT_READY:   // copy operation in progress
      RASDEBUG((DEBUG_ERROR, "\t spare copy not complete! Copy in progress for Node[%d].CH[%d]\n", NodeId, DdrCh));
        Status = EFI_SUCCESS;
        *IsEventHandled = FALSE;
      break;

    default:              // Error in operation
      *IsEventHandled = FALSE;
      break;
    }
  } else {
    RASDEBUG((DEBUG_INFO,"\t SDDC Operation not in progress exiting\n"));
    *IsEventHandled = FALSE;
    Status = EFI_SUCCESS;
  }
  return Status;
}
/**

    Check for SDDC Plus One events and handle them

    @param NodeId    - Memory controller ID
    @param *IsEventHandled - ptr to buffer to hold event handled status
    @retval status - EFI_SUCCESS if no failures
    @retval non-zero error code for failures

    IsEventHandled - TRUE -  event was handled
                     FALSE - event not handled

**/

EFI_STATUS
EFIAPI
HandleNewSddcPlusOneEventsIfAny(
    IN UINT8 NodeId,
    OUT BOOLEAN *IsEventHandled
 )
{
  UINT8         DdrCh =0;
  UINT8         Rank=0;
  EFI_STATUS    Status = EFI_SUCCESS;
  BOOLEAN       HardErrorFlag = FALSE;
  UINT32        errStatus;

  for (DdrCh = 0; DdrCh < MAX_MC_CH; DdrCh++ ){
    for (Rank = 0; Rank < MAX_RANK_CH; Rank++ ){
      if(IsErrorExcdForThisRank(NodeId, DdrCh, Rank)){
        RASDEBUG((DEBUG_INFO, "HandleNewSddcPlusOneEventsIfAny - NodeId:%d.ddrCh%d.rank%d exceeded threshold\n", NodeId, DdrCh, Rank));
        HardErrorFlag = TRUE;
        break;
      }
    }
    if(HardErrorFlag) { //Error Rank Detected and Handled
      HardErrorFlag = FALSE;
      StartSddcPlusOne(NodeId, DdrCh, Rank, IsEventHandled);
      if(*IsEventHandled) {
        GetOrSetErrorHandlingStatus(NodeId, GetEventPendingStatus, &errStatus);
        if (!errStatus) {
          errStatus = 1;
          GetOrSetErrorHandlingStatus(NodeId, UpdateEventPendingStatus, &errStatus); // Save the error bitmap for internal use
        }
        errStatus = 1;
        GetOrSetErrorHandlingStatus(NodeId, UpdateEventProgressStatus, &errStatus);
      }
      
      break;//DdrCh
    }
  }

  return Status;
}

/**
  Disable Rank SMI interrupt

    @param NodeId    - Memory controller ID
    @param ddrCh     - Channel ID
    @param Rank      - Failed Rank

    @retval None

**/
VOID
EFIAPI
DisableSmiOnRank (
  IN  UINT8     NodeId,
  IN  UINT8     DdrCh,
  IN  UINT8     Rank
  )
{
    DIS_CORR_ERR_LOG_MCDDC_DP_STRUCT     CorrErrLog;
    UINT8                                Socket;
    UINT8                                Ch;
    Socket = NODE_TO_SKT (NodeId);
    Ch     = NODECH_TO_SKTCH (NodeId, DdrCh);

    RASDEBUG((DEBUG_INFO, "Error on SVL Rank. Disable SMI on Rank %d\n",Rank));
    CorrErrLog.Data = mCpuCsrAccess->ReadCpuCsr (Socket, Ch, DIS_CORR_ERR_LOG_MCDDC_DP_REG);
    CorrErrLog.Data |= (1 << Rank);
    mCpuCsrAccess->WriteCpuCsr (Socket, Ch, DIS_CORR_ERR_LOG_MCDDC_DP_REG, CorrErrLog.Data);
}


/**

    SDDCPlusOne : Push the failed Rank to SDDCPlus1 Mode

    @param NodeId    - Memory controller ID
    @param ddrCh     - Channel ID
    @param Rank      - Failed Rank
    @param *IsEventHandled - ptr to event handled status

    @retval status - EFI_SUCCESS if no failures \ non-zero error code for failures

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
EFI_STATUS
EFIAPI
StartSddcPlusOne (
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  Rank,
    OUT BOOLEAN *IsEventHandled
  )
{
  UINT8                                       Ch;
  UINT8                                       CurrentConfig=0xFF;
  UINT8                                       FailedCh=0;
  UINT8                                       FailedRank= 0;
  UINT8                                       FailedDIMM=0;
  UINT8                                       FailedDev=0;
  UINT8                                       Plus1Rank = 0;
  EFI_STATUS                                  Status = EFI_SUCCESS;
  UINT8                                       Socket = NODE_TO_SKT(NodeId);
  UINT8                                       RegNum=0;
  UINT8                                       Plus1BuddyRank=0;
  UINT8                                       Mc = NODE_TO_MC(NodeId);
  UINT8                                       BuddyCs = 0;
  BOOLEAN                                     BuddyCsEn = FALSE;
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  SPARING_CONTROL_SOURCE_MC_MAIN_STRUCT       spareCtlSrc;
  SPARING_CONTROL_DESTINATION_MC_MAIN_STRUCT  spareCtlDest;
  PLUS1_RANK0_MCDDC_DP_STRUCT                 Plus1RankOffset;
  MCMTR_MC_MAIN_STRUCT                        McmtrMain;
  FAILED_DIMM_INFO                            FailedDimmInfo;

  *IsEventHandled       = FALSE;

  RASDEBUG((DEBUG_INFO,"<StartSddcPlusOne>\n"));

  Status = GetFailedDimmErrorInfo(NodeId, DdrCh, 00, Rank, &FailedDimmInfo);
  if(Status == EFI_SUCCESS){
    FailedCh    = FailedDimmInfo.SktCh;
    FailedRank  = FailedDimmInfo.FailedRank;
    FailedDev   = FailedDimmInfo.FailedDev;
    FailedDIMM  = FailedDimmInfo.FailedDimm;

  }else{
    RASDEBUG((DEBUG_INFO,"\tGet FailedDimm info failed\n"));
    return Status;
  }

  if (mSddcPlusOneStatus[NodeId][DdrCh][FailedRank] == SddcPlusOneAvailable) {


      // Identify the  current configuration
    if (FailedDimmInfo.ddr3_width == 0 )  {      // is it X4
  
      /*A0 WA 1: : 4930278: Cloned From SKX Si Bug Eco: Sparing in one channel causes RD block
       in another channel due to common sparing_in_progress signal used in all 3 chnls*/
      //A0 WA 1: Identify if adddcevents took place for any other channel
      if(mMemRas->pSysHost->var.common.stepping < B0_REV_SKX) {
        for(Ch=0;Ch<MAX_MC_CH;Ch++) {
          if(Ch ==  DdrCh)
            continue;
          if (mAdddcSparingEventStruct[NodeId][Ch].NumStrikes > 0) {
            RASDEBUG((DEBUG_INFO,"\t ADDDC on other channel. SDDDC plus 1 not possible for A0!! \n"));
            *IsEventHandled = TRUE;
            return Status;
          }
        }
      }

      if (IsThisRankInADDDCRegion(NodeId, DdrCh, FailedRank, &RegNum, &Plus1BuddyRank))
        CurrentConfig = X4DIMM_ADDDC;
       else
        CurrentConfig = X4DIMM_NoADDDC;

    } else {    // is it X8
      
      if (FailedDimmInfo.RegionType & (1 << ErrInSvlsReg)) {

        //A0 WA 1: Check this is the only channel populated in that node
        RASDEBUG((DEBUG_INFO,"\t Virtual Lockstep enabled\n"));
        if(mMemRas->pSysHost->var.common.stepping < B0_REV_SKX) {
          for(Ch=0;Ch<MAX_MC_CH;Ch++) {

            RASDEBUG((DEBUG_INFO,"\t Channel %d Enabled:%d\n",Ch,mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].Enabled));

            if(Ch ==  DdrCh)
              continue;

            if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].Enabled) {
              RASDEBUG((DEBUG_INFO,"\t More than one channel populated. SDDDC plus 1  not possible for A0!! \n"));
              *IsEventHandled = TRUE;
              return Status;
            }
          }
        }

        CurrentConfig = X8DIMM_VLS;
      }

      else {
        CurrentConfig = X8DIMM_NoVLS;
      }
    }

    // calculate the Plus1Rank,FailedDev for Current Configuration
    switch (CurrentConfig) {
      case X8DIMM_VLS:
        /*Plus1Rank = FailedRank;
        BuddyCs = (Plus1Rank + 4 ) % 8 ;
        BuddyCsEn = TRUE;*/
        DisableSmiOnRank (NodeId, DdrCh, Rank);
        *IsEventHandled = FALSE;
        RASDEBUG((DEBUG_INFO, "\t StartSddcPlusOne X8 DIMM in VLS - No plus one\n"));
        return Status;

      case X8DIMM_NoVLS:
        *IsEventHandled = FALSE;
        return Status;

      case X4DIMM_ADDDC:
        Plus1Rank = FailedRank;
        BuddyCs = FailedDimmInfo.BuddyRank; // = Plus1BuddyRank
        BuddyCsEn = TRUE;
        RASDEBUG((DEBUG_INFO, "\t StartSddcPlusOne X4 DIMM in ADDDC - FailedDev:%d Plus1Rank %d. Buddy Rank %d\n", FailedDev, Plus1Rank, BuddyCs));
        *IsEventHandled = TRUE;
        break;

      case X4DIMM_NoADDDC:
        Plus1Rank = FailedRank;
        BuddyCs = 0;
        BuddyCsEn = FALSE;
        RASDEBUG((DEBUG_INFO, "\t StartSddcPlusOne X4 DIMM no ADDDC - FailedDev:%d Plus1Rank %d. \n", FailedDev, Plus1Rank));
        *IsEventHandled = TRUE;
        break;

      default:
        RASDEBUG((DEBUG_INFO, "\t SDDC Plus1 for INVALID CONFIGURATION \n"));
        return Status;
    }

    //
    // Program SPARING_CONTROL_SOURCE, SPARING_CONTROL_DESTINATION and SPARING_CONTROL
    //
    spareCtlSrc.Data = (UINT16) mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG);
    spareCtlSrc.Bits.source_cs = Plus1Rank;
    spareCtlSrc.Bits.source_c = 0;
    spareCtlSrc.Bits.source_ba = 0;
    spareCtlSrc.Bits.source_bg = 0;
    spareCtlSrc.Bits.buddy_cs_en = (UINT8)BuddyCsEn;
    spareCtlSrc.Bits.buddy_cs = BuddyCs;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG, spareCtlSrc.Data);

    spareCtlDest.Data = (UINT16) mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG);
    spareCtlDest.Bits.destination_cs = Plus1Rank;
    spareCtlDest.Bits.destination_c = 0;
    spareCtlDest.Bits.destination_ba = 0;
    spareCtlDest.Bits.destination_bg = 0;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG, spareCtlDest.Data);

    spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
    spareCtlReg.Bits.reverse=0;
    spareCtlReg.Bits.rank_sparing = 0;
    spareCtlReg.Bits.sddc_sparing = 1;
    spareCtlReg.Bits.adddc_sparing = 0;
    spareCtlReg.Bits.channel_select = DdrCh;
    spareCtlReg.Bits.region_size = 01;        // for Rank
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

    //
    //Program Plus1Rank
    //
    Plus1RankOffset.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket,NODECH_TO_SKTCH(NodeId, DdrCh),(PLUS1_RANK0_MCDDC_DP_REG+Plus1Rank));
    Plus1RankOffset.Bits.faildevice = FailedDev;
    mCpuCsrAccess->WriteCpuCsr(Socket, NODECH_TO_SKTCH(NodeId, DdrCh), (PLUS1_RANK0_MCDDC_DP_REG + Plus1Rank), Plus1RankOffset.Data);

    Plus1RankOffset.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket,NODECH_TO_SKTCH(NodeId, DdrCh),(PLUS1_RANK0_MCDDC_DP_REG+BuddyCs));
    Plus1RankOffset.Bits.faildevice = FailedDev;
    mCpuCsrAccess->WriteCpuCsr(Socket, NODECH_TO_SKTCH(NodeId, DdrCh), (PLUS1_RANK0_MCDDC_DP_REG + BuddyCs), Plus1RankOffset.Data);

    //Program MCMTR.addc_mode = 1
     McmtrMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG);
     McmtrMain.Bits.adddc_mode = 1;
     mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG, McmtrMain.Data);
  
    RASDEBUG((DEBUG_INFO, "StartSddcPlusOne Programming SPARING_CONTROL regs and Plus1 Reg are done \n"));
    DoSpareCopy(NodeId, DdrCh, FailedRank, NEW_SPARE_COPY, SddcPlusOne, RegNum);
   
    mAdddcSparingEventStruct[NodeId][DdrCh].SddcPlusOneDone = TRUE;
  
    // Need to update the SDDCPlus 1 state for this and quit SMI
     mSddcPlusOneStatus[NodeId][DdrCh][FailedRank] = SddcPlusOneInProgress;

  }
    
  if (mSddcPlusOneStatus[NodeId][DdrCh][FailedRank] == SddcPlusOneDone) {
    *IsEventHandled = TRUE;
  } 

  RASDEBUG((DEBUG_INFO,"</StartSddcPlusOne>\n"));
  return Status;
}


BOOLEAN
IsSddcPlusOneAvailable(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  Rank
 )
{
  //RASDEBUG((DEBUG_INFO," NodeId %x, DdrCh : %x : Rank : %x,\n",NodeId, DdrCh, Rank));
  //RASDEBUG((DEBUG_INFO," RasModesEnabled : %x,mSddcPlusOneStatus : %\n",mSystemMemoryMap->RasModesEnabled, mSddcPlusOneStatus[NodeId][DdrCh][Rank] ));
  return (( mSddcPlusOneStatus[NodeId][DdrCh][Rank] == SddcPlusOneAvailable) && (mSystemMemoryMap->ExRasModesEnabled & SDDC_EN));
}
/**

    Call to find whether rank in the ADDDC region (either Primary or Buddy ) 

    @param NodeId       - Memory controller ID
    @param DdrCh        - Channel ID
    @param Rank         - Rank that we are matching with 
    @param RegionNum    - Used to indicate if we are identifying bank or rank

    @retval BOOLEAN     - TRUE if previous rank/bank buddy FALSE if not used as previous rank/bank buddy

    @retval IsEventHandled - TRUE -  event was handled \ FALSE - event not handled

**/
BOOLEAN
IsThisRankInADDDCRegion(
    IN UINT8 NodeId,
    IN UINT8 DdrCh,
    IN UINT8 Rank, 
    OUT UINT8 *RegionNum,
    OUT UINT8 *Plus1BuddyRank
    )
{
  BOOLEAN Status=FALSE;
  UINT8 Socket = NODE_TO_SKT(NodeId);
  UINT8 RegNum;

  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT		AdddcRegionReg;
  
  for(RegNum=0; RegNum < mMaxRegion; RegNum++) {
    AdddcRegionReg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, NODECH_TO_SKTCH(NodeId,DdrCh), (ADDDC_REGION0_CONTROL_MC_MAIN_REG + (RegNum*4)));
    if (AdddcRegionReg.Bits.region_enable == 1 ) { 
      if ( AdddcRegionReg.Bits.failed_cs == Rank ) { 
          Status = TRUE; 
          *RegionNum = RegNum;
          *Plus1BuddyRank = (UINT8)AdddcRegionReg.Bits.nonfailed_cs;
          break;
      } else if (  AdddcRegionReg.Bits.nonfailed_cs == Rank ) { 
          Status = TRUE; 
          *RegionNum = RegNum;
          *Plus1BuddyRank = (UINT8)AdddcRegionReg.Bits.failed_cs;
          break;
      }
    }
  }
  return Status;
}


/**
  Check if SDDC Plus One is enabled and perform that

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
CheckAndHandleSddcPlusOneSparing(
  IN UINT8 NodeId,
  IN EVENT_TYPE EventType,
  OUT BOOLEAN * IsEventHandled
  )
{
  EFI_STATUS status = EFI_SUCCESS;

  RASDEBUG((DEBUG_INFO,"<CheckAndHandleSddcPlusOneSparing>\n"));

  if (mSystemMemoryMap->ExRasModesEnabled & SDDC_EN) {
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks Support
    if ((EventType == NEW_EVENTS) || (EventType == EVENTS_IN_PROGRESS)) {
        ZeroMem (gMemRasMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
        FillMemRecordBeforeHandling(NodeId, gMemRasMemoryErrorRecord, ERROR_EVENT_SDDC);
    }
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support
    switch (EventType) {
    case NEW_EVENTS:
       status = HandleNewSddcPlusOneEventsIfAny(NodeId, IsEventHandled);
      // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hooks Support
       OemReportStatusCode(SDDC_START, gMemRasMemoryErrorRecord);
      // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hooks Support
       break;

    case EVENTS_IN_PROGRESS:
      status = HandleSddcPlusOneEventDone(NodeId, IsEventHandled);
      // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hooks Support
      OemReportStatusCode(SDDC_COMPLETE, gMemRasMemoryErrorRecord);
     // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hooks Support
      break;

    default:
       RASDEBUG((DEBUG_INFO,"\t Something went wrong!!\n"));
       status = EFI_UNSUPPORTED;     // Invalid event type
       break;
    }
  } else {
    RASDEBUG((DEBUG_INFO,"\t SDDC Plus One Not Enabled Exiting\n"));
    *IsEventHandled = FALSE;
    status = EFI_SUCCESS;
  }
  RASDEBUG((DEBUG_INFO,"/ <CheckAndHandleSddcPlusOneSparing>\n"));
  return status;
}


