/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  2010 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  ConsoleLock.h

@brief
  Console Lock Guid definitions

**/
#ifndef _EFI_CONSOLE_LOCK_GUID_H_
#define _EFI_CONSOLE_LOCK_GUID_H_

#define EFI_CONSOLE_LOCK_GUID \
  { \
    0x368cda0d, 0xcf31, 0x4b9b, 0x8c, 0xf6, 0xe7, 0xd1, 0xbf, 0xff, 0x15, 0x7e \
  }

#define EFI_CONSOLE_LOCK_VARIABLE_NAME        (L"ConsoleLock")
#define EFI_CONSOLE_LOCK_FORMAT_VARIABLE_NAME ("ConsoleLock")

extern CHAR16   gEfiConsoleLockName[];

extern EFI_GUID gEfiConsoleLockGuid;

#define LOCK_CONSOLE    1
#define NO_LOCK_CONSOLE 0

#endif
