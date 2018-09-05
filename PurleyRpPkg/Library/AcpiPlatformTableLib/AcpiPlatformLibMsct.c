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


  @file AcpiPlatformLinMsct.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"
//
// APTIOV_SERVER_OVERRIDE_RC_START : Added to report correct Physical address bits for Purley.
//
#include <Library/HobLib.h>
//
// APTIOV_SERVER_OVERRIDE_RC_END : Added to report correct Physical address bits for Purley.
//

/**

    Update the MSCT ACPI table

    @param *MsctAcpiTable - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchMsctAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  )
{
  UINTN             idx;
  UINT8             checksum;
  EFI_ACPI_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE *MsctAcpiTable;
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Added to report correct Physical address bits for Purley.
  //
  UINT8             PhysicalAddressBits;
  VOID*             Hob;
  //
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to report correct Physical address bits for Purley.
  //

  MsctAcpiTable = (EFI_ACPI_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE *)Table;

  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Added to report correct Physical address bits for Purley.
  //
  //
  // Get physical address bits supported.
  //
  Hob = GetFirstHob (EFI_HOB_TYPE_CPU);
  if (Hob != NULL) {
    PhysicalAddressBits = ((EFI_HOB_CPU *) Hob)->SizeOfMemorySpace;
  } else {
    PhysicalAddressBits = PcdGet8(PcdMaxProcessorPhysicalAddressBits);
  }
  //
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to report correct Physical address bits for Purley.
  //

  // Update Maximum Physical Address
  // NHM-EX has 44 address lines, so Maximum Physical Address is 2^44 - 1.
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Added to report correct Physical address bits for Purley.
  //
  MsctAcpiTable->MaxPhysicalAddress = (LShiftU64 (0x01, PhysicalAddressBits) - 1);
  //
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to report correct Physical address bits for Purley.
  //

  // First Proximity Domain Information Structure reports characteristics for all proximity domains,
  // since the characteristics are the same for all proximity domains.
  MsctAcpiTable->ProxDomInfoStructure[0].ProxDomRangeLow  = 0;
  MsctAcpiTable->ProxDomInfoStructure[0].ProxDomRangeHigh = MsctAcpiTable->MaxNumProxDom;

  // Max Number of Threads that the processor can have
  MsctAcpiTable->ProxDomInfoStructure[0].MaxProcessorCapacity = (MAX_THREAD * MAX_CORE);

  // Max Memory capacity per proximity domain
  MsctAcpiTable->ProxDomInfoStructure[0].MaxMemoryCapacity = MsctAcpiTable->MaxPhysicalAddress; // 2 TB

  // Update Checksum
  MsctAcpiTable->Header.Checksum = 0;
  checksum = 0;
  for(idx = 0; idx < sizeof(EFI_ACPI_MAXIMUM_SYSTEM_CHARACTERISTICS_TABLE); idx++) {
    checksum = checksum + (UINT8) (((UINT8 *)(MsctAcpiTable))[idx]);
  }
  MsctAcpiTable->Header.Checksum = (UINT8) (0 - checksum);

  return EFI_SUCCESS;
}
