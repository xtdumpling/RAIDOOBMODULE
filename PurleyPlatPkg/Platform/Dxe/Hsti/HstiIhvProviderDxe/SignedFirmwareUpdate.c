/** @file
  This file contains the tests for the SignedFirmwareUpdate bit

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
//  SignedFirmwareResults Array Contents
//      - TRUE:  Passed
//      - FALSE: Failed
//    Index 00 - BIOS Guard support Check
//    Index 01 - BIOS Guard Lock Check
//    Index 02 - BIOS Guard Enable
//
#define SF_MAX_TESTS          3
BOOLEAN   SignedFirmwareResults[SF_MAX_TESTS];

/**
  Multithreaded function to collect test results on each thread.
**/
VOID
CollectSignedFirmwareTestResults(
  VOID
)
{
  UINT64      PlatformInfo;
  UINT64      PlatFrmwProtCtrl;

  //
  // BIOS Guard support Check
  //
  PlatformInfo = AsmReadMsr64 (EFI_PLATFORM_INFORMATION);
  if ((PlatformInfo & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) == 0) {
    SignedFirmwareResults[0] = FALSE;
  }

  //
  // BIOS Guard Lock Check
  //
  if (SignedFirmwareResults[0] == TRUE) {
    PlatFrmwProtCtrl = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL);
    if ((PlatFrmwProtCtrl & B_MSR_PLAT_FRMW_PROT_CTRL_LK) == 0) {
      SignedFirmwareResults[1] = FALSE;
    }

    //
    // BIOS Guard Enable
    //
    if ((PlatFrmwProtCtrl & B_MSR_PLAT_FRMW_PROT_CTRL_EN) == 0) {
      SignedFirmwareResults[2] = FALSE;
    }
  }

}

/**
  Run tests for SignedFirmwareUpdate bit
**/
VOID
CheckSignedFirmwareUpdate (
  VOID
  )
{
  EFI_STATUS      Status;
  BOOLEAN         BiGResult;
  BOOLEAN         SpiWpResult;
  UINT8           Index = 0;
  UINT8           BiosControl;
  CHAR16          *HstiErrorString;

  if ((mFeatureImplemented[0] & HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE) == 0) {
    return;
  }

  //
  // Intialize Results Array
  //
  for(Index = 0; Index < SF_MAX_TESTS; Index++) {
    SignedFirmwareResults[Index] = TRUE;
  }

  CollectSignedFirmwareTestResults();
  mMpService->StartupAllAPs (
                mMpService,
                (EFI_AP_PROCEDURE)CollectSignedFirmwareTestResults,
                FALSE,
                NULL,
                0,
                NULL,
                NULL
                );

  BiGResult = TRUE;
  SpiWpResult = TRUE;

  DEBUG ((DEBUG_INFO, "Signed Firmware Update Verification\n"));

  DEBUG ((DEBUG_INFO, "  BIOS Guard Security Configuration\n"));

  DEBUG ((DEBUG_INFO, "    1. BIOS Guard support\n"));

  if (SignedFirmwareResults[0] == FALSE) {
    DEBUG ((DEBUG_INFO, "BIOS Guard support failed: BIOS Guard not supported\n"));
    BiGResult = FALSE;
  }

  if (SignedFirmwareResults[0] == TRUE) {
    DEBUG ((DEBUG_INFO, "    2. BIOS Guard Lock\n"));

    if (SignedFirmwareResults[1] == FALSE) {
      DEBUG ((DEBUG_INFO, "BIOS Guard support failed: BIOS Guard lock not set\n"));
      BiGResult = FALSE;
    }

    DEBUG ((DEBUG_INFO, "    3. BIOS Guard Enable\n"));

    if (SignedFirmwareResults[2] == FALSE) {
      DEBUG ((DEBUG_INFO, "BIOS Guard support failed: BIOS Guard not enabled\n"));
      BiGResult = FALSE;
    }
  }

  DEBUG ((DEBUG_INFO, "  BIOS write-protection\n"));

  BiosControl = MmioRead8 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_LPC,PCI_FUNCTION_NUMBER_PCH_SPI) + R_PCH_SPI_BC);
  if (((BiosControl & B_PCH_SPI_BC_LE) == 0) ||
      ((BiosControl & B_PCH_SPI_BC_WPD) != 0) ||
      ((BiosControl & B_PCH_SPI_BC_EISS) == 0)) {

    DEBUG ((DEBUG_INFO, "FAIL:  BIOS write-protection not set\n"));
    SpiWpResult = FALSE;
  }

  //
  // If both Signed Firmware update protections failed, report the failure strings
  //
  if (!(BiGResult || SpiWpResult)) {
    //
    // BIOS Guard not supported
    //
    if (SignedFirmwareResults[0] == FALSE) {
      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE_ERROR_CODE_1 ,HSTI_BIOS_GUARD_SECURITY_CONFIGURATION, HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE_ERROR_STRING_1);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      FreePool (HstiErrorString);
    } else {
      //
      // BIOS Guard not locked
      //
      if (SignedFirmwareResults[1] == FALSE) {
        HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE_ERROR_CODE_2 ,HSTI_BIOS_GUARD_SECURITY_CONFIGURATION, HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE_ERROR_STRING_2);
        Status = HstiLibAppendErrorString (
                   PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                   NULL,
                   HstiErrorString
                   );
        ASSERT_EFI_ERROR (Status);
        FreePool (HstiErrorString);
      }
      //
      // BIOS Guard not enabled
      //
      if (SignedFirmwareResults[2] == FALSE) {
        HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE_ERROR_CODE_3 ,HSTI_BIOS_GUARD_SECURITY_CONFIGURATION, HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE_ERROR_STRING_3);
        Status = HstiLibAppendErrorString (
                   PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                   NULL,
                   HstiErrorString
                   );
        ASSERT_EFI_ERROR (Status);
        FreePool (HstiErrorString);
      }
    }

    //
    // Bios Write Protection failed
    //
    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE_ERROR_CODE_4 ,HSTI_BIOS_GUARD_SECURITY_CONFIGURATION, HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_STRING_4);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    FreePool (HstiErrorString);
  }

  //
  // For Server, either BIOS Guard or BIOS Write Protection are acceptable flash protection
  //
  if (BiGResult || SpiWpResult) {
    DEBUG ((DEBUG_INFO, "Signed Firmware Update Verification Passed\n"));
    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               0,
               HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
