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

/** @file SmbiosPei.c
    This file provides function to update "Wake-up Type"
    data field in Smbios Type 1 structure

**/

#include <AmiPeiLib.h>
#include <Protocol/AmiSmbios.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Library/DebugLib.h>

//extern VOID		OemRuntimeShadowRamWrite(IN BOOLEAN Enable);
extern UINT8	getWakeupTypeForSmbios(VOID);
extern VOID     NbRuntimeShadowRamWrite(IN BOOLEAN Enable);

/**
    Detect and update SMBIOS Type 1 structure "Wake-up Type"
    data field

    @param PeiServices

    @retval VOID Updated SMBIOS Type 1 "Wake-up Type"

**/
VOID
UpdateSmbiosWakeupType(
    IN CONST EFI_PEI_SERVICES **PeiServices
)
{
    EFI_STATUS                          Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    UINTN                               DataSize = 4;
    UINT32                              WakeupTypePtr;

    Status = (*PeiServices)->LocatePpi(
                                        PeiServices,
                                        &gEfiPeiReadOnlyVariable2PpiGuid,
                                        0,
                                        NULL,
                                        (void**)&ReadOnlyVariable);
    ASSERT_PEI_ERROR(PeiServices, Status);

    if (Status == EFI_SUCCESS){
        Status = ReadOnlyVariable->GetVariable(
                                        ReadOnlyVariable,
                                        L"WakeUpType",
                                        &gAmiSmbiosNvramGuid,
                                        NULL,
                                        &DataSize,
                                        &WakeupTypePtr);
        ASSERT_PEI_ERROR(PeiServices, Status);

        PEI_TRACE((-1, PeiServices, "WakeupType location: %x", WakeupTypePtr));

        if (Status == EFI_SUCCESS) {
            if (WakeupTypePtr > 0xf0000) {
                *(UINT8*)WakeupTypePtr = getWakeupTypeForSmbios();
            }
            else {
//                OemRuntimeShadowRamWrite(TRUE);
                NbRuntimeShadowRamWrite(TRUE);
                *(UINT8*)WakeupTypePtr = getWakeupTypeForSmbios();
//                OemRuntimeShadowRamWrite(FALSE);
                NbRuntimeShadowRamWrite(FALSE);
            }
        }
    }
}

/**
    Driver entry point for SmbiosPei

    @param FfsHeader
    @param PeiServices

    @retval EFI_STATUS Status

**/
EFI_STATUS
EFIAPI
SmbiosPeiEntryPoint(
    IN       EFI_PEI_FILE_HANDLE    FileHandle,
  	IN CONST EFI_PEI_SERVICES       **PeiServices
)
{
    EFI_STATUS      Status;
    EFI_BOOT_MODE   BootMode;

    // Determine boot mode
    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Failed to get BootMode\n"));
    }

    if (BootMode == BOOT_ON_S3_RESUME) {
        UpdateSmbiosWakeupType(PeiServices);
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
