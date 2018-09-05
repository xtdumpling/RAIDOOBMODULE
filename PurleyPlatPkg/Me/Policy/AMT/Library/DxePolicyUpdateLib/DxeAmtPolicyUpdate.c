/** @file

@copyright
 Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
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

#include "DxeAmtPolicyUpdate.h"
#include <Library/SetupLib.h>
#include <Library/MeTypeLib.h>

/**
  Install DxeAmtPolicy with setup values

  @param[in, out] DxeAmtPolicy  DXE Amt Policy

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
UpdateDxeAmtPolicy (
  IN OUT  AMT_POLICY_PROTOCOL   *DxeAmtPolicy
  )
{
#if AMT_SUPPORT  // SERVER_BIOS_FLAG
  EFI_STATUS                Status;
  ME_RC_CONFIGURATION       MeSetup;
  UINT8                     Index;

  Index     = 0;

  DEBUG ((DEBUG_INFO, "Enter UpdateDxeAmtPolicy\n"));

  Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeSetup);

  if (!EFI_ERROR (Status)) {
    DxeAmtPolicy->AmtConfig.AsfEnabled                = MeSetup.Asf;
    DxeAmtPolicy->AmtConfig.AmtEnabled                = MeSetup.Amt;
    DxeAmtPolicy->AmtConfig.WatchDog                  = MeSetup.WatchDog;
    DxeAmtPolicy->AmtConfig.WatchDogOs                = MeSetup.WatchDogOs;
    DxeAmtPolicy->AmtConfig.WatchDogTimerOs           = MeSetup.WatchDogTimerOs;
    DxeAmtPolicy->AmtConfig.WatchDogTimerBios         = MeSetup.WatchDogTimerBios;
    DxeAmtPolicy->AmtConfig.CiraRequest               = MeSetup.AmtCiraRequest;
    DxeAmtPolicy->AmtConfig.CiraTimeout               = MeSetup.AmtCiraTimeout;
    DxeAmtPolicy->AmtConfig.UnConfigureMe             = MeSetup.UnConfigureMe;
    DxeAmtPolicy->AmtConfig.HideUnConfigureMeConfirm  = MeSetup.HideUnConfigureMeConfirm;
    DxeAmtPolicy->AmtConfig.MebxDebugMsg              = MeSetup.MebxDebugMsg;
    DxeAmtPolicy->AmtConfig.USBProvision              = MeSetup.USBProvision;
    DxeAmtPolicy->AmtConfig.FWProgress                = MeSetup.FWProgress;
    DxeAmtPolicy->AmtConfig.AmtbxSelectionScreen      = MeSetup.AmtbxSelectionScreen;
    DxeAmtPolicy->AmtConfig.AmtbxHotkeyPressed        = MeSetup.AmtbxHotKeyPressed;
    DxeAmtPolicy->AmtConfig.MebxNonUiTextMode         = MeSetup.MebxNonUiTextMode;
    DxeAmtPolicy->AmtConfig.MebxUiTextMode            = MeSetup.MebxUiTextMode;
    DxeAmtPolicy->AmtConfig.MebxGraphicsMode          = MeSetup.MebxGraphicsMode;

  }
  return Status;
#else
  return EFI_SUCCESS;
#endif // AMT_SUPPORT
}

#ifdef AMT_SUPPORT
/**
  SaveMeTypeInSetup updates MeIsCorporateAmt setup var 
  to store information what kind of AMT is present at the platform.

  @retval EFI_SUCCESS on success.
  @retval EFI_ERROR otherwise

**/
EFI_STATUS
EFIAPI
SaveMeTypeInSetup(VOID)
{
  ME_RC_CONFIGURATION       MeSetup;
  EFI_STATUS                Status;
  UINT8                     MeIsCorporateAmt;

  Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeSetup);
  if( EFI_ERROR(Status) ){
    return Status;
  }

  MeIsCorporateAmt = MeTypeIsCorporateAmt();

  if(MeSetup.MeIsCorporateAmt != MeIsCorporateAmt){
    MeSetup.MeIsCorporateAmt = MeIsCorporateAmt;
    Status = SetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeSetup);
  }

  return Status;
}
#endif // AMT_SUPPORT
