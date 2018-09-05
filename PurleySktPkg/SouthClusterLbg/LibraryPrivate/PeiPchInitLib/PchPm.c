/** @file
  Initializes PCH power management controller.

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
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PchAccess.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchHsioLib.h>
#include <Library/MmPciBaseLib.h>

#include <Ppi/PchPolicy.h>
#include <IncludePrivate/PchHsio.h>
#include <IncludePrivate/Ppi/PchPmcXramOffsetData.h>

//
// PCH - CPU Power sharing defines
//
#define PCH2CPU_PWR_LIM_WIDTH           5
#define PCH2CPU_PWR_LIM_WIDTH_MASK      0x1F
#define NUM_PRIMARY_PCH2CPU_PL_SUPPORT  3
#define NUM_EXTENDED_PCH2CPU_PL_SUPPORT 4

/**
  Configure PMC related settings when SCS controllers are disabled.
**/
VOID
ConfigurePmcWhenScsDisabled (
  BOOLEAN                               Disable
  )
{
  UINT32                                PchPwrmBase;

  PchPwrmBaseGet (&PchPwrmBase);

  if (Disable == TRUE) {
    MmioOr32 (
      PchPwrmBase + R_PCH_PWRM_31C,
      BIT14
      );
  } else {
    MmioAnd32 (
      PchPwrmBase + R_PCH_PWRM_31C,
      (UINT32) ~BIT14
      );
  }
}

/**
  Configure PMC related settings when CIO2 controllers is disabled.
**/
VOID
ConfigurePmcWhenCio2Disabled (
  BOOLEAN                               Disable
  )
{
  UINT32                                PchPwrmBase;
  PCH_STEPPING                          PchStep;

  PchPwrmBaseGet (&PchPwrmBase);
  PchStep   = PchStepping ();

  if (Disable == TRUE) {
    MmioOr32 (
      PchPwrmBase + R_PCH_PWRM_31C,
      BIT12
      );
  } else {
    if (PchStep >= PchLpC0) {
      MmioAnd32 (
        PchPwrmBase + R_PCH_PWRM_31C,
        (UINT32) ~BIT12
        );
    }
  }
}

/**
  Configure deep Sx programming

  @param[in] PchPolicy                  The PCH Policy instance

  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
ProgramDeepSx (
  PCH_POLICY_PPI               *PchPolicy
  )
{
  UINT32                                PchPwrmBase;
  UINT32                                S3Data32;
  UINT32                                S4Data32;
  UINT32                                S5Data32;

  PchPwrmBaseGet (&PchPwrmBase);
  ///
  /// PCH BIOS Spec Section 19.11  Deep Sx Power Policies
  /// The System BIOS can perform the following register programming guidelines to enable system
  /// enter Deep S3, Deep S4 or Deep S5.
  ///
  ///                            DPS3_EN_DC          DPS3_EN_AC               DPS4_EN_DC              DPS4_EN_AC             DPS5_EN_DC             DPS5_EN_AC
  ///                           PchPwrmBase+28h[1]   PchPwrmBase + 28h[0]     PchPwrmBase + 2Ch[1]    PchPwrmBase + 2Ch[0]   PchPwrmBase + 30h[15]  PchPwrmBase + 30h[14]
  /// Deep Sx disabled                 0              0                            0                        0                       0                 0
  ///
  /// Enabled in S5                    0              0                            0                        0                       1                 1
  ///
  /// Enabled in S4 and S5             0              0                            1                        1                       1                 1
  ///
  /// Enabled in S3, S4 and S5         1              1                            1                        1                       1                 1
  ///
  /// Configuration supported by MOBILE:
  /// Enabled in S5                    0              0                            0                        0                       1                 0
  /// (Battery mode)
  /// Enabled in S4 and S5             0              0                            1                        0                       1                 0
  /// (Battery Mode)
  /// Enabled in S3, S4 and S5         1              0                            1                        0                       1                 0
  /// (Battery Mode)
  ///
  /// NOTE: Mobile platforms support Deep S3/S4/S5 in DC ONLY,
  /// Desktop and Intel C206 Chipset (LPC Dev ID 0x1C56) platforms support Deep S3/S4/S5 in AC ONLY,
  /// Intel C204 Chipset (LPC Dev ID 0x1C54) and Intel C202 Chipset (LPC Dev ID 0x1C52) platforms DO NOT support Deep S4/S5.
  ///
  /// Deep Sx disabled                 0              0                   0                 0                 0                 0
  ///
  switch (PchPolicy->PmConfig.PchDeepSxPol) {
  case PchDpS5AlwaysEn:
    ///
    /// Configuration 2: Enabled in S5/AC-DC
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 0; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 1;
    ///
    S3Data32  = 0;
    S4Data32  = 0;
    S5Data32  = (UINT32) (B_PCH_PWRM_S5AC_GATE_SUS | B_PCH_PWRM_S5DC_GATE_SUS);
    break;

  case PchDpS4S5AlwaysEn:
    ///
    /// Configuration 4: Enabled only in S4-S5
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 1;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 1;
    ///
    S3Data32  = 0;
    S4Data32  = (UINT32) (B_PCH_PWRM_S4AC_GATE_SUS | B_PCH_PWRM_S4DC_GATE_SUS);
    S5Data32  = (UINT32) (B_PCH_PWRM_S5AC_GATE_SUS | B_PCH_PWRM_S5DC_GATE_SUS);
    break;

  case PchDpS3S4S5AlwaysEn:
    ///
    /// Configuration 6: Enabled only in S3-S4-S5
    /// DEEP_S3_POL.DPS3_EN_DC = 1; DEEP_S3_POL.DPS3_EN_AC = 1;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 1;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 1;
    ///
    S3Data32  = (UINT32) (B_PCH_PWRM_S3AC_GATE_SUS | B_PCH_PWRM_S3DC_GATE_SUS);
    S4Data32  = (UINT32) (B_PCH_PWRM_S4AC_GATE_SUS | B_PCH_PWRM_S4DC_GATE_SUS);
    S5Data32  = (UINT32) (B_PCH_PWRM_S5AC_GATE_SUS | B_PCH_PWRM_S5DC_GATE_SUS);
    break;

  case PchDpS5BatteryEn:
    ///
    /// Configuration 1: Enabled in S5/Battery only
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 0; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = 0;
    S4Data32  = 0;
    S5Data32  = (UINT32) (B_PCH_PWRM_S5DC_GATE_SUS);
    break;

  case PchDpS4S5BatteryEn:
    ///
    /// Configuration 3: Enabled only in S4-S5/Battery Mode
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = 0;
    S4Data32  = (UINT32) (B_PCH_PWRM_S4DC_GATE_SUS);
    S5Data32  = (UINT32) (B_PCH_PWRM_S5DC_GATE_SUS);
    break;

  case PchDpS3S4S5BatteryEn:
    ///
    /// Configuration 5: Enabled only in S4-S5/Battery Mode
    /// DEEP_S3_POL.DPS3_EN_DC = 1; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 1; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 1; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = (UINT32) (B_PCH_PWRM_S3DC_GATE_SUS);
    S4Data32  = (UINT32) (B_PCH_PWRM_S4DC_GATE_SUS);
    S5Data32  = (UINT32) (B_PCH_PWRM_S5DC_GATE_SUS);
    break;

  case PchDeepSxPolDisable:
  default:
    ///
    /// Configuration 5: DeepSx Disabled
    /// DEEP_S3_POL.DPS3_EN_DC = 0; DEEP_S3_POL.DPS3_EN_AC = 0;
    /// DEEP_S4_POL.DPS4_EN_DC = 0; DEEP_S4_POL.DPS4_EN_AC = 0;
    /// DEEP_S5_POL.DPS5_EN_DC = 0; DEEP_S5_POL.DPS5_EN_AC = 0;
    ///
    S3Data32  = 0;
    S4Data32  = 0;
    S5Data32  = 0;
    break;
  }

  MmioWrite32 ((PchPwrmBase + R_PCH_PWRM_S3_PWRGATE_POL), S3Data32);
  MmioWrite32 ((PchPwrmBase + R_PCH_PWRM_S4_PWRGATE_POL), S4Data32);
  MmioWrite32 ((PchPwrmBase + R_PCH_PWRM_S5_PWRGATE_POL), S5Data32);

  return EFI_SUCCESS;
}

/**
  Configure miscellaneous power management settings

  @param[in] PchPolicy          The PCH Policy instance

  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
ConfigureMiscPm (
  IN  PCH_POLICY_PPI           *PchPolicy
  )
{
  UINTN                                 PciPmcRegBase;
  UINT32                                PchPwrmBase;
  UINT32                                Data32;
  UINT32                                PmConA;
  UINT32                                PmConB;
  PCH_SERIES                            PchSeries;
  PCH_STEPPING                          PchStep;
  UINT16                                DeviceId;
  UINT32                                BiosSspad7Address;
  UINT32                                BiosSspad7Data;

  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );
  PchPwrmBaseGet (&PchPwrmBase);
  PchStep   = PchStepping ();
  PchSeries = GetPchSeries ();

  ///
  /// Clear power / reset status bits on PCH Corporate
  ///
  Data32 = 0;
  if (PchPolicy->PmConfig.PowerResetStatusClear.MeWakeSts) {
    Data32 |= B_PCH_PWRM_PRSTS_ME_WAKE_STS;
  }
  if (PchPolicy->PmConfig.PowerResetStatusClear.MeHrstColdSts) {
    Data32 |= B_PCH_PWRM_PRSTS_ME_HRST_COLD_STS;
  }
  if (PchPolicy->PmConfig.PowerResetStatusClear.MeHrstWarmSts) {
    Data32 |= B_PCH_PWRM_PRSTS_ME_HRST_WARM_STS;
  }
  if (PchPolicy->PmConfig.PowerResetStatusClear.MeHostPowerDn) {
    Data32 |= B_PCH_PWRM_PRSTS_ME_HOST_PWRDN;
  }
  if (PchPolicy->PmConfig.PowerResetStatusClear.WolOvrWkSts) {
    Data32 |= B_PCH_PWRM_PRSTS_WOL_OVR_WK_STS;
  }
  ///
  /// Handling Status Registers
  /// System BIOS must set 1b to clear the following registers during power-on
  /// and resuming from Sx sleep state.
  /// - PWRMBASE + Offset 10h[0] = 1b
  /// - PWRMBASE + Offset 10h[4] = 1b, needs to be done as early as possible during PEI
  ///   Done in PchEarlyInit ()
  /// - PWRMBASE + Offset 10h[5] = 1b
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_PRSTS, Data32);


  ///
  /// We need to enable LAN_WAKE_PIN_DSX_EN for Wake from both SX and DSX
  ///
  Data32 = MmioRead32 (PchPwrmBase + R_PCH_PWRM_DSX_CFG);
  if (PchPolicy->PmConfig.WakeConfig.LanWakeFromDeepSx == TRUE) {
    Data32 |= B_PCH_PWRM_DSX_CFG_LAN_WAKE_EN;
  } else {
    Data32 &= ~B_PCH_PWRM_DSX_CFG_LAN_WAKE_EN;
  }
  //
  // Disable PCH internal AC PRESENT pulldown
  //
  if (PchPolicy->PmConfig.DisableDsxAcPresentPulldown) {
    Data32 |= B_PCH_PWRM_DSX_CFG_ACPRES_PD_DSX_DIS;
  } else {
    Data32 &= ~B_PCH_PWRM_DSX_CFG_ACPRES_PD_DSX_DIS;
  }
  ///
  /// Enable WAKE_PIN__DSX_EN for Wake
  ///
  if (PchPolicy->PmConfig.WakeConfig.PcieWakeFromDeepSx) {
    Data32 |= B_PCH_PWRM_DSX_CFG_WAKE_PIN_DSX_EN;
  } else {
    Data32 &= ~B_PCH_PWRM_DSX_CFG_WAKE_PIN_DSX_EN;
  }
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_DSX_CFG, Data32);

  ///
  /// Handle wake policy
  ///
  PmConA = MmioRead32 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A);
  PmConB = MmioRead32 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B);

  //
  // Don't clear B_PCH_PMC_GEN_PMCON_A_MS4V, B_PCH_PMC_GEN_PMCON_A_GBL_RST_STS,
  // B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR, B_PCH_PMC_GEN_PMCON_B_HOST_RST_STS, B_PCH_PMC_GEN_PMCON_B_PWR_FLR by accident
  // since those are RW/1C.
  //
  PmConA &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_A_MS4V | B_PCH_PMC_GEN_PMCON_A_GBL_RST_STS);
  PmConB &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR | B_PCH_PMC_GEN_PMCON_B_HOST_RST_STS | B_PCH_PMC_GEN_PMCON_B_PWR_FLR);

  PmConB &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_B_PME_B0_S5_DIS | B_PCH_PMC_GEN_PMCON_B_WOL_EN_OVRD);

  if (PchPolicy->PmConfig.WakeConfig.PmeB0S5Dis) {
    PmConB |= B_PCH_PMC_GEN_PMCON_B_PME_B0_S5_DIS;
  }

  if (PchPolicy->PmConfig.WakeConfig.WolEnableOverride) {
    ///
    /// Wake-On-LAN (WOL) Implementation
    /// Step 1
    /// Clear PMC PCI offset A0h[30] = 0b to ensure the LAN PHY will be powered for WOL
    /// when the power source is either the AC or the DC battery.
    ///
    PmConA &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_DC_PP_DIS;

    ///
    /// Step 2
    /// Clear PMC PCI offset A0h[29] = 0b to ensure the LAN PHY will be powered for WOL in Deep Sx.
    ///
    PmConA &= (UINT32) ~B_PCH_PMC_GEN_PMCON_A_DSX_PP_DIS;

    ///
    /// Step 3
    /// Set PMC PCI offset A0h[28] = 1b to ensure the LAN PHY will be powered for WOL after a G3 transition.
    ///
    PmConA |= (UINT32) B_PCH_PMC_GEN_PMCON_A_AG3_PP_EN;

    ///
    /// Step 4
    /// Set PMC PCI offset A0h[27] = 1b to ensure the LAN PHY will be powered for WOL from Sx.
    ///
    PmConA |= (UINT32) B_PCH_PMC_GEN_PMCON_A_SX_PP_EN;

    ///
    /// Step 5
    /// "PME_B0_EN", ABASE + Offset 28h[13], bit must be programmed to enable wakes
    /// from S1-S4 at the Power Management Controller
    /// Done in ASL code(_PRW)
    ///
    ///
    /// Step 6
    /// Set "WOL Enable Override", PMC PCI offset A4h[13], bit to 1b to guarantee the
    /// LAN-Wakes are enabled at the Power Management Controller, even in surprise
    /// S5 cases such as power loss/return and Power Button Override
    ///
    PmConB |= B_PCH_PMC_GEN_PMCON_B_WOL_EN_OVRD;

    ///
    /// Step 7
    /// Moreover, system BIOS also require to enables in the LAN device by performing
    /// the WOL configuration requirements in the GbE region of the SPI flash.
    /// Done in PchSmmSxGoToSleep() SMM handler.
    ///
  } else {
    ///
    /// PMC PCI offset A0h[30:27] and PMC PCI offset A4h[13] are all in RTC or DSW well, so BIOS also
    /// needs to program them while WOL setup option is disabled.
    ///
    PmConA &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_A_AG3_PP_EN | B_PCH_PMC_GEN_PMCON_A_SX_PP_EN);
    PmConA |= (B_PCH_PMC_GEN_PMCON_A_DC_PP_DIS | B_PCH_PMC_GEN_PMCON_A_DSX_PP_DIS);

    PmConB &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_B_WOL_EN_OVRD);
  }

  ///
  /// Configure On DC PHY Power Diable according to policy SlpLanLowDc.
  /// When this bit is set, SLP_LAN# will be driven low when ACPRESENT is low.
  /// This indicates that LAN PHY should be powered off on battery mode.
  /// This will override the DC_PP_DIS setting by WolEnableOverride.
  ///
  if (PchPolicy->PmConfig.SlpLanLowDc) {
    PmConA |= (UINT32) (B_PCH_PMC_GEN_PMCON_A_DC_PP_DIS);
  } else {
    PmConA &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_A_DC_PP_DIS);
  }

  ///
  /// Enabling SLP_S3# and SLP_S4# Stretch
  /// PMC PCI offset A4h[12:10]
  /// PMC PCI offset A4h[5:3]
  ///
  PmConB &= (UINT32) ~(B_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW | B_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW);

  switch (PchPolicy->PmConfig.PchSlpS3MinAssert) {
    case PchSlpS360us:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_60US;
      break;

    case PchSlpS31ms:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_1MS;
      break;

    case PchSlpS350ms:
    default:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_50MS;
      break;

    case PchSlpS32s:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_2S;
      break;
  }

  switch (PchPolicy->PmConfig.PchSlpS4MinAssert) {
    case PchSlpS4PchTime:
      PmConB &= (UINT32) (~B_PCH_PMC_GEN_PMCON_B_SLP_S4_ASE);
      break;

    case PchSlpS41s:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_1S | B_PCH_PMC_GEN_PMCON_B_SLP_S4_ASE;
      break;

    case PchSlpS42s:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_2S | B_PCH_PMC_GEN_PMCON_B_SLP_S4_ASE;
      break;

    case PchSlpS43s:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_3S | B_PCH_PMC_GEN_PMCON_B_SLP_S4_ASE;
      break;

    case PchSlpS44s:
    default:
      PmConB |= V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_4S | B_PCH_PMC_GEN_PMCON_B_SLP_S4_ASE;
      break;
  }

  if (PchPolicy->PmConfig.SlpStrchSusUp == FALSE) {
    PmConB |= B_PCH_PMC_GEN_PMCON_B_DISABLE_SX_STRETCH;
  } else {
    PmConB &= (UINT32)~B_PCH_PMC_GEN_PMCON_B_DISABLE_SX_STRETCH;
  }

  MmioWrite32 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A, PmConA);
  MmioWrite32 (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B, PmConB);

  ///
  /// Set Power Management Initialization Register (PMIR) Field 1, PMC PCI offset ACh[31] = 1b
  /// Done in Intel Management Engine Framework Reference Code
  /// Set GEN_PMCON_LOCK register, PMC PCI offset A6h = 06h, after stretch and ACPI base programming completed.
  /// Done in PchOnEndOfDxe()
  ///

  ///
  /// Note: PchPwrmBase + 18h[19:16] are platform dependent settings (0Fh provides longest assertion),
  /// please consult with your board design engineers for correct values to be programmed to.
  ///
  /// For PchPwrmBase + 18h[9:8] Reset Power Cycle Duration could be customized, please refer to EDS
  /// and make sure the setting correct, which never less than the following register.
  /// - GEN_PMCON_3.SLP_S3_MIN_ASST_WDTH
  /// - GEN_PMCON_3.SLP_S4_MIN_ASST_WDTH
  /// - PM_CFG.SLP_A_MIN_ASST_WDTH
  /// - PM_CFG.SLP_LAN_MIN_ASST_WDTH
  ///
  Data32 = MmioRead32 (PchPwrmBase + R_PCH_PWRM_CFG);
  Data32 &= (UINT32)~(B_PCH_PWRM_CFG_SSMAW_MASK | B_PCH_PWRM_CFG_SAMAW_MASK | B_PCH_PWRM_CFG_RPCD_MASK);
  Data32 |= BIT3;
  switch (PchPolicy->PmConfig.PchSlpSusMinAssert) {
    case PchSlpSus0ms:
      Data32 |= V_PCH_PWRM_CFG_SSMAW_0S;
      break;

    case PchSlpSus500ms:
      Data32 |= V_PCH_PWRM_CFG_SSMAW_0_5S;
      break;

    case PchSlpSus1s:
      Data32 |= V_PCH_PWRM_CFG_SSMAW_1S;
      break;

    case PchSlpSus4s:
    default:
      Data32 |= V_PCH_PWRM_CFG_SSMAW_4S;
      break;
  }
  switch (PchPolicy->PmConfig.PchSlpAMinAssert) {
    case PchSlpA0ms:
      Data32 |= V_PCH_PWRM_CFG_SAMAW_0S;
      break;

    case PchSlpA4s:
      Data32 |= V_PCH_PWRM_CFG_SAMAW_4S;
      break;

    case PchSlpA98ms:
      Data32 |= V_PCH_PWRM_CFG_SAMAW_98ms;
      break;

    case PchSlpA2s:
    default:
      Data32 |= V_PCH_PWRM_CFG_SAMAW_2S;
      break;
  }
  switch (PchPolicy->PmConfig.PchPwrCycDur) {
    case 0:  // treat as PCH default
      Data32 |= V_PCH_PWRM_CFG_RPCD_4S;
      break;

    case 1:
      Data32 |= V_PCH_PWRM_CFG_RPCD_1S;
      break;

    case 2:
      Data32 |= V_PCH_PWRM_CFG_RPCD_2S;
      break;

    case 3:
      Data32 |= V_PCH_PWRM_CFG_RPCD_3S;
      break;

    case 4:
      Data32 |= V_PCH_PWRM_CFG_RPCD_4S;
      break;

    default:
      Data32 |= V_PCH_PWRM_CFG_RPCD_4S;
      DEBUG ((DEBUG_ERROR, "Invalid value for PchPwrCycDur. Using 4Sec as the default value.\n"));
      break;
  }
  // HSD 4928868 - [LBG Si request] UEFI FW to program t581 timer to 10ms in EP configuration
  Data32 |= V_PCH_PWRM_CFG_TIMING_T581_10MS;
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_CFG, Data32);

  ///
  /// For wake on WLAN from S3/S4/S5, set PWRM_CFG3 [BIT17], HOST_WLAN_PP_EN = 1
  ///
  if (PchPolicy->PmConfig.WakeConfig.WoWlanEnable == TRUE) {
    MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG3, B_PCH_PWRM_CFG3_HOST_WLAN_PP_EN);
    ///
    /// For wake on WLAN from DeepSx S3/S4/S5, set PWRM_CFG3 [BIT16], DSX_WLAN_PP_EN = 1
    ///
    if (PchPolicy->PmConfig.WakeConfig.WoWlanDeepSxEnable == TRUE) {
      MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG3, B_PCH_PWRM_CFG3_DSX_WLAN_PP_EN);
    }
  }

  ///
  /// s4927906 UEFI FW to enable Dirty Warm Reset by default
  /// Enable the Global to Host reset promotion feature by setting 0x10Ch[0]=1
  /// when setup option is Enabled
  /// For HEDT SKUs it needs to be disabled
  ///
  if (PchPolicy->PmConfig.DirtyWarmReset){
    DeviceId = GetPchLpcDeviceId();
    MmioWrite32 (PchPwrmBase + R_PCH_PWRM_GBL2HOST_EN, PchPolicy->PmConfig.PchGbl2HostEn.Value);

    if (PchPolicy->PmConfig.Dwr_MeResetPrepDone) {
      MmioOr32(PciPmcRegBase + R_PCH_PMC_ETR3, BIT17);
    } else {
      MmioAnd32(PciPmcRegBase + R_PCH_PMC_ETR3, (UINT32)~BIT17);
    }

    if (PchPolicy->PmConfig.Dwr_IeResetPrepDone) {
      MmioOr32(PciPmcRegBase + R_PCH_PMC_ETR3, BIT16);
    } else {
      MmioAnd32(PciPmcRegBase + R_PCH_PMC_ETR3, (UINT32)~BIT16);
    }

    if (!IS_PCH_LBG_WS_LPC_DEVICE_ID(DeviceId)) {
      Data32  = B_PCH_PWRM_GBL2HOST_EN_G2H_FEAT_EN;
      MmioOr32 (
        PchPwrmBase + R_PCH_PWRM_GBL2HOST_EN,
        Data32
        );
    }

    if (PchPolicy->PmConfig.StallDirtyWarmReset){
      BiosSspad7Address = MmPciAddress (
                          0,
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          UBOX_SCRATCH_DEVICE,
                          UBOX_SCRATCH_FUNC2,
                          UBOX_BIOS_SCRATCHPAD7
                          );

      //
      // s4929724 Add debug ability to stall in the DWR flow prior to the reset
      //
      BiosSspad7Data = MmioRead32(BiosSspad7Address);
      if (BiosSspad7Data != 0xFFFFFFFF) {
        BiosSspad7Data |= BIT25;
        MmioWrite32(BiosSspad7Address, BiosSspad7Data);
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Configure PCH to CPU energy report

  @param[in] PchPolicy                  The PCH Policy instance

**/
VOID
ConfigureEnergyReport (
  IN  PCH_POLICY_PPI                    *PchPolicy
  )
{
    return;
}

/**
  Configure PSF power management.
  Must be called after all PSF configuration is completed.
**/
VOID
ConfigurePsfPm (
  VOID
  )
{
  UINT16        DeviceId;

  PchPcrAndThenOr32 (PID_PSF1,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSF2,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSF3,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_PSF4,     R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  PchPcrAndThenOr32 (PID_CSME_PSF, R_PCH_PCR_PSF_GLOBAL_CONFIG, ~0u, B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG);
  //
  // Upon LBG Si Arch request s4929226 Trunk clock gating for PSF's 1,2,3,8,9 and 10 when with on LBG-NS must be disabled
  //
  PchPcrAndThenOr32 (PID_PSF1, R_PCH_PCR_PSF_GLOBAL_CONFIG, (UINT32)~B_PCH_PCR_PSF_GLOBAL_CONFIG_ENLCG, 0);
  PchPcrAndThenOr32 (PID_PSF2, R_PCH_PCR_PSF_GLOBAL_CONFIG, (UINT32)~B_PCH_PCR_PSF_GLOBAL_CONFIG_ENLCG, 0);
  PchPcrAndThenOr32 (PID_PSF3, R_PCH_PCR_PSF_GLOBAL_CONFIG, (UINT32)~B_PCH_PCR_PSF_GLOBAL_CONFIG_ENLCG, 0);
  PchPcrAndThenOr32 (PID_PSF8, R_PCH_PCR_PSF_GLOBAL_CONFIG, (UINT32)~B_PCH_PCR_PSF_GLOBAL_CONFIG_ENLCG, 0);
  PchPcrAndThenOr32 (PID_PSF9, R_PCH_PCR_PSF_GLOBAL_CONFIG, (UINT32)~B_PCH_PCR_PSF_GLOBAL_CONFIG_ENLCG, 0);
  //
  // Check whether we are running on LBG-NS. If yes, then disable LCG on PSF10 as well.
  //
  DeviceId = GetPchLpcDeviceId();
  if (IS_PCH_LBG_NS_LPC_DEVICE_ID(DeviceId)) {
    PchPcrAndThenOr32 (PID_PSF10, R_PCH_PCR_PSF_GLOBAL_CONFIG, (UINT32)~B_PCH_PCR_PSF_GLOBAL_CONFIG_ENLCG, 0);
  }
}

/**
  Configure SPI/eSPI power management
**/
VOID
ConfigureSpiPm (
  VOID
  )
{
  //
  // Set SPI PCR 0xC004 bits 29, 10-8, 5-0 to 1b, 111b, 11111b
  // Clear SPI PCR 0xC008 bit 2,1,0
  PchPcrAndThenOr32 (PID_ESPISPI, R_PCH_PCR_SPI_CLK_CTL, ~0u, 0x2000041D);
  PchPcrAndThenOr32 (PID_ESPISPI, R_PCH_PCR_SPI_PWR_CTL, (UINT32)~(BIT2 | BIT1 | BIT0), 0x00);
}

/**
  Configure LPC power management.

  @param[in] PchPolicy                  The PCH Policy instance
**/
VOID
ConfigureLpcPm (
  IN  PCH_POLICY_PPI                    *PchPolicy
  )
{
  UINTN                                 PciLpcRegBase;

  PciLpcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC
                      );

  ///
  /// System BIOS is also required to set following bit.
  /// PCI CLKRUN# Enable" bit (LPC PCI offset E0h[0]) = 1b
  ///
  if (PchPolicy->PmConfig.PciClockRun == FALSE) {
    MmioAnd16 ((UINTN) (PciLpcRegBase + R_PCH_LPC_PCC), (UINT16) (~B_PCH_LPC_PCC_CLKRUN_EN));
  } else {
    MmioOr16 ((UINTN) (PciLpcRegBase + R_PCH_LPC_PCC), (UINT16) (B_PCH_LPC_PCC_CLKRUN_EN));
  }
}

/**
  Perform power management initialization

  @param[in] PchPolicy                  The PCH Policy PPI instance

  @retval EFI_SUCCESS                   Succeeds.
**/
EFI_STATUS
PchPmInit (
  IN  PCH_POLICY_PPI                    *PchPolicy
  )
{
  UINTN                                 PciPmcRegBase;
  PCH_STEPPING                          PchStep;
  UINT32                                PchPwrmBase;
  UINT32                                Data32;
  UINT32                                Data32And;
  UINT32                                Data32Or;
  BOOLEAN                               DciEnabled;
  PCH_SERIES                            PchSeries;
  UINT8                                 LaneOwner;
  UINT8                                 PortId;
  UINT8                                 LaneNum;
  UINT8                                 MaxUsb3LaneNum;

  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );
  PchPwrmBaseGet (&PchPwrmBase);

  PchStep    = PchStepping ();
  PchSeries  = GetPchSeries ();
  Data32     = 0;

  DciEnabled = PchPolicy->DciConfig.DciEn == TRUE;
  if ((DciEnabled == FALSE) && (PchPolicy->DciConfig.DciAutoDetect == TRUE)) {
    ///
    /// Detect DCI being avaiable (PCR[DCI] + 0x4[9] == 1 or PCR[DCI] + 0x4[10] == 1)
    ///
    PchPcrRead32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, &Data32);
    DciEnabled = (Data32 & (BIT9 | BIT10)) != 0;
  }

  //
  // Perform PM recommendation settings
  //

  ///
  /// GEN_PMCON_A (0x0A0h[13]) = 1, Allow OPI PLL Shutdown in C0 (ALLOW_OPI_PLL_SD_INC0)
  /// GEN_PMCON_A (0x0A0h[12]) = 1, Allow SPXB Clock Gating in C0 (ALLOW_SPXB_CG_INC0)
  /// GEN_PMCON_A (0x0A0h[7])  = 1, Allow L1.LOW Entry during C0 (ALLOW_L1LOW_C0)
  /// GEN_PMCON_A (0x0A0h[6])  = 1, Allow L1.LOW Entry with OPI Voltage ON (ALLOW_L1LOW_OPI_ON)
  /// GEN_PMCON_A (0x0A0h[5])  = 1, Allow L1.LOW Entry with CPU BCLK REQ Asserted (ALLOW_L1LOW_BCLKREQ_ON)
  ///
  //
  // Do not clear those RW/1C bits by accident.
  //
  MmioAndThenOr32 (
    PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A,
    (UINT32) ~(B_PCH_PMC_GEN_PMCON_A_MS4V | B_PCH_PMC_GEN_PMCON_A_GBL_RST_STS),
    B_PCH_PMC_GEN_PMCON_A_ALLOW_PLL_SD_INC0 |
    B_PCH_PMC_GEN_PMCON_A_ALLOW_SPXB_CG_INC0 |
    B_PCH_PMC_GEN_PMCON_A_ALLOW_L1LOW_C0 |
    B_PCH_PMC_GEN_PMCON_A_ALLOW_L1LOW_OPI_ON |
    B_PCH_PMC_GEN_PMCON_A_ALLOW_L1LOW_BCLKREQ_ON
    );

  ///
  /// PMC CFG offset 0xA8 [14, 10, 9] to all 1
  /// This register MUST be programmed before PM_SYNC_MODE is programmed.
  ///
  MmioOr32 (
    PciPmcRegBase + R_PCH_PMC_BM_CX_CNF,
    B_PCH_PMC_BM_CX_CNF_PHOLD_BM_STS_BLOCK |
    B_PCH_PMC_BM_CX_CNF_BM_STS_ZERO_EN |
    B_PCH_PMC_BM_CX_CNF_PM_SYNC_MSG_MODE
    );

  ///
  /// PWRMBASE + 0x14 [24,18,16,13,11,10,9,8,7,6,5,4,3,2,1,0] = all 1s
  ///
  MmioOr32 (
    PchPwrmBase + R_PCH_PWRM_14,
    BIT24 | BIT18 | BIT16 | BIT13 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
    );

  ///
  /// PWRMBASE + 0x3C[23,22] = 11b
  /// Bit[26] should be set/clear once BIOS is ready to bring DRAM out of reset (handled in SA)
  ///
  Data32Or = BIT23 | BIT22;
  ///
  /// PWRMBASE + 0x3C[31:29]
  /// For PCH power button override period, 0=4sec, 011b=10sec
  ///
  Data32Or |= (PchPolicy->PmConfig.PwrBtnOverridePeriod) << N_PCH_PWRM_CFG2_PBOP;
  ///
  /// PWRMBASE + 0x3C[28] for power button native mode disable
  ///
  if (PchPolicy->PmConfig.DisableNativePowerButton) {
    Data32Or |= B_PCH_PWRM_CFG2_PB_DIS;
  }
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG2, Data32Or);

  //
  // 5332729: [SKX B0 PO] [LBG] Need BIOS to fix PM register values
  //

  ///
  /// PWRMBASE + 0x48 = 0x1E2DD701
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_SN_SLOW_RING, 0x020DDF01);

  ///
  /// PWRMBASE + 0x4C = 0x00000001
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_SN_SLOW_RING2, 0x00010001);

  ///
  /// PWRMBASE + 0x50 = 0x1E2DD701
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_SN_SA, 0x020DDF01);

  ///
  /// PWRMBASE + 0x54 = 0x00000001
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_SN_SA2, 0x00010001);

  ///
  /// PWRMBASE + 0x58 = 0x0001C000
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_SN_SLOW_RING_CF, 0x0001C000);

  ///
  /// PWRMBASE + 0x68 = 0x04043400
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_NS_SA, 0x04041C00);

  ///
  /// PWRMBASE + 0x80 = 0xBF8FF701
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_CW_SLOW_RING, 0x9F85DB01);

  ///
  /// PWRMBASE + 0x84 = 0x0201C7E1
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_CW_SLOW_RING2, 0x0181C7E1);

  ///
  /// PWRMBASE + 0x88 = 0x3F8FF701
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_CW_SA, 0x9F85DB01);

  ///
  /// PWRMBASE + 0x8C = 0x0201C7E1
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_CW_SA2, 0x0181C7E1);

  ///
  /// PWRMBASE + 0x98 = 0x0001C000
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_CW_SLOW_RING_CF, 0x0001C000);

  ///
  /// PWRMBASE + 0xA8 = 0x00181300
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_PA_SLOW_RING, 0x00181300);

  ///
  /// PWRMBASE + 0xAC = 0x02006001
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_PA_SLOW_RING2, 0x02006001);

  ///
  /// PWRMBASE + 0xB0 = 0x00181300
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_PA_SA, 0x00181300);

  ///
  /// PWRMBASE + 0xB4 = 0x02006001
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_PA_SA2, 0x02006001);

  ///
  /// PWRMBASE + 0xC0 = 0x00060A00
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_EN_MISC_EVENT, 0x00060A00);

  ///
  /// PWRMBASE + 0xD0 = 0x00200840
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_PM_SYNC_STATE_HYS, 0x00200840);

  ///
  /// PWRMBASE + 0xD4 |= 0x00000026
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE, 0x00000026);

  ///
  /// PWRMBASE + 0xE0 = 0x00040001
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_CFG3, 0x00040001);

  ///
  /// PWRMBASE + 0xE4 = 0x16BF0003
  ///
  // This register is used based on messages received on PM_DOWN pin which is not supported in Purley
  // MmioWrite32 (PchPwrmBase + R_PCH_PWRM_PM_DOWN_PPB_CFG, 0x16BF0003);

  ///
  /// PWRMBASE + 0xF4 |= 0x00000000
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE_C0, 0x00000000);


  ///
  /// For LP, set R_PCH_PWRM_MODPHY_PM_CFG1[MLS0SWPGP] Bits 31:16 = 0xFFFF  
  ///
  if (PchSeries == PchLp) {
    MmioOr32 (PchPwrmBase + R_PCH_PWRM_MODPHY_PM_CFG1, 0xFFFF0000);
  }
  ///
  /// Clear corresponding bit in MLSXSWPGP when ModPHY SUS Well Lane Power Gating is not permitted in Sx
  /// As USB3 lanes do not support SUS Well Power Gating as they support in-band wake, the bits corresponding
  /// to USB3 lanes need to be cleared
  ///
  if (PchSeries == PchLp) {
    MaxUsb3LaneNum = 5;
  } else {
    MaxUsb3LaneNum = 9;
  }
  for (LaneNum = 0; LaneNum <= MaxUsb3LaneNum; LaneNum++){
    if (PchGetLaneInfo (LaneNum, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_USB3) {
        MmioAnd32 (
          PchPwrmBase + R_PCH_PWRM_MODPHY_PM_CFG1,
          (UINT32) ~(1 << LaneNum)
          );
      }
    }
  }
  ///
  /// MODPHY_PM_CFG2 (0x204h[28:24]) = 01010, External FET Ramp Time (EFRT)
  ///
  /// If DCI is enabled, skip 0x204 [28:24] programming. Disable ModPHY sus power gating feature
  ///
  Data32And = (UINT32) ~(B_PCH_PWRM_MODPHY_PM_CFG2_EMFC | B_PCH_PWRM_MODPHY_PM_CFG2_EFRT);
  Data32Or  = 0;
  if (DciEnabled == FALSE) {
    Data32Or  |= (V_PCH_PWRM_MODPHY_PM_CFG2_EFRT_200US << N_PCH_PWRM_MODPHY_PM_CFG2_EFRT);
  }
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_MODPHY_PM_CFG2,
    Data32And,
    Data32Or
    );

  ///
  /// PM_CFG4 (0x0E8h[30])  = 1,    USB2 PHY SUS Well Power Gating Enable (U2_PHY_PG_EN)
  /// PM_CFG4 (0x0E8h[8:0]) = 018h, CPU I/O VR Ramp Duration
  ///
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_CFG4,
    (UINT32) ~B_PCH_PWRM_CFG4_CPU_IOVR_RAMP_DUR,
    (V_PCH_PWRM_CFG4_CPU_IOVR_RAMP_DUR_240US << N_PCH_PWRM_CFG4_CPU_IOVR_RAMP_DUR)
    );

  ///
  /// CPPM_CG_POL1A (0x0324h[30])  = 1,    CPPM Shutdown Qualifier Enable for Clock Source Group 1 (CPPM_G1_QUAL)
  /// CPPM_CG_POL2A (0x0340h[30])  = 1,    CPPM Shutdown Qualifier Enable for Clock Source Group 2 (CPPM_G2_QUAL)
  /// CPPM_CG_POL3A (0x03A8h[30])  = 1,    CPPM Shutdown Qualifier Enable for Clock Source Group 3 (CPPM_G3_QUAL)
  /// CPPM_CG_POL1A (0x0324h[8:0]) = 006h, LTR Threshold for Clock Source Group 1 (LTR_G1_THRESH)
  /// CPPM_CG_POL2A (0x0340h[8:0]) = 008h, LTR Threshold for Clock Source Group 2 (LTR_G2_THRESH)
  /// CPPM_CG_POL3A (0x03A8h[8:0]) = 029h, LTR Threshold for Clock Source Group 3 (LTR_G3_THRESH)
  ///
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_CPPM_CG_POL1A,
    (UINT32) ~B_PCH_PWRM_CPPM_CG_POLXA_LTR_GX_THRESH,
    B_PCH_PWRM_CPPM_CG_POLXA_CPPM_GX_QUAL |
    0x006
    );
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_CPPM_CG_POL2A,
    (UINT32) ~B_PCH_PWRM_CPPM_CG_POLXA_LTR_GX_THRESH,
    B_PCH_PWRM_CPPM_CG_POLXA_CPPM_GX_QUAL |
    0x008
    );
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_CPPM_CG_POL3A,
    (UINT32) ~B_PCH_PWRM_CPPM_CG_POLXA_LTR_GX_THRESH,
    B_PCH_PWRM_CPPM_CG_POLXA_CPPM_GX_QUAL |
    0x029
    );

  ///
  /// PWRMBASE + 0x314 = 0x1E0A4616
  ///
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_OBFF_CFG, 0x1E0A4616);

  ///
  /// PWRMBASE + 0x31C [8:0] = 0x00A
  /// For LP - PWRMBASE + 0x31C [29,27,25,24,23,22] = all 1s
  /// For H  - PWRMBASE + 0x31C [29,27,25,24,23,22,14] = all 1s
  /// Program  PWRMBASE + 0x31C[13] to 0b
  /// Program  PWRMBASE + 0x31C[15] to 1b if SLP_S0# is enabled.
  ///
  Data32And = (UINT32) ~(BIT15 | BIT13 | 0x000001FF);
  if (PchSeries == PchLp) {
    Data32Or  = BIT29 | BIT27 | BIT25 | BIT24 | BIT23 | BIT22         | 0x00A;
  } else {
    Data32Or  = BIT29 | BIT27 | BIT25 | BIT24 | BIT23 | BIT22 | BIT14 | 0x00A;
  }
  if (PchPolicy->PmConfig.SlpS0Enable) {
    Data32Or |= BIT15;
  }

  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_31C,
    Data32And,
    Data32Or
    );

  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG3, (UINT32) PchPolicy->PmConfig.GrPfetDurOnDef << N_PCH_PWRM_CFG3_GR_PFET_DUR_ON_DEF);

  ///
  /// PWRMBASE + 0x320 = 0x0005DB00
  /// Note: SKX Does not spupport PM_DMD message (s5331367)
  MmioWrite32 (PchPwrmBase + R_PCH_PWRM_CPPM_MISC_CFG, 0x0005DB00);

  Data32  = BIT31 | BIT30;
  if (PchSeries == PchH) {
    Data32 |= BIT21;
  }
  MmioOr32 (
    PchPwrmBase + R_PCH_PWRM_34C,
    Data32
    );
  ///
  /// PWRM + 0x100 [3] = 1
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_VR_MISC_CTL, B_PCH_PWRM_VR_MISC_CTL_VIDSOVEN);
  
  ///
  /// CS_SD_CTL1 (0x3E8[22:20]) = 010, Clock Source 5 Control Configuration (CS5_CTL_CFG)
  /// CS_SD_CTL1 (0x3E8[2:0])   = 001, Clock Source 1 Control Configuration (CS1_CTL_CFG)
  ///
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_CS_SD_CTL1,
    (UINT32) ~(B_PCH_PWRM_CS_SD_CTL1_CS5_CTL_CFG | B_PCH_PWRM_CS_SD_CTL1_CS1_CTL_CFG),
    (2 << N_PCH_PWRM_CS_SD_CTL1_CS5_CTL_CFG) |
    (1 << N_PCH_PWRM_CS_SD_CTL1_CS1_CTL_CFG)
    );

  ///
  /// PWRMBASE + 0x600 [31,30] = 1,1
  /// PWRMBASE + 0x600 [29] = 1
  /// PWRMBASE + 0x600 [10:6] = 02h
  ///
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_600,
    (UINT32) ~(0x1F << 6),
    BIT31 | BIT30 | BIT29 | (2 << 6)
    );

  ///
  /// PWRMBASE + 0x604 [10:6] = 02h
  ///
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_604,
    (UINT32) ~(0x1F << 6),
    (2 << 6)
    );

  ///
  /// PWRMBASE + 0x3D0 [31] = 1
  /// PWRMBASE + 0x3D0 [11,10] = all 0s
  /// PWRMBASE + 0x3D0 [21:16] = 0Ah
  ///
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_3D0,
    (UINT32) ~((0x3F << 16) | BIT11 | BIT10),
    BIT31 | (0x0A << 16)
    );

  ///
  /// CPPM_MPG_POL1A (0x3E0h[30])  = 1,    CPPM Shutdown Qualifier Enable for ModPHY (CPPM_MODPHY_QUAL)
  /// CPPM_MPG_POL1A (0x3E0h[29])  = 1,    ASLT/PLT Selection for ModPHY (LT_MODPHY_SEL)
  /// CPPM_MPG_POL1A (0x3E0h[8:0]) = 069h, LTR Treshold for ModPHY (LTR_MODPHY_THRESH)
  ///
  MmioAndThenOr32 (
    PchPwrmBase + R_PCH_PWRM_CPPM_MPG_POL1A,
    (UINT32) ~B_PCH_PWRM_CPPM_MPG_POL1A_LTR_MODPHY_THRESH,
    B_PCH_PWRM_CPPM_MPG_POL1A_CPPM_MODPHY_QUAL |
    B_PCH_PWRM_CPPM_MPG_POL1A_LT_MODPHY_SEL |
    0x69
    );


  //
  // Configure MISC settings
  //
  ConfigureMiscPm (PchPolicy);

  //
  // Configure Energy Report
  //
  ConfigureEnergyReport (PchPolicy);

  //
  // Deep Sx Enabling
  //
  ProgramDeepSx (PchPolicy);

  ConfigurePsfPm ();


  ConfigureSpiPm ();


  ConfigureLpcPm (PchPolicy);

  return EFI_SUCCESS;
}

/**
  Get PMC Data BAR Address

  @param[in]   TempMemBaseAddr          Temporary memory base address for PMC device
  @param[in]   PmcBaseAddress           PMC Base Address
  @param[out]  PmcDataBar               PMC Data BAR Address

  @retval FALSE                         No cleanup temporary MMIO resource to PM_DATA_BAR required
  @retval TRUE                          Cleanup of temporary MMIO resource to PM_DATA_BAR required
**/
BOOLEAN
PchPmcGetDataBar (
  IN  UINT32                          TempMemBaseAddr,
  IN  UINTN                           PmcBaseAddress,
  OUT UINTN                           *PmcDataBar
  )
{
  BOOLEAN       ResetPmcBar;

  ResetPmcBar = FALSE;
  if ((MmioRead32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR) & B_PCH_PMC_PM_DATA_BAR) == 0x0 ||
    (MmioRead32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR) & B_PCH_PMC_PM_DATA_BAR) == B_PCH_PMC_PM_DATA_BAR) {
    ResetPmcBar = TRUE;
    //
    // Disable memory space decoding in command register
    //
    MmioAnd16 (PmcBaseAddress + PCI_COMMAND_OFFSET, (UINT16) ~(EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
    //
    // Program temporary memory base address
    //
    MmioWrite32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR, TempMemBaseAddr);
    //
    // Enable memory space decoding in command register
    //
    MmioOr16 (PmcBaseAddress + PCI_COMMAND_OFFSET, (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
  }
  *PmcDataBar = MmioRead32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR) & B_PCH_PMC_PM_DATA_BAR;

  return ResetPmcBar;
}

/**
  Reset PMC Data BAR
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcResetDataBar (
  IN  UINTN                           PmcBaseAddress
)
{
  //
  // Disable memory space decoding in command register
  //
  MmioAnd16 (PmcBaseAddress + PCI_COMMAND_OFFSET, (UINT16) ~(EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
  ///
  /// Clean up Temporary MMIO resource to PM_DATA_BAR
  ///
  MmioWrite32 (PmcBaseAddress + R_PCH_PMC_PM_DATA_BAR, 0);
}


/**
  Worker function that return the PMC XRAM Offset Data

  @retval PCH_PMC_XRAM_OFFSET_DATA        PMC XRAM Offset Data
**/
PCH_PMC_XRAM_OFFSET_DATA *
GetPchPmcXramOffsetDataPpi (
  VOID
  )
{
  EFI_PEI_PPI_DESCRIPTOR      *PchPmcXramOffsetDataPpiDesc;
  PCH_PMC_XRAM_OFFSET_DATA    *PchPmcXramOffsetData;
  EFI_STATUS                  Status;

  PchPmcXramOffsetDataPpiDesc = NULL;
  PchPmcXramOffsetData        = NULL;

  Status = PeiServicesLocatePpi (
    &gPchPmcXramOffsetDataPpiGuid,
    0,
    NULL,
    (VOID **) &PchPmcXramOffsetData
    );
  if (Status != EFI_SUCCESS) {
    //
    // Initialize PMC XRAM Offset Data
    //
    PchPmcXramOffsetData = AllocateZeroPool (sizeof (PCH_PMC_XRAM_OFFSET_DATA));
    ASSERT (PchPmcXramOffsetData != NULL);
    if (PchPmcXramOffsetData != NULL) {
      PchPmcXramOffsetData->PmcXramHsioCount = 0;
      PchPmcXramOffsetData->PmcXramAfeCount = 0;
      PchPmcXramOffsetData->PmcXramOffset = sizeof (PCH_SBI_HSIO_HDR_TBL);
    }
    PchPmcXramOffsetDataPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
    ASSERT (PchPmcXramOffsetDataPpiDesc != NULL);
    if (PchPmcXramOffsetDataPpiDesc != NULL) {
      PchPmcXramOffsetDataPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
      PchPmcXramOffsetDataPpiDesc->Guid = &gPchPmcXramOffsetDataPpiGuid;
      PchPmcXramOffsetDataPpiDesc->Ppi = PchPmcXramOffsetData;
      DEBUG ((DEBUG_INFO, "Installing PMC XRAM Data\n"));
      Status = PeiServicesInstallPpi (PchPmcXramOffsetDataPpiDesc);
      ASSERT_EFI_ERROR (Status);
    }
  }
  return PchPmcXramOffsetData;
}

/**
  Calculate PMC XRAM Header offset based on group data

  @param[in]  Pid                       Port ID
  @param[in]  RowOffset                 Rows of data already present
  @param[in]  EndTableMarkers           Total bytes required for end table marker
  @param[in]  Delay                     Number of delays introduced

  @retval Header Data
**/
UINT16
SurvPhyGrpTblEntry (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT8                             RowOffset,
  IN  UINT8                             EndTableMarkers,
  IN  UINT8                             Delay
  )
{
  UINT8 Dwords;
  if ((Pid == 0x00) && (RowOffset == 0x0F)) {
    return (UINT16) 0x0F00;
  } else {
    DEBUG ((DEBUG_INFO, "Pid = %d\n", Pid));
    DEBUG ((DEBUG_INFO, "RowOffset = %d\n", RowOffset));
    DEBUG ((DEBUG_INFO, "EndTableMarkers = %d\n", EndTableMarkers));
    DEBUG ((DEBUG_INFO, "Delay = %d\n", Delay));
    Dwords = (RowOffset * 3) + EndTableMarkers + Delay;
    DEBUG ((DEBUG_INFO, "Dwords = %d\n", Dwords));
    return (UINT16) (((Dwords & 0x0F) << 12) | ((Pid & 0x0F) << 8) | (Dwords >> 4));
  }
}

/**
  Write Pmc Xram data.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.
  @param[in]  TempMemBaseAddr           Temporary memory base address for PMC device
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcXramWrite32 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            InData,
  IN  UINT32                            TempMemBaseAddr,
  IN  UINTN                             PmcBaseAddress
)
{
  BOOLEAN                     ResetPmcBar;
  PCH_SBI_HSIO_CMD_TBL        PchHsioCmdTblEntry;
  PCH_DELAY_HSIO_CMD_TBL      PchDelayTblEntry;
  UINTN                       PmcDataBar;
  UINTN                       XramOffset;
  PCH_PMC_XRAM_OFFSET_DATA    *PchPmcXramOffsetData;

  if (GetPchSeries () == PchLp) {
    ResetPmcBar = PchPmcGetDataBar (TempMemBaseAddr, PmcBaseAddress, &PmcDataBar);
    SetMem (&PchHsioCmdTblEntry, sizeof (PCH_SBI_HSIO_CMD_TBL), 0x00);
    PchPmcXramOffsetData = GetPchPmcXramOffsetDataPpi ();
    if (PchPmcXramOffsetData == NULL) {
      return;
    }
    if (Pid == PID_USB2) {
      if (PchPmcXramOffsetData->PmcXramAfeCount == 0) {
        XramOffset = PchPmcXramOffsetData->PmcXramOffset + PmcDataBar;
        //Add delay of 700us before USB2 Restore Table Execution
        SetMem (&PchDelayTblEntry, sizeof (PCH_DELAY_HSIO_CMD_TBL), 0x00);
        PchDelayTblEntry.Command = DoDelay;
        PchDelayTblEntry.Size = PMC_DATA_DELAY_CMD_SIZE;
        PchDelayTblEntry.DelayPeriod = Delay100us;
        PchDelayTblEntry.DelayCount = 7;
        PchDelayTblEntry.Padding = 0;
        CopyMem ((EFI_PHYSICAL_ADDRESS *) XramOffset, &PchDelayTblEntry, sizeof (PCH_DELAY_HSIO_CMD_TBL));
        DEBUG ((DEBUG_INFO, "HSIO : XramOffset = %x, Value0-3 = %x\n", XramOffset, MmioRead32 (XramOffset)));
        PchPmcXramOffsetData->PmcXramOffset += sizeof (PCH_DELAY_HSIO_CMD_TBL);
      }
      PchPmcXramOffsetData->PmcXramAfeCount++;
    } else {
      PchPmcXramOffsetData->PmcXramHsioCount++;
    }
    PchHsioCmdTblEntry.Command = SendSBIPosted;
    PchHsioCmdTblEntry.Size = PMC_DATA_SBI_CMD_SIZE;
    PchHsioCmdTblEntry.Pid = Pid;
    PchHsioCmdTblEntry.Bar = 0;
    PchHsioCmdTblEntry.OpCode = PrivateControlWrite;
    PchHsioCmdTblEntry.Fbe = 0x0F;
    PchHsioCmdTblEntry.Fid = 0x00;
    PchHsioCmdTblEntry.Offset = Offset;
    PchHsioCmdTblEntry.Value = InData;
    XramOffset = PchPmcXramOffsetData->PmcXramOffset + PmcDataBar;
    CopyMem ((EFI_PHYSICAL_ADDRESS *) XramOffset, &PchHsioCmdTblEntry, sizeof (PCH_SBI_HSIO_CMD_TBL));
    DEBUG ((DEBUG_INFO, "HSIO : XramOffset = %x, Value0-3 = %x, Value4-7 = %x, Value8-11 = %x\n", XramOffset, MmioRead32 (XramOffset), MmioRead32 (XramOffset + 4), MmioRead32 (XramOffset + 8)));
    PchPmcXramOffsetData->PmcXramOffset += sizeof (PCH_SBI_HSIO_CMD_TBL);
    if (ResetPmcBar == TRUE) {
      PchPmcResetDataBar (PmcBaseAddress);
    }
  }
}

/**
  Write PMC XRAM End Table Marker

  @param[in]  TempMemBaseAddr           Temporary memory base address for PMC device
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcXramEndTableMarker (
  IN  UINT32          TempMemBaseAddr,
  IN  UINTN           PmcBaseAddress
)
{
  BOOLEAN                     ResetPmcBar;
  UINTN                       PmcDataBar;
  UINTN                       XramOffset;
  PCH_PMC_XRAM_OFFSET_DATA    *PchPmcXramOffsetData;
  UINT32                      Data32;

  if (GetPchSeries () == PchLp) {
    ResetPmcBar = PchPmcGetDataBar (TempMemBaseAddr, PmcBaseAddress, &PmcDataBar);
    PchPmcXramOffsetData = GetPchPmcXramOffsetDataPpi ();
    if (PchPmcXramOffsetData == NULL) {
      return;
    }
    XramOffset = PchPmcXramOffsetData->PmcXramOffset + PmcDataBar;
    Data32 = 0x0F;
    CopyMem ((EFI_PHYSICAL_ADDRESS *) XramOffset, &Data32, sizeof (UINT32));
    PchPmcXramOffsetData->PmcXramOffset += 4;
    if (ResetPmcBar == TRUE){
      PchPmcResetDataBar (PmcBaseAddress);
    }
  }
}

/**
  Program PMC XRAM Data Header

  @param[in]  TempMemBaseAddr           Temporary memory base address for PMC device
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcXramDataHdrProg (
  IN  UINT32                            TempMemBaseAddr,
  IN  UINTN                             PmcBaseAddress
)
{
  BOOLEAN                     ResetPmcBar;
  PCH_PMC_XRAM_OFFSET_DATA    *PchPmcXramOffsetData;
  PCH_SBI_HSIO_HDR_TBL        PchHsioHdrTbl;
  UINTN                       PmcDataBar;
  UINT16                      Data16;
  UINT8                       TotalRows;
  UINT8                       CurrentRowOffset;
  UINT8                       Delay;

  if (GetPchSeries () == PchLp) {
    PchPmcXramOffsetData = GetPchPmcXramOffsetDataPpi ();
    if (PchPmcXramOffsetData == NULL) {
      return;
    }
    ResetPmcBar = PchPmcGetDataBar (TempMemBaseAddr, PmcBaseAddress, &PmcDataBar);
    SetMem (&PchHsioHdrTbl, sizeof (PCH_SBI_HSIO_HDR_TBL), 0x00);
    TotalRows = 16;
    CurrentRowOffset = 0;
    Delay = 0;
    if (PchPmcXramOffsetData->PmcXramAfeCount != 0){
      Data16 = SurvPhyGrpTblEntry (1, CurrentRowOffset, 16 - TotalRows, Delay);
      CopyMem ((EFI_PHYSICAL_ADDRESS *) (PmcDataBar + (16 - TotalRows) * sizeof (UINT16)), &Data16, sizeof (UINT16));
      CurrentRowOffset += PchPmcXramOffsetData->PmcXramAfeCount;
      TotalRows--;
      Delay++;
      DEBUG ((DEBUG_INFO, "Total AFE settings in PMC : %d\n", PchPmcXramOffsetData->PmcXramAfeCount));
    }
    if (PchPmcXramOffsetData->PmcXramHsioCount != 0){
      Data16 = SurvPhyGrpTblEntry (3, CurrentRowOffset, 16 - TotalRows, Delay);
      CopyMem ((EFI_PHYSICAL_ADDRESS *) (PmcDataBar + (16 - TotalRows) * sizeof (UINT16)), &Data16, sizeof (UINT16));
      CurrentRowOffset += PchPmcXramOffsetData->PmcXramHsioCount;
      TotalRows--;
      DEBUG ((DEBUG_INFO, "Total HSIO settings in PMC : %d\n", PchPmcXramOffsetData->PmcXramHsioCount));
    }
    Data16 = SurvPhyGrpTblEntry (0x00, 0x0F, 0x00, Delay);
    CopyMem ((EFI_PHYSICAL_ADDRESS *) (PmcDataBar + (16 - TotalRows) * sizeof (UINT16)), &Data16, sizeof (UINT16));
    if (ResetPmcBar == TRUE){
      PchPmcResetDataBar (PmcBaseAddress);
    }
    ///
    /// Program the PM_DATA_BAR_DIS, PMC PCI offset A4h bit[16] to 1
    ///
    MmioOr8 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B + 2, B_PCH_PMC_GEN_PMCON_B_PM_DATA_BAR_DIS >> 16);
  }
}

/**
  Lock down PMC settings

  @param[in] PchPolicy                  The PCH Policy PPI instance
**/
VOID
LockPmcSettings (
  IN  PCH_POLICY_PPI                    *PchPolicy
  )
{
  UINTN                                 PciPmcRegBase;

  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );
  ///
  /// Set PMC PCI Offset 0xAC [24]
  ///
  MmioOr32 (PciPmcRegBase + R_PCH_PMC_ETR3, BIT24);
}
