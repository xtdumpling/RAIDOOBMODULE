//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA PCD Update Library Header File.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_PCD_UPDATE_LIB_H
#define _UBA_PCD_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#define PLATFORM_PCD_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'P', 'C', 'D')
#define PLATFORM_PCD_UPDATE_VERSION    01

// {D5081573-B3B6-4a1f-9FBC-C3DEDA04CD49}
#define   PLATFORM_PCD_CONFIG_DATA_GUID \
{ 0xd5081573, 0xb3b6, 0x4a1f, { 0x9f, 0xbc, 0xc3, 0xde, 0xda, 0x4, 0xcd, 0x49 } }

typedef
EFI_STATUS
(*PCD_UPDATE_CALLBACK) (
  VOID
);

typedef struct {
  UINT32                  Signature;
  UINT32                  Version;
  
  PCD_UPDATE_CALLBACK     CallUpdate;
  
} PLATFORM_PCD_UPDATE_TABLE;

EFI_STATUS
PlatformUpdatePcds (
  VOID
);

STATIC  EFI_GUID gPlatformPcdConfigDataGuid = PLATFORM_PCD_CONFIG_DATA_GUID;

#endif //_UBA_PCD_UPDATE_LIB_H
