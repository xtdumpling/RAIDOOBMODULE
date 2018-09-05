/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SetupCfgUpdateDxe.c

Abstract:

--*/
#include "SetupCfgUpdateDxe.h"
#include <Library/UbaSetupUpdateLib.h>

EFI_STATUS
UpdateFormBeforeSetup (
  VOID
)
{
  EFI_STATUS                                Status;

  Status = EFI_NOT_FOUND;
  
  return EFI_NOT_FOUND;
}

EFI_STATUS
FormUpdateCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  )
{
  EFI_STATUS                                Status;

  Status = EFI_NOT_FOUND;
  
  return EFI_NOT_FOUND;
}


PLATFORM_SETUP_UPDATE_DATA  SetupUpdateTable = 
{
  PLATFORM_SETUP_UPDATE_SIGNATURE,
  PLATFORM_SETUP_UPDATE_VERSION,
  UpdateFormBeforeSetup,
  FormUpdateCallback
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
SetupConfigUpdateEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                                Status;
  UBA_CONFIG_DATABASE_PROTOCOL        *UbaConfigProtocol = NULL;

  DEBUG((EFI_D_ERROR, "UBA:SETUPConfigUpdate-TypeNeonCityFPGA\n"));
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
                                     &gPlatformSetupUpdateConfigDataGuid, 
                                     &SetupUpdateTable, 
                                     sizeof(SetupUpdateTable)
                                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  return Status;
}
