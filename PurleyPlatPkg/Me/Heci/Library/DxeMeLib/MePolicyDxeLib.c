/** @file
  Implementation file for Me Policy functionality

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

#include "MeDxeLibInternals.h"
#include "Library/MeTypeLib.h"
#include <Library/SetupLib.h>
#include <Guid/MeRcVariable.h>

//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED ME_POLICY_PROTOCOL            *mDxeMePolicy = NULL;

/**
  Check if Me is enabled.

  @retval EFI_SUCCESS             ME Policy pointer is initialized.
  @retval All other error conditions encountered when no ME Policy available.
**/
EFI_STATUS
MePolicyLibInit (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeMePolicy != NULL) {
    return EFI_SUCCESS;
  }
  //
  // Get the desired platform setup policy.
  //
  Status = gBS->LocateProtocol (&gDxeMePolicyGuid, NULL, (VOID **) &mDxeMePolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "No ME Policy Protocol available"));
    ASSERT_EFI_ERROR (Status);
  } else if (mDxeMePolicy == NULL) {
    DEBUG ((DEBUG_ERROR, "No ME Policy Protocol available"));
    Status = EFI_UNSUPPORTED;
  }
  return Status;
}

/**
  Check if End of Post Message is enabled in setup options.

  @retval FALSE                   EndOfPost is disabled.
  @retval TRUE                    EndOfPost is enabled.
**/
BOOLEAN
MeEndOfPostEnabled (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeMePolicy == NULL) {
    Status = MePolicyLibInit ();
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }
  
  if (mDxeMePolicy->MeConfig.EndOfPostMessage != EOP_SEND_IN_DXE) {
    Supported = FALSE;
  } else {
    Supported = TRUE;
  }

  return Supported;
}

/**
  Checks if EndOfPost event already happened


  @retval TRUE                    if end of post happened
  @retval FALSE                   if not yet
**/
BOOLEAN
MeIsAfterEndOfPost (
  VOID
  )
{
  EFI_STATUS          Status;

  if (mDxeMePolicy == NULL) {
    Status = MePolicyLibInit ();
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }
  return (BOOLEAN)mDxeMePolicy->EndOfPostDone;
}

/**

  Sets EndOfPostDone value.

  @param[in]
  @retval TRUE    EndOfPostDone has been set
  @retval FALSE   Cannot set EndOfPostDone

**/
BOOLEAN
SetEndOfPostDone (
  VOID
  )
{
  EFI_STATUS          Status;
  ME_POLICY_PROTOCOL *MePolicy;

  Status = gBS->LocateProtocol (&gDxeMePolicyGuid, NULL, &MePolicy);
  if (!EFI_ERROR(Status)) {
    MePolicy->EndOfPostDone = TRUE;
    return TRUE;
  }

  return FALSE;
}

/**
  Show Me Error message. This is used to support display error message on the screen for localization
  description

  @param[in] MsgId                Me error message ID for displaying on screen message

**/
VOID
MeReportError (
  IN ME_ERROR_MSG_ID              MsgId
  )
{
  EFI_STATUS                      Status;

  if (mDxeMePolicy == NULL) {
    Status = MePolicyLibInit ();
    if (EFI_ERROR (Status)) {
      return;
    }
  }
  if (mDxeMePolicy->MeReportError != NULL) {
    mDxeMePolicy->MeReportError (MsgId);
  }

  return ;
}
// APTIOV_SERVER_OVERRIDE_RC_START : To resolve build error in AMT_SUPPORT disable case
#if defined(AMT_SUPPORT) && AMT_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : To resolve build error in AMT_SUPPORT disable case
/**
  Check if MeFwDowngrade is enabled in setup options.

  @retval FALSE                   MeFwDowngrade is disabled.
  @retval TRUE                    MeFwDowngrade is enabled.
**/
BOOLEAN
MeFwDowngradeSupported (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxeMePolicy == NULL) {
    Status = MePolicyLibInit ();
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }
  if (mDxeMePolicy->MeConfig.MeFwDownGrade == 1) {
#if AMT_SUPPORT
    //
    // FW Downgrade BIOS Setup Option is only valid for one boot
    //
    ME_RC_CONFIGURATION   MeRcConfig;

    mDxeMePolicy->MeConfig.MeFwDownGrade = 0;
    Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeRcConfig);
    if (!EFI_ERROR(Status)) {
      MeRcConfig.MeFwDowngrade = 0;
      SetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeRcConfig);
    }
#else
    return FALSE;
#endif // AMT_SUPPORT
    return TRUE;
  }

  return FALSE;
}
// APTIOV_SERVER_OVERRIDE_RC_START : To resolve build error in AMT_SUPPORT disable case
#endif // AMT_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : To resolve build error in AMT_SUPPORT disable case
/**
  Check if Core Bios Done Message is enabled in setup options.

  @retval FALSE                   Core Bios Done is disabled.
  @retval TRUE                    Core Bios Done is enabled.
**/

BOOLEAN
CoreBiosDoneEnabled (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxeMePolicy == NULL) {
    Status = MePolicyLibInit ();
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;
  if (mDxeMePolicy->MeConfig.CoreBiosDoneEnabled == 1) {
    Supported = TRUE;
  }

  return Supported;
}

