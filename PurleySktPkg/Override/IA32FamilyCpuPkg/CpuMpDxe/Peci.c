/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for PECI Feature

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Peci.c

**/

#include "Peci.h"
//
// PURLEY_OVERRIDE_BEGIN
//
#include <Library/PlatformHooksLib.h> //for IsSimicsPlatform() will remove when Simic fixIsSimicsPlatform
//
// PURLEY_OVERRIDE_END
//
BOOLEAN    mEnablePeci = FALSE;
//
// PURLEY_OVERRIDE_BEGIN
//
BOOLEAN    EnablePCIeDownStreamPECI[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
//
// PURLEY_OVERRIDE_END
//
/**
  Detect capability of PECI feature for specified processor.
  
  This function detects capability of PECI feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PeciDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;
  UINT64    MsrValue;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if (FamilyId == 0x06) {
    if (ModelId == 0x0F) {
      MsrValue = AsmReadMsr64 (MSR_EXT_CONFIG);
      if (SteppingId >= 0x02 && BitFieldRead64 (MsrValue, 22, 22) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
      }
    } else if (ModelId == 0x17 || ModelId == 0x16) {
      MsrValue = AsmReadMsr64 (MSR_EXT_CONFIG);
      if (BitFieldRead64 (MsrValue, 22, 22) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
      }
    } else if (IS_NEHALEM_PROC(FamilyId, ModelId)) {
      SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
    }
  }
}

/**
  Configures Processor Feature Lists for PECI feature for all processors.
  
  This function configures Processor Feature Lists for PECI feature for all processors.

**/
VOID
PeciConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  EFI_STATUS            Status;

  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_PECI_BIT) != 0) {
    mEnablePeci = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports PECI
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, Peci, NULL)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of PECI capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_PECI_BIT);
        ASSERT_EFI_ERROR (Status);
        //
        // Set the bit of PECI setting according to BSP setting.
        //
        if (mEnablePeci) {
          Status = PcdSet32S (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_PECI_BIT);
          ASSERT_EFI_ERROR (Status);
        }
      }

      //
      // If this logical processor supports PECI, then add feature into feature list for operation
      // on corresponding register.
      //
      AppendProcessorFeatureIntoList (ProcessorNumber, Peci, &mEnablePeci);
    }
  }
}

/**
  Produces entry of PECI feature in Register Table for specified processor.
  
  This function produces entry of PECI feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
PeciReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN   *Enable;
  UINT64    Value;
  UINT32    FamilyId;
  UINT32    ModelId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);
  
  //
  // If Attribute is TRUE, then write 1 to MSR_PECI_CONTROL[0].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  if (!IS_NEHALEM_PROC(FamilyId, ModelId)) {
    WriteRegisterTable (ProcessorNumber, Msr, MSR_PECI_CONTROL, 0, 1, Value);
  }
}


//
// PURLEY_OVERRIDE_BEGIN
//
/**
  Detect capability of PECI Downstream for specified processor.
  
  This function detects capability of PECI Downstream for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PCIeDownstreamDetect (
  UINTN   ProcessorNumber
  ){
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, PeciDownstreamWrite, NULL);
  }
}

/**
  Configures Processor Feature Lists for PECI Downstream for all processors.
  
  This function configures Processor Feature Lists for PECI Downstream for all processors.

**/
VOID
PCIeDownstreamFeatureList (
  VOID
  ){
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;
  EFI_STATUS            Status;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_PECI_DOWNSTREAM_WRITE_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports PCIeDownStreamPECI
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, PeciDownstreamWrite, NULL)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of PCIeDownStreamPECI capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_PECI_DOWNSTREAM_WRITE_BIT);
        ASSERT_EFI_ERROR(Status);
        //
        // Set the bit of PCIeDownStreamPECI setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          Status =  PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_PECI_DOWNSTREAM_WRITE_BIT);
          ASSERT_EFI_ERROR(Status);
        }
      }

      //
      // If this logical processor supports PCIeDownStreamPECI, then add feature into feature list for operation
      // on corresponding register.
      //
      EnablePCIeDownStreamPECI[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, PeciDownstreamWrite, &(EnablePCIeDownStreamPECI[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of PECI Downstream feature in Register Table for specified processor.
  
  This function produces entry of PECI Downstream feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
PCIeDownstreamReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  ){
  BOOLEAN   *Enable;
  UINT64    Value;
  UINT64    data;
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  if (IsSimicsPlatform() == TRUE) { //SKX TODO, will remove this when Simics fixed
    return;                         //also remove include header in this file and PlatformLib in inf
  }

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
    (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId) && (SteppingId >= 4)) ||  //if IVT C0 or later
    (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {

    // If Attribute is TRUE, then write 1 to MSR_PECI_DNSTRM_WRITE_EN[0].
    // Otherwise, write 0 to the bit.
    //
    Enable = (BOOLEAN *) Attribute;
    Value  = 0;
    data   = 0;
    if(*Enable) {
	    data |= BIT0; // PECI_CTRL.EnablePCIConfigWr 
	    Value = 1;
	    AsmWriteMsr64 (MSR_PECI_DNSTRM_WRITE_EN, data); 
	    WriteRegisterTable (ProcessorNumber, Msr, MSR_PECI_DNSTRM_WRITE_EN, 0, 1, Value);
    } else {
      data &= ~BIT0; // PECI_CTRL.EnablePCIConfigWr 
	    Value = 0;
	    AsmWriteMsr64 (MSR_PECI_DNSTRM_WRITE_EN, data); 
	    WriteRegisterTable (ProcessorNumber, Msr, MSR_PECI_DNSTRM_WRITE_EN, 0, 1, Value);
    }
  }
}
//
// PURLEY_OVERRIDE_END
//
