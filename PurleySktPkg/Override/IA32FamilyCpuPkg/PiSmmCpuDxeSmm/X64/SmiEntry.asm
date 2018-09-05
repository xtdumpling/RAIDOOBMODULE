;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;   SmiEntry.asm
;
; Abstract:
;
;   Code template of the SMI handler for a particular processor
;
;------------------------------------------------------------------------------

;
; Variables referrenced by C code
;
EXTERNDEF   SmiRendezvous:PROC
EXTERNDEF   gcSmiHandlerTemplate:BYTE
EXTERNDEF   gcSmiHandlerSize:WORD
EXTERNDEF   gSmiCr3:DWORD
EXTERNDEF   gSmiStack:DWORD
EXTERNDEF   gSmbase:DWORD
EXTERNDEF   mSaveStateInMsr:BYTE
EXTERNDEF   FeaturePcdGet (PcdCpuSmmDebug):BYTE
EXTERNDEF   gcSmiPMHandlerTemplate:BYTE
EXTERNDEF   gcSmiPMHandlerSize:WORD
EXTERNDEF   gProtModeSmbase:DWORD
EXTERNDEF   gcSmiIdtr:FWORD
EXTERNDEF   mSmmProtectedModeEnable:BYTE

;
; Constants relating to PROCESSOR_SMM_DESCRIPTOR
;
DSC_OFFSET    EQU     0fb00h
DSC_GDTPTR    EQU     30h
DSC_GDTSIZ    EQU     38h
DSC_CS        EQU     14
DSC_DS        EQU     16
DSC_SS        EQU     18
DSC_OTHERSEG  EQU     20
MSR_DR6       EQU     0c05h
MSR_DR7       EQU     0c06h
;
; Constants relating to CPU State Save Area
;
SSM_DR6         EQU     0ffd0h
SSM_DR7         EQU     0ffc8h

PROTECT_MODE_CS EQU     08h
PROTECT_MODE_DS EQU     20h
LONG_MODE_CS    EQU     38h
TSS_SEGMENT     EQU     40h
GDT_SIZE        EQU     50h

    .code

gcSmiHandlerTemplate    LABEL   BYTE

_SmiEntryPoint:
    ;
    ; The encoding of BX in 16-bit addressing mode is the same as of RDI in 64-
    ; bit addressing mode. And that coincidence has been used in the following
    ; "64-bit like" 16-bit code. Be aware that once RDI is referrenced as a
    ; base address register, it is actually BX that is referrenced.
    ;
    DB      0bbh                        ; mov bx, imm16
    DW      offset _GdtDesc - _SmiEntryPoint + 8000h  ; bx = GdtDesc offset
; fix GDT descriptor
    DB      2eh, 0a1h                   ; mov ax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTSIZ
    DB      48h                         ; dec ax
    DB      2eh
    mov     [rdi], eax                  ; mov cs:[bx], ax
    DB      66h, 2eh, 0a1h              ; mov eax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTPTR
    DB      2eh
    mov     [rdi + 2], ax               ; mov cs:[bx + 2], eax
    DB      66h, 2eh
    lgdt    fword ptr [rdi]             ; lgdt fword ptr cs:[bx]
; Patch ProtectedMode Segment
    DB      0b8h                        ; mov ax, imm16
    DW      PROTECT_MODE_CS             ; set AX for segment directly
    DB      2eh
    mov     [rdi - 2], eax              ; mov cs:[bx - 2], ax
; Patch ProtectedMode entry
    DB      66h, 0bfh                   ; mov edi, SMBASE
gSmbase    DD    ?
    lea     ax, [edi + (@ProtectedMode - _SmiEntryPoint) + 8000h]
    DB      2eh
    mov     [rdi - 6], ax               ; mov cs:[bx - 6], eax
; Switch into @ProtectedMode
    mov     rbx, cr0
    DB      66h
    and     ebx, 9ffafff3h
    DB      66h
    or      ebx, 00000023h

    mov     cr0, rbx
    DB      66h, 0eah
    DD      ?
    DW      ?

_GdtDesc    FWORD   ?
@ProtectedMode:
    mov     ax, PROTECT_MODE_DS
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    DB      0bch                   ; mov esp, imm32
gSmiStack   DD      ?
    jmp     ProtFlatMode

gcSmiPMHandlerTemplate    LABEL   BYTE
_SmiPMEntryPoint:
; if SMM PROT MODE feature is ok, processor will break here with 32bit protected mode
    DB      0bfh                        ; mov edi, SMBASE
gProtModeSmbase    DD    ?
    mov     eax, PROTECT_MODE_DS
    push    rax
    mov     eax, esp
    add     eax, edi
    add     eax, 4
    push    rax
    sub     eax, 8
    lss     esp, fword ptr [rax]

    mov     eax, edi
    add     eax, ProtFlatMode - _SmiPMEntryPoint + 8000h
    mov     dx, PROTECT_MODE_CS
    mov     [rax - 2], dx               ; mov cs:[bx - 6], eax
    mov     [rax - 6], eax              ; mov cs:[bx - 6], eax

    DB      0eah                        ; jmp @ProtFlatMode
    DD      ?
    DW      ?

ProtFlatMode:
    DB      0b8h                        ; mov eax, offset gSmiCr3
gSmiCr3     DD      ?
    mov     cr3, rax
    mov     eax, 668h                   ; as cr4.PGE is not set here, refresh cr3
    mov     cr4, rax                    ; in PreModifyMtrrs() to flush TLB.
; Load TSS
    sub     esp, 8                      ; reserve room in stack
    sgdt    fword ptr [rsp]
    mov     eax, [rsp + 2]              ; eax = GDT base
    add     esp, 8
    mov     edx, eax
    add     edx, GDT_SIZE
    mov     [rax + TSS_SEGMENT + 2], dl
    mov     [rax + TSS_SEGMENT + 3], dh
    DB      0c1h, 0eah, 10h             ; shr     edx, 16
    mov     [rax + TSS_SEGMENT + 4], dl
    mov     [rax + TSS_SEGMENT + 7], dh
    mov     edx, eax
    mov     dl, 89h
    mov     [rax + TSS_SEGMENT + 5], dl ; clear busy flag
    mov     eax, TSS_SEGMENT
    ltr     ax

; Switch into @LongMode
    push    LONG_MODE_CS                ; push cs hardcore here
    call    Base                       ; push reture address for retf later
Base:
    add     dword ptr [rsp], @LongMode - Base; offset for far retf, seg is the 1st arg
    mov     ecx, 0c0000080h
    rdmsr
    or      ah, 1
    wrmsr
    mov     rbx, cr0
    bts     ebx, 31
;; PURLEY_OVERRIDE_BEGIN_5333001
    or      ebx, 00000023h              ; set CR0.NE
;; PURLEY_OVERRIDE_END_5333001
    mov     cr0, rbx
    retf
@LongMode:                              ; long mode (64-bit code) starts here
    mov     rax, offset gcSmiIdtr
    lidt    fword ptr [rax]
    lea     ebx, [rdi + DSC_OFFSET]
    mov     ax, [rbx + DSC_DS]
    mov     ds, eax
    mov     ax, [rbx + DSC_OTHERSEG]
    mov     es, eax
    mov     fs, eax
    mov     gs, eax
    mov     ax, [rbx + DSC_SS]
    mov     ss, eax
;   jmp     _SmiHandler                 ; instruction is not needed

_SmiHandler:
;
; The following lines restore DR6 & DR7 before running C code. They are useful
; when you want to enable hardware breakpoints in SMM without setting
; smmentrybreak=true in ITP.
;
; NOTE: These lines might not be appreciated in runtime since they might
;       conflict with OS debugging facilities. Turn them off in RELEASE.
;
    mov     rax, offset FeaturePcdGet (PcdCpuSmmDebug) ;Get absolute address. Avoid RIP relative addressing
    cmp     byte ptr [rax], 0
    jz      @1

    mov     rax, offset mSaveStateInMsr
    cmp     byte ptr [rax], 0
    jz      @F
    mov     ecx, MSR_DR6
    rdmsr
    shl     rax, 32
    shld    rdx, rax, 32
    push    rdx
    mov     ecx, MSR_DR7
    rdmsr
    shl     rax, 32
    shld    rdx, rax, 32
    pop     rcx
    jmp     @3

@@:
    mov     rax, offset mSmmProtectedModeEnable
    cmp     byte ptr [rax], 1
    jz      @4  
    DB      48h, 8bh, 0dh               ; mov rcx, [rip + disp32]
    DD      SSM_DR6 - ($ + 4 - _SmiEntryPoint + 8000h)
    DB      48h, 8bh, 15h               ; mov rdx, [rip + disp32]
    DD      SSM_DR7 - ($ + 4 - _SmiEntryPoint + 8000h)
    jmp     @3
@4:
    DB      48h, 8bh, 0dh               ; mov rcx, [rip + disp32]
    DD      SSM_DR6 - ($ + 4 - _SmiPMEntryPoint + 8000h)
    DB      48h, 8bh, 15h               ; mov rdx, [rip + disp32]
    DD      SSM_DR7 - ($ + 4 - _SmiPMEntryPoint + 8000h)
@3:
    mov     dr6, rcx
    mov     dr7, rdx
@1:
    mov     rcx, [rsp]                  ; rcx <- CpuIndex
    mov     rax, SmiRendezvous          ; rax <- absolute addr of SmiRedezvous

    ;
    ; Save FP registers
    ;
    sub     rsp, 208h
    DB      48h                         ; FXSAVE64
    fxsave  [rsp]
    
    add     rsp, -20h
    call    rax
    add     rsp, 20h

    ;
    ; Restore FP registers
    ;
    DB      48h                         ; FXRSTOR64
    fxrstor [rsp]  
 
    mov     rax, offset FeaturePcdGet (PcdCpuSmmDebug) ;Get absolute address. Avoid RIP relative addressing
    cmp     byte ptr [rax], 0
    jz      @2

    mov     rax, offset mSaveStateInMsr
    cmp     byte ptr [rax], 0
    jnz     @2                          ; DR6/DR7 MSR is RO

    mov     rdx, dr7
    mov     rcx, dr6
    
    mov     rax, offset mSmmProtectedModeEnable
    cmp     byte ptr [rax], 1
    jz      @5 
    DB      48h, 89h, 15h               ; mov [rip + disp32], rdx
    DD      SSM_DR7 - ($ + 4 - _SmiEntryPoint + 8000h)
    DB      48h, 89h, 0dh               ; mov [rip + disp32], rcx
    DD      SSM_DR6 - ($ + 4 - _SmiEntryPoint + 8000h)
    jmp     @2
@5:
    DB      48h, 89h, 15h               ; mov [rip + disp32], rdx
    DD      SSM_DR7 - ($ + 4 - _SmiPMEntryPoint + 8000h)
    DB      48h, 89h, 0dh               ; mov [rip + disp32], rcx
    DD      SSM_DR6 - ($ + 4 - _SmiPMEntryPoint + 8000h)
@2:
    rsm

gcSmiHandlerSize    DW      $ - _SmiEntryPoint
gcSmiPMHandlerSize  DW      $ - _SmiPMEntryPoint

    END
