/** @file
  This file contains the tests for the DebugModeDisabled Bit

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

/**
  Run tests for DebugModeDisabled bit
**/
VOID
CheckDebugModeDisabled (
  VOID
  )
{
  EFI_STATUS      Status;
  BOOLEAN         Result;
  UINT32          RegEcx;
  UINT64          DebugInterface;
  CHAR16          *HstiErrorString;

  if ((mFeatureImplemented[0] & HSTI_BYTE0_DEBUG_MODE_DISABLED_VERIFICATION) == 0) {
    return;
  }

  Result = TRUE;

  DEBUG ((DEBUG_INFO, "  Debug Interface check\n"));

  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, &RegEcx, NULL);

  if ((RegEcx & BIT11) != 0) {

    DebugInterface =  AsmReadMsr64 (MSR_IA32_DEBUG_INTERFACE);

    DEBUG ((DEBUG_INFO, "    1. Disabled Check\n"));

    if ((DebugInterface & B_ENABLE_DEBUG_FEATURES) != 0) {
      DEBUG ((DEBUG_INFO, "Fail: Debug Interface Enabled\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_DEBUG_MODE_DISABLED_VERIFICATION_ERROR_CODE_1 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_DEBUG_MODE_DISABLED_VERIFICATION_ERROR_STRING_1);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    2. Locked\n"));

    if ((DebugInterface & B_DEBUG_INTERFACE_LOCK) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: Debug interface not locked\n"));


      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_DEBUG_MODE_DISABLED_VERIFICATION_ERROR_CODE_2 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_DEBUG_MODE_DISABLED_VERIFICATION_ERROR_STRING_2);
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
               HSTI_BYTE0_DEBUG_MODE_DISABLED_VERIFICATION
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
