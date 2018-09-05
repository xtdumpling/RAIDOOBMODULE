/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2006 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MeSpsPolicyLibPei.c

@brief:

  Implementation file for ME Policy functionality for PEIM

**/
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include "Ppi/MeSpsPolicyPei.h"
#include "Library/MeSpsPolicyAccessPeiLib.h"

/**
  @brief
  Get SPS_POLICY_PPI

  @param[in] MeSpsPolicy The ME SPS Policy protocol instance

  @retval SPS_POLICY_PPI*       Pointer to ME SPS Policy PPI
          NULL                         is returned on error

-**/

SPS_POLICY_PPI*
GetSpsPolicyPpi (VOID)
{
  EFI_STATUS       Status;
  SPS_POLICY_PPI  *SpsPolicyPpi;

  ///
  /// Locate system configuration variable
  ///
  Status = PeiServicesLocatePpi (
            &gSpsPolicyPpiGuid,        /// GUID
            0,                           /// INSTANCE
            NULL,                        /// EFI_PEI_PPI_DESCRIPTOR
            (VOID **)&SpsPolicyPpi       /// PPI
            );
  if (EFI_ERROR(Status)) {
    SpsPolicyPpi = NULL;
    ASSERT_EFI_ERROR (Status);
  }

  return SpsPolicyPpi;
}

/**

  Get ME_POLICY_PPI pointer

  @param none

  @retval MePolicyPpi           Pointer to ME_POLICY_PPI
                                In error case NULL is returned

**/
ME_POLICY_PPI *
GetMePolicyPpi (
  VOID
)
{
  ME_POLICY_PPI *MePolicyPpi;
  EFI_STATUS Status;

  Status = PeiServicesLocatePpi (
             &gMePolicyPpiGuid,
             0,
             NULL,
             (VOID **) &MePolicyPpi
             );
  if (EFI_ERROR(Status)) {
    MePolicyPpi = NULL;
    ASSERT_EFI_ERROR (Status);
  }

  return MePolicyPpi;
}