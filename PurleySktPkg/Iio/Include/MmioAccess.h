//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

 
  @file MmioAccess.h

  Macros to simplify and abstract the interface to PCI configuration.

**/

#ifndef _MMIOACCESS_H_
#define _MMIOACCESS_H_


#define IIO_ASSERT_INVALID_ADDRESS(IioGlobalData, A) \
  if (((A) & ~0xfffffff) == 0) { \
    IIO_ASSERT(IioGlobalData, IIO_INVALID_PARAMETER, A); \
  }

VOID
IioMmioWrite32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT32 Data
  );

VOID
IioMmioWrite16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address,
  IN  UINT16      Data
  );

VOID
IioMmioWrite8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address,
  IN  UINT8       Data
  );

UINT32
IioMmioRead32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address
  );

UINT16
IioMmioRead16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address
  );

UINT8
IioMmioRead8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address
  );

VOID
IioMioOr32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address,
  IN  UINT32      DataOr
  );

VOID
IioMmioOr16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address,
  IN  UINT16      DataOr
  );

VOID
IioMmioOr8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 Address,
  IN  UINT8  DataOr
  );

VOID
IioMmioAnd32 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address,
  IN  UINT32      DataAnd
  );

VOID
IioMmioAnd16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address,
  IN  UINT16      DataAnd
  );

VOID
IioMmioAnd8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32      Address,
  IN  UINT8       DataAnd
  );

VOID
IioMmioAndThenOr32 (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT32       Address,
  IN  UINT32       DataAnd,
  IN  UINT32       DataOr
  );

VOID
IioMmioAndThenOr16 (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT32        Address,
  IN  UINT16        DataAnd,
  IN  UINT16        DataOr
);

VOID
IioMmioAndThenOr8 (
  IN IIO_GLOBALS *IioGlobalData,
  IN UINT32       Address,
  IN UINT8        DataAnd,
  IN UINT8        DataOr
);

#endif // _MMIOACCESS_H_
