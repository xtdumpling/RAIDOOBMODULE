/** @file
  Header for the RDRAND APIs used by RNG DXE driver.

  Support API definitions for RDRAND instruction access, which will leverage
  Intel Secure Key technology to provide high-quality random numbers for use
  in applications, or entropy for seeding other random number generators.
  Refer to http://software.intel.com/en-us/articles/intel-digital-random-number
  -generator-drng-software-implementation-guide/ for more information about Intel
  Secure Key technology.

Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __RD_RAND_H__
#define __RD_RAND_H__

#include <Library/BaseLib.h>

//
// The maximun number of retries to obtain one available random number. 
//
#define RETRY_LIMIT    10

/**
  Generates a 64-bit random number through RDRAND instruction.

  @param[out]  Rand          Buffer pointer to store the random result.

  @retval TRUE               RDRAND call was successful.
  @retval FALSE              Failed attempts to call RDRAND.

**/
BOOLEAN
EFIAPI
RdRand64Step  (
  OUT UINT64   *Rand
  );


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
  );
 
#endif  // __RD_RAND_H__