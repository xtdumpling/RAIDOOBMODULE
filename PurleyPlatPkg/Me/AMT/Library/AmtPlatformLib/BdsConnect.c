/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2004-2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file BdsConnect.c

  BDS Lib functions which relate with connect the device

**/
#if AMT_SUPPORT

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/DxeServices.h>
#include "Library/BdsLib.h"

/**

  This function will connect all current system handles recursively. The
  connection will finish until every handle's child handle created if it have.

  @param None

  @retval EFI_SUCCESS          - All handles and it's child handle have been connected

  EFI_STATUS           - Return the status of gBS->LocateHandleBuffer().

**/
EFI_STATUS
EFIAPI
BdsLibConnectAllEfi (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       HandleCount;
  EFI_HANDLE  *HandleBuffer;
  UINTN       Index;

  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);
  }

  FreePool (HandleBuffer);

  return EFI_SUCCESS;
}

/**

  Connects all drivers to all controllers.
  This function make sure all the current system driver will manage
  the correspoinding controllers if have. And at the same time, make
  sure all the system controllers have driver to manage it if have.

  @param None

  @retval None

**/
VOID
EFIAPI
BdsLibConnectAllDriversToAllControllers (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = EfiGetSystemConfigurationTable (&gEfiDxeServicesTableGuid, (VOID**) &gDS);
  if (EFI_ERROR(Status) || (gDS == NULL)){
    return;
  }
  do {
    //
    // Connect All EFI 1.10 drivers following EFI 1.10 algorithm
    //
    BdsLibConnectAllEfi ();

    //
    // Check to see if it's possible to dispatch an more DXE drivers.
    // The BdsLibConnectAllEfi () may have made new DXE drivers show up.
    // If anything is Dispatched Status == EFI_SUCCESS and we will try
    // the connect again.
    //
    Status = gDS->Dispatch ();

  } while (!EFI_ERROR (Status));

}
#endif // AMT_SUPPORT