/** @file

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

#include "PeiMePolicyUpdate.h"
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/SetupLib.h>
#include <Ppi/HeciPpi.h>
#include <Library/HobLib.h>
#include <Library/MeTypeLib.h>
#include <Library/MeConfigLib.h>


/**
  Update the ME Policy Library

  @param[in] PeiMePolicyPpi     The pointer to MePolicyPpi

  @retval EFI_SUCCESS           Update complete.
**/
EFI_STATUS
UpdatePeiMePolicy (
  IN OUT   ME_POLICY_PPI          *PeiMePolicyPpi
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         MeIsCorporateAmt = FALSE;
  ME_RC_CONFIGURATION             MeSetup;
  
  Status = GetSpecificConfigGuid(&gEfiMeRcVariableGuid, (VOID*)&MeSetup);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[ME] ERROR: Cannot find ME RC Variable, policy update failed (%r)\n", Status));
    return Status;
  }
#if AMT_SUPPORT
  if (GetBootModeHob() == BOOT_ON_S3_RESUME)
  {
    MeIsCorporateAmt = MeSetup.MeIsCorporateAmt;
  }
  else
  {
    MeIsCorporateAmt = MeTypeIsCorporateAmt();
  }
#endif

#if ME_TESTMENU_FLAG

  PeiMePolicyPpi->MeConfig.DidInitStat = MeSetup.MeDidInitStat;
  PeiMePolicyPpi->MeConfig.SendDidMsg = MeSetup.MeDidEnabled;
  PeiMePolicyPpi->MeConfig.HeciCommunication1 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC1,
                                                                          MeSetup.MeHeci1Enabled,
                                                                          MeIsCorporateAmt);
  PeiMePolicyPpi->MeConfig.HeciCommunication2 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC2,
                                                                          MeSetup.MeHeci2Enabled,
                                                                          MeIsCorporateAmt);
  PeiMePolicyPpi->MeConfig.HeciCommunication3 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC3,
                                                                          MeSetup.MeHeci3Enabled,
                                                                          MeIsCorporateAmt);
  PeiMePolicyPpi->MeConfig.IderDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_IDER,
                                                                        MeSetup.MeIderEnabled,
                                                                        MeIsCorporateAmt);
  PeiMePolicyPpi->MeConfig.KtDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_SOL,
                                                                      MeSetup.MeKtEnabled,
                                                                      MeIsCorporateAmt);
  
  PeiMePolicyPpi->MeConfig.HostResetNotification = MeSetup.HostResetNotification;
  PeiMePolicyPpi->MeConfig.HsioMessaging = MeSetup.HsioMessagingEnabled;
  PeiMePolicyPpi->MeConfig.EndOfPostMessage = MeSetup.MeEndOfPostEnabled;
  
  PeiMePolicyPpi->MeConfig.DisableD0I3SettingForHeci = MeSetup.DisableD0I3SettingForHeci;
#else
  PeiMePolicyPpi->MeConfig.KtDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_SOL,
                                                                      ME_DEVICE_AUTO,
                                                                      MeIsCorporateAmt);
#endif // ME_TESTMENU_FLAG

  PeiMePolicyPpi->MeConfig.MeFwDownGrade = MeSetup.MeFwDowngrade;
  PeiMePolicyPpi->MeConfig.MeLocalFwUpdEnabled = MeSetup.LocalFwUpdEnabled;
  PeiMePolicyPpi->MeConfig.OsPtpAware = MeSetup.OsPtpAware;
  PeiMePolicyPpi->MeConfig.CoreBiosDoneEnabled = MeSetup.CoreBiosDoneEnabled;
  
  return EFI_SUCCESS;
}

