//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MERcVariable.h

  Data format for Universal Data Structure

**/
#ifndef _ME_RC_CONFIG_DATA_H_
#define _ME_RC_CONFIG_DATA_H_

#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG

// for ME_RC_CONFIGURATION
extern EFI_GUID gEfiMeRcVariableGuid;
#define ME_RC_CONFIGURATION_NAME L"MeRcConfiguration"

#pragma pack(1)
typedef struct {
  //
  // ASF
  //
  UINT8   Asf;
  //
  // WatchDog
  //
  UINT8   WatchDog;
  UINT16  WatchDogTimerBios;
  UINT8   WatchDogOs;
  UINT16  WatchDogTimerOs;
  //
  // Intel AMT
  //
  UINT8   Amt;
  //
  // ME Setup entries - start here
  //
#if ME_TESTMENU_FLAG
  UINT8   MeDidEnabled;                // Whether DRAM Init Done should be sent to ME
  UINT8   MeDidInitStat;               // Override DRAM status sent to ME
  UINT8   HsioMessagingEnabled;
  UINT8   MeEndOfPostEnabled;
  
  UINT8   MeHeci1Enabled;
  UINT8   MeHeci2Enabled;
  UINT8   MeHeci3Enabled;
  UINT8   MeIderEnabled;
  UINT8   MeKtEnabled;
  
  UINT8   DisableD0I3SettingForHeci;
  UINT8   HostResetNotification;
#endif
  
  UINT8   CoreBiosDoneEnabled;
  UINT8   MeFwDowngrade;
  UINT8   LocalFwUpdEnabled;
  UINT8   OsPtpAware;
  
#if SPS_SUPPORT
  UINT16  SpsAltitude;
  UINT16  SpsMctpBusOwner;

  UINT8   PreDidMeResetEnabled;
  
  UINT8   MeHeci1HideInMe;
  UINT8   MeHeci2HideInMe;
  UINT8   MeHeci3HideInMe;

  UINT8   MePttSupported;
  UINT8   MePttEnabled;
  UINT8   NmPwrOptBoot;
  UINT8   NmCores2Disable;
#if ME_TESTMENU_FLAG
  UINT8   MeTimeout;

  UINT8   MeHmrfpoLockEnabled;
  UINT8   MeHmrfpoEnableEnabled;
  
  UINT8   MeGrLockEnabled;
  UINT8   MeGrPromotionEnabled;
  UINT8   BreakRtcEnabled;
  
  UINT8   SpsIccClkOverride;           // Enable overriding default ICC Clock configuration SPS
  UINT8   SpsIccClkSscSetting;

  UINT8   NmPwrOptBootOverride;
  UINT8   NmCores2DisableOverride;

  UINT8   NmPowerMsmtOverride;
  UINT8   NmPowerMsmtSupport;
  UINT8   NmHwChangeOverride;
  UINT8   NmHwChangeStatus;
  UINT8   NmPtuLoadOverride;           // For MROM-less systems only
#endif
#endif // SPS_SUPPORT

  UINT8     MeType;
  UINT8     MeFirmwareMode;
#if defined(AMT_SUPPORT) && AMT_SUPPORT
  UINT8     MeImageType;
  UINT8     MngState;
  UINT8     MdesCapability;
  UINT8     MeIsCorporateAmt;
  UINT8     FWProgress;
  UINT8     RemoteSessionActive;
  UINT8     AmtCiraRequest;
  UINT8     AmtCiraTimeout;
  UINT8     UnConfigureMe;
  UINT8     HideUnConfigureMeConfirm;
  UINT8     MebxDebugMsg;
  UINT8     USBProvision;
  UINT8     AmtbxSelectionScreen;
  UINT8     AmtbxHotKeyPressed;
  UINT8     MebxNonUiTextMode;
  UINT8     MebxUiTextMode;
  UINT8     MebxGraphicsMode;
  UINT8     MeStateControl;
  
#if ME_TESTMENU_FLAG
  UINT8   BiosReflash;
  UINT8   BiosSetup;
  UINT8   BiosPause;
  UINT8   SecureBootExposureToFw;
#endif // ME_TESTMENU_FLAG
  UINT8   MeFirmwareInfo;
#endif // AMT_SUPPORT
  //
  // ME Setup entries - end here <><><><><>
  //

#if defined(ICC_SUPPORT) && ICC_SUPPORT
  UINT8   IccClkOverride;      // Enable overriding default ICC Clock configuration
                               // for AMT
  UINT8   IccProfile;
  UINT8   ShowProfile;
  UINT32  BClkFrequency;
  UINT32  BClkOverride;
#ifndef MAX_UI_CLOCKS
#define MAX_UI_CLOCKS 2
#endif // MAX_UI_CLOCKS
  UINT16 Frequency[MAX_UI_CLOCKS];
  UINT8  Spread[MAX_UI_CLOCKS];
  UINT8  ShowClock[MAX_UI_CLOCKS];
  UINT8  ShowFrequency[MAX_UI_CLOCKS];
  UINT8  ShowSpread[MAX_UI_CLOCKS];
  UINT8  RegLock;
  UINT8  AllowAdvancedOptions;
  UINT8  PlatformType;
#endif // ICC_SUPPORT

} ME_RC_CONFIGURATION;
#pragma pack()

#endif // ME_SUPPORT_FLAG

#endif // __ME_RC_CONFIG_DATA_H__


