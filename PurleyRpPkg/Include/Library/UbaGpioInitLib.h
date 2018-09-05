//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA GPIO Initializtion Library Header File.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_GPIO_INIT_LIB_H
#define _UBA_GPIO_INIT_LIB_H

#include <Base.h>
#include <Uefi.h>

// {9282563E-AE17-4E12-B1DC-070F29F37120}
#define   PLATFORM_GPIO_INIT_DATA_GUID \
{ 0x9282563e, 0xae17, 0x4e12, { 0xb1, 0xdc, 0x7, 0xf, 0x29, 0xf3, 0x71, 0x20 } }

EFI_STATUS
PlatformInitGpios (
  VOID
);

STATIC  EFI_GUID gPlatformGpioInitDataGuid = PLATFORM_GPIO_INIT_DATA_GUID;

#endif //_UBA_GPIO_INIT_LIB_H
