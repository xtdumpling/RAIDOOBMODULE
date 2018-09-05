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
  ChkSystemInfo.c

@brief
  This file contains definitions for a function to check the system information
  values against the corresponding preprocessor definition values.
**/
#ifdef SSA_FLAG

#define BSSA_LOGGING_LEVEL BSSA_LOGGING_INTERNAL_DEBUG_LEVEL

#include "ssabios.h"
#include "bssalogging.h"
#include "ChkSystemInfo.h"
#include "Platform.h"

// flag to enable verbose error messages
#define ENBL_VERBOSE_ERROR_MSGS (0)

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
  OUT TEST_STATUS *pTestStat)
{
  if ((pSystemInfo->MaxNumberSockets > MAX_SOCKET_CNT) ||
    (pSystemInfo->MaxNumberControllers > MAX_CONTROLLER_CNT) ||
    (pSystemInfo->MaxNumberChannels > MAX_CHANNEL_CNT) ||
    (pSystemInfo->MaxNumberDimms > MAX_DIMM_CNT) ||
    (pSystemInfo->MaxNumberRanks > MAX_RANK_CNT) ||
    (pSystemInfo->BusWidth > MAX_BUS_WIDTH)) {
#if ENBL_VERBOSE_ERROR_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "ERROR: SystemInfo value(s) exceed preprocessor value(s).\n");
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "MaxNumberSockets=%u MAX_SOCKET_CNT=%u\n"
      "MaxNumberControllers=%u MAX_CONTROLLER_CNT=%u\n",
      pSystemInfo->MaxNumberSockets, MAX_SOCKET_CNT,
      pSystemInfo->MaxNumberControllers, MAX_CONTROLLER_CNT);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "MaxNumberChannels=%u MAX_CHANNEL_CNT=%u\n"
      "MaxNumberDimms=%u MAX_DIMM_CNT=%u\n",
      pSystemInfo->MaxNumberChannels, MAX_CHANNEL_CNT,
      pSystemInfo->MaxNumberDimms, MAX_DIMM_CNT);
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "MaxNumberRanks=%u MAX_RANK_CNT=%u\n"
      "BusWidth=%u MAX_BUS_WIDTH=%u\n",
      pSystemInfo->MaxNumberRanks, MAX_RANK_CNT,
      pSystemInfo->BusWidth, MAX_BUS_WIDTH);
#endif
    ProcError(SsaServicesHandle, Failure, __FILE__, __LINE__, pTestStat);
    return 1;
  }

  return 0;
} // end function ChkSystemInfo

#endif  // SSA_FLAG

// end file ChkSystemInfo.c
