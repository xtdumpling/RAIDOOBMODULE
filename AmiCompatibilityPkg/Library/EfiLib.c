//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/SOURCE/Core/Library/EfiLib.c 97    7/18/11 3:43p Felixp $
//
// $Revision: 97 $
//
// $Date: 7/18/11 3:43p $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: EfiLib.c
//
// Description: 
//  Contains generic EFI library functions.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Protocol/AmiDebugService.h>

#include <AmiDxeLib.h>
#include <StatusCodes.h>
#include <Hob.h>
#include <Token.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileInfo.h>
#include <Protocol/LoadFile.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/StatusCode.h>
#include <Protocol/DiskIo.h>
#if SMM_SUPPORT
#include <Protocol/SmmStatusCode.h>
#endif
#include <Protocol/Cpu.h>
#include <Acpi50.h>
#include <Library/DebugPrintErrorLevelLib.h>
// ===========================
// In Aptio4, these are externals defined in the AmiProtocolLib.
// In AptioV, there is no AmiProtocolLib.
// We could've listed these in AmiDxeLib.inf, but it may break the
// build of some of the Aptio4 modules due to multiple defined symbols.
static EFI_GUID gLocalEfiSmmBaseProtocolGuid = EFI_SMM_BASE_PROTOCOL_GUID;
static EFI_GUID gLocalEfiCpuArchProtocolGuid = EFI_CPU_ARCH_PROTOCOL_GUID;
static EFI_GUID gLocalEfiDiskIoProtocolGuid = EFI_DISK_IO_PROTOCOL_GUID;
static EFI_GUID gLocalEfiComponentName2ProtocolGuid = EFI_COMPONENT_NAME2_PROTOCOL_GUID;
static EFI_GUID gLocalEfiSmmBase2ProtocolGuid = EFI_SMM_BASE2_PROTOCOL_GUID;
static EFI_GUID gLocalEfiComponentNameProtocolGuid = EFI_COMPONENT_NAME_PROTOCOL_GUID;
static EFI_GUID gLocalEfiBootScriptSaveGuid = EFI_BOOT_SCRIPT_SAVE_GUID;
#if SMM_SUPPORT
static EFI_GUID gLocalEfiSmmStatusCodeProtocolGuid = EFI_SMM_STATUS_CODE_PROTOCOL_GUID;
#endif
// ===========================

GLOBAL_REMOVE_IF_UNREFERENCED EFI_SYSTEM_TABLE 		*pST=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_BOOT_SERVICES 		*pBS=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_RUNTIME_SERVICES 	*pRS=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_BASE2_PROTOCOL*pSmmBase=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_SYSTEM_TABLE2	*pSmst=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED DXE_SERVICES			*pDxe=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE              TheImageHandle=NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID guidFS = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID guidFileInfo = EFI_FILE_INFO_ID;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID guidLoadFile = EFI_LOAD_FILE_PROTOCOL_GUID;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID guidDevicePath = EFI_DEVICE_PATH_PROTOCOL_GUID;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID guidFV = EFI_FIRMWARE_VOLUME2_PROTOCOL_GUID;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID guidDxeSvcTbl = DXE_SERVICES_TABLE_GUID;

static EFI_GUID gLocalEfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
static EFI_GUID gLocalEfiStatusCodeRuntimeProtocolGuid = EFI_STATUS_CODE_RUNTIME_PROTOCOL_GUID;
static EFI_STATUS_CODE_PROTOCOL *StatusCodePtr = NULL;
static AMI_DEBUG_SERVICE_PROTOCOL *AmiDebugServiceProtocol = NULL;

#if SMM_SUPPORT
static EFI_SMM_STATUS_CODE_PROTOCOL *StatusCodeSmmPtr = NULL;
static AMI_DEBUG_SERVICE_PROTOCOL *AmiDebugServiceProtocolSmmPtr = NULL;
#endif
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID SmmRsTableGuid = EFI_SMM_RUNTIME_SERVICES_TABLE_GUID; 

static BOOLEAN          IsAtRuntime = FALSE;
static BOOLEAN          IsInVirtualMode = FALSE;
static BOOLEAN          IsInSmm = FALSE;
extern EFI_GUID         gAmiGlobalVariableGuid;

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Device_Path_Functions
//
// Description:
//  Device path related functions defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiDxeLib  DPLength       Find the size of a device path in bytes.
// AmiDxeLib  DPAddNode      Create new device path with the contents of one device path + the first element from another.
// AmiDxeLib  DPAdd          Create new device path by appending a device path's elements to another device path.
// AmiDxeLib  DPAddInstance  Create new device path utilizing two provided device paths as instances.
// AmiDxeLib  DPCopy         Create new device path containing the exact contents of another device path.
// AmiDxeLib  DPIsOneOf      Determine whether a device path contains a user provided device path.
// AmiDxeLib  DPGetLastNode  Return the last node of a device path prior to END_OF_DEVICE_PATH.
// AmiDxeLib  DPCmp          Determine whether two device paths are equivalent.
// AmiDxeLib  DPCut          Create new device path with all but the last node of another device path.
// AmiDxeLib  DPGetEndNode   Return the last node of a device path.
// AmiDxeLib  DPNextInstance Create a copy of the current device path instance and updates the device path to point to the next instance.
// AmiDxeLib  DPCreateNode   Create a device path node.
// AmiDxeLib  DPIsMultiInstance Determine whether device path is single or multi-instance.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions.  Append .h to the given name.
//  
//<AMI_GHDR_END>
//*************************************************************************
 
//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Performance_Measurement_Functions
//
// Description:
//  Functions which are used to measure boot performance that are defined in
// the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiPeiLib  PEI_PERF_START Macro that starts timing for performance measurement.
// AmiPeiLib  PEI_PERF_END   Macro that stops timing for performance measurement.
// AmiPeiLib  PeiPerfMeasure Start or stop timing performance measurement.
// AmiDxeLib  PERF_START     Macro that starts timing for performance measurement.
// AmiDxeLib  StartMeasure   Start timing for performance measurement.
// AmiDxeLib  PERF_END       Macro that stops timing for performance measurement.
// AmiDxeLib  EndMeasure     Stop timing for performance measurement.
// AmiDxeLib  PERF_CODE      Macro that inserts performance measurement related code.
// AmiPeiLib  PEI_PERF_SAVE_S3_DATA         Macro that saves performance information to a HOB for S3 measurement.
// AmiPeiLib  SaveS3PerformanceData         Save performance information to a HOB for S3 measurement.
// AmiDxeLib  WriteBootToOsPerformanceData  Write performance information.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions.  Append .h to the given name.
//  
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Doubly_Linked_List_Functions
//
// Description:
// The following functions are provided by the AMI library as a basic API for
// a doubly-linked list.  Note that the DLINK node structure does not include
// an entry for the actual data.  There are several methods for getting
// around this limitation.
//
//  The easiest method is to create a new structure type which contains
// a DLINK structure as its first member and the data afterwards.  This new
// structure can then be typecasted to the old DLINK type when using the
// API.  See notes for an example.
//
//  There are presently no search functions provided for this API.
//
// Fields: Header Name Description
// ------------------------------------------------------------------
// AmiDxeLib DLIST       Doubly-linked list structure.
// AmiDxeLib DLINK       Doubly-linked list node.
// AmiDxeLib DListInit   Initialize a DLIST list structure.
// AmiDxeLib DListAdd    Add a DLINK node to the end of a DLIST.
// AmiDxeLib DListDelete Remove a DLINK node from a DLIST.
// AmiDxeLib DListInsert Insert a DLINK node after a specific DLINK node inside a DLIST.
// AmiDxeLib DListEmpty  Determine whether a DLIST is empty.
// 
// Notes:
//  Header details which header file contains the function prototype for
// the above functions; append .h to the given name.  None indicates that
// no header file contains a function prototype.
//
//  The following code demonstrates creating a node which
// contains a UINT32 data member, inserting it into a list, then changing
// its value.
//
//   typedef struct {
//     DLINK  Link;
//     UINT32 Data;
//   } UINT32_NODE;
//
//   DLIST       ExampleList;
//   UINT32_NODE ExampleNode;
//
//   ExampleNode.Data = 0xDEADBEEF;
//
//   DListInit(&ExampleList);
//   DListAdd(&ExampleList, (DLINK*) &ExampleNode);
//   ((UINT32_NODE*)ExampleList.pHead)->Data = 0x12345678;
//  
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: InitAmiLib
//
// Description:	
//  VOID InitAmiLib(IN EFI_HANDLE ImageHandle,
// IN EFI_SYSTEM_TABLE *SystemTable) initializes the global variables used by
// library routines. It must be called before any other library routine is
// used. See notes for variables initialized.
//
// Input:
//  IN EFI_HANDLE ImageHandle
// The image handle.
//
//  IN EFI_SYSTEM_TABLE *SystemTable
// Pointer to the EFI System Table.
//
// Output:
//  VOID.
//
// Modified:
//  pST
//  pBS
//  pRS
//  TheImageHandle
//
// Referrals:
// 
// Notes:	
//  The global variables initialized by this function are:
//   pST             Pointer to the EFI System Table.
//   pBS             Pointer to the Boot Services Table.
//   pRS             Pointer to the Runtime Services Table.
//   TheImageHandle  Image handle.
//
//<AMI_PHDR_END>
//*************************************************************************
VOID InitAmiLib(
	IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable
)
{
	if (pST) return;
	pST = SystemTable;
	pBS = pST->BootServices;
	pRS = pST->RuntimeServices;
    TheImageHandle = ImageHandle;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetEfiConfigurationTable
//
// Description:
//  VOID *GetEfiConfigurationTable(IN EFI_SYSTEM_TABLE *SystemTable,
// IN EFI_GUID *Guid) retrieves a pointer to the system configuration table
// from the EFI System Table based on a specified GUID.  If a match is found,
// then a pointer to the configuration table is returned.  If a matching GUID
// is not found, then NULL is returned.
//
// Input:
//  IN EFI_SYSTEM_TABLE *SystemTable
// Pointer to the EFI System Table.
//
//  IN EFI_GUID *Guid
// Pointer to table's GUID type.
//
// Output:
//  VOID* pointer to configuration table if a match is found; otherwise NULL.
//
// Modified:
//
// Referrals:
//  guidcmp
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID * GetEfiConfigurationTable(
	IN EFI_SYSTEM_TABLE *SystemTable,
	IN EFI_GUID			*Guid
	)
{
	EFI_CONFIGURATION_TABLE	*Table = SystemTable->ConfigurationTable;
	UINTN i = SystemTable->NumberOfTableEntries;

	for (;i;--i,++Table)
	{
		if (guidcmp(&Table->VendorGuid,Guid)==0) return Table->VendorTable;
	}
	return 0;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LibInitStatusCodePtr
//
// Description:
//  This function initializes pointers to StatusCode protocols in and outside SMM.
//
// Input: None
//
// Output:
//  EFI_STATUS: EFI_SUCCESS - if pointer already initialized or LocateProtocol was successful.
//		        EFI_UNSUPPORTED - if no protocol was located.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS LibInitStatusCodePtr()
{
    EFI_STATUS Status = EFI_SUCCESS;
    if (IsInSmm){
        if (!StatusCodeSmmPtr){
            if (IsAtRuntime || pSmst==NULL) return EFI_UNSUPPORTED;
            Status = pSmst->SmmLocateProtocol(
            &gLocalEfiSmmStatusCodeProtocolGuid,NULL,&StatusCodeSmmPtr
            );
        }
        return Status;
    }else if (!StatusCodePtr){
        if (IsAtRuntime) return EFI_UNSUPPORTED;
        Status = pBS->LocateProtocol(
            &gLocalEfiStatusCodeRuntimeProtocolGuid,NULL,&StatusCodePtr
        );
    }
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LibReportStatusCode
//
// Description:	
//  EFI_STATUS LibReportStatusCode(IN EFI_STATUS_CODE_TYPE Type, 
// IN EFI_STATUS_CODE_VALUE Value, IN UINT32 Instance,
// IN EFI_GUID *CallerId OPTIONAL, IN EFI_STATUS_CODE_DATA *Data OPTIONAL)
// abstracts status code reporting.  The Framework moved from a proprietary
// to a UEFI 2.0-based mechanism.  This function abstracts the caller from
// how the status code is reported to prevent the code from having to change
// with this version of the specification supported.
//
// Input:
//  IN EFI_STATUS_CODE_TYPE Type
// Indicates the type of status code being reported. 
//
//  IN EFI_STATUS_CODE_VALUE Value
// Describes the current status of a hardware or software entity. This
// includes information about the class and subclass that is used to classify
// the entity as well as an operation. For progress codes, the operation is
// the current activity. For error codes, it is the exception. For debug
// codes, it is not defined at this time.
//
//  IN UINT32 Instance
// The enumeration of a hardware or software entity within the system. A
// system may contain multiple entities that match a class/subclass pairing.
// The instance differentiates between them. An instance of 0 indicates that
// instance information is unavailable, not meaningful, or not relevant.
// Valid instance numbers start with 1.
//
//  IN EFI_GUID *CallerId OPTIONAL
// This optional parameter may be used to identify the caller. This parameter
// allows the status code driver to apply different rules to different
// callers.
//
//  IN EFI_STATUS_CODE_DATA *Data OPTIONAL
// This optional parameter may be used to pass additional data. The contents
// of this data type may have additional GUID-specific data.
//
// Output:
//  EFI_UNSUPPORTED, if Status Code protocol unavailable.
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

EFI_STATUS LibReportStatusCode(
	IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
	IN UINT32 Instance, IN EFI_GUID *CallerId OPTIONAL,
	IN EFI_STATUS_CODE_DATA *Data OPTIONAL
){
    EFI_STATUS Status;
#if SMM_SUPPORT && !USE_DXE_STATUS_CODE_INSTANCE_IN_SMM
    if (IsInSmm){
        if (StatusCodeSmmPtr==NULL) return EFI_NOT_FOUND;
        return StatusCodeSmmPtr->ReportStatusCode(StatusCodeSmmPtr,Type,Value,Instance,CallerId,Data);
    }else 
#endif
    Status = LibInitStatusCodePtr();
    if(EFI_ERROR(Status)) return Status;
    return StatusCodePtr->ReportStatusCode(Type,Value,Instance,CallerId,Data);
}

/**
  The function returns AmiDebugService instance
  @retval AmiDebugService instance or NULL if service is not available
 */
EFI_STATUS TraceGetAmiDebugServicesProtocolInstance (VOID)
{
    EFI_STATUS Status;
    EFI_TPL CurrentTpl;
    
    if (IsInSmm)
    {
        if (AmiDebugServiceProtocolSmmPtr != NULL) return EFI_SUCCESS;
        if (pSmst == NULL) return EFI_UNSUPPORTED;
        Status = pSmst->SmmLocateProtocol(&gAmiSmmDebugServiceProtocolGuid, NULL, &AmiDebugServiceProtocolSmmPtr);
        if (EFI_ERROR(Status)) AmiDebugServiceProtocolSmmPtr = NULL;
    }
    else 
    {
        if (AmiDebugServiceProtocol != NULL) return EFI_SUCCESS;

        // IsAtRuntime is reset to NULL in the exit-boot-services callback.  
        //  This check makes sure we don't try to locate Protocols at runtime
        if (IsAtRuntime == TRUE) return EFI_UNSUPPORTED;
        
        // It's illegal to call LocateProtocol at TPL higher than TPL_NOTIFY
        CurrentTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
        pBS->RestoreTPL(CurrentTpl);
        if ( CurrentTpl > TPL_NOTIFY ) return EFI_UNSUPPORTED;
        Status = pBS->LocateProtocol(&gAmiDebugServiceProtocolGuid, NULL, &AmiDebugServiceProtocol);
        if (EFI_ERROR(Status)) AmiDebugServiceProtocol = NULL;
    }
    
    return Status;
}
//*************************************************************************
//
// Name: PrintDebugMessageVaList
//
// Description:
//  VOID PrintDebugMessageVaList(IN UINTN Level, IN CHAR8 *sFormat,
// IN va_list ArgList) generates a Status Code string with
// PrepareStatusCodeString, then outputs it through LibReportStatusCode.
//
// Input:
//  IN UINTN Level
// The error level of the debug message.
//
//  IN CHAR8 *sFormat
// Format string for the debug message to print.
//
//  IN va_list ArgList
// Argument list for the format string.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//  PrepareStatusCodeString
//  LibReportStatusCode
// 
// Notes:	
//  Helper function for Trace.
//          
//*************************************************************************
VOID PrintDebugMessageVaList(UINTN Level, CHAR8 *sFormat, va_list ArgList)
{
    CHAR8  Buffer[2048];

    PrepareStatusCodeString( Buffer, sizeof(Buffer), sFormat, ArgList );
    LibReportStatusCode(
        EFI_DEBUG_CODE, 
        EFI_SOFTWARE_UNSPECIFIED, 
        0, 
        NULL,
        (EFI_STATUS_CODE_DATA*)Buffer
    );
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Trace
//
// Description:
//  VOID Trace(IN UINTN Level, IN CHAR8 *sFormat, IN ...) prints a debug
// message using status code protocol if the specified error level is
// enabled.
//
// Input:
//  IN UINTN Level
// The error level of the debug message. 
//
//  IN CHAR8 *sFormat
// Format string for the debug message to print.
//
//  IN ...
// Argument list for the format string.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//  va_start
//  PrintDebugMessageVaList
//  va_end
//  TRACE(Arguments)
//  Sprintf
// 
// Notes:
//  Use the TRACE(Arguments) macro for generating debug messages in DXE.
// This allows the DEBUG_MODE token to control whether the debug message is
// included or removed in the build process.  See Sprintf for format string
// parameters.
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID Trace(UINTN Level, CHAR8 *sFormat,...)
{
    EFI_STATUS Status;
    va_list	ArgList;
    UINT32  PcdErrorLevel;
    AMI_DEBUG_SERVICE_PROTOCOL *LocalAmiDebugServiceProtocolPtr = NULL;
    
    Status = TraceGetAmiDebugServicesProtocolInstance();
    if (EFI_ERROR(Status))
        return;
    
    if (IsInSmm)
        LocalAmiDebugServiceProtocolPtr = AmiDebugServiceProtocolSmmPtr;
    else
        LocalAmiDebugServiceProtocolPtr = AmiDebugServiceProtocol;
                
    PcdErrorLevel = GetDebugPrintErrorLevel();
	
	if ( (LocalAmiDebugServiceProtocolPtr == NULL) || (((UINT32)Level & PcdErrorLevel) == 0) ) 
        return;

	ProcessFormatStringtoEdk2(sFormat);

	ArgList = va_start(ArgList,sFormat);
	LocalAmiDebugServiceProtocolPtr->Debug(Level, sFormat, ArgList);
	va_end(ArgList);
}



//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetImageName
//
// Description:
//  BOOLEAN GetImageName(IN EFI_HANDLE ImageHandle, OUT CHAR8 *sName)
// retrieves the name of the image specified by ImageHandle and returns it
// in sName. 
//
// Input:
//  IN EFI_HANDLE ImageHandle
// The image handle whose name is desired.
//
//  OUT CHAR8 *sName
// Address of the null-terminated ASCII string to put image name into.  User
// is reponsible for allocating the necessary memory resources.
//
// Output:
//  TRUE if the image name has been found; otherwise, FALSE.
//
// Modified:
//
// Referrals:
//  GetName
//  Sprintf
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN GetImageName(EFI_HANDLE ImageHandle, CHAR8 *sName)
{
	EFI_LOADED_IMAGE_PROTOCOL *pImage;
	EFI_COMPONENT_NAME_PROTOCOL *pComponentName;
	CHAR16 *wsName;
	EFI_STATUS Status;
	if (   !EFI_ERROR(Status = pBS->HandleProtocol(ImageHandle,&gLocalEfiLoadedImageProtocolGuid,&pImage))
		&& GetName((UINT8*)pImage->ImageBase,sName)
	) return TRUE;

    if (    !EFI_ERROR(pBS->HandleProtocol(ImageHandle,&gLocalEfiComponentName2ProtocolGuid,(VOID **)&pComponentName))
         && (     !EFI_ERROR(pComponentName->GetDriverName(pComponentName,LANGUAGE_CODE_ENGLISH,&wsName))
               || !EFI_ERROR(pComponentName->GetDriverName(pComponentName,"en",&wsName))   
            )
            //Try UEFI 2.0 ComponentName protocol
       ||   !EFI_ERROR(pBS->HandleProtocol(ImageHandle,&gLocalEfiComponentNameProtocolGuid,(VOID **)&pComponentName))
         && !EFI_ERROR(pComponentName->GetDriverName(pComponentName,"eng",&wsName))
    )
	{
		Sprintf(sName,"%S", wsName);
		return TRUE;
	}
	if (   !EFI_ERROR(Status)
		&& pImage->FilePath->Type==MEDIA_DEVICE_PATH
		&& pImage->FilePath->SubType==MEDIA_FV_FILEPATH_DP
	)
	{
 		Sprintf(sName,"[%G]",&((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)pImage->FilePath)->FvFileName);
		return TRUE;            // Build: "NameGuid" changed to FvFileName" as the strcture MEDIA_FW_VOL_FILEPATH_DEVICE_PATH has been changed.
	}
	Sprintf(sName,"Unknown");
	return FALSE;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FindDeviceName
//
// Description:
//  BOOLEAN FindDeviceName( IN EFI_HANDLE Controller, IN EFI_HANDLE Child,
// OUT CHAR16 **wsName) checks all the protocols associated with a device
// handle and its child controller for its name; if a child is not defined,
// checks all associated protocols.
//
// Input:
//  IN EFI_HANDLE Controller
// The image handle whose name is desired.
//
//  IN EFI_HANDLE Child
// The child handle to be checked.  If set to NULL, checks all protocols
// associated with the Controller instead.
//
//  OUT CHAR16 **wsName
// Address of the null-terminated Unicode string to put device name into.
// User is reponsible for allocating the necessary memory resources.
//
// Output:
//  TRUE if device name found; otherwise, FALSE.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN FindDeviceName(
    EFI_HANDLE Controller, EFI_HANDLE Child, 
    CHAR16 **wsName, BOOLEAN *ChildFound
){
	UINTN i, Count;
	EFI_GUID **ppGuid;
	BOOLEAN NameFound = FALSE;
	UINT32 Attributes = Child ? EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER : EFI_OPEN_PROTOCOL_BY_DRIVER;
	if (EFI_ERROR(pBS->ProtocolsPerHandle(Controller, &ppGuid, &Count))) return FALSE;
    if (ChildFound) *ChildFound=FALSE;
	for(i=0; !NameFound && i<Count; i++)
	{
		UINTN j, InfoCount;
		EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *pInfo;
        // This "if (!guidcmp(..." statement below is a workaround for mass storage devices that have no partition 
        // (floppy or USB key formatted as floppy). The name for these devices 
        // is provided by more than one driver.
        // One of the name sources if file system driver. It provides a name like "FAT File System",
        // which is not very interesting. The real device name  like "Kingston DataTreveler"
        // (that comes from other driver)  is far more useful.
        // This workaround is to ignore name returned by a consumer of the disk I/O protocol,
        // which is a file system driver.
        if (!guidcmp(ppGuid[i],&gLocalEfiDiskIoProtocolGuid)) continue;
		if (EFI_ERROR(pBS->OpenProtocolInformation(
			Controller,ppGuid[i],&pInfo,&InfoCount
		))) continue;
		for(j=0; j<InfoCount; j++)
		{
			EFI_COMPONENT_NAME_PROTOCOL *pComponentName;
			if (pInfo[j].Attributes!=Attributes) continue;
			if (Child){
                if (pInfo[j].ControllerHandle!=Child)
                    continue;
                if (ChildFound) *ChildFound=TRUE;
            }
			if (!EFI_ERROR(pBS->HandleProtocol(
				pInfo[j].AgentHandle,&gLocalEfiComponentName2ProtocolGuid,&pComponentName
			))){
    			if (!EFI_ERROR(pComponentName->GetControllerName(
    					pComponentName,Controller,Child,LANGUAGE_CODE_ENGLISH,wsName
    			))){NameFound=TRUE; break;}
    			else if (!EFI_ERROR(pComponentName->GetControllerName(
                        pComponentName,Controller,Child,"en",wsName
                ))){NameFound=TRUE; break;}
            }
			//Try UEFI 2.0 ComponentName protocol
			if (!EFI_ERROR(pBS->HandleProtocol(
				pInfo[j].AgentHandle,&gLocalEfiComponentNameProtocolGuid,&pComponentName
			))){
    			if (!EFI_ERROR(pComponentName->GetControllerName(
    					pComponentName,Controller,Child,"eng",wsName
    			))){NameFound=TRUE; break;}
            }
		}
		if (InfoCount) pBS->FreePool(pInfo);
	}
	if (Count) pBS->FreePool(ppGuid);
	return NameFound;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetControllerName
//
// Description:
//  BOOLEAN GetControllerName(IN EFI_HANDLE Controller, OUT CHAR16 **wsName) 
// retrieves the name of the controller specified by Controller and copies
// it into the caller allocated Unicode string at address wsName.
//
// Input:
//  IN EFI_HANDLE Controller
// Handle of the controller whose name is desired.
//
//  OUT CHAR16 **wsName
// Address of the null-terminated Unicode string to put controller name into.
//
// Output:
//  TRUE if name found; otherwise, FALSE.
//
// Modified:
//
// Referrals:
//  FindDeviceName
//  DPCopy
//  DPGetLastNode
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN GetControllerName(EFI_HANDLE Controller, CHAR16 **wsName)
{
	const EFI_DEVICE_PATH_PROTOCOL End = {END_DEVICE_PATH,END_ENTIRE_SUBTYPE,sizeof(EFI_DEVICE_PATH_PROTOCOL)};
	BOOLEAN NameFound;
	EFI_HANDLE Parent;
	EFI_DEVICE_PATH_PROTOCOL *pDevicePath, *pLastNode, *pDp;
    BOOLEAN ChildFound;

	if (FindDeviceName(Controller, NULL, wsName, NULL)) return TRUE;
	if (EFI_ERROR(pBS->HandleProtocol(Controller, &guidDevicePath, &pDevicePath))) return FALSE;
	pDevicePath = DPCopy(pDevicePath);
    do{
        ChildFound=TRUE;
        pDp = pDevicePath;
	    pLastNode = DPGetLastNode(pDevicePath);
        if (!pLastNode){
            NameFound =  FALSE;
            break;
        }
	    *pLastNode = End;
	    if (	!EFI_ERROR(pBS->LocateDevicePath(&guidDevicePath, &pDp, &Parent))
		    &&	pDp==pLastNode
	    ){
            NameFound = FindDeviceName(Parent, Controller, wsName, &ChildFound);
	    }else{
            NameFound =  FALSE;
            break;
        }
    }while(!ChildFound);
	pBS->FreePool(pDevicePath);
	return NameFound;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FvReadPe32Image
//
// Description:
//  EFI_STATUS FvReadPe32Image(IN EFI_GUID *NameGuid, IN OUT VOID **Buffer, 
// IN OUT UINTN *BufferSize, OUT UINT32 *AuthenticationStatus)
//
// Input:
//  IN EFI_GUID *NameGuid
// GUID associated with the image in the firmware volume.
//
//  IN OUT VOID **Buffer
// Pointer to buffer for the image to be returned in.  Caller may specify
// *Buffer = NULL, in which case FvReadPe32Image will dynamically allocate
// the necessary amount of memory and return the pointer to the buffer
// here.
//
//  IN OUT UINTN *BufferSize
// If Buffer is caller allocated, caller must put size of the Buffer here.
// Returns with the size of the image, even in the case the caller allocated
// Buffer is too small.
//
//  OUT UINT32 *AuthenticationStatus)
// Pointer to a caller-allocated UINT32 in which the authentication status
// is returned.  See Possible AuthenticationStatus Bit Values in the EFI 
// Firmware Volume Specification for more information.
//
// Output:
//  EFI_WARN_BUFFER_TOO_SMALL,  if Buffer is too small to contain image.
//                             BufferSize is updated with the necessary size,
//                             and Buffer is filled with the truncated image. 
//  EFI_OUT_OF_RESOURCES,       if an allocation failure occurred.
//  EFI_NOT_FOUND,              if the requested file or EFI_SECTION_PE32 was
//                             not found.
//  EFI_DEVICE_ERROR,           if a hardware error occurred when attempting
//                             to access the firmware volume.
//  EFI_ACCESS_DENIED,          if the firmware volume is configured to
//                             disallow reads.
//  EFI_PROTOCOL_ERROR,         if the GUIDED_SECTION_EXTRACTION_PROTOCOL for
//                             the EFI_SECTION_PE32 was not found.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  guidFV
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS FvReadPe32Image (
  IN EFI_GUID                       *NameGuid,
  IN OUT VOID                       **Buffer,
  IN OUT UINTN                      *BufferSize,
  OUT UINT32                        *AuthenticationStatus)
{
	EFI_FIRMWARE_VOLUME_PROTOCOL	*Fv;

	EFI_STATUS	Status;
	UINTN		NumHandles;
	EFI_HANDLE	*HandleBuffer;
	UINTN		i;

	Status = pBS->LocateHandleBuffer(ByProtocol,&guidFV,NULL,&NumHandles,&HandleBuffer);
	if (EFI_ERROR(Status)) return Status;

	for (i = 0; i< NumHandles; ++i)
	{
		pBS->HandleProtocol(HandleBuffer[i],&guidFV,&Fv);
		Status = Fv->ReadSection(
			Fv,NameGuid,
			EFI_SECTION_PE32,0,
			Buffer,BufferSize,
			AuthenticationStatus
		);
		if (!EFI_ERROR(Status)) break;
	}

	pBS->FreePool(HandleBuffer);
	return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Malloc
//
// Description:
//  VOID *Malloc(IN UINTN Size) allocates Size bytes of memory from
// EfiBootServicesData and returns a pointer to the allocated space.
//
// Input:
//  IN UINTN Size
// The number of bytes to be allocated.
//
// Output:
//  VOID* address of the allocated memory.
//
// Modified:
//
// Referrals:
// 
// Notes:
//  This function is available only in the DXE phase.  Use
// pBS->FreePool(VOID*) to return memory to the pool once it's no longer
// needed.
//          
//<AMI_PHDR_END>
//*************************************************************************
void* Malloc(UINTN Size){
	VOID *p=NULL;
	pBS->AllocatePool(EfiBootServicesData,Size,&p);
	return p;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MallocZ
//
// Description:
//  VOID* MallocZ(IN UINTN Size) allocates Size bytes of memory from
// EfiBootServicesData, initializes it to zero, then returns a pointer to
// the allocated space.
//
// Input:
//  IN UINTN Size
// The number of bytes to be allocated.
//
// Output:
//  VOID* address of the allocated and zeroed memory.
//
// Modified:
//
// Referrals:
//  Malloc
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
void* MallocZ(UINTN Size){
	VOID *p=NULL;
	p=Malloc(Size);
	if(!p) return p;
	pBS->SetMem(p,Size,0);
	return p;
}

static UINT8 DevicePathTypes[] = {
    HARDWARE_DEVICE_PATH,
    ACPI_DEVICE_PATH,
    MESSAGING_DEVICE_PATH,
    MEDIA_DEVICE_PATH,
    BBS_DEVICE_PATH,
    END_DEVICE_PATH
};

static UINTN DevicePathTypesSize = sizeof(DevicePathTypes) / sizeof(UINT8);
#define MAX_DP_SIZE 0x10000

EFI_STATUS IsValidDevicePath(
    IN EFI_DEVICE_PATH_PROTOCOL *pDp
)
{
    UINTN i;
    UINTN DevicePathSize = 0;
    UINTN NodeLength = 0;

    for( ; !(isEndNode(pDp) && pDp->SubType == END_ENTIRE_SUBTYPE); pDp = NEXT_NODE(pDp) ) {
        // verify that the length of the current Node is valid before continuing
        NodeLength = NODE_LENGTH(pDp);
        // return if the device path has an invalid length
        if (NodeLength < sizeof (EFI_DEVICE_PATH_PROTOCOL)) {
            return EFI_INVALID_PARAMETER;
        }
        // store total Device Path Length
        DevicePathSize += NodeLength;
        
        // check to see if this node is a valid Device Path type
        for(i = 0; i < DevicePathTypesSize; i++) {
            if(pDp->Type == DevicePathTypes[i])
                break;
        }
        // check to see if a matching Device Path type was found
        if((i == DevicePathTypesSize) || (DevicePathSize > MAX_DP_SIZE))    //unknown device path type or big size
            return EFI_INVALID_PARAMETER;
    }
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPLength
//
// Description:
//  UINTN DPLength(IN EFI_DEVICE_PATH_PROTOCOL *pDp) returns the length of
// the provided device path, pDp (including the size of EndOfDevicePath).
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp
// The device path whose length is desired.
//
// Output:
//  UINTN size of the device path, pDp, including EndOfDevicePath.
//
// Modified:
//
// Referrals:
//  isEndNode
//  NEXT_NODE
//  NODE_LENGTH
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
UINTN DPLength(EFI_DEVICE_PATH_PROTOCOL *pDp)
{
	UINTN Size;
    if (!pDp) return 0;
    Size = 0;
	for( 
		; !(isEndNode(pDp) && pDp->SubType==END_ENTIRE_SUBTYPE)
		; pDp = NEXT_NODE(pDp)
	){
        UINTN Length = NODE_LENGTH(pDp);
        //Protection from the junk data.
        //Zero type and zero length are illegal.
        //If we encountered them, return
        if (!pDp->Type || !Length) return Size;
        Size += Length;
    }
	return Size + sizeof(EFI_DEVICE_PATH_PROTOCOL); // add size of END_DEVICE_PATH node
}

static EFI_DEVICE_PATH_PROTOCOL EndOfDevicePathNode = {
    END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
    {sizeof(EFI_DEVICE_PATH_PROTOCOL),0}
};

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPAddNode
//
// Description:
//  VOID* DPAddNode(IN EFI_DEVICE_PATH_PROTOCOL *pDp1,
// IN EFI_DEVICE_PATH_PROTOCOL *pDp2) adds the very first element of pDp2 to
// pDp1 just before its device path terminator, and returns the result in a
// newly allocated buffer.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp1
// The device path to be appended to.  Element will be inserted prior to
// pDp1's device path terminator.  Device path will not be altered.
//
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp2
// The device path to be read from.  The very first element will be appended
// to pDp1; note that pDp2 does not require a device path terminator.
//
// Output:
//  VOID* address of the modified device path.  User is responsible for
// managing this piece of memory.
//
// Modified:
//
// Referrals:
//  DPCopy
//  DPLength
//  Malloc
//  MemCpy
//  NODE_LENGTH
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* DPAddNode(EFI_DEVICE_PATH_PROTOCOL *pDp1, EFI_DEVICE_PATH_PROTOCOL *pDp2)
{
	UINTN l1;
	UINT8 *NewDp, *p;

    if (!pDp2) return (pDp1) ? DPCopy(pDp1) : DPCopy(&EndOfDevicePathNode);

	l1 = pDp1 ? DPLength(pDp1)-sizeof(EFI_DEVICE_PATH_PROTOCOL) : 0;
	NewDp = Malloc(l1+NODE_LENGTH(pDp2)+sizeof(EFI_DEVICE_PATH_PROTOCOL));
	p = NewDp;

	if (l1) { MemCpy(p, pDp1, l1); p+=l1; }
	MemCpy(p, pDp2, NODE_LENGTH(pDp2)); p+=NODE_LENGTH(pDp2);
	*((EFI_DEVICE_PATH_PROTOCOL*)p) = EndOfDevicePathNode;
	return NewDp;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPAdd
//
// Description:
//  VOID* DPAdd(IN EFI_DEVICE_PATH_PROTOCOL *pDp1,
// IN EFI_DEVICE_PATH_PROTOCOL *pDp2) appends all the elements of pDp2 just
// before the device path terminator of pDp1, and returns the result in a
// newly allocated buffer.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp1
// The device path to be appended to.  Element(s) will be inserted prior to
// pDp1's device path terminator.  Device path will not be altered.
//
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp2
// The device path to be read from.  All elements will be appended to pDp1.
// If NULL, returns copy of pDp1.
//
// Output:
//  VOID* address of the modified device path.  User is responsible for
// managing this piece of memory.
//
// Modified:
//
// Referrals:
//  DPCopy
//  DPLength
//  Malloc
//  MemCpy
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* DPAdd(EFI_DEVICE_PATH_PROTOCOL *pDp1, EFI_DEVICE_PATH_PROTOCOL *pDp2)
{
	UINTN l1,l2;
	UINT8 *NewDp,*p;

    if (!pDp1) return (pDp2) ? DPCopy(pDp2) : DPCopy(&EndOfDevicePathNode);
    if (!pDp2) return DPCopy(pDp1);

	l1 = DPLength(pDp1)-sizeof(EFI_DEVICE_PATH_PROTOCOL);
	l2 = DPLength(pDp2);
	NewDp = Malloc(l1+l2);
	p = NewDp;

	MemCpy(p, pDp1, l1); p+=l1;
	MemCpy(p, pDp2, l2);
	return NewDp;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPAddInstance
//
// Description:
//  VOID* DPAddInstance(IN EFI_DEVICE_PATH_PROTOCOL *pDp1,
// IN EFI_DEVICE_PATH_PROTOCOL *pDp2) makes a new device path with pDp1 as
// its first instance and pDp2 as its second instance, then returns the
// result in a newly allocated buffer.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp1
// The first device path instance.
//
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp2
// The second device path instance.
//
// Output:
//  VOID* address of the modified device path.  User is responsible for
// managing this piece of memory.
//
// Modified:
//
// Referrals:
//  DPCopy
//  DPLength
//  Malloc
//  MemCpy
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* DPAddInstance(EFI_DEVICE_PATH_PROTOCOL *pDp1, EFI_DEVICE_PATH_PROTOCOL *pDp2)
{
	UINTN l1,l2;
	UINT8 *NewDp,*p;

    if (!pDp2) return NULL;        
    if (!pDp1) return DPCopy(pDp2);
    //Uncomment if pDp2 can be NULL:if (!pDp2) return DPCopy(pDp1);
    
	l1 = DPLength(pDp1);
	l2 = DPLength(pDp2);
	NewDp = Malloc(l1+l2);
	p = NewDp;

	MemCpy(p, pDp1, l1); p+=l1;
	(((EFI_DEVICE_PATH_PROTOCOL*)p)-1)->SubType = END_INSTANCE_SUBTYPE;
	MemCpy(p, pDp2, l2);
	return NewDp;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPCopy
//
// Description:
//  VOID* DPCopy(IN EFI_DEVICE_PATH_PROTOCOL *pDp) copies the user provided
// device path into a new buffer and returns its address.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp
// The device path to be copied.
//
// Output:
//  VOID* address of the new copy of pDp.  User is responsible for managing
// this piece of memory.
//
// Modified:
//
// Referrals:
//  DPLength
//  Malloc
//  MemCpy
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* DPCopy(EFI_DEVICE_PATH_PROTOCOL *pDp)
{
	UINTN l;
	UINT8 *p;

    if (!pDp) return NULL;
	l = DPLength(pDp);
	p = Malloc(l);
	MemCpy(p, pDp, l);
	return p;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPIsOneOf
//
// Description:
//  BOOLEAN DPIsOneOf(IN EFI_DEVICE_PATH_PROTOCOL *pAll,
// IN EFI_DEVICE_PATH_PROTOCOL *pOne, IN BOOLEAN ExactMatch) determines
// whether a device path, pAll, contains another device path, pOne.  If the
// ExactMatch parameter is TRUE, it checks for an exact match instead.
//
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pAll
// The device path to be scanned.
//
//  IN EFI_DEVICE_PATH_PROTOCOL *pOne
// The device path to locate within pAll.
//
//  IN BOOLEAN ExactMatch
// If TRUE, checks whether pAll is exactly pOne.
//
// Output:
//  TRUE if pAll contains pOne and ExactMatch is FALSE, or if pAll equals
// pOne and ExactMatch is TRUE.  Otherwise, FALSE.
//
// Modified:
//
// Referrals:
//  NODE_LENGTH
//  isEndNode
//  MemCmp
//  NEXT_NODE
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN DPIsOneOf(EFI_DEVICE_PATH_PROTOCOL *pAll, EFI_DEVICE_PATH_PROTOCOL *pOne, BOOLEAN ExactMatch)
{
	do{
		EFI_DEVICE_PATH_PROTOCOL *pPath;
		for(	pPath = pOne
			;	NODE_LENGTH(pPath)==NODE_LENGTH(pAll)
				&& !isEndNode(pPath)
				&& !MemCmp(pPath, pAll, NODE_LENGTH(pAll))
			;	pPath=NEXT_NODE(pPath), pAll=NEXT_NODE(pAll)
		) ;
		if (isEndNode(pAll) && (!ExactMatch || isEndNode(pPath))) return TRUE;
		for(; !isEndNode(pAll); pAll = NEXT_NODE(pAll)) ;	
		if (pAll->SubType==END_ENTIRE_SUBTYPE) return FALSE;
		pAll++ ;
	}while (TRUE);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPGetLastNode
//
// Description:
//  VOID* DPGetLastNode(IN EFI_DEVICE_PATH_PROTOCOL *pDp) returns a pointer
// to the very last device path node before END_OF_DEVICE_PATH node.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp
// The device path whose last node is needed.
//
// Output:
//  VOID* address of the last device path node before END_OF_DEVICE_PATH in
// the provided device path, pDp.
//
// Modified:
//
// Referrals:
//  isEndNode
//  NEXT_NODE
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* DPGetLastNode(EFI_DEVICE_PATH_PROTOCOL *pDp)
{
	EFI_DEVICE_PATH_PROTOCOL	*dp=NULL;
//---------------------------------
	if(!pDp)return dp;
	for( ; !isEndNode(pDp); pDp=NEXT_NODE(pDp))	dp = pDp;
	return 	dp;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPCmp
//
// Description:
//  INTN DPCmp(IN EFI_DEVICE_PATH_PROTOCOL *pDp1,
// IN EFI_DEVICE_PATH_PROTOCOL *pDp2) compares two device paths, pDp1 and
// pDp2, and returns zero if both are equivalent; otherwise, it returns
// a non-zero value.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp1
// Address of the first device path to compare.
//
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp2
// Address of the second device path to compare.
//
// Output:
//  INTN non-zero value if both input device paths are not equivalent;
// otherwise, zero.
//
// Modified:
//
// Referrals:
//  DPLength
//  MemCmp
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
//Compares Dp1 and Dp2 returns ZERO if Dp1==Dp2, NONZERO if Dp1!=Dp2  
INTN DPCmp(EFI_DEVICE_PATH_PROTOCOL *pDp1, EFI_DEVICE_PATH_PROTOCOL *pDp2){
	UINTN	len;
//---------------------
	if(!pDp1 && !pDp2) return 0; //if both is NULL than Dp1==Dp2
	if(!pDp1 || !pDp2) return -1; // if one is NULL than Dp1!=Dp2
	len=DPLength(pDp1);
	if(DPLength(pDp2)!=len) return -1;
	return MemCmp(pDp1,pDp2, len);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPCut
//
// Description:
//  VOID* DPCut(IN EFI_DEVICE_PATH_PROTOCOL *pDp) creates a new device path
// containing all but the last node of pDp, then returns the address to the
// new device path.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp
// The device path to be copied.
//
// Output:
//  VOID* address of the new copy of pDp containing all but the last node.
// Returns NULL if there is not enough memory or pDp is NULL.
//
// Modified:
//
// Referrals:
//  DPGetLastNode
//  Malloc
//  MemCpy
//  SET_NODE_LENGTH
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID *DPCut(EFI_DEVICE_PATH_PROTOCOL *pDp)
{
	EFI_DEVICE_PATH_PROTOCOL *pdp, *edp;
	UINTN			len;
//------------------------------
	if(!pDp)return NULL;
	pdp=DPGetLastNode(pDp);
	len=(UINTN)pdp-(UINTN)pDp;
	if(len==0) return NULL;
	pdp=Malloc(len+sizeof(EFI_DEVICE_PATH_PROTOCOL));
	ASSERT(pdp);
	MemCpy(pdp,pDp,len);
	edp=(EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)pdp+len);
	edp->Type=END_DEVICE_PATH;
	edp->SubType=END_ENTIRE_SUBTYPE;
	SET_NODE_LENGTH(edp,END_DEVICE_PATH_LENGTH);
	return pdp;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPGetEndNode
//
// Description:
//  EFI_DEVICE_PATH_PROTOCOL* DPGetEndNode(IN EFI_DEVICE_PATH_PROTOCOL *pDp)
// returns the end of the device path node for the specified device path.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *pDp
// Address of the device path.
//
// Output:
//  EFI_DEVICE_PATH_PROTOCOL* address of the end of device path node.  NULL
// if pDp is NULL.
//
// Modified:
//
// Referrals:
//  isEndNode
//  NEXT_NODE
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_DEVICE_PATH_PROTOCOL* DPGetEndNode(EFI_DEVICE_PATH_PROTOCOL *pDp)
{
	if(!pDp)return NULL;
	for( ; !isEndNode(pDp); pDp=NEXT_NODE(pDp))	;
	return 	pDp;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPNextInstance
//
// Description:
//  VOID* DPNextInstance(IN OUT EFI_DEVICE_PATH_PROTOCOL **ppDp,
// OUT UINTN *pSize) creates a copy of the current device path instance. It
// also updates DevicePath to point to the next device path instance in the
// device path (or NULL if no more) and updates Size to hold the size of the
// device path instance copy. If DevicePath is NULL, then NULL is returned.
// If there is not enough memory to allocate space for the new device path,
// then NULL is returned.  The memory is allocated from EFI boot services
// memory. It is the responsibility of the caller to free the memory
// allocated.
//
// Input:
//  IN OUT EFI_DEVICE_PATH_PROTOCOL **ppDp
// On input, this holds the pointer to the current device path instance. On
// output, this holds the pointer to the next device path instance or NULL
// if there are no more device path instances in the device path pointer to
// a device path data structure.
//
//  OUT UINTN *pSize
// On output, this holds the size of the device path instance, in bytes, or
// zero if DevicePath is NULL.
//
// Output:
//  VOID* address of a copy of the next device path instance.  NULL if there
// are no more device path instances, or not enough memory, or the provided
// device path was NULL.  It is the responsibility of the caller to free the
// memory allocated.
//
// Modified:
//
// Referrals:
//  DPGetEndNode
//  Malloc
//  MemCpy
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* DPNextInstance(EFI_DEVICE_PATH_PROTOCOL **ppDp, UINTN *pSize)
{
    EFI_DEVICE_PATH_PROTOCOL *pEnd, *pNew;
    UINTN Size;
    if (pSize) *pSize=0;
    if (!ppDp) return NULL;
    pEnd = DPGetEndNode(*ppDp);
    if (!pEnd) return NULL;
    Size = (UINT8*)(pEnd+1) - (UINT8*)*ppDp;
    if (pSize) *pSize=Size;
    if (!(pNew = Malloc(Size))) return NULL;
    MemCpy(pNew,*ppDp, Size);
    ((EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)pNew+Size)-1)->SubType = END_ENTIRE_SUBTYPE;
    if (pEnd->SubType==END_ENTIRE_SUBTYPE) *ppDp = NULL;
    else *ppDp = pEnd+1;
    return pNew;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPCreateNode
//
// Description:
//  VOID* DPCreateNode(IN UINT8 Type, IN UINT8 SubType, IN UINT16 Length)
// creates a new device node in a newly allocated buffer of size NodeLength
// and initializes the device path node header with NodeType and NodeSubType.  
// The new device path node is returned.  If NodeLength is smaller than a
// device path header, then NULL is returned.  If there is not enough memory
// to allocate space for the new device path, then NULL is returned.  The
// memory is allocated from EFI boot services memory. It is the
// responsibility of the caller to free the memory allocated.
//
// Input:
//  IN UINT8 Type
// The device node type for the new device node.
//
//  IN UINT8 SubType
// The device node sub-type for the new device node.
//
//  IN UINT16 Length
// The length of the new device node.
//
// Output:
//  VOID* address of the new device path node.  NULL if NodeLength is
// smaller than a device path header, or there is not enough memory.
//
// Modified:
//
// Referrals:
//  Malloc
//  SET_NODE_LENGTH
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* DPCreateNode(UINT8 Type, UINT8 SubType, UINT16 Length)
{
    EFI_DEVICE_PATH_PROTOCOL *pDp;

    if (Length<sizeof(EFI_DEVICE_PATH_PROTOCOL)) return NULL;
    if ( !(pDp = MallocZ(Length)) ) return NULL;

    pDp->Type = Type;
    pDp->SubType = SubType;
    SET_NODE_LENGTH(pDp,Length);
    return pDp;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DPIsMultiInstance
//
// Description:
//  BOOLEAN DPIsMultiInstance(IN EFI_DEVICE_PATH_PROTOCOL* pDp) determines
// if a device path is single or multi-instance.  This function returns TRUE
// if the device path specified by pDp is multi-instance.  Otherwise, FALSE
// is returned.  If pDp is NULL, then FALSE is returned.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL* pDp
// A pointer to a device path data structure.
//
// Output:
//  TRUE if the device path specified by DevicePath is multi-instance.
//  FALSE if pDp is NULL or not multi-instance.
//
// Modified:
//
// Referrals:
//  DPGetEndNode
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN DPIsMultiInstance(EFI_DEVICE_PATH_PROTOCOL* pDp)
{
    if (!pDp) return FALSE;
    return DPGetEndNode(pDp)->SubType != END_ENTIRE_SUBTYPE;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: TimerIsSet
//
// Description:
//  VOID TimerIsSet(IN EFI_EVENT Event, IN OUT VOID *Context)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID TimerIsSet(EFI_EVENT Event, VOID *Context)
{
	*(BOOLEAN*)Context = TRUE;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: TimerStart
//
// Description:
//  EFI_EVENT TimerStart(OUT BOOLEAN *pTimerFlag, IN UINT64 Delay)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//  TimerIsSet
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_EVENT TimerStart(BOOLEAN *pTimerFlag, UINT64 Delay)
{
	EFI_STATUS	Status;
	EFI_EVENT 	Event;

	*pTimerFlag = FALSE;

	Status = pBS->CreateEvent(
		EVT_TIMER|EVT_NOTIFY_SIGNAL,
		TPL_NOTIFY,
		TimerIsSet,
		pTimerFlag,
		&Event
	);
	if (EFI_ERROR(Status)) return NULL;


	Status = pBS->SetTimer(Event, TimerRelative, Delay);
	if (EFI_ERROR(Status))
	{
		pBS->CloseEvent(Event);
		return NULL;
	}

	return Event;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: TimerStop
//
// Description:
//  VOID TimerStop(IN BOOLEAN TimerFlag, IN EFI_EVENT Event)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
void TimerStop(BOOLEAN TimerFlag, EFI_EVENT Event)
{
	if (!TimerFlag) pBS->SetTimer(Event, TimerCancel,0);
	pBS->CloseEvent(Event);
}
/*
//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FSReadFile
//
// Description:
//  EFI_STATUS FSReadFile(IN EFI_HANDLE hDevice, IN EFI_DEVICE_PATH_PROTOCOL* pPath, OUT VOID** ppSource, OUT UINTN *pSize)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//  isEndNode
//  NODE_LENGTH
//  MemCpy
//  Malloc
//  NEXT_NODE
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS FSReadFile(EFI_HANDLE hDevice, EFI_DEVICE_PATH_PROTOCOL* pPath, VOID** ppSource, UINTN *pSize)
{
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *pFS;
	EFI_FILE_PROTOCOL *pRootFile=NULL, *pFile=NULL;
//S.Y. 
//FileInfo contains not a pointer to FileName Buffer
//But Copy of the file name at the very end of the file 
	EFI_FILE_INFO *pFileInfo=NULL;
//S.Y.
	UINTN Size=0;
	CHAR16 *wsFileName;
	EFI_DEVICE_PATH_PROTOCOL *pDp;
	VOID *pSource = NULL;
	EFI_STATUS Status;
	if (EFI_ERROR(pBS->HandleProtocol(hDevice, &guidFS, &pFS))) return EFI_NOT_FOUND;
	if (EFI_ERROR(pFS->OpenVolume(pFS,&pRootFile))) return EFI_NOT_FOUND;
	//create file name
	//1. Determine Buffer size to hold filename
	Size = 0;
	pDp = pPath; 	
	while(!isEndNode(pDp)){ 				
		if(pDp->Type == MEDIA_DEVICE_PATH && pDp->SubType == MEDIA_FILEPATH_DP){
			Size=Size+(NODE_LENGTH(pDp)-sizeof(EFI_DEVICE_PATH_PROTOCOL));		
		} else break;		
		pDp = NEXT_NODE(pDp);
	}

	if ( !isEndNode(pDp) ) return EFI_NOT_FOUND;
	//2. Alocate buffer for file name 
	wsFileName = Malloc(Size);
	if (!wsFileName) return EFI_OUT_OF_RESOURCES;
	//3.Copy Characters fron FileDevicePAth to the Buffer
	Size=0;//Now Size will show position of char in wsFileName buffer
	pDp = pPath; 	
	while(!isEndNode(pDp)){
		if(pDp->Type == MEDIA_DEVICE_PATH && pDp->SubType == MEDIA_FILEPATH_DP){
			UINTN l = NODE_LENGTH(pDp)-sizeof(EFI_DEVICE_PATH_PROTOCOL);
			MemCpy(wsFileName+Size,pDp+1,l);
			Size=Size+(l/sizeof(CHAR16));
// PLEASE NOTE::
//EFI Shell prepares File DevicePath very strange it should not be like this
//when you calling App from fsX:\DIR_NAME> app_name it creates following dev path entry
//(\DIR_NAME);('.');(APP_NAME.efi);(END_DP). 
//but when you type fsX:\> DIR_NAME\app_name it creates 
// (\);(DIR_NAME\);(app_name);(END_DP).  
			//replace str_terminator with "\" char if previous char is not "\"
			if(wsFileName[Size-2]!=0x005c) wsFileName[Size-1]=0x005c; //"\"symbol
			else Size--;
//END of Change for EFI Shell
		} else break;
		pDp = NEXT_NODE(pDp);
	}
	
	wsFileName[Size-1]=0;
	Status = pRootFile->Open(pRootFile,&pFile,wsFileName,EFI_FILE_MODE_READ,0);
	pBS->FreePool(wsFileName);
	if (EFI_ERROR(Status)) return EFI_NOT_FOUND;
//S.Y.
	//Size = sizeof(FileInfo);
	Size=0;
	//Try to determine Size of memory to allocate for EFI_FILE_INFO var
	Status=pFile->GetInfo(pFile,&guidFileInfo,&Size,pFileInfo);
//	if (!EFI_ERROR(Status=pFile->GetInfo(pFile,&guidFileInfo,&Size,&FileInfo)))
	if(EFI_ERROR(Status)){
		if (Status==EFI_BUFFER_TOO_SMALL){
			pFileInfo=Malloc(Size);
			if(!pFileInfo) {
				Status=EFI_OUT_OF_RESOURCES;		
				goto Done;
			} 
		} else goto Done;
	}	
		
	Status=pFile->GetInfo(pFile,&guidFileInfo,&Size,pFileInfo);
	if(EFI_ERROR(Status)){
		pBS->FreePool(pFileInfo);	
		goto	Done;
	}			
		
	Size = (UINTN)pFileInfo->FileSize;

	pBS->FreePool(pFileInfo);	
	
	pSource=Malloc(Size);
	if (!pSource) Status=EFI_OUT_OF_RESOURCES;
	else Status=pFile->Read(pFile,&Size,pSource);

Done:
	if(pFile)pFile->Close(pFile);
	if(pRootFile)pRootFile->Close(pRootFile);
	if (EFI_ERROR(Status) && pSource){ 
		pBS->FreePool(pSource); 
		return Status;
	}
	*ppSource = pSource;
	*pSize = Size;
	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FVReadFile
//
// Description:
//  EFI_STATUS FVReadFile(IN EFI_HANDLE hDevice, IN EFI_GUID* pFileName, OUT VOID** ppSource, OUT UINTN *pSize)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS FVReadFile(EFI_HANDLE hDevice, EFI_GUID* pFileName, VOID** ppSource, UINTN *pSize)
{
	EFI_FIRMWARE_VOLUME_PROTOCOL *pFV;
	UINT32 AuthStatus;
	if (EFI_ERROR(pBS->HandleProtocol(hDevice, &guidFV, &pFV))) return EFI_NOT_FOUND;
	// no support for raw files
	if (EFI_ERROR(pFV->ReadSection(pFV, pFileName, EFI_SECTION_PE32, 
								   0, ppSource, pSize, &AuthStatus
								  )
				 )
	   ) return EFI_UNSUPPORTED;
	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LFReadFile
//
// Description:
//  EFI_STATUS LFReadFile(IN EFI_HANDLE hDevice, IN EFI_DEVICE_PATH_PROTOCOL* pPath, IN BOOLEAN BootPolicy, OUT VOID** ppSource, OUT UINTN *pSize)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS LFReadFile(EFI_HANDLE hDevice, EFI_DEVICE_PATH_PROTOCOL* pPath, BOOLEAN BootPolicy, VOID** ppSource, UINTN *pSize)
{
	EFI_LOAD_FILE_PROTOCOL *pLoadFile;
	VOID *pSource = NULL;
	UINTN Size=0;
	if (EFI_ERROR(pBS->HandleProtocol(hDevice, &guidLoadFile, &pLoadFile))) return EFI_NOT_FOUND;
	if (pLoadFile->LoadFile(pLoadFile, pPath, BootPolicy, &Size, pSource)!=EFI_BUFFER_TOO_SMALL) return EFI_NOT_FOUND;
	if (!Size) return EFI_NOT_FOUND;
	pSource=Malloc(Size);
	if (!pSource) return EFI_OUT_OF_RESOURCES;
	if (EFI_ERROR(pLoadFile->LoadFile(pLoadFile, pPath, BootPolicy, &Size, pSource)!=EFI_BUFFER_TOO_SMALL))
	{
		pBS->FreePool(pSource);
		return EFI_NOT_FOUND;
	}
	*ppSource = pSource;
	*pSize = Size;
	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ReadImage
//
// Description:
//  EFI_STATUS ReadImage(IN BOOLEAN BootPolicy,
// IN EFI_DEVICE_PATH_PROTOCOL *FilePath, IN OUT VOID **ppSource,
// IN OUT UINTN *pSourceSize OPTIONAL, OUT UINTN *pImageSize OPTIONAL,
// OUT EFI_HANDLE *phDevice OPTIONAL,
// OUT EFI_DEVICE_PATH_PROTOCOL **ppPath OPTIONAL)
//
// Input:
//  
//
// Output:
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS ReadImage (
	IN BOOLEAN BootPolicy, IN EFI_DEVICE_PATH_PROTOCOL *FilePath,
	IN OUT VOID **ppSource, IN OUT UINTN *pSourceSize OPTIONAL,
	OUT UINTN *pImageSize OPTIONAL,
	OUT EFI_HANDLE *phDevice OPTIONAL, OUT EFI_DEVICE_PATH_PROTOCOL **ppPath OPTIONAL
)
{
	EFI_HANDLE hDevice = NULL;
	EFI_STATUS Status;
	EFI_DEVICE_PATH_PROTOCOL *pPath = FilePath;
	VOID *pSource = NULL;
	if (!ppSource) return EFI_INVALID_PARAMETER;
	if (!*ppSource)
	{
		UINTN Size;
		Status = pBS->LocateDevicePath(&guidDevicePath, &pPath, &hDevice);
		if (EFI_ERROR(Status))
		{//if device for the FV File is not found
		 //scan all FVs
			UINTN Number,i;
			EFI_HANDLE *Handle;
			if (	pPath->Type != MEDIA_DEVICE_PATH	
				||	pPath->SubType != MEDIA_FV_FILEPATH_DP
			) return EFI_NOT_FOUND;
			Status=pBS->LocateHandleBuffer(ByProtocol, &guidFV, NULL, &Number, &Handle);
			if (EFI_ERROR(Status)) return EFI_NOT_FOUND;
			for(i=0; i<Number; i++)
			{
				Status = FVReadFile(Handle[i], (EFI_GUID*)(pPath+1), &pSource, &Size);
				if (!EFI_ERROR(Status)) break;
			}
			pBS->FreePool(Handle);
			if (EFI_ERROR(Status)) return Status;
		}
		else if (pPath->Type == MEDIA_DEVICE_PATH)
		{
			if(pPath->SubType == MEDIA_FV_FILEPATH_DP)
			{
				if (EFI_ERROR(Status = FVReadFile(hDevice, (EFI_GUID*)(pPath+1), &pSource, &Size))) return Status;
			}
			else if(pPath->SubType == MEDIA_FILEPATH_DP)
			{
				if (EFI_ERROR(Status = FSReadFile(hDevice, pPath, &pSource, &Size))) return Status;
			}
		}
		else
		{
			if (EFI_ERROR(Status = LFReadFile(hDevice, pPath, BootPolicy, &pSource, &Size))) return Status;
		}
		if (pSourceSize) *pSourceSize = Size;
		*ppSource = pSource;
	}
	else
	{
		//EFI Spec does not answer what this should be, however, 
		//	SCT checks for EFI_LOAD_ERROR.
		if (!pSourceSize || !*pSourceSize) return EFI_LOAD_ERROR;
	}
	if (!IsPeImageValid(*ppSource)) return EFI_LOAD_ERROR;
	if (pImageSize) *pImageSize = GetImageSize(*ppSource);
	if (phDevice) *phDevice = hDevice;
	if (ppPath) *ppPath = pPath;
	return EFI_SUCCESS;
}
*/
// =================================== LIST
//List functions

//*************************************************************************
//<AMI_SHDR_START>
//
// Name: DLIST
//
// Description:
//  DLIST is a type defined structure used in an AMILIB provided
// doubly-linked list API to maintain pointers for a list's head, tail, and
// size.
//
// Fields: Name Type Description
// ------------------------------------------------------------------
// Size  UINTN  Number of DLINK nodes contained by the DLIST.
// pHead DLINK* Pointer to the head node (start) of the list.
// pTail DLINK* Pointer to the tail node (end) of the list.
//
// Referrals:
//  Doubly_Linked_List_Functions
//  DLINK
//  DListInit
//  DListEmpty
//  DListAdd
//  DListDelete
//  DListInsert
//  DListEmpty
// 
// Notes:
//  Prototype provided in AmiDxeLib.h.  See Doubly_Linked_List_Functions for
// example usage.
//
//<AMI_SHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_SHDR_START>
//
// Name: DLINK
//
// Description:
//  DLINK is a type defined structure used in an AMILIB provided
// doubly-linked list API which is used as a node for an entry into the list.
//
// Fields: Name Type Description
// ------------------------------------------------------------------
// pNext DLINK* Pointer to the next node in the list.
// pPrev DLINK* Pointer to the previous node in the list.
//
// Referrals:
//  Doubly_Linked_List_Functions
//  DLIST
// 
// Notes:
//  Prototype provided in AmiDxeLib.h.  See Doubly_Linked_List_Functions for
// example usage.
//
//<AMI_SHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DListEmpty
//
// Description:
//  DlistEmpty(IN DLIST *pList) is a macro which returns NULL if the provided
// DLIST is empty.
//
// Input:
//  IN DLIST *pList
// A pointer to the doubly linked list structure.
//
// Output:
//  DLINK* address of the list's head; NULL if the list is empty.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  Macro defined in AmiDxeLib.h.  No validation performed on pList.
//          
//<AMI_PHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DListInit
//
// Description:
//  VOID DListInit(IN DLIST *pList) initializes the head, tail links and
// number of elements of a new linked list structure.  After initializing a
// linked list with this function, the other linked list functions may be
// used to add and remove nodes from the linked list.  It is up to the caller
// of this function to allocate the memory for pList.
//
// Input:
//  IN DLIST *pList
// A pointer to the doubly linked list structure.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  It is up to the caller of this function to allocate the memory for pList.
//          
//<AMI_PHDR_END>
//*************************************************************************

VOID DListInit(DLIST* pList){
	pList->pHead = NULL;
	pList->pTail = NULL;
	pList->Size = 0;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DListAdd
//
// Description:
//  VOID DListAdd(IN DLIST *pList, IN DLINK *pElement) adds the node pElement
// to the end of the doubly linked list denoted by pList.
//
// Input:
//  IN DLIST *pList
// A pointer to the doubly linked list structure.
//
//  IN DLINK *pElement
// A pointer to a node that is to be added at the end of the doubly linked
// list.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID DListAdd(DLIST* pList, DLINK* pElement){
	pElement->pNext = NULL;
	pElement->pPrev = pList->pTail;
	if (!pList->pHead) pList->pHead = pElement;
	else pList->pTail->pNext = pElement;
	pList->pTail = pElement;
	pList->Size++;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DListDelete
//
// Description:
//  VOID DListDelete(IN DLIST *pList, IN DLINK *pElement) removes the node
// pElement from a doubly linked list.  It is up to the caller of this
// function to release the memory used by this node if that is required.
//
// Input:
//  IN DLIST *pList
// A pointer to the doubly linked list structure.
//
//  IN DLINK *pElement
// A pointer to a node that is to be deleted from the end of the doubly
// linked list.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
// 
// Notes:
//  It is up to the caller of this function to release the memory used by
// this node if that is required.
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID DListDelete(DLIST* pList, DLINK* pElement){
	
	if (pList->pTail == pElement) pList->pTail = pElement->pPrev;
	else pElement->pNext->pPrev = pElement->pPrev;
	if (pList->pHead == pElement) pList->pHead = pElement->pNext;
	else pElement->pPrev->pNext = pElement->pNext;
	pList->Size--;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DListInsert
//
// Description:
//  VOID DListInsert(IN DLIST* pList, IN DLINK* pElement, IN DLINK* pAfter)
// inserts the node pElement in the doubly linked list denoted by pList
// immediately after the node pAfter.
//
// Input:
//  IN DLIST *pList
// A pointer to the doubly linked list structure.
//
//  IN DLINK *pElement
// A pointer to a node that is to be added to the doubly linked list.
//
//  IN DLINK *pAfter
// A pointer to a node after which pElement  is to be added.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID DListInsert(DLIST* pList, DLINK* pElement, DLINK* pAfter){
	pElement->pPrev = pAfter;
	if (pAfter)
	{
		pElement->pNext = pAfter->pNext;
        if (pAfter==pList->pTail) pList->pTail=pElement;
		else pAfter->pNext->pPrev = pElement;
		pAfter->pNext = pElement;
	}
	else
	{
		pElement->pNext = pList->pHead;
		if (pList->pHead) pList->pHead->pPrev = pElement;
		else pList->pTail = pElement;
		pList->pHead = pElement;
	}
    pList->Size++;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetEfiVariable
//
// Description:
//  EFI_STATUS GetEfiVariable(IN CHAR16 *sName, IN EFI_GUID *pGuid,
// OUT UINT32 *pAttributes OPTIONAL, IN OUT UINTN *pDataSize,
// OUT VOID **ppData) reads an EFI variable with the name Name and GUID Guid.
// If variable is found, variable attributes, size, and data are returned
// using Attributes, DataSize, and Data parameters and EFI_SUCCESS is
// returned. If variable is not found, EFI_NOT_FOUND is returned. The output
// buffer is specified by a double indirection of the Data parameter. The
// input value of *Data is used to determine if the output buffer is caller
// allocated or is dynamically allocated by this functions. If the input
// value of  *Data is not NULL, it indicates that the output buffer is caller
// allocated. In this case, the input value of *DataSize indicates the size
// of the caller-allocated output buffer. If the output buffer is not large
// enough to contain the variable data, it is freed and new   buffer of
// sufficient size is allocated from boot services pool memory, which will be
// returned in *Data.  If the input value of *Data not NULL, it indicates
// that the output buffer is allocated by GetEfiVariable routine. In this
// case, GetEfiVariable will allocate an appropriately sized buffer from boot
// services pool memory, which will be returned in *Data.
//
// Input:
//  IN CHAR16 *sName
// Pointer to the null-terminated Unicode string with name of the EFI
// Variable to read.
//
//  IN EFI_GUID *pGuid
// Pointer to GUID of the EFI Variable to read.
//
//  OUT UINT32 *pAttributes OPTIONAL
// Pointer to the Attributes of the EFI Variable.
//
//  IN OUT UINTN *pDataSize
// Pointer to the variable data size.
//
//  OUT VOID **ppData
// Pointer to the variable data.
//
// Output:
//  EFI_NOT_FOUND, if variable is not found.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  Malloc
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS GetEfiVariable(
	IN CHAR16 *sName, IN EFI_GUID *pGuid,
	OUT UINT32 *pAttributes OPTIONAL,
	IN OUT UINTN *pDataSize, OUT VOID **ppData
)
{
	EFI_STATUS Status;
	if (!*ppData) *pDataSize=0;
	Status = pRS->GetVariable(sName, pGuid, pAttributes, pDataSize, *ppData);
	if (!EFI_ERROR(Status)) return Status;
	if (Status==EFI_BUFFER_TOO_SMALL)
	{
		if (*ppData) pBS->FreePool(*ppData);
		if (!(*ppData=Malloc(*pDataSize))) return EFI_OUT_OF_RESOURCES;
		Status = pRS->GetVariable(sName, pGuid, pAttributes, pDataSize, *ppData);
	}
	return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ReadImageResource
//
// Description:
//  EFI_STATUS ReadImageResource(IN EFI_HANDLE ImageHandle,
// IN EFI_GUID *pGuid, OUT VOID **ppData, OUT UINTN *pDataSize)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS ReadImageResource(
	EFI_HANDLE ImageHandle, EFI_GUID *pGuid, 
	VOID **ppData, UINTN *pDataSize
)
{
	EFI_STATUS Status;
	EFI_LOADED_IMAGE_PROTOCOL *pImage;
	EFI_FIRMWARE_VOLUME_PROTOCOL *pFV;
	UINTN i=0, DataSize;
	EFI_GUID *pSectionGuid;
	UINT32 Authentication;
	Status=pBS->HandleProtocol(ImageHandle, &gLocalEfiLoadedImageProtocolGuid, &pImage);
	if (EFI_ERROR(Status)) return Status;
	Status=pBS->HandleProtocol(pImage->DeviceHandle, &guidFV, &pFV);
	if (EFI_ERROR(Status)) return Status;
	do{
		pSectionGuid=NULL;
		DataSize=0;
		Status=pFV->ReadSection (
			pFV,&((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)(pImage->FilePath))->FvFileName,  // Build: "NameGuid" changed to FvFileName" as the 
			EFI_SECTION_FREEFORM_SUBTYPE_GUID,i++, &pSectionGuid, &DataSize,            // strcture MEDIA_FW_VOL_FILEPATH_DEVICE_PATH has
			&Authentication                                                             // been changed.
	 	);
		if (!EFI_ERROR(Status)&&!guidcmp(pSectionGuid,pGuid)) break;
		pBS->FreePool(pSectionGuid);
	}while(!EFI_ERROR(Status));
	if (EFI_ERROR(Status)) return Status;
	*ppData = pSectionGuid;
	if (pDataSize) *pDataSize = DataSize;
	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: RegisterProtocolCallback
//
// Description:
//  EFI_STATUS RegisterProtocolCallback(IN EFI_GUID *pProtocol,
// IN EFI_EVENT_NOTIFY NotifyFunction, IN VOID *pNotifyContext,
// OUT EFI_EVENT *pEvent, OUT VOID **ppRegistration) creates a notification
// event and registers that event with protocol instances specified by
// ProtocolGuid.  Every time a protocol of type ProtocolGuid instance is
// installed or reinstalled, the notification function is executed.  This
// function returns EFI_SUCCESS if notification event has been created.
//
// Input:
//  IN EFI_GUID *pProtocol
// Supplies GUID of the protocol upon whose installation the event is fired.
//
//  IN EFI_EVENT_NOTIFY NotifyFunction
// Supplies the function to notify when the event is signaled.
//
//  IN VOID *pNotifyContext
// The context parameter to pass to NotifyFunction.  This is an optional
// parameter and may be NULL.
//
//  OUT EFI_EVENT *pEvent
// Event created event.
//
//  OUT VOID **ppRegistration
// A pointer to a memory location to receive the registration value.  This
// value is passed to LocateHandle() to obtain new handles that have been
// added that support the ProtocolGuid-specified protocol. 
//
// Output:
//  EFI_OUT_OF_RESOURCES,   if space for the notification event could not be
//                         allocated.
//  EFI_INVALID_PARAMETER,  if Protocol, Event, or Registration are NULL.
//  Otherwise, EFI_SUCCESS
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS RegisterProtocolCallback(
	IN EFI_GUID *pProtocol, IN EFI_EVENT_NOTIFY	NotifyFunction,
	IN VOID *pNotifyContext, OUT EFI_EVENT *pEvent,
	OUT VOID **ppRegistration
)
{
	EFI_STATUS Status;
	Status = pBS->CreateEvent(
		EVT_NOTIFY_SIGNAL,TPL_CALLBACK, 
		NotifyFunction, pNotifyContext, pEvent
	);
	if (EFI_ERROR(Status)) return Status;
	return pBS->RegisterProtocolNotify(pProtocol, *pEvent, ppRegistration);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LibGetDxeSvcTbl
//
// Description:
//  EFI_STATUS LibGetDxeSvcTbl(OUT DXE_SERVICES **ppDxe OPTIONAL) locates the
// DXE Service Table in the EFI System Table and returns it in ppDxe. If
// pointer is found, EFI_SUCCESS is returned.
//
// Input:
//  OUT DXE_SERVICES **ppDxe OPTIONAL
// Places the address of the DXE Services Table at *ppDxe, if **ppDxe not 
// equal to NULL.
//
// Output:
//  EFI_INVALID_PARAMETER,  if InitAmiLib() was not called prior to invoking
//                         this function.
//  EFI_NOT_AVAILABLE_YET,  if EFI Configuration Table does not have the DXE
//                         Services Table pointer initialized yet.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  GetEfiConfigurationTable
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS LibGetDxeSvcTbl(DXE_SERVICES **ppDxe OPTIONAL){
	//Check if we have pST pointer initialized.
	if(pST==NULL) return EFI_INVALID_PARAMETER;
	if(pDxe==NULL){
		//Locate Dxe Services Table
		pDxe=(DXE_SERVICES*)GetEfiConfigurationTable(pST,&guidDxeSvcTbl);
		if(pDxe==NULL) return EFI_NOT_AVAILABLE_YET;
	}
	if(ppDxe!=NULL)*ppDxe=pDxe;
	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LibAllocCspResource
//
// Description:
//  EFI_STATUS LibAllocCspResource(IN CSP_RES_ITEM *ResTable,
// IN UINTN ResCount, IN EFI_HANDLE ImgHandle, IN EFI_HANDLE CntrHandle) uses
// GCD services to allocate list of resources specified by ResTable.
//
// Input:
//  IN CSP_RES_ITEM *ResTable
// Table of requested GCD resources.
//
//  IN UINTN ResCount
// Number of elements in the ResTable.
//
//  IN EFI_HANDLE ImgHandle
// Handle of the image to allocate resources to.
//
//  IN EFI_HANDLE CntrHandle
// Handle of the controller to allocate resources to.
//
// Output:
//  EFI_ERROR_CODE, if function fails.
//  Otherwise, EFI_SUCCESS. 
//
// Modified:
//
// Referrals:
//  LibGetDxeSvcTbl
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
#define CACHEABILITY_ATTRIBUTES \
    (EFI_MEMORY_UC|EFI_MEMORY_WC|EFI_MEMORY_WT|EFI_MEMORY_WB|EFI_MEMORY_WP)
EFI_STATUS LibAllocCspResource(CSP_RES_ITEM *ResTable, UINTN ResCount,
							EFI_HANDLE ImgHandle,EFI_HANDLE CntrHandle)
{
	UINTN				i;
	CSP_RES_ITEM		*Res=NULL;
	EFI_STATUS 			Status=0;
	UINT64				top=0, bot=-1, len=0;
	UINTN				uccnt=0, memcnt=0; 	
	EFI_GCD_MEMORY_SPACE_DESCRIPTOR	mdsc;
	EFI_GCD_IO_SPACE_DESCRIPTOR 	idsc;
//--------------------------------
	//Get Dxe Services Table 
	if(pDxe==NULL){
		Status=LibGetDxeSvcTbl(NULL);
		if(EFI_ERROR(Status)){
			TRACE((DEBUG_ERROR, "Dxe Services Table could not be found! Status = %r",Status));
			ASSERT_EFI_ERROR(Status);
			return Status;
		}
	}
	//Add this Space as MMIO to the GCD
	for(i=0; i<ResCount; i++){
		Res=&ResTable[i];
		//if all non existet space, Memory and IO has been converted to MMIO 
		//we don't want to hang on ERROR Status here
		if(Res->Attributes==-1){
			Status=pDxe->AddIoSpace(Res->ResType,Res->ResBase,Res->ResLength);
			TRACE((DEBUG_INFO, "GCD: AddI/O"));
		} else {
			//Some attempt to optimize region caching
			//we will try to group some regions based on their cache requirements.
			memcnt++;
			if(Res->Attributes & EFI_MEMORY_UC) uccnt++;
			if((Res->ResBase+Res->ResLength) > top ) top=Res->ResBase+Res->ResLength;
			if(Res->ResBase < bot) bot=Res->ResBase;
			Status=pDxe->AddMemorySpace(Res->ResType,Res->ResBase,Res->ResLength,GCD_COMMON_MMIO_CAPS);
			TRACE((DEBUG_INFO, "GCD: AddMem"));
		}
		TRACE((DEBUG_INFO, "Space   B=%lX, L=%X, i=%d, S=%r\n",Res->ResBase,Res->ResLength,i,Status));		
		
		//Allocate the Space
		if(Res->Attributes==-1) {
			//Check Gcd IO we are trying to Allocate
			Status=pDxe->GetIoSpaceDescriptor(Res->ResBase, &idsc);
			if(idsc.ImageHandle != NULL) continue;
			//If Space is not allocated but marked as different type of Space - use the one we got
			else if(idsc.GcdIoType!=Res->ResType /*&& 
					idsc.BaseAddress<=Res->ResBase &&
					(idsc.BaseAddress+idsc.Length) >= (Res->ResBase+Res->ResLength)*/
				) Res->ResType=idsc.GcdIoType;
				Status=pDxe->AllocateIoSpace(EfiGcdAllocateAddress,
					   Res->ResType,0, Res->ResLength, &Res->ResBase, ImgHandle, CntrHandle);
				TRACE((DEBUG_INFO, "GCD: AllocI/O"));
		} else { 
			//Check Gcd Memory we are trying to Allocate
			Status=pDxe->GetMemorySpaceDescriptor(Res->ResBase, &mdsc);
			//the resource has been allocated earlier by MRC or trough HOB
			if(mdsc.ImageHandle != NULL) continue;
			//If Space is not allocated but marked as different type of Space - use the one we got
			else if(mdsc.GcdMemoryType!=Res->ResType /*&& 
					mdsc.BaseAddress<=Res->ResBase &&
					(mdsc.BaseAddress+mdsc.Length) >= (Res->ResBase+Res->ResLength)*/
				) Res->ResType=mdsc.GcdMemoryType;
					
			Status=pDxe->AllocateMemorySpace(EfiGcdAllocateAddress,
				   Res->ResType, 0, Res->ResLength, &Res->ResBase, ImgHandle, CntrHandle);
			TRACE((DEBUG_INFO, "GCD: AllocMem"));
		}
		TRACE((DEBUG_INFO, "Space B=%lX, L=%X, i=%d, S=%r\n",Res->ResBase,Res->ResLength,i,Status));
		if(EFI_ERROR(Status)){
			ASSERT_EFI_ERROR(Status);
			return Status;
		}
	} //for loop for allocation

	//this is the case when entire region suppose to be uncached
	if(memcnt && (memcnt==uccnt)){
        EFI_GCD_MEMORY_SPACE_DESCRIPTOR md;
        UINT64                          newb, newt, newl;        
    //----------------------
		//Adjust Caching Attribute Address to fit PAGES 
		len=top-bot;
		if(bot & EFI_PAGE_MASK){
			bot&=(~EFI_PAGE_MASK);
			len = (len&(~EFI_PAGE_MASK))+EFI_PAGE_SIZE;
		} 
		if(len & EFI_PAGE_MASK) len = (len&(~EFI_PAGE_MASK))+EFI_PAGE_SIZE;
        
        //Now we must apply Caching attributes but must be wery careful 
        //not to clean RT attributes if it was set by earlier call
		TRACE((DEBUG_INFO, "GCD: SpaceAttr (UC ALL) B=%lX; L=%lX; \n",bot,top-bot));

        newb=bot;

        for(;;){        
            Status=pDxe->GetMemorySpaceDescriptor(newb,&md);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) break;
            
            newt=md.BaseAddress+md.Length;
            if(newt>=top) newt=top;
            newl=newt-newb;

            if(md.Attributes & EFI_MEMORY_UC){
        		TRACE((DEBUG_INFO, "GCD:(UC ALL) skipping"));
            } else {
                md.Attributes|=EFI_MEMORY_UC;
        		Status=pDxe->SetMemorySpaceAttributes(newb,newl, md.Attributes);
        		TRACE((DEBUG_INFO, "GCD:(UC ALL) setting "));
            }
            TRACE((DEBUG_INFO," B=%lX, L=%lX, A=%lX; S=%r\n",newb,newl,md.Attributes, Status));
            
            if(newt>=top) break;
            newb=newt;
        }
        
	}
    //set cacheability attributes
    for(i=0; i<ResCount; i++){
        //Skip I/O
		if(ResTable[i].Attributes==-1) continue;
		//Set Attributes For this Region.
		Status=pDxe->SetMemorySpaceAttributes(ResTable[i].ResBase,ResTable[i].ResLength,ResTable[i].Attributes);
		TRACE((DEBUG_INFO, "GCD: SpaceAttr A=%lX B=%lX, L=%X, i=%X, S=%r\n",
            ResTable[i].Attributes, ResTable[i].ResBase, ResTable[i].ResLength,i,Status));
        if (EFI_ERROR(Status)){
            //Attempt to set attributes failed;
            //Let's set non-cacheability attributes
            UINT64 attr=ResTable[i].Attributes & ~CACHEABILITY_ATTRIBUTES;
            //If all descriptors are uncacheable add US attribute
            if(memcnt==uccnt) attr|=EFI_MEMORY_UC;
            Status=pDxe->SetMemorySpaceAttributes(ResTable[i].ResBase,ResTable[i].ResLength,  attr);
        }
	}
	return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AllocCspResource
//
// Description:
//  EFI_STATUS AllocCspResource(IN DXE_SERVICES *Dxe,
// IN CSP_RES_ITEM *ResTable, IN UINTN ResCount, IN EFI_HANDLE ImgHandle,
// IN EFI_HANDLE CntrHandle, IN BOOLEAN AddSpace) provides compatibility with
// core 4.5.3; acts as a wrapper for LibAllocCspResource.
//
// Input:
//  IN DXE_SERVICES *Dxe
// Unused.
//
//  IN CSP_RES_ITEM *ResTable
// Table of requested GCD resources.
// 
//  IN UINTN ResCount
// Number of elements in the ResTable.
//
//  IN EFI_HANDLE ImgHandle
// Handle of the image to allocate resources to.
//
//  IN EFI_HANDLE CntrHandle
// Handle of the controller to allocate resources to. 
//
//  IN BOOLEAN AddSpace
// Unused.
//
// Output:
//  EFI_ERROR_CODE, if function fails.
//  Otherwise, EFI_SUCCESS. 
//
// Modified:
//
// Referrals:
//  LibAllocCspResource
// 
// Notes:
//  Use LibAllocCspResource unless you are using core 4.5.3 or older.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS AllocCspResource(DXE_SERVICES *Dxe, CSP_RES_ITEM *ResTable, UINTN ResCount,
							EFI_HANDLE ImgHandle,EFI_HANDLE CntrHandle, BOOLEAN AddSpace)
{
	return LibAllocCspResource(ResTable, ResCount,ImgHandle, CntrHandle);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetSmstConfigurationTable
//
// Description:
//  VOID *GetEfiConfigurationTable( IN EFI_GUID *TableGuid) 
// retrieves a pointer to the system configuration table
// from the SMM System Table based on a specified GUID.  If a match is found,
// then a pointer to the configuration table is returned.  If a matching GUID
// is not found, then NULL is returned.
//
// Input:
//  IN EFI_GUID *TableGuid
// Pointer to table's GUID type.
//
// Output:
//  VOID* pointer to configuration table if a match is found; otherwise NULL.
//
// Referrals:
//  guidcmp
// 
// Notes:	
//  This function only works in SMM. It relies on global initialization performed
//  by InitSmmHandler(or InitSmmHandlerEx) function.
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID* GetSmstConfigurationTable(IN EFI_GUID *TableGuid){
    EFI_CONFIGURATION_TABLE	*Table;
    UINTN i;

    ASSERT(pSmst!=NULL); //pSmst must be initialized with the call to InitSmiHandler
    if (pSmst==NULL) return NULL;
	Table = pSmst->SmmConfigurationTable;
	i = pSmst->NumberOfTableEntries;

	for (;i;--i,++Table)
	{
		if (guidcmp(&Table->VendorGuid,TableGuid)==0) 
            return Table->VendorTable;
	}
	return NULL;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: InitSmmHandler
//
// Description:
//  EFI_STATUS InitSmmHandler(IN EFI_HANDLE ImageHandle,
// IN EFI_SYSTEM_TABLE *SystemTable,
// IN EFI_STATUS (*InSmmFunction)(EFI_HANDLE ImageHandle,
// EFI_SYSTEM_TABLE *SystemTable),
// IN EFI_STATUS (*NotInSmmFunction)(EFI_HANDLE ImageHandle,
// EFI_SYSTEM_TABLE *SystemTable) OPTIONAL) first calls the provided
// NotInSmmFunction (if provided), then runs the InSmmFunction in SMM.
//
// Input:
//  IN EFI_HANDLE ImageHandle
// This driver's image handle.
//
//  IN EFI_SYSTEM_TABLE *SystemTable
// Pointer to the system table.
// 
//  IN EFI_STATUS (*InSmmFunction)(EFI_HANDLE ImageHandle EFI_SYSTEM_TABLE *SystemTable)
// Pointer to the function to be run inside SMM.
//
//  IN EFI_STATUS (*NotInSmmFunction)(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) OPTIONAL
// Pointer to the function to be run outside SMM.
//
// Output:
//  EFI_NOT_FOUND, if the SMM Base Protocol is not installed.
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
EFI_STATUS InitSmmHandler(
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable,
	IN EFI_STATUS		(*InSmmFunction)(EFI_HANDLE	ImageHandle, EFI_SYSTEM_TABLE *SystemTable),
	IN EFI_STATUS		(*NotInSmmFunction)(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) OPTIONAL
)
{
	BOOLEAN						InSmm = FALSE;
	EFI_STATUS					Status;
    VOID                        *p;

    InitAmiLib(ImageHandle,SystemTable);
	Status = pBS->LocateProtocol(&gLocalEfiSmmBase2ProtocolGuid, NULL, &pSmmBase);
	if (!EFI_ERROR(Status)) pSmmBase->InSmm(pSmmBase, &InSmm);
  
	if (EFI_ERROR(Status) || !InSmm) {
		//First do non InSmm initialization.
		if (NotInSmmFunction) {
			Status = NotInSmmFunction(ImageHandle,SystemTable);
		}
		return Status;
	}

	Status = pSmmBase->GetSmstLocation(pSmmBase,&pSmst);
	if (EFI_ERROR(Status)) return Status;

    p = GetSmstConfigurationTable(&SmmRsTableGuid);
    if (p!=NULL) pRS = p;
    IsInSmm = TRUE;
    IsAtRuntime = FALSE;
    LibInitStatusCodePtr();
    Status = TraceGetAmiDebugServicesProtocolInstance();
    IsAtRuntime = TRUE;

    return InSmmFunction(ImageHandle,SystemTable);
}


EFI_STATUS InitAmiSmmLib( 
	IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE	*SystemTable)
{
	BOOLEAN						InSmm;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	EFI_STATUS					Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    VOID                        *p;


    InitAmiLib(ImageHandle,SystemTable);
    if(pSmmBase==NULL){
    	Status = pBS->LocateProtocol(&gLocalEfiSmmBase2ProtocolGuid, NULL, &pSmmBase);
	    if (EFI_ERROR(Status)) return Status;
    }

   	pSmmBase->InSmm(pSmmBase, &InSmm);

    if(InSmm){
    	Status = pSmmBase->GetSmstLocation(pSmmBase,&pSmst);
	    if (EFI_ERROR(Status)) return Status;

        p = GetSmstConfigurationTable(&SmmRsTableGuid);
        if (p!=NULL) pRS = p;
        IsInSmm = TRUE;
        IsAtRuntime = FALSE;
        LibInitStatusCodePtr();
        Status = TraceGetAmiDebugServicesProtocolInstance();
        IsAtRuntime = TRUE;
    }
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DummyFunction
//
// Description:
//  VOID DummyFunction(IN EFI_EVENT Event, IN VOID *Context) is an empty
// VOID function which does nothing.
//
// Input:
//  IN EFI_EVENT Event
// Unused.
//
//  IN VOID *Context
// Unused.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID DummyFunction(IN EFI_EVENT Event, IN VOID *Context) {}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CreateReadyToBootEvent
//
// Description:
//  EFI_STATUS CreateReadyToBootEvent(IN EFI_TPL NotifyTpl,
// IN EFI_EVENT_NOTIFY NotifyFunction, IN VOID *pNotifyContext,
// OUT EFI_EVENT *pEvent) creates an EFI event in the Ready To Boot Event
// Group and allows the caller to specify a notification function.  Prior to
// UEFI 2.0 this was done via a non-standard UEFI extension, and this library
// abstracts the implementation mechanism of this event from the caller.
//
// Input:
//  IN EFI_TPL NotifyTpl
// The task priority level of the event.
//
//  IN EFI_EVENT_NOTIFY NotifyFunction
// The notification function to call when the event is signaled.
//
//  IN VOID *pNotifyContext
// The content to pass to NotifyFunction when the event is signaled.
//
//  OUT EFI_EVENT *pEvent
// Return the event that was created.
//
// Output:
//  EFI_INVALID_PARAMETER,  if the input parameters are incorrect.
//  EFI_OUT_OF_RESOURCES,   if the event could not be allocated.
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
EFI_STATUS CreateReadyToBootEvent(
    IN EFI_TPL NotifyTpl, IN EFI_EVENT_NOTIFY	NotifyFunction,
	IN VOID *pNotifyContext, OUT EFI_EVENT *pEvent
)
{
    static EFI_GUID guidReadyToBoot = EFI_EVENT_GROUP_READY_TO_BOOT;
    return pBS->CreateEventEx(
        EVT_NOTIFY_SIGNAL, NotifyTpl, 
        (NotifyFunction) ? NotifyFunction : DummyFunction,
        pNotifyContext, &guidReadyToBoot,
        pEvent
    );
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CreateLegacyBootEvent
//
// Description:
//  EFI_STATUS CreateLegacyBootEvent(IN EFI_TPL NotifyTpl,
// IN EFI_EVENT_NOTIFY NotifyFunction, IN VOID *pNotifyContext,
// OUT EFI_EVENT *pEvent) creates an EFI event in the Legacy Boot Event Group
// and allows the caller to specify a notification function.  Prior to
// UEFI 2.0 this was done via a non-standard UEFI extension, and this library
// abstracts the implementation mechanism of this event from the caller.
//
// Input:
//  IN EFI_TPL NotifyTpl
// The task priority level of the event.
//
//  IN EFI_EVENT_NOTIFY NotifyFunction
// The notification function to call when the event is signaled.
//
//  IN VOID *pNotifyContext
// The content to pass to NotifyFunction when the event is signaled.
//
//  OUT EFI_EVENT *pEvent
// Return the event that was created 
//
// Output:
//  EFI_INVALID_PARAMETER,  if the input parameters are incorrect.
//  EFI_OUT_OF_RESOURCES,   if the event could not be allocated.
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
EFI_STATUS CreateLegacyBootEvent(
    IN EFI_TPL NotifyTpl, IN EFI_EVENT_NOTIFY	NotifyFunction,
	IN VOID *pNotifyContext, OUT EFI_EVENT *pEvent
)
{
    static EFI_GUID guidLegacyBoot = EFI_EVENT_LEGACY_BOOT_GUID;
    return pBS->CreateEventEx(
        EVT_NOTIFY_SIGNAL, NotifyTpl,
        (NotifyFunction) ? NotifyFunction : DummyFunction,
        pNotifyContext, &guidLegacyBoot,
        pEvent
    );
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetBootMode
//
// Description:
//  EFI_BOOT_MODE GetBootMode() returns current boot mode. If no boot mode
// information is available, BOOT_WITH_FULL_CONFIGURATION is returned.
//
// Input:
//  VOID.
//
// Output:
//  BOOT_WITH_FULL_CONFIGURATION,  if no boot mode information is available.
//  Otherwise, the current EFI_BOOT_MODE.
//
// Modified:
//
// Referrals:
//  GetEfiConfigurationTable
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_BOOT_MODE GetBootMode()
{
	static EFI_GUID guidHob = HOB_LIST_GUID;
	EFI_HOB_HANDOFF_INFO_TABLE *pHit;
	//Get Boot Mode
	pHit = GetEfiConfigurationTable(pST, &guidHob);
    return (pHit) ? pHit->BootMode : BOOT_WITH_FULL_CONFIGURATION;
}


static EFI_FPDT_STRUCTURE          *FpdtVar = NULL;
//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AmiFillFpdt
//
// Description:
//  VOID AmiFillFpdt () Finds address of Bas Boot Perf Record  and
// writes performance data into it, based on inpoot parameter.  
//
// Input:
//  FPDT_FILL_TYPE FieldToFill - Indicator which field in Bas Boot Perf Record to fill
//
// Output:
//  VOID.
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID AmiFillFpdt (IN FPDT_FILL_TYPE FieldToFill){
    BASIC_BOOT_PERF_REC         *BasBootPerRec;
    EFI_STATUS                  StatusFpdt;
    UINTN                       VarSize = sizeof(UINT32);

    if ((FpdtVar == NULL) || (FpdtVar->NanoFreq == 0) || (FpdtVar->BasBootPointer == 0)){
        StatusFpdt = pRS->GetVariable(
            EFI_FPDT_VARIABLE, &gAmiGlobalVariableGuid,
		    NULL, &VarSize, &FpdtVar
            ); 
        if (EFI_ERROR (StatusFpdt) || (FpdtVar->NanoFreq == 0) || (FpdtVar->BasBootPointer == 0)) 
            return;
    }
    if (((PERF_TAB_HEADER*)(UINT8*)(FpdtVar->BasBootPointer))->Signature != 0x54504246)
        return;
    BasBootPerRec = (BASIC_BOOT_PERF_REC*)((UINT8*)(FpdtVar->BasBootPointer) + sizeof(PERF_TAB_HEADER));
    if (BasBootPerRec->Header.PerfRecType != 2) return;
    switch (FieldToFill)
    {
        case FillOsLoaderLoadImageStart:
        
            BasBootPerRec->OsLoaderLoadImageStart = Div64 (Mul64 (GetCpuTimer (), (UINTN)FpdtVar->NanoFreq), 1000000 , NULL);
            break;
        
        case FillOsLoaderStartImageStart:
        
            BasBootPerRec->OsLoaderStartImageStart = Div64 (Mul64 (GetCpuTimer (), (UINTN)FpdtVar->NanoFreq), 1000000 , NULL);
            break;
        
        case FillExitBootServicesEntry:
        
            BasBootPerRec->ExitBootServicesEntry = Div64 (Mul64 (GetCpuTimer (), (UINTN)FpdtVar->NanoFreq), 1000000 , NULL);
            break;
        
        case FillExitBootServicesExit:
        
            BasBootPerRec->ExitBootServicesExit = Div64 (Mul64 (GetCpuTimer (), (UINTN)FpdtVar->NanoFreq), 1000000 , NULL);
            break;
        
    }

}




static INT32 NumberOfGoVirtualCallbacks=0;
static INT32 NumberOfExitBsCallbacks=0;
static EFI_RUNTIME_SERVICES *LibTempCopyOfpRs=NULL;

static VOID AmiLibExitBs (IN EFI_EVENT Event, IN VOID *Context){
    IsAtRuntime=TRUE;
    if (Context!=NULL) ((EFI_EVENT_NOTIFY)Context)(Event,NULL);
    if (--NumberOfExitBsCallbacks==0){
        pBS = NULL;
        LibTempCopyOfpRs = pRS;
    }
}

static VOID AmiLibGoVirtual (IN EFI_EVENT Event, IN VOID *Context){
    IsInVirtualMode=TRUE;
    if (Context!=NULL) ((EFI_EVENT_NOTIFY)Context)(Event,NULL);
    if (--NumberOfGoVirtualCallbacks==0){
        // at this point it is not safe to use pRS
        // because it may have been already converted to the virtual address
        // by the library consumer.
        LibTempCopyOfpRs->ConvertPointer(0,&pST);
        LibTempCopyOfpRs->ConvertPointer(0,&pRS);
        if (StatusCodePtr != NULL) LibTempCopyOfpRs->ConvertPointer(0,&StatusCodePtr);
        if (AmiDebugServiceProtocol != NULL) LibTempCopyOfpRs->ConvertPointer(0,&AmiDebugServiceProtocol);
    }
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: InitAmiRuntimeLib
//
// Description:	
//  VOID InitAmiRuntimeLib(IN EFI_HANDLE ImageHandle,
// IN EFI_SYSTEM_TABLE *SystemTable, IN EFI_EVENT_NOTIFY ExitBsCallback OPTIONAL,
// IN EFI_EVENT_NOTIFY GoVirtualCallback OPTIONAL) initializes the global variables using call to InitAmiLib.
// Runtime drivers should use this function instead of InitAmiLib.
//
// Input:
//  IN EFI_HANDLE ImageHandle
// The image handle.
//
//  IN EFI_SYSTEM_TABLE *SystemTable
// Pointer to the EFI System Table.
//
//  IN EFI_EVENT_NOTIFY ExitBsCallback OPTIONAL
//      Optional Exit Boot Services callback function pointer
//
//  IN EFI_EVENT_NOTIFY GoVirtualCallback OPTIONAL
//      Optional Virtual Address Change callback function pointer
//
// Output:
//  VOID.
//
// Modified:
//  pST
//  pBS
//  pRS
//  TheImageHandle
//
// Referrals:
// 
// Notes:	
//  The global variables initialized by this function are:
//   pST             Pointer to the EFI System Table.
//   pBS             Pointer to the Boot Services Table.
//   pRS             Pointer to the Runtime Services Table.
//   TheImageHandle  Image handle.
//
//<AMI_PHDR_END>
//*************************************************************************
VOID EFIAPI InitAmiRuntimeLib(
    IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable,
    IN EFI_EVENT_NOTIFY ExitBsCallback OPTIONAL,
    IN EFI_EVENT_NOTIFY GoVirtualCallback OPTIONAL
){
    EFI_EVENT Event;
    InitAmiLib(ImageHandle,SystemTable);
    LibInitStatusCodePtr();
    TraceGetAmiDebugServicesProtocolInstance();
    NumberOfExitBsCallbacks++;
    NumberOfGoVirtualCallbacks++;
	pBS->CreateEvent(
        EVT_SIGNAL_EXIT_BOOT_SERVICES,TPL_CALLBACK, 
        &AmiLibExitBs, ExitBsCallback, &Event
    );
    pBS->CreateEvent(
        EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE,TPL_CALLBACK, 
        &AmiLibGoVirtual, GoVirtualCallback, &Event
    );
}



//*************************************************************************
//<AMI_PHDR_START>
//
// Name: StrcmpNoCase
//
// Description:	
//  Function compares two CHAR8 strings ignoring case for letters
//
// Input:
//  CHAR8 *Str1 - pointer to first string
//  CHAR8 *Str2 - pointer to second string
//
// Output:
//  BOOLEAN TRUE - strings are equal (en-us is equal to en-US)
//          FALSE - strings are not equal
//
// Referrals:
//   None
//
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN StrcmpNoCase(CHAR8 *Str1, CHAR8 *Str2)
{
    CHAR8 Char1;
    CHAR8 Char2;
    while (*Str1) {
        if(*Str1 != *Str2) {
            Char1 = (*Str1 >= 0x41 && *Str1 <= 0x5a) ? *Str1 : (*Str1 >= 0x61 && *Str1 <= 0x7a) ? *Str1 - 0x20 : 0xff;
            Char2 = (*Str2 >= 0x41 && *Str2 <= 0x5a) ? *Str2 : (*Str2 >= 0x61 && *Str2 <= 0x7a) ? *Str2 - 0x20 : 0;
            if(Char1 != Char2)
                return FALSE;
        }
        Str1++;
        Str2++;
    }
    return (*Str1 == *Str2);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LanguageCodesEqual
//
// Description:	
//  BOOLEAN LanguageCodesEqual(CONST CHAR8* LangCode1, CONST CHAR8* LangCode2) 
// compares two language codes and returns TRUE if they are equal.
// Case is ignored (RFC 4646 requirement)
//
// Input:
//  CONST CHAR8* LangCode1 - first language code
//  CONST CHAR8* LangCode2 - second language code
//
// Output:
//  BOOLEAN TRUE - the language codes are equal
//          FALSE - the language codes are not equal
//
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN LanguageCodesEqual(CONST CHAR8* LangCode1, CONST CHAR8* LangCode2) {
    return StrcmpNoCase((CHAR8*)LangCode1, (CHAR8*)LangCode2);
}

VOID* _GetBootScriptSaveInterface(){
    static VOID* BootScriptSaveInterface = NULL;
    static EFI_GUID gEfiBootScriptSaveInterfaceGuid = EFI_BOOT_SCRIPT_SAVE_GUID;
    EFI_STATUS Status;

    if (BootScriptSaveInterface != NULL) return BootScriptSaveInterface;
    ASSERT(pBS!=NULL);
    Status = pBS->LocateProtocol(
        &gEfiBootScriptSaveInterfaceGuid,NULL,&BootScriptSaveInterface
    );
    if (EFI_ERROR(Status)) BootScriptSaveInterface = NULL;
    return BootScriptSaveInterface;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
