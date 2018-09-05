#
# This file contains an 'Intel Peripheral Driver' and is      
# licensed for Intel CPUs and chipsets under the terms of your
# license agreement with Intel or your vendor.  This file may 
# be modified by the user, subject to additional terms of the 
# license agreement                                           
#
#------------------------------------------------------------------------------
#
# Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
#
# Module Name:
#
#   stackless32.asm
#
# Abstract:
#
#------------------------------------------------------------------------------

.macro MOVQIN0
.byte 0x48, 0x0f, 0x06e, 0x0c0  # movq mm0, rax
.endm

.macro MOVQOUT0
.byte 0x48, 0x0f, 0x07e, 0x0c0  # movq rax, mm0
.endm

.macro MOVDOUT0
.byte 0x0f, 0x07e, 0x0c0  # movd eax, mm0
.endm

.macro MOVQIN1
.byte 0x48, 0x0f, 0x06e, 0x0c8  # movq mm1, rax
.endm

.macro MOVQOUT1
.byte 0x48, 0x0f, 0x07e, 0x0c8  # movq rax, mm1
.endm

.macro MOVDOUT1
.byte 0x0f, 0x07e, 0x0c8  # movd eax, mm1
.endm

.macro MOVQIN2
.byte 0x48, 0x0f, 0x06e, 0x0d0  # movq mm2, rax
.endm

.macro MOVQOUT2
.byte 0x48, 0x0f, 0x07e, 0x0d0  # movq rax, mm2
.endm

.macro MOVDOUT2
.byte 0x0f, 0x07e, 0x0d0  # movd eax, mm2
.endm

.macro MOVQIN3
.byte 0x48, 0x0f, 0x06e, 0x0d8  # movq mm3, rax
.endm

.macro MOVQOUT3
.byte 0x48, 0x0f, 0x07e, 0x0d8  # movq rax, mm3
.endm

.macro MOVDOUT3
.byte 0x0f, 0x07e, 0x0d8  # movd eax, mm3
.endm

.macro MOVQIN4
.byte 0x48, 0x0f, 0x06e, 0x0e0  # movq mm4, rax
.endm

.macro MOVQOUT4
.byte 0x48, 0x0f, 0x07e, 0x0e0  # movq rax, mm4
.endm

.macro MOVDOUT4
.byte 0x0f, 0x07e, 0x0e0  # movd eax, mm4
.endm

.macro MOVQIN5
.byte 0x48, 0x0f, 0x06e, 0x0e8  # movq mm5, rax
.endm

.macro MOVQOUT5
.byte 0x48, 0x0f, 0x07e, 0x0e8  # movq rax, mm5
.endm

.macro MOVDOUT5
.byte 0x0f, 0x07e, 0x0e8  # movd eax, mm5
.endm

.macro MOVQIN6
.byte 0x48, 0x0f, 0x06e, 0x0f0  # movq mm6, rax
.endm

.macro MOVQOUT6
.byte 0x48, 0x0f, 0x07e, 0x0f0  # movq rax, mm6
.endm

.macro MOVDOUT6
.byte 0x0f, 0x07e, 0x0f0  # movd eax, mm6
.endm

.macro MOVQIN7
.byte 0x48, 0x0f, 0x06e, 0x0f8  # movq mm7, rax
.endm

.macro MOVQOUT7
.byte 0x48, 0x0f, 0x07e, 0x0f8  # movq rax, mm7
.endm

.macro MOVDOUT7
.byte 0x0f, 0x07e, 0x0f8  # movd eax, mm7
.endm

.macro _EMMS
.byte 0x0f, 0x77
.endm

#-----------------------------------------------------------------------------
#  Macro: PUSHA_64
# 
#  Description: Saves all registers on stack
#  
#  Input:   None
# 
#  Output:  None
#-----------------------------------------------------------------------------
.macro PUSHA_64
    push    %rsp
    push    %rbp
    push    %rax
    push    %rbx
    push    %rcx
    push    %rdx
    push    %rsi
    push    %rdi
    push    %r8
    push    %r9
    push    %r10
    push    %r11
    push    %r12
    push    %r13
    push    %r14
    push    %r15
.endm

#-----------------------------------------------------------------------------
#  Macro: POPA_64
# 
#  Description: Restores all registers from stack
#  
#  Input:   None
# 
#  Output:  None
#-----------------------------------------------------------------------------
.macro POPA_64
    pop    %r15
    pop    %r14
    pop    %r13
    pop    %r12
    pop    %r11
    pop    %r10
    pop    %r9
    pop    %r8
    pop    %rdi
    pop    %rsi
    pop    %rdx
    pop    %rcx
    pop    %rbx
    pop    %rax
    pop    %rbp
    pop    %rsp
.endm
