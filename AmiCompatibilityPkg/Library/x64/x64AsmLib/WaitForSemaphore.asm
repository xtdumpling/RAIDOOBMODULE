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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/WaitForSemaphore.asm 1     10/01/10 5:08p Felixp $
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
; Name: WaitForSemaphore
;
; Description:
;  VOID WaitForSemaphore(IN volatile UINT8 *Semaphore) waits for the
; semaphore to become available; once available, it claims the semaphore and
; returns.
;
; Input:
;  IN volatile UINT8 *Semaphore
; Pointer to the desired semaphore.
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
WaitForSemaphore Proc
	mov	al, 1
@@:
	xchg al, [rcx]
	or	al, al
	pause
	jnz	@b
	ret
WaitForSemaphore endp

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
