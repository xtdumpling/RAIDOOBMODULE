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
  Copyright (c) 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  GetCpuInfo.h

@brief
  This file contains definitions for a function to get information about the
  CPU.
**/
#ifndef __GET_CPU_INFO_H__
#define __GET_CPU_INFO_H__

#ifdef SSA_FLAG

#include "ssabios.h"

#define SKX_PROC_TYPE    0x0
#define SKX_FAMILY       0x6
#define SKX_EXT_FAMILY   0x1
#define SKX_MODEL        0x5
#define SKX_EXT_MODEL    0x4

//
// SKX stepping values
//
#define SKX_A0_STEPPING 0x0
#define SKX_A2_STEPPING 0x1
#define SKX_B0_STEPPING 0x2
#define SKX_B1_STEPPING 0x3
#define SKX_C0_STEPPING 0x5


typedef struct {
  UINT32 Stepping       : 4;
  UINT32 Model          : 4;
  UINT32 Family         : 4;
  UINT32 ProcessorType  : 2;
  UINT32 ExtendedModel  : 4;
  UINT32 ExtendedFamily : 8;
} CPU_INFO;

/**
@brief
  This function is used to get information about the CPU.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[out]      pCpuInfo           Pointer to CPU information structure.

  @retval  Nothing.
**/
VOID GetCpuInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  OUT CPU_INFO* pCpuInfo);

/**
@brief
  This function is used to determine if the CPU is SKX.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       CpuInfo            CPU information structure.

  @retval  TRUE  = CPU is SKX.
  @retval  FALSE = CPU is not SKX.
**/
BOOLEAN IsCpuSkx(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CPU_INFO CpuInfo);

#endif  // SSA_FLAG
#endif  // __GET_CPU_INFO_H__

// end file GetCpuInfo.h
