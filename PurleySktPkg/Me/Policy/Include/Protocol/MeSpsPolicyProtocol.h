/** @file
  Interface definition details between ME and platform drivers during DXE phase.

@copyright
  Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
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
#ifndef _ME_SPS_POLICY_PROTOCOL_H_
#define _ME_SPS_POLICY_PROTOCOL_H_

typedef struct _SPS_POLICY_PROTOCOL_ SPS_POLICY_PROTOCOL;

#include <Protocol/MePolicy.h>
#include <Config/MeSpsConfig.h>
#include <Guid/MeRcVariable.h>

//
// ME SPS policy provided by platform for DXE phase
// This protocol provides an interface to get Intel SPS ME Configuration information
//

//
// Initial Revision
//
#define SPS_POLICY_PROTOCOL_REVISION  1

#ifdef AMT_SUPPORT

#define AMT_BITMASK   0x00000004
#define NFC_BITMASK   0x80000000

#define CLEAR_FEATURE_BIT 0x00000000

#pragma pack(1)
typedef struct
{
  //
  // This member determines the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS
  // Type 14 - Group Associations structure - item type. FVI structure uses it as
  // SMBIOS OEM type to provide MEBx, ME FW and reference code version information
  //
  UINT8  FviSmbiosType;
  UINT8  PreviousMeStateControl : 1;   // 0: Disabled; 1: Enabled
  UINT8  Reserved0              : 7;

  UINT8  PreviousLocalFwUpdEnabled;

  UINT8  ByteReserved[8];
} ME_AMT_CONFIG;
#pragma pack()
#endif // AMT_SUPPORT


typedef
EFI_STATUS
(EFIAPI *EFI_SAVE_ME_SPS_POLICY_TO_SETUP_CONFIGURATION) (
   SPS_POLICY_PROTOCOL *This
  );

typedef
EFI_STATUS
(EFIAPI *UPDATE_ME_SPS_POLICY_FROM_SETUP) (
  SPS_POLICY_PROTOCOL     *SpsPolicyInstance,
  ME_RC_CONFIGURATION     *pSetupData,
  BOOLEAN                 *pResetRequest
  );

//
// ME Policy Dump type
//
typedef enum {
  ME_POLICY_DUMP_ME              = 1,
  ME_POLICY_DUMP_AMT             = 2,
  ME_POLICY_DUMP_SPS             = 4
} ME_POLICY_DUMP_TYPE;

typedef
VOID
(EFIAPI *ME_SPS_POLICY_DEBUG_DUMP) (
  UINTN                   DebugLevel,
  ME_POLICY_DUMP_TYPE     WhatToDump
  );

//
// ME DXE SPS Policy
// This protocol provides information of the current Intel ME feature selection. Information is
// passed from the platform code to the Intel ME Reference code using this structure. There are
// 2 types of information, BIOS setup option and ME status information.
//
typedef struct _SPS_POLICY_PROTOCOL_
{
  //
  // Revision for the protocol structure
  //
  UINT8                           Revision;
  UINT8                           MeType;
  //
  // Intel ME feature selection enable/disable and firmware configuration information
  //
#if SPS_SUPPORT
  ME_SPS_CONFIG                   SpsConfig;
#endif
#if AMT_SUPPORT
  //
  // Intel ME AMT feature selection
  // TBD: Move it from SPS policy to AMT
  //
  ME_AMT_CONFIG                   MeAmtConfig;
#endif
  //
  // Variables required to Manage changes in Setup
  //
  BOOLEAN                         AnythingChanged; // any change after After EndOfPost requires reset

  //
  // Protocol functions
  //

  // Save ME Policy configuration to System Setup
  EFI_SAVE_ME_SPS_POLICY_TO_SETUP_CONFIGURATION  SaveSpsPolicyToSetupConfiguration;

  // Update ME Policy from setup e.g. after configuration change in UI
  UPDATE_ME_SPS_POLICY_FROM_SETUP                UpdateSpsPolicyFromSetup;

  // Support Localization for displaying on screen message
  ME_REPORT_ERROR                                MeReportError;
  BOOLEAN (*EnableRedirection) (BOOLEAN Enable); 
  // Dump ME Policy
  ME_SPS_POLICY_DEBUG_DUMP                       MePolicyDump;
} SPS_POLICY_PROTOCOL;

extern EFI_GUID gSpsPolicyProtocolGuid;

#endif
