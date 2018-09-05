/** @file
  PCH GPIO Sx Isolation Driver Entry

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
#include "PchInitSmm.h"
#include <Library/GpioLib.h>
#include <Library/GpioNativeLib.h>

/**
  This function performs GPIO Sx Isolation for DevSlp pins.
**/
VOID
PchGpioSxIsolationCallback (
  VOID
  )
{
  UINT32         SataPortsMax;
  UINT32         SataPort;
  GPIO_PAD       DevSlpGpioPad;
  
  SataPortsMax = GetPchMaxSataPortNum ();

  //
  // If DevSlp pad is used in its native mode then
  // set PadRstCfg to 10'b (GPIO Reset)
  //
  for (SataPort = 0; SataPort < SataPortsMax; SataPort++) {
    //
    // Check if DevSlp pad is in native mode
    //
    if (GpioIsSataDevSlpPinEnabled (SataPort, &DevSlpGpioPad)) {
      //
      // Program PADCFG_DW0.PadRstCfg
      //
      GpioSetPadResetConfig (DevSlpGpioPad, GpioResetNormal);
    }
  }
}

