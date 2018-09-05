/** @file

@copyright
 Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
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

#ifndef _DXE_ME_POLICY_UPDATE_H_
#define _DXE_ME_POLICY_UPDATE_H_

#include <PiDxe.h>
#include <HeciRegs.h>
#include <MeSetup.h>
#include <Guid/MeRcVariable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>
#include <IndustryStandard/Acpi10.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/MePolicy.h>
#include <Library/HobLib.h>
#include <MeBiosPayloadHob.h>

#define AMT_BITMASK   0x00000004
#define CLEAR_AMT_BIT 0x00000000

#define PLATFORM_BOOT_TABLE_PTR_TYPE   0x1001
#define PLATFORM_BOOT_RECORD_TYPE      0x1022
#define CONVERSION_MULTIPLIER          1000000  ///< msec to nanosec multiplier
#define PLATFORM_BOOT_TABLE_SIGNATURE  SIGNATURE_32 ('P', 'B', 'P', 'T')

extern EFI_GUID                        gMeInfoSetupGuid;

//
// Platform Boot Performance Table Record
//

typedef struct {
  UINT16 Type;
  UINT8  Length;
  UINT8  Revision;
  UINT32 Reserved;
  UINT64 TimestampDelta1;
  UINT64 TimestampDelta2;
  UINT64 TimestampDelta3;
} PLATFORM_BOOT_TABLE_RECORD;

//
// Platform boot Performance Table
//

typedef struct {
  EFI_ACPI_COMMON_HEADER     Header;
  PLATFORM_BOOT_TABLE_RECORD PlatformBoot;
} PLATFORM_BOOT_PERFORMANCE_TABLE;

/**
  Update ME Policy while MePlatformProtocol is installed.

  @param[in] MePolicyInstance     Instance of ME Policy Protocol
  @param[in] MeSetupPtr           A point to ME Setup Data
  @param[in] MeSetupStoragePtr    A point to ME Setup Storage

**/
VOID
UpdateMePolicyFromSetup (
  IN ME_POLICY_PROTOCOL           *MePolicyInstance,
  IN ME_RC_CONFIGURATION          *MeSetupPtr
  )
;

/**
  Functions performs HECI exchange with FW to update MePolicy settings.

  @param[in] Event         A pointer to the Event that triggered the callback.
  @param[in] Context       A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
UpdateMeSetupCallback (
  IN  EFI_EVENT                       Event,
  IN  VOID                            *Context
  )
;

#endif
