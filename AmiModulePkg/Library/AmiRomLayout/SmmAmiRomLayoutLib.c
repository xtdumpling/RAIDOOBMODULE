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
#include <Library/SmmServicesTableLib.h>
#include <Protocol/AmiRomLayout.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Guid/AmiRomLayout.h>
#include <Library/BaseMemoryLib.h>

extern UINT32 gAmiRomLayoutVersion;
extern UINT32 gAmiRomAreaSize;
extern UINT32 gAmiRomLayoutSize;
extern AMI_ROM_AREA *gAmiRomLayout;
extern AMI_ROM_AREA *gAmiRomLayoutEnd;

EFI_STATUS EFIAPI SmmAmiRomLayoutLibConstructor (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    AMI_ROM_LAYOUT_HOB *AmiRomLayoutHob = NULL;
    AMI_ROM_LAYOUT_HOB_ENTRY *AmiRomLayoutHobEntry = NULL;
    AMI_ROM_AREA *RomArea = NULL;
    UINTN Index;

    BOOLEAN InstallSmmConfigurationTable = FALSE;

    for(Index = 0; Index < gSmst->NumberOfTableEntries; Index++)
    {
        if(CompareGuid (&(gSmst->SmmConfigurationTable[Index].VendorGuid), &gAmiRomLayoutGuid))
        {
            AmiRomLayoutHob = (AMI_ROM_LAYOUT_HOB*) gSmst->SmmConfigurationTable[Index].VendorTable;
        }
    }
    if(AmiRomLayoutHob == NULL)
    {
        AmiRomLayoutHob = GetFirstGuidHob (&gAmiRomLayoutGuid);
        InstallSmmConfigurationTable = TRUE;
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

        Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, gAmiRomLayoutSize, &gAmiRomLayout);
        if(!EFI_ERROR(Status))
        {
            gAmiRomLayoutEnd = (AMI_ROM_AREA*)((UINT8*)gAmiRomLayout + gAmiRomLayoutSize);

            // Go through the HOB and copy out the Rom Layout Information into the gAmiRomArea entries
            for(AmiRomLayoutHobEntry = (AMI_ROM_LAYOUT_HOB_ENTRY*)(AmiRomLayoutHob+1), RomArea = gAmiRomLayout;
                AmiRomLayoutHobEntry < (AMI_ROM_LAYOUT_HOB_ENTRY*)((UINT8*)(AmiRomLayoutHob+1) + AmiRomLayoutHob->Header.Header.HobLength - sizeof(AMI_ROM_LAYOUT_HOB));
                AmiRomLayoutHobEntry = (AMI_ROM_LAYOUT_HOB_ENTRY*)((UINT8*)AmiRomLayoutHobEntry + AmiRomLayoutHob->HobEntrySize), RomArea = (AMI_ROM_AREA*)((UINT8*)RomArea + gAmiRomAreaSize))
            {
                CopyMem(RomArea, &(AmiRomLayoutHobEntry->Area), gAmiRomAreaSize);
            }

            if(InstallSmmConfigurationTable)
            {
                AMI_ROM_LAYOUT_HOB *ConfigurationTable = NULL;
                Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData, AmiRomLayoutHob->Header.Header.HobLength, &ConfigurationTable);
                CopyMem(ConfigurationTable, AmiRomLayoutHob, AmiRomLayoutHob->Header.Header.HobLength);

                Status = gSmst->SmmInstallConfigurationTable(gSmst, &gAmiRomLayoutGuid, (VOID*)AmiRomLayoutHob, AmiRomLayoutHob->Header.Header.HobLength);
            }
        }
    }
	return EFI_SUCCESS;
}


/**
 * @param AreaGuid
 * @return
 */
EFI_STATUS AmiPublishFvArea(IN AMI_ROM_AREA *FvArea)
{
    // FV Area publishing is not supported in SMM.
    // The support can potentially be added in the future if required, 
    // but it has to be done with the following restriction in mind:
    // - No FV publishing after End Of DXE because it's a call outside of SMM.
    return EFI_UNSUPPORTED;
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
