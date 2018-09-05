/** @file
  PCH SPI Async SMI handler.

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
#include "PchInitSmm.h"

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SMI_DISPATCH_PROTOCOL     *mPchSmiDispatchProtocol;

/**
  This hardware SMI handler will be run every time the flash write/earse happens.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchSpiAsyncCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  //
  // Dummy SMI handler
  //
}

/**
  This fuction install SPI ASYNC SMI handler.

  @retval EFI_SUCCESS             Initialization complete.
**/
EFI_STATUS
EFIAPI
InstallPchSpiAsyncSmiHandler (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;

  DEBUG ((DEBUG_INFO, "InstallPchSpiAsyncSmiHandler()\n"));

  ///
  /// Get the PCH SMM dispatch protocol
  ///
  mPchSmiDispatchProtocol = NULL;
  Status = gSmst->SmmLocateProtocol (&gPchSmiDispatchProtocolGuid, NULL, (VOID **) &mPchSmiDispatchProtocol);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register an SpiAsync callback function
  ///
  Handle = NULL;
  Status = mPchSmiDispatchProtocol->SpiAsyncRegister (
                                      mPchSmiDispatchProtocol,
                                      PchSpiAsyncCallback,
                                      &Handle
                                      );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

