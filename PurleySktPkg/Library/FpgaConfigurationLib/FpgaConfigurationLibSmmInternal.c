//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  FPGA Configuration library for SMM

  Copyright (c) 2015-2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/FpgaSocketVariable.h>
#include <Protocol/SmmVariable.h>
#include "FpgaConfigurationLibInternal.h"

#define TESTING TRUE

VOID *
InternalCreateHob (VOID)
{
#ifdef TESTING
  UINTN             BufferSize;
  VOID              *Buffer = NULL;

  BufferSize = sizeof(FPGA_CONFIGURATION_HOB);

  //
  // get space to store it
  //
  Buffer = AllocateZeroPool (BufferSize);
  return Buffer;
#else
 return NULL;
#endif
}


FPGA_SOCKET_CONFIGURATION *
EFIAPI
FpgaGetVariable (
  VOID
  )
{
  EFI_STATUS                 Status;
  EFI_SMM_VARIABLE_PROTOCOL  *mSmmVariable;
  UINTN                      BufferSize;
  VOID                       *Buffer = NULL;

  BufferSize = sizeof(FPGA_SOCKET_CONFIGURATION);

  //
  // get space to store it
  //
  Buffer = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return NULL;
  }

  Status = gSmst->SmmLocateProtocol(
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    &mSmmVariable
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Get data from EFI variable
  //

  Status = mSmmVariable->SmmGetVariable (
                           FPGA_SOCKET_CONFIGURATION_NAME,
                           &gFpgaSocketVariableGuid,
                           NULL,
                           &BufferSize,
                           Buffer
                           );

  return (FPGA_SOCKET_CONFIGURATION *)Buffer;
}
