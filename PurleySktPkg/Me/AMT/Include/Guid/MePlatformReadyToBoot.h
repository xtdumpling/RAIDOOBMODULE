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
  MePlatformReadyToBoot.h

@brief
  MePlatformReadyToBoot Guid definitions

**/
#ifndef _EFI_ME_PLATFORM_READY_TO_BOOT_GUID_H_
#define _EFI_ME_PLATFORM_READY_TO_BOOT_GUID_H_

#define EFI_EVENT_ME_PLATFORM_READY_TO_BOOT \
  { \
    0x3fdf171, 0x1d67, 0x4ace, 0xa9, 0x4, 0x3e, 0x36, 0xd3, 0x38, 0xfa, 0x74 \
  }

extern EFI_GUID gMePlatformReadyToBootGuid;

#endif
