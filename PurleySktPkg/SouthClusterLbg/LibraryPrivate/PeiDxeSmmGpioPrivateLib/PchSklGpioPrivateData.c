/** @file
  This file contains SKL specific GPIO information

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <PchAccess.h>
#include <Library/GpioLib.h>
#include <Library/GpioNativeLib.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <Library/PeiDxeSmmGpioLib/GpioLibrary.h>


//
// GPIO pin for PCIE SCRCLKREQB
// SCRCLKREQB_x -> GPIO pin y
//


GPIO_PAD_NATIVE_FUNCTION mPchHPcieSrcClkReqbPinToGpioMap[PCH_H_PCIE_MAX_CLK_REQ]  =
{
  {GPIO_SKL_H_GPP_B5,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_B6,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_B7,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_B8,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_B9,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_B10, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H0,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H1,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H2,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H3,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H4,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H5,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H6,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H7,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H8,  GpioPadModeNative1},
  {GPIO_SKL_H_GPP_H9,  GpioPadModeNative1}
};

//
// PCHHOTB pin
//
GPIO_PAD_NATIVE_FUNCTION mPchHPchHotbPin = {GPIO_SKL_H_GPP_B23,  GpioPadModeNative2};

//
// CPU GP pins
//

GPIO_PAD_NATIVE_FUNCTION mPchHCpuGpPinMap[4] =
{
  {GPIO_SKL_H_GPP_E3, GpioPadModeNative1}, // CPU_GP_0
  {GPIO_SKL_H_GPP_E7, GpioPadModeNative1}, // CPU_GP_1
  {GPIO_SKL_H_GPP_B3, GpioPadModeNative1}, // CPU_GP_2
  {GPIO_SKL_H_GPP_B4, GpioPadModeNative1}, // CPU_GP_3
};
