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
/** @file NvramDxe.c

**/
//**********************************************************************
#include "NvramDxeCommon.h"
#include "NvramSmi.h"
#include <Library/PcdLib.h>
#include <AmiNvramDxePlatformHooks.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/HardwareErrorVariable.h>

#define HASH_SHA256_LEN 32

#define ALL_VARIABLE_ATTRIBUTES ( EFI_VARIABLE_NON_VOLATILE |\
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS |\
                                  EFI_VARIABLE_RUNTIME_ACCESS|\
                                  EFI_VARIABLE_HARDWARE_ERROR_RECORD |\
                                  EFI_VARIABLE_APPEND_WRITE |\
                                  UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES\
                                )

//##### Authenticated Variables Support ######
#if AuthVariable_SUPPORT
EFI_STATUS VerifyVariable (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32   *Attributes,
    IN VOID     **Data,
    IN UINTN    *DataSize, 
    IN VOID     *OldData,   
    IN UINTN     OldDataSize,
    IN OUT EXT_SEC_FLAGS *ExtFlags
);
#else
EFI_STATUS VerifyVariable (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32   *Attributes,
    IN VOID     **Data,
    IN UINTN    *DataSize, 
    IN VOID     *OldData,   
    IN UINTN     OldDataSize,
   IN OUT EXT_SEC_FLAGS *ExtFlags
){
    // According to the UEFI specification,  SECURITY_VIOLATION should be returned 
    // if no Authenticated Variables are not supported
    if (*Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES)
        return EFI_SECURITY_VIOLATION; 
    else
        return EFI_SUCCESS;
}
#endif //#if AuthVariable_SUPPORT

//============================================================================
// Type definitions
typedef struct {
    CHAR16 *Name;
    EFI_GUID Guid;
} SDL_VAR_ENTRY;

typedef struct {
    CHAR16 *Name;
    UINT32 Attributes;
} STD_EFI_VAR_ENTRY;

/**
    Structure describing single entry in list of locked variables
**/
typedef struct 
{
	UINTN   	EntryLength;
	EFI_GUID    Guid;
} LOCK_VAR_ENTRY;

/**
	Header of locked variables table
**/
typedef struct {
	UINT8  *NextPtr;///< Pointer to the next free position
	UINTN 	TableSize;///< Size of the table
    UINTN 	NumTableEntries;///< Number of  entries in the table
} LOCK_VAR_TABLE;

typedef BOOLEAN (*COPY_VAR_STORE_FILTER)(
    IN VOID *Context, CHAR16* VarName, EFI_GUID* VarGuid
);

typedef struct {
	NVRAM_STORE_INFO *DefInfo;
    UINT32 	ResetPropertyMask;
} NVRAM_RESET_FILTER_CONTEXT;

//======================================================================
//Function Prototypes
EFI_STATUS InitTmpVolatileStore(
    NVRAM_STORE_INFO *Info, UINTN Size,
    UINT32 HeaderLength, UINT8 Flags,
    BOOLEAN Runtime
);
EFI_STATUS GetFfsFileStateAddressAndValue(
    VOID **Address, EFI_FFS_FILE_STATE* NewState
);
EFI_STATUS CheckLockedVarList (CHAR16* VarName, EFI_GUID* VarGuid);
//======================================================================
// Global and external variables
extern BOOLEAN NvramChecksumSupport;

static BOOLEAN Runtime = FALSE;
static BOOLEAN HideBtVariables = TRUE;
BOOLEAN SmmLocked = FALSE;

VARSTORE_INFO VarStoreInfo;

const static UINT32 NvramSignature = NVAR_SIGNATURE;                                    
UINT8 NvramDriverAllocationMask = 0;

// Build time array of variables to be preserved.
SDL_VAR_ENTRY gPreservedSdlVarLst[] = {
    NVRAM_PRESERVE_VARIABLES_LIST {NULL, {0,0,0,0,0,0,0,0,0,0,0}}
};

SDL_VAR_ENTRY gPersistentSdlVarLst[] = {
    NVRAM_PERSISTENT_VARIABLES_LIST {NULL, {0,0,0,0,0,0,0,0,0,0,0}}
};

STD_EFI_VAR_ENTRY gStdEfiVarList[] = {
	{L"LangCodes",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"Lang",                   EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"Timeout",                EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"PlatformLangCodes",      EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"PlatformLang",           EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"ConIn",                  EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"ConOut",                 EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"ErrOut",                 EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"ConInDev",               EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"ConOutDev",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"ErrOutDev",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"BootOrder",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"BootNext",               EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"BootCurrent",            EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"BootOptionSupport",      EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"DriverOrder",            EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"SysPrepOrder",           EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"HwErrRecSupport",        EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
    {L"OsIndicationsSupported", EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
    {L"OsIndications",          EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
#if SECURE_BOOT_MODULE_REVISION
	{L"SetupMode",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"KEK",                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS},
	{L"PK",                     EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS},
	{L"SignatureSupport",       EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"SecureBoot",             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"KEKDefault",             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"PKDefault",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"dbDefault",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"dbxDefault",             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"dbtDefault",             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"VendorKeys",             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
#endif
#if SECURE_BOOT_MODULE_REVISION >= 19
	{L"dbrDefault",             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"OsRecoveryOrder",        EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS},
#endif
#if SECURE_BOOT_MODULE_REVISION >= 20
	{L"AuditMode",              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{L"DeployedMode",           EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
#endif	
	{NULL, 0}
};

STD_EFI_VAR_ENTRY gStdEfiVarListWildCard[] = {
	{L"Boot****",                              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"Driver****",                            EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"Key****",                               EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"SysPrep****",                           EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE},
	{L"PlatformRecovery****",                  EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS},
	{NULL, 0}
};

STD_EFI_VAR_ENTRY gStdSecurityDbVarList[] = {
#if SECURE_BOOT_MODULE_REVISION        
	{L"db",                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS},
	{L"dbx",                   EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS},
	{L"dbt",                   EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS},
#endif	
#if SECURE_BOOT_MODULE_REVISION >= 19
    {L"dbr",                   EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS},
#endif  
	{NULL, 0}
};

HYBRID_NVRAM_STORE_INTERFACE HybridInterface;

NVRAM_STORE_INFO *UpdataHandlerSourceStore = NULL;
NVRAM_STORE_INFO *UpdateHandlerDestinationStore = NULL;
VOID *gNvramLockVarBuffer = NULL;

UINTN GetVariableNameSize(IN CONST CHAR16 *String, IN UINTN MaxSize){
	CHAR16 *Str, *EndOfStr;
	ASSERT(String!=NULL);
	if (String==NULL) return 0;
	
	EndOfStr = (CHAR16*)((UINT8*)String + MaxSize);
	for(Str = String; Str < EndOfStr; Str++)
		if (!*Str) return (Str - String + 1)*sizeof(CHAR16);

	return MaxSize+1;
}

//--- GetVariable, SetVariable and GetNextVarName Hooks ------
EFI_STATUS GetVariableHook(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
){
    UINTN i;
    EFI_STATUS Result = EFI_UNSUPPORTED;
    for(i=0; GetVariableHookList[i] && Result == EFI_UNSUPPORTED; i++) 
        Result = GetVariableHookList[i](VariableName, VendorGuid, Attributes, DataSize, Data);
    return Result;
}

EFI_STATUS SetVariableHook(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
){
    UINTN i;
    EFI_STATUS Result = EFI_UNSUPPORTED;
    for(i=0; SetVariableHookList[i] && Result == EFI_UNSUPPORTED; i++) 
        Result = SetVariableHookList [i](VariableName, VendorGuid, Attributes, DataSize, Data);
    return Result;
}

EFI_STATUS GetNextVarNameHook(
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
){
    UINTN i;
    EFI_STATUS Result = EFI_UNSUPPORTED;
    CHAR16 FirstCharacter;
    
    // The caller of this function must make sure that VariableName is not NULL.
    FirstCharacter = VariableName[0];
    for(i=0; GetNextVarNameHookList[i] && Result == EFI_UNSUPPORTED; i++){
        Result = GetNextVarNameHookList [i](VariableNameSize, VariableName, VendorGuid);
        if (Result == EFI_NOT_FOUND){
            //EFI_NOT_FOUND is returned when variable with VariableName/VendorGuid
            //is found but it's the last variable in the store supported by a hook.
            //In this case we have to return first variable from the store handled by the next hook.
            //In order to do that we need to reset VariableName.
        	VariableName[0] = 0;
        	Result = EFI_UNSUPPORTED;
        }
    }
    if (EFI_ERROR(Result)) VariableName[0] = FirstCharacter;
    return Result;
}
//--- GetVariable, SetVariable and GetNextVarName Hooks END ------

//Update Interfaces:
//Memory-based
/**
    This function is dummy function doing nothing and always returning TRUE

    @param Address Memory address
    @param Size Size

    @retval TRUE always

**/
BOOLEAN DummyUpdateRoutine(IN NVRAM_STORE_INTERFACE *This, IN VOID *Address, IN UINTN Size) {return TRUE;}
EFI_STATUS MemBeginGarbageCollection(
    IN NVRAM_STORE_INTERFACE *This, VOID* Address, UINTN Size
){ 
    HYBRID_NVRAM_STORE_INTERFACE *Interface = (HYBRID_NVRAM_STORE_INTERFACE*)This;
    BOOLEAN ok;
    UINT32 HeaderLength;
    
    if (!Runtime) PROGRESS_CODE(DXE_NVRAM_CLEANUP);
    NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: MEM Store Garbage Collection\n"));

    HeaderLength = GetNvStoreHeaderSize(Address);
    This->EnableUpdate(This,Address,Size);
    ok=This->EraseBytes(This,(UINT8*)Address+HeaderLength, Size-HeaderLength);
    This->DisableUpdate(This,Address,Size);
    if (!ok) return EFI_DEVICE_ERROR;
    return EFI_SUCCESS;
}
EFI_STATUS DummyEndGarbageCollection(
    IN NVRAM_STORE_INTERFACE *This, BOOLEAN ErrorsFound, VOID* Address
){ 
    return EFI_SUCCESS;
}
EFI_STATUS DummyVirtualFixup(
    IN NVRAM_STORE_INTERFACE *This, UINT8 *BufferAddress
){
    return EFI_SUCCESS;
}

EFI_STATUS DummyCheckStore(
    IN NVRAM_STORE_INTERFACE *This, UINT8 *BufferAddress
){
	return EFI_SUCCESS;
}
EFI_STATUS DummyReinitialize(
    IN NVRAM_STORE_INTERFACE *This, IN UINT8 *Buffer, IN UINTN Size, IN BOOLEAN ReadOnly, OUT UINT32 *HeaderLength
){
	return EFI_SUCCESS;
}

/**
    This function sets Size amount memory starting from Address to FLASH_EMPTY_BYTE 

    @param Address Memory address
    @param Size Size

    @retval TRUE always

**/
BOOLEAN MemErase(IN NVRAM_STORE_INTERFACE *This, IN VOID *Address, IN UINTN Size)
{
    MemSet(Address,Size,FLASH_EMPTY_BYTE);
    return TRUE;
}

/**
    This function writs Data to memory starting from Address 

    @param Address Memory address
    @param Size Size of data to write
    @param Data pointer to data to write

    @retval TRUE always

**/
BOOLEAN MemProgram(IN NVRAM_STORE_INTERFACE *This, IN VOID *Address, IN UINTN Size, IN VOID *Data)
{
    MemCpy(Address,Data,Size);
    return TRUE;
}

//Flash-based
/**
    This function enables writes to flash starting from Address 

    @param Address Start address
    @param Size Size of flash to be write enabled

    @retval TRUE always

**/
BOOLEAN NvEnableWrites(IN AMI_FLASH_PROTOCOL *Flash, IN VOID *Address, IN UINTN Size)
{
    Flash->DeviceWriteEnable();
    return TRUE;
}

/**
    This function disables writes to flash starting from Address 

    @param Address Start address
    @param Size Size of flash to be write disable

    @retval TRUE always

**/
BOOLEAN NvDisableWrites(IN AMI_FLASH_PROTOCOL *Flash, IN VOID *Address, IN UINTN Size)
{
    Flash->DeviceWriteDisable();
    return TRUE;
}

#define INT_SIZE sizeof(INTN)

/**
    This function checks if defined memory chunk is clean

    @param pDest pointer to memory to check
    @param Size size of the chunk

    @retval BOOLEAN TRUE if pDest is clean, otherwise FALSE

**/
static BOOLEAN IsClean(
    IN  UINT8           *pDest,
    IN  UINTN           Size
)
{
    // loops through the destination looking to see if the buffer is empty
    if (!( (UINTN)pDest & (INT_SIZE-1) ))
    {
        for ( ; Size >= INT_SIZE; Size -= INT_SIZE, pDest += INT_SIZE)
	    {
            if (*(UINTN*)pDest != FlashEmpty) return FALSE;
	    }
    }
    
    // since the address may not be INT_SIZE aligned, this checks
    //  the rest of the data
    for ( ; Size > 0; Size--, pDest++)
	{
        if (*pDest != FLASH_EMPTY_BYTE) return FALSE;
	}

    return TRUE;
}

/**
    This function programs Size amount of flash starting with Address

    @param Address pointer to flash to program
    @param Size size of the data to program
    @param Data pointer to the data to be programmed

    @retval BOOLEAN TRUE if pDest is clean, otherwise FALSE

**/
BOOLEAN NvProgram(IN AMI_FLASH_PROTOCOL *Flash, IN VOID *Address, IN UINTN Size, IN VOID *Data)
{
    BOOLEAN Status;
     
    if (Size==0) return TRUE;
    
    Status = !EFI_ERROR(Flash->Write(Address,Size,Data));
    
    if (!Runtime && !Status) ERROR_CODE(DXE_FLASH_UPDATE_FAILED,EFI_ERROR_MAJOR);
    
    return Status;
}

/**
    This function returns pointer to allocated memory to use as
    temp buffer for any occasions. If NvramDriverBuffer is not 0 - 
    memory allocated previously will be used. If not - function will 
    allocate memory with the Size - and return pointer to it.

    @param FirstPart if TRUE - first part of NvramDriverBuffer 
        will be used.
    @param Size size of memory to allocate.

    @retval UINT8 pointer to allocated memory.

**/
VOID* NvBufferAllocate (UINTN Size){
	UINT8 i;
	UINTN AllocationSize = VarStoreInfo.NvramDriverBufferSize / 3;
	
    if (VarStoreInfo.NvramDriverBuffer==0) return NULL;
    if (Size > AllocationSize){
    	ASSERT(FALSE);
    	return NULL;
    }
    
    for(i=0;i<3;i++){
    	if ((NvramDriverAllocationMask & (1<<i))==0){
    		NvramDriverAllocationMask |= 1<<i;
    		return (UINT8*)VarStoreInfo.NvramDriverBuffer + AllocationSize*i;
    	}
    }
    ASSERT(FALSE);
    return NULL;	
}
        
/**
    This function free memory allocated for temp buffer 
    for any occasions. If address passed to this function fits 
    (NvramDriverBuffer +  it's size) range memory allocated in  
    NvramDriverBuffer will be released. If not - function will 
    call FreePool routine.

    @param Target pointer to the beginning of memory to free

    @retval VOID

**/
            
VOID NvBufferFree (VOID* Address)
{
	UINTN AllocationSize = VarStoreInfo.NvramDriverBufferSize / 3;
	UINT8 i;

    for(i=0;i<3;i++){
    	if (Address == (UINT8*)VarStoreInfo.NvramDriverBuffer + AllocationSize*i){
    		NvramDriverAllocationMask &= ~(1<<i);
    		return;
    	}
    }
    ASSERT(FALSE);
}

/**
    This function erases Size amount of flash starting with Address

    @param Address pointer to flash to erase
    @param Size size of the flash to erase

    @retval BOOLEAN TRUE or FALSE based on result

**/
BOOLEAN NvErase(IN AMI_FLASH_PROTOCOL *Flash, IN VOID *Address, IN UINTN Size)
{
    UINT8* pStart = (UINT8*)Address;
    UINT8* pEnd = pStart+Size;
    UINT8* pStartBlock = (UINT8*)BLOCK(pStart);
    UINT8* pEndBlock = (UINT8*)BLOCK(pEnd-1)+FlashBlockSize;
    UINTN PrologSize = pStart - pStartBlock;
    UINTN EpilogSize = pEndBlock-pEnd;
    UINT8* pBuffer=NULL;
    BOOLEAN ok;
    
    if ( Size==0 ) return TRUE;
    
    if (PrologSize+EpilogSize)
    {
        pBuffer = NvBufferAllocate (PrologSize + EpilogSize);
        if (pBuffer == NULL) return FALSE;
        MemCpy(pBuffer,pStartBlock,PrologSize);
        MemCpy(pBuffer+PrologSize,pEnd,EpilogSize);
    }
    ok = !EFI_ERROR(Flash->Erase(Address,Size));
    //erase first block
    if (ok)
    {
        if (PrologSize) ok=NvProgram(Flash,pStartBlock,PrologSize,pBuffer);
        
        if (ok && EpilogSize) ok=NvProgram(Flash,pEnd,EpilogSize,pBuffer+PrologSize);
    }
    
    if (pBuffer) NvBufferFree (pBuffer);
    
    if (!ok) ERROR_CODE(DXE_FLASH_UPDATE_FAILED,EFI_ERROR_MAJOR);
    
    return ok;
}

NVRAM_STORE_INTERFACE MemInterface = {
    DummyUpdateRoutine, DummyUpdateRoutine, MemErase, MemProgram, 
    MemBeginGarbageCollection, DummyEndGarbageCollection,DummyVirtualFixup,
    DummyCheckStore,DummyReinitialize
};

//Hybrid interface
BOOLEAN HybridEnableWrites(NVRAM_STORE_INTERFACE *This, IN VOID *Address, IN UINTN Size){
    UINTN Delta = ((HYBRID_NVRAM_STORE_INTERFACE*)This)->AddressDelta;
	return NvEnableWrites(((HYBRID_NVRAM_STORE_INTERFACE*)This)->Flash, Delta+(UINT8*)Address, Size);
}

BOOLEAN HybridDisableWrites(NVRAM_STORE_INTERFACE *This, IN VOID *Address, IN UINTN Size){
    UINTN Delta = ((HYBRID_NVRAM_STORE_INTERFACE*)This)->AddressDelta;
	return NvDisableWrites(((HYBRID_NVRAM_STORE_INTERFACE*)This)->Flash, Delta+(UINT8*)Address, Size);
}

BOOLEAN HybridErase(NVRAM_STORE_INTERFACE *This, IN VOID *Address, IN UINTN Size){
    BOOLEAN ok;
    UINTN Delta = ((HYBRID_NVRAM_STORE_INTERFACE*)This)->AddressDelta;

    if ( Size==0 || IsClean(Address,Size) ) return TRUE;
	ok = NvErase(((HYBRID_NVRAM_STORE_INTERFACE*)This)->Flash, Delta+(UINT8*)Address, Size);
    // Zero Delta implies that there is no memory buffer
    if (Delta==0) return ok;
    if (ok) return MemErase(NULL, Address, Size);
    // Flash update operation has failed.
    // The memory buffer is out of synch with the flash device.
    // Re-synchronize the memory buffer by reading flash device.
    ((HYBRID_NVRAM_STORE_INTERFACE*)This)->Flash->Read(Delta+(UINT8*)Address, Size, Address);
    return FALSE;
}

BOOLEAN HybridProgram(IN NVRAM_STORE_INTERFACE *This, IN VOID *Address, UINTN Size, VOID *Data){
    BOOLEAN ok;
    UINTN Delta;
    HYBRID_NVRAM_STORE_INTERFACE *Interface = (HYBRID_NVRAM_STORE_INTERFACE*)This;

    if (Size==0) return TRUE;
    
    Delta = Interface->AddressDelta;
	ok = NvProgram(Interface->Flash, Delta+(UINT8*)Address, Size, Data);
    if (ok) return MemProgram(NULL, Address, Size, Data);
    // Flash update operation has failed.
    // The memory buffer is out of synch with the flash device.
    // Re-synchronize the memory buffer by reading flash device.
    Interface->Flash->Read(Delta+(UINT8*)Address, Size, Address);
    return FALSE;
}

EFI_STATUS BeginGarbageCollection(
    IN NVRAM_STORE_INTERFACE *This, VOID* Address, UINTN Size
);
EFI_STATUS EndGarbageCollection(
    IN NVRAM_STORE_INTERFACE *This, BOOLEAN ErrorsFound, VOID* Address
);

/**
    This function will be invoked to convert
    runtime pointers to virtual address

    @param Interface pointer to NVRAM_STORE_INTERFACE structure to fixup

    @retval VOID

**/
VOID VarInterfaceVirtualFixup(NVRAM_STORE_INTERFACE *Interface)
{
    pRS->ConvertPointer(0,(VOID**)&Interface->EnableUpdate);
    pRS->ConvertPointer(0,(VOID**)&Interface->DisableUpdate);
    pRS->ConvertPointer(0,(VOID**)&Interface->EraseBytes);
    pRS->ConvertPointer(0,(VOID**)&Interface->WriteBytes);
    pRS->ConvertPointer(0,(VOID**)&Interface->BeginGarbageCollection);
    pRS->ConvertPointer(0,(VOID**)&Interface->EndGarbageCollection);
    pRS->ConvertPointer(0,(VOID**)&Interface->VirtualFixup);
    pRS->ConvertPointer(0,(VOID**)&Interface->CheckStore);
    pRS->ConvertPointer(0,(VOID**)&Interface->Reinitialize);
}

EFI_STATUS HybridInterfaceVirtualFixup(
    IN NVRAM_STORE_INTERFACE *This, UINT8 *BufferAddress
){
    HYBRID_NVRAM_STORE_INTERFACE *HybridInterface = (HYBRID_NVRAM_STORE_INTERFACE*)This;
    pRS->ConvertPointer(0,(VOID**)&HybridInterface->NvramFlashAddress);
    HybridInterface->AddressDelta=HybridInterface->NvramFlashAddress-BufferAddress;
    if (HybridInterface->BackupAddress!=NULL)
        pRS->ConvertPointer(0,(VOID**)&HybridInterface->BackupAddress);
    pRS->ConvertPointer(0, (VOID**)&HybridInterface->Flash);
    VarInterfaceVirtualFixup(This);
    return EFI_SUCCESS;
}

EFI_FFS_FILE_STATE ReadNvramStoreState(
    IN AMI_FLASH_PROTOCOL *Flash, VOID *FlashAddress, UINT32 StateOffset
){
	EFI_STATUS Status;
	EFI_FFS_FILE_STATE State;
    UINT32 Signature;

    Status = Flash->Read((UINT8*)FlashAddress+OFFSET_OF(EFI_FIRMWARE_VOLUME_HEADER,Signature),sizeof(Signature),&Signature);
    if (EFI_ERROR(Status) || Signature != EFI_FVH_SIGNATURE) return EFI_FILE_HEADER_INVALID;
    Status = Flash->Read((UINT8*)FlashAddress+StateOffset,sizeof(State),&State);
    if (EFI_ERROR(Status)) return EFI_FILE_HEADER_INVALID;
    if (FlashEmpty!=0) State = ~State;
    if (State==0) State = EFI_FILE_HEADER_INVALID;
    
    return State;
}

EFI_STATUS HybridInterfaceCheckStore(
    IN NVRAM_STORE_INTERFACE *This, VOID *BufferAddress
){
	HYBRID_NVRAM_STORE_INTERFACE *Interface = (HYBRID_NVRAM_STORE_INTERFACE*)This;
	EFI_FFS_FILE_STATE BufferState,State,BackupState;
    UINT32 StateOffset;

    // The function is not intended to be used during interface initialization.
    // It is called periodically to make sure physical store is still in synch with the memory buffer.
    // Since the function is called frequently, it should be fast. Therefore, it can't really do anything fancy.
    // Just a basic sanity check.
    // We're using State field from the FFS file header to detect successful or unsuccessful updates of NVRAM areas
    // that happened without knowledge of NVRAM driver.
    
    StateOffset = GetFvHeaderSize(BufferAddress);
    if (StateOffset == 0) return EFI_INVALID_PARAMETER;
    BufferState = GetNvramFfsFileState(BufferAddress);
    if ((BufferState & EFI_FILE_DATA_VALID)==0) return EFI_INVALID_PARAMETER;
    StateOffset += OFFSET_OF(EFI_FFS_FILE_HEADER, State);
    
    State = ReadNvramStoreState(Interface->Flash,Interface->NvramFlashAddress,StateOffset);
    if (Interface->BackupAddress==0) BackupState = EFI_FILE_HEADER_INVALID;
    else BackupState = ReadNvramStoreState(Interface->Flash,Interface->BackupAddress,StateOffset);
    // If the state is what we think it is, we're in synch.
    if (   State==BufferState 
        && (    BackupState == EFI_FILE_HEADER_INVALID
             || (BackupState & EFI_FILE_MARKED_FOR_UPDATE) != 0
           )
    ) return EFI_SUCCESS;
    // If state has changed, check if it has an original build time value
    if (State == (EFI_FILE_HEADER_CONSTRUCTION | EFI_FILE_HEADER_VALID | EFI_FILE_DATA_VALID)){
    	//Yes, the state has a built-time value.
    	// It means that NVRAM flash area has been updated and we 
    	// need to reinitialize data structures associated with this store.
    	return EFI_MEDIA_CHANGED;
    }
    // We don't recognize the state value. The store is corrupted.
    // Let's see if back up area is OK.
    if (Interface->BackupAddress==0) return EFI_VOLUME_CORRUPTED;

    // If backup area state has an original build time value, the NVRAM flash area has been updated.
    // Switching to a backup area and reinitialization of the interface data structures associated with this store.
    // is not our responsibility. It is done in the follow up Interface->Reinitialize call.
    if (BackupState == (EFI_FILE_HEADER_CONSTRUCTION | EFI_FILE_HEADER_VALID | EFI_FILE_DATA_VALID)) {
    	return EFI_MEDIA_CHANGED;
    }
    return EFI_VOLUME_CORRUPTED;
}

EFI_STATUS HybridInterfaceReinitialize(
    IN NVRAM_STORE_INTERFACE *This, UINT8 *Buffer, UINTN Size, BOOLEAN ReadOnly, UINT32 *HeaderLength
){
    BOOLEAN IsBackupStoreValid,IsMainStoreValid;
    EFI_STATUS Status;
	HYBRID_NVRAM_STORE_INTERFACE *Interface = (HYBRID_NVRAM_STORE_INTERFACE*)This;

	if (Size < MAX_NVRAM_STORE_HEADER_SIZE) return EFI_OUT_OF_RESOURCES;
	Status = Interface->Flash->Read(Interface->NvramFlashAddress,MAX_NVRAM_STORE_HEADER_SIZE,Buffer);
    if (EFI_ERROR(Status)) return Status;
    if (Interface->BackupAddress != NULL){
    	if (Size < MAX_NVRAM_STORE_HEADER_SIZE*2) return EFI_OUT_OF_RESOURCES;
    	Status = Interface->Flash->Read(Interface->BackupAddress,MAX_NVRAM_STORE_HEADER_SIZE,Buffer+MAX_NVRAM_STORE_HEADER_SIZE);
    	if (EFI_ERROR(Status)) return Status;
    	IsMainStoreValid = IsMainNvramStoreValid(Buffer, Buffer+MAX_NVRAM_STORE_HEADER_SIZE,&IsBackupStoreValid);
    }else{
    	IsMainStoreValid = IsMainNvramStoreValid(Buffer, NULL,&IsBackupStoreValid);
    }
    
    if (!IsMainStoreValid && IsBackupStoreValid){
    	UINT8* OriginalNvramFlashAddress = Interface->NvramFlashAddress;
        Interface->NvramFlashAddress = Interface->BackupAddress;
        Interface->BackupAddress = OriginalNvramFlashAddress;
        IsMainStoreValid = TRUE;
    }
    Interface->AddressDelta=Interface->NvramFlashAddress-Buffer;
    Status = Interface->Flash->Read(Interface->NvramFlashAddress,Size,Buffer);
    if (EFI_ERROR(Status)) return Status;
    if (!IsMainStoreValid){
    	if (ReadOnly){
    		Status = EFI_VOLUME_CORRUPTED;
    	}else{
            EFI_STATUS Status, Status2;
            Status = This->BeginGarbageCollection(This,Buffer,Size);
            Status2 = This->EndGarbageCollection(This,EFI_ERROR(Status),Buffer);
            if (EFI_ERROR(Status) || EFI_ERROR(Status2)) Status = EFI_DEVICE_ERROR;
            else Status = NVRAM_WARN_STORE_REPAIRED;
    	}
    }
    if (Status != EFI_DEVICE_ERROR && ((EFI_FIRMWARE_VOLUME_HEADER *)Buffer)->FvLength != Size)
    	Status = EFI_INVALID_PARAMETER;
    if (!ReadOnly && !EFI_ERROR(Status)){
        EFI_FFS_FILE_STATE State = GetNvramFfsFileState(Buffer);
        if (State == (EFI_FILE_HEADER_CONSTRUCTION | EFI_FILE_HEADER_VALID | EFI_FILE_DATA_VALID)){
        	// The state has a default built-time value.
        	// set EFI_FILE_MARKED_FOR_UPDATE bit to indicate the store is managed by NVRAM driver.
        	EFI_FFS_FILE_STATE *StatePtr = Buffer;
        	BOOLEAN ok;
            State = EFI_FILE_MARKED_FOR_UPDATE;
            Status = GetFfsFileStateAddressAndValue(&StatePtr,&State);
            if (EFI_ERROR(Status)) return Status;
            This->EnableUpdate(This,StatePtr,sizeof(State));
            ok = This->WriteBytes(This,StatePtr,sizeof(State),&State);
            This->DisableUpdate(This,StatePtr,sizeof(State));
            if (!ok) return EFI_DEVICE_ERROR;
            Status = NVRAM_WARN_MEDIA_CHANGED;
        }
    }
    if (HeaderLength!=NULL) *HeaderLength = GetNvStoreHeaderSize(Buffer);
    return Status;
}

EFI_STATUS InitHybridInterface(
        HYBRID_NVRAM_STORE_INTERFACE *This, 
        UINT8 *Buffer, UINTN Size, BOOLEAN ReadOnly, UINT8* FlashAddress, UINT8* BackupAddress,
        AMI_FLASH_PROTOCOL *Flash, UINT32 *HeaderLength
){
    static HYBRID_NVRAM_STORE_INTERFACE HybridInterfaceTemplate = {
        { 
            HybridEnableWrites, HybridDisableWrites, HybridErase, HybridProgram, 
            BeginGarbageCollection, EndGarbageCollection,HybridInterfaceVirtualFixup,
            HybridInterfaceCheckStore, HybridInterfaceReinitialize
        },
        NULL,0,NULL,NULL
    };
    *This = HybridInterfaceTemplate;
    This->BackupAddress = BackupAddress;
    This->NvramFlashAddress = FlashAddress;
    This->Flash = Flash;
    return HybridInterfaceReinitialize(&This->Interface,Buffer,Size,ReadOnly,HeaderLength);
}

///////////////////////////////////////////////////////////////////////////
BOOLEAN AreBtVariablesHidden()
{
	return HideBtVariables && Runtime;
}

/**
    This function returns the value of Runtime local variable

    @param VOID

    @retval BOOLEAN current value of Runtime local variable

**/
BOOLEAN IsNvramRuntime()
{
    return Runtime;
}

BOOLEAN FindVariableGetSetRecords(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINTN *VariableNameSize, 
    OUT NVAR **ReadRecord, OUT NVAR **LastReadRecord, OUT NVRAM_STORE_INFO **ReadInfo,
    OUT NVAR **WriteRecord, OUT NVAR **LastWriteRecord, OUT NVRAM_STORE_INFO **WriteInfo
){
    UINTN NameSize;
    NVRAM_STORE_INFO *Info;
    UINT32 i;
    NVAR* Var;
    BOOLEAN GetRecordFound;

    GetRecordFound = FALSE;
    for(i = 0; i < VarStoreInfo.InfoCount; i++){
        NVAR *LastVar;
        Info = &VarStoreInfo.NvramInfo[i];
        if ( (Info->Flags & NVRAM_STORE_FLAG_READ_ONLY)!=0 && GetRecordFound) continue;
        Var = NvFindVariable(VariableName,VendorGuid,&NameSize,Info);
        if (Var == NULL) continue;
        LastVar = NULL;
        if (!GetRecordFound){
            GetRecordFound = TRUE;
            if (VariableNameSize != NULL) *VariableNameSize = NameSize;
            if (ReadInfo != NULL) *ReadInfo = Info;
            if (ReadRecord != NULL) *ReadRecord = Var;
            if (LastReadRecord != NULL){
                LastVar = NvGetDataNvar(Var,Info);
                *LastReadRecord = LastVar;
            }
            if (WriteRecord == NULL) return TRUE;
            if ( (Info->Flags & NVRAM_STORE_FLAG_READ_ONLY)!=0 ) continue;
        }
        *WriteRecord  = Var;
        if (LastWriteRecord != NULL){
            *LastWriteRecord = (LastVar == NULL)? NvGetDataNvar(Var,Info) : LastVar;
        }
        if (WriteInfo != NULL) *WriteInfo = Info;
        return TRUE;
    }
    if (GetRecordFound){
    	if (WriteRecord != NULL) *WriteRecord = NULL; 
    	return TRUE;
    }else{
    	return FALSE;
    }
}

BOOLEAN FindVariableGetRecords(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINTN *VariableNameSize, 
    OUT NVAR **ReadRecord, OUT NVAR **LastReadRecord, OUT NVRAM_STORE_INFO **ReadInfo
){
    return FindVariableGetSetRecords(
            VariableName, VendorGuid,
            VariableNameSize, 
            ReadRecord, LastReadRecord, ReadInfo,
            NULL, NULL, NULL
    );
}

///////////////////////////////////////////////////////////////////////////
//Basic operations
/**
    This function checks is there is Size space available in NVRAM_STORE_INFO

    @param Info pointer to NVRAM_STORE_INFO structure
    @param Size Size to check for

    @retval BOOLEAN TRUE if space available, FALSE otherwise

**/
BOOLEAN IsEnoughSpace(NVRAM_STORE_INFO* Info, UINTN Size)
{
    return (   Info->pEndOfVars + Size
               <= Info->NvramAddress+Info->NvramSize - sizeof(EFI_GUID)*(Info->NextGuid+1) - sizeof(NVAR)
           );
}

/**
    This function checks is there was a flash update failure and signal about it
    to CheckStore

    @param Info pointer to NVRAM_STORE_INFO structure
    @param EndOfVars End of Vars pointer
    @param DataSize Size of data, that was programmed
    @param PrevVar pointer to previous instance of Var

    @retval VOID

**/
VOID RecoverFromFlashUpdateFailure(
    NVRAM_STORE_INFO *Info, VOID *EndOfVars, UINTN DataSize, NVAR *PrevVar 
)
{
    UINT8 *p;
    NVAR *NewVar;
    //Nothing has been programmed. Just return
    if (EndOfVars==Info->pEndOfVars) return;
    
    NewVar = (NVAR*)Info->pEndOfVars;
    //Check if variable size has been programmed correctly
    if (NvSize(NewVar)==DataSize)
    {
        // Let's see if the signature is correct.
        // If the signature is correct, let's see if 
        // the 'next' field has been properly updated.
        if (   NvSignature(NewVar)==NvramSignature
            && NvGetDataNvar(PrevVar, Info) != NewVar
        ){
            // if we are here, the next field has not been properly updated
            // leave pInfo->pEndOfVars at the address of the last record to trigger error recovery.
            Info->LastVarSize=(VAR_SIZE_TYPE)DataSize;
            return;
        }
        //Yes, the size has been programmed correctly.
        //NvGetNextNvar routine is smart enough to skip
        //invalid records with valid size.
        Info->pEndOfVars+=DataSize;//Update Info->pEndOfVars
        Info->LastVarSize=0;//Operation failed, set Info->LastVarSize to zero
        return;
    }
    
    //check if something has actually been programmed
    for (p=Info->pEndOfVars; p<(UINT8*)EndOfVars && *p==FLASH_EMPTY_BYTE; p++) ;
    
    //if nothing has been programmed, just return
    if (p==EndOfVars) return;
    
    //if we reached this point, NVRAM store is corrupted
    //Set pInfo->pEndOfVars at the address of corruption.
    //This will trigger recovery operation
    //(See 'Error Recovery' section at the end of the DxeSetVariable routine).
    Info->pEndOfVars = p;
    Info->LastVarSize=(VAR_SIZE_TYPE)DataSize;
}

/**
    This function returns Security fields Mc and KeyHash form existing 
    Var in Var store
    It is called called every time variable with one of
    UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES is being verified.

    @param Var pointer to the 1st instance of Var
    @param pInfo pointer to NVRAM store structure
        OUT EXT_SEC_FLAGS->MonotonicCount - value of MC or TIME stamp 
        OUT EXT_SEC_FLAGS->KeyHash - pointer to memory, allocated by caller, 
        where Hash of PublicKeyDigest will be returned.

    @retval *EXT_SEC_FLAGS

**/
VOID
GetVarAuthExtFlags(
    IN NVAR *Var, 
    IN NVRAM_STORE_INFO *pInfo, 
    OUT EXT_SEC_FLAGS *ExtFlags
)
{
    if(Var==NULL || pInfo==NULL || ExtFlags==NULL)
        return;
// Return Authenticate attributes along with NonVolitile attrib
    if ( pInfo->Flags & NVRAM_STORE_FLAG_NON_VALATILE )
        ExtFlags->AuthFlags |=EFI_VARIABLE_NON_VOLATILE;

    if(Var->flags & (NVRAM_FLAG_AUTH_WRITE | NVRAM_FLAG_EXT_HEDER)) {
        ExtFlags->AuthFlags |= (UINT8)(*NvGetExtFlags (Var) & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES) ;//(NVRAM_eFLAG_TIME_BASED_AUTH_WRITE | NVRAM_eFLAG_AUTH_WRITE));
        ExtFlags->Mc = (*(UINT64*)(NvGetExtFlags (NvGetDataNvar(Var, pInfo)) + 1));
        MemCpy(&ExtFlags->KeyHash[0], (VOID*)(NvGetExtFlags (Var) + 1 + 8), HASH_SHA256_LEN);
    }
}

/**
    This function searches for Var with specific GUID and Name

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var is located

    @retval EFI_SUCCESS if was found
	@retval EFI_NOT_FOUND if not

**/
EFI_STATUS FindVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes ,
    IN OUT UINTN *DataSize, OUT VOID **Data
)
{
    EFI_STATUS Status;
    UINTN VariableNameSize;
    NVRAM_STORE_INFO *Info;
    NVAR *Nvar, *LastNvar;
    BOOLEAN Found;
    
    Found = FindVariableGetRecords(VariableName,VendorGuid,&VariableNameSize,&Nvar,&LastNvar, &Info);
    if (!Found) return EFI_NOT_FOUND;
    if (Data==NULL || DataSize==NULL) return EFI_INVALID_PARAMETER;
    // Get the attributes from the first entry, data-only entries do not store attributes
    if (Attributes!=NULL){
        Status = NvGetAttributesFromNvar(Nvar, Info, Attributes);
        if (EFI_ERROR(Status)) return Status;
    }
    *Data = NvGetData(LastNvar, VariableNameSize, DataSize, Info);
    return EFI_SUCCESS;
}
                
/**
    This function adds new variable to Varstore

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes attributes of Variable
    @param DataSize size of Var data
    @param Data pointer to the Var data
    @param Info pointer to NVRAM_STORE_INFO structure
    @param Interface pointer to the interface with the Variable routines
        (OPTIONAL) NVAR* OldVar - in case of garbage collection pointer to the first
        instance of variable to get Monotonic Counter and Flags from
        (OPTIONAL) NVRAM_STORE_INFO *FromInfo - in case of garbage collection pointer to the
        store info structure - to get Public Key from

    @retval EFI_STATUS - based on result

**/
EFI_STATUS CreateVariableEx(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data,
    NVRAM_STORE_INFO *Info, OPTIONAL NVAR* OldVar, EXT_SEC_FLAGS *ExtSecFlags, 
    OPTIONAL VOID *AppendData, OPTIONAL UINTN AppendDataSize
)
{
    UINTN Size=sizeof(NVAR) + AppendDataSize, ExtSize = 0;
    NVAR Var;
    CHAR16* s;
    EFI_GUID* Guid;
    INT16 guid=0;
    BOOLEAN ok;
    UINT8 *p, *p1, Dummy, ExtFlags = 0;
    NVRAM_STORE_INTERFACE *Interface = Info->Interface;
    if (Interface==NULL) return EFI_UNSUPPORTED;
    
    Var.signature = FLASH_EMPTY_SIGNATURE;
    Var.size = 0;
    Var.next = FLASH_EMPTY_NEXT;
    Var.flags = NVRAM_FLAG_VALID;

    if (Attributes & EFI_VARIABLE_RUNTIME_ACCESS)
        Var.flags |= NVRAM_FLAG_RUNTIME;
    if ( Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD )
        Var.flags |= NVRAM_FLAG_HARDWARE_ERROR_RECORD;
    if (Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES/*EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS*/)
    {
        Var.flags |= NVRAM_FLAG_EXT_HEDER | NVRAM_FLAG_AUTH_WRITE;
        ExtSize = 1 + 8 + HASH_SHA256_LEN + 2; //ExtFlags + Monotonic Counter + PublicKeyDigest Hash + ExtSize
        ExtFlags |= Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES;
        // We store PublicKeyDigest Hash only in the first instance of Var in store to save space
    }
    if (NvramChecksumSupport) 
    {
        Var.flags |= NVRAM_FLAG_EXT_HEDER;
        if (ExtSize == 0) ExtSize += 4; // ExtSize - Ext Flag, Checksumm and Size
        else ExtSize += 1; // Add Checksumm Field
        ExtFlags |= NVRAM_EXT_FLAG_CHECKSUM;
    }
        
    //find GUID in the GUID area
    for ( Guid=Info->NvramGuidsAddress
               ;   Guid>Info->NvramGuidsAddress-Info->NextGuid
            && guidcmp(Guid,VendorGuid)
            ; Guid--
        ) guid++;
        
    if (((OldVar != NULL) && (OldVar->flags & NVRAM_FLAG_GUID)) || (Info->NextGuid>255 && guid>=Info->NextGuid))
    {
        Var.flags |= NVRAM_FLAG_GUID;
        Size+=sizeof(EFI_GUID);
    }
    
    else
    {
        Size+=1;
    }
    
    //Analyze the name. Check if it has non basic Latin characters
    for (s=VariableName; *s&&!*((CHAR8*)s+1); s++) ;
    
    if (!*s)
    {
        Var.flags |= NVRAM_FLAG_ASCII_NAME;
        Size+=s-VariableName+1;
    }
    
    else
    {
        while (*s++);
        
        Size+=(UINT8*)s-(UINT8*)VariableName;
    }
    if ((UINTN)(~0) - Size < DataSize + ExtSize)
    	return EFI_OUT_OF_RESOURCES;
    
    Size += DataSize + ExtSize;
    
    if (!IsEnoughSpace(Info,Size)) return EFI_OUT_OF_RESOURCES;
    
    Var.size=(VAR_SIZE_TYPE)Size;
    Interface->EnableUpdate(Interface,Info->NvramAddress,Info->NvramSize);
    p = Info->pEndOfVars;
    
    do
    {
        //Program Var at pEndOfVars
        ok=Interface->WriteBytes(Interface,p,sizeof(NVAR),&Var);
        p += sizeof(NVAR);
        
        if (!ok) break;
        
        //Program Guid
        if (Var.flags&NVRAM_FLAG_GUID)
        {
            //program the whole guid
            ok=Interface->WriteBytes(Interface,p,sizeof(EFI_GUID),VendorGuid);
            p+=sizeof(EFI_GUID);
        }
        
        else
        {
            //program the whole guid at NVRAM_GUIDS_ADDRESS - NextGuid
            if (guid==Info->NextGuid)
            {
                ok=Interface->WriteBytes(
                        Interface,
                       (UINT8*)(Info->NvramGuidsAddress-Info->NextGuid),
                       sizeof(EFI_GUID),VendorGuid
                   );
                Info->NextGuid++;
                
                if (!ok) break;
            }
            
            //program guid(1 byte) at pEndOfVars+sizeof(NVAR);
            ok=Interface->WriteBytes(Interface,p++,1,&guid);
        }
        
        if (!ok) break;
        
        //Program name and data
        if (Var.flags&NVRAM_FLAG_ASCII_NAME)
        {
            UINT8* q;
            
            for (q=(UINT8*)VariableName; *(CHAR16*)q && ok ; q+=2,p++)
                ok = Interface->WriteBytes(Interface,p,1,q);
                
            if (ok) ok = Interface->WriteBytes(Interface,p++,1,q); //zero at end
        }
        
        else
        {
            UINT16* q;
            
            for (q=VariableName; *q && ok ; q++,p+=2)
                ok = Interface->WriteBytes(Interface,p,2,q);
                
            if (ok)
            {
                ok = Interface->WriteBytes(Interface,p,2,q); //zero at end
                p+=2;
            }
        }
        
        if (!ok) break;

        //AppendWrite after Garbage collection
        if (Attributes & EFI_VARIABLE_APPEND_WRITE) {  
            ok=Interface->WriteBytes(Interface,p,AppendDataSize,AppendData);
            p+=AppendDataSize;
            if (!ok) break;
        }
        
        //Program data
        ok=Interface->WriteBytes(Interface,p,DataSize,Data);
        p+=DataSize;
        p1=p; 
        
        if (!ok) break;
        
        //---Fill Ext area---
        if (Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES/*EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS*/)
        {   
            ok=Interface->WriteBytes(Interface, p, 1, &ExtFlags);//--Ext Flag 
            if (!ok) break;
            p += 1;
            ok=Interface->WriteBytes(Interface, p, 8, &ExtSecFlags->Mc);//--Monotonic counter
            if (!ok) break;
            p += 8;
            ok=Interface->WriteBytes(Interface, p, HASH_SHA256_LEN, &ExtSecFlags->KeyHash[0]);//--Public Key Hash
            if (!ok) break;
            p = Info->pEndOfVars + Size - 2;
            ok=Interface->WriteBytes(Interface, p, 2,&ExtSize);//--Ext Size 
            if (!ok) break;
         }
        if (NvramChecksumSupport)
        {   
            if (!(Var.flags & NVRAM_FLAG_AUTH_WRITE)) // ???NVRAM_FLAG_EXT_HEDER
            {   
                p = p1 + ExtSize; 
                //write ExtSize
                ok=Interface->WriteBytes(Interface, p-sizeof(VAR_SIZE_TYPE),sizeof(VAR_SIZE_TYPE),&ExtSize);
                if (!ok) break;
			    //write ExtFlags
                ok=Interface->WriteBytes(Interface, p1, 1, &ExtFlags);
                if (!ok) break;
			    //write checksum
                Dummy = NvCalculateNvarChecksum((NVAR*)Info->pEndOfVars);

			    //adjust to exclude empty checksum field
                Dummy += FLASH_EMPTY_BYTE;
                ok=Interface->WriteBytes(Interface, p-3, 1, &Dummy);
                if (!ok) break;
            }
            else
            {
                p = p1 + ExtSize; 

                Dummy = NvCalculateNvarChecksum((NVAR*)Info->pEndOfVars);

                //adjust to exclude empty checksum field
                Dummy += FLASH_EMPTY_BYTE;
                ok=Interface->WriteBytes(Interface, p-3, 1, &Dummy);
                if (!ok) break;
            }
        }
        //write a signature
        ok=Interface->WriteBytes(
                Interface,Info->pEndOfVars,sizeof(NvramSignature),(VOID*)&NvramSignature
           );

    }
    while (FALSE);
    
    Interface->DisableUpdate(Interface,Info->NvramAddress,Info->NvramSize);
    
    if (ok)
    {
        Info->pEndOfVars += Size;
        Info->LastVarSize=(VAR_SIZE_TYPE)Size;
    }
    
    else
    {
        RecoverFromFlashUpdateFailure(Info,p,Size,(NVAR*)Info->pEndOfVars);
    }
    
    return (ok) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS CreateVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data,
    NVRAM_STORE_INFO *Info, OPTIONAL NVAR* OldVar, EXT_SEC_FLAGS *ExtSecFlags
)
{
    return CreateVariableEx(VariableName, VendorGuid, Attributes, DataSize, 
                            Data, Info, OldVar, ExtSecFlags, NULL, 0);
}
/**
    This function updates variable 

    @param OldVar pointer to the old Var
    @param VariableName pointer to Var Name in Unicode
    @param NameSize size of the Var mane
    @param VendorGuid pointer to Var GUID
    @param Attributes attributes of Variable
    @param DataSize size of Var data
    @param Data pointer to the Var data
    @param Info pointer to NVRAM_STORE_INFO structure
    @param Interface pointer to the interface with the Variable routines

    @retval EFI_STATUS - based on result

**/
EFI_STATUS UpdateVariable(
    IN NVAR *OldVar, IN CHAR16 *VariableName, IN UINTN NameSize,
    IN EFI_GUID *VendorGuid, IN UINT32 Attributes,
    IN UINTN DataSize, IN VOID *Data,
    IN NVAR* LastVar,
    IN EXT_SEC_FLAGS *ExtSecFlags,
    NVRAM_STORE_INFO *Info
)
{
    UINTN OldDataSize;
    VOID *OldData;
    BOOLEAN ok;
    UINTN Size, ExtSize = 0;
    NVAR Var;
    UINT8 *p, *p1, Dummy, ExtFlags = 0;
    NVRAM_STORE_INTERFACE *Interface = Info->Interface;
    if (Interface==NULL) return EFI_UNSUPPORTED;

    if (!NvAttribEq(OldVar,Attributes,Info)) return EFI_INVALID_PARAMETER;

    Var.signature = FLASH_EMPTY_SIGNATURE;
    Var.size = 0;
    Var.next = FLASH_EMPTY_NEXT;
    Var.flags = NVRAM_FLAG_VALID|NVRAM_FLAG_DATA_ONLY;
    
    OldData = NvGetData( LastVar, NameSize, &OldDataSize, Info );

    if (    (Attributes & EFI_VARIABLE_APPEND_WRITE) == 0 
		 && OldDataSize == DataSize && !MemCmp(OldData, Data, DataSize)) 
        return EFI_SUCCESS;
    
    if (Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES/*EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS*/)
    {
        Var.flags |= NVRAM_FLAG_EXT_HEDER | NVRAM_FLAG_AUTH_WRITE;
        ExtSize = 1 + 8 + 2; //ExtFlags + Monotonic Counter + ExtSize
        // AuthFlags only set in 1st instance
        // ExtFlags |= Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES;
    }
    //if new data equal to the old data don't do anything
    if (NvramChecksumSupport)
    {   ExtFlags |= NVRAM_EXT_FLAG_CHECKSUM;
        if (ExtSize == 0) ExtSize += 4; // ExtSize - Ext Flag, Checksumm and Size
        else ExtSize += 1; // Add Checksumm Feild
        Var.flags |= NVRAM_FLAG_EXT_HEDER;
    }

    // Append write implementation     
    if (Attributes & EFI_VARIABLE_APPEND_WRITE)   
        Size=sizeof(NVAR)+ OldDataSize + DataSize + ExtSize;
    else
        Size=sizeof(NVAR)+ DataSize + ExtSize;
    
    if (!IsEnoughSpace(Info,Size)) return EFI_OUT_OF_RESOURCES;
    
    Var.size=(VAR_SIZE_TYPE)Size;
    Interface->EnableUpdate(Interface,Info->NvramAddress,Info->NvramSize);
    p = Info->pEndOfVars;
    
    //Program Var at pEndOfVars
    do
    {
        ok=Interface->WriteBytes(Interface,p,sizeof(NVAR),&Var);
        p += sizeof(NVAR);
        
        if (!ok) break;
        
        //Program data
        //AppendWrite 
        if (Attributes & EFI_VARIABLE_APPEND_WRITE) {  
            ok=Interface->WriteBytes(Interface,p,OldDataSize,OldData);
            p+=OldDataSize;
            if (!ok) break;
        }
        // end Append write
        ok=Interface->WriteBytes(Interface,p,DataSize,Data);
        p+=DataSize;
        p1 = p;
        if (!ok) break;

        //---Fill Ext area---
        if (Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES/*EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS*/)
        {   
            ok=Interface->WriteBytes(Interface,p, 1, &ExtFlags);//--Ext Flag 
            if (!ok) break;
            p += 1;
            ok=Interface->WriteBytes(Interface,p, 8, &ExtSecFlags->Mc);//--Monotonic counter
            if (!ok) break;
            p = Info->pEndOfVars + Size - 2;
            ok=Interface->WriteBytes(Interface,p, 2,&ExtSize);//--Ext Size 
            if (!ok) break;
         }
        if (NvramChecksumSupport)
        {   
            if (!(Var.flags & NVRAM_FLAG_AUTH_WRITE)) // ??? should be ExtFlags?
            {   
                p = p1 + ExtSize; 
                //write ExtSize
                ok=Interface->WriteBytes(Interface,p-sizeof(VAR_SIZE_TYPE),sizeof(VAR_SIZE_TYPE),&ExtSize);
                if (!ok) break;
			    //write ExtFlags
                ok=Interface->WriteBytes(Interface,p-ExtSize, 1, &ExtFlags);
                if (!ok) break;
			    //write checksum
                Dummy = NvCalculateNvarChecksum((NVAR*)Info->pEndOfVars);

			    //adjust to exclude empty checksum field
                Dummy += FLASH_EMPTY_BYTE;
                ok=Interface->WriteBytes(Interface,p-3, 1, &Dummy);
                if (!ok) break;
            }
            else
            {
                p = p1 + ExtSize; 

                Dummy = NvCalculateNvarChecksum((NVAR*)Info->pEndOfVars);

  	            //adjust to exclude empty checksum field
                Dummy += FLASH_EMPTY_BYTE;
                ok=Interface->WriteBytes(Interface,p-3, 1, &Dummy);
                if (!ok) break;
            }
        }    
        //write a signature
        ok=Interface->WriteBytes(Interface,Info->pEndOfVars,sizeof(NvramSignature),(VOID*)&NvramSignature);
        
        if (!ok) break;
        //set Var.next;

        OldVar = LastVar;

        Var.next=(UINT32)(Info->pEndOfVars-(UINT8*)OldVar);
        ok=Interface->WriteBytes(Interface,(UINT8*)OldVar+NEXT_OFFSET,NEXT_SIZE,(UINT8*)&Var+NEXT_OFFSET);
    }
    while (FALSE);
    
    Interface->DisableUpdate(Interface,Info->NvramAddress,Info->NvramSize);
    
    if (ok)
    {
        Info->pEndOfVars += Size;
        Info->LastVarSize=(VAR_SIZE_TYPE)Size;
    }
    
    else
    {
        RecoverFromFlashUpdateFailure(Info,p,Size,OldVar);
    }
    
    return (ok) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/**
    This function deletes variable 

    @param Var pointer to the Var to be deleted
    @param Info pointer to NVRAM_STORE_INFO structure
    @param Interface pointer to the interface with the Variable routines

    @retval EFI_STATUS - based on result

**/
EFI_STATUS DeleteVariable(IN NVAR *Var, NVRAM_STORE_INFO *Info)
{
    UINT8 flag = Var->flags^NVRAM_FLAG_VALID;//invert validity bit
    BOOLEAN ok;
    CHAR8 *WriteAddress = (CHAR8*)Var+FLAG_OFFSET;
    UINTN WriteSize = FLAG_SIZE;
    NVRAM_STORE_INTERFACE *Interface = Info->Interface;
    if (Interface==NULL) return EFI_UNSUPPORTED;
    
    Interface->EnableUpdate(Interface,WriteAddress,WriteSize);
    ok=Interface->WriteBytes(Interface,WriteAddress,WriteSize,&flag);
    Interface->DisableUpdate(Interface,WriteAddress,WriteSize);
    return (ok) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/**
    This function copies variable from one Nvram store to another

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param DataSize size of Var data
    @param Data pointer to the Var data temporary storage
    @param FromInfo pointer to NVRAM_STORE_INFO structure to copy from
    @param ToInfo pointer to NVRAM_STORE_INFO structure to copy to
    @param ToInterface pointer to the interface with the Variable routines

    @retval EFI_STATUS - based on result

**/
EFI_STATUS CopyVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    NVRAM_STORE_INFO *FromInfo, NVRAM_STORE_INFO *ToInfo
)
{
    EFI_STATUS Status;
    UINT32 Attributes;
    NVAR *nVar;
    EXT_SEC_FLAGS ExtSecFlags = {0,0,{0}};
    UINTN DataSize;
    VOID *Data;
    
    DataSize = FromInfo->NvramSize;
    Data = NvBufferAllocate (DataSize);
    if (Data==NULL) return EFI_OUT_OF_RESOURCES;
    
    Status = NvGetVariable(
                 VariableName, VendorGuid, &Attributes, &DataSize, Data,
                 FromInfo, &nVar
             );
             
    if (EFI_ERROR(Status)){
    	NvBufferFree(Data);
    	return Status;
    }

    GetVarAuthExtFlags(nVar, FromInfo, &ExtSecFlags);

    if (nVar->flags & NVRAM_FLAG_AUTH_WRITE)
    	Status = CreateVariable(
               VariableName, VendorGuid, Attributes, DataSize, Data,
               ToInfo, nVar, &ExtSecFlags);
    else
    	Status = CreateVariable(
               VariableName, VendorGuid, Attributes, DataSize, Data,
               ToInfo, nVar, NULL);
    NvBufferFree(Data);
    return Status;
}

//Varstore maintenance

/**
    Looks for the provided variable in the preserved list.

    @param VarName Variable name to be found.
    @param Guid Variable GUID to be found.

    @retval BOOLEAN Depending on result. TRUE if not in list, FALSE - if in list.

**/
BOOLEAN VarInSdlList(
    IN SDL_VAR_ENTRY SdlVarLst[],
    IN CHAR16 *VarName,
    IN EFI_GUID *Guid
)
{
    SDL_VAR_ENTRY *Entry;
    UINTN   Index;
    if (VarName == NULL || Guid == NULL)
        return FALSE; // If it has no name or GUID - it is not in the list 

    // Scan list 
    for (Index = 0, Entry = &SdlVarLst[0]; 
                Entry->Name != NULL; 
                Entry = &SdlVarLst[++Index] )  
    {
        // Check the GUID's for a match.
        if (!guidcmp(&Entry->Guid, Guid)) {
            // Compare variable name strings.
            if ( Entry->Name[0]==L'*'&& Entry->Name[1]==0 || StrCmp(Entry->Name, VarName) == 0 ){
                // Match found!
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOLEAN VarNotInStdVarList (
	IN STD_EFI_VAR_ENTRY StdVarList[], IN CHAR16 *VarName, IN UINT32 Attributes
){
	UINT32 i;

    for (i = 0; StdVarList[i].Name != NULL; i ++){
    	if (    StrCmp (StdVarList[i].Name, VarName) == 0
    		 && StdVarList[i].Attributes == Attributes 
    	) return FALSE; 
    }
    return TRUE;
}

BOOLEAN IsHexNumber(CONST CHAR16 *Str){
	if (Str==NULL) return FALSE;
	while(*Str!=0){
		if (!(   *Str >= L'0' && *Str <= L'9'
	          || *Str >= L'a' && *Str <= L'f'
              || *Str >= L'A' && *Str <= L'F'
             )
        ) return FALSE;
		Str++;
	}
	return TRUE;
}

/**
    Searches for the passed variable in the EFI Global Var lists and 
    if found, makes sure, that it has the same attributes.

    @param VarName Variable name to be found.
    @param Attributes Variable attributes.

    @retval BOOLEAN Depending on result. TRUE if not in list, FALSE - if in list.

**/
BOOLEAN VarNotInEfiGlobalVarList (
  IN CHAR16             *VariNameToTest,
  IN UINT32             Attributes
  )
{
	UINT32    i;
	UINTN     CommonLength;

	if (!VarNotInStdVarList(gStdEfiVarList,VariNameToTest,Attributes)) return FALSE;

    // Look in the list where names could have 4 different hexadecimal numbers at the end
	CommonLength = StrLen(VariNameToTest);
	if (CommonLength<=4) return FALSE;
    CommonLength -= 4; // Subtract 4 last characters, that can be different from those, that are in list
    for (i = 0; gStdEfiVarListWildCard[i].Name != NULL; i ++){
    	if (    CommonLength + 4 == StrLen (gStdEfiVarListWildCard[i].Name)
    		 && StrnCmp(gStdEfiVarListWildCard[i].Name, VariNameToTest, CommonLength) == 0 
    		 && gStdEfiVarListWildCard[i].Attributes == Attributes
    	){
    		if (IsHexNumber(VariNameToTest+CommonLength)) return FALSE;
    		else return TRUE;
    	}
    }
    return TRUE;
}

BOOLEAN DispatchFilterFunctionList(NVRAM_STORE_UPDATE_FILTER *FunctionList[], CHAR16* VarName, EFI_GUID* VarGuid){
	UINT32 i;
	
	if (FunctionList==NULL) return FALSE;
	for(i=0; FunctionList[i]!=NULL; i++)
		if (FunctionList[i](VarName,VarGuid)) return TRUE;
	return FALSE;
}

BOOLEAN IsDefaultVarStoreVariable(CHAR16* VarName, EFI_GUID* VarGuid){
    if (  guidcmp(VarGuid,&AmiDefaultsVariableGuid)==0
        && (    Wcscmp(VarName, (CHAR16*)StdDefaults)==0
             || Wcscmp(VarName, (CHAR16*)MfgDefaults)==0
           )
    ) return TRUE;
    return FALSE;
}

/**
    Helper varstore filtering function. 
    Skips the variables that have defaults.
	
	@param DefInfo pointer to NVRAM_STORE_INFO structure with defaults
	@param VarName pointer to the Var Name
	@param VarGuid pointer to the Var GUID
	@param Info pointer to NVRAM_STORE_INFO structure
  
    @retval TRUE skip the variable
    @retval FALSE do not skip the variable

**/
BOOLEAN NvramResetFilter(
	NVRAM_RESET_FILTER_CONTEXT *Context, CHAR16* VarName, EFI_GUID* VarGuid
){
	// Variables with defaults are processed by NvResetConfiguration. They have to be skipped during a store copy operation.
    if (IsDefaultVarStoreVariable(VarName,VarGuid)) return FALSE;
	if (Context==NULL) return TRUE;
	if (Context->ResetPropertyMask & NVRAM_RESET_KEEP_VARIABLES_WITHOUT_DEFAULTS){
		if(   Context->DefInfo==NULL 
		   || NvFindVariable(VarName, VarGuid, NULL, Context->DefInfo) == NULL
		) return TRUE;
	}
	if (Context->ResetPropertyMask & NVRAM_RESET_KEEP_PERSISTENT_VARIABLES){
		return DispatchFilterFunctionList(NvramUpdateFilterFunctions,VarName,VarGuid);
	}
	return FALSE;
}

BOOLEAN KeepVarsWithDefaults(
    CHAR16* VarName, EFI_GUID* VarGuid
){
	UINT32 i;
	
	if (  guidcmp(VarGuid,&AmiDefaultsVariableGuid)==0
		&& (    Wcscmp(VarName, (CHAR16*)StdDefaults)==0
		     || Wcscmp(VarName, (CHAR16*)MfgDefaults)==0
		   )
	) return TRUE;
	
    for(i = 0; i < VarStoreInfo.InfoCount; i++){
        NVAR *Var;
        NVRAM_STORE_INFO *Info  = &VarStoreInfo.NvramInfo[i];
        
        if ( (Info->Flags & NVRAM_STORE_FLAG_READ_ONLY)==0 ) continue;
        Var = NvFindVariable(VarName,VarGuid,NULL,Info);
        if (Var != NULL) return TRUE;
    }
    return FALSE;
}

BOOLEAN KeepVarsInPreservedList(
    CHAR16* VarName, EFI_GUID* VarGuid
){
    return VarInSdlList(gPreservedSdlVarLst,VarName,VarGuid);
}

BOOLEAN KeepVarsInPersistentList(
    CHAR16* VarName, EFI_GUID* VarGuid
){
    return VarInSdlList(gPersistentSdlVarLst,VarName,VarGuid);
}

EFI_STATUS UpdateHandlerGetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes, OPTIONAL
	IN OUT UINTN *DataSize, OUT VOID *Data
){
	if (UpdataHandlerSourceStore == NULL) return EFI_UNSUPPORTED;
	return NvGetVariable(
		VariableName, VendorGuid, Attributes, DataSize, Data, UpdataHandlerSourceStore, NULL
    );
}

EFI_STATUS UpdateHandlerCopyVariable(
    IN CHAR16* VarName, IN EFI_GUID* VarGuid
){
    UINTN DataSize;
    EFI_STATUS Status;
    
    if (UpdataHandlerSourceStore == NULL || UpdateHandlerDestinationStore == NULL) return EFI_UNSUPPORTED;

    // Check if the variable already exists in the destination store
    DataSize = 0;
	Status = NvGetVariable(VarName, VarGuid, NULL, &DataSize, NULL, UpdateHandlerDestinationStore, NULL);
	if (Status==EFI_BUFFER_TOO_SMALL) return EFI_ALREADY_STARTED;
	
	return CopyVariable(VarName,VarGuid,UpdataHandlerSourceStore,UpdateHandlerDestinationStore);
}

EFI_STATUS DispatchStoreCopyHandlerList(
	NVRAM_STORE_UPDATE_HANDLER *FunctionList[], EFI_GET_VARIABLE GetVariable, NVRAM_COPY_VARIABLE CopyVariable
){
	UINT32 i;
	
	if (FunctionList==NULL) return EFI_SUCCESS;
	for(i=0; FunctionList[i]!=NULL; i++){
		EFI_STATUS Status = FunctionList[i](GetVariable,CopyVariable);
		if (EFI_ERROR(Status)) return Status;
	}
	return EFI_SUCCESS;
}

EFI_STATUS DispatchUpdateHandlerList(
	EFI_GET_VARIABLE GetVariable, NVRAM_COPY_VARIABLE CopyVariable
){
	return DispatchStoreCopyHandlerList(NvramUpdateHandlerFunctions,GetVariable,CopyVariable);
}

EFI_STATUS DispatchCleanupHandlerList(
	EFI_GET_VARIABLE GetVariable, NVRAM_COPY_VARIABLE CopyVariable
){
	return DispatchStoreCopyHandlerList(NvramCleanupHandlerFunctions,GetVariable,CopyVariable);
}

EFI_STATUS CopyBootOptions(
	EFI_GET_VARIABLE GetVariable, NVRAM_COPY_VARIABLE CopyVariable
){
	UINT16 BootOrder[50];
	UINTN BootOrderSize = sizeof(BootOrder);
	EFI_STATUS Status;
	UINT32 i;

	Status = GetVariable(L"BootOrder",&gEfiGlobalVariableGuid,NULL,&BootOrderSize,BootOrder);
	if (EFI_ERROR(Status)) return EFI_SUCCESS;
	Status = CopyVariable(L"BootOrder",&gEfiGlobalVariableGuid);
	if (Status==EFI_ALREADY_STARTED) Status=EFI_SUCCESS;
	if (EFI_ERROR(Status)) return Status;
	for(i=0; i<BootOrderSize/sizeof(UINT16); i++){
		CHAR16 BootVariableName[9];
		Swprintf(BootVariableName,L"Boot%04X",BootOrder[i]);
		Status = CopyVariable(BootVariableName,&gEfiGlobalVariableGuid);
		if (Status==EFI_NOT_FOUND || Status==EFI_ALREADY_STARTED) Status=EFI_SUCCESS;
		if (EFI_ERROR(Status)) return Status;
	}
	return EFI_SUCCESS;
}

/**
    This function copies variables from one varstore to another

    @param Info pointer to NVRAM_STORE_INFO structure to copy from
    @param NewInfo pointer to NVRAM_STORE_INFO structure to copy to
    @param NewInterface pointer to the interface with the update routines
    @param FilterFunction filtering function
    @param FilterContext filtering function context

    @retval EFI_STATUS Depending on result.

**/
EFI_STATUS CopyVariables(
    IN NVRAM_STORE_INFO* Info,
    OUT NVRAM_STORE_INFO *NewInfo,
    COPY_VAR_STORE_FILTER FilterFunction OPTIONAL,
    VOID *FilterContext OPTIONAL,
    NVRAM_STORE_UPDATE_HANDLER UpdateHandler OPTIONAL
)
{
    EFI_STATUS Status;
    CHAR16 *TmpName;
    UINTN NvramSize = Info->NvramSize;
    EFI_GUID Guid;
    
    TmpName = NvBufferAllocate (NvramSize);   
    
    if (TmpName==NULL) return EFI_OUT_OF_RESOURCES;
    
    TmpName[0]=0;
    
    //Copy variables
    do
    {
        UINTN Size = NvramSize;
        Status = NvGetNextVariableName(
                     &Size, TmpName, &Guid, Info, FALSE
                 );
                 
        if (EFI_ERROR(Status)) break;
        
        //check if the variable needs to be skipped
        if (   FilterFunction!=NULL 
            && !FilterFunction(FilterContext, TmpName, &Guid)
        ) continue;
        Status=CopyVariable(
                   TmpName, &Guid, Info, NewInfo
               );
               
        if (EFI_ERROR(Status))
        {
            if (Status==EFI_NOT_FOUND) continue;
            
            NvBufferFree (TmpName);
            return Status;
        }
    }
    while (TRUE);
    if (Status==EFI_NOT_FOUND) Status=EFI_SUCCESS;
    if (!EFI_ERROR(Status) && UpdateHandler!=NULL){
    	UpdataHandlerSourceStore = Info;
    	UpdateHandlerDestinationStore = NewInfo;
    	Status = UpdateHandler(UpdateHandlerGetVariable, UpdateHandlerCopyVariable);
    	UpdataHandlerSourceStore = NULL;
    	UpdateHandlerDestinationStore = NULL;
    }
    NvBufferFree (TmpName);
    return Status;
}

/**
    This function allocates memory copies varstore to a new location

    @param Info pointer to NVRAM_STORE_INFO structure to copy from
    @param SkipVar pointer to var to be skipped
    @param NewInfo pointer to NVRAM_STORE_INFO structure which was created

    @retval EFI_STATUS Depending on result

**/
EFI_STATUS CopyVarStoreToMemStore(
    IN NVRAM_STORE_INFO* Info,
    COPY_VAR_STORE_FILTER FilterFunction,
    VOID *FilterContext,
    OUT NVRAM_STORE_INFO *NewInfo,
    IN NVRAM_STORE_UPDATE_HANDLER UpdateHandler
)
{
    EFI_STATUS Status;
    
    Status = InitTmpVolatileStore(
                 NewInfo, Info->NvramSize, Info->HeaderLength, Info->Flags, FALSE
             );
             
    if (EFI_ERROR(Status)) return Status;
    
    Status = CopyVariables(
        Info,NewInfo,FilterFunction,FilterContext,UpdateHandler
    );

    if (EFI_ERROR(Status)) NvBufferFree (NewInfo->NvramAddress);
    return Status;
}

/**
    This function updates State of Ffs file with new bits described by NewState

    @param Info pointer to NVRAM_STORE_INFO structure
    @param Interface pointer to the interface with the Variable routines
    @param NewState New state to update to

    @retval EFI_STATUS Depending on result.

**/
EFI_STATUS GetFfsFileStateAddressAndValue(
    VOID **Address, EFI_FFS_FILE_STATE* NewState
)
{
    EFI_FFS_FILE_STATE *StatePtr;
    
    if (Address == NULL || NewState == NULL ) return EFI_INVALID_PARAMETER;
    StatePtr = GetNvramFfsFileStatePtr(*Address);

    if (StatePtr==NULL) return EFI_DEVICE_ERROR;
    
    if (FlashEmpty!=0) *NewState = ~*NewState & *StatePtr;
    else               *NewState |= *StatePtr;
    *Address = StatePtr;
    return EFI_SUCCESS;
}

/**
    This function sets State of Ffs file to new value described by NewState

    @param Info pointer to NVRAM_STORE_INFO structure
    @param NewState New state to set

    @retval EFI_STATUS Depending on result.

**/
EFI_STATUS ResetFfsFileStateInMemory(
    VOID *Address, EFI_FFS_FILE_STATE NewState
)
{
    EFI_FFS_FILE_STATE *StatePtr = GetNvramFfsFileStatePtr(Address);
    
    if (StatePtr==NULL) return EFI_DEVICE_ERROR;
    
    if (FlashEmpty!=0) NewState = ~NewState;
    
    *StatePtr = NewState;
    return (*StatePtr == NewState) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/**
    This function validates correctness of the header fields used by the driver
    and fixes them if Update parameter is TRUE

    @param Address NVRAM buffer address
    @param Size NVRAM buffer size
    @param Update If TRUE, invalid header will be updated to fix the problem


    @retval TRUE No problems detected
    @retval FALSE Problem(s) detected
**/
BOOLEAN CheckTheHeader(VOID *Address, UINTN Size, BOOLEAN Update){
    EFI_FIRMWARE_VOLUME_HEADER *Fv = (EFI_FIRMWARE_VOLUME_HEADER*)Address;

    //check the signature
    if (   GetFvHeaderSize(Fv)==0
        || Fv->FvLength!=Size
    ){
        if (Update){
            Fv->Signature=FV_SIGNATURE;
            Fv->ExtHeaderOffset = 0;
            Fv->FvLength=Size;
            Fv->HeaderLength=(UINT16)(sizeof(EFI_FIRMWARE_VOLUME_HEADER)+sizeof(EFI_FV_BLOCK_MAP_ENTRY));
            ResetFfsFileStateInMemory(
                Address,
                EFI_FILE_HEADER_CONSTRUCTION | EFI_FILE_HEADER_VALID | EFI_FILE_DATA_VALID | EFI_FILE_MARKED_FOR_UPDATE
            );
        }
        return FALSE;
    }
    return TRUE;
}

EFI_STATUS BeginGarbageCollection(
    IN NVRAM_STORE_INTERFACE *This, VOID* Address, UINTN Size
){
    HYBRID_NVRAM_STORE_INTERFACE *Interface = (HYBRID_NVRAM_STORE_INTERFACE*)This;
    BOOLEAN ok;
    EFI_STATUS Status;
    UINT32 HeaderLength;
    
    if (!Runtime) PROGRESS_CODE(DXE_NVRAM_CLEANUP);
    NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: NV Store Garbage Collection\n"));

    ok = CheckTheHeader(Address, Size, TRUE);
    HeaderLength = GetNvStoreHeaderSize(Address);
    if (HeaderLength==0) return EFI_UNSUPPORTED;
    if (Interface->BackupAddress!=0){
        UINT8* OriginalNvramFlashAddress;
        if (!ok){
            // If CheckTheHeader returned FALSE, the FV header is corrupted and has to be fixed.
            NvEnableWrites(Interface->Flash,Interface->NvramFlashAddress,Size);
            ok=NvErase(Interface->Flash,Interface->NvramFlashAddress, Size);
            if (!ok) {
                NvDisableWrites(Interface->Flash,Interface->NvramFlashAddress,Size);
                Interface->Flash->Read(Interface->NvramFlashAddress, Size, Address);
                return EFI_DEVICE_ERROR;
            }
            ok=NvProgram(Interface->Flash,Interface->NvramFlashAddress,HeaderLength,Address);
            NvDisableWrites(Interface->Flash,Interface->NvramFlashAddress,Size);
            if (!ok){
                Interface->Flash->Read(Interface->NvramFlashAddress, HeaderLength, Address);
                return EFI_DEVICE_ERROR;
            }
        }
        OriginalNvramFlashAddress = Interface->NvramFlashAddress;
        Interface->NvramFlashAddress = Interface->BackupAddress;
        Interface->AddressDelta=Interface->NvramFlashAddress-(UINT8*)Address;
        Interface->BackupAddress = OriginalNvramFlashAddress;
        Status=ResetFfsFileStateInMemory(
            Address, EFI_FILE_HEADER_CONSTRUCTION|EFI_FILE_HEADER_VALID
        );
        if (EFI_ERROR(Status)) return Status;
    }
    NvEnableWrites(Interface->Flash,Interface->NvramFlashAddress,Size);
    ok=NvErase(Interface->Flash,Interface->NvramFlashAddress, Size);
    if (!ok) {
        NvDisableWrites(Interface->Flash,Interface->NvramFlashAddress,Size);
        Interface->Flash->Read(Interface->NvramFlashAddress, Size, Address);
        return EFI_DEVICE_ERROR;
    }
    ok=NvProgram(Interface->Flash,Interface->NvramFlashAddress,HeaderLength,Address);
    NvDisableWrites(Interface->Flash,Interface->NvramFlashAddress,Size);
    if (!ok){
        Interface->Flash->Read(Interface->NvramFlashAddress, HeaderLength, Address);
        return EFI_DEVICE_ERROR;
    }
    return (MemErase(NULL, (UINT8*)Address+HeaderLength, Size-HeaderLength)) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EndGarbageCollection(
    IN NVRAM_STORE_INTERFACE *This, BOOLEAN ErrorsFound, VOID* Address
){
	// State Transitions:
	//    Main Store State                          Backup Store State                        Valid Store
	// = Initial State After Firmware Flashing
	// 0. DATA_VALID                                HEADER_INVALID                            Main
	// = Initial State After the first boot (InitHybridInterface sets MARKED_FOR_UPDATE bit)
	// 1. DATA_VALID|MARKED_FOR_UPDATE              HEADER_INVALID                            Main
	// = Update  Cycle 1
	// 2. DATA_VALID|MARKED_FOR_UPDATE              HEADER_VALID                              Main
	// 3. DATA_VALID|MARKED_FOR_UPDATE|DELETED      HEADER_VALID                              Main
	// 4. DATA_VALID|MARKED_FOR_UPDATE|DELETED      DATA_VALID|MARKED_FOR_UPDATE              Backup
	// = Update  Cycle 1 is Over
	// = Update  Cycle 2
	// 5. HEADER_VALID                              DATA_VALID|MARKED_FOR_UPDATE              Backup
	// 6. HEADER_VALID                              DATA_VALID|MARKED_FOR_UPDATE|DELETED      Backup
	// 7. DATA_VALID|MARKED_FOR_UPDATE              DATA_VALID|MARKED_FOR_UPDATE|DELETED      Main
	// = Update  Cycle 2 is Over
	// = Update  Cycle 3
	// 8. DATA_VALID|MARKED_FOR_UPDATE              HEADER_VALID                              Main
	// State 8 == State 2

    EFI_STATUS Status;
    HYBRID_NVRAM_STORE_INTERFACE *Interface = (HYBRID_NVRAM_STORE_INTERFACE*)This;
    VOID* StatePtr;
    EFI_FFS_FILE_STATE State;
    BOOLEAN ok;
    
    if (Interface->BackupAddress==0) return EFI_SUCCESS;
    if (ErrorsFound){
        UINT8* OriginalNvramFlashAddress = Interface->NvramFlashAddress;
        Interface->NvramFlashAddress = Interface->BackupAddress;
        Interface->AddressDelta=Interface->NvramFlashAddress-(UINT8*)Address;
        Interface->BackupAddress = OriginalNvramFlashAddress;
        return EFI_SUCCESS;
    }
    
    // Update state of the backup store
    StatePtr = Interface->BackupAddress;
    State = EFI_FILE_DELETED | EFI_FILE_MARKED_FOR_UPDATE;
    Status = GetFfsFileStateAddressAndValue(&StatePtr,&State);
    if (EFI_ERROR(Status)) return Status;
    NvEnableWrites(Interface->Flash,StatePtr,sizeof(State));
    ok = NvProgram(Interface->Flash,StatePtr,sizeof(State),&State);
    NvDisableWrites(Interface->Flash,StatePtr,sizeof(State));
    if (!ok) return EFI_DEVICE_ERROR;

    // Update state of the main store
    StatePtr = Address;
    State = EFI_FILE_DATA_VALID | EFI_FILE_MARKED_FOR_UPDATE;
    Status = GetFfsFileStateAddressAndValue(&StatePtr,&State);
    if (EFI_ERROR(Status)) return Status;
    This->EnableUpdate(This,StatePtr,sizeof(State));
    ok = This->WriteBytes(This,StatePtr,sizeof(State),&State);
    This->DisableUpdate(This,StatePtr,sizeof(State));
    if (!ok) return EFI_DEVICE_ERROR;

    return EFI_SUCCESS;
}

/**
    This function copies Varstore to a new location and makes this copy main Varstore
    if old varstore flag has NVRAM_STORE_FLAG_NON_VALATILE and Backupadress is valid, else it
    just copy Varstor to new location

    @param Info pointer to NVRAM_STORE_INFO structure
    @param NewInfo pointer to the new NVRAM_STORE_INFO Varstore 

    @retval EFI_STATUS Depending on result.

**/
EFI_STATUS UpdateVarstore(
    NVRAM_STORE_INFO* Info, NVRAM_STORE_INFO* NewInfo
)
{
    EFI_STATUS Status,Status2;
    NVRAM_STORE_INTERFACE *Interface = Info->Interface;

    if (Interface==NULL) return EFI_UNSUPPORTED;
    if (Interface->BeginGarbageCollection!=NULL){
        Status = Interface->BeginGarbageCollection(Interface,Info->NvramAddress,Info->NvramSize);
        if (EFI_ERROR(Status)) return Status;
        NvResetInfoBuffer(Info);
    }
    Status = CopyVariables(NewInfo,Info,NULL,NULL,NULL);
    if (Interface->EndGarbageCollection!=NULL){
        Status2 = Interface->EndGarbageCollection(Interface,EFI_ERROR(Status),Info->NvramAddress);
        if (!EFI_ERROR(Status)){
            Status=Status2;
            if (EFI_ERROR(Status)){
                Interface->EndGarbageCollection(Interface,EFI_ERROR(Status),Info->NvramAddress);
            }
        }
    }
    if (!EFI_ERROR(Status))
    {
        VarStoreInfo.LastInfoIndex = 0;
        if (Info==VarStoreInfo.NvInfo) UpdateNestedVarstores();
    }
    return Status;
}

/**
    This function resets NvRam configuration

    @param Info pointer to NVRAM_STORE_INFO structure
    @param Interface pointer to the interface with the Variable routines
    @param ExternalDefaults if TRUE - use external defaults

    @retval EFI_STATUS Depending on result.

**/
EFI_STATUS NvResetConfiguration(
    NVRAM_STORE_INFO* Info, 
    BOOLEAN ExternalDefaults, UINT32 ResetPropertyMask
)
{
    UINT32 HeaderLength = Info->HeaderLength;
    NVRAM_STORE_INFO NewInfo;
    NVRAM_STORE_INFO DefaultsInfo;
    NVRAM_STORE_INFO *DefInfo = NULL;
    EFI_STATUS Status;
 
    PROGRESS_CODE(DXE_CONFIGURATION_RESET);
    NVRAM_TRACE((TRACE_DXE_CORE,
           "NVRAM: NVRAM Reset: ExternalDefaults=%d; ResetPropertyMask=%d\n",
           ExternalDefaults, ResetPropertyMask
    ));
    
    Status = InitTmpVolatileStore(
                 &NewInfo, Info->NvramSize, Info->HeaderLength, Info->Flags, FALSE
             );
             
    if (EFI_ERROR(Status)) return Status;
    
    MemCpy(NewInfo.NvramAddress, Info->NvramAddress, HeaderLength);
    if (ExternalDefaults)
    {
        // {9221315B-30BB-46b5-813E-1B1BF4712BD3}
#define SETUP_DEFAULTS_FFS_GUID { 0x9221315b, 0x30bb, 0x46b5, { 0x81, 0x3e, 0x1b, 0x1b, 0xf4, 0x71, 0x2b, 0xd3 } }
        static EFI_GUID SetupDefaultsFfsGuid = SETUP_DEFAULTS_FFS_GUID;
        EFI_FIRMWARE_VOLUME_PROTOCOL *Fv;
        EFI_HANDLE *FvHandle;
        UINTN Number,i;
        UINT32 Authentication;

        Status = pBS->LocateHandleBuffer(
                     ByProtocol,&gEfiFirmwareVolume2ProtocolGuid, NULL, &Number, &FvHandle
                 );
        
        for (i=0; i<Number; i++)
        {
            Status=pBS->HandleProtocol(FvHandle[i], &gEfiFirmwareVolume2ProtocolGuid, (VOID**) &Fv);
            
            if (EFI_ERROR(Status)) continue;
            
            DefaultsInfo.NvramAddress=NULL;
            DefaultsInfo.NvramSize=0;
            Status=Fv->ReadSection (
                       Fv,&SetupDefaultsFfsGuid,
                       EFI_SECTION_RAW, 0,
                       (VOID**)&DefaultsInfo.NvramAddress, &DefaultsInfo.NvramSize,
                       &Authentication
                   );
                   
            if (!EFI_ERROR(Status))
            {
                NVRAM_TRACE((TRACE_DXE_CORE, "NVRAM: External Defaults Found: Size=%X\n",DefaultsInfo.NvramSize));
                NvInitInfoBuffer(
                    &DefaultsInfo, 0,
                    NVRAM_STORE_FLAG_NON_VALATILE
                    | NVRAM_STORE_FLAG_READ_ONLY
                    | NVRAM_STORE_FLAG_DO_NOT_ENUMERATE,
                    NULL
                );
                DefInfo = &DefaultsInfo;
                break;
            }
        }
    }
    
    else
    {
        DefaultsInfo.NvramAddress = NULL;
        DefInfo = Info;
    }
    
    if (DefInfo!=NULL)
    {
        Status=CopyVariable(
                   (CHAR16*)StdDefaults, (EFI_GUID*)&AmiDefaultsVariableGuid,
                   DefInfo,&NewInfo
               );
        //It's OK if variable does not exist.
        if (Status==EFI_NOT_FOUND) Status=EFI_SUCCESS;
        if (!EFI_ERROR(Status))
        {
            Status=CopyVariable(
                       (CHAR16*)MfgDefaults, (EFI_GUID*)&AmiDefaultsVariableGuid,
                       DefInfo,&NewInfo
                   );
            //It's OK if variable does not exist.
            if (Status==EFI_NOT_FOUND) Status=EFI_SUCCESS;
        }

        if (!EFI_ERROR(Status)){
            NVRAM_STORE_INFO DefVarInfo;
            NVRAM_RESET_FILTER_CONTEXT Context;
            
            Context.DefInfo = NvGetDefaultsInfo(StdDefaults,DefInfo,&DefVarInfo);
            Context.ResetPropertyMask = ResetPropertyMask;
            Status=CopyVariables(
              Info, &NewInfo, NvramResetFilter ,&Context,
              (ResetPropertyMask & NVRAM_RESET_KEEP_PERSISTENT_VARIABLES) ? DispatchUpdateHandlerList : NULL
            );
        }
        if (DefaultsInfo.NvramAddress!=NULL )
        {
            pBS->FreePool(DefaultsInfo.NvramAddress);
        }
        // don't reset NVRAM if no defaults found
        if (NewInfo.pEndOfVars == NewInfo.pFirstVar) DefInfo=NULL;
    }
    
    if (   !EFI_ERROR(Status)
        && DefInfo!=NULL
        && (    Info->pEndOfVars - Info->NvramAddress!= NewInfo.pEndOfVars - NewInfo.NvramAddress
             || DefInfo!=Info
           )
    ) Status=UpdateVarstore(Info,&NewInfo);
       
    NvBufferFree (NewInfo.NvramAddress);
    return Status;
}

/**
    This function checks is Varstore is correct

    @param Info pointer to NVRAM_STORE_INFO structure

    @retval BOOLEAN based on result

**/
BOOLEAN IsStoreOk(NVRAM_STORE_INFO *Info)
{
    return
        (   Info->LastVarSize==0
            || NvIsVariable((NVAR*)(Info->pEndOfVars-Info->LastVarSize),Info)
        )
        && (ReadUnaligned32((UINT32*)Info->pEndOfVars) ==(UINT32)FlashEmpty)
        && (ReadUnaligned16 ((UINT16*)&(((NVAR*)Info->pEndOfVars)->size))==FLASH_EMPTY_SIZE)
        && *(UINT32*)(Info->NvramGuidsAddress-Info->NextGuid)==(UINT32)FlashEmpty;
}

/**
    This function checks if Varstore is correct and otherwise tries to 
    Reinitialize NvRam, copy Varstore to memory and prints messages
    based on situation.

    @param Info pointer to NVRAM_STORE_INFO structure

    @retval BOOLEAN based on result

**/
EFI_STATUS CheckStore(NVRAM_STORE_INFO *Info)
{
    NVRAM_STORE_INFO NewInfo;
    EFI_STATUS Status;
    BOOLEAN MediaChanged;

    if (Info==NULL || Info->Interface==NULL) return EFI_UNSUPPORTED;
    Status = Info->Interface->CheckStore(Info->Interface,Info->NvramAddress);
    if (!EFI_ERROR(Status) && IsStoreOk(Info)) return EFI_SUCCESS;
    NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: NVRAM buffer is out of synch with the physical storage. Reinitializing...\n"));
    MediaChanged = Status == EFI_MEDIA_CHANGED;
    Status = InitTmpVolatileStore(
                 &NewInfo, Info->NvramSize, Info->HeaderLength, Info->Flags, FALSE
             );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    MemCpy(NewInfo.NvramAddress,Info->NvramAddress,Info->NvramSize);
    ResetVarStore(&NewInfo);
    Status = Info->Interface->Reinitialize(Info->Interface, Info->NvramAddress,Info->NvramSize, FALSE, &Info->HeaderLength);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)){
    	NvBufferFree (NewInfo.NvramAddress);
    	return Status;
    }
    if (Status==NVRAM_WARN_STORE_REPAIRED){
    	MediaChanged = FALSE;
    }
    ResetVarStore(Info);
    if (MediaChanged){
    	NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: NVRAM update is detected. Appending persistent variables...\n"));
    	Status = CopyVariables(&NewInfo,Info,DispatchFilterFunctionList,NvramUpdateFilterFunctions,DispatchUpdateHandlerList);
    }else{
    	NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: NVRAM corruption is detected. Restoring variables from memory buffer...\n"));
    	Status = CopyVariables(&NewInfo,Info,NULL,NULL,NULL);
    }
    if (EFI_ERROR(Status)){
        NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: New NVRAM buffer is corrupted. Trying to restore original NVRAM image...\n"));
        Status = UpdateVarstore(Info,&NewInfo);
    }
    ASSERT_EFI_ERROR(Status);
    NvBufferFree (NewInfo.NvramAddress);
    if (EFI_ERROR(Status)) return Status;
    VarStoreInfo.LastInfoIndex = 0;
    if (Info==VarStoreInfo.NvInfo) UpdateNestedVarstores();
    
    return (MediaChanged) ? NVRAM_WARN_MEDIA_CHANGED : NVRAM_WARN_STORE_REPAIRED;
}

//High level routines
/**
    This function searches for Var with specific GUID and Name

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var will be returned

    @retval EFI_STATUS based on result

**/
EFI_STATUS DxeGetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
)
{
    EFI_STATUS Status;
    UINTN VariableNameSize;
    NVRAM_STORE_INFO *Info;
    NVAR *Nvar, *LastNvar;
    BOOLEAN Found;

    if (!VariableName || !VendorGuid || !DataSize || !Data && *DataSize)
        return EFI_INVALID_PARAMETER;
    Found = FindVariableGetRecords(VariableName,VendorGuid,&VariableNameSize,&Nvar,&LastNvar, &Info);
    if (!Found) return EFI_NOT_FOUND;
    if (AreBtVariablesHidden() && !(Nvar->flags & NVRAM_FLAG_RUNTIME)) return EFI_NOT_FOUND;
    Status = NvGetVariableFromNvar(
        LastNvar,VariableNameSize,NULL,DataSize,Data,Info, NULL
    );
    if (!EFI_ERROR(Status) && Attributes){
    	// Get the attributes from the first entry, data-only entries do not store attributes
    	Status = NvGetAttributesFromNvar(Nvar, Info, Attributes);
    	if (EFI_ERROR(Status)) return Status;
    }

    return Status;
}

/**
    This function searches for next Var after Var with specific name and GUID and returns it's Name. 

    @param VariableNameSize size of Varname - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param VariableName pointer where Var Name in Unicode will be stored
    @param VendorGuid pointer to memory where Var GUID is stored

    @retval EFI_STATUS based on result

**/
EFI_STATUS DxeGetNextVariableName(
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
)
{
    return NvGetNextVariableName2(
                 VariableNameSize, VariableName, VendorGuid,
                 VarStoreInfo.InfoCount, VarStoreInfo.NvramInfo,
                 &VarStoreInfo.LastInfoIndex,AreBtVariablesHidden()
             );
}

/**
    This function checks parameters for exit conditions

    @param VariableName - pointer to Var Name in Unicode
    @param VendorGuid - pointer to Var GUID
    @param Attributes - Attributes of the Var
    @param DataSize - size of Var
    @param Data - Pointer to memory where Var data is stored

    @retval EFI_STATUS based on result

**/

EFI_STATUS CheckOnSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
	if (
           !VariableName || VariableName[0]==0 || !VendorGuid
        || ( Attributes & ~ALL_VARIABLE_ATTRIBUTES)
        || (Attributes & EFI_VARIABLE_RUNTIME_ACCESS) && !(Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS )
        || DataSize && !Data
        || Runtime && Attributes && (
                                        !(Attributes & EFI_VARIABLE_NON_VOLATILE) 
                                     || !(Attributes & EFI_VARIABLE_RUNTIME_ACCESS) && AreBtVariablesHidden()
                                    )
    ) return EFI_INVALID_PARAMETER;
	// Skip the verification if we are trying to delete a variable
    if ((!(Attributes & EFI_VARIABLE_APPEND_WRITE) && !DataSize) || 
         !(Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS) 
    ) return EFI_SUCCESS;
    // Reset Append attribute, because it is optional and we don't have to verify it.
    Attributes &= ~EFI_VARIABLE_APPEND_WRITE;
    if (guidcmp(VendorGuid, &gEfiGlobalVariableGuid) == 0){
    	if (VarNotInEfiGlobalVarList(VariableName, Attributes)){
    		NVRAM_TRACE((TRACE_DXE_CORE, "NVRAM: Variable with EfiGlobalVariableGuid has illegal name(%S) or attributes(%x). \n", VariableName, Attributes));
    		ASSERT(FALSE);
    		return EFI_INVALID_PARAMETER;
    	}
    }else if (guidcmp(VendorGuid, &gEfiImageSecurityDatabaseGuid) == 0){
    	if (VarNotInStdVarList(gStdSecurityDbVarList, VariableName, Attributes)){
    		NVRAM_TRACE((TRACE_DXE_CORE, "NVRAM: Variable with gEfiImageSecurityDatabaseGuid has illegal name(%S) or attributes(%x).\n", VariableName, Attributes));
    		ASSERT(FALSE);
    		return EFI_INVALID_PARAMETER;
    	}
    }else if (Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD){
    	if (    guidcmp(VendorGuid, &gEfiHardwareErrorVariableGuid) != 0
    	     || StrLen(VariableName)!=12 // "HwErrRec####"
    	     || MemCmp(VariableName,L"HwErrRec",8*sizeof(CHAR16))!=0
    	     || !IsHexNumber(VariableName+8)
    	){
    		NVRAM_TRACE((TRACE_DXE_CORE, "NVRAM: Variable with HR attribute has illegal name(%S) or GUID(%G).\n", VariableName, VendorGuid));
    		ASSERT(FALSE);
    		return EFI_INVALID_PARAMETER;
    	}
    }
    return EFI_SUCCESS;
}

NVRAM_STORE_INFO* GetSetVariableVarstore(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid, IN UINT32 Attributes
){
    NVRAM_STORE_INFO *Info=NULL;
    
    if (Attributes & EFI_VARIABLE_NON_VOLATILE)
    {
        Info = VarStoreInfo.NvInfo;
    }
    else
    {
        Info = VarStoreInfo.MemInfo;
    }
    return Info;
}

BOOLEAN NvramCleanUpEnabled(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	IN UINT32 Attributes, IN UINTN DataSize, IN UINTN OldDataSize
){
	if (DataSize-OldDataSize < 0x512 || DataSize < 1024) return TRUE;
	if (   VarInSdlList(gPersistentSdlVarLst,VariableName,VendorGuid)
		|| VarInSdlList(gPreservedSdlVarLst,VariableName,VendorGuid)
	) return TRUE;
	return FALSE;
}

/**
    This function validates new content for a variable with defaults.

    @param VariableName Variable name
    @param DataSize     Size of the Data buffer
    @param Data         New variable data passed to SetVariable

    @retval EFI_SUCCESS         No problems found. It's OK to update the variable with the new data.
    @retval EFI_OUT_OF_REOURCES Not enough resources to validate the data. The variable cannot be updated.
    @retval EFI_ACCESS_DENIED   Validation failed. The variable cannot be updated.
**/
EFI_STATUS ValidateDefaultVarStoreVariable (
    IN CHAR16 *VariableName, IN UINTN DataSize, IN VOID *Data
)

{
	NVRAM_STORE_INFO NewDefaultInfo;
	NVRAM_STORE_INFO OldDefaultInfo;
	CHAR16 *TmpName = NULL;
	VOID *NewData = NULL;
	VOID *OldData = NULL;	
	UINTN Size, NewDataSize, OldDataSize;
	EFI_GUID Guid;
	EFI_STATUS Status;
	
	NewDefaultInfo.NvramAddress = Data; 
	NewDefaultInfo.NvramSize = DataSize; 
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    // Get ready to parse new data as a varstore
    NvInitInfoBuffer(
        &NewDefaultInfo, 0,
          NVRAM_STORE_FLAG_NON_VALATILE
        | NVRAM_STORE_FLAG_READ_ONLY
        | NVRAM_STORE_FLAG_DO_NOT_ENUMERATE,
        NULL
    );
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	// Locate current instance of defaults
	if (!NvGetDefaultsInfo(VariableName, VarStoreInfo.NvInfo, &OldDefaultInfo))
		return EFI_NOT_FOUND;
	// Make sure variable size is the same
	if (NewDefaultInfo.NvramSize != OldDefaultInfo.NvramSize)
		return EFI_ACCESS_DENIED;   	
	TmpName = NvBufferAllocate (DataSize);
	if (TmpName==NULL)
		return EFI_OUT_OF_RESOURCES;
	NewData = NvBufferAllocate (DataSize);
	if (NewData==NULL)
		return EFI_OUT_OF_RESOURCES;
	OldData = NvBufferAllocate (DataSize);
	if (OldData==NULL)
		return EFI_OUT_OF_RESOURCES;
	TmpName[0]=0;
	
	do // Scan all the variable in the new defaults varstore
	{
		UINT32 NewAttributes, OldAttributes;
		BOOLEAN LockedVarDetected;
		
		Size = DataSize; 
		
		Status = NvGetNextVariableName(
		                     &Size, TmpName, &Guid, &NewDefaultInfo, FALSE
		                 );    
		
		if (EFI_ERROR(Status)) 
		{	// We are done.
			if (Status == EFI_NOT_FOUND)
				Status= EFI_SUCCESS;
			break;
		}
		NewDataSize = DataSize;
		Status = NvGetVariable(
						TmpName, &Guid, &NewAttributes, 
						&NewDataSize, NewData,
						&NewDefaultInfo, NULL
		             );
		if (EFI_ERROR(Status)) 
		{// This should never happen. If we are hear, the Data buffer is not a well-formed defaults store.
			if (Status == EFI_NOT_FOUND)
				Status = EFI_ACCESS_DENIED;
			break;
				
		}
		// Run standard variable checks performed during variable update
		Status = CheckOnSetVariable (
						TmpName, &Guid, NewAttributes, NewDataSize, NewData
		            );
		if (EFI_ERROR(Status)) 
		{	
			Status = EFI_ACCESS_DENIED;
		    break;
		}
		LockedVarDetected = FALSE;
		Status = CheckLockedVarList (TmpName, &Guid);
		if (EFI_ERROR(Status)) 
			LockedVarDetected = TRUE;
		OldDataSize = DataSize;
		Status = NvGetVariable(
		        		TmpName, &Guid, &OldAttributes, 
		        		&OldDataSize, OldData,
		        		&OldDefaultInfo, NULL
		        		);
		if (EFI_ERROR(Status)) 
		{ // Make sure the variable exists in the current defaults store
			if (Status == EFI_NOT_FOUND)
		    	Status = EFI_ACCESS_DENIED;
		    break;
		        				
		}
		if((OldDataSize != NewDataSize)||(OldAttributes != NewAttributes))
		{ // Make sure variable size and attributes are the same
			Status = EFI_ACCESS_DENIED;
			break;
		}
		if (LockedVarDetected)
		{   // If variable is locked, it cannot be updated unless it is updated with the same data.
			if (MemCmp(OldData,NewData,OldDataSize))
			{
				Status = EFI_ACCESS_DENIED;
        		break;
			}
		}

	}while (TRUE);
	
	NvBufferFree (TmpName);
	NvBufferFree (NewData);
	NvBufferFree (OldData);
	
	return Status;

}

/**
    This function sets Var with specific GUID, Name and attributes

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Attributes of the Var
    @param DataSize size of Var
    @param Data Pointer to memory where Var data is stored

    @retval EFI_STATUS based on result

**/
EFI_STATUS DxeSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
    NVAR    *Var = NULL;
    UINTN NameSize;
    NVRAM_STORE_INFO *Info=NULL;
    EFI_STATUS Status;
    NVRAM_STORE_INFO NewInfo;
    UINTN OldDataSize=0;
    VOID *OldData=NULL;
    EXT_SEC_FLAGS ExtSecFlags = {0, 0,{0}};
    NVAR *ReadVar, *LastVar;
    NVRAM_STORE_INFO *ReadInfo;
    BOOLEAN Found;
    UINT32 i;
 
    if (DataSize>MAX_NVRAM_VARIABLE_SIZE) return EFI_OUT_OF_RESOURCES;
    Status = CheckOnSetVariable (
                VariableName,VendorGuid,Attributes,DataSize,Data
            );
    if (EFI_ERROR(Status)) return Status;
    Status = CheckLockedVarList (VariableName, VendorGuid);
    if (EFI_ERROR(Status)) return Status;
    
    for(i=0; i<2; i++){
        Found = FindVariableGetSetRecords(
        	VariableName,VendorGuid,&NameSize,
        	&ReadVar,NULL,&ReadInfo,&Var,&LastVar,&Info
        );
        if (!Found){
        	Var = NULL;
        	ReadVar = NULL;
        }
        if( Var!=NULL )
        {
            // It's illegal to set boot time variable at run time.
            if (AreBtVariablesHidden() && !(Var->flags & NVRAM_FLAG_RUNTIME)) return EFI_INVALID_PARAMETER;
            // It's illegal to change attributes of existing variable.
            // This is a preliminary check. A more advanced check is performed in the UpdateVarible function.
            if (    (Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS)!=0 // make sure we are not in the delete case and Attributes are valid
            	 && Info != GetSetVariableVarstore(VariableName,VendorGuid,Attributes)
            ) return EFI_INVALID_PARAMETER;
            GetVarAuthExtFlags(Var, Info, &ExtSecFlags);
            OldData = NvGetData( LastVar, NameSize, &OldDataSize, Info);
        } else {
        	if (ReadVar!=NULL){
                //if ReadVar is not NULL, default value exists.
                //If default value exists, the variable is non-volatile.
                //Attributes for existing variable can not be changed.
        		// (Make sure we are not in the delete case and Attributes are valid)
                if (    ( Attributes & (EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS) ) 
                	 == EFI_VARIABLE_BOOTSERVICE_ACCESS 
                ) return EFI_INVALID_PARAMETER;
                
                GetVarAuthExtFlags(ReadVar, ReadInfo, &ExtSecFlags);
                OldData = NvGetData( ReadVar, NameSize, &OldDataSize, ReadInfo);
        	}else{
                OldDataSize = 0;
                OldData = NULL;
        	}
            Info = GetSetVariableVarstore(VariableName,VendorGuid,Attributes);
            if (Info==NULL) return EFI_INVALID_PARAMETER;
        }
        Status = CheckStore(Info);
        if (EFI_ERROR(Status)) return Status;
        // It's important for check specifically for EFI_SUCCESS, as opposed to just !EFI_ERROR(Status).
        // CheckStore can return warning codes. If warning is returned, the content of the store has changed
        // and have to start over.
        if (Status == EFI_SUCCESS) break;
    }
    if (i==2) return EFI_DEVICE_ERROR;
    if (GetVariableNameSize(VariableName, Info->NvramSize) > Info->NvramSize) return EFI_OUT_OF_RESOURCES;
    Status = VerifyVariable(VariableName, VendorGuid, &Attributes, &Data, &DataSize, OldData, OldDataSize, &ExtSecFlags);
    if (EFI_ERROR(Status)) {
    // case for SigDb Append mode. EFI_ALREADY_STARTED treat as Ok to exit SetVar
        if(Status == EFI_ALREADY_STARTED) 
            Status = EFI_SUCCESS;
        return Status;
    }

// Function called with empty access attributes - the variable shall be erased.
    if(!Attributes)
        DataSize = 0;
    //
    // Verification passed.
    //
    if ((!(Attributes & EFI_VARIABLE_APPEND_WRITE) && !DataSize) || 
         !(Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS) )
    {
        //delete
        if (Var==NULL) //if ReadVar is not NULL, default value (which can not be deleted ) exists
            return (ReadVar!=NULL) ? EFI_WRITE_PROTECTED : EFI_NOT_FOUND;
        if (Runtime && (Info->Flags & NVRAM_STORE_FLAG_NON_VALATILE) == 0) return EFI_INVALID_PARAMETER;
        // Variables with defaults can't be updated with user level API.
        if (IsDefaultVarStoreVariable(VariableName, VendorGuid)) return EFI_WRITE_PROTECTED;
        Status = DeleteVariable(Var,Info);
        return Status;
    }
    
    if (Var==NULL)
    {
    	// If we are dealing with variable with defaults, don't create new record if data didn't change
        if (    OldData!=NULL && (Attributes & EFI_VARIABLE_APPEND_WRITE) == 0 
			 && OldDataSize == DataSize && !MemCmp(OldData, Data, DataSize)
		) return EFI_SUCCESS;
    			
        //create new
        Status = CreateVariable(
                     VariableName,VendorGuid,Attributes,
                     DataSize, Data, Info, NULL, &ExtSecFlags// NULL
                 );
        // If Variables with defaults does not exist, update nested varstores once variable is created.
        if (!EFI_ERROR(Status) && IsDefaultVarStoreVariable(VariableName, VendorGuid)) UpdateNestedVarstores();
    }
    else
    {
        // Variables with defaults can be updated with user level API only with restrictions.
        if (IsDefaultVarStoreVariable(VariableName, VendorGuid)){
            Status = ValidateDefaultVarStoreVariable (VariableName, DataSize, Data);
            if (EFI_ERROR(Status)) return Status;
        }
        //update existing
        Status = UpdateVariable(
                     Var,VariableName,NameSize,VendorGuid,Attributes,
                     DataSize, Data,
                     LastVar,
                    &ExtSecFlags,
                     Info
                 );
    }
    
    //Error Recovery
    if (   Status==EFI_OUT_OF_RESOURCES
        || Status==EFI_DEVICE_ERROR && !IsStoreOk(Info)
       )
	{
    	COPY_VAR_STORE_FILTER FilterFunciton[] = {NULL,DispatchFilterFunctionList};
    	VOID* FilterContext[] = {NULL,NvramCleanupFilterFunctions};
    	NVRAM_STORE_UPDATE_HANDLER* UpdateHandler[] = {NULL,DispatchCleanupHandlerList};
    	UINT32 i;
    	// Mark variable as deleted so that it's not copied into a new store.
    	if (Var!=NULL) Var->flags^= NVRAM_FLAG_VALID;
    	NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: SetVariable failed. Status=%r. Starting Recovery...\n",Status));
    	for(i=0; i<sizeof(FilterFunciton)/sizeof(FilterFunciton[0]); i++){
            if (i!=0){
                NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: SetVariable failed After Recovery. Clearing NVRAM...\n"));
            }
            NewInfo.NvramAddress = NULL;
            if (EFI_ERROR(CopyVarStoreToMemStore(Info,FilterFunciton[i],FilterContext[i],&NewInfo,UpdateHandler[i]))){
                if (NewInfo.NvramAddress!=NULL) NvBufferFree (NewInfo.NvramAddress);
                break;
            }
            if (Attributes & EFI_VARIABLE_APPEND_WRITE)
                Status = CreateVariableEx(
                            VariableName,VendorGuid,Attributes,
                            DataSize, Data, &NewInfo, NULL, &ExtSecFlags, 
                            OldData, OldDataSize
                        );
            else
                Status = CreateVariable(
                            VariableName,VendorGuid,Attributes,
                            DataSize, Data, &NewInfo, NULL, &ExtSecFlags
                        );         
                 
            if (   Status==EFI_OUT_OF_RESOURCES 
            	&& NvramCleanUpEnabled(VariableName,VendorGuid,Attributes,DataSize,OldDataSize)
            ){
				NvBufferFree (NewInfo.NvramAddress);
            	continue;
			}
            
            if (!EFI_ERROR(Status))
                Status = UpdateVarstore(Info,&NewInfo);
            
            NvBufferFree (NewInfo.NvramAddress);
            break;
        }
    	NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: SetVariable Status after recovery=%r.\n",Status));
    	
    	if (EFI_ERROR(Status)){
        	// Restore variable flags.
    		// We only have to do it in case of an error.
    		// In case of success, the store has already been updated with the new variable instance.
    		if (Var!=NULL) Var->flags^= NVRAM_FLAG_VALID;
        	// Make sure the store is healthy.
    		CheckStore(Info);
    	}
    }
    return Status;
}

// Top level functions
/**
    This function searches for Var with specific GUID and Name 
    beginning and ending critical section 

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var will be returned

    @retval EFI_STATUS based on result

**/
EFI_STATUS DxeGetVariableWrapper(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
)
{
    EFI_STATUS Status;
    if (!VariableName || !VendorGuid || !DataSize || !Data && *DataSize)
        return EFI_INVALID_PARAMETER;
    
    Status = GetVariableHook (
                 VariableName,VendorGuid,Attributes,DataSize,Data
             );
    if (Status == EFI_UNSUPPORTED)
    {
        Status = DxeGetVariable(
                     VariableName,VendorGuid,Attributes,DataSize,Data
                );
    }
    return Status;
}

/**
    This function searches for Var following after Var with specific name and GUID 
    and returns it's Name, beginning and ending critical section . 

    @param VariableNameSize size of Varname - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param VariableName pointer where Var Name in Unicode will be stored
    @param VendorGuid pointer to memory where Var GUID is stored

    @retval EFI_STATUS based on result

**/
EFI_STATUS DxeGetNextVariableNameWrapper(
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
)
{
    EFI_STATUS Status;
    if ( !VariableNameSize || !VariableName || !VendorGuid)
        return EFI_INVALID_PARAMETER;
    
    Status = GetNextVarNameHook (
                 VariableNameSize, VariableName, VendorGuid
             );
    if (Status == EFI_UNSUPPORTED) 
    {
        Status = DxeGetNextVariableName(
                     VariableNameSize,VariableName,VendorGuid
                 );
    }
    return Status;
}

VOID InitNvramLockBuffer(VOID* Address, UINTN Size){
	gNvramLockVarBuffer = Address;
	((LOCK_VAR_TABLE*)gNvramLockVarBuffer)->NextPtr = (UINT8*)gNvramLockVarBuffer + sizeof(LOCK_VAR_TABLE);
	((LOCK_VAR_TABLE*)gNvramLockVarBuffer)->TableSize = Size - sizeof(LOCK_VAR_TABLE);
	((LOCK_VAR_TABLE*)gNvramLockVarBuffer)->NumTableEntries = 0;
}

/**
    This function adds variable with passed Name and Guid the read-only variables list.

    @param VariableName - pointer to Var Name in Unicode
    @param VendorGuid - pointer to Var GUID

    @retval EFI_INVALID_PARAMETER - wrong parameters
    @retval EFI_OUT_OF_RESOURCES - no place in table left, or table was not init.
    @retval EFI_ACCESS_DENIED - to late to add entries.
    @retval EFI_SUCCESS - name and guid added successfully.

**/ 
EFI_STATUS DxeRequestToLock (
  IN CONST EDKII_VARIABLE_LOCK_PROTOCOL *This,
  IN       CHAR16                       *VariableName,
  IN       EFI_GUID                     *VendorGuid
  )
{
	LOCK_VAR_ENTRY *LockVar;
	LOCK_VAR_TABLE *Table;
	UINTN VarNameSize;
	UINTN MaxVariableNameSize;

    if (VariableName == NULL || VendorGuid == NULL || VariableName[0] == 0) return EFI_INVALID_PARAMETER;
    if (SmmLocked) return EFI_ACCESS_DENIED;
    if (gNvramLockVarBuffer == NULL) return EFI_OUT_OF_RESOURCES;
    
    Table = (LOCK_VAR_TABLE*)gNvramLockVarBuffer;
    MaxVariableNameSize = Table->TableSize - (Table->NextPtr - (UINT8*)Table);
    if (MaxVariableNameSize<=sizeof(LOCK_VAR_ENTRY)){
        ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        return EFI_OUT_OF_RESOURCES;
    }
    MaxVariableNameSize -= sizeof(LOCK_VAR_ENTRY);
    VarNameSize=GetVariableNameSize(VariableName,MaxVariableNameSize);
    if (VarNameSize > MaxVariableNameSize){
        ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        return EFI_OUT_OF_RESOURCES;
    }

    LockVar = (LOCK_VAR_ENTRY*)Table->NextPtr;
    LockVar->EntryLength = sizeof (LOCK_VAR_ENTRY) + VarNameSize;
    // Align entry length to next UINTN boundary to make sure all LockVar structures are naturally aligned
    LockVar->EntryLength =  ALIGN_VARIABLE(LockVar->EntryLength);
    MemCpy(&LockVar->Guid, VendorGuid, sizeof (EFI_GUID));
    MemCpy(LockVar+1, VariableName, VarNameSize);
    Table->NextPtr += LockVar->EntryLength;
    Table->NumTableEntries ++;
    return EFI_SUCCESS;
}

/**
    Search for VarName and VarGuid in Locked Variables list

    @param VarName pointer to Var Name in Unicode
    @param VarGuid pointer to Var GUID

    @retval EFI_WRITE_PROTECTED - variable with VarName and VarGuid is in the protected list
    @retval EFI_SUCCESS - variable with VarName and VarGuid not in the protected list

**/
EFI_STATUS CheckLockedVarList (CHAR16* VarName, EFI_GUID* VarGuid)

{
    LOCK_VAR_ENTRY *LockVar;
    UINTN i;
    LOCK_VAR_TABLE *LockTable;
    
    if (gNvramLockVarBuffer == NULL || !IsLockEnforcementActive() ) return EFI_SUCCESS;
    LockTable = (LOCK_VAR_TABLE*)gNvramLockVarBuffer;
    
    for (  i = 0, LockVar = (LOCK_VAR_ENTRY*)(LockTable+1) 
         ; i < LockTable->NumTableEntries 
         ; i++, LockVar = (LOCK_VAR_ENTRY*)( (UINT8*)LockVar + LockVar->EntryLength )
    ) {
        if (!guidcmp(&LockVar->Guid, VarGuid)){
            // Compare variable name strings.
            // We don't have to worry about LockVar + 1 alignment because all LockVar entries are naturally aligned
            //(see DxeRequestToLock).
            if (StrCmp((CHAR16*)(LockVar + 1), VarName) == 0){
                // Match found
                return EFI_WRITE_PROTECTED;
            }
        }
    }
    return EFI_SUCCESS; 
}

VOID LockBufferVirtualFixup(){
    if (gNvramLockVarBuffer == NULL ) return ;
    
    pRS->ConvertPointer(0,(VOID**)&((LOCK_VAR_TABLE*)gNvramLockVarBuffer)->NextPtr);
    pRS->ConvertPointer(0,(VOID**)&gNvramLockVarBuffer);
}

/**
    This function sets Var with specific GUID, Name and attributes
    beginning and ending critical section. 

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Attributes of the Var
    @param DataSize size of Var
    @param Data Pointer to memory where Var data is stored

    @retval EFI_STATUS based on result

**/
EFI_STATUS DxeSetVariableWrapper(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
    EFI_STATUS Status;

    Status = CheckOnSetVariable (
                VariableName,VendorGuid,Attributes,DataSize,Data
            );
    if (EFI_ERROR(Status)) return Status;
    Status = CheckLockedVarList (VariableName, VendorGuid);
    if (EFI_ERROR(Status)) return Status;
    Status = SetVariableHook (
                     VariableName,VendorGuid,Attributes,DataSize,Data
                 );
    if (Status == EFI_UNSUPPORTED)
    {
        Status = DxeSetVariable(
                     VariableName,VendorGuid,Attributes,DataSize,Data
                 );
    }
    return Status;
}

//Initialization
/**
    This function allocates memory and initializes NVRAM_STORE_INFO structure. 

    @param pInfo pointer to NVRAM store structure
    @param HeaderSize Size of the header 
    @param Flags default Flags

    @retval EFI_STATUS based on result

**/
EFI_STATUS InitTmpVolatileStore(
    NVRAM_STORE_INFO *Info, UINTN Size,
    UINT32 HeaderLength, UINT8 Flags,
    BOOLEAN Runtime
)
{                                      
    EFI_STATUS      Status = EFI_SUCCESS;

    Info->NvramAddress = NvBufferAllocate (Size);
    if (Info->NvramAddress == NULL) return EFI_OUT_OF_RESOURCES;
    Info->NvramSize = Size;
    MemSet(Info->NvramAddress,Info->NvramSize,FLASH_EMPTY_BYTE);
    NvInitInfoBuffer(
        Info,HeaderLength,Flags,&MemInterface
    );
    return EFI_SUCCESS;
}

/**
    This function searches for last Var and fills the NVRAM_STORE_INFO structure. 

    @param pInfo pointer to NVRAM store structure

    @retval VOID

**/
VOID EnumerateStore(NVRAM_STORE_INFO *Info)
{
    NVAR *Var, *LastVar;
    INT16 NextGuid;
    VAR_SIZE_TYPE LastVarSize;
    
    //Init NextGuid and pEndOfVars
    Var = (NVAR*)Info->pFirstVar;
    LastVar = Var;
    NextGuid=0;
    
    if (NvIsVariable(Var,Info))
    {
        //We assume that the first variable pInfo->pFirstVar is always valid
        //It's OK since we checked for pInfo->pFirstVar validity
        //during during NVRAM_STORE_INFO initialization in NvInitInfoBuffer routine
        for (; Var; Var=NvGetNextNvar(Var,Info))
        {
            if ( ( Var->flags&(NVRAM_FLAG_DATA_ONLY|NVRAM_FLAG_GUID) ) == 0 )
            {
                INT16 guid = *(UINT8*)(Var+1);
                
                if (guid>NextGuid) NextGuid=guid;
            }
            
            LastVar = Var;
        }
        
        NextGuid++;
        Info->LastVarSize = LastVar->size;
        Info->pEndOfVars = (UINT8*)LastVar+LastVar->size;
        Info->NextGuid = NextGuid;

        // Skip incomplete NVAR record at the end of the NVRAM area (if any)
        LastVar = (NVAR*)Info->pEndOfVars;
        LastVarSize = ReadUnaligned16(&LastVar->size);
        if (   LastVarSize != FLASH_EMPTY_SIZE
            &&     LastVarSize 
                <= (UINT8*)(Info->NvramGuidsAddress-NextGuid) - Info->pEndOfVars
        ){
            Info->LastVarSize += LastVarSize;
            Info->pEndOfVars += LastVarSize;
        }
    }
}

/**
    This function searches for nested Verstores and adds them to the NVRAM_STORE_INFO structure. 

    @param pInfo pointer to NVRAM store structure
    @param MemInfo pointer to NVRAM store structure in memory

    @retval VOID

**/
VOID UpdateNestedVarstores()
{
    UINT32 i=0;
    UINT32 DefaultStoreIndex;
    NVRAM_STORE_INFO *MainStore = VarStoreInfo.NvInfo;
    CHAR16* DefaultStoreName[] = {MfgDefaults,StdDefaults,NULL};
    DefaultStoreIndex = (VarStoreInfo.NvramMode & NVRAM_MODE_MANUFACTORING) ? 0 : 1;

    for(i=0; i<VarStoreInfo.InfoCount; i++){
        NVRAM_STORE_INFO *NestedStore = &VarStoreInfo.NvramInfo[i];
        if (   NestedStore->Flags 
            != (   NVRAM_STORE_FLAG_NON_VALATILE
                 | NVRAM_STORE_FLAG_READ_ONLY
                 | NVRAM_STORE_FLAG_DO_NOT_ENUMERATE
               )
        ) continue;
        if (NestedStore->NvramAddress < MainStore->NvramAddress)
            continue;
        if (NestedStore->NvramAddress > MainStore->NvramAddress+MainStore->NvramSize)
            continue;
        if (NvGetDefaultsInfo(DefaultStoreName[DefaultStoreIndex],MainStore,NestedStore) == NULL){
            NestedStore->NvramAddress=MainStore->NvramAddress;
            NestedStore->NvramSize=0;
            NvResetInfoBuffer(NestedStore);
            DefaultStoreIndex++;
            if (DefaultStoreName[DefaultStoreIndex]==NULL) return;
        }
    }
}

/**
    This function searches for nested Verstores and adds them to the NVRAM_STORE_INFO structure. 

    @param pInfo pointer to NVRAM store structure
    @param MemInfo pointer to NVRAM store structure in memory

    @retval VOID

**/
VOID VarStoreDiscovery(NVRAM_STORE_INFO *NvInfo)
{
    VarStoreInfo.InfoCount = 0;
    VarStoreInfo.LastInfoIndex = 0;
    
    //Handle Manufacturing mode
    if (   VarStoreInfo.NvramMode & NVRAM_MODE_MANUFACTORING
            && NvGetDefaultsInfo(
                MfgDefaults,NvInfo,&VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount]
            ) != NULL
       )
    {
        VarStoreInfo.InfoCount++;
    }
    
    //Handle Default config mode
    if (!(VarStoreInfo.NvramMode & NVRAM_MODE_DEFAULT_CONFIGURATION))
    {
        //add regular NVRAM
        VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount]=*NvInfo;
        VarStoreInfo.NvInfo = &VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount];
        if (VarStoreInfo.NvramMode & NVRAM_MODE_MANUFACTORING)
    		VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount].Flags |= NVRAM_STORE_FLAG_DO_NOT_ENUMERATE;
        VarStoreInfo.InfoCount++;
        
        //add defaults
        if (NvGetDefaultsInfo(
                    StdDefaults,NvInfo,&VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount]
                ) != NULL
           )
        {
            VarStoreInfo.InfoCount++;
        }
    }
    
    else
    {
        //add defaults
        if (NvGetDefaultsInfo(
                    StdDefaults,NvInfo,&VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount]
                ) != NULL
           )
        {
            VarStoreInfo.InfoCount++;
        }
        
        //add regular NVRAM
        VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount]=*NvInfo;
        VarStoreInfo.NvInfo = &VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount];
        VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount].Flags |= NVRAM_STORE_FLAG_DO_NOT_ENUMERATE;
        VarStoreInfo.InfoCount++;
    }
    
    //Init NextGuid and pEndOfVars
    EnumerateStore(VarStoreInfo.NvInfo);
}

////////////////////////////////////////////////////////////////////
/**
    This function returns parameters of VarStore with passed attributes

    @param Attributes Attributes to search for
    @param MaximumVariableStorageSize Maximum Variable Storage Size
    @param RemainingVariableStorageSize Remaining Variable Storage Size
    @param MaximumVariableSize Maximum Variable Size

    @retval EFI_STATUS based on result

**/
EFI_STATUS QueryVariableInfo (
    IN UINT32 Attributes,
    OUT UINT64 *MaximumVariableStorageSize,
    OUT UINT64 *RemainingVariableStorageSize,
    OUT UINT64 *MaximumVariableSize
)
{
    UINT64 StorageSize,RemainingStorage,MaxVarSize;
    NVRAM_STORE_INFO *Info;
    UINTN SizeOfVarSizeType = MAX_NVRAM_VARIABLE_SIZE;
    NVRAM_STORE_INFO NewInfo;
    EFI_STATUS Status;
    
    if ( !Attributes || (Attributes & ~ALL_VARIABLE_ATTRIBUTES)
            ||(Attributes & EFI_VARIABLE_RUNTIME_ACCESS) && !(Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS )
            || !MaximumVariableStorageSize || !RemainingVariableStorageSize || !MaximumVariableSize
       )
        return EFI_INVALID_PARAMETER;
        
    Info =   (Attributes & EFI_VARIABLE_NON_VOLATILE)
             ? VarStoreInfo.NvInfo : VarStoreInfo.MemInfo;
    StorageSize=Info->NvramSize;
    // Perform Garbage Collection using a memory buffer to figure out how much free
    // space will be available is we remove stale records.
    NewInfo.NvramAddress = NULL;
    if (EFI_ERROR(Status = CopyVarStoreToMemStore(Info,NULL,NULL,&NewInfo,NULL))){
        if (NewInfo.NvramAddress!=NULL) NvBufferFree (NewInfo.NvramAddress);
        return Status;
    }
    RemainingStorage=(UINT8*)(
                         (EFI_GUID*)(NewInfo.NvramAddress+NewInfo.NvramSize)
                         - NewInfo.NextGuid-1
                     )-NewInfo.pEndOfVars;
    NvBufferFree (NewInfo.NvramAddress);
    MaxVarSize=  (SizeOfVarSizeType>RemainingStorage)
                 ? RemainingStorage : SizeOfVarSizeType;
    MaxVarSize -=  sizeof(NVAR) + sizeof(EFI_GUID);
    
    if ((INTN)MaxVarSize<0) MaxVarSize=0;
    
    *MaximumVariableStorageSize = StorageSize;
    *RemainingVariableStorageSize = RemainingStorage;
    *MaximumVariableSize = MaxVarSize;
    return EFI_SUCCESS;
}

/**
    This function will be called when Exit BS will signaled
    will update data to work in RunTime.

    @param Event signaled event
    @param Context calling context

    @retval VOID

**/

VOID SwitchToRuntime ()
{
    Runtime = TRUE;
}

VOID SwitchToBds ()
{
	SmmLocked = TRUE;
}

BOOLEAN IsSmmLocked () { return SmmLocked; }

VOID ResetVarStore(NVRAM_STORE_INFO *Info){
    NvResetInfoBuffer(Info);
    EnumerateStore(Info);
}

// Do not use this function.
// The function is included for backward compatibility. 
// Some projects contain project specific code that depends on this function.
EFI_STATUS ShowBootTimeVariables (BOOLEAN Show){
    HideBtVariables = !Show;
    NVRAM_TRACE((-1,"Setting HideBtVariables to %d\n",HideBtVariables));
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI AmiNvramUpdateProtocolUpdateNvram(
	AMI_NVRAM_UPDATE_PROTOCOL *This,
	VOID *NvramBuffer, UINTN NvramBufferSize, BOOLEAN UpdateNvram
){
    NVRAM_STORE_INFO NewInfo;
    UINT32 HeaderSize;
    EFI_STATUS Status;

	if (NvramBuffer==NULL || NvramBufferSize==0) return EFI_INVALID_PARAMETER;
	if (UpdateNvram && NvramBufferSize!=VarStoreInfo.NvInfo->NvramSize) return EFI_BAD_BUFFER_SIZE;

    if (!CheckTheHeader(NvramBuffer,NvramBufferSize,FALSE)){
        // NvramBuffer is not a valid NVRAM buffer
    	NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: ERROR: AmiNvramUpdate->UpdateNvram has failed. The provided NVRAM buffer is invalid (no valid NVRAM FV header is detected).\n"));
        return EFI_VOLUME_CORRUPTED;
    }
    HeaderSize = GetNvStoreHeaderSize(NvramBuffer);
    if (HeaderSize==0){
    	NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: ERROR: AmiNvramUpdate->UpdateNvram has failed. The provided NVRAM buffer is invalid (no valid NVRAM FV header is detected).\n"));
    	return EFI_VOLUME_CORRUPTED;
    }
    NewInfo.NvramAddress = NvramBuffer;
    NewInfo.NvramSize = NvramBufferSize;
    NvInitInfoBuffer(&NewInfo, HeaderSize, VarStoreInfo.NvInfo->Flags, &MemInterface);
    EnumerateStore(&NewInfo);
    Status = CopyVariables(VarStoreInfo.NvInfo,&NewInfo,DispatchFilterFunctionList,NvramUpdateFilterFunctions,DispatchUpdateHandlerList);
    if (EFI_ERROR(Status)) return Status;
    if (UpdateNvram){
        NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: Updating NVRAM via AmiNvramUpdate->UpdateNvram call...\n"));
    	Status = UpdateVarstore(VarStoreInfo.NvInfo,&NewInfo);
    }
    return Status;
}

EFI_STATUS EFIAPI AmiNvramUpdateProtocolMigrateNvram(
    AMI_NVRAM_UPDATE_PROTOCOL *This,
    UINT64 NvramAddress, UINT64 NvramBackupAddress, UINTN NvramSize
){
    NVRAM_STORE_INFO NewInfo;
    NVRAM_STORE_INFO *Info;
    EFI_STATUS Status;
    
    if (NvramSize != VarStoreInfo.NvInfo->NvramSize) return EFI_UNSUPPORTED;
    if (VarStoreInfo.NvInfo->Interface != &HybridInterface.Interface) return EFI_UNSUPPORTED;
    if (NvramAddress > MAX_UINTN || NvramBackupAddress > MAX_UINTN) return EFI_INVALID_PARAMETER;
    
    HybridInterface.NvramFlashAddress = (UINT8*)(UINTN)NvramAddress;
    HybridInterface.BackupAddress = (UINT8*)(UINTN)NvramBackupAddress;
    
    Info = VarStoreInfo.NvInfo;
    Status = InitTmpVolatileStore(
                 &NewInfo, Info->NvramSize, Info->HeaderLength, Info->Flags, FALSE
             );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    MemCpy(NewInfo.NvramAddress,Info->NvramAddress,Info->NvramSize);
    ResetVarStore(&NewInfo);
    Status = Info->Interface->Reinitialize(Info->Interface, Info->NvramAddress, Info->NvramSize, FALSE, &Info->HeaderLength);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)){
        NvBufferFree (NewInfo.NvramAddress);
        return Status;
    }
    ResetVarStore(Info);
    #ifdef EFI_DEBUG
    if (Status==NVRAM_WARN_STORE_REPAIRED) 
        NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: AmiNvramUpdate->MigrateNvram detected and fixed corruption of the new NVRAM flash area.\n"));
    #endif

    NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: AmiNvramUpdate->MigrateNvram: Appending persistent variables...\n"));
    Status = CopyVariables(&NewInfo,Info,DispatchFilterFunctionList,NvramUpdateFilterFunctions,DispatchUpdateHandlerList);
    ASSERT_EFI_ERROR(Status);
    NvBufferFree (NewInfo.NvramAddress);
    VarStoreInfo.LastInfoIndex = 0;
    UpdateNestedVarstores();
    
    return Status;
}

AMI_NVRAM_UPDATE_PROTOCOL AmiNvramUpdate = {AmiNvramUpdateProtocolUpdateNvram, AmiNvramUpdateProtocolMigrateNvram};

EFI_STATUS InitNonVolatileVarStore(NVRAM_HOB *NvramHob, UINTN DefaultNvramSize){
    NVRAM_STORE_INFO NvStore;
    NVRAM_STORE_INFO DefaultsInfo;
    BOOLEAN UseExternalDefaults=FALSE;
    UINT32 ResetPropertyMask = 0;

    UINT8* MainAddress;
    UINT8* BackupAddress;
    EFI_STATUS Status;

    if (NvramHob !=NULL)
    {
        VarStoreInfo.NvramMode = NvramHob->NvramMode;
        NvStore.NvramSize = NvramHob->NvramSize;
        MainAddress = (UINT8*)(UINTN)NvramHob->NvramAddress;
        BackupAddress = (UINT8*)(UINTN)NvramHob->BackupAddress;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.		
        if (MainAddress == NULL || NvStore.NvramSize==0){
            NVRAM_TRACE((DEBUG_ERROR,"NVRAM: ERROR: NVRAM HOB contains invalid data: main store address: %p; NVRAM size: %X\n", MainAddress, NvStore.NvramSize));
            return EFI_INVALID_PARAMETER;
        }
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.		
        NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: main store address: %lX; back up store address: %lX\n", NvramHob->NvramAddress, NvramHob->BackupAddress));
    }
    else
    {
        VarStoreInfo.NvramMode = NVRAM_MODE_SIMULATION;
        NvStore.HeaderLength = 0;
        NvStore.NvramSize = DefaultNvramSize;
        MainAddress = NULL;
        NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: NVRAM HOB is not found. Switching to simulation mode.\n"));
    }
    NvStore.NvramAddress = NvramAllocatePages(NvStore.NvramSize);
    if (NvStore.NvramAddress == NULL){
    	ASSERT(EFI_OUT_OF_RESOURCES);
    	return EFI_OUT_OF_RESOURCES;
    }
    if (MainAddress != NULL){
        AMI_FLASH_PROTOCOL *Flash = LocateFlashProtocol();
        if (Flash==NULL) return EFI_NOT_FOUND;
        Status = InitHybridInterface(
            &HybridInterface,NvStore.NvramAddress,NvStore.NvramSize, 
            (VarStoreInfo.NvramMode & NVRAM_MODE_SIMULATION)!=0,
            MainAddress,BackupAddress,Flash,&NvStore.HeaderLength
        );
        if (EFI_ERROR(Status)) return Status;
        NvStore.Interface = &HybridInterface.Interface;
    }else{
        MemSet(NvStore.NvramAddress,NvStore.NvramSize,FLASH_EMPTY_BYTE);
        Status = EFI_SUCCESS;
    }
    if (VarStoreInfo.NvramMode & NVRAM_MODE_SIMULATION){
        NvStore.Interface = &MemInterface;
        NVRAM_TRACE((TRACE_DXE_CORE,"NVRAM: working in simulation mode\n"));
    }
    NvStore.Flags = NVRAM_STORE_FLAG_NON_VALATILE;
    NvResetInfoBuffer(&NvStore);

    //If built-in defaults are not found,
    //reset NVRAM data; use external defaults.
    if ( !EFI_ERROR(Status) && NvGetDefaultsInfo(StdDefaults, &NvStore, &DefaultsInfo) == NULL ){
        UseExternalDefaults = TRUE;
        ResetPropertyMask |= NVRAM_RESET_KEEP_VARIABLES_WITHOUT_DEFAULTS;
        VarStoreInfo.NvramMode |= NVRAM_MODE_RESET_CONFIGURATION;
    }
    //Handle NVRAM reset
    if (VarStoreInfo.NvramMode & NVRAM_MODE_RESET_CONFIGURATION)
    {
    	ResetPropertyMask |= PcdGet32(AmiPcdNvramResetPropertyMask);
        NvResetConfiguration(
            &NvStore,
            UseExternalDefaults, ResetPropertyMask
        );
        VarStoreInfo.NvramMode&=~NVRAM_MODE_RESET_CONFIGURATION;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.		
        if (NvramHob !=NULL) NvramHob->NvramMode&=~NVRAM_MODE_RESET_CONFIGURATION;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.			
    }
    VarStoreDiscovery(&NvStore);
    
    return EFI_SUCCESS;
}

EFI_STATUS InitVolatileVarStore(NVRAM_STORE_INFO *MemInfo, UINTN VolatileVariableStoreSize){
    MemInfo->NvramAddress = NvramAllocatePages(VolatileVariableStoreSize);
    if (MemInfo->NvramAddress == NULL){
    	ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
    	return EFI_OUT_OF_RESOURCES;
    }

    MemInfo->NvramSize = VolatileVariableStoreSize;
    MemSet(MemInfo->NvramAddress,MemInfo->NvramSize,FLASH_EMPTY_BYTE);
    NvInitInfoBuffer(
        MemInfo,0,0,&MemInterface
    );
    VarStoreInfo.MemInfo=MemInfo;
    return EFI_SUCCESS;
}

/**
    This function initializes Varstore

    @param VOID

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvramInitialize()
{
    EFI_STATUS Status;
    UINT32 i;
    UINTN Size=0;

    NVRAM_HOB *NvramHob;
    extern const UINTN NvramSize;
    BOOLEAN SizeChanged;
    UINT32 VolatileVariableStoreSize = PcdGet32(PcdVariableStoreSize);
    
    NvramHob = (NVRAM_HOB*)GetEfiConfigurationTable(pST,&gEfiHobListGuid);
    if (NvramHob !=NULL && EFI_ERROR(FindNextHobByGuid((EFI_GUID*)&AmiNvramHobGuid, (VOID**)&NvramHob)))
        NvramHob = NULL;
    Size = (NvramHob==NULL) ? NvramSize : NvramHob->NvramSize;
    if (Size < VolatileVariableStoreSize) Size = VolatileVariableStoreSize;
    MemSet(&VarStoreInfo,sizeof(VarStoreInfo),0);
    VarStoreInfo.NvramDriverBufferSize = (UINT32)Size * 3;
    
    VarStoreInfo.NvramDriverBuffer = (UINTN)NvramAllocatePages(VarStoreInfo.NvramDriverBufferSize);
    if (VarStoreInfo.NvramDriverBuffer == 0){
    	ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
    	return EFI_OUT_OF_RESOURCES;
    }
    
    Status = InitNonVolatileVarStore(NvramHob,NvramSize);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    Status = InitVolatileVarStore(&VarStoreInfo.NvramInfo[VarStoreInfo.InfoCount],VolatileVariableStoreSize);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    VarStoreInfo.InfoCount++;

    SizeChanged = FALSE;
    for(i=0;i<VarStoreInfo.InfoCount;i++){
        if (VarStoreInfo.NvramInfo[i].Flags & NVRAM_STORE_FLAG_READ_ONLY)
            continue;
        if (Size < VarStoreInfo.NvramInfo[i].NvramSize){
            Size=VarStoreInfo.NvramInfo[i].NvramSize;
            SizeChanged = TRUE;
        }
    }
    if (SizeChanged){
        NvramFreePages((VOID*)VarStoreInfo.NvramDriverBuffer,VarStoreInfo.NvramDriverBufferSize);
        VarStoreInfo.NvramDriverBufferSize = (UINT32)Size * 3;
        VarStoreInfo.NvramDriverBuffer = (UINTN)NvramAllocatePages(VarStoreInfo.NvramDriverBufferSize);
        if (VarStoreInfo.NvramDriverBuffer == 0){
        	ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        	return EFI_OUT_OF_RESOURCES;
        }
    }
    return EFI_SUCCESS;
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
