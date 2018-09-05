/** @file
  This file is SampleCode for Intel PEI Platform Policy initialzation.

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
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

#include "PeiPolicyInit.h"

//
// Function implementations
//
/**
  Initilize Intel PEI Platform Policy

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.
**/
VOID
EFIAPI
PeiPolicyInitPreMem (
  IN UINT8                     FirmwareConfiguration,
  IN SYSTEM_CONFIGURATION      *SystemConfiguration,
  IN PCH_RC_CONFIGURATION      *PchRcConfiguration
  )
{
  EFI_STATUS                   Status;

  //
  // PCH PEI Policy Initialization
  //
  Status = PeiPchPolicyInit (FirmwareConfiguration, SystemConfiguration, PchRcConfiguration);
  DEBUG ((DEBUG_INFO, "PCH PEI Policy Initialization Done \n"));
  ASSERT_EFI_ERROR (Status);

}
