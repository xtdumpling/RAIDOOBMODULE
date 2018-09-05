//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA Mp table Update Library Header File.

  Copyright (c) 2008 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_MP_TABLE_UPDATE_LIB_H
#define _UBA_MP_TABLE_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#include <PlatDevData.h>

#define PLATFORM_MP_TABLE_UPDATE_SIGNATURE  SIGNATURE_32 ('U', 'M', 'P', 'T')
#define PLATFORM_MP_TABLE_UPDATE_VERSION    0x01

// {0FF8A1CF-A0AB-4ac0-BFC9-34A78F68DD8A}
#define   PLATFORM_MP_TABLE_CONFIG_DATA_GUID \
{ 0xff8a1cf, 0xa0ab, 0x4ac0, { 0xbf, 0xc9, 0x34, 0xa7, 0x8f, 0x68, 0xdd, 0x8a } };


typedef struct {
  UINT32                  Signature;
  UINT32                  Version;
  
  DEVICE_DATA             *DeviceDataPtr;
  DEVICE_UPDATE_DATA      *DeviceUpdateDataPtr;
  
} PLATFORM_MP_UPDATE_TABLE;

EFI_STATUS
PlatformGetMpTableDataPointer (
  IN  DEVICE_DATA                       **DeviceData,
  IN  DEVICE_UPDATE_DATA                **DeviceUpdateData
);

STATIC  EFI_GUID gPlatformMpTableConfigDataGuid =PLATFORM_MP_TABLE_CONFIG_DATA_GUID;

#endif //_UBA_MP_TABLE_UPDATE_LIB_H
