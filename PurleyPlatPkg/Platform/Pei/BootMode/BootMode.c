/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file BootMode.c

  EFI PEIM to determine the bootmode of the platform

**/
#include "BootMode.h"
#include <IncludePrivate/Library/PchAlternateAccessModeLib.h>

EFI_PEI_PPI_DESCRIPTOR  mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

/**
  Parse the status registers for figuring out the wake-up event.

  @param[out]  WakeUpType       Updates the wakeuptype based on the status registers
**/
VOID
GetWakeupEvent (
  OUT  UINT8   *WakeUpType
  )
{
  UINT16             Pm1Sts;
  UINTN              Gpe0Sts;
  UINTN              Gpe0PmeSts;
  UINTN              Gpe0RiSts;
  UINT16             AcpiBase;

  DEBUG ((DEBUG_INFO, "GetWakeupEvent Entry()\n"));

  Gpe0Sts    = 0;
  Gpe0RiSts  = 0;
  Gpe0PmeSts = 0;
  PchAcpiBaseGet (&AcpiBase);

  ///
  /// Read the ACPI registers
  ///
  Pm1Sts     = IoRead16 (AcpiBase + R_PCH_ACPI_PM1_STS);
  Gpe0Sts    = IoRead32 (AcpiBase + R_PCH_ACPI_GPE0_STS_127_96);
  Gpe0RiSts  = Gpe0Sts & B_PCH_ACPI_GPE0_STS_127_96_RI;
  Gpe0PmeSts = Gpe0Sts & B_PCH_ACPI_GPE0_STS_127_96_PME;

  DEBUG ((DEBUG_INFO, "Gpe0Sts is: %02x\n", Gpe0Sts));
  DEBUG ((DEBUG_INFO, "ACPI Wake Status Register: %04x\n", Pm1Sts));

  ///
  /// Figure out the wake-up event
  ///
  if ((Pm1Sts & B_PCH_ACPI_PM1_EN_PWRBTN) != 0 ) {
    *WakeUpType = SystemWakeupTypePowerSwitch;
  } else if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0) {
    if (Gpe0PmeSts != 0) {
      *WakeUpType = SystemWakeupTypePciPme;
    } else if (Gpe0RiSts != 0) {
      *WakeUpType = SystemWakeupTypeModemRing;
    } else if (Gpe0Sts != 0) {
      *WakeUpType = SystemWakeupTypeOther;
    } else {
      *WakeUpType = SystemWakeupTypeOther;
    }
  } else if ((Pm1Sts & B_PCH_ACPI_PM1_STS_RTC) != 0) {
    *WakeUpType = SystemWakeupTypeApmTimer;
  } else {
    *WakeUpType = SystemWakeupTypeOther;
  }

  DEBUG ((DEBUG_INFO, "GetWakeupEvent : WakeUpType %x\n", *WakeUpType));
}

// APTIOV_SERVER_OVERRIDE_RC_START : Call AMI IsRecovery function
//Defined in OEMPort.c
BOOLEAN 
IsRecovery (
    EFI_PEI_SERVICES    **PeiServices,
    EFI_PEI_PCI_CFG2_PPI   *PciCfg,
    EFI_PEI_CPU_IO_PPI    *CpuIo
);
// APTIOV_SERVER_OVERRIDE_RC_END : Call AMI IsRecovery function

/**

    Determines the Boot Mode of the platform and sets the Boot Mode variable

    @param PeiServices
    @param FileHandle

    @retval Status - Success if BootMode was set

**/
EFI_STATUS
UpdateBootMode (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  CHAR16                  *strBootMode;
  UINT32                  GpiValue;
  SYSTEM_CONFIGURATION    SystemConfiguration;

  EFI_STATUS                    Status;
  UINT16                        SleepType;
  EFI_BOOT_MODE                 BootMode;
  UINT8                         WakeupType;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // Updates the wakeupType which will be used to update the same in Smbios table 01
  //
  GetWakeupEvent (&WakeupType);
  PcdSet8S (PcdWakeupType, WakeupType);

  if (IsSimicsPlatform() == TRUE) {
    GpiValue =0;
  } else {
#ifdef PC_HOOK
    if (PciRead8 (PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC)) & B_PCH_SPI_BC_TSS) {
      GpiValue = 1;
    } else {
      GpiValue = 0;
    }
#else
	Status = GpioGetInputInversion(GPIO_SKL_H_GPP_B14, &GpiValue);
#endif
    ASSERT_EFI_ERROR (Status);
  }

  if (IsManufacturingMode()) {
    //
    // MFG mode must override all other Boot Modes.
    //
    BootMode = BOOT_WITH_MFG_MODE_SETTINGS;
    // APTIOV_SERVER_OVERRIDE_RC_START : Call AMI IsRecovery function
    } else if ( (GpiValue == 1) || \
            IsRecovery((EFI_PEI_SERVICES**)PeiServices, (*PeiServices)->PciCfg, (*PeiServices)->CpuIo)) {
    // APTIOV_SERVER_OVERRIDE_RC_END : Call AMI IsRecovery function
    BootMode = BOOT_IN_RECOVERY_MODE;
    Status = PeiServicesInstallPpi (&mPpiListRecoveryBootMode);
    ASSERT_EFI_ERROR (Status);
  // APTIOV_SERVER_OVERRIDE_RC_START : Following code always sets BootMode to BOOT_WITH_FULL_CONFIGURATION since PcdBootState is commented
  #if 0
  } else if (PcdGetBool(PcdBootState)) {
    //
    // If this is first boot after flash, override sleep states.
    //
    BootMode = BOOT_WITH_FULL_CONFIGURATION;
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Following code always sets BootMode to BOOT_WITH_FULL_CONFIGURATION since PcdBootState is commented
  } else if ((MmioRead16 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B) & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) ||
                (IsCMOSBad())) {

    BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    //
    // If the BIOS defaults are loaded,
    // Admin password should be saved (to prevent its reset)
    //
    GetSpecificConfigGuid (&gEfiSetupVariableGuid, &SystemConfiguration);
    SaveAdminPassToHob(PeiServices, &SystemConfiguration);

  } else {
    //
    // Normal boot mode most of the time. Safe to override it with S states below.
    //
    BootMode = BOOT_WITH_MINIMAL_CONFIGURATION;

    if (GetSleepTypeAfterWakeup (PeiServices, &SleepType)) {
      switch (SleepType) {
        case V_PCH_ACPI_PM1_CNT_S3:
          BootMode = BOOT_ON_S3_RESUME;
          break;
        case V_PCH_ACPI_PM1_CNT_S4:
          BootMode = BOOT_ON_S4_RESUME;
          break;
        case V_PCH_ACPI_PM1_CNT_S5:
          BootMode = BOOT_ON_S5_RESUME;
          DEBUG ((DEBUG_ERROR, "\nBUGBUG: Overriding S5_RESUME with BOOT_WITH_MINIMAL_CONFIGURATION.\n"));
          BootMode = BOOT_WITH_MINIMAL_CONFIGURATION;
          break;
      }
    }

    if (MmioRead16 (
          MmPciBase (
          DEFAULT_PCI_BUS_NUMBER_PCH,
          PCI_DEVICE_NUMBER_PCH_PMC,
          PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B) & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS)
    {
      //
      // Report RTC battery failure
      //
      DEBUG ((DEBUG_ERROR, "RTC Power failure !! Could be due to a weak or missing battery.\n"));

      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
  }


  switch (BootMode) {
    case BOOT_WITH_FULL_CONFIGURATION:
      strBootMode = L"BOOT_WITH_FULL_CONFIGURATION";
      break;
    case BOOT_WITH_MINIMAL_CONFIGURATION:
      strBootMode = L"BOOT_WITH_MINIMAL_CONFIGURATION";
      break;
    case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
      strBootMode = L"BOOT_ASSUMING_NO_CONFIGURATION_CHANGES";
      break;
    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
      strBootMode = L"BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS";
      break;
    case BOOT_WITH_DEFAULT_SETTINGS:
      strBootMode = L"BOOT_WITH_DEFAULT_SETTINGS";
      break;
    case BOOT_ON_S4_RESUME:
      strBootMode = L"BOOT_ON_S4_RESUME";
      break;
    case BOOT_ON_S5_RESUME:
      strBootMode = L"BOOT_ON_S5_RESUME";
      break;
    case BOOT_ON_S2_RESUME:
      strBootMode = L"BOOT_ON_S2_RESUME";
      break;
    case BOOT_ON_S3_RESUME:
      strBootMode = L"BOOT_ON_S3_RESUME";
      break;
    case BOOT_ON_FLASH_UPDATE:
      strBootMode = L"BOOT_ON_FLASH_UPDATE";
      break;
    case BOOT_IN_RECOVERY_MODE:
      strBootMode = L"BOOT_IN_RECOVERY_MODE";
      break;
    case BOOT_WITH_MFG_MODE_SETTINGS:
      strBootMode = L"BOOT_WITH_MFG_MODE_SETTINGS";
      break;
    default:
      strBootMode = L"Unknown boot mode";
  } // switch (BootMode)
  DEBUG ((EFI_D_ERROR, "Setting BootMode to %s\n", strBootMode));

  Status = PeiServicesSetBootMode (BootMode);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**

  Get sleep type after wakeup

  @param PeiServices       Pointer to the PEI Service Table.
  SleepType         Sleep type to be returned.

  @retval TRUE              A wake event occured without power failure.
  @retval FALSE             Power failure occured or not a wakeup.

**/
BOOLEAN
GetSleepTypeAfterWakeup (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  OUT UINT16                    *SleepType
  )
{
  UINT16  Pm1Sts;
  UINT16  Pm1Cnt;
  UINTN         PciPmcRegBase;
  PciPmcRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC);
  //
  // Read the ACPI registers
  //
  Pm1Sts  = IoRead16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS);
  Pm1Cnt  = IoRead16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT);


  //
  // Get sleep type if a wake event occurred and there is no power failure
  //
  DEBUG ((DEBUG_ERROR, "\nGetSleepTypeAfterWakeup() Pm1Sts = %x, Pm1Cnt = %x\n", Pm1Sts, Pm1Cnt));
  if (((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0 &&
      (MmioRead8(PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B)) & (B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS | B_PCH_PMC_GEN_PMCON_B_PWR_FLR)) == 0 ) {
    *SleepType = Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP;
    if((*SleepType == V_PCH_ACPI_PM1_CNT_S3) && (Pm1Sts & B_PCH_ACPI_PM1_STS_PRBTNOR)) {
      DEBUG((DEBUG_ERROR, "Power Button override during S3, force system to S5 boot path\n"));
      *SleepType = V_PCH_ACPI_PM1_CNT_S5; // Power button override when S3, force the system to S5 boot path;
      IoWrite16(PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, (B_PCH_ACPI_PM1_STS_WAK));
    }
    return TRUE;
  }
  //
  // Make sure to clear WAK STS bit since we decided it is not returning from S3
  //
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0) {
    IoWrite16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, (B_PCH_ACPI_PM1_STS_WAK));
    Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SLP_TYP;
    IoWrite32 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
  }
  return FALSE;
}


/**
  Admin password is read and saved to HOB
  to prevent its cleared on defaults load.

  @param PeiServices          - pointer to the PEI Service Table
  @param SystemConfiguration  - pointer to Platform Setup Configuration

  @retval None.

**/
VOID
SaveAdminPassToHob (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  IN SYSTEM_CONFIGURATION   *SystemConfiguration
  )
{
  BuildGuidDataHob (
        &gEfiAdminPasswordHobGuid,
        SystemConfiguration->AdminPassword,
        sizeof (SystemConfiguration->AdminPassword)
        );

}

/**

    Check to see if CMOS is bad or cleared

    @param None

    @retval TRUE  - CMOS is bad
    @retval FALSE - CMOS is good

**/
BOOLEAN
IsCMOSBad(
     VOID
)
{
  UINT8           Nmi;
  volatile UINT32 Data32;
  Nmi     = 0;
  Data32  = 0;
  //
  // Preserve NMI bit setting
  //
  PchAlternateAccessMode (TRUE);
  Nmi     = (IoRead8 (R_IOPORT_CMOS_STANDARD_INDEX) & 0x80);
  PchAlternateAccessMode (FALSE);

  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, R_IOPORT_CMOS_IDX_DIAGNOSTIC_STATUS | Nmi);
  if (IoRead8 (R_IOPORT_CMOS_STANDARD_DATA) & (BIT6 + BIT7)) {
    return TRUE;
  } else {
    return FALSE;
  }
}
