//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#include <Pei.h>
#include <AmiPeiLib.h>
#include <token.h>
#include <Ppi/stall.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Hob.h>
#include <AmiHobs.h>
#include <BiosGuardDefinitions.h>
#include <Library/DebugLib.h>
#include <AmiCspLib.h>
#include <Cpu/CpuRegs.h>

#define BIOSGUARD_PACKAGE_CERT   524

//----------------------------------------------------------------------------
// Local prototypes
typedef struct
{
    EFI_PEI_DEVICE_RECOVERY_MODULE_PPI          *pDRM_Ppi;
    EFI_PEI_DEVICE_LOAD_RECOVERY_CAPSULE        SavedLoadCapsule;
    EFI_PEI_DEVICE_GET_RECOVERY_CAPSULE_INFO    GetRecoveryCapsuleInfo;
} DEVICE_RECOVERY_MODULE_INFO;

//----------------------------------------------------------------------------
// Local Variables
DEVICE_RECOVERY_MODULE_INFO gDeviceRecoveryModuleInfo[MAX_DEVICE_RECOVERY_MODULE];

EFI_PHYSICAL_ADDRESS
FindAvailableMemory (
    IN EFI_PEI_SERVICES      **PeiServices
)
{
    VOID                    *p;
    UINT8                   i;
    EFI_PHYSICAL_ADDRESS    TopOfMemory = 0xffffffff;

    for ((*PeiServices)->GetHobList(PeiServices, &p), i = 0; \
            !(FindNextHobByType(EFI_HOB_TYPE_MEMORY_ALLOCATION, &p));	i++) {
        if (TopOfMemory > ((EFI_HOB_MEMORY_ALLOCATION*)p)->AllocDescriptor.MemoryBaseAddress &&
             ((EFI_HOB_MEMORY_ALLOCATION*)p)->AllocDescriptor.MemoryBaseAddress > BIOSGUARD_RECOVERY_IMAGE_SIZE)
            TopOfMemory = ((EFI_HOB_MEMORY_ALLOCATION*)p)->AllocDescriptor.MemoryBaseAddress;
    }

    return (TopOfMemory - (BIOSGUARD_RECOVERY_IMAGE_SIZE & 0xFFF00000));
}

VOID
ReportBiosGuardRecoverySize (
    EFI_PEI_SERVICES **PeiServices,
    UINT32 Size
)
{
    static EFI_GUID RecoveryHobGuid = AMI_RECOVERY_IMAGE_HOB_GUID;
    EFI_STATUS Status;
    RECOVERY_IMAGE_HOB *RecoveryHob;
    
    Status = (*PeiServices)->GetHobList(PeiServices, (VOID **)&RecoveryHob);
    if(!EFI_ERROR(Status)) {
        Status = FindNextHobByGuid(&RecoveryHobGuid, (VOID **)&RecoveryHob);
        if(EFI_ERROR(Status))
            return;
    }
    
    RecoveryHob->ImageSize = Size;
}

VOID
BiosGuardRecoveryFileLoaded (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN RECOVERY_IMAGE_HOB       *RecoveryHob
)
{
    UINT8                   AmiBiosGuardSign[] = "_AMIPFAT";
    UINT8                   *RecoveryRomAddress;
    UINT32                  AmiBiosGuardHeaderLength = 0, CurrentBGUPAddress = 0;
    BGUP_HEADER             *BgupHdr;
    EFI_STATUS              Status;
    EFI_PHYSICAL_ADDRESS    Buffer = 0, SavedBiosGuardImage = 0;
    EFI_BOOT_MODE           BootMode;
    RECOVERY_IMAGE_HOB      *BiosGuardRecoveryImageHob;
    EFI_GUID                gBiosGuardRecoveryImageHobGuid = AMI_BIOSGUARD_RECOVERY_IMAGE_HOB_GUID;

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : BiosGuardRecoveryFileLoaded Start\n"));
#endif

    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    if ( (EFI_ERROR(Status)) || !(BootMode == BOOT_ON_FLASH_UPDATE || BootMode == BOOT_IN_RECOVERY_MODE) ) return;

    //
    // To avoid out of memory resource with AllocatePages in PEI phase, find out the top of available memory for collecting ROM.
    //
    Buffer = FindAvailableMemory(PeiServices);
#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : FindAvailableMemory =%x\n", Buffer));
#endif

    RecoveryRomAddress = (UINT8*)RecoveryHob->Address;
#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : RecoveryRomAddress =%x\n", RecoveryRomAddress));
#endif

    //
    // Checking the recovery ROM whether is AMI BIOS GUARD ROM or Not.
    //
    if (MemCmp (RecoveryRomAddress + 8, AmiBiosGuardSign, 8)) {
        DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : This is Not BIOS Guard ROM.\n"));
        ASSERT_EFI_ERROR (EFI_UNSUPPORTED);
        return;
    }

    AmiBiosGuardHeaderLength = *(UINT32*)RecoveryRomAddress;
    BgupHdr = (BGUP_HEADER*)(RecoveryRomAddress + AmiBiosGuardHeaderLength);
    do {
        if (BgupHdr->ScriptSectionSize == 0xFFFFFFFF) break;

        //
        // Skip ScriptSection and BGUP Header
        //
        AmiBiosGuardHeaderLength += (BgupHdr->ScriptSectionSize + sizeof(BGUP_HEADER));

        //
        // Collect DataSection only to buffer.
        //
        (*PeiServices)->CopyMem ((UINT8*)Buffer + CurrentBGUPAddress, (UINT8*)RecoveryRomAddress + AmiBiosGuardHeaderLength, BgupHdr->DataSectionSize);

        //
        // Pointer to next BGUP block.
        //
        AmiBiosGuardHeaderLength += (BgupHdr->DataSectionSize + BIOSGUARD_PACKAGE_CERT);
        CurrentBGUPAddress += BgupHdr->DataSectionSize;
        BgupHdr = (BGUP_HEADER*)(RecoveryRomAddress + AmiBiosGuardHeaderLength);
    } while ( AmiBiosGuardHeaderLength < BIOSGUARD_RECOVERY_IMAGE_SIZE );

    //
    // Allocate memory for saving BiosGuard image.
    //
    Status = (*PeiServices)->AllocatePages ( \
             PeiServices, \
             EfiBootServicesData, \
             (BIOSGUARD_RECOVERY_IMAGE_SIZE >> 12) + 1, \
             &SavedBiosGuardImage);
    if (!EFI_ERROR(Status)) {
#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : Saved BiosGuard Image %lx\n", SavedBiosGuardImage));
#endif

        //
        // Save BiosGuard image to HOB.
        //
        (*PeiServices)->CopyMem ((UINT8*)SavedBiosGuardImage, (UINT8*)RecoveryRomAddress, BIOSGUARD_RECOVERY_IMAGE_SIZE);
        Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof(RECOVERY_IMAGE_HOB), &BiosGuardRecoveryImageHob);
        if ( !EFI_ERROR(Status) ) {
            //
            // Create BiosGuard recovery Image HOB for DXE used.
            //
            BiosGuardRecoveryImageHob->Header.Name = gBiosGuardRecoveryImageHobGuid;
            BiosGuardRecoveryImageHob->Address = SavedBiosGuardImage;
            BiosGuardRecoveryImageHob->FailedStage = 0;
        }
    }

    //
    // Skip ME Region if needed (CurrentBGUPAddress = Original image size).
    //
    (*PeiServices)->CopyMem ((UINT8*)RecoveryHob->Address, (UINT8*)Buffer + (CurrentBGUPAddress - FLASH_SIZE), FLASH_SIZE );

    ReportBiosGuardRecoverySize (PeiServices, FLASH_SIZE);
    return;
}

/**
    LoadRecoveryCapsuleHook function invoke the Original LoadRecoveryCapsule 
    procedure, and check if the loaded ROM is BiosGuard ROM then reconstruct 
    the BIOS image.
        
    @param PeiServices - pointer to PeiServices Structure 
    @param This - pointer to the PPI structure
    @param CapsuleInstance - value indicating the instance of the PPI
    @param Buffer - contains information read from the block device

         
    @retval EFI_SUCCESS-  File read from recovery media
    @retval EFI_INVALID_PARAMETER - Buffer is a NULL pointer
    @retval EFI_NOT_FOUND - asking for a 1 or greater instance of the PPI
    @retval Other return error values from LocatePpi or FsRecoveryRead
**/
EFI_STATUS
LoadRecoveryCapsuleHook (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI *This,
    IN UINTN                              CapsuleInstance,
    OUT VOID                              *Buffer
)
{
    UINT8                                 NumberOfImageProviders = 0;
    RECOVERY_IMAGE_HOB                    RecoveryHob;
    EFI_STATUS                            Status;

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "\nBiosGuardRecovery.c : LoadRecoveryCapsuleHook Start\n"));
#endif

    for (NumberOfImageProviders = 0; gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi != NULL; NumberOfImageProviders++) {
        //
        // Checking "This" for identfiy the LoadRecoveryCapsule call.
        //
        if (This != gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi) continue;

        //
        // Invoke the Original LoadRecoveryCapsule procedure
        //
        Status = gDeviceRecoveryModuleInfo[NumberOfImageProviders].SavedLoadCapsule ( PeiServices, This, CapsuleInstance, Buffer );
        if (EFI_ERROR(Status)) {
            DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : Invoke the Original LoadRecoveryCapsule procedure is fail.\n"));
            return Status;
        }

        //
        // Recovery image loaded, check if BiosGuard ROM, reconstruct the BIOS image.
        //
        RecoveryHob.Address = (EFI_PHYSICAL_ADDRESS)Buffer;

        DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : Ready to do BIOS Guard recovery.\n"));
        BiosGuardRecoveryFileLoaded(PeiServices, &RecoveryHob);
        break;
    }
    return EFI_SUCCESS;
}

/**
    GetRecoveryCapsuleInfoHook function invoke the Original GetRecoveryCapsuleInfo 
    procedure, and override the recovery/capsule size to BiosGuard recovery/capsule 
    image size.

        
    @param PeiServices - pointer to PeiServices Structure
    @param This - pointer to the PPI structure
    @param CapsuleInstance - value indicating the instance of the PPI
    @param Size - Size of the recovery capsule
    @param CapsuleType OPTIONAL - Type of recovery capsule

         
    @retval EFI_SUCCESS - Parameters are valid and output parameters are updated
    @retval EFI_INVALID_PARAMETER - Size pointer is NULL
    @retval EFI_NOT_FOUND - asking for a 1 or greater instance of the PPI
**/
EFI_STATUS 
EFIAPI 
GetRecoveryCapsuleInfoHook (
    IN EFI_PEI_SERVICES                      **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
    IN UINTN                                 CapsuleInstance,
    OUT UINTN                                *Size,
    OUT EFI_GUID                             *CapsuleType 
)
{
    EFI_STATUS                            Status;
    UINT8                                 NumberOfImageProviders = 0;
    
    for (NumberOfImageProviders = 0; gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi != NULL; NumberOfImageProviders++) {
        //
        // Checking "This" for identfiy the LoadRecoveryCapsule call.
        //
        if (This != gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi) continue;
        
        //
        // Invoke the Original GetRecoveryCapsuleInfo procedure
        //
        Status = gDeviceRecoveryModuleInfo[NumberOfImageProviders].GetRecoveryCapsuleInfo ( PeiServices, This, CapsuleInstance, Size, CapsuleType );
        if (EFI_ERROR(Status)) {
            DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : Invoke the Original GetRecoveryCapsuleInfo procedure is fail.\n"));
            return Status;
        }
        
        *Size = BIOSGUARD_RECOVERY_IMAGE_SIZE;
        DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : Modify recovery size to 0x%x\n", *Size));
        break;
    }
    return EFI_SUCCESS;
}

EFI_STATUS
BiosGuardRecoveryEntry (
    IN EFI_FFS_FILE_HEADER                *FfsHeader,
    IN EFI_PEI_SERVICES                   **PeiServices
)
{
    UINTN                                 NumberOfImageProviders = 0;
    EFI_STATUS                            Status;
    EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *pRecoveryDevice;
    EFI_PEI_PPI_DESCRIPTOR                *pDummy;

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : BiosGuardRecoveryEntry Start\n"));
#endif

    if ( !((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
            (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) ) {
        //
        // BIOS Guard is disabled or not supported
        //
        DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : BIOS Guard is disabled or not supported\n"));
        return EFI_SUCCESS;
    }

    //
    // Initialize gDeviceRecoveryModuleInfo structure
    //
    for(NumberOfImageProviders = 0; NumberOfImageProviders < MAX_DEVICE_RECOVERY_MODULE; NumberOfImageProviders++) {
        gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi = NULL;
        gDeviceRecoveryModuleInfo[NumberOfImageProviders].SavedLoadCapsule = NULL;
        gDeviceRecoveryModuleInfo[NumberOfImageProviders].GetRecoveryCapsuleInfo = NULL;
    }

    NumberOfImageProviders = 0;
    do {
        //
        // Discover the Device Recovery Module PPIs for hooking the LoadRecoveryCapule procedure.
        //
        Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiDeviceRecoveryModulePpiGuid, NumberOfImageProviders, &pDummy, &pRecoveryDevice);
        if (!EFI_ERROR(Status)) {
            //
            // Save Ppi pointer and LoadRecoveryCapsule procedure for Hook used.
            //
            gDeviceRecoveryModuleInfo[NumberOfImageProviders].pDRM_Ppi = pRecoveryDevice;
            gDeviceRecoveryModuleInfo[NumberOfImageProviders].SavedLoadCapsule = pRecoveryDevice->LoadRecoveryCapsule;
            gDeviceRecoveryModuleInfo[NumberOfImageProviders++].GetRecoveryCapsuleInfo = pRecoveryDevice->GetRecoveryCapsuleInfo;

            //
            // Hook the LaodRecoveryCapsule procedure.
            //
            pRecoveryDevice->LoadRecoveryCapsule = LoadRecoveryCapsuleHook;
            pRecoveryDevice->GetRecoveryCapsuleInfo = GetRecoveryCapsuleInfoHook;
        }
    } while( !EFI_ERROR(Status) );

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecovery.c : BiosGuardRecoveryEntry End\n"));
#endif

    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
