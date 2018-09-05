/** @file
  Prototype of the PeiSiPolicyLib library.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#ifndef _PEI_SI_POLICY_LIB_H_
#define _PEI_SI_POLICY_LIB_H_

#include <Ppi/SiPolicyPei.h>

/**
  SiCreatePolicyDefaults creates the default setting of Silicon Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] SiPolicyPpi         The pointer to get Silicon Policy PPI instance

  @retval EFI_SUCCESS             The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiCreatePolicyDefaults (
  OUT  SI_POLICY_PPI **SiPolicyPpi
  );

/**
  SiInstallPolicyPpi installs SiPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiPolicyPpi         The pointer to Silicon Policy PPI instance

  @retval EFI_SUCCESS            The policy is installed.
  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiInstallPolicyPpi (
  IN  SI_POLICY_PPI *SiPolicyPpi
  );

#endif // _PEI_SI_POLICY_LIB_H_
