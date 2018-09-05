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
/** @file NVRAM.h
    NVRAM Definitions

**/
//**********************************************************************
#ifndef __NVRAM__H__
#define __NVRAM__H__

#include <AmiLib.h>
#include <Flash.h>
#include <Hob.h>
#include <Ffs.h>
#include <Guid/AmiNvram.h>

#define FLASH_EMPTY_BYTE ((UINT8)FlashEmpty)
#define FLASH_EMPTY_FLAG FLASH_EMPTY_BYTE
#define FLASH_EMPTY_NEXT FlashEmptyNext
#define FLASH_EMPTY_SIGNATURE ((UINT32)FlashEmpty)
#define FLASH_EMPTY_SIZE ((VAR_SIZE_TYPE)FlashEmpty)

#define NVRAM_FLAG_VALID 0x80
#define NVRAM_FLAG_RUNTIME 1
#define NVRAM_FLAG_ASCII_NAME 2
#define NVRAM_FLAG_GUID 4
#define NVRAM_FLAG_DATA_ONLY 8
#define NVRAM_FLAG_EXT_HEDER 0x10
#define NVRAM_FLAG_AUTH_WRITE 0x40
#define NVRAM_FLAG_HARDWARE_ERROR_RECORD 0x20
#define UEFI21_SPECIFIC_NVRAM_FLAGS (NVRAM_FLAG_HARDWARE_ERROR_RECORD | NVRAM_FLAG_AUTH_WRITE)
#define UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES \
                (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)

#define NVRAM_EXT_FLAG_CHECKSUM 1

#define NVRAM_eFLAG_AUTH_WRITE              EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS //0x10
#define NVRAM_eFLAG_TIME_BASED_AUTH_WRITE   EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS //0x20

#define ALL_FLAGS (NVRAM_FLAG_VALID | NVRAM_FLAG_RUNTIME |\
				   NVRAM_FLAG_ASCII_NAME | NVRAM_FLAG_GUID|\
				   NVRAM_FLAG_DATA_ONLY | NVRAM_FLAG_EXT_HEDER|\
                   UEFI21_SPECIFIC_NVRAM_FLAGS\
                  )

#define NVAR_SIGNATURE ('N'+('V'<<8)+(('A'+('R'<<8))<<16))//NVAR

#define NVRAM_HOB_GUID \
    { 0xc0ec00fd, 0xc2f8, 0x4e47, { 0x90, 0xef, 0x9c, 0x81, 0x55, 0x28, 0x5b, 0xec } }

#define NVRAM_MODE_MANUFACTORING 1
#define NVRAM_MODE_RESET_CONFIGURATION 2
#define NVRAM_MODE_DEFAULT_CONFIGURATION 4
#define NVRAM_MODE_SIMULATION 8

#define NVRAM_STORE_FLAG_NON_VALATILE 1
#define NVRAM_STORE_FLAG_READ_ONLY 2
#define NVRAM_STORE_FLAG_DO_NOT_ENUMERATE 4

#define MAX_NVRAM_STORE_HEADER_SIZE 0x100

extern const UINTN FlashEmpty;
extern const UINT32 FlashEmptyNext;
extern const EFI_GUID AmiDefaultsVariableGuid;
extern const EFI_GUID AmiNvramHobGuid;
extern const CHAR16 MfgDefaults[];
extern const CHAR16 StdDefaults[];

typedef UINT16 VAR_SIZE_TYPE;
#define NEXT_OFFSET (EFI_FIELD_OFFSET(NVAR,size)+sizeof(VAR_SIZE_TYPE))
#define NEXT_SIZE 3
#define FLAG_SIZE 1
#define FLAG_OFFSET (NEXT_OFFSET + NEXT_SIZE)
#define MAX_NVRAM_VARIABLE_SIZE ((1<<(sizeof(VAR_SIZE_TYPE)<<3))-1)

#define NvSignature(Var) ReadUnaligned32(&(Var)->signature)
#define NvSize(Var) ReadUnaligned16(&(Var)->size)
#define NvExtSize(Var) ReadUnaligned16(NvGetExtSize(Var))
#define NvNextAddress(Var) ( (UINT32*)( (UINT8*)(Var)+sizeof(NVAR)-sizeof(UINT32) ) )
#define NvNext(Var) (ReadUnaligned32(NvNextAddress(Var)) & 0xFFFFFF)

#define SetNvSignature(Var) WriteUnaligned32(&(Var)->signature,NVAR_SIGNATURE)
#define SetNvSize(Var,Value) WriteUnaligned16(&(Var)->size,Value)
#define SetNvExtSize(Var,Value) WriteUnaligned16(NvGetExtSize(Var),Value)
#define SetNvNext(Var,Value) WriteUnaligned32(NvNextAddress(Var),(Value)|((Var)->flags<<24))

#define VALID_STATE_FLAGS (\
    EFI_FILE_HEADER_CONSTRUCTION | EFI_FILE_HEADER_VALID |\
    EFI_FILE_DATA_VALID | EFI_FILE_MARKED_FOR_UPDATE | EFI_FILE_DELETED)

// The corrupted store has been successfully repaired
#define NVRAM_WARN_STORE_REPAIRED EFI_WARN_WRITE_FAILURE
// The store is healthy, but it out of synch with the store memory buffer
#define NVRAM_WARN_MEDIA_CHANGED EFI_WARN_STALE_DATA

#pragma pack(push)
#pragma pack(1)
typedef struct{
	UINT32 signature;
	VAR_SIZE_TYPE size;
	UINT32 next:24;
	UINT32 flags:8;
//  guid and name are there only if NVRAM_FLAG_DATA_ONLY is not set
//	UINT8 guid; (or EFI_GUID guid; if NVRAM_FLAG_GUID is set)
//	CHAR8 or CHAR16 name[...]; (if NVRAM_FLAG_ASCII_NAME is set, CHAR8; otherwise CHAR16)
//  UINT8 data[...];
//  if NVRAM_FLAG_EXT_HEDER is set
//  UINT8 extflags;
//  UINT8 extdata[extisze - sizeof(extsize) - sizeof(extflags)];
//  VAR_SIZE_TYPE extsize;
// =====
//	extdata format:
//	UINT64 MonotonicCounter; // only if NVRAM_FLAG_AUTH_WRITE is set
//	UINT8 KeyHash[32]; // only if NVRAM_FLAG_AUTH_WRITE is set
//  UINT8 CheckSum; // only if NvramChecksumSupport is TRUE
}NVAR;
#pragma pack(pop)

typedef struct {
    EFI_HOB_GUID_TYPE Header;
    EFI_PHYSICAL_ADDRESS NvramAddress;
    EFI_PHYSICAL_ADDRESS BackupAddress;
    UINT32 NvramSize;
    UINT32 HeaderLength;
    UINT32 NvramMode;
} NVRAM_HOB;

typedef struct _NVRAM_STORE_INTERFACE NVRAM_STORE_INTERFACE;

typedef struct {
    UINT8 *NvramAddress;
    UINTN NvramSize;
    EFI_GUID* NvramGuidsAddress;
    UINT8 *pEndOfVars, *pFirstVar;
    INT16 NextGuid;
    VAR_SIZE_TYPE LastVarSize;
    NVAR *pLastReturned;
    UINT8 Flags;
    UINT32 HeaderLength;
    NVRAM_STORE_INTERFACE *Interface;
} NVRAM_STORE_INFO;

typedef BOOLEAN (*NVRAM_STORE_UPDATE_ROUTINE)(
    IN NVRAM_STORE_INTERFACE *This, IN VOID *Address, UINTN Size
);
typedef BOOLEAN (*NVRAM_STORE_PROGRAM_ROUTINE)(
    IN NVRAM_STORE_INTERFACE *This, IN VOID *Address, UINTN Size, VOID *Data
);

typedef EFI_STATUS (*NVRAM_STORE_BEGIN_GARBAGE_COLLECTION)(
    IN NVRAM_STORE_INTERFACE *This, VOID* Address, UINTN Size
);
typedef EFI_STATUS (*NVRAM_STORE_END_GARBAGE_COLLECTION)(
    IN NVRAM_STORE_INTERFACE *This, BOOLEAN ErrorsFound, VOID* Address
);
typedef EFI_STATUS (*NVRAM_STORE_VIRTUAL_FIXUP)(
    IN NVRAM_STORE_INTERFACE *This, UINT8 *BufferAddress
);
typedef EFI_STATUS (*NVRAM_STORE_CHECK_STORE)(
    IN NVRAM_STORE_INTERFACE *This, UINT8 *BufferAddress
);
typedef EFI_STATUS (*NVRAM_STORE_REINITIALIZE)(
    IN NVRAM_STORE_INTERFACE *This, IN UINT8 *Buffer, IN UINTN Size, IN BOOLEAN ReadOnly, OUT UINT32 *HeaderLength
);

struct _NVRAM_STORE_INTERFACE {
    NVRAM_STORE_UPDATE_ROUTINE EnableUpdate;
    NVRAM_STORE_UPDATE_ROUTINE DisableUpdate;
    NVRAM_STORE_UPDATE_ROUTINE EraseBytes;
    NVRAM_STORE_PROGRAM_ROUTINE WriteBytes;
    NVRAM_STORE_BEGIN_GARBAGE_COLLECTION BeginGarbageCollection;
    NVRAM_STORE_END_GARBAGE_COLLECTION EndGarbageCollection;
    NVRAM_STORE_VIRTUAL_FIXUP VirtualFixup;
    NVRAM_STORE_CHECK_STORE   CheckStore;
    NVRAM_STORE_REINITIALIZE Reinitialize;
};

//Low level access routines
EFI_GUID* NvGetGuid(NVAR* pVar, NVRAM_STORE_INFO *pInfo);
VOID* NvGetName(NVAR* pVar);
NVAR* NvGetDataNvar(NVAR *pVar, NVRAM_STORE_INFO *pInfo);
VOID* NvGetData(NVAR* pVar, UINTN NameLength, UINTN* pDataSize, NVRAM_STORE_INFO *pInfo);
VAR_SIZE_TYPE* NvGetExtSize (NVAR* pVar);
UINT8* NvGetExtFlags (NVAR* pVar);


//Validation routines
BOOLEAN NvIsVariable(NVAR *pVar, NVRAM_STORE_INFO *pInfo);
BOOLEAN NvIsValid(NVAR* pVar);

//Iteration routines
NVAR* NvGetNextNvar(NVAR* pVar, NVRAM_STORE_INFO *pInfo);
NVAR* NvGetNextValid(NVAR* pVar, NVRAM_STORE_INFO *pInfo);

//Comparison routines
BOOLEAN NvAttribEq(NVAR* pNvar, UINT32 Attributes, NVRAM_STORE_INFO *pInfo);
BOOLEAN NvVarEq(NVAR* pNvar, CHAR16* sName, EFI_GUID* pGuid, UINTN* pNameSize, NVRAM_STORE_INFO *pInfo);
BOOLEAN NvarEqNvar(NVAR *Nvar1, NVRAM_STORE_INFO *Info1, NVAR *Nvar2, NVRAM_STORE_INFO *Info2);

//High level routines that work with a single NV store
VOID* NvFindVariable(CHAR16* sName, EFI_GUID* pGuid, UINTN* pNameSize, NVRAM_STORE_INFO *pInfo);
EFI_STATUS NvGetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data, 
    NVRAM_STORE_INFO *pInfo,  OUT NVAR **Var OPTIONAL
);
EFI_STATUS NvGetNextVariableName(
	IN OUT UINTN *VariableNameSize,
	IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid,
    NVRAM_STORE_INFO *pInfo, BOOLEAN Runtime
);

//High level routines that work with a multiple NV stores
EFI_STATUS NvGetVariable2(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data,
    UINT32 InfoCount, NVRAM_STORE_INFO *pInfo
);
EFI_STATUS NvGetNextVariableName2(
	IN OUT UINTN *VariableNameSize,
	IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid,
    UINT32 InfoCount, NVRAM_STORE_INFO *pInfo, UINT32 *LastInfoIndex,
    BOOLEAN Runtime
);

//Service routines
VOID NvInitInfoBuffer(
    IN NVRAM_STORE_INFO *pInfo, UINT32 HeaderSize, UINT8 Flags,
    NVRAM_STORE_INTERFACE *Interface
);
VOID NvResetInfoBuffer(IN NVRAM_STORE_INFO *pInfo);
NVRAM_STORE_INFO* NvGetDefaultsInfo(
    IN const CHAR16* DefaultsVar, 
    IN NVRAM_STORE_INFO *pInInfo, OUT NVRAM_STORE_INFO *pOutInfo
);
EFI_STATUS NvGetAttributesFromNvar(
    IN NVAR *pNvar, IN NVRAM_STORE_INFO *pInfo,
    OUT UINT32 *Attributes
);
EFI_STATUS NvGetVariableFromNvar(
    NVAR *pNvar, UINTN NameSize, OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data,
    IN NVRAM_STORE_INFO *pInfo, OUT UINT8 *Flags OPTIONAL
);
EFI_FFS_FILE_STATE* GetNvramFfsFileStatePtr(VOID *Address);
EFI_FFS_FILE_STATE GetNvramFfsFileState(VOID *Address);
BOOLEAN IsMainNvramStoreValid(
        VOID *MainAddress, VOID *BackupAddress,
    BOOLEAN *BackupStoreValid
);
UINT32 GetFvHeaderSize(VOID *Address);
UINT32 GetNvStoreHeaderSize(VOID *Address);
UINT8 NvCalculateNvarChecksum(NVAR* pVar);
UINT8* NvGetExtFlags (NVAR* pVar);

// Shared with AuthVariable service
typedef struct {
    UINT8  AuthFlags; // AuthWriteAccess = 0x10 and TimeWriteAccess = 0x20
    UINT64 Mc;
    UINT8  KeyHash[32]; // sha256
}EXT_SEC_FLAGS; 

VOID
GetVarAuthExtFlags(
    IN NVAR *Var, 
    IN NVRAM_STORE_INFO *pInfo, 
    OUT EXT_SEC_FLAGS *ExtFlags
);

EFI_STATUS FindVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes ,
    IN OUT UINTN *DataSize, OUT VOID **Data
);

EFI_STATUS CheckStore(
    IN NVRAM_STORE_INFO *Info
);

BOOLEAN IsNvramRuntime();

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
