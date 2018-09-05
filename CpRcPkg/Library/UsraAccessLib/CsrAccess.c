//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  Unified Silicon Register Access (USRA) Lib

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "UsraAccessLib.h"

/**
  This API get the CSR address from the given USRA Address.

  @param[in] Global               Global pointer
  @param[in] Virtual              Virtual address
  @param[in] Address              A pointer of the address of the USRA Address Structure
  @param[out] AlignedAddress      A pointer of aligned address converted from USRA address

  @retval NONE
**/
VOID
GetCsrAccessAddress (
  IN VOID                     *Global,
  IN BOOLEAN                  Virtual,
  IN USRA_ADDRESS             *Address,
  OUT UINTN                   *AlignedAddress
  )
{
  CsrGetPcieAlignAddress (Global, Virtual, Address, AlignedAddress);
}

/**
  This API performs 8-bit, 16-bit, 32-bit or 64-bit CSR silicon register read operations.
  It transfers data from a register into a naturally aligned data buffer.

  @param[in] Address              A pointer of the address of the USRA Address Structure to be read out
  @param[in] Buffer               A pointer of buffer for the value read from the register

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
CsrRegisterRead (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *Buffer
  )
{
  UINTN                       AlignedAddress = 0;

  GetCsrAccessAddress (NULL, 0, Address, &AlignedAddress);
  if (UsraRegisterFilterRead(Address, AlignedAddress, Buffer))
    UsraRegAlignedRead((UINT32)Address->Attribute.AccessWidth, AlignedAddress, Buffer);
  
  if(FeaturePcdGet(PcdUsraTraceEnable))
  {
    USRATrace(Address, AlignedAddress, TraceRead, Buffer, NULL);
  }

  return RETURN_SUCCESS;
}

/**
  This API performs 8-bit, 16-bit, 32-bit or 64-bit CSR silicon register write operations.
  It transfers data from a naturally aligned data buffer into a register.

  @param[in] Address              A pointer of the address of the USRA Address Structure to be written
  @param[in] Buffer               A pointer of buffer for the value write to the register

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
CsrRegisterWrite (
  IN USRA_ADDRESS             *Address,
  OUT VOID                    *Buffer
  )
{
  UINTN                       AlignedAddress = 0;

  GetCsrAccessAddress (NULL, 0, Address, &AlignedAddress);
  if (UsraRegisterFilterWrite(Address, AlignedAddress, Buffer))
    UsraRegAlignedWrite((UINT32)Address->Attribute.AccessWidth, AlignedAddress, Buffer);  

  if (FeaturePcdGet (PcdUsraSupportS3))
  {
    if(Address->Attribute.S3Enable)
    {
      S3BootScriptSaveMemWrite ((S3_BOOT_SCRIPT_LIB_WIDTH)Address->Attribute.AccessWidth, (UINT64)AlignedAddress, 1, Buffer);
    }
  }

  if(FeaturePcdGet(PcdUsraTraceEnable))
  {
    USRATrace(Address, AlignedAddress, TraceWrite, Buffer, NULL);
  }

  return RETURN_SUCCESS;
}

/**
  This API performs 8-bit, 16-bit, 32-bit or 64-bit CSR silicon register AND then OR operations. It read data from a
  register, And it with the AndBuffer, then Or it with the OrBuffer, and write the result back to the register

  @param[in] Address              A pointer of the address of the silicon register to be written
  @param[in] AndBuffer            A pointer of buffer for the value used for AND operation
                                  A NULL pointer means no AND operation. RegisterModify() equivalents to RegisterOr()
  @param[in] OrBuffer             A pointer of buffer for the value used for OR operation
                                  A NULL pointer means no OR operation. RegisterModify() equivalents to RegisterAnd()

  @retval RETURN_SUCCESS          The function completed successfully.
**/
RETURN_STATUS
CsrRegisterModify (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *AndBuffer,
  IN VOID                     *OrBuffer
  )
{

  UINT64                      Data;
  UINT8                       WidthTable[] = {1,2,4,8};
  UINTN                       AlignedAddress = 0;

  GetCsrAccessAddress (NULL, 0, Address, &AlignedAddress);

  if (UsraRegisterFilterModify(Address, AlignedAddress, AndBuffer, OrBuffer)) {
    UsraRegAlignedRead((UINT32)Address->Attribute.AccessWidth, AlignedAddress, &Data);
    DataAndOr (&Data, AndBuffer, OrBuffer, WidthTable[(UINT8)Address->Attribute.AccessWidth]);
    UsraRegAlignedWrite((UINT32)Address->Attribute.AccessWidth, AlignedAddress, &Data);
  }

  if (FeaturePcdGet(PcdUsraTraceEnable))
  {
    USRATrace(Address, 0, TraceModify, AndBuffer, OrBuffer);
  }

  return RETURN_SUCCESS;
}
