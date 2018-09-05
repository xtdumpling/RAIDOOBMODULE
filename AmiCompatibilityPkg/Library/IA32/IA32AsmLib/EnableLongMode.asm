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
; $Header: /Alaska/SOURCE/Core/Modules/IA32Core/IA32AsmLib/EnableLongMode.asm 1     10/01/10 4:56p Felixp $
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
;<AMI_PHDR_START>
;
; Name: EnableLongMode
;
; Description:
;  VOID EnableLongMode(IN VOID *PageTable, IN VOID *Function, 
; IN VOID *Parameter1, IN VOID *Parameter2) enables long mode then calls the
; provided function with the provided parameters.
;
; Input:
;  IN VOID *PageTable
; Pointer to level 4 page map.
;
;  IN VOID *Function
; Pointer to function to call.
;
;  IN VOID *Parameter1
; Parameter 1 for above function call.
;
;  IN VOID *Parameter2
; Parameter 2 for above function call.
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
EnableLongMode proc C public PageTable:DWORD, Function:DWORD, Parameter1:DWORD, Parameter2:DWORD
	mov eax, PageTable
	mov cr3, eax		;Set CR3 to first page directory pointer table

	mov eax, cr4
	or ax, 620h			;Enable PAE and XMM in case it was turned off.
	mov cr4, eax

	;Enable long mode in msr register. Doesn't actually enter long mode yet.
	mov ecx, 0c0000080h
	rdmsr
	bts eax, 8	
	wrmsr

	;Enable paging
	mov eax, cr0
	bts	eax, 31
	mov cr0, eax		;Now in long mode compatiblity.
	jmp	@f
@@:

	;jmp far segment:offset
		db 67h, 0eah
		dd offset long_mode_64
		dw 38h			;SYS_CODE64_SEL
long_mode_64:
	;in 64-bit long mode

	db 48h
	xor	 eax, eax		;xor rax, rax
	db 48h
	xor  ebx, ebx		;xor rbx, rbx
	db 48h
	xor	 ecx, ecx		;xor rcx, rcx
	db 48h
	xor	 edx, edx		;xor rdx, rdx

	mov	 ecx, Parameter1
	mov	 edx, Parameter2
	mov  ebx, Function

	mov ax, 30h			;SYS_DATA64_SEL
	mov	ds, ax
	mov es, ax
	mov ss, ax

        push    37fh
        fldcw   word ptr [esp]      ;Uses rsp. Set FP control word according UEFI
        db 48h
        add     esp, 8      ;add rsp, 8

	mov	eax, 0fffffff0h
	db 48h
	and esp, eax		;rsp must be on a 16 byte boundary. C compiler expects that.
	call ebx			;call rbx
	ret
EnableLongMode endp

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
