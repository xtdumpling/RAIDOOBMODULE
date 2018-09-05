/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.
**/

#ifndef _AMT_SUPPORT_LIB_H_
#define _AMT_SUPPORT_LIB_H_

#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AmtWrapperProtocol.h>
#include <MeBiosPayloadData.h>
#include <AmtSetup.h>

VOID
AmtBdsBoot (
  VOID
  );

VOID
AmtUpdateConsoleVariable (
  VOID
  );

BOOLEAN
AmtEnterSetup (
  VOID
  );

/**
  AMT action when console is ready.
  1. Signal AlertAtHandler event.
  2. Dynamically switch the output mode to 80x25 when SOL is enabled.
**/
VOID
AmtConsoleReady (
  VOID
  );

EFI_STATUS
AmtGetSetupPrompt (
  IN OUT  UINTN     *PromptIndex,
  OUT     CHAR16    **String
  );

EFI_STATUS
AmtDetermineSetupHotKey (
  IN  EFI_KEY_DATA  *KeyData
  );

EFI_STATUS
AmtGetSupportedHotkeys (
  OUT  UINTN             *KeyCnt,
  OUT  EFI_KEY_DATA      **KeyData
  );

/**
  Register ME Hotkey.
**/
VOID
RegisterMeHotKey (
  EFI_EVENT *HotkeyTriggered
 );

/**
  Raise Exception if ME FW requests to push media table

  @param[in][out]   None

  @return None.
**/
VOID
AmtRaiseBootException (
  VOID
  );

/**
  Check whether do full configuration enumeration for media asset table

  @param[in][out]   BootMode        A pointer to BootMode to be updated.

  @return None.
**/
VOID
AmtUpdateBootMode (
  IN OUT EFI_BOOT_MODE *BootMode
  );

CHAR16 *
DevicePathToStr (
    IN EFI_DEVICE_PATH_PROTOCOL *DevPath
);


#endif //_AMT_SUPPORT_H_
