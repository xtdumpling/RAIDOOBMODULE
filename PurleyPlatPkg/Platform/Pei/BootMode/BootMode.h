//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file BootMode.h

  BootMode header file

**/

#ifndef _EFI_BOOT_MODE_H_
#define _EFI_BOOT_MODE_H_

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Platform.h>
#include <BackCompatible.h>
#include <Guid/SetupVariable.h>
#include <Guid/AdminPasswordGuid.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PciLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/SetupLib.h>
#include <Library/PchPlatformLib.h>
#include <Library/GpioLib.h>
#include <Library/PchPlatformLib.h>
#include <Register/PchRegsPmc.h>
#include <GpioPinsSklH.h>
#include <Library/MmPciBaseLib.h>
#include <Ppi/BootInRecoveryMode.h>
#include <IndustryStandard/SmBios.h>

/**
  Parse the status registers for figuring out the wake-up event.

  @param[out]  WakeUpType       Updates the wakeuptype based on the status registers
**/
VOID
GetWakeupEvent (
  OUT  UINT8   *WakeUpType
  );

/**
  Admin password is read and saved to HOB
  to prevent its cleared on defaults load.

  @param PeiServices          - pointer to the PEI Service Table
  @param SystemConfiguration  - pointer to Platform Setup Configuration

  @retval None.

**/
VOID
SaveAdminPassToHob (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  IN SYSTEM_CONFIGURATION   *SystemConfiguration
  );

/**
  Get sleep type after wakeup
  
  @param PeiServices       Pointer to the PEI Service Table.
  SleepType                Sleep type to be returned.

  @retval TRUE              A wake event occured without power failure.
  @retval FALSE             Power failure occured or not a wakeup.

**/
BOOLEAN
GetSleepTypeAfterWakeup (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  OUT UINT16                    *SleepType
  );

/**

    Check to see if CMOS is bad or cleared

    @param None

    @retval TRUE  - CMOS is bad
    @retval FALSE - CMOS is good

**/
BOOLEAN
IsCMOSBad(
     VOID
  );

#endif
