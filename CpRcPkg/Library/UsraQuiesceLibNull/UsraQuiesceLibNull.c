//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
Implementation of UsraQuiesceLibNull class library.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <Library/UsraQuiesceLib.h>

#pragma pack(1)

UINT8 mMemQuiesceEnabled = FALSE;
VOID  *mCurrQuiescePtr = NULL, *mStartQuiescePtr = NULL;

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
  return 0;
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
  return FALSE;
}
