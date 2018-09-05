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

  UbaOpromUpdateLib.c

Abstract:

--*/

#include <Base.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Library/UbaOpromUpdateLib.h>

#include <Protocol/UbaCfgDb.h>


BOOLEAN
PlatformCheckPcieRootPort (
  IN  UINTN                 Bus,
  IN  UINT32                PcieSlotOpromBitMap
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return TRUE;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return TRUE;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  return OptionRomUpdateTable.CallCheckRootPort (Bus, PcieSlotOpromBitMap);
}

EFI_STATUS
PlatformGetOptionRomTable (
  IN  PC_PCI_OPTION_ROM_TABLE         **OptionRomTable
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  if (OptionRomUpdateTable.GetOptionRomTable == NULL) {
    return EFI_NOT_FOUND;
  }

  return OptionRomUpdateTable.GetOptionRomTable (OptionRomTable);
}

EFI_STATUS
PlatformGetNicSetupConfigTable (
  IN  NIC_SETUP_CONFIGURATION_STUCT     **NicSetupConfigTable,
  IN  UINTN                             *NumOfConfig
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  if (OptionRomUpdateTable.GetNicSetupConfigTable == NULL) {
    return EFI_NOT_FOUND;
  }

  return OptionRomUpdateTable.GetNicSetupConfigTable (NicSetupConfigTable, NumOfConfig);
}

EFI_STATUS
PlatformGetNicCapabilityTable (
  IN  NIC_OPTIONROM_CAPBILITY_STRUCT    **NicCapabilityTable,
  IN  UINTN                             *NumOfNicCapTable
  )
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  if (OptionRomUpdateTable.GetNicCapabilityTable == NULL) {
    return EFI_NOT_FOUND;
  }

  return OptionRomUpdateTable.GetNicCapabilityTable (NicCapabilityTable, NumOfNicCapTable);
}

EFI_STATUS
PlatformSetupPcieSlotNumber (
  OUT  UINT8                   *PcieSlotItemCtrl
)
{
  EFI_STATUS                              Status;

  UBA_CONFIG_DATABASE_PROTOCOL      *UbaConfigProtocol = NULL;
  UINTN                                   DataLength = 0;
  PLATFORM_OPTION_ROM_UPDATE_DATA         OptionRomUpdateTable;
  
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DataLength  = sizeof (OptionRomUpdateTable);
  Status = UbaConfigProtocol->GetData (
                                    UbaConfigProtocol,
                                    &gPlatformOptionRomUpdateConfigDataGuid,
                                    &OptionRomUpdateTable,
                                    &DataLength
                                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (OptionRomUpdateTable.Signature == PLATFORM_OPTION_ROM_UPDATE_SIGNATURE);
  ASSERT (OptionRomUpdateTable.Version == PLATFORM_OPTION_ROM_UPDATE_VERSION);

  return OptionRomUpdateTable.SetupSlotNumber (PcieSlotItemCtrl);
}

