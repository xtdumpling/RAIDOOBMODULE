//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
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
#include <Library/AmiBufferValidationLib.h>
#include "NvramSmi.h"
#include "NvramDxeCommon.h"

EFI_HANDLE  VarSmiHandle = NULL;
BOOLEAN 	gMorLock = FALSE; 
BOOLEAN		MorLockCheckIsActive = FALSE;
UINT64		gMorKey = 0;


static EFI_GUID gMemoryOverwriteRequestControlLockGuid = { 0xbb983ccf, 0x151d, 0x40e1, { 0xa0, 0x7b, 0x4a, 0x17, 0xbe, 0x16, 0x82, 0x92 } };
static EFI_GUID gMemoryOverwriteRequestControlGuid = {0xe20939be, 0x32d4, 0x41be, { 0xa1, 0x50, 0x89, 0x7f, 0x85, 0xd4, 0x98,0x29 } };

VOID ResetMorVariables(){
    UINT8 Data = 0;
    EFI_STATUS Status;
    UINTN DataSize = 0;
    
    // Firmware shall initialize the variable with 0 prior to processing the Boot#### on every boot. 
    // (BIOS Enabling Guide for Windows 10, Section#9 - Virtual Security Mode)
    Status = DxeSetVariableWrapper(
        L"MemoryOverwriteRequestControlLock",&gMemoryOverwriteRequestControlLockGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
        sizeof(Data), &Data
    );
    ASSERT_EFI_ERROR(Status);
    // If MemoryOverwriteRequestControl variable does not exist, create it.
    // If the variable exists, we are not reseting it back to zero.
    // Code that cleans the memory should reset variable back to zero.
    Status = DxeGetVariableWrapper(
        L"MemoryOverwriteRequestControl",&gMemoryOverwriteRequestControlGuid, NULL, &DataSize, NULL
    );
    if (Status == EFI_NOT_FOUND){
        Status = DxeSetVariableWrapper(
            L"MemoryOverwriteRequestControl",&gMemoryOverwriteRequestControlGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
            sizeof(Data), &Data
        );
        ASSERT_EFI_ERROR(Status);
    }
    MorLockCheckIsActive = TRUE;
}
/**
  This function locks the update of MOR variables once 
  MemoryOverwriteRequestControlLock variable is locked-down by setting it to one.
  This function is called before DxeSetVariableWrapper in SetVariableSmmHandler
  and based on returned status DxeSetVariableWrapper will or will not be executed with passed parameters. 
 
 @param VariableName pointer to Var Name in Unicode
 @param VendorGuid pointer to Var GUID
 @param Attributes Attributes of the Var
 @param DataSize size of Var
 @param Data Pointer to memory where Var data is stored
 
 @retval EFI_UNSUPPORTED - Passed Variable can be set by DxeSetVariableWrapper. 
 @retval Any Status other than EFI_UNSUPPORTED - DxeSetVariableWrapper will not be called and this Status 
 	 	 will be returned by DxeSetVariableWrapper.
 
**/
EFI_STATUS CheckForMorVariables(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
){

    EFI_STATUS Status;
    UINT8 VarData;
    
    if (  !StrCmp (VariableName, L"MemoryOverwriteRequestControlLock") 
        && !guidcmp (VendorGuid, &gMemoryOverwriteRequestControlLockGuid) 
    ){
    	if ( DataSize != 0 && Data == NULL )
    	    return EFI_INVALID_PARAMETER;
    	if ( Attributes == 0 || DataSize == 0 || Data == NULL )
    		return EFI_WRITE_PROTECTED;
    	if ( Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE) 
    			|| (DataSize != 1 && DataSize != 8))
    		return EFI_INVALID_PARAMETER;
    	
    	if (gMorLock == FALSE){
    		// MOR Variable Unlocked
    		if (DataSize == 1){
    			if (*(UINT8*)Data == 0) 
    				return EFI_UNSUPPORTED;
    			if (*(UINT8*)Data == 1){
    				gMorLock = TRUE;
    				return EFI_UNSUPPORTED;
    			}
    			return EFI_INVALID_PARAMETER;
    		} //if (DataSize == 1)
    		
    		if (DataSize == 8){	
    			gMorLock = TRUE;
    			gMorKey = ReadUnaligned64((UINT64*)Data);
    			VarData = 2;
    			Status = DxeSetVariableWrapper(VariableName, VendorGuid, Attributes, 1, &VarData);
    			return Status;
    		}
    		ASSERT (FALSE); //We should not get here, because DataSize can be only 1 or 8
    		
    		
    	}else{//if (gMorLock == FALSE)
    		// MOR Variable Locked
    		if (DataSize !=8 || gMorKey == 0) 
    			return EFI_ACCESS_DENIED;
    		if (gMorKey != ReadUnaligned64((UINT64*)Data)){
    			gMorKey = 0;
    			return EFI_ACCESS_DENIED;
    		}
    		gMorLock = FALSE;
    		gMorKey = 0;
    		VarData = 0;
    		Status = DxeSetVariableWrapper(VariableName, VendorGuid, Attributes, 1, &VarData);
    		return Status;
    	}	
    	
    }
    if(  !StrCmp (VariableName, L"MemoryOverwriteRequestControl") 
            && !guidcmp (VendorGuid, &gMemoryOverwriteRequestControlGuid) 
    ){	
    	if ( Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)
    			|| (DataSize != 1))
            return EFI_INVALID_PARAMETER;
    	if ( Data != NULL && *(UINT8*)Data > 1 && *(UINT8*)Data != 0x11)
    		return EFI_INVALID_PARAMETER;
    	if (gMorLock == FALSE) 
    		return EFI_UNSUPPORTED;
    	else 
    		return EFI_ACCESS_DENIED;
    }
    
    return EFI_UNSUPPORTED;
}


EFI_STATUS GetVariableSmmHandler(
	SMI_GET_VARIABLE_BUFFER *GetVarBuffer, UINTN BufferSize
){
	CHAR16 *VariableName;
	
    // Validate input buffer before accessing it.
	if (BufferSize <= sizeof(SMI_GET_VARIABLE_BUFFER)) return EFI_INVALID_PARAMETER;
	BufferSize -= sizeof(SMI_GET_VARIABLE_BUFFER);
	// Variable name must contains at least one character (zero terminator). String size must be an even number
    if (   GetVarBuffer->VariableNameSize<sizeof(CHAR16)
        || (GetVarBuffer->VariableNameSize & 1) != 0
    ) return EFI_INVALID_PARAMETER;
    if (BufferSize < GetVarBuffer->VariableNameSize) return EFI_INVALID_PARAMETER;
    BufferSize -= (UINTN)GetVarBuffer->VariableNameSize;
	VariableName = (CHAR16*)(GetVarBuffer+1);
	// Make sure variable name is zero terminated.
	if (VariableName[(UINTN)GetVarBuffer->VariableNameSize/sizeof(CHAR16)-1]) return EFI_INVALID_PARAMETER;
    if (BufferSize != GetVarBuffer->DataSize) return EFI_INVALID_PARAMETER;
	
    return DxeGetVariableWrapper (
    	VariableName, &GetVarBuffer->Guid,
    	&GetVarBuffer->Attributes, (UINTN*)&GetVarBuffer->DataSize,
    	(BufferSize==0) ? NULL : (VOID*)((UINT8*)(GetVarBuffer+1) + GetVarBuffer->VariableNameSize)
    );
}

EFI_STATUS GetNextVariableNameSmmHandler(
	SMI_GET_NEXT_VARIABLE_NAME_BUFFER *GetNextVarNameBuffer, UINTN BufferSize
){
    CHAR16 *VariableName;
    
    //Validate input buffer before accessing it.
	if (BufferSize <= sizeof(SMI_GET_NEXT_VARIABLE_NAME_BUFFER)) return EFI_INVALID_PARAMETER;
	BufferSize -= sizeof(SMI_GET_NEXT_VARIABLE_NAME_BUFFER);
    if (BufferSize != GetNextVarNameBuffer->DataSize) return EFI_INVALID_PARAMETER;
    // Input variable name must contains at least one character (zero terminator).
    if (GetNextVarNameBuffer->DataSize<sizeof(CHAR16)) return EFI_INVALID_PARAMETER;
	//Validating VariableName
    VariableName = (CHAR16*)(GetNextVarNameBuffer+1);
    if (GetVariableNameSize(VariableName, (UINTN)GetNextVarNameBuffer->DataSize) > GetNextVarNameBuffer->DataSize) return EFI_INVALID_PARAMETER;
    
	return DxeGetNextVariableNameWrapper(
		(UINTN*)&GetNextVarNameBuffer->DataSize,
		VariableName, &GetNextVarNameBuffer->Guid
	);
}

EFI_STATUS SetVariableSmmHandler(
	SMI_SET_VARIABLE_BUFFER *SetVarBuffer, UINTN BufferSize
){
	CHAR16 *VariableName;
	EFI_STATUS	Status;
	
    //Validate input buffer before accessing it.
	if (BufferSize <= sizeof(SMI_SET_VARIABLE_BUFFER)) return EFI_INVALID_PARAMETER;
	BufferSize -= sizeof(SMI_SET_VARIABLE_BUFFER);
	// Variable name must contains at least one character (zero terminator). String size must be an even number
    if (   SetVarBuffer->VariableNameSize<sizeof(CHAR16)
        || (SetVarBuffer->VariableNameSize & 1) != 0
    ) return EFI_INVALID_PARAMETER;
	if (BufferSize < SetVarBuffer->VariableNameSize) return EFI_INVALID_PARAMETER;
    BufferSize -= (UINTN)SetVarBuffer->VariableNameSize;
	VariableName = (CHAR16*)(SetVarBuffer+1);
	// Make sure variable name is zero terminated.
	if (VariableName[(UINTN)SetVarBuffer->VariableNameSize/sizeof(CHAR16)-1]) return EFI_INVALID_PARAMETER;
    if (BufferSize != SetVarBuffer->DataSize) return EFI_INVALID_PARAMETER;
	// Special handling of the Memory Override Request Variables
    if (MorLockCheckIsActive == TRUE){
    	Status = CheckForMorVariables (
        		VariableName, &SetVarBuffer->Guid,
        		SetVarBuffer->Attributes, (UINTN)SetVarBuffer->DataSize,
        		(BufferSize==0) ? NULL : (VOID*)((UINT8*)(SetVarBuffer+1) + SetVarBuffer->VariableNameSize)
        	);
    	if (Status != EFI_UNSUPPORTED){ 
    		return Status;
    	}
    }
    return DxeSetVariableWrapper (
    	VariableName, &SetVarBuffer->Guid,
    	SetVarBuffer->Attributes, (UINTN)SetVarBuffer->DataSize,
    	(BufferSize==0) ? NULL : (VOID*)((UINT8*)(SetVarBuffer+1) + SetVarBuffer->VariableNameSize)
    );
}

EFI_STATUS QueryVariableInfoSmmHandler(
	SMI_QUERY_VARIABLE_INFO_BUFFER *QueryVarInfoBuffer, UINTN BufferSize
){
    //Validate input buffer before accessing it.
	if (BufferSize != sizeof(SMI_QUERY_VARIABLE_INFO_BUFFER)) return EFI_INVALID_PARAMETER;
	
	return QueryVariableInfo (
		QueryVarInfoBuffer->Attributes,
        &QueryVarInfoBuffer->MaximumVariableStorageSize,
        &QueryVarInfoBuffer->RemainingVariableStorageSize,
        &QueryVarInfoBuffer->MaximumVariableSize
    );
}

EFI_STATUS RequestToLockSmmHandler(
	SMI_REQUEST_TO_LOCK_BUFFER *RequestToLockBuffer, UINTN BufferSize
){
	CHAR16 *VariableName;
	
    //Validate input buffer before accessing it.
	if (BufferSize <= sizeof(SMI_REQUEST_TO_LOCK_BUFFER)) return EFI_INVALID_PARAMETER;
	BufferSize -= sizeof(SMI_REQUEST_TO_LOCK_BUFFER);
    if (BufferSize != RequestToLockBuffer->DataSize) return EFI_INVALID_PARAMETER;
    // Variable name must contains at least one character (zero terminator). String size must be an even number
    if (   RequestToLockBuffer->DataSize<sizeof(CHAR16)
        || (RequestToLockBuffer->DataSize & 1) != 0
    ) return EFI_INVALID_PARAMETER;
	VariableName = (CHAR16*)(RequestToLockBuffer+1);
	// Make sure variable name is zero terminated.
	if (VariableName[(UINTN)RequestToLockBuffer->DataSize/sizeof(CHAR16)-1]) return EFI_INVALID_PARAMETER;
	
	return DxeRequestToLock(
		NULL,VariableName,&RequestToLockBuffer->Guid
	);	
}

EFI_STATUS ExitBootServicesSmmHandler(
	SMI_EXIT_BOOT_SERVICES_BUFFER *ExitBootServicesBuffer, UINTN BufferSize
){
    //Validate input buffer before accessing it.
	if (BufferSize != sizeof(SMI_EXIT_BOOT_SERVICES_BUFFER)) return EFI_INVALID_PARAMETER;
	SwitchToRuntime();
	return EFI_SUCCESS;
}

/**
     SMI handler for NVRAM SMM Communication API.

    @param DispatchHandle - Dispatch Handle
	@param Context - Pointer to the passed context
	@param CommBuffer - Pointer to the passed Communication Buffer
	@param CommBufferSize - Pointer to the Comm Buffer Size


    @retval EFI_STATUS based on result

**/
EFI_STATUS EFIAPI NvramSmmCommunicationHandler (
  IN EFI_HANDLE DispatchHandle, IN CONST VOID *Context,
  IN OUT VOID *CommBuffer, IN OUT UINTN *CommBufferSize
){
    EFI_STATUS	Status;
    UINT64 *Command;
    VOID *Tmp;
    UINTN BufferSize;

    // Validate the buffer
    BufferSize = *CommBufferSize;
    Status = AmiValidateMemoryBuffer(CommBuffer, BufferSize);
    // A list of SMI handler return codes in defined by PI specification.
    // We can't return arbitrary error here.
    if (EFI_ERROR(Status)) return EFI_SUCCESS;
    if (BufferSize < sizeof(*Command))  return EFI_SUCCESS;
    
    // The CommBuffer is a pointer passed to SMM Communicate incremented to strip the header (GUID and MessageSize).
    // Since message size is UINTN, the pointer will not be naturally aligned in 32-bit mode.
    // We're aligning pointer to guarantee natural alignment.
    // See NVRAM_COMMUNICATION_BUFFER definition in NvramSmiDxe.c (The structure is naturally aligned).
    Tmp = ALIGN_POINTER(CommBuffer,sizeof(*Command));
    BufferSize -= (UINTN)Tmp-(UINTN)CommBuffer;
    CommBuffer = Tmp;
    Command = (UINT64*)CommBuffer;
    
    // Perform basic buffer size validation.
    // A more advanced validation is performed by the command handling functions.
    if (BufferSize < sizeof(*Command))  return EFI_SUCCESS;
    switch (*Command){
        case NVRAM_SMM_COMMAND_GET_VARIABLE: //GetVariable
        	Status = GetVariableSmmHandler((SMI_GET_VARIABLE_BUFFER*)CommBuffer, BufferSize);
        	break;
        case NVRAM_SMM_COMMAND_GET_NEXT_VARIABLE_NAME: //GetNextVariableName
        	Status = GetNextVariableNameSmmHandler((SMI_GET_NEXT_VARIABLE_NAME_BUFFER*)CommBuffer, BufferSize);
        	break;
        case NVRAM_SMM_COMMAND_SET_VARIABLE: //SetVariable
        	Status = SetVariableSmmHandler((SMI_SET_VARIABLE_BUFFER*)CommBuffer, BufferSize);
        	break;
        case NVRAM_SMM_COMMAND_QUERY_VARIABLE_INFO: //QueryVariableInfo
        	Status = QueryVariableInfoSmmHandler((SMI_QUERY_VARIABLE_INFO_BUFFER*)CommBuffer, BufferSize);
        	break;
        case NVRAM_SMM_COMMAND_REQUEST_TO_LOCK:
        	Status = RequestToLockSmmHandler((SMI_REQUEST_TO_LOCK_BUFFER*)CommBuffer, BufferSize);
            break;
        case NVRAM_SMM_COMMAND_EXIT_BOOT_SERVICES:
        	Status = ExitBootServicesSmmHandler((SMI_EXIT_BOOT_SERVICES_BUFFER*)CommBuffer, BufferSize);
        	break;
        default:
        	Status = EFI_INVALID_PARAMETER;
        	break;
	}
    *Command = EFI_STATUS_TO_NVRAM_SMM_STATUS(Status);
   
    return EFI_SUCCESS;
}

/**
    Entry point.

    @param  ImageHandle - Image Handle
    @param  SystemTable - Pointer to a System Table

    @retval EFI_STATUS based on result

**/
EFI_STATUS EFIAPI NvramSmiEntry(
    IN EFI_HANDLE          		 ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
){
    EFI_STATUS Status;

    InitAmiSmmLib (ImageHandle, SystemTable);
    ASSERT (pSmst != NULL);
    Status = pSmst->SmiHandlerRegister(NvramSmmCommunicationHandler, &gAmiNvramSmmCommunicationGuid, &VarSmiHandle);
    ASSERT_EFI_ERROR (Status);

    return Status;
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
