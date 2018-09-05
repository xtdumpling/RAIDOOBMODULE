/** @file
  Support routines for RDRAND instruction access.

Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "RdRand.h"

/**
  Calls RDRAND to obtain a 64-bit random number.

  @param[out]  Rand          Buffer pointer to store the random result.
  @param[in]   NeedRetry     Determine whether or not to loop retry.

  @retval EFI_SUCCESS        RDRAND call was successful.
  @retval EFI_NOT_READY      Failed attempts to call RDRAND.

**/
EFI_STATUS
EFIAPI
RdRand64 (
  OUT UINT64       *Rand,
  IN BOOLEAN       NeedRetry
  )
{
  UINT32      Index;
  UINT32      RetryCount;

  if (NeedRetry) {
    RetryCount = RETRY_LIMIT;
  } else {
    RetryCount = 1;
  }

  //
  // Perform a single call to RDRAND, or enter a loop call until RDRAND succeeds.
  //
  for (Index = 0; Index < RetryCount; Index++) {
    if (RdRand64Step (Rand)) {
      return EFI_SUCCESS;
    }
  }
  
  return EFI_NOT_READY;
}
