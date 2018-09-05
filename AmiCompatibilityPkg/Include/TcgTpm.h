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
  TCG definitions
*/

#ifndef __TCG__H__
#define __TCG__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

// Include from MdePkg
#include <IndustryStandard/Tpm12.h>
#include <IndustryStandard/UefiTcgPlatform.h>

typedef UINT32 TPM_NV_PER_ATTRIBUTES;
#define TPM_FAMFLAG_DELEGATE_ADMIN_LOCK   (((UINT32)1)<<1)

typedef UINT32 TPM_DELEGATE_TYPE;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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
