/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  2010 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Amt.h

@brief
  Header file for common Active Management Technology defines.

**/
#ifndef _AMT_H_
#define _AMT_H_

///
/// Intel Internet Assigned Numbers Authority Manufacturer ID
/// (The firmware sends 0x57010000 for decimal value 343)
///
#define INTEL_IANA_SWAP32(x)      ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | \
                                    (((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24))
#define ASF_INTEL_IANA            0x00000157
#define ASF_INTEL_CONVERTED_IANA  INTEL_IANA_SWAP32 (ASF_INTEL_IANA)  ///< 0X57010000, received from ME
///
/// Intel OEM Special Command
///
#define ASF_INTEL_OEM_CMD 0xC1

///
/// Intel OEM Parameters 16 bit OEM Parameter values
///
#define USE_SOL 0x0001  ///< 0000 0000 0000 0001 - bit 0, use SOL on the next boot
#ifdef ENABLE_SECURE_ERASE
#undef ENABLE_SECURE_ERASE
#endif // ENABLE_SECURE_ERASE
///
/// Intel OEM Command 16 bit special command parameter values
///
#define USE_STORAGE_REDIR       0x0001  ///< 0000 0000 0000 0001 - bit 0 Paramater 2 will be used to indicate the channel
#define ENFORCE_SECURE_BOOT     0x0002  ///< 0000 0000 0000 0010 - bit 1 Enforce secure boot over Storage Redirection
#define REFLASH_BIOS            0x0004  ///< 0000 0000 0000 0100 - bit 2
#define BIOS_SETUP              0x0008  ///< 0000 0000 0000 1000 - bit 3
#define BIOS_PAUSE              0x0010  ///< 0000 0000 0001 0000 - bit 4
#define USE_KVM                 0x0020  ///< 0000 0000 0010 0000 - bit 5
#define ENABLE_SECURE_ERASE     0x0040  ///< 0000 0000 0010 0000 - bit 6
#define STORAGE_REDIRECTION_BOOT_DEVICE_MASK   0x0300
#define STORAGE_REDIRECTION_BOOT_DEVICE_SHIFT  8

#endif
