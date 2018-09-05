/**

@copyright
  Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
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

@file
  ExecuteTargetOnly.h

@brief
  BIOS SSA loader ITP implementation.

**/

#ifndef __EXECUTE_TARGET_ONLY_H__
#define __EXECUTE_TARGET_ONLY_H__

#include "EvItpDownloadAgent.h"

#ifdef SSA_FLAG
#ifdef SSA_LOADER_FLAG
/**

  BIOS SSA test tool

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID ExecuteTargetOnlyCmd(PSYSHOST host);
#endif //SSA_LOADER_FLAG
#else
//Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif //__EXECUTE_TARGET_ONLY_H__
