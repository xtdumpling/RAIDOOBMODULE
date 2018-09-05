/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  2013 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


--*/

#include <PiDxe.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/PlatformSaltLib.h>
extern EFI_GUID gEfiMfgUUIDHobGuid;

UINT8  mUuid[16] = {0,};

/**
  The constructor function caches platform 16-byte UUID as platform salt.

  The constructor function locates platform info hob.
  It will ASSERT() if that operation fails and it will always return EFI_SUCCESS.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
InitializePlatformSaltLib (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{
  UINT8                           *SystemUuid = NULL;
  EFI_HOB_GUID_TYPE               *GuidHob;
  
  GuidHob       = GetFirstGuidHob (&gEfiMfgUUIDHobGuid);
  ASSERT (GuidHob != NULL);
  SystemUuid  = GET_GUID_HOB_DATA (GuidHob);
  CopyMem(mUuid, SystemUuid, sizeof (mUuid));

  return EFI_SUCCESS;
}


/**
  Gets platform salt.

  This function retrieves the platform specific salt. A pointer to the new allocated salt
  buffer will be returned if it can successfully get the platform salt; otherwise, it will
  return NULL.

  @param[out]  SaltSize              The size of the output salt buffer.

  @return The allocated salt buffer or NULL if platform salt is not supported.

**/
VOID *
EFIAPI
GetPlatformSalt (
  OUT UINTN      *SaltSize           
  )
{
  if (SaltSize == NULL) {
    DEBUG ((DEBUG_ERROR, "\nError!!! SaltSize cannot be NULL!!!\n\n"));
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return NULL;
  }

  *SaltSize = sizeof (mUuid);

  DEBUG ((DEBUG_INFO, "Get UUID %g as platform salt for password\n", mUuid));
  
  return AllocateCopyPool (sizeof (mUuid), mUuid);
}