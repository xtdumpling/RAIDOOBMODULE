//***************************************************************************
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug fixed:   Modify Me version strings, remove the "Manufacturer ID" string.
//    Auditor:     Jimmy Chiu
//    Date:        May/25/2017
//
//***************************************************************************
/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@file

  Copyright (c) 2016, Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Name:
    SpsSetup.c

  Description:
    This file contains functions that update ME related strings in BIOS setup.

**/
#if defined(SPS_SUPPORT) && SPS_SUPPORT

#include "SetupPlatform.h"
#include <Library/PciLib.h>
#include <Library/HiiLib.h>
#include <Protocol/HeciProtocol.h>
#include "Protocol/MeSpsPolicyProtocol.h"
#include <Sps.h>
#include <Guid/SpsInfoHobGuid.h>
#include "Library/MeTypeLib.h"
#include "MeAccess.h"
#include "MeState.h"

#include <Library/MeSpsPolicyUpdateDxeLib.h>
#include <Library/SetupLib.h>
#include <Library/DxeMePolicyUpdateLib.h>
#include <Library/PttHeciLib.h>
#include "Library/HeciCoreLib.h"

#define ME_SPS_INFO_MAX_STR_SIZE 0x200

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
VOID MeInitSpsSetupStrings(
  OUT EFI_HII_HANDLE HiiAdvancedHandle,
  OUT EFI_HII_HANDLE HiiMainHandle
  )
{
  EFI_STATUS            Status;
  HECI_PROTOCOL         *pHeciProtocol;
  SPS_MEFS1             MeFs1;
  SPS_MEFS2             MeFs2;
  UINT32                RspLen;
  SPS_FEATURE_SET       FeatureSet;
  CHAR16                StrBuf[ME_SPS_INFO_MAX_STR_SIZE];
  CHAR16*               MeTypeString;

  union {
    MKHI_MSG_GET_FW_VERSION_REQ      FwVerReq;
    MKHI_MSG_GET_FW_VERSION_RSP      FwVerRsp;
    SPS_MSG_GET_MEBIOS_INTERFACE_REQ MeBiosIfReq;
    SPS_MSG_GET_MEBIOS_INTERFACE_RSP MeBiosIfRsp;
  } HeciMsg;

  DEBUG((EFI_D_INFO, "[ME] MeInitSpsInfo called...\n"));
// APTIOV_SERVER_OVERRIDE_RC_START
  if(NULL != HiiAdvancedHandle)// && (NULL != HiiMainHandle))
// APTIOV_SERVER_OVERRIDE_RC_END
  {
    //
    // Init MEFS#1 info in setup string database
    //
    MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
    if (MeFs1.UInt32 == 0xFFFFFFFF) {
      MeFs1.UInt32 = GetMeFs1FromHob(MeFs1.UInt32);
    }
    DEBUG((EFI_D_INFO, "[ME] MEFS1: 0x%08X\n", MeFs1.UInt32));
    UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"0x%08X", MeFs1.UInt32);
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWSTS1_VALUE), StrBuf, NULL);

    //
    // Init MEFS#2 info in setup string database
    //
    MeFs2.UInt32 = HeciPciRead32(SPS_REG_MEFS2);
    DEBUG((EFI_D_INFO, "[ME] MEFS2: 0x%08X\n", MeFs2.UInt32));
    UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"0x%08X", MeFs2.UInt32);
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWSTS2_VALUE), StrBuf, NULL);

    //
    // Update ME Current State in setup strings database
    //
    switch (MeFs1.Bits.CurrentState)
    {
      case MEFS1_CURSTATE_RESET:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Reset");
        break;

      case MEFS1_CURSTATE_INIT:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Initializing");
        break;

      case MEFS1_CURSTATE_RECOVERY:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Recovery");
        break;

      case MEFS1_CURSTATE_DISABLED:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Disabled");
        break;

      case MEFS1_CURSTATE_NORMAL:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Operational");
        break;

      case MEFS1_CURSTATE_TRANSITION:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Transitioning");
        break;


      default:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Unknown(%d)", MeFs1.Bits.CurrentState);
        break;
    }
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWCURSTE_VALUE), StrBuf, NULL);

    //
    // Update ME Error Code in setup strings database
    //
    switch (MeFs1.Bits.ErrorCode)
    {
      case MEFS1_ERRCODE_NOERROR:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"No Error");
        break;

      case MEFS1_ERRCODE_UNKNOWN:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Uncategorized Failure");
        break;

      case MEFS1_ERRCODE_DISABLED:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Disabled");
        break;

      case MEFS1_ERRCODE_IMAGE_FAIL:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Image Failure");
        break;

      default:
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Unknown(%d)", MeFs1.Bits.ErrorCode);
        break;
    }
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWERRCODE_VALUE), StrBuf, NULL);

    //
    // Update ME FW Type in setup strings database
    //
    MeTypeString = HiiGetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FW_TYPE_SPS), NULL);
    HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FW_TYPE_VALUE), MeTypeString, NULL);

    switch (MeFs1.Bits.CurrentState)
    {
      case MEFS1_CURSTATE_RECOVERY:
        //
        // Update Recovery Cause in setup strings database
        //
        switch (MeFs2.Bits.RecoveryCause)
        {
          case MEFS2_RCAUSE_RCVJMPR:
            UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"ME Recovery Jumper");
            break;
          case MEFS2_RCAUSE_MFGJMPR:
            UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Manufacturing Jumper");
            break;
          case MEFS2_RCAUSE_IPMICMD:
            UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"IPMI Command");
            break;
          case MEFS2_RCAUSE_FLASHCFG:
            UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Flash Conf. Error");
            break;
          case MEFS2_RCAUSE_MEERROR:
            UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"ME Internal Error");
            break;
          case MEFS2_RCAUSE_UMAERROR:
            UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"UMA Memory Error");
            break;
          default:
            UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"Unknown(%d)", MeFs2.Bits.RecoveryCause);
            break;
        }
        HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_RCVCAUSE_VALUE), StrBuf, NULL);
        // Fall Through
      case MEFS1_CURSTATE_NORMAL:     // Fall Through
      case MEFS1_CURSTATE_TRANSITION:
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

    //
    // Send MKHI_GET_FW_VERSION request to ME
    //
    DEBUG((EFI_D_INFO, "[ME] Sending MKHI_GET_FW_VERSION request to ME\n"));

    gBS->SetMem(&HeciMsg.FwVerReq, sizeof(HeciMsg.FwVerReq), 0);
    HeciMsg.FwVerReq.Mkhi.Fields.GroupId = MKHI_GRP_GEN;
    HeciMsg.FwVerReq.Mkhi.Fields.Command = MKHI_CMD_GET_FW_VERSION;
    RspLen = sizeof(HeciMsg.FwVerRsp);

    Status = pHeciProtocol->SendwACK(
      HECI1_DEVICE,
      (UINT32*)&HeciMsg.FwVerReq,
      sizeof(HeciMsg.FwVerReq),
      &RspLen,
      SPS_CLIENTID_BIOS,
      SPS_CLIENTID_ME_MKHI
      );

    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot send MKHI_GET_FW_VERSION (status: %r)\n", Status));
    }
    else if(!((HeciMsg.FwVerRsp.Mkhi.Fields.IsResponse) &&
             (MKHI_GRP_GEN == HeciMsg.FwVerReq.Mkhi.Fields.GroupId) &&
             (MKHI_CMD_GET_FW_VERSION == HeciMsg.FwVerReq.Mkhi.Fields.Command)))
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Invalid MKHI_GET_FW_VERSION response (MKHI: 0x%X)\n", HeciMsg.FwVerRsp.Mkhi.Data));
    }
    else
    {
      DEBUG((EFI_D_INFO,
        "[ME] Firmware version is:\n"
        "\tOperational: %02X:%d.%d.%d.%d\n"
        "\tRecovery:    %02X:%d.%d.%d.%d\n",
        (HeciMsg.FwVerRsp.Act.Major >> 8), (HeciMsg.FwVerRsp.Act.Major & 0x00FF), HeciMsg.FwVerRsp.Act.Minor, HeciMsg.FwVerRsp.Act.Patch, HeciMsg.FwVerRsp.Act.Build,
        (HeciMsg.FwVerRsp.Rcv.Major >> 8), (HeciMsg.FwVerRsp.Rcv.Major & 0x00FF), HeciMsg.FwVerRsp.Rcv.Minor, HeciMsg.FwVerRsp.Rcv.Patch, HeciMsg.FwVerRsp.Rcv.Build));

#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT
      UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"%d.%d.%d.%d",
          (HeciMsg.FwVerRsp.Act.Major & 0x00FF), HeciMsg.FwVerRsp.Act.Minor, HeciMsg.FwVerRsp.Act.Patch, HeciMsg.FwVerRsp.Act.Build);
#else
      UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"%02X:%d.%d.%d.%d",
          (HeciMsg.FwVerRsp.Act.Major >> 8), (HeciMsg.FwVerRsp.Act.Major & 0x00FF), HeciMsg.FwVerRsp.Act.Minor, HeciMsg.FwVerRsp.Act.Patch, HeciMsg.FwVerRsp.Act.Build);
#endif

      HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWVER_OPERATIONAL_VALUE), StrBuf, NULL);

      //
      // Update ME firmware info in platform information menu
      //
      // APTIOV_SERVER_OVERRIDE_RC_START : Form not provided in AptioV
      //HiiSetString(HiiMainHandle, STRING_TOKEN(STR_ME_REVISION_VALUE), StrBuf, NULL);
      // APTIOV_SERVER_OVERRIDE_RC_END : Form not provided in AptioV

#if defined SMCPKG_SUPPORT && SMCPKG_SUPPORT
      UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"%d.%d.%d.%d",
        (HeciMsg.FwVerRsp.Rcv.Major & 0x00FF), HeciMsg.FwVerRsp.Rcv.Minor, HeciMsg.FwVerRsp.Rcv.Patch, HeciMsg.FwVerRsp.Rcv.Build);
#else
      UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"%02X:%d.%d.%d.%d",
        (HeciMsg.FwVerRsp.Rcv.Major >> 8), (HeciMsg.FwVerRsp.Rcv.Major & 0x00FF), HeciMsg.FwVerRsp.Rcv.Minor, HeciMsg.FwVerRsp.Rcv.Patch, HeciMsg.FwVerRsp.Rcv.Build);
#endif

      HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWVER_RECOVERY_VALUE), StrBuf, NULL);

      //
      // ME in dual-image configuration provides the version of the backup image yet
      //
      if (RspLen > sizeof(HeciMsg.FwVerRsp) - sizeof(HeciMsg.FwVerRsp.Bkp))
      {
        DEBUG((EFI_D_ERROR,
            "\tBackup:      %02X:%d.%d.%d.%d\n",
            (HeciMsg.FwVerRsp.Bkp.Major >> 8), (HeciMsg.FwVerRsp.Bkp.Major & 0x00FF), HeciMsg.FwVerRsp.Bkp.Minor, HeciMsg.FwVerRsp.Bkp.Patch, HeciMsg.FwVerRsp.Bkp.Build));

        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"%02X:%d.%d.%d.%d",
            (HeciMsg.FwVerRsp.Bkp.Major >> 8), (HeciMsg.FwVerRsp.Bkp.Major & 0x00FF), HeciMsg.FwVerRsp.Bkp.Minor, HeciMsg.FwVerRsp.Bkp.Patch, HeciMsg.FwVerRsp.Bkp.Build);

        HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_ME_FWVER_BACKUP_VALUE), StrBuf, NULL);
      }

      //
      // Send SPS_CMD_GET_MEBIOS_INTERFACE request to ME
      //
      DEBUG((EFI_D_INFO, "[ME] Sending SPS_CMD_GET_MEBIOS_INTERFACE request to ME\n"));

      gBS->SetMem(&HeciMsg.MeBiosIfReq, sizeof(HeciMsg.MeBiosIfReq), 0);
      HeciMsg.MeBiosIfReq.Command.Data = SPS_CMD_GET_MEBIOS_INTERFACE_REQ;
      RspLen = sizeof(HeciMsg.MeBiosIfRsp);

      Status = pHeciProtocol->SendwACK(
        HECI1_DEVICE,
        (UINT32*)&HeciMsg.MeBiosIfReq,
        sizeof(HeciMsg.MeBiosIfReq),
        &RspLen,
        SPS_CLIENTID_BIOS,
        SPS_CLIENTID_ME_SPS
        );

      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot send SPS_GET_MEBIOS_INTERFACE (status: %r)\n", Status));
      }
      else if (HeciMsg.MeBiosIfRsp.Command.Data != SPS_CMD_GET_MEBIOS_INTERFACE_RSP)
      {
        DEBUG((EFI_D_ERROR, "[ME] ERROR: Invalid SPS_GET_MEBIOS_INTERFACE response command (command: 0x%x)\n", HeciMsg.MeBiosIfRsp.Command));
      }
      else if ((RspLen != sizeof(HeciMsg.MeBiosIfRsp)) &&
               (RspLen != (sizeof(HeciMsg.MeBiosIfRsp) - sizeof(HeciMsg.MeBiosIfRsp.FeatureSet2))))
      {
        DEBUG((EFI_D_ERROR, "[ME] ERROR: Invalid SPS_GET_MEBIOS_INTERFACE response (length %d)\n", RspLen));
      }
      else
      {
        //
        // Fill the ME firmware SKU info in setup
        //
        FeatureSet.Data.Set1 = HeciMsg.MeBiosIfRsp.FeatureSet;
        FeatureSet.Data.Set2 = HeciMsg.MeBiosIfRsp.FeatureSet2;
        UnicodeSPrint(StrBuf, ME_SPS_INFO_MAX_STR_SIZE, L"SiEn\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
          FeatureSet.Bits.NodeManager                 ? L"NM\n" : L"",
          FeatureSet.Bits.PeciProxy                   ? L"PECIProxy\n" : L"",
          FeatureSet.Bits.ICC                         ? L"ICC\n" : L"",
          FeatureSet.Bits.MeStorageServices           ? L"MeStorageServices\n" : L"",
          FeatureSet.Bits.BootGuard                   ? L"BootGuard\n" : L"",
          FeatureSet.Bits.PTT                         ? L"PTT\n" : L"",
          FeatureSet.Bits.OemDefinedCpuDebugPolicy    ? L"OemDefinedCpuDebugPolicy\n" : L"",
          FeatureSet.Bits.ResetSuppression            ? L"ResetSuppression\n" : L"",
          FeatureSet.Bits.PmBusProxy                  ? L"PmBusProxy\n" : L"",
          FeatureSet.Bits.CpuHotPlug                  ? L"CpuHotPlug\n" : L"",
          FeatureSet.Bits.MicProxy                    ? L"MicProxy\n" : L"",
          FeatureSet.Bits.MctpProxy                   ? L"MctpProxy\n" : L"",
          FeatureSet.Bits.ThermalReportVolumeAirflow  ? L"ThermalReport\n" : L"",
          FeatureSet.Bits.SocThermalReporting         ? L"SocThermalReporting\n" : L"",
          FeatureSet.Bits.DualBIOSSupport             ? L"DualBiosImage\n" : L"",
          FeatureSet.Bits.MPHYSurvivabilityProgramming? L"HSIO\n" : L"",
          FeatureSet.Bits.PECIOverDMI                 ? L"PECIOverDMI\n" : L"",
          FeatureSet.Bits.PCHDebug                    ? L"PCHDebug\n" : L"",
          FeatureSet.Bits.PowerThermalUtilitySupport  ? L"PowerThermalUtility\n" : L"",
          FeatureSet.Bits.FiaMuxConfiguration         ? L"FiaMuxConfiguration\n" : L"",
          FeatureSet.Bits.PchThermalSensorInit        ? L"PCHThermalSensorInit\n" : L"",
          FeatureSet.Bits.DeepSx                      ? L"DeepSx\n" : L"",
          FeatureSet.Bits.DualMeFwImage               ? L"DualMeImage\n" : L"",
          FeatureSet.Bits.DirectFwUpdate              ? L"DirectMeUpdate\n" : L"",
          FeatureSet.Bits.MctpInfrastructure          ? L"MctpInfrastructure\n" : L"",
          FeatureSet.Bits.CUPS                        ? L"CUPS\n" : L"",
          FeatureSet.Bits.FlashDescRegionVerification ? L"FlashDescRegionVerification\n" : L"",
          FeatureSet.Bits.TurboStateLimiting          ? L"TurboStateLimiting\n" : L"",
          FeatureSet.Bits.TelemetryHub                ? L"TelemetryHub\n" : L"",
          FeatureSet.Bits.MeShutdown                  ? L"MeShutdown\n" : L"",
          FeatureSet.Bits.ASA                         ? L"ASA" : L"",
          FeatureSet.Bits.WarmResetNotificationSubFlow? L"WarmResetNotificationSubFlow" : L""
          );
        HiiSetString(HiiAdvancedHandle, STRING_TOKEN(STR_SPS_FEATURES_VALUE), StrBuf, NULL);
      }
    }
  }
}

/**
  This function extracts ME RC Configuration data on Setup enter

  @param[in out] MeRcConfiguration - ME FW RC Configuration Data

  @return EFI Status code
  @retval EFI_SUCCESS Operation completed successfully
**/
EFI_STATUS EFIAPI MeExtractSpsConfigOnSetupEnter(
  IN OUT ME_RC_CONFIGURATION *MeRcConfiguration
  )
{
  BOOLEAN    MePttState;
  UINT32     MeMode;
  
  DEBUG((DEBUG_INFO, "[ME] MeExtractSpsConfigOnSetupEnter called...\n"));
  
  if (NULL == MeRcConfiguration)
  {
    ASSERT(MeRcConfiguration != NULL);
    return EFI_INVALID_PARAMETER;
  }
  
  MeRcConfiguration->MeType = MeTypeGet();
  MeTypeShowDebug(L"Set MeType on SetupEnter");

  // Update MeFirmwareMode in setup to correct show/hide operations
  MeMode = ME_MODE_FAILED;
  HeciGetMeMode(&MeMode);
  MeRcConfiguration->MeFirmwareMode = (UINT8)MeMode;
  DEBUG((DEBUG_INFO, "[ME] MeFirmwareMode is set to %d\n", MeRcConfiguration->MeFirmwareMode));
  
  MePttState = FALSE;
  PttHeciGetCapability(&MePttState);
  MeRcConfiguration->MePttSupported = (UINT8)MePttState;
  
  MePttState = FALSE;
  if (MeRcConfiguration->MePttSupported)
  {
    PttHeciGetState(&MePttState);
  }
  MeRcConfiguration->MePttEnabled = (UINT8)MePttState;
  
  DEBUG((DEBUG_INFO, "[ME] PTT supported/enabled: %d/%d\n",
                     MeRcConfiguration->MePttSupported, MeRcConfiguration->MePttEnabled));
  return EFI_SUCCESS;
}


/**
  This function routes ME RC Configuration data

  @param[in out] MeRcConfiguration - ME FW RC Configuration Data
  @param[in out] pMeResetRequested - Indicates whether reset was requested or not

  @return EFI Status code
  @retval EFI_SUCCESS Operation completed successfully
**/
EFI_STATUS EFIAPI MeRouteSpsAdvancedConfig(
  IN OUT ME_RC_CONFIGURATION *MeRcConfiguration,
  IN OUT BOOLEAN             *pMeResetRequested
  )
{
  EFI_STATUS            Status;
  SPS_POLICY_PROTOCOL   *SpsPolicy = NULL;
  ME_POLICY_PROTOCOL    *MePolicy = NULL;
  SPS_MEFS1             MeFs1;
  BOOLEAN               MeResetRequested = FALSE;
  // APTIOV_SERVER_OVERRIDE_RC_START : WA - ADR_COMPLETE signal high after "Save and reset" in setup
  ME_CONFIG             MeConfigOrg;
  // APTIOV_SERVER_OVERRIDE_RC_END : WA - ADR_COMPLETE signal high after "Save and reset" in setup

  DEBUG((DEBUG_INFO, "[ME] MeRouteSpsAdvancedConfig called...\n"));

  if((NULL != MeRcConfiguration) && (NULL != pMeResetRequested))
  {
    Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &SpsPolicy);
    ASSERT_EFI_ERROR(Status);

    if(!EFI_ERROR(Status))
    {
      Status = SpsPolicy->UpdateSpsPolicyFromSetup(SpsPolicy, MeRcConfiguration, &MeResetRequested);
      if(EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[ME] ERROR: Cannot update SPS policy (%r)\n", Status));
        MeResetRequested = TRUE;
      }
    }
    else
    {
      MeResetRequested = TRUE;
    }

    Status = gBS->LocateProtocol (&gDxeMePolicyGuid, NULL, &MePolicy);
    ASSERT_EFI_ERROR(Status);

    if(!EFI_ERROR(Status))
    {
      // APTIOV_SERVER_OVERRIDE_RC_START : WA - ADR_COMPLETE signal high after "Save and reset" in setup
      CopyMem(&MeConfigOrg, &MePolicy->MeConfig, sizeof(ME_CONFIG));
      // APTIOV_SERVER_OVERRIDE_RC_END : WA - ADR_COMPLETE signal high after "Save and reset" in setup
      Status = UpdateDxeMePolicy(MePolicy);
      if(EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[ME] ERROR: Cannot update ME policy (%r)\n", Status));
      }
      else
      {
        // APTIOV_SERVER_OVERRIDE_RC_START : WA - ADR_COMPLETE signal high after "Save and reset" in setup
        if (CompareMem(&MeConfigOrg, &MePolicy->MeConfig, sizeof(ME_CONFIG)) != 0) {
          DEBUG((DEBUG_INFO, "[ME] INFO: ME setup options have changed. Requesting Global reset..\n"));
          MeResetRequested = TRUE;
        } else {
          MeResetRequested = FALSE;
        }
        // APTIOV_SERVER_OVERRIDE_RC_END : WA - ADR_COMPLETE signal high after "Save and reset" in setup
      }
    }
    else
    {
      MeResetRequested = TRUE;
    }

    if (MeRcConfiguration->MePttSupported)
    {
      DEBUG((DEBUG_INFO, "[ME] Save to ME MeRcConfiguration->MePttEnabled: %d\n", MeRcConfiguration->MePttEnabled));
      PttHeciUpdateState(MeRcConfiguration->MePttEnabled);
    }

    MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
    switch (MeFs1.Bits.CurrentState)
    {
      case MEFS1_CURSTATE_RECOVERY:
      case MEFS1_CURSTATE_NORMAL:
      case MEFS1_CURSTATE_TRANSITION:
        break;

      default:
        //
        // Do not send any HECI requests, no chance ME responds.
        //
        DEBUG((DEBUG_INFO, "[ME] Do not send any HECI requests, no chance ME responds\n"));
        MeResetRequested = TRUE;
    }

    if (MeIsAfterEndOfPost())
    {
      *pMeResetRequested = TRUE;
    }
    else
    {
      *pMeResetRequested = FALSE;
      if (EFI_ERROR(Status) || MeResetRequested)
      {
        // In case no policy protocol, request reset
        // any changes will be applied after reset
        *pMeResetRequested = TRUE;
      }
      else if (SpsPolicy && SpsPolicy->AnythingChanged)
      {
        *pMeResetRequested = TRUE;
      }
    }
  }
  else
  {
    DEBUG ((DEBUG_ERROR, "[ME] Invalid parameters (MeRouteSpsAdvancedConfig)\n"));
    Status = EFI_INVALID_PARAMETER;
  }
  return Status;
}

#endif //SPS_SUPPORT
