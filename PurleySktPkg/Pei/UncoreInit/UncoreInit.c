/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/**

Copyright (c) 2005-2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file UncoreInitPeim.c

**/
#include "Uefi.h"
#include <Library/ProcMemInitLib.h>
#include <Library/OemProcMemInit.h>

/**

  Driver entry point                

  @param FfsHeader -  Not used.
  @param PeiServices - General purpose services available to every PEIM.

  @retval EFI_SUCCESS:  Memory initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
EFIAPI
PeimProcMemInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  SYSHOST host;

  //
  // Initialize data structures
  //
  MemSetLocal ((VOID *) &host, 0x00, sizeof (SYSHOST));

  host.var.common.oemVariable = (UINT32) PeiServices;

  ProcMemInit (&host);

  return EFI_SUCCESS;
}

/**

  Driver entry point                

  @param FfsHeader -  Not used.
  @param PeiServices - General purpose services available to every PEIM.

  @retval EFI_SUCCESS:  Memory initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
EFIAPI
UncoreInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  return PeimProcMemInit(FileHandle, PeiServices);
}
