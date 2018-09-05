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
GLOBAL_REMOVE_IF_UNREFERENCED MISC_SUBCLASS_TABLE_DATA(EFI_MISC_OEM_TYPE_0x83, MiscOemType0x83) = {
  {
    0x83,                         // Smbios type
    0x40,                         // Data Length
    0x00                          // handle value is dummy and is filled by Smbios Core driver
  },
  {
    0                             // Cpu Capabilities
  },
  {
    0x00,                         // MEBx Version.Major
    0x00,                         // MEBx Version.Minor
    0x00,                         // MEBx Version.Hotfix
    0x00                          // MEBx Version.Build
  },
  0,                              // Pch Capabilities
  0,                              // Me Capabilities
  0,                              // Me Fw Configured
  {
    0                             // Network Device
  },
  0,                              // Bios Capabilities
  'v','P','r','o',                // Structure Identifier
  0,                              // Reserved
  0                               // Terminator
};