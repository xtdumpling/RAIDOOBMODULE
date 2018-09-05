/** @file
  This file contains the tests for the SecureSystemAgentConfiguration bit

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
//  SystemAgentResults Array Contents
//      - TRUE:  Passed
//      - FALSE: Failed
//    Index 00 - ConfigLock Check
//
#define   SA_MAX_TESTS        1
BOOLEAN   SystemAgentResults[SA_MAX_TESTS];

/**
  Multithreaded function to collect test results on each thread.
**/
VOID
CollectSystemAgentTestResults(
  VOID
)
{
  UINT64      ConfigLock;

  //
  // ConfigLock Check
  //
  ConfigLock = AsmReadMsr64 (EFI_MSR_LT_CONTROL);
  if ((ConfigLock & BIT0) == 0) {
    SystemAgentResults[0] = FALSE;
  }

}


/**
  Run tests for SecureSystemAgentConfiguration bit
**/
VOID
CheckSecureSystemAgentConfiguration (
  VOID
  )
{
  EFI_STATUS      Status;
  BOOLEAN         Result;
  CHAR16          *HstiErrorString;
  UINT8           Index = 0;

  if ((mFeatureImplemented[0] & HSTI_BYTE0_SECURE_SYSTEM_AGENT_CONFIGURATION) == 0) {
    return;
  }

  //
  // Intialize Results Array
  //
  for(Index = 0; Index < SA_MAX_TESTS; Index++) {
    SystemAgentResults[Index] = TRUE;
  }

  CollectSystemAgentTestResults();
  mMpService->StartupAllAPs (
                mMpService,
                (EFI_AP_PROCEDURE)CollectSystemAgentTestResults,
                FALSE,
                NULL,
                0,
                NULL,
                NULL
                );

  Result = TRUE;
  Index = 0;

  DEBUG ((DEBUG_INFO, "  Configuration Locking\n"));

  DEBUG ((DEBUG_INFO, "    1. ConfigLock\n"));

  if (SystemAgentResults[Index++] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: ConfigLock not set\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_SYSTEM_AGENT_CONFIGURATION_ERROR_CODE_1 ,HSTI_SYSTEM_AGENT_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_SYSTEM_AGENT_CONFIGURATION_ERROR_STRING_1);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               0,
               HSTI_BYTE0_SECURE_SYSTEM_AGENT_CONFIGURATION
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
