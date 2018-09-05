/** @file
  This file is PeiMePolicy library.

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
#include "PeiMePolicyLibrary.h"
#include <Library/MeConfigLib.h>


/*****************************************************************************
 * Function implementations
 *****************************************************************************/
/**
  MeCreatePolicyDefaults creates the default setting of ME Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] MePolicyPpi       The pointer to get ME Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
MeCreatePolicyDefaults (
  OUT  ME_POLICY_PPI           **MePolicyPpi
  )
{
  ME_POLICY_PPI                *MePolicy;

  MePolicy = (ME_POLICY_PPI *) AllocateZeroPool (sizeof (ME_POLICY_PPI));
  if (MePolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  ///
  /// Update the REVISION number
  ///
  MePolicy->Revision = ME_POLICY_PPI_REVISION;
  
  MeConfigDefaults(&MePolicy->MeConfig);
  
  *MePolicyPpi = MePolicy;
  return EFI_SUCCESS;
}

/**
  MeInstallPolicyPpi installs MePolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] MePolicyPpi        The pointer to ME Policy PPI instance

  @retval EFI_SUCCESS           The policy is installed.
  @retval EFI_OUT_OF_RESOURCES  Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
MeInstallPolicyPpi (
  IN  ME_POLICY_PPI            *MePolicyPpi
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_PPI_DESCRIPTOR                *MePolicyPpiDesc;

  MePolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (MePolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Initialize the PPI
  ///
  MePolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  MePolicyPpiDesc->Guid   = &gMePolicyPpiGuid;

  MePolicyPpiDesc->Ppi    = MePolicyPpi;


  ///
  /// Install the ME PEI Policy PPI
  ///
  Status = PeiServicesInstallPpi (MePolicyPpiDesc);
  if (!EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "[ME] ME Policy PPI Installed\n"));
  }
  ASSERT_EFI_ERROR (Status);

  return Status;
}
