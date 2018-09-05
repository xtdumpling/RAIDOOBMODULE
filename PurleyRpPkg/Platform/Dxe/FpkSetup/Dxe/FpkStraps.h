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
  );

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
  );

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
  );
