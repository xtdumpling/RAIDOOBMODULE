/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include <PiDxe.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>

static GPIO_PAD mPciDisNGpioPad;
static GPIO_PAD mLanDisNGpioPad;

typedef enum {
  FpkGpioOutputDriveLow,
  FpkGpioOutputDriveHigh,
  FpkGpioInput
} FPK_GPIO_MODE;

/**
  Checks if given GPIO pad is at specified level.

  If both RX and TX buffers are disabled, this function enables RX buffer
  (sets pad mode to GpioDirIn) before reading RX level.

  @param   GpioPad  GPIO pad.
  @param   Level    0: low, 1: high.

  @retval  TRUE     Pad at specified level.
  @retval  FALSE    Error occurred or pad not at specified level.

*/
static
BOOLEAN
GpioPadIs (
  GPIO_PAD GpioPad,
  UINT32   Level
  )
{
  EFI_STATUS Status;
  GPIO_CONFIG GpioData;
  UINT32 CurrentLevel;

  Status = GpioGetPadConfig(GpioPad, &GpioData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkStraps: GpioPadIs: ERROR: GpioGetPadConfig(0x%08x, ...) failed: %r\n", GpioPad, Status));
    return FALSE;
  }
  DEBUG ((DEBUG_INFO, "FpkStraps: GpioPadIs: Pad 0x%08x: GpioData.Direction: %d\n", GpioPad, GpioData.Direction));
  if (GpioData.Direction == 15) {
    DEBUG ((DEBUG_INFO, "FpkStraps: GpioPadIs: enabling input buffer\n"));
    GpioData.Direction = GpioDirIn;
    Status = GpioSetPadConfig (GpioPad, &GpioData);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "FpkStraps: GpioPadIs: ERROR: GpioSetPadConfig(0x%08x, ...) failed: %r\n", GpioPad, Status));
      return FALSE;
    }
  }
  if (GpioData.Direction == GpioDirIn) {
    Status = GpioGetInputValue (GpioPad, &CurrentLevel);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "FpkStraps: GpioPadIs: ERROR: GpioGetInputValue(0x%08x, ...) failed: %r\n", GpioPad, Status));
      return FALSE;
    }
    DEBUG ((DEBUG_INFO, "FpkStraps: GpioPadIs: Pad 0x%08x (DirIn): Level: %d, CurrentLevel: %d\n", GpioPad, Level, CurrentLevel));
    return CurrentLevel == Level;
  }
  if (GpioData.Direction == 13) {
    Status = GpioGetOutputValue (GpioPad, &CurrentLevel);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "FpkStraps: GpioPadIs: ERROR: GpioGetOutputValue(0x%08x, ...) failed: %r\n", GpioPad, Status));
      return FALSE;
    }
    DEBUG ((DEBUG_INFO, "FpkStraps: GpioPadIs: Pad 0x%08x (DirOut): Level: %d, CurrentLevel: %d\n", GpioPad, Level, CurrentLevel));
    return CurrentLevel == Level;
  }
  return FALSE;
}

/**
  Sets desired pad mode/level of given pad.

  Note: this function assumes straps are pulled high by board circuitry.

  @param   GpioPad  GPIO pad.
  @param   Mode     FpkGpioOutputDriveLow: set pad mode to output and drive low
                    FpkGpioOutputDriveHigh: set pad mode to output and drive high.
                    FpkGpioInput: set pad mode to input.

  @retval  TRUE     Mode/level changed.
  @retval  FALSE    Expected pad level not reached.

**/
static
BOOLEAN
GpioPadSetMode (
  GPIO_PAD      GpioPad,
  FPK_GPIO_MODE Mode
  )
{
  GPIO_CONFIG GpioData;
  UINT32 CheckLevel = 0;

  ZeroMem (&GpioData, sizeof (GpioData));
  GpioData.PadMode = GpioPadModeGpio;
  GpioData.PowerConfig = GpioResetPwrGood;
  switch (Mode) {
    case FpkGpioOutputDriveLow:
      GpioData.Direction = GpioDirOut;
      GpioData.OutputState = GpioOutLow;
      CheckLevel = 0;
      break;
    case FpkGpioOutputDriveHigh: // currently unused
      GpioData.Direction = GpioDirOut;
      GpioData.OutputState = GpioOutHigh;
      CheckLevel = 1;
      break;
    case FpkGpioInput:
      GpioData.Direction = GpioDirIn;
      CheckLevel = 1; // external pull-up assumed
      break;
    default:
      ASSERT(FALSE);
  }
  GpioSetPadConfig (GpioPad, &GpioData);
  return GpioPadIs (GpioPad, CheckLevel);
}

/**
  Checks if straps are at desired level and adjusts them if necessary.

  Note: this function assumes straps are pulled high by board circuitry.

  @param       Level         Desired level, 0: low, 1: high.
  @param[out]  LevelChanged  Pointer to a BOOLEAN.
                             TRUE: straps were adjusted.
                             FALSE: if straps were not adjusted (already at Level)
                             If retval is FALSE, *LevelChanged is preserved.

  @retval      TRUE          Straps at Level.
  @retval      FALSE         Underlying GPIO function failed. Straps undefined.

**/
BOOLEAN
FpkCheckAndSetStraps (
      UINT32  Level,
  OUT BOOLEAN *LevelChanged
  )
{
  FPK_GPIO_MODE FpkGpioMode = FpkGpioInput;
  BOOLEAN TempLevelChanged = FALSE;

  ASSERT (mPciDisNGpioPad != 0 && mLanDisNGpioPad != 0);
  switch (Level) {
    case 0:
      FpkGpioMode = FpkGpioOutputDriveLow;
      break;
    case 1:
      FpkGpioMode = FpkGpioInput; // external pull-up, disable output buffer
      break;
    default:
      ASSERT(FALSE);
      break;
  }
  if (GpioPadIs (mPciDisNGpioPad, Level) == FALSE) {
    DEBUG ((DEBUG_INFO, "FpkStraps: FpkCheckAndSetStraps: setting PCI_DIS_N (%d)...\n", FpkGpioMode));
    if (GpioPadSetMode (mPciDisNGpioPad, FpkGpioMode) == FALSE) {
      return FALSE;
    }
    TempLevelChanged = TRUE;
  }
  if (GpioPadIs (mLanDisNGpioPad, Level) == FALSE) {
    DEBUG ((DEBUG_INFO, "FpkStraps: FpkCheckAndSetStraps: setting LAN_DIS_N (%d)...\n", FpkGpioMode));
    if (GpioPadSetMode (mLanDisNGpioPad, FpkGpioMode) == FALSE) {
      return FALSE;
    }
    TempLevelChanged = TRUE;
  }
  *LevelChanged = TempLevelChanged;
  return TRUE;
}

/**
  Performs sanity check of GPIOs that drive PCI_DIS_N and LAN_DIS_N straps.

  This function checks if following sanity criteria are met:
  - pads are in GPIO mode AND
  - pads I/O buffers are disabled OR
  - (either of I/O buffers is enabled and) reset config is GpioResetPwrGood

  Objective of this function is detection if configuration of GPIOs was changed outside FPK setup driver.

  Checks performed by this function are best effort tests and returning TRUE does not give 100% confidence
  against configuratio being modified by outside entity.

  @retval  TRUE   Sanity check passed.
  @retval  FALSE  Sanity check failed. Pad(s) configuration changed outside FPK setup driver.

**/
BOOLEAN
FpkSanityCheckStraps (
  VOID
  )
{
  EFI_STATUS Status;
  GPIO_CONFIG GpioData;

  ASSERT (mPciDisNGpioPad != 0 && mLanDisNGpioPad != 0);
  Status = GpioGetPadConfig(mPciDisNGpioPad, &GpioData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkStraps: FpkSanityCheckStraps: ERROR: GpioGetPadConfig(0x%08x, ...) failed: %r\n", mPciDisNGpioPad, Status));
    return FALSE;
  }
  if (GpioData.PadMode != GpioPadModeGpio) {
    DEBUG ((DEBUG_ERROR, "FpkStraps: FpkSanityCheckStraps: ERROR: PCI_DIS_N_GPIO_PAD not in GPIO mode\n"));
    return FALSE;
  }
  if (GpioData.PowerConfig != GpioResetPwrGood && GpioData.Direction != 15) {
    DEBUG ((DEBUG_ERROR, "FpkStraps: FpkSanityCheckStraps: ERROR: PCI_DIS_N_GPIO_PAD configuration changed externally\n"));
    return FALSE;
  }
  Status = GpioGetPadConfig(mLanDisNGpioPad, &GpioData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FpkStraps: FpkSanityCheckStraps: ERROR: GpioGetPadConfig(0x%08x, ...) failed: %r\n", mLanDisNGpioPad, Status));
    return FALSE;
  }
  if (GpioData.PadMode != GpioPadModeGpio) {
    DEBUG ((DEBUG_ERROR, "FpkStraps: FpkSanityCheckStraps: ERROR: LAN_DIS_N_GPIO_PAD not in GPIO mode\n"));
    return FALSE;
  }
  if (GpioData.PowerConfig != GpioResetPwrGood && GpioData.Direction != 15) {
    DEBUG ((DEBUG_ERROR, "FpkStraps: FpkSanityCheckStraps: ERROR: LAN_DIS_N_GPIO_PAD configuration changed externally\n"));
    return FALSE;
  }
  return TRUE;
}

/**
  Stores GPIO pads that drive PCI_DIS_N and LAN_DIS_N in module variable for internal use.

  This function must be called before calling either of following functions:
  - FpkCheckAndSetStraps
  - FpkSanityCheckStraps

  @param  PciDisNGpioPad  GPIO pad that drives PCI_DIS_N
  @param  LanDisNGpioPad  GPIO pad that drives LAN_DIS_N

**/
VOID
FpkSelectStrapPads (
  GPIO_PAD PciDisNGpioPad,
  GPIO_PAD LanDisNGpioPad
  )
{
  ASSERT (PciDisNGpioPad != 0 && LanDisNGpioPad != 0);
  mPciDisNGpioPad = PciDisNGpioPad;
  mLanDisNGpioPad = LanDisNGpioPad;
}
