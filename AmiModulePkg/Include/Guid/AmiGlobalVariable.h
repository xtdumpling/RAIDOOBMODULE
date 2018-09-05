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

/** @file
  @brief AMI Global Variable GUID Definition.

  The file contains defintion of the variable GUID used by Aptio specific UEFI variables.
**/

#ifndef __AMI_GLOBAL_VARIABLE_GUID__H__
#define __AMI_GLOBAL_VARIABLE_GUID__H__

/// This GUID is used by Aptio specific UEFI variables.
// The GUID is commented out to avoid macro redefinition conflict with AmiCompatibilityPkg
/*
#define AMI_GLOBAL_VARIABLE_GUID \
    { 0x1368881, 0xc4ad, 0x4b1d, { 0xb6, 0x31, 0xd5, 0x7a, 0x8e, 0xc8, 0xdb, 0x6b } }
*/

/// see ::AMI_GLOBAL_VARIABLE_GUID
extern EFI_GUID gAmiGlobalVariableGuid;

#endif
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
