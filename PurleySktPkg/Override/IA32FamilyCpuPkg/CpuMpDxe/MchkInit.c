/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Machine Check initialization

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  MchkInit.c

**/

#include "MchkInit.h"

MACHINE_CHECK_ATTRIBUTE *mMachineCheckAttribute;

/**
  Allocate buffer for Machine Check feature configuration.
  
**/
VOID
MachineCheckAllocateBuffer (
  VOID
  )
{
  mMachineCheckAttribute = (MACHINE_CHECK_ATTRIBUTE *)AllocatePool (
                                                        sizeof (MACHINE_CHECK_ATTRIBUTE) * mCpuConfigContextBuffer.NumberOfProcessors
                                                        );
  ASSERT (mMachineCheckAttribute != NULL);
}

/**
  Detect capability of machine check for specified processor.
  
  This function detects capability of machine check for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MachineCheckDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER       *CpuidRegisters;
  UINT32                   RegEdx;
  UINT64                   Value;
//
// PURLEY_OVERRIDE_BEGIN
//
struct mcBankTableEntry mcBankTable[] = {
    //
    // List the core MC Banks
    //
    {0,  IFU_CTL},  // IFU_CTL
    {1,  DCU_CTL},  // DCU_CTL
    {2,  DTLB_CTL}, // DTLB_CTL
    {3,  MLC_CTL}   // MLC_CTL
};

  UINT16  noOfCoreMcBanks = sizeof(mcBankTable)/sizeof(struct mcBankTableEntry);
  UINT8   mcIndex;
  UINT32  packageNumber;
  UINT32  threadNumber;
  union {
    UINT64		  data64;
    UINT64_STRUCT dataHiLow;
  } tempData;
//
// PURLEY_OVERRIDE_END
//
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);
  RegEdx         = CpuidRegisters->RegEdx;

  //
  // Read CPUID(1).EDX[7] for MCE capability
  //
  if (BitFieldRead32 (RegEdx, N_CPUID_MCE_SUPPORT, N_CPUID_MCE_SUPPORT) == 1) { 
    //
    // Read CPUID(1).EDX[14] for MCA capability
    //
    if (BitFieldRead32 (RegEdx, N_CPUID_MCA_SUPPORT, N_CPUID_MCA_SUPPORT) == 1) {
      mMachineCheckAttribute[ProcessorNumber].Mca = TRUE;
      //
      // Check MSR IA32_MCG_CAP[8] for the presence of IA32_MCG_CTL MSR
      //
      Value = AsmReadMsr64 (EFI_IA32_MCG_CAP);
      mMachineCheckAttribute[ProcessorNumber].ControlMsrPresent = FALSE;
      if (BitFieldRead64 (Value, N_MSR_MCG_CTL_P, N_MSR_MCG_CTL_P) == 1) {
        mMachineCheckAttribute[ProcessorNumber].ControlMsrPresent = TRUE;
      }
      mMachineCheckAttribute[ProcessorNumber].NumberOfErrorReportingBank = (UINTN) BitFieldRead64 (Value, N_MSR_MCG_COUNT_START, N_MSR_MCG_COUNT_STOP);
//
// PURLEY_OVERRIDE_BEGIN
//
	  GetProcessorLocation    (ProcessorNumber, &packageNumber, NULL, &threadNumber);
      //
	  // Only need to read one thread per core
	  //

      if (!(threadNumber & 0x01)) {
        for (mcIndex = 0; mcIndex < noOfCoreMcBanks; mcIndex++) {
      
          tempData.data64 = AsmReadMsr64(mcBankTable[mcIndex].mcBankAddress + MC_BANK_STATUS_REG);

          //
          // Skip if there is no fatal error / log not valid / not enabled
          //
          if ((tempData.dataHiLow.hi & (MSR_LOG_VALID | MSR_LOG_UC | MSR_LOG_EN)) != (MSR_LOG_VALID | MSR_LOG_UC | MSR_LOG_EN)) continue;
            //
            // Capture log information
            //
          MachineCheckAttribute[ProcessorNumber].McReg[mcIndex].socketId   = (UINT8)packageNumber;
          MachineCheckAttribute[ProcessorNumber].McReg[mcIndex].mcBankNum  = mcBankTable[mcIndex].mcBankNum;
          MachineCheckAttribute[ProcessorNumber].McReg[mcIndex].mcStatus   = tempData.dataHiLow;
          tempData.data64 = AsmReadMsr64(mcBankTable[mcIndex].mcBankAddress + MC_BANK_ADDRESS_REG);
          MachineCheckAttribute[ProcessorNumber].McReg[mcIndex].mcAddress  = tempData.dataHiLow;
          tempData.data64 = AsmReadMsr64(mcBankTable[mcIndex].mcBankAddress + MC_BANK_MISC_REG);
          MachineCheckAttribute[ProcessorNumber].McReg[mcIndex].mcMisc     = tempData.dataHiLow;
        } // mcbank loop
      } // Only even threads
//
// PURLEY_OVERRIDE_END
//
    } else {
      mMachineCheckAttribute[ProcessorNumber].Mca = FALSE;
    }
    SetProcessorFeatureCapability (ProcessorNumber, MachineCheck, &(mMachineCheckAttribute[ProcessorNumber]));
  } else {
    ClearProcessorFeatureCapability (ProcessorNumber, MachineCheck);
  }
}

/**
  Configures Processor Feature Lists for machine check for all processors.
  
  This function configures Processor Feature Lists for machine check for all processors.

**/
VOID
MachineCheckConfigFeatureList (
  VOID
  )
{
  VOID                 *Attribute;
  UINTN                ProcessorNumber;
  BOOLEAN              UserConfigurationSet;
  EFI_STATUS           Status;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MACHINE_CHECK_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports MCE
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, MachineCheck, &Attribute)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of machine check capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_MACHINE_CHECK_BIT);
        ASSERT_EFI_ERROR (Status);
        //
        // Set the bit of machine check setting according to BSP setting
        //
        if (UserConfigurationSet) {
          Status = PcdSet32S (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_MACHINE_CHECK_BIT);
          ASSERT_EFI_ERROR (Status);
        }
      }
      //
      // Add node into feature list.
      //
      if (UserConfigurationSet) {
        AppendProcessorFeatureIntoList (ProcessorNumber, MachineCheck, Attribute);
      }
    }
  }
}

/**
  Produces entry of machine check in Register Table for specified processor.
  
  This function produces entry of machine check in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Machine check Attribute.

**/
VOID
MachineCheckReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  MACHINE_CHECK_ATTRIBUTE    *McAttribute;
  UINTN                      Index;
  UINT32                     FamilyId;
  UINT32                     ModelId;
  UINT8                      StartIndex;

  //ESS Override start

  WA_InitializeLLC (ProcessorNumber);

  //ESS Override End

  McAttribute = (MACHINE_CHECK_ATTRIBUTE *) Attribute;

  if (McAttribute->Mca) {
    //
    // If IA32_MCG_CTL register is present, enable all MCA features
    //
    if (McAttribute->ControlMsrPresent) {
      WriteRegisterTable (ProcessorNumber, Msr, EFI_IA32_MCG_CTL, 0, 64, 0xffffffffffffffffULL);
    }

    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

    //
    // Enable logging of all errors.
    // For 0x06 Family and Model < 1Ah, exclude IA32_MC0_CTL register. Else, include IA32_MC0_CTL register.
    //
    if (FamilyId == 0x6 && ModelId < 0x1A) {
      StartIndex = 1;
    } else {
      StartIndex = 0;
    }
    for (Index = StartIndex; Index < McAttribute->NumberOfErrorReportingBank; Index++) {
      WriteRegisterTable (
        ProcessorNumber,
        Msr,
        (UINT32) (EFI_IA32_MC0_CTL + Index * 4),
        0,
        64,
        0xffffffffffffffffULL
      );
    }

    //
    // If power-on reset, clear all errors in status registers
    //
    if (PcdGetBool (PcdIsPowerOnReset)) {
      for (Index = 0; Index < McAttribute->NumberOfErrorReportingBank; Index++) {
        WriteRegisterTable (
          ProcessorNumber,
          Msr,
          (UINT32) (EFI_IA32_MC0_STATUS + Index * 4),
          0,
          64,
          0x0
          );
      }
    }

    //
    // For Pentium 4 processor family, enable initiator MCERR#, internal MCERR#, and BINIT#
    //
    if (FeaturePcdGet (PcdCpuCedarMillFamilyFlag) || FeaturePcdGet (PcdCpuPrescottFamilyFlag)) {
      if (FamilyId == 0xF){
        WriteRegisterTable (ProcessorNumber, Msr, EFI_MSR_EBC_SOFT_POWERON, 4, 3, 0x0);
      }
    }
  }
  //
  // Set bit 6 of CR4 to enable MCE
  //
  WriteRegisterTable (ProcessorNumber, ControlRegister, 4, 6, 1, 1);
}
