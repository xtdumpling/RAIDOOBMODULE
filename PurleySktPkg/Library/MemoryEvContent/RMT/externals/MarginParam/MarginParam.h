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
  MarginParam.h

@brief
  This file contains product specific definitions for margin parameters.
**/
#ifndef __MARGIN_PARAM_H__
#define __MARGIN_PARAM_H__

#ifdef SSA_FLAG

#include "ssabios.h"

#include "MemCfg.h"
#include "MemPointTest.h"

/**
  This function is used to get a string associated with the given I/O level.

  @param[in]   IoLevel   I/O level.

  @retval   String describing the I/O level.
**/
CONST char* GetIoLevelStr(
  GSM_LT IoLevel);

/**
  This function is used to get a string associated with the given margin group.

  @param[in]   MarginGroup   Margin group.

  @retval   String describing the margin group.
**/
CONST char* GetMarginGroupStr(
  GSM_GT MarginGroup);

/**
  This function is used to determine if the given margin parameter requires
  incremental stepping.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   Incremental stepping is required.
  @retval  FALSE  Incremental stepping is not required.
**/
BOOLEAN IsIncrementalSteppingRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup);

/**
  This function is used to determine if the given margin parameter requires
  an I/O reset after an error is detected.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   I/O reset is required.
  @retval  FALSE  I/O reset is not required.
**/
BOOLEAN IsIoResetAfterErrRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup);

/**
  This function is used to determine if the given margin parameter requires
  a JEDEC initialization after an error is detected.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   Incremental stepping is required.
  @retval  FALSE  Incremental stepping is not required.
**/
BOOLEAN IsJedecInitAfterErrRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup);

/**
  This function is used to get strobe width for the given margin parameter and
  DIMM.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.
  @param[in]       Socket             Zero-based Socket.
  @param[in]       Controller         Zero-based Controller.
  @param[in]       Channel            Zero-based Channel.
  @param[in]       Dimm               Zero-based DIMM.

  @retval  Strobe width.
**/
UINT8 GetMarginParamStrobeWidth(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_CFG *pMemCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 Dimm);

/**
  This function is used to determine if the given margin group is specific to
  DDR-T.

  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   The margin group is specific to DDR-T.
  @retval  FALSE  The margin group is not specific to DDR-T.
**/
BOOLEAN IsMarginGroupDdrTSpecific(
  IN GSM_GT MarginGroup);

/**
  This function is used to determine if the given margin group is for
  Early-Read-ID.

  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   The margin group is for Early-Read-ID.
  @retval  FALSE  The margin group is not for Early-Read-ID.
**/
BOOLEAN IsMarginGroupForErid(
  IN GSM_GT MarginGroup);

/**
  This function is used to determine if the given margin parameter requires
  the point test timeout feature to be enabled.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   Point test timeout is required.
  @retval  FALSE  Point test timeout is not required.
**/
BOOLEAN IsPointTestTimeoutRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup);

#endif // SSA_FLAG
#endif // __MARGIN_PARAM_H__

// end file MarginParam.h
