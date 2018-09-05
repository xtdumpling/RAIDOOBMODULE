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
#include <Uefi/UefiSpec.h>
#include <Library/BaseMemoryLib.h>
#include <Capsule.h>

EFI_STATUS EFIAPI SupportCapsuleImage(
    IN EFI_CAPSULE_HEADER *CapsuleHeader
){
    static EFI_GUID gFWCapsuleGuid           = APTIO_FW_CAPSULE_GUID;
    static EFI_GUID gESRTCapsuleGuid =W8_FW_UPDATE_IMAGE_CAPSULE_GUID;
    static EFI_GUID gW8ScreenImageGuid = W8_SCREEN_IMAGE_CAPSULE_GUID;
    static EFI_GUID gFmpCapsuleGuid =  { 0x6dcbd5ed, 0xe82d, 0x4c44, {0xbd, 0xa1, 0x71, 0x94, 0x19, 0x9a, 0xd9, 0x2a }};
    static EFI_GUID* SupportedCapsuleGuid[] = {
        &gFWCapsuleGuid, &gESRTCapsuleGuid, &gW8ScreenImageGuid, &gFmpCapsuleGuid,
        NULL
    };
    UINTN i;
    for(i = 0; SupportedCapsuleGuid[i] != NULL; i++){
        if (CompareGuid (SupportedCapsuleGuid[i], &CapsuleHeader->CapsuleGuid))
            return EFI_SUCCESS;
    }
    return EFI_UNSUPPORTED;
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
