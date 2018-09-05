/** @file
  TraceHubStatusCodeHandlerPei implementation.

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

#include <TraceHubStatusCodeHandlerPei.h>

/**
  Entry point of TraceHub Status Code PEIM.
  
  This function is the entry point of this Status Code PEIM.
  It initializes supported status code devices.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCESS  The entry point executes successfully.

**/
EFI_STATUS
EFIAPI
TraceHubStatusCodeHandlerPeiEntry (
  IN       EFI_PEI_FILE_HANDLE    FileHandle,
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS                Status;
  EFI_PEI_RSC_HANDLER_PPI   *RscHandlerPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **) &RscHandlerPpi
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Register NothPeak worker to ReportStatusCodeRouter.
  //
  if (FeaturePcdGet (PcdStatusCodeUseTraceHub)) {
    //
    // Register NothPeak worker.
    //
    Status = RscHandlerPpi->Register (TraceHubStatusCodeReportWorkerPei);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
