/** @file
  Task priority (TPL) functions.

Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "DxeMain.h"
#include "Event.h"

/**
  Set Interrupt State.

  @param  Enable  The state of enable or disable interrupt

**/
VOID
CoreSetInterruptState (
  IN BOOLEAN      Enable
  )
{
  EFI_STATUS  Status;
  BOOLEAN     InSmm;
  
  if (gCpu == NULL) {
    return;
  }
  if (!Enable) {
    gCpu->DisableInterrupt (gCpu);
    return;
  }
  if (gSmmBase2 == NULL) {
    gCpu->EnableInterrupt (gCpu);
    return;
  }
  Status = gSmmBase2->InSmm (gSmmBase2, &InSmm);
  if (!EFI_ERROR (Status) && !InSmm) {
    gCpu->EnableInterrupt(gCpu);
  }
}


/**
  Raise the task priority level to the new level.
  High level is implemented by disabling processor interrupts.

  @param  NewTpl  New task priority level

  @return The previous task priority level

**/
EFI_TPL
EFIAPI
CoreRaiseTpl (
  IN EFI_TPL      NewTpl
  )
{
  EFI_TPL     OldTpl;

  OldTpl = gEfiCurrentTpl;
  if (OldTpl > NewTpl) {
    DEBUG ((EFI_D_ERROR, "FATAL ERROR - RaiseTpl with OldTpl(0x%x) > NewTpl(0x%x)\n", OldTpl, NewTpl));
    ASSERT (FALSE);
  }
  ASSERT (VALID_TPL (NewTpl));

  //
  // If raising to high level, disable interrupts
  //
  if (NewTpl >= TPL_HIGH_LEVEL  &&  OldTpl < TPL_HIGH_LEVEL) {
    CoreSetInterruptState (FALSE);
  }

  //
  // Set the new value
  //
  gEfiCurrentTpl = NewTpl;

  return OldTpl;
}




/**
  Lowers the task priority to the previous value.   If the new
  priority unmasks events at a higher priority, they are dispatched.

  @param  NewTpl  New, lower, task priority

**/
VOID
EFIAPI
CoreRestoreTpl (
  IN EFI_TPL NewTpl
  )
{
  EFI_TPL     OldTpl;

  OldTpl = gEfiCurrentTpl;
  if (NewTpl > OldTpl) {
    DEBUG ((EFI_D_ERROR, "FATAL ERROR - RestoreTpl with NewTpl(0x%x) > OldTpl(0x%x)\n", NewTpl, OldTpl));
    ASSERT (FALSE);
  }
  ASSERT (VALID_TPL (NewTpl));

  //
  // If lowering below HIGH_LEVEL, make sure
  // interrupts are enabled
  //

  if (OldTpl >= TPL_HIGH_LEVEL  &&  NewTpl < TPL_HIGH_LEVEL) {
    gEfiCurrentTpl = TPL_HIGH_LEVEL;
  }

  //
  // Dispatch any pending events
  //
  //*** AMI PORTING BEGIN ***//
  // Improvement: The code below relies on undefined behavior. Shift of negative value is undefined in C99 standard.
  /*Relevant quote from ISO C99 (6.5.7/4):
    The result of E1 << E2 is E1 left-shifted E2 bit positions; vacated bits
    are filled with zeros. If E1 has an unsigned type, the value of the result
    is E1 x 2^E2, reduced modulo one more than the maximum value representable
    in the result type. If E1 has a signed type and nonnegative value, and
    E1 x 2^E2 is representable in the result type, then that is the resulting
    value; otherwise, the behavior is undefined.
  */
  //while (((-2 << NewTpl) & gEventPending) != 0) {
  while ((((UINTN)-2 << NewTpl) & gEventPending) != 0) {
  //*** AMI PORTING END *****//  	  
    gEfiCurrentTpl = (UINTN) HighBitSet64 (gEventPending);
    if (gEfiCurrentTpl < TPL_HIGH_LEVEL) {
      CoreSetInterruptState (TRUE);
    }
    CoreDispatchEventNotifies (gEfiCurrentTpl);
  }

  //
  // Set the new value
  //

  gEfiCurrentTpl = NewTpl;

  //
  // If lowering below HIGH_LEVEL, make sure
  // interrupts are enabled
  //
  if (gEfiCurrentTpl < TPL_HIGH_LEVEL) {
    CoreSetInterruptState (TRUE);
  }

}
