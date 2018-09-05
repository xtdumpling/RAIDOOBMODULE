/** @file
  Implementation file for AMT Policy functionality for PEIM

@copyright
 Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
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

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>

#include <Library/PeiAmtLib.h>

/**
  Check if Amt is enabled in setup options.

  @param[in] AmtPolicyPpi The AMT Policy protocol instance

  @retval EFI_SUCCESS             AMT policy Ppi located
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
PeiAmtPolicyLibInit (
  IN AMT_POLICY_PPI      **AmtPolicyPpi
  )
{
  EFI_STATUS  Status;

  ///
  /// Locate system configuration variable
  ///
  Status = PeiServicesLocatePpi (
            &gAmtPolicyPpiGuid,             // GUID
            0,                              // INSTANCE
            NULL,                           // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) AmtPolicyPpi          // PPI
            );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Check if AMT WatchDog is enabled in setup options.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
PeiAmtWatchDog (
  VOID
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  AMT_POLICY_PPI                  *AmtPolicyPpi;

  Supported = FALSE;
  if (ManageabilityModeSetting () != 0) {
    Status = PeiAmtPolicyLibInit (&AmtPolicyPpi);
    ASSERT_EFI_ERROR (Status);
    //
    // First check if AMT WatchDog is enabled in Setup.
    //
    if (AmtPolicyPpi->WatchDog == 1) {
      Supported = TRUE;
    }
  }

  DEBUG ((DEBUG_INFO, "[ME Policy] PeiAmtWatchDog() = %d\n",
                      Supported));

  return Supported;
}

/**
  Get WatchDog BIOS Timmer.

  @retval UINT16                  WatchDog BIOS Timer
**/
UINT16
PeiAmtWatchTimerBiosGet (
  VOID
  )
{
  EFI_STATUS                  Status;
  AMT_POLICY_PPI              *AmtPolicyPpi;
  UINT16                      WatchDogTimerBios = 0;

  Status = PeiAmtPolicyLibInit (&AmtPolicyPpi);
  if (!EFI_ERROR (Status)) {
    WatchDogTimerBios = AmtPolicyPpi->WatchDogTimerBios;
  }

  DEBUG ((DEBUG_INFO, "[ME Policy] PeiAmtWatchTimerBiosGet() = %d\n",
                      WatchDogTimerBios));

  return WatchDogTimerBios;
}

/**
  Check if AMT is enabled in setup options.

  @retval FALSE                   ActiveManagement is disabled.
  @retval TRUE                    ActiveManagement is enabled.
**/
BOOLEAN
PeiAmtSupported (
  VOID
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  AMT_POLICY_PPI                  *AmtPolicyPpi;

  Status = PeiAmtPolicyLibInit (&AmtPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // First check if AMT support is enabled in Setup.
  //
  if (AmtPolicyPpi->AmtEnabled == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if ASF is enabled in setup options.

  @retval FALSE                   ASF is disabled.
  @retval TRUE                    ASF is enabled.
**/
BOOLEAN
PeiAsfSupported (
  VOID
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  AMT_POLICY_PPI                  *AmtPolicyPpi;

  Status = PeiAmtPolicyLibInit (&AmtPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // First check if ASF support is enabled in Setup.
  //
  if (AmtPolicyPpi->AsfEnabled == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF
**/
UINT8
ManageabilityModeSetting (
  VOID
  )
{
  EFI_STATUS                      Status;
  AMT_POLICY_PPI                  *AmtPolicyPpi;

  Status = PeiAmtPolicyLibInit (&AmtPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return (UINT8) (AmtPolicyPpi->ManageabilityMode);
}

/**
  This will return progress event Option.
  True if the option is enabled.

  @param[in] VOID

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
PeiFwProgressSupport (
  VOID
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  AMT_POLICY_PPI                  *AmtPolicyPpi;

  Status = PeiAmtPolicyLibInit (&AmtPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  Supported = FALSE;

  //
  // Check if progress event is enabled in Setup.
  //
  if (AmtPolicyPpi->FWProgress == 1) {
    Supported = TRUE;
  }

  return Supported;
}

/**
  Provide Amt Sol setting from MEBx BIOS Sync Data

  @retval TRUE                    Amt Sol is enabled.
  @retval FALSE                   Amt Sol is disabled.
**/
BOOLEAN
AmtSolSetting (
  VOID
  )
{
  EFI_STATUS                      Status;
  AMT_POLICY_PPI                  *AmtPolicyPpi;

  Status = PeiAmtPolicyLibInit (&AmtPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Referring AmtSol state from Mebx by previous POST boot status. The AmtSol status was retrieved via Mebx
  /// and saved in DXE phase by ME_BIOS_EXTENSION_SETUP variable, then reflect the next boot path via AmtPolicyPpi.
  /// Platform should ensure the state is updated depended on current ME_BIOS_EXTENSION_SETUP.AmtSol status.
  ///

  return (UINT8) (AmtPolicyPpi->AmtSolEnabled);
}

