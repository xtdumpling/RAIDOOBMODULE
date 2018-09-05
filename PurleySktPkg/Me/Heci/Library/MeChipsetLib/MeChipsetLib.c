/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2004 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeChipsetLib.c

@brief:
  Me Chipset Lib implementation.

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Config/MeConfig.h>
#include "MeAccess.h"
#include "PchAccess.h"
#include "HeciRegs.h"


/**
  Put ME device out of D0I3

  @param[in] Function  ME function where DOI3 is to be changed

**/
VOID
ClearD0I3Bit (
   IN  UINT32   Function
  )
{
  UINTN        DevicePciCfgBase;
  UINT32       DeviceBar[2];
  UINTN       *pBar;
  UINT8        Cmd;
  UINTN        Timeout;
  UINT32       D0I3Ctrl;

  DEBUG ((DEBUG_INFO, "[HECI%d] Clearing D0I3 bit\n", HECI_NAME_MAP (Function)));
  ///
  /// Get Device MMIO address
  ///
  DevicePciCfgBase = MmPciBase (ME_BUS, ME_DEVICE_NUMBER, Function);
  if (MmioRead16 (DevicePciCfgBase + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    DEBUG ((DEBUG_ERROR, "[HECI%d] Function is disabled, cannot clear D0I3 bit!\n", HECI_NAME_MAP (Function)));
    return;
  }
  DeviceBar[0] = MmioRead32 (DevicePciCfgBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  DeviceBar[1] = 0x0;
  if ((MmioRead32 (DevicePciCfgBase + PCI_BASE_ADDRESSREG_OFFSET) & 0x6) == 0x4) {
    DeviceBar[1] = MmioRead32 (DevicePciCfgBase + (PCI_BASE_ADDRESSREG_OFFSET + 4));
  }

  ///
  /// Put CSME Device out of D0I3
  /// (1) Poll D0I3C[0] CIP bit is 0 with timeout 5 seconds
  /// (2) Write D0I3C[2] = 0
  /// (3) Poll D0I3C[0] CIP bit is 0 with timeout 5 seconds
  ///
  if (!(DeviceBar[0] == 0x0 && DeviceBar[1] == 0x0) && !(DeviceBar[0] == 0xFFFFFFF0 && DeviceBar[1] == 0xFFFFFFFF)) {
    Cmd = MmioRead8(DevicePciCfgBase + PCI_COMMAND_OFFSET);
    if ((Cmd & EFI_PCI_COMMAND_MEMORY_SPACE) != EFI_PCI_COMMAND_MEMORY_SPACE) {
      MmioOr8 (DevicePciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    }
    pBar = (UINTN*)DeviceBar;
    D0I3Ctrl = MmioRead32 (*pBar + D0I3C);
    if ((D0I3Ctrl & BIT2) == BIT2) {
      ///
      /// (1) If entering D0I3 is in progress wait till it finishes. Let's give it 5000 ms timeout.
      ///
      Timeout = 5000;
      while ((D0I3Ctrl & BIT0) == BIT0 && Timeout-- > 0) {
        MicroSecondDelay (1000);
        D0I3Ctrl = MmioRead32 (*pBar + D0I3C);
      }

      ///
      /// (2) Write D0I3C[2] = 0
      ///
      MmioWrite32 (*pBar + D0I3C, D0I3Ctrl & ~BIT2);

      D0I3Ctrl = MmioRead32 (*pBar + D0I3C);
      ///
      /// (3) If exiting D0I3 is in progress wait till it finishes. Let's give it 5000 ms timeout.
      ///
      Timeout = 5000;
      while ((D0I3Ctrl & BIT0) == BIT0 && Timeout-- > 0) {
        MicroSecondDelay (1000);
        D0I3Ctrl = MmioRead32 (*pBar + D0I3C);
      }
    }
    DEBUG ((DEBUG_INFO, "[HECI%d] D0I3C register = %08X\n", HECI_NAME_MAP (Function), MmioRead32 (*pBar + D0I3C)));
    MmioWrite8 (DevicePciCfgBase + PCI_COMMAND_OFFSET, Cmd);
  }
} // ClearD0I3Bit()


/**
  Put ME device into D0I3

  @param[in] Function          Select of Me device

**/
VOID
SetD0I3Bit (
  IN  UINT32   Function
  )
{
  UINTN                           DevicePciCfgBase;
  UINT32                          DeviceBar[2];
  UINTN                           *Bar;

  DEBUG ((DEBUG_INFO, "[HECI%d] Setting D0I3 bit\n", HECI_NAME_MAP (Function)));
  ///
  /// Get Device MMIO address
  ///
  DevicePciCfgBase = MmPciBase (ME_BUS, ME_DEVICE_NUMBER, Function);
  if (MmioRead16 (DevicePciCfgBase + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    DEBUG ((DEBUG_ERROR, "[HECI%d] Function is disabled, can't set D0I3 bit!\n", HECI_NAME_MAP (Function)));
    return;
  }
  if ((MmioRead8(DevicePciCfgBase + R_ME_HIDM) & ~V_ME_HIDM_LOCK) != V_ME_HIDM_MSI) {
    DEBUG ((DEBUG_WARN, "[HECI%d] HIDM is not legacy/MSI, do not set DOI3 bit!\n", HECI_NAME_MAP (Function)));
    return;
  }
  DeviceBar[0] = MmioRead32 (DevicePciCfgBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFFFF0;
  DeviceBar[1] = 0x0;
  if ((MmioRead32 (DevicePciCfgBase + PCI_BASE_ADDRESSREG_OFFSET) & 0x6) == 0x4) {
    DeviceBar[1] = MmioRead32 (DevicePciCfgBase + (PCI_BASE_ADDRESSREG_OFFSET + 4));
  }

  ///
  /// Put CSME Device in D0I3
  ///
  MmioOr8 (DevicePciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
  if (!(DeviceBar[0] == 0x0 && DeviceBar[1] == 0x0) && !(DeviceBar[0] == 0xFFFFFFF0 && DeviceBar[1] == 0xFFFFFFFF)) {
    Bar = (UINTN*)DeviceBar;
    MmioOr32 (*Bar + D0I3C, BIT2);
    DEBUG ((DEBUG_INFO, "[HECI%d] D0I3C register = %08X\n", HECI_NAME_MAP (Function), MmioRead32 (*Bar + D0I3C)));
  }

  MmioAnd8 (DevicePciCfgBase + PCI_COMMAND_OFFSET,(UINT8)~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));
} // SetD0I3Bit

/**

  @brief
  Enable/Disable Me devices

  @param[in] WhichDevice          Select of Me device
  @param[in] DeviceFuncCtrl       Function control


**/
VOID
MeDeviceControl (
  IN  ME_DEVICE             WhichDevice,
  IN  ME_DEVICE_FUNC_CTRL   DeviceFuncCtrl
  )
{
  UINT16                    DevPsfBase;
  UINT32                    PchPwrmBase;
  UINTN                     PciBaseAdd;

  DevPsfBase = 0;
  PciBaseAdd = 0;
  PchPwrmBaseGet (&PchPwrmBase);

  switch (WhichDevice) {
    case HECI1:
    case HECI2:
    case IDER:
    case SOL:
    case HECI3:
      if (WhichDevice == HECI1) {
        DevPsfBase = R_PCH_H_PCR_PSF1_T0_SHDW_HECI1_REG_BASE;
      } else if (WhichDevice == HECI2) {
        DevPsfBase = R_PCH_H_PCR_PSF1_T0_SHDW_HECI2_REG_BASE;
      } else if (WhichDevice == HECI3) {
        DevPsfBase = R_PCH_H_PCR_PSF1_T0_SHDW_HECI3_REG_BASE;
      } else if (WhichDevice == IDER) {
        DevPsfBase = R_PCH_H_PCR_PSF1_T0_SHDW_IDER_REG_BASE;
        PciBaseAdd = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, ME_DEVICE_NUMBER, IDER_FUNCTION_NUMBER);
      } else if (WhichDevice == SOL) {
        DevPsfBase = R_PCH_H_PCR_PSF1_T0_SHDW_KT_REG_BASE;
        PciBaseAdd = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, ME_DEVICE_NUMBER, SOL_FUNCTION_NUMBER);
      } else {
        break;
      }

      if (DeviceFuncCtrl == Enabled) {
        PchPcrAndThenOr32 (
          PID_PSF1, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
          (UINT32)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS,
          0
          );
        DEBUG ((DEBUG_INFO, "[ME] Enabling device 0:22:%d\n", (UINT8)WhichDevice));
        if (WhichDevice == HECI1 /* || WhichDevice == HECI2 || WhichDevice == HECI3*/) {
          ClearD0I3Bit ((UINT32)WhichDevice);
        }
      } else {
        DEBUG ((DEBUG_INFO, "[ME] Disabling device 0:22:%d\n", (UINT8)WhichDevice));
        if (WhichDevice == HECI1 /* || WhichDevice == HECI2 || WhichDevice == HECI3*/) {
          SetD0I3Bit ((UINT32)WhichDevice);
        }
        else if (WhichDevice == IDER || WhichDevice == SOL) {
          MmioWrite32 (PciBaseAdd + R_ME_PMCSR, (UINT32) V_ME_PMCSR);
        }
        PchPcrAndThenOr32 (
          PID_PSF1, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
          (UINT32)~0,
          B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
          );
      }
      break;
    ///
    /// Function Disable SUS well lockdown
    ///
    case FDSWL:
      if (DeviceFuncCtrl) {
        MmioOr32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, (UINT32)(B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK));
      } else {
        MmioAnd32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, (UINT32)(~B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK));
      }
      break;
    default:
      DEBUG((DEBUG_ERROR, "[ME] ERROR: Unknown device %d operation (%d)\n", (UINT8)WhichDevice, DeviceFuncCtrl));
      break;
  }
}

