;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
  page    ,132
  title   CPU ARCHITECTURAL DXE PROTOCOL ASSEMBLY HOOKS
;-------------------------------------------------------------------------------
;
; Copyright (c) 2006 - 2013, Intel Corporation. All rights reserved.<BR>
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
;   CpuAsm.asm
; 
; Abstract:
; 
;
;-------------------------------------------------------------------------------


text  SEGMENT

;
;
;
;------------------------------------------------------------------------------
;  Generic IDT Vector Handlers for the Host. They are all the same so they
;  will compress really well.
;
;  By knowing the return address for Vector 00 you can can calculate the
;  vector number by looking at the call CommonInterruptEntry return address.
;  (return address - AsmIdtVector00Base)/8 == IDT index
;
;------------------------------------------------------------------------------

ALIGN   8

CODE_SELECTOR EQU 38h
DATA_SELECTOR EQU 30h

InitializeGdt  proc 

  mov     rdx, OFFSET Gdtr    ; Load GDT register

  mov     ax,cs               ; Get the selector data from our code image          
  mov     ds,ax
  lgdt    FWORD PTR [rdx]     ; and update the GDTR

SelectorRld::
  mov     ax, DATA_SELECTOR ; Update the Base for the new selectors, too
  mov     ds, ax
  mov     es, ax
  mov     fs, ax
  mov     gs, ax
  mov     ss, ax  

  ret
InitializeGdt  endp

;-----------------------------------------------------------------------------;
; data
;-----------------------------------------------------------------------------;

        align 16

gdtr    dw GDT_END - GDT_BASE - 1   ; GDT limit
        dq OFFSET GDT_BASE          ; (GDT base gets set above)

;-----------------------------------------------------------------------------;
;   global descriptor table (GDT)
;-----------------------------------------------------------------------------;

        align 16

public GDT_BASE
GDT_BASE:
; null descriptor
NULL_SEL            equ $-GDT_BASE    ; Selector [0]
    dw  0  ; limit 15:0
    dw  0  ; base  15:0
    db  0  ; base  23:16
    db  0  ;
    db  0  ; type & limit 19:16
    db  0  ; base  31:24

; linear data segment descriptor
LINEAR_SEL      equ $-GDT_BASE        ; Selector [0x8]
    dw  0ffffh  ; limit 15:0
    dw  0       ; base  15:0
    db  0       ; base  23:16
    db  92h     ;present, ring 0, data, expand-up writable
    db  0cfh    ; type & limit 19:16 
    db  0       ; base  31:24

; linear code segment descriptor
LINEAR_CODE_SEL equ $-GDT_BASE        ; Selector [0x10]
    dw  0ffffh      ; limit 15:0
    dw  0          ; base  15:0
    db  0          ; base  23:16
    db  9ah        ; present, ring 0, code, expand-up writable
    db  0cfh       ; type & limit 19:16   
    db  0          ; base  31:24

; system data segment descriptor
SYS_DATA_SEL    equ $-GDT_BASE        ; Selector [0x18]
    dw  0ffffh     ; limit 15:0
    dw  0          ; base  15:0
    db  0          ; base  23:16
    db  92h       ; type & limit 19:16
    db  0cfh
    db  0          ; base  31:24

; system code segment descriptor
SYS_CODE_SEL    equ $-GDT_BASE        ; Selector [0x20]
    dw  0ffffh     ; limit 15:0
    dw  0      ; base  15:0
    db  0      ; base  23:16
    db  9ah ; type & limit 19:16
    db  0cfh
    db  0      ; base  31:24

; spare segment descriptor             ; Selector [0x28]
SPARE3_SEL  equ $-GDT_BASE
    dw  0  ; limit 15:0
    dw  0  ; base  15:0
    db  0  ; base  23:16
    db  0  ; type & limit 19:16
    db  0  ; base  31:24
    db  0

; 64bit data segment descriptor
LONG_MODE_DATA_SEL    equ $-GDT_BASE        ; Selector [0x30]
    dw  0ffffh  ; limit 15:0
    dw  0      ; base  15:0
    db  0      ; base  23:16
    db  92h    ; type & limit 19:16
    db  0cfh
    db  0      ; base  31:24

; 64bit code segment descriptor
LONG_MODE_CODE    equ $-GDT_BASE        ; Selector [0x38]
   dw  0ffffh  ; limit 15:0
   dw  0      ; base  15:0
   db  0      ; base  23:16
   db  9ah ; type & limit 19:16
   db  0afh
   db  0      ; base  31:24

; spare segment descriptor
SPARE5_SEL  equ $-GDT_BASE               ; Selector [0x40]
    dw  0  ; limit 15:0
    dw  0  ; base  15:0
    db  0  ; base  23:16
    db  0  ; type & limit 19:16
    db  0  ; base  31:24
    db  0

GDT_END:

text  ENDS
END

