/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/** @file
  Null instance of Sec Platform Hook Lib.

  Copyright (c) 2014, Intel Corporation. All rights reserved. <BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


**/

//
// The package level header files this module uses
//
#include "ProcessorStartup.h"
#include <Library/MtrrLib.h>
#include <CpuHost.h>

/**
  A developer supplied function to perform platform specific operations.

  It's a developer supplied function to perform any operations appropriate to a
  given platform. It's invoked just before passing control to PEI core by SEC
  core. Platform developer may modify the SecCoreData and PPI list that is
  passed to PEI Core. 

  @param  SecCoreData           The same parameter as passing to PEI core. It
                                could be overridden by this function.
  @param  PpiList               The default PPI list passed from generic SEC
                                part.

  @return The final PPI list that platform wishes to passed to PEI core.

**/
EFI_PEI_PPI_DESCRIPTOR *
EFIAPI
SecPlatformMain (
  IN OUT   EFI_SEC_PEI_HAND_OFF        *SecCoreData,
  IN       EFI_PEI_PPI_DESCRIPTOR      *PpiList
  )
{
  return NULL;
}


/**
  This interface conveys state information out of the Security (SEC) phase into PEI.

  @param  PeiServices               Pointer to the PEI Services Table.
  @param  StructureSize             Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN OUT   UINT64                                *StructureSize,
  OUT      EFI_SEC_PLATFORM_INFORMATION_RECORD   *PlatformInformationRecord
  )
{
  UINT32  *BIST;
  UINT32  Size;
  UINT32  Count;

  //
  // The entries of BIST information, together with the number of them,
  // reside in the bottom of stack, left untouched by normal stack operation.
  // This routine copies the BIST information to the buffer pointed by
  // PlatformInformationRecord for output.
  //
  Count = *(UINT32*)(TopOfCar - (sizeof (UINT32)*4) - 1);
  Size  = Count * sizeof (UINT32);

  if ((*StructureSize) < (UINT64) Size) {
    *StructureSize = Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StructureSize  = Size;
  BIST            = (UINT32*)(TopOfCar - (sizeof (UINT32)*4) - 1 - Count);

  CopyMem (PlatformInformationRecord, BIST, Size);

  return EFI_SUCCESS;
}

/**
  This interface disables temporary memory in SEC Phase.
**/
VOID
EFIAPI
SecPlatformDisableTemporaryMemory (  
  VOID
  )
{
  UINT64  MsrValue;
  UINT64  MtrrDefaultType;

  //
  // Force and INVD.
  //
  AsmInvd ();

  //
  // Disable MTRRs.
  //
  MtrrDefaultType = AsmReadMsr64 (MTRR_LIB_IA32_MTRR_DEF_TYPE);
  MsrValue = MtrrDefaultType & ~((UINT64)MTRR_LIB_CACHE_MTRR_ENABLED);
  AsmWriteMsr64 (MTRR_LIB_IA32_MTRR_DEF_TYPE, MsrValue);
  
  //
  // Clear NEM Run and NEM Setup bits individually.
  //
  MsrValue = AsmReadMsr64 (MSR_NO_EVICT_MODE);
  MsrValue &= ~((UINT64) BIT1);
  AsmWriteMsr64 (MSR_NO_EVICT_MODE, MsrValue);
  MsrValue &= ~((UINT64) BIT0);
  AsmWriteMsr64 (MSR_NO_EVICT_MODE, MsrValue);
  
  //
  // Restore MTRR default setting
  //
  AsmWriteMsr64 (MTRR_LIB_IA32_MTRR_DEF_TYPE, MtrrDefaultType);
}

