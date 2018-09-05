/** @file

@copyright
 Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _DXE_AMT_POLICY_UPDATE_H_
#define _DXE_AMT_POLICY_UPDATE_H_

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/AmtPolicy.h>

BOOLEAN
EnableRedirection (
  BOOLEAN Enable
  )
/*++
Routine Description:
  This will return Serial-over-Lan Boot Option.
  True if the option is enabled/disabled.
  False if enable/disable failed.

Arguments:
  None.

Returns:
  True    Redirection is enabled/disabled.
  False   Enable/Disable Failed.  
--*/
;

#endif
