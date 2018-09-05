//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
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
   CryptoPPI.h
   AMI Digital Signature PPI Protocol Definition
**/

#ifndef ___AMI_DIGITAL_SIGNATURE_PEI__H__
#define ___AMI_DIGITAL_SIGNATURE_PEI__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Library/CryptLib.h>

#define AMI_DIGITAL_SIGNATURE_PPI_GUID \
    { 0x86c29aa5, 0xdb0, 0x4343, 0xbd, 0x52, 0x7a, 0x72, 0x9f, 0x37, 0xc9, 0x6d }

typedef struct _AMI_CRYPT_DIGITAL_SIGNATURE_PPI AMI_CRYPT_DIGITAL_SIGNATURE_PPI;

typedef struct{
    EFI_GUID   AlgGuid;
    UINT32     BlobSize;
    UINT8      *Blob;
} PEI_CRYPT_HANDLE;

typedef 
EFI_STATUS
(EFIAPI *PEI_CRYPT_DS_VERIFY_SIG) (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN PEI_CRYPT_HANDLE        *PublicKey,
  IN PEI_CRYPT_HANDLE        *Hash,
  IN VOID                    *Signature,
  IN UINTN                    SignatureSize,
  IN UINT32                   Flags
  );

typedef 
EFI_STATUS
(EFIAPI *PEI_CRYPT_DS_VERIFY_PKCS7SIG) (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST UINT8 *P7Data,
  IN UINTN        P7Size,
  IN CONST UINT8 *TrustedCert,
  IN UINTN        CertSize,
  IN OUT UINT8  **Data,
  IN OUT UINTN   *DataSize
  );

typedef 
EFI_STATUS
(EFIAPI *PEI_CRYPT_DS_GET_KEY) (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST EFI_GUID          *KeyAlgorithm, // supported PKPUB_KEY_GUID only
  IN PEI_CRYPT_HANDLE        *PublicKey
  );

typedef 
EFI_STATUS
(EFIAPI *PEI_CRYPT_DS_VERIFY_KEY) (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST EFI_GUID          *KeyAlgorithm, // supported PKPUB_KEY_GUID only
  IN PEI_CRYPT_HANDLE        *PublicKey
  );

typedef
EFI_STATUS
(EFIAPI *PEI_CRYPT_DS_HASH)(
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST EFI_GUID          *HashAlgorithm,
  IN UINTN                    num_elem,
  IN CONST UINT8             *addr[],
  IN CONST UINTN             *len,
  OUT UINT8                  *Hash
  );    

struct _AMI_CRYPT_DIGITAL_SIGNATURE_PPI {
  PEI_CRYPT_DS_HASH           Hash;
  PEI_CRYPT_DS_VERIFY_KEY     VerifyKey; // compares Key with Platform Signing key(PKpub)
  PEI_CRYPT_DS_VERIFY_SIG     VerifySig;
  PEI_CRYPT_DS_GET_KEY        GetKey;  
  PEI_CRYPT_DS_VERIFY_PKCS7SIG   VerifyPkcs7Sig;
};

extern EFI_GUID gAmiDigitalSignaturePPIGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif // ___AMI_DIGITAL_SIGNATURE_PEI__H__
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
