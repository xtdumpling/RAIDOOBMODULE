//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
#include <Protocol/SmmAccess2.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/AmiBufferValidationLib.h>
#include <Library/UefiLib.h> // We only use NEXT_MEMORY_DESCRIPTOR macro. No need to mention UefiLib in the .inf file.
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmReadyToLock.h>
#include <Protocol/AmiSmmBufferValidation.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>
#include <AmiSmmCommunicationMailbox.h>

#define EFI_STATUS_TO_SMM_MAILBOX_STATUS(Status) (UINT32)( (EFI_ERROR(Status)) ? 0x80000000|(UINT32)(Status) : (UINT32)(Status) )
#define IS_BUFFER_OVERFLOW(BufferAddress, BufferSize) ( (BufferSize) != 0 && (UINTN)(BufferAddress) - 1 + (BufferSize) < (UINTN)(BufferAddress) )

typedef struct {
    EFI_PHYSICAL_ADDRESS Address;
    UINT64 Size;
} SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE;

SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE *SmmAmiBufferValidationLibSmramRanges = NULL;
UINTN SmmAmiBufferValidationLibNumberOfSmramRanges = 0;

SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE *SmmAmiBufferValidationLibMemoryRanges = NULL;
UINTN SmmAmiBufferValidationLibNumberOfMemoryRanges = 0;

SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE *SmmAmiBufferValidationLibMmioRanges = NULL;
UINTN SmmAmiBufferValidationLibNumberOfMmioRanges = 0;

VOID *SmmAmiBufferValidationLibReadyToLockRegistration = NULL;
VOID *SmmAmiBufferValidationLibSwDispatchRegistration = NULL;

BOOLEAN SmmAmiBufferValidationLibSmmIsLocked = FALSE;
BOOLEAN SmmAmiBufferValidationLibCsmEnabled = FALSE;

EFI_HANDLE SmmAmiBufferValidationLibValidationProtocolHandle = NULL;

AMI_SMM_COMMUNICATION_MAILBOX *SmmAmiBufferValidationLibSmmMailBox = NULL;
EFI_PHYSICAL_ADDRESS SmmAmiBufferValidationLibCommunicationBuffer = 0;
UINT32 SmmAmiBufferValidationLibCommunicationBufferSize = 0;
UINT32 SmmAmiBufferValidationLibTransactionId = 0;
BOOLEAN SmmAmiBufferValidationLibBufferLocked = FALSE;
EFI_SMM_CPU_PROTOCOL *SmmAmiBufferValidationLibSmmCpu = NULL;

// //////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Functions
// //////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @internal
    Returns an error if buffer overlaps with a list of illegal address ranges.
    
    @param  Buffer Buffer address 
    @param  BufferSize Size of the Buffer
    @param  Range Pointer to array of illegal ranges to check the buffer against
    @param  NumberOfRanges Number of elements in the Range array
    
    @retval  EFI_SUCCESS - The buffer address range is valid and does not overlap with the address ranges in Range
    @retval  EFI_ACCESS_DENIED - The buffer can't be used because its address range overlaps with one of the ranges in Range.
    @retval  EFI_INVALID_PARAMETER - The buffer can't be used because its address range is invalid.
    @retval  EFI_NOT_FOUND - The buffer can't be used because its validity cannot be verified.
**/
EFI_STATUS SmmAmiBufferValidationLibCheckBufferAgainstBlackList(
    CONST VOID* Buffer, CONST UINTN BufferSize,
    CONST SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE* Range, CONST UINTN NumberOfRanges
){
    UINTN  i;
    UINTN BufferAddress;

    if (Buffer == NULL) return EFI_INVALID_PARAMETER;
    if (Range==NULL) return EFI_NOT_FOUND;

    BufferAddress = (UINTN)Buffer;
    if (IS_BUFFER_OVERFLOW(BufferAddress, BufferSize)) return EFI_INVALID_PARAMETER; // overflow
    for (i = 0; i < NumberOfRanges; i ++) {
        if (    BufferAddress >= Range[i].Address
             && BufferAddress < Range[i].Address + Range[i].Size
        ) return EFI_ACCESS_DENIED; // Buffer starts in the black list region
        if (    BufferAddress < Range[i].Address
             && BufferAddress+BufferSize > Range[i].Address
        ) return EFI_ACCESS_DENIED; // Buffer overlaps with the black list region
    }
    
    return EFI_SUCCESS;
}

/** @internal
    Returns an error if buffer does not fit into one of the dedicated address ranges.
    
    @param  Buffer Buffer address 
    @param  BufferSize Size of the Buffer
    @param  Range Pointer to array of dedicated ranges to check the buffer against
    @param  NumberOfRanges Number of elements in the Range array
    
    @retval  EFI_SUCCESS - The buffer address range is valid and is a subregion of one of the address ranges in Range
    @retval  EFI_ACCESS_DENIED - The buffer can't be used because it is not a subregion of an address ranges in Range
    @retval  EFI_INVALID_PARAMETER - The buffer can't be used because its address range is invalid.
    @retval  EFI_NOT_FOUND - The buffer can't be used because its validity cannot be verified.
**/
EFI_STATUS SmmAmiBufferValidationLibCheckBufferAgainstWhiteList(
    CONST VOID* Buffer, CONST UINTN BufferSize,
    CONST SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE* Range, CONST UINTN NumberOfRanges
){
    UINTN  i;
    UINTN BufferAddress;

    if (Buffer == NULL) return EFI_INVALID_PARAMETER;
    if (Range==NULL) return EFI_NOT_FOUND;

    BufferAddress = (UINTN)Buffer;
    if (IS_BUFFER_OVERFLOW(BufferAddress, BufferSize)) return EFI_INVALID_PARAMETER; // overflow
    for (i = 0; i < NumberOfRanges; i ++) {
        if (    BufferAddress >= Range[i].Address
             && BufferAddress+BufferSize <= Range[i].Address + Range[i].Size
        ) return EFI_SUCCESS; // Entire Buffer is in the white list region
    }
    
    return EFI_ACCESS_DENIED;
}

/** @internal
    If buffer is within SMM Communication Buffer, makes sure it was properly allocated.
    
    @param  Buffer Buffer address 
    @param  BufferSize Size of the Buffer
    
    @retval  EFI_SUCCESS - The buffer is valid
    @retval  EFI_ACCESS_DENIED - The buffer can't be used
    @retval  EFI_INVALID_PARAMETER - The buffer can't be used because its address range is invalid.
**/
EFI_STATUS SmmAmiBufferValidationLibCheckSmmCommunicationBuffer(
    CONST VOID* Buffer, CONST UINTN BufferSize
){
    UINTN BufferAddress;
    EFI_PHYSICAL_ADDRESS EndOfCommBuffer;

    if (Buffer == NULL) return EFI_INVALID_PARAMETER;

	if (SmmAmiBufferValidationLibSmmMailBox == NULL) return EFI_SUCCESS;
    BufferAddress = (UINTN)Buffer;
    if (IS_BUFFER_OVERFLOW(BufferAddress, BufferSize)) return EFI_INVALID_PARAMETER; // overflow

    EndOfCommBuffer = SmmAmiBufferValidationLibCommunicationBuffer + SmmAmiBufferValidationLibCommunicationBufferSize;
    // If buffer is outside SMM Communication buffer, return EFI_SUCCESS
    if (    BufferAddress >= EndOfCommBuffer
    	 || BufferAddress+BufferSize <= SmmAmiBufferValidationLibCommunicationBuffer
    ) return EFI_SUCCESS;
	
    // If buffer overlaps with the SMM Communication buffer, but is not entirely
	// in the SMM Communication buffer, return an error
    if (    BufferAddress < SmmAmiBufferValidationLibCommunicationBuffer
         || BufferAddress+BufferSize > EndOfCommBuffer
    ){
    	SmmAmiBufferValidationLibSmmMailBox->Status = EFI_STATUS_TO_SMM_MAILBOX_STATUS(EFI_ACCESS_DENIED);
    	return EFI_ACCESS_DENIED;
    }
	// If buffer is in the SMM Communication buffer, the size can't be zero
	if (BufferSize == 0){
		SmmAmiBufferValidationLibSmmMailBox->Status = EFI_STATUS_TO_SMM_MAILBOX_STATUS(EFI_INVALID_PARAMETER);
		return EFI_INVALID_PARAMETER;
	}

    // Buffer must be locked before it's used.
    // Owner must initialized mailbox field Token before every call.
    if (    !SmmAmiBufferValidationLibBufferLocked
    	 || SmmAmiBufferValidationLibSmmMailBox->Token != SmmAmiBufferValidationLibTransactionId
    ){
        DEBUG_CODE(
            if (!SmmAmiBufferValidationLibBufferLocked)
                DEBUG((DEBUG_ERROR, "ERROR: Application SMM communication buffer is unlocked.\n"));
            else
                DEBUG((DEBUG_ERROR, "ERROR: Application SMM communication buffer access token is invalid.\n"));
        );
    	SmmAmiBufferValidationLibSmmMailBox->Status = EFI_STATUS_TO_SMM_MAILBOX_STATUS(EFI_ACCESS_DENIED);
    	return EFI_ACCESS_DENIED;
    }
    
    SmmAmiBufferValidationLibSmmMailBox->Status = 0;
	return EFI_SUCCESS;
}

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
EFI_STATUS EFIAPI SmmAmiBufferValidationLibAmiValidateMemoryBuffer(CONST VOID* Buffer, CONST UINTN BufferSize){
    EFI_STATUS Status;
    
    Status = SmmAmiBufferValidationLibCheckBufferAgainstBlackList(
        Buffer,BufferSize,
        SmmAmiBufferValidationLibSmramRanges,SmmAmiBufferValidationLibNumberOfSmramRanges
    );
    if (EFI_ERROR(Status)){
        DEBUG((DEBUG_ERROR, "ERROR: SMM communication buffer is in SMRAM.\n"));
        return Status;
    }

    if (   (PcdGet32(AmiPcdSmmMemLibProperties) & AMI_BUFFER_VALIDATION_LIB_ALLOW_ACCESS_TO_OS_MEMORY) == 0 
         && SmmAmiBufferValidationLibSmmIsLocked
    ) {
        Status = SmmAmiBufferValidationLibCheckBufferAgainstWhiteList(
            Buffer,BufferSize,
            SmmAmiBufferValidationLibMemoryRanges,SmmAmiBufferValidationLibNumberOfMemoryRanges
         );
        if (EFI_ERROR(Status)){
        	// If CSM is enabled we're allowing SMM communication with buffers below 1MB.
        	if (SmmAmiBufferValidationLibCsmEnabled){
        		UINTN BufferAddress = (UINTN)Buffer;
        		// Area below 0x400 contains IVT
        		// Area 0xA0000..0xFFFFF is reserved for video buffer, OpROM space, and runtime portion of csm16.
        		if (   BufferAddress < 0xA0000 && BufferAddress >= 0x400 && BufferSize <= 0xA0000 - 0x400
        			&& BufferAddress + BufferSize <= 0xA0000
        		) Status = EFI_SUCCESS;
        	}
        	if (EFI_ERROR(Status))
                DEBUG((DEBUG_ERROR, "ERROR: SMM communication buffer is NOT in the region reserved by firmware for RT access.\n"));
        } else {
        	Status = SmmAmiBufferValidationLibCheckSmmCommunicationBuffer(Buffer, BufferSize);
        }
    } 
    
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
EFI_STATUS EFIAPI SmmAmiBufferValidationLibAmiValidateMmioBuffer(CONST VOID* Buffer, CONST UINTN BufferSize){
    
    EFI_STATUS Status;
    
    Status = SmmAmiBufferValidationLibCheckBufferAgainstBlackList(
        Buffer,BufferSize,
        SmmAmiBufferValidationLibSmramRanges,SmmAmiBufferValidationLibNumberOfSmramRanges
    );
    if (EFI_ERROR(Status)) return Status;

    if (!SmmAmiBufferValidationLibSmmIsLocked) return Status;
    
    Status = SmmAmiBufferValidationLibCheckBufferAgainstWhiteList(
        Buffer,BufferSize,
        SmmAmiBufferValidationLibMmioRanges,SmmAmiBufferValidationLibNumberOfMmioRanges
    );

    return Status;
}

EFI_STATUS EFIAPI AmiValidateMemoryBuffer(CONST VOID* Buffer, CONST UINTN BufferSize){
	EFI_STATUS Status = SmmAmiBufferValidationLibAmiValidateMemoryBuffer(Buffer, BufferSize);
    if (EFI_ERROR(Status)) 
        DEBUG((DEBUG_ERROR, 
            "[%a]: SMM buffer security violation.\n(Address=%p; Size=%X).\n",
            gEfiCallerBaseName, Buffer, BufferSize
        ));
    return Status;
}

EFI_STATUS EFIAPI AmiValidateMmioBuffer(CONST VOID* Buffer, CONST UINTN BufferSize){
	EFI_STATUS Status = SmmAmiBufferValidationLibAmiValidateMmioBuffer(Buffer, BufferSize);
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
    return SmmAmiBufferValidationLibCheckBufferAgainstWhiteList(
        Buffer,BufferSize,
        SmmAmiBufferValidationLibSmramRanges,SmmAmiBufferValidationLibNumberOfSmramRanges
    );
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////
// Library Initialization Functions
// //////////////////////////////////////////////////////////////////////////////////////////////////////////
EFI_STATUS SmmAmiBufferValidationLibGetMemMap(){
    EFI_STATUS Status;
    UINTN Size;
    EFI_MEMORY_DESCRIPTOR *MemMap = NULL;
    EFI_MEMORY_DESCRIPTOR *Descriptor;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    UINTN i;
    
    Size = 0;
    Status = gBS->GetMemoryMap(&Size, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    ASSERT(Status==EFI_BUFFER_TOO_SMALL);
    if (Status!=EFI_BUFFER_TOO_SMALL) return Status;

    // The following memory allocation may alter memory map.
    // Let's add space for 16 more descriptors to be sure the buffer is big enough.
    Size += 16 * DescriptorSize;
    Status = gBS->AllocatePool(EfiBootServicesData, Size, &MemMap);
    ASSERT_EFI_ERROR(Status);

    Status = gBS->GetMemoryMap(&Size, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)){
        gBS->FreePool(MemMap);
        return Status;
    }
    for(   Descriptor = MemMap
         ; (UINT8*)Descriptor < (UINT8*)MemMap+Size
         ; Descriptor = NEXT_MEMORY_DESCRIPTOR(Descriptor,DescriptorSize)
    ){
        if (    Descriptor->Type == EfiReservedMemoryType || Descriptor->Type == EfiACPIMemoryNVS
             || Descriptor->Type == EfiRuntimeServicesCode || Descriptor->Type == EfiRuntimeServicesData
        ) SmmAmiBufferValidationLibNumberOfMemoryRanges++;
    }

    SmmAmiBufferValidationLibMemoryRanges = AllocatePool (
        SmmAmiBufferValidationLibNumberOfMemoryRanges * sizeof(SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE)
    );
    ASSERT (SmmAmiBufferValidationLibMemoryRanges != NULL);
    if (SmmAmiBufferValidationLibMemoryRanges == NULL){
        SmmAmiBufferValidationLibNumberOfMemoryRanges = 0;
        gBS->FreePool(MemMap);
        return EFI_OUT_OF_RESOURCES;
    }
    for(   Descriptor = MemMap, i=0
         ; (UINT8*)Descriptor < (UINT8*)MemMap+Size
         ; Descriptor = NEXT_MEMORY_DESCRIPTOR(Descriptor,DescriptorSize)
    ){
        if (    Descriptor->Type == EfiReservedMemoryType || Descriptor->Type == EfiACPIMemoryNVS
             || Descriptor->Type == EfiRuntimeServicesCode || Descriptor->Type == EfiRuntimeServicesData
        ){
            ASSERT((UINTN)Descriptor->NumberOfPages == Descriptor->NumberOfPages);
            SmmAmiBufferValidationLibMemoryRanges[i].Address = Descriptor->PhysicalStart;
            SmmAmiBufferValidationLibMemoryRanges[i].Size = EFI_PAGES_TO_SIZE((UINTN)Descriptor->NumberOfPages);
            i++;
        }
    }
    ASSERT(i==SmmAmiBufferValidationLibNumberOfMemoryRanges);
    gBS->FreePool(MemMap);
    return EFI_SUCCESS;
}

EFI_STATUS SmmAmiBufferValidationLibGetGcdMap(){
    UINTN NumberOfDescriptors, i, j;
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR *GcdMap;
    EFI_STATUS Status;
    
    Status = gDS->GetMemorySpaceMap (&NumberOfDescriptors, &GcdMap);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    for (i = 0; i < NumberOfDescriptors; i++) {
        if (    GcdMap[i].GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo
             ||    GcdMap[i].GcdMemoryType == EfiGcdMemoryTypeNonExistent 
                && (PcdGet32(AmiPcdSmmMemLibProperties) & AMI_BUFFER_VALIDATION_LIB_ALLOW_MMIO_IN_NON_EXISTENT_REGIONS)
             ||    GcdMap[i].GcdMemoryType == EfiGcdMemoryTypeReserved 
                && (PcdGet32(AmiPcdSmmMemLibProperties) & AMI_BUFFER_VALIDATION_LIB_ALLOW_MMIO_IN_RESERVED_REGIONS)
        ) SmmAmiBufferValidationLibNumberOfMmioRanges++;
    }
    
    SmmAmiBufferValidationLibMmioRanges = AllocatePool (
        SmmAmiBufferValidationLibNumberOfMmioRanges * sizeof(SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE)
    );
    ASSERT (SmmAmiBufferValidationLibMmioRanges != NULL);
    if (SmmAmiBufferValidationLibMmioRanges == NULL){
        SmmAmiBufferValidationLibNumberOfMmioRanges = 0;
        gBS->FreePool(GcdMap);
        return EFI_OUT_OF_RESOURCES;
    }
    for (i = 0, j=0; i < NumberOfDescriptors; i++) {
        if (    GcdMap[i].GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo
             ||    GcdMap[i].GcdMemoryType == EfiGcdMemoryTypeNonExistent 
                && (PcdGet32(AmiPcdSmmMemLibProperties) & AMI_BUFFER_VALIDATION_LIB_ALLOW_MMIO_IN_NON_EXISTENT_REGIONS)
             ||    GcdMap[i].GcdMemoryType == EfiGcdMemoryTypeReserved 
                && (PcdGet32(AmiPcdSmmMemLibProperties) & AMI_BUFFER_VALIDATION_LIB_ALLOW_MMIO_IN_RESERVED_REGIONS)
        ){
            SmmAmiBufferValidationLibMmioRanges[j].Address = GcdMap[i].BaseAddress;
            SmmAmiBufferValidationLibMmioRanges[j].Size = GcdMap[i].Length;
            j++;
        }
    }
    ASSERT(j==SmmAmiBufferValidationLibNumberOfMmioRanges);
    gBS->FreePool (GcdMap);
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI SmmAmiBufferValidationLibReadyToLock (
    IN CONST EFI_GUID *Protocol, IN VOID *Interface, IN EFI_HANDLE Handle
){
    if ( (PcdGet32(AmiPcdSmmMemLibProperties) & AMI_BUFFER_VALIDATION_LIB_ALLOW_ACCESS_TO_OS_MEMORY) == 0 ){
        VOID *Dummy;
        
        // Get UEFI memory map and save runtime memory ranges
        SmmAmiBufferValidationLibGetMemMap();
        // Check if CSM is enabled.
        // If CSM is enabled we're allowing SMM communication with buffers below 1MB. 
        // See AmiValidateMemoryBuffer for details.
        if ( !EFI_ERROR( gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &Dummy) ) )
            SmmAmiBufferValidationLibCsmEnabled = TRUE;
    }
    // Get GCD memory map and save MMIO ranges
    SmmAmiBufferValidationLibGetGcdMap();
    
    SmmAmiBufferValidationLibSmmIsLocked = TRUE;
    return EFI_SUCCESS;
}

VOID SmmAmiBufferValidationLibSetSmiStatus(UINTN CpuIndex, EFI_STATUS Status){

	UINT32 SmiStatus = EFI_STATUS_TO_SMM_MAILBOX_STATUS(Status);
			
	Status = SmmAmiBufferValidationLibSmmCpu->WriteSaveState(
		SmmAmiBufferValidationLibSmmCpu,4, EFI_SMM_SAVE_STATE_REGISTER_RCX, CpuIndex, &SmiStatus
	);
	ASSERT_EFI_ERROR(Status);
}

EFI_STATUS EFIAPI SmmAmiBufferValidationLibSmmMailBoxHandler(
	IN EFI_HANDLE DispatchHandle, IN CONST VOID *Context OPTIONAL,
	IN OUT VOID *CommBuffer OPTIONAL, IN OUT UINTN *CommBufferSize OPTIONAL
){
	EFI_STATUS Status;
	UINTN CpuIndex;
	UINT32 Command;
	EFI_PHYSICAL_ADDRESS MailboxAddress;
	
	if (    SmmAmiBufferValidationLibSmmMailBox == NULL
		 || SmmAmiBufferValidationLibSmmCpu == NULL
		 || CommBuffer == NULL || CommBufferSize == NULL || *CommBufferSize != sizeof(EFI_SMM_SW_CONTEXT)
	) return EFI_SUCCESS;
	
	CpuIndex = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
	Status = SmmAmiBufferValidationLibSmmCpu->ReadSaveState(
		SmmAmiBufferValidationLibSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RCX, CpuIndex, &Command
	);
	if (EFI_ERROR(Status)) return EFI_SUCCESS;
	
	if (Command == AMI_SMM_COMMUNICATION_MAILBOX_COMMAND_LOCK ){
		// Lock
		UINT32 BufferSize;
		UINT32 Flags;
		
		Status = SmmAmiBufferValidationLibSmmCpu->ReadSaveState(
			SmmAmiBufferValidationLibSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RSI, CpuIndex, &BufferSize
		);
		if (EFI_ERROR(Status)) return EFI_SUCCESS;
        Status = SmmAmiBufferValidationLibSmmCpu->ReadSaveState(
            SmmAmiBufferValidationLibSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RDI, CpuIndex, &Flags
        );
        if (EFI_ERROR(Status)) return EFI_SUCCESS;
		
		if (    SmmAmiBufferValidationLibBufferLocked 
			 && (Flags & AMI_SMM_COMMUNICATION_MAILBOX_LOCK_FLAG_FORCE_LOCK)==0
		){
		    DEBUG((DEBUG_ERROR, "ERROR: Attempt to lock application SMM communication buffer that is already locked.\n"));
			Status = EFI_ACCESS_DENIED;
			SmmAmiBufferValidationLibSetSmiStatus(CpuIndex,Status);
			return EFI_SUCCESS;
		}

		if (BufferSize > SmmAmiBufferValidationLibCommunicationBufferSize){
            DEBUG((DEBUG_ERROR,
                "ERROR: Requested application SMM communication buffer size is not supported.\nRequested size=%X; Supported size=%X.\n",
                BufferSize, SmmAmiBufferValidationLibCommunicationBufferSize
            ));
			Status = EFI_BAD_BUFFER_SIZE;
			SmmAmiBufferValidationLibSetSmiStatus(CpuIndex,Status);
			BufferSize = SmmAmiBufferValidationLibCommunicationBufferSize;
			Status = SmmAmiBufferValidationLibSmmCpu->WriteSaveState(
				SmmAmiBufferValidationLibSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RSI, CpuIndex, &BufferSize
			);
			ASSERT_EFI_ERROR(Status);
			return EFI_SUCCESS;
		}
		SmmAmiBufferValidationLibTransactionId++;
		SmmAmiBufferValidationLibBufferLocked = TRUE;
		SmmAmiBufferValidationLibSmmMailBox->Revision = AMI_SMM_COMMUNICATION_MAILBOX_REVISION;
		SmmAmiBufferValidationLibSmmMailBox->PhysicalAddress = SmmAmiBufferValidationLibCommunicationBuffer;
		SmmAmiBufferValidationLibSmmMailBox->Token = SmmAmiBufferValidationLibTransactionId;
		SmmAmiBufferValidationLibSmmMailBox->Status = 0;
		
		MailboxAddress = (UINTN)SmmAmiBufferValidationLibSmmMailBox;
		Status = SmmAmiBufferValidationLibSmmCpu->WriteSaveState(
			SmmAmiBufferValidationLibSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RSI, CpuIndex, &MailboxAddress
		);
		ASSERT_EFI_ERROR(Status);
        Status = SmmAmiBufferValidationLibSmmCpu->WriteSaveState(
            SmmAmiBufferValidationLibSmmCpu, 4, EFI_SMM_SAVE_STATE_REGISTER_RDI, CpuIndex, (UINT32*)&MailboxAddress + 1
        );
        ASSERT_EFI_ERROR(Status);
	}else if (Command == AMI_SMM_COMMUNICATION_MAILBOX_COMMAND_UNLOCK ){
		// Unlock
		if (!SmmAmiBufferValidationLibBufferLocked){
			Status = EFI_ACCESS_DENIED;
			SmmAmiBufferValidationLibSetSmiStatus(CpuIndex,Status);
			DEBUG((DEBUG_ERROR, "ERROR: Attempt to unlock application SMM communication buffer that is not locked.\n"));
			return EFI_SUCCESS;
		}
		if (    SmmAmiBufferValidationLibSmmMailBox->Revision != AMI_SMM_COMMUNICATION_MAILBOX_REVISION
			 || SmmAmiBufferValidationLibSmmMailBox->PhysicalAddress != SmmAmiBufferValidationLibCommunicationBuffer
			 || SmmAmiBufferValidationLibSmmMailBox->Token != SmmAmiBufferValidationLibTransactionId
		){
			Status = EFI_INVALID_PARAMETER;
			SmmAmiBufferValidationLibSetSmiStatus(CpuIndex,Status);
			DEBUG((DEBUG_ERROR, "ERROR: Attempt to unlock SMM communication buffer with invalid mailbox content.\n"));
			return EFI_SUCCESS;
		}
		SmmAmiBufferValidationLibBufferLocked = FALSE;
		SmmAmiBufferValidationLibSmmMailBox->Token = 0;
		SmmAmiBufferValidationLibSmmMailBox->Status = 0;
	}else {
		SmmAmiBufferValidationLibSetSmiStatus(CpuIndex,EFI_UNSUPPORTED);
		return EFI_SUCCESS;
	}
	SmmAmiBufferValidationLibSetSmiStatus(CpuIndex,EFI_SUCCESS);
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI SmmAmiBufferValidationLibSwDispatch (
    IN CONST EFI_GUID *Protocol, IN VOID *Interface, IN EFI_HANDLE Handle
){
	EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch = Interface;
	EFI_SMM_SW_REGISTER_CONTEXT Context;
	EFI_HANDLE SmiHandle;
	EFI_STATUS Status;

	if (SmmAmiBufferValidationLibSmmIsLocked) return EFI_SUCCESS;
	if (SmmAmiBufferValidationLibSmmMailBox != NULL) return EFI_SUCCESS;
	Status = gSmst->SmmLocateProtocol(&gEfiSmmCpuProtocolGuid, NULL, &SmmAmiBufferValidationLibSmmCpu);
	ASSERT_EFI_ERROR(Status);
	if (EFI_ERROR(Status)) return EFI_SUCCESS;
	
	Context.SwSmiInputValue = PcdGet8(AmiPcdSmmCommunicationBufferSwSmi);
	Status = SwDispatch->Register(
		SwDispatch, SmmAmiBufferValidationLibSmmMailBoxHandler,&Context, &SmiHandle
	);
	if ( EFI_ERROR(Status) ) return EFI_SUCCESS;
	SmmAmiBufferValidationLibCommunicationBufferSize = PcdGet32(AmiPcdSmmCommunicationBufferSize);
    Status = gBS->AllocatePages(
    	AllocateAnyPages, EfiRuntimeServicesData, 
    	EFI_SIZE_TO_PAGES(SmmAmiBufferValidationLibCommunicationBufferSize),
    	&SmmAmiBufferValidationLibCommunicationBuffer
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;
    
    Status = gBS->AllocatePool(
    	EfiRuntimeServicesData, sizeof(AMI_SMM_COMMUNICATION_MAILBOX),
    	(VOID **)&SmmAmiBufferValidationLibSmmMailBox
    );
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)){
    	gBS->FreePages(
    			SmmAmiBufferValidationLibCommunicationBuffer,
    		EFI_SIZE_TO_PAGES(SmmAmiBufferValidationLibCommunicationBufferSize)
    	);
    	SmmAmiBufferValidationLibSmmMailBox = NULL;
    }
    
    return EFI_SUCCESS;
}

AMI_SMM_BUFFER_VALIDATION_PROTOCOL SmmAmiBufferValidationLibValidationProtocol = {
		SmmAmiBufferValidationLibAmiValidateMemoryBuffer, 
		SmmAmiBufferValidationLibAmiValidateMmioBuffer, 
		AmiValidateSmramBuffer
};

EFI_STATUS EFIAPI SmmCoreAmiBufferValidationLibConstructor(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){
    EFI_STATUS Status;
    EFI_SMM_ACCESS2_PROTOCOL *SmmAccess;
    EFI_SMRAM_DESCRIPTOR *SmramRanges;
    UINTN Size;
    UINTN i;
    
    // Get SMRAM information
    Status = gBS->LocateProtocol (&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&SmmAccess);
    if (EFI_ERROR(Status)) return Status;
    
    Size = 0;
    Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
    ASSERT (Status == EFI_BUFFER_TOO_SMALL);
    if (Status != EFI_BUFFER_TOO_SMALL) return Status;
    Status = gBS->AllocatePool (EfiBootServicesData, Size, (VOID **)&SmramRanges);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    Status = SmmAccess->GetCapabilities (SmmAccess, &Size, SmramRanges);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)){
    	gBS->FreePool(SmramRanges);
    	return Status;
    }
    SmmAmiBufferValidationLibNumberOfSmramRanges = Size / sizeof (EFI_SMRAM_DESCRIPTOR);
    SmmAmiBufferValidationLibSmramRanges = AllocatePool (
            SmmAmiBufferValidationLibNumberOfSmramRanges * sizeof(SMM_AMI_BUFFER_VALIDATION_LIB_ADDRESS_RANGE)
    );
    ASSERT (SmmAmiBufferValidationLibSmramRanges != NULL);
    if (SmmAmiBufferValidationLibSmramRanges == NULL){
        SmmAmiBufferValidationLibNumberOfSmramRanges = 0;
        gBS->FreePool(SmramRanges);
        return EFI_OUT_OF_RESOURCES;
    }
    for(i=0; i<SmmAmiBufferValidationLibNumberOfSmramRanges; i++){
        SmmAmiBufferValidationLibSmramRanges[i].Address = SmramRanges[i].CpuStart;
        SmmAmiBufferValidationLibSmramRanges[i].Size = SmramRanges[i].PhysicalSize;
    }
    gBS->FreePool(SmramRanges);
    
    Status = gSmst->SmmRegisterProtocolNotify(
        &gEfiSmmReadyToLockProtocolGuid,
        SmmAmiBufferValidationLibReadyToLock,
        &SmmAmiBufferValidationLibReadyToLockRegistration
    );
    ASSERT_EFI_ERROR (Status);
    Status = gSmst->SmmInstallProtocolInterface(
        &SmmAmiBufferValidationLibValidationProtocolHandle, 
        &gAmiSmmBufferValidationProtocolGuid, 
        EFI_NATIVE_INTERFACE, &SmmAmiBufferValidationLibValidationProtocol
    );
    ASSERT_EFI_ERROR (Status);

    Status = gSmst->SmmRegisterProtocolNotify(
        &gEfiSmmSwDispatch2ProtocolGuid,
        SmmAmiBufferValidationLibSwDispatch,
        &SmmAmiBufferValidationLibSwDispatchRegistration
    );
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI SmmCoreAmiBufferValidationLibDestructor(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){
    if (SmmAmiBufferValidationLibSmramRanges != NULL) FreePool(SmmAmiBufferValidationLibSmramRanges);
    if (SmmAmiBufferValidationLibMemoryRanges != NULL) FreePool(SmmAmiBufferValidationLibMemoryRanges);
    if (SmmAmiBufferValidationLibMmioRanges != NULL) FreePool(SmmAmiBufferValidationLibMmioRanges);
    
    gSmst->SmmRegisterProtocolNotify(
        &gEfiSmmReadyToLockProtocolGuid, NULL, &SmmAmiBufferValidationLibReadyToLockRegistration
    );
    gSmst->SmmUninstallProtocolInterface(
    	SmmAmiBufferValidationLibValidationProtocolHandle,
    	&gAmiSmmBufferValidationProtocolGuid,
    	&SmmAmiBufferValidationLibValidationProtocol
    );
    
    gSmst->SmmRegisterProtocolNotify(
        &gEfiSmmSwDispatch2ProtocolGuid, NULL, &SmmAmiBufferValidationLibSwDispatchRegistration
    );
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
