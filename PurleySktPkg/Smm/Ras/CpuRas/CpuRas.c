/*++

Copyright (c) 2008 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CpuRas.c

Abstract:



--*/

//BKL_PORTING - remove this when all macro re-definitions are solved
#pragma warning(disable :4005)
//BKL_PORTING


#include "CpuRas.h"
#include <Library/mpsyncdatalib.h>
#include <Register/PchRegsPmc.h>

EFI_RAS_MPLINK_PROTOCOL           *mRasMpLink;
EFI_CPU_RAS_PROTOCOL              *mCpuRas;
EFI_MEM_RAS_PROTOCOL              *mMemRas  = NULL;
EFI_CPU_CSR_ACCESS_PROTOCOL       *mCpuCsrAccess;
static EFI_SMM_CPU_SERVICE_PROTOCOL      *mSmmCpuServiceProtocol;
EFI_HP_IOX_ACCESS_PROTOCOL        *mHpIOXAccess;
EFI_CPU_PM_STRUCT                 *mLocalPpmStruct;
EFI_IIO_UDS_PROTOCOL              *mIioUds;
IIO_UDS                           *mIioUdsData;
BIOS_ACPI_PARAM                   *mAcpiParameter;
ACPI_CPU_DATA                     *mAcpiCpuData;
CPU_HOT_ADD_DATA                  *mCpuHotAddData;
HOT_ADD_CPU_EXCHANGE_INFO         *mExchangeInfo = NULL;
EFI_PHYSICAL_ADDRESS               mStartupVector;
EFI_PHYSICAL_ADDRESS               mSmmStartupcodeArea;
SPS_RAS_NOTIFY_PROTOCOL           *mSpsRasNotifyProtocol;

CPU_HOT_PLUG_DATA                  *mCpuSmmInitData;

UINT8                             mCpuRasNextState;
UINT8                             mCpuRasStartDone;
UINT32                            mGlobalPkgCSControl[MAX_SOCKET];
UINT32                            mGblNidMap0, mGblNidMap1;
UINT32                            mLatestSocketPresentBitMap, mCurrentCpuStatus;
PAM456_CHABC_SAD_STRUCT           mSbspPam456Reg;
UINT32                            mRCBA, mPmSyncData, mAdrEn;


GENPROTRANGE0_BASE_N0_IIO_VTD_STRUCT  mSbspGenProt0Base0[MAX_IIO_STACK];
GENPROTRANGE0_BASE_N1_IIO_VTD_STRUCT  mSbspGenProt0Base1[MAX_IIO_STACK];
GENPROTRANGE0_LIMIT_N0_IIO_VTD_STRUCT mSbspGenProt0Limit0[MAX_IIO_STACK];
GENPROTRANGE0_LIMIT_N1_IIO_VTD_STRUCT mSbspGenProt0Limit1[MAX_IIO_STACK];

VOID ProgramInComingSocketGenProtReg (
  UINT8   SbspSktId,
  UINT8   IncomingSktIdx
  )
{
  UINT32  Data32;
  UINT8   Stack;
  UINT8   SktId;

  for(Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    if (!(mIioUdsData->PlatformData.CpuQpiInfo[IncomingSktIdx].stackPresentBitmap & (1 << Stack)))
      continue;
    // program GEN4
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE4_BASE_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE4_BASE_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE4_BASE_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE4_BASE_N0_IIO_VTD_REG, Data32);

    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE4_LIMIT_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE4_LIMIT_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE4_LIMIT_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE4_LIMIT_N0_IIO_VTD_REG, Data32);

    //program GEN3
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE3_BASE_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE3_BASE_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE3_BASE_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE3_BASE_N0_IIO_VTD_REG, Data32);

    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE3_LIMIT_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE3_LIMIT_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE3_LIMIT_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE3_LIMIT_N0_IIO_VTD_REG, Data32);

    //program GEN2
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE2_BASE_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE2_BASE_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE2_BASE_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE2_BASE_N0_IIO_VTD_REG, Data32);

    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE2_LIMIT_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE2_LIMIT_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE2_LIMIT_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE2_LIMIT_N0_IIO_VTD_REG, Data32);

    //program GEN1
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE1_BASE_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE1_BASE_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE1_BASE_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE1_BASE_N0_IIO_VTD_REG, Data32);

    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE1_LIMIT_N1_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE1_LIMIT_N1_IIO_VTD_REG, Data32);
    Data32 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE1_LIMIT_N0_IIO_VTD_REG);
    mCpuCsrAccess->WriteCpuCsr(IncomingSktIdx, Stack, GENPROTRANGE1_LIMIT_N0_IIO_VTD_REG, Data32);

    //Save GenProtRang0 register of SBSP
    mSbspGenProt0Base1[Stack].Data = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE0_BASE_N1_IIO_VTD_REG);
    mSbspGenProt0Base0[Stack].Data = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE0_BASE_N0_IIO_VTD_REG);
    mSbspGenProt0Limit1[Stack].Data = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE0_LIMIT_N1_IIO_VTD_REG);
    mSbspGenProt0Limit0[Stack].Data = mCpuCsrAccess->ReadCpuCsr(SbspSktId, Stack, GENPROTRANGE0_LIMIT_N0_IIO_VTD_REG);


    //Program GenprotRange0 register to protect 38000 segment on all active sockets
    for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
      if (((mLatestSocketPresentBitMap >> SktId) & 0x01) == 1) {
        Data32 = 0;
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_BASE_N1_IIO_VTD_REG, Data32);
        Data32 = 0x30000;
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_BASE_N0_IIO_VTD_REG, Data32);
        Data32 = 0;
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_LIMIT_N1_IIO_VTD_REG, Data32);
        Data32 = 0x30000;
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_LIMIT_N0_IIO_VTD_REG, Data32);
      }
    } //end for socket

  } //end for Stack

  return;
}

VOID 
RestoreGenProtReg (
  VOID
  )
{
  UINT8   SktId;
  UINT8   Stack;

  for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
    if (((mLatestSocketPresentBitMap >> SktId) & 0x01) == 1) {
      for(Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
        if (!(mIioUdsData->PlatformData.CpuQpiInfo[SktId].stackPresentBitmap & (1 << Stack))) continue;
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_BASE_N1_IIO_VTD_REG, mSbspGenProt0Base1[Stack].Data);
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_BASE_N0_IIO_VTD_REG, mSbspGenProt0Base0[Stack].Data);
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_LIMIT_N1_IIO_VTD_REG, mSbspGenProt0Limit1[Stack].Data);
        mCpuCsrAccess->WriteCpuCsr(SktId, Stack, GENPROTRANGE0_LIMIT_N0_IIO_VTD_REG, mSbspGenProt0Limit0[Stack].Data);
      }
    }
  }

  return;
}

UINT32
GetHotAddPbspApicId (
    UINT8  SocketId
)
{
  UINT32  ApicId;

  ApicId = mCpuCsrAccess->ReadCpuCsr(SocketId, 0, SR_PBSP_CHECKIN_CSR);
  ApicId = (ApicId >> 8) & 0xFF;

  return ApicId;
}


EFI_STATUS
CpuRasOnline (
)
{
  UINT8           SbspSktId;
  UINT8           SktIdToBeOl;
  EFI_STATUS      Status;
  UINT8           SktId;
  UINT32          PbspApicId = 0;
  UINT32          Index;
  UINT32          ApicId;
  UINTN           ProcessorNumber;
  UINT64          CoreThreadBitMap, CoreThreadId;
  UINT32          Temp=0;
  UINT8           Port;
  UINT8           RestoreSbspPam = 0;
  PAM456_CHABC_SAD_STRUCT Pam456Reg;
  BIOS_RESET_CPL_PCU_FUN1_STRUCT RstCplReg, SbspRstCplReg;

  Status = IN_PROGRESS;

  SbspSktId = mRasMpLink->Host->nvram.common.sbspSocketId;
  SktIdToBeOl = mCpuRas->SocketToBeOl;
  mLatestSocketPresentBitMap = mRasMpLink->Host->var.common.socketPresentBitMap;

  if (mCpuRas->FirstCpuRasEntry) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online First Entry\n"));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nThe ID of the CPU that is being onlined = %x",SktIdToBeOl));
    mCurrentCpuStatus = mRasMpLink->Host->var.common.socketPresentBitMap;
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCurrentCpuStatus = %x\n",mCurrentCpuStatus));

    mCpuRas->FirstCpuRasEntry = 0;

    // Check if the AP buffer has been relocated
    if (mExchangeInfo->InitFlag < 2) {
      Status = NO_SUPPORT;
      return Status;
    }

    mCpuRasNextState = CpuRasOnlineRasStart;
  }

  if (mCpuRasNextState == CpuRasOnlineRasStart) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - In CpuRasOnlineRasStart state\n"));

    // Notify BMC
    PlatformHookNotifyBmc(mCpuRas->RASEvent, RasEventStart);
    // Notify ME
    PlatformHookNotifyME(mCpuRas->RASEvent, RasNotificationStart);
    PlatformHookNotifyME(mCpuRas->RASEvent, RasNotificationSerialPeciQuiescence);

    // RAS Start
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online - Do the RAS Start on existing CPUs\n"));
    for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
      if (((mCurrentCpuStatus >> SktId) & 0x01) == 1) {
        Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(SktId, MAILBOX_BIOS_CMD_RAS_START, 0);
      }
    }

    // PM Synch disable
    // Save ADR_EN and PM_SYNC_MODE register, clear ADR_EN and PM_SYNCH_MODE registers
    // Save and clear ADR_EN
    mAdrEn = mCpuCsrAccess->ReadPciCsr (0,REG_ADDR(0,PMC_DEVICE, PMC_FUNC, ADR_EN, sizeof(UINT32)));
    mCpuCsrAccess->WritePciCsr (0,REG_ADDR(0,PMC_DEVICE, PMC_FUNC, ADR_EN, sizeof(UINT32)), 0);
    //Save and clear PM_SYNC_MODE
    mPmSyncData = mCpuCsrAccess->ReadPciCsr (0,REG_ADDR(0,PMC_DEVICE, PMC_FUNC, PM_SYNC_MODE, sizeof(UINT32)));
    mCpuCsrAccess->WritePciCsr (0,REG_ADDR(0,PMC_DEVICE, PMC_FUNC, PM_SYNC_MODE, sizeof(UINT32)), 0);
    MicroSecondDelay  (5);

    // Capture Global_PKG_C_S_control Register on all existing sockets, to do the sanity check before the RAS_EXIT
    for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
      if (((mCurrentCpuStatus >> SktId) & 0x01) == 1) {
         mGlobalPkgCSControl[SktId] = mCpuCsrAccess->ReadCpuCsr(SktId, 0, GLOBAL_PKG_C_S_CONTROL_REGISTER_PCU_FUN2_REG);
         DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tmGlobalPkgCSControl of %d is %x\n",SktId,mGlobalPkgCSControl[SktId]));
      }
    }
    // Capture GlobNid Register
    mGblNidMap0 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, 0, GLOBAL_NID_SOCKET_0_TO_3_MAP_PCU_FUN2_REG);
    mGblNidMap1 = mCpuCsrAccess->ReadCpuCsr(SbspSktId, 0, GLOBAL_NID_SOCKET_4_TO_7_MAP_PCU_FUN4_REG);

    mCpuRasNextState = CpuRasOnlining;
  }

  if (mCpuRasNextState == CpuRasOnlining) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - RasMpLinkCheckEvent Event Start\n"));
    Status = mRasMpLink->RasMpLinkCheckEvent(RAS_MPLINK_EVNT_ONLINE, SktIdToBeOl);

    if (Status == RAS_MPLINK_PENDING) {
      return IN_PROGRESS;
    }

    if (Status == RAS_MPLINK_SUCCESS) {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU Online - RasMpLinkCheckEvent End\n"));
      mCpuRasNextState = CpuRasOnlineQpiDiscovery;
      return IN_PROGRESS;
    } else {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU Online - ERROR in RasMpLinkCheckEvent, exiting the CPU RAS Handler\n"));
      return ERROR_EXIT;
    }

  }

  if (mCpuRasNextState == CpuRasOnlineQpiDiscovery) {

    // Need to call QPI RAS for the QPI for initializing the uncore
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - calling QPI Ras Handler for  QpiDiscovery \n"));
    Status = mRasMpLink->RasMpLinkHandler (RAS_MPLINK_EVNT_ONLINE, SktIdToBeOl);

    if (Status == RAS_MPLINK_PENDING) {
      return IN_PROGRESS;
    }
    if (Status == RAS_MPLINK_INT_DOMAIN_PENDING) {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU Online - QpiDiscovery is Success\n"));
      mCpuRasNextState = CpuRasOnlineBringInPbsp;
      return IN_PROGRESS;
    } else {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU Online - ERROR in Qpi Topology Discovery, exiting the CPU RAS Handler\n"));
      return ERROR_EXIT;
    }
  }

  if (mCpuRasNextState == CpuRasOnlineBringInPbsp) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - In BringInPBSP state \n"));

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nUpdate CpuCsrAccess Protcol data structure"));
    mCpuCsrAccess->UpdateCpuCsrAccessVar();

    // Clear  BIST in waking buffer
    SetMem ((VOID *)mExchangeInfo->BistBuffer, sizeof(mExchangeInfo->BistBuffer), 0);
    mExchangeInfo->InitFlag        = 3;
    mExchangeInfo->NumCpuCheckedIn = 1;
    mExchangeInfo->SignalFlag      = 1;
    mExchangeInfo->PModeSignalFlag = 0;
    mLocalPpmStruct->Info->SocketPresentBitMap = mIioUdsData->SystemStatus.socketPresentBitMap;

    // Add the hot added processor into IPI list
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - Add socket to the IPI list \n"));
    mRasMpLink->RasMpLinkAdjustIpiList(1);

    mSbspPam456Reg.Data = mCpuCsrAccess->ReadCpuCsr(SbspSktId, 0, PAM456_CHABC_SAD_REG);
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nPAM Register of SBSP: is %x\n", mSbspPam456Reg.Data));

    // Open PAM Region for incoming CPU
    Pam456Reg.Data = mCpuCsrAccess->ReadCpuCsr(SktIdToBeOl, 0, PAM456_CHABC_SAD_REG);
    Pam456Reg.Bits.pam4_hienable  = 3;  // 0DC000-0DFFFF
    Pam456Reg.Bits.pam4_loenable  = 3;  // 0D8000-0DBFFF
    Pam456Reg.Bits.pam5_hienable  = 3;  // 0E4000-0E7FFF
    Pam456Reg.Bits.pam5_loenable  = 3;  // 0E0000-0E3FFF
    Pam456Reg.Bits.pam6_hienable  = 3;  // 0EC000-0EFFFF
    Pam456Reg.Bits.pam6_loenable  = 3;  // 0E8000-0EBFFF
    mCpuCsrAccess->WriteCpuCsr(SktIdToBeOl, 0, PAM456_CHABC_SAD_REG, Pam456Reg.Data);
    Pam456Reg.Data = mCpuCsrAccess->ReadCpuCsr(SktIdToBeOl, 0, PAM456_CHABC_SAD_REG);

    // Program GenProtRange registers of the incoming CPU to the same values as SBSP
    ProgramInComingSocketGenProtReg(SbspSktId, SktIdToBeOl);

    // Send INIT and Start IPI to the hot added processor
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - Send SIPI to the new CPU\n"));
    PbspApicId = GetHotAddPbspApicId (SktIdToBeOl);
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online -PBSP APICID of the new CPU is %x\n", PbspApicId));

    //
    // Restore Startup Vector Data from Boot-Time
    //
    if (mSbspPam456Reg.Bits.pam5_loenable != 3) {
      RestoreSbspPam = 1;
      Pam456Reg.Data = mSbspPam456Reg.Data;
      Pam456Reg.Bits.pam5_loenable = 3;
      mCpuCsrAccess->WriteCpuCsr(SbspSktId, 0, PAM456_CHABC_SAD_REG, Pam456Reg.Data);
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nPAM Register of SBSP: is %x\n", Pam456Reg.Data));
    }
    CopyMem((VOID *)(UINTN)mStartupVector, (VOID *)(UINTN)mSmmStartupcodeArea, 0x1000);
	
    SendInitSipiSipi (PbspApicId, (UINT32)mStartupVector);

    mCpuRasNextState = CpuRasOnlineWaitForMpInit;
  }

  if (mCpuRasNextState == CpuRasOnlineWaitForMpInit) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - In WaitForMpInit state \n"));

    // Check for package BSP MpInit done
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online - Waiting for PBSP Init done\n"));
    while (mExchangeInfo->SignalFlag != (UINT32)(-1)) {
      CpuPause ();
    }

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online - PBSP Init done\n"));

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tAPIC Ids of the incoming CPU\n"));

    for (Index = 0; Index < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); Index++) {
    if (mExchangeInfo->BistBuffer[Index].Number != 0) {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tThe APIC id is %x\n",mExchangeInfo->BistBuffer[Index].BIST));
      }
    }

    // Prepare for SMM relocation
    mExchangeInfo->CpuSmmInitData.IEDBase = mCpuSmmInitData->IEDBase;
    mExchangeInfo->CpuSmmInitData.SmrrBase = mCpuSmmInitData->SmrrBase;
    mExchangeInfo->CpuSmmInitData.SmrrSize = mCpuSmmInitData->SmrrSize;

    for (Index = 0; Index < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); Index++) {
      mExchangeInfo->CpuSmmInitData.ApicId[Index] = mCpuSmmInitData->ApicId[Index];
      mExchangeInfo->CpuSmmInitData.SmBase[Index] = mCpuSmmInitData->SmBase[Index];
    }

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n\tCPU Online - Do SMM relocation on new CPU\n"));
    mCpuHotAddData->PackageSmmRelocate();
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n\tCPU Online - SMM relocation on new CPU is done\n"));

    // Update SMM Core data structure with the new APIC IDs
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online - Add the new CPU threads to SMM Core data structure \n"));

    for (Index = 0; Index < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); Index++) {
      if (mCpuSmmInitData->SmBase[Index] == mExchangeInfo->CpuSmmInitData.SmBase[Index] &&
        mCpuSmmInitData->ApicId[Index] == INVALID_APIC_ID &&
        mExchangeInfo->CpuSmmInitData.ApicId[Index] != INVALID_APIC_ID) {
        mCpuSmmInitData->ApicId[Index] = mExchangeInfo->CpuSmmInitData.ApicId[Index];
          Status = mSmmCpuServiceProtocol->AddProcessor (mSmmCpuServiceProtocol, mExchangeInfo->CpuSmmInitData.ApicId[Index], &ProcessorNumber);
          ASSERT_EFI_ERROR (Status);
        }
    }
    mCpuRasNextState = CpuRasOnlineExitRas;
  }

  if (mCpuRasNextState == CpuRasOnlineExitRas) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - In ExitRas state \n"));

    //
    // Check the Sanity of Global_PKG_C_S_control,GLOBAL_NID_MAP_REGISTER_0_PCU_FUN2_REG, GLOBAL_NID_MAP_REGISTER_1_PCU_FUN4_REG
    //

    //Global_PKG_C_S_control
    for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
      if (((mCurrentCpuStatus >> SktId) & 0x01) == 1) {
        if (mGlobalPkgCSControl[SktId] != (mCpuCsrAccess->ReadCpuCsr(SktId, 0, GLOBAL_PKG_C_S_CONTROL_REGISTER_PCU_FUN2_REG))) {
          DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nGlobal_PKG_C_S_control of sktid %x is corrupted. so entering the deadloop",SktId));
          EFI_DEADLOOP();
        }
      }
    }


    // Calcualte new GlobalNid value
    mGblNidMap0 = mGblNidMap0 | (1 << (28 + SktIdToBeOl)) | (SktIdToBeOl << (4 * SktIdToBeOl));

    if (SktIdToBeOl > 4) {
      mGblNidMap1 = mGblNidMap1 | (1 << (28 + (SktIdToBeOl - 4))) | (SktIdToBeOl << (4 * (SktIdToBeOl - 4)));
    }

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nNew GlobNid0 is  %x",mGblNidMap0));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nNew GlobNid1 is  %x",mGblNidMap1));

    for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
      if (((mLatestSocketPresentBitMap >> SktId) & 0x01) == 1) {
        Temp = mCpuCsrAccess->ReadCpuCsr(SktId, 0, GLOBAL_NID_SOCKET_0_TO_3_MAP_PCU_FUN2_REG);
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nGlobNid0 of sktid %x is %x",SktId, Temp));

        if (Temp !=  mGblNidMap0) {
          DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nGlobNid0 of sktid %x is corrupted. so entering the deadloop",SktId));
          EFI_DEADLOOP();
        }
      }
    }

    // Update the PAM register of the incoming CPU to the same value as SBSP
    if (RestoreSbspPam == 1) {
      mCpuCsrAccess->WriteCpuCsr(SbspSktId, 0, PAM456_CHABC_SAD_REG, mSbspPam456Reg.Data);
    }
    mCpuCsrAccess->WriteCpuCsr(SktIdToBeOl, 0, PAM456_CHABC_SAD_REG, mSbspPam456Reg.Data);
    Pam456Reg.Data = mCpuCsrAccess->ReadCpuCsr(SktIdToBeOl, 0, PAM456_CHABC_SAD_REG);
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nFinal value of PAM Register of incoming skt %d: is %x\n",SktIdToBeOl,Pam456Reg.Data));

    //Restore GenprotRange0 register
    RestoreGenProtReg();

    // PM Synch reenable  - Restore PM_SYNCH_MODE first and then ADR_EN
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online - Re-Enable the PM synch \n"));
    mCpuCsrAccess->WritePciCsr (0,REG_ADDR(0,PMC_DEVICE, PMC_FUNC, PM_SYNC_MODE, sizeof(UINT32)), mPmSyncData);
    mCpuCsrAccess->WritePciCsr (0,REG_ADDR(0,PMC_DEVICE, PMC_FUNC, ADR_EN, sizeof(UINT32)), mAdrEn);

    if (IsSimicsPlatform() == FALSE) {
      // Program CstEntry again on all the neighbours of the incoming skt
      for (Port = 0; Port < MAX_KTI_PORTS; ++Port) {
        if ( (mIioUdsData->PlatformData.CpuQpiInfo[SktIdToBeOl].PeerInfo[Port].Valid) &&
          (mIioUdsData->PlatformData.CpuQpiInfo[SktIdToBeOl].PeerInfo[Port].PeerSocType == SOCKET_TYPE_CPU )) {

          mLocalPpmStruct->Info->SocketNumber = mIioUdsData->PlatformData.CpuQpiInfo[SktIdToBeOl].PeerInfo[Port].PeerSocId;
          DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - The PeerSocId of the incoming skt is %x\n", mLocalPpmStruct->Info->SocketNumber));
          ProgramCsrPkgCstEntryCriteriaMask((EFI_CPU_PM_STRUCT *)mLocalPpmStruct);
        }
      }
    }

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - Update RST_CPL register on incoming Skt as same value as SBSP \n"));
    SbspRstCplReg.Data =  mCpuCsrAccess->ReadCpuCsr(SbspSktId, 0, BIOS_RESET_CPL_PCU_FUN1_REG);
    RstCplReg.Data = mCpuCsrAccess->ReadCpuCsr(SktIdToBeOl, 0, BIOS_RESET_CPL_PCU_FUN1_REG);

    if (SbspRstCplReg.Bits.rst_cpl2 == 1) {
      RstCplReg.Bits.rst_cpl2 = 1;
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n:CPU Online - BIOS_RESET_CPL: Set CPL2.\n"));
      mCpuCsrAccess->WriteCpuCsr(SktIdToBeOl, 0, BIOS_RESET_CPL_PCU_FUN1_REG,RstCplReg.Data);
      if (SbspRstCplReg.Bits.rst_cpl3 == 1) {
        RstCplReg.Bits.rst_cpl3 = 1;
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n:CPU Online - BIOS_RESET_CPL: Set CPL3.\n"));
        mCpuCsrAccess->WriteCpuCsr(SktIdToBeOl, 0, BIOS_RESET_CPL_PCU_FUN1_REG,RstCplReg.Data);

        RstCplReg.Bits.rst_cpl4 = 1;
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n:CPU Online - BIOS_RESET_CPL: Set CPL4.\n"));
        mCpuCsrAccess->WriteCpuCsr(SktIdToBeOl, 0, BIOS_RESET_CPL_PCU_FUN1_REG,RstCplReg.Data);
      }
    }

    // CATERR enable on the new sokcet
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online - Enable CATERR on new CPU \n"));
    Status = mHpIOXAccess->IssuePldCmdThroIox (ISSUE_BIOS_READY, SktIdToBeOl, TRUE);

    // Exit RAS
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Online - Do ExitRAS command on existing CPUs \n"));

    for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
      if (((mCurrentCpuStatus >> SktId) & 0x01) == 1) {
        // Notify ME
        PlatformHookNotifyME(mCpuRas->RASEvent, RasNotificationDone);
        Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(SktId, MAILBOX_BIOS_CMD_RAS_EXIT, 0);
      }
    }

    // Notify BMC
    PlatformHookNotifyBmc(mCpuRas->RASEvent, RasEventEnd);

    mCpuRasNextState = CpuRasUpdateAcpiList;

  }

  if (mCpuRasNextState == CpuRasUpdateAcpiList) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - UpdateACPIList state \n"));


    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - Add everything to the IPI list \n"));
    mRasMpLink->RasMpLinkAdjustIpiList(2);

    // Set InitFlag to '2' to support another CPU online.
    mExchangeInfo->InitFlag = 2;

    //Calculate ProcessorBit Mask and send all the theads to CLI+HLT loop
    CoreThreadBitMap = 0;
    for (Index = 0; Index < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); Index++) {
      if (mExchangeInfo->BistBuffer[Index].Number != 0) {
        ApicId = (UINT8)mExchangeInfo->BistBuffer[Index].BIST;
        CoreThreadId = ApicId & 0x1F;
        CoreThreadBitMap = CoreThreadBitMap | (UINT64)(1 << CoreThreadId);
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nThe Bit map of %x is %x\n",mExchangeInfo->BistBuffer[Index].BIST, CoreThreadBitMap));
        SendInitSipiSipi (ApicId, (UINT32)mStartupVector);
      }
    }

    // Commuincate State through ASL through AcpiParameter table.
    mAcpiParameter->SocketBitMask |= (1 << SktIdToBeOl);
    mAcpiParameter->ProcessorBitMask[SktIdToBeOl] = CoreThreadBitMap;

    // Register which device(s) have a pending online/offline request
    mAcpiParameter->CpuChangeMask = (1 << SktIdToBeOl);

    // Remove the newly onlined processor from the list of available spare CPUs.
    mAcpiParameter->CpuSpareMask &= ~(1 << SktIdToBeOl);

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - SocketBitMask is %x", mAcpiParameter->SocketBitMask));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - ProcessorBitMask  of %x is %x", SktIdToBeOl, mAcpiParameter->ProcessorBitMask[SktIdToBeOl]));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - CpuChangeMask is %x", mAcpiParameter->CpuChangeMask));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Online - CpuSpareMask is %x", mAcpiParameter->CpuSpareMask));

    Status = STATUS_DONE;
    return Status;
  }

  return Status;
}

EFI_STATUS
CpuRasOffline (
)
{

  EFI_STATUS      Status;
  UINT8           SktIdToBeOl;
  UINT32          LocalSktId;
  UINT8           SktId;
  UINT32          PmSyncData;
  UINT32          ApicIdValue = 0;
  UINT8           Index, Index2;
  UINT8           MemNodePresent;
  EFI_PROCESSOR_INFORMATION			  ProcessorInfoBuffer;

  Status = IN_PROGRESS;
  SktIdToBeOl = mCpuRas->SocketToBeOl;

  if (mCpuRas->FirstCpuRasEntry) {
    mCpuRas->FirstCpuRasEntry = 0;
    mCpuRasNextState = CpuOfflineSanityCheck;
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nFirst CPU Offline Entry\n"));
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU OFFLINE - SktId To Be Offlined = %x\n",SktIdToBeOl));
    mCurrentCpuStatus = mRasMpLink->Host->var.common.socketPresentBitMap;
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU Offline - CurrentCpuStatus = %x\n",mCurrentCpuStatus));

  }

  if (mCpuRasNextState == CpuOfflineSanityCheck) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU Offline : Sanity Check for memory risers\n"));

    for (Index = 0; Index < MAX_IMC; Index++) {
      Status = mMemRas->GetCurrentMemoryNodeState(SKTMC_TO_NODE(SktIdToBeOl, Index), &MemNodePresent);
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t CPU Offline: Memory Riser %d of %d is %d\n",Index, SktIdToBeOl,MemNodePresent));
      if(MemNodePresent == 1) {
        DEBUG((EFI_D_LOAD|EFI_D_INFO, "\t CPU CANNOT be offlined, as Memory Riser is present on the CPU\n So, returning with NO Support"));
        return NO_SUPPORT;
      }
    }

    // check if the QPI RAS code can offline the socket

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "CPU Offline : Sanity Check for QPI\n"));
    Status = mRasMpLink->RasMpLinkCheckEvent(RAS_MPLINK_EVNT_OFFLINE, SktIdToBeOl);

    if (Status == RAS_MPLINK_PENDING) {
      return IN_PROGRESS;
    }

    if (Status == RAS_MPLINK_SUCCESS) {
      mCpuRasNextState = CpuOfflineRasStart;
      return IN_PROGRESS;
    } else {
      return ERROR_EXIT;
    }
  }

  if (mCpuRasNextState == CpuOfflineRasStart) {

    if (mCpuRasStartDone == 0 ) {

      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : In RasStart State\n"));

      // Notify BMC
      PlatformHookNotifyBmc(mCpuRas->RASEvent, RasEventStart);

      // RAS Start
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : Do RAS Start on existing sockets\n"));
      for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
        if (((mCurrentCpuStatus >> SktId) & 0x01) == 1) {
          Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(SktId, MAILBOX_BIOS_CMD_RAS_START, 0);
        }
      }


      // PM Synch disable
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : Do PM Synch disable\n"));
      mCpuCsrAccess->WritePciCsr (0, REG_ADDR(0, PCI_DEVICE_NUMBER_PCH_PM, PCI_FUNCTION_NUMBER_PCH_PM, R_PCH_PM_SYNC_MODE, sizeof(UINT32)), 0);
      MicroSecondDelay  (5);
      PmSyncData = mCpuCsrAccess->ReadPciCsr (0, REG_ADDR(0,PCI_DEVICE_NUMBER_PCH_PM, PCI_FUNCTION_NUMBER_PCH_PM, R_PCH_PM_SYNC_MODE, sizeof(UINT32)));
      MicroSecondDelay  (1);

      mCpuRasStartDone = 1;
    }

    mCpuRasNextState = CpuOfflineChangeMonarch;
    return IN_PROGRESS;
  }

  if (mCpuRasNextState == CpuOfflineCheckSci) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : In CheckSCI State\n"));

    // check if the socket is ejected by OS or not.  If not generate SCI to make OS eject the socket
    if (mAcpiParameter->SciRequestParam[0] == 0) {
      mAcpiParameter->SciRequestParam[0]   = SCI_EJECTION;
      mAcpiParameter->SciRequestParam[1]   = 0;
      mAcpiParameter->SciRequestParam[2]   = 1 << SktIdToBeOl;
      mAcpiParameter->SciRequestParam[3]   = 0;
      mAcpiParameter->CpuChangeMask = 1 << SktIdToBeOl;

      // Generate SCI to tell OS to issue EJECT command.
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : Generate SCI to eject the CPU from OS\n"));
      GenerateRasSci();

      return STATUS_DONE;

    } else if (mAcpiParameter->SciRequestParam[0] == SCI_EJECTION) {
      mCpuRasNextState = CpuOfflineChangeMonarch;
    } else {
     return STATUS_DONE;
    }
  }

  if (mCpuRasNextState == CpuOfflineChangeMonarch) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : In ChangeMonarch state\n"));


    LocalSktId = GetApicId() >> 5;
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline Local Skt Id: %d \n", LocalSktId));

    if (LocalSktId != SktIdToBeOl) {

      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : Change Monarch is not required\n"));

      // Remove the threads of the socket to be offlined from the SMM core data.
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : Remove the processor from SMM Core data and send them to Init State\n\t"));
      for (Index = 0; Index < MAX_CORE * MAX_THREAD; Index++) {
        if ((mAcpiParameter->ProcessorBitMask[SktIdToBeOl] >> Index) & 0x01) {
          ApicIdValue = mAcpiParameter->ProcessorApicIdBase[SktIdToBeOl] + Index;

        for (Index2 = 0; Index2 < gSmst->NumberOfCpus; Index2++) {
          Status = mSmmCpuServiceProtocol->GetProcessorInfo (mSmmCpuServiceProtocol, Index2, &ProcessorInfoBuffer);
          if (Status == EFI_SUCCESS && ProcessorInfoBuffer.ProcessorId == ApicIdValue) {
              DEBUG((EFI_D_LOAD|EFI_D_INFO, "The APIC id of the processor is %x\n", ApicIdValue));
            break;
          }
        }

        Status = mSmmCpuServiceProtocol->RemoveProcessor (mSmmCpuServiceProtocol, Index2);
        ASSERT_EFI_ERROR (Status);

        // Send the thread to Init State.
        SendInitIpi (ApicIdValue);
      }
}

      mCpuRasNextState = CpuRasOfflineQpiLinkUpdate;
      Status = IN_PROGRESS;
      return  Status;

    } else {
      // << TO-DO -- need to find how to change monarch
    }
  }

  if (mCpuRasNextState == CpuRasOfflineQpiLinkUpdate) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : In QpiLinkUpdate State"));

    // Call RasMpLink Hander to cut off the links and power down the socket
    Status = mRasMpLink->RasMpLinkHandler (RAS_MPLINK_EVNT_OFFLINE, SktIdToBeOl);

    if (Status == RAS_MPLINK_PENDING) {
      return IN_PROGRESS;
    }

    if (Status == RAS_MPLINK_SUCCESS) {
      mCpuRasNextState = CpuOfflineRasExit;
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : QpiLinkUpdate SUCCESS"));
      return IN_PROGRESS;
    } else {
      DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : QpiLinkUpdate ERROR, so returning with ERROR_EXIT"));
      return ERROR_EXIT;
    }
  }

  if (mCpuRasNextState == CpuOfflineRasExit) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : In RasExit State\n"));

    // Update SocketBitMask, ProcessorBitMask, CpuChangeMask of mAcpiParameter to communicate with the ASL code
    mCurrentCpuStatus = mAcpiParameter->SocketBitMask &= ~(1 << SktIdToBeOl);
    mAcpiParameter->ProcessorBitMask[SktIdToBeOl] = 0x00;
    mAcpiParameter->CpuChangeMask = (1 << SktIdToBeOl);

    // Exit RAS
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : Do Ras Exit"));
    for (SktId = 0; SktId < MAX_SOCKET; SktId++) {
      if (((mCurrentCpuStatus >> SktId) & 0x01) == 1) {
        Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(SktId, MAILBOX_BIOS_CMD_RAS_EXIT, 0);
      }
    }

    // PM Synch reenable
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : ReEnable PM Synch"));
    PmSyncData = mCpuCsrAccess->ReadPciCsr (0,REG_ADDR( 0, PCI_DEVICE_NUMBER_PCH_PM, PCI_FUNCTION_NUMBER_PCH_PM, R_PCH_PM_SYNC_MODE, sizeof(UINT32)));
    PmSyncData = PmSyncData | GPIO_B_PM_SYNC_EN;
    PmSyncData = PmSyncData | GPIO_B_C0_TX_EN;
    PmSyncData = PmSyncData | REG_10_C0_TX_EN;
    mCpuCsrAccess->WritePciCsr (0, REG_ADDR(0,PCI_DEVICE_NUMBER_PCH_PM, PCI_FUNCTION_NUMBER_PCH_PM, R_PCH_PM_SYNC_MODE, sizeof(UINT32)), PmSyncData);

    // Notify BMC
    PlatformHookNotifyBmc(mCpuRas->RASEvent, RasEventEnd);

    mCpuRasNextState = CpuOfflineNotifyOS;
    return IN_PROGRESS;
  }

  if (mCpuRasNextState == CpuOfflineNotifyOS) {

     DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU Offline : In NotifyOS State\n"));

    // Add the newly offlined processor to the list of available spare CPUs
    mAcpiParameter->CpuSpareMask |= (1 << SktIdToBeOl);

    //  Request OS to check the device
    mAcpiParameter->SciRequestParam[0]   = SCI_CHECK;
    mAcpiParameter->SciRequestParam[1]   = 0;
    mAcpiParameter->SciRequestParam[2]   = 0;
    mAcpiParameter->SciRequestParam[3]   = 0;

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\tCPU Offline : Generate SCI to aks OS to check the device\n"));

    GenerateRasSci();

    return STATUS_DONE;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
InitializeCpuRasHandler (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_HANDLE                Handle = NULL;
  EFI_GLOBAL_NVS_AREA_PROTOCOL *AcpiNvsProtocol = NULL;

  mIioUdsData         = NULL;

  // Make sure the RAS driver is not initialized already
  Status = gSmst->SmmLocateProtocol (&gEfiCpuRasProtocolGuid, NULL, &mCpuRas);
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol  (&gEfiCpuHotAddDataProtocolGuid, NULL, &mCpuHotAddData);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiRasMpLinkProtocolGuid, NULL, &mRasMpLink);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &mMemRas);
  ASSERT_EFI_ERROR(Status);

  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  &AcpiNvsProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  mAcpiParameter = AcpiNvsProtocol->Area;
  ASSERT (mAcpiParameter);


  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);
  mIioUdsData = (IIO_UDS *)mIioUds->IioUdsPtr;

  Status = gSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuServiceProtocolGuid, NULL, &mSmmCpuServiceProtocol);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiHpIoxAccessGuid, NULL, &mHpIOXAccess);
  ASSERT_EFI_ERROR (Status);

  // Allocate the buffer for protocol data structure
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof(EFI_CPU_RAS_PROTOCOL), &mCpuRas);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gSpsRasNotifyProtocolGuid, NULL,&mSpsRasNotifyProtocol);

  mCpuRas->CpuRasOnline = CpuRasOnline;
  mCpuRas->CpuRasOffline = CpuRasOffline;

  // Install the protocol to expose the APIs
  Status = gSmst->SmmInstallProtocolInterface (&Handle, &gEfiCpuRasProtocolGuid, EFI_NATIVE_INTERFACE, mCpuRas);
  ASSERT_EFI_ERROR (Status);

  mAcpiCpuData = (ACPI_CPU_DATA *)(UINTN)PcdGet64 (PcdCpuS3DataAddress);
  mExchangeInfo   = mCpuHotAddData->MpExchangeInfoPtr;
  mStartupVector  = mCpuHotAddData->StartupVector;
  mLocalPpmStruct = mCpuHotAddData->CpuPmStruct;
  mSmmStartupcodeArea = mCpuHotAddData->SmmStartupCodeArea;

  mCpuSmmInitData = (CPU_HOT_PLUG_DATA *)(UINTN) PcdGet64 (PcdCpuHotPlugDataAddress);



  return EFI_SUCCESS;
}

EFI_STATUS
GenerateRasSci (
    VOID
)
/*++

Routine Description:
    This function generates an SCI interrupt for Ras OS Consumption

Arguments:
    None

Returns:
    None

--*/
{
  UINT32  GpeCtrl = 0;
  UINT16 PmBase = 0;

  PmBase = MmioRead16 (MmPciBase (PMC_BUS, PMC_DEVICE, PMC_FUNC) + R_PCH_PMC_ACPI_BASE); 
  PmBase = (UINT16) (PmBase & 0xFF80);

  // Generate SCI event
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_ACPI_GPE_CNTL), 1, &GpeCtrl); 
  GpeCtrl |= B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL; 
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_ACPI_GPE_CNTL), 1, &GpeCtrl); 
  return EFI_SUCCESS;
}

