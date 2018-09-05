/** @file
  Prototype of the DxeMePolicyLib library.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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
#ifndef _DXE_ME_POLICY_LIB_H_
#define _DXE_ME_POLICY_LIB_H_

#include <Protocol/MePolicy.h>

/**
  This function prints the ME DXE phase policy.

  @param[in] DxeMePolicy - ME DXE Policy protocol
**/
VOID
MePrintPolicyProtocol (
  IN  ME_POLICY_PROTOCOL             *DxeMePolicy
  )
;

/**
  MeCreatePolicyDefaults creates the default setting of ME Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] DxeMePolicy               The pointer to get ME Policy protocol instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
MeCreatePolicyDefaults (
  IN OUT  ME_POLICY_PROTOCOL            *DxeMePolicy
  )
;

/**
  MeInstallPolicyProtocol installs ME Policy.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] ImageHandle                Image handle of this driver.
  @param[in] DxeMePolicy                The pointer to ME Policy Protocol instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
MeInstallPolicyProtocol (
  IN  EFI_HANDLE                    ImageHandle,
  IN  ME_POLICY_PROTOCOL            *DxeMePolicy
  )
;

#endif // _DXE_ME_POLICY_LIB_H_
