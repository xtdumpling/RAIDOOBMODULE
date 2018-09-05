/** @file
  Contains definitions relevant to the Processor Error Handler.

  Copyright (c) 2009-2015 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
	
#ifndef _CORE_ERROR_HANDLER_H
#define _CORE_ERROR_HANDLER_H

#include "ErrorRecords.h"
#include "UncoreCommonIncludes.h"
#include <Library/ProcMemErrReporting.h>
#include <Library/emcaplatformhookslib.h>
#include <Library/emcalib.h>
#include <Guid/PprVariable.h>
#include <Library/SmmCpuPlatformHookLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Library/HobLib.h>   //Added support for Last Boot Error Logging
//Added support for SEL/Gpnv Error logging using Runtime ErrorLog module
#include "IohErrorHandler.h"
#include "RtErrorLog.h"
#include "RtErrorLogBoard.h"
#include <ProcMemErrReporting.h>
#include <Library\AmiErrReportingLib\AmiErrReportingLib.h>   //OEM Hooks Support
// APTIOV_SERVER_OVERRIDE_RC_END

// Definitions

#define MSR_MCA_ERROR_CONTROL   0x52
#define MSR_MCG_CONTAIN         0x178
#define MSR_IA32_MCG_CAP        0x179
#define MSR_ERROR_CONTROL       0x17F

//
//
//===============================================================
//

//
// Function prototype declarations
//
typedef
VOID
(EFIAPI *EFI_FRU_ISOLATION_CALLBACK)(
  IN VOID        *pContext,
  IN EFI_STATUS  Status,
  IN UINT8       RespCode,
  IN EFI_PHYSICAL_ADDRESS   Address,
  IN UINT8       AccessLength,
  IN BOOLEAN     PreviousBoot
  );

////////////////////////////////// Error Handling Routines //////////////////////////////////
  
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
  );

/**
  Get error severity from error signature. 

  @param[in] McErrSig   MC Error signature

  @retval Sev   Severity

**/
UINT32
MapErrorSeverity (
  IN      EMCA_MC_SIGNATURE_ERR_RECORD  *McErrSig
  );

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
      OUT   UEFI_PROCESSOR_ERROR_RECORD   *ErrSts
  );

/**
  Gets the physical location of the processor with the given Apic ID.
  
  @param[in]  ApicId          - The Apic ID
  
  @retval BOOLEAN True if executing on local core.  Otherwise, False.

**/
BOOLEAN
ExecutingOnLocalCore (
  IN        UINT32   ApicId
  );



/**
  Check for fatal or viral conditions. 

  @retval EFI_SUCCESS if no viral or fatal conditions

**/
EFI_STATUS
ProcessFatalAndViralError (
  VOID
  );

/**
  Finds the first MC Bank that reports an error.
  
  @param[in] McBankBitField   Bit map of banks reporting errors
  @param[out] Idx             Index of first bit set
  
  @retval Status

**/
EFI_STATUS
FindFirstSet (
  IN      UINT64    McBankBitField,
      OUT UINT8     *Idx
  );

/**
  The Processor Error Handler. 

  This function is responsible for detecting Processor Errors.
  
  @param[out] Severity   Highest severity error detected

  @retval Status.

**/
EFI_STATUS
ProcessorErrorHandler (
      OUT   UINT32  *Severity
  );

////////////////////////////////// Initialization routines //////////////////////////////////


/**
  Enable Viral in each UBOX.

  @param[in] Skt      - The socket to enable

  @retval None

**/
VOID
EnableUBoxViralPerSkt(
  IN        UINT8   Skt
  );

/**
  Function to enbale Ubox errors.

  @retval None

**/
VOID
EnableUboxError(
  VOID
  );

/**
  Enables logging of QPI errors.  

	@param	void

  @retval EFI_SUCCESS if the call succeeded.

**/
EFI_STATUS
EnableElogKTI (
  VOID  
  );

/**
  Enables CSMI.
  
  @retval EFI_STATUS.

**/
EFI_STATUS
EnableCsmi (
  VOID
  );
  
VOID
InitializePPRData(VOID);
// External definitions
extern  EFI_GUID  gErrRecordNotifyMceGuid;
extern  EFI_GUID  gErrRecordProcGenErrGuid;

#endif
