/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MePolicyInitDxe.c

@brief:
  This file is SampleCode for Intel ME DXE Sps Policy initialization.


**/
#include <Library/MmPciBaseLib.h>
#include "MePolicyInitDxe.h"
#include "Protocol/HeciProtocol.h"
#include "MeState.h"
#include "Library/MeTypeLib.h"
#include "Library/MeSpsPolicyInitDxeLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/DxePolicyUpdateLib/DxeMePolicyUpdate.h>
#include <Library/UefiBootServicesTableLib.h>

SPS_POLICY_PROTOCOL *gDxeSpsPolicy = NULL;

#if AMT_SUPPORT
#include "MeBiosExtensionSetup.h"
#include "MeBiosPayloadData.h"
#include "MeBiosPayloadHob.h"
#include <Library/HobLib.h>
#include "Library/DxeAmtPolicyLib.h"
#include <Library/DxeAmtPolicyUpdateLib.h>
#endif // AMT_SUPPORT

#include <Protocol/MePolicy.h>
#include <Library/DxeMePolicyLib.h>
#include <Library/DxeMePolicyUpdateLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED ME_POLICY_PROTOCOL mMePolicyProtocol = { 0 };

///
/// Function implementations
///
/**

  @brief
  Initialize Intel ME DXE Sps Policy

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
MePolicyInitDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS              Status;
#if AMT_SUPPORT
  AMT_POLICY_PROTOCOL     *DxeAmtPolicy;
#endif // AMT_SUPPORT

  DEBUG ((DEBUG_ERROR | DEBUG_INFO, "[ME Policy] ME Sps Policy (Dxe) Entry Point\n"));

  //
  // Call MeCreatePolicyDefaults to initialize platform policy structure
  // and get all default policy settings.
  //
  Status = MeCreatePolicyDefaults (&mMePolicyProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME Policy] Error: Cannot create ME Policy defaults (%r)\n", Status));
    ASSERT_EFI_ERROR (Status);
  }

  UpdateDxeMePolicy (&mMePolicyProtocol);

  mMePolicyProtocol.MeReportError = ShowMeReportError;


  //
  // Install MeInstallPolicyProtocol.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = MeInstallPolicyProtocol (ImageHandle, &mMePolicyProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME Policy] Error: Cannot install ME Policy (%r)\n", Status));
    ASSERT_EFI_ERROR (Status);
  }

#if AMT_SUPPORT
  DxeAmtPolicy = (AMT_POLICY_PROTOCOL *) AllocateZeroPool (sizeof (AMT_POLICY_PROTOCOL));
  if (DxeAmtPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  Status = AmtCreatePolicyDefaults(DxeAmtPolicy);
  if (EFI_ERROR(Status)) {
    if (MeTypeIsAmt()) {
      DEBUG ((DEBUG_ERROR, "[ME Policy] Error: Cannot create Amt Policy defaults (%r)\n", Status));
    } else {
      DEBUG ((DEBUG_WARN, "[ME Policy] Warning: Cannot create Amt Policy defaults (%r)\n", Status));
    }
    FreePool(DxeAmtPolicy);
    DxeAmtPolicy = NULL;
  } else {
    Status = AmtInstallPolicyProtocol(ImageHandle, DxeAmtPolicy);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "[ME Policy] Error: Cannot install Amt Policy (%r)\n", Status));
      ASSERT_EFI_ERROR (Status);
    }

    UpdateDxeAmtPolicy(DxeAmtPolicy);
  }

  SaveMeTypeInSetup();

#endif // AMT_SUPPORT

  gDxeSpsPolicy = (SPS_POLICY_PROTOCOL *) AllocateZeroPool (sizeof (SPS_POLICY_PROTOCOL));
  if (gDxeSpsPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = SpsPolicyInit(ImageHandle, gDxeSpsPolicy);

  if (!EFI_ERROR(Status))
  {
    gDxeSpsPolicy->MePolicyDump(  DEBUG_INFO,
                                  ME_POLICY_DUMP_ME
#if AMT_SUPPORT
                                + ME_POLICY_DUMP_AMT
#endif // AMT_SUPPORT
#if SPS_SUPPORT
                                + ME_POLICY_DUMP_SPS
#endif // SPS_SUPPORT
                                );
  }

  return Status;
}
