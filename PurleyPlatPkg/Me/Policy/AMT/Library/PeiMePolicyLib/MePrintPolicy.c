/** @file
  This file is MePolicyLibPei library.

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
#include "PeiMePolicyLibrary.h"
#include <Library/MeConfigLib.h>


//
// Function implementations
//


/**
  Print PEI ME Policy

  @param[in] MePolicyPpi The RC Policy PPI instance
**/
VOID
EFIAPI
MePrintPolicyPpi (
  IN  ME_POLICY_PPI      *MePolicyPpi
  )
{
DEBUG_CODE_BEGIN();
  DEBUG((DEBUG_INFO, "\n---------------------- ME Policy PPI Begin ----------------------\n"));
  DEBUG((DEBUG_INFO, "ME Policy PPI Revision    : %d\n", MePolicyPpi->Revision));
  ASSERT (MePolicyPpi->Revision == ME_POLICY_PPI_REVISION);

  MeConfigPrint(&MePolicyPpi->MeConfig);
  DEBUG((DEBUG_INFO, "\n---------------------- ME Policy PPI End ------------------------\n"));
DEBUG_CODE_END();
}


