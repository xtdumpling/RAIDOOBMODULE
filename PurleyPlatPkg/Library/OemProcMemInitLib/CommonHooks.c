//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03
//      Bug Fixed:  Fix the Training Margin Tool Small warning cannot be logged
//      Reason:     All Warning which will disable channels/dimms should be logged
//                  remove the judgement here and modify the Error.c to avoid twice log
//      Auditor:    Max Mu
//      Date:       Jun/30/2017
//
//  Rev. 1.02
//      Bug Fixed:  Fix that system log 2 times sel when no memory found.
//      Reason:     
//      Auditor:    Isaac Hsu
//      Date:       Mar/07/2017
//
//  Rev. 1.01
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
//  Rev. 1.00
//      Bug Fixed:  Add Early video support.
//      Reason:     Purley platform need add it after KTI initialize
//      Auditor:    Salmon Chen
//      Date:       Mar/07/2016
//
//*****************************************************************************
/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"
// APTIOV_SERVER_OVERRIDE_RC_START
// Report MRC progress/error codes to status code listener
#include <AmiStatusCodes.h>
#include <Library/ReportStatusCodeLib.h>
#include <AmiMrcHooksWrapperLib\AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END
//SMCPKG_SUPPORT
#include <token.h>
#include <PiPei.h>
//SMCPKG_SUPPORT
//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

//
// GUID for this revision of EWL
//
EFI_GUID mEwlGuid = EWL_REVISION1_GUID;

/**

  PlatformCheckpoint - OEM hook to provide major checkpoint output
  to the user interface. By default a byte is written to port 80h.

  @param host  - Pointer to the system host (root) structure

  @retval N/A

**/
#if SMCPKG_SUPPORT && EarlyVideo_SUPPORT
#define MRC_PROGRESS_CODE(PeiServices, Code)\
      (*PeiServices)->ReportStatusCode(PeiServices, MRC_SC_TYPE, Code, 0, NULL, NULL)
#endif
void
PlatformCheckpoint (
  PSYSHOST host
  )
{
  //
  // Output major checkpoint
  //
#if SMCPKG_SUPPORT
#if EarlyVideo_SUPPORT
  EFI_PEI_SERVICES **PeiServices;
  
  PeiServices      = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  if (host->var.common.socketId == 0) { //SMC_PORTING, Only BSP output CKP
     MRC_PROGRESS_CODE(PeiServices, (UINT8) (host->var.common.checkpoint >> 24));
  }else 
  {
  OutPort8 (host, host->setup.common.debugPort, (UINT8) (host->var.common.checkpoint >> 24));
  }
#endif
#else
  OutPort8 (host, host->setup.common.debugPort, (UINT8) (host->var.common.checkpoint >> 24));
#endif
}


/**

  PlatformLogWarning - OEM hook to provide common warning output to the
  user interface

  @param host      - pointer to sysHost structure
  @param majorCode - major error/warning code
  @param minorCode - minor error/warning code
  @param logData   - data log

**/
void
PlatformLogWarning (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT32   logData
  )
{
#if SMCPKG_SUPPORT
//if (majorCode == WARN_FPT_CORRECTABLE_ERROR || majorCode == WARN_FPT_UNCORRECTABLE_ERROR)
#endif
  // APTIOV_SERVER_OVERRIDE_RC_START
  AmiPlatformLogWarningWrapper(host, majorCode,minorCode,logData);
  // APTIOV_SERVER_OVERRIDE_RC_END
  return ;
}

/**

  PlatformFatalError - OEM hook to provide fatal error output to the
  user interface and to override the halt on error policy.

  @param host - pointer to sysHost structure
  @param majorCode - major error/warning code
  @param minorCode - minor error/warning code
  @param haltOnError - pointer to variable that selects policy

**/
void
PlatformFatalError (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    *haltOnError
  )
{
	// APTIOV_SERVER_OVERRIDE_RC_START
	  // Report MRC progress/error codes to status code listener
	  // Report Memory not detected Error code.
	  //
  if (( majorCode == ERR_NO_MEMORY ) || ( majorCode == WARN_NO_DDR_MEMORY )) {
		REPORT_STATUS_CODE (
		  EFI_ERROR_CODE|EFI_ERROR_MAJOR,
		  PEI_MEMORY_NOT_DETECTED
		  );
	  }
	  //
	  // Report Memory not useful Error code.
	  //
	  if ( majorCode == ERR_MEM_TEST ) {
		REPORT_STATUS_CODE (
		  EFI_ERROR_CODE|EFI_ERROR_MAJOR,
		  PEI_MEMORY_NONE_USEFUL
		  );
	  }
    AmiPlatformFatalErrorWrapper( host, majorCode, minorCode, haltOnError );
    // APTIOV_SERVER_OVERRIDE_RC_END
 //SKX_TODO: remove for 64 bit build, this is defined in a "sources.ia32", so does not resolve in 64 bit mode
#ifdef IA32 
 OemPlatformFatalError(host, majorCode, minorCode, haltOnError);
#endif
  return ;
}

#if ENHANCED_WARNING_LOG
/**

Platform hook called to see if the warning should be promoted to an exception

@param  host        - Pointer to the system host (root) structure
@param  EwlEntry    - Pointer to new EWL entry buffer
@retval None

**/
VOID
PlatformPromoteWarningsToErrors(
  PSYSHOST          host,         // Pointer to the system host (root) structure
  EWL_ENTRY_HEADER  *EwlEntry     // Pointer to the EWL entry header
)
{
  UINT8               i;
  UINT8               promoteWarning;
  EWL_ENTRY_CONTEXT   *ewlContext;

  //Initialize promoteWarning to 0
  promoteWarning = 0;

  //
  // Only follow this code path if the EwlEntry's type has an EWL_ENTRY_CONTEXT member
  // (Cannot be guaranteed on types greater than EwlTypeMax)
  //
  if ((EwlEntry->Type < EwlTypeMax) &&
     (EwlEntry->Type != EwlType21) &&
     (EwlEntry->Type != EwlType6)  && 
     (EwlEntry->Type != EwlType8) &&
     (EwlEntry->Type != EwlType9)) {
    //
    // EWL Context currently follows the EWL Header in all Intel defined entries
    // except types: EwlType21 (Degraded Topology)
    // Legacy KTI warnings did not follow this error promotion path, so EwlType6 is also an exception.
    // EwlType8 and 9 should not be promoted to Fatal errors.

    ewlContext = (EWL_ENTRY_CONTEXT*)(EwlEntry + 1); // Context follows Header, so + 1 adds size of header

    //
    // Caller assigned fatal error severity
    //
    if (EwlEntry->Severity == EwlSeverityFatal) {
      rcPrintf((host, "This is a Fatal Error!\n"));
      FatalError(host, ewlContext->MajorCheckpoint, ewlContext->MinorCheckpoint);
    }

    //
    // Promote warning control is based on setup option
    //
    if (host->setup.common.options & PROMOTE_WARN_EN) {
      promoteWarning = 1;
      //
      // Do NOT promote items added to the promoteWarnList (exceptions)
      //
      for (i = 0; i < host->var.common.promoteWarnLimit; i++) {
        if ((host->var.common.promoteWarnList[i] == (UINT16)((ewlContext->MajorWarningCode << 8) | ewlContext->MinorWarningCode)) ||
          (host->var.common.promoteWarnList[i] == (UINT16)((ewlContext->MajorWarningCode << 8) | WARN_MINOR_WILDCARD))
          ) {
          promoteWarning = 0;
          break;
        }
      }

      //
      // Upgrade FPT warnings according to MRC_WARN setup option
      //
      if ((ewlContext->MajorWarningCode >= WARN_FPT_CORRECTABLE_ERROR) && (ewlContext->MajorWarningCode <= WARN_FPT_UNCORRECTABLE_ERROR)) {
        if (host->setup.common.options & PROMOTE_MRC_WARN_EN) {
          promoteWarning = 1;
        }
        else {
          promoteWarning = 0;
        }
      }

      //
      // Promote to fatal
      //
      if (promoteWarning) {
        rcPrintf((host, "Warning upgraded to Fatal Error!\n"));
        FatalError(host, ewlContext->MajorWarningCode, ewlContext->MinorWarningCode);
      }
    }
  }
  else {
    //
    // This path means we don't know the format of the entry and the context
    // So we use the current global context as the approximation
    //

    //
    // Caller assigned fatal error severity
    //
    if (EwlEntry->Severity == EwlSeverityFatal) {
      rcPrintf((host, "This is a Fatal Error!\n"));
      FatalError(host, ((host->var.common.checkpoint) >> 24) & 0xFF, ((host->var.common.checkpoint) >> 16) & 0xFF);
    }
  }
}
#endif //ENHANCED_WARNING_LOG
/**

Platform function for initializing the enhanced warning log.
It is called from core prior any calls to PlatformEwlLogEntry.
This function will initialize the enhanced warning log public
and private data structures.

@param host        - Pointer to the system host (root) structure
@retval ¨C SUCCESS

**/
UINT32
PlatformEwlInit(
PSYSHOST host
)
{
#if ENHANCED_WARNING_LOG
  EWL_PRIVATE_DATA      *ewlPrivateData = &host->var.common.ewlPrivateData;
  EWL_HEADER            *Header = &ewlPrivateData->status.Header;

  //
  // Initialize the EWL Header
  //
  MemCopy((UINT8*)&Header->EwlGuid, (UINT8*)&mEwlGuid, sizeof(EFI_GUID));
  Header->Size = sizeof(EWL_PUBLIC_DATA);
  Header->FreeOffset = 0;
  Header->Reserved = 0;  // Required by spec to be initialized to 0

  //
  // It is not strictly necessary to initialize the Buffer to a known state
  //

  //
  // Calculate Crc 
  //
  CalculateCrc32((UINT8*)&host->var.common.ewlPrivateData.status, Header->Size, &Header->Crc);

  //
  // Initialize some implementation private data helpers
  //
  ewlPrivateData->bufSizeOverflow = 0;
  ewlPrivateData->numEntries = 0;

  //
  // Initialize the warning promotion to error list
  //
  PlatformMemInitWarning(host);
  return SUCCESS;
#else
  return FAILURE;
#endif //ENHANCED_WARNING_LOG
}

/**

Platform hook that is called to report a new entry is available for reporting to
the Enhanced Warning Log.

This function will add it to the log or return an error if there is insufficient
space remaining for the entry.

@param  host        - Pointer to the system host (root) structure
@param  EwlEntry    - Pointer to new EWL entry buffer
@retval ¨C SUCCESS if entry is added; FAILURE if entry is not added

**/
MRC_STATUS
PlatformEwlLogEntry(
PSYSHOST          host,         // Pointer to the system host (root) structure
EWL_ENTRY_HEADER  *EwlEntry     // Pointer to the EWL entry header
)
{
#if ENHANCED_WARNING_LOG
  EWL_PRIVATE_DATA      *ewlPrivateData = &host->var.common.ewlPrivateData;
  EWL_PUBLIC_DATA       *log = &host->var.common.ewlPrivateData.status;
  UINT32                offset = log->Header.FreeOffset;
  UINT32                entrySize = EwlEntry->Size;

  //
  // Print warning message if the type and size don't match expectations
  //
  if (((EwlEntry->Type == EwlType1) && (entrySize != sizeof(EWL_ENTRY_TYPE1))) ||
    ((EwlEntry->Type == EwlType2) && (entrySize != sizeof(EWL_ENTRY_TYPE2)))   ||
    ((EwlEntry->Type == EwlType3) && (entrySize != sizeof(EWL_ENTRY_TYPE3)))   ||
    ((EwlEntry->Type == EwlType4) && (entrySize != sizeof(EWL_ENTRY_TYPE4)))   ||
    ((EwlEntry->Type == EwlType5) && (entrySize != sizeof(EWL_ENTRY_TYPE5)))   ||
    ((EwlEntry->Type == EwlType6) && (entrySize != sizeof(EWL_ENTRY_TYPE6)))   ||
    ((EwlEntry->Type == EwlType7) && (entrySize != sizeof(EWL_ENTRY_TYPE7)))   ||
    ((EwlEntry->Type == EwlType8) && (entrySize != sizeof(EWL_ENTRY_TYPE8)))   ||
    ((EwlEntry->Type == EwlType9) && (entrySize != sizeof(EWL_ENTRY_TYPE9)))   ||
    ((EwlEntry->Type == EwlType10) && (entrySize != sizeof(EWL_ENTRY_TYPE10))) ||
    ((EwlEntry->Type == EwlType21) && (entrySize != sizeof(EWL_ENTRY_TYPE21)))
    ) {
    rcPrintf((host, "\nEWL warning size mismatch"));
    return FAILURE;
  }

  //
  // Verify we have sufficient space for this entry.
  //
  if ((offset + entrySize) >= sizeof(log->Buffer)) {
    rcPrintf((host, "\nThis warning was omitted from the log, increase size of EWL buffer!"));
    ewlPrivateData->bufSizeOverflow += entrySize;
    return FAILURE;
  }

  //
  // Add the entry
  //
  MemCopy(&log->Buffer[offset], (UINT8*)EwlEntry, entrySize); //dest, source, size

  //
  // Update the FreeOffset and internal tracking
  //
  log->Header.FreeOffset += entrySize;
  ewlPrivateData->numEntries += 1;

  //
  // Recompute crc
  //
  CalculateCrc32((UINT8*)log, sizeof(EWL_PUBLIC_DATA), &log->Header.Crc);

  //
  // Promote to fatal and handle fatal errors
  //
  PlatformPromoteWarningsToErrors(host, EwlEntry);

  return MRC_STATUS_SUCCESS;
#else
  return FAILURE;
#endif //ENHANCED_WARNING_LOG
}

/**

  OEM hook to read data from a device on the SMBbus.

  @param host - pointer to sysHost structure
  @param socket - socket number
  @param dev - SMB device structure
  @param byteOffset - offset of data to read
  data - Pointer to data to be returned

  @retval data - Pointer to data to be returned
  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadSmb

**/
UINT8
PlatformReadSmb (
  PSYSHOST         host,
  UINT8            socket,
  struct smbDevice dev,
  UINT8            byteOffset,
  UINT8            *data
  )
{
  //
  // Hook not implemented
  //
  return 0;
}

/**

  OEM hook to write data to a device on the SMBbus.

  @param host - pointer to sysHost structure
  @param dev - SMB device structure
  @param byteOffset - offset of data to write
  @param data - Pointer to data to write

**/
UINT8
PlatformWriteSmb (
  PSYSHOST         host,
  struct smbDevice dev,
  UINT8            byteOffset,
  UINT8            *data
  )
{
  //
  // Hook not implemented
  //
  return 0;
}

/**

  OEM hook for one-time initialization of GPIO parameters such
  as decode, direction, and polarity init.

  @param host - pointer to sysHost structure

**/
void
PlatformInitGpio (
  PSYSHOST host
  )
{
  return ;
}

/**

  OEM hook to read a byte from a platform GPIO device that is
  not recognized by ReadGpio().

  @param host      - pointer to sysHost structure
  @param address   - GPIO device address
  @param data      - Pointer to data

  @retval 0         - Hook not implemented (default)
  @retval 1         - Hook implemented and read was successful
  @retval 2         - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadGpio

**/
UINT32
PlatformReadGpio (
  PSYSHOST host,
  UINT16   address,
  UINT32   *data
  )
{
  //
  // Hook not implemented
  //
  return 0;
}

/**

  OEM hook to write a byte to a platform GPIO device that is
  not recognized by WriteGpio().

  @param host      - pointer to sysHost structure
  @param address   - GPIO device address
  @param data      - Data to write

  @retval 0         - Hook not implemented (default)
  @retval 1         - Hook implemented and write was successful
  @retval 2         - Hook implemented and write failed / slave device not present
  @retval Return 1 or 2 if this routine should replace WriteGpio

**/
UINT32
PlatformWriteGpio (
  PSYSHOST host,
  UINT16   address,
  UINT32   data
  )
{
  //
  // Hook not implemented
  //
  return 0;
}
