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
  MemCfg.h

@brief
  This file contains definitions for the MemSet function.
**/
#ifndef __MEM_CFG_H__
#define __MEM_CFG_H__

#ifdef SSA_FLAG

#include "ssabios.h"
#include "ChkRetStat.h"
#include "Platform.h"

// memory configuration
typedef struct {
  // Note that the bitmask of the sockets populated is available in the MRC_SYSTEM_INFO structure
  UINT8 ControllerBitmasks[MAX_SOCKET_CNT]; ///< bitmasks for controllers populated
  UINT8 ChannelBitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT]; ///< bitmasks for channels populated
  UINT8 DimmBitmasks[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT]; ///< bitmasks of DIMMs populated
  UINT8 RankCounts[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_DIMM_CNT]; ///< number or ranks
  // Note that the following field uses UINT8 instead of MEMORY_TECHNOLOGY to conserve space.
  UINT8 MemoryTech[MAX_SOCKET_CNT][MAX_CONTROLLER_CNT][MAX_CHANNEL_CNT][MAX_DIMM_CNT]; ///< memory technology type
  BOOLEAN IsLrDimms; ///< flag indicating whether the system contains LRDIMMs (excludes DDR-T devices)
} MEM_CFG;

/**
  This function gets the memory configuration and stores it in the given structure.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[out]      pMemCfg            Pointer to memory configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 GetMemCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  OUT MEM_CFG *pMemCfg,
  OUT TEST_STATUS *pTestStat);

/**
  This function logs the contents of the given memory configuration structure.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
**/
VOID LogMemCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN MEM_CFG *pMemCfg);

#if SUPPORT_FOR_DDRT
/**
  This function determines whether the given memory configuration contains any
  DDR-T devices.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.

  @retval  TRUE   The memory configuration has at least one DDR-T device.
  @retval  FALSE  The memory configuration has no DDR-T devices.
**/
BOOLEAN GetHasDdrtDevices(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg);
#endif // SUPPORT_FOR_DDRT

#endif  // SSA_FLAG
#endif  // __MEM_CFG_H__

// end file MemCfg.h
