//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/SOURCE/Core/Library/PELoader.c 15    7/10/09 3:49p Felixp $
//
// $Revision: 15 $
//
// $Date: 7/10/09 3:55p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: PELoader.c
//
// Description:	
//  Loader functions for PE32/PE32+ files.
//  The functions are used by Core to implement standard image loading API.
//  Consumers outside Core should not use functions from this file.
//  The public API should be used instead.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiLib.h>

#include <IndustryStandard/PeImage.h>                                           // Build: EDKII includes
/************************************************************************/
/*                     PE Loader                                                                */
/************************************************************************/
//AptioV
//In Aptio4 PE_LOADER_SUPPORT_ALL is defined from the command line only when building AmiPeLib for x64 projects.
//For now let's always define it.
#define PE_LOADER_SUPPORT_ALL 1
#define PE32_SUPPORT (PE_LOADER_SUPPORT_ALL||!EFI64&&!EFIx64)
#define PE32_PLUS_SUPPORT (PE_LOADER_SUPPORT_ALL||EFI64||EFIx64)

#pragma pack(push)
#pragma pack(2)
//**********************************************************************//
//          Structure type definitions for standard PE headers          //
//**********************************************************************//
typedef struct _SECTION_HEADER { // Section Header
	UINT64 Name;
	UINT32 VirtualSize;
	UINT32 VirtualAddress;
	UINT32 SizeOfRawData;
	UINT32 PointerToRawData;
	UINT32 Dummy[4];
} SECTION_HEADER;

typedef struct _FIXUP_BLOCK{
	UINT32     VirtualAddress;
	UINT32     Size;
} FIXUP_BLOCK;

typedef struct _FIXUP{
	UINT16 Offset : 12, Type: 4;
}FIXUP;

typedef struct _OPTIONAL_HEADER_CODA { // Optional header coda
	UINT32 NumberOfRvaAndSizes;
	DIRECTORY_ENTRY Export, Import, Resource, Exception,
				 	Certificate, Reallocations, Debug;
} OPTIONAL_HEADER_CODA;
#pragma pack(pop)

        
/*                                                                    // Build: Commented as it is defined in MdePkg: IndustryStandard/PeImage.h
// Header format for TE images
typedef struct {
	UINT16 Signature;
	UINT16 Machine; // from the original file header
	UINT8 NumberOfSections; // from the original file header
	UINT8 Subsystem; // from original optional header
	UINT16 StrippedSize; // how many bytes we removed from the header
	UINT32 AddressOfEntryPoint; // offset to entry point from original optional header
	UINT32 BaseOfCode; // from original image -- required for ITP
	UINT64 ImageBase; // from original file header
	DIRECTORY_ENTRY DataDirectory[2]; // only base relocation and debug directory
} EFI_TE_IMAGE_HEADER;
*/

//#define EFI_TE_IMAGE_HEADER_SIGNATURE     0x5A56      // TE        // Build: Commented as it is defined in MdePkg: IndustryStandard/PeImage.h

// Data directory indexes in our TE image header
#define EFI_TE_IMAGE_DIRECTORY_ENTRY_BASERELOC      0
#define EFI_TE_IMAGE_DIRECTORY_ENTRY_DEBUG          1

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: IsPeImageValid
//
// Description:
//  BOOLEAN IsPeImageValid(IN UINT8 *pstart) reads the header signature of a
// portable executable (PE) image and returns TRUE if it is valid.
//
// Input:
//  IN UINT8 *pstart
// Pointer to the start of the PE image.
// 
// Output:
//  FALSE if image is invalid, otherwise TRUE.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  If x64_BUILD enabled, supports both PE32 and PE32+.  Otherwise only
// supports PE32.
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN IsPeImageValid(UINT8* pstart){
	return 
		   *(UINT16*)pstart == 0x5A4D && //DOS header signature is valid
		   *(UINT32*)(pstart+=*(UINT32*)(pstart+60)) == 0x00004550 && //PE signature is valid
#if PE_LOADER_SUPPORT_ALL
		(
#endif
#if PE32_SUPPORT
		   *(UINT16*)(pstart+4) == 0x14c && // IA32 machine
		   *(UINT16*)(pstart+24) == 0x10b   // this is PE32
#endif
#if PE_LOADER_SUPPORT_ALL
		||
#endif
#if PE32_PLUS_SUPPORT
		   (*(UINT16*)(pstart+4) == 0x200 || *(UINT16*)(pstart+4) == 0x8664)&& // x64 or IA64 machine
		   *(UINT16*)(pstart+24) == 0x20b   // this is PE32+
#endif
#if PE_LOADER_SUPPORT_ALL
		)
#endif
		;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: isTeImageValid
//
// Description:	
//  BOOLEAN isTeImageValid(IN UINT8 *pstart) reads the header signature of a
// terse executable (TE) image and returns true if it is valid.
//
// Input:
//  IN UINT8 *pstart
// Pointer to the start of the TE image.
//
// Output:
//  FALSE if image is invalid, otherwise TRUE.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  If x64_BUILD enabled, supports both PE32 and PE32+.  Otherwise only
// supports PE32.
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN IsTeImageValid(UINT8* pstart){
	EFI_TE_IMAGE_HEADER *pTe = (EFI_TE_IMAGE_HEADER*)pstart;
	return 
		pTe->Signature == EFI_TE_IMAGE_HEADER_SIGNATURE && //TE signature is valid
#if PE_LOADER_SUPPORT_ALL
		(
#endif
#if PE32_SUPPORT
		pTe->Machine == 0x14c // IA32 machine
#endif
#if PE_LOADER_SUPPORT_ALL
		||
#endif
#if PE32_PLUS_SUPPORT
		(pTe->Machine == 0x200 || pTe->Machine == 0x8664) // x64 or IA64 machine
#endif
#if PE_LOADER_SUPPORT_ALL
		)
#endif
		&& pTe->StrippedSize >= 2/*MZ signature*/+4/*PE signature*/+ 
							 sizeof(FILE_HEADER)+
							 //number of directory entries may vary
							 sizeof(OPTIONAL_HEADER)-sizeof(DIRECTORY_ENTRY)*7 &&
		pTe->AddressOfEntryPoint>=pTe->BaseOfCode;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: PEEntryPoint
//
// Description:
//  VOID* PEEntryPoint(IN VOID *p) returns the address of the entry point to
// a portable executable (PE) image.
//
// Input:
//  IN VOID *p
// Pointer to the start of the PE image whose entry point is desired.
//
// Output:
//  VOID* address to the entry point of the PE image, p.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* PEEntryPoint(VOID *p)
{
	UINT8 *q = (UINT8*)p;
	return q+((OPTIONAL_HEADER*)((FILE_HEADER*)(q+*(UINT32*)(q+60)+4)+1))->AddressOfEntryPoint;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: TEEntryPoint
//
// Description:	
//  VOID* TEEntryPoint(IN VOID *p) returns the address of the entry point to
// a terse executable (TE) image.
//
// Input:
//  IN VOID *p
// Pointer to the start of the TE image whose entry point is desired.
//
// Output:
//  VOID* address to the entry point of the TE image, p.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* TEEntryPoint(VOID *p)
{ 
	EFI_TE_IMAGE_HEADER* pTE = (EFI_TE_IMAGE_HEADER*)p;
	return (UINT8*)(pTE+1) + pTE->AddressOfEntryPoint - pTE->StrippedSize;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetDebugDirectory
//
// Description:	
//  DEBUG_DIRECTORY* GetDebugDirectory(IN VOID *ptr, OUT UINTN *TeAdjustment)
// locates and returns the location of DEBUG_DIRECTORY in a terse executable
// (TE) or portable executable (PE) image.
//
// Input:
//  IN VOID *ptr
// Pointer to the start of the TE or PE image.
//
//  OUT UINTN *TeAdjustment
// If ptr is a valid TE image and not NULL, returns the number of bytes
// stripped from the header.  If ptr is a PE image, returns 0.
//
// Output:
//  DEBUG_DIRECTORY* address to the image's DEBUG_DIRECTORY structure.
//
// Modified:
//
// Referrals:
//  IsTeImageValid
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
DEBUG_DIRECTORY* GetDebugDirectory(VOID *ptr, UINTN *TeAdjustment)
{
	FILE_HEADER *pFileHeader;
	OPTIONAL_HEADER* pOptHeader;
	UINT8 *p = (UINT8*)ptr;
	OPTIONAL_HEADER_CODA *pCoda;
    if (IsTeImageValid(ptr))
    {
	    EFI_TE_IMAGE_HEADER* pTE = (EFI_TE_IMAGE_HEADER*)p;
        UINT32 DebugDirectoryOffset = pTE->DataDirectory[EFI_TE_IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
        if (DebugDirectoryOffset == 0) return NULL;
        if (TeAdjustment) *TeAdjustment=sizeof(*pTE)-pTE->StrippedSize;
    	return (DEBUG_DIRECTORY*)((UINT8*)(pTE+1) + DebugDirectoryOffset - pTE->StrippedSize);
    }
	pFileHeader = (FILE_HEADER*)(p+*(UINT32*)(p+60)+4);
	pOptHeader= (OPTIONAL_HEADER*)(pFileHeader+1);
#if PE_LOADER_SUPPORT_ALL
	if (pOptHeader->Magic == 0x10b)
	{//PE32
		pCoda = (OPTIONAL_HEADER_CODA*)((UINT8*)pOptHeader+92);
	}
	else
	{//PE32+
		pCoda = (OPTIONAL_HEADER_CODA*)((UINT8*)pOptHeader+108);
	}
#else
	pCoda = (OPTIONAL_HEADER_CODA*)&pOptHeader->NumberOfRvaAndSizes;
#endif
	if (pCoda->NumberOfRvaAndSizes<(UINT32)(&pCoda->Debug - &pCoda->Export))
		return NULL;
	if (pCoda->Debug.VirtualAddress >= pOptHeader->SizeOfImage) return NULL;
    if (TeAdjustment) *TeAdjustment=0;
	return (DEBUG_DIRECTORY*)(p + pCoda->Debug.VirtualAddress);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetImageSize
//
// Description:	
//  UINT32 GetImageSize(IN VOID *p) retrieves the size of a terse executable
// (TE) image or portable executable (PE) image.
//
// Input:
//  IN VOID *p
// Pointer to the start of the TE or PE image.
//
// Output:
//  UINT32 size of the image.
//
// Modified:
//
// Referrals:
//  GetDebugDirectory
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
UINT32 GetImageSize(VOID *p)
{
	UINT8 *q = (UINT8*)p;
	UINT32 Size = ((OPTIONAL_HEADER*)((FILE_HEADER*)(q+*(UINT32*)(q+60)+4)+1))->SizeOfImage;
#ifdef EFI_DEBUG
{
	//ITP Support
	DEBUG_DIRECTORY *pDebug = GetDebugDirectory(q,NULL);
	if (pDebug /*&& !pDebug->AddressOfRawData*/ && pDebug->PointerToRawData)
		Size+=pDebug->SizeOfData;
}
#endif
	return Size;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetPeOptionalHeader
//
// Description:	
//  OPTIONAL_HEADER* GetPeOptionalHeader(IN VOID *p) returns the address of a
// portable executable (PE) image's optional header.
//
// Input:
//  IN VOID *p
// Pointer to the start of the PE image whose optional header is desired.
//
// Output:
//  OPTIONAL_HEADER* address to the PE image's optional header.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
OPTIONAL_HEADER* GetPeOptionalHeader(VOID *p)
{
	UINT8 *q = (UINT8*)p;
	return (OPTIONAL_HEADER*)((FILE_HEADER*)(q+*(UINT32*)(q+60)+4)+1);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetName
//
// Description:
//  BOOLEAN GetName(IN UINT8 *p, OUT CHAR8 *sName) retrieves the name of
// portable executable (PE) or terse executable (TE) image, if it is
// available.
//
// Input:
//  IN UINT8 *p
// Pointer to the start of the PE or TE image whose name is desired.
//
//  OUT CHAR8 *sName
// Returns string containing image's name, if it is available.  User is
// responsible for allocating the necessary memory resources.
//
// Output:
//  TRUE, if name is located; otherwise FALSE.
//
// Modified:
//
// Referrals:
//  GetDebugDirectory
//  Strlen
//  Strcmp
//  MemCpy
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN GetName(UINT8 *p, CHAR8 *sName)
{
	UINTN TeAdjustment;
	DEBUG_DIRECTORY *pDebug=GetDebugDirectory(p,&TeAdjustment);
	UINT32 sig;
	UINTN length;
	CHAR8 *s, *q, *CvEntry;
	if (!pDebug||pDebug->Type!=2) return FALSE;
    CvEntry = (CHAR8*)(p+pDebug->AddressOfRawData+TeAdjustment);
	sig = *(UINT32*)CvEntry;
	if (sig==('N'+('B'<<8)+(('1'+('0'<<8))<<16))) //NB10
		s = CvEntry + 0x10;
	else if (sig==('R'+('S'<<8)+(('D'+('S'<<8))<<16))) //RSDS
		s = CvEntry + 0x18;
	else return FALSE;
	length = Strlen(s);
	if (length<5 || Strcmp(&s[length-4],".pdb")) return FALSE;
	for(q = &s[length-5]; q>=s && *q!='\\'; q--);
	q++;
	length = s+length-4-q;
	MemCpy(sName,q,length);
	sName[length]=0;
	return TRUE;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ReallocatePeImage
//
// Description:	
//  BOOLEAN ReallocatePeImage(IN UINT8 *pcurrent, IN UINT8 *pnew,
// IN UINT8 *prealloc) loads an already loaded portable executable (PE) image
// at a new memory location with the proper fixup.
// 
// Input:
//  IN UINT8 *pcurrent
// Pointer to the current location of the PE image in memory.
//
//  IN UINT8 *pnew
// Pointer to the new location of the PE image in memory.  May be set to the
// same value as pcurrent to adjust fixup only.
//
//  IN UINT8 *prealloc
// Pointer to an array of UINTN fixup reallocations.  May be set to NULL.
//
// Output:
//  FALSE if image contains an unknown fixup type; otherwise TRUE.
//
// Modified:
//
// Referrals:
//  GetPeOptionalHeader
// 
// Notes:
//  Can only handle the IA32 or IA64 fix up types.
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN ReallocatePeImage(UINT8 *pcurrent, UINT8 *pnew, UINT8 *prealloc)
{
	// Fix up
	// There is a lot of fix up types but we can handle only IA32 or IA64 specific fix up types.
	OPTIONAL_HEADER* pOptHeader = GetPeOptionalHeader(pcurrent);
	OPTIONAL_HEADER_CODA *pCoda;
	UINT64 ImageBase;
	UINT64 Delta;
	FIXUP_BLOCK *pBegin, *pEnd;
    UINTN *pReallocations = (UINTN*)prealloc;
#if PE_LOADER_SUPPORT_ALL
	if (pOptHeader->Magic == 0x10b)
	{//PE32
		pCoda = (OPTIONAL_HEADER_CODA*)((UINT8*)pOptHeader+92);
		ImageBase = *(UINT32*)((UINT8*)pOptHeader+28);
		*(UINT32*)((UINT8*)pOptHeader+28)=(UINT32)pnew;
	}
	else
	{//PE32+
		pCoda = (OPTIONAL_HEADER_CODA*)((UINT8*)pOptHeader+108);
		ImageBase = *(UINT64*)((UINT8*)pOptHeader+24);
		*(UINT64*)((UINT8*)pOptHeader+24)=(UINT64)(UINTN)pnew;
	}
#else
	pCoda = (OPTIONAL_HEADER_CODA*)&pOptHeader->NumberOfRvaAndSizes;
	ImageBase = pOptHeader->ImageBase;
	pOptHeader->ImageBase=(UINTN)pnew;
#endif
	Delta = (UINT64)(UINTN)pnew-ImageBase;
	if (!Delta) return TRUE;
	pBegin = (FIXUP_BLOCK*)(pcurrent+pCoda->Reallocations.VirtualAddress);
	pEnd = (FIXUP_BLOCK*)((UINT8*)pBegin + pCoda->Reallocations.Size);
	while(pBegin<pEnd){
		FIXUP *pFixupInfo,*pEndOfBlock;
		UINT8 *pFixupPage;
		if (!pBegin->Size) return FALSE;
		pFixupInfo = (FIXUP*)(pBegin+1);
		pEndOfBlock = (FIXUP*)((UINT8*)pBegin+pBegin->Size);
		pFixupPage = pcurrent + pBegin->VirtualAddress;
		for(;pFixupInfo<pEndOfBlock;pFixupInfo++){
			UINTN *pFixup = (UINTN*)(pFixupPage + pFixupInfo->Offset);
			switch (pFixupInfo->Type){
#if PE32_SUPPORT
				case 3://IMAGE_REL_BASED_HIGHLOW
					if (!pReallocations || *pReallocations++ == *pFixup) *pFixup += (UINTN)Delta;
					break;
#endif
#if PE32_PLUS_SUPPORT
				case 10://IMAGE_REL_BASED_DIR64:
				{
#if EFI64
					if ((UINT64)pFixup&0x7){
					    UINT8 *p;
					    UINT32 i;
						UINT64 R;
						p = (UINT8*)&R;
						for(i=0;i<8;i++) p[i]=((UINT8*)pFixup)[i];
						if (!Pred || Pred((UINTN)R))
						{
							R+=Delta;
							for(i=0;i<8;i++) ((UINT8*)pFixup)[i]=p[i];
						}
					}else 
#endif
                    if (!pReallocations || *pReallocations++ == *pFixup) *(UINT64*)pFixup += Delta;
					break;
				}
#endif
				case 0://IMAGE_REL_BASED_ABSOLUTE:
					break;
				default://Error: Unknown fixup type
					return FALSE;
			}//switch
		}//for
		pBegin=(FIXUP_BLOCK*)pEndOfBlock;
	}//while
	return TRUE;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LoadPeImageEx
//
// Description:	
//  VOID* LoadPeImageEx(IN UINT8 *pold, IN UINT8 *pnew, IN UINT8 *prealloc)
// loads a portal executable into memory from a file.
//
// Input:
//  IN UINT8 *pold
// Pointer to the start of the file in memory.
//
//  IN UINT8 *pnew
// Pointer to starting location in memory for the loaded image.
//
//  IN UINT8 *prealloc
// Pointer to an array of UINTN fixup reallocations.  May be set to NULL.
//
// Output:
//  If successful, VOID* address of the entry point for the newly loaded
// image; otherwise, NULL if pold and or pnew are not 16-byte aligned, or
// pold does not point to valid PE image. 
//
// Modified:
//
// Referrals:
//  IsPeImageValid
//  MemCpy
//  MemSet
//  ReallocatePeImage
//  GetDebugDirectory
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* LoadPeImageEx(UINT8* pold, UINT8* pnew, UINT8 *prealloc){
	FILE_HEADER *pFileHeader;
	OPTIONAL_HEADER* pOptHeader;
	SECTION_HEADER *pSection;
	UINT8 *p;
	UINT32 i;
#ifdef EFI64
	// Destination and source buffers should starts at the 16-byte boundary
	if ((int)pnew&0xf || (int)pold&0xf) return NULL;
#endif
	if (!IsPeImageValid(pold)) return NULL;
	pFileHeader = (FILE_HEADER*)(pold+*(UINT32*)(pold+60)+4);
	pOptHeader= (OPTIONAL_HEADER*)(pFileHeader+1);
	// Sections should have at least sizeof(INTN)-byte alignment
	if (pOptHeader->SectionAlignment<sizeof(INTN)) return NULL;
	pSection=(SECTION_HEADER*)((UINT8*)pOptHeader+pFileHeader->OptionalHeaderSize);
	// copy headers
	MemCpy(pnew,pold,pOptHeader->SizeOfHeaders);
	//copy sections
	// in the file image pSection->PointerToRawData contains offset of the section data
	// in the memory image the section data should be placed at pSection->VirtualAddress RVA.
	p=pnew+pOptHeader->SizeOfHeaders;
	for(i=0;i<pFileHeader->NumberOfSections;i++,pSection++){
		// fill with zeros am empty space between sections
		MemSet(p,pnew+pSection->VirtualAddress-p,0);
		p=pnew+pSection->VirtualAddress;
		if (pSection->PointerToRawData){
			UINTN SectionSize = (pSection->VirtualSize<pSection->SizeOfRawData)?pSection->VirtualSize:pSection->SizeOfRawData;
			MemCpy(
				pnew+pSection->VirtualAddress,
				pold+pSection->PointerToRawData,
				SectionSize				
			);
			p+=SectionSize;
		}
	}
	//align the last section
	MemSet(p,pnew+pOptHeader->SizeOfImage-p,0);
	// Fix up
	if (!ReallocatePeImage(pnew, pnew, prealloc)) return NULL;
#ifdef EFI_DEBUG
{
	//ITP Support
	DEBUG_DIRECTORY *pDebug = GetDebugDirectory(pnew,NULL);
	if (pDebug /*&& !pDebug->AddressOfRawData*/ && pDebug->PointerToRawData)
	{
		MemCpy(pnew+pOptHeader->SizeOfImage,pold+pDebug->PointerToRawData,pDebug->SizeOfData);
		pDebug->AddressOfRawData = pOptHeader->SizeOfImage;
	}
}
#endif
	return pnew+pOptHeader->AddressOfEntryPoint;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LoadPeImage
//
// Description:
//  VOID* LoadPeImage(IN UINT8 *pold, IN UINT8 *pnew) is a wrapper function
// for LoadPeImageEx which passes NULL for prealloc.
//
// Input:
//  IN UINT8 *pold
// Pointer to the start of the file in memory.
//
//  IN UINT8 *pnew
// Pointer to starting location in memory for the loaded image.
//
// Output:
//  If successful, VOID* address of the entry point for the newly loaded
// image; otherwise, NULL if pold and or pnew are not 16-byte aligned, or
// pold does not point to valid PE image. 
//
// Modified:
//
// Referrals:
//  LoadPeImageEx
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* LoadPeImage(UINT8* pold, UINT8* pnew){
	return LoadPeImageEx(pold,pnew,NULL);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FindPeSection
//
// Description:	
//  VOID* FindPeSection(IN UINT8 *pstart, IN UINT64 name, OUT UINTN *pSize)
// locates a section with the user provided name inside a portable executable
// (PE) image.
//
// Input:
//  IN UINT8 *pstart
// Pointer to the start of the image to be searched.
//
//  IN UINT64 name
// UINT64 name associated with the section to be located.
//
//  OUT UINTN *pSize
// Contains the size of the located section.
//
// Output:
//  If located, VOID* pointer to the virtual address of the located section;
// otherwise, NULL.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* FindPeSection(UINT8* pstart, UINT64 name, UINTN* pSize){
	FILE_HEADER *pFileHeader = (FILE_HEADER*)(pstart+*(UINT32*)(pstart+60)+4);
	SECTION_HEADER *pSection = (SECTION_HEADER*)((UINT8*)(pFileHeader+1)+pFileHeader->OptionalHeaderSize);
	UINT32 i;
	for(i=0;i<pFileHeader->NumberOfSections;i++,pSection++){
		if (pSection->Name!=name) continue;
		if (pSize) *pSize = pSection->VirtualSize;
		return pstart+pSection->VirtualAddress;
	}
	return NULL;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
