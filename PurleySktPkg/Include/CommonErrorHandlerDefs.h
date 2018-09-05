/** @file
  RAS specific definitions.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#ifndef _COMMON_ERRORHANDLER_DEFS_H_
#define _COMMON_ERRORHANDLER_DEFS_H_

#include <Library/mpsyncdatalib.h>
#include <Library/PcdLib.h>
#include <Base.h>

//
// Bit map macro
//
#define BITS(x)  (1<<(x))

//
// Categorized error types for logging and viewing
//
#define INVALID_ERROR_TYPE        0x00
#define NONFATAL_ERROR_TYPE       0x01
#define FATAL_ERROR_TYPE          0x02
#define COR_ERROR_TYPE            0x03
#define SERROR_TYPE               0x04
#define PERROR_TYPE               0x05
#define DPERROR_TYPE              0x06
#define REC_MASTER_ABORT_TYPE     0x07
#define REC_TARGET_ABORT_TYPE     0x08
#define REC_SERROR_TYPE           0x09
#define REDUNDANCY_ERROR_TYPE     0x0a
#define LEGACY_ERROR_TYPE         0x0b
#define EXTENDED_ERROR_TYPE       0x0c
#define RECOVERABLE_ERROR_TYPE    0x0d
#define MISCERROR_TYPE            0xee

#define GES_SIZE SIZE_4KB


typedef enum {
  SmmMceClearedState = 0,
  SmmMceReadingState,
  SmmMceProcessingState,
  SmmMceWritingState,
  SmmMceLoggingState,
  SmmMceReadingInterrupted,
  SmmMceWritingInterrupted,
  SmmMceProcessingInterrupted,
} SMM_MCE_BANK_SYNC_STATE;


typedef struct _MCA_BANK_ERR_TYPE {
  UINT16                        UnitType;
  UINT8                         ErrType;
} MCA_BANK_ERR_TYPE;
#endif


