/** @file
  Do Platform Stage AMT initialization.

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

#include "PeiAmtPolicyUpdate.h"

#ifdef AMT_SUPPORT
#include <AmtForcePushPetPolicy.h>
#include <MeBiosExtensionSetup.h>
#include <AlertStandardFormat.h>
#include <Library/SetupLib.h>
#include "Library/MeTypeLib.h"

GLOBAL_REMOVE_IF_UNREFERENCED ASF_FRAMEWORK_MESSAGE_TYPE mAmtForcePushPetPolicy[] = {
  AsfMessageChassisIntrusion,
  AsfMessageUserAuthenticationInvalid,
  AsfMessageHddAuthenticationInvalid,
};

/**
  Install the Active Management push PET policy
  
  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
InstallAmtForcePushPetPolicy (
  VOID
  ) 
{
  VOID        *Hob;

  //
  // Build HOB for setup memory information
  //
  Hob = BuildGuidDataHob (
            &gAmtForcePushPetPolicyGuid,
            &mAmtForcePushPetPolicy,
            sizeof (mAmtForcePushPetPolicy)
            );

  //
  // Cannot Build HOB
  //
  ASSERT (Hob != NULL);

  return EFI_SUCCESS;
}

/**
  Install the Active Management Policy Ppi Library
  
  @param[in, out] AmtPolicyPpi PEI Amt Policy
 
  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
UpdatePeiAmtPolicy (
  IN OUT  AMT_POLICY_PPI *AmtPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  ME_RC_CONFIGURATION             MeSetup;
  ME_BIOS_EXTENSION_SETUP         MeBiosExtensionSetupData;

  Status = EFI_SUCCESS;

  Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeSetup);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[AMT Policy] Error: System configuration variable cannot be located\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Locate Variable Ppi
  //
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, &VariableServices);

  //
  // Make sure we have a PPI, if not, just return.
  //
  if (!VariableServices) {
    return EFI_SUCCESS;
  }

  if (Status == EFI_SUCCESS) {
    AmtPolicyPpi->AmtEnabled         = MeSetup.Amt;
    AmtPolicyPpi->WatchDog           = MeSetup.WatchDog;
    AmtPolicyPpi->WatchDogTimerBios  = MeSetup.WatchDogTimerBios;
    AmtPolicyPpi->WatchDogTimerOs    = MeSetup.WatchDogTimerOs;
    AmtPolicyPpi->AsfEnabled         = MeSetup.Asf;
    AmtPolicyPpi->FWProgress         = MeSetup.FWProgress;
  }

  //
  // Get BIOS Sync-up data from MEBx to Amt Policy
  //
  VariableSize = sizeof (MeBiosExtensionSetupData);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
                               &gMeBiosExtensionSetupGuid,
                               NULL,
                               &VariableSize,
                               &MeBiosExtensionSetupData
                               );
  if (Status != EFI_SUCCESS) {
    AmtPolicyPpi->ManageabilityMode = 0;
    AmtPolicyPpi->AmtSolEnabled = 0;
    if (!MeTypeIsAmt())
    {
      Status = EFI_SUCCESS;
    }
    else
    {
      DEBUG((DEBUG_ERROR, "[AMT Policy] ERROR: Cannot locate MEBx configuration (%r)\n", Status));
    }
  } else {
    AmtPolicyPpi->ManageabilityMode = MeBiosExtensionSetupData.PlatformMngSel;
    DEBUG ((DEBUG_INFO, "[AMT Policy] Mebx set ManageabilityMode to %x at previous boot\n", MeBiosExtensionSetupData.PlatformMngSel));
    AmtPolicyPpi->AmtSolEnabled = MeBiosExtensionSetupData.AmtSol;
    DEBUG ((DEBUG_INFO, "[AMT Policy] Mebx set ManageabilityMode to %x at previous boot\n", MeBiosExtensionSetupData.PlatformMngSel));
  }

  InstallAmtForcePushPetPolicy ();

  return Status;
}
#endif
