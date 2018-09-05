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
  GetMemPoolSize.c

@brief
  This file contains a function to get the current memory pool size.
**/
#ifdef SSA_FLAG

#include "GetMemPoolSize.h"

// flag to enable function trace debug messages
#define ENBL_FUNCTION_TRACE_DEBUG_MSGS (0)

// flag to enable Malloc debug log messages
#define ENBL_MALLOC_DEBUG_LOG_MSGS (0)

// flag to enable memory pool debug log messages
#define ENBL_MEM_POOL_DEBUG_LOG_MSGS (0)

/**
  Function used to get the current memory pool size.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.

  @retval  Memory pool size.
**/
UINT32 GetMemPoolSize(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle)
{
  UINT32 BfrSize, ExtraBfrSize;
  VOID* pBfr;

#if ENBL_FUNCTION_TRACE_DEBUG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "GetMemPoolSize()\n");
#endif

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // get the largest 2n buffer
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  BfrSize = 1;
  do {
    // try allocating a buffer of this size
#if ENBL_MALLOC_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Malloc(Size=%u) for DqDB buffer\n", BfrSize);
#endif
    pBfr = (UINT64*)SsaServicesHandle->SsaCommonConfig->Malloc(
      SsaServicesHandle, BfrSize);
#if ENBL_MALLOC_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Free(pBfr=%p)\n", pPattern);
#endif
    // IF the buffer size was available
    if (pBfr != NULL) {
      SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle, pBfr);
    }
    else {
      // the previous 2n value is the biggest 2n size available
      BfrSize >>= 1;
      break;
    }

    // IF this is the biggest 2n value we can try
    if (BfrSize == (1 << 31)) {
      break;
    }

    // setup to try the next larger 2n value
    BfrSize <<= 1;
  } while (pBfr != NULL);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // explore the remaining available buffer by decreasing powers of 2
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  ExtraBfrSize = BfrSize >> 1;
  do {
    if (ExtraBfrSize == 0) {
      break;
    }

    // try allocating a buffer of this size
#if ENBL_MALLOC_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Malloc(Size=%u) for DqDB buffer\n", (BfrSize + ExtraBfrSize));
#endif
    pBfr = (UINT64*)SsaServicesHandle->SsaCommonConfig->Malloc(
      SsaServicesHandle, (BfrSize + ExtraBfrSize));
#if ENBL_MALLOC_DEBUG_LOG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Free(pBfr=%p)\n", pPattern);
#endif
    // IF the buffer size was available
    if (pBfr != NULL) {
      SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle, pBfr);

      // add this 2n value to the buffer size
      BfrSize += ExtraBfrSize;
    }

    // setup for the next smaller 2n value
    ExtraBfrSize >>= 1;
  } while (ExtraBfrSize);

#if ENBL_MEM_POOL_DEBUG_LOG_MSGS
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "  MemPoolSize=%u\n", BfrSize);
#endif

  return BfrSize;
} // end function GetMemPoolSize

#endif  // SSA_FLAG

// end file GetMemPoolSize.c
