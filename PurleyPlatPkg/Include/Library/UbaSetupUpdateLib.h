/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2008 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UbaSetupUpdateLib.h

Abstract:

--*/

#ifndef _PLATFORM_SETUP_UPDATE_LIB_H_
#define _PLATFORM_SETUP_UPDATE_LIB_H_


#include <Base.h>
#include <Uefi.h>

#include <Protocol/HiiConfigAccess.h>

#define PLATFORM_SETUP_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'S', 'E', 'T')
#define PLATFORM_SETUP_UPDATE_VERSION    01


// {CD1F9574-DD03-4196-96AD-4965146F9665}
STATIC EFI_GUID  gPlatformSetupUpdateConfigDataGuid = 
{ 0xcd1f9574, 0xdd03, 0x4196, { 0x96, 0xad, 0x49, 0x65, 0x14, 0x6f, 0x96, 0x65 } };


typedef
EFI_STATUS
(*PLATFORM_SETUP_UPDATE_FORM_BEFORE_SETUP) (
  VOID
);

typedef
EFI_STATUS
(*PLATFORM_SETUP_FORM_UPDATE_CALLBACK) (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  );

typedef struct 
{
  UINT32                                    Signature;
  UINT32                                    Version;
  
  PLATFORM_SETUP_UPDATE_FORM_BEFORE_SETUP   UpdateFormBeforeSetup;
  PLATFORM_SETUP_FORM_UPDATE_CALLBACK       FormUpdateCallback;
  
} PLATFORM_SETUP_UPDATE_DATA;

EFI_STATUS
PlatformUpdateFormBeforeSetup (
  VOID
);

EFI_STATUS
PlatformFormUpdateCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  );

EFI_STATUS
PlatformInitSetupUpdateData (
  VOID
);

EFI_STATUS
PlatformGetMainHiiHandle (
  IN  EFI_HII_HANDLE                          *HiiHandle
  );

EFI_STATUS
PlatformGetAdvanceHiiHandle (
  IN  EFI_HII_HANDLE                          *HiiHandle
  );

EFI_STATUS
PlatformGetErrorManagerHiiHandle (
  IN  EFI_HII_HANDLE                          *HiiHandle
  );

BOOLEAN
PlatformIsMainForm (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *ConfigAccess
  );

BOOLEAN
PlatformIsAdvanceForm (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *ConfigAccess
  );

BOOLEAN
PlatformIsErrorManagerForm (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *ConfigAccess
  );

 
#endif //_PLATFORM_SETUP_UPDATE_LIB_H_
