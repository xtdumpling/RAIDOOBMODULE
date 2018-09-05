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
//**********************************************************************
/** @file
 * CryptoDxe.c
 */
#include <Token.h>
#include <AmiDxeLib.h>
#include "Setup.h"
#include "AmiCertificate.h"
#include <Protocol/AmiDigitalSignature.h>
#include <Protocol/SmmBase.h> // used for SMM Malloc
#include <Library/MemoryAllocationLib.h>

//
// Global variables
//

// The ptr to a buffer containing the Fw Platform Key
// Key Image is copied from .ffs to a local buffer. 
// It is a safe location in case of AmiSig running in SMM
static CRYPT_HANDLE gKey = {{0},0,NULL};

// Second part of the Public key, public exponent (e), 
// SDL Hardwired value of publicExponent--e(recommended value of e=65537).
//
// SDL Hardwired value of publicExponent--e
const UINT8  KeyE[] = {E_CONST}; // 0x10001
const UINTN LenE = sizeof(KeyE);
const INT32  saltlen = PSS_SIG_SALTLEN; // 8
static UINT8 Rsa2048Sig[DEFAULT_RSA_SIG_LEN];

// Mutex to control Memory reset during atomic Pkcs7 operations
static UINT8  MutexLock = RESET;

//----------------------------------------------------------------------------
// Crypto Function prototypes
//----------------------------------------------------------------------------
EFI_STATUS
EFIAPI
DxePkcs1Verify (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL *This,
  IN CRYPT_HANDLE *PublicKey,
  IN CRYPT_HANDLE *Hash,
  IN VOID         *Signature,
  IN UINTN        SignatureSize,
  IN UINT32       Flags
  );

EFI_STATUS
EFIAPI
DxePkcs7Verify (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL *This,
  IN CONST UINT8 *P7Data,
  IN UINTN        P7Size,
  IN CONST UINT8 *TrustedCert,
  IN UINTN        CertSize,
  IN OUT UINT8  **Data,
  IN OUT UINTN   *DataSize,
  IN UINT8        Operation,
  IN UINT32       Flags
  );

EFI_STATUS
EFIAPI
DxeHash(
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL  *This,
  IN CONST EFI_GUID   *HashAlgorithm,
  IN UINTN            Num_elem,
  IN CONST UINT8      *Addr[],
  IN CONST UINTN      *Len,
  OUT UINT8           *Hash
  );

EFI_STATUS
EFIAPI
DxeGetKey (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL  *This,
  OUT CRYPT_HANDLE  *Key,
  IN  EFI_GUID      *AlgId,
  IN  UINTN          KeyLen,
  IN  UINT32         Flags
  );

EFI_STATUS
EFIAPI
DxeVerifyKey (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL *This,
  IN EFI_GUID       *AlgId,
  IN CRYPT_HANDLE   *Key
  );
//----------------------------------------------------------------------------
// Crypto Protocol Identifiers
//----------------------------------------------------------------------------
AMI_DIGITAL_SIGNATURE_PROTOCOL  mAmiSig = {
  DxePkcs1Verify,
  DxePkcs7Verify,
  DxeHash,
  DxeGetKey,
  DxeVerifyKey
};

//----------------------------------------------------------------------------
// Crypto Function Implementation
//----------------------------------------------------------------------------

/**
  Allows creating a hash of an arbitrary message digest using one or more hash algorithms

  @param[in] This          Pointer to the AMI_DIGITAL_SIGNATURE_PROTOCOL instance.
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
DxeHash(
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL  *This,
  IN CONST EFI_GUID               *HashAlgorithm,
  IN UINTN                        num_elem,
  IN CONST UINT8                  *addr[],
  IN CONST UINTN                  *len,
  OUT UINT8                       *Hash
  )
{
    HASH_ALG    HashAlgo;
    UINTN       HashLen;

// Support only SHA1 & SHA256 hashes
    if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha1Guid) ||
       !guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha1NoPadGuid)) 
    {
        HashAlgo=SHA1;
        HashLen = SHA1_DIGEST_SIZE;
    }
    else if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha256Guid) ||
            !guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha256NoPadGuid))
         {
             HashAlgo=SHA256;
             HashLen = SHA256_DIGEST_SIZE;
         }
        else if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha384Guid)/* ||
                !guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha384NoPadGuid)*/)
             {
                 HashAlgo=SHA384;
                 HashLen = SHA384_DIGEST_SIZE;
             }
            else if(!guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha512Guid)/* ||
                    !guidcmp((EFI_GUID*)HashAlgorithm, &gEfiHashAlgorithmSha512NoPadGuid)*/)
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
  Verifies the validity of a PKCS#7 signed data as described in "PKCS #7: Cryptographic
                 Message Syntax Standard".
                 Function performs several operations on input Pkcs7 certificate 
                 based on the input Operation argument
                 1. Verifies validity of the signature contained inside the Certificate
                    This function decrypts the signature with the Public key from the Signer certificate 
                    and then compares the decrypted value to the input Data
                 2. Extracts the Signer certificate in format of x509
                 3. Extracts Root CA certificate that used to sign the Signer certificate.

  @param[in]  This         Pointer to the AMI_DIGITAL_SIGNATURE_PROTOCOL instance.
  @param[in]  P7Data       Pointer to the PKCS#7 DER encoded message to verify.
  @param[in]  P7Size       Size of the PKCS#7 message in bytes.
  @param[in]  TrustedCert  Pointer to a trusted/root X509 certificate encoded in DER, which
                           is used for certificate chain verification.
  @param[in]  CertSize     Size of the trusted certificate in bytes.
  @param[in]  Data         Pointer to the content to be verified/returned at
  @param[in]  DataSize     Size of Data in bytes
  @param[in]  Operation    Specifies different tasks to perform:
                   0-Function presence check. Must return EFI_SUCCESS 
                   1-Validate Certificate
                   2-Return Signer Certificate in *Data. Performs cert chaining and time based validity tests
                   3-Return Root CA certificate referenced by Signer. Used to compare with Trusted Cert in FW
                   4-Validate Root Certificate
                   5-Validate Signer Certificate Chain
                   6-Return Signature Digest Algorithm
                   7-Validate Certificate, return Root CA Certificate
                   8-Validate Certificate, return Signer Certificate
                   9-Validate Certificate, return Signer Key Hash (SHA256)
                   10-Validate Certificate and return Success if match is found between Trust and any leaf certificates in the Signer chain
                   11-Validate Certificate, return ptr in *Data to n-modulus of a Signer Key
                   12-Return Success if match is found between Trust and any leaf certificates in the Signer chain
                   13-Validate Certificate, return ptr in *Data to n-modulus of a Root CA Key
                   14-Return ptr in *Data to n-modulus of a Root CA Key
                   15-Return ptr in *Data to n-modulus of a Signer Key
                   16-Validate TimeStamp certificate chain in Pkcs7 Certificate
                   17-Retrieves all embedded certificates from PKCS#7 signed data, and outputs certificate lists chained to the signer's certificates.
                   18-Retrieves all embedded certificates from PKCS#7 signed data, and outputs certificate lists un-chained to the signer's certificates.
                   19-Return null-terminated ASCII string of Subject's CommonName. Caller is responsible for allocating a buffer
                   20-255 reserved values
  @param[in]  Flags        Specifies additional flags to further customize the signing/verifying behavior.

  @retval     Status
    EFI_SUCCESS              The specified PKCS#7 signed data is valid
    EFI_SECURITY_VIOLATION   Invalid PKCS#7 signed data.
    EFI_ACCESS_DENIED        The Trusted certificate does not have a match in SignedData.certificate store.
    EFI_INVALID_PARAMETER    The size of input message or signature does not meet the criteria 
                             of the underlying signature algorithm.
**/
EFI_STATUS
EFIAPI
DxePkcs7Verify (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL *This,
  IN CONST UINT8 *P7Data,
  IN UINTN        P7Size,
  IN CONST UINT8 *TrustedCert,
  IN UINTN        CertSize,
  IN OUT UINT8   **Data,
  IN OUT UINTN   *DataSize,
  IN UINT8        Operation,
  IN UINT32       Flags
  )
{
    EFI_STATUS  Status;
    INTN        err, reason;
static struct pkcs7_signed_data_st* PKCS7cert;
    struct x509_certificate *x509SignCert;
    struct x509_certificate *x509TrustCert;
    UINT8  *x509SignCertPtr, *x509RootCertPtr;
    UINTN  SignCert_len, CARootCert_len;
    struct pkcs7_cert_revoke_info   revokeInfo;
    EFI_CERT_X509_SHA256     *revokeCert;
    UINT8    *CertBuf;

// Mutex functionality:
/*
Structure mutex
Mutual exclusion (mutex) semaphore locking mechanism used to serialize interthread intraprocess activities.
*/
#if PKCS7_MUTEX_LOCK == 1
    switch(Flags & 0x3) {
        case RESET:
            ResetCRmm();    // first time needs mem clean
            MutexLock = RESET;
            PKCS7cert = NULL;
          break;

        case LOCK:
            ResetCRmm();    // first time needs mem clean
            MutexLock = LOCK;
            PKCS7cert = NULL;
          break;

        case RELEASE:       // clear memory after execute routines
            if(MutexLock == RESET)
                ResetCRmm();
            MutexLock = RESET;
          break;

        case KEEP:      
            if(MutexLock == RESET)
                ResetCRmm();
          break;

        default: 
            ResetCRmm();
    }
#else
    PKCS7cert = NULL;
    ResetCRmm();
#endif

    err = -1;
    reason=0;

    switch(Operation){
        case Pkcs7Arg0: 
            Status = EFI_SUCCESS; 
            err = 0;
            break;    
// Validate Digest and returns Root CA cert or Signer Cert
        case Pkcs7CertGetMatchInCertChain:
        case Pkcs7CertValidateGetMatchInCertChain: // obsolete
            reason = -1; // non-0 reason argument forces Pkcs7_Chain_Validate to scan for matching Cert in the Chain if other checks failed
        case Pkcs7CertValidate: 
        case Pkcs7CertValidateGetCAKey:
        case Pkcs7CertValidateGetCACert:
        case Pkcs7CertValidateGetSignerKey:
        case Pkcs7CertValidateGetSignerCert:
        case Pkcs7CertValidateGetSignerKeyHash:
        case Pkcs7TimeStampCertValidateGet:
            if(Data == NULL || DataSize == NULL)
                break;
        case Pkcs7ValidateRootCert:
            if(P7Data == NULL || P7Size == 0)
                break;
#if PKCS7_MUTEX_LOCK == 1
            if (!PKCS7cert) 
#endif
                  PKCS7cert = Pkcs7_parse_Authenticode_certificate(P7Data, P7Size);
            if (!PKCS7cert) 
                break;
            // verify Pkcs7 Signing Cert chain up to the TrustCert...if provided
            if(TrustedCert && CertSize) {
                x509TrustCert = x509_certificate_parse(TrustedCert, CertSize);
                if(x509TrustCert) {
                    err = Pkcs7_x509_certificate_chain_validate_with_timestamp(PKCS7cert, (Operation == Pkcs7TimeStampCertValidateGet), x509TrustCert, NULL, (int*)&reason);
                } else {
                // potentially a TimeStamped revocation cert 
                    revokeCert = (EFI_CERT_X509_SHA256*)TrustedCert;
                    if(CertSize == sizeof(EFI_CERT_X509_SHA256))
                        revokeInfo.ToBeSignedHashLen = 32;
                    else 
                        if(CertSize == sizeof(EFI_CERT_X509_SHA384))
                            revokeInfo.ToBeSignedHashLen = 48;
                        else 
                            if(CertSize == sizeof(EFI_CERT_X509_SHA512))
                                revokeInfo.ToBeSignedHashLen = 64;
                            else { // unsupported Hash struct
                                err = -1;
                                break;
                            }
                    revokeInfo.ToBeSignedHash = (UINT8*)&revokeCert->ToBeSignedHash;
                    err = Pkcs7_x509_certificate_chain_validate_with_timestamp(PKCS7cert, FALSE, NULL, &revokeInfo, (int*)&reason);
                }
                if(err == -1 || 
                    Operation==Pkcs7CertGetMatchInCertChain || 
                    Operation == Pkcs7ValidateRootCert
                )
                    break;     // break(err=0) if leaf found to match trust cert
            }
            // x509SignCert== NULL -> extract SignCert from Pkcs7 crt
            err = Pkcs7_certificate_validate_digest(PKCS7cert, NULL, (UINT8*)*Data, (UINTN)*DataSize);
            if(!err) {
                switch(Operation){
           //Returns CA Root cert after successfully validating Cert chain and signature digest
                    case Pkcs7CertValidateGetCACert:
                        err = Pkcs7_return_cerificate_ptr(PKCS7cert, Data, (size_t *)DataSize, &x509SignCertPtr, (size_t*)&SignCert_len);
                        break;
           //Returns Signing Key from Cert after successfully validating Cert chain and signature digest
                    case Pkcs7CertValidateGetSignerCert:
                        err = Pkcs7_return_cerificate_ptr(PKCS7cert, &x509RootCertPtr, (size_t*)&CARootCert_len, Data, (size_t*)DataSize);
                        break;
                    case Pkcs7CertValidateGetSignerKeyHash:
                        err = Pkcs7_x509_return_signing_Key(PKCS7cert, &x509SignCertPtr, (size_t*)&SignCert_len);
                        if(!err) {
                            DxeHash(This, &gEfiHashAlgorithmSha256Guid, 1,(const UINT8 **) &x509SignCertPtr, (const UINTN*)&SignCert_len, *Data); 
                            *DataSize=SHA256_DIGEST_SIZE;
                        }
                        break;
                    case Pkcs7CertValidateGetSignerKey:
                        err = Pkcs7_x509_return_signing_Key(PKCS7cert, Data, (size_t*)DataSize);
                        break;
                    case Pkcs7CertValidateGetCAKey:
                        err = Pkcs7_return_cerificate_ptr(PKCS7cert, Data, (size_t*)DataSize, &x509SignCertPtr, (size_t*)&SignCert_len);
                        if(!err)
                            err = Pkcs7_x509_return_Cert_pubKey(*Data, (size_t)*DataSize, Data, (size_t*)DataSize);
                        break;
                    case Pkcs7TimeStampCertValidateGet:
                        err = Pkcs7_certificate_validate_timestamp_digest(PKCS7cert, (long*)DataSize);
                        break;
                 } 
            }
            break;

// returns Ptr within input P7Data DER buffer to RootCA cert or Signing Cert
        case Pkcs7GetSignerCert: 
        case Pkcs7GetCACert: 
        case Pkcs7GetSignerKey:
        case Pkcs7GetCAKey:
            if(P7Data == NULL || P7Size == 0 || Data == NULL || DataSize == NULL) {
                break;
            }
#if PKCS7_MUTEX_LOCK == 1
            if (!PKCS7cert) 
#endif
                PKCS7cert = Pkcs7_parse_Authenticode_certificate(P7Data, P7Size);

            if (PKCS7cert) {
                err = Pkcs7_return_cerificate_ptr(PKCS7cert, &x509RootCertPtr, (size_t*)&CARootCert_len, &x509SignCertPtr, (size_t*)&SignCert_len);
                if(!err) {
                    switch(Operation){
                        case Pkcs7GetSignerCert: 
                            *DataSize = SignCert_len;
                            *Data   = x509SignCertPtr;
                            break;
                        case Pkcs7GetCACert: 
                            *DataSize = CARootCert_len;
                            *Data = x509RootCertPtr;
                            break;
                        case Pkcs7GetCAKey:
                            err = Pkcs7_x509_return_Cert_pubKey(x509RootCertPtr, (size_t)CARootCert_len, Data, (size_t*)DataSize);
                            break;
                        case Pkcs7GetSignerKey:
                            err = Pkcs7_x509_return_Cert_pubKey(x509SignCertPtr, (size_t)SignCert_len, Data, (size_t*)DataSize);
                            break;
                    }
                }
            }
            break;
/**
  Retrieves all embedded certificates from PKCS#7 signed data as described in "PKCS #7:
  Cryptographic Message Syntax Standard", and outputs two certificate lists chained or
  un-chained to the signer's certificates.
**/
        case Pkcs7GetChainedCertificates:
        case Pkcs7GetUnChainedCertificates:
            if(P7Data == NULL || P7Size == 0 || Data == NULL || DataSize == NULL) {
                break;
            }
#if PKCS7_MUTEX_LOCK == 1
            if (!PKCS7cert) 
#endif
                PKCS7cert = Pkcs7_parse_Authenticode_certificate(P7Data, P7Size);

            if (PKCS7cert) {
                if(Operation==Pkcs7GetChainedCertificates)
                        err = Pkcs7_x509_get_certificates_List(PKCS7cert, TRUE, Data, (size_t*)DataSize);
                if(Operation==Pkcs7GetUnChainedCertificates)
                        err = Pkcs7_x509_get_certificates_List(PKCS7cert, FALSE, Data, (size_t*)DataSize);
                if(!err) {
                    CertBuf = (UINT8*)AllocatePool(*DataSize);
                    if (CertBuf == NULL) {
                      err = -1; break;
                    }
                    MemCpy (CertBuf, *Data, *DataSize);
                    *Data = CertBuf;
                }
            }
            break;
            /**
             *  Produces a Null-terminated ASCII string in an output buffer based on a Null-terminated
             *  ASCII format string from a Subject name structure of a CommonName.
             */
        case x509GetSubjectsCommonNameStr:
            if(P7Data == NULL || P7Size == 0 || Data == NULL || DataSize == NULL) {
                break;
            }
            err = Pkcs7_x509_return_SubjectNameStr(P7Data, P7Size, *Data, (size_t)*DataSize );
            break;

        case x509ValidateCertChain:
        //Validates Signer certificate's key against Trusted Cert
            if(TrustedCert && CertSize) {
                x509SignCert = x509_certificate_parse(P7Data, P7Size);
                x509TrustCert = x509_certificate_parse(TrustedCert, CertSize);
                if (x509TrustCert && x509SignCert) {
                    err = x509_certificate_chain_validate(x509TrustCert, x509SignCert, (int*)&reason);
                }
            }
            break;

        case Pkcs7GetDigestAlgorithm: 
        // validate input params.
            if(Data == NULL || DataSize == NULL) {
                break;
            }
#if PKCS7_MUTEX_LOCK == 1
            if (!PKCS7cert) 
#endif
                PKCS7cert = Pkcs7_parse_Authenticode_certificate(P7Data, P7Size);
            if (PKCS7cert) {
                err = Pkcs7_return_digestAlgorithm(PKCS7cert, (UINT8*)*Data);
                if(!err)
                    *DataSize = sizeof(UINT8);
             }
             break;

        default: 
            Status = EFI_INVALID_PARAMETER;
            err = -1;
    }

// Security concern, memory heap is being cleared on exit 
    if(err)  
        MutexLock = RESET;

#if PKCS7_MUTEX_LOCK == 1
        if(MutexLock == RESET) 
#endif            
    {
        ResetCRmm();
        PKCS7cert = NULL;
    }

// failed to process
    Status = !err ? EFI_SUCCESS:EFI_SECURITY_VIOLATION;

    return Status;
}

/**
   Function verifies that the specified signature matches the specified hash. 
                 Verifies the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in
                 RSA PKCS#1.
                 This function decrypts the signature with the provided key and then compares 
                 the decrypted value to the specified hash value

  @param[in]  This          Pointer to the AMI_DIGITAL_SIGNATURE_PROTOCOL instance.
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
DxePkcs1Verify (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL *This,
  IN CRYPT_HANDLE   *PublicKey,
  IN CRYPT_HANDLE   *Hash,
  IN VOID           *Signature,
  IN UINTN           SignatureSize,
  IN UINT32          Flags
  )
{
    EFI_STATUS  Status;
    INTN        err;
    struct      crypto_rsa_key *key = NULL;
    size_t     *sig_len=(size_t*)&SignatureSize;
    INT32       modulus_bitlen = DEFAULT_RSA_SIG_LEN << 3;
    UINT32      HashLen;
// Only supporting RSASSA_PKCS1V15 & PSS signature types
    if(!((Flags & EFI_CRYPT_RSASSA_PKCS1V15) || 
         (Flags & EFI_CRYPT_RSASSA_PSS)) )
            return EFI_INVALID_PARAMETER;

    if(!PublicKey || !Hash || !Signature)
        return EFI_INVALID_PARAMETER;

    ResetCRmm();
    MutexLock = RESET;

// For now Public Key is supported in 2 formats: RAW 256 bytes and ASN.1 Integer
    if(!guidcmp(&PublicKey->AlgGuid, &gEfiCertRsa2048Guid))
        key = crypto_import_rsa2048_public_key(PublicKey->Blob, PublicKey->BlobSize, (UINT8*)&KeyE, LenE);
    else 
        return EFI_INVALID_PARAMETER;

// 2 hash types supported: SHA1 & SHA256
    if(!guidcmp(&Hash->AlgGuid, &gEfiHashAlgorithmSha256Guid))
        HashLen = SHA256_DIGEST_SIZE;
    else if(!guidcmp(&Hash->AlgGuid, &gEfiHashAlgorithmSha1Guid))
            HashLen = SHA1_DIGEST_SIZE;
        else
            HashLen = SHA256_DIGEST_SIZE;

    if(key == NULL )
        err = -1;
    else
        err = crypto_rsa_exptmod((const UINT8*)Signature, (size_t)SignatureSize, (UINT8*)&Rsa2048Sig, sig_len, key, 0);

// locate Hash inside the decrypted signature body and compare it with given Hash;
    if(!err) {
        if(Flags & EFI_CRYPT_RSASSA_PKCS1V15)
        {
            // Validate PKCS#1 Padding
//            err = pkcs_1_v1_5_decode((const UINT8 *)Hash->Blob, HashLen, (const UINT8 *)&Rsa2048Sig, (unsigned long)*sig_len);
            err = MemCmp((void*)(UINTN)Hash->Blob, (void*)(UINTN)((UINTN)Rsa2048Sig + (UINTN)(*sig_len - HashLen)), HashLen);
        }
        else //(Flags & EFI_CRYPT_RSASSA_PSS))
            // Validate PKCS_1 PSS Signature: padding & hash
            err = pkcs_1_pss_decode((UINT8*)Hash->Blob, HashLen,(const unsigned char *)&Rsa2048Sig, (unsigned long)*sig_len, 
                    saltlen,  modulus_bitlen);
    }

    Status = !err ? EFI_SUCCESS:EFI_SECURITY_VIOLATION;

// Security concern, memory heap is being cleared on exit 
    ResetCRmm();

    return Status;
}

/**
   Passes FwKey info from a Hob to an external Key handler structure

  @param[in]  SystemTable Efi System Table
  @param[out] Key         ptr to the buffer to hold the target key parameters

  @retval     Status
               EFI_NOT_FOUND  - Can't find the Key Hob
               EFI_INVALID_PARAMETER - Wrong KeyID
**/
EFI_STATUS
CryptoGetFwKey (
  IN EFI_SYSTEM_TABLE *SystemTable,
  IN CRYPT_HANDLE   *Key
)
{
    EFI_STATUS    Status=EFI_NOT_FOUND;
    FW_KEY_HOB     *pFwKeyHob;
    static EFI_GUID HobListGuid = HOB_LIST_GUID;

    if(!Key) return EFI_INVALID_PARAMETER;

    pFwKeyHob = GetEfiConfigurationTable(SystemTable, &HobListGuid);
    if (pFwKeyHob!=NULL) {
        Status = FindNextHobByGuid(&gPRKeyGuid, (void **)&pFwKeyHob);
        if(!EFI_ERROR(Status)) {
// make sure the Key buffer is mapped to FV_BB address space 
/*
            if(!((UINTN)pFwKeyHob->KeyAddress > FV_BB_BASE && 
                 (UINT64)((UINTN)pFwKeyHob->KeyAddress+pFwKeyHob->KeySize) < 
                    (UINT64)(FV_BB_BASE+(UINT64)FV_BB_BLOCKS*FLASH_BLOCK_SIZE))) {
                return EFI_NOT_FOUND;
*/
            Key->Blob = (UINT8*)((UINTN)(pFwKeyHob->KeyAddress));
            Key->BlobSize = pFwKeyHob->KeySize;
            Key->AlgGuid  = pFwKeyHob->KeyGuid;

            DEBUG ((AmiCryptoPkg_DEBUG_LEVEL,"Get Key Hob %g\n(addr %X, len 0x%lx) %X...\n", &Key->AlgGuid, Key->Blob, Key->BlobSize, *(UINT32*)Key->Blob));
        }
    }


    return Status;
}
/**
   Return Firmware Signing Key from a local storage

  @param[in]   This         Pointer to the AMI_DIGITAL_SIGNATURE_PROTOCOL instance.
  @param[out]  Key          ptr to the buffer to hold the target key
  @param[in]   AlgId        Key GUID
  @param[in]   KeyLen       Length of the target Key buffer
  @param[in]   Flags        Specifies additional flags to further customize the GetKey behavior.

  @retval     Status
    EFI_SUCCESS
    EFI_ACCESS_DENIED        The key could not be located
    EFI_INVALID_PARAMETER    Wrong KeyID
    BUFFER_TOO_SMALL         The Key->BlobSize will contain the size of the buffer to be prepared
**/
EFI_STATUS
EFIAPI
DxeGetKey (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL *This,
  OUT     CRYPT_HANDLE               *Key,
  IN      EFI_GUID                   *AlgId,
  IN      UINTN                      KeyLen,
  IN      UINT32                     Flags
  )
{
    if(!AlgId || !Key)
        return EFI_INVALID_PARAMETER;

    // Guid must match one of the valid keys we can use in Sig verification.
    if(guidcmp(AlgId, &gPRKeyGuid))
        return EFI_UNSUPPORTED;

    if(gKey.Blob && gKey.BlobSize){

// make sure the Key buffer is mapped to FV_BB address space 
/*        if(!((UINT32)gKey.Blob > FV_BB_BASE && 
             (UINT64)((UINT32)gKey.Blob+gKey.BlobSize) < 
                (UINT64)(FV_BB_BASE+(UINT64)FV_BB_BLOCKS*FLASH_BLOCK_SIZE)))
            return EFI_NOT_FOUND;
*/
        Key->BlobSize = gKey.BlobSize;
        Key->AlgGuid = gKey.AlgGuid;
        if(Key->Blob) {
            if(KeyLen < gKey.BlobSize)
                return EFI_BUFFER_TOO_SMALL;
            else
                MemCpy(Key->Blob, gKey.Blob, gKey.BlobSize);
        } else
            Key->Blob = gKey.Blob;// upd address to int buffer

        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}
/**
  Function compares the input PublicKey against Platform Signing Key (PK) image in the flash.

  @param[in]   This         Pointer to the AMI_DIGITAL_SIGNATURE_PROTOCOL instance.
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
DxeVerifyKey (
  IN CONST AMI_DIGITAL_SIGNATURE_PROTOCOL *This,
  IN EFI_GUID       *AlgId,
  IN CRYPT_HANDLE   *Key
  )
{
    EFI_STATUS          Status;
    CRYPT_HANDLE        PubKeyHndl;
    UINT8               Hash[SHA256_DIGEST_SIZE]={0};
    UINT8               *KeyFfs, *KeyCmp;
    size_t              KeyLen;
    UINT32              Flags=0;
    INTN                err;

    if(!AlgId || !Key || !Key->Blob)
        return EFI_INVALID_PARAMETER;

    // Guid must match one of the valid keys we can use in Sig verification.
    if(guidcmp(AlgId, &gPRKeyGuid))
        return EFI_UNSUPPORTED;

// Get PRKey
    PubKeyHndl.Blob = NULL;
    PubKeyHndl.BlobSize = 0;
    Status = DxeGetKey(NULL, &PubKeyHndl, AlgId, 0, Flags);
    if(!EFI_ERROR(Status))
    {
        KeyFfs = PubKeyHndl.Blob;
        KeyLen = (size_t)PubKeyHndl.BlobSize;
        KeyCmp = Key->Blob;

        // If FwKey is Hash of Rsa2048 Key and Key->Algo is Rsa2048 - 
        // prepare Key for SHA256 Hash compare
        if(!guidcmp(&Key->AlgGuid, &gEfiCertRsa2048Guid)) 
        {
            if(!guidcmp(&PubKeyHndl.AlgGuid, &gEfiCertSha256Guid)) 
            {
                KeyCmp = Hash;
                KeyLen = (size_t)Key->BlobSize;
                sha256_vector(1, (const UINT8**)&Key->Blob, (const size_t *)&KeyLen, Hash);
                KeyLen = SHA256_DIGEST_SIZE;
            } else 
                // if FwKey is x509 and Key->Algo - gEfiCertRsa2048Guid:
                // derive nModulus from x509 Key Cert for comparison
                if(!guidcmp(&PubKeyHndl.AlgGuid, &gEfiCertX509Guid)) 
                {
                    KeyFfs = &Rsa2048Sig[0];
                    KeyLen = DEFAULT_RSA_KEY_MODULUS_LEN;
                    ResetCRmm();
                    err = Pkcs7_x509_return_Cert_pubKey((UINT8*)PubKeyHndl.Blob, (size_t)PubKeyHndl.BlobSize, &KeyFfs,(size_t*) &KeyLen);
                    ResetCRmm();
                    if(err) return EFI_SECURITY_VIOLATION;
                }
        }
        err = MemCmp(KeyFfs, KeyCmp, KeyLen);

        Status = !err ? EFI_SUCCESS:EFI_SECURITY_VIOLATION;
    }
    return Status;
}
//----------------------------------------------------------------------------
// END of Crypto DXE Function definitions
//----------------------------------------------------------------------------

/**
  InSMM Driver Entry point. This function is called from outside of SMM during SMM registration.

  @param[in]   ImageHandle     Image Handle
  @param[in]   SystemTable     Efi System Table

  @retval     Status
**/
EFI_STATUS EFIAPI InSmmFunction(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS  Status;
    EFI_HANDLE  DummyHandle;
    UINT8       *pKey;

    InitAmiSmmLib( ImageHandle, SystemTable );
    
    //
    // Get location of PRKey from .ffs
    // ReadFfs/FindHob are pre-boot services whereas Key needs to be available during EFI runtime
    //
    gKey.Blob = NULL;
    Status = CryptoGetFwKey(SystemTable, &gKey);
    if(!EFI_ERROR(Status) && gKey.Blob) {
    // re-allocate buffer in runtime mem 
        pKey = AllocatePool(gKey.BlobSize);
        if (pKey != NULL) {    
            MemCpy(pKey, gKey.Blob, gKey.BlobSize);
            gKey.Blob = pKey;
        }
    }

    DEBUG ((AmiCryptoPkg_DEBUG_LEVEL,"Save Key in SMM %r (addr %X)\n", Status, gKey.Blob));

///////////////////////////////////////////////////////////////////////////////////
// Install the SMM clone of DXE Crypto protocol
////////////////////////////////////////////////////////////////////////////////////
// Temporary w/a. installing it on pBS for backward compatibility with consumers who use it or locate it on pBS
   DummyHandle = NULL;
   pBS->InstallMultipleProtocolInterfaces (
            &DummyHandle,
            &gAmiSmmDigitalSignatureProtocolGuid, 
            &mAmiSig,
            NULL
            );
// Main interface
   DummyHandle = NULL;
   return pSmst->SmmInstallProtocolInterface (    
            &DummyHandle,
            &gAmiSmmDigitalSignatureProtocolGuid, 
            EFI_NATIVE_INTERFACE,
            &mAmiSig
            );
}

/**
  DXE handle for CryptoDXE Driver. Invoked from DXE Driver entry point.

  @param[in]   ImageHandle     Image Handle
  @param[in]   SystemTable     Efi System Table

  @retval     Status
**/
EFI_STATUS EFIAPI NotInSmmFunction(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS  Status;
    UINT8       *pKey;

    InitAmiLib(ImageHandle, SystemTable);

    //
    // Get location of PRKey from .ffs
    // ReadFfs/FindHob are preboot services whereas Key needs to be available during EFI runtime
    //
    gKey.Blob = NULL;
    Status = CryptoGetFwKey(SystemTable, &gKey);
    if(!EFI_ERROR(Status) && gKey.Blob) {
    // re-allocate buffer in runtime mem 
        pKey = AllocateRuntimePool(gKey.BlobSize);
        if (pKey != NULL) {    
            MemCpy(pKey, gKey.Blob, gKey.BlobSize);
            gKey.Blob = pKey;
        }
    }

    DEBUG ((AmiCryptoPkg_DEBUG_LEVEL,"Save Key in DXE %r (addr %X)\n", Status, gKey.Blob));

///////////////////////////////////////////////////////////////////////////////
//
// Install the DXE Crypto protocols
//
///////////////////////////////////////////////////////////////////////////////
   return pBS->InstallMultipleProtocolInterfaces (
            &ImageHandle,
            &gAmiDigitalSignatureProtocolGuid, &mAmiSig,
            NULL
            );
}

/**
  DXE Driver Entry point. 

  @param[in]   ImageHandle     Image Handle
  @param[in]   SystemTable     Efi System Table

  @retval     Status
**/
/*
EFI_STATUS EFIAPI
CryptoDxe_Init (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);
    //
    // Install DXE & SMM Crypto Services
    //
    return InitSmmHandlerEx(ImageHandle, SystemTable, InSmmFunction, NotInSmmFunction);
}
*/
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
