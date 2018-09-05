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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/MemReadWrite32.asm 1     10/01/10 5:07p Felixp $
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
; Name: MemReadWrite32
;
; Description:
;  VOID MemReadWrite32(IN UINT32 *Address, IN UINT32 Value, IN UINT32 Mask)
; reads the 32-bit value stored at Address, ANDs it with Mask, ORs the result
; with Value, then writes the result back to Address.
;
; Input:
;  IN UINT32 *Address
; Address which shall be read from and subsequently written to.
;
;  IN UINT32 Value
; Value to be ORed with the value stored at Address after it has been ANDed
; with the provided Mask.
;
;  IN UINT32 Mask
; Mask to be ANDed with the original value stored at Address.
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
MemReadWrite32 proc
    mov	eax, [rcx]
    and eax, r8D        ;Mask
    or	eax, edx        ;Value
    mov [rcx], eax
    ret
MemReadWrite32 endp

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
