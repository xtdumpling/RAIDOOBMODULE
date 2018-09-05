/**++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FnvErrorHandler.c

  This file will contain all definitions related to FNV Error Handler

----------------------------------------------------------------**/

#include <FnvErrorHandler.h>
#include <SysFunc.h>
#include <MemHost.h>

extern EFI_RAS_SYSTEM_TOPOLOGY              *mRasTopology;
extern SYSTEM_RAS_SETUP                     *mRasSetup;
extern EFI_CRYSTAL_RIDGE_PROTOCOL           *mCrystalRidge;
extern EFI_PLATFORM_ERROR_HANDLING_PROTOCOL *mPlatformErrorHandlingProtocol;

extern  EFI_GUID                             gErrRecordNotifyCmcGuid;
extern  EFI_GUID                             gErrRecordPlatformMemErrGuid;

extern  EFI_MEM_RAS_PROTOCOL                 *mMemRas;

BOOLEAN   mFnvErrorsEnabled = FALSE;      // Is FNV errors enabled to signal SMI?
UINT8     mSmiPriority = 0;               // Which Bucket low or high is set for SMI signaling?

/**
    Enables error logging for FNV errors

    @param ErrorLogSetupData  - Private data structure of error handling module

    @retval EFI_SUCCESS       - Error logging for FNV errors are enabled successfully
    @retval <other>           - Failure

**/
EFI_STATUS
EnableElogFnv (
    VOID
    ) {
  struct sysHost              *pSysHost = NULL;
  UINT8       Socket;
  UINT8       Ch;           // at socket level
  UINT8       Dimm;         // with in channel
  EFI_STATUS  Status;

  // Skip if FNV error logging is not enabled in BIOS setup
  RASDEBUG ((DEBUG_INFO, "EnableElog NVMCTLR : Function called\n"));
  if (!mRasSetup->Memory.FnvErrorEn) {
    RASDEBUG ((DEBUG_INFO, "EnableElog NVMCTLR : NVMCTLR error logging is not enabled in BIOS setup\n"));
    return EFI_SUCCESS;
  }

  // Skip if BIOS setup option is not enable for both low and high priority FNV errors
  if ((mRasSetup->Memory.FnvErrorLowPrioritySignal == NO_LOG) && (mRasSetup->Memory.FnvErrorHighPrioritySignal == NO_LOG))
    return EFI_SUCCESS;

  mFnvErrorsEnabled = TRUE;   // Flag to psyshost runtime FNV Error Handler whether to run or not
  pSysHost = (struct sysHost *)mMemRas->pSysHost;
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Ch = 0; Ch < MAX_CH; Ch++) {
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {

        Status = mCrystalRidge->NvmCtlrSetHostAlert(Socket, Ch, Dimm, mRasSetup->Memory.HostAlerts);
        if (EFI_ERROR(Status)) continue;

        Status = mCrystalRidge->NvmCtlrErrorLogProgramSignal(Socket, Ch, Dimm,
            LOW_PRIORITY, mRasSetup->Memory.FnvErrorLowPrioritySignal); // can we abstract the HIGH/LOW terminology?
        if (EFI_ERROR(Status)) continue; // stop any further communication with FNV Mailbox of this NGN DIMM

        mCrystalRidge->NvmCtlrErrorLogProgramSignal(Socket, Ch, Dimm,
            HIGH_PRIORITY, mRasSetup->Memory.FnvErrorHighPrioritySignal);

      } // for (Dimm)
    } // for (Ch)
  } // for (Socket)

  return EFI_SUCCESS;
}


/**
  Creates the memory error record based on the inputs.

  This function will only handle correctable errors.

  @param[in]  Type      - The error type
  @param[in]  SubType   - The error sub type
  @param[in]  Node      - The node the error was detected on
  @param[in]  Channel   - The channel the error was detected on
  @param[in]  Rank      - The rank the error was detected on
  @param[in]  Dimm      - The dimm the error was detected on
  @param[in]  McaCode   - The MCA code for the error

  @retval None
**/
VOID
CreateMemUncorrErrRecord (
  IN  UINT8                 Socket,
  IN  UINT8                 Ch,
  IN  UINT8                 Dimm,
  IN  UINT8                 ErrorType,
  OUT UEFI_MEM_ERROR_RECORD *MemRecord
  )
{
  RASDEBUG ((DEBUG_ERROR, "LogUncCorrectedMemError : Socket = %d, Channel = %d, Dimm = %d\n", Socket, Ch, Dimm));

  // All clear memory error record structure
  ZeroMem (MemRecord, sizeof (UEFI_MEM_ERROR_RECORD));

  // Build Error Record Header
  MemRecord->Header.SignatureStart = SIGNATURE_32('C','P','E','R');
  MemRecord->Header.Revision = 0x0204;  // UEFI Spec version 2.4
  MemRecord->Header.SignatureEnd = 0xffffffff;
  MemRecord->Header.SectionCount = 1;  // Only supporting one section per error record
  // when mirroring is enabled, log this as CORRECTED error instead
  MemRecord->Header.Severity = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;
  MemRecord->Header.ValidBits = 0;
  MemRecord->Header.RecordLength = sizeof (UEFI_MEM_ERROR_RECORD);
  CopyMem (&MemRecord->Header.NotificationType, &gErrRecordNotifyCmcGuid, sizeof (EFI_GUID));
  MemRecord->Header.Flags = HW_ERROR_FLAGS_RECOVERED;

  // Fill error descriptor
  MemRecord->Descriptor.Offset = sizeof (UEFI_ERROR_RECORD_HEADER) + sizeof (UEFI_ERROR_SECT_DESC);
  MemRecord->Descriptor.Length = sizeof (MEMORY_ERROR_SECTION);
  MemRecord->Descriptor.Revision = 0x0204;  // UEFI Spec version 2.4
  MemRecord->Descriptor.ValidBits = 0;
  MemRecord->Descriptor.Flags = 1;   // Primary
  CopyMem (&MemRecord->Descriptor.SectionType, &gErrRecordPlatformMemErrGuid, sizeof (EFI_GUID));
  MemRecord->Descriptor.Severity = EFI_ACPI_5_0_ERROR_SEVERITY_FATAL;

  // Fill location details
  MemRecord->Section.ValidBits = 0;
  MemRecord->Section.Node = Socket;
  MemRecord->Section.Card = Ch; // ?????
  MemRecord->Section.Module = Dimm;
  //MemRecord->Section.CardHandle = mRasTopology->SystemInfo.SmbiosType16Handle; // Type 16 handle - ?????
  MemRecord->Section.ModuleHandle = mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[Ch].DimmInfo[Dimm].SmbiosType17Handle; // Type 17 handle
  MemRecord->Section.ErrorType = PLATFORM_MEM_ERROR_MLTIBIT_ECC;

  MemRecord->Section.ValidBits = PLATFORM_MEM_NODE_VALID | PLATFORM_MEM_MODULE_VALID | PLATFORM_Module_HANDLE_VALID; // | PLATFORM_CARD_HANDLE_VALID

  return;
}

// remove this when ERROR_LOG_ENTRY in CrystalRidge.h is fixed (
typedef struct _LOG_ENTRY_STRUCT {
  UINT64_STRUCT     timeStamp;       // Last system time set by BIOS using the set admin feature
  UINT32            timeOffset;      // Power on time in seconds since last system time update
  UINT64_STRUCT     dpaError;        // Specifies the DPA Address of the error
  UINT64_STRUCT     pdaError;        // Specifies the PDA address of the error
  UINT8             range;           // Specifies the length in address space of this error
  UINT8             errorID;         // Indicates what kind of error was logged
  UINT8             transactionType; // Indicates what transaction caused the error
} LOG_ENTRY_STRUCT;

typedef struct _ERROR_PAGE_LOG1 {
  UINT8                  totalEntries;    //Total Number of valid entries in the log new or old
  UINT8                  newEntries;      //Specifies the total number of new entries in the log
  UINT8                  overrunFlag;     // flag to indicate that the log fifo had an overrun condition
  UINT8                  returnCount;     // number of log entries returned
  struct errorLogEntry   errorEntry[MAX_LOG_ENTRY]; // Log Entry
} ERROR_PAGE_LOG1;

VOID
HandleNVDIMMError (
  UINT8                  Socket,
  UINT8                  Ch,         // at socket level
  UINT8                  Dimm       // with in channel
  )
{
  EFI_STATUS             Status;
  UINT8                  i;
  BOOLEAN                Priority;
  UINT32                 Log;
  UINT8                  CountToCheck = 4;
  UINT8                  ErrorType;
  MEDIA_ERR_LOG          *pMediaLog;
  MEDIA_ERR_LOG_DATA     MediaLogData;
  UEFI_MEM_ERROR_RECORD  MemRecord;

  for (Priority = 0; Priority <= HIGH_LEVEL_LOG; Priority++) {
    Status = mCrystalRidge->NvmCtlrErrorLogDetectError(Socket, Ch, Dimm, Priority, DO_NOT_CLEAR_ON_READ, &Log);
    if (EFI_ERROR(Status))
      continue;

    if (!(Log & FNVERR_MASK))
	  continue;

    if ((Log & FNVERR_FW_HANG) || (Log & FNVERR_FATAL)) {
	  CreateMemUncorrErrRecord (Socket, Ch, Dimm, PLATFORM_MEM_ERROR_UNKNOWN, &MemRecord);
	  mPlatformErrorHandlingProtocol->LogReportedError (&MemRecord.Header);
	  continue;
    }

    //We only logs
    if (!(Log & FNVERR_NGNVM_ERROR_LOG))
	  continue;

    Status = mCrystalRidge->NvmCtlrGetLatestErrorLog(Socket, Ch, Dimm, HIGH_LEVEL_LOG, MEDIA_LOG_REQ, CountToCheck, &MediaLogData);
    if (EFI_ERROR(Status))
      continue;

    for (i = 0; i < MediaLogData.ReturnCount; i++) {
	  pMediaLog = &MediaLogData.MediaLogs[i];
	  switch (pMediaLog->TransactionType) {
        case FNV_TT_2LM_READ:
        case FNV_TT_2LM_WRITE:
        case FNV_TT_PM_READ:
        case FNV_TT_PM_WRITE:
          ErrorType = PLATFORM_MEM_ERROR_MLTIBIT_ECC;
	      break;
	    case FNV_TT_PATROL_SCRUB:
	      ErrorType = PLATFORM_MEM_ERROR_SCRUB_UNCOR;
	      break;
	    default:
	      continue; //TODO: Confirm that we don't log any other type of transaction error
	  }
	  CreateMemUncorrErrRecord (Socket, Ch, Dimm, ErrorType, &MemRecord);
	  mPlatformErrorHandlingProtocol->LogReportedError (&MemRecord.Header);
    }
  }
}


/**

    FNV Error Handler

    @retval EFI_SUCCESS - Error Handler performed successfully

**/
EFI_STATUS
FnvErrorHandler (
  VOID
  ) {
  UINT8                  Socket;
  UINT8                  Ch;         // at socket level
  UINT8                  Dimm;       // with in channel

  if (!mFnvErrorsEnabled) {
    RASDEBUG ((DEBUG_INFO, "NVMCTLR ErrorHandler : NVMCTLR errors Not enabled\n"));
    return EFI_SUCCESS;
  }

  for (Socket = 0 ; Socket < MAX_SOCKET; Socket++) {
    for (Ch = 0; Ch < MAX_CH; Ch++) {
      for (Dimm = 0 ; Dimm < MAX_DIMM; Dimm++) {
    	if (mCrystalRidge->IsNvmDimm(Socket, Ch, Dimm)) {
          HandleNVDIMMError(Socket, Ch, Dimm);
    	}
      }
    }
  }

  return EFI_SUCCESS;
}

