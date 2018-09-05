/** @file
  Source code file for Silicon Init Pre Memory module.

@copyright
  Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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

@par Specification
**/
#include "SiInitPreMem.h"
#include <Library/PerformanceLib.h>

#include <Library/PchInfoLib.h>
#include <Library/TraceHubInitLib.h>
#include "Ppi/MeSpsPolicyPei.h"


/**
  Slicon Initializes after Policy PPI produced, All required polices must be installed before the callback

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             Succeeds.
**/
EFI_STATUS
EFIAPI
SiInitPreMemOnPolicy (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR  mSiInitNotifyList[] = {
  {
#ifndef FSP_FLAG
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
#else
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
#endif
    &gPchPlatformPolicyPpiGuid,
    SiInitPreMemOnPolicy
  }
};

/**
  Slicon Initializes after Policy PPI produced, All required polices must be installed before the callback

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             Succeeds.
**/
EFI_STATUS
EFIAPI
SiInitPreMemOnPolicy (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi
  )
{
  EFI_STATUS         Status;
  PCH_POLICY_PPI     *PchPolicyPpi;


  DEBUG ((DEBUG_INFO, "SiInitPreMemOnPolicy() Start\n"));


  Status = PeiServicesLocatePpi (
             &gPchPlatformPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&PchPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);


  ASSERT_EFI_ERROR (Status);


#ifndef MDEPKG_NDEBUG


#endif // MDEPKG_NDEBUG


  //
  // Configure PSFs for MCTP
  //
  PchMctpConfigure();
  //
  // Validate PCH policies
  //
  PchValidatePolicy (PchPolicyPpi);

  //
  // Initialize PCH after Policy PPI produced
  //
  PchOnPolicyInstalled (PchPolicyPpi);


  DEBUG ((DEBUG_INFO, "SiInitPreMemOnPolicy() - End\n"));
  return EFI_SUCCESS;
}

/**
  Silicon Initializes before Policy PPI produced

  @param[in] FileHandle           The file handle of the file, Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completes successfully
**/
EFI_STATUS
EFIAPI
SiInitPrePolicy (
  IN  EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                            Status;

  DEBUG ((DEBUG_INFO, "SiInitPrePolicy() Start\n"));

  //  Put your platform TraceHub init code here
  TraceHubInitialize (PcdGet8(PcdTraceHubEnMode)
  );

#ifdef FSP_FLAG
  EarlySiliconInit ();
#endif
  //
  // Initializes PCH before Policy initialized
  //
  PERF_START_EX (&gPerfPchPrePolicyGuid, NULL, NULL, AsmReadTsc(), 0x50E0);
  PchInitPrePolicy ();
  PERF_END_EX (&gPerfPchPrePolicyGuid, NULL, NULL, AsmReadTsc(), 0x50E1);

  //
  // Register Silicon init call back after PlatformPolicy PPI produced
  //
  Status = PeiServicesNotifyPpi (mSiInitNotifyList);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "SiInitPrePolicy() - End\n"));
  return EFI_SUCCESS;
}
