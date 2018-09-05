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
//
// PKCS7 SignedData certificate parsing and processing (RFC 3215 profile)
//
//**********************************************************************

#include "includes.h"

#include "common.h"

#ifndef CONFIG_NO_INTERNAL_PEI_PKCS7

#include "crypto.h"
#include "sha256.h"
#include "sha1.h"
#include "x509v3.h"
#include "Pkcs7.h"

void pkcs7_signed_data_free(struct pkcs7_signed_data_st *cert);

/**
 * pkcs7_signed_data_Timestamp_free - Free an Pkcs7 Timestamp certificate chain
 * @cert: Certificate to be freed
 */
void pkcs7_signed_data_Timestamp_free(struct pkcs7_signed_data_st *cert)
{
    if (cert == NULL)
        return;
    pkcs7_signed_data_free(cert);
}

/**
 * pkcs7_signer_info_free - Free an Pkcs7 signer Info certificate
 * @cert: Certificate to be freed
 */
void pkcs7_signer_info_free(struct pkcs7_signer_info_st *cert)
{
    if (cert == NULL)
        return;
//    x509_free_name(&cert->issuer);
    os_free((void*)cert->authAttr.messageDigest);
    os_free((void*)cert->authAttr.auth_attrib_start);
    os_free((void*)cert->enc_digest);
    os_free(cert);
}

/**
 * pkcs7_signed_data_free - Free an Pkcs7 signed Data certificate
 * @cert: Certificate to be freed
 */
void pkcs7_signed_data_free(struct pkcs7_signed_data_st *cert)
{
    if (cert == NULL)
        return;
    pkcs7_signer_info_free(cert->tstInfo);
    pkcs7_signer_info_free(cert->SignerInfo); 
    pkcs7_signed_data_Timestamp_free(cert->tstSignedData);
    os_free((void*)cert->Digest);
    os_free(cert);
}

// 1.3.6.1.4.1.311
int Pkcs7_Microsoft_oid(struct asn1_oid *oid)
{
    return oid->len >= 7  &&
        oid->oid[0] == 1 /* iso */ &&
        oid->oid[1] == 3 /* identified-organization */ &&
        oid->oid[2] == 6 /* oiw */ &&
        oid->oid[3] == 1 /* secsig */ &&
        oid->oid[4] == 4 /* algorithms */ &&
        oid->oid[5] == 1;// &&
//        oid->oid[6] == 311;
}

/**
 *
 *    Data ::= OCTET STRING -- message
 *
*/
int Pkcs7_store_octet_string(u8 **buf, size_t len, u8 **out, size_t *outLen)
{
    struct asn1_hdr hdr;
    const u8 *pos, *end;

    int err = -1;

    if (!buf || !out || !outLen)
        return err;
    
    pos = *buf;
    end = pos + len;

// IDENDIFIER ::= OCTET STRING  
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_OCTETSTRING) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected OCTETSTRING "
               "- found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }
    if (hdr.length < 1) {
        
        return err;
    }
    pos = hdr.payload;
    os_free((void*)*out);
    *out = (u8*)os_malloc(hdr.length);
    if (*out) {

        os_memcpy((void*)*out, pos, hdr.length);
        *outLen = hdr.length;
        wpa_hexdump(MSG_MSGDUMP, "PKCS7: OCTET STRING",
                *out, hdr.length);

        *buf = (u8*)pos+hdr.length;

        return 0;
    }
    wpa_printf(MSG_DEBUG, "PKCS7: Failed to allocate memory for \
           Output buff");
    
    return err;
}

/**
 *  IssuerSerial ::= SEQUENCE {
 *       issuer                   GeneralNames,
 *       serialNumber             CertificateSerialNumber
 *  }
*/
int Pkcs7_parse_IssuerSerial_sequence(struct pkcs7_signer_info_st *cert, u8 **buf, size_t len)
{
    struct asn1_hdr hdr;
    const u8 *pos, *end;

    size_t left;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char sbuf[128];
#endif
    int err = -1;

    if (!buf || !cert)
        return err;
    
    pos = *buf;
    end = pos + len;

    /* issuer  GeneralNames */
    if (x509_parse_name(pos, end - pos, &cert->issuer, &pos))
    {
        
        return err;
    }
    x509_name_string(&cert->issuer, sbuf, sizeof(sbuf));
    wpa_printf(MSG_MSGDUMP, "PKCS7: issuer %a", sbuf);

    /* serialNumber CertificateSerialNumber ::= INTEGER */
    if (asn1_get_next(pos, end - pos, &hdr) < 0 || 
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_INTEGER) {
        wpa_printf(MSG_DEBUG, "PKCS7: No INTEGER tag found for "
               "serialNumber; Class=%d tag=0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }

    pos = hdr.payload;
    left = hdr.length;
    while (left) {
        cert->serial_number <<= 8;
        cert->serial_number |= *pos++;
        left--;
    }
    wpa_printf(MSG_MSGDUMP, "PKCS7: serialNumber %x", cert->serial_number);

    *buf = (u8*)pos;

    return 0;

}
/**
 * Pkcs7_signer_info_common_hdr_parse - Parse SignerInfo Issuer/Version Hdr
 * @buf: pointer to DER encoded value
 */
struct pkcs7_signer_info_st * Pkcs7_signer_info_common_hdr_parse(u8 **buf, size_t len, int *err)
{
    struct asn1_hdr hdr;
    struct pkcs7_signer_info_st *cert;
    const u8 *pos, *end, *next, *next_end;
    struct asn1_oid oid;
    unsigned char *p;//, ch; 

    size_t left;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char sbuf[128];
#endif
    unsigned long value;

    if(!err || buf==NULL)
        return NULL;

    *err = -1;

    pos = *buf;
    next_end = end = pos + len;

    cert = (struct pkcs7_signer_info_st *)os_zalloc(sizeof(*cert));
    if (cert == NULL)
        return cert;
    
    os_memset(cert, 0, sizeof(*cert));    


    /* RFC 3215 - PKCS#7 SignerInfo structure / ASN.1 DER */
    /* SignerInfo ::= SET::SEQUENCE */
    /* SEQUENCE */
    if (asn1_get_next(pos, len, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: SignerInfo did not start with "
               "a valid SEQUENCE - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return cert;
    }
    pos = hdr.payload;
    if (pos + hdr.length > end) {
        
        return cert;
    }
    end = pos + hdr.length;

    /*
     * version [0]  EXPLICIT Version DEFAULT v1
     * Version  ::=  INTEGER  {  v1(1)  }
     */
    if (asn1_get_next(pos, end - pos, &hdr) < 0)
    {
        
        return cert;
    }
    pos = hdr.payload;

    if (hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_INTEGER) {
        wpa_printf(MSG_DEBUG, "PKCS7: No INTEGER tag found for "
               "version field - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return cert;
    }
    if (hdr.length != 1) {
        wpa_printf(MSG_DEBUG, "PKCS7: Unexpected version field "
               "length %lx (expected 1)", hdr.length);
        
        return cert;
    }
    pos = hdr.payload;
    left = hdr.length;
    value = 0;
    while (left) {
        value <<= 8;
        value |= *pos++;
        left--;
    }

    cert->version = value;
    wpa_printf(MSG_MSGDUMP, "PKCS7: Version X.509v%d", cert->version + 1);

    if (cert->version != 1) {
        wpa_printf(MSG_DEBUG, "PKCS7: Unsupported version %d",
               cert->version + 1);
        
        return cert;
    }

     
    /* issuer Serial */
    /* SEQUENCE */
    if (asn1_get_next(pos, end - pos, &hdr) < 0)
    {
        
        return cert;
    }

    pos = hdr.payload;

    if(Pkcs7_parse_IssuerSerial_sequence(cert, (u8**)&pos, len))
        return  cert;

    end = next_end;

    /* DigestAlgorithm  */
    if (asn1_get_next(pos, end - pos, &hdr) < 0) {
        return cert;
    }
    /* DigestAlgorithm.AlgorithmIdentifier */
    if (x509_parse_algorithm_identifier(pos, end - pos,
                        &cert->digest_alg, &pos)) {
        return cert;
    }
////////////////
// start of AuthenticatedAttributes. May not exist in AuthVariable Certificates 
////////////////
    if (asn1_get_next(pos, end-pos, &hdr) < 0)
    {
         return cert;
    }
    // TAG[0]
    if(hdr.class == ASN1_CLASS_CONTEXT_SPECIFIC/* &&
       hdr.tag == 0*/)
    {

        //AuthAttrib digest_start
        cert->authAttr.auth_attrib_len = hdr.length + (hdr.payload - pos);

        os_free((void*)cert->authAttr.auth_attrib_start);
        cert->authAttr.auth_attrib_start = (u8*)os_malloc(cert->authAttr.auth_attrib_len);
        if (cert->authAttr.auth_attrib_start == NULL) {
            wpa_printf(MSG_DEBUG, "PKCS7: Failed to allocate memory for \
                   authAttrib digesting");
            
            return cert;
        }
        os_memcpy((void*)cert->authAttr.auth_attrib_start, pos, cert->authAttr.auth_attrib_len);
        //
        // Fixup for IMPLICIT tag: note this messes up for tags > 30,
        // but so did the old code. Tags > 30 are very rare anyway.
        // The trick is to replace IMPLICIT [0] TAG with EXPLICIT SET OF tag
        //
        p = (unsigned char*)cert->authAttr.auth_attrib_start;
    // start of TAG fix up
    // fix up to-> 0x31: TAG:SET_OF; CLASS:UNIVERSAL;
        *p = '1'; 
    // end TAG fix up

    // preserve ptr to next encryptedDigest section
        next = hdr.payload + hdr.length;    
        pos = hdr.payload;
        while(pos < next)
        {
            // Sequence
            if (asn1_get_next(pos, next - pos, &hdr) < 0 ||
                hdr.class != ASN1_CLASS_UNIVERSAL ||
                hdr.tag != ASN1_TAG_SEQUENCE) {
                wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
                       "(AttributeTypeAndValue) - found Class %d "
                       "tag 0x%x", hdr.class, hdr.tag);
                
                return cert;
            }
            pos = hdr.payload;
            next_end = end = pos + hdr.length;

            // OID    
            asn1_get_oid(pos, end-pos, &oid, &pos);

            // skip SET
            if (asn1_get_next(pos, end-pos, &hdr) < 0 ||
                hdr.class != ASN1_CLASS_UNIVERSAL ||
                hdr.tag != ASN1_TAG_SET) {
                wpa_printf(MSG_DEBUG, "PKCS7: Expected SET \
                       (messageDigest) - found Class %d tag 0x%x",
                       hdr.class, hdr.tag);
                
                return cert;
            }
            pos = hdr.payload;
            end = pos + hdr.length;
            asn1_oid_to_str(&oid, sbuf, sizeof(sbuf));
            wpa_printf(MSG_DEBUG, "PKCS7: Found OID=%a", sbuf);

        //    CounterType
        //        data
        /*
        6871   24:                     SEQUENCE {
        6873    9:                       OBJECT IDENTIFIER
                 :                         contentType (1 2 840 113549 1 9 3)
        6884   11:                       SET {
        6886    9:                         OBJECT IDENTIFIER data (1 2 840 113549 1 7 1)
                 :                         }
                 :                       }
        */
            if (x509_pkcs_oid(&oid) && oid.len    == 7 &&
                oid.oid[5] == 9 && oid.oid[6] == 3       // Content Type
            ) {
                wpa_printf(MSG_DEBUG, "PKCS7: == contentType");
                // IDENDIFIER ::= OID 
                if (asn1_get_oid(pos, end - pos, &oid, &pos)) {
                     wpa_printf(MSG_DEBUG, "PKCS7: OID Data Expected");

                    return cert;
                }
                asn1_oid_to_str(&oid, sbuf, sizeof(sbuf));
                 wpa_printf(MSG_DEBUG, "PKCS7: Data OID=%a", sbuf);
            // possible contentInfo types:
            // spcIndirectDataContext   1 3 6 1 4 1 311 2 1 4
            // Data                        1 2 840 113549 1 7 1
            // tSTInfo                    1 2 840 113549 1 9 16 1 4
                if (
                    (x509_pkcs_oid(&oid) && 
                     (oid.len == 7 && oid.oid[5] == 7 && oid.oid[6] == 1 ) || // Data
                     (oid.len == 9 && oid.oid[5] == 9 && oid.oid[6] == 16 && oid.oid[7] == 1 && oid.oid[8] == 4)// tSTInfo
                     ) || 
                    (Pkcs7_Microsoft_oid(&oid) && // spcIndirectDataContext
                     (oid.len == 10 && oid.oid[6] == 311 && oid.oid[7] == 2  && oid.oid[8] == 1 && oid.oid[9] == 4)
                     )
                    )
                {
                    pos = next_end;
                    continue;
                }
                wpa_printf(MSG_DEBUG, "PKCS7: Unsupported contentInfo OID");
                 return cert;

            }
        //    SigningTime
        //        UCTTime
        /*
        6897   28:                     SEQUENCE {
        6899    9:                       OBJECT IDENTIFIER
                 :                         signingTime (1 2 840 113549 1 9 5)
        6910   15:                       SET {
        6912   13:                         UTCTime 22/04/2009 05:54:34 GMT
                 :                         }
                 :                       }
        */
            if (x509_pkcs_oid(&oid) && oid.len == 7 &&
                oid.oid[5] == 9 && oid.oid[6] == 5    // signingTime Type
            ) {

                wpa_printf(MSG_DEBUG, "PKCS7: == signingTime");
            // IDENDIFIER ::= UTCTime  
                if (asn1_get_next(pos, end-pos, &hdr) < 0 ||
                    hdr.class != ASN1_CLASS_UNIVERSAL ||
                    x509_parse_time(hdr.payload, hdr.length, hdr.tag,
                            &cert->authAttr.signingTime) < 0) {
                    wpa_hexdump_ascii(MSG_DEBUG, "PKCS7: Failed to parse UTC"
                              "Time", hdr.payload, hdr.length);
                    return cert;
                }
                pos = next_end;
                continue;
            }
        //    MessageDigest
        //        OCTET_STRING
        /*
        6927   35:                     SEQUENCE {
        6929    9:                       OBJECT IDENTIFIER
                 :                         messageDigest (1 2 840 113549 1 9 4)
        6940   22:                       SET {
        6942   20:                         OCTET STRING  74 D7 54 A0 C0 9E 62 F0 DD 92 DC D6 96 34 A1 0B F0 D5 B5 3A
                 :                         }
                 :                       }
        */
            if (x509_pkcs_oid(&oid) &&    oid.len    == 7 &&
                oid.oid[5] == 9 && oid.oid[6] == 4  // MessageDigest OID
            ) {
                wpa_printf(MSG_DEBUG, "PKCS7: MessageDigest");
                if(Pkcs7_store_octet_string((u8**)&pos, end - pos, (u8**)&cert->authAttr.messageDigest, &cert->authAttr.messageDigest_len))
                    return cert;

                pos = next_end;
                continue;
            }
// find signingCertificate OID (1 2 840 113549 1 9 16 2 12)
/*
   ESSCertID ::=  SEQUENCE {
        certHash                 Hash,
        issuerSerial             IssuerSerial OPTIONAL
   }
   Hash ::= OCTET STRING -- SHAxxx hash of entire certificate

   IssuerSerial ::= SEQUENCE {
        issuer                   GeneralNames,
        serialNumber             CertificateSerialNumber
   }
*/
            if (x509_pkcs_oid(&oid) &&    oid.len == 9 &&
                oid.oid[5] == 9 && oid.oid[6] == 16 &&
                oid.oid[7] == 2 && oid.oid[8] == 12)  // signingCertificate OID
            {
                wpa_printf(MSG_DEBUG, "PKCS7: == signingCertificate");
/*
8017   11:                               OBJECT IDENTIFIER
         :                                 signingCertificate (1 2 840 113549 1 9 16 2 12)
8030  201:                               SET {
8033  198:                                 SEQUENCE {
8036  195:                                   SEQUENCE {
8039  168:                                     SEQUENCE {
8042   20:                                       OCTET STRING
         :                   6D A7 F9 6A A2 C6 15 DD 58 E9 A6 AF 7B 48 CE 29
         :                   A1 D1 27 C0
8064  143:                                       SEQUENCE {
8067  128:                                         SEQUENCE {
8070  126:                                           [4] {
*/
                // skip SEQ/SEQ/SEQ
                pos = hdr.payload;
                end = pos + hdr.length;
            // Sequence1
                if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
                    hdr.class != ASN1_CLASS_UNIVERSAL ||
                    hdr.tag != ASN1_TAG_SEQUENCE) {
                    return cert;
                }
                pos = hdr.payload;
                end = pos + hdr.length;
            // Sequence2
                if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
                    hdr.class != ASN1_CLASS_UNIVERSAL ||
                    hdr.tag != ASN1_TAG_SEQUENCE) {

                    return cert;
                }
                pos = hdr.payload;
                end = pos + hdr.length;
            // Sequence3
                if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
                    hdr.class != ASN1_CLASS_UNIVERSAL ||
                    hdr.tag != ASN1_TAG_SEQUENCE) {

                    return cert;
                }
                pos = hdr.payload;
                end = pos + hdr.length;

                // Get certHash OCTET STRING
/*
   When creating an ESSCertID, the certHash is computed over the entire
   DER encoded certificate including the signature. The issuerSerial
   would normally be present unless the value can be inferred from other
   information.
*/
                wpa_printf(MSG_DEBUG, "PKCS7:  ESScertID.certHash");
                if(Pkcs7_store_octet_string((u8**)&pos, end - pos, (u8**)&cert->authAttr.certHash, &cert->authAttr.certHash_len))
                    return cert;

                // skip SEQ/SEQ/TAG[4]
            // Sequence1
                if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
                    hdr.class != ASN1_CLASS_UNIVERSAL ||
                    hdr.tag != ASN1_TAG_SEQUENCE) {

                    return cert;
                }
                pos = hdr.payload;
                end = pos + hdr.length;
            // Sequence2
                if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
                    hdr.class != ASN1_CLASS_UNIVERSAL ||
                    hdr.tag != ASN1_TAG_SEQUENCE) {

                    return cert;
                }
                pos = hdr.payload;
                end = pos + hdr.length;
                if (asn1_get_next(pos, end-pos, &hdr) < 0 ||
                    hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC ||
                     hdr.tag != 4)
                {
                    wpa_printf(MSG_DEBUG, "PKCS7: Expected TAG[4]"
                           "- found Class %d "
                           "tag 0x%x", hdr.class, hdr.tag);
                    return cert;
                }
                pos = hdr.payload;
                end = pos + hdr.length;
                // override Signer in signerInfo. OPTIONAL
                wpa_printf(MSG_DEBUG, "PKCS7:  ESScertID.IssuerSerial");
                if(Pkcs7_parse_IssuerSerial_sequence((struct pkcs7_signer_info_st*)cert, (u8**)&pos, len)) {
//                    return  cert;
                    wpa_printf(MSG_DEBUG, "PKCS7: Optional IssuerSerial"
                           "- not found");
                }

                pos = next_end;
                continue;
            }

            pos = next_end;
        } // end while

        if(!cert->authAttr.messageDigest) {
            wpa_printf(MSG_DEBUG, "PKCS7: Missing required messageDigest entry in authenticatedAttrib collection");
            
            return cert;
        }

    } else {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected TAG[0] for authenticatedAttributes "
               "with a valid SEQUENCE - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
    }
// end of authenticatedAttributes
    end = (u8*)*buf+len;

    /* digestEncryptedAlgo */
    /*
    6964   13:                   SEQUENCE {
    6966    9:                     OBJECT IDENTIFIER
             :                       sha1withRSAEncryption (1 2 840 113549 1 1 5)
    6977    0:                     NULL
             :                     }
    */
    if (x509_parse_algorithm_identifier(pos, end - pos,
                        &cert->digest_enc_alg, &pos))
    {
        return cert;
    }
    /* encryptedDigest */
    /*
    6979  256:                   OCTET STRING
    */
    wpa_printf(MSG_MSGDUMP, "PKCS7: Encrypted Digest");
    if(Pkcs7_store_octet_string((u8**)&pos, end - pos, (u8**)&cert->enc_digest, &cert->enc_digest_len))
        return cert;

    *err = 0;

    *buf = (u8*)pos;

    return cert;
}

/**
 * Pkcs7_RFC6131_TimeStampToken_parse - Parse SignerInfo Timestamp value
 * @buf: pointer to DER encoded value
 */
int Pkcs7_RFC6131_TimeStampToken_parse( const u8 *buf, size_t len,
                    struct pkcs7_signed_data_st *SignedData)
{
    struct pkcs7_signed_data_st *tstSignedData;
/*
    add tSTInfo struct to Pkcs7_struct
    parse Pkcs#7
    get Content_start, len
    parse Content_start.TimeStamp
    fill in TimeStamp fields
*/
    wpa_printf(MSG_DEBUG, "PKCS7: Found RFC6131 TimeStamp, len=%x",len);

    // 
    if(len < 256) {
        return -1;
    }

    tstSignedData = Pkcs7_parse_Authenticode_certificate(buf, len);
    if(tstSignedData==NULL)
        return -1;

    SignedData->tstSignedData = tstSignedData;
// bringing up the tstTime info
    SignedData->tstInfo = tstSignedData->tstInfo;
    
    return 0;
}

/**
 * pkcs7_signer_info_st - Parse SignerInfo Timestamp value
 * @buf: pointer to DER encoded value
 */
int Pkcs7_TimeStamp_parse(const u8 *buf, size_t len,
                    struct pkcs7_signed_data_st *SignedData)
{
    struct asn1_hdr hdr;
    const u8 *pos, *end;
    struct asn1_oid oid;

#ifndef CONFIG_NO_STDOUT_DEBUG
    char sbuf[128];
#endif
    int err = -1;

    BOOLEAN bTimeStamp2 = FALSE; // RFC6131 TimeStamp

    wpa_printf(MSG_DEBUG, "PKCS7: Parse SignerInfo.TimeStamp, len=%x", len);

    // 
    if(len < 256) {
        return err;
    }

    pos = buf;
    end = buf + len;

    /*
    6694  541:           [1] {
    6698  537:             SEQUENCE {
    */
    /* Timestamp ::= TAG[1] */
    // [1] TAG
    if (asn1_get_next(buf, len, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC ||
        hdr.tag != 1) {
        wpa_printf(MSG_DEBUG, "PKCS7: Timestamp did not start "
               "with a valid SEQUENCE - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }

    pos = hdr.payload;
    end = pos + hdr.length;

    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AttributeTypeAndValue) - found Class %d "
               "tag 0x%x", hdr.class, hdr.tag);
        return err;
    }
    pos = hdr.payload;

/* Authenticode Timestamp ia a PKCS #9 v1 countersignature / ASN.1 DER */
/*
    6702    9:               OBJECT IDENTIFIER countersignature (1 2 840 113549 1 9 6)
    6713  522:               SET {
*/
//or
/* or MSFT timestampResponce PKCS# 7 Cert RFC 3161  */
/*
    3625   10:               OBJECT IDENTIFIER '1 3 6 1 4 1 311 3 3 1'
    3637 4868:               SET {
*/

    asn1_get_oid(pos, end-pos, &oid, &pos);
    asn1_oid_to_str(&oid, sbuf, sizeof(sbuf));
    wpa_printf(MSG_DEBUG, "PKCS7: Found OID=%a", sbuf);
    if (Pkcs7_Microsoft_oid(&oid) &&
        oid.len == 10 && 
        oid.oid[6] == 311 && oid.oid[7] == 3 && oid.oid[8] == 3 && oid.oid[9] == 1 ) 
    {
        bTimeStamp2 = TRUE;
    }
    else
        return err;    // unknown format

    /*
    6713  522:               SET {
    */
    if (asn1_get_next(pos, end-pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SET) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SET \
               (messageDigest) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        return err;
    }
    pos = hdr.payload;

    if(bTimeStamp2)
        err = Pkcs7_RFC6131_TimeStampToken_parse(pos, hdr.length, SignedData);

    return err;

}

/**
 * Pkcs7_signer_info_parse - Parse SignerInfo value
 * @buf: pointer to DER encoded value
 */
int Pkcs7_signer_info_parse(const u8 *buf, size_t len, struct pkcs7_signed_data_st *SignedData)
{
//    struct asn1_hdr hdr;
    const u8 *pos, *end, *next;
    struct pkcs7_signer_info_st *cert;
//    struct asn1_oid oid;
#ifndef CONFIG_NO_STDOUT_DEBUG
//    char sbuf[128];
#endif
//    unsigned char *p, ch; 
    int    err= -1;

    pos = buf;
    end = buf + len;

    /* RFC 3215 - PKCS#7 SignerInfo structure / ASN.1 DER */
    /* SignerInfo ::= SET::SEQUENCE */
    /* SET */
    /* SEQUENCE */
    cert = Pkcs7_signer_info_common_hdr_parse((u8**)&pos, len, &err);
    if(err) {
//        os_free(cert);
        return err;
    }
    SignedData->SignerInfo = cert;

// may find a SignerInfo.TimeStamp
    next = pos;
    Pkcs7_TimeStamp_parse(next, end - next, SignedData);

    return 0;
}

/**
 * Pkcs7_parse_Authenticode_contentInfo - Parse Authenticode signedData.contenType value
 * @buf: pointer to DER encoded value
 */
int Pkcs7_parse_Authenticode_contentInfo(const u8 *buf, size_t len, struct pkcs7_signed_data_st *SignedData)
{
    struct asn1_hdr hdr;
    const u8 *pos, *end;
    const u8 *seq_pos, *seq_end;
    struct asn1_oid oid;

#ifndef CONFIG_NO_STDOUT_DEBUG
    char sbuf[128];
#endif
    int     err = -1;

    pos = seq_pos = buf;
    end = seq_end = buf + len;

    // ContentInfo.content
    // !!! may be missing in UEFI 2.3.1 AuthVariable PKCS7 certificate format
    // !!! skip till Certificates
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected ContentInfo.content:ASN1_CLASS_CONTEXT_SPECIFIC "
               "(AttributeTypeAndValue) - found Class %d "
               "tag 0x%x", hdr.class, hdr.tag);

        return 0; // OK
    }
    pos = hdr.payload;
    //SpcIndirectDataContent
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AttributeTypeAndValue) - found Class %d "
               "tag 0x%x", hdr.class, hdr.tag);

        return err;
    }
    pos = hdr.payload;
    end = hdr.payload + hdr.length;

    SignedData->Content_start = (u8*)hdr.payload; // ContentInfo OCTET STRING
    SignedData->Content_len = hdr.length;

    if (asn1_get_next(pos, end - seq_pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AttributeTypeAndValue) - found Class %d "
               "tag 0x%x", hdr.class, hdr.tag);

        return err;
    }
    /*
    pos = hdr.payload;
    end = hdr.payload + hdr.length;    
    //     AlgOID - Authenticode Microsoft OIDs 1 3 6 1 4 1 311 2 1 15 SPC_PE_IMAGE_DATA_OBJID    
    if (asn1_get_oid(pos, end - pos, &oid, &pos)) {

        return err;
    }
    // next - string 
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AttributeTypeAndValue) - found Class %d "
               "tag 0x%x", hdr.class, hdr.tag);

        return err;
    }

    */
    // skip // "Obsolete" string
    pos = hdr.payload + hdr.length;
    // next Sha1 hash. Need this
    if (asn1_get_next(pos, seq_end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AlgorithmIdentifier) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);

        return err;
    }
    pos = hdr.payload;
    end = hdr.payload + hdr.length;

    // preserve the next Tag pointer
    /*
     * X.509:
     * AlgorithmIdentifier ::= SEQUENCE {
     *     algorithm            OBJECT IDENTIFIER,
     *     parameters           ANY DEFINED BY algorithm OPTIONAL
     * }
     */
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AlgorithmIdentifier) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);

        return err;
    }
    seq_pos = hdr.payload + hdr.length;
    // SHA OID
    if (asn1_get_oid(hdr.payload, hdr.length, &oid, &end)) {
        wpa_printf(MSG_DEBUG, "PKCS7: Failed to parse digestAlgorithm");

        return err;
    }

    if (x509_sha1_oid(&oid) || 
        x509_sha256_oid(&oid)||
        x509_sha384_oid(&oid)||
        x509_sha512_oid(&oid)
        ) 
    {
        pos = seq_pos;
        end = seq_end;
        /* Digest ::= OCTET STRING */
        wpa_printf(MSG_DEBUG, "PKCS7:  Decrypted Digest ");
        if(Pkcs7_store_octet_string((u8**)&pos, end - pos, (u8**)&SignedData->Digest, &SignedData->Digest_len))
        {
            return err;
        }
/*
        if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
            hdr.class != ASN1_CLASS_UNIVERSAL ||
            hdr.tag != ASN1_TAG_OCTETSTRING) {
            wpa_printf(MSG_DEBUG, "PKCS7: Expected OCTETSTRING "
                   "(Digest) - found Class %d tag 0x%x",
                   hdr.class, hdr.tag);
            
            return err;
        }
        wpa_hexdump(MSG_MSGDUMP, "PKCS7: Decrypted Digest",
                hdr.payload, hdr.length);

    
//        pos = hdr.payload;
//        os_free(SignedData->Digest);
//        SignedData->Digest = (u8*)os_malloc(hdr.length);
//        if (SignedData->Digest == NULL) {
//            wpa_printf(MSG_DEBUG, "PKCS7: Failed to allocate memory for \
//                   ContentInfo.Digest");
//            // pkcs7_signed_data_free(SignedData);
//            return NULL;
//        }
//        os_memcpy(SignedData->Digest, (u8*)hdr.payload, hdr.length);
//        SignedData->Digest_len = hdr.length;
//        wpa_hexdump(MSG_MSGDUMP, "PKCS7: file Digest:",
//                SignedData->Digest, hdr.length);

        SignedData->Digest = (u8*)hdr.payload; // SHA1 hash
    // 
        SignedData->Digest_len = hdr.length;
        pos = hdr.payload + hdr.length;
*///////////////////////////////////////////////////////
    }
    else {
        wpa_printf(MSG_DEBUG, "PKCS7: digestAlgorithm SHA1/SHA256/SHA384/SHA512 "
               "expected");
        asn1_oid_to_str(&oid, sbuf, sizeof(sbuf));

        return err;
    }


    return 0;
}

/**
 * Pkcs7_parse_contentInfo_tstInfo - Parse MS tstInfo signedData.contenType value
 * @buf: pointer to DER encoded value
 */

int Pkcs7_parse_contentInfo_tstInfo(const u8 *buf, size_t len, struct pkcs7_signed_data_st *SignedData)
{
    struct asn1_hdr hdr;
    const u8 *pos, *end;
    const u8 *seq_end;
    size_t left;
    unsigned long value;
    struct asn1_oid oid;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char sbuf[128];
#endif
    int    err = -1;
    struct pkcs7_signer_info_st *cert;

    wpa_printf(MSG_DEBUG, "PKCS7: Parse ContentInfo.tstInfo");

    if(len < 256) { // can be smaller if GeneralName is short....
        return err;
    }

    // allocate TimeStamp cert
    cert = (struct pkcs7_signer_info_st *)os_zalloc(sizeof(*cert));
    if (cert == NULL)
        return err;

    os_memset(cert, 0, sizeof(*cert));    


/*
TSTInfo ::= SEQUENCE  {
   version                      INTEGER  { v1(1) },
   policy                       TSAPolicyId,
   messageImprint               MessageImprint,
     -- MUST have the same value as the similar field in
     -- TimeStampReq
   serialNumber                 INTEGER,
    -- Time-Stamping users MUST be ready to accommodate integers
    -- up to 160 bits.
   genTime                      GeneralizedTime,
   accuracy                     Accuracy                 OPTIONAL,
   ordering                     BOOLEAN             DEFAULT FALSE,
   nonce                        INTEGER                  OPTIONAL,
     -- MUST be present if the similar field was present
     -- in TimeStampReq.  In that case it MUST have the same value.
   tsa                          [0] GeneralName          OPTIONAL,
   extensions                   [1] IMPLICIT Extensions   OPTIONAL  }
*/

    pos = buf;
    end = seq_end = buf + len;

    // [0] TAG
    // begin of AuthenticatedAttributes. 
    if (asn1_get_next(pos, end-pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC ||
        hdr.tag != 0) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected TAG[0] "
               "with a valid SEQUENCE - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }

    pos = hdr.payload;
    /* eContent ::= OCTET STRING */
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_OCTETSTRING) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected OCTETSTRING "
               "(Digest) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }
    pos = hdr.payload;
    end = pos + hdr.length;

// A digest of RFC3161 TimeStamp certificate TSTInfo content must be validated against 
// the digest stored inside of TimeStamp.SignerInfo.authAttributes.messageDigest
    SignedData->Content_start = (u8*)hdr.payload; // ContentInfo OCTET STRING
    SignedData->Content_len = hdr.length;
/*
TSTInfo ::= SEQUENCE  {
*/

    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AlgorithmIdentifier) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        return err;
    }
    pos = hdr.payload;
    end = pos + hdr.length;
/*
   version                      INTEGER  { v1(1) },
*/
    /*
     * version [0]  EXPLICIT Version DEFAULT v1
     * Version  ::=  INTEGER  {  v1(1)  }
     */
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_INTEGER) {
        wpa_printf(MSG_DEBUG, "PKCS7: No INTEGER tag found for "
               "version field - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }
    if (hdr.length != 1) {
        wpa_printf(MSG_DEBUG, "PKCS7: Unexpected version field "
               "length %lx (expected 1)", hdr.length);
        
        return err;
    }
    pos = hdr.payload;
    left = hdr.length;
    value = 0;
    while (left) {
        value <<= 8;
        value |= *pos++;
        left--;
    }

    cert->version = value;
    wpa_printf(MSG_MSGDUMP, "PKCS7: Version X.509v%d", cert->version + 1);

    if (value != 1) {
        wpa_printf(MSG_DEBUG, "PKCS7: Unsupported version %d != 1",
               value);
        
        return err;
    }

/*
   policy                       TSAPolicyId,
    OBJECT IDENTIFIER '1 3 6 1 4 1 601 10 3 1' ///////TSAPolicyId
*/
    if (asn1_get_oid(pos, end-pos, &oid, &pos)) {
        wpa_printf(MSG_DEBUG, "PKCS7: Failed to parse TSAPolicyId");

        return err;
    }
    asn1_oid_to_str(&oid, sbuf, sizeof(sbuf));
     wpa_printf(MSG_DEBUG, "PKCS7: TSAPolicyOID =%a", sbuf);

    if (!Pkcs7_Microsoft_oid(&oid) ||
    oid.len != 10 || 
    oid.oid[6] != 601 || oid.oid[7] != 10 || oid.oid[8] != 3 || oid.oid[9] != 1 ) 
    {
        wpa_printf(MSG_DEBUG, "PKCS7: TSAPolicyId OID is Expected");
         return err;
    }
/*
    messageImprint               MessageImprint,
    Digest Algo+Hash
*/
    // next Sha hash. Need this
    if (asn1_get_next(pos, seq_end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AlgorithmIdentifier) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);

        return err;
    }
    pos = hdr.payload;
    end = hdr.payload + hdr.length;
    /*
     * X.509:
     * AlgorithmIdentifier ::= SEQUENCE {
     *     algorithm            OBJECT IDENTIFIER,
     *     parameters           ANY DEFINED BY algorithm OPTIONAL
     * }
     */
    /* DigestAlgorithm.AlgorithmIdentifier */
    if (x509_parse_algorithm_identifier(pos, end - pos,
                        &cert->digest_alg, &pos)) {
        return err;
    }

    if (x509_sha1_oid(&cert->digest_alg.oid) || 
        x509_sha256_oid(&cert->digest_alg.oid) ||
        x509_sha384_oid(&cert->digest_alg.oid) ||
        x509_sha512_oid(&cert->digest_alg.oid)
        ) 
    {
        /* Digest ::= OCTET STRING */
        wpa_printf(MSG_DEBUG, "PKCS7:  hashedMessage");
        if(Pkcs7_store_octet_string((u8**)&pos, end - pos, (u8**)&cert->authAttr.messageDigest, &cert->authAttr.messageDigest_len))
        {
            return err;
        }
/*
        if (asn1_get_next(pos, seq_end - pos, &hdr) < 0 ||
            hdr.class != ASN1_CLASS_UNIVERSAL ||
            hdr.tag != ASN1_TAG_OCTETSTRING) {
            wpa_printf(MSG_DEBUG, "PKCS7: Expected OCTETSTRING "
                   "(Digest) - found Class %d tag 0x%x",
                   hdr.class, hdr.tag);
            
            return err;
        }
        wpa_hexdump(MSG_MSGDUMP, "PKCS7: hashedMessage",
                hdr.payload, hdr.length);

        cert->authAttr.messageDigest = (u8*)hdr.payload; // SHA hash
    // 
        cert->authAttr.messageDigest_len = hdr.length;
        pos = hdr.payload + hdr.length;
*/
    }
    else {
        wpa_printf(MSG_DEBUG, "PKCS7: hashAlgorithm SHA1/SHA256/SHA384/SHA512 "
               "expected");
        asn1_oid_to_str(&oid, sbuf, sizeof(sbuf));

        return err;
    }

/*
   serialNumber                 INTEGER,
*/
    /* serialNumber CertificateSerialNumber ::= INTEGER */
    if (asn1_get_next(pos, seq_end - pos, &hdr) < 0 || 
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_INTEGER) {
        wpa_printf(MSG_DEBUG, "PKCS7: No INTEGER tag found for "
               "serialNumber; Class=%d tag=0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }

    pos = hdr.payload;
    left = hdr.length;
    while (left) {
        cert->serial_number <<= 8;
        cert->serial_number |= *pos++;
        left--;
    }
    wpa_printf(MSG_MSGDUMP, "PKCS7: serialNumber %x", cert->serial_number);

/*
   genTime                      GeneralizedTime,
*/
    if (hdr.payload +  hdr.length > buf + len)
        return err;

    pos = hdr.payload + hdr.length;
    if (asn1_get_next(pos, seq_end-pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        x509_parse_time(hdr.payload, hdr.length, hdr.tag,
                &cert->authAttr.signingTime) < 0) {
        wpa_hexdump_ascii(MSG_DEBUG, "PKCS7: Failed to parse GeneralizedTime"
                  "Time", hdr.payload, hdr.length);
        return err;
    }
    wpa_printf(MSG_MSGDUMP, "PKCS7: GeneralizedTime: %x",
           (unsigned long) cert->authAttr.signingTime);

/*
   accuracy                     Accuracy                 OPTIONAL,
   ordering                     BOOLEAN             DEFAULT FALSE,
   nonce                        INTEGER                  OPTIONAL,
*/
    pos = hdr.payload + hdr.length;
    if (asn1_get_next(pos, seq_end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AlgorithmIdentifier) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);

        return err;
    }
// skip TAG[0]
//    pos = hdr.payload;
    pos = hdr.payload + hdr.length;
    if (asn1_get_next(pos, seq_end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC ||
        hdr.tag != 0) {
        wpa_printf(MSG_DEBUG, "PKCS7: tstInfo struct expecting TAG[0] "
               " - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }
// skip TAG[4]
    pos = hdr.payload;
    if (asn1_get_next(pos, seq_end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC ||
        hdr.tag != 4) {
        wpa_printf(MSG_DEBUG, "PKCS7: tstInfo struct expecting TAG[4] "
               " - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        
        return err;
    }
/*
   tsa                          [0] GeneralName          OPTIONAL,
*/
    pos = hdr.payload;
//    pos = hdr.payload + hdr.length;
    if (x509_parse_name(pos, seq_end - pos, &cert->issuer, &pos))
        return err;
    x509_name_string(&cert->issuer, sbuf, sizeof(sbuf));
    wpa_printf(MSG_MSGDUMP, "PKCS7: issuer %a", sbuf);

    SignedData->tstInfo = cert;

    return 0;
}

struct pkcs7_signed_data_st * Pkcs7_parse_Authenticode_certificate(const u8 *buf, size_t len)
{
    struct asn1_hdr hdr;
    struct pkcs7_signed_data_st *SignedData;
    struct x509_certificate  *cert, *cert_priv=NULL, *cert_first=NULL;
    const u8 *pos, *end;
    const u8 *set_pos, *set_end, *seq_pos, *seq_end;
    struct asn1_oid oid;
    size_t left;
    unsigned long value;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char sbuf[128];
#endif
    BOOLEAN bAuthenticodeSignedData = FALSE;
    int    err = -1;

    SignedData = (struct pkcs7_signed_data_st *)os_zalloc(sizeof(*SignedData));
    if (SignedData == NULL)
        return NULL;
    os_memset(SignedData, 0, sizeof(*SignedData));
//    os_memcpy(SignedData + 1, buf, len);
    SignedData->Pkcs7cert_start = buf;//(u8 *) (SignedData + 1);
    SignedData->Pkcs7cert_len = len;
/////

// 1. ID the SignedData sequence
//   0 30  510: SEQUENCE {
//   4 06    9:   OBJECT IDENTIFIER signedData (1 2 840 113549 1 7 2)
//  15 A0  495:   [0] {
//  19 30  491:     SEQUENCE {
//  23 02    1:       INTEGER 1
//  26 31   15:       SET {
//  28 30   13:         SEQUENCE {
//  30 06    9:           OBJECT IDENTIFIER
//            :             sha1withRSAEncryption (1 3 14 3 2 26) // SHA1
//  41 05    0:           NULL
//            :           }
//            :         }
//  43 30   69:       SEQUENCE {
//                OBJECTIDENTIFIER = { 1 3 6 1 4 1 311 2 1 4 }   Microsoft Windows Authenticode - SPC_INDIRECT_DATA_OBJID
//                CONTEXT_0 {
//                    SEQUENCE {
//                        SEQUENCE {
//                            OBJECTIDENTIFIER = { 1 3 6 1 4 1 311 2 1 15 }    Microsoft Windows Authenticode - SPC_PE_IMAGE_DATA_OBJID
//                             SEQUENCE {
//                                BIT_STRING =                                 
//                                CONTEXT_0 {
//                                    CONTEXT_2 {
//                                        CONTEXT_0 = 003C003C003C004F00620073006F006C006500740065003E003E003E     // O b s o l e t e
//                                    }
//                                }
//                            }
//                        }
//                        SEQUENCE {
//                            SEQUENCE {
//                                OBJECTIDENTIFIER = { 1 3 14 3 2 26 }    // SHA1                           
//                                NULL =                                 
//                            }
//                            OCTET_STRING = 44FD4FA917EEACCF1F0BE3A14D5BA6618297C4B6                            
//                        }
//                    }
 //               }
//            }

// collect all TBS certificates io the cert chain
// while ()
    // x508_certificate_parse();
// }
// SignedData
//
//    pos = SignedData->Pkcs7cert_start;//buf;
//    end = SignedData->Pkcs7cert_start/*buf*/ + len;
    pos = buf;
    end = buf + len;


    /* RFC 2315 - PKCS#7 v1.5 certificate / ASN.1 DER */

    /* SignedFile Certificate ::= SEQUENCE */
    if (asn1_get_next(pos, len, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Certificate did not start with "
               "a valid SEQUENCE - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }
    pos = hdr.payload;

    if (pos + hdr.length > end) {
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }

    if (pos + hdr.length < end) {
        wpa_hexdump(MSG_MSGDUMP, "PKCS7: Ignoring extra data after DER "
                "encoded certificate",
                pos + hdr.length, end - (pos + hdr.length));
        end = pos + hdr.length;
    }

    // Assume 
    /* SignedData ::= OID */
    //1 2 840 113549 1 7 2
    asn1_get_oid(pos, end-pos, &oid, &pos);
    if (x509_pkcs_oid(&oid) && oid.len == 7 &&
        oid.oid[5] == 7 /* PKCS7*/ &&
        oid.oid[6] == 2 /* SignedData*/) {
        wpa_printf(MSG_DEBUG, "PKCS7: Detected Authenticode SignedData signature "
               "algorithm");

        bAuthenticodeSignedData = TRUE;

        if (asn1_get_next(pos, end-pos, &hdr) < 0 ||
            hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC)
        {
            // pkcs7_signed_data_free(SignedData);
            return NULL;
        }
        pos = hdr.payload;
        end = pos + hdr.length;
        if (asn1_get_next(pos, end - pos, &hdr) < 0){
            // pkcs7_signed_data_free(SignedData);
            return NULL;
        }

    // Sequence
        if (hdr.class != ASN1_CLASS_UNIVERSAL ||
            hdr.tag != ASN1_TAG_SEQUENCE) {
            wpa_printf(MSG_DEBUG, "PKCS7: No SEQUENCE tag found for "
                   "version field - found Class %d tag 0x%x",
                   hdr.class, hdr.tag);
            // pkcs7_signed_data_free(SignedData);
            return NULL;
        }
        pos = hdr.payload;
        end = pos + hdr.length;
    } // Authenticode hdr
// else it can be AuthVariable Data
/*
   0 1171: SEQUENCE {
   4    1:   INTEGER 1
   7   11:   SET {
   9    9:     SEQUENCE {
  11    5:       OBJECT IDENTIFIER sha1 (1 3 14 3 2 26)
  18    0:       NULL
         :       }
         :     }
  20   11:   SEQUENCE {
  22    9:     OBJECT IDENTIFIER data (1 2 840 113549 1 7 1)
         :     }
  33  791:   [0] {
x509 certificates
SignerInfo
noAuthAttributes
Digest
*/
    if (asn1_get_next(pos, end - pos, &hdr) < 0) {
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }

    if (hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_INTEGER) {
         wpa_printf(MSG_DEBUG, "PKCS7: No INTEGER tag found for "
               "version field - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }
// Integer -> Version = 1
// Version = 1 is spcIndirectDataContext 
// Version = 3 is tstInfo 

    pos = hdr.payload;
    left = hdr.length;
    value = 0;
    while (left) {
        value <<= 8;
        value |= *pos++;
        left--;
    }

// Set ->
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SET) {
         wpa_printf(MSG_DEBUG, "PKCS7: Expected SET "
                  "(RelativeDistinguishedName) - found class "
               "%d tag 0x%x", hdr.class, hdr.tag);
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }

    set_pos = hdr.payload;
    pos = set_end = hdr.payload + hdr.length;

// Sequence
    if (asn1_get_next(set_pos, set_end - set_pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(AttributeTypeAndValue) - found Class %d "
               "tag 0x%x", hdr.class, hdr.tag);
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }
    seq_pos = hdr.payload;
    seq_end = hdr.payload + hdr.length;

//     AlgOID - SHA1/SHA256
/* DigestAlgorithm.AlgorithmIdentifier */ 
    if (asn1_get_oid(seq_pos, seq_end - seq_pos, &SignedData->Digest_alg.oid, &seq_pos)) {
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }
    asn1_oid_to_str(&SignedData->Digest_alg.oid, sbuf, sizeof(sbuf));
     wpa_printf(MSG_DEBUG, "PKCS7: Found OID=%a", sbuf);
    if (!x509_sha1_oid(&SignedData->Digest_alg.oid) &&
         !x509_sha256_oid(&SignedData->Digest_alg.oid) &&
         !x509_sha384_oid(&SignedData->Digest_alg.oid) &&
         !x509_sha512_oid(&SignedData->Digest_alg.oid)
    ){
         wpa_printf(MSG_DEBUG, "PKCS7: digestAlgorithm SHA1/SHA256/SHA384/SHA512 "
                  "expected");
         // pkcs7_signed_data_free(SignedData);
         return NULL;
     }

    // [NULL]
    if (asn1_get_next(seq_pos, seq_end - seq_pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_NULL)  {
        wpa_printf(MSG_DEBUG, "PKCS7: Failed to parse "
               "AttributeValue");
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }

    // ContentInfo.contentType == Data || SpcIndirectData || tstInfo
    pos = hdr.payload;
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: ContentInfo.contentType."
               "Expected SEQUENCE  (AttributeTypeAndValue) - found Class %d "
               "tag 0x%x", hdr.class, hdr.tag);
        // UEFI AuthVariable    
        if(bAuthenticodeSignedData == TRUE) {
               // pkcs7_signed_data_free(SignedData);
            return NULL;

        }
    }
    seq_pos = hdr.payload;
    seq_end = hdr.payload + hdr.length;

    /* IDENDIFIER ::= OID */
    if (asn1_get_oid(seq_pos, seq_end - seq_pos, &oid, &pos)) {
         wpa_printf(MSG_DEBUG, "PKCS7: ContentInfo.contentType OID Expected");
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }
    asn1_oid_to_str(&oid, sbuf, sizeof(sbuf));
    wpa_printf(MSG_DEBUG, "PKCS7: ContentInfo.contentType OID=%a", sbuf);

    seq_pos = pos;
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// ContentInfo.ContenTypes in Deviation from Authenticode:
// 1. Authenticode Microsoft OIDs SPC_INDIRECT_DATA_OBJID  { 1 3 6 1 4 1 311 2 1 4 }  
// 2. UEFI AuthVariable: OID Data        1 2 840 113549 1 7 1 
// 3. MS TimeStamp info: OID tSTInfo    1 2 840 113549 1 9 16 1 4
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// AlgOID - Authenticode Microsoft OIDs ContentInfo.contentType == 1 2 840 113549 1 7 1 Data
// This could be AuthVariable Data
    if (x509_pkcs_oid(&oid) &&  oid.len == 7 &&
        oid.oid[5] == 7 /* PKCS7*/ &&
        oid.oid[6] == 1 /* Data*/) {

        wpa_printf(MSG_DEBUG, "PKCS7: OID = Data");

        if (value != 1) {
            wpa_printf(MSG_DEBUG, "PKCS7: Unsupported signedData version %d != 1",
                   value);
            // pkcs7_signed_data_free(SignedData);
            return NULL;
        }

        if(!x509_sha256_oid(&SignedData->Digest_alg.oid)) { 
            asn1_oid_to_str(&SignedData->Digest_alg.oid, sbuf, sizeof(sbuf));
            wpa_printf(MSG_DEBUG, "Warning: UEFI AuthVariable SignedData: only SHA256 digestAlgorithm is accepted"
            "OID=%a", sbuf);
// Msft AuthVAr BUG: UEFI 2.3.1 page223 : Only a digest algorithm of SHA-256 is accepted.
//             // pkcs7_signed_data_free(SignedData);
//          return NULL;
        }
//        pos = seq_end;    
    } else 
        if (Pkcs7_Microsoft_oid(&oid) && oid.len == 10 &&
            oid.oid[6] == 311 &&
            oid.oid[7] == 2 && 
            oid.oid[8] == 1 && 
            oid.oid[9] == 4 ) {

            wpa_printf(MSG_DEBUG, "PKCS7: OID = spcIndirectData");

            if (value != 1) {
                wpa_printf(MSG_DEBUG, "PKCS7: Unsupported Authenticode signedData version %d != 1",
                       value);
                // pkcs7_signed_data_free(SignedData);
                return NULL;
            }
    
        // 1. Authenticode Microsoft OIDs SPC_INDIRECT_DATA_OBJID  { 1 3 6 1 4 1 311 2 1 4 }  
            err = Pkcs7_parse_Authenticode_contentInfo(seq_pos, seq_end - seq_pos, SignedData);
            if (err) {
                wpa_printf(MSG_DEBUG, "PKCS7: Error spcIndirectData parser",
                       value);
                // pkcs7_signed_data_free(SignedData);
                return NULL;
            }

        } // SignedData.spcIndirectData OID
        else // /* tstInfo*/
            if (x509_pkcs_oid(&oid) &&  oid.len == 9 &&
                oid.oid[5] == 9  && /* pkcs-9 */
                oid.oid[6] == 16 && /* smime */
                oid.oid[7] == 1  && /* ct */
                oid.oid[8] == 4) {
// process tstInfo
                wpa_printf(MSG_DEBUG, "PKCS7: OID = tstInfo");
                if (value != 3) {
                    wpa_printf(MSG_DEBUG, "PKCS7: Unsupported Authenticode tstInfo signedData version %d != 3",
                           value);
                    // pkcs7_signed_data_free(SignedData);
                    return NULL;
                }

                err = Pkcs7_parse_contentInfo_tstInfo(seq_pos, seq_end - seq_pos, SignedData);
                if (err) {
                    wpa_printf(MSG_DEBUG, "PKCS7: Error contentInfo.tstInfo parser",
                           value);
                    // pkcs7_signed_data_free(SignedData);
                    return NULL;
                }
            }
            else
            {
                wpa_printf(MSG_DEBUG, "PKCS7: Expecting OIDs = "
                    "1.2.840.113549.1.7.1 Data"
                    "1.3.6.1.4.1.311.2.1.4 spcIndirectDataContext"
                    "1.2.840.113549.1.9.16.1.4 TimeStamp info");
        //         // pkcs7_signed_data_free(SignedData);
                 return NULL;
            }

    pos = seq_end;
    //    end = SignedData->Pkcs7cert_start + len;
    end = buf + len;

    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_CONTEXT_SPECIFIC) {
        wpa_printf(MSG_DEBUG, "PKCS7: Failed to parse "
               "AttributeValue");
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }

    pos = hdr.payload;
    seq_end = hdr.payload + hdr.length;
// here goes a loop to collect all certificates
    do{
//     get the cert size
    /* RFC 3280 - X.509 v3 certificate / ASN.1 DER */
    /* Certificate ::= SEQUENCE */
        if (asn1_get_next(pos, end - pos, &hdr) < 0) {
            wpa_printf(MSG_DEBUG, "PKCS7: Failed to parse "
                   "AttributeValue");
            // pkcs7_signed_data_free(SignedData);
            return NULL;
        }
//TAG [1] ??? skip until we figured out if this is needed
        if(hdr.class == ASN1_CLASS_CONTEXT_SPECIFIC && 
           hdr.tag == ASN1_TAG_BOOLEAN) 
        {  
            pos = end; 
            end = seq_end;
            continue;
        }
        if (hdr.class != ASN1_CLASS_UNIVERSAL ||
            hdr.tag != ASN1_TAG_SEQUENCE) {
            wpa_printf(MSG_DEBUG, "PKCS7: Certificate did not start with "
                   "a valid SEQUENCE - found Class %d tag 0x%x",
                   hdr.class, hdr.tag);
            // pkcs7_signed_data_free(SignedData);
            return NULL;
        }
        end = hdr.payload + hdr.length;

        cert = x509_certificate_parse(pos, /*hdr.length+10*/end-pos);

        if(cert == NULL){
            // pkcs7_signed_data_free(SignedData);
            return NULL;
        }

        if(cert_first == NULL)
            cert_first = cert;
        else {
            if(cert_priv) // dummy test to avoid CPP check error
                cert_priv->next = cert;
        }

        cert_priv = cert;

        pos = end; 
        end = seq_end;
    } while (pos < seq_end);

    SignedData->CertStore = cert_first;

/* RFC 2315 - PKCS #7 Signer Info structure / ASN.1 DER */
//    end = SignedData->Pkcs7cert_start + len;
    end = buf + len;
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
            hdr.class != ASN1_CLASS_UNIVERSAL ||
            hdr.tag != ASN1_TAG_SET) { 
         wpa_printf(MSG_DEBUG, "PKCS7: Expecting signerInfos."
                  "Failed to parse AttributeValue.");
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }
    pos = hdr.payload;
    err = Pkcs7_signer_info_parse(pos, hdr.length, SignedData);
    if(err){
        // pkcs7_signed_data_free(SignedData);
        return NULL;
    }

    return SignedData;
}

/**
 * PKCS7_return_signing_cerificate_common - Return certificate with signing public key
 * SignedData: List of trusted certificates
 * SignerInfo: Lookup name and serial number. Maybe SignerInfo or a TimeStamp
 * Returns: cert if chain is valid, NULL if not found
 */
struct x509_certificate * Pkcs7_return_signing_cerificate(struct pkcs7_signed_data_st *SignedData)
{
    long unsigned idx;
    struct x509_certificate *cert;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char buf[128];
#endif
    struct pkcs7_signer_info_st *SignerInfo;

    if(SignedData==NULL) {
        return NULL;
    }

    SignerInfo = SignedData->SignerInfo;
    x509_name_string(&SignerInfo->issuer, buf, sizeof(buf)); 
    wpa_printf(MSG_DEBUG, "PKCS7: Looking for the Signing certificate...");
    wpa_printf(MSG_DEBUG, "PKCS7: Issuer: %a", buf);

    cert = SignedData->CertStore;
    
    for (idx = 0; cert; cert = cert->next, idx++) {
         x509_name_string(&cert->issuer, buf, sizeof(buf));
         wpa_printf(MSG_DEBUG, "PKCS7: %x: %a", idx, buf);

// Compare Name and Serial Number
        if (x509_name_compare(&cert->issuer,
              &SignerInfo->issuer) != 0) {
            wpa_printf(MSG_DEBUG, "PKCS7: Cert chain issuer name mismatch...");
                continue;
        } else {
            if (cert->serial_number != SignerInfo->serial_number) {
                   wpa_printf(MSG_DEBUG, "PKCS7: Cert chain serial number mismatch...");
                    continue;
            }
            break; // found
        }
    }
    // cert == NULL if end was reached

    if (cert==NULL) {
        wpa_printf(MSG_DEBUG, "PKCS7: Did not find certificate "
               "matching Signer Info parameters");
            return cert;
    }

    wpa_printf(MSG_DEBUG, "PKCS7: Signing cert found!");

    return cert;
}

/**
 * Pkcs7_x509_certificate_chain_validate - Validate X.509 certificate chain
 * @chain: Certificate chain to be validated (first chain must be issued by
 * signed by the second certificate in the chain and so on)
 * @trusted: List of trusted certificates
 *            if trust certificate not defined = NULL, 
 *            just update Sign and CA certificate Ptrs 
 * @reason: Buffer for returning failure reason (X509_VALIDATE_*)
 * Returns: 0 if chain is valid, -1 if not
 */
int Pkcs7_x509_certificate_chain_validate(
                    struct pkcs7_signed_data_st *SignedData,
                    struct x509_certificate *trusted,
                    int *reason)
{
    return Pkcs7_x509_certificate_chain_validate_with_timestamp(SignedData, FALSE, trusted, NULL, reason);
}

/**
 * Pkcs7_x509_certificate_chain_validate_with_timestamp - Validate X.509 certificate chain
 * Certificate chain to be validated (first chain must be issued by signed by the second 
 * certificate in the chain and so on)
 * bTimeStampToken: Control switch 
 *             TRUE : Process Pkcs7->TimeStamp signing branch only
 *             FALSE: Process full Pkcs7 x509 Cert chain
 * @trusted: List of trusted certificates
 *            if trust certificate not defined = NULL, 
 *            just update Sign and CA certificate Ptrs 
 * @revokeInfo: special TimeStamp revocation certificate 
 *             including Cert Hash and TimeofRevocation            
 * @reason: Buffer for returning failure reason (X509_VALIDATE_*)
 * Returns: 0 if chain is valid, -1 if not
 */
int Pkcs7_x509_certificate_chain_validate_with_timestamp(
                    struct pkcs7_signed_data_st *Pkcs7SignedData,
                    BOOLEAN bTimeStampToken,
                    struct x509_certificate *trusted,
                    struct pkcs7_cert_revoke_info *revokeInfo,
                    int *reason)
{
    long unsigned idx;
    int chain_trusted = 0, chain_valid;
    struct pkcs7_signed_data_st *SignedData;
    struct x509_certificate *cert, *signer, *leaf, *trust;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char buf[128];
#endif
    
    struct os_time now;
    u8     hashType = 0;
    size_t hashLen;
    u8     hash[SHA512_DIGEST_SIZE];
    int    err = -1;

    wpa_printf(MSG_DEBUG, "PKCS7: Validate TimeStamped Certificate chain");

    if(reason==NULL) {
        return err;
    }

    if(Pkcs7SignedData==NULL) {
        *reason = X509_VALIDATE_CERTIFICATE_UNKNOWN;
        return err;
    }

// if both not NULL
    if(trusted && revokeInfo)
        return err;

    if(revokeInfo && revokeInfo->ToBeSignedHash==NULL)
        return err;

    if(bTimeStampToken) {
        if(!Pkcs7SignedData->tstSignedData) // TimeStamping unsupported
            return err;
        SignedData = Pkcs7SignedData->tstSignedData;
    }
    else {
        SignedData = Pkcs7SignedData;
    }

// find Signer's Cert by looking at Signer's Info name and proceed with finding matched Root CA Cert store 
    signer = Pkcs7_return_signing_cerificate(SignedData);
    if(signer==NULL) {
        *reason = X509_VALIDATE_CERTIFICATE_UNKNOWN;
        return err;
    }

    SignedData->sign_cert_start = signer->cert_start;
    SignedData->sign_cert_len =  signer->cert_len;

    os_get_time(&now);

    *reason = (*reason == -1)?X509_VALIDATE_CERTIFICATE_REVOKED:X509_VALIDATE_OK;

    do {
        // locate chained certs w matching Subject name
        for (cert = SignedData->CertStore, idx = 0; !chain_trusted && cert; cert = cert->next, idx++) {
            x509_name_string(&cert->subject, buf, sizeof(buf)); 
            wpa_printf(MSG_DEBUG, "PKCS7: %x:  Chain Cert (%x) Subject name: %a", idx, cert->cert_start, buf);
 
            //
            // Verify if cur cert is chained to Signer
            // Test Cert Expiration
            if ((long) now.sec <
                (long) cert->not_before ||
                (long) now.sec >
                (long) cert->not_after) {
                  wpa_printf(MSG_INFO, "PKCS7: Cert Timestamp Expired: "
                                "Time now=%X not_before=%X not_after=%X",
                              now.sec, cert->not_before, cert->not_after);
                //
                // w/a ignore time stamp checking
                //
#ifndef    CONFIG_IGNORE_X509_CERTIFICATE_EXPIRATION
                           *reason = X509_VALIDATE_CERTIFICATE_EXPIRED;
                            return err;
#endif                            
                //
                //            continue; // skip to next cert
            }

            // we match any cert in the Signer cert chain
            chain_valid = 0;
            if(  signer != cert &&
                // dbx may contain any cert in a cert store - even if not chained to a signer !?
                *reason != X509_VALIDATE_CERTIFICATE_REVOKED)
            {
                //...compare the issuer of image signer with subject name of a key signer candidate
                 if (x509_name_compare(&signer->issuer,
                                  &cert->subject) != 0)
                 {
                     wpa_printf(MSG_DEBUG, "PKCS7: Certificate "
                             "chain issuer name mismatch, keep looking...");
//                    x509_name_string(&signer->issuer, buf,
//                             sizeof(buf));
//                    wpa_printf(MSG_DEBUG, "PKCS7: Signer "
//                           "Issuer: %a", buf);
                        // give a chance to test a leaf cert against trusted
                     continue;
                 }
                    // found chained cert match.verifying chain validity
                 if (x509_valid_issuer(cert) < 0) {
                    *reason = X509_VALIDATE_BAD_CERTIFICATE;
                     return err;
                 }

//    redundant if we do not go down the chain - only match one cert
//    note: may need to un-comment if we do go down the cert chain
//            if ((cert->extensions_present &
//                 X509_EXT_PATH_LEN_CONSTRAINT) &&
//                idx > signer->path_len_constraint) {
//                wpa_printf(MSG_DEBUG, "PKCS7: pathLenConstraint"
//                       " not met (idx=%x issuer "
//                       "pathLenConstraint=%x)", idx,
//                       signer->path_len_constraint);
//                *result = X509_VALIDATE_BAD_CERTIFICATE;
//                return err;
//            }

                 if (x509_certificate_check_signature(cert, signer)
                     < 0) {
                    wpa_printf(MSG_DEBUG, "PKCS7: Invalid "
                       "certificate signature within chain");
                    *reason = X509_VALIDATE_BAD_CERTIFICATE;
                     return err;
                 }

                 SignedData->CAcert_start = cert->cert_start;
                 SignedData->CAcert_len = cert->cert_len;

                 chain_valid = 1;
                 // NEW: track if the cert is in chain to a signer
                 cert->chain = signer->chain = cert;
            }

            // note: Revocation rule for Hashed certs only apply to a Signer chained certificates
            if(revokeInfo) {
                wpa_printf(MSG_DEBUG, "PKCS7: Enter revokeInfo branch...");
                // based on a hashAlgo get hash of Trust Cert (trust->sign_value, trust->sign_value_len)
                //trust->signature_alg.
                hashLen = revokeInfo->ToBeSignedHashLen;
                if (hashLen == 32) {
                    hashType = SHA256;
                } else
                    if (hashLen == 48) {
                        hashType = SHA384;
                    } else
                        if (hashLen == 64) {
                            hashType = SHA512;
                        } else    {
                            wpa_printf(MSG_DEBUG, "PKCS7: Unsupported digestAlgorithm");
                            return err;
                        }
//APTIOV_SERVER_OVERRIDE_START
                if(0 != crypto_calculate_digest(hashType, &cert->tbs_cert_start, &cert->tbs_cert_len, hash, &hashLen))
//APTIOV_SERVER_OVERRIDE_END
                   return err;

            // hash mismatch keep looking for cert in a chain
                wpa_printf(MSG_DEBUG, "PKCS7: Find match with revoked Certificate Hash==>");
                wpa_hexdump(MSG_MSGDUMP, "Trust Cert", revokeInfo->ToBeSignedHash, 32);
                wpa_hexdump(MSG_MSGDUMP, "Chain Cert", hash, 32);
                if(os_memcmp(revokeInfo->ToBeSignedHash, hash, revokeInfo->ToBeSignedHashLen))
                    continue;    

            // TimeOfRevocation !=0 && TimeOfSigning >= TimeOfRevocation
                wpa_printf(MSG_DEBUG, "==>BINGO!");

                chain_trusted = 1;
                break;
            }

            leaf = NULL;
            for (trust = trusted; trust; trust = trust->next) {
                x509_name_string(&trust->subject, buf, sizeof(buf)); 
                wpa_printf(MSG_DEBUG, "PKCS7:  Trust Cert (%x) Subject name: %a", trust->cert_start, buf);

                // leaf cert may be used as trusted Root. Cert chain is already tested 
                if (chain_valid && x509_name_compare(&cert->subject, &trust->subject) == 0) {
                    leaf = cert;
                    break;
                }
                
                // Trusted Cert may match the Signer Cert
                // E.g. UEFI PK/KEK are signed by PK key directly without chaining.
                if (x509_name_compare(&signer->subject, &trust->subject) == 0) {
                    leaf = signer; 
                    break;
                }

                // Msft Win8 SignedData may use one level down certs as Trust cert.
                // chain may be same as Trust. 
                // Use Key from Trust cert to verify Signed cert or chain_cert 
                if (x509_name_compare(&cert->issuer, &trust->subject)
                    == 0)
                    break;
            }

            if (trust) {

                wpa_printf(MSG_DEBUG, "PKCS7: Found issuer from the "
                       "list of trusted certificates");

                // skip leaf check for self-signed certificates - Trust & Cert have same address 
                if(leaf && 
                    leaf->cert_start != trust->cert_start &&
//                    !os_memcmp(leaf->cert_start, trust->cert_start, trust->cert_len))
                    !os_memcmp(signer->tbs_cert_start, trust->tbs_cert_start, trust->tbs_cert_len))
                {
                    wpa_printf(MSG_DEBUG, "PKCS7: Found trusted certificate match to a leaf cert");
                    //
                    // Pass the certificate verification.
                    //    If any certificate in the chain is enrolled as trusted certificate,
                    // Digest verification will fail in the next step if the key is invalid.
    //                if(*result != X509_VALIDATE_CERTIFICATE_REVOKED)
//                    *reason = X509_VALIDATE_OK;
                    chain_trusted = 1;
                    break;
                } 

                if (x509_valid_issuer(trust) < 0) {
    // TEMP!!! Win8 PK may not have CA, but we Trust this Key certificate as it's known to be a CA 
    //              *result = X509_VALIDATE_BAD_CERTIFICATE;
    //             return err;
                }

                if (x509_certificate_check_signature(trust, cert) < 0)
                {
                   wpa_printf(MSG_DEBUG, "PKCS7: Invalid "
                           "certificate signature");
                        *reason = X509_VALIDATE_BAD_CERTIFICATE;
                    return err;
                }

                wpa_printf(MSG_DEBUG, "PKCS7: Trusted certificate match "
                       "found to complete the chain");

                chain_trusted = 1;
                break;
            }
            // valid chain found, if no match to trusted cert found - continue from the current cert
            if(chain_valid) {
                signer = cert;
            }
            
        } // for(cert
        
    // loop through additional TimeStamp Cert store for revoked certs
    } while(*reason == X509_VALIDATE_CERTIFICATE_REVOKED && 
            !chain_trusted && 
            (SignedData=SignedData->tstSignedData) !=NULL 
            );

    if (!chain_trusted) {
         wpa_printf(MSG_DEBUG, "PKCS7: Did not find any of the issuers "
               "from the list of trusted certificates");

         if (trusted || revokeInfo) {
            *reason = X509_VALIDATE_UNKNOWN_CA;
            return err;
        }
         wpa_printf(MSG_DEBUG, "PKCS7: Certificate chain validation "
               "disabled - ignore unknown CA issue");
    }

    *reason = X509_VALIDATE_OK;
    wpa_printf(MSG_DEBUG, "PKCS7: Certificate chain valid");

    return 0;

}

/**
 * Calculate digest of the input buffer based on hash typeVerify signature of certificate encryptedDigest
 * 
 * @param[in]  hashType    Type of hashing algorithm: MD5, SHA1, SHA256, SHA384 or SHA512
 * @param[in]  addr        pointer to an array of bytes to be hashed
 * @param[in]  len         data size to be hashed
 * @param[out] hash        hash buffer
 * @param[out] hashLen     hash buffer length
 * 
 * @retval     status      0 No error
 *                        -1 Error
 */
int crypto_calculate_digest(u8 hashType,
                            const u8 *addr[], const size_t *len,
                            u8 *hash, size_t *hashLen)
{
    int err = 0;

    switch(hashType) {
        case SHA1:
            if(*hashLen != SHA1_DIGEST_SIZE) {
                err = -1;
                *hashLen = SHA1_DIGEST_SIZE;
            } else 
                sha1_vector(1,  addr, len, hash);
            break;
        case SHA256:
            if(*hashLen != SHA256_DIGEST_SIZE) {
                err = -1;
                *hashLen = SHA256_DIGEST_SIZE;
            } else 
                sha256_vector(1, addr, len, hash);
            break;
        case SHA384:
            if(*hashLen != SHA384_DIGEST_SIZE) {
                err = -1;
                *hashLen = SHA384_DIGEST_SIZE;
            } else 
                sha384_vector(1, addr, len, hash);
            break;
        case SHA512:
            if(*hashLen != SHA512_DIGEST_SIZE) {
                *hashLen = 512;
                err = -1;
            } else
                sha512_vector(1, addr, len, hash);
            break;
#ifdef INTERNAL_MD5
        case MD5:
            if(*hashLen != MD5_DIGEST_SIZE) {
                *hashLen = MD5_DIGEST_SIZE;
                err = -1;
            } else
                md5_vector(1, addr, len, hash);
            break;
#endif
        default: 
            err = -1;
            *hashLen = 0;
            break;
    }
    return err;

}

/**
 * Pkcs7_decrypt_validate_digest - Verify signature of certificate encryptedDigest 
 * Returns: 0 if cert has a valid signature that was signed by the issuer,
 * -1 if not
 */
int Pkcs7_decrypt_validate_digest(const u8* enc_digest,
                                  size_t enc_digest_len,
                                  u8* public_key, 
                                  size_t public_key_len, 
                                  struct asn1_oid *digest_alg, 
                                  u8 *testHash, size_t hashLen)
{
    struct crypto_public_key *pk;
    u8 *data;
    const u8 *pos, *end, *next, *da_end;
    size_t data_len;
    struct asn1_hdr hdr;
    struct asn1_oid oid;
    int err = -1;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char buf[128];
#endif

    if(enc_digest==NULL || public_key==NULL || digest_alg==NULL || testHash==NULL) 
        return err;

    pk = crypto_public_key_import(public_key, public_key_len);
    if (pk == NULL)
        return err;

    data_len = enc_digest_len;
    data = (u8*)os_malloc(data_len);
    if (data == NULL) {
        crypto_public_key_free(pk);
        return err;
    }

    if (crypto_public_key_decrypt_pkcs1(pk, enc_digest, data_len, data, &data_len) < 0) {
        wpa_printf(MSG_DEBUG, "PKCS7: Failed to decrypt signature");
        crypto_public_key_free(pk);
        os_free(data);
        return err;
    }
    crypto_public_key_free(pk);

    wpa_hexdump(MSG_MSGDUMP, "PKCS7: Encrypted Signature data D", data, data_len);

    /*
     * PKCS #1 v1.5, 10.1.2:
     *
     * DigestInfo ::= SEQUENCE {
     *     digestAlgorithm DigestAlgorithmIdentifier,
     *     digest Digest
     * }
     *
     * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
     *
     * Digest ::= OCTET STRING
     *
     */
    if (asn1_get_next(data, data_len, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
               "(DigestInfo) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
//         os_free(data);
        return err;
    }

    pos = hdr.payload;
    end = pos + hdr.length;

    /*
     * X.509:
     * AlgorithmIdentifier ::= SEQUENCE {
     *     algorithm            OBJECT IDENTIFIER,
     *     parameters           ANY DEFINED BY algorithm OPTIONAL
     * }
     */

    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected SEQUENCE "
                  "(AlgorithmIdentifier) - found class %d tag 0x%x",
                  hdr.class, hdr.tag);
//         os_free(data);
        return err;
    }
    da_end = hdr.payload + hdr.length;

    if (asn1_get_oid(hdr.payload, hdr.length, &oid, &next)) {
         wpa_printf(MSG_DEBUG, "PKCS7: Failed to parse digestAlgorithm");
//         os_free(data);
        return err;
    }
    // compare MessageDigest algorithm
    if(oid.len != digest_alg->len ||
      os_memcmp(oid.oid, digest_alg->oid, digest_alg->len))
    {
        asn1_oid_to_str(&oid, buf, sizeof(buf));
        wpa_printf(MSG_DEBUG, "PKCS7: decryptDigest algorithm=%s", buf);
        asn1_oid_to_str(digest_alg, buf, sizeof(buf));
        wpa_printf(MSG_DEBUG, "PKCS7:    testDigest algorithm=%s", buf);
        wpa_printf(MSG_DEBUG, "PKCS7: digestAlgorithms don't match");
//        os_free(data);
        return err;
    }
    /* Digest ::= OCTET STRING */
    pos = da_end;
    end = data + data_len;

    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_OCTETSTRING) {
        wpa_printf(MSG_DEBUG, "PKCS7: Expected OCTETSTRING "
               "(Digest) - found Class %d tag 0x%x",
               hdr.class, hdr.tag);
//        os_free(data);
        return err;
    }
    wpa_hexdump(MSG_MSGDUMP, "PKCS7: Decrypted Digest",
            hdr.payload, hdr.length);

    /* AMI FIX for Bleichenbacher's RSA signature vulnerability*/
    /* Hash field must be at the right of the decoded Sig data*/
    if((hdr.payload + hdr.length) < end){
        wpa_printf(MSG_DEBUG, "PKCS7: Pkcs#1v1.5 Signature invalid");
        return err;
    }
    /* AMI FIX */        
    if (hdr.length > hashLen)
    {
        wpa_printf(MSG_DEBUG, "PKCS7: digest len does not match testHash %d > %d",
            hdr.length, hashLen);
//        os_free(data);
        return err;
    }
    wpa_hexdump(MSG_MSGDUMP, "PKCS7: Calculated hash ",
            testHash, hdr.length);

    if (os_memcmp(hdr.payload, (void*)testHash, hdr.length) != 0) {
        wpa_printf(MSG_INFO, "PKCS7: Certificate Digest does not match "
               "with calculated message hash");
//        os_free(data);
        return err;
    }

    os_free(data);

    wpa_printf(MSG_DEBUG, "PKCS7: Certificate Digest matches with "
           "calculated hash");

    return 0;
}
/**
 * PKCS_certificate_validate_digest - Verify certificate signature
 * SignedData: Complete PKCS certificate
 * SignCert: Signing certificate containing the public key
 * Returns: 0 if cert has a valid signature that was signed by the issuer,
 * -1 if not
 */
int Pkcs7_certificate_validate_digest(
                    struct pkcs7_signed_data_st *SignedData,
                    struct x509_certificate *SignCert,
                    u8 *fileHash, size_t fileHash_len)
{
    u8 hashType = 0;
    size_t hashLen = 0;
    u8 hash[SHA512_DIGEST_SIZE];    // calc hash on auth attrib
    u8 *pHash = &hash[0];
    int err = -1;

    wpa_printf(MSG_DEBUG, "PKCS7: validate Authenticode encrypted certificate...");

    if(SignedData==NULL) 
        return err;

//................................................................................
// what's to test: 
// 0. File hash matches one in CertType.Digest field
// 1. CertType hash matches one from messageDigest value
// 2. Decoded hash matches either CertType hash or authent.attrib hash
//    if(SignerInfo->auth_attrib_start && 
//        os_memcmp(SignerInfo->messageDigest, hash2, SignerInfo->messageDigest_len) != 0) {
//        wpa_printf(MSG_INFO, "PKCS7: Certificate Digests don't not match "
//               "with calculated message hash");
//        os_free(data);
//        return err;
//    }

// Verify with SelfSigned certificate
    if(SignCert==NULL)
        SignCert = Pkcs7_return_signing_cerificate(SignedData);
    if(SignCert==NULL)
        return err;
    
    // get MessageDigest algorithm
    if (x509_sha1_oid(&SignedData->SignerInfo->digest_alg.oid)) {
        hashType = SHA1;
        hashLen = SHA1_DIGEST_SIZE;
    } else 
        if (x509_sha256_oid(&SignedData->SignerInfo->digest_alg.oid)) {
            hashType = SHA256;
            hashLen = SHA256_DIGEST_SIZE;
        } else
            if (x509_sha384_oid(&SignedData->SignerInfo->digest_alg.oid)) {
                hashType = SHA384;
                hashLen = SHA384_DIGEST_SIZE;
            } else
                if (x509_sha512_oid(&SignedData->SignerInfo->digest_alg.oid)) {
                    hashType = SHA512;
                    hashLen = SHA512_DIGEST_SIZE;
                } else    {
                    wpa_printf(MSG_DEBUG, "PKCS7: Unsupported digestAlgorithm");
                    return err;
                }
/*
    if contentInfo present
        1. if fileHash present -
        1.1 compare with Digest from ContentInfo.SpcDirect data: Size, bin comparison. Return if failed
        1.2 if fileHash contains whole SpcIndirect buffer->hash the data and
     if autAttrib present :
        2. hash AuthAttribs and continue
     else
        3. Hash SignedData-> SpcIndirectData
    else
        if fileHash -> compare it with hash from decrypt Sig
        else
               Error. No digests to verify
*/
// UEFI: AuthVariable Signed Data Certificate must NOT have SignerInfo.authAttributes nor ContentInfo.Content
    if(SignedData->Content_start)
    {
        if(SignedData->Digest && fileHash && fileHash_len)
        {
            if(fileHash_len == SignedData->Digest_len) {
                if (os_memcmp(fileHash, SignedData->Digest, fileHash_len) != 0) {
                    wpa_hexdump(MSG_MSGDUMP, "PKCS7: contentInfo.Digest", SignedData->Digest, SignedData->Digest_len);
                    wpa_hexdump(MSG_MSGDUMP, "PKCS7: Input block       ", fileHash, fileHash_len);
                    wpa_printf(MSG_DEBUG, "PKCS7: contentInfo.Digest does not match "
                           "with calculated message hash");
                    return err;
                }
            } else
                if(fileHash_len == SignedData->Content_len) {
                    if (os_memcmp(fileHash, SignedData->Content_start, fileHash_len) != 0) {
                        wpa_hexdump(MSG_MSGDUMP, "PKCS7: SpcIndirectData", SignedData->Content_start, SignedData->Content_len);
                        wpa_hexdump(MSG_MSGDUMP, "PKCS7: Input block    ", fileHash, fileHash_len);
                        wpa_printf(MSG_DEBUG, "PKCS7: SpcIndirectData does not match "
                               "with input data");
                        return err;
                    }
                }
                else {
                    wpa_printf(MSG_DEBUG, "PKCS7: SpcIndirectData size not matching the size of input block");
                    return err;
                }
        }
//APTIOV_SERVER_OVERRIDE_START
        // 1st check passed --> calculated File hash matching the one in the Certificate
        if(0 != crypto_calculate_digest(hashType, &SignedData->Content_start, &SignedData->Content_len, pHash, &hashLen))
//APTIOV_SERVER_OVERRIDE_END
            return err;
    }
    if(SignedData->SignerInfo->authAttr.auth_attrib_start)
    {
        if(!SignedData->SignerInfo->authAttr.messageDigest)
        {
            wpa_printf(MSG_DEBUG, "PKCS7: SignerInfo.authAttributes.messageDigest must be present in "
                   "SignerInfo.authAttributes block");
            return err;
        }
        // verify the input value (hash of it) against the digest values stored within ContentInfo or signedAttributes 
        if(SignedData->Content_start)
        {
            if (os_memcmp(pHash, SignedData->SignerInfo->authAttr.messageDigest, hashLen) != 0) {
                wpa_hexdump(MSG_MSGDUMP, "PKCS7: AuthAttrib.messageDigest ", SignedData->SignerInfo->authAttr.messageDigest, SignedData->SignerInfo->authAttr.messageDigest_len);
                wpa_hexdump(MSG_MSGDUMP, "PKCS7: ContentInfo digest\t", pHash, hashLen);
                wpa_printf(MSG_DEBUG, "PKCS7: SignerInfo.authAttributes.messageDigest does not match "
                       "with contentInfo digest");
                return err;
            }
        } else
            if(fileHash && (fileHash_len == hashLen)) {
                if (os_memcmp(fileHash, SignedData->SignerInfo->authAttr.messageDigest, fileHash_len) != 0) {
                    wpa_hexdump(MSG_MSGDUMP, "PKCS7: AuthAttrib.messageDigest ", SignedData->SignerInfo->authAttr.messageDigest, SignedData->SignerInfo->authAttr.messageDigest_len);
                    wpa_hexdump(MSG_MSGDUMP, "PKCS7: Input block\t\t", fileHash, fileHash_len);
                    wpa_printf(MSG_DEBUG, "PKCS7: SignerInfo.authAttributes.messageDigest does not match "
                           "with input data");
                    return err;
                }
            }
//APTIOV_SERVER_OVERRIDE_START
        // If signedAttributes are present --> calculated the digest of the SignedAttrs fields
    	if(0 != crypto_calculate_digest(hashType, &SignedData->SignerInfo->authAttr.auth_attrib_start, &SignedData->SignerInfo->authAttr.auth_attrib_len, pHash, &hashLen))
//APTIOV_SERVER_OVERRIDE_END
            return err;
    }
    // expected format of Signed Data for Authenticated Variables 
    if(!SignedData->Content_start && !SignedData->SignerInfo->authAttr.auth_attrib_start)
    {
        if(fileHash && (fileHash_len == hashLen)) {
            pHash = fileHash;
        } else {
            wpa_printf(MSG_DEBUG, "PKCS7: Missing the Digest source");
            return err;
        }
    }
    return Pkcs7_decrypt_validate_digest(
            SignedData->SignerInfo->enc_digest, SignedData->SignerInfo->enc_digest_len,
            SignCert->public_key, SignCert->public_key_len, 
            &SignedData->SignerInfo->digest_alg.oid,
            pHash, hashLen);

}
/**
 * Pkcs7_certificate_validate_timestamp_digest - Verify TimeStamp signature of certificate encryptedDigest signature
 * SignedData: Complete PKCS certificate
 * SignCert: Signing certificate containing the public key
 * Returns: 0 if cert has a valid signature that was signed by the issuer,
 * -1 if not
 */
int Pkcs7_certificate_validate_timestamp_digest(struct pkcs7_signed_data_st *SignedData,
        os_time_t *TimeOfSigning)
{
    u8 hashType = 0;
    u8 hash[SHA512_DIGEST_SIZE];    // calc hash on auth attrib
    size_t hashLen= 0;
    int err = -1;

    wpa_printf(MSG_DEBUG, "PKCS7: validate TimeStamp encrypted certificate...");

    if(SignedData==NULL || TimeOfSigning == NULL) 
        return err;

    //  2. RFC6132 TimeStamp Token with tstInfo

    if(SignedData->tstSignedData && SignedData->/*tstSignedData->*/tstInfo) {

        wpa_printf(MSG_DEBUG, "PKCS7: tst TimeStamp certificate validate...");

        // get MessageDigest algorithm
        // 1. Matching the Hash TimeStamp->messageDigest field with calculated over SignedData->SignerInfo->enc_digest
        // get MessageDigest algorithm
        if (x509_sha1_oid(&SignedData->tstInfo->digest_alg.oid)) {
            hashType = SHA1;
            hashLen = 20;
        } else 
            if (x509_sha256_oid(&SignedData->tstInfo->digest_alg.oid)) {
                hashType = SHA256;
                hashLen = SHA256_DIGEST_SIZE;
            } else
                if (x509_sha384_oid(&SignedData->tstInfo->digest_alg.oid)) {
                    hashType = SHA384;
                    hashLen = SHA384_DIGEST_SIZE;
                } else
                    if (x509_sha512_oid(&SignedData->tstInfo->digest_alg.oid)) {
                        hashType = SHA512;
                        hashLen = SHA512_DIGEST_SIZE;
                    } else {
                        wpa_printf(MSG_DEBUG, "PKCS7: Unsupported digestAlgorithm");
                        return err;
                    }
//APTIOV_SERVER_OVERRIDE_START
            if(0 != crypto_calculate_digest(hashType, &SignedData->SignerInfo->enc_digest, &SignedData->SignerInfo->enc_digest_len, hash, &hashLen))
                return err;
//APTIOV_SERVER_OVERRIDE_END
            if (hashLen != SignedData->tstInfo->authAttr.messageDigest_len ||
                os_memcmp(SignedData->tstInfo->authAttr.messageDigest, hash, hashLen) != 0) {
                wpa_printf(MSG_INFO, "PKCS7: TimeStamp messageDigest does not match "
                                     "with hash of signature in encryptedDigest");
                return err;
            }

        *TimeOfSigning = SignedData->tstInfo->/*tstSignedData->*/authAttr.signingTime;

        return Pkcs7_certificate_validate_digest(SignedData->tstSignedData,NULL, NULL, 0);

    }

    wpa_printf(MSG_DEBUG, "PKCS7: TimeStamp certificate not detected...");

    return err;
    
}

/**
 * Pkcs7_x509_return_cert_pubKey 
 *  pCert: x509 cert to get the pub key from
 * Returns: 0 - success, -1 if error
 */
int Pkcs7_x509_return_pubKey(struct x509_certificate *pCert,
    u8 **public_key, size_t *public_key_len)
{
    const u8 *pos, *end;
    struct asn1_hdr hdr;
    int err = -1;

    if(pCert==NULL || public_key == NULL || public_key_len==NULL)
        return err;
    /*
     * PKCS #1, 7.1:
     * RSAPublicKey ::= SEQUENCE {
     *     modulus INTEGER, -- n
     *     publicExponent INTEGER -- e 
     * }
     */
    if (asn1_get_next(pCert->public_key, pCert->public_key_len, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL ||
        hdr.tag != ASN1_TAG_SEQUENCE) {
            wpa_printf(MSG_DEBUG, "RSA: Expected SEQUENCE "
                       "(public key) - found Class %d tag 0x%x",
                       hdr.class, hdr.tag);
        return err;
    }
    pos = hdr.payload;
    end = pos + hdr.length;
// this step is to get the pointer to beginning of the N modulus.
    if (asn1_get_next(pos, end - pos, &hdr) < 0 ||
        hdr.class != ASN1_CLASS_UNIVERSAL || hdr.tag != ASN1_TAG_INTEGER) {
            wpa_printf(MSG_DEBUG, "RSA: Expected INTEGER - found Class %d "
                       "tag 0x%x", hdr.class, hdr.tag);
        return err;
    }
    *public_key = (u8*)hdr.payload;
    *public_key_len = hdr.length;
// account for leading Zero in Asn.1 integer
    if(**public_key == 0) 
    {
        if((*public_key_len & 1) == 1)
// KeyGen adds a leading ZERO if the msb of the first byte of the n-modulus is ONE 
// account for leading Zero in Asn.1 integer
// ignore that Zero in the output file. We will add it back at the time of parsing of the Key
            {(*public_key)++;(*public_key_len)--;}
        else
            return err;
    }

    return 0;
}

/**
 * Pkcs7_x509_return_Cert_pubKey 
 *  pCert: x509 Key Certificate
 * Returns: 0 - success, -1 if error
 */
int Pkcs7_x509_return_Cert_pubKey(u8 *pCert, size_t cert_len,
    u8 **public_key, size_t *public_key_len)
{
    struct x509_certificate *x509Cert;

    if(pCert==NULL || !cert_len)
        return -1;

    x509Cert = x509_certificate_parse(pCert, cert_len);
        
    return Pkcs7_x509_return_pubKey(x509Cert, public_key, public_key_len);
}

/**
 * Pkcs7_x509_return_signing_Key 
 *  PKCS7cert: List of trusted certificates
 * Returns: 0 - success, -1 if error
 */
int Pkcs7_x509_return_signing_Key(struct pkcs7_signed_data_st *SignedData,
    u8 **public_key, size_t *public_key_len)
{
    struct x509_certificate *SignCert;

    SignCert = Pkcs7_return_signing_cerificate(SignedData);
    if(SignCert==NULL || SignCert->public_key==NULL)
        return -1;

    return Pkcs7_x509_return_pubKey(SignCert, public_key, public_key_len);

//    *public_key = SignCert->public_key;
//  *public_key_len = SignCert->public_key_len;
//  return 0;
}

/**
 * PKCS7_return_cerificate_ptr - Return pointer within DER buffer to:
 *  1. Signing certificate 
 *  2. Root CA certificate that is used to verify key in Signing certificate
 * SignedData: List of trusted certificates
  * Returns: cert if chain is valid, -1 if not found
 */
int Pkcs7_return_cerificate_ptr(struct pkcs7_signed_data_st *SignedData, 
    u8** CAcert, size_t* CAcert_len,
    u8** SignCert, size_t* SignCert_len)
{
    int res=0, reason=0;

    if(SignedData == NULL)
        return -1;
    
    // First time run - Pkcs7cert has empty ptrs
    if(SignedData->CAcert_start == NULL) {
        // run validate chain with NULL in Trust field
        res = Pkcs7_x509_certificate_chain_validate(SignedData, NULL, &reason);
    }
    *CAcert_len = SignedData->CAcert_len;
    *SignCert_len = SignedData->sign_cert_len;
    if(!res) {
        *CAcert = (u8*)SignedData->CAcert_start;
        *SignCert = (u8*)SignedData->sign_cert_start;
    }
    return res;
}

/**
 * Pkcs7_return_digestAlgorithm - Return digestAlgorithm type
 *  PKCS7cert: List of trusted certificates
  * Returns: cert if chain is valid, -1 if not found
 */
int Pkcs7_return_digestAlgorithm(struct pkcs7_signed_data_st *PKCS7cert, u8* HashType)
{
    struct asn1_oid *oid;

    if(HashType==NULL)
        return -1;

    *HashType = SHA1;

    oid = (struct asn1_oid *)&PKCS7cert->Digest_alg.oid;//SignerInfo->digest_alg.oid;

    if (x509_sha1_oid(oid)) {
        *HashType = SHA1;
    } 
    else
        if (x509_sha256_oid(oid)) {
            *HashType = SHA256;
        }
        else
            if (x509_sha384_oid(oid)) {
                *HashType = SHA384;
            }
            else
                if (x509_sha512_oid(oid)) {
                    *HashType = SHA512;
                }
                else if (!x509_digest_oid(oid)) {
                    wpa_printf(MSG_DEBUG, "PKCS7: Unrecognized digestAlgorithm");
                    return -1;
                }

    return 0;
}

/**
 *  Produces a Null-terminated ASCII string in an output buffer based on a Null-terminated
 *  ASCII format string from a Subject name structure of a CommonName.
 * Returns: 0 if operation is success, -1 if not
 */
int Pkcs7_x509_return_SubjectNameStr(u8 *pCert, size_t cert_len, 
                                     u8 *buf, size_t len)
{
    int res=0;
    struct x509_name *name;
    struct x509_certificate *x509Cert;

    if(pCert==NULL || !cert_len || buf==NULL || !len)
        return -1;

    x509Cert = x509_certificate_parse(pCert, cert_len);

    if(x509Cert==NULL )
        return -1;

    name = (struct x509_name *)&x509Cert->subject;
    
//#ifndef CONFIG_NO_STDOUT_DEBUG
//    x509_name_string(name, (char *)buf, len);
//#else
    if (name->cn == NULL)
        return -1;
    res = os_snprintf((char *)buf, len, "%s", name->cn);
    if (res < 0 || res >= (int)len) {
         return -1;
    }
//#endif
    return 0;
}

/**
  Retrieves all embedded certificates from PKCS#7 signed data as described in "PKCS #7:
  Cryptographic Message Syntax Standard", and outputs two certificate lists chained or
  unchained to the signer's certificates.

  @param[in]  P7Data            Pointer to the PKCS#7 message.
  @param[in]  P7Length          Length of the PKCS#7 message in bytes.
  @param[in]  bChain            Which type of a certificate list to return . 1 - Signer chained, 0 - un-chained
  @param[out] CertList          Pointer to the certificates list chained to signer's/un-chained
                                certificate. It's caller's responsibility to free the buffer.
  @param[out] ListLength        Length of the linked certificates list buffer in bytes.

  @retval  0         The operation is finished successfully.
  @retval  -1        Error occurs during the operation.

**/
int Pkcs7_x509_get_certificates_List(struct pkcs7_signed_data_st *SignedData, BOOLEAN bChained,
    u8** CertList, size_t* ListLength)
{
    int res=0, reason=0;
    long unsigned idx;
    struct x509_certificate *cert;
#ifndef CONFIG_NO_STDOUT_DEBUG
    char buf[128];
#endif
    u8              *CertBuf;
    u8              *NewCert;
    u8              *OldBuf;
    unsigned int     BufferSize;
    unsigned int     OldSize;

  //
  // Parameter Checking
  //
    if ((SignedData == NULL) || (CertList == NULL) || (ListLength == NULL) ){
        return -1;
    }
  // run validate chain with NULL in Trust field
    res = Pkcs7_x509_certificate_chain_validate(SignedData, NULL, &reason);
    if(res == -1)
        return res;
    //
    // Converts Chained and Untrusted Certificate to Certificate Buffer in following format:
    //      UINT8  CertNumber;
    //      UINT32 Cert1Length;
    //      UINTN  Cert1Ptr;
    //      UINT32 Cert2Length;
    //      UINTN  Cert2Ptr;
    //      ...
    //      UINT32 CertnLength;
    //      UINTN  CertnPtr;

    CertBuf    = NULL;
    OldBuf     = NULL;

    cert = SignedData->CertStore;

    BufferSize = sizeof (UINT8);
    OldSize    = BufferSize;

    res = -1;
    for (idx = 1; cert; cert = cert->next) {
        x509_name_string(&cert->subject, buf, sizeof(buf)); 
        wpa_printf(MSG_DEBUG, "PKCS7: %x: chain=%x: %a", idx, cert->chain, buf);
        // Chained
         if(bChained && (cert->chain == NULL))
             continue;
        // un-chained
         if(!bChained && cert->chain)
             continue;
/*
        OldSize    = BufferSize;
        OldBuf     = CertBuf;
        BufferSize = OldSize + cert->cert_len + sizeof (UINT32);
        CertBuf    = os_malloc (BufferSize);
        if (CertBuf == NULL) {
          goto _Exit;
        }
        if (OldBuf != NULL) {
          os_memcpy(CertBuf, OldBuf, OldSize);
          os_free (OldBuf);
          OldBuf = NULL;
        }
        WriteUnaligned32 ((UINT32 *) (CertBuf + OldSize), (UINT32)cert->cert_len);
        os_memcpy (CertBuf + OldSize + sizeof (UINT32), cert->cert_start, cert->cert_len);
*/
         OldSize    = BufferSize;
         OldBuf     = CertBuf;
         BufferSize = OldSize + sizeof (UINTN) + sizeof (UINT32);
         CertBuf    = os_malloc (BufferSize);
         if (CertBuf == NULL) {
           goto _Exit;
         }
         if (OldBuf != NULL) {
           os_memcpy (CertBuf, OldBuf, OldSize);
           os_free (OldBuf);
           OldBuf = NULL;
         }
         NewCert = CertBuf + (UINT32)OldSize;
         // Update CertLen
         *(UINT32*)NewCert = (UINT32)cert->cert_len;
         NewCert = CertBuf + (UINTN)OldSize + sizeof (UINT32);
         // Update CertPtr
         *(UINTN*)NewCert = (UINTN)cert->cert_start;
        // Update CertNumber
        CertBuf[0] = (UINT8)idx++;
    }
    if (CertBuf != NULL) {
        *CertList = CertBuf;
        *ListLength = (size_t)BufferSize;
        res = 0;
    }

_Exit:
    //
    // Release Resources
    //
    if (OldBuf != NULL) {
        os_free (OldBuf);
    }
 
    return res;
}
#endif /* CONFIG_NO_INTERNAL_PEI_PKCS7 */
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
