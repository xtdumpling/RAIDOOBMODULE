/*++        

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  InitializingDriverMain.c

Abstract:

  Main implementation source file for the InitializingDriver driver
  
--*/

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "InitializingDriver.h"

//
// Local function declarations
//
EFI_STATUS
InitializingDriverEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_DRIVER_ENTRY_POINT (InitializingDriverEntry)

EFI_STATUS
InitializingDriverEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:

  InitializingDriver driver entry point function. 

Arguments:

  ImageHandle   - image handle for this driver image
  SystemTable   - pointer to the EFI System Table

Returns:

  EFI_SUCCESS   - driver initialization completed successfully
  TODO: Add other return values  

--*/
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  
  Status = EFI_SUCCESS;
  Handle = NULL;
  EfiInitializeDriverLib (ImageHandle, SystemTable);
  //
  // TODO: Add implementation code
  //
  Status = EFI_UNLOAD_IMAGE;
  return Status;
}

