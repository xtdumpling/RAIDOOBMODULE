//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
    AuthVariable.h: Implement authentication services for the authenticated variable
    service in UEFI2.2+
**/

#ifndef _AUTHVARIABLE_H_
#define _AUTHVARIABLE_H_

#include "AuthServiceInternal.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Variable Auth Hdr EFI_VARIABLE_AUTHENTICATION
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
EFI_STATUS VerifyVariable1 (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32    Attributes,
    IN VOID    **Data,
    IN UINTN    *DataSize, 
    IN VOID     *OldData,
    IN UINTN     OldDataSize,
    IN OUT EXT_SEC_FLAGS *ExtFlags
    );

EFI_STATUS VerifyDataPayload (
    IN VOID     *Data,
    IN UINTN    DataSize, 
    IN UINT8    *PubKey
    );

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Variable Auth Hdr EFI_VARIABLE_AUTHENTICATION_2
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
EFI_STATUS VerifyVariable2 (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32    Attributes,
    IN VOID    **Data,
    IN UINTN    *DataSize, 
    IN VOID     *OldData,
    IN UINTN     OldDataSize,
    IN OUT EXT_SEC_FLAGS *ExtFlags
    );

EFI_STATUS ValidateSelfSigned (
    IN UINT8     *Pkcs7Cert,
    IN UINTN      Pkcs7Cert_len,
    IN OUT UINT8 **pDigest,
    IN OUT UINTN  *Digest_len,
    IN UINT8       Operation
    );

EFI_STATUS ConstructDataParameter (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32    Attributes,
    IN VOID     *Data,
    IN UINTN     DataSize, 
    OUT UINT8   *pDigest,
    OUT UINTN   *Digest_len,
    IN  UINT8    Mutex    
    );

EFI_STATUS ProcessVarWithPk2 (
    IN  UINT8     *Pkcs7Cert,
    IN  UINTN      Pkcs7Cert_len,
    IN  UINT8     *pDigest,
    IN  UINTN      Digest_len
    );

 EFI_STATUS ProcessVarWithKekDbr2 (
    IN  CHAR16    *VariableName,
    IN  EFI_GUID  *VendorGuid,
    IN  UINT8     *Pkcs7Cert,
    IN  UINTN      Pkcs7Cert_len,
    IN  UINT8     *pDigest,
    IN  UINTN      Digest_len
    );

#endif  // _AUTHVARIABLE_H_
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
