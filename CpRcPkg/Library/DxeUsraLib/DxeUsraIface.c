//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  Implementation of UsarLib class library for DXE phase.

  Copyright (c) 2011 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/SiliconRegAccess.h>
#include <Library/DebugLib.h>
#include <Guid/DxeServices.h>
#include <Library/UefiLib.h>

USRA_PROTOCOL     *mUsra   = NULL;

/**
  Retrieves the USRA protocol from the handle database.
**/
USRA_PROTOCOL *
EFIAPI
GetUsraProtocol (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mUsra == NULL) {
    //
    // USRA protocol need to be installed before the module access USRA.
    // 
    Status = gBS->LocateProtocol (&gUsraProtocolGuid, NULL, (VOID **)&mUsra);
    ASSERT_EFI_ERROR (Status);
    ASSERT (mUsra != NULL);
  }
  return mUsra;
}


/**
  The constructor function initialize UsraProtocol.
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmDxeUsraLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  GetUsraProtocol();
  return EFI_SUCCESS;
}
