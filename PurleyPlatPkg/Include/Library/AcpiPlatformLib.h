//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++
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


Module Name:

  AcpiPlatformLib.h
  
Abstract:

--*/

#ifndef _ACPI_PLATFORM_LIB_H_
#define _ACPI_PLATFORM_LIB_H_

//
// Statements that include other header files
//
#include <PiDxe.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

#include <IndustryStandard/Acpi.h>

#include <Protocol/MpService.h>

#include <Protocol/AcpiSystemDescriptionTable.h>


EFI_STATUS
AcpiPlatformHooksIsActiveTable (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  );

/*++

Routine Description:

  Called for every ACPI table found in the BIOS flash.
  Returns whether a table is active or not. Inactive tables
  are not published in the ACPI table list. This hook can be
  used to implement optional SSDT tables or enabling/disabling
  specific functionality (e.g. SPCR table) based on a setup
  switch or platform preference. In case of optional SSDT tables,
  the platform flash will include all the SSDT tables but will
  return EFI_SUCCESS only for those tables that need to be
  published.
  This hook can also be used to update the table data. The header
  is updated by the common code. For example, if a platform wants
  to use an SSDT table to export some platform settings to the
  ACPI code, it needs to update the data inside that SSDT based
  on platform preferences in this hook.

Arguments:

  None

Returns:

  Status  - EFI_SUCCESS if the table is active
  Status  - EFI_UNSUPPORTED if the table is not active
*/

/**

  This function will update any runtime platform specific information.
  This currently includes:
    Setting OEM table values, ID, table ID, creator ID and creator revision.
    Enabling the proper processor entries in the APIC tables.

  @param Table  -  The table to update

  @retval EFI_SUCCESS  -  The function completed successfully.

**/
EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  ,IN OUT EFI_ACPI_TABLE_VERSION     *Version
  );


#endif
