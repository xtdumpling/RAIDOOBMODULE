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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/MemSet.asm 1     10/01/10 5:08p Felixp $
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
; Name: MemSet
;
; Description:
;  VOID* MemSet(IN VOID *pBuffer, IN UINTN Count, IN UINT8 Value) fills Count
; bytes of memory in pBuffer with Value.
;
; Input:
;  IN VOID *pBuffer
; The starting location in memory where to begin filling.
;
;  IN UINTN Count
; The number of bytes to fill with Value.
;
;  IN UINT8 Value
; The value to fill memory with starting at pBuffer.
;
; Output:
;  Returns pointer to pBuffer.
;
; Modified:
;
; Referrals:
;
; Notes:
;
;<AMI_PHDR_END>
;*************************************************************************
public memset
memset:
		push rdi
		push rbx
		push rcx ; save pBuffer
		mov rdi, rcx  ; rdi = pBuffer
		mov rax, rdx ; al = Value
		mov rcx, r8 ; rcx = Count
		jmp MemSet2
MemSetAsm proc
		push rdi
		push rbx
		push rcx ; save pBuffer
		mov rdi, rcx  ; rdi = pBuffer
		mov rcx, rdx ; rcx = Count
		mov rax, r8 ; al = Value
MemSet2::		
		; fill EAX with the Value so that we can perform DWORD operatins
		mov ah, al
		mov bx,ax
		shl rax,16
		mov ax,bx
		; if Counter is less then 4, jump to byte copy
		cmp rcx, 4
		jb  CopyByte
		; check if the Buffer is 4-bytes aligned
		mov rdx,rdi
		and rdx, 3
		; if the Buffer is 4-bytes aligned, jump to DWORD copy
		jz CopyDword
		; Buffer is not 4-bytes aligned
		; Calculate 4-(Buffer%4), which is a number of bytes we have to copy before
		; Buffer will reach 4-bytes boundary, and perform byte copy
		neg rdx
		add rdx, 4
		xchg rcx, rdx
		sub rdx, rcx
		rep stosb
		mov rcx, rdx
CopyDword:
		; perform DWORD copy
		mov rdx, rcx
		shr rcx, 2
		rep stosd
		; copy the remainder
		and rdx,3
		mov rcx, rdx
CopyByte:
		rep stosb
		;;;
		pop rax ; return pBuffer
		pop rbx
		pop rdi
		ret
MemSetAsm endp

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
