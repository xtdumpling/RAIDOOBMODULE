/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SparingEngine.c

  Functions to access/setup Sparing engine
**/

#include "SysFunc.h"
#include "SparingEngine.h"
#include "PatrolScrub.h"
#include "MemRas.h"
// APTIOV_SERVER_OVERRIDE_RC_START : To Resolve build error.
#include <Library/TimerLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : To Resolve build error.
// Externals used
extern  MEMRAS_S3_PARAM     mMemrasS3Param;
extern  UINT32  mAdddcRegionoffset;
extern  struct sysHost       mHost;
extern SPARING_EVENT_STRUCT mAdddcSparingEventStruct[MC_MAX_NODE][MAX_CH];
extern EFI_SMM_CPU_SERVICE_PROTOCOL     *m2SmmCpuServiceProtocol;
extern EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL        *mSmmPeriodicTimerDispatch;
extern EFI_HANDLE                                       mPeriodicTimerHandle;
SPIN_LOCK                                               mApLock;
UINT32                                                  mMwaitMonitorAddr;
UINT32                                                  mQuiescePcodeCounter=0;
ADDDC_PCODE_STATUS mAdddcPcodeSts[MAX_SOCKET];
static EFI_SMM_VARIABLE_PROTOCOL         *mSmmVariable = NULL;

#define MAX_MICRO_SECOND_DELAY           50
#define TAD_RULE_ONE					 1

//
// Data structure to maintain status for
// all memory nodes
//
SPARING_ENGINE_STATUS mSpareEngineStatus[MC_MAX_NODE];

//
// mSocketPkgCSemaphore will be used to enable / disable PkgC6states in a socket.
// PkgC6states can be enabled / disabled only at a socket level. If a node
// requests PkgC6states disable, the semaphore for the socket will be
// incremented.  If a node requests PkgCstates enable, the semaphore will
// be decremented. PkgCstates will be enabled only when mSocketGvSemaphore is 0
//
UINT8 mSocketPkgCSemaphore[MAX_SOCKET];

//
// mSocketGvSemaphore will be used to enable / disable GV in a socket.
// GV can be enabled / disabled only at a socket level. If a node
// requests GV disable, the semaphore for the socket will be
// incremented.  If a node requests GV enable, the semaphore will
// be decremented. GV will be enabled only when mSocketGvSemaphore is 0
//
UINT8 mSocketGvSemaphore[MAX_SOCKET] = {0};

/**

Disables pkg C states.

@param Node ID

@retval None

**/
VOID
DisablePkgC6(
     UINT8 NodeId)
{
  UINT8                           socket = NODE_TO_SKT(NodeId);
  EFI_STATUS                      Status;

  //
  // If PkgC state is already disabled in the socket, do nothing
  //
  if(!mSocketPkgCSemaphore[socket]){
    // RAS start disables the Pkg C states
        Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(socket, MAILBOX_BIOS_CMD_RAS_START, 0);
        ASSERT_EFI_ERROR(Status);
  }

  //
  // update PkgC semaphore
  //
  mSocketPkgCSemaphore[socket]++;

}

/**

  Enables pkg C states.

@param Node ID

@retval  None

**/

VOID
EnablePkgC6(
  UINT8 NodeId)
{
  EFI_STATUS                      Status;
  UINT8                           socket = NODE_TO_SKT(NodeId);
  //
  // Reset PkgC states for this NODE
  //
  if(mSocketPkgCSemaphore){
    mSocketPkgCSemaphore[socket]--;
  }

  //
  // Do not enable PkgC states even if 1 MC behind socket has disabled status
  //
  if(!mSocketPkgCSemaphore[socket]){
  // RAS Exit Re-enables the Pkg C states
        Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(socket, MAILBOX_BIOS_CMD_RAS_EXIT, 0);
        ASSERT_EFI_ERROR(Status);
  }
}

/**

Routine Description:
   Disable GV

Arguments:

    NodeId

Returns:
  None

--*/

VOID
DisableGV(
    UINT8 NodeId
)
{
  UINT8                           Socket = NODE_TO_SKT(NodeId);
  PCU_BIOS_SPARE2_PCU_FUN3_STRUCT pcuBiosSpare2Reg;

  SSKPD_N0_PCU_FUN1_STRUCT        sskPdReg;
  UINT8                           TimeOutCounter;

  RASDEBUG((DEBUG_INFO,"<DisableGV>\n"));

  //
  // If GV is already disabled in the socket, do nothing
  //
  if (mHost.setup.cpu.EnableGv) {
    if (!mSocketGvSemaphore[Socket]){
      pcuBiosSpare2Reg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG);
      RASDEBUG((DEBUG_INFO,"\t Socket:%d \n",Socket));
      //
      // Disable GV
      // Set bit[31] of pcu_bios_spare2
      //
      pcuBiosSpare2Reg.Data |= BIT31;
      mCpuCsrAccess->WriteCpuCsr(Socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG, pcuBiosSpare2Reg.Data);

      //
      // Poll for setting of bit[0] of register sskpd
      //

      for (TimeOutCounter = 0; TimeOutCounter < MAX_MICRO_SECOND_DELAY; TimeOutCounter++) {
        MicroSecondDelay(2);
        sskPdReg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, 0, SSKPD_N0_PCU_FUN1_REG);
        if (sskPdReg.Data & BIT0){
          break;
        }
      }
    }

    //
    // update GV semaphore
    //
    mSocketGvSemaphore[Socket]++;
    RASDEBUG((DEBUG_INFO,"\t mSocketGvSemaphore[%d]:%d\n",Socket,mSocketGvSemaphore[Socket]));
    RASDEBUG((DEBUG_INFO,"/ <DisableGV>\n"));
  }
}




/**

Routine Description:
   Enable GV

Arguments:

    NodeId

Returns:
  None

--*/
VOID
EnableGV(
  UINT8 NodeId
 )
{
  PCU_BIOS_SPARE2_PCU_FUN3_STRUCT pcuBiosSpare2Reg;
  SSKPD_N0_PCU_FUN1_STRUCT        sskPdReg;
  UINT8                           Socket = NODE_TO_SKT(NodeId);
  UINT8                           TimeOutCounter;

  RASDEBUG((DEBUG_INFO,"/ <EnableGV>\n"));
  //
  // Reset GV status for this NODE
  //
  if (mHost.setup.cpu.EnableGv) {
    if(mSocketGvSemaphore[Socket]){
      RASDEBUG((DEBUG_INFO,"/ mSocketGvSemaphore[%d]\n",Socket));
      mSocketGvSemaphore[Socket]--;
    }

    //
    // Do not enable GV even if 1 MC behind socket has disabled status
    //
    if(!mSocketGvSemaphore[Socket]){
      pcuBiosSpare2Reg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG);

      //
      // Enable GV
      // Clear bit[31] of pcu_bios_spare2
      //
      pcuBiosSpare2Reg.Data &= ~BIT31;
      mCpuCsrAccess->WriteCpuCsr(Socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG, pcuBiosSpare2Reg.Data);

      //
      // Poll for clearing of bit[0] of register sskpd
      //

      for (TimeOutCounter = 0; TimeOutCounter < MAX_MICRO_SECOND_DELAY; TimeOutCounter++) {
        MicroSecondDelay(2);
        sskPdReg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, 0, SSKPD_N0_PCU_FUN1_REG);
        if((sskPdReg.Data & BIT0) == 0){
          break;
        }
      }
    }
  }
  RASDEBUG((DEBUG_INFO,"/ <EnableGV>\n"));
}


EFI_STATUS
EFIAPI
MonitorMwaitApProc (
    VOID
)
{

  AsmMonitor((UINTN)&mMwaitMonitorAddr,0,0);

  // eax = 0f (C0 state); ecx = 00;
  AsmMwait(0xf,0);

  ReleaseSpinLock(&mApLock);

  return EFI_SUCCESS;
}


/**

Function to setup mwait on all the threads except Monarch.

@param MwaitFlag : 0 - write to monitor address
                 : 1 - Put all the threads in Mwait

@retval  None

**/
VOID
CallforMwait(
    UINT8 socket,
    UINT8 MwaitFlag  // monitor mwait entry and exit flag
    )
{

  EFI_STATUS  Status=EFI_SUCCESS;
  UINT64      CpuIndex;
  BOOLEAN     FoundThread=FALSE;
  EFI_PROCESSOR_INFORMATION                     ProcInfo;

  RASDEBUG((DEBUG_INFO,"<CallforMwait>\n"));

  if( MwaitFlag == 0) {
    //RASDEBUG((DEBUG_INFO,"\t Exit\n"));
    *(UINT32 *)(UINTN)(&mMwaitMonitorAddr)  = 0xabcdabcd;
    return;
  }

    InitializeSpinLock(&mApLock);
    for (CpuIndex = 0; CpuIndex < gSmst->NumberOfCpus; CpuIndex++) {
      FoundThread = FALSE;
      if(m2SmmCpuServiceProtocol->GetProcessorInfo(m2SmmCpuServiceProtocol,CpuIndex, &ProcInfo) == EFI_SUCCESS) {
        if((ProcInfo.Location.Package == socket) && (ProcInfo.StatusFlag & PROCESSOR_ENABLED_BIT) != 0) {
        FoundThread = TRUE;
        }
      }

      if (FoundThread){
        AcquireSpinLock(&mApLock);
        Status = gSmst->SmmStartupThisAp ((EFI_AP_PROCEDURE) MonitorMwaitApProc, CpuIndex, &socket);
        if(Status == EFI_SUCCESS) {
          while ( (AcquireSpinLockOrFail(&mApLock)) != TRUE  ) {
            CpuPause();
          }
        }
        ReleaseSpinLock(&mApLock);
      }
    }
  RASDEBUG((DEBUG_INFO,"/<CallforMwait>\n"));
}


/**

Function to block / unblock the iio for doing quiesce

@param BlockFlag : 1 - Block the iio
                 : 0 - unblock the iio

@retval  None

**/
VOID
BlockUnBlockIIO(
    UINT8 Socket,
    UINT8 BlockFlag
    )
{
  UINT8   IioStack;
  ITCARBBLOCK_IIO_DFX_GLOBAL_STRUCT Itcarbblock;

  if(BlockFlag == 1) {
    RASDEBUG((DEBUG_INFO,"Blocking IIO\n"));
    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
      Itcarbblock.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, IioStack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG);
      Itcarbblock.Data = 0x19FF9;
      mCpuCsrAccess->WriteCpuCsr(Socket, IioStack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG, Itcarbblock.Data);
    }
  } else if(BlockFlag == 0) {
    RASDEBUG((DEBUG_INFO,"UnBlocking IIO\n"));
    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
      Itcarbblock.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, IioStack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG);
      Itcarbblock.Data = 0x1;
      mCpuCsrAccess->WriteCpuCsr(Socket, IioStack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG, Itcarbblock.Data);
    }
    for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
      Itcarbblock.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, IioStack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG);
      Itcarbblock.Data = 0x0;
      mCpuCsrAccess->WriteCpuCsr(Socket, IioStack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG, Itcarbblock.Data);
    }
  }

}

VOID
SwitchAddressModeToLegacy(
   UINT8 Node
   )
{
  SCRUBADDRESSHI_MC_MAIN_STRUCT   ScrubAddrHi;
  UINT8                           Socket = NODE_TO_SKT(Node);
  UINT8                           Mc = NODE_TO_MC(Node);

  ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);

  //
  // If the patrol scrub mode is SPA mode, need to move to legacy mode to setup the registers
  // and then move back to SPA mode. 
  //

  if (ScrubAddrHi.Bits.ptl_sa_mode == 1) {
    ScrubAddrHi.Bits.ptl_sa_mode = 0;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);
  }
}

VOID
SwitchAddressModeToSystem(
   UINT8 Node
   )
{
  SCRUBADDRESSHI_MC_MAIN_STRUCT   ScrubAddrHi;
  UINT8                           Socket = NODE_TO_SKT(Node);
  UINT8                           Mc = NODE_TO_MC(Node);
  if (mSystemMemoryMap->MemSetup.patrolScrubAddrMode & PATROL_SCRUB_SPA)
  {
    ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);

    if (ScrubAddrHi.Bits.ptl_sa_mode == 0) {
      ScrubAddrHi.Bits.ptl_sa_mode = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);
    }
  }
}

/**

  Checks the internal status and takes ownership of the sparing engine

  @param NodeId - Memory controller ID

  @retval EFI_SUCCESS if sparing engine is free
  @retval EFI_OUT_OF_RESOURCES if sparing engine is already in use

**/
EFI_STATUS
AcquireSparingEngine(
    IN UINT8 NodeId
 )
{
  EFI_STATUS status = EFI_SUCCESS;

  RAS_DEBUG((4, "AcquireSparingEngine - Node[%d] Entry\n", NodeId));

  if(mSpareEngineStatus[NodeId] == SparingEngineFree) {
    //
    // Disable patrol scrub if enabled
    //
    if(CheckPatrolScrubEnable()){    
      status = SavePatrolScrubEngine(NodeId);
      status = DisablePatrolScrubEngine( NodeId);
    }

    if( status == EFI_SUCCESS){
      mSpareEngineStatus[NodeId] = SparingEngineInUse;
      RAS_DEBUG((4, "AcquireSparingEngine - Node%d - Success\n", NodeId));
      // Disable Pkg C6 before Sparing
      DisablePkgC6(NodeId);
      DisableGV(NodeId);
    }
  } else {
    status = EFI_OUT_OF_RESOURCES;
    RAS_DEBUG((4, "AcquireSparingEngine - Node%d - Failed\n", NodeId));
  }

  RAS_DEBUG((4, "AcquireSparingEngine - Node[%d] Exit\n", NodeId));
  return status;
}

/**

  Checks the internal status and releases ownership of the sparing engine.
  It is expected that only caller who has successfully acquired ownership
  using AcquireSparingEngine calls this function

  @param NodeId - Memory controller ID

  @retval EFI_SUCCESS if sparing engine is released
  @retval EFI_UNSUPPORTED any other error

**/
EFI_STATUS
ReleaseSparingEngine(
    IN UINT8 NodeId
 )
{
  EFI_STATUS status = EFI_SUCCESS;

  RAS_DEBUG((1, "ReleaseSparingEngine - Node[%d] Entry\n", NodeId));
  if(mSpareEngineStatus[NodeId] == SparingEngineInUse) {

    //
    // Re-enable patrol scrub if it was enabled before
    //
    if(CheckPatrolScrubEnable()){
      status = ReEnablePatrolScrubEngine( NodeId);
    }

    //enable PkgC6 after sparing
    EnablePkgC6(NodeId);
    EnableGV(NodeId);

    mSpareEngineStatus[NodeId] = SparingEngineFree;

  } else {
    status = EFI_UNSUPPORTED;     // called before AcquireSparingEngine
    RAS_DEBUG((2, "ReleaseSparingEngine - Node%d - Failed\n", NodeId));
  }

  RAS_DEBUG((1, "ReleaseSparingEngine - Node[%d] Exit\n", NodeId));
  return status;
}


/*++

Routine Description:

This is the callback routine registered to PeriodicSMI handler.
This routine will check if the SMBus recovery process is complete.
If so, will end the recovery process and unregister the periodic handler.


Arguments:

No Arguments

Returns:

Success if the SMBus is recovered, and the periodic handler is unregistered


--*/

EFI_STATUS
M2MQuiescePCodeCallback (
  IN  EFI_HANDLE     Handle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  )

{
  EFI_STATUS                                  Status;
  UINT32                                      MailBoxCommand;
  UINT32                                      MailboxData;
  UINT32                                      MailboxSts;
  UINT8                                       Socket;
  UINT8                                       Mc;
  DEFEATURES0_M2MEM_MAIN_STRUCT               M2mDefeatures0;

  //Look for the Socket and Mc for which the Quiesce was in progress
  for(Socket=0;Socket<MAX_SOCKET;Socket++){
    for(Mc=0;Mc<MAX_IMC;Mc++){
      if(mAdddcPcodeSts[Socket]==QuiesceInProgress) {
        /*5330799: Cloned From SKX Si Bug Eco: egress bypass with quiesce flow
        (pfmiss or force) cause CR chunk to be sent out twice onto mesh*/

        if(mMemRas->pSysHost->var.common.stepping < B0_REV_SKX) {
          M2mDefeatures0.Data    = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, DEFEATURES0_M2MEM_MAIN_REG);
          M2mDefeatures0.Bits.egrbypdis = 1;
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, DEFEATURES0_M2MEM_MAIN_REG, M2mDefeatures0.Data);
        }

        MailBoxCommand = 0xb0;
        MailboxData = 2 | (Mc << 0x8);
        MailboxSts = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MailBoxCommand, MailboxData);
        mQuiescePcodeCounter++;

        if ((MailboxSts==0)||(mQuiescePcodeCounter == 100)) {
          mAdddcPcodeSts[Socket] = QuiesceAvailable;
          Status = mSmmPeriodicTimerDispatch->UnRegister(mSmmPeriodicTimerDispatch, mPeriodicTimerHandle);
          mPeriodicTimerHandle  = NULL;
          ASSERT_EFI_ERROR (Status);
        }
      }
    }
  }
  return EFI_SUCCESS;
}

/**

  Pcode mailbox call that enables M2M quiesce. The spare enable bit is set in Pcode after Quiesce

  @param Socket    - Socket ID
  @param Mc        - Node ID


  @retval EFI_SUCCESS if spare copy is started and the operation successful


**/

EFI_STATUS
M2MQuiescePCodeInterface(
UINT8 Socket,
UINT32 MailBoxCommand,
UINT32 MailboxData
)
{

  UINT32  MailboxSts = 0;
  EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT   PeriodicTimer;
  EFI_STATUS Status = EFI_SUCCESS;

  if(mAdddcPcodeSts[Socket] != QuiesceInProgress) {

    MailboxSts = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MailBoxCommand, MailboxData);

    if (MailboxSts == 0) {
      RASDEBUG((DEBUG_INFO,"Quiesce Successful\n"));
      return Status;
    }
    else {
      RASDEBUG((DEBUG_INFO,"Quiesce going to RETRY\n"));
      //Attempt to read the quiesce status twice before initiating periodic SMI for reading quiesce status
      MailboxSts = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MailBoxCommand, MailboxData);
      if(( MailboxSts !=0) && (mAdddcPcodeSts[Socket] != 01)) {
        //Initiate periodic SMI to trigger here

        if(!mPeriodicTimerHandle) {
          PeriodicTimer.Period = 15000;  // to convert to 100 nano seconds
          PeriodicTimer.SmiTickInterval = 15000; //minimum possible tick supported in SKX
          mAdddcPcodeSts[Socket] = QuiesceInProgress;
          Status = mSmmPeriodicTimerDispatch->Register(mSmmPeriodicTimerDispatch, M2MQuiescePCodeCallback,  &PeriodicTimer, &mPeriodicTimerHandle);
          ASSERT_EFI_ERROR (Status);
        }
      }
    }
  }

  else {
    Status = EFI_OUT_OF_RESOURCES;
    RAS_DEBUG((4, "M2MQuiescePCodeInterface - Socket%d - Failed\n", Socket));
  }
  return Status;
}

VOID
M2MQuiesceUnquiesce(
    UINT8 Socket,
    UINT8 Mc,
    IN  BOOLEAN QuiesceFlag
    )
{
  M2MBIOSQUIESCE_M2MEM_MAIN_STRUCT            M2mBiosQuiesce;

  if(QuiesceFlag){

    BlockUnBlockIIO(Socket, IIO_BLOCK_QUIESCE);
    CallforMwait (Socket, MWAIT_START_QUIESCE);

    M2mBiosQuiesce.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, M2MBIOSQUIESCE_M2MEM_MAIN_REG);
    M2mBiosQuiesce.Bits.forcequiescem2m = 1;
    M2mBiosQuiesce.Bits.drainrttimer =  0x78;
    mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, M2MBIOSQUIESCE_M2MEM_MAIN_REG, M2mBiosQuiesce.Data);

  } else {
    M2mBiosQuiesce.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, M2MBIOSQUIESCE_M2MEM_MAIN_REG);
    M2mBiosQuiesce.Bits.forcequiescem2m = 0;
    mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, M2MBIOSQUIESCE_M2MEM_MAIN_REG, M2mBiosQuiesce.Data);

    CallforMwait (Socket, MWAIT_END_QUIESCE);
    BlockUnBlockIIO(Socket,IIO_UNBLOCK_QUIESCE);
  }

}

VOID
DisplayADDDDCRegisterValues (
    IN UINT8  Socket,
    IN UINT8  Mc,
    IN UINT8  Ch,
    IN UINT8  RegionNum
    )
{
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  SPARING_CONTROL_SOURCE_MC_MAIN_STRUCT       spareCtlSrc;
  SPARING_CONTROL_DESTINATION_MC_MAIN_STRUCT  spareCtlDest;
  //RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_STRUCT   RetryRdErrLogAddr1;
  ADDDC_REGION0_MCDDC_DP_STRUCT               AdddcRegion;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT        AdddcCtrRegion;
  UINT32                                      AdddcCtrlRegionoffset;
  UINT32                                      AdddcRegionoffset;
  MCMTR_MC_MAIN_STRUCT                        McMtrMain;
  MODE_M2MEM_MAIN_STRUCT                      ModeMain;
  UINT8                                       SktCh;
  SktCh = NODECH_TO_SKTCH(Mc, Ch);

  RASDEBUG((DEBUG_INFO, "-----------------------------------------------------------------------------\n"));
  RASDEBUG((DEBUG_INFO, "\t\t\tDisplaying ADDDC Register Values\n \t\t Skt[%d]iMc[%d]ch[%d]Region[%d] \n ", Socket, Mc, Ch, RegionNum));
  RASDEBUG((DEBUG_INFO, "-----------------------------------------------------------------------------\n"));

  spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
  RASDEBUG((DEBUG_INFO, "SPARING_CONTROL_MC_MAIN_REG : %x\n", spareCtlReg.Data));

  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.spare_enable        : %x\n", spareCtlReg.Bits.spare_enable));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.Channel_select      : %x\n", spareCtlReg.Bits.channel_select));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.reverse             : %x\n", spareCtlReg.Bits.reverse));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.rank_sparing        : %x\n", spareCtlReg.Bits.rank_sparing));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.adddc_sparing       : %x\n", spareCtlReg.Bits.adddc_sparing));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.sddc_sparing        : %x\n", spareCtlReg.Bits.sddc_sparing));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.region_size         : %x\n", spareCtlReg.Bits.region_size));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.virtual_lockstep_en : %x\n", spareCtlReg.Bits.virtual_lockstep_en));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.minimum_tad_rule    : %x\n", spareCtlReg.Bits.minimum_tad_rule));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlReg.Bits.maximum_tad_rule    : %x\n", spareCtlReg.Bits.maximum_tad_rule));

  spareCtlSrc.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG);
  RASDEBUG((DEBUG_INFO, "\nSPARING_CONTROL_SOURCE_MC_MAIN_REG : %x\n", spareCtlSrc.Data));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlSrc.Bits.source_cs     : %x\n", spareCtlSrc.Bits.source_cs ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlSrc.Bits.source_c      : %x\n", spareCtlSrc.Bits.source_c ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlSrc.Bits.source_ba     : %x\n", spareCtlSrc.Bits.source_ba ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlSrc.Bits.source_bg     : %x\n", spareCtlSrc.Bits.source_bg ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlSrc.Bits.buddy_cs_en   : %x\n", spareCtlSrc.Bits.buddy_cs_en ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlSrc.Bits.buddy_cs      : %x\n", spareCtlSrc.Bits.buddy_cs ));

  spareCtlDest.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG);
  RASDEBUG((DEBUG_INFO, "\nSPARING_CONTROL_DESTINATION_MC_MAIN_REG : %x\n", spareCtlDest.Data));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlDest.Bits.destination_cs     : %x\n", spareCtlDest.Bits.destination_cs ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlDest.Bits.destination_c      : %x\n", spareCtlDest.Bits.destination_c ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlDest.Bits.destination_ba     : %x\n", spareCtlDest.Bits.destination_ba ));
  RASDEBUG((DEBUG_INFO, "\t\t spareCtlDest.Bits.destination_bg     : %x\n", spareCtlDest.Bits.destination_bg ));

  AdddcCtrlRegionoffset = (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionNum*4);
  AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AdddcCtrlRegionoffset);
  RASDEBUG((DEBUG_INFO, "\n ADDDC_REGION0_CONTROL_MC_MAIN_REG : \n", AdddcCtrRegion.Data));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.region_enable     : %x\n", AdddcCtrRegion.Bits.region_enable ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.region_size       : %x\n", AdddcCtrRegion.Bits.region_size ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.failed_cs         : %x\n", AdddcCtrRegion.Bits.failed_cs ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.failed_c          : %x\n", AdddcCtrRegion.Bits.failed_c ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.failed_ba         : %x\n", AdddcCtrRegion.Bits.failed_ba ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.failed_bg         : %x\n", AdddcCtrRegion.Bits.failed_bg ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.nonfailed_cs      : %x\n", AdddcCtrRegion.Bits.nonfailed_cs ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.nonfailed_c       : %x\n", AdddcCtrRegion.Bits.nonfailed_c ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.nonfailed_ba      : %x\n", AdddcCtrRegion.Bits.nonfailed_ba ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.nonfailed_bg      : %x\n", AdddcCtrRegion.Bits.nonfailed_bg ));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcCtrlRegionoffset.Bits.copy_in_progress  : %x\n", AdddcCtrRegion.Bits.copy_in_progress ));

  AdddcRegionoffset     = (ADDDC_REGION0_MCDDC_DP_REG+RegionNum);
  AdddcRegion.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AdddcRegionoffset);
  RASDEBUG((DEBUG_INFO, "\n ADDDC_REGION0_MCDDC_DP_REG : %x \n", AdddcRegion.Data));
  RASDEBUG((DEBUG_INFO, "\t\t AdddcRegion.Bits.faildevice     : %x\n", AdddcRegion.Bits.faildevice ));

  McMtrMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG);
  RASDEBUG((DEBUG_INFO, "\t\t McMtrMain.ch23cmd_ctl_delay  : %x, Adddc_mode : %x\n", McMtrMain.Bits.ch23cmd_ctl_delay, McMtrMain.Bits.adddc_mode ));
  ModeMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MODE_M2MEM_MAIN_REG);
  RASDEBUG((DEBUG_INFO, "\t\t Adddc_mode : %x\n", ModeMain.Bits.adddc ));

}


/**

  Initialize the VIRTUAL_LOCKSTEP_VARIABLE_NAME variable to 0s at the boot time.

  @param NONE      - Node ID

  @retval NONE

**/
VOID
InitializeVLSInfo()
{
  EFI_STATUS                          Status;
  VIRTUAL_LOCKSTEP_VARIABLE_DATA      VLSInfo;
  UINTN                               VLSVarSize;
  EDKII_VARIABLE_LOCK_PROTOCOL        *VariableLock;

  DEBUG((DEBUG_INFO,"<InitializeVLSInfo>\n"));
  VLSVarSize = sizeof(VIRTUAL_LOCKSTEP_VARIABLE_DATA);
  ZeroMem (&VLSInfo, VLSVarSize);

  //
  // Clear VLS Variable from storage
  //
  Status = gRT->SetVariable (
      VIRTUAL_LOCKSTEP_VARIABLE_NAME,
      &gEfiVirtualLockstepGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
      VLSVarSize,
      &VLSInfo
  );

  if(EFI_ERROR(Status)){
    DEBUG((DEBUG_INFO,"ERROR INITIALIZING - In Set Variable status:%d \n",Status));
  }

  //
  // Mark VLS_VARIABLE_NAME variable to read-only if the Variable Lock protocol exists
  // This will allow access only through smm and restrict 3rd party access
  //
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    Status = VariableLock->RequestToLock (VariableLock, VIRTUAL_LOCKSTEP_VARIABLE_NAME, &gEfiVirtualLockstepGuid);
    ASSERT_EFI_ERROR (Status);
  }

}

/**

  Update VLS EFI Variable to indicate if VLS happened in this boot. This value will be
  read in the next boot to force Normal boot if true.

  @param VLSEstablished      - Boolean to indicate true or false

  @retval EFI_STATUS         - SUCCESS or EFI_ERROR

**/
EFI_STATUS
UpdateVLSVariable (
    BOOLEAN VLSEstablished
    )
{
  VIRTUAL_LOCKSTEP_VARIABLE_DATA              VLSInfo;
  UINTN                                       VLSVarSize;
  EFI_STATUS                                  Status;

  VLSVarSize = sizeof(VIRTUAL_LOCKSTEP_VARIABLE_DATA);


  DEBUG((DEBUG_INFO,"<UpdateVLSVariable>\n"));
  Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, &mSmmVariable);
  ASSERT_EFI_ERROR (Status);

  Status = mSmmVariable->SmmGetVariable (
                              VIRTUAL_LOCKSTEP_VARIABLE_NAME,
                              &gEfiVirtualLockstepGuid,
                              NULL,
                              &VLSVarSize,
                              &VLSInfo
                              );

  //Variable was already updated before
  if(Status == EFI_SUCCESS) {
    if (VLSInfo.VirtualLockstepEstablished) {
      //DEBUG((DEBUG_INFO,"Previously updated Virtual Lock step variable structure :%d\n",VLSInfo.VirtualLockstepEstablished));
      //Update the structure only one time
      return EFI_SUCCESS;
    }
  }
  if (EFI_ERROR(Status) || (!VLSInfo.VirtualLockstepEstablished) ) {
    //
    // Variable is not present. Initialize the data structure to all 0
    //
    ZeroMem (&VLSInfo, VLSVarSize);
  }

  if(VLSEstablished) {
    VLSInfo.VirtualLockstepEstablished = TRUE;
  } else {
    VLSInfo.VirtualLockstepEstablished = FALSE;
  }

  Status = mSmmVariable->SmmSetVariable (
                              VIRTUAL_LOCKSTEP_VARIABLE_NAME,
                              &gEfiVirtualLockstepGuid,
                              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                              VLSVarSize,
                              &VLSInfo
                              );

  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Creating VLS established variable failed - Status: %d\n", Status));
  }


  return Status;

}

/**

  Programs the hardware registers for DDDC or Rank Sparing and
  starts spare copy operation

  @param NodeId    - Memory controller ID
  @param SrcRank   - Source rank
  @param DstRank   - Destination rank or target rank
  @param Ch        - Channel in which Src and Destination ranks reside
  @param SparingMode - Type of sparing - DDDC / Rank

  @retval EFI_SUCCESS if spare copy is started
  @retval EFI_DEVICE_ERROR if there is an error in spare copy operation
  @retval EFI_UNSUPPORTED all other errors

**/
EFI_STATUS
StartSparing(
    IN UINT8 NodeId,
    IN UINT8 SrcRank,
    IN UINT8 DstRank,
    IN UINT8 Ch,
    IN SPARING_MODE SparingMode,
    IN UINT8  RegionNum
 )
{
  EFI_STATUS                                  status;
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  SPARING_CONTROL_SOURCE_MC_MAIN_STRUCT       spareCtlSrc;
  SPARING_CONTROL_DESTINATION_MC_MAIN_STRUCT  spareCtlDest;
  SCRUBADDRESSHI_MC_MAIN_STRUCT               ScrubAddrHiReg;
  DEFEATURES0_M2MEM_MAIN_STRUCT               M2mDefeatures0;
  DEFEATURES1_M2MEM_MAIN_STRUCT               M2mDefeatures1;
  RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_STRUCT   RetryRdErrLogAddr1;
  MCMTR_MC_MAIN_STRUCT						            McmtrMain;
  MODE_M2MEM_MAIN_STRUCT					            ModeMain;
  ADDDC_REGION0_MCDDC_DP_STRUCT               AdddcRegion;
  ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT		    AdddcCtrRegion;
  SPARING_2LM_CONTROL_MC_MAIN_STRUCT		      Sparing2LMCtrl;
  SPARING_2LM_ADDR0HI_MC_MAIN_STRUCT		      Sparing2LMAddr0Hi;
  SPARING_2LM_ADDR1HI_MC_MAIN_STRUCT		      Sparing2LMAddr1Hi;
  SPARING_2LM_ADDR2HI_MC_MAIN_STRUCT		      Sparing2LMAddr2Hi;
  SPARING_2LM_ADDR0LO_MC_MAIN_STRUCT          Sparing2LMAddr0Lo;
  SPARING_2LM_ADDR1LO_MC_MAIN_STRUCT          Sparing2LMAddr1Lo;
  SPARING_2LM_ADDR2LO_MC_MAIN_STRUCT          Sparing2LMAddr2Lo;
  MCNMCACHINGCFG2_MC_2LM_STRUCT               mcNMCachingCfg2;
  SPARING_CONTROL_MC_MAIN_STRUCT              iMcSparingCtrl;
  UINT32                                      SparingStartAddr;
  UINT32									                    ChNMSize;
  UINT8                                       Socket, Mc, SktCh;
  UINT32                                      MailboxData = 0;

  AMAP_MC_MAIN_STRUCT                         AMAPReg;
  UINT32                                      SparePhysicalRank, SpareDimm, SpareRankPerDimm;
  UINT32                                      FailedPhysicalRank, FailedDimm, FailedRankPerDimm;
  UINT8		 								                    TadIndex;
  struct  									                  TADTable *TAD;
  UINT8 									                    FailedDevice;
  UINT32                                      AdddcCtrlRegionoffset, AdddcRegionoffset;
  VMSE_CFG_READ_1_MCDDC_DP_STRUCT             VmseCfgRead;
  PLUS1_RANK0_MCDDC_DP_STRUCT                 Plus1Rank0;
  UINT8                                       FailedRank;
  BOOLEAN									                    FastDivThree = FALSE;
  UINT8                                       ChNMSizeLog2;
  UINT8                                       ChIndex;
  struct ddrChannel                           *channelList;
  TCLRDP_MCDDC_CTL_STRUCT                     tddr4;
  VMSE_ERROR_MCDDC_DP_STRUCT                  VMSEErrorData;
  MCWDB_CHKN_BIT_MCDDC_DP_STRUCT              McWdbChknBit;
  MCSCHED_CHKN_BIT2_MCDDC_CTL_STRUCT          McSchedChknBit2;
  TCOTHP_MCDDC_CTL_STRUCT                     TCOTHP;
  TCOTHP2_MCDDC_CTL_STRUCT                    TCOTHP2;
  TCDBP_MCDDC_CTL_STRUCT                      TCDBP;
  DDR4_CA_CTL_MCDDC_DP_STRUCT                 DDR4CACtl;
  UINT8                                       VmseErrorLatency;
  TADBASE_0_MC_MAIN_STRUCT          		      mcTADBase;
  UINT32                                      BaseAddress, MaxTadAddress;
  UINT32									  t_ccd_wr;

  UINT32 TADBase[TAD_RULES] =
    { TADBASE_0_MC_MAIN_REG, TADBASE_1_MC_MAIN_REG, TADBASE_2_MC_MAIN_REG, TADBASE_3_MC_MAIN_REG,
      TADBASE_4_MC_MAIN_REG, TADBASE_5_MC_MAIN_REG, TADBASE_6_MC_MAIN_REG, TADBASE_7_MC_MAIN_REG } ;


  Socket          = NODE_TO_SKT(NodeId);
  Mc              = NODE_TO_MC(NodeId);
  SktCh           = NODECH_TO_SKTCH(NodeId, Ch);
  status          = EFI_SUCCESS;
  TAD             = &mSystemMemoryMap->Socket[Socket].imc[Mc].TAD[0];
  channelList     = &mHost.var.mem.socket[Socket].channelList[0];

  tddr4.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, TCLRDP_MCDDC_CTL_REG);
  t_ccd_wr = tddr4.Bits.t_ccd_wr;

  switch(SparingMode){
    case RankSparing:

      //A0 W/A 4930005: Cloned From SKX Si Bug Eco: DUE not detected for RNKSPR in x4sddc ecc mode
      if(mHost.var.common.stepping < B0_REV_SKX) {
        VmseCfgRead.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, VMSE_CFG_READ_1_MCDDC_DP_REG);
        VmseCfgRead.Bits.read_data |= BIT26;
        mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, VMSE_CFG_READ_1_MCDDC_DP_REG, VmseCfgRead.Data);
      }

      //B0 W/A 4930005: Cloned From SKX Si Bug Eco: DUE not detected for RNKSPR in x4sddc ecc mode
      else {
        Plus1Rank0.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, (PLUS1_RANK0_MCDDC_DP_REG+SrcRank));
        RetryRdErrLogAddr1.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_REG);
        FailedDevice      = (UINT8)RetryRdErrLogAddr1.Bits.failed_dev;
        FailedRank        = (UINT8)RetryRdErrLogAddr1.Bits.chip_select;
        if(FailedRank == SrcRank) {
            Plus1Rank0.Bits.faildevice = FailedDevice;
        }
        mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, (PLUS1_RANK0_MCDDC_DP_REG+SrcRank), Plus1Rank0.Data);
      }

      //
      // make sure scrub ctrl register is not set to scrub on failed rank after sparing.
      //
      ScrubAddrHiReg.Data=  mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);
      if( (ScrubAddrHiReg.Bits.chnl == Ch) && (ScrubAddrHiReg.Bits.rank == mMemRas->GetLogicalToPhysicalRankId(NodeId, Ch, SrcRank)) )  {
        RAS_DEBUG((4, "StartSparing - spare ctrl Src Logical rank[%x] same as Scrubbing channel phy. rankbefore sparing. Chaning the scrub rank to spare ctrl Dstrank[%x] \n", SrcRank, DstRank ));
        ScrubAddrHiReg.Bits.rank  = mMemRas->GetLogicalToPhysicalRankId(NodeId, Ch, DstRank);
        mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG, 00);
        mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHiReg.Data);
      } else {
        RAS_DEBUG((4, "StartSparing - Spare ctrl Src Logical rank[%x] is not same as Scrubbing channel phy. rank[%x] before sparing.\n", SrcRank, ScrubAddrHiReg.Bits.rank ));
      }

      //
      // update AMAP register to exclude spare rank.
      //
      AMAPReg.Data= mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AMAP_MC_MAIN_REG);
      SparePhysicalRank = mMemRas->GetLogicalToPhysicalRankId(NodeId,Ch,DstRank);
      SpareDimm = SparePhysicalRank /4;
      SpareRankPerDimm  = SparePhysicalRank % 4;
      FailedPhysicalRank = mMemRas->GetLogicalToPhysicalRankId(NodeId,Ch,SrcRank);
      FailedDimm = FailedPhysicalRank /4;
      FailedRankPerDimm  = FailedPhysicalRank % 4;

      RAS_DEBUG((4, "StartSparing -  Spare Rank :%x  SpareDimm:%x \n", DstRank, SpareDimm));
      RAS_DEBUG((4, "StartSparing -  SpareRankPerDimm :%x  AmapReadValue:%x \n", SpareRankPerDimm, AMAPReg.Data));

      if(SpareDimm == 0){
          AMAPReg.Bits.dimm0_pat_rank_disable  &= (~(1 << SpareRankPerDimm));
      }
      if(SpareDimm == 1){
        AMAPReg.Bits.dimm1_pat_rank_disable  &= (~(1 << SpareRankPerDimm));
      }
      if(FailedDimm == 0){
        AMAPReg.Bits.dimm0_pat_rank_disable  |= (1 << FailedRankPerDimm);
      }
      if(FailedDimm == 1){
        AMAPReg.Bits.dimm1_pat_rank_disable  |= (1 << FailedRankPerDimm);
      }

      RAS_DEBUG((4, "StartSparing -  AMAP value Channel:%d  reg:%x \n", Ch, AMAPReg.Data));
      mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, AMAP_MC_MAIN_REG, AMAPReg.Data);


      // Set the source rank
      spareCtlSrc.Data =  mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG);  
      spareCtlSrc.Bits.source_cs = SrcRank;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG, spareCtlSrc.Data);

      // Set the destination rank
      spareCtlDest.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG);

      spareCtlDest.Bits.destination_cs = DstRank;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG, spareCtlDest.Data);

      // Set the spare mode in the Ch as rank_sparing
      spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
      spareCtlReg.Bits.channel_select  = Ch;
      spareCtlReg.Bits.rank_sparing    = 1;
      spareCtlReg.Bits.sddc_sparing    = 0;
      spareCtlReg.Bits.adddc_sparing   = 0;
      spareCtlReg.Bits.region_size     = 1; //Rank
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

      // Record spare in use
      mMemrasS3Param.spareInUse[NodeId][Ch]       = 1;
      mMemrasS3Param.oldLogicalRank[NodeId][Ch]   = SrcRank;
      mMemrasS3Param.spareLogicalRank[NodeId][Ch]   = DstRank;

      break;

    case BankAdddcSparingMR:
      DEBUG((DEBUG_INFO,"StartSparing: BankAdddcSparingMR\n"));

      /*5370517: CLONE SKX Sighting: [ADDDC/SVL] 2B writes and write merge causes UC errors*/

      if(mMemRas->pSysHost->var.common.stepping < H0_REV_SKX) {
        TCOTHP.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, TCOTHP_MCDDC_CTL_REG);
        TCOTHP2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, TCOTHP2_MCDDC_CTL_REG);
        TCDBP.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, TCDBP_MCDDC_CTL_REG);
        McWdbChknBit.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, MCWDB_CHKN_BIT_MCDDC_DP_REG);
  
        // Update VMSE Error Latency
        VMSEErrorData.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, VMSE_ERROR_MCDDC_DP_REG);
        VmseErrorLatency = (UINT8)TCDBP.Bits.t_cwl + (UINT8)TCOTHP.Bits.t_cwl_adj - (UINT8)TCOTHP2.Bits.t_cwl_adj_neg - 6;
 
        if (McWdbChknBit.Bits.dis_ddrio_earlywdata == 0) {
          VmseErrorLatency = VmseErrorLatency - 1;
        }

        /*5371638: CLONE SKX Sighting: SVL failing on supercollider w/CECC */
        if ((mMemRas->pSysHost->var.common.stepping == B0_REV_SKX) || (mMemRas->pSysHost->var.common.stepping == B1_REV_SKX) || (mMemRas->pSysHost->var.common.stepping == L0_REV_SKX)) {
          McSchedChknBit2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG);
          VmseErrorLatency = VmseErrorLatency + (UINT8)McSchedChknBit2.Bits.x8_wpq_dly - 4;
        }

        VMSEErrorData.Bits.vmse_err_latency = VmseErrorLatency;
        mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, VMSE_ERROR_MCDDC_DP_REG, VMSEErrorData.Data);

        // Disable Error Flow
        DDR4CACtl.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, DDR4_CA_CTL_MCDDC_DP_REG);
        DDR4CACtl.Bits.erf_en0 = 0;
        mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, DDR4_CA_CTL_MCDDC_DP_REG, DDR4CACtl.Data);
      }

      if(mSystemMemoryMap->RasModesEnabled & FULL_MIRROR_1LM) {
        iMcSparingCtrl.Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
        iMcSparingCtrl.Bits.mirr_adddc_en = 1;
        mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, iMcSparingCtrl.Data);
      }

      if(mSystemMemoryMap->RasModesEnabled & PARTIAL_MIRROR_1LM) {
        iMcSparingCtrl.Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
        iMcSparingCtrl.Bits.mirr_adddc_en = 1;
        iMcSparingCtrl.Bits.partial_mirr_en = 1;
        mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, iMcSparingCtrl.Data);
      }

      //Indicate the presence of VLS with the variable
      UpdateVLSVariable(TRUE);

      AdddcCtrlRegionoffset = (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionNum*4);
      AdddcRegionoffset     = (ADDDC_REGION0_MCDDC_DP_REG+RegionNum);
      spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
      spareCtlReg.Bits.channel_select  = Ch;
      spareCtlReg.Bits.rank_sparing    = 0;
      spareCtlReg.Bits.sddc_sparing    = 0;
      spareCtlReg.Bits.adddc_sparing   = 1;
      spareCtlReg.Bits.region_size     = ADDDC_REGION_SIZE_BANK;
      spareCtlReg.Bits.virtual_lockstep_en = 0;

      if( mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].ReverseRegion) {
        spareCtlReg.Bits.reverse = 1;
      } else {
        spareCtlReg.Bits.reverse = 0;
      }

      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

      spareCtlSrc.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG);
      spareCtlSrc.Bits.source_cs = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryRank;
      spareCtlSrc.Bits.source_c  = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimarySubRank;
      spareCtlSrc.Bits.source_ba = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryBank;
      spareCtlSrc.Bits.source_bg = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryBankGrp;
      spareCtlSrc.Bits.buddy_cs_en = 0;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG, spareCtlSrc.Data);

      spareCtlDest.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG);
      spareCtlDest.Bits.destination_cs = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryRank;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG, spareCtlDest.Data);

      McmtrMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG);
      McmtrMain.Bits.adddc_mode = 1;
      McmtrMain.Bits.ch23cmd_ctl_delay = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG, McmtrMain.Data);

      ModeMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MODE_M2MEM_MAIN_REG);
      ModeMain.Bits.adddc = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MODE_M2MEM_MAIN_REG, ModeMain.Data);

      AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AdddcCtrlRegionoffset);
      AdddcCtrRegion.Bits.region_enable    = 1;
      AdddcCtrRegion.Bits.region_size      = ADDDC_REGION_SIZE_BANK;
      AdddcCtrRegion.Bits.failed_cs        = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryRank;
      AdddcCtrRegion.Bits.failed_c         = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimarySubRank;
      AdddcCtrRegion.Bits.failed_ba        = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryBank;
      AdddcCtrRegion.Bits.failed_bg        = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryBankGrp;
      AdddcCtrRegion.Bits.nonfailed_cs     = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].BuddyRank;
      AdddcCtrRegion.Bits.nonfailed_c      = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].BuddySubRank;
      AdddcCtrRegion.Bits.nonfailed_ba     = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].BuddyBank;
      AdddcCtrRegion.Bits.nonfailed_bg     = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].BuddyBankGrp;
      AdddcCtrRegion.Bits.copy_in_progress = 1;
      mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, AdddcCtrlRegionoffset, AdddcCtrRegion.Data);

      AdddcRegion.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AdddcRegionoffset);
      AdddcRegion.Bits.faildevice          = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].FailedDevice;
      mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, AdddcRegionoffset, AdddcRegion.Data);

      break;

    case RankAdddcSparingMR:

      /*5370517: CLONE SKX Sighting: [ADDDC/SVL] 2B writes and write merge causes UC errors*/

      if(mMemRas->pSysHost->var.common.stepping < H0_REV_SKX) {
        TCOTHP.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, TCOTHP_MCDDC_CTL_REG);
        TCOTHP2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, TCOTHP2_MCDDC_CTL_REG);
        TCDBP.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, TCDBP_MCDDC_CTL_REG);
        McWdbChknBit.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, MCWDB_CHKN_BIT_MCDDC_DP_REG);

        // Update VMSE Error Latency
        VMSEErrorData.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, VMSE_ERROR_MCDDC_DP_REG);
        VmseErrorLatency = (UINT8)TCDBP.Bits.t_cwl + (UINT8)TCOTHP.Bits.t_cwl_adj - (UINT8)TCOTHP2.Bits.t_cwl_adj_neg - 6;
        if (McWdbChknBit.Bits.dis_ddrio_earlywdata == 0) {
          VmseErrorLatency = VmseErrorLatency - 1;
        }

        /*5371638: CLONE SKX Sighting: SVL failing on supercollider w/CECC */
        if ((mMemRas->pSysHost->var.common.stepping == B0_REV_SKX) || (mMemRas->pSysHost->var.common.stepping == B1_REV_SKX) || (mMemRas->pSysHost->var.common.stepping == L0_REV_SKX)) {
          McSchedChknBit2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, MCSCHED_CHKN_BIT2_MCDDC_CTL_REG);
          VmseErrorLatency = VmseErrorLatency + (UINT8)McSchedChknBit2.Bits.x8_wpq_dly - 4;
        }

        VMSEErrorData.Bits.vmse_err_latency = VmseErrorLatency;
        mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, VMSE_ERROR_MCDDC_DP_REG, VMSEErrorData.Data);

        // Disable Error Flow
        DDR4CACtl.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, DDR4_CA_CTL_MCDDC_DP_REG);
        DDR4CACtl.Bits.erf_en0 = 0;
        mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, DDR4_CA_CTL_MCDDC_DP_REG, DDR4CACtl.Data);
      }

      if(mSystemMemoryMap->RasModesEnabled & FULL_MIRROR_1LM) {
        iMcSparingCtrl.Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
        iMcSparingCtrl.Bits.mirr_adddc_en = 1;
        mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, iMcSparingCtrl.Data);
      }

      if(mSystemMemoryMap->RasModesEnabled & PARTIAL_MIRROR_1LM) {
        iMcSparingCtrl.Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
        iMcSparingCtrl.Bits.mirr_adddc_en = 1;
        iMcSparingCtrl.Bits.partial_mirr_en = 1;
        mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, iMcSparingCtrl.Data);
      }

      //Indicate the presence of VLS with the variable
      UpdateVLSVariable(TRUE);

      AdddcCtrlRegionoffset = (ADDDC_REGION0_CONTROL_MC_MAIN_REG + RegionNum*4);
      AdddcRegionoffset     = (ADDDC_REGION0_MCDDC_DP_REG+RegionNum);
      spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
      spareCtlReg.Bits.channel_select  = Ch;
      spareCtlReg.Bits.rank_sparing    = 0;
      spareCtlReg.Bits.sddc_sparing    = 0;
      spareCtlReg.Bits.adddc_sparing   = 1;

      spareCtlReg.Bits.region_size     = ADDDC_REGION_SIZE_RANK;
      spareCtlReg.Bits.virtual_lockstep_en = 0;

      if( mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].ReverseRegion) {
        spareCtlReg.Bits.reverse = 1;
      } else {
        spareCtlReg.Bits.reverse = 0;
      }

      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

      spareCtlSrc.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG);
      spareCtlSrc.Bits.source_cs = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryRank;
      spareCtlSrc.Bits.buddy_cs_en = 0;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_SOURCE_MC_MAIN_REG, spareCtlSrc.Data);

      spareCtlDest.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG);
      spareCtlDest.Bits.destination_cs = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryRank;

      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_DESTINATION_MC_MAIN_REG, spareCtlDest.Data);

      McmtrMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG);
      McmtrMain.Bits.adddc_mode = 1;
      McmtrMain.Bits.ch23cmd_ctl_delay = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG, McmtrMain.Data);

      ModeMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MODE_M2MEM_MAIN_REG);
      ModeMain.Bits.adddc = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MODE_M2MEM_MAIN_REG, ModeMain.Data);

      AdddcCtrRegion.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AdddcCtrlRegionoffset);
      AdddcCtrRegion.Bits.region_enable    = 1;
      AdddcCtrRegion.Bits.region_size      = ADDDC_REGION_SIZE_RANK;
      AdddcCtrRegion.Bits.failed_cs        = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimaryRank;
      AdddcCtrRegion.Bits.failed_c         = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].PrimarySubRank;
      AdddcCtrRegion.Bits.nonfailed_cs     = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].BuddyRank;
      AdddcCtrRegion.Bits.nonfailed_c      = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].BuddySubRank;
      AdddcCtrRegion.Bits.copy_in_progress = 1;
      mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, AdddcCtrlRegionoffset, AdddcCtrRegion.Data);

      AdddcRegion.Data = (UINT8)mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, AdddcRegionoffset);
      AdddcRegion.Bits.faildevice          = mAdddcSparingEventStruct[NodeId][Ch].Region[RegionNum].FailedDevice;
      mCpuCsrAccess->WriteCpuCsr(Socket, SktCh, AdddcRegionoffset, AdddcRegion.Data);

      break;

    case SddcPlusOne:

      //Indicate the presence of VLS with the variable
      UpdateVLSVariable(TRUE);

      /*5331336 Cloned From SKX Si Bug Eco: CLONE SKX Sighting: ADDDC +1 Bank Spare
       * causes SDC and UC/CECC when the failed CS of +1 Spare is not 0*/
      t_ccd_wr = t_ccd_wr+1;

      iMcSparingCtrl.Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
      iMcSparingCtrl.Bits.mirr_adddc_en = 0;
      iMcSparingCtrl.Bits.partial_mirr_en = 0;
      mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, iMcSparingCtrl.Data);

      // all register programming is done for SDDCPlus1 when we come here       
      break;

    default:
      status = EFI_UNSUPPORTED;   // Invalid sparing mode
      break;
  }
    spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);

    //calculate min_tad_rule/max_tad_rule in SPARING_CONTROL to take care of all cases for 3 channel complexity
    // Skipping it in Partial Mirroring case, as it is handled during boot time
    if(mSystemMemoryMap->MemSetup.volMemMode == VOL_MEM_MODE_1LM) {
      if((mSystemMemoryMap->RasModesEnabled & PARTIAL_MIRROR_1LM) != PARTIAL_MIRROR_1LM){
        DEBUG((DEBUG_INFO,"   Updating Min and Max Tad rule in sparing contorl Reg\n"));
        spareCtlReg.Bits.minimum_tad_rule = 0x0;
        for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {
          if (!TAD[TadIndex].Enable) break; // end of TAD Table
        }
        spareCtlReg.Bits.maximum_tad_rule = TadIndex-1;
      }
    }
    else {

      /* Multiple FM regions will map into same NM region. We have to choose only one of
       * these ranges for the spare address range Else, there could be data corruption.
       * To do this always select min TAD as 1 and Max TAD as Base Address TAD Rule 1 + NM Size
       */
      mcNMCachingCfg2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, MCNMCACHINGCFG2_MC_2LM_REG);
      ChNMSize = mcNMCachingCfg2.Bits.mcnmcachingnmchncap;

      spareCtlReg.Bits.minimum_tad_rule = TAD_RULE_ONE;
      mcTADBase.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, TADBase[TAD_RULE_ONE]);
      BaseAddress = mcTADBase.Bits.base;
      MaxTadAddress = BaseAddress + ChNMSize;

      for (TadIndex = 1; TadIndex < TAD_RULES; TadIndex++) {
        if (MaxTadAddress <= TAD[TadIndex].Limit) {
          spareCtlReg.Bits.maximum_tad_rule = TadIndex;
          break;
        }
      }

    }

    //4929254: Cloned From SKX Si Bug Eco: 2LM X Sparing : during init quiesce flow fill is incorrectly getting blocked from allocation into Egress
    M2mDefeatures1.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, DEFEATURES1_M2MEM_MAIN_REG);
    M2mDefeatures1.Bits.egreco |= 0x04;
    mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, DEFEATURES1_M2MEM_MAIN_REG, M2mDefeatures1.Data);

    //5330286: Cloned From SKX Si Bug Eco: Refile 306354: SDDC+1/ADDDC+1 sparing is not using SysAddress comparison for incoming M2M requests
    /*
     * FastDiv3 Mode is always enabled when the system is in 2LM mode and we have 3 Ch populated.
     */

    if (mSystemMemoryMap->MemSetup.volMemMode == VOL_MEM_MODE_2LM) {

      FastDivThree  = TRUE;
      for(ChIndex = 0; ChIndex< MAX_MC_CH; ChIndex++) {
        if( !( ( channelList[(Mc*MAX_MC_CH)+ChIndex].volSize ) ) ) {
            FastDivThree = FALSE ;
          break;
        }
      }
    }

    if(FastDivThree) {
      FastDivThree      =  FALSE;
      mcNMCachingCfg2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, SktCh, MCNMCACHINGCFG2_MC_2LM_REG);
      ChNMSize = mcNMCachingCfg2.Bits.mcnmcachingnmchncap;
      ChNMSizeLog2      = Log2x32(ChNMSize) ;
      mcTADBase.Data    = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, TADBase[TAD_RULE_ONE]);
      SparingStartAddr  = mcTADBase.Bits.base;

      //If memory size is even power of 2
      if (ChNMSizeLog2 & BIT0){
            Sparing2LMCtrl.Data   = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_CONTROL_MC_MAIN_REG);
            Sparing2LMCtrl.Bits.enable = 1;
            Sparing2LMCtrl.Bits.nm_size = ChNMSize;
            mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_CONTROL_MC_MAIN_REG, Sparing2LMCtrl.Data);

            Sparing2LMAddr0Hi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0HI_MC_MAIN_REG);
            Sparing2LMAddr0Lo.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0LO_MC_MAIN_REG);
            Sparing2LMAddr0Hi.Bits.curr_addr = SparingStartAddr;
            Sparing2LMAddr0Lo.Bits.curr_addr = SparingStartAddr;
            mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0HI_MC_MAIN_REG, Sparing2LMAddr0Hi.Data);
            mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0LO_MC_MAIN_REG, Sparing2LMAddr0Lo.Data);

            Sparing2LMAddr1Hi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1HI_MC_MAIN_REG);
            Sparing2LMAddr1Lo.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1LO_MC_MAIN_REG);
            Sparing2LMAddr1Hi.Bits.curr_addr = SparingStartAddr + 2*(ChNMSize) + 1;
            Sparing2LMAddr1Lo.Bits.curr_addr = SparingStartAddr + 2*(ChNMSize) + 1;
            mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1HI_MC_MAIN_REG, Sparing2LMAddr1Hi.Data);
            mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1LO_MC_MAIN_REG, Sparing2LMAddr1Lo.Data);

            Sparing2LMAddr2Hi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2HI_MC_MAIN_REG);
            Sparing2LMAddr2Lo.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2LO_MC_MAIN_REG);
            Sparing2LMAddr2Hi.Bits.curr_addr = SparingStartAddr + ChNMSize + 2;
            Sparing2LMAddr2Lo.Bits.curr_addr = SparingStartAddr + ChNMSize + 2;
            mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2HI_MC_MAIN_REG, Sparing2LMAddr2Hi.Data);
            mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2LO_MC_MAIN_REG, Sparing2LMAddr2Lo.Data);
      } else {
          Sparing2LMCtrl.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_CONTROL_MC_MAIN_REG);
          Sparing2LMCtrl.Bits.enable = 1;
          Sparing2LMCtrl.Bits.nm_size = ChNMSize;
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_CONTROL_MC_MAIN_REG, Sparing2LMCtrl.Data);

          Sparing2LMAddr0Hi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0HI_MC_MAIN_REG);
          Sparing2LMAddr0Lo.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0LO_MC_MAIN_REG);
          Sparing2LMAddr0Hi.Bits.curr_addr = SparingStartAddr;
          Sparing2LMAddr0Lo.Bits.curr_addr = SparingStartAddr;
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0HI_MC_MAIN_REG, Sparing2LMAddr0Hi.Data);
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR0LO_MC_MAIN_REG, Sparing2LMAddr0Lo.Data);

          Sparing2LMAddr1Hi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1HI_MC_MAIN_REG);
          Sparing2LMAddr1Lo.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1LO_MC_MAIN_REG);
          Sparing2LMAddr1Hi.Bits.curr_addr = SparingStartAddr + ChNMSize + 1;
          Sparing2LMAddr1Lo.Bits.curr_addr = SparingStartAddr + ChNMSize + 1;
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1HI_MC_MAIN_REG, Sparing2LMAddr1Hi.Data);
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR1LO_MC_MAIN_REG, Sparing2LMAddr1Lo.Data);

          Sparing2LMAddr2Hi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2HI_MC_MAIN_REG);
          Sparing2LMAddr2Lo.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2LO_MC_MAIN_REG);
          Sparing2LMAddr2Hi.Bits.curr_addr = SparingStartAddr + ChNMSize + 2;
          Sparing2LMAddr2Lo.Bits.curr_addr = SparingStartAddr + ChNMSize + 2;
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2HI_MC_MAIN_REG, Sparing2LMAddr2Hi.Data);
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_2LM_ADDR2LO_MC_MAIN_REG, Sparing2LMAddr2Lo.Data);
      }
    }
    //End : 5330286


    //Set Spare Enabled bit inside Quiesce for ADDDC and SDDC sparing flows
    if(SparingMode==RankSparing) {
      spareCtlReg.Bits.spare_enable = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);
    } else {

      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SPARING_CONTROL_MC_MAIN_REG, spareCtlReg.Data);

      /*5330799: Cloned From SKX Si Bug Eco: egress bypass with quiesce flow
      (pfmiss or force) cause CR chunk to be sent out twice onto mesh*/

      M2mDefeatures0.Data    = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, DEFEATURES0_M2MEM_MAIN_REG);
      M2mDefeatures0.Bits.egrbypdis = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, DEFEATURES0_M2MEM_MAIN_REG, M2mDefeatures0.Data);

      //Quiesce Pcode Interface will also set spare_enable bit to initiate Sparing
      DisplayADDDDCRegisterValues (Socket, Mc, Ch,RegionNum);
      M2mDefeatures0.Data    = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, DEFEATURES0_M2MEM_MAIN_REG);
      RASDEBUG((DEBUG_INFO,"M2mDefeatures0.Bits.egrbypdis:%d\n",M2mDefeatures0.Bits.egrbypdis));

      ModeMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MODE_M2MEM_MAIN_REG);
      RASDEBUG((DEBUG_INFO,"ModeMain.Bits.adddc:%d\n",ModeMain.Bits.adddc));

      McmtrMain.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMTR_MC_MAIN_REG);
      RASDEBUG((DEBUG_INFO,"McmtrMain.Bits.adddc_mode:%d\n",McmtrMain.Bits.adddc_mode));

      MailboxData = MAILBOX_BIOS_ADDDC_QUIESCE | (Mc << 0x8) | (Ch << 0x9) | (t_ccd_wr << 11);

      M2MQuiescePCodeInterface(Socket, MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE, MailboxData);
    }

    return status;
}

/**

  Checks the hardware registers for spare copy completion and returns
  status

  @param NodeId    - Memory controller ID
  @param SparingMode - Type of sparing - DDDC / Rank

  @retval EFI_SUCCESS if spare copy operation completed without errors
  @retval EFI_DEVICE_ERROR if there is an error in spare copy operation
  @retval EFI_NOT_READY if copy operation has not completed

**/
EFI_STATUS
IsCopyComplete(
    IN UINT8 Socket,
    IN UINT8 Mc
 )
{
  EFI_STATUS                                  status = EFI_SUCCESS;
  SPARING_CONTROL_MC_MAIN_STRUCT              spareCtlReg;
  SPARING_PATROL_STATUS_MC_MAIN_STRUCT        ssrStatusReg;
  //ADDDC_REGION0_CONTROL_MC_MAIN_STRUCT		  AdddcCtrRegion;
  DEFEATURES1_M2MEM_MAIN_STRUCT               M2mDefeatures1;

    spareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr( Socket, Mc, SPARING_CONTROL_MC_MAIN_REG);
    ssrStatusReg.Data = mCpuCsrAccess->ReadMcCpuCsr( Socket, Mc, SPARING_PATROL_STATUS_MC_MAIN_REG);
    if( ssrStatusReg.Bits.copy_in_progress == 0 && ssrStatusReg.Bits.copy_complete == 1 && spareCtlReg.Bits.spare_enable == 0) {

      //
      // 4929254 Cloned From SKX Si Bug Eco: 2LM X Sparing : during init quisce flow Fill is incorrectly getting blocked from allocation into Egress
      //
      M2mDefeatures1.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, DEFEATURES1_M2MEM_MAIN_REG);
      M2mDefeatures1.Bits.egreco |= 0x00;
      mCpuCsrAccess->WriteMcCpuCsr( Socket, Mc, DEFEATURES1_M2MEM_MAIN_REG, M2mDefeatures1.Data);

      status = EFI_SUCCESS;
    } else if( spareCtlReg.Bits.spare_enable == 0) {
      //
      // Check for errors
      //
      status = EFI_DEVICE_ERROR;  // Spare copy operation is done, but there are errors
    }
    else {
      status = EFI_NOT_READY;     // spare copy operation still in progress
    }
  return status;
}


