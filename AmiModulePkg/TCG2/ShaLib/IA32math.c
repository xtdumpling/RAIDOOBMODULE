//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
#include <Uefi.h>
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
    return MultU64x32(Value64, Value32);
}




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
_aullshr( IN UINT64 Value, IN UINT8 Shift )
{
    //return ARShiftU64(Value, Shift);
    _asm
    {
        xor    ebx, ebx
        test cl, 32
        cmovnz   eax, edx
        cmovnz   edx, ebx
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
//  Value64  long value to be shifted
//  Value8   number of bits to shift by
//
// Output:      UINT64 - 64bit product of multiplier and multiplicand
//
//**********************************************************************
//<AMI_PHDR_END>

_allshl( IN UINT64 Value, IN UINT8 Shift )
{
    _asm
    {
        xor    ebx, ebx
        test cl, 32
        cmovz   eax, edx
        cmovz   edx, ebx
        shld    edx, eax, cl
        shl     eax, cl
    }
}


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
    UINTN    *Remainder=0;
    return DivU64x32Remainder(Dividend,Divisor,Remainder);
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
