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
  ChkRetStat.h

@brief
  This file contains definitions for functions for checking return status values.
**/
#ifndef __CHK_RET_STAT_H__
#define __CHK_RET_STAT_H__

#ifdef SSA_FLAG

#include "ssabios.h"

#pragma pack (push, 1)

typedef struct TEST_STATUS {
  // Test status code. 0=Success
  UINT8 StatusCode;

  // Line number where error occurred.
  UINT16 LineNumber;

  // Least significant portion of name of file where error occurred.
  CHAR8 FileName[13];
} TEST_STATUS;

#pragma pack (pop)

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
  OUT TEST_STATUS *pTestStat);

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
  OUT TEST_STATUS *pTestStat);

#endif  // SSA_FLAG
#endif  // __CHK_RET_STAT_H__

// end file ChkRetStat.h
