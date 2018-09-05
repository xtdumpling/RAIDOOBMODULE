//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
#------------------------------------------------------------------------------
#
# Copyright (c) 2009 - 2014, Intel Corporation. All rights reserved.<BR>
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
# Module Name:
#
#   SmiException.S
#
# Abstract:
#
#   Exception handlers used in SM mode
#
#------------------------------------------------------------------------------
ASM_GLOBAL  ASM_PFX(SmiPFHandler)
ASM_GLOBAL  ASM_PFX(gSmiMtrrs)
ASM_GLOBAL  ASM_PFX(gcSmiIdtr)
ASM_GLOBAL  ASM_PFX(gcSmiGdtr)
ASM_GLOBAL  ASM_PFX(gcPsd)
ASM_GLOBAL  ASM_PFX(gSmiExceptionCr3)
ASM_GLOBAL  ASM_PFX(mMceHandlerLongMode)

    .data

NullSeg:    .quad 0                     # reserved by architecture
CodeSeg32:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x9b
            .byte 0xcf                  # LimitHigh
            .byte 0                     # BaseHigh
ProtModeCodeSeg32:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x9b
            .byte 0xcf                  # LimitHigh
            .byte 0                     # BaseHigh
ProtModeSsSeg32:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x93
            .byte 0xcf                  # LimitHigh
            .byte 0                     # BaseHigh
DataSeg32:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x93
            .byte 0xcf                  # LimitHigh
            .byte 0                     # BaseHigh
CodeSeg16:
            .word -1
            .word 0
            .byte 0
            .byte 0x9b
            .byte 0x8f
            .byte 0
DataSeg16:
            .word -1
            .word 0
            .byte 0
            .byte 0x93
            .byte 0x8f
            .byte 0
CodeSeg64:
            .word -1                    # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x9b
            .byte 0xaf                  # LimitHigh
            .byte 0                     # BaseHigh
# TSS Segment for X64 specially
TssSeg:
            .word TSS_DESC_SIZE         # LimitLow
            .word 0                     # BaseLow
            .byte 0                     # BaseMid
            .byte 0x89
            .byte 0xDB                  # LimitHigh
            .byte 0                     # BaseHigh
            .long 0                     # BaseUpper
            .long 0                     # Reserved
.equ  GDT_SIZE, .- NullSeg

TssDescriptor:
            .space 104, 0
.equ  TSS_DESC_SIZE, .- TssDescriptor

#
# This structure serves as a template for all processors.
#
ASM_PFX(gcPsd):
            .ascii  "PSDSIG  "
            .word      PSD_SIZE
            .word 2
            .word      1 << 2
            .word      CODE_SEL
            .word      DATA_SEL
            .word      DATA_SEL
            .word      DATA_SEL
            .word 0
            .quad 0
            .quad 0
            .quad 0                     # fixed in InitializeMpServiceData()
            .quad      NullSeg
            .long      GDT_SIZE
            .long 0
            .space 24, 0
            .quad      ASM_PFX(gSmiMtrrs)
.equ  PSD_SIZE,  . - ASM_PFX(gcPsd)

#
# CODE & DATA segments for SMM runtime
#
.equ  CODE_SEL,    CodeSeg64 - NullSeg
.equ  DATA_SEL,    DataSeg32 - NullSeg
.equ  CODE32_SEL,  CodeSeg32 - NullSeg

ASM_PFX(gcSmiGdtr):
    .word      GDT_SIZE - 1
    .quad      NullSeg

ASM_PFX(gcSmiIdtr):
    .word      IDT_SIZE - 1
    .quad      _SmiIDT


#
# Here is the IDT. There are 32 (not 255) entries in it since only processor
# generated exceptions will be handled.
#
_SmiIDT:
# The following segment repeats 32 times:
# No. 1
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 2
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 3
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 4
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 5
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 6
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 7
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 8
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 9
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 10
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 11
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 12
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 13
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 14
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 15
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 16
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 17
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 18
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 19
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 20
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 21
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 22
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 23
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 24
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 25
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 26
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 27
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 28
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 29
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 30
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 31
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63
# No. 32
    .word 0                             # Offset 0:15
    .word CODE_SEL
    .byte 0                             # Unused
    .byte 0x8e                          # Interrupt Gate, Present
    .word 0                             # Offset 16:31
    .quad 0                             # Offset 32:63

_SmiIDTEnd:

.equ  IDT_SIZE, (_SmiIDTEnd - _SmiIDT)

    .text

ASM_GLOBAL ASM_PFX(PMIdtHandler)
ASM_PFX(PMIdtHandler):
    jmp    .
    retf

ASM_GLOBAL ASM_PFX(PMIdtHandlerMachineCheck)
ASM_PFX(PMIdtHandlerMachineCheck):
    subl    $8, %esp
    sidt    (%rsp)                     # save 32bit IDTR
    pushq   %rax
    pushq   %rdx
    pushq   %rcx

    .byte   0xb8                       # mov eax, offset gSmiExceptionCr3
ASM_PFX(gSmiExceptionCr3):   .space  4
    movq    %rax, %cr3

    movl    $0x668, %eax               # as cr4.PGE is not set here, refresh cr3
    movq    %rax, %cr4                 # in PreModifyMtrrs() to flush TLB.

# save old ss:esp
    movl    %ss, %eax
    pushq   %rax
    pushq   %rsp
    cmpw    $DATA_SEL, %ax             # check if ss:esp have been updated
    jz      Skip                       # if not, update ss:esp

# change ss:esp
    sgdt    -8(%rsp)
    movl    -6(%rsp), %ecx
    addl     %eax, %ecx

    xorl    %eax, %eax
    movw    2(%rcx), %ax
    xorl    %edx, %edx
    movb    4(%rcx), %dl
    movb    7(%rcx), %dh
    shl     $0x10, %edx
    orl     %edx, %eax
    addl    %eax, %esp
    movl    $DATA_SEL, %eax
    movw    %ax, %ss

Skip:
    movq    %cr4, %rax
    pushq   %rax                        # save cr4
    movl    $0xC0000080, %ecx
    rdmsr
    pushq   %rax                        # save MSR(0xc0000080)
    movq    %cr0, %rax
    pushq   %rax                        # save cr0
 
    movl    $CODE_SEL, %eax
    pushq   %rax                        # push cs hardcore
    call    Base                        # push reture address for retf later
Base:
    addl    $(LongMode - Base), (%rsp)  # offset for far retf, seg is the 1st arg
    movq    %cr4, %rax
    orb     $(1 << 5), %al              #??????
    movq    %rax, %cr4                  # enable PAE
    movl    $0xc0000080, %ecx
    rdmsr
    orb     $1,%ah                      # set LME
    wrmsr
    movq    %cr0, %rbx
    btsl    $31, %ebx
    movq    %rbx, %cr0                  # enable paging
    retf
LongMode:                               # long mode starts here
    movq    ASM_PFX(gcSmiIdtr)(%rip), %rax    # load long mode IDT table to handle the exception
    lidt    (%rax)                            # that maybe issued in 64bit MC exception handler
    movq    %rsp, %rcx
    andl    $0x0fffffff0, %esp
    xorq    %rax, %rax
    movw    %ss, %ax
    pushq   %rax
    pushq   %rcx
    addq    $32, %rcx
    movl    8(%rcx), %eax
    pushq   %rax                        # old eflags
    xorq    %rax, %rax
    movw    %cs, %ax                    # cs
    pushq   %rax
    movabsq $BackPmIdtHandler, %rax      # return EIP
    pushq   %rax
    movabsq $ASM_PFX(mMceHandlerLongMode), %rax
    pushq   (%rax)                      # long mode MCA handle
    movl     -8(%rcx), %eax
    movl    -12(%rcx), %edx
    movl    -16(%rcx), %ecx
    
    addq    $8, %rsp
    jmp     *-8(%rsp)                   # jmp to long mode MCA handle

BackPmIdtHandler:
    #
    # let rax save DS
    #
    movq    $DATA_SEL, %rax

    #
    # Change to Compatible Segment
    #
    movq    $CODE32_SEL, %rcx           # load compatible mode selector
    shlq    $32, %rcx
    movabsq $Compatible, %rdx            # assume address < 4G
    orq     %rdx, %rcx
    pushq   %rcx
    retf

Compatible:
    addl    $16, %esp
    lss     -4(%rsp), %esp              # restore ss:esp
    subl    $16, %esp

    popq    %rax
    movq    %rax, %cr0                  # restore cr0 to disable paging.

    movl    $0xC0000080, %ecx
    rdmsr
    popq    %rax
    wrmsr                               # restore MSR (0xc0000080) to clear EFER.LME

    popq    %rax
    movq    %rax, %cr4                  # restore cr4 to clear PAE
    
    addl    $8, %esp                    # skip old ss and esp
    popq    %rcx
    popq    %rdx
    popq    %rax
    lidt    (%rsp)                      # load saved 32bit IDTR
    addl    $(12 + 8), %esp
    #
    # retf maybe used for MCE
    #
    pushq   -8(%rsp)
    pushq   -8(%rsp)
    retf

#------------------------------------------------------------------------------
# _SmiExceptionEntryPoints is the collection of exception entrypoints followed
# by a common exception handler.
#
# Stack frame would be as follows as specified in IA32 manuals:
# +---------------------+ <-- 16-byte aligned ensured by processor
# +    Old SS           +
# +---------------------+
# +    Old RSP          +
# +---------------------+
# +    RFlags           +
# +---------------------+
# +    CS               +
# +---------------------+
# +    RIP              +
# +---------------------+
# +    Error Code       +
# +---------------------+
# +   Vector Number     +
# +---------------------+
# +    RBP              +
# +---------------------+ <-- RBP, 16-byte aligned
#
# RSP set to odd multiple of 8 at @CommonEntryPoint means ErrCode PRESENT
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(PageFaultIdtHandlerSmmProfile)
ASM_PFX(PageFaultIdtHandlerSmmProfile):
    pushq   $0x0e                    # Page Fault
    .byte   0x40, 0xf6, 0xc4, 0x08    #test    spl, 8
    jnz     L1
    pushq   (%rsp)
    movq    $0, 8(%rsp)
L1:
    pushq   %rbp
    movq    %rsp, %rbp

    #
    # Since here the stack pointer is 16-byte aligned, so
    # EFI_FX_SAVE_STATE_X64 of EFI_SYSTEM_CONTEXT_x64
    # is 16-byte aligned
    #       

## UINT64  Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
## UINT64  R8, R9, R10, R11, R12, R13, R14, R15;
    pushq   %r15
    pushq   %r14
    pushq   %r13
    pushq   %r12
    pushq   %r11
    pushq   %r10
    pushq   %r9
    pushq   %r8
    pushq   %rax
    pushq   %rcx
    pushq   %rdx
    pushq   %rbx
    pushq   48(%rbp)                   # RSP
    pushq   (%rbp)                     # RBP
    pushq   %rsi
    pushq   %rdi

## UINT64  Gs, Fs, Es, Ds, Cs, Ss;  insure high 16 bits of each is zero
    movzwq 56(%rbp), %rax
    pushq   %rax                       # for ss
    movzwq  32(%rbp), %rax
    pushq   %rax                       # for cs
    movq    %ds, %rax
    pushq   %rax
    movq    %es, %rax
    pushq   %rax
    movq    %fs, %rax
    pushq   %rax
    movq    %gs, %rax
    pushq   %rax

## UINT64  Rip;
    pushq   24(%rbp)

## UINT64  Gdtr[2], Idtr[2];
    subq    $16, %rsp
    sidt    (%rsp)
    subq    $16, %rsp
    sgdt    (%rsp)

## UINT64  Ldtr, Tr;
    xorq    %rax, %rax
    strw    %ax
    pushq   %rax
    sldtw   %ax
    pushq   %rax

## UINT64  RFlags;
    pushq   40(%rbp)

## UINT64  Cr0, Cr1, Cr2, Cr3, Cr4, Cr8;
    movq    %cr8, %rax
    pushq   %rax
    movq    %cr4, %rax
    orq     $0x208, %rax
    movq    %rax, %cr4
    pushq   %rax
    movq    %cr3, %rax
    pushq   %rax
    movq    %cr2, %rax
    pushq   %rax
    xorq    %rax, %rax
    pushq   %rax
    movq    %cr0, %rax
    pushq   %rax

## UINT64  Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
    movq    %dr7, %rax
    pushq   %rax
    movq    %dr6, %rax
    pushq   %rax
    movq    %dr3, %rax
    pushq   %rax
    movq    %dr2, %rax
    pushq   %rax
    movq    %dr1, %rax
    pushq   %rax
    movq    %dr0, %rax
    pushq   %rax

## FX_SAVE_STATE_X64 FxSaveState;

    subq    $512, %rsp
    movq    %rsp, %rdi
    .byte   0xf, 0xae, 0x7                  # fxsave [rdi]

# UEFI calling convention for x64 requires that Direction flag in EFLAGs is clear
    cld

## UINT32  ExceptionData;
    pushq   16(%rbp)

## call into exception handler
    movq    8(%rbp), %rcx
    movabsq $ASM_PFX(SmiPFHandler), %rax

## Prepare parameter and call
    movq    %rsp, %rdx
    #
    # Per X64 calling convention, allocate maximum parameter stack space
    # and make sure RSP is 16-byte aligned
    #
    subq    $4 * 8 + 8, %rsp
    call    *%rax
    addq    $4 * 8 + 8, %rsp
    jmp     L5

L5:
## UINT64  ExceptionData;
    addq    $8, %rsp

## FX_SAVE_STATE_X64 FxSaveState;

    movq    %rsp, %rsi
    .byte   0xf, 0xae, 0xe                 # fxrstor [rsi]
    addq    $512, %rsp

## UINT64  Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
## Skip restoration of DRx registers to support in-circuit emualators
## or debuggers set breakpoint in interrupt/exception context
    addq    $8 * 6, %rsp 

## UINT64  Cr0, Cr1, Cr2, Cr3, Cr4, Cr8;
    popq    %rax
    movq    %rax, %cr0
    addq    $8, %rsp                      # not for Cr1
    popq    %rax
    movq    %rax, %cr2
    popq    %rax
    movq    %rax, %cr3
    popq    %rax
    movq    %rax, %cr4
    popq    %rax
    movq    %rax, %cr8

## UINT64  RFlags;
    popq    40(%rbp)

## UINT64  Ldtr, Tr;
## UINT64  Gdtr[2], Idtr[2];
## Best not let anyone mess with these particular registers...
    addq    $48, %rsp

## UINT64  Rip;
    popq    24(%rbp)

## UINT64  Gs, Fs, Es, Ds, Cs, Ss;
    popq    %rax
    # mov   gs, rax ; not for gs
    popq    %rax
    # mov   fs, rax ; not for fs
    # (X64 will not use fs and gs, so we do not restore it)
    popq    %rax
    movq    %rax, %es
    popq    %rax
    movq    %rax, %ds
    popq    32(%rbp)                      # for cs
    popq    56(%rbp)                      # for ss

## UINT64  Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
## UINT64  R8, R9, R10, R11, R12, R13, R14, R15;
    popq    %rdi
    popq    %rsi
    addq    $8, %rsp                      # not for rbp
    popq    48(%rbp)                      # for rsp
    popq    %rbx
    popq    %rdx
    popq    %rcx
    popq    %rax
    popq    %r8
    popq    %r9
    popq    %r10
    popq    %r11
    popq    %r12
    popq    %r13
    popq    %r14
    popq    %r15

    movq    %rbp, %rsp

# Enable TF bit after page fault handler runs
    btsl    $8, 40(%rsp)                 #RFLAGS

    popq    %rbp
    addq    $16, %rsp                    # skip INT# & ErrCode
    iretq

ASM_GLOBAL ASM_PFX(InitializeIDTSmmStackGuard)
ASM_PFX(InitializeIDTSmmStackGuard):
# If SMM Stack Guard feature is enabled, set the IST field of
# the interrupe gate for Page Fault Exception to be 1
#
    movabsq  $_SmiIDT + 14 * 16, %rax
    movb     $1, 4(%rax)
    ret
