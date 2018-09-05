/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for AES (Advanced Encryption Standard) feature.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Aes.c

**/

#include "Aes.h"

BOOLEAN    mEnableAes = FALSE;

/**
  Detect capability of AES for specified processor.
  
  This function detects capability of AES for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
AesDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  //
  // Processor feature flag checking
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);

  //
  // MSR_FEATURE_CONFIG is visible only if CPUID.(EAX=01):ECX[25]=1, which
  // indicates the processor was manufactured with AES-NI enabled.
  //
  if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_AESNI_SUPPORT, N_CPUID_AESNI_SUPPORT) == 1) {
    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

    if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSilvermontFamilyFlag) && IS_SILVERMONT_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuGoldmontFamilyFlag) && IS_GOLDMONT_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))
        ) {
      SetProcessorFeatureCapability (ProcessorNumber, Aes, NULL);
    }
  }
}

/**
  Configures Processor Feature Lists for AES for all processors.
  
  This function configures Processor Feature Lists for AES for all processors.

**/
VOID
AesConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  EFI_STATUS            Status;

  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_AES_BIT) != 0) {
    mEnableAes = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports AES
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, Aes, NULL)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of AES capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_AES_BIT);
        ASSERT_EFI_ERROR (Status);
        //
        // Set the bit of AES setting according to BSP setting.
        //
        if (mEnableAes) {
          Status = PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_AES_BIT);
          ASSERT_EFI_ERROR (Status);
        }
      }

      //
      // If this logical processor supports AES, then add feature into feature list for operation
      // on corresponding register.
      //
      AppendProcessorFeatureIntoList (ProcessorNumber, Aes, &mEnableAes);
    }
  }
}

/**
  Produces entry of AES feature in Register Table for specified processor.
  
  This function produces entry of AES feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
AesReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINTN      Value;
  UINT64     MsrFeatureConfig;
  UINT32     ThreadNumber;

  //
  // The scope of the MSR_FEATURE_CONFIG is Core, only program MSR_FEATURE_CONFIG for thead 0
  // of each core. Otherwise, once a thread in the core disabled AES, the other thread will cause GP when
  // programming it.
  //
  GetProcessorLocation (ProcessorNumber, NULL, NULL, &ThreadNumber);
  if (ThreadNumber == 0) {
    Enable = (BOOLEAN *) Attribute;
    Value  = (1 << 1);
    if (*Enable) {
      Value = 0;
    }

    MsrFeatureConfig = AsmReadMsr64 (MSR_FEATURE_CONFIG);
    if ((MsrFeatureConfig & B_MSR_FEATURE_CONFIG_LOCK) == 0) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_FEATURE_CONFIG, N_MSR_FEATURE_CONFIG_LOCK, 2, Value | B_MSR_FEATURE_CONFIG_LOCK);
    }
  }
}

