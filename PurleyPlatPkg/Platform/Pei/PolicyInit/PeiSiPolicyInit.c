/** @file
  This file is SampleCode for Intel Silicon PEI Policy initialzation.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#include "PeiSiPolicyInit.h"

/**
  This function performs Silicon Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
PeiSiPolicyInit (
  IN UINT8               FirmwareConfiguration
  )
{
  EFI_STATUS             Status;
  SI_POLICY_PPI          *SiPolicyPpi;

  //
  // Call SiCreatePolicyDefaults to initialize Silicon Policy structure
  // and get all Intel default policy settings.
  //
  Status = SiCreatePolicyDefaults (&SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);
  //
  // Update and override all platform related and customized settings below.
  //
  UpdatePeiSiPolicy (SiPolicyPpi);
  if ((FirmwareConfiguration >= FwConfigDebug) && (FirmwareConfiguration < FwConfigMax)) {
    // Update Debug Policies
  }
  //
  // Install SiPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiInstallPolicyPpi (SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

