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
#   MpFuncs32.S
#
# Abstract:
#
#   This is the assembly code for MP support
#
#-------------------------------------------------------------------------------


.equ PROTECT_MODE_CS,              0x10
.equ PROTECT_MODE_DS,              0x18

.equ VacantFlag,                   0x00
.equ NotVacantFlag,                0xff

.equ CPU_SWITCH_STATE_IDLE,        0
.equ CPU_SWITCH_STATE_STORED,      1
.equ CPU_SWITCH_STATE_LOADED,      2

.equ LockLocation,                 (RendezvousFunnelProcEnd - RendezvousFunnelProcStart)
.equ StackStartAddressLocation,    LockLocation + 0x04
.equ StackSizeLocation,            LockLocation + 0x08
.equ ApProcedureLocation,          LockLocation + 0x0C
.equ GdtrLocation,                 LockLocation + 0x10
.equ IdtrLocation,                 LockLocation + 0x16
.equ BufferStartLocation,          LockLocation + 0x1C
.equ PmodeOffsetLocation,          LockLocation + 0x20
.equ ApIndexLocation,              LockLocation + 0x24 #OVERIDE_HSD_5370234
.equ NumApsExecutingLocation,      LockLocation + 0x28 #OVERIDE_HSD_5370234


.text

.extern InitializeFloatingPointUnits

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
    .byte 0x66,0x8b,0xe8                # mov        ebp, eax    ; save BIST information

    .byte 0x8c,0xc8                     # mov        ax,cs
    .byte 0x8e,0xd8                     # mov        ds,ax
    .byte 0x8e,0xc0                     # mov        es,ax
    .byte 0x8e,0xd0                     # mov        ss,ax
    .byte 0x33,0xc0                     # xor        ax,ax
    .byte 0x8e,0xe0                     # mov        fs,ax
    .byte 0x8e,0xe8                     # mov        gs,ax

    .byte 0xBE                          # opcode of mov si, mem16
    .word BufferStartLocation
    .byte 0x66,0x8B,0x1C                # mov        ebx,dword ptr [si]

    .byte 0xBF                          # opcode of mov di, mem16
    .word PmodeOffsetLocation
    .byte 0x66,0x8B,0x5                 # mov        eax,dword ptr [di]
    .byte 0x8B,0xF8                     # mov        di, ax
    .byte 0x83,0xEF,0x6                 # sub        di, 06h
    .byte 0x66,0x3,0xC3                 # add        eax, ebx
    .byte 0x66,0x89,0x5                 # mov        dword ptr [di],eax

    .byte 0xBE                          # opcode of mov si, mem16
    .word GdtrLocation
    .byte 0x66                          # db         66h
    .byte 0x2E,0xF,0x1,0x14             # lgdt       fword ptr cs:[si]

    .byte 0xBE
    .word IdtrLocation
    .byte 0x66                          # db         66h
    .byte 0x2E,0xF,0x1,0x1C             # lidt       fword ptr cs:[si]

    .byte 0x33,0xC0                     # xor        ax,  ax
    .byte 0x8E,0xD8                     # mov        ds,  ax

    .byte 0xF,0x20,0xC0                 # mov        eax, cr0            ;Get control register 0
    .byte 0x66,0x83,0xC8,0x3            # or         eax, 000000003h     ;Set PE bit (bit #0) & MP
    .byte 0xF,0x22,0xC0                 # mov        cr0, eax

    .byte 0x66,0x67,0xEA                # far jump
    .long 0x0                           # 32-bit offset
    .word PROTECT_MODE_CS

Flat32Start:                            # protected mode entry point
    movw       $PROTECT_MODE_DS, %ax
    movw       %ax, %ds
    movw       %ax, %es
    movw       %ax, %fs
    movw       %ax, %gs
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
    movl       $NotVacantFlag, %eax

TestLock: 
    xchgl      %eax, (%edi)
    cmpl       $NotVacantFlag, %eax
    jz         TestLock

    movl       %esi, %edi
    addl       $ApIndexLocation, %edi #OVERIDE_HSD_5370234
    incl       (%edi)
    movl       (%edi), %ebx

ProgramStack: 
    movl       %esi, %edi
    addl       $StackSizeLocation, %edi
    movl       (%edi), %eax
    movl       %esi, %edi
    addl       $StackStartAddressLocation, %edi
    addl       (%edi), %eax
    movl       %eax, %esp
    movl       %eax, (%edi)

Releaselock: 
    movl       $VacantFlag, %eax
    movl       %esi, %edi
    addl       $LockLocation, %edi
    xchgl      %eax, (%edi)

CProcedureInvoke: 
    pushl      %ebp              # push BIST data at top of AP stack
    xorl       %ebp, %ebp        # clear ebp for call stack trace
    pushl      %ebp
    movl       %esp, %ebp

    movl       $InitializeFloatingPointUnits, %eax
    call       *%eax             # Call assembly function to initialize FPU per UEFI spec

    pushl      %ebx              # Push ApIndex OVERIDE_HSD_5370234
    movl       %esi, %eax
    addl       $LockLocation, %eax
    pushl      %eax              # push address of exchange info data buffer

    movl       %esi, %edi
    addl       $ApProcedureLocation, %edi
    movl       (%edi), %eax

    call       *%eax             # invoke C function

    jmp        .                 # never reach here
RendezvousFunnelProcEnd:

ASM_GLOBAL ASM_PFX(AsmCliHltLoop)
ASM_PFX(AsmCliHltLoop):
1:
    cli
    hlt
    jmp        1b

#-------------------------------------------------------------------------------------
#  AsmGetAddressMap (&AddressMap);
#-------------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
    pushal
    movl       %esp, %ebp

    movl       0x24(%ebp), %ebx

    movl       $RendezvousFunnelProcStart, (%ebx)
    movl       $(Flat32Start - RendezvousFunnelProcStart), 0x4(%ebx)
    movl       $0, 0x8(%ebx)
    movl       $(RendezvousFunnelProcEnd - RendezvousFunnelProcStart), 0x0c(%ebx)

    popal
    ret

.macro PAUSE32
    .byte   0xF3
    .byte   0x90
.endm

#-------------------------------------------------------------------------------------
#AsmExchangeRole procedure follows. This procedure executed by current BSP, that is
#about to become an AP. It switches it stack with the current AP.
#AsmExchangeRole (IN   CPU_EXCHANGE_INFO    *MyInfo, IN   CPU_EXCHANGE_INFO    *OthersInfo);
#-------------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmExchangeRole)
ASM_PFX(AsmExchangeRole):
    # DO NOT call other functions in this function, since 2 CPU may use 1 stack
    # at the same time. If 1 CPU try to call a function, stack will be corrupted.
    pushal
    movl       %esp, %ebp

    # esi contains MyInfo pointer
    movl       0x24(%ebp), %esi

    # edi contains OthersInfo pointer
    movl       0x28(%ebp), %edi

    #Store EFLAGS, GDTR and IDTR register to stack
    pushfl
    movl       %cr4, %eax
    pushl      %eax      # push cr4 firstly
    movl       %cr0, %eax
    pushl      %eax

    sgdt       8(%esi)  # sgdt       fword ptr [edi+8]
    sidt       14(%esi) # sidt       fword ptr [edi+14]

    # Store the its StackPointer
    movl       %esp, 4(%esi)

    # update its switch state to STORED
    movb       $CPU_SWITCH_STATE_STORED, (%esi)

WaitForOtherStored: 
    # wait until the other CPU finish storing its state
    cmpb       $CPU_SWITCH_STATE_STORED, (%edi)
    jz         OtherStored
    PAUSE32
    jmp        WaitForOtherStored

OtherStored: 
    # Since another CPU already stored its state, load them
    # load GDTR value
    lgdt       8(%edi)  # lgdt       fword ptr [edi+8]
    lidt       14(%edi) # lidt       fword ptr [edi+14]

    # load its future StackPointer
    movl       4(%edi), %esp

    # update the other CPU switch state to LOADED
    movb       $CPU_SWITCH_STATE_LOADED, (%edi)

WaitForOtherLoaded: 
    # wait until the other CPU finish loading new state,
    # otherwise the data in stack may corrupt
    cmpb       $CPU_SWITCH_STATE_LOADED, (%esi)
    jz         OtherLoaded
    PAUSE32
    jmp        WaitForOtherLoaded

OtherLoaded: 
    # since the other CPU already get the data it want, leave this procedure
    popl       %eax
    movl       %eax, %cr0
    popl       %eax
    movl       %eax, %cr4
    popfl

    popal
    ret

ASM_GLOBAL ASM_PFX(AsmInitializeGdt)
ASM_PFX(AsmInitializeGdt):
  pushl        %ebp
  movl         %esp, %ebp
  pushal
  movl         8(%ebp), %edi       # Load GDT register

  movw         %cs, %ax            # Get the selector data from our code image
  movw         %ax, %es
  lgdt         %es:(%edi)          # lgdt         FWORD PTR es:[edi]

  pushl        $PROTECT_MODE_CS
  leal         SetCodeSelectorFarJump, %eax
  pushl        %eax
  ret
SetCodeSelectorFarJump: 
  movw         $PROTECT_MODE_DS, %ax # Update the Base for the new selectors, too
  movw         %ax, %ds
  movw         %ax, %es
  movw         %ax, %fs
  movw         %ax, %gs
  movw         %ax, %ss

  popal
  popl         %ebp
  ret


