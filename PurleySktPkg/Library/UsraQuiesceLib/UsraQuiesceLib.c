//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
Implementation of UsraQuiesceLib class library.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#include <Uefi.h>
#include <Protocol/QuiesceProtocol.h>
#include <Library/UsraQuiesceLib.h>
#include <Library/UsraAccessApi.h>

#pragma pack(1)

UINT8 mMemQuiesceEnabled = FALSE;
UINT64 *mQuiescePtr;
QUIESCE_DATA_TABLE_ENTRY *mCurrQuiescePtr = NULL, *mStartQuiescePtr = NULL;

/**
  This function will read data from Quiesce buffer if CSR found, otherwise, read from HW.

  @param Address   - Pointer to USRA Address

  @retval data read for READ_ACCESS
  
**/
UINT32
QuiesceRegisterRead (
  IN USRA_ADDRESS             *Address
  )
{
  QUIESCE_DATA_TABLE_ENTRY *Ptr;
  UINT32 Data32;
  
  UINT64 RegAddress;
  UINT8  Width;
  
  RegAddress = GetRegisterAddress(Address);
  Width  = (UINT8)(Address->Attribute.AccessWidth);

  if (mCurrQuiescePtr) {
    Ptr = mCurrQuiescePtr - 1;
    while (Ptr >= mStartQuiescePtr) {
      if (Ptr->CsrAddress == RegAddress) {
        return  Ptr->DataMask.Data;
      }
      Ptr--;
    }
  }

  //
  // If data not in quiesce buffer, read from hardware
  //
  switch (Width) {
  case 8:
    Data32 = *(volatile UINT8 *)(UINTN)RegAddress;
    break;
  case 16:
    Data32 = *(volatile UINT16 *)(UINTN)RegAddress;
    break;
  case 32:
    Data32 = *(volatile UINT32 *)(UINTN)RegAddress;
    break;
  default:
    Data32 = *(volatile UINT32 *)(UINTN)RegAddress;
    break;
  }
  return  Data32;
}

/**
  This function will write data to Quiesce buffer.

  @param RegAddr   - Pointer to USRA Address
  @param Buffer    - Pointer to data to be written

  @retval None

**/
VOID
QuiesceRegisterWrite (
  IN USRA_ADDRESS             *Address,
  IN VOID                    *Buffer
  )
{
  UINT64 RegAddress;
  UINT8  Width;
  UINT32 Data; 

  RegAddress = GetRegisterAddress(Address);
  Width  = (UINT8)(Address->Attribute.AccessWidth);
  Data = *((UINT32*)Buffer);

  if (!mCurrQuiescePtr) {
    return;
  }
  mCurrQuiescePtr->Operation       = EnumQuiesceWrite;
  mCurrQuiescePtr->AccessWidth     = Width;
  mCurrQuiescePtr->CsrAddress      = RegAddress;
  mCurrQuiescePtr->AndMask         = 0;
  mCurrQuiescePtr->DataMask.Data   = Data;
  mCurrQuiescePtr++;
  mQuiescePtr = (UINT64 *)mCurrQuiescePtr;

  return;
}

/**
  This function will return Quiesce status.

  @param void

  @retval Quiesce status.

**/
BOOLEAN
QuiesceStatusCheck (
  VOID
  )
{
  if (mMemQuiesceEnabled !=0)
    return TRUE;
  else
    return FALSE;
}
