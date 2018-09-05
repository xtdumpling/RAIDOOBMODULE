;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
  page    ,132
  title   CPU ARCHITECTURAL DXE PROTOCOL ASSEMBLY HOOKS
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2013, Intel Corporation. All rights reserved.<BR>
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
;   CpuAsm.asm
; 
; Abstract:
; 
;   Assembly code that supports IA32 CPU architectural protocol
;
;------------------------------------------------------------------------------

.686p
.model  flat        

.code
.MMX
.XMM

UINT8    TYPEDEF    BYTE
UINT16   TYPEDEF    WORD
UINT32   TYPEDEF    DWORD
UINT64   TYPEDEF    QWORD
UINTN    TYPEDEF    UINT32

;---------------------------------------;
; _InitializeSelectors                  ;
;----------------------------------------------------------------------------;
; 
; Protocol prototype
;   InitializeSelectors (
;   )
;           
; Routine Description:
; 
;  Creates an new GDT in RAM.  The problem is that our former selectors
;  were ROM based and the EFI OS Loader does not manipulate the machine state 
;  to change them (as it would for a 16-bit PC/AT startup code that had to
;  go from Real Mode to flat mode).
; 
; Arguments:
; 
; 
; Returns: 
; 
;   Nothing
;
; 
; Input:  [ebp][0]  = Original ebp
;         [ebp][4]  = Return address
;          
; Output: Nothing
;           
; Destroys: Nothing
;-----------------------------------------------------------------------------;

CODE_SELECTOR EQU 10h
DATA_SELECTOR EQU 18h

_InitializeGdt  proc near public
  push    ebp                 ; C prolog
  mov     ebp, esp
  pushad
  mov     edi, OFFSET Gdtr    ; Load GDT register

  mov     ax,cs               ; Get the selector data from our code image          
  mov     es,ax
  lgdt    FWORD PTR es:[edi]  ; and update the GDTR

  db      067h
  db      0eah              ; Far Jump Offset:Selector to reload CS
  dd      OFFSET SelectorRld;   Offset is ensuing instruction boundary
  dw      CODE_SELECTOR     ;   Selector is our code selector, 10h
SelectorRld::
  mov     ax, DATA_SELECTOR ; Update the Base for the new selectors, too
  mov     ds, ax
  mov     es, ax
  mov     fs, ax
  mov     gs, ax
  mov     ss, ax  

  popad
  pop     ebp
  ret
_InitializeGdt  endp
  
;-----------------------------------------------------------------------------;
; data
;-----------------------------------------------------------------------------;

;.data
        align 16

gdtr    dw GDT_END - GDT_BASE - 1   ; GDT limit
        dd OFFSET GDT_BASE          ; (GDT base gets set above)

;-----------------------------------------------------------------------------;
;   global descriptor table (GDT)
;-----------------------------------------------------------------------------;

        align 16

public GDT_BASE
GDT_BASE:
; null descriptor
NULL_SEL            equ $-GDT_BASE    ; Selector [0]
        dw 0            ; limit 15:0
        dw 0            ; base 15:0
        db 0            ; base 23:16
        db 0            ; type
        db 0            ; limit 19:16, flags
        db 0            ; base 31:24

; linear data segment descriptor
LINEAR_SEL      equ $-GDT_BASE        ; Selector [0x8]
        dw 0FFFFh       ; limit 0xFFFFF
        dw 0            ; base 0
        db 0
        db 092h         ; present, ring 0, data, expand-up, writable
        db 0CFh                 ; page-granular, 32-bit
        db 0

; linear code segment descriptor
LINEAR_CODE_SEL equ $-GDT_BASE        ; Selector [0x10]
        dw 0FFFFh       ; limit 0xFFFFF
        dw 0            ; base 0
        db 0
        db 09Ah         ; present, ring 0, data, expand-up, writable
        db 0CFh                 ; page-granular, 32-bit
        db 0

; system data segment descriptor
SYS_DATA_SEL    equ $-GDT_BASE        ; Selector [0x18]
        dw 0FFFFh       ; limit 0xFFFFF
        dw 0            ; base 0
        db 0
        db 092h         ; present, ring 0, data, expand-up, writable
        db 0CFh                 ; page-granular, 32-bit
        db 0

; system code segment descriptor
SYS_CODE_SEL    equ $-GDT_BASE
        dw 0FFFFh       ; limit 0xFFFFF
        dw 0            ; base 0
        db 0
        db 09Ah         ; present, ring 0, data, expand-up, writable
        db 0CFh                 ; page-granular, 32-bit
        db 0

; spare segment descriptor
SPARE3_SEL  equ $-GDT_BASE
        dw 0            ; limit 0xFFFFF
        dw 0            ; base 0
        db 0
        db 0            ; present, ring 0, data, expand-up, writable
        db 0            ; page-granular, 32-bit
        db 0

; spare segment descriptor
SPARE4_SEL  equ $-GDT_BASE
        dw 0            ; limit 0xFFFFF
        dw 0            ; base 0
        db 0
        db 0            ; present, ring 0, data, expand-up, writable
        db 0            ; page-granular, 32-bit
        db 0

; spare segment descriptor
SPARE5_SEL  equ $-GDT_BASE
        dw 0            ; limit 0xFFFFF
        dw 0            ; base 0
        db 0
        db 0            ; present, ring 0, data, expand-up, writable
        db 0            ; page-granular, 32-bit
        db 0

GDT_END:


  end

