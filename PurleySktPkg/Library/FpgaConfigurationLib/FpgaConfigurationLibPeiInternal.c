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

  FpgaConfigurationLibPeiInternal.c

Abstract:

  This file provides Pei Internal functions of FPGA  configuration values.

--*/
#include "FpgaConfigurationLibInternal.h"
#include <Guid/FpgaSocketVariable.h>

extern FPGA_SOCKET_CONFIGURATION  mFpgaConfig;
VOID *
InternalCreateHob (VOID)
{
   VOID                             *Hob;
   UINTN                            BufferSize;
   FPGA_CONFIGURATION_HOB           data; 

   BufferSize = (sizeof (FPGA_CONFIGURATION_HOB));
   ZeroMem(&data,BufferSize);
   Hob = BuildGuidDataHob (&gFpgaSocketHobGuid, &data, BufferSize );

   return Hob;
}


FPGA_SOCKET_CONFIGURATION *
EFIAPI
FpgaGetVariable (
  VOID
  )
{
  EFI_STATUS                            Status;
  VOID                                  *Buffer = NULL;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  CONST EFI_PEI_SERVICES                **PeiServices;


  UINTN               BufferSize = sizeof(FPGA_SOCKET_CONFIGURATION);

  PeiServices = GetPeiServicesTablePointer ();
  (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiReadOnlyVariable2PpiGuid,
                  0,
                  NULL,
                  &PeiVariable
                  );

  Buffer = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return NULL;
  }
  //
  // Read variable into the allocated buffer.
  //
  Status = PeiVariable->GetVariable (PeiVariable, FPGA_SOCKET_CONFIGURATION_NAME, &gFpgaSocketVariableGuid, NULL, &BufferSize, Buffer);
  DEBUG((EFI_D_ERROR, "FpgaGetVariable status = %r !\n", Status));
  if (EFI_ERROR (Status)) {    
    return NULL;
  }

  return (FPGA_SOCKET_CONFIGURATION *)Buffer;
}
