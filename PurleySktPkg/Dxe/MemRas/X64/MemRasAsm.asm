;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c)  2004-2011 Intel Corporation. All rights reserved
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
;  MemRasAsm.asm
;
; Abstract:
;
;  Mem Ras Code schedule
;
text  SEGMENT

EXTERN  mCurrHostStack:  QWORD
EXTERN  mCurrGuestStack: QWORD
EXTERN  mGuestStackTop:  QWORD
EXTERN  mScheduleFunc:   QWORD

; VOID
; EFIAPI
; SchedulingCall (UINTN Param0, UINTN Param1, UINTN Param2, UINTN Param3)
;
SchedulingCall   PROC    PUBLIC
    mov    rax, mCurrGuestStack
    cmp    rax, 0
    jnz    SchedulingStart
    mov    mCurrHostStack, rsp
    mov    rsp, mGuestStackTop

    ; return address for the function call
    mov    rax, offset GuestFunctionCallRet
    push   rax

    mov    rax, mScheduleFunc
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

    mov    mCurrGuestStack, rsp
    mov    rsp, mCurrHostStack

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
    mov    rax, offset SchedulingCallEnd
    mov    [rsp+16*8], rax
    mov    mCurrHostStack, rsp
    mov    rsp, mCurrGuestStack
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
    mov    mCurrGuestStack, rcx    
    mov    rsp, mCurrHostStack    
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
    mov    rax,   04015h
    ret
SchedulingCall    ENDP


; VOID
; EFIAPI
; SchedulingOnce (VOID)
;
SchedulingOnce    PROC    PUBLIC
    pushfq
    pushfq
    push   rax
    mov    rax, mCurrGuestStack
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
    mov    rax, offset SchedulingOnceEnd
    mov    [rsp+16*8], rax
    mov    mCurrGuestStack, rsp
    mov    rsp, mCurrHostStack
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
SchedulingOnce    ENDP

;------------------------------------------------------------------------------
; VOID *
; EFIAPI 
; AsmFlushCacheLine (
;   IN      VOID                      *LinearAddress
;   );
;------------------------------------------------------------------------------
AsmFlushCacheLine   PROC
    clflush [rcx]
    mov     rax, rcx
    ret
AsmFlushCacheLine   ENDP


UINT8    TYPEDEF    BYTE
UINT16   TYPEDEF    WORD
UINT32   TYPEDEF    DWORD
UINT64   TYPEDEF    QWORD
UINTN    TYPEDEF    UINT32

    END
