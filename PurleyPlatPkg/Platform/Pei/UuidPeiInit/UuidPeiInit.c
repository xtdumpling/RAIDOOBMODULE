/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file UuidPeiInit.c

  Get Uuid value and build a hob for Uuid. If in MFG mode and get variable MfgSystemUuid successfully, then write UUID into PDR.

**/

#include <PiPei.h>
#include <Library/PeiPdrAccessLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Guid/HobList.h>

extern EFI_GUID gEfiMfgUUIDHobGuid;

/**

  Build UUID Hob, if get a new UUID variable in MFG Mode, write it to PDR.

  @param FileHandle         Pointer to the PEIM FFS file header.
  @param PeiServices        General purpose services available to every PEIM.

  @retval EFI_SUCCESS       Operation completed successfully.
  @retval Otherwise         Get UUID or Build hob failed.
**/
EFI_STATUS
EFIAPI
UuidInitEntry (
  IN EFI_PEI_FILE_HANDLE          FileHandle,
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS                      Status;
  EFI_BOOT_MODE                   BootMode = BOOT_WITH_FULL_CONFIGURATION;
  UINT8                           SystemUuid[sizeof (EFI_GUID)]={0};;
  			
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  
  if (BootMode == BOOT_WITH_MFG_MODE_SETTINGS) {
    Status = MfgUuidGet((EFI_PEI_SERVICES**)PeiServices, SystemUuid);
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_INFO, "MfgUuidGet return Status:%r\n", Status));
    }
  } else {
    Status = UuidGetByPdr((EFI_PEI_SERVICES**)PeiServices, SystemUuid);
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_INFO, "UuidGetByPdr return Status:%r\n", Status));
    }
  }
//
// For RP PXD PDK test, the value of UUID cannot be full 00 or FF
//
#ifndef PC_HOOK
  Status = RpUuidRefresh (PeiServices, SystemUuid);
#endif
  //
  // Create the HOB with the UUID
  //
  BuildGuidDataHob (
               &gEfiMfgUUIDHobGuid,
               SystemUuid,
               sizeof(EFI_GUID)
               );

  return Status;
}
