/** @file
  Base TraceHub Init Lib Null.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include <Uefi/UefiBaseType.h>

/**
  Null instance of Set TraceHub base address.

  @param[in] AddressHi                   High 32-bits for TraceHub base address.
  @param[in] AddressLo                   Low 32-bits for TraceHub base address.

  @retval EFI_UNSUPPORTED                This is a null instance.
**/
EFI_STATUS
EFIAPI
TraceHubBaseSet (
  UINT32                                AddressHi,
  UINT32                                AddressLo
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Null instance of basic initialization for TraceHub

  @param[in] EnableMode                 Trace Hub Enable Mode
  @param[in] Destination                Trace Hub Trace Destination

**/

VOID
TraceHubInitialize (
  IN  UINT8                             EnableMode
 )
{
  return;
}
