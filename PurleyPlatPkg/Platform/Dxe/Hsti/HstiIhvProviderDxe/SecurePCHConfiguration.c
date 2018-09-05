/** @file
  This file contains the tests for the SecuePCHConfiguration bit

@copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/
#include "HstiIhvProviderDxe.h"

#define B_PCH_THERMAL_TBARB_MASK_64 0xFFFFFFFFFFFFF000

/**
  Run tests for SecurePCHConfiguration bit
**/
VOID
CheckSecurePchConfiguration (
  VOID
  )
{

  EFI_STATUS      Status;
  BOOLEAN         Result;
  UINT16          PmBase;
  UINT64          TbarB;
  UINT8           Srdl;
  UINT32          GenPmConLock;
  UINT32          PmCfg;
  UINT32          SmiEn;
  UINT8           GenPmCon1;
  UINT16          Tco1Cnt;
  UINT32          Hostc;
  UINT16          Cmd;
  CHAR16          *HstiErrorString;
  UINT32          PchSpiBar0;
  UINT32          PchPwrmBase;
  UINT32          PcrRtc;
  UINT16          TcoBase;
  UINTN           P2sbBase;
  BOOLEAN         P2sbOrgStatus;

  if ((mFeatureImplemented[1] & HSTI_BYTE1_SECURE_PCH_CONFIGURATION) == 0) {
    return;
  }

  Result = TRUE;

  PchSpiBar0 = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_SPI,PCI_FUNCTION_NUMBER_PCH_SPI) + R_PCH_SPI_BAR0) & ~B_PCH_SPI_BAR0_MASK;

  PchAcpiBaseGet (&PmBase);
  PchTcoBaseGet (&TcoBase);

  DEBUG ((DEBUG_INFO, "  PCH Security Configuration\n"));

  DEBUG ((DEBUG_INFO, "    1. Thermal Throttling Locks\n"));

  TbarB = LShiftU64(MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_THERMAL,PCI_FUNCTION_NUMBER_PCH_THERMAL) + R_PCH_THERMAL_TBARBH), 32);
  TbarB |= MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_THERMAL,PCI_FUNCTION_NUMBER_PCH_THERMAL) + R_PCH_THERMAL_TBARB);
  if (TbarB != 0xFFFFFFFFFFFFFFFF) {
    if ((TbarB & B_PCH_THERMAL_SPTYPEN) != 0) {
      TbarB = TbarB & B_PCH_THERMAL_TBARB_MASK_64;
    } else {
      TbarB = 0;
    }
    if (TbarB == 0) {
      TbarB = LShiftU64(MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_THERMAL,PCI_FUNCTION_NUMBER_PCH_THERMAL) + R_PCH_THERMAL_TBARH), 32);
      TbarB |= MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_THERMAL,PCI_FUNCTION_NUMBER_PCH_THERMAL) + R_PCH_THERMAL_TBAR);
      TbarB = TbarB & B_PCH_THERMAL_TBARB_MASK_64;
    }
    if ((MmioRead32 (TbarB + R_PCH_TBAR_TCFD) & B_PCH_TBAR_TCFD_TCD) == 0) {
      Cmd = MmioRead16 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_THERMAL,PCI_FUNCTION_NUMBER_PCH_THERMAL) + PCI_COMMAND_OFFSET);
      MmioWrite16 ((MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_THERMAL,PCI_FUNCTION_NUMBER_PCH_THERMAL) + PCI_COMMAND_OFFSET), Cmd | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
      if (((MmioRead8 (TbarB + R_PCH_TBAR_TSC) & B_PCH_TBAR_TSC_PLD) == 0) ||
          ((MmioRead8 (TbarB + R_PCH_TBAR_TSEL) & B_PCH_TBAR_TSEL_PLD) == 0) ||
          ((MmioRead8 (TbarB + R_PCH_TBAR_TSMIC) & B_PCH_TBAR_TSMIC_PLD) == 0) ||
          ((MmioRead32 (TbarB + R_PCH_TBAR_TL) & B_PCH_TBAR_TL_LOCK) == 0) ||
          ((MmioRead8 (TbarB + R_PCH_TBAR_PHLC) & B_PCH_TBAR_PHLC_LOCK) == 0)) {

        DEBUG ((DEBUG_INFO, "Thermal Throttling not locked\n"));

        HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_CODE_1 ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_STRING_1);
        Status = HstiLibAppendErrorString (
                   PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                   NULL,
                   HstiErrorString
                   );
        ASSERT_EFI_ERROR (Status);
        Result = FALSE;
        FreePool (HstiErrorString);
      }
      MmioWrite16 ((MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_THERMAL,PCI_FUNCTION_NUMBER_PCH_THERMAL) + PCI_COMMAND_OFFSET), Cmd);
    }
  } else {
    DEBUG ((DEBUG_INFO, "      Thermal device not present.\n"));
  }

  DEBUG ((DEBUG_INFO, "    2. Set Strap Lock\n"));

  Srdl = MmioRead8 (PchSpiBar0 + R_PCH_SPI_SSML);
  if ((Srdl & B_PCH_SPI_SSML_SSL) == 0) {
    DEBUG ((DEBUG_INFO, "Set Strap Lock %x\n",Srdl));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_CODE_2 ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_STRING_2);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    3. ACPI Base Lock\n"));

  GenPmConLock = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_PMC,PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B);

  if ((GenPmConLock & B_PCH_PMC_GEN_PMCON_B_ACPI_BASE_LOCK) == 0) {

    DEBUG ((DEBUG_INFO, "ACPI Base not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_CODE_2 ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_STRING_2);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    4. PM_CFG\n"));

  PchPwrmBaseGet (&PchPwrmBase);
  PmCfg = MmioRead32 (PchPwrmBase + R_PCH_PWRM_CFG);

  if (((PmCfg & BIT22) == 0) ||
      ((PmCfg & BIT27) == 0)) {

    DEBUG ((DEBUG_INFO, "PMC not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_CODE_3 ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_STRING_3);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    5. SMI Enable\n"));

  SmiEn = IoRead32 (PmBase + R_PCH_SMI_EN);
  if ((SmiEn & B_PCH_SMI_EN_GBL_SMI) == 0x0) {

    DEBUG ((DEBUG_INFO, "SMIs not enabled\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_CODE_D ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_STRING_D);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    6. SMI Lock\n"));

  GenPmCon1 = MmioRead8 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_PMC,PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_A);
  if ((GenPmCon1 & B_PCH_PMC_GEN_PMCON_A_SMI_LOCK) == 0x0) {

    DEBUG ((DEBUG_INFO, "SMI Not locked\n"));
    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_CODE_D ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_STRING_D);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    7. TCO SMI Lock\n"));

  Tco1Cnt = IoRead16 (TcoBase + R_PCH_TCO1_CNT);
  if ((Tco1Cnt & B_PCH_TCO_CNT_LOCK) == 0x0) {
    DEBUG ((DEBUG_INFO, "TCO SMI Lock %x\n",(Tco1Cnt & B_PCH_TCO_CNT_LOCK)));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_CODE_E ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_STRING_E);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    8. RTC Bios Interface Lock\n"));

  PcrRtc = MmioRead32 (PCH_PCR_ADDRESS (PID_RTC,R_PCH_PCR_RTC_CONF));

  if ((PcrRtc & BIT31) == 0x0) {
    DEBUG ((DEBUG_INFO, "Rtc BILD %x\n",(PcrRtc & BIT31)));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_CODE_4 ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE0_BOOT_FIRMWARE_MEDIA_PROTECTION_ERROR_STRING_4);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }


  DEBUG ((DEBUG_INFO, "  Table 7-7. SPD Configuration and Write Protection\n"));

  Hostc = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_SMBUS,PCI_FUNCTION_NUMBER_PCH_SMBUS) + R_PCH_SMBUS_HOSTC);
  if ((Hostc & B_PCH_SMBUS_HOSTC_SPDWD) == 0) {

    DEBUG ((DEBUG_INFO, "SPD write not disabled -  %x\n", Hostc));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_CODE_4 ,HSTI_PROCESSOR_SPD_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_STRING_4);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    10. P2SB SBI Lock\n"));

  P2sbBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB);
  PchRevealP2sb (P2sbBase, &P2sbOrgStatus);
  DEBUG ((DEBUG_INFO, "P2SB SBI Control Reg %x\n",(MmioRead32 (P2sbBase + R_PCH_P2SB_E0))));

  ///
  /// Check P2SB PCI Offset 0xE0[31] to be 1 indicating SBI interface is locked down
  ///
  if((MmioRead32 (P2sbBase + R_PCH_P2SB_E0) & BIT31) == 0){

    DEBUG ((DEBUG_INFO, "P2SB SBI Lock %x\n",(MmioRead32 (P2sbBase + R_PCH_P2SB_E0) & BIT31)));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_CODE_4 ,HSTI_PCH_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_PCH_CONFIGURATION_ERROR_STRING_4);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  if (!P2sbOrgStatus) {
    PchHideP2sb (P2sbBase);
  }

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               1,
               HSTI_BYTE1_SECURE_PCH_CONFIGURATION
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
