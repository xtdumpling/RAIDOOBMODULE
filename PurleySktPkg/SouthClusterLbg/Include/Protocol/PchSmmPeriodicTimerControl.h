/** @file
  PCH SMM Periodic Timer Control Protocol

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_SMM_PERIODIC_TIMER_CONTROL_H_
#define _PCH_SMM_PERIODIC_TIMER_CONTROL_H_


//
// Extern the GUID for protocol users.
//
extern EFI_GUID                                             gPchSmmPeriodicTimerControlGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_SMM_PERIODIC_TIMER_CONTROL_PROTOCOL     PCH_SMM_PERIODIC_TIMER_CONTROL_PROTOCOL;

//
// Related Definitions
//

//
// Member functions
//

/**
  The Prototype of Pause and Resume SMM PERIODIC TIMER function.

  @param[in] This                       Pointer to the PCH_SMM_PERIODIC_TIMER_CONTROL_PROTOCOL instance.
  @param[in] DispatchHandle             Handle of the child service to change state.

  @retval EFI_SUCCESS                   This operation is complete.
  @retval EFI_INVALID_PARAMETER         The DispatchHandle is invalid.
  @retval EFI_ACCESS_DENIED             The SMI status is alrady PAUSED/RESUMED.
**/
typedef
EFI_STATUS
(EFIAPI *PCH_SMM_PERIODIC_TIMER_CONTROL_FUNCTION) (
  IN PCH_SMM_PERIODIC_TIMER_CONTROL_PROTOCOL      *This,
  IN EFI_HANDLE                                   DispatchHandle
  );

/**
  Interface structure for the SMM PERIODIC TIMER pause and resume protocol
  This protocol provides the functions to runtime control the SM periodic timer enabled/disable.
  This applies the capability to the DispatchHandle which returned by SMM periodic timer callback
  registration.
  Besides, when S3 resume, it only restores the state of callback registration.
  The Paused/Resume state won't be restored after S3 resume.
**/
struct _PCH_SMM_PERIODIC_TIMER_CONTROL_PROTOCOL {
  /**
    This runtime pauses the registered periodic timer handler.
  **/
  PCH_SMM_PERIODIC_TIMER_CONTROL_FUNCTION         Pause;
  /**
    This runtime resumes the registered periodic timer handler.
  **/
  PCH_SMM_PERIODIC_TIMER_CONTROL_FUNCTION         Resume;
};

#endif // _PCH_SMM_PERIODIC_TIMER_CONTROL_H_
