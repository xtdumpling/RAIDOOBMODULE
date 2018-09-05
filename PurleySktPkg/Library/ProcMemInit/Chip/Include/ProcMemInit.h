/**
//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file ProcMemInit.h

  Memory Initialization Module.

**/

#ifndef _PROCMEM_INIT_H_
#define _PROCMEM_INIT_H_

#include "SysFunc.h"
#include "MemApiSkx.h"
#include "KtiSetupDefinitions.h"


VOID
KtiInit (
  struct sysHost             *host
  );

VOID
CpuInit (
  struct sysHost             *host
  );

VOID
ProcMemInit (
  struct sysHost             *host
  );

VOID
SetBIOSInitDone (
  struct sysHost             *host
  );

VOID
CpuInitMISC (
  struct sysHost             *host
  );

VOID
SetRstCpl (
  struct sysHost             *host,
  UINT8          socket
  );

VOID
ClearDmictrlAutoPmCplt (
  struct sysHost             *host,
    UINT8          socket
  );


#endif // _PROCMEM_INIT_H_
