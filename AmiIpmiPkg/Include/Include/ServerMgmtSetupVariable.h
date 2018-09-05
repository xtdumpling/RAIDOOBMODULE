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

/** @file ServerMgmtSetupVariable.h
    ServerMgmtSetup page related definitions

**/

#ifndef _SERVER_MGMT_SETUP_VARIABLE_H
#define _SERVER_MGMT_SETUP_VARIABLE_H

//----------------------------------------------------------------------

#include <Token.h>
#ifndef VFRCOMPILE
extern EFI_GUID gEfiServerMgmtSetupVariableGuid;
#else//VFRCOMPILE
#include<ServerMgmtControlDefinitions.h>
#endif//VFRCOMPILE

#if ServerMgmtSetup_SUPPORT
#include <ServerAutoId.h>
#include <ServerMgmtSetupDataDefinition.h>
#endif

//----------------------------------------------------------------------

//----------------------------------------------------------------------

#define SERVER_MGMT_CONFIGURATION_GUID \
  { 0x1239999, 0xfc0e, 0x4b6e, { 0x9e, 0x79, 0xd5, 0x4d, 0x5d, 0xb6, 0xcd, 0x20 } }

#define SERVER_MGMT_CLASS_ID          20
#define SERVER_MGMT_SUBCLASS_ID        0x00

#define TIME_3MIN                    180    //Time in seconds
#define TIME_4MIN                    240
#define TIME_5MIN                    300
#define TIME_6MIN                    360
#define TIME_10MIN                   600
#define TIME_15MIN                   900
#define TIME_20MIN                  1200

#define SERVER_MGMT_KEY_ID          2048

//
//Define SYSTEM_ACCESS_VARSTORE if it is not defined
//
#ifndef SERVER_MGMT_CONFIGURATION_DATA_VARSTORE
#define SERVER_MGMT_CONFIGURATION_DATA_VARSTORE\
        varstore SERVER_MGMT_CONFIGURATION_DATA, key = SERVER_MGMT_KEY_ID,\
            name = ServerSetup, guid = SERVER_MGMT_CONFIGURATION_GUID;
#endif

//
//Define SYSTEM_ACCESS_VARSTORE if it is not defined
//
#ifndef SYSTEM_ACCESS_VARSTORE
#define SYSTEM_ACCESS_VARSTORE\
        varstore SYSTEM_ACCESS, key = SYSTEM_ACCESS_KEY_ID,\
            name = SystemAccess, guid = SYSTEM_ACCESS_GUID;
#endif

//
//Define SUPPRESS_GRAYOUT_ENDIF if it is not defined
//
#ifndef SUPPRESS_GRAYOUT_ENDIF
#define SUPPRESS_GRAYOUT_ENDIF endif; endif;
#endif

//----------------------------------------------------------------------

#endif // #ifndef _SERVER_MGMT_SETUP_VARIABLE_H

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
