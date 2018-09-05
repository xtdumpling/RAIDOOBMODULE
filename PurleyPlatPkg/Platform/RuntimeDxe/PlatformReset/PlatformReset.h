/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2011 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  PlatformReset.h

@brief:

  Definitions for PlatformReset driver

**/
#ifndef _PLATFORM_RESET_H_
#define _PLATFORM_RESET_H_

#include <Protocol/PchReset.h>
#include <Protocol/PchPlatformResetPolicy.h>
#include <Protocol/PchExtendedReset.h>
#include <Protocol/Reset.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>

#define EFI_INTERNAL_POINTER 0x00000004

#endif /// _PLATFORM_RESET_H_
// APTIOV_SERVER_OVERRIDE_RC_START : To make sure Reset System works in Runtime UEFI OS environment.
VOID
PchResetVirtualddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );
// APTIOV_SERVER_OVERRIDE_RC_END : To make sure Reset System works in Runtime UEFI OS environment.