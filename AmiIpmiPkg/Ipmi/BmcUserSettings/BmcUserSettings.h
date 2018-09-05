//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file BmcUserSettings.h
    Includes for BMC User Settings.

**/

#ifndef _BMC_USER_SETTINGS_H_
#define _BMC_USER_SETTINGS_H_

//----------------------------------------------------------------------

#include <Protocol/IPMITransportProtocol.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Include/ServerMgmtSetupVariable.h>
#include <ServerMgmtSetup.h>
#include <Include/IpmiBmcCommon.h>
#include <Library/BaseLib.h>
#include <Include/AmiDxeLib.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>

//----------------------------------------------------------------------

//
// Macro definitions
//
#define MAX_BMC_USER_COUNT        64
#define MAX_BMC_USER_NAME_LENGTH  16
#define MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER  17

#define LAN_CHANNEL_TYPE 4
#define MAX_CHANNEL_NUMBER 0x0F

#define MAX_BMC_USER_PASSWORD_LENGTH  BMC_USER_PASSWORD_MAX_SIZE
#define MAX_BMC_USER_PASSWORD_LENGTH_WITH_NULL_CHARACTER  BMC_USER_PASSWORD_MAX_SIZE+1

#define STRING_BUFFER_LENGTH    4000

#pragma pack(1)
typedef struct _BMC_USER_DETAILS_LIST {
  UINT8     UserId;
  CHAR8     UserName[MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER];
}BMC_USER_DETAILS_LIST;

typedef struct _BMC_USER_DETAILS {
  UINT8       UserId;
  CHAR8       UserName[MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER];
  CHAR8       UserPassword[MAX_BMC_USER_PASSWORD_LENGTH_WITH_NULL_CHARACTER];
}BMC_USER_DETAILS;

#pragma pack()
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
