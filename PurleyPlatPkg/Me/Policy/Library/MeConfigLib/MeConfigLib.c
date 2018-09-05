/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeConfigLib.c

@brief:
  ME Config Lib provides platform defaults for ME configuration.

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Config/MeConfig.h>
#include <Library/MeTypeLib.h>
#include <Library/MeConfigLib.h>
#include <HeciRegs.h>
#include <MeAccess.h>


/**
  Configure Intel ME default settings.

  @param[io] pMeConfig         The pointer to get ME Policy PPI instance

  @retval EFI_SUCCESS          The defaults initialized.
**/
EFI_STATUS EFIAPI
MeConfigDefaults(
  OUT  ME_CONFIG    *pMeConfig)
{
  pMeConfig->Revision      = ME_CONFIG_REVISION;
  pMeConfig->HeciTimeouts  = 1;
  pMeConfig->DidInitStat = 0;
  pMeConfig->DisableCpuReplacedPolling = 0;
  pMeConfig->SendDidMsg = 1;
  pMeConfig->DisableHeciRetry = 0;
  pMeConfig->DisableMessageCheck = 0;
  pMeConfig->SkipMbpHob = 0;
  pMeConfig->HeciCommunication1 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC1, ME_DEVICE_AUTO,
#if AMT_SUPPORT
                                                            MeTypeIsCorporateAmt()
#else
                                                            FALSE
#endif // AMT_SUPPORT
                                                            );
  pMeConfig->HeciCommunication2 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC2, ME_DEVICE_AUTO,
#if AMT_SUPPORT
                                                            MeTypeIsCorporateAmt()
#else
                                                            FALSE
#endif // AMT_SUPPORT
                                                            );
  pMeConfig->HeciCommunication3 = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_HEC3, ME_DEVICE_AUTO,
#if AMT_SUPPORT
                                                            MeTypeIsCorporateAmt()
#else
                                                            FALSE
#endif // AMT_SUPPORT
                                                            );
  pMeConfig->IderDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_IDER, ME_DEVICE_AUTO,
#if AMT_SUPPORT
                                                            MeTypeIsCorporateAmt()
#else
                                                            FALSE
#endif // AMT_SUPPORT
                                                            );
  pMeConfig->KtDeviceEnable = MeConfigDeviceAutoResolve(ME_AUTO_CONFIG_SOL, ME_DEVICE_AUTO,
#if AMT_SUPPORT
                                                            MeTypeIsCorporateAmt()
#else
                                                            FALSE
#endif // AMT_SUPPORT
                                                            );
  pMeConfig->HostResetNotification = 1;
  pMeConfig->HsioMessaging = 1;

  pMeConfig->EndOfPostMessage = EOP_SEND_IN_DXE;
  pMeConfig->DisableD0I3SettingForHeci = 0;

  pMeConfig->MeFwDownGrade = 0;
  pMeConfig->MeLocalFwUpdEnabled = 0;
  pMeConfig->OsPtpAware = 1;
  pMeConfig->HidePttFromOS = 0;
  pMeConfig->MeJhiSupport = 0;
  pMeConfig->CoreBiosDoneEnabled = 1;

  return EFI_SUCCESS;
}


/**
  Print ME Config structure

  @param[in] pMeConfig Pointer to the ME config structure
**/
VOID EFIAPI
MeConfigPrint(
  IN     ME_CONFIG  *pMeConfig)
{
DEBUG_CODE_BEGIN();
  DEBUG((DEBUG_INFO, "ME Config Revision        : %d\n", pMeConfig->Revision));
  ASSERT(pMeConfig->Revision == ME_CONFIG_REVISION);

  DEBUG((DEBUG_INFO, " HeciTimeouts             : %d\n", pMeConfig->HeciTimeouts));
  DEBUG((DEBUG_INFO, " DidInitStat              : 0x%X\n", pMeConfig->DidInitStat));
  DEBUG((DEBUG_INFO, " DisableCpuReplacedPolling: %d\n", pMeConfig->DisableCpuReplacedPolling));
  DEBUG((DEBUG_INFO, " DisableHeciRetry         : %d\n", pMeConfig->DisableHeciRetry));
  DEBUG((DEBUG_INFO, " DisableMessageCheck      : %d\n", pMeConfig->DisableMessageCheck));
  DEBUG((DEBUG_INFO, " SkipMbpHob               : %d\n", pMeConfig->SkipMbpHob));
  DEBUG((DEBUG_INFO, " HeciCommunication1       : %d\n", pMeConfig->HeciCommunication1));
  DEBUG((DEBUG_INFO, " HeciCommunication2       : %d\n", pMeConfig->HeciCommunication2));
  DEBUG((DEBUG_INFO, " HeciCommunication3       : %d\n", pMeConfig->HeciCommunication3));
  DEBUG((DEBUG_INFO, " KtDeviceEnable           : %d\n", pMeConfig->KtDeviceEnable));
  DEBUG((DEBUG_INFO, " IderDeviceEnable         : %d\n", pMeConfig->IderDeviceEnable));
  DEBUG((DEBUG_INFO, " HostResetNotification    : %d\n", pMeConfig->HostResetNotification));
  DEBUG((DEBUG_INFO, " HsioMessaging            : %d\n", pMeConfig->HsioMessaging));
  DEBUG((DEBUG_INFO, " EndOfPostMessage         : %d\n", pMeConfig->EndOfPostMessage));
  DEBUG((DEBUG_INFO, " DisableD0I3SettingForHeci: %d\n", pMeConfig->DisableD0I3SettingForHeci));
  DEBUG((DEBUG_INFO, " MeFwDownGrade            : %d\n", pMeConfig->MeFwDownGrade));
  DEBUG((DEBUG_INFO, " MeLocalFwUpdEnabled      : %d\n", pMeConfig->MeLocalFwUpdEnabled));
  DEBUG((DEBUG_INFO, " OsPtpAware               : %d\n", pMeConfig->OsPtpAware));
  DEBUG((DEBUG_INFO, " HidePttFromOS            : %d\n", pMeConfig->HidePttFromOS));
  DEBUG((DEBUG_INFO, " MeJhiSupport             : %d\n", pMeConfig->MeJhiSupport));
DEBUG_CODE_END();
}


/**

  @brief
  Function resolves AutoSettings for particular configuration entry

  @param[in] Device             Device to resolve auto-configuration
  @param[in] ConfigToResolve    Auto-configuration to be resolved
  @param[in] MeMode             ME Mode
  @param[in] MeIsCorporateAmt   Is Corporate AMT is present at the platform?

  @retval resolved auto-configuration

**/
UINT32
MeConfigDeviceAutoResolve(
  IN     ME_AUTO_CONFIG Device,
  IN     UINT32         ConfigToResolve,
  IN     BOOLEAN        MeIsCorporateAmt)
{
#if AMT_SUPPORT
  HECI_FWS_REGISTER     Mefs1;
#endif
  
  if (ConfigToResolve != ME_DEVICE_AUTO)
  {
    DEBUG((DEBUG_INFO, "[ME Policy] Not Auto-configuration (%d) passed for device %d\n",
                       ConfigToResolve, Device));
    return ConfigToResolve;
  }
  //
  // If DFX all ME devices should be enabled
  //
  if (MeTypeIsDfx())
  {
    return ME_DEVICE_ENABLED;
  }
  //
  // If ME is desibled all ME devices should be disabled
  //
  if (MeTypeIsDisabled())
  {
    return ME_DEVICE_DISABLED;
  }
  switch (Device)
  {
    case ME_AUTO_CONFIG_HEC1:
#if SPS_SUPPORT
      if (MeTypeIsSps())
      {
        return ME_DEVICE_ENABLED;
      }
#endif
#if AMT_SUPPORT
      if (MeTypeIsAmt())
      {
        Mefs1.ul = HeciPciRead32(R_FWSTATE);
        if ((Mefs1.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_JMPR) ||
            (Mefs1.r.MeOperationMode == ME_OPERATION_MODE_SOFT_TEMP_DISABLE))
        {
          return ME_DEVICE_DISABLED;
        }
        return ME_DEVICE_ENABLED;
      }
#endif
      break;
      
    case ME_AUTO_CONFIG_HEC2:
#if SPS_SUPPORT
      if (MeTypeIsSps())
      {
        if (MeTypeIsSpsNm())
        {
          return ME_DEVICE_ENABLED;
        }
        return ME_DEVICE_DISABLED;
      }
#endif
#if AMT_SUPPORT
      if (MeTypeIsAmt())
      {
        return ME_DEVICE_DISABLED;
      }
#endif
      break;
      
    case ME_AUTO_CONFIG_HEC3:
#if SPS_SUPPORT
      if (MeTypeIsSps())
      {
        return ME_DEVICE_ENABLED;
      }
#endif
#if AMT_SUPPORT
      if (MeTypeIsAmt())
      {
        return ME_DEVICE_DISABLED;
      }
#endif
      break;
      
    case ME_AUTO_CONFIG_SOL:
//APTIOV_SERVER_OVERRIDE_RC_START : Enable IDE-R
    case ME_AUTO_CONFIG_IDER:
//APTIOV_SERVER_OVERRIDE_RC_END : Enable IDE-R
#if SPS_SUPPORT
      if (MeTypeIsSps())
      {
        return ME_DEVICE_DISABLED;
      }
#endif
#if AMT_SUPPORT
      if (MeTypeIsAmt())
      {
        if (MeIsCorporateAmt)
        {
          Mefs1.ul = HeciPciRead32(R_FWSTATE);
          if ((Mefs1.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_JMPR) ||
              (Mefs1.r.MeOperationMode == ME_OPERATION_MODE_SOFT_TEMP_DISABLE))
          {
            return ME_DEVICE_DISABLED;
          }
          return ME_DEVICE_ENABLED;
        }
        return ME_DEVICE_DISABLED;
      }
#endif // AMT_SUPPORT
      break;
//APTIOV_SERVER_OVERRIDE_RC_START : Enable IDE-R
#if 0
    case ME_AUTO_CONFIG_IDER:
      return ME_DEVICE_DISABLED;
#endif
//APTIOV_SERVER_OVERRIDE_RC_END
      
    default:
      DEBUG((DEBUG_WARN, "[ME Policy] WARNING: Auto-configuration passed for unrecognised device %d\n", Device));
      return ME_DEVICE_DISABLED;
  }

  DEBUG((DEBUG_WARN, "[ME Policy] WARNING: Unexpected ME type (MEFS1: %08X)\n", HeciPciRead32(R_FWSTATE)));
  return ME_DEVICE_DISABLED;
}

