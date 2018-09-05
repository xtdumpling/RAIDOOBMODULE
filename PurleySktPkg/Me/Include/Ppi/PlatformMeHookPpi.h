/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

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
  PlatformMeHookPpi.h

@brief
  Interface definition details for platform hook support to ME module.

**/
#ifndef _PLATFORM_ME_HOOK_PPI_H_
#define _PLATFORM_ME_HOOK_PPI_H_

///
/// ME Hook provided by platform for PEI phase
///
#define PLATFORM_ME_HOOK_PPI_GUID \
  { \
    0xe806424f, 0xd425, 0x4b1a, 0xbc, 0x26, 0x5f, 0x69, 0x03, 0x89, 0xa1, 0x5a \
  }

extern EFI_GUID gPlatformMeHookPpiGuid;

#define EFI_FORWARD_DECLARATION(x) typedef struct _##x x
EFI_FORWARD_DECLARATION (PLATFORM_ME_HOOK_PPI);

///
/// Revision
///
#define PLATFORM_ME_HOOK_PPI_REVISION 1

typedef
EFI_STATUS
(EFIAPI *PLATFORM_ME_HOOK_PRE_GLOBAL_RESET) (
  IN      EFI_PEI_SERVICES             **PeiServices,
  IN      PLATFORM_ME_HOOK_PPI         * This
  );

///
/// PPI definition
///
typedef struct _PLATFORM_ME_HOOK_PPI {
  UINT8                             Revision;
  PLATFORM_ME_HOOK_PRE_GLOBAL_RESET PreGlobalReset;
} PLATFORM_ME_HOOK_PPI;

#endif
