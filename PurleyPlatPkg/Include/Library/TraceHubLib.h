/** @file
  Definitions for the TraceHub Lib.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains 'Framework Code' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may not be modified, except as allowed by
 additional terms of your license agreement.

@par Specification Reference:
**/

#ifndef _TRACE_HUB_LIB_H_
#define _TRACE_HUB_LIB_H_

#include <Uefi/UefiBaseType.h>

/**
  Calculate TraceHub Trace Address.

  @param [in] Thread          Master is being used.
  @param [in] Channel         Channel is being used.
  @param [out] TraceAddress   TraceHub Address for the Master/Channel
                              is being used.

  @retval                     RETURN_SUCCESS      Retrieve correct Address.
                              RETURN_UNSUPPORTED  Not implemented.
                              Other               Fail to Retrieve correct
                                                  TraceHub Address.

**/
RETURN_STATUS
EFIAPI
TraceHubMmioTraceAddress (
  IN  UINT16    Master,
  IN  UINT16    Channel,
  OUT UINT32    *TraceAddress
  );

#endif // _TRACE_HUB_LIB_H_
