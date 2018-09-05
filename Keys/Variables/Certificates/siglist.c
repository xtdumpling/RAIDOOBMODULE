#include <AmiCertificate.h>
#include <Protocol/AmiDigitalSignature.h>
#include <Guid/ImageAuthentication.h>
#include "siglist.h"
#include "OwnerGuid.h"
#pragma pack(1)
typedef struct {
    EFI_SIGNATURE_LIST   SigList;
    EFI_GUID             SigOwner;
} EFI_SIGNATURE_LIST_1;

#ifndef SIG_TYPE
#if CertSize == 32
#define SIG_TYPE   EFI_CERT_SHA256_GUID    // SigList.Type
#endif
#if CertSize == 48 //sizeof(EFI_CERT_X509_SHA256)
#define SIG_TYPE   EFI_CERT_X509_SHA256_GUID
#endif
#if CertSize == 64 //sizeof(EFI_CERT_X509_SHA384)
#define SIG_TYPE   EFI_CERT_X509_SHA384_GUID
#endif
#if CertSize == 80 //sizeof(EFI_CERT_X509_SHA512)
#define SIG_TYPE   EFI_CERT_X509_SHA512_GUID,
#endif
#if CertSize == 256
#define SIG_TYPE   EFI_CERT_RSA2048_GUID
#endif
#if CertSize > 256
#define SIG_TYPE   EFI_CERT_X509_GUID
#endif
#endif

#ifndef SIG_SIZE
#define SIG_SIZE   sizeof(EFI_GUID)+CertSize
#endif


EFI_SIGNATURE_LIST_1 SigList1 = {
    {
        SIG_TYPE,// SigType
        (UINT32)(sizeof(EFI_GUID)+CertSize+sizeof(EFI_SIGNATURE_LIST)), // SigList.Size
        0,       // HdrSize
        (UINT32)SIG_SIZE // SigSize
    },
    SIGOWNER_GUID,
};
