/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
  Copyright (c) 2013-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  RMT.h

  Header file for the PEIM used to create downloadable Dlls.
**/
#ifndef __RMT_H__
#define __RMT_H__

#ifdef SSA_FLAG

#include "ssabios.h"

#ifndef BSSA_TC_MAJOR_VER
#define BSSA_TC_MAJOR_VER 3
#endif

#ifndef BSSA_TC_MINOR_VER
#define BSSA_TC_MINOR_VER 19
#endif

#ifndef BSSA_TC_REV_VER
#define BSSA_TC_REV_VER 4
#endif

#ifndef BSSA_TC_BUILD_VER
#define BSSA_TC_BUILD_VER 0
#endif

/**
  This function is the RMT test entry point.

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

#endif // SSA_FLAG
#endif // __RMT_H__

// end file RMT.h
