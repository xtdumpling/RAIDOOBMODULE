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
; $Header: /Alaska/SOURCE/Core/Modules/IA32Core/IA32AsmLib/EnableMachineCheck.asm 1     10/01/10 4:56p Felixp $
;
; $Revision: 1 $
;
; $Date: 10/01/10 4:56p $Log:$
;  
; 
;*************************************************************************
;<AMI_FHDR_START>
;
; Name:
;
; Description:
;
;<AMI_FHDR_END>
;*************************************************************************
.686p
.xmm
.model  flat
.code

;*************************************************************************
;
; Name: EnableMachineCheck
;
; Description:
;  VOID EnableMachineCheck(VOID) sets the Machine Check Exception bit in CR4,
; which enables machine check interrupts to occur.
;
; Input:
;  VOID.
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
;*************************************************************************
_EnableMachineCheck proc
	mov	eax, cr4
	or	eax, 1 SHL 6
	mov	cr4, eax
	ret
_EnableMachineCheck endp

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
;*************************************************************************
