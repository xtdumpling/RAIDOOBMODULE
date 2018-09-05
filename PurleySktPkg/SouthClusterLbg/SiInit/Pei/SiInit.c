/** @file
    Source code file for Silicon Init Post Memory module.

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

#include "SiInit.h"
#include <Library/PerformanceLib.h>
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
#include <Library/MeLibPei.h>
#if defined(AMT_SUPPORT) && AMT_SUPPORT
#include "Library/PeiAmtLib.h"
#endif // AMT_SUPPORT
#endif // ME_SUPPORT_FLAG

/**
  Silicon Init End of PEI callback function. This is the last change before entering DXE and OS when S3 resume.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
EFI_STATUS
SiInitOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  );

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR  mSiInitNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    SiInitOnEndOfPei
  }
};

/**
  Slicon Initializes after PostMem phase Policy PPI produced,
  All required polices must be installed before the callback

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  SiPolicy PPI.

  @retval EFI_SUCCESS             Succeeds.
**/

EFI_STATUS
EFIAPI
SiInitPostMemOnPolicy (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR  mSiInitPostMemNotifyList[] = {
  {
#ifndef FSP_FLAG
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
#else
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
#endif
    &gPchPlatformPolicyPpiGuid,
    SiInitPostMemOnPolicy
  }
};

/**
  Silicon Init End of PEI callback function. This is the last change before entering DXE and OS when S3 resume.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
EFI_STATUS
SiInitOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  )
{
  DEBUG ((DEBUG_INFO, "SiInitOnEndOfPei - Start\n"));

  //
  // Initializes PCH after End of Pei
  //
  PchOnEndOfPei ();

#ifdef ME_SUPPORT_FLAG
  MeOnEndOfPei ();
#endif // ME_SUPPORT_FLAG


  DEBUG ((DEBUG_INFO, "SiInitOnEndOfPei - End\n"));
  return EFI_SUCCESS;
}

/**
  Slicon Initializes after PostMem phase Policy PPI produced,
  All required polices must be installed before the callback

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  SiPolicy PPI.

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_UNSUPPORTED         The function failed to locate SiPolicy
**/
EFI_STATUS
EFIAPI
SiInitPostMemOnPolicy (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi
  )
{
  PCH_POLICY_PPI     *PchPolicy;
  EFI_STATUS         Status;

  DEBUG ((DEBUG_INFO, "SiInit () - Start\n"));

  //
  // Locate installed PCH Policy PPI
  //
  Status = PeiServicesLocatePpi (
             &gPchPlatformPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &PchPolicy
             );
  if (Status != EFI_SUCCESS) {
    //
    // PCH_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  //
  // Initializes PCH after memory services initialized
  //
  PchInit (PchPolicy);

  //
  // Initializes ME after memory services initialized
  //
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  MePostMemInit ();
#if defined(AMT_SUPPORT_FLAG) && AMT_SUPPORT_FLAG
  AmtPostMemInit ();
#endif // AMT_SUPPORT_FLAG
#endif // ME_SUPPORT_FLAG

  //
  // Install EndOfPei callback function.
  //
  Status = PeiServicesNotifyPpi (mSiInitNotifyList);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "SiInit () - End\n"));
  return EFI_SUCCESS;
}

/**
  Silicon Initializes after memory services initialized

  @param[in] FileHandle           The file handle of the file, Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completes successfully
**/
EFI_STATUS
EFIAPI
SiInit (
  IN  EFI_PEI_FILE_HANDLE               FileHandle,
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                Status;

  //
  // Install PostMem phase OnPolicyInstalled callback function.
  //
  Status = PeiServicesNotifyPpi (mSiInitPostMemNotifyList);
  ASSERT_EFI_ERROR (Status);
  return Status;
}
