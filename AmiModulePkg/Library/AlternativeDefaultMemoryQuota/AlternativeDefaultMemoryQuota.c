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

#include <Guid/MemoryTypeInformation.h>
/** @file
  @brief Defines table with project specific memory allocation quotas.
**/
// Defined in AmiDxeIpl.c (part of DxeIpl PEIM)
extern CONST EFI_MEMORY_TYPE_INFORMATION *DefaultMemoryTypeInformationPtr;
extern UINTN DefaultMemoryTypeInformationSize;

// The order of elements in this array is important.
// It defines the order of memory types in the DXE memory map.
// The first array element corresponds to the type with the largest address.
// Keeping boot time memory types at the bottom of the list improves 
// stability of the runtime portions of the memory map
// which is important during S4 resume.
CONST EFI_MEMORY_TYPE_INFORMATION CrbDefaultMemoryTypeInformation[] = {
#include <AlternativeDefaultMemoryQuota.h>
};

/// Library constructor
EFI_STATUS EFIAPI SetAlternativeDefaultMemoryQuota (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
    DefaultMemoryTypeInformationPtr = CrbDefaultMemoryTypeInformation;
    DefaultMemoryTypeInformationSize = sizeof(CrbDefaultMemoryTypeInformation);
    return EFI_SUCCESS;
}
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
