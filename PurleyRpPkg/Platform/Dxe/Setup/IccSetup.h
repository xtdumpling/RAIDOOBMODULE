/**@file

@copyright
 Copyright (c) 2009 - 2015 Intel Corporation. All rights reserved
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

#ifndef __ICC_SETUP_H__
#define __ICC_SETUP_H__

VOID
IccExtractConfigOnSetupEnter (
  ME_RC_CONFIGURATION *MeRcConfiguration
);

VOID
IccRouteConfig (
  ME_RC_CONFIGURATION *MeRcConfiguration
);

EFI_STATUS
IccCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION Action,
  IN EFI_QUESTION_ID KeyValue,
  IN UINT8 Type,
  IN EFI_IFR_TYPE_VALUE *Value,
  OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
);

EFI_STATUS
EFIAPI
IccProfileCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION Action,
  IN EFI_QUESTION_ID KeyValue,
  IN UINT8 Type,
  IN EFI_IFR_TYPE_VALUE *Value,
  OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
);

VOID
InitIccStrings (
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class
)
;
#endif
