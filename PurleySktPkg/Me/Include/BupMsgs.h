/** @file
  BUP Messages - HECI messages exchanged in pre-DID phase with CSME FW

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _BUP_MSGS_H
#define _BUP_MSGS_H

#if AMT_SUPPORT
#include <MeBiosPayloadData.h>
#endif // AMT_SUPPORT

#pragma pack(1)

///
/// BUP group IDs
///
typedef enum {
  BUP_COMMON_GROUP_ID = 0xF0,
  BUP_ICC_GROUP_ID ,
  BUP_HSIO_GROUP_ID,
  BUP_PM_GROUP_ID
} BUP_GROUP_ID;

///
/// Common group definitions
///
#define DRAM_INIT_DONE_CMD       0x1

typedef union _DRAM_INIT_DONE_DATA {
  struct {
    UINT32 UmaBaseAddr    :16; ///< BIT0-15
    UINT32 UmaBARsvrd     :7;  ///< BIT16-BIT22
    UINT32 FfsEntryExit   :1;  ///< BIT23
    UINT32 MemInitStatus  :4;  ///< BIT24 - BIT27
    UINT32 Reserved       :4;  ///< BIT28 - BIT31
  } Fields;
  UINT32 ul;
} DRAM_INIT_DONE_DATA;

typedef struct _DRAM_INIT_DONE_CMD_REQ {
   MKHI_MESSAGE_HEADER  MkhiHeader;
   DRAM_INIT_DONE_DATA  DIDData;
} DRAM_INIT_DONE_CMD_REQ;

///
/// Host Reset Warning message is used in server BIOS to warn ME about
/// a reset of host partition done on Sx resume path that will not break
/// the Sx resume. I.e. after reset host continues the same Sx flow,
/// so ME should ignore this reset and sustaing previews flow.
///
/// Reset warning is one of ME Bring Up PM messages (BUP_PM_GROUP_ID).
///
#define HOST_RESET_WARNING_CMD       0x1

typedef struct _HOST_RESET_WARNING_CMD_REQ {
   MKHI_MESSAGE_HEADER  MkhiHeader;
} HOST_RESET_WARNING_CMD_REQ;

typedef struct _HOST_RESET_WARNING_CMD_RESP
{
   MKHI_MESSAGE_HEADER  MkhiHeader;
   UINT8                BiosAction;
   UINT8                Reserved[3];
} HOST_RESET_WARNING_CMD_RESP;

#define HOST_RESET_WARNING_ACTION_CONTINUE 0 // Continue boot, the only action for now
#define HOST_RESET_WARNING_ACTION_LAST     HOST_RESET_WARNING_ACTION_CONTINUE


///
/// Memory Init Status codes
///
#define BIOS_MSG_DID_SUCCESS              0
#define BIOS_MSG_DID_NO_MEMORY            0x1
#define BIOS_MSG_DID_INIT_ERROR           0x2
#define BIOS_MSG_DID_MEM_NOT_PRESERVED    0x3

typedef struct _DRAM_INIT_DONE_CMD_RESP
{
   MKHI_MESSAGE_HEADER  MkhiHeader;
   UINT8                BiosAction;
   UINT8                Reserved[3];
} DRAM_INIT_DONE_CMD_RESP;

///
/// BIOS action codes
///
#define DID_ACK_NON_PCR       0x1
#define DID_ACK_PCR           0x2
#define DID_ACK_RSVD3         0x3
#define DID_ACK_RSVD4         0x4
#define DID_ACK_RSVD5         0x5
#define DID_ACK_GRST          0x6
#define DID_ACK_CONTINUE_POST 0x7

#define MBP_CMD               0x2
#define MAX_MBP_SIZE          0x1000

#if AMT_SUPPORT
//
// MBP items definition
//
typedef struct _MBP_FW_VERSION_NAME_ITEM {
  MBP_ITEM_HEADER     Header;
  MBP_FW_VERSION_NAME MeFwVersionName;
} MBP_FW_VERSION_NAME_ITEM;

typedef struct _MBP_ICC_PROFILE_ITEM {
  MBP_ITEM_HEADER Header;
  MBP_ICC_PROFILE IccProfile;
} MBP_ICC_PROFILE_ITEM;

typedef struct _MBP_FW_CAPS_SKU_ITEM {
  MBP_ITEM_HEADER Header;
  MEFWCAPS_SKU    FwCapabilities;
} MBP_FW_CAPS_SKU_ITEM;

typedef struct _MBP_ROM_BIST_DATA_ITEM {
  MBP_ITEM_HEADER   Header;
  MBP_ROM_BIST_DATA RomBistData;
} MBP_ROM_BIST_DATA_ITEM;

typedef struct _MBP_PLATFORM_KEY_ITEM{
  MBP_ITEM_HEADER  Header;
  MBP_PLATFORM_KEY PlatformKey;
} MBP_PLATFORM_KEY_ITEM;

typedef struct _MBP_PLAT_TYPE_ITEM {
  MBP_ITEM_HEADER         Header;
  PLATFORM_TYPE_RULE_DATA RuleData;
} MBP_PLAT_TYPE_ITEM;

typedef struct _MBP_HWA_REQ_ITEM {
  MBP_ITEM_HEADER Header;
  HWA_DATA        Data;
} MBP_HWA_REQ_ITEM;

typedef struct _PLAT_BOOT_PERF_DATA_ITEM {
  MBP_ITEM_HEADER Header;
  MBP_PERF_DATA   MbpPerfData;
} PLAT_BOOT_PERF_DATA_ITEM;

typedef struct _MBP_NFC_SUPPORT_ITEM {
  MBP_ITEM_HEADER Header;
  MBP_NFC_DATA    NfcData;
} MBP_NFC_SUPPORT_ITEM;

typedef struct _MFS_INTEGRITY_ITEM {
  MBP_ITEM_HEADER Header;
  UINT32          MFSIntegrity;
} MFS_INTEGRITY_ITEM;

///
/// All items in a MKHI msg
///
typedef struct _MBP_ITEMS {
  MBP_FW_VERSION_NAME_ITEM FwVersionNameItem;
  MBP_FW_CAPS_SKU_ITEM     FwCapsSkuItem;
  MBP_ROM_BIST_DATA_ITEM   RomBistDataItem;
  MBP_PLATFORM_KEY_ITEM    PlatformKeyItem;
  MBP_PLAT_TYPE_ITEM       FwPlatTypeItem;
  MFS_INTEGRITY_ITEM       MFSIntegrityItem;
  PLAT_BOOT_PERF_DATA_ITEM PlatBootPerfDataItem;
  MBP_ICC_PROFILE_ITEM     IccProfileItem;
  MBP_HWA_REQ_ITEM         HwaRequestItem;  
  MBP_NFC_SUPPORT_ITEM     NfcSupportItem;
} MBP_ITEMS;

typedef struct _MBP_CMD_REQ
{
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              SkipMbp;
} MBP_CMD_REQ;

typedef struct _MBP_CMD_RESP
{
  MKHI_MESSAGE_HEADER MkhiHeader;
  MBP_HEADER          MbpHeader;
  UINT32              MbpItems[1];
} MBP_CMD_RESP;

#endif // AMT_SUPPORT

#define ME_ENABLE_CMD            0x3

typedef struct _ME_ENABLE_CMD_REQ {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ME_ENABLE_CMD_REQ;

typedef struct _ME_ENABLE_CMD_RESP {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ME_ENABLE_CMD_RESP;

#define HMRFPO_DISABLE_CMD  0x4
#define HMRFPO_DISABLE_RESP 0x84

typedef struct _HMRFPO_DISABLE_CMD_REQ {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_DISABLE_CMD_REQ;

typedef struct _HMRFPO_DISABLE_CMD_RESP {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_DISABLE_CMD_RESP;

#define TRACING_ENABLE_CMD       0x5
#define TRACING_ENABLE_RESP      0x85

typedef struct _TRACING_ENABLE_CMD_REQ {
  MKHI_MESSAGE_HEADER MkhiHeader;
} TRACING_ENABLE_CMD_REQ;

typedef struct _TRACING_ENABLE_CMD_RESP {
  MKHI_MESSAGE_HEADER MkhiHeader;
} TRACING_ENABLE_CMD_RESP;

///
/// ICC group defines
///
#define ICC_CMD  0x01

typedef struct _ICC_MESSAGE_REQ_DATA {
  UINT32 BClkRampOnWarmResetEn:1;   ///< BIT0
  UINT32 Reserved:31;               ///< BIT1-31
}ICC_MESSAGE_REQ_DATA;

typedef struct _ICC_MESSAGE_RESP_DATA {
  UINT32 FwNeedsWarmResetFlag:1;    ///< BIT0
  UINT32 Reserved:31;               ///< BIT1-31
}ICC_MESSAGE_RESP_DATA;

typedef struct _ICC_CMD_REQ {
  MKHI_MESSAGE_HEADER  MkhiHeader;
  ICC_MESSAGE_REQ_DATA Data;
} ICC_CMD_REQ;

typedef struct _ICC_CMD_RESP {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  ICC_MESSAGE_RESP_DATA Data;
} ICC_CMD_RESP;

///
/// HSIO group defines
///
#define HSIO_CMD  0x1

///
/// BIOS HSIO request types
///
#define HSIO_CLOSE_INTERFACE_REQ    0x0
#define HSIO_REPORT_VERSION_REQ     0x1

typedef struct _HSIO_CMD_REQ {
  MKHI_MESSAGE_HEADER   MkhiHeader;  
} HSIO_CMD_REQ;

typedef struct _HSIO_MESSAGE_RESP_DATA {
  UINT16 Crc;
  UINT8  Version;
  UINT8  Family;  
} HSIO_MESSAGE_RESP_DATA;

typedef struct _HSIO_CMD_RESP {
  MKHI_MESSAGE_HEADER    MkhiHeader;
  HSIO_MESSAGE_RESP_DATA Data;
} HSIO_CMD_RESP;

///
/// PM group defines
///
#define NPCR_NOTIFICATION_CMD  0x1

typedef struct _NPCR_NOTIFICATION_CMD_REQ {
  MKHI_MESSAGE_HEADER MkhiHeader;
} NPCR_NOTIFICATION_CMD_REQ;

typedef struct _NPCR_NOTIFICATION_CMD_RESP {
  MKHI_MESSAGE_HEADER MkhiHeader;
} NPCR_NOTIFICATION_CMD_RESP;


//
// Defines the HECI request buffer format for the ICC_HSIO_WRITE_SETTINGS_CMD.
//
typedef struct _ICC_HSIO_WRITE_SETTINGS_REQ {
   ICC_HEADER  Header;      // Standard ICC HECI Header
   UINT32      Reserved;    // Reserved for future use
} HSIO_WRITE_SETTINGS_REQ;

#pragma pack()

#endif // _BUP_MSGS_H

