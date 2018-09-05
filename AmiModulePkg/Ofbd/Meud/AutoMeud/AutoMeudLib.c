//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AutoMeudLib.c

**/

//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <AmiDxeLib.h>
#include <AmiHobs.h>
#include <Capsule.h>
#include <Token.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/HobLib.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/HobList.h>
#include <Protocol/Decompress.h>
#include <Protocol/GuidedSectionExtraction.h>
#include <Protocol/MeFwUpdLclProtocol.h>
#include <Protocol/AMIPostMgr.h>
//----------------------------------------------------------------------------
// Function Externs

static EFI_GUID gMeFwUpdVariableGuid = ME_FW_UPD_VARIABLE_GUID;
extern UINT8 *RecoveryBuffer;

//----------------------------------------------------------------------------
// Local prototypes
#define FLASH_DEVICE_BASE (0xFFFFFFFF - FLASH_SIZE + 1)
#define ME_UPD_LCL_SIGNATURE SIGNATURE_32 ('_', 'M', 'U', 'L')

typedef enum {
    FvTypeMain,
    FvTypeBootBlock,
    FvTypeNvRam,
    FvTypeCustom,
    FvTypeMax
} FLASH_FV_TYPE;

typedef struct _FLASH_AREA_EX FLASH_AREA_EX;

typedef EFI_STATUS (REFLASH_FUNCTION_EX)(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
);

struct _FLASH_AREA_EX
{
    UINT8 *BlockAddress;
    UINT8 *BackUpAddress;
    UINT32 RomFileOffset;
    UINTN Size;
    UINT32 BlockSize;
    FLASH_FV_TYPE Type;
    BOOLEAN Update;
    BOOLEAN TopSwapTrigger;
    REFLASH_FUNCTION_EX *BackUp;
    REFLASH_FUNCTION_EX *Program;
    EFI_STRING_ID BackUpStringId;
    EFI_STRING_ID ProgramStringId;
};

typedef struct {
    EFI_GUID    Guid;
    UINT8       Type;
} FW_SUPPORT_FILE;

extern FLASH_AREA_EX *BlocksToUpdate;
//----------------------------------------------------------------------------
// Local Variables
EFI_GUID gMeVersionFfsFileGuid = \
  {0x0B4AE6BE, 0x6DA6, 0x4908, 0x8A, 0x71, 0x7E, 0x6A, 0x8A, 0x33, 0xB1, 0x1C};
EFI_GUID gMeVersionFfsSectionGuid = \
  {0x6A6D576A, 0x8F38, 0x45E7, 0x97, 0xC0, 0x8A, 0xCD, 0x9E, 0x99, 0x26, 0x74};
static EFI_GUID AmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
static AMI_POST_MANAGER_PROTOCOL *gAmiPostMgr = NULL;
const ME_FW_UPD_VERSION  gMeFwImgVersion[] = ME_FW_IMAGE_VERSION;
ME_FW_UPD_VERSION     MbpMeFwVer;

static FW_SUPPORT_FILE FwUpdateSupportFileList[] = {
#if defined(ISH_FW_UPDATE_SUPPORT) && (ISH_FW_UPDATE_SUPPORT == 1)
  {{0xDD7E6038, 0x3049, 0x4E76, 0x96, 0x8F, 0xA2, 0xC2, 0x5B, 0x07, 0xC2, 0x84}, ISH_FIRMWARE},
  {{0xD6F5D3F2, 0x96C0, 0x4EA6, 0xB2, 0x00, 0xC8, 0xB7, 0xE3, 0x63, 0x66, 0x40}, PDT_DATA},
#endif
  {{0xDE90FFA8, 0xB985, 0x4575, 0xAB, 0x8D, 0xAD, 0xE5, 0x2C, 0x36, 0x2C, 0xA3}, ME_FIRMWARE},
  {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, MAX_FIRMWARE_TYPE}
};
//----------------------------------------------------------------------------
// Function Definitions
/**
    Decompress the binary and return NVAR region.
    If the binary is not compressed, just copy NVAR region to the
    buffer and return it

    @param BinaryBuffer A pointer to the memory
        address of binary buffer
    @param BinaryBufferSize The size of the binary
    @param DecompressBuffer The buffer contain
        decompress section's data
    @param DecompressBufferSize Decompress section's data
        size

    @retval EFI_STATUS Return the EFI Stauts

**/
EFI_STATUS
DecompressBinary(
    IN  UINTN   *BinaryBuffer,
    IN  UINTN   BinaryBufferSize,
    OUT UINTN   **DecompressBuffer,
    OUT UINTN   *DecompressBufferSize
)
{
    EFI_STATUS                              Status;
    EFI_FFS_FILE_HEADER                     *FfsFileHeader = NULL;
    EFI_COMMON_SECTION_HEADER               *SectionHeader = NULL;
    UINTN                                   NvarSize = 0;
    VOID                                    *NvarBuffer = NULL;
    EFI_COMPRESSION_SECTION                 *CompressSection = NULL;
    EFI_GUID_DEFINED_SECTION                *GuidedSection = NULL;
    EFI_DECOMPRESS_PROTOCOL                 *Decompress = NULL;
    EFI_GUIDED_SECTION_EXTRACTION_PROTOCOL  *GuidedExtraction = NULL;
    VOID                                    *CompressionSource = NULL;
    UINT32                                  CompressionSourceSize = 0;
    UINT32                                  UncompressedLength = 0;
    UINT8                                   CompressionType;
    VOID                                    *ScratchBuffer = NULL;
    UINT32                                  ScratchSize = 0;
    VOID                                    *NewBuffer = NULL;
    UINTN                                   NewBufferSize = 0;
    UINT32                                  AuthenticationStatus = 0;

    FfsFileHeader = (EFI_FFS_FILE_HEADER*)BinaryBuffer;
    SectionHeader = (EFI_COMMON_SECTION_HEADER*)((UINT8*)FfsFileHeader + sizeof(EFI_FFS_FILE_HEADER));

    if( FfsFileHeader->Type == EFI_FV_FILETYPE_RAW )
    {
        //The binary is the ffs without any section,
        //allocate the memory and copy it.

        NvarSize = BinaryBufferSize - sizeof(EFI_FFS_FILE_HEADER);

        Status = pBS->AllocatePool(
                        EfiBootServicesData,
                        NvarSize,
                        &NvarBuffer );
        if( EFI_ERROR(Status) )
            return Status;

        MemCpy( NvarBuffer, (VOID*)SectionHeader, NvarSize );

        *DecompressBuffer = NvarBuffer;
        *DecompressBufferSize = NvarSize;

        return EFI_SUCCESS;
    }
    //The binary is the ffs with section,
    //determine the section type.

    switch( SectionHeader->Type )
    {
        case EFI_SECTION_COMPRESSION:

            //The section is compressed by PI_STD
            CompressSection = (EFI_COMPRESSION_SECTION*)SectionHeader;

            if( IS_SECTION2(CompressSection) )
            {
                CompressionSource = (VOID*)((UINT8*)CompressSection + sizeof(EFI_COMPRESSION_SECTION2));
                CompressionSourceSize = (UINT32)(SECTION2_SIZE(CompressSection) - sizeof (EFI_COMPRESSION_SECTION2));
                UncompressedLength = ((EFI_COMPRESSION_SECTION2*)CompressSection)->UncompressedLength;
                CompressionType = ((EFI_COMPRESSION_SECTION2*)CompressSection)->CompressionType;
            }
            else
            {
                CompressionSource = (VOID*)((UINT8*)CompressSection + sizeof(EFI_COMPRESSION_SECTION));
                CompressionSourceSize = (UINT32)(SECTION_SIZE(CompressSection) - sizeof(EFI_COMPRESSION_SECTION));
                UncompressedLength = CompressSection->UncompressedLength;
                CompressionType = CompressSection->CompressionType;
            }

            if( UncompressedLength > 0 )
            {
                NewBufferSize = UncompressedLength;

                Status = pBS->AllocatePool(
                                EfiBootServicesData,
                                NewBufferSize,
                                &NewBuffer );
                if( EFI_ERROR(Status) )
                    return Status;

                if( CompressionType == EFI_NOT_COMPRESSED )
                {
                    MemCpy( NewBuffer, (VOID*)CompressionSource, NewBufferSize );
                }
                else if ( CompressionType == EFI_STANDARD_COMPRESSION )
                {
                    Status = pBS->LocateProtocol(
                                    &gEfiDecompressProtocolGuid,
                                    NULL,
                                    &Decompress );
                    if( EFI_ERROR(Status) )
                    {
                        pBS->FreePool( NewBuffer );
                        return Status;
                    }

                    Status = Decompress->GetInfo(
                                            Decompress,
                                            CompressionSource,
                                            CompressionSourceSize,
                                            (UINT32 *)&NewBufferSize,
                                            &ScratchSize );
                    if( EFI_ERROR(Status) || (NewBufferSize != UncompressedLength) )
                    {
                        pBS->FreePool( NewBuffer );
                        if( !EFI_ERROR(Status) )
                            Status = EFI_BAD_BUFFER_SIZE;
                        return Status;
                    }

                    Status = pBS->AllocatePool(
                                    EfiBootServicesData,
                                    ScratchSize,
                                    &ScratchBuffer );
                    if( EFI_ERROR(Status) )
                    {
                        pBS->FreePool( NewBuffer );
                        return Status;
                    }

                    Status = Decompress->Decompress(
                                            Decompress,
                                            CompressionSource,
                                            CompressionSourceSize,
                                            NewBuffer,
                                            (UINT32)NewBufferSize,
                                            ScratchBuffer,
                                            ScratchSize );
                    pBS->FreePool( ScratchBuffer );
                    if( EFI_ERROR(Status) )
                    {
                        pBS->FreePool( NvarBuffer );
                        return Status;
                    }

                    //The decompressed data is the EFI_SECTION_RAW
                    //Add the section header length to get data
                    SectionHeader = (EFI_COMMON_SECTION_HEADER*)NewBuffer;

                    NvarSize = SECTION_SIZE(SectionHeader) - sizeof(EFI_COMMON_SECTION_HEADER);
                    NvarBuffer = (VOID*)((UINT8*)NewBuffer + sizeof(EFI_COMMON_SECTION_HEADER));
                }
            }

            break;

        case EFI_SECTION_GUID_DEFINED:

            //The section is compressed by LZMA
            GuidedSection = (EFI_GUID_DEFINED_SECTION*)SectionHeader;

            Status = pBS->LocateProtocol(
                            &(GuidedSection->SectionDefinitionGuid),
                            NULL,
                            &GuidedExtraction );
            if( EFI_ERROR(Status) )
                return Status;

            Status = GuidedExtraction->ExtractSection(
                                        GuidedExtraction,
                                        GuidedSection,
                                        &NewBuffer,
                                        &NewBufferSize,
                                        &AuthenticationStatus );
            if( EFI_ERROR(Status) )
                return Status;

            //The decompressed data is the EFI_SECTION_RAW
            //Add the section header length to get data
            SectionHeader = (EFI_COMMON_SECTION_HEADER*)NewBuffer;

            NvarSize = SECTION_SIZE(SectionHeader) - sizeof(EFI_COMMON_SECTION_HEADER);
            NvarBuffer = (VOID*)((UINT8*)NewBuffer + sizeof(EFI_COMMON_SECTION_HEADER));

            break;

        case EFI_SECTION_RAW:

            //The section is not compressed.
            NvarSize = BinaryBufferSize - (sizeof(EFI_FFS_FILE_HEADER) + sizeof(EFI_COMMON_SECTION_HEADER));

            Status = pBS->AllocatePool(
                            EfiBootServicesData,
                            NvarSize,
                            &NvarBuffer );
            if( EFI_ERROR(Status) )
                return Status;

            MemCpy( NvarBuffer, (VOID*)((UINT8*)SectionHeader + sizeof(EFI_COMMON_SECTION_HEADER)), NvarSize );

            break;

        default:

            NvarBuffer = NULL;
            NvarSize = 0;

            break;
    }

    *DecompressBuffer = NvarBuffer;
    *DecompressBufferSize = NvarSize;

    return Status;
}
/**
    @param Buffer 
    @param pOFBDDataHandled 
    @retval VOID
**/
UINT8*
GetFvMeFwRegionAddress(
    UINT8   *StartAddress
)
{
#if (AUTO_ME_UPDATE_POLICY == 2)
    EFI_GUID mMeFwCapsuleFirmwareVolumeGuid = ME_FW_CAPSULE_FIRMWARE_VOLUME_GUID;
    EFI_FIRMWARE_VOLUME_HEADER *FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)StartAddress;
    // Check the ME FW Capsule at the start of update image.
    // Use firmware volume extended header to check whether the ME capsule.
    if( FvHeader->Signature == EFI_FVH_SIGNATURE )
    {
        if( !guidcmp( (UINT8*)FvHeader + FvHeader->ExtHeaderOffset, \
                      &mMeFwCapsuleFirmwareVolumeGuid ) ) return (UINT8*)FvHeader;
    }
    // Check the ME FW Capsule beyond the update image.
    FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)(StartAddress + FLASH_SIZE);
    if( FvHeader->Signature == EFI_FVH_SIGNATURE )
    {
        if( !guidcmp( (UINT8*)FvHeader + FvHeader->ExtHeaderOffset, \
                      &mMeFwCapsuleFirmwareVolumeGuid ) ) return (UINT8*)FvHeader;
    }
    return NULL;
#else
    return StartAddress;
#endif
}
/**
    @param Buffer
    @param pOFBDDataHandled

    @retval VOID
**/
static
UINT8*
FvFindFfsFileByGuid(
    UINT8       *FvAddress,
    EFI_GUID    *pGuid
)
{
    EFI_FFS_FILE_HEADER     *pFfsFile;
    UINT32                  i, FvLength = FLASH_SIZE, FileSize;
    UINT8                   *p = (UINT8*)NULL;

    if( (p = GetFvMeFwRegionAddress(FvAddress)) == NULL ) return NULL;
    FvLength = (UINT32)((EFI_FIRMWARE_VOLUME_HEADER*)p)->FvLength;
    for( i = 0; (i + 16) < FvLength; i += 16 )
    {
        if( guidcmp( &((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->FileSystemGuid, \
                                         &gEfiFirmwareFileSystem2Guid ) ) continue;
        FileSize = ((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->HeaderLength;
        pFfsFile = (EFI_FFS_FILE_HEADER*)(p + i + FileSize);
        do
        {
            if( !guidcmp( &((EFI_FFS_FILE_HEADER*)pFfsFile)->Name, pGuid ) )
            {
                return ((UINT8*)pFfsFile);    
            }
            FileSize = *(UINT32 *)pFfsFile->Size & 0x00FFFFFF;
            pFfsFile = (EFI_FFS_FILE_HEADER*)((UINT32)pFfsFile + FileSize);
            pFfsFile = (EFI_FFS_FILE_HEADER*)(((UINT32)pFfsFile + 7) & 0xfffffff8);
        }while( ((*(UINT32 *)pFfsFile->Size & 0x00FFFFFF) != 0xFFFFFF) && \
                 ((*(UINT32 *)pFfsFile->Size & 0x00FFFFFF) != 0) );
        i += (UINT32)(((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->FvLength - 16);
    }
    return ((UINT8*)NULL);
}
/**
    @param Buffer
    @param pOFBDDataHandled

    @retval VOID
**/
EFI_STATUS 
MeFwUpdateViaIntelLib(
    IN  UINT8                       *pBuffer,
    IN  AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr
)
{
    EFI_STATUS                  Status;
    UINT8                       i;
    EFI_GUID                    mMeFwUpdLclProtocolGuid = ME_FW_UPD_LOCAL_PROTOCOL_GUID;
    ME_FW_UPDATE_LOCAL_PROTOCOL *mMeFwUpdateLclProtocol = NULL;

    // Locate ME FW Update Local Protocol.
    Status = pBS->LocateProtocol( &mMeFwUpdLclProtocolGuid, \
                                        NULL, (VOID**)&mMeFwUpdateLclProtocol );
    if( EFI_ERROR(Status) || (pBuffer == NULL) ) return Status;

    // Check ME is in Normal mode.
    Status = mMeFwUpdateLclProtocol->MeFwCheckMode( mMeFwUpdateLclProtocol, MeModeNormal );
    if( EFI_ERROR(Status) ) return Status;

    // Update reflash progress bar only if Secure flash capsule update.
    // Do not display any messages if Windwos Firmware Update Capsule.
    mMeFwUpdateLclProtocol->AmiPostMgr = NULL;
    if( AmiPostMgr != NULL ) mMeFwUpdateLclProtocol->AmiPostMgr = AmiPostMgr;

    for( i = 0; FwUpdateSupportFileList[i].Type != MAX_FIRMWARE_TYPE; i++ )
    {
        UINTN               *pMeRegionFile, *UcMeBuffer, UcMeBufferSize;
        EFI_FFS_FILE_HEADER *pHdr;
        if( (pMeRegionFile = (UINTN*)FvFindFfsFileByGuid(\
                    pBuffer, &FwUpdateSupportFileList[i].Guid )) == NULL ) continue;
        pHdr = (EFI_FFS_FILE_HEADER*)pMeRegionFile;
        Status = DecompressBinary(
                    pMeRegionFile,
                    (UINTN)(*(UINT32*)pHdr->Size & 0xffffff),
                    &UcMeBuffer,
                    &UcMeBufferSize); 
        if( EFI_ERROR(Status) ) return Status;
        Status = mMeFwUpdateLclProtocol->FwUpdLcl( mMeFwUpdateLclProtocol, \
                (UINT8*)UcMeBuffer, UcMeBufferSize, FwUpdateSupportFileList[i].Type );
    }
    return Status;
}
/**
    @param pFileSection 

    @retval TRUE Version is correct
    @retval FALSE Version is not correct
**/
BOOLEAN 
CheckMeFirmwareVersion(
    UINT8   *pFileSection
)
{
    UINT8               *p = pFileSection + sizeof(EFI_COMMON_SECTION_HEADER);
    ME_FW_UPD_VERSION   *NewMeFwVer;

    if( guidcmp( p, &gMeVersionFfsSectionGuid ) ) return FALSE;
    NewMeFwVer = (ME_FW_UPD_VERSION*)(p + sizeof (EFI_GUID));

    // ??? PORTING REQUEST ??? if can't get ME FW version from MBP ??
    //    ==== PORTING REQUEST ==== >> 
    if( MbpMeFwVer.MajorVersion != NewMeFwVer->MajorVersion ) return FALSE;
    if( MbpMeFwVer.MinorVersion < NewMeFwVer->MinorVersion ) return FALSE;
    if( (UINT32)((NewMeFwVer->HotfixVersion << 16) + NewMeFwVer->BuildVersion) > 
        (UINT32)((MbpMeFwVer.HotfixVersion << 16) + MbpMeFwVer.BuildVersion) ) {
        return TRUE;
    }    
    // << ==== PORTING REQUEST ==== 
    return FALSE;
}
/**
    @param Buffer 
    @param pOFBDDataHandled 
    @retval VOID
**/
BOOLEAN 
IsMeFirmawareUpgraded(
    EFI_GUID    *pGuid
)
{
    UINT8   *pFfsFile;
    if( (pFfsFile = FvFindFfsFileByGuid( RecoveryBuffer, pGuid )) == NULL ) return FALSE;
    return CheckMeFirmwareVersion( pFfsFile + sizeof(EFI_FFS_FILE_HEADER) );
}
/**
    @param Buffer 
    @param pOFBDDataHandled 
    @retval VOID
**/
BOOLEAN
IsWindowsFwUpdate(
    VOID
)
{
    EFI_STATUS              Status;
    EFI_CAPSULE_HEADER      *Capsule;
    EFI_HOB_UEFI_CAPSULE    *Hob;
    static EFI_GUID         ImageCapsuleGuid = W8_SCREEN_IMAGE_CAPSULE_GUID;
    static EFI_GUID         HobListGuid = HOB_LIST_GUID;

    if( (Hob = GetEfiConfigurationTable( pST, &HobListGuid )) == NULL ) return FALSE;
    do
    {
        Status = FindNextHobByType( EFI_HOB_TYPE_UEFI_CAPSULE, &Hob );
        if( !EFI_ERROR(Status) )
        { 
            Capsule = (EFI_CAPSULE_HEADER*)(VOID*)(UINTN)Hob->BaseAddress;
            if( !guidcmp( &(Capsule->CapsuleGuid), &ImageCapsuleGuid ) ) return TRUE;
        }
    }while( !EFI_ERROR(Status) );
    return FALSE;
}
/**
    @param Buffer 
    @param pOFBDDataHandled 
    @retval VOID
**/
VOID
AutoMeudAfterReflashHook(
    VOID
)
{
    EFI_STATUS  Status = EFI_VOLUME_CORRUPTED;
    UINT32      MeMode;

    if( (gMeFwImgVersion->MajorVersion != 0) && \
        (!IsMeFirmawareUpgraded( &gMeVersionFfsFileGuid )) ) return ;
#if (AUTO_ME_UPDATE_POLICY != 0)
    if( !IsWindowsFwUpdate() )
        Status = pBS->LocateProtocol( &AmiPostManagerProtocolGuid, NULL, &gAmiPostMgr );
    // ME FW Update API could returns error "EFI_VOLUME_CORRUPTED" if capsule mode
    // in S3 resume path, so, we set the MeAutoUpdateReq variable to "_MUL" for 
    // trying update ME FW again if error.
    MeMode = ME_UPD_LCL_SIGNATURE;
    Status = MeFwUpdateViaIntelLib( RecoveryBuffer, gAmiPostMgr );
#endif
    if( EFI_ERROR(Status) )
    {    
        pRS->SetVariable(
                L"MeAutoUpdateReq",
                &gMeFwUpdVariableGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof(UINT32),
                &MeMode );
    }
    return;
}
/**
    @param Buffer
    @param pOFBDDataHandled

    @retval VOID
**/
VOID
AutoMeudBeforeReflashHook(
    VOID
)
{
    UINTN       Length = sizeof(ME_FW_UPD_VERSION);
    EFI_STATUS  Status;

    // Save "MeAutoUpdateReq" variable here for avoiding NVRAM could be updated later.
    Status = pRS->GetVariable(
                    L"MbpMeFwVersion",
                    &gMeFwUpdVariableGuid,
                    NULL,
                    &Length,
                    &MbpMeFwVer );
    if( EFI_ERROR(Status) )
        MemSet( &MbpMeFwVer, sizeof(ME_FW_UPD_VERSION), 0 ); 

#if (AUTO_ME_UPDATE_POLICY == 2)
    // Start ME FW update process here if ME FW Capsule only.
    if( RecoveryBuffer == GetFvMeFwRegionAddress( RecoveryBuffer ) )
    {
        EFI_EVENT event;
        AutoMeudAfterReflashHook();
        if( gAmiPostMgr != NULL )
        {
            gAmiPostMgr->DisplayInfoBox(
                            L"ME FW update",
                            L"ME FW update completed, Press any key to reset the system",
                            5,
                            &event );
        }
        pRS->ResetSystem( EfiResetCold, EFI_SUCCESS, 0, NULL );
    }
#endif

    return;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
