/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2011 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  MeInfo.h

@brief
  This file defines the ME Info Protocol.

**/
#ifndef _ME_INFO_H_
#define _ME_INFO_H_
#include <Include/SiVersion.h>
///
/// Define ME INFO protocol GUID
///
#define EFI_ME_INFO_PROTOCOL_GUID \
  { \
    0x11fbfdfb, 0x10d2, 0x43e6, 0xb5, 0xb1, 0xb4, 0x38, 0x6e, 0xdc, 0xcb, 0x9a \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gEfiMeInfoProtocolGuid;

///
/// Revision 1:  Original version
///
#define ME_INFO_PROTOCOL_REVISION_1  1

///
/// Me Reference Code formats 0xAABBCCDD
///   DD - Build Number
///   CC - Reference Code Revision
///   BB - Reference Code Minor Version
///   AA - Reference Code Major Version
/// Example: Me Reference Code 0.7.1 should be 00 07 01 00 (0x00070100)
///
#define ME_RC_BUILD             0x00
#define ME_RC_VERSION           (SI_MAJOR_VERSION << 24) | (SI_MINOR_VERSION << 16) | (SI_REV_VERSION << 8) | (ME_RC_BUILD)
#define ME_FVI_STRING           "Reference Code - ME 10.0"
#define ME_FVI_SMBIOS_INSTANCE  0x03

#define MEBX_RC_VERSION     0xFFFFFFFF
#define MEBX_FVI_STRING     "MEBx version"

#define MEFW_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

typedef enum {
  EnumMeRc = 0, ///< ME Reference Code Version
  EnumMebx,     ///< MEBx Version
  EnumMeFw      ///< ME FW Version
} ME_FVI_INDEX;

///
/// Protocol definition
///
typedef struct _EFI_ME_INFO_PROTOCOL {
  UINT8   Revision;
  UINT32  RCVersion;
} EFI_ME_INFO_PROTOCOL;
#endif
