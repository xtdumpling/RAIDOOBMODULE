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

  IioMmioAccess.c

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/
#pragma warning (disable: 4127)     // disable C4127: constant conditional expression
#include <IioPlatformData.h>
#include <MmioAccess.h>
#include <IioMisc.h>
#ifndef MINIBIOS_BUILD
#ifndef IA32
#include <Library/DebugLib.h>
#include <Library/S3BootScriptLib.h>
#endif // IA32
#else
#define UINTN UINT32
#endif // MINIBIOS_BUILD

/**

Routine Description:
  Writes 32-bits Memory address memory

Arguments:
  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param Data               - Data value

Returns:
  None

--*/
VOID
IioMmioWrite32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN UINT32 Data
 )
{
  IIO_ASSERT_INVALID_ADDRESS (IioGlobalData, Address);
  *(volatile UINT32 *) (UINTN)Address =  Data;
#ifndef IA32
  S3BootScriptSaveMemWrite(S3BootScriptWidthUint32, Address, 1, &Data);
#endif // IA32
}

/**

  Writes 16-bits Memory address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param Data               - Data value

  @retval None

--*/
VOID
IioMmioWrite16 ( 
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT16 Data
  )
{

  IIO_ASSERT_INVALID_ADDRESS (IioGlobalData, Address);
  *(volatile UINT16 *) (UINTN) Address =  Data;
#ifndef IA32
  S3BootScriptSaveMemWrite(S3BootScriptWidthUint16, Address, 1, &Data);
#endif // IA32
}

/**


  Writes 8-bits Memory address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param Data               - Data value

  @retval None

--*/
VOID
IioMmioWrite8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT8 Data
  )
{

  IIO_ASSERT_INVALID_ADDRESS (IioGlobalData, Address);
  *(volatile UINT8 *) (UINTN)Address = Data;
#ifndef IA32
  S3BootScriptSaveMemWrite(S3BootScriptWidthUint8, Address, 1, &Data);
#endif // IA32
}

/**

  Reads 32-bits Memory address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated

  @retval Data               - Data value

--*/
UINT32
IioMmioRead32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address
  )
{
  UINT32 Data;

  IIO_ASSERT_INVALID_ADDRESS (IioGlobalData, Address);

  Data = *(volatile UINT32 *)(UINTN) Address;

  return Data;
}

/**

  Reads 16-bits Memory address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated

  @retval Data               - Data value

***/
UINT16
IioMmioRead16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address
 )
{
  UINT16 Data;

  IIO_ASSERT_INVALID_ADDRESS (IioGlobalData, Address);

  Data = *(volatile UINT16 *)(UINTN) Address;

  return Data;
}

/**

  Reads 8-bits Memory address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated

  @retval Data               - Data value

--*/
UINT8
IioMmioRead8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address
  )
{
  UINT8 Data;

  IIO_ASSERT_INVALID_ADDRESS (IioGlobalData, Address);

  Data = *(volatile UINT8 *) (UINTN)Address;

  return Data;
}

/**

  Write 8-bits Memory address memory and ORs the value with new Data

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataOr             - Or Data

  @retval None

--*/
VOID
IioMmioOr32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT32  DataOr
  )
{
  IioMmioWrite32(IioGlobalData, Address, (IioMmioRead32( IioGlobalData, Address) | DataOr));

}

/**

  Write 16-bits Memory address memory and ORs the value with new Data

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataOr             - Or Data

  @retval  None

--*/
VOID
IioMmioOr16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT16  DataOr
  )
{
  IioMmioWrite16(IioGlobalData, Address, (IioMmioRead16( IioGlobalData, Address) | DataOr));
}

/**

  Write 8-bits Memory address memory and ORs the value with new Data

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataOr             - Or Data

  @retval None

--*/
VOID
IioMmioOr8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT8  DataOr
  )
{
  IioMmioWrite8(IioGlobalData, Address, (IioMmioRead8( IioGlobalData, Address) | DataOr));

}

/**

  Write 32-bits Memory address memory and ANDs the value with new Data

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param ddress            - Address that needs to be updated
  @param DataAnd            - And Data

  @retval None

--*/
VOID
IioMmioAnd32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT32  DataAnd
  )
{
  IioMmioWrite32(IioGlobalData, Address, (IioMmioRead32( IioGlobalData, Address) & DataAnd));

}


/**

  Write 16-bits Memory address memory and ANDs the value with new Data

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataAnd            - And Data

  @retval None

--*/
VOID
IioMmioAnd16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN UINT16  DataAnd
  )
{
  IioMmioWrite16(IioGlobalData, Address, (IioMmioRead16( IioGlobalData, Address) & DataAnd));
}


/**

  Write 8-bits Memory address memory and ANDs the value with new Data

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataAnd            - And Data

  @retval None

--*/
VOID
IioMmioAnd8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT8  DataAnd
  )
{
  IioMmioWrite8(IioGlobalData, Address,  (IioMmioRead8( IioGlobalData, Address) & DataAnd));
}


/**

  Write 32-bits Memory address memory. ANDs then ORs the value with new Data.

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataAnd            - And Data
  @param DataOr             - Or Data

  @retval None

--*/
VOID
IioMmioAndThenOr32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT32  DataAnd,
  IN  UINT32  DataOr
  )
{
  IioMmioWrite32(IioGlobalData, Address,((IioMmioRead32( IioGlobalData, Address) & DataAnd) | DataOr));
}

/**

  Write 32-bits Memory address memory. ANDs then ORs the value with new Data.

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataAnd            - And Data
  @param DataOr             - Or Data

  @retval None

--*/
VOID
IioMmioAndThenOr16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT16  DataAnd,
  IN  UINT16  DataOr
  )
{
  IioMmioWrite16(IioGlobalData, Address, ((IioMmioRead16( IioGlobalData, Address) & DataAnd) | DataOr));
}

/**

  Write 32-bits Memory address memory. ANDs then ORs the value with new Data.

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param DataAnd            - And Data
  @param DataOr             - Or Data

  @retval None

--*/
VOID
IioMmioAndThenOr8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT8  DataAnd,
  IN  UINT8  DataOr
  )
{
  IioMmioWrite8(IioGlobalData, Address, ((IioMmioRead8( IioGlobalData, Address) & DataAnd) | DataOr));
}
