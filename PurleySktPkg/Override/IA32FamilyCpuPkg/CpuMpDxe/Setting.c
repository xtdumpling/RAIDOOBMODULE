/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Code for Setting phase.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Setting.c

**/

#include "Cpu.h"
#include "MpService.h"
#include "CpuOnlyReset.h"
//
// PURLEY_OVERRIDE_BEGIN
//
#include "CpuSncInit.h"
//
// PURLEY_OVERRIDE_END
//
BOOLEAN    mRestoreSettingAfterInit = FALSE;
BOOLEAN    mSetBeforeCpuOnlyReset;
//
// PURLEY_OVERRIDE_BEGIN
//
extern UINT8 mPcdMachineCheckEnable; //OVERIDE_HSD_5330628
//
// PURLEY_OVERRIDE_END
//
/**
  Programs registers for the calling processor.

  This function programs registers for the calling processor.

  @param  PreSmmInit         Specify the target register table.
                             If TRUE, the target is the pre-SMM-init register table.
                             If FALSE, the target is the post-SMM-init register table.
  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
SetProcessorRegisterEx (
  IN BOOLEAN  PreSmmInit,
  IN UINTN    ProcessorNumber
  )
{
  CPU_REGISTER_TABLE        *RegisterTable;
  CPU_REGISTER_TABLE_ENTRY  *RegisterTableEntry;
  UINTN                     Index;
  UINTN                     Value;
  UINTN                     StartIndex;
  UINTN                     EndIndex;

  if (PreSmmInit) {
    RegisterTable = &mCpuConfigContextBuffer.PreSmmInitRegisterTable[ProcessorNumber];
  } else {
    RegisterTable = &mCpuConfigContextBuffer.RegisterTable[ProcessorNumber];
  }
  
  StartIndex = 0;

//
// PURLEY_OVERRIDE_BEGIN
//
//
// PURLEY_OVERRIDE_END
//
  //
  // If microcode patch has been applied, then the first register table entry
  // is for microcode upate, so it is skipped.  
  //
  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    if (GET_CPU_MISC_DATA (ProcessorNumber, MicrocodeRevision) != 0) {
//
// PURLEY_OVERRIDE_BEGIN
//
      if (RegisterTable->TableLength > 0) {
        RegisterTableEntry = &RegisterTable->RegisterTableEntry[StartIndex];
        if (RegisterTableEntry->RegisterType == Msr && RegisterTableEntry->Index == EFI_MSR_IA32_BIOS_UPDT_TRIG) {
          StartIndex += 1;
        }
//
// PURLEY_OVERRIDE_END
//
      }
    }
  }

  if (mSetBeforeCpuOnlyReset) {
    EndIndex    = StartIndex + RegisterTable->NumberBeforeReset;
  } else {
    StartIndex += RegisterTable->NumberBeforeReset;
    EndIndex    = RegisterTable->TableLength;
  }

  //
  // Traverse Register Table of this logical processor
  //
  for (Index = StartIndex; Index < EndIndex; Index++) {

    RegisterTableEntry = &RegisterTable->RegisterTableEntry[Index];
    
    //
    // Check the type of specified register
    //
    switch (RegisterTableEntry->RegisterType) {
    //
    // The specified register is Control Register
    //
    case ControlRegister:
      switch (RegisterTableEntry->Index) {
      case 0:
        Value = AsmReadCr0 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr0 (Value);
        break;
      case 2:
        Value = AsmReadCr2 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr2 (Value);
        break;
      case 3:
        Value = AsmReadCr3 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr3 (Value);
        break;
      case 4:
        Value = AsmReadCr4 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr4 (Value);
        break;
      case 8:
        //
        //  Do we need to support CR8?
        //
        break;
      default:
        break;
      }
      break;
    //
    // The specified register is Model Specific Register
    //
    case Msr:
      //
	  // SKX PO HSD: 5330357 to workaround MC Bank bug Start
      // 5330861 SKX_TEMP_SKIP_EXT
      //
      if ( (!mPcdMachineCheckEnable) && (RegisterTableEntry->Index >= 0x400) && (RegisterTableEntry->Index <= 0x44C)) { //is MC MSR
		break;
	  }
      //SKX PO HSD: 5330357 to workaround MC Bank bug End
      //
      // If this function is called to restore register setting after INIT signal,
      // there is no need to restore MSRs in register table.
      //
      if (!mRestoreSettingAfterInit) {
        //
        // Get lock to avoid Package/Core scope MSRs programming issue in parallel execution mode
        //
        AcquireSpinLock (&mMPSystemData.MsrLock);
        if (RegisterTableEntry->ValidBitLength >= 64) {
          //
          // If length is not less than 64 bits, then directly write without reading
          //
          AsmWriteMsr64 (
            RegisterTableEntry->Index,
            RegisterTableEntry->Value
            );
        } else {
          //
          // Set the bit section according to bit start and length
          //
          AsmMsrBitFieldWrite64 (
            RegisterTableEntry->Index,
            RegisterTableEntry->ValidBitStart,
            RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
            RegisterTableEntry->Value
            );
        }
        ReleaseSpinLock (&mMPSystemData.MsrLock);
      }
      break;
    //
    // Enable or disable cache
    //
    case CacheControl:
      //
      // If value of the entry is 0, then disable cache.  Otherwise, enable cache.
      //
      if (RegisterTableEntry->Value == 0) {
        AsmDisableCache ();
      } else {
        AsmEnableCache ();
      }
      break;

    default:
      break;
    }
  }
}

/**
  Programs registers after SMM initialization for the calling processor.

  This function programs registers after SMM initialization for the calling processor.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
SetProcessorRegister (
  IN UINTN  ProcessorNumber
  )
{
  SetProcessorRegisterEx (FALSE, ProcessorNumber);
}

/**
  Programs registers before SMM initialization for the calling processor.

  This function programs registers before SMM initialization for the calling processor.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
SetPreSmmInitProcessorRegister (
  IN UINTN  ProcessorNumber
  )
{
  SetProcessorRegisterEx (TRUE, ProcessorNumber);
}

/**
  Triggers CPU-only reset and restores processor environment.

  This function triggers CPU-only reset and restores processor environment.

**/
VOID
CpuOnlyResetAndRestore (
  VOID
  )
{
  MTRR_SETTINGS  MtrrSetting;

  MtrrGetAllMtrrs (&MtrrSetting);

  InitiateCpuOnlyReset ();

  DispatchAPAndWait (
    TRUE,
    0,
    EarlyMpInit
    );

  EarlyMpInit (mCpuConfigContextBuffer.BspNumber);

  ProgramVirtualWireMode ();
}

/**
  Programs processor registers according to register tables.

  This function programs processor registers according to register tables.

**/
VOID
SettingPhase (
  VOID
  )
{
  UINT8                     CallbackSignalValue;
  UINTN                     Index;
  UINTN                     ProcessorNumber;
  CPU_REGISTER_TABLE        *RegisterTable;
  BOOLEAN                   NeedCpuOnlyReset;
  EFI_STATUS                Status;

  //
  // Set PcdCpuCallbackSignal to trigger callback function, and reads the value back.
  //
  CallbackSignalValue = SetAndReadCpuCallbackSignal (CPU_PROCESSOR_SETTING_SIGNAL);
  //
  // Checks whether the callback function requests to bypass Setting phase.
  //
  if (CallbackSignalValue == CPU_BYPASS_SIGNAL) {
    return;
  }

  //
  // Check if CPU-only reset is needed
  //
  NeedCpuOnlyReset = FALSE;
  for (Index = 0; Index < mCpuConfigContextBuffer.NumberOfProcessors; Index++) {
    RegisterTable = &mCpuConfigContextBuffer.RegisterTable[Index];
    if (RegisterTable->NumberBeforeReset > 0) {
      NeedCpuOnlyReset = TRUE;
      break;
    }
  }

  //
  // if CPU-only reset is needed, then program corresponding registers, and
  // trigger CPU-only reset.
  //
  if (NeedCpuOnlyReset) {
    mSetBeforeCpuOnlyReset = TRUE;
    DispatchAPAndWait (
      TRUE,
      0,
      SetProcessorRegister
      );

    SetProcessorRegister (mCpuConfigContextBuffer.BspNumber);

    CpuOnlyResetAndRestore ();
  }

  //
  // Program processors' registers in sequential mode.
  //
  mSetBeforeCpuOnlyReset = FALSE;
  for (Index = 0; Index < mCpuConfigContextBuffer.NumberOfProcessors; Index++) {
    
    ProcessorNumber = mCpuConfigContextBuffer.SettingSequence[Index];

    if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
      SetProcessorRegister (ProcessorNumber);
    } else {

      DispatchAPAndWait (
        FALSE,
        GET_CPU_MISC_DATA (ProcessorNumber, ApicID),
        SetProcessorRegister
        );
    }

    RegisterTable = &mCpuConfigContextBuffer.RegisterTable[ProcessorNumber];
    RegisterTable->InitialApicId = GetInitialLocalApicId (ProcessorNumber);
  }
//
// PURLEY_OVERRIDE_BEGIN
//
  CpuSncInitialize();
//
// PURLEY_OVERRIDE_END
//
  //
  // Set PcdCpuCallbackSignal to trigger callback function
  //
  Status = PcdSet8S (PcdCpuCallbackSignal, CPU_PROCESSOR_SETTING_END_SIGNAL);
  ASSERT_EFI_ERROR (Status);

  //
  // From now on, SetProcessorRegister() will be called only by SimpleApProcWrapper()
  // and ApProcWrapper to restore register settings after INIT signal, so switch
  // this flag from FALSE to TRUE.
  //
  mRestoreSettingAfterInit = TRUE;
}
