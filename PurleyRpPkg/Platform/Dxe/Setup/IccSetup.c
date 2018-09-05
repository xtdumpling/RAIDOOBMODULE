/**@file

@copyright
 Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
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

#if defined(ICC_SUPPORT) && ICC_SUPPORT

#include <Base.h>
#include <PchAccess.h>
#include <Protocol/PchReset.h>
#include <Protocol/HiiConfigAccess.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/SetupLib.h>
#include <Protocol/Wdt.h>
#include "SetupPlatform.h"
#include <IccPlatform.h>
#include <Protocol/IccDataProtocol.h>
#include "IccSetup.h"
#include <IccSetupData.h>
#include "Library/HeciCoreLib.h"
#include "CoreBiosMsg.h"
#include "Library/HeciMsgLib.h"
#include "MeState.h"
#include "Library/MeTypeLib.h"
// APTIOV_SERVER_OVERRIDE_RC_START
//#include <Library/CmosAccessLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#include <CmosMap.h>

// Compiler (.NET 2008) automatically inserts memcpy/memset fn calls
// in certain cases due to optimization.
// Disable optimization to solve compilation error
#ifndef __GNUC__
#pragma optimize( "g", off )
#endif //__GNUC__


#define NUM_USAGES          8
#define ICC_NOT_INITIALIZED 0xFF
#define HZ_TO_10KHZ         10000
#define STRING_WIDTH_100    100

static ICC_GETSET_CLOCK_SETTINGS_REQRSP mSettingsBclk;
static ICC_GETSET_CLOCK_SETTINGS_REQRSP mSettingsPcie;
static ICC_GETSET_CLOCK_SETTINGS_REQRSP mDefaultsBclk;
static ICC_GETSET_CLOCK_SETTINGS_REQRSP mDefaultsPcie;
static ICC_SINGLE_CLK_RANGE_DEF mRangesBclk;
static ICC_SINGLE_CLK_RANGE_DEF mRangesPcie;
static BOOLEAN            mProfileChanged = FALSE;
static BOOLEAN            gClockChanged[MAX_UI_CLOCKS] = {0};
static HECI_PROTOCOL      *mHeci = NULL;

GLOBAL_REMOVE_IF_UNREFERENCED ICC_MBP_DATA               gIccMbpData;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_HII_HANDLE             gHiiHandle;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                    gIccReset = FALSE;

VOID MaybeChangeRegLock (UINT8 RegLock);
VOID MaybeChangeProfile (UINT8 Profile);

////////////////////////////////////////////////////////////////////////////////////////////////
VOID InitString(EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StrRef, CHAR16 *sFormat, ...)
{
  CHAR16      StringBuffer[1024];
  VA_LIST     ArgList;

  //
  // Construct string value.
  //
  VA_START(ArgList,sFormat);
  UnicodeVSPrint (StringBuffer,sizeof(StringBuffer),sFormat,ArgList);
  VA_END(ArgList);

  //
  // Set string
  //
  HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(StrRef), StringBuffer, NULL);

}


/**
  Construct Request String (L"&OFFSET=%x&WIDTH=%x") base on the input Offset and Width.
  If the input RequestString is not NULL, new request will be cat at the end of it. The full
  request string will be constructed and return. Caller is responsible to free it.

  @param RequestString   Current request string.
  @param Offset          Offset of data in Storage.
  @param Width           Width of data.

  @return String         Request string with input Offset and Width.
**/
EFI_STRING
EFIAPI
HiiConstructRequestString (
  IN EFI_STRING      RequestString, OPTIONAL
  IN UINTN           Offset,
  IN UINTN           Width
  )
{
  CHAR16             RequestElement[30];
  UINTN              StringLength;
  EFI_STRING         NewString;

  StringLength = UnicodeSPrint (
                   RequestElement,
                   sizeof (RequestElement),
                   L"&OFFSET=%x&WIDTH=%x",
                   Offset,
                   Width
                   );

  if (RequestString != NULL) {
    StringLength = StringLength + StrLen (RequestString);
  }

  NewString = AllocateZeroPool ((StringLength + 1) * sizeof (CHAR16));

  if (NewString == NULL) {
    return NULL;
  }

  if (RequestString != NULL) {
    StrCatS (NewString, StringLength + 1, RequestString);
    FreePool (RequestString);
  }

  StrCatS (NewString, StringLength + 1, RequestElement);
  return NewString;
}

////////////////////////////////////////////////////////////////////////////////////////////////

VOID
UpdateClockData (
  IN  ICC_GETSET_CLOCK_SETTINGS_REQRSP  ClockSettings,
  IN  UINT8                             ClockNumber,
  OUT ME_RC_CONFIGURATION*              SetupPtr
  )
/*++
--*/
{
  SetupPtr->Frequency[ClockNumber]  = (UINT16) ( (ClockSettings.Freq + (HZ_TO_10KHZ/2)) / HZ_TO_10KHZ);
  SetupPtr->Spread[ClockNumber] = ClockSettings.SscPercent;
}

VOID
ExtractClockData (
  IN  ME_RC_CONFIGURATION*                IccSetupPtr,
  IN  UINT8                               ClockNumber,
  OUT ICC_GETSET_CLOCK_SETTINGS_REQRSP*   ClockSettings
  )
/*++
--*/
{
  ClockSettings->UserFreq    = HZ_TO_10KHZ * IccSetupPtr->Frequency[ClockNumber];
  ClockSettings->SscPercent  = IccSetupPtr->Spread[ClockNumber];
  ClockSettings->SscMode     = ICC_SSC_DOWN;
  ClockSettings->ReqClock    = ClockNumber;
}

BOOLEAN
AreSettingsAtDefault (
  IN ICC_GETSET_CLOCK_SETTINGS_REQRSP *Requested,
  IN UINT8                            ClockNumber
  )
/*++
Routine Description:
  Checks if provided settings are equal to clock's defaults
--*/
{
  ICC_GETSET_CLOCK_SETTINGS_REQRSP Default;
  if (ClockNumber == CLOCK1) {
    CopyMem(&Default, &mDefaultsBclk, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP));
    Default = mDefaultsBclk;
  } else {
    CopyMem(&Default, &mDefaultsPcie, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP));
    Default = mDefaultsPcie;
  }
  if (Requested->Freq        == Default.Freq   &&
      Requested->SscMode     == Default.SscMode     &&
      Requested->SscPercent  == Default.SscPercent ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

EFI_STATUS
IccCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION Action,
  IN EFI_QUESTION_ID KeyValue,
  IN UINT8 Type,
  IN EFI_IFR_TYPE_VALUE *Value,
  OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
  )
{
  //
  // Cannot access ICC menu after EOP.
  //
  if (MeIsAfterEndOfPost()) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_ERROR, "(ICC) callback: key %d(%x), action %x\n", KeyValue, KeyValue, Action));

  switch (KeyValue) {

    case KEY_ICC_FREQ2:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK1] = TRUE;
        Value->u16 = (UINT16)(mDefaultsBclk.Freq / HZ_TO_10KHZ);
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK1] = TRUE;
        if (Value->u16 < (UINT16)(mRangesBclk.Fields.ClkFreqMin)) {
          Value->u16 = (UINT16)(mRangesBclk.Fields.ClkFreqMin);
        } else if (Value->u16 > (UINT16)(mRangesBclk.Fields.ClkFreqMax)) {
          Value->u16 = (UINT16)(mRangesBclk.Fields.ClkFreqMax);
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    case KEY_ICC_FREQ3:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK0] = TRUE;
        Value->u16 = (UINT16)(mDefaultsPcie.Freq / HZ_TO_10KHZ);
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK0] = TRUE;
        if (Value->u16 < (UINT16)(mRangesPcie.Fields.ClkFreqMin)) {
          Value->u16 = (UINT16)(mRangesPcie.Fields.ClkFreqMin);
        } else if (Value->u16 > (UINT16)(mRangesPcie.Fields.ClkFreqMax)) {
          Value->u16 = (UINT16)(mRangesPcie.Fields.ClkFreqMax);
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    case KEY_ICC_SPREAD2:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK1] = TRUE;
        Value->u8 = mDefaultsBclk.SscPercent;
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK1] = TRUE;
        if (Value->u8 > mRangesBclk.Fields.SscSprPercentMax) {
          Value->u8 = (UINT8) mRangesBclk.Fields.SscSprPercentMax;
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    case KEY_ICC_SPREAD3:
      if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        gClockChanged[CLOCK0] = TRUE;
        Value->u8 = mDefaultsPcie.SscPercent;
        return EFI_SUCCESS;
      } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
        gClockChanged[CLOCK0] = TRUE;
        if (Value->u8 > mRangesPcie.Fields.SscSprPercentMax) {
          Value->u8 = (UINT8) mRangesPcie.Fields.SscSprPercentMax;
        }
        return EFI_SUCCESS;
      } else {
        return EFI_UNSUPPORTED;
      }

    default:
      return EFI_UNSUPPORTED;
  }
}

VOID
UpdateSubmenuStrings (
  IN UINT8 SubMenuNumber,
  IN ICC_SINGLE_CLK_RANGE_DEF Ranges
  )
{
  CHAR16  StringBuffer[100];
  CHAR16* TempBuffer;
  UINT8   i;
  UINT8   NeedSlash;
  UINT16  StrClockTitle;
  UINT16  StrFreqRange;
  UINT16  StrMaxSpread;
  UINT16  StrSpreadAdjust;
  CHAR16* ClockUsageName[NUM_USAGES];

  //
  // This function is shard by both clocks. We need to map the string tokens
  // before we use them later in the function. This is intended to avoid unneeded
  // if-else statements. Any new clock support will need to add a new case.
  //
  switch (SubMenuNumber)
  {
    case CLOCK1:
      StrClockTitle   =  STR_ICC_CLOCK1_TITLE;
      StrFreqRange    =  STR_ICC_CLOCK1_FREQ_RANGE_TEXT;
      StrMaxSpread    =  STR_ICC_CLOCK1_MAX_SPREAD_TEXT;
      StrSpreadAdjust =  STR_ICC_CLOCK1_SPREAD_ADJUSTMENTS_TEXT;
    break;

    case CLOCK0:
      StrClockTitle   =  STR_ICC_CLOCK0_TITLE;
      StrFreqRange    =  STR_ICC_CLOCK0_FREQ_RANGE_TEXT;
      StrMaxSpread    =  STR_ICC_CLOCK0_MAX_SPREAD_TEXT;
      StrSpreadAdjust =  STR_ICC_CLOCK0_SPREAD_ADJUSTMENTS_TEXT;
    break;

    default:
      DEBUG ((DEBUG_ERROR, "(ICC) Unsupported submenu detected\n"));
      return;
    break;
  }

  ClockUsageName[0] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_DMI), NULL);
  ClockUsageName[1] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_BCLK), NULL);
  ClockUsageName[2] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_PEG), NULL);
  ClockUsageName[3] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_SATA), NULL);
  ClockUsageName[4] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_RESERVED), NULL);
  ClockUsageName[5] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_RESERVED), NULL);
  ClockUsageName[6] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_USB3), NULL);
  ClockUsageName[7] = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_USE_PCIE), NULL);

  //
  // ClockUsage mask is defined in the SPT ICC FAS. Bit 4 and 5 of ClockUsage is reserved,
  // so let's pretend it's zero for purpose of displaying clock names
  //
  Ranges.Fields.ClockUsage &= ~(BIT5 || BIT4);
  StringBuffer[0] = 0;
  NeedSlash = 0;

  //
  // Clear the title buffer before we process the usages
  //
  InitString(gHiiHandle, STRING_TOKEN(StrClockTitle), L"%s", StringBuffer);

  //
  // Search through the usage mask list and concatenate valid usages to string
  //
  for (i=0; i<NUM_USAGES; i++) {
    if (Ranges.Fields.ClockUsage & (1<<i)) {
      if (NeedSlash == 1) {
        StrCatS(StringBuffer, STRING_WIDTH_100, L"/");
      }
      StrCatS(StringBuffer, STRING_WIDTH_100, ClockUsageName[i]);
      NeedSlash = 1;
    }
  }

  for (i=0; i<NUM_USAGES; i++) {
    FreePool(ClockUsageName[i]);
  }

  //
  // Add "Clock Settings" to the end of the string to complete formatting
  //
  TempBuffer = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_CLOCK_SETTINGS), NULL);
  if (TempBuffer == NULL) return;
  StrCatS(StringBuffer, STRING_WIDTH_100, TempBuffer);
  FreePool(TempBuffer);

  //
  // Copy formatted strings into clock settings menu
  //
  InitString(gHiiHandle, STRING_TOKEN(StrClockTitle), L"%s", StringBuffer);

  //
  // Frequency Range
  //
  InitString(gHiiHandle, STRING_TOKEN(StrFreqRange), L"%d.%02d - %d.%02d MHz",
    Ranges.Fields.ClkFreqMin/100, Ranges.Fields.ClkFreqMin%100, Ranges.Fields.ClkFreqMax/100, Ranges.Fields.ClkFreqMax%100);

  //
  // Maximum Spread %
  //
  InitString(gHiiHandle, STRING_TOKEN(StrMaxSpread), L"%d.%02d%%", Ranges.Fields.SscSprPercentMax/100, Ranges.Fields.SscSprPercentMax%100);

  //
  // Spread Mode Adjustments
  //
  if(Ranges.Fields.SscChangeAllowed) {
    TempBuffer = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_SSC_CHANGE_ALLOWED), NULL);
  } else if(Ranges.Fields.SscHaltAllowed) {
    TempBuffer = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_SSC_HALT_ALLOWED), NULL);
  } else {
    TempBuffer = HiiGetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ICC_SSC_NOTHING_ALLOWED), NULL);
  }
  InitString(gHiiHandle, STRING_TOKEN(StrSpreadAdjust), TempBuffer);
  FreePool(TempBuffer);
}

/*++
Routine Description:
  Setup callback executed when user selects a ICC Profile from the BIOS UI.
  Changes visibility of other options
Arguments:
  interface to ITEM_CALLBACK_EX
Returns:
  always SUCCESS
--*/
EFI_STATUS
EFIAPI
IccProfileCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION Action,
  IN EFI_QUESTION_ID KeyValue,
  IN UINT8 Type,
  IN EFI_IFR_TYPE_VALUE *Value,
  OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
  )
{
  UINTN               Size = sizeof(ME_RC_CONFIGURATION);
  ME_RC_CONFIGURATION MeRcConfiguration;
  EFI_STRING          RequestString = NULL;

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action < EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    return EFI_UNSUPPORTED;
  }

  if (MeIsAfterEndOfPost()) {
    return EFI_UNSUPPORTED;
  }

  if (Action >= EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    if (gIccMbpData.ProfileSelectionAllowed) {
      //
      // Perform change only if profile is selectable by BIOS
      //
      Value->u8 = 0;
    }
  }

  //
  // sanity check: can't choose profile that doesn't exist
  //
  if (Value->u8 > gIccMbpData.NumProfiles - 1) {
    Value->u8 = gIccMbpData.NumProfiles - 1;
  }

  //
  // when profile is changed, most other icc options can't be changed before reboot. hide those options.
  //
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (ME_RC_CONFIGURATION, AllowAdvancedOptions), sizeof (MeRcConfiguration.AllowAdvancedOptions));
  if (mProfileChanged || (Value->u8 != gIccMbpData.Profile)) {
    
    HiiGetBrowserData (&gEfiMeRcVariableGuid, ME_RC_CONFIGURATION_NAME, Size, (UINT8*)&MeRcConfiguration);
    MeRcConfiguration.AllowAdvancedOptions = DONT_DISPLAY;
    HiiSetBrowserData(&gEfiMeRcVariableGuid, ME_RC_CONFIGURATION_NAME, Size, (UINT8 *)&MeRcConfiguration, RequestString);
  } else if (!mProfileChanged && (Value->u8 == gIccMbpData.Profile)) {
    HiiGetBrowserData (&gEfiMeRcVariableGuid, ME_RC_CONFIGURATION_NAME, Size, (UINT8*)&MeRcConfiguration);
    MeRcConfiguration.AllowAdvancedOptions = DISPLAY;
    HiiSetBrowserData(&gEfiMeRcVariableGuid, ME_RC_CONFIGURATION_NAME, Size, (UINT8 *)&MeRcConfiguration, RequestString);
  }
  FreePool(RequestString);
  return EFI_SUCCESS;
}

VOID
ApplyClockSettings (
  IN ICC_GETSET_CLOCK_SETTINGS_REQRSP *RequestedClockSettings
  )
/*++
Routine Description:
  Executed by setup calback function
  Based on data entered by user, sends clock change requests to ICC OverClocking
  Writing to susram or flash requires that old susram and flash contents be invalidated
  In case of any problem, messagebox is displayed so user can know what corrective action is required
Arguments:
  initial clock divider value
Returns:
  validated clock divider value
--*/
{
  EFI_STATUS                  Status;
  WDT_PROTOCOL*               WdtProtocol;
  ME_RC_CONFIGURATION         MeRcConfiguration;
  UINT8                       BclkRampFlag;
  EFI_PLATFORM_TYPE_PROTOCOL *PlatformInfo;

  if (RequestedClockSettings == NULL) {
    return;
  }

  Status = gBS->LocateProtocol (&gEfiPlatformTypeProtocolGuid, NULL, &PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Set permanent and applied on warm reset flags if needed
  ///
  RequestedClockSettings->CurrentFlags.Flags.Type = 0;
  RequestedClockSettings->CurrentFlags.Flags.AppliedOnWarmRst = 1;

  Status = HeciSetIccClockSettings(RequestedClockSettings);
  DEBUG ((DEBUG_INFO, "(ICC) HeciSetIccClockSettings, Status = %r\n", Status));

  Status = GetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeRcConfiguration);
  ASSERT_EFI_ERROR (Status);

  if (Status == EFI_SUCCESS) {
    //
    // Update the BclkFrequency setup option to sync with MRC and XTU
    //
    if (RequestedClockSettings->ReqClock == CLOCK1) {
      MeRcConfiguration.BClkFrequency = RequestedClockSettings->UserFreq / HZ_TO_10KHZ;

      Status = SetSpecificConfigGuid (&gEfiMeRcVariableGuid, (VOID *)&MeRcConfiguration);
      ASSERT_EFI_ERROR (Status);
      DEBUG ((DEBUG_INFO, "(ICC) BClkFrequency = %d\n", MeRcConfiguration.BClkFrequency));
      DEBUG ((DEBUG_INFO, "(ICC) BClkOverride  = %d\n", MeRcConfiguration.BClkOverride));
    }

    if (!AreSettingsAtDefault(RequestedClockSettings, RequestedClockSettings->ReqClock)) {
      Status = gBS->LocateProtocol(&gWdtProtocolGuid, NULL, &WdtProtocol);
      ASSERT_EFI_ERROR(Status);
      WdtProtocol->AllowKnownReset();
    }

    //
    //  Set the BCLK Ramp flag
    //
    BclkRampFlag = 1;
    // APTIOV_SERVER_OVERRIDE_RC_START : No use
    //CmosWrite8 (CMOS_OC_SEND_BCLK_RAMP_MSG, BclkRampFlag);
    // APTIOV_SERVER_OVERRIDE_RC_END : No use
  }
}

VOID DebugDumpConfig (ME_RC_CONFIGURATION *IccSetup)
{
  DEBUG ((DEBUG_INFO, "Frequency %d %d\n", IccSetup->Frequency[0], IccSetup->Frequency[1]));
  DEBUG ((DEBUG_INFO, "Spread %d %d\n", IccSetup->Spread[0], IccSetup->Spread[1]));
  DEBUG ((DEBUG_INFO, "ShowFrequency %d %d\n", IccSetup->ShowFrequency[0], IccSetup->ShowFrequency[1]));
  DEBUG ((DEBUG_INFO, "ShowSpread %d %d\n", IccSetup->ShowSpread[0], IccSetup->ShowSpread[1]));
  DEBUG ((DEBUG_INFO, "ShowClock %d %d\n", IccSetup->ShowClock[0], IccSetup->ShowClock[1]));
  DEBUG ((DEBUG_INFO, "ShowProfile %d\n", IccSetup->ShowProfile));
  DEBUG ((DEBUG_INFO, "Profile %d\n", IccSetup->IccProfile));
  DEBUG ((DEBUG_INFO, "RegLock %d\n", IccSetup->RegLock));
  DEBUG ((DEBUG_INFO, "AllowAdvancedOptions %d\n", IccSetup->AllowAdvancedOptions));
}

VOID
InitIccStrings (
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class
  )
{
  if (!MeTypeIsAmt()) {
    return;
  }

  // APTIOV_SERVER_OVERRIDE_RC_START : AMT_SUPPORT
  //  if(Class != VFR_ADVANCED_CLASS) {
    if(Class != VFR_INTELRCSETUP_CLASS) {
  // APTIOV_SERVER_OVERRIDE_RC_END : AMT_SUPPORT
    return;
  }
  gHiiHandle = HiiHandle;
}

VOID
GetClockSettings ()
{
  EFI_STATUS            Status;
  static BOOLEAN        StringInit = FALSE;
  ICC_CLK_RANGE_DEF_REC Crdr;

  if (MeIsAfterEndOfPost() || StringInit) {
    return;
  }

  Status = HeciGetClockRangeDefinitionRecord(&Crdr);
  DEBUG ((DEBUG_INFO, "(ICC) CRDR[0] Dword[0] = %X Dword[1] = %X\n", Crdr.SingleClkRangeDef[0].Dword[0], Crdr.SingleClkRangeDef[0].Dword[1]));
  DEBUG ((DEBUG_INFO, "(ICC) CRDR[1] Dword[0] = %X Dword[1] = %X\n", Crdr.SingleClkRangeDef[1].Dword[0], Crdr.SingleClkRangeDef[1].Dword[1]));

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "(ICC) Get Clock Range Definition Record message failed. Status = %r\n", Status));
    return;
  }

  //
  //  Get BCLK Settings
  //
  SetMem(&mSettingsBclk, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP), 0);

  mSettingsBclk.ReqClock = CLOCK1;
  Status = HeciGetIccClockSettings(&mSettingsBclk);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "(ICC) Get Clock Settings messge failed, clock 1. Status = %r\n", Status));
    return;
  }
  CopyMem(&mRangesBclk.Dword[0], &Crdr.SingleClkRangeDef[CLOCK1], sizeof (ICC_SINGLE_CLK_RANGE_DEF));

  //
  //  Get PCIe Settings
  //
  SetMem(&mSettingsPcie, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP), 0);

  mSettingsPcie.ReqClock = CLOCK0;
  Status = HeciGetIccClockSettings(&mSettingsPcie);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "(ICC) Get Clock Settings messge failed, clock 0. Status = %r\n", Status));
    return;
  }
  CopyMem(&mRangesPcie.Dword[0], &Crdr.SingleClkRangeDef[CLOCK0], sizeof (ICC_SINGLE_CLK_RANGE_DEF));

  StringInit = TRUE;
}

VOID
UpdateVisibility (
  ME_RC_CONFIGURATION* SetupPtr
  )
{
  UINT32              MeMode;

  mHeci->GetMeMode (&MeMode);

  if (MeIsAfterEndOfPost() || (MeMode != ME_MODE_NORMAL)) {
    SetupPtr->AllowAdvancedOptions = DONT_DISPLAY;
  } else {
    SetupPtr->AllowAdvancedOptions = DISPLAY;
  }
  if (MeIsAfterEndOfPost() || gIccMbpData.ProfileSelectionAllowed == DONT_DISPLAY || (MeMode != ME_MODE_NORMAL)) {
    SetupPtr->ShowProfile = DONT_DISPLAY;
  } else {
    SetupPtr->ShowProfile = DISPLAY;
  }
  if (MeIsAfterEndOfPost() || mRangesBclk.Fields.ClockUsage == 0) {
    SetupPtr->ShowClock[CLOCK1]= DONT_DISPLAY;
  } else {
    SetupPtr->ShowClock[CLOCK1] = DISPLAY;
  }
  if (MeIsAfterEndOfPost() || mRangesPcie.Fields.ClockUsage == 0) {
    SetupPtr->ShowClock[CLOCK0] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowClock[CLOCK0] = DISPLAY;
  }
  if (MeIsAfterEndOfPost() || mRangesBclk.Fields.ClockUsage == 0 || mRangesBclk.Fields.ClkFreqMax == mRangesBclk.Fields.ClkFreqMin) {
    SetupPtr->ShowFrequency[CLOCK1]= DONT_DISPLAY;
  } else {
    SetupPtr->ShowFrequency[CLOCK1] = DISPLAY;
  }
  if (MeIsAfterEndOfPost() || mRangesPcie.Fields.ClockUsage == 0 || mRangesPcie.Fields.ClkFreqMin == mRangesPcie.Fields.ClkFreqMin) {
    SetupPtr->ShowFrequency[CLOCK0] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowFrequency[CLOCK0] = DISPLAY;
  }

  if (MeIsAfterEndOfPost() || mRangesBclk.Fields.ClockUsage == 0 || mRangesBclk.Fields.SscChangeAllowed == 0) {
    SetupPtr->ShowSpread[CLOCK1] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowSpread[CLOCK1] = DISPLAY;
  }
  if (MeIsAfterEndOfPost() || mRangesPcie.Fields.ClockUsage == 0 || mRangesPcie.Fields.SscChangeAllowed == 0) {
    SetupPtr->ShowSpread[CLOCK0] = DONT_DISPLAY;
  } else {
    SetupPtr->ShowSpread[CLOCK0] = DISPLAY;
  }
}

VOID
IccExtractConfigOnSetupEnter (
  ME_RC_CONFIGURATION *MeRcConfiguration
  )
{
  EFI_STATUS          Status;
  UINTN               Size;

  if ((MeRcConfiguration == NULL)
#if defined(SPS_SUPPORT) && SPS_SUPPORT
    || !MeTypeIsAmt()
#endif //SPS_SUPPORT
  ) {
    return;
  }

  if (!mHeci) {
    Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, &mHeci);
    if (EFI_ERROR(Status)) {
      MeRcConfiguration->AllowAdvancedOptions = DONT_DISPLAY;
      MeRcConfiguration->ShowProfile = DONT_DISPLAY;
      return;
    }
  }

  if (MeIsAfterEndOfPost()) {
    MeRcConfiguration->AllowAdvancedOptions = DONT_DISPLAY;
    MeRcConfiguration->ShowProfile = DONT_DISPLAY;
    return;
  }

  MeRcConfiguration->AllowAdvancedOptions = DISPLAY;
  MeRcConfiguration->ShowProfile = DISPLAY;

  GetClockSettings();

  UpdateSubmenuStrings(CLOCK1, mRangesBclk);
  UpdateSubmenuStrings(CLOCK0, mRangesPcie);

  //
  // Use data retrieved from MBP
  //
  Status = gRT->GetVariable (ICC_MBP_DATA_NAME, &gEfiMeRcVariableGuid, NULL, &Size, &gIccMbpData);
  if (EFI_ERROR(Status)) {
    ICC_DATA_PROTOCOL* mIccData;

    Status = gBS->LocateProtocol (&gIccDataProtocolGuid, NULL, &mIccData);
    if (!EFI_ERROR(Status)) {
      Status = mIccData->GetMbpData(&gIccMbpData);
    }
  }

  MeRcConfiguration->IccProfile = gIccMbpData.Profile;
  MeRcConfiguration->RegLock = gIccMbpData.RegLock;

  UpdateClockData (mSettingsBclk, CLOCK1, MeRcConfiguration);
  UpdateClockData (mSettingsPcie, CLOCK0, MeRcConfiguration);
  UpdateVisibility(MeRcConfiguration);

  DebugDumpConfig(MeRcConfiguration);

}

VOID
IccRouteConfig (
  ME_RC_CONFIGURATION *MeRcConfiguration
  )
{
  ICC_GETSET_CLOCK_SETTINGS_REQRSP  ClockSettings;

  if ((MeRcConfiguration == NULL)
#if defined(SPS_SUPPORT) && SPS_SUPPORT
      || !MeTypeIsAmt()
#endif //SPS_SUPPORT
     ) {
    return;
  }

  if (MeIsAfterEndOfPost() || (mHeci == NULL)) {
    return;
  }

  MaybeChangeProfile(MeRcConfiguration->IccProfile);
  MaybeChangeRegLock(MeRcConfiguration->RegLock);

  SetMem(&ClockSettings, sizeof (ICC_GETSET_CLOCK_SETTINGS_REQRSP), 0);
  if (!mProfileChanged && MeRcConfiguration->AllowAdvancedOptions) {
    //
    //  Update BCLK settings if needed
    //
    if (MeRcConfiguration->ShowClock[CLOCK1] && gClockChanged[CLOCK1]) {
      gIccReset = TRUE;
      gClockChanged[CLOCK1] = FALSE;
      ExtractClockData(MeRcConfiguration, CLOCK1, &ClockSettings);
      ApplyClockSettings(&ClockSettings);
    }
  }

}

VOID
MaybeChangeProfile (
  UINT8 Profile
  )
{
  static UINT8 LastSavedProfile;

  if (!mProfileChanged) {
    LastSavedProfile = gIccMbpData.Profile;
  }
  if (Profile != LastSavedProfile) {
    DEBUG ((DEBUG_INFO, "(ICC) Changing profile: old %d, new %d\n", LastSavedProfile, Profile));
    HeciSetIccProfile (Profile);
    LastSavedProfile = Profile;
    mProfileChanged = TRUE;
    gIccReset = TRUE;
  }
}

VOID
MaybeChangeRegLock (
  UINT8 RegLock
  )
{
  static UINT8        LastSavedRegLock = ICC_NOT_INITIALIZED;

  if (LastSavedRegLock == ICC_NOT_INITIALIZED) {
    LastSavedRegLock = gIccMbpData.RegLock;
  }
  if (RegLock != LastSavedRegLock) {
    DEBUG ((DEBUG_INFO, "(ICC) Changing RegLock: old %d, new %d\n", LastSavedRegLock, RegLock));
    HeciSetIccRegLock (RegLock);
  }

  LastSavedRegLock = RegLock;
}

#endif // ICC_SUPPORT
