//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2016, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//
//
//  Rev. 1.03
//    Bug Fix:  Still keep supporting CMCI
//    Reason:   
//    Auditor:  Chen Lin
//    Date:     Feb/22/2017
//
//  Rev. 1.02
//   Bug Fixed:  Enable Cloaking by default for hiding Mcbank status.
//               Enable BIT5 of MSR 0x17F for disabling Memory Correctable logging in Mcbank status.
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Jan/19/2017
// 
//
//  Rev. 1.01
//   Bug Fixed:  1.Add workaround for Memory UCE. Will check if we still keep it in the future.
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Dec/30/2016
//
//  Rev. 1.00
//   Bug Fixed:  Fixed that PERR could not work.
//   Reason:     Skip MCbank 3 erorr when Mcbank ADDR is not valid.
//   Auditor:    Chen Lin
//   Date:       Nov/11/2016
//  
//
//**********************************************************************// 
/** @file
  Implementation of the CPU Core Error Handler.

  Copyright (c) 2009-2016 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
#include "Token.h" //SMCPKG_SUPPORT
#include <Library/emcaplatformhookslib.h>
#include <Library/ProcMemErrReporting.h>
#include <Library/mpsyncdatalib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/LocalApicLib.h>
#include <Library/IoLib.h>
#include <Setup/IioUniversalData.h>
#include <IndustryStandard/Acpi.h>
#include <Cpu/CpuBaseLib.h>
#include <Protocol/ErrorHandlingProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/CrystalRidge.h>
#include <Protocol/SmmCpuService.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/VariableLock.h>
#include <Register/LocalApic.h>
#include "CommonErrorHandlerDefs.h"
#include "ProcessorErrorHandler.h"
#include "MemoryErrorHandler.h"
#include "FnvErrorHandler.h"
#include "McaHandler.h"
#include "ErrorRecords.h"
#include "IoMcaHandler.h"
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks Support
#include <OemRasLib/OemRasLib.h>  
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support

// Module data
EFI_ERROR_HANDLING_PROTOCOL           *mErrorHandlingProtocol;
EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
EFI_SMM_BASE2_PROTOCOL                *mSmmBase2;
EFI_MEM_RAS_PROTOCOL                  *mMemRas;
EFI_CPU_CSR_ACCESS_PROTOCOL           *mCpuCsrAccess;
EFI_CRYSTAL_RIDGE_PROTOCOL            *mCrystalRidge;
SMM_ERROR_MP_SYNC_DATA                *mSmmErrorMpSyncData;
EFI_RAS_SYSTEM_TOPOLOGY               *mRasTopology;
SYSTEM_RAS_SETUP                      *mRasSetup;
SYSTEM_RAS_CAPABILITY                 *mRasCapability;
EFI_SMM_CPU_SERVICE_PROTOCOL          *mSmmCpuService;
EFI_SMM_VARIABLE_PROTOCOL             *mSmmVariable;
// APTIOV_SERVER_OVERRIDE_RC_START
//Total valid MCBanks
UINT8                                 OtherMcBankCount = 0;
MCA_BANK_INFO                         *OtherMcBankInfo;
// APTIOV_SERVER_OVERRIDE_RC_END
UINT8                                 DengatePoisonRetryCount = 0;
UINT32                                mSeverity = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;
BOOLEAN                               mSuppressMca = FALSE;
struct SystemMemoryMapHob             *mSystemMemoryMap;
REPORTER_PROTOCOL                     mMcaReporter;
SPIN_LOCK                             mApLock;



MCA_BANK_ERR_TYPE mMcaBankErrorType[] = {
  {MCA_UNIT_TYPE_IFU,  UEFI_PROC_CACHE_ERR_TYPE},
  {MCA_UNIT_TYPE_DCU,  UEFI_PROC_CACHE_ERR_TYPE},
  {MCA_UNIT_TYPE_DTLB, UEFI_PROC_TLB_ERR_TYPE},
  {MCA_UNIT_TYPE_MLC,  UEFI_PROC_CACHE_ERR_TYPE},
  {MCA_UNIT_TYPE_PCU,  UEFI_PROC_UNKNOWN_ERR_TYPE},
  {MCA_UNIT_TYPE_KTI,  UEFI_PROC_BUS_ERR_TYPE},
  {MCA_UNIT_TYPE_CHA,  UEFI_PROC_CACHE_ERR_TYPE},
  {MCA_UNIT_TYPE_IIO,  UEFI_PROC_UNKNOWN_ERR_TYPE},
};

// External Data
extern MP_CORE_SYNC_DATA              mMpSyncData;

EFI_STATUS McaClearStatus (VOID);

// Function definitions

////////////////////////////////////// Common Routines //////////////////////////////////////

/**
  Check the lmce status for all the threads and decide if MCE needs to be
  broadcast or local.

  @param  SyncData     Sync structure which contains MCA semaphores
**/
EFI_STATUS
UpdateLmceStatus(
  SMM_ERROR_MP_SYNC_DATA *SyncData
)
{
  UINTN                      CpuNumber;
  SMM_ERROR_CPU_DATA_BLOCK    *CpuDataPtr;
  BOOLEAN LmceOsEnabled = FALSE;
  BOOLEAN Broadcast = FALSE;
  BOOLEAN Lmce = FALSE;

  if(SyncData == NULL){
    return EFI_INVALID_PARAMETER;
  }

  if (mRasCapability->SiliconRasCapability.LmceCap && mRasSetup->Emca.LmceEn) {
    //RASDEBUG ((DEBUG_ERROR, "socketbitmap %lx\n",mRasTopology->SystemInfo.SocketBitMap));
    Broadcast = isBMCE(mRasTopology->SystemInfo.SocketBitMap);
    IsLmceOsEnabled(&LmceOsEnabled);
    for (CpuNumber = 0; CpuNumber < gSmst->NumberOfCpus; CpuNumber++) {
      CpuDataPtr = &SyncData->CpuData[CpuNumber];
      CpuDataPtr->Lmce = FALSE;
      if (gSmst->CurrentlyExecutingCpu != CpuNumber) {
        SmmBlockingStartupThisAp (IsLmceSignaled,
                                  CpuNumber,
                                  (BOOLEAN *)&CpuDataPtr->Lmce
                                  );
      } else {
        IsLmceSignaled((BOOLEAN *)&CpuDataPtr->Lmce);
      }
      if(LmceOsEnabled && CpuDataPtr->Lmce) {
        Lmce |= TRUE;
      }
    }
    //if broadcast == true && lmce == true then clear lmce
    //if broadcast == false && lmce == false then default to broadcast.
    if(Broadcast) {
      //RASDEBUG ((DEBUG_ERROR, "Broadcast == TRUE\n"));
      if(Lmce) {
        //RASDEBUG ((DEBUG_ERROR, "Lmce == TRUE\n"));
        //Clear Lmce Status
        for (CpuNumber = 0; CpuNumber < gSmst->NumberOfCpus; CpuNumber++) {
          CpuDataPtr = &SyncData->CpuData[CpuNumber];
          CpuDataPtr->Lmce = FALSE;
          if (gSmst->CurrentlyExecutingCpu != CpuNumber) {
            SmmBlockingStartupThisAp (ClearLmceStatus,
                                      CpuNumber,
                                      NULL
                                      );
          } else {
            ClearLmceStatus(NULL);
          }
        }
      }
    } else {
      //RASDEBUG ((DEBUG_ERROR, "Broadcast == FALSE\n"));
      if(!Lmce) {
        //RASDEBUG ((DEBUG_ERROR, "Lmce == FALSE : setting broadcast TRUE\n"));
        Broadcast = TRUE;
      }
    }
  }else {
    //RASDEBUG ((DEBUG_ERROR, "LMCE disabled broadcast = TRUE\n"));
    Broadcast = TRUE;
  }

  SyncData->Broadcast = Broadcast;
  return EFI_SUCCESS;
}

/**
  Notify the OS via Processor mechanisms

  This function takes the appropriate action for a particular error based on severity.

  @param[in] ErrorSeverity    The severity of the error to be handled
  @param[out] OsNotified      Returns whether the OS was notified

  @retval Status.

**/
EFI_STATUS
ProcessorNotifyOs (
  IN      UINT8     ErrorSeverity,
     OUT  BOOLEAN   *OsNotified
  )
{
  UINTN                      CpuNumber;
  BOOLEAN                     UncorrectedErrorFound = FALSE;
  BOOLEAN                     CorrectedErrorFound = FALSE;
  SMM_ERROR_CPU_DATA_BLOCK    *CpuDataPtr;
  UINT32 SaveStateType;
  UINT8 i;

  if(mRasCapability->SiliconRasCapability.EmcaGen2Cap  == FALSE) {
    return EFI_UNSUPPORTED;
  }

  for (CpuNumber = 0; CpuNumber < gSmst->NumberOfCpus; CpuNumber++) {
    CpuDataPtr = &mSmmErrorMpSyncData->CpuData[CpuNumber];
    if (CpuDataPtr->EventLog.ErrorsFound) {

      if (((CpuDataPtr->EventLog.Events[McbankSrc].SeverityMap & (1 << SEVERITY_MAP_RECOVERABLE)) != 0) ||
         ((CpuDataPtr->EventLog.Events[McbankSrc].SeverityMap & (1 << SEVERITY_MAP_FATAL)) != 0) ) {
        if( mRasSetup->Emca.EmcaMsmiEn != 0) {
          UncorrectedErrorFound = TRUE;
        }
      }

      if(((CpuDataPtr->EventLog.Events[McbankSrc].SeverityMap & (1 << SEVERITY_MAP_CORRECTED)) != 0)) {
        if(mRasSetup->Emca.EmcaCsmiEn == 2) {
          SignalCmciToOs (CpuDataPtr->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_CORRECTED].BankIdx,
                          CpuDataPtr->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_CORRECTED].ApicId
                          );
          CorrectedErrorFound = TRUE;
          *OsNotified = TRUE;
        }
      }

      CpuDataPtr->EventLog.Events[McbankSrc].SeverityMap = 0;
      CpuDataPtr->EventLog.Events[McbankSrc].ErrorFound = FALSE;
    }
    CpuDataPtr->EventLog.ErrorsFound = FALSE;

    for(i=0;i < MAX_ERROR_SEVERITY; i++) {
      CpuDataPtr->EventLog.Events[McbankSrc].Log[i].BankIdx = 0;
      CpuDataPtr->EventLog.Events[McbankSrc].Log[i].ApicId = 0;
    }
  }

  if(UncorrectedErrorFound) {
    if(mRasSetup->Emca.EmcaMsmiEn == 2) {
      SaveStateType = GetSaveStateType (mRasTopology->SystemInfo.SocketBitMap);
      RASDEBUG ((DEBUG_INFO, "Sending OS notification via MCE\n"));
      for(CpuNumber = 0; CpuNumber < gSmst->NumberOfCpus; CpuNumber++) {
        CpuDataPtr = &mSmmErrorMpSyncData->CpuData[CpuNumber];
        RASDEBUG ((DEBUG_INFO, "MCE CpuNumber:%lx Broadcast:%lx lmce:%lx\n",CpuNumber,mSmmErrorMpSyncData->Broadcast,mSmmErrorMpSyncData->CpuData[CpuNumber].Lmce));
        SignalMceToOs(CpuNumber,
                      SaveStateType,
                      mSmmErrorMpSyncData->Broadcast,
                      mSmmErrorMpSyncData->CpuData[CpuNumber].Lmce
                      );
        CpuDataPtr->Lmce = FALSE;
      }
    }
    *OsNotified = TRUE;
  }
  if (CorrectedErrorFound) {
    RASDEBUG ((DEBUG_INFO, "Sending OS notification via CMCI\n"));
  }
  mSmmErrorMpSyncData->Broadcast = FALSE;

  return  EFI_SUCCESS;
}

// This is used in MemoryErrorHandler.c.  So why is it defined here?
/**
 Put CPU into infinite loop.

  @retval None

**/
VOID
ProcessorCatastrophicErrorHandler (
  VOID
  )
{
  //
  // IERR => should not get here
  //
  CpuDeadLoop();
  return;
}

/**
 Put CPU into infinite loop based on setup option.

  @retval None

**/
VOID
 SpinLoopOnFatalErr(
  VOID
  )
{

 if(mRasSetup->FatalErrSpinLoopEn == 1) {
  RASDEBUG((DEBUG_INFO,"SpinLoopOnFatalErr: deadloop function \n"));
  CpuDeadLoop();
  }
  return;
}



/**
  Get error severity from error signature.

  @param[in] McErrSig   MC Error signature

  @retval Sev   Severity

**/
UINT32
MapErrorSeverity (
  IN      EMCA_MC_SIGNATURE_ERR_RECORD  *McErrSig
  )
{
  IA32_MCI_STATUS_MSR_STRUCT            McSts;
  UINT32                                sev;

  sev = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

  McSts.Data = McErrSig->Signature.McSts;

  if (McSts.Bits.uc == 1) {
    if (McSts.Bits.pcc != 1) {
      if (McSts.Bits.s == 1) {
        sev = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE; // recoverable
        //if((McSts.Bits.ar == 1) {
        //  sev = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE; // SRAR - Recoverable
        //} else {
        //  sev = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE; // SRAO - Recoverable
        //}
      } else {
        sev = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED; // UCNA
      }
    } else {
      sev = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
      SpinLoopOnFatalErr( );
    }
  } else {
    sev = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED; //Corrected Error
  }

  return sev;
}

/**
  Create the processor error record

  @param[in] McBank     - The machine check bank
  @param[in] skt        - The socket number
  @param[in] Sev        - The severity of the error
  @param[in] McErrSig   - MC Error signature
  @param[in] ErrSts     - A pointer to the Processor Error Record.

  @retval Sev   Severity

**/
EFI_STATUS
FillProcessorErrorSection (
  IN        UINT8                         McBank,
  IN        UINT8                         skt,
  IN        UINT32                        Sev,
  IN        EMCA_MC_SIGNATURE_ERR_RECORD  *McErrSig,
  OUT       UEFI_PROCESSOR_ERROR_RECORD   *ErrSts
  )
{
  UINT32                                  UnitType;
  UINT8                                   Index;
  EFI_CPUID_REGISTER                      Register;

  if(ErrSts == NULL || McErrSig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (ErrSts, sizeof(UEFI_PROCESSOR_ERROR_RECORD));
  AsmCpuid (EFI_CPUID_VERSION_INFO, &Register.RegEax, &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);

  // Build Error Record Header
  ErrSts->Header.SignatureStart = SIGNATURE_32('C','P','E','R');
  ErrSts->Header.Revision = 0x0204; // UEFI Spec version 2.4
  ErrSts->Header.SignatureEnd = 0xffffffff;
  ErrSts->Header.SectionCount = 1;  // Only supporting one section per error record
  ErrSts->Header.Severity = Sev;
  ErrSts->Header.ValidBits = 0;
  ErrSts->Header.RecordLength = sizeof (UEFI_PROCESSOR_ERROR_RECORD);
  CopyMem (&ErrSts->Header.NotificationType, &gErrRecordNotifyMceGuid, sizeof (EFI_GUID));
  if (Sev == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) {
    ErrSts->Header.Flags = HW_ERROR_FLAGS_RECOVERED;
  }

  // Fill error descriptor
  ErrSts->Descriptor.Offset = sizeof (UEFI_ERROR_RECORD_HEADER) + sizeof (UEFI_ERROR_SECT_DESC);
  ErrSts->Descriptor.Length = sizeof (PROCESSOR_GENERIC_ERROR_SECTION);
  ErrSts->Descriptor.Revision = 0x0204; // UEFI Spec version 2.4
  ErrSts->Descriptor.ValidBits = 0;
  ErrSts->Descriptor.Flags = 1;   // Primary
  CopyMem (&ErrSts->Descriptor.SectionType, &gErrRecordProcGenErrGuid, sizeof (EFI_GUID));
  ErrSts->Descriptor.Severity = Sev;

  // Fill error section body
  ErrSts->Section.Type = GEN_ERR_PROC_TYPE_IA32_X64;
  ErrSts->Section.Isa = GEN_ERR_PROC_ISA_X64;
  UnitType = GetMcBankUnitType (McBank);
  for (Index = 0; Index < (sizeof (mMcaBankErrorType) / sizeof (MCA_BANK_ERR_TYPE)); Index++) {
    if (UnitType & mMcaBankErrorType[Index].UnitType) {
      ErrSts->Section.ErrorType = mMcaBankErrorType[Index].ErrType;
    }
  };
  ErrSts->Section.Operation = 0;  // Generic error
  ErrSts->Section.VersionInfo = Register.RegEax;
  ErrSts->Section.ApicId = GetApicID ();
  ErrSts->Section.ValidBits = GEN_ERR_PROC_TYPE_VALID | GEN_ERR_PROC_ISA_VALID | GEN_ERR_PROC_ERROR_TYPE_VALID |
    GEN_ERR_PROC_OPERATION_VALID | GEN_ERR_PROC_VERSION_VALID | GEN_ERR_PROC_ID_VALID;

  ErrSts->Section.McaBankInfo.ApicId = (UINT32)ErrSts->Section.ApicId;
  ErrSts->Section.McaBankInfo.BankNum = McBank;
  ErrSts->Section.McaBankInfo.BankType = UnitType;
  ErrSts->Section.McaBankInfo.BankScope = GetMcBankScope (McBank);
  ErrSts->Section.McaBankInfo.McaStatus = McErrSig->Signature.McSts;
  ErrSts->Section.McaBankInfo.McaAddress = McErrSig->Signature.McAddr;
  ErrSts->Section.McaBankInfo.McaMisc = McErrSig->Signature.McMisc;
  ErrSts->Section.McaBankInfo.Valid = TRUE;
  // APTIOV_SERVER_OVERRIDE_RC_START : For filling OtherMcBankInfo and OtherMcBankCount
  ErrSts->Section.OtherMcBankCount = OtherMcBankCount;
  CopyMem (ErrSts->Section.OtherMcBankInfo, OtherMcBankInfo, OtherMcBankCount * sizeof (MCA_BANK_INFO));
  // APTIOV_SERVER_OVERRIDE_RC_END : For filling OtherMcBankInfo and OtherMcBankCount
  return EFI_SUCCESS;
}

/**
  Returns the more severe of the two inputs.

  @param[in] NewSev   The newer severity
  @param[in] OldSev   The old severity

  @retval The more severe of the two severities.

**/
UINT32
GetHighestSeverity (
  IN      UINT32    NewSev,
  IN      UINT32    OldSev
  )
{
  switch (NewSev) {
    case EFI_ACPI_5_0_ERROR_SEVERITY_FATAL:
      return NewSev;
      break;
    case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE:
      if (OldSev == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED || OldSev == EFI_ACPI_5_0_ERROR_SEVERITY_NONE) {
        return NewSev;
      }
      break;
    case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED:
      if (OldSev == EFI_ACPI_5_0_ERROR_SEVERITY_NONE) {
        return NewSev;
      }
      break;
    case EFI_ACPI_5_0_ERROR_SEVERITY_NONE:
      return OldSev;
    default:
      //RASDEBUG ((DEBUG_ERROR, "Invalid error severity: %x \n", NewSev));
      ASSERT (0);
      break;
  }
  return OldSev;
}

/**
  Handle an uncorrected machine check bank error.

  @param[in] McBank   The machine check bank to handle
  @param[in] skt      The socket number
  @param[in] CpuData  Cpu data block
  @param[out] ErrSts  Error Status

  @retval None.

**/
VOID
McBankErrorHandler (
  IN      UINT8                     McBank,
  IN      UINT8                     *Skt,
  IN      SMM_ERROR_CPU_DATA_BLOCK  *CpuData
  )
{
  EFI_STATUS                        Status;
  IA32_MCI_STATUS_MSR_STRUCT        MciStsMsr;
  EMCA_MC_SIGNATURE_ERR_RECORD      McErrSig;
  UEFI_PROCESSOR_ERROR_RECORD       ProcErrSts;
  UEFI_MEM_ERROR_RECORD             MemErrSts;
  UINT32                            Sev;
  BOOLEAN                           Recovered;
  UINT64                            PrevState;
  UINT64                            State;
  UINT16                            SecType;
  UEFI_ERROR_RECORD_HEADER         *ErrStsHead;
  BOOLEAN                           CancelMce = FALSE;  
#if SMCPKG_SUPPORT  
  BOOLEAN                           IgnoreError = FALSE;
#endif
  SecType = GetMcBankSecType (McBank);

  //RASDEBUG((DEBUG_INFO,"McBankErrorHandler: Inside the function \n"));

  //Init structures
  PrevState = SmmMceClearedState;
  State = SmmMceClearedState;
  Recovered = FALSE;
  McErrSig.Signature.McSts = 0;
  McErrSig.Signature.McAddr = 0;
  McErrSig.Signature.McMisc = 0;
  McErrSig.ApicId = 0;
  Sev = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

  PrevState = InterlockedCompareExchange64((UINT64*) &CpuData->SmmMceSyncState, SmmMceClearedState, SmmMceReadingState);
  State = (PrevState == SmmMceClearedState) ? SmmMceReadingState : PrevState; //Reading State
  do {
    //RASDEBUG ((DEBUG_ERROR, "McBankErrorHandler : Skt = %d, McBank = %d, State = %d\n", *Skt, McBank, State));
    switch(State) {
      case SmmMceReadingState:
        Status = ReadMcBankSigHook (McBank, *Skt, &McErrSig);
        ASSERT_EFI_ERROR (Status);
        PrevState = InterlockedCompareExchange64 ((UINT64*) &CpuData->SmmMceSyncState, SmmMceReadingState, SmmMceProcessingState);
        State = (PrevState == SmmMceReadingState) ? SmmMceProcessingState : PrevState;
        break;
      case SmmMceProcessingState:
        MciStsMsr.Data = McErrSig.Signature.McSts;
        if (MciStsMsr.Bits.val != 1) {
          //Removed MciStsMsr.Bits.uc != 1  || MciStsMsr.Bits.en != 1
          // This is to process correctable errors also. Further review might be needed.
          PrevState = InterlockedCompareExchange64 ((UINT64*) &CpuData->SmmMceSyncState, SmmMceProcessingState, SmmMceClearedState);
          State = (PrevState == SmmMceProcessingState) ? SmmMceClearedState : PrevState;
          // APTIOV_SERVER_OVERRIDE_RC_START : Added support for Last Boot Error Logging
        #if HandleLastbootError_Support  
            if ((McBank == MCA_BANK_M2M0) || (McBank == (MCA_BANK_M2M1) ))
            {
                Status = ReadMcBankSigHook(McBank,*Skt,&McErrSig);
                MciStsMsr.Data = McErrSig.Signature.McSts;
                
            }
            if (MciStsMsr.Bits.val != 1)
            {
                break; //Not valid error or not uncorrected error, end of the flow     
            }
        #else   
            break; //Not valid error or not uncorrected error, end of the flow
        #endif    
         // APTIOV_SERVER_OVERRIDE_RC_END : Added support for Last Boot Error Logging
        } //else
        Sev = MapErrorSeverity (&McErrSig);
        
     #if SMCPKG_SUPPORT 
     {   
         IA32_MCI_STATUS_MSR_STRUCT            MciSts;

         MciSts.Data = McErrSig.Signature.McSts;
        //WA for UCE with Mcbank 7,8 
         if ( McBank == MCA_BANK_M2M0 ||  McBank == MCA_BANK_M2M1 ){
           if ( MciSts.Bits.val == 1 && MciSts.Bits.uc == 1 && ( Sev != EFI_ACPI_5_0_ERROR_SEVERITY_FATAL)  ) {
             // set SEV as Fatal; 
             RASDEBUG ((DEBUG_ERROR, "set SEV to Fatal\n"));  
             Sev =  EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
           }
        }  
     }     
    #endif 
             
        if (Sev == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE) {
          Recovered = AttemptRecovery (McBank, *Skt, &McErrSig);
          if (Recovered == TRUE) {
            Sev = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
            PrevState = InterlockedCompareExchange64 ((UINT64*) &CpuData->SmmMceSyncState, SmmMceProcessingState, SmmMceWritingState);
            State = (PrevState == SmmMceProcessingState) ? SmmMceWritingState : PrevState;
            break;
          }
        }
        if(Sev == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) {
          CpuData->CmciMcBankErrCount[McBank]++;

          // Check if the error on bank is occuring more than 20 times, then it is hard error
          // disable CMCI morphing on the particular bank and come out the loop
          if (CpuData->CmciMcBankErrCount[McBank] >= 20) {
            DisableCSMIAtMcBank(McBank);
          }
        }
        //Recovery not posible, log the error.
        PrevState = InterlockedCompareExchange64 ((UINT64*) &CpuData->SmmMceSyncState, SmmMceProcessingState, SmmMceLoggingState);
        State = (PrevState == SmmMceProcessingState) ? SmmMceLoggingState : PrevState;
        break;
      case SmmMceWritingState:
        WriteMcBank (McBank, &McErrSig);
        PrevState = InterlockedCompareExchange64 ((UINT64*) &CpuData->SmmMceSyncState, SmmMceWritingState, SmmMceLoggingState);
        State = (PrevState == SmmMceWritingState) ? SmmMceLoggingState : PrevState;
        break;
      case SmmMceLoggingState:
        ErrStsHead = NULL;
        // APTIOV_SERVER_OVERRIDE_RC_START : For filling OtherMcBankInfo and OtherMcBankCount
        if (mRasTopology->SystemInfo.SkipFillOtherMcBankInfoFlag == FALSE) {
            gSmst->SmmAllocatePool (EfiRuntimeServicesData, (GetErrorValidMsrCount() * sizeof (MCA_BANK_INFO)), (VOID**)&OtherMcBankInfo);
            FillOtherMcBankInfo(*Skt, OtherMcBankInfo, &OtherMcBankCount);
            mRasTopology->SystemInfo.SkipFillOtherMcBankInfoFlag = TRUE;
        }
        // APTIOV_SERVER_OVERRIDE_RC_END : For filling OtherMcBankInfo and OtherMcBankCount
        switch (SecType) {
          case EMCA_SECTYPE_PLATMEM:
            //RASDEBUG ((DEBUG_INFO,"Section Type = EMCA_SECTYPE_PLATMEM\n"));
          ErrStsHead = &MemErrSts.Header;
            Status = FillPlatformMemorySection (McBank, *Skt, Sev, &McErrSig, &MemErrSts);
            break;
          case EMCA_SECTYPE_PROCGEN:
            ErrStsHead = &ProcErrSts.Header;
            Status = FillProcessorErrorSection (McBank, *Skt, Sev, &McErrSig, &ProcErrSts);
            break;
          case EMCA_SECTYPE_PROCIA32: //Cache hierarchy errors
            if (mPlatformRasPolicyProtocol->SystemRasSetup->PoisonEn) { // For Poison enable case. Check for possible memory source error first.
              ErrStsHead = &MemErrSts.Header;
              Status = FillPlatformMemorySection (McBank, *Skt, Sev, &McErrSig, &MemErrSts);
              if (EFI_ERROR(Status)) {
                ErrStsHead = &ProcErrSts.Header;
                Status = FillProcessorErrorSection (McBank, *Skt, Sev, &McErrSig, &ProcErrSts);
              }
            } else {
              ErrStsHead = &ProcErrSts.Header;
              Status = FillProcessorErrorSection (McBank, *Skt, Sev, &McErrSig, &ProcErrSts);
            }
            break;
          case EMCA_SECTYPE_IIO:
            Status = HandleIoMca (&(McErrSig.Signature)); //break instead of return as bios needs to post MCE even if emca log is not created.
            if (!EFI_ERROR (Status)) {
              RASDEBUG((DEBUG_INFO, "McBankErrorHandler: CTO calling McaClearStatus. \n"));
              McaClearStatus();
              McErrSig.Signature.McSts  = 0;
              McErrSig.Signature.McMisc = 0;
              McErrSig.Signature.McAddr = 0;
              RASDEBUG((DEBUG_INFO, "McBankErrorHandler: CTO calling WriteMcBank to clear MCA. \n"));
              WriteMcBank (6, &McErrSig);
              CancelMce = TRUE;
              }
              // APTIOV_SERVER_OVERRIDE_RC_START - Log processor error when IOMCA setup option is enabled.
            if (CancelMce == FALSE)
            {
            	ErrStsHead = &ProcErrSts.Header;
            	Status = FillProcessorErrorSection (McBank, *Skt, Sev, &McErrSig, &ProcErrSts);
            }
            // APTIOV_SERVER_OVERRIDE_RC_START - Log processor error when IOMCA setup option is enabled.
            break;
          default:
            Status = EFI_UNSUPPORTED;//break instead of return as bios needs to post MCE even if emca log is not created.
            break;
        }
   #if SMCPKG_SUPPORT 
       #if 1    
         RASDEBUG ((DEBUG_ERROR, "McBank=%d,McAddr=0x%lx McSts=0x%lx, McMisc=0x%lx \n",McBank,McErrSig.Signature.McAddr,McErrSig.Signature.McSts,McErrSig.Signature.McMisc ));
       #endif
	    //Do not report when  McSts vaild is not set with McBank 3 and Address is not valid   
        //BIT58 : ADDRV-IA32_MC3_ADDR register is valid. 
        if ( McBank == 3 &&  ( ( McErrSig.Signature.McSts & BIT58) == 0 ) ){
            RASDEBUG ((DEBUG_ERROR, "Skip this\n"));  
            IgnoreError = TRUE ;
        } 
  #endif        
        if (!EFI_ERROR(Status)) {
       #if SMCPKG_SUPPORT                     
          if ( !IgnoreError ){
       #endif        
           mPlatformErrorHandlingProtocol->LogReportedError (ErrStsHead);
       #if SMCPKG_SUPPORT   
          }
       #endif    
        }

        if (CancelMce) {
          RASDEBUG((DEBUG_INFO, "McBankErrorHandler: Set ErrorFound = 0 and skip. \n"));
          CpuData->EventLog.ErrorsFound = FALSE;
        } else {
          CpuData->EventLog.ErrorsFound = TRUE;

        #if SMCPKG_SUPPORT
          //Skip this Mcbank 
          if ( IgnoreError ){ 
               RASDEBUG ((DEBUG_ERROR, "Set Sev none\n"));
               CpuData->EventLog.ErrorsFound = FALSE;
               Sev = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;
               IgnoreError = FALSE; 
          }
       #endif           
          //Update severity
          switch(Sev) {
            default:
              break;

            case EFI_ACPI_5_0_ERROR_SEVERITY_NONE:
              break;

            case EFI_ACPI_5_0_ERROR_SEVERITY_FATAL:
              CpuData->EventLog.Events[McbankSrc].SeverityMap |= 1 << SEVERITY_MAP_FATAL;
              if (SEVERITY_MAP_FATAL < MAX_ERROR_SEVERITY) {
                CpuData->EventLog.Events[McbankSrc].ErrorFound = TRUE;
                CpuData->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_FATAL].ApicId = McErrSig.ApicId;
                CpuData->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_FATAL].BankIdx = McBank;
              }
              break;

            case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE:
              CpuData->EventLog.Events[McbankSrc].SeverityMap |= 1 << SEVERITY_MAP_RECOVERABLE;

              if (SEVERITY_MAP_RECOVERABLE < MAX_ERROR_SEVERITY) {
                CpuData->EventLog.Events[McbankSrc].ErrorFound = TRUE;
                CpuData->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_RECOVERABLE].ApicId = McErrSig.ApicId;
                CpuData->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_RECOVERABLE].BankIdx = McBank;
              }
              break;

            case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED:
              CpuData->EventLog.Events[McbankSrc].SeverityMap |= 1 << SEVERITY_MAP_CORRECTED;

              if (SEVERITY_MAP_CORRECTED < MAX_ERROR_SEVERITY) {
                CpuData->EventLog.Events[McbankSrc].ErrorFound = TRUE;
                CpuData->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_CORRECTED].ApicId = McErrSig.ApicId;
                CpuData->EventLog.Events[McbankSrc].Log[SEVERITY_MAP_CORRECTED].BankIdx = McBank;
               }
              break;
          }
        }

        if (mSeverity != GetHighestSeverity (Sev, mSeverity)) {
          mSeverity = Sev;
        }

        PrevState = InterlockedCompareExchange64 ((UINT64*) &CpuData->SmmMceSyncState, SmmMceLoggingState, SmmMceClearedState);
        State = (PrevState == SmmMceLoggingState) ? SmmMceClearedState : PrevState;
        break;
      case SmmMceReadingInterrupted:
      case SmmMceProcessingInterrupted:
        //Recovery is not possible
        PrevState = InterlockedCompareExchange64 ((UINT64*) &CpuData->SmmMceSyncState, State, SmmMceClearedState);
        State = (PrevState == State) ? SmmMceClearedState : PrevState;
        break;
      case SmmMceClearedState:
        break;
      case SmmMceWritingInterrupted:
        //RASDEBUG ((DEBUG_ERROR, "McBankErrorHandler: SmmMce Writing Interrupted\n"));
        ASSERT (FALSE); //MCE handler never returns when Writing is interrupted.
        break;
      default:
        ASSERT (FALSE);
        break;
    }
  } while (State != SmmMceClearedState);

  return;
}

/**
  Gets the physical location of the processor with the given Apic ID.

  @param[in]  ApicId          - The Apic ID

  @retval BOOLEAN True if executing on local core.  Otherwise, False.

**/
BOOLEAN
ExecutingOnLocalCore ( // RASCM_TODO - move to emcalib
  IN        UINT32   ApicId
  )
{
  UINT32            LocalApicId;

  LocalApicId = GetApicID ();

  if (ApicId == LocalApicId) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Finds the first MC Bank that reports an error.

  @param[in] McBankBitField   Bit map of banks reporting errors
  @param[out] Idx             Index of first bit set

  @retval Status

**/
EFI_STATUS
FindFirstSet ( // RASCM_TODO - move to emcalib
  IN      UINT64    McBankBitField,
      OUT UINT8     *Idx
  )
{
  EFI_STATUS      Status;
  UINT8           i;

  Status = EFI_SUCCESS;
  if (McBankBitField != 0 || Idx != NULL) {
    Status = EFI_SUCCESS;
    i = 0;

    while ((McBankBitField & 0x1) == 0) {
       McBankBitField >>= 1;
       i++;
    }

    *Idx = i;
  } else {
    Status = EFI_INVALID_PARAMETER;
  }
  return Status;
}

/**
  Process Core MC Bank errors.

  @param[in] buffer     - Empty buffer
**/
VOID
ProcessCoreMcBankError (
  IN      UINT8     *Buffer
  )
{
  UINT8                       McBank;
  UINT64                      McBankBitField;
  UINT32                      ApicId;
  EFI_CPU_PHYSICAL_LOCATION   Loc;
  UINTN                       CpuNumber;
  UINT8                       Skt;
  UINT8                       Mode;

  //RASDEBUG((DEBUG_INFO,"ProcessCoreMcBankError: Inside the function \n"));
  ApicId = GetApicID ();
  ExtractProcessorLocation (ApicId, &Loc);
  Skt = (UINT8)Loc.Package;
  CpuNumber = GetTheCpuNumber((UINT8)Loc.Package, (UINT8)Loc.Core, (UINT8)Loc.Thread);
  if(CpuNumber == (UINTN)-1) {
    RASDEBUG((DEBUG_INFO,"Invalid CpuNumber \n"));
    return;
  }
  mSmmErrorMpSyncData->CpuData[CpuNumber].SmmMceSyncState = SmmMceClearedState;
  mSmmErrorMpSyncData->CpuData[CpuNumber].CoreProcessingBank = 0;

  if(mRasCapability->SiliconRasCapability.EmcaGen2Cap && (mRasSetup->Emca.EmcaMsmiEn == 2 || mRasSetup->Emca.EmcaCsmiEn == 2)) {
    Mode = MODE_EMCA;
  } else if(mRasCapability->SiliconRasCapability.EmcaGen1Cap && (mRasSetup->Emca.EmcaMsmiEn == 1 || mRasSetup->Emca.EmcaCsmiEn == 1 )){
    Mode = MODE_EMCAGEN1;
  } else {
    Mode = MODE_UMC;
  }

  McBankBitField = ReadCoreMcBankBitField (Mode);
  if(McBankBitField) {
    RASDEBUG((DEBUG_INFO,"McBankBitField: %lx \n",McBankBitField));
  }

  while (McBankBitField != 0) {
    FindFirstSet (McBankBitField,&McBank);
    mSmmErrorMpSyncData->CpuData[CpuNumber].CoreProcessingBank = (1ULL << McBank);
    McBankBitField &= ~mSmmErrorMpSyncData->CpuData[CpuNumber].CoreProcessingBank;
    ClearCoreMcBankBitField (mSmmErrorMpSyncData->CpuData[CpuNumber].CoreProcessingBank, Mode);
    McBankErrorHandler (McBank, &Skt, &mSmmErrorMpSyncData->CpuData[CpuNumber]);
    mSmmErrorMpSyncData->CpuData[CpuNumber].CoreProcessingBank = 0;

    if(Mode == MODE_EMCA) {
      McBankBitField = ReadCoreMcBankBitField (Mode);
    }

  }
}

/**
  Process Uncore MC Bank errors.

  @param[in] buffer     - Empty buffer
**/
VOID
ProcessUncoreMcBankError
(
  IN        UINT8             *Buffer
)
{
  UINT8                       McBank;
  UINT64                      McBankBitField;
  UINT32                      ApicId;
  EFI_CPU_PHYSICAL_LOCATION   Loc;
  UINTN                       CpuNumber;
  UINT8                       Skt;
  UINT8                       Mode;

  RASDEBUG ((DEBUG_INFO,"ProcessUncoreMcBankError: Inside the function \n"));

  ApicId = GetApicID ();
  ExtractProcessorLocation (ApicId, &Loc);
  Skt = (UINT8)Loc.Package;
  CpuNumber = GetTheCpuNumber((UINT8)Loc.Package, (UINT8)Loc.Core, (UINT8)Loc.Thread);
  if(CpuNumber == (UINTN)-1) {
    RASDEBUG((DEBUG_INFO,"Invalid CpuNumber \n"));
    return;
  }
  mSmmErrorMpSyncData->CpuData[CpuNumber].SmmMceSyncState = SmmMceClearedState;
  mSmmErrorMpSyncData->CpuData[CpuNumber].UncoreProcessingBank = 0;

  if(mRasCapability->SiliconRasCapability.EmcaGen2Cap && (mRasSetup->Emca.EmcaMsmiEn == 2 || mRasSetup->Emca.EmcaCsmiEn == 2)) {
    Mode = MODE_EMCA;
  } else if(mRasCapability->SiliconRasCapability.EmcaGen1Cap && (mRasSetup->Emca.EmcaMsmiEn == 1 || mRasSetup->Emca.EmcaCsmiEn == 1 )){
    Mode = MODE_EMCAGEN1;
  } else {
    Mode = MODE_UMC;
  }

  McBankBitField = ReadUncoreMcBankBitField (Mode);
  if(McBankBitField) {
    RASDEBUG((DEBUG_INFO,"McBankBitField: %lx \n",McBankBitField));
  }
#if SMCPKG_SUPPORT
{
    UINT8    i;
    //UINT64   McbankStatus = 0;
    IA32_MCI_STATUS_MSR_STRUCT            MciSts;
      
  // check Mcbank bitmap for IMC  
  if (  ! ( McBankBitField & (BIT7|BIT8) ) ) {
    //Check MSR  from MCbank 7 Status  MSR 41Dh  
    //                       8         MSR 421h
   for ( i = 0; i < 2 ; i++ ){ 
       MciSts.Data = AsmReadMsr64(0x41D+i*4);
      //check BIT63 :Valid BIT61:UC 
     if ( MciSts.Bits.val == 1 && MciSts.Bits.uc == 1 ){
       McBankBitField = (UINT64) ( 1 << (i+7) ); 
       RASDEBUG((DEBUG_INFO,"new McBankBitField: %lx \n",McBankBitField));
       break;
     }
   } 
      
  } //...... if (  ! ( McBankBitField & (BIT7|BIT8) ) ) {
}  
#endif  
  while (McBankBitField != 0) {
    FindFirstSet (McBankBitField, &McBank);
    mSmmErrorMpSyncData->CpuData[CpuNumber].UncoreProcessingBank = (1ULL << McBank);
    McBankBitField &= ~mSmmErrorMpSyncData->CpuData[CpuNumber].UncoreProcessingBank;
    ClearUncoreMcBankBitField (mSmmErrorMpSyncData->CpuData[CpuNumber].UncoreProcessingBank, Mode);
    McBankErrorHandler (McBank, &Skt, &mSmmErrorMpSyncData->CpuData[CpuNumber]);
    mSmmErrorMpSyncData->CpuData[CpuNumber].UncoreProcessingBank = 0;

    if(Mode == MODE_EMCA) {
      McBankBitField = ReadUncoreMcBankBitField (Mode);
    }
  }
}
/**
  Process Socket MC Bank errors.

  @param[in] buffer     - Empty buffer
**/
EFI_STATUS
ProcessSocketMcBankError (
  VOID
  )
{
  UINT8       Skt;

  RASDEBUG((DEBUG_INFO,"ProcessSocketMcBankError: Inside the function\n"));
  for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
    if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid != TRUE) continue;

    //
    // Uncore uncorrected error
    //
    if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid) {
      SmmStartupPackageApBlocking(Skt,
                                  ProcessUncoreMcBankError,
                                  NULL
                                 );
      SmmStartupCoreApBlkCnt(Skt,
                                  ProcessCoreMcBankError,
                                  NULL
                                 );
    }
  } // for (Skt)
  return EFI_SUCCESS;
}

/**
  Disables the CMCI signalling.

  MSR 17f - setting Bit4 disables CMCI.

  @param[in] Buffer   A pointer to a buffer

  @retval None.

**/
VOID
DisableCmci (
  IN        VOID  *Buffer
  )
{
  UINT64          Data;
  UINTN           CpuNumber;

  CpuNumber = GetAPCpuNumber ();
  Data = AsmReadMsr64 (0x17F);
  Data |= BIT4;
  AsmWriteMsr64 (0x17f, Data);
  MpSyncReleaseAp (CpuNumber);

  return;
}

/**
  Disables the CMCI signalling in the system.

  @retval None.

**/
VOID
DisableCmci_s4030032 (
  VOID
  )
{
  if (SmmStartupAllSocketAllCoreApBlocking (DisableCmci, NULL) != EFI_SUCCESS) {
    ASSERT (FALSE);
  }
}


/**
  Initialize mMpSyncData for runtime error handling.

**/
VOID
InitMpSyncGlobals (
  VOID
  )
{
  UINT32 i;

  for (i = 0; i < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); i++) {
    InitializeSpinLock (&mMpSyncData.CpuData[i].ApBusy);
  }
}


/**
  This function enables the SMI generation of uncorrected errors.

  @retval EFI_SUCCESS if the call is succeed.

**/
EFI_STATUS
EnableUncorrectableErrRep (
  UINT8 Socket
  )
{

  //
  // 1. Enable MCA banks on each cpu
  // Note: All MCA bank MCERRs are enabled at start up in InitializeMckRegs() when MCERR option is enabled.
  //

  //
  // Enable SMI for uncorrected errors, if logging needed
  //
  if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
    if ( (mRasCapability->SiliconRasCapability.EmcaGen2Cap == 1) &&
         (mRasSetup->Emca.EmcaEn == 1) &&
         (mRasSetup->Emca.EmcaMsmiEn == 2)
        ) {
      EnableEmcaSmiForUncorrectedErrors(Socket);
      if (SmmStartupCoreApBlocking (Socket, EnableMsmiBanks, NULL) != EFI_SUCCESS)
          ASSERT(FALSE);
    }

    if ( (mRasCapability->SiliconRasCapability.EmcaGen1Cap == 1) &&
         (mRasSetup->Emca.EmcaEn == 1) &&
         (mRasSetup->Emca.EmcaMsmiEn == 1)
        ) {
      if (SmmStartupPackageApBlocking (Socket, EnableEmcaSmiGen1ForUncorrectedErrors, NULL) != EFI_SUCCESS)
        ASSERT(FALSE);
    }

  }
  EnableElogFnv(); // Enable error logging for Falcon Valley reported errors
  return EFI_SUCCESS;
}

/**
  This function enables LMCE

  @retval EFI_SUCCESS if the call is succeed.

**/
EFI_STATUS
InitLMCE(
  VOID
)
{
  UINTN lp;

  if(mRasCapability->SiliconRasCapability.LmceCap) {
    for (lp = 0; lp < gSmst->NumberOfCpus; lp++) {
      if (lp == gSmst->CurrentlyExecutingCpu) {
        EnableLMCE(&mRasSetup->Emca.LmceEn);
      } else {
        SmmBlockingStartupThisAp (EnableLMCE, lp, &mRasSetup->Emca.LmceEn);
      }
    }
  }
  return EFI_SUCCESS;
}

/**
  Enables CSMI.

  @retval EFI_STATUS.

**/
EFI_STATUS
EnableCsmi (
  VOID
  )
{
  UINTN             Cpu;
  UINT8             Node;
  UINT8             mc;
  UINT8             Socket;
  EFI_STATUS        Status;
  CMCI_GEN2_PARAMS  Params;


  if (mRasSetup->Emca.EmcaCsmiEn == 2) {

    if (mRasCapability->SiliconRasCapability.EmcaGen2Cap) {

      InitializeSpinLock (&Params.Busy);

      for (Cpu = 0; Cpu < gSmst->NumberOfCpus; Cpu++) {
        if (Cpu == gSmst->CurrentlyExecutingCpu) {
          EnableCSMIGen2 (&Params);

        } else {
          AcquireSpinLock (&Params.Busy);
          Status = gSmst->SmmStartupThisAp (EnableCSMIGen2,
                                           Cpu,
                                           &Params);
          if (Status == EFI_SUCCESS) {
            AcquireSpinLock (&Params.Busy);
          }
          ReleaseSpinLock (&Params.Busy);
        }
      }

      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if(mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0) {
          continue;
        }
        for(mc = 0; mc < MAX_IMC; mc++) {
          if(mRasTopology->SystemInfo.SocketInfo[Socket].imcEnabled != 0) {
            RASDEBUG ((DEBUG_INFO, "EnableEmcaSmiForCorrectedErrors Socket 0x%lx 0x%lx\n",Socket, mc));
            EnableEmcaSmiForCorrectedErrors (Socket,mc);
          }
        }
      }
    }
  } else if(mRasSetup->Emca.EmcaCsmiEn == 1) {

    if(mRasCapability->SiliconRasCapability.EmcaGen1Cap) {
      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if(mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0) {
          continue;
        }
        for(mc = 0; mc < MAX_IMC; mc++) {
          if(mRasTopology->SystemInfo.SocketInfo[Socket].imcEnabled != 0) {
            Node = SKTMC_TO_NODE(Socket,mc);
            RASDEBUG ((DEBUG_INFO, "EnableCSMIGen1 0x%lx\n",Node));
            EnableCSMIGen1 (&Node);
          }
        }
      }
    }
  }
  return EFI_SUCCESS;
}

//
// Interfaces for reporter protocol
//

BOOLEAN
McaIsSupported (
  VOID
  )
{
  UINT8 SystemErrorEn = mRasSetup->SystemErrorEn;
  UINT8 EmcaErrorEn   = (mRasSetup->Emca.EmcaEn && (mRasCapability->SiliconRasCapability.EmcaGen1Cap || mRasCapability->SiliconRasCapability.EmcaGen2Cap));
  BOOLEAN Lmce = (mRasCapability->SiliconRasCapability.LmceCap == TRUE && mRasSetup->Emca.LmceEn == 1);
  RASDEBUG ((DEBUG_INFO, "<McaIsSupported>\n"));
  return (SystemErrorEn && (EmcaErrorEn || Lmce));
}

EFI_STATUS
McaEnableReporting (
    VOID
    )
{
  UINT8 Socket;

  RASDEBUG((DEBUG_INFO, "<McaEnableReporting>\n"));

  //SiWAForInit();
  if(InitSmmRuntimeCtlHooks() == EFI_SUCCESS) {
    SetEmcaHandlerState(SmmMceNotStartedState);
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
      // Init Uncorrected processor errors / Enable SMI routing
      EnableUncorrectableErrRep(Socket);
    }
  }
   // Program corrected error signals
  EnableCsmi();

  //Enable LMCE
  InitLMCE();

  // Initilize PPR data for memory error reporting
  InitializePPRData();

  return EFI_SUCCESS;
}

EFI_STATUS
McaDisableReporting (
  UINT32 DeviceType,
  UINT32 EventType,
  UINT32 *ExtData
  )
{
  // Not implemented for Purley.  OEM can implement at their discretion.
  return EFI_UNSUPPORTED;
}

/**
  Checks for pending processor errors.

  @retval TRUE  - Pending error exists
          FALSE - No pending error exists

**/
BOOLEAN
McaCheckStatus (
  VOID
  )
{
  UINT8       Socket;
  UINTN       Thread;
  BOOLEAN     ErrorFound;
  UINT8       Ch;         // at socket level
  UINT8       Dimm;       // with in channel
  BOOLEAN     Priority;
  UINT32      Log;
  EFI_STATUS  Status = 0;

  //RASDEBUG ((DEBUG_INFO, "<McaCheckStatus>\n"));
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {

    //
    // Ignore disabled nodes
    //
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0) {
      continue;
    }

    if((mRasCapability->SiliconRasCapability.EmcaGen2Cap || mRasCapability->SiliconRasCapability.EmcaGen1Cap) &&
        CheckErrLogReg(Socket,mRasSetup->Emca.EmcaMsmiEn,mRasSetup->Emca.EmcaCsmiEn)) {
        //RASDEBUG ((DEBUG_ERROR, "CheckErrLogReg TRUE\n"));
      return TRUE;
    }
  }
  if((mRasSetup->Emca.EmcaCsmiEn == 2 || mRasSetup->Emca.EmcaMsmiEn == 2) && (mRasCapability->SiliconRasCapability.EmcaGen2Cap == TRUE)) {
    ErrorFound = FALSE;
    for (Thread = 0; Thread < gSmst->NumberOfCpus; Thread++) {
      if (Thread == gSmst->CurrentlyExecutingCpu) {
        EmcaSmiSrc (&ErrorFound);
      } else {
        SmmBlockingStartupThisAp (EmcaSmiSrc, Thread, &ErrorFound);
      }
      if(ErrorFound == TRUE) {
        //RASDEBUG ((DEBUG_ERROR, "EmcaSmiSrc TRUE\n"));
        return TRUE;
      }
    }
  }

  if (mRasSetup->Memory.FnvErrorEn) {
      // check to see if AEP DIMM has any error that are not covered by MCAbanks
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      for (Ch = 0; Ch < MAX_CH; Ch++) {
        for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
          for (Priority = 0; Priority <= 1; Priority++) {
           Status = mCrystalRidge->NvmCtlrErrorLogDetectError(Socket, Ch, Dimm, Priority, DO_NOT_CLEAR_ON_READ,  &Log);
            if (Status == EFI_SUCCESS) {
              return TRUE;
            }
          }
        }
      }
    }
  }
  return FALSE;
}

EFI_STATUS
McaDetectAndHandle (
  UINT32      *Severity,
  ClearOption  Clear       // Flag. 0 - do not clear the detected/handled error status. 1 - clear it.
  )
{
  EFI_STATUS      Status;
  UINT64                            PrevState;
  UINT64                            State;

  RASDEBUG ((DEBUG_INFO, "<McaDetectAndHandle>\n"));

  Status = EFI_SUCCESS;

  // For processor uncorrected error, mSmmErrorMpSyncData contains severity information
  *Severity = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;
  State = SmmMceNotStartedState;

  do {
    //RASDEBUG ((DEBUG_ERROR, "McBankErrorHandler : Skt = %d, McBank = %d, State = %d\n", *Skt, McBank, State));
    switch(State) {
      case SmmMceNotStartedState:
        PrevState = TransitionEmcaHandlerState(SmmMceNotStartedState,SmmMceInProgressState);
        State = (PrevState == SmmMceNotStartedState) ? SmmMceInProgressState : PrevState;
        break;
      case SmmMceInProgressState:
        ProcessSocketMcBankError();
        PrevState = TransitionEmcaHandlerState(SmmMceInProgressState,SmmMceFinishedState);
        State = (PrevState == SmmMceInProgressState) ? SmmMceFinishedState : PrevState;
        break;
      case SmmMceInterruptedState:
        PrevState = TransitionEmcaHandlerState(SmmMceInterruptedState,SmmMceInProgressState);
        State = (PrevState == SmmMceInterruptedState) ? SmmMceInProgressState : PrevState;
        break;
      default:
        ASSERT (FALSE);
        break;
    }
  } while (State != SmmMceFinishedState);
  UpdateLmceStatus(mSmmErrorMpSyncData);

  if (*Severity != GetHighestSeverity (mSeverity, *Severity)) {
    *Severity = mSeverity;
  }

  FnvErrorHandler(); // Falcon Valley Error Handling - NGN Memory Errors

  return Status;
}

EFI_STATUS
McaClearStatus (
  VOID
  )
{
  UINT8  Mode;
  UINT8  Socket;
  UINT8 mc;

  if(mRasCapability->SiliconRasCapability.EmcaGen2Cap && (mRasSetup->Emca.EmcaMsmiEn == 2 || mRasSetup->Emca.EmcaCsmiEn == 2)) {
    Mode = MODE_EMCA;
  } else if(mRasCapability->SiliconRasCapability.EmcaGen1Cap && (mRasSetup->Emca.EmcaMsmiEn == 1 || mRasSetup->Emca.EmcaCsmiEn == 1)) {
    Mode = MODE_EMCAGEN1;
  } else {
    Mode = MODE_UMC;
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {

    //
    // Ignore disabled nodes
    //
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0) {
      continue;
    }

    if(Mode == MODE_EMCA || Mode == MODE_EMCAGEN1) {
      if(!GetPendSmiFlag()) {
        //RASDEBUG ((DEBUG_ERROR, "ClearErrLogReg TRUE\n"));
        ClearErrLogReg(Socket,mRasSetup->Emca.EmcaMsmiEn,mRasSetup->Emca.EmcaCsmiEn);
        for(mc = 0; mc < MAX_IMC; mc++) {
          if(mRasTopology->SystemInfo.SocketInfo[Socket].imcEnabled != 0) {
            ClearMciShadows(Socket,mc);
          }
        }
      }
    } else if(Mode == MODE_UMC) {
      //RASDEBUG ((DEBUG_ERROR, "ClearUmcStatus TRUE\n"));
      ClearUmcStatus(Socket);
    }
  }

  return EFI_SUCCESS;
}

VOID
EnableCloakingPerSocket (
  UINT8 *buffer
  )
/**
  Enables Cloaking per socket, by disabling the CMCI in MSR 17F and by enabling bits 0&1 in MSR 52
  so that read to MC status registers will return 0 for UCNA & corrected Errors

  @param[in]  *buffer         - Not Used.
  @param[out] None
  @retval     None

**/
{
  UINT64  RegValue_McaErrorControl;
  UINT64  RegValue_ErrorControl;

  RegValue_McaErrorControl = AsmReadMsr64(MSR_MCA_ERROR_CONTROL);
  RegValue_McaErrorControl |= (BIT0 | BIT1);
  AsmWriteMsr64(MSR_MCA_ERROR_CONTROL, RegValue_McaErrorControl);

  RegValue_ErrorControl = AsmReadMsr64(MSR_ERROR_CONTROL);
#if SMCPKG_SUPPORT 
  RegValue_ErrorControl |= (BIT4|BIT5);
#else  
  RegValue_ErrorControl |= BIT4;
#endif  
  AsmWriteMsr64(MSR_ERROR_CONTROL, RegValue_ErrorControl);
  ReleaseSpinLock(&mApLock);
  return;
}

VOID
EnableCloaking (
  VOID
  )
/**
  Enables Cloaking Bits for all the sockets using SmmStartupThisAp()

  @param[in]  None
  @param[out] None
  @retval     None

**/
{
  UINT8  socket;
  UINT8  thread;
  BOOLEAN  FoundThread = FALSE;
  EFI_PROCESSOR_INFORMATION  ProcInfo;
  EFI_STATUS  Status;
  UINT64  RegValue_McaErrorControl;
  UINT64  RegValue_ErrorControl;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //
    // Ignore disabled nodes
    //
    if ((mRasTopology->SystemInfo.SocketBitMap & (1 << socket)) == 0)
      continue;

    if (ExcutingOnLocalNode (socket) == TRUE) {
      RegValue_McaErrorControl = AsmReadMsr64(MSR_MCA_ERROR_CONTROL);
      RegValue_McaErrorControl |= (BIT0 | BIT1);
      AsmWriteMsr64(MSR_MCA_ERROR_CONTROL, RegValue_McaErrorControl);
      RegValue_ErrorControl = AsmReadMsr64(MSR_ERROR_CONTROL);
#if SMCPKG_SUPPORT
      RegValue_ErrorControl |= BIT5;
#else	  
      RegValue_ErrorControl |= BIT4;
#endif	  
      AsmWriteMsr64(MSR_ERROR_CONTROL, RegValue_ErrorControl);
    } else {
      InitializeSpinLock(&mApLock);
      FoundThread = FALSE;
      for(thread = 0; thread < gSmst->NumberOfCpus; thread++) {
        if(mSmmCpuService->GetProcessorInfo(mSmmCpuService,thread, &ProcInfo) == EFI_SUCCESS) {
          if((ProcInfo.Location.Package == socket) && (ProcInfo.StatusFlag & PROCESSOR_ENABLED_BIT) != 0) {
            FoundThread = TRUE;
            break;
          }
        }
      }
      if (FoundThread){
        AcquireSpinLock(&mApLock);
        Status = gSmst->SmmStartupThisAp(EnableCloakingPerSocket, thread, &socket);
        if(Status == EFI_SUCCESS) {
          while ( (AcquireSpinLockOrFail(&mApLock)) != TRUE  ) {
            CpuPause();
          }
        }
      ReleaseSpinLock(&mApLock);
      }
    }
  }

  return;
}

//VOID
//InitializePPRData(VOID)
//{
//  EFI_STATUS          Status;
//  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;
//  PPR_ADDR_VARIABLE pprData;
//  UINTN             varSize;
//
//  varSize = sizeof(PPR_ADDR_VARIABLE);
//  ZeroMem (&pprData, varSize);
//
//  //
//  // Clear PPR Variable from storage
//  //
//  Status = gRT->SetVariable (
//      PPR_VARIABLE_NAME,
//      &gEfiPprVariableGuid,
//      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
//      varSize,
//      &pprData
//  );
//  //
//  // Mark PPR_VARIABLE_NAME variable to read-only if the Variable Lock protocol exists
//  // Still lock it even the variable cannot be saved to prevent it's set by 3rd party code.
//  //
//  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
//  if (!EFI_ERROR (Status)) {
//    Status = VariableLock->RequestToLock (VariableLock, PPR_VARIABLE_NAME, &gEfiPprVariableGuid);
//    ASSERT_EFI_ERROR (Status);
//  }
//}

/**
  Entry point for the Processor Handler initialization.

  This function initializes the error handling and enables error
  sources for the CPU Core.


  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
EFIAPI
InitializeProcessorErrHandler (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS                Status;
  //BOOLEAN                   SkipRasCode = 1;
  //if (SkipRasCode == 1) {
  //  return EFI_SUCCESS;
  //}
  // APTIOV_SERVER_OVERRIDE_RC_START : Added support for Last Boot Error Logging
  #if HandleLastbootError_Support
    struct SystemMemoryMapHob             *systemMemoryMap;
    EFI_HOB_GUID_TYPE                     *GuidHob;
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Added support for Last Boot Error Logging

  RASDEBUG ((EFI_D_INFO, "Entering InitializeProcessorErrHandler (%x) \n", InitializeProcessorErrHandler));
  Status = gSmst->SmmLocateProtocol (
                    &gEfiErrorHandlingProtocolGuid,
                    NULL,
                    &mErrorHandlingProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformErrorHandlingProtocolGuid,
                    NULL,
                    &mPlatformErrorHandlingProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
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
  mSmmErrorMpSyncData = mPlatformErrorHandlingProtocol->SmmErrorMpSyncData;

  Status = gSmst->SmmLocateProtocol (
      &gEfiMemRasProtocolGuid,
      NULL,
      &mMemRas
      );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
      &gEfiCpuCsrAccessGuid,
      NULL,
      &mCpuCsrAccess
      );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiCrystalRidgeSmmGuid, NULL, &mCrystalRidge);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuServiceProtocolGuid,
                    NULL,
                    &mSmmCpuService
                    );
  ASSERT_EFI_ERROR (Status);

  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID**)&mSmmVariable);
  // APTIOV_SERVER_OVERRIDE_RC_START : Added support for Last Boot Error Logging
  	  GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
  	  if ( GuidHob != NULL) {
  	        systemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA (GuidHob);
  	  }
  	  else {
  	        systemMemoryMap = NULL;
  	        Status = EFI_DEVICE_ERROR;
  	        ASSERT_EFI_ERROR (Status);
  	  }

#if HandleLastbootError_Support    
    if ( ( systemMemoryMap != NULL )  && ( systemMemoryMap->IsColdBoot != TRUE ) ) {
        LastBootErrorHandler();
    }
#endif
    // APTIOV_SERVER_OVERRIDE_RC_END : Added support for Last Boot Error Logging
  // Perform initialization here
  if(McaIsSupported()) {

    McaEnableReporting();
    // APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook support for Enabling/Disabling MCA Error Reporting Registers
    OemMcaErrEnableDisableReporting(mPlatformRasPolicyProtocol);  
    // APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook support for Enabling/Disabling MCA Error Reporting Registers

    //
    // Create the REPORTER_PROTOCOL packet
    //
    mMcaReporter.IsSupported = McaIsSupported;
    mMcaReporter.EnableReporting = McaEnableReporting;
    mMcaReporter.DisableReporting = McaDisableReporting;
    mMcaReporter.CheckStatus = McaCheckStatus;
    mMcaReporter.ClearStatus = McaClearStatus;
    mMcaReporter.DetectAndHandle = McaDetectAndHandle;

    // RASCM_TODO - publish the ERROR_REPORTER protocol, so that O*L code can use it

    // Register the Mca Error Handler
    mErrorHandlingProtocol->RegisterHandler (&mMcaReporter, HighPriority);

    mPlatformErrorHandlingProtocol->RegisterNotifier (ProcessorNotifyOs, 0x1);

    if (mRasCapability->SiliconRasCapability.EmcaGen2Cap == TRUE && mRasSetup->Emca.EmcaEn == 1) {
      RASDEBUG ((DEBUG_INFO, "Installing MCE Handler..."));
      Status = mSmmCpuService->RegisterExceptionHandler (mSmmCpuService, EXCEPT_IA32_MACHINE_CHECK, SmiMcaHandler);
      if (Status == EFI_SUCCESS) {
        RASDEBUG ((DEBUG_INFO, "Success!\n"));
      } else {
        RASDEBUG ((DEBUG_INFO, "Failed!\n"));
      }
      RASDEBUG ((DEBUG_INFO, "SmiMcaHandler Address = %x\n", SmiMcaHandler));
    }
  }else {
    RASDEBUG ((DEBUG_INFO, "<McaIsNotSupported>\n"));
  }

  if(mRasSetup->CloakingEn == TRUE) {
      RASDEBUG ((EFI_D_INFO, "System Cloaking is enabled.\n"));
      EnableCloaking ();
    } else RASDEBUG ((EFI_D_INFO, "System Cloaking is disabled.\n"));

  return Status;
}
