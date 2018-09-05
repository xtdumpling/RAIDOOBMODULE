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
// Name: RsaKeyImport.c - Helper routines for PEI and DXE crypto library
//
// Description:     Implements following auxiliary functions:
//                  -crypto_import_rsa2048_public_key
//                  -crypto_import_asn1_public_key
//                  -ASN.1 DER base64_decode
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>
#include "includes.h"
#include "common.h"
#include "crypto.h"
#include "asn1.h"
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
//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  crypto_import_rsa2048_public_key
//
// Description:    Imports RSA2048 public key binary from RAW format into internal crypto_rsa_key data structure
//
// Input:
//  KeyN      - Pointer to N public modulus buffer
//  KeyE      - Pointer to E public exponent buffer
//  LenN      - Size of KeyN buffer (must be 256)
//  LenE      - Size of KeyE buffer
//
// Output:      Pointer to Key data structure crypto_rsa_key 
//            NULL if failed to import Key
//
//**********************************************************************
//<AMI_PHDR_END>

struct crypto_rsa_key *
crypto_import_rsa2048_public_key(const u8 *KeyN, size_t lenN, const u8 *KeyE, size_t lenE)
{
    struct crypto_rsa_key *key;
    u8 *KeyNmod;
    
    if(!KeyN || !KeyE)
        return NULL;
    
    KeyNmod = (u8*)KeyN;
// !!! NOTE !!!!
// KeyGen adds a leading ZERO if the msb of the first byte of the n-modulus is ONE.
// This is to avoid this integer to be treated as negative value.
// If your calculations produce a number with the high bit set to 1,
// just increase the length by another byte and pad the beginning with 0x00
// to keep it positive.
    if(KeyN[0] >> 7 == 1)
    {
        KeyNmod = os_malloc((UINTN)(lenN+1));
        if (KeyNmod == NULL)
            return NULL;
        memcpy(KeyNmod+1, (void*)KeyN, lenN);
        KeyNmod[0] = 0;
        lenN++;
    }

    key = os_malloc(sizeof(*key));
    if (key == NULL)
        return NULL;

    key->n = bignum_init();
    key->e = bignum_init();
    if (key->n == NULL || key->e == NULL) {
        goto error;
    }

//         * RSA2048PublicKey ::= 
//         *     modulus INTEGER, -- n

    if(bignum_set_unsigned_bin(key->n, KeyNmod, lenN) < 0 )
        goto error;
    if(bignum_set_unsigned_bin(key->e, KeyE, lenE) < 0 )
        goto error;

    return key;

error:
    if(KeyNmod != (u8*)KeyN)
        os_free(KeyNmod);
    crypto_rsa_free(key);
    return NULL;
}

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  crypto_public_key_import
//
// Description:    
//
// Input:
//
// Output:     
//
//**********************************************************************
//<AMI_PHDR_END>
struct crypto_public_key * crypto_public_key_import(const u8 *key, size_t len)
{
    return (struct crypto_public_key *)
        crypto_rsa_import_public_key(key, len);
}

void crypto_public_key_free(struct crypto_public_key *key)
{
    crypto_rsa_free((struct crypto_rsa_key *) key);
//    if (key) {
//        bignum_deinit(key->n);
//        bignum_deinit(key->e);
//    }
}

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  crypto_public_key_decrypt_pkcs1
//
// Description:    
//
// Input:
//
// Output:     
//
//**********************************************************************
//<AMI_PHDR_END>
int crypto_public_key_decrypt_pkcs1(struct crypto_public_key *key,
                    const u8 *crypt, size_t crypt_len,
                    u8 *plain, size_t *plain_len)
{
    size_t len;
    u8 *pos;

    len = *plain_len;
    if (crypto_rsa_exptmod(crypt, crypt_len, plain, &len,
                   (struct crypto_rsa_key *) key, 0) < 0)
        return -1;

    /*
     * PKCS #1 v1.5, 8.1:
     *
     * EB = 00 || BT || PS || 00 || D
     * BT = 00 or 01
     * PS = k-3-||D|| times (00 if BT=00) or (FF if BT=01)
     * k = length of modulus in octets
     */
    wpa_hexdump(MSG_MSGDUMP, "PKCS1: Sig Decrypted ", plain, len);

    if (len < 3 + 8 + 16 /* min hash len */ ||
        plain[0] != 0x00 || (plain[1] != 0x00 && plain[1] != 0x01)) {
        wpa_printf(MSG_INFO, "PKCS1: Invalid signature EB structure");
        return -1;
    }

    pos = plain + 3;
    if (plain[1] == 0x00) {
        /* BT = 00 */
        if (plain[2] != 0x00) {
            wpa_printf(MSG_INFO, "PKCS1: Invalid signature PS (BT=00)");
            return -1;
        }
        while (pos + 1 < plain + len && *pos == 0x00 && pos[1] == 0x00)
            pos++;
    } else {
        /* BT = 01 */
        if (plain[2] != 0xff) {
            wpa_printf(MSG_INFO, "PKCS1: Invalid signature PS (BT=01)");
            return -1;
        }
        while (pos < plain + len && *pos == 0xff)
            pos++;
    }

    if (pos - plain - 2 < 8) {
        /* PKCS #1 v1.5, 8.1: At least eight octets long PS */
        wpa_printf(MSG_INFO, "PKCS1: Too short signature padding");
        return -1;
    }

    if (pos + 16 /* min hash len */ >= plain + len || *pos != 0x00) {
        wpa_printf(MSG_INFO, "PKCS1: Invalid signature EB structure (2)");
        return -1;
    }
    pos++;
    len -= pos - plain;

    /* Strip PKCS #1 header */
    os_memmove(plain, pos, len);
    *plain_len = len;

    return 0;
}

#if 0
//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  crypto_x509_key_hdr_strip
//
// Description:    Strips the Key header in ASN.1 buffer leaving only standard Public Key structure
//
// Input:
//  buf      - Pointer to ASN.1 public key buffer
//  len      - Size of buffer
//
// Output:      Updated offset and length within Key buffer
//            -1 if failed to process the Key hdr
//
//**********************************************************************
//<AMI_PHDR_END>
static int
crypto_x509_key_hdr_strip(const u8 **buf, size_t *len)
{
    struct asn1_hdr hdr;
    const u8 *pos, *end;

/*
*  SubjectPublicKeyInfo  ::=  SEQUENCE  {
*       algorithm            AlgorithmIdentifier,
*       subjectPublicKey     BIT STRING }
*/
    if (asn1_get_next(*buf, *len, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
            //"PKCS #8: Does not start with PKCS #8 header (SEQUENCE); assume PKCS #8 not used");
            return -1;
    }
    pos = hdr.payload;
    end = pos + hdr.length;

    /* version Version (Version ::= INTEGER) */
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL || hdr.tag != ASN1_TAG_SEQUENCE) {
            return -1;
    }

    /* Description of the key parameters per PKCS #1,8 */
    pos = hdr.payload + hdr.length;
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL || hdr.tag != ASN1_TAG_BITSTRING) {
            return -1;
    }
    pos = hdr.payload + 1;
    end = pos + hdr.length;

    *buf = pos;
    *len = end - pos;

    return 0;
}

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  crypto_import_asn1_public_key
//
// Description:    Imports RSA2048 public key from ASN.1 DER format into 
//              internal crypto_rsa_key data structure
//
// Input:
//  key       - Pointer to public key buffer
//  len       - Size of key buffer
//
// Output:      Pointer to Key data structure crypto_rsa_key 
//            NULL if failed to import Key
//
//**********************************************************************
//<AMI_PHDR_END>
struct crypto_rsa_key * crypto_import_asn1_public_key(const u8 *key, size_t len)
{
    int res;
    /* First, check for possible X.509 public key encoding */
    res = crypto_x509_key_hdr_strip(&key, &len);

    /* Not PKCS#7, so try to import PKCS #1 encoded RSA public key */
    return (struct crypto_rsa_key *)
            crypto_rsa_import_public_key(key, len );

}
#endif

#if defined(ASN1_BASE64_DECODE) && ASN1_BASE64_DECODE == 1
//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  base64_decode
//
// Description:    Do in-place decoding of base-64 data of LENGTH in BUFFER.
//
// Input:
//  buffer       - Data to be decoded
//  length       - Length of the data to be decoded
//
// Output:      New length of the buffer bytes of decoded data,
//            or -1 n failure
//
//**********************************************************************
//<AMI_PHDR_END>
size_t
base64_decode (char *buffer, size_t length)
{
  static unsigned char const asctobin[128] = 
    {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
      0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
      0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
      0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
      0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
      0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff 
    };

  int idx = 0;
  unsigned char val = 0;
  int c = 0;
  char *d, *s;
  int lfseen = 1;

  /* Find BEGIN line.  */
  for (s=buffer; length; length--, s++)
    {
      if (lfseen && *s == '-' && length > 11 && !memcmp (s, "-----BEGIN ", 11))
        {
          for (; length && *s != '\n'; length--, s++)
            ;
          break;
        } 
      lfseen = (*s == '\n');
    }

  /* Decode until pad character or END line.  */
  for (d=buffer; length; length--, s++)
    {
      if (lfseen && *s == '-' && length > 9 && !memcmp (s, "-----END ", 9))
        break;
      if ((lfseen = (*s == '\n')) || *s == ' ' || *s == '\r' || *s == '\t')
        continue;
      if (*s == '=')
        {
          /* Pad character: stop */
          if (idx == 1)
            *d++ = val;
          break;
        }

      if ( (*s & 0x80) || (c = asctobin[*(unsigned char *)s]) == 0xff)
        return -1;

      switch (idx)
        {
        case 0:
          val = c << 2;
          break;
        case 1:
          val |= (c>>4)&3;
          *d++ = val;
          val = (c<<4)&0xf0;
          break;
        case 2:
          val |= (c>>2)&15;
          *d++ = val;
          val = (c<<6)&0xc0;
          break;
        case 3:
          val |= c&0x3f;
          *d++ = val;
          break;
        }
      idx = (idx+1) % 4;
    }

  return d - buffer;
}
#endif

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
