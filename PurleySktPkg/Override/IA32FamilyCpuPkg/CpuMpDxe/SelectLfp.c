/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Selecting Least-Feature Processor as BSP

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  SelectLfp.c

**/

#include "MpService.h"
#include "Cpu.h"

//
// Common set of processors' feature flags.
//
UINT32    CommonFeatureFlagSet[6];
//
// Flags of whether corresponding processor matches common set.
//
BOOLEAN   *mMatchingCommonSet;

/**
  Get the common set of processors' feature flags.
  
  This function calculates the common set of processors' feature flags.
  Processor's feature flags include 6 segments: CPUID(1).ECX, CPUID(1).EDX,
  CPUID(0x80000001).EAX, CPUID(0x80000001).EBX, CPUID(0x80000001).ECX, 
  and CPUID(0x80000001).EDX. "AND" them together and result is the common set.

**/
VOID
GetCommonFeatureFlagSet (
  VOID
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINTN               ProcessorNumber;

  //
  // Initialize common set of feature flag with all "1"s.
  //
  SetMem32 (CommonFeatureFlagSet, sizeof (CommonFeatureFlagSet), 0xffffffff);

  //
  // Traverse all processors' feature flags
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {

    //
    // AND CPUID(1).ECX and CPUID(1).EDX with common set
    //
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
    ASSERT (CpuidRegisters != NULL);
    CommonFeatureFlagSet[0] &= CpuidRegisters->RegEcx;
    CommonFeatureFlagSet[1] &= CpuidRegisters->RegEdx;

    //
    // AND CPUID(0x80000001).EAX, CPUID(0x80000001).EBX, CPUID(0x80000001).ECX,
    // and CPUID(0x80000001).EDX with common set.
    //
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_EXTENDED_CPU_SIG);
    ASSERT (CpuidRegisters != NULL);
    CommonFeatureFlagSet[2] &= CpuidRegisters->RegEax;
    CommonFeatureFlagSet[3] &= CpuidRegisters->RegEbx;
    CommonFeatureFlagSet[4] &= CpuidRegisters->RegEcx;
    CommonFeatureFlagSet[5] &= CpuidRegisters->RegEdx;
  }
}

/**
  Checks which processors match the common set of feature flag
  
  This function Checks which processors match the common set of feature flag.

  @retval TRUE    At least one processor matches the common set of feature flag.
  @retval FALSE   No processor matches the common set of feature flag.

**/
BOOLEAN
CompareProcessorsWithCommonSet (
  VOID
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINTN               ProcessorNumber;
  BOOLEAN             ReturnValue;

  mMatchingCommonSet = (BOOLEAN *)AllocatePool (
                                    sizeof (BOOLEAN) * mCpuConfigContextBuffer.NumberOfProcessors
                                    );
  ASSERT (mMatchingCommonSet != NULL);

  ReturnValue = FALSE;
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    mMatchingCommonSet[ProcessorNumber] = FALSE;
    
    //
    // Compare CPUID(1).ECX and CPUID(1).EDX with common set
    // 
    CpuidRegisters = GetProcessorCpuid (0, EFI_CPUID_VERSION_INFO);
    ASSERT (CpuidRegisters != NULL);
    if (CommonFeatureFlagSet[0] != CpuidRegisters->RegEcx || CommonFeatureFlagSet[1] != CpuidRegisters->RegEdx) {
      continue;
    }

    //
    // Compare CPUID(0x80000001).EAX, CPUID(0x80000001).EBX, CPUID(0x80000001).ECX,
    // and CPUID(0x80000001).EDX with common set.
    //
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_EXTENDED_CPU_SIG);
    ASSERT (CpuidRegisters != NULL);
    if (CommonFeatureFlagSet[2] != CpuidRegisters->RegEax || CommonFeatureFlagSet[3] != CpuidRegisters->RegEbx ||
        CommonFeatureFlagSet[4] != CpuidRegisters->RegEcx || CommonFeatureFlagSet[5] != CpuidRegisters->RegEdx) {
      continue;
    }

    //
    // If the processor matches common set, flag it, and set return value as TRUE.
    //
    mMatchingCommonSet[ProcessorNumber] = TRUE;
    ReturnValue = TRUE;
  }

  return ReturnValue;
}

/**
  Select least-feature processor as BSP
  
  This function selects least-feature processor as BSP.

**/
VOID
SelectLfpAsBsp (
  VOID
  )
{
  UINTN               ProcessorNumber;
  UINTN               OldestProcessorNumber;
  UINT32              OldestProcessorFms[3];
  UINT32              FamilyId;
  UINT32              ModelId;
  UINT32              SteppingId;
  BOOLEAN             MatchingProcessorExist;
//ESS Override start
  UINT32              SbspSkt = 0xFF;
//ESS Override end

  //
  // Calculate the common set of processors' feature flags
  //
  GetCommonFeatureFlagSet ();

  //
  // Compare all logical processors with common set.
  //
  MatchingProcessorExist = CompareProcessorsWithCommonSet ();

//OVERRIDE_HSD_5370421_BEGIN
  OldestProcessorNumber = mCpuConfigContextBuffer.BspNumber;
  GetProcessorVersionInfo (OldestProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);
  if (!MatchingProcessorExist || mMatchingCommonSet[OldestProcessorNumber]) {
    //
    // If no processor matches common set or current BSP matches the common set,
    // set current BSP firstly. So if more than 1 processor match common set and with the 
    // same CPUID, the current one is BSP.
    //
    OldestProcessorFms[2] = FamilyId;
    OldestProcessorFms[1] = ModelId;
    OldestProcessorFms[0] = SteppingId;
  } else {
    OldestProcessorFms[2] = 0xFFFFFFFF;
    OldestProcessorFms[1] = 0;
    OldestProcessorFms[0] = 0;
  }
//OVERRIDE_HSD_5370421_END
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {

    //
    // If only 1 processor matches common set, it should be BSP
    // If more than 1 processors match common set, the one with lowest CPUID should be BSP.
    // If no processor matches common set, the processor with lowest CPUID should be BSP.
    //
    if (!MatchingProcessorExist || mMatchingCommonSet[ProcessorNumber]) {
      GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

      if (FamilyId > OldestProcessorFms[2]) {
        continue;
      }

      if (FamilyId == OldestProcessorFms[2]) {
        if (ModelId > OldestProcessorFms[1]) {
          continue;
        }
        if (ModelId == OldestProcessorFms[1]) {
          if (SteppingId >= OldestProcessorFms[0]) {
            continue;
          }
        }
      }

      OldestProcessorFms[2] = FamilyId;
      OldestProcessorFms[1] = ModelId;
      OldestProcessorFms[0] = SteppingId;
      OldestProcessorNumber = ProcessorNumber;
    }
  }

  FreePool (mMatchingCommonSet);
  //ESS Override start
  //check if user want to select SBSP
  //if yes, override
  SbspSkt = PcdGet8(PcdSbspSelection);
  if(SbspSkt != 0xFF) {          //if select SBSP
    OldestProcessorNumber = GetNewSbspProcessNumber(SbspSkt);
  }
  //ESS Override end

  //
  // If current BSP is not the least-feature processor, then switch BSP.
  //
  if (OldestProcessorNumber != mCpuConfigContextBuffer.BspNumber) {
    SwitchBSP (&mMpService, OldestProcessorNumber, TRUE);
    DEBUG ((EFI_D_ERROR, "BSP switched to processor: %8d\r\n", OldestProcessorNumber));
  }
}
