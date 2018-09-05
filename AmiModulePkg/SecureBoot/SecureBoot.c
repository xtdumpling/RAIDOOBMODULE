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

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCertificate.h>
#include <Protocol/AmiDigitalSignature.h>

#include <Setup.h>
#include <SecureBoot.h>
#include "TimeStamp.h"

#if !defined(NO_SETUP_COMPILE)

#include <AutoId.h>

#include <Protocol/AMIPostMgr.h> 
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/HiiString.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LegacyBios.h>
#include <Guid/GlobalVariable.h>
#include <Guid/ImageAuthentication.h>
#endif

extern EFI_RUNTIME_SERVICES *pRS;

static EFI_GUID PKeyFileGuid  = //CC0F8A3F-3DEA-4376-9679-5426BA0A907E
{ 0xCC0F8A3F, 0x3DEA,  0x4376, 0x96, 0x79, 0x54, 0x26, 0xba, 0x0a, 0x90, 0x7e };
static EFI_GUID KekFileGuid  =  // {9FE7DE69-0AEA-470a-B50A-139813649189}
{ 0x9fe7de69, 0xaea, 0x470a, 0xb5, 0xa, 0x13, 0x98, 0x13, 0x64, 0x91, 0x89 };
static EFI_GUID DbFileGuid  =  // {FBF95065-427F-47b3-8077-D13C60710998}
{ 0xfbf95065, 0x427f, 0x47b3, 0x80, 0x77, 0xd1, 0x3c, 0x60, 0x71, 0x9, 0x98 };
static EFI_GUID DbxFileGuid  =  // {9D7A05E9-F740-44c3-858B-75586A8F9C8E}
{ 0x9d7a05e9, 0xf740, 0x44c3, 0x85, 0x8b, 0x75, 0x58, 0x6a, 0x8f, 0x9c, 0x8e };
static EFI_GUID DbtFileGuid  =  // {C246FBBF-F75C-43F7-88A6-B5FD0CF1DB7F}
{ 0xC246FBBF, 0xF75C, 0x43F7, 0x88, 0xa6, 0xb5, 0xfd, 0x0c, 0xf1, 0xdb, 0x7f };
static EFI_GUID DbrFileGuid  =  // {5F7CE43D-565A-4420-B4F8-22ECA7245755}
{ 0x5f7ce43d, 0x565a, 0x4420, 0xb4, 0xf8, 0x22, 0xec, 0xa7, 0x24, 0x57, 0x55 };

static EFI_GUID guidSecurity = SECURITY_FORM_SET_GUID;
static EFI_GUID gAmiDeploymentModeNvGuid = AMI_DEPLOYMENT_MODE_VARIABLE_GUID;
static EFI_GUID gAmiVendorKeysNvGuid = AMI_VENDOR_KEYS_NV_VARIABLE_GUID;


typedef enum {
    DbxVar=0,
    DbtVar,
    DbrVar,
    DbVar,
    KekVar,
    PkVar,
    MaxVarCount
} SECURE_BOOT_POLICY_VARIABLES;

static EFI_GUID *SecureVariableFileGuid [] = {
    &DbxFileGuid,
    &DbtFileGuid,
    &DbrFileGuid,
    &DbFileGuid,
    &KekFileGuid,
    &PKeyFileGuid,
    NULL
};

static CHAR16* SecureVariableFileName[] = {
    EFI_IMAGE_SECURITY_DATABASE1,
    EFI_IMAGE_SECURITY_DATABASE2,
    EFI_IMAGE_SECURITY_DATABASE3,
    EFI_IMAGE_SECURITY_DATABASE,
    EFI_KEY_EXCHANGE_KEY_NAME,
    EFI_PLATFORM_KEY_NAME,
    NULL
};
#define SecureBootPolicyVariableCount MaxVarCount
#define SecureVariableSigDatabaseCount 4

static CHAR16* SecureVariableFileNameDefault[] = {
    EFI_DBX_DEFAULT_VARIABLE_NAME,
    EFI_DBT_DEFAULT_VARIABLE_NAME,
    EFI_DBR_DEFAULT_VARIABLE_NAME,
    EFI_DB_DEFAULT_VARIABLE_NAME, 
    EFI_KEK_DEFAULT_VARIABLE_NAME,
    EFI_PK_DEFAULT_VARIABLE_NAME,
    NULL
};

static SECURE_BOOT_SETUP_VAR SecureBootSetup = {
    DEFAULT_SECURE_BOOT_ENABLE, 
    DEFAULT_SECURE_BOOT_MODE, 
    DEFAULT_PROVISION_SECURE_VARS,
    0x0, // obsolete
    LOAD_FROM_OROM,
    LOAD_FROM_REMOVABLE_MEDIA,
    LOAD_FROM_FIXED_MEDIA};

typedef enum { Absent=0, Custom, Vendor, Mixed, AmiTest} SECURE_VAR_PRESENT_TYPE;
static UINT8 bKey[SecureBootPolicyVariableCount] = {Absent, Absent, Absent, Absent, Absent, Absent};

static AMI_DIGITAL_SIGNATURE_PROTOCOL *mAmiSig = NULL;
static UINT8 mDigest[SHA256_DIGEST_SIZE]={0};
#define MAX_ELEM_NUM        28 // ~28 max number of PE hdr elements to be hashed in one pass

#if AMI_CRYPTOPACKAGE_MODULE_REVISION < 24
static UINT8 *gDstAddress = NULL;
static UINTN  gHeapSize   = CR_DXE_MAX_HEAP_SIZE;
#endif

typedef enum { RESET_NV_KEYS=1, SET_NV_DEFAULT_KEYS=2, SET_RT_DEFAULT_KEYS=4};
// 
//
// AMI_EFI_VARIABLE_AUTHENTICATION_2 descriptor
//
// A time-based authentication method descriptor template
//
#pragma pack(1)
#ifndef AMI_EFI_VARIABLE_AUTHENTICATION_2
typedef struct {
    EFI_TIME                            TimeStamp;
    WIN_CERTIFICATE_UEFI_GUID_1         AuthInfo;
} AMI_EFI_VARIABLE_AUTHENTICATION_2;
#endif
typedef struct {
    AMI_EFI_VARIABLE_AUTHENTICATION_2   AuthHdr;
    EFI_SIGNATURE_LIST                  SigList;
    EFI_SIGNATURE_DATA                  SigData;
} EFI_VARIABLE_SIG_HDR_2;
#pragma pack()

#define EFI_CERT_TYPE_RSA2048_SIZE        256
#define EFI_CERT_TYPE_SHA256_SIZE         32
#define EFI_CERT_TYPE_CERT_X509_SHA256_GUID_SIZE        48
#define EFI_CERT_TYPE_CERT_X509_SHA384_GUID_SIZE        64
#define EFI_CERT_TYPE_CERT_X509_SHA512_GUID_SIZE        80

// InstallVars
#define SET_SECURE_VARIABLE_DEL 1
#define SET_SECURE_VARIABLE_SET 2
#define SET_SECURE_VARIABLE_APPEND 4

#define StrMaxSize 200
static CHAR16 StrTitle[StrMaxSize], StrMessage[StrMaxSize];
static CHAR16 StrTemp[StrMaxSize];
//
#if !defined( TSE_FOR_APTIO_4_50) && !defined(NO_SETUP_COMPILE)
//
static AMI_POST_MANAGER_PROTOCOL *mPostMgr = NULL;

typedef struct
{
    UINT64 Type;
    UINTN Size;
    CHAR16 *Name;
    STRING_REF Token;
} FILE_TYPE;

static EFI_HII_STRING_PROTOCOL *HiiString = NULL;
static EFI_HII_HANDLE gHiiHandle;

static STRING_REF SecureVariableStringTitle[] = {
    STRING_TOKEN(STR_DBX_TITLE),
    STRING_TOKEN(STR_DBT_TITLE),
    STRING_TOKEN(STR_DBR_TITLE),
    STRING_TOKEN(STR_DB_TITLE),
    STRING_TOKEN(STR_KEK_TITLE),
    STRING_TOKEN(STR_PK_TITLE),
    0
};
static STRING_REF SecureVariableStringRef[] = {
    STRING_TOKEN(STR_DBX_TITLE_TEXT),
    STRING_TOKEN(STR_DBT_TITLE_TEXT),
    STRING_TOKEN(STR_DBR_TITLE_TEXT),
    STRING_TOKEN(STR_DB_TITLE_TEXT),
    STRING_TOKEN(STR_KEK_TITLE_TEXT),
    STRING_TOKEN(STR_PK_TITLE_TEXT),
    0
};
EFI_STATUS DevicePathToStr(EFI_DEVICE_PATH_PROTOCOL *Path,CHAR8    **Str);
EFI_STATUS FileBrowserLaunchFileSystem(BOOLEAN bSelectFile, OUT EFI_HANDLE **outFsHandle, OUT CHAR16 **outFilePath, OUT UINT8 **outFileBuf,OUT UINTN *size );
VOID GetHiiString(IN EFI_HII_HANDLE HiiHandle, IN STRING_REF Token, UINTN DataSize, CHAR16  * pData);
VOID SetAppendSecureBootDBCallback(EFI_HII_HANDLE HiiHandle, UINT8 VarSetMode, UINT8 index);
VOID DeleteSecureBootDBCallback(EFI_HII_HANDLE HiiHandle, UINT8 index);
//VOID GetSecureBootVarCertificateInfo(EFI_HII_HANDLE HiiHandle, UINT8 index);
#endif // no Setup link

#ifdef TSE_FOR_APTIO_4_50
extern BOOLEAN gBrowserCallbackEnabled;
#else
BOOLEAN gBrowserCallbackEnabled = FALSE;
#endif //#ifndef TSE_FOR_APTIO_4_50

//----------------------------------------------------------------------------
// Function forward declaration
//----------------------------------------------------------------------------
VOID UpdateSecureVariableBrowserInfo();
VOID UpdateSecureBootBrowserInfo ();
EFI_STATUS InstallSecureVariables (UINT16);
VOID UpdateSecureBootSetupVariable(UINT8);

EFI_STATUS ValidateSignatureList (
    IN VOID         *Data,
    IN UINTN        DataSize,
    IN OUT UINTN    *SigCount OPTIONAL
    );

//----------------------------------------------------------------------------
#if !defined(NO_SETUP_COMPILE)

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   UpdateSecureBootBrowserInfo
//
// Description: Update Secure Boot flags status
//
// Input:       none
//
// Output:      none
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UpdateSecureBootBrowserInfo ()
{
    UINT8       Byte;
    UINTN       Size;
    UINT8       i;
    BOOLEAN     tmpBrowserCallbackEnabled = gBrowserCallbackEnabled;

    gBrowserCallbackEnabled = TRUE;
    Size = sizeof(bKey);
    pRS->GetVariable(AMI_SECURE_VAR_PRESENT_VAR, &guidSecurity, NULL, &Size, bKey);
    TRACE((TRACE_ALWAYS,"\n%S:",AMI_SECURE_VAR_PRESENT_VAR));
    for(i=0;i<Size;i++) TRACE((TRACE_ALWAYS," %x:",bKey[i]));
    TRACE((TRACE_ALWAYS,"\n"));
    HiiLibSetBrowserData( sizeof(bKey), &bKey, &guidSecurity, AMI_SECURE_VAR_PRESENT_VAR);
    Size = 1;
    pRS->GetVariable(EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &Byte);
    TRACE((TRACE_ALWAYS,"%S =%x\n",EFI_SETUP_MODE_NAME,  Byte));
    HiiLibSetBrowserData( Size, &Byte, &gEfiGlobalVariableGuid, EFI_SETUP_MODE_NAME);
    Size = 1;
    pRS->GetVariable(EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &Byte);
    TRACE((TRACE_ALWAYS,"%S=%x\n", EFI_SECURE_BOOT_MODE_NAME, Byte));
    HiiLibSetBrowserData(Size, &Byte, &gEfiGlobalVariableGuid, EFI_SECURE_BOOT_MODE_NAME);
    Size = 1;
    pRS->GetVariable(EFI_VENDOR_KEYS_VARIABLE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &Byte);
    TRACE((TRACE_ALWAYS,"%S=%x\n",EFI_VENDOR_KEYS_VARIABLE_NAME,  Byte));
    HiiLibSetBrowserData(Size, &Byte, &gEfiGlobalVariableGuid, EFI_VENDOR_KEYS_VARIABLE_NAME);
    // Update Audit/Deployed Vars.
    Size = 1;
    pRS->GetVariable(EFI_AUDIT_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &Byte);
    TRACE((TRACE_ALWAYS,"%S=%x\n",EFI_AUDIT_MODE_NAME,  Byte));
    HiiLibSetBrowserData(Size, &Byte, &gEfiGlobalVariableGuid, EFI_AUDIT_MODE_NAME);
    Size = 1;
    pRS->GetVariable(EFI_DEPLOYED_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &Byte);
    TRACE((TRACE_ALWAYS,"%S=%x\n",EFI_DEPLOYED_MODE_NAME,  Byte));
    HiiLibSetBrowserData(Size, &Byte, &gEfiGlobalVariableGuid, EFI_DEPLOYED_MODE_NAME);
    gBrowserCallbackEnabled = tmpBrowserCallbackEnabled;
    // dbg    
    Size = 1;
    pRS->GetVariable(AMI_DEPLOYMENT_MODE_VARIABLE_NAME, &gAmiDeploymentModeNvGuid, NULL, &Size, &Byte);
    TRACE((TRACE_ALWAYS,"%S=%x\n",AMI_DEPLOYMENT_MODE_VARIABLE_NAME,  Byte));
    Size = 1;
    pRS->GetVariable(EFI_VENDOR_KEYS_NV_VARIABLE_NAME, &gAmiVendorKeysNvGuid, NULL, &Size, &Byte);
    TRACE((TRACE_ALWAYS,"%S=%x\n",EFI_VENDOR_KEYS_NV_VARIABLE_NAME,  Byte));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   UpdateSecureVariableBrowserInfo
//
// Description: Detect 5 EFI Variables: PK, KEK, db, dbt, dbx, dbr
//
// Input:       none
//
// Output:      none
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UpdateSecureVariableBrowserInfo()
{
    EFI_STATUS  Status;
    UINT8       Index;
    EFI_GUID    *EfiVarGuid;
    UINTN       SigCount;
    UINTN       Size, Size2, Size3;
    UINT8       *Buf1, *Buf2;

// Ami Test PK Key, n-modulus RSA2048
static UINT8 AmiPkKeyHash1[SHA256_DIGEST_SIZE] = {
      0xa9, 0x51, 0x19, 0x2f, 0x10, 0xe3, 0x90, 0x51, 0xf8, 0x0a, 0xfb, 0xc3, 0x83, 0x0e, 0x9f, 0xc9,
      0x10, 0xe7, 0xa5, 0x49, 0x20, 0xbe, 0x50, 0xf9, 0xb8, 0x04, 0xd6, 0x7e, 0x88, 0x15, 0x96, 0xd9
    };
// Ami Test PK Key, x509 Certificate
static UINT8 AmiPkKeyHash2[SHA256_DIGEST_SIZE]={
      0xcc, 0xa4, 0xe3, 0xf3, 0x17, 0x02, 0x30, 0x03, 0x0d, 0xc3, 0xe3, 0x3d, 0x1e, 0x3f, 0xa7, 0xd1,
      0x38, 0x3d, 0xe8, 0xb3, 0x36, 0x74, 0x30, 0x89, 0x2e, 0x93, 0xcb, 0xcc, 0xde, 0x03, 0x4c, 0xe0
    };
// AMI Test KEK (TBD)

    UINT8  *CertStart;
    UINTN   CertLen;
    EFI_SIGNATURE_LIST *SigList;
    EFI_SIGNATURE_DATA *SigData;
    INTN err;

    Index = 0;
    while(SecureVariableFileName[Index] != NULL)
    {
        if(Index < SecureVariableSigDatabaseCount)
            EfiVarGuid = &gEfiImageSecurityDatabaseGuid;
        else
            EfiVarGuid = &gEfiGlobalVariableGuid;

        Size = 0;
        Size2 = 0;
        SigCount = 0;
        bKey[Index] = Absent;
        //get var size
        Status = pRS->GetVariable( SecureVariableFileName[Index], EfiVarGuid, NULL, &Size, NULL);
        TRACE((TRACE_ALWAYS,"NV Var %S(%d),  status=%r\n",  SecureVariableFileName[Index], Size, Status));
        if(Status == EFI_BUFFER_TOO_SMALL) {
            bKey[Index] = Custom;
//
// enhanced logic for vendor/custom key detection
// 
            Buf1 = NULL;
            Status = pBS->AllocatePool(EfiBootServicesData, Size, &Buf1);
            ASSERT_EFI_ERROR (Status);
//read Key var to a buffer
            Status = pRS->GetVariable( SecureVariableFileName[Index], EfiVarGuid, NULL, &Size, Buf1);
            if(!EFI_ERROR(Status)) {
#if !defined( TSE_FOR_APTIO_4_50)
                ValidateSignatureList (Buf1, Size, &SigCount);
#endif
                //get Default Key from Volatile key var (use Key size)
                Buf2 = NULL;
                Status = pBS->AllocatePool(EfiBootServicesData, Size, &Buf2);
                ASSERT_EFI_ERROR (Status);
                EfiVarGuid = &gEfiGlobalVariableGuid;
                Size2 = Size;
                Status = pRS->GetVariable( SecureVariableFileNameDefault[Index], EfiVarGuid, NULL, &Size2, Buf2);
                TRACE((TRACE_ALWAYS,"RT Var %S(%d),  status=%r\n",  SecureVariableFileName[Index], Size2, Status));
    // search for matching patterns of buf2, sz 2 inside of buf1
                if(!EFI_ERROR(Status)) {
                    SigList = (EFI_SIGNATURE_LIST *)Buf1;
                    Size3 = 0;
                    while((Size3 + Size2) <= Size) {
                        if(!MemCmp(SigList, Buf2, Size2) ) {
                            bKey[Index] = Vendor;      //vendor
                            if(Size > Size2)
                               bKey[Index] = Mixed;   // vendor+extra keys
                            break;
                        }
                        Size3+=SigList->SignatureListSize;
                        SigList =(EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
                    }
                }
                if(Buf2) pBS->FreePool(Buf2);
//
// Check for AMI Test key in Secure Boot vars
//
  //          if(Wcscmp(SecureVariableFileName[Index], EFI_PLATFORM_KEY_NAME) == 0) {
// Ami Test Rsa2048 key
                // Verify the PK SignatureType GUID
                SigList = (EFI_SIGNATURE_LIST *)Buf1;
                Size3 = 0;
                while(Size3 < Size) {
                    TRACE((TRACE_ALWAYS,"SigList->SignatureType %g\n",  &(SigList->SignatureType)));
                    if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), &gEfiCertRsa2048Guid) 
                        ||
                        !guidcmp ((EFI_GUID*) &(SigList->SignatureType), &gEfiCertX509Guid)
                    ){
                        SigData   = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigList + sizeof (EFI_SIGNATURE_LIST) + SigList->SignatureHeaderSize);
                        CertStart = (UINT8*)SigData->SignatureData;
                        CertLen = (UINTN)SigList->SignatureSize-sizeof(EFI_GUID);
                        TRACE((TRACE_ALWAYS, "CertStartOffs %x, CertLen %x\n", (UINT32)(CertStart-Buf1), CertLen));
                        if(mAmiSig == NULL)    {
                            Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &mAmiSig); 
                        }
                        if(mAmiSig) {
                            Status = mAmiSig->Hash(mAmiSig, &gEfiHashAlgorithmSha256Guid, 
                                    1, &CertStart, (const UINTN*)&CertLen, mDigest);
                            err = MemCmp(AmiPkKeyHash1, mDigest, SHA256_DIGEST_SIZE);
                            TRACE((TRACE_ALWAYS, "Compare Key[%d]_1 == FW Key : %s\n", 1,(!err)?"Yes":"No"));
                            if(err) {
                                err = MemCmp(AmiPkKeyHash2, mDigest, SHA256_DIGEST_SIZE);
                                TRACE((TRACE_ALWAYS, "Compare Key[%d]_2 == FW Key : %s\n", 1,(!err)?"Yes":"No"));
                            }
                            if(!err) {
                               bKey[Index] = AmiTest;   // AMI Test PK
                               break;
                            }
                        }
                    }
                    Size3+=SigList->SignatureListSize;
                    SigList =(EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
                }
//            }
                if(Buf1) pBS->FreePool(Buf1);
            } 
        }  
#if !defined( TSE_FOR_APTIO_4_50)
        switch(bKey[Index]) 
        {
            case Custom: GetHiiString(gHiiHandle, STRING_TOKEN(STR_CUSTOM_KEY), sizeof(StrMessage), StrMessage);
                break;
            case Vendor: GetHiiString(gHiiHandle, STRING_TOKEN(STR_DEFAULT_KEY), sizeof(StrMessage), StrMessage);
                break;
            case Mixed: GetHiiString(gHiiHandle, STRING_TOKEN(STR_MIXED_KEY), sizeof(StrMessage), StrMessage);
                break;
            case AmiTest: GetHiiString(gHiiHandle, STRING_TOKEN(STR_AMI_TEST_KEY), sizeof(StrMessage), StrMessage);
                break;
            case Absent:
            default: GetHiiString(gHiiHandle, STRING_TOKEN(STR_ABSENT_KEY), sizeof(StrMessage), StrMessage);
                break;
        }
        MemSet(StrTemp, sizeof(StrTemp), 0);
        GetHiiString(gHiiHandle, SecureVariableStringRef[Index], sizeof(StrTemp), StrTemp);
        Swprintf_s(StrTitle, sizeof(StrTitle), StrTemp, Size, SigCount, StrMessage);
//        TRACE((TRACE_ALWAYS,"StrTitle '%S'\n", StrTitle));
        HiiLibSetString(gHiiHandle, SecureVariableStringTitle[Index], StrTitle);
#endif
        Index++;
    }
    pRS->SetVariable(AMI_SECURE_VAR_PRESENT_VAR, &guidSecurity, EFI_VARIABLE_BOOTSERVICE_ACCESS, sizeof(bKey), &bKey);

    UpdateSecureBootBrowserInfo();
}
#endif //!defined(NO_SETUP_COMPILE)

#ifndef TSE_FOR_APTIO_4_50
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   UpdateSecureBootSetupVariable
//
// Description: Update Browser Info with Vendor Key status change
//
// Input:       UINT8 CustomMode change
//
// Output:      none
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UpdateSecureBootSetupVariable( 
    IN UINT8 CustomMode
    )
{
    EFI_STATUS  Status;
    UINTN       Size;
    UINT32      Attributes;
    BOOLEAN     tmpBrowserCallbackEnabled = gBrowserCallbackEnabled;
    SECURE_BOOT_SETUP_VAR SecureBootSetup1;

    Size = sizeof(SECURE_BOOT_SETUP_VAR);
    gBrowserCallbackEnabled = TRUE;
    Status = HiiLibGetBrowserData( &Size, &SecureBootSetup, &guidSecurity, AMI_SECURE_BOOT_SETUP_VAR);
    gBrowserCallbackEnabled = tmpBrowserCallbackEnabled;
    if(!EFI_ERROR (Status)) {
        pRS->GetVariable (AMI_SECURE_BOOT_SETUP_VAR, &guidSecurity, &Attributes, &Size, &SecureBootSetup1);
        SecureBootSetup.SecureBootMode = CustomMode;
        if(MemCmp(&SecureBootSetup, &SecureBootSetup1, Size) != 0) {
            Status = pRS->SetVariable (AMI_SECURE_BOOT_SETUP_VAR, &guidSecurity, Attributes, Size, &SecureBootSetup);
            TRACE((TRACE_ALWAYS,"Update %S Var %r\n", AMI_SECURE_BOOT_SETUP_VAR, Status));
        }
    }
}
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  CryptoGetRawImage
//
// Description:    Loads binary from RAW section of X firmware volume
//
//  Input:
//               NameGuid  - The guid of binary file
//               Buffer    - Returns a pointer to allocated memory. Caller must free it when done.
//               Size      - Returns the size of the binary loaded into the buffer.
//
// Output:         Buffer - returns a pointer to allocated memory. Caller
//                        must free it when done.
//               Size  - returns the size of the binary loaded into the
//                        buffer.
//               EFI_NOT_FOUND  - Can't find the binary.
//               EFI_LOAD_ERROR - Load fail.
//               EFI_SUCCESS    - Load success.
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
CryptoGetRawImage (
  IN      EFI_GUID       *NameGuid,
  IN OUT  VOID           **Buffer,
  IN OUT  UINTN          *Size
  )
{
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  EFI_HANDLE                    *HandleBuff;
  UINT32                        AuthenticationStatus;

 *Buffer=0;
 *Size=0;
  Status = pBS->LocateHandleBuffer (ByProtocol,&gEfiFirmwareVolume2ProtocolGuid,NULL,&HandleCount,&HandleBuff);
  if (EFI_ERROR (Status) || HandleCount == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = pBS->HandleProtocol (HandleBuff[Index],&gEfiFirmwareVolume2ProtocolGuid,&Fv);

    if (EFI_ERROR (Status)) {
       continue;//return EFI_LOAD_ERROR;
    }
    //
    // Try a raw file
    //
    Status = Fv->ReadSection (
                  Fv,
                  NameGuid,
                  EFI_SECTION_RAW,
                  0,    //Instance
                  Buffer,
                  Size,
                  &AuthenticationStatus
                  );

    if (Status == EFI_SUCCESS) break;
  }

  pBS->FreePool(HandleBuff);

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   InstallSecureVariables
//
// Description: Install 5 EFI Variables: PK, KEK, db, dbt, dbx, dbr
//
// Input:       BOOLEAN InstallVars
//                  TRUE  - attempt to install secure variables
//                  FALSE - erase secure variables
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
InstallSecureVariables (
    UINT16    InstallVars
){
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8      *pVarBuffer = NULL;
    UINTN       Size, FileSize, Offset;
    UINT8       Index;
    EFI_GUID    *EfiVarGuid;
    UINT32      Attributes;
    AMI_EFI_VARIABLE_AUTHENTICATION_2 *AuthHdr2;

///////////////////////////////////////////////////////////////////////////////
// Initial provisioning of Authenticated non-volatile EFI Variables 
///////////////////////////////////////////////////////////////////////////////
    Attributes = EFI_VARIABLE_RUNTIME_ACCESS |
                 EFI_VARIABLE_BOOTSERVICE_ACCESS; 
    Index = 0;
    while(/*!EFI_ERROR(Status) &&*/ SecureVariableFileName[Index] != NULL)
    {
        if(Index < SecureVariableSigDatabaseCount)
            EfiVarGuid = &gEfiImageSecurityDatabaseGuid;
        else
            EfiVarGuid = &gEfiGlobalVariableGuid;
// if SET_NV_DEFAULT_KEYS set
/*
1. check if File is present CryptoGetRawImage
2. if not - skip to next var
3. if present -> move to Erase... 
*/        
        if(InstallVars & (SET_NV_DEFAULT_KEYS | SET_RT_DEFAULT_KEYS)) {
            pVarBuffer = NULL;
            FileSize = 0 ; 
            Status = CryptoGetRawImage( SecureVariableFileGuid[Index], &pVarBuffer, (UINTN*)&FileSize);
            TRACE((TRACE_ALWAYS,"Get Raw image %S, Status %r, sz = %x\n",SecureVariableFileName[Index], Status, FileSize));
            if(EFI_ERROR(Status)) {
                Index++;
                continue;
            }
        }
        if((InstallVars & RESET_NV_KEYS)== RESET_NV_KEYS) {
        // try to erase. should succeed if system in pre-boot and Admin mode
            Status = pRS->SetVariable(SecureVariableFileName[Index],EfiVarGuid,0,0,NULL);
            TRACE((TRACE_ALWAYS,"Clear NV Var %S, Status %r\n",SecureVariableFileName[Index], Status));
            if(Status == EFI_NOT_FOUND) 
                Status = EFI_SUCCESS;
        }
        if((InstallVars & (SET_NV_DEFAULT_KEYS | SET_RT_DEFAULT_KEYS)) &&
            pVarBuffer && FileSize ) {
            if(InstallVars & (SET_RT_DEFAULT_KEYS)) {
                AuthHdr2 = (AMI_EFI_VARIABLE_AUTHENTICATION_2*)pVarBuffer;
                Offset = sizeof(EFI_TIME) + AuthHdr2->AuthInfo.Hdr.dwLength;
                Status = pRS->SetVariable(SecureVariableFileNameDefault[Index],
                       &gEfiGlobalVariableGuid,
                        Attributes,
                        FileSize - Offset,
                        (UINT8*)pVarBuffer + Offset
                );
                TRACE((TRACE_ALWAYS,"Set RT Var %S, Status %r\n",SecureVariableFileNameDefault[Index], Status));
            }
            if(InstallVars & (SET_NV_DEFAULT_KEYS)) {
                Size = 0;
                Status = pRS->GetVariable( SecureVariableFileName[Index], EfiVarGuid, NULL, &Size, NULL);
                if(EFI_ERROR(Status) && Status == EFI_NOT_FOUND ) {
                    Status = pRS->SetVariable(SecureVariableFileName[Index],
                            EfiVarGuid,
                            (UINT32)(Attributes | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS),
                            FileSize,
                            pVarBuffer
                    );
                    TRACE((TRACE_ALWAYS,"Set NV Var %S, Status %r\n",SecureVariableFileName[Index], Status));
                }
            }
            pBS->FreePool(pVarBuffer);
        }
        Index++;
    }

    return Status;
}

// only linked with the Setup
#if /* not defined( TSE_FOR_APTIO_4_50) &&*/ !defined(NO_SETUP_COMPILE)

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   FillAuthHdr
//
// Description: 
//
// Input:       NONE
//
// Output:      NONE
//
// Returns:     NONE
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
static VOID FillAuthHdr(
    UINT8*  pVar,
    UINT8   VarSetMode
)
{
    AMI_EFI_VARIABLE_AUTHENTICATION_2 *AuthHdr2;
    static EFI_TIME EfiTime = {
    FOUR_DIGIT_YEAR_INT,
    TWO_DIGIT_MONTH_INT,
    TWO_DIGIT_DAY_INT,
    TWO_DIGIT_HOUR_INT,
    TWO_DIGIT_MINUTE_INT,
    TWO_DIGIT_SECOND_INT,0,0,0,0,0};

    AuthHdr2 = (AMI_EFI_VARIABLE_AUTHENTICATION_2*)pVar;
    MemCpy (&AuthHdr2->TimeStamp, &EfiTime, sizeof (EFI_TIME));
    if((VarSetMode & SET_SECURE_VARIABLE_APPEND)== SET_SECURE_VARIABLE_APPEND)
        AuthHdr2->TimeStamp.Year = 2000; // append should not change the original date the var was created
    AuthHdr2->AuthInfo.Hdr.dwLength = sizeof(WIN_CERTIFICATE_UEFI_GUID_1);
    AuthHdr2->AuthInfo.Hdr.wRevision = 0x200;
    AuthHdr2->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
    AuthHdr2->AuthInfo.CertType = gEfiCertPkcs7Guid;

    return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetHiiString
//
// Description: This function Reads a String from HII
//
// Input:       IN EFI_HII_HANDLE   HiiHandle - Efi Hii Handle
//              IN STRING_REF       Token     - String Token
//              IN OUT UINTN        *pDataSize - Length of the StringBuffer
//              OUT EFI_STRING      *ppData - The buffer to receive the characters in the string.
//
// Output:      EFI_STATUS - Depending on result
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID GetHiiString(
    IN EFI_HII_HANDLE HiiHandle,
    IN STRING_REF Token,
    IN  UINTN DataSize,
    IN OUT CHAR16  *ppData
    )
{
    EFI_STATUS Status;

    if (!ppData) return;

    Status = HiiLibGetString(HiiHandle, Token, &DataSize, (EFI_STRING)ppData);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status))  Wcscpy(ppData,(CHAR16*)L"??? ");

//TRACE((TRACE_ALWAYS,"%r, StrRef '%S', Size %d, Token=%d\n",Status, ppData, DataSize, Token));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   FillAuthVarHdr
//
// Description: 
//
// Input:       NONE
//
// Output:      NONE
//
// Returns:     NONE
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
static VOID FillAuthVarHdr(
    UINT8 *pVar,
    UINT8 *pCert,
    UINTN CertSize,
    UINT8 VarSetMode
)
{
    EFI_VARIABLE_SIG_HDR_2 *AuthHdr2;    
    static EFI_GUID    AmiSigOwner = AMI_APTIO_SIG_OWNER_GUID;
    EFI_GUID          *pGuid;
    struct x509_certificate *x509SignCert = NULL;

    AuthHdr2 = (EFI_VARIABLE_SIG_HDR_2*)pVar;

    FillAuthHdr(pVar, VarSetMode); // Append AuthHdr to Var data.

    if(CertSize == EFI_CERT_TYPE_SHA256_SIZE)
        pGuid = &gEfiCertSha256Guid;
    if(CertSize == EFI_CERT_TYPE_CERT_X509_SHA256_GUID_SIZE)
        pGuid = &gEfiCertX509Sha256Guid;
    if(CertSize == EFI_CERT_TYPE_CERT_X509_SHA384_GUID_SIZE)
        pGuid = &gEfiCertX509Sha384Guid;
    if(CertSize == EFI_CERT_TYPE_CERT_X509_SHA512_GUID_SIZE)
        pGuid = &gEfiCertX509Sha512Guid;
    if(CertSize == EFI_CERT_TYPE_RSA2048_SIZE)
        pGuid = &gEfiCertRsa2048Guid;
    if(CertSize > EFI_CERT_TYPE_RSA2048_SIZE) {
/*      if(mAmiSig == NULL)
            Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &mAmiSig); 
        TRACE((TRACE_ALWAYS, "mAmiSig %x,  CertLen %x\n", mAmiSig, CertSize));
        if(mAmiSig == NULL)
            return;
        Status = mAmiSig->Pkcs7Verify (
                mAmiSig,
                pCert, CertSize,
                pCert, CertSize,
                NULL, NULL,
                x509ValidateCertChain,
                RESET // Flags 
        );
        if(EFI_ERROR(Status)) 
            return;
*/
#if AMI_CRYPTOPACKAGE_MODULE_REVISION < 24
        if(gDstAddress == NULL) {
            if(EFI_ERROR(pBS->AllocatePool(EfiRuntimeServicesData, gHeapSize, (void**)&gDstAddress))) {
                return;
            }
            InitCRmm( (void*)gDstAddress, gHeapSize);
        }
#endif        
        ResetCRmm();
        x509SignCert = x509_certificate_parse(pCert, CertSize);
        if(x509SignCert == NULL)
            return;

        pGuid = &gEfiCertX509Guid;
    }
    
//      CopyMem (&AuthHdr2->SigList.SignatureType, *pGuid, sizeof (EFI_GUID));
    AuthHdr2->SigList.SignatureType = *pGuid;
    AuthHdr2->SigList.SignatureSize = (UINT32)(sizeof(EFI_GUID)+CertSize);
    AuthHdr2->SigList.SignatureListSize = AuthHdr2->SigList.SignatureSize+sizeof(EFI_SIGNATURE_LIST);
    AuthHdr2->SigList.SignatureHeaderSize = 0;
    AuthHdr2->SigData.SignatureOwner = AmiSigOwner;

TRACE((TRACE_ALWAYS,"SigList Type=%g\nSigSize=%x\nListSize=%x\n", &(AuthHdr2->SigList.SignatureType), AuthHdr2->SigList.SignatureSize, AuthHdr2->SigList.SignatureListSize));

//    MemCpy(AuthHdr2->SigData.SignatureData, pCert, CertSize);

    return;
}

/**
  Validate the data payload begins with valid Signature List header
  and based on the results returns Status.
        
  @param[in]   VOID *Data - pointer to the Var data
  @param[in]   UINTN DataSize - size of Var data
  @param[out]  *SigCount - Number of Signatures encountered inside the Data
  
  @return EFI_SECURITY_VIOLATION          The Signature does NOT pass the validation
                                          check carried out by the firmware.
  @return EFI_SUCCESS                     Data passed validation successfully.

**/
EFI_STATUS ValidateSignatureList (
    IN VOID         *Data,
    IN UINTN        DataSize,
    IN OUT UINTN    *SigCount OPTIONAL
)
{
    EFI_STATUS          Status;
    EFI_SIGNATURE_LIST *SigList;
    UINTN               Index, count, SigSize;
    static EFI_GUID mSignatureSupport[SIGSUPPORT_NUM] = {SIGSUPPORT_LIST};

    if(DataSize == 0 || Data == NULL)
        return EFI_SECURITY_VIOLATION; // Sig not found

// loop till end of DataSize for all available SigLists
// Verify signature is one from SigDatabase list mSignatureSupport / sizeof(EFI_GUID)
// SigData begins with SigOwner GUID
// SignatureHdrSize = 0 for known Sig Types

    SigList  = (EFI_SIGNATURE_LIST *)Data;
    Status = EFI_SECURITY_VIOLATION;
    count = 0;
    while ((DataSize > 0) && (DataSize >= SigList->SignatureListSize)) {

        for (Index = 0; Index < SIGSUPPORT_NUM; Index++) {
            if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), &mSignatureSupport[Index]))
                break;
        }
TRACE((TRACE_ALWAYS,"SigList.Type(%g) -", &(SigList->SignatureType)));
        if(Index >= SIGSUPPORT_NUM)
            return EFI_SECURITY_VIOLATION; // Sig not found

TRACE((TRACE_ALWAYS,"OK\nSigList.Size(4C < %X < %X)-",SigList->SignatureListSize, NVRAM_SIZE));
        if(SigList->SignatureListSize < 0x4C || // Min size for SHA2 Hash Certificate sig list
           SigList->SignatureListSize > NVRAM_SIZE)
            return EFI_SECURITY_VIOLATION; 

TRACE((TRACE_ALWAYS,"OK\nSigList.HdrSize(%X)-",SigList->SignatureHeaderSize));
        if(SigList->SignatureHeaderSize != 0)
            return EFI_SECURITY_VIOLATION; // Sig not found

TRACE((TRACE_ALWAYS,"OK\nSigList.SigSize(%X)\n",SigList->SignatureSize));
        for(SigSize=sizeof(EFI_SIGNATURE_LIST); SigSize < SigList->SignatureListSize; SigSize+=SigList->SignatureSize)
            count++;
            
TRACE((TRACE_ALWAYS,"Sig.Count(%d)\n", count));
        DataSize -= SigList->SignatureListSize;
        SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
        TRACE((TRACE_ALWAYS,"DataSize left(%X)\n", DataSize));

        Status = EFI_SUCCESS;
    }
    if(SigCount)
        *SigCount = count;
    
    return Status;
}
/**
   For variables with GUID EFI_IMAGE_SECURITY_DATABASE_GUID 
  (i.e. where the data buffer is formatted as EFI_SIGNATURE_LIST), 
   the driver shall not perform an append of EFI_SIGNATURE_DATA values 
   that are already part of the existing variable value 
   Note: This situation is not considered an error, and shall in itself not cause a status code other 
   than EFI_SUCCESS to be returned or the timestamp associated with the variable not to be updated

  @param[in]  VOID  *Data - pointer to data block within AutVar Data
  @param[in]  UINTN *DataSize - ptr to size of data block
  @param[in]  VOID  *SigDb - current SigDb
  @param[in]  UINTN  SigDbSize 

  @return EFI_ALREADY_STARTED           Signature is already present in current Signature Database
  @return EFI_SUCCESS                   New signature can be appended to a current signature list

**/
/*
EFI_STATUS ModifySignatureList (
    IN VOID         *Data,
    IN OUT UINTN    *DataSize,
    IN VOID         *SigDb,
    IN UINTN        SigDbSize
){
    EFI_SIGNATURE_LIST             *SigList;
    EFI_SIGNATURE_LIST             *SigListNew;
    EFI_SIGNATURE_DATA             *SigItem;
    EFI_SIGNATURE_DATA             *SigItemNew;
    UINT32                          SigCount;
    UINT32                          Index;
    UINT32                          SigCountNew;
    UINT32                          IndexNew;
    UINTN                           SigNewSize;
    BOOLEAN                         bSigMatch;

    if (SigDb && SigDbSize && (Data != SigDb) &&
        // Validate Signature List integrity 
        !EFI_ERROR(ValidateSignatureList (Data, *DataSize, NULL))
    ) {
        SigList     = (EFI_SIGNATURE_LIST *)SigDb;
        SigListNew  = (EFI_SIGNATURE_LIST *)Data;
TRACE((TRACE_ALWAYS,"FindInDB(x)\nDataSize In %d (0x%X)\n",*DataSize,*DataSize));
        //
        // Enumerate all Sig items in this list to verify the variable certificate data.
        //
        //
        // scan through multiple Sig Lists in DB exist.
        while ((SigDbSize > 0) && (SigDbSize >= SigList->SignatureListSize)) {
            SigCount  = (SigList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - SigList->SignatureHeaderSize) / SigList->SignatureSize;
            SigItem   = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigList + sizeof (EFI_SIGNATURE_LIST) + SigList->SignatureHeaderSize);
//TRACE((TRACE_ALWAYS,"Org SigList Count: %d, SigSize %X\n", SigCount, SigList->SignatureSize));
            // scan through multiple Sig Lists in NewSigList.
            for (Index = 1; Index <= SigCount; Index++) {
// TRACE((TRACE_ALWAYS,"OrgCert %d, Data %X\n",Index, *(UINT32*)SigItem->SignatureData));
                SigListNew  = (EFI_SIGNATURE_LIST *)Data;
                SigNewSize  = *DataSize;
                while ((SigNewSize > 0) && (SigNewSize >= SigListNew->SignatureListSize)) {
                    bSigMatch = FALSE;
                    SigItemNew = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigListNew + sizeof (EFI_SIGNATURE_LIST) + SigListNew->SignatureHeaderSize);
                    SigCountNew  = (SigListNew->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - SigListNew->SignatureHeaderSize) / SigListNew->SignatureSize;
                    if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), (EFI_GUID*)&(SigListNew->SignatureType)) && 
                        SigList->SignatureSize == SigListNew->SignatureSize) {
//TRACE((TRACE_ALWAYS,"New SigDb Size %X\nNew SigList Count: %d, SigSize %X\n", SigNewSize, SigCountNew, SigListNew->SignatureSize));
                      // loop through all instances of NewSigList->SigData. 
                      for (IndexNew = 1; IndexNew <= SigCountNew; IndexNew++) {
// TRACE((TRACE_ALWAYS,"NewCert %d, Data %X\n",IndexNew, *(UINT32*)SigItemNew->SignatureData));
                            if (MemCmp (SigItem->SignatureData, SigItemNew->SignatureData, SigList->SignatureSize-sizeof(EFI_GUID)) == 0) {
//TRACE((TRACE_ALWAYS,"---> match found!!!\n"));
//TRACE((TRACE_ALWAYS,"OrgCert %4d, Data %X\n",Index, *(UINT32*)SigItem->SignatureData));
//TRACE((TRACE_ALWAYS,"NewCert %4d, Data %X\n",IndexNew, *(UINT32*)SigItemNew->SignatureData));
                               if(SigCountNew == 1) {
                                // only 1 SigData per SigList - discard this SigList
                                      bSigMatch = TRUE;
//TRACE((TRACE_ALWAYS,"Before: DataSize=%x\nAfter : DataSize=%x\n", *DataSize, *DataSize-SigListNew->SignatureListSize));
                                      // 1. Decrease *Datasize by SigListNew->SignatureSize 
                                      SigNewSize -= SigListNew->SignatureListSize;
                                      *DataSize -= SigListNew->SignatureListSize;
                                      // 2. replace this SigData block with data following it
                                      MemCpy (SigListNew, (void*)((UINTN)SigListNew+SigListNew->SignatureListSize), SigNewSize); 
                                      // 3. Skip to next SigListNew    
                                      break;
                                } else {
                                // more then 1 - discard this SigData
                                    // 1. replace this SigData block with data following it
                                    MemCpy (SigItemNew, (void*)((UINTN)SigItemNew+SigListNew->SignatureSize), ((UINTN)Data+*DataSize)-((UINTN)SigItemNew+SigListNew->SignatureSize));
                                    // 2. Decrease SigListNew->SignatureListSize by SigListNew->SignatureSize 
                                    SigListNew->SignatureListSize-=SigListNew->SignatureSize;
                                    *DataSize-=SigListNew->SignatureSize;
//TRACE((TRACE_ALWAYS,"Upd SignatureListSize=%x, DataSize=%x\n",SigListNew->SignatureListSize, *DataSize));
                                    // 3. If this is last SigData element
                                    if((SigListNew->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - SigListNew->SignatureHeaderSize)==0)
                                    {  
//TRACE((TRACE_ALWAYS,"SigList is Empty!\n"));
                                        break;
                                    }         
                                    // 4. Skip incrementing of SigItemNew
                                    continue;
                                }
                            } // if cmp 
                            SigItemNew = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigItemNew + SigListNew->SignatureSize);
                        } // for SigItemNew 
                    } // if guid
                    // Skip incrementing of SigListNew if bSigListMatch is found - we already on next siglist
                    if(!bSigMatch) {
                        SigNewSize -= SigListNew->SignatureListSize;
                        SigListNew = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigListNew + SigListNew->SignatureListSize);
                    }
                } // while SigListNew
                SigItem = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigItem + SigList->SignatureSize);
            } // for SigItem
            SigDbSize -= SigList->SignatureListSize;
            SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
        } // while SigList 

TRACE((TRACE_ALWAYS,"DataSize Out: %d (0x%X)\n",*DataSize, *DataSize));
    if(*DataSize==0)
        return EFI_ALREADY_STARTED;

TRACE((TRACE_ALWAYS,"UPDATE SUCCESS!\n"));
    }

    return EFI_SUCCESS;
}
*/
/**
  Calculate hash of Pe/Coff image based on the authenticode image hashing in
  PE/COFF Specification 8.0 Appendix A

  Caution: This function may receive untrusted input.
  PE/COFF image is external input, so this function will validate its data structure
  within this image buffer before use.
  Caller is responsible to free newly created Digest buffer.

  @param[in]    ImageAddress
  @param[in]    ImageSize
  @param[in,out]    SHA256 Digest

  @retval TRUE      Successfully hashed image.
  @retval FALSE     Fail in hashing of image.

**/
BOOLEAN
HashPeImage (
  IN const UINT8 *ImageAddress,
  IN UINTN        ImageSize
  )
{
    EFI_STATUS                        Status;
    EFI_IMAGE_DOS_HEADER              *DosHdr;
    UINT32                            PeCoffHeaderOffset;
    EFI_IMAGE_SECTION_HEADER          *Section;
    UINT8                             *HashBase;
    UINTN                             HashSize;
    UINTN                             SumOfBytesHashed;
    EFI_IMAGE_SECTION_HEADER          *SectionHeader;
    UINTN                             Index, Pos;
    UINT16                            Magic;
    BOOLEAN                           res;
    const UINT8             *addr[MAX_ELEM_NUM];
    UINTN                    num_elem, len[MAX_ELEM_NUM];

    EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION  Hdr;

    SectionHeader = NULL;
    res = FALSE;
    PeCoffHeaderOffset = 0;
    num_elem = 0;

    //
    // Check PE/COFF image
    //
    DosHdr = (EFI_IMAGE_DOS_HEADER *) (UINTN) ImageAddress;
    PeCoffHeaderOffset = 0;
    if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
        PeCoffHeaderOffset = DosHdr->e_lfanew;
      } else {
        goto Finish;
    }

    if ( ((EFI_TE_IMAGE_HEADER *)((UINT8 *) (UINTN) ImageAddress + PeCoffHeaderOffset))->Signature
        == EFI_TE_IMAGE_HEADER_SIGNATURE) {
        goto Finish;
    }

  //
  // PE/COFF Image Measurement
  //
  //    NOTE: The following codes/steps are based upon the authenticode image hashing in 
  //      PE/COFF Specification 8.0 Appendix A.
  //      
  //

  // 1.    Load the image header into memory.
  
  // 2.    Initialize a SHA hash context.
    if(mAmiSig == NULL) {
        Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &mAmiSig); 
    }
    if(EFI_ERROR(Status))
        goto Finish;
  //
  // Measuring PE/COFF Image Header; 
  // But CheckSum field and SECURITY data directory (certificate) are excluded
  //
    Hdr.Pe32 = (EFI_IMAGE_NT_HEADERS32 *)((UINT8 *) (UINTN) ImageAddress + PeCoffHeaderOffset);
    Magic    = Hdr.Pe32->OptionalHeader.Magic;

    //
    // 3.  Calculate the distance from the base of the image header to the image checksum address.
    // 4.  Hash the image header from its base to beginning of the image checksum.
    //
    HashBase = (UINT8 *) (UINTN) ImageAddress;
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset
      //
      HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32->OptionalHeader.CheckSum) - HashBase);
    } else {
      //
      // Use PE32+ offset
      //
      HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32Plus->OptionalHeader.CheckSum) - HashBase);
    }

    addr[num_elem] = HashBase;
    len[num_elem++] =  HashSize;

  //
  // 5.    Skip over the image checksum (it occupies a single ULONG).
  // 6.    Get the address of the beginning of the Cert Directory.
  // 7.    Hash everything from the end of the checksum to the start of the Cert Directory.
  //
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset
      //
        HashBase = (UINT8 *) &Hdr.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
        HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
    } else {
      //
      // Use PE32+ offset
      //
        HashBase = (UINT8 *) &Hdr.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
        HashSize = (UINTN) ((UINT8 *)(&Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
    }

    addr[num_elem] = HashBase;
    len[num_elem++] =  HashSize;

  //
  // 8.    Skip over the Cert Directory. (It is sizeof(IMAGE_DATA_DIRECTORY) bytes.)
  // 9.    Hash everything from the end of the Cert Directory to the end of image header.
  //
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
     //
     // Use PE32 offset
     //
        HashBase = (UINT8 *) &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
         HashSize = Hdr.Pe32->OptionalHeader.SizeOfHeaders -
              (UINTN) ((UINT8 *)(&Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - (UINT8 *) (UINTN) ImageAddress);
    } else {
     //
     // Use PE32+ offset
     //
        HashBase = (UINT8 *) &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
        HashSize = Hdr.Pe32Plus->OptionalHeader.SizeOfHeaders -
              (UINTN) ((UINT8 *)(&Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - (UINT8 *) (UINTN) ImageAddress);
    }

    addr[num_elem] = HashBase;
    len[num_elem++] =  HashSize;

  //
  // 10. Set the SUM_OF_BYTES_HASHED to the size of the header 
  //
   if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset
      //
        SumOfBytesHashed = Hdr.Pe32->OptionalHeader.SizeOfHeaders;
    } else {
      //
      // Use PE32+ offset
      //
        SumOfBytesHashed = Hdr.Pe32Plus->OptionalHeader.SizeOfHeaders;
    }

  //
  // 11. Build a temporary table of pointers to all the IMAGE_SECTION_HEADER 
  //     structures in the image. The 'NumberOfSections' field of the image 
  //     header indicates how big the table should be. Do not include any 
  //     IMAGE_SECTION_HEADERs in the table whose 'SizeOfRawData' field is zero.   
  //

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof (EFI_IMAGE_SECTION_HEADER) * Hdr.Pe32->FileHeader.NumberOfSections, &SectionHeader);  
    if (SectionHeader == NULL) {
        goto Finish;
    }
    MemSet( SectionHeader, sizeof (EFI_IMAGE_SECTION_HEADER) * Hdr.Pe32->FileHeader.NumberOfSections, 0 );

  //
  // 12.    Using the 'PointerToRawData' in the referenced section headers as 
  //      a key, arrange the elements in the table in ascending order. In other 
  //      words, sort the section headers according to the disk-file offset of 
  //      the section.
  //
    Section = (EFI_IMAGE_SECTION_HEADER *) (
                   (UINT8 *) (UINTN) ImageAddress +
                   PeCoffHeaderOffset +
                   sizeof(UINT32) +
                   sizeof(EFI_IMAGE_FILE_HEADER) +
                   Hdr.Pe32->FileHeader.SizeOfOptionalHeader
                   );
    for (Index = 0; Index < Hdr.Pe32->FileHeader.NumberOfSections; Index++) {
        Pos = Index;
        while ((Pos > 0) && (Section->PointerToRawData < SectionHeader[Pos - 1].PointerToRawData)) {
          MemCpy (&SectionHeader[Pos], &SectionHeader[Pos - 1], sizeof(EFI_IMAGE_SECTION_HEADER));
          Pos--;
        }
        MemCpy (&SectionHeader[Pos], Section, sizeof(EFI_IMAGE_SECTION_HEADER));
        Section += 1;
    }

  //
  // 13.    Walk through the sorted table, bring the corresponding section 
  //      into memory, and hash the entire section (using the 'SizeOfRawData' 
  //      field in the section header to determine the amount of data to hash).
  // 14.    Add the section's 'SizeOfRawData' to SUM_OF_BYTES_HASHED .
  // 15.    Repeat steps 13 and 14 for all the sections in the sorted table.
  //
     for (Index = 0; Index < Hdr.Pe32->FileHeader.NumberOfSections; Index++) {
         Section  = (EFI_IMAGE_SECTION_HEADER *) &SectionHeader[Index];
         if (Section->SizeOfRawData == 0) {
             continue;
         }
         HashBase = (UINT8 *) (UINTN) ImageAddress + Section->PointerToRawData;
         HashSize = (UINTN) Section->SizeOfRawData;

         addr[num_elem] = HashBase;
         len[num_elem++] =  HashSize;
         if(num_elem >= MAX_ELEM_NUM)
             goto Finish;

         SumOfBytesHashed += HashSize;
    }


  //
  // 16.    If the file size is greater than SUM_OF_BYTES_HASHED, there is extra
  //      data in the file that needs to be added to the hash. This data begins 
  //      at file offset SUM_OF_BYTES_HASHED and its length is:
  //             FileSize  -  (CertDirectory->Size)
  //
    if (ImageSize > SumOfBytesHashed) {
        HashBase = (UINT8 *) (UINTN) ImageAddress + SumOfBytesHashed;
        if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        //
        // Use PE32 offset
        //
            HashSize = (UINTN)(ImageSize -
                       Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size -
                       SumOfBytesHashed);
        } else {
        //
        // Use PE32+ offset
        //
            HashSize = (UINTN)(ImageSize -
                       Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size -
                       SumOfBytesHashed);
        }

        addr[num_elem] = HashBase;
        len[num_elem++] =  HashSize;
        if(num_elem >= MAX_ELEM_NUM)
            goto Finish;
    }
  //
  // 17.    Finalize the SHA hash.
  //
    Status = mAmiSig->Hash(mAmiSig, &gEfiHashAlgorithmSha256Guid, num_elem, addr, len, mDigest);
    res = (!EFI_ERROR(Status)) ? TRUE : FALSE;

Finish:

    if(SectionHeader!=NULL)
    {
        pBS->FreePool(SectionHeader);
    }

    return res;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    IsCsmEnabled
//
// Description:  This function checks if CSM is enabled
//
//  Input:
//     None
//
//  Output:
//  0 - CSM is disabled
//  1 - CSM is enabled
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if defined(CSM_SUPPORT) && CSM_SUPPORT
BOOLEAN
IsCsmEnabled(VOID)
{
    EFI_STATUS Status;
    UINTN Size = sizeof(EFI_HANDLE);
    EFI_HANDLE Handle;

    Status = pBS->LocateHandle(ByProtocol, &gEfiLegacyBiosProtocolGuid, NULL, &Size, &Handle);
    return (EFI_ERROR(Status)) ? 0 : 1;
}
#endif
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   ForceSetupModeCallback
//
// Description: 
//
// Input:       none
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ForceSetupModeCallback(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
    EFI_STATUS Status;
    UINT8       Sel = 0;
    UINTN       Size;
    UINT8       SetupMode;
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
    UINT8       AuditMode, DeployedMode;
#endif

    CALLBACK_PARAMETERS *Callback;
    EFI_BROWSER_ACTION_REQUEST *rq;

    Callback = GetCallbackParameters();
    if(!Callback) {
        return EFI_SUCCESS;
    }
    TRACE((TRACE_ALWAYS,"\n====ForceSetupModeCallback==== Key=%d, Action=%0X\n",  Key, Callback->Action));
    if( Callback->Action != EFI_BROWSER_ACTION_CHANGING) {
//        if(Callback->Action==EFI_BROWSER_ACTION_RETRIEVE)
//            UpdateSecureVariableBrowserInfo();
        return EFI_UNSUPPORTED;
    }
    if(mPostMgr == NULL)
    {
        Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL, &mPostMgr);
        if(EFI_ERROR(Status) || !mPostMgr) {
            return EFI_SUCCESS;
        }
    }
    Status = EFI_SUCCESS;
    gHiiHandle = HiiHandle;
    switch(Key) {
        case  SECURE_BOOT_MENU_REFRESH:
            UpdateSecureBootBrowserInfo();
            break;
        case  KEY_MANAGEMENT_MENU_REFRESH:
            UpdateSecureVariableBrowserInfo();
            break;
        case SECURE_BOOT_SUPPORT_CHANGE_KEY:
            Size = 1;
            pRS->GetVariable(EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &SetupMode);
            if( Callback->Value->u8 == SECURE_BOOT) // trying to switch Secure Boot from Disable to Enable
            {
                rq = Callback->ActionRequest;
                *rq = EFI_BROWSER_ACTION_REQUEST_NONE;
                if(SetupMode == SETUP_MODE) {
                    GetHiiString(HiiHandle, STRING_TOKEN(STR_ENABLE_ERROR_MODE_TITLE), sizeof(StrTitle), StrTitle);
                    GetHiiString(HiiHandle, STRING_TOKEN(STR_ENABLE_ERROR_MODE), sizeof(StrMessage),StrMessage);
                    mPostMgr->DisplayMsgBox( StrTitle,  StrMessage, MSGBOX_TYPE_OK,NULL);
        #if DEFAULT_SECURE_BOOT_ENABLE == 0
                    Status = EFI_UNSUPPORTED;
        #endif
                } else {
#if defined(CSM_SUPPORT) && CSM_SUPPORT
                    if (IsCsmEnabled()) {
                        GetHiiString(HiiHandle, STRING_TOKEN(STR_CSM_LOAD_TITLE), sizeof(StrTitle),StrTitle);
                        GetHiiString(HiiHandle, STRING_TOKEN(STR_CSM_LOAD), sizeof(StrMessage),StrMessage);
                        mPostMgr->DisplayMsgBox( StrTitle,  StrMessage, MSGBOX_TYPE_OK,NULL);
        #if DEFAULT_SECURE_BOOT_ENABLE == 0
                       Status = EFI_UNSUPPORTED;
        #endif
                    }
#endif // CSM
                }
            }
            break;
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
        case FORCE_AUDIT_KEY:
                  AuditMode = 1;
                  Size = 1;
                  pRS->GetVariable(EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &Size, &SetupMode);
                  if(SetupMode == USER_MODE) {
                      GetHiiString(HiiHandle, STRING_TOKEN(STR_FORCE_AUDIT_MODE), sizeof(StrTitle),StrTitle);
                      GetHiiString(HiiHandle, STRING_TOKEN(STR_PK_DEL_WARNING), sizeof(StrMessage),StrMessage);
                      mPostMgr->DisplayMsgBox( StrTitle,  StrMessage, (UINT8)MSGBOX_TYPE_YESNO, &Sel);
                      if (Sel == 1)
                          break;
                  }
                  Status = pRS->SetVariable(EFI_AUDIT_MODE_NAME,&gEfiGlobalVariableGuid, 
                              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS, 1, &AuditMode);
                  TRACE((TRACE_ALWAYS,"Force '%S' %r\n",EFI_AUDIT_MODE_NAME,  Status));

                  UpdateSecureBootBrowserInfo();
           break;

        case FORCE_DEPLOYED_KEY:
                  DeployedMode = 1;
                  Status = pRS->SetVariable(EFI_DEPLOYED_MODE_NAME,&gEfiGlobalVariableGuid, 
                              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS, 1, &DeployedMode);
                  TRACE((TRACE_ALWAYS,"Force '%S' %r\n",EFI_DEPLOYED_MODE_NAME,  Status));
                  UpdateSecureBootBrowserInfo();
           break;
#endif
        case SECURE_BOOT_MODE_CHANGE_KEY:
            if( Callback->Value->u8 == 0)    // trying to switch from Custom to Standard
                ForceSetupModeCallback(HiiHandle, Class, SubClass, FORCE_DEFAULT_KEY);
            UpdateSecureBootSetupVariable(Callback->Value->u8); // upd sec Boot Setup variable
            break;

        case KEY_PROVISION_CHANGE_KEY:
            if(Callback->Value->u8 == 0) // trying to switch from Disable to Enable
                break;
        case FORCE_DEFAULT_KEY:
            GetHiiString(HiiHandle, STRING_TOKEN(STR_LOAD_DEFAULT_VARS_TITLE), sizeof(StrTitle),StrTitle);
            GetHiiString(HiiHandle, STRING_TOKEN(STR_LOAD_DEFAULT_VARS), sizeof(StrMessage),StrMessage);
            mPostMgr->DisplayMsgBox( StrTitle,  StrMessage, (UINT8)MSGBOX_TYPE_YESNO, &Sel);
            if (Sel == 0)
                Status = InstallSecureVariables(RESET_NV_KEYS | SET_NV_DEFAULT_KEYS);    // erase+set
        case FORCE_SETUP_KEY:
            if (Key == FORCE_SETUP_KEY)
            {
                GetHiiString(HiiHandle, STRING_TOKEN(STR_FORCE_SETUP_MODE), sizeof(StrTitle),StrTitle);
                GetHiiString(HiiHandle, STRING_TOKEN(STR_RESET_TO_SETUP), sizeof(StrMessage),StrMessage);
                mPostMgr->DisplayMsgBox( StrTitle,  StrMessage, (UINT8)MSGBOX_TYPE_YESNO, &Sel);
                if (Sel == 0)
                   Status = InstallSecureVariables(RESET_NV_KEYS);    // erase
            }
            UpdateSecureVariableBrowserInfo();
            if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND)
            {
                GetHiiString(HiiHandle, STRING_TOKEN(STR_VAR_UPDATE_LOCKED_TITLE), sizeof(StrTitle),StrTitle);
                GetHiiString(HiiHandle, STRING_TOKEN(STR_VAR_UPDATE_LOCKED), sizeof(StrMessage),StrMessage);
                mPostMgr->DisplayMsgBox( StrTitle,  StrMessage, MSGBOX_TYPE_OK,NULL);
            }
            Status = EFI_SUCCESS;
            break;

        default:
            break;
    }
    return Status;//EFI_SUCCESS;
}

EFI_STATUS SetSecureVariable(UINT8 Index, UINT16 InstallVars, UINT8 *pVarBuffer, UINTN VarSize )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_GUID    *EfiVarGuid;
    UINT32      Attributes;
    UINTN       Size= 0;

///////////////////////////////////////////////////////////////////////////////
// Initial provisioning of Authenticated non-volatile EFI Variables 
///////////////////////////////////////////////////////////////////////////////
    if(SecureVariableFileName[Index] != NULL)
    {
        if(Index < SecureVariableSigDatabaseCount)
            EfiVarGuid = &gEfiImageSecurityDatabaseGuid;
        else
            EfiVarGuid = &gEfiGlobalVariableGuid;

        Status = pRS->GetVariable( SecureVariableFileName[Index], EfiVarGuid, NULL, &Size, NULL);
        if(Status == EFI_BUFFER_TOO_SMALL && // Var exists
          (InstallVars & SET_SECURE_VARIABLE_DEL) == SET_SECURE_VARIABLE_DEL) {
        // try to erase. should succeed if system in pre-boot and Admin mode
            Status = pRS->SetVariable(SecureVariableFileName[Index],EfiVarGuid,0,0,NULL);
            TRACE((TRACE_ALWAYS,"Del Var %S, Status %r\n",SecureVariableFileName[Index], Status));
        }
        if((InstallVars & SET_SECURE_VARIABLE_SET)==SET_SECURE_VARIABLE_SET &&
            pVarBuffer && VarSize) {

            Attributes = EFI_VARIABLE_RUNTIME_ACCESS |
                         EFI_VARIABLE_NON_VOLATILE | 
                         EFI_VARIABLE_BOOTSERVICE_ACCESS |
                         EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS; 

            if((InstallVars & SET_SECURE_VARIABLE_APPEND)== SET_SECURE_VARIABLE_APPEND)
                Attributes |= EFI_VARIABLE_APPEND_WRITE;

            Status = pRS->SetVariable(SecureVariableFileName[Index], EfiVarGuid, Attributes, VarSize, pVarBuffer);
            TRACE((TRACE_ALWAYS,"Set Var %S(%d), Status %r\n",SecureVariableFileName[Index], VarSize, Status));
            VarSize = 0;
            pRS->GetVariable( SecureVariableFileName[Index], EfiVarGuid, NULL, &VarSize, NULL);
            TRACE((TRACE_ALWAYS,"Old Var Size %d\nNew Var size %d\n", Size, VarSize));
            if(((InstallVars & SET_SECURE_VARIABLE_APPEND)== SET_SECURE_VARIABLE_APPEND) && Size == VarSize )
                Status = EFI_ALREADY_STARTED;
        }
    }

    return Status;
}

EFI_STATUS
SaveToFileSecureBootVar(EFI_HII_HANDLE HiiHandle, UINT16 Key)
{
    EFI_STATUS Status;
    EFI_HANDLE *FsHandle;
    UINT8 *FileBuf=NULL;
    UINTN size;
    CHAR16 *FilePath=NULL;
    UINT8 Index, nVars;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *pSFP;
    EFI_FILE_PROTOCOL *pRoot,*FileHandle;
    EFI_GUID    *EfiVarGuid;
    UINT8       *Data=NULL;
    BOOLEAN     bFound = TRUE;

    if(Key > SecureBootPolicyVariableCount)
        return EFI_SUCCESS;

    gHiiHandle = HiiHandle;
    size = 0;
    Status = FileBrowserLaunchFileSystem(FALSE, &FsHandle, &FilePath, &FileBuf, &size);
    if(EFI_ERROR(Status))
       goto Done;

    Index = (Key == SecureBootPolicyVariableCount)?0:Key;
//    Index = 0;
    nVars = 0;
    MemSet(StrTemp, sizeof(StrTemp), 0);
    while(bFound && SecureVariableFileName[Index] != NULL)
    {
        if(Index < SecureVariableSigDatabaseCount)
            EfiVarGuid = &gEfiImageSecurityDatabaseGuid;
        else
            EfiVarGuid = &gEfiGlobalVariableGuid;

        size = 0;  
        Status = pRS->GetVariable( SecureVariableFileName[Index], EfiVarGuid, NULL, &size, NULL);
        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            // Append AuthHdr to Var data.
            //Allocate Size
            Status = pBS->AllocatePool(EfiBootServicesData, size, &Data);
            ASSERT_EFI_ERROR (Status);

            // Read the Variable
            Status = pRS->GetVariable( SecureVariableFileName[Index], EfiVarGuid, NULL, &size, Data);
            if (!EFI_ERROR(Status)) 
            {
                Status = pBS->HandleProtocol( FsHandle, &gEfiSimpleFileSystemProtocolGuid, &pSFP );
                if (!EFI_ERROR(Status)) 
                {
                    Status = pSFP->OpenVolume(pSFP,&pRoot);
                    if (!EFI_ERROR(Status)) 
                    {
                        Status = pRoot->Open(pRoot,&FileHandle,SecureVariableFileName[Index],
                            EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,0);
                        if(!EFI_ERROR(Status))
                        {
                            FileHandle->Delete(FileHandle);
                            Status = pRoot->Open(pRoot,&FileHandle,SecureVariableFileName[Index],
                                EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,0);
                            if(!EFI_ERROR(Status))
                            {
                                // Write
                                Status = FileHandle->Write(FileHandle,&size, Data);
                                FileHandle->Close(FileHandle);
                                nVars++;
                            }
                        }
                    }
                }
               if (EFI_ERROR(Status)) {
                    bFound = FALSE;
                    Swprintf_s(StrTemp, sizeof(StrTemp), L" %s ", SecureVariableFileName[Index]);
                    GetHiiString(HiiHandle, STRING_TOKEN(STR_WRITE_ERROR_TITLE), sizeof(StrTitle),StrTitle);
                    mPostMgr->DisplayMsgBox( StrTitle, StrTemp, MSGBOX_TYPE_OK, NULL );
                }
            }
            if(Data!=NULL)
                pBS->FreePool(Data);
        }
        Index++;
        if(Key != SecureBootPolicyVariableCount) { Index = SecureBootPolicyVariableCount; break; }
    }
    if (Index==SecureBootPolicyVariableCount && bFound) {
        GetHiiString(HiiHandle, STRING_TOKEN(STR_SAVE_SEC_KEY),sizeof(StrMessage), StrMessage);
        Swprintf_s(StrTemp, sizeof(StrTemp), StrMessage, nVars);
        GetHiiString(HiiHandle, STRING_TOKEN(STR_SAVE_SEC_KEY_TITLE),sizeof(StrTitle), StrTitle);
        mPostMgr->DisplayMsgBox(StrTitle , StrTemp, MSGBOX_TYPE_OK, NULL );
    }

Done:

    if(FileBuf)
        pBS->FreePool(FileBuf);

    if(FilePath)
        pBS->FreePool(FilePath);

    return EFI_SUCCESS;
}
/*
display MsgBox with drop down list

====DB=====
#string STR_KEY_TITLE                   #language en-US " Cert Index | Cert count | Cert GUID type | Size | Owner (AMI, Vendor, Msft, Unknown, GUID) | Name"
#string STR_KEY_TITLE_TEXT              #language en-US "  %10d| %10d| %s | %g | %s | %s"

 Cert Index: Cert count : Cert GUID type : Size : Owner GUID type (AMI, Vendor, Msft, Unknown)
      1          1            X.509         740     GUID XXX-XX (Msft)  : Subject Name (x509 only)
      2          13           SHA256        1370    XXXX (Msft)

by selecting the Index will display the next menu with:
 Display Subject Name of a x509 Cert and prompt to delete the Sig List

 Analogous to File List browser 
 scan through SignatureListst
 record ptr to each cert  and length
 Fill in info: 
 * alloc mem for selected certificate
 * get Cert Name from Pkcs7Verify
 * To delete the var -> update the Var' SigList and sat var back to db/dbx/...
 * use ModifySignatureList() to strip down cert to be deleted
 * free alloc cert pool
*/
//VOID 
//GetSecureBootVarCertificateInfo(EFI_HII_HANDLE HiiHandle, UINT8 index) 
//{
//    GetHiiString(HiiHandle, STRING_TOKEN(STR_SECURE_KEY_INFO),sizeof(StrTitle), StrTitle);
//    GetHiiString(HiiHandle, STRING_TOKEN(STR_AMI_TEST_KEY), sizeof(StrMessage), StrMessage);
//    mPostMgr->DisplayMsgBox( StrTitle, StrMessage, MSGBOX_TYPE_OK, NULL );
//
//    return;
//}

//
// Key Management Callbacks 
//
EFI_STATUS
ManageSecureBootKeysCallback(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
    EFI_STATUS Status;
    UINT8 index;
    UINT16 CertSel = 0;
    UINT8 MenuSize, DelItem, SetItem, AppendItem;
    UINT8 /*KeyInfo,*/ SaveItem;
    POSTMENU_TEMPLATE MenuList[5]; // Info/Save/Set/Append/Delete
    CALLBACK_PARAMETERS *Callback;

    Callback = GetCallbackParameters();
    if(!Callback || Callback->Action != EFI_BROWSER_ACTION_CHANGING)
        return EFI_UNSUPPORTED;

    switch(Key)
    {
    case MANAGE_PK_KEY:
                    index = PkVar;
                    break;
    case MANAGE_KEK_KEY:
                    index = KekVar;
                    break;
    case MANAGE_DB_KEY:
                    index = DbVar;
                    break;
    case MANAGE_DBR_KEY:
                    index = DbrVar;
                    break;
    case MANAGE_DBT_KEY:
                    index = DbtVar;
                    break;
    case MANAGE_DBX_KEY:
                    index = DbxVar;
                    break;
    default:
        return EFI_SUCCESS;
    }

    if(mPostMgr == NULL)
        return EFI_SUCCESS;

    gHiiHandle = HiiHandle;

    MenuSize = 4;//5; 
//    KeyInfo  = 0;
    SaveItem  = 0;//1;
    SetItem  = 1;//2;
    AppendItem = 2;//3;
    DelItem = 3;//4;

    if(bKey[index]==Absent) { // no Key - no Del
        MenuSize = 2; 
        SetItem  = 0;
        AppendItem = 1;
        // hidden
//        KeyInfo  = 4;
        SaveItem  = 4;
        DelItem = 4;
    }
    if(Key == MANAGE_PK_KEY) { // PK - no Append
        MenuSize--;
        DelItem--; // move Del up
        AppendItem = 4;
    }

    // Clear the memory allocated 
    MemSet(MenuList, sizeof(MenuList), 0);
    MenuList[SetItem].ItemToken = STRING_TOKEN(STR_SECURE_SET);
    MenuList[AppendItem].ItemToken = STRING_TOKEN(STR_SECURE_APPEND);
    MenuList[DelItem].ItemToken = STRING_TOKEN(STR_SECURE_DELETE);
    MenuList[SaveItem].ItemToken = STRING_TOKEN(STR_SAVE_VAR_DATA);
//    MenuList[KeyInfo].ItemToken = STRING_TOKEN(STR_SECURE_KEY_INFO);

    // Call post manager to display the menu
    Status = mPostMgr->DisplayPostMenu(HiiHandle,
                        STRING_TOKEN(STR_KEY_MANAGEMENT_TITLE),
                        0,
                        MenuList,
                        MenuSize,
                        &CertSel);

    if(!EFI_ERROR(Status))
    {
        if(CertSel == DelItem) {
            if(bKey[index] != Absent)
                DeleteSecureBootDBCallback( HiiHandle,  index);
        }
        if(CertSel == SetItem) {
            SetAppendSecureBootDBCallback( HiiHandle,  SET_SECURE_VARIABLE_SET | SET_SECURE_VARIABLE_DEL, index);
        }
        if(CertSel == AppendItem) {
            if(Key != MANAGE_PK_KEY)
                SetAppendSecureBootDBCallback( HiiHandle,  SET_SECURE_VARIABLE_SET | SET_SECURE_VARIABLE_APPEND,  index);
        }
        if(CertSel == SaveItem) {
            SaveToFileSecureBootVar(HiiHandle, index);
        }
//        if(CertSel == KeyInfo) {
//            GetSecureBootVarCertificateInfo( HiiHandle,  index);
//        }
    }
    return EFI_SUCCESS;
}

//
// ENROLL_EFI_IMAGE
//
EFI_STATUS
EnrollCertificateToDbFromEfiImageCallback(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
    EFI_STATUS Status;
    EFI_HANDLE *FsHandle;
    UINT8 *FileBuf=NULL;
    UINT8 *Data=NULL;
    UINTN size, VarSize; //DataSize;
    CHAR16 *FilePath=NULL;
    UINT16 AddSize;
    UINT8 Sel;

    CALLBACK_PARAMETERS *Callback;

    Callback = GetCallbackParameters();
    if(!Callback || Callback->Action != EFI_BROWSER_ACTION_CHANGING)
        return EFI_UNSUPPORTED;

    if(mPostMgr == NULL)
        return EFI_SUCCESS;

    size = 0;
    AddSize = 0;
    Status = FileBrowserLaunchFileSystem(TRUE, &FsHandle, &FilePath, &FileBuf, &size);
    if(!EFI_ERROR(Status) && FileBuf)
    {
        Status = EFI_LOAD_ERROR;
        if(FALSE == HashPeImage ((const UINT8*)FileBuf, size)) {
            goto exit;
        }
        pBS->FreePool(FileBuf);
        FileBuf = &mDigest[0];
        size = SHA256_DIGEST_SIZE;
        //
        // form an AuthVar Hdr on top of Var
        //
        //Allocate new Size
        AddSize = sizeof(EFI_VARIABLE_SIG_HDR_2)-1; // decrement by 1 byte as SIG_DATA adds 1 dummy byte
        VarSize = size+AddSize;
        if(EFI_ERROR(pBS->AllocatePool(EfiBootServicesData, VarSize, &Data)))
            goto exit;
        MemSet(Data, VarSize, 0);
        // Append AuthHdr to Var data.
        FillAuthVarHdr(Data,FileBuf,size, SET_SECURE_VARIABLE_APPEND);
        MemCpy ((VOID*)((UINTN)Data+AddSize), FileBuf, size);
        FileBuf = NULL;

        MemSet(StrTemp, sizeof(StrTemp), 0);
        GetHiiString(HiiHandle, STRING_TOKEN(STR_UPDATE_FROM_FILE), sizeof(StrMessage), StrMessage);
        Swprintf_s(StrTemp, sizeof(StrTemp),StrMessage, SecureVariableFileName[DbVar],FilePath);
        GetHiiString(HiiHandle, STRING_TOKEN(STR_ENROLL_EFI_IMAGE),sizeof(StrTitle), StrTitle);
        mPostMgr->DisplayMsgBox( StrTitle, StrTemp, (UINT8)MSGBOX_TYPE_YESNO, &Sel);
        if(Sel == 0)
        {
            Status = SetSecureVariable(DbVar, SET_SECURE_VARIABLE_SET | SET_SECURE_VARIABLE_APPEND, Data, VarSize);
        }
        pBS->FreePool(Data);
    }
exit:
    if(!EFI_ERROR(Status)){
        UpdateSecureVariableBrowserInfo();
        GetHiiString(HiiHandle, STRING_TOKEN(STR_SUCCESS), sizeof(StrMessage),StrMessage);
    } else {
        GetHiiString(HiiHandle, STRING_TOKEN(STR_FAILED), sizeof(StrMessage),StrMessage);
    }
    GetHiiString(HiiHandle, STRING_TOKEN(STR_ENROLL_EFI_IMAGE),sizeof(StrTitle), StrTitle);
    mPostMgr->DisplayMsgBox( StrTitle, StrMessage, MSGBOX_TYPE_OK, NULL );

    if(FileBuf)
        pBS->FreePool(FileBuf);

    if(FilePath)
        pBS->FreePool(FilePath);

    return EFI_SUCCESS;
}

VOID
SetAppendSecureBootDBCallback(EFI_HII_HANDLE HiiHandle, UINT8 VarSetMode, UINT8 index)
{
    EFI_STATUS Status;
    EFI_HANDLE *FsHandle;
    UINT8 *FileBuf=NULL;
    UINT8 *Data=NULL;
    UINTN size, VarSize; //DataSize;
    CHAR16 *FilePath=NULL;
    UINT8 Sel;
    UINT16 CertSel, AddSize;
    UINT8 MenuSize;
    POSTMENU_TEMPLATE MenuList[3]; // SigList/Var/PE/COFF

    MemSet(StrTemp, sizeof(StrTemp), 0);
    Sel = 1; // No
    CertSel = 0;
    
    if(VarSetMode & SET_SECURE_VARIABLE_APPEND)
        GetHiiString(HiiHandle, STRING_TOKEN(STR_SECURE_APPEND), sizeof(StrTitle), StrTitle);
    else
        GetHiiString(HiiHandle, STRING_TOKEN(STR_SECURE_SET), sizeof(StrTitle), StrTitle);

    GetHiiString(HiiHandle, STRING_TOKEN(STR_UPDATE_FROM_DEFAULTS),sizeof(StrMessage), StrMessage);
    Swprintf_s(StrTemp, sizeof(StrTemp), StrMessage ,  SecureVariableFileName[index]);
    Status = mPostMgr->DisplayMsgBox( StrTitle, StrTemp, (UINT8)MSGBOX_TYPE_YESNO, &Sel);
    if(Sel == 0)
    {
        size = 0 ; 
        Status = CryptoGetRawImage( SecureVariableFileGuid[index], &FileBuf, (UINTN*)&size);
    } else
    {
        size = 0;
        AddSize = 0;
        MenuSize = (index < SecureVariableSigDatabaseCount)?3:2; // all db(x) can accept certs from PE/COFF
//        MenuSize = 2;
        Status = FileBrowserLaunchFileSystem(TRUE, &FsHandle, &FilePath, &FileBuf, &size);
        if(!EFI_ERROR(Status) && FileBuf)
        {
            // Clear the memory allocated 
            MemSet(MenuList, sizeof(MenuList), 0);
            MenuList[0].ItemToken = STRING_TOKEN(STR_SECURE_CER);
            MenuList[1].ItemToken = STRING_TOKEN(STR_SECURE_VAR);
            MenuList[2].ItemToken = STRING_TOKEN(STR_SECURE_EFI);
            // Call post manager to display the menu
            Status = mPostMgr->DisplayPostMenu(gHiiHandle,
                                                STRING_TOKEN(STR_SECURE_FILE_TITLE), // Change this
                                                0,
                                                MenuList,
                                                MenuSize,
                                                &CertSel);
            if(!EFI_ERROR(Status))
            {
                Status = EFI_LOAD_ERROR;
                if(CertSel==2) { // Efi executable
                    if(FALSE == HashPeImage ((const UINT8*)FileBuf, size)) {
                        goto exit;
                    }
                    pBS->FreePool(FileBuf);
                    FileBuf = &mDigest[0];
                    size = SHA256_DIGEST_SIZE;
                    CertSel = 0; // treat as a Certificate 
                }
                if(CertSel==0) {
                    AddSize = sizeof(EFI_VARIABLE_SIG_HDR_2)-1; // decrement by 1 byte as SIG_DATA adds 1 dummy byte
                }
                // Validate Signature List integrity 
                if(!EFI_ERROR(ValidateSignatureList (FileBuf, size, NULL))) {
                    CertSel=MenuSize;
                    AddSize = sizeof(AMI_EFI_VARIABLE_AUTHENTICATION_2);
                }
                //
                // form an AuthVar Hdr on top of Var
                //
                //Allocate new Size
                VarSize = size+AddSize;
                if(EFI_ERROR(pBS->AllocatePool(EfiBootServicesData, VarSize, &Data)))
                    goto exit;
                MemSet(Data, VarSize, 0);
                // Append AuthHdr to Var data.
                if(CertSel==0)
                    FillAuthVarHdr(Data,FileBuf,size, VarSetMode);
                else 
                    if(CertSel==MenuSize) // unsupported - append from SigList
                        FillAuthHdr(Data, VarSetMode);

                MemCpy ((VOID*)((UINTN)Data+AddSize), FileBuf, size);
                if(FileBuf!=&mDigest[0])
                    pBS->FreePool(FileBuf);
                FileBuf = Data;
                size = VarSize;
                Status = EFI_SUCCESS;
            }
        }
    }
    if(!EFI_ERROR(Status) && FileBuf) {

        if(Sel == 1) {
            GetHiiString(HiiHandle, STRING_TOKEN(STR_UPDATE_FROM_FILE), sizeof(StrMessage), StrMessage);
            Swprintf_s(StrTemp, sizeof(StrTemp),StrMessage, SecureVariableFileName[index],FilePath);
            mPostMgr->DisplayMsgBox( StrTitle, StrTemp, (UINT8)MSGBOX_TYPE_YESNO, &Sel);
        }
        if(Sel == 0)
        {
            Status = SetSecureVariable(index, VarSetMode, FileBuf, size);
        } else 
            Status = EFI_ABORTED;
    }
exit:    
    if(!EFI_ERROR(Status)){
        GetHiiString(HiiHandle, STRING_TOKEN(STR_SUCCESS), sizeof(StrMessage),StrMessage);
    } else {
        GetHiiString(HiiHandle, STRING_TOKEN(STR_FAILED), sizeof(StrMessage),StrMessage);
    }
    mPostMgr->DisplayMsgBox( StrTitle, StrMessage, MSGBOX_TYPE_OK, NULL );
    UpdateSecureVariableBrowserInfo();

    if(FileBuf)
        pBS->FreePool(FileBuf);

    if(FilePath)
        pBS->FreePool(FilePath);
}

VOID
DeleteSecureBootDBCallback(EFI_HII_HANDLE HiiHandle, UINT8 index)
{
    EFI_STATUS Status;
    UINT8 Sel = 0;    

    MemSet(StrTemp, sizeof(StrTemp), 0);

    GetHiiString(HiiHandle, STRING_TOKEN(STR_DELETE_SEC_KEY_TITLE), sizeof(StrTitle),StrTitle);
    if(Wcscmp(SecureVariableFileName[index], (CHAR16*)EFI_PLATFORM_KEY_NAME)==0) {
        GetHiiString(HiiHandle, STRING_TOKEN(STR_PK_DEL_WARNING), sizeof(StrTemp),StrTemp);
        mPostMgr->DisplayMsgBox( StrTitle,  StrTemp, (UINT8)MSGBOX_TYPE_OK, NULL);
    }
    GetHiiString(HiiHandle, STRING_TOKEN(STR_DELETE_SEC_KEY), sizeof(StrMessage), StrMessage);
    MemSet(StrTemp, sizeof(StrTemp), 0);
    Swprintf_s(StrTemp, sizeof(StrTemp), StrMessage, SecureVariableFileName[index]);
    mPostMgr->DisplayMsgBox( StrTitle,  StrTemp, (UINT8)MSGBOX_TYPE_YESNO, &Sel);
    if(Sel == 0)
    {
        Status = SetSecureVariable(index, SET_SECURE_VARIABLE_DEL, NULL, 0);
        if(!EFI_ERROR(Status)){
            GetHiiString(HiiHandle, STRING_TOKEN(STR_SUCCESS), sizeof(StrMessage),StrMessage);
        }
        else
        {
            GetHiiString(HiiHandle, STRING_TOKEN(STR_FAILED), sizeof(StrMessage),StrMessage);
        }            
        mPostMgr->DisplayMsgBox( StrTitle, StrMessage, MSGBOX_TYPE_OK, NULL );
        UpdateSecureVariableBrowserInfo();
    }
}

EFI_STATUS
GetSecureBootDBCallback(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
    CALLBACK_PARAMETERS *Callback;

    Callback = GetCallbackParameters();
    if(!Callback || Callback->Action != EFI_BROWSER_ACTION_CHANGING)
       return EFI_UNSUPPORTED;

    gHiiHandle = HiiHandle;

    SaveToFileSecureBootVar(HiiHandle, SecureBootPolicyVariableCount);

    return EFI_SUCCESS;
}
// 
// Auxiliary functions 
//
static VOID EfiStrCat (
    IN CHAR16   *Destination,
    IN CHAR16   *Source
    )
{   
    Wcscpy (Destination + Wcslen (Destination), Source);
}

static CHAR16 *StrDup8to16( CHAR8 *string )
{
    CHAR16  *text;
    UINTN   i;

    if ( string == NULL )
        return NULL;

    pBS->AllocatePool(EfiBootServicesData, (1 + Strlen( string )) * sizeof(CHAR16), &text);

    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR16)string[i])
            i++;
    }

    return text;
}
 
EFI_STRING_ID HiiAddString(IN EFI_HII_HANDLE HiiHandle,IN CHAR16 *String)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    UINTN LangSize = 0;
    CHAR8* CurrentLanguage;
    BOOLEAN LastLanguage = FALSE;
    EFI_STRING_ID  StringId = 0;
    CHAR8          *SupportedLanguages=NULL;

    if(HiiString == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
        if(EFI_ERROR(Status))
            return 0;
    }

    if(SupportedLanguages == NULL) {
        Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        if(Status == EFI_BUFFER_TOO_SMALL) {
            Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
            if(EFI_ERROR(Status))
                return 0;        //not enough resources to allocate string
            Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        }
        if(EFI_ERROR(Status))
            return 0;
    } else {
        Languages = SupportedLanguages;
     }

    while(!LastLanguage) {
        CurrentLanguage = Languages;        //point CurrentLanguage to start of new language
        while(*Languages != ';' && *Languages != 0)
            Languages++;

        if(*Languages == 0) {       //last language in language list
            LastLanguage = TRUE;
            if(StringId == 0)
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            else
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            if(EFI_ERROR(Status))
                return 0;
        } else {
            *Languages = 0;         //put null-terminator
            if(StringId == 0)
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            else
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            *Languages = ';';       //restore original character
            Languages++;
            if(EFI_ERROR(Status))
                return 0;
        }
    }
    return StringId;        
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    EfiLibAllocateCopyPool
//
// Description:    Allocate BootServicesData pool and use a buffer provided by 
//                    caller to fill it.
//
// Input:    AllocationSize  - The size to allocate
//                    Buffer          - Buffer that will be filled into the buffer allocated
//
// Output:    Pointer of the buffer allocated.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID *
EfiLibAllocateCopyPool (
  IN  UINTN   AllocationSize,
  IN  VOID    *Buffer
  )
{
    VOID  *Memory;

    Memory = NULL;
    pBS->AllocatePool (EfiBootServicesData, AllocationSize, &Memory);
    if (Memory != NULL && Buffer != NULL) {
        pBS->CopyMem (Memory, Buffer, AllocationSize);
    }

    return Memory;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   CleanFileTypes
//
// Description: Frees all allocated memory associated with the FILE_TYPE structure
//      and resets the InternalString current strings next available string token
//      to be the first dynamically added string
//
// Input:   FILE_TYPE **FileList - The array of FILE_TYPE structures found in 
//              a directory
//          UINTN *FileCount - pointer to the number of entries in the FileList
//
// Output:
//
// Returns:
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CleanFileTypes(FILE_TYPE **FileList, UINTN *FileCount)
{
    UINTN i;
    for(i = 0; i<*FileCount; i++) pBS->FreePool((*FileList)[i].Name);
    if(FileList!=NULL && (*FileList!=NULL) && (*FileCount>0)) pBS->FreePool(*FileList);
    if(FileList!=NULL) *FileList = NULL;
    *FileCount = 0;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   CheckDirectoryType
//
// Description: Checks if the EFI_FILE_INFO is a directory (and not the current directory)
//
// Input:   EFI_FILE_INFO *File
//
// Output:  
//
// Returns: BOOLEAN - TRUE - item is a directory, and not the current directory
//                    FALSE - item is not a directory, or is the current directory
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
/*
BOOLEAN CheckDirectoryType(EFI_FILE_INFO *File)
{
    BOOLEAN Status = FALSE;

    if((File->Attribute & EFI_FILE_DIRECTORY) && (Wcscmp(File->FileName, L".") != 0)) {

        Status = TRUE;
    }

    return Status;
}
*/
//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   CheckExtension
//
// Description: Check is the EFI_FILE_INFO has the same extension as the 
//      extension passed in the second parameter
//
// Input:   EFI_FILE_INFO *File - The file entry whose extension should be checked
//          CHAR16 *ExtensionEfi - the extension
//
// Output:
//
// Returns: BOOLEAN - TRUE - The extension matches
//                    FALSE - the extension does not match
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN CheckExtension(EFI_FILE_INFO *File, CHAR16 *ExtensionEfi)
{
    BOOLEAN Status = FALSE;
    UINTN Length = Wcslen(File->FileName);

    if((File->Attribute & EFI_FILE_DIRECTORY) != EFI_FILE_DIRECTORY && Length > 3)
        if((((File->FileName[Length-1])&0xdf) == ((ExtensionEfi[2])&0xdf)) &&
           (((File->FileName[Length-2])&0xdf) == ((ExtensionEfi[1])&0xdf)) &&
           (((File->FileName[Length-3])&0xdf) == ((ExtensionEfi[0])&0xdf)))
            Status = TRUE;
    return Status;    
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   FindInsertionIndex
//
// Description: Finds the index where directories items turn into file items
//
// Input:   FILE_TYPE *List - the current array of File Type structures
//          UINTN FileCount - the count of File Type structures in the array
//
// Output:
//
// Returns: the index to insert a new item
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN FindInsertionIndex(FILE_TYPE *List, UINTN FileCount)
{
    UINTN i = 0;
    
    if(FileCount <= 1) return 0;

    for(i = 1; i < (FileCount-1); i++)
    {
        if(List[i-1].Type != List[i].Type)
        break;
    }

    return i;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   AddFileTypeEntry
//
// Description: Creates a new entry in the FILE_TYPE array and adds the current File into
//      the array.
//
// Input:   FILE_TYPE **List - Array of FILE_TYPE structures alread found
//          UINTN *FileCount - number of entries in the FILE_TYPE array
//          EFI_FILE_INFO *FileInfo - file info of the file that should be added
//
// Output:
//
// Returns:
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
static VOID AddFileTypeEntry(FILE_TYPE **List, UINTN *FileCount, EFI_FILE_INFO *FileInfo)
{
    FILE_TYPE *NewList=NULL;
    UINTN Length;
    UINTN Index = 0;

    Length = (Wcslen(FileInfo->FileName)+3)*sizeof(CHAR16);

    // Allocate space for a new list entry plus all the previous list items
    NewList = EfiLibAllocateCopyPool(sizeof(FILE_TYPE)*(++(*FileCount)), NewList);
    if (NewList != NULL) 
    {
        // Clear the memory of the entire list
        MemSet(NewList, sizeof(FILE_TYPE)*(*FileCount), 0);
    
        // Copy the old entries (if there are any old entries to copy)
        if(*List != NULL) 
        {
            Index = FindInsertionIndex(*List, *FileCount);

            pBS->CopyMem(NewList, *List, sizeof(FILE_TYPE)*(Index));
            pBS->CopyMem(&(NewList[Index+1]), &((*List)[Index]), sizeof(FILE_TYPE)*((*FileCount)-Index-1));

            pBS->FreePool(*List);
        }

        // Store the type of this FILE_TYPE entry (non-zero is directory)
        NewList[Index].Type = ((FileInfo->Attribute) & EFI_FILE_DIRECTORY);

        // Store the size of the file
        NewList[Index].Size = (UINTN)FileInfo->FileSize;

        // Allocate space for the string
        NewList[Index].Name = EfiLibAllocateCopyPool(Length, NewList[Index].Name);
        if((NewList[Index].Name) != NULL )
        {
            // Clear the allocated memory
            MemSet(NewList[Index].Name, Length, 0);

            // Create either a Dir string or a File string for addition to the HiiDataBase
            if(NewList[Index].Type == EFI_FILE_DIRECTORY)
                 Swprintf_s(NewList[Index].Name, Length, L"<%s>", FileInfo->FileName);
            else
                 Swprintf_s(NewList[Index].Name, Length, L"%s", FileInfo->FileName);

            // Add the string to the HiiDataBase
            ///NewList[Index].Token = AddStringToHii(FileInfo->FileName, &gInternalStrings);    ///Just by trying using the following line
            NewList[Index].Token =     HiiAddString(gHiiHandle, NewList[Index].Name );

            // Clear the memory and create the string for the File Structure
            MemSet(NewList[Index].Name, Length, 0);
             Swprintf_s(NewList[Index].Name, Length, L"%s", FileInfo->FileName);
        }
        *List = NewList;
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   CreateFileList
//
// Description: Parse all the files in the current directory and add valid files to the
//      FILE_TYPE list and update the filecount
//
// Input:   EFI_FILE_PROTOCOL *FileProtocol - the current directory to parse
//
// Output:  FILE_TYPE **FileList - pointer in which to return the array of FileType items
//          UINTN *FileCount - the count of filetype items discovered
//
// Returns: EFI_STATUS
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
static EFI_STATUS CreateFileList(EFI_FILE_PROTOCOL *FileProtocol, FILE_TYPE **FileList, UINTN *FileCount)
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINTN   BufferSize = 1;
    EFI_FILE_INFO *File = NULL;

//    CHAR16 ExtensionEfi[] = L"EFI";

    // Continue parsing the directory until we no longer read valid files
    while(BufferSize != 0 && !EFI_ERROR(Status))
    {
        BufferSize = 0;
        Status = FileProtocol->Read(FileProtocol, &BufferSize, NULL);

        if(!EFI_ERROR(Status)) break;

        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            File = EfiLibAllocateCopyPool(BufferSize, File);
            if(File != NULL) {
                    MemSet(File, BufferSize, 0);
                }
        }

        Status = FileProtocol->Read(FileProtocol, &BufferSize, File);

        // Check if a valid file was read
        if(!EFI_ERROR(Status) && BufferSize != 0)
        {
            // check if the file read was a directory or a ".efi" extension
//            if(CheckDirectoryType(File) ||  CheckExtension(File, ExtensionEfi))
//            {
                // the file was valid, add it to the file list
                AddFileTypeEntry(FileList, FileCount, File);
//            }
        }

        // free the space allocated for reading the file info structure
        pBS->FreePool(File);

        // set the pointer to null to prevent the chance of memory corruption
        File = NULL;
    }

    if(*FileCount == 0) Status = EFI_NOT_FOUND;

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   DisplayFileListMenu
//
// Description: Display a menu of the FILE_TYPE items and return the selected item
//              in the Selection
//
// Input:   FILE_TYPE *FileList - List of FILE_TYPE items to display in the menu
//          UINTN FileCount - the number of FILE_TYPE items in the list
//
// Output:  UINT16 *Selection - The index of the selected FILE_TYPE item
//
// Returns:
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
static EFI_STATUS DisplayFileListMenu(FILE_TYPE *FileList, UINTN FileCount, UINT16 *Selection)
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINT16 i = 0;

    POSTMENU_TEMPLATE *List = NULL;

    // Check there are any files to display
    if(FileCount != 0 && FileList != NULL)
    {
        // allocate space for the POSTMENU_TEMPLATE items
        List = EfiLibAllocateCopyPool(sizeof(POSTMENU_TEMPLATE)*FileCount, List);
        if(List != NULL)
        {
            // Clear the memory allocated 
            MemSet(List, sizeof(POSTMENU_TEMPLATE)*FileCount, 0);

            // Add the STRING_REF tokens to the POSTMENU_TEMPLATE structures
            for(i = 0; i < FileCount; i++)
                List[i].ItemToken = FileList[i].Token;
        }

        // Call post manager to display the menu
        Status = mPostMgr->DisplayPostMenu(gHiiHandle,
                                            STRING_TOKEN(STR_FILE_SELECT), // Change this
                                            0,
                                            List,
                                            (UINT16)FileCount,
                                            Selection);
    }
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   UpdateFilePathString
//
// Description: To create the File Path string based on the file selected.
//
// Input:   CHAR16 *FilePath  - Buffer to fill with the file path
//          CHAR16 * CurFile  - current file selected
//          UINT16 idx        - Index of the file in the current directory
//
// Output:  CHAR16 *FilePath - Updated File Path
//
// Returns:
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UpdateFilePathString(CHAR16 *FilePath, CHAR16 * CurFile, UINT16 idx)
{
    UINTN Length=0;

    if(Wcslen(FilePath))
    {
        if( idx==0 ) {
            if(Wcscmp(CurFile,L".."))  {
                EfiStrCat(FilePath,L"\\");
                EfiStrCat(FilePath,CurFile);
            }
            else {
                
                for ( Length = Wcslen(FilePath); ( Length!= 0 ) && (FilePath[Length-1] != L'\\') ; Length -- ); 
                    if ( Length )
                        FilePath[Length-1] = L'\0';
                    else
                        FilePath[Length] = L'\0'; 
            }
        }
        else {
            EfiStrCat(FilePath,L"\\");
            EfiStrCat(FilePath,CurFile);
        }
    }
    else {
        Wcscpy(FilePath,CurFile);
    }
}

EFI_STATUS FileBrowserLaunchFilePath(IN EFI_HANDLE *FileHandle, IN BOOLEAN ValidOption, OUT CHAR16 **outFilePath, OUT UINT8 **outFileBuf,OUT UINTN *size );

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   FileBrowserLaunchFileSystem
//
// Description: To select the File System for the new boot option with the help of file browser.
//
// Input:   BOOLEAN bSelectFile - TRUE  - Select FSHandle and File path
//                                FALSE - Select only FSHandle
//
// Output:  Selected File System Index
//
// Returns: EFI_STATUS
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FileBrowserLaunchFileSystem(BOOLEAN bSelectFile, OUT EFI_HANDLE **outFsHandle,OUT CHAR16 **outFilePath, OUT UINT8 **outFileBuf,OUT UINTN *size )
{
    EFI_STATUS Status;
    UINTN Count = 0;
    UINT16 i = 0;
    EFI_HANDLE *gSmplFileSysHndlBuf = NULL;    
    UINT16 gSelIdx=0;
    BOOLEAN ValidOption = FALSE;
    
    EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;

    POSTMENU_TEMPLATE *PossibleFileSystems = NULL;

    if(mPostMgr == NULL)
        return EFI_UNSUPPORTED;
    
    // To launch the files from the selected file system
    if(!size)
        return EFI_INVALID_PARAMETER;

    // Locate all the simple file system devices in the system
    Status = pBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &Count, &gSmplFileSysHndlBuf);
    if(!EFI_ERROR(Status))
    {
        // allocate space to display all the simple file system devices
        PossibleFileSystems = EfiLibAllocateCopyPool(sizeof(POSTMENU_TEMPLATE)*Count,PossibleFileSystems);
        if(PossibleFileSystems != NULL)
        {
            // clear the allocated space
            MemSet(PossibleFileSystems, sizeof(POSTMENU_TEMPLATE)*Count, 0);
            for(i = 0; i < Count; i++)
            {
                // get the device path for each handle with a simple file system
                Status = pBS->HandleProtocol(gSmplFileSysHndlBuf[i], &gEfiDevicePathProtocolGuid, &Dp);
                if(!EFI_ERROR(Status))
                {
                    CHAR16 *Name;
                    CHAR8  *Name8;
                    // Get the name of the driver installed on the handle
                    // GetControllerName(gHandleBuffer[i],&Name);

                    Name8 = NULL;
                    Status  = DevicePathToStr(Dp, &Name8 );
                    Name = StrDup8to16(Name8);

                    // Add the name to the Hii Database
                    //PossibleFileSystems[i].ItemToken = AddStringToHii(Name);
                    PossibleFileSystems[i].ItemToken = HiiAddString(gHiiHandle, Name ); 

                    PossibleFileSystems[i].Attribute = AMI_POSTMENU_ATTRIB_FOCUS;
                    pBS->FreePool(Name);
                    pBS->FreePool(Name8);
                }
                else
                {
                    PossibleFileSystems[i].ItemToken = 0;
                    PossibleFileSystems[i].Attribute = AMI_POSTMENU_ATTRIB_HIDDEN;
                }
            }
            // Reset the item selected to be the first item
            gSelIdx = 0;

            // Display the post menu and wait for user input
            Status = mPostMgr->DisplayPostMenu(gHiiHandle,
                                                STRING_TOKEN(STR_FILE_SYSTEM),
                                                0,
                                                PossibleFileSystems,
                                                (UINT16)Count,
                                                &gSelIdx);

            // A valid item was selected by the user
            if(!EFI_ERROR(Status))
            {
                ValidOption = TRUE;
            }
        }
        // Free the allocated menu list space
        if(PossibleFileSystems != NULL) 
            pBS->FreePool(PossibleFileSystems);

         *outFsHandle = gSmplFileSysHndlBuf[gSelIdx];

         *size = 0; 
         if(bSelectFile)
             Status = FileBrowserLaunchFilePath(*outFsHandle, ValidOption, outFilePath, outFileBuf,size );//EIP:41615 Returning the status of Filebrowselaunchfilepath
    }
    
    else {
        GetHiiString(gHiiHandle, STRING_TOKEN(STR_NO_VALID_FS_TITLE),  sizeof(StrTitle) ,StrTitle);
        GetHiiString(gHiiHandle, STRING_TOKEN(STR_NO_VALID_FS), sizeof(StrMessage),StrMessage);
        mPostMgr->DisplayMsgBox( StrTitle, StrMessage, MSGBOX_TYPE_OK, NULL );//EIP:41615  To display Warning message when there is no file system connected.
    }
    
     if(gSmplFileSysHndlBuf != NULL) 
        pBS->FreePool(gSmplFileSysHndlBuf);

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   FileBrowserLaunchFilePath
//
// Description: To select the Boot file for the new boot option with the help of file browser.
//
// Input:   VOID
//
// Output:  File Path string
//
// Returns: EFI_STATUS
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FileBrowserLaunchFilePath(IN EFI_HANDLE *FileHandle,IN BOOLEAN ValidOption, OUT CHAR16 **outFilePath, OUT UINT8 **outFileBuf,OUT UINTN *size )
{
    EFI_STATUS Status;

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFs = NULL;
    EFI_FILE_PROTOCOL *NewFs = NULL;
    FILE_TYPE *FileList = NULL;
    UINTN FileCount = 0;
    UINT16 i = 0;
    CHAR16 FilePath[120];
    EFI_FILE_PROTOCOL *gFs = NULL;

    // Attempt to locate the post manager protocol
    if(mPostMgr == NULL)
        return EFI_UNSUPPORTED;

    if( ValidOption == TRUE ) 
    {

        // Get the simple file system protocol 
        Status = pBS->HandleProtocol(FileHandle, &gEfiSimpleFileSystemProtocolGuid, &SimpleFs);
        if(!EFI_ERROR(Status))
        {
            // And open it and return the efi file protocol
            Status = SimpleFs->OpenVolume(SimpleFs, &gFs);
        }
    }
    else {
            return EFI_UNSUPPORTED;
    }

    // clean up the file list and strings used in getting the file system
    CleanFileTypes(&FileList, &FileCount);

    while(!EFI_ERROR(Status) && gFs != NULL)
    {
        i = 0;
        MemSet(FilePath, sizeof(FilePath), 0);

        // Create a list of the files in the current directory
        Status = CreateFileList(gFs, &FileList, &FileCount);
        if(!EFI_ERROR(Status))
        {
            // Display the list in a menu and allow the user to select
            Status = DisplayFileListMenu(FileList, FileCount, &i);
            if(!EFI_ERROR(Status))
            {
                // The user selected something, attempt to open it
                Status = gFs->Open(  gFs,
                                    &NewFs,
                                    FileList[i].Name,
                                    EFI_FILE_MODE_READ,
                                    0);

                // close the old file system protocol and set it to null
                gFs->Close(gFs);
                gFs = NULL;

                // Create the File Path based on the file selected
                UpdateFilePathString(FilePath, FileList[i].Name, i);

                // the newly selected item was opened correctly
                if(!EFI_ERROR(Status))
                {
                    // check what type was opened
                    if(FileList[i].Type != EFI_FILE_DIRECTORY)
                    {

                        
                        Status = pBS->AllocatePool(EfiBootServicesData,FileList[i].Size, (VOID**)outFileBuf);
                        if(!EFI_ERROR(Status))
                        {
                            *size = FileList[i].Size;
                            // The user selected something, attempt to open it
                            Status = NewFs->Read( NewFs, size, *outFileBuf); }

                        // the file was read, close the file system protocol and set it to null
                        NewFs->Close(NewFs);
                        NewFs = NULL;
                        //Swprintf_s (FileName, 50, L"%s", FileList[i].Name);
                        //ShowPostMsgBox( L"Selected Boot File Name", FileName, MSGBOX_TYPE_OK, &SelOpt );
                    }
                    gFs = NewFs;
                }
            }
        }

        if(FileCount <= 0) {
            GetHiiString(gHiiHandle, STRING_TOKEN(STR_NO_VALID_FILE_TITLE),sizeof(StrTitle), StrTitle);
            GetHiiString(gHiiHandle, STRING_TOKEN(STR_NO_VALID_FILE),sizeof(StrMessage),StrMessage);
            mPostMgr->DisplayMsgBox( StrTitle, StrMessage, MSGBOX_TYPE_OK, NULL );//EIP:41615 Warning message to show unavailability of the selected file
        }

        // clean the strings that were used and free allocated space
        CleanFileTypes(&FileList, &FileCount);

        if(Status == EFI_ABORTED) {
            return Status;//EIP:41615 Returning the status if its aborted.
        }
    }
    // Set the File path for the new boot option added.
    Status = pBS->AllocatePool(EfiBootServicesData, ((Wcslen(FilePath)+1)*sizeof(CHAR16)), (void**)outFilePath);
    Wcscpy (*outFilePath, FilePath);

    return Status;
}
#endif // #ifdef NO_SETUP_COMPILE

#ifdef NO_SETUP_COMPILE
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SecureMod_Init
//
// Description: Entry point of Secure Module DXE driver
//
// Input:       EFI_HANDLE           ImageHandle,
//              EFI_SYSTEM_TABLE     *SystemTable
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI 
SecureBootMod_Init (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS Status;
    UINTN      DataSize;
    UINT32     Attributes;
    UINT8      DeployedMode, SetupMode;
    VOID      *pHobList = NULL;
    static EFI_GUID gHobListGuid  = HOB_LIST_GUID;

    InitAmiLib(ImageHandle, SystemTable);

    DataSize = sizeof(SECURE_BOOT_SETUP_VAR);
    Status = pRS->GetVariable (AMI_SECURE_BOOT_SETUP_VAR, &guidSecurity,&Attributes,&DataSize,&SecureBootSetup);

    TRACE((TRACE_ALWAYS,"SecureBootSetup (%r) Attrib=%x,\nSecBoot-%d, SecMode-%d, DefaultProvision-%d\n", 
           Status, Attributes, SecureBootSetup.SecureBootSupport, SecureBootSetup.SecureBootMode, SecureBootSetup.DefaultKeyProvision));

    // Provisioning of secure boot defaults
    if(SecureBootSetup.DefaultKeyProvision == 1) {
        DataSize = 1;
        Status = pRS->GetVariable(EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &SetupMode);
        if(!EFI_ERROR(Status) && SetupMode == SETUP_MODE) {
            Status = InstallSecureVariables(SET_NV_DEFAULT_KEYS);
            // Status of last key to be installed - PK
            if(!EFI_ERROR(Status)) {
                // Issue reset only if in User Mode and SecureBootSupport flag is enabled
                Status = pRS->GetVariable(EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &SetupMode);
                if(!EFI_ERROR(Status) && SetupMode == USER_MODE) {
                    DeployedMode = 1;
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT && DEFAULT_SECURE_BOOT_DEPLOYED_MODE
                    Status = pRS->SetVariable(EFI_DEPLOYED_MODE_NAME,&gEfiGlobalVariableGuid, 
                                EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS, 1, &DeployedMode);
                    TRACE((TRACE_ALWAYS,"Force '%S' %r\n",EFI_DEPLOYED_MODE_NAME,  Status));
#endif
                    if(SecureBootSetup.SecureBootSupport==SECURE_BOOT) {
                        //Get Boot Mode
                        pHobList = GetEfiConfigurationTable(SystemTable, &gHobListGuid);
                        if (pHobList) {
                            if (((EFI_HOB_HANDOFF_INFO_TABLE*)pHobList)->BootMode!=BOOT_IN_RECOVERY_MODE && 
                                ((EFI_HOB_HANDOFF_INFO_TABLE*)pHobList)->BootMode!=BOOT_ON_FLASH_UPDATE
                            ) 
                                pRS->ResetSystem(EfiResetCold, Status, 0, NULL);
                        }
                    }
                }
            }
        }
    }
    // Always install xxDefault variables
    InstallSecureVariables(SET_RT_DEFAULT_KEYS);

    return EFI_SUCCESS;
}
#endif // #ifdef NO_SETUP_COMPILE
#endif // #ifndef TSE_FOR_APTIO_4_50

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
