/** @file
  This file is PeiAmtPolicy library.

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
#include "PeiAmtPolicyLibrary.h"

//
// Function implementations
//
/**
  AmtCreatePolicyDefaults creates the default setting of AMT policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] AmtPolicyPpi              The pointer to get AMT Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
AmtCreatePolicyDefaults (
  OUT  AMT_POLICY_PPI          **AmtPolicyPpi
  )
{
  AMT_POLICY_PPI               *AmtPolicy;

  AmtPolicy = (AMT_POLICY_PPI *) AllocateZeroPool (sizeof (AMT_POLICY_PPI));
  if (AmtPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  ///
  /// Update the REVISION number
  ///
  AmtPolicy->Revision = AMT_POLICY_PPI_REVISION;

  ///
  /// Initialize the Platform Configuration
  ///
  AmtPolicy->AsfEnabled        = 1;
  AmtPolicy->FWProgress        = 1;

  *AmtPolicyPpi = AmtPolicy;
  return EFI_SUCCESS;
}

/**
  AmtInstallPolicyPpi installs AmtPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] AmtPolicyPpi               The pointer to AMT Policy PPI instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
AmtInstallPolicyPpi (
  IN  AMT_POLICY_PPI           *AmtPolicyPpi
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_PPI_DESCRIPTOR                *AmtPolicyPpiDesc;

  AmtPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (AmtPolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Initialize the PPI
  ///
  AmtPolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  AmtPolicyPpiDesc->Guid   = &gAmtPolicyPpiGuid;

  AmtPolicyPpiDesc->Ppi    = AmtPolicyPpi;


  ///
  /// Install the AMT PEI Policy PPI
  ///
  Status = PeiServicesInstallPpi (AmtPolicyPpiDesc);
  if (!EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "[ME] AMT Policy PPI Installed\n"));
  }
  ASSERT_EFI_ERROR (Status);

  return Status;
}
