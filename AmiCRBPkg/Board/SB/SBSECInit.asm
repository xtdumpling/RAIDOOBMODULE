;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
; Name:  SBSECInit.ASM
;
; Description: Program any workaround or initialization needed before
;              enabling Cache as memory in the SEC stage
;
; Notes:       None of the chipset initialization is done at this point.
;              System is exactly as if came out of RESET.
;
;<AMI_FHDR_END>
;*************************************************************************

 INCLUDE token.equ

.586P
.XMM
.MODEL SMALL, c

; Externs
EXTERN SECSB_EarlyInitEnd:NEAR32

; Define the equates here
BIT00          equ 00000001h
BIT01          equ 00000002h
BIT02          equ 00000004h
BIT03          equ 00000008h
BIT04          equ 00000010h
BIT05          equ 00000020h
BIT06          equ 00000040h
BIT07          equ 00000080h
BIT08          equ 00000100h
BIT09          equ 00000200h

PCI_LPC_BASE   equ 8000F800h
LPC_SS         equ 2Ch
LPC_REG_RCBA   equ 0F0h
ICH_RCRB_CIR1  equ 088h
ICH_REG_NMI_SC equ 061h

LPC_IO_DEC     equ 80h
LPC_GEN1_DEC   equ 84h
LPC_GEN2_DEC   equ 88h
LPC_GEN3_DEC   equ 8Ch
LPC_GEN4_DEC   equ 90h
IO_DEC_DEFAULT equ 370F0010h


;----------------------------------------------------------------------------
;       STARTUP_SEG  S E G M E N T  STARTS
;----------------------------------------------------------------------------
STARTUP_SEG  SEGMENT  PARA PUBLIC 'CODE' USE32


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure: SECSB_EarlyInit
;
; Description: This routine initializes South bridge for PEI preparation
;
; Input: ESP BIST Info
;  EBP Reset ID (EDX at reset)
;  Stack not available
;
; Output: None
;
; Modified: All, except EBP and ESP
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>
SECSB_EarlyInit  PROC PUBLIC

IF 0 ; #RCBA: RCBA is obselete from LBG PCH onwards
        mov al, 04h
        out 80h, al

        mov ebx, (MKF_SB_RCBA + 3410h)
        mov al, byte ptr [ebx]
        and al, NOT (BIT02)
IF MKF_RESERVED_PAGE_ROUTE
        ;Enable 80 port decoding to PCI
        or al, (BIT02)
ENDIF
        mov byte ptr [ebx], al

IF MKF_SB_IO_DECODE_IN_SEC
        ;Enable LPC IO decoding
        mov dx, 0CF8h
        mov eax, (PCI_LPC_BASE + LPC_IO_DEC)
        out dx, eax
        add dx, 04h
        mov     eax, IO_DEC_DEFAULT
ENDIF
        out dx, eax

 IFDEF MKF_SIO1_CONFIG_INDEX
  IF ((MKF_SIO1_CONFIG_INDEX NE 2Eh) AND (MKF_SIO1_CONFIG_INDEX NE 4Eh))
        mov dx, 0CF8h
        mov eax, (PCI_LPC_BASE + LPC_GEN2_DEC) ; 88h
        out dx, eax
        add dx, 04h
        mov eax, (MKF_SIO1_CONFIG_INDEX and 0FFFCh) or 1;
        out dx, eax
  ENDIF
 ENDIF

 IF MKF_SB_GEN_DECODE_RANGE_1
; Enable decoding of PME & GPIO Runtime Registers as Generic Decode Range 1
        mov dx, 0CF8h
        mov eax, (PCI_LPC_BASE + LPC_GEN1_DEC) ; 84h
        out dx, eax
        add dx, 04h
        mov eax, ((MKF_SB_GEN_DECODE_RANGE_1 AND 0FFFFh) OR (0FCh SHL 16))
        bts eax, 0
        out dx, eax
 ENDIF

 IF MKF_SB_GEN_DECODE_RANGE_2
; Enable decoding of PME & GPIO Runtime Registers as Generic Decode Range 2
        mov dx, 0CF8h
        mov eax, (PCI_LPC_BASE + LPC_GEN2_DEC) ; 88h
        out dx, eax
        add dx, 04h
        mov eax, ((MKF_SB_GEN_DECODE_RANGE_2 AND 0FFFFh) OR (0FCh SHL 16))
        bts eax, 0
        out dx, eax
 ENDIF

 IF MKF_SB_GEN_DECODE_RANGE_3
; Enable decoding of PME & GPIO Runtime Registers as Generic Decode Range 3
        mov dx, 0CF8h
        mov eax, (PCI_LPC_BASE + LPC_GEN3_DEC) ; 8Ch
        out dx, eax
        add dx, 04h
        mov eax, ((MKF_SB_GEN_DECODE_RANGE_3 AND 0FFFFh) OR (0FCh SHL 16))
        bts eax, 0
        out dx, eax
 ENDIF

 IF MKF_SB_GEN_DECODE_RANGE_4
; Enable decoding of PME & GPIO Runtime Registers as Generic Decode Range 4
        mov dx, 0CF8h
        mov eax, (PCI_LPC_BASE + LPC_GEN4_DEC) ; 90h
        out dx, eax
        add dx, 04h
        mov eax, ((MKF_SB_GEN_DECODE_RANGE_4 AND 0FFFFh) OR (0FCh SHL 16))
        bts eax, 0
        out dx, eax
 ENDIF
ENDIF ;
                                        ; (P092010G)>
        jmp SECSB_EarlyInitEnd
SECSB_EarlyInit  ENDP

;----------------------------------------------------------------------------
;       STARTUP_SEG  S E G M E N T  ENDS
;----------------------------------------------------------------------------
STARTUP_SEG  ENDS

END
;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************;
