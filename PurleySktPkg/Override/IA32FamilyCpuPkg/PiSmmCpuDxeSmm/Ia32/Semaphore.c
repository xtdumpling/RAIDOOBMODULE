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

UINTN   mSmmRelocationOriginalAddress;
BOOLEAN *mRebasedFlag;

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
  UINT16                        AutoHaltRestart;
  SOCKET_LGA_775_SMM_CPU_STATE  *CpuState;

  mRebasedFlag = (BOOLEAN *) &mRebased[CpuIndex];

  if (!mSmmInitSaveStateInMsr) {
    CpuState = (SOCKET_LGA_775_SMM_CPU_STATE *)(UINTN)(SMM_DEFAULT_SMBASE + SMRAM_SAVE_STATE_MAP_OFFSET);

    //
    // The offset of EIP/RIP is different depending on the SMMRevId
    //
    if (CpuState->x86.SMMRevId < SOCKET_LGA_775_SMM_MIN_REV_ID_x64) {
      mSmmRelocationOriginalAddress = (UINTN) CpuState->x86._EIP;
      CpuState->x86._EIP            = (UINT32) (UINTN) &SmmRelocationSemaphoreComplete;
    } else {
      mSmmRelocationOriginalAddress = (UINTN) CpuState->x64._RIP;
      CpuState->x64._RIP            = (UINT64) (UINTN) &SmmRelocationSemaphoreComplete;
    }

    if (CpuState->x86.AutoHALTRestart & BIT0) {
      //
      // Clear the auto HALT restart flag so the RSM instruction returns 
      //   program control to the instruction following the HLT instruction,
      //   actually returns to SmmRelocationSemaphoreComplete
      //
      CpuState->x86.AutoHALTRestart &= ~BIT0;
    }
  } else {
    mSmmRelocationOriginalAddress = AsmReadMsr32 (EFI_MSR_HASWELL_RIP);
    AsmWriteMsr32 (EFI_MSR_HASWELL_RIP, (UINT32) (UINTN) &SmmRelocationSemaphoreComplete);

    AutoHaltRestart = (UINT16)RShiftU64 (AsmReadMsr64 (EFI_MSR_HASWELL_EVENT_CTL_HLT_IO), 16);
    if ((AutoHaltRestart & BIT0) != 0) {
      AsmMsrBitFieldAnd64 (EFI_MSR_HASWELL_EVENT_CTL_HLT_IO, 16, 16, 0);
    }
  }
}
          
