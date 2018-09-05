;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2005, American Megatrends, Inc.         **
;**                                                                  **
;**                       All Rights Reserved.                       **
;**                                                                  **
;**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
;**                                                                  **
;**                       Phone: (770)-246-8600                      **
;**                                                                  **
;**********************************************************************
;**********************************************************************
; $Header: /Alaska/SOURCE/Modules/TCG/EMpCallBin32.asm 4     4/27/11 3:03p Fredericko $
;
; $Revision: 4 $
;
; $Date: 4/27/11 3:03p $
;*************************************************************************
;Revision History
; ----------------
; $Log: /Alaska/SOURCE/Modules/TCG/EMpCallBin32.asm $
; 
; 4     4/27/11 3:03p Fredericko
; Changes for working around stack problems when calling into binaries
; that don't return properly.
; 
; 3     3/29/11 1:15p Fredericko
; 
; [TAG]        EIP 54642
; [Category] Improvement
; [Description] 1. Checkin Files related to TCG function override 
; 2. Include TCM and TPM auto detection
; [Files] Affects all TCG files
; 
; 
;**********************************************************************
;<AMI_FHDR_START>
;
; Name: EMpCallBin32.asm  
;
; Description:
;
;
;<AMI_FHDR_END>
;**********************************************************************
#if defined __GNUC_PATCHLEVEL__

#else
.686
.model  flat
.code
    INCLUDE token.equ

;Global variable for MpLoc
    public MpLoc
 MpLoc label dword
    FWORD    ?

TPMTransmitEntryStruct STRUC
    pbInBuf    DD  ? ;[IN] Pointer to input data for the data transfers to TPM
    dwInLen    DD  ? ; [IN] Length of the input data record
    pbOutBuf   DD  0 ; [OUT] Pointer to output buffer for the data from the TPM
    dwOutLen   DD  0 ; [IN/OUT] DWORD to store the length info of the
                     ; output data record.
TPMTransmitEntryStruct ENDS






;<AMI_PHDR_START>
;-----------------------------------------------------------------------------
; 
; Procedure:    Prepare2Thunkproc
; 
; Description: 
; Function to setup location of Mp Driver as a local variable
; 
; Input: 
; ecx - Location of MP Driver
; dx  - Segment for MP Driver 
;
; Output: 
; 
; Modified: 
; 
; Referrals:
; 
; Notes:      
;---------------------------------------------------------------------------
;<AMI_PHDR_END>
_Prepare2Thunkproc proc
        push     ebp
        mov      ebp, esp
        pusha
        mov      ecx, dword ptr [ebp + 08h]
        mov      edx, dword ptr [ebp + 0ch]
        mov      dword ptr [MpLoc],ecx
        mov      word  ptr [MpLoc + 4], dx
        popa
        pop      ebp
        ret
_Prepare2Thunkproc ENDP





;<AMI_PHDR_START>
;-----------------------------------------------------------------------------
; 
; Procedure:    TcgDxeCallMPDriver (TCM_support eq 0)
; 
; Description: 
; Makes a function call into MP Driver
; 
; Input: 
; rcx - Call type: 4-Buffer Transmission call
; rsi - Buffer to be tansmitted
;
; Output: 
; rax - TCG return code
; 
; Modified: 
; 
; Referrals:
; 
; Notes:      
;---------------------------------------------------------------------------
;<AMI_PHDR_END>
_TpmDxeCallMPDriver proc
        push    ebp
        mov     ebp, esp
        pusha
        mov     ecx, dword ptr [ebp + 08h]
        cmp     ecx, 04h
        jnz     Empty_Buff
        mov     esi, dword ptr [ebp + 0ch]
Empty_Buff:
        mov     eax, ecx
        call    fword ptr [MpLoc]
IF MKF_Stack_Work_Around
        pop     ebp     ;balance the stack after call
ENDIF
        popa
        pop     ebp
        ret
_TpmDxeCallMPDriver ENDP




;<AMI_PHDR_START>
;-----------------------------------------------------------------------------
; 
; Procedure:    TcgDxeCallMPDriver (TCM_support eq 1)
; 
; Description: 
; Makes a function call into MP Driver
; 
; Input: 
; rcx - Call type: 4-Buffer Transmission call
; rsi - Buffer to be tansmitted
;
; Output: 
; rax - TCG return code
; 
; Modified: 
; 
; Referrals:
; 
; Notes:      
;---------------------------------------------------------------------------
;<AMI_PHDR_END>
_TcmDxeCallMPDriver proc
        push     ebp
        mov      ebp, esp
        pusha
        mov      ecx, dword ptr [ebp + 08h]
        cmp      ecx, 04h
        jnz      Empty_Buff
        mov      esi, dword ptr [ebp + 0ch]
Empty_Buff:
        mov      eax, ecx
        call     fword ptr [MpLoc]
        popa
        pop      ebp
        ret
_TcmDxeCallMPDriver ENDP

;<AMI_PHDR_START>
;-----------------------------------------------------------------------------
; 
; Procedure:    TcgThunk32
; 
; Description: 
; Go to compatibility mode for MP driver
; 
; Input: 
;
; Output: 
; 
; Modified: 
; 
; Referrals:
; 
; Notes:      
;---------------------------------------------------------------------------
;<AMI_PHDR_END>
_TcgThunk32 proc
_TcgThunk32 ENDP





;<AMI_PHDR_START>
;-----------------------------------------------------------------------------
; 
; Procedure:    TcgThunkSwitchback
; 
; Description: 
; Switch back to 64bit mode
; 
; Input: 
;
; Output: 
; 
; Modified: 
; 
; Referrals:
; 
; Notes:      
;---------------------------------------------------------------------------
;<AMI_PHDR_END>
_TcgThunkSwitchback proc
_TcgThunkSwitchback ENDP
END
#endif