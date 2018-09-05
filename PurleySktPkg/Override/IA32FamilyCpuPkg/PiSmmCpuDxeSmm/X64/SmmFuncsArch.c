/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  SMM CPU misc functions for x64 arch specific.

Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
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
// IA32_IDT_GATE_DESCRIPTOR is aliased here for X64 build benefit
//
typedef union {
  struct {
    UINT32  OffsetLow:16;   ///< Offset bits 15..0.
    UINT32  Selector:16;    ///< Selector.
    UINT32  Reserved_0:8;   ///< Reserved.
    UINT32  GateType:8;     ///< Gate Type.  See #defines above.
    UINT32  OffsetHigh:16;  ///< Offset bits 31..16.
  } Bits;
  UINT64  Uint64;
} SMM_PM_IA32_IDT_GATE_DESCRIPTOR;

UINTN                         mMceHandlerLongMode;
IA32_DESCRIPTOR               mSmiPMIdtr;
UINT32                        gProtModeIdtr;
extern UINTN                  SmiPMExceptionEntryPoints;

/**
  Protected Mode IDT handler of machine check.

**/
VOID
EFIAPI
PMIdtHandlerMachineCheck (
  VOID
  );

/**
  Protected Mode IDT handler.

**/
VOID
EFIAPI
PMIdtHandler (
  VOID
  );

/**
  Initialize SMM Protected Mode IDT table.

**/
VOID
InitProtectedModeIdt (
  VOID
  )
{
  UINTN                             Index;
  UINTN                             PmIdtSize;
  SMM_PM_IA32_IDT_GATE_DESCRIPTOR   *PmIdtEntry;
  UINTN                             InterruptHandler;
  IA32_IDT_GATE_DESCRIPTOR          *IdtEntry;
  
  //
  // Allocate IDT table size 
  //
  PmIdtSize = sizeof (SMM_PM_IA32_IDT_GATE_DESCRIPTOR) * (EXCEPT_IA32_MACHINE_CHECK + 1) * 2;
  mSmiPMIdtr.Base  = (UINTN) AllocateZeroPool (PmIdtSize);
  ASSERT (mSmiPMIdtr.Base != 0);
  mSmiPMIdtr.Limit = (UINT16) PmIdtSize - 1;
  PmIdtEntry =  (SMM_PM_IA32_IDT_GATE_DESCRIPTOR *)(mSmiPMIdtr.Base);

  gProtModeIdtr = (UINT32)(UINTN)&mSmiPMIdtr;
  //
  // Set up IA32 IDT handler
  //
  for (Index = 0; Index < EXCEPTION_VECTOR_NUMBER; Index++) {
    if (Index == EXCEPT_IA32_MACHINE_CHECK) {
      InterruptHandler = (UINTN)PMIdtHandlerMachineCheck;
    } else {
      InterruptHandler = (UINTN)PMIdtHandler;
    }
    PmIdtEntry[Index].Bits.Selector   = PROTECT_MODE_CODE_SEGMENT;
    PmIdtEntry[Index].Bits.GateType   = IA32_IDT_GATE_TYPE_INTERRUPT_32;
    PmIdtEntry[Index].Bits.OffsetLow  = (UINT16) (0x0000FFFF & InterruptHandler);
    PmIdtEntry[Index].Bits.OffsetHigh = (UINT16) (0x0000FFFF & (InterruptHandler >> 16));
  }
  //
  // Set X64 MCA IDT handler at location 0x24&0x25 in IA32 IDT Table. In case, MCA issues
  // before X64 IDT table is loaded in long mode, this MCA IDT handler will be invoked.
  //
  IdtEntry  = (IA32_IDT_GATE_DESCRIPTOR *) gcSmiIdtr.Base;
  IdtEntry += EXCEPT_IA32_MACHINE_CHECK;
  mMceHandlerLongMode = IdtEntry->Bits.OffsetLow + (((UINTN) IdtEntry->Bits.OffsetHigh)  << 16) +
                                    (((UINTN) IdtEntry->Bits.OffsetUpper) << 32);
  IdtEntry  = (IA32_IDT_GATE_DESCRIPTOR *) mSmiPMIdtr.Base;
  IdtEntry += EXCEPT_IA32_MACHINE_CHECK;
  IdtEntry->Bits.Selector       = LONG_MODE_CODE_SEGMENT;
  IdtEntry->Bits.OffsetLow      = (UINT16)mMceHandlerLongMode;
  IdtEntry->Bits.Reserved_0     = 0;
  IdtEntry->Bits.GateType       = IA32_IDT_GATE_TYPE_INTERRUPT_32;
  IdtEntry->Bits.OffsetHigh     = (UINT16)(mMceHandlerLongMode >> 16);
  IdtEntry->Bits.OffsetUpper    = (UINT32)(mMceHandlerLongMode >> 32);
  IdtEntry->Bits.Reserved_1     = 0;
}
