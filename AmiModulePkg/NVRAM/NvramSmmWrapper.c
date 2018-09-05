//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
/** @file
    NvramSmi helps secure a system by providing a set of variable service functions 
	which route variable access through the SMM interface.
**/

#include <AmiDxeLib.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/VariableLock.h>
#include <Guid/AmiNvram.h>
#include "NvramSmi.h"
#include "NvramDxeCommon.h"

typedef struct {
    /// Allows for disambiguation of the message format.
    EFI_GUID  HeaderGuid;
    /// Describes the size of Data (in bytes) and does not include the size of the header.
    UINTN     MessageLength;
    // NOTE: The compiler will insert 4 byte padding in this place in 32-bit mode (the structure is naturally aligned).
    // We're skipping it in SMI handler prior to processing (see NvramSmiHandlerComBuffer in NvramSmi.c)
   	UINT64 Control;
}NVRAM_COMMUNICATION_BUFFER;

// initialized in ActivateSmmWrapper
EFI_SMM_COMMUNICATION_PROTOCOL *SmmCommProtocol = NULL;
// initialized in NvramSmiDxeEntry
NVRAM_COMMUNICATION_BUFFER *NvramSmmCommunicationBuffer = NULL;
VOID *NvramSmmCommunicationBufferPhysicalAddress = NULL;
UINTN NvramSmmCommunicationBufferSize = 0;
UINTN CommunicationHeaderSize = 0; 
UINTN MaxMessageLength = 0;

//Defined in NvramDxe.c
VOID SwitchToSmmWrapper ();

//----------------------------------------------------------------------------
// Function Definitions

/**
    This function will be called to Activate NVRAM SMM API

    @param Event signaled event
    @param Context calling context

    @retval EFI_STATUS

**/
EFI_STATUS EFIAPI ActivateSmmWrapper(){
    EFI_STATUS Status;
    if (SmmCommProtocol != NULL) return EFI_ALREADY_STARTED;
    if (NvramSmmCommunicationBuffer == NULL) return EFI_UNSUPPORTED;
    // The SmmCommunication protocol must be available at this time.
    // This function runs when gAmiNvramSmmCommunicationGuid is installed.
    // The protocol is installed by NVRAMSMM driver, which has dependency on SmmCommunication protocol.
    Status = pBS->LocateProtocol ( &gEfiSmmCommunicationProtocolGuid, NULL, &SmmCommProtocol );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    SwitchToSmmWrapper();
    return EFI_SUCCESS;
}

/**
    TThis function initiates communication between VAR services outside
    of SMM and SMI handlers inside of SMM. With the help of Smm Communication
    Protocol, if it is present, or direct I/O write, if not.

    @param VOID

    @retval VOID

**/
EFI_STATUS Communicate (UINTN MessageLength){
    UINTN CommSize;
    UINT64 Control; 
    EFI_STATUS Status;
    
    if (SmmCommProtocol==NULL) return EFI_UNSUPPORTED;
    if (   NvramSmmCommunicationBuffer == NULL 
    	|| NvramSmmCommunicationBufferPhysicalAddress == NULL
    ) return EFI_OUT_OF_RESOURCES;
    if (MessageLength > MaxMessageLength) return EFI_OUT_OF_RESOURCES;

    Control = NvramSmmCommunicationBuffer->Control;
   	NvramSmmCommunicationBuffer->MessageLength = MessageLength;
   	CommSize = CommunicationHeaderSize + MessageLength;
    Status = SmmCommProtocol->Communicate (SmmCommProtocol, NvramSmmCommunicationBufferPhysicalAddress, &CommSize);

    if (EFI_ERROR(Status)) return Status;
    if (NvramSmmCommunicationBuffer->Control == Control)
            return EFI_NO_RESPONSE;
    if ((NvramSmmCommunicationBuffer->Control & NVRAM_SMM_ERROR_BIT)!=0)
        Status = NVRAM_SMM_STATUS_TO_EFI_STATUS(NvramSmmCommunicationBuffer->Control);
    return Status;
}

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

EFI_STATUS DxeGetVariableSmmWrapper (
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
)
{
    EFI_STATUS          Status;
    UINTN               AvailableBufferSize, VariableNameSize, VariableDataSize;
    SMI_GET_VARIABLE_BUFFER *GetBuffer;

    if (NvramSmmCommunicationBuffer == NULL) return EFI_UNSUPPORTED;
    if (!VariableName || !VendorGuid || !DataSize || !Data && *DataSize)
        return EFI_INVALID_PARAMETER;
    
    AvailableBufferSize = MaxMessageLength - sizeof(SMI_GET_VARIABLE_BUFFER);
    VariableNameSize = GetVariableNameSize(VariableName, AvailableBufferSize);
    VariableDataSize = *DataSize;
    
    // If variable name is too large to fit into our buffer, it is also too large to fit
    // into NVRAM store.
    if (AvailableBufferSize < VariableNameSize) return EFI_NOT_FOUND;
    
    AvailableBufferSize -= VariableNameSize;
    // If user data buffer is larger than our internal buffer, trim the buffer size.
    if (AvailableBufferSize < VariableDataSize) VariableDataSize = AvailableBufferSize;

    GetBuffer = (SMI_GET_VARIABLE_BUFFER *)&NvramSmmCommunicationBuffer->Control;
    GetBuffer->Control = NVRAM_SMM_COMMAND_GET_VARIABLE;
    GetBuffer->DataSize = VariableDataSize;
    GetBuffer->Guid = *VendorGuid;
    GetBuffer->VariableNameSize = VariableNameSize;
    MemCpy(GetBuffer+1, VariableName, VariableNameSize);
    
    Status = Communicate( sizeof(SMI_GET_VARIABLE_BUFFER) + VariableNameSize + VariableDataSize );
    if (!EFI_ERROR(Status) || Status==EFI_BUFFER_TOO_SMALL){
        *DataSize = (UINTN)GetBuffer->DataSize;
        if (!EFI_ERROR(Status)){
            if (Attributes!=NULL) *Attributes = GetBuffer->Attributes;
            MemCpy(Data, (UINT8*)(GetBuffer+1)+GetBuffer->VariableNameSize, *DataSize);
        }
    }
    return Status;
}

/**
    This function searches for next Var after Var with specific name and GUID and returns it's Name
    in SMM synchronizing Varstors before and after operation.

    @param VariableNameSize size of Varname - if smaller than actual EFI_BUFFER_TOO_SMALL
        will be returned and DataSize will be set to actual size needed
    @param VariableName pointer where Var Name in Unicode will be stored
    @param VendorGuid pointer to menory where Var GUID is stored

    @retval EFI_STATUS based on result

**/
EFI_STATUS DxeGetNextVariableNameSmmWrapper (
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
)
{
    EFI_STATUS          Status;
    UINTN               AvailableBufferSize, VariableNameStrSize, VariableNameBufferSize;
    SMI_GET_NEXT_VARIABLE_NAME_BUFFER *GetNextBuffer;
	
    if (NvramSmmCommunicationBuffer == NULL) return EFI_UNSUPPORTED;
    if ( !VariableNameSize || !VariableName || !VendorGuid)
        return EFI_INVALID_PARAMETER;

    AvailableBufferSize = MaxMessageLength - sizeof(SMI_GET_NEXT_VARIABLE_NAME_BUFFER);
    VariableNameStrSize = GetVariableNameSize(VariableName, AvailableBufferSize);
    VariableNameBufferSize = *VariableNameSize;
    // Size of an empty Unicode string (just the NULL terminator) is two bytes.
    // Buffer size less than two bytes is a bug on a caller side.
    // We're providing a workaround.
    if (VariableNameBufferSize < 2) VariableNameBufferSize=2;
    
    if (VariableNameStrSize > VariableNameBufferSize) return EFI_INVALID_PARAMETER;
    
    // If variable name is too large to fit into our buffer, it is also too large to fit
    // into NVRAM store.
    if (AvailableBufferSize < VariableNameStrSize) return EFI_NOT_FOUND;
    
    AvailableBufferSize -= VariableNameStrSize;
    // If user variable name buffer is larger than our internal buffer, trim the buffer size.
    if (AvailableBufferSize < VariableNameBufferSize) VariableNameBufferSize = AvailableBufferSize;

    GetNextBuffer = (SMI_GET_NEXT_VARIABLE_NAME_BUFFER *)&NvramSmmCommunicationBuffer->Control;
    GetNextBuffer->Control = NVRAM_SMM_COMMAND_GET_NEXT_VARIABLE_NAME;
    GetNextBuffer->DataSize = VariableNameBufferSize;
    GetNextBuffer->Guid = *VendorGuid;
    MemCpy(GetNextBuffer+1, VariableName, VariableNameStrSize);

    Status = Communicate( sizeof(SMI_GET_NEXT_VARIABLE_NAME_BUFFER) + VariableNameBufferSize );
    if (!EFI_ERROR(Status) || Status==EFI_BUFFER_TOO_SMALL){
        *VariableNameSize = (UINTN)GetNextBuffer->DataSize;
        if (!EFI_ERROR(Status)){
            MemCpy(VariableName, GetNextBuffer+1, *VariableNameSize);
            *VendorGuid = GetNextBuffer->Guid;
        }
    }
    return Status;
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
EFI_STATUS DxeSetVariableSmmWrapper (
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
    EFI_STATUS Status;
    UINTN AvailableBufferSize, VariableNameSize;
    SMI_SET_VARIABLE_BUFFER *SetBuffer;

    if (NvramSmmCommunicationBuffer == NULL) return EFI_UNSUPPORTED;
    if (!VariableName || !VendorGuid || (DataSize && !Data))
        return EFI_INVALID_PARAMETER;
    
    AvailableBufferSize = MaxMessageLength - sizeof(SMI_SET_VARIABLE_BUFFER);
    VariableNameSize = GetVariableNameSize(VariableName, AvailableBufferSize);
    
    // If variable name or data is too large to fit into our buffer, it is also too large to fit
    // into NVRAM store.
    if (AvailableBufferSize < VariableNameSize) return EFI_OUT_OF_RESOURCES;
    AvailableBufferSize -= VariableNameSize;
    if (AvailableBufferSize < DataSize) return EFI_OUT_OF_RESOURCES;

    SetBuffer = (SMI_SET_VARIABLE_BUFFER *)&NvramSmmCommunicationBuffer->Control;
    SetBuffer->Control = NVRAM_SMM_COMMAND_SET_VARIABLE;
    SetBuffer->Attributes = Attributes;
    SetBuffer->DataSize = DataSize;
    SetBuffer->Guid = *VendorGuid;
    SetBuffer->VariableNameSize = VariableNameSize;
    MemCpy(SetBuffer+1, VariableName, VariableNameSize);
    MemCpy((UINT8*)(SetBuffer+1)+VariableNameSize, Data, DataSize);
    
    Status = Communicate( sizeof(SMI_SET_VARIABLE_BUFFER) + VariableNameSize + DataSize );

    return Status;
}

/**
    This function returns parameters of VarStore with passed attributes

    @param Attributes Attributes to search for
    @param MaximumVariableStorageSize Maximum Variable Storage Size
    @param RemainingVariableStorageSize Remaining Variable Storage Size
    @param MaximumVariableSize Maximum Variable Size

    @retval EFI_STATUS based on result

**/
EFI_STATUS QueryVariableInfoSmmWrapper (
    IN UINT32 Attributes,
    OUT UINT64 *MaximumVariableStorageSize,
    OUT UINT64 *RemainingVariableStorageSize,
    OUT UINT64 *MaximumVariableSize
)
{
    EFI_STATUS Status;
    SMI_QUERY_VARIABLE_INFO_BUFFER *QueryVarInfoBuffer;
    
    if (NvramSmmCommunicationBuffer == NULL) return EFI_UNSUPPORTED;
    if (!Attributes || !MaximumVariableStorageSize || !RemainingVariableStorageSize || !MaximumVariableSize)
        return EFI_INVALID_PARAMETER;
    
    QueryVarInfoBuffer = (SMI_QUERY_VARIABLE_INFO_BUFFER *)&NvramSmmCommunicationBuffer->Control;
    QueryVarInfoBuffer->Control = NVRAM_SMM_COMMAND_QUERY_VARIABLE_INFO;
    QueryVarInfoBuffer->Attributes = Attributes;
    Status = Communicate( sizeof(SMI_QUERY_VARIABLE_INFO_BUFFER) );
    if (EFI_ERROR(Status)) return Status;
    *MaximumVariableStorageSize = QueryVarInfoBuffer->MaximumVariableStorageSize;
    *RemainingVariableStorageSize = QueryVarInfoBuffer->RemainingVariableStorageSize;
    *MaximumVariableSize = QueryVarInfoBuffer->MaximumVariableSize;
    return Status;
}

EFI_STATUS RequestToLockSmmWrapper (
  IN CONST EDKII_VARIABLE_LOCK_PROTOCOL *This,
  IN       CHAR16                       *VariableName,
  IN       EFI_GUID                     *VendorGuid
  ){
    UINTN               AvailableBufferSize, VariableNameSize;
    SMI_REQUEST_TO_LOCK_BUFFER *LockBuffer;
    
    if (NvramSmmCommunicationBuffer == NULL) return EFI_UNSUPPORTED;
    if ( !VariableName || !VendorGuid) return EFI_INVALID_PARAMETER;

    AvailableBufferSize = MaxMessageLength - sizeof(SMI_REQUEST_TO_LOCK_BUFFER);
    VariableNameSize = GetVariableNameSize(VariableName, AvailableBufferSize);
    if (AvailableBufferSize < VariableNameSize) return EFI_OUT_OF_RESOURCES;
    
    LockBuffer = (SMI_REQUEST_TO_LOCK_BUFFER *)&NvramSmmCommunicationBuffer->Control;
    LockBuffer->Control = NVRAM_SMM_COMMAND_REQUEST_TO_LOCK;
    LockBuffer->DataSize = VariableNameSize;
    LockBuffer->Guid = *VendorGuid;
    MemCpy(LockBuffer+1, VariableName, VariableNameSize);
    return Communicate( sizeof(SMI_REQUEST_TO_LOCK_BUFFER) + VariableNameSize );
}

/**
    This function signals that exit BS event was taken place and switches
    SwitchToRuntime field in NvRamMailbox

    @param VOID

    @retval VOID

**/
VOID NvramSmmWrapperExitBootServicesHandler()
{
	SMI_EXIT_BOOT_SERVICES_BUFFER *ExitBootServicesBuffer;
    
    if (NvramSmmCommunicationBuffer == NULL) return;
    
    ExitBootServicesBuffer = (SMI_EXIT_BOOT_SERVICES_BUFFER *)&NvramSmmCommunicationBuffer->Control;
    ExitBootServicesBuffer->Control = NVRAM_SMM_COMMAND_EXIT_BOOT_SERVICES;
    
    Communicate( sizeof(SMI_EXIT_BOOT_SERVICES_BUFFER) );
}

/**
    Called on Virtual address change event and converts
    SmmCommProtocol pointer.

    @param Event signaled event
    @param Context calling context

    @retval VOID

**/
VOID EFIAPI NvramSmmWrapperVirtualAddressChangeHandler ()
{
    pRS->ConvertPointer (0, &NvramSmmCommunicationBuffer);
    if (SmmCommProtocol != NULL)
        pRS->ConvertPointer (0, (VOID**)&SmmCommProtocol);

}

/**
    This is the "NotInSmmFunction" parameter for the
    InitSmmHandler function in the entry point. It allocates
    memory for parameters buffer, saves its location, etc.

    @param EFI_HANDLE          - ImageHandle
        EFI_SYSTEM_TABLE*   - SystemTable

    @retval EFI_STATUS

**/


EFI_STATUS NvramSmiDxeEntry()
{
    EFI_STATUS  Status;
    UINT64 MaximumVariableStorageSize, RemainingVariableStorageSize, MaximumVariableSize;

    // Detect required buffer size and initialize the buffer.
    // The buffer should be large enough for a largest UEFI variable...
     Status = QueryVariableInfo(
    		EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
    		&MaximumVariableStorageSize, &RemainingVariableStorageSize, &MaximumVariableSize
    );    
	ASSERT_EFI_ERROR(Status);		
    if (EFI_ERROR(Status)) return Status;
    if (MaximumVariableStorageSize >= 0x100000000) return EFI_OUT_OF_RESOURCES;
    NvramSmmCommunicationBufferSize = (UINTN)MaximumVariableStorageSize;

    Status = QueryVariableInfo(
    		EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
    		&MaximumVariableStorageSize, &RemainingVariableStorageSize, &MaximumVariableSize
    );    
	ASSERT_EFI_ERROR(Status);		
    if (EFI_ERROR(Status)) return Status;
    if (MaximumVariableStorageSize >= 0x100000000) return EFI_OUT_OF_RESOURCES;
    if ((UINTN)MaximumVariableStorageSize > NvramSmmCommunicationBufferSize) 
    	NvramSmmCommunicationBufferSize = (UINTN)MaximumVariableStorageSize;

    // The buffer should be large enough for a largest UEFI variable.
    // It is natural to assume that the combined size of variable name and data can't be larger than the variable storage size.
    // However, in our case this is not true.
    // Names of UEFI variables are Unicode strings. 
    // Our main NVRAM driver (NvramDxeCommon.c) stores variable names comprised from Basic Latin characters as ASCII strings,
    // thus achieving 50% data compression.
    // Since SMM wrapper functions (in this file) pass variable name as is, we need up to twice the variable storage size
    // to be on par with the main NVRAM driver.
    // ( The worst case scenario is a variable with a one byte of data and a basic Latin name that occupies the rest of the variable storage).
    NvramSmmCommunicationBufferSize *= 2;
    
    Status = pBS->AllocatePool ( EfiRuntimeServicesData, NvramSmmCommunicationBufferSize, &NvramSmmCommunicationBuffer );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    NvramSmmCommunicationBufferPhysicalAddress = NvramSmmCommunicationBuffer;
    NvramSmmCommunicationBuffer->HeaderGuid = gAmiNvramSmmCommunicationGuid;
    CommunicationHeaderSize = ALIGN_VALUE( OFFSET_OF(EFI_SMM_COMMUNICATE_HEADER,Data), sizeof(UINT64) );
    MaxMessageLength = NvramSmmCommunicationBufferSize - CommunicationHeaderSize;

    return Status;
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
