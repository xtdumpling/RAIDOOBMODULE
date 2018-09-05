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
#   LaunchBiosAcm.asm
#
# Abstract:
#
#   Assembly code of LaunchBiosAcm
#
#------------------------------------------------------------------------------

    .include "txt.i"
    .include "mmx64.i"

#-----------------------------------------------------------------------------
#  Procedure:   LaunchBiosAcm
# 
#  Input:       AcmBase  - Base address of LT BIOS ACM in flash
#               EsiValue - 
# 
#  Output:      
# 
#  Stack:       Available at beginning of routine, but routine goes 'stackless'
# 
#  Registers:   
# 
#  Description: Setup GETSEC environment (protected mode, mtrrs, etc) and
#                 invoke GETSEC:ENTERACCS with requested BIOS ACM entry point.
#                                                        
#-----------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(LaunchBiosAcm)
ASM_PFX(LaunchBiosAcm):

.equ ACM_SIZE_TO_CACHE,  %xmm0
.equ ACM_BASE_TO_CACHE,  %xmm1
.equ NEXT_MTRR_INDEX,    %xmm2
.equ NEXT_MTRR_SIZE,     %xmm3

    enterq $0x30, $0x0

    #
    # Save the general purpose register state
    #
    pushfq
    PUSHA_64

    #
    # Save the parameters passed to us
    #
    mov     %rcx, %rax      # save address of BIOS ACM in MMX0
    MOVQIN0
    mov     %cr3, %rax      # save page table in MMX1
    MOVQIN1
    mov     %rdx, %rax      # save value of ESI for GETSEC[ENTERACCS] in MMX2
    MOVQIN2
    or      %rdx, %rdx      # have we been called to run SCLEAN?
    jz      BeginLaunch     # if so, we dont need to save state.

    cmp     $5, %rbx        # have we been called to run ClearSecretsFlag?
    je      BeginLaunch     # if so, we dont need to save state.

    sgdt    -16(%rbp)        # save gdtr
    sidt    -32(%rbp)       # save idtr
    lea     -16(%rbp),  %rax # rax = address of saved gdtr

    MOVQIN3                 # mm3 = address of saved gdtr

    mov     %rbp, %rax
    MOVQIN4                 # mm4 = rbp
    xor     %rax, %rax
    mov     %ss, %ax
    MOVQIN5                 # mm5 = ss
    mov     %cs, %ax
    MOVQIN6                 # mm6 = cs

    #
    # Save control registers
    #
    mov     %cr4, %rax
    push    %rax
    mov     %cr0, %rax
    push    %rax

    #
    # Save MISC ENABLE MSR
    #
    mov     $IA32_MISC_ENABLE, %rcx
    rdmsr
    push    %rax
    push    %rdx

    #
    # Save MTRR
    #
    mov     $IA32_MTRR_CAP, %rcx
    rdmsr
    and     $0xFF, %rax
    shl     $1, %rax
    mov     %rax, %rcx

SaveNextMtrr: 
    add     $(IA32_MTRR_PHYSBASE0 - 1), %rcx
    rdmsr
    push    %rdx
    push    %rax
    sub     $(IA32_MTRR_PHYSBASE0 - 1), %rcx
    loop    SaveNextMtrr

    # Save def MTRR type
    mov     $IA32_MTRR_DEF_TYPE, %rcx
    rdmsr
    push    %rax
    push    %rdx

    # Save the rest of the segment registers
    xor     %rax, %rax
    mov     %gs, %ax
    push    %rax
    mov     %fs, %ax
    push    %rax
    mov     %es, %ax
    push    %rax
    mov     %ds, %ax
    push    %rax

    #
    # For reloading CS to Long Mode
    #
    mov     %cs, %rcx
    shl     $32, %rcx
    lea     ReloadCS(%rip), %rdx # BUGBUG: Assume it is below 4G
    or      %rdx, %rcx
    push    %rcx

    mov     %cr3, %rax       # save page table in stack
    push    %rax

BeginLaunch: 
    #
    # Now that all of our state is on the stack, save esp
    #
    mov     %rsp, %rax
    MOVQIN7                 # mm7 = rsp

    #--------------------------------
    # Begin to launch ACM
    #--------------------------------

    #
    # Enable SMXE, SSE and debug extensions 
    #
    mov         %cr4, %rax
    or          $(CR4_OSFXSR_MASK + CR4_DE_MASK + CR4_SMXE), %rax
    mov         %rax, %cr4

    #
    # Disable cache
    #
    mov         %cr0, %rax   # set CR0:CD and CR0:NE, clear CR0:NW
    or          $(CR0_CD_MASK | CR0_NE_MASK), %rax
    and         $(~CR0_NW_MASK), %rax
    mov         %rax, %cr0

    #
    # Check to see how we should invalidate the cache
    #
    MOVQOUT2
    or       %rax, %rax      # have we been called to run SCLEAN?
    jz      ScleanInvd
    wbinvd                  # Nope, SCHECK.  Writeback is necessary
    jmp      OverScleanInvd

ScleanInvd: 
    invd                    # Yep, SCLEAN. Invalidate the cache

OverScleanInvd: 
    #
    # Disable MTRRs, set Default Type to UC
    #
    mov     $IA32_MTRR_DEF_TYPE, %rcx
    xor     %rax, %rax
    xor     %rdx, %rdx
    wrmsr

    #
    # Clear all of the Variable MTRRs
    #
    mov     $IA32_MTRR_CAP, %rcx
    rdmsr
    and     $0xFF, %rax
    shl     %rax
    mov     %rax, %rcx
    xor     %rax, %rax
    xor     %rdx, %rdx
ClearMtrrContinue:
    add     $(IA32_MTRR_PHYSBASE0 - 1), %rcx
    wrmsr
    sub     $(IA32_MTRR_PHYSBASE0 - 1), %rcx
    loop    ClearMtrrContinue

    #
    # Determine size of AC module
    #
    MOVQOUT0                            # rax = AcmBase
    mov         %rax, %rsi              # rsi = AcmBase
    mov         ACMOD_SIZE(%rsi), %rax  # rax = size of ACM
    shl         $2, %rax                #  ...in bytes (ACM header has size in dwords)

    #
    # Round up to page size
    #
    mov         %rax, %rcx                      # Save
    .byte 0x48, 0x81, 0xe1, 0x00, 0xf0, 0xff, 0xff          # and         $0xFFFFF000, %rcx               # Number of pages in AC module
    and         $0x0FFF, %rax                   # Number of "less-than-page" bytes
    jz          rounded
    mov         $0x1000, %rax                   # Add the whole page size

rounded:
    add         %rcx, %rax                      # rax = rounded up AC module size
    mov         %rax, %rbx                      # rbx = rounded up AC module size

    #
    # Initialize "locals"
    #
    sub     %rcx, %rcx
    movd    %ecx, NEXT_MTRR_INDEX   # Start from MTRR0

    #
    # Save remaining size to cache
    #
    movd    %ebx, ACM_SIZE_TO_CACHE # Size of ACM that must be cached
    movd    %esi, ACM_BASE_TO_CACHE # Base ACM address

nextMtrr: 
    #
    # Get remaining size to cache
    #
    xor     %rax, %rax
    movd    ACM_SIZE_TO_CACHE, %eax
    and     %rax, %rax
    jz      done                    # If no left size - we are done
    #
    # Determine next size to cache.
    # We start from bottom up. Use the following algorythm:
    # 1. Get our own alignment. Max size we can cache equals to our alignment
    # 2. Determine what is bigger - alignment or remaining size to cache.
    #    If aligment is bigger - cache it.
    #      Adjust remaing size to cache and base address
    #      Loop to 1.
    #    If remaining size to cache is bigger
    #      Determine the biggest 2^N part of it and cache it.
    #      Adjust remaing size to cache and base address
    #      Loop to 1.
    # 3. End when there is no left size to cache or no left MTRRs
    #
    xor     %rsi, %rsi
    movd    ACM_BASE_TO_CACHE, %esi
    bsf     %rsi, %rcx              # Get index of lowest bit set in base address
    #
    # Convert index into size to be cached by next MTRR
    #
    mov     $0x1, %rdx
    shl     %cl, %rdx               # Alignment is in edx
    cmp     %rax, %rdx              # What is bigger, alignment or remaining size?
    jbe     gotSize                 # JIf aligment is less
    #
    # Remaining size is bigger. Get the biggest part of it, 2^N in size
    #
    bsr     %rax, %rcx              # Get index of highest set bit
    #
    # Convert index into size to be cached by next MTRR
    #
    mov     $1, %rdx
    shl     %cl, %rdx               # Size to cache

gotSize: 
    mov     %rdx, %rax
    movd    %eax, NEXT_MTRR_SIZE    # Save

    #
    # Compute MTRR mask value:  Mask = NOT (Size - 1)
    #
    dec     %rax                    # eax - address of last byte in rounded AC module size
    not     %eax                    # eax contains low 32 bits of mask
    or      $MTRR_VALID, %rax        # Set valid bit
    #
    # Program mask register
    #
    mov     $IA32_MTRR_PHYSMASK0, %rcx # setup variable mtrr 
    xor     %rbx, %rbx
    movd    NEXT_MTRR_INDEX, %ebx
    add     %rbx, %rcx

    mov     $0xF, %rdx      # 8K range (FFFFFFE800)
    wrmsr
    #
    # Program base register
    #
    xor     %rdx, %rdx
    mov     $IA32_MTRR_PHYSBASE0, %rcx # setup variable mtrr 
    add     %ebx, %ecx               # ebx is still NEXT_MTRR_INDEX

    xor     %rax, %rax
    movd    ACM_BASE_TO_CACHE, %eax
    orl     $WB, %eax                # set type to write back
    wrmsr
    #
    # Advance and loop
    # Reduce remaining size to cache
    #
    movd    ACM_SIZE_TO_CACHE, %ebx
    movd    NEXT_MTRR_SIZE, %eax
    sub     %eax, %ebx
    movd    %ebx, ACM_SIZE_TO_CACHE

    #
    # Increment MTRR index
    #
    movd    NEXT_MTRR_INDEX, %ebx
    add     $2, %ebx
    movd    %ebx, NEXT_MTRR_INDEX
    #
    # Increment base address to cache
    #
    movd    ACM_BASE_TO_CACHE, %ebx
    movd    NEXT_MTRR_SIZE, %eax
    add     %eax, %ebx
    movd    %ebx, ACM_BASE_TO_CACHE

    jmp     nextMtrr
done:

    #
    # Re-enable Variable MTRRs
    #
    mov     $IA32_MTRR_DEF_TYPE, %rcx
    xor     %rdx, %rdx
    mov     $MTRR_ENABLE, %rax
    wrmsr

    #
    # Re-enable cache
    #
    mov     %cr0, %rax
    and     $(~CR0_CD_MASK), %rax
    mov     %rax, %cr0

    #
    # Clean all MCi_STATUS MSR registers
    # SCLEAN will generate GPF otherwise
    #
    mov     $MCG_CAP, %rcx
    rdmsr
    movzb   %al, %rbx               # rbx = MCR bank count 
    sub     %rax, %rax              # Write 0 into all MCi_STATUS registers
    sub     %rdx, %rdx
    mov     $MC0_STATUS, %rcx

McaErrorCleanLoopStart: 
    wrmsr
    dec     %rbx
    jz      CallGetsec
    add     $4, %rcx                # rcx = number of MSRs per bank
    jmp     McaErrorCleanLoopStart

CallGetsec:
    #
    # Change to Compatible Segment
    #
    mov     $0x20, %rcx             # BUGBUG: Hardcode Compatible mode segment
    shl     $32, %rcx
    mov     $Compatible, %rdx        # BUGBUG: assume address < 4G
    or      %rdx, %rcx
    push    %rcx
    retf

Compatible:
    #
    # disable paging
    #
    mov     %cr0, %rax
    btr     $31, %eax               # set PG=0
    mov     %rax, %cr0
    #
    # set EFER.LME = 0 to leave long mode
    #
    mov     $0x0c0000080, %ecx      # EFER MSR number
    rdmsr                           # Read EFER
    btr     $8, %eax                # Set LME=0
    wrmsr                           # Write EFER

    #
    # Call GETSEC[ENTERACCS]
    #
    MOVDOUT2                        # eax = ACM function
    mov     %eax, %esi              # esi = ACM function
    MOVDOUT0                        # eax = AcmBase
    mov     %eax, %ebx              # ebx = AcmBase
    movl    ACMOD_SIZE(%rbx), %ecx  # ebx = size of ACM in dwords
    shl     $2, %ecx                # ecx = size of ACM in bytes
    xor     %edx, %edx
    xor     %edi, %edi
    mov     $ENTERACCS, %eax         # eax = ENTERACCS

    _GETSEC

    #
    # Check if we need to restore BIOS state and configuration
    #
    MOVDOUT2
    or      %eax, %eax      # Have we been called to run SCLEAN?
    jz      ResetSystem     # Jump to ResetSystem, no-need to restore state,
                            # because system will reset soon.
    cmp     $5, %rax        # Have we been called to run ClearSecretsFlag?
    je      ResetSystem     # Jump to ResetSystem, no-need to restore state,
                            # because system will reset soon.

    #
    # Reload the GDTR
    #
    MOVDOUT3                # ebx = the address of our local variable
    lgdt    (%rax)

    #
    # Restore the stack
    #
    MOVDOUT4
    mov     %eax, %ebp      # restore ebp
    MOVDOUT7
    mov     %eax, %esp      # restore esp
    MOVDOUT5
    mov     %ax, %ss        # restore ss

    #
    # Enable protected mode
    #
    mov     $0x33, %ecx
    mov     %rcx, %cr0

    #
    # Enable PAE in CR4
    #
    mov     $(0x668 | CR4_SMXE), %ecx
    mov     %rcx, %cr4

    #
    # Reload CR3
    #
    pop     %rax
    mov     %cr3, %rax
    pop     %rax            # pop the redandant

    #
    # Setup re-enable of paging
    #
    mov     %cr0, %rbx
    bts     $31, %ebx

    #
    # Set EFER.LME to re-enable ia32-e
    #
    mov     $0x0c0000080, %ecx
    xor     %edx, %edx
    mov     $0x500, %eax
    wrmsr

    #
    # Re-enable paging
    #
    mov     %rbx, %cr0

    #
    # Now we are in Compatibility mode
    #
    MOVDOUT3                # rax = the address of our local variable
    lgdt    (%rax)
    lidt    -0x10(%rax)

    #
    # Reload cs register 
    #
    ret
ReloadCS: 
    #
    # Restore the rest of the segment registers
    #
    pop     %rax
    mov     %ax, %ds
    pop     %rax
    mov     %ax, %es
    pop     %rax
    mov     %ax, %fs
    pop     %rax
    mov     %ax, %gs

    #
    # Clear out the cache 
    #
    mov        %cr0, %rax           # Set CR0:CD and CR0:NE, clear CR0:NW
    or         $(CR0_CD_MASK | CR0_NE_MASK), %rax
    and        $(~CR0_NW_MASK), %rax
    mov        %rax, %cr0
    wbinvd                          # Flush and invalidate the cache

    #
    # Restore def MTRR type
    #
    mov     $IA32_MTRR_DEF_TYPE, %rcx
    pop     %rdx
    pop     %rax
    wrmsr

    #
    # Reset Variable MTRRs to Pre-GETSEC state
    #
    mov     $IA32_MTRR_CAP, %rcx
    rdmsr
    and     $0xFF, %rax
    shl     %rax
    mov     %rax, %rcx
    mov     %rax, %rbx
    shl     $4, %rbx   # rbx = the total length for MTRR register
    add     %rbx, %rsp # adjust rsp to skip MTRR register
    mov     %rsp, %rbx
    sub     $8, %rbx   # let rbx point to head of MTRR register now

RestoreNextMtrr: 
    add     $(IA32_MTRR_PHYSBASE0 - 1), %rcx
    mov     (%rbx), %rdx
    mov     -8(%rbx), %rax
    wrmsr
    sub     $(IA32_MTRR_PHYSBASE0 - 1), %rcx
    sub     $16, %rbx
    loop    RestoreNextMtrr

    #
    # Restore MISC ENABLE MSR
    #
    pop     %rdx
    pop     %rax
    mov     $IA32_MISC_ENABLE, %rcx
    wrmsr

    #
    # Restore control registers
    #
    pop     %rax
    mov     %rax, %cr0
    pop     %rax
    mov     %rax, %cr4

    wbinvd                          # Flush and invalidate the cache

    POPA_64
    popfq

_EMMS
    leave  # C epilog
    ret

ResetSystem: 
    mov     $0xCF9, %dx
    mov     $0xA, %al
    out     %al, %dx
    jmp     1f
1:
    jmp     2f
2:
    mov     $0xE, %al
    out     %al, %dx
DeadLoop: 
    jmp     .           # should never get here, but just in case
