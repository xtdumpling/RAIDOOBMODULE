/** @file
  Dump whole MBP_DATA_PROTOCOL and serial out.

@copyright
 Copyright (c) 2012 - 2015 Intel Corporation. All rights reserved
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
#if AMT_SUPPORT

#include <Library/DebugLib.h>
#include <MeBiosPayloadData.h>
#include "MbpData.h"

/**
  Dump MBP_DATA_PROTOCOL

  @param[in] MbpData              Pointer to MBP_DATA_PROTOCOL

**/
VOID
MbpDebugPrint (
  IN UINT8                     MbpRevision,
  IN UINT8                     MbpSensitiveRevision,
  IN ME_BIOS_PAYLOAD           *MbpPtr,
  IN ME_BIOS_PAYLOAD_SENSITIVE *MbpSensitivePtr
  )
{
  UINTN i;

  if (MbpPtr == NULL) {
    return;
  }
  DEBUG ((DEBUG_INFO, "\n------------------------ MeBiosPayload Data Protocol Print Begin -----------------\n"));
  DEBUG ((DEBUG_INFO, " Revision : 0x%x\n", MbpRevision));
  DEBUG ((DEBUG_INFO, "MeBiosPayload FwVersionName ---\n"));
  DEBUG ((DEBUG_INFO, " ME FW MajorVersion : 0x%x\n", MbpPtr->FwVersionName.MajorVersion));
  DEBUG ((DEBUG_INFO, " ME FW MinorVersion : 0x%x\n", MbpPtr->FwVersionName.MinorVersion));
  DEBUG ((DEBUG_INFO, " ME FW HotfixVersion : 0x%x\n", MbpPtr->FwVersionName.HotfixVersion));
  DEBUG ((DEBUG_INFO, " ME FW BuildVersion : 0x%x\n", MbpPtr->FwVersionName.BuildVersion));

  if (MbpPtr->FwCapsSku.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload FwCapabilities ---\n"));
    DEBUG ((DEBUG_INFO, " FullNet : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.FullNet));
    DEBUG ((DEBUG_INFO, " StdNet : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.StdNet));
    DEBUG ((DEBUG_INFO, " Manageability : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.Manageability));
    DEBUG ((DEBUG_INFO, " SmallBusiness : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.SmallBusiness));
    DEBUG ((DEBUG_INFO, " IntegratedTouch : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.IntegratedTouch));
    DEBUG ((DEBUG_INFO, " IntelCLS : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.IntelCLS));
    DEBUG ((DEBUG_INFO, " ISH : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.ISH));
    DEBUG ((DEBUG_INFO, " PAVP : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.PAVP));
    DEBUG ((DEBUG_INFO, " IPV6 : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.IPV6));
    DEBUG ((DEBUG_INFO, " KVM : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.KVM));
    DEBUG ((DEBUG_INFO, " OCH : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.OCH));
    DEBUG ((DEBUG_INFO, " DAL : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.DAL));
    DEBUG ((DEBUG_INFO, " TLS : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.TLS));
    DEBUG ((DEBUG_INFO, " WLAN : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.WLAN));
    DEBUG ((DEBUG_INFO, " PTT : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.PTT));
    DEBUG ((DEBUG_INFO, " NFC : 0x%x\n", MbpPtr->FwCapsSku.FwCapabilities.Fields.NFC));
  }

  if (MbpPtr->FwFeaturesState.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload FwFeaturesState ---\n"));
    DEBUG ((DEBUG_INFO, " FullNet : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.FullNet));
    DEBUG ((DEBUG_INFO, " StdNet : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.StdNet));
    DEBUG ((DEBUG_INFO, " Manageability : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.Manageability));
    DEBUG ((DEBUG_INFO, " SmallBusiness : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.SmallBusiness));
    DEBUG ((DEBUG_INFO, " IntegratedTouch : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.IntegratedTouch));
    DEBUG ((DEBUG_INFO, " IntelCLS : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.IntelCLS));
    DEBUG ((DEBUG_INFO, " ISH : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.ISH));
    DEBUG ((DEBUG_INFO, " PAVP : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.PAVP));
    DEBUG ((DEBUG_INFO, " IPV6 : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.IPV6));
    DEBUG ((DEBUG_INFO, " KVM : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.KVM));
    DEBUG ((DEBUG_INFO, " OCH : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.OCH));
    DEBUG ((DEBUG_INFO, " DAL : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.DAL));
    DEBUG ((DEBUG_INFO, " TLS : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.TLS));
    DEBUG ((DEBUG_INFO, " WLAN : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.WLAN));
    DEBUG ((DEBUG_INFO, " PTT : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.PTT));
    DEBUG ((DEBUG_INFO, " NFC : 0x%x\n", MbpPtr->FwFeaturesState.FwFeatures.Fields.NFC));
  }

  if (MbpPtr->FwPlatType.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload ME Platform TYpe ---\n"));
    DEBUG ((DEBUG_INFO, " PlatformTargetUsageType : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.PlatformTargetUsageType));
    DEBUG ((DEBUG_INFO, " SuperSku : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.SuperSku));
    DEBUG ((DEBUG_INFO, " IntelMeFwImageType : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.IntelMeFwImageType));
    DEBUG ((DEBUG_INFO, " PlatformBrand : 0x%x\n", MbpPtr->FwPlatType.RuleData.Fields.PlatformBrand));
  }

  DEBUG ((DEBUG_INFO, "MeBiosPayload IccProfile ---\n"));
  DEBUG ((DEBUG_INFO, " IccNumOfProfiles : 0x%x\n",  MbpPtr->IccProfile.IccNumOfProfiles));
  DEBUG ((DEBUG_INFO, " IccProfileIndex : 0x%x\n",  MbpPtr->IccProfile.IccProfileIndex));
  DEBUG ((DEBUG_INFO, " IccProfileChangeable : 0x%x\n",  MbpPtr->IccProfile.IccProfileChangeable));
  DEBUG ((DEBUG_INFO, " IccLockMaskType : 0x%x\n",  MbpPtr->IccProfile.IccLockMaskType));

  DEBUG ((DEBUG_INFO, "MeBiosPayload MFSIntegrity: 0x%x\n", MbpPtr->MFSIntegrity));

  if (MbpPtr->HwaRequest.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload HwaRequest ---\n"));
    DEBUG ((DEBUG_INFO, " MediaTablePush : 0x%x\n", MbpPtr->HwaRequest.Data.Fields.MediaTablePush));
  }

  if (MbpPtr->NfcSupport.Available == TRUE) {
    DEBUG ((DEBUG_INFO, "MeBiosPayload NFC ---\n"));
    DEBUG ((DEBUG_INFO, " DeviceType : 0x%x\n", MbpPtr->NfcSupport.NfcData.DeviceType));
  }

  DEBUG ((DEBUG_INFO, " Sensitive Data Revision : 0x%x\n", MbpSensitiveRevision));
  DEBUG ((DEBUG_INFO, "MeBiosPayload Platform Key ---\n"));
  for (i = 0; i < 8; i++) {
    DEBUG ((DEBUG_INFO, " Key[0x%x] : 0x%x \n", i, MbpSensitivePtr->PlatformKey.Key[i]));
  }

  DEBUG ((DEBUG_INFO, "MeBiosPayload RomBistData ---\n"));
  DEBUG ((DEBUG_INFO, " DeviceId : 0x%x\n", MbpSensitivePtr->RomBistData.DeviceId));
  DEBUG ((DEBUG_INFO, " FuseTestFlags : 0x%x\n", MbpSensitivePtr->RomBistData.FuseTestFlags));
  for (i = 0; i < 4; i++) {
    DEBUG ((DEBUG_INFO, " UMCHID[0x%x] : 0x%x \n", i, MbpSensitivePtr->RomBistData.UMCHID[i]));
  }

  DEBUG ((DEBUG_INFO, "\n------------------------ MeBiosPayload Data Protocol Print End -------------------\n"));
}

#endif // AMT_SUPPORT