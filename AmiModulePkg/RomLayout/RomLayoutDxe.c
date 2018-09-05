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
  ROM Layout Dxe file.
**/
#include <Dxe.h>
#include <AmiRomLayout.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <AmiHobs.h>
#include <Guid/AmiRomLayout.h>
#include <Library/AmiRomLayoutLib.h>
#include <Protocol/AmiRomLayout.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PerformanceLib.h>

#include <AmiRomLayoutHooks.h>

typedef BOOLEAN (DXE_PROCESS_FV_BEFORE_PUBLISHING)(
    IN OUT AMI_ROM_AREA *Area
);

DXE_PROCESS_FV_BEFORE_PUBLISHING* DxeProcessFvBeforePublishingList[]=
    { DxeProcessFvBeforePublishing NULL };

static EFI_EVENT BdsEvent = NULL;

extern AMI_ROM_AREA *gAmiRomLayout;
extern UINT32 gAmiRomLayoutVersion;
extern UINT32 gAmiRomAreaSize;
extern UINT32 gAmiRomLayoutSize;
extern AMI_ROM_AREA *gAmiRomLayoutEnd;

EFI_STATUS EFIAPI DxeGetRomLayout(  IN AMI_ROM_LAYOUT_PROTOCOL *This,
                                    IN OUT UINT32 *BufferSize,
                                    IN OUT AMI_ROM_AREA *RomLayoutBuffer,
                                    OUT UINT32 *DescriptorSize,
                                    OUT UINT32 *DescriptorVersion OPTIONAL);
EFI_STATUS EFIAPI DxePublishFv(IN AMI_ROM_LAYOUT_PROTOCOL *This, IN EFI_GUID *FvName);
EFI_STATUS EFIAPI DxePublishFvArea(IN AMI_ROM_LAYOUT_PROTOCOL *This, IN AMI_ROM_AREA *FvArea);
BOOLEAN ProcessFvBeforePublishingHook(IN OUT AMI_ROM_AREA *Area);

AMI_ROM_LAYOUT_PROTOCOL AmiRomLayoutProtocol = { DxeGetRomLayout, DxePublishFv, DxePublishFvArea };

/**
 * GetRomLayout function of the AMI_ROM_LAYOUT_PROTOCOL. Function will return the
 * information about the current RomLayout
 *
 * @param This Pointer to the AMI_ROM_LAYOUT_PROTOCOL structure.
 * @param BufferSize Size, in bytes, of the buffer pointed to by the RomLayoutBuffer parameter.
 * @param RomLayoutBuffer Pointer to the buffer to fill with the AMI_ROM_AREA
 * @param DescriptorSize Size, in bytes, of each entry in the RomLayoutBuffer structure
 * @param DescriptorVersion OPTIONAL Version of the AMI_ROM_AREA structure that is being used in the system
 *  *
 * @retval EFI_INVALID_PARAMETER The parameter This or BufferSize is NULL
 * @retval EFI_BUFFER_TOO_SMALL The RomLayoutBuffer pointer is not large enough to store the AMI_ROM_AREA structures. The size is returned in the BufferSize parameter.
 * @retval EFI_INVALID_PARAMETER The RomLayoutBuffer, or the DescriptorSize parameter are NULL
 * @retval EFI_SUCCESS The Layout has been correctly returned by the function.
 */
EFI_STATUS EFIAPI DxeGetRomLayout(  IN AMI_ROM_LAYOUT_PROTOCOL *This,
                                    IN OUT UINT32 *BufferSize,
                                    IN OUT AMI_ROM_AREA *RomLayoutBuffer,
                                    OUT UINT32 *DescriptorSize,
                                    OUT UINT32 *DescriptorVersion OPTIONAL)
{
    if(This == NULL || BufferSize == NULL)
        return EFI_INVALID_PARAMETER;

    if(*BufferSize < gAmiRomLayoutSize)
    {
        *BufferSize = gAmiRomLayoutSize;
        return EFI_BUFFER_TOO_SMALL;
    }

    if( RomLayoutBuffer == NULL || DescriptorSize == NULL)
        return EFI_INVALID_PARAMETER;

    gBS->SetMem(RomLayoutBuffer, *BufferSize, 0);

    gBS->CopyMem(RomLayoutBuffer, gAmiRomLayout, gAmiRomLayoutSize);
    *BufferSize = gAmiRomLayoutSize;
    *DescriptorSize = gAmiRomAreaSize;

    if(DescriptorVersion != NULL)
        *DescriptorVersion = gAmiRomLayoutVersion;

    return EFI_SUCCESS;
}

/**
 * PublishFv function of the AMI_ROM_LAYOUT_PROTOCOL. Function will attempt to publish the
 * first firmware volume that matches the passed EFI_GUID.
 *
 * @param This Pointer to the AMI_ROM_LAYOUT_PROTOCOL structure
 * @param FvName Pointer to the EFI_GUID to match to a firmware volume, and to them attempt to publish
 *
 * @retval EFI_INVALID_PARAMETER This or FvName are NULL pointers
 * @retval EFI_NOT_FOUND The passed EFI_GUID did not match any of the EFI_GUIDS in the AmiRomLayout
 * @retval EFI_INVALID_PARAMETER Attempting to publish the FV failed because the OEM Hooks prevented it
 * @retval EFI_OUT_OF_RESOURCES The FV was not published due to lack of system resources
 * @retval EFI_VOLUME_CORRUPTED The FV was not published because it was not a valid Firmware Volume
 * @retval EFI_SUCCESS The firmware volume was published
 */
EFI_STATUS EFIAPI DxePublishFv(IN AMI_ROM_LAYOUT_PROTOCOL *This, IN EFI_GUID *FvName)
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_ROM_AREA *RomArea = NULL;
    AMI_ROM_AREA Current;

    RECOVERY_IMAGE_HOB *RecoveryHob = NULL;

    if(This != NULL && FvName != NULL)
    {
        RecoveryHob = GetFirstGuidHob(&gAmiRecoveryImageHobGuid);

        Status = EFI_NOT_FOUND;
        for(RomArea = AmiGetFirstRomArea(); RomArea != NULL; RomArea = AmiGetNextRomArea(RomArea))
        {
            gBS->CopyMem(&Current, RomArea, gAmiRomAreaSize);

            if(RecoveryHob != NULL && PcdGetBool(PcdUseNewImageOnFlashUpdate))
                Current.Address = (EFI_PHYSICAL_ADDRESS)((UINTN)(RecoveryHob->Address) + Current.Offset);

            if(CompareGuid(&(RomArea->Guid), FvName) )
            {
                gBS->CopyMem(&Current, RomArea, gAmiRomAreaSize);
                Status = DxePublishFvArea(This, &Current);
            }
        }
    }
    return Status;
}

/**
 * PublishFvArea function of the AMI_ROM_LAYOUT_PROTOCOL. Function will attempt to publish the
 * firmware volume pointed to by FvArea
 *
 * @param This Pointer to the AMI_ROM_LAYOUT_PROTOCOL structure
 * @param FvArea Pointer to the AMI_ROM_AREA to publish
 *
 * @retval EFI_INVALID_PARAMETER This or FvName are NULL pointers
 * @retval EFI_INVALID_PARAMETER Attempting to publish the FV failed because the OEM Hooks prevented it
 * @retval EFI_OUT_OF_RESOURCES The FV was not published due to lack of system resources
 * @retval EFI_VOLUME_CORRUPTED The FV was not published because it was not a valid Firmware Volume
 * @retval EFI_SUCCESS The firmware volume was published
 */
EFI_STATUS EFIAPI DxePublishFvArea(IN AMI_ROM_LAYOUT_PROTOCOL *This, IN AMI_ROM_AREA *FvArea)
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_ROM_AREA Current;

    RECOVERY_IMAGE_HOB *RecoveryHob = NULL;

    if(This != NULL && FvArea != NULL)
    {
        RecoveryHob = GetFirstGuidHob(&gAmiRecoveryImageHobGuid);
        
        gBS->CopyMem(&Current, FvArea, gAmiRomAreaSize);
        
        if(RecoveryHob != NULL && PcdGetBool(PcdUseNewImageOnFlashUpdate))
            Current.Address = (EFI_PHYSICAL_ADDRESS)((UINTN)(RecoveryHob->Address) + Current.Offset);

        if(ProcessFvBeforePublishingHook(&Current) == TRUE)
            Status = AmiPublishFvArea(&Current);
    }
    return Status;
}

/**
 * Porting hook to support OEM specific FV validation or additional Fv processing. The
 * Area that is passed in is allowed to be modified by the callee functions.
 *
 * @param Area Pointer to the AMI_ROM_AREA to pass to the OEM hoosk
 *
 * @retval TRUE Validation of the FV passed, continue processing it.
 * @retval FALSE Validation of the FV failed, don't continue processing it.
 */
BOOLEAN ProcessFvBeforePublishingHook(IN OUT AMI_ROM_AREA *Area)
{
    UINTN i;
    BOOLEAN Result = TRUE;

    for(i=0; DxeProcessFvBeforePublishingList[i]; i++)
    {
        Result = DxeProcessFvBeforePublishingList[i](Area);
        if(Result == FALSE)
            break;
    }
    return Result;
}

/**
 * Callback function executed when the BdsConnectDrivers protocol is installed by the BDS phase.
 * Callback code will report any firmware volumes with the AMI_ROM_AREA_FV_BDS attributes.
 *
 * @param Event Pointer to the EFI_EVENT
 * @param Context Pointer to the context for this event
 */
VOID EFIAPI ReportFvBdsCallback(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS Status;

    AMI_ROM_AREA *RomLayout = NULL;
    AMI_ROM_AREA Current;

    EFI_TPL TplValue;
    BOOLEAN NewFvReported = FALSE;

    RECOVERY_IMAGE_HOB *RecoveryHob = NULL;

    PERF_START(gImageHandle, "DRL-Bds", NULL, 0);

    RecoveryHob = GetFirstGuidHob(&gAmiRecoveryImageHobGuid);

    for(RomLayout = AmiGetFirstRomArea(); RomLayout != NULL; RomLayout = AmiGetNextRomArea(RomLayout))
    {
        if((RomLayout->Attributes & AMI_ROM_AREA_FV_BDS) == AMI_ROM_AREA_FV_BDS)
        {
            gBS->CopyMem(&Current, RomLayout, gAmiRomAreaSize);

            if(RecoveryHob != NULL && PcdGetBool(PcdUseNewImageOnFlashUpdate))
                Current.Address = (EFI_PHYSICAL_ADDRESS)((UINTN)(RecoveryHob->Address) + Current.Offset);

            if(ProcessFvBeforePublishingHook(&Current) == TRUE)
            {
                Status = AmiPublishFvArea(&Current);
                if(!EFI_ERROR(Status))
                    NewFvReported = TRUE;
            }
        }
    }
    PERF_END(gImageHandle, "DRL-Bds",  NULL, 0);

    PERF_START(gImageHandle, "DRL-Dis", NULL, 0);
    if(NewFvReported)
    {
        TplValue = gBS->RaiseTPL(TPL_HIGH_LEVEL);

        // Lower the TPL before calling Dispatch
        gBS->RestoreTPL(TPL_APPLICATION);

        gDS->Dispatch();

        // Restore the TPL before returning
        gBS->RaiseTPL(TplValue);
    }
    PERF_END(gImageHandle, "DRL-Dis",  NULL, 0);

    gBS->CloseEvent(BdsEvent);
    return;
}

#define BDS_CONNECT_DRIVERS_PROTOCOL_GUID \
    { 0x3aa83745, 0x9454, 0x4f7a, { 0xa7, 0xc0, 0x90, 0xdb, 0xd0, 0x2f, 0xab, 0x8e } }
static EFI_GUID BdsConnectDriversProtocolGuid = BDS_CONNECT_DRIVERS_PROTOCOL_GUID;

/**
 * Entry pointer to the ReportFvDxeDriver. Attempts to report fimware volumes with the
 * AMI_ROM_AREA_FV_DXE2 attributes as soon as this driver is launched. function also
 * creates a callback event for when the BDS phase installs the Bds Connect Drivers protocol
 * so that firmware volumes with the AMI_ROM_AREA_FV_BDS attributes will be reported.
 *
 * @param ImageHandle Pointer to the handle that corresponds to this image
 * @param SystemTable Pointer to the EFI_SYSTEM_TABLE
 *
 * @retval EFI_SUCCESS This driver's entry point executed correctly
 */
EFI_STATUS EFIAPI RomLayoutDxeEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    VOID *Registration = NULL;

    AMI_ROM_AREA *RomLayout = NULL;
    AMI_ROM_AREA Current;
    RECOVERY_IMAGE_HOB *RecoveryHob = NULL;

    PERF_START(gImageHandle, "DRL-Ent", NULL, 0);

    RecoveryHob = GetFirstGuidHob(&gAmiRecoveryImageHobGuid);

    for(RomLayout = AmiGetFirstRomArea(); RomLayout != NULL; RomLayout = AmiGetNextRomArea(RomLayout))
    {
        if( (RomLayout->Attributes & AMI_ROM_AREA_FV_DXE2) == AMI_ROM_AREA_FV_DXE2 &&
            (RomLayout->Attributes & AMI_ROM_AREA_FV_DXE) == 0)
        {
            gBS->CopyMem(&Current, RomLayout, gAmiRomAreaSize);

            if(RecoveryHob != NULL && PcdGetBool(PcdUseNewImageOnFlashUpdate))
                Current.Address = (EFI_PHYSICAL_ADDRESS)((UINTN)(RecoveryHob->Address) + Current.Offset);

            if(ProcessFvBeforePublishingHook(&Current) == TRUE)
            {
                Status = AmiPublishFvArea(&Current);
            }
        }
    }
    PERF_END(gImageHandle,"DRL-Ent",  NULL, 0);

    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, ReportFvBdsCallback, NULL, &BdsEvent);
    if(!EFI_ERROR(Status))
        Status = gBS->RegisterProtocolNotify(&BdsConnectDriversProtocolGuid, BdsEvent, &Registration);

    Status = gBS->InstallMultipleProtocolInterfaces(&ImageHandle,
                                                    &AmiRomLayoutProtocolGuid, &AmiRomLayoutProtocol,
                                                    NULL);

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
