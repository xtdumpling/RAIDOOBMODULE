//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  PlatformBeepMap implementation.

  Copyright (c) 2011 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <Base.h>
#include <Uefi.h>
#include <Library/PlatformStatusCodes.h>

STATUS_CODE_TO_DATA_MAP mBeepProgressMap[] = {
  //
  // PEI
  //
  // Recovery
  { PEI_RECOVERY_STARTED, 2 },

  //
  // DXE
  //

  {0,0}
};

STATUS_CODE_TO_DATA_MAP mBeepErrorMap[] = {
  //
  // PEI
  //
  //Regular boot
  { PEI_MEMORY_NOT_DETECTED, 3 },
  { PEI_MEMORY_INSTALLED_TWICE, 1 },
  { PEI_DXEIPL_NOT_FOUND, 3 },
  { PEI_DXE_CORE_NOT_FOUND, 3 },
  { PEI_RESET_NOT_AVAILABLE, 7 },
  // Recovery
  { PEI_RECOVERY_FAILED, 4 },
  // S3 Resume
  { PEI_S3_RESUME_FAILED, 4 },

  //
  // DXE
  //
  { DXE_ARCH_PROTOCOL_NOT_AVAILABLE, 4 },
  { DXE_NO_CON_OUT, 5 },
  { DXE_NO_CON_IN, 5 },
  { DXE_INVALID_PASSWORD, 1 },    
  { DXE_FLASH_UPDATE_FAILED, 6 },    
  { DXE_RESET_NOT_AVAILABLE, 7 },    

  {0,0}
};

STATUS_CODE_TO_DATA_MAP *mBeepStatusCodesMap[] = {
  //#define EFI_PROGRESS_CODE 0x00000001
  mBeepProgressMap,
  //#define EFI_ERROR_CODE 0x00000002
  mBeepErrorMap
  //#define EFI_DEBUG_CODE 0x00000003
};

UINT32
FindBeepData (
  IN STATUS_CODE_TO_DATA_MAP *Map,
  IN EFI_STATUS_CODE_VALUE   Value
  )
{
  while (Map->Value != 0) {
    if (Map->Value == Value) {
      return Map->Data;
    }
    Map++;
  }
  return 0;
}

/**
  Get BeepValue from status code type and value.

  @param  CodeType         Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.

  @return BeepValue
**/
UINT32
EFIAPI
GetBeepValueFromStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value
  )
{
  UINT32 CodeTypeIndex;
  
  CodeTypeIndex = STATUS_CODE_TYPE (CodeType) - 1;
  
  if (CodeTypeIndex >= sizeof (mBeepStatusCodesMap) / sizeof(mBeepStatusCodesMap[0])) {
    return 0;
  }
  
  return FindBeepData (mBeepStatusCodesMap[CodeTypeIndex], Value);
}
