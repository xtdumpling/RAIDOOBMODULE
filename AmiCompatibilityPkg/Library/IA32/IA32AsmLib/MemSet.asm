;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
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
.686p
.xmm
.model  flat
.code


;*************************************************************************
;<AMI_PHDR_START>
;
; Name: MemSet
;
; Description:
;  VOID MemSet(IN VOID *pBuffer, IN UINTN Count, IN UINT8 Value) fills Count
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
public _memset
_memset:
		mov ecx, [esp+12] ; ecx = Count
		mov al, [esp+8] ; al = Value
		jmp MemSet2
_MemSetAsm proc
		mov ecx, [esp+8] ; ecx = Count
		mov al, [esp+12] ; al = Value		
MemSet2::
		push edi
		mov edi, [esp+4+4] ; edi = pBuffer
		push [esp+4+4] ; save pBuffer
		push ebx		
		; fill EAX with the Value so that we can perform DWORD operatins
		mov ah, al
		mov bx,ax
		shl eax,16
		mov ax,bx
		; if Counter is less then 4, jump to byte copy
		cmp ecx, 4
		jb  CopyByte
		; check if the Buffer is 4-bytes aligned
		mov edx,edi
		and edx, 3
		; if the Buffer is 4-bytes aligned, jump to DWORD copy
		jz CopyDword
		; Buffer is not 4-bytes aligned
		; Calculate 4-(Buffer%4), which is a number of bytes we have to copy before
		; Buffer will reach 4-bytes boundary, and perform byte copy
		neg edx
		add edx, 4
		xchg ecx, edx
		sub edx, ecx
		rep stosb
		mov ecx, edx
CopyDword:
		; perform DWORD copy
		mov edx, ecx
		shr ecx, 2
		rep stosd
		; copy the remainder
		and edx,3
		mov ecx, edx
CopyByte:
		rep stosb
		///
        pop ebx
		pop eax		
		pop edi
		ret
_MemSetAsm endp

END
;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
