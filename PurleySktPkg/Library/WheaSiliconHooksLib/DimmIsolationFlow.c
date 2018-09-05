//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//
//***************************************************************************
/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2009-2016, Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file DimmIsolationFlow.c

    Faulty dimm isolation flow implementation file.

**/

#include "DimmIsolationFlow.h"
#include <Protocol/CrystalRidge.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include "Token.h"	//SMCPKG_SUPPORT

extern EFI_WHEA_BOOT_PROTOCOL      *mWheaBootProtocolLib;
extern EFI_CRYSTAL_RIDGE_PROTOCOL  *m2CrystalRidgeProtocol;
extern EFI_MEM_RAS_PROTOCOL        *mMemRasLib;
extern EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;

EFI_PLATFORM_RAS_POLICY_PROTOCOL   *mPlatformRasPolicyProtocolLib;

POISON_TRACK      PoisonTrack;
//READ_MSR          MsrReader;
READ_CSR          CsrReader;
LOG_ACTION        UcnaLogger;
LOG_ACTION        SraoLogger;
LOG_ACTION        DueLogger;
LOG_ACTION        CeLogger;





MCE_CLASS
GetMceClass (
  CONST IA32_MCI_STATUS_MSR_STRUCT * CONST pMcStatus
  )
{
  UINT64 McgCap;
  McgCap = AsmReadMsr64(EFI_IA32_MCG_CAP);

  if (pMcStatus->Bits.val == 1) {
    if (pMcStatus->Bits.uc == 1 && pMcStatus->Bits.en == 1) {
      if (pMcStatus->Bits.pcc == 0) {
        if(BitFieldRead64(McgCap,N_MSR_MCG_MCG_SER_P, N_MSR_MCG_MCG_SER_P)) {
          if (pMcStatus->Bits.s == 1) {
            if(pMcStatus->Bits.ar == 1) {
              return SRAR;
            } else {
              return SRAO;
            }
          } else {
            return UCNA;
          }
        } else {
          return FATAL;
        }
      } else {
         return FATAL;
      }
    } else {
      return CORRECTED;
    }
  } else {
    return INVALID;
  }
}

MCE_SOURCE
GetMceSource (
  UINT16  McBankNum
  )
{
  if (McBankNum == CORE_IFU_BANK) {
    return IFU;
  }

  if (McBankNum == CORE_DCU_BANK) {
    return DCU;
  }

  if ((McBankNum <= MEM_IMC_MCBANK_NUM_END) && (McBankNum >= MEM_IMC_MCBANK_NUM_START)) {
    return IMC;
  }

  if ((McBankNum <= MEM_M2M_MCBANK_NUM_END) && (McBankNum >= MEM_M2M_MCBANK_NUM_START)) {
    return M2M;
  }

  return DONT_CARE;
}

EFI_STATUS
MemInfoToPoisonRecord (
  CONST MEMORY_DEV_INFO * CONST pMemInfo,
  POISON_RECORD * CONST pRecord
  )
{
  UINT64           ValidFlag = (PLATFORM_MEM_ERROR_STATUS_VALID | PLATFORM_MEM_PHY_ADDRESS_VALID | PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID | PLATFORM_MEM_MODULE_VALID);

  if (pMemInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((pMemInfo->ValidBits & ValidFlag) != ValidFlag) {
    return EFI_NOT_READY;
  }

  pRecord->Address = pMemInfo->PhyAddr;
  pRecord->Socket  = (UINT8)NODE_TO_SKT(pMemInfo->Node);
  pRecord->Mc      = (UINT8)NODE_TO_MC(pMemInfo->Node);
  pRecord->Ch      = (UINT8)pMemInfo->Channel;
  pRecord->DIMM    = (UINT8)pMemInfo->Dimm;
  pRecord->Valid   = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
PoisonRecordToMemInfo (
  CONST POISON_RECORD *   CONST pRecord,
  MEMORY_DEV_INFO * CONST pMemInfo

  )
{
  if (!pRecord->Valid) {
    return EFI_INVALID_PARAMETER;
  }

  pMemInfo->PhyAddr = pRecord->Address;
  pMemInfo->Node    = SKT_TO_NODE(pRecord->Socket,pRecord->Mc);
  pMemInfo->Channel = pRecord->Ch;
  pMemInfo->Dimm    = pRecord->DIMM;
  pMemInfo->ErrorType = PLATFORM_MEM_ERROR_MLTIBIT_ECC;
  pMemInfo->ValidBits |= (PLATFORM_MEM_ERROR_STATUS_VALID | PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID | PLATFORM_MEM_MODULE_VALID);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LookUpPoisonRecord(
  CONST MCBANK_ERR_INFO * CONST pMcBankInfo,
  OUT   MEMORY_DEV_INFO * CONST pMemInfo
  )
{
  UINT32           i;
  POISON_RECORD   *pPoisonRec;

  if (pMemInfo == NULL || pMcBankInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < MAX_POISON_RECORD; i++) {
    pPoisonRec = PoisonTrack.Overflow ? &(PoisonTrack.Records[(i + PoisonTrack.Pivot) % MAX_POISON_RECORD]) : &(PoisonTrack.Records[i]);

    if (!pPoisonRec->Valid) {
      return EFI_NOT_FOUND;
    }

    if ((pPoisonRec->Address & CACHELINE_MASK) == (pMemInfo->PhyAddr & CACHELINE_MASK)) {
      PoisonRecordToMemInfo(pPoisonRec, pMemInfo);
      return EFI_SUCCESS;
    }

  }
  return EFI_NOT_FOUND;
}


EFI_STATUS
EFIAPI
SetPoisonRecord (
  MEMORY_DEV_INFO * pMemInfo
  )
{
  UINT32           i;
  UINT64           ValidFlag = (PLATFORM_MEM_ERROR_STATUS_VALID | PLATFORM_MEM_PHY_ADDRESS_VALID | PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID | PLATFORM_MEM_MODULE_VALID);
  POISON_RECORD   *pPoisonRec;
  pPoisonRec = NULL;

  RASDEBUG((DEBUG_ERROR,"SetPoisonRecord\n"));

  if ((pMemInfo->ValidBits & ValidFlag) != ValidFlag) {
    RASDEBUG ((DEBUG_INFO, "SetPoisonRecord: no valid error status/address/socket/channel/dimm \n"));
    return EFI_NOT_READY;
  }

  for (i = 0; i < MAX_POISON_RECORD; i++) {
    pPoisonRec = PoisonTrack.Overflow ? &(PoisonTrack.Records[(i + PoisonTrack.Pivot) % MAX_POISON_RECORD]) : &(PoisonTrack.Records[i]);
    if (!pPoisonRec->Valid) { // First non-occupied
      MemInfoToPoisonRecord(pMemInfo, pPoisonRec);
      PoisonTrack.Pivot = (PoisonTrack.Pivot++) % MAX_POISON_RECORD;
      return EFI_SUCCESS;
    }

    if ((pPoisonRec->Address & CACHELINE_MASK) == (pMemInfo->PhyAddr & CACHELINE_MASK)) { //Poison addr already logged
      return EFI_SUCCESS;
    }
  }

  if (i == MAX_POISON_RECORD) { //Record FIFO overflow
    PoisonTrack.Overflow = TRUE;
    MemInfoToPoisonRecord(pMemInfo, pPoisonRec);
    PoisonTrack.Pivot = (PoisonTrack.Pivot++) % MAX_POISON_RECORD;
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FinalizeUELog (
  MEMORY_DEV_INFO * pMemInfo
  )
{
  RASDEBUG((DEBUG_ERROR,"FinalizeUELog\n"));
  if (pMemInfo == NULL) {
    RASDEBUG ((DEBUG_INFO, "CreateWheaLog: pMemInfo should not be zero\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (pMemInfo->ErrorType == PLATFORM_MEM_ERROR_SCRUB_UNCOR) {
    RASDEBUG((DEBUG_ERROR,"SetPoisonRecord for SRAO error\n"));
    SetPoisonRecord (pMemInfo);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FinalizeCELog (
  MEMORY_DEV_INFO * pMemInfo
  )
{
  if (pMemInfo == NULL) {
    RASDEBUG ((DEBUG_INFO, "CreateWheaLog: pMemInfo should not be zero\n"));
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetRetryRdErrorLog (
  UINT8                 Socket,
  UINT8                 Ch,
  PTRANSLATED_ADDRESS   TA
    )
{
  RETRY_RD_ERR_LOG_MCDDC_DP_STRUCT            RetryRdErrLog;
  RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_STRUCT   RetryRdErrLogAddr1;
  RETRY_RD_ERR_LOG_ADDRESS2_MCDDC_DP_STRUCT   RetryRdErrLogAddr2;

  RetryRdErrLog.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, RETRY_RD_ERR_LOG_MCDDC_DP_REG);
  if (!RetryRdErrLog.Bits.v) {
    return EFI_NOT_READY;
  }

  RetryRdErrLogAddr1.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, RETRY_RD_ERR_LOG_ADDRESS1_MCDDC_DP_REG);
  RetryRdErrLogAddr2.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, RETRY_RD_ERR_LOG_ADDRESS2_MCDDC_DP_REG);

  TA->SocketId   = Socket;
  TA->ChannelId  = Ch;
  TA->DimmSlot   = (UINT8)(RetryRdErrLogAddr1.Bits.chip_select / 4); //Purley is 2DPC. CS / 4 shuold be the DIMM id
  TA->ChipSelect = (UINT8)RetryRdErrLogAddr1.Bits.chip_select;
  TA->Col        = RetryRdErrLogAddr1.Bits.col;
  //retry_rd_err_log_address1.bank : Bank ID for last retry. Bottom two bits are Bank Group, top two bits are Bank Address
  TA->Bank       = (UINT8)(RetryRdErrLogAddr1.Bits.bank & 0x3);
  TA->BankGroup  = (UINT8)((RetryRdErrLogAddr1.Bits.bank & 0xC0) >> 2);
  TA->Row        = RetryRdErrLogAddr2.Bits.row;
  TA->ChipSelect = (UINT8)RetryRdErrLogAddr1.Bits.chip_select;
  return EFI_SUCCESS;
}

EFI_STATUS
CheckDDRTError(
  MEMORY_DEV_INFO    *pMemInfo,
  UINTN               ErrorSpa,
  LOG_ACTION          Logger
  )
{
  EFI_STATUS            Status;
  UINT8                 i;
  UINT8                 CountToCheck = 4;
  UINT8                 Socket;
  UINT8                 Ch;
  UINT8                 Dimm;
  UINT8                 XlatSocket;
  UINT8                 XlatCh;
  UINT8                 XlateDimm;
  UINT64                ValidFlag;
  UINTN                 ErrorDpa = 0;
  TRANSLATED_ADDRESS    RetryAddr;
  MEMORY_DEV_INFO       MemInfo;
  MEDIA_ERR_LOG         *pMediaLog;
  MEDIA_ERR_LOG_DATA    MediaLogData;

  if (pMemInfo == NULL) {
    RASDEBUG ((DEBUG_INFO, "UpdateDDRTError: pMemDevInfo should not be zero\n"));
    return EFI_INVALID_PARAMETER;
  }

  pMediaLog = NULL;
  CopyMem (&MemInfo, pMemInfo, sizeof(MEMORY_DEV_INFO));

  ValidFlag = (PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID | PLATFORM_MEM_MODULE_VALID);

  // MemInfo should be valid Socket, Ch and Dimm (Node, Card and Dimm) info.
  if ((MemInfo.ValidBits & ValidFlag) != ValidFlag) {
    RASDEBUG ((DEBUG_INFO, "CheckDDRTError: MemInfo should be valid Socket, Ch and Dimm (Node, Card and Module) info.\n"));
    return EFI_INVALID_PARAMETER;
  }

  Socket = (UINT8)NODE_TO_SKT(MemInfo.Node);
  Ch     = (UINT8)MemInfo.Channel;
  Dimm   = (UINT8)MemInfo.Dimm;

  //Translate ErrorSpa to Dpa for checking later.
  Status = m2CrystalRidgeProtocol->SpaToNvmDpa(ErrorSpa, &XlatSocket, &XlatCh, &XlateDimm, &ErrorDpa);
  if (EFI_ERROR(Status)) { //We really should not fail at this point. This fail to translate SPA to DPA means we have an invalid memory map (?) or bad xlation implementation.
    RASDEBUG((DEBUG_ERROR, "CheckDDRTError: Failed to perform Spa to Dpa translated at specified addrress %x, Socket = %d, Ch = %d, Dimm = %d\n", ErrorSpa, Socket, Ch, Dimm));
    MemInfo.ValidBits &= ~(PLATFORM_MEM_MODULE_VALID | PLATFORM_MEM_BANK_VALID | PLATFORM_MEM_ROW_VALID | PLATFORM_MEM_COLUMN_VALID); // Clear out dimm info. due to bad spa to dpa translation.
    return Logger(&MemInfo);
  }

  //Get High Priority Log
  Status = m2CrystalRidgeProtocol->NvmCtlrGetLatestErrorLog(Socket, Ch, Dimm, HIGH_LEVEL_LOG, MEDIA_LOG_REQ, CountToCheck, &MediaLogData);
  if (EFI_ERROR(Status)) {
    RASDEBUG((DEBUG_ERROR, "CheckDDRTError: Failed to GetLatestErrorLog. Socket = %d, Ch = %d, Dimm = %d\n", ErrorSpa, Socket, Ch, Dimm));
    return Status;
  }

  for (i = 0; i < MediaLogData.ReturnCount; i++) {
    if (MediaLogData.MediaLogs[i].Dpa == ErrorDpa) {
      pMediaLog = &MediaLogData.MediaLogs[i];
      break;
    }
  }

  if (!pMediaLog) { //No match in high priority. Get low priority log
    Status = m2CrystalRidgeProtocol->NvmCtlrGetLatestErrorLog(Socket, Ch, Dimm, LOW_LEVEL_LOG, MEDIA_LOG_REQ, CountToCheck, &MediaLogData);
    for (i = 0; i < MediaLogData.ReturnCount; i++) {
      if (MediaLogData.MediaLogs[i].Dpa == ErrorDpa) {
        pMediaLog = &MediaLogData.MediaLogs[i];
        break;
      }
    }
  }

  if (pMediaLog) { //Insert DDRT Error Log here
    if (pMediaLog->TransactionType == FNV_TT_PATROL_SCRUB) {
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_SCRUB_UNCOR; // TODO: 0xE is EFI CPER Scrub UC type in CPER. Need to replace it with macro define
    }
    return Logger(&MemInfo);
  }

  //No DPA match here or no valid FNV error entry. Let's check retry_rd_err_log
  Status = GetRetryRdErrorLog(Socket, Ch, &RetryAddr);

  if (EFI_ERROR(Status) || (Dimm != RetryAddr.DimmSlot)) { //No valid retry_rd_err_log or dimm does not match
    MemInfo.ValidBits &= ~(PLATFORM_MEM_MODULE_VALID | PLATFORM_MEM_BANK_VALID | PLATFORM_MEM_ROW_VALID | PLATFORM_MEM_COLUMN_VALID); // Clear out dimm info. due to bad spa to dpa translation.
    return Logger(&MemInfo);
  }

  return Logger(&MemInfo);
}

EFI_STATUS
CheckDDR4Error (
  MEMORY_DEV_INFO       *pMemInfo,
  LOG_ACTION            Logger
  )
{
  EFI_STATUS            Status;
  TRANSLATED_ADDRESS    RetryAddr;
  UINT8                 Socket;
  UINT8                 Ch;
  UINT64                ValidFlag;

  if (pMemInfo == NULL) {
    RASDEBUG ((DEBUG_INFO, "UpdateNMError: pMemDevInfo should not be zero\n"));
    return EFI_INVALID_PARAMETER;
  }

  ValidFlag = (PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID);

  // MemInfo should be valid Socket and Ch (Node and Card) info.
  if ((pMemInfo->ValidBits & ValidFlag) != ValidFlag) {
    RASDEBUG ((DEBUG_INFO, "UpdateNMError: MemInfo should be valid Socket and Ch (Node and Card) info.\n"));
    return EFI_INVALID_PARAMETER;
  }

  Socket = (UINT8)NODE_TO_SKT(pMemInfo->Node);
  Ch = (UINT8)pMemInfo->Channel;

  Status = GetRetryRdErrorLog(Socket, Ch, &RetryAddr);
  if (EFI_ERROR(Status)) { //No valid retry_rd_err_log. Log up to ch level.
    return Logger(pMemInfo);
  }

  pMemInfo->Dimm   = RetryAddr.DimmSlot;
  pMemInfo->Bank   = RetryAddr.Bank;
  pMemInfo->Column = RetryAddr.Col;
  pMemInfo->Row    = RetryAddr.Row;
  pMemInfo->Rank   = RetryAddr.DimmRank;

  pMemInfo->ValidBits |= (PLATFORM_MEM_MODULE_VALID | PLATFORM_MEM_BANK_VALID | PLATFORM_MEM_ROW_VALID | PLATFORM_MEM_COLUMN_VALID | PLATFORM_RANK_NUMBER_VALID);
  if (RetryAddr.Row >> 16) {
    pMemInfo->ValidBits &= ~PLATFORM_MEM_ROW_VALID;          // Row number will not fit -> set old row number invalid
    // APTIOV_SERVER_OVERRIDE_RC_START
    pMemInfo->ValidBits |= PLATFORM_MEM_EXTENDED_ROW_VALID;  // Set row overflow valid bit 18
    pMemInfo->Extended = (UINT8)(RetryAddr.Row >> 16) & 0x03;       // Drop-in bits 16,17
    // APTIOV_SERVER_OVERRIDE_RC_END
  }
  pMemInfo->Bank = (UINT16)(RetryAddr.BankGroup << 8) | RetryAddr.Bank;                 // Drop-in bits 15:8
  pMemInfo->ValidBits |= (PLATFORM_MEM_BANK_GROUP_VALID | PLATFORM_MEM_BANK_ADDRESS_VALID);
  pMemInfo->ValidBits &= ~PLATFORM_MEM_BANK_VALID;
  // APTIOV_SERVER_OVERRIDE_RC_START
  pMemInfo->Extended  |= (UINT8)(RetryAddr.ChipSelect << 5);   // Merge extended row bits and chip select
  // APTIOV_SERVER_OVERRIDE_RC_END
  pMemInfo->ValidBits |= PLATFORM_MEM_CHIP_IDENTIFICATION_VALID;

  return Logger(pMemInfo);
}


EFI_STATUS
EFIAPI
CheckCAPError (
  MEMORY_DEV_INFO            * CONST pMemInfo,
  CONST IA32_MCI_STATUS_MSR_STRUCT   McStatus

  )
{
  CORRERRORSTATUS_MCDDC_DP_STRUCT     CorrErrSts;
  UINT8                               Socket;
  UINT8                               Ch;
  UINT64                              ValidFlag;

  if (pMemInfo == NULL) {
    RASDEBUG ((DEBUG_INFO, "CheckCAPError: pMemDevInfo should not be NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  ValidFlag = (PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID);

  // MemInfo should be valid Socket and Ch (Node and Card) info.
  if ((pMemInfo->ValidBits & ValidFlag) != ValidFlag) {
    RASDEBUG ((DEBUG_INFO, "CheckCAPError: MemInfo should be valid Socket and Ch (Node and Card) info.\n"));
    return EFI_INVALID_PARAMETER;
  }

  Socket = (UINT8)NODE_TO_SKT(pMemInfo->Node);
  Ch = (UINT8)pMemInfo->Channel;

  if (McStatus.Bits.ms_code == 0x200 && (McStatus.Bits.mca_code & BIT7)) { //DDR4 CAP or WrCRC
	CorrErrSts.Data = mCpuCsrAccess->ReadCpuCsr(Socket, Ch, CORRERRORSTATUS_MCDDC_DP_REG);

	if ((CorrErrSts.Bits.dimm_alert & ( BIT1 | BIT0 )) == 0) { //No dimm_alert set. could be WrCRC case.
	  pMemInfo->ErrorType = PLATFORM_MEM_ERROR_UNKNOWN;
	  return EFI_SUCCESS;
	} else {

      pMemInfo->ValidBits |= PLATFORM_MEM_MODULE_VALID;

      if (CorrErrSts.Bits.dimm_alert & BIT0) {
	    pMemInfo->Dimm = 0;
	  } else if(CorrErrSts.Bits.dimm_alert & BIT1) {
	    pMemInfo->Dimm = 1;
	  }
      return EFI_SUCCESS;
	}
  } else if(McStatus.Bits.ms_code == 0x80 && (McStatus.Bits.mca_code & BIT7)) { //DDRT CAP/WrCRC
	pMemInfo->ValidBits |= PLATFORM_MEM_MODULE_VALID;
	pMemInfo->Dimm = m2CrystalRidgeProtocol->IsNvmDimm(Socket, Ch, 0) ? 0 : 1; //AEP should be either on DIMM 0 or DIMM 1
	return EFI_SUCCESS;
  } else {
	return EFI_NOT_FOUND;
  }

}
EFI_STATUS
CheckM2MUCNA (
  IN   MEMORY_DEV_INFO              *pMemInfo,
  OUT  IA32_MCI_STATUS_MSR_STRUCT   *McBankSts
  )
{
  UINT8                                   Socket;
  UINT8                                   McId;
  UINT64                                  Address;
  MCI_STATUS_SHADOW_N0_M2MEM_MAIN_STRUCT  Mc5Shadow0Status;
  MCI_STATUS_SHADOW_N1_M2MEM_MAIN_STRUCT  Mc5Shadow1Status;
  MCI_ADDR_SHADOW0_M2MEM_MAIN_STRUCT      Mc5Shadow0Addr;
  MCI_ADDR_SHADOW1_M2MEM_MAIN_STRUCT      Mc5Shadow1Addr;

  Socket = (UINT8)NODE_TO_SKT(pMemInfo->Node);
  McId = (UINT8)NODE_TO_MC(pMemInfo->Node);

  Mc5Shadow0Status.Data  = mCpuCsrAccess->ReadCpuCsr(Socket, McId, MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG);
  Mc5Shadow1Status.Data  = mCpuCsrAccess->ReadCpuCsr(Socket, McId, MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG);


  if (!Mc5Shadow1Status.Bits.addrv) {
    return EFI_NOT_FOUND;
  }

  Mc5Shadow0Addr.Data   = mCpuCsrAccess->ReadCpuCsr(Socket, McId, MCI_ADDR_SHADOW0_M2MEM_MAIN_REG);
  Mc5Shadow1Addr.Data   = mCpuCsrAccess->ReadCpuCsr(Socket, McId, MCI_ADDR_SHADOW1_M2MEM_MAIN_REG);
  Address = ((UINT64)Mc5Shadow1Addr.Data << 32 | (UINT64)Mc5Shadow1Addr.Data);

  if ((Address & CACHELINE_MASK) != (pMemInfo->PhyAddr & CACHELINE_MASK)) {
    return EFI_NOT_FOUND;
  }

  McBankSts->Data = ((UINT64)Mc5Shadow1Status.Data << 32 | (UINT64)Mc5Shadow0Status.Data);
  return EFI_SUCCESS; //should not reach here.
}

//Double check for poison mscod and mcacod
BOOLEAN
IsCachePoisonError (
  MCBANK_ERR_INFO * pMcBankInfo
  )
{
  IA32_MCI_STATUS_MSR_STRUCT                  McBankSts;

  if (pMcBankInfo == NULL) {
    return FALSE;
  }

  //Only cover IFU and DCU poison detection case here.
  if (pMcBankInfo->McBankNum != CORE_IFU_BANK && pMcBankInfo->McBankNum != CORE_DCU_BANK) {
    return FALSE;
  }

  McBankSts.Data = pMcBankInfo->McBankStatus;

  if (pMcBankInfo->McBankNum == CORE_IFU_BANK && McBankSts.Bits.ms_code == IFU_POISON_DETECTED) {
    return TRUE;
  }

  if (pMcBankInfo->McBankNum == CORE_DCU_BANK && McBankSts.Bits.ms_code == DCU_POISON_DETECTED) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
ValidMcAddress (
  CONST MCBANK_ERR_INFO * CONST pMcBankInfo,
  MEMORY_DEV_INFO       * CONST pMemInfo,
  TRANSLATED_DIMM_TYPE  * CONST pDimmType
  )
{
  EFI_STATUS                        Status;
  IA32_MCI_STATUS_MSR_STRUCT        McBankSts;
  TRANSLATED_ADDRESS                XlatedAddr;

  if (pMcBankInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  McBankSts.Data = pMcBankInfo->McBankStatus;
  if (!McBankSts.Bits.addrv) {
    return EFI_NOT_READY;
  }
#if SMCPKG_SUPPORT  
  RASDEBUG((DEBUG_ERROR,"ValidMcAddress: McBankNum = %lx\n",pMcBankInfo->McBankNum));
  RASDEBUG((DEBUG_ERROR,"ValidMcAddress: McBankMisc = %lx\n",pMcBankInfo->McBankMisc));
  // check this is reverse translation sparing and scrub address for recoverable errors                              
  // check REC_ERR_LSB MC MISC Bits5:0 if MC bank is from 13 to 18 (MC bank for IMCx, refer mcBankTable at ErrorChip.c)    
//  if (  ((pMcBankInfo->McBankNum >= MEM_IMC_MCBANK_NUM_START) && (pMcBankInfo->McBankNum <= MEM_IMC_MCBANK_NUM_END)) 
//         && (pMcBankInfo->McBankMisc & 0x3F) ){
//     RASDEBUG((DEBUG_ERROR,"ValidMcAddress:Patrol scrub detect error, ret\n",pMcBankInfo->McBankNum));       
//     return EFI_NOT_READY;       
//  }
#endif   
  ZeroMem (&XlatedAddr, sizeof(TRANSLATED_ADDRESS));
  //MC has a valid address. Log it.
  Status = mMemRasLib->SystemAddressToDimmAddress (pMcBankInfo->McbankAddr, &XlatedAddr);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  *pDimmType = XlatedAddr.DimmType;

  pMemInfo->PhyAddr = pMcBankInfo->McbankAddr;
  pMemInfo->Node    = XlatedAddr.Node;
  pMemInfo->Channel = XlatedAddr.ChannelId;
  pMemInfo->Dimm    = XlatedAddr.DimmSlot;
  pMemInfo->Rank    = XlatedAddr.DimmRank;
  pMemInfo->Bank    = XlatedAddr.Bank;
  pMemInfo->Column  = XlatedAddr.Col;
  pMemInfo->Row     = XlatedAddr.Row;
  // APTIOV_SERVER_OVERRIDE_RC_START
  pMemInfo->RankAddress = XlatedAddr.RankAddress;
  // APTIOV_SERVER_OVERRIDE_RC_END
  pMemInfo->ValidBits |= PLATFORM_MEM_PHY_ADDRESS_VALID | PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID | PLATFORM_MEM_MODULE_VALID | PLATFORM_RANK_NUMBER_VALID |  PLATFORM_MEM_BANK_VALID | PLATFORM_MEM_ROW_VALID | PLATFORM_MEM_COLUMN_VALID ;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
IsolateFaultyDimm (
  MCBANK_ERR_INFO * pMcBankInfo,
  MEMORY_DEV_INFO * CONST pMemInfo
  )
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  UINT8                              MscodeDdrType;
  UINT8                              McaCh;
  UINT8                              IMCId;
  UINT8                              IsolationCh;
  MCE_CLASS                          ErrClass;
  MCE_SOURCE                         ErrSource;
  LOG_ACTION                         Logger;
  IA32_MCI_STATUS_MSR_STRUCT         McBankSts;
  TRANSLATED_DIMM_TYPE               DimmType;

  DimmType = 0;
  if (pMcBankInfo == NULL || pMemInfo == NULL) {
    RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: McBankStatus = %lx\n",pMcBankInfo->McBankStatus));
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: McbankAddr = %lx\n",pMcBankInfo->McbankAddr));
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: McBankMisc = %lx\n",pMcBankInfo->McBankMisc));
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: McBankNum = %lx\n",pMcBankInfo->McBankNum));
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: Type = %lx\n",pMcBankInfo->Type));
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: Segment = %lx\n",pMcBankInfo->Segment));
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: Socket = %lx\n",pMcBankInfo->Socket));
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ApicId = %lx\n",pMcBankInfo->ApicId));

  ErrSource = GetMceSource(pMcBankInfo->McBankNum);
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ErrSource = %lx\n",ErrSource));

  if (ErrSource == DONT_CARE) {
    RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ErrSource == DONT_CARE\n"));
    return EFI_NOT_FOUND;
  }

  McBankSts.Data = pMcBankInfo->McBankStatus;
  ErrClass = GetMceClass(&McBankSts);
  RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ErrClass = %lx\n",ErrClass));

  ZeroMem (pMemInfo, sizeof(MEMORY_DEV_INFO));

  pMemInfo->ErrorType = PLATFORM_MEM_ERROR_UNKNOWN;
  pMemInfo->ValidBits = PLATFORM_MEM_ERROR_STATUS_VALID;

  if (ErrClass == UCNA) { //We only hand UCNA case from M2M here because memory sourced poison data should trigger UCNA at M2M bank
    RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ErrClass == UCNA\n"));
    if (ErrSource != M2M) {
      RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ErrSource != M2M\n"));
      Logger = CeLogger;
    }
    Logger = UcnaLogger;
  } else if (ErrClass != INVALID && ErrClass != CORRECTED) { //All other UC case
    RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ErrClass != INVALID && ErrClass != CORRECTED\n"));
    Logger = DueLogger;
  } else if (ErrClass != INVALID) {
    RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: ErrClass == CORR \n"));
    Logger = CeLogger;
  } else {
    RASDEBUG((DEBUG_ERROR,"IsolateFaultyDimm: EFI_NOT_FOUND\n"));
    return EFI_NOT_FOUND;
  }

  if (ErrSource == IFU || ErrSource == DCU) {
    if (!IsCachePoisonError(pMcBankInfo)) { //Only process poison detected case for cache bank.
      RASDEBUG((DEBUG_ERROR,"!IsCachePoisonError(pMcBankInfo)\n"));
      return EFI_NOT_FOUND;
    }

    Status = ValidMcAddress(pMcBankInfo, pMemInfo, &DimmType);
    if (EFI_ERROR(Status)) {
      RASDEBUG((DEBUG_ERROR,"ValidMcAddress Error Status\n"));
      return Status;
    }

    Status = LookUpPoisonRecord(pMcBankInfo, pMemInfo);
    if (Status == EFI_SUCCESS) {////1LM or NM sourced poison should be found here
      RASDEBUG((DEBUG_ERROR,"LookUpPoisonRecord\n"));
      return  Logger(pMemInfo);
    }

    Status = CheckM2MUCNA(pMemInfo, &McBankSts);

    if (Status == EFI_SUCCESS) {
      ErrSource = M2M;
      RASDEBUG((DEBUG_ERROR,"CheckM2MUCNA Success\n"));
    } else { // Last possible poison source. NVDIMM sourced error.
      RASDEBUG((DEBUG_ERROR,"CheckDDRTError\n"));
      return CheckDDRTError(pMemInfo, pMcBankInfo->McbankAddr, Logger);
    }
  } else if (ErrSource == M2M || ErrSource == IMC) {
    //Determine info to ch level based on MC Bank info
    //Double check for mirroring cases.
    RASDEBUG((DEBUG_ERROR,"ErrSource == M2M || ErrSource == IMC\n"));
    McaCh = McBankSts.Bits.mca_code & 0xF;
    IMCId = (pMcBankInfo->McBankNum == MEM_M2M_0_BANK || pMcBankInfo->McBankNum == MEM_IMC0_CH0_BANK ||
          pMcBankInfo->McBankNum == MEM_IMC0_CH1_BANK || pMcBankInfo->McBankNum == MEM_IMC0_CH2_BANK) ? 0 : 1;

    if (McaCh >= MAX_MC_CH) { //No valid channel info in M2M bank. IMC bank should not hit the cause.
      RASDEBUG((DEBUG_ERROR, "McaCh >= MAX_MC_CH\n"));
      return Logger(pMemInfo);
    }
    if(ErrSource == M2M) {
      if((pMcBankInfo->McBankMisc & BIT12) != 0) {
        pMemInfo->ErrorType = PLATFORM_MEM_ERROR_SINGLEBIT_ECC;
      } else {
        pMemInfo->ErrorType = PLATFORM_MEM_ERROR_MLTIBIT_ECC;
      }
    }


    IsolationCh = NODECH_TO_SKTCH (IMCId, McaCh);

    if (ErrSource == IMC) { // Check for CAP or WrCRC error first.
      pMemInfo->Node = SKT_TO_NODE(pMcBankInfo->Socket,IMCId);
      pMemInfo->Channel = McaCh;
      pMemInfo->ValidBits |= PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_CARD_VALID;

      if (McBankSts.Bits.ms_code == MC_UC_PATROL_SCRUB) {
        pMemInfo->ErrorType = PLATFORM_MEM_ERROR_SCRUB_UNCOR;
      }

      Status = CheckCAPError(pMemInfo, McBankSts);
      if (Status == EFI_SUCCESS) {
    	return Logger(pMemInfo);
      }
    }

    Status = ValidMcAddress(pMcBankInfo, pMemInfo, &DimmType);
    //If translation fail or channel does not match. Trust the channel obtained from MC Bank. Clear out valid bit below dimm (include) and address
    if (EFI_ERROR(Status) || (pMemInfo->Channel != McaCh) || (NODE_TO_SKT(pMemInfo->Node) != pMcBankInfo->Socket)) {
      RASDEBUG((DEBUG_ERROR, "If translation fail or channel does not match. Trust the channel obtained from MC Bank.\n"));
      pMemInfo->Node = SKT_TO_NODE(pMcBankInfo->Socket,IMCId);
      pMemInfo->Channel = McaCh;
      pMemInfo->ValidBits &= ~(PLATFORM_MEM_PHY_ADDRESS_VALID | PLATFORM_MEM_MODULE_VALID | PLATFORM_RANK_NUMBER_VALID | PLATFORM_MEM_BANK_VALID | PLATFORM_MEM_ROW_VALID | PLATFORM_MEM_COLUMN_VALID);
    } else {
      return Logger(pMemInfo);
    }
  }

  //Condition to meet for reaching this point: 1. MC bank is M2M or iMC OR 2.MC bank is DCU/IFU and mscod indicates poison detection.

  //!IMPORTANT check about DDRT link error case.

  // Address is in 1LM region
  // Possible condition to reach here:1. Patrol failure; 2. Poison disable. Demand read. UCNA on M2M and 3. Poison enabled. IFU/DCU poison victim.
  if (DimmType == ddr4dimmType) {
    RASDEBUG((DEBUG_ERROR,"ddr4dimmType\n"));
    return  (pMemInfo->ValidBits & PLATFORM_MEM_PHY_ADDRESS_VALID) ? Logger(pMemInfo) : CheckDDR4Error(pMemInfo, Logger); //Later case might occur for Patrol scrub failure.
  }

  //2LM cases start here
  if (ErrSource == IMC) {   //Possible condition 1. Patrol failure
    RASDEBUG((DEBUG_ERROR,"2lm ErrSource == IMC\n"));
    return CheckDDR4Error(pMemInfo, Logger); //
  }

  //M2M case
  if (ErrSource == M2M) { //Possible condition 1. Poison disabled. Demand UC NM and FM. 2. Poison enable. NM UCNA
    //MC7/8_Status[25:24] is MscodDdrType
    MscodeDdrType = (McBankSts.Data & (BIT25 | BIT24) >> 24);
    RASDEBUG((DEBUG_ERROR,"2lm ErrSource == M2M\n"));
    if (MscodeDdrType == 0) { //We don't have clear error source. just report error to ch level.
      RASDEBUG((DEBUG_ERROR,"MscodeDdrType == 0\n"));
      return Logger(pMemInfo);
    }

    if (MscodeDdrType & BIT0) { //2LM region. MSCOD points source to DDR4
      RASDEBUG((DEBUG_ERROR,"MscodeDdrType & BIT0\n"));
      return CheckDDR4Error(pMemInfo, Logger);
    }

    if (MscodeDdrType & BIT1) { //This is the DDRT case
      RASDEBUG((DEBUG_ERROR,"MscodeDdrType & BIT1\n"));
      return CheckDDRTError(pMemInfo, pMcBankInfo->McbankAddr, Logger);
    }
  }
  RASDEBUG((DEBUG_ERROR,"return EFI_NOT_FOUND\n"));
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
IsolateFaultyDimmForErrSig (
  UINT8                          McBank,
  UINT8                          Socket,
  EMCA_MC_SIGNATURE_ERR_RECORD * CONST pMcSigRecord,
  MEMORY_ERROR_SECTION * CONST   MemErrSection
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  MCBANK_ERR_INFO   McBankInfo;
  MEMORY_DEV_INFO   MemInfo;

  if (pMcSigRecord == NULL || MemErrSection == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  McBankInfo.ApicId        = (UINT16)pMcSigRecord->ApicId;
  McBankInfo.Socket        = Socket;
  McBankInfo.McBankNum     = McBank;
  McBankInfo.McBankMisc    = pMcSigRecord->Signature.McMisc;
  McBankInfo.McBankStatus  = pMcSigRecord->Signature.McSts;
  McBankInfo.McbankAddr    = pMcSigRecord->Signature.McAddr;

  Status = IsolateFaultyDimm(&McBankInfo, &MemInfo);

  if (Status == EFI_SUCCESS) {
    MemErrSection->ValidBits  = MemInfo.ValidBits;
    MemErrSection->ErrorType  = (UINT8)MemInfo.ErrorType;
    MemErrSection->Node       = MemInfo.Node;
    MemErrSection->Card       = MemInfo.Channel;
    MemErrSection->Module     = MemInfo.Dimm;
    MemErrSection->Bank       = MemInfo.Bank;
    MemErrSection->RankNumber = MemInfo.Rank;
    MemErrSection->Col        = (UINT16)MemInfo.Column;
    MemErrSection->Row        = (UINT16)MemInfo.Row;
  //  MemErrSection->Extended   = MemInfo.Extended;
  // APTIOV_SERVER_OVERRIDE_RC_START
  MemErrSection->RankAddress = MemInfo.RankAddress;
  MemErrSection->Extended   = MemInfo.Extended;
  // APTIOV_SERVER_OVERRIDE_RC_END
  }

  return Status;
}

EFI_STATUS
InitializeIsolationFlow(
  )
{
  EFI_STATUS Status;
  Status = gSmst->SmmLocateProtocol (
                &gEfiPlatformRasPolicyProtocolGuid,
                NULL,
                &mPlatformRasPolicyProtocolLib
                );
  ASSERT_EFI_ERROR (Status);

  ZeroMem(&PoisonTrack, sizeof(POISON_TRACK));
  CsrReader = mCpuCsrAccess->ReadCpuCsr;
  CeLogger = FinalizeCELog;
  UcnaLogger = SetPoisonRecord;
  DueLogger  = FinalizeUELog;

  return EFI_SUCCESS;
}

