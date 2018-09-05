;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c)  2004 - 2016 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.`
;
; Module Name:
;
;  RasSupport.nasm
;
; Abstract:
;
;  RAS related ASM functions
;
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; ModifyCacheLines (
;   UINTN  StartAddress,
;   UINTN  Length,
;   UINTN  CacheLineSize
;   );
;------------------------------------------------------------------------------
global ASM_PFX(ModifyCacheLines)
ASM_PFX(ModifyCacheLines):
    cmp     rdx,    0
    jz      Done
NextCacheLine:
    mov     rax,    [rcx]
    lock    cmpxchg [rcx], rax
    add     rcx,    r8
    sub     rdx,    r8
    jnz     NextCacheLine
Done:
    ret

; VOID
; EFIAPI
; ModifyCacheLinesWithFlush (
;   UINTN  StartAddress,
;   UINTN  Length,
;   UINTN  CacheLineSize
;   );
;------------------------------------------------------------------------------
global ASM_PFX(ModifyCacheLinesWithFlush)
ASM_PFX(ModifyCacheLinesWithFlush):
    cmp     rdx,    0
    jz      Done1
NextCacheLine1:
    mov     rax,    [rcx]
    lock    cmpxchg [rcx], rax
    clflush [rcx]
    add     rcx,    r8
    sub     rdx,    r8
    jnz     NextCacheLine1
Done1:
    ret

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; CpuPause (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(CpuCliHlt)
ASM_PFX(CpuCliHlt):

    wbinvd
    cli
    hlt
    jmp $-2

    ret

