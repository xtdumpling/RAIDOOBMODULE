/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
SMM MP service inplementation

Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include "PiSmmCpuDxeSmm.h"

//
// Slots for all MTRR( FIXED MTRR + VARIABLE MTRR + MTRR_LIB_IA32_MTRR_DEF_TYPE)
//
UINT64                                      gSmiMtrrs[MTRR_NUMBER_OF_FIXED_MTRR + 2 * MTRR_NUMBER_OF_VARIABLE_MTRR + 1];
UINT64                                      gPhyMask;
SMM_DISPATCHER_MP_SYNC_DATA                 *mSmmMpSyncData = NULL;
UINTN                                       mSmmMpSyncDataSize;

// PURLEY_OVERRIDE_BEGIN_4929970
/**
  Changes test-and-set loop into test-and-set with inner test loop.
  This function should be called in any loop containing an 
  InterlockedCompareExchange32 which may fail.

  A problem with InterlockedCompareExchange32 is that by repeatedly 
  accessing a specific memory with locking access, a high percentage
  of the memory bandwidth is used in the case of high thread counts
  competing for the semaphore causing a hot spot which reduces the 
  overall memory bus utilization by all processors.
  
  This function contains the inner loop that should be used before 
  retrying the InterlockedCompareExchange32. First a short delay is 
  invoked via a pause instruction. This is particularly important 
  when Hyperthreading is enabled to allow other threads in the same 
  core to use shared resources. Then a read of the cached copy of the 
  memory location occurs. Cache snoops are used by the cache coherence 
  algorithm, so the cached copy will be invalidated when the shared 
  memory copy is updated. So the loop will continue until the semaphore's 
  memory location is no longer changing.

  Original test-and-set loop looks like this:
    Value = *Sem;
    while InterlockedCompareExchange32();
  
  Improved test-and-set with inner test loop looks like this:
    Value = *Sem;
    while InterlockedCompareExchange32() {
      do {
        CpuPause ();
        Value = *Sem;
      } while (Value != *Sem);
    }
    
  Refactored implementation looks like this:
    Value = *Sem;
    while InterlockedCompareExchange32() {
      Value = WaitForStableValue();
    }

  @param      Sem        IN:  32-bit unsigned integer
  @return     Value

**/
STATIC
UINT32
WaitForStableValue (
  IN volatile UINT32    *Sem
  )
{
  UINT32                Value;

  do {
    CpuPause ();
    Value = *Sem;
  } while (Value != *Sem);
  return Value;
}
// PURLEY_OVERRIDE_END_4929970
/**
  Performs an atomic compare exchange operation to get semaphore.
  The compare exchange operation must be performed using
  MP safe mechanisms.

  @param      Sem        IN:  32-bit unsigned integer
                         OUT: original integer - 1
  @return     Original integer - 1

**/
UINT32
WaitForSemaphore (
  IN OUT  volatile UINT32           *Sem
  )
{
  UINT32                            Value;
// PURLEY_OVERRIDE_BEGIN_4929970
  Value = *Sem;
  while (Value == 0 || InterlockedCompareExchange32((UINT32*)Sem, Value, Value - 1) != Value) {
    Value = WaitForStableValue(Sem);
  }
// PURLEY_OVERRIDE_END_4929970
  return Value - 1;
}

// PURLEY_OVERRIDE_BEGIN_4929970
UINT32
WaitForSemaphoreCount (
  IN OUT  volatile UINT32           *Sem,
  IN      UINT32                    Count
  )
{
  UINT32                            Value;

  Value = *Sem;
  while (Value == 0 || InterlockedCompareExchange32((UINT32*)Sem, Value, Value - Count) != Value) {
    Value = WaitForStableValue(Sem);
  }
  return Value - Count;
}
// PURLEY_OVERRIDE_END_4929970

/**
  Performs an atomic compare exchange operation to release semaphore.
  The compare exchange operation must be performed using
  MP safe mechanisms.

  @param      Sem        IN:  32-bit unsigned integer
                         OUT: original integer + 1
  @return     Original integer + 1

**/
UINT32
ReleaseSemaphore (
  IN OUT  volatile UINT32           *Sem
  )
{
  UINT32                            Value;

// PURLEY_OVERRIDE_BEGIN_4929970
  Value = *Sem;
  while (Value + 1 != 0 && InterlockedCompareExchange32((UINT32*)Sem, Value, Value + 1) != Value) {
    Value = WaitForStableValue(Sem);
  }
// PURLEY_OVERRIDE_END_4929970
  return Value + 1;
}

/**
  Performs an atomic compare exchange operation to lock semaphore.
  The compare exchange operation must be performed using
  MP safe mechanisms.

  @param      Sem        IN:  32-bit unsigned integer
                         OUT: -1
  @return     Orignal integer

**/
UINT32
LockdownSemaphore (
  IN OUT  volatile UINT32           *Sem
  )
{
  UINT32                            Value;

// PURLEY_OVERRIDE_BEGIN_4929970
  Value = *Sem;
  while (InterlockedCompareExchange32 ((UINT32*)Sem, Value, (UINT32)-1) != Value) {
    Value = WaitForStableValue(Sem);
  }
// PURLEY_OVERRIDE_END_4929970
  return Value;
}

/**
  Wait all APs to performs an atomic compare exchange operation to release semaphore.

  @param   NumberOfAPs      AP number

**/
VOID
WaitForAllAPs (
  IN      UINTN                     NumberOfAPs
  )
{
  UINTN                             BspIndex;
  UINT32                            Count;  // PURLEY_OVERRIDE_4929970

  BspIndex = mSmmMpSyncData->BspIndex;

// PURLEY_OVERRIDE_BEGIN_4929970
  while (NumberOfAPs > 0) {
    Count = (UINT32)mSmmMpSyncData->CpuData[BspIndex].Run;
    WaitForSemaphoreCount (&mSmmMpSyncData->CpuData[BspIndex].Run, Count);
    NumberOfAPs -= Count;
  }
// PURLEY_OVERRIDE_END_4929970
}

/**
  Performs an atomic compare exchange operation to release semaphore
  for each AP.

**/
VOID
ReleaseAllAPs (
  VOID
  )
{
  UINTN                             Index;
  UINTN                             BspIndex;

  BspIndex = mSmmMpSyncData->BspIndex;
  for (Index = mMaxNumberOfCpus; Index-- > 0;) {
    if (Index != BspIndex && mSmmMpSyncData->CpuData[Index].Present) {
      ReleaseSemaphore (&mSmmMpSyncData->CpuData[Index].Run);
    }
  }
}

/**
  Checks if all CPUs (with certain exceptions) have checked in for this SMI run

  NOTE: for Haswell's exceptions checking, only single-socket CPU is supported

  @param   Exceptions     CPU Arrival exception flags.

  @retval   TRUE  if all CPUs the have checked in.
  @retval   FALSE  if at least one Normal AP hasn't checked in.

**/
BOOLEAN
AllCpusInSmmWithExceptions (
  SMM_CPU_ARRIVAL_EXCEPTIONS  Exceptions  
  )
{
  UINTN                             Index;
  SMM_CPU_SYNC_FEATURE              *SyncFeature;
  SMM_CPU_SYNC_FEATURE              *SmmSyncFeatures;
  UINT64                            LogProcEnBit;
  SMM_CPU_DATA_BLOCK                *CpuData;
  EFI_PROCESSOR_INFORMATION         *ProcessorInfo;

  ASSERT (mSmmMpSyncData->Counter <= mNumberOfCpus);

  if (mSmmMpSyncData->Counter == mNumberOfCpus) {
    return TRUE;
  }

  CpuData = mSmmMpSyncData->CpuData;
  ProcessorInfo = gSmmCpuPrivate->ProcessorInfo;
  SmmSyncFeatures = gSmmCpuPrivate->SmmSyncFeature;
  for (Index = mMaxNumberOfCpus; Index-- > 0;) {
    if (!CpuData[Index].Present && ProcessorInfo[Index].ProcessorId != INVALID_APIC_ID) {
      SyncFeature = &(SmmSyncFeatures[Index]);
      LogProcEnBit = SyncFeature->HaswellLogProcEnBit;
      if (((Exceptions & ARRIVAL_EXCEPTION_DELAYED) != 0) && SyncFeature->DelayIndicationSupported && (SmmReadReg64 (Index, SmmRegSmmDelayed) & LogProcEnBit)) {
        continue;
      }
      if (((Exceptions & ARRIVAL_EXCEPTION_BLOCKED) != 0) && SyncFeature->BlockIndicationSupported && (SmmReadReg64 (Index, SmmRegSmmBlocked) & LogProcEnBit)) {
        continue;
      }
      if (((Exceptions & ARRIVAL_EXCEPTION_SMI_DISABLED) != 0) && SyncFeature->TargetedSmiSupported && !(SmmReadReg64 (Index, SmmRegSmmEnable) & LogProcEnBit)) {
        continue;
      }
      return FALSE;
    }
  }

  return TRUE;
}


/**
  Given timeout constraint, wait for all APs to arrive, and insure when this function returns, no AP will execute normal mode code before
  entering SMM, except SMI disabled APs.

**/
VOID
SmmWaitForApArrival (
  VOID
  )
{
  UINT64                            Timer;
  UINTN                             Index;
//
// ServerCommonPkg BpHotFix begin
//
  BOOLEAN                           Lmce;
  BOOLEAN                           LmceEn;
//
// ServerCommonPkg BpHotFix end
//

  ASSERT (mSmmMpSyncData->Counter <= mNumberOfCpus);

  //
  // Platform implementor should choose a timeout value appropriately:
  // - The timeout value should balance the SMM time constrains and the likelihood that delayed CPUs are excluded in the SMM run. Note 
  //   the SMI Handlers must ALWAYS take into account the cases that not all APs are available in an SMI run.
  // - The timeout value must, in the case of 2nd timeout, be at least long enough to give time for all APs to receive the SMI IPI 
  //   and either enter SMM or buffer the SMI, to insure there is no CPU running normal mode code when SMI handling starts. This will
  //   be TRUE even if a blocked CPU is brought out of the blocked state by a normal mode CPU (before the normal mode CPU received the
  //   SMI IPI), because with a buffered SMI, and CPU will enter SMM immediately after it is brought out of the blocked state.
  // - The timeout value must be longer than longest possible IO operation in the system
  // (BTW, ** to confirm **, Is Slow I/O operation reported as an SMM Delayed status in Haswell?)
  //

  //
  // Sync with APs 1st timeout
  //
//
// ServerCommonPkg BpHotFix begin
//
  IsLmceOsEnabled(&LmceEn);
  IsLmceSignaled(&Lmce);
//
// ServerCommonPkg BpHotFix end
//
  
  for (Timer = StartSyncTimer ();
       !IsSyncTimerTimeout (Timer) &&
 //
// ServerCommonPkg BpHotFix begin
//
       !(LmceEn && Lmce) &&
//
// ServerCommonPkg BpHotFix end
//
       !AllCpusInSmmWithExceptions (ARRIVAL_EXCEPTION_BLOCKED | ARRIVAL_EXCEPTION_SMI_DISABLED );
       ) {
    CpuPause ();
  }
  
  //
  // Not all APs have arrived, so we need 2nd round of timeout. IPIs should be sent to ALL none present APs,
  // because:
  // a) Delayed AP may have just come out of the delayed state. Blocked AP may have just been brought out of blocked state by some AP running 
  //    normal mode code. These APs need to be guaranteed to have an SMI pending to insure that once they are out of delayed / blocked state, they 
  //    enter SMI immediately without executing instructions in normal mode. Note traditional flow requires there are no APs doing normal mode
  //    work while SMI handling is on-going.
  // b) As a consequence of SMI IPI sending, (spurious) SMI may occur after this SMM run.
  // c) ** NOTE **: Use SMI disabling feature VERY CAREFULLY (if at all) for traditional flow, because a processor in SMI-disabled state
  //    will execute normal mode code, which breaks the traditional SMI handlers' assumption that no APs are doing normal
  //    mode work while SMI handling is on-going.
  // d) We don't add code to check SMI disabling status to skip sending IPI to SMI disabled APs, because:
  //    - In traditional flow, SMI disabling is discouraged.
  //    - In relaxed flow, CheckApArrival() will check SMI disabling status before calling this function.
  //    In both cases, adding SMI-disabling checking code increases overhead.
  //
  if (mSmmMpSyncData->Counter < mNumberOfCpus) {
    //
    // Send SMI IPIs to bring outside processors in
    //
    for (Index = mMaxNumberOfCpus; Index-- > 0;) {
      if (!mSmmMpSyncData->CpuData[Index].Present && gSmmCpuPrivate->ProcessorInfo[Index].ProcessorId != INVALID_APIC_ID) {
        SendSmiIpi ((UINT32)gSmmCpuPrivate->ProcessorInfo[Index].ProcessorId);
      }
    }
  
    //
    // Sync with APs 2nd timeout.
    //
    for (Timer = StartSyncTimer ();
         !IsSyncTimerTimeout (Timer) &&
         !AllCpusInSmmWithExceptions (ARRIVAL_EXCEPTION_BLOCKED | ARRIVAL_EXCEPTION_SMI_DISABLED );
         ) {
      CpuPause ();
    }
  }

  return;
}


/**
  Replace OS MTRR's with SMI MTRR's.

  @param    CpuIndex             Processor Index

**/
VOID
ReplaceOSMtrrs (
  IN      UINTN                     CpuIndex
  )
{
  PROCESSOR_SMM_DESCRIPTOR       *Psd;
  UINT64                         *SmiMtrrs;
  MTRR_SETTINGS                  *BiosMtrr;

  Psd = (PROCESSOR_SMM_DESCRIPTOR*)(mCpuHotPlugData.SmBase[CpuIndex] + SMM_PSD_OFFSET);
  SmiMtrrs = (UINT64*)(UINTN)Psd->MtrrBaseMaskPtr;

  DisableSmrr ();

  //
  // Replace all MTRRs registers
  //
  BiosMtrr  = (MTRR_SETTINGS*)SmiMtrrs;
  MtrrSetAllMtrrs(BiosMtrr);
}

/**
  SMI handler for BSP.

  @param     CpuIndex         BSP processor Index
  @param     SyncMode         SMM MP sync mode

**/
VOID
BSPHandler (
  IN      UINTN                     CpuIndex,
  IN      SMM_CPU_SYNC_MODE         SyncMode
  )
{
  UINTN                             Index;
  MTRR_SETTINGS                     Mtrrs;
  UINTN                             ApCount;
  BOOLEAN                           ClearTopLevelSmiResult;
  UINTN                             PresentCount;

  ASSERT (CpuIndex == mSmmMpSyncData->BspIndex);
  ApCount = 0;

  //
  // Flag BSP's presence
  //
  mSmmMpSyncData->InsideSmm = TRUE;

  //
  // Initialize Debug Agent to start source level debug in BSP handler
  //
  InitializeDebugAgent (DEBUG_AGENT_INIT_ENTER_SMI, NULL, NULL);
  
  //
  // Mark this processor's presence
  //
  mSmmMpSyncData->CpuData[CpuIndex].Present = TRUE;

  //
  // Clear platform top level SMI status bit before calling SMI handlers. If
  // we cleared it after SMI handlers are run, we would miss the SMI that
  // occurs after SMI handlers are done and before SMI status bit is cleared.
  // 
  ClearTopLevelSmiResult = ClearTopLevelSmiStatus();
  ASSERT (ClearTopLevelSmiResult == TRUE);

  //
  // Set running processor index
  //
  gSmmCpuPrivate->SmmCoreEntryContext.CurrentlyExecutingCpu = CpuIndex;
  
  //
  // If Traditional Sync Mode or need to configure MTRRs: gather all available APs.
  // To-Do: make NeedConfigureMtrrs a PCD?
  //
  if (SyncMode == SmmCpuSyncModeTradition || NeedConfigureMtrrs()) {

    //
    // Wait for APs to arrive
    //
    SmmWaitForApArrival();

    //
    // Lock the counter down and retrieve the number of APs
    //
    mSmmMpSyncData->AllCpusInSync = TRUE;
    ApCount = LockdownSemaphore (&mSmmMpSyncData->Counter) - 1;
    
    //
    // Wait for all APs to get ready for programming MTRRs
    //
    WaitForAllAPs (ApCount);
    
    if (NeedConfigureMtrrs()) {
      //
      // Signal all APs it's time for backup MTRRs
      //
      ReleaseAllAPs ();
    
      //
      // WaitForSemaphore() may wait for ever if an AP happens to enter SMM at
      // exactly this point. Please make sure PcdCpuSmmMaxSyncLoops has been set
      // to a large enough value to avoid this situation.
      // Note: For HT capable CPUs, threads within a core share the same set of MTRRs.
      // We do the backup first and then set MTRR to avoid race condition for threads
      // in the same core.
      //
      MtrrGetAllMtrrs(&Mtrrs);
    
      //
      // Wait for all APs to complete their MTRR saving
      //
      WaitForAllAPs (ApCount);
    
      //
      // Let all processors program SMM MTRRs together
      //
      ReleaseAllAPs ();
    
      //
      // WaitForSemaphore() may wait for ever if an AP happens to enter SMM at
      // exactly this point. Please make sure PcdCpuSmmMaxSyncLoops has been set
      // to a large enough value to avoid this situation.
      //
      ReplaceOSMtrrs (CpuIndex);
    
      //
      // Wait for all APs to complete their MTRR programming
      //
      WaitForAllAPs (ApCount);
    }
  }

  //
  // The BUSY lock is initialized to Acquired state
  //
  AcquireSpinLockOrFail (&mSmmMpSyncData->CpuData[CpuIndex].Busy);

  //
  // Special handlement for S3 boot path.
  //
  RestoreSmmConfigurationInS3 ();

  //
  // Invoke SMM Foundation EntryPoint with the processor information context.
  //
  gSmmCpuPrivate->SmmCoreEntry (&gSmmCpuPrivate->SmmCoreEntryContext);

  //
  // Make sure all APs have completed their pending none-block tasks
  //
  for (Index = mMaxNumberOfCpus; Index-- > 0;) {
    if (Index != CpuIndex && mSmmMpSyncData->CpuData[Index].Present) {
      while(1 != mSmmMpSyncData->CpuData[Index].Busy);  // PURLEY_OVERRIDE_4929970
    }
  }

  //
  // Perform the remaining tasks
  //
  PerformRemainingTasks ();
  
  //
  // Set the EOS bit before SMI resume.
  //
  // BUGBUG: The following is a chipset specific action from a CPU module.
  //
  ClearSmi();

  //
  // If Relaxed-AP Sync Mode: gather all available APs after BSP SMM handlers are done, and
  // make those APs to exit SMI synchronously. APs which arrive later will be excluded and 
  // will run through freely.
  //
  if (SyncMode != SmmCpuSyncModeTradition && !NeedConfigureMtrrs()) {

    //
    // Lock the counter down and retrieve the number of APs
    //
    mSmmMpSyncData->AllCpusInSync = TRUE;
    ApCount = LockdownSemaphore (&mSmmMpSyncData->Counter) - 1;
    //
    // Make sure all APs have their Present flag set
    //
    while (TRUE) {
      PresentCount = 0;
      for (Index = mMaxNumberOfCpus; Index-- > 0;) {
        if (mSmmMpSyncData->CpuData[Index].Present) {
          PresentCount ++;
        }
      }
      if (PresentCount > ApCount) {
        break;
      }
    }
  }

  //
  // Notify all APs to exit
  //
  mSmmMpSyncData->InsideSmm = FALSE;
  ReleaseAllAPs ();

  //
  // Wait for all APs to complete their pending tasks
  //
  WaitForAllAPs (ApCount);

  //
  // Set the effective Sync Mode for next SMI run.
  // Note this setting must be performed in the window where no APs can check in.
  //
  mSmmMpSyncData->EffectiveSyncMode = mSmmMpSyncData->SyncModeToBeSet;

  if (NeedConfigureMtrrs()) {
    //
    // Signal APs to restore MTRRs
    //
    ReleaseAllAPs ();

    //
    // Restore OS MTRRs
    //
    ReenableSmrr ();
    MtrrSetAllMtrrs(&Mtrrs);

    //
    // Wait for all APs to complete MTRR programming
    //
    WaitForAllAPs (ApCount);
  }

  //
  // Stop source level debug in BSP handler, the code below will not be
  // debugged.
  //
  InitializeDebugAgent (DEBUG_AGENT_INIT_EXIT_SMI, NULL, NULL);

  //
  // Signal APs to Reset states/semaphore for this processor
  //
  ReleaseAllAPs ();

  //
  // Perform pending operations for hot-plug
  //
  SmmCpuUpdate ();

  //
  // Clear the Present flag of BSP
  //
  mSmmMpSyncData->CpuData[CpuIndex].Present = FALSE;

  //
  // Gather APs to exit SMM synchronously. Note the Present flag is cleared by now but
  // WaitForAllAps does not depend on the Present flag.
  //
  WaitForAllAPs (ApCount);

  //
  // Reset BspIndex to -1, meaning BSP has not been elected.
  //
  if (FeaturePcdGet (PcdCpuSmmEnableBspElection)) {
    mSmmMpSyncData->BspIndex = (UINT32)-1;
  }

  //
  // Allow APs to check in from this point on
  //
  mSmmMpSyncData->Counter = 0;
  mSmmMpSyncData->AllCpusInSync = FALSE;
}

/**
  SMI handler for AP.

  @param     CpuIndex         AP processor Index.
  @param     ValidSmi         Indicates that current SMI is a valid SMI or not.
  @param     SyncMode         SMM MP sync mode.

**/
VOID
APHandler (
  IN      UINTN                     CpuIndex,
  IN      BOOLEAN                   ValidSmi,
  IN      SMM_CPU_SYNC_MODE         SyncMode
  )
{
  UINT64                            Timer;
  UINTN                             BspIndex;
  MTRR_SETTINGS                     Mtrrs;

  //
  // Timeout BSP
  //
  for (Timer = StartSyncTimer ();
       !IsSyncTimerTimeout (Timer) &&
       !mSmmMpSyncData->InsideSmm;
       ) {
    CpuPause ();
  }

  if (!mSmmMpSyncData->InsideSmm) {
    //
    // BSP timeout in the first round
    //
    if (mSmmMpSyncData->BspIndex != -1) {
      //
      // BSP Index is known
      //
      BspIndex = mSmmMpSyncData->BspIndex;
      ASSERT (CpuIndex != BspIndex);

      //
      // Send SMI IPI to bring BSP in
      //
      SendSmiIpi ((UINT32)gSmmCpuPrivate->ProcessorInfo[BspIndex].ProcessorId);

      //
      // Now clock BSP for the 2nd time
      //
      for (Timer = StartSyncTimer ();
           !IsSyncTimerTimeout (Timer) &&
           !mSmmMpSyncData->InsideSmm;
           ) {
        CpuPause ();
      }

      if (!mSmmMpSyncData->InsideSmm) {
        //
        // Give up since BSP is unable to enter SMM
        // and signal the completion of this AP
        WaitForSemaphore (&mSmmMpSyncData->Counter);
        return;
      }
    } else {
      //
      // Don't know BSP index. Give up without sending IPI to BSP.
      //
      WaitForSemaphore (&mSmmMpSyncData->Counter);
      return;
    }
  }
  
  //
  // BSP is available
  //
  BspIndex = mSmmMpSyncData->BspIndex;
  ASSERT (CpuIndex != BspIndex);

  //
  // Mark this processor's presence
  //
  mSmmMpSyncData->CpuData[CpuIndex].Present = TRUE;

  if (SyncMode == SmmCpuSyncModeTradition || NeedConfigureMtrrs()) {
    //
    // Notify BSP of arrival at this point
    //
    ReleaseSemaphore (&mSmmMpSyncData->CpuData[BspIndex].Run);
  }

  if (NeedConfigureMtrrs()) {
    //
    // Wait for the signal from BSP to backup MTRRs
    //
    WaitForSemaphore (&mSmmMpSyncData->CpuData[CpuIndex].Run);

    //
    // Backup OS MTRRs
    //
    MtrrGetAllMtrrs(&Mtrrs);

    //
    // Signal BSP the completion of this AP
    //
    ReleaseSemaphore (&mSmmMpSyncData->CpuData[BspIndex].Run);

    //
    // Wait for BSP's signal to program MTRRs
    //
    WaitForSemaphore (&mSmmMpSyncData->CpuData[CpuIndex].Run);

    //
    // Replace OS MTRRs with SMI MTRRs
    //
    ReplaceOSMtrrs (CpuIndex);

    //
    // Signal BSP the completion of this AP
    //
    ReleaseSemaphore (&mSmmMpSyncData->CpuData[BspIndex].Run);
  }

  while (TRUE) {
    //
    // Wait for something to happen
    //
    WaitForSemaphore (&mSmmMpSyncData->CpuData[CpuIndex].Run);

    //
    // Check if BSP wants to exit SMM
    //
    if (!mSmmMpSyncData->InsideSmm) {
      break;
    }

    //
    // BUSY should be acquired by SmmStartupThisAp()
    //
    ASSERT (
      !AcquireSpinLockOrFail (&mSmmMpSyncData->CpuData[CpuIndex].Busy)
      );

    //
    // Invoke the scheduled procedure
    //
    (*mSmmMpSyncData->CpuData[CpuIndex].Procedure) (
      (VOID*)mSmmMpSyncData->CpuData[CpuIndex].Parameter
      );

    //
    // Release BUSY
    //
    ReleaseSpinLock (&mSmmMpSyncData->CpuData[CpuIndex].Busy);
  }

  if (NeedConfigureMtrrs()) {
    //
    // Notify BSP the readiness of this AP to program MTRRs
    //
    ReleaseSemaphore (&mSmmMpSyncData->CpuData[BspIndex].Run);

    //
    // Wait for the signal from BSP to program MTRRs
    //
    WaitForSemaphore (&mSmmMpSyncData->CpuData[CpuIndex].Run);

    //
    // Restore OS MTRRs
    //
    ReenableSmrr ();
    MtrrSetAllMtrrs(&Mtrrs);
  }

  //
  // Notify BSP the readiness of this AP to Reset states/semaphore for this processor
  //
  ReleaseSemaphore (&mSmmMpSyncData->CpuData[BspIndex].Run);

  //
  // Wait for the signal from BSP to Reset states/semaphore for this processor
  //
  WaitForSemaphore (&mSmmMpSyncData->CpuData[CpuIndex].Run);
  
  //
  // Reset states/semaphore for this processor
  //
  mSmmMpSyncData->CpuData[CpuIndex].Present = FALSE;

  //
  // Notify BSP the readiness of this AP to exit SMM
  //
  ReleaseSemaphore (&mSmmMpSyncData->CpuData[BspIndex].Run);

}

/**
  Create 4G PageTable in SMRAM.

  @param          ExtraPages       Additional page numbers besides for 4G memory
  @return         PageTable Address

**/
UINT32
Gen4GPageTable (
  IN      UINTN                     ExtraPages
  )
{
  VOID    *PageTable;
  UINTN   Index;
  UINT64  *Pte;
  UINTN   PagesNeeded;
  UINTN   Low2MBoundary;
  UINTN   High2MBoundary;
  UINTN   Pages;
  UINTN   GuardPage;
  UINT64  *Pdpte;
  UINTN   PageIndex;
  UINTN   PageAddress;

  Low2MBoundary = 0;
  High2MBoundary = 0;
  PagesNeeded = 0;
  if (FeaturePcdGet (PcdCpuSmmStackGuard)) {
    //
    // Add one more page for known good stack, then find the lower 2MB aligned address.
    //
    Low2MBoundary = (mSmmStackArrayBase + EFI_PAGE_SIZE) & ~(SIZE_2MB-1);
    //
    // Add two more pages for known good stack and stack guard page,
    // then find the lower 2MB aligned address.
    //
    High2MBoundary = (mSmmStackArrayEnd - mSmmStackSize + EFI_PAGE_SIZE * 2) & ~(SIZE_2MB-1);
    PagesNeeded = ((High2MBoundary - Low2MBoundary) / SIZE_2MB) + 1;
  }
  //
  // Allocate the page table
  //
  PageTable = AllocatePages (ExtraPages + 5 + PagesNeeded);
  ASSERT (PageTable != NULL);

  PageTable = (VOID *)((UINTN)PageTable + EFI_PAGES_TO_SIZE (ExtraPages));
  Pte = (UINT64*)PageTable;

  //
  // Zero out all page table entries first
  //
  ZeroMem (Pte, EFI_PAGES_TO_SIZE (1));

  //
  // Set Page Directory Pointers
  //
  for (Index = 0; Index < 4; Index++) {
    Pte[Index] = (UINTN)PageTable + EFI_PAGE_SIZE * (Index + 1) + IA32_PG_P;
  }
  Pte += EFI_PAGE_SIZE / sizeof (*Pte);

  //
  // Fill in Page Directory Entries
  //
  for (Index = 0; Index < EFI_PAGE_SIZE * 4 / sizeof (*Pte); Index++) {
    Pte[Index] = (Index << 21) + IA32_PG_PS + IA32_PG_RW + IA32_PG_P;
  }

  if (FeaturePcdGet (PcdCpuSmmStackGuard)) {
    Pages = (UINTN)PageTable + EFI_PAGES_TO_SIZE (5);
    GuardPage = mSmmStackArrayBase + EFI_PAGE_SIZE;
    Pdpte = (UINT64*)PageTable;
    for (PageIndex = Low2MBoundary; PageIndex <= High2MBoundary; PageIndex += SIZE_2MB) {
      Pte = (UINT64*)(UINTN)(Pdpte[BitFieldRead32 ((UINT32)PageIndex, 30, 31)] & ~(EFI_PAGE_SIZE - 1));
      Pte[BitFieldRead32 ((UINT32)PageIndex, 21, 29)] = (UINT64)Pages + IA32_PG_RW + IA32_PG_P;
      //
      // Fill in Page Table Entries
      //
      Pte = (UINT64*)Pages;
      PageAddress = PageIndex;
      for (Index = 0; Index < EFI_PAGE_SIZE / sizeof (*Pte); Index++) {
        if (PageAddress == GuardPage) {
          //
          // Mark the guard page as non-present
          //
          Pte[Index] = PageAddress;
          GuardPage += mSmmStackSize;
          if (GuardPage > mSmmStackArrayEnd) {
            GuardPage = 0;
          }
        } else {
          Pte[Index] = PageAddress + IA32_PG_RW + IA32_PG_P;
        }
        PageAddress+= EFI_PAGE_SIZE;
      }
      Pages += EFI_PAGE_SIZE;
    }
  }

  return (UINT32)(UINTN)PageTable;
}

/**
  Set memory cache ability.

  @param    PageTable              PageTable Address
  @param    Address                Memory Address to change cache ability
  @param    Cacheability           Cache ability to set

**/
VOID
SetCacheability (
  IN      UINT64                    *PageTable,
  IN      UINTN                     Address,
  IN      UINT8                     Cacheability
  )
{
  UINTN   PTIndex;
  VOID    *NewPageTableAddress;
  UINT64  *NewPageTable;
  UINTN   Index;

  ASSERT ((Address & EFI_PAGE_MASK) == 0);

  if (sizeof (UINTN) == sizeof (UINT64)) {
    PTIndex = (UINTN)RShiftU64 (Address, 39) & 0x1ff;
    ASSERT (PageTable[PTIndex] & IA32_PG_P);
    PageTable = (UINT64*)(UINTN)(PageTable[PTIndex] & gPhyMask);
  }

  PTIndex = (UINTN)RShiftU64 (Address, 30) & 0x1ff;
  ASSERT (PageTable[PTIndex] & IA32_PG_P);
  PageTable = (UINT64*)(UINTN)(PageTable[PTIndex] & gPhyMask);

  //
  // A perfect implementation should check the original cacheability with the
  // one being set, and break a 2M page entry into pieces only when they
  // disagreed.
  //
  PTIndex = (UINTN)RShiftU64 (Address, 21) & 0x1ff;
  if ((PageTable[PTIndex] & IA32_PG_PS) != 0) {
    //
    // Allocate a page from SMRAM
    //
    NewPageTableAddress = AllocatePages (1);
    ASSERT (NewPageTableAddress != NULL);

    NewPageTable = (UINT64 *)NewPageTableAddress;

    for (Index = 0; Index < 0x200; Index++) {
      NewPageTable[Index] = PageTable[PTIndex];
      if ((NewPageTable[Index] & IA32_PG_PAT_2M) != 0) {
        NewPageTable[Index] &= ~((UINT64)IA32_PG_PAT_2M);
        NewPageTable[Index] |= (UINT64)IA32_PG_PAT_4K;
      }
      NewPageTable[Index] |= (UINT64)(Index << EFI_PAGE_SHIFT);
    }

    PageTable[PTIndex] = ((UINTN)NewPageTableAddress & gPhyMask) | IA32_PG_P;
  }

  ASSERT (PageTable[PTIndex] & IA32_PG_P);
  PageTable = (UINT64*)(UINTN)(PageTable[PTIndex] & gPhyMask);

  PTIndex = (UINTN)RShiftU64 (Address, 12) & 0x1ff;
  ASSERT (PageTable[PTIndex] & IA32_PG_P);
  PageTable[PTIndex] &= ~((UINT64)((IA32_PG_PAT_4K | IA32_PG_CD | IA32_PG_WT)));
  PageTable[PTIndex] |= (UINT64)Cacheability;
}


/**
  Schedule a procedure to run on the specified CPU.

  @param  Procedure                The address of the procedure to run
  @param  CpuIndex                 Target CPU Index
  @param  ProcArguments            The parameter to pass to the procedure

  @retval EFI_INVALID_PARAMETER    CpuNumber not valid
  @retval EFI_INVALID_PARAMETER    CpuNumber specifying BSP
  @retval EFI_INVALID_PARAMETER    The AP specified by CpuNumber did not enter SMM
  @retval EFI_INVALID_PARAMETER    The AP specified by CpuNumber is busy
  @retval EFI_SUCCESS              The procedure has been successfully scheduled

**/
EFI_STATUS
EFIAPI
SmmStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  )
{
  if (CpuIndex >= gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus ||
      CpuIndex == gSmmCpuPrivate->SmmCoreEntryContext.CurrentlyExecutingCpu ||
      !mSmmMpSyncData->CpuData[CpuIndex].Present ||
      gSmmCpuPrivate->Operation[CpuIndex] == SmmCpuRemove ||
      !AcquireSpinLockOrFail (&mSmmMpSyncData->CpuData[CpuIndex].Busy)) {
    return EFI_INVALID_PARAMETER;
  }

  mSmmMpSyncData->CpuData[CpuIndex].Procedure = Procedure;
  mSmmMpSyncData->CpuData[CpuIndex].Parameter = ProcArguments;
  ReleaseSemaphore (&mSmmMpSyncData->CpuData[CpuIndex].Run);

  if (FeaturePcdGet (PcdCpuSmmBlockStartupThisAp)) {
    AcquireSpinLock (&mSmmMpSyncData->CpuData[CpuIndex].Busy);
    ReleaseSpinLock (&mSmmMpSyncData->CpuData[CpuIndex].Busy);
  }
  return EFI_SUCCESS;
}

/**
  Schedule a procedure to run on the specified CPU in a blocking fashion.

  @param  Procedure                The address of the procedure to run
  @param  CpuIndex                 Target CPU Index
  @param  ProcArguments            The parameter to pass to the procedure

  @retval EFI_INVALID_PARAMETER    CpuNumber not valid
  @retval EFI_INVALID_PARAMETER    CpuNumber specifying BSP
  @retval EFI_INVALID_PARAMETER    The AP specified by CpuNumber did not enter SMM
  @retval EFI_INVALID_PARAMETER    The AP specified by CpuNumber is busy
  @retval EFI_SUCCESS              The procedure has been successfully scheduled

**/
EFI_STATUS
EFIAPI
SmmBlockingStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  )
{
  if (CpuIndex >= gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus ||
      CpuIndex == gSmmCpuPrivate->SmmCoreEntryContext.CurrentlyExecutingCpu ||
      !mSmmMpSyncData->CpuData[CpuIndex].Present ||
      gSmmCpuPrivate->Operation[CpuIndex] == SmmCpuRemove ||
      !AcquireSpinLockOrFail (&mSmmMpSyncData->CpuData[CpuIndex].Busy)) {
    return EFI_INVALID_PARAMETER;
  }

  mSmmMpSyncData->CpuData[CpuIndex].Procedure = Procedure;
  mSmmMpSyncData->CpuData[CpuIndex].Parameter = ProcArguments;
  ReleaseSemaphore (&mSmmMpSyncData->CpuData[CpuIndex].Run);

  AcquireSpinLock (&mSmmMpSyncData->CpuData[CpuIndex].Busy);
  ReleaseSpinLock (&mSmmMpSyncData->CpuData[CpuIndex].Busy);
  return EFI_SUCCESS;
}


/**
  C function for SMI entry, each processor comes here upon SMI trigger.

  @param    CpuIndex              Cpu Index

**/
VOID
EFIAPI
SmiRendezvous (
  IN      UINTN                     CpuIndex
  )
{
  EFI_STATUS        Status;
  BOOLEAN           ValidSmi;
  BOOLEAN           IsBsp;
  BOOLEAN           BspInProgress;
  UINTN             Index;
  UINTN             Cr2;
  UINT64            MsrValue;
  BOOLEAN           XdDisableFlag;

  //
  // Save Cr2 because Page Fault exception in SMM may override its value
  //
  Cr2 = AsmReadCr2 ();

  //
  // Enable XMM instructions & exceptions
  //
  AsmWriteCr4 (AsmReadCr4 () | 0x600);
  
  //
  // Perform CPU specific entry hooks
  //
  SmmRendezvousEntry (CpuIndex);

  //
  // Determine if this is a valid Smi
  //
  ValidSmi = PlatformValidSmi();
  
  //
  // Determine if BSP has been already in progress. Note this must be checked after
  // ValidSmi because BSP may clear a valid SMI source after checking in.
  //
  BspInProgress = mSmmMpSyncData->InsideSmm;

  if (!BspInProgress && !ValidSmi) {
    //
    // If we reach here, it means when we sampled the ValidSmi flag, SMI status had not 
    // been cleared by BSP in a new SMI run (so we have a truly invalid SMI), or SMI 
    // status had been cleared by BSP and an existing SMI run has almost ended. (Note 
    // we sampled ValidSmi flag BEFORE judging BSP-in-progress status.) In both cases, there
    // is nothing we need to do.
    //
    goto Exit;
  } else {
    //
    // Signal presence of this processor
    //
    if (ReleaseSemaphore (&mSmmMpSyncData->Counter) == 0) {
      //
      // BSP has already ended the synchronization, so QUIT!!!
      //

      //
      // Wait for BSP's signal to finish SMI
      //
      while (mSmmMpSyncData->AllCpusInSync) {
        CpuPause ();
      }
      goto Exit;
    } else {

      //
      // The BUSY lock is initialized to Released state.
      // This needs to be done early enough to be ready for BSP's SmmStartupThisAp() call.
      // E.g., with Relaxed AP flow, SmmStartupThisAp() may be called immediately 
      // after AP's present flag is detected.
      //
      InitializeSpinLock (&mSmmMpSyncData->CpuData[CpuIndex].Busy);
    }

    //
    // Try to enable XD if XD feature is supported
    //
    XdDisableFlag = FALSE;
    if (mXdSupported) {
      //
      // Clear XD disable bit if it is set by others
      // and set the flag
      //
      MsrValue = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
      if ((MsrValue & B_XD_DISABLE_BIT) != 0) {
        XdDisableFlag = TRUE;
        AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, MsrValue & ~B_XD_DISABLE_BIT);
      }
      ActivateXd ();
    }

    if (FeaturePcdGet (PcdCpuSmmProfileEnable)) {
      ActivateSmmProfile (CpuIndex);
    }

    if (BspInProgress) {
      //
      // BSP has been elected. Follow AP path, regardless of ValidSmi flag
      // as BSP may have cleared the SMI status
      //
      APHandler (CpuIndex, ValidSmi, mSmmMpSyncData->EffectiveSyncMode);
    } else {
      //
      // We have a valid SMI
      //

      //
      // Elect BSP
      //
      IsBsp = FALSE;
      if (FeaturePcdGet (PcdCpuSmmEnableBspElection)) {
        if (!mSmmMpSyncData->SwitchBsp || mSmmMpSyncData->CandidateBsp[CpuIndex]) {
          //
          // Call platform hook to do BSP election
          //
          Status = PlatformSmmBspElection (&IsBsp);
          if (EFI_SUCCESS == Status) {
            //
            // Platform hook determines successfully
            //
            if (IsBsp) {
              mSmmMpSyncData->BspIndex = (UINT32)CpuIndex;
            }
          } else {
            //
            // Platform hook fails to determine, use default BSP election method
            //
            InterlockedCompareExchange32 (
              (UINT32*)&mSmmMpSyncData->BspIndex,
              (UINT32)-1,
              (UINT32)CpuIndex
              );
          }
        }
      }

      //
      // "mSmmMpSyncData->BspIndex == CpuIndex" means this is the BSP
      //
      if (mSmmMpSyncData->BspIndex == CpuIndex) {

        //
        // Clear last request for SwitchBsp.
        //
        if (mSmmMpSyncData->SwitchBsp) {
          mSmmMpSyncData->SwitchBsp = FALSE;
          for (Index = 0; Index < mMaxNumberOfCpus; Index++) {
            mSmmMpSyncData->CandidateBsp[Index] = FALSE;
          }
        }

        if (FeaturePcdGet (PcdCpuSmmProfileEnable)) {
          SmmProfileRecordSmiNum ();
        }

        //
        // BSP Handler is always called with a ValidSmi == TRUE
        //
        BSPHandler (CpuIndex, mSmmMpSyncData->EffectiveSyncMode);

      } else {
        APHandler (CpuIndex, ValidSmi, mSmmMpSyncData->EffectiveSyncMode);
      }
    }
    
    ASSERT (mSmmMpSyncData->CpuData[CpuIndex].Run == 0);

    //
    // Wait for BSP's signal to exit SMI
    //
    while (mSmmMpSyncData->AllCpusInSync) {
      CpuPause ();
    }

    if (mXdSupported && XdDisableFlag) {
      //
      // Set XD disable bit if it was set before entering SMM
      // 
      MsrValue = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
      AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, MsrValue | B_XD_DISABLE_BIT);
    }
  }

Exit:
  SmmRendezvousExit (CpuIndex);
  //
  // Restore Cr2
  //
  AsmWriteCr2 (Cr2);
}


/**
  Initialize un-cacheable data.

**/
VOID
EFIAPI
InitializeMpSyncData (
  VOID
  )
{
  if (mSmmMpSyncData != NULL) {
    ZeroMem (mSmmMpSyncData, mSmmMpSyncDataSize);
    mSmmMpSyncData->CpuData = (SMM_CPU_DATA_BLOCK *)((UINT8 *)mSmmMpSyncData + sizeof (SMM_DISPATCHER_MP_SYNC_DATA));
    mSmmMpSyncData->CandidateBsp = (BOOLEAN *)(mSmmMpSyncData->CpuData + gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus);
    if (FeaturePcdGet (PcdCpuSmmEnableBspElection)) {
      //
      // Enable BSP election by setting BspIndex to -1
      //
      mSmmMpSyncData->BspIndex = (UINT32)-1;
    }
    mSmmMpSyncData->SyncModeToBeSet = SmmCpuSyncModeTradition;
    mSmmMpSyncData->EffectiveSyncMode = SmmCpuSyncModeTradition;
  }
}

/**
  Initialize global data for MP synchronization.

  @param ImageHandle  File Image Handle.
  @param Stacks       Base address of SMI stack buffer for all processors.
  @param StackSize    Stack size for each processor in SMM.

**/
VOID
InitializeMpServiceData (
  IN EFI_HANDLE  ImageHandle,
  IN VOID        *Stacks,
  IN UINTN       StackSize
 )
{
  UINTN                     Index;
  MTRR_SETTINGS             *Mtrr;
  PROCESSOR_SMM_DESCRIPTOR  *Psd;
  UINTN                     GdtTssTableSize;
  UINT8                     *GdtTssTables;
  IA32_SEGMENT_DESCRIPTOR   *GdtDescriptor;
  UINTN                     TssBase;
  UINTN                     GdtTableStepSize;

  //
  // Initialize physical address mask
  // NOTE: Physical memory above virtual address limit is not supported !!!
  //
  AsmCpuid (0x80000008, (UINT32*)&Index, NULL, NULL, NULL);
  gPhyMask = LShiftU64 (1, (UINT8)Index) - 1;
  gPhyMask &= (1ull << 48) - EFI_PAGE_SIZE;

  InitializeSmmMtrrManager ();
  //
  // Create page tables
  //
  gSmiCr3 = SmmInitPageTable ();
  
  //
  // Initialize SMM startup code & PROCESSOR_SMM_DESCRIPTOR structures
  //
  gSmiStack = (UINT32)((UINTN)Stacks + StackSize - sizeof (UINTN));

  //
  // The Smi Handler of CPU[i] and PSD of CPU[i+x] are in the same SMM_CPU_INTERVAL,
  // and they cannot overlap.
  //
  ASSERT (gcSmiHandlerSize + 0x10000 - SMM_PSD_OFFSET < SMM_CPU_INTERVAL);
  ASSERT (SMM_HANDLER_OFFSET % SMM_CPU_INTERVAL == 0);

  GdtTssTables    = NULL;
  GdtTssTableSize = 0;
  GdtTableStepSize = 0;
  //
  // For X64 SMM, we allocate separate GDT/TSS for each CPUs to avoid TSS load contention
  // on each SMI entry.
  //
  if (EFI_IMAGE_MACHINE_TYPE_SUPPORTED(EFI_IMAGE_MACHINE_X64)) {
    GdtTssTableSize = (gcSmiGdtr.Limit + 1 + TSS_SIZE + 7) & ~7; // 8 bytes aligned
    if (mSmmProtectedModeEnable && GdtTssTableSize <= sizeof (IA32_SEGMENT_DESCRIPTOR) * SMM_PROT_MODE_GDT_ENTRY_COUNT) {
      //
      // If the memory hole between SMMREG structure is enough, use it to allocate GDT table 
      //
      GdtTssTables     = (UINT8*)&mSmmProtModeContext->SmmProtectedModeGdt;
      GdtTableStepSize = sizeof (SMM_PROT_MODE_CONTEXT);
    } else {
      GdtTssTables = (UINT8*)AllocatePages (EFI_SIZE_TO_PAGES (GdtTssTableSize * gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus));
      ASSERT (GdtTssTables != NULL);
      GdtTableStepSize = GdtTssTableSize;
    }

    for (Index = 0; Index < gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus; Index++) {
      CopyMem (GdtTssTables + GdtTableStepSize * Index, (VOID*)(UINTN)gcSmiGdtr.Base, gcSmiGdtr.Limit + 1 + TSS_SIZE);
      if (FeaturePcdGet (PcdCpuSmmStackGuard)) {
        //
        // Setup top of known good stack as IST1 for each processor.
        //
        *(UINTN *)(GdtTssTables + GdtTableStepSize * Index + gcSmiGdtr.Limit + 1 + TSS_X64_IST1_OFFSET) = (mSmmStackArrayBase + EFI_PAGE_SIZE + Index * mSmmStackSize);
      }
    }
  } else if (FeaturePcdGet (PcdCpuSmmStackGuard)) {

    //
    // For IA32 SMM, if SMM Stack Guard feature is enabled, we use 2 TSS.
    // in this case, we allocate separate GDT/TSS for each CPUs to avoid TSS load contention
    // on each SMI entry.
    //

    //
    // Enlarge GDT to contain 2 TSS descriptors
    //
    gcSmiGdtr.Limit += (UINT16)(2 * sizeof (IA32_SEGMENT_DESCRIPTOR));
    
    GdtTssTableSize = (gcSmiGdtr.Limit + 1 + TSS_SIZE * 2 + 7) & ~7; // 8 bytes aligned
    if (mSmmProtectedModeEnable && GdtTssTableSize <= sizeof (IA32_SEGMENT_DESCRIPTOR) * SMM_PROT_MODE_GDT_ENTRY_COUNT) {
      //
      // If the memory hole between SMMREG structure is enough, use it to allocate GDT table 
      //
      GdtTssTables     = (UINT8*)&mSmmProtModeContext->SmmProtectedModeGdt;
      GdtTableStepSize = sizeof (SMM_PROT_MODE_CONTEXT);
    } else {
      GdtTssTables = (UINT8*)AllocatePages (EFI_SIZE_TO_PAGES (GdtTssTableSize * gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus));
      ASSERT (GdtTssTables != NULL);
      GdtTableStepSize = GdtTssTableSize;
    }

    for (Index = 0; Index < gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus; Index++) {
      CopyMem (GdtTssTables + GdtTableStepSize * Index, (VOID*)(UINTN)gcSmiGdtr.Base, gcSmiGdtr.Limit + 1 + TSS_SIZE * 2);
      //
      // Fixup TSS descriptors
      //
      TssBase = (UINTN)(GdtTssTables + GdtTableStepSize * Index + gcSmiGdtr.Limit + 1);
      GdtDescriptor = (IA32_SEGMENT_DESCRIPTOR *)(TssBase) - 2;
      GdtDescriptor->Bits.BaseLow = (UINT16)TssBase;
      GdtDescriptor->Bits.BaseMid = (UINT8)(TssBase >> 16);
      GdtDescriptor->Bits.BaseHigh = (UINT8)(TssBase >> 24);

      TssBase += TSS_SIZE;
      GdtDescriptor++;
      GdtDescriptor->Bits.BaseLow = (UINT16)TssBase;
      GdtDescriptor->Bits.BaseMid = (UINT8)(TssBase >> 16);
      GdtDescriptor->Bits.BaseHigh = (UINT8)(TssBase >> 24);
      //
      // Fixup TSS segments
      //
      // ESP as known good stack
      //
      *(UINTN *)(TssBase + TSS_IA32_ESP_OFFSET) =  mSmmStackArrayBase + EFI_PAGE_SIZE + Index * mSmmStackSize;
      *(UINT32 *)(TssBase + TSS_IA32_CR3_OFFSET) = gSmiCr3;
    }
  }

  for (Index = 0; Index < mMaxNumberOfCpus; Index++) {
    *(UINTN*)(UINTN)gSmiStack = Index;
    if (mSmmProtectedModeEnable) {
      gProtModeSmbase = (UINT32)mCpuHotPlugData.SmBase[Index];
      CopyMem (
        (VOID*)(UINTN)(mCpuHotPlugData.SmBase[Index] + SMM_HANDLER_OFFSET),
        (VOID*)gcSmiPMHandlerTemplate,
        gcSmiPMHandlerSize
        );
    } else {
      gSmbase = (UINT32)mCpuHotPlugData.SmBase[Index];
      CopyMem (
        (VOID*)(UINTN)(mCpuHotPlugData.SmBase[Index] + SMM_HANDLER_OFFSET),
        (VOID*)gcSmiHandlerTemplate,
        gcSmiHandlerSize
        );
    }

    Psd = (PROCESSOR_SMM_DESCRIPTOR*)(VOID*)(UINTN)(mCpuHotPlugData.SmBase[Index] + SMM_PSD_OFFSET);
    CopyMem (Psd, &gcPsd, sizeof (gcPsd));
    if (EFI_IMAGE_MACHINE_TYPE_SUPPORTED(EFI_IMAGE_MACHINE_X64)) {
      //
      // For X64 SMM, set GDT to the copy allocated above.
      //
      Psd->SmmGdtPtr = (UINT64)(UINTN)(GdtTssTables + GdtTableStepSize * Index);
    } else if (FeaturePcdGet (PcdCpuSmmStackGuard)) {
      //
      // For IA32 SMM, if SMM Stack Guard feature is enabled, set GDT to the copy allocated above.
      //
      Psd->SmmGdtPtr = (UINT64)(UINTN)(GdtTssTables + GdtTableStepSize * Index);
      Psd->SmmGdtSize = gcSmiGdtr.Limit + 1;
    }
    if (mSmmProtectedModeEnable) {
      //
      // Prepare for the SMMSEG structure
      //
      SetupSmmProtectedModeContext (Index, gProtModeSmbase, gSmiStack, (UINTN)Psd->SmmGdtPtr, Psd->SmmGdtSize);
    }
    gSmiStack += (UINT32) StackSize;
  }

  //
  // Initialize mSmmMpSyncData
  //
  mSmmMpSyncDataSize = sizeof (SMM_DISPATCHER_MP_SYNC_DATA) + 
                       (sizeof (SMM_CPU_DATA_BLOCK) + sizeof (BOOLEAN)) * gSmmCpuPrivate->SmmCoreEntryContext.NumberOfCpus;
  mSmmMpSyncData = (SMM_DISPATCHER_MP_SYNC_DATA*) AllocatePages (EFI_SIZE_TO_PAGES (mSmmMpSyncDataSize));
  ASSERT (mSmmMpSyncData != NULL);
  InitializeMpSyncData ();

  if (FeaturePcdGet (PcdCpuSmmUncacheCpuSyncData)) {
    //
    // mSmmMpSyncData should resides in un-cached RAM
    //
    SetCacheability ((UINT64*)(UINTN)gSmiCr3, (UINTN)mSmmMpSyncData, IA32_PG_CD);
  }

  //
  // Record current MTRR settings
  //
  ZeroMem(gSmiMtrrs, sizeof (gSmiMtrrs));
  Mtrr =  (MTRR_SETTINGS*)gSmiMtrrs;
  MtrrGetAllMtrrs (Mtrr);

}

/**

  Register the SMM Foundation entry point.

  @param          This              Pointer to EFI_SMM_CONFIGURATION_PROTOCOL instance
  @param          SmmEntryPoint     SMM Foundation EntryPoint

  @retval         EFI_SUCCESS       Successfully to register SMM foundation entry point

**/
EFI_STATUS
EFIAPI
RegisterSmmEntry (
  IN CONST EFI_SMM_CONFIGURATION_PROTOCOL  *This,
  IN EFI_SMM_ENTRY_POINT                   SmmEntryPoint
  )
{
  //
  // Record SMM Foundation EntryPoint, later invoke it on SMI entry vector.
  //
  gSmmCpuPrivate->SmmCoreEntry = SmmEntryPoint;
  return EFI_SUCCESS;
}


/**
  Set SMM synchronization mode starting from the next SMI run.

  @param  SyncMode              The SMM synchronization mode to be set and effective from the next SMI run.

  @retval EFI_SUCCESS           The function completes successfully.
  @retval EFI_INVALID_PARAMETER SynMode is not valid.

**/
EFI_STATUS
SmmSyncSetModeWorker (
  IN    SMM_CPU_SYNC_MODE                    SyncMode
  )
{
  if (SyncMode >= SmmCpuSyncModeMax) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Sync Mode effective from next SMI run
  //
  mSmmMpSyncData->SyncModeToBeSet = SyncMode;

  return EFI_SUCCESS;
}


/**
  Get current effective SMM synchronization mode.

  @param  SyncMode              Output parameter. The current effective SMM synchronization mode.

  @retval EFI_SUCCESS           The SMM synchronization mode has been retrieved successfully.
  @retval EFI_INVALID_PARAMETER SyncMode is NULL.

**/
EFI_STATUS
SmmSyncGetModeWorker (
  OUT   SMM_CPU_SYNC_MODE                    *SyncMode
  )
{
  if (SyncMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *SyncMode = mSmmMpSyncData->EffectiveSyncMode;
  return EFI_SUCCESS;
}


/**
  Given timeout constraint, wait for all APs to arrive. If this function returns EFI_SUCCESS, then
  no AP will execute normal mode code before entering SMM, so it is safe to access shared hardware resource 
  between SMM and normal mode. Note if there are SMI disabled APs, this function will return EFI_NOT_READY.


  @retval EFI_SUCCESS           No AP will execute normal mode code before entering SMM, so it is safe to access 
                                shared hardware resource between SMM and normal mode
  @retval EFI_NOT_READY         One or more CPUs may still execute normal mode code
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.

**/
EFI_STATUS
SmmCpuSyncCheckApArrivalWorker (
  VOID
  )
{
  UINTN                             Index;
  SMM_CPU_SYNC_FEATURE              *SyncFeature;
  SMM_CPU_SYNC_FEATURE              *SmmSyncFeatures;
  UINT64                            LogProcEnBit;
  SMM_CPU_DATA_BLOCK                *CpuData;
  EFI_PROCESSOR_INFORMATION         *ProcessorInfo;

  ASSERT (mSmmMpSyncData->Counter <= mNumberOfCpus);

  if (mSmmMpSyncData->Counter == mNumberOfCpus) {
    return EFI_SUCCESS;
  }
  
  //
  // Check if there are any SMI Disabled APs
  //
  CpuData = mSmmMpSyncData->CpuData;
  ProcessorInfo = gSmmCpuPrivate->ProcessorInfo;
  SmmSyncFeatures = gSmmCpuPrivate->SmmSyncFeature;
  for (Index = mMaxNumberOfCpus; Index-- > 0;) {
    if (ProcessorInfo[Index].ProcessorId != INVALID_APIC_ID) {
      SyncFeature = &(SmmSyncFeatures[Index]);
      LogProcEnBit = SyncFeature->HaswellLogProcEnBit;
      if (SyncFeature->TargetedSmiSupported && !(SmmReadReg64 (Index, SmmRegSmmEnable) & LogProcEnBit)) {
        return EFI_NOT_READY;
      }
    }
  }

  //
  // Wait for APs to arrive
  //
  SmmWaitForApArrival();

  //
  // If there are CPUs in SMI Disabled Statue, we return EFI_NOT_READY because this is not a safe environment for accessing shared
  // hardware resource between SMM and normal mode.
  //
  if (AllCpusInSmmWithExceptions (ARRIVAL_EXCEPTION_BLOCKED)) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_READY;
  }
}


/**
  Given timeout constraint, wait for all APs to arrive. If this function returns EFI_SUCCESS, then
  no AP will execute normal mode code before entering SMM, so it is safe to access shared hardware resource 
  between SMM and normal mode. Note if there are SMI disabled APs, this function will return EFI_NOT_READY.


  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.

  @retval EFI_SUCCESS           No AP will execute normal mode code before entering SMM, so it is safe to access 
                                shared hardware resource between SMM and normal mode
  @retval EFI_NOT_READY         One or more CPUs may still execute normal mode code
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.

**/
EFI_STATUS
EFIAPI
SmmCpuSyncCheckApArrival (
  IN    SMM_CPU_SYNC_PROTOCOL            *This
  )
{
  return SmmCpuSyncCheckApArrivalWorker();
}

/**
  Given timeout constraint, wait for all APs to arrive, and insure if this function returns EFI_SUCCESS, then
  no AP will execute normal mode code before entering SMM, so it is safe to access shared hardware resource 
  between SMM and normal mode. Note if there are SMI disabled APs, this function will return EFI_NOT_READY.


  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.

  @retval EFI_SUCCESS           No AP will execute normal mode code before entering SMM, so it is safe to access 
                                shared hardware resource between SMM and normal mode
  @retval EFI_NOT_READY         One or more CPUs may still execute normal mode code
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.

**/
EFI_STATUS
EFIAPI
SmmCpuSync2CheckApArrival (
  IN    SMM_CPU_SYNC2_PROTOCOL            *This
  )
{
  return SmmCpuSyncCheckApArrivalWorker();
}


/**
  Set SMM synchronization mode starting from the next SMI run.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              The SMM synchronization mode to be set and effective from the next SMI run.

  @retval EFI_SUCCESS           The function completes successfully.
  @retval EFI_INVALID_PARAMETER SynMode is not valid.

**/
EFI_STATUS
EFIAPI
SmmCpuSyncSetMode (
  IN    SMM_CPU_SYNC_PROTOCOL            *This,
  IN    SMM_CPU_SYNC_MODE                SyncMode
  )
{
  return SmmSyncSetModeWorker(SyncMode);
}

/**
  Set SMM synchronization mode starting from the next SMI run.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              The SMM synchronization mode to be set and effective from the next SMI run.

  @retval EFI_SUCCESS           The function completes successfully.
  @retval EFI_INVALID_PARAMETER SynMode is not valid.

**/
EFI_STATUS
EFIAPI
SmmCpuSync2SetMode (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  IN    SMM_CPU_SYNC_MODE                SyncMode
  )
{
  return SmmSyncSetModeWorker(SyncMode);
}


/**
  Get current effective SMM synchronization mode.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              Output parameter. The current effective SMM synchronization mode.

  @retval EFI_SUCCESS           The SMM synchronization mode has been retrieved successfully.
  @retval EFI_INVALID_PARAMETER SyncMode is NULL.

**/
EFI_STATUS
EFIAPI
SmmCpuSyncGetMode (
  IN    SMM_CPU_SYNC_PROTOCOL            *This,
  OUT   SMM_CPU_SYNC_MODE                *SyncMode
  )
{
  return SmmSyncGetModeWorker(SyncMode);
}

/**
  Get current effective SMM synchronization mode.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              Output parameter. The current effective SMM synchronization mode.

  @retval EFI_SUCCESS           The SMM synchronization mode has been retrieved successfully.
  @retval EFI_INVALID_PARAMETER SyncMode is NULL.

**/
EFI_STATUS
EFIAPI
SmmCpuSync2GetMode (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  OUT   SMM_CPU_SYNC_MODE                *SyncMode
  )
{
  return SmmSyncGetModeWorker(SyncMode);
}


/**
  Checks CPU SMM synchronization state

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  CpuIndex              Index of the CPU for which the state is to be retrieved.
  @param  CpuState              The state of the CPU

  @retval EFI_SUCCESS           Returns EFI_SUCCESS if the CPU's state is retrieved.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid or CpuState is NULL
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.
**/
EFI_STATUS
EFIAPI
SmmCpuSync2CheckCpuState (
  IN    SMM_CPU_SYNC2_PROTOCOL            *This,
  IN    UINTN                             CpuIndex,
  OUT   SMM_CPU_SYNC2_CPU_STATE           *CpuState               
  )
{
  SMM_CPU_SYNC_FEATURE              *SyncFeature;
  UINT64                            LogProcEnBit;
  
  if (CpuIndex >= mMaxNumberOfCpus) {
    return EFI_INVALID_PARAMETER;
  }

  if (gSmmCpuPrivate->ProcessorInfo[CpuIndex].ProcessorId == INVALID_APIC_ID) {
    return EFI_INVALID_PARAMETER;
  }

  if (CpuState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *CpuState = SmmCpuSync2StateNormal;
  SyncFeature = &(gSmmCpuPrivate->SmmSyncFeature[CpuIndex]);
  LogProcEnBit = SyncFeature->HaswellLogProcEnBit;

  if (SyncFeature->DelayIndicationSupported && (SmmReadReg64 (CpuIndex, SmmRegSmmDelayed) & LogProcEnBit)) {
    *CpuState = SmmCpuSync2StateDelayed;
  } else if (SyncFeature->BlockIndicationSupported && (SmmReadReg64 (CpuIndex, SmmRegSmmBlocked) & LogProcEnBit)) {
    *CpuState = SmmCpuSync2StateBlocked;
  } else if (SyncFeature->TargetedSmiSupported && !(SmmReadReg64 (CpuIndex, SmmRegSmmEnable) & LogProcEnBit)) {
    *CpuState = SmmCpuSync2StateDisabled;
  }

  return EFI_SUCCESS;
}


/**
  Change CPU's SMM enabling state.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  CpuIndex              Index of the CPU to enable / disable SMM
  @param  Enable                If TRUE, enable SMM; if FALSE disable SMM

  @retval EFI_SUCCESS           The CPU's SMM enabling state is changed.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid
  @retval EFI_UNSUPPORTED       Returns EFI_UNSUPPORTED the CPU does not support dynamically enabling / disabling SMI
  @retval EFI_DEVICE_ERROR      Error occured in changing SMM enabling state.
**/
EFI_STATUS
EFIAPI
SmmCpuSync2ChangeSmmEnabling (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  IN    UINTN                            CpuIndex,
  IN    BOOLEAN                          Enable
  )
{
  SMM_CPU_SYNC_FEATURE              *SyncFeature;
  UINT64                            LogProcEnBit;

  if (CpuIndex >= mMaxNumberOfCpus) {
    return EFI_INVALID_PARAMETER;
  }

  if (gSmmCpuPrivate->ProcessorInfo[CpuIndex].ProcessorId == INVALID_APIC_ID) {
    return EFI_INVALID_PARAMETER;
  }

  SyncFeature = &(gSmmCpuPrivate->SmmSyncFeature[CpuIndex]);
  LogProcEnBit = SyncFeature->HaswellLogProcEnBit;

  if (!SyncFeature->TargetedSmiSupported) {
    return EFI_UNSUPPORTED;
  }
  
  if (Enable) {
    SmmWriteReg64 (CpuIndex, SmmRegSmmEnable, SmmReadReg64 (CpuIndex, SmmRegSmmEnable) | LogProcEnBit);
  } else {
    SmmWriteReg64 (CpuIndex, SmmRegSmmEnable, SmmReadReg64 (CpuIndex, SmmRegSmmEnable) & (~LogProcEnBit));
  }

  return EFI_SUCCESS;
}


/**
  Send SMI IPI to a specific CPU.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  CpuIndex              Index of the CPU to send SMI to.

  @retval EFI_SUCCESS           The SMI IPI is sent successfully.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid
  @retval EFI_DEVICE_ERROR      Error occured in sending SMI IPI.
**/
EFI_STATUS
EFIAPI 
SmmCpuSync2SendSmi (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This,
  IN    UINTN                            CpuIndex
  )
{
  if (CpuIndex >= mMaxNumberOfCpus) {
    return EFI_INVALID_PARAMETER;
  }

  if (gSmmCpuPrivate->ProcessorInfo[CpuIndex].ProcessorId == INVALID_APIC_ID) {
    return EFI_INVALID_PARAMETER;
  }

  SendSmiIpi ((UINT32)gSmmCpuPrivate->ProcessorInfo[CpuIndex].ProcessorId);
  
  return EFI_SUCCESS;
}


/**
  Send SMI IPI to all CPUs excluding self

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.

  @retval EFI_SUCCESS           The SMI IPI is sent successfully.
  @retval EFI_INVALID_PARAMETER Returns EFI_INVALID_PARAMETER if CpuIndex is invalid
  @retval EFI_DEVICE_ERROR      Error occured in sending SMI IPI.
**/
EFI_STATUS
EFIAPI 
SmmCpuSync2SendSmiAllExcludingSelf (
  IN    SMM_CPU_SYNC2_PROTOCOL           *This
  )
{
  SendSmiIpiAllExcludingSelf();
  return EFI_SUCCESS;
}
