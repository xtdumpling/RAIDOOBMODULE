;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c)  2004 - 2012 Intel Corporation. All rights reserved
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
;  RasSupport.asm
;
; Abstract:
;
;  RAS related ASM functions
;
;------------------------------------------------------------------------------

text  SEGMENT

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; ModifyCacheLines (
;   UINTN  StartAddress,
;   UINTN  Length,
;   UINTN  CacheLineSize
;   );
;------------------------------------------------------------------------------
ModifyCacheLines  PROC PUBLIC
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
ModifyCacheLines  ENDP

; VOID
; EFIAPI
; ModifyCacheLinesWithFlush (
;   UINTN  StartAddress,
;   UINTN  Length,
;   UINTN  CacheLineSize
;   );
;------------------------------------------------------------------------------
ModifyCacheLinesWithFlush  PROC PUBLIC
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
ModifyCacheLinesWithFlush  ENDP

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; CpuPause (
;   VOID
;   );
;------------------------------------------------------------------------------
CpuCliHlt    PROC    PUBLIC

    wbinvd
    cli
    hlt
    jmp $-2

    ret
CpuCliHlt    ENDP

    END
