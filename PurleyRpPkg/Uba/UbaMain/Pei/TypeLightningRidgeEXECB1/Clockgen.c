//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    ACPI table pcds update.

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

#include <Library/UbaClkGenUpdateLib.h>

//
// No External clockgen, Use ICC Hybrid mode
//

// PLATFORM_CLOCKGEN_UPDATE_TABLE    ClockgenTable = 
// {
//   PLATFORM_CLOCKGEN_UPDATE_SIGNATURE,
//   PLATFORM_CLOCKGEN_UPDATE_VERSION,
//   07,                       // Clockgen ID register offset
//   0x26,                     // Clockgen ID
//   12,                       // Number of clockgen data for write
//   04,                       // Spread Spectrum Byte Offset
//   03,                       // Spread Spectrum Value BIT0+BIT1
//   {
//     0xFF, 
//     0x9E, 
//     0x3F, 
//     0x00, 
//     0x00, 
//     0x0F, 
//     0x08, 
//     0x11, 
//     0x0A, 
//     0x17, 
//     0xFF, 
//     0xFE
//   }  
// };


EFI_STATUS
TypeLightningRidgeEXECB1InstallClockgenData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  
//   Status = UbaConfigPpi->AddData (
//                                  UbaConfigPpi,
//                                  &gPlatformClockgenConfigDataGuid, 
//                                  &ClockgenTable, 
//                                  sizeof(ClockgenTable)
//                                  );

  return Status;
}

