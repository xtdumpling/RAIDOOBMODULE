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
  Image Authentication definitions header file
*/

#ifndef __AMI_IMAGE_AUTHTICATION_H__
#define __AMI_IMAGE_AUTHTICATION_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <WinCertificate.h>

// Include files from EDKII
// MdePkg:
#include <Guid/ImageAuthentication.h>

#define EFI_SECURE_BOOT_NAME              L"SecureBoot"

#define SECURE_BOOT                       1
#define NONSECURE_BOOT                    0

#define EFI_CERT_X509 EFI_CERT_X509_GUID


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
