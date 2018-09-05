//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CpuDataStruct.h

Abstract:

--*/

#ifndef _CPU_BASE_LIB_H
#define _CPU_BASE_LIB_H

#include <CpuRegs.h>
#include <CpuCsr.h>
#include <CpuDataStruct.h>

//
// Combine f(FamilyId), m(Model), s(SteppingId) to a single 32 bit number
//
#define EfiMakeCpuVersion(f, m, s)         \
  (((UINT32) (f) << 16) | ((UINT32) (m) << 8) | ((UINT32) (s)))

#ifdef __GNUC__
#define IA32API
#else
#define IA32API __cdecl
#endif

VOID
IA32API
EfiHalt (
  VOID
  )
;

/*++                                                                                                                               
Routine Description:                                                
  Halt the Cpu    
Arguments:                
   None                                                          
Returns:                                                            
   None                                                
--*/
VOID
IA32API
EfiWbinvd (
  VOID
  )
;

/*++                                                                                                                               
Routine Description:                                                
  Write back and invalidate the Cpu cache
Arguments:                
   None                                                          
Returns:                                                            
   None                                                
--*/
VOID
IA32API
EfiInvd (
  VOID
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Invalidate the Cpu cache
Arguments:                
   None                                                          
Returns:                                                            
   None                                                
--*/
VOID
IA32API
EfiCpuid (
  IN  UINT32                 RegisterInEax,
  OUT EFI_CPUID_REGISTER     *Regs
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Get the Cpu info by excute the CPUID instruction
Arguments:                
   RegisterInEax: -The input value to put into register EAX
   Regs:          -The Output value                      
Returns:                                                            
   None                                                
--*/

VOID
IA32API
EfiCpuidExt (
  IN  UINT32                 RegisterInEax,
  IN  UINT32                 CacheLevel,
  OUT EFI_CPUID_REGISTER     *Regs
  )
/*++                                                                                                                          
Routine Description:                                                
  When RegisterInEax != 4, the functionality is the same as EfiCpuid.
  When RegisterInEax == 4, the function return the deterministic cache
  parameters by excuting the CPUID instruction
Arguments:                
   RegisterInEax: - The input value to put into register EAX
   CacheLevel:      - The deterministic cache level
   Regs:          - The Output value                      
Returns:                                                            
   None                                                
--*/
;

UINT64
IA32API
EfiReadMsr (
  IN UINT32     Index
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Read Cpu MSR
Arguments:                
  Index: -The index value to select the register
                       
Returns:                                                            
   Return the read data                                                
--*/
VOID
IA32API
EfiWriteMsr (
  IN UINT32     Index,
  IN UINT64     Value
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Write Cpu MSR
Arguments:                
  Index: -The index value to select the register
  Value: -The value to write to the selected register                      
Returns:                                                            
   None                                                
--*/
UINT64
IA32API
EfiReadTsc (
  VOID
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Read Time stamp
Arguments:                
  None                 
Returns:                                                            
   Return the read data                                                
--*/
VOID
IA32API
EfiDisableCache (
  VOID
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Writing back and invalidate the cache,then diable it
Arguments:                
  None                 
Returns:                                                            
  None                                               
--*/
VOID
IA32API
EfiEnableCache (
  VOID
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Invalidate the cache,then Enable it
Arguments:                
  None                 
Returns:                                                            
  None                                               
--*/
UINT32
IA32API
EfiGetEflags (
  VOID
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Get Eflags
Arguments:                
  None                 
Returns:                                                            
  Return the Eflags value                                               
--*/
VOID
IA32API
EfiDisableInterrupts (
  VOID
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Disable Interrupts
Arguments:                
  None                 
Returns:                                                            
  None
--*/
VOID
IA32API
EfiEnableInterrupts (
  VOID
  )
;

/*++                                                                                                                          
Routine Description:                                                
  Enable Interrupts
Arguments:                
  None                 
Returns:                                                            
  None                                               
--*/


VOID
IA32API
EfiCpuVersion (
  IN   UINT16  *FamilyId,    OPTIONAL
  IN   UINT8   *Model,       OPTIONAL
  IN   UINT8   *SteppingId,  OPTIONAL
  IN   UINT8   *Processor    OPTIONAL
  )
/*++

Routine Description:
  Extract CPU detail version infomation

Arguments:
  FamilyId   - FamilyId, including ExtendedFamilyId
  Model      - Model, including ExtendedModel
  SteppingId - SteppingId
  Processor  - Processor

--*/
;

#endif
