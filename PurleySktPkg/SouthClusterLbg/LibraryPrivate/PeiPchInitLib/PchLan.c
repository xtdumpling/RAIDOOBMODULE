/** @file
  Initializes PCH LAN Device in PEI

@copyright
 Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
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

#include "PchInitPei.h"


/**
  Performs global reset required to change GbE static power gating state.
**/
STATIC
VOID
PchGbeGlobalReset (
  VOID
  )
{
  PCH_RESET_PPI   *PchResetPpi;
  EFI_STATUS      Status;

  DEBUG ((DEBUG_ERROR, "PchGbeGlobalReset\n"));

  Status = PeiServicesLocatePpi (
             &gPchResetPpiGuid,
             0,
             NULL,
             (VOID **)&PchResetPpi
             );
  ASSERT_EFI_ERROR (Status);

  PchResetPpi->Reset (PchResetPpi, GlobalReset);
  ASSERT (FALSE);
}

/**
  Enable GbE by disabling static power gating.
**/
STATIC
VOID
PchEnableGbe (
  VOID
)
{
  UINT32  PwrmBase;

  DEBUG ((DEBUG_INFO, "Enabling internal LAN...\n"));

  PchPwrmBaseGet (&PwrmBase);

  //
  // Before modifying LAN Disable bit, make sure it's not locked.
  //
  ASSERT ((MmioRead32 (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1) & B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK) == 0);

  //
  // Enable the Internal GbE Controller
  // Set PWRMBASE + 620h[0] = 0b to enable GbE controller
  //
  MmioAnd32 (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, (UINT32) ~B_PCH_PWRM_ST_PG_FDIS_PMC_1_GBE_FDIS_PMC);
  //
  // Read back for posted write to take effect
  //
  MmioRead32 (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1);
}


/**
  Disable the Internal GbE Controller

  BIOS has the ability to disable the LAN Controller functionality, in addition to
  configuring PMC to disable the LAN Controller BIOS need to set Disable bit in the
  LANDISCTRL register and then lock the LANDISCTRL register by setting the Lock bit in
  the LOCKLANDIS register.
**/
STATIC
VOID
PchDisableGbe (
  VOID
)
{
  UINTN   GbePciBase;
  UINT32  PwrmBase;

  DEBUG ((DEBUG_INFO, "Disabling internal LAN...\n"));

  GbePciBase = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_LAN,
                 PCI_FUNCTION_NUMBER_PCH_LAN
                 );

  ASSERT (MmioRead32 (GbePciBase) != 0xFFFFFFFF);

  //
  // Set LANDISCTRL, GbE PCI config + 0xA0 bit[0] to 1
  //
  MmioOr32 (GbePciBase + R_PCH_LAN_LANDISCTRL, B_PCH_LAN_LANDISCTRL_DISABLE);
  ASSERT (MmioRead32 (GbePciBase) != 0xFFFFFFFF);

  //
  // Set LOCKLANDIS, GbE PCI config + 0xA4 bit[0] to 1
  //
  MmioOr32 (GbePciBase + R_PCH_LAN_LOCKLANDIS, B_PCH_LAN_LOCKLANDIS_LOCK);

  //
  // LAN function is hidden now
  //
  ASSERT (MmioRead32 (GbePciBase) == 0xFFFFFFFF);

  PchPwrmBaseGet (&PwrmBase);
  //
  // Before modifying LAN Disable bit, make sure it's not locked and not disabled yet
  //
  ASSERT ((MmioRead32 (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1) & B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK) == 0);
  ASSERT ((MmioRead32 (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1) & B_PCH_PWRM_ST_PG_FDIS_PMC_1_GBE_FDIS_PMC) == 0);

  //
  // Set PWRMBASE + 620h[0] = 1b to statically disable GbE Controller
  //
  MmioOr32 (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, B_PCH_PWRM_ST_PG_FDIS_PMC_1_GBE_FDIS_PMC);
  //
  // Read back for posted write to take effect
  //
  MmioRead32 (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1);
}

/**
  Configure LAN device registers based on policy settings.
  @param[in] PchPolicy            The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI
                                  devices to be used to initialize MMIO
                                  registers.
**/
STATIC
VOID
PchConfigureGbeRegisters (
  IN  PCH_POLICY_PPI              *PchPolicy
  )
{
  UINTN       GbePciBase;
  UINT32      GbeMemBar;
  PCH_SERIES  PchSeries;

  DEBUG ((DEBUG_INFO, "ConfigureGbe\n"));

  PchSeries = GetPchSeries ();

  GbePciBase = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_LAN,
                 PCI_FUNCTION_NUMBER_PCH_LAN
                 );

  ASSERT (PchPolicy->LanConfig.Enable);
  ASSERT (MmioRead32 (GbePciBase) != 0xFFFFFFFF);

  //
  // Set GbE PCI config + 0x80 (CPCE) bit[5] (HAE) = 1
  //
  MmioOr32 (GbePciBase + R_PCH_LAN_CPCE, B_PCH_LAN_CPCE_HAE);

  //
  // Set GbE PCI config + 0x94 (clock control of GBE) to 0x1F
  // To avoid blocking PCH floor power measurement when GbE enabled.
  //
  MmioWrite8 (GbePciBase + R_PCH_LAN_CLCTL, 0x1F);

  //
  // Set LOCKLANDIS, GbE PCI config + 0xA4 bit[0]
  //
  MmioOr32 (GbePciBase + R_PCH_LAN_LOCKLANDIS, B_PCH_LAN_LOCKLANDIS_LOCK);

  //
  //
  // LAN LTR Programming
  // The maximum snoop/non-snoop platform latency values to 3ms
  // Program GbE controller's PCI LTR capability register at D31:F6:Reg 0A8h
  //
  MmioWrite32 (GbePciBase + R_PCH_LAN_LTR_CAP, 0x08280828);

  //
  // Assign MMIO resource to Gbe controller
  //
  GbeMemBar = PchPolicy->TempMemBaseAddr;
  MmioWrite32 (GbePciBase + R_PCH_LAN_MBARA, GbeMemBar);
  MmioOr16 (GbePciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  //
  // Additional GbE Controller Configurations for WOL Support
  // System BIOS requires to program the registers listed below for internal GbE.
  // 1. Set MBARA + 2Ch[31] = 1b
  // 2. If WOL is enabled set MBARA + Offset 2Ch[30] = 1b
  //    Else if disabled set MBARA + 2Ch[30] = 0b
  //
  if (PchPolicy->PmConfig.WakeConfig.WolEnableOverride) {
    MmioOr32 (GbeMemBar + R_PCH_LAN_CSR_FEXT, B_PCH_LAN_CSR_FEXT_WOL | B_PCH_LAN_CSR_FEXT_WOL_VALID);
  } else {
    MmioAndThenOr32 (GbeMemBar + R_PCH_LAN_CSR_FEXT, (UINT32) ~(B_PCH_LAN_CSR_FEXT_WOL), B_PCH_LAN_CSR_FEXT_WOL_VALID);
  }

  //
  // Enable K1 off to use CLKREQ for GbE
  //
  if (PchPolicy->LanConfig.K1OffEnable) {
    if (PchPolicy->LanConfig.ClkReqSupported) {
      MmioOr32 (GbeMemBar + R_PCH_LAN_CSR_F18, B_PCH_LAN_CSR_F18_K1OFF_EN);
      DEBUG((DEBUG_INFO, "K1Off enabled.\n"));
    } else {
      DEBUG((DEBUG_ERROR, "Error: K1Off requires CLKREQ# support -- disabled!\n"));
    }
  }

    //
  // Enable internal ECC for Gbe Controller
  // Set GbeMemBar + 0x100c [16] to 1b
  // Set GbeMemBar + 0 [19] to 1b
    //
  MmioOr32 (GbeMemBar + R_PCH_LAN_CSR_PBECCSTS, B_PCH_LAN_CSR_PBECCSTS_ECC_EN);
  MmioOr32 (GbeMemBar + R_PCH_LAN_CSR_CTRL, B_PCH_LAN_CSR_CTRL_MEHE);

  //
  // Disable memory space decoding
  //
  MmioAnd16 (GbePciBase + PCI_COMMAND_OFFSET, (UINT16) ~EFI_PCI_COMMAND_MEMORY_SPACE);
  MmioWrite32 (GbePciBase + R_PCH_LAN_MBARA, 0);
    }


/**
  Initialize LAN device.

  Global reset may be perfomed due to static power gating requirements.

  @param[in] PchPolicy            The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI
                                  devices to be used to initialize MMIO
                                  registers.
**/
VOID
PchConfigureLan (
  IN  PCH_POLICY_PPI              *PchPolicy
  )
{
  UINT32                          PchPwrmBase;
  BOOLEAN                         IsGbePresent;

  DEBUG ((DEBUG_INFO, "PchConfigureLan() Start\n"));

  PchPwrmBaseGet (&PchPwrmBase);

  DEBUG ((DEBUG_INFO, "GbePortNumber:  %d\n",  PchGetGbePortNumber ()));
  DEBUG ((DEBUG_INFO, "GbeRegionValid: %d\n",  PchIsGbeRegionValid ()));
  IsGbePresent = PchIsGbePresent ();
  DEBUG ((DEBUG_INFO, "GbePmcEnabled:  %d\n",  IsGbePresent));


  if (PchPolicy->LanConfig.ClkReqSupported) {
    //
    // Enable CLKREQ# if supported by board regardless of GbE being enabled
    // to allow clock shut-down.
    //
    GpioEnableClkreq (PchPolicy->LanConfig.ClkReqNumber);
  }

  if (IsGbePresent) {
      //
      // Apply GbE settings
      //
      PchConfigureGbeRegisters (PchPolicy);
    } else {
      DEBUG ((DEBUG_INFO, "LAN disabled\n"));
    }
  DEBUG ((DEBUG_INFO, "PchConfigureLan() End\n"));
}

