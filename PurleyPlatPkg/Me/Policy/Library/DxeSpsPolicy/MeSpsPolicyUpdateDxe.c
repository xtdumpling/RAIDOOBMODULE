/**

This file contains a 'Sample Driver' and is licensed as such
under the terms of your license agreement with Intel or your
vendor.  This file may be modified by the user, subject to
the additional terms of the license agreement

**/

/**
Copyright (c)  2008 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file MeSpsPolicyUpdateDxe.c

--*/

#include "Protocol/FirmwareVolume.h"
#include "FrameworkDxe.h"
#include "Library/DebugLib.h"
#include "Guid/HiiSetupEnter.h"
#include "Framework/DxeCis.h"
#include "CommonIncludes.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Guid/MeRcVariable.h"
#include "Library/UefiRuntimeServicesTableLib.h"
#include "Library/MeSpsPolicyUpdateDxeLib.h"
#include "MeState.h"
#include "Protocol/HeciProtocol.h"
#include "Library/HeciMsgLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/SetupLib.h>
#if AMT_SUPPORT
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmtPolicy.h>
#include <Library/DxeAmtPolicyLib.h>
#include <MeBiosPayloadHob.h>

#include <Library/DxeAmtPolicyUpdateLib.h>

#endif // AMT_SUPPORT




BOOLEAN MeIsAfterEndOfPost (void);

//
// Function implementations
//
UINT8 GetMeMode(void)
{
  EFI_STATUS        Status;
  UINT32            retVal;
  HECI_PROTOCOL     *Heci;

  //
  // Default ME Mode
  //
  retVal = ME_MODE_FAILED;

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, &Heci);
  if (!EFI_ERROR (Status)) {
    Status = Heci->GetMeMode (&retVal);
    ASSERT_EFI_ERROR(Status);
  }
  return (UINT8)retVal;
}

/**

  Gets data from FW and updates initial values of BIOS setup data structures.
  Function is called only if we enter BIOS Setup, because it sends heci
  messages and takes some time to execute.

  @param Event         Pointer to the event that triggered this Callback Function
  @param Context       VOID Pointer required for Callback function

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
UpdateSetupDataWithFwInfo (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
#if AMT_SUPPORT
  EFI_STATUS              Status;
  UINT8                   FwUpdateState;
  ME_RC_CONFIGURATION     MeRcConfiguration;
  ME_RC_CONFIGURATION     MeRcConfigurationOrg;
  UINT32                  MeMode;
  MEFWCAPS_SKU            CurrentFeatures;
  SPS_POLICY_PROTOCOL    *SpsPolicy;

  if (MeTypeIsAmt())
  {
    FwUpdateState = 0;

    MeMode = GetMeMode();
    if (MeMode == ME_MODE_FAILED) {
      return EFI_DEVICE_ERROR;
     }

    Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeRcConfiguration);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[SPS Policy] UpdateSetupDataWithFwInfo - Error %r getting Setup Data \n", Status));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    
    CopyMem(&MeRcConfigurationOrg, &MeRcConfiguration, sizeof(ME_RC_CONFIGURATION));

    MeRcConfiguration.MeStateControl = 0;
    if (MeMode == ME_MODE_NORMAL) {
      MeRcConfiguration.MeStateControl = 1;
      Status = HeciGetFwFeatureStateMsg (&CurrentFeatures);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[SPS Policy] HeciGetFwFeatureStateMsg Status = %r\n", Status));
        return EFI_SUCCESS; // Event should not return error status, so just exit routine
      }

      if (CurrentFeatures.Fields.Manageability) {
        MeRcConfiguration.MngState = 1;
      } else {
        MeRcConfiguration.MngState = 0;
      }
    }

    if (CompareMem(&MeRcConfigurationOrg, &MeRcConfiguration, sizeof(ME_RC_CONFIGURATION)) != 0) {
      Status = SetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeRcConfiguration);
      CopyMem(&MeRcConfigurationOrg, &MeRcConfiguration, sizeof(ME_RC_CONFIGURATION));
    }

    //
    // Skip Local Firmware update if not Consumer sku or not in normal mode
    //
    if ((MeRcConfiguration.MeImageType != INTEL_ME_CONSUMER_FW) || (MeMode != ME_MODE_NORMAL)) {
      return EFI_SUCCESS;
    }

    Status = HeciGetLocalFwUpdate (&FwUpdateState);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[SPS Policy] HeciGetLocalFwUpdate Status = %r\n", Status));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

    //
    // MEBx Provides three options - Enabled/Disable/Password Protected
    // BIOS provides two options. If for some reason Password Protected
    // is selected then update FW with BIOS default/Selection
    //
    if (MeRcConfiguration.LocalFwUpdEnabled != FwUpdateState) {
      if (FwUpdateState < MAX_FW_UPDATE_BIOS_SELECTIONS) {
        MeRcConfiguration.LocalFwUpdEnabled = FwUpdateState;
      }
    }
    Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &SpsPolicy);
    if (!EFI_ERROR(Status)) {
      SpsPolicy->MeAmtConfig.PreviousMeStateControl = MeRcConfigurationOrg.MeStateControl;
      SpsPolicy->MeAmtConfig.PreviousLocalFwUpdEnabled = MeRcConfigurationOrg.LocalFwUpdEnabled;
      SpsPolicy->AnythingChanged = FALSE;
    }

    if (CompareMem(&MeRcConfigurationOrg, &MeRcConfiguration, sizeof(ME_RC_CONFIGURATION)) != 0) {
      Status = SetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeRcConfiguration);
    }
  }
#endif // AMT_SUPPORT

  return EFI_SUCCESS;
}

/**

  Install the ME Sps Policy Library

  @param SpsPolicy              ME Sps Policy in DXE phase

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
InstallSpsPolicyLibrary (
  IN OUT  SPS_POLICY_PROTOCOL  *SpsPolicy
)
{
  EFI_STATUS              Status;
  EFI_EVENT               SetupEnterEvent;
  VOID                    *pSetupRegistration;

  DEBUG ((DEBUG_ERROR, "[SPS Policy] SpsPolicyEntryPoint start [InstallSpsPolicyLibrary()]\n"));
  ASSERT(SpsPolicy != NULL);

  Status = UpdateSpsPolicyFromSetup (SpsPolicy, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] UpdateSpsPolicyFromSetup Failed and the Status is %x\n", Status));
    return Status;
  }
  //
  // Get info for ME setup options
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  UpdateSetupDataWithFwInfo,
                  NULL,
                  &SetupEnterEvent
                  );
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get FW data if we enter UI
  //
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSetupEnterGuid,
                  SetupEnterEvent,
                  &pSetupRegistration
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


VOID
UpdateMeSpsPolicyFromSetup(
  SPS_POLICY_PROTOCOL     *SpsPolicyInstance,
  // APTIOV_SERVER_OVERRIDE_RC_START : Change in SpsAltitude option default causes Global reset after Save & Reset in setup
  ME_RC_CONFIGURATION     *pSetupData,
  BOOLEAN                 *resetRequest)  
  // APTIOV_SERVER_OVERRIDE_RC_END : Change in SpsAltitude option default causes Global reset after Save & Reset in setup
{
#if SPS_SUPPORT
  SPS_POLICY_PROTOCOL     SpsPolicyInstanceOrg;

  if (SpsPolicyInstance == NULL) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] Error: UpdateMeNmPolicyFromSetup: wrong input parameters\n"));
    return;
  }
  DEBUG ((DEBUG_ERROR, "[SPS Policy] UpdateMeNmPolicyFromSetup\n"));
  CopyMem(&SpsPolicyInstanceOrg, SpsPolicyInstance, sizeof(SPS_POLICY_PROTOCOL));

  SpsPolicyInstance->SpsConfig.SpsAltitude = pSetupData->SpsAltitude;
  SpsPolicyInstance->SpsConfig.SpsMctpBusOwner = pSetupData->SpsMctpBusOwner;
  SpsPolicyInstance->SpsConfig.PreDidMeResetEnabled = pSetupData->PreDidMeResetEnabled;
  SpsPolicyInstance->SpsConfig.Heci1HideInMe = pSetupData->MeHeci1HideInMe;
  SpsPolicyInstance->SpsConfig.Heci2HideInMe = pSetupData->MeHeci2HideInMe;
  SpsPolicyInstance->SpsConfig.Heci3HideInMe = pSetupData->MeHeci3HideInMe;
  SpsPolicyInstance->SpsConfig.NmPwrOptBoot = pSetupData->NmPwrOptBoot;
  SpsPolicyInstance->SpsConfig.NmCores2Disable = pSetupData->NmCores2Disable;
#if ME_TESTMENU_FLAG
  SpsPolicyInstance->SpsConfig.MeHmrfpoLockEnabled = pSetupData->MeHmrfpoLockEnabled;
  SpsPolicyInstance->SpsConfig.MeHmrfpoEnableEnabled = pSetupData->MeHmrfpoEnableEnabled;
  SpsPolicyInstance->SpsConfig.NmPwrOptBootOverride = pSetupData->NmPwrOptBootOverride;
  SpsPolicyInstance->SpsConfig.NmCores2DisableOverride = pSetupData->NmCores2DisableOverride;
  SpsPolicyInstance->SpsConfig.NmPowerMsmtOverride = pSetupData->NmPowerMsmtOverride;
  SpsPolicyInstance->SpsConfig.NmPowerMsmtSupport = pSetupData->NmPowerMsmtSupport;
  SpsPolicyInstance->SpsConfig.NmHwChangeOverride = pSetupData->NmHwChangeOverride;
  SpsPolicyInstance->SpsConfig.NmHwChangeStatus = pSetupData->NmHwChangeStatus;
  SpsPolicyInstance->SpsConfig.NmPtuLoadOverride = pSetupData->NmPtuLoadOverride;
  SpsPolicyInstance->SpsConfig.MeGrLockEnabled = pSetupData->MeGrLockEnabled;
  SpsPolicyInstance->SpsConfig.MeGrPromotionEnabled = pSetupData->MeGrPromotionEnabled;
  SpsPolicyInstance->SpsConfig.BreakRtcEnabled = pSetupData->BreakRtcEnabled;
#endif // ME_TESTMENU_FLAG

  // APTIOV_SERVER_OVERRIDE_RC_START : Change in SpsAltitude option default causes Global reset after Save & Reset in setup
  if ((CompareMem(&SpsPolicyInstanceOrg, SpsPolicyInstance, sizeof(SPS_POLICY_PROTOCOL)) != 0) && (resetRequest != NULL))
  // APTIOV_SERVER_OVERRIDE_RC_END : Change in SpsAltitude option default causes Global reset after Save & Reset in setup
  {
    SpsPolicyInstance->AnythingChanged = TRUE;
  }
#endif // SPS_SUPPORT
} // UpdateMeNmPolicyFromSetup()

/**

  Function that is called when we save UI Data

  @param pSetupData           Pointer to Setup Data
  @param resetRequest         Returns information if function requests reset

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
updateMngState (
  ME_RC_CONFIGURATION    *pSetupData,
  BOOLEAN                *resetRequest
)
{
#if AMT_SUPPORT
  EFI_STATUS             Status;
  UINT32                 EnableBitmap = 0;
  UINT32                 DisableBitmap = 0;
  MEFWCAPS_SKU           CurrentFeatures;
  UINT8                  AmtState = 0;
#endif // AMT_SUPPORT

  DEBUG ((DEBUG_ERROR, "[SPS Policy] updateMngState - called\n"));
  if (pSetupData == NULL) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] Error: Wrong parameters\n"));
    return EFI_INVALID_PARAMETER;
  }

#if AMT_SUPPORT
  if (resetRequest == NULL) {
    // Called not from setup,
    // in case of change reset cannot be applied
    return EFI_SUCCESS;
  }

  Status = HeciGetFwFeatureStateMsg (&CurrentFeatures);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] HeciGetFwFeatureStateMsg Status = %r\n", Status));
    return Status;
  }

  if (CurrentFeatures.Fields.Manageability) {
    AmtState = 1;
  } else {
    AmtState = 0;
  }

  if (AmtState == pSetupData->MngState) {
    return EFI_SUCCESS;
  }

  if (pSetupData->MngState == 1) {
    //
    // User wants to enable AMT
    //
    EnableBitmap  = AMT_BITMASK;
    DisableBitmap = CLEAR_AMT_BIT;
  } else if (pSetupData->MngState == 0) {
    //
    // User wants to disable AMT
    //
    EnableBitmap  = CLEAR_AMT_BIT;
    DisableBitmap = AMT_BITMASK;
  }

  if (!MeIsAfterEndOfPost()) {
    Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[SPS Policy] HeciFwFeatureStateOverride Status = %r\n", Status));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    if (resetRequest) {
      *resetRequest = TRUE;
    }
  }
#endif // AMT_SUPPORT

  return EFI_SUCCESS;
}

/**

  Function that is called when we save UI Data

  @param SpsPolicy            Pointer to ME SPS Policy
  @param pSetupData           Pointer to Setup Data
  @param resetRequest         Returns information if function requests reset

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
updateFwuData (
  SPS_POLICY_PROTOCOL    *SpsPolicy,
  ME_RC_CONFIGURATION    *pSetupData,
  BOOLEAN                *resetRequest
  )
{
#if AMT_SUPPORT
  EFI_STATUS              Status;
  HECI_PROTOCOL           *Heci;
  UINT32                  MeMode;

  if ((SpsPolicy == NULL) || (pSetupData == NULL)) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] ERROR: updateFwuData - Wrong input data\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Skip Local Firmware update if not Consumer sku
  //
  if (pSetupData->MeImageType != INTEL_ME_CONSUMER_FW) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
             &gHeciProtocolGuid,
             NULL,
             &Heci
           );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] Error: Can't find Heci protocol, Status = %r\n", Status));
    return Status;
  }

  //
  // Get ME Operation Mode
  //
  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] Error: Can't get ME mode, Status = %r\n", Status));
    return Status;
  }

  //
  // Skip Local Firmware update if not in normal mode
  //
  if (MeMode != ME_MODE_NORMAL) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &SpsPolicy);
  if (EFI_ERROR(Status)) {
    // no gSpsPolicyProtocolGuid, no update (reset should be requested)
    return EFI_SUCCESS;
  }

  if (SpsPolicy->MeAmtConfig.PreviousLocalFwUpdEnabled != pSetupData->LocalFwUpdEnabled) {
    if (!MeIsAfterEndOfPost()) {
      Status = HeciSetLocalFwUpdate (pSetupData->LocalFwUpdEnabled);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[SPS Policy] HeciSetLocalFwUpdate Status = %r\n", Status));
        ASSERT_EFI_ERROR (Status);
        return Status;
      }
    }
    SpsPolicy->MeAmtConfig.PreviousLocalFwUpdEnabled = pSetupData->LocalFwUpdEnabled;
    SpsPolicy->AnythingChanged = TRUE;
  }
#endif // AMT_SUPPORT

  return EFI_SUCCESS;
}


/**

  Function that is Called if we change Manageability State BIOS Setup Option

  @param SpsPolicy          Pointer to ME SPS Policy
  @param pSetupData           Pointer to Setup Data
  @param resetRequest         Returns information if function requests reset

  @retval EFI_SUCCESS           Initialization complete.


**/
EFI_STATUS
updateMeState (
  SPS_POLICY_PROTOCOL    *SpsPolicy,
  ME_RC_CONFIGURATION    *pSetupData,
  BOOLEAN                *resetRequest
)
{
  DEBUG ((DEBUG_ERROR, "[SPS Policy] updateMeState called\n"));

  if ((pSetupData == NULL) || (SpsPolicy == NULL)) {
    DEBUG ((DEBUG_ERROR, "[SPS Policy] ERROR: updateMeState - Invalid parameters\n"));
    return EFI_INVALID_PARAMETER;
  }

#if defined(AMT_SUPPORT) && AMT_SUPPORT
  if (!MeIsAfterEndOfPost()) {
    EFI_STATUS              Status;

    if (SpsPolicy->MeAmtConfig.PreviousMeStateControl != pSetupData->MeStateControl) {
      HECI_PROTOCOL *Heci;
      UINT32        MeMode;

      Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[SPS Policy] ERROR: updateMeState(): Can't locate Heci protocol, Status = %r\n", Status));
        return EFI_UNSUPPORTED;
      }
      Status = Heci->GetMeMode (&MeMode);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[SPS Policy] ERROR: updateMeState(): Can't get ME Mode, Status = %r\n", Status));
        return EFI_UNSUPPORTED;
      }
      //
      // ME State Control
      //
      if (pSetupData->MeStateControl == 0) {
        if (MeMode == ME_MODE_NORMAL) {
          Status = HeciSetMeDisableMsg ();
          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "[SPS Policy] HeciSetMeDisableMsg Status = %r\n", Status));
            return Status;
          }
        } else {
          DEBUG ((DEBUG_INFO, "[SPS Policy] HeciSetMeDisableMsg() not called. ME is not in Normal state already\n"));
        }
      }

      if (pSetupData->MeStateControl == 1) {
        if (MeMode != ME_MODE_NORMAL) {
          Status = HeciSetMeEnableMsg ();
          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "[SPS Policy] HeciSetMeEnableMsg Status = %r\n", Status));
            return Status;
          }
        } else {
          DEBUG ((DEBUG_INFO, "[SPS Policy] HeciSetMeEnableMsg() not called. ME is in Normal state already\n"));
        }
      }
      SpsPolicy->MeAmtConfig.PreviousMeStateControl = pSetupData->MeStateControl;
      SpsPolicy->AnythingChanged = TRUE;
    }
  }
#endif // AMT_SUPPORT

  return EFI_SUCCESS;
}

BOOLEAN
UpdateMeAmtPolicyFromSetup (
  SPS_POLICY_PROTOCOL     *SpsPolicyInstance,
  ME_RC_CONFIGURATION     *SetupData,
  BOOLEAN                 *resetRequest
  )
{
  BOOLEAN retVal = FALSE; // Indication if setup has to be saved

  DEBUG((DEBUG_INFO, "[SPS Policy] UpdateMeAmtPolicyFromSetup\n"));

#if AMT_SUPPORT
#if SPS_SUPPORT
    if (MeTypeIsAmt())
#endif // SPS_SUPPORT
    {
      AMT_POLICY_PROTOCOL  *DxeAmtPolicy;
      EFI_STATUS           Status;

      Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, (VOID **) &DxeAmtPolicy);
      if (!EFI_ERROR (Status)) {
        AMT_POLICY_PROTOCOL DxeAmtPolicyOrg;

        CopyMem(&DxeAmtPolicyOrg, DxeAmtPolicy, sizeof(AMT_POLICY_PROTOCOL));
        UpdateDxeAmtPolicy(DxeAmtPolicy);

        if ((CompareMem(&DxeAmtPolicyOrg, DxeAmtPolicy, sizeof(AMT_POLICY_PROTOCOL)) != 0) &&
            (SpsPolicyInstance != NULL)) {
          SpsPolicyInstance->AnythingChanged = TRUE;
        }
      }

      if (SpsPolicyInstance != NULL) {

        if (GetMeMode() == ME_MODE_NORMAL) {
          Status = updateMngState(SetupData, resetRequest);
          if (!EFI_ERROR (Status)) {
            Status = updateFwuData(SpsPolicyInstance, SetupData, resetRequest);
            SpsPolicyInstance->AnythingChanged = TRUE;
          }
        }
        updateMeState(SpsPolicyInstance, SetupData, resetRequest);
      }
    }
#endif // AMT_SUPPORT

  return retVal;
}

/**

  Update Me SPS policy while MeSpsProtocol is installed.

  @param SpsPolicyInstance        - ME Sps Policy
  @param pSetupData               - pointer to system configuration
  @param resetRequest             - Returns information if function requests reset

  @retval Efi Status

**/
EFI_STATUS
UpdateSpsPolicyFromSetup (
  SPS_POLICY_PROTOCOL     *SpsPolicyInstance,
  ME_RC_CONFIGURATION     *pSetupData,
  BOOLEAN                 *resetRequest
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  ME_RC_CONFIGURATION  MeRcConfiguration;

  DEBUG((DEBUG_INFO, "[SPS Policy] UpdateSpsPolicyFromSetup: start\n"));
  ASSERT(SpsPolicyInstance != NULL);

  if (pSetupData == NULL)
  {
    Status = GetSpecificConfigGuid(&gEfiMeRcVariableGuid, (VOID *)&MeRcConfiguration);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS Policy] ERROR: Cannot locate ME RC Variable (%r)\n", Status));
    }
    else
    {
      pSetupData = &MeRcConfiguration;
    }
  }
  if (pSetupData != NULL)
  {
    // APTIOV_SERVER_OVERRIDE_RC_START : Change in SpsAltitude option default causes Global reset after Save & Reset in setup
    UpdateMeSpsPolicyFromSetup(SpsPolicyInstance, pSetupData, resetRequest);
	// APTIOV_SERVER_OVERRIDE_RC_END : Change in SpsAltitude option default causes Global reset after Save & Reset in setup
    
    if (UpdateMeAmtPolicyFromSetup(SpsPolicyInstance, pSetupData, resetRequest))
    {
      //
      // update FW Downgrade in Setup
      //
      Status = SetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)pSetupData);
    }
  }

  return Status;
}


/**
  Function stores Me SPS Policy to Setup configuration

  @param SpsPolicyInstance           ME SPS Policy instance to save
  @param saveMask                    What to save

  @retval EFI_SUCCESS           Saving MePolicy to Setup succeed.
  @retval EFI_INVALID_PARAMETER Wrong input parameters
**/
EFI_STATUS
SaveSpsPolicyToSetupConfiguration(SPS_POLICY_PROTOCOL *SpsPolicyInstance)
{
  EFI_STATUS            Status;
  ME_RC_CONFIGURATION   MeRcConfiguration;
  ME_RC_CONFIGURATION   MeRcConfigurationOrg;

  Status = GetSpecificConfigGuid(&gEfiMeRcVariableGuid, (VOID*)&MeRcConfiguration);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS Policy] ERROR: Failed to read ME_RC_CONFIGURATION!  Status = %r\n", Status));
    return EFI_NOT_READY;
  }

  CopyMem(&MeRcConfigurationOrg, &MeRcConfiguration, sizeof(ME_RC_CONFIGURATION));
  MeRcConfiguration.MeType = SpsPolicyInstance->MeType;
#ifdef AMT_SUPPORT
  if (MeTypeIsAmt())
  {
    ME_BIOS_PAYLOAD_HOB *MbpHob = GetFirstGuidHob(&gMeBiosPayloadHobGuid);

    if (MbpHob)
    {
      MeRcConfiguration.MeImageType = (UINT8) MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;
    }
  }
#endif // AMT_SUPPORT

#ifdef SPS_SUPPORT
  MeRcConfiguration.SpsAltitude = SpsPolicyInstance->SpsConfig.SpsAltitude;
  MeRcConfiguration.SpsMctpBusOwner = SpsPolicyInstance->SpsConfig.SpsMctpBusOwner;
  MeRcConfiguration.PreDidMeResetEnabled = (UINT8)SpsPolicyInstance->SpsConfig.PreDidMeResetEnabled;
  MeRcConfiguration.MeHeci1HideInMe = (UINT8)SpsPolicyInstance->SpsConfig.Heci1HideInMe;
  MeRcConfiguration.MeHeci2HideInMe = (UINT8)SpsPolicyInstance->SpsConfig.Heci2HideInMe;
  MeRcConfiguration.MeHeci3HideInMe = (UINT8)SpsPolicyInstance->SpsConfig.Heci3HideInMe;
  MeRcConfiguration.NmPwrOptBoot = (UINT8)SpsPolicyInstance->SpsConfig.NmPwrOptBoot;
  MeRcConfiguration.NmCores2Disable = (UINT8)SpsPolicyInstance->SpsConfig.NmCores2Disable;
#if ME_TESTMENU_FLAG
  MeRcConfiguration.MeHmrfpoLockEnabled = (UINT8)SpsPolicyInstance->SpsConfig.MeHmrfpoLockEnabled;
  MeRcConfiguration.MeHmrfpoEnableEnabled = (UINT8)SpsPolicyInstance->SpsConfig.MeHmrfpoEnableEnabled;
  MeRcConfiguration.NmPwrOptBootOverride = (UINT8)SpsPolicyInstance->SpsConfig.NmPwrOptBootOverride;
  MeRcConfiguration.NmCores2DisableOverride = (UINT8)SpsPolicyInstance->SpsConfig.NmCores2DisableOverride;
  MeRcConfiguration.NmPowerMsmtOverride = (UINT8)SpsPolicyInstance->SpsConfig.NmPowerMsmtOverride;
  MeRcConfiguration.NmPowerMsmtSupport = (UINT8)SpsPolicyInstance->SpsConfig.NmPowerMsmtSupport;
  MeRcConfiguration.NmHwChangeOverride = (UINT8)SpsPolicyInstance->SpsConfig.NmHwChangeOverride;
  MeRcConfiguration.NmHwChangeStatus = (UINT8)SpsPolicyInstance->SpsConfig.NmHwChangeStatus;
  MeRcConfiguration.NmPtuLoadOverride = (UINT8)SpsPolicyInstance->SpsConfig.NmPtuLoadOverride;
  MeRcConfiguration.MeGrLockEnabled = (UINT8)SpsPolicyInstance->SpsConfig.MeGrLockEnabled;
  MeRcConfiguration.MeGrPromotionEnabled = (UINT8)SpsPolicyInstance->SpsConfig.MeGrPromotionEnabled;
  MeRcConfiguration.BreakRtcEnabled = (UINT8)SpsPolicyInstance->SpsConfig.BreakRtcEnabled;
#endif // ME_TESTMENU_FLAG
#endif // SPS_SUPORT
  if (CompareMem(&MeRcConfigurationOrg, &MeRcConfiguration, sizeof(ME_RC_CONFIGURATION)) != 0)
  {
    Status = SetSpecificConfigGuid(&gEfiMeRcVariableGuid, (VOID *)&MeRcConfiguration);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS Policy] ERROR: Failed to save ME policy to ME RC Variable (%r)\n", Status));
    }
    else
    {
      // Me Policy is stored in setup. Clear AnythingChanged
      SpsPolicyInstance->AnythingChanged = 0;
    }
  }
  return Status;
}

