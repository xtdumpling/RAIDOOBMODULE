/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement.
--*/
/*++
Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

 SpsSmmHmrfpoProtocol.h

Abstract:
--*/
#ifndef _SPS_SMM_HMRFPO_PROTOCOL_H_
#define _SPS_SMM_HMRFPO_PROTOCOL_H_

extern EFI_GUID gSpsSmmHmrfpoProtocolGuid;

typedef struct _SPS_SMM_HMRFPO_PROTOCOL_ _SPS_SMM_HMRFPO_PROTOCOL;

typedef EFI_STATUS (EFIAPI *HMRFPO_LOCK_FUNC)(void);

typedef EFI_STATUS (EFIAPI *HMRFPO_ENABLE_FUNC)(void);

typedef EFI_STATUS (EFIAPI *HMRFPO_MERESET_FUNC)(void);

// APTIOV_SERVER_OVERRIDE_RC_START : Provide Nonce in SpsSmm.
typedef UINT64 (EFIAPI *HMRFPO_GET_NONCE)(void);
// APTIOV_SERVER_OVERRIDE_RC_END : Provide Nonce in SpsSmm.

typedef struct
{
  UINT16 Minor;
  UINT16 Major;
  UINT16 Build;
  UINT16 Patch;
} ME_FW_VERSION;

typedef struct _SPS_SMM_HMRFPO_PROTOCOL_ {

  ME_FW_VERSION   ActiveFwVersion;     // Active operational firmware
  ME_FW_VERSION   RecoveryFwVersion;   // Recovery firmware
  ME_FW_VERSION   BackupFwVersion;     // Backup operational firmware (optional)

  HMRFPO_LOCK_FUNC     Lock;
  HMRFPO_ENABLE_FUNC   Enable;
  HMRFPO_MERESET_FUNC  MeReset;
  // APTIOV_SERVER_OVERRIDE_RC_START : Provide Nonce in SpsSmm.
  HMRFPO_GET_NONCE     GetNonce;
  // APTIOV_SERVER_OVERRIDE_RC_END : Provide Nonce in SpsSmm.

} SPS_SMM_HMRFPO_PROTOCOL;

#endif // _SPS_SMM_HMRFPO_PROTOCOL_H_

