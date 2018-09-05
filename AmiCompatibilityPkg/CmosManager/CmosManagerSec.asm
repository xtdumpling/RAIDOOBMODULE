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
; Name: CmosManagerSec.asm
;
; Description:
;   This file contains all code necessary to support CMOS Manager's SEC
;   phase initialization and external services.
;
;<AMI_FHDR_END>
;*************************************************************************

;----------------------------------------------------------------------------
; Assembler build options
;----------------------------------------------------------------------------

.686P
.XMM
.MODEL SMALL
.code

INCLUDE        TOKEN.EQU

IF MKF_CMOS_SEC_SUPPORT

INCLUDE        TOKENEQU.EQU

CMOS_NO_EXTERNS equ 1
INCLUDE        CmosAccessSec.inc


; Externs
EXTERN  SEC_CmosManagerEnd:NEAR32

IF  SEC_LAST_ADDRESS_VALID
  EXTERN  LAST_ADDRESS:ABS
ENDIF

; Equates
MKF_stnd_storage_size   equ   00Fh  ; size of standard
CLOCK_STATUS_REG        EQU   MKF_CMOS_RTC_STATUS_REGISTER
BATT_BIT                EQU   080h  ; bit 8 is battery status (set == good)

MSR_XAPIC_BASE          EQU   01Bh  ; standard APIC base MSR
XAPIC_BASE_BSP_BIT      EQU   008h

CMOS_ADDR_PORT      equ MKF_CMOS_BANK0_INDEX ; standard CMOS r/w port
CMOS_DATA_PORT      equ MKF_CMOS_BANK0_DATA
CMOS_ADDR2_PORT     equ MKF_CMOS_BANK1_INDEX ; extended CMOS r/w port
CMOS_DATA2_PORT     equ MKF_CMOS_BANK1_DATA

IO_DELAY_bb    MACRO            
        jmp     $+2
        jmp     $+2
ENDM                    

;----------------------------------------------------------------------------
;   STARTUP_SEG  S E G M E N T  STARTS
;----------------------------------------------------------------------------
;STARTUP_SEG   SEGMENT   PARA PUBLIC 'CODE' USE32

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    CmosReadWrite
;
; Description:
;   Returns the CMOS value associated with the given CMOS token
;   from the actual CMOS storage. CMOS checksum is don't care. Upon detecting
;   an error, the carry flag is set and an error code is returned in AX.
;
; Input:        
;   AX - CMOS Token.
;   DL - Set for write or Clear for read
;   DH - Value to write
;
; Output:        
;   AX - CMOS value if reading
;   NC - If success
;   CY - If error & AX = Error code
;                                                                               
; Modified:        
;   AX, MM0, MM1, MM2, MM3
;   (EDX upper word and EAX upper word are unmodified)
;   All other registers are preserved 
;
; Referrals:        
;   None
;
; Notes:
;   An error code is returned in AX with Carry Flag set upon detected
;   error condition. The following error code constants (defined in 
;   CmosAccessSec.inc) are used:
;
;                   CMOS_BATTERY_ERR
;                   CMOS_BITSIZE_ERR
;                   CMOS_OVERFLOW_ERR
;                                                                               
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

CmosReadWrite PROC NEAR PUBLIC

; Token[2:0]   = Bit offset within a byte of item's start address in CMOS
; Token[11:3]  = Byte offset of item's start address in CMOS
; Token[15:12] = Size of item (in units of bits)

        movd    mm1, ebx            ; save registers
        movd    mm2, ecx
        movd    mm3, edx

        mov     bx, ax                        
        shr     bx, 12
        cmp     bx, 8               ; BX has the size in Bits
        ja      CmosReadWriteSizeError
        cmp     bx, 0
        je      CmosReadWriteSizeError

        mov     cx, ax
        and     cx, 07h             ; CX has the bit position.


        and     ax, 0FFFh             
        shr     ax, 3               ; AL has the CMOS address

;-----------------------------------------------------
; return error if battery is bad (using standard ports)
;-----------------------------------------------------

        xchg    ah, al              ; save address in AH
        mov     al, CLOCK_STATUS_REG
        or      al, MKF_CMOS_NMI_BIT_VALUE ; set value of NMI bit
        out     CMOS_ADDR_PORT, al
        io_delay_bb
        io_delay_bb
        in      al, CMOS_DATA_PORT  ; AL = battery status
        test    al, BATT_BIT        ; bit is set if OK
        .if (zero?)
            mov ax, CMOS_BATTERY_ERR ; CMOS battery is low.
            jmp CmosReadWriteError
        .endif
        xchg    ah, al              ; restore address to AL

;-----------------------------------------------------
; Create an AND mask in CX with bits set corresponding
; the destination bit field
;-----------------------------------------------------

        ror ecx, 16                 ; ECX upper = bit position
        xor cx, cx
        .while (bx > 0)
            shl cx, 1
            or  cx, 1
            dec bx
        .endw
        mov bx, cx                  ; BX = mask
        ror ecx, 16                 ; CX = bit position
        shl bx, cl                  ; BX = shifted mask
        ror ecx, 16                 ; ECX upper = bit position
        mov cx, bx                  ; CX = shifted mask

;-----------------------------------------------------
; If writing, encode the value:
;
; 1) shift the value in DH to its position,
; 2) complement the mask so that it can be used to clear
;    the destination bit field
; 3) apply/test mask to DH to ensure adjacent bits are not
;    overwritten, returning an error if there is overflow
;
; Note: error checking here will detect an error 
;       if attempted write is not within the owned
;       bits of a token.
;-----------------------------------------------------

        .if (dl == CMOS_WRITE_ACCESS)
            ror   ecx, 16           ; CX = bit position & ECX upper = mask
            movzx bx, dh            ; test upper bit overflow
            shl   bx, cl        
            or    bh, bh
            .if (!zero?)            ; error if bits are shifted into BH
                mov ax, CMOS_OVERFLOW_ERR
                jmp CmosReadWriteError
            .endif
            shl   dh, cl            ; do the shift for real
            ror   ecx, 16           ; CX = mask & ECX upper = bit position
            not   cx                ; CX = complemented mask
            test  dh, cl            ; ensure no overwriting of destination
            .if (!zero?)
                mov ax, CMOS_OVERFLOW_ERR
                jmp CmosReadWriteError
            .endif
            mov   bl, dh            ; BL = value to write                  
        .endif

;-----------------------------------------------------
; Note: EDX will contain value/access type information
;       or data/index port information as needed.
;       MM0/MM3 are used as a swap registers for this
;       purpose. 
;-----------------------------------------------------

     .if (AL < 80h)
        mov     dx, CMOS_DATA_PORT
        ror     edx, 16
        mov     dx, CMOS_ADDR_PORT
     .else
        mov     dx, CMOS_DATA2_PORT
        ror     edx, 16
        mov     dx, CMOS_ADDR2_PORT
     .endif
        movd    mm0, edx            ; save data|index in MM0
                                    ; data port in EDX upper word
                                    ; index port in DX

     ; set value of the NMI bit or
     ; translate the address for the upper bank

     .if (al < 80h)
        or      al, MKF_CMOS_NMI_BIT_VALUE
     .else
        sub     al, MKF_CMOS_BANK1_OFFSET
     .endif

;-----------------------------------------------------
; Read/write the CMOS register
;
; Note: A read always occurs. If a write is requested,
;       only the bits that are owned by the token
;       are modified and written back.
;-----------------------------------------------------


        out     dx, al              ; AL = address
        io_delay_bb
        io_delay_bb
        xchg    ah, al              ; AH contains address
        ror     edx, 16             ; DX = data port
        in      al, dx              ; AL = value from CMOS
        movd    edx, mm3            ; EDX = value|access type

        .if (dl == CMOS_WRITE_ACCESS)
            movd    edx, mm0        ; EDX = data|index ports
            xchg    ah, al          ; AL = address & AH = register value
            out     dx, al
            io_delay_bb
            io_delay_bb
            xchg    al, ah          ; AL = register value & AH = address
            and     al, cl          ; clear the destination field
            or      al, bl          ; set the value in AL
            ror     edx, 16         ; DX = data port
            out     dx, al
            clc                     ; clear error flag
            jmp     CmosReadWriteExit

;-----------------------------------------------------
; If reading, decode the value:
;
; 1) AND with mask to clear unused bits
; 2) Shift down to zero by bit position
;-----------------------------------------------------

        .else 
            and     al, cl          ; CL is shifted, uncomplemented mask
            ror     ecx, 16         ; CL = bit position & ECX upper = mask
            shr     al, cl
            movzx   ax, al          ; AX = value
            clc                     ; clear error flag
            jmp     CmosReadWriteExit
        .endif

CmosReadWriteSizeError:
        mov ax, CMOS_BITSIZE_ERR

CmosReadWriteError:
        stc                         ; set error flag

CmosReadWriteExit:
        movd    ebx, mm1            ; restore registers
        movd    ecx, mm2
        movd    edx, mm3

        jmp     edi

CmosReadWrite    ENDP



;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SEC_CmosManager
;
; Description:        
;   This is the entrypoint for CMOS Manager SEC code.
;
; Input:
;   None
;
; Output:        
;   None
;                                                                               
; Modified:        
;   None
;
; Referrals:        
;   None
;                                                                               
; Notes:
;   Currently, there are no SEC tasks for the CMOS Manager.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

SEC_CmosManager     PROC PUBLIC
        jmp     SEC_CmosManagerEnd
SEC_CmosManager     ENDP

;----------------------------------------------------------------------------
;   STARTUP_SEG  S E G M E N T  ENDS
;----------------------------------------------------------------------------
;STARTUP_SEG         ENDS

ENDIF

END

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

