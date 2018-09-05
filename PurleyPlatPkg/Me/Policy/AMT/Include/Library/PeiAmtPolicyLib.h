/** @file
  Prototype of the AmtPolicyLibPei library.

@copyright
 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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
#ifndef _PEI_AMT_POLICY_LIB_H_
#define _PEI_AMT_POLICY_LIB_H_

#include <Ppi/AmtPolicyPpi.h>

/**
  Print whole AMT_POLICY_PPI and serial out.

  @param[in] AmtPolicyPpi               The RC Policy PPI instance
**/
VOID
EFIAPI
AmtPrintPolicyPpi (
  IN  AMT_POLICY_PPI           *AmtPolicyPpi
  );

/**
  AmtCreatePolicyDefaults creates the default setting of AMT policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] AmtPolicyPpi              The pointer to get AMT Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
AmtCreatePolicyDefaults (
  OUT  AMT_POLICY_PPI          **AmtPolicyPpi
  );

/**
  AmtInstallPolicyPpi installs AmtPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] AmtPolicyPpi               The pointer to AMT Policy PPI instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
AmtInstallPolicyPpi (
  IN  AMT_POLICY_PPI           *AmtPolicyPpi
  );

#endif // _PEI_AMT_POLICY_LIB_H_
