/** @file
  PCH policy protocol produced by a platform driver specifying various
  expected PCH settings. This protocol is consumed by the PCH drivers.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

  @file PlatformPolicy.h
    
  Protocol used for Platform Policy definition.

**/

#ifndef _PLATFORM_POLICY_PROTOCOL_H_
#define _PLATFORM_POLICY_PROTOCOL_H_

typedef struct _EFI_PLATFORM_POLICY_PROTOCOL EFI_PLATFORM_POLICY_PROTOCOL;

#define EFI_PLATFORM_POLICY_PROTOCOL_GUID \
  { \
    0x2977064f, 0xab96, 0x4fa9, { 0x85, 0x45, 0xf9, 0xc4, 0x02, 0x51, 0xe0, 0x7f }\
  }

//
// Protocol to describe various platform information. Add to this as needed.
//
struct _EFI_PLATFORM_POLICY_PROTOCOL {
  UINT8 NumRsvdSmbusAddresses;
  UINT8 *RsvdSmbusAddresses;
};

extern EFI_GUID gEfiPlatformPolicyProtocolGuid;

#endif
