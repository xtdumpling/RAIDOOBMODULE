/** @file
Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __MEMORY_EVCONTENT_LIB_H__
#define __MEMORY_EVCONTENT_LIB_H__

#ifdef SSA_FLAG
#include "MrcSsaServices.h"

/**
  This function is the Run CPGC Point Test test entry point.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       Config             Pointer to test configuration structure.

  @retval  0  success
  @retval  1  error occurred
**/
EFI_STATUS
EFIAPI
TestEntryPoint (
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN VOID *Config
);
#endif //SSA_FLAG
#endif // __MEMORY_EVCONTENT_LIB_H__
