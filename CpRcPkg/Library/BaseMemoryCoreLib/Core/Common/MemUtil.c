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
#include "MemFunc.h"


UINT32 MemScan(UINT8 *dest, UINT32 value, UINT32 numBytes);

void
MemSetLocal (
            UINT8  *dest,
            UINT32 value,
            UINT32 numBytes
            )
/*++

  Flood fill a specific memory location block with a specified value

  @param dest      - Pointer to memory location
  @param value     - Value to flood location with
  @param numBytes  - number of bytes to flood with value provided

  @retval N/A

--*/
{
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "pushf\n\t"
    "push  %%edi\n\t"
    "cld\n\t"
    "mov   %0, %%edi\n\t"
    "mov   %1, %%eax\n\t"
    "mov   %2, %%ecx\n\t"
    "rep   stosb\n\t"
    "pop   %%edi\n\t"
    "popf\n\t"
    :
    : "r" (dest), "r" (value), "r" (numBytes)
    :
  );
#else
  _asm
  {
    pushf
    push  edi
    cld
    mov   edi, dest
    mov   eax, value
    mov   ecx, numBytes
    rep   stosb
    pop   edi
    popf
  }
#endif
#else
  UINT8 *ptr;
  ptr = dest;
  while (numBytes--) {
    *(ptr++) = (UINT8)value;
  }
#endif
}

UINT32
MemScan (
        UINT8  *dest,
        UINT32 value,
        UINT32 numBytes
        )
/*++

  Finds if a specific 32-bit pattern is in a block of memory

  @param dest      - Pointer to memory location
  @param value     - 32-bit value to find in memory block
  @param numBytes  - number of bytes to flood with value provided

  @retval 0 - pattern present
  @retval 1 - patern not present

--*/
{
  UINT32 status = FAILURE;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "pushf\n\t"
    "push  %%edi\n\t"
    "cld\n\t"
    "mov   %1, %%edi\n\t"
    "mov   %2, %%eax\n\t"
    "mov   %3, %%eax\n\t"
    "repz  scasl\n\t"
    "jnz   exit\n\t"
    "mov   $0, %0\n\t"
    "exit:\n\t"
    "pop   %%edi\n\t"
    "popf\n\t"
    : "=r" (status)
    : "r" (dest), "r" (value), "r" (numBytes)
    :
  );
#else
  _asm
  {
    pushf
    push  edi
    cld
    mov   edi, dest
    mov   eax, value
    mov   ecx, numBytes
    repz  scasd
    jnz   exit
    mov   status, 0
    exit:
    pop   edi
    popf
  }
#endif
#else
  UINT32 *ptr;
  ptr = (UINT32*)dest;
  do {
    if (*(ptr++) == value)
      status = 0;
  } while (--numBytes != 0);
#endif
  return status;
}

void
MemCopy (
        UINT8  *dest,
        UINT8  *src,
        UINT32 numBytes
        )
/*++

  copies a specified block of memory from source location to destination location

  @param dest      - pointer to copy destination
  @param src       - pointer to source to copy
  @param numBytes  - size to copy

  @retval N/A

--*/
{
  while (numBytes--) {
    *(dest++) = *(src++);
  }
}

UINT32
MemCompare(
          UINT8 *dest,
          UINT8 *src,
          UINT32 numBytes
          )
/*++

  compares a specified block of memory from source location to destination location

  @param dest      - pointer to copy destination
  @param src       - pointer to source to copy
  @param numBytes  - size to copy

  @retval UINT32 = 0 if all data matches
         @retval = 1 if miscompare

--*/
{
  while (numBytes--) {
    if (*(dest++) != *(src++)) return 1;
  }
  return 0;
}

#ifndef IA32
/*++
  Hack function for passing build.
--*/
VOID
__chkstk()
{
}
#endif




