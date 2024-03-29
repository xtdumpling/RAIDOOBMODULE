/** @file
  ARM specifc functionality for DxeLoad. 
  
Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "DxeIpl.h"

#include <Library/ArmLib.h>

/**
   Transfers control to DxeCore.

   This function performs a CPU architecture specific operations to execute
   the entry point of DxeCore with the parameters of HobList.
   It also installs EFI_END_OF_PEI_PPI to signal the end of PEI phase.

   @param DxeCoreEntryPoint         The entry point of DxeCore.
   @param HobList                   The start of HobList passed to DxeCore.

**/
VOID
HandOffToDxeCore (
  IN EFI_PHYSICAL_ADDRESS   DxeCoreEntryPoint,
  IN EFI_PEI_HOB_POINTERS   HobList
  )
{
  VOID                *BaseOfStack;
  VOID                *TopOfStack;
  EFI_STATUS          Status;

  //
  // Allocate 128KB for the Stack
  //
  BaseOfStack = AllocatePages (EFI_SIZE_TO_PAGES (STACK_SIZE));
  ASSERT (BaseOfStack != NULL);

  if (PcdGetBool (PcdSetNxForStack)) {
    Status = ArmSetMemoryRegionNoExec ((UINTN)BaseOfStack, STACK_SIZE);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Compute the top of the stack we were allocated. Pre-allocate a UINTN
  // for safety.
  //
  TopOfStack = (VOID *) ((UINTN) BaseOfStack + EFI_SIZE_TO_PAGES (STACK_SIZE) * EFI_PAGE_SIZE - CPU_STACK_ALIGNMENT);
  TopOfStack = ALIGN_POINTER (TopOfStack, CPU_STACK_ALIGNMENT);

  //
  // End of PEI phase singal
  //
  //*** AMI PORTING BEGIN ***//
  PERF_START (NULL,"PeiEnd", NULL, 0);
  //*** AMI PORTING END *****//  
  Status = PeiServicesInstallPpi (&gEndOfPeiSignalPpi);
  //*** AMI PORTING BEGIN ***//
  PERF_END (NULL,"PeiEnd", NULL, 0);
  PERF_END (NULL,"DxeIpl", NULL, 0);
  //*** AMI PORTING END *****//    
  ASSERT_EFI_ERROR (Status);

  //
  // Update the contents of BSP stack HOB to reflect the real stack info passed to DxeCore.
  //    
  UpdateStackHob ((EFI_PHYSICAL_ADDRESS)(UINTN) BaseOfStack, STACK_SIZE);
  
//*** AMI PORTING BEGIN ***//
  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "DXE-Core.Entry(%p)\n", DxeCoreEntryPoint));
  REPORT_STATUS_CODE(EFI_PROGRESS_CODE,DXE_CORE_STARTED);
//*** AMI PORTING END *****//

  SwitchStack (
    (SWITCH_STACK_ENTRY_POINT)(UINTN)DxeCoreEntryPoint,
    HobList.Raw,
    NULL,
    TopOfStack
    );
}
