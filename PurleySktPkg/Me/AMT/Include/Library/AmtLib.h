/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  2006 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  AmtLib.h

@brief
  Header file for AMT functionality

**/
#ifndef _AMT_LIB_H_
#define _AMT_LIB_H_

/**

  Check if Asf is enabled in setup options.

  @param[in] None.

**/
EFI_STATUS
AmtLibInit (
  VOID
  )
;

/**

  This will return Storage Redirection Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    Storage Redirection boot option is enabled.
  @retval False                   Storage Redirection boot option is disabled.

**/
BOOLEAN
ActiveManagementEnableStorageRedir (
  IN  VOID
  )
;

/**

  This will return Enforce Secure Boot over Storage Redirection Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    Enforce Secure Boot is enabled.
  @retval False                   Enforce Secure Boot is disabled.

**/
BOOLEAN
ActiveManagementEnforceSecureBoot (
  IN  VOID
  )
;

/**

  This will return BIOS Pause Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    BIOS Pause is enabled.
  @retval False                   BIOS Pause is disabled.

**/
BOOLEAN
ActiveManagementPauseBoot (
  IN  VOID
  )
;

/**

  This will return BIOS Setup Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    BIOS Setup is enabled.
  @retval False                   BIOS Setup is disabled.

**/
BOOLEAN
ActiveManagementEnterSetup (
  IN  VOID
  )
;

/**

  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    Console Lock is enabled.
  @retval False                   Console Lock is disabled.

**/
BOOLEAN
EFIAPI
ActiveManagementConsoleLocked (
  IN  VOID
  )
;

/**

  This will return KVM Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    KVM is enabled.
  @retval False                   KVM is disabled.

**/
BOOLEAN
EFIAPI
ActiveManagementEnableKvm (
  IN  VOID
  )
;

/**

  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    Serial-over-Lan is enabled.
  @retval False                   Serial-over-Lan is disabled.

**/
BOOLEAN
ActiveManagementEnableSol (
  IN  VOID
  )
;

/**

  This will return IDE Redirection boot device index to boot

  @param[in] None.

  @retval IdeBootDevice           Return the boot device number to boot
                                    Bit 1  Bit0
                                     0    0    USBr is connected to CD/DVD device
                                     0    1    USBr is connected to floppy device
**/
UINT8
ActiveManagementStorageRedirBootDeviceGet (
  IN VOID
  )
;

/**

  This will return progress event Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.

**/
BOOLEAN
ActiveManagementFwProgress (
  IN  VOID
  )
;

//
// ASF Message
//
#define ASF_START_WDT_VERSION                         0x10
#define ASF_STOP_WDT_VERSION                          0x10

#define ASF_MESSAGE_COMMAND_SENSE_DEVICE_SYSTEM_STATE 0x01
#define ASF_MESSAGE_COMMAND_MANAGEMENT_CONTROL        0x02
#define ASF_MESSAGE_COMMAND_ASF_CONFIGURATION         0x03
#define ASF_MESSAGE_COMMAND_MESSAGE                   0x04
#define ASF_MESSAGE_COMMAND_GETUUID                   0xC7

/**

  Stop ASF Watch Dog Timer

  @param[in] None.

**/
VOID
AsfStopWatchDog (
  VOID
  )
;

#define ASF_START_BIOS_WDT  0
#define ASF_START_OS_WDT    1

/**

  Start ASF Watch Dog Timer

  @param[in] WatchDogType         Which kind of WatchDog, ASF OS WatchDog Timer setting or ASF BIOS WatchDog Timer setting

**/
VOID
AsfStartWatchDog (
  IN  UINT8                       WatchDogType
  )
;

/**

  Sent initialize KVM message

  @param[in] None.

  @retval True                    KVM Initialization is successful
  @retval False                   KVM is not enabled

**/
BOOLEAN
BdsKvmInitialization (
  IN  VOID
  )
;
#endif
