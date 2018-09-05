/** @file
  This file contains sample test for setting Continuous Crypto strength 
  bit in sample IBV HSTI structure. Actual test implementation is not present
  in this sample driver.

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification
**/

#include "HstiIbvPlatformDxe.h"

/**
  This function sets the verified bit for FirmwareTrustContinuationCryptoStrength.
  Actual test implementation is not present in this sample function.
**/
VOID
CheckFirmwareTrustContinuationCryptoStrength (
  IN UINT32                   Role
  )
{
  EFI_STATUS      Status;
  BOOLEAN         Result;

  if ((mFeatureImplemented[2] & HSTI_BYTE2_FIRMWARE_TRUSTED_CONTINUATION_CRYPTO_STRENGTH) == 0) {
    return ;
  }

  Result = TRUE;
  DEBUG ((EFI_D_INFO, "  Firmware Trust Continuation Crypto Strength\n"));

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               Role,
               NULL,
               2,
               HSTI_BYTE2_FIRMWARE_TRUSTED_CONTINUATION_CRYPTO_STRENGTH
               );
    ASSERT_EFI_ERROR (Status);
  }

  return ;
}
