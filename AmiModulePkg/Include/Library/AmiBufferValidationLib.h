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

/** @file
  @brief Defines AmiBufferValidationLib library class.

  Buffer Validation Functions.
**/
#ifndef __AMI_BUFFER_VALIDATION_LIB__H__
#define __AMI_BUFFER_VALIDATION_LIB__H__

/// @name AMI Buffer Validation Library Properties
/// These flags can be used to soften validation policy by setting AmiPcdSmmMemLibProperties PCD token
///@{
/// Enables access to memory regions that after ExitBootServices are owned by OS.
/// When this flag is set, system is not compliant with Microsoft Device Guard requirements.
/// When this flag is not set, OS applications that allocate SMM Communication 
/// buffer using OS services and Firmware drivers that use boot time memory
/// for SMM Communication buffers will not work.
#define AMI_BUFFER_VALIDATION_LIB_ALLOW_ACCESS_TO_OS_MEMORY 1
/// Enables MMIO in the regions marked as reserved in GCD memory map.
#define AMI_BUFFER_VALIDATION_LIB_ALLOW_MMIO_IN_RESERVED_REGIONS 2
/// Enables MMIO in the regions marked as non-existent in GCD memory map.
/// This flag must be set if PCI bus enumeration happens after EndOfDxe,
/// which is the case when BDS SDL token BDS_START_PCI_BEFORE_END_OF_DXE is to Off.
/// Enabling MMIO in non-existent regions is required because depending on implementation
/// of the PCI root bridge driver, PCI resources may get added to GCD only during PCI enumeration.
/// Since AmiBufferValidationLib saves GCD map in the EndOfDxe callback, all non-existent memory
/// ranges have to be treated by the library as a potential PCI MMIO space.
#define AMI_BUFFER_VALIDATION_LIB_ALLOW_MMIO_IN_NON_EXISTENT_REGIONS 4
///@}

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
EFI_STATUS EFIAPI AmiValidateMemoryBuffer(CONST VOID* Buffer, CONST UINTN BufferSize);

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
EFI_STATUS EFIAPI AmiValidateMmioBuffer(CONST VOID* Buffer, CONST UINTN BufferSize);

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
EFI_STATUS EFIAPI AmiValidateSmramBuffer(CONST VOID* Buffer, CONST UINTN BufferSize);

#endif
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
