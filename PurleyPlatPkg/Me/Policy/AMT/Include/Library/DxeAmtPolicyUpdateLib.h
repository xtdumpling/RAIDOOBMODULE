/** @file

@copyright
 Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
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
#ifndef _DXE_AMT_POLICY_UPDATE_LIB_H_
#define _DXE_AMT_POLICY_UPDATE_LIB_H_

#include <Protocol/AmtPolicy.h>

#if AMT_SUPPORT  // SERVER_BIOS_FLAG
EFI_STATUS
EFIAPI
UpdateDxeAmtPolicy (
  IN OUT  AMT_POLICY_PROTOCOL   *DxeAmtPolicy
)
/*++
 
Routine Description:
 
  Update DxeAmtPolicy with setup values
  
Arguments:

  DxeAmtPolicy          DXE Amt Policy
 
Returns:
 
  EFI_SUCCESS           Initialization complete.
  EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  EFI_DEVICE_ERROR      Device error, driver exits abnormally.
 
--*/
;
EFI_STATUS
EFIAPI
SaveMeTypeInSetup(VOID)
/**++

Routine Description:

  SaveMeTypeInSetup updates MeIsCorporateAmt setup var 
  to store information about kind of AMT present at the platform.

Returns:

  @retval EFI_SUCCESS on success.

--*/
;

#endif // AMT_SUPPORT

#endif
