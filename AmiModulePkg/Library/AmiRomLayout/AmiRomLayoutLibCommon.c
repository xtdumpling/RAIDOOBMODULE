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
#pragma pack(pop)

// These variables are initialized in the constructor
AMI_ROM_AREA *gAmiRomLayout = NULL;
UINT32 gAmiRomLayoutVersion = 0;
UINT32 gAmiRomAreaSize = 0;
UINT32 gAmiRomLayoutSize = 0;
AMI_ROM_AREA *gAmiRomLayoutEnd = NULL;

EFI_STATUS AmiGetRomLayoutProperties(
	OUT UINT32 *Version OPTIONAL, OUT UINT32 *RomAreaSize OPTIONAL, OUT UINT32 *RomLayoutSize OPTIONAL
){
	if (gAmiRomLayout==NULL) return EFI_NOT_FOUND;
	if (Version != NULL) *Version = gAmiRomLayoutVersion;
	if (RomAreaSize != NULL) *RomAreaSize = gAmiRomAreaSize;
	if (RomLayoutSize != NULL) *RomLayoutSize = gAmiRomLayoutSize;
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
    return gAmiRomLayout;
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
    AMI_ROM_AREA *Next = NULL;
    if(Start != NULL)
    {
        // Make sure the pointer is within the area we are reporting for Rom Layout
        if( (gAmiRomLayout <= Start) && (gAmiRomLayoutEnd >= Start) )
        {
            if((AMI_ROM_AREA*)((UINT8*)Start + gAmiRomAreaSize) < gAmiRomLayoutEnd)
                Next = (AMI_ROM_AREA*)((UINT8*)Start + gAmiRomAreaSize);
        }
    }
    return Next;
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
    ASSERT( (UINTN)SearchPointer == ((UINTN)SearchPointer & (~0x07)) );

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
// The method of reporting a FV Area is specific to the driver type. This function will be defined in the
//  accompanying file
//EFI_STATUS AmiPublishFvArea(IN AMI_ROM_AREA *FvArea) { return EFI_INVALID_PARAMETER;}


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
