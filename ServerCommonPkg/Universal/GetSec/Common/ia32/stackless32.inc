;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2007 Intel Corporation. All rights reserved
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
;   stackless32.inc
;
; Abstract:
;
;------------------------------------------------------------------------------

MOVDIN0                         EQU movd mm0, ebx ; db 00fh, 06eh, 0c3h
MOVDOUT0                        EQU movd ebx, mm0 ; db 00fh, 07eh, 0c3h

MOVDIN1                         EQU movd mm1, ebx ; db 00fh, 06eh, 0cbh
MOVDOUT1                        EQU movd ebx, mm1 ; db 00fh, 07eh, 0cbh

MOVDIN2                         EQU movd mm2, ebx ; db 00fh, 06eh, 0d3h
MOVDOUT2                        EQU movd ebx, mm2 ; db 00fh, 07eh, 0d3h

MOVDIN3                         EQU movd mm3, ebx ; db 00fh, 06eh, 0dbh
MOVDOUT3                        EQU movd ebx, mm3 ; db 00fh, 07eh, 0dbh

MOVDIN4                         EQU movd mm4, ebx ; db 00fh, 06eh, 0e3h
MOVDOUT4                        EQU movd ebx, mm4 ; db 00fh, 07eh, 0e3h

MOVDIN5                         EQU movd mm5, ebx ; db 00fh, 06eh, 0ebh
MOVDOUT5                        EQU movd ebx, mm5 ; db 00fh, 07eh, 0ebh

MOVDIN6                         EQU movd mm6, ebx ; db 00fh, 06eh, 0f3h
MOVDOUT6                        EQU movd ebx, mm6 ; db 00fh, 07eh, 0f3h

MOVDIN7                         EQU movd mm7, ebx ; db 00fh, 06eh, 0fbh
MOVDOUT7                        EQU movd ebx, mm7 ; db 00fh, 07eh, 0fbh

_EMMS                           EQU emms ; db 0fh, 77h

;-----  CALL_NS  ------------------------------------------------------------;
;                                                                            ;
;       Purpose:        Stackless "call" to destination procedure            ;
;                                                                            ;
;       Input:          routine to jump to                                   ;
;                                                                            ;
;       Output:         none                                                 ;
;                                                                            ;
;       Setup:          CR4 bit 9 must be set                                ;
;                                                                            ;
;       Uses:           esp, XMM4                                            ;
;                                                                            ;
;       Notes:          This macro allows the nesting of CALL_NS routines to ;
;                       be 3-deep.  DO NOT ATTEMPT TO GO DEEPER THAN THAT,   ;
;                       SINCE THE FIRST ADDRESS WILL BE LOST!!!              ;
;                                                                            ;
;----------------------------------------------------------------------------;
CALL_NS  macro   dest:REQ                       ; destination routine
        local   retaddr                         ; address to jump back to

        mov     esp, offset retaddr             ; get the return address

        ; Shift XMM register left to make room for the WORD.
        db      066h,0fh,73h,0fch,02h           ; PSLLDQ xmm4, 02h

        ; Insert the word
        db      066h,0fh,0c4h,0e4h,00h          ; PINSRW xmm4, esp, 00h

        ; Do the above again, but for the high word of esp
        shr     esp, 16
        db      066h,0fh,73h,0fch,02h           ; PSLLDQ xmm4, 02h
        db      066h,0fh,0c4h,0e4h,00h          ; PINSRW xmm4, esp, 00h
        jmp     dest
retaddr:

endm


;-----  RET_NS  -------------------------------------------------------------;
;                                                                            ;
;       Purpose:        Return to a stackless "call"                         ;
;                                                                            ;
;       Input:          none                                                 ;
;                                                                            ;
;       Output:         none                                                 ;
;                                                                            ;
;       Setup:          CR4 bit 9 must be set                                ;
;                                                                            ;
;       Uses:           esp, XMM4                                            ;
;                                                                            ;
;       Notes:          See the description for CALL_NS for details          ;
;                                                                            ;
;----------------------------------------------------------------------------;
RET_NS   macro
        ; Extract the most significant word of return address
        db      066h,0fh,0c5h,0e4h,00h        ; PEXTRW xmm4, esp, 00h
        ; Shift XMM register right to setup the next WORD.
        db      066h,0fh,73h,0dch,02h         ; PSRLDQ xmm4, 02h

        ; Do the above again, except for the low word of esp
        shl     esp, 16
        db      066h,0fh,0c5h,0e4h,00h        ; PEXTRW xmm4, esp, 00h
        db      066h,0fh,73h,0dch,02h         ; PSRLDQ xmm4, 02h
        jmp     esp

endm

