/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file

  Code for Enhanced Intel Speedstep Technology

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Eist.c

**/

#include "Eist.h"

//
//EPSDIP_EIST_START
//
UINT8                               mMaxRatio = MAX_VALUE_UINT8;
//
//EPSDIP_EIST_END
//
GLOBAL_REMOVE_IF_UNREFERENCED EIST_CONFIG mEistConfig;

/**
  Detect capability of EIST feature for specified processor.

  This function detects capability of EIST feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
EistDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT64              Ia32MiscEnables;
  UINTN               MaxRatio;
  UINTN               MinRatio;
  UINT32              FamilyId;
  UINT32              ModelId;

  //
  // Processor feature flag checking
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);

  if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_EIST_SUPPORT, N_CPUID_EIST_SUPPORT) == 1) {
    //
    // Check whether ratio range is valid
    //
    MaxRatio = GET_CPU_MISC_DATA (ProcessorNumber, MaxCoreToBusRatio);
    MinRatio = GET_CPU_MISC_DATA (ProcessorNumber, MinCoreToBusRatio);

    if (MaxRatio > MinRatio) {
      SetProcessorFeatureCapability (ProcessorNumber, Eist, NULL);
    }

    //
    // Check to see if Turbo Mode is supported on this processor.
    //
    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);
    if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSilvermontFamilyFlag) && IS_SILVERMONT_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuGoldmontFamilyFlag) && IS_GOLDMONT_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))
        ) {
      Ia32MiscEnables = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
      CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_SIGNATURE);
      ASSERT (CpuidRegisters != NULL);
      if (CpuidRegisters->RegEax >= 6) {
        CpuidRegisters = GetProcessorCpuid (ProcessorNumber, 6);
        ASSERT (CpuidRegisters != NULL);
        //
        //  +----------------------------------------------------------------------------+
        //  | CPUID.(EAX=06):EAX[1] | MSR_IA32_MISC_ENABLE.bit[38] | Turbo Mode          |
        //  +-----------------------|------------------------------|---------------------+
        //  |          0            |       0                      |Not Available        |
        //  +-----------------------|------------------------------|---------------------+
        //  |          0            |       1                      |Available but hidden |
        //  +-----------------------|------------------------------|---------------------+
        //  |          1            |       0                      |Available and visible|
        //  +-----------------------|------------------------------|---------------------+
        //
        if ((BitFieldRead32 (CpuidRegisters->RegEax, N_CPUID_TURBO_MODE_AVAILABLE, N_CPUID_TURBO_MODE_AVAILABLE) == 1) ||
            (BitFieldRead64 (Ia32MiscEnables, N_MSR_TURBO_MODE_DISABLE, N_MSR_TURBO_MODE_DISABLE) == 1)) {
            SetProcessorFeatureCapability (ProcessorNumber, TurboMode, NULL);
        }
      }
    }
  }
}

/**
  Calculates the number of P-states using the algorithm in the Avoton BWG.

  @param  MaxRatio          P1 (Max Non-Turbo also known as Guar) BCLK to Core Ratio.
  @param  MinRatio          Pn (LFM) BCLK to Core Ratio.
  @param  TurboEnabled      1 if enabled, 0 if disabled.
  @param  PssLimit          Limit of number of P-states. ACPI limit is 16 P-states.

  @return Number of P-states to create entries for.

**/
UINTN
ComputeNumPstates (
  IN     UINTN         MaxRatio,
  IN     UINTN         MinRatio,
  IN     UINTN         TurboEnabled,
  IN     UINTN         PssLimit
  )
{
  UINTN                RatioRange;
  UINTN                MaxStates;
  UINTN                NumStates;

  RatioRange = MaxRatio - MinRatio;
  MaxStates = RatioRange + 1 + TurboEnabled;

  if (PssLimit < MaxStates) {
    NumStates = PssLimit;
  } else {
    NumStates = MaxStates;
  }
  if (PssLimit < (TurboEnabled + 2)) {
    NumStates = 0;
  }
  return NumStates;
}

/**
  Calculates a specific Pstate ratio using the algorithm in the Avoton BWG.

  @param  MaxRatio          P1 (Max Non-Turbo also known as Guar) BCLK to Core Ratio.
  @param  MinRatio          Pn (LFM) BCLK to Core Ratio.
  @param  TurboEnabled      1 if enabled, 0 if disabled.
  @param  NumStates         Limit of number of P-states. ACPI limit is 16 P-states.
  @param  Pstate            The current Pstate number.
  @param  MaxTurboRatio     Max Turbo Ratio.

  @return Specific P-states ratio.

**/
UINTN
ComputePstateRatio (
  IN     UINTN         MaxRatio,
  IN     UINTN         MinRatio,
  IN     UINTN         TurboEnabled,
  IN     UINTN         NumStates,
  IN     UINTN         Pstate,
  IN     UINTN         MaxTurboRatio
  )
{
  UINTN                RatioRange;
  UINTN                AdjPstate;
  UINTN                NumGaps;
  UINTN                PstateRatio;

  RatioRange = MaxRatio - MinRatio;
  NumGaps = (NumStates - 1) - TurboEnabled;
  AdjPstate = Pstate - TurboEnabled;
  
  if (Pstate == 0) {
    if (TurboEnabled == 1) {
      PstateRatio = MaxTurboRatio;
    } else {
      PstateRatio = MaxRatio;
    }  
  } else if (RatioRange == 0) {
    PstateRatio = MaxRatio;
  } else {
    PstateRatio = MaxRatio - (((AdjPstate * RatioRange) + (NumGaps / 2)) / NumGaps);
  }
  return PstateRatio;
}

/**
  Calculates a P-state VID using the VID slope equation in the Avoton BWG.

  @param  MaxRatio          P1 (Max Non-Turbo also known as Guar) BCLK to Core Ratio.
  @param  MinRatio          Pn (LFM) BCLK to Core Ratio.
  @param  TargetRatio       Specific P-states ratio. 
  @param  TurboEnabled      1 if enabled, 0 if disabled.
  @param  MaxVid            Max Vid value.
  @param  MinVid            Min Vid value.
  @param  Pstate            The current Pstate number.

  @return Specific P-state VID using the VID slope equation in the Avoton BWG.

**/
UINTN
ComputePstateVid (
  IN     UINTN         MaxRatio,
  IN     UINTN         MinRatio,
  IN     UINTN         TargetRatio,
  IN     UINTN         TurboEnabled,
  IN     UINTN         MaxVid,
  IN     UINTN         MinVid,
  IN     UINTN         Pstate
  )
{

  UINTN                TargetVid;  // Calculated VID for the TargetRatio
  UINTN                DeltaRatio; // MaxRatio - MinRatio
  UINTN                DeltaVid;   // MaxVid - MinVid
  UINTN                MiSlope;    // To stay with only integer math, this is the integer portion of deltaVID / deltaRATIO
  UINTN                MrSlope;    // Remainder of deltaVID / deltaRATIO
  UINTN                Xratio;     // Target ratio - min RATIO
  UINTN                Ymr;        // partial VID calculated from the slope remainder (MrSlope).

  if ((Pstate == 0) && (TurboEnabled == 1)) {
    //
    // Turbo is a special case that does not use the slope equation, read the VID directly
    //
    TargetVid = (UINTN) (AsmReadMsr64 (MSR_IACORE_TURBO_VIDS) & B_MSR_MAX_VID_1C_MASK);

  //
  // Make sure there is a delta ratio, otherwise a DIV 0 exception will be raised.
  // Return MaxVid if there is no delta between MaxRatio and MinRatio.
  //
  } else if (MaxRatio == MinRatio) {
    TargetVid = MaxVid;

  } else {  
    DeltaRatio = (MaxRatio - MinRatio);
    DeltaVid = (MaxVid - MinVid);
    
    //
    // To keep from bringing in the FPU library, determine the Integer portion of the slope "MiSlope"
    // and also determine the remainder portion of the slope "MrSlope".
    //
    MiSlope = DeltaVid / DeltaRatio;
    MrSlope = DeltaVid % DeltaRatio;
    Xratio = TargetRatio - MinRatio;

    TargetVid = (MiSlope * Xratio) + MinVid;            // Integer part of Slope * offset from min ratio
    Ymr = (MrSlope * Xratio) / DeltaRatio;      // decimal part of Slope * offset from min ratio

    //
    // If modulo division does not yield 0 remainder, then need to increment the 
    // target VID to emulate the CEILING(x) function.
    //
    if (((MrSlope * Xratio) % DeltaRatio) != 0) {
      Ymr++;                                    // Simulate CEILING function.  If a remainder, then round up.
    }

    TargetVid += Ymr;                                   // Sum of the Integer and Remainder values
  }

  return TargetVid;
}

/**
  Calculate the FVID table for all logical processors based on their CPU ID.

  @param TurboMode     Whether Turbo mode is enabled or not.
**/
VOID
CalculateFvidTable (
  IN BOOLEAN  TurboMode
  )
{
  UINTN            ProcessorNumber;
  UINTN            Index;

  UINTN            MaxRatio;
  UINTN            MinRatio;
  UINTN            RatioStepSize;
  UINTN            PreviousRatio;

  UINTN            MaxVid;
  UINTN            MinVid;
  UINTN            VidSlope;

  UINTN            NumberOfPStates;
  FVID_ENTRY       *FvidEntry;
  CPU_MISC_DATA    *CpuMiscData;

  UINT32           FamilyId;
  UINT32           ModelId;

  UINT64           PowerSlope;
  UINT64           RatioPower;
  UINT64           PowerStepSize;
  UINTN            CurrentRatio;
  UINTN            PackageTdp;
  UINTN            CoreTdp;
  UINTN            UncoreTdp;
  UINTN            TableIndex;
  UINTN            NumGaps;
  UINTN            RatioRange;
  UINT32           DebugOutputLevel;

  NumGaps = 0;

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Get the core to bus ratio and VID range for the processor
    //
    CpuMiscData = &mCpuConfigContextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
    MaxRatio    = CpuMiscData->MaxCoreToBusRatio;
    MinRatio    = CpuMiscData->MinCoreToBusRatio;
    MaxVid      = CpuMiscData->MaxVid;
    MinVid      = CpuMiscData->MinVid;
    PackageTdp  = CpuMiscData->PackageTdp;
    CoreTdp     = CpuMiscData->CoreTdp;
    RatioRange  = MaxRatio - MinRatio;

    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

    //
    // Algorithm for Prescott and Cedar Mill family
    //
    if (FeaturePcdGet (PcdCpuCedarMillFamilyFlag) || FeaturePcdGet (PcdCpuPrescottFamilyFlag)) {
      if (FamilyId == 0x0F && (ModelId == 0x03 || ModelId == 0x04 || ModelId == 0x06)) {
        //
        // For Pentium 4 family, we just support 2 P-states
        //
        CpuMiscData->NumberOfPStates = 2;

        //
        // Allocate memory for FVID table.
        //
        CpuMiscData->FvidTable = AllocatePool (2 * sizeof (FVID_ENTRY));
        ASSERT (CpuMiscData->FvidTable != NULL);

        //
        // Calculate the tuple for the max ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[0]);
        FvidEntry->Ratio = MaxRatio;
        FvidEntry->Vid   = MaxVid;
        FvidEntry->Power = 86 * 1000;

        //
        // Calculate the tuple for the min ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[1]);
        FvidEntry->Ratio = MinRatio;
        FvidEntry->Vid   = MinVid;
        FvidEntry->Power = ((5 * MinRatio + 5 * MinRatio) * (MinVid + 0x42) * (MinVid + 0x42) * 86 * 1000)
                           / (10 * MaxRatio * (MaxVid + 0x42) * (MaxVid + 0x42));
      }
    }

    //
    // Algorithm for Core 2 family
    //
    if (FeaturePcdGet (PcdCpuConroeFamilyFlag)) {
      if (FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) {
        //
        // Calculate the vid slope multiply by 1000 to maintain accuracy
        // Later on, add 999 and divide by 1000 to get the ceiling
        //
        VidSlope = (MaxVid - MinVid) * 1000 / (MaxRatio - MinRatio);

        //
        // Determine the step
        //
        if ((MaxRatio - MinRatio) == 1) {
          RatioStepSize = 1;
        } else {
          RatioStepSize = 2;
        }

        //
        // Determine the number of P-states suppored by processor
        //
        NumberOfPStates = (MaxRatio - MinRatio) / RatioStepSize + 1;

        CpuMiscData->NumberOfPStates = NumberOfPStates;

        //
        // Allocate memory for FVID table.
        //
        CpuMiscData->FvidTable = AllocatePool (NumberOfPStates * sizeof (FVID_ENTRY));
        ASSERT(CpuMiscData->FvidTable != NULL);

        //
        // Calculate the tuple for the max ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[0]);
        FvidEntry->Ratio = MaxRatio;
        FvidEntry->Vid   = MaxVid;
        FvidEntry->Power = 65 * 1000;

        //
        // Calculate the tuple for the min ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[NumberOfPStates - 1]);
        FvidEntry->Ratio = MinRatio;
        FvidEntry->Vid   = MinVid;
        FvidEntry->Power = 32 * 1000;

        PreviousRatio = MinRatio;

        for (Index = NumberOfPStates - 2; Index > 0; Index--) {
          //
          // Calculate for the FVID tuple
          //
          FvidEntry = &(CpuMiscData->FvidTable[Index]);

          FvidEntry->Ratio = PreviousRatio + RatioStepSize;
          FvidEntry->Vid   = ((VidSlope * (FvidEntry->Ratio - MinRatio)) + 999) / 1000 + MinVid;
          //
          // If the calculated VID is greater than the max VID, then use the max VID
          //
          if (FvidEntry->Vid > MaxVid) {
            FvidEntry->Vid = MaxVid;
          }
          FvidEntry->Power = (FvidEntry->Ratio - MinRatio) * (65000 - 32000) / (MaxRatio - MinRatio) + 32000;

          PreviousRatio    = FvidEntry->Ratio;
        }
      }
    }

    //
    // Nehalem based processor support
    //
    if (FeaturePcdGet (PcdCpuNehalemFamilyFlag)) {
      if (IS_NEHALEM_PROC (FamilyId, ModelId)) {
        //
        // Set step size for this processor type.  Currently only a step size of
        // one is supported.
        //
        RatioStepSize = 1;

        //
        // Determine the Uncore TDP based on Package and Core information.
        //
        UncoreTdp = PackageTdp - CoreTdp;

        //
        // Compute the number of P-States.
        //
        CpuMiscData->NumberOfPStates = ((MaxRatio - MinRatio) / RatioStepSize) + 1;
        if (TurboMode) {
          CpuMiscData->NumberOfPStates += 1;
        }

        //
        // Allocate buffer to put P-State information in.
        //
        CpuMiscData->FvidTable = AllocatePool (CpuMiscData->NumberOfPStates * sizeof (FVID_ENTRY));
        ASSERT(CpuMiscData->FvidTable != NULL);

        //
        // Fill in Turbo Mode information if enabled.
        //
        TableIndex = 0;
        if (TurboMode) {
          CpuMiscData->FvidTable[TableIndex].Ratio = MaxRatio + 1;
          CpuMiscData->FvidTable[TableIndex].Vid = 0;
          CpuMiscData->FvidTable[TableIndex].TransitionLatency = 10;
          CpuMiscData->FvidTable[TableIndex].BusMasterLatency = 10;
          CpuMiscData->FvidTable[TableIndex].Power = PackageTdp;
          TableIndex++;
        }

        //
        // Fill in ratio information for non-Turbo ratios.
        //
        CurrentRatio = MaxRatio;
        for (;TableIndex <CpuMiscData->NumberOfPStates; TableIndex++, CurrentRatio--) {
          //
          // Fill in ratio data.  VID is ignored on these processors as it is controlled
          // by the PCU.
          //
          CpuMiscData->FvidTable[TableIndex].Ratio = CurrentRatio;
          CpuMiscData->FvidTable[TableIndex].Vid = 0;

          //
          // Processor specific latency information from BWG.
          //
          CpuMiscData->FvidTable[TableIndex].TransitionLatency = 10;
          CpuMiscData->FvidTable[TableIndex].BusMasterLatency = 10;

          //
          // Compute power slope.  Floating point math done using fixed point
          // computations (5 decimal places).
          //
          // M = ((1.1 - (MaxNonTurboRatio - Ratio) 0.00625) / 1.1)^2
          //
          PowerSlope = 110000 - (MultU64x32 ((MaxRatio - CurrentRatio), 625));
          PowerSlope = DivU64x32 (MultU64x32 (PowerSlope, 100), 110);
          PowerSlope = DivU64x32 (MultU64x64 (PowerSlope, PowerSlope), 100000);

          //
          // Compute power for this ratio.
          //
          RatioPower = DivU64x64Remainder (MultU64x32 (CurrentRatio, 100), MaxRatio, NULL);
          RatioPower = DivU64x32 (MultU64x64 (RatioPower, PowerSlope), 100);
          RatioPower = DivU64x32 (MultU64x64 (RatioPower, CoreTdp), 100000) + UncoreTdp;
          CpuMiscData->FvidTable[TableIndex].Power = (UINTN) RatioPower;
        }
      }
    }

    //
    // Sandy Bridge/Ivy Bridge/Haswell/Boradwell/Skylake/KNL based processor support
    //
    if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSilvermontFamilyFlag) && IS_VALLEYVIEW_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))||
        (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))
        ) {
 
      RatioStepSize = 1;

      //
      // Compute the number of P-States.
      //
      CpuMiscData->NumberOfPStates = ((MaxRatio - MinRatio) / RatioStepSize) + 1;
      if (TurboMode) {
        CpuMiscData->NumberOfPStates += 1;
      }

      if (IS_HASWELL_PROC (FamilyId, ModelId) || IS_BROADWELL_PROC (FamilyId, ModelId) ||
          IS_SKYLAKE_PROC (FamilyId, ModelId) || IS_KNIGHTSLANDING_PROC (FamilyId, ModelId)
          ) {
        if(CpuMiscData->NumberOfPStates > 16) {
          CpuMiscData->NumberOfPStates = 16;
        }
        NumGaps = CpuMiscData->NumberOfPStates - 1;
        if (TurboMode) {
          NumGaps -= 1;
        }
      } else {
        //
        // The ACPI specification strictly recommends no more than 16 performance states, so increase
        // the step size in this case.
        //
        while (CpuMiscData->NumberOfPStates > 16) {
          RatioStepSize += 1;
          CpuMiscData->NumberOfPStates = ((MaxRatio - MinRatio) / RatioStepSize) + 1;
          if (TurboMode) {
            CpuMiscData->NumberOfPStates += 1;
          }
        }
      }

      //
      // Allocate buffer to put P-State information in.
      //
      CpuMiscData->FvidTable = AllocatePool (CpuMiscData->NumberOfPStates * sizeof (FVID_ENTRY));
      ASSERT(CpuMiscData->FvidTable != NULL);

      //
      // According to BWG,
      // 1. If Turbo Mode is available and enabled, P(0) = Max Turbo Ratio, P(1) = Max Non-Turbo Ratio,
      //   else P(0) = Max Non-Turbo Ratio and P(1) is computed below based on RatioStepSize.
      // 2. P(n) = Min Non-Turbo Ration, and P(n-1) = P(n) + RatioStepSize.
      //

      //
      // Fill in Turbo Mode information if enabled.
      //
      Index = 0;
      if (TurboMode) {
        //
        // If the Turbo Mode is available and enabled, then the maximum performance state is
        // represented as read from the TURBO_RATIO_LIMIT MSR bits [7:0].
        //
        CpuMiscData->FvidTable[Index].Ratio = CpuMiscData->MaxTurboRatio;
        //
        // Operating Point Protection is always enabled on Sandy Bridge processors. The end result is
        // that BIOS can not configure the processor VID.
        //
        CpuMiscData->FvidTable[Index].Vid = 0;
        //
        // Processor specific latency information from BWG.
        //
        CpuMiscData->FvidTable[Index].TransitionLatency = 10;
        CpuMiscData->FvidTable[Index].BusMasterLatency = 10;
        CpuMiscData->FvidTable[Index].Power = PackageTdp;
        Index++;
      }

      //
      // TO-DO: Get factory-configured maximum Non-Turbo Ratio if FLEX RATIO is enabled.
      //

      //
      // Fill in ratio information for non-Turbo ratios.
      //
      CpuMiscData->FvidTable[Index].Ratio = MaxRatio;
      //
      // Operating Point Protection is always enabled on Sandy Bridge processors. The end result is
      // that BIOS can not configure the processor VID.
      //
      CpuMiscData->FvidTable[Index].Vid = 0;
      //
      // Processor specific latency information from BWG.
      //
      CpuMiscData->FvidTable[Index].TransitionLatency = 10;
      CpuMiscData->FvidTable[Index].BusMasterLatency = 10;
      CpuMiscData->FvidTable[Index].Power = PackageTdp;
      Index++;

      // BDE Override start
      CurrentRatio = MinRatio; //With this commented, KW report uninitialized variable path thru code.  Leave this here to elminate this issue.
//      for (TableIndex = CpuMiscData->NumberOfPStates - 1; TableIndex >= Index; TableIndex--, CurrentRatio += RatioStepSize) {
      for(TableIndex = Index; TableIndex < CpuMiscData->NumberOfPStates; TableIndex++) {
      // BDE Override end
        //
        // Fill in ratio data.  
        //
        if (IS_HASWELL_PROC (FamilyId, ModelId) || IS_BROADWELL_PROC (FamilyId, ModelId) ||
            IS_SKYLAKE_PROC (FamilyId, ModelId) || IS_KNIGHTSLANDING_PROC (FamilyId, ModelId)
            ) {
          ASSERT (NumGaps != 0);
          if(TurboMode) {
            CurrentRatio = MaxRatio - ((((TableIndex - 1) * RatioRange) + (NumGaps / 2)) / NumGaps);
          } else {
            CurrentRatio = MaxRatio - (((TableIndex * RatioRange) + (NumGaps / 2)) / NumGaps);
          }
        }
        CpuMiscData->FvidTable[TableIndex].Ratio = CurrentRatio;
        //
        // Operating Point Protection is always enabled on Sandy Bridge processors. The end result is
        // that BIOS can not configure the processor VID.
        //
        CpuMiscData->FvidTable[TableIndex].Vid = 0;

        //
        // Processor specific latency information from BWG.
        //
        CpuMiscData->FvidTable[TableIndex].TransitionLatency = 10;
        CpuMiscData->FvidTable[TableIndex].BusMasterLatency = 10;

        //
        // Compute power slope.  Floating point math done using fixed point
        // computations (5 decimal places).
        //
        // M = ((1.1 - (P1_ratio - Ratio) * 0.00625) / 1.1)^2
        //
        PowerSlope = 110000 - (MultU64x32 ((MaxRatio - CurrentRatio), 625));
        PowerSlope = DivU64x32 (MultU64x32 (PowerSlope, 100), 110);
        PowerSlope = DivU64x32 (MultU64x64 (PowerSlope, PowerSlope), 100000);

        //
        // pstate_power[ratio]=((ratio/P1_ratio)*M*TDP)
        //
        RatioPower = DivU64x64Remainder (MultU64x32 (CurrentRatio, 100), MaxRatio, NULL);
        RatioPower = DivU64x32 (MultU64x64 (RatioPower, PowerSlope), 100);
        RatioPower = DivU64x32 (MultU64x64 (RatioPower, PackageTdp), 100000);
        CpuMiscData->FvidTable[TableIndex].Power = (UINTN) RatioPower;
      }
    }
    
    //
    // Algorithm for TunnelCreek family
    //
    if (FeaturePcdGet (PcdCpuTunnelCreekFamilyFlag)) {
      if (IS_TUNNELCREEK_PROC(FamilyId, ModelId)) {
        //
        // Calculate the vid slope multiply by 1000 to maintain accuracy
        // Later on, add 999 and divide by 1000 to get the ceiling
        //
        VidSlope = (MaxVid - MinVid) * 1000 / (MaxRatio - MinRatio);

        //
        // Determine the step
        //
        if ((MaxRatio - MinRatio) == 1) {
          RatioStepSize = 1;
        } else {
          RatioStepSize = 2;
        }

        //
        // Determine the number of P-states suppored by processor
        //
        NumberOfPStates = (MaxRatio - MinRatio) / RatioStepSize + 1;
    
        CpuMiscData->NumberOfPStates = NumberOfPStates;

        //
        // Allocate memory for FVID table.
        //
        CpuMiscData->FvidTable = AllocatePool (NumberOfPStates * sizeof (FVID_ENTRY));
        ASSERT(CpuMiscData->FvidTable != NULL);

        //
        // Calculate the tuple for the max ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[0]);
        FvidEntry->Ratio = MaxRatio;
        FvidEntry->Vid   = MaxVid;
        FvidEntry->Power = 2 * 1000;

        //
        // Calculate the tuple for the min ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[NumberOfPStates - 1]);
        FvidEntry->Ratio = MinRatio;
        FvidEntry->Vid   = MinVid;
        FvidEntry->Power = 6 * 100;

        PreviousRatio = MinRatio;

        for (Index = NumberOfPStates - 2; Index > 0; Index--) {
          //
          // Calculate for the FVID tuple
          //
          FvidEntry = &(CpuMiscData->FvidTable[Index]);

          FvidEntry->Ratio = PreviousRatio + RatioStepSize;
          FvidEntry->Vid   = ((VidSlope * (FvidEntry->Ratio - MinRatio)) + 999) / 1000 + MinVid;
          //
          // If the calculated VID is greater than the max VID, then use the max VID
          //
          if (FvidEntry->Vid > MaxVid) {
            FvidEntry->Vid = MaxVid;
          }
          FvidEntry->Power = (FvidEntry->Ratio - MinRatio) * (2000 - 600) / (MaxRatio - MinRatio) + 600;

          PreviousRatio    = FvidEntry->Ratio;
        }
      }
    }

    if ((FeaturePcdGet (PcdCpuSilvermontFamilyFlag) && IS_AVOTON_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuGoldmontFamilyFlag) && IS_GOLDMONT_PROC (FamilyId, ModelId))) {
      //
      // Compute the number of P-States.
      //
      CpuMiscData->NumberOfPStates = ComputeNumPstates (MaxRatio, MinRatio, (UINTN)TurboMode, 16);
      //
      // Allocate memory for FVID table.
      //
      CpuMiscData->FvidTable = AllocatePool (CpuMiscData->NumberOfPStates * sizeof (FVID_ENTRY));
      ASSERT(CpuMiscData->FvidTable != NULL);
    
      //
      // Until the Power Management Task Force provides actual power equations, use the below fix value ranges
      // of 10w for max Non-Turbo Ratio and 1w for LFM ratio - regardless of max Non-Turbo Ratio and BCLK frequency.
      // Below 10w = 10000mw and 1w = 1000mw.
      //
      PowerStepSize = (10000 - 1000) / (MaxRatio - MinRatio);

      for (TableIndex = CpuMiscData->NumberOfPStates - 1; (INTN)TableIndex >= 0; TableIndex--) {
        CurrentRatio = ComputePstateRatio (MaxRatio, MinRatio, (UINTN)TurboMode, CpuMiscData->NumberOfPStates, TableIndex, CpuMiscData->MaxTurboRatio);
        CpuMiscData->FvidTable[TableIndex].Ratio  = CurrentRatio;
        if (IS_AVOTON_PROC (FamilyId, ModelId)) {
          CpuMiscData->FvidTable[TableIndex].Vid = ComputePstateVid (MaxRatio, MinRatio, CurrentRatio, (UINTN)TurboMode, MaxVid, MinVid, TableIndex);
        } else {
          CpuMiscData->FvidTable[TableIndex].Vid = 0;
        }
        //
        // Processor specific latency information from BWG.
        //
        CpuMiscData->FvidTable[TableIndex].TransitionLatency = 10;
        CpuMiscData->FvidTable[TableIndex].BusMasterLatency = 10;
        //
        // This is a temporary equation for power calculations.
        //
        CpuMiscData->FvidTable[TableIndex].Power = 1000 + ((UINTN)PowerStepSize * (CurrentRatio - MinRatio));                                                      
      }
    }
  }

  //
  // Print debug info for FVID table.
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
      DebugOutputLevel = EFI_D_INFO;
    } else {
      DebugOutputLevel = EFI_D_VERBOSE;
    }
    DEBUG ((DebugOutputLevel, "Proc: %8d\n", ProcessorNumber));
    CpuMiscData = &mCpuConfigContextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
    NumberOfPStates = CpuMiscData->NumberOfPStates;
    for (Index = 0; Index < NumberOfPStates; Index++) {
      FvidEntry = &(CpuMiscData->FvidTable[Index]);
      DEBUG ((DebugOutputLevel, "  State: %8d\n", Index));
      DEBUG ((DebugOutputLevel, "    Ratio: %8d\n", FvidEntry->Ratio));
      DEBUG ((DebugOutputLevel, "    VID:   %8d\n", FvidEntry->Vid));
      DEBUG ((DebugOutputLevel, "    Power: %8d\n", FvidEntry->Power));
    }
  }
}

/**
  Configures Processor Feature Lists for EIST feature for all processors.

  This function configures Processor Feature Lists for EIST feature for all processors.

**/
VOID
EistConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               EistSupported;
  BOOLEAN               TurboSupported;
  BOOLEAN               EistSetting;
  BOOLEAN               TurboSetting;
  BOOLEAN               HwCoordinationSetting;
  EFI_STATUS            Status;
  CPU_MISC_DATA         *CpuMiscData;  //PURLEY_OVERRIDE_5372787

  EistSupported = TRUE;
  TurboSupported = TRUE;
  EistSetting = FALSE;
  TurboSetting = FALSE;
  HwCoordinationSetting = FALSE;

  //
  // Check whether all logical processors support EIST
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    if (!GetProcessorFeatureCapability (ProcessorNumber, Eist, NULL)) {
      EistSupported = FALSE;
      break;
    }
    if (!GetProcessorFeatureCapability (ProcessorNumber, TurboMode, NULL)) {
      TurboSupported = FALSE;
    }
//
//PURLEY_OVERRIDE_5372787_BEGIN
//
    CpuMiscData = &mCpuConfigContextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
    if (mMaxRatio > (UINT8) CpuMiscData->MaxCoreToBusRatio) {
      mMaxRatio   = (UINT8) CpuMiscData->MaxCoreToBusRatio;
    }
//
//PURLEY_OVERRIDE_5372787_END
//
  }

  if (EistSupported) {
    //
    // Set the bit of EIST capability
    //
    Status = PcdSet32S (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_EIST_BIT);
    ASSERT_EFI_ERROR (Status);
    //
    // Set the bit of Turbo Mode capability
    //
    if (TurboSupported) {
      Status = PcdSet32S (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_TURBO_MODE_BIT);
      ASSERT_EFI_ERROR (Status);
    }

    //
    // Checks whether user indicates to enable EIST
    //
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_EIST_BIT) != 0) {
      //
      // Set the bit of current setting for EIST
      //
      Status = PcdSet32S (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_EIST_BIT);
      ASSERT_EFI_ERROR (Status);
      EistSetting = TRUE;

      //
      // Set the bit of current setting for Turbo Mode
      //
      if (TurboSupported && (PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_TURBO_MODE_BIT) != 0) {
        Status = PcdSet32S (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_TURBO_MODE_BIT);
        ASSERT_EFI_ERROR (Status);
        TurboSetting = TRUE;
      }

      HwCoordinationSetting = PcdGetBool (PcdCpuHwCoordination);

      //
      // Calculate the FVID table for all logical processors.
      //
      CalculateFvidTable (TurboSetting);

    }
//
// PURLEY_OVERRIDE_5372787_BEGIN
//
    //
    // Only append Feature into list if EIST is supported on this processor
    //
    mEistConfig.EnableEist = EistSetting;
    mEistConfig.EnableTurbo = TurboSetting;
    mEistConfig.EnableHwCoordination = HwCoordinationSetting;
    for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
      AppendProcessorFeatureIntoList (ProcessorNumber, Eist, &mEistConfig);
    }
//
// PURLEY_OVERRIDE_5372787_END
//
  }

}

/**
  Produces entry of EIST feature in Register Table for specified processor.

  This function produces entry of EIST feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
EistReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  EIST_CONFIG  *Enable;
  UINT64       Value;
  UINT32       FamilyId;
  UINT32       ModelId;
  UINT64       Ia32MiscEnables;

  //
  // If EIST is TRUE, then write 1 to MSR_IA32_MISC_ENABLE[16].
  // Otherwise, write 0 to the bit.
  //
  Enable = (EIST_CONFIG *) Attribute;
  Value  = 0;
  if (Enable->EnableEist) {
    Value = 1;
  }

//
// PURLEY_OVERRIDE_5372787_BEGIN
//
  //
  // The processor is equipped with EIST feature, but the user wish to disable it
  // In this situation BIOS should make the processors run at 
  //   max none turbo freq followed by disabling EIST
  //
  if (!Enable->EnableEist) {
    SetMaxNonTurboPReqFrequency(ProcessorNumber);
  }
//
// PURLEY_OVERRIDE_5372787_END
//

  WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_EIST_ENABLE, 1, Value);

  if ((PcdGet32 (PcdCpuProcessorFeatureCapability) & PCD_CPU_TURBO_MODE_BIT) != 0) {
    Value  = 1;
    if (Enable->EnableTurbo) {
      Value = 0;
    }
    Ia32MiscEnables = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
    if (BitFieldRead64 (Ia32MiscEnables, N_MSR_TURBO_MODE_DISABLE, N_MSR_TURBO_MODE_DISABLE) != Value) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_TURBO_MODE_DISABLE, 1, Value);
    }
  }

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  //
  // For Conroe family, we need to set EIST Select Lock bit after programming EIST
  //
  if (FeaturePcdGet (PcdCpuConroeFamilyFlag)) {
    if (FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16 || ModelId == 0x17)) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_EIST_SELECT_LOCK, 1, 1);
    }
  }
  
  //
  // For Tunnel Creek family, we need to set EIST Select Lock bit after programming EIST
  //
  if (FeaturePcdGet (PcdCpuTunnelCreekFamilyFlag)) {
    if (IS_TUNNELCREEK_PROC(FamilyId, ModelId)) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_EIST_SELECT_LOCK, 1, 1);
    }
  }

  //
  // HW Coordination programming
  //
  Value  = 1;
  if (Enable->EnableHwCoordination) {
    Value = 0;
  }
  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuBroadwellFamilyFlag) && IS_BROADWELL_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId))
      ) {
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_MISC_PWR_MGMT,
      N_MSR_EIST_HW_COORDINATION_DISABLE,
      1,
      Value
      );
  } else if (FeaturePcdGet (PcdCpuSilvermontFamilyFlag) && IS_VALLEYVIEW_PROC (FamilyId, ModelId)) {
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_PMG_CST_CONFIG_CONTROL,
      N_MSR_EIST_HARDWARE_COORDINATION_DISABLE,
      1,
      Value
      );
  }
}

//
//EPSDIP_EIST_START
//PURLEY_OVERRIDE_5372787_BEGIN
//
/**

  Set each processor to P-state running at max none turbo frequency.
  Then disable the EIST if previously disabled.

  @param  UINTN  ProcessorNumber - Processor index for current thread

  @retval None

**/
STATIC
VOID 
SetMaxNonTurboPReqFrequency (
  UINTN   ProcessorNumber
  )
{
  UINT64                                    Ia32MiscEnable;
  BOOLEAN                                   EistEnabled;

  //
  // Check whether EIST is enabled
  //
  Ia32MiscEnable = AsmReadMsr64 (MSR_IA32_MISC_ENABLES);
  EistEnabled    = (BOOLEAN) RShiftU64 ((Ia32MiscEnable & EIST_ENABLE), EIST_ENABLE_OFFSET);
  //
  //   Chapter 4 of Sandy Bridge Processor Family BIOS Writer's Guide (BWG)
  // Revision 1.6.0 (Document Number 504790) states the following:
  //   Incase where System BIOS wants to disable the Enhanced Intel Speedstep feature,
  // BIOS must switch the processor speed to maximum ratio before disabling EIST feature
  // to ensure the best performance the processor is capable.
  //
  if (!EistEnabled) {
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_IA32_MISC_ENABLES,
      EIST_ENABLE_OFFSET,
      1,
      1
      );
  }

  //
  // Set P-State Request to max ratio
  //
  WriteRegisterTable (
    ProcessorNumber,
    Msr,
    EFI_MSR_IA32_PERF_CTL,
    P_STATE_TARGET_OFFSET,
    8,
    mMaxRatio
    );

  //
  // Disable EIST if it was enabled above
  //
  if (!EistEnabled) {
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_IA32_MISC_ENABLES,
      EIST_ENABLE_OFFSET,
      1,
      0
      );
  }
}
//
//PURLEY_OVERRIDE_5372787_END
//EPSDIP_EIST_END
//
