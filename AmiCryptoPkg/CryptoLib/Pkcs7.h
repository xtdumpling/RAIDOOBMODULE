//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
 PKCS#7 Authenticode SignedData parsing and processing
**/

#ifndef PKCS7_H
#define PKCS7_H

#include "asn1.h"

///
/// SignerInfo.authAttributes
///
struct pkcs7_signer_info_authAttr_st {
// authAttributes
    const u8 *auth_attrib_start;
    size_t auth_attrib_len;

    // messageDigest
    const u8 *messageDigest;
    size_t messageDigest_len;

    // Signing Time
    os_time_t signingTime;

    // signingCertificate.IssuerSerial
    //    certHash
    const u8 *certHash;
    size_t certHash_len;
    //    Issuer_and_serial#
    struct x509_name issuer;
    unsigned long serial_number;
};

///
/// SignerInfo
///
struct pkcs7_signer_info_st {
    unsigned long version;    // 1
// issuer_and_serial#
    struct x509_name issuer;
    unsigned long serial_number;
// digest_alg
    struct x509_algorithm_identifier digest_alg;
// authenticatedAttributes
    struct pkcs7_signer_info_authAttr_st authAttr; 
//encryptedDigest
    struct x509_algorithm_identifier digest_enc_alg;
    const u8 *enc_digest;
    size_t enc_digest_len;
};

///
/// TimeStampInfo = SignerInfo
///#define pkcs7_signer_info_Timestamp_st pkcs7_signer_info_st
///

///
/// Pkcs7 SignedData
///
struct pkcs7_signed_data_st {
// ContentInfo.Content
    const u8 *Content_start;    // der value start
    size_t Content_len;
// Digest_alg
    struct x509_algorithm_identifier Digest_alg;
    const u8 *Digest;        // file digest
    size_t Digest_len;
// tstInfo specific data
    struct pkcs7_signer_info_st *tstInfo;
// x.509 Cert store
    struct x509_certificate  *CertStore;
// SignerInfo
    struct pkcs7_signer_info_st *SignerInfo;
// TimeStamp Token RFC6131
    struct pkcs7_signed_data_st *tstSignedData;
    /*
     * The DER format certificate. These
     * pointers point to begin of corresponding cert in DER buffer.
     */
    const u8 *Pkcs7cert_start;
    size_t Pkcs7cert_len;
    const u8 *CAcert_start;
    size_t CAcert_len;
    const u8 *sign_cert_start;
    size_t sign_cert_len;
};

struct pkcs7_cert_revoke_info {
    u8           *ToBeSignedHash;
    size_t        ToBeSignedHashLen;
};

#ifdef CONFIG_INTERNAL_X509
// x509 helper routines
void x509_certificate_free(struct x509_certificate *cert);
int x509_parse_name(const u8 *buf, size_t len, struct x509_name *name,const u8 **next);
int x509_parse_time(const u8 *buf, size_t len, unsigned int/*u8*/ asn1_tag, os_time_t *val);
int x509_parse_algorithm_identifier(const u8 *buf, size_t len,
struct x509_algorithm_identifier *id, const u8 **next);
void x509_free_name(struct x509_name *name);
int x509_rsadsi_oid(struct asn1_oid *oid);
int x509_pkcs_oid(struct asn1_oid *oid);
int x509_digest_oid(struct asn1_oid *oid);
int x509_sha1_oid(struct asn1_oid *oid);
int x509_sha256_oid(struct asn1_oid *oid);
int x509_sha384_oid(struct asn1_oid *oid);
int x509_sha512_oid(struct asn1_oid *oid);
int x509_valid_issuer(const struct x509_certificate *cert);
// Pkcs7 
struct pkcs7_signed_data_st * Pkcs7_parse_Authenticode_certificate(const u8 *buf, size_t len);
struct x509_certificate * Pkcs7_return_signing_cerificate(struct pkcs7_signed_data_st *SignedData);
//////// obsolete
int Pkcs7_x509_certificate_chain_validate(struct pkcs7_signed_data_st *SignedData,
    struct x509_certificate *trusted, int *reason);
////////
int Pkcs7_certificate_validate_digest(struct pkcs7_signed_data_st *SignedData,
                                      struct x509_certificate *SignCert,
                                      u8 *fileHash, size_t hashLen);
int Pkcs7_return_cerificate_ptr(struct pkcs7_signed_data_st *SignedData,
                                u8** CAcert, size_t *CAcert_len,
                                u8** SignCert, size_t *SignCert_len);
int Pkcs7_return_digestAlgorithm(struct pkcs7_signed_data_st *PKCS7cert, u8* HashType);

int Pkcs7_x509_return_SubjectNameStr(u8 *pCert, size_t cert_len, 
                                     u8 *buf,   size_t len);

int Pkcs7_x509_return_cert_pubKey(struct x509_certificate *pCert,
                                  u8 **public_key, size_t *public_key_len);
    
int Pkcs7_x509_return_signing_Key(struct pkcs7_signed_data_st *SignedData,
                                  u8 **public_key, size_t *public_key_len);

int Pkcs7_x509_return_Cert_pubKey(u8 *pCert, size_t cert_len,
                                  u8 **public_key, size_t *public_key_len);

int Pkcs7_decrypt_validate_digest(const u8* enc_digest,
                                  size_t enc_digest_len,
                                  u8* public_key, 
                                  size_t public_key_len, 
                                  struct asn1_oid *digest_alg, 
                                  u8* testHash, size_t hashLen);
// TimeStamp
int Pkcs7_certificate_validate_timestamp_digest(
                    struct pkcs7_signed_data_st *SignedData,
                    os_time_t *TimeOfSign);

int Pkcs7_x509_certificate_chain_validate_with_timestamp(
                    struct pkcs7_signed_data_st *Pkcs7SignedData,
                    BOOLEAN bTimeStampToken,
                    struct x509_certificate *trusted,
                    struct pkcs7_cert_revoke_info *revokeInfo,
                    int *reason);

int Pkcs7_x509_signed_certificate_chain_validate(
                    struct pkcs7_signed_data_st *SignedData,
                    const  u8 *trusted,
                    int *reason);

int Pkcs7_x509_get_certificates_List(struct pkcs7_signed_data_st *SignedData, BOOLEAN bChained,
                    u8** CertList, size_t* ListLength);

#endif /* CONFIG_INTERNAL_X509 */

#endif /* PKCS7_H */
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
