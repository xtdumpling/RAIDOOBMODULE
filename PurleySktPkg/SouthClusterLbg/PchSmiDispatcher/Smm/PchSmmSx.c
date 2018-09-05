/** @file
  File to contain all the hardware specific stuff for the Smm Sx dispatch protocol.

@copyright
 Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
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
#include "PchSmmHelpers.h"
#include <Library/PchOemSmmLib.h>

#define PROGRESS_CODE_S3_SUSPEND_END  PcdGet32 (PcdProgressCodeS3SuspendEnd)

GLOBAL_REMOVE_IF_UNREFERENCED CONST PCH_SMM_SOURCE_DESC SX_SOURCE_DESC = {
  PCH_SMM_NO_FLAGS,
  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_PCH_SMI_EN}
      },
      S_PCH_SMI_EN,
      N_PCH_SMI_EN_ON_SLP_EN
    },
    NULL_BIT_DESC_INITIALIZER
  },
  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_PCH_SMI_STS}
      },
      S_PCH_SMI_STS,
      N_PCH_SMI_STS_ON_SLP_EN
    }
  },
  {
    {
      ACPI_ADDR_TYPE,
      {R_PCH_SMI_STS}
    },
    S_PCH_SMI_STS,
    N_PCH_SMI_STS_ON_SLP_EN
  }
};

/**
  Get the Sleep type

  @param[in] Record               No use
  @param[out] Context             The context that includes SLP_TYP bits to be filled

**/
VOID
EFIAPI
SxGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *Context
  )
{
  UINT32  Pm1Cnt;

  Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));

  ///
  /// By design, the context phase will always be ENTRY
  ///
  Context->Sx.Phase = SxEntry;

  ///
  /// Map the PM1_CNT register's SLP_TYP bits to the context type
  ///
  switch (Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) {
  case V_PCH_ACPI_PM1_CNT_S0:
    Context->Sx.Type = SxS0;
    break;

  case V_PCH_ACPI_PM1_CNT_S1:
    Context->Sx.Type = SxS1;
    break;

  case V_PCH_ACPI_PM1_CNT_S3:
    Context->Sx.Type = SxS3;
    break;

  case V_PCH_ACPI_PM1_CNT_S4:
    Context->Sx.Type = SxS4;
    break;

  case V_PCH_ACPI_PM1_CNT_S5:
    Context->Sx.Type = SxS5;
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}

/**
  Check whether sleep type of two contexts match

  @param[in] Context1             Context 1 that includes sleep type 1
  @param[in] Context2             Context 2 that includes sleep type 2

  @retval FALSE                   Sleep types match
  @retval TRUE                    Sleep types don't match
**/
BOOLEAN
EFIAPI
SxCmpContext (
  IN PCH_SMM_CONTEXT     *Context1,
  IN PCH_SMM_CONTEXT     *Context2
  )
{
  return (BOOLEAN) (Context1->Sx.Type == Context2->Sx.Type);
}

/**
  When we get an SMI that indicates that we are transitioning to a sleep state,
  we need to actually transition to that state.  We do this by disabling the
  "SMI on sleep enable" feature, which generates an SMI when the operating system
  tries to put the system to sleep, and then physically putting the system to sleep.


**/
VOID
PchSmmSxGoToSleep (
  VOID
  )
{
  UINT32      Pm1Cnt;
  UINT32      PchPwrmBase;

  PchPwrmBaseGet (&PchPwrmBase);

  ///
  /// Flush cache into memory before we go to sleep. It is necessary for S3 sleep
  /// because we may update memory in SMM Sx sleep handlers -- the updates are in cache now
  ///
  AsmWbinvd ();

  ///
  /// Disable SMIs
  ///
  PchSmmClearSource (&SX_SOURCE_DESC);
  PchSmmDisableSource (&SX_SOURCE_DESC);

  ///
  /// Get Power Management 1 Control Register Value
  ///
  Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));

  ///
  /// Record S3 suspend performance data
  ///
  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S3) {
    ///
    /// Report status code before goto S3 sleep
    ///
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PROGRESS_CODE_S3_SUSPEND_END);

    ///
    /// Flush cache into memory before we go to sleep.
    ///
    AsmWbinvd ();
  }

  OemSmmSxPrepareToSleep();

  ///
  /// Now that SMIs are disabled, write to the SLP_EN bit again to trigger the sleep
  ///
  Pm1Cnt |= B_PCH_ACPI_PM1_CNT_SLP_EN;

  IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT), Pm1Cnt);

  ///
  /// Should only proceed if wake event is generated.
  ///
  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S1) {
    while (((IoRead16 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_STS))) & B_PCH_ACPI_PM1_STS_WAK) == 0x0);
  } else {
    CpuDeadLoop ();
  }
  ///
  /// The system just went to sleep. If the sleep state was S1, then code execution will resume
  /// here when the system wakes up.
  ///
  Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));

  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SCI_EN) == 0) {
    ///
    /// An ACPI OS isn't present, clear the sleep information
    ///
    Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SLP_TYP;
    Pm1Cnt |= V_PCH_ACPI_PM1_CNT_S0;

    IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT), Pm1Cnt);
  }

  PchSmmClearSource (&SX_SOURCE_DESC);
  PchSmmEnableSource (&SX_SOURCE_DESC);
}
