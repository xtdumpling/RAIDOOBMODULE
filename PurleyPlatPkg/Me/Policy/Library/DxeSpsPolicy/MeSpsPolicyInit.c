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
  This file is SampleCode for Intel ME DXE Sps Policy initialization.


**/
#include <Library/MmPciBaseLib.h>
#include "Library/MeSpsPolicyInitDxeLib.h"
#include "Protocol/HeciProtocol.h"
#include <Protocol/MeSpsPolicyProtocol.h>
#include "MeState.h"
#include "Library/MeTypeLib.h"
#include <Library/MeSpsPolicyUpdateDxeLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MeSpsPolicyDebugDump.h>
#include <Library/IoLib.h>

#include "PchAccess.h"
#include "HeciRegs.h"
#include <Library/DxeMePolicyUpdateLib.h>


/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
  Configure Intel ME SPS default settings.

  @param[io] pSpsConfig   The pointer to SPS config structure

  @retval EFI_SUCCESS     The defaults initialized.
**/
EFI_STATUS
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


#if AMT_SUPPORT
EFI_STATUS
MeAmtConfigDefaults(
  OUT  ME_AMT_CONFIG  *pAmtConfig)
{
  ///
  /// Misc. Config
  ///
  /// FviSmbiosType is the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS Type 14 - Group
  /// Associations structure - item type. FVI structure uses it as SMBIOS OEM type to provide
  /// version information. The default value is type 221.
  ///
  pAmtConfig->FviSmbiosType = 0xDD;
  pAmtConfig->PreviousMeStateControl = 0;
  pAmtConfig->PreviousLocalFwUpdEnabled = 0;
  return EFI_SUCCESS;
}
#endif // AMT_SUPPORT


/**

  @brief
  Initialize Intel ME DXE Sps Policy

  @param[in] SpsPolicy            SPS Policy Protocol

  @retval EFI_SUCCESS             Initialization complete.
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
SpsPolicyInit (
  IN     EFI_HANDLE           ImageHandle,
  IN OUT SPS_POLICY_PROTOCOL *SpsPolicy)
{
  EFI_STATUS              Status;

  DEBUG ((DEBUG_INFO, "[SPS Policy] ME Sps Policy (Dxe)\n", SpsPolicy));

  if (SpsPolicy == NULL) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] ME Sps Policy (Dxe) wrong input parameters\n"));
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// ME DXE Policy Init
  ///
  SpsPolicy->Revision = SPS_POLICY_PROTOCOL_REVISION;
  SpsPolicy->MeType = MeTypeGet();
#if SPS_SUPPORT
  MeSpsConfigDefaults(&SpsPolicy->SpsConfig);
#endif
#if AMT_SUPPORT
  MeAmtConfigDefaults(&SpsPolicy->MeAmtConfig);
#endif
  SpsPolicy->MeReportError = ShowMeReportError;

#if SPS_SUPPORT
  if (MeTypeIsSps()) {
    UINTN   PchSpiBase;
    ///
    /// Thermal reporting policy is based on strap settings
    ///
#define R_PCH_SPI_STRP15              0x3C

    PchSpiBase = MmioRead32 (MmPciBase (
                             DEFAULT_PCI_BUS_NUMBER_PCH,
                             PCI_DEVICE_NUMBER_PCH_SPI,
                             PCI_FUNCTION_NUMBER_PCH_SPI)
                             + R_PCH_SPI_BAR0) & ~(B_PCH_SPI_BAR0_MASK);
    MmioAndThenOr32 (
      PchSpiBase + R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
      (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP15)
      );
  }
#endif // SPS_SUPPORT

  SpsPolicy->SaveSpsPolicyToSetupConfiguration = SaveSpsPolicyToSetupConfiguration;
  SpsPolicy->UpdateSpsPolicyFromSetup = UpdateSpsPolicyFromSetup;
  SpsPolicy->MePolicyDump = MePolicyDump;

  InstallSpsPolicyLibrary (SpsPolicy);

  ///
  /// Install protocol to to allow access to this Policy.
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gSpsPolicyProtocolGuid,
                  SpsPolicy,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Save configuration changes to setup
  ///
  SaveSpsPolicyToSetupConfiguration(SpsPolicy);

  return Status;
}
