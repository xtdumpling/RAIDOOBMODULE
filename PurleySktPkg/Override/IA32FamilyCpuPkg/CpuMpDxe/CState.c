/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for C-State feature.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CState.c

**/

#include "CState.h"

BOOLEAN    mEnableCState = FALSE;
BOOLEAN    mEnableC1AutoDemotion = FALSE;
BOOLEAN    mEnableC3AutoDemotion = FALSE;
BOOLEAN    mEnableC1AutoUndemotion = FALSE;
BOOLEAN    mEnableC3AutoUndemotion = FALSE;
UINTN      *mMaxCStateValue;

//
// PURLEY_OVERRIDE_BEGIN
//
extern  UINT8   mPcdCpuCoreCStateValue;
//
// PURLEY_OVERRIDE_END
//

/**
  Allocate buffer for C State feature configuration.
  
**/
VOID
CStateAllocateBuffer (
  VOID
  )
{
  mMaxCStateValue = (UINTN *)AllocatePool (
                               sizeof (UINTN) * mCpuConfigContextBuffer.NumberOfProcessors
                               );
  ASSERT (mMaxCStateValue != NULL);
}

/**
  Detect capability of C-State for specified processor.
  
  This function detects capability of C-State for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
CStateDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT32              FamilyId;
  UINT32              ModelId;
  UINT32              SteppingId;
  UINTN               Index;
  UINTN               CStateValue;
  UINT32              Params;
  
  //
  // C-State feature has dependency on Monitor/Mwait capability, so
  // Monitor/Mwait feature detection should be perform prior to C-State feature.
  //
  if (!GetProcessorFeatureCapability (ProcessorNumber, MonitorMwait, NULL)) {
    return;
  }

  //
  // By default, Monitor/Mwait is available if it is supported. We don't need to enable it here.
  //
  DEBUG_CODE (
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
    ASSERT (CpuidRegisters != NULL);
    ASSERT (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_MONITOR_MWAIT_SUPPORT, N_CPUID_MONITOR_MWAIT_SUPPORT) == 1);  
  );
  
  //
  // Processor feature flag checking
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_MONITOR_MWAIT_PARAMS);
  ASSERT (CpuidRegisters != NULL);

  // OVERIDE_HSD_5330336_BEGIN (replaces old code)
  if ((CpuidRegisters->RegEdx & 0xFFFFFFF0) != 0) {
    // 
    // Calculate MAX C state
    // 
    CStateValue = 0;
    for (Index = 7; Index > 0; Index--) {
      Params = CpuidRegisters->RegEdx >> (Index * 4);
      if((Params & 0xF) != 0) {
        CStateValue = Index;
        break;
      }
    }
    if (CStateValue > 1) {
      mMaxCStateValue[ProcessorNumber] = mPcdCpuCoreCStateValue;
      if (CStateValue < mPcdCpuCoreCStateValue) {
        mMaxCStateValue[ProcessorNumber] = CStateValue;
      }
  // OVERIDE_HSD_5330336_END

      SetProcessorFeatureCapability (ProcessorNumber, CState, NULL);

      GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);
      if (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) {
        SetProcessorFeatureCapability (ProcessorNumber, C1AutoDemotion, NULL);
        SetProcessorFeatureCapability (ProcessorNumber, C3AutoDemotion, NULL);
      }
      if (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId)) {
        SetProcessorFeatureCapability (ProcessorNumber, C1AutoDemotion, NULL);
        SetProcessorFeatureCapability (ProcessorNumber, C1AutoUndemotion, NULL);
      }
    }
  }
}

/**
  Configures Processor Feature Lists for C-State for all processors.
  
  This function configures Processor Feature Lists for C-State for all processors.

**/
VOID
CStateConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               AllProcessorsSupportedCState;
  EFI_STATUS            Status;

//
// PURLEY_OVERRIDE_BEGIN
//
  if ( (PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C_STATE_BIT) != 0) {
    mEnableCState = TRUE;
  }
//
// PURLEY_OVERRIDE_END
//

  AllProcessorsSupportedCState = mEnableCState;
  mEnableC1AutoDemotion = (BOOLEAN)((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C1_AUTO_DEMOTION_BIT) != 0);
  mEnableC3AutoDemotion = (BOOLEAN)((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C3_AUTO_DEMOTION_BIT) != 0);
  mEnableC1AutoUndemotion = (BOOLEAN)((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C1_AUTO_UNDEMOTION_BIT) != 0);
  mEnableC3AutoUndemotion = (BOOLEAN)((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C3_AUTO_UNDEMOTION_BIT) != 0);

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports C-State
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, CState, NULL)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of C-State capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C_STATE_BIT);
        ASSERT_EFI_ERROR (Status);
        //
        // Set the bit of C-State setting according to BSP setting.
        //
        if (mEnableCState) {
          Status = PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C_STATE_BIT);
          ASSERT_EFI_ERROR (Status);
        }

        if (GetProcessorFeatureCapability (ProcessorNumber, C1AutoDemotion, NULL)) {
          Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C1_AUTO_DEMOTION_BIT);
          ASSERT_EFI_ERROR (Status);
          if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C1_AUTO_DEMOTION_BIT) != 0) {
            Status = PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C1_AUTO_DEMOTION_BIT);
            ASSERT_EFI_ERROR (Status);
          }
        }
        if (GetProcessorFeatureCapability (ProcessorNumber, C3AutoDemotion, NULL)) {
          Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C3_AUTO_DEMOTION_BIT);
          ASSERT_EFI_ERROR (Status);
          if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C3_AUTO_DEMOTION_BIT) != 0) {
            Status = PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C3_AUTO_DEMOTION_BIT);
            ASSERT_EFI_ERROR (Status);
          }
        }
        if (GetProcessorFeatureCapability (ProcessorNumber, C1AutoUndemotion, NULL)) {
          Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C1_AUTO_UNDEMOTION_BIT);
          ASSERT_EFI_ERROR (Status);
          if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C1_AUTO_UNDEMOTION_BIT) != 0) {
            Status = PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C1_AUTO_UNDEMOTION_BIT);
            ASSERT_EFI_ERROR (Status);
          }
        }
        if (GetProcessorFeatureCapability (ProcessorNumber, C3AutoUndemotion, NULL)) {
          Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C3_AUTO_UNDEMOTION_BIT);
          ASSERT_EFI_ERROR (Status);
          if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C3_AUTO_UNDEMOTION_BIT) != 0) {
            Status = PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C3_AUTO_UNDEMOTION_BIT);
            ASSERT_EFI_ERROR (Status);
          }
        }
      }

      //
      // If this logical processor supports C-State, then add feature into feature list for operation
      // on corresponding register.
      //
      AppendProcessorFeatureIntoList (ProcessorNumber, CState, &mEnableCState);
    } else {
      //
      // Not all processors support C-State.
      //
      AllProcessorsSupportedCState = FALSE;
    }
  }

  //
  // Check if all APs could be placed in Mwait-Loop mode.
  //
  if (PcdGet8 (PcdCpuApLoopMode) == ApInMwaitLoop) {
    if (AllProcessorsSupportedCState) {
      mExchangeInfo->ApLoopMode = ApInMwaitLoop;
    } else {
      DEBUG ((EFI_D_WARN,"AP can not be placed in Mwait-Loop mode since not all processors' C-State are supported by platform.\n"));
    }
  }
}

/**
  Produces entry of C-State feature in Register Table for specified processor.
  
  This function produces entry of C-State feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
CStateReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64      MsrValue;
  UINTN       MaxCState;

  Enable = (BOOLEAN *) Attribute;
  if (*Enable) {
    //
    // For Native MWAIT and IO Redirection Support
    // Enable IO MWAIT translation for processor core C-state control.
    // Also limit the package to the max supported C-state.
    //
    MsrValue = AsmReadMsr64 (MSR_PMG_CST_CONFIG_CONTROL);
    MsrValue &= ~((UINT64)B_MSR_PACKAGE_C_STATE_LIMIT_MASK);
    MsrValue |= (UINT64)(PcdGet8 (PcdCpuPackageCStateLimit) & B_MSR_PACKAGE_C_STATE_LIMIT_MASK);
//
// PURLEY_OVERRIDE_BEGIN
//
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MONITOR_MWAIT_BIT)) {
      MsrValue |= B_MSR_IO_MWAIT_REDIRECTION_ENABLE;
    }
//
// PURLEY_OVERRIDE_END
//
    if (GetProcessorFeatureCapability (ProcessorNumber, C1AutoDemotion, NULL) &&
      mEnableC1AutoDemotion) {
      MsrValue |= B_MSR_C1_STATE_AUTO_DEMOTION_ENABLE;
    } else {
      MsrValue &= ~((UINT64)B_MSR_C1_STATE_AUTO_DEMOTION_ENABLE);
    }

    if (GetProcessorFeatureCapability (ProcessorNumber, C3AutoDemotion, NULL) &&
      mEnableC3AutoDemotion) {
      MsrValue |= B_MSR_C3_STATE_AUTO_DEMOTION_ENABLE;
    } else {
      MsrValue &= ~((UINT64)B_MSR_C3_STATE_AUTO_DEMOTION_ENABLE);
    }

    if (GetProcessorFeatureCapability (ProcessorNumber, C1AutoUndemotion, NULL) &&
      mEnableC1AutoUndemotion) {
      MsrValue |= B_MSR_C1_STATE_AUTO_UNDEMOTION_ENABLE;
    } else {
      MsrValue &= ~((UINT64)B_MSR_C1_STATE_AUTO_UNDEMOTION_ENABLE);
    }

    if (GetProcessorFeatureCapability (ProcessorNumber, C3AutoUndemotion, NULL) &&
      mEnableC3AutoUndemotion) {
      MsrValue |= B_MSR_C3_STATE_AUTO_UNDEMOTION_ENABLE;
    } else {
      MsrValue &= ~((UINT64)B_MSR_C3_STATE_AUTO_UNDEMOTION_ENABLE);
    }

    WriteRegisterTable (ProcessorNumber, Msr, MSR_PMG_CST_CONFIG_CONTROL, 0, 64, MsrValue);

    //
    // For IO Redirection Support
    // Tell processor that 2 C-states (besides C1) are supported (C2 and C3).
    // These correspond to LVL_2 and LVL_3.  Setup LVL_2 base address.
    //
    // Enable the MAX C-State as we can, use _CST to report the correct setup in processor setup page
    //
//
// PURLEY_OVERRIDE_BEGIN
//
    if (PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MONITOR_MWAIT_BIT)
    {
    MaxCState = mMaxCStateValue[ProcessorNumber];
      MsrValue = AsmReadMsr64 (MSR_PMG_IO_CAPTURE_BASE);
      if((MaxCState == CST_C3)|| (MaxCState == CST_C6) ||(MaxCState == CST_C7)) {
      MsrValue |= (UINT64)((MaxCState - 2) << 16);
      }
    MsrValue |= (UINT64)PcdGet16 (PcdCpuAcpiLvl2Addr);
      WriteRegisterTable (ProcessorNumber, Msr, MSR_PMG_IO_CAPTURE_BASE, 0, 64, MsrValue);
    }
  }
}
//
// PURLEY_OVERRIDE_END
//