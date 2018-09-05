/** @file

@copyright
 Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
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

#include "DxeMePolicyUpdate.h"
#include <Library/SetupLib.h>
#include <Library/MeTypeLib.h>
#include <Library/MeConfigLib.h>
//
// Record version
//
#define RECORD_REVISION_1              0x01

#define MAX_FW_UPDATE_BIOS_SELECTIONS  2

GLOBAL_REMOVE_IF_UNREFERENCED UINT32   mMeMode;

//
// Function implementations executed during policy initialization phase
//
/**
  This is to display localized message in the console. This is
  used to display message strings in local language. To display
  the message, the routine will check the message ID and ConOut
  the message strings.

  @param[in] MsgId                Me error message ID for displaying on screen message

**/
VOID
EFIAPI
ShowMeReportError (
  IN ME_ERROR_MSG_ID              MsgId
  )
{
  UINTN            MsgDelay;

  MsgDelay = HECI_MSG_DELAY;
  gST->ConOut->ClearScreen (gST->ConOut);

  switch (MsgId) {
  case MSG_EOP_ERROR:
    gST->ConOut->OutputString (gST->ConOut, L"Error sending End Of Post message to ME!\n");
    //
    // Additional delay only for EOP error message
    //
    MsgDelay *= 5;
    break;

  case MSG_ME_FW_UPDATE_FAILED:
    gST->ConOut->OutputString (gST->ConOut, L"ME FW Update Failed, please try again!\n");
    break;

  case MSG_ASF_BOOT_DISK_MISSING:
    gST->ConOut->OutputString (gST->ConOut, L"Boot disk missing, please insert boot disk and press ENTER\r\n");
    break;

  case MSG_KVM_TIMES_UP:
    gST->ConOut->OutputString (gST->ConOut, L"Error!! Times up and the KVM session was cancelled!!");
    break;

  case MSG_KVM_REJECTED:
    gST->ConOut->OutputString (gST->ConOut, L"Error!! The request has rejected and the KVM session was cancelled!!");
    break;

  case MSG_HMRFPO_LOCK_FAILURE:
    gST->ConOut->OutputString (gST->ConOut, L"(A7) Me FW Downgrade - Request MeSpiLock Failed\n");
    break;

  case MSG_HMRFPO_UNLOCK_FAILURE:
    gST->ConOut->OutputString (gST->ConOut, L"(A7) Me FW Downgrade - Request MeSpiEnable Failed\n");
    break;

  case MSG_ME_FW_UPDATE_WAIT:
    gST->ConOut->OutputString (gST->ConOut, L"Intel(R) Firmware Update is in progress. It may take up to 90 seconds. Please wait.\n");
    break;

  case MSG_ILLEGAL_CPU_PLUGGED_IN:
    gST->ConOut->OutputString (gST->ConOut, L"\n\n\rAn unsupported CPU/PCH configuration has been identified.\n");
    gST->ConOut->OutputString (gST->ConOut, L"\rPlease refer to the Platform Validation Matrix\n\rfor supported CPU/PCH combinations.");
    break;

  case MSG_KVM_WAIT:
    gST->ConOut->OutputString (gST->ConOut, L"Waiting Up to 8 Minutes For KVM FW.....");
    break;

  case MSG_PLAT_DISABLE_WAIT:
    gST->ConOut->OutputString (gST->ConOut, L"WARNING! Firmware encountered errors and will reboot the platform in 30 minutes.");
    MsgDelay = 5 * HECI_MSG_DELAY;
    break;

  default:
    DEBUG ((DEBUG_ERROR, "This Message Id hasn't been defined yet, MsgId = %x\n", MsgId));
    break;
  }

  gBS->Stall (MsgDelay);
}

/**
  Update the ME Policy Library

  @param[in] DxeMePolicy                The pointer to get ME Policy protocol instance

  @retval EFI_SUCCESS                   Initialization complete.
  @retval EFI_UNSUPPORTED               The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES          Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR              Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
UpdateDxeMePolicy (
  IN OUT  ME_POLICY_PROTOCOL            *DxeMePolicy
  )
{
  EFI_STATUS              Status;
  ME_RC_CONFIGURATION     MeSetup;
  ME_RC_CONFIGURATION     *MeSetupPtr;

  DEBUG ((DEBUG_INFO, "UpdateDxeMePolicy start\n"));

  if (DxeMePolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get Me configuration from Setup Data
  //
  Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeSetup);

  if (EFI_ERROR (Status)) {
    MeSetupPtr = NULL;
  } else {
    MeSetupPtr = &MeSetup;
  }

  if (MeSetupPtr != NULL) {
    UpdateMePolicyFromSetup (DxeMePolicy, MeSetupPtr);
  }

  DEBUG ((DEBUG_INFO, "UpdateDxeMePolicy exit\n"));

  return Status;
}

/**
  Update ME Policy while MePlatformProtocol is installed.

  @param[in] MePolicyInstance     Instance of ME Policy Protocol
  @param[in] MeSetupPtr           A point to ME Setup Data
  @param[in] MeSetupStoragePtr    A point to ME Setup Storage

**/
VOID
UpdateMePolicyFromSetup (
  IN ME_POLICY_PROTOCOL           *MePolicyInstance,
  IN ME_RC_CONFIGURATION          *MeSetupPtr
  )
{
  BOOLEAN  MeIsCorporateAmt = FALSE;
  
  DEBUG((DEBUG_INFO, "[ME] UpdateMePolicyFromSetup: start\n"));
  if ((MePolicyInstance == NULL) || (MeSetupPtr == NULL)) {
    DEBUG((DEBUG_ERROR, "[ME] ERROR: UpdateMePolicyFromSetup: Wrong input parameters\n"));
    ASSERT(MePolicyInstance != NULL && MeSetupPtr != NULL);
    return;
  }

#if AMT_SUPPORT
  MeIsCorporateAmt = MeTypeIsCorporateAmt();
#endif
  
#if ME_TESTMENU_FLAG
  
  MePolicyInstance->MeConfig.DidInitStat = MeSetupPtr->MeDidInitStat;
  MePolicyInstance->MeConfig.SendDidMsg = MeSetupPtr->MeDidEnabled;
  MePolicyInstance->MeConfig.HeciCommunication1 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC1,
                                                                        MeSetupPtr->MeHeci1Enabled,
                                                                            MeIsCorporateAmt);
  MePolicyInstance->MeConfig.HeciCommunication2 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC2,
                                                                        MeSetupPtr->MeHeci2Enabled,
                                                                            MeIsCorporateAmt);
  MePolicyInstance->MeConfig.HeciCommunication3 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC3,
                                                                        MeSetupPtr->MeHeci3Enabled,
                                                                            MeIsCorporateAmt);
  MePolicyInstance->MeConfig.IderDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_IDER,
                                                                         MeSetupPtr->MeIderEnabled,
                                                                          MeIsCorporateAmt);
  MePolicyInstance->MeConfig.KtDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_SOL,
                                                                        MeSetupPtr->MeKtEnabled,
                                                                        MeIsCorporateAmt);
  MePolicyInstance->MeConfig.HostResetNotification = MeSetupPtr->HostResetNotification;
  MePolicyInstance->MeConfig.HsioMessaging = MeSetupPtr->HsioMessagingEnabled;
  MePolicyInstance->MeConfig.EndOfPostMessage = MeSetupPtr->MeEndOfPostEnabled;
  
  MePolicyInstance->MeConfig.DisableD0I3SettingForHeci = MeSetupPtr->DisableD0I3SettingForHeci;
#else
  MePolicyInstance->MeConfig.KtDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_SOL,
                                                                        ME_DEVICE_AUTO,
                                                                        MeIsCorporateAmt);
#endif // ME_TESTMENU_FLAG
  
  MePolicyInstance->MeConfig.MeFwDownGrade = MeSetupPtr->MeFwDowngrade;
  MePolicyInstance->MeConfig.MeLocalFwUpdEnabled = MeSetupPtr->LocalFwUpdEnabled;
  MePolicyInstance->MeConfig.OsPtpAware = MeSetupPtr->OsPtpAware;
  MePolicyInstance->MeConfig.CoreBiosDoneEnabled = MeSetupPtr->CoreBiosDoneEnabled;
}


#ifdef AMT_SUPPORT    // SERVER_BIOS_FLAG
/**
  Functions performs HECI exchange with FW to update MePolicy settings.

  @param[in] Event         A pointer to the Event that triggered the callback.
  @param[in] Context       A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
UpdateMeSetupCallback (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  )
{
  EFI_STATUS                      Status;
  ME_RC_CONFIGURATION             MeSetup;
  ME_INFO_SETUP_DATA              MeInfoSetupData;
  ME_BIOS_PAYLOAD_HOB             *MbpHob;
  HECI_PROTOCOL                   *Heci;

  DEBUG ((DEBUG_INFO, "UpdateMeSetup event start\n"));
  //
  // Default ME Mode
  //
  mMeMode = ME_MODE_FAILED;
  MbpHob  = NULL;

  ZeroMem (&MeInfoSetupData, sizeof(MeInfoSetupData));
  //
  // Get Mbp Hob
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob == NULL) {
    MeInfoSetupData.MeFirmwareInfo = ME_MODE_FAILED;
  } else {
    MeInfoSetupData.MeMajor      = MbpHob->MeBiosPayload.FwVersionName.MajorVersion;
    MeInfoSetupData.MeMinor      = MbpHob->MeBiosPayload.FwVersionName.MinorVersion;
    MeInfoSetupData.MeHotFix     = MbpHob->MeBiosPayload.FwVersionName.HotfixVersion;
    MeInfoSetupData.MeBuildNo    = MbpHob->MeBiosPayload.FwVersionName.BuildVersion;
    MeInfoSetupData.MeFileSystem = MbpHob->MeBiosPayload.MFSIntegrity;
  }

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, &Heci);
  if (!EFI_ERROR (Status)) {
    Status = Heci->GetMeMode (&mMeMode);
    ASSERT_EFI_ERROR(Status);

    MeInfoSetupData.MeFirmwareInfo = (UINT8) mMeMode;
  }

  Status = gRT->SetVariable (
                  L"MeInfoSetup",
                  &gMeInfoSetupGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof(MeInfoSetupData),
                  &MeInfoSetupData
                  );
  ASSERT_EFI_ERROR(Status);


  Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeSetup);

  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR(Status)) {
    //
    // FW Downgrade BIOS Setup Option is only valid for one boot
    //
    MeSetup.MeFwDowngrade = 0;

    //
    // Reset Firmware Info
    //
    MeSetup.MeFirmwareInfo = MeInfoSetupData.MeFirmwareInfo;

    MeSetup.MeImageType            = 0;
    if (MbpHob != NULL) {

      //
      // Set Image Type for MEBx disposition
      //
      MeSetup.MeImageType = (UINT8) MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;

    }

    //
    // Update AMT settings
    //
    MeSetup.UnConfigureMe  = 0;
    MeSetup.AmtCiraRequest = 0;
  Status = SetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeSetup);
    ASSERT_EFI_ERROR (Status);
  }
  gBS->CloseEvent (Event);
}
#endif // AMT_SUPPORT : SERVER_BIOS_FLAG

