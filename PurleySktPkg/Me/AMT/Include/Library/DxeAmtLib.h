/** @file
  Header file for AMT functionality

@copyright
  Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
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
#ifndef _DXE_AMT_LIB_H_
#define _DXE_AMT_LIB_H_

#include <MeBiosExtensionSetup.h>

//
// Prototype for AMT
//
/**
  Check if Asf is enabled in setup options.

  @retval EFI_SUCCESS             mActiveManagement is not NULL
  @retval                         Error Status code returned by
                                  LocateProtocol.
**/
EFI_STATUS
AmtLibInit (
  VOID
  )
;

/**
  This will return Storage Redirection Boot Option.
  True if the option is enabled.

  @retval True                    Storage Redirection boot option is enabled.
  @retval False                   Storage Redirection boot option is disabled.
**/
BOOLEAN
ActiveManagementEnableStorageRedir (
  VOID
  )
;

/**
  This will return Enforce Secure Boot over Storage Redirection Boot Option.
  True if the option is enabled.

  @retval True                    Enforce Secure Boot is enabled.
  @retval False                   Enforce Secure Boot is disabled.
**/
BOOLEAN
ActiveManagementEnforceSecureBoot (
  VOID
  )
;

/**
  This will return BIOS Pause Boot Option.
  True if the option is enabled.

  @retval True                    BIOS Pause is enabled.
  @retval False                   BIOS Pause is disabled.
**/
BOOLEAN
ActiveManagementPauseBoot (
  VOID
  )
;

/**
  This will return BIOS Setup Boot Option.
  True if the option is enabled.

  @retval True                    BIOS Setup is enabled.
  @retval False                   BIOS Setup is disabled.
**/
BOOLEAN
ActiveManagementEnterSetup (
  VOID
  )
;

/**
  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @retval True                    Console Lock is enabled.
  @retval False                   Console Lock is disabled.
**/
BOOLEAN
EFIAPI
ActiveManagementConsoleLocked (
  VOID
  )
;

/**
  This will return KVM Boot Option.
  True if the option is enabled.

  @retval True                    KVM is enabled.
  @retval False                   KVM is disabled.
**/
BOOLEAN
EFIAPI
ActiveManagementEnableKvm (
  VOID
  )
;

/**
  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @retval True                    Serial-over-Lan is enabled.
  @retval False                   Serial-over-Lan is disabled.
**/
BOOLEAN
ActiveManagementEnableSol (
  VOID
  )
;

/**
  This will return IDE Redirection boot device index to boot

  @retval IdeBootDevice           Return the boot device number to boot 
                                    Bit 1  Bit0
                                     0    0    USBr is connected to CD/DVD device 
                                     0    1    USBr is connected to floppy device 
                                  Bits 2-7: Reserved set to 0
**/
UINT8
ActiveManagementStorageRedirBootDeviceGet (
  VOID
  )
;

/**
  Stop ASF Watch Dog Timer
**/
VOID
AsfStopWatchDog (
  VOID
  )
;

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
  This will return progress event Option.
  True if the option is enabled.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
ActiveManagementFwProgress (
  VOID
  )
;

/**
  Sent initialize KVM message

  @retval True                    KVM Initialization is successful
  @retval False                   KVM is not enabled
**/
BOOLEAN
BdsKvmInitialization (
  VOID
  )
;

//
// Prototype for AMT Policy
//
/**
  Check if AMT is enabled in setup options.

  @retval EFI_SUCCESS             Amt Policy is initialized.
  @retval All other error conditions encountered when no Amt Policy available.
**/
EFI_STATUS
AmtPolicyLibInit (
  VOID
  )
;

/**
  Check if Asf is enabled in setup options.

  @retval FALSE                   Asf is disabled.
  @retval TRUE                    Asf is enabled.
**/
BOOLEAN
AsfSupported (
  VOID
  )
;

/**
  Check if Amt is enabled in setup options.

  @retval FALSE                   Amt is disabled.
  @retval TRUE                    Amt is enabled.
**/
BOOLEAN
AmtSupported (
  VOID
  )
;

/**
  Check if AMT BIOS Extension hotkey was pressed during BIOS boot.

  @retval FALSE                   MEBx hotkey was not pressed.
  @retval TRUE                    MEBx hotkey was pressed.
**/
BOOLEAN
AmtHotkeyPressed (
  VOID
  )
;

/**
  Check if AMT BIOS Extension Selection Screen is enabled in setup options.

  @retval FALSE                   AMT Selection Screen is disabled.
  @retval TRUE                    AMT Selection Screen is enabled.
**/
BOOLEAN
AmtSelectionScreen (
  VOID
  )
;

/**
  Check if AMT WatchDog is enabled in setup options.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
AmtWatchDog (
  VOID
  )
;

/**
  Return BIOS watchdog timer

  @retval UINT16                  BIOS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerBiosGet (
  VOID
  )
;

/**
  Return OS watchdog timer

  @retval UINT16                  OS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerOsGet (
  VOID
  )
;

/**
  Provide CIRA request information from OEM code.

  @retval Check if any CIRA requirement during POST
**/
BOOLEAN
AmtCiraRequestTrigger (
  VOID
  )
;

/**
  Provide CIRA request Timeout from OEM code.

  @retval CIRA require Timeout for MPS connection to be estabilished
**/
UINT8
AmtCiraRequestTimeout (
  VOID
  )
;

/**
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF
**/
UINT8
ManageabilityModeSetting (
  VOID
  )
;

/**
  Provide UnConfigure ME without password request from OEM code.

  @retval Check if unConfigure ME without password request
**/
BOOLEAN
AmtUnConfigureMe (
  VOID
  )
;

/**
  Provide 'Hiding the Unconfigure ME without password confirmation prompt' request from OEM code.

  @retval Check if 'Hide unConfigure ME without password Confirmation prompt' request
**/
BOOLEAN
AmtHideUnConfigureMeConfPrompt (
  VOID
  )
;

/**
  Provide show MEBx debug message request from OEM code.

  @retval Check show MEBx debug message request
 **/
BOOLEAN
AmtMebxDebugMsg (
  VOID
  )
;

/**
  Provide on-board device list table and do not need to report them to AMT.  AMT only need to know removable PCI device
  information.

  @retval on-board device list table pointer other than system device.
**/
UINT32
AmtPciDeviceFilterOutTable (
  VOID
  )
;

/**
  Check if USB provisioning enabled/disabled in Policy.

  @retval FALSE                   USB provisioning is disabled.
  @retval TRUE                    USB provisioning is enabled.
**/
BOOLEAN
USBProvisionSupport (
  VOID
  )
;

/**
  This will return progress event Option.
  True if the option is enabled.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
FwProgressSupport (
  VOID
  )
;

/**
  Check if ForcMebxSyncUp is enabled in setup options.

  @retval FALSE                   ForcMebxSyncUp is disabled.
  @retval TRUE                    ForcMebxSyncUp is enabled.
**/
BOOLEAN
AmtForcMebxSyncUp (
  VOID
  )
;

/**
  Dump ME_BIOS_EXTENSION_SETUP variable

  @param[in] *MeBiosExtensionSetup Pointer to ME_BIOS_EXTENSION_SETUP variable
**/
VOID
DxeMebxSetupVariableDebugDump (
  IN ME_BIOS_EXTENSION_SETUP  *MeBiosExtensionSetup OPTIONAL
  )
;
/**

  Check if AMT WatchDogOs is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDogOs is disabled.
  @retval TRUE                    AMT WatchDogOs is enabled.

**/
BOOLEAN
AmtWatchDogOs (
  VOID
  )
;

#endif
