//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  USRA DXE driver manage all USRA entry initialized in PEI phase and DXE phase, and
  produce the implementation of native USRA protocol defined

  Copyright (c) 2014 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#include <Protocol/SiliconRegAccess.h>
#include <Library/UsraAccessApi.h>
#include <Library/UsraQuiesceLib.h>
extern UINTN *mHostPtr;
//////////////////////////////////////////////////////////////////////////
//
// Common Hardware Access Library
//
//////////////////////////////////////////////////////////////////////////

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register read operations. 
  It transfers data from a register into a naturally aligned data buffer.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be read out
  @param[in] Buffer               A pointer of buffer for the value read from the register

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
INTN
EFIAPI
SmmRegRead (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *Buffer
  )
{

  if(FeaturePcdGet(PcdQuiesceSupport))
  {
    if(QuiesceStatusCheck()){
      *((UINT32*)Buffer)= QuiesceRegisterRead(Address);
      return 0;
    }
  }
  Address->Attribute.HostPtr = (UINT32) (*(UINTN*)(&mHostPtr));
  return RegisterRead(Address, Buffer);
};

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register write operations. 
  It transfers data from a naturally aligned data buffer into a silicon register.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be written
  @param[in] Buffer               A pointer of buffer for the value write to the register

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
INTN
EFIAPI
SmmRegWrite (
  IN USRA_ADDRESS             *Address,
  OUT VOID                    *Buffer
  )
{
  if(FeaturePcdGet(PcdQuiesceSupport))
  {
    if(QuiesceStatusCheck()){
      QuiesceRegisterWrite(Address, Buffer);
      return 0;
    }
  }
  
  Address->Attribute.HostPtr = (UINT32) (*(UINTN*)(&mHostPtr));
  return RegisterWrite(Address, Buffer);
};

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register AND then OR operations. It read data from a
  register, And it with the AndBuffer, then Or it with the OrBuffer, and write the result back to the register
  
  @param[in] Address              A pointer of the address of the silicon register to be modified
  @param[in] AndBuffer            A pointer of buffer for the value used for AND operation
                                  A NULL pointer means no AND operation. RegisterModify() equivalents to RegisterOr()
  @param[in] OrBuffer             A pointer of buffer for the value used for OR operation
                                  A NULL pointer means no OR operation. RegisterModify() equivalents to RegisterAnd()

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
INTN
EFIAPI
SmmRegModify (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *AndBuffer,
  IN VOID                     *OrBuffer
  )
{
  Address->Attribute.HostPtr = (UINT32) (*(UINTN*)(&mHostPtr));
  return RegisterModify(Address, AndBuffer, OrBuffer);
};

/**
  This API get the flat address from the given USRA Address.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be read out

  @retval                         The flat address
**/
INTN
EFIAPI
SmmGetRegAddr (
  IN USRA_ADDRESS               *Address
  )
{
  Address->Attribute.HostPtr = (UINT32) (*(UINTN*)(&mHostPtr));
  return GetRegisterAddress(Address);
};
