;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2009 - 2014, Intel Corporation. All rights reserved.<BR>
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
;   SmmInit.Asm
;
; Abstract:
;
;   Functions for relocating SMBASE's for all processors
;
; Notes:
;
;------------------------------------------------------------------------------

    .686p
    .xmm
    .model  flat,C

SmmInitHandler  PROTO   C

EXTERNDEF   C   gSmmCr0:DWORD
EXTERNDEF   C   gSmmCr3:DWORD
EXTERNDEF   C   gSmmCr4:DWORD
EXTERNDEF   C   gcSmmInitTemplate:BYTE
EXTERNDEF   C   gcSmmInitSize:WORD
EXTERNDEF   C   gSmmJmpAddr:QWORD
EXTERNDEF   C   mRebasedFlag:PTR BYTE
EXTERNDEF   C   mSmmRelocationOriginalAddress:DWORD
EXTERNDEF   C   gSmmInitStack:DWORD
EXTERNDEF   C   gcSmiInitGdtr:FWORD

PROTECT_MODE_CS EQU   08h
PROTECT_MODE_DS EQU   20h

    .code

gcSmiInitGdtr   LABEL   FWORD
            DW      0
            DQ      0

SmmStartup  PROC
    DB      66h, 0b8h
gSmmCr3     DD      ?
    mov     cr3, eax
    DB      67h, 66h
    lgdt    fword ptr cs:[ebp + (offset gcSmiInitGdtr - SmmStartup)]
    DB      66h, 0b8h
gSmmCr4     DD      ?
    mov     cr4, eax
    DB      66h, 0b8h
gSmmCr0     DD      ?
    DB      0bfh, PROTECT_MODE_DS, 0    ; mov di, PROTECT_MODE_DS
    mov     cr0, eax
    DB      66h, 0eah                   ; jmp far [ptr48]
gSmmJmpAddr LABEL   QWORD
    DD      @32bit
    DW      PROTECT_MODE_CS
@32bit:
    mov     ds, edi
    mov     es, edi
    mov     fs, edi
    mov     gs, edi
    mov     ss, edi
    DB      0bch                        ; mov esp, imm32
gSmmInitStack  DD ?
    call    SmmInitHandler
    rsm
SmmStartup  ENDP

gcSmmInitTemplate   LABEL   BYTE

_SmmInitTemplate    PROC
    DB      66h
    mov     ebp, SmmStartup
    DB      66h, 81h, 0edh, 00h, 00h, 03h, 00  ; sub ebp, 30000h
    jmp     bp                          ; jmp ebp actually
_SmmInitTemplate    ENDP

gcSmmInitSize   DW  $ - gcSmmInitTemplate

SmmRelocationSemaphoreComplete PROC
    push    eax
    mov     eax, mRebasedFlag
    mov     byte ptr [eax], 1
    pop     eax
    jmp     [mSmmRelocationOriginalAddress]
SmmRelocationSemaphoreComplete ENDP
    END
