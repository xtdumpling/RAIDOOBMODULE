//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Public include file for the SMM CPU Platform Hook Library.

  Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef __SMM_CPU_PLATFORM_HOOK_LIB_H__
#define __SMM_CPU_PLATFORM_HOOK_LIB_H__

/**
  Checks if platform produces a valid SMI. 
  
  This function checks if platform produces a valid SMI. This function is 
  called at SMM entry to detect if this is a spurious SMI. This function 
  must be implemented in an MP safe way because it is called by multiple CPU
  threads.
  
  @retval TRUE              There is a valid SMI
  @retval FALSE             There is no valid SMI

**/
BOOLEAN
EFIAPI
PlatformValidSmi (
  VOID
  );

/**
  Clears platform top level SMI status bit. 
  
  This function clears platform top level SMI status bit.
  
  @retval TRUE              The platform top level SMI status is cleared.
  @retval FALSE             The paltform top level SMI status cannot be cleared.

**/
BOOLEAN
EFIAPI
ClearTopLevelSmiStatus (
  VOID
  );

/**
  Init internal variables

**/
EFI_STATUS
EFIAPI
InitSmmRuntimeCtlHooks(
  VOID
);

/**
  Clears Pending SMI 
  
  This function clears pending SMI using . SMM_RUNTIME_CTL.
  
  @param[in,out] Buffer  Pointer to private data buffer.

**/
VOID
EFIAPI
ClearAnyPendingSmi (
  IN OUT VOID  *Buffer
);

/**
  Morphing off 
  
  This function turns off MSMI to MCE morphing by clearing SMM_RUNTIME_CTL
  
  @param[in,out] Buffer  Pointer to private data buffer.

**/
VOID
EFIAPI
TurnOffMorphing (
  IN OUT VOID  *Buffer
);


/**
  Set Flag to Cause Smm Cpu to pend SMI IPI at end of the flow
  
  @param Value             Value of the flag.
**/
VOID
EFIAPI
SetPendSmiFlag(
  BOOLEAN Value
  );

/**
  Get value of Flag that Cause Smm Cpu to pend SMI IPI at end of the flow
  
  @retval TRUE              SMI will be pended.
  @retval FALSE             SMI will not pended.

**/
BOOLEAN
EFIAPI
GetPendSmiFlag(
  VOID
  );

/**
  Performs an atomic compare exchange operation on EmcaHandlerState.

  Performs an atomic compare exchange operation on the EmcaHandlerState to make state transition. 
  If original value is equal to CompareValue, then Value is set to ExchangeValue and
  CompareValue is returned.  If original value is not equal to CompareValue, then original value is returned.
  The compare exchange operation must be performed using MP safe mechanisms.


  @param  CompareValue  64-bit value used in compare operation.
  @param  ExchangeValue 64-bit value used in exchange operation.

  @return The original EmcaHandlerState before exchange.

**/
UINT64
EFIAPI
TransitionEmcaHandlerState(
  IN UINT64 CompareValue,
  IN UINT64 ExchangeValue
  );

/**
  Get value of EmcaHandlerState
  
  @return EmcaHandlerState value.

**/
UINT64
EFIAPI
GetEmcaHandlerState(
  VOID
  );

/**
  Set value of EmcaHandlerState
  
  @param EmcaHandlerState value.

**/
VOID
EFIAPI
SetEmcaHandlerState(
  UINT64 Value
  );


/**
  Performs platform specific way of SMM BSP election.
  
  This function performs platform specific way of SMM BSP election.
  
  @param  IsBsp             Output parameter. TRUE: the CPU this function executes
                            on is elected to be the SMM BSP. FALSE: the CPU this 
                            function executes on is to be SMM AP.

  @retval EFI_SUCCESS       The function executes successfully.
  @retval EFI_NOT_READY     The function does not determine whether this CPU should be
                            BSP or AP. This may occur if hardware init sequence to
                            enable the determination is yet to be done, or the function
                            chooses not to do BSP election and will let SMM CPU driver to
                            use its default BSP election process.
  @retval EFI_DEVICE_ERROR  The function cannot determine whether this CPU should be
                            BSP or AP due to hardware error.

**/
EFI_STATUS
EFIAPI
PlatformSmmBspElection (
  OUT BOOLEAN     *IsBsp
  );


typedef enum {
    SmmPageSize4K,
    SmmPageSize2M,
    SmmPageSize1G,
    MaxSmmPageSizeType
} SMM_PAGE_SIZE_TYPE;

typedef enum {
  SmmMceNotStartedState = 0,
  SmmMceInProgressState,
  SmmMceFinishedState,
  SmmMceInterruptedState
} SMM_MCE_HANDLER_SYNC_STATE;



/**
  Get platform page table attribute .

  This function gets page table attribute of platform.

  @param  Address        Input parameter. Obtain the page table entries attribute on this address.
  @param  PageSize       Output parameter. The size of the page.
  @param  NumOfPages     Output parameter. Number of page.
  @param  PageAttribute  Output parameter. Paging Attributes (WB, UC, etc).
  
  @retval EFI_SUCCESS       The platform page table attribute from the address is determined.
  @retval EFI_UNSUPPORTED  The paltform not supoort to get page table attribute from the address.

**/
EFI_STATUS
EFIAPI
GetPlatformPageTableAttribute (
  IN  UINT64                Address,
  OUT SMM_PAGE_SIZE_TYPE    *PageSize,
  OUT UINTN                 *NumOfPages,
  OUT UINTN                 *PageAttribute
  );

#endif   
