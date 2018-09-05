/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"


/**

  Finds greatest common denominator

  @param x - First Value
  @param y - Second Value

  @retval Greatest common denominator

**/
UINT32
GetGCD (
       UINT32 x,
       UINT32 y
       )
{
  UINT32 t;

  do {
    if (x < y) {
      t = x;
      x = y;
      y = t;
    }
    x -= y;
  } while (x);

  return y;
}

/**

  Finds larger of two values

  @param x - First value
  @param y - Second value

  @retval largest of two values

**/
UINT32
MaxVal (
       UINT32 x,
       UINT32 y
       )
{
  UINT32 t;

  if (x < y) t = y;
  else t = x;
  return t;
}

/**

  Finds smaller of two values

  @param x - First value
  @param y - Second value

  @retval smallest of two values

**/
UINT32
MinVal (
       UINT32 x,
       UINT32 y
       )
{
  UINT32 t;

  if (x < y) t = x;
  else t = y;
  return t;
}

/**

  Multiplies two 32 bit values

  @param dw1 - first 32 bit value
  @param dw2 - second 32 bit value

  @retval 64 bit value resulting from the multiplication of two given values

**/
UINT64_STRUCT
MulDw64 (
        UINT32 dw1,
        UINT32 dw2
        )
{
  UINT64_STRUCT retVal;
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov   %2, %%eax\n\t"
    "mov   %3, %%ecx\n\t"
    "mul   %%ecx\n\t"
    "mov   %%edx, %0\n\t"
    "mov   %%eax, %1\n\t"
    : "=m" (retVal.hi), "=m" (retVal.lo)
    : "m" (dw1), "m" (dw2)
    :
  );
#else
  _asm
  {
    mov   eax, dw1
    mov   ecx, dw2
    mul   ecx
    mov   retVal.hi, edx
    mov   retVal.lo, eax
  }
#endif
#else
  {
    UINT64 value = 0;
    value = dw1 * dw2;
    retVal.lo = (UINT32)value;
    retVal.hi = (UINT32)(value >> 32);
  }
#endif

  return retVal;
}

/**

  divides 64 bit value by 2

  @param qw1 - 64 bit value to divide

  @retval result of qw1 / 2

**/
UINT64_STRUCT
DivBy2Qw64 (
           UINT64_STRUCT qw1
           )
{
  UINT64_STRUCT retVal;
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov   %2, %%eax\n\t"
    "shr   $1, %%eax\n\t"
    "mov   %%eax, %0\n\t"
    "mov   %3, %%eax\n\t"
    "rcr   $1, %%eax\n\t"
    "mov   %%eax, %1\n\t"
    : "=m" (retVal.hi), "=m" (retVal.lo)
    : "m" (qw1. hi), "m" (qw1.lo)
    :
  );
#else
  _asm
  {
    mov   eax, qw1.hi
    shr   eax, 1
    mov   retVal.hi, eax
    mov   eax, qw1.lo
    rcr   eax, 1
    mov   retVal.lo, eax
  }
#endif
#else
  {
    UINT64 value = 0;
    value = (UINT64)((UINT64)qw1.hi << 32) | qw1.lo;
    value /= 2;
    retVal.lo = (UINT32)value;
    retVal.hi = (UINT32)(value >> 32);
  }
#endif

  return retVal;
}

/**

  This routine allows a 64 bit value to be left shifted by 32 bits and returns
  the shifted value. Count is valid up 63. (Only Bits 0-5 is valid for Count)

  @param Data  - value to be shifted
  @param Count - number of time to shift

  @retval Value shifted left identified by the Count.

**/
UINT64_STRUCT
LShiftUINT64 (
             UINT64_STRUCT Data,
             UINT32        Count
             )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "push   %%ecx\n\t"

    "mov    %2, %%eax\n\t"
    "mov    %3, %%edx\n\t"

    // CL is valid from 0 - 31. shld will move EDX:EAX by CL times but EAX is not touched
    // For CL of 32 - 63, it will be shifted 0 - 31 so we will move eax to edx later.
    "mov    %4, %%ecx\n\t"
    "and    $63, %%ecx\n\t"
    "shld   %%cl, %%edx, %%eax\n\t"
    "shl    %%cl, %%eax\n\t"

    // Since Count is 32 - 63, eax will have been shifted  by 0 - 31
    // If shifted by 32 or more, set lower 32 bits to zero.
    "cmp    $32, %%ecx\n\t"
    "jc     _LShiftUINT64_Done\n\t"

    "mov    %%eax, %%edx\n\t"
    "xor    %%eax, %%eax\n\t"

    "_LShiftUINT64_Done:\n\t"
    "pop    %%ecx\n\t"
    "mov    %%eax, %0\n\t"
    "mov    %%edx, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Data.lo), "m" (Data.hi), "m" (Count)
    :
  );
#else
  _asm
  {
    push   ecx

    mov    eax, dword ptr Data.lo
    mov    edx, dword ptr Data.hi

    ;
    ; CL is valid from 0 - 31. shld will move EDX:EAX by CL times but EAX is not touched
    ; For CL of 32 - 63, it will be shifted 0 - 31 so we will move eax to edx later.
    ;
    mov    ecx, Count
    and    ecx, 63
    shld   edx, eax, cl
    shl    eax, cl

    ;
    ; Since Count is 32 - 63, eax will have been shifted  by 0 - 31
    ; If shifted by 32 or more, set lower 32 bits to zero.
    ;
    cmp    ecx, 32
    jc     short _LShiftUINT64_Done

    mov    edx, eax
    xor    eax, eax

    _LShiftUINT64_Done:
    pop    ecx
    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, edx
  }
#endif
#else
  {
    UINT64 value = 0;

    value = (UINT64)((UINT64)Data.hi << 32) | Data.lo;
    value = value << Count;
    RetData.lo = (UINT32)value;
    RetData.hi = (UINT32)(value >> 32);
  }
#endif
  return RetData;
}

/**

  This routine allows a 64 bit value to be multiplied with a 32 bit
  value returns 64bit result. No checking if the result is greater than 64bits.

  @param Multiplicand  - Multiplicand
  @param Multiplier    - what to multiply by

  @retval result of Multiplicand * Multiplier

**/
UINT64_STRUCT
MultUINT64x32 (
              UINT64_STRUCT Multiplicand,
              UINT32        Multiplier
              )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "movl   %2, %%eax\n\t"
    "mull   %4\n\t"
    "push   %%eax\n\t"
    "push   %%edx\n\t"
    "movl   %3, %%eax\n\t"
    "mull   %4\n\t"
    // The value in edx stored by second multiplication overflows
    // the output and should be discarded. So here we overwrite it
    // with the edx value of first multiplication.
    "pop    %%edx\n\t"
    "addl   %%eax, %%edx\n\t"
    "pop    %%eax\n\t"
    "movl   %%eax, %0\n\t"
    "movl   %%eax, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Multiplicand.lo), "m" (Multiplicand.hi), "m" (Multiplier)
    :
  );
#else
  _asm
  {
    mov    eax, dword ptr Multiplicand.lo
    mul    Multiplier
    push   eax
    push   edx
    mov    eax, dword ptr Multiplicand.hi
    mul    Multiplier
    ;
    ; The value in edx stored by second multiplication overflows
    ; the output and should be discarded. So here we overwrite it
    ; with the edx value of first multiplication.
    ;
    pop    edx
    add    edx, eax
    pop    eax
    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, edx
  }
#endif
#else
  {
    UINT64 value = 0;
    value = (UINT64)((UINT64)Multiplicand.hi << 32) | Multiplicand.lo;
    value = value * Multiplier;
    RetData.lo = (UINT32)value;
    RetData.hi = (UINT32)(value >> 32);
  }
#endif
  return RetData;
}

/**

  This routine allows a 64 bit value to be divided with a 32 bit value returns
  64bit result and the Remainder.

  @param Dividend  - Value to divide into
  @param Divisor   - Value to divide by
  @param Remainder - Pointer to location to write remainder

  @retval Remainder of Dividend / Divisor

**/
UINT64_STRUCT
DivUINT64x32 (
             UINT64_STRUCT Dividend,
             UINT32        Divisor,
             UINT32        *Remainder
             )
{
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "push   %%ecx\n\t"
    // let edx contain the intermediate result of remainder
    "xor    %%edx, %%edx\n\t"
    "mov   $64, %%ecx\n\t"

    "_DivUINT64x32_Wend:\n\t"
    "shll   $1, %0\n\t"
    "rcll   $1, %1\n\t"
    "rcl    $1, %%edx\n\t"

    // If intermediate result of remainder is larger than
    // or equal to divisor, then set the lowest bit of dividend,
    // and subtract divisor from intermediate remainder
    "cmp    %4, %%edx\n\t"
    "jb     _DivUINT64x32_Cont\n\t"
    "bts    $0, %0\n\t"
    "sub    %4, %%edx\n\t"

    "_DivUINT64x32_Cont:\n\t"
    "loop   _DivUINT64x32_Wend\n\t"

    "cmp    $0, %5\n\t"
    "je     _DivUINT64x32_Done\n\t"
    "mov    %5, %%eax\n\t"
    "mov    %%edx, (%%eax)\n\t"

    "_DivUINT64x32_Done:\n\t"
    "pop    %%ecx\n\t"
    : "=m" (Dividend.lo), "=m" (Dividend.hi)
    : "m" (Dividend.lo), "m" (Dividend.hi), "m" (Divisor), "m" (Remainder)
    :
  );
#else
  _asm
  {
    push   ecx
    ;
    ; let edx contain the intermediate result of remainder
    ;
    xor    edx, edx
    mov    ecx, 64

    _DivUINT64x32_Wend:
    shl    dword ptr Dividend.lo, 1
    rcl    dword ptr Dividend.hi, 1
    rcl    edx, 1

    ;
    ; If intermediate result of remainder is larger than
    ; or equal to divisor, then set the lowest bit of dividend,
    ; and subtract divisor from intermediate remainder
    ;
    cmp    edx, Divisor
    jb     _DivUINT64x32_Cont
    bts    dword ptr Dividend.lo, 0
    sub    edx, Divisor

    _DivUINT64x32_Cont:
    loop   _DivUINT64x32_Wend

    cmp    Remainder, 0
    je     _DivUINT64x32_Done
    mov    eax, Remainder
    mov    dword ptr [eax], edx

    _DivUINT64x32_Done:
    pop    ecx
  }
#endif
#else
  {
    UINT64 value = 0;
    value = (UINT64)((UINT64)Dividend.hi << 32) | Dividend.lo;
    *Remainder = value % Divisor;
    value = value / Divisor;
    Dividend.lo = (UINT32)value;
    Dividend.hi = (UINT32)(value >> 32);
  }
#endif

  return Dividend;
}

/**

  Calculates the largest integer that is both
  a power of two and less than Input

  @param Input - value to calculate power of two

  @retval The largest integer that is both  a power of two and less than Input

**/
UINT64_STRUCT
GetPowerOfTwo (
              UINT64_STRUCT Input
              )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "xor     %%eax, %%eax\n\t"
    "mov     %%eax, %%edx\n\t"
    "mov     %2, %%ecx\n\t"
    "jecxz   _GetPowerOfTwo_Next\n\t"
    "bsr     %%ecx, %%ecx\n\t"
    "bts     %%ecx, %%edx\n\t"
    "jmp     _GetPowerOfTwo_Exit\n\t"
    "_GetPowerOfTwo_Next:\n\t"
    "mov     %3, %%ecx\n\t"
    "jecxz   _GetPowerOfTwo_Exit\n\t"
    "bsr     %%ecx, %%ecx\n\t"
    "bts     %%ecx, %%eax\n\t"
    "_GetPowerOfTwo_Exit:\n\t"
    "mov     %%eax, %0\n\t"
    "mov     %%edx, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Input.hi), "m" (Input.lo)
    :
  );
#else
  _asm
  {
    xor     eax, eax
    mov     edx, eax
    mov     ecx, Input.hi
    jecxz   _GetPowerOfTwo_Next
    bsr     ecx, ecx
    bts     edx, ecx
    jmp     _GetPowerOfTwo_Exit
    _GetPowerOfTwo_Next:
    mov     ecx, Input.lo
    jecxz   _GetPowerOfTwo_Exit
    bsr     ecx, ecx
    bts     eax, ecx
    _GetPowerOfTwo_Exit:
    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, edx
  }
#endif
#else
  {
    UINT64 value = 0;
    value = (UINT64)((UINT64)Input.hi << 32) | Input.lo;
    value = value ^ 2;
    RetData.lo = (UINT32)value;
    RetData.hi = (UINT32)(value >> 32);
  }
#endif
  return RetData;
}

/**

  Calculates the largest integer that is both
  a power of two and less than or equal to input

  @param Input - value to calculate power of two

  @retval The largest integer that is both a power of two and less than or equal to Input

**/
UINT32
GetPowerOfTwox32 (
              UINT32 Input
              )
{
  UINT8 count=0;
  UINT32 result, temp = Input;

  if( Input == 0 ) return ( 0 );

  while(!(temp & 0x80000000 ) ) {
    temp = temp << 1;
    count++;
  }

  result = 0x80000000 >> count ;

  return(result);
}

/**

  This function calculates floor(log2(x)) + 1
  by finding the index of the most significant bit
  and adding 1

  @param value     - Input value

  @retval Returns floor(log2(x)) + 1 or 0, if the input was 0 or negative

**/
UINT8
Log2x32 (
        UINT32 Value
        )
{
  UINT8 log = 0xFF;

  // Return 0 if value is negative
  if ((INT32)Value <= 0) return 0;

  while (Value != 0) {
    Value >>= 1;
    log++;
  }
  return log;
}

/**

  This routine allows a 64 bit value to be right shifted by 32 bits and returns the
  shifted value. Count is valid up 63. (Only Bits 0-5 is valid for Count)

  @param Operand - Value to be shifted
  @param Count   - Number of times to shift right.

  @retval Value shifted right identified by the Count.

**/
UINT64_STRUCT
RShiftUINT64 (
             UINT64_STRUCT Operand,
             UINT32        Count
             )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "push   %%ecx\n\t"
    "mov    %2, %%eax\n\t"
    "mov    %3, %%edx\n\t"

    // CL is valid from 0 - 31. shld will move EDX:EAX by CL times but EDX is not touched
    // For CL of 32 - 63, it will be shifted 0 - 31 so we will move edx to eax later.
    "mov    %4, %%ecx\n\t"
    "and    $63, %%ecx\n\t"
    "shrd   %%cl, %%edx, %%eax\n\t"
    "shr    %%cl, %%edx\n\t"
    "cmp    $32, %%ecx\n\t"
    "jc     _RShiftUINT64_Done\n\t"

    // Since Count is 32 - 63, edx will have been shifted  by 0 - 31
    // If shifted by 32 or more, set upper 32 bits to zero.
    "mov    %%edx, %%eax\n\t"
    "xor    %%edx, %%edx\n\t"

    "_RShiftUINT64_Done:\n\t"
    "pop    %%ecx\n\t"
    "mov    %%eax, %0\n\t"
    "mov    %%edx, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Operand.lo), "m" (Operand.hi), "m" (Count)
    :
  );
#else
  _asm
  {
    push   ecx
    mov    eax, dword ptr Operand.lo
    mov    edx, dword ptr Operand.hi

    ;
    ; CL is valid from 0 - 31. shld will move EDX:EAX by CL times but EDX is not touched
    ; For CL of 32 - 63, it will be shifted 0 - 31 so we will move edx to eax later.
    ;
    mov    ecx, Count
    and    ecx, 63
    shrd   eax, edx, cl
    shr    edx, cl
    cmp    ecx, 32
    jc     short _RShiftUINT64_Done

    ;
    ; Since Count is 32 - 63, edx will have been shifted  by 0 - 31
    ; If shifted by 32 or more, set upper 32 bits to zero.
    ;
    mov    eax, edx
    xor    edx, edx

    _RShiftUINT64_Done:
    pop    ecx
    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, edx
  }
#endif
#else
  {
    UINT64 value = 0;
    value = (UINT64)((UINT64)Operand.hi << 32) | Operand.lo;
    value = value >> Count;
    RetData.lo = (UINT32)value;
    RetData.hi = (UINT32)(value >> 32);
  }
#endif
  return RetData;
}

INT16 Crc16 (char *ptr, int count)
{
  INT16 crc, i;
  crc = 0;
  while (--count >= 0) {
    crc = crc ^ (INT16)(int)*ptr++ << 8;
    for (i = 0; i < 8; ++i) {
      if (crc & 0x8000) {
        crc = crc << 1 ^ 0x1021;
      } else {
        crc = crc << 1;
      }
    }
  }
  return(crc & 0xFFFF);
}

/**

  Adds two UINT64 values

  @param Operand1  - first UINT64 to add
  @param Operand2  - second UINT64 to add

  @retval sum of operand1 and operand2

**/
UINT64_STRUCT
AddUINT64 (
          UINT64_STRUCT Operand1,
          UINT64_STRUCT Operand2
          )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov    %2, %%eax\n\t"
    "mov    %3, %%ebx\n\t"


    "mov    %4, %%ecx\n\t"
    "mov    %5, %%edx\n\t"

    "add    %%ecx, %%eax\n\t"
    "adc    %%edx, %%ebx\n\t"

    "mov    %%eax, %0\n\t"
    "mov    %%ebx, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Operand1.lo), "m" (Operand1.hi), "m" (Operand2.lo), "m" (Operand2.hi)
    :
  );
#else
  _asm
  {
    mov    eax, dword ptr Operand1.lo
    mov    ebx, dword ptr Operand1.hi

    mov    ecx, dword ptr Operand2.lo
    mov    edx, dword ptr Operand2.hi

    add    eax, ecx
    adc    ebx, edx

    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, ebx
  }
#endif
#else
  {
    UINT64 value1 = 0, value2 = 0;
    value1 = ((UINT64)Operand1.hi << 32) | Operand1.lo;
    value2 = ((UINT64)Operand2.hi << 32) | Operand2.lo;
    value1 = value1 + value2;
    RetData.lo = (UINT32)value1;
    RetData.hi = (UINT32)(value1 >> 32);
  }
#endif
  return RetData;
}

/**

  Add UINT64 and UINT32

  @param Operand1  - UINT64 to add
  @param Operand2  - UINT32 to add

  @retval sum of operand1 and operand2

**/
UINT64_STRUCT
AddUINT64x32 (
             UINT64_STRUCT Operand1,
             UINT32        Operand2
             )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov    %2, %%eax\n\t"
    "mov    %3, %%ebx\n\t"

    "mov    %4, %%ecx\n\t"

    "add    %%ecx, %%eax\n\t"
    "adc    0, %%ebx\n\t"

    "mov    %%eax, %0\n\t"
    "mov    %%ebx, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Operand1.lo), "m" (Operand1.hi), "m" (Operand2)
    :
  );
#else
  _asm
  {
    mov    eax, dword ptr Operand1.lo
    mov    ebx, dword ptr Operand1.hi

    mov    ecx, dword ptr Operand2

    add    eax, ecx
    adc    ebx, 0

    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, ebx
  }
#endif
#else
  {
    UINT64 value1 = 0;
    value1 = ((UINT64)Operand1.hi << 32) | Operand1.lo;
    value1 = value1 + (UINT64)Operand2;
    RetData.lo = (UINT32)value1;
    RetData.hi = (UINT32)(value1 >> 32);
  }
#endif
  return RetData;
}

/**

  Subtract two UINT64s

  @param Operand1  - first UINT64
  @param Operand2  - second UINT64

  @retval operand1 - operand2

**/
UINT64_STRUCT
SubUINT64 (
          UINT64_STRUCT Operand1,
          UINT64_STRUCT Operand2
          )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov    %2, %%eax\n\t"
    "mov    %3, %%ebx\n\t"

    "mov    %4, %%ecx\n\t"
    "mov    %5, %%edx\n\t"

    "sub    %%ecx, %%eax\n\t"
    "sbb    %%edx, %%ebx\n\t"

    "mov    %%eax, %0\n\t"
    "mov    %%eax, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Operand1.lo), "m" (Operand1.hi), "m" (Operand2.lo), "m" (Operand2.hi)
    :
  );
#else
  _asm
  {
    mov    eax, dword ptr Operand1.lo
    mov    ebx, dword ptr Operand1.hi

    mov    ecx, dword ptr Operand2.lo
    mov    edx, dword ptr Operand2.hi

    sub    eax, ecx
    sbb    ebx, edx

    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, ebx
  }
#endif
#else
  {
    UINT64 value1 = 0, value2 = 0;
    value1 = ((UINT64)Operand1.hi << 32) | Operand1.lo;
    value2 = ((UINT64)Operand2.hi << 32) | Operand2.lo;
    if (value1 > value2)
      value1 = value1 - value2;
    else
      value1 = value2 - value1;
    RetData.lo = (UINT32)value1;
    RetData.hi = (UINT32)(value1 >> 32);
  }
#endif
  return RetData;
}

/**

  Subtract an UINT32 from an UINT64

  @param Operand1  -  UINT64
  @param Operand2  -  UINT32

  @retval operand1 - operand2

**/
UINT64_STRUCT
SubUINT64x32 (
             UINT64_STRUCT Operand1,
             UINT32        Operand2
             )
{
  UINT64_STRUCT RetData;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov    %2, %%eax\n\t"
    "mov    %3, %%ebx\n\t"

    "mov    %4, %%ecx\n\t"

    "sub    %%ecx, %%eax\n\t"
    "sbb    $0, %%ebx\n\t"

    "mov    %%eax, %0\n\t"
    "mov    %%ebx, %1\n\t"
    : "=m" (RetData.lo), "=m" (RetData.hi)
    : "m" (Operand1.lo), "m" (Operand1.hi), "m" (Operand2)
    :
  );
#else
  _asm
  {
    mov    eax, dword ptr Operand1.lo
    mov    ebx, dword ptr Operand1.hi

    mov    ecx, dword ptr Operand2

    sub    eax, ecx
    sbb    ebx, 0

    mov    dword ptr RetData.lo, eax
    mov    dword ptr RetData.hi, ebx
  }
#endif
#else
  {
    UINT64 value1 = 0;
    value1 = ((UINT64)Operand1.hi << 32) | Operand1.lo;
    if (value1 > (UINT64)Operand2)
      value1 = value1 - (UINT64)Operand2;
    else
      value1 = (UINT64)Operand2 - value1;
    RetData.lo = (UINT32)value1;
    RetData.hi = (UINT32)(value1 >> 32);
  }
#endif
  return RetData;
}

/**

  Raise 10 to the power of Power, and multiply the result with Operand

  @param Operand - multiplicand
  @param Power   - value for 10 ^ Power

  @retval Operand * (10 ^ Power)

**/
UINT64_STRUCT
Power10UINT64 (
  UINT64_STRUCT Operand,
  UINT32        Power
  )
{
   while(Power--) {
      Operand = MultUINT64x32(Operand, 10);
   }
   return Operand;
}


/**

  This function shifts a 32 bit int either positive or negative

  @param Value     - Input value to be shifted
  @param Shift       -Number of bits places to be shifted.

  @retval Value    - Updated 32 bit value.

**/
UINT32
BitShift (
  UINT32 Value,
  SINT8 ShiftAmount
  )
{
  if ((ShiftAmount > 31) || (ShiftAmount < -31)) return 0;

  if (ShiftAmount > 0) return (Value << ShiftAmount);
  else                 return (Value >> (-1*ShiftAmount));
}

/**

  This function Sign extends 7 to 8 Bits (Bit 6 to Bit 7)

  @param CurrentValue     - Input value to be shifted

  @retval CurrentValue    - Updated 32 bit value.

**/
UINT8
SignExt (
  UINT8 CurrentValue
  )
{
  CurrentValue |= ((CurrentValue & 0x40) << 1);
  return CurrentValue;
}

/**

  This function calculates the Natural Log of the Input parameter using integers

  @param Input        - 100 times a number to get the Natural log from.
                        - Max Input Number is 40,000 (without 100x)

  @retval Output      - 100 times the actual result. Accurate within +/- 2

**/
UINT32
NaturalLog (
  UINT32 Input
  )
{
  UINT32  Output = 0;

  while (Input > 271) {
    Input = (Input*1000)/2718;
    Output += 100;
  }

  Output += ((-16*Input*Input+11578*Input-978860)/10000);

  return Output;
}

/**

  This function calculates e to the power of of the Input parameter using integers

  @param Input        - 100 times a number to elevate e to.

  @retval Output      - 100 times the actual result. Accurate within +/- 2

**/
UINT32
Eexp (
  UINT32 Input
  )
{
  UINT32  Extra100 = 0;
  UINT32  Output = 1;

  while (Input > 100){
    Input -= 100;
    Output = (Output*2718)/10;
    if (Extra100) Output /= 100;
    Extra100 = 1;
  }

  Output = ((Output*(8*Input*Input+900*Input+101000))/1000);

  if (Extra100) Output /= 100;

  return Output;
}

BOOLEAN
IsPowerOfTwo(
  UINT32  value)
/**

  Description: Find if a value is a power of 2

  @param value  - value to be examined


  @retval TRUE - the value is a power of two
  @retval FALSE-  the value is not a power of two

**/
{
  if((value != 0) && !(value & (value - 1)))
    return  TRUE;
  else
    return  FALSE;
}

/**
  Rotates 64-bit data left by given count

  @param Data             - 64-bit data to rotate
  @param Count            - Number of bit positions to rotate

  @retval Rotated 64-bit result

**/
UINT64_STRUCT
RotateLeftUINT64 (
  UINT64_STRUCT Data,
  UINT32        Count
  )
{
   UINT64_STRUCT RetData;
   UINT32 msb;
   UINT32 i;

   RetData.hi = Data.hi;
   RetData.lo = Data.lo;
   for (i = 0; i < Count; i++) {
     msb = (RetData.hi >> 31) & BIT0;
     RetData = LShiftUINT64 (RetData, 1);
     RetData.lo = (RetData.lo & (~BIT0)) | msb;
   }
   return RetData;
}

/**
  Rotates 64-bit data right by given count

  @param Data             - 64-bit data to rotate
  @param Count            - Number of bit positions to rotate

  @retval Rotated 64-bit result

**/
UINT64_STRUCT
RotateRightUINT64 (
  UINT64_STRUCT Data,
  UINT32        Count
  )
{
   UINT64_STRUCT RetData;
   UINT32 lsb;
   UINT32 i;

   RetData.hi = Data.hi;
   RetData.lo = Data.lo;
   for (i = 0; i < Count; i++) {
     lsb = RetData.lo & BIT0;
     RetData = RShiftUINT64 (RetData, 1);
     RetData.hi = (RetData.hi & (~BIT31)) | (lsb << 31);
   }
   return RetData;
}

UINT32
Saturate (
  UINT32    maxVal,
  UINT32    val
  )
{
  if (maxVal < val) {
    return maxVal;
  } else {
    return val;
  }
} // Saturate


