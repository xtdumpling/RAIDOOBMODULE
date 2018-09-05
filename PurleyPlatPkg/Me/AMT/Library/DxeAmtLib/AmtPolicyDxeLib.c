/** @file
  Implementation file for AMT Policy functionality

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

#include "AmtDxeLibInternals.h"

//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED AMT_POLICY_PROTOCOL  *mDxeAmtPolicy = NULL;

/**
  Check if AMT is enabled in setup options.

  @retval EFI_SUCCESS             Amt Policy pointer is initialized.
  @retval All other error conditions encountered when no Amt Policy available.
**/
EFI_STATUS
AmtPolicyLibInit (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mDxeAmtPolicy != NULL) {
    return EFI_SUCCESS;
  }
  //
  // Get the desired platform setup policy.
  //
  Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, (VOID **) &mDxeAmtPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "No Amt Policy Protocol available"));
    Status = EFI_UNSUPPORTED;
  } else if (mDxeAmtPolicy == NULL) {
    DEBUG ((DEBUG_ERROR, "No Amt Policy Protocol available"));
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Check if Asf is enabled in setup options.

  @retval FALSE                   Asf is disabled.
  @retval TRUE                    Asf is enabled.
**/
BOOLEAN
AsfSupported (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if ASF support is enabled in Setup.
  //
  if (mDxeAmtPolicy->AmtConfig.AsfEnabled != 1) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check if Amt is enabled in setup options.

  @retval FALSE                   Amt is disabled.
  @retval TRUE                    Amt is enabled.
**/
BOOLEAN
AmtSupported (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if AMT support is enabled in Setup.
  //
  if (mDxeAmtPolicy->AmtConfig.AmtEnabled != 1) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check if AMT BIOS Extension hotkey was pressed during BIOS boot.

  @retval FALSE                   MEBx hotkey was not pressed.
  @retval TRUE                    MEBx hotkey was pressed.
**/
BOOLEAN
AmtHotkeyPressed (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if AMT Setup Prompt is enabled in Setup.
  //
  if (mDxeAmtPolicy->AmtConfig.AmtbxHotkeyPressed == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if AMT BIOS Extension Selection Screen is enabled in setup options.

  @retval FALSE                   AMT Selection Screen is disabled.
  @retval TRUE                    AMT Selection Screen is enabled.
**/
BOOLEAN
AmtSelectionScreen (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if AMT Selection Screen is enabled in Setup.
  //
  if (mDxeAmtPolicy->AmtConfig.AmtbxSelectionScreen == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if AMT WatchDog is enabled in setup options.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
AmtWatchDog (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "[ME Policy] ERROR: AmtWatchDog() = 0\n"));
      return FALSE;
    }
  }
  Supported = FALSE;
  if (ManageabilityModeSetting () != 0) {
    //
    // First check if AMT WatchDog is enabled in Setup.
    //
    if (AsfSupported ()) {
      if (mDxeAmtPolicy->AmtConfig.WatchDog == 1) {
        Supported = TRUE;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "[ME Policy] AmtWatchDog() = %d\n",
                      Supported));

  return Supported;
}

/**
  Return BIOS watchdog timer

  @retval UINT16                  BIOS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerBiosGet (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "[ME Policy] ERROR: AmtWatchDogTimerBiosGet() = 0\n"));
      return 0;
    }
  }
  DEBUG ((DEBUG_INFO, "[ME Policy] AmtWatchDogTimerBiosGet() = %d\n",
                      mDxeAmtPolicy->AmtConfig.WatchDogTimerBios));

  return mDxeAmtPolicy->AmtConfig.WatchDogTimerBios;
}

/**
  Return OS watchdog timer

  @retval UINT16                  OS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerOsGet (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "[ME Policy] ERROR: AmtWatchDogTimerOsGet() = 0\n"));
      return 0;
    }
  }
  DEBUG ((DEBUG_INFO, "[ME Policy] AmtWatchDogTimerOsGet() = %d\n",
                      mDxeAmtPolicy->AmtConfig.WatchDogTimerOs));

  return mDxeAmtPolicy->AmtConfig.WatchDogTimerOs;
}

/**
  Provide CIRA request information from OEM code.

  @retval Check if any CIRA requirement during POST
**/
BOOLEAN
AmtCiraRequestTrigger (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxeAmtPolicy->AmtConfig.CiraRequest == 1;
}

/**
  Provide CIRA request Timeout from OEM code.

  @retval CIRA require Timeout for MPS connection to be estabilished
**/
UINT8
AmtCiraRequestTimeout (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return (UINT8) (UINTN) mDxeAmtPolicy->AmtConfig.CiraTimeout;
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

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return (UINT8) (mDxeAmtPolicy->AmtConfig.ManageabilityMode);
}

/**
  Provide UnConfigure ME without password request from OEM code.

  @retval Check if unConfigure ME without password request
**/
BOOLEAN
AmtUnConfigureMe (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxeAmtPolicy->AmtConfig.UnConfigureMe == 1;
}

/**
  Provide 'Hiding the Unconfigure ME without password confirmation prompt' request from OEM code.

  @retval Check if 'Hide unConfigure ME without password Confirmation prompt' request
**/
BOOLEAN
AmtHideUnConfigureMeConfPrompt (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxeAmtPolicy->AmtConfig.HideUnConfigureMeConfirm == 1;
}

/**
  Provide show MEBx debug message request from OEM code.

  @retval Check show MEBx debug message request
 **/
BOOLEAN
AmtMebxDebugMsg (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxeAmtPolicy->AmtConfig.MebxDebugMsg == 1;
}

/**
  Provide on-board device list table and do not need to report them to AMT.  AMT only need to know removable PCI device
  information.

  @retval on-board device list table pointer other than system device.
**/
UINT32
AmtPciDeviceFilterOutTable (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return mDxeAmtPolicy->AmtConfig.PciDeviceFilterOutTable;
}

/**
  Check if USB provisioning enabled/disabled in Policy.

  @retval FALSE                   USB provisioning is disabled.
  @retval TRUE                    USB provisioning is enabled.
**/
BOOLEAN
USBProvisionSupport (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;

  //
  // First check if USB Provision is enabled in Setup.
  //
  if (mDxeAmtPolicy->AmtConfig.USBProvision == 1) {
    Supported = TRUE;
  }

  return Supported;
}

/**
  This will return progress event Option.
  True if the option is enabled.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
FwProgressSupport (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;

  //
  // First check if FW Progress is enabled in Setup.
  //
  if (mDxeAmtPolicy->AmtConfig.FWProgress == 1) {
    Supported = TRUE;
  }

  return Supported;
}

/**
  Check if ForcMebxSyncUp is enabled in setup options.

  @retval FALSE                   ForcMebxSyncUp is disabled.
  @retval TRUE                    ForcMebxSyncUp is enabled.
**/
BOOLEAN
AmtForcMebxSyncUp (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  if (mDxeAmtPolicy->AmtConfig.ForcMebxSyncUp == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**

  @brief
  Check if AMT WatchDogOs is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDogOs is disabled.
  @retval TRUE                    AMT WatchDogOs is enabled.

**/
BOOLEAN
AmtWatchDogOs (
  VOID
  )
{
  EFI_STATUS Status;
  BOOLEAN Supported;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "[ME Policy] ERROR: AmtWatchDogOs() = 0\n"));
      return FALSE;
    }
  }

  Supported = FALSE;
  if (ManageabilityModeSetting () != 0) {
    ///
    ///
    /// First check if AMT WatchDogOs is enabled in Setup.
    ///
    if (AsfSupported ()) {
      if (mDxeAmtPolicy->AmtConfig.WatchDogOs == 1) {
        Supported = TRUE;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "[ME Policy] AmtWatchDogOs() = %d\n",
                      Supported));
  return Supported;
}
