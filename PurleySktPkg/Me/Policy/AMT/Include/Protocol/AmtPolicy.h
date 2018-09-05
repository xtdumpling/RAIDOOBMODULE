/** @file
  Interface definition details between AMT and platform drivers during DXE phase.

@copyright
  Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _AMT_POLICY_H_
#define _AMT_POLICY_H_

///
/// AMT policy provided by platform for DXE phase
/// This protocol provides the information about Intel AMT platform configuration settings.
///

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
#define AMT_POLICY_PROTOCOL_REVISION 1   ///< Initial Revision

extern EFI_GUID gDxeAmtPolicyGuid;

#pragma pack (push,1)
typedef struct {
  //
  // DWORD 0
  // Byte 0, bit definition for functionality enable/disable
  //
  UINT32  AsfEnabled : 1;               ///< 0: Disabled; 1: Enabled
  UINT32  AmtEnabled : 1;               ///< 0: Disabled; 1: Enabled
  UINT32  AmtbxPasswordWrite : 1;       ///< 0: Disabled; 1: Enabled
  /**
  ME WatchDog timer feature
  <b> 0: Disabled <b>
  1: Enabled ME WDT if corresponding timer value is not zero.
  See description for WatchDogTimerOs and WatchDogTimerBios.
  **/
  UINT32  WatchDog : 1;
  UINT32  CiraRequest : 1;              ///< 0: No CIRA request; 1: Trigger CIRA request
  UINT32  ManageabilityMode : 1;        ///< 0: Disabled; 1:AMT
  UINT32  UnConfigureMe : 1;            ///< 0: No; 1: Un-configure ME without password
  UINT32  MebxDebugMsg : 1;             ///< 0: Disabled; 1: Enabled
  UINT32  ForcMebxSyncUp : 1;           ///< 0: No; 1: Force MEBX execution
  UINT32  UsbrEnabled : 1;              ///< 0: Disabled;  1: Enabled
  UINT32  UsbLockingEnabled : 1;        ///< 0: Disabled;  1: Enabled
  UINT32  HideUnConfigureMeConfirm : 1; ///< 0: Don't hide; 1: Hide Un-configure ME Confirmation Prompt
  UINT32  USBProvision : 1;             ///< 0: Disabled; 1: Enabled
  UINT32  FWProgress : 1;               ///< 0: Disabled; 1: Enabled
  UINT32  AmtbxHotkeyPressed : 1;       ///< 0: Disabled; 1: Enabled
  UINT32  AmtbxSelectionScreen : 1;     ///< 0: Disabled; 1: Enabled
  UINT32  WatchDogOs : 1;               ///< 0: Disabled; 1: Enabled
  UINT32  RsvdBits : 15;

  //
  // DWORD 1, Watchdog timer settings
  //
  /**
  Byte 4-5 OS WatchDog Timer
  <b> 0 - Disable <b> OS WDT won't be started after stopping BIOS WDT even if WatchDog is enabled.
  Non zero value - OS WDT will be started after stopping BIOS WDT if WatchDog is enabled. The timer is set
  according to the value.
  **/
  UINT16  WatchDogTimerOs;
  /**
  Byte 6-7 BIOS WatchDog Timer
  <b> 0 - Disable <b> BIOS WDT won't be started even if WatchDog is enabled.
  Non zero value - The BIOS WDT is set according to the value and started if WatchDog is enabled.
  **/
  UINT16  WatchDogTimerBios;
  //
  // DWORD 2
  //
  UINT32  CiraTimeout:8;
  ///
  /// Byte 8 CPU Replacement Timeout
  /// 0: 10 seconds; 1: 30 seconds; 2~5: Reserved; 6: No delay; 7: Unlimited delay
  ///
  UINT32  CpuReplacementTimeout:8;
  //
  // Byte 9-10 OemResolutionSettings
  //
  UINT32  MebxNonUiTextMode : 4;        ///< 0: Auto; 1: 80x25; 2: 100x31
  UINT32  MebxUiTextMode : 4;           ///< 0: Auto; 1: 80x25; 2: 100x31
  UINT32  MebxGraphicsMode : 4;         ///< 0: Auto; 1: 640x480; 2: 800x600; 3: 1024x768
  UINT32  OemResolutionSettingsRsvd : 4;
  //
  // DWORD 3,
  // Byte 12-15 Pointer to a list which contain on-board devices bus/device/fun number
  //
  UINT32  PciDeviceFilterOutTable;

} AMT_CONFIG;

///
/// AMT DXE Policy
/// This protocol is initialized by Policy driver. Other modules can locate this protocol
/// to retrieve Intel AMT related setup options setting
///
typedef struct _AMT_POLICY_PROTOCOL {
  UINT8                 Revision;                 ///< Revision for the protocol structure
  UINT8                 Reserved[3];
  AMT_CONFIG            AmtConfig;                ///< AMT policy for platform code to pass to Reference Code
} AMT_POLICY_PROTOCOL;
#pragma pack (pop)
#endif
