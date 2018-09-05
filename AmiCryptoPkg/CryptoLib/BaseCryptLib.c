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
BaseCryptLib.h

 Defines base cryptographic library APIs compatible with EDK2 BaseCryptLib.
 The Base Cryptographic Library provides implementations of basic cryptography
 primitives (SHA-1, SHA-256, RSA, etc) for UEFI security functionality enabling.
**/

#include "includes.h"
#include <BaseCryptLib.h>
#include <CryptLib.h>
#include "common.h"
#include "bignum.h"
#include "rsa.h"

// Structure is defined in rsa.c
struct crypto_rsa_key {
    int private_key; /* whether private key is set */
    struct bignum *n; /* modulus (p * q) */
    struct bignum *e; /* public exponent */
    /* The following parameters are available only if private_key is set */
//    struct bignum *d; /* private exponent */
//    struct bignum *p; /* prime p (factor of n) */
//    struct bignum *q; /* prime q (factor of n) */
//    struct bignum *dmp1; /* d mod (p - 1); CRT exponent */
//    struct bignum *dmq1; /* d mod (q - 1); CRT exponent */
//    struct bignum *iqmp; /* 1 / q mod p; CRT coefficient */
};

struct MD5Context {
    UINT32 buf[4];
    UINT32 bits[2];
    UINT8 in[64];
};
void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, const void *data, UINT32 len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);

struct SHA1Context {
    UINT32 state[5];
    UINT32 count[2];
    UINT8 buffer[64];
};
void SHA1Init(struct SHA1Context *context);
void SHA1Update(struct SHA1Context *context, const void *data, UINT32 len);
void SHA1Final(unsigned char digest[20], struct SHA1Context *context);

struct sha256_state {
    UINT64 length;
    UINT32 state[8], curlen;
    UINT8  buf[64];
};
void sha256_init(struct sha256_state *md);
int sha256_process(struct sha256_state *md, const unsigned char *in, unsigned long inlen);
int sha256_done(struct sha256_state *md, unsigned char *out);

struct sha512_state {
    UINT64 length;
    UINT32 state[8], curlen;
    UINT8  buf[128];
};
void sha384_init(struct sha512_state *md);
int sha384_done(struct sha512_state *md, unsigned char *out);

void sha512_init(struct sha512_state *md);
int sha512_process(struct sha512_state *md, const unsigned char *in, unsigned long inlen);
int sha512_done(struct sha512_state *md, unsigned char *out);

/**
  Retrieves the size, in bytes, of the context buffer required for MD5 hash operations.

  If this interface is not supported, then return zero.

  @return  The size, in bytes, of the context buffer required for MD5 hash operations.
  @retval  0   This interface is not supported.

**/
UINTN
EFIAPI
Md5GetContextSize (
  VOID
  ){
    return sizeof(struct MD5Context);
}

/**
  Initializes user-supplied memory pointed by Md5Context as MD5 hash context for
  subsequent use.

  If Md5Context is NULL, then return FALSE.
  If this interface is not supported, then return FALSE.

  @param[out]  Md5Context  Pointer to MD5 context being initialized.

  @retval TRUE   MD5 context initialization succeeded.
  @retval FALSE  MD5 context initialization failed.
  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
Md5Init (
  OUT  VOID  *Md5Context
  ){
    MD5Init((struct MD5Context*)Md5Context);
    return TRUE;
}

/**
  Makes a copy of an existing MD5 context.

  If Md5Context is NULL, then return FALSE.
  If NewMd5Context is NULL, then return FALSE.
  If this interface is not supported, then return FALSE.

  @param[in]  Md5Context     Pointer to MD5 context being copied.
  @param[out] NewMd5Context  Pointer to new MD5 context.

  @retval TRUE   MD5 context copy succeeded.
  @retval FALSE  MD5 context copy failed.
  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
Md5Duplicate (
  IN   CONST VOID  *Md5Context,
  OUT  VOID        *NewMd5Context
  ){
    //
    // Check input parameters.
    //
    if (Md5Context == NULL || NewMd5Context == NULL) {
      return FALSE;
    }

    memcpy (NewMd5Context, Md5Context, sizeof (struct MD5Context));

    return TRUE;
}

/**
  Digests the input data and updates MD5 context.

  This function performs MD5 digest on a data buffer of the specified size.
  It can be called multiple times to compute the digest of long or discontinuous data streams.
  MD5 context should be already correctly initialized by Md5Init(), and should not be finalized
  by Md5Final(). Behavior with invalid context is undefined.

  If Md5Context is NULL, then return FALSE.
  If this interface is not supported, then return FALSE.

  @param[in, out]  Md5Context  Pointer to the MD5 context.
  @param[in]       Data        Pointer to the buffer containing the data to be hashed.
  @param[in]       DataSize    Size of Data buffer in bytes.

  @retval TRUE   MD5 data digest succeeded.
  @retval FALSE  MD5 data digest failed.
  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
Md5Update (
  IN OUT  VOID        *Md5Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  ){
    MD5Update((struct MD5Context*)Md5Context, (UINT8*)Data, (UINT32)DataSize);
    return TRUE;
}

/**
  Completes computation of the MD5 digest value.

  This function completes MD5 hash computation and retrieves the digest value into
  the specified memory. After this function has been called, the MD5 context cannot
  be used again.
  MD5 context should be already correctly initialized by Md5Init(), and should not be
  finalized by Md5Final(). Behavior with invalid MD5 context is undefined.

  If Md5Context is NULL, then return FALSE.
  If HashValue is NULL, then return FALSE.
  If this interface is not supported, then return FALSE.

  @param[in, out]  Md5Context  Pointer to the MD5 context.
  @param[out]      HashValue   Pointer to a buffer that receives the MD5 digest
                               value (16 bytes).

  @retval TRUE   MD5 digest computation succeeded.
  @retval FALSE  MD5 digest computation failed.
  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
Md5Final (
  IN OUT  VOID   *Md5Context,
  OUT     UINT8  *HashValue
  ){
    MD5Final(HashValue, (struct MD5Context *)Md5Context);
    return TRUE;
}

/**
  Retrieves the size, in bytes, of the context buffer required for SHA-1 hash operations.

  @return  The size, in bytes, of the context buffer required for SHA-1 hash operations.

**/
UINTN
EFIAPI
Sha1GetContextSize(
    VOID)
{
    return sizeof(struct SHA1Context);
}


/**
  Initializes user-supplied memory pointed by Sha1Context as the SHA-1 hash context for
  subsequent use.

  If Sha1Context is NULL, then ASSERT().

  @param[in, out]  Sha1Context  Pointer to the SHA-1 Context being initialized.

  @retval TRUE  SHA-1 initialization succeeded.

**/
BOOLEAN
EFIAPI
Sha1Init(
    OUT  VOID  *Sha1Context)
{
    SHA1Init((struct SHA1Context*)Sha1Context);
    return TRUE;
}

/**
  Makes a copy of an existing SHA-1 context.

  If Sha1Context is NULL, then return FALSE.
  If NewSha1Context is NULL, then return FALSE.

  @param[in]  Sha1Context     Pointer to SHA-1 context being copied.
  @param[out] NewSha1Context  Pointer to new SHA-1 context.

  @retval TRUE   SHA-1 context copy succeeded.
  @retval FALSE  SHA-1 context copy failed.

**/
BOOLEAN
EFIAPI
Sha1Duplicate (
  IN   CONST VOID  *Sha1Context,
  OUT  VOID        *NewSha1Context
  )
{
  //
  // Check input parameters.
  //
  if (Sha1Context == NULL || NewSha1Context == NULL) {
    return FALSE;
  }

  memcpy (NewSha1Context, Sha1Context, sizeof (struct SHA1Context));

  return TRUE;
}

/**
  Performs SHA-1 digest on a data buffer of the specified length. This function can
  be called multiple times to compute the digest of long or discontinuous data streams.

  If Sha1Context is NULL, then ASSERT().

  @param[in, out]  Sha1Context  Pointer to the SHA-1 context.
  @param[in]       Data         Pointer to the buffer containing the data to be hashed.
  @param[in]       DataLength   Length of Data buffer in bytes.

  @retval TRUE   SHA-1 data digest succeeded.
  @retval FALSE  Invalid SHA-1 context. After Sha1Final function has been called, the
                 SHA-1 context cannot be reused.

**/
BOOLEAN
EFIAPI
Sha1Update(
  IN OUT  VOID        *Sha1Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataLength)
{
    SHA1Update((struct SHA1Context*)Sha1Context, (UINT8*)Data, (UINT32)DataLength);
    return TRUE;
}


/**
  Completes SHA-1 hash computation and retrieves the digest value into the specified
  memory. After this function has been called, the SHA-1 context cannot be used again.

  If Sha1Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Sha1Context  Pointer to the SHA-1 context
  @param[out]      HashValue    Pointer to a buffer that receives the SHA-1 digest
                                value (20 bytes).

  @retval TRUE  SHA-1 digest computation succeeded.

**/
BOOLEAN
EFIAPI
Sha1Final(
    IN OUT  VOID   *Sha1Context,
    OUT     UINT8  *HashValue)
{
    SHA1Final(HashValue, (struct SHA1Context *)Sha1Context);
    return TRUE;
}


/**
  Retrieves the size, in bytes, of the context buffer required for SHA-256 operations.

  @return  The size, in bytes, of the context buffer required for SHA-256 operations.

**/
UINTN
EFIAPI
Sha256GetContextSize(
    VOID)
{
    return sizeof(struct sha256_state);
}


/**
  Initializes user-supplied memory pointed by Sha256Context as SHA-256 hash context for
  subsequent use.

  If Sha256Context is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to SHA-256 Context being initialized.

  @retval TRUE  SHA-256 context initialization succeeded.

**/
BOOLEAN
EFIAPI
Sha256Init (
    IN OUT  VOID  *Sha256Context)
{
    sha256_init((struct sha256_state *)Sha256Context);
    return TRUE;
}

/**
  Makes a copy of an existing SHA-256 context.

  If Sha256Context is NULL, then return FALSE.
  If NewSha256Context is NULL, then return FALSE.

  @param[in]  Sha256Context     Pointer to SHA-256 context being copied.
  @param[out] NewSha256Context  Pointer to new SHA-256 context.

  @retval TRUE   SHA-256 context copy succeeded.
  @retval FALSE  SHA-256 context copy failed.

**/
BOOLEAN
EFIAPI
Sha256Duplicate (
  IN   CONST VOID  *Sha256Context,
  OUT  VOID        *NewSha256Context
  )
{
  //
  // Check input parameters.
  //
  if (Sha256Context == NULL || NewSha256Context == NULL) {
    return FALSE;
  }

  memcpy (NewSha256Context, Sha256Context, sizeof (struct sha256_state));

  return TRUE;
}
/**
  Performs SHA-256 digest on a data buffer of the specified length. This function can
  be called multiple times to compute the digest of long or discontinuous data streams.

  If Sha256Context is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to the SHA-256 context.
  @param[in]       Data           Pointer to the buffer containing the data to be hashed.
  @param[in]       DataLength     Length of Data buffer in bytes.

  @retval TRUE   SHA-256 data digest succeeded.
  @retval FALSE  Invalid SHA-256 context. After Sha256Final function has been called, the
                 SHA-256 context cannot be reused.

**/
BOOLEAN
EFIAPI
Sha256Update(
    IN OUT  VOID        *Sha256Context,
    IN      CONST VOID  *Data,
    IN      UINTN       DataLength)
{
    return sha256_process(
        (struct sha256_state *)Sha256Context, 
        (const unsigned char *)Data, 
        (unsigned long)DataLength) == 0?TRUE:FALSE;
}


/**
  Completes SHA-256 hash computation and retrieves the digest value into the specified
  memory. After this function has been called, the SHA-256 context cannot be used again.

  If Sha256Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Sha256Context  Pointer to SHA-256 context
  @param[out]      HashValue      Pointer to a buffer that receives the SHA-256 digest
                                  value (32 bytes).

  @retval TRUE  SHA-256 digest computation succeeded.

**/
BOOLEAN
EFIAPI
Sha256Final(
    IN OUT  VOID   *Sha256Context,
    OUT     UINT8  *HashValue)
{
    return sha256_done(
        (struct sha256_state *)Sha256Context, 
        HashValue) == 0 ? TRUE:FALSE;
}

/**
  Retrieves the size, in bytes, of the context buffer required for SHA-384 operations.

  @return  The size, in bytes, of the context buffer required for SHA-384 operations.

**/
UINTN
EFIAPI
Sha384GetContextSize(
    VOID)
{
    return sizeof(struct sha512_state);
}


/**
  Initializes user-supplied memory pointed by Sha384Context as SHA-384 hash context for
  subsequent use.

  If Sha384Context is NULL, then ASSERT().

  @param[in, out]  Sha384Context  Pointer to SHA-384 Context being initialized.

  @retval TRUE  SHA-384 context initialization succeeded.

**/
BOOLEAN
EFIAPI
Sha384Init (
    IN OUT  VOID  *Sha384Context)
{
    sha384_init((struct sha512_state *)Sha384Context);
    return TRUE;
}

/**
  Makes a copy of an existing SHA-384 context.

  If Sha384Context is NULL, then return FALSE.
  If NewSha384Context is NULL, then return FALSE.

  @param[in]  Sha384Context     Pointer to SHA-384 context being copied.
  @param[out] NewSha384Context  Pointer to new SHA-384 context.

  @retval TRUE   SHA-384 context copy succeeded.
  @retval FALSE  SHA-384 context copy failed.

**/
BOOLEAN
EFIAPI
Sha384Duplicate (
  IN   CONST VOID  *Sha384Context,
  OUT  VOID        *NewSha384Context
  )
{
  //
  // Check input parameters.
  //
  if (Sha384Context == NULL || NewSha384Context == NULL) {
    return FALSE;
  }

  memcpy (NewSha384Context, Sha384Context, sizeof (struct sha512_state));

  return TRUE;
}
/**
  Performs SHA-384 digest on a data buffer of the specified length. This function can
  be called multiple times to compute the digest of long or discontinuous data streams.

  If Sha384Context is NULL, then ASSERT().

  @param[in, out]  Sha384Context  Pointer to the SHA-384 context.
  @param[in]       Data           Pointer to the buffer containing the data to be hashed.
  @param[in]       DataLength     Length of Data buffer in bytes.

  @retval TRUE   SHA-384 data digest succeeded.
  @retval FALSE  Invalid SHA-384 context. After Sha384Final function has been called, the
                 SHA-384 context cannot be reused.

**/
BOOLEAN
EFIAPI
Sha384Update(
    IN OUT  VOID        *Sha384Context,
    IN      CONST VOID  *Data,
    IN      UINTN       DataLength)
{
    return sha512_process(
        (struct sha512_state *)Sha384Context, 
        (const unsigned char *)Data, 
        (unsigned long)DataLength) == 0?TRUE:FALSE;
}


/**
  Completes SHA-384 hash computation and retrieves the digest value into the specified
  memory. After this function has been called, the SHA-384 context cannot be used again.

  If Sha384Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Sha384Context  Pointer to SHA-384 context
  @param[out]      HashValue      Pointer to a buffer that receives the SHA-384 digest
                                  value (32 bytes).

  @retval TRUE  SHA-384 digest computation succeeded.

**/
BOOLEAN
EFIAPI
Sha384Final(
    IN OUT  VOID   *Sha384Context,
    OUT     UINT8  *HashValue)
{
    return sha384_done(
        (struct sha512_state *)Sha384Context, 
        HashValue) == 0 ? TRUE:FALSE;
}

/**
  Retrieves the size, in bytes, of the context buffer required for SHA-512 operations.

  @return  The size, in bytes, of the context buffer required for SHA-512 operations.

**/
UINTN
EFIAPI
Sha512GetContextSize(
    VOID)
{
    return sizeof(struct sha512_state);
}


/**
  Initializes user-supplied memory pointed by Sha512Context as SHA-512 hash context for
  subsequent use.

  If Sha512Context is NULL, then ASSERT().

  @param[in, out]  Sha512Context  Pointer to SHA-512 Context being initialized.

  @retval TRUE  SHA-512 context initialization succeeded.

**/
BOOLEAN
EFIAPI
Sha512Init (
    IN OUT  VOID  *Sha512Context)
{
    sha512_init((struct sha512_state *)Sha512Context);
    return TRUE;
}

/**
  Makes a copy of an existing SHA-512 context.

  If Sha512Context is NULL, then return FALSE.
  If NewSha512Context is NULL, then return FALSE.

  @param[in]  Sha512Context     Pointer to SHA-512 context being copied.
  @param[out] NewSha512Context  Pointer to new SHA-512 context.

  @retval TRUE   SHA-512 context copy succeeded.
  @retval FALSE  SHA-512 context copy failed.

**/
BOOLEAN
EFIAPI
Sha512Duplicate (
  IN   CONST VOID  *Sha512Context,
  OUT  VOID        *NewSha512Context
  )
{
  //
  // Check input parameters.
  //
  if (Sha512Context == NULL || NewSha512Context == NULL) {
    return FALSE;
  }

  memcpy (NewSha512Context, Sha512Context, sizeof (struct sha512_state));

  return TRUE;
}
/**
  Performs SHA-512 digest on a data buffer of the specified length. This function can
  be called multiple times to compute the digest of long or discontinuous data streams.

  If Sha512Context is NULL, then ASSERT().

  @param[in, out]  Sha512Context  Pointer to the SHA-512 context.
  @param[in]       Data           Pointer to the buffer containing the data to be hashed.
  @param[in]       DataLength     Length of Data buffer in bytes.

  @retval TRUE   SHA-512 data digest succeeded.
  @retval FALSE  Invalid SHA-512 context. After Sha512Final function has been called, the
                 SHA-512 context cannot be reused.

**/
BOOLEAN
EFIAPI
Sha512Update(
    IN OUT  VOID        *Sha512Context,
    IN      CONST VOID  *Data,
    IN      UINTN       DataLength)
{
    return sha512_process(
        (struct sha512_state *)Sha512Context, 
        (const unsigned char *)Data, 
        (unsigned long)DataLength) == 0?TRUE:FALSE;
}


/**
  Completes SHA-512 hash computation and retrieves the digest value into the specified
  memory. After this function has been called, the SHA-512 context cannot be used again.

  If Sha512Context is NULL, then ASSERT().
  If HashValue is NULL, then ASSERT().

  @param[in, out]  Sha512Context  Pointer to SHA-512 context
  @param[out]      HashValue      Pointer to a buffer that receives the SHA-512 digest
                                  value (32 bytes).

  @retval TRUE  SHA-512 digest computation succeeded.

**/
BOOLEAN
EFIAPI
Sha512Final(
    IN OUT  VOID   *Sha512Context,
    OUT     UINT8  *HashValue)
{
    return sha512_done(
        (struct sha512_state *)Sha512Context, 
        HashValue) == 0 ? TRUE:FALSE;
}

/**
  Allocates and Initializes one RSA Context for subsequent use.

  @return  Pointer to the RSA Context that has been initialized.
           If the allocations fails, RsaNew() returns NULL.

**/
VOID *
EFIAPI
RsaNew(
    VOID)
{
    struct crypto_rsa_key *RsaKey;

    ResetCRmm();

    RsaKey = os_malloc(sizeof(*RsaKey));
    if (RsaKey == NULL)
            return FALSE;

    return RsaKey;
}


/**
  Release the specified RSA Context.

  @param[in]  RsaContext  Pointer to the RSA context to be released.

**/
VOID
EFIAPI
RsaFree (
    IN  VOID  *RsaContext)
{  
// Security concern, memory heap is being cleared on exit 
    ResetCRmm();

    return;
}

/**
  Sets the tag-designated RSA key component into the established RSA context from
  the user-specified nonnegative integer (octet string format represented in RSA
  PKCS#1).

  If RsaContext is NULL, then ASSERT().

  @param[in, out]  RsaContext  Pointer to RSA context being set.
  @param[in]       KeyTag      Tag of RSA key component being set.
  @param[in]       BigNumber   Pointer to octet integer buffer.
  @param[in]       BnLength    Length of big number buffer in bytes.

  @return  TRUE   RSA key component was set successfully.
  @return  FALSE  Invalid RSA key component tag.

**/
BOOLEAN
EFIAPI
RsaSetKey(
    IN OUT VOID         *RsaContext,
    IN     RSA_KEY_TAG  KeyTag,
    IN     CONST UINT8  *BigNumber,
    IN     UINTN        BnLength)
{
    struct crypto_rsa_key *key;
    UINT8 *KeyNmod = NULL;

    if(RsaContext==NULL)
        return FALSE;

    key = (struct crypto_rsa_key *)RsaContext;

    if(KeyTag==RsaKeyN) {
// !!! NOTE !!!!
// KeyGen adds a leading ZERO if the msb of the first byte of the n-modulus is ONE. 
// This is to avoid this integer to be treated as negative value.
// If your calculations produce a number with the high bit set to 1, 
// just increase the length by another byte and pad the beginning with 0x00 
// to keep it positive.
        KeyNmod = (UINT8*)BigNumber;
        if(BigNumber[0] >> 7 == 1)
        {
            KeyNmod = os_malloc((unsigned int)(BnLength+1));
            if (KeyNmod == NULL)
                return FALSE;
            memcpy((void*)&KeyNmod[1], (void*)BigNumber, BnLength);
            KeyNmod[0] = 0;
            BnLength++;
        }
    
        key->n = bignum_init();
        if (key->n == NULL)
            goto error; 

//         * RSA2048PublicKey ::= 
//         *     modulus INTEGER, -- n

        if(bignum_set_unsigned_bin(key->n, KeyNmod, BnLength) < 0 )
            goto error;
    } else 
        if(KeyTag==RsaKeyE) {
            key->e = bignum_init();
            if (key->e == NULL)
                goto error; 
//         * RSA2048PublicKey ::= 
//         *     exponent INTEGER, -- e
            if(bignum_set_unsigned_bin(key->e, BigNumber, BnLength) < 0 )
                goto error;
        } else // unknown Key Tag
            return FALSE;

    return TRUE;

error:
    if(KeyNmod != NULL && KeyNmod != (u8*)BigNumber)
        os_free(KeyNmod);
    crypto_rsa_free(key);
    return FALSE;
}


/**
  Verifies the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in
  RSA PKCS#1.

  If RsaContext is NULL, then ASSERT().
  If MessageHash is NULL, then ASSERT().
  If Signature is NULL, then ASSERT().
  If HashLength is not equal to the size of MD5, SHA-1 or SHA-256 digest, then ASSERT().

  @param[in]  RsaContext   Pointer to RSA context for signature verification.
  @param[in]  MessageHash  Pointer to octet message hash to be checked.
  @param[in]  HashLength   Length of the message hash in bytes.
  @param[in]  Signature    Pointer to RSA PKCS1-v1_5 signature to be verified.
  @param[in]  SigLength    Length of signature in bytes.

  @return  TRUE   Valid signature encoded in PKCS1-v1_5.
  @return  FALSE  Invalid signature or invalid RSA context.

**/
BOOLEAN
EFIAPI
RsaPkcs1Verify(
    IN  VOID         *RsaContext,
    IN  CONST UINT8  *MessageHash,
    IN  UINTN        HashLength,
    IN  CONST UINT8  *Signature,
    IN  UINTN        SigLength)
{
    INTN        err;
    size_t     *sig_len;
    UINT8       DecriptedSig[DEFAULT_RSA_SIG_LEN];

    sig_len=(size_t*)&SigLength;

    if(RsaContext == NULL )
        return FALSE;

    err = crypto_rsa_exptmod((const UINT8*)Signature, *sig_len, (UINT8*)&DecriptedSig, sig_len, (struct crypto_rsa_key*)RsaContext, 0);

// locate Hash inside the decrypted signature body and compare it with given Hash;
// !!! only supporting PKCS#1 v1_5 Padding
    if(!err) {
//        err = pkcs_1_v1_5_decode(MessageHash, HashLength, (const UINT8 *)&DecriptedSig, (unsigned long)*sig_len);
        err = memcmp((void*)MessageHash, (void*)((UINTN)DecriptedSig + (UINTN)(*sig_len - HashLength)), HashLength);
    }

    return !err ? TRUE:FALSE;
}

//########################
#ifndef CONFIG_NO_INTERNAL_PEI_PKCS7
//########################
/**
  Verifies the validity of a PKCS#7 signed data as described in "PKCS #7:
  Cryptographic Message Syntax Standard". The input signed data could be wrapped
  in a ContentInfo structure.

  If P7Data, TrustedCert or InData is NULL, then return FALSE.
  If P7Length, CertLength or DataLength overflow, then return FAlSE.
  If this interface is not supported, then return FALSE.

  @param[in]  P7Data       Pointer to the PKCS#7 message to verify.
  @param[in]  P7Length     Length of the PKCS#7 message in bytes.
  @param[in]  TrustedCert  Pointer to a trusted/root certificate encoded in DER, which
                           is used for certificate chain verification.
  @param[in]  CertLength   Length of the trusted certificate in bytes.
  @param[in]  InData       1. Pointer to the content to be verified.
                           2. A pointer to a hash value (SHA1 or SHA256 supported) of the content to be verified.
  @param[in]  DataLength   Length of InData in bytes.

  @retval  TRUE  The specified PKCS#7 signed data is valid.
  @retval  FALSE Invalid PKCS#7 signed data.
  @retval  FALSE This interface is not supported.

**/
BOOLEAN
EFIAPI
Pkcs7Verify (
  IN  CONST UINT8  *P7Data,
  IN  UINTN        P7Length,
  IN  CONST UINT8  *TrustedCert,
  IN  UINTN        CertLength,
  IN  CONST UINT8  *InData,
  IN  UINTN        DataLength
  )
{
    UINT8       Hash[SHA256_DIGEST_SIZE];
    UINT8       HashAlgo;
    INTN        err, reason;
    struct pkcs7_signed_data_st* PKCS7cert;
    struct x509_certificate *x509SignCert=NULL;
    struct x509_certificate *x509TrustCert;
    
    err = 0;
    reason = 0;

    ResetCRmm();

    PKCS7cert = Pkcs7_parse_Authenticode_certificate(P7Data, P7Length);
    if (PKCS7cert) {
        // verify Pkcs7 Signing Cert chain up to the TrustCert
        if(TrustedCert && CertLength) {
            x509TrustCert = x509_certificate_parse(TrustedCert, CertLength);
            if(x509TrustCert)
                err = Pkcs7_x509_certificate_chain_validate(PKCS7cert, x509TrustCert, (int*)&reason);
             else 
                err = -1;
        } 
        //
        // For generic PKCS#7 handling, InData may be NULL if the content is present
        // in PKCS#7 structure. So ignore NULL checking here.
        //
        if(InData && DataLength > SHA256_DIGEST_SIZE) {
            err = Pkcs7_return_digestAlgorithm(PKCS7cert, (UINT8*)&HashAlgo);
            if(!err && HashAlgo == SHA1) {
                sha1_vector( 1, &InData, (const UINTN*)&DataLength, Hash);
                DataLength = SHA1_DIGEST_SIZE;
            } else if(!err && HashAlgo == SHA256) {
                sha256_vector( 1, &InData, (const UINTN*)&DataLength, Hash);
                DataLength = SHA256_DIGEST_SIZE;
            } else 
                err = -1;

            InData = (UINT8*)&Hash[0];
        }
        // x509SignCert== NULL -> extract SignCert from Pkcs7 crt
        if(!err)
            err = Pkcs7_certificate_validate_digest(PKCS7cert, x509SignCert, (UINT8*)InData, DataLength);
     } else 
         err = -1;
// Security concern, memory heap is being cleared on exit 
    ResetCRmm();

    return !err ? TRUE:FALSE;
}
/**
  Verifies the validity of a PE/COFF Authenticode Signature as described in "Windows
  Authenticode Portable Executable Signature Format".

  If AuthData is NULL, then return FALSE.
  If ImageHash is NULL, then return FALSE.
  If this interface is not supported, then return FALSE.

  @param[in]  AuthData     Pointer to the Authenticode Signature retrieved from signed
                           PE/COFF image to be verified.
  @param[in]  DataSize     Size of the Authenticode Signature in bytes.
  @param[in]  TrustedCert  Pointer to a trusted/root certificate encoded in DER, which
                           is used for certificate chain verification.
  @param[in]  CertSize     Size of the trusted certificate in bytes.
  @param[in]  ImageHash    Pointer to the original image file hash value. The produce
                           for calculating the image hash value is described in Authenticode
                           specification.
  @param[in]  HashSize     Size of Image hash value in bytes.

  @retval  TRUE   The specified Authenticode Signature is valid.
  @retval  FALSE  Invalid Authenticode Signature.
  @retval  FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
AuthenticodeVerify (
  IN  CONST UINT8  *AuthData,
  IN  UINTN        DataSize,
  IN  CONST UINT8  *TrustedCert,
  IN  UINTN        CertSize,
  IN  CONST UINT8  *ImageHash,
  IN  UINTN        HashSize
  )
{
//
// Verifies the PKCS#7 Signed Data in PE/COFF Authenticode Signature
//
    return (BOOLEAN) Pkcs7Verify (AuthData, DataSize, TrustedCert, CertSize, ImageHash, HashSize);
}

#endif // CONFIG_NO_INTERNAL_PEI_PKCS7
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
