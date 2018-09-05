/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Semaphore mechanism to indicate to the BSP that an AP has exited SMM
  after SMBASE relocation.

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "PiSmmCpuDxeSmm.h"

#define LMA BIT10

UINTN   mSmmRelocationOriginalAddress;
BOOLEAN *mRebasedFlag;
extern  UINT32 mSmmRelocationOriginalAddressPtr32;
extern  UINT32 mRebasedFlagAddr32;

/**
  AP Seamphore operation in 32-bit mode while BSP runs in 64-bit mode.
**/
VOID
SmmRelocationSemaphoreComplete32 (
  VOID
  );

/**
  Hook return address of SMM Save State so that semaphore code
  can be executed immediately after AP exits SMM to indicate to
  the BSP that an AP has exited SMM after SMBASE relocation.

  @param CpuIndex  The processor index.
**/
VOID
SemaphoreHook (
  IN UINTN  CpuIndex
  )
{
  UINTN                         FunctionPointer;
  UINT64                        Efer;
  UINT16                        AutoHaltRestart;
  SOCKET_LGA_775_SMM_CPU_STATE  *CpuState;
  UINTN                         TempValue;

  mRebasedFlag = (BOOLEAN *) &mRebased[CpuIndex];

  //
  // If BSP runs in 64-bit mode executing boot script while APs rendezvous
  // in 32-bit mode. In this case semaphore code should be 32-bit code instead of x64 code.
  //
  // Note that this module is loaded below 4G, so truncation of 64-bit address to 32-bit for 32-bit semaphore
  // code is safe.
  //
  CpuState = NULL;
  
  if (!mSmmInitSaveStateInMsr) {
    CpuState = (SOCKET_LGA_775_SMM_CPU_STATE *)(UINTN)(SMM_DEFAULT_SMBASE + SMRAM_SAVE_STATE_MAP_OFFSET);

    //
    // We are now in 64-bit mode, so SMM Save State Map must be x64 format.
    //

    mSmmRelocationOriginalAddress = CpuState->x64._RIP;
    Efer = CpuState->x64.IA32_EFER;
    AutoHaltRestart = CpuState->x86.AutoHALTRestart;
  } else {
    mSmmRelocationOriginalAddress = AsmReadMsr64 (EFI_MSR_HASWELL_RIP);
    Efer = AsmReadMsr64 (EFI_MSR_HASWELL_EFER);
    AutoHaltRestart = (UINT16)RShiftU64 (AsmReadMsr64 (EFI_MSR_HASWELL_EVENT_CTL_HLT_IO), 16);
  }

  if ((Efer & LMA) == 0) {
    //
    // Use temp value to fix ICC complier warning
    // 
    TempValue                          = (UINTN)&mSmmRelocationOriginalAddress;
    mSmmRelocationOriginalAddressPtr32 = (UINT32)TempValue;
    mRebasedFlagAddr32                 = (UINT32)(UINTN)mRebasedFlag;

    FunctionPointer                    = (UINTN)&SmmRelocationSemaphoreComplete32;
  } else {
    FunctionPointer                    = (UINTN)&SmmRelocationSemaphoreComplete;
  }

  if (!mSmmInitSaveStateInMsr) {
    CpuState->x64._RIP = FunctionPointer;
  } else {
    AsmWriteMsr64 (EFI_MSR_HASWELL_RIP, FunctionPointer);
  }

  if ((AutoHaltRestart & BIT0) != 0) {
    //
    // Clear the auto HALT restart flag so the RSM instruction returns 
    //   program control to the instruction following the HLT instruction,
    //   actually returns to SmmRelocationSemaphoreComplete
    //
    if (!mSmmInitSaveStateInMsr) {
      CpuState->x86.AutoHALTRestart &= ~BIT0;
    } else {
      AsmMsrBitFieldAnd64 (EFI_MSR_HASWELL_EVENT_CTL_HLT_IO, 16, 16, 0);
    }
  } 
}
          
