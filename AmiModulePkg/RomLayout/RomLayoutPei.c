
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

#include <Token.h>
#include <AmiPeiLib.h>
#include <AmiHobs.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Ppi/AmiReadyToLoadDxeCore.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <PiPei.h>
#include <Library/PerformanceLib.h>
#include <Guid/AmiRomLayout.h>
#include <Library/AmiRomLayoutLib.h>
#include <AmiHobs.h>
#include <AmiRomLayoutHooks.h>

#pragma pack(push)
#pragma pack(1)
typedef struct {
    UINT32 Version;             ///< Version from the AMI_ROM_LAYOUT_HEADER structure
    UINT32 DescriptorSize;      ///< DescriptorSize from the AMI_ROM_LAYOUT_HEADER structure
    UINT32 HobEntrySize;        ///< Total size of each HOB entry
    UINT32 Reserved;            ///< Reserved for future use
} AMI_ROM_LAYOUT_HOB_PAYLOAD;


// Legacy definitions included
typedef struct {
    EFI_PHYSICAL_ADDRESS Address;
    UINT32 Offset;
    UINT32 Size;
    AMI_ROM_AREA_TYPE Type;
    UINT32 Attributes;
} ROM_AREA;
#pragma pack(pop)

EFI_STATUS EFIAPI ReportFV2PeiAfterMem(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *NullPpi);

static EFI_PEI_NOTIFY_DESCRIPTOR MemoryAvailableNotify[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiMemoryDiscoveredPpiGuid,
        ReportFV2PeiAfterMem
    }
};

EFI_STATUS EFIAPI ReportFv2DxeWrapper(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *NullPpi);

static EFI_PEI_NOTIFY_DESCRIPTOR ReadyToLoadDxeCoreNotify[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAmiReadyToLoadDxeCorePpiGuid,
        ReportFv2DxeWrapper
    }
};

typedef BOOLEAN (PEI_PROCESS_FV_BEFORE_PUBLISHING)(
    IN CONST EFI_PEI_SERVICES **PeiServices, IN OUT AMI_ROM_AREA *Area, IN UINT32 FvType
);

PEI_PROCESS_FV_BEFORE_PUBLISHING* PeiProcessFvBeforePublishingList[]=
    { PeiProcessFvBeforePublishing NULL };

typedef BOOLEAN (PROCESS_FV_BEFORE_PUBLISHING)(
    IN CONST EFI_PEI_SERVICES **PeiServices, IN OUT ROM_AREA *Area, IN UINT32 FvType
);
extern PROCESS_FV_BEFORE_PUBLISHING LegacyProcessFvBeforePublishing EndOfLegacyProcessFvBeforePublishing;
PROCESS_FV_BEFORE_PUBLISHING *LegacyProcessFvBeforePublishingList[]=
    { LegacyProcessFvBeforePublishing NULL };

/**
 * Porting hook to support OEM specific FV validation or additional Fv processing. The
 * Area that is passed in is allowed to be modified by the callee functions.
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 * @param Area Pointer to the RomArea being checked
 * @param FvType The attributes of the RomArea that is being checked
 *
 * @retval TRUE Validation of the FV passed, continue processing it.
 * @retval FALSE Validation of the FV failed, don't continue processing it.
 */
BOOLEAN PeiProcessFvBeforePublishingHook(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN OUT AMI_ROM_AREA *Area,
    IN UINT32 FvType)
{
    UINTN i;
    BOOLEAN Result = TRUE;
    for(i=0; PeiProcessFvBeforePublishingList[i] && Result; i++)
    {
        Result = PeiProcessFvBeforePublishingList[i](PeiServices, Area, FvType);
        if(Result == FALSE)
            break;
    }

    if(Result != FALSE)
    {
        ROM_AREA LegacyRomArea;

        LegacyRomArea.Address = Area->Address;
        LegacyRomArea.Offset = Area->Offset;
        LegacyRomArea.Size = Area->Size;
        LegacyRomArea.Type = Area->Type;
        LegacyRomArea.Attributes = Area->Attributes;

        for(i = 0; LegacyProcessFvBeforePublishingList[i] != NULL && Result; i++)
        {
            Result = LegacyProcessFvBeforePublishingList[i](PeiServices, &LegacyRomArea ,FvType);
            if(Result == FALSE)
                break;

            if(LegacyRomArea.Address != Area->Address)
                Area->Address = LegacyRomArea.Address;

            if(LegacyRomArea.Offset != Area->Offset)
                Area->Offset = LegacyRomArea.Offset;

            if(LegacyRomArea.Size != Area->Size)
                Area->Size = LegacyRomArea.Size;

            if(LegacyRomArea.Type != Area->Type)
                Area->Type = LegacyRomArea.Type;

            if(LegacyRomArea.Attributes != Area->Attributes)
                Area->Attributes = LegacyRomArea.Attributes;
        }
    }

    return Result;
}

/**
 * Function to remove one, or all, of the FV HOBs that already exist in the HOB list.
 * If Area is null, all FV HOBs will be removed. If Area is non-null, only the FV HOB
 * that matches the address will be removed.
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table.
 * @param Area OPTIONAL parameter that specifies the area that should be removed from the FV HOB list.
 */
VOID RemoveFvHob(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN AMI_ROM_AREA         *Area OPTIONAL
)
{
    EFI_HOB_FIRMWARE_VOLUME *FvHob;

    if(Area != NULL)
    {
        for(FvHob = GetFirstHob(EFI_HOB_TYPE_FV); FvHob != NULL; FvHob = GetNextHob(EFI_HOB_TYPE_FV, GET_NEXT_HOB(FvHob)))
        {
            if (FvHob->BaseAddress == Area->Address)
                FvHob->Header.HobType = EFI_HOB_TYPE_UNUSED;
        }

        for(FvHob = GetFirstHob(EFI_HOB_TYPE_FV2); FvHob != NULL; FvHob = GetNextHob(EFI_HOB_TYPE_FV2, GET_NEXT_HOB(FvHob)))
        {
            if (FvHob->BaseAddress == Area->Address)
                FvHob->Header.HobType = EFI_HOB_TYPE_UNUSED;
        }
    }
    else
    {
        for(FvHob = GetFirstHob(EFI_HOB_TYPE_FV); FvHob != NULL; FvHob = GetNextHob(EFI_HOB_TYPE_FV, GET_NEXT_HOB(FvHob)))
            FvHob->Header.HobType = EFI_HOB_TYPE_UNUSED;

        for(FvHob = GetFirstHob(EFI_HOB_TYPE_FV2); FvHob != NULL; FvHob = GetNextHob(EFI_HOB_TYPE_FV2, GET_NEXT_HOB(FvHob)))
                FvHob->Header.HobType = EFI_HOB_TYPE_UNUSED;
    }

    return;
}

/**
 * Function to publish the Firmware Volume for a corresponding AMI_ROM_AREA. Function calls the
 * PeiProcessFvBeforePublishingHook prior to attempting to publish the FV.
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 * @param Area Pointer to the AMI_ROM_AREA that is requested to be published
 *
 * @retval EFI_INVALID_PARAMETER The Area is not a valid pointer
 * @retval EFI_VOLUME_CORRUPTED The Area being reported does not appear to be a valid Firmware Volume or the OEM hooks prevented the publishing of the Volume
 * @retval EFI_SUCCESS The Firmware Volume was reported to the system
 */
EFI_STATUS PublishFv(IN CONST EFI_PEI_SERVICES **PeiServices, IN AMI_ROM_AREA *Area)
{
    EFI_STATUS Status = EFI_SUCCESS;

    if(!PeiProcessFvBeforePublishingHook(PeiServices, Area, Area->Attributes))
        return EFI_VOLUME_CORRUPTED;

    Status = AmiPublishFvArea(Area);

    return Status;
}

/**
 * Function that will force the system into recovery mode
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 */
VOID InitRecovery(IN CONST EFI_PEI_SERVICES **PeiServices)
{
    static EFI_PEI_PPI_DESCRIPTOR RecoveryModePpi = {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiBootInRecoveryModePpiGuid, NULL
    };

    EFI_BOOT_MODE BootMode;
    EFI_STATUS Status;

    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);

    if(EFI_ERROR(Status) || (BootMode != BOOT_IN_RECOVERY_MODE)) {
        (*PeiServices)->SetBootMode(PeiServices, BOOT_IN_RECOVERY_MODE);
        (*PeiServices)->InstallPpi(PeiServices, &RecoveryModePpi);
        PEI_PROGRESS_CODE(PeiServices, PEI_RECOVERY_AUTO);
    }
}

/**
 * Function to relocate a firmware volume into system memory.
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 * @param Area Pointer to the AMI_ROM_AREA that should be moved into memory, and whose Address should be updated with the memory address
 *
 * @retval EFI_SUCCESS The firmware volume was relocated correctly
 */
EFI_STATUS RelocateFv(IN CONST EFI_PEI_SERVICES **PeiServices, IN OUT AMI_ROM_AREA *Area)
{
    EFI_STATUS Status;
    EFI_PHYSICAL_ADDRESS Dest;
    UINTN Pages;

    Pages = EFI_SIZE_TO_PAGES(Area->Size);
    Status = (*PeiServices)->AllocatePages(PeiServices, EfiBootServicesData, Pages, &Dest);
    if(!EFI_ERROR(Status))
    {
        CopyMem((VOID *)(UINTN)Dest, (VOID *)(UINTN)Area->Address, Area->Size);
        Area->Address = Dest;
    }
    return Status;
}

/**
 * Notification function that is executed when the system memory is reported as installed. Function
 * will attempt to publish firmware volumes that required memory to be available.
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table.
 * @param NotifyDescriptor The notification descriptor for this notification event.
 * @param NullPpi Pointer to a NULL ppi.
 *
 * @retval EFI_SUCCESS All FVs were published sucessfully
 */
EFI_STATUS EFIAPI ReportFV2PeiAfterMem(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *NullPpi)
{
    AMI_ROM_AREA *Area;
    AMI_ROM_AREA Current;

    EFI_STATUS Status;
    EFI_BOOT_MODE BootMode;

    // Try to get the boot mode, if it cannot be retrieved, assume that we should boot with
    //  full configuration
    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    if(EFI_ERROR(Status))
    {
        DEBUG((DEBUG_WARN, "WARNING: Unable to determine BootMode\n"));
        BootMode = BOOT_WITH_FULL_CONFIGURATION;
    }

    PERF_START (NULL, "PRL-AM", NULL, 0);
    for(Area = AmiGetFirstRomArea(); Area != NULL; Area = AmiGetNextRomArea(Area))
    {
        Status = EFI_SUCCESS;
        CopyMem(&Current, Area, sizeof(AMI_ROM_AREA));

        // If this is a PEI Shadow FV
        if( (Current.Attributes & AMI_ROM_AREA_FV_PEI_SHADOW) == AMI_ROM_AREA_FV_PEI_SHADOW)
        {
            if(BootMode != BOOT_ON_S3_RESUME)
            {
                // Calling RelocateFv will copy the Current FV into memory and update the
                //  address of the FV
                Status = RelocateFv(PeiServices, &Current);
                if(!EFI_ERROR(Status))
                    Status = PublishFv(PeiServices, &Current);
            }
        }
        // If this should be reported after memory and it wasn't reported during the shadow or the PEI
        else if((Current.Attributes & AMI_ROM_AREA_FV_PEI_MEM) == AMI_ROM_AREA_FV_PEI_MEM &&
                (Current.Attributes & AMI_ROM_AREA_FV_PEI_SHADOW) == 0 &&
                (Current.Attributes & AMI_ROM_AREA_FV_PEI) == 0)
        {
            Status = PublishFv(PeiServices, &Current);
        }

        // If the Attributes does not have the Non Critical bit set, then recovery will
        //  be triggered because of a critical block corruption
        if(EFI_ERROR(Status) && (Current.Attributes & AMI_ROM_AREA_NON_CRITICAL) != AMI_ROM_AREA_NON_CRITICAL)
        {
            DEBUG((DEBUG_ERROR, "ERROR: Corrupt FirmwareVolume at address 0x%p\n", Current.Address));
            InitRecovery(PeiServices);
        }
    }
    PERF_END (NULL, "PRL-AM", NULL, 0);

    return EFI_SUCCESS;
}

/**
 * Notification function that is executed when it is time to report firmware volumes that may contain
 * the DXE core. This function will check if the system has a valid recovery capsule and is in recovery mode
 * and it will replace the Ami Rom Layout Hob with a new one that contains the recovery image's firmware layout.
 *
 * @param RecoveryCapsule OPTIONAL Pointer to the location in memory where the recovery capsule can be found
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 *
 * @retval EFI_SUCCESS
 */
EFI_STATUS ReportFV2Dxe(IN VOID* RecoveryCapsule OPTIONAL, IN CONST EFI_PEI_SERVICES **PeiServices)
{
    EFI_STATUS Status;

    AMI_ROM_AREA *Area = NULL;
    AMI_ROM_AREA *Next = NULL;
    AMI_ROM_AREA Current;

    UINT32 Version = 0;
    UINT32 DescriptorSize = 0;
    UINT32 RomLayoutSize = 0;
    AMI_ROM_LAYOUT_HOB *AmiRomLayoutHob = NULL;
    AMI_ROM_LAYOUT_HOB_ENTRY *RomLayoutHobEntry = NULL;

    if(RecoveryCapsule != NULL)
    {
        AMI_ROM_LAYOUT_HOB_PAYLOAD *PayLoad = NULL;

        Status = AmiGetImageRomLayout((VOID*)RecoveryCapsule, FLASH_SIZE, &Area, &Version, &DescriptorSize, &RomLayoutSize);
        if(EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "ERROR: ROM Layout is not found in the Recovery Image\n"));
            DEBUG((DEBUG_ERROR, "       Recovery will fail and boot will be attempted using current ROM image\n"));
            return Status;
        }
        RemoveFvHob(PeiServices, NULL);

        // Remove the existing AmiRomLayoutHob, that was reported for the flashpart
        AmiRomLayoutHob = GetFirstGuidHob(&gAmiRomLayoutGuid);
        AmiRomLayoutHob->Header.Header.HobType = EFI_HOB_TYPE_UNUSED;

        PayLoad = BuildGuidHob(&gAmiRomLayoutGuid, sizeof(AMI_ROM_LAYOUT_HOB) - sizeof(EFI_HOB_GUID_TYPE) + (UINT32)MultU64x32(sizeof(AMI_ROM_LAYOUT_HOB_ENTRY), (UINT32)DivU64x32(RomLayoutSize, DescriptorSize)));
        if(PayLoad != NULL)
        {
            PayLoad->Version = Version;
            PayLoad->DescriptorSize = DescriptorSize;
            PayLoad->HobEntrySize = sizeof(AMI_ROM_LAYOUT_HOB_ENTRY);
            PayLoad->Reserved = 0;

            for(Next = Area, RomLayoutHobEntry = (AMI_ROM_LAYOUT_HOB_ENTRY*)(PayLoad + 1);
                Next < (AMI_ROM_AREA*)((UINT8*)Area + RomLayoutSize);
                Next = (AMI_ROM_AREA*)((UINT8*)Next + DescriptorSize), RomLayoutHobEntry = RomLayoutHobEntry + 1)
            {
                CopyMem(&(RomLayoutHobEntry->Area), Next, DescriptorSize);
                RomLayoutHobEntry->Reserved = 0;
                RomLayoutHobEntry->Offset = (UINT32)((UINT8*)RomLayoutHobEntry - ((UINT8*)PayLoad - sizeof(EFI_HOB_GUID_TYPE)));
            }
        }
    }

    for(Area = AmiGetFirstRomArea(); Area != NULL; Area = AmiGetNextRomArea(Area))
    {
        CopyMem(&Current, Area, sizeof(AMI_ROM_AREA));

        if(RecoveryCapsule != NULL)
            Current.Address = (EFI_PHYSICAL_ADDRESS)((UINTN)RecoveryCapsule + Current.Offset);

        if((Current.Attributes & (AMI_ROM_AREA_FV_DXE)) == AMI_ROM_AREA_FV_DXE)
        {
            if(RecoveryCapsule == NULL &&
                (Current.Attributes & (AMI_ROM_AREA_FV_PEI)) == 0 &&
                (Current.Attributes & (AMI_ROM_AREA_FV_PEI_MEM)) == 0 &&
                (Current.Attributes & (AMI_ROM_AREA_FV_PEI_SHADOW)) == 0)
            {
                Status = PublishFv(PeiServices, &Current);

                if(EFI_ERROR(Status) && ((Current.Attributes & AMI_ROM_AREA_NON_CRITICAL) != AMI_ROM_AREA_NON_CRITICAL))
                {
                    DEBUG((DEBUG_ERROR, "ERROR: Corrupt FirmwareVolume at address 0x%p\n", Current.Address));
                    InitRecovery(PeiServices);
                }
            }
            else if(RecoveryCapsule != NULL)
            {
                // Just report the FV because the system is already in recovery mode.
                Status = PublishFv(PeiServices, &Current);
            }
        }
        // if the firmware volume is marked PEI (and not dxe) and we are loading the recovery image
        //  then the firmware volume needs to be republished to allow the dxe stage to access the
        //  FFS files that are contained in the firmware volume
        else if(RecoveryCapsule != NULL)
        {
            if( (Current.Attributes & (AMI_ROM_AREA_FV_PEI)) == AMI_ROM_AREA_FV_PEI ||
                (Current.Attributes & (AMI_ROM_AREA_FV_PEI_MEM)) == AMI_ROM_AREA_FV_PEI_MEM ||
                (Current.Attributes & (AMI_ROM_AREA_FV_PEI_SHADOW)) == AMI_ROM_AREA_FV_PEI_SHADOW)
            {
                Status = PublishFv(PeiServices, &Current);
            }
        }
    }

    return EFI_SUCCESS;
}

/**
 * Function that will create the Legacy Rom Layout HOB for consumption by modules which have not yet
 * switched to using the new interfaces.
 */
VOID BuildLegacyRomLayoutHob(){
    EFI_STATUS Status;

    static EFI_GUID LegacyRomLayoutHobGuid = AMI_ROM_LAYOUT_HOB_GUID;
    ROM_AREA *LegacyRomArea;
    AMI_ROM_AREA *Area;
    UINT32 RomAreaSize, RomLayoutSize;

    Status = AmiGetRomLayoutProperties(NULL,&RomAreaSize,&RomLayoutSize);
    if (EFI_ERROR(Status)) return;

    LegacyRomArea = BuildGuidHob( &LegacyRomLayoutHobGuid, (RomLayoutSize/RomAreaSize+1) * sizeof(ROM_AREA) );
    if ( LegacyRomArea == NULL ) return;

    for( Area = AmiGetFirstRomArea(); Area != NULL; Area = AmiGetNextRomArea(Area), LegacyRomArea++ ){
        LegacyRomArea->Address = Area->Address;
        LegacyRomArea->Attributes = Area->Attributes;
        LegacyRomArea->Offset = Area->Offset;
        LegacyRomArea->Size = Area->Size;
        LegacyRomArea->Type = (AMI_ROM_AREA_TYPE)Area->Type;
    }
    LegacyRomArea->Address = 0;
    LegacyRomArea->Attributes = 0;
    LegacyRomArea->Offset = 0;
    LegacyRomArea->Size = 0;
    LegacyRomArea->Type = 0;
}

/**
  Notification function given controll by the installation of the
  ReadyToLoadDxeCore GUID.

  This function performs checks on recovery and installs the Hobs for
  the DXE core FV.

  @param  **PeiServices Describes the list of possible PEI Services.
  @param  NotifyDescriptor Descriptor used to store data about notifying this functino
  @param  NullPpi Address of the PPI installed with the GUID

  @retval EFI_SUCESS  The entry point of DXE IPL PEIM executes successfully.
  @retval Others      Some error occurs during the execution of this function.
**/
EFI_STATUS EFIAPI ReportFv2DxeWrapper(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *NullPpi)
{
    EFI_BOOT_MODE BootMode;
    EFI_STATUS Status;
    RECOVERY_IMAGE_HOB *RecoveryHob = NULL;
    EFI_HOB_GENERIC_HEADER *Hobs = NULL;

    PERF_START (NULL, "PRL-DX", NULL, 0);
    BootMode = GetBootModeHob ();
    if (BootMode == BOOT_IN_RECOVERY_MODE || BootMode == BOOT_ON_FLASH_UPDATE && PcdGetBool(PcdUseNewImageOnFlashUpdate))
    {
        Status = (*PeiServices)->GetHobList(PeiServices, (VOID**)&Hobs);
        if(EFI_ERROR(Status))
            return Status;

        RecoveryHob = GetNextGuidHob(&gAmiRecoveryImageHobGuid, (VOID*)Hobs);
        if(RecoveryHob != NULL)
        {
            if(RecoveryHob->Status == EFI_SUCCESS && RecoveryHob->Address != 0)
            {
                Status = ReportFV2Dxe((VOID *)(UINTN)RecoveryHob->Address, PeiServices);
                BuildLegacyRomLayoutHob();
                if(EFI_ERROR(Status))
                {
                    (*PeiServices)->ReportStatusCode(PeiServices, EFI_ERROR_CODE|EFI_ERROR_MAJOR, PEI_RECOVERY_FAILED, 0, NULL, NULL);

                    //if we're here either we were on normal boot path or recovery failed, or we use current image on flash update path
                    //if recovery failed based on token value we have to revert to normal boot path
                    if(!PcdGetBool(PcdForceSetupOnFailedRecovery))
                        (*PeiServices)->SetBootMode(PeiServices, BOOT_WITH_FULL_CONFIGURATION);
                }
                return Status;
            }
        }
    }

    Status = ReportFV2Dxe(NULL, PeiServices);
    BuildLegacyRomLayoutHob();
    PERF_END (NULL, "PRL-DX", NULL, 0);
    return Status;
}

/**
  Entry point of the ReportFv Module.

  This module provides the interfaces for reporting FVs in Pei.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCESS  The entry point of DXE IPL PEIM executes successfully.
  @retval Others      Some error occurs during the execution of this function.
**/
EFI_STATUS
EFIAPI
RomLayoutPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS Status;

    AMI_ROM_AREA *Area;
    AMI_ROM_AREA Current;

    PERF_START (NULL, "PRL-Ent", NULL, 0);
    for(Area = AmiGetFirstRomArea(); Area != NULL; Area = AmiGetNextRomArea(Area))
    {
        CopyMem(&Current, Area, sizeof(AMI_ROM_AREA));

        // If this is a PEI volume that is not marked as shadow
        if( (Current.Attributes & AMI_ROM_AREA_FV_PEI) == AMI_ROM_AREA_FV_PEI &&
            (Current.Attributes & AMI_ROM_AREA_FV_PEI_SHADOW) == 0)
        {
            Status = PublishFv(PeiServices, &Current);
            if(EFI_ERROR(Status) && ((Current.Attributes & AMI_ROM_AREA_NON_CRITICAL) != AMI_ROM_AREA_NON_CRITICAL))
                InitRecovery(PeiServices);
        }
    }
    PERF_END (NULL, "PRL-Ent", NULL, 0);

    Status = (*PeiServices)->NotifyPpi(PeiServices, MemoryAvailableNotify);

    Status = (*PeiServices)->NotifyPpi(PeiServices, ReadyToLoadDxeCoreNotify);
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
