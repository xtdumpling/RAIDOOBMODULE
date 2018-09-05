;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;-------------------------------------------------------------------------------
;
; Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
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
;   MpFuncs.asm
;
; Abstract:
;
;   This is the assembly code for EM64T MP support for Cpu Hot-Add
;
;-------------------------------------------------------------------------------

; Make Sure to sync up these offsets with  CPU_INIT_INFO
%define BufferStartLocation RendezvousFunnelProcEnd - RendezvousFunnelProcStart
%define Cr3OffsetLocation BufferStartLocation + 0x4
%define GdtrLocation BufferStartLocation + 0x8
%define IdtrLocation BufferStartLocation + 0x12
%define MpExchangeInfoLocation BufferStartLocation + 0x20

; Make Sure to sync up these offsets with  HOT_ADD_CPU_EXCHANGE_INFO_OFFSET
%define LockLocation 0
%define StackStartAddressLocation 0x8
%define StackSizeLocation 0x10
%define CProcedureLocation 0x18
%define InitFlagLocation 0x20
%define SignalFlagLocation 0x24
%define PModeSignalFlagLocation 0x28
%define NumCpuCheckedInLocation 0x2C
%define BistBufferLocation 0x30
;-------------------------------------------------------------------------------------

;-------------------------------------------------------------------------------------
;RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
;procedure serializes all the AP processors through an Init sequence. It must be
;noted that APs arrive here very raw...ie: real mode, no stack.
;ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
;IS IN MACHINE CODE.
;-------------------------------------------------------------------------------------
;RendezvousFunnelProc (&WakeUpBuffer,MemAddress);

;text      SEGMENT
DEFAULT REL
SECTION .text

global ASM_PFX(RendezvousFunnelProc)
ASM_PFX(RendezvousFunnelProc):
RendezvousFunnelProcStart:

; At this point CS = 0x(vv00) and ip= 0x0.
        db 0x66,  0x8b, 0xe8           ; mov        ebp, eax
        db 0x8c,  0xc8                 ; mov        ax,  cs
        db 0x8e,  0xd8                 ; mov        ds,  ax
        db 0x8e,  0xc0                 ; mov        es,  ax
        db 0x8e,  0xd0                 ; mov        ss,  ax
        db 0x33,  0xc0                 ; xor        ax,  ax
        db 0x8e,  0xe0                 ; mov        fs,  ax
        db 0x8e,  0xe8                 ; mov        gs,  ax

;        db 0ebh, 0feh
; Switch to flat mode.

flat32Start:

        db 0xBE
        dw BufferStartLocation        ; mov        si, BufferStartLocation
        db 0x66,  0x8B, 0x14             ; mov        edx,dword ptr [si]          ; EDX is keeping the start address of wakeup buffer

        db 0xBE
        dw Cr3OffsetLocation          ; mov        si, Cr3Location
        db 0x66,  0x8B, 0xC             ; mov        ecx,dword ptr [si]          ; ECX is keeping the value of CR3

        db 0xBE
        dw GdtrLocation               ; mov        si, GdtrProfile
        db 0x66                        ; db         66h
        db 0x2E,  0xF, 0x1, 0x14        ; lgdt       fword ptr cs:[si]

        db 0xBE
        dw IdtrLocation               ; mov        si, IdtrProfile
        db 0x66                        ; db         66h
        db 0x2E,  0xF, 0x1, 0x1C        ; lidt       fword ptr cs:[si]

        db 0x31,  0xC0                 ; xor        ax,  ax
        db 0x8E,  0xD8                 ; mov        ds,  ax

        db 0xF,  0x20, 0xC0            ; mov        eax, cr0                    ; Get control register 0
        db 0x66,  0x83, 0xC8, 0x3       ; or         eax, 000000003h             ; Set PE bit (bit #0) and MP
        db 0xF,  0x22, 0xC0            ; mov        cr0, eax

FLAT32_JUMP:

        db 0x66,  0x67, 0xEA            ; far jump
        dd 0x0                         ; 32-bit offset
        dw 0x20                        ; 16-bit selector

ProtModeStart:                       ; protected mode entry point

        db 0x66,  0xB8, 0x18,  0x0      ; mov        ax,  18h
        db 0x66,  0x8E,  0xD8           ; mov        ds,  ax
        db 0x66,  0x8E,  0xC0           ; mov        es,  ax
        db 0x66,  0x8E,  0xE0           ; mov        fs,  ax
        db 0x66,  0x8E,  0xE8           ; mov        gs,  ax
        db 0x66,  0x8E,  0xD0           ; mov        ss,  ax                     ; Flat mode setup.

        db 0xF,  0x20,  0xE0           ; mov        eax, cr4
        db 0x66,  0xD,  0x20, 0x6      ; or         ax,  0620h
        db 0xF,  0x22,  0xE0           ; mov        cr4, eax

        db 0xF,  0x22,  0xD9           ; mov        cr3, ecx

        db 0x8B,  0xF2                 ; mov        esi, edx                    ; Save wakeup buffer address

        db 0xB9
        dd 0xC0000080                 ; mov        ecx, 0c0000080h             ; EFER MSR number.
        db 0xF,  0x32                  ; rdmsr                                  ; Read EFER.
        db 0xF,  0xBA, 0xE8, 0x8      ; bts        eax, 8                      ; Set LME=1.
        db 0xF,  0x30                  ; wrmsr                                  ; Write EFER.

        db 0xF,  0x20,  0xC0           ; mov        eax, cr0                    ; Read CR0.
        db 0xF,  0xBA, 0xE8, 0x1F      ; bts        eax, 31                     ; Set PG=1.
        db 0xF,  0x22,  0xC0           ; mov        cr0, eax                    ; Write CR0.

LONG_JUMP:

        db 0x67,  0xEA                 ; far jump
        dd 0x0                         ; 32-bit offset
        dw 0x38                        ; 16-bit selector

LongModeStart:

        mov         ax,  0x30
        mov         ds,  ax
        mov         es,  ax
        mov         ss,  ax

        ;
        ; Get pointer to MpExchangeInfo Structure
        ;
        mov         edi, esi
        add         edi, MpExchangeInfoLocation
        mov         edi, dword [edi]
        xchg        esi, edi                     ; esi - MpExchangeInfoPtr; edi - Buffer Start

        ; get ApicId
        mov        eax, 0
        cpuid
        cmp        eax, 0xb
        jnb        X2Apic                        ; br, if proc capable of 32bit apicid

        mov        eax, 1
        cpuid
        shr        ebx, 24
        and        ebx, 0xff                     ; EBX is APIC ID
        jmp        checkInitFlag
X2Apic:
        mov        eax, 0xb                      ;get apicid from cpuid_edx(0xb,0)
        xor        ecx, ecx
        cpuid
        mov        ebx, edx                      ; EBX has Apic ID

checkInitFlag:
        ;
        ; Check InitFlag for command
        ;
        cmp         dword [esi + InitFlagLocation], 0
        jz          CallApFunc

        cmp         dword [esi + InitFlagLocation], 1
        jz          RecordBist

        cmp         dword [esi + InitFlagLocation], 2
        jz          CheckInandHlt

        cmp         dword [esi + InitFlagLocation], 3
        jz          RecordBist

SkipFlag:
        wbinvd
        jmp        GoToSleep

CheckInandHlt:
        inc        dword [esi + NumCpuCheckedInLocation]
        jmp        SkipFlag

RecordBist:
        mov        dword [esi + BistBufferLocation + (8* ebx)], 1       ; Valid Flag
        mov        dword [esi + BistBufferLocation + (8* ebx) + 4], ebx ; BIST

        ; Now wait till the BSP processes the BIST info and assigns unique CPU number to each thread.

Wait4Signal:
        pause
        cmp        dword [esi + SignalFlagLocation], 0
        jz         Wait4Signal

CallApFunc:
        ;
        ; ProgramStack
        ;
        xor         rcx, rcx
        mov         ecx, dword [esi + BistBufferLocation + (8 * ebx)]    ; RCX = CpuNumber
        inc         rcx
        mov         rax, qword [esi + StackSizeLocation]
        mul         rcx                               ; RAX = StackSize * (CpuNumber + 1)

        mov         rbx, qword [esi + StackStartAddressLocation]
        add         rax, rbx                          ; RAX = StackStart + StackSize * (CpuNumber + 1)

        sub         rax, 0x10
        mov         rsp, rax
        ;
        ; Call C Function
        ;
        mov         rax, qword [esi + CProcedureLocation]

        test        rax, rax
        jz          GoToSleep
        push        rsi
        sub         rsp, 0x20
        call        rax
        add         rsp, 0x20
        pop         rsi

Wait4PMSignal:
        pause
        cmp        dword [esi + PModeSignalFlagLocation], 0
        jz         Wait4PMSignal

        wbinvd

GoToSleep:

        cli
        hlt
        jmp         $-2

RendezvousFunnelProcEnd:

;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
;typedef struct {
;  UINT8 *RendezvousFunnelAddress;
;  UINTN PModeEntryOffset;
;  UINTN FlatJumpOffset;
;  UINTN LModeEntryOffset;
;  UINTN LongJumpOffset;
;  UINTN Size;
;} MP_ASSEMBLY_ADDRESS_MAP;
global ASM_PFX(AsmGetHotAddCodeAddressMap)
ASM_PFX(AsmGetHotAddCodeAddressMap):

        mov         rax, RendezvousFunnelProcStart
        mov         qword [rcx], rax
        mov         qword [rcx+0x8], ProtModeStart - RendezvousFunnelProcStart
        mov         qword [rcx+0x10], FLAT32_JUMP - RendezvousFunnelProcStart
        mov         qword [rcx+0x18], LongModeStart - RendezvousFunnelProcStart
        mov         qword [rcx+0x20], LONG_JUMP - RendezvousFunnelProcStart
        mov         qword [rcx+0x28], RendezvousFunnelProcEnd - RendezvousFunnelProcStart

        ret

