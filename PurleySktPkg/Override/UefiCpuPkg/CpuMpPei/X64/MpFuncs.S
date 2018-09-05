#------------------------------------------------------------------------------ ;
# Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
# Module Name:
#
#   MpFuncs.S
#
# Abstract:
#
#   This is the assembly code for MP support
#
#-------------------------------------------------------------------------------

.equ PROTECT_MODE_CS,                       0x10
.equ PROTECT_MODE_DS,                       0x18
.equ LONG_MODE_CS,                          0x38
.equ LONG_MODE_DS,                          0x30

.equ VacantFlag,                            0x00
.equ NotVacantFlag,                         0xff

.equ CPU_SWITCH_STATE_IDLE,                 0
.equ CPU_SWITCH_STATE_STORED,               1
.equ CPU_SWITCH_STATE_LOADED,               2

.equ LockLocation,                          (RendezvousFunnelProcEnd - RendezvousFunnelProcStart)
.equ StackStartAddressLocation,             LockLocation + 0x08
.equ StackSizeLocation,                     LockLocation + 0x10
.equ ApProcedureLocation,                   LockLocation + 0x18
.equ GdtrLocation,                          LockLocation + 0x20
.equ IdtrLocation,                          LockLocation + 0x2A
.equ BufferStartLocation,                   LockLocation + 0x34
.equ PmodeOffsetLocation,                   LockLocation + 0x3C
# OVERIDE_HSD_5370234_BEGIN
.equ ApIndexLocation,                       LockLocation + 0x44
.equ NumApsExecutingLocation,               LockLocation + 0x4C
.equ LmodeOffsetLocation,                   LockLocation + 0x54
.equ Cr3Location,                           LockLocation + 0x5C
# OVERIDE_HSD_5370234_END

.extern  InitializeFloatingPointUnits

.text

#-------------------------------------------------------------------------------------
#RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
#procedure serializes all the AP processors through an Init sequence. It must be
#noted that APs arrive here very raw...ie: real mode, no stack.
#ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
#IS IN MACHINE CODE.
#-------------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(RendezvousFunnelProc)
ASM_PFX(RendezvousFunnelProc):
RendezvousFunnelProcStart:
# At this point CS = 0x(vv00) and ip= 0x0.
# Save BIST information to ebp firstly
    .byte 0x66,0x8b,0xe8              # mov        ebp, eax    ; save BIST information

    .byte 0x8c,0xc8                   # mov        ax,cs
    .byte 0x8e,0xd8                   # mov        ds,ax
    .byte 0x8e,0xc0                   # mov        es,ax
    .byte 0x8e,0xd0                   # mov        ss,ax
    .byte 0x33,0xc0                   # xor        ax,ax
    .byte 0x8e,0xe0                   # mov        fs,ax
    .byte 0x8e,0xe8                   # mov        gs,ax

    .byte 0xBE                        # opcode of mov si, mem16
    .word BufferStartLocation
    .byte 0x66,0x8B,0x1C              # mov        ebx,dword ptr [si]

    .byte 0xBF                        # opcode of mov di, mem16
    .word PmodeOffsetLocation
    .byte 0x66,0x8B,0x5               # mov        eax,dword ptr [di]
    .byte 0x8B,0xF8                   # mov        di, ax
    .byte 0x83,0xEF,0x6               # sub        di, 06h
    .byte 0x66,0x3,0xC3               # add        eax, ebx
    .byte 0x66,0x89,0x5               # mov        dword ptr [di],eax

    .byte 0xBF                        # opcode of mov di, mem16
    .word LmodeOffsetLocation
    .byte 0x66,0x8B,0x5               # mov        eax,dword ptr [di]
    .byte 0x8B,0xF8                   # mov        di, ax
    .byte 0x83,0xEF,0x6               # sub        di, 06h
    .byte 0x66,0x3,0xC3               # add        eax, ebx
    .byte 0x66,0x89,0x5               # mov        dword ptr [di],eax

    .byte 0xBE
    .word Cr3Location
    .byte 0x66,0x8B,0xC               # mov        ecx,dword ptr [si]     ; ECX is keeping the value of CR3

    .byte 0xBE                        # opcode of mov si, mem16
    .word GdtrLocation
    .byte 0x66                        # db         66h
    .byte 0x2E,0xF,0x1,0x14           # lgdt       fword ptr cs:[si]

    .byte 0xBE
    .word IdtrLocation
    .byte 0x66                        # db         66h
    .byte 0x2E,0xF,0x1,0x1C           # lidt       fword ptr cs:[si]

    .byte 0x33,0xC0                   # xor        ax,  ax
    .byte 0x8E,0xD8                   # mov        ds,  ax

    .byte 0xF,0x20,0xC0               # mov        eax, cr0               ;Get control register 0
    .byte 0x66,0x83,0xC8,0x3          # or         eax, 000000003h        ;Set PE bit (bit #0) & MP
    .byte 0xF,0x22,0xC0               # mov        cr0, eax

    .byte 0x66,0x67,0xEA              # far jump
    .long 0x0                         # 32-bit offset
    .word PROTECT_MODE_CS

Flat32Start:                          # protected mode entry point
    movw       $PROTECT_MODE_DS, %ax
    movw       %ax, %ds
    movw       %ax, %es
    movw       %ax, %fs
    movw       %ax, %gs
    movw       %ax, %ss

    .byte 0xF,0x20,0xE0           # mov        eax, cr4
    .byte 0xF,0xBA,0xE8,0x5       # bts        eax, 5
    .byte 0xF,0x22,0xE0           # mov        cr4, eax

    .byte 0xF,0x22,0xD9           # mov        cr3, ecx

    .byte 0xB9
    .long 0xC0000080              # mov        ecx, 0c0000080h     ; EFER MSR number.
    .byte 0xF,0x32                # rdmsr                          ; Read EFER.
    .byte 0xF,0xBA,0xE8,0x8       # bts        eax, 8              ; Set LME=1.
    .byte 0xF,0x30                # wrmsr                          ; Write EFER.

    .byte 0xF,0x20,0xC0           # mov        eax, cr0            ; Read CR0.
    .byte 0xF,0xBA,0xE8,0x1F      # bts        eax, 31             ; Set PG=1.
    .byte 0xF,0x22,0xC0           # mov        cr0, eax            ; Write CR0.

LONG_JUMP: 
    .byte 0x67,0xEA               # far jump
    .long 0x0                     # 32-bit offset
    .word LONG_MODE_CS

LongModeStart:
    movw       $LONG_MODE_DS, %ax
    movw       %ax, %ds
    movw       %ax, %es
    movw       %ax, %ss

    movl       %ebx, %esi

# OVERIDE_HSD_5370234_BEGIN
    # Increment the number of APs executing here as early as possible
    # This is decremented in C code when AP is finished executing
    movl %esi, %edi
    addl $NumApsExecutingLocation, %edi
    lock incl (%edi)
# OVERIDE_HSD_5370234_END

    movl       %esi, %edi
    addl       $LockLocation, %edi
    movq       $NotVacantFlag, %rax

TestLock: 
    xchgq      %rax, (%edi)
    cmp        $NotVacantFlag, %rax
    jz         TestLock

    movl       %esi, %edi
    addl       ApIndexLocation, %edi #OVERIDE_HSD_5370234
    incl       (%edi)
    movl       (%edi), %ebx

ProgramStack: 
    movl       %esi, %edi
    addl       StackSizeLocation, %edi
    movq       (%edi), %rax
    movl       %esi, %edi
    addl       StackStartAddressLocation, %edi
    add        (%edi), %rax
    mov        %rax, %rsp
    movq       %rax, (%edi)

Releaselock: 
    movq       $VacantFlag, %rax
    movl       %esi, %edi
    addl       $LockLocation, %edi
    xchg       %rax, (%edi)

CProcedureInvoke: 
    pushq      %rbp              # push BIST data
    xor        %rbp, %rbp
    pushq      %rbp
    mov        %rsp, %rbp

    mov        InitializeFloatingPointUnits, %rax
    subq       $0x20, %rsp
    call       *%rax             # Call assembly function to initialize FPU per UEFI spec
    addq       $0x20, %rsp

    movl       %ebx, %edx        # edx is ApIndex OVERIDE_HSD_5370234
    movl       %esi, %ecx
    addl       $LockLocation, %ecx # rcx is address of exchange info data buffer

    movl       %esi, %edi
    addl       $ApProcedureLocation, %edi
    movq       (%edi), %rax

    subq       $0x20, %rsp
    call       *%rax               # invoke C function
    addq       $0x20, %rsp
    jmp        .
RendezvousFunnelProcEnd:

ASM_GLOBAL ASM_PFX(AsmCliHltLoop)
ASM_PFX(AsmCliHltLoop):
1:
    cli
    hlt
    jmp 1b

#-------------------------------------------------------------------------------------
#  AsmGetAddressMap (&AddressMap);
#-------------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
    mov        $RendezvousFunnelProcStart, %rax
    movq       %rax, (%rcx)
    movq       $(Flat32Start - RendezvousFunnelProcStart), 8(%rcx)
    movq       $(LongModeStart - RendezvousFunnelProcStart), 0x10(%rcx)
    movq       $(RendezvousFunnelProcEnd - RendezvousFunnelProcStart), 0x18(%rcx)
    ret

#-------------------------------------------------------------------------------------
#AsmExchangeRole procedure follows. This procedure executed by current BSP, that is
#about to become an AP. It switches it stack with the current AP.
#AsmExchangeRole (IN   CPU_EXCHANGE_INFO    *MyInfo, IN   CPU_EXCHANGE_INFO    *OthersInfo);
#-------------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmExchangeRole)
ASM_PFX(AsmExchangeRole):
    # DO NOT call other functions in this function, since 2 CPU may use 1 stack
    # at the same time. If 1 CPU try to call a function, stack will be corrupted.

    pushq      %rax
    pushq      %rbx
    pushq      %rcx
    pushq      %rdx
    pushq      %rsi
    pushq      %rdi
    pushq      %rbp
    pushq      %r8
    pushq      %r9
    pushq      %r10
    pushq      %r11
    pushq      %r12
    pushq      %r13
    pushq      %r14
    pushq      %r15

    mov        %cr0, %rax
    pushq      %rax

    mov        %cr4, %rax
    pushq      %rax

    # rsi contains MyInfo pointer
    mov        %rcx, %rsi

    # rdi contains OthersInfo pointer
    mov        %rdx, %rdi

    #Store EFLAGS, GDTR and IDTR regiter to stack
    pushfq
    sgdt       16(%rsi)
    sidt       26(%rsi)

    # Store the its StackPointer
    movq       %rsp, 8(%rsi)

    # update its switch state to STORED
    movb       $CPU_SWITCH_STATE_STORED, (%rsi)

WaitForOtherStored: 
    # wait until the other CPU finish storing its state
    cmpb       $CPU_SWITCH_STATE_STORED, (%rdi)
    jz         OtherStored
    pause
    jmp        WaitForOtherStored

OtherStored: 
    # Since another CPU already stored its state, load them
    # load GDTR value
    lgdt        16(%rdi)

    # load IDTR value
    lidt        26(%rdi)

    # load its future StackPointer
    movq        8(%rdi), %rsp

    # update the other CPUs switch state to LOADED
    movb        $CPU_SWITCH_STATE_LOADED, (%rdi)

WaitForOtherLoaded: 
    # wait until the other CPU finish loading new state,
    # otherwise the data in stack may corrupt
    cmpb       $CPU_SWITCH_STATE_LOADED, (%rsi)
    jz         OtherLoaded
    pause
    jmp        WaitForOtherLoaded

OtherLoaded: 
    # since the other CPU already get the data it want, leave this procedure
    popfq

    popq       %rax
    mov        %rax, %cr4

    popq       %rax
    mov        %rax, %cr0

    popq       %r15
    popq       %r14
    popq       %r13
    popq       %r12
    popq       %r11
    popq       %r10
    popq       %r9
    popq       %r8
    popq       %rbp
    popq       %rdi
    popq       %rsi
    popq       %rdx
    popq       %rcx
    popq       %rbx
    popq       %rax

    ret

ASM_GLOBAL ASM_PFX(AsmInitializeGdt)
ASM_PFX(AsmInitializeGdt):
    pushq      %rbp
    mov        %rsp, %rbp

    lgdt       (%rcx)

    subl       $0x10, rsp
    lea        SetCodeSelectorFarJump, %rax
    mov        %rax, (%rsp)
    mov        $LONG_MODE_CS, %rdx
    mov        %dx, 4(%rsp)
    jmp        *(%rsp)
SetCodeSelectorFarJump: 
    addl       $0x10, rsp

    mov        $LONG_MODE_DS, %rax
    movw       %ax, %ds
    movw       %ax, %es
    movw       %ax, %fs
    movw       %ax, %gs
    movw       %ax, %ss

    popq       %rbp
    ret
