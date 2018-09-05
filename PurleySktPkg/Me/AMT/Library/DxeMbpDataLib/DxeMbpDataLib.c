/** @file
  This is a library to get Intel MBP Data.

@copyright
 Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MbpDataLib.h>
#include <MkhiMsgs.h>
#include <Library/HobLib.h>
#include <MeBiosPayloadHob.h>

/**
  MBP gets Firmware update info from ME client

  @param[in] MECapability         Structure of FirmwareUpdateInfo

  @exception EFI_UNSUPPORTED      No MBP Data Protocol available
**/
EFI_STATUS
MbpGetMeFwInfo (
  IN OUT ME_CAP *MECapability
  )
{
  MEFWCAPS_SKU          FwCapsSku;
  ME_BIOS_PAYLOAD_HOB   *MbpHob;

  MbpHob = NULL;

  //
  // Get Mbp Data HOB
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob == NULL) {
    DEBUG ((DEBUG_ERROR, "HeciGetMeFwInfo: No MBP Data Protocol available\n"));
    return EFI_UNSUPPORTED;
  }

  MECapability->MeEnabled = 1;

  FwCapsSku.Data          = MbpHob->MeBiosPayload.FwCapsSku.FwCapabilities.Data;

  if (FwCapsSku.Fields.KVM) {
    MECapability->IntelKVM = 1;
  }

  switch (MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.PlatformBrand) {
    case INTEL_AMT_BRAND:
      MECapability->IntelAmtFw        = 1;
      MECapability->LocalWakeupTimer  = 1;
      break;

    case INTEL_STAND_MANAGEABILITY_BRAND:
      MECapability->IntelAmtFwStandard = 1;
      break;

    case INTEL_SMALL_BUSINESS_TECHNOLOGY_BRAND:
      MECapability->IntelSmallBusiness = 1;
      break;
  }

  MECapability->MeMajorVer  = MbpHob->MeBiosPayload.FwVersionName.MajorVersion;
  MECapability->MeMinorVer  = MbpHob->MeBiosPayload.FwVersionName.MinorVersion;
  MECapability->MeBuildNo   = MbpHob->MeBiosPayload.FwVersionName.BuildVersion;
  MECapability->MeHotFixNo  = MbpHob->MeBiosPayload.FwVersionName.HotfixVersion;

  return EFI_SUCCESS;
}