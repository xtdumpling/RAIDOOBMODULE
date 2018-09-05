data SEGMENT
data ENDS

.code


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
; Procedure:	DisableBreakpointOnIO
;
; Description:	This function writes the CR4 to clear the DE bit, in order to
;				disable the breakpoint on IO
;
; Input:		none
;
; Output:		None
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>
DisableBrkptOnIO	PROC PUBLIC
	mov		rax,cr4
	and 	eax,0xffffffF7
	mov		cr4,rax
	ret
DisableBrkptOnIO	ENDP
		
END
;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
