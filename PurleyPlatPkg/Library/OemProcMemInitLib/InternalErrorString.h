//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file InternalErrorString.h


**/

#ifndef _INTERNAL_ERROR_STRING_H_
#define _INTERNAL_ERROR_STRING_H_

#include <Library/FastVideoPrintLib.h>
#include <MemPlatformCommon.h>
#include <KtiMisc.h>

typedef struct {
  UINT8                   ErrorCode;
  CHAR8                   ErrorString[VIDEO_PRINT_MAX_STRING];
} ERROR_CODE_LOOKUP_TABLE;

ERROR_CODE_LOOKUP_TABLE mErrorCodeToken[] = {
  // Supported errors
  { ERR_NO_MEMORY,                       "Fatal Error : (MEMORY) No Memory Detected!"                                  },//0xE8
  { ERR_MEM_TEST,                        "Fatal Error : (MEMORY) Memory Test Failed!"                                  },//0xEB
  { ERR_DIMM_COMPAT,                     "Fatal Error : (MEMORY) Mixed DIMM or Invalid DIMM Population Error!"         },//0xED
  { ERR_MRC_COMPATIBILITY,               "Fatal Error : (MEMORY) MRC doesn't support non-ECC DIMM!"                    },//0xEE
  { ERR_SET_VDD,                         "Fatal Error : (MEMORY) Active DRAM VR Failed!"                               },//0xF0
  { ERR_SMBUS,                           "Fatal Error : (MEMORY) Read Processor SMBUS with TSOD Polling Failed!"       },//0xF6
  // End of error code table
  { 0x0,                                 "Fatal Error : System Encounter A Stopper Error!"                             }// other halt errors.
};

#endif
