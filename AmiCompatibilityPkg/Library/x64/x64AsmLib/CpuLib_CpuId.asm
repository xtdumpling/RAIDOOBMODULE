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
; $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64AsmLib/CPULib_CpuID.asm 2     11/11/11 3:40p Artems $
;
; $Revision: 2 $
;
; $Date: 11/11/11 3:40p $
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
; Name: CPULib_CpuID
;
; Description:
;  VOID CPULib_CpuID(IN UINT32 CpuIDIndex, IN UINT32 *pRegEAX,
; IN UINT32 *pRegEBX, IN UINT32 *pRegECX, IN UINT32 *pRegEDX) issues the
; CPUID instruction with the index provided and returns the register values.
;
; Input:
;  IN UINT32 CpuIDIndex
; 32-bit CPUID index.
;
;  IN UINT32 *pRegEAX
; Value of EAX after CPUID instruction.
;
;  IN UINT32 *pRegEBX
; Value of EBX after CPUID instruction.
;
;  IN UINT32 *pRegECX
; Value of ECX after CPUID instruction.
;
;  IN UINT32 *pRegEDX
; Value of EDX after CPUID instruction.
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
CPULib_CpuID proc
;rcx = CpuIDIndex
;rdx = pRegEax
;r8  = pRegEBX
;r9  = pRegECX
;[rsp + 8] = pRegEDX:DWORD

    push rbx
    mov r11, [rsp + 30h]    ;pRegEDX
    mov r10, r9             ;pRegECX
    mov r9,  r8             ;pRegEBX
    mov r8,  rdx            ;pRegEAX

	mov eax, ecx
	mov ecx, [r10]
    cpuid

    or  r8, r8
    jz  skip1
    mov [r8], eax
skip1:
    or  r9, r9
    jz  skip2
    mov [r9], ebx
skip2:
    or  r10, r10
    jz  skip3
    mov [r10], ecx
skip3:
    or  r11, r11
    jz  skip4
    mov [r11], edx
skip4:
    pop rbx
	ret
CPULib_CpuID endp

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
