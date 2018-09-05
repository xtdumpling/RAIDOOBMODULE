/** @file
  This file contains the tests for the SecureCPUConfiguration bit

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

extern IIO_GLOBALS              *IioGlobalData;
extern EFI_MP_SERVICES_PROTOCOL *mMpService;
//
//  HstiCpuTestResults Array Contents
//      - TRUE:  Passed
//      - FALSE: Failed
//    Index 00 - Microcode Update Revision Check
//    Index 01 - Sample Part Check
//    Index 02 - IA32_FEATURE_CONTROL MSR Lock Check
//    Index 03 - FEATURE_CONFIG MSR Lock Check
//    Index 04 - SMRR1 Supported Check
//    Index 05 - SMRR1 Programmed Consistently Check
//    Index 06 - SMRR1 enabled/correct Check
//    Index 07 - SMRR1 and TSEGMB match
//    Index 08 - SMRR1 size Check
//    Index 09 - SMRR1 work Check
//    Index 10 - C-State Configuration Lock Check
//    Index 11 - Flexratio Lock Check
//    Index 12 - SNC Config Lock Check
//    Index 13 - PROCHOT# config Lock Check
//    Index 14 - Thermal Monitoring Interrupts Config Lock Check
//
#define CPU_MAX_TESTS           15
BOOLEAN                         HstiCpuTestResults[CPU_MAX_TESTS];
UINT64                          MasterSmrrBase = 0;
UINT64                          MasterSmrrMask = 0;
EFI_CPU_CSR_ACCESS_PROTOCOL     *CpuCsrAccess;
UINT32                          TsegMB;
EFI_IIO_UDS_PROTOCOL            *IioUds;
BOOLEAN                         SimicsPlatform = FALSE;

/**
  Multithreaded function to collect test results on each thread.
**/
VOID
CollectCpuTestResults (
  VOID
)
{
  UINT64                      Ia32FeatureControlMsr;
  UINT64                      SmrrBase;
  UINT64                      SmrrMask;
  CPUID_VERSION_INFO_ECX      Ecx;
  UINT64                      MtrrCapMsr;

  //
  // Microcode Update Revision Check
  //
  if ((AsmReadMsr64(MSR_IA32_BIOS_SIGN_ID) & 0xFFFFFFFF00000000) == 0) {
    HstiCpuTestResults[0] = FALSE;
  }

  //
  // Sample Part Check
  //
  if ((AsmReadMsr64(MSR_PLATFORM_INFO) & BIT27) != 0) {
    HstiCpuTestResults[1] = FALSE;
  }

  //
  // IA32_FEATURE_CONTROL MSR Lock Check
  //
  Ia32FeatureControlMsr = AsmReadMsr64 (EFI_MSR_IA32_FEATURE_CONTROL);
  if ((Ia32FeatureControlMsr & B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
    HstiCpuTestResults[2] = FALSE;
  }

  //
  // FEATURE_CONFIG MSR Lock Check
  //
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, &Ecx.Uint32, NULL);
  if (Ecx.Bits.AESNI == 1) {
    if ((AsmReadMsr64 (EFI_MSR_IA32_FEATURE_CONFIG) & B_EFI_IA32_AESNI_LOCK) == 0) {
      HstiCpuTestResults[3] = FALSE;
    }
  }

  //
  // SMRR1 Supported Check
  //
  MtrrCapMsr = AsmReadMsr64 (EFI_IA32_MTRR_CAP);
  if ((MtrrCapMsr & B_EFI_IA32_MTRR_CAP_SMRR_SUPPORT) == 0) {
    HstiCpuTestResults[4] = FALSE;
  }

  //
  // Only run the following checks if SMRR1 is supported
  //
  if ((MtrrCapMsr & B_EFI_IA32_MTRR_CAP_SMRR_SUPPORT) != 0) {

    SmrrBase = AsmReadMsr64 (EFI_SMRR_PHYS_BASE);
    SmrrMask = AsmReadMsr64 (EFI_SMRR_PHYS_MASK);

    if ((MasterSmrrBase == 0) && (MasterSmrrMask == 0)) {
      MasterSmrrBase = SmrrBase;
      MasterSmrrMask = SmrrMask;
    }

    //
    // SMRR1 Programmed Consistently Check
    //
    if ((SmrrBase != MasterSmrrBase) || (SmrrMask != MasterSmrrMask)) {
      HstiCpuTestResults[5] = FALSE;
    }

    //
    // SMRR1 enabled/correct Check
    //
    if (((SmrrMask & BIT11) == 0x0) ||
        ((SmrrBase & 0x7) != 0x6)) {
      HstiCpuTestResults[6] = FALSE;
    }

    //
    // SMRR1 and TSEGMB match
    //
    if ((SmrrBase & (SmrrMask & 0xFFFFF000)) != (TsegMB & B_SA_TSEGMB_TSEGMB_MASK)) {
      HstiCpuTestResults[7] = FALSE;
    }

    //
    // SMRR1 Size check
    //
    if ((UINT32) (~(SmrrMask & 0xFFFFF000) + 1) != IioUds->IioUdsPtr->PlatformData.MemTsegSize) {
      HstiCpuTestResults[8] = FALSE;
    }

    //
    // SMRR1 work Check
    //
    if (MmioRead32 (SmrrBase & SmrrMask) != 0xFFFFFFFF) {
      HstiCpuTestResults[9] = FALSE;
    }
  }
  //
  // C-State Configuration Locked
  //
  if ((AsmReadMsr64 (EFI_IA32_PMG_CST_CONFIG) & B_EFI_CST_CONTROL_LOCK) == 0) {
    HstiCpuTestResults[10] = FALSE;
  }

  //
  // Flex_Ratio Locked
  // First check if OC is supported (OC_BINS)
  //
  if (AsmReadMsr64 (MSR_FLEX_RATIO) & (BIT19 | BIT18 | BIT17)) {
    if ((AsmReadMsr64 (MSR_FLEX_RATIO) & BIT20) == 0) {
      HstiCpuTestResults[11] = FALSE;
    }
  }

  //
  // SNC Config Locked
  //
  if (SimicsPlatform == FALSE) {
    if ((AsmReadMsr64 (MSR_SNC_CONFIG) & BIT28) == 0) {
      HstiCpuTestResults[12] = FALSE;
    }
  }

  //
  // PROCHOT# config Locked
  //
  if ((AsmReadMsr64 (EFI_MSR_POWER_CTL) & BIT27) == 0) {
    HstiCpuTestResults[13] = FALSE;
  }

  //
  // Thermal Monitoring Interrupts Config locked
  //
  if ((AsmReadMsr64 (MSR_MISC_PWR_MGMT) & B_EFI_MSR_MISC_PWR_MGMT_LOCK) == 0) {
    HstiCpuTestResults[14] = FALSE;
  }

}


/**
  Run tests for SecureCPUConfiguration bit
**/
VOID
CheckSecureCpuConfiguration (
  VOID
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           Result;
  FIRMWARE_INTERFACE_TABLE_ENTRY    *FitEntry;
  UINT32                            EntryNum;
  UINT64                            FitTableOffset;
  UINT32                            FitIndex;
  UINT64                            SmmFeatureControl;
  UINTN                             FoundValidUcode;
  CHAR16                            *HstiErrorString;
  UINT32                            CsrReg;
  UINT64                            LtCtrl;
  UINT8                             Index = 0;

  if ((mFeatureImplemented[0] & HSTI_BYTE0_SECURE_CPU_CONFIGURATION) == 0) {
    return;
  }

  //
  // Intialize Results Array
  //
  for(Index = 0; Index < CPU_MAX_TESTS; Index++) {
    HstiCpuTestResults[Index] = TRUE;
  }

  //
  // Initialize global variables
  //
  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;

  TsegMB = CpuCsrAccess->ReadCpuCsr(0, 0, TSEG_N0_IIO_VTD_REG);

  SimicsPlatform = IsSimicsPlatform();

  //
  // Locate gEfiIioUdsProtocolGuid protocol instance and assign it to a global variable
  //
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, (VOID **) &IioUds);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR,"Failed to locate gEfiIioUdsProtocolGuid Protocol\n"));
  }

  //
  // Collect HSTI MSR Cpu Results on all threads
  //
  CollectCpuTestResults();
  mMpService->StartupAllAPs (
                mMpService,
                (EFI_AP_PROCEDURE)CollectCpuTestResults,
                FALSE,
                NULL,
                0,
                NULL,
                NULL
                );

  Result = TRUE;

  DEBUG ((DEBUG_INFO, "CPU Security Configuration\n"));

  DEBUG ((DEBUG_INFO, "    1. Microcode Update Revision\n"));

  if (HstiCpuTestResults[0] == FALSE) {

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_1 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_1);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    2. Sample Part \n"));


  if (HstiCpuTestResults[1] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: This is a sample part\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_2 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_2);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    3. IA32_FEATURE_CONTROL MSR Lock\n"));

  if (HstiCpuTestResults[2] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: IA32_FEATURE_CONTROL MSR not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_3 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_3);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    4. FEATURE_CONFIG MSR  Lock\n"));

  if (HstiCpuTestResults[3] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: Feature control msr not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_5 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_5);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    5. FIT Patch Support \n"));

  FitTableOffset = *(UINT64 *) (UINTN) (BASE_4GB - 0x40);
  FitEntry = (FIRMWARE_INTERFACE_TABLE_ENTRY *) (UINTN) FitTableOffset;

  if ((FitEntry <= (FIRMWARE_INTERFACE_TABLE_ENTRY *) (UINTN) 0xFF000000) ||
      (FitEntry >= (FIRMWARE_INTERFACE_TABLE_ENTRY *) (UINTN) 0xFFFFFFB0) ||
      (FitEntry[0].Address != *(UINT64 *) "_FIT_   ")) {
    DEBUG ((DEBUG_INFO, "Fail: Fit header incorrect\n"));


    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_6 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_6);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  if ((FitEntry[0].Size[0] == 0) ||
      ((FitEntry[0].Size[0] * 16) + FitEntry > (FIRMWARE_INTERFACE_TABLE_ENTRY *) (UINTN) 0xFFFFFFC0 ) ||
      (FitEntry[0].Type != 0) ||
      (FitEntry[0].Version != 0x0100) ||
      (FitEntry[0].Reserved != 0)) {
    DEBUG ((DEBUG_INFO, "Fail: Fit table sanity checks failed\n"));


    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_6 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_6);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  EntryNum = *(UINT32 *) (&FitEntry[0].Size[0]) & 0xFFFFFF;
  FoundValidUcode = 0;

  for (FitIndex = 0; FitIndex < EntryNum; FitIndex++) {
    if (FitEntry[FitIndex].Type == FIT_TYPE_00_HEADER) {
      continue;
    }

    if (FitEntry[FitIndex].Type == FIT_TYPE_7F_SKIP) {
      continue;
    }

    if (FitEntry[FitIndex].Type == FIT_TYPE_01_MICROCODE) {
   	  // APTIOV_SERVER_OVERRIDE_RC_START: Skip and continue parsing Fit table if the first DWORD in empty slot of Type 0x01 is 0xFFFF_FFFF
      if ((UINT32)(FitEntry[FitIndex].Address) == 0xFFFFFFFF) {
   	  // APTIOV_SERVER_OVERRIDE_RC_END: Skip and continue parsing Fit table if the first DWORD in empty slot of Type 0x01 is 0xFFFF_FFFF
        continue;
      }

      if (FitEntry[FitIndex].Address > BASE_4GB) {
        FoundValidUcode = 0;
        break;
      }

      if (MmioRead32 (FitEntry[FitIndex].Address) != 0x00000001) {
        FoundValidUcode = 0;
        break;
      }
      FoundValidUcode = 1;
    }

    if (FitEntry[FitIndex].Type != FIT_TYPE_01_MICROCODE) {
      continue;
    }
  }

  if (FoundValidUcode != 1) {
    DEBUG ((DEBUG_INFO, "Fail: Fit table does not contain valid ucode entry\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_6 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_6);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "SMM Security Configuration\n"));

  DEBUG ((DEBUG_INFO, "    1. SMRAM Lock\n"));

  LtCtrl = AsmReadMsr64(EFI_MSR_LT_CONTROL);

  if ((LtCtrl & BIT0) == 0) {
    DEBUG ((DEBUG_INFO, "Fail: SMRAM not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_8 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_8);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    2. TSEGMB\n"));

  if ((LtCtrl & BIT0) == 0) {
    DEBUG ((DEBUG_INFO, "Fail: TSEGMB not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_8 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_8);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    3. TSEGMB Alignment\n"));

  if (((TsegMB & B_SA_TSEGMB_TSEGMB_MASK) & (IioUds->IioUdsPtr->PlatformData.MemTsegSize - 1)) != 0) {
    DEBUG ((DEBUG_INFO, "Fail: TSEGMB not size aligned, TSEG size: 0x%x\n",(IioUds->IioUdsPtr->PlatformData.MemTsegSize - 1)));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_9 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_9);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    4. SMRR1 are supported\n"));

  if (HstiCpuTestResults[4] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: SMRR1 not supported\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_A ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_A);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  } else {
    DEBUG ((DEBUG_INFO, "    5. SMRR1 programmed consistently on all cores\n"));

    if (HstiCpuTestResults[5] == FALSE) {
      DEBUG ((DEBUG_INFO, "Fail: SMRR1 not programmed consistently across all cores\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_A ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_A);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    6. SMRR1 enabled/correct\n"));

    if (HstiCpuTestResults[6] == FALSE) {
      DEBUG ((DEBUG_INFO, "Fail: SMRR1 not enabled/correct\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_A ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_A);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    7. SMRR1 and TSEGMB match\n"));

    DEBUG ((DEBUG_INFO, "SMRR:   0x%08x - 0x%08x\n", MasterSmrrBase & (MasterSmrrMask & 0xFFFFF000), (UINT32) (~(MasterSmrrMask & 0xFFFFF000) + 1)));
    DEBUG ((DEBUG_INFO, "TSEGMB: 0x%08x\n", TsegMB & B_SA_TSEGMB_TSEGMB_MASK));

    if (HstiCpuTestResults[7] == FALSE) {
      DEBUG ((DEBUG_INFO, "Fail: SMRR1 != TSEGMB\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_A ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_A);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    8. SMRR1 size\n"));

    if (HstiCpuTestResults[8] == FALSE) {
      DEBUG ((DEBUG_INFO, "Fail: SMRR1 size != TSEGMB size\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_A ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_A);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    9. SMRR1 work\n"));

    if (HstiCpuTestResults[9] == FALSE) {
      DEBUG ((DEBUG_INFO, "Fail: SMRR1 not working, read succeeded\n"));


      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_A ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_A);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }
  }

  DEBUG ((DEBUG_INFO, "CPU Security Configuration Continued\n"));

  DEBUG ((DEBUG_INFO, "    6. C-State Configuration\n"));
  if (HstiCpuTestResults[10] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: C-State Configuration not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_B ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_B);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    7. Flex Ratio\n"));
  if (HstiCpuTestResults[11] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: Flex Ratio not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_C ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_C);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    8. SNC Configuration\n"));
  if (HstiCpuTestResults[12] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: SNC Config not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_D ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_D);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    9. PROCHOT# configuration\n"));
  if (HstiCpuTestResults[13] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: PROCHOT# Config not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_E ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_E);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    10. Thermal Monitoring Interrupts Config\n"));
  if (HstiCpuTestResults[14] == FALSE) {
    DEBUG ((DEBUG_INFO, "Fail: Thermal Monitoring Interrupts Config not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_16 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_16);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  //
  // Need to run the following checks on all sockets using CSRs
  //
  for (Index = 0; Index < MaxIIO; Index++) {
    if(!IioGlobalData->IioVar.IioVData.SocketPresent[Index]){
      continue;
    }

    DEBUG ((DEBUG_INFO, "    11. SMM_FEATURE_CONTROL MSR Lock\n"));
    DEBUG ((DEBUG_INFO, "       SMM_CODE_CHK_EN TEST\n"));

    SmmFeatureControl = CpuCsrAccess->ReadCpuCsr (Index, 0, SMM_FEATURE_CONTROL_UBOX_DFX_REG);
    if ((SmmFeatureControl & BIT2) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: SMM Code Fetch outside SMRAM detect feature disabled\n"));

      HstiErrorString = BuildHstiErrorString(HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_4 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_4A);
      Status = HstiLibAppendErrorString(
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool(HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "       SMM_FEATURE_CONTROL MSR Lock\n"));

    if ((SmmFeatureControl & BIT0) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: Smm feature control msr not locked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_4 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_4);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    12. Turbo Activation Ratio Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, TURBO_ACTIVATION_RATIO_PCU_FUN0_REG);
    if ((CsrReg & BIT31) == 0) {
      //
      // Not generating an error for this check yet
      //
      DEBUG ((DEBUG_INFO, "Warning: Turbo Activation unlocked\n"));
    }

    DEBUG ((DEBUG_INFO, "    13. P-State Limits Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, P_STATE_LIMITS_PCU_FUN0_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: P-State Limits unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_F ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_F);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    14. Package RAPL Limit Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: Package RAPL Limit unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_10 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_10);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    15. PMAX Config Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, PMAX_PCU_FUN0_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: PMAX config unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_11 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_11);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    16. Core Enable/Disable Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, CSR_DESIRED_CORES_CFG2_PCU_FUN1_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: Core Enable/Disable unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_12 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_12);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    17. SAPM Registers Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, SAPMCTL_PCU_FUN1_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: SAPM Registers unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_13 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_13);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    18. Power Plane Limit Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, DRAM_PLANE_POWER_LIMIT_N0_PCU_FUN2_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: Power Plane Limit unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_14 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_14);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    19. Package Power Limit config Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, PACKAGE_RAPL_LIMIT_N1_PCU_FUN0_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: Package Power Limit config unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_15 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_15);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    20. TDP Control Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, CONFIG_TDP_CONTROL_PCU_FUN3_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: TDP Control unlocked\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_17 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_17);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }

    DEBUG ((DEBUG_INFO, "    21. Memory Controller Scrambling Seed Lock\n"));
    DEBUG ((DEBUG_INFO, "       CH0\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, MCSCRAMBLECONFIG_MCDDC_DP_REG);
    if ((CsrReg & BIT2) != 0) {
      if ((CsrReg & BIT3) == 0) {
        DEBUG ((DEBUG_INFO, "Fail: Memory Controller Scrambling Seed unlocked\n"));

        HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_18 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_18);
        Status = HstiLibAppendErrorString (
                   PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                   NULL,
                   HstiErrorString
                   );
        ASSERT_EFI_ERROR (Status);
        Result = FALSE;
        FreePool (HstiErrorString);
      }
    } else {
      DEBUG ((DEBUG_INFO, "       Channel not enabled\n"));
    }

    DEBUG ((DEBUG_INFO, "       CH1\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 1, MCSCRAMBLECONFIG_MCDDC_DP_REG);
    if ((CsrReg & BIT2) != 0) {
      if ((CsrReg & BIT3) == 0) {
        DEBUG ((DEBUG_INFO, "Fail: Memory Controller Scrambling Seed unlocked\n"));

        HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_18 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_18);
        Status = HstiLibAppendErrorString (
                   PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                   NULL,
                   HstiErrorString
                   );
        ASSERT_EFI_ERROR (Status);
        Result = FALSE;
        FreePool (HstiErrorString);
      }
    } else {
      DEBUG ((DEBUG_INFO, "       Channel not enabled\n"));
    }

    DEBUG ((DEBUG_INFO, "       CH2\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 2, MCSCRAMBLECONFIG_MCDDC_DP_REG);
    if ((CsrReg & BIT2) != 0) {
      if ((CsrReg & BIT3) == 0) {
        DEBUG ((DEBUG_INFO, "Fail: Memory Controller Scrambling Seed unlocked\n"));

        HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_18 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_18);
        Status = HstiLibAppendErrorString (
                   PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                   NULL,
                   HstiErrorString
                   );
        ASSERT_EFI_ERROR (Status);
        Result = FALSE;
        FreePool (HstiErrorString);
      }
    } else {
      DEBUG ((DEBUG_INFO, "       Channel not enabled\n"));
    }

    DEBUG ((DEBUG_INFO, "    22. PECI is Trusted Lock\n"));
    CsrReg = CpuCsrAccess->ReadCpuCsr (Index, 0, PCU_LT_CTRL_PCU_FUN3_REG);
    if ((CsrReg & BIT31) == 0) {
      DEBUG ((DEBUG_INFO, "Fail: PECI is Trusted not set\n"));

      HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_CODE_19 ,HSTI_CPU_SECURITY_CONFIGURATION, HSTI_BYTE0_SECURE_CPU_CONFIGURATION_ERROR_STRING_19);
      Status = HstiLibAppendErrorString (
                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
                 NULL,
                 HstiErrorString
                 );
      ASSERT_EFI_ERROR (Status);
      Result = FALSE;
      FreePool (HstiErrorString);
    }
  }

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               0,
               HSTI_BYTE0_SECURE_CPU_CONFIGURATION
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
