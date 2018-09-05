/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@file

Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  Name:
    MeSetup.c

  Description:
    This file contains functions that update ME related strings in BIOS setup.

**/
#if defined(AMT_SUPPORT) && AMT_SUPPORT

#include "SetupPlatform.h"
#include <Library/PciLib.h>
#include <Library/HiiLib.h>
#include <Protocol/HeciProtocol.h>
#include "Protocol/MeSpsPolicyProtocol.h"
#include "HeciRegs.h"
#include <Guid/SpsInfoHobGuid.h>
#include "Library/MeTypeLib.h"
#include "MeAccess.h"
#include "MeState.h"
#include <Library/MeSpsPolicyUpdateDxeLib.h>
#include <Library/SetupLib.h>
#include <Library/DxeMePolicyUpdateLib.h>
#include "Library/HeciCoreLib.h"
#include "AmtSetup.h"
#include "Protocol/AmtWrapperProtocol.h"
#include <Library/DxeAmtPolicyUpdateLib.h>
#include <Library/DxeMeLib.h>

#define ME_AMT_INFO_MAX_STR_SIZE  0x200
#define ME_SETUP_VAR_STRING_SIZE  0x46

// Restore MeStateControl when settings are not in sync
// to avoid roll back user changes
static BOOLEAN MeStateControlIsInSync = FALSE;

/**
  This function updates ME related strings in BIOS setup database.

  Some of the strings contain dummy values to be update with run-time data.
  These strings are updated by this function before any setup screen is
  printed. For some reason the strings from various screens,
  although identified with uniqueue ids, are stored in separate databases.
  Class identifes the database. To avoid multiple requests to ME
  and multiple traces parts of this function are executed for particular
  forms.

  @param[out] HiiAdvancedHandle - BIOS setup datebase handle
  @param[out] HiiMainHandle - BIOS setup datebase handle

  @return Void.
**/
VOID MeInitAmtSetupStrings(
  OUT EFI_HII_HANDLE HiiAdvancedHandle,
  OUT EFI_HII_HANDLE HiiMainHandle
  )
{
  EFI_STATUS            Status;
  HECI_PROTOCOL         *pHeciProtocol = NULL;
  HECI_FWS_REGISTER     HeciFwsReg;
  HECI_GS_SHDW_REGISTER HeciGsShdwReg;
  UINT32                RspLen;
  CHAR16                StrBuf[ME_AMT_INFO_MAX_STR_SIZE];
#if defined(ME_TESTMENU_FLAG) && ME_TESTMENU_FLAG
  CHAR16                MeSetupVarString[ME_SETUP_VAR_STRING_SIZE];
  UINTN                 MeSetupVarStringSize;
#endif // ME_TESTMENU_FLAG
  CHAR16*               MeTypeString = NULL;
  CHAR16*               MeInfoString = NULL;
  ME_RC_CONFIGURATION   MeSetup;
  UINT32                MeFirmwareMode;
  const  unsigned int   MaxMeMode = 6;
  static EFI_STRING_ID  MeMode[] = {
    STRING_TOKEN(STR_ME_NORMAL_MODE),
    STRING_TOKEN(STR_ME_FAILED),
    STRING_TOKEN(STR_ME_ALT_DISABLED),
    STRING_TOKEN(STR_ME_TEMP_DISABLED),
    STRING_TOKEN(STR_ME_SECOVER),
    STRING_TOKEN(STR_ME_FAILED),
    STRING_TOKEN(STR_ME_FAILED)
    };
  union {
    GEN_GET_FW_VER       FwVerReq;
    GEN_GET_FW_VER_ACK   FwVerRsp;
  } HeciMsg;

  DEBUG((EFI_D_INFO, "[ME] MeInitAmtSetupStrings called...\n"));
// APTIOV_SERVER_OVERRIDE_RC_START
  if(NULL != HiiAdvancedHandle)// && (NULL != HiiMainHandle))
// APTIOV_SERVER_OVERRIDE_RC_END
  {
    //
    // Init MEFS#1 info in setup string database
    //
    HeciFwsReg.ul = HeciPciRead32(R_ME_HFS);
    if (HeciFwsReg.ul == 0xFFFFFFFF) {
      HeciFwsReg.ul = GetMeFs1FromHob(HeciFwsReg.ul);
    }
    DEBUG((EFI_D_INFO, "[ME] MEFS1: 0x%08X\n", HeciFwsReg.ul));
    UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"0x%08X", HeciFwsReg.ul);
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWSTS1_VALUE), StrBuf, NULL);

    //
    // Init MEFS#2 info in setup string database
    //
    HeciGsShdwReg.ul = HeciPciRead32(R_ME_GS_SHDW);
    DEBUG((EFI_D_INFO, "[ME] MEFS2: 0x%08X\n", HeciGsShdwReg.ul));
    UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"0x%08X", HeciGsShdwReg.ul);
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWSTS2_VALUE), StrBuf, NULL);

    Status = GetSpecificConfigGuid(&gEfiMeRcVariableGuid, (VOID *)&MeSetup);
    if(!EFI_ERROR(Status))
    {
      UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"%X", MeSetup.MeImageType);
      HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FILE_SYSTEM_CORRUPTION_VALUE), StrBuf, NULL);

      //
      // Update ME FW SKU in setup strings database
      //
      switch(MeSetup.MeImageType)
      {
        case ME_IMAGE_CONSUMER_SKU_FW:
          UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"%a", STR_ME_FW_SKU_CONSUMER_FW);
          break;

        case ME_IMAGE_CORPORATE_SKU_FW:
          UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"%a", STR_ME_FW_SKU_CORPORATE_FW);
          break;

        default:
          UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"%a", STR_ME_FW_SKU_UNIDENTIFIED);
          break;
      }
      HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FW_SKU_VALUE), StrBuf, NULL);
    }
    else
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot locate MeSetup variable (status: %r)\n", Status));
    }

    //
    // Update ME Current State in setup strings database
    //
    switch (HeciFwsReg.r.CurrentState)
    {
      case ME_STATE_RESET:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Reset");
        break;

      case ME_STATE_INIT:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Initializing");
        break;

      case ME_STATE_RECOVERY:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Recovery");
        break;

      case ME_STATE_NORMAL:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Operational");
        break;

      case ME_STATE_TRANSITION:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Transitioning");
        break;


      default:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Unknown(%d)", HeciFwsReg.r.CurrentState);
        break;
    }
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWCURSTE_VALUE), StrBuf, NULL);

    //
    // Update ME Error Code in setup strings database
    //
    switch (HeciFwsReg.r.ErrorCode)
    {
      case ME_ERROR_CODE_NO_ERROR:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"No Error");
        break;

      case ME_ERROR_CODE_UNKNOWN:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Uncategorized Failure");
        break;

      case ME_ERROR_CODE_IMAGE_FAILURE:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Image Failure");
        break;

      case ME_ERROR_CODE_DEBUG_FAILURE:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Debug Failure");
        break;

      default:
        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"Unknown(%d)", HeciFwsReg.r.ErrorCode);
        break;
    }
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWERRCODE_VALUE), StrBuf, NULL);

    //
    // Update ME FW Type in setup strings database
    //
    MeTypeString = HiiGetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FW_TYPE_AMT), NULL);
    if (MeTypeString)
    {
      HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FW_TYPE_VALUE), MeTypeString, NULL);
      FreePool(MeTypeString);
      MeTypeString = NULL;
    }

    switch (HeciFwsReg.r.CurrentState)
    {
      case ME_STATE_RECOVERY:    // Fall through
      case ME_STATE_NORMAL:      // Fall through
      case ME_STATE_TRANSITION:
        break;

      default:
        //
        // Do not send any HECI requests, no chance ME responds.
        //
        DEBUG((EFI_D_INFO, "[ME] Do not send any HECI requests, no chance ME responds\n"));
        return;
    }

    //
    // Update the ME firmware version info in BIOS Setup strings database
    //
    Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeciProtocol);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot locate HECI protocol (status: %r)\n", Status));
      return;
    }

    Status = pHeciProtocol->GetMeMode (&MeFirmwareMode);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot get current ME mode (status: %r)\n", Status));
    }
    else
    {
      //
      // Update ME FW Mode in setup strings database
      //
      if (MeFirmwareMode > MaxMeMode)
      {
        MeFirmwareMode = ME_MODE_FAILED;
      }
      MeFirmwareMode &= ME_MODE_MASK;
      DEBUG((EFI_D_INFO, "[Me] MeFirmwareMode read %d\n", MeFirmwareMode));
      MeInfoString = HiiGetString(mAdvancedHiiHandle, MeMode[MeFirmwareMode], NULL);
      if (MeInfoString) {
        HiiSetString(mAdvancedHiiHandle,STRING_TOKEN(STR_ME_FW_INFO_VALUE), MeInfoString, NULL);
        FreePool (MeInfoString);
        MeInfoString = NULL;
      }
      if(ME_MODE_TEMP_DISABLED == MeFirmwareMode)
      {
        DEBUG((EFI_D_INFO, "[ME] ME is in TEMP DISABLED MODE (don't send MKHI_GET_FW_VERSION message)\n"));
        return;
      }
    }

    //
    // Send MKHI_GET_FW_VERSION request to ME
    //
    DEBUG((EFI_D_INFO, "[ME] Sending MKHI_GET_FW_VERSION request to ME\n"));

    gBS->SetMem(&HeciMsg.FwVerReq, sizeof(HeciMsg.FwVerReq), 0);
    HeciMsg.FwVerReq.MKHIHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
    HeciMsg.FwVerReq.MKHIHeader.Fields.Command = GEN_GET_FW_VERSION_CMD;
    RspLen = sizeof(HeciMsg.FwVerRsp);

    Status = pHeciProtocol->SendwACK(
      HECI1_DEVICE,
      (UINT32*)&HeciMsg.FwVerReq,
      sizeof(HeciMsg.FwVerReq),
      &RspLen,
      BIOS_FIXED_HOST_ADDR,
      HECI_CORE_MESSAGE_ADDR
      );

    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot send MKHI_GET_FW_VERSION (status: %r)\n", Status));
    }
    else if(!((HeciMsg.FwVerRsp.MKHIHeader.Fields.IsResponse) &&
             (MKHI_GEN_GROUP_ID == HeciMsg.FwVerReq.MKHIHeader.Fields.GroupId) &&
             (GEN_GET_FW_VERSION_CMD == HeciMsg.FwVerReq.MKHIHeader.Fields.Command)))
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Invalid MKHI_GET_FW_VERSION response (MKHI: 0x%X)\n", HeciMsg.FwVerRsp.MKHIHeader.Data));
    }
    else
    {
      DEBUG((EFI_D_INFO,
        "[ME] Firmware version is:\n"
        "\tOperational: %02X:%d.%d.%d.%d\n"
        "\tRecovery:    %02X:%d.%d.%d.%d\n",
        (HeciMsg.FwVerRsp.Data.CodeMajor >> 8), (HeciMsg.FwVerRsp.Data.CodeMajor & 0xFF), HeciMsg.FwVerRsp.Data.CodeMinor, HeciMsg.FwVerRsp.Data.CodeHotFix, HeciMsg.FwVerRsp.Data.CodeBuildNo,
        (HeciMsg.FwVerRsp.Data.RcvyMajor >> 8), (HeciMsg.FwVerRsp.Data.CodeMajor & 0xFF), HeciMsg.FwVerRsp.Data.RcvyMinor, HeciMsg.FwVerRsp.Data.RcvyHotFix, HeciMsg.FwVerRsp.Data.RcvyBuildNo));

      UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"%02X:%d.%d.%d.%d",
          (HeciMsg.FwVerRsp.Data.CodeMajor >> 8), (HeciMsg.FwVerRsp.Data.CodeMajor & 0xFF), HeciMsg.FwVerRsp.Data.CodeMinor, HeciMsg.FwVerRsp.Data.CodeHotFix, HeciMsg.FwVerRsp.Data.CodeBuildNo);

      HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWVER_OPERATIONAL_VALUE), StrBuf, NULL);

      //
      // Update ME firmware info in platform information menu
      //
      // APTIOV_SERVER_OVERRIDE_RC_START : Form not provided in AptioV
      //HiiSetString(HiiMainHandle, STRING_TOKEN(STR_ME_REVISION_VALUE), StrBuf, NULL);
      // APTIOV_SERVER_OVERRIDE_RC_END : Form not provided in AptioV

      UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"%02X:%d.%d.%d.%d",
          (HeciMsg.FwVerRsp.Data.RcvyMajor >> 8), (HeciMsg.FwVerRsp.Data.CodeMajor & 0xFF), HeciMsg.FwVerRsp.Data.RcvyMinor, HeciMsg.FwVerRsp.Data.RcvyHotFix, HeciMsg.FwVerRsp.Data.RcvyBuildNo);

      HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWVER_RECOVERY_VALUE), StrBuf, NULL);

      //
      // ME in dual-image configuration provides the version of the backup image yet
      //
      if (RspLen == sizeof(HeciMsg.FwVerRsp))
      {
        DEBUG((EFI_D_ERROR,
            "\tBackup:      %02X:%d.%d.%d.%d\n",
            (HeciMsg.FwVerRsp.Data.FitcMajor >> 8), (HeciMsg.FwVerRsp.Data.FitcMajor & 0xFF), HeciMsg.FwVerRsp.Data.FitcMinor, HeciMsg.FwVerRsp.Data.FitcHotFix, HeciMsg.FwVerRsp.Data.FitcBuildNo));

        UnicodeSPrint(StrBuf, ME_AMT_INFO_MAX_STR_SIZE, L"%02X:%d.%d.%d.%d",
            (HeciMsg.FwVerRsp.Data.FitcMajor >> 8), (HeciMsg.FwVerRsp.Data.FitcMajor & 0xFF), HeciMsg.FwVerRsp.Data.FitcMinor, HeciMsg.FwVerRsp.Data.FitcHotFix, HeciMsg.FwVerRsp.Data.FitcBuildNo);

        HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWVER_BACKUP_VALUE), StrBuf, NULL);
      }
    }

#if defined(ME_TESTMENU_FLAG) && ME_TESTMENU_FLAG
    // Update default settings for ME9+
    if(GetBootModeHob() == BOOT_WITH_DEFAULT_SETTINGS)
    {
      StrCpyS(MeSetupVarString, ME_SETUP_VAR_STRING_SIZE, ME_RC_CONFIGURATION_NAME);
      StrCatS(MeSetupVarString, ME_SETUP_VAR_STRING_SIZE, L"Default");

      Status = gRT->GetVariable (
        MeSetupVarString,
        &gEfiMeRcVariableGuid,
        NULL,
        &MeSetupVarStringSize,
        &MeSetup
        );

      if(!EFI_ERROR(Status))
      {
        // disable HECI2
        MeSetup.MeHeci2Enabled = 0;

        Status = gRT->SetVariable (
          MeSetupVarString,
          &gEfiMeRcVariableGuid,
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
          sizeof(ME_RC_CONFIGURATION),
          &MeSetup
          );

        if(EFI_ERROR(Status))
        {
          DEBUG((EFI_D_ERROR, "[ME] Can't update ME9+ defaults (%r).\n", Status));
        }
      }
      else
      {
        DEBUG((EFI_D_ERROR, "[ME] Can't open ME9+ defaults (%r).\n", Status));
      }
    }
#endif // ME_TESTMENU_FLAG
  }
  return;
}

/**

  @brief
  Sets MeStateControl status in provided ME Configuration

  @param[in] ME Configuration to update

  @retval EFI Status of operation

**/
EFI_STATUS MeSetMeStateControl(
  IN ME_RC_CONFIGURATION *MeRcConfiguration
  )
{
  UINT32      MeMode;
  EFI_STATUS  Status;
  BOOLEAN     MeStateControl = FALSE;

  DEBUG ((EFI_D_INFO, "[ME] MeSetStateControl Called\n"));

  if (MeStateControlIsInSync) {
    DEBUG ((EFI_D_INFO, "[ME] MeSetStateControl: next call, exit\n"));
    return EFI_SUCCESS;
  }

  if(MeRcConfiguration != NULL)
  {
    Status = HeciGetMeMode (&MeMode);
    if(!EFI_ERROR (Status) && (MeMode == ME_MODE_NORMAL))
    {
      MeStateControl = TRUE;
    }

    if(MeRcConfiguration->MeStateControl != (UINT8)MeStateControl)
    {
      DEBUG ((EFI_D_INFO, "[ME] MeStateControl present in setup is different to detected. Updating ...\n"));
      MeRcConfiguration->MeStateControl = (UINT8)MeStateControl;
    } else {
      // settings in BIOS Setup and ME state
      // are in sync, no additional sync is required
      MeStateControlIsInSync = TRUE;
    }
  }
  else
  {
    DEBUG ((EFI_D_ERROR, "[ME] Invalid parameters (MeSetStateControl)\n"));
    Status = EFI_INVALID_PARAMETER;
  }
  return Status;
}

/**
  This function extracts ME RC Configuration data on setup enter

  @param[in out] MeRcConfiguration - ME FW RC Configuration Data

  @return EFI Status code
  @retval EFI_SUCCESS Operation completed successfully
**/
EFI_STATUS EFIAPI MeExtractAmtConfigOnSetupEnter (
  IN OUT ME_RC_CONFIGURATION *MeRcConfiguration
)
{
  EFI_STATUS Status;
  UINT32     MeMode = ME_MODE_FAILED;

  DEBUG((EFI_D_INFO, "[ME] MeExtractAmtConfigOnSetupEnter called...\n"));

  if(NULL != MeRcConfiguration)
  {
    MeRcConfiguration->MeType = MeTypeGet();
    MeTypeShowDebug(L"Set MeType in MeOnEnterSetup");

    Status = MeSetMeStateControl(MeRcConfiguration);
    if(EFI_ERROR(Status))
    {
      DEBUG ((EFI_D_ERROR, "[ME] ERROR: Cannot Set MeStateControl\n"));
    }

    // Update MeFirmwareMode in setup to correct show/hide operations
    HeciGetMeMode (&MeMode);
    MeRcConfiguration->MeFirmwareMode = (UINT8) MeMode;
    DEBUG((EFI_D_INFO, "[Me] MeFirmwareMode is set to %d\n", MeRcConfiguration->MeFirmwareMode));
  }
  else
  {
    DEBUG ((EFI_D_ERROR, "[ME] Invalid parameters (MeExtractAmtConfigOnSetupEnter)\n"));
    Status = EFI_INVALID_PARAMETER;
  }
  return Status;
}

/**
  This function routes ME RC Configuration data

  @param[in out] MeRcConfiguration - ME FW RC Configuration Data
  @param[in out] pMeResetRequested - Indicates whether reset was requested or not

  @return EFI Status code
  @retval EFI_SUCCESS Operation completed successfully
**/
EFI_STATUS EFIAPI MeRouteAmtAdvancedConfig (
  IN OUT ME_RC_CONFIGURATION *MeRcConfiguration,
  IN OUT BOOLEAN             *pMeResetRequested
)
{
  EFI_STATUS          Status;
  ME_POLICY_PROTOCOL  *MePolicy = NULL;
  AMT_POLICY_PROTOCOL *DxeAmtPolicy = NULL;
  SPS_POLICY_PROTOCOL *SpsPolicy = NULL;
  HECI_FWS_REGISTER   HeciFwsReg;
  BOOLEAN             MeResetRequested = FALSE;

  DEBUG((EFI_D_INFO, "[ME] MeRouteAmtAdvancedConfig called...\n"));

  if((NULL != MeRcConfiguration) && (NULL != pMeResetRequested))
  {
    Status = gBS->LocateProtocol (&gDxeMePolicyGuid, NULL, &MePolicy);
    ASSERT_EFI_ERROR (Status);

    if(!EFI_ERROR(Status))
    {
      Status = UpdateDxeMePolicy(MePolicy);
      if(EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot update ME policy (status: %r)\n", Status));
      }
      else
      {
        MeResetRequested = TRUE;
      }
    }
    else
    {
      MeResetRequested = TRUE;
    }

    Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, (VOID **) &DxeAmtPolicy);
    if(!EFI_ERROR (Status))
    {
      UpdateDxeAmtPolicy(DxeAmtPolicy);
    }
    else
    {
      MeResetRequested = TRUE;
    }

    // Update SPS Policy to manage AMT settings stored there.
    Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &SpsPolicy);
    ASSERT_EFI_ERROR(Status);

    if(!EFI_ERROR(Status))
    {
      Status = SpsPolicy->UpdateSpsPolicyFromSetup(SpsPolicy, MeRcConfiguration, &MeResetRequested);
      if(EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot update SPS policy (status: %r)\n", Status));
        MeResetRequested = TRUE;
      }
    }
    else
    {
      MeResetRequested = TRUE;
    }

    HeciFwsReg.ul = HeciPciRead32(R_ME_HFS);
    switch (HeciFwsReg.r.CurrentState)
    {
      case ME_STATE_RECOVERY:    // Fall through
      case ME_STATE_NORMAL:      // Fall through
      case ME_STATE_TRANSITION:
        break;

      default:
        //
        // Do not send any HECI requests, no chance ME responds.
        //
        DEBUG((EFI_D_INFO, "[ME] Do not send any HECI requests, no chance ME responds\n"));
        MeResetRequested = TRUE;
    }

    if (MeIsAfterEndOfPost() || EFI_ERROR(Status) || MeResetRequested)
    {
      *pMeResetRequested = TRUE;
    }
    else
    {
      *pMeResetRequested = FALSE;
    }
  }
  else
  {
    DEBUG ((EFI_D_ERROR, "[ME] Invalid parameters (MeRouteAmtAdvancedConfig)\n"));
    Status = EFI_INVALID_PARAMETER;
  }
  return Status;
}

#endif // AMT_SUPPORT
