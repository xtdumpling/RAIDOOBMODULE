//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  Csr To Pcie Conversion library

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <Library/CsrToPcieAddress.h>

//////////////////////////////////////////////////////////////////////////
//
// USRA Csr to PCIE  Address Library
// This Lib provide the way use platform Library instance
//
//////////////////////////////////////////////////////////////////////////

/**
  This Lib Convert the logical address (CSR type, e.g. CPU ID, Boxtype, Box instance etc.) into physical address
  
  @param[in] Global               Global pointer
  @param[in] Virtual              Virtual address
  @param[in] Address              A pointer of the address of the USRA Address Structure
  @param[out] AlignedAddress      A pointer of aligned address converted from USRA address

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
UINTN
EFIAPI
CsrGetPcieAlignAddress (
  IN VOID                     *Global,
  IN BOOLEAN                  Virtual,
  IN USRA_ADDRESS             *Address,
  OUT UINTN                   *AlignedAddress
  )
{
  USRA_ADDRESS PcieAddress;
  USRA_ZERO_ADDRESS_TYPE(PcieAddress, AddrTypePCIE);
  return 0;
};

