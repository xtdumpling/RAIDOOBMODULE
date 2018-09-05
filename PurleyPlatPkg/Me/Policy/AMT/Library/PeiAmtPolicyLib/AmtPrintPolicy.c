/** @file
  This file is AmtPolicyLibPei library.

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
#include "PeiAmtPolicyLibrary.h"

//
// Function implementations
//
/**
  Print PEI AMT Policy

  @param[in] AmtPolicyPpi The RC Policy PPI instance
**/
VOID
EFIAPI
AmtPrintPolicyPpi (
  IN  AMT_POLICY_PPI     *AmtPolicyPpi
  )
{
DEBUG_CODE_BEGIN();
  DEBUG ((DEBUG_INFO, " Revision : 0x%x\n", AmtPolicyPpi->Revision));
  ASSERT (AmtPolicyPpi->Revision == AMT_POLICY_PPI_REVISION);

  DEBUG ((DEBUG_INFO, "AmtConfig ---\n"));
  //
  // Byte 1, bit definition for functionality enable/disable
  //
  DEBUG ((DEBUG_INFO, " AmtEnabled : 0x%x\n", AmtPolicyPpi->AmtEnabled));
  DEBUG ((DEBUG_INFO, " WatchDog : 0x%x\n", AmtPolicyPpi->WatchDog));
  DEBUG ((DEBUG_INFO, " AsfEnabled : 0x%x\n", AmtPolicyPpi->AsfEnabled));
  DEBUG ((DEBUG_INFO, " ManageabilityMode : 0x%x\n", AmtPolicyPpi->ManageabilityMode));
  DEBUG ((DEBUG_INFO, " FWProgress : 0x%x\n", AmtPolicyPpi->FWProgress));
  DEBUG ((DEBUG_INFO, " AmtSolEnabled : 0x%x\n", AmtPolicyPpi->AmtSolEnabled));

  //
  // Byte 2-3 OS WatchDog Timer
  //
  DEBUG ((DEBUG_INFO, " WatchDogTimerOs : 0x%x\n", AmtPolicyPpi->WatchDogTimerOs));

  //
  // Byte 4-5 BIOS WatchDog Timer
  //
  DEBUG ((DEBUG_INFO, " WatchDogTimerBios : 0x%x\n", AmtPolicyPpi->WatchDogTimerBios));
DEBUG_CODE_END();
}

