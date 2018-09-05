/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  SMM profile header file.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved. <BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _SMM_PROFILE_H_
#define _SMM_PROFILE_H_

#include "SmmProfileInternal.h"
//
// External functions
//

/**
  Initialize processor environment for SMM profile.

  @param  CpuIndex  The index of the processor.

**/
VOID
ActivateSmmProfile (
  IN UINTN CpuIndex
  );

/**
  Initialize SMM profile in SMM CPU entrypoint.

**/
VOID
InitSmmProfile (
  VOID
  );

/**
  Increase SMI number in each SMI entry.

**/
VOID
SmmProfileRecordSmiNum (
  VOID
  );

/**
  The Page fault handler to save SMM profile data.

  @param  Rip        The RIP when exception happens.
  @param  ErrorCode  The Error code of exception.

**/
VOID
SmmProfilePFHandler (
  UINTN Rip,
  UINTN ErrorCode
  );

/**
  Updates page table to make some memory ranges (like system memory) absent
  and make some memory ranges (like MMIO) present and execute disable. It also
  update 2MB-page to 4KB-page for some memory ranges.

**/
VOID
SmmProfileStart (
  VOID
  );

/**
  Page fault IDT handler for SMM Profile.

**/
VOID
EFIAPI
PageFaultIdtHandlerSmmProfile (
  VOID
  );
  

/**
  Check if XD feature is supported by a processor.

  @param[in,out] Buffer  The pointer to private data buffer.

**/
VOID
EFIAPI
CheckFeatureSupported (
  IN OUT VOID   *Buffer
  );

/**
  Enable XD feature.

**/
VOID
ActivateXd (
  VOID
  );

/**
  Update page table according to protected memory ranges and the 4KB-page mapped memory ranges.

**/
VOID
InitPaging (
  VOID
  );

/**
  Check if XD and BTS features are supported by all processors.
  
**/
VOID
CheckProcessorFeature (
  VOID
  );

extern BOOLEAN    mXdSupported;
extern BOOLEAN    mXdEnabled;

#endif // _SMM_PROFILE_H_
