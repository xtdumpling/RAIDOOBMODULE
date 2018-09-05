/** @file
  Definitions for the TraceHub Debug Lib.

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

#ifndef _TRACE_HUB_DEBUG_LIB_H_
#define _TRACE_HUB_DEBUG_LIB_H_

typedef enum {
  SeverityNone = 0,
  SeverityFatal = 1,
  SeverityError = 2,
  SeverityWarning = 3,
  SeverityNormal = 4,
  SeverityUser1 = 5,
  SeverityUser2 = 6,
  SeverityUser3 = 7,
  SeverityMax
} TRACE_HUB_SEVERITY_TYPE;

RETURN_STATUS
EFIAPI
TraceHubDebugWrite (
  IN TRACE_HUB_SEVERITY_TYPE    SeverityType,
  IN UINT8                      *Buffer,
  IN UINTN                      NumberOfBytes
  );

#endif // _TRACE_HUB_DEBUG_LIB_H_
