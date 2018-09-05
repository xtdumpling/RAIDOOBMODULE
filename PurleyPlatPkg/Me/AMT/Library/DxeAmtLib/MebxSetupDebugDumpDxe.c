/** @file
  Dump whole ME_BIOS_EXTENSION_SETUP and serial out.

@copyright
 Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
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

#include "AmtDxeLibInternals.h"

/**
  Dump ME_BIOS_EXTENSION_SETUP variable

  @param[in] MeBiosExtensionSetup   Pointer to ME_BIOS_EXTENSION_SETUP variable

**/
VOID
DxeMebxSetupVariableDebugDump (
  IN ME_BIOS_EXTENSION_SETUP      *MeBiosExtensionSetup OPTIONAL
  )
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;
  ME_BIOS_EXTENSION_SETUP   MeBxSetup;
  ME_BIOS_EXTENSION_SETUP   *MeBxSetupPtr;

  if (MeBiosExtensionSetup == NULL) {
    Status = gRT->GetVariable (
              ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
              &gMeBiosExtensionSetupGuid,
              NULL,
              &VariableSize,
              &MeBxSetup
              );
    if (EFI_ERROR(Status)) {
      return;
    }
    MeBxSetupPtr = &MeBxSetup;
  } else {
    MeBxSetupPtr = MeBiosExtensionSetup;
  }

  DEBUG ((DEBUG_INFO, "\n------------------------ MeBiosExtensionSetup Dump Begin -----------------\n"));
  DEBUG ((DEBUG_INFO, " InterfaceVersion : 0x%x\n", MeBxSetupPtr->InterfaceVersion));
  DEBUG ((DEBUG_INFO, " Flags : 0x%x\n", MeBxSetupPtr->Flags));
  DEBUG ((DEBUG_INFO, " PlatformMngSel : 0x%x\n", MeBxSetupPtr->PlatformMngSel));
  DEBUG ((DEBUG_INFO, " AmtSol : 0x%x\n", MeBxSetupPtr->AmtSol));
  DEBUG (
    (DEBUG_INFO,
    " RemoteAssistanceTriggerAvailablilty : 0x%x\n",
    MeBxSetupPtr->RemoteAssistanceTriggerAvailablilty)
    );
  DEBUG ((DEBUG_INFO, " KvmEnable : 0x%x\n", MeBxSetupPtr->KvmEnable));
  DEBUG ((DEBUG_INFO, " MebxDefaultSol : 0x%x\n", MeBxSetupPtr->MebxDefaultSol));

  DEBUG ((DEBUG_INFO, "\n------------------------ MeBiosExtensionSetup Dump End -------------------\n"));
}
