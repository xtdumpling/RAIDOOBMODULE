//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**
@copyright
  Copyright (c) 2011 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  CommonAccessApi.h

@brief
  Common Silicon Access Library

**/

#ifndef __COMMON_HW_ACCESS_H__
#define __COMMON_HW_ACCESS_H__


#include <UsraAccessApi.h>

//////////////////////////////////////////////////////////////////////////
//
// Common Silicon Access Library
//
//////////////////////////////////////////////////////////////////////////

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register read operations. It transfers data from
  a register into a naturally aligned data buffer.

  @param  Address: A pointer of the address of the Common Address Structure to read from.
  @param  Buffer: A pointer of buffer for the value read from the CSR.

  @retval 0 The function completed successfully.
  @retval <>0 Returen Error
**/

INTN
EFIAPI
RegisterRead (
  IN USRA_ADDRESS    *Address,
  OUT VOID                    *Buffer
  );

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register write operations. It transfers data from
  a naturally aligned data buffer into a silicon register (CSR, Pcie Register etc).

  @param  Address: A pointer of the address of the Common Address Structure to write to.
  @param  Buffer A pointer of buffer for the value write to the CSR.

  @retval 0 The function completed successfully.
  @retval <>0 Returen Error
**/
INTN
EFIAPI
RegisterWrite (
  IN USRA_ADDRESS    *Address,
  IN VOID                     *Buffer
  );

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit Hardware Register AND then OR operations. It read data from a
  a Register, And it with the AndBuffer, then Or it with the OrBuffer, and write the result back to the Register

  @param  Address A pointer of the address of the Hardware Register to write to.
  @param  AndBuffer A pointer of buffer for the value used for AND operation.
          A NULL pointer means no AND operation. RegisterModify() equivalents to RegisterOr()
  @param  OrBuffer A pointer of buffer for the value used for OR operation.
          A NULL pointer means no OR operation. RegisterModify() equivalents to RegisterAnd()

  @retval 0 The function completed successfully.
  @retval <>0 Returen Error
**/
INTN
EFIAPI
RegisterModify (
  IN USRA_ADDRESS    *Address,
  IN VOID                     *AndBuffer,
  IN VOID                     *OrBuffer
  );

/**
  This API get the flat address from the given Common Address.

  @param  Address: A pointer of the address of the Common Address Structure to read from.

  @retval the flat address.
**/

INTN
EFIAPI
GetRegisterAddress (
  IN USRA_ADDRESS    *Address
  );

#endif
