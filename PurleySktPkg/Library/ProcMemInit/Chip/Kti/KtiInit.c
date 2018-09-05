/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file KtiInit.c

  Reference Code Module.

**/
#include "ProcMemInit.h"
#include "KtiLib.h"

/**

  Entry point for KTI initialization

  @param host  - Pointer to the system host (root) structure


  @retval N/A

**/
VOID
KtiInit (
  struct sysHost             *host
  )
{
  UINT32  KtiStatus;

  OemPreProcInit (host);

  //
  // Execute KTI initialization
  //
  KtiStatus = KtiMain (host);

  OemPostProcInit (host, KtiStatus);

}
