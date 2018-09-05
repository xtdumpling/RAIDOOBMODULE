; /*++
; Copyright (c) 2012 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
; --*/

.586P
.XMM
IFDEF MINIBIOS_BUILD
.MODEL  SMALL
ELSE
.MODEL  SMALL, c
ENDIF

IFNDEF MINIBIOS_BUILD
; Externs
EXTERN  PcdGet32 (PcdFlashFvSecPeiBase):DWORD   
EXTERN  PcdGet8 (PcdSerialDbgLvlAtBadCmos):BYTE
EXTERN  PcdGet8(PcdCmosDebugPrintLevelReg):BYTE
;EXTERN  PcdGet32 (PcdFlashFvBackSecPeiBase):DWORD   

;Constant equates
CMOS_INDEX EQU 70h
CMOS_DATA  EQU 71h
CMOS_STATUS EQU 0Eh
CMOS_RTCSTS EQU 80h
CMOS_CHECKSUMSTS EQU 40h

ENDIF
;----------------------------------------------------------------------------
;  STARTUP_SEG  S E G M E N T  STARTS
;----------------------------------------------------------------------------
STARTUP_SEG    SEGMENT    PARA PUBLIC 'CODE' USE32

;----------------------------------------------------------------------------
;
; Procedure:  EarlyChpsetInit_OemHook
;
; Description:  This routine provide oem hook after EarlyChipsetInit
;
; Input:  Stack NOT available
;        MMX3 = routine return address
;
; Output:  None
;
; Modified:  All,
;
;----------------------------------------------------------------------------
EarlyChpsetInit_OemHook    PROC PUBLIC
 ;platform OEM code

IFNDEF MINIBIOS_BUILD
  mov al, CMOS_STATUS
  out CMOS_INDEX, al
  in al, CMOS_DATA

  and al, CMOS_CHECKSUMSTS OR CMOS_RTCSTS
  jz CmosGood
  mov bl, PcdGet8 (PcdSerialDbgLvlAtBadCmos)
  xor edx, edx                          ; Serial debug message level data default = 00
  cmp bl, 01
  jnz @f
  mov edx, 80000000h                    ; Minimum level
  jmp UpdateData
@@:
  cmp bl, 02
  jnz @f
  mov edx, 80000042h                    ; Normal level
  jmp UpdateData
@@:
  cmp bl, 03
  jnz UpdateData
  mov edx, 0FFFFFFFFh                   ; Maximum level

UpdateData:
  mov ecx, 4
  mov al, PcdGet8 (PcdCmosDebugPrintLevelReg)
@@:
  out CMOS_INDEX, al
  ror eax, 16
  mov al, dl
  out CMOS_DATA, al
  shr edx, 8
  ror eax, 16
  inc al
  loop @b

CmosGood:
ENDIF

 ;the following code is RET_ESI_MMX3
  movd    esi, mm3                      ; restore return address from MM3
  jmp     esi                           ; Absolute jump
EarlyChpsetInit_OemHook    ENDP

;----------------------------------------------------------------------------
;
; Procedure:  LateChipsetInit_OemHook
;
; Description:  This routine provide oem hook after LateChipsetInit
;
; Input:  none
;
; Output:  None
;
; Modified:  All, except EBP and ESP
;
;----------------------------------------------------------------------------
LateChipsetInit_OemHook    PROC PUBLIC
 ;platform OEM code
  ret
LateChipsetInit_OemHook    ENDP



;----------------------------------------------------------------------------
;  STARTUP_SEG  S E G M E N T  ENDS
;----------------------------------------------------------------------------
STARTUP_SEG    ENDS

END

