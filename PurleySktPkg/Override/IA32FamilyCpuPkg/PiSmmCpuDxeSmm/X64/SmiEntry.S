//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
#------------------------------------------------------------------------------
#
# Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
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
#   SmiEntry.S
#
# Abstract:
#
#   Code template of the SMI handler for a particular processor
#
#------------------------------------------------------------------------------

ASM_GLOBAL  ASM_PFX(gcSmiHandlerTemplate)
ASM_GLOBAL  ASM_PFX(gcSmiHandlerSize)
ASM_GLOBAL  ASM_PFX(gSmiCr3)
ASM_GLOBAL  ASM_PFX(gSmiStack)
ASM_GLOBAL  ASM_PFX(gSmbase)
ASM_GLOBAL  ASM_PFX(mSaveStateInMsr)
ASM_GLOBAL  ASM_PFX(FeaturePcdGet (PcdCpuSmmDebug))
ASM_GLOBAL  ASM_PFX(gcSmiPMHandlerTemplate)
ASM_GLOBAL  ASM_PFX(gcSmiPMHandlerSize)
ASM_GLOBAL  ASM_PFX(gProtModeSmbase)
ASM_GLOBAL  ASM_PFX(gcSmiIdtr)
ASM_GLOBAL  ASM_PFX(mSmmProtectedModeEnable)
ASM_GLOBAL  ASM_PFX(gProtModeIdtr)

#
# Constants relating to PROCESSOR_SMM_DESCRIPTOR
#
.equ            DSC_OFFSET, 0xfb00
.equ            DSC_GDTPTR, 0x30
.equ            DSC_GDTSIZ, 0x38
.equ            DSC_CS, 14
.equ            DSC_DS, 16
.equ            DSC_SS, 18
.equ            DSC_OTHERSEG, 20
.equ            MSR_DR6,    0x0c05
.equ            MSR_DR7,    0x0c06
#
# Constants relating to CPU State Save Area
#
.equ            SSM_DR6,   0xffd0
.equ            SSM_DR7,   0xffc8

.equ            PROTECT_MODE_CS, 0x08
.equ            PROTECT_MODE_DS, 0x20
.equ            LONG_MODE_CS, 0x38
.equ            TSS_SEGMENT, 0x40
.equ            GDT_SIZE, 0x50

    .text

ASM_PFX(gcSmiHandlerTemplate):

_SmiEntryPoint:
    #
    # The encoding of BX in 16-bit addressing mode is the same as of RDI in 64-
    # bit addressing mode. And that coincidence has been used in the following
    # "64-bit like" 16-bit code. Be aware that once RDI is referrenced as a
    # base address register, it is actually BX that is referrenced.
    #
    .byte 0xbb                          # mov bx, imm16
    .word _GdtDesc - _SmiEntryPoint + 0x8000
    #
    # fix GDT descriptor
    #
    .byte 0x2e,0xa1                     # mov ax, cs:[offset16]
    .word      DSC_OFFSET + DSC_GDTSIZ
    .byte 0x48                          # dec ax
    .byte 0x2e
    movl    %eax, (%rdi)                # mov cs:[bx], ax
    .byte 0x66,0x2e,0xa1                # mov eax, cs:[offset16]
    .word      DSC_OFFSET + DSC_GDTPTR
    .byte 0x2e
    movw    %ax, 2(%rdi)
    .byte 0x66,0x2e
    lgdt    (%rdi)
    #
    # Patch ProtectedMode Segment
    #
    .byte 0xb8
    .word PROTECT_MODE_CS
    .byte 0x2e
    movl    %eax, -2(%rdi)
    #
    # Patch ProtectedMode entry
    #
    .byte 0x66, 0xbf                    # mov edi, SMBASE
ASM_PFX(gSmbase): .space 4
    lea     ((ProtectedMode - _SmiEntryPoint) + 0x8000)(%edi), %ax
    .byte 0x2e
    movw    %ax, -6(%rdi)
    #
    # Switch into ProtectedMode
    #
    movq    %cr0, %rbx
    .byte 0x66
    andl    $0x9ffafff3, %ebx
    .byte 0x66
    orl     $0x00000023, %ebx

    movq    %rbx, %cr0
    .byte 0x66, 0xea
    .space 6

_GdtDesc:    .space  6

ProtectedMode:
    movw    $PROTECT_MODE_DS, %ax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss
    .byte   0xbc                       # mov esp, imm32
ASM_PFX(gSmiStack):   .space  4
    jmp     ProtFlatMode

ASM_PFX(gcSmiPMHandlerTemplate):
_SmiPMEntryPoint:
# when SMM PROT MODE feature is ok, processor will break here with 32bit protected mode
    .byte    0x0bf                     # mov edi, SMBASE
ASM_PFX(gProtModeSmbase):    .space    4
    movl    $PROTECT_MODE_DS, %eax
    pushq   %rax
    movl    %esp, %eax
    addl    %edi, %eax
    addl    $4, %eax
    pushq   %rax
    subl    $8, %eax
    lss     (%rax), %esp

    movl    %edi, %eax
    addl    $(ProtFlatMode - _SmiPMEntryPoint + 0x8000), %eax
    movw    $PROTECT_MODE_CS, %dx
    movw    %dx, 2(%rax)                # mov cs:[bx - 6], eax
    movl    %eax, 6(%rax)               # mov cs:[bx - 6], eax

## PURLEY_OVERRIDE_BEGIN_5333001
    movq    %cr0, %rbx
    .byte 0x66
    orl     $0x00000020, %ebx           # set CR0.NE
    movq    %rbx, %cr0
## PURLEY_OVERRIDE_END_5333001

    .byte   0xea                        # jmp @ProtFlatMode
    .space  6
    
ProtFlatMode:
    .byte   0xb8
ASM_PFX(gSmiCr3):    .space  4
    movq    %rax, %cr3
    movl    $0x668,%eax                 # as cr4.PGE is not set here, refresh cr3
    movq    %rax, %cr4                  # in PreModifyMtrrs() to flush TLB.
# Load TSS
    subl    $8, %esp                    # reserve room in stack
    sgdt    (%rsp)
    movl    2(%rsp), %eax               # eax = GDT base
    addl    $8, %esp
    movl    %eax, %edx
    addl    $GDT_SIZE, %edx
    movb    %dl, (TSS_SEGMENT + 2)(%rax)
    movb    %dh, (TSS_SEGMENT + 3)(%rax)
    .byte   0xc1, 0xea, 0x10             # shr     edx, 16
    movb    %dl, (TSS_SEGMENT + 4)(%rax)
    movb    %dh, (TSS_SEGMENT + 7)(%rax)
    movl    %eax, %edx
    movb    $0x89, %dl
    movb    %dl, (TSS_SEGMENT + 5)(%rax) # clear busy flag
    movl    $TSS_SEGMENT, %eax
    ltr     %ax

    #
    # Switch to LongMode
    #
    pushq    $LONG_MODE_CS                # push cs hardcore here
    call     Base                         # push reture address for retf later
Base:
    addl    $(LongMode - Base), (%rsp)  # offset for far retf, seg is the 1st arg
    movl    $0xc0000080, %ecx
    rdmsr
    orb     $1,%ah
    wrmsr
    movq    %cr0, %rbx
    btsl    $31, %ebx
    movq    %rbx, %cr0
    retf
LongMode:                               # long mode (64-bit code) starts here
    movabsq $ASM_PFX(gcSmiIdtr), %rax
    lidt    (%rax)
    lea     (DSC_OFFSET)(%rdi), %ebx
    movw    DSC_DS(%rbx), %ax
    movl    %eax,%ds
    movw    DSC_OTHERSEG(%rbx), %ax
    movl    %eax,%es
    movl    %eax,%fs
    movl    %eax,%gs
    movw    DSC_SS(%rbx), %ax
    movl    %eax,%ss
#   jmp     _SmiHandler                 ; instruction is not needed

_SmiHandler:
    movabsq $ASM_PFX(FeaturePcdGet (PcdCpuSmmDebug)), %rax
    cmpb    $0, (%rax)
    jz      L1

    movabsq $ASM_PFX(mSaveStateInMsr), %rax
    cmpb    $0, (%rax)
    jz      L3
    movl    $MSR_DR6, %ecx
    rdmsr
    shlq    $32, %rax
    shldq   $32, %rax, %rdx
    pushq   %rdx
    movl    $MSR_DR7, %ecx
    rdmsr
    shlq    $32, %rax
    shldq   $32, %rax, %rdx
    popq    %rcx
    jmp     L4

L3:
    movabsq $ASM_PFX(mSmmProtectedModeEnable), %rax
    cmpb    $1, (%rax)
    jz      L4
    .byte   0x48, 0x8b, 0x0d            # mov rcx, [rip + disp32]
    .long   SSM_DR6 - (. + 4 - _SmiEntryPoint + 0x8000)
    .byte   0x48, 0x8b, 0x15            # mov rdx, [rip + disp32]
    .long   SSM_DR7 - (. + 4 - _SmiEntryPoint + 0x8000)
    jmp     L5
L4:
    .byte   0x48, 0x8b, 0x0d            # mov rcx, [rip + disp32]
    .long   SSM_DR6 - (. + 4 - _SmiPMEntryPoint + 0x8000)
    .byte   0x48, 0x8b, 0x15            # mov rdx, [rip + disp32]
    .long   SSM_DR7 - (. + 4 - _SmiPMEntryPoint + 0x8000)
L5:
    movq    %rcx, %dr6
    movq    %rdx, %dr7
L1:

    movabsq $ASM_PFX(SmiRendezvous), %rax
    movq    (%rsp), %rcx
    # Save FP registers

    subq    $0x208, %rsp
    .byte   0x48                        # FXSAVE64
    fxsave  (%rsp)

    addq    $-0x20, %rsp
    call    *%rax
    addq    $0x20, %rsp

    #
    # Restore FP registers
    #
    .byte   0x48                        # FXRSTOR64
    fxrstor (%rsp)

    movabsq $ASM_PFX(FeaturePcdGet (PcdCpuSmmDebug)), %rax
    cmpb    $0, (%rax)
    jz      L2

    movabsq $ASM_PFX(mSaveStateInMsr), %rax
    cmpb    $0, (%rax)
    jnz     L2                          # DR6/DR7 MSR is RO

    movq    %dr7, %rdx
    movq    %dr6, %rcx
    movabsq $ASM_PFX(mSmmProtectedModeEnable), %rax
    cmpb    $1, (%rax)
    jz      L2
    .byte   0x48, 0x89, 0x15            # mov [rip + disp32], rdx
    .long   SSM_DR7 - (. + 4 - _SmiEntryPoint + 0x8000)
    .byte   0x48, 0x89, 0x0d            # mov [rip + disp32], rcx
    .long   SSM_DR6 - (. + 4 - _SmiEntryPoint + 0x8000)
    jmp     L6
L2:
    .byte   0x48, 0x89, 0x15            # mov [rip + disp32], rdx
    .long   SSM_DR7 - (. + 4 - _SmiPMEntryPoint + 0x8000)
    .byte   0x48, 0x89, 0x0d            # mov [rip + disp32], rcx
    .long   SSM_DR6 - (. + 4 - _SmiPMEntryPoint + 0x8000)
L6:
    rsm

ASM_PFX(gcSmiHandlerSize):    .word      . - _SmiEntryPoint
ASM_PFX(gcSmiPMHandlerSize):  .word      . - _SmiPMEntryPoint

