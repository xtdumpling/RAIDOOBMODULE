/**@file

@copyright
 Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef __ME_SETUP_H__
#define __ME_SETUP_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AMT_SUPPORT

#include <MeBiosExtensionSetup.h>

#define ME_MODE_MASK                  0x0F
#define NORMAL_MODE                   0x00
#define TEMP_DISABLE_MODE             0x03
#define ME_IMAGE_CONSUMER_SKU_FW      0x03
#define ME_IMAGE_CORPORATE_SKU_FW     0x04

#define ME_FW_IMAGE_TYPE_CONSUMER_STRING       "Consumer SKU"
#define ME_FW_IMAGE_TYPE_CORPORATE_STRING      "Corporate SKU"
#define ME_FW_IMAGE_TYPE_UNIDENTIFIED_STRING   "Unidentified"

#define STR_NFC_DISABLED    "Disabled"
#define STR_NFC_MODULE_1    ""
#define STR_NFC_MODULE_2    "NXP PN544"
#define STR_NFC_MODULE_3    "NXP NPC100"

#define ME_RESET            L"MeReset"

#pragma pack(1)

typedef struct _ME_SETUP_STORAGE {
  UINT8  MngState;
  UINT8  LocalFwUpdEnabled;
  UINT8  MeStateControl;
  UINT8  AfterEoP;
  UINT8  RemoteSessionActive;
  UINT8  PttState;
  UINT8  TpmDeactivate;
  UINT8  BootGuardSupport;
  UINT8  MeasureBoot;
  UINT8  MeUnconfigOnRtcClear;
  UINT8  NfcState;
} ME_SETUP_STORAGE;

typedef struct _ME_INFO_SETUP_DATA {
  UINT8   MeFirmwareInfo;
  UINT32  MeMajor;
  UINT32  MeMinor;
  UINT32  MeHotFix;
  UINT32  MeBuildNo;
  UINT32  MeFileSystem;
} ME_INFO_SETUP_DATA;

#pragma pack()

extern EFI_GUID           gMeInfoSetupGuid;
extern ME_INFO_SETUP_DATA mMeInfoSetupData;

#endif // AMT_SUPPORT

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
