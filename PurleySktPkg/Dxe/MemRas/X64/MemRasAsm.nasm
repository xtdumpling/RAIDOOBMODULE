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
;  MemRasAsm.nasm
;
; Abstract:
;
;  Mem Ras Code schedule
;

extern ASM_PFX(mCurrHostStack)
extern ASM_PFX(mCurrGuestStack)
extern ASM_PFX(mGuestStackTop)
extern ASM_PFX(mScheduleFunc)

; VOID
; EFIAPI
; SchedulingCall (UINTN Param0, UINTN Param1, UINTN Param2, UINTN Param3)
;
global ASM_PFX(SchedulingCall)
ASM_PFX(SchedulingCall):
    mov    rax, ASM_PFX(mCurrGuestStack)
    cmp    rax, 0
    jnz    SchedulingStart
    mov    [ASM_PFX(mCurrHostStack)], rsp
    mov    rsp, ASM_PFX(mGuestStackTop)

    ; return address for the function call
    mov    rax, GuestFunctionCallRet
    push   rax

    mov    rax, ASM_PFX(mScheduleFunc)
    push   rax

    pushfq
    push   r8
    push   r9
    push   r10
    push   r11
    push   r12
    push   r13
    push   r14
    push   r15
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    push   rbp

    mov    [ASM_PFX(mCurrGuestStack)], rsp
    mov    rsp, ASM_PFX(mCurrHostStack)

SchedulingStart:
    pushfq       ; start call the guest function
    pushfq
    push   r8
    push   r9
    push   r10
    push   r11
    push   r12
    push   r13
    push   r14
    push   r15
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    push   rbp
    mov    rax, SchedulingCallEnd
    mov    [rsp+16*8], rax
    mov    [ASM_PFX(mCurrHostStack)], rsp
    mov    rsp, ASM_PFX(mCurrGuestStack)
    pop    rbp
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    popfq
    ret

GuestFunctionCallRet:
    xor    rcx, rcx
    mov    [ASM_PFX(mCurrGuestStack)], rcx
    mov    rsp, ASM_PFX(mCurrHostStack)
    mov    [rsp+ 6*8], rax
    pop    rbp
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    popfq
    add    rsp, 8
    ret

SchedulingCallEnd:
    mov    rax,   0x4015
    ret

; VOID
; EFIAPI
; SchedulingOnce (VOID)
;
global ASM_PFX(SchedulingOnce)
ASM_PFX(SchedulingOnce):
    pushfq
    pushfq
    push   rax
    mov    rax, ASM_PFX(mCurrGuestStack)
    cmp    rax, 0
    pop    rax
    jnz    SchedulingGuest
    popfq
    popfq
    ret
SchedulingGuest:
    push   r8
    push   r9
    push   r10
    push   r11
    push   r12
    push   r13
    push   r14
    push   r15
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    push   rbp
    mov    rax, SchedulingOnceEnd
    mov    [rsp+16*8], rax
    mov    [ASM_PFX(mCurrGuestStack)], rsp
    mov    rsp, ASM_PFX(mCurrHostStack)
    pop    rbp
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    popfq
    ret
SchedulingOnceEnd:
    ret

;------------------------------------------------------------------------------
; VOID *
; EFIAPI
; AsmFlushCacheLine (
;   IN      VOID                      *LinearAddress
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmFlushCacheLine)
ASM_PFX(AsmFlushCacheLine):
    clflush [rcx]
    mov     rax, rcx
    ret

;UINT8    TYPEDEF    BYTE
;UINT16   TYPEDEF    WORD
;UINT32   TYPEDEF    DWORD
;UINT64   TYPEDEF    QWORD
;UINTN    TYPEDEF    UINT32

