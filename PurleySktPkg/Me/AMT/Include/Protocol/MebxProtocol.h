/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  MebxProtocol.h

@brief
  EFI MEBx Protocol

**/
#ifndef _MEBX_PROTOCOL_H
#define _MEBX_PROTOCOL_H

#define MEBX_CALLBACK_INFO_SIGNATURE    EFI_SIGNATURE_32 ('m', 'e', 'b', 'x')

#define MEBX_CALLBACK_INFO_FROM_THIS(a) CR (a, MEBX_CALLBACK_INFO, DriverCallback, MEBX_CALLBACK_INFO_SIGNATURE)

#ifndef INTEL_MEBX_PROTOCOL_GUID
#define INTEL_MEBX_PROTOCOL_GUID \
  { \
    0x01ab1829, 0xcecd, 0x4cfa, 0xa1, 0x8c, 0xea, 0x75, 0xd6, 0x6f, 0x3e, 0x74 \
  }
#endif

typedef
EFI_STATUS
(EFIAPI *EFI_MEBX_API_ENTRY_POINT) (
  IN UINT32  BiosParams,
  OUT UINT32 *MebxReturnValue
  );

typedef struct _MEBX_VER {
  UINT16  Major;
  UINT16  Minor;
  UINT16  Hotfix;
  UINT16  Build;
} MEBX_VER;

typedef enum {
  MEBX_GRAPHICS_AUTO    = 0,
  MEBX_GRAPHICS_640X480,
  MEBX_GRAPHICS_800X600,
  MEBX_GRAPHICS_1024X768
} MEBX_GRAPHICS_MODE;

typedef enum {
  MEBX_TEXT_AUTO        = 0,
  MEBX_TEXT_80X25,
  MEBX_TEXT_100X31
} MEBX_TEXT_MODE;

typedef struct _MEBX_PROTOCOL {
  MEBX_VER                  MebxVersion;
  EFI_MEBX_API_ENTRY_POINT  CoreMebxEntry;
} MEBX_PROTOCOL;

typedef struct _MEBX_INSTANCE {
  EFI_HANDLE        Handle;
  MEBX_PROTOCOL     MebxProtocol;
} MEBX_INSTANCE;

extern EFI_GUID gMebxProtocolGuid;

#endif
