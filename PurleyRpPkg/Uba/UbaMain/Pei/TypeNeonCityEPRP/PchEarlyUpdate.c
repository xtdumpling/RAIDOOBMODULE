//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    Pch Early update.

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "UbaMainPei.h"

#include <Library/UbaPchEarlyUpdateLib.h>

#include <PchAccess.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>

EFI_STATUS
TypeNeonCityEPRPPchLanConfig (
  IN SYSTEM_CONFIGURATION         *SystemConfig
)
{
  GpioSetOutputValue (GPIO_SKL_H_GPP_I9, (UINT32)SystemConfig->LomDisableByGpio);

  return EFI_SUCCESS;
}

EFI_STATUS
TypeNeonCityEPRPOemInitLateHook (
  IN SYSTEM_CONFIGURATION         *SystemConfig
)
{
//
// TODO for board specific
//
  return EFI_SUCCESS;
}


PLATFORM_PCH_EARLY_UPDATE_TABLE  TypeNeonCityEPRPPchEarlyUpdateTable = 
{
  PLATFORM_PCH_EARLY_UPDATE_SIGNATURE,
  PLATFORM_PCH_EARLY_UPDATE_VERSION,
  TypeNeonCityEPRPPchLanConfig,
  TypeNeonCityEPRPOemInitLateHook
};


/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
EFIAPI
TypeNeonCityEPRPPchEarlyUpdate(
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
  )
{
  EFI_STATUS                            Status;

  Status = PeiServicesLocatePpi (
            &gUbaConfigDatabasePpiGuid, 
            0,
            NULL,
            &UbaConfigPpi
            );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigPpi->AddData (
                               UbaConfigPpi,
                               &gPlatformPchEarlyConfigDataGuid, 
                               &TypeNeonCityEPRPPchEarlyUpdateTable, 
                               sizeof(TypeNeonCityEPRPPchEarlyUpdateTable)
                               );

  return Status;
}
