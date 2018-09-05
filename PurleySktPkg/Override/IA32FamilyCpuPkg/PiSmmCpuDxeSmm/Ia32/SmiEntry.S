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
ASM_GLOBAL  ASM_PFX(FeaturePcdGet (PcdCpuSmmStackGuard))
ASM_GLOBAL  ASM_PFX(gcSmiPMHandlerTemplate)
ASM_GLOBAL  ASM_PFX(gcSmiPMHandlerSize)
ASM_GLOBAL  ASM_PFX(gProtModeSmbase)
ASM_GLOBAL  ASM_PFX(gcSmiIdtr)

.equ            DSC_OFFSET, 0xfb00
.equ            DSC_GDTPTR, 0x30
.equ            DSC_GDTSIZ, 0x38
.equ            DSC_CS, 14
.equ            DSC_DS, 16
.equ            DSC_SS, 18
.equ            DSC_OTHERSEG, 20

.equ            MSR_DR6,    0x0c05
.equ            MSR_DR7,    0x0c06

.equ            PROTECT_MODE_CS, 0x08
.equ            PROTECT_MODE_DS, 0x20
.equ            TSS_SEGMENT,     0x40

    .text

ASM_PFX(gcSmiHandlerTemplate):

_SmiEntryPoint:
    .byte 0xbb                          # mov bx, imm16
    .word _GdtDesc - _SmiEntryPoint + 0x8000
    .byte 0x2e,0xa1                     # mov ax, cs:[offset16]
    .word DSC_OFFSET + DSC_GDTSIZ
    decl    %eax
    movl    %eax, %cs:(%edi)            # mov cs:[bx], ax
    .byte 0x66,0x2e,0xa1                # mov eax, cs:[offset16]
    .word   DSC_OFFSET + DSC_GDTPTR
    movw    %ax, %cs:2(%edi)
    movw    %ax, %bp                    # ebp = GDT base
    .byte 0x66
    lgdt    %cs:(%edi)
# Patch ProtectedMode Segment
    .byte   0xb8                        # mov ax, imm16
    .word   PROTECT_MODE_CS             # set AX for segment directly
    movl    %eax, %cs:-2(%edi)          # mov cs:[bx - 2], ax
# Patch ProtectedMode entry
    .byte 0x66, 0xbf                    # mov edi, SMBASE
ASM_PFX(gSmbase): .space 4
    .byte 0x67
    lea     ((Start32bit - _SmiEntryPoint) + 0x8000)(%edi), %ax
    movw     %ax, %cs:-6(%edi)
    movl    %cr0, %ebx
    .byte 0x66
    andl    $0x9ffafff3, %ebx
    .byte 0x66
    orl     $0x23, %ebx
    movl    %ebx, %cr0
    .byte 0x66,0xea
    .space  4
    .space  2
_GdtDesc:   .space 4
            .space 2

Start32bit: 
    movw    $PROTECT_MODE_DS, %ax
    movl    %eax,%ds
    movl    %eax,%es
    movl    %eax,%fs
    movl    %eax,%gs
    movl    %eax,%ss
    .byte   0xbc                          # mov esp, imm32
ASM_PFX(gSmiStack): .space 4
    movl    $ASM_PFX(gcSmiIdtr), %eax
    lidt    (%eax)
    jmp     ProtFlatMode

ASM_PFX(gcSmiPMHandlerTemplate):
_SmiPMEntryPoint:
# when SMM PROT MODE feature is ok, processor will break here with 32bit protected mode
    .byte    0x0bf                        # mov edi, SMBASE
ASM_PFX(gProtModeSmbase):    .space    4
    movl    $PROTECT_MODE_DS, %eax
    pushl   %eax
    movl    %esp, %eax
    addl    %edi, %eax
    addl    $4, %eax
    pushl   %eax
    subl    $8, %eax
    lss     (%eax), %esp

    movl    %edi, %eax
    addl    $(ProtFlatMode - _SmiPMEntryPoint + 0x8000), %eax
    movw    $PROTECT_MODE_CS, %dx
    movw    %dx, 2(%eax)                   # mov cs:[bx - 6], eax
    movl    %eax, 6(%eax)                  # mov cs:[bx - 6], eax

## PURLEY_OVERRIDE_BEGIN_5333001
    movl    %cr0, %ebx
    .byte 0x66
    orl     $0x20, %ebx                    # set CR0.NE
    movl    %ebx, %cr0
## PURLEY_OVERRIDE_END_5333001

    .byte   0xea                           # jmp @ProtFlatMode
    .space  6
ProtFlatMode:
    .byte   0xb8                           # mov eax, imm32
ASM_PFX(gSmiCr3): .space 4
    movl    %eax, %cr3
    movl    $0x668, %eax                   # as cr4.PGE is not set here, refresh cr3
    movl    %eax, %cr4                     # in PreModifyMtrrs() to flush TLB.
    movl    %cr0, %ebx
    orl     $0x080000000, %ebx             # enable paging
    movl    %ebx, %cr0
    leal    DSC_OFFSET(%edi),%ebx
    movw    DSC_DS(%ebx),%ax
    movl    %eax, %ds
    movw    DSC_OTHERSEG(%ebx),%ax
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movw    DSC_SS(%ebx),%ax
    movl    %eax, %ss

    cmpb    $0, ASM_PFX(FeaturePcdGet (PcdCpuSmmStackGuard))
    jz      L5

# Load TSS
    movb    $0x89, (TSS_SEGMENT + 5)(%ebp) # clear busy flag
    movl    $TSS_SEGMENT, %eax
    ltrw    %ax
L5:

#   jmp     _SmiHandler                 # instruction is not needed

_SmiHandler:
    cmpb    $0, ASM_PFX(FeaturePcdGet (PcdCpuSmmDebug))
    jz      L3
    cmpb    $0, ASM_PFX(mSaveStateInMsr)
    jz      L6
    movl    $MSR_DR6, %ecx
    rdmsr
    pushl   %eax
    movl    $MSR_DR7, %ecx
    rdmsr
    movl    %eax, %edx
    popl    %ecx
    jmp     L7

L6:
    call    L1
L1:
    popl    %ebp
    movl    $0x80000001, %eax
    cpuid
    btl     $29, %edx                   # check cpuid to identify X64 or IA32 
    leal    (0x7fc8 - (L1 - _SmiEntryPoint))(%ebp), %edi
    leal    4(%edi), %esi
    jnc     L2
    addl    $4, %esi
L2:
    movl    (%esi), %ecx
    movl    (%edi), %edx
L7:
    movl    %ecx, %dr6
    movl    %edx, %dr7                  # restore DR6 & DR7 before running C code
L3:

    pushl   (%esp)
    
    movl    $ASM_PFX(SmiRendezvous), %eax
    call    *%eax
    popl    %ecx


    cmpb    $0, ASM_PFX(FeaturePcdGet (PcdCpuSmmDebug))
    jz      L4
    cmpb    $0, ASM_PFX(mSaveStateInMsr)
    jnz     L4                          # DR6/DR7 MSR is RO

    movl    %dr6, %ecx
    movl    %dr7, %edx
    movl    %ecx, (%esi)
    movl    %edx, (%edi)
L4:

    rsm

ASM_PFX(gcSmiHandlerSize):    .word      . - _SmiEntryPoint
ASM_PFX(gcSmiPMHandlerSize):  .word      . - _SmiPMEntryPoint
