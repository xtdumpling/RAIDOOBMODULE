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
  CommonInitPlatform.h

@brief
  This file contains definitions for functions that perform common platform
  initialization.
**/
#ifndef __COMMON_INIT_PLATFORM_H__
#define __COMMON_INIT_PLATFORM_H__

#ifdef SSA_FLAG

#include "ssabios.h"
#include "MemCfg.h"
#include "MemPointTest.h"
#include "Platform.h"

typedef enum {
  DisablePageOpen   = 0,  ///< disable the page-open policy during setup; restore it during cleanup
  EnablePageOpen    = 1,  ///< enable the page-open policy during setup; restore it during cleanup
  DontTouchPageOpen = 2,  ///< don't touch the page-open policy value during setup and cleanup
  OverridePageOpenMax,    ///< PAGE_OPEN_OVERRIDE_MODE enumeration maximum value.
  PAGE_OPEN_OVERRIDE_MODE_DELIM = INT32_MAX ///< This value ensures the enum size is consistent on both sides of the PPI.
} PAGE_OPEN_OVERRIDE_MODE;

/**
  This function performs page-open policy initialization for running EV
  margining tests.  When the SetupCleanup parameter is set to Setup, the
  page-open policy is set per the given PageOpenOverrideMode parameter value
  and (if PageOpenOverrideMode parameter is not DontTouchPageOpen then) the
  previous settings are returned in the PrevPageOpenEnables parameter.  When
  the SetupCleanup parameter is set to Cleanup (and the PageOpenOverrideMode
  parameter is not DontTouchPageOpen), the values in the PrevPageOpenEnables
  parameter are restored.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       SetupCleanup       Specifies setup or cleanup action.
  @param[in]       PageOpenOverrideMode   Specifies mode for overriding the page-open policy.
  @param[in, out]  PrevPageOpenEnables   Pointer to array of previous page-open policy enable values.

  @retval  Nothing.
**/
VOID InitPageOpen(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN SETUP_CLEANUP SetupCleanup,
  IN PAGE_OPEN_OVERRIDE_MODE PageOpenOverrideMode,
  IN OUT BOOLEAN PrevPageOpenEnables[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT]);

/**
  This function performs ZQCal initialization for running EV margining tests.
  When SetupCleanup is set to Setup, the ZQCal is disabled and the previous
  settings are returned in the PrevZQCalEnables parameter.  When SetupCleanup
  is set to Cleanup, the values in the PrevZQCalEnables are restored.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       SetupCleanup       Specifies setup or cleanup action.
  @param[in, out]  PrevZQCalEnables   Pointer to array of previous ZQCal enable values.

  @retval  Nothing.
**/
VOID InitZQCal(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN SETUP_CLEANUP SetupCleanup,
  IN OUT BOOLEAN PrevZQCalEnables[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT]);

/**
  This function performs RComp update initialization for running EV margining tests.
  When SetupCleanup is set to Setup, the RComp update is disabled and the previous
  settings are returned in the PrevRCompUpdateEnables parameter.  When SetupCleanup
  is set to Cleanup, the values in the PrevRCompUpdateEnables are restored.

  @param[in, out]  SsaServicesHandle       Pointer to SSA services.
  @param[in]       pSystemInfo             Pointer to system information structure.
  @param[in]       pMemCfg                 Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg        Pointer to generic memory point test configuration structure.
  @param[in]       SetupCleanup            Specifies setup or cleanup action.
  @param[in, out]  PrevRCompUpdateEnables  Pointer to array of previous RComp update enable values.

  @retval  Nothing.
**/
VOID InitRCompUpdate(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN SETUP_CLEANUP SetupCleanup,
  IN OUT BOOLEAN PrevRCompUpdateEnables[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT]);

#endif  // SSA_FLAG
#endif  // __COMMON_INIT_PLATFORM_H__

// end file CommonInitPlatform.h
