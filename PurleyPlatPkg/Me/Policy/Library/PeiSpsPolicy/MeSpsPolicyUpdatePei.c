/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MeSpsPolicyUpdatePei.c

  Do SPS Stage ME initialization.

**/
#if SPS_SUPPORT
#include <Guid/MeRcVariable.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SetupLib.h>
#include <Ppi/MeSpsPolicyPei.h>
#include <Library/DebugLib.h>
#include <Library/MeSpsPolicyAccessPeiLib.h>


/**

  Install the ME SPS Policy Library

  @param SpsPolicyPpi           Me SPS Policy Ppi

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
  @retval EFI_NOT_FOUND         Not all required services are loaded. Try later.

**/
EFI_STATUS
UpdateSpsPolicy (
  IN OUT  SPS_POLICY_PPI  *SpsPolicyPpi
  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  ME_RC_CONFIGURATION      MeConfiguration;
  
  //
  // Locate ME configuration variable
  //
  Status = GetSpecificConfigGuid(&gEfiMeRcVariableGuid, (VOID *)&MeConfiguration);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS Policy] ERROR: ME Variable not found (%r)\n", Status));
    return EFI_NOT_FOUND;
  }

  SpsPolicyPpi->SpsConfig.NmPwrOptBoot = MeConfiguration.NmPwrOptBoot;
  SpsPolicyPpi->SpsConfig.NmCores2Disable = MeConfiguration.NmCores2Disable;
  
#if ME_TESTMENU_FLAG
  SpsPolicyPpi->SpsConfig.MeTimeout = MeConfiguration.MeTimeout;
  
  SpsPolicyPpi->SpsConfig.MeHmrfpoLockEnabled = MeConfiguration.MeHmrfpoLockEnabled;
  SpsPolicyPpi->SpsConfig.MeHmrfpoEnableEnabled = MeConfiguration.MeHmrfpoEnableEnabled;
  
  SpsPolicyPpi->SpsConfig.MeGrLockEnabled = MeConfiguration.MeGrLockEnabled;
  SpsPolicyPpi->SpsConfig.MeGrPromotionEnabled = MeConfiguration.MeGrPromotionEnabled;
  SpsPolicyPpi->SpsConfig.BreakRtcEnabled = MeConfiguration.BreakRtcEnabled;
  
  SpsPolicyPpi->SpsConfig.SpsIccClkOverride = MeConfiguration.SpsIccClkOverride;
  SpsPolicyPpi->SpsConfig.SpsIccClkSscSetting = MeConfiguration.SpsIccClkSscSetting;
  
  SpsPolicyPpi->SpsConfig.NmPwrOptBootOverride = MeConfiguration.NmPwrOptBootOverride;
  SpsPolicyPpi->SpsConfig.NmCores2DisableOverride = MeConfiguration.NmCores2DisableOverride;
  
  SpsPolicyPpi->SpsConfig.NmPowerMsmtOverride = MeConfiguration.NmPowerMsmtOverride;
  SpsPolicyPpi->SpsConfig.NmPowerMsmtSupport = MeConfiguration.NmPowerMsmtSupport;
  SpsPolicyPpi->SpsConfig.NmHwChangeOverride = MeConfiguration.NmHwChangeOverride;
  SpsPolicyPpi->SpsConfig.NmHwChangeStatus = MeConfiguration.NmHwChangeStatus;
  SpsPolicyPpi->SpsConfig.NmPtuLoadOverride = MeConfiguration.NmPtuLoadOverride;
#endif
  return Status;
}
#endif // SPS_SUPPORT

