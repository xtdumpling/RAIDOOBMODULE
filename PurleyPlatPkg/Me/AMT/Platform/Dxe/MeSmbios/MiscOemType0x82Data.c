/**@file

@copyright
 Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "MeSmbios.h"


//
// Static (possibly build generated) Oem data.
//
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SUBCLASS_TABLE_DATA(EFI_MISC_OEM_TYPE_0x82, MiscOemType0x82) = {
  {
    0x82,                         // Smbios type
    0x14,                         // Data Length
    0x00                          // handle value is dummy and is filled by Smbios Core driver
  },
  '$','A','M','T',                // AMT Signature
  1,                              // AMT Supported?
  0,                              // AMT Enabled?
  0,                              // SRoU Enabled?
  0,                              // Serial Over Lan Enabled?
  0,                              // Network interface of the Intel AMT part is enabled?
  0xA5,                           // 0xA5 (for backward compatibility)
  0xAF,                           // 1st Byte of OEM capabilities - bits 0-1 reserved (must be 0x01), 2 BIOS Reflash, 3 BIOS Setup, 4 BIOS Pause, 5 No floppy boot, 6 No CD boot, 7 reserved (must be 0x01)
  0x02,                           // 2nd Byte of OEM capabilities (Terminal Emulation), 0x02 = VT100+
  0xC0,                           // 3rd Byte of OEM capabilities - bits 0-5 reserved (must be 0x00), 6 HDD Secure Erase Capability, 7 BIOS Secure Boot Capability,
  0,                              // 4th Byte of OEM capabilities (reserved must be 0x00)
  0,                              // Kvm Enabled?
  0,                              // Reserved
  0                               // Terminator
};