/** @file
  ME config block.

@copyright
  Copyright (c) 2016 Intel Corporation. All rights reserved
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
#ifndef _ME_CONFIG_H_
#define _ME_CONFIG_H_

#define ME_CONFIG_REVISION 1

#ifndef PLATFORM_POR
#define PLATFORM_POR  0
#endif
#ifndef FORCE_ENABLE
#define FORCE_ENABLE  1
#endif
#ifndef FORCE_DISABLE
#define FORCE_DISABLE 2
#endif

#define ME_DEVICE_DISABLED 0
#define ME_DEVICE_ENABLED  1
#define ME_DEVICE_AUTO     2

///
/// End of Post
///
#define EOP_DISABLED    0
#define EOP_SEND_IN_DXE 2


#ifndef VFRCOMPILE

#pragma pack (push,1)
typedef struct
{
  UINT32 Revision;                   ///< Revision for the config structure
  
  ///
  /// @name Pre-Memory
  /// Below data will be consumed before memory is initialized.
  ///@{
  UINT32 HeciTimeouts                     : 1;  ///< 0: Disable; <b>1: Enable</b> - HECI Send/Receive Timeouts.

  /**
  <b>(Test)</b>
  <b>0: Disabled</b>
  1: ME DID init stat 0 - Success
  2: ME DID init stat 1 - No Memory in Channels
  3: ME DID init stat 2 - Memory Init Error
  4: ME DID init stat 3 - Memory not preserved across reset
  **/
  UINT32 DidInitStat                      : 8;
  /**
  <b>(Test)</b>
  <b>0: Set to 0 to enable polling for CPU replacement</b>
  1: Set to 1 will disable polling for CPU replacement
  **/
  UINT32 DisableCpuReplacedPolling        : 1;
  UINT32 SendDidMsg                       : 1;  ///< <b>(Test)</b> 0: Disable; <b>1: Enable</b> - Enable/Disable to send DID message.
  /**
  <b>(Test)</b>
  <b>0: Set to 0 to enable retry mechanism for HECI APIs</b>
  1: Set to 1 will disable retry mechanism for HECI APIs
  **/
  UINT32 DisableHeciRetry                 : 1;
  /**
  <b>(Test)</b>
  <b>0: ME BIOS will check each messages before sending</b>
  1: ME BIOS always sends messages without checking
  **/
  UINT32 DisableMessageCheck              : 1;
  /**
  <b>(Test)</b>
  The SkipMbpHob policy determines whether ME BIOS Payload data will be requested during boot
  in a MBP message. If set to 1, BIOS will send the MBP message with SkipMbp flag
  set causing CSME to respond with MKHI header only and no MBP data
  <b>0: ME BIOS will keep MBP and create HOB for MBP data</b>
  1: ME BIOS will skip MBP data
  **/
  UINT32 SkipMbpHob                       : 1;

  UINT32 HeciCommunication1               : 2;  ///< 0: Disabled; 1: Enabled; 2: Auto
  UINT32 HeciCommunication2               : 2;  ///< 0: Disabled; 1: Enabled; 2: Auto
  UINT32 HeciCommunication3               : 2;  ///< 0: Disabled; 1: Enabled; 2: Auto
  UINT32 IderDeviceEnable                 : 2;  ///< 0: Disabled; 1: Enabled; 2: Auto
  UINT32 KtDeviceEnable                   : 2;  ///< 0: Disabled; 1: Enabled; 2: Auto

  UINT32 HostResetNotification            : 1;  ///< <b>(Test)</b> <b>0: Disable</b>; 1: Enable - Enable/Disable sending notification on Host reset.
  UINT32 HsioMessaging                    : 1;  ///< <b>(Test)</b> <b>0: Disable</b>; 1: Enable - Enable/Disable HSIO synchronisation with ME.
  UINT32 DWord0RsvdBits                   : 6;
  ///@}

  ///
  /// @name Post-Memory
  /// Below data will be consumed after memory is initialized.
  ///@{
  UINT32 EndOfPostMessage                 : 2;  ///< 0: Disabled; 1: Send in PEI; <b>2: Send in DXE</b> - Send EOP at specific phase.
  UINT32 DisableD0I3SettingForHeci        : 1;  ///< <b>(Test)</b> <b>0: Disable</b>; 1: Enable - Enable/Disable D0i3 for HECI.

  UINT32 DWord1RsvdBits                   : 28;
  ///@}

  ///
  /// @name DXE
  /// Below data will be consumed in DXE phase.
  ///@{
  UINT32 MeFwDownGrade                    : 1;  ///< <b>0: Disabled</b>; 1: Enabled - Enable/Disable ME FW downgrade
  UINT32 MeLocalFwUpdEnabled              : 1;  ///< <b>0: Disabled</b>; 1: Enabled - Enadle/Disable ME FW local update
  UINT32 OsPtpAware                       : 1;  ///< 0: OS is not aware of PTP; <b>1: OS is aware of PTP interface</b>
  UINT32 HidePttFromOS                    : 1;  ///< <b>0: Ptt should be exposed to OS</b> - 1: Ptt should be hidden from OS
  UINT32 MeJhiSupport                     : 1;  ///< <b>0: Disabled</b>; 1: Enabled - Enable/Disable Intel DAL Host Interface Service.
  UINT32 CoreBiosDoneEnabled              : 1;  ///< 0: Disabled; <b>1: Enabled</b> - Enable/Disable Sending notification that BIOS is starting to run 3rd party code
  UINT32 DWord2RsvdBits                   : 26;
  
  ///@}
} ME_CONFIG;
#pragma pack (pop)

#endif // VFRCOMPILE

#endif // _ME_CONFIG_H_

