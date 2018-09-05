/** @file
  Implementation of the RAS Module Main Error Handler.

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
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MemRasProtocol.h>
#include "CommonErrorHandlerDefs.h"
#include "MainErrorHandler.h"
#include "ErrorRecords.h"

// Protocol Definitions

EFI_ERROR_HANDLING_PROTOCOL mErrorHandlingProtocol = {
  RegisterSiliconErrorHandler,
  };

// Protocols
EFI_RAS_ERROR_RECORDS                 *mRasErrorRecords;
EFI_SMM_BASE2_PROTOCOL                *mSmmBase2;
EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandler;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;

// Module data
HANDLER_LIST_ENTRY                    *ListHead = NULL;
HANDLER_LIST_ENTRY                    *ListTail = NULL;

EFI_RAS_SYSTEM_TOPOLOGY               *mRasTopology;

// Function Definitions

/**
  Register a silicon error Handler. 

  This function allows a silicon module to add its handler to a linked list
  of handlers that will be executed on each SMI.

  @param[in]  Reporter         A pointer to the silicon module error handler
  @param[in]  Priority            The priority of the error handler

  @retval Status.

**/
EFI_STATUS
RegisterSiliconErrorHandler (
  IN      REPORTER_PROTOCOL           *Reporter,
  IN      UINT8                       Priority
  )
{
  EFI_STATUS                          Status;
  HANDLER_LIST_ENTRY                  *NewEntry;
  HANDLER_LIST_ENTRY                  *PrevEntry;
  HANDLER_LIST_ENTRY                  *CurrEntry;
  
  if (Reporter == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (HANDLER_LIST_ENTRY),
                    &NewEntry
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NewEntry->Reporter = Reporter;
  NewEntry->NextHandler = NULL;
  NewEntry->Priority = Priority;
  CurrEntry = NULL;
  PrevEntry = NULL;
  
  if (ListHead == NULL) {
    ListHead = NewEntry;
    ListTail = NewEntry;
  } else {
    switch (Priority) {
      case NoPriority:
      case LowPriority:
        // No Priority/Low Priority gets placed at the end of the list.
        ListTail->NextHandler = NewEntry;
        ListTail = ListTail->NextHandler;
        break;
      case MediumPriority:
      case HighPriority:
        for (CurrEntry = ListHead; CurrEntry != NULL; ) {
          if (Priority > CurrEntry->Priority) {
            if (CurrEntry == ListHead) {
              NewEntry->NextHandler = CurrEntry;
              ListHead = NewEntry;
            } else {
              PrevEntry->NextHandler = NewEntry;
              NewEntry->NextHandler = CurrEntry;
            }
            break;
          } else {
            if (CurrEntry == ListTail) {
              CurrEntry->NextHandler = NewEntry;
              ListTail = NewEntry;
              break;
            } else {
              PrevEntry = CurrEntry;
              CurrEntry = CurrEntry->NextHandler;
            }
          }
        }
        break;
      default:
        break;
    }
  }

  Status = EFI_SUCCESS;
  return Status;
}

/**
  The RAS Main Error Handler. 

  This function is responsible for calling Silicon error handlers to determine
  the source of errors.  Additionally, it will call hooks to allow OEMs to
  customize their error handling solutions.

  @param[in] DispatchHandle       Dispatch handle
  @param[in] Context              Context information
  @param[in] CommunicationBuffer  Buffer used for communication between caller/callback function
  @param[in] SourceSize           Size of communication buffer

  @retval Status.

**/
EFI_STATUS
MainErrorHandler (
  IN        EFI_HANDLE      DispatchHandle,
  IN        CONST VOID      *Context OPTIONAL,
  IN  OUT   VOID            *CommunicationBuffer,
  IN  OUT   UINTN           *SourceSize
  )
{
  EFI_STATUS                Status;
  HANDLER_LIST_ENTRY        *CurrentHandler;
  UINT32                    CurrentSeverity;
  UINT32                    Severity;  
  BOOLEAN                   ErrorOccured = FALSE;

  Status = EFI_SUCCESS;

  //while (!Status); // For ITP debug only

  Severity = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;
  CurrentSeverity = EFI_ACPI_5_0_ERROR_SEVERITY_NONE;

// APTIOV_SERVER_OVERRIDE_RC_START
  Status = gSmst->SmmLocateProtocol (&gEfiPlatformRasPolicyProtocolGuid, NULL, &mPlatformRasPolicyProtocol);
  ASSERT_EFI_ERROR (Status);
  mRasTopology = mPlatformRasPolicyProtocol->EfiRasSystemTopology;
  mRasTopology->SystemInfo.SkipFillOtherMcBankInfoFlag = FALSE;
// APTIOV_SERVER_OVERRIDE_RC_END

  //
  // Check for Status Log, Detect & Handle them
  //
  CurrentHandler = ListHead;
  while (CurrentHandler != NULL) {
    //Status = CurrentHandler->Handler (&CurrentSeverity);
    if (CurrentHandler->Reporter->CheckStatus()) {
      Status = CurrentHandler->Reporter->DetectAndHandle(&CurrentSeverity, AutoOption);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      switch (CurrentSeverity) {
        case EFI_ACPI_5_0_ERROR_SEVERITY_FATAL:
          Severity = CurrentSeverity;
          break;
        case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED:
          if (Severity == EFI_ACPI_5_0_ERROR_SEVERITY_NONE) {
            Severity = CurrentSeverity;
          }
          break;
        case EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE:
          if ((Severity == EFI_ACPI_5_0_ERROR_SEVERITY_NONE) || (Severity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED)) {
            Severity = CurrentSeverity;
          }
          break;
        case EFI_ACPI_5_0_ERROR_SEVERITY_NONE:
          break;
        default:
          RASDEBUG ((DEBUG_ERROR, "Invalid error severity: %x \n", CurrentSeverity));
          break;
      }
    }

    // Check next handler
    CurrentHandler = CurrentHandler->NextHandler;
  }

  //
  // Clear all the Status Logs
  //
  CurrentHandler = ListHead;
  while (CurrentHandler != NULL) {
    //Status = CurrentHandler->Handler (&CurrentSeverity);
    if (CurrentHandler->Reporter->CheckStatus()) {
      Status = CurrentHandler->Reporter->ClearStatus();
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    // Check next handler
    CurrentHandler = CurrentHandler->NextHandler;
  }

  if (Severity != EFI_ACPI_5_0_ERROR_SEVERITY_NONE) {
    RASDEBUG ((DEBUG_INFO, "Highest Error Severity: %x \n", Severity)); 
    mPlatformErrorHandler->NotifyOs (Severity);
    ErrorOccured = TRUE;
  }
  
  if(ErrorOccured == TRUE) {
    RASDEBUG ((DEBUG_INFO, "MainErrorHandler Exit: \n" ));
  }

  return Status;
}

/**
  Entry point for the RAS Module Main Handler. 

  This function function performs generic error handling
  initialization.  It is responsible for installing the Error
  Handling Protocol.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
EFIAPI
InitializeMainErrHandler (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS      Status;
  EFI_HANDLE      Handle;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformErrorHandlingProtocolGuid,
                    NULL,
                    &mPlatformErrorHandler
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  // Produce the error handling protocol for use by silicon and platform
  // modules.
  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiErrorHandlingProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mErrorHandlingProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Register the main error handler to be checked on every SMI.
  Handle = NULL;
  Status = gSmst->SmiHandlerRegister (MainErrorHandler, NULL, &Handle);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
