/** @file
  PCH PCIE root port library.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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
#include <Library/BaseLib.h>
#include <Library/MmPciBaseLib.h>
#include <PchAccess.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchPcieRpLib.h>
//
// For SKL PCH-LP, it supports three express port controllers:
//   Controller 1:
//     Port 1-4, Device 28, function 0-3
//   Controller 2:
//     Port 5-8, Device 28, function 4-7
//   Controller 3:
//     port 9-12, Device 29, function 0-3
// For SKL PCH-H, it supports five express port controllers:
//   Controller 1:
//     Port 1-4, Device 28, function 0-3
//   Controller 2:
//     Port 5-8, Device 28, function 4-7
//   Controller 3:
//     port 9-12, Device 29, function 0-3
//   Controller 4:
//     Port 13-16, Device 29, function 4-7
//   Controller 5:
//     port 17-20, Device 27, function 0-3
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PID_SPA,  0 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PID_SPB,  4 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PID_SPC,  8 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PID_SPD, 12 }, // PCH-H only
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PID_SPE, 16 }  // PCH-H only
};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 mPchPcieControllerInfoSize = sizeof (mPchPcieControllerInfo) / sizeof (mPchPcieControllerInfo[0]);

/**
  Get Pch Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
GetPchPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  )
{
  UINTN       Index;
  UINTN       FuncIndex;
  UINT32      PciePcd;

  //
  // if SKL PCH-LP, RpNumber must be < 12.
  // if SKL PCH-H , RpNumber must be < 20.
  //
  if (RpNumber >= GetPchMaxPciePortNum ()) {
    DEBUG ((DEBUG_ERROR, "GetPchPcieRpDevFun invalid RpNumber %x", RpNumber));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Index = RpNumber / PCH_PCIE_CONTROLLER_PORTS;
  FuncIndex = RpNumber - mPchPcieControllerInfo[Index].RpNumBase;
  *RpDev = mPchPcieControllerInfo[Index].DevNum;
  PchPcrRead32 (mPchPcieControllerInfo[Index].Pid, R_PCH_PCR_SPX_PCD, &PciePcd);
  *RpFun = (PciePcd >> (FuncIndex * S_PCH_PCR_SPX_PCD_RP_FIELD)) & B_PCH_PCR_SPX_PCD_RP1FN;

  return EFI_SUCCESS;
}

/**
  Get Root Port physical Number by Pch Pcie Root Port Device and Function Number

  @param[in]  RpDev                 Root port device number.
  @param[in]  RpFun                 Root port function number.
  @param[out] RpNumber              Return corresponding physical Root Port index (0-based)

  @retval     EFI_SUCCESS           Physical root port is retrieved
  @retval     EFI_INVALID_PARAMETER RpDev and/or RpFun are invalid
  @retval     EFI_UNSUPPORTED       Root port device and function is not assigned to any physical root port
**/
EFI_STATUS
EFIAPI
GetPchPcieRpNumber (
  IN  UINTN   RpDev,
  IN  UINTN   RpFun,
  OUT UINTN   *RpNumber
  )
{
  PCH_SERIES  PchSeries;
  UINTN       Index;
  UINTN       FuncIndex;
  UINT32      PciePcd;

  PchSeries = GetPchSeries ();

  //
  // if SKL PCH-LP, must be Dev == 28 and Fun < 8, or Dev == 29 and Fun < 4.
  // if SKL PCH-H,  must be Dev == 28 and Fun < 8, or Dev == 29 and Fun < 8, or Dev == 27 and Fun < 4.
  //
  if (PchSeries == PchLp) {
    if (!((RpDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1) && (RpFun < 8)) &&
        !((RpDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2) && (RpFun < 4)))
    {
      DEBUG ((DEBUG_ERROR, "GetPchPcieRpNumber invalid RpDev %x RpFun %x", RpDev, RpFun));
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
  }
  if (PchSeries == PchH) {
    if (!((RpDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1) && (RpFun < 8)) &&
        !((RpDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2) && (RpFun < 8)) &&
        !((RpDev == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3) && (RpFun < 4))
    ) {
      DEBUG ((DEBUG_ERROR, "GetPchPcieRpNumber invalid RpDev %x RpFun %x", RpDev, RpFun));
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
  }

  for (Index = 0; Index < mPchPcieControllerInfoSize; Index++) {
    if (mPchPcieControllerInfo[Index].DevNum == RpDev) {
      PchPcrRead32 (mPchPcieControllerInfo[Index].Pid, R_PCH_PCR_SPX_PCD, &PciePcd);
      for (FuncIndex = 0; FuncIndex < 4; FuncIndex ++) {
        if (RpFun == ((PciePcd >> (FuncIndex * S_PCH_PCR_SPX_PCD_RP_FIELD)) & B_PCH_PCR_SPX_PCD_RP1FN)) {
          break;
        }
      }
      if (FuncIndex < 4) {
        *RpNumber = mPchPcieControllerInfo[Index].RpNumBase + FuncIndex;
        break;
      }
    }
  }
  if (Index >= mPchPcieControllerInfoSize) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Gets base address of PCIe root port.

  @param RpIndex    Root Port Index (0 based)
  @return PCIe port base address.
**/
UINTN
PchPcieBase (
  IN  UINT32   RpIndex
  )
{
  UINTN   RpDevice;
  UINTN   RpFunction;
  GetPchPcieRpDevFun (RpIndex, &RpDevice, &RpFunction);
  return MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDevice, (UINT32) RpFunction);
}

/**
  Determines whether L0s is supported on current stepping.

  @return TRUE if L0s is supported, FALSE otherwise
**/
BOOLEAN
PchIsPcieL0sSupported (
  VOID
  )
{
  if (GetPchSeries () == PchLp) {
    if (PchStepping () < PchLpC0) {
      return FALSE;
    }
  } else {
    if (PchStepping () < PchHD0) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Some early SKL PCH steppings require Native ASPM to be disabled due to hardware issues:
   - RxL0s exit causes recovery
   - Disabling PCIe L0s capability disables L1
  Use this function to determine affected steppings.

  @return TRUE if Native ASPM is supported, FALSE otherwise
**/
BOOLEAN
PchIsPcieNativeAspmSupported (
  VOID
  )
{
  return PchIsPcieL0sSupported ();
}

