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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/ReadMsr.asm 1     10/01/10 5:08p Felixp $
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
; Name: ReadMsr
;
; Description:
;  UINT64 ReadMsr(UINT32 Msr) reads the CPU MSR index defined by Msr and
; returns the value.
;
; Input:
;  IN UINT32 Msr
; 32-bit MSR index to be read.
;
; Output:
;  UINT64 MSR value at MSR index, Msr.
;
; Modified:
;
; Referrals:
;
; Notes:
;
;<AMI_PHDR_END>
;*************************************************************************
ReadMsr Proc ;(UINT32 Msr)
    rdmsr       ;rcx = MSR
    and     rax, 0ffffffffh
    shl     rdx, 32
    or      rax, rdx              ;rax = (rdx << 32) | eax
	ret
ReadMsr endp

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
