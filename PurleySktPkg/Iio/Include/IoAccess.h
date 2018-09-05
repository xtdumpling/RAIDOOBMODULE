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

  IoAccess.h

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/

#ifndef _IO_ACCESS_H_
#define _IO_ACCESS_H_

UINT32
IioIoRead32 (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT16          Address
  );

UINT16
IioIoRead16 (
  IIO_GLOBALS     *IioGlobalData,
  UINT16          Address
  );

UINT8
IioIoRead8(
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT16          Address
  );

VOID
IioIoWrite32 (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT16          Address,
  IN  UINT32 Data
  );

VOID
IioIoWrite16 (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT16          Address,
  IN  UINT16          Data
  );

VOID
IioIoWrite8 (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT16          Address,
  IN  UINT8           Data
  );

#endif // _IO_ACCESS_H_
