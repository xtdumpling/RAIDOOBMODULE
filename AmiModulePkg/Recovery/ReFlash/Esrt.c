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

/** @file
  This file contains Reflash driver ESRT table related code

**/

#include <AmiHobs.h>
#include <Capsule.h>
#include <Ppi/FwVersion.h>
#include <Protocol/AmiReflashProtocol.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/EsrtManagement.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/SystemResourceTable.h>
#include "ReFlash.h"

#define FID_FFS_FILE_NAME_GUID \
    { 0x3fd1d3a2, 0x99f7, 0x420b, 0xbc, 0x69, 0x8b, 0xb1, 0xd4, 0x92, 0xa3, 0x32 }

#define FID_FFS_FILE_SECTION_GUID \
    { 0x2EBE0275, 0x6458, 0x4AF9, 0x91, 0xED, 0xD3, 0xF4, 0xED, 0xB1, 0x00, 0xAA }

typedef struct {
    UINT32 Version;
    UINT32 Status;
} AMI_FW_UPDATE_STATUS;

static EFI_GUID SystemFirmwareUpdateClass = W8_FW_UPDATE_IMAGE_CAPSULE_GUID;
static EFI_GUID guidHob = HOB_LIST_GUID;
static W8_IMAGE_CAPSULE *Image;
extern RECOVERY_IMAGE_HOB *RecoveryHob;
extern EFI_GUID gAmiGlobalVariableGuid;


EFI_STATUS EFIAPI GetDisplayImage(
    IN EFI_REFLASH_PROTOCOL *This,
    OUT UINTN               *CoordinateX,
    OUT UINTN               *CoordinateY,
    OUT VOID                **ImageAddress
);

EFI_STATUS EFIAPI FwUpdate(
        IN EFI_REFLASH_PROTOCOL *This
);

EFI_REFLASH_PROTOCOL AmiReflashProtocol = {
    FwUpdate,
    GetDisplayImage
};

UINT32 GetFirmwareVersion(
    VOID *Image OPTIONAL
);

/**
  This function returns current FID descriptor
  
  @param Fid     Pointer where to store FID descriptor

  @retval EFI_SUCCESS Layout returned successfully
  @retval other       error occured during execution

**/
EFI_STATUS GetFidFromFv(
    OUT VOID *Fid
)
{
    static EFI_GUID FidFileName = FID_FFS_FILE_NAME_GUID;
    EFI_STATUS Status;
    EFI_HANDLE *FvHandle;
    UINTN FvCount;
    UINTN i;
    UINTN BufferSize;
    VOID *Buffer;

	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL, &FvCount, &FvHandle);
	if (EFI_ERROR(Status)) 
        return Status;

	for(i = 0; i < FvCount; i++) {
	    EFI_FIRMWARE_VOLUME_PROTOCOL *Fv;
	    UINT32 AuthStatus;
	    
        Status = gBS->HandleProtocol(FvHandle[i], &gEfiFirmwareVolume2ProtocolGuid, &Fv);
		if (EFI_ERROR(Status)) 
            continue;
		
        Buffer = 0;
        BufferSize = 0;
	    Status = Fv->ReadSection(Fv, &FidFileName, EFI_SECTION_FREEFORM_SUBTYPE_GUID, 0, &Buffer, &BufferSize, &AuthStatus);
        if (!EFI_ERROR(Status)) {
            TRACE((-1, "extracted section with guid %g\n", (EFI_GUID *)Buffer));
            Buffer = (UINT8 *)Buffer + sizeof(EFI_GUID);
            CopyMem(Fid, Buffer, sizeof(FW_VERSION));
            Buffer = (UINT8 *)Buffer - sizeof(EFI_GUID);
            gBS->FreePool(Buffer);
            return EFI_SUCCESS;
        }
	}
	gBS->FreePool(FvHandle);
	return EFI_NOT_FOUND;
}

/**
  This function returns FID descriptor stored in provided buffer
  
  @param Fid     Pointer where to store FID descriptor
  @param Buffer  Pointer to the buffer to be searched

  @retval EFI_SUCCESS       Layout returned successfully
  @retval EFI_NOT_FOUND     There is no FID descriptor in buffer

**/
EFI_STATUS GetFidFromBuffer(
    IN VOID *Buffer,
    OUT VOID *Fid
)
{
    static EFI_GUID FidSectionGuid = FID_FFS_FILE_SECTION_GUID;
    UINT32 Signature;
    UINT32 *SearchPointer;

    SearchPointer = (UINT32 *)((UINT8 *)Buffer - sizeof(EFI_GUID) + FLASH_SIZE);
    Signature = FidSectionGuid.Data1;

    do {
        if(*SearchPointer == Signature) {
            if(CompareGuid(&FidSectionGuid, (EFI_GUID *)SearchPointer)) {
                SearchPointer = (UINT32 *)((UINT8 *)SearchPointer + sizeof(EFI_GUID));
                CopyMem(Fid, SearchPointer, sizeof(FW_VERSION));
                return EFI_SUCCESS;
            }
        }
    } while(SearchPointer-- >= (UINT32 *)Buffer);

    return EFI_NOT_FOUND;
}

/**
  This function returns firmware version from FID descriptor
  
  @param Image   Pointer to the recovery image (or NULL if current
                 image to be used)

  @return Firmware version

**/
UINT32 GetVersionFromFid(
    VOID *Image OPTIONAL
)
{
    FW_VERSION Fid;
    EFI_STATUS Status;
    CHAR16 cs[5];

    if(Image == NULL)
        Status = GetFidFromFv(&Fid);
    else
        Status = GetFidFromBuffer(Image, &Fid);

    if(EFI_ERROR(Status))
        return 0;

    cs[0] = Fid.ProjectMajorVersion[0];
    cs[1] = Fid.ProjectMajorVersion[1];
    cs[2] = Fid.ProjectMinorVersion[0];
    cs[3] = Fid.ProjectMinorVersion[1];
    cs[4] = 0;
    
    return (UINT32)StrDecimalToUintn(cs);
}

UINT32 GetErrorCode(
    IN UINT32 VerificationStatus
)
{
    switch(VerificationStatus) {
        case 1:
            return LAST_ATTEMPT_STATUS_ERROR_INVALID_FORMAT;
        case 2:
            return LAST_ATTEMPT_STATUS_ERROR_AUTH_ERROR;
        case 3:
            return LAST_ATTEMPT_STATUS_ERROR_AUTH_ERROR;
        case 4:
            return LAST_ATTEMPT_STATUS_ERROR_INCORRECT_VERSION;
        default:
            break;
    }
    return LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL;
}

EFI_STATUS InstallEsrtTable(
    VOID
)
{
    EFI_STATUS Status;
    UINTN Size = sizeof(AMI_FW_UPDATE_STATUS);
    EFI_SYSTEM_RESOURCE_ENTRY Entry;
    EFI_SYSTEM_RESOURCE_ENTRY Cached;
    ESRT_MANAGEMENT_PROTOCOL *EsrtMp;
    AMI_FW_UPDATE_STATUS FwStatus;
    static EFI_GUID InvalidGuid = INVALID_GUID;
    
    if(!PcdGetBool(AmiPcdInstallEsrtTable))
        return EFI_UNSUPPORTED;
    
    if (CompareGuid (&InvalidGuid, &SystemFirmwareUpdateClass))
        return EFI_UNSUPPORTED;
    
    Status = gBS->LocateProtocol (&gEsrtManagementProtocolGuid, NULL, &EsrtMp);
    if (EFI_ERROR (Status))
        return Status;

    Entry.FwClass = SystemFirmwareUpdateClass;
    Entry.FwType = ESRT_FW_TYPE_SYSTEMFIRMWARE;
    Entry.FwVersion = GetFirmwareVersion(NULL);
    Entry.LowestSupportedFwVersion = Entry.FwVersion;     //no rollback allowed
    Entry.CapsuleFlags = CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_INITIATE_RESET;

    Status = gRT->GetVariable(FW_VERSION_VARIABLE, &gAmiGlobalVariableGuid, NULL, &Size, &FwStatus);
    if(!EFI_ERROR(Status)) {
        Entry.LastAttemptVersion = FwStatus.Version;
        Entry.LastAttemptStatus = GetErrorCode(FwStatus.Status);
    } else {
        Entry.LastAttemptVersion = Entry.FwVersion;
        Entry.LastAttemptStatus = LAST_ATTEMPT_STATUS_SUCCESS;
    }

    Status = EsrtMp->GetEsrtEntry (&SystemFirmwareUpdateClass, &Cached);
    if (EFI_ERROR (Status)) {   //entry not found in cache
        Status = EsrtMp->RegisterEsrtEntry (&Entry);
    } else {
        if (CompareMem (&Cached, &Entry, sizeof (EFI_SYSTEM_RESOURCE_ENTRY)))
            Status = EsrtMp->UpdateEsrtEntry (&Entry);
    }
    return Status;
}

/**
  This function checks if we're on Windows 8 firmware update boot path
  
  @param RecoveryFailed Firmware update status flag
  
  @retval TRUE   We're on Windows 8 firmware update boot path
  @retval FALSE  We're not on Windows 8 firmware update boot path

**/
EFI_STATUS IsWin8Update(
    BOOLEAN RecoveryFailed
)
{
    VOID *HobStart;
    EFI_HOB_UEFI_CAPSULE *Hob;
    EFI_CAPSULE_HEADER *Capsule = NULL;
    static EFI_GUID ImageCapsuleGuid = W8_SCREEN_IMAGE_CAPSULE_GUID;
    static EFI_GUID guidBootFlow = BOOT_FLOW_VARIABLE_GUID;
    EFI_HANDLE Handle = NULL;
    EFI_STATUS Status;
    UINT32 BootFlow = BOOT_FLOW_CONDITION_OS_UPD_CAP;
    AMI_FW_UPDATE_STATUS FwStatus;
    BOOLEAN ImageHobFound = FALSE;

    HobStart = GetHobList ();
    if (HobStart == NULL)
        return EFI_NOT_FOUND;

    do {
        Hob = (EFI_HOB_UEFI_CAPSULE *) GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobStart);
        if(Hob != NULL) { 
            Capsule = (EFI_CAPSULE_HEADER *)(VOID *)(UINTN)Hob->BaseAddress;
            if(CompareGuid(&(Capsule->CapsuleGuid), &ImageCapsuleGuid)) {
                ImageHobFound = TRUE;
                break;
            }
            Hob = GET_NEXT_HOB (Hob);
        }
        HobStart = Hob;
    } while(HobStart != NULL);

    if(!ImageHobFound)   //no image hob - we're not on OS FW update path
        return EFI_NOT_FOUND;

/* set version we're upgrading to */
    FwStatus.Version = GetFirmwareVersion(RecoveryBuffer);
    FwStatus.Status = RecoveryHob->FailedStage;
    gRT->SetVariable(FW_VERSION_VARIABLE, &gAmiGlobalVariableGuid,
                              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                              sizeof(AMI_FW_UPDATE_STATUS), &FwStatus);

    if(RecoveryFailed) { //we're on OS FW update path, but recovery can't be performed
        InstallEsrtTable();
        return EFI_UNLOAD_IMAGE;
    }

//save image capsule pointer
    Image = (W8_IMAGE_CAPSULE *)Capsule;
//install reflash protocol
    Status = gBS->InstallMultipleProtocolInterfaces(
		                        &Handle,
		                        &gAmiReflashProtocolGuid, 
                                &AmiReflashProtocol,
		                        NULL);
    if(!EFI_ERROR(Status))
        //set boot flow
        gRT->SetVariable(L"BootFlow", &guidBootFlow, EFI_VARIABLE_BOOTSERVICE_ACCESS, sizeof(BootFlow), &BootFlow);

    return Status;
}

/**
  This function returns Windows 8 firmware update image attributes
  
  @param This           Pointer to the EFI_REFLASH_PROTOCOL instance
  @param CoordinateX    Pointer where to store image left corner horisontal coordinate
  @param CoordinateY    Pointer where to store image left corner vertical coordinate
  @param ImageAddress   Pointer where to store pointer to image data

  @retval EFI_SUCCESS   Image returned successfully
  @retval other         There is some error occured during execution

**/
EFI_STATUS EFIAPI GetDisplayImage(
    IN EFI_REFLASH_PROTOCOL *This,
    OUT UINTN               *CoordinateX,
    OUT UINTN               *CoordinateY,
    OUT VOID                **ImageAddress
)
{
    if(CoordinateX == NULL ||
       CoordinateY == NULL ||
       ImageAddress == NULL)
        return EFI_INVALID_PARAMETER;

    *CoordinateX = Image->ImageOffsetX;
    *CoordinateY = Image->ImageOffsetY;
    *ImageAddress = Image->Image;
    return EFI_SUCCESS;
}

/**
  This function performs Windows 8 firmware update
  
  @param This           Pointer to the EFI_REFLASH_PROTOCOL instance

  @retval EFI_SUCCESS   Firmware updated successfully
  @retval other         There is some error occured during execution

**/
EFI_STATUS EFIAPI FwUpdate(
        IN EFI_REFLASH_PROTOCOL *This
)
{
    EFI_STATUS Status;

    Status = Prologue(FALSE, TRUE);
    if(EFI_ERROR(Status))
        gRT->ResetSystem(EfiResetCold, Status, 0, NULL);

    Status = FlashWalker(FALSE);
    if(EFI_ERROR(Status))
        gRT->ResetSystem(EfiResetCold, Status, 0, NULL);

    Status = Epilogue();
    return Status;
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
