/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
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

@file:

  MeSpsPolicyInit.c

@brief:

  This file is SampleCode for Intel ME PEI SPS Policy initialization.

**/

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/MeSpsPolicyPei.h>
#include <Library/MeSpsPolicyUpdatePeiLib.h>


/**
  Configure Intel ME SPS default settings.

  @param[io] pSpsConfig   The pointer to SPS config structure

  @retval EFI_SUCCESS     The defaults initialized.
**/
EFI_STATUS EFIAPI
MeSpsConfigDefaults(
  OUT  ME_SPS_CONFIG  *pSpsConfig)
{
  pSpsConfig->Revision = ME_SPS_CONFIG_REVISION;
  pSpsConfig->SpsAltitude = ME_SPS_ALTITUDE_NONE;
  pSpsConfig->SpsMctpBusOwner = ME_SPS_MCTP_BUS_OWNER_NONE;
  
  pSpsConfig->PreDidMeResetEnabled = FALSE;
  
  pSpsConfig->Heci1HideInMe = ME_SPS_HECI_IN_ME_OFF;
  pSpsConfig->Heci2HideInMe = ME_SPS_HECI_IN_ME_OFF;
  pSpsConfig->Heci3HideInMe = ME_SPS_HECI_IN_ME_OFF;

  pSpsConfig->NmPwrOptBoot = 0;
  pSpsConfig->NmCores2Disable = 0;
  
#if ME_TESTMENU_FLAG
  pSpsConfig->MeTimeout = ME_SPS_INIT_TIMEOUT;
  
  pSpsConfig->MeHmrfpoLockEnabled = TRUE;
  pSpsConfig->MeHmrfpoEnableEnabled = FALSE;
  
  pSpsConfig->MeGrLockEnabled = TRUE;
  pSpsConfig->MeGrPromotionEnabled = FALSE;
  pSpsConfig->BreakRtcEnabled = FALSE;
  
  pSpsConfig->SpsIccClkOverride = FALSE;
  pSpsConfig->SpsIccClkSscSetting = 0;
  
  pSpsConfig->NmPwrOptBootOverride = FALSE;
  pSpsConfig->NmCores2DisableOverride = FALSE;
  
  pSpsConfig->NmPowerMsmtOverride = FALSE;
  pSpsConfig->NmPowerMsmtSupport = FALSE;
  pSpsConfig->NmHwChangeOverride = FALSE;
  pSpsConfig->NmHwChangeStatus = FALSE;
  pSpsConfig->NmPtuLoadOverride = FALSE;
#endif
  return EFI_SUCCESS;
}


/**

  @brief
  Initialize Intel ME PEI SPS Policy

  @param[in] PeiServices          Pei Services pointer

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.

**/
EFI_STATUS
SpsPolicyInitPei (
  IN CONST EFI_PEI_SERVICES   **PeiServices
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *SpsPolicyPpiDesc;
  SPS_POLICY_PPI              *SpsPolicyPpi;

  DEBUG ((DEBUG_INFO, "[SPS Policy] SPS Policy init\n"));
  ///
  /// Allocate descriptor and PPI structures
  ///
  SpsPolicyPpi = (SPS_POLICY_PPI *) AllocateZeroPool (sizeof (SPS_POLICY_PPI));
  if (SpsPolicyPpi == NULL) {
    ASSERT (SpsPolicyPpi != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  SpsPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (SpsPolicyPpiDesc == NULL) {
    ASSERT (SpsPolicyPpiDesc != NULL);
    FreePool(SpsPolicyPpi);
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Initialize the PPI
  ///
  SpsPolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SpsPolicyPpiDesc->Guid   = &gSpsPolicyPpiGuid;

  ///
  /// Update the REVISION number
  ///
  SpsPolicyPpi->Revision = SPS_POLICY_PPI_REVISION;
  MeSpsConfigDefaults(&SpsPolicyPpi->SpsConfig);
  
  ///
  /// Initialize the SPS Configuration
  ///
  UpdateSpsPolicy (SpsPolicyPpi);

  SpsPolicyPpiDesc->Ppi = SpsPolicyPpi;

  ///
  /// TBD put initialize codes in here if needs
  ///
  ///
  /// Install the ME PEI SPS Policy PPI
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, SpsPolicyPpiDesc);
  DEBUG((DEBUG_INFO, "[SPS Policy] SPS Policy PPI Installed (Status: %r)\n", Status));
  if (EFI_ERROR(Status))
  {
    ASSERT_EFI_ERROR(Status);
    FreePool(SpsPolicyPpi);
    FreePool(SpsPolicyPpiDesc);
  }
  
  return Status;
}
