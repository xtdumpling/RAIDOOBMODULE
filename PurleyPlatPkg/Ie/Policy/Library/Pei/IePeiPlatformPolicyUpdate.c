/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IePeiPlatformPolicyUpdate.c

  Do Platform Stage IE initialization.

**/
#include <Library/HobLib.h>
#include <Guid/IeRcVariable.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SetupLib.h>
#include <Ppi/IePlatformPolicyPei.h>
#include "Library/DebugLib.h"
#include "IeHob.h"
#include "Library/IeHeciCoreLib.h"
#include <Library/IePeiPolicyAccessLib.h>
#include <PchAccess.h>  // for V_PCH_DEFAULT_SID


/*

  Create IePolicy Hob

  @param PeiIePlatformPolicyPpi IE policy

  @retval NONE

 */
VOID
CreateIePolicyHob(PEI_IE_PLATFORM_POLICY_PPI  *PeiIePlatformPolicyPpi)
{
  IE_HOB *IeHob;

  if (PeiIePlatformPolicyPpi == NULL) {
    return;
  }

  IeHob = GetFirstGuidHob (&gIeHobGuid);
  if (IeHob == NULL) {
    EFI_STATUS  Status = EFI_SUCCESS;

    ///
    /// Create HOB for ME Data
    ///
    Status = PeiServicesCreateHob (
               EFI_HOB_TYPE_GUID_EXTENSION,
               sizeof (IE_HOB),
               (VOID **) &IeHob
               );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return;
    }

    DEBUG ((DEBUG_INFO, "[IE Policy] IE HOB installed\n"));

    //
    // Initialize default HOB data
    //
    ZeroMem (&(IeHob->IeDisabledInSoftStraps), (sizeof (IE_HOB) - sizeof (EFI_HOB_GUID_TYPE)));
    IeHob->EfiHobGuidType.Name = gIeHobGuid;
  }
  IeHob->IeDisabledInSoftStraps = PeiIePlatformPolicyPpi->IeDisabledInSoftStraps;
  DEBUG ((DEBUG_INFO, "[IE Policy] IE FW HOB data updated (%d)\n", PeiIePlatformPolicyPpi->IeDisabledInSoftStraps));
}

/*

  Install the Ie Platform Policy Library

  @param PeiIePlatformPolicyPpi  Ie Platform Policy Ppi

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
  @retval EFI_NOT_FOUND         Not all required services are loaded. Try later.

*/

EFI_STATUS
UpdatePeiIePlatformPolicy (
  IN OUT  PEI_IE_PLATFORM_POLICY_PPI  *PeiIePlatformPolicyPpi
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
#if defined(IE_SUPPORT) && IE_SUPPORT
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  IE_RC_CONFIGURATION             IeConfiguration;

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
                            &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
                            0,                                // INSTANCE
                            NULL,                             // EFI_PEI_PPI_DESCRIPTOR
                            &VariableServices                 // PPI
                            );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[IE] ERROR: Read-only variable PPI cannot be located (%r)\n", Status));
    ASSERT_EFI_ERROR(Status);
    return EFI_NOT_FOUND;
  }

  Status = GetSpecificConfigGuid (&gEfiIeRcVariableGuid, (VOID *)&IeConfiguration);

  if (Status == EFI_SUCCESS) {
    PeiIePlatformPolicyPpi->IeHeci1Enabled = IeConfiguration.IeHeci1Enabled;
    PeiIePlatformPolicyPpi->IeHeci2Enabled = IeConfiguration.IeHeci2Enabled;
    PeiIePlatformPolicyPpi->IeHeci3Enabled = IeConfiguration.IeHeci3Enabled;
    PeiIePlatformPolicyPpi->IeIderEnabled = IeConfiguration.IeIderEnabled;
    PeiIePlatformPolicyPpi->IeKtEnabled = IeConfiguration.IeKtEnabled;
    PeiIePlatformPolicyPpi->SubsystemVendorId = IeConfiguration.SubsystemVendorId;
    PeiIePlatformPolicyPpi->SubsystemId = IeConfiguration.SubsystemId;
  } else {
    // Can't get Setup configuration, configure default values for Policy
    ZeroMem(PeiIePlatformPolicyPpi, sizeof(PEI_IE_PLATFORM_POLICY_PPI));
    PeiIePlatformPolicyPpi->SubsystemVendorId = V_PCH_INTEL_VENDOR_ID;
    PeiIePlatformPolicyPpi->SubsystemId = V_PCH_DEFAULT_SID;
  }
  PeiIePlatformPolicyPpi->IeDisabledInSoftStraps = !IeIsEnabled();
  if (PeiIePlatformPolicyPpi->IeDisabledInSoftStraps == 1) {
    PeiIePlatformPolicyPpi->IeHeci1Enabled = 0;
    PeiIePlatformPolicyPpi->IeHeci2Enabled = 0;
    PeiIePlatformPolicyPpi->IeHeci3Enabled = 0;
    PeiIePlatformPolicyPpi->IeIderEnabled = 0;
    PeiIePlatformPolicyPpi->IeKtEnabled = 0;
  } else {
    PeiIePlatformPolicyPpi->DfxIe = IeTypeIsDfx();
    if (PeiIePlatformPolicyPpi->DfxIe) {
      PeiIePlatformPolicyPpi->IeHeci1Enabled = 1;
      PeiIePlatformPolicyPpi->IeHeci2Enabled = 1;
      PeiIePlatformPolicyPpi->IeHeci3Enabled = 1;
      PeiIePlatformPolicyPpi->IeIderEnabled = 1;
      PeiIePlatformPolicyPpi->IeKtEnabled = 1;
      DEBUG((DEBUG_INFO, "[IE] DFX detected, enabling all IE devices\n"));
    }
  }
  CreateIePolicyHob(PeiIePlatformPolicyPpi);

#else
  PeiIePlatformPolicyPpi = NULL;
  Status                 = EFI_NOT_FOUND;
#endif

  return Status;
}
