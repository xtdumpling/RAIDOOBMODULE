/** @file
  This file contains routines for GPIO native and chipset specific purpose
  used by Reference Code only.

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
#include <Library/PchInfoLib.h>
#include <Library/PeiDxeSmmGpioLib/GpioLibrary.h>

//
// Chipset specific data
//
//PCIe SRCCLKREQB
extern GPIO_PAD_NATIVE_FUNCTION mPchHPcieSrcClkReqbPinToGpioMap[PCH_H_PCIE_MAX_CLK_REQ];
extern GPIO_PAD_NATIVE_FUNCTION mPchHPchHotbPin;
extern GPIO_PAD_NATIVE_FUNCTION mPchHCpuGpPinMap[4];





/**
  Returns a pad for given CLKREQ# index.

  @param[in]  ClkreqIndex       CLKREQ# number

  @return CLKREQ# pad.
**/
GPIO_PAD
GpioGetClkreqPad (
  IN     UINT32   ClkreqIndex
  )
{
    ASSERT (ClkreqIndex < PCH_H_PCIE_MAX_CLK_REQ);
    return mPchHPcieSrcClkReqbPinToGpioMap[ClkreqIndex].Pad;
}

/**
  Enables CLKREQ# pad in native mode.

  @param[in]  ClkreqIndex       CLKREQ# number

  @return none
**/
VOID
GpioEnableClkreq (
  IN     UINT32   ClkreqIndex
  )
{
  GPIO_CONFIG        PadConfig;
  GPIO_PAD           ClkreqPad;
  GPIO_PAD_MODE      PadMode;

  ZeroMem (&PadConfig, sizeof (PadConfig));
  
    ASSERT (ClkreqIndex < PCH_H_PCIE_MAX_CLK_REQ);
    ClkreqPad = mPchHPcieSrcClkReqbPinToGpioMap[ClkreqIndex].Pad;
    PadMode = mPchHPcieSrcClkReqbPinToGpioMap[ClkreqIndex].Mode;

  PadConfig.PadMode      = PadMode;
  PadConfig.Direction    = GpioDirNone;
  PadConfig.PowerConfig  = GpioResetDeep;
  DEBUG ((DEBUG_INFO, "Enabling CLKREQ%d\n", ClkreqIndex));
  GpioSetPadConfig (ClkreqPad, &PadConfig);
}

/**
  This function checks if GPIO pin for PCHHOTB is in NATIVE MODE

  @param[in]  none

  @retval TRUE                    Pin is in PCHHOTB native mode
          FALSE                   Pin is in gpio mode or is not owned by HOST
**/
BOOLEAN
GpioIsPchHotbPinInNativeMode (
  VOID
  )
{
  EFI_STATUS               Status;
  GPIO_PAD_NATIVE_FUNCTION PchHotbPin;
  GPIO_PAD_MODE            GpioMode;

  
    PchHotbPin = mPchHPchHotbPin;

  Status =  GetGpioPadMode (PchHotbPin.Pad, &GpioMode);

  if ((EFI_ERROR (Status)) || (GpioMode != PchHotbPin.Mode)) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  This function sets CPU GP pins into native mode

  @param[in]  CpuGpPinNum               CPU GP pin number

  @retval Status
**/
EFI_STATUS
GpioSetCpuGpPinsIntoNativeMode (
  IN  UINT32                            CpuGpPinNum
  )
{
  EFI_STATUS               Status;
  GPIO_PAD_NATIVE_FUNCTION *CpuGpPins;
  
  CpuGpPins = mPchHCpuGpPinMap;

  ASSERT (CpuGpPinNum < 4);

  Status = SetGpioPadMode (CpuGpPins[CpuGpPinNum].Pad, CpuGpPins[CpuGpPinNum].Mode);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  return EFI_SUCCESS;
}