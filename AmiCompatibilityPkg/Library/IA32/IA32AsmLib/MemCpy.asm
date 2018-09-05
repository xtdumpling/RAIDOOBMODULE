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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/MemCpy.asm 2     3/09/11 1:58p Felixp $
;
; $Revision: 2 $
;
; $Date: 3/09/11 1:58p $
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
;<AMI_PHDR_START>
;
; Name: MemCpy
;
; Description:
;  VOID* MemCpy(OUT VOID *pDestination, IN VOID *pSource, IN UINTN Count)
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
public _memcpy
public _MemCpy32
_memcpy:
_MemCpy32:
MemCpyAsm proc C public pDestination:DWORD, pSource:DWORD, Count:DWORD
		push edi
		push esi
        push ebx
		pushf
                push pDestination
		mov esi, pSource
		mov edi, pDestination
		mov ecx, Count
		mov dl, 0
		mov eax, esi
		sub eax, edi
		jnb CopyForward
		lea ebx, [esi+ecx]
		neg eax
		cmp ebx, edi
		jb CopyForward
		mov esi, ebx
		lea edi, [edi+ecx]
		mov dl, 1
		std
CopyForward:
		cmp ecx, 4
		jb m8
		cmp eax, 4
		jb m8
		mov eax, esi
		mov ebx, edi
		and eax, 3
		and ebx, 3
		test dl, dl
		jz skip1
		dec esi
		dec edi
skip1:
		cmp eax, ebx
		jne m32
		test eax, eax
		jz m32
		test dl, dl
		jnz skip_nz1
		neg eax
		add eax, 4
skip_nz1:
		xchg eax, ecx
		sub eax, ecx
		rep movsb
		mov ecx, eax
m32:
		test dl, dl
		jz skip2
		sub esi, 3
		sub edi, 3
skip2:
		mov eax, ecx
		shr ecx, 2
		rep movsd
		and eax, 3
		jz memcpyend
		test dl, dl
		jz skip3
		add esi, 4
		add edi, 4
skip3:
		mov ecx, eax
m8:
		test dl, dl
		jz skip4
		dec esi
		dec edi
skip4:
		rep movsb
memcpyend:
                pop eax
                popf
        pop ebx
		pop esi
		pop edi
        ret
MemCpyAsm endp

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
