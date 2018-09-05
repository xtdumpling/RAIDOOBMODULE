/** @file
  Implementation of the MCE Error Handler for eMCA Gen 2.

  Copyright (c) 2009-2015 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

//#include <Library/emcaplatformhookslib.h>
#include <Library/ProcMemErrReporting.h>
#include <Library/ResetSystemLib.h>
#include <Library/TimerLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include "CommonErrorHandlerDefs.h"
#include "McaHandler.h"
#include "ProcessorErrorHandler.h"
#include "MemoryErrorHandler.h"
#include "ErrorRecords.h"
#include <Library/WheaSiliconHooksLib/DimmIsolationFlow.h>

/**
  Prints a message to the serial port.

  @param  Format      Format string for the message to print.
  @param  ...         Variable argument list whose contents are accessed 
                      based on the format string specified by Format.

**/
VOID
EFIAPI
McePrint (
  IN  CONST CHAR8  *Format,
  ...
  )
{
  CHAR8    Buffer[250];
  VA_LIST  Marker;

  //
  // Convert the message to an ASCII String
  //
  VA_START (Marker, Format);
  AsciiVSPrint (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);

  //
  // Send the print string to a Serial Port 
  //
  while (!AcquireSpinLockOrFail (&mSmmErrorMpSyncData->SerialLock)) {
      CpuPause ();
  }
  SerialPortWrite ((UINT8 *)Buffer, AsciiStrLen (Buffer));
  ReleaseSpinLock(&mSmmErrorMpSyncData->SerialLock);
}

/**
  Shutdown.
**/
VOID
MceShutdown(
  UINT8 Policy
  )
{
  if(Policy == 0) {
    CpuDeadLoop();
  } else {
    ResetWarm();
  }  
}


/**
  Returns whether machine check is in progress.

  @retval BOOLEAN - True/False

**/
BOOLEAN
IsMcip(
  VOID
)
{
  IA32_MCG_STATUS_MSR_STRUCT McgStatus;
  
  McgStatus.Data = AsmReadMsr32 (IA32_MCG_STATUS);

  return (McgStatus.Bits.mcip == 1);
}

/**
  Returns whether IP is related to error.

  @retval BOOLEAN - True/False

**/
BOOLEAN
IsEipv(
  VOID
)
{
  IA32_MCG_STATUS_MSR_STRUCT McgStatus;
  
  McgStatus.Data = AsmReadMsr32 (IA32_MCG_STATUS);

  return (McgStatus.Bits.eipv == 1);
}


/**
  Returns whether restart IP is valid.

  @retval BOOLEAN - True/False

**/
BOOLEAN
IsRipv(
  VOID
  )
{
  IA32_MCG_STATUS_MSR_STRUCT McgStatus;
  
  McgStatus.Data = AsmReadMsr32 (IA32_MCG_STATUS);

  return (McgStatus.Bits.ripv == 1);
}

/**
  Clear Machine Check in progress bit.

**/
VOID
ClearMcip(
  VOID
  )
{
  IA32_MCG_STATUS_MSR_STRUCT McgStatus;
  McgStatus.Data = AsmReadMsr32 (IA32_MCG_STATUS);
  McgStatus.Bits.mcip = 0;
  AsmWriteMsr32 (IA32_MCG_STATUS, McgStatus.Data);  
}

BOOLEAN
DetectSmmErrorContention
(
  IN        SMM_MCA_CPU_INFO        *CpuInfo,
  IN   EMCA_MC_SIGNATURE_ERR_RECORD * McSig
)
{
  IA32_MCI_STATUS_MSR_STRUCT        MciStsMsr;                        
  EFI_PHYSICAL_ADDRESS PhyAddr;
  /*
   * Need to add logic to check for MMIO address and detect a safe range to return to. 
  */
  
  MciStsMsr.Data = McSig->Signature.McSts;
  if(MciStsMsr.Bits.addrv == 1 && MciStsMsr.Bits.miscv == 1) {
    
    PhyAddr = (EFI_PHYSICAL_ADDRESS) MCAddress (McSig->Signature.McAddr, McSig->Signature.McMisc); 
    
    //
    // Check if the error is in SMRAM to avoid returning to the SMM handler if the cause of error is the handler.
    //
    if (!SmmIsBufferOutsideSmmValid (PhyAddr, 64)) {
      //McePrint("MCE %ld: MCAddress is in SMM\n",CpuInfo->CpuIndex);
      return TRUE;
    } 
  } else  {
    return TRUE;
  }
  
  return FALSE;
}

VOID
DetectHandleMcBankContention
(
  IN        SMM_MCA_CPU_INFO        *CpuInfo,
  IN        UINT64                   *SmiSrc,
  IN        UINT64                   *ProcessingBank
)
{
  UINTN                             CpuNumber;
  IA32_MCI_STATUS_MSR_STRUCT        MciStsMsr;
  UINT32                            MsrAddr;  
  UINT8                             McBank;
  
  CpuNumber = CpuInfo->CpuIndex;    
  
  if ((*SmiSrc & *ProcessingBank) != 0) {
        switch (mSmmErrorMpSyncData->CpuData[CpuNumber].SmmMceSyncState) {
          case SmmMceReadingState:
            mSmmErrorMpSyncData->CpuData[CpuNumber].SmmMceSyncState = SmmMceReadingInterrupted; 
            break;
          case SmmMceProcessingState:
            mSmmErrorMpSyncData->CpuData[CpuNumber].SmmMceSyncState = SmmMceProcessingInterrupted;
            break;
          case SmmMceWritingState:
            FindFirstSet (*ProcessingBank, &McBank);
            MsrAddr = GetAddrMcBankSts (McBank);
            MciStsMsr.Data = AsmReadMsr64 (MsrAddr);
            MciStsMsr.Bits.uc = 1;
            MciStsMsr.Bits.val = 1;
            MciStsMsr.Bits.pcc = 1;
            MciStsMsr.Bits.over  = 1;
            MciStsMsr.Bits.other_info |= MCA_FIRMWARE_UPDATED_BIT;
            AsmWriteMsr64(MsrAddr,MciStsMsr.Data);            
            mSmmErrorMpSyncData->CpuData[CpuNumber].SmmMceSyncState = SmmMceWritingInterrupted;
            break;
          case SmmMceLoggingState:
          case SmmMceClearedState:
            //Do nothing
            break;
          default:
            ASSERT (FALSE);
            break;
        }
      }  
}

/**
  Process core errors
  @param  CpuInfo     Cpu Information structure

**/
VOID
McaProcessErrors (
  IN        SMM_MCA_CPU_INFO        *CpuInfo,
  IN        UINT64                   *SmiSrc,
  IN        UINT64                   *ProcessingBank
) 
{
  UINTN                             CpuNumber;
  UINT64                            NewErrorsBitmap;
  UINT8                             McBank;
  IA32_MCI_STATUS_MSR_STRUCT        MciStsMsr;
  EMCA_MC_SIGNATURE_ERR_RECORD      McErrSig;
  EFI_STATUS                        Status;
  MCE_CLASS                         MceClass;
  CpuNumber = CpuInfo->CpuIndex;

  switch(GetEmcaHandlerState()) {
    case SmmMceNotStartedState:
      //nothing to do but return
      return;
      break;
    case SmmMceInProgressState:
      DetectHandleMcBankContention(CpuInfo,SmiSrc,ProcessingBank);
      break;
    case SmmMceFinishedState:
      SetPendSmiFlag(TRUE);
      break;
  }
  NewErrorsBitmap = *SmiSrc;
  while (NewErrorsBitmap != 0) {
    //McePrint("MCE %ld: SMISrc %lx \n",CpuNumber,NewErrorsBitmap);
    FindFirstSet (NewErrorsBitmap, &McBank);
    //McePrint("MCE %ld: McBank = %d\n",CpuNumber, McBank);
    
    NewErrorsBitmap &= ~(1ULL << McBank);
    
    Status = ReadMcBankSigHook (McBank, (UINT8) CpuInfo->Loc.Package, &McErrSig);
    MciStsMsr.Data = McErrSig.Signature.McSts;
    /*McePrint("MCE %ld: MCi_STS: %lx, MCi_ADDR: %lx, MCi_MISC: %lx, APICID: %lx\n", CpuNumber,
                                                                                   McErrSig.Signature.McSts, 
                                                                                   McErrSig.Signature.McAddr, 
                                                                                   McErrSig.Signature.McMisc, 
                                                                                   McErrSig.ApicId
             );
    */
    MceClass = GetMceClass(&MciStsMsr);
    if(MciStsMsr.Bits.over) {
      //TBD
      //McePrint("MCE %ld: Overflow\n",CpuInfo->CpuIndex);
    }

    switch(MceClass) {
      case INVALID:
        //McePrint("MCE %ld: MCE Class INVALID: %ld\n",CpuInfo->CpuIndex,MceClass);
        // Proceed
        break;
      case CORRECTED:
        //McePrint("MCE %ld: MCE Class CORRECTED: %ld\n",CpuInfo->CpuIndex,MceClass);
        // Proceed
        break;
      case UCNA:
        //McePrint("MCE %ld: MCE Class UCNA: %ld\n",CpuInfo->CpuIndex,MceClass);
        // Proceed
        break;
      case SRAO:
        //McePrint("MCE %ld: MCE Class SRAO: %ld\n",CpuInfo->CpuIndex,MceClass);
        // Proceed
        break;
      case SRAR:
        //McePrint("MCE %ld: MCE Class SRAR: %ld\n",CpuInfo->CpuIndex,MceClass);
        if(DetectSmmErrorContention(CpuInfo,&McErrSig)) {
          //McePrint("MCE %ld: Smm Error Contention\n",CpuInfo->CpuIndex);
          //McePrint("MCE %ld: Perform Reset\n",CpuInfo->CpuIndex);
          MceShutdown(0);
        }
        break;
      case DUE:
        //McePrint("MCE %ld: MCE Class DUE: %ld\n",CpuInfo->CpuIndex,MceClass);
        //McePrint("MCE %ld: Perform Reset\n",CpuInfo->CpuIndex);
        MceShutdown(0);
        break;
      case FATAL:
        //McePrint("MCE %ld: MCE Class FATAL: %ld\n",CpuInfo->CpuIndex,MceClass);
        //McePrint("MCE %ld: Perform Reset\n",CpuInfo->CpuIndex);
        MceShutdown(0);
        break;
    }
  }
}


/**
  Process core errors
  @param  CpuInfo     Cpu Information structure

**/
VOID
McaProcessCoreErrors (
  IN        SMM_MCA_CPU_INFO        *CpuInfo
)
{
  UINT64                            CoreSmiSrc;
  UINTN                             CpuNumber;
  
  CpuNumber = CpuInfo->CpuIndex;
  
  //McePrint("t %ld: Core handler\n", CpuNumber);
  CoreSmiSrc = ReadCoreSmiSrc ();
  McaProcessErrors(CpuInfo,&CoreSmiSrc,(UINT64 *) &mSmmErrorMpSyncData->CpuData[CpuNumber].CoreProcessingBank);    
}

/**
  Process uncore errors
  @param  CpuInfo     Cpu Information structure

**/
VOID
McaProcessUncoreErrors (
  IN        SMM_MCA_CPU_INFO        *CpuInfo
)
{
  UINT64                            UncoreSmiSrc;
  UINTN                             CpuNumber;

  CpuNumber = CpuInfo->CpuIndex;

  
  
  //McePrint("t %ld: Uncore handler\n",CpuNumber);
  UncoreSmiSrc = ReadUncoreSmiSrc ();
  McaProcessErrors(CpuInfo,&UncoreSmiSrc,(UINT64 *) &mSmmErrorMpSyncData->CpuData[CpuNumber].UncoreProcessingBank);   
}

/**
  Fill CpuInfo Structure with cpu data
  
  @param  CpuInfo     Cpu Information structure

**/
EFI_STATUS
GetMcaCpuInfo(
  SMM_MCA_CPU_INFO  *CpuInfo,
  EFI_EXCEPTION_TYPE    *InterruptType,
  EFI_SYSTEM_CONTEXT    *SystemContext
)
{
  ASSERT (CpuInfo != NULL);
  ASSERT (InterruptType != NULL);
  ASSERT (SystemContext != NULL);

  if(CpuInfo == NULL || InterruptType == NULL || SystemContext == NULL ) {
    return EFI_INVALID_PARAMETER;
  }
  
  CpuInfo->InterruptType = *InterruptType;
  CpuInfo->SystemContext = *SystemContext;
  
  CpuInfo->ApicId = GetApicID ();
  if(ExtractProcessorLocation(CpuInfo->ApicId,&CpuInfo->Loc) != EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }
  CpuInfo->CpuIndex = GetTheCpuNumber((UINT8)CpuInfo->Loc.Package, (UINT8)CpuInfo->Loc.Core, (UINT8)CpuInfo->Loc.Thread); 
  if(CpuInfo->CpuIndex == (UINTN)-1) {
    return EFI_NOT_FOUND;;
  }

  return EFI_SUCCESS;
}



/**
  MCA handler for SMM use.

**/
VOID
EFIAPI
SmiMcaHandler (
  IN        EFI_EXCEPTION_TYPE    InterruptType,
  IN        EFI_SYSTEM_CONTEXT    SystemContext
)
{
  SMM_MCA_CPU_INFO CpuInfo;

  if(GetMcaCpuInfo(&CpuInfo,&InterruptType,&SystemContext) != EFI_SUCCESS) {
    //McePrint("MCE %ld: Failed to get cpu data: \n",CpuInfo.CpuIndex);
    ASSERT(FALSE);
    CpuDeadLoop();
    return;
  }
  //McePrint("MCE %ld: SmiMcaHandler Entry: \n",CpuInfo.CpuIndex);

  if (IsMcip () == FALSE) {
    //McePrint("MCE %ld: MCIP == 0\n",CpuInfo.CpuIndex);
    ASSERT(FALSE);
    MceShutdown(0);
    return;
  } 
  
  if(IsRipv() == 0) {
    //McePrint("MCE %ld: RIPV == 0\n",CpuInfo.CpuIndex);
    //McePrint("MCE %ld: Perform Reset\n",CpuInfo.CpuIndex);
    MceShutdown(0);    
  }

  ASSERT (mRasCapability->SiliconRasCapability.EmcaGen2Cap == TRUE);
  
  McaProcessUncoreErrors (&CpuInfo);
  McaProcessCoreErrors (&CpuInfo);
  

  //McePrint ("MCE %ld: MceRestart\n",CpuInfo.CpuIndex);
  ClearMcip ();
  
}

