;------------------------------------------------------------------------------
;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;
; Copyright (c)2009 - 2016 Intel Corporation. All rights reserved
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

  SECTION .text

%define CR0_CD_MASK 0x40000000    ;(1 << 30)

mBSPPrefetch: dd 00
mAPPrefetch: dd 00

;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmWbinvd (
;   VOID
;   );
;------------------------------------------------------------------------------
;GCC inline - not needed here
;global ASM_PFX(AsmWbinvd)
;ASM_PFX(AsmWbinvd): ;NEAR PUBLIC
;    wbinvd
;    ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmNop (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmNop)
ASM_PFX(AsmNop): ;NEAR PUBLIC

  nop
  nop

  ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOffCache (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(TurnOffCache)
ASM_PFX(TurnOffCache): ;NEAR PUBLIC

  ;Disable Cache
  Mov   Rax, CR0
  Or    Rax, CR0_CD_MASK  ;Set the CD Bit
  Mov   CR0, Rax

  ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOnCache (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(TurnOnCache)
ASM_PFX(TurnOnCache): ;NEAR PUBLIC

  Mov   Rax, CR0
  And   Rax, ~ CR0_CD_MASK    ;Clear the CD Bit
  Mov   CR0, Rax

  ret

;;;;;; Added for WheaEinj driver ;;;;;
;------------------------------------------------------------------------------
; VOID *
; EFIAPI
; AsmFlushCacheLine (
;   IN      VOID                      *LinearAddress
;   );
;------------------------------------------------------------------------------
; GCC inline - not needed here
;global ASM_PFX(AsmFlushCacheLine)
;ASM_PFX(AsmFlushCacheLine):
;    mfence
;    clflush [rcx]
;    mfence
;    mov     rax, rcx
;    ret

;;;;;; Added for WheaEinj driver ;;;;;
;------------------------------------------------------------------------------
; VOID *
; EFIAPI
; AsmCommitLine (
;   IN      VOID                      *LinearAddress,
;   IN      UINT32                    Value
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmCommitLine)
ASM_PFX(AsmCommitLine):
    mfence
    mov dword[rcx],edx
    clflush [rcx]
    mfence
    mov     rax, rcx
    ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; DisablePsmi (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(DisablePsmi)
ASM_PFX(DisablePsmi): ;NEAR PUBLIC
; Disable PSMI
  xor edx, edx
  mov ecx, 0x51
  mov eax, 01
  wrmsr

  ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; DisablePrefetch (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(DisablePrefetch)
ASM_PFX(DisablePrefetch): ;NEAR PUBLIC
; Disable Prefetch
  mov ecx, 0x1a4
  rdmsr
  mov [mBSPPrefetch], eax
  or  eax, 0xf
  wrmsr

  ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; EnablePrefetch (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(EnablePrefetch)
ASM_PFX(EnablePrefetch): ;NEAR PUBLIC
;Enable Prefetch
  mov ecx, 0x1a4
  rdmsr
  mov eax, mBSPPrefetch
  wrmsr

  ret

