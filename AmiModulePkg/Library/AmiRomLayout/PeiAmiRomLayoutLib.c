//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  ROM Layout PEIM.
**/
#include <Library/AmiRomLayoutLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Guid/AmiRomLayout.h>
#include <Library/DebugLib.h>

// defines for FFS items
#define GET_AMI_ROM_AREA_FROM_AMI_ROM_LAYOUT_HEADER(a) ((AMI_ROM_AREA*)(((AMI_ROM_LAYOUT_HEADER*)a)+1))

#pragma pack(push)
#pragma pack(1)
/**
 * Structure that defines the layout of hte
 */
typedef struct _AMI_ROM_LAYOUT_SECTION {
    EFI_FFS_FILE_HEADER FfsFileHeader;                  //< The FFS file header
    EFI_FREEFORM_SUBTYPE_GUID_SECTION GuidedSection;    //< The Guided Section of the FFS file
    AMI_ROM_LAYOUT_HEADER RomLayoutHeader;
} AMI_ROM_LAYOUT_SECTION;

typedef struct {
    UINT32 Version;             //< Version from the AMI_ROM_LAYOUT_HEADER structure
    UINT32 DescriptorSize;      //< DescriptorSize from the AMI_ROM_LAYOUT_HEADER structure
    UINT32 HobEntrySize;        //< Total size of the
    UINT32 Reserved;            //< Reserved for future use
} AMI_ROM_LAYOUT_HOB_PAYLOAD;

#pragma pack(pop)

#define GET_AMI_ROM_LAYOUT_HOB_ENTRY_FROM_THIS(a) \
    BASE_CR( a, AMI_ROM_LAYOUT_HOB_ENTRY, Area )

/**
 * Constructor for the AmiRomLayoutLib PEI file
 * @param FileHandle The handle of the image that is consuming this constructor
 * @param PeiServices Double pointer to the Pei Services Table
 */
EFI_STATUS EFIAPI PeiAmiRomLayoutLibConstructor(
    IN EFI_PEI_FILE_HANDLE FileHandle, IN CONST EFI_PEI_SERVICES **PeiServices
) {
    return EFI_SUCCESS;
}

/**
 * Locate the RomLayout data from the Boot Firmware Volume (reported by the SEC core).
 * If the RomLayout FFS is correctly located, then create the AMI_ROM_LAYOUT_HOB from
 * the FFS's data.
 *
 * @return AMI_ROM_LAYOUT_HEADER A pointer to the AMI_ROM_LAYOUT_HEADER, as found in the FFS
 */
static AMI_ROM_LAYOUT_HEADER* GetAmiRomLayoutHeader(VOID)
{
    EFI_STATUS Status;

    EFI_PEI_FV_HANDLE BootFv = NULL;
    EFI_PEI_FILE_HANDLE File = NULL;

    EFI_FREEFORM_SUBTYPE_GUID_SECTION *GuidedSection = NULL;

    AMI_ROM_LAYOUT_HEADER *RomLayoutHeader;

    AMI_ROM_LAYOUT_HOB_PAYLOAD *PayLoad = NULL;
    AMI_ROM_LAYOUT_HOB_ENTRY *RomLayoutHobEntry = NULL;

    Status = PeiServicesFfsFindNextVolume(0, &BootFv);
    if(!EFI_ERROR(Status))
    {
        Status = PeiServicesFfsFindFileByName(&gAmiRomLayoutFfsFileGuid, BootFv, &File);
        if(!EFI_ERROR(Status))
        {
            RomLayoutHeader = &(((AMI_ROM_LAYOUT_SECTION*)File)->RomLayoutHeader);
            if(RomLayoutHeader->Signature == AMI_ROM_LAYOUT_SIGNATURE)
            {
                PayLoad = BuildGuidHob(&gAmiRomLayoutGuid, sizeof(AMI_ROM_LAYOUT_HOB) - sizeof(EFI_HOB_GUID_TYPE) + (RomLayoutHeader->NumberOfDescriptors * sizeof(AMI_ROM_LAYOUT_HOB_ENTRY)));
                if(PayLoad != NULL)
                {
                    AMI_ROM_AREA *RomArea = NULL;

                    PayLoad->Version = RomLayoutHeader->Version;
                    PayLoad->DescriptorSize = RomLayoutHeader->DescriptorSize;
                    PayLoad->HobEntrySize = sizeof(AMI_ROM_LAYOUT_HOB_ENTRY);
                    PayLoad->Reserved = 0;

                    for(RomArea = (AMI_ROM_AREA*)(RomLayoutHeader + 1), RomLayoutHobEntry = (AMI_ROM_LAYOUT_HOB_ENTRY*)(PayLoad + 1);
                        RomArea < (AMI_ROM_AREA*)((UINT8*)(RomLayoutHeader + 1) + (RomLayoutHeader->DescriptorSize * RomLayoutHeader->NumberOfDescriptors));
                        RomArea = (AMI_ROM_AREA*)((UINT8*)RomArea + RomLayoutHeader->DescriptorSize), RomLayoutHobEntry = RomLayoutHobEntry + 1)
                    {
                        CopyMem(&(RomLayoutHobEntry->Area), RomArea, RomLayoutHeader->DescriptorSize);
                        RomLayoutHobEntry->Reserved = 0;
                        RomLayoutHobEntry->Offset = (UINT32)((UINT8*)(RomLayoutHobEntry) - ((UINT8*)PayLoad - sizeof(EFI_HOB_GUID_TYPE)));
                    }
                }

                return RomLayoutHeader;
            }
        }
    }

    return NULL;
}

EFI_STATUS AmiGetRomLayoutProperties(
	OUT UINT32 *Version OPTIONAL, OUT UINT32 *RomAreaSize OPTIONAL, OUT UINT32 *RomLayoutSize OPTIONAL
){
    AMI_ROM_LAYOUT_HOB *AmiRomLayoutHob = NULL;

    AmiRomLayoutHob = GetFirstGuidHob (&gAmiRomLayoutGuid);
    if(AmiRomLayoutHob == NULL)
    {
        GetAmiRomLayoutHeader();
        AmiRomLayoutHob = GetFirstGuidHob (&gAmiRomLayoutGuid);
    }

	if (AmiRomLayoutHob==NULL) return EFI_NOT_FOUND;
	if (Version != NULL) *Version = AmiRomLayoutHob->Version;
	if (RomAreaSize != NULL) *RomAreaSize = AmiRomLayoutHob->DescriptorSize;
	if (RomLayoutSize != NULL) *RomLayoutSize = GET_GUID_HOB_DATA_SIZE(AmiRomLayoutHob)
												/ AmiRomLayoutHob->HobEntrySize
												* AmiRomLayoutHob->DescriptorSize;
	return EFI_SUCCESS;
}

/**
 * Return a pointer to the first structure describing the RomLayout of the system
 *
 * @return NULL There was no RomLayout information available
 * @return AMI_ROM_AREA Pointer to an AMI_ROM_ARE entry
 */
AMI_ROM_AREA* AmiGetFirstRomArea(VOID)
{
    AMI_ROM_AREA *Start = NULL;

    AMI_ROM_LAYOUT_HOB *AmiRomLayoutHob = NULL;

    AmiRomLayoutHob = GetFirstGuidHob (&gAmiRomLayoutGuid);
    if(AmiRomLayoutHob == NULL)
    {
        GetAmiRomLayoutHeader();
        AmiRomLayoutHob = GetFirstGuidHob (&gAmiRomLayoutGuid);
    }

    if(AmiRomLayoutHob != NULL)
        Start = &(((AMI_ROM_LAYOUT_HOB_ENTRY*)(AmiRomLayoutHob + 1))->Area);

    return Start;
}


/**
 * Return a pointer to the next RomLayout structure based upon the current structure Start
 *
 * @param Start Pointer to the current AMI_ROM_AREA structure
 * @return NULL The passed structure pointer is not a valid pointer
 * @return NULL There are no more structures in the system
 * @return AMI_ROM_AREA* Pointer to the next structure
 */
AMI_ROM_AREA* AmiGetNextRomArea(AMI_ROM_AREA *Start)
{
    AMI_ROM_LAYOUT_HOB_ENTRY *AmiHobEntry = NULL;
    AMI_ROM_LAYOUT_HOB *AmiRomLayoutHob = NULL;

    AMI_ROM_LAYOUT_HOB_ENTRY *AmiRomLayoutHobStart;
    AMI_ROM_LAYOUT_HOB_ENTRY *AmiRomLayoutHobEnd;
    if(Start != NULL)
    {
        AmiHobEntry = GET_AMI_ROM_LAYOUT_HOB_ENTRY_FROM_THIS(Start);

        AmiRomLayoutHob = (AMI_ROM_LAYOUT_HOB*)((UINT8*)AmiHobEntry - AmiHobEntry->Offset);

        if( CompareGuid(&(AmiRomLayoutHob->Header.Name), &gAmiRomLayoutGuid) )
        {
            AmiRomLayoutHobStart = (AMI_ROM_LAYOUT_HOB_ENTRY*)(AmiRomLayoutHob + 1);
            AmiRomLayoutHobEnd = ((AMI_ROM_LAYOUT_HOB_ENTRY*)((UINT8*)AmiRomLayoutHobStart + AmiRomLayoutHob->Header.Header.HobLength - sizeof(AMI_ROM_LAYOUT_HOB)));

            AmiHobEntry = (AMI_ROM_LAYOUT_HOB_ENTRY*)((UINT8*)AmiHobEntry + AmiRomLayoutHob->HobEntrySize);

            if( (AmiRomLayoutHobStart <= AmiHobEntry) && (AmiRomLayoutHobEnd > AmiHobEntry) )
                return &(AmiHobEntry->Area);
        }
    }
    return NULL;
}

/**
 * Return a pointer to the AMI_ROM_AREA structure which contains the region of this Address
 *
 * @param Address The physical address of the area to match
 * @return NULL No AMI_ROM_AREA structure contains this Address
 * @return AMI_ROM_AREA Pointer to the structure that contains this Address
 */
AMI_ROM_AREA* AmiGetRomAreaByAddress(IN EFI_PHYSICAL_ADDRESS Address)
{
    AMI_ROM_AREA *RomArea = NULL;
    for(RomArea = AmiGetFirstRomArea(); RomArea != NULL; RomArea = AmiGetNextRomArea(RomArea))
    {
        if(RomArea->Address <= Address && (RomArea->Address + RomArea->Size) >= Address)
            break;
    }
    return RomArea;
}

/**
 * Return a pointer to the first AMI_ROM_AREA structure that uses the passed GUID.
 *
 * @param AreaGuid Pointer to the Guid to match to an AMI_ROM_AREA structure
 * @return NULL No AMI_ROM_AREA structure was matched to that GUID
 * @return AMI_ROM_AREA Pointer to the structure that matched the GUID
 */
AMI_ROM_AREA* AmiGetFirstRomAreaByGuid(IN EFI_GUID *AreaGuid)
{
    AMI_ROM_AREA *RomArea = NULL;

    if(AreaGuid != NULL)
    {
        for(RomArea = AmiGetFirstRomArea(); RomArea != NULL; RomArea = AmiGetNextRomArea(RomArea))
        {
            if( CompareGuid(&RomArea->Guid, AreaGuid))
                break;
        }
    }
    return RomArea;
}

/**
 * Return a pointer to the next AMI_ROM_AREA structure that occurs after the passed AMI_ROM_AREA structure and which
 * uses the passed AreaGuid. There may be multiple areas in the RomLayout that use the same GUID. This
 * function will allow locating all
 * of them.
 *
 * @param AreaGuid Pointer to the Guid to match to an AMI_ROM_AREA structure
 * @param PreviousRomArea Pointer to the previous AMI_ROM_AREA
 * @return NULL The AreaGuid pointer, or the PreviousRomArea pointer were invalid
 * @return NULL There are no more structures in the system that use this guid
 * @return AMI_ROM_AREA Pointer to the next entry that uses this GUID
 */
AMI_ROM_AREA* AmiGetNextRomAreaByGuid(IN EFI_GUID *AreaGuid, IN AMI_ROM_AREA *PreviousRomArea)
{
    AMI_ROM_AREA *RomArea = NULL;

    if(PreviousRomArea != NULL && AreaGuid != NULL)
    {
        RomArea = AmiGetFirstRomArea();
        while( RomArea != NULL && (PreviousRomArea != RomArea))
            RomArea = AmiGetNextRomArea(RomArea);

        RomArea = AmiGetNextRomArea(RomArea);
        for( ; RomArea != NULL; RomArea = AmiGetNextRomArea(RomArea) )
        {
            if( CompareGuid(AreaGuid, &RomArea->Guid) )
                break;
        }
    }
    return RomArea;
}

/**
 * Locates the Rom Layout table inside of the the passed ImageAddress and returns the information
 * into the various passed parameters.
 *
 * @param ImageAddress Pointer to the start of the image.
 * @param ImageSize Size of the image.
 * @param RomLayout Double Pointer that, upon sucessfull locating of the Rom Layout Table, will be updated to point to the first AMI_ROM_AREA.
 * @param Version Pointer that, upon sucessfull locating of the Rom Layout Table, will be updated with the version information of the RomLayout structure
 * @param RomAreaSize Pointer that, upon sucessfull locating of the Rom Layout Table, will be updated with the size of each AMI_ROM_AREA entry
 * @param RomLayoutSize Pointer that, upon sucessfull locating of the Rom Layout Table, will be updated with the total size of the AMI_ROM_AREA entries.
 *
 * @return EFI_INVALID_PARAMETER The ImageAddress pointer, or the RomLayout pointer was invalid
 * @return EFI_NOT_FOUND The RomLayout information was not found in the Image
 * @return EFI_SUCCESS The RomLayout was found, and the returned data is valid
 */
EFI_STATUS AmiGetImageRomLayout(
	IN VOID *ImageAddress, IN UINTN ImageSize,
	OUT AMI_ROM_AREA **RomLayout, OUT UINT32 *Version OPTIONAL,
	OUT UINT32 *RomAreaSize OPTIONAL, OUT UINT32 *RomLayoutSize OPTIONAL
){
    UINT32 *SearchPointer;
    AMI_ROM_LAYOUT_SECTION *Section = NULL;

    if (ImageAddress == NULL || RomLayout == NULL)
        return EFI_INVALID_PARAMETER;

    SearchPointer = (UINT32 *)((UINT8*)ImageAddress - sizeof(EFI_GUID) + ImageSize);
    ASSERT( (UINTN)(UINT32)SearchPointer == ((UINTN)(UINT32)SearchPointer & (~0x07)) );

    // Do a manual search for the Loaded Recovery capsule searching for instances of the gAmiRomLayoutFfsFileGuid.
    //  For every match to the guid, check if it matches the full guid and subsection guid, and if it does,
    //  then treat it as the RomLayout from the recovery capsule. Note: This search starts at the end of the
    //  rom image and searches backwards.
    while(SearchPointer != (UINT32*)ImageAddress)
    {
        Section = (AMI_ROM_LAYOUT_SECTION*)SearchPointer;
        if( CompareGuid(&gAmiRomLayoutFfsFileGuid, &(Section->FfsFileHeader.Name)) &&
            CompareGuid(&gAmiRomLayoutGuid, &(Section->GuidedSection.SubTypeGuid)) )
        {

            if(Section->RomLayoutHeader.Signature == AMI_ROM_LAYOUT_SIGNATURE)
            {
                if(Version != NULL)
                    *Version = Section->RomLayoutHeader.Version;

                if(RomAreaSize != NULL)
                    *RomAreaSize = Section->RomLayoutHeader.DescriptorSize;

                if(RomLayoutSize != NULL)
                    *RomLayoutSize = Section->RomLayoutHeader.DescriptorSize * Section->RomLayoutHeader.NumberOfDescriptors;

                *RomLayout = GET_AMI_ROM_AREA_FROM_AMI_ROM_LAYOUT_HEADER(&(Section->RomLayoutHeader));
                return EFI_SUCCESS;
            }
        }
        SearchPointer = SearchPointer - 1;
    }

    return EFI_NOT_FOUND;
}

/**
 * Locate the AMI_ROM_ARE that uses the passed GUID, and publish into the system
 *
 * @param FvName Pointer to the GUID to match to a AMI_ROM_AREA, and then to publish
 * @return EFI_NOT_FOUND No AMI_ROM_AREAs were found that use the GUID
 */
EFI_STATUS AmiPublishFv(IN EFI_GUID *FvName)
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;

    AMI_ROM_AREA *RomArea;
    if(FvName != NULL)
    {
        RomArea = AmiGetFirstRomAreaByGuid(FvName);
        if(RomArea != NULL)
            Status = AmiPublishFvArea(RomArea);
        else
            Status = EFI_NOT_FOUND;
    }
    return Status;
}

/**
 * Report to the system the Firmware Volume described by the passed AMI_ROM_AREA.
 *
 * @param FvArea Pointer to the AMI_ROM_AREA structure to report to the system
 * @return EFI_INVALID_PARAMETER The FvArea is not a valid pointer
 * @return EFI_VOLUME_CORRUPTED The FvArea being reported does not appear to be a valid Firmware Volume
 * @return EFI_SUCCESS The Firmware Volume was reported to the system
 */
EFI_STATUS AmiPublishFvArea(IN AMI_ROM_AREA *FvArea)
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    EFI_FIRMWARE_VOLUME_HEADER *Fv = NULL;

    if(FvArea != NULL)
    {
        if (FvArea->Address == AMI_ROM_AREA_NOT_MEMORY_MAPPED) return EFI_NO_MAPPING;
        Status = EFI_VOLUME_CORRUPTED;
        Fv = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)ReadUnaligned64(&FvArea->Address);
        if(Fv->Signature == EFI_FVH_SIGNATURE)
        {
            Status = EFI_SUCCESS;
            BuildFvHob(ReadUnaligned64(&FvArea->Address), (UINT64)FvArea->Size);

            PeiServicesInstallFvInfoPpi(&(((EFI_FIRMWARE_VOLUME_HEADER*)ReadUnaligned64(&FvArea->Address))->FileSystemGuid),
                                        (VOID*)(UINTN)ReadUnaligned64(&FvArea->Address),
                                        FvArea->Size,
                                        NULL,
                                        NULL);
        }
    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
