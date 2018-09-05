/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2011 - 2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file MemRasRoutines.c

    Mem Ras functions and platform data

---------------------------------------------------------------------------**/
// APTIOV_SERVER_OVERRIDE_RC_START : For AdddcCtrRegion0 uninitialized variable build error
#pragma warning ( disable : 4701 )
// APTIOV_SERVER_OVERRIDE_RC_END : For AdddcCtrRegion0 uninitialized variable build error

#include "MemRas.h"
#include "RankSparing.h"
#include "DramDeviceCorrection.h"
#include "AdddcSparing.h"
#include <CpuPciAccess.h>
#include <Ras/RasStateMachine.h>

extern UINT8                              mMemQuiesceEnabled;
extern QUIESCE_DATA_TABLE_ENTRY           *mCurrQuiescePtr;
extern QUIESCE_DATA_TABLE_ENTRY           *mStartQuiescePtr;
extern EFI_CRYSTAL_RIDGE_PROTOCOL         *mCrystalRidgeProtocol;
extern UINT8                              mMaxRegion;

extern SYSHOST                            mHost;
EFI_QUIESCE_SUPPORT_PROTOCOL              *mQuiesceSupport;
EFI_RAS_MPLINK_PROTOCOL                   *mRasMpLink;

UINT64                                    *m2QuiescePtr;

typedef struct {
  UINT8 DdrCh;
  UINT8 Dimm;
  UINT8 Rank;
}LAST_ERR_INFO;

#define PREPARE_QUIESCE    1
#define RELEASE_QUIESCE    2
#define ABORT_QUIESCE      3
#define EXECUTE_QUIESCE    4

#define   PAM_DRAM_DISABLED   0x0
#define   PAM_DRAM_READ_ONLY  0x1
#define   PAM_DRAM_WRITE_ONLY 0x2
#define   PAM_DRAM_RW         0x3
#define   MAX_X4_DIMM_DEVID_CH0   17
#define   MAX_X4_DIMM_DEVID_CH1   35
#define   MAX_X8_DIMM_DEVID_CH0   8
#define   MAX_X8_DIMM_DEVID_CH1   17
#define   ADDDC_SPARE_DEVICE      17
//
// Global variables to keep track of event status across
// memory nodes.  BIT0 represents Memory Node 0, BIT1 is NODE1
// and so on
//
static UINT32   mEventProgressBitmap;   // Keeps track of memory nodes that have events in progress
static UINT32   mEventPendingBitmap;    // Keeps track of memory nodes that have new errors
static
LAST_ERR_INFO   mLastErrInfo[MC_MAX_NODE];

static  BOOLEAN             IsPamOpen = FALSE;
static  UINT32              OldPam0123RegVal;
static  UINT32              OldPam456RegVal;
static  UINT8               origSktIdOfMigTarget;  // Used to preserve original Skt Id of migration target



//SKX TODO START: Following block of code should be in MRC, Code can implemented in MRC Chip, but RasMemMap need to expose through
//MRC CODE Include, keep temp implementation here, will discuss with MEM MAP owner on this later 
extern UINT32 InitMemoryMap(PSYSHOST host);
extern UINT32 InitADR(PSYSHOST host);
extern UINT32 SetRASConfig(PSYSHOST host);
extern UINT32 MemLate(PSYSHOST host);
static UINT32 mMemMapFuncInx;
UINT32 (*mMemMapTable[])(PSYSHOST    host) = {
  InitMemoryMap,
  InitADR,
  SetRASConfig,
  MemLate,
  NULL
};

/**

    This function loop through function call table to execute all MEM MAP related functions

    @param host - Pointer to sysHost

    @retval Status - EFI_STATUS

**/
EFI_STATUS 
RasMemMap(PSYSHOST    host)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  mMemMapFuncInx = 0;
  while ((Status == EFI_SUCCESS) && (mMemMapTable[mMemMapFuncInx] != NULL)) {
    Status = (mMemMapTable[mMemMapFuncInx])(host);
    mMemMapFuncInx++;
  }

  if((Status != EFI_SUCCESS) || (mMemMapTable[mMemMapFuncInx] == NULL)) {
    mMemMapFuncInx = 0;
  }

  return Status;
}
//SKX TODO END

/**

    This function update systemMemoryMap struct for online socket

    @param host - Pointer to sysHost

    @retval None

**/
VOID
UpdateSystemMemoryMapStruct(
  PSYSHOST    host
  )
{
  struct SystemMemoryMapHob             *systemMemoryMap = mMemRas->SystemMemInfo;
  UINT8                                 socket, i, mc, ch, dimm, rank;
  UINT16                                MemInfoMemFreq;
  UINT8                                 MemInfoDdrVoltage;
  UINT8                                 ScratchCounter = 0;
  struct memVar                         *mem;
  UINT32                                MemInfoTotalMem;

  socket = host->var.common.inComingSktId;

  if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) return;
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return;

  MemInfoMemFreq = host->nvram.mem.socket[socket].ddrFreqMHz*2;
  MemInfoDdrVoltage = host->nvram.mem.socket[socket].ddrVoltage;

  MemInfoTotalMem = (host->var.mem.memSize << 6) ;  // Memory size in 1MB units

  //Init Memory map structure here for NUMA
  //The below code is invalid for UMA case.
  if (host->setup.mem.options & NUMA_AWARE) {
    mem = &(host->var.mem);

    // For each SAD entry in system
    for(i = 0; i < SAD_RULES; i++) {
      // If SADTable entry is active, populate information into HOB
      if((mem->socket[socket].SAD[i].Enable) && (mem->socket[socket].SAD[i].IotEnabled != 1)) { //To ensure that IOT is not reported to systemMemoryMap/SRAT, other tables

        //If this SAD is created to map to Control region of the AEP dimms, we need to ignore it.
        if (mem->socket[socket].SAD[i].type == MEM_TYPE_CTRL) continue;
          
        //Initialize the socketId variable based on the socket number. This will be used in SRAT creation          
        systemMemoryMap->Element[ScratchCounter].SocketId = socket; 
        
        systemMemoryMap->Element[ScratchCounter].NodeId = mem->socket[socket].SADintList[i][0]; // Get Node ID
        if(i) {
          systemMemoryMap->Element[ScratchCounter].BaseAddress =  mem->socket[socket].SAD[i-1].Limit;
          systemMemoryMap->Element[ScratchCounter].ElementSize = (mem->socket[socket].SAD[i].Limit - mem->socket[socket].SAD[i-1].Limit); // Get size of current element
        } else {
          //
          // If its the First SAD, then base is always zero and limit is the size for the range.
          //
          systemMemoryMap->Element[ScratchCounter].BaseAddress = 0;
          systemMemoryMap->Element[ScratchCounter].ElementSize = mem->socket[socket].SAD[i].Limit; // Get size of current element
        }
        DEBUG((EFI_D_ERROR, "Node:%d BaseAddress:%08x ElementSize:%08x\n",
              systemMemoryMap->Element[ScratchCounter].NodeId,
              systemMemoryMap->Element[ScratchCounter].BaseAddress,
              systemMemoryMap->Element[ScratchCounter].ElementSize));

        ScratchCounter++;    // Adding a valid entry
      }
    } //end for SAD_RULE loop
    //
    // Copy SAD information to HOB, NOTE: In Purley the SAD & SADintList is in socket loop of the Hob.
    //
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].SADintList), (VOID *)(&(host->var.mem.socket[socket].SADintList)), sizeof(host->var.mem.socket[socket].SADintList));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].SAD), (VOID *)(&(host->var.mem.socket[socket].SAD)), sizeof(host->var.mem.socket[socket].SAD));

    systemMemoryMap->numberEntries = ScratchCounter;
  }

  systemMemoryMap->lowMemBase   = host->var.mem.lowMemBase;
  systemMemoryMap->lowMemSize   = host->var.mem.lowMemSize;
  systemMemoryMap->highMemBase  = host->var.mem.highMemBase;
  systemMemoryMap->highMemSize  = host->var.mem.highMemSize;
  systemMemoryMap->memSize      = host->var.mem.memSize;

  //systemMemoryMap->AepDimmPresent = SKX TODO: if we need to support AEP O*L;

  for(mc = 0; mc < MAX_IMC; mc++) {
    systemMemoryMap->Socket[socket].imcEnabled[mc] = host->var.mem.socket[socket].imcEnabled[mc];
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].imc[mc].TAD), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TAD)),
                            sizeof(host->var.mem.socket[socket].imc[mc].TAD));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].imc[mc].TADChnIndex), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TADChnIndex)),
                            sizeof(host->var.mem.socket[socket].imc[mc].TADChnIndex));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].imc[mc].TADOffset), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TADOffset)),
                            sizeof(host->var.mem.socket[socket].imc[mc].TADOffset));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].imc[mc].TADintList), (VOID *)(&(host->var.mem.socket[socket].imc[mc].TADintList)),
                            sizeof(host->var.mem.socket[socket].imc[mc].TADintList));
  }
  for (ch = 0; ch < MAX_CH; ch++) {
    systemMemoryMap->Socket[socket].ChannelInfo[ch].Enabled  = host->nvram.mem.socket[socket].channelList[ch].enabled;
    systemMemoryMap->Socket[socket].ChannelInfo[ch].Features = host->nvram.mem.socket[socket].channelList[ch].features;
    systemMemoryMap->Socket[socket].ChannelInfo[ch].MaxDimm  = host->nvram.mem.socket[socket].channelList[ch].maxDimm;
    systemMemoryMap->Socket[socket].ChannelInfo[ch].NumRanks = host->nvram.mem.socket[socket].channelList[ch].numRanks;
    systemMemoryMap->Socket[socket].ChannelInfo[ch].chFailed = host->nvram.mem.socket[socket].channelList[ch].chFailed;
    systemMemoryMap->Socket[socket].ChannelInfo[ch].ngnChFailed = host->nvram.mem.socket[socket].channelList[ch].ngnChFailed;
    for(rank = 0; rank < MAX_SPARE_RANK ; rank++){
      systemMemoryMap->Socket[socket].ChannelInfo[ch].SpareLogicalRank[rank] = host->nvram.mem.socket[socket].channelList[ch].spareLogicalRank[rank];
      systemMemoryMap->Socket[socket].ChannelInfo[ch].SpareRankSize[rank] = host->nvram.mem.socket[socket].channelList[ch].spareRankSize[rank];
    }
    systemMemoryMap->Socket[socket].ChannelInfo[ch].EnabledLogicalRanks = 0;
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].Present           = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].dimmPresent;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].aepDimmPresent    = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].aepDimmPresent;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].VendorID          = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].VendorID;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].DeviceID          = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].DeviceID;
      for (i = 0; i < MAX_MANUFACTURER_STRLEN; i++) 
        systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].manufacturer[i] = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].manufacturer[i];
      for (i = 0; i < MAX_SERIALNUMBER_STRLEN; i++) 
        systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].serialNumber[i] = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].serialNumber[i];
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].NumRanks          = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].numRanks;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].X4Present         = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].x4Present;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDMMfgId         = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDMMfgId;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].DimmSize          = host->var.mem.socket[socket].channelList[ch].dimmList[dimm].memSize;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDSDRAMBanks     = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDSDRAMBanks;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].actSPDModuleOrg   = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].actSPDModuleOrg;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].keyByte           = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].keyByte;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].actKeyByte2       = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].actKeyByte2;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].actSPDSDRAMBanks  = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].actSPDSDRAMBanks;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].SPDMemBusWidth    = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].SPDMemBusWidth;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].newDimm           = host->var.mem.socket[socket].channelList[ch].dimmList[dimm].newDimm;
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].nonVolCap         = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].nonVolCap; 
      systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].dimmTs            = host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].dimmTs;
      for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled == 0) continue;
        if (host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].mapOut[rank]) continue;
        systemMemoryMap->Socket[socket].ChannelInfo[ch].EnabledLogicalRanks |=
          1 <<  host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].logicalRank;

        systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].rankList[rank].enabled =
          host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].enabled;

        systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].rankList[rank].logicalRank =
          host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].logicalRank;

        systemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].rankList[rank].rankSize =
          host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].rankList[rank].rankSize;
      } // rank loop
    } // dimm loop

    //
    // Copy RIR information
    //
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].ChannelInfo[ch].rirIntList),
                            (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirIntList)), sizeof(host->var.mem.socket[socket].channelList[ch].rirIntList));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].ChannelInfo[ch].rirLimit),
                            (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirLimit)), sizeof(host->var.mem.socket[socket].channelList[ch].rirLimit));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].ChannelInfo[ch].rirWay),
                            (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirWay)), sizeof(host->var.mem.socket[socket].channelList[ch].rirWay));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].ChannelInfo[ch].rirValid),
                            (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirValid)), sizeof(host->var.mem.socket[socket].channelList[ch].rirValid));
    CopyMem((VOID *)(&systemMemoryMap->Socket[socket].ChannelInfo[ch].rirOffset),
                            (VOID *)(&(host->var.mem.socket[socket].channelList[ch].rirOffset)), sizeof(host->var.mem.socket[socket].channelList[ch].rirOffset));
  } // ch loop

  return;
}

/**

    This function prepares Quiesce data buffer and commit the CSR through Quiesce per request

    @param Action  - Action to take. Prepare Quiesce Data buffer, abort/relase the buffer, or commit the CSR program

    @retval Status - EFI_STATUS

**/
EFI_STATUS 
CallQuiesceSupport (
    UINT8                        Action
)
{
  EFI_STATUS Status = EFI_ABORTED;

  switch (Action) {
  case PREPARE_QUIESCE:
    if (!mCurrQuiescePtr) {
      mMemQuiesceEnabled = TRUE;
      if ((Status  = mQuiesceSupport->AquireDataBuffer((EFI_PHYSICAL_ADDRESS *)(UINTN)&mCurrQuiescePtr)) == EFI_SUCCESS) {
        if (!mStartQuiescePtr) {
          mStartQuiescePtr = mCurrQuiescePtr;
        }

        m2QuiescePtr = (UINT64 *)mCurrQuiescePtr;
      } 
    }
    break;

  case ABORT_QUIESCE:
    if (mStartQuiescePtr) {
      mMemQuiesceEnabled = FALSE;    
      Status  = mQuiesceSupport->ReleaseDataBuffer((EFI_PHYSICAL_ADDRESS)(UINTN)mStartQuiescePtr);
      mStartQuiescePtr = NULL;
      mCurrQuiescePtr  = NULL;
    }
    break;

  case EXECUTE_QUIESCE:
    if (mCurrQuiescePtr) {
      mMemQuiesceEnabled = FALSE;    
      Status = mQuiesceSupport->ReleaseDataBuffer((EFI_PHYSICAL_ADDRESS)(UINTN)mCurrQuiescePtr);
      if (!EFI_ERROR(Status)) {
        Status = mQuiesceSupport->QuiesceMain();   
      }
      mCurrQuiescePtr  = NULL;
      mStartQuiescePtr = NULL;
    }
    break;

  default:
    break;
  } 

  return Status;
}

/**

    Get hanodeid from socket ID and MC

    @param Skt   - Socket ID
    @param HaNum - HA number

    @retval haNodeId - HA Node id

**/
UINT8
SktHaToHaNodeId
(
  UINT8 Skt,
  UINT8 HaNum
)
{
  UINT8 haNodeId;

  haNodeId = ((Skt & 0x4) << 1) + (Skt & 0x3) + (4 * HaNum);

  return haNodeId;
}

/**

    Initializes all the global data structures/variables
    required for MemRas routines

  @param None

  @retval EFI_SUCCESS - if no errors
  @retval non-zero error code on any errors

**/
EFI_STATUS
EFIAPI
InitializeMemRasData(
 )
{
  EFI_STATUS status = EFI_SUCCESS;

  return status;
}

/**

    Saves the DdrCh/Dimm/Rank information

  @param NodeId    - Memory controller ID
  @param Ch        - Channel ID
  @param Rank      - Rank ID

  @retval None
**/
static
void
SaveErrInfo(
    IN UINT8                          NodeId,
    IN UINT8                          DdrCh,
    IN UINT8                          Rank
 )
{
  UINT8                                       failDevId;
  UINT8                                       tgtDdrCh;
  UINT8                                       dimmSlot;
  UINT8                                       socket;
  RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_STRUCT   RetryRdErrLogAddr1;


  if(NodeId >= MC_MAX_NODE) {
    ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
  }

  //
  // Get the error socket
  //
  socket = NODE_TO_SKT(NodeId);

  //
  // Convert logical rank to DIMM/Physical rank
  //
  dimmSlot = Rank / MAX_RANK_DIMM;
  mLastErrInfo[NodeId].Dimm = dimmSlot;
  mLastErrInfo[NodeId].Rank = Rank % MAX_RANK_DIMM;


  //
  // If lock-step Mode is enabled, determine target
  // DDR CH ID from fail device ID and DIMM type
  //
  tgtDdrCh = DdrCh;
  if(mMemRas->SystemMemInfo->RasModesEnabled & STAT_VIRT_LOCKSTEP){
    //
    // Get Fail device ID
    //
    RetryRdErrLogAddr1.Data     = mCpuCsrAccess->ReadCpuCsr(socket, NODECH_TO_SKTCH(NodeId, DdrCh), RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_REG);
    failDevId                   = (UINT8)RetryRdErrLogAddr1.Bits.failed_dev;

    //
    // Device to DDRCH mapping
    // x4 -> DEV0-17 DDRCH0; DEV18-35 DDRCH1;
    // x8 -> DEV0-8  DDRCH0; DEV09-17 DDRCH1;
    //
    if(socket < MAX_SOCKET){
      if(mMemRas->SystemMemInfo->Socket[socket].ChannelInfo[NODECH_TO_SKTCH(NodeId, DdrCh)].DimmInfo[dimmSlot].X4Present){
        //
        // X4 DIMM
        //
        if(failDevId > MAX_X4_DIMM_DEVID_CH0){
          tgtDdrCh++;
        }
      } else {
        //
        // X8 DIMM
        //
        if(failDevId > MAX_X8_DIMM_DEVID_CH0){
          tgtDdrCh++;
        }
      } // if x4
    } else {
      RAS_DEBUG((DEBUG_ERROR, "Tryed to use Socket %u and MAX_SOCKET = %u", socket, MAX_SOCKET));
      return ;
    }// if socket
  } // if LOCKSTEP_EN

  mLastErrInfo[NodeId].DdrCh = tgtDdrCh;
}

/**

    Return DDRCH/DIMM/Rank that had the most recent error

  @param NodeId    - Memory controller ID
  @param *DdrCh    - ptr to buffer to hold DdrCh
  @param *Dimm     - ptr to buffer to hold Dimm
  @param *Rank     - ptr to buffer to hold Rank

  @retval DdrCh
  @retval Dimm
  @retval Rank
**/
void
EFIAPI
GetLastErrInfo(
    IN UINT8                          NodeId,
    IN UINT8                          *DdrCh,
    IN UINT8                          *Dimm,
    IN UINT8                          *Rank
 )
{
  *DdrCh  = mLastErrInfo[NodeId].DdrCh;
  *Dimm   = mLastErrInfo[NodeId].Dimm;
  *Rank   = mLastErrInfo[NodeId].Rank;
}


/**

    Checks if the specified rank has error overflow
    If the CPU error counter overflows clear overflow
    status in CPU; counters are not cleared

    Note: In lock-step mode, CPU logs all rank ECC errors in
    registers of Primary channel only (i.e., DDR CH0 or DDR CH2)

  @param NodeId    - Memory controller ID
  @param Ch        - Channel ID
  @param Rank      - Rank ID

  @retval TRUE      - if the specified rank has error overflow
  @retval FALSE     - specified rank has no error overflow

**/
BOOLEAN
IsErrorExcdForThisRank(
    IN UINT8 Node,
    IN UINT8 NodeCh,
    IN UINT8 Rank
 )
{
  BOOLEAN                         cpuErrOverflow = FALSE;
  BOOLEAN                         isErrorExcd    = FALSE;
  CORRERRORSTATUS_MCDDC_DP_STRUCT corrErrStsReg;
  UBOXERRSTS_UBOX_CFG_STRUCT      uboxErrorStatusReg;
  UINT8                           Socket = NODE_TO_SKT(Node);
  UINT8                           SktCh = NODECH_TO_SKTCH(Node, NodeCh);

  //
  // Check error status in CPU for this rank
  //
  corrErrStsReg.Data = mCpuCsrAccess->ReadCpuCsr( Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG );
  if( corrErrStsReg.Bits.err_overflow_stat & (1 << Rank)){
    cpuErrOverflow = TRUE;
  }

  if( cpuErrOverflow ){
    DEBUG((DEBUG_INFO,"Error counter overflow : Rank%d\n",Rank));
    isErrorExcd = TRUE;

    //
    // Clear error status in CPU for this rank
    //
    ClearErrorsForThisRank (Node, NodeCh, Rank);

    //
    // Clear CORRERRORSTATUS, write 1 to clear
    //
    corrErrStsReg.Data = 0;
    corrErrStsReg.Bits.err_overflow_stat = 1 << Rank; // clear only for the rank that is handled
    mCpuCsrAccess->WriteCpuCsr(  Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG, corrErrStsReg.Data );

    if(mCpuCsrAccess->ReadCpuCsr( Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG ) & (1 <<Rank)){
      RAS_DEBUG((4, "ch err_overflow:%lx    rank_err_overflow:%x ",
        mCpuCsrAccess->ReadCpuCsr( Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG ),
        mCpuCsrAccess->ReadCpuCsr( Socket, SktCh, (UINT32)(CORRERRCNT_0_MCDDC_DP_REG) + (Rank/2) * 4 )
        ));

      corrErrStsReg.Data = 0;
      corrErrStsReg.Bits.err_overflow_stat = 1 << Rank; // clear only for the rank that is handled
      mCpuCsrAccess->WriteCpuCsr(  Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG, corrErrStsReg.Data );
    }

    //
    // Clear SMI status
    //
    uboxErrorStatusReg.Data = mCpuCsrAccess->ReadCpuCsr( Socket, 0, UBOXERRSTS_UBOX_CFG_REG );
    uboxErrorStatusReg.Bits.smi_delivery_valid  = 0;
    uboxErrorStatusReg.Bits.smisrcimc           = 0;
    mCpuCsrAccess->WriteCpuCsr( Socket, 0, UBOXERRSTS_UBOX_CFG_REG, uboxErrorStatusReg.Data );

  }


  //
  // Save error Info
  //
  if( isErrorExcd ){
    //
    // If in lock-step mode, then the use the fail device ID & device
    // width to determine the correct DDR CH the failed rank falls
    //
    SaveErrInfo(Node, NodeCh, Rank);
  }

  return isErrorExcd;
}

/**

    Clears error counters and over flow status for specified
    rank in CPU

  @param NodeId    - Memory controller ID
  @param Ch        - Channel ID
  @param Rank      - Rank ID

  @retval None
**/
void
ClearErrorsForThisRank(
    IN UINT8 NodeId,
    IN UINT8 DdrCh,
    IN UINT8 Rank
 )
{
  CORRERRCNT_0_MCDDC_DP_STRUCT  corrErrCntReg;
  UINT32                        corrErrCntRegBaseAddr = CORRERRCNT_0_MCDDC_DP_REG;
  UINT8                         Socket = NODE_TO_SKT(NodeId);
  UINT8                         SktCh = NODECH_TO_SKTCH(NodeId, DdrCh);
  //
  // Clear error counters and status in CPU for this rank
  //
  corrErrCntReg.Data = mCpuCsrAccess->ReadCpuCsr(  Socket, SktCh, corrErrCntRegBaseAddr + (Rank/2) * 4 );
  if( (Rank % 2) == 0) {
    corrErrCntReg.Bits.overflow_0     = 1;
    corrErrCntReg.Bits.overflow_1     = 0;
    corrErrCntReg.Bits.cor_err_cnt_0  = 0;
  } else {
    corrErrCntReg.Bits.overflow_1     = 1;
    corrErrCntReg.Bits.overflow_0     = 0;
    corrErrCntReg.Bits.cor_err_cnt_1  = 0;
  }
  mCpuCsrAccess->WriteCpuCsr(  Socket, SktCh, corrErrCntRegBaseAddr + (Rank/2) * 4, corrErrCntReg.Data );

}

/**
  Checks the error status registers and returns a bitmap of
  Memory NodeIDs that have errors. A copy of the bitmap
  is also stored for internal use

  @param *ErrorBitmap      - ptr to buffer to return 32-bit bitmap of Nodes with error

  @retval *ErrorBitmap      -  32 bit bitmap of Nodes with error. If a bit is set, then
                       @retval that Memory controller has an error

**/
void
EFIAPI
GetBitmapOfNodeWithErrors(
    IN OUT UINT32                     * ErrorBitmap
  )
{
  CORRERRORSTATUS_MCDDC_DP_STRUCT corrErrorStatusReg;
  UBOXERRSTS_UBOX_CFG_STRUCT      uboxErrorStatusReg;
  UINT32                          data;
  UINT8                           socket;
  UINT8                           imc;
  UINT8                           node;
  UINT8                           sktch;

  RASDEBUG ((DEBUG_INFO, "GetBitmapOfNodeWithErrors \n"));
  *ErrorBitmap = 0;
  uboxErrorStatusReg.Data = 0;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((mMemRas->SystemMemInfo->Socket[socket].imcEnabled[0] == 0) && (mMemRas->SystemMemInfo->Socket[socket].imcEnabled[1] == 0)) continue; // Socket[] doesnt have a Enabled - This is temp W/A to unblock Simics Purley 4.8.pre81 
    uboxErrorStatusReg.Data = mCpuCsrAccess->ReadCpuCsr (socket, 0, UBOXERRSTS_UBOX_CFG_REG);
    if (uboxErrorStatusReg.Bits.smisrcimc) {
      RASDEBUG((DEBUG_INFO,"   GetBitmapOfNodeWithErrors: socket=%d uboxErrorStatus=%x\n", socket, uboxErrorStatusReg.Data));
      for (imc=0; imc<MAX_IMC; imc++) {
        if (mMemRas->SystemMemInfo->Socket[socket].imcEnabled[imc] == 0) continue;

        node = SKTMC_TO_NODE(socket, imc);
        for (sktch = 0; sktch < MAX_CH; sktch++) {
          corrErrorStatusReg.Data = mCpuCsrAccess->ReadCpuCsr (socket, sktch, CORRERRORSTATUS_MCDDC_DP_REG);
          if (corrErrorStatusReg.Bits.err_overflow_stat) {
            *ErrorBitmap |= (1 << node);
            RASDEBUG ((DEBUG_INFO, "GetBitmapOfNodeWithErrors:ErrorBitmap:%d \n",ErrorBitmap));
            //
            // Set pending event only if there is no other event already pending
            // RAS code is non re-entant. We can handle only 1 event at a time
            //
            GetOrSetErrorHandlingStatus (node, GetEventPendingStatus, &data);
            if (!data) {
              data = 1;
              GetOrSetErrorHandlingStatus (node, UpdateEventPendingStatus, &data); // Save the error bitmap for internal use
            }
            break;    // Move to next Node
          } // if (corrErrorStatusReg)
        } // for (sktch)
      } // for (imc)
    } // if (uboxErrorStatusReg)
  } // for (socket)

  return ;
}

/**

  Returns a bitmap of  Memory NodeIDs with event handling already
  in progress

  @param *EventProgressBitmap  - ptr to buffer to return 32-bit bitmap of Nodes with events in progress

  @retval *EventProgressBitmap  - 32 bit bitmap of Nodes with events in progress. If a bit is set, then
                          @retval that Memory controller has event handling in progress

**/
void
EFIAPI
GetBitMapOfNodeWithEventInProgress(
    IN OUT UINT32                     * EventProgressBitmap
  )
{
  *EventProgressBitmap = mEventProgressBitmap;
}

/**

  Updates / Gets error handling status for specified NodeID based on
  Opcode. Function will soft-hang if it encounters any error

  @param NodeId            - Memory Node Id
  @param Opcode            - indicates the operation to be performed on
                             error/event status variables (mEventProgressBitmap, mEventPendingBitmap)
  @param *Value            - caller passes the value in this buffer to update the status

  @retval *Value            -  value of event status when Opcode = GetEventProgressStatus or GetEventPendingStatus

**/
void
EFIAPI
GetOrSetErrorHandlingStatus (
  IN UINT8                           NodeId,
  IN ERR_STATUS_OPCODE               Opcode,
  IN OUT UINT32                      *Value
 )
{
  switch( Opcode ){
  case UpdateEventProgressStatus:
    switch( *Value ){
    case 0:
      ASSERT( (mEventProgressBitmap & (1 << NodeId)) != 0 );        // Make sure we have an event in progress
      mEventProgressBitmap &= ~(1 << NodeId);
      RAS_DEBUG((2, "GetOrSetErrorHandlingStatus - Clear Event Progress status for Node%d\n", NodeId));
      break;

    case 1:
      //
      // We can move an event to in-progress only if there is a
      // pending error already for this node. Assert it
      //
      ASSERT( (mEventPendingBitmap & (1 << NodeId)) != 0 );
      mEventProgressBitmap |= (1 << NodeId);
      RAS_DEBUG((2, "GetOrSetErrorHandlingStatus - Set Event Progress status for Node%d\n", NodeId));
      break;

    default:
      ASSERT_EFI_ERROR(EFI_UNSUPPORTED);
      break;
    }
    break;

  case UpdateEventPendingStatus:
    switch( *Value ){
    case 0:
      ASSERT( (mEventProgressBitmap & (1 << NodeId)) == 0 );        // Make sure we don't have an event in progress
      mEventPendingBitmap &= ~(1 << NodeId);
      RAS_DEBUG((2, "GetOrSetErrorHandlingStatus - Clear Event Pending status for Node%d\n", NodeId));
      break;

    case 1:
      //
      // Make sure there are no events that are already
      // pending i.e., yet to be handled.  RAS code is not
      // designed to handle new events when there are pending
      // events
      //
      ASSERT( (mEventPendingBitmap & (1 << NodeId)) == 0 );
      mEventPendingBitmap |= (1 << NodeId);
      RAS_DEBUG((2, "GetOrSetErrorHandlingStatus - Set Event Pending status for Node%d\n", NodeId));
      break;

    default:
      ASSERT_EFI_ERROR(EFI_UNSUPPORTED);
      break;
    }
    break;

  case GetEventProgressStatus:
    *Value = (mEventProgressBitmap >> NodeId) & 1;
    RAS_DEBUG((2, "GetOrSetErrorHandlingStatus - Get Event Progress status for Node%d = %d\n", NodeId, *Value));
    break;

  case GetEventPendingStatus:
    *Value = (mEventPendingBitmap >> NodeId) & 1;
    RAS_DEBUG((2, "GetOrSetErrorHandlingStatus - Get Event Pending status for Node%d = %d\n", NodeId, *Value));
    break;

  default:
    ASSERT_EFI_ERROR(EFI_UNSUPPORTED);
    break;
  } // switch( opcode )
}

/**

   Open PAM region for DRAM RW access

  @param SocketId - Socket ID for which PAM has to be opened

  @retval EFI_SUCCESS / Error code

  If this function encounters an error, it will soft-hang
**/
EFI_STATUS
EFIAPI
OpenPamRegion(
    IN UINT8 SocketId
 )
{
  PAM0123_CHABC_SAD_STRUCT		pam0123Reg;
  PAM456_CHABC_SAD_STRUCT      pam456Reg;
  EFI_STATUS                    status = EFI_SUCCESS;

  if(!IsPamOpen){
    pam0123Reg.Data   = 0;
    pam456Reg.Data    = 0;

    //
    // Save current register values
    //
    OldPam0123RegVal  = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, PAM0123_CHABC_SAD_REG);
    OldPam456RegVal   = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, PAM456_CHABC_SAD_REG);

    //
    // Open-up PAM regions
    //
    pam0123Reg.Bits.pam0_hienable = PAM_DRAM_RW;  // 0F0000-0FFFFF
    pam0123Reg.Bits.pam1_hienable = PAM_DRAM_RW;  // 0C4000-0C7FFF
    pam0123Reg.Bits.pam1_loenable = PAM_DRAM_RW;  // 0C0000-0C3FFF
    pam0123Reg.Bits.pam2_hienable = PAM_DRAM_RW;  // 0CC000-0CFFFF
    pam0123Reg.Bits.pam2_loenable = PAM_DRAM_RW;  // 0C8000-0CBFFF
    pam0123Reg.Bits.pam3_hienable = PAM_DRAM_RW;  // 0D4000-0D7FFF
    pam0123Reg.Bits.pam3_loenable = PAM_DRAM_RW;  // 0D0000-0D3FFF

    pam456Reg.Bits.pam4_hienable  = PAM_DRAM_RW;  // 0DC000-0DFFFF
    pam456Reg.Bits.pam4_loenable  = PAM_DRAM_RW;  // 0D8000-0DBFFF
    pam456Reg.Bits.pam5_hienable  = PAM_DRAM_RW;  // 0E4000-0E7FFF
    pam456Reg.Bits.pam5_loenable  = PAM_DRAM_RW;  // 0E0000-0E3FFF
    pam456Reg.Bits.pam6_hienable  = PAM_DRAM_RW;  // 0EC000-0EFFFF
    pam456Reg.Bits.pam6_loenable  = PAM_DRAM_RW;  // 0E8000-0EBFFF

    mCpuCsrAccess->WriteCpuCsr(SocketId, 0, PAM0123_CHABC_SAD_REG, pam0123Reg.Data);
    mCpuCsrAccess->WriteCpuCsr(SocketId, 0, PAM456_CHABC_SAD_REG, pam456Reg.Data);

    IsPamOpen = TRUE;

  }
  return status;
}

/**

   Restore PAM region with previously saved settings

  @param SocketId - Socket ID for which PAM has to be opened

  @retval EFI_SUCCESS / Error code

  If this function encounters an error, it will soft-hang
**/
EFI_STATUS
EFIAPI
RestorePamRegion(
    IN UINT8 SocketId
 )
{
  EFI_STATUS                  status = EFI_SUCCESS;

  if(IsPamOpen){
    //
    // Restore previous register values
    //
    mCpuCsrAccess->WriteCpuCsr(SocketId, 0, PAM0123_CHABC_SAD_REG, OldPam0123RegVal);
    mCpuCsrAccess->WriteCpuCsr(SocketId, 0, PAM456_CHABC_SAD_REG, OldPam456RegVal);

    IsPamOpen = FALSE;

  }
  return status;
}



/**

   Check if current memory node is enabled

    @param Memory Node ID
    @param ptr to memory node enabled flag

Modifies:
    Memory node enabled flag

  @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
EFIAPI
IsMemNodeEnabled(
    IN UINT8 Node,
    OUT BOOLEAN *IsMemNodeEnabledFlag
)
{
  UINT8  State;

  GetCurrentMemoryNodeState (Node, &State);
  *IsMemNodeEnabledFlag = State;

  return EFI_SUCCESS;
}

/**

   Enable RAS error signaling to BIOS

    @param Memory Node ID

Modifies:
    None

  @retval EFI_SUCCESS / Error code


**/
EFI_STATUS
EFIAPI
EnableErrorSignallingtoBIOS(
    IN UINT8 Node
)
{
  return EFI_SUCCESS;
}

VOID
DispalyFailedErrorInfo (
  IN PFAILED_DIMM_INFO pFailedDimmInfo
  )
{
  if(pFailedDimmInfo->ValidBits == 0) return;
  DEBUG((DEBUG_ERROR, "\t\t Socket : %x\n", pFailedDimmInfo->Socket));
  DEBUG((DEBUG_ERROR, "\t\t SktCh : %x\n", pFailedDimmInfo->SktCh));
  DEBUG((DEBUG_ERROR, "\t\t RegionType (0 - ind reg, 1- adddc reg , 2-svls reg, 3- rank in LS mode) : %x\n", pFailedDimmInfo->RegionType));
  DEBUG((DEBUG_ERROR, "\t\t RegionSubType (01 - Failed region, 02- non failed) : %x\n", pFailedDimmInfo->RegionSubType));
  DEBUG((DEBUG_ERROR, "\t\t ddr3_width : %x\n", pFailedDimmInfo->ddr3_width));
  DEBUG((DEBUG_ERROR, "\t\t FailedRank : %x\n", pFailedDimmInfo->FailedRank));
  DEBUG((DEBUG_ERROR, "\t\t FailedSubRank : %x\n", pFailedDimmInfo->FailedSubRank));
  DEBUG((DEBUG_ERROR, "\t\t FailedDev : %x\n", pFailedDimmInfo->FailedDev));
  DEBUG((DEBUG_ERROR, "\t\t FailedBank : %x\n", pFailedDimmInfo->FailedBank));
  DEBUG((DEBUG_ERROR, "\t\t FailedBG : %x\n", pFailedDimmInfo->FailedBG));
  DEBUG((DEBUG_ERROR, "\t\t FailedDimm : %x\n", pFailedDimmInfo->FailedDimm));
  DEBUG((DEBUG_ERROR, "\t\t BuddyRank : %x\n", pFailedDimmInfo->BuddyRank));
  DEBUG((DEBUG_ERROR, "\t\t BuddySubRank : %x\n", pFailedDimmInfo->BuddySubRank));
  DEBUG((DEBUG_ERROR, "\t\t BuddyBank : %x\n", pFailedDimmInfo->BuddyBank));
  DEBUG((DEBUG_ERROR, "\t\t BuddyBG : %x\n", pFailedDimmInfo->BuddyBG));
}

/**

This function provides the failed dimm errro info for dram ecc corrected errors

@param NodeId          - Memory Node ID
@param NodeCh          - Memory Node ch
@param RankValid,      - // Bit[0-1]  = 00 - use retry register to get failed rank.
                                        01 - Use rank over flow sts register to get the failed dimm
                                        02 - Use correrrorstatus to get failed dimm.
                                        03 - future use.
                         // Bit [2]  = 01 -  clear per rank error counters and error overflow status.
                                     = 00 -  don't clear rank error counters
                                     = 00 -  don't clear the retry read error log register
@param Rank            - Rank parameter is the failed rank whose error threshold exceeded.

Output : If error is valid, updates the failed dimm details ( skt,sktch, failed dimm, rank,subrank,dev,bank and bank group)I
          has error.
         If rank is valid , updates failed dimm details of(skt,sktch, rank, failed dimm ). Rank valid is ignored.

@retval EFI_SUCCESS / EFI_INVALID

*/
EFI_STATUS
EFIAPI
GetFailedDimmErrorInfo (
  IN UINT8 Node,
  IN UINT8 NodeCh,
  IN UINT8 ValidBits,
  IN UINT8 Rank,
  OUT PFAILED_DIMM_INFO pFailedDimmInfo
){

  UINT8        Socket;
  UINT8        SktCh ;
  UINT8        AddcRegNum;
  UINT8        Bank;
  BOOLEAN      SpareDeviceFailed;
  BOOLEAN      ADDDCRegionValid;
  UINT8        LocalRank;

  DIMMMTR_0_MC_MAIN_STRUCT                  dimmMtr;
  ADDDC_REGION0_MCDDC_DP_STRUCT             AdddcRegion;
  CORRERRORSTATUS_MCDDC_DP_STRUCT           corrErrStsReg;
  SPARING_CONTROL_MC_MAIN_STRUCT            spareCtlReg;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT      AdddcCtrRegion0;
  RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_STRUCT RetryRdErrLogAddr1;
  RETRY_RD_ERR_LOG_ADDRESS2_MCDDC_DP_STRUCT RetryRdErrLogAddr2;
  RETRY_RD_ERR_LOG_MCDDC_DP_STRUCT          RetryRdErrLog;

  //Initializations
  SpareDeviceFailed = FALSE;
  Socket = NODE_TO_SKT(Node);
  SktCh = NODECH_TO_SKTCH(Node, NodeCh);
  ADDDCRegionValid = TRUE;
  LocalRank = Rank;
  pFailedDimmInfo->RegionSubType = ErrInIndReg;
  RetryRdErrLog.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, RETRY_RD_ERR_LOG_MCDDC_DP_REG);
  RetryRdErrLogAddr1.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_REG);
  RetryRdErrLogAddr2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, RETRY_RD_ERR_LOG_ADDRESS2_MCDDC_DP_REG);

  DEBUG((DEBUG_INFO, "GetFailedDimmErrorInfo Entry : i/p n:%x, ch:%x, v: r :%x, %x\n", Node, NodeCh, ValidBits, Rank));
  ZeroMem (pFailedDimmInfo, sizeof(FAILED_DIMM_INFO));

  if ( ValidBits & BIT0)  {
    LocalRank = (UINT8)RetryRdErrLogAddr1.Bits.chip_select;
  }

  if (ValidBits & BIT1) {
    corrErrStsReg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG);
    pFailedDimmInfo->ValidBits = 01;
    pFailedDimmInfo->Socket = Socket;
    pFailedDimmInfo->SktCh = SktCh;
    if (corrErrStsReg.Bits.dimm_alert & BIT0) {
      pFailedDimmInfo->FailedDimm = 0;
    } else if (corrErrStsReg.Bits.dimm_alert & BIT1) {
      pFailedDimmInfo->FailedDimm = 1;
    } else {
      pFailedDimmInfo->FailedDimm = 0xFF;
    }
    return EFI_SUCCESS;
  }

  if( ValidBits & BIT2) {
    // clear per rank error counters and clear CORRERRORSTATUS, write 1 to clear
    ClearErrorsForThisRank( Node, SktCh, LocalRank );
    corrErrStsReg.Data = 0;
    corrErrStsReg.Bits.err_overflow_stat = 1 << LocalRank; // clear only for the rank that is handled
    mCpuCsrAccess->WriteCpuCsr(  Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG, corrErrStsReg.Data );
  }

  if( ValidBits & BIT3) {
    // clear retry read error log register
    mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, RETRY_RD_ERR_LOG_MCDDC_DP_REG, 00);
  }

  if(RetryRdErrLog.Bits.v == 0){
    DEBUG((DEBUG_INFO, "GetFailedDimmErrorInfo error Exit  :\n"));
    return EFI_NOT_FOUND;
  }

  if ((RetryRdErrLog.Bits.v == 1) && (RetryRdErrLog.Bits.uc == 0) && (RetryRdErrLog.Bits.corr_correctable)) {
    RetryRdErrLogAddr1.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_REG);

    pFailedDimmInfo->ValidBits     = 01; // Error valid
    pFailedDimmInfo->Socket        = Socket;
    pFailedDimmInfo->SktCh         = SktCh;
    pFailedDimmInfo->FailedDimm    = ((UINT8)RetryRdErrLogAddr1.Bits.chip_select & BIT2) >> 2;
    pFailedDimmInfo->FailedCol     = RetryRdErrLogAddr1.Bits.col;
    pFailedDimmInfo->FailedRow     = RetryRdErrLogAddr2.Bits.row;
    pFailedDimmInfo->FailedDev     = (UINT8)RetryRdErrLogAddr1.Bits.failed_dev;
    pFailedDimmInfo->FailedRank    = (UINT8)RetryRdErrLogAddr1.Bits.chip_select;
    pFailedDimmInfo->FailedSubRank = (UINT8)RetryRdErrLogAddr1.Bits.cbit;
    pFailedDimmInfo->FailedBank    = (UINT8)(RetryRdErrLogAddr1.Bits.bank & 0x0c) >> 02;
    pFailedDimmInfo->FailedBG      = (UINT8)RetryRdErrLogAddr1.Bits.bank & 03;

    /*  Look up the actual rank/bank/device for the failure if
     *  retry_rd.mode indicates ADDDC or VLS (values 4, 5, 6, 7 for ADDDC & 9 ,10 for SVL).*/
    if ((RetryRdErrLog.Bits.mode >= 04) && (RetryRdErrLog.Bits.mode <= 10) && (RetryRdErrLog.Bits.mode != 0x08)) {

      for (AddcRegNum = 0; AddcRegNum < mMaxRegion; AddcRegNum++) {
        AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, (ADDDC_REGION0_CONTROL_MC_MAIN_REG + AddcRegNum * 4));
        AdddcRegion.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, ADDDC_REGION0_MCDDC_DP_REG+ AddcRegNum);
        if (AdddcCtrRegion0.Bits.region_enable == 1){
          RASDEBUG ((DEBUG_INFO, "adddc region %d active \n",AddcRegNum));
          if((LocalRank != AdddcCtrRegion0.Bits.failed_cs) && (LocalRank != AdddcCtrRegion0.Bits.nonfailed_cs))
            continue;

          if ((mHost.var.common.stepping == H0_REV_SKX) || (mHost.var.common.stepping == M0_REV_SKX) || (mHost.var.common.stepping == U0_REV_SKX)){
            RASDEBUG ((DEBUG_ERROR, "Applying retry read work-around for H0 and after \n"));
            if ((UINT8)(AdddcCtrRegion0.Bits.nonfailed_cs) == (UINT8)(RetryRdErrLogAddr1.Bits.chip_select)) {
              pFailedDimmInfo->RegionType |= (1 << FailedRankInLSMode);  // Failed rank in lock step mode.
              if (AdddcRegion.Bits.faildevice == RetryRdErrLogAddr1.Bits.failed_dev) {//device already spared
                SpareDeviceFailed = TRUE;
              }else {
                SpareDeviceFailed = FALSE;
              }
              switch (AdddcCtrRegion0.Bits.region_size) {
              case 0:
                Bank = (UINT8)((AdddcCtrRegion0.Bits.nonfailed_ba << 2) | (AdddcCtrRegion0.Bits.nonfailed_bg));
                if ((Bank == RetryRdErrLogAddr1.Bits.bank) && (RetryRdErrLogAddr1.Bits.cbit == AdddcCtrRegion0.Bits.nonfailed_c)) {
                  pFailedDimmInfo->RegionSubType = ErrInFailedBank;
                }
                else {
                  pFailedDimmInfo->RegionSubType = ErrInFailedRank;
                }
                break;
              case 1:
                pFailedDimmInfo->RegionSubType   = ErrInFailedRank;
                break;
              default:
                RASDEBUG((DEBUG_ERROR, "Incorrect adddc region!! \n"));
                break;
              }
              break;  // Adddc region found
            }  else if ((UINT8)AdddcCtrRegion0.Bits.failed_cs == (UINT8)RetryRdErrLogAddr1.Bits.chip_select) {
              pFailedDimmInfo->RegionType |= (1 << FailedRankInLSMode);

              switch (AdddcCtrRegion0.Bits.region_size) {
              case 0:
                Bank = (UINT8)((AdddcCtrRegion0.Bits.failed_ba << 2) | (AdddcCtrRegion0.Bits.failed_bg));
                if ((Bank == RetryRdErrLogAddr1.Bits.bank) && (RetryRdErrLogAddr1.Bits.cbit == AdddcCtrRegion0.Bits.nonfailed_c)) {
                  pFailedDimmInfo->RegionSubType = ErrInNonFailedBank;
                }
                else {
                  pFailedDimmInfo->RegionSubType = ErrInNonFailedRank;
                }
                break;
              case 1:
                pFailedDimmInfo->RegionSubType = ErrInNonFailedRank;
                break;
              default:
                RASDEBUG((DEBUG_ERROR, "Incorrect adddc region!! \n"));
                break;
              }
              break;
            }
          } else { // All steppings other than H0 stepping
            if ((UINT8)(AdddcCtrRegion0.Bits.failed_cs) == (UINT8)(RetryRdErrLogAddr1.Bits.chip_select)) {
              RASDEBUG ((DEBUG_INFO, " Retry-Read lookup below H0 stepping \n"));
              pFailedDimmInfo->RegionType |= (1 << FailedRankInLSMode);  // Failed rank in lock step mode.
              if (AdddcRegion.Bits.faildevice == RetryRdErrLogAddr1.Bits.failed_dev) {//device already spared
                SpareDeviceFailed = TRUE;
              } else {
                SpareDeviceFailed = FALSE;
              }
              switch (AdddcCtrRegion0.Bits.region_size) {
              case 0:
                Bank = (UINT8)((AdddcCtrRegion0.Bits.nonfailed_ba << 2) | (AdddcCtrRegion0.Bits.nonfailed_bg));
                if ((Bank == RetryRdErrLogAddr1.Bits.bank) && (RetryRdErrLogAddr1.Bits.cbit == AdddcCtrRegion0.Bits.nonfailed_c)) {
                  pFailedDimmInfo->RegionSubType = ErrInFailedBank;
                }
                else {
                  pFailedDimmInfo->RegionSubType = ErrInFailedRank;
                }
                break;
              case 1:
                pFailedDimmInfo->RegionSubType = ErrInFailedRank;
                break;
              default:
                RASDEBUG((DEBUG_ERROR, "Incorrect adddc region size! \n"));
                break;
              }
              break;  // Adddc region found
            }
            else if ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs == (UINT8)RetryRdErrLogAddr1.Bits.chip_select) {
              pFailedDimmInfo->RegionType |= (1 << FailedRankInLSMode);

              switch (AdddcCtrRegion0.Bits.region_size) {
              case 0:
                Bank = (UINT8)((AdddcCtrRegion0.Bits.failed_ba << 2) | (AdddcCtrRegion0.Bits.failed_bg));
                if ((Bank == RetryRdErrLogAddr1.Bits.bank) && (RetryRdErrLogAddr1.Bits.cbit == AdddcCtrRegion0.Bits.nonfailed_c)) {
                  pFailedDimmInfo->RegionSubType = ErrInNonFailedBank;
                }
                else {
                  pFailedDimmInfo->RegionSubType = ErrInNonFailedRank;
                }
                break;
              case 1:
                pFailedDimmInfo->RegionSubType = ErrInNonFailedRank;
                break;
              }
              break;
            } // error in failed region
          }
        }
      }
    } // Ecc mode adddc or SVL
    /*Retry_rd look up is not required unless the current failure is in adddc mode.
     *But determine adddc region that failed device falls in to determine the buddy_cs*/
    else {
      for (AddcRegNum = 0; AddcRegNum < mMaxRegion; AddcRegNum++) {
        AdddcCtrRegion0.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, (ADDDC_REGION0_CONTROL_MC_MAIN_REG + AddcRegNum * 4));
        if (AdddcCtrRegion0.Bits.region_enable == 1){
          RASDEBUG ((DEBUG_INFO, "adddc region %d active.\n",AddcRegNum));
          if((LocalRank != AdddcCtrRegion0.Bits.failed_cs) && (LocalRank != AdddcCtrRegion0.Bits.nonfailed_cs) && (LocalRank != RetryRdErrLogAddr1.Bits.chip_select))
            continue;
          if ((UINT8)(AdddcCtrRegion0.Bits.failed_cs) == (UINT8)(RetryRdErrLogAddr1.Bits.chip_select)) {
            pFailedDimmInfo->RegionSubType = ErrInFailedRank;
            break;
          }
          else if ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs == (UINT8)RetryRdErrLogAddr1.Bits.chip_select) {
            pFailedDimmInfo->RegionSubType = ErrInNonFailedRank;
            break;
          }
        }
      }
    } //Ecc mode not adddc or SVL

    spareCtlReg.Data      = mCpuCsrAccess->ReadMcCpuCsr( Socket, NODE_TO_MC(Node), SPARING_CONTROL_MC_MAIN_REG);
    AdddcCtrRegion0.Data  = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, (ADDDC_REGION0_CONTROL_MC_MAIN_REG + AddcRegNum * 4));
    if (spareCtlReg.Bits.virtual_lockstep_en == 0) {
      if ( ((UINT8)(RetryRdErrLogAddr1.Bits.failed_dev) < 18) || SpareDeviceFailed) {
        if (SpareDeviceFailed){
           pFailedDimmInfo->FailedDev   = ADDDC_SPARE_DEVICE;
        }
        switch (pFailedDimmInfo->RegionSubType) {
        case ErrInIndepReg:
          RASDEBUG((DEBUG_INFO, "Error in Independent region \n"));
          break;
        case ErrInFailedRank :
          RASDEBUG((DEBUG_INFO, "Error in Failed Rank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.failed_cs & BIT2) >> 2;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;

          break;
        case ErrInNonFailedRank :
          RASDEBUG((DEBUG_INFO, "Error in Non Failed Rank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs & BIT2) >> 2;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.failed_c;

          break;
        case ErrInFailedBank :
          RASDEBUG((DEBUG_INFO, "Error in Failed Bank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.failed_cs & BIT2) >> 2;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.failed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.failed_bg;

          break;
        case ErrInNonFailedBank :
          RASDEBUG((DEBUG_INFO, "Error in Failed Rank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs & BIT2) >> 2;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.nonfailed_bg;

          break;
        default:
          RASDEBUG((DEBUG_INFO, "Invalid switch case\n"));
          break;
        }// Region subtype switch
      } //Failed Dev < 18
      else if ((UINT8)(RetryRdErrLogAddr1.Bits.failed_dev) > 17) {
        pFailedDimmInfo->FailedDev   = (UINT8)(RetryRdErrLogAddr1.Bits.failed_dev) -18;
        switch (pFailedDimmInfo->RegionSubType) {
        case ErrInIndepReg:
          RASDEBUG((DEBUG_INFO, "Error in Independent region \n"));
          break;
        case ErrInFailedRank :
          RASDEBUG((DEBUG_INFO, "Error in Failed Rank failed dev >17\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs & BIT2) >> 2;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.failed_c;

          break;
        case ErrInNonFailedRank :
          RASDEBUG((DEBUG_INFO, "Error in Non Failed Rank failed dev >17\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.failed_cs & BIT2) >> 2;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;

          break;
        case ErrInFailedBank :
          RASDEBUG((DEBUG_INFO, "Error in Failed Bank failed dev >17\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs & BIT2) >> 2;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.nonfailed_bg;

          break;
        case ErrInNonFailedBank :
          RASDEBUG((DEBUG_INFO, "Error in Non Failed Bank failed dev >17\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.failed_cs & BIT2) >> 2;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.failed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.failed_bg;

          break;
        default:
          RASDEBUG((DEBUG_INFO, "Invalid switch case\n"));
          break;
        }// Region subtype switch
      } //Failed dev >= 18
    }//SVL = 0
    else if (spareCtlReg.Bits.virtual_lockstep_en == 1) {
      pFailedDimmInfo->RegionType |= (1 << ErrInSvlsReg);  // SVLS region.
      if (((UINT8)(RetryRdErrLogAddr1.Bits.failed_dev) & 01) == 00) { // Failed device even

        pFailedDimmInfo->FailedDev = (UINT8)RetryRdErrLogAddr1.Bits.failed_dev;
        switch (pFailedDimmInfo->RegionSubType) {
        case ErrInFailedRank :
          RASDEBUG((DEBUG_INFO, "SVL Error in Failed Rank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.failed_cs & BIT2) >> 2;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.failed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.failed_bg;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->BuddyBank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_ba;
          pFailedDimmInfo->BuddyBG       = (UINT8)AdddcCtrRegion0.Bits.nonfailed_bg;
          break;
        case ErrInNonFailedRank :
          RASDEBUG((DEBUG_INFO, "SVL Error in Non Failed Rank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs & BIT2) >> 2;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.nonfailed_bg;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->BuddyBank     = (UINT8)AdddcCtrRegion0.Bits.failed_ba;
          pFailedDimmInfo->BuddyBG       = (UINT8)AdddcCtrRegion0.Bits.failed_bg;
          break;
        default:
          RASDEBUG((DEBUG_INFO, "SVL GetfailedDimminfo inconsistent!\n"));
          break;
        }
      }
      else{  // Failed device odd
        pFailedDimmInfo->FailedDev       = ((UINT8)RetryRdErrLogAddr1.Bits.failed_dev) - 1;
        switch (pFailedDimmInfo->RegionSubType) {
        case ErrInFailedRank :
          RASDEBUG((DEBUG_INFO, "SVL Error in Non Failed Rank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.nonfailed_cs & BIT2) >> 2;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.nonfailed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.nonfailed_bg;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->BuddyBank     = (UINT8)AdddcCtrRegion0.Bits.failed_ba;
          pFailedDimmInfo->BuddyBG       = (UINT8)AdddcCtrRegion0.Bits.failed_bg;
        case ErrInNonFailedRank :
          RASDEBUG((DEBUG_INFO, "SVL Error in Failed Rank\n"));
          pFailedDimmInfo->FailedRank    = (UINT8)AdddcCtrRegion0.Bits.failed_cs;
          pFailedDimmInfo->FailedSubRank = (UINT8)AdddcCtrRegion0.Bits.failed_c;
          pFailedDimmInfo->FailedDimm    = ((UINT8)AdddcCtrRegion0.Bits.failed_cs & BIT2) >> 2;
          pFailedDimmInfo->FailedBank    = (UINT8)AdddcCtrRegion0.Bits.failed_ba;
          pFailedDimmInfo->FailedBG      = (UINT8)AdddcCtrRegion0.Bits.failed_bg;
          pFailedDimmInfo->BuddyRank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_cs;
          pFailedDimmInfo->BuddySubRank  = (UINT8)AdddcCtrRegion0.Bits.nonfailed_c;
          pFailedDimmInfo->BuddyBank     = (UINT8)AdddcCtrRegion0.Bits.nonfailed_ba;
          pFailedDimmInfo->BuddyBG       = (UINT8)AdddcCtrRegion0.Bits.nonfailed_bg;
        }
      }
    } //SVL = 1
  } //Retry read valid

  //dimmMtr is dependent on failed Dimm info that is got from adddc regions if they are active
  dimmMtr.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, DIMMMTR_0_MC_MAIN_REG + ((pFailedDimmInfo->FailedDimm) * 4));
  pFailedDimmInfo->ddr3_width    = (UINT8)dimmMtr.Bits.ddr3_width;

  DispalyFailedErrorInfo(pFailedDimmInfo);
  DEBUG((DEBUG_INFO, "GetFailedDimmErrorInfo Exit :\n"));
  return EFI_SUCCESS;
}

/**

   Get System Address from Error Log

  @param NodeId          - Memory Node ID
  @param ErrorLogSysAddr - ptr to buffer to hold sysaddress that
                           has error

  @retval EFI_SUCCESS / Error code
  @retval ErrorLogSysAddr - System Address that has error

**/
EFI_STATUS
EFIAPI
GetErrorLogSysAddr(
    IN  UINT8    NodeId,
    OUT UINT64   *ErrorLogSysAddr
 )
{
  // TODO:
  //
  // Check MC log to see if address can be used
  // If yes, then return that address
  //
  //
  // If MC log doesn't have a valid address,
  // reverse translate the address
  // from patrol scrub lo/hi addr registers
  //

  DEBUG((DEBUG_ERROR, "GetErrorLogSysAddr: Not implemented yet\n"));
  return EFI_NOT_AVAILABLE_YET;
}


/**

   Get Memory node state

  @param Memory Node ID
  @param ptr to memory node state

  @retval SUCCESS or error code

**/
EFI_STATUS
EFIAPI
GetCurrentMemoryNodeState (
    IN UINT8      Node,
    IN OUT UINT8 *MemState
)
{
  if(Node >= MC_MAX_NODE) {
    return EFI_INVALID_PARAMETER;
  }
  *MemState = mMemRas->SystemMemInfo->Socket[NODE_TO_SKT(Node)].imcEnabled[NODE_TO_MC(Node)];
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ModifyRasFlag (
    IN UINT64                         FlagBit,
    IN UINT8                          Value
)
{

  //SKXEX_TODO: This is part of EX RAS flows. Yet to be ported from HSX-EX BIOS
  return EFI_SUCCESS;
}


/**

    Check correctable errors for independent mode and handle them

  @param NodeId    - Memory controller ID
  @param EventType - NEW_EVENTS or EVENTS_IN_PROGRESS
  @param *IsEventHandled - ptr to buffer to hold event handled status
    @retval status - EFI_SUCCESS if no failures
             @retval non-zero error code for failures

    IsEventHandled - TRUE -  event was handled
                     FALSE - event not handled

**/
EFI_STATUS
EFIAPI
ClearUnhandledCorrError(
    IN UINT8 NodeId,
    IN EVENT_TYPE EventType,
    OUT BOOLEAN *IsEventHandled
 ){
  UINT8                             ddrCh;
  UINT8                             rank = 0;
  BOOLEAN                           OverflowFound = FALSE;
  EFI_STATUS                        status = EFI_SUCCESS;

  *IsEventHandled       = FALSE;

  if(EventType == EVENTS_IN_PROGRESS){
    //
    // clear unhandled correct error events cannot be 'IN PROGRESS'
    // indicate that event as not handled always
    //
    status = EFI_SUCCESS;
    *IsEventHandled = FALSE;

    return status;
  }

  for( ddrCh = 0; (ddrCh < MAX_MC_CH) && (OverflowFound == FALSE); ddrCh++ ){
    //
    // Check if there is any rank in this channel
    // that has overflow bit set
    //
    for( rank = 0; (rank < MAX_RANK_CH) ; rank++ ){

      if( IsErrorExcdForThisRank( NodeId, ddrCh, rank) ){

        OverflowFound = TRUE;

        ClearErrorsForThisRank( NodeId, ddrCh, rank );

        //
        // Update event handled status
        //
        *IsEventHandled = TRUE;

        break;
      }
    } // for(rank ...)
  } // for(ddrCh ...)

  return status;
}

/**

Routine Description:
   Inject UC Error in specified address

Arguments:
    NodeId
    ErrorLogAddress

Returns:
   status

--*/
EFI_STATUS
EFIAPI
InjectPoison(
    IN UINT8   NodeId,
    IN UINT64  ErrorLogAddress
 )
{
  DEBUG((DEBUG_ERROR,"InjectPoison: not implemented yet\n"));
  return EFI_SUCCESS;
}

/*++

Routine Description:
    Check if Migration is possible between 2
    memory Nodes

Arguments:

  MasterNode  - Memory Node ID
  SlaveNode   - Slave Node ID
  * MigrationPossibleFlag - ptr to return buffer
  IsMirrorMigration - FALSE - Spare migration
                      TRUE  - Migration for mirror replacement

Returns:
  *MigrationPossibleFlag - 0 - Migration Not possible
                           1 - Migration possible
  EFI_SUCCESS

--*/
EFI_STATUS
EFIAPI
MigrationPossible (
    IN UINT8 SrcNode,
    IN UINT8 DstNode,
    OUT BOOLEAN *MigrationPossibleFlag,
    IN BOOLEAN  IsMirrorMigration
)
{
  DEBUG((DEBUG_ERROR,"MigrationPossible: not implemented yet\n"));
  return EFI_SUCCESS;
}

/*++

Routine Description:
    Sets up registers to enable migration

Arguments:

  SrcNodeId     - Source Memory Node
  DstNodeId     - Destination Memory Node

Returns:
  EFI_SUCCESS / Error code

  If this function encounters an error, it will soft-hang
--*/
EFI_STATUS
EFIAPI
SetupMemoryMigration(
    IN UINT8 SrcNodeId,
    IN UINT8 DstNodeId
 )
{
  DEBUG((DEBUG_ERROR,"SetupMemoryMigration: not implemented yet\n"));
  return EFI_SUCCESS;
}
/*++

Routine Description:
    Sets up registers to disable migration

Arguments:

  SrcNodeId     - Source Memory Node
  DstNodeId     - Destination Memory Node
  UpdateMemoryMap - Flag that indicates if memory map should be updated or not

Returns:
  EFI_SUCCESS / Error code

  If this function encounters an error, it will soft-hang
--*/
EFI_STATUS
EFIAPI
DisableMemoryMigration(
    IN UINT8 SrcNodeId,
    IN UINT8 DstNodeId
 )
{
  DEBUG((DEBUG_ERROR,"DisableMemoryMigration: not implemented yet\n"));
  return EFI_SUCCESS;
}

BOOLEAN
IsMirrorAvailable(
 )
{
  return ((mSystemMemoryMap->RasModesEnabled & CH_ALL_MIRROR));
}

/**

Routine Description:
   Get Memory RAS MODES supported

Arguments:
    NodeId
    DdrCh
    Rank

    If Rank is 0xFF, then return only non-rank RAS modes (sparing/wirekill)

Returns:
   Supported Memory RAS Modes

--*/
UINT8
GetSupportedMemRasModes(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  Rank
 )
{
  UINT8 supportedRasModes = 0;
  if(Rank < MAX_RANK_CH) {
    if(IsSpareRankAvailable(NodeId, DdrCh,Rank)) {
      supportedRasModes |= RK_SPARE;
    }

    if(IsSddcPlusOneAvailable(NodeId, DdrCh, Rank)) {
      supportedRasModes |= SDDC_EN;
    }

    if(IsAdddcAvailable(NodeId, DdrCh)) {
      supportedRasModes |= ADDDC_EN;
    }
  }
  return supportedRasModes;
}
  
EFI_STATUS
EFIAPI
MemoryNodeOnline(
    IN UINT8      Node,
    IN OUT UINT8  *MemHpStatus
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       OnlineOfflineStatus;
  PSYSHOST    host = NULL;

  OnlineOfflineStatus = *MemHpStatus;
  host = mRasMpLink->Host;
  switch (OnlineOfflineStatus){
    case MemNodeInstallInProgressEntry:
      //    
      // Save Old system memory map, setup data structures (PREPARE QUIESCE)
      // 
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\tMemoryNodeOnline STATE[PrepareQuiesce]\n"));

      if (!mRasMpLink->HostParmValid) {
        OnlineOfflineStatus = MemNodeError;
        break;
      }
      if ((Status = CallQuiesceSupport(PREPARE_QUIESCE)) != EFI_SUCCESS) {
        OnlineOfflineStatus = MemNodeError;
        break;
      }

      OnlineOfflineStatus = MemNodeInstallInProgress;

    case MemNodeInstallInProgress:
      //
      // Action = MEM_MAP_UPDATE starts here!
      // Add memory node to system memory map
      //
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\tMemoryNodeOnline STATE[UdpateSAD]\n"));
      if ((Status = RasMemMap(host)) != EFI_SUCCESS) {
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "MemoryNodeOnline MemMap FAIL ABORT!\n\n"));
        OnlineOfflineStatus = MemNodeInstallAbortQuiesce;
      } else {
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "MemoryNodeOnline MemMap SUCCESS!\n\n"));
        OnlineOfflineStatus = MemNodeInstallQuiesce;
      }
      break;

    case MemNodeInstallQuiesce:
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\tMemoryNodeOnline STATE[ExecuteQuiesce]\n"));
      CallQuiesceSupport (EXECUTE_QUIESCE);
      OnlineOfflineStatus = MemNodeUpdateStructuresOnline;
      break;

    case MemNodeUpdateStructuresOnline:
      //
      // Update System memory here. 
      //
      UpdateSystemMemoryMapStruct(host);  //SKX TODO: other MEM RAS may be impacted due to online, need each owner update this too
      OnlineOfflineStatus = MemNodeOnlineComplete;
      break;

    case MemNodeOnlineComplete:
      //clean up
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\tMemoryNodeOnline: OnlineCompleted\n"));
      break;
    case MemNodeInstallAbortQuiesce:
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\tMemoryNodeOnline STATE[AbortQuiesce]\n"));
      CallQuiesceSupport (ABORT_QUIESCE);

      OnlineOfflineStatus = MemNodeError;

    case MemNodeError:
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t\t\tMemoryNodeOnline STATE[ErrorAbort]\n"));

    default:
      break;
  }

  *MemHpStatus = OnlineOfflineStatus;

  return Status;
}

EFI_STATUS
EFIAPI
MemoryNodeOffline(
    IN UINT8      Node,
    IN OUT UINT8 *MemHpStatus
)
{
  DEBUG((DEBUG_ERROR,"MemoryNodeOffline: not support for Purley!!!\n"));
  return EFI_SUCCESS;
}

/**

Routine Description:
   Get Logical to Physical rank ID

Arguments:
    NodeId
    DdrCh
    Logical Rank

Returns:
  Physical Rank Id

--*/
UINT8
GetLogicalToPhysicalRankId(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  LogicalRank
 )
{
  UINT8   csMapping2DPC[] = {0, 1, 2, 3, 4, 5, 6, 7};
  UINT8   phyRank = 0xFF;

  if (LogicalRank >= 0 && LogicalRank < 8) {
    phyRank = csMapping2DPC[LogicalRank];
  }

  RAS_DEBUG((4,"GetLogicalToPhysicalRankId  logical rank:%d -->physical rank:%d\n",LogicalRank,phyRank));
  return phyRank;
}
