//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    Device data installation.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include "StaticSkuDataDxe.h"

#include <Library/UbaPirqUpdateLib.h>
#include <Library/UbaMpTableUpdateLib.h>

#include <PlatPirqData.h>
#include <PlatDevData.h>

extern PLATFORM_PIRQ_DATA    mPlatformPirqDataPlatformSRP;

PLATFORM_PIRQ_UPDATE_TABLE  PirqUpdate = 
{
  PLATFORM_PIRQ_UPDATE_SIGNATURE,
  PLATFORM_PIRQ_UPDATE_VERSION,
  &mPlatformPirqDataPlatformSRP
};

EFI_STATUS
InstallPirqData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
)
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  
  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol, 
                                     &gPlatformPirqConfigDataGuid, 
                                     &PirqUpdate, 
                                     sizeof(PirqUpdate)
                                     );

  return Status;
}

extern DEVICE_DATA           mDeviceDataPlatformSRP;
extern DEVICE_UPDATE_DATA    mDeviceUpdateDataPlatformSRP;

PLATFORM_MP_UPDATE_TABLE  MpTableUpdate = 
{
  PLATFORM_MP_TABLE_UPDATE_SIGNATURE,
  PLATFORM_MP_TABLE_UPDATE_VERSION,
  &mDeviceDataPlatformSRP,
  &mDeviceUpdateDataPlatformSRP
};

EFI_STATUS
InstallMpTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
)
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  
  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformMpTableConfigDataGuid, 
                                     &MpTableUpdate, 
                                     sizeof(MpTableUpdate)
                                     );

  return Status;
}
