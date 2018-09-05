/** @file
  Interface definition details between ME and platform drivers during DXE phase.

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
#ifndef _ME_POLICY_H_
#define _ME_POLICY_H_

#include <Config/MeConfig.h>

///
/// ME policy provided by platform for DXE phase
/// This protocol provides an interface to get Intel ME Configuration information
///

/**
  ME Policy Protocol \n
  All ME Policy Protocol change history listed here \n

  -<b> Revision 1</b>:
   - Initial version \n
  -<b> Revision 2</b>
   - Added MeJhiSupport \n
  -<b> Revision 3</b>
   - Added CoreBiosDoneEnabled \n
   - Deprecated NffCapabilityState \n

**/
#define ME_POLICY_PROTOCOL_REVISION  1

extern EFI_GUID gDxeMePolicyGuid;

///
/// ME ERROR Message ID
///
typedef enum {
  MSG_EOP_ERROR             = 0,
  MSG_ME_FW_UPDATE_FAILED,
  MSG_ASF_BOOT_DISK_MISSING,
  MSG_KVM_TIMES_UP,
  MSG_KVM_REJECTED,
  MSG_HMRFPO_LOCK_FAILURE,
  MSG_HMRFPO_UNLOCK_FAILURE,
  MSG_ME_FW_UPDATE_WAIT,
  MSG_ILLEGAL_CPU_PLUGGED_IN,
  MSG_KVM_WAIT,
  MSG_PLAT_DISABLE_WAIT,
  MAX_ERROR_ENUM
} ME_ERROR_MSG_ID;

/**
  Show ME Error message. This is to display localized message in
  the console. This is used to display message strings in local
  language. To display the message, the routine will check the
  message ID and ConOut the message strings. For example, the
  End of Post error displayed in English will be:
  gST->ConOut->OutputString (gST->ConOut, L"Error sending End Of
  Post message to ME\n"); It is recommended to clear the screen
  before displaying the error message and keep the message on
  the screen for several seconds.
  A sample is provided, see ShowMeReportError () to retrieve
  details.

  @param[in] MsgId                ME error message ID for displaying on screen message

**/
typedef
VOID
(EFIAPI *ME_REPORT_ERROR) (
  IN ME_ERROR_MSG_ID              MsgId
  );

///
/// ME DXE Policy
/// This protocol provides information of the current Intel ME feature selection. Information is
/// passed from the platform code to the Intel ME Reference code using this structure. There are
/// 2 types of information, BIOS setup option and ME status information.
///
typedef struct _ME_POLICY_PROTOCOL {
  ///
  /// Revision for the protocol structure
  ///
  UINT32                          Revision;

  ME_CONFIG                       MeConfig;
  ///
  /// Runtime configuration
  ///
  UINT32                          EndOfPostDone : 1;
  ///
  /// Support Localization for displaying on screen message
  ///
  ME_REPORT_ERROR                 MeReportError;
  
} ME_POLICY_PROTOCOL;

#endif // _ME_POLICY_H_
