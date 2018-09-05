//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    ACPI static data update.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "StaticSkuDataDxe.h"
#include <Library/UbaAcpiUpdateLib.h>

// APTIOV_SERVER_OVERRIDE_RC_START : Change the name as per AMI build
//#include "EPRPPlatform.offset.h"
#include "DsdtAsl.offset.h"
// APTIOV_SERVER_OVERRIDE_RC_END : Change the name as per AMI build


ACPI_FIX_UPDATE_TABLE  FixupTableUpdate = 
{
  PLATFORM_ACPI_FIX_UPDATE_SIGNATURE,
  PLATFORM_ACPI_FIX_UPDATE_VERSION,
  // APTIOV_SERVER_OVERRIDE_RC_START : Change the name as per AMI build
  &DSDTPatchOffsetTable
  // APTIOV_SERVER_OVERRIDE_RC_END : Change the name as per AMI build
};

EFI_STATUS
InstallAcpiFixupTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
)
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  
  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformAcpiFixTableGuid, 
                                     &FixupTableUpdate, 
                                     sizeof(FixupTableUpdate)
                                     );

  return Status;
}
