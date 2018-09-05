/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

Page table manipulation functions for IA-32 processors

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

// PURLEY_OVERRIDE_BEGIN_4929970
ALIGN128 SPIN_LOCK                  mPFLock;
// PURLEY_OVERRIDE_END_4929970

/**
  Create PageTable for SMM use.

  @return     PageTable Address    

**/
UINT32
SmmInitPageTable (
  VOID
  )
{
  UINTN                             PageFaultHandlerHookAddress;
  IA32_IDT_GATE_DESCRIPTOR          *IdtEntry;

  //
  // Initialize spin lock
  //
  InitializeSpinLock (&mPFLock);

  if (FeaturePcdGet (PcdCpuSmmProfileEnable)) {
    //
    // Set own Page Fault entry instead of the default one, because SMM Profile
    // feature depends on IRET instruction to do Single Step
    //
    PageFaultHandlerHookAddress = (UINTN)PageFaultIdtHandlerSmmProfile;
    IdtEntry  = (IA32_IDT_GATE_DESCRIPTOR *) gcSmiIdtr.Base;
    IdtEntry += EXCEPT_IA32_PAGE_FAULT;
    IdtEntry->Bits.OffsetLow      = (UINT16)PageFaultHandlerHookAddress;
    IdtEntry->Bits.Reserved_0     = 0;
    IdtEntry->Bits.GateType       = IA32_IDT_GATE_TYPE_INTERRUPT_32;
    IdtEntry->Bits.OffsetHigh     = (UINT16)(PageFaultHandlerHookAddress >> 16);
  } else {
    //
    // Register Smm Page Fault Handler
    //
    SmmRegisterExceptionHandler (&mSmmCpuService, EXCEPT_IA32_PAGE_FAULT, SmiPFHandler);
  }

  //
  // Additional SMM IDT initialization for Smm stack guard
  //
  if (FeaturePcdGet (PcdCpuSmmStackGuard)) {
    InitializeIDTSmmStackGuard ();
  } 
  return Gen4GPageTable (0);
}

/**
  Page Fault handler for SMM use.

**/
VOID
SmiDefaultPFHandler (
  VOID
  )
{
  CpuDeadLoop ();
}

/**
  ThePage Fault handler wrapper for SMM use.

  @param  InterruptType    Defines the type of interrupt or exception that
                           occurred on the processor.This parameter is processor architecture specific.
  @param  SystemContext    A pointer to the processor context when
                           the interrupt occurred on the processor.
**/
VOID
EFIAPI
SmiPFHandler (
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
  UINTN             PFAddress;

  ASSERT (InterruptType == EXCEPT_IA32_PAGE_FAULT);

  AcquireSpinLock (&mPFLock);
  
  PFAddress = AsmReadCr2 ();

  if ((FeaturePcdGet (PcdCpuSmmStackGuard)) && 
      (PFAddress >= mCpuHotPlugData.SmrrBase) && 
      (PFAddress < (mCpuHotPlugData.SmrrBase + mCpuHotPlugData.SmrrSize))) {
    DEBUG ((EFI_D_ERROR, "SMM stack overflow!\n"));
    CpuDeadLoop ();
  }

  //
  // If a page fault occurs in SMM range
  //
  if ((PFAddress < mCpuHotPlugData.SmrrBase) || 
      (PFAddress >= mCpuHotPlugData.SmrrBase + mCpuHotPlugData.SmrrSize)) {
    if ((SystemContext.SystemContextIa32->ExceptionData & IA32_PF_EC_ID) != 0) {
      DEBUG ((EFI_D_ERROR, "Code executed on IP(0x%x) out of SMM range after SMM is locked!\n", PFAddress));
      DEBUG_CODE (
        DumpModuleInfoByIp (*(UINTN *)(UINTN)SystemContext.SystemContextIa32->Esp);
      );
      CpuDeadLoop ();
    }
  }

  if (FeaturePcdGet (PcdCpuSmmProfileEnable)) {
    SmmProfilePFHandler (
      SystemContext.SystemContextIa32->Eip,
      SystemContext.SystemContextIa32->ExceptionData
      );
  } else {
    SmiDefaultPFHandler ();
  }  
  
  ReleaseSpinLock (&mPFLock);
}
