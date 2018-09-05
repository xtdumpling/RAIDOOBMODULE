//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    IIO Config Update.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "IioCfgUpdateDxe.h"

EFI_STATUS
UpdateNeonCityEPRPIioConfig (
  IN  IIO_GLOBALS             *IioGlobalData
  )
{
  return EFI_SUCCESS;
}

PLATFORM_IIO_CONFIG_UPDATE_TABLE  TypeNeonCityEPRPIioConfigTable = 
{
  PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE,
  PLATFORM_IIO_CONFIG_UPDATE_VERSION,
  
  IioBifurcationTable,
  sizeof(IioBifurcationTable),
  UpdateNeonCityEPRPIioConfig,
  IioSlotTable,
  sizeof(IioSlotTable)
  
};

/**
  The Driver Entry Point.
  
  The function is the driver Entry point.
  
  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
IioCfgUpdateEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                               Status;
  UBA_CONFIG_DATABASE_PROTOCOL             *UbaConfigProtocol = NULL;

  DEBUG((EFI_D_ERROR, "UBA:IioCfgUpdate-TypeNeonCityEPRP\n"));
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid, 
                                     &TypeNeonCityEPRPIioConfigTable, 
                                     sizeof(TypeNeonCityEPRPIioConfigTable)
                                     );

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid_1, 
                                     &TypeNeonCityEPRPIioConfigTable, 
                                     sizeof(TypeNeonCityEPRPIioConfigTable)
                                     );

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid_2, 
                                     &TypeNeonCityEPRPIioConfigTable, 
                                     sizeof(TypeNeonCityEPRPIioConfigTable)
                                     );

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformIioConfigDataDxeGuid_3, 
                                     &TypeNeonCityEPRPIioConfigTable, 
                                     sizeof(TypeNeonCityEPRPIioConfigTable)
                                     );
  
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  return Status;
}
