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
  Copyright (c) 2014-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  InitPlatform.c

@brief
  This file contains functions that perform product specific platform
  initialization.
**/
#ifdef SSA_FLAG

#include "InitPlatform.h"

// flag to enable BIOS-SSA debug messages
#define ENBL_BIOS_SSA_API_DEBUG_MSGS (0)

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
  IN OUT BOOLEAN PrevSelfRefreshEnables[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT])
{
  UINT8 Socket, Controller;
  BOOLEAN ScramblerEnable = (ScramblerOverrideMode == EnableScrambler) ? TRUE : FALSE;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not enabled for testing THEN skip it
      if (!IsControllerTestingEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
        pMemPointTestCfg, Socket, Controller)) {
        continue;
      }

      // IF this call is for setup
      if (SetupCleanup == Setup) {
        // IF the scrambler is to be overridden
        if (ScramblerOverrideMode != DontTouchScrambler) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SetScramblerConfig(Socket=%u Controller=%u Enable=%u)\n",
            Socket, Controller, ScramblerEnable);
#endif
          SsaServicesHandle->SsaMemoryServerConfig->SetScramblerConfig(SsaServicesHandle,
            Socket, Controller, ScramblerEnable, &PrevScramblerEnables[Socket][Controller]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "    PrevScramblerEnable=%u\n",
            PrevScramblerEnables[Socket][Controller]);
#endif
        } // end if the scrambler is to be overridden

        // disable self-refresh
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "  SetSelfRefreshConfig(Socket=%u Controller=%u Enable=%u)\n",
          Socket, Controller, FALSE);
#endif
        SsaServicesHandle->SsaMemoryServerConfig->SetSelfRefreshConfig(SsaServicesHandle,
          Socket, Controller, FALSE, &PrevSelfRefreshEnables[Socket][Controller]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    PrevSelfRefreshEnable=%u\n",
          PrevSelfRefreshEnables[Socket][Controller]);
#endif
        // ensure that the value is a simple TRUE/FALSE Boolean
        PrevSelfRefreshEnables[Socket][Controller] = \
          (PrevSelfRefreshEnables[Socket][Controller]) ? TRUE : FALSE;
      }
      // ELSE this call is for cleanup
      else {
        // IF the values were changed THEN restore them
        if (PrevSelfRefreshEnables[Socket][Controller] != FALSE) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "  SetSelfRefreshConfig(Socket=%u Controller=%u Enable=%u)\n",
            Socket, Controller,
            PrevSelfRefreshEnables[Socket][Controller]);
#endif
          SsaServicesHandle->SsaMemoryServerConfig->SetSelfRefreshConfig(SsaServicesHandle,
            Socket, Controller, PrevSelfRefreshEnables[Socket][Controller], NULL);
        } // end if the values were changed

        // IF the scrambler was overridden
        if (ScramblerOverrideMode != DontTouchScrambler) {
          // IF the value was changed THEN restore it
          if (PrevScramblerEnables[Socket][Controller] != ScramblerEnable) {
#if ENBL_BIOS_SSA_API_DEBUG_MSGS
            NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
              "  SetScramblerConfig(Socket=%u Controller=%u Enable=%u)\n",
              Socket, Controller, PrevScramblerEnables[Socket][Controller]);
#endif
            SsaServicesHandle->SsaMemoryServerConfig->SetScramblerConfig(SsaServicesHandle,
              Socket, Controller, PrevScramblerEnables[Socket][Controller], NULL);
          } // end if the value was changed
        } // end if the scrambler was overridden
      } // end else this call is for cleanup
    } // end for each controller
  } // end for each socket
} // end function ClientInitPlatform

#endif // SSA_FLAG

// end file InitPlatform.c
