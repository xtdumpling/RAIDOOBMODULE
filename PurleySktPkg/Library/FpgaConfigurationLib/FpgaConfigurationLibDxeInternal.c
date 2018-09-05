//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaConfigurationLibDxeInternal.c

Abstract:

  This file provides Dxe Internal functions of FPGA  configuration values.

--*/
#include <Guid/FpgaSocketVariable.h>
#include "FpgaConfigurationLibInternal.h"

#define TESTING TRUE

VOID *
InternalCreateHob (VOID)
{
#ifdef TESTING

    UINTN             BufferSize;
    VOID                                  *Buffer = NULL;

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
  EFI_STATUS        Status;
  UINTN             BufferSize;
  VOID                                  *Buffer = NULL;

  BufferSize = sizeof(FPGA_SOCKET_CONFIGURATION);

  //
  // get space to store it
  //
  Buffer = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return NULL;
  }


  //
  // Get data from EFI variable
  //

  Status = gRT->GetVariable (
                  FPGA_SOCKET_CONFIGURATION_NAME,
                  &gFpgaSocketVariableGuid,
                  NULL,
                  &BufferSize,
          Buffer
                  );

  return (FPGA_SOCKET_CONFIGURATION *)Buffer;
}
