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
    .include "stackless32.i"

.code32
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

    # C prolog
    pushl   %ebp
    movl    %esp, %ebp
    add     $0xfffffff0, %esp

    pushfl
    pushal

    # check if the AcmBase is a good address (none zero)
    cmpl    $0, 8(%ebp)     # bad ACM address? - AcmBase
    je      SkipLaunch      # Yes, then exit without any farther work

    #
    # Check to see if we need to save state for return to BIOS after GETSEC
    #
    movl    12(%ebp), %ebx  # ebx = GETSEC leaf number - EsiValue
    orl     %ebx, %ebx      # have we been called to run SCLEAN?
    jz      BeginLaunch     # if so, we dont need to save state.

    cmpl    $5, %ebx        # have we been called to run ClearSecretsFlag?
    je      BeginLaunch     # if so, we dont need to save state.

    sgdt    -8(%ebp)        # SavedGdtr
    sidt    -16(%ebp)       # SavedIdtr
    leal    -8(%ebp),  %ebx # NOTE: This assumes segs will always be zero-based - SavedGdtr
                            # NOTE: This assumption is valid per PEI CIS
    MOVDIN3                 # mm3 = address of our local variable
    movl    %ebp, %ebx
    MOVDIN4                 # mm4 = ebp
    xorl    %ebx, %ebx
    movw    %ss, %bx
    MOVDIN5                 # mm5 = ss

    #
    # Save control registers
    #
    movl    %cr4, %ebx
    pushl   %ebx
    movl    %cr0, %ebx
    pushl   %ebx

    #
    # Save MISC ENABLE MSR
    #
    movl    $IA32_MISC_ENABLE, %ecx
    rdmsr
    pushl   %eax
    pushl   %edx

    #
    # Save MTRR
    #
    movl    $IA32_MTRR_CAP, %ecx
    rdmsr
    andl    $0xFF, %eax
    shll    %eax
    movl    %eax, %ecx

SaveNextMtrr: 
    addl    $(IA32_MTRR_PHYSBASE0 - 1), %ecx
    rdmsr
    pushl   %edx
    pushl   %eax
    subl    $(IA32_MTRR_PHYSBASE0 - 1), %ecx
    loop    SaveNextMtrr

    # Save def MTRR type
    movl    $IA32_MTRR_DEF_TYPE, %ecx
    rdmsr
    pushl   %eax
    pushl   %edx

    # Save the rest of the segment registers
    xorl    %ebx, %ebx
    movw    %gs, %bx
    pushl   %ebx
    movw    %fs, %bx
    pushl   %ebx
    movw    %es, %bx
    pushl   %ebx
    movw    %ds, %bx
    pushl   %ebx

    #
    # For reloading CS
    #
    movw    %cs, %bx
    pushl   %ebx
    lea     ReloadCS, %ebx
    pushl   %ebx

BeginLaunch: 
    #
    # Now that all of our state is on the stack, save esp
    #
    movl    %esp, %ebx
    MOVDIN7                 # mm7 = esp

    #--------------------------------
    # Begin to launch ACM
    #--------------------------------

    #
    # Save the parameters passed to us
    #
    movl    8(%ebp), %ebx   # save address of BIOS ACM in MMX0 
    MOVDIN0
    movl    12(%ebp), %ebx  # save value of ESI for GETSEC[ENTERACCS] in MMX2 
    MOVDIN2

    #
    # Enable SMXE, SSE and debug extensions 
    #
    movl        %cr4, %eax
    orl         $(CR4_OSFXSR_MASK + CR4_DE_MASK + CR4_SMXE), %eax
    movl        %eax, %cr4

    #
    # Disable cache
    #
    movl        %cr0, %eax   # set CR0:CD and CR0:NE, clear CR0:NW
    orl         $(CR0_CD_MASK | CR0_NE_MASK), %eax
    andl        $(~CR0_NW_MASK), %eax
    movl        %eax, %cr0

    #If LockConfig func 06, we jump pass this invd 
    MOVDOUT2
    cmpl     $0x6, %ebx
    jz       OverScleanInvd
    #
    # Check to see how we should invalidate the cache
    #
    MOVDOUT2
    orl      %ebx, %ebx      # have we been called to run SCLEAN?
    # bugbug, current MCU 20A still need skip wbinvd for LT-SX
    #jz      ScleanInvd
    jmp      ScleanInvd
    wbinvd                  # Nope, SCHECK.  Writeback is necessary
    jmp      OverScleanInvd

ScleanInvd: 
    invd                    # Yep, SCLEAN. Invalidate the cache

OverScleanInvd: 
    #
    # Disable MTRRs, set Default Type to UC
    #
    movl    $IA32_MTRR_DEF_TYPE, %ecx
    xorl    %eax, %eax
    xorl    %edx, %edx
    wrmsr

    #
    # Clear all of the Variable MTRRs
    #
    movl    $IA32_MTRR_CAP, %ecx
    rdmsr
    andl    $0xFF, %eax
    shll    %eax
    movl    %eax, %ecx
    xorl    %eax, %eax
    xorl    %edx, %edx
ClearMtrrContinue:
    addl    $(IA32_MTRR_PHYSBASE0 - 1), %ecx
    wrmsr
    subl    $(IA32_MTRR_PHYSBASE0 - 1), %ecx
    loop    ClearMtrrContinue

    #
    # Determine size of AC module
    #
    MOVDOUT0                            # ebx = AcmBase
    movl        %ebx, %esi              # esi = AcmBase
    movl        ACMOD_SIZE(%esi), %eax  # ebx = size of ACM
    shll        $2, %eax                #  ...in bytes (ACM header has size in dwords)

    #
    # Round up to page size
    #
    movl        %eax, %ecx                      # Save
    andl        $0xFFFFF000, %ecx               # Number of pages in AC module
    andl        $0xFFF, %eax                    # Number of "less-than-page" bytes
    jz          rounded
    movl        $0x1000, %eax                   # Add the whole page size

rounded:
    addl        %ecx, %eax                      # eax = rounded up AC module size

    #
    # Initialize "locals"
    #
    subl    %ecx, %ecx
    movd    %ecx, NEXT_MTRR_INDEX   # Start from MTRR0

    #
    # Save remaining size to cache
    #
    movd    %eax, ACM_SIZE_TO_CACHE # Size of ACM that must be cached
    movd    %esi, ACM_BASE_TO_CACHE # Base ACM address

nextMtrr: 
    #
    # Get remaining size to cache
    #
    movd    ACM_SIZE_TO_CACHE, %eax
    andl    %eax, %eax
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
    movd    ACM_BASE_TO_CACHE, %esi
    bsfl    %esi, %ecx              # Get index of lowest bit set in base address
    #
    # Convert index into size to be cached by next MTRR
    #
    movl    $0x1, %edx
    shl     %cl, %edx               # Alignment is in edx
    cmpl    %eax, %edx              # What is bigger, alignment or remaining size?
    jbe     gotSize                 # JIf aligment is less
    #
    # Remaining size is bigger. Get the biggest part of it, 2^N in size
    #
    bsrl    %eax, %ecx              # Get index of highest set bit
    #
    # Convert index into size to be cached by next MTRR
    #
    movl    $1, %edx
    shl     %cl, %edx               # Size to cache

gotSize: 
    movl    %edx, %eax
    movd    %eax, NEXT_MTRR_SIZE    # Save

    #
    # Compute MTRR mask value:  Mask = NOT (Size - 1)
    #
    decl    %eax                    # eax - address of last byte in rounded AC module size
    notl    %eax                    # eax contains low 32 bits of mask
    orl     $MTRR_VALID, %eax        # Set valid bit
    #
    # Program mask register
    #
    movl    $IA32_MTRR_PHYSMASK0, %ecx # setup variable mtrr 
    movd    NEXT_MTRR_INDEX, %ebx
    addl    %ebx, %ecx

    movl    $0xF, %edx      # 8K range (FFFFFFE800)
    wrmsr
    #
    # Program base register
    #
    xorl    %edx, %edx
    movl    $IA32_MTRR_PHYSBASE0, %ecx # setup variable mtrr 
    addl    %ebx, %ecx               # ebx is still NEXT_MTRR_INDEX

    movd    ACM_BASE_TO_CACHE, %eax
    orl     $WB, %eax                # set type to write back
    wrmsr
    #
    # Advance and loop
    # Reduce remaining size to cache
    #
    movd    ACM_SIZE_TO_CACHE, %ebx
    movd    NEXT_MTRR_SIZE, %eax
    subl    %eax, %ebx
    movd    %ebx, ACM_SIZE_TO_CACHE

    #
    # Increment MTRR index
    #
    movd    NEXT_MTRR_INDEX, %ebx
    addl    $2, %ebx
    movd    %ebx, NEXT_MTRR_INDEX
    #
    # Increment base address to cache
    #
    movd    ACM_BASE_TO_CACHE, %ebx
    movd    NEXT_MTRR_SIZE, %eax
    addl    %eax, %ebx
    movd    %ebx, ACM_BASE_TO_CACHE

    jmp     nextMtrr
done:

    #
    # Re-enable Variable MTRRs
    #
    movl    $IA32_MTRR_DEF_TYPE, %ecx
    xorl    %edx, %edx
    movl    $MTRR_ENABLE, %eax
    wrmsr

    #
    # Re-enable cache
    #
    movl    %cr0, %eax
    andl    $(~CR0_CD_MASK), %eax
    movl    %eax, %cr0

    #
    # Clean all MCi_STATUS MSR registers
    # SCLEAN will generate GPF otherwise
    #
    movl    $MCG_CAP, %ecx
    rdmsr
    movzbl  %al, %ebx               # ebx = MCR bank count 
    subl    %eax, %eax              # Write 0 into all MCi_STATUS registers
    subl    %edx, %edx
    movl    $MC0_STATUS, %ecx

McaErrorCleanLoopStart: 
    wrmsr
    decl    %ebx
    jz      CallGetsec
    addl    $4, %ecx                # ecx = number of MSRs per bank
    jmp     McaErrorCleanLoopStart

    #
    # Call GETSEC[ENTERACCS]
    #
CallGetsec: 
    movl    $ENTERACCS, %eax         # eax = ENTERACCS
    MOVDOUT2                        # ebx = ACM function
    movl    %ebx, %esi              # esi = ACM function
    MOVDOUT0                        # ebx = AcmBase
    movl    ACMOD_SIZE(%ebx), %ecx  # ecx = size of ACM in dwords
    shll    $2, %ecx                # ecx = size of ACM in bytes
    xorl    %edx, %edx
    xorl    %edi, %edi
    _GETSEC

    #
    # Check if we need to restore BIOS state and configuration
    #
    MOVDOUT2
    orl     %ebx, %ebx      # Have we been called to run SCLEAN?
    jz      ResetSystem     # Jump to ResetSystem, no-need to restore state,
                            # because system will reset soon.
    cmpl    $5, %ebx        # Have we been called to run ClearSecretsFlag?
    je      ResetSystem     # Jump to ResetSystem, no-need to restore state,
                            # because system will reset soon.

    MOVDOUT3                # ebx = the address of our local variable
    lgdt    (%ebx)
    lidt    -8(%ebx)

    MOVDOUT4
    movl    %ebx, %ebp      # restore ebp
    MOVDOUT7
    movl    %ebx, %esp      # restore esp
    MOVDOUT5
    movw    %bx, %ss        # restore ss

    #
    # Reload cs register 
    #
    retl
ReloadCS: 

    #
    # Restore the rest of the segment registers
    #
    popl    %ebx
    movw    %bx, %ds
    popl    %ebx
    movw    %bx, %es
    popl    %ebx
    movw    %bx, %fs
    popl    %ebx
    movw    %bx, %gs

    #
    # Clear out the cache 
    #
    movl       %cr0, %eax           # Set CR0:CD and CR0:NE, clear CR0:NW
    orl        $(CR0_CD_MASK | CR0_NE_MASK), %eax
    and        $(~CR0_NW_MASK), %eax
    movl       %eax, %cr0
    wbinvd                          # Flush and invalidate the cache

    #
    # Restore def MTRR type
    #
    movl    $IA32_MTRR_DEF_TYPE, %ecx
    popl    %edx
    popl    %eax
    wrmsr

    #
    # Reset Variable MTRRs to Pre-GETSEC state
    #
    movl    $IA32_MTRR_CAP, %ecx
    rdmsr
    andl    $0xFF, %eax
    shll    %eax
    movl    %eax, %ecx
    movl    %eax, %ebx
    shll    $3, %ebx   # ebx = the total length for MTRR register
    addl    %ebx, %esp # adjust esp to skip MTRR register
    movl    %esp, %ebx
    subl    $4, %ebx   # let ebx point to head of MTRR register now

RestoreNextMtrr: 
    addl    $(IA32_MTRR_PHYSBASE0 - 1), %ecx
    movl    (%ebx), %edx
    movl    -4(%ebx), %eax
    wrmsr
    subl    $(IA32_MTRR_PHYSBASE0 - 1), %ecx
    subl    $8, %ebx
    loop    RestoreNextMtrr

    #
    # Restore MISC ENABLE MSR
    #
    popl    %edx
    popl    %eax
    movl    $IA32_MISC_ENABLE, %ecx
    wrmsr

    #
    # Restore control registers
    #
    popl    %ebx
    movl    %ebx, %cr0
    popl    %ebx
    movl    %ebx, %cr4

SkipLaunch: 
    popal
    popfl
_EMMS
    leave  # C epilog
    ret

ResetSystem: 
    movw    $0xCF9, %dx
    movb    $0xA, %al
    outb    %al, %dx
    jmp     1f
1:
    jmp     2f
2:
#ifdef POWERON
    movb    $0x3, %al
#else
    movb    $0x6, %al
#endif
    outb    %al, %dx
DeadLoop: 
    jmp     .           # should never get here, but just in case
