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

//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

UINT8
InPort8_ (
         UINT16 ioAddress
         )
/*++

  Reads 8 bit IO port

  @param ioAddress - IO Address to use

  @retval Value read from ioAddress

--*/
{
  UINT8 alReg = 0;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov %1, %%dx\n\t"
    "in %%dx, %%al\n\t"
    "mov %%al, %0\n\t"
    : "=r" (alReg)
    : "r" (ioAddress)
    :
  );
#else
  _asm
  {
    mov dx, ioAddress
    in al, dx
    mov alReg, al
  }
#endif
#else
  alReg = IoRead8(ioAddress);
#endif

  return alReg;
}

UINT8
InPort8 (
        PSYSHOST host,
        UINT16   ioAddress
        )
/*++

  Reads 8 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use

  @retval Value read from ioAddress

--*/
{
  UINT8 alReg;

  alReg = InPort8_ (ioAddress);

#ifdef MRC_TRACE
  /*
   *Socket, Type, Address, Direction, Size, Data

   Where:
   - Socket = processor socket # that executes the instruction
   - Type = 0 for IO, or 1 for Memory
   - Address = 16-bit IO or 32-bit Memory address (the MMIO address will encode bus, device, function, offset according to MMCFG format)
   - Direction = 0 for Rd, or 1 for Wr
   - Size = 1, 2, or 4 bytes
   - Data = hex data corresponding to the size

   For example:
   *0, 0, 0x80, 1, 1, 0xA0
   */
  if (host) {
    if (host->setup.common.serialDebugMsgLvl & SDBG_TRACE) {
      rcPrintf ((host, "*%d, 0, 0x%x, 0, %d, 0x%02x\n", host->var.common.socketId, ioAddress, sizeof (UINT8), alReg));
    }
    //
    // SDBG_TRACE
    //
  }
  //
  // host
  //
#endif
  return alReg;
}

UINT16
InPort16 (
         PSYSHOST host,
         UINT16   ioAddress
         )
/*++

  Reads 16 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use

  @retval Value read from ioAddress

--*/
{
  UINT16  axReg=0;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov %1, %%dx\n\t"
    "in %%dx, %%ax\n\t"
    "mov %%ax, %0\n\t"
    : "=r" (axReg)
    : "r" (ioAddress)
    :
  );
#else
  _asm
  {
    mov dx, ioAddress
    in ax, dx
    mov axReg, ax
  }
#endif
#else
  axReg = IoRead16(ioAddress);
#endif

#ifdef MRC_TRACE
  if (host) {
    if (host->setup.common.serialDebugMsgLvl & SDBG_TRACE) {
      rcPrintf ((host, "*%d, 0, 0x%x, 0, %d, 0x%04x\n", host->var.common.socketId, ioAddress, sizeof (UINT16), axReg));
    }
    //
    // SDBG_TRACE
    //
  }
  //
  // host
  //
#endif // MRC_TRACE
  return axReg;
}

UINT32
InPort32 (
         PSYSHOST host,
         UINT16   ioAddress
         )
/*++

  Reads 32 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use

  @retval Value read from ioAddress

--*/
{
  UINT32  eaxReg=0;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov %1, %%dx\n\t"
    "in %%dx, %%eax\n\t"
    "mov %%eax, %0\n\t"
    : "=r" (eaxReg)
    : "r" (ioAddress)
    :
  );
#else
  _asm
  {
    mov dx, ioAddress
    in eax, dx
    mov eaxReg, eax
  }
#endif
#else
  eaxReg = IoRead32(ioAddress);
#endif

#ifdef MRC_TRACE
  if (host) {
    if (host->setup.common.serialDebugMsgLvl & SDBG_TRACE) {
      rcPrintf ((host, "*%d, 0, 0x%x, 0, %d, 0x%08x\n", host->var.common.socketId, ioAddress, sizeof (UINT32), eaxReg));
    }
    //
    // SDBG_TRACE
    //
  }
  //
  // host
  //
#endif // MRC_TRACE
  return eaxReg;
}

void
OutPort8_ (
          UINT16 ioAddress,
          UINT8  data
          )
/*++

  Writes 8 bit IO port with designated value

  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval N/A

--*/
{
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov %0, %%dx\n\t"
    "mov %1, %%al\n\t"
    "out %%al, %%dx\n\t"
    :
    : "r" (ioAddress), "r" (data)
    :
  );
#else
  _asm
  {
    mov dx, ioAddress
    mov al, data
    out dx, al
  }
#endif
#else
  IoWrite8(ioAddress, data);
#endif
}

void
OutPort8 (
         PSYSHOST host,
         UINT16   ioAddress,
         UINT8    data
         )
/*++

  Writes 8 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval N/A

--*/
{
  OutPort8_ (ioAddress, data);

#ifdef MRC_TRACE
  if (host) {
    if (host->setup.common.serialDebugMsgLvl & SDBG_TRACE) {
      rcPrintf ((host, "*%d, 0, 0x%x, 1, %d, 0x%02x\n", host->var.common.socketId, ioAddress, sizeof (UINT8), data));
    }
  }
#endif
}

void
OutPort16 (
          PSYSHOST host,
          UINT16   ioAddress,
          UINT16   data
          )
/*++

  Writes 16 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval N/A

--*/
{
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov %0, %%dx\n\t"
    "mov %1, %%ax\n\t"
    "out %%ax, %%dx\n\t"
    :
    : "r" (ioAddress), "r" (data)
    :
  );
#else
  _asm
  {
    mov dx, ioAddress
    mov ax, data
    out dx, ax
  }
#endif
#else
  IoWrite16(ioAddress, data);
#endif //IA32

#ifdef MRC_TRACE
  if (host) {
    if (host->setup.common.serialDebugMsgLvl & SDBG_TRACE) {
      rcPrintf ((host, "*%d, 0, 0x%x, 1, %d, 0x%04x\n", host->var.common.socketId, ioAddress, sizeof (UINT16), data));
    }
  }
#endif
}

void
OutPort32 (
          PSYSHOST host,
          UINT16   ioAddress,
          UINT32   data
          )
/*++

  Writes 32 bit IO port with serial debug output

  @param host      - Pointer to the system host (root) structure
  @param ioAddress - IO Address to use
  @param data      - Data to write to IO Address

  @retval N/A

--*/
{
#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov %0, %%dx\n\t"
    "mov %1, %%eax\n\t"
    "out %%eax, %%dx\n\t"
    :
    : "r" (ioAddress), "r" (data)
    :
  );
#else
  _asm
  {
    mov dx, ioAddress
    mov eax, data
    out dx, eax
  }
#endif
#else
  IoWrite32(ioAddress, data);
#endif //IA32

#ifdef MRC_TRACE
  if (host) {
    if (host->setup.common.serialDebugMsgLvl & SDBG_TRACE) {
      rcPrintf ((host, "*%d, 0, 0x%x, 1, %d, 0x%08x\n", host->var.common.socketId, ioAddress, sizeof (UINT32), data));
    }
  }
#endif
}
