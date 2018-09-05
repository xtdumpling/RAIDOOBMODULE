//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: VerifyFwCapsule.c - PEI secured recovery services
//
// Description:   File contains VerifyFwCapsule hook to evaluate Fw Capsule
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>

//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include "Efi.h"
#include "Pei.h"
#include "Token.h"
#include <AmiPeiLib.h>
#include <Hob.h>
#include <RomLayout.h>
#include <Ffs.h>
#include <FlashUpd.h>
#include <PPI/CryptoPPI.h>
#include <Ppi/FwVersion.h> 
#include "AmiCertificate.h"
#include <Protocol/AmiDigitalSignature.h>
#include <Library/DebugLib.h>

//----------------------------------------------------------------------------
// Function Externs 

//----------------------------------------------------------------------------
// Local prototypes

//----------------------------------------------------------------------------
typedef struct {
    EFI_FFS_FILE_HEADER FfsHdr;
    EFI_COMMON_SECTION_HEADER SecHdr;
    EFI_GUID            SectionGuid;
    UINT8               FwCapHdr[0];
} AMI_FFS_COMMON_SECTION_HEADER;

typedef struct _FID_SECTION {
    EFI_GUID   Guid;
    FW_VERSION FwVersion;
} FID_SECTION;

//----------------------------------------------------------------------------
// Local Variables
static EFI_GUID FWCapsuleGuid    = APTIO_FW_CAPSULE_GUID;
static EFI_GUID FWkeyGuid        = PR_KEY_GUID;
static EFI_GUID FwCapFfsGuid     = AMI_FW_CAPSULE_FFS_GUID;
static EFI_GUID FwCapSectionGuid = AMI_FW_CAPSULE_SECTION_GUID;
static EFI_GUID FidSectionGuid   = \
    { 0x2EBE0275, 0x6458, 0x4AF9, 0x91, 0xed, 0xD3, 0xF4, 0xED, 0xB1, 0x00, 0xAA };

const UINT8 *FwBadKey = "$BAD";
const UINT8 *FidSignature = "$FID";

EFI_PEI_SERVICES  **gPeiServices;
static AMI_CRYPT_DIGITAL_SIGNATURE_PPI *gpAmiSigPPI=NULL;

static UINT8 gHashDB[SHA256_DIGEST_SIZE];
// Allocate Hash Descr table
static UINTN *gAddrList=NULL;
static UINTN *gLenList=NULL;
static UINTN gHashNumElem=0;

//----------------------------------------------------------------------------
// Function Definitions

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   VerifyProjectId
//
// Description: 
//
// Input:       
//
// Output:      
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN 
VerifyProjectId (
    IN FW_VERSION      *FwVersionData,
    IN OUT UINT32      *FailedVTask
)
{
    char  *strProjectId = CONVERT_TO_STRING(PROJECT_TAG);
    char   strOrgProjectId[sizeof(CONVERT_TO_STRING(PROJECT_TAG))];
    UINTN  Size = sizeof(CONVERT_TO_STRING(PROJECT_TAG));
/*
CHAR8       BiosTag[9];     //BIOS Tag
EFI_GUID    FirmwareGuid;       //Firmware GUID
CHAR8       CoreMajorVersion[3];    
CHAR8       CoreMinorVersion[3];
CHAR8       ProjectMajorVersion[3];
CHAR8       ProjectMinorVersion[3];
*/
// Project ID, Major, Minor rev    
    MemSet(&strOrgProjectId, Size, 0);
    MemCpy( strOrgProjectId, FwVersionData->BiosTag, Size-1);
    DEBUG ((SecureMod_DEBUG_LEVEL, "OrgBiosTag=%a, NewBiosTag=%a\nOrgPrjVer=%d, NewPrjVer=%d\n",
        strOrgProjectId, strProjectId,(PROJECT_MAJOR_VERSION*100+PROJECT_MINOR_VERSION),
        (Atoi(FwVersionData->ProjectMajorVersion)*100+Atoi(FwVersionData->ProjectMinorVersion))
    ));
    if (Size==0 || MemCmp (strOrgProjectId, strProjectId, Size-1)) return FALSE;
#if IGNORE_IMAGE_ROLLBACK == 0
    if((Atoi(FwVersionData->ProjectMajorVersion)*100+Atoi(FwVersionData->ProjectMinorVersion)) <
       (PROJECT_MAJOR_VERSION*100+PROJECT_MINOR_VERSION) 
       )
#if (defined(REFLASH_INTERACTIVE) && REFLASH_INTERACTIVE==1)
    // Physically present user may override the roll-back protection from the Setup screen
    return TRUE;
#else    
    return FALSE;
#endif    
#endif

    *FailedVTask = 0;
    return TRUE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetFidData
//
// Description: Function to read FFS FID data structure from the given data buffer
//
// Input:       OUT FW_VERSION **Fid - pointer to output buffer
//              IN  VOID *pFV - pointer to data buffer to read from
//
// Output:      EFI_SUCCESS if FID data is retrieved
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN GetFidData(
    IN  VOID           *pFV,
    IN  UINT32          Size,
    OUT FW_VERSION    **FwVersionData
)
{
//    UINT32 Signature;
    UINT8* SearchPointer;
    FID_SECTION *Section;

// Simplified search by $FID signature only.
//    SearchPointer = (UINT32 *)((UINT8 *)pFV + sizeof(EFI_GUID));
//    Signature = FidSectionGuid.Data1;
    SearchPointer = (UINT8 *)pFV;

    do {
//        if(*SearchPointer == Signature) {
            Section = (FID_SECTION *)SearchPointer;
            if(!guidcmp(&FidSectionGuid, &(Section->Guid)) && 
               (*((UINT32*)(&Section->FwVersion.FirmwareID[0])) == *(UINT32*)FidSignature)){ 
                *FwVersionData = &Section->FwVersion;
                return TRUE;
            }
//        }
    } while( SearchPointer++ < (UINT8*)((UINTN)pFV+Size));

    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   VerifyFwRevision
//
// Description: Verify Fw revision compatibility
//              NewVer > OldVer, newProjectTAGid = oldProjectTAGid
//
// Input:
//      IN OUT UINT8 *pCapsule
// Output:
//      EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
VerifyFwRevision (
    IN APTIO_FW_CAPSULE_HEADER  *FWCapsuleHdr,
    IN UINT8           *RomData,
    IN OUT UINT32      *FailedVTask
)    
{
    ROM_AREA                *Area;
    EFI_PHYSICAL_ADDRESS    FvAddress;
    FW_VERSION             *FwVersionData;

    *FailedVTask = Ver;

    Area = (ROM_AREA *)((UINTN)FWCapsuleHdr+FWCapsuleHdr->RomLayoutOffset);
    for (Area; Area->Size != 0; Area++) {
        if (!(Area->Attributes & ROM_AREA_FV_SIGNED)) 
            continue;
        //  $FID can be in FV with either PEI or DXE
        if (!(Area->Attributes & (ROM_AREA_FV_PEI+ROM_AREA_FV_DXE))) 
            continue;

        FvAddress = (EFI_PHYSICAL_ADDRESS)RomData + (Area->Offset);
        if (GetFidData((UINT8*)FvAddress, Area->Size, &FwVersionData)){
            if(VerifyProjectId(FwVersionData, FailedVTask))
                return EFI_SUCCESS;
            break;
        }
    } 
// At least one FW block must be signed OR no $FID structure found in the new FW image
    return EFI_SECURITY_VIOLATION;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FindCapHdrFFS
//
// Description: Function to read FW Cap Sig data from Ffs
//
// Input:       OUT UINT8 **pFwCapHdr - pointer to output buffer
//              IN  VOID *pCapsule - pointer to data buffer to read from
//
// Output:      EFI_SUCCESS if Capsule Hdr with Signature is retrieved
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FindCapHdrFFS(
    IN  VOID    *pCapsule,
    OUT UINT8 **pFfsData
)
{
    UINT32 Signature;
    UINT32 *SearchPointer;
    AMI_FFS_COMMON_SECTION_HEADER *FileSection;
    APTIO_FW_CAPSULE_HEADER *pFwCapHdr;

    SearchPointer = (UINT32 *)((UINT8 *)pCapsule - sizeof(AMI_FFS_COMMON_SECTION_HEADER) + FWCAPSULE_MAX_PAYLOAD_SIZE);
    Signature = FwCapFfsGuid.Data1;

    do {
        if(*SearchPointer == Signature) {
            FileSection = (AMI_FFS_COMMON_SECTION_HEADER *)SearchPointer;
            if(!guidcmp(&FwCapFfsGuid, &(FileSection->FfsHdr.Name)) &&
               !guidcmp(&FwCapSectionGuid, &(FileSection->SectionGuid))
            ){
                pFwCapHdr = (APTIO_FW_CAPSULE_HEADER*)(FileSection->FwCapHdr);
                // just a sanity check - Cap Size must match the Section size
                if(((*(UINT32 *)FileSection->FfsHdr.Size) & 0xffffff) >=
                        pFwCapHdr->CapHdr.HeaderSize + sizeof(AMI_FFS_COMMON_SECTION_HEADER) &&
                    !guidcmp((EFI_GUID*)&pFwCapHdr->CapHdr.CapsuleGuid, &FWCapsuleGuid)
                ){
                    *pFfsData = (UINT8*)pFwCapHdr;
                        return EFI_SUCCESS;
                }    
            }
        }
    } while(SearchPointer-- != pCapsule);

    return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HashFwRomMapImage
//
// Description:    The Rom image hash is calculated based on info from the Rom Area map
//
// Input:        
//          Payload - pointer to a FW Image
//          FwCapsuleHdr - pointer to a FW Capsule Hdr
//          RomSize - Size of Rom Image
//
// Output:      EFI_SUCCESS - capsule processed successfully
//              EFI_DEVICE_ERROR - capsule processing failed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HashFwRomMapImage (
    IN APTIO_FW_CAPSULE_HEADER  *FWCapsuleHdr,
    IN UINT8                    *Payload,
    IN UINTN                     RomSize,
    OUT UINT8                   *gHashDB
){
    EFI_STATUS              Status = EFI_SUCCESS;
    ROM_AREA                *RomAreaTbl;

    UINTN    i, RomMap_size, max_num_elem, num_elem;

    RomAreaTbl = (ROM_AREA *)((UINTN)FWCapsuleHdr+FWCapsuleHdr->RomLayoutOffset);

    RomMap_size = FWCapsuleHdr->RomImageOffset-FWCapsuleHdr->RomLayoutOffset;
    max_num_elem = RomMap_size/sizeof(ROM_AREA);

// assume max size of RomMap array = RomMap_size/sizeof(ROM_AREA);
// or better yet ...calculate exact number
    num_elem = 0;
    for (i=0; i < max_num_elem && RomAreaTbl[i].Size != 0; i++ )
    {
        if (RomAreaTbl[i].Attributes & ROM_AREA_FV_SIGNED)
            num_elem++;
    }
    max_num_elem=i;
    num_elem+=2; // add 2 extra entries

    if(!gAddrList || !gHashNumElem || gHashNumElem<num_elem) {
        gHashNumElem = num_elem;
        i = num_elem*sizeof(UINTN);
        Status = (*gPeiServices)->AllocatePool(gPeiServices, i*2, &gAddrList);
        ASSERT_PEI_ERROR (gPeiServices, Status);
        if(EFI_ERROR(Status)) return Status;
        gLenList = (UINTN*)((UINTN)gAddrList + i);
    }
    num_elem = 0;
    for(i=0; i < max_num_elem && num_elem < gHashNumElem && RomAreaTbl[i].Size != 0; i++)
    {
        if (!(RomAreaTbl[i].Attributes & ROM_AREA_FV_SIGNED)) 
            continue;
    // sanity check for buffer overruns
        if(RomAreaTbl[i].Offset > RomSize ||
           ((UINT64)RomAreaTbl[i].Offset + RomAreaTbl[i].Size > RomSize))
            return EFI_SECURITY_VIOLATION;
    // RomArea only holds offsets within a payload
        gAddrList[num_elem] = (UINTN)Payload + RomAreaTbl[i].Offset;
        gLenList[num_elem] = RomAreaTbl[i].Size;

        num_elem++;

    }
    if(num_elem >= gHashNumElem) return EFI_SECURITY_VIOLATION;
//
//  Hash of Capsule Hdr + FW Certificate Hdr
//
    if(FWCapsuleHdr->CapHdr.Flags & CAPSULE_FLAGS_CAPHDR_IN_SIGNCERT) {
        gAddrList[num_elem] = (UINTN) FWCapsuleHdr;
        gLenList[num_elem] = (UINTN)&FWCapsuleHdr->FWCert.SignCert.CertData - (UINTN)FWCapsuleHdr;
        num_elem++;
        if(num_elem >= gHashNumElem) return EFI_SECURITY_VIOLATION;
    }
//
//  Hash of the ROM_MAP table
//
    gAddrList[num_elem] = (UINTN)RomAreaTbl;
    gLenList[num_elem] = (i+1)*sizeof(ROM_AREA);
    num_elem++;

    Status = gpAmiSigPPI->Hash(gpAmiSigPPI, &gEfiHashAlgorithmSha256Guid, 
                num_elem, (const UINT8**)gAddrList,  (const UINTN*)gLenList, gHashDB );

    DEBUG ((SecureMod_DEBUG_LEVEL, "Hash the FW Image %r\n", Status));

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    VerifyFwCertPkcs7
//
// Description:    This code verifies FW Capsule is genuine, 
//                 and performs following checks on the image:
//                  1. Signing certificate is signed with trusted Root Platform key
//                  2. Integrity check. Image Signature verification
//
// Input:        
//          Payload - pointer to a FW Image
//          FwCapsuleHdr - pointer to a FW Capsule Hdr
//          RomSize - Size of Rom Image
//
// Output:      EFI_SUCCESS - capsule processed successfully
//              EFI_DEVICE_ERROR - capsule processing failed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS VerifyFwCertPkcs7 (
    IN APTIO_FW_CAPSULE_HEADER  *FWCapsuleHdr,
    IN UINT8                    *Payload,
    IN UINTN                     RomSize,
    IN PEI_CRYPT_HANDLE         *PubKeyHndl,
    IN OUT UINT32               *FailedVTask
){
    EFI_STATUS              Status;
    UINT8                  *Pkcs7Cert, *pDigest, *TrustCert;
    UINTN                   Pkcs7Cert_len, DigestLen, CertSize;

    EFI_CERT_X509_SHA256     HashCert;
//
// 1. Validate Root Key
//
    if( FailedVTask==NULL )
        return EFI_SECURITY_VIOLATION; 

    *FailedVTask = Key;

    if( PubKeyHndl==NULL || PubKeyHndl->Blob==NULL)
        return EFI_SECURITY_VIOLATION; 

    if(!guidcmp(&PubKeyHndl->AlgGuid, &gEfiCertX509Guid)) {
        TrustCert = PubKeyHndl->Blob;
        CertSize  = PubKeyHndl->BlobSize;
    } else
    // potentially a TimeStamped Hash of x509 tbs certificate data
    if(!guidcmp(&PubKeyHndl->AlgGuid, &gEfiCertSha256Guid)) {
        MemCpy(HashCert.ToBeSignedHash, PubKeyHndl->Blob, SHA256_DIGEST_SIZE);
        TrustCert = (UINT8*)&HashCert;
        CertSize = sizeof(EFI_CERT_X509_SHA256);
    } else
        return EFI_UNSUPPORTED;

// 2. Verify Signing Cert Signature
//
// 2.1 The Rom image hash is calculated based on info from the Rom Area map
//
    *FailedVTask = Sig;

    Status = HashFwRomMapImage(FWCapsuleHdr, Payload, RomSize, gHashDB);
    if (EFI_ERROR(Status)) return Status;

// 2.2 Verify Fw Certificate
    pDigest = &gHashDB[0];
    DigestLen = SHA256_DIGEST_SIZE;
    Pkcs7Cert = (UINT8*)&FWCapsuleHdr->FWCert.SignCert.CertData; 
    Pkcs7Cert_len = FWCapsuleHdr->FWCert.SignCert.Hdr.Hdr.dwLength-sizeof(WIN_CERTIFICATE_UEFI_GUID_1);

    return gpAmiSigPPI->VerifyPkcs7Sig( gpAmiSigPPI,
                     Pkcs7Cert, Pkcs7Cert_len,           // Pkcs7Cert
                     TrustCert, CertSize,
                     &pDigest, &DigestLen               // In/OutData
                     );
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    VerifyFwCertRsa2048Sha256
//
// Description:    This code verifies FW Capsule is genuine, 
//                 and performs following checks on the image:
//                  1. Signing certificate is signed with trusted Root Platform key
//                  2. Integrity check. Image Signature verification
//
// Input:        
//          Payload - pointer to a FW Image
//          FwCapsuleHdr - pointer to a FW Capsule Hdr
//          RomSize - Size of Rom Image
//
// Output:      EFI_SUCCESS - capsule processed successfully
//              EFI_DEVICE_ERROR - capsule processing failed
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS VerifyFwCertRsa2048Sha256 (
    IN APTIO_FW_CAPSULE_HEADER  *FWCapsuleHdr,
    IN UINT8                    *Payload,
    IN UINTN                     RomSize,
    IN OUT UINT32               *FailedVTask
){
    EFI_STATUS              Status;
    PEI_CRYPT_HANDLE        HashHndl;
    PEI_CRYPT_HANDLE        PubKeyHndl;
    UINT8                   *pSig;
    UINT32                  Flags;
    UINT8                   *Addr;
    UINTN                   Size;
    EFI_CERT_BLOCK_RSA_2048_SHA256* pRootCert;

    Flags = (FWCapsuleHdr->CapHdr.Flags & CAPSULE_FLAGS_RSA_PSS_PADDING_SCHEME)?EFI_CRYPT_RSASSA_PSS:EFI_CRYPT_RSASSA_PKCS1V15;

    HashHndl.AlgGuid  = gEfiHashAlgorithmSha256Guid;
    HashHndl.BlobSize = SHA256_DIGEST_SIZE;
    HashHndl.Blob = (UINT8*)&gHashDB;
//
// 1. Compare Capsule's Sign Cert key with Platform Root Key
//
    *FailedVTask = Key;

    PubKeyHndl.BlobSize =  DEFAULT_RSA_KEY_MODULUS_LEN;
    PubKeyHndl.AlgGuid = gEfiCertRsa2048Guid;
    PubKeyHndl.Blob = (UINT8*)FWCapsuleHdr->FWCert.SignCert.CertData.PublicKey;
    Status = gpAmiSigPPI->VerifyKey(gpAmiSigPPI, &FWkeyGuid, &PubKeyHndl); 
    DEBUG ((SecureMod_DEBUG_LEVEL, "Compare Platform and SignCert Keys : %r\n", Status));
//  Skip the RootCert key checking if SignCert Key and PR Key are a Match
    if(EFI_ERROR(Status)) {
//
// 1.1 Compare Platform Root with Capsule's Key from a Root Key store
//
        for (pRootCert = &FWCapsuleHdr->FWCert.RootCert; 
             (UINTN)pRootCert < ((UINT64)(UINTN)&FWCapsuleHdr->FWCert+FWCapsuleHdr->FWCert.SignCert.Hdr.Hdr.dwLength) &&
                 pRootCert->PublicKey[0]!=0;
             pRootCert++) 
        {
            PubKeyHndl.Blob = (UINT8*)pRootCert->PublicKey;
            Status = gpAmiSigPPI->VerifyKey(gpAmiSigPPI, &FWkeyGuid, &PubKeyHndl); 
            DEBUG ((SecureMod_DEBUG_LEVEL, "Compare Platform and RootCert Keys : %r\n", Status));
            if (EFI_ERROR(Status)) continue;        
    
    // 2. Verify RootCert.Signature
    //
    // 2.1  Compute FWCert.SignCert.PublicKey Hash
    // 
            if(FWCapsuleHdr->CapHdr.Flags & CAPSULE_FLAGS_SIGNKEY_IN_ROOTCERT)
            {
                Addr = (UINT8*)FWCapsuleHdr->FWCert.SignCert.CertData.PublicKey;
                Size = DEFAULT_RSA_KEY_MODULUS_LEN;
            } else
    // 2.2  Compute FWCert.SignCert Hash
            {
                Addr = (UINT8*)&FWCapsuleHdr->FWCert.SignCert;
                Size = sizeof(AMI_CERTIFICATE_RSA2048_SHA256);
            }

            Status = gpAmiSigPPI->Hash(gpAmiSigPPI,&gEfiHashAlgorithmSha256Guid, 1,&Addr,(const UINTN*)&Size, gHashDB); 
            if (EFI_ERROR(Status)) break;
        
            pSig = (void*)pRootCert->Signature; 
            Status = gpAmiSigPPI->VerifySig(gpAmiSigPPI, &PubKeyHndl, &HashHndl, pSig, DEFAULT_RSA_SIG_LEN, Flags ); 
            DEBUG ((SecureMod_DEBUG_LEVEL, "Verify Root Cert : %r\n", Status));
            break;
        }
    }
    if (EFI_ERROR(Status)) return EFI_SECURITY_VIOLATION;
//
// 3. Verify Signing Cert
//
    *FailedVTask = Sig;
//
// 3.1 The Rom image hash is calculated based on info from the Rom Area map
//
    Status = HashFwRomMapImage(FWCapsuleHdr, Payload, RomSize, gHashDB);
    if (EFI_ERROR(Status)) return Status;

    pSig = (void*)FWCapsuleHdr->FWCert.SignCert.CertData.Signature; 
    PubKeyHndl.Blob = (UINT8*)FWCapsuleHdr->FWCert.SignCert.CertData.PublicKey;

    return gpAmiSigPPI->VerifySig(gpAmiSigPPI, &PubKeyHndl, &HashHndl, pSig, DEFAULT_RSA_SIG_LEN, Flags); 
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  VerifyFwImage
//
// Description:    Function verifies various sections of the FW Capsule 
//
// 0. Locate protocol AMI_CRYPT_DIGITAL_SIGNATURE_PPI
// 1. Check if FV GUID matches Aptio FW Capsule. 
//         - Yes - 
//          update Payload ptr to beginning of BIOS ROM data
//            continue with Image Verify
//         - Not found at offs 0 - assume Cap Hdr is in FFS
//          locate FFS by Hole GUID, Sec GUID
//          update FWCapsuleHdr ptr,
//            continue with Image Verify
// 2. Validate  Root Certificate 
//    -Compare PubKey in Root Certificate Hdr with local FW Platform Key
//    -Hash SignCert.PubKey 
//    -VerifySig for RootCert.Signature and compare with SignCert.Key hash
// 3. Validate  Sign Certificate 
//    -Hash the ROM image inside the FW Capsule
//    -VerifySig for SignCert.Signature and compare with ROM hash
// 4. Update pCapsuleSize = FwPayload and set pCapsule to point to FW Payload
//
//  Input:
//    PeiServices   Pointer to PeiServices instance.
//    pCapsule      Points to the start of the Aptio FW Capsule.
//    pCapsuleSize  The size of buffer, in bytes.
//    FailedVTask   Specifies additional flags to further customize the signing/verifying behavior.
//
// Output:
//    pCapsule      ptr is set to start of Capsule's Payload - ROM image
//    pCapsuleSize  return ROM image size
//    EFI_SUCCESS               The signature is successfully verified.
//    EFI_SECURITY_VIOLATION    The signature does not match the given message.
//    EFI_ACCESS_DENIED         The key could not be used in signature operation.
//    EFI_INVALID_PARAMETER     The size of input message or signature does not meet the criteria 
//                              of the underlying signature algorithm.
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS 
VerifyFwImage(
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID          **pCapsule,
  IN OUT UINT32         *pCapsuleSize,
  IN OUT UINT32         *FailedVTask
){
    EFI_STATUS              Status;
    APTIO_FW_CAPSULE_HEADER  *FWCapsuleHdr;
    UINT8                   *Payload;
    UINTN                   RomSize;
    PEI_CRYPT_HANDLE        PubKeyHndl;

    gPeiServices = PeiServices; 

    DEBUG ((SecureMod_DEBUG_LEVEL, "\nValidating FW Capsule (size %X)...\n", *pCapsuleSize));

    // Predefined bit mask of checks to perform on Aptio FW Capsule
    *FailedVTask = Cap;

    Status = (*PeiServices)->LocatePpi(PeiServices, &gAmiDigitalSignaturePPIGuid, 0, NULL, &gpAmiSigPPI);
    if(EFI_ERROR(Status)) return Status;

    // ignore Verification if FwKey is not detected in the FW.
    // Works with unsigned Aptio.ROM image or Signed ROM with embedded sig.
    PubKeyHndl.BlobSize =  0;
    PubKeyHndl.Blob = NULL;
    Status = gpAmiSigPPI->GetKey(gpAmiSigPPI, &FWkeyGuid, &PubKeyHndl);
    DEBUG ((SecureMod_DEBUG_LEVEL, "Get Platform FW Key (%r), %X (%d bytes)\n", Status, (*(UINT32*)PubKeyHndl.Blob), PubKeyHndl.BlobSize));
    if(EFI_ERROR(Status)) {
#if (defined(REFLASH_INTERACTIVE) && REFLASH_INTERACTIVE==1)
        if(Status == EFI_NOT_FOUND)
            return EFI_SUCCESS;
#endif
        *FailedVTask = Key;
         return Status;
    }
    // FW Capsule presence check
    if(*pCapsule == NULL)
        return EFI_SECURITY_VIOLATION;

    FWCapsuleHdr = *pCapsule;
    Payload = (UINT8*)*pCapsule;

// verify Capsule Mailbox points to FW_CAPSULE hdr
    if(!guidcmp((EFI_GUID*)&FWCapsuleHdr->CapHdr.CapsuleGuid, &FWCapsuleGuid)) 
    {
        // Update Payload to point to beginning of Bios ROM
        Payload = (UINT8*)((UINTN)FWCapsuleHdr + FWCapsuleHdr->RomImageOffset);
        if ((UINTN)Payload < (UINTN)FWCapsuleHdr)
          return EFI_SECURITY_VIOLATION;
    }
    else
    {
// looking FwCap hdr inside BIOS.ROM
        if(EFI_ERROR(FindCapHdrFFS(Payload, (UINT8**)&FWCapsuleHdr)))
            return EFI_SECURITY_VIOLATION;
    }
    RomSize = FWCapsuleHdr->CapHdr.CapsuleImageSize - FWCapsuleHdr->RomImageOffset;
    DEBUG ((SecureMod_DEBUG_LEVEL, "FW Capsule Hdr offs 0x%X\nGUID: %g\nPayload Size: 0x%X\n",
               ((UINTN)FWCapsuleHdr-(UINTN)*pCapsule), &(FWCapsuleHdr->CapHdr.CapsuleGuid), RomSize));

    // Aptio FW Capsule only supporting WIN_CERT_TYPE_EFI_GUID 
    // More Hdr fields sanity checks for buffer overruns
    if((RomSize > *pCapsuleSize) ||
       (FWCapsuleHdr->FWCert.SignCert.Hdr.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) ||
       (FWCapsuleHdr->CapHdr.CapsuleImageSize > FWCAPSULE_IMAGE_SIZE) || 
       (FWCapsuleHdr->CapHdr.HeaderSize > FWCapsuleHdr->CapHdr.CapsuleImageSize) ||
       (FWCapsuleHdr->CapHdr.HeaderSize > FWCapsuleHdr->RomImageOffset) ||
       (FWCapsuleHdr->RomImageOffset > (FWCAPSULE_IMAGE_SIZE-FWCAPSULE_MAX_PAYLOAD_SIZE)) || // 16k is a MAX possible FwCap Hdr size
       (FWCapsuleHdr->RomLayoutOffset > FWCapsuleHdr->RomImageOffset) ||
       ((UINT64)FWCapsuleHdr->FWCert.SignCert.Hdr.Hdr.dwLength + offsetof(APTIO_FW_CAPSULE_HEADER, FWCert) >
         FWCapsuleHdr->RomLayoutOffset)
    )
        return EFI_SECURITY_VIOLATION;

    // If dummy FWkey - skip integrity check - only test the Capsule's structure 
    if(*((UINT32*)(PubKeyHndl.Blob)) == *(UINT32*)FwBadKey) {
        *FailedVTask = Key;
        DEBUG ((SecureMod_DEBUG_LEVEL, "Dummy FW Key detected. Skip image verification...\n"));
    } else 
    {
// Begin Authentication
        if(!guidcmp((EFI_GUID*)&FWCapsuleHdr->FWCert.SignCert.Hdr.CertType, &gEfiCertPkcs7Guid))
            Status = VerifyFwCertPkcs7(FWCapsuleHdr, Payload, RomSize, &PubKeyHndl, FailedVTask);
        else
            Status = VerifyFwCertRsa2048Sha256(FWCapsuleHdr, Payload, RomSize, FailedVTask);

        DEBUG ((SecureMod_DEBUG_LEVEL, "Verify Sign Certificate Sig : %r\n", Status));
        if (EFI_ERROR(Status)) return Status;

        *FailedVTask = 0;
// Local PEI $FID is linked with CspLib. extern FW_VERSION   FwVersionData;
// Find $FID in new Fw FVs. Any instance found should do for us. Use RomMap from Capsule's Hdr
// compare local BB and Main $Fid BIOS Major/Minor revs with New one.
        Status = VerifyFwRevision(FWCapsuleHdr, Payload, FailedVTask);
        DEBUG ((SecureMod_DEBUG_LEVEL, "FW Revision test %r (FailedVTask = %x)\n", Status, *FailedVTask));
    }

    *pCapsule = (UINT32*)Payload;
    *pCapsuleSize = (UINT32)RomSize;

    return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
