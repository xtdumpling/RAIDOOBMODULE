/** @file
  TraceHubStatusCodeHandlerRuntimeDxe implementation.

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains 'Framework Code' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may not be modified, except as allowed by
 additional terms of your license agreement.

@par Specification Reference:
**/

#include <TraceHubStatusCodeHandlerRuntimeDxe.h>

EFI_RSC_HANDLER_PROTOCOL    *mRscHandlerProtocol   = NULL;
EFI_EVENT                   mExitBootServicesEvent = NULL;

/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
UnregisterBootTimeHandlers (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  if (FeaturePcdGet (PcdStatusCodeUseTraceHub)) {
    mRscHandlerProtocol->Unregister (TraceHubStatusCodeReportWorkerRuntimeDxe);
  }
}

/**
  Entry point of TraceHub Status Code Driver.

  This function is the entry point of this DXE Status Code Driver.
  It initializes registers status code handlers, and registers event for 
  EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
TraceHubStatusCodeHandlerRuntimeDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;

  Status = gBS->LocateProtocol (
                  &gEfiRscHandlerProtocolGuid,
                  NULL,
                  (VOID **) &mRscHandlerProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  if (FeaturePcdGet (PcdStatusCodeUseTraceHub)) {
    mRscHandlerProtocol->Register (
                           TraceHubStatusCodeReportWorkerRuntimeDxe,
                           TPL_HIGH_LEVEL
                           );
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UnregisterBootTimeHandlers,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );

  return EFI_SUCCESS;
}
