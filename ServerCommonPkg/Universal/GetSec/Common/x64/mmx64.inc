;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2007 Intel Corporation. All rights reserved
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
;   stackless32.asm
;
; Abstract:
;
;------------------------------------------------------------------------------

MOVQIN0                         EQU db 48h, 0fh, 06eh, 0c0h ; movq mm0, rax
MOVQOUT0                        EQU db 48h, 0fh, 07eh, 0c0h ; movq rax, mm0
MOVDOUT0                        EQU db 0fh, 07eh, 0c0h ; movd eax, mm0

MOVQIN1                         EQU db 48h, 0fh, 06eh, 0c8h ; movq mm1, rax
MOVQOUT1                        EQU db 48h, 0fh, 07eh, 0c8h ; movq rax, mm1
MOVDOUT1                        EQU db 0fh, 07eh, 0c8h ; movd eax, mm1

MOVQIN2                         EQU db 48h, 0fh, 06eh, 0d0h ; movq mm2, rax
MOVQOUT2                        EQU db 48h, 0fh, 07eh, 0d0h ; movq rax, mm2
MOVDOUT2                        EQU db 0fh, 07eh, 0d0h ; movd eax, mm2

MOVQIN3                         EQU db 48h, 0fh, 06eh, 0d8h ; movq mm3, rax
MOVQOUT3                        EQU db 48h, 0fh, 07eh, 0d8h ; movq rax, mm3
MOVDOUT3                        EQU db 0fh, 07eh, 0d8h ; movd eax, mm3

MOVQIN4                         EQU db 48h, 0fh, 06eh, 0e0h ; movq mm4, rax
MOVQOUT4                        EQU db 48h, 0fh, 07eh, 0e0h ; movq rax, mm4
MOVDOUT4                        EQU db 0fh, 07eh, 0e0h ; movd eax, mm4

MOVQIN5                         EQU db 48h, 0fh, 06eh, 0e8h ; movq mm5, rax
MOVQOUT5                        EQU db 48h, 0fh, 07eh, 0e8h ; movq rax, mm5
MOVDOUT5                        EQU db 0fh, 07eh, 0e8h ; movd eax, mm5

MOVQIN6                         EQU db 48h, 0fh, 06eh, 0f0h ; movq mm6, rax
MOVQOUT6                        EQU db 48h, 0fh, 07eh, 0f0h ; movq rax, mm6
MOVDOUT6                        EQU db 0fh, 07eh, 0f0h ; movd eax, mm6

MOVQIN7                         EQU db 48h, 0fh, 06eh, 0f8h ; movq mm7, rax
MOVQOUT7                        EQU db 48h, 0fh, 07eh, 0f8h ; movq rax, mm7
MOVDOUT7                        EQU db 0fh, 07eh, 0f8h ; movd eax, mm7

_EMMS                           EQU db 0fh, 77h

;-----------------------------------------------------------------------------
;  Macro: PUSHA_64
; 
;  Description: Saves all registers on stack
;  
;  Input:   None
; 
;  Output:  None
;-----------------------------------------------------------------------------
PUSHA_64   macro
    push    rsp
    push    rbp
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
endm

;-----------------------------------------------------------------------------
;  Macro: POPA_64
; 
;  Description: Restores all registers from stack
;  
;  Input:   None
; 
;  Output:  None
;-----------------------------------------------------------------------------
POPA_64   macro
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    pop    rbp
    pop    rsp
endm

;-----------------------------------------------------------------------------
;  Macro: START_FRAME
; 
;  Description: Starts frame declaration. Creates variable to hold total
;               size of all local vars.
;  
;  Input:   None
; 
;  Output:  None
; 
;-----------------------------------------------------------------------------
START_FRAME MACRO
    SZ = 0
ENDM

;-----------------------------------------------------------------------------
;  Macro: MAKE_LOCAL
; 
;  Description: Defines local procedure variable. Adds size of created variable
;               to total size of locals.
;               size of all local vars.
;  
;  Input:   ARG - var definition in form Label[Count]:QuolifiedType
; 
;  Output:  None
; 
;-----------------------------------------------------------------------------
MAKE_LOCAL MACRO ARG:REQ
    local brk, rbrk, clm, lbl, cnt, qtp
    brk  INSTR <ARG>, <[>
    rbrk INSTR <ARG>, <]>
    clm INSTR <ARG>, <:>   
    IF brk GT 0                 
      lbl SUBSTR <ARG>, 1, brk-1
      cnt SUBSTR <ARG>, brk+1, rbrk-brk-1
      qtp SUBSTR <ARG>, clm+1
      LOCAL lbl[cnt]:qtp
    ELSE
      lbl SUBSTR <ARG>, 1, clm-1
      qtp SUBSTR <ARG>, clm+1
      LOCAL lbl:qtp
    ENDIF                 

    SZ = SZ + sizeof lbl
ENDM

;-----------------------------------------------------------------------------
;  Macro: END_FRAME
; 
;  Description: Ends frame declaration. Creates stack sufficient to hold
;               all declared variables. 
;  
;  Input:   None
; 
;  Output:  None
; 
;-----------------------------------------------------------------------------
END_FRAME MACRO
    SZ = SZ + 10h                   ; Margin                  
    enter SZ, 0                     ; With spare room on stack
    .allocstack SZ
    .endprolog                        
ENDM


