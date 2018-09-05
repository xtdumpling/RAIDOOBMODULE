/** @file
  This file contains the tests for the HardwareRootedBootIntegrity bit

@copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/

#include "HstiIhvProviderDxe.h"

extern EFI_MP_SERVICES_PROTOCOL *mMpService;

//
//  BootIntegrityResults Array Contents
//      - TRUE:  Passed
//      - FALSE: Failed
//    Index 00 - Boot Guard Capability Check
//    Index 01 - Boot Guard Verified Boot Check
//
#define HRBI_MAX_TESTS       2
BOOLEAN   BootIntegrityResults[HRBI_MAX_TESTS];

/**
  Multithreaded function to collect test results on each thread.
**/
VOID
CollectBootIntegrityTestResults(
  VOID
)
{
  UINT64      SAcmInfo;

  //
  // Boot Guard Capability Check
  //
  SAcmInfo = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
  if ((SAcmInfo & B_BOOT_GUARD_SACM_INFO_CAPABILITY) == 0) {
    BootIntegrityResults[0] = FALSE;
  }

  //
  // Boot Guard Verified Boot Check
  //
  if ((SAcmInfo & B_BOOT_GUARD_SACM_INFO_CAPABILITY) != 0) {
    if ((SAcmInfo & B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT) == 0) {
      BootIntegrityResults[1] = FALSE;
    }
  }
}

/**
  Run tests for HardwareRootedBootIntegrity bit
**/
VOID
CheckHardwareRootedBootIntegrity (
  VOID
  )
{
  EFI_STATUS    Status;
  BOOLEAN       Result;
  CHAR16        *HstiErrorString;
  UINT8         Index = 0;

  HstiErrorString = NULL;

  if ((mFeatureImplemented[0] & HSTI_BYTE0_HARDWARE_ROOTED_BOOT_INTEGRITY) == 0) {
    return;
  }

  //
  // Intialize Results Array
  //
  for(Index = 0; Index < HRBI_MAX_TESTS; Index++) {
    BootIntegrityResults[Index] = TRUE;
  }

  CollectBootIntegrityTestResults();
  mMpService->StartupAllAPs (
                mMpService,
                (EFI_AP_PROCEDURE)CollectBootIntegrityTestResults,
                FALSE,
                NULL,
                0,
                NULL,
                NULL
                );

  Result = TRUE;
  Index = 0;

  DEBUG ((DEBUG_INFO, "  Boot Guard check\n"));

  DEBUG ((DEBUG_INFO, "    1. SACM INFO - Boot Guard Capability\n"));

  if (BootIntegrityResults[Index++] == FALSE) {
    DEBUG ((DEBUG_INFO, "Boot Guard Capability Failed: Bootguard Disabled\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_HARDWARE_ROOTED_BOOT_INTEGRITY_ERROR_CODE_2, HSTI_BOOTGUARD_CONFIGURATION, HSTI_BYTE0_HARDWARE_ROOTED_BOOT_INTEGRITY_ERROR_STRING_2);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  if (BootIntegrityResults[0] == TRUE) {
    DEBUG ((DEBUG_INFO, "    2. SACM INFO - Boot Guard Verified Boot\n"));
    if (BootIntegrityResults[Index++] == FALSE) {
      DEBUG ((DEBUG_INFO, "Boot Guard Capability Failed: Verified boot not enabled\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_HARDWARE_ROOTED_BOOT_INTEGRITY_ERROR_CODE_1, HSTI_BOOTGUARD_CONFIGURATION, HSTI_BYTE0_HARDWARE_ROOTED_BOOT_INTEGRITY_ERROR_STRING_1);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }
  }

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               0,
               HSTI_BYTE0_HARDWARE_ROOTED_BOOT_INTEGRITY
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
