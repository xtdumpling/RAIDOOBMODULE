//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  Read FFS Library.

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef __READ_FFS__
#define __READ_FFS__
#include <Base.h>
#include <Pi/PiFirmwareVolume.h>

#define MAX_COMPRESS_ITEM 196
#define MAX_HOB_ENTRY_SIZE  8*1024
#define COMPRESS_DUPLICATE 1
#define COMPRESS_SINGLE    2

#pragma pack(1)
typedef struct {
  UINT16                   Value;
  UINT16                   Length;
  UINT8                    Type;
  UINT16                   Offset;
} COMPRESS_ITEM;

typedef struct {
  UINT32                    Signature;
  UINT32                    Count;
  //COMPRESS_ITEM             item[Count];
} COMPRESS_HOBO_DATA;
#pragma pack()


BOOLEAN NormalHobToCompressHob(IN OUT VOID* hobAddr,IN OUT UINTN* size);
BOOLEAN CompressHobToNormalHob(IN OUT VOID* hobAddr,OUT UINTN* size);

//read a FFS from FV.
EFI_STATUS
ReadFFSFile (
  IN EFI_FIRMWARE_VOLUME_HEADER* FwVolHeader,
  IN EFI_GUID FFSGuid,
  IN UINT32   FFSDataSize,
  IN OUT VOID *FFSData,
  OUT UINT32  *FFSSize,
  IN BOOLEAN skipheader
  );
EFI_STATUS ValidateCommonFvHeader (  EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader  );
#endif