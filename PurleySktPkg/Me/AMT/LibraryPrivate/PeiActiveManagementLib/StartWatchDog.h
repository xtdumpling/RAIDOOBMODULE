/** @file
  StartWatchDog header file

@copyright
 Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _START_WATCH_DOG_H_
#define _START_WATCH_DOG_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>

#include <Library/PeiAmtLib.h>
#include <Library/PeiMeLib.h>
#include <Ppi/HeciPpi.h>

//
// Function Prototypes
//

/**
  Perform the platform spefific initializations.

  @retval EFI_SUCCESS             if the interface could be successfully installed.
**/
EFI_STATUS
PeiInitStartWatchDog (
  VOID
  )
;

#endif // _START_WATCH_DOG_H_
