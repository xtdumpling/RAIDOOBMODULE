//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: PKCS_1_Sig_Decode.c - Helper routines for PEI and DXE crypto library
//
// Description:     Implements PKCS signature decoding
//                  -pkcs_1_v1_5_decode
//                  -pkcs_1_pss_decode
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>

/** 
  @file pkcs_1_sig_encode.c
  PKCS #1 PSS Signature Padding, Tom St Denis 
  +++ // A M I //
  PKCS #1 v1_5 SIG decode by AMI
*/
/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtomcrypt.com
 */
#include "includes.h"

#include "common.h"
#include "sha256.h"
#include "sha1.h"
#include "crypto.h"

#ifdef PKCS12_FUNCS
/* error codes [will be expanded in future releases] */
enum {
   CRYPT_OK=0,             /* Result OK */
   CRYPT_ERROR,            /* Generic Error */
   CRYPT_NOP,              /* Not a failure but no operation was performed */

   CRYPT_INVALID_KEYSIZE,  /* Invalid key size given */
   CRYPT_INVALID_ROUNDS,   /* Invalid number of rounds */
   CRYPT_FAIL_TESTVECTOR,  /* Algorithm failed test vectors */

   CRYPT_BUFFER_OVERFLOW,  /* Not enough space for output */
   CRYPT_INVALID_PACKET,   /* Invalid input packet given */

   CRYPT_INVALID_PRNGSIZE, /* Invalid number of bits for a PRNG */
   CRYPT_ERROR_READPRNG,   /* Could not read enough from PRNG */

   CRYPT_INVALID_CIPHER,   /* Invalid cipher specified */
   CRYPT_INVALID_HASH,     /* Invalid hash specified */
   CRYPT_INVALID_PRNG,     /* Invalid PRNG specified */

   CRYPT_MEM,              /* Out of memory */

   CRYPT_PK_TYPE_MISMATCH, /* Not equivalent types of PK keys */
   CRYPT_PK_NOT_PRIVATE,   /* Requires a private PK key */

   CRYPT_INVALID_ARG,      /* Generic invalid argument */
   CRYPT_FILE_NOTFOUND,    /* File Not Found */

   CRYPT_PK_INVALID_TYPE,  /* Invalid type of PK key */
   CRYPT_PK_INVALID_SYSTEM,/* Invalid PK system specified */
   CRYPT_PK_DUP,           /* Duplicate key already in key ring */
   CRYPT_PK_NOT_FOUND,     /* Key not found in keyring */
   CRYPT_PK_INVALID_SIZE,  /* Invalid size input for PK parameters */

   CRYPT_INVALID_PRIME_SIZE,/* Invalid size of prime requested */
   CRYPT_PK_INVALID_PADDING /* Invalid padding on input */
};
enum ltc_pkcs_1_v1_5_blocks
{
  LTC_PKCS_1_EMSA   = 1,        /* Block type 1 (PKCS #1 v1.5 signature padding) */
  LTC_PKCS_1_EME    = 2         /* Block type 2 (PKCS #1 v1.5 encryption padding) */
};

enum ltc_pkcs_1_paddings
{
  LTC_PKCS_1_V1_5   = 1,        /* PKCS #1 v1.5 padding (\sa ltc_pkcs_1_v1_5_blocks) */
  LTC_PKCS_1_OAEP   = 2,        /* PKCS #1 v2.0 encryption padding */
  LTC_PKCS_1_PSS    = 3         /* PKCS #1 v2.1 signature padding */
};

// Format of PSS Encoding
/*++
// --[-----------------------------------------------------
// --[ PSS signature padding scheme:
// --[ 
// --[ M - input message
// --[ 1. mHash = Hash (M)
// --[ 2. M = (0x)00 00 00 00 00 00 00 00 || mHash || salt;
// --[ 3. H = Hash (M)
// --[ 4. dbMask = MGF (H, emLen - hLen - 1).
// --[ 5. DB = PS || 0x01 || salt; PS - zeros till emLen - sLen - hLen - 2;DB is an octet string of length emLen - hLen - 1.
// --[ 6. maskedDB = DB xor dbMask
// --[ 7. EM = maskedDB || H || 0xbc.
// --[-----------------------------------------------------
--*/

unsigned long ghLen=SHA256_MAC_LEN;

/**
   Perform PKCS #1 MGF1 (internal)
   @param seed        The seed for MGF1
   @param seedlen     The length of the seed
   @param mask        [out] The destination
   @param masklen     The length of the mask desired
   @return CRYPT_OK if successful
*/
int pkcs_1_mgf1(      const unsigned char *seed, unsigned long seedlen,
                      unsigned char *mask, unsigned long masklen)
{
   unsigned long /*ghLen,*/ x;
    int       counter;
    int       err;
    unsigned char *buf;
    const u8 *addr[3];
    size_t len[3];
 
   /* get hash output size */
//   ghLen = SHA256_MAC_LEN;

   /* allocate memory */
    buf = (unsigned char *)os_malloc(ghLen);
    if (buf == NULL) {
        return CRYPT_MEM;
    }

   /* start counter */
    counter = 0;

    while (masklen > 0) {
       /* handle counter */
       WPA_PUT_BE32(buf, counter);
       ++counter;

        /* get hash of seed || counter */

        addr[0] = (u8 *) seed; len[0] = seedlen;
        addr[1] = (u8 *) buf; len[1] = 4;
        if(ghLen == SHA256_MAC_LEN)
            sha256_vector(2, addr, (const size_t*)&len, buf);
        else
            sha1_vector(2, addr, (const size_t*)&len, buf);

       /* store it */
        for (x = 0; x < ghLen && masklen > 0; x++, masklen--) {
            *mask++ = buf[x];
        }
    }

    err = CRYPT_OK;

    os_free(buf);

    return err;
}

/**
   PKCS #1 v2.00 PSS decode
   @param  msghash         The hash to verify
   @param  msghashlen      The length of the hash (octets)
   @param  sig             The signature data (encoded data)
   @param  siglen          The length of the signature data (octets)
   @param  saltlen         The length of the salt used (octets)
   @param  modulus_bitlen  The bit length of the RSA modulus
   @return CRYPT_OK if successful (even if the comparison failed)
*/
int pkcs_1_pss_decode(const unsigned char *msghash, unsigned long msghashlen,
                      const unsigned char *sig,     unsigned long siglen,
                            unsigned long saltlen,  unsigned long modulus_bitlen)
{
   unsigned char *DB, *mask, *salt, *hash;
   unsigned long x, y/*, ghLen*/, modulus_len;
   int           err;
   const u8 *addr[3];
   size_t len[3];

   ghLen        = msghashlen;
   modulus_len = (modulus_bitlen>>3) + ((modulus_bitlen & 7) ? 1 : 0);

//   DEFAULT_RSA_KEY_MODULUS_LEN
   /* check sizes */
   if ((saltlen > modulus_len) || 
       (modulus_len < ghLen + saltlen + 2) || (siglen != modulus_len)) {
      return CRYPT_PK_INVALID_SIZE;
   }

   /* allocate ram for DB/mask/salt/hash of size modulus_len */
   DB   = (unsigned char *)os_malloc(modulus_len);
   mask = (unsigned char *)os_malloc(modulus_len);
   salt = (unsigned char *)os_malloc(modulus_len);
   hash = (unsigned char *)os_malloc(modulus_len);

    if (DB == NULL || mask == NULL || salt == NULL || hash == NULL) {
      if (DB != NULL) {
         os_free(DB);
      }
      if (mask != NULL) {
         os_free(mask);
      }
      if (salt != NULL) {
         os_free(salt);
      }
      if (hash != NULL) {
         os_free(hash);
      }
      return CRYPT_MEM;
   }

   /* ensure the 0xBC byte */
   if (sig[siglen-1] != 0xBC) {
      err = CRYPT_INVALID_PACKET;
      goto LBL_ERR;
   }

   /* copy out the DB */
   x = 0;
   memcpy(DB, sig + x, modulus_len - ghLen - 1);
   x += modulus_len - ghLen - 1;

   /* copy out the hash */
   memcpy(hash, sig + x, ghLen);
   x += ghLen;

   /* check the MSB */
   if ((sig[0] & ~(0xFF >> ((modulus_len<<3) - (modulus_bitlen-1)))) != 0) {
      err = CRYPT_INVALID_PACKET;
      goto LBL_ERR;
   }

   /* generate mask of length modulus_len - ghLen - 1 from hash */
   if ((err = pkcs_1_mgf1(hash, ghLen, mask, modulus_len - ghLen - 1)) != 0) {
      goto LBL_ERR;
   }

   /* xor against DB */
   for (y = 0; y < (modulus_len - ghLen - 1); y++) {
      DB[y] ^= mask[y];
   }
   
   /* now clear the first byte [make sure smaller than modulus] */
   DB[0] &= 0xFF >> ((modulus_len<<3) - (modulus_bitlen-1));

   /* DB = PS || 0x01 || salt, PS == modulus_len - saltlen - ghLen - 2 zero bytes */

   /* check for zeroes and 0x01 */
   for (x = 0; x < modulus_len - saltlen - ghLen - 2; x++) {
       if (DB[x] != 0x00) {
          err = CRYPT_INVALID_PACKET;
          goto LBL_ERR;
       }
   }

   /* check for the 0x01 */
   if (DB[x++] != 0x01) {
      err = CRYPT_INVALID_PACKET;
      goto LBL_ERR;
   }

   /* M = (eight) 0x00 || msghash || salt, mask = H(M) */
    addr[0] = (u8 *) mask; len[0] = 8;
    addr[1] = (u8 *) msghash; len[1] = msghashlen;
    addr[2] = (u8 *) (DB+x); len[2] = saltlen;

    memset(mask, 0, 8);

    if(ghLen == SHA256_MAC_LEN)
        sha256_vector(3, addr, (const size_t*)len, mask);
    else
        sha1_vector(3, addr, (const size_t*)len, mask);

   /* mask == hash means valid signature */
   if (memcmp(mask, hash, ghLen) != 0) {
      err = CRYPT_ERROR;
      goto LBL_ERR;
   }

   err = CRYPT_OK;
LBL_ERR:

   os_free(hash);
   os_free(salt);
   os_free(mask);
   os_free(DB);

   return err;
}

//----------------------------------------------------------------------------
// Format of PKCS1_5 signature encoding

//// ASN.1 OID for SHA256 32byte hash
//#define DEFAULT_HASH_SHA256_ID_LEN 19 /* Fixed Object ID is  2.16.840.1.101.3.4.2.1 */ 
//#define DEFAULT_HASH_SHA256_ID {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x05, 0x00, 0x04, 0x00, 0x20}

//// ASN.1 OID for SHA1 32byte hash
//#define DEFAULT_HASH_SHA1_ID_LEN 15 /* Fixed */
//#define DEFAULT_HASH_SHA1_ID {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14}

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        PKCS1_5_SigType, PKCS1_5_SigTypePtr
//
// Description: PKCS1_5 signature. Used to define the Decoded Signature block in a C parsible format. 
//
// --[--------------------------------------------------------------------------
// --[ PKCS#1 v1.5 signature padding scheme:
// --[ 
// --[ EM = 
// --[ 0x00 || 0x01 ||           PS           || 0x00 || T =
// --[ 0x00 || 0x01 || 0xFF .. [202B] .. 0xFF || 0x00 || ASN.1 OID || Hash[32]
// --[--------------------------------------------------------------------------
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

//typedef struct _PKCS1_5SigType
//{
//   UINT8 ZeroByte1;                            // Fixed to 0x00
//   UINT8 OneByte;                              // Fixed to 0x01, EMSA block type
//   UINT8 FFBytes[202];                         // Fixed to 0xFF
//   UINT8 ZeroByte0;                            // Fixed to 0x00
//   UINT8 Sha256HashId[DEFAULT_HASH_SHA256_ID_LEN]; // Fixed ASN.1 OID SHA256 identifier
//   UINT8 Sha256Hash[DEFAULT_HASH_SHA256_LEN]; // SHA256 hash of capsule's payload
//} PKCS1_5_SigType;

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  pkcs_1_v1_5_decode
//
// Description:     Strip PKCS #1 header 
//
// Input:   Sig              The encoded data to decode
//          siglen          The length of the encoded data (octets)
//
// Output:      CRYPT_OK if successful
//
//**********************************************************************
//<AMI_PHDR_END>
int pkcs_1_v1_5_decode(const u8 *hash, size_t hashlen, const u8 *Sig, unsigned long siglen)
{
    u8 *pos;
    u8 pad;
    int err = -1;

    /*
     * PKCS #1 v1.5, 8.1:
     *
     * EB = 00 || BT || PS || 00 || D
     * BT = 01
     * PS = k-3-||D|| times FF
     * k = length of modulus in octets
     */

    if (siglen < 3 + 8 + 16 /* min hash siglen */ ||
        Sig[0] != 0x00 || 
      !(Sig[1] == LTC_PKCS_1_EMSA || Sig[1] == LTC_PKCS_1_EME) || 
        Sig[2] != 0xff) 
    {
        // Invalid signature EB 
        return err;
    }
    pos = Sig + 3;
    if(Sig[1] == LTC_PKCS_1_EMSA) {
        pad = 0xff;
    } else { // EME- random padding - skip till 0
        pad = 0x00;
    }
    while (pos < Sig + siglen && *pos == pad)
        pos++;

    if (pos - Sig - 2 < 8) {
        /* PKCS #1 v1.5, 8.1: At least eight octets long PS */
        // Too short signature 
        return err;
    }
    if (pos + 16 /* min hash siglen */ >= Sig + siglen || *pos != 0x00) {
        // Invalid signature EB structure (2)
        return err;
    }
    // skip HASH ASN.1 OID and check the hash
    if(hashlen == SHA256_MAC_LEN){
        /* AMI FIX for Bleichenbacher's RSA signature vulnerability*/
        if (pos + 20 + hashlen /* skip Hash oid */ != Sig + siglen)
            return err;
        err = (int)memcmp( pos + 20, (u8*)hash, hashlen);
    }
    else // SHA1
    {
        /* AMI FIX for Bleichenbacher's RSA signature vulnerability*/
        if (pos + 16 + hashlen /* skip Hash oid */ != Sig + siglen)
            return err;        
        err = (int)memcmp( pos + 16, (u8*)hash, hashlen );
    }

    return err;
}
#endif /* PKCS12_FUNCS */
