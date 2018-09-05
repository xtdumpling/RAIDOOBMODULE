/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UbaAcpiUpdateLib.c

Abstract:

--*/

#include <Base.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Library/UbaAcpiUpdateLib.h>

#include <Protocol/UbaCfgDb.h>

EFI_STATUS
PlatformGetAcpiFixTableDataPointer (
  IN  VOID                               **TablePtr
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  ACPI_FIX_UPDATE_TABLE                   AcpiFixUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength  = sizeof (AcpiFixUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformAcpiFixTableGuid,
                                    &AcpiFixUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (AcpiFixUpdateTable.Signature == PLATFORM_ACPI_FIX_UPDATE_SIGNATURE);
  ASSERT (AcpiFixUpdateTable.Version == PLATFORM_ACPI_FIX_UPDATE_VERSION);

  *TablePtr = AcpiFixUpdateTable.TablePtr;

  return EFI_SUCCESS;
}


