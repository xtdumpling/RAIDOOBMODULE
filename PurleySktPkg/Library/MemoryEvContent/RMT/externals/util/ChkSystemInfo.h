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
  Copyright (c) 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  ChkSystemInfo.h

@brief
  This file contains definitions for a function to check the system information
  values against the corresponding preprocessor definition values.
**/
#ifndef __CHK_SYSTEM_INFO_H__
#define __CHK_SYSTEM_INFO_H__

#ifdef SSA_FLAG

#include "ssabios.h"
#include "ChkRetStat.h"

/**
@brief
  This function checks the given system information values against the
  corresponding preprocessor definition values.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 ChkSystemInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN MRC_SYSTEM_INFO *pSystemInfo,
  OUT TEST_STATUS *pTestStat);

#endif  // SSA_FLAG
#endif  // __CHK_SYSTEM_INFO_H__

// end file ChkSystemInfo.h
