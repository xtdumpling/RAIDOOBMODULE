;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license 
; agreement with Intel or your vendor.  This file may be      
; modified by the user, subject to additional terms of the    
; license agreement                                           
;

;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2014, Intel Corporation. All rights reserved.<BR>
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
;  Flat32.asm
;
; Abstract:
;
;  This is the code that goes from real-mode to protected mode.
;  It consumes the reset vector
;
;------------------------------------------------------------------------------

  INCLUDE Flat32.inc
  INCLUDE ProcessorStartupPlatform.inc

.686p
.xmm
.model small, c

PUBLIC  TopOfCar

EXTERN  _ProcessorStartup:NEAR
EXTERN   SecStartup:NEAR

_TEXT_REALMODE      SEGMENT PARA PUBLIC USE16 'CODE'
                    ASSUME  CS:_TEXT_REALMODE, DS:_TEXT_REALMODE

align 4
Flat32Start PROC NEAR C PUBLIC

  ;
  ; Save BIST state in MM0
  ;
  fninit
  movd    mm0, eax

  DB	0B8h, 000h, 0F0h	; mov     ax, 0F000h
  DB	08Eh, 0D8h	        ; mov     ds, ax
  DB	0BEh, 0F0h, 0FFh	; mov     si, 0FFF0h
  DB	080h, 03Ch, 0EAh	; cmp     BYTE PTR [si], 0EAh   ; Is it warm reset ?
  jne	NotWarmReset		; JIf not.

  DB	0EAh			; Far jump to F000:E05B (legacy BIOS warm reset entry)
  DW	0E05Bh			;
  DW	0F000h			;
NotWarmReset:

  xor     eax, eax
  mov     es, ax
  mov     ax, cs
  mov     ds, ax


  ;
  ; Enter Protected mode.
  ; 
  mov     esi,  OFFSET GdtDesc
  DB      66h
  lgdt    fword ptr cs:[si]
  mov     eax, cr0                      ; Get control register 0
  or      eax, 00000003h                ; Set PE bit (bit #0) & MP bit (bit #1)
  mov     cr0, eax                      ; Activate protected mode
  mov     eax, cr4                      ; Get control register 4
  or      eax, 00000600h                ; Set OSFXSR bit (bit #9) & OSXMMEXCPT bit (bit #10)
  mov     cr4, eax

  ;
  ; Now we're in Protected16
  ; Set up the selectors for protected mode entry
  ;
  mov     ax, SYS_DATA_SEL
  mov     ds, ax
  mov     es, ax
  mov     fs, ax
  mov     gs, ax
  mov     ss, ax

  ;
  ; Go to Protected32
  ;
  mov     esi, offset NemInitLinearAddress
  jmp     fword ptr cs:[si]


Flat32Start ENDP
_TEXT_REALMODE      ENDS
;-----------------------------------------------------------------------------------------------;

;-----------------------------------------------------------------------------------------------;

_TEXT_PROTECTED_MODE      SEGMENT PARA PUBLIC USE32 'CODE'					;
                          ASSUME  CS:_TEXT_PROTECTED_MODE, DS:_TEXT_PROTECTED_MODE		;
;-----------------------------------------------------------------------------------------------;
align 4
;-----------------------------------------------------------------------------------------------;
;					ProtectedModeSECStart					;
;-----------------------------------------------------------------------------------------------;
ProtectedModeSECStart PROC NEAR PUBLIC
  mov     esi, OFFSET ProcessorStartupReturnAddress
  mov     edi, 0
  jmp _ProcessorStartup
  
ProcessorStartupReturnAddress:: 
  jmp ExitSECPhase  


ExitSecPhase:
 	jmp  HandOffToNextModule
ProtectedModeSECStart ENDP


HandOffToNextModule   PROC    NEAR	PUBLIC
  ; Push CPU count to stack first, then AP's (if there is one)
  ; BIST status, and then BSP's
  ;

  ;
  ; Here work around for BIST
  ; Can only read BSP BIST, so only pass 1 CPU value
  ;
  movd    eax, mm0
  mov     ecx, 1
  push    ecx

PushBist:
  push    eax
  loop    PushBist

TransferToSecStartup:

  ;
  ; Add for PI
  ;
  
  mov     eax, esp
  mov     ebx, 087655678h
  push    ebx
  push    eax
  mov     bx,  0
  push    bx
  lidt    fword  ptr [esp]        ;Load IDT register
  pop     bx
  pop     eax  

  ;
  ; Pass BFV into the PEI Core
  ;
  mov     edi, FV_MAIN_BASE             ; 0FFFFFFFCh
  push    Dword Ptr Ds:[edi]

  
  ;
  ; Pass TempRamBase into the PEI Core for PI
  ;
  push    DATA_STACK_BASE_ADDRESS


  ;
  ; Pass stack size into the PEI Core
  ;
  push    DATA_STACK_SIZE

  ;
  ; Pass Control into the PEI Core
  ;
  call SecStartup

HandOffToNextModule   ENDP


MtrrInitTable   LABEL BYTE
    DW  MTRR_DEF_TYPE
    DW  MTRR_FIX_64K_00000
    DW  MTRR_FIX_16K_80000
    DW  MTRR_FIX_16K_A0000
    DW  MTRR_FIX_4K_C0000
    DW  MTRR_FIX_4K_C8000
    DW  MTRR_FIX_4K_D0000
    DW  MTRR_FIX_4K_D8000
    DW  MTRR_FIX_4K_E0000
    DW  MTRR_FIX_4K_E8000
    DW  MTRR_FIX_4K_F0000
    DW  MTRR_FIX_4K_F8000
    DW  MTRR_PHYS_BASE_0
    DW  MTRR_PHYS_MASK_0
    DW  MTRR_PHYS_BASE_1
    DW  MTRR_PHYS_MASK_1
    DW  MTRR_PHYS_BASE_2
    DW  MTRR_PHYS_MASK_2
    DW  MTRR_PHYS_BASE_3
    DW  MTRR_PHYS_MASK_3
    DW  MTRR_PHYS_BASE_4
    DW  MTRR_PHYS_MASK_4
    DW  MTRR_PHYS_BASE_5
    DW  MTRR_PHYS_MASK_5
    DW  MTRR_PHYS_BASE_6
    DW  MTRR_PHYS_MASK_6
    DW  MTRR_PHYS_BASE_7
    DW  MTRR_PHYS_MASK_7
    DW  MTRR_PHYS_BASE_8
    DW  MTRR_PHYS_MASK_8
    DW  MTRR_PHYS_BASE_9
    DW  MTRR_PHYS_MASK_9
MtrrCount      EQU (($ - MtrrInitTable) / 2)

align 10h
PUBLIC  BootGDTtable

;
; GDT[0]: 0x00: Null entry, never used.
;
NULL_SEL        EQU $ - GDT_BASE        ; Selector [0]
GDT_BASE:
BootGDTtable        DD  0
                    DD  0
;
; Linear data segment descriptor
;
LINEAR_SEL      EQU $ - GDT_BASE        ; Selector [0x8]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  092h                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Linear code segment descriptor
;
LINEAR_CODE_SEL EQU $ - GDT_BASE        ; Selector [0x10]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Bh                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; System data segment descriptor
;
SYS_DATA_SEL    EQU $ - GDT_BASE        ; Selector [0x18]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0

;
; System code segment descriptor
;
SYS_CODE_SEL    EQU $ - GDT_BASE        ; Selector [0x20]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Ah                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_CODE_SEL  EQU $ - GDT_BASE        ; Selector [0x28]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0Eh                             ; Changed from F000 to E000.
    DB  09Bh                            ; present, ring 0, code, expand-up, writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_DATA_SEL  EQU $ - GDT_BASE        ; Selector [0x30]
    DW  0FFFFh                          ; limit 0xFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0

;
; Spare segment descriptor
;
SPARE5_SEL      EQU $ - GDT_BASE        ; Selector [0x38]
    DW  0                               ; limit 0
    DW  0                               ; base 0
    DB  0
    DB  0                               ; present, ring 0, data, expand-up, writable
    DB  0                               ; page-granular, 32-bit
    DB  0
GDT_SIZE        EQU $ - BootGDTtable    ; Size, in bytes

GdtDesc:                                ; GDT descriptor
OffsetGDTDesc   EQU $ - Flat32Start
    DW  GDT_SIZE - 1                    ; GDT limit
    DD  OFFSET BootGDTtable             ; GDT base address

NemInitLinearAddress   LABEL   FWORD
NemInitLinearOffset    LABEL   DWORD
    DD  OFFSET ProtectedModeSECStart    ; Offset of our 32 bit code
    DW  LINEAR_CODE_SEL

TopOfCar  DD  DATA_STACK_BASE_ADDRESS + DATA_STACK_SIZE

	PUBLIC	LAST_ADDRESS

LAST_ADDRESS	EQU	$

_TEXT_PROTECTED_MODE    ENDS
END
