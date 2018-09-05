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
  GetCpuInfo.c

@brief
  This file contains a function to get information about the CPU.
**/
#ifdef SSA_FLAG

#include "GetCpuInfo.h"

#ifndef IA32
#include <CpuDataStruct.h>
#endif

// flag to enable verbose log messages
#define ENBL_VERBOSE_LOG_MSGS (0)

/**
@brief
  This function is used to get information about the CPU.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[out]      pCpuInfo           Pointer to information about the CPU.

  @retval  Nothing.
**/
VOID GetCpuInfo(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  OUT CPU_INFO* pCpuInfo)
{
#ifdef __STUB__
  bssa_memset(pCpuInfo, 0, sizeof(*pCpuInfo));

  pCpuInfo->ProcessorType  = SKX_PROC_TYPE;
  pCpuInfo->Family         = SKX_FAMILY;
  pCpuInfo->ExtendedFamily = SKX_EXT_FAMILY;
  pCpuInfo->Model          = SKX_MODEL;
  pCpuInfo->ExtendedModel  = SKX_EXT_MODEL;
  pCpuInfo->Stepping       = SKX_A0_STEPPING;
#else
  UINT32 RegEax = 1;
#ifdef IA32
#ifdef __GNUC__
  asm (
    "movl %1, %%eax\n\t"
    "cpuid\n\t"
    "movl %%eax, %0\n\t"
    : "=&r"(RegEax) : "r"(RegEax) : "%eax"
  );
#else
  _asm{
    MOV eax, RegEax
    CPUID
    MOV RegEax, eax
  }
#endif
#else
  UINT32 RegEbx;
  UINT32 RegEcx;
  UINT32 RegEdx;
  EFI_CPUID_REGISTER cpuIdRegisters;

  AsmCpuid(RegEax, &cpuIdRegisters.RegEax, &cpuIdRegisters.RegEbx, &cpuIdRegisters.RegEcx, &cpuIdRegisters.RegEdx);

  RegEax = cpuIdRegisters.RegEax;
  //*RegEbx = cpuIdRegisters.RegEbx;
  //*RegEcx = cpuIdRegisters.RegEcx;
  //*RegEdx = cpuIdRegisters.RegEdx;
#endif // !IA32
  *pCpuInfo = *((CPU_INFO*) &RegEax);
#endif // !__STUB__

#if ENBL_VERBOSE_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "CPU Stepping=%u Model=%u Family=%u ProcessorType=%u ExtendedModel=%u ExtendedFamily=%u\n",
    pCpuInfo->Stepping, pCpuInfo->Model, pCpuInfo->Family,
    pCpuInfo->ProcessorType, pCpuInfo->ExtendedModel, pCpuInfo->ExtendedFamily);
#endif // ENBL_VERBOSE_LOG_MSGS
} // end function GetCpuInfo

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
  IN CPU_INFO CpuInfo)
{
  if ((CpuInfo.ProcessorType == SKX_PROC_TYPE) &&
    (CpuInfo.Family == SKX_FAMILY) &&
    (CpuInfo.ExtendedFamily == SKX_EXT_FAMILY) &&
    (CpuInfo.Model == SKX_MODEL) &&
    (CpuInfo.ExtendedModel == SKX_EXT_MODEL)) {
    return TRUE;
  }

  return FALSE;
} // end function IsCpuSkx

#endif  // SSA_FLAG

// end file GetCpuInfo.c
