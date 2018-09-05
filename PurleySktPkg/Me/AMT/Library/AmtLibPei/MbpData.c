/** @file
  ME PayLoad Data implementation.

@copyright
  Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#include <PiPei.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/PerformanceLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <MeChipset.h>
#include <HeciRegs.h>
#include <PchAccess.h>
#include <IndustryStandard/Pci22.h>
#include <Library/HeciMsgLib.h>
#include "Library/MeTypeLib.h"
#include <Ppi/MbpSensitivePpi.h>
#include <MeBiosPayloadHob.h>
#include <BupMsgs.h>
#include "MbpData.h"

#define EVENT_REC_TOK                   "EventRec"

//
// Function Declarations
//
static MBP_SENSITIVE_PPI               mMbpSensitivePpi = {
  MBP_SENSITIVE_PPI_REVISION_1,
  { 0 }
};

static EFI_PEI_PPI_DESCRIPTOR mInstallMbpSensitivePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gMbpSensitivePpiGuid,
  &mMbpSensitivePpi
};


/**
  Install MbpData protocol.

#ifndef SERVER_BIOS_FLAG
  @param[in]  HeciPpi                 The pointer with Heci Ppi
#endif // SERVER_BIOS_FLAG
  @param[out] MbpHobPtr               The pointer with MbpHob installed.
**/
VOID
InstallMbp (
  OUT ME_BIOS_PAYLOAD_HOB  **MbpHobPtr
  )
{
  EFI_STATUS                          Status;
  ME_BIOS_PAYLOAD                     MbpData;
  ME_BIOS_PAYLOAD_SENSITIVE           MbpSensitiveData;
  ME_BIOS_PAYLOAD_HOB                 *MbpHob;

  ///
  /// Check if it's DFX ME FW first
  ///
  if (MeTypeIsDfx()) {
    return;
  }

  ///
  /// Check if ME FW is disabled/debug mode
  ///
  if (MeTypeIsDisabled()) {
    return;
  }

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc (), 0x8010);
  Status = PrepareMeBiosPayload (&MbpData, &MbpSensitiveData);
  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc (), 0x8011);

  if (!EFI_ERROR (Status)
#if USE_PCD_SKIP_MBP_HOB
      && !PcdGetBool(PcdSkipMbpHob)
#endif // USE_PCD_SKIP_MBP_HOB
     ) {
    ///
    /// Create MBP HOB
    ///
    Status = PeiServicesCreateHob (
              EFI_HOB_TYPE_GUID_EXTENSION,
              sizeof (ME_BIOS_PAYLOAD_HOB),
              (VOID**) &MbpHob
              );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "MeBiosPayloadHob could not be created.\n"));
      ASSERT_EFI_ERROR (Status);
      return ;
    }

    //
    // Initialize MBP HOB
    //
    MbpHob->Header.Name   = gMeBiosPayloadHobGuid;
    MbpHob->Revision      = MBP_DATA_PROTOCOL_REVISION_1;
    CopyMem (&MbpHob->MeBiosPayload, &MbpData, sizeof (ME_BIOS_PAYLOAD));

    //
    // Check if selected members retrieved from FW are up to date. If not - send additional synch messages.

    if(MbpHob->MeBiosPayload.FwCapsSku.Available == FALSE) {
      //
      // FW Capabilities
      //  
      Status = PeiHeciGetFwCapsSkuMsg (&MbpHob->MeBiosPayload.FwCapsSku.FwCapabilities);
      if (!EFI_ERROR (Status)) {
        MbpHob->MeBiosPayload.FwCapsSku.Available = TRUE;
      }
    }
    if(MbpHob->MeBiosPayload.FwFeaturesState.Available == FALSE) {
      Status = PeiHeciGetFwFeatureStateMsg (&MbpHob->MeBiosPayload.FwFeaturesState.FwFeatures);
      //
      // FW Features State
      //
      if (!EFI_ERROR (Status)) {
        MbpHob->MeBiosPayload.FwFeaturesState.Available = TRUE;
      }
    }
    if(MbpHob->MeBiosPayload.FwPlatType.Available == FALSE) {
      //
      // ME Platform Type
      //
      Status = PeiHeciGetPlatformTypeMsg (&MbpHob->MeBiosPayload.FwPlatType.RuleData);
      if (!EFI_ERROR (Status)) {
        MbpHob->MeBiosPayload.FwPlatType.Available = TRUE;
      }
    }

    //
    // Initialize MBP sensitive data PPI
    //
    CopyMem (&mMbpSensitivePpi.MbpSensitiveData, &MbpSensitiveData, sizeof (ME_BIOS_PAYLOAD_SENSITIVE));

    //
    // Install MBP sensitive data PPI
    //
    Status = PeiServicesInstallPpi (&mInstallMbpSensitivePpi);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to install MbpSensitivePpi! Status = %r\n", Status));
      ASSERT_EFI_ERROR (Status);
    }
    if (MbpHobPtr) {
      *MbpHobPtr = MbpHob;
    }
    //
    // Dump the Mbp data
    //
    MbpDebugPrint (MbpHob->Revision, mMbpSensitivePpi.Revision, &MbpData, &MbpSensitiveData);
  }
}

/**
  This routine returns ME-BIOS Payload information.

  @param[out] MbpPtr              ME-BIOS Payload information.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        Failed to consume MBP
  @retval EFI_OUT_OF_RESOURCES    Out of resource
**/

EFI_STATUS
PrepareMeBiosPayload (
  OUT  ME_BIOS_PAYLOAD                *MbpPtr,
  OUT  ME_BIOS_PAYLOAD_SENSITIVE      *MbpSensitivePtr
  )
{
  EFI_STATUS                          Status;
  MBP_HEADER                          MbpHeader;
  UINT32                              *MbpItems;
  MBP_ITEM_HEADER                     MbpItemHeader;
  UINT32                              *Ptr;
  UINT32                              MbpItemId;
  UINT32                              MbpItemSize;
  UINT32                              i;
  UINT32                              Index;
  VOID                                *DestPtr;
  UINTN                               DestSize;
  UINT32                              *MbpItemPtr;

  DEBUG ((DEBUG_INFO, "PrepareMeBiosPayload () - Start\n"));

  MbpItems = AllocateZeroPool (MAX_MBP_SIZE);
  if (MbpItems == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = PeiHeciGetMbpMsg(
             &MbpHeader,
             MbpItems,
#if USE_PCD_SKIP_MBP_HOB
             PcdGetBool(PcdSkipMbpHob)
#else
             FALSE
#endif // USE_PCD_SKIP_MBP_HOB
           );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "HECI get MBP error: %r!", Status));
    goto Exit;
  }
#if USE_PCD_SKIP_MBP_HOB
  if (PcdGetBool(PcdSkipMbpHob)) {
    DEBUG ((DEBUG_ERROR, "Skipping MBP data due to PcdSkipMbp set!\n"));
    goto Exit;
  }
#endif // USE_PCD_SKIP_MBP_HOB

  DEBUG ((DEBUG_INFO, "HECI get MBP succeeded\n"));
  DEBUG ((DEBUG_INFO, "MBP header: %x\n", MbpHeader.Data));
  DEBUG ((DEBUG_INFO, "MbpSize: %x\n", MbpHeader.Fields.MbpSize));
  DEBUG ((DEBUG_INFO, "No. of Mbp Entries: %x\n", MbpHeader.Fields.NumEntries));
  DEBUG ((DEBUG_INFO, "Mbp flags: %x\n", MbpHeader.Fields.Flags));

  MbpPtr->FwCapsSku.Available                   = FALSE;
  MbpPtr->FwPlatType.Available                  = FALSE;
  MbpPtr->HwaRequest.Available                  = FALSE;
  MbpPtr->PlatBootPerfData.Available            = FALSE;
  MbpPtr->HwaRequest.Data.Fields.MediaTablePush = FALSE;
  MbpPtr->NfcSupport.Available                  = FALSE;

  MbpItemPtr = MbpItems;
  Ptr        = NULL;

  for (i = 0; i < MbpHeader.Fields.NumEntries; i++) {
    MbpItemHeader.Data = *MbpItemPtr;
    MbpItemPtr++;
    DEBUG ((DEBUG_INFO, "MBP Item %x header: %x\n", i + 1, MbpItemHeader.Data));

    MbpItemId = (MbpItemHeader.Fields.AppId << 4) | MbpItemHeader.Fields.ItemId;
    //
    // Copy data from HECI buffer per size of each MBP item
    //
    MbpItemSize = MbpItemHeader.Fields.Length - 1;
    DEBUG ((DEBUG_INFO, "MBP Item Size is: %x\n", MbpItemSize));
    if ((MbpItemSize == 0) || (MbpItemSize > (MAX_UINT32/4))) {
      DEBUG ((DEBUG_ERROR, "Skipping MBP Item (%d) - wrong size\n", MbpItemId));
      continue;
    }

    Ptr         = AllocateZeroPool ((MbpItemSize) * 4);
    ASSERT (Ptr);
    if (Ptr == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    for (Index = 0; Index < MbpItemSize; Index++) {
      Ptr[Index] = *MbpItemPtr;
      MbpItemPtr++;
      DEBUG ((DEBUG_INFO, "MBP Item Data: %x\n", Ptr[Index]));
    }

    switch (MbpItemId) {
      //
      // Insensitive items
      //
      case 0x11:
        //
        // FW Version Name
        //
        DestSize = sizeof (MbpPtr->FwVersionName);
        DestPtr = &MbpPtr->FwVersionName;
        break;

      case 0x12:
        //
        // FW Capabilities
        //
        DestSize = sizeof (MbpPtr->FwCapsSku.FwCapabilities);
        DestPtr = &MbpPtr->FwCapsSku.FwCapabilities;
        MbpPtr->FwCapsSku.Available = TRUE;
        break;

      case 0x15:
        //
        // ME Platform TYpe
        //
        DestSize = sizeof (MbpPtr->FwPlatType.RuleData);
        DestPtr = &MbpPtr->FwPlatType.RuleData;
        MbpPtr->FwPlatType.Available = TRUE;
        break;

      case 0x16:
        //
        // ME MFS status
        //
        DestSize = sizeof (MbpPtr->MFSIntegrity);
        DestPtr = &MbpPtr->MFSIntegrity;
        break;

      case 0x17:
        ///
        /// Platform timing information
        ///
        DestSize = sizeof (MbpPtr->PlatBootPerfData.MbpPerfData);
        DestPtr = &MbpPtr->PlatBootPerfData.MbpPerfData;
        MbpPtr->PlatBootPerfData.Available = TRUE;
        break;

      case 0x18:
        //
        // ME Unconfiguration on RTC clear state
        //
        DestSize = sizeof (MbpPtr->UnconfigOnRtcClearState.UnconfigOnRtcClearData);
        DestPtr = &MbpPtr->UnconfigOnRtcClearState.UnconfigOnRtcClearData;
        MbpPtr->UnconfigOnRtcClearState.Available = TRUE;
        break;

      case 0x19:
        //
        // FW Features State
        //
        DestSize = sizeof (MbpPtr->FwFeaturesState.FwFeatures);
        DestPtr = &MbpPtr->FwFeaturesState.FwFeatures;
        MbpPtr->FwFeaturesState.Available = TRUE;
        break;

      case 0x41:
        //
        // HWA Request
        //
        DestSize = sizeof (MbpPtr->HwaRequest.Data);
        DestPtr = &MbpPtr->HwaRequest.Data;
        MbpPtr->HwaRequest.Available = TRUE;
        break;

      case 0x51:
        //
        // ICC Profile
        //
        DestSize = sizeof (MbpPtr->IccProfile);
        DestPtr = &MbpPtr->IccProfile;
        break;

      case 0x61:
        //
        // NFC Data
        //
        DestSize = sizeof (MbpPtr->NfcSupport.NfcData);
        DestPtr = &MbpPtr->NfcSupport.NfcData;
        MbpPtr->NfcSupport.Available = TRUE;
        break;
      //
      // Sensitive items
      //
      case 0x13:
        //
        // ROM BIST Data
        //
        DestSize = sizeof (MbpSensitivePtr->RomBistData);
        DestPtr = &MbpSensitivePtr->RomBistData;
        break;

      case 0x14:
        //
        // Platform Key
        //
        DestSize = sizeof (MbpSensitivePtr->PlatformKey);
        DestPtr = &MbpSensitivePtr->PlatformKey;
        break;
      default:
        //
        // default case
        //
        DestSize = 0;
        DestPtr = NULL;
        break;
    }
    if (DestPtr != NULL) {
      if ((MbpItemSize * 4) <= DestSize) {
        CopyMem (DestPtr, Ptr, (MbpItemSize * 4));
      } else {
        DEBUG ((DEBUG_INFO, "Data size is larger than destination buffer. This item is not copied.\n"));
      }
      //
      // Clear buffer
      //
      ZeroMem (Ptr, MbpItemSize * 4);
      FreePool (Ptr);
    }
  }

Exit:
  FreePool (MbpItems);

  return Status;
}

#endif // #if AMT_SUPPORT