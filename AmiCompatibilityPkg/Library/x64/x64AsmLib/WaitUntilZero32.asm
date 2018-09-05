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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/WaitUntilZero32.asm 1     10/01/10 5:08p Felixp $
;
; $Revision: 1 $
;
; $Date: 10/01/10 5:08p $
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
; Name: WaitUntilZero32
;
; Description:
;  VOID WaitUntilZero32(IN volatile UINT32 *Value) waits until the UINT32
; value stored at the Value address becomes 0, then continues.
;
; Input:
;  IN volatile UINT32 *Value
; Address of the UINT32 value to be monitored.
;
; Output:
;  VOID.
;
; Modified:
;
; Referrals:
;
; Notes:
;
;<AMI_PHDR_END>
;*************************************************************************
WaitUntilZero32 Proc
@@:
	mov eax, [rcx]
	or	eax, eax
	pause
	jnz @b
	ret
WaitUntilZero32 endp

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
