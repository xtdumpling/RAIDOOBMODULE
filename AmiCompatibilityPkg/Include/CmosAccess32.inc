;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
;**                                                                  **
;**                       All Rights Reserved.                       **
;**                                                                  **
;**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
;**                                                                  **
;**                       Phone: (770)-246-8600                      **
;**                                                                  **
;**********************************************************************
;**********************************************************************
; $Header: $
;
; $Revision: $
;
; $Date: $
;
;****************************************************************************
;<AMI_FHDR_START>
;
; Name: CmosAccess32.inc
;
; Description:
;   This file contains macros, constants, procedure definitions and structure 
;   declarations to be used in 32-bit assembly code. All these methods
;   use the CMOS Access PPI for writing to CMOS. 
;
;   These macros are chipset-specific and are provided only as example
;   code.  
;
;   The NMI bit setting should conform to platform policy. (See the 
;   CMOS_MGR_SET_NMI_BIT SDL token.)
;
;<AMI_FHDR_END>
;*************************************************************************

IFNDEF  _CMOSACCESS32_INC_
_CMOSACCESS32_INC_   EQU 1

INCLUDE TokenEqu.equ
INCLUDE Token.equ

EFI_CMOS_ACCESS_INTERFACE struct
        PeiServices             DD 0 ; EFI_PEI_SERVICES **
        Read                    DD 0 ; EFI_CMOS_READ
        Write                   DD 0 ; EFI_CMOS_WRITE
        GetTokenFromRegister    DD 0 ; EFI_CMOS_GET_TOKEN_FROM_REGISTER
        ReadCmosStatusBytes     DD 0 ; EFI_CMOS_READ_STATUS_BYTES
        GetTime                 DD 0 ; EFI_CMOS_GET_DATE_TIME
        SetTime                 DD 0 ; EFI_CMOS_SET_DATE_TIME
        Oem                     DD 0 ; EFI_HANDLE *
EFI_CMOS_ACCESS_INTERFACE ends

CMOS_WRITE_ACCESS   EQU 0
CMOS_READ_ACCESS    EQU 1

CLOCK_STATUS_REG    EQU MKF_CMOS_RTC_STATUS_REGISTER
BATT_BIT            EQU 080h        ; bit 8 is battery status (set == good)
CMOS_BATTERY_ERR    EQU 012h
CMOS_BITSIZE_ERR    EQU 013h
CMOS_OVERFLOW_ERR   EQU 014h

CMOS_ADDR_PORT      equ MKF_CMOS_BANK0_INDEX ; standard CMOS r/w port
CMOS_DATA_PORT      equ MKF_CMOS_BANK0_DATA  
CMOS_ADDR2_PORT     equ MKF_CMOS_BANK1_INDEX ; extended CMOS r/w port
CMOS_DATA2_PORT     equ MKF_CMOS_BANK1_DATA 

_Cmos           TEXTEQU <(EFI_CMOS_ACCESS_INTERFACE PTR [eax])>
byte_reg_list   TEXTEQU <al aL Al AL ah Ah aH AH bl Bl bL BL bh Bh bH BH cl Cl cL CL ch Ch cH CH dl Dl dL DL dh Dh dH DH> 


;-----------------------------------------------------------------------------
; return true (1) if arg is a byte register or variable
; otherwise return 0
;-----------------------------------------------------------------------------
IS_BYTE_ARG MACRO arg
    LOCAL pos, retval

    ; test for byte register
    pos = @InStr(, %byte_reg_list, <arg>)
    IF (pos GT 0)
        retval = 1

    ; test for byte memory variable
    ELSEIF (((OPATTR(arg)) AND 00000010y) AND (SIZEOF(TYPE(arg)) eq 1))
        retval = 1

    ; otherwise a pushd should work
    ELSE
        retval = 0

    ENDIF

    EXITM %retval
ENDM


IO_DELAY    MACRO            
        jmp     $+2
        jmp     $+2
ENDM                    

IFDEF  USE_EXTERN_CMOS_PROCS
    EXTERN MaReadCmosByte           :NEAR
    EXTERN GetPointerCmos32         :NEAR
    EXTERN ReadWriteCmosToken       :NEAR C
    EXTERN ReadWriteCmosRegister    :NEAR C
ELSE

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    MaReadCmosByte
;
; Description:        
;       Reads a byte from the CMOS location specified by AL.
;
; Input:            
;       AL - CMOS location
;
; Output:            
;       AH - Value at CMOS location specified by AL
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

MaReadCmosByte        PROC NEAR PRIVATE USES EDX
        LOCAL   Index:WORD,
                Data:WORD,
                Location:BYTE
        
        mov  Location, al
        
        .if (al < 80h)                   ; set value of the NMI bit
          mov     Index, CMOS_ADDR_PORT
          mov     Data, CMOS_DATA_PORT
          or      al, MKF_CMOS_NMI_BIT_VALUE    
        .else                            ; translate address
          mov     Index, CMOS_ADDR2_PORT
          mov     Data, CMOS_DATA2_PORT
          sub     al, MKF_CMOS_BANK1_OFFSET
        .endif
        
        ; Read the value.
          
        mov     dx, Index
        out     dx, al
        jmp     short $+2
        xchg    ah, al
        mov     dx, Data
        in      al, dx
        xchg    ah, al                   ; AH = CMOS value
        mov     al, Location             ; AL = original location
        ret
MaReadCmosByte        ENDP


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    GetPointerCmos32
;
; Description:        
;       This routine returns the 32-bit value stored in CMOS where the high
;       byte starts at location specified by register AL.
;
; Input:        
;       AL =        Upper byte register (reads upper to lower bytes in 
;                   increasing register addresses) 
;
; Output:       
;       EAX = 32bit value stored in CMOS
;       CF = set if error (EAX = Error code)
;       CF = clear if success
;
; Modified:            
;       None
;
; Referrals:        
;       MaReadCmosByte
;
; Notes:            
;       This function is defined to be called from C or assembly.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

GetPointerCmos32    PROC NEAR  PRIVATE  USES EBX
        LOCAL   ApiPointer:PTR DWORD, 
                LastByteRegister:BYTE
				
;-----------------------------------------------------
; return error if battery is bad
; (using clock range ports)
;-----------------------------------------------------

        push    eax
        mov     al, CLOCK_STATUS_REG
        call    MaReadCmosByte
        test    ah, BATT_BIT        ; bit is set if OK
        pop     eax
        .if (zero?)
            stc
            mov eax, CMOS_BATTERY_ERR ; CMOS battery is bad.
            jmp GetPointerCmos32Exit
        .endif

;-----------------------------------------------------
; No error, so get the API pointer
;-----------------------------------------------------

        mov     ApiPointer, 0
        mov     LastByteRegister, MKF_CMOS_ACCESS_API_BYTE3
        add     LastByteRegister, 3

        mov     al, MKF_CMOS_ACCESS_API_BYTE3
        .while (al <= LastByteRegister)
            call    MaReadCmosByte          
            movzx   ebx, ah         ; ah = current byte ApiPointer
            or      ApiPointer, ebx ; set current byte in ApiPointer
            inc     al
            .if (al <= LastByteRegister)
                shl ApiPointer, 8
            .endif
        .endw
        clc
        mov     eax, ApiPointer     ; return the 32-bit ApiPointer in eax

GetPointerCmos32Exit:
        ret

GetPointerCmos32    ENDP


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: ReadWriteCmosRegister
;
; Description:  
;       This function calls EFI_CMOS_ACCESS_INTERFACE.Write or 
;       EFI_CMOS_ACCESS_INTERFACE.Read to write or read a CMOS value to
;       a CMOS register.
;
; Input:   
;       AccessType - CMOS_WRITE_ACCESS or CMOS_READ_ACCESS (equate)
;     CmosRegister - the CMOS register to write (this is not a CMOS token)
;        CmosValue - value to be written or the return value of the read
;
;
; Output:       
;             eax - CMOS_BATTERY_ERR if battery is bad
;             eax - EFI error code if bit 31 is set
;      carry flag - clear on success or set on error
;
; Notes:
;
;       Caller must ensure the register has been reserved & specified properly
;       in SSP. 
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

ReadWriteCmosRegister   PROC NEAR32 C PRIVATE   AccessType:DWORD, 
                                                CmosRegister:DWORD, 
                                                CmosValue:DWORD
    LOCAL   Cmos:DWORD

;    AccessType: WriteType = 0
;                ReadType = 1


; Cmos = GetPointerCmos32();

        call    GetPointerCmos32
        jc      @F                  ; carry flag set on error
        mov     Cmos, eax    

; return Cmos->Read( Cmos,  
;                    Cmos->GetTokenFromRegister(Cmos, CmosRegister),
;                    CmosValuePtr );
; (or)
;
; return Cmos->Write( Cmos,  
;                     Cmos->GetTokenFromRegister(Cmos, CmosRegister),
;                     CmosValue );

        push    CmosValue
        push    CmosRegister
        push    Cmos                     
        mov     eax, Cmos
        call    _Cmos.GetTokenFromRegister
        add     esp, 2*4            ; pop CmosRegister & Cmos
        push    eax                 ; eax = CmosToken
        push    Cmos
        mov     eax, Cmos
        .if (AccessType == CMOS_READ_ACCESS)
            call    _Cmos.Read
        .else 
            call    _Cmos.Write
        .endif
        add     esp, 3*4            ; pop 3 args
        clc                         ; clear error flag

@@:

; EAX - CMOS_BATTERY_ERR if battery is bad
; EAX - EFI error code if bit 31 is set

        ret

ReadWriteCmosRegister   ENDP



;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: ReadWriteCmosToken
;
; Description:  
;       This function calls EFI_CMOS_ACCESS_INTERFACE.Write or 
;       EFI_CMOS_ACCESS_INTERFACE.Read to write or read a CMOS value to
;       a CMOS register.
;
; Input:   
;       AccessType - CMOS_WRITE_ACCESS or CMOS_READ_ACCESS (equate)
;        CmosToken - the encoded CMOS token (not a CMOS register)
;        CmosValue - value to be written or the return value of the read
;
; Output:       
;             eax - CMOS_BATTERY_ERR if battery is bad
;             eax - EFI error code if bit 31 is set
;
; Notes:
;
;       Caller must ensure the register has been reserved & specified properly
;       in SSP. 
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

ReadWriteCmosToken   PROC NEAR32 C PRIVATE  AccessType:DWORD, 
                                            CmosToken:DWORD, 
                                            CmosValue:DWORD
    LOCAL   Cmos:DWORD

;    AccessType: WriteType = 0
;                ReadType = 1


; Cmos = GetPointerCmos32();

        call    GetPointerCmos32
        jc      @F                  ; carry flag set on error
        mov     Cmos, eax    

; return Cmos->Read( Cmos,  
;                    CmosToken,
;                    CmosValue );
; (or)
;
; return Cmos->Write( Cmos,  
;                     CmosToken,
;                     CmosValue );

        push    CmosValue
        push    CmosToken
        push    Cmos                     
        mov     eax, Cmos
        .if (AccessType == 1)
            call    _Cmos.Read
        .else 
            call    _Cmos.Write
        .endif
        add     esp, 3*4            ; pop 3 args
        clc                         ; clear error flag

@@:

; EAX - CMOS_BATTERY_ERR if battery is bad
; EAX - EFI error code if bit 31 is set

        ret

ReadWriteCmosToken   ENDP

ENDIF

;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
;                   Start macro definitions
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: READ_CMOS_TOKEN
;
; Description:  
;       This macro invokes ReadWriteCmosToken() to read the CMOS location 
;       associated with CmosToken into the value byte at the address 
;       specified by CmosValuePtr, after pushing the required arguments onto 
;       the stack.
;
; Input:       
;       CmosToken - valid CMOS token as defined in TokenEqu.equ
;
; Output:       
;       CmosValuePtr -  the byte value at this address is updated upon 
;                               successful execution
;       eax - CMOS_BATTERY_ERR if battery is bad
;       eax - EFI error code if bit 31 is set
;
; Notes:
;
;       Example call:
;
;                   lea   ecx, CmosValue
;                   READ_CMOS_TOKEN   CMOS_S3_COUNT_LOC, ecx
;                   .if (eax != 0)
;                       ; handle error
;                   .endif
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

READ_CMOS_TOKEN   MACRO       CmosToken:REQ, ; SSP encoded CMOS Token
                            CmosValuePtr:REQ ; Address of byte to update

        push    CmosValuePtr            ; push byte value's address
        movzx   eax, CmosToken
        push    eax
        mov     eax, CMOS_READ_ACCESS
        push    eax
        call    ReadWriteCmosToken
        add     sp, 3*4                 ; Pop 3 args @ 4 bytes each

; EAX - CMOS_BATTERY_ERR if battery is bad
; EAX - EFI error code if bit 31 is set

ENDM


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: READ_CMOS_PPI
;
; Description:  
;       This macro calls EFI_CMOS_ACCESS_INTERFACE.Read() to read the CMOS 
;       location associated with CmosToken into the value byte at the address  
;       specified by CmosValuePtr, after pushing the required arguments onto  
;       the stack.
;
; Input:       
;       Cmos - pointer to an instance of EFI_CMOS_ACCESS_INTERFACE
;       CmosToken - valid CMOS Token as defined in TokenEqu.equ
;
; Output:       
;       CmosValuePtr 
;                   -  the byte value at this address is updated upon 
;                      successful execution
;       eax - EFI error code if bit 31 is set
;
; Modified:   
;       mm0 - destroyed
;
; Notes:
;
;       Example usage:
;
;                   lea   ecx, CmosValue
;                   READ_CMOS_PPI   CmosPpiPtr, CMOS_S3_COUNT_LOC, ecx
;                   .if (eax != 0)
;                       ; handle error
;                   .endif
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

READ_CMOS_PPI   MACRO   Cmos:REQ,           ; PPI Pointer
                        CmosToken:REQ,      ; SSP encoded CMOS Token
                        CmosValuePtr:REQ    ; Address of byte to update

; return Cmos->Read(  Cmos,  
;                     CmosToken,
;                     CmosValuePtr );
        push    CmosValuePtr
        ;pushd   CmosToken
        movzx   eax, CmosToken
        push    eax
        push    Cmos                     
        mov     eax, Cmos
        call    _Cmos.Read
        add     esp, 3*4            ; pop 3 args

; EAX - EFI error code if error or zero if success

ENDM



;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: READ_CMOS_REGISTER
;
; Description:  
;       This macro calls ReadWriteCmosRegister to read the physical CMOS 
;       register specified by CmosRegister into the value byte at the address
;       specified by CmosValuePtr, after pushing the required arguments onto 
;       the stack.
;
; Input:       
;       CmosRegister - the physical CMOS register to read (not a CMOS token)
;
; Output:       
;       CmosValuePtr - the byte value at this address is updated upon 
;                              successful execution
;       eax - EFI_STATUS         
;
; Notes:
;
;       Caller must ensure the entire 8-bits of the register have been 
;       reserved properly/explicitly in SSP.
;
;       Example call:
;
;                   lea   ecx, CmosValue
;                   READ_CMOS_REGISTER   CMOS_S3_COUNT_LOC_REG, ecx
;                   .if (eax != 0)
;                       ; handle error
;                   .endif
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

READ_CMOS_REGISTER   MACRO  CmosRegister:REQ, ; CMOS register address
                            CmosValuePtr:REQ  ; Address of byte to update

        pushd   CmosValuePtr            ; push the address of the value
        IF IS_BYTE_ARG(CmosRegister) eq 1
            movd    mm0, eax            ; save eax
            mov     al, CmosRegister
            and     eax, 0FFh           ; clear all but lower byte
            push    eax
            movd    eax, mm0            ; restore eax
        ELSE
            ;pushd    CmosRegister
            movzx   eax, CmosRegister
            pushd   eax
        ENDIF
        ;pushd   CMOS_READ_ACCESS
        mov     eax, CMOS_READ_ACCESS
        push    eax
        call    ReadWriteCmosRegister
        add     esp, 3*4                ; Pop 3 Read() args @ 4 bytes each

; EAX - CMOS_BATTERY_ERR if battery is bad
; EAX - EFI error code if bit 31 is set

ENDM


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: WRITE_CMOS_TOKEN
;
; Description:  
;       This macro invokes ReadWriteCmosToken() to write the byte specified
;       by CmosValue to the CMOS location associated with CmosToken, 
;       after pushing the required arguments onto the stack.
;
; Input:       
;       CmosToken - valid CMOS Token as defined in TokenEqu.equ
;       CmosValue - Byte value to write to the location associated with 
;                           CmosToken
;
; Output:       
;       eax - EFI_STATUS         
;
; Modified:   
;       mm0 - destroyed (used as temporary register if needed)
;
; Notes:
;
;       Example usage:
;
;                   mov   cl, TokenValue
;                   WRITE_CMOS_TOKEN   CMOS_S3_COUNT_LOC, cl
;                   .if (eax != 0)
;                       ; handle error
;                   .endif
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

WRITE_CMOS_TOKEN   MACRO    CmosToken:REQ, ; SSP encoded CMOS Token
                            CmosValue:REQ  ; Byte value to write

        IF IS_BYTE_ARG(CmosValue) eq 1
            movd    mm0, eax            ; save eax
            mov     al, CmosValue
            and     eax, 0FFh           ; clear all but lower byte
            push    eax
            movd    eax, mm0            ; restore eax
        ELSE
            ;pushd    CmosValue
            movzx   eax, CmosValue
            push    eax
        ENDIF
        ;pushd   CmosToken               ; push CmosToken value
        movzx   eax, CmosToken
        push    eax
        ;pushd   CMOS_WRITE_ACCESS
        mov     eax, CMOS_WRITE_ACCESS
        push    eax
        call    ReadWriteCmosToken
        add     sp, 3*4                 ; Pop 3 Write() args @ 4 bytes each

; EAX - CMOS_BATTERY_ERR if battery is bad
; EAX - EFI error code if bit 31 is set

ENDM


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: WRITE_CMOS_PPI
;
; Description:  
;       This macro calls EFI_CMOS_ACCESS_INTERFACE.Write() to write the 
;       the value specified by CmosValue to the CMOS location associated with
;       CmosToken, after pushing the required arguments onto the stack.
;
; Input:       
;       Cmos - pointer to an instance of EFI_CMOS_ACCESS_INTERFACE
;       CmosToken - valid CMOS Token as defined in TokenEqu.equ
;       CmosValue - Byte value to write to the location associated with 
;                           CmosToken
;
; Output:       
;       eax - EFI_STATUS         
;
; Modified:   
;       mm0 - destroyed (used as temporary register if needed)
;
; Notes:
;
;       Example usage:
;
;                   mov   cl, CmosTokenValue
;                   WRITE_CMOS_PPI   CmosPpiPtr, CMOS_S3_COUNT_LOC, cl
;                   .if (eax != 0)
;                       ; handle error
;                   .endif
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

WRITE_CMOS_PPI   MACRO  Cmos:REQ,       ; PPI Pointer
                        CmosToken:REQ,  ; SSP encoded CMOS Token
                        CmosValue:REQ   ; Byte Value to write

; return Cmos->Write( Cmos,
;                     CmosToken,
;                     CmosValue );

        IF IS_BYTE_ARG(CmosValue) eq 1
            movd    mm0, eax            ; save eax
            mov     al, CmosValue
            and     eax, 0FFh           ; clear all but lower byte
            push    eax
            movd    eax, mm0            ; restore eax
        ELSE
            ;pushd    CmosValue
            movzx   eax, CmosValue
            push    eax
        ENDIF
        ;pushd   CmosToken
        movzx   eax, CmosToken
        push    eax
        push    Cmos 
        mov     eax, Cmos
        call    _Cmos.Write
        add     esp, 3*4            ; pop 3 args

; EAX - EFI error code if error or zero if success

ENDM


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Name: WRITE_CMOS_REGISTER
;
; Description:  
;       This macro calls ReadWriteCmosRegister to write the value specified
;       by CmosValue to the physical CMOS location specified by CmosRegister
;       after pushing the required arguments onto the stack.
;
; Input:       
;       CmosRegister - the physical CMOS register to write (not a CMOS token)
;       CmosValue - byte value to write to CmosRegister
;
; Output:       
;       eax - EFI_STATUS
;
; Modified:   
;       mm0 - destroyed
;
; Notes:
;       Caller must ensure the entire 8-bits of the register have been 
;       reserved properly/explicitly in SSP.
;
;       Example usage:
;
;                   mov   cl, CmosValue
;                   WRITE_CMOS_REGISTER   S3_COUNT_LOC_REG, cl
;                   .if (eax != 0)
;                       ; handle error
;                   .endif
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

WRITE_CMOS_REGISTER   MACRO     CmosRegister:REQ, ; physical CMOS register
                                CmosValue:REQ     ; Byte value to write
        IF IS_BYTE_ARG(CmosValue) eq 1
            movd    mm0, eax            ; save eax
            mov     al, CmosValue
            and     eax, 0FFh           ; clear all but lower byte
            push    eax
            movd    eax, mm0            ; restore eax
        ELSE
            ;pushd    CmosValue
            movzx   eax, CmosValue
            push    eax
        ENDIF
        IF IS_BYTE_ARG(CmosRegister) eq 1
            movd    mm0, eax            ; save eax
            mov     al, CmosRegister
            and     eax, 0FFh           ; clear all but lower byte
            push    eax
            movd    eax, mm0            ; restore eax
        ELSE
            ;pushd    CmosRegister
            movzx   eax, CmosRegister
            push    eax
        ENDIF
        ;pushd   CMOS_WRITE_ACCESS
        mov     eax, CMOS_WRITE_ACCESS
        push    eax
        call    ReadWriteCmosRegister
        add     sp, 3*4                 ; Pop 3 args @ 4 bytes each

; EAX - CMOS_BATTERY_ERR if battery is bad
; EAX - EFI error code if bit 31 is set
 
ENDM


ENDIF

;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
;**                                                                  **
;**                       All Rights Reserved.                       **
;**                                                                  **
;**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
;**                                                                  **
;**                       Phone: (770)-246-8600                      **
;**                                                                  **
;**********************************************************************
;**********************************************************************
