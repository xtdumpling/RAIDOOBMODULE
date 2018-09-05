/** @file
  This file contains functions that configures PCI Express Root Ports function swapping.

@copyright
 Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
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
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchPcieRpLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>
#include <IncludePrivate/Library/PchPciExpressHelpersLib.h>
#include <IncludePrivate/Library/PchInitCommonLib.h>
#include <Library/S3BootScriptLib.h>

/**
  Configure root port function number mapping

  @retval EFI_SUCCESS                   The function completed successfully
**/
// SERVER_BIOS_FLAG sync up to 2015-10-16
EFI_STATUS
PchConfigureRpfnMapping (
  VOID
  )
{
  UINT8                                 PortIndex;
  UINT8                                 OriginalFuncZeroRp;
  UINT8                                 MaxPciePortNum;
  UINT32                                Data32;
  UINTN                                 DevNum;
  UINTN                                 FuncNum;
  UINTN                                 RpBase;
  UINT32                                ControllerPcd[PCH_MAX_PCIE_CONTROLLERS];
  UINT16                                Psf1RpFuncCfgBase;
  PCH_SERIES                            PchSeries;
  UINT32                                PcieControllers;
  UINT32                                ControllerIndex;
  UINT32                                FirstController;
  PCH_SBI_PID                           ControllerPid;

  DEBUG((DEBUG_INFO,"PchConfigureRpfnMapping () Start\n"));
  PchSeries      = GetPchSeries ();
  MaxPciePortNum = GetPchMaxPciePortNum ();

  if (PchSeries == PchLp) {
    PcieControllers   = PCH_LP_PCIE_MAX_CONTROLLERS;
    Psf1RpFuncCfgBase = R_PCH_LP_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_SPA_D28_F0;
  } else {
    PcieControllers   = PCH_H_PCIE_MAX_CONTROLLERS;
    Psf1RpFuncCfgBase = R_PCH_H_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_SPA_D28_F0;
  }
  for (ControllerIndex = 0; ControllerIndex < PcieControllers; ++ControllerIndex) {
    PchPcrRead32 (PchGetPcieControllerSbiPid (ControllerIndex), R_PCH_PCR_SPX_PCD, &ControllerPcd[ControllerIndex]);
  }

  ///
  /// Configure root port function number mapping
  ///
  for (PortIndex = 0; PortIndex < MaxPciePortNum; ) {
    GetPchPcieRpDevFun (PortIndex, &DevNum, &FuncNum);
    RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32)DevNum, (UINT32)FuncNum);
    //
    // Search for first enabled function
    //
    if (MmioRead16 (RpBase) != 0xFFFF) {
      if (FuncNum != 0) {
        //
        // First enabled root port that is not function zero will be swapped with function zero on the same device
        // RP PCD register must sync with PSF RP function config register
        //
        ControllerIndex    = PortIndex / 4;
        OriginalFuncZeroRp = (PortIndex / 8) * 8;
        FirstController    = OriginalFuncZeroRp / 4;

        //
        // The enabled root port becomes function zero
        //
        ControllerPcd[ControllerIndex] &= (UINT32) ~(B_PCH_PCR_SPX_PCD_RP1FN << ((PortIndex % 4) * S_PCH_PCR_SPX_PCD_RP_FIELD));
        ControllerPcd[ControllerIndex] |= 0u;
        //
        // Origianl function zero on the same device takes the numer of the current port
        //
        ControllerPcd[FirstController] &= (UINT32) ~B_PCH_PCR_SPX_PCD_RP1FN;
        ControllerPcd[FirstController] |= (UINT32) FuncNum;

        //
        // Program PSF1 RP function config register.
        //
        PchPcrAndThenOr32 (
          PID_PSF1, Psf1RpFuncCfgBase + 4 * PortIndex,
          (UINT32)~B_PCH_PCR_PSFX_TX_AGENT_FUNCTION_CONFIG_FUNCTION,
          0
          );
        PchPcrRead32 (PID_PSF1, Psf1RpFuncCfgBase + 4 * PortIndex, &Data32);
        PCH_PCR_BOOT_SCRIPT_WRITE (
          S3BootScriptWidthUint32,
          PID_PSF1, Psf1RpFuncCfgBase + 4 * PortIndex,
          1,
          &Data32
          );
        PchPcrAndThenOr32 (
          PID_PSF1, Psf1RpFuncCfgBase + 4 * OriginalFuncZeroRp,
          (UINT32) ~B_PCH_PCR_PSFX_TX_AGENT_FUNCTION_CONFIG_FUNCTION,
          (UINT32) (FuncNum << N_PCH_PCR_PSFX_TX_AGENT_FUNCTION_CONFIG_FUNCTION)
          );
        PchPcrRead32 (PID_PSF1, Psf1RpFuncCfgBase + 4 * OriginalFuncZeroRp, &Data32);
        PCH_PCR_BOOT_SCRIPT_WRITE (
          S3BootScriptWidthUint32,
          PID_PSF1, Psf1RpFuncCfgBase + 4 * OriginalFuncZeroRp,
          1,
          &Data32
          );
      }
      //
      // Once enabled root port was found move to next PCI device
      //
      PortIndex = ((PortIndex / 8) + 1) * 8;
      continue;
    }
    //
    // Continue search for first enabled root port
    //
    PortIndex++;
  }

  //
  // Write to PCD and lock the register
  //
  for (ControllerIndex = 0; ControllerIndex < PcieControllers; ++ControllerIndex) {
    ControllerPid = PchGetPcieControllerSbiPid (ControllerIndex);
    Data32 = ControllerPcd[ControllerIndex] | B_PCH_PCR_SPX_PCD_SRL;
    PchPcrWrite32 (ControllerPid, R_PCH_PCR_SPX_PCD, Data32);
    PCH_PCR_BOOT_SCRIPT_WRITE (
      S3BootScriptWidthUint32,
      ControllerPid, R_PCH_PCR_SPX_PCD,
      1,
      &Data32
      );
  }
  return EFI_SUCCESS;
}
// SERVER_BIOS_FLAG