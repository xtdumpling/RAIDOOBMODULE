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

#include "NVRAM.h"
#include <AmiDxeLib.h>
#include <Library/AmiCriticalSectionLib.h>
#include <Protocol/AmiFlash.h>
#include <Protocol/VariableLock.h>
#include <Protocol/AmiNvramUpdate.h>
#include <Guid/GlobalVariable.h>
#include <Token.h>

#define BEGIN_CRITICAL_SECTION(Cs) \
    { EFI_STATUS Status = BeginCriticalSection(Cs);\
      ASSERT(Status==EFI_SUCCESS || Status==EFI_ACCESS_DENIED);\
      if (EFI_ERROR(Status)) return Status;\
    }
#define END_CRITICAL_SECTION(Cs) VERIFY_EFI_ERROR(EndCriticalSection(Cs))

#ifdef EFI_DEBUG
#define NVRAM_TRACE(Arguments) { if (!IsNvramRuntime()) TRACE(Arguments); }
#else
#define NVRAM_TRACE(Arguments)
#endif

#define NVRAM_RESET_KEEP_VARIABLES_WITHOUT_DEFAULTS 1
#define NVRAM_RESET_KEEP_PERSISTENT_VARIABLES 2

typedef struct
{
    UINT32 NvramMode;
    UINT32 InfoCount, LastInfoIndex;
    NVRAM_STORE_INFO NvramInfo[10];
    NVRAM_STORE_INFO *NvInfo, *MemInfo;
    // Runtime buffer for use in Runtime with the size of 3 times NVRAM_SIZE
    // First part (2 times NVRAM_SIZE) usually used for TmpBuffer, while last part for VolatileStore.
    EFI_PHYSICAL_ADDRESS NvramDriverBuffer;
    UINT32 NvramDriverBufferSize;     
} VARSTORE_INFO;

typedef struct
{
    NVRAM_STORE_INTERFACE Interface;
    UINT8 *BackupAddress;
    UINTN AddressDelta;
    UINT8* NvramFlashAddress;
    AMI_FLASH_PROTOCOL *Flash;
} HYBRID_NVRAM_STORE_INTERFACE;

#define HOOK_GET_VARIABLE_PARAMS\
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,\
    OUT UINT32 *Attributes OPTIONAL,\
    IN OUT UINTN *DataSize, OUT VOID *Data
typedef EFI_STATUS (HOOK_GET_VARIABLE)(HOOK_GET_VARIABLE_PARAMS);

#define HOOK_SET_VARIABLE_PARAMS\
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,\
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
typedef EFI_STATUS (HOOK_SET_VARIABLE)(HOOK_SET_VARIABLE_PARAMS);

#define HOOK_GET_NEXT_VARIABLE_NAME_PARAMS\
    IN OUT UINTN *VariableNameSize,\
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
typedef EFI_STATUS (HOOK_GET_NEXT_VARIABLE_NAME)(HOOK_GET_NEXT_VARIABLE_NAME_PARAMS);

typedef EFI_STATUS (EFIAPI *NVRAM_MAILBOX_SWITCH_TO_SMM_WRAPPER_FUNCTION)();
typedef struct {
    VARSTORE_INFO *VarStoreInfoPtr;
    NVRAM_MAILBOX_SWITCH_TO_SMM_WRAPPER_FUNCTION SwitchToSmmWrapper;
} NVRAM_MAILBOX;

extern VARSTORE_INFO VarStoreInfo;
extern NVRAM_STORE_INTERFACE MemInterface;

extern HOOK_GET_VARIABLE* GetVariableHookList[];
extern HOOK_SET_VARIABLE* SetVariableHookList[];
extern HOOK_GET_NEXT_VARIABLE_NAME* GetNextVarNameHookList[];

extern AMI_NVRAM_UPDATE_PROTOCOL AmiNvramUpdate;

EFI_STATUS DxeGetVariableWrapper(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
);

EFI_STATUS DxeGetNextVariableNameWrapper(
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
);

EFI_STATUS DxeSetVariableWrapper(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);

EFI_STATUS QueryVariableInfo (
    IN UINT32 Attributes,
    OUT UINT64 *MaximumVariableStorageSize,
    OUT UINT64 *RemainingVariableStorageSize,
    OUT UINT64 *MaximumVariableSize
);

EFI_STATUS DxeRequestToLock (
	IN CONST EDKII_VARIABLE_LOCK_PROTOCOL *This,
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid
);

VOID SwitchToRuntime ();
VOID SwitchToBds ();
BOOLEAN IsSmmLocked ();
BOOLEAN IsLockEnforcementActive ();
VOID InitNvramLockBuffer(VOID* Address, UINTN Size);
VOID ResetVarStore(NVRAM_STORE_INFO *Info);
UINTN GetVariableNameSize(IN CONST CHAR16 *String, IN UINTN MaxSize);
EFI_STATUS NvramInitialize();

VOID* NvramAllocatePages(UINTN Size);
VOID NvramFreePages(VOID *Address, UINTN Size);
AMI_FLASH_PROTOCOL *LocateFlashProtocol();
VOID LockBufferVirtualFixup();
VOID UpdateNestedVarstores();

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
