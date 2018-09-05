//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************l
/** @file
  @brief Defines AmiRomLayout library class.

  ROM Layout Processing Functions.
**/
#ifndef __AMI_ROM_LAYOUT_LIB__H__
#define __AMI_ROM_LAYOUT_LIB__H__
#include <AmiRomLayout.h>

/**
  Return ROM layout properties

  @param[out] Version Version of the ROM Layout data structure
  @param[out] RomAreaSize Size of a single ROM area descriptor
  @param[out] RomLayoutSize Total size of the ROM layout array

  @return  EFI_STATUS
*/
EFI_STATUS AmiGetRomLayoutProperties(
	OUT UINT32 *Version OPTIONAL, OUT UINT32 *RomAreaSize OPTIONAL, OUT UINT32 *RomLayoutSize OPTIONAL
);

/**
  Return a pointer to the first entry of AMI_ROM_AREA structure.

  @retval AMI_ROM_AREA*       Pointer to the first entry of the AMI_ROM_AREA structure
  @retval NULL                The AMI_ROM_AREA structure could not be found.
*/
AMI_ROM_AREA* AmiGetFirstRomArea(VOID);

/**
  Using the passed parameter as the starting entry, return a pointer to the next AMI_ROM_AREA entry.

  @param[in] Start  Pointer to the entry previously returned by the AmiGetFirstRomArea or AmiGetNextRomArea function. 

  @retval  AMI_ROM_AREA *     Pointer to the next entry after the passed starting entry
  @retval  NULL               There were no more entries after the starting entry, or the starting
                                entry was not a valid pointer.
 */
AMI_ROM_AREA* AmiGetNextRomArea(AMI_ROM_AREA *Start);

/**
  Return a pointer to the single ROM_AREA structure that covers the address passed into this
  function. If the address does not correspond to an address in the ROM_AREA structures, NULL will
  be returned

  @param[in] Address   The address of the region in question being queried
  
  @retval ROM_AREA **    The pointer to the ROM_AREA structure that describes the region
                           which encompasses the address

  @retval NULL           The region was not found.
*/
AMI_ROM_AREA* AmiGetRomAreaByAddress(IN EFI_PHYSICAL_ADDRESS Address);

/**
  Return a pointer to the first ROM_AREA structure with the AreaGuid.

  @param[in] AreaGuid    The GUID of the region in question being queried
  
  @retval ROM_AREA **    The pointer to the ROM_AREA structure that describes the region which encompasses the address
  @retval NULL           The region was not found.
*/
AMI_ROM_AREA* AmiGetFirstRomAreaByGuid(IN EFI_GUID *AreaGuid);

/**
  Return a pointer to the next ROM_AREA structure with the AreaGuid.
  There may be more than one area with the same GUID.
  Set PreviousRomArea to NULL to get the first area.

  @param[in] AreaGuid               The GUID of the region in question being queried
  @param[in] PreviousRomArea        The pointer to the ROM_AREA structure previously returned by the 
                                      AmiGetFirstRomAreaByGuid or AmiGetNextRomAreaByGuid function. 

  @retval ROM_AREA **    The pointer to the ROM_AREA structure with the specified GUID
  @retval NULL           The region was not found.
*/
AMI_ROM_AREA* AmiGetNextRomAreaByGuid(IN EFI_GUID *AreaGuid, IN AMI_ROM_AREA *PreviousRomArea);

/**
  Searches ROM image at ImageAddress for a ROM Layout table.

  @param[in]  ImageAddress image starting address
  @param[in]  ImageSize Size of the image at ImageAddress
  @param[out] RomLayout Pointer to a ROM Layout Table extracted from the image at ImageAddress
  @param[out] Version Version of the ROM Layout data structure
  @param[out] RomAreaSize Size of a single ROM area descriptor
  @param[out] RomLayoutSize Total size of the ROM layout array

  @retval  EFI_SUCCESS  ROM Layout Data structure has been found
  @retval  EFI_NOT_FOUND ROM Layout Data structure has not been found
*/
EFI_STATUS AmiGetImageRomLayout(
	IN VOID *ImageAddress, IN UINTN ImageSize,
	OUT AMI_ROM_AREA **RomLayout, OUT UINT32 *Version OPTIONAL, 
	OUT UINT32 *RomAreaSize OPTIONAL, OUT UINT32 *RomLayoutSize OPTIONAL
);

/**
  Publish FV

  @param[in] FvName    The GUID of the region to publish
  
  @return  EFI_STATUS
*/
EFI_STATUS AmiPublishFv(IN EFI_GUID *FvName);

/**
  Publish FV

  @param[in] FvArea    ROM_AREA descriptor of the region to publish
  
  @return  EFI_STATUS
*/
EFI_STATUS AmiPublishFvArea(IN AMI_ROM_AREA *FvArea);

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
