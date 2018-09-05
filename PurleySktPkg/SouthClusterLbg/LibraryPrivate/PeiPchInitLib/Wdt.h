/** @file
  This file contains definitions of WDT functions.

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
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

#ifndef _WDT_PEI_H_
#define _WDT_PEI_H_

/**
  This runtine clears status bits and disable watchdog, then lock the
  WDT registers.
  while WDT is designed to be disabled and locked by policy,
  bios should not enable WDT by WDT PPI. In such case, bios shows the
  warning message but not disable and lock WDT register to make sure
  WDT event trigger correctly.

  @param[in] DisableAndLock             Policy to disable and lock WDT register

  @retval    NONE
**/
VOID
WdtOnPolicyInstalled (
  BOOLEAN                               DisableAndLock
  );

/**
  Initializes watchdog failure bits.
  If there was an unexpected reset, enforces WDT expiration.
  Stores initial WDT state in a HOB, it is useful in flows with S3/S4 resume.
  Stops watchdog.
  Installs watchdog PPI for other modules to use.

  @retval EFI_SUCCESS             When everything is OK
**/
EFI_STATUS
EFIAPI
WdtPeiEntryPoint (
  VOID
  );

/**
  Support for WDT in S3 resume.
  If WDT was enabled during S0->S3 transition, this function will turn on WDT
  just before waking OS. Timeout value will be overridden if it was too small.

  @retval EFI_SUCCESS             When everything is OK
  @retval EFI_NOT_FOUND           WdtHob is not found
**/
EFI_STATUS
WdtEndOfPeiCallback (
  VOID
  );
#endif // _WDTPEI_H_
