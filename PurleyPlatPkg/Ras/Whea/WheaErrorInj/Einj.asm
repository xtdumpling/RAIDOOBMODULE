;------------------------------------------------------------------------------
;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;
; Copyright (c)2009-2015 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
;
; Module Name:
;
;   Smm.asm
;
; Abstract:
;
;   Assembly code of SmmCoreDispatcher
;
;------------------------------------------------------------------------------
 
  .CODE

CR0_CD_MASK       equ 40000000h    ;(1 << 30)

mBSPPrefetch  dd  00
mAPPrefetch   dd  00

;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmWbinvd (
;   VOID
;   );
;------------------------------------------------------------------------------
AsmWbinvd   PROC ;NEAR PUBLIC
    wbinvd
    ret
AsmWbinvd   ENDP

;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmNop (
;   VOID
;   );
;------------------------------------------------------------------------------
AsmNop   PROC ;NEAR PUBLIC

  nop
  nop
  
  ret
AsmNop   ENDP

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOffCache (
;   VOID
;   );
;------------------------------------------------------------------------------
TurnOffCache   PROC ;NEAR PUBLIC

  ;Disable Cache
  Mov   Rax, CR0    
  Or    Rax, CR0_CD_MASK  ;Set the CD Bit
  Mov   CR0, Rax
  
  ret
TurnOffCache   ENDP

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOnCache (
;   VOID
;   );
;------------------------------------------------------------------------------
TurnOnCache   PROC ;NEAR PUBLIC

  Mov   Rax, CR0    
  And   Rax, NOT CR0_CD_MASK    ;Clear the CD Bit
  Mov   CR0, Rax
  
  ret
TurnOnCache   ENDP

;;;;;; Added for WheaEinj driver ;;;;;
;------------------------------------------------------------------------------
; VOID *
; EFIAPI 
; AsmFlushCacheLine (
;   IN      VOID                      *LinearAddress
;   );
;------------------------------------------------------------------------------
AsmFlushCacheLine   PROC
    mfence
    clflush [rcx]
    mfence
    mov     rax, rcx
    ret
AsmFlushCacheLine   ENDP

;;;;;; Added for WheaEinj driver ;;;;;
;------------------------------------------------------------------------------
; VOID *
; EFIAPI 
; AsmCommitLine (
;   IN      VOID                      *LinearAddress, 
;   IN      UINT32                    Value
;   );
;------------------------------------------------------------------------------
AsmCommitLine   PROC    
    mfence
    mov dword ptr[rcx],edx
    clflush [rcx]
    mfence
    mov     rax, rcx
    ret
AsmCommitLine   ENDP

;------------------------------------------------------------------------------
; VOID
; IA32API
; DisablePsmi (
;   VOID
;   );
;------------------------------------------------------------------------------
DisablePsmi   PROC ;NEAR PUBLIC
; Disable PSMI
  xor edx, edx
  mov ecx, 51h
  mov eax, 01
  wrmsr  
  
  ret

DisablePsmi ENDP
;------------------------------------------------------------------------------
; VOID
; IA32API
; DisablePrefetch (
;   VOID
;   );
;------------------------------------------------------------------------------
DisablePrefetch   PROC ;NEAR PUBLIC
; Disable Prefetch
  mov ecx, 1a4h
  rdmsr
  mov mBSPPrefetch, eax
  or  eax, 0fh
  wrmsr
  
  ret
DisablePrefetch ENDP

;------------------------------------------------------------------------------
; VOID
; IA32API
; EnablePrefetch (
;   VOID
;   );
;------------------------------------------------------------------------------
EnablePrefetch   PROC ;NEAR PUBLIC
;Enable Prefetch
  mov ecx, 1a4h
  rdmsr
  mov eax, mBSPPrefetch
  wrmsr
  
  ret
EnablePrefetch ENDP

      END
