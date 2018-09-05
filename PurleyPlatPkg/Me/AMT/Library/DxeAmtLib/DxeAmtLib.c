/** @file
  Implementation file for AMT functionality

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

#include "AmtDxeLibInternals.h"
#include <Protocol/AlertStandardFormat.h>
#include <CoreBiosMsg.h>
#include "MeState.h"
#include "Protocol/HeciProtocol.h"
#include "Library/MeSpsPolicyAccessLib.h"
#include "Library/DxeMeLib.h"

GLOBAL_REMOVE_IF_UNREFERENCED ACTIVE_MANAGEMENT_PROTOCOL  *mActiveManagement = NULL;
//
// AMT GUID and Variable Name
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
{
  EFI_STATUS  Status;

  if (mActiveManagement == NULL) {
    Status = gBS->LocateProtocol (
                    &gActiveManagementProtocolGuid,
                    NULL,
                    (VOID **) &mActiveManagement
                    );
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

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
{
  BOOLEAN                 CurrentState;

  CurrentState  = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetStorageRedirState (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}

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
{
  BOOLEAN CurrentState;

  CurrentState = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetEnforceSecureBootState (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}

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
{
  BOOLEAN CurrentState;

  CurrentState = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetBiosPauseState (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}

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
{
  BOOLEAN CurrentState;

  CurrentState = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetBiosSetupState (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}

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
{
  BOOLEAN CurrentState;

  CurrentState = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetConsoleLockState (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}

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
{
  BOOLEAN CurrentState;

  CurrentState = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetKvmState (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}

/**
  This will return Secure Erase Boot Option.
  True if the option is enabled.

  @retval True                    Secure Erase is enabled.
  @retval False                   Secure Erase is disabled.
**/
BOOLEAN
ActiveManagementEnableSecureErase (
  VOID
  )
{
  BOOLEAN                 CurrentState;

  CurrentState  = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetSecureEraseState (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}


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
{
  BOOLEAN                 CurrentState;
  UINTN                   VariableSize;
  EFI_STATUS              Status;
  ME_BIOS_EXTENSION_SETUP MeBiosExtensionSetupData;
// APTIOV_SERVER_OVERRIDE_RC_START
  CurrentState  = TRUE/*FALSE*/;
// APTIOV_SERVER_OVERRIDE_RC_END
  VariableSize  = 0;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetSolState (mActiveManagement, &CurrentState);
  }

  VariableSize = sizeof (MeBiosExtensionSetupData);
  Status = gRT->GetVariable (
                  ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
                  &gMeBiosExtensionSetupGuid,
                  NULL,
                  &VariableSize,
                  &MeBiosExtensionSetupData
                  );
  if (!EFI_ERROR (Status)) {
    if ((MeBiosExtensionSetupData.AmtSol & SOL_ENABLE) == 0) {
      CurrentState = FALSE;
    }
  }

  return CurrentState;
}

/**
  This will return Storage Redirection boot device number/type

  @retval            Return the boot device number to boot
                                  Bit 1  Bit0
                                   0    0    USBr is connected to CD/DVD device
                                   0    1    USBr is connected to floppy device
                                  Bits 2-7: Reserved set to 0
**/
UINT8
ActiveManagementStorageRedirBootDeviceGet (
  VOID
  )
{
  UINT8 StorageRedirBootDevice;
  StorageRedirBootDevice = 0;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetStorageRedirBootDeviceSelected (mActiveManagement, &StorageRedirBootDevice);
  }

  return StorageRedirBootDevice;
}

/**
  Stop ASF Watch Dog Timer

**/
VOID
AsfStopWatchDog (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT32            HeciLength;
  ASF_STOP_WDT      AsfStopWdt;
  UINT32            MeStatus;
  UINT32            MeMode;
  HECI_PROTOCOL     *Heci;

  DEBUG ((DEBUG_INFO, "[ME] AsfStopWatchDog() - Start\n"));

  if (AmtWatchDog () == FALSE) {
    DEBUG ((DEBUG_INFO, "AMT WatchDog policy is not enabled, don't stop ASF WDT\n"));
    return ;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );

  if (!EFI_ERROR (Status)) {
    Status = Heci->GetMeMode (&MeMode);
    if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
      DEBUG ((DEBUG_ERROR, "MeMode is %x, unable to stop ASF WDT\n", MeMode));
      return ;
    }

    ///
    /// Send WDT message when ME is ready.  Do not care about if ME FW INIT is completed.
    ///
    Status = Heci->GetMeStatus (&MeStatus);
    if (EFI_ERROR (Status) || (ME_STATUS_ME_STATE_ONLY (MeStatus) != ME_READY)) {
      DEBUG ((DEBUG_ERROR, "MeStatus is %x, unable to stop ASF WDT\n", MeStatus));
      return;
    }

    DEBUG ((DEBUG_INFO, "AsfStopWatchDog () - AMT WatchDog policy is enabled, stopping ASF WDT...\n"));

    ZeroMem ((VOID *) &AsfStopWdt, sizeof (ASF_STOP_WDT));
    AsfStopWdt.Command        = ASF_MESSAGE_COMMAND_MANAGEMENT_CONTROL;
    AsfStopWdt.ByteCount      = ASF_STOP_WDT_BYTE_COUNT;
    AsfStopWdt.SubCommand     = ASF_SUB_COMMAND_STOP_WDT;
    AsfStopWdt.VersionNumber  = ASF_STOP_WDT_VERSION_NUMBER;

    HeciLength                = ASF_STOP_WDT_LENGTH;
    Status = Heci->SendMsg (
                    HECI1_DEVICE,
                    (UINT32 *) &AsfStopWdt,
                    HeciLength,
                    BIOS_FIXED_HOST_ADDR,
                    HECI_ASF_MESSAGE_ADDR
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Unable to stop ASF WDT\n"));
    }

  } else {
    DEBUG ((DEBUG_ERROR, "Heci protocol is not existing, unable to stop ASF WDT\n"));
  }
  DEBUG ((DEBUG_INFO, "[ME] AsfStopWatchDog() - End\n"));
}

/**
  Start ASF Watch Dog Timer.
  The WDT will be started only if AMT WatchDog policy is enabled and corresponding timer value is not zero.

  @param[in] WatchDogType         Which kind of WatchDog, ASF OS WatchDog Timer setting or ASF BIOS WatchDog Timer setting

**/
VOID
AsfStartWatchDog (
  IN  UINT8                       WatchDogType
  )
{
  EFI_STATUS        Status;
  UINT32            HeciLength;
  ASF_START_WDT     AsfStartWdt;
  HECI_PROTOCOL     *Heci;
  UINT32            MeStatus;
  UINT32            MeMode;
  UINT16            WaitTimer;

  DEBUG ((DEBUG_INFO, "[ME] AsfStartWatchDog(%d) - Start\n", WatchDogType));

  if (AmtWatchDog () == FALSE) {
    DEBUG ((DEBUG_INFO, "AMT WatchDog policy is not enabled, don't start ASF WDT\n"));
    return ;
  }

  if (WatchDogType == ASF_START_BIOS_WDT) {
    WaitTimer = AmtWatchDogTimerBiosGet ();
  } else {
    WaitTimer = AmtWatchDogTimerOsGet ();
  }
  if (WaitTimer == 0) {
    DEBUG ((DEBUG_ERROR, "Timeout value is 0, unable to start ASF WDT\n"));
    return;
  }

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );

  if (!EFI_ERROR (Status)) {
    Status = Heci->GetMeMode (&MeMode);
    if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
      DEBUG ((DEBUG_ERROR, "MeMode is %x, unable to start ASF WDT\n", MeMode));
      return;
    }

    ///
    /// Send WDT message when ME is ready.  Do not care about if ME FW INIT is completed.
    ///
    Status = Heci->GetMeStatus (&MeStatus);
    if (EFI_ERROR (Status) || (ME_STATUS_ME_STATE_ONLY (MeStatus) != ME_READY)) {
      DEBUG ((DEBUG_ERROR, "MeStatus is %x, unable to start ASF WDT\n", MeStatus));
      return;
    }

    DEBUG ((
      DEBUG_INFO,
      "AsfStartWatchDog () - Starting ASF WDT with timeout %d seconds \n",
      WaitTimer
      ));

    ZeroMem ((VOID *) &AsfStartWdt, sizeof (ASF_START_WDT));
    AsfStartWdt.Command         = ASF_MESSAGE_COMMAND_MANAGEMENT_CONTROL;
    AsfStartWdt.ByteCount       = ASF_START_WDT_BYTE_COUNT;
    AsfStartWdt.SubCommand      = ASF_SUB_COMMAND_START_WDT;
    AsfStartWdt.VersionNumber   = ASF_START_WDT_VERSION_NUMBER;
    AsfStartWdt.EventSensorType = ASF_START_WDT_EVENT_SENSOR_TYPE;
    AsfStartWdt.EventType       = ASF_START_WDT_EVENT_TYPE;
    AsfStartWdt.EventOffset     = ASF_START_WDT_EVENT_OFFSET;
    AsfStartWdt.EventSeverity   = ASF_START_WDT_EVENT_SEVERITY;
    AsfStartWdt.SensorDevice    = ASF_START_WDT_SENSOR_DEVICE;
    AsfStartWdt.SensorNumber    = ASF_START_WDT_SENSOR_NUMBER;
    AsfStartWdt.Entity          = ASF_START_WDT_ENTITY;
    AsfStartWdt.EntityInstance  = ASF_START_WDT_ENTITY_INSTANCE;
    AsfStartWdt.EventData[0]    = ASF_START_WDT_EVENT_DATA_BYTE_0;
    if (WatchDogType == ASF_START_BIOS_WDT) {
      AsfStartWdt.EventSourceType = ASF_START_WDT_EVENT_SOURCE_TYPE_BIOS;
      AsfStartWdt.EventData[1]    = ASF_START_WDT_EVENT_DATA_BYTE_1_BIOS_TIMEOUT;
    } else {
      AsfStartWdt.EventSourceType = ASF_START_WDT_EVENT_SOURCE_TYPE_OS;
      AsfStartWdt.EventData[1]    = ASF_START_WDT_EVENT_DATA_BYTE_1_OS_TIMEOUT;
    }

    AsfStartWdt.TimeoutLow  = (UINT8) WaitTimer;
    AsfStartWdt.TimeoutHigh = (UINT8) (WaitTimer >> 8);

    HeciLength              = ASF_START_WDT_LENGTH;

    Status = Heci->SendMsg (
                    HECI1_DEVICE,
                    (UINT32 *) &AsfStartWdt,
                    HeciLength,
                    BIOS_FIXED_HOST_ADDR,
                    HECI_ASF_MESSAGE_ADDR
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Unable to start ASF WDT\n"));
    }

  } else {
    DEBUG ((DEBUG_ERROR, "Heci protocol is not existing, unable to start ME WDT\n"));
  }
  DEBUG ((DEBUG_INFO, "[ME] AsfStartWatchDog() - End\n"));
}

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
{
  BOOLEAN CurrentState;

  CurrentState = FALSE;

  if (mActiveManagement != NULL) {
    mActiveManagement->GetProgressMsgRequest (mActiveManagement, &CurrentState);
  }

  return CurrentState;
}

/**
  Sent initialize KVM message

  @retval True                    KVM Initialization is successful
  @retval False                   KVM is not enabled
**/
BOOLEAN
BdsKvmInitialization (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT32                          ResponseCode;

  MeReportError (MSG_KVM_WAIT);
  Status = HeciQueryKvmRequest (QUERY_REQUEST, &ResponseCode);
  if (EFI_ERROR (Status)) {
    MeReportError (MSG_KVM_TIMES_UP);
    Status = HeciQueryKvmRequest (CANCEL_REQUEST, &ResponseCode);
  } else if (ResponseCode == KVM_SESSION_CANCELLED) {
    MeReportError (MSG_KVM_REJECTED);
  } else {
    return TRUE;
  }

  return FALSE;
}
