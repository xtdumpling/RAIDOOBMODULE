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
// $Header: /Alaska/SOURCE/Core/Library/PeiLib.c 26    5/27/11 5:59p Felixp $
//
// $Revision: 26 $
//
// $Date: 5/27/11 5:59p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: PeiLib.c
//
// Description:	
//  Contains an assortment of localized PEI library functions.
//
//<AMI_FHDR_END>
//*************************************************************************

#include <Token.h>
#include <AmiPeiLib.h>
#include <StatusCodes.h>
#include <AmiHobs.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/LoadFile.h>
#include <Ppi/AmiDebugService.h>
#include <Library/DebugPrintErrorLevelLib.h>

// ===========================
// In Aptio4, these are externals defined in the AmiPpiLib.
// In AptioV, there is no AmiPpiLib.
// We could've listed these in AmiPeiLib.inf, but may break the
// build of some of the Aptio4 modules due to multiple defined symbols.
static EFI_GUID gLocalEfiPeiReadOnlyVariable2PpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;
static EFI_GUID gLocalEfiPeiLoadFilePpiGuid = EFI_PEI_LOAD_FILE_PPI_GUID;
// ===========================

#define AMI_PLATFORM_RESOURCE_GUID \
    { 0xd769ac33, 0x7692, 0x4d8b, 0xb5, 0x5e, 0xf6, 0xde, 0xe7, 0x62, 0x54, 0xaa }

//===================================================================================
/**
  The function returns AmiDebugService instance
  @retval AmiDebugService instance or NULL if service is not available
 */
AMI_DEBUG_SERVICE_PPI *LocalGetAmiDebugServicesPpiInstance (CONST EFI_PEI_SERVICES **ppPS)
{
    AMI_DEBUG_SERVICE_PPI *AmiDebugServicePpi;
    EFI_STATUS Status;
    EFI_PEI_PPI_DESCRIPTOR *Dummy;
    
    Status = (*ppPS)->LocatePpi (ppPS, &gAmiDebugServicePpiGuid, 0, &Dummy, (VOID**)&AmiDebugServicePpi);
    if (EFI_ERROR(Status)) return NULL;
   
    return AmiDebugServicePpi;
}


/**
    Prints a debug message using the ReportStatusCode from PEI Services if the specified error level is enabled.

    @param Level The error level of the debug message.
    @param ppPS Double pointer to PEI Services Table.
    @param *sFormat Format string for the debug message to print.
    @param ... Additional parameters utilized by the format string.

    @return   VOID.

 @Notes:
  Use the PEI_TRACE macro for debug messages in PEI!  This allows the
   DEBUG_MODE token to control the inclusion of debug messages.  See 
   Sprintf function for format string syntax.
  Uses PrepareStatusCodeString, va_start, va_end, PEI_TRACE, Sprintf
*/
VOID PeiTrace(UINTN Level, CONST EFI_PEI_SERVICES **ppPS, CHAR8 *sFormat,...)
{
    extern const UINT32 TraceLevelMask;
	va_list	ArgList;
    AMI_DEBUG_SERVICE_PPI *AmiDebugServicePpi = LocalGetAmiDebugServicesPpiInstance(ppPS);

    if (AmiDebugServicePpi == NULL || (Level & GetDebugPrintErrorLevel ()) == 0) return;

    ProcessFormatStringtoEdk2(sFormat);
    
    ArgList = va_start(ArgList,sFormat);
    AmiDebugServicePpi->Debug(Level, sFormat, ArgList);
	va_end(ArgList);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CreateHobMemoryAllocationModule
//
// Description:	
//  EFI_STATUS CreateHobMemoryAllocationModule(
// IN CONST EFI_PEI_SERVICES **PeiServices,
// IN EFI_PHYSICAL_ADDRESS MemoryBaseAddress, IN UINT64 MemoryLength,
// IN EFI_MEMORY_TYPE MemoryType, IN EFI_GUID *ModuleName, 
// IN EFI_PHYSICAL_ADDRESS EntryPoint) adds a memory allocation HOB with a 
// memory allocation module GUID using CreateHob from PEI Services.
//
// Input:
//  IN CONST EFI_PEI_SERVICES **PeiServices
// Double pointer to PEI Services Table.
//
//  IN EFI_PHYSICAL_ADDRESS MemoryBaseAddress 
// The physical address of the module.
//
//  IN UINT64 MemoryLength
// The length of the module in bytes.
//
//  IN EFI_MEMORY_TYPE MemoryType
// Module memory type.
//
//  IN EFI_GUID *ModuleName 
// The GUID File Name of the module.
//
//  IN EFI_PHYSICAL_ADDRESS EntryPoint
// The 64-bit physical address of the module's entry point.
//
// Output:
//  EFI_INVALID_PARAMETER, if Hob = NULL.
//  EFI_OUT_OF_RESOURCES,  if there is not enough space to create HOB.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS CreateHobMemoryAllocationModule(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_PHYSICAL_ADDRESS MemoryBaseAddress,
    IN UINT64 MemoryLength, IN EFI_MEMORY_TYPE MemoryType,
    IN EFI_GUID *ModuleName,
    IN EFI_PHYSICAL_ADDRESS EntryPoint
)
{
	static EFI_GUID gHobMemAllocModGuid=EFI_HOB_MEMORY_ALLOC_MODULE_GUID;
    EFI_STATUS Status;
    EFI_HOB_MEMORY_ALLOCATION_MODULE *MemAllocModHob;

    Status = (*PeiServices)->CreateHob(
        PeiServices,
        EFI_HOB_TYPE_MEMORY_ALLOCATION,
        sizeof(EFI_HOB_MEMORY_ALLOCATION_MODULE),
        (VOID **)&MemAllocModHob);
    if (Status != EFI_SUCCESS) return Status;
    MemAllocModHob->MemoryAllocationHeader.Name=gHobMemAllocModGuid;
    MemAllocModHob->MemoryAllocationHeader.MemoryBaseAddress=MemoryBaseAddress;
    MemAllocModHob->MemoryAllocationHeader.MemoryLength=MemoryLength;
    MemAllocModHob->MemoryAllocationHeader.MemoryType=MemoryType;
    *(UINT32*)&MemAllocModHob->MemoryAllocationHeader.Reserved=0;
    MemAllocModHob->ModuleName=*ModuleName;
    MemAllocModHob->EntryPoint=EntryPoint;
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CreateHobResourceDescriptor
//
// Description:	
//  EFI_STATUS CreateHobResourceDescriptor(IN CONST EFI_PEI_SERVICES **PeiServices,
// IN EFI_GUID *Owner, IN EFI_RESOURCE_TYPE ResourceType, 
// IN EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute, 
// IN EFI_PHYSICAL_ADDRESS PhysicalStart, IN UINT64 ResourceLength) builds a
// resource descriptor HOB that describes a chunk of system memory using 
// CreateHob from PEI Services.
//
// Input:
//  IN CONST EFI_PEI_SERVICES **PeiServices
// Double pointer to the PEI Services Table.
//
//  IN EFI_GUID *Owner
// GUID of the owner of this resource.  Make this NULL if there is none.
//
//  IN EFI_RESOURCE_TYPE ResourceType
// The type of resource described by this HOB.
//
//  IN EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute
// The resource attributes of the memory described by this HOB.
//
//  IN EFI_PHYSICAL_ADDRESS PhysicalStart
// The 64-bit physical address of memory described by this HOB.
//
//  IN UINT64 ResourceLength
// The length of the memory described by this HOB in bytes.
//
// Output:
//  EFI_INVALID_PARAMETER, if HOB = NULL.
//  EFI_OUT_OF_RESOURCES,  if there is not enough space to create HOB.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS CreateHobResourceDescriptor
(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_GUID *Owner, IN EFI_RESOURCE_TYPE ResourceType,
    IN EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute,
    IN EFI_PHYSICAL_ADDRESS PhysicalStart,
    IN UINT64 ResourceLength
)
{
    EFI_STATUS Status;
    EFI_HOB_RESOURCE_DESCRIPTOR *ResHob;
    static EFI_GUID AmiPlatformResourceGuid = AMI_PLATFORM_RESOURCE_GUID;

    Status = (*PeiServices)->CreateHob(
        PeiServices,
        EFI_HOB_TYPE_RESOURCE_DESCRIPTOR,
        sizeof(EFI_HOB_RESOURCE_DESCRIPTOR),
        (VOID **)&ResHob);
    if (Status != EFI_SUCCESS) return Status;

    ResHob->Owner = (Owner == NULL) ? AmiPlatformResourceGuid : *Owner;
    ResHob->ResourceType=ResourceType;
    ResHob->ResourceAttribute=ResourceAttribute;
    ResHob->PhysicalStart=PhysicalStart;
    ResHob->ResourceLength=ResourceLength;
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CreateLoadHob
//
// Description:	
//  EFI_STATUS CreateLoadHob(IN CONST EFI_PEI_SERVICES **ppPS,
// IN EFI_FFS_FILE_HEADER *pFfsHeader, EFI_PEIM_ENTRY_POINT EntryPoint, 
// EFI_PEIM_ENTRY_POINT InMemEntryPoint) builds a EFI_HOB_TYPE_GUID_EXTENSION
// HOB with a PEIM load HOB.
//
// Input:
//	IN CONST EFI_PEI_SERVICES **ppPS
// Double pointer to the PEI Services Table.
//
//  IN EFI_FFS_FILE_HEADER *pFfsHeader
// Pointer to the desired FFS header.
//
//  EFI_PEIM_ENTRY_POINT EntryPoint
// Pointer to the FFS entry point.
//
//  EFI_PEIM_ENTRY_POINT InMemEntryPoint
// Pointer to the FFS entry point in memory.
//
// Output:
//  EFI_INVALID_PARAMETER, if HOB = NULL.
//  EFI_OUT_OF_RESOURCES,  if not enough space to create HOB.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS CreateLoadHob(
	IN CONST EFI_PEI_SERVICES **ppPS, IN EFI_FFS_FILE_HEADER *pFfsHeader,
	EFI_PEIM_ENTRY_POINT EntryPoint, EFI_PEIM_ENTRY_POINT InMemEntryPoint
)
{
	EFI_STATUS	Status;
	PEIM_LOAD_HOB *pHob;
	static EFI_GUID guidAmiPeimLoadHob = AMI_PEIM_LOAD_HOB_GUID;
	
	Status=(*ppPS)->CreateHob(ppPS, EFI_HOB_TYPE_GUID_EXTENSION, sizeof(PEIM_LOAD_HOB), (VOID **)&pHob);
	if (!EFI_ERROR(Status))
	{
		pHob->Header.Name = guidAmiPeimLoadHob;
		pHob->pFfsHeader = pFfsHeader;
		pHob->EntryPoint = EntryPoint;
		pHob->InMemEntryPoint= InMemEntryPoint;
	}
	return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: PeiGetVariable
//
// Description:	
//  EFI_STATUS PeiGetNextVariableName(IN CONST EFI_PEI_SERVICES **PeiServices,
// IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VariableGuid,
// OUT UINT32 *Attributes, IN OUT UINTN *DataSize, OUT VOID *Data) reads an
// EFI variable; it serves as a wrapper for the Read Only Variable PPI 
// GetVariable routine. The functions first locates the Read Only Variable
// PPI instance, then makes a call to the GetVariable routine and returns 
// the EFI_STATUS of the GetVariable call. 
//
// Input:
//  IN CONST EFI_PEI_SERVICES **PeiServices
// Double pointer to the PEI Services Table.
//
//  IN CONST CHAR16 *VariableName
// A pointer to a null-terminated string that is the variable's name.
//
//  IN CONST EFI_GUID *VariableGuid
// A pointer to an EFI_GUID that is the variable's GUID. The combination of
// VariableGuid and VariableName must be unique.
//
//  OUT UINT32 *Attributes
// If non-NULL, on return, points to the variable's attributes.
//
//  IN OUT UINTN *DataSize
// On entry, points to the size in bytes of the Data buffer. On return,
// points to the size of the data returned in Data.
//
//  OUT VOID *Data
// Points to the buffer which will hold the returned variable value.  The
// user is responsible for allocating this memory!
//
// Output:
//  EFI_BUFFER_TOO_SMALL,  if DataSize is too small to hold the contents of
//                         the variable.  DataSize will be set to show the
//                         minimum required size.
//  EFI_INVALID_PARAMETER, if VariableName, VariableGuid, DataSize, or Data
//                         are equal to NULL.
//  EFI_DEVICE_ERROR,      if variable could not be retrieved because of a
//                         device error.
//  EFI_NOT_FOUND,         if the variable or Read Only Variable PPI could
//                         not be found.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  See Platform Initialization Specification for details regarding the
// Read Only Variable PPI and GetVariable.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS PeiGetVariable(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VariableGuid,
    OUT UINT32 *Attributes, 
    IN OUT UINTN *DataSize, OUT VOID *Data
)
{
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
    EFI_STATUS Status = (*PeiServices)->LocatePpi(
        PeiServices, &gLocalEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi    // Build: gPeiReadOnlyVariable2PpiGuid is changed to
    );                                                                          // gLocalEfiPeiReadOnlyVariable2PpiGuid
    if (EFI_ERROR(Status)) return Status;
    return VariablePpi->GetVariable(
        VariablePpi,VariableName, VariableGuid, Attributes, DataSize, Data
    );
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: PeiGetNextVariableName
//
// Description:	
//  EFI_STATUS PeiGetNextVariableName(IN CONST EFI_PEI_SERVICES **PeiServices, 
// IN OUT UINTN *VariableNameSize, IN OUT CHAR16 *VariableName, 
// IN OUT EFI_GUID *VariableGuid) performs enumeration of the EFI variables. 
// This function serves as a wrapper for the Read Only Variable PPI
// GetNextVariableName routine. It locates the Read Only Variable PPI
// instance, then makes a call to the GetNextVariableName routine and returns
// its EFI_STATUS.
//
// Input:
//  IN CONST EFI_PEI_SERVICES **PeiServices
// Double pointer to the PEI Services Table.
//
//  IN OUT UINTN *VariableNameSize
// On entry, points to the size of the buffer pointed to by VariableName.
// If function returns EFI_BUFFER_TOO_SMALL, VariableNameSize is updated to
// reflect the buffer size required for the next variable's name.
// 
//  IN OUT CHAR16 *VariableName
// On entry, a pointer to a null-terminated string that is the variable's
// name. On return, points to the next variable's null-terminated name
// string.
//
//  IN OUT EFI_GUID *VariableGuid
// On entry, a pointer to an UEFI _GUID that is the variable's GUID. On
// return, a pointer to the next variable's GUID.
//
// Output:
//  EFI_NOT_FOUND,         if the variable or Read Only Variable PPI could
//                         not be found.
//  EFI_BUFFER_TOO_SMALL,  if VariableNameSize is too small for the resulting
//                         data.  VariableNameSize is updated with the size
//                         required for the specified variable.
//  EFI_INVALID_PARAMETER, if VariableName, VariableNameSize, or VariableGuid
//                         are NULL.
//  EFI_DEVICE_ERROR,      if the variable could not be retrieved because of
//                         a device error.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:
//  See the Platform Initialization Specification for details regarding
// the Read Only Variable PPI and NextVariableName.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS PeiGetNextVariableName(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN OUT UINTN *VariableNameSize, IN OUT CHAR16 *VariableName,
    IN OUT EFI_GUID *VariableGuid
)
{
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
    EFI_STATUS Status = (*PeiServices)->LocatePpi(
        PeiServices, &gLocalEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi    // Build: Same as above
    );
    if (EFI_ERROR(Status)) return Status;
    return VariablePpi->NextVariableName(
        VariablePpi,VariableNameSize, VariableName, VariableGuid
    );
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FindFfsFileByName
//
// Description:	
//  VOID* FindFfsFileByName(IN CONST EFI_PEI_SERVICES **PeiServices, 
// IN EFI_FV_FILETYPE FileType, IN EFI_GUID *FileName) scans all available 
// firmware volumes for a FFS file of type FileType with GUID equal to
// FileName and returns the address of the first byte of the FFS file data.
// If more than one file matches the criteria, the function returns the
// address of the first file found.
//
// Input:
//  IN CONST EFI_PEI_SERVICES **PeiServices
// Double pointer to the PEI Services Table.
//
//  IN EFI_FV_FILETYPE FileType
// Type of FFS file to find.
//
//  IN EFI_GUID *FileName
// GUID of FFS file to find.
//
// Output:
//  VOID* pointer to the first byte of the FFS file data. Returns NULL if no
// FFS file found.
//   
// Modified:
//
// Referrals:
// 
// Notes:
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* FindFfsFileByName(
    IN CONST EFI_PEI_SERVICES **PeiServices, EFI_FV_FILETYPE FileType, 
    EFI_GUID *FileName
){
	EFI_FIRMWARE_VOLUME_HEADER* pFV;
	UINTN FvNum=0;
    //loop over all FV
	while( !EFI_ERROR ((*PeiServices)->FfsFindNextVolume (PeiServices, FvNum++, (VOID **)&pFV) ) ){
        EFI_FFS_FILE_HEADER* pFile = NULL;
	    EFI_STATUS Status;
	    // loop over raw files within FV
	    while(TRUE){
		    Status = (*PeiServices)->FfsFindNextFile(PeiServices, FileType, pFV, (VOID **)&pFile);
		    if( EFI_ERROR(Status) ) break;
            //make sure this is our RAW file.
            //guidcmp works like strcmp. It returns 0 when GUIDs are the same.
            if (guidcmp(&pFile->Name,&FileName)) continue;
            //skip file header
            return pFile+1;
	    }
    }
    return NULL;
}

EFI_STATUS PeiLoadFile(
    IN CONST EFI_PEI_SERVICES ** PeiServices,
    IN  EFI_PEI_FILE_HANDLE            FileHandle,
    OUT EFI_PHYSICAL_ADDRESS           *ImageAddress,
    OUT UINT64                         *ImageSize,
    OUT EFI_PHYSICAL_ADDRESS           *EntryPoint
){
    EFI_PEI_LOAD_FILE_PPI *Loader;
    UINT32 AuthenticationState;
    EFI_PEI_PPI_DESCRIPTOR *Dummy;
    EFI_STATUS Status = (*PeiServices)->LocatePpi(
        PeiServices, 
        &gLocalEfiPeiLoadFilePpiGuid,0, &Dummy, (VOID **)&Loader
    );
    if ((EFI_ERROR(Status))) return Status;
    return Status = Loader->LoadFile(
    	        Loader, FileHandle, ImageAddress, 
    	        ImageSize, EntryPoint,
    	        &AuthenticationState
    );
}

EFI_STATUS PciCfgModify(
    IN CONST EFI_PEI_SERVICES ** PeiServices,
	IN EFI_PEI_PCI_CFG_PPI_WIDTH		Width,
	IN UINT64							Address,
	IN UINTN							SetBits,
	IN UINTN							ClearBits
){
    if ((*PeiServices)->PciCfg==NULL) return EFI_NOT_AVAILABLE_YET;
    return (*PeiServices)->PciCfg->Modify(
        PeiServices, (*PeiServices)->PciCfg,
        Width, Address,
        &SetBits, &ClearBits
    );
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
