/** @file
  Implements Platform Trust Technology (PTT) HECI SkuMgr Interface Library.

@copyright
 Copyright (c) 2012 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/DxeMeLib.h>
#include <Library/PttHeciLib.h>
#include <Library/HobLib.h>
#include <Library/MeTypeLib.h>

#if defined(AMT_SUPPORT) && AMT_SUPPORT
#include <MeBiosPayloadHob.h>
#endif //AMT_SUPPORT

#if defined(SPS_SUPPORT) && SPS_SUPPORT
#include <Guid/SpsInfoHobGuid.h>
#endif //SPS_SUPPORT

#define CLEAR_FEATURES_BITMASK 0x00000000
#define PTT_BITMASK            BIT29

/**
  Checks whether ME FW has the Platform Trust Technology capability.

  @param[out] PttCapability      TRUE if PTT is supported, FALSE othewrwise.

  @retval EFI_SUCCESS            Command succeeded
**/
EFI_STATUS
EFIAPI
PttHeciGetCapability (
  OUT BOOLEAN   *PttCapability
  )
{
#if defined(AMT_SUPPORT) && AMT_SUPPORT
  ME_BIOS_PAYLOAD_HOB       *AmtHob;
#endif //AMT_SUPPORT
#if defined(SPS_SUPPORT) && SPS_SUPPORT
  EFI_HOB_GUID_TYPE         *SpsHob;
  SPS_INFO_HOB              *SpsHobData;
#endif //SPS_SUPPORT

  if (PttCapability == NULL) {
    ASSERT(PttCapability != NULL);
    return EFI_INVALID_PARAMETER;
  }

  *PttCapability = FALSE;

#if defined(AMT_SUPPORT) && AMT_SUPPORT
  if (MeTypeIsAmt())
  {
    AmtHob = GetFirstGuidHob(&gMeBiosPayloadHobGuid);
    if (AmtHob != NULL)
    {
      *PttCapability = (BOOLEAN)AmtHob->MeBiosPayload.FwCapsSku.FwCapabilities.Fields.PTT;
    }
  }
#endif //AMT_SUPPORT
#if defined(SPS_SUPPORT) && SPS_SUPPORT
  if (MeTypeIsSps())
  {
    SpsHob = GetFirstGuidHob(&gSpsInfoHobGuid);
    if (SpsHob != NULL)
    {
      SpsHobData = GET_GUID_HOB_DATA(SpsHob);
      *PttCapability = (BOOLEAN)SpsHobData->FeatureSet.Bits.PTT;
    }
  }
#endif //SPS_SUPPORT
  
  DEBUG ((DEBUG_INFO, "[ME] PTT SkuMgr: PttCapability = %d\n", *PttCapability));
  
  return EFI_SUCCESS;
}


/**
  Checks Platform Trust Technology enablement state.

  @param[out] IsPttEnabledState  TRUE if PTT is enabled, FALSE othewrwise.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciGetState (
  OUT BOOLEAN   *IsPttEnabledState
  )
{
  EFI_STATUS                 Status;
  MEFWCAPS_SKU               CurrentFeatures;

  *IsPttEnabledState = FALSE;

  if ( MeTypeIsInRecovery ()) {
    return EFI_UNSUPPORTED;
  }

  Status = HeciGetFwFeatureStateMsg (&CurrentFeatures);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (CurrentFeatures.Fields.PTT) {
    *IsPttEnabledState = TRUE;
  }

  DEBUG((DEBUG_INFO, "[ME] PTT SkuMgr: PttState = %d\n", *IsPttEnabledState));

  return EFI_SUCCESS;
}

/**
  Changes current Platform Trust Technology state.

  @param[in] PttEnabledState     TRUE to enable, FALSE to disable.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciSetState (
  IN  BOOLEAN   PttEnabledState
  )
{
  EFI_STATUS      Status;
  UINT32          EnableBitmap;
  UINT32          DisableBitmap;

  if (PttEnabledState) {
    //
    // Enable PTT
    //
    DEBUG ((DEBUG_INFO, "[ME] PTT SkuMgr: Enable PTT\n"));
    EnableBitmap  = PTT_BITMASK;
    DisableBitmap = CLEAR_FEATURES_BITMASK;
  } else {
    //
    // Disable PTT
    //
    DEBUG ((DEBUG_INFO, "[ME] PTT SkuMgr: Disable PTT\n"));
    EnableBitmap  = CLEAR_FEATURES_BITMASK;
    DisableBitmap = PTT_BITMASK;
  }
  Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);

  return Status;
}

/**
  Updates Platform Trust Technology state to new state.

  @param[in] NewPttState         TRUE to enable, FALSE to disable.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciUpdateState (
  IN  BOOLEAN   NewPttState
  )
{
  EFI_STATUS  Status;
  BOOLEAN     PttCurrentState;

  DEBUG((EFI_D_INFO, "[ME] PttHeciUpdateState(%d)\n", NewPttState));
  Status = PttHeciGetState(&PttCurrentState);
  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "[ME] Current Ptt State: %d\n", PttCurrentState));
    if (PttCurrentState != NewPttState) {
      DEBUG((EFI_D_INFO, "[ME] Run PttHeciSetState(%d)\n", NewPttState));
      Status = PttHeciSetState(NewPttState);
    }
  }

  return Status;
}
