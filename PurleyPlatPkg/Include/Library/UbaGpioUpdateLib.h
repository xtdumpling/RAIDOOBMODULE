//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA GPIO Update Library Header File.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_GPIO_UPDATE_LIB_H
#define _UBA_GPIO_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#define PLATFORM_GPIO_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'G', 'P', 'O')
#define PLATFORM_GPIO_UPDATE_VERSION    01

// {E02C2982-0009-46f6-AF19-DF52BB9742BF}
#define   PLATFORM_GPIO_CONFIG_DATA_GUID \
{ 0xe02c2982, 0x9, 0x46f6, { 0xaf, 0x19, 0xdf, 0x52, 0xbb, 0x97, 0x42, 0xbf } }

#define PLATFORM_NUMBER_OF_GPIO_REGISTERS   20
#define PLATFORM_END_OF_GPIO_LIST           0xFFFFFFFF

#define GPIO_NO_OR                          0
#define GPIO_NO_AND                         0xFFFFFFFF


typedef struct {
  UINT32        Register;
  UINT32        Value;
} GPIO_DATA;

typedef struct {
  UINT32                  Signature;
  UINT32                  Version;
  
  GPIO_DATA               Gpios[PLATFORM_NUMBER_OF_GPIO_REGISTERS];
  
} PLATFORM_GPIO_UPDATE_TABLE;


EFI_STATUS
PlatformUpdateGpios (
  VOID
);

STATIC  EFI_GUID gPlatformGpioConfigDataGuid = PLATFORM_GPIO_CONFIG_DATA_GUID;

#endif //_UBA_GPIO_UPDATE_LIB_H
