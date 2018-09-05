/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2007 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  Hecidrv.c

@brief:
  HECI driver for AMT support

**/
#if defined(AMT_SUPPORT) && AMT_SUPPORT

#include <PchAccess.h>
#include <Library/MmPciBaseLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PerformanceLib.h>

#include "CommonIncludes.h"
// STRING_REF definition required in DataHubRecords.h
typedef UINT16                    STRING_REF;
#include "Guid/DataHubRecords.h"
#include "Library/SiFviLib.h"
#include "Protocol/AmtReadyToBoot.h"
#include "Protocol/MebxProtocol.h"
#include "MeBiosExtensionSetup.h"
#include "Guid/MePlatformReadyToBoot.h"

#include <Protocol/Wdt.h>
#include <Protocol/PchReset.h>

#include "HeciInit.h"
#include "Hecidrv.h"
#include "Library/HeciCoreLib.h"
#include "HeciRegs.h"
#include "Library/DxeMeLib.h"
#include "Library/MeSpsPolicyAccessLib.h"
#include "Library/MeTypeLib.h"
#include "MeAccess.h"
#include "IndustryStandard/Pci.h"

#ifdef TCG_SUPPORT_FLAG
#include "Acpi1_0.h"
#include "Acpi2_0.h"
#include "Acpi3_0.h"

#include "TpmPc.h"
#endif /// TCG_SUPPORT_FLAG

#include "MeInit.h"

#include "MeBiosPayloadData.h"
#include "MeBiosPayloadHob.h"
#include <Library/HobLib.h>
#include <AmtSetup.h>
#include <IncludePrivate/PchPolicyHob.h>
#include <Library/PchPlatformLib.h>

#define ONE_SECOND_TIMEOUT  1000000
#define FWU_TIMEOUT         90
#define DEFAULT_ME_FW_STRING STR_ME_FW_SKU_CONSUMER_FW"   "
///
/// Global driver data
///
extern HECI_INSTANCE          *mHeciContext;
CHAR16                         gMeBiosExtensionSetupName[] = ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME;

FVI_DATA mMeFviElementsData[] = {
  DEFAULT_FVI_DATA(ME),
  DEFAULT_FVI_DATA(MEBX),
  FVI_VERSION (MEFW_VERSION),
};

FVI_HEADER mMeFviHeader = DEFAULT_FVI_HEADER_DATA(mMeFviElementsData);

FVI_STRINGS mMeFviStrings[] = {
  DEFAULT_FVI_STRINGS(ME),
  DEFAULT_FVI_STRINGS(MEBX),
  {
    STR_ME_FW_STRING,
    DEFAULT_ME_FW_STRING,
  },
};

#ifdef TCG_SUPPORT_FLAG
/**

  @brief
  Perform measurement for HER register.

  @param[in] HerValue             The value of HECI Extend Register.
  @param[in] Index                HerValue Size.

  @retval EFI_SUCCESS             Measurement performed

**/
EFI_STATUS
MeasureHer (
  IN  UINT32                      *HerValue,
  IN  UINT8                       Index
  )
{
  EFI_STATUS            Status;
  EFI_TCG_PROTOCOL      *TcgProtocol;
  EFI_TCG_PCR_EVENT     TcgEvent;
  UINT32                EventNumber;
  EFI_PHYSICAL_ADDRESS  EventLogLastEntry;

  Status = gBS->LocateProtocol (
                  &gEfiTcgProtocolGuid,
                  NULL,
                  &TcgProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// This below data will be stored in Tcg eventlog
  ///
  TcgEvent.Event.PostCode.PostCodeAddress = *HerValue;
  TcgEvent.Event.PostCode.PostCodeLength  = sizeof (UINT32);

  ///
  /// Fill the TcgEvent Header
  ///
  TcgEvent.Header.PCRIndex      = PCRi_CRTM_AND_POST_BIOS;
  TcgEvent.Header.EventType     = EV_S_CRTM_CONTENTS;

  TcgEvent.Header.EventDataSize = (Index * sizeof (UINT32));

  Status = TcgProtocol->HashLogExtendEvent (
                          TcgProtocol,
                          (EFI_PHYSICAL_ADDRESS) HerValue,
                          TcgEvent.Header.EventDataSize,
                          TPM_ALG_SHA,
                          (TCG_PCR_EVENT *) &TcgEvent,
                          &EventNumber,
                          &EventLogLastEntry
                          );
  return Status;
}

/**
  @brief
  Me Measurement.

  @param[in] None.

  @retval EFI_NOT_READY           Not ready for measurement.
  @retval EFI_SUCCESS             Measurement done

**/
EFI_STATUS
MeMeasurement (
  VOID
  )
{
  EFI_STATUS    Status;
  DATA32_UNION  Data32[7];
  UINT8         HerMax;
  UINT8         HerIndex;
  UINT8         Index;

  Index = 0;
  ///
  /// Measure HER
  ///
  HerMax                = R_ME_HER5;

  Data32[Index].Data32  = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_ME_HERS);
  if ((Data32[Index].Data32 & B_ME_EXTEND_REG_VALID) == B_ME_EXTEND_REG_VALID) {
    if ((Data32[Index].Data8[0] & B_ME_EXTEND_REG_ALGORITHM) == V_ME_SHA_256) {
      HerMax = R_ME_HER8;
    }

    for (HerIndex = R_ME_HER1, Index = 0; HerIndex <= HerMax; HerIndex += 4, Index++) {
      Data32[Index].Data32 = MmioRead32 (MmPciBase (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + HerIndex);
    }

    Status = MeasureHer (&Data32->Data32, Index);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "ME Measurement feature failed, Status is %r \n", Status));
    }
  } else {
    Status = EFI_NOT_READY;
  }

  return Status;
}

/**
  @brief
  Signal a event for last checking.  

  @param[in] Event                The event that triggered this notification function  
  @param[in] ParentImageHandle    Pointer to the notification functions context

  @retval EFI_SUCCESS             Event executed and closed.

**/
EFI_STATUS
MeMeasurementEvent (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *ParentImageHandle
  )
{
  DEBUG ((EFI_D_ERROR, "MeMeasurementEvent is called.\n"));

  MeMeasurement ();

  gBS->CloseEvent (Event);

  return EFI_SUCCESS;
}
#endif

/**
  @brief
  Show warning message to user.  

  @param[in] None.

  @retval EFI_SUCCESS             Warning reported

**/
EFI_STATUS
MeWarningMessage (
  VOID
  )
{
  HECI_FWS_REGISTER MeFirmwareStatus;

  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);

  ///
  /// Check for ME FPT Bad & FT BUP LD FLR
  ///
  if (MeFirmwareStatus.r.FptBad != 0 || MeFirmwareStatus.r.FtBupLdFlr != 0) {
    MeReportError (MSG_ME_FW_UPDATE_FAILED);
  }

  return EFI_SUCCESS;
}

/**
  @brief
  Halt Boot for up to 90 seconds if Bit 11 of FW Status Register (FW_UPD_IN_PROGRESS) is set

  @param[in] None

  @retval None

**/
VOID
CheckFwUpdInProgress (
  VOID
  )
{
  HECI_FWS_REGISTER FwStatus;
  UINT8             StallCount;
  EFI_STATUS        Status;

  StallCount  = 0;
  Status      = mHeciContext->HeciCtlr.GetMeStatus (&FwStatus.ul);
  if (!EFI_ERROR (Status)) {
    if (FwStatus.ul & ME_FW_UPDATES_IN_PROGRESS) {
      MeReportError (MSG_ME_FW_UPDATE_WAIT);
    }

    while ((FwStatus.ul & ME_FW_UPDATES_IN_PROGRESS) && (StallCount < FWU_TIMEOUT)) {
      gBS->Stall (ONE_SECOND_TIMEOUT);
      StallCount  = StallCount + 1;
      Status      = mHeciContext->HeciCtlr.GetMeStatus (&FwStatus.ul);
    }
  }

  return ;
}

/**

  @brief
  Dummy return for Me signal event use

  @param[in] Event                The event that triggered this notification function
  @param[in] ParentImageHandle    Pointer to the notification functions context

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
MeEmptyEventFunction (
  IN  EFI_EVENT                   Event,
  IN  void                        *ParentImageHandle
  )
{
  return EFI_SUCCESS;
}

/**
  @brief
  Signal a event for Me ready to boot.  

  @param[in] Event                The event that triggered this notification function  
  @param[in] ParentImageHandle    Pointer to the notification functions context

  @retval EFI_SUCCESS             Always return EDI_SUCCESS

**/
EFI_STATUS
MeReadyToBootEventFunction (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *ParentImageHandle
  )
{
  EFI_STATUS                  Status;
  EFI_EVENT                   MePlatformReadyToBootEvent;
  EFI_HANDLE                  *Handles;
  UINTN                       Index;
  UINTN                       Count;
  AMT_READY_TO_BOOT_PROTOCOL  *AmtReadyToBoot;
  UINT32                      MeMode;
  UINT32                      MeStatus;
#ifdef TCG_SUPPORT_FLAG
  EFI_EVENT                   LegacyBootEvent;
  EFI_EVENT                   ExitBootServicesEvent;
#endif // TCG_SUPPORT_FLAG
  static BOOLEAN              AlreadyCalled = FALSE;

  DEBUG ((EFI_D_ERROR, "MeReadyToBootEventFunction is called.\n"));

  if (AlreadyCalled) {
    DEBUG ((EFI_D_ERROR, "MeReadyToBootEventFunction is already called. Exit\n"));
    gBS->CloseEvent (Event);
    return EFI_SUCCESS;
  }
  AlreadyCalled = TRUE;

#ifdef TCG_SUPPORT_FLAG
  Status = MeMeasurement ();
  if (Status == EFI_NOT_READY) {
    ///
    /// Create a Legacy Boot event.
    ///
    Status = EfiCreateEventLegacyBootEx (
              TPL_CALLBACK,
              MeMeasurementEvent,
              NULL,
              &LegacyBootEvent
              );
    ASSERT_EFI_ERROR (Status);

    ///
    /// Create a ExitBootService event.
    ///
    Status = gBS->CreateEvent (
                    EVENT_SIGNAL_EXIT_BOOT_SERVICES,
                    TPL_CALLBACK,
                    MeMeasurementEvent,
                    NULL,
                    &ExitBootServicesEvent
                    );
    ASSERT_EFI_ERROR (Status);
  }

#endif ///TCG_SUPPORT_FLAG
  /// We will trigger all events in order
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gAmtReadyToBootProtocolGuid,
                  NULL,
                  &Count,
                  &Handles
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < Count; Index++) {
      Status = gBS->HandleProtocol (Handles[Index], &gAmtReadyToBootProtocolGuid, &AmtReadyToBoot);
      ASSERT_EFI_ERROR (Status);
      AmtReadyToBoot->Signal ();
    }
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  MeEmptyEventFunction,
                  NULL,
                  &gMePlatformReadyToBootGuid,
                  &MePlatformReadyToBootEvent
                  );
  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent (MePlatformReadyToBootEvent);
    gBS->CloseEvent (MePlatformReadyToBootEvent);
  }

  HeciGetMeMode (&MeMode);
  HeciGetMeStatus (&MeStatus);

  ///
  /// Send EOP if ME is at normal mode and ME is ready
  ///
  if ((MeMode == ME_MODE_NORMAL) &&
      (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_READY)) {
    UINT32 RequestedActions = HECI_EOP_STATUS_SUCCESS;

    CheckFwUpdInProgress ();
    MeWarningMessage ();

    if (MeEndOfPostEnabled ()) {
      DEBUG ((DEBUG_INFO, "[ME] Sending END_OF_POST to ME\n"));
      Status = MeEndOfPostEvent ( &RequestedActions );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[Me] MeEndOfPostEvent failed with Status = %r\n", Status));
        MeReportError (MSG_EOP_ERROR);
      } else if (RequestedActions == HECI_EOP_PERFORM_GLOBAL_RESET) {
        PCH_RESET_PROTOCOL  *PchResetProtocol;
        WDT_PROTOCOL        *WdtProtocol;
        VOID                *ResetData;
        UINTN               DataSize;

        Status = gBS->LocateProtocol (&gPchResetProtocolGuid, NULL, (VOID **) &PchResetProtocol);
        if (!EFI_ERROR (Status)) {
          Status = gBS->LocateProtocol (&gWdtProtocolGuid, NULL, (VOID **) &WdtProtocol);
          if (!EFI_ERROR (Status)) {
            WdtProtocol->AllowKnownReset ();
          }
          DEBUG ((DEBUG_INFO, "Global Reset requested by FW EOP ACK %r\n"));
          ResetData = PchResetProtocol->GetResetData (&gPchGlobalResetGuid, &DataSize);
          PchResetProtocol->Reset (PchResetProtocol, EfiResetPlatformSpecific, DataSize, ResetData);
        } else {
          DEBUG ((
            DEBUG_ERROR,
            "Global Reset requested by FW EOP ACK but can't find PchReset protocol %r\n"
            ));
        }
      } 
    }
  } else {
    DEBUG ((DEBUG_ERROR, "[Me] EndOfPost not sent due wrong MeMode(0x%x) or MeStatus(0x%x)\n",
            MeMode,
            MeStatus));
  }

  ///
  /// PMIR Configuration & FDSW Lockdown
  ///
  LockConfig ();

  gBS->CloseEvent (Event);

  return EFI_SUCCESS;
}

/**
  @brief
  Signal a event to save Me relevant registers and this event 
  must be run before EndOfDxe. 

  @param[in] Event             - The event that triggered this notification function  
  @param[in] ParentImageHandle - Pointer to the notification functions context

  @retval EFI_SUCCESS             Always return EDI_SUCCESS

**/
EFI_STATUS
MeScriptSaveEventFunction (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *ParentImageHandle
  )
{
  EFI_STATUS              Status;
  HECI_PROTOCOL           *Heci;
  UINT32                  MeMode;
  HECI_FWS_REGISTER       MeFirmwareStatus;
  UINTN                   PmcBaseAddress;
  UINT32                  Data;
  const UINT8             StrCorporateFw[sizeof (STR_ME_FW_SKU_CORPORATE_FW)]     = STR_ME_FW_SKU_CORPORATE_FW;
  MEBX_PROTOCOL           *MebxProtocol;
  ME_BIOS_PAYLOAD_HOB     *MbpHob;
  SPS_POLICY_PROTOCOL     *SpsPolicy;

  Heci = NULL;

  //
  // Get the MBP Data.
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob == NULL) {
    DEBUG ((DEBUG_ERROR, "MeScriptSaveEventFunction: No MBP Data Protocol available\n"));
    return EFI_UNSUPPORTED;
  }

  ///
  /// PMIR Configuration Save
  ///
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Check ME Status
  ///
  Status = Heci->GetMeMode (&MeMode);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Report ME components version information to FVI
  ///
  SpsPolicy = GetSpsPolicy();
  if (SpsPolicy != NULL)
  {
    mMeFviHeader.SmbiosHeader.Type = SpsPolicy->MeAmtConfig.FviSmbiosType;
  }

  mMeFviElementsData[EnumMeFw].Version.MajorVersion = (UINT8) MbpHob->MeBiosPayload.FwVersionName.MajorVersion;
  mMeFviElementsData[EnumMeFw].Version.MinorVersion = (UINT8) MbpHob->MeBiosPayload.FwVersionName.MinorVersion;
  mMeFviElementsData[EnumMeFw].Version.Revision = (UINT8) MbpHob->MeBiosPayload.FwVersionName.HotfixVersion;
  mMeFviElementsData[EnumMeFw].Version.BuildNum = (UINT16) MbpHob->MeBiosPayload.FwVersionName.BuildVersion;
  if (MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType == INTEL_ME_CORPORATE_FW) {
    mMeFviStrings[EnumMeFw].VersionString = (CHAR8 *) &StrCorporateFw;
  }
  Status = gBS->LocateProtocol (&gMebxProtocolGuid, NULL, (VOID **) &MebxProtocol);
  if (!EFI_ERROR (Status)) {
    mMeFviElementsData[EnumMebx].Version.MajorVersion = (UINT8) MebxProtocol->MebxVersion.Major;
    mMeFviElementsData[EnumMebx].Version.MinorVersion = (UINT8) MebxProtocol->MebxVersion.Minor;
    mMeFviElementsData[EnumMebx].Version.Revision = (UINT8) MebxProtocol->MebxVersion.Hotfix;
    mMeFviElementsData[EnumMebx].Version.BuildNum = (UINT16) MebxProtocol->MebxVersion.Build;
  }

  Status = AddFviEntry (mMeFviHeader, mMeFviElementsData, mMeFviStrings);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AddFviEntry failed. Status = %r\n", Status));
  }

  ///
  /// Get PMC Base Address
  ///
  PmcBaseAddress = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC);

  ///
  /// PCH BIOS Spec Rev 0.9 Section 18.4  Additional Power Management Programming
  /// Step 2
  ///   Set "Power Management Initialization Register (ETR3) Field 1", D31:F2:ACh[31] = 1b
  ///   for production machine according to "RS - PCH Intel Management Engine
  ///  (Intel(r) ME) BIOS Writer's Guide".
  ///
  /// PCH ME BWG section 4.5.1
  /// The IntelR FPT tool /GRST option uses CF9GR bit to trigger global reset.
  /// Based on above reason, the BIOS should not lock down CF9GR bit during Manufacturing and
  /// Re-manufacturing environment.
  ///
  Data = MmioRead32 (PmcBaseAddress + R_PCH_PMC_ETR3);
  Data &= (UINT32) (~(B_PCH_PMC_ETR3_CF9LOCK | B_PCH_PMC_ETR3_CF9GR));

  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);

  if ((((MeMode == ME_MODE_NORMAL) || (MeMode == ME_MODE_TEMP_DISABLED)) && !(MeFirmwareStatus.r.ManufacturingMode))) {
    ///
    /// PCH ME BWG section 4.4.1
    /// BIOS must also ensure that CF9GR is cleared and locked (via bit31 of the same register) before
    /// handing control to the OS in order to prevent the host from issuing global resets and reseting
    /// Intel Management Engine.
    ///
    Data |= (UINT32) (B_PCH_PMC_ETR3_CF9LOCK);
  }

  ///
  /// ETR3 is a resume well register and has no script save for it.
  /// System may go through S3 resume path from G3 if RapidStart is enabled,
  /// that means all resume well registers will be reset to defaults.
  /// Save boot script for ETR3 register if RapidStart is enabled.
  ///
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) MmPciBase (
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_PMC,
            PCI_FUNCTION_NUMBER_PCH_PMC) +
            R_PCH_PMC_ETR3,
    1,
    &Data
  );
    
  //
  //Sending notification that BIOS is starting to run 3rd party code.
  //
  if (CoreBiosDoneEnabled ()) {
    HeciCoreBiosDoneMsg();
  }

  gBS->CloseEvent (Event);

  return EFI_SUCCESS;
}

#endif // AMT_SUPPORT
