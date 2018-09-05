//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
Implementation of UsraRegisterFilterLibNull class library.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <Library/UsraAccessApi.h>

/**
  Perform register read.
  
  @param[in]  Address              A pointer of the address of the USRA Address Structure to be read out.
  @param[in]  AlignedAddress       An address to be read out.
  @param[out] Buffer               A pointer of buffer contains the data to be read out.

  @retval RETURN_UNSUPPORTED       Register access is not issued.
**/
RETURN_STATUS
EFIAPI
UsraRegisterFilterRead (
  IN  USRA_ADDRESS             *Address,
  IN  UINTN                    AlignedAddress,
  OUT VOID                    *Buffer
  )
{
  return RETURN_UNSUPPORTED;
}

/**
  Perform register write.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be written.
  @param[in] AlignedAddress       An address to be written.
  @param[in] Buffer               A pointer of buffer contains the data to be written.

  @retval RETURN_UNSUPPORTED      Register access is not issued.
**/
RETURN_STATUS
EFIAPI
UsraRegisterFilterWrite (
  IN  USRA_ADDRESS             *Address,
  IN  UINTN                    AlignedAddress,
  IN  VOID                     *Buffer
  )
{
  return RETURN_UNSUPPORTED;
}

/**
  Perform register modify.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be modified.
  @param[in] AlignedAddress       An address to be modified.
  @param[in] AndBuffer            A pointer of buffer for the value used for AND operation.
                                  A NULL pointer means no AND operation. RegisterModify() equivalents to RegisterOr().
  @param[in] OrBuffer             A pointer of buffer for the value used for OR operation.
                                  A NULL pointer means no OR operation. RegisterModify() equivalents to RegisterAnd().

  @retval RETURN_UNSUPPORTED      Register access is not issued.
**/
RETURN_STATUS
EFIAPI
UsraRegisterFilterModify (
  IN  USRA_ADDRESS             *Address,
  IN  UINTN                    AlignedAddress,
  IN  VOID                     *AndBuffer,
  IN  VOID                     *OrBuffer
  )
{
  return RETURN_UNSUPPORTED;
}


