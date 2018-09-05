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

    .686p
    .model  flat,C
    .xmm

DSC_OFFSET    EQU     0fb00h
DSC_GDTPTR    EQU     30h
DSC_GDTSIZ    EQU     38h
DSC_CS        EQU     14
DSC_DS        EQU     16
DSC_SS        EQU     18
DSC_OTHERSEG  EQU     20
MSR_DR6       EQU     0c05h
MSR_DR7       EQU     0c06h

PROTECT_MODE_CS EQU   08h
PROTECT_MODE_DS EQU   20h
TSS_SEGMENT     EQU   40h

SmiRendezvous   PROTO   C

EXTERNDEF   gcSmiHandlerTemplate:BYTE
EXTERNDEF   gcSmiHandlerSize:WORD
EXTERNDEF   gSmiCr3:DWORD
EXTERNDEF   gSmiStack:DWORD
EXTERNDEF   gSmbase:DWORD
EXTERNDEF   mSaveStateInMsr:BYTE
EXTERNDEF   FeaturePcdGet (PcdCpuSmmDebug):BYTE
EXTERNDEF   FeaturePcdGet (PcdCpuSmmStackGuard):BYTE
EXTERNDEF   gcSmiPMHandlerTemplate:BYTE
EXTERNDEF   gcSmiPMHandlerSize:WORD
EXTERNDEF   gProtModeSmbase:DWORD
EXTERNDEF   gcSmiIdtr:DWORD

    .code

gcSmiHandlerTemplate    LABEL   BYTE

_SmiEntryPoint:
    DB      0bbh                        ; mov bx, imm16
    DW      offset _GdtDesc - _SmiEntryPoint + 8000h
    DB      2eh, 0a1h                   ; mov ax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTSIZ
    dec     eax
    mov     cs:[edi], eax               ; mov cs:[bx], ax
    DB      66h, 2eh, 0a1h              ; mov eax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTPTR
    mov     cs:[edi + 2], ax            ; mov cs:[bx + 2], eax
    mov     bp, ax                      ; ebp = GDT base
    DB      66h
    lgdt    fword ptr cs:[edi]          ; lgdt fword ptr cs:[bx]
; Patch ProtectedMode Segment
    DB      0b8h                        ; mov ax, imm16
    DW      PROTECT_MODE_CS             ; set AX for segment directly
    mov     cs:[edi - 2], eax           ; mov cs:[bx - 2], ax
; Patch ProtectedMode entry
    DB      66h, 0bfh                   ; mov edi, SMBASE
gSmbase    DD    ?
    DB      67h
    lea     ax, [edi + (@32bit - _SmiEntryPoint) + 8000h]
    mov     cs:[edi - 6], ax            ; mov cs:[bx - 6], eax
    mov     ebx, cr0
    DB      66h
    and     ebx, 9ffafff3h
    DB      66h
    or      ebx, 23h
    mov     cr0, ebx
    DB      66h, 0eah
    DD      ?
    DW      ?
_GdtDesc    FWORD   ?

@32bit:
    mov     ax, PROTECT_MODE_DS
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    DB      0bch                   ; mov esp, imm32
gSmiStack   DD      ?
    mov     eax, offset gcSmiIdtr
    lidt    fword ptr [eax]
    jmp     ProtFlatMode

gcSmiPMHandlerTemplate    LABEL   BYTE
_SmiPMEntryPoint:
; if SMM PROT MODE feature is ok, processor will break here with 32bit protected mode
    DB      0bfh                        ; mov edi, SMBASE
gProtModeSmbase    DD    ?
    mov     eax, PROTECT_MODE_DS
    push    eax
    mov     eax, esp
    add     eax, edi
    add     eax, 4
    push    eax
    sub     eax, 8
    lss     esp, fword ptr [eax]

    mov     eax, edi
    add     eax, ProtFlatMode - _SmiPMEntryPoint + 8000h
    mov     dx, PROTECT_MODE_CS
    mov     [eax - 2], dx               ; mov cs:[bx - 6], eax
    mov     [eax - 6], eax              ; mov cs:[bx - 6], eax

    DB      0eah                        ; jmp @ProtFlatMode
    DD      ?
    DW      ?

ProtFlatMode:
    DB      0b8h                        ; mov eax, imm32
gSmiCr3     DD      ?
    mov     cr3, eax
    mov     eax, 668h                   ; as cr4.PGE is not set here, refresh cr3
    mov     cr4, eax                    ; in PreModifyMtrrs() to flush TLB.
    mov     ebx, cr0
    or      ebx, 080000000h             ; enable paging
;; PURLEY_OVERRIDE_BEGIN_5333001
    or      ebx, 00000023h              ; set CR0.NE
;; PURLEY_OVERRIDE_END_5333001
    mov     cr0, ebx
    lea     ebx, [edi + DSC_OFFSET]
    mov     ax, [ebx + DSC_DS]
    mov     ds, eax
    mov     ax, [ebx + DSC_OTHERSEG]
    mov     es, eax
    mov     fs, eax
    mov     gs, eax
    mov     ax, [ebx + DSC_SS]
    mov     ss, eax

    cmp     FeaturePcdGet (PcdCpuSmmStackGuard), 0
    jz      @F

; Load TSS
    mov     byte ptr [ebp + TSS_SEGMENT + 5], 89h ; clear busy flag
    mov     eax, TSS_SEGMENT
    ltr     ax
@@:
;   jmp     _SmiHandler                 ; instruction is not needed

_SmiHandler PROC
    cmp     FeaturePcdGet (PcdCpuSmmDebug), 0
    jz      @3
    cmp     mSaveStateInMsr, 0
    jz      @F
    mov     ecx, MSR_DR6
    rdmsr
    push    eax
    mov     ecx, MSR_DR7
    rdmsr
    mov     edx, eax
    pop     ecx
    jmp     @5

@@:
    call    @1
@1:
    pop     ebp
    mov     eax, 80000001h
    cpuid
    bt      edx, 29                     ; check cpuid to identify X64 or IA32 
    lea     edi, [ebp - (@1 - _SmiEntryPoint) + 7fc8h]
    lea     esi, [edi + 4]
    jnc     @2
    add     esi, 4
@2:
    mov     ecx, [esi]
    mov     edx, [edi]
@5:
    mov     dr6, ecx
    mov     dr7, edx                    ; restore DR6 & DR7 before running C code
@3:
    mov     ecx, [esp]                  ; CPU Index
    
    push    ecx
    mov     eax, SmiRendezvous
    call    eax
    pop     ecx

    cmp     FeaturePcdGet (PcdCpuSmmDebug), 0
    jz      @4

    cmp     mSaveStateInMsr, 0
    jnz     @4                          ; DR6/DR7 MSR is RO

    mov     ecx, dr6
    mov     edx, dr7
    mov     [esi], ecx
    mov     [edi], edx
@4:
    rsm
_SmiHandler ENDP

gcSmiHandlerSize    DW      $ - _SmiEntryPoint
gcSmiPMHandlerSize  DW      $ - _SmiPMEntryPoint

    END
