;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************

;*************************************************************************
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/CPULib_GetInterruptState.asm 1     10/01/10 5:07p Felixp $
;
; $Revision: 1 $
;
; $Date: 10/01/10 5:07p $
;*************************************************************************
;<AMI_FHDR_START>
;
; Name:
;
; Description:
;
;<AMI_FHDR_END>
;*************************************************************************
.code

;*************************************************************************
;<AMI_PHDR_START>
;
; Name: CPULib_GetInterruptState
;
; Description:
;  BOOLEAN CPULib_GetInterruptState()returns the current CPU interrupt state.
;
; Input:
;  VOID.
;
; Output:
;  FALSE if interrupts are disabled; otherwise TRUE.
;
; Modified:
;
; Referrals:
;
; Notes:
;
;<AMI_PHDR_END>
;*************************************************************************
CPULib_GetInterruptState	PROC PUBLIC
	xor	cl, cl
	pushf			; push flags onto stack.
	pop	ax		    ; eax = flags.
	bt	ax, 9		; IF (bit 9) if set, set carry flag.
                                ; Interrupts are allowed if IF is set.
	adc	cl, 0		; CL = IF = CF.

	mov	al, cl		; Return value
	ret
CPULib_GetInterruptState	ENDP

END
;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
