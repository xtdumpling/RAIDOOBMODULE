//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA ClockGen Update Library Header File.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_CLOCKGEN_UPDATE_LIB_H
#define _UBA_CLOCKGEN_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#define PLATFORM_CLOCKGEN_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'C', 'L', 'K')
#define PLATFORM_CLOCKGEN_UPDATE_VERSION    0x01

// {CF3845B1-7EB0-44ef-9D67-A80ECE6FED73}
#define   PLATFORM_CLOCKGEN_CONFIG_DATA_GUID \
{ 0xcf3845b1, 0x7eb0, 0x44ef, { 0x9d, 0x67, 0xa8, 0xe, 0xce, 0x6f, 0xed, 0x73 } }

#define PLATFORM_NUMBER_OF_CLOCKGEN_DATA    20
#define PLATFORM_CLOCKGEN_NO_ID             0xFF

typedef struct {
  UINT32                  Signature;
  UINT32                  Version;

  UINTN                   IdOffset;     // Clockgen ID register offset
  UINT8                   Id;           // Clockgen ID
  UINTN                   DataLength;   // Number of clockgen data for write

  UINTN                   SpreadSpectrumByteOffset;
  UINT8                   SpreadSpectrumValue;
  
  UINT8                   Data[PLATFORM_NUMBER_OF_CLOCKGEN_DATA];
  
} PLATFORM_CLOCKGEN_UPDATE_TABLE;


EFI_STATUS
PlatformUpdateClockgen (
  IN  BOOLEAN     EnableSpreadSpectrum
);

STATIC  EFI_GUID gPlatformClockgenConfigDataGuid = PLATFORM_CLOCKGEN_CONFIG_DATA_GUID;

#endif //_UBA_CLOCKGEN_UPDATE_LIB_H
