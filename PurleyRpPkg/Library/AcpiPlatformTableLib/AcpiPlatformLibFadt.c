/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2015, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file AcpiPlatformLibFadt.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"

extern UINT8                 mKBPresent;
extern UINT8                 mMousePresent;
extern SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;



EFI_STATUS
PatchFadtTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   )
{
  UINT16                                    LegacyDevice;
  // APTIOV_SERVER_OVERRIDE_RC_START
//  EFI_LPC_POLICY_PROTOCOL                   *LpcPolicy;
  // APTIOV_SERVER_OVERRIDE_RC_END
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *FadtHeader;
  EFI_STATUS                                Status;
  UINT8                                     PcieGlobalAspm;
  Status = GetOptionData (&gEfiSocketIioVariableGuid, OFFSET_OF(SOCKET_IIO_CONFIGURATION, PcieGlobalAspm), &PcieGlobalAspm, sizeof(UINT8));
  if (EFI_ERROR (Status)) {
    PcieGlobalAspm = 0x2;
  }
  
  //
  // Patch FADT for legacy free
  //
  LegacyDevice  = 0;
  FadtHeader    = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *) Table;

  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Remove LpcPlatform component. Pilot3 has a COM port, so setting the Legacy device
  // flag in FADT/FACP.
  //
  #if 0
  Status        = gBS->LocateProtocol (&gEfiLpcPolicyProtocolGuid, NULL, &LpcPolicy);

  if (!EFI_ERROR (Status)) {
    if (LpcPolicy->PolicyCom.Enable[0] ||
        LpcPolicy->PolicyCom.Enable[1] ||
        LpcPolicy->PolicyParallel.Enable[0] ||
        LpcPolicy->PolicyFloppy.Enable[0]
        ) {
  #endif
      LegacyDevice |= 1;      // set LEGACY_DEVICES FADT flag
  #if 0
    }

    if (LpcPolicy->PolicyKeyboard.Enable) {
      mKBPresent     = 1;      // copy later to ACPI NVS
      LegacyDevice |= 2;      // set 8042 FADT flag
    }

    if (LpcPolicy->PolicyMouse.Enable) {
      mMousePresent  = 1;      // copy later to ACPI NVS
      LegacyDevice |= 2;      // set 8042 FADT flag
    }
  }
  #endif

  //
  // APTIOV_SERVER_OVERRIDE_RC_END : Remove LpcPlatform component. Pilot3 has a COM port, so setting the Legacy device
  //

  //
  // If Pcie Global ASPM is disabled, then set disabled bit in FADT
  //
  if (PcieGlobalAspm == 0) {
    LegacyDevice |= (1 << 4); // Prevent OS from tampering with ASPM settings
  }
// APTIOV_SERVER_OVERRIDE_RC_START : Do not overwrite IaPcBootArch, append the flags.
  FadtHeader->IaPcBootArch |= LegacyDevice;
// APTIOV_SERVER_OVERRIDE_RC_END : Do not overwrite IaPcBootArch, append the flags.
  FadtHeader->Flags |= (mSocketProcessorCoreConfiguration.ForcePhysicalModeEnable) ? EFI_ACPI_3_0_FORCE_APIC_PHYSICAL_DESTINATION_MODE : 0;
  //FadtHeader->Flags |= EFI_ACPI_3_0_FORCE_APIC_CLUSTER_MODEL;

  return EFI_SUCCESS;
}
