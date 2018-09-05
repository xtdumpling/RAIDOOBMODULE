/** @file
  Start Watchdog timer in PEI phase

@copyright
 Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
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

#include "StartWatchDog.h"

/**
  Perform the platform spefific initializations.

  @retval EFI_SUCCESS             if the interface could be successfully installed.
**/
EFI_STATUS
PeiInitStartWatchDog (
  VOID
  )
{
  EFI_STATUS    Status;
  EFI_BOOT_MODE BootMode;
  HECI_PPI      *HeciPpi;
  UINT16        WaitTimerBios;

  DEBUG ((DEBUG_INFO, "PeiInitStartWatchDog () - Start\n"));

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi (
            &gHeciPpiGuid,      // GUID
            0,                  // INSTANCE
            NULL,               // EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi  // PPI
            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// If it is corporate SKU, the first thing for BIOS is to stop ASF BIOS watchdog timer
  /// when BIOS proceeds with normal POST after DID message. The reason is because BIOS
  /// doesn't know what the state of ASF BIOS watchdog timer is in the previous boot.
  ///
  PeiHeciAsfStopWatchDog (HeciPpi);

  WaitTimerBios = PeiAmtWatchTimerBiosGet ();

  ///
  /// Start ASF BIOS watchdog timer if the corresponding option policy is true
  /// with the non-zero value in the BIOS timeout setting.
  ///
  PeiHeciAsfStartWatchDog (HeciPpi, WaitTimerBios);

  DEBUG ((DEBUG_INFO, "PeiInitStartWatchDog () - End\n"));
  return Status;
}
