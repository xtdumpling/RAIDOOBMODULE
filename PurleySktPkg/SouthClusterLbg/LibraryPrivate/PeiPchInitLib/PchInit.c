/** @file
  The PCH Init After Memory PEI module.

@copyright
  Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#include <HeciRegs.h>
#include <MeChipset.h>
#include "Wdt.h"
#include "PchSpi.h"
#include <IncludePrivate/Library/PchSmmControlLib.h>
#include <Library/PchSerialIoLib.h>

/**
  Initialize IOAPIC according to IoApicConfig policy of the PCH
  Policy PPI

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchIoApicInit (
  IN  PCH_POLICY_PPI     *PchPolicyPpi
  )
{
  UINT16                RegData16;
  UINTN                 P2sbBase;
  PCH_IOAPIC_CONFIG     *IoApicConfig;
  UINT32                IoApicAddress;
  UINT32                IoApicId;
  UINT32                McastTarget;

  ///
  /// Program PSF Multicast registers accordingly to SKL PCH BWG 5.14.4 PSF EOI Multicast Configuration
  ///
  if (GetPchSeries () == PchLp) {
    McastTarget = V_PCH_LP_PCR_PSFX_PSF_MC_AGENT_MCAST_TGT_P2SB;
    ///
    /// Set PCR[PSF1] + 4060h = 38A00h
    ///
    PchPcrWrite32 (
      PID_PSF1, R_PCH_LP_PCR_PSF1_PSF_MC_AGENT_MCAST0_TGT0_EOI,
      McastTarget
      );
    ///
    /// Set PCR[PSF1] + 4050h[7:0] = 11b
    ///
    PchPcrWrite32 (
      PID_PSF1, R_PCH_LP_PCR_PSF1_PSF_MC_CONTROL_MCAST0_EOI,
      0x3
      );
  } else {
    McastTarget = V_PCH_H_PCR_PSFX_PSF_MC_AGENT_MCAST_TGT_P2SB;
    ///
    /// Set PCR[PSF1] + 4058h = 38B00h
    ///
    PchPcrWrite32 (
      PID_PSF1, R_PCH_H_PCR_PSF1_PSF_MC_AGENT_MCAST0_TGT0_EOI,
      McastTarget
      );
    ///
    /// Set PCR[PSF1] + 4048h[7:0] = 11b
    ///
    PchPcrWrite32 (
      PID_PSF1, R_PCH_H_PCR_PSF1_PSF_MC_CONTROL_MCAST0_EOI,
      0x3
      );
  }
  ///
  /// Set PCR[PSF1] + 4054h to P2SB target
  ///
  PchPcrWrite32 (
    PID_PSF3, R_PCH_PCR_PSF3_PSF_MC_AGENT_MCAST0_TGT0_EOI,
    McastTarget
    );
  ///
  /// Set PCR[PSF3] + 404Ch[7:0] = 11b
  ///
  PchPcrWrite32 (
    PID_PSF3, R_PCH_PCR_PSF3_PSF_MC_CONTROL_MCAST0_EOI,
    0x3
    );

  //
  // Configure ITSS power management settings
  //
  ///
  /// Set PCR[ITSS] + 3300h[4:0] = 11011b
  ///
  PchPcrAndThenOr32 (
    PID_ITSS, R_PCH_PCR_ITSS_ITSSPRC,
    ~(UINT32)(0),
    (UINT32)(B_PCH_PCR_ITSS_ITSSPRC_PGCBDCGE | B_PCH_PCR_ITSS_ITSSPRC_HPETDCGE | B_PCH_PCR_ITSS_ITSSPRC_IOSFICGE | B_PCH_PCR_ITSS_ITSSPRC_ITSSCGE)
    );

  //
  // Get P2SB base address
  //
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );
  IoApicConfig    = &PchPolicyPpi->IoApicConfig;

  if (IoApicConfig->ApicRangeSelect != (UINT8)(MmioRead16 (P2sbBase + R_PCH_P2SB_IOAC) & B_PCH_P2SB_IOAC_ASEL)) {
    ///
    /// Program APIC Range Select bits that define address bits 19:12 for the IOxAPIC range.
    /// This value must not be changed unless the IOxAPIC Enable bit is cleared.
    ///
    MmioAnd16 (P2sbBase + R_PCH_P2SB_IOAC, (UINT16)~(B_PCH_P2SB_IOAC_AE));
    ///
    /// Program APIC Range Select bits at P2SB PCI offset 64h[7:0]
    ///
    MmioAndThenOr16 (
      P2sbBase + R_PCH_P2SB_IOAC,
      (UINT16)~(B_PCH_P2SB_IOAC_ASEL),
      (UINT16) IoApicConfig->ApicRangeSelect
      );
  }
  ///
  /// PCH BIOS Spec Section 6.6.2.1
  /// 1. Enable the IOAPIC by setting the APIC Enable bit, P2SB PCI offset 64h [8], if the
  ///    system needs to use the IOxAPIC. The APIC Enable bits needs read back after the bit is written.
  ///
  MmioOr16 (P2sbBase + R_PCH_P2SB_IOAC, B_PCH_P2SB_IOAC_AE);
  ///
  /// Reads back for posted write to take effect
  ///
  MmioRead16 (P2sbBase + R_PCH_P2SB_IOAC);

  ///
  /// Get current IO APIC ID
  ///
  IoApicAddress = (UINT32) (MmioRead8 (P2sbBase + R_PCH_P2SB_IOAC) << N_PCH_IO_APIC_ASEL);
  MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 0);
  IoApicId = MmioRead32 ((UINTN) (R_PCH_IO_APIC_DATA | IoApicAddress)) >> 24;
  ///
  /// IO APIC ID is at APIC Identification Register [27:24]
  ///
  if ((IoApicConfig->IoApicId != IoApicId) && (IoApicConfig->IoApicId < 0x10)) {
    ///
    /// Program APIC ID
    ///
    MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 0);
    MmioWrite32 ((UINTN) (R_PCH_IO_APIC_DATA | IoApicAddress), (UINT32) (IoApicConfig->IoApicId << 24));
  }

  if (IoApicConfig->IoApicEntry24_119 == FALSE) {
    ///
    /// Program IOAPIC maximal entry to 24 in PCR[ITSS] + 31FCh[9]
    /// and in IOAPIC Version Register
    PchPcrAndThenOr16 (PID_ITSS, R_PCH_PCR_ITSS_GIC, 0xFFFF, (UINT16) B_PCH_PCR_ITSS_GIC_MAX_IRQ_24);
    MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 1);
    MmioWrite32 ((R_PCH_IO_APIC_DATA | IoApicAddress), 0x170020);
  } else {
    ///
    /// Program IOAPIC maximal entry to 119 in PCR[ITSS] + 31FCh[9]
    /// and in IOAPIC Version Register
    PchPcrAndThenOr16 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT16)(~B_PCH_PCR_ITSS_GIC_MAX_IRQ_24), 0x0);
    MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), 1);
    MmioWrite32 ((R_PCH_IO_APIC_DATA | IoApicAddress), 0x770020);
  }

  ///
  /// Program this field to provide a unique bus:device:function number for the internal IOxAPIC
  ///
  if (IoApicConfig->BdfValid) {
    RegData16 = ((UINT16) (IoApicConfig->BusNumber) << 8) & B_PCH_P2SB_IBDF_BUF;
    RegData16 |= ((UINT16) (IoApicConfig->DeviceNumber) << 3) & B_PCH_P2SB_IBDF_DEV;
    RegData16 |= (UINT16) (IoApicConfig->FunctionNumber) & B_PCH_P2SB_IBDF_FUNC;
    MmioWrite16 ((UINTN) (P2sbBase + R_PCH_P2SB_IBDF), RegData16);
  }

  //
  // Build the resource descriptor hob for IOAPIC address resource.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    R_PCH_IO_APIC_INDEX | IoApicAddress,
    0x1000
    );
  BuildMemoryAllocationHob (
    R_PCH_IO_APIC_INDEX | IoApicAddress,
    0x1000,
    EfiMemoryMappedIO
    );

  return EFI_SUCCESS;
}

/**
  The function performs ModPHY specific programming.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance

  @retval EFI_SUCCESS                   The ModPHY required settings programmed correctly
**/
EFI_STATUS
EFIAPI
PchHsioProg (
  IN  PCH_POLICY_PPI           *PchPolicy
  )
{
  return EFI_SUCCESS;
}

/**
  The function performs P2SB specific programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval EFI_SUCCESS           The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchP2sbConfigure (
  IN  PCH_POLICY_PPI           *PchPolicy
  )
{
  UINTN                                 P2sbBase;
  //
  // Get P2SB base address
  //
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );
  //
  // P2SB power management settings.
  //
  ///
  /// Set PGCB clock gating enable (PGCBCGE)
  /// P2SB PCI offset 0xE0[16] = 1
  ///
  MmioOr8 (P2sbBase + R_PCH_P2SB_E0 + 2, BIT0);
  ///
  /// For LBG can't set Hardware Autonomous Enable (HAE) and PMC Power Gating Enable (PMCPG_EN)
  /// P2SB PCI offset 0xE4[5] = 0 and 0xE4[0] = 0. Bits 0xE4[2,1] are RO
  ///
  MmioAnd32 (P2sbBase + R_PCH_P2SB_PCE,  (UINT32)~(R_PCH_P2SB_PCE_HAE | R_PCH_P2SB_PCE_PMCRE));


  return EFI_SUCCESS;
}

/**
  The function performs PSTH specific programming.

  @param[in] PchPolicy          The PCH Policy instance

  @retval EFI_SUCCESS                   The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchPsthConfigure (
  IN  PCH_POLICY_PPI           *PchPolicy
  )
{
  //
  // PSTH power management settings.
  //

  return EFI_SUCCESS;
}

/**
  The function performs GPIO Power Management programming.

  @param[in] PchPolicyPpi    The PCH Policy PPI instance

  @retval EFI_SUCCESS                The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchGpioConfigurePm (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{


  //
  // Program MISCCFG.GPDLCGEn and MISCCFG.GPDPCGEn for Community 0
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM0, R_PCH_PCR_GPIO_MISCCFG),
    ~(UINT32)(0),
    (UINT32)(B_PCH_PCR_GPIO_MISCCFG_GPDLCGEN | B_PCH_PCR_GPIO_MISCCFG_GPDPCGEN)
    );

  //
  // Program MISCCFG.GPDLCGEn and MISCCFG.GPDPCGEn for Community 1
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM1, R_PCH_PCR_GPIO_MISCCFG),
    ~(UINT32)(0),
    (UINT32)(B_PCH_PCR_GPIO_MISCCFG_GPDLCGEN | B_PCH_PCR_GPIO_MISCCFG_GPDPCGEN)
    );

  //
  // Program MISCCFG.GPDLCGEn and MISCCFG.GPDPCGEn for Community 2
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM2, R_PCH_PCR_GPIO_MISCCFG),
    ~(UINT32)(0),
    (UINT32)(B_PCH_PCR_GPIO_MISCCFG_GPDLCGEN | B_PCH_PCR_GPIO_MISCCFG_GPDPCGEN)
    );

  //
  // Program MISCCFG.GPDLCGEn and MISCCFG.GPDPCGEn for Community 3
  //
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM3, R_PCH_PCR_GPIO_MISCCFG),
    ~(UINT32)(0),
    (UINT32)(B_PCH_PCR_GPIO_MISCCFG_GPDLCGEN | B_PCH_PCR_GPIO_MISCCFG_GPDPCGEN)
    );

  return EFI_SUCCESS;
}

/**
  The function performs RTC specific programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval EFI_SUCCESS           The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchRtcConfigure (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{


  ///
  /// Set PCR[RTC] + 3418h[1:0] = 11b
  ///
  PchPcrAndThenOr32 (
    PID_RTC, R_PCH_PCR_RTC_RTCDCG,
    ~(UINT32)(0),
    (UINT32)(R_PCH_PCR_RTC_RTCDCG_RTCPCICLKDCGEN | R_PCH_PCR_RTC_RTCDCG_RTCROSIDEDCGEN)
    );

  ///
  /// Set PCR[RTC] + 3F00h[8] = 1b
  ///
  PchPcrAndThenOr32 (
    PID_RTC, R_PCH_PCR_RTC_3F00,
    ~(UINT32)(0),
    (UINT32)(BIT8)
    );

  //
  // Clear RTC SMI enable and status.
  //
  PchPcrWrite32 (PID_RTC, R_PCH_PCR_RTC_UIPSMI, 0x00030000);

  return EFI_SUCCESS;
}

/**
  The function performs SMBUS specific programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval EFI_SUCCESS           The required settings programmed successfully
**/
EFI_STATUS
EFIAPI
PchSmbusConfigure (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  )
{
  UINTN     SmbusBaseAddress;
  UINT32    Data32And;
  UINT32    Data32Or;

  SmbusBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_SMBUS,
                       PCI_FUNCTION_NUMBER_PCH_SMBUS
                       );

  ///
  /// Enable SMBus dynamic clock gating by setting Smbus PCI offset 80h [16,14,12,10,8] = 0b and [5] = 1b respectively
  /// Disable SMBus dynamic clock gating of PGCB by [18] = 1
  ///
  Data32And = (UINT32)~(BIT16 | BIT14 | BIT12 | BIT10 | BIT8);
  Data32Or  = BIT18 | BIT5;
  if (PchPolicyPpi->SmbusConfig.DynamicPowerGating) {
    Data32Or &= (UINT32)~BIT18;
  }
  MmioAndThenOr32 (
    SmbusBaseAddress + R_PCH_SMBUS_80,
    Data32And,
    Data32Or
    );

  ///
  /// For LBG HW PG is not supported
  /// SMBUS PCR 0x10h [5] = 0 and clear [0] = 0, bits [2,1] are RO
  ///
  PchPcrAndThenOr32 (
    PID_SMB, R_PCH_PCR_SMBUS_PCE,
    ~(UINT32)(B_PCH_PCR_SMBUS_PCE_HAE | BIT0),
    (UINT32) 0
    );


  return EFI_SUCCESS;
}

/**
  The function performs Serial IRQ specific programming.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
**/
VOID
EFIAPI
PchConfigureSerialIrq (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  UINTN                                 PciPmcRegBase;
  UINTN                                 PciLpcRegBase;
  UINT8                                 RegData8;

  if (PchPolicyPpi->SerialIrqConfig.SirqEnable == FALSE) {
    return;
  }

  PciLpcRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC);
  PciPmcRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC);
  RegData8        = 0;

  ///
  /// PCH BIOS Spec Section 6.3 Serial IRQs
  /// The only System BIOS requirement to use IRQs as a serial IRQ is to enable the function in LPC PCI offset 64h[7] and
  /// select continuous or quiet mode, LPC PCI offset 64h[6].
  /// PCH requires that the System BIOS first set the SERIRQ logic to continuous mode operation for at least one frame
  /// before switching it into quiet mode operation. This operation should be performed during the normal boot sequence
  /// as well as a resume from STR (S3).
  ///
  RegData8  = MmioRead8 (PciLpcRegBase + R_PCH_LPC_SERIRQ_CNT);
  RegData8  &= (UINT8) ~(B_PCH_LPC_SERIRQ_CNT_SIRQEN | B_PCH_LPC_SERIRQ_CNT_SFPW);

  switch (PchPolicyPpi->SerialIrqConfig.StartFramePulse) {
  case PchSfpw8Clk:
    RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_8CLK;
    break;

  case PchSfpw6Clk:
    RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_6CLK;
    break;

  case PchSfpw4Clk:
  default:
    RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_4CLK;
    break;
  }
  ///
  /// Set the SERIRQ logic to continuous mode
  ///
  RegData8 |= (UINT8) (B_PCH_LPC_SERIRQ_CNT_SIRQEN | B_PCH_LPC_SERIRQ_CNT_SIRQMD);

  MmioWrite8 (PciLpcRegBase + R_PCH_LPC_SERIRQ_CNT, RegData8);

  ///
  /// PCH BIOS Spec Section 6.3 Serial IRQs
  /// The only System BIOS requirement to use IRQs as a serial IRQ is to enable the function
  /// in LPC PCI offset 64h[7] and select continuous or quiet mode, LPC PCI offset 64h[6].
  ///
  if (PchPolicyPpi->SerialIrqConfig.SirqMode == PchQuietMode) {
    MmioAnd8 (PciLpcRegBase + R_PCH_LPC_SERIRQ_CNT, (UINT8)~B_PCH_LPC_SERIRQ_CNT_SIRQMD);
  }
}

/**
  Clear RTC PWR STS.
**/
VOID
ClearRtcPwrSts (
  VOID
  )
{
  UINTN                                 PciPmcRegBase;

  PciPmcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );
  ///
  /// PCH BIOS Spec Section 19.8.1 RTC Resets
  /// The PCH will set the RTC_PWR_STS bit (PMC PCI offset A4h[2]) when the RTCRST# pin goes low.
  /// The System BIOS shouldn't rely on the RTC RAM contents when the RTC_PWR_STS bit is set.
  /// BIOS should clear this bit by writing a 0 to this bit position.
  /// This bit isn't cleared by any reset function.
  ///
  //
  // Set B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS to 0 to clear it.
  // Don't clear B_PCH_PMC_GEN_PMCON_B_PWR_FLR by accident, since it's RW/1C.
  //
  MmioAnd8 ((UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B), (UINT8) ~(B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS | B_PCH_PMC_GEN_PMCON_B_PWR_FLR));
}

/**
  Pch End of PEI callback function. This is the last event before entering DXE and OS in S3 resume.
**/
VOID
PchOnEndOfPei (
  VOID
  )
{
  EFI_STATUS      Status;
  UINTN           P2sbBase;
  PCH_POLICY_PPI  *PchPolicy;
  PCH_SERIES      PchSeries;
  PCH_STEPPING    PchStep;
  EFI_BOOT_MODE   BootMode;

  DEBUG ((DEBUG_INFO, "PchOnEndOfPei after memory PEI module - Start\n"));

  PchSeries = GetPchSeries ();
  PchStep   = PchStepping ();


  //
  // Get Policy settings through the PchPolicy PPI
  //
  Status = PeiServicesLocatePpi (
             &gPchPlatformPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &PchPolicy
             );
  if (Status != EFI_SUCCESS) {
    //
    // PCH_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
    return;
  }

  //
  // Clear RTC PWR STS in end of PEI so others won't miss the status
  //
  ClearRtcPwrSts ();


  PchSetLinkWidth (PchPolicy);

  //
  // Init thermal device and lock down settings.
  //
  Status = PchThermalInit (PchPolicy);

  //
  // Init Memory Throttling.
  // Memory Throttling doesn't depend on PCH thermal.
  //
  PchMemoryThrottlingInit (PchPolicy);

  //
  // SKX PCH: Programming P2SB masks for certain Port IDs to be unaccessible from Host
  // This is step is done in ProcessAllLocks() on EndOfDxe event
  //

  WdtEndOfPeiCallback ();

  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );

  Status = PchTraceHubOnEndOfPei (PchPolicy);

#ifdef FSP_FLAG
  //
  // Configure root port function number mapping
  // For FSP, execute RPFN mapping here before PCI enumeration.
  //
  Status = PchConfigureRpfnMapping ();
  ASSERT_EFI_ERROR (Status);
#endif

  //
  // Lock PMC settings
  //
  LockPmcSettings (PchPolicy);

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // For S3 path:
  //   If it's PCH A0 stepping, skip the P2SB SBI lock and Hidden.
  //   else, lock SBI, hide P2SB and do P2SB PSF configuration space disable.
  // For S4/S5 path:
  //   Hide P2SB only.
  //   If FSP is supported, also lock SBI and do P2SB PSF configuration space disable.
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
    if (PchPolicy->LockDownConfig.GpioLockDown == TRUE) {
      Status = GpioLockGpios ();
      if (Status != EFI_SUCCESS) {
        DEBUG ((DEBUG_ERROR, "PchOnEndOfPei: GpioLockGpios() failed!\n"));
      }
    }
    if ((PchSeries == PchH ) && (PchStep == PchHA0 )) { 
    /// 
    /// Skip Hiding P2sb for PCH LP A0 and PCH H A0 
    /// 
    DEBUG ((DEBUG_INFO, "PchOnEndOfPei skips hiding P2sb controller for PCH H A0\n")); 
    } else { 
    ///
    /// Lock down the P2sb SBI before going into OS.
    /// This only apply to PCH B0 onward.
    ///
    ConfigureP2sbSbiLock (&PchPolicy->P2sbConfig);
    ///
    /// Hide P2SB controller in the end of PEI.
    ///
    PchHideP2sb (P2sbBase);
    }
  } else {
    ///
    /// Hide P2SB controller in the end of PEI.
    ///
    PchHideP2sb (P2sbBase);
    ///
    /// WA: Unset P2SB PCI Offset 0xE0[31].
    ///
  }

  if (BootMode == BOOT_ON_S3_RESUME) {
    RemovePsfAccess (&PchPolicy->P2sbConfig);
  }

  DEBUG ((DEBUG_INFO, "PchOnEndOfPei after memory PEI module - End\n"));
}

/**
  Pch init after memory PEI module

  @param[in] SiPolicy     The Silicon Policy PPI instance
  @param[in] PchPolicy    The PCH Policy PPI instance

  @retval None
**/
VOID
EFIAPI
PchInit (
  IN  PCH_POLICY_PPI *PchPolicy
  )
{
  EFI_STATUS                            Status;
  PCH_PCIE_DEVICE_OVERRIDE              *PcieDeviceTable;
  VOID                                  *HobPtr;
  UINTN                                 Count;
  PCH_SERIES                            PchSeries;
#ifdef FSP_FLAG
  BOOLEAN                               RstPcieStorageRemapEnabled;
#endif

  DEBUG ((DEBUG_INFO, "PchInit - Start\n"));

  PchSeries = GetPchSeries ();

  //
  // Install PEI SMM Control PPI
  //
  Status = PchSmmControlInit ();

  //
  // Locate and PCIe device override table and add to HOB area.
  //
  Status = PeiServicesLocatePpi (
             &gPchPcieDeviceTablePpiGuid,
             0,
             NULL,
             (VOID **)&PcieDeviceTable
             );

  if (Status == EFI_SUCCESS) {
    for (Count = 0; PcieDeviceTable[Count].DeviceId != 0; Count++) {
    }
    DEBUG ((DEBUG_INFO, "PCH Installing PcieDeviceTable HOB (%d entries)\n", Count));
    HobPtr = BuildGuidDataHob (
               &gPchDeviceTableHobGuid,
               PcieDeviceTable,
               Count * sizeof (PCH_PCIE_DEVICE_OVERRIDE)
               );
    ASSERT (HobPtr != 0);
  }

  HobPtr = BuildGuidDataHob (&gPchPolicyHobGuid, PchPolicy, sizeof (PCH_POLICY_PPI));
  ASSERT (HobPtr != 0);

  //
  // Check if ME has the right HSIO Settings and sync with ME if required
  //
  Status = PchHsioChipsetInitProg (TRUE);
  ASSERT_EFI_ERROR (Status);

#ifdef FSP_FLAG
  RstPcieStorageRemapEnabled  = IsRstPcieStorageRemapEnabled (&PchPolicy->SataConfig);
  if (PchSeries == PchH) {
    Status = EarlyConfigurePchHSata (
               &PchPolicy->SataConfig,
               RstPcieStorageRemapEnabled,
               SiPolicy->TempMemBaseAddr
               );
    ConfigurePchHSata (
               &PchPolicy->SataConfig,
               SiPolicy->TempMemBaseAddr
               );
  } else {
    Status = EarlyConfigurePchLpSata (
               &PchPolicy->SataConfig,
               RstPcieStorageRemapEnabled,
               SiPolicy->TempMemBaseAddr
               );
    ConfigurePchLpSata (
               &PchPolicy->SataConfig,
               SiPolicy->TempMemBaseAddr
               );
  }
  // Tune the USB 2.0 high-speed signals quality.
  // This is done before USB Initialization because the PMC XRAM has
  // to be filled up with USB2 AFE settings followed by HSIO settings
  // before it is locked
  //
  Usb2AfeProgramming (&(PchPolicy->UsbConfig), SiPolicy->TempMemBaseAddr);

  //
  // Configure USB3 ModPHY turning.
  //
  XhciUsb3Tune (&(PchPolicy->UsbConfig));

  //
  // Configure PCH xHCI, pre-mem phase
  //
  Status = ConfigureXhciPreMem (PchPolicy, SiPolicy->TempMemBaseAddr);

  //
  // Configure PCH OTG (xDCI), pre-mem phase
  //
  Status = ConfigureXdci (PchPolicy, SiPolicy->TempMemBaseAddr);

  //
  // Init Serial IO
  //
  ConfigureSerialIoPei (PchPolicy);
#endif // FSP_FLAG

  //
  // Configure DMI
  //
  PchDmiAndOpDmiInit (PchPolicy);
  //
  // Configure P2SB
  //
  Status = PchP2sbConfigure (PchPolicy);
  ASSERT_EFI_ERROR (Status);
  //
  // Configure PSTH
  //
  Status = PchPsthConfigure (PchPolicy);
  ASSERT_EFI_ERROR (Status);
  //
  // Configure IOAPIC
  //
  Status = PchIoApicInit (PchPolicy);
  ASSERT_EFI_ERROR (Status);
  //
  // Configure interrupts.
  //
  Status = PchConfigureInterrupts (PchPolicy);
  ASSERT_EFI_ERROR (Status);

  //
  // Configure PCH xHCI, post-mem phase
  //
  Status = ConfigureXhci (PchPolicy);
  //
  // Initialize HD-Audio
  //
  Status = PchHdAudioInit (PchPolicy);


  //
  // Configure GPIO PM settings
  //
  Status = PchGpioConfigurePm (PchPolicy);

  //
  // Configure RTC
  //
  Status = PchRtcConfigure (PchPolicy);

  //
  // Configure SMBUS
  //
  Status = PchSmbusConfigure (PchPolicy);

  //
  // Configure PM settings
  //
  Status = PchPmInit (PchPolicy);
  
  //
  // Configure Serial IRQ
  //
  PchConfigureSerialIrq (PchPolicy);

  //
  // Configure SPI after memory
  //
  ConfigureSpiAfterMem (PchPolicy);
  
  //
  // Finalize CIO2 configuration: power off CSI2 HC core logic and lock registers
  //
  if (PchSeries == PchLp) {
  } else {
    // there is no Cio2 in PchH
    ConfigurePmcWhenCio2Disabled (TRUE);
  }  

  DEBUG ((DEBUG_INFO, "PchInit - End\n"));
}
