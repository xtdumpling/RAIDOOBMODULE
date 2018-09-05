/** @file
  This is a library for Amt Heci Message functionality.

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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

#include <PiPei.h>

#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>

#include <Ppi/HeciPpi.h>
#include <CoreBiosMsg.h>
#include <Library/PeiAmtLib.h>
/**
  Start ASF BIOS Watch Dog Timer HECI message
  The WDT will be started only if AMT WatchDog policy is enabled and corresponding timer value is not zero.

  @param[in] HeciPpi              The pointer to HECI PPI
  @param[in] WaitTimerBios        The value of waiting limit in seconds

  @retval EFI_UNSUPPORTED         Current ME mode or Amt policy doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_INVALID_PARAMETER   Timeout value is invalid as 0
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciAsfStartWatchDog (
  IN  HECI_PPI                    *HeciPpi,
  IN  UINT16                      WaitTimerBios
  )
{
  EFI_STATUS                      Status;
  ASF_START_WDT                   AsfStartWdt;
  UINT32                          MeStatus;
  UINT32                          HeciLength;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] PeiHeciAsfStartWatchDog() - Start\n"));

  if (PeiAmtWatchDog () == FALSE) {
    DEBUG ((DEBUG_INFO, "AMT WatchDog policy is not enabled, don't  start ASF BIOS WDT \n"));
    return EFI_UNSUPPORTED;
  }

  if (WaitTimerBios == 0) {
    DEBUG ((DEBUG_ERROR, "0 timeout value, unable to start ASF BIOS WDT \n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = HeciPpi->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    DEBUG ((DEBUG_ERROR, "MeMode is %x, unable to start ASF BIOS WDT \n", MeMode));
    return EFI_UNSUPPORTED;
  }

  Status = HeciPpi->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || (ME_STATUS_ME_STATE_ONLY (MeStatus) != ME_READY)) {
    DEBUG ((DEBUG_ERROR, "MeStatus is %x, unable to start ASF BIOS WDT \n", MeStatus));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((
    DEBUG_INFO,
    "PeiHeciAsfStartWatchDog () - Starting ASF BIOS WDT with timeout %d seconds \n",
    WaitTimerBios
    ));

  ///
  /// Clear Start Watch Dog Timer HECI message
  ///
  ZeroMem ((VOID *) &AsfStartWdt, sizeof (ASF_START_WDT));

  AsfStartWdt.Command         = ASF_MANAGEMENT_CONTROL;
  AsfStartWdt.ByteCount       = ASF_START_WDT_BYTE_COUNT;
  AsfStartWdt.SubCommand      = ASF_SUB_COMMAND_START_WDT;
  AsfStartWdt.VersionNumber   = ASF_START_WDT_VERSION_NUMBER;
  AsfStartWdt.TimeoutLow      = (UINT8) WaitTimerBios;
  AsfStartWdt.TimeoutHigh     = (UINT8) (WaitTimerBios >> 8);
  AsfStartWdt.EventSensorType = ASF_START_WDT_EVENT_SENSOR_TYPE;
  AsfStartWdt.EventType       = ASF_START_WDT_EVENT_TYPE;
  AsfStartWdt.EventOffset     = ASF_START_WDT_EVENT_OFFSET;
  AsfStartWdt.EventSourceType = ASF_START_WDT_EVENT_SOURCE_TYPE_BIOS;
  AsfStartWdt.EventSeverity   = ASF_START_WDT_EVENT_SEVERITY;
  AsfStartWdt.SensorDevice    = ASF_START_WDT_SENSOR_DEVICE;
  AsfStartWdt.SensorNumber    = ASF_START_WDT_SENSOR_NUMBER;
  AsfStartWdt.Entity          = ASF_START_WDT_ENTITY;
  AsfStartWdt.EntityInstance  = ASF_START_WDT_ENTITY_INSTANCE;
  AsfStartWdt.EventData[0]    = ASF_START_WDT_EVENT_DATA_BYTE_0;
  AsfStartWdt.EventData[1]    = ASF_START_WDT_EVENT_DATA_BYTE_1_BIOS_TIMEOUT;

  HeciLength                  = ASF_START_WDT_LENGTH;

  Status = HeciPpi->SendMsg (
                      HECI1_DEVICE,
                      (UINT32 *) &AsfStartWdt,
                      HeciLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_ASF_MESSAGE_ADDR
                      );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to start ASF BIOS WDT \n"));
  }
  DEBUG ((DEBUG_INFO, "[ME] PeiHeciAsfStartWatchDog() - End Status = %r\n",
                      Status));
  return Status;
}

/**
  Stop ASF BIOS Watch Dog Timer HECI message. The function call in PEI phase 
  will not check WatchDog policy, always disable WDT once it is invoked to disable
  any initialized WDT.

  @param[in] HeciPpi              The pointer to HECI PPI

**/
VOID
PeiHeciAsfStopWatchDog (
  IN  HECI_PPI                    *HeciPpi
  )
{
  EFI_STATUS                      Status;
  ASF_STOP_WDT                    AsfStopWdt;
  UINT32                          MeStatus;
  UINT32                          HeciLength;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] PeiHeciAsfStopWatchDog() - Start\n"));

  Status = HeciPpi->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    DEBUG ((DEBUG_ERROR, "MeMode is %x, unable to stop ASF BIOS WDT \n", MeMode));
    return;
  }

  Status = HeciPpi->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || (ME_STATUS_ME_STATE_ONLY (MeStatus) != ME_READY)) {
    DEBUG ((DEBUG_ERROR, "MeStatus is %x, unable to stop ASF BIOS WDT \n", MeStatus));
    return;
  }

  DEBUG ((DEBUG_INFO, "PeiHeciAsfStopWatchDog () - Stopping ASF BIOS WDT...\n"));
  ///
  /// Clear Stop Watch Dog Timer HECI message
  ///
  ZeroMem ((VOID *) &AsfStopWdt, sizeof (ASF_STOP_WDT));

  AsfStopWdt.Command        = ASF_MANAGEMENT_CONTROL;
  AsfStopWdt.ByteCount      = ASF_STOP_WDT_BYTE_COUNT;
  AsfStopWdt.SubCommand     = ASF_SUB_COMMAND_STOP_WDT;
  AsfStopWdt.VersionNumber  = ASF_STOP_WDT_VERSION_NUMBER;

  HeciLength                = ASF_STOP_WDT_LENGTH;
  Status = HeciPpi->SendMsg (
                      HECI1_DEVICE,
                      (UINT32 *) &AsfStopWdt,
                      HeciLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_ASF_MESSAGE_ADDR
                      );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Unable to stop ASF BIOS WDT \n"));
  }

  DEBUG ((DEBUG_INFO, "[ME] PeiHeciAsfStopWatchDog() - End Status = %r\n",
                      Status));
}
