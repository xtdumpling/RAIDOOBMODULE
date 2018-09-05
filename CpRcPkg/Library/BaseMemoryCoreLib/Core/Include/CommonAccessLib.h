//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**

@copyright
  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  CommonAccessLib.h

@brief
  Common Silicon Access Library
**/

#ifndef __COMMON_ACCESS_LIB_H__
#define __COMMON_ACCESS_LIB_H__

#include "PlatformHost.h"

#ifdef   MINIBIOS_BUILD
#include "DataTypes.h"
#include "UsraAccessApi.h"
#include "CsrToPcieAddress.h"
#include "PcieAddress.h"

///
/// S3 Boot Script Width.
///
typedef enum {
  S3BootScriptWidthUint8,        ///< 8-bit operation.
  S3BootScriptWidthUint16,       ///< 16-bit operation.
  S3BootScriptWidthUint32,       ///< 32-bit operation.
  S3BootScriptWidthUint64,       ///< 64-bit operation.
  S3BootScriptWidthMaximum
} S3_BOOT_SCRIPT_LIB_WIDTH;

VOID
EFIAPI
MemoryFence (
  VOID
  )
{
  return;
}

/**
  Reads an 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address. The 8-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT8
EFIAPI
MmioRead8 (
  IN      UINTN                     Address
  )
{
  UINT8                             Value;

  MemoryFence ();
  Value = *(volatile UINT8*)Address;
  MemoryFence ();

  return Value;
}

/**
  Writes an 8-bit MMIO register.

  Writes the 8-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

  @return Value.

**/
UINT8
EFIAPI
MmioWrite8 (
  IN      UINTN                     Address,
  IN      UINT8                     Value
  )
{
  MemoryFence ();
  *(volatile UINT8*)Address = Value;
  MemoryFence ();

  return Value;
}

/**
  Reads a 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address. The 16-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT16
EFIAPI
MmioRead16 (
  IN      UINTN                     Address
  )
{
  UINT16                            Value;

  //ASSERT ((Address & 1) == 0);

  MemoryFence ();
  Value = *(volatile UINT16*)Address;
  MemoryFence ();

  return Value;
}

/**
  Writes a 16-bit MMIO register.

  Writes the 16-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

  @return Value.

**/
UINT16
EFIAPI
MmioWrite16 (
  IN      UINTN                     Address,
  IN      UINT16                    Value
  )
{
  //ASSERT ((Address & 1) == 0);

  MemoryFence ();
  *(volatile UINT16*)Address = Value;
  MemoryFence ();

  return Value;
}

/**
  Reads a 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address. The 32-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT32
EFIAPI
MmioRead32 (
  IN      UINTN                     Address
  )
{
  UINT32                            Value;

  //ASSERT ((Address & 3) == 0);

  MemoryFence ();
  Value = *(volatile UINT32*)Address;
  MemoryFence ();

  return Value;
}

/**
  Writes a 32-bit MMIO register.

  Writes the 32-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

  @return Value.

**/
UINT32
EFIAPI
MmioWrite32 (
  IN      UINTN                     Address,
  IN      UINT32                    Value
  )
{
  //ASSERT ((Address & 3) == 0);

  MemoryFence ();
  *(volatile UINT32*)Address = Value;
  MemoryFence ();

  return Value;
}

/**
  Reads a 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address. The 64-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT64
EFIAPI
MmioRead64 (
  IN      UINTN                     Address
  )
{
  UINT64                            Value;

  //ASSERT ((Address & 7) == 0);

  MemoryFence ();
  Value = *(volatile UINT64*)Address;
  MemoryFence ();

  return Value;
}

/**
  Writes a 64-bit MMIO register.

  Writes the 64-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
MmioWrite64 (
  IN      UINTN                     Address,
  IN      UINT64                    Value
  )
{
  //ASSERT ((Address & 7) == 0);

  MemoryFence ();
  *(volatile UINT64*)Address = Value;
  MemoryFence ();

  return Value;
}

#ifndef NT32_BUILD
BOOLEAN
EFIAPI
FeaturePcdGet (
  IN      BOOLEAN        Value
  )
{
  return Value;
}
#endif // #ifndef NT32_BUILD

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

UINTN
EFIAPI
S3BootScriptSaveMemWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH          Width,
  IN  UINT64                            Address,
  IN  UINTN                             Count,
  IN  VOID                              *Buffer
  )
{
    return 0;
}

#endif

#endif
