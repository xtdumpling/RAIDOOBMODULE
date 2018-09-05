//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: IA32math.c -  64-bit Math Functions to patch up 32bit MS compiler
//                      
//
// Description:     The 32-bit versions of C compiler generate calls to library routines
//                  to handle 64-bit math. These functions use non-standard calling conventions.
//                  _allmul(IA32 use)
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Library/BaseLib.h>

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  _allmul
//
// Description:    64x32 bit multiplication
//              long multiply routine
//              Does a long multiply (same for signed/unsigned)
//
// Input:
//  Value64  multiplier (QWORD)
//  Value32  multiplicand (QWORD)
//
// Output:      UINT64 - 64bit product of multiplier and multiplicand
//
//**********************************************************************
//<AMI_PHDR_END>
UINT64 _allmul(IN UINT64 Value64,IN UINTN Value32)
{
//    return Mul64(Value64, Value32);
    return MultU64x32(Value64, Value32);
}
/*
 * Multiplies a 64-bit signed or unsigned value by a 64-bit signed or unsigned value
 * and returns a 64-bit result.
 */
/*
__declspec(naked) void __cdecl _allmul (void)
{
  //
  // Wrapper Implementation over EDKII MultS64x64() routine
  //    INT64
  //    EFIAPI
  //    MultS64x64 (
  //      IN      INT64      Multiplicand,
  //      IN      INT64      Multiplier
  //      )
  //
  _asm {
    ; Original local stack when calling _allmul
    ;               -----------------
    ;               |               |
    ;               |---------------|
    ;               |               |
    ;               |--Multiplier --|
    ;               |               |
    ;               |---------------|
    ;               |               |
    ;               |--Multiplicand-|
    ;               |               |
    ;               |---------------|
    ;               |  ReturnAddr** |
    ;       ESP---->|---------------|
    ;

    ;
    ; Set up the local stack for Multiplicand parameter
    ;
    mov  eax, [esp + 16]
    push eax
    mov  eax, [esp + 16]
    push eax

    ;
    ; Set up the local stack for Multiplier parameter
    ;
    mov  eax, [esp + 16]
    push eax
    mov  eax, [esp + 16]
    push eax

    ;
    ; Call native MulS64x64 of BaseLib
    ;
    call MultS64x64

    ;
    ; Adjust stack
    ;
    add  esp, 16

    ret  16
  }
} 
*/
//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  _aullshr
//
// Description:    Does a Long Shift Right (signed and unsigned are identical)
//              Shifts a long right any number of bits.
//
// Input:
//          EDX:EAX - long value to be shifted
//          CL    - number of bits to shift by
//
// Output:     
//          EDX:EAX - shifted value
//
//**********************************************************************
//<AMI_PHDR_END>
//__declspec(naked) void __cdecl _aullshr(void)
_aullshr( IN UINT64 Value, IN UINT8 Shift )
{
//    return Shr64(Value, Shift);
    _asm {
        xor     ebx, ebx
        test    cl, 32
        ; Handle shifting of 32-63 bits
        cmovnz  eax, edx
        cmovnz  edx, ebx

        shrd    eax, edx, cl
        shr     edx, cl
    }
}

/*
 * Shifts a 64-bit unsigned value right by a certain number of bits.
 */
/*
__declspec(naked) void __cdecl _aullshr (void)
{
  _asm {
    ;
    ; Checking: Only handle 64bit shifting or more
    ;
    cmp     cl, 64
    jae     _Exit

    ;
    ; Handle shifting between 0 and 31 bits
    ;
    cmp     cl, 32
    jae     More32
    shrd    eax, edx, cl
    shr     edx, cl
    ret

    ;
    ; Handle shifting of 32-63 bits
    ;
More32:
    mov     eax, edx
    xor     edx, edx
    and     cl, 31
    shr     eax, cl
    ret

    ;
    ; Invalid number (less then 32bits), return 0
    ;
_Exit:
    xor     eax, eax
    xor     edx, edx
    ret
  }
}
*/
//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  _allshl
//
// Description:    Does a Long Shift Left (signed and unsigned are identical)
//              Shifts a long left any number of bits.
//
// Input:
//         EDX:EAX - long value to be shifted
//         CL      - number of bits to shift by
//
// Output: 
//         EDX:EAX - shifted value
//
//**********************************************************************
//<AMI_PHDR_END>
//__declspec(naked) void __cdecl _allshl(void)
_allshl( IN UINT64 Value, IN UINT8 Shift )
{
    _asm {
        xor    ebx, ebx
        test   cl, 32
        ; Handle shifting of 32-63 bits
        cmovnz  edx, eax
        cmovnz  eax, ebx

        shld    edx, eax, cl
        shl     eax, cl
    }
}
/*
    __asm {
;
; Handle shifts of 64 or more bits (all get 0)
;
        cmp     cl, 64
        jae     short RETZERO

;
; Handle shifts of between 0 and 31 bits
;
        cmp     cl, 32
        jae     short MORE32
        shld    edx,eax,cl
        shl     eax,cl
        ret

;
; Handle shifts of between 32 and 63 bits
;
MORE32:
        mov     edx,eax
        xor     eax,eax
        and     cl,31
        shl     edx,cl
        ret

;
; return 0 in edx:eax
;
RETZERO:
        xor     eax,eax
        xor     edx,edx
        ret
    }
 */    

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  _aulldiv
//
// Description:    Does a unsigned long divide of the arguments.
//
// Input:
//  Value64  dividend (QWORD)
//  Value64  divisor (QWORD)
//
// Output:      UINT64 - 64bit product of multiplier and multiplicand
//
//**********************************************************************
//<AMI_PHDR_END>
UINT64 _aulldiv(IN UINT64 Dividend, IN UINTN Divisor)    //Can only be 31 bits for IA-32
{
//    UINTN    *Remainder=0;
//    return Div64(Dividend, Divisor, Remainder);
////    return DivU64x64Remainder(Dividend, Divisor, Remainder);
    return DivU64x32(Dividend, Divisor);
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
