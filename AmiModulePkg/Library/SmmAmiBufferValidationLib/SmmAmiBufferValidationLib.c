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
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AmiSmmBufferValidation.h>

AMI_SMM_BUFFER_VALIDATION_PROTOCOL *SmmAmiBufferValidationLibValidationProtocol = NULL;

// //////////////////////////////////////////////////////////////////////////////////////////////////////////
// AmiBufferValidationLib Library Class Functions
// //////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    Validates memory buffer.
    
    The function verifies the buffer to make sure its address range is legal for a memory buffer.
    SMI handlers that receive buffer address and/or size from outside of SMM at runtime must validate
    the buffer using this function prior to using it or passing to other SMM interfaces.

    @param  Buffer Buffer address 
    @param  BufferSize Size of the Buffer
    
    @retval  EFI_SUCCESS - The buffer address range is valid and can be safely used.
    @retval  EFI_ACCESS_DENIED - The buffer can't be used because its address range overlaps with protected area such as SMRAM.
    @retval  EFI_INVALID_PARAMETER - The buffer can't be used because its address range is invalid.
    @retval  EFI_NOT_FOUND - The buffer can't be used because its validity cannot be verified.
**/
EFI_STATUS EFIAPI AmiValidateMemoryBuffer(CONST VOID* Buffer, CONST UINTN BufferSize){
	EFI_STATUS Status;
	
	if (SmmAmiBufferValidationLibValidationProtocol == NULL) return EFI_ACCESS_DENIED;
	Status = SmmAmiBufferValidationLibValidationProtocol->ValidateMemoryBuffer(Buffer, BufferSize);
    if (EFI_ERROR(Status)) 
        DEBUG((DEBUG_ERROR, 
            "[%a]: SMM buffer security violation.\n(Address=%p; Size=%X).\n",
            gEfiCallerBaseName, Buffer, BufferSize
        ));
    return Status;
}

/**
    Validates MMIO buffer.
    
    The function verifies the buffer to make sure its address range is legal for a MMIO buffer.
    SMI handlers that receive buffer address and/or size from outside of SMM at runtime must validate
    the buffer using this function prior to using it or passing to other SMM interfaces.

    @param  Buffer Buffer address 
    @param  BufferSize Size of the Buffer
    
    @retval  EFI_SUCCESS - The buffer address range is valid and can be safely used.
    @retval  EFI_ACCESS_DENIED - The buffer can't be used because its address range overlaps with protected area such as SMRAM.
    @retval  EFI_INVALID_PARAMETER - The buffer can't be used because its address range is invalid.
    @retval  EFI_NOT_FOUND - The buffer can't be used because its validity cannot be verified.
**/
EFI_STATUS EFIAPI AmiValidateMmioBuffer(CONST VOID* Buffer, CONST UINTN BufferSize){
	EFI_STATUS Status;
	
	if (SmmAmiBufferValidationLibValidationProtocol == NULL) return EFI_ACCESS_DENIED;
	Status = SmmAmiBufferValidationLibValidationProtocol->ValidateMmioBuffer(Buffer, BufferSize);
    if (EFI_ERROR(Status)) 
        DEBUG((DEBUG_ERROR, 
            "[%a]: SMM buffer security violation.\nBased on GCD map, MMIO Buffer(Address=%p; Size=%X) is NOT in the region where MMIO is allowed.\n",
            gEfiCallerBaseName, Buffer, BufferSize
        ));
    return Status;
    
}

/**
    Validates SMRAM buffer.
    
    The function verifies the buffer to make sure it resides in the SMRAM.
    
    @param  Buffer Buffer address 
    @param  BufferSize Size of the Buffer
    
    @retval  EFI_SUCCESS - The buffer resides in the SMRAM and can be safely used.
    @retval  EFI_ACCESS_DENIED - The buffer can't be used because at least one byte of the buffer is outside of SMRAM.
    @retval  EFI_INVALID_PARAMETER - The buffer can't be used because its address range is invalid.
    @retval  EFI_NOT_FOUND - The buffer can't be used because its validity cannot be verified.
**/
EFI_STATUS EFIAPI AmiValidateSmramBuffer(CONST VOID* Buffer, CONST UINTN BufferSize){
    if (SmmAmiBufferValidationLibValidationProtocol == NULL) return EFI_ACCESS_DENIED;
    return SmmAmiBufferValidationLibValidationProtocol->ValidateSmramBuffer(Buffer, BufferSize);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////
// Library Initialization Functions
// //////////////////////////////////////////////////////////////////////////////////////////////////////////
EFI_STATUS EFIAPI SmmAmiBufferValidationLibConstructor(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){
    EFI_STATUS Status;
    
    // Locate AmiSmmBufferValidation protocol
    Status = gSmst->SmmLocateProtocol (
    	&gAmiSmmBufferValidationProtocolGuid, NULL,
    	(VOID **)&SmmAmiBufferValidationLibValidationProtocol
    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) SmmAmiBufferValidationLibValidationProtocol = NULL;
    return Status;
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
