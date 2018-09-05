;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
;**                                                                  **
;**                       All Rights Reserved.                       **
;**                                                                  **
;**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
;**                                                                  **
;**                       Phone: (770)-246-8600                      **
;**                                                                  **
;**********************************************************************
;**********************************************************************
; $Header: $
;
; $Revision: $
;
; $Date: $
;
;****************************************************************************
;<AMI_FHDR_START>
;
; Name: CmosAccessSec.inc
;
; Description:
;   This file contains macros, constants, procedure definitions and structure 
;   declarations to be used in SEC phase assembly code.
;
;<AMI_FHDR_END>
;*************************************************************************

IFNDEF  _CMOSACCESS_SEC_INC_
_CMOSACCESS_SEC_INC_   EQU 1

INCLUDE Token.equ

IF MKF_CMOS_SEC_SUPPORT

INCLUDE TokenEqu.equ

SEC_LAST_ADDRESS_VALID = 0

jmp_edi macro dummy                 ; goto routine & back (via EDI)
        local   llll                ; local label
        mov     edi, OFFSET llll
        jmp     dummy
llll:
endm

jmp_edx macro dummy                 ; goto routine & back (via EDX)
        local   llll                ; local label
        mov     edx, OFFSET llll
        jmp     dummy
llll:
endm

CMOS_WRITE_ACCESS   = 0
CMOS_READ_ACCESS    = 1

CMOS_BATTERY_ERR    = 012h
CMOS_BITSIZE_ERR    = 013h
CMOS_OVERFLOW_ERR   = 014h

IFNDEF CMOS_NO_EXTERNS
EXTERN  CmosReadWrite:NEAR32
ENDIF

IF SEC_LAST_ADDRESS_VALID
EXTERN  LAST_ADDRESS:ABS
ENDIF

ENDIF

ENDIF

;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
;**                                                                  **
;**                       All Rights Reserved.                       **
;**                                                                  **
;**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
;**                                                                  **
;**                       Phone: (770)-246-8600                      **
;**                                                                  **
;**********************************************************************
;**********************************************************************
