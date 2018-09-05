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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/MemCpy32.asm 1     10/17/11 1:03p Yakovlevs $
;
; $Revision: 1 $
;
; $Date: 10/17/11 1:03p $
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
; Name: MemCpy32
;
; Description:
;  VOID* MemCpy32(OUT VOID *pDestination, IN VOID *pSource, IN UINTN Count)
; copies Count bytes of memory from Source to Destination.
;
; Input:
;  OUT VOID *pDestination
; Memory address where data shall be copied.  User is responsible for
; allocating the necessary memory resources.
;
;  IN VOID *pSource
; Memory address from where data shall be copied.
;
;  IN UINTN Count
; Number of bytes to copy from pSource.
;
; Output:
;  Returns pointer to destination buffer.
;
; Modified:
;
; Referrals:
;
; Notes:
;  This function checks for overlapping of source and destination and
; selects copy direction that prevents memory corruption.
;
;<AMI_PHDR_END>
;*************************************************************************
MemCpy32 proc
		push rdi
		push rsi
        push rbx
		pushf
                push rcx ; pDestination
		mov rsi, rdx ; pSource
		mov rdi, rcx ; pDestination
		mov rcx, r8 ; Count
		mov dl, 0
        ; if pSource > pDestination CopyForward
		mov rax, rsi
		sub rax, rdi ; rax = pSource-pDestination
		jnb CopyForward; if pSource-pDestination > 0 CopyForward
        ; if pSource+Count < pDestination then CopyForward
		lea rbx, [rsi+rcx] ; rbx = pSource + Count
		neg rax ; rax = pDestination - pSource
		cmp rbx, rdi
		jb CopyForward ; if (pSource + Count < pDestination ) CopyForward
        ; Copy Backward
		mov rsi, rbx; rsi = pSource + Count
		lea rdi, [rdi+rcx]; rdi = pDestination + Count
		mov dl, 1; Flag to indicate that we are copying backward
		std; set direction flag to copy backward
CopyForward:
		cmp rcx, 4 ; if (Counter<4) copy byte by byte
		jb m8
		cmp rax, 4 ; if (pDestination - pSource < 4) copy byte by byte
		jb m8
        ; if pSource and pDestination are not 4 byte aligned
		; Calculate 4-(Buffer%4), which is a number of bytes we have to copy to align the buffer
        ; if this number if the same for source and destinations
        ; copy several bytes to align them
		; otherwise proceed to DWORD copy
		mov rax, rsi
		mov rbx, rdi
		and rax, 3
		and rbx, 3
		test dl, dl
		jz skip1
		dec rsi
		dec rdi
skip1:
		cmp rax, rbx
		jne m32
		test rax, rax
		jz m32
		test dl, dl
		jnz skip_nz1
		neg rax
		add rax, 4
skip_nz1:
		xchg rax, rcx
		sub rax, rcx
		rep movsb
		mov rcx, rax
m32:
		test dl, dl
		jz skip2
		sub rsi, 3
		sub rdi, 3
skip2:
		mov rax, rcx
		shr rcx, 2
		rep movsd
		and rax, 3
		jz MemCpuEnd
		test dl, dl
		jz skip3
		add rsi, 4
		add rdi, 4
skip3:
		mov rcx, rax
m8:
		test dl, dl
		jz skip4
		dec rsi
		dec rdi
skip4:
		rep movsb
MemCpuEnd:
		pop rax
                popf
        pop rbx
		pop rsi
		pop rdi
        ret
MemCpy32 endp

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
