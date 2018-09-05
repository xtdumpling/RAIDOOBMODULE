/**
//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MemApiSkx.h

  Prototypes for KTI functions called from BaseMemoryCoreLib

**/

#ifndef _KTI_API_H_
#define _KTI_API_H_

#include "SysHost.h"

VOID
UncoreEnableMeshMode (
  struct sysHost             *host
  );

#endif // _KTI_API_H_
