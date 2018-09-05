/** @file
  PCH RESET PEIM DRIVER.

@copyright
 Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
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

@par Specification Reference:
**/
#include "PchReset.h"

/**
  Execute Pch Reset from the host controller.

  @param[in] This                 Pointer to the PCH_RESET_PPI instance.
  @param[in] PchResetType         Pch Reset Types which includes ColdReset, WarmReset, ShutdownReset,
                                  PowerCycleReset, GlobalReset, GlobalResetWithEc

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_INVALID_PARAMETER   If ResetType is invalid.
**/
EFI_STATUS
EFIAPI
Reset (
  IN PCH_RESET_PPI  *This,
  IN PCH_RESET_TYPE PchResetType
  )
{
  PCH_RESET_INSTANCE  *PchResetInstance;
  EFI_STATUS          Status;

  PchResetInstance = PCH_RESET_INSTANCE_FROM_THIS (This);
  Status = PchReset (PchResetInstance, PchResetType);

  return Status;
}

/**
  Installs PCH RESET PPI

  @retval EFI_SUCCESS             PCH RESET PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
InstallPchReset (
  VOID

  )
{
  EFI_STATUS              Status;
  PEI_PCH_RESET_INSTANCE  *PeiPchResetInstance;
  PCH_RESET_INSTANCE      *PchResetInstance;

  DEBUG ((DEBUG_INFO, "InstallPchReset() Start\n"));

  PeiPchResetInstance = (PEI_PCH_RESET_INSTANCE *) AllocateZeroPool (sizeof (PEI_PCH_RESET_INSTANCE));
  if (NULL == PeiPchResetInstance) {
    return EFI_OUT_OF_RESOURCES;
  }

  PchResetInstance = &(PeiPchResetInstance->PchResetInstance);
  PchResetConstructor (PchResetInstance);

  ///
  /// Initialize the Reset ppi instance
  ///
  PchResetInstance->PchResetInterface.PchResetPpi.Reset = Reset;
  PeiPchResetInstance->PpiDescriptor.Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PeiPchResetInstance->PpiDescriptor.Guid   = &gPchResetPpiGuid;
  PeiPchResetInstance->PpiDescriptor.Ppi    = &(PchResetInstance->PchResetInterface.PchResetPpi);

  ///
  /// Install the PCH RESET PPI
  ///
  Status = PeiServicesInstallPpi (&PeiPchResetInstance->PpiDescriptor);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "InstallPchReset() End\n"));

  return Status;
}

/**
  Execute call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @exception EFI_UNSUPPORTED      Do not do any reset from PCH
**/
EFI_STATUS
PchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  EFI_STATUS              Status;
  UINTN                   Instance;
  PCH_RESET_CALLBACK_PPI  *PchResetCallbackPpi;

  if ((PchResetType == GlobalReset) || (PchResetType == GlobalResetWithEc)) {
    ///
    /// After MRC is done, DRAM Init Done message will be sent to ME FW.
    ///
    Status = PeiServicesLocatePpi (
              &gEfiPeiMemoryDiscoveredPpiGuid,
              0,
              NULL,
              NULL
              );

    if (Status == EFI_SUCCESS) {
      ///
      /// After sending DRAM Init Done to ME FW, please do the global reset through HECI.
      ///
      DEBUG ((DEBUG_ERROR, "Please do the global reset through HECI \n"));
      return EFI_UNSUPPORTED;
    }
  }

  Instance = 0;
  do {
    Status = PeiServicesLocatePpi (
              &gPchResetCallbackPpiGuid,
              Instance,
              NULL,
              (VOID**) &PchResetCallbackPpi
              );

    switch (Status) {
    case EFI_SUCCESS:
      PchResetCallbackPpi->ResetCallback (PchResetType);
      break;
    case EFI_NOT_FOUND:
      break;
    default:
      ASSERT_EFI_ERROR (Status);
      break;
      }
    ++Instance;
  } while (Status == EFI_SUCCESS);

  return EFI_SUCCESS;
}