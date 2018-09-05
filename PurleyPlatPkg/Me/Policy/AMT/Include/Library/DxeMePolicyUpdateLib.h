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

#ifndef _DXE_ME_POLICY_UPDATE_LIB_H_
#define _DXE_ME_POLICY_UPDATE_LIB_H_

/**
  Update the ME Policy Library

  @param[in] DxeMePolicy                The pointer to get ME Policy protocol instance

  @retval EFI_SUCCESS                   Initialization complete.
  @retval EFI_UNSUPPORTED               The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES          Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR              Device error, driver exits abnormally.

**/
EFI_STATUS
UpdateDxeMePolicy (
  IN OUT  ME_POLICY_PROTOCOL      *DxeMePolicy
  )
;

/**
  This is to display localized message in the console. This is
  used to display message strings in local language. To display
  the message, the routine will check the message ID and ConOut
  the message strings.

  @param[in] MsgId                Me error message ID for displaying on screen message

**/
VOID
EFIAPI
ShowMeReportError (
  IN ME_ERROR_MSG_ID              MsgId
  )
;

#endif // _DXE_ME_POLICY_UPDATE_LIB_H_