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
//**********************************************************************
/** @file
 * CryptoPei.c
 */
#include <Token.h>
#include <AmiPeiLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include "Ppi/LoadFile.h"
#include <Protocol/Hash.h>
#include <PPI/CryptoPPI.h>
#include "AmiCertificate.h"
#include <Protocol/AmiDigitalSignature.h> // EFI_CERT_X509_SHAXXX

//
// Global variables
//
// Although ShaXXXGuid global variables are defined in EDK's EdkProtocol Lib, but linking it adds additional 20k in debug mode.
//static EFI_GUID gEfiHashAlgorithmSha1Guid   = EFI_HASH_ALGORITHM_SHA1_GUID;
//static EFI_GUID gEfiHashAlgorithmSha256Guid = EFI_HASH_ALGORITHM_SHA256_GUID;
//static EFI_GUID gEfiHashAlgorithmSha384Guid = EFI_HASH_ALGORITHM_SHA384_GUID;
//static EFI_GUID gEfiHashAlgorithmSha512Guid = EFI_HASH_ALGORITHM_SHA512_GUID;
//static EFI_GUID gPRKeyGuid                  = PR_KEY_GUID;

// Hardwired at Build time. Supported formats: RSA2048, HASH256 Key Certs

static EFI_GUID *gKeyFileGuid [] = {
    &gPKeyFileSha256Guid,
    &gPKeyFileX509Guid,
    &gPKeyFileRsa2048Guid,
    NULL
};
static EFI_GUID *gKeyTypeGuid [] = {
    &gEfiCertSha256Guid,
    &gEfiCertX509Guid,
    &gEfiCertRsa2048Guid,
    NULL
};

FW_KEY_HOB         *pFwKeyHob = NULL;
//
// SDL defined Public Exponent E of RSA Key.
//
const UINT8  KeyE[] = {E_CONST}; // 0x10001
const UINTN  LenE = sizeof(KeyE);
//    PKCS_1 PSS Signature constant. Size of the Salt (random data) field in PSS signature.
const INT32  saltlen = PSS_SIG_SALTLEN; // 8 
static UINT8 DecriptedSig[DEFAULT_RSA_SIG_LEN];
//----------------------------------------------------------------------------
// Crypto Function prototypes
//----------------------------------------------------------------------------
EFI_STATUS
EFIAPI
PeiHash (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI      *This,
  IN CONST EFI_GUID               *HashAlgorithm,
  IN UINTN                        num_elem,
  IN CONST UINT8                  *addr[],
  IN CONST UINTN                  *len,
  OUT UINT8                       *Hash
  );

EFI_STATUS
EFIAPI
PeiPkcs7Verify (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST UINT8 *P7Data,
  IN UINTN        P7Size,
  IN CONST UINT8 *TrustedCert,
  IN UINTN        CertSize,
  IN OUT UINT8  **Data,
  IN OUT UINTN   *DataSize
  );
  
EFI_STATUS
EFIAPI
PeiVerifySig 
(
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN PEI_CRYPT_HANDLE          *PublicKey,
  IN PEI_CRYPT_HANDLE          *Hash,
  IN VOID                      *Signature,
  IN UINTN                     SignatureSize,
  IN UINT32                    Flags
);

EFI_STATUS
EFIAPI
PeiGetKey 
(
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST EFI_GUID            *KeyAlgorithm, // reserved to PKPUB_KEY_GUID
  IN PEI_CRYPT_HANDLE          *PublicKey
  );
  
EFI_STATUS
EFIAPI
PeiVerifyKey 
(
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST EFI_GUID            *KeyAlgorithm, // reserved to PKPUB_KEY_GUID
  IN PEI_CRYPT_HANDLE          *PublicKey
  );

//----------------------------------------------------------------------------
// Crypto Protocol Identifiers
//----------------------------------------------------------------------------
AMI_CRYPT_DIGITAL_SIGNATURE_PPI  mSigPeiInitPpi = {
  PeiHash,
  PeiVerifyKey,
  PeiVerifySig,
  PeiGetKey,
  PeiPkcs7Verify
};

//static 
EFI_PEI_PPI_DESCRIPTOR mPpiSigListVariable = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmiDigitalSignaturePPIGuid,
  &mSigPeiInitPpi
};

/**
  Loads binary from RAW section of X firmware volume

  @param[out] Buffer - returns a pointer to allocated memory. Caller must free it when done
  @param[out] Size  - returns the size of the binary loaded into the buffer.
  
  @retval     Status
**/
EFI_STATUS
CryptoGetRawImage (IN EFI_GUID *FileGuid, IN OUT VOID **Buffer, IN OUT UINTN *Size)
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME_HEADER    *pFV;
  UINTN                         FvNum=0;
  EFI_FFS_FILE_HEADER           *ppFile=NULL;
//  EFI_FV_FILE_INFO            FileInfo;  
  static EFI_GUID               pFV_BB_Guid = FWKEY_FV_HEADER_GUID;
  BOOLEAN    bFound = FALSE;

  if (!Buffer || !Size) 
    return EFI_INVALID_PARAMETER;

  while(TRUE)
  {
      Status = PeiServicesFfsFindNextVolume(FvNum++, &pFV);
      if(EFI_ERROR(Status)) return Status;
//      DEBUG ((AmiCryptoPkg_DEBUG_LEVEL, "FvNum %d : %g\n", FvNum-1, (EFI_GUID*)((UINTN)pFV+pFV->ExtHeaderOffset)));
      //
      // FwKey location is in FV_BB only
      //
      if(guidcmp((EFI_GUID*)((UINTN)pFV+pFV->ExtHeaderOffset), &pFV_BB_Guid) != 0)
        continue;

      ppFile = NULL;

      while(TRUE)
      {
          Status = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_ALL, pFV, &ppFile);
          if(Status == EFI_NOT_FOUND)
          {
              break;
          }
          if(guidcmp(&ppFile->Name, FileGuid)==0) 
          {
              bFound = TRUE; 
              break;
          }
      }
      if(bFound) break;
  }
    // hopefully we found the file...now try to read raw data
    // !!! Keys are uncompressed. There is no much reason to run compression on prime numbers anyway
  Status = PeiServicesFfsFindSectionData (EFI_SECTION_RAW, ppFile, Buffer);
  if(!EFI_ERROR(Status)) {
    // Size may need to subtract Section hdr size = 28 bytes sizeof(EFI_FFS_FILE_HEADER + EFI_COMMON_SECTION_HEADER)
    *Size = FVFILE_SIZE(ppFile)-sizeof(EFI_FFS_FILE_HEADER)-sizeof(EFI_COMMON_SECTION_HEADER);
  }
/*  Status = PeiServicesFfsGetFileInfo(ppFile, &FileInfo);
    if(!EFI_ERROR(Status) ) {
      // Size may need to subtract Section hdr size = 28 bytes sizeof(EFI_FFS_FILE_HEADER + EFI_COMMON_SECTION_HEADER)
        *Buffer = (UINT8*)FileInfo.Buffer + sizeof(EFI_COMMON_SECTION_HEADER);
        *Size = FileInfo.BufferSize-sizeof(EFI_COMMON_SECTION_HEADER);
    }
*/
  DEBUG ((AmiCryptoPkg_DEBUG_LEVEL, "Find Key Ffs %r addr=%X (%X,%X), size=%db\n", Status, (UINT32)*Buffer, ((UINT8*)*Buffer)[0], ((UINT8*)*Buffer)[1], *Size));

  return Status;  

}

/**
  Allows creating a hash of an arbitrary message digest using one or more hash algorithms

  @param[in] This           Pointer to the AMI_CRYPT_DIGITAL_SIGNATURE_PPI instance.
  @param[in]  HashAlgorithm Points to the EFI_GUID which identifies the algorithm to use.
  @param[in]  num_elem      Number of blocks to be passed via next argument:addr[]
  @param[in]  addr[]        Pointer to array of UINT8* addresses of data blocks to be hashed
  @param[in]  len           Pointer to array of integers containing length of each block listed by addr[]
  @param[in]  Hash          Holds the resulting hash computed from the message.
  
  @retval     Status
        EFI_SUCCESS           Hash returned successfully.
        EFI_INVALID_PARAMETER Message or Hash is NULL
        EFI_UNSUPPORTED       The algorithm specified by HashAlgorithm is not supported by this driver.
**/
EFI_STATUS
EFIAPI
PeiHash (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI      *This,
  IN CONST EFI_GUID               *HashAlgorithm,
  IN UINTN                        num_elem,
  IN CONST UINT8                  *addr[],
  IN CONST UINTN                  *len,
  OUT UINT8                       *Hash
  )
{
    HASH_ALG    HashAlgo;
    UINTN       HashLen;

// Support SHA hashes
    if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha1Guid)) 
    {
        HashAlgo= SHA1;
        HashLen = SHA1_DIGEST_SIZE;
    }
    else if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha256Guid))
         {
             HashAlgo= SHA256;
             HashLen = SHA256_DIGEST_SIZE;
         }
        else if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha384Guid))
             {
                 HashAlgo=SHA384;
                 HashLen = SHA384_DIGEST_SIZE;
             }
                else if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha512Guid))
                     {
                         HashAlgo=SHA512;
                         HashLen = SHA512_DIGEST_SIZE;
                     }
                    else
                        return EFI_UNSUPPORTED;

    MemSet(Hash, HashLen, 0);
    if(HashAlgo==SHA1)
        sha1_vector(num_elem, (const UINT8 **)addr, (const size_t *)len, Hash);
    else if(HashAlgo==SHA256)
            sha256_vector(num_elem, (const UINT8 **)addr, (const size_t *)len, Hash);
        else if(HashAlgo==SHA384)
                sha384_vector(num_elem, (const UINT8 **)addr, (const size_t *)len, Hash);
            else if(HashAlgo==SHA512)
                    sha512_vector(num_elem, (const UINT8 **)addr, (const size_t *)len, Hash);

    return  EFI_SUCCESS;
}

/**
   Function verifies that the specified signature matches the specified hash. 
                 Verifies the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in
                 RSA PKCS#1.
                 This function decrypts the signature with the provided key and then compares 
                 the decrypted value to the specified hash value

  @param[in]  This          Pointer to the AMI_CRYPT_DIGITAL_SIGNATURE_PPI instance.
  @param[in]  PublicKey     Handle to a key used for verifying signatures. This handle must be identifying a public key.
  @param[in]  Hash          Handle of the hash object to verify.
  @param[in]  Signature     Pointer to the signature data to be verified.
  @param[in]  SignatureSize The size, in bytes, of the signature data.
  @param[in]  Flags        Specifies additional flags to further customize the signing/verifying behavior.

  @retval     Status
    EFI_SUCCESS               The signature is successfully verified.
    EFI_SECURITY_VIOLATION    The signature does not match the given message.
    EFI_ACCESS_DENIED         The key could not be used in signature operation.
    EFI_INVALID_PARAMETER     The size of input message or signature does not meet the criteria 
                              of the underlying signature algorithm.
**/
EFI_STATUS
EFIAPI
PeiVerifySig
(
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI      *This,
  IN PEI_CRYPT_HANDLE            *PublicKey,
  IN PEI_CRYPT_HANDLE            *Hash,
  IN VOID                        *Signature,
  IN UINTN                       SignatureSize,
  IN UINT32                      Flags
  )
{
    EFI_STATUS      Status;
    INTN            err;
    struct          crypto_rsa_key *key = NULL;
    size_t         *sig_len=(size_t*)&SignatureSize;
    INT32           modulus_bitlen = DEFAULT_RSA_SIG_LEN << 3;
    UINT32          HashLen;

// Only supporting RSASSA_PKCS1V15 signature types
    if(!((Flags & EFI_CRYPT_RSASSA_PKCS1V15) || 
         (Flags & EFI_CRYPT_RSASSA_PSS)) )
            return EFI_INVALID_PARAMETER;

    ResetCRmm();

// check Key handle if requested PubKey is a Platform FW Key
// In this case use the FW key from ffs image
// validity of the Key must be verified prior to calling VerifySig
// Guid must match one of the valid keys we can use in PEI Sig verification.
// For now Public Key is supported in 2 formats: RAW 256 bytes and ASN.1 Integer
// shall be extended to gPBkey_x509_Guid
    if(!guidcmp(&PublicKey->AlgGuid, &gEfiCertRsa2048Guid))
        key = crypto_import_rsa2048_public_key(PublicKey->Blob, PublicKey->BlobSize, (UINT8*)&KeyE, LenE);
    else
        return EFI_INVALID_PARAMETER;

    if(!guidcmp(&Hash->AlgGuid, &gEfiHashAlgorithmSha256Guid))
        HashLen = SHA256_DIGEST_SIZE;
    else if(!guidcmp(&Hash->AlgGuid, &gEfiHashAlgorithmSha1Guid))
            HashLen = SHA1_DIGEST_SIZE;
        else
            HashLen = SHA256_DIGEST_SIZE;

    if(key == NULL )
        err = -1;
    else
        err = crypto_rsa_exptmod((const UINT8*)Signature, (size_t)SignatureSize, (UINT8*)&DecriptedSig, sig_len, key, 0);
// locate Hash inside the decrypted signature body and compare it with given Hash;
// Should be extended to handle sha1, sha256 hashes. use Hash->AlgGuid to determine the Hash type
    if(!err) 
    {
        if(Flags & EFI_CRYPT_RSASSA_PKCS1V15)
        {
            // Validate PKCS#1v1.5 Padding
//            err = pkcs_1_v1_5_decode((const UINT8 *)Hash->Blob, HashLen, (const UINT8 *)&DecriptedSig, (unsigned long)*sig_len);
// just compare the hash at the end of the sig blob
            err = MemCmp(Hash->Blob, (void*)((UINTN)DecriptedSig + (UINTN)(*sig_len - HashLen)), HashLen);
        } else //(Flags & EFI_CRYPT_RSASSA_PSS))
            // Validate PKCS#1 PSS Signature: padding & hash
            err = pkcs_1_pss_decode(
                    Hash->Blob, HashLen, 
                    (const unsigned char *)&DecriptedSig, (unsigned long)*sig_len, 
                    saltlen,  modulus_bitlen);
    }

    Status = !err ? EFI_SUCCESS:EFI_SECURITY_VIOLATION;

// Security concern, memory heap is being cleared on exit 
    ResetCRmm();

    return Status;
}

/**
 Verifies the validity of a PKCS#7 signed data as described in "PKCS #7: Cryptographic Message Syntax Standard".
 Function verifies validity of the signature contained inside the Certificate
 This function decrypts the signature with the Public key from the Signer certificate
 and then compares the decrypted value to the input Data

  @param[in]  This         Pointer to the AMI_CRYPT_DIGITAL_SIGNATURE_PPI instance.
  @param[in]  P7Data       Pointer to the PKCS#7 DER encoded message to verify.
  @param[in]  P7Size       Size of the PKCS#7 message in bytes.
  @param[in]  TrustedCert  Pointer to a trusted/root X509 certificate encoded in DER, which
                           is used for certificate chain verification.
  @param[in]  CertSize     Size of the trusted certificate in bytes.
  @param[in]  Data         Pointer to the content to be verified/returned at
  @param[in]  DataSize     Size of Data in bytes

  @retval     Status
    EFI_SUCCESS              The specified PKCS#7 signed data is valid
    EFI_SECURITY_VIOLATION   Invalid PKCS#7 signed data.
    EFI_ACCESS_DENIED        The Trusted certificate does not have a match in SignedData.certificate store.
    EFI_INVALID_PARAMETER    The size of input message or signature does not meet the criteria 
                             of the underlying signature algorithm.
**/
EFI_STATUS
EFIAPI
PeiPkcs7Verify (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI *This,
  IN CONST UINT8 *P7Data,
  IN UINTN        P7Size,
  IN CONST UINT8 *TrustedCert,
  IN UINTN        CertSize,
  IN OUT UINT8  **Data,
  IN OUT UINTN   *DataSize
  )
{
#if CONFIG_PEI_PKCS7 == 1

    EFI_STATUS  Status;
    INTN        err, reason;
    struct pkcs7_signed_data_st* PKCS7cert;
    struct x509_certificate *x509TrustCert;
    struct x509_certificate *x509SignCert;
    struct pkcs7_cert_revoke_info revokeInfo;

    err     = -1;
    reason  = 0;
    x509SignCert = NULL;

    if((*Data == NULL || DataSize == NULL) ||
       (!P7Data || P7Size== 0) || 
       (!TrustedCert || CertSize== 0)
    ) 
        return EFI_INVALID_PARAMETER;

    ResetCRmm();

    PKCS7cert = Pkcs7_parse_Authenticode_certificate(P7Data, P7Size);
    if (PKCS7cert) {
        // verify Pkcs7 Signing Cert chain up to the TrustCert...
        x509TrustCert = x509_certificate_parse(TrustedCert, CertSize);
        if(x509TrustCert) {
            err = Pkcs7_x509_certificate_chain_validate(PKCS7cert, x509TrustCert, (int*)&reason);
        } else {
        // potentially a TimeStamped Hash of x509 tbs certificate data
            if(CertSize == sizeof(EFI_CERT_X509_SHA256)) {
                revokeInfo.ToBeSignedHashLen = 32;
                revokeInfo.ToBeSignedHash = TrustedCert;
                err = Pkcs7_x509_certificate_chain_validate_with_timestamp(PKCS7cert, FALSE, NULL, &revokeInfo, (int*)&reason);
            }
            else // unsupported Hash struct
                err = -1;
        }
        if(!err)
            err = Pkcs7_certificate_validate_digest(PKCS7cert, x509SignCert, *Data, *DataSize);
    } 

    // Security concern, memory heap is being cleared on exit 
    ResetCRmm();

    // failed to process
    Status = !err ? EFI_SUCCESS:EFI_SECURITY_VIOLATION;

    return Status;
#else
    return EFI_UNSUPPORTED;
#endif    
}

/**
    Function returns Ptr to a Platform Signing Key (PK) Ffs inside Recovery FV (FV_BB or similar)

  @param[in]   This         Pointer to the AMI_CRYPT_DIGITAL_SIGNATURE_PPI instance.
  @param[in]   KeyAlgorithm Points to the EFI_GUID which identifies the PKpub algorithm to use.
  @param[out]  PublicKey    Handle to a key used to return a ptr to a Key. This handle must be identifying a public key.

  @retval     Status
    EFI_SUCCESS               The Key is successfully returned.
    EFI_NOT_FOUND             The Key not found
    EFI_ACCESS_DENIED         The key could not be used in signature operation.
    EFI_INVALID_PARAMETER     The size of input message or signature does not meet the criteria 
                              of the underlying signature algorithm.
**/
EFI_STATUS
EFIAPI
PeiGetKey (
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI      *This,
  IN CONST EFI_GUID          *KeyAlgorithm, // reserved to PKPUB_KEY_GUID
  IN OUT PEI_CRYPT_HANDLE        *PublicKey
  )
{
    if(!PublicKey || !KeyAlgorithm)
        return EFI_INVALID_PARAMETER;

    // now only supporting PKpub key comparison
    if(guidcmp((EFI_GUID*)KeyAlgorithm, &gPRKeyGuid))
        return EFI_INVALID_PARAMETER;

    if(pFwKeyHob) {
        PublicKey->AlgGuid = pFwKeyHob->KeyGuid;
        PublicKey->Blob = (UINT8*)(UINTN)(pFwKeyHob->KeyAddress);
        PublicKey->BlobSize = pFwKeyHob->KeySize;
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

/**
  Function compares the input PublicKey against Platform Signing Key (PK) image in the flash.

  @param[in]   This         Pointer to the AMI_CRYPT_DIGITAL_SIGNATURE_PPI instance.
  @param[in]   KeyAlgorithm    Points to the EFI_GUID which identifies the PKpub algorithm to use.
  @param[in]   PublicKey       Handle to a key used for verifying signatures.  This handle must be identifying a public key.

  @retval     Status
    EFI_SUCCESS               The Key is successfully verified.
    EFI_SECURITY_VIOLATION    The Key does not match current FW key.
    EFI_ACCESS_DENIED         The key could not be used in signature operation.
    EFI_INVALID_PARAMETER     The size of input message or signature does not meet the criteria 
                              of the underlying signature algorithm.
**/
EFI_STATUS
EFIAPI
PeiVerifyKey 
(
  IN CONST AMI_CRYPT_DIGITAL_SIGNATURE_PPI      *This,
  IN CONST EFI_GUID          *KeyAlgorithm, // reserved to PKPUB_KEY_GUID
  IN PEI_CRYPT_HANDLE        *PublicKey
  )
{
    EFI_STATUS    Status;
    int           err;
    PEI_CRYPT_HANDLE KeyHndl;
    UINT8         Hash[SHA256_DIGEST_SIZE] = {0};
    UINT8         *KeyBuf, *PKpubBuffer=NULL;
    size_t        KeyLen;

    if(!PublicKey || !KeyAlgorithm || !PublicKey->Blob)
        return EFI_INVALID_PARAMETER;

// check Key handle if requested PubKey is a Platform Key PKpub.
// In this case use PKpub key from ffs image
    if(guidcmp((EFI_GUID*)KeyAlgorithm, &gPRKeyGuid))
        return EFI_INVALID_PARAMETER;
    
    Status = PeiGetKey(This, KeyAlgorithm, &KeyHndl);
    if(!EFI_ERROR(Status)) {
//
//==========================    
// FWKey : hash 
//    pubkey : Rsa2048 - hash it
//    pubkey : x509 - N/A 
//
// FWKey : x509
//    pubkey : Rsa2048 (Pkcs1) - xtract x509 pub key from FWkey
// 
// FWKey : Rsa2048
//    pubkey : x509 (Pkcs7) - xtract nModulus from x509 pubkey
//==========================    
//
        PKpubBuffer = KeyHndl.Blob;
        KeyLen = (size_t)KeyHndl.BlobSize; // always 256
        KeyBuf = PublicKey->Blob;
        err = TRUE;
//  FWKey : hash
        if(!guidcmp(&KeyHndl.AlgGuid, &gEfiCertSha256Guid) ){
            KeyBuf = Hash;
            //    pubkey : Rsa2048 - hash it
            if(!guidcmp(&PublicKey->AlgGuid, &gEfiCertRsa2048Guid) ){
            // SHA256 Hash of RSA Key
                KeyLen = (size_t)PublicKey->BlobSize;
                sha256_vector(1, (const UINT8**)&PublicKey->Blob, (const size_t*)&KeyLen, KeyBuf);
            } 
            KeyLen = SHA256_DIGEST_SIZE;
        } else 
// FWKey : x509        
        if(!guidcmp(&KeyHndl.AlgGuid, &gEfiCertX509Guid) ){
        //    pubkey : Rsa2048 (Pkcs1) - xtract x509 pub key
            if(!guidcmp(&PublicKey->AlgGuid, &gEfiCertRsa2048Guid) ){
#if CONFIG_PEI_PKCS7 == 1
                KeyLen = DEFAULT_RSA_KEY_MODULUS_LEN;
                KeyBuf = PublicKey->Blob;
                ResetCRmm();
                err = Pkcs7_x509_return_Cert_pubKey((UINT8*)KeyHndl.Blob, KeyHndl.BlobSize, &PKpubBuffer, &KeyLen);
                ResetCRmm();
#endif                
                if(err) return EFI_SECURITY_VIOLATION;
            }
        } else
// FWKey : Rsa2048            
        if(!guidcmp(&KeyHndl.AlgGuid, &gEfiCertRsa2048Guid) ){
        //    pubkey : x509 (Pkcs7) - xtract nModulus    
            if(!guidcmp(&PublicKey->AlgGuid, &gEfiCertX509Guid) ){
#if CONFIG_PEI_PKCS7 == 1
                PKpubBuffer = &Hash[0];
                KeyLen = DEFAULT_RSA_KEY_MODULUS_LEN;
                KeyBuf = PublicKey->Blob;
                ResetCRmm();
                err = Pkcs7_x509_return_Cert_pubKey((UINT8*)PublicKey->Blob, PublicKey->BlobSize, &PKpubBuffer, &KeyLen);
                ResetCRmm();
#endif                
                if(err) return EFI_SECURITY_VIOLATION;
            }
        }

        err = MemCmp(PKpubBuffer, KeyBuf, KeyLen);
        Status = !err ? EFI_SUCCESS:EFI_SECURITY_VIOLATION;

    }

    return Status;
}

/**
  This function is the entry point for this PEI.

  @param[in]   FileHandle  Pointer to the file handle
  @param[in]   PeiServices Pointer to the PEI services table

  @retval     Status
**/
EFI_STATUS
EFIAPI
CryptoPei_Init (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS              Status;
    UINTN                   Size;
    EFI_BOOT_MODE           BootMode;
    UINT8                   Index;
    UINT8                   *pBuf;
//    UINTN                   i;
//    UINT8                   Byte;
//    const UINT8 *FwBadKey = "$BAD";

    // WARNING. This module must always be launched from RAM after the boot mode type is finalized.
    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
    if(EFI_ERROR(Status) || BootMode == BOOT_ON_S3_RESUME) {
        return Status; // skip Crypto PPI install on S3 resume
    }

////////////////////////////////////////////////////////////////////////////////////
//
// Create FwKey Hob
//
////////////////////////////////////////////////////////////////////////////////////
    Index = 0;
    while(gKeyFileGuid[Index] != NULL)
    {
// Available Key Cert GUIDs: RSA2048, SHA256 and x509
        Status = CryptoGetRawImage(gKeyFileGuid[Index], (VOID**)&pBuf, (UINTN*)&Size);
        if(!EFI_ERROR(Status)) {
/*
            //  make sure the Key buffer is mapped to FV_BB address space 
            if(!((UINT32)pBuf > FV_BB_BASE && 
                 (UINT64)((UINT32)pBuf+Size) < 
                 (UINT64)(FV_BB_BASE+(UINT64)FV_BB_BLOCKS*FLASH_BLOCK_SIZE)))
                break;
*/
/*
            // don't report a Key if dummy FWkey  is found
            if(*((UINT32*)(pBuf)) == *(UINT32*)FwBadKey) {
                DEBUG ((AmiCryptoPkg_DEBUG_LEVEL, "FFS with dummy FW Key detected...\n"));
                break;
            }
*/            
            Status = (*PeiServices)->CreateHob( PeiServices,
                EFI_HOB_TYPE_GUID_EXTENSION, 
                sizeof(FW_KEY_HOB), 
                (void**)&pFwKeyHob);

            if (!EFI_ERROR(Status) && pFwKeyHob) {
                pFwKeyHob->Header.Name = gPRKeyGuid;
                pFwKeyHob->KeyGuid =  *gKeyTypeGuid[Index];
                pFwKeyHob->KeyAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)pBuf;
                pFwKeyHob->KeySize = Size;

            }
            break;
        }
        Index++;
    }
////////////////////////////////////////////////////////////////////////////////////
//
// Install VerifySig PPI
//
////////////////////////////////////////////////////////////////////////////////////
    return PeiServicesInstallPpi (&mPpiSigListVariable);
}

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
