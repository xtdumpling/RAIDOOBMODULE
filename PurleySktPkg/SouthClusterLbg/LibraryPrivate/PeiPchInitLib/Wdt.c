/** @file
  Implementation file for Watchdog Timer functionality

@copyright
 Copyright (c) 2010 - 2015 Intel Corporation. All rights reserved
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
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Library/PeiServicesLib.h>
#include <PchAccess.h>
#include <Library/PchCycleDecodingLib.h>
#include <Ppi/PchReset.h>
#include <Ppi/Wdt.h>
#include <IncludePrivate/Library/PchWdtCommonLib.h>

EFI_STATUS
EFIAPI
WdtPchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  );

static WDT_PPI                    mWdtPpi = {
  WdtReloadAndStart,
  WdtCheckStatus,
  WdtDisable,
  WdtAllowKnownReset,
  IsWdtRequired,
  IsWdtEnabled
};

static PCH_RESET_CALLBACK_PPI     mPchResetCallbackPpi = { WdtPchResetCallback };

static EFI_PEI_PPI_DESCRIPTOR     mInstallWdtPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gWdtPpiGuid,
  &mWdtPpi
};

static EFI_PEI_PPI_DESCRIPTOR     mInstallPchResetCallbackPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPchResetCallbackPpiGuid,
  &mPchResetCallbackPpi
};

#define MINIMUM_TIMEOUT_AT_S3_EXIT  10  ///< seconds

/**
  Reads PCH registers to check if platform wakes from S3/S4


  @retval TRUE                    if platfrom wakes from S3/S4
  @retval FALSE                   otherwise
**/
UINT8
IsWakeFromS3_S4 (
  VOID
  )
{
  UINT16  ABase;
  UINT16  SleepType;

  PchAcpiBaseGet (&ABase);

  if (IoRead16 (ABase + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_WAK) {
    SleepType = IoRead16 (ABase + R_PCH_ACPI_PM1_CNT) & B_PCH_ACPI_PM1_CNT_SLP_TYP;
    if ((SleepType == V_PCH_ACPI_PM1_CNT_S3) || (SleepType == V_PCH_ACPI_PM1_CNT_S4)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  This runtine clears status bits and disable watchdog, then lock the
  WDT registers.
  while WDT is designed to be disabled and locked by policy,
  bios should not enable WDT by WDT PPI. In such case, bios shows the
  warning message but not disable and lock WDT register to make sure
  WDT event trigger correctly.

  @param[in] DisableAndLock             Policy to disable and lock WDT register

  @retval    NONE
**/
VOID
WdtOnPolicyInstalled (
  BOOLEAN                               DisableAndLock
  )
{
  if (!DisableAndLock) {
    return;
  }

  if (!IsWdtEnabled ()) {
    //
    // clear status bits and disable watchdog, then lock the register
    //
    IoWrite32 (WdtGetAddress (), (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS));
    IoWrite32 (WdtGetAddress (), B_PCH_OC_WDT_CTL_LCK);
  } else {
    //
    // while WDT is designed to be disabled and locked by policy,
    // bios should not enable WDT by WDT PPI. In such case, bios shows the
    // warning message but not disable and lock WDT register to make sure
    // WDT event trigger correctly.
    //
    DEBUG ((DEBUG_ERROR, "(WDT) Usage mismatched with policy\n"));
  }
  return;
}

/**
  Initializes watchdog failure bits.
  If there was an unexpected reset, enforces WDT expiration.
  Stores initial WDT state in a HOB, it is useful in flows with S3/S4 resume.
  Stops watchdog.
  Installs watchdog PPI for other modules to use.

  @retval EFI_SUCCESS             When everything is OK
**/
EFI_STATUS
EFIAPI
WdtPeiEntryPoint (
  VOID
  )
{
  UINT32      Readback;
  EFI_STATUS  Status;
  UINT16      TimeoutValue;
  UINT8       Active;
  WDT_HOB     *WdtHobPtr;

  Readback = IoRead32 (WdtGetAddress ());

  DEBUG ((DEBUG_INFO, "(WDT) Readback = 0x%08x\n", Readback));
  ///
  /// Write current Wdt settings to a HOB, they may be be needed in S3/S4 resume paths
  ///
  if (Readback & B_PCH_OC_WDT_CTL_EN) {
    Active        = 1;
    TimeoutValue  = (UINT16) ((Readback & B_PCH_OC_WDT_CTL_TOV_MASK) + 1);
  } else {
    Active        = 0;
    TimeoutValue  = 0;
  }

  Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, sizeof (WDT_HOB), (VOID **) &WdtHobPtr);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  WdtHobPtr->Header.Name  = gWdtHobGuid;
  WdtHobPtr->Active       = Active;
  WdtHobPtr->TimeoutValue = TimeoutValue;
  ///
  /// If there was a WDT expiration, set Failure Status and clear timeout status bits
  /// Timeout status bits are cleared by writing '1'
  ///
  if (Readback & (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS)) {
    DEBUG ((DEBUG_ERROR, "(WDT) Expiration detected.\n", Readback));
    Readback |= B_PCH_OC_WDT_CTL_FAILURE_STS;
    Readback |= (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS);
    Readback &= ~(B_PCH_OC_WDT_CTL_UNXP_RESET_STS);
  } else {
    ///
    /// If there was unexpected reset but no WDT expiration and no resume from S3/S4,
    /// clear unexpected reset status and enforce expiration. This is to inform Firmware
    /// which has no access to unexpected reset status bit, that something went wrong.
    ///
    if ((Readback & B_PCH_OC_WDT_CTL_UNXP_RESET_STS) && !IsWakeFromS3_S4 ()) {
#ifndef MDEPKG_NDEBUG
      DEBUG ((DEBUG_ERROR, "(WDT) Unexpected reset detected and ignored.\n"));
      Readback &= ~(B_PCH_OC_WDT_CTL_FAILURE_STS | B_PCH_OC_WDT_CTL_UNXP_RESET_STS);
      Readback |= (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS);
#else
      DEBUG ((DEBUG_ERROR, "(WDT) Unexpected reset detected. Enforcing Wdt expiration.\n"));
      WdtReloadAndStart (1);
      ///
      /// wait for reboot caused by WDT expiration
      ///
      CpuDeadLoop ();
#endif
    } else {
      ///
      /// No WDT expiration and no unexpected reset - clear Failure status
      ///
      DEBUG ((DEBUG_INFO, "(WDT) Status OK.\n", Readback));
      Readback &= ~(B_PCH_OC_WDT_CTL_FAILURE_STS);
      Readback |= (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS);
    }
  }

  IoWrite32 (WdtGetAddress (), Readback);

  Status = PeiServicesInstallPpi (&mInstallWdtPpi);

  Status = PeiServicesInstallPpi (&mInstallPchResetCallbackPpi);

  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Support for WDT in S3 resume.
  If WDT was enabled during S0->S3 transition, this function will turn on WDT
  just before waking OS. Timeout value will be overridden if it was too small.

  @retval EFI_SUCCESS             When everything is OK
  @retval EFI_NOT_FOUND           WdtHob is not found
**/
EFI_STATUS
WdtEndOfPeiCallback (
  VOID
  )
{
  WDT_HOB       *WdtHob;
  EFI_STATUS    Status;
  EFI_BOOT_MODE BootMode;

  DEBUG ((DEBUG_INFO, "(WDT) EndOfPeiCallback\n"));

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  WdtHob = GetFirstGuidHob (&gWdtHobGuid);
  if (WdtHob == NULL) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "(WDT) BootMode %d, Hob, active %d, ToV %d\n", BootMode, WdtHob->Active, WdtHob->TimeoutValue));

  if (BootMode == BOOT_ON_S3_RESUME) {
    if (WdtHob->Active == 1) {
      if (WdtHob->TimeoutValue < MINIMUM_TIMEOUT_AT_S3_EXIT) {
        WdtReloadAndStart (MINIMUM_TIMEOUT_AT_S3_EXIT);
      } else {
        WdtReloadAndStart (WdtHob->TimeoutValue);
      }
    } else {
      WdtDisable ();
    }
  }

  return EFI_SUCCESS;
}

/**
  WDT call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
WdtPchResetCallback (
  IN PCH_RESET_TYPE PchResetType
  )
{
  WdtAllowKnownReset ();
  return EFI_SUCCESS;
}
