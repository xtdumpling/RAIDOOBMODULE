/** @file
  Header file for PEI SiPolicyUpdate Library.

@copyright
 Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _PEI_ME_POLICY_UPDATE_LIB_H_
#define _PEI_ME_POLICY_UPDATE_LIB_H_

#include <Ppi/MePolicyPpi.h>
#include <Ppi/AmtPolicyPpi.h>

/**
  Update the ME Policy Library

  @param[in, out] PeiMePolicyPpi The pointer to MePolicyPpi

  @retval EFI_SUCCESS            Update complete.
**/
EFI_STATUS
UpdatePeiMePolicy (
  IN OUT   ME_POLICY_PPI      *PeiMePolicyPpi
  );

/**
  Install the Active Management Policy Ppi Library

  @param[in, out] AmtPolicyPpi PEI Amt Policy

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
UpdatePeiAmtPolicy (
  IN OUT  AMT_POLICY_PPI *AmtPolicyPpi
  );

#endif
