/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  2006 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  MeBiosPayloadData.h

@brief
  Interface definition details for MBP during DXE phase.

**/
#ifndef _ME_BIOS_PAYLOAD_DATA_H_
#define _ME_BIOS_PAYLOAD_DATA_H_
#include "CoreBiosMsg.h"

///
/// MBP Protocol for DXE phase
///

///
/// Revision 1:  Original version
///
#define MBP_DATA_PROTOCOL_REVISION_1  1

#define MBP_ITEM_FLAG_SENSITIVE     BIT0  ///< Item contains sensitive data.  Do not store in flash.
#define MBP_ITEM_FLAG_RUNTIME       BIT1  ///< Item is generated at runtime.  Do not store in flash.
#define MBP_ITEM_FLAG_RSVD2         BIT2  ///< Reserved for future use.
#define MBP_ITEM_FLAG_RSVD3         BIT3  ///< Reserved for future use.
#define MBP_ITEM_FLAG_RSVD4         BIT4  ///< Reserved for future use.
#define MBP_ITEM_FLAG_RSVD5         BIT5  ///< Reserved for future use.
#define MBP_ITEM_FLAG_RSVD6         BIT6  ///< Reserved for future use.
#define MBP_ITEM_FLAG_RSVD7         BIT7  ///< Reserved for future use.

#pragma pack(push, 1)
///
/// Common MBP types
///
typedef union _MBP_HEADER {
  UINT32 Data;
  struct {
    UINT32 MbpSize    : 8;  ///< Byte 0 - MBP Size in DW including Header
    UINT32 NumEntries : 8;  ///< Byte 1 - Number of Entries (Data Items) in MBP
    UINT32 Flags      : 8;  ///< Byte  2 - Flags
    UINT32 Rsvd       : 8;  ///< Byte 3 - Reserved
  } Fields;
} MBP_HEADER;

typedef union _MBP_ITEM_HEADER {
  UINT32 Data;
  struct {
    UINT32 AppId  : 8; ///< Byte 0 - Application ID
    UINT32 ItemId : 8; ///< Byte 1 - Item ID
    UINT32 Length : 8; ///< Byte 2 - Length in DW
    UINT32 Flags  : 8; ///< Byte 3 - Flags
  } Fields;
} MBP_ITEM_HEADER;

///
/// MBP items
///
typedef struct _MBP_FW_VERSION_NAME {
  UINT32  MajorVersion  : 16;
  UINT32  MinorVersion  : 16;
  UINT32  HotfixVersion : 16;
  UINT32  BuildVersion  : 16;
} MBP_FW_VERSION_NAME;

typedef struct _ICC_PROFILE_SELECTION_TYPE
{
   UINT8            IccProfileId          :4;
   UINT8            Reserved              :3;
   UINT8            IccProfileSelectedBy  :1;
} ICC_PROFILE_SELECTION_TYPE;

typedef struct _ICC_PROFILE_INDEX_TYPE
{
   UINT8            IccProfileId   :4;
   UINT8            Reserved       :4;
} ICC_PROFILE_INDEX_TYPE;

typedef struct
{
   UINT8                            IccNumOfProfiles;
   ICC_PROFILE_INDEX_TYPE           IccProfileIndex;
   UINT8                            IccProfileChangeable;
   UINT8                            IccLockMaskType;
} MBP_ICC_PROFILE;

typedef struct _MBP_FW_CAPS_SKU {
  MEFWCAPS_SKU  FwCapabilities;
  BOOLEAN       Available;
} MBP_FW_CAPS_SKU;

typedef struct _MBP_FW_FEATURES_STATE {
  MEFWCAPS_SKU  FwFeatures;
  BOOLEAN       Available;
} MBP_FW_FEATURES_STATE;

typedef struct _MBP_ROM_BIST_DATA {
  UINT16  DeviceId;
  UINT16  FuseTestFlags;
  UINT32  UMCHID[4];
} MBP_ROM_BIST_DATA;

typedef struct _MBP_PLATFORM_KEY {
  UINT32  Key[8];
} MBP_PLATFORM_KEY;

typedef struct _MBP_PLAT_TYPE {
  PLATFORM_TYPE_RULE_DATA RuleData;
  BOOLEAN                 Available;
} MBP_PLAT_TYPE;

typedef union _HWA_DATA {
  UINT32  Raw;
  struct {
    UINT32  MediaTablePush : 1;
    UINT32  Reserved : 31;
  } Fields;
} HWA_DATA;

typedef struct _MBP_HWA_REQ {
  HWA_DATA Data;
  BOOLEAN Available;
} MBP_HWA_REQ;

typedef struct _MBP_PERF_DATA {
  UINT32  PwrbtnMrst;
  UINT32  MrstPltrst;
  UINT32  PltrstCpurst;
} MBP_PERF_DATA;

typedef struct _PLAT_BOOT_PERF_DATA {
  MBP_PERF_DATA MbpPerfData;
  BOOLEAN       Available;
} PLAT_BOOT_PERF_DATA;

typedef struct _MBP_NFC_DATA {
  UINT32 DeviceType :2;
  UINT32 Reserved : 30;
} MBP_NFC_DATA;

typedef struct _MBP_NFC_SUPPORT {
  MBP_NFC_DATA   NfcData;
  BOOLEAN        Available;
} MBP_NFC_SUPPORT;

typedef struct _MBP_ME_UNCONF_ON_RTC_DATA {
  UINT32 DisUnconfigOnRtcClearState :1;
  UINT32 Reserved : 31;
} MBP_ME_UNCONF_ON_RTC_DATA;

typedef struct _MBP_ME_UNCONF_ON_RTC_STATE {
  MBP_ME_UNCONF_ON_RTC_DATA   UnconfigOnRtcClearData;
  BOOLEAN                     Available;
} MBP_ME_UNCONF_ON_RTC_STATE;

///
/// ME BIOS Payload structure containing insensitive data only
///
typedef struct {
  MBP_FW_VERSION_NAME        FwVersionName;
  MBP_FW_CAPS_SKU            FwCapsSku;
  MBP_FW_FEATURES_STATE      FwFeaturesState;
  MBP_PLAT_TYPE              FwPlatType;
  MBP_ICC_PROFILE            IccProfile;
  AT_STATE_INFO              AtState;         ///< @deprecated
  UINT32                     MFSIntegrity;
  MBP_HWA_REQ                HwaRequest;
  PLAT_BOOT_PERF_DATA        PlatBootPerfData;
  MBP_NFC_SUPPORT            NfcSupport;
  MBP_ME_UNCONF_ON_RTC_STATE UnconfigOnRtcClearState;
} ME_BIOS_PAYLOAD;

///
/// All items which can't be available when 3rd part OPROMs/drivers are loaded
/// must be added to below structure only
///
typedef struct {
  MBP_ROM_BIST_DATA          RomBistData;
  MBP_PLATFORM_KEY           PlatformKey;
} ME_BIOS_PAYLOAD_SENSITIVE;

#pragma pack(pop)

#endif
