//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
#ifndef __AMI_SDL_LIB__H__
#define __AMI_SDL_LIB__H__

#include <Protocol/AmiBoardInfo2.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
extern AMI_BOARD_INFO2_PROTOCOL *gAmiBoardInfo2Protocol;
extern AMI_SDL_PCI_DATA			*gSdlPciData;
extern AMI_SDL_SIO_DATA			*gSdlSioData;

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:  
//
// Description:
//
// Input:
//
// Output:
//  EFI_SUCCESS - Set successfully.
//  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//
// Notes:
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiSdlInitBoardInfo();


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindIndexRecord()
//
// Description: Find Record of given Index 
//
// Input: 	UINTN					Index		Record Index to find. 
//
// Output:	AMI_SDL_PCI_DEV_INFO	**Record 	Double Pointer at AMI_SDL_PCI_DEV_INFO  
//												updated with address of a structure 
//												to be found.
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindIndexRecord(UINTN	Index, AMI_SDL_PCI_DEV_INFO **Record);


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindRecordIndex()
//
// Description: Find Index of given Record 
//
// Input: AMI_SDL_PCI_DEV_INFO 	*Record 	Pointer at AMI_SDL_PCI_DEV_INFO structure 
//											which Index needed to be found.
//
// Output: UINTN				*Index		Pointer to Index that will be updated. 
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindRecordIndex(AMI_SDL_PCI_DEV_INFO *Record, UINTN *Index);


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlCheckIfParent()
//
// Description: Find Index of given Record 
//
// Input: UINTN 	DeviceIndex Index in DB of Child to check
//		  UINTN		ParentIndex Index in DB of Parent.									
//
// Output:  	TRUE if Device Index appears in the brunch produces by Parent 
//				FALSE otherwise
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
BOOLEAN AmiSdlCheckIfParent(UINTN DeviceIndex, UINTN ParentIndex);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlPciGetHostBridges(AMI_SDL_PCI_DEV_INFO ***HostBridgesBuffer, UINTN *HostBridgesCount);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindRbBySegBus()
//
// Description: Finds Root Bridge SDL record by PCI Segment and Bus Number
//
// Input: UINT32                Segment     PCI Segment Number
//
// Input: UINT8                 Bus         PCI Bus Number
//
// Output: AMI_SDL_PCI_DEV_INFO *Record     Pointer at AMI_SDL_PCI_DEV_INFO structure 
//                                          of a RootBridge record matching with search criteria.
//
// Output: UINTN                *Index      Pointer to Index that will be updated. 
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindRbBySegBus(IN UINT32 Segment, IN UINT8 Bus, OUT AMI_SDL_PCI_DEV_INFO **Record, OUT UINTN *Index);


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindRbByAslName()
//
// Description: Finds Root Bridge SDL record by PCI Segment and Bus Number
//
// Input: UINT8*                AslName     Root Bridge ASL Name (4 Char string)
//
// Output: AMI_SDL_PCI_DEV_INFO *Record     Pointer at AMI_SDL_PCI_DEV_INFO structure 
//                                          of a RootBridge record matching with search criteria.
//
// Output: UINTN                *Index      Pointer to Index that will be updated. 
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindRbByAslName(IN UINT8* AslName, OUT AMI_SDL_PCI_DEV_INFO **Record, OUT UINTN *Index);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlPciGetChildsOfParentIndex(AMI_SDL_PCI_DEV_INFO ***ChildrenSdlDataBuffer,
											UINTN 				*ChildCount,
											UINTN 				ParentIndex);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlReserveCspResources(EFI_HANDLE ImgHandle);


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlReadFfsSdlData(
	OUT UINT8 **SdlAddress, UINTN *SdlDataSize,
	IN EFI_GUID *FileGuid, IN EFI_GUID *SectionGuid OPTIONAL
);

#endif //__AMI_SDL_LIB__H__
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
