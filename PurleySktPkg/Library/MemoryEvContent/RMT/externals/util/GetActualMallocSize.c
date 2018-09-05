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
  Copyright (c) 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  GetActualMallocSize.c

@brief
  This file contains a function to get the actual Malloc buffer size.
**/
#ifdef SSA_FLAG

#include "GetActualMallocSize.h"
#include "Platform.h"

/**
@brief
  This function is used to get the actual Malloc buffer size.

  @param[in]  ReqBufferSize  Requested buffer size (in bytes).

  @retval  The actual Malloc buffer size.
**/
UINT32 GetActualMallocSize(
  IN UINT32 ReqBufferSize)
{
  UINT32 ActualMallocSize = ReqBufferSize;

  // IF the requested buffer size is not an integral of the Malloc buffer
  // granularity
  if (ReqBufferSize % MALLOC_BUFFER_GRANULARITY) {
    // increase the buffer size per the granularity
    ActualMallocSize = ReqBufferSize;
    ActualMallocSize /= MALLOC_BUFFER_GRANULARITY;
    ActualMallocSize += 1;
    ActualMallocSize *= MALLOC_BUFFER_GRANULARITY;
  }

  // add in the space for the Malloc header
  ActualMallocSize += MALLOC_HEADER_SIZE;

  return ActualMallocSize;
} // end function GetActualMallocSize

#endif  // SSA_FLAG

// end file GetActualMallocSize.c
