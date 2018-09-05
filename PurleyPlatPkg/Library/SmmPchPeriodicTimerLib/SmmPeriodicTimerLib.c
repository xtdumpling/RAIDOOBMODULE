//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  SMM Periodic timer library.

  Copyright (c) 2014 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <PiSmm.h>

#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/PchSmmPeriodicTimerControl.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>


EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL   *mSmmPeriodicTimerDispatch = NULL;

PCH_SMM_PERIODIC_TIMER_CONTROL_PROTOCOL     *mPchSmmPeriodicTimerControl = NULL;

EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT     mPeriodicTimer;

EFI_SMM_HANDLER_ENTRY_POINT2                mDispatchFunction = NULL;

EFI_HANDLE                                  mPeriodicTimerHandle = NULL;

BOOLEAN                                     mStarted = FALSE;


/**
  Main entry point for an SMM handler dispatch or communicate-based callback.

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers 
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should 
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still 
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.
**/
EFI_STATUS
EFIAPI
InternalDispatchFunction (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  if (!mStarted) {
    //
    // In case SMI periodic timer has been launched by other handler
    //
    return EFI_NOT_READY;
  }

  return mDispatchFunction (DispatchHandle, Context, CommBuffer, CommBufferSize);
}


/**
  Sets periodic SMM timer.

  This function initialize periodic SMM timer so that it can be used with StartSmmPeriodicTimer and
  EndSmmPeriodicTimer.

  @param[in] Period             The minimum period of time in 100 nanosecond units that the child gets called. The 
                                child will be called back after a time greater than the time Period.
  @param[in] Interval           The period of time interval between SMIs. Children of this interface should use this 
                                field when registering for periodic timer intervals when a finer granularity periodic 
                                SMI is desired.
  @param[in] DispatchFunction   Function to register for handler when at least the specified amount of time has elapsed.

  @retval EFI_SUCCESS           SMM periodic timer is successfully set.
  @return others                Some error occurs when setting periodic SMM timer.

**/
EFI_STATUS
EFIAPI
SetSmmPeriodicTimer (
  IN UINT64                        Period,
  IN UINT64                        Interval,
  IN EFI_SMM_HANDLER_ENTRY_POINT2  DispatchFunction
  )
{
  EFI_STATUS      Status;

  mPeriodicTimer.Period = Period;
  mPeriodicTimer.SmiTickInterval = Interval;
  mDispatchFunction = DispatchFunction;
  
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmPeriodicTimerDispatch2ProtocolGuid, 
                    NULL, 
                    &mSmmPeriodicTimerDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gPchSmmPeriodicTimerControlGuid,
                    NULL,
                    &mPchSmmPeriodicTimerControl
                    );
  ASSERT_EFI_ERROR (Status);
  
  DEBUG ((EFI_D_INFO, "Register SMM Periodic Timer Period %lx, Interval %lx, Status=%r\n", Period, Interval, Status));


  Status = mSmmPeriodicTimerDispatch->Register(
                                        mSmmPeriodicTimerDispatch,
                                        InternalDispatchFunction,
                                        &mPeriodicTimer,
                                        &mPeriodicTimerHandle
                                        );
  DEBUG ((EFI_D_INFO, "Start SMM periodic Timer, status=%r\n", Status));
  ASSERT_EFI_ERROR (Status);

  Status = mPchSmmPeriodicTimerControl->Pause (mPchSmmPeriodicTimerControl, mPeriodicTimerHandle);
  DEBUG ((EFI_D_INFO, "Pause SMM Periodic timer returns %r\n", Status));
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Starts periodic SMM timer.

  This function starts periodic SMM timer initialized with SetSmmPeriodicTimer()

  @retval EFI_SUCCESS           SMM periodic timer is successfully started.
  @return others                Some error occurs when starting periodic SMM timer.

**/
EFI_STATUS
EFIAPI
StartSmmPeriodicTimer (
  VOID
  )
{
  EFI_STATUS              Status;

  //
  // Ensure we have set SMM periodic timer before
  //
  ASSERT (mPeriodicTimerHandle != NULL);

  Status = mPchSmmPeriodicTimerControl->Resume (mPchSmmPeriodicTimerControl, mPeriodicTimerHandle);
  DEBUG ((EFI_D_INFO, "Starting SMM Periodic timer returns %r\n", Status));
  mStarted = TRUE;

  return Status;
}


/**
  Ends periodic SMM timer.

  This function ends periodic SMM timer initialized with SetSmmPeriodicTimer()

  @retval EFI_SUCCESS         SMM periodic timer is successfully ended.
  @return others              Some error occurs when ending periodic SMM timer.

**/
EFI_STATUS
EFIAPI
EndSmmPeriodicTimer (
  VOID
  )
{
  EFI_STATUS                  Status;

  mStarted = FALSE;
  Status = mPchSmmPeriodicTimerControl->Pause (mPchSmmPeriodicTimerControl, mPeriodicTimerHandle);
  DEBUG ((EFI_D_INFO, "Ending SMM Periodic timer returns %r\n", Status));

  return Status;
}
