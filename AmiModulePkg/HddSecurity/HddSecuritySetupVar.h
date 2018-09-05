//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file HddSecuritySetupVar.h
    HDD security setup configuration variable related header
**/

#ifndef _HDD_SECURITY_SETUP_VAR_H_
#define _HDD_SECURITY_SETUP_VAR_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>

#define HDD_SECURITY_CONFIG_GUID \
    { 0x3dd0de67, 0x2d7, 0x4129, 0x91, 0x4a, 0x9f, 0x37, 0x7c, 0xc3, 0x4b, 0xd}

typedef struct _HDD_SECURITY_CONFIG {
    UINT16      Count;
    UINT16      Supported[6];
    UINT16      Enabled[6];
    UINT16      Locked[6];
    UINT16      Frozen[6]; 
    UINT16      UserPasswordStatus[6]; 
    UINT16      MasterPasswordStatus[6]; 
    UINT16      ShowMaster[6];
    UINT16      HddUserPassword[IDE_PASSWORD_LENGTH];
    UINT16      HddMasterPassword[IDE_PASSWORD_LENGTH];
    UINT16      HddUserPassword_HDD2[IDE_PASSWORD_LENGTH];
    UINT16      HddMasterPassword_HDD2[IDE_PASSWORD_LENGTH];
    UINT16      HddUserPassword_HDD3[IDE_PASSWORD_LENGTH];
    UINT16      HddMasterPassword_HDD3[IDE_PASSWORD_LENGTH];
    UINT16      HddUserPassword_HDD4[IDE_PASSWORD_LENGTH];
    UINT16      HddMasterPassword_HDD4[IDE_PASSWORD_LENGTH];
    UINT16      HddUserPassword_HDD5[IDE_PASSWORD_LENGTH];
    UINT16      HddMasterPassword_HDD5[IDE_PASSWORD_LENGTH];
    UINT16      HddUserPassword_HDD6[IDE_PASSWORD_LENGTH];
    UINT16      HddMasterPassword_HDD6[IDE_PASSWORD_LENGTH];
}HDD_SECURITY_CONFIG;

#ifdef __cplusplus
}
#endif
#endif 
//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
