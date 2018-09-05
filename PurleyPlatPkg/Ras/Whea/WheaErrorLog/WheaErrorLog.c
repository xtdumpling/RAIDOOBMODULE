/** @file
  Implementation of the WHEA Error Logger.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#include <Setup/IioUniversalData.h>
#include <Protocol/ErrorHandlingProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/WheaPlatformHooksLib.h>
#include <Library/mpsyncdatalib.h>
#include "WheaErrorLog.h"
#include "ErrorRecords.h"

// Module global data
EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
EFI_WHEA_PLATFORM_SUPPORT_PROTOCOL    *mWheaPlatformSupportProtocol;

extern  EFI_GUID  gErrRecordProcGenErrGuid;
extern  EFI_GUID  gErrRecordProcSpecErrGuid;
extern  EFI_GUID  gErrRecordPlatformMemErrGuid;
extern  EFI_GUID  gErrRecordVtdErrGuid;
extern  EFI_GUID  gErrRecordPcieErrGuid;

BOOLEAN   IsWheaEnabled = FALSE;    // OS not enabled WHEA yet
BOOLEAN   IsCeLogged = FALSE;    // No CE logged yet
BOOLEAN   IsUeFatalLogged = FALSE;    // No UE/Fatal logged yet
BOOLEAN   IsErrorSourceClear = FALSE;    // Have the error sources been cleared?

/**
  Enable WHEA Logging

  This function enables WHEA logging on the platform.
  

  @param[in] DispatchHandle       Dispatch handle
  @param[in] Context              Context information
  @param[in] CommunicationBuffer  Buffer used for communication between caller/callback function
  @param[in] SourceSize           Size of communication buffer

  @retval Status.

**/
EFI_STATUS
EnableWheaLogging (
  IN      EFI_HANDLE    DispatchHandle,
  IN      CONST VOID    *DispatchContext,
  IN OUT  VOID          *CommBuffer,
  IN OUT  UINTN         *CommBufferSize
  )
{
  IsWheaEnabled = TRUE; 
  //
  // Call platform hook for SKU or board specific programming
  //
  ProgramWheaSignalsHook (IsWheaEnabled, NULL, gSmst);

  return EFI_SUCCESS;
}

/**
  Disable WHEA Logging

  This function disables WHEA logging on the platform.
  

  @param[in] DispatchHandle       Dispatch handle
  @param[in] Context              Context information
  @param[in] CommunicationBuffer  Buffer used for communication between caller/callback function
  @param[in] SourceSize           Size of communication buffer

  @retval Status.

**/
EFI_STATUS
DisableWheaLogging (
  IN      EFI_HANDLE    DispatchHandle,
  IN      CONST VOID    *DispatchContext,
  IN OUT  VOID          *CommBuffer,
  IN OUT  UINTN         *CommBufferSize
  )
{
  IsWheaEnabled = FALSE;
  //
  // Call platform hook for SKU or board specific programming
  //
  ProgramWheaSignalsHook (IsWheaEnabled, NULL, gSmst);

  return EFI_SUCCESS;
}

/**
  Entry point for the WHEA Error Handler initialization. 

  This function initializes WHEA Error handling.
  

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
EFIAPI
InitializeWheaErrorLogger (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_WHEA_SUPPORT_PROTOCOL       *WheaSupportProtocol;
  UINTN                           i;
  EFI_HANDLE                      SwHandle;
  EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL   *SwDispatch  = 0;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiPlatformRasPolicyProtocolGuid,
                  NULL,
                  &mPlatformRasPolicyProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((mPlatformRasPolicyProtocol->SystemRasCapability->PlatformRasCapability.WheaLogCap == 0) ||
       (mPlatformRasPolicyProtocol->SystemRasSetup->Whea.WheaSupportEn == 0)                    ||
         ((mPlatformRasPolicyProtocol->SystemRasSetup->Whea.WheaLogPciEn == 0)     && 
          (mPlatformRasPolicyProtocol->SystemRasSetup->Whea.WheaLogMemoryEn == 0)  &&
          (mPlatformRasPolicyProtocol->SystemRasSetup->Whea.WheaLogProcEn == 0))
      )  {
    return EFI_SUCCESS;
  }
  
  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformErrorHandlingProtocolGuid,
                    NULL,
                    &mPlatformErrorHandlingProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                &gEfiWheaSupportProtocolGuid, 
                  NULL, 
                &WheaSupportProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid, 
                    NULL, 
                    &SwDispatch);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  InitWheaPlatformHookLib();

  SwContext.SwSmiInputValue = EFI_WHEA_ENABLE_SWSMI;
  Status = SwDispatch->Register (
                        SwDispatch, 
                        EnableWheaLogging, 
                        &SwContext, 
                        &SwHandle);
  ASSERT_EFI_ERROR (Status);

  SwContext.SwSmiInputValue = EFI_WHEA_DISABLE_SWSMI;
  Status = SwDispatch->Register (
                        SwDispatch, 
                        DisableWheaLogging, 
                        &SwContext, 
                        &SwHandle);
  ASSERT_EFI_ERROR (Status);
                                
  for (i = 0; i < NumNativeErrorSources; i++) {
    Status = WheaSupportProtocol->AddErrorSource(
                                    WheaSupportProtocol,
                                    SysNativeErrSources[i].Type,
                                    SysNativeErrSources[i].Flags,
                                    TRUE,
                                    &SysNativeErrSources[i].SourceId,
                                    SysNativeErrSources[i].NumOfRecords,
                                    SysNativeErrSources[i].MaxSecPerRecord,
                                    SysNativeErrSources[i].SourceData);
    ASSERT_EFI_ERROR (Status);
  }
#ifdef RASCM_TODO 
  // Need to revisit the Error SysGenErrSources  
#endif
  for (i = 0; i < NumSysGenericErrorSources; i++) {
    Status = WheaSupportProtocol->AddErrorSource(
                                    WheaSupportProtocol,
                                    GenericHw,
                                    SysGenErrSources[i].Flags,
                                    TRUE,
                                    &SysGenErrSources[i].SourceId,
                                    SysGenErrSources[i].NumOfRecords,
                                    SysGenErrSources[i].MaxSecPerRecord,
                                    SysGenErrSources[i].SourceData);
    ASSERT_EFI_ERROR (Status);
  }
  
  Status = mPlatformErrorHandlingProtocol->RegisterErrorListener (LogErrorToWhea);
  ASSERT_EFI_ERROR (Status);

  Status = mPlatformErrorHandlingProtocol->RegisterNotifier (WheaNotifyOs, 0x00);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Find Generic Error Source

  This function searches for a generic error source of the type specified.
  

  @param[in]  Type        Error Type
  @param[out] **ErrSrc    Pointer to the pointer to the error source  

  @retval Status.

**/
EFI_STATUS
FindSysGenErrorSource(
  IN      UINT16                        Type,
     OUT  SYSTEM_GENERIC_ERROR_SOURCE   **ErrSrc
  )
{
  UINTN i;

  *ErrSrc = NULL;
  for (i = 0; i < NumSysGenericErrorSources; i++) {
    if (Type == SysGenErrSources[i].Type) {
      *ErrSrc = &SysGenErrSources[i];
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
} 

/**
  Restart all WHEA Elog

  This function clears all error sources so a fresh log will be sent to the OS.
  

  @retval Status.

**/
EFI_STATUS
RestartAllWheaElog()
{
  GENERIC_ERROR_STATUS      *ErrStsBlk;
  GENERIC_HW_ERROR_SOURCE   *ErrSrc;
  UINTN                     i;
  UINTN                     MaxErrorStatusSize;

  if (!IsWheaEnabled) {
    return EFI_NOT_READY;
  }

  if (IsErrorSourceClear == FALSE) {
    RASDEBUG ((DEBUG_INFO, "WHEA: Erasing all Error Status blocks\n"));
    for (i = 0; i < NumSysGenericErrorSources; i++) {
      ErrSrc = (GENERIC_HW_ERROR_SOURCE *)SysGenErrSources[i].SourceData;
      ErrStsBlk = (GENERIC_ERROR_STATUS *)(*(UINT64 *)ErrSrc->ErrorStatusAddress.Address);
      ASSERT (ErrStsBlk != NULL);

      MaxErrorStatusSize = ErrSrc->MaxErrorDataSize;
      if (ErrSrc->ErrorStatusSize > MaxErrorStatusSize) {
        MaxErrorStatusSize = ErrSrc->ErrorStatusSize;
      }
      ZeroMem((VOID *)ErrStsBlk, MaxErrorStatusSize);
    }
  
  	IsErrorSourceClear = TRUE;
  }
  
  return EFI_SUCCESS;
}

/**
  Notify the OS via WHEA mechanism

  This function takes the appropriate action for a particular error based on severity.

  @param[in]  ErrorSeverity   The severity of the error to be handled
  @param[out] OsNotified      Returns whether the OS was notified

  @retval Status.

**/
EFI_STATUS
WheaNotifyOs (
  IN      UINT8     ErrorSeverity,
  OUT     BOOLEAN   *OsNotified
  )
{
  
   if (!IsWheaEnabled) return EFI_NOT_READY;
  
  // For WHEA Mechanism, we need to generate SCI, but we don't want to preclude other
  // notify mechanisms, so don't set OsNotified
  if (IsCeLogged) {
// APTIOV_SERVER_OVERRIDE_RC_START : SCI has been replaced with Polling
 //   RASDEBUG ((DEBUG_INFO, "WHEA: Sending OS notification via SCI\n"));
//    TriggerWheaSci(NULL, gSmst);
// APTIOV_SERVER_OVERRIDE_RC_END : SCI has been replaced with Polling
    IsCeLogged = FALSE;
  }
  
#ifdef RASCM_TODO 
  //  WHEA notification is something we need to revisit. NMI must happen only for platform errors.
  //Pcie must not signal NMI if OS is in control of AER. Unless the error is IIO specific error. 
#endif
  if (IsUeFatalLogged) {
    //
    // Not required.  The PCIe module will generate an NMI
    //
    //TriggerWheaNmi();
    IsUeFatalLogged = FALSE;
  }

  IsErrorSourceClear = FALSE;
  
  return  EFI_SUCCESS;
}

/**
  The WHEA Error Logger. 

  This function is responsible for logging errors via the WHEA interface.

  @param[in] ErrorRecordHeader    A pointer to the error record header

  @retval Status.

**/
EFI_STATUS
LogErrorToWhea (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  )
{
  EFI_STATUS          Status;
  UEFI_ERROR_SECT_DESC  *ErrorRecordDescription;
  VOID                *ErrorRecord;
  SYSTEM_GENERIC_ERROR_SOURCE   *SysErrSrc;
  GENERIC_HW_ERROR_SOURCE       *ErrSrc;
  GENERIC_ERROR_STATUS          *GenErrSts;
  GENERIC_ERROR_DATA_ENTRY      *GenErrData;
  VOID                          *ErrorSection;
  BOOLEAN                       ValidEntry;
  UINT32                        SectionSize;
  
  ValidEntry = FALSE;
  SectionSize =  0;
  
  if(ErrorRecordHeader == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = RestartAllWheaElog();
  
  if (Status == EFI_SUCCESS) { 
    ErrorRecordDescription = (UEFI_ERROR_SECT_DESC *)((UINT8 *)ErrorRecordHeader + sizeof(UEFI_ERROR_RECORD_HEADER));
    ErrorRecord = (UINT8 *)ErrorRecordDescription + sizeof(UEFI_ERROR_SECT_DESC);

    if (ErrorRecordDescription->Severity == EFI_ACPI_5_0_ERROR_SEVERITY_FATAL) { 
      Status = FindSysGenErrorSource(GENERIC_ERROR_FATAL, &SysErrSrc);
    } else {
      Status = FindSysGenErrorSource(GENERIC_ERROR_CORRECTED, &SysErrSrc);
    }

    if (EFI_ERROR(Status) || SysErrSrc == NULL) {
      return EFI_NOT_FOUND;
    }

    ErrSrc = (GENERIC_HW_ERROR_SOURCE *)SysErrSrc->SourceData;
    GenErrSts = (GENERIC_ERROR_STATUS *)(*(UINT64 *)ErrSrc->ErrorStatusAddress.Address);

    if (GenErrSts == NULL) {
      return EFI_NOT_FOUND;
    }

    if (GenErrSts->BlockStatus.NumErrorDataEntry >= MAX_WHEA_ERR_REC_COUNT) {
      RASDEBUG ((DEBUG_INFO, "WHEA erros logs count exceeded. So, returing w/o creating the record\n"));
      return EFI_UNSUPPORTED;
    }
    
    // Calculate offset for next error data log
    GenErrData = (GENERIC_ERROR_DATA_ENTRY *) ((UINT8 *)GenErrSts + sizeof(GENERIC_ERROR_STATUS) + GenErrSts->ErrDataSize);
    ErrorSection = (VOID *) ((UINT8 *)GenErrData + sizeof(GENERIC_ERROR_DATA_ENTRY));

    if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordProcGenErrGuid)) { 
      if ((mPlatformRasPolicyProtocol->SystemRasCapability->PlatformRasCapability.WheaProcLogCap == 1) 
          && (mPlatformRasPolicyProtocol->SystemRasSetup->Whea.WheaLogProcEn == 1)) {
        RASDEBUG ((DEBUG_INFO, "WHEA: Detected Processor Error\n"));

        ValidEntry = TRUE;
        SectionSize = sizeof (PROCESSOR_GENERIC_ERROR_SECTION) - sizeof (MCA_BANK_INFO);
      }
    } else if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordProcSpecErrGuid)) {
      // Currently not used
    } else if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordPlatformMemErrGuid)) {
      if ((mPlatformRasPolicyProtocol->SystemRasCapability->PlatformRasCapability.WheaMemLogCap == 1)
          && (mPlatformRasPolicyProtocol->SystemRasSetup->Whea.WheaLogMemoryEn == 1)) {
        RASDEBUG ((DEBUG_INFO, "WHEA: Detected Memory Error\n"));

        ValidEntry = TRUE;
        SectionSize = sizeof (MEMORY_ERROR_SECTION) - sizeof (MCA_BANK_INFO);
      }
    } else if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordVtdErrGuid)) {
      // Currently not used
    } else if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordPcieErrGuid)) {
      if ((mPlatformRasPolicyProtocol->SystemRasCapability->PlatformRasCapability.WheaPciLogCap == 1) 
          && (mPlatformRasPolicyProtocol->SystemRasSetup->Whea.WheaLogPciEn == 1)) {
        RASDEBUG ((DEBUG_INFO, "WHEA: Detected PCIe Error\n"));

        ValidEntry = TRUE;
        SectionSize = sizeof (PCIE_ERROR_SECTION);
      }
        }
        
    if (ValidEntry) {
      if ((!IsUeFatalLogged) 
          && ((!IsCeLogged)
            || (ErrorRecordDescription->Severity == EFI_ACPI_5_0_ERROR_SEVERITY_FATAL)
            || (GenErrSts->Severity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED))) {
        GenErrSts->Severity = ErrorRecordDescription->Severity;
        GenErrData->Severity = ErrorRecordDescription->Severity;
      }
      if (ErrorRecordDescription->Severity == EFI_ACPI_5_0_ERROR_SEVERITY_FATAL) { 
        RASDEBUG ((DEBUG_INFO, " --Logging Uncorrectable Error to WHEA\n"));
        IsUeFatalLogged = TRUE;
        if (GenErrSts->BlockStatus.UeValid == 0) {
          GenErrSts->BlockStatus.UeValid = 1;
        } else {
          GenErrSts->BlockStatus.MultipleUeValid = 1;
      }
        } else {
        RASDEBUG ((DEBUG_INFO, " --Logging Correctable Error to WHEA\n"));
        GenErrData->Severity = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
        GenErrSts->Severity = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
        IsCeLogged = TRUE;
        if (GenErrSts->BlockStatus.CeValid == 0) {
          GenErrSts->BlockStatus.CeValid = 1;
        } else {
          GenErrSts->BlockStatus.MultipleCeValid = 1;
        }
      }

      GenErrSts->ErrDataSize += sizeof(GENERIC_ERROR_DATA_ENTRY) + SectionSize;
      GenErrSts->BlockStatus.NumErrorDataEntry++;

      GenErrData->DataSize = SectionSize;
      GenErrData->Revision = ErrorRecordDescription->Revision;
      GenErrData->SectionFlags.Primary = 1;
      CopyMem (&GenErrData->SectionType, &ErrorRecordDescription->SectionType, sizeof (GUID));
      if (ErrorRecordDescription->ValidBits & BIT0) {
        GenErrData->SecValidMask.FruIdValid = 1;
        CopyMem (&GenErrData->FruId, &ErrorRecordDescription->FruId, sizeof (GUID));
      }
      if (ErrorRecordDescription->ValidBits & BIT1) {
        GenErrData->SecValidMask.FruStringValid = 1;
        CopyMem (&GenErrData->FruString[0], &ErrorRecordDescription->FruText[0], 20);
    }

      CopyMem (ErrorSection, ErrorRecord, SectionSize);
    }
  }

  return EFI_SUCCESS;
}
