//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.
 

Module Name:

  IioMisc.h

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/

#ifndef _IIO_MISC_H_
#define _IIO_MISC_H_

#ifndef MINIBIOS_BUILD
#ifndef IA32
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#endif // IA32
#endif // MINIBIOS_BUILD

#define IIO_DEBUG_WARN      0x00000002  // Warnings
#define IIO_DEBUG_INFO      0x00000040  // Informational debug messages
#define IIO_DEBUG_ERROR     0x80000000  // Error


#define IIO_SUCCESS              0
#define IIO_ERROR                0xE00000FF
#define IIO_NOT_READY            0xE00000FE
#define IIO_UNSUPPORTED          0xE00000FD
#define IIO_INVALID_PARAMETER    0xE00000FC
#define IIO_OUT_OF_RESOURCES     0xE00000FB
#define IIO_ERR_GENERAL_FAILURE  0xE00000FA
#define IIO_MEMORY_UNMAPPED      0xE00000F9

#define IIO_MINOR_ERR_GENERAL    0x1
#define IIO_MINOR_ERR_TIMEOUT    0x2

#ifdef IA32
#define IioDebugPrintInfo(IioGlobalData, ...) { IioDebugPrint (IioGlobalData, __VA_ARGS__); }
#else
#define IioDebugPrintInfo(IioGlobalData, ...) { DEBUG((__VA_ARGS__)); }
#endif // IA32

VOID
IioStall (
  IIO_GLOBALS *IioGlobalData,
  UINT32      UsDelay
  );

VOID
IioWarmReset(
  IIO_GLOBALS *IioGlobalData
);

VOID
IioCpuDeadLoop(
);

VOID
IioCopyMem(
  VOID *dest,
  VOID *src,
  UINT32 size
);

VOID
IioAssert (
  IIO_GLOBALS     *IioGlobalData,
  UINT32          Error
);  

#ifdef IA32
VOID
IioDebugPrint (
  IIO_GLOBALS *IioGlobalData,
  UINT32 DbgLevel,
  char* Format,
  ...
  );
#endif // IA32

#define IIO_ASSERT(IioGlobalData, Error, Data)   \
  if (Error > 0) { \
    IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_ERROR, "\n%s: %u \n Error: %x", __FILE__, __LINE__, Error); \
    IioAssert(IioGlobalData, Data); \
  }


#endif // _IIO_MISC_H_
