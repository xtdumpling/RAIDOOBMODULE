/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
  Copyright (c) 2013-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  ChkRetStat.c

@brief
  This file contains functions for checking return status values.
**/
#ifdef SSA_FLAG

#define BSSA_LOGGING_LEVEL BSSA_LOGGING_INTERNAL_DEBUG_LEVEL

#include "ssabios.h"
#include "bssalogging.h"
#include "ChkRetStat.h"

/**
@brief
  This function processes the given error information.  The error status and
  string are logged and placed in the given metadata structure.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       RetStat            Return status value to use.
  @param[in]       FileName           File name.
  @param[in]       LineNumber         Line number.
  @param[out]      pTestStat          Pointer to test status in results metadata structure (or NULL).
**/
VOID ProcError(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN SSA_STATUS RetStat,
  IN char* FileName,
  IN UINT32 LineNumber,
  OUT TEST_STATUS *pTestStat)
{
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "ERROR: File=%s Line=%u RetStat=%d\n", FileName, LineNumber, RetStat);

  if (pTestStat != NULL) {
    INT32 StringLen, Offset;
    UINT32 Length = sizeof(pTestStat->FileName);

    // get the string length
    for (StringLen = 0; FileName[StringLen] != 0; StringLen++) {
    }

    // get the offset into the string such that the least significant portion
    // will be placed in the character array
    Offset = StringLen - sizeof(pTestStat->FileName);

    // IF the offset is negative (could happen is string is shorter than
    // character array)
    if (Offset < 0) {
      // copy the full string and no more
      Length += Offset;
      Offset = 0;
    }

    pTestStat->StatusCode = (UINT8)RetStat;
    pTestStat->LineNumber = (UINT16)LineNumber;
    bssa_memcpy(pTestStat->FileName, &FileName[Offset], Length);
  }
} // end function ProcError

/**
@brief
  This function checks the given return status value for error values.  If an
  error is detected, a message is logged and the status and function name are
  placed in the given metadata structure.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       RetStat            Return status value to check.
  @param[in]       FileName           File name.
  @param[in]       LineNumber         Line number.
  @param[out]      pTestStat          Pointer to test status in results metadata structure (or NULL).

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 ChkRetStat(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN SSA_STATUS RetStat,
  IN char* FileName,
  IN UINT32 LineNumber,
  OUT TEST_STATUS *pTestStat)
{
  if (RetStat != Success) {
    ProcError(SsaServicesHandle, RetStat, FileName, LineNumber, pTestStat);

    return 1;
  }

  return 0;
} // end function ChkRetStat

#endif  // SSA_FLAG

// end file ChkRetStat.c
