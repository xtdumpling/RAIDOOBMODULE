/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MeSpsPolicyUpdateDxeLib.h

  Header file for DXE MeSpsPolicyUpdate Library.

**/
#ifndef _ME_SPS_POLICY_UPDATE_DXE_LIB_H_
#define _ME_SPS_POLICY_UPDATE_DXE_LIB_H_

#include <Protocol/MeSpsPolicyProtocol.h>
#include "Library/MeTypeLib.h"

#ifdef AMT_SUPPORT

#include "MeBiosExtensionSetup.h"

/**

  Check if AMT is enabled in setup options.

  @param[in] None.

  @retval EFI_SUCCESS             AMT (SPS) policy is initialized.
  @retval All other error conditions encountered when no AMT (SPS) policy available.

**/
EFI_STATUS
AmtPolicyLibInit (
  VOID
  )
;


/**

  Dump DXE Amt Policy

  @param[in] None.

**/
VOID
DxeAmtPolicyDebugDump (
  VOID
  )
;
#endif // AMT_SUPPORT

/**

  Install the ME SPS Policy Library

  @param SpsPolicy              ME SPS Policy in DXE phase

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
InstallSpsPolicyLibrary (
  IN OUT  SPS_POLICY_PROTOCOL *SpsPolicy
  )
;

#define AMT_BITMASK   0x00000004
#define CLEAR_AMT_BIT 0x00000000

#define MAX_FW_UPDATE_BIOS_SELECTIONS 2

#define EFI_NVRAM_UPDATE_GUID \
{ \
  0xd84beff0, 0x159a, 0x4b60, 0x9a, 0xb9, 0xac, 0x5c, 0x47, 0x4b, 0xd3, 0xb1 \
}

/**

  Function stores Me SPS Policy to Setup configuration

  @param SpsPolicyInstance           ME SPS olicy instance to save
  @param saveMask                    What to save

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_INVALID_PARAMETER Wrong input parameters

**/
EFI_STATUS
SaveSpsPolicyToSetupConfiguration(
  SPS_POLICY_PROTOCOL *SpsPolicyInstance
)
;

/**

  Update Me SPS policy while MeSpsProtocol is installed.

  @param SpsPolicyInstance        - ME SPS Policy
  @param pSetupData               - pointer to system configuration
  @param resetRequest             - Returns information if function requests reset

  @retval Efi Status

**/
EFI_STATUS
UpdateSpsPolicyFromSetup (
  SPS_POLICY_PROTOCOL    *SpsPolicyInstance,
  ME_RC_CONFIGURATION    *pSetupData,
  BOOLEAN                *resetRequest
  )
;

#endif // _ME_SPS_POLICY_UPDATE_DXE_LIB_H_
