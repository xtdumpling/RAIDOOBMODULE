/**++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2016 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file WheaErrorInj.c

    This is an implementation of the Core Whea error injection method.

---------------------------------------------------------------------------**/

#include "WheaErrorInj.h"
#pragma optimize( "", off)

// GLobal variables and protocols
BOOLEAN                                       mAcpi5Support = FALSE;
BOOLEAN                                       mPcieErrInjActionTable = FALSE;
BOOLEAN                                       mMESegInjEn = FALSE;
BOOLEAN                                       mMESegIcvErrEn = FALSE;
UINT8                                         mErrorExpected = WHEA_NO_CONSUME; // Memory Error consume
UINT8                                         *aMemoryLlcErrInjAddr;
UINT32                                        mMESegInjAddr;
UINT64                                        mUmaErrorInjectionType;
UINT64                                        mBiosRsvdInjAddr;
EFI_SMM_BASE2_PROTOCOL                        *mSmmBase = NULL;
static EFI_SMM_CPU_SERVICE_PROTOCOL                  *mSmmCpuServiceProtocol; // 10770
EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL     *mSmmPeriodicTimerDispatch = NULL;
EFI_HANDLE                                    mPeriodicTimerHandle = NULL;
EFI_SMM_SYSTEM_TABLE2                         *mSmst = NULL;
WHEA_EINJ_PARAM_BUFFER                        *mEinjParam = NULL;
STATIC  VOID                                  *mEinjEsts = NULL;
WHEA_EINJ_TRIGGER_ACTION_TABLE                *mEinjAction = NULL;
EFI_MEM_RAS_PROTOCOL                          *mMemRas = NULL;
EFI_CPU_CSR_ACCESS_PROTOCOL                   *mCpuCsrAccess;
static EFI_CRYSTAL_RIDGE_PROTOCOL                    *mCrystalRidgeProtocol;
UINT32                                        mInjAddrValue;
SPIN_LOCK                                     mApLock;
EFI_PLATFORM_RAS_POLICY_PROTOCOL              *mPlatformRasPolicyProtocol;
EFI_RAS_SYSTEM_TOPOLOGY                       *mRasTopology = NULL;
SYSTEM_RAS_SETUP                              *mRasSetup = NULL;
SYSTEM_RAS_CAPABILITY                         *mRasCapability = NULL;
extern IIO_UDS                                *mIioUdsLib;
extern UINT64                                 EinjIioErrInjLockSwSmi;
BOOLEAN                                       SysDirectoryModeChkForInj = FALSE;

//
// WHEA Serialization Table
//
STATIC
EFI_ACPI_WHEA_SERIALIZATION_ACTION mSimEinj[9] = {
  //
  // Serialization Action Table
  //
  {   //Action0
    WHEA_EINJ_BEGIN_INJECT_OP,          // BEGIN_INJECTION_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    EINJ_BEGIN_OPERATION,               // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits
  },
  {   //Action1
    WHEA_EINJ_GET_TRIGGER_ACTION_TABLE, // GET_TRIGGER_ERROR_STRUC
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x40, 0x00, 0x04, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffffffffffff                  // Mask is only valid for 32-bits
  },
  {   //Action2
    WHEA_EINJ_SET_ERROR_TYPE,           // SET_ERROR_TYPE
    INSTRUCTION_WRITE_REGISTER,         // WRITE_REGISTER
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits
  },
  {   //Action3
    WHEA_EINJ_GET_ERROR_TYPE,           // GET_ERROR_TYPE
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 //{0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits
  },
  {   //Action4
    WHEA_EINJ_END_INJECT_OP,            // END_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 //{0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    EINJ_END_OPERATION,                 // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits
  },
  {   //Action5
    WHEA_EINJ_EXECUTE_INJECT_OP,        // EXECUTE_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterExecute,  // GAS (WORD IO). Address will be filled in runtime
    EFI_WHEA_EINJ_EXECUTE_SWSMI,          // Value for InjectError()
    0xffff                                // Mask is only valid for 16-bits
  },
  {   //Action6
    WHEA_EINJ_CHECK_BUSY_STATUS,        // CHECK_BUSY_STATUS
    INSTRUCTION_READ_REGISTER_VALUE,    // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x30, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0x00000001,                         // Value for InjectError()
    0x00000001                          // Mask is only valid for 32-bits
  },
  {   //Action7
    WHEA_EINJ_GET_CMD_STATUS,           // GET_OPERATION_STATUS
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x40, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0x000001fe                          // Mask is only valid for 32-bits
  },
  {   //Action8
    WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRSS,  // SET_ERROR_TYPE_WITH_ADDRESS
    INSTRUCTION_WRITE_REGISTER,         // WRITE_REGISTER
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits
  },
};

STATIC
WHEA_EINJ_TRIGGER_ACTION_TABLE mSimEinjAction = \
{
  {sizeof (WHEA_ERROR_TRIGGER_ACTION_HEADER), 0, sizeof (WHEA_EINJ_TRIGGER_ACTION_TABLE), 4},
  { //Action0
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffffffffffff                // Mask is only valid for 32-bits
  },
  { //Action1
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffff                        // Mask is only valid for 32-bits
  },
  { //Action2
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffff                        // Mask is only valid for 32-bits
  },
  { //Action3
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,         // Write register
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffff                        // Mask is only valid for 32-bits
  }
};

/**

    As part of LLC error injection. BIOS needs to disable cache prefetch. (DCU IP, DCU Streamer, MLC spatial, MLC streamer).
    Also, start a PSMI event to start quiesce handshake.
    Quiesce is needed to ensure that DMA/DCA accesses do not interfere with the error injection.
    For more information see MCA HAS LLC Error injection and RAS HAS for Quiesce flow.

    @param node - Node Id

    @retval None

**/
void
DisablePsmiAndPrefetch (
    UINT8 *node
)
{

  DisablePsmi();
  DisablePrefetch();

}

VOID
SeeErrorAp(
  UINT8 *Skt)
/**

    Write Data to error injection address to seed the error in memory.

    @param Skt - Socket Id

    @retval None

**/
{
  UINT64  PhyAddr=0;

  // PhyAddr contains the error injection Address
  PhyAddr = mEinjParam->EinjAddr;

  if (mMESegInjEn) {
    PhyAddr = mMESegInjAddr;
  }
  // Write data to memory to seed error in memeory
  AsmCommitLine((VOID*)PhyAddr,mInjAddrValue);
  AsmWbinvd();
  ReleaseSpinLock(&mApLock);
}


/**

    Disable Corrected machine check interrupt(CMCI) by writting to ERROR_COMTROL MSR.
    When disabled MCG_CAP[10] will read 0 indicating that CMCI is not supported in the processor.

    @param node - Node Id mapping to every memory controller in the system.

    @retval None

**/
void
DisableCMCI(
  UINT8 *node
)
{
  UINT64      Data;
  Data = AsmReadMsr64(MC_PS_INT_DOWNGRADE);

}

/**

    Enable Corrected machine check interrupt(CMCI) by writting to ERROR_COMTROL MSR.
    When enabled MCG_CAP[10] will read 1 indicating that CMCI is supported in the processor.

    @param node - Node Id mapping to every memory controller in the system.

    @retval None

**/
void
ClearDisableCMCI(
  UINT8 *node
)
{
  UINT64      Data;

  Data = AsmReadMsr64(MC_PS_INT_DOWNGRADE);
  Data &= (~BIT4);
  AsmWriteMsr64 (MC_PS_INT_DOWNGRADE, Data);

}

void
InjectAepError (
    UINT8    socket,
    UINT8    Ch,
    UINT8    Dimm,
    UINT64   DPA
    )
{
  EFI_STATUS    Status;
  UINT8         Enable;
  UINT32    EinjCsr;

  Status =  mCrystalRidgeProtocol->NvmCtlrEnableErrInjection(socket,Ch,Dimm);
  ASSERT_EFI_ERROR(Status);

  //call Inject Error with Enable = 0 for clearing errors
  /*Enable = 0;
  Status = mCrystalRidgeProtocol->NvmCtlrInjectPoisonError(socket, Ch, Dimm, DPA, Enable, &EinjCsr);
  ASSERT_EFI_ERROR(Status);*/

  //call Inject Error with Enable = 1 for Injecting an error
  Enable = 1;
  Status = mCrystalRidgeProtocol->NvmCtlrInjectPoisonError(socket, Ch, Dimm, DPA, Enable, &EinjCsr);
  if(EFI_ERROR(Status)) {
    mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
    return;
  }

  //Injected Poison errors are only triggered on a subsequent read of the poisoned address

  mEinjAction->Trigger0.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
  mEinjAction->Trigger0.Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
  mEinjAction->Trigger0.Flags                      = FLAG_NOTHING;
  mEinjAction->Trigger0.Reserved8                  = 00;
  mEinjAction->Trigger0.Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_MEMORY;
  mEinjAction->Trigger0.Register.RegisterBitWidth  = 0x20;
  mEinjAction->Trigger0.Register.RegisterBitOffset = 0x00;
  mEinjAction->Trigger0.Register.AccessSize        = EFI_ACPI_3_0_DWORD;
  mEinjAction->Trigger0.Register.Address           = DPA;
  mEinjAction->Trigger0.Value                      = mInjAddrValue;
  mEinjAction->Trigger0.Mask                       = 0xffffffff;

  return;
}

/**

    Inject Memory Related errors. i.e. Patrol Scrub, Corrected or UC errors.

    @param ErrorToInject - BIT3 - Memory Correctable (ECC correctable)
                                 - BIT4  - Memory UC non-fatal (i.e. Patrol Scrub)
                                 - BIT5 - Memory UC Fatal - (ECC uncorrectable error)
                                 - BIT12 - Memory uncorrectable error (Core consumption IFU/DCU errors, software is expected to consume the line to trigger errors)

    @retval None

**/
VOID
InjectMemoryError (
 UINT32   ErrorToInject
)
{
  UINT8         node=0;
  UINT8         SocketId, SktIndex;
  UINT8         MemoryControllerId;
  UINT64        PhyAddr, TempAddr;
  UINT8         TadMode;
  UINT8		      SktCh;
  UINT16        thread;
  UINT32        PhyRank;
  UINT32        SocketPresentBitMap;
  TRANSLATED_ADDRESS TA;

  RSP_FUNC_CRC_ERR_INJ_EXTRA_MCDDC_DP_STRUCT    EnjExtra;
  RSP_FUNC_ADDR_MATCH_LO_MC_MAIN_STRUCT         MatchLoReg;
  RSP_FUNC_ADDR_MATCH_HI_MC_MAIN_STRUCT         MatchHiReg;
  RSP_FUNC_ADDR_MASK_LO_MC_MAIN_STRUCT          MaskLoReg;
  RSP_FUNC_ADDR_MASK_HI_MC_MAIN_STRUCT          MaskHiReg;
  SCRUBADDRESSHI_MC_MAIN_STRUCT                 ScrubAddrHi;
  SCRUBADDRESSLO_MC_MAIN_STRUCT                 ScrubAddrLo;
  SCRUBADDRESS2LO_MC_MAIN_STRUCT                ScrubAddr2Lo;
  SCRUBADDRESS2HI_MC_MAIN_STRUCT                ScrubAddr2Hi;
  SCRUBCTL_MC_MAIN_STRUCT                       ScrubCtl;
  EFI_STATUS                                    Status = EFI_SUCCESS;
  BOOLEAN                                       FoundThread = FALSE; //10770
  EFI_PROCESSOR_INFORMATION                     ProcInfo;   //10770
  UINT32                                        Read32=0;
  MCI_STATUS_SHADOW_N0_M2MEM_MAIN_STRUCT        Mc5Shadow0Status;
  MCI_STATUS_SHADOW_N1_M2MEM_MAIN_STRUCT        Mc5Shadow1Status;
  struct SystemMemoryMapHob                     *SystemMemoryMap = NULL;
  UINT8		 				TadIndex;
  struct  				TADTable *TAD;

  TempAddr = 0;
  ZeroMem (&TA, sizeof(TRANSLATED_ADDRESS));
  if (mMemRas == NULL)
	return;

  SystemMemoryMap = mMemRas->SystemMemInfo;

  //Step 1.  Unlock error injection mechanism on socket basis
  // Scope is package level. No need to do on each thread.

  for (thread = 0; thread < mSmst->NumberOfCpus; thread++) {
    mSmst->SmmStartupThisAp (UnLockInjLogic,
                  thread,
                  &node);
  }
  UnLockInjLogic(&node);

  PhyAddr = mEinjParam->EinjAddr;
  RASDEBUG ((DEBUG_ERROR, " InjectMemoryError - PhyAddr = 0x%lx\n",(UINT64)PhyAddr));

  if (mMESegInjEn) {
    PhyAddr = mMESegInjAddr;
    RASDEBUG ((DEBUG_ERROR, " Inject UMA Error - PhyAddr = 0x%lx\n",(UINT64)PhyAddr));   
    if (PhyAddr == ((UINT64)-1)) {
      mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
	    return;
    }

    //
    // Need to do pre-setup before injecting error in ME segment
    // set MC9[Bit5] = 0 and MESEG_LIMIT.EN = 0
    //
    PreMESegErrInjSetup ();


    //
    // If the error is ICV type, we just need to corrupt the memory location, do the post ME injection setup
    // and prepare the triggr action table
    //
    if (mMESegIcvErrEn) {
      CorruptMESegInjOffset ();

      //
      // Need to do post-setup after memory corruption/error injection in ME seg
      // set MESEG_LIMIT.EN = 1 and MC9[Bit5] = 1
      //
      PostMESegErrInjSetup ();

      //
      // Setup the exclusive Trigger action table for ME
      //
      SetupMESegTriggerActionTable ();
      return;
    }
  }


  if(mMemRas == NULL){
    mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
    return;
  }

  //
  // Translate physical address to DIMM Address
  //
  Status = mMemRas->SystemAddressToDimmAddress (PhyAddr,&TA);
  if(EFI_ERROR(Status))   {
    mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
    RASDEBUG ((DEBUG_ERROR, " InjectMemoryError - Address tarnslation error \n"));
    return;
  }

  if (TA.DimmType == ddrtdimmType) {
      InjectAepError(TA.SocketId,TA.ChannelId,TA.DimmSlot,TA.DPA);
      return;
    }

  for (node = 0; node < MC_MAX_NODE; node++) {
    if (mMemRas->SystemMemInfo->Socket[NODE_TO_SKT(node)].imcEnabled[NODE_TO_MC(node)] == 0 ) continue;

    // Clear the Mc5 status shadow to track the error incase mcbank status gets cleared.
    MemoryControllerId  = 00;
    Mc5Shadow1Status.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG);
    Mc5Shadow0Status.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG);
    mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG, 00);
    mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG, 00);
    MemoryControllerId  = 01;
    Mc5Shadow1Status.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG);
    Mc5Shadow0Status.Data = mCpuCsrAccess->ReadMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG);
    mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG, 00);
    mCpuCsrAccess->WriteMcCpuCsr(NODE_TO_SKT(node),MemoryControllerId,MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG, 00);

  }


  SocketId = TA.SocketId;
  MemoryControllerId = TA.MemoryControllerId;
  PhyRank = (TA.DimmSlot << 2) + TA.DimmRank;
  node   = SKTMC_TO_NODE(SocketId, MemoryControllerId);
	SktCh	 = NODECH_TO_SKTCH(node, TA.ChannelId);

  // Disable Patrol scrub
  mMemRas->DisablePatrolScrubEngine(node);
  RASDEBUG ((DEBUG_ERROR, " InjectMemoryError -Addr = %lx, Node = %x, Ch = %x , PhyRank = %x, \n", PhyAddr, node, TA.ChannelId, TA.PhysicalRankId));

  ScrubCtl.Data = 0;

  // Invalidate the line in memory
  mInjAddrValue = *(volatile  UINT32 *)(UINTN)(PhyAddr);
  *(volatile  UINT32 *)(UINTN)(PhyAddr) = 0x12345678;
  AsmFlushCacheLine ((VOID*)PhyAddr);

  if( mErrorExpected == WHEA_REC_PATROLSCRUB_ERROR ) {

    SocketPresentBitMap  = mIioUdsLib->SystemStatus.socketPresentBitMap;

    ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG);

    //
    // If the patrol scrub mode is SPA mode, need to move to legacy mode to setup the registers
    // and then move back to SPA mode. And while in the legacy mode, scrubbing has to be enabled once
    // for which Pkg C states should be disbled which is done through RAS_START command and reenabled
    // again using the RAS_EXIT mailbox commands
    //

    if (ScrubAddrHi.Bits.ptl_sa_mode == 1) {
      RASDEBUG((DEBUG_ERROR, "\n\nWe are in SA mode, scrubAddrHi.Data is 0x%x\n",ScrubAddrHi.Data));


      // clearing the scrub ctrl register
      RASDEBUG((EFI_D_LOAD|EFI_D_INFO, "\nClearing the scrubCtl Register\n"));
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG, 00);

      //Switch to Legacy Mode
      ScrubAddrHi.Bits.ptl_sa_mode = 0;
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);
      RASDEBUG((DEBUG_ERROR, "\nSwitched to Legacy mode\n"));

      //Call RAS_START
      RASDEBUG((DEBUG_ERROR, "Do the RAS Start on existing CPUs\n"));
      for (SktIndex = 0; SktIndex < MAX_SOCKET; SktIndex++) {
        if (((SocketPresentBitMap >> SktIndex) & 0x01) == 1) {
          Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(SktIndex, MAILBOX_BIOS_CMD_RAS_START, 0);
        }
      }
    
      ScrubAddrLo.Data  = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSLO_MC_MAIN_REG);
      RASDEBUG((DEBUG_ERROR, "ScrubAddrLo.Data = 0x%x\n", ScrubAddrLo.Data));

      // Doing the legacy scrub
      RASDEBUG((DEBUG_ERROR, "Enabling Scrub_en command\n"));
      ScrubCtl.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG);
      ScrubCtl.Bits.scrub_en = 1;
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG, ScrubCtl.Data);

      AsmNop();
      AsmNop();
      AsmNop();
                  
      ScrubCtl.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG);
      RASDEBUG((DEBUG_ERROR, "ScrubCtl.scrubissued = 0x%x\n", ScrubCtl.Bits.scrubissued));

      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG,00);

      // Call RAS_EXIT
      RASDEBUG((DEBUG_ERROR, "Do the RAS Exit on existing CPUs\n"));
      for (SktIndex = 0; SktIndex < MAX_SOCKET; SktIndex++) {
        if (((SocketPresentBitMap >> SktIndex) & 0x01) == 1) {
          Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(SktIndex, MAILBOX_BIOS_CMD_RAS_EXIT, 0);
        }
      }

      //
      // Programming the ScrubAddrHi Register
      //
      ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG);

      //Set the maximum, minumum and current tad rule
      TAD = &SystemMemoryMap->Socket[SocketId].imc[MemoryControllerId].TAD[0];
      for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {
        if (!TAD[TadIndex].Enable) break; // end of TAD Table
      }

      if(SystemMemoryMap->MemSetup.volMemMode == VOL_MEM_MODE_1LM) {
        ScrubAddrHi.Bits.minimum_tad_rule = 0x0;
        ScrubAddrHi.Bits.maximum_tad_rule = TadIndex - 1;
      }
      else {
        ScrubAddrHi.Bits.minimum_tad_rule = ScrubAddrHi.Bits.maximum_tad_rule = 0x0;
      }

      ScrubAddrHi.Bits.tad_rule  = TA.TadId;
      ScrubAddrHi.Bits.rankaddhi = (UINT32)(PhyAddr >> 38);
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);
      RASDEBUG((DEBUG_ERROR, "ScrubAddrHi.Data is 0x%x\n",ScrubAddrHi.Data));


      // Programming ScrubAddrLo
      ScrubAddrLo.Data  = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSLO_MC_MAIN_REG);
      ScrubAddrLo.Bits.rankadd = (UINT32)(PhyAddr >> 6);
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSLO_MC_MAIN_REG, ScrubAddrLo.Data);
      RASDEBUG((DEBUG_ERROR, "ScrubAddrLo.Data is 0x%x\n",ScrubAddrLo.Data));

      // Program ScrubAddress2Lo/hi register
      TAD = &SystemMemoryMap->Socket[SocketId].imc[MemoryControllerId].TAD[0];
      TadMode = TAD[TA.TadId].mode;

      if (TadMode == 2)  // 64 byte interleaving
        TempAddr = (UINT64)PhyAddr & (UINT64)~(0x3F);
      if (TadMode == 1)  // 256 byte interleaving
        TempAddr = (UINT64)PhyAddr & (UINT64)~(0xFF);

      ScrubAddr2Lo.Data  = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESS2LO_MC_MAIN_REG);
      ScrubAddr2Lo.Bits.baseadd = (UINT32)(TempAddr >> 6);
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESS2LO_MC_MAIN_REG, ScrubAddr2Lo.Data);
      RASDEBUG((DEBUG_ERROR, "ScrubAddr2Lo.Data is 0x%x\n",ScrubAddr2Lo.Data));

      ScrubAddr2Hi.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESS2HI_MC_MAIN_REG);
      ScrubAddr2Hi.Bits.baseaddhi = (UINT32)(PhyAddr >> 38);
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESS2HI_MC_MAIN_REG, ScrubAddr2Hi.Data);
      RASDEBUG((DEBUG_ERROR, "ScrubAddr2Hi.Data is 0x%x\n",ScrubAddr2Hi.Data));

      
       // Switch back to SPA mode
      RASDEBUG((DEBUG_ERROR, "Moving to SPA mode\n"));
      ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG);
      ScrubAddrHi.Bits.ptl_sa_mode = 1;
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);

     
      // Setting the start scrub register in scrubctl register
      ScrubCtl.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG);
      ScrubCtl.Bits.startscrub   = 1;
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG, ScrubCtl.Data);
      RASDEBUG((DEBUG_ERROR, "ScrubCtl.Data is 0x%x\n",ScrubCtl.Data));
     
    } else {

      // The Patrol Scrub mode is legacy mode, just setup the address in the low/high registers

      RASDEBUG((DEBUG_ERROR, "We are in the Legacy Mode.\n"));
      ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG);
      ScrubAddrHi.Bits.rank  = TA.PhysicalRankId;
      ScrubAddrHi.Bits.chnl  = TA.ChannelId;
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);

      ScrubAddrLo.Data  = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSLO_MC_MAIN_REG);
      ScrubAddrLo.Bits.rankadd = (UINT32)TA.RankAddress;
      mCpuCsrAccess->WriteMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSLO_MC_MAIN_REG, ScrubAddrLo.Data);

    }

    // setup ScrubCtl register,  which is written by the OS through Trigger Action Table
    ScrubCtl.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG);
    ScrubCtl.Bits.scrub_en     = 1;
    RASDEBUG((DEBUG_ERROR, "ScrubCtl.Data  to be written by OS 0x%x\n",ScrubCtl.Data));

    ScrubAddrLo.Data = mCpuCsrAccess->ReadMcCpuCsr(SocketId, MemoryControllerId, SCRUBADDRESSLO_MC_MAIN_REG);
    RASDEBUG((DEBUG_ERROR, "ScrubAddrLo.Data is 0x%x\n",ScrubAddrLo.Data));
    
  }

  //
  // Preparing the Error Injection Table
  //
  if (!(mMESegInjEn)) {

    if( mErrorExpected == WHEA_REC_PATROLSCRUB_ERROR ) {
      mEinjAction->Trigger0.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
      mEinjAction->Trigger0.Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
      mEinjAction->Trigger0.Flags                      = FLAG_NOTHING;
      mEinjAction->Trigger0.Reserved8                  = 00;
      mEinjAction->Trigger0.Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_MEMORY;
      mEinjAction->Trigger0.Register.RegisterBitWidth  = 0x20;
      mEinjAction->Trigger0.Register.RegisterBitOffset = 0x00;
      mEinjAction->Trigger0.Register.AccessSize        = EFI_ACPI_3_0_DWORD;
      mEinjAction->Trigger0.Register.Address           = mCpuCsrAccess->GetMcCpuCsrAddress (SocketId, MemoryControllerId, SCRUBCTL_MC_MAIN_REG);
      mEinjAction->Trigger0.Value                      = ScrubCtl.Data;
      mEinjAction->Trigger0.Mask                       = 0xffffffff;
      RASDEBUG((DEBUG_ERROR, "SocketId = 0x%x, MemoryControllerId= 0x%x\n",SocketId, MemoryControllerId));
      RASDEBUG((DEBUG_ERROR, "mEinjAction->Trigger0.Register.Address: 0x%lx\n",mEinjAction->Trigger0.Register.Address));
      RASDEBUG((DEBUG_ERROR, "mEinjAction->Trigger0.Value: 0x%x\n",mEinjAction->Trigger0.Value));
    }
    
    if (ErrorToInject == INJECT_ERROR_MEMORY_UE_FATAL)  {
      mEinjAction->Trigger0.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
      mEinjAction->Trigger0.Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
      mEinjAction->Trigger0.Flags                      = FLAG_NOTHING;
      mEinjAction->Trigger0.Reserved8                  = 00;
      mEinjAction->Trigger0.Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_MEMORY;
      mEinjAction->Trigger0.Register.RegisterBitWidth  = 0x20;
      mEinjAction->Trigger0.Register.RegisterBitOffset = 0x00;
      mEinjAction->Trigger0.Register.AccessSize        = EFI_ACPI_3_0_DWORD;
      mEinjAction->Trigger0.Register.Address           = PhyAddr;
      mEinjAction->Trigger0.Value                      = mInjAddrValue;
      mEinjAction->Trigger0.Mask                       = 0xffffffff;


      mEinjAction->Trigger1.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
      mEinjAction->Trigger1.Instruction                = INSTRUCTION_READ_REGISTER_VALUE;
      mEinjAction->Trigger1.Flags                      = FLAG_NOTHING;
      mEinjAction->Trigger1.Reserved8                  = 00;
      mEinjAction->Trigger1.Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_MEMORY;
      mEinjAction->Trigger1.Register.RegisterBitWidth  = 0x20;
      mEinjAction->Trigger1.Register.RegisterBitOffset = 0x00;
      mEinjAction->Trigger1.Register.AccessSize        = EFI_ACPI_3_0_DWORD;
      mEinjAction->Trigger1.Register.Address           = PhyAddr;
      mEinjAction->Trigger1.Value                      = Read32;
      mEinjAction->Trigger1.Mask                       = 0xffffffff;
    }

    if (mErrorExpected == WHEA_MEM_COR_ERROR)  {
      mEinjAction->Trigger0.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
      mEinjAction->Trigger0.Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
      mEinjAction->Trigger0.Flags                      = FLAG_NOTHING;
      mEinjAction->Trigger0.Reserved8                  = 00;
      mEinjAction->Trigger0.Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_MEMORY;
      mEinjAction->Trigger0.Register.RegisterBitWidth  = 0x20;
      mEinjAction->Trigger0.Register.RegisterBitOffset = 0x00;
      mEinjAction->Trigger0.Register.AccessSize        = EFI_ACPI_3_0_DWORD;
      mEinjAction->Trigger0.Register.Address           = PhyAddr;
      mEinjAction->Trigger0.Value                      = mInjAddrValue;
      mEinjAction->Trigger0.Mask                       = 0xffffffff;
    }
  }

  //
  // Setup Error Injection registers
  //

  mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_CRC_ERR_INJ_DEV0_XOR_MSK_MCDDC_DP_REG, 01);

  EnjExtra.Data = mCpuCsrAccess->ReadCpuCsr (SocketId, SktCh, RSP_FUNC_CRC_ERR_INJ_EXTRA_MCDDC_DP_REG);
  EnjExtra.Bits.crc_err_inj_dev0_5_bits = 01;

  if( (ErrorToInject == INJECT_ERROR_MEMORY_UE_NON_FATAL) || (ErrorToInject == INJECT_ERROR_MEMORY_UE_FATAL)) {
    mInjAddrValue = INJECTION_ADDRESS_VALUE;
    EnjExtra.Bits.crc_err_inj_dev1_5_bits = 05;
    mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_MCDDC_DP_REG,04);
  } else {
    mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_CRC_ERR_INJ_DEV1_XOR_MSK_MCDDC_DP_REG,0);
  }
  mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_CRC_ERR_INJ_EXTRA_MCDDC_DP_REG, EnjExtra.Data);


  MaskHiReg.Data = mCpuCsrAccess->ReadCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MASK_HI_MC_MAIN_REG);
  MaskHiReg.Bits.addr_mask_higher = 00;
  mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MASK_HI_MC_MAIN_REG, MaskHiReg.Data);

  MaskLoReg.Data = mCpuCsrAccess->ReadCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MASK_LO_MC_MAIN_REG);
  MaskLoReg.Bits.addr_mask_lower  = 07;
  mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MASK_LO_MC_MAIN_REG, MaskLoReg.Data);

  // Addr Match Lower: 32-bits (Match Addr[34:3])
  MatchLoReg.Data = mCpuCsrAccess->ReadCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MATCH_LO_MC_MAIN_REG);
  MatchLoReg.Bits.addr_match_lower = (UINT32)(PhyAddr >> 03);
  mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MATCH_LO_MC_MAIN_REG, MatchLoReg.Data);

  // RSP_FUNC_ADDR_MATCH_EN RSP_FUNC_ADDR_MATCH_EN 11:11 Enabling the Address Match Response Function when set.
  // The enable bit is self cleared after match and the lock is driven from the AND output of EPMCMAIN_DFX_LCK_CNTL.RSPLCK (uCR) AND
  // MC_ERR_INJ_LCK.MC_ERR_INJ_LCK (MSR) registers.
  // Bits 10:0 Match Addr[45:35]
  MatchHiReg.Data = mCpuCsrAccess->ReadCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MATCH_HI_MC_MAIN_REG);
  MatchHiReg.Bits.addr_match_higher = PhyAddr >> 0x23;
  MatchHiReg.Bits.rsp_func_addr_match_en  = 01;
  mCpuCsrAccess->WriteCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MATCH_HI_MC_MAIN_REG, MatchHiReg.Data );

  RASDEBUG ((DEBUG_ERROR, " InjectMemoryError Before mem write - RSP_FUNC_ADDR_MATCH_HI.rsp_func_addr_match_en = %x, \n", MatchHiReg.Bits.rsp_func_addr_match_en ));
  InitializeSpinLock(&mApLock);
  // Write data to memory to seed error in memeory
  if(SocketId == 00) {
    SeeErrorAp(&SocketId);
    AsmWbinvd();
  } else {

    FoundThread = FALSE;
    for(thread = 0; thread < mSmst->NumberOfCpus; thread++) {
      if(mSmmCpuServiceProtocol->GetProcessorInfo(mSmmCpuServiceProtocol,thread, &ProcInfo) == EFI_SUCCESS) {
        if((ProcInfo.Location.Package == SocketId) && (ProcInfo.StatusFlag & PROCESSOR_ENABLED_BIT) != 0) {
        FoundThread = TRUE;
        break;
        }
      }
    }
    if(FoundThread) {
      AcquireSpinLock(&mApLock);
      Status = mSmst->SmmStartupThisAp(SeeErrorAp, thread, &SocketId);
      if(Status == EFI_SUCCESS) {
        while ( (AcquireSpinLockOrFail(&mApLock)) != TRUE  ) {
          CpuPause();
        }
      }
      ReleaseSpinLock(&mApLock);
    }
  }

  AsmNop();
  AsmNop();

  MatchHiReg.Data = mCpuCsrAccess->ReadCpuCsr (SocketId, SktCh, RSP_FUNC_ADDR_MATCH_HI_MC_MAIN_REG);
  RASDEBUG ((DEBUG_ERROR, " InjectMemoryError After mem write - RSP_FUNC_ADDR_MATCH_HI.rsp_func_addr_match_en = %x, \n", MatchHiReg.Bits.rsp_func_addr_match_en ));

  if (mMESegInjEn) {

      //
      // Need to do post-setup after error injection in ME seg
      // set MESEG_LIMIT.EN = 1 and MC9[Bit5] = 1
      //
      PostMESegErrInjSetup ();

      //
      // Setup the exclusive Trigger action table for ME 
      //
      SetupMESegTriggerActionTable ();
  }

  // Lock the error injection mechanism
  for (thread = 0; thread < mSmst->NumberOfCpus; thread++) {
    mSmst->SmmStartupThisAp (LockInjLogic,
                             thread,
                             &node);
  }
  LockInjLogic(&node);

  //
  // 5332907: CLONE SKX Sighting: IMC Error Injection Fails ~25% of the Time With Directory Mode Enabled on 2S
  // For this sighting, a Warning message will be displayed to let user know to disable DirectoryMode in BIOS setup
  //
  if (SysDirectoryModeChkForInj) {
    DEBUG ((DEBUG_ERROR, "\n\nWARNING: DIRECTORY MODE IS ENABLED. SOME ERROR INJECTIONS MIGHT NOT WORK!!!"));
    DEBUG ((DEBUG_ERROR, "\nPLEASE DISABLE DIRECTORY MODE IN BIOS SETUP\n\n"));
  }
}

VOID
SetupLlcInjRegisterAp(
  IN      UINT8     *Buffer) //Empty Buffer
{
  SetupLlcInjRegister(mEinjParam);
  ReleaseSpinLock(&mApLock);
}

/**

Routine Description: This function injects LLC error in the cache

    @param None

    @retval none

**/
VOID
InjectLLCError(
    UINT8 SocketId
)
{

  UINT16   thread = 0;
  UINT64  PhyAddr;
  BOOLEAN FoundThread = FALSE;
  EFI_PROCESSOR_INFORMATION ProcInfo;
  EFI_STATUS Status = EFI_SUCCESS;

  mEinjParam->Skt = SocketId;
  // Unlock the error injection logic
  for (thread = 0; thread < mSmst->NumberOfCpus; thread++) {
    mSmst->SmmStartupThisAp (UnLockInjLogic,
                 thread,
                 &SocketId);
  }
  UnLockInjLogic(&SocketId);
  RASDEBUG ((DEBUG_ERROR, "InjectLLCError :  Unlock Completed \n"));
  PhyAddr = mEinjParam->EinjAddr;

  for (thread = 0; thread < mSmst->NumberOfCpus; thread++) {
    mSmst->SmmStartupThisAp (DisablePsmiAndPrefetch,
                 thread,
                 &SocketId);
  }

  // Disable PSMI and Disable Prefetch
  DisablePsmiAndPrefetch(&SocketId);

  RASDEBUG ((DEBUG_ERROR, "InjectLLCError :  EWB Inj Regs setup on Skt %d start : \n", SocketId));

  if(SocketId == 0) {
    RASDEBUG ((DEBUG_INFO,"Setting up Injection register on thread: %d for SocketID 0 \n",thread));
    SetupLlcInjRegister(mEinjParam);
  } else {
    InitializeSpinLock(&mApLock);
    FoundThread = FALSE;
    for(thread = 0; thread < mSmst->NumberOfCpus; thread++) {
      if(mSmmCpuServiceProtocol->GetProcessorInfo(mSmmCpuServiceProtocol,thread, &ProcInfo) == EFI_SUCCESS) {
        if((ProcInfo.Location.Package == SocketId) && (ProcInfo.StatusFlag & PROCESSOR_ENABLED_BIT) != 0) {
          AcquireSpinLock(&mApLock);
          RASDEBUG ((DEBUG_INFO,"Setting up Injection register on thread: %d\n",thread));
          Status = mSmst->SmmStartupThisAp(SetupLlcInjRegisterAp, thread, NULL);
          if(Status == EFI_SUCCESS) {
            while ( (AcquireSpinLockOrFail(&mApLock)) != TRUE  ) {
              CpuPause();
            }
          }
          ReleaseSpinLock(&mApLock);
          break;
        }
      }
    }
  }

  RASDEBUG((DEBUG_INFO,"Err inj bits set \n"));
  mEinjAction->Trigger0.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION;
  mEinjAction->Trigger0.Instruction                = WHEA_INSTRUCTION_CFLUSH;    // CLFLUSH
  mEinjAction->Trigger0.Flags                      = FLAG_NOTHING;
  mEinjAction->Trigger0.Reserved8                  = 00;
  mEinjAction->Trigger0.Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_MEMORY;
  mEinjAction->Trigger0.Register.RegisterBitWidth  = 0x20;
  mEinjAction->Trigger0.Register.RegisterBitOffset = 0x00;
  mEinjAction->Trigger0.Register.AccessSize        = EFI_ACPI_3_0_DWORD;
  mEinjAction->Trigger0.Register.Address           = PhyAddr;
  mEinjAction->Trigger0.Value                      = 0x12345678;
  mEinjAction->Trigger0.Mask                       = 0xffffffff;
}

EFI_STATUS
SetBiosRsvdInjAddress(

)
{

  UINT64      BiosRsvdStartAddr;
  UINT64      BiosRsvdEndAddr;

  BiosRsvdStartAddr  = (UINT64)(UINTN)aMemoryLlcErrInjAddr + 0x100;
  BiosRsvdStartAddr  &= ~(0xff);
  BiosRsvdEndAddr    = (UINT64)(UINTN)aMemoryLlcErrInjAddr + WHEA_EINJ_ADDR_RANGE - 0x400;  // Safe side reduced 200h bytes from the limit.
  mBiosRsvdInjAddr += 0x400;
  if(mBiosRsvdInjAddr >= BiosRsvdEndAddr)  {
      return EFI_OUT_OF_RESOURCES;
  }
  mEinjParam->EinjAddr = mBiosRsvdInjAddr;
  mEinjParam->AddrMask = 0xfffffffffffff000;

  return EFI_SUCCESS;
}

/**

    Check if given SBDF supports error injection.

    @param  PcieSBDF -  BYTE 3 - PCIe Segment
                                 BYTE 2 - Bus Number
                                 BYTE 1 - Device Number[BITs 7:3], Function Number BITs[2:0]
                                 BYTE 0 - RESERVED

    @retval TRUE - Injection capable
    @retval FALSE - Injection not capable

**/
BOOLEAN
ValidateSBDFForInjection (
  IN   UINT32      PcieSBDF
  )
{
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  UINT16  ErrInjCapId = 0xB;
  UINT32  PciInjdevAddr;
  BOOLEAN Supported = FALSE;
  ERRINJCAP_IIO_PCIE_STRUCT ErrInjCap;
  ERRINJCON_IIO_PCIE_STRUCT ErrInjCon;

  Bus = (PcieSBDF >> 16) & (0xFF);
  Dev = (PcieSBDF >> 11) & (0x1F);
  Func = (PcieSBDF >> 8) & (0x7);

  PciInjdevAddr = (UINT32)mIioUdsLib->PlatformData.PciExpressBase | PCI_PCIE_ADDR(Bus, Dev, Func, ONLY_REGISTER_OFFSET(ERRINJCAP_IIO_PCIE_REG));   // ERRINJCAP
  ErrInjCap.Data = *((volatile UINT32 *) ((UINTN) PciInjdevAddr));

  if (((ErrInjCap.Bits.extcapid  & 0xffff) & ErrInjCapId)) {   //  Verify the capability of the injection device
    PciInjdevAddr =  (UINT32)mIioUdsLib->PlatformData.PciExpressBase | PCI_PCIE_ADDR(Bus, Dev, Func, ONLY_REGISTER_OFFSET(ERRINJCON_IIO_PCIE_REG));   // ERRINJCAP
    ErrInjCon.Data = *((volatile UINT16 *) ((UINTN) PciInjdevAddr));
    if (ErrInjCon.Bits.errinjdis == 0) {
      Supported = TRUE;
    } else {
      Supported = FALSE;
    }
  }

  if (Supported == FALSE) {
    RASDEBUG ((DEBUG_ERROR, "Error: Injection is not available on this device (B%x:D%x:F%x)\n", Bus, Dev, Func));
  }

  return Supported;
}

/**

    This function gets the first available port that is connected with slot . This function is for SV usage.
    @param None   -

    @retval PcieSBDF -  BYTE 3 - PCIe Segment
                                BYTE 2 - Bus Number
                                BYTE 1 - Device Number[BITs 7:3], Function Number BITs[2:0]
                                BYTE 0 - RESERVED
    @retval 0x0 - Slot is not populated

**/
UINT32
GetRootPortForInjection (
 VOID
 )
{

  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  UINT8   Port;
  UINT8   RootBridgeLoop;
  UINT32  PciAddr;
  UINT32  Data;
  UINT32  PcieSBDF = 0;
  BOOLEAN Found = FALSE;

  for (RootBridgeLoop = 0; RootBridgeLoop < MaxIIO; RootBridgeLoop++) {
    if(mRasTopology->SystemInfo.SocketInfo[RootBridgeLoop].Valid != 1) continue;

    for (Port = 0; Port < NUMBER_PORTS_PER_SOCKET; Port++) {
      Bus = mRasTopology->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.PciPortInfo[Port].Bus;
      Dev = mRasTopology->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.PciPortInfo[Port].Device;
      Func  = mRasTopology->SystemInfo.SocketInfo[RootBridgeLoop].UncoreIioInfo.PciPortInfo[Port].Function;

      // Ignore the DMI port
      if((Bus == 0) && (Dev == 0) && (Func == 0)) continue;

      // Check if device is present or not
      PciAddr = (UINT32)mIioUdsLib->PlatformData.PciExpressBase | PCI_PCIE_ADDR(Bus, Dev, Func, 00);   // VID
      Data = *((volatile UINT32 *) ((UINTN) PciAddr));
      if ((Data & 0xFFFF) != 0x8086) {
        continue;
      }

      PcieSBDF = (Bus << 16) | (Dev << 11) | (Func << 0x08);
      // Check if the root port is capable of error injection
      Found = ValidateSBDFForInjection (PcieSBDF);
      if (Found == TRUE) {
        return PcieSBDF;
      }
    }
  }

  return 00;
}

/**
  IIO Error Injection Lock Handler

  This function locks error injection in the IIO.

  @param[in] DispatchHandle       Dispatch handle
  @param[in] Context              Context information
  @param[in] CommunicationBuffer  Buffer used for communication between caller/callback function
  @param[in] SourceSize           Size of communication buffer

  @retval Status.

**/
STATIC
EFI_STATUS
IioErrorInjectionLockHandler (
  IN      EFI_HANDLE                          DispatchHandle,
  IN      CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
  IN OUT  VOID                                *CommBuffer,      OPTIONAL
  IN OUT  UINTN                               *CommBufferSize   OPTIONAL
  )
{
  UINTN         Thread;
  UINT8         Node = 0;

  for (Thread = 0; Thread < mSmst->NumberOfCpus; Thread++) {
    mSmst->SmmStartupThisAp (LockInjLogic,
                 Thread,
                 &Node);
  }
  LockInjLogic (&Node);

  return EFI_SUCCESS;
}

/**

    SW SMI handler Registered to EFI_WHEA_EINJ_EXECUTE_SWSMI. This will execute error injection.
    This handler supports patrol scrub injection, EWB error injection and correctable error injections.

    @param DispatchHandle     - The unique handle assigned to this handler by SmiHandlerRegister().Type
                                            EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.1 Specification.
    @param DispatchContext   - Points to the optional handler context which was specified when the handler was registered.
    @param CommBuffer         - A pointer to a collection of data in memory that will be conveyed from a non-SMM
                                           environment into an SMM environment. The buffer must be contiguous, physically mapped, and be a physical address.
    @param CommBufferSize    - The size of the CommBuffer.

    @retval EFI_SUCCESS - The interrupt was handled and quiesced. No other handlers should still be called.

**/
STATIC
EFI_STATUS
WheaEinjHandler (
  IN  EFI_HANDLE                          DispatchHandle,
  IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
  IN OUT VOID                             *CommBuffer,     OPTIONAL
  IN OUT UINTN                            *CommBufferSize  OPTIONAL
  )
{
  UINT8           node=0;
  UINT32          PcieSBDF = 00;
  UINT16          thread=0;
  UINT32          ErrorToInject;
  BOOLEAN         PoisonEnable = FALSE;
  BOOLEAN         Einj5ValidErr = FALSE;
  UINT32          ApicId = 0;
  UINT8          SktId = 0;
  EFI_STATUS      Status;
  EFI_CPU_PHYSICAL_LOCATION Location;

  mEinjParam->OpStatus = 0;
  mEinjParam->CmdStatus = 0;

  mMESegInjEn = FALSE;
  mMESegIcvErrEn = FALSE;

  // This code is needed for sv debuggign purpose.
  /*if(mEinjParam->SetAddressTable.MemAddress == 0) {
    mEinjParam->SetAddressTable.MemAddress  = PcieSBDF + 0x100000000;
    mEinjParam->SetAddressTable.MemAddressRange = 0x0f;
    mEinjParam->SetAddressTable.ErrType =  WHEA_TEMP_MEM_ERROR_TYPE;
  }
  mEinjParam->SetAddressTable.MemAddress += 0x100000; */

  mEinjParam->EinjAddr              = 00;  // Set address
  mEinjParam->AddrMask              = 00;  // Address Mask
  //
  // Check what type of error to be injected and create log accordingly.
  //
  CopyMem (mEinjAction, &mSimEinjAction, sizeof(WHEA_EINJ_TRIGGER_ACTION_TABLE));

  // From SetError type field.
  ErrorToInject = (UINT32)mEinjParam->ErrorToInject[0] & 0x7fffffff;

  mErrorExpected = WHEA_NO_CONSUME;

  if(ErrorToInject == INJECT_ERROR_MEMORY_UE_NON_FATAL) {
    PoisonEnable    = TRUE;
    mErrorExpected  = WHEA_REC_PATROLSCRUB_ERROR;
  }

  if(ErrorToInject == INJECT_ERROR_MEMORY_CE)
    mErrorExpected = WHEA_MEM_COR_ERROR;

  if(mAcpi5Support == TRUE)  {
    // Included Temp error type also for testing by SSG
    if ((mEinjParam->SetAddressTable.ErrType & WHEA_TEMP_MEM_ERROR_TYPE )  ||
       ((mEinjParam->SetAddressTable.ErrType & WHEA_VENDOR_EXT_TYPE) && (mEinjParam->VendorExt.OemDefType2 & WHEA_MEM_ERROR_REC_TYPE) )) {
      ErrorToInject   = INJECT_ERROR_MEMORY_UE_NON_FATAL;
      mErrorExpected  = WHEA_CORE_IFU_DCU_ERR;
      PoisonEnable    = TRUE;
    }

    if ((mEinjParam->SetAddressTable.ErrType & WHEA_VENDOR_EXT_TYPE) && (mEinjParam->VendorExt.OemDefType2 & WHEA_PATROL_SCRUB_ERROR) ){
      ErrorToInject = INJECT_ERROR_MEMORY_UE_NON_FATAL;
      mErrorExpected  = WHEA_REC_PATROLSCRUB_ERROR;
      PoisonEnable    = TRUE;
    }

    if ((mEinjParam->SetAddressTable.ErrType & WHEA_VENDOR_EXT_TYPE) && (mEinjParam->VendorExt.OemDefType2 & WHEA_UMA_ERROR_TYPE) ){
      mMESegInjEn = TRUE;
      PoisonEnable = TRUE;
      mUmaErrorInjectionType = mEinjParam->VendorExt.OemDefType3;

      if (mUmaErrorInjectionType & (WHEA_MESEG_COR_ERR_INJ_CONSUME_NOW | WHEA_MESEG_COR_ERR_INJ_CONSUME_1MS 
              | WHEA_MESEG_COR_ERR_INJ_CONSUME_G0_S1_RW | WHEA_MESEG_COR_ERR_INJ_CONSUME_G0_S1_RW)) {
        ErrorToInject   = INJECT_ERROR_MEMORY_CE;
        RASDEBUG((DEBUG_ERROR, "Requesting UMA CE njection\n"));
      }

      if (mUmaErrorInjectionType & (WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_NOW | WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_1MS 
              | WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_G0_S1_RW | WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_G0_S1_RW)) {
        ErrorToInject   = INJECT_ERROR_MEMORY_UE_NON_FATAL;
        RASDEBUG((DEBUG_ERROR, "Requesting UMA UCE Injection\n"));
      }

      if (mUmaErrorInjectionType & (WHEA_MESEG_ICV_ERR_INJ_CONSUME_NOW | WHEA_MESEG_ICV_ERR_INJ_CONSUME_1MS 
              | WHEA_MESEG_ICV_ERR_INJ_CONSUME_G0_S1_RW | WHEA_MESEG_ICV_ERR_INJ_CONSUME_G0_S1_RW)) {
        mMESegIcvErrEn  = TRUE;
        ErrorToInject   = INJECT_ERROR_MEMORY_CE;
        RASDEBUG((DEBUG_ERROR, "Requesting UMA ICV Error Injection\n"));
      }
    }
 
    if( (mEinjParam->SetAddressTable.ErrType & WHEA_VENDOR_EXT_TYPE) != WHEA_VENDOR_EXT_TYPE) {

      if( mEinjParam->SetAddressTable.ErrType & ( INJECT_ERROR_PCIE_CE | INJECT_ERROR_PCIE_UE_NON_FATAL | INJECT_ERROR_PCIE_UE_FATAL | INJECT_ERROR_MEMORY_CE |
                                                INJECT_ERROR_PROCESSOR_UE_NON_FATAL | INJECT_ERROR_MEMORY_UE_NON_FATAL | INJECT_ERROR_MEMORY_UE_FATAL)) {
        ErrorToInject = mEinjParam->SetAddressTable.ErrType;
        if(mEinjParam->SetAddressTable.ErrType & INJECT_ERROR_MEMORY_UE_NON_FATAL ) {
          PoisonEnable    = TRUE;
          mErrorExpected  = WHEA_REC_PATROLSCRUB_ERROR;
        }
        if( mEinjParam->SetAddressTable.ErrType & INJECT_ERROR_MEMORY_CE) {
          mErrorExpected = WHEA_MEM_COR_ERROR;
        }
      }
    }

    if((mEinjParam->SetAddressTable.Flags & WHEA_SET_ERROR_TYPE_WITH_MEMORY_ADDRESS_VALID) == WHEA_SET_ERROR_TYPE_WITH_MEMORY_ADDRESS_VALID) {
    mEinjParam->EinjAddr = (UINT64)mEinjParam->SetAddressTable.MemAddress;  // Set address
    mEinjParam->AddrMask = (UINT64)mEinjParam->SetAddressTable.MemAddressRange;  // Address Mask
    Einj5ValidErr  = TRUE;
    }

    RASDEBUG((DEBUG_ERROR, "mEinjParam->SetAddressTable.Flags : %d",mEinjParam->SetAddressTable.Flags));

    if((mEinjParam->SetAddressTable.Flags & WHEA_SET_ERROR_TYPE_WITH_PROC_APIC_VALID) == WHEA_SET_ERROR_TYPE_WITH_PROC_APIC_VALID) {
      ApicId = mEinjParam->SetAddressTable.ApicId;
      RASDEBUG ((DEBUG_INFO, " - WheaEinjHandler: ApicId= %x, \n", ApicId ));
      ExtractProcessorLocation(ApicId,&Location);
      SktId = (UINT8)Location.Package;
      RASDEBUG ((DEBUG_INFO, " - WheaEinjHandler: ApicId= %x, SocketID = %x ,\n", ApicId ,SktId));
    }
    mEinjParam->SetAddressTable.ErrType = 00;  // Bios has to clear this field after we consumed it.
  }
/*
  if(Einj5ValidErr == FALSE && mEinjParam->ErrorToInject[3] && mEinjParam->ErrorToInject[4] ) {  //Check the old type of address is paassed in the set error type structure.
    mEinjParam->EinjAddr              = mEinjParam->ErrorToInject[3];  // Set address
    mEinjParam->AddrMask              = mEinjParam->ErrorToInject[4];  // Address Mask
  }
*/
  if((ErrorToInject != INJECT_ERROR_PCIE_UE_NON_FATAL) && (ErrorToInject != INJECT_ERROR_PCIE_CE) ) {
    if((!(mEinjParam->EinjAddr)) || (!(mEinjParam->AddrMask)) ) {

      Status = SetBiosRsvdInjAddress( );

      if (EFI_ERROR(Status)) {
        mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
        return EFI_INTERRUPT_PENDING;
      }
    }
  }

  switch (ErrorToInject) {

    case INJECT_ERROR_PROCESSOR_UE_NON_FATAL:           //Explicit Write Back Error

      if( (mIioUdsLib->SystemStatus.socketPresentBitMap >> SktId) == 0 ) {
        RASDEBUG ((DEBUG_ERROR, " - EWB injection Request, SktId passed= %x, Socket not present\n", SktId ));
        return EFI_SUCCESS;
      }

      if (mRasCapability->SiliconRasCapability.PoisonCap != 0) {
        for (thread = 0; thread < mSmst->NumberOfCpus; thread++) {
          mSmst->SmmStartupThisAp (EnablePoison,
                       thread,
                       &node);
        }
        EnablePoison(&node);
      }

      InjectLLCError(SktId);
      break;

    case INJECT_ERROR_MEMORY_CE:
      RASDEBUG ((DEBUG_ERROR, "INJECT_ERROR_MEMORY_CE\n"));
      InjectMemoryError( INJECT_ERROR_MEMORY_CE);
      break;

    case INJECT_ERROR_MEMORY_UE_NON_FATAL:
      RASDEBUG ((DEBUG_ERROR, "INJECT_ERROR_MEMORY_UE_NON_FATAL\n"));
      if ((PoisonEnable == TRUE) && (mRasCapability->SiliconRasCapability.PoisonCap != 0)) {
        for (thread = 0; thread < mSmst->NumberOfCpus; thread++) {
          mSmst->SmmStartupThisAp (EnablePoison,
                       thread,
                       &node);
        }
        EnablePoison(&node);
      }
      InjectMemoryError(INJECT_ERROR_MEMORY_UE_NON_FATAL);
      break;

    case INJECT_ERROR_MEMORY_UE_FATAL:
      RASDEBUG ((DEBUG_ERROR, "INJECT_ERROR_MEMORY_UE_FATAL\n"));

      // For WHQL testing , Disabling the poison and injecting uncorrected memeory error to cause system reset with blue screen.
      if (mRasCapability->SiliconRasCapability.PoisonCap != 0) {
        for (thread = 0; thread < mSmst->NumberOfCpus; thread++) {
            mSmst->SmmStartupThisAp (DisablePoison,
                         thread,
                         &node);
        }
        DisablePoison(&node);
      }
      InjectMemoryError(INJECT_ERROR_MEMORY_UE_FATAL);
      break;

    case INJECT_ERROR_PCIE_UE_FATAL :
    case INJECT_ERROR_PCIE_CE :
    case INJECT_ERROR_PCIE_UE_NON_FATAL :
      // Check Set addresstable has valid bus, dev, func numbers.
      if((mEinjParam->SetAddressTable.PcieSBDF) && ( ((mEinjParam->SetAddressTable.Flags & WHEA_SET_ERROR_TYPE_WITH_SBDF_VALID) == WHEA_SET_ERROR_TYPE_WITH_SBDF_VALID) ) ) {
        PcieSBDF = mEinjParam->SetAddressTable.PcieSBDF;
        if (ValidateSBDFForInjection (PcieSBDF) == TRUE) {
          Status = InjectPciExError (PcieSBDF, ErrorToInject, mEinjAction, mEinjParam);
        } else {
          Status = EFI_UNSUPPORTED;
        }
      } else  {
        PcieSBDF = GetRootPortForInjection ();
        if (PcieSBDF) {
          Status = InjectPciExError (PcieSBDF, ErrorToInject, mEinjAction, mEinjParam);
        } else {
          Status = EFI_UNSUPPORTED;
        }
      }

      if (EFI_ERROR (Status)) {
        mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
      }
    break;

    default:
      break;

  }
  return EFI_SUCCESS;

}

/**

  Entry point of the Whea error injection driver.

  @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
InitWheaErrorInj (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        SwHandle;
  EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch  = 0;
  BOOLEAN                           InSmm;
  EFI_IIO_UDS_PROTOCOL              *IioUdsProtocol;

  //
  // Retrieve SMM Base Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  &mSmmBase
                  );
  if (mSmmBase == NULL) {
    InSmm = FALSE;
  } else {
      mSmmBase->InSmm (mSmmBase, &InSmm);
    mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  }

  if (InSmm) {
    //
    // Init WHEA globals
    //
    Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid,NULL,&IioUdsProtocol);
    ASSERT_EFI_ERROR (Status);
    mIioUdsLib = IioUdsProtocol->IioUdsPtr;

    //
    // Locate CrystalRidge Protocol
    //
    Status = mSmst->SmmLocateProtocol (&gEfiCrystalRidgeSmmGuid, NULL, &mCrystalRidgeProtocol);
    ASSERT_EFI_ERROR (Status);

    // Update EINJ table entries.
    Status = UpdateEinj();
    if (EFI_ERROR(Status)) {
      return EFI_SUCCESS;
    }

    //
    // 5332907: CLONE SKX Sighting: IMC Error Injection Fails ~25% of the Time With Directory Mode Enabled on 2S
    // For this sighting, a Warning message will be displayed to let user know to disable DirectoryMode in BIOS setup
    //
    if (mRasTopology->SystemInfo.CpuStepping < H0_REV_SKX) {
        if ((mRasSetup->Qpi.DirectoryModeEn == 1) && (mRasTopology->SystemInfo.NumOfSocket > 1)) {
            SysDirectoryModeChkForInj = 1;
        }
    }

    //A0 Si WA 4929966: Cloned From SKX Si Bug Eco: DDRTRdViralDnnnH is not qualified with DDRT mode
    //Use the BIOS know to disable HA and DDRT parity check
    if (mRasTopology->SystemInfo.CpuStepping < B0_REV_SKX) {
      SetHAParityCheckEnable(mRasSetup->ErrInj.ParityCheckEn);
    }

    Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    ASSERT_EFI_ERROR (Status);

    Status = mSmst->SmmLocateProtocol (&gEfiSmmCpuServiceProtocolGuid, NULL, &mSmmCpuServiceProtocol);
    ASSERT_EFI_ERROR (Status);

    Status = mSmst->SmmLocateProtocol (&gEfiSmmPeriodicTimerDispatch2ProtocolGuid, NULL, (VOID **)&mSmmPeriodicTimerDispatch);
    ASSERT_EFI_ERROR (Status);

    //
    // Install SW SMI handler to build error data for simulated error injection
    //
    SwContext.SwSmiInputValue = EFI_WHEA_EINJ_EXECUTE_SWSMI;
    Status                    = SwDispatch->Register (SwDispatch, WheaEinjHandler, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);

    EinjIioErrInjLockSwSmi = EFI_WHEA_EINJ_IIO_ERR_INJ_LCK_SWSMI;
    SwContext.SwSmiInputValue = EinjIioErrInjLockSwSmi;
    Status = SwDispatch->Register (SwDispatch, IioErrorInjectionLockHandler, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);

// APTIOV_SERVER_OVERRIDE_RC_START : SPS only
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : SPS only
    //
    //SWSMI Handler for ME page Error consumption
    //
    if (mRasSetup->ErrInj.MeSegErrorInjEn) {
      SwContext.SwSmiInputValue = EFI_WHEA_ME_EINJ_CONSUME_SWSMI;
      Status                    = SwDispatch->Register (SwDispatch, WheaMeEinjSwSmiHanlder, &SwContext, &SwHandle);
      ASSERT_EFI_ERROR (Status);
    }
// APTIOV_SERVER_OVERRIDE_RC_START : SPS only
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : SPS only
  }
  return EFI_SUCCESS;
}

/**

    Updates and install EINJ ACPI Tables

    @param None

    @retval Status - The update/install was successful

**/
EFI_STATUS
UpdateEinj (
  VOID
  )
{

  UINT8                             *Ests;
  UINT32                            Offset;
  UINT8                             Size = 4;
  EFI_WHEA_SUPPORT_PROTOCOL         *WheaDrv;
  EFI_STATUS                        Status;

  //
  // DXE/PST initialization - create all error sources
  //
  Status = gBS->LocateProtocol (&gEfiWheaSupportProtocolGuid, NULL, &WheaDrv);
  if (EFI_ERROR(Status))return Status;

  Status = mSmst->SmmLocateProtocol(
                           &gEfiPlatformRasPolicyProtocolGuid,
                           NULL,
                           &mPlatformRasPolicyProtocol
                          );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mRasTopology = mPlatformRasPolicyProtocol->EfiRasSystemTopology;
  mRasSetup = mPlatformRasPolicyProtocol->SystemRasSetup;
  mRasCapability = mPlatformRasPolicyProtocol->SystemRasCapability;

  if (mRasSetup->ErrInj.WheaErrorInjSupportEn == 0) {
    RASDEBUG ((DEBUG_ERROR, " WHEA Error Injection is not enabled in Setup\n"));
    return EFI_UNSUPPORTED;
  }

  // Locate MemRas protocol
  Status = mSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &mMemRas);
  if (EFI_ERROR(Status))return Status;

  // Locate CPUCSR access protocol
  Status = mSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  if (EFI_ERROR(Status))return Status;

  //
  // Allocate Error status block for Error Injection - we should change this to use
  // Error status block assigned for the error source
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, 0x900, (VOID **)&Ests);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (Ests, 0x900);

  //
  // Init buffer pointers and data for Einj parameter and Error log address range.
  //
  mEinjParam    = (WHEA_EINJ_PARAM_BUFFER *)Ests;
  mEinjAction   = (WHEA_EINJ_TRIGGER_ACTION_TABLE *)(Ests + ((sizeof(WHEA_EINJ_PARAM_BUFFER)+0x3F)/0x40)*0x40);
  mEinjEsts     = ((UINT8 *)mEinjAction) + ((sizeof(WHEA_EINJ_TRIGGER_ACTION_TABLE)+0x3F)/0x40)*0x40;

  //
  // Init EINJ action table in the allocated reserved memory
  // Update Error injection Action table - Static initialization is sufficient
  //
  gBS->CopyMem(mEinjAction, &mSimEinjAction, sizeof(WHEA_EINJ_TRIGGER_ACTION_TABLE));

  // Allocate memory for 6000h bytes for Error injection structure.
  Status = gBS->AllocatePool (EfiRuntimeServicesData, WHEA_EINJ_ADDR_RANGE, &aMemoryLlcErrInjAddr);
  // Make it cache line boundary - This is the incremantal injection address Bios uses for error injection
  mBiosRsvdInjAddr  = (UINT64)(UINTN)aMemoryLlcErrInjAddr + 0x800;
  mBiosRsvdInjAddr  &= ~(0xff);
  //
  // Init EINJ parameters
  //
  mEinjParam->TriggerActionTable    = (UINT64) (UINTN)mEinjAction;
  mEinjParam->ErrorInjectCap        = INJECT_ERROR_MEMORY_UE_NON_FATAL | INJECT_ERROR_MEMORY_CE | INJECT_ERROR_MEMORY_UE_FATAL;

  // Check Acpi 5.0 support enabled.
  if (mRasSetup->ErrInj.WheaErrInjEn)
    mAcpi5Support  = TRUE;

  if (mRasSetup->ErrInj.WheaPcieErrInjEn)
    mEinjParam->ErrorInjectCap  |= INJECT_ERROR_PCIE_CE  | INJECT_ERROR_PCIE_UE_NON_FATAL | INJECT_ERROR_PCIE_UE_FATAL ;

  if(mRasSetup->ErrInj.PcieErrInjActionTable == 1) {
    mPcieErrInjActionTable = TRUE;
  }
  mEinjParam->PcieErrInjActionTable = mPcieErrInjActionTable;

  if(mAcpi5Support == TRUE) {
    mEinjParam->ErrorInjectCap      |= WHEA_VENDOR_EXT_TYPE | WHEA_TEMP_MEM_ERROR_TYPE;
    mEinjParam->VendorExt.OemDefType1 = WHEA_MEM_ERROR_REC_TYPE | WHEA_PATROL_SCRUB_ERROR;
  }
  mEinjParam->ErrorToInject[3]      = 0;
  mEinjParam->ErrorToInject[4]      = 0;
  mEinjParam->OpState               = 0;
  mEinjParam->OpStatus              = 0;
  mEinjParam->CmdStatus             = 0;
  mEinjParam->ErrorToInject[0]      = 0;
  mEinjParam->EinjAddr              = 0;
  mEinjParam->AddrMask              = 0;


  //
  // Update Error injection serialization informations
  //
  mSimEinj[0].Register.Address = (UINT64) (UINTN)&mEinjParam->OpState;              // Update address for BEGIN_OPERATION
  mSimEinj[1].Register.Address = (UINT64) (UINTN)&mEinjParam->TriggerActionTable;   // Update address for GET_TRIGGER_TABLE
  mSimEinj[2].Register.Address = (UINT64) (UINTN)&mEinjParam->ErrorToInject[0];     // Update address for SET_ERROR
  mSimEinj[3].Register.Address = (UINT64) (UINTN)&mEinjParam->ErrorInjectCap;       // Update address for GET_ERROR_CAP
  mSimEinj[4].Register.Address = (UINT64) (UINTN)&mEinjParam->OpState;              // Update address for END_OPERATION
  mSimEinj[6].Register.Address = (UINT64) (UINTN)&mEinjParam->OpStatus;             // Update address for CHECK_BUSY_STATUS
  mSimEinj[7].Register.Address = (UINT64) (UINTN)&mEinjParam->CmdStatus;            // Update address for GET_CMD_STATUS

  if(mAcpi5Support == TRUE)  {
    mSimEinj[8].Register.Address = (UINT64) (UINTN)&mEinjParam->SetAddressTable;        // Update address for SET_ERROR_TYPE_WITH_ADDRESS

    Offset = (UINT32)((UINTN)&mEinjParam->VendorExt - (UINTN)&mEinjParam->SetAddressTable);
    mEinjParam->SetAddressTable.VendorErrTypeExtOffset = Offset;
    mEinjParam->SetAddressTable.PcieSBDF  = 00;

    mEinjParam->VendorExt.Length    = sizeof (WHEA_EINJ_VENDOR_EXT);
    //SBDF Byte 0 - reserved, Byte 1 - Device and Function, Byte 2 - Bus, Byte 3 PCIe segment
    mEinjParam->VendorExt.SBDF      = (mCpuCsrAccess->GetCpuCsrAddress (0, 0, VID_M2MEM_MAIN_REG, &Size) & 0xFFFF000) >> 4;
    mEinjParam->VendorExt.VendorId  = (UINT16)mCpuCsrAccess->ReadCpuCsr(0, 0, VID_M2MEM_MAIN_REG);
    mEinjParam->VendorExt.DeviceId  = (UINT16)mCpuCsrAccess->ReadCpuCsr(0, 0, DID_M2MEM_MAIN_REG);
    mEinjParam->VendorExt.RevId     = (UINT8)mCpuCsrAccess->ReadCpuCsr(0, 0, VID_M2MEM_MAIN_REG);
  }

  

 // APTIOV_SERVER_OVERRIDE_RC_START : SPS only
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : SPS only
  // ME Seg Error Injection support
  if (mRasSetup->ErrInj.MeSegErrorInjEn){
    //
    // Call ME interface to get the Page Offset for Error Injection
    // 
    DEBUG ((DEBUG_ERROR, "MeSegErrorInj is enabled in setup\n"));
    Status = GetMEPageForErrorInj();

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Error in getting MEPage\n"));
      mMESegInjAddr = ((UINT64)-1);
    } else {
      mEinjParam->VendorExt.OemDefType1 |= WHEA_UMA_ERROR_TYPE;
      DEBUG ((DEBUG_ERROR, "The address of mEinjParam->SetAddressTable.ErrType = 0x%lx\n", &(mEinjParam->SetAddressTable.ErrType)));
      DEBUG ((DEBUG_ERROR, "The address of mEinjParam->VendorExt.OemDefType1 = 0x%lx\n", &(mEinjParam->VendorExt.OemDefType1)));
      DEBUG ((DEBUG_ERROR, "The address of mEinjParam->VendorExt.OemDefType2 = 0x%lx\n", &(mEinjParam->VendorExt.OemDefType2)));
      DEBUG ((DEBUG_ERROR, "The address of mEinjParam->VendorExt.OemDefType3 = 0x%lx\n", &(mEinjParam->VendorExt.OemDefType3)));
    }



  }
// APTIOV_SERVER_OVERRIDE_RC_START : SPS only
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : SPS only


  RASDEBUG ((DEBUG_ERROR, " WHEA Error Injection driver is installed\n"));
  //
  // Register core Whea Error injection method
  //
  Status = WheaDrv->InstallErrorInjectionMethod (
             WheaDrv,
             INJECT_ERROR_PROCESSOR_UE_NON_FATAL | INJECT_ERROR_MEMORY_UE_NON_FATAL | INJECT_ERROR_MEMORY_CE,

             00,         // WheaSupport.c file updates the entries.
             &mSimEinj[0]
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
