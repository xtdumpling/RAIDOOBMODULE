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
  CryptLib.h
  Defines Aptio cryptographic library APIs.
  This Cryptographic Library provides implementations of basic and extended cryptography
  primitives (SHA-1, SHA-256, SHA-512, SHA-512, RSA, x509, PKCS1, PKCS7, ASN1.1) for UEFI security functionality enabling.
**/

#ifndef CRYPTOLIB_H
#define CRYPTOLIB_H

#if defined(_WIN64)
//typedef unsigned __int64        size_t;
#else
//typedef unsigned int            size_t;
#ifndef __size_t__
#define __size_t__
typedef UINTN           size_t;
#endif // __size_t__
#endif

///
/// MD5 digest size in bytes
///
#define MD5_DIGEST_SIZE     16
///
/// SHA-1 digest size in bytes.
///
#define SHA1_DIGEST_SIZE    20
///
/// SHA-256 digest size in bytes
///
#define SHA256_DIGEST_SIZE  32
///
/// SHA-384 digest size in bytes
///
#define SHA384_DIGEST_SIZE  48
///
/// SHA-512 digest size in bytes
///
#define SHA512_DIGEST_SIZE  64
///
/// Hash Algorithm types
///
typedef enum { SHA1, SHA256, SHA384, SHA512} HASH_ALG;
///
/// Default RSA2048 Modulus length
///
#define DEFAULT_RSA_KEY_MODULUS_LEN 256 // 2048 bits
///
/// Default RSA2048 PKCSv1.5 Signature length
///
#define DEFAULT_RSA_SIG_LEN DEFAULT_RSA_KEY_MODULUS_LEN // This is true as long as > data

// Flags to define type of signature to process
#define EFI_CRYPT_RSASSA_PKCS1V15     1
#define EFI_CRYPT_RSASSA_PSS          2

//
// Common Crypto Lib API
//
/**
  Initialize Amy Crypto library internal state. 

  @param[in]  ImageHandle
  @param[in]  SystemTable

  @retval     EFI_STATUS

**/
EFI_STATUS RuntimeCryptLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS RuntimeCryptLibDestructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);


/**
  sets the debug message print mode

  @param[in]  level    print level: 0 - no messages, 1 - minimal, 2 - full

  @retval     none

**/
VOID wpa_set_trace_level(int level);

/**
  Updates the local value of crypto EfiTime 

  @param[in]  EfiTime 

  @retval     none

**/
VOID set_crypt_efitime(EFI_TIME *EfiTime);

// 0. auxilary Memory Manager 
/**
  Initializes Crypo Memory Manager

  @param[in]  pHeap 
  @param[in]  HeapSize 
  
  @retval     none

**/
void InitCRmm(void* pHeap, UINTN HeapSize);

/**
  Resets Crypto Memory Manager. Clears mem heap and descriptors

  @param[in]  none
  
  @retval     none
**/
void ResetCRmm();

// 1. Rsa Public Key import
/**
  Imports RSA2048 public key binary from RAW format into internal crypto_rsa_key data structure

  @param[in]  KeyN    Pointer to N public modulus buffer
  @param[in]  KeyE    Pointer to E public exponent buffer
  @param[in]  LenN    Size of KeyN buffer (must be 256)
  @param[in]  LenE    Size of KeyE buffer
  
  @retval     crypto_rsa_key Pointer to Key data structure crypto_rsa_key 
                             NULL if failed to import Key
**/
struct crypto_rsa_key *crypto_import_rsa2048_public_key(const UINT8 *KeyN, size_t lenN,
                        const UINT8 *KeyE, size_t lenE);

/**
  Imports RSA2048 public key from ASN.1 DER format into internal crypto_rsa_key data structure

  @param[in]  key    Pointer to public key buffer
  @param[in]  len    Size of key buffer
  
  @retval     crypto_rsa_key Pointer to Key data structure crypto_rsa_key 
                             NULL if failed to import Key
**/
struct crypto_rsa_key *crypto_import_asn1_public_key(const UINT8 *key, size_t len);

/**
  Do in-place decoding of base-64 data of LENGTH in BUFFER.

  @param[in]  buffer    Data to be decoded
  @param[in]  length    Length of the data to be decoded
  
  @retval     size_t    New length of the buffer bytes of decoded data,
                        or -1 n failure
**/
size_t    base64_decode (char *buffer, size_t length);

// 2. RSA Exponentiation
/**
  RSA modular exponentiation

  @param[in]  in       Input data
  @param[in]  inlen    Length of the data
  @param[in]  use_private 1 = Use RSA private key, 0 = Use RSA public key
  
  @retval     out      Buffer for output data
  @retval     outlen   Maximum size of the output buffer and used size on success
  @retval     key    RSA key

  @retval     0 success
             -1 failed
**/
int crypto_rsa_exptmod(const UINT8 *in, size_t inlen, UINT8 *out, size_t *outlen,
                       struct crypto_rsa_key *key, int use_private);

// 3. PKCS#1 Signatures
/**
  PKCS #1 v2.00 PSS sig decode

  @param[in]  msghash        The hash to verify
  @param[in]  msghashlen     The length of the hash (octets)
  @param[in]  sig            The signature data (encoded data)
  @param[in]  siglen         The length of the signature data (octets)
  @param[in]  saltlen        The length of the salt used (octets)
  @param[in]  modulus_bitlen The bit length of the RSA modulus
        
  @retval     CRYPT_OK if successful (even if the comparison failed)
**/
int pkcs_1_pss_decode(const unsigned char *msghash, unsigned long msghashlen,
                      const unsigned char *sig,     unsigned long siglen,
                      unsigned long saltlen,  unsigned long modulus_bitlen);

/**
  Strip PKCS #1 header 

  @param[in]  sig            The signature data (encoded data)
  @param[in]  siglen         The length of the signature data (octets)
        
  @retval     CRYPT_OK if successful (even if the comparison failed)
**/
int pkcs_1_v1_5_decode(const UINT8 *hash, size_t hashlen, const UINT8 *sig, unsigned long siglen);

// 4. SHA hashing
/**
   MD5 hash for data vector

  @param[in]   num_elem        Number of elements in the data vecto
  @param[in]   addr[]     Pointers to the data areas
  @param[in]   len[]      Lengths of the data blocks
  @param[out]  mac[]      Buffer for the hash
  
  @retval     none
**/
void md5_vector(size_t num_elem, const UINT8 *addr[], const size_t *len, UINT8 *mac);

/**
   SHA-1 hash for data vector

  @param[in]   num_elem   Number of elements in the data vector
  @param[in]   addr[]     Pointers to the data areas
  @param[in]   len[]      Lengths of the data blocks
  @param[out]  mac[]      Buffer for the hash
  
  @retval     none
**/
void sha1_vector(size_t num_elem, const UINT8 *addr[], const size_t *len, UINT8 *mac);

/**
   SHA-256 hash for data vector

  @param[in]   num_elem   Number of elements in the data vector
  @param[in]   addr[]     Pointers to the data areas
  @param[in]   len[]      Lengths of the data blocks
  @param[out]  mac[]      Buffer for the hash
  
  @retval     none
**/
void sha256_vector(size_t num_elem, const UINT8 *addr[], const size_t *len, UINT8 *mac);

/**
   SHA-384 hash for data vector

  @param[in]   num_elem   Number of elements in the data vector
  @param[in]   addr[]     Pointers to the data areas
  @param[in]   len[]      Lengths of the data blocks
  @param[out]  mac[]      Buffer for the hash
  
  @retval     none
**/
void sha384_vector(size_t num_elem, const UINT8 *addr[], const size_t *len, UINT8 *mac);

/**
   SHA-512 hash for data vector

  @param[in]   num_elem   Number of elements in the data vector
  @param[in]   addr[]     Pointers to the data areas
  @param[in]   len[]      Lengths of the data blocks
  @param[out]  mac[]      Buffer for the hash
  
  @retval     none
**/
void sha512_vector(size_t num_elem, const UINT8 *addr[], const size_t *len, UINT8 *mac);

// 5. Pkcs7/x509 certificates
/**
  Parse a X.509 certificate in DER format. Caller is responsible for freeing the returned certificate by calling
  x509_certificate_free().

  @param[in]   buf      Pointer to the X.509 certificate in DER format
  @param[in]   len      Buffer length
  
  @retval     x509_certificate  Pointer to the parsed certificate or %NULL on failure
**/
struct x509_certificate *x509_certificate_parse(const UINT8 *buf, size_t len);

/**
  Validate X.509 certificate chain.

  @param[in]   trusted    List of trusted certificates
  @param[in]   chain      Certificate chain to be validated (first chain must be issued by signed by the second certificate in the chain and so on)
  @param[in]   reason     Buffer for returning failure reason (X509_VALIDATE_*)
  
  @retval     0 if chain is valid, -1 if not
**/
int x509_certificate_chain_validate(struct x509_certificate *trusted, struct x509_certificate *chain, int *reason);

/**
  Parse a Pkcs7 Authenticode certificate in DER format. Caller is responsible for freeing the returned certificate by calling
  Pkcs7_certificate_free().

  @param[in]   buf    Pointer to the Pkcs7 Authenticode SignedData in DER format
  @param[in]   len    Buffer length
  
  @retval     pkcs7_signed_data_st Pointer to the parsed certificate or %NULL on failure
**/
struct pkcs7_signed_data_st * Pkcs7_parse_Authenticode_certificate(const UINT8 *buf, size_t len);

/**
  Verify PKCS7 certificate signature.

  @param[in]   SignedData   Pointer to the Pkcs7 certificate structure
  @param[in]   SignCert    Signing certificate containing the public key
  
  @retval     0 if cert has a valid signature that was signed by the issuer, -1 if not
**/
int Pkcs7_certificate_validate_digest(struct pkcs7_signed_data_st *SignedData, struct x509_certificate *SignCert,
                            UINT8 *fileHash, size_t hashLen);

/**
  Return certificate with signer's public key

  @param[in]   SignedData   Pointer to the Pkcs7 certificate structure
  
  @retval      ptr to x509 certificate if chain is valid, NULL if not found
**/
struct x509_certificate 
    * Pkcs7_return_signing_cerificate(struct pkcs7_signed_data_st *SignedData);

/**
   Return Time stamping root signing certificate

  @param[in]   SignedData   Pointer to the Pkcs7 certificate structure
  
  @retval      ptr to x509 certificate if chain is valid, NULL if not found
**/
struct x509_certificate 
    * Pkcs7_return_timestamp_signing_cerificate(struct pkcs7_signed_data_st *SignedData);

/**
  Validate Pkcs7 certificate chain

  @param[in]   trusted    List of trusted certificates
  @param[in]   chain      Certificate chain to be validated (first chain must be issued by signed by the second certificate in the chain and so on)
  @param[in]   reason     Buffer for returning failure reason (X509_VALIDATE_*)
  
  @retval     0 if chain is valid, -1 if not
**/
int Pkcs7_x509_certificate_chain_validate(struct pkcs7_signed_data_st *SignedData, 
        struct x509_certificate *trusted, int *reason);

/**
  Returns a pointer within DER buffer to:
  1. Signing certificate 
  2. Root CA certificate that is used to sign Signing certificate

  @param[in]   SignedData    List of trusted certificates
  @param[out]  CAcert        Root CA certificate that is used to sign Signing certificate
  @param[out]  CAcert_len    Certificate length
  @param[out]  SignCert      Signing certificate
  @param[out]  SignCert_len  Certificate length
  
  @retval     0 if chain is valid, -1 if not
**/
int Pkcs7_return_cerificate_ptr(struct pkcs7_signed_data_st *SignedData, 
        UINT8** CAcert, size_t* CAcert_len, 
        UINT8** SignCert, size_t* SignCert_len);

/**
  Extract n-Modulus of a Public Key from a Signer x509 certificate within Pkcs7 certificate

  @param[in]   SignedData        Pre-parsed Pkcs7 Certificate structure
  @param[out]  public_key        Pointer to a n-Modulus within certificate
  @param[out]  public_key_len    Key length
  
  @retval     0 key present, -1 if not found
**/
int Pkcs7_x509_return_signing_Key(struct pkcs7_signed_data_st *SignedData,
    UINT8 **public_key, size_t *public_key_len);

/**
  Extract n-Modulus of a Public Key from x509 certificate

  @param[in]   pCert            x509 DER Certificate buffer
  @param[out]  public_key        Pointer to a n-Modulus within certificate
  @param[out]  public_key_len    Key length
  
  @retval     0 key present, -1 if not found
**/
int Pkcs7_x509_return_Cert_pubKey(UINT8 *pCert, size_t cert_len,
    UINT8 **public_key, size_t *public_key_len);

/**
  Return digestAlgorithm type. Enumerated type:
  0-SHA1, 1-SHA256, 2-SHA384, 3-SHA512

  @param[in]   PKCS7cert         Pre-parsed Pkcs7 Certificate structure
  @param[out]  HashType          Storage for Hash type value
  
  @retval     cert if chain is valid, -1 if not found
**/
int Pkcs7_return_digestAlgorithm(struct pkcs7_signed_data_st *PKCS7cert, UINT8* HashType);

/**
  Common routine to validate Pkcs7 certificate chain and revoked time stamped certs
  Supported RFC6131 TimeStampToken format

  @param[in]  SignedData  Pre-parsed Pkcs7 Certificate with a cert chain to be validated 
                          (first chain must be issued by signed by the second certificate in the chain and so on)
  @param[in]  bTimeStampCert Type of certificate to test
  @param[in]  trusted      List of trusted x509 certificates
  @param[in]  revokeInfo   Hash certificate
  @param[in]  action       Buffer for returning either result of operation or intermediate data, e.g TimeOfSigning
  
  @retval     cert if chain is valid, -1 if not found
**/
#ifndef pkcs7_cert_revoke_info
struct pkcs7_cert_revoke_info {
    UINT8   *ToBeSignedHash;
    size_t   ToBeSignedHashLen;
};
#endif
int Pkcs7_x509_certificate_chain_validate_with_timestamp(
                    struct pkcs7_signed_data_st *SignedData,
                    BOOLEAN bTimeStampCert,
                    struct x509_certificate *trusted,
                    struct pkcs7_cert_revoke_info *revokeInfo,
                    int *action);

/**
  Convert broken-down time into seconds since 1970-01-01
  Note: The result is in seconds from Epoch, i.e., in UTC, not in local time
        which is used by POSIX mktime().
        
  @param[in]  year: Four digit year
  @param[in]  month: Month (1 .. 12)
  @param[in]  day: Day of month (1 .. 31)
  @param[in]  hour: Hour (0 .. 23)
  @param[in]  min: Minute (0 .. 59)
  @param[in]  sec: Second (0 .. 60)
  @param[out] t: Buffer for returning calendar time representation 
                 (seconds since 1970-01-01 00:00:00)
  
  @retval     0 on success, -1 on failure
**/
#ifndef __os_time_t__
#define __os_time_t__
typedef long os_time_t;
#endif // __os_time_t__

int os_mktime(int year, int month, int day, int hour, int min, int sec,
              os_time_t *t);

/**
  Validate RFC6131 TimeStampToken digest

  @param[in]   pCert            PKCS7 Certificate
  @param[out]  TimeOfSigning    os_time data structure with time of signing
  
  @retval      0 if chain is valid, -1 if not
**/
int Pkcs7_certificate_validate_timestamp_digest(struct pkcs7_signed_data_st *SignedData, os_time_t *TimeOfSigning);

/**
 *  Produces a Null-terminated ASCII string in an output buffer based on a Null-terminated
 *  ASCII format string from a Subject name structure of a CommonName.
 * Returns: 0 if operation is success, -1 if not
 */
int Pkcs7_x509_return_SubjectNameStr(UINT8 *pCert, size_t cert_len, UINT8 *buf, size_t len);

/**
  Retrieves all embedded certificates from PKCS#7 signed data as described in "PKCS #7:
  Cryptographic Message Syntax Standard", and outputs two certificate lists chained or
  unchained to the signer's certificates.
**/  
int Pkcs7_x509_get_certificates_List(struct pkcs7_signed_data_st *SignedData, BOOLEAN bChained,
                                     UINT8** CertList, size_t* ListLength);
#endif /* CRYPTOLIB_H */
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
