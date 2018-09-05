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
  GetMemPoolSize.h

@brief
  This file contains definitions for a function to create the largest result
  buffer possible.
**/
#ifndef __GET_MEM_POOL_SIZE_H__
#define __GET_MEM_POOL_SIZE_H__

#ifdef SSA_FLAG

#include "ssabios.h"

/**
  Function used to get the current memory pool size.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.

  @retval  Memory pool size.
**/
UINT32 GetMemPoolSize(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle);

#endif  // SSA_FLAG
#endif // __GET_MEM_POOL_SIZE_H__

// end file GetMemPoolSize.h
