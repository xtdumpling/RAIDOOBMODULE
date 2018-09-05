//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA Softstrap Update Library Header File.

  Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_SOFT_STRAP_UPDATE_LIB_H
#define _UBA_SOFT_STRAP_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#define PLATFORM_SOFT_STRAP_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'S', 'T', 'P')
#define PLATFORM_SOFT_STRAP_UPDATE_VERSION    01

// {F06383FE-54BD-4ae1-9C99-1DC83B6A7277}
#define   PLATFORM_SOFT_STRAP_CONFIG_DATA_GUID \
{ 0xf06383fe, 0x54bd, 0x4ae1, { 0x9c, 0x99, 0x1d, 0xc8, 0x3b, 0x6a, 0x72, 0x77 } }

#define   PLATFORM_NUMBER_OF_SOFT_STRAP_ENTRIES   0x20

typedef struct _PLATFORM_SOFTSTRAP_FIXUP_ENTRY
{
  UINT8  BoardId;
  UINT8  SoftStrapNumber;
  UINT8  BitfieldOffset;
  UINT8  BitfieldLength;
  UINT32 Value; 
} PLATFORM_SOFTSTRAP_FIXUP_ENTRY;

typedef struct 
{
  UINT32                            Signature;
  UINT32                            Version;
  
  PLATFORM_SOFTSTRAP_FIXUP_ENTRY    *StrapPtr;
  
} PLATFORM_SOFT_STRAP_UPDATE_TABLE;

EFI_STATUS
PlatformGetSoftStrapTable (
  IN  VOID                    **StrapTable
);

STATIC  EFI_GUID gPlatformSoftStrapConfigDataGuid = PLATFORM_SOFT_STRAP_CONFIG_DATA_GUID;

#endif //_UBA_SOFT_STRAP_UPDATE_LIB_H
