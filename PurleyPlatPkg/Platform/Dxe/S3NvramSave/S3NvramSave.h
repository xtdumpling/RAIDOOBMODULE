/**

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file S3NvramSave.h

  
**/

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>

#include "SysHost.h"

extern EFI_GUID gEfiMemoryConfigDataHobGuid;
extern EFI_GUID gEfiMemoryConfigDataGuid;

#define MAX_HOB_ENTRY_SIZE  60*1024

EFI_STATUS
EFIAPI
S3NvramSaveEntry ( 
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
);

VOID
SaveS3StructToNvram (
  VOID
);

BOOLEAN
S3NvramCompareStructs (
  IN VOID                   *CurrentBootStruct,
  IN VOID                   *SavedBootStruct
);
