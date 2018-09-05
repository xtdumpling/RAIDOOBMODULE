/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for ThermalManagement Feature

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  ThermalManagement.c

**/

#include "ThermalManagement.h"

UINT8    *mThermalManagementCapability;

/**
  Allocate buffer for Thermal Management feature configuration.
  
**/
VOID
ThermalManagementAllocateBuffer (
  VOID
  )
{
  mThermalManagementCapability = (UINT8 *)AllocateZeroPool (sizeof (UINT8) * mCpuConfigContextBuffer.NumberOfProcessors);
  ASSERT (mThermalManagementCapability != NULL);
}

/**
  Detect capability of Thermal Management feature for specified processor.
  
  This function detects capability of Thermal Management feature feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ThermalManagementDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT32              FamilyId;
  UINT32              ModelId;

  //
  // Retrieve CPUID data from context buffer
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);

  //
  // If TM is supported.
  //
  if (BitFieldRead32 (CpuidRegisters->RegEdx, N_CPUID_TM_SUPPORT, N_CPUID_TM_SUPPORT) != 0) {
    mThermalManagementCapability[ProcessorNumber] |= CPU_THERMAL_MONITOR1_BIT;
  }
  //
  // If TM2 is supported.
  //
  if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_TM2_SUPPORT, N_CPUID_TM2_SUPPORT) != 0) {
    mThermalManagementCapability[ProcessorNumber] |= CPU_THERMAL_MONITOR2_BIT;
  }

  //
  // If TM or TM2 is supported, set feature capability for Thermal Management feature.
  //
  if ((mThermalManagementCapability[ProcessorNumber] &
       (CPU_THERMAL_MONITOR1_BIT | CPU_THERMAL_MONITOR2_BIT)) != 0) {
    SetProcessorFeatureCapability (ProcessorNumber, ThermalManagement, NULL);
  }

  //
  // If TCC Activation Offset supported, set feature capability for TCC Activation Offset feature
  // For SilverMont processors, needn't to check MSR_PLATFORM_INFO
  //
  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);
  if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag)  && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId)) || 
      (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))
      ) {
    if ((AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PROGRAMMABLE_TCC_ACTIVATION_OFFSET) == B_MSR_PROGRAMMABLE_TCC_ACTIVATION_OFFSET) {
      //
      // If programmming TCC Activation Offset is supported
      //
      SetProcessorFeatureCapability (ProcessorNumber, TccActivation, NULL);
    }
  }
}

/**
  Configures Processor Feature Lists for Thermal Management feature for all processors.
  
  This function configures Processor Feature Lists for Thermal Management feature for all processors.

**/
VOID
ThermalManagementConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;
  EFI_STATUS            Status;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_THERMAL_MANAGEMENT_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Thermal Management
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, ThermalManagement, NULL)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of Thermal Management capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_THERMAL_MANAGEMENT_BIT);
        ASSERT_EFI_ERROR (Status);
        //
        // Set the bit of Thermal Management setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          Status = PcdSet32S (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_THERMAL_MANAGEMENT_BIT);
          ASSERT_EFI_ERROR (Status);
        }
      }

      //
      // If this logical processor supports Thermal Management, then add feature into feature list for operation
      // on corresponding register.
      //
      if (UserConfigurationSet) {
        AppendProcessorFeatureIntoList (ProcessorNumber, ThermalManagement, NULL);
      }
    }
  }
}

/**
  Produces entry of Thermal Management feature in Register Table for specified processor.
  
  This function produces entry of Thermal Management feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
ThermalManagementReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    TmEnabled;
  BOOLEAN    Tm2Enabled;
  UINT32     FamilyId;
  UINT32     ModelId;

  TmEnabled  = FALSE;
  Tm2Enabled = FALSE;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  //
  // If TM2 is supported, then write 1 to MSR_IA32_MISC_ENABLE[13].
  //
  if ((mThermalManagementCapability[ProcessorNumber] & CPU_THERMAL_MONITOR2_BIT) != 0) {
    if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) ||
        IS_SILVERMONT_PROC (FamilyId, ModelId) || IS_GOLDMONT_PROC (FamilyId, ModelId) ||
        IS_IVYBRIDGE_PROC (FamilyId, ModelId) || IS_HASWELL_PROC (FamilyId, ModelId) ||
        IS_BROADWELL_PROC (FamilyId, ModelId) || IS_SKYLAKE_PROC(FamilyId, ModelId) ||
        IS_KNIGHTSLANDING_PROC (FamilyId, ModelId)
        ) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_AUTOMATIC_TCC_ENABLE, 1, 1);
    } else {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_TM2_ENABLE, 1, 1);
    }
    Tm2Enabled = TRUE;
  }

  //
  // If TM1 is supported
  //
  if ((mThermalManagementCapability[ProcessorNumber] & CPU_THERMAL_MONITOR1_BIT) != 0) {
    //
    // For Pentium 4 processor, enable TM1 if TM1 is supported and TM2 is not enabled.
    // For Conroe processor family and TunnelCreek family, enable TM1 just if it is supported.
    //
    if ((FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) || IS_TUNNELCREEK_PROC(FamilyId, ModelId) || !Tm2Enabled) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_AUTOMATIC_TCC_ENABLE, 1, 1);
      TmEnabled = TRUE;
    }
  }

  //
  // If TM or TM2 is enabled, then enable bi-directional PROCHOT# or FORCEPR#,
  // depending on the platform type.
  //
  if (TmEnabled || Tm2Enabled) {
    //
    // Enable Bi-Directional PROCHOT#
    //
    if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) ||
        IS_IVYBRIDGE_PROC (FamilyId, ModelId) || IS_HASWELL_PROC (FamilyId, ModelId) ||
        IS_BROADWELL_PROC (FamilyId, ModelId) || IS_SKYLAKE_PROC(FamilyId, ModelId) ||
        IS_KNIGHTSLANDING_PROC (FamilyId, ModelId)
        ) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_POWER_CTL, N_MSR_BI_DIRECTIONAL_PROCHOT_ENABLE, 1, 1);
    } else if (IS_GOLDMONT_PROC (FamilyId, ModelId)) {
      //
      // Goldmont does not support Bi-Directional PROCHOT#
      //
      WriteRegisterTable (ProcessorNumber, Msr, MSR_POWER_CTL, N_MSR_BI_DIRECTIONAL_PROCHOT_ENABLE, 1, 0);
    } else {
      //
      // If this is a server platform, enable FORCEPR#
      //
      if (mPlatformType == PLATFORM_SERVER) {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, 21, 1, 1);
      } else {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, 17, 1, 1);
      }
    }
  }

  //
  // For Conroe Processor Family, advanced registers should be set.
  //
  if (FeaturePcdGet (PcdCpuConroeFamilyFlag) || FeaturePcdGet (PcdCpuNehalemFamilyFlag) ||
      FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) || FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) ||
      FeaturePcdGet (PcdCpuTunnelCreekFamilyFlag) || FeaturePcdGet (PcdCpuHaswellFamilyFlag) ||
      FeaturePcdGet (PcdCpuBroadwellFamilyFlag) || FeaturePcdGet (PcdCpuSkylakeFamilyFlag) ||
      FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag)
      ) {
    if ((FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) || IS_TUNNELCREEK_PROC(FamilyId, ModelId)) {
      //
      // BIOS should enable Lock TM Interrupt(MSR_PIC_SENS_CFG[22]), Lock TM1(MSR_PIC_SENS_CFG[21]).
      // In addition, if both TM1 and TM2 are enabled, we also enable Extended Throttle(MSR_PIC_SENS_CFG[20]).
      //
      if (TmEnabled && Tm2Enabled) {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_PIC_SENS_CFG, 20, 3, 7);
      } else {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_PIC_SENS_CFG, 21, 2, 3);
      }
    } else if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) ||
               IS_IVYBRIDGE_PROC (FamilyId, ModelId) || IS_HASWELL_PROC (FamilyId, ModelId) ||
               IS_BROADWELL_PROC (FamilyId, ModelId) || IS_SKYLAKE_PROC(FamilyId, ModelId) ||
               IS_KNIGHTSLANDING_PROC (FamilyId, ModelId)
               ) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_MISC_PWR_MGMT, N_MSR_LOCK_TM_INTERRUPT, 1, 1);
    }
  }

  if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag)  && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))
      ) {
    if (GetProcessorFeatureCapability (ProcessorNumber, TccActivation, NULL)) {
      WriteRegisterTable (
        ProcessorNumber,
        Msr,
        MSR_TEMPERATURE_TARGET,
        N_MSR_TCC_ACTIVATION_OFFSET_START,
        N_MSR_TCC_ACTIVATION_OFFSET_STOP - N_MSR_TCC_ACTIVATION_OFFSET_START + 1,
        PcdGet8 (PcdCpuTccActivationOffset)
        );  
    }
  }
}
