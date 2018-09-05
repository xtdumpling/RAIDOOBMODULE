//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
#include <PiPei.h>
#include <Uefi/UefiSpec.h>
#include<Library/BaseMemoryLib.h>
#include<Library/PeiServicesLib.h>
#include <Capsule.h>
#include <FlashUpd.h>

EFI_STATUS EFIAPI ProcessCapsuleImage(
    IN EFI_CAPSULE_HEADER *CapsuleHeader
){
    static EFI_GUID gFWCapsuleGuid           = APTIO_FW_CAPSULE_GUID;
    static EFI_GUID gESRTCapsuleGuid =W8_FW_UPDATE_IMAGE_CAPSULE_GUID;
    static EFI_GUID gFlashUpdBootModePpiGuid = EFI_PEI_BOOT_IN_FLASH_UPDATE_MODE_PEIM_PPI;
    static EFI_PEI_PPI_DESCRIPTOR FlashUpdateModePpi = {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gFlashUpdBootModePpiGuid, NULL
    };

    if (   !CompareGuid (&CapsuleHeader->CapsuleGuid, &gFWCapsuleGuid) 
        && !CompareGuid (&CapsuleHeader->CapsuleGuid, &gESRTCapsuleGuid)
    ) return EFI_UNSUPPORTED;
    PeiServicesSetBootMode(BOOT_ON_FLASH_UPDATE);
    PeiServicesInstallPpi(&FlashUpdateModePpi);
	return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************