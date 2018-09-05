/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MeSpsPolicyInitDxeLib.h

@brief:

  Header file for the MeSpsPolicyInitDxe Driver.

**/
#ifndef _DXE_ME_SPS_POLICY_INIT_LIB_H_
#define _DXE_ME_SPS_POLICY_INIT_LIB_H_

#include <Protocol/MeSpsPolicyProtocol.h>

/**

  @brief
  Initialize Intel ME DXE Sps Policy

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] DxeSpsPolicy         Sps Policy

  @retval EFI_SUCCESS             Initialization complete.
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
SpsPolicyInit (
  IN EFI_HANDLE          ImageHandle,
  SPS_POLICY_PROTOCOL   *SpsPolicy
  )
;


#endif // _DXE_ME_SPS_POLICY_INIT_LIB_H_
