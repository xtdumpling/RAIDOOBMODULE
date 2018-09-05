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
  
*/
#ifndef __AMI_PEI_LIB__H__
#define __AMI_PEI_LIB__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <AmiPpi.h>
#include <Pei.h>
#include <Hob.h>
#include <AmiLib.h>
#include <Library/PeiServicesTablePointerLib.h>
//****************************************************************************/
//		TYPE DECLARATIONS
//****************************************************************************/

//****************************************************************************/
//		MACOROS DECLARATIONS
//****************************************************************************/
#define FVSECTION_SIZE(pSection) ((*(UINT32*)&((EFI_COMMON_SECTION_HEADER*)(pSection)-1)->Size[0] & 0xffffff) - sizeof(EFI_COMMON_SECTION_HEADER))
#define FVFILE_SIZE(pFile) (*(UINT32*)&(pFile)->Size[0]&0xffffff)

#ifdef EFI_DEBUG
#define PEI_TRACE(Arguments) PeiTrace Arguments

#ifndef PEI_ASSERT
#define PEI_ASSERT(PeiServices,Condition) if(!(Condition)) { \
	PeiTrace(-1,PeiServices,"ASSERT in %s on %i: %s\n",__FILE__, __LINE__, #Condition);\
	EFI_DEADLOOP() \
	}
#define ASSERT_PEI_ERROR(PeiServices,Status)  PEI_ASSERT(PeiServices,!EFI_ERROR(Status))
#endif

#define PEI_VERIFY PEI_ASSERT
#define VERIFY_PEI_ERROR ASSERT_EFI_ERROR

#else // EFI_DEBUG==0
#define PEI_TRACE(Arguments)

#ifndef PEI_ASSERT
#define PEI_ASSERT(PeiServices,Condition)
#define ASSERT_PEI_ERROR(PeiServices,Status)
#endif

#define PEI_VERIFY(x) (x)
#define VERIFY_PEI_ERROR(x) (x)

#endif

#define PEI_PROGRESS_CODE(PeiServices, Code)\
	(*PeiServices)->ReportStatusCode(PeiServices, EFI_PROGRESS_CODE, Code, 0, NULL, NULL)

#define PEI_ERROR_CODE(PeiServices, Code, Severity)\
	(*PeiServices)->ReportStatusCode(\
        (CONST EFI_PEI_SERVICES**)PeiServices, EFI_ERROR_CODE|(Severity),\
        Code, 0, NULL, NULL\
    )

#ifdef EFI_PEI_PERFORMANCE
#define PEI_PERF_START(Ps, Token, FileHeader, Value)  PeiPerfMeasure (Ps, Token, FileHeader, FALSE, Value)
#define PEI_PERF_END(Ps, Token, FileHeader, Value)    PeiPerfMeasure (Ps, Token, FileHeader, TRUE, Value)
#define PEI_PERF_SAVE_S3_DATA(Ps) SaveS3PerformanceData(Ps)
#else
#define PEI_PERF_START(Ps, Token, FileHeader, Value)
#define PEI_PERF_END(Ps, Token, FileHeader, Value)
#define PEI_PERF_SAVE_S3_DATA(Ps) 
#endif

//****************************************************************************/
//		VARIABLE DECLARATIONS
//****************************************************************************/

//****************************************************************************/
//		FUNCTION DECLARATIONS
//****************************************************************************/
VOID* PEEntryPoint(VOID *p);
VOID* TEEntryPoint(VOID *p);
UINT32 GetImageSize(VOID *p);

VOID PeiTrace(UINTN Level, CONST EFI_PEI_SERVICES **ppPS, CHAR8 *sFormat,...);
VOID PeiReportErrorCode(EFI_PEI_SERVICES **ppPS, UINT16 Code, UINT32 Action);

EFI_STATUS CreateHobMemoryAllocationModule(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_PHYSICAL_ADDRESS MemoryBaseAddress,
    IN UINT64 MemoryLength, IN EFI_MEMORY_TYPE MemoryType,
    IN EFI_GUID *ModuleName,
    IN EFI_PHYSICAL_ADDRESS EntryPoint
);
EFI_STATUS CreateHobResourceDescriptor
(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_GUID *Owner, IN EFI_RESOURCE_TYPE ResourceType,
    IN EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute,
    IN EFI_PHYSICAL_ADDRESS PhysicalStart,
    IN UINT64 ResourceLength
);
EFI_STATUS CreateLoadHob(
	IN CONST EFI_PEI_SERVICES **ppPS, IN EFI_FFS_FILE_HEADER *pFfsHeader,
	EFI_PEIM_ENTRY_POINT EntryPoint, EFI_PEIM_ENTRY_POINT InMemEntryPoint
);

EFI_STATUS PeiGetVariable(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VariableGuid,
    OUT UINT32 *Attributes, 
    IN OUT UINTN *DataSize, OUT VOID *Data
);
EFI_STATUS PeiGetNextVariableName(
	IN CONST EFI_PEI_SERVICES **PeiServices,
    IN OUT UINTN *VariableNameSize, IN OUT CHAR16 *VariableName,
    IN OUT EFI_GUID *VariableGuid
);

VOID* FindFfsFileByName(
    IN CONST EFI_PEI_SERVICES **PeiServices, EFI_FV_FILETYPE FileType, 
    EFI_GUID *FileName
);

VOID PeiPerfMeasure (
  CONST EFI_PEI_SERVICES **PeiServices,
  IN CHAR16 *Token, IN EFI_FFS_FILE_HEADER *FileHeader,
  IN BOOLEAN EntryExit, IN UINT64 Value
);

VOID SaveS3PerformanceData(IN CONST EFI_PEI_SERVICES **PeiServices);

EFI_STATUS PeiLoadFile(
    IN CONST EFI_PEI_SERVICES ** PeiServices,
    IN  EFI_PEI_FILE_HANDLE            FileHandle,
    OUT EFI_PHYSICAL_ADDRESS           *ImageAddress,
    OUT UINT64                         *ImageSize,
    OUT EFI_PHYSICAL_ADDRESS           *EntryPoint
);

EFI_STATUS PciCfgModify(
    IN CONST EFI_PEI_SERVICES ** PeiServices,
	IN EFI_PEI_PCI_CFG_PPI_WIDTH		Width,
	IN UINT64							Address,
	IN UINTN							SetBits,
	IN UINTN							ClearBits
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
