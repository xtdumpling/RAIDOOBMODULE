//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file


  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef __USRA_ACCESS_API_H__
#define __USRA_ACCESS_API_H__


#include <UsraAccessType.h>

//////////////////////////////////////////////////////////////////////////
//
// USRA Silicon Access Library
//
//////////////////////////////////////////////////////////////////////////

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register read operations. 
  It transfers data from a register into a naturally aligned data buffer.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be read out
  @param[in] Buffer               A pointer of buffer for the value read from the register

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
EFIAPI
RegisterRead (
  IN USRA_ADDRESS             *Address,
  OUT VOID                    *Buffer
  );

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register write operations. 
  It transfers data from a naturally aligned data buffer into a silicon register.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be written
  @param[in] Buffer               A pointer of buffer for the value write to the register

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
EFIAPI
RegisterWrite (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *Buffer
  );

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register AND then OR operations. It read data from a
  register, And it with the AndBuffer, then Or it with the OrBuffer, and write the result back to the register
  
  @param[in] Address              A pointer of the address of the silicon register to be written
  @param[in] AndBuffer            A pointer of buffer for the value used for AND operation
                                  A NULL pointer means no AND operation. RegisterModify() equivalents to RegisterOr()
  @param[in] OrBuffer             A pointer of buffer for the value used for OR operation
                                  A NULL pointer means no OR operation. RegisterModify() equivalents to RegisterAnd()

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
EFIAPI
RegisterModify (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *AndBuffer,
  IN VOID                     *OrBuffer
  );

/**
  This API get the flat address from the given USRA Address.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be read out

  @retval                         The flat address
**/
INTN
EFIAPI
GetRegisterAddress (
  IN USRA_ADDRESS            *Address
  );

#endif
