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
  GetActualMallocSize.h

@brief
  This file contains definitions for a function to get the actual Malloc buffer
  size.
**/
#ifndef __GET_ACTUAL_MALLOC_SIZE_H__
#define __GET_ACTUAL_MALLOC_SIZE_H__

#ifdef SSA_FLAG

#include "ssabios.h"

/**
@brief
  This function is used to get the actual Malloc buffer size.

  @param[in]  ReqBufferSize  Requested buffer size (in bytes).

  @retval  The actual Malloc buffer size.
**/
UINT32 GetActualMallocSize(
  IN UINT32 ReqBufferSize);

#endif  // SSA_FLAG
#endif  // __GET_ACTUAL_MALLOC_SIZE_H__

// end file GetActualMallocSize.h
