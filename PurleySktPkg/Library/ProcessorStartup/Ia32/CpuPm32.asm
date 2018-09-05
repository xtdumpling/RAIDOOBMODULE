;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Intel Restricted Secret
;
; CPU Reference Code
;
; Copyright (c) 2007 - 2012, Intel Corporation.
;
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; This program has been developed by Intel Corporation.
; Licensee has Intel's permission to incorporate this source code
; into their product, royalty free.  This source code may NOT be
; redistributed to anyone without Intel's written permission.
;
; Intel specifically disclaims all warranties, express or
; implied, and all liability, including consequential and other
; indirect damages, for the use of this code, including liability
; for infringement of any proprietary rights, and including the
; warranties of merchantability and fitness for a particular
; purpose.  Intel does not assume any responsibility for any
; errors which may appear in this code nor any responsibility to
; update it.
;
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;
; Module Name:
;
;   cpuPm32.asm
;
; Abstract:
;
;
;
; Notes:
;
;------------------------------------------------------------------------------

   .686P
   .xmm

.XLIST

; Files generated during build time...do not edit!!!
   include procstartupentry.inc


.LIST

_TEXT16         SEGMENT PARA USE16 PUBLIC 'CODE16'

ALIGN 16

;------------------------------------------------------------------------------
; _PM32Entry:
;       In this sample implementation, entry to this procedure is from
;       a FAR JMP at the reset vector itself. Then load GDT, enter protected
;       mode, and performs a far jump into 32-bit flat code.
;
; Input:
;       Nothing.
;
; Output:
;       Nothing.
;
; Destroys:
;       All.
;
; Note:
;------------------------------------------------------------------------------
_PM32Entry      PROC    NEAR    PUBLIC

   ;
   ; Save BIST state currently in EAX into MM0
   ;
   movd    MM0, eax

   ;
   ; Switch to Protected mode.
   ;
   xor     eax, eax
   mov     ax, cs
   mov     ebx, eax                           ; Save the code segment for use later
   mov     ds, ax
   DB      66h
   lgdt    fword ptr cs:GDTDescriptor

   mov     eax, cr0                           ; Get control register 0
   or      eax, 00000003h                     ; Set PE bit (bit #0) & MP bit (bit #1)
   mov     cr0, eax                           ; Activate protected mode
   mov     eax, cr4                           ; Get control register 4
   or      eax, 00000600h                     ; Set OSFXSR bit (bit #9) & OSXMMEXCPT bit (bit #10)
   mov     cr4, eax

   ; Set up selectors for Protected Mode entry.
   mov     ax, SYS_DATA_SEL
   mov     ds, ax
   mov     es, ax
   mov     fs, ax
   mov     gs, ax
   mov     ss, ax

   ;
   ; Go to Protected32
   ;
   jmp     FWORD PTR cs:[ProcStartupRefCodeLinearAddress]

; 16-bit Selector and 32-bit offset value for the FAR JMP to Processor Reference Code.

ProcStartupRefCodeLinearAddress   LABEL   FWORD
   DD      PROCSTARTUPINITENTRYPOINT
   DW      LINEAR_CODE_SEL

   PUBLIC  BootGDTtable

; GDT[0]: 0x00: Null entry, never used.
NULL_SEL        EQU     $ - GDT_BASE            ; Selector [0]
GDT_BASE:
BootGDTtable    DD      0
                DD      0
;
; Linear code segment descriptor
;
LINEAR_CODE_SEL EQU     $ - GDT_BASE            ; Selector [0x8]
                DW      0FFFFh                  ; limit 0xFFFFF
                DW      0                       ; base 0
                DB      0
                DB      09Bh                    ; present, ring 0, code, execute/read, non-conforming, accessed
                DB      0CFh                    ; page-granular, 32-bit
                DB      0
; System data segment descriptor
SYS_DATA_SEL    EQU     $ - GDT_BASE            ; Selector [0x10]
                DW      0FFFFh                  ; limit 0xFFFFF
                DW      0                       ; base 0
                DB      0
                DB      093h                    ; present, ring 0, data, read/write, expand-up, accessed
                DB      0CFh                    ; page-granular, 32-bit
                DB      0

; F000h code segment descriptor
F000_CODE_SEL   EQU     $ - GDT_BASE            ; Selector [0x18]
                DW      0FFFFh                  ; limit 0xFFFF
                DW      0000h                   ; base 0xF000
                DB      0Fh
                DB      09Bh                    ; present, ring 0, code, execute/read, non-conforming, accessed
                DB      000h                    ; byte-granular, 16-bit
                DB      0

GDT_SIZE        EQU     $ - BootGDTtable        ; Size, in bytes

; Global Descriptor Table Descriptor
GDTDescriptor:                                          ; GDT descriptor
                DW      GDT_SIZE - 1                    ; GDT limit
                DW      LOWWORD OFFSET BootGDTtable     ; GDT base address
                DW      0ffffh



_PM32Entry      ENDP

_TEXT16 ENDS

END

