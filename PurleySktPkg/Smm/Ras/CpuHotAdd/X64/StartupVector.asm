;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;-------------------------------------------------------------------------------
;
; Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
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
BufferStartLocation           equ        RendezvousFunnelProcEnd - RendezvousFunnelProcStart
Cr3OffsetLocation             equ        BufferStartLocation + 04h
GdtrLocation                  equ        BufferStartLocation + 08h
IdtrLocation                  equ        BufferStartLocation + 12h
MpExchangeInfoLocation        equ        BufferStartLocation + 20h

; Make Sure to sync up these offsets with  HOT_ADD_CPU_EXCHANGE_INFO_OFFSET
LockLocation                  equ        0
StackStartAddressLocation     equ        08h
StackSizeLocation             equ        10h
CProcedureLocation            equ        18h
InitFlagLocation              equ        20h
SignalFlagLocation            equ        24h
PModeSignalFlagLocation       equ        28h
NumCpuCheckedInLocation       equ        2Ch
BistBufferLocation            equ        30h
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
.code

RendezvousFunnelProc   PROC  
RendezvousFunnelProcStart::

; At this point CS = 0x(vv00) and ip= 0x0.
        db 66h,  08bh, 0e8h           ; mov        ebp, eax
        db 8ch,  0c8h                 ; mov        ax,  cs
        db 8eh,  0d8h                 ; mov        ds,  ax
        db 8eh,  0c0h                 ; mov        es,  ax
        db 8eh,  0d0h                 ; mov        ss,  ax 
        db 33h,  0c0h                 ; xor        ax,  ax
        db 8eh,  0e0h                 ; mov        fs,  ax
        db 8eh,  0e8h                 ; mov        gs,  ax
      
;        db 0ebh, 0feh 
; Switch to flat mode.

flat32Start::

        db 0BEh
        dw BufferStartLocation        ; mov        si, BufferStartLocation
        db 66h,  8Bh, 14h             ; mov        edx,dword ptr [si]          ; EDX is keeping the start address of wakeup buffer

        db 0BEh
        dw Cr3OffsetLocation          ; mov        si, Cr3Location
        db 66h,  8Bh, 0Ch             ; mov        ecx,dword ptr [si]          ; ECX is keeping the value of CR3
        
        db 0BEh
        dw GdtrLocation               ; mov        si, GdtrProfile
        db 66h                        ; db         66h
        db 2Eh,  0Fh, 01h, 14h        ; lgdt       fword ptr cs:[si]

        db 0BEh
        dw IdtrLocation               ; mov        si, IdtrProfile
        db 66h                        ; db         66h
        db 2Eh,  0Fh, 01h, 1Ch        ; lidt       fword ptr cs:[si]
        
        db 31h,  0C0h                 ; xor        ax,  ax
        db 8Eh,  0D8h                 ; mov        ds,  ax
        
        db 0Fh,  20h, 0C0h            ; mov        eax, cr0                    ; Get control register 0
        db 66h,  83h, 0C8h, 03h       ; or         eax, 000000003h             ; Set PE bit (bit #0) and MP
        db 0Fh,  22h, 0C0h            ; mov        cr0, eax

FLAT32_JUMP::

        db 66h,  67h, 0EAh            ; far jump
        dd 0h                         ; 32-bit offset
        dw 20h                        ; 16-bit selector

ProtModeStart::                       ; protected mode entry point

        db 66h,  0B8h, 18h,  00h      ; mov        ax,  18h
        db 66h,  8Eh,  0D8h           ; mov        ds,  ax
        db 66h,  8Eh,  0C0h           ; mov        es,  ax
        db 66h,  8Eh,  0E0h           ; mov        fs,  ax
        db 66h,  8Eh,  0E8h           ; mov        gs,  ax
        db 66h,  8Eh,  0D0h           ; mov        ss,  ax                     ; Flat mode setup.

        db 0Fh,  20h,  0E0h           ; mov        eax, cr4
        db 66h,  0Dh,  020h, 06h      ; or         ax,  0620h
        db 0Fh,  22h,  0E0h           ; mov        cr4, eax

        db 0Fh,  22h,  0D9h           ; mov        cr3, ecx
        
        db 8Bh,  0F2h                 ; mov        esi, edx                    ; Save wakeup buffer address

        db 0B9h
        dd 0C0000080h                 ; mov        ecx, 0c0000080h             ; EFER MSR number.
        db 0Fh,  32h                  ; rdmsr                                  ; Read EFER.
        db 0Fh,  0BAh, 0E8h, 08h      ; bts        eax, 8                      ; Set LME=1.
        db 0Fh,  30h                  ; wrmsr                                  ; Write EFER.
        
        db 0Fh,  20h,  0C0h           ; mov        eax, cr0                    ; Read CR0.
        db 0Fh,  0BAh, 0E8h, 1Fh      ; bts        eax, 31                     ; Set PG=1.
        db 0Fh,  22h,  0C0h           ; mov        cr0, eax                    ; Write CR0.

LONG_JUMP::
        
        db 67h,  0EAh                 ; far jump
        dd 0h                         ; 32-bit offset
        dw 38h                        ; 16-bit selector
        
LongModeStart::

        mov         ax,  30h
        mov         ds,  ax
        mov         es,  ax
        mov         ss,  ax

        ; 
        ; Get pointer to MpExchangeInfo Structure
        ;
        mov         edi, esi
        add         edi, MpExchangeInfoLocation
        mov         edi, dword ptr [edi]
        xchg        esi, edi                     ; esi - MpExchangeInfoPtr; edi - Buffer Start

        ; get ApicId
        mov        eax, 0                     
        cpuid
        cmp        eax, 0bh                             
        jnb        X2Apic                        ; br, if proc capable of 32bit apicid

        mov        eax, 1
        cpuid
        shr        ebx, 24
        and        ebx, 0ffh                     ; EBX is APIC ID
        jmp        checkInitFlag
X2Apic:
        mov        eax, 0bh                      ;get apicid from cpuid_edx(0xb,0)
        xor        ecx, ecx
        cpuid
        mov        ebx, edx                      ; EBX has Apic ID
               
checkInitFlag:
        ; 
        ; Check InitFlag for command
        ;
        cmp         dword ptr [esi + InitFlagLocation], 0
        jz          CallApFunc
	
        cmp         dword ptr [esi + InitFlagLocation], 1
        jz          RecordBist
	
        cmp         dword ptr [esi + InitFlagLocation], 2
        jz          CheckInandHlt

        cmp         dword ptr [esi + InitFlagLocation], 3
        jz          RecordBist

SkipFlag::
        wbinvd
        jmp        GoToSleep

CheckInandHlt::
        inc        dword ptr [esi + NumCpuCheckedInLocation]
        jmp        SkipFlag

RecordBist::
        mov        dword ptr [esi + BistBufferLocation + (8* ebx)], 1       ; Valid Flag
        mov        dword ptr [esi + BistBufferLocation + (8* ebx) + 4], ebx ; BIST

        ; Now wait till the BSP processes the BIST info and assigns unique CPU number to each thread.

Wait4Signal::
        pause
        cmp        dword ptr [esi + SignalFlagLocation], 0
        jz         Wait4Signal

CallApFunc::
        ; 
        ; ProgramStack
        ;
        xor         rcx, rcx
        mov         ecx, dword ptr [esi + BistBufferLocation + (8 * ebx)]    ; RCX = CpuNumber
        inc         rcx
        mov         rax, qword ptr [esi + StackSizeLocation]
        mul         rcx                               ; RAX = StackSize * (CpuNumber + 1)
        
        mov         rbx, qword ptr [esi + StackStartAddressLocation]
        add         rax, rbx                          ; RAX = StackStart + StackSize * (CpuNumber + 1)
        
        sub         rax, 10h
        mov         rsp, rax
        ;
        ; Call C Function
        ;
        mov         rax, qword ptr [esi + CProcedureLocation]

        test        rax, rax
        jz          GoToSleep
        push        rsi
        sub         rsp, 20h
        call        rax
        add         rsp, 20h
        pop         rsi
        
Wait4PMSignal::
        pause
        cmp        dword ptr [esi + PModeSignalFlagLocation], 0
        jz         Wait4PMSignal
        
        wbinvd

GoToSleep::

        cli
        hlt
        jmp         $-2
        
RendezvousFunnelProc   ENDP
RendezvousFunnelProcEnd::


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
AsmGetHotAddCodeAddressMap   PROC  PUBLIC

        mov         rax, offset RendezvousFunnelProcStart
        mov         qword ptr [rcx], rax
        mov         qword ptr [rcx+8h], ProtModeStart - RendezvousFunnelProcStart
        mov         qword ptr [rcx+10h], FLAT32_JUMP - RendezvousFunnelProcStart
        mov         qword ptr [rcx+18h], LongModeStart - RendezvousFunnelProcStart
        mov         qword ptr [rcx+20h], LONG_JUMP - RendezvousFunnelProcStart
        mov         qword ptr [rcx+28h], RendezvousFunnelProcEnd - RendezvousFunnelProcStart
        
        ret
        
AsmGetHotAddCodeAddressMap   ENDP



END
