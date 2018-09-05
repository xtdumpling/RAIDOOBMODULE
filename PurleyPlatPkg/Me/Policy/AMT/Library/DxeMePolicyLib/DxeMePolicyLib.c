/** @file
  This file is DxeMePolicyLib library.

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
#include "DxeMePolicyLibrary.h"
#include <Library/MeConfigLib.h>


/**
  This function prints the ME DXE phase policy.

  @param[in] DxeMePolicy - ME DXE Policy protocol
**/
VOID
EFIAPI
MePrintPolicyProtocol (
  IN  ME_POLICY_PROTOCOL            *DxeMePolicy
  )
{
DEBUG_CODE_BEGIN();
  DEBUG((DEBUG_INFO, "\n------------------------ ME Policy Begin -----------------------\n"));
  DEBUG((DEBUG_INFO, "ME Policy Revision       : %d\n", DxeMePolicy->Revision));
  ASSERT(DxeMePolicy->Revision == ME_POLICY_PROTOCOL_REVISION);
  
  MeConfigPrint(&DxeMePolicy->MeConfig);
  DEBUG((DEBUG_INFO, "EndOfPostDone            : %d\n", DxeMePolicy->EndOfPostDone));
  DEBUG((DEBUG_INFO, "\n------------------------ ME Policy End --------------------------\n"));
DEBUG_CODE_END();
}


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
{
  ///
  /// ME DXE Policy Init
  ///
  DxeMePolicy->Revision = ME_POLICY_PROTOCOL_REVISION;

  ///
  /// Initialzie the ME Configuration
  ///
  MeConfigDefaults(&DxeMePolicy->MeConfig);

  ///
  /// Please don't change the default value of EndOfPostDone and
  /// the value will be update to 1 when EOP has been sent to ME FW successfully
  ///
  DxeMePolicy->EndOfPostDone = 0;
  DxeMePolicy->MeReportError = NULL;

  return EFI_SUCCESS;
}

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
{
  EFI_STATUS            Status;

  ///
  /// Print ME DXE Policy
  ///

  ///
  /// Install protocol to to allow access to this Policy.
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gDxeMePolicyGuid,
                  DxeMePolicy,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

