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
//<AMI_FHDR_START>
//
// Name:	CryptoPkgTest.h
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __CRYPTOPKG_TEST_H__
#define __CRYPTOPKG_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Cryptest.h"

#define AMI_CRYPTOPKG_TEST_PROTOCOL_REVISION_01        0x01
#define AMI_CRYPTOPKG_TEST_PROTOCOL_REVISION_02        0x02

#define AMI_CRYPTOPKG_TEST_GUID \
    {0x840b0034, 0x6f90, 0x4af5, 0x9a, 0xc4, 0xa1, 0x32, 0x7b, 0x7a, 0x66, 0x69}

EFI_GUID gAmiCryptoPkgTestProtocolGuid = AMI_CRYPTOPKG_TEST_GUID;

extern EFI_GUID gAmiCryptoPkgTestProtocolGuid;

#pragma pack(1)

typedef
EFI_STATUS
(EFIAPI *GET_OPENSSL_VER_STR)(
  IN OUT const char **Buffer
);

typedef
EFI_STATUS
(EFIAPI *GET_OPENSSL_VER_NUM)(
  IN OUT unsigned long *Buffer
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_DIGEST)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_HMAC)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_BLOCKCIPHER)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_RSA)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_RSA2)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_PKCS7)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_AUTHENTI_CODE)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_TSCOUNTER_SIGNATURE)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_DH)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_PRNG)(
  VOID
);

typedef
EFI_STATUS
(EFIAPI *VALIDATE_CRYPT_PKCS5)(
  VOID
);

typedef struct _AMI_CRYPTOPKG_TEST_PROTOCOL
{
  UINT32               	Revision;
  GET_OPENSSL_VER_STR  	GetOpenSslVersionString;
  GET_OPENSSL_VER_NUM  	GetOpenSslVersionNumber;
  VALIDATE_CRYPT_DIGEST	ValidateCryptDigest;
  VALIDATE_CRYPT_HMAC	ValidateCryptHmac;
  VALIDATE_CRYPT_BLOCKCIPHER ValidateCryptBlockCipher;
  VALIDATE_CRYPT_RSA	ValidateCryptRsa;
  VALIDATE_CRYPT_RSA2	ValidateCryptRsa2;
  VALIDATE_CRYPT_PKCS7	ValidateCryptPkcs7;
  VALIDATE_AUTHENTI_CODE  ValidateAuthenticode;
  VALIDATE_TSCOUNTER_SIGNATURE ValidateTscounterSignature;
  VALIDATE_CRYPT_DH	ValidateCryptDh;
  VALIDATE_CRYPT_PRNG	ValidateCryptPrng;
  VALIDATE_CRYPT_PKCS5  ValidateCryptPkcs5;
} AMI_CRYPTOPKG_TEST_PROTOCOL;

#pragma pack()

EFI_STATUS GetOpenSslVersionString(const char **Buffer);
EFI_STATUS GetOpenSslVersionNumber(unsigned long *Buffer);
EFI_STATUS ValidateCryptDigestFunc(VOID);
EFI_STATUS ValidateCryptHmacFunc(VOID);
EFI_STATUS ValidateCryptBlockCipherFunc(VOID);
EFI_STATUS ValidateCryptRsaFunc(VOID);
EFI_STATUS ValidateCryptRsa2Func(VOID);
EFI_STATUS ValidateCryptPkcs5Func(VOID);
EFI_STATUS ValidateCryptPkcs7Func(VOID);
EFI_STATUS ValidateAuthenticodeFunc(VOID);
EFI_STATUS ValidateTscounterSignatureFunc(VOID);
EFI_STATUS ValidateCryptDhFunc(VOID);
EFI_STATUS ValidateCryptPrngFunc(VOID);


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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
