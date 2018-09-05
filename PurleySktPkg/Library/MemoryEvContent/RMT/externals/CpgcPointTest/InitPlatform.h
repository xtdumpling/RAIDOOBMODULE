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
  Copyright (c) 2014-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  InitPlatform.h

@brief
  This file contains definitions for functions that perform product specific
  platform initialization.
**/
#ifndef __INIT_PLATFORM_H__
#define __INIT_PLATFORM_H__

#ifdef SSA_FLAG

#include "ssabios.h"
#include "MemCfg.h"
#include "MemPointTest.h"
#include "Platform.h"

typedef enum {
  DisableScrambler   = 0,  ///< disable the scrambler during setup; restore it during cleanup
  EnableScrambler    = 1,  ///< enable the scrambler during setup; restore it during cleanup
  DontTouchScrambler = 2,  ///< don't touch the scrambler value during setup and cleanup
  OverrideScramblerMax,    ///< SCRAMBLER_OVERRIDE_MODE enumeration maximum value.
  SCRAMBLER_OVERRIDE_MODE_DELIM = INT32_MAX ///< This value ensures the enum size is consistent on both sides of the PPI.
} SCRAMBLER_OVERRIDE_MODE;

/**
  This function performs product specific platform initialization for running
  EV margining tests.

  @param[in, out]  SsaServicesHandle       Pointer to SSA services.
  @param[in]       pSystemInfo             Pointer to system information structure.
  @param[in]       pMemCfg                 Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg        Pointer to generic memory point test configuration structure.
  @param[in]       SetupCleanup            Specifies setup or cleanup action.
  @param[in]       ScramblerOverrideMode   Specifies mode for overriding the scrambler.
  @param[in, out]  PrevScramblerEnables    Pointer to array of previous ZQCal enable values.
  @param[in, out]  PrevSelfRefreshEnables  Pointer to array of previous self-refresh enable values.

  @retval  Nothing.
**/
VOID InitPlatform(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN SETUP_CLEANUP SetupCleanup,
  IN SCRAMBLER_OVERRIDE_MODE ScramblerOverrideMode,
  IN OUT BOOLEAN PrevScramblerEnables[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT],
  IN OUT BOOLEAN PrevSelfRefreshEnables[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT]);

#endif  // SSA_FLAG
#endif  // __INIT_PLATFORM_H__

// end file InitPlatform.h
