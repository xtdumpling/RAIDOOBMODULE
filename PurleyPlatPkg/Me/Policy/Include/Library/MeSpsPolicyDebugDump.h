/** @file
  ME SPS Debug Dump Policy definitions.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#ifndef _ME_SPS_POLICY_DEBUG_DUMP_H_
#define _ME_SPS_POLICY_DEBUG_DUMP_H_

#include <Protocol/MeSpsPolicyProtocol.h>

/**

  Dump ME SPS Policy function called from Policy

  @param[in] DebugLevel - Specifies used debug level for dump output.
  @param[in] WhatToDump - Specifies what part of policy has to be dumped.

**/
VOID
MePolicyDump (
  UINTN                   DebugLevel,
  ME_POLICY_DUMP_TYPE     WhatToDump
  )
;

#endif // _ME_SPS_POLICY_DEBUG_DUMP_H_
