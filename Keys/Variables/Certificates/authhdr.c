#include "TimeStamp.h"
#include "authhdr.h"
#include <AmiCertificate.h>

#ifndef CertSize 
#define CertSize 0
#endif

#pragma pack(1)
typedef struct {
    EFI_TIME                            TimeStamp;
    WIN_CERTIFICATE_UEFI_GUID_1         AuthInfo;
} AMI_EFI_VARIABLE_AUTHENTICATION_2;
#pragma pack()

AMI_EFI_VARIABLE_AUTHENTICATION_2 AuthHdr = 
    {   
       FOUR_DIGIT_YEAR_INT, 
       TWO_DIGIT_MONTH_INT, 
       TWO_DIGIT_DAY_INT, 
       TWO_DIGIT_HOUR_INT, 
       TWO_DIGIT_MINUTE_INT, 
       TWO_DIGIT_SECOND_INT,
/*
        2015,//FOUR_DIGIT_YEAR_INT, 
        01,//TWO_DIGIT_MONTH_INT, 
        01,//TWO_DIGIT_DAY_INT, 
        00,//TWO_DIGIT_HOUR_INT, 
        00,//TWO_DIGIT_MINUTE_INT, 
        00,//TWO_DIGIT_SECOND_INT,*/
       0,0,0,0,0,
       CertSize+sizeof(WIN_CERTIFICATE_UEFI_GUID_1),
       0x200,                      // revision
       WIN_CERT_TYPE_EFI_GUID,     // wCertificateType
       EFI_CERT_TYPE_PKCS7_GUID    // CertType
    };
