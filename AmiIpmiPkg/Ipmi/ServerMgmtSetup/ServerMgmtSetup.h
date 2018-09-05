//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file ServerMgmtSetup.h
    Server Mgmt setup header file

**/

#ifndef __SERVER_MGMT_SETUP_PAGE_H__
#define __SERVER_MGMT_SETUP_PAGE_H__
#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------

#include "ServerMgmtSetupVariable.h"
#include <ServerMgmtSetupStrDefs.h>
#include <Setup.h>

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

#define EFI_BDS_EVENT_BEFORE_SETUP_GUID  \
  { 0xdbc9fd21, 0xfad8, 0x45b0, { 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93 } };

//-----------------------------------------------------------------------

#ifndef VFRCOMPILE

EFI_STATUS
EFIAPI
IpmiHiiGetString (
  IN       EFI_HII_HANDLE  HiiHandle,
  IN       EFI_STRING_ID   StringId,
  IN       EFI_STRING      String,
  IN       UINTN           *StringSize,
  IN CONST CHAR8           *Language  OPTIONAL
);

#endif//VFRCOMPILE
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
