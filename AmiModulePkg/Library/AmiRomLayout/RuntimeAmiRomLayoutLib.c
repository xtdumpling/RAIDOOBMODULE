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
  ROM Layout PEIM.
**/
#include <Library/AmiRomLayoutLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Guid/AmiRomLayout.h>

extern UINT32 gAmiRomLayoutVersion;
extern UINT32 gAmiRomAreaSize;
extern UINT32 gAmiRomLayoutSize;
extern AMI_ROM_AREA *gAmiRomLayout;
extern AMI_ROM_AREA *gAmiRomLayoutEnd;
static EFI_EVENT   RuntimeAmiRomLayoutLibExitBootServicesEvent = NULL;
static EFI_EVENT   RuntimeAmiRomLayoutLibVirtualAddressChangeEvent = NULL;

#define ConvertPointer(p) (gRT->ConvertPointer(0, (VOID **) &(p)));

VOID EFIAPI AmiRomLayoutLibExitBsNotifyEvent(IN EFI_EVENT Event, IN VOID *Context)
{
    gDS = NULL;
}

VOID EFIAPI AmiRomLayoutLibVirtualNotifyEvent(IN EFI_EVENT Event, IN VOID *Context)
{
	AMI_ROM_AREA *RomArea;

	if (gAmiRomLayout == NULL) return;

    for(RomArea = AmiGetFirstRomArea(); RomArea != NULL; RomArea = AmiGetNextRomArea(RomArea))
		ConvertPointer(RomArea->Address);

	ConvertPointer(gAmiRomLayout);
	ConvertPointer(gAmiRomLayoutEnd);
}

EFI_STATUS EFIAPI RuntimeAmiRomLayoutLibConstructor (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    AMI_ROM_LAYOUT_HOB *AmiRomLayoutHob = NULL;
    AMI_ROM_LAYOUT_HOB_ENTRY *AmiRomLayoutHobEntry = NULL;
    AMI_ROM_AREA *RomArea = NULL;

    Status = EfiGetSystemConfigurationTable(&gAmiRomLayoutGuid, (VOID**)&AmiRomLayoutHob);
    if(EFI_ERROR(Status))
    {
        AmiRomLayoutHob = GetFirstGuidHob (&gAmiRomLayoutGuid);
    }

    if(AmiRomLayoutHob != NULL)
    {
        gAmiRomLayoutVersion = AmiRomLayoutHob->Version;
        gAmiRomAreaSize = AmiRomLayoutHob->DescriptorSize;

        // So this calculation is a little trickier. The HobLength from the HOB minus the AMI_ROM_LAYOUT_HOB size gives the total size
        //  consumed by the Hob Entries (In AMI_ROM_LAYOUT_HOB_ENTRY sizes). Divide that number by the size of a AMI_ROM_LAYOUT_HOB_ENTRY
        //  size (as specified in the HOB) and then multipy that by the Descriptor Size (again from the header) and that gives the
        //  total size required for the RomLayouts
        gAmiRomLayoutSize = (UINT32) MultU64x32( DivU64x32( AmiRomLayoutHob->Header.Header.HobLength - sizeof(AMI_ROM_LAYOUT_HOB), AmiRomLayoutHob->HobEntrySize ), AmiRomLayoutHob->DescriptorSize);

        gAmiRomLayout = AllocateRuntimeZeroPool(gAmiRomLayoutSize);
        if(gAmiRomLayout != NULL)
        {
            gAmiRomLayoutEnd = (AMI_ROM_AREA*)((UINT8*)gAmiRomLayout + gAmiRomLayoutSize);

            // Go through the HOB and copy out the Rom Layout Information into the gAmiRomArea entries
            for(AmiRomLayoutHobEntry = (AMI_ROM_LAYOUT_HOB_ENTRY*)(AmiRomLayoutHob+1), RomArea = gAmiRomLayout;
                AmiRomLayoutHobEntry < (AMI_ROM_LAYOUT_HOB_ENTRY*)((UINT8*)(AmiRomLayoutHob+1) + AmiRomLayoutHob->Header.Header.HobLength - sizeof(AMI_ROM_LAYOUT_HOB));
                AmiRomLayoutHobEntry = (AMI_ROM_LAYOUT_HOB_ENTRY*)((UINT8*)AmiRomLayoutHobEntry + AmiRomLayoutHob->HobEntrySize), RomArea = (AMI_ROM_AREA*)((UINT8*)RomArea + gAmiRomAreaSize))
            {
                gBS->CopyMem(RomArea, &(AmiRomLayoutHobEntry->Area), gAmiRomAreaSize);
            }

            if(EFI_ERROR(Status))
            {
                AMI_ROM_LAYOUT_HOB *ConfigurationTable = NULL;
                ConfigurationTable = AllocateRuntimeZeroPool(AmiRomLayoutHob->Header.Header.HobLength);
                gBS->CopyMem(ConfigurationTable, AmiRomLayoutHob, AmiRomLayoutHob->Header.Header.HobLength);

                Status = gBS->InstallConfigurationTable(&gAmiRomLayoutGuid, (VOID*)ConfigurationTable);
            }
        }
    }

    gBS->CreateEvent(
        EVT_SIGNAL_EXIT_BOOT_SERVICES, TPL_CALLBACK, AmiRomLayoutLibExitBsNotifyEvent, NULL, 
        &RuntimeAmiRomLayoutLibExitBootServicesEvent
    );
    gBS->CreateEvent(
        EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE, TPL_CALLBACK, AmiRomLayoutLibVirtualNotifyEvent, NULL, 
        &RuntimeAmiRomLayoutLibVirtualAddressChangeEvent
    );
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI RuntimeAmiRomLayoutLibDestructor (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    gBS->CloseEvent(RuntimeAmiRomLayoutLibExitBootServicesEvent);
    gBS->CloseEvent(RuntimeAmiRomLayoutLibVirtualAddressChangeEvent);
    
    return EFI_SUCCESS;
}

/**
 * @param AreaGuid
 * @return
 */
EFI_STATUS AmiPublishFvArea(IN AMI_ROM_AREA *FvArea)
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    EFI_HANDLE FvHandle = NULL;
    
    // gDS is reset to NULL in the exit-boot-services callback
    // No FV publishing at runtime.
    if (gDS==NULL) return EFI_UNSUPPORTED;
    
    if(FvArea != NULL)
    {
        if (FvArea->Address == AMI_ROM_AREA_NOT_MEMORY_MAPPED) return EFI_NO_MAPPING;
        Status = gDS->ProcessFirmwareVolume((VOID*)(FvArea->Address), FvArea->Size, &FvHandle);
    }
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
