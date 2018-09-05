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


  @file AcpiPlatformLibSvos.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


EFI_SV_SERVICE_PROTOCOL       *mSvService;

/**

    Update the SVOS ACPI table

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchSvosAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS        Status;
  UINT64            Address;
  UINTN             idx;
  UINT8             checksum;
  EFI_SVOS_ACPI_DESCRIPTION_TABLE *SvosAcpiTable;


  Address = 0xffffffff;
  SvosAcpiTable = (EFI_SVOS_ACPI_DESCRIPTION_TABLE *)Table;

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   1,     //page
                   &Address
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //update SVOS ACPI table
  SvosAcpiTable->SvSmmGas.Address = Address;

  //update checksum
  SvosAcpiTable->Header.Checksum = 0;
  checksum = 0;
  for(idx = 0; idx < sizeof(EFI_SVOS_ACPI_DESCRIPTION_TABLE); idx++) {
    checksum = checksum + (UINT8) (((UINT8 *)(SvosAcpiTable))[idx]);
  }
  SvosAcpiTable->Header.Checksum = (UINT8) (0 - checksum);

  //DEBUG ((EFI_D_ERROR, "AcpiHook get SvSmm Addr!!!! %x\n", Address));

  // Update SV SMM Data struct for SVOS ACPI GAS address
  Status = gBS->LocateProtocol (&gEfiSvSmmProtocolGuid, NULL, &mSvService);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nAcpiPlatformHooks Cannot find gEfiSvSmmProtocolGuid!\n"));
    return Status;
  }
  mSvService->AcpiService.SetSvosGasAddress(Address);
  mSvService->AcpiService.SetSvosSmiDoorBell(SvosAcpiTable->SmiDoorBell);

  //DEBUG ((EFI_D_ERROR, "ACPIHook SVSMMAddr %x\n", Address));

  return Status;

}
