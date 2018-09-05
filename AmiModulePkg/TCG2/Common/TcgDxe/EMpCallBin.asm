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
;*************************************************************************
; $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgDxe/EMpCallBin.asm 1     10/08/13 12:03p Fredericko $
;
; $Revision: 1 $
;
; $Date: 10/08/13 12:03p $
;*************************************************************************
; Revision History
;<AMI_FHDR_START>
;
; Name: EMpCallBin.asm  
;
; Description:
;    Contains functions for legacy TCG calls in 32 bit Binaries 
;
;<AMI_FHDR_END>
;**********************************************************************
#if defined __GNUC_PATCHLEVEL__

#else
.code
    INCLUDE token.equ

;Global variable for MpLoc
    public MpLoc
 MpLoc label dword
    FWORD    ?

public TPMRTCSave
 TPMRTCSave label dword
    WORD    ?

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
Prepare2Thunkproc proc
        push     rax
        push     rcx
        push     rdx
        mov      dword ptr [MpLoc],ecx
        mov      word  ptr [MpLoc + 4],dx
        pop      rdx
        pop      rcx
        pop      rax
        ret
Prepare2Thunkproc ENDP




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
TpmDxeCallMPDriver proc
        push    rbp
        push    rax
        push    rcx
        push    rdx
        mov     rbp,rsp
        cmp     rcx, 04h
        jnz     Empty_Buff
        MOV     rsi,  rdx
Empty_Buff:
        mov     rax, rcx
        push    rcx
        mov     rcx, offset next
IF MKF_Stack_Work_Around
        mov     dword ptr [IP_Offset], ecx
ENDIF
IF MKF_PMBASE_RTC_WKAround
        push     rax
        push     rdx
        mov      dx, MKF_PM_BASE_ADDRESS
        in       ax, dx 
        mov      word ptr [TPMRTCSave],ax
        pop      rdx
        pop      rax
ENDIF
        pop     rcx
        call    fword ptr [MpLoc]
IF MKF_Stack_Work_Around
        db      0eah
IP_Offset:
        dd      0
        dw      038h
ENDIF
next:
IF MKF_PMBASE_RTC_WKAround
        push  rax
        push  rdx
        mov   dx,MKF_PM_BASE_ADDRESS
        in    ax, dx 
        test  ax, dx
        jz    RtcNotSet      
        mov   ax, word ptr [TPMRTCSave]
        and   ax,0FBFEh
        or    ax,0400h
        out   dx, ax
RtcNotSet:
        pop   rdx
        pop   rax
ENDIF
        mov     rsp,rbp
        pop     rdx
        pop     rcx
        pop     rax
        pop     rbp
        ret
TpmDxeCallMPDriver ENDP



TcmIoDelay proc
    push rax
    mov al, 80h
    out 080h, al
    in  al, 080h
    pop rax
    ret
TcmIoDelay ENDP

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
TcmDxeCallMPDriver proc
        push     rax
        push     rcx
        push     rdx
        push     rsi
        cmp      rcx, 04h
        jnz      Empty_Buff
        MOV      rsi,  rdx
Empty_Buff:
        mov      rax, rcx
        call    TcmIoDelay
        call     fword ptr [MpLoc]
        call    TcmIoDelay
        pop      rsi
        pop      rdx
        pop      rcx
        pop      rax
        ret
TcmDxeCallMPDriver ENDP





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
TcgThunk32 proc
TcgThunk32 ENDP





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
TcgThunkSwitchback proc
TcgThunkSwitchback ENDP

END
#endif
