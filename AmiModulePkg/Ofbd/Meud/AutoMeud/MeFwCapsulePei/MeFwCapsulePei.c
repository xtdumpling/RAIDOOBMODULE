//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file MeFwCapsulePei.c
    ME FW Capsule Update Pei driver.
**/
//**********************************************************************
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <Pei.h>
#include <AmiPeiLib.h>
#include <FlashUpd.h>
#include <Token.h>
#include <Ppi/MasterBootMode.h>
//----------------------------------------------------------------------------
// Function Externs
extern EFI_GUID gAmiPeiBeforeMrcGuid;

//----------------------------------------------------------------------------
// Local prototypes
EFI_STATUS
EFIAPI
MeFwBootOnFlashUpdateNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
);
EFI_STATUS
EFIAPI
MeFwBeforeMrcNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
);

static 
EFI_PEI_NOTIFY_DESCRIPTOR 
MeFwBootOnFlashUpdateNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gFlashUpdBootModePpiGuid, MeFwBootOnFlashUpdateNotify },
};

static 
EFI_PEI_NOTIFY_DESCRIPTOR 
MeFwBeforeMrcNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
      &gAmiPeiBeforeMrcGuid, MeFwBeforeMrcNotify }
};

//----------------------------------------------------------------------------
// Local Variables
#define R_PCH_ACPI_PM1_STS                        0x00
#define B_PCH_ACPI_PM1_STS_WAK                    0x8000
#define R_PCH_ACPI_PM1_CNT                        0x04
#define B_PCH_ACPI_PM1_CNT_SLP_TYP                0x00001C00
#define R_PCH_RST_CNT                             0xCF9
#define V_PCH_RST_CNT_SOFTSTARTSTATE              0x00
#define V_PCH_RST_CNT_HARDSTARTSTATE              0x02
#define V_PCH_RST_CNT_SOFTRESET                   0x04
#define V_PCH_RST_CNT_HARDRESET                   0x06

//----------------------------------------------------------------------------
// Function Definitions
#if defined(ME_DFU_USE_RAW_FILE) && (ME_DFU_USE_RAW_FILE == 1)
EFI_GUID gDfuMeFirmwareIdGuid = DFU_ME_FIRMWARE_ID_GUID;

EFI_STATUS
EFIAPI
MeFwMemoryReadyNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
);

// PPI to be installed
static 
EFI_PEI_NOTIFY_DESCRIPTOR 
MeFwMemoryReadyNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiMemoryDiscoveredPpiGuid, MeFwMemoryReadyNotify },
};

/**
 * This hook is the Notify function of EfiPeiMemoryDiscoveredPpiGuid.
 * @param  **PeiServices Describes the list of possible PEI Services.
 * @param  NotifyDescriptor Descriptor used to store data about notifying this functino
 * @param  NullPpi Address of the PPI installed with the GUID
 * @retval EFI_SUCESS The Notify function executes successfully.
**/
EFI_STATUS
EFIAPI
MeFwMemoryReadyNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
)
{
    VOID *p;
    for( (*PeiServices)->GetHobList( PeiServices, &p ); \
            !(FindNextHobByType( EFI_HOB_TYPE_UEFI_CAPSULE, &p )); )
    {
        EFI_CAPSULE_HEADER *FwCapHdr = \
                    (EFI_CAPSULE_HEADER*)((EFI_HOB_UEFI_CAPSULE*)p)->BaseAddress;
        if( !guidcmp( &FwCapHdr->CapsuleGuid, &gDfuMeFirmwareIdGuid ) )
        {
            // if DfuMeFirmwareGuid found, invoke MeFwBootOnFlashUpdateNotify function
            // to perform system reset for Heci reset.
            MeFwBootOnFlashUpdateNotify( PeiServices, NULL, NULL );
            break;
        }    
    }
    return EFI_SUCCESS;
}
#endif // #if defined ME_DFU_USE_RAW_FILE && ME_DFU_USE_RAW_FILE == 1

/**
 * This hook is the Notify function of FlashUpdBootModePpiGuid.
 * @param  **PeiServices Describes the list of possible PEI Services.
 * @param  NotifyDescriptor Descriptor used to store data about notifying this functino
 * @param  NullPpi Address of the PPI installed with the GUID
 * @retval EFI_SUCESS The Notify function executes successfully.
**/
EFI_STATUS
EFIAPI
MeFwBootOnFlashUpdateNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
)
{
    EFI_CAPSULE_HEADER  *FwCapHdr = (EFI_CAPSULE_HEADER*)NULL;
    VOID                *p;

    for( (*PeiServices)->GetHobList( PeiServices, &p ); \
            !(FindNextHobByType( EFI_HOB_TYPE_UEFI_CAPSULE, &p )); )
    {
        FwCapHdr = (EFI_CAPSULE_HEADER*)((EFI_HOB_UEFI_CAPSULE*)p)->BaseAddress;
#if defined(ME_DFU_USE_RAW_FILE) && (ME_DFU_USE_RAW_FILE == 1)
        if( guidcmp( &FwCapHdr->CapsuleGuid, &gDfuMeFirmwareIdGuid ) )
#endif 
        {
            UINT8   *p = (UINT8*)FwCapHdr;
            UINT32  i;
            // Here looks for ME Capsule Firmware Volume GUID.
            for( i = FwCapHdr->HeaderSize; i < FwCapHdr->CapsuleImageSize; i += 16 )
            {
                EFI_GUID mMeFwCapsuleFvGuid = {0x9F8B1DEF, 0xB62B, 0x45F3, \
                                0x82, 0x82, 0xBF, 0xD7, 0xEA, 0x19, 0x80, 0x1B};
                if( ((EFI_FIRMWARE_VOLUME_HEADER*)(p + \
                                i))->Signature != EFI_FVH_SIGNATURE ) continue;
                if( !guidcmp( (UINT8*)p + i + ((EFI_FIRMWARE_VOLUME_HEADER*)\
                        (p + i))->ExtHeaderOffset, &mMeFwCapsuleFvGuid ) ) break;
                i += (UINT32)(((EFI_FIRMWARE_VOLUME_HEADER*)(p + i))->FvLength - 16);
            } 
            if( i >= FwCapHdr->CapsuleImageSize ) continue;
        }
        // Clear Wake Status (WAK_STS) and Sleep Type (SLP_TYP)
        IoWrite16( PM_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_WAK );
        IoWrite16( PM_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, \
          (IoRead16( PM_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT ) & ~B_PCH_ACPI_PM1_CNT_SLP_TYP) );
        // Since, HECI interface not work if S3 resume path, to generate a Soft Reset
        // to re-activate HECI.
        IoWrite8( R_PCH_RST_CNT, V_PCH_RST_CNT_HARDSTARTSTATE );
        IoWrite8( R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET );
        EFI_DEADLOOP()
    }
    return EFI_SUCCESS;
}
/**
 * This hook is the Notify function of AmiPeiBeforeMrcGuid.
 * @param  **PeiServices Describes the list of possible PEI Services.
 * @param  NotifyDescriptor Descriptor used to store data about notifying this functino
 * @param  NullPpi Address of the PPI installed with the GUID
 * @retval EFI_SUCESS The Notify function executes successfully.
**/
EFI_STATUS
EFIAPI
MeFwBeforeMrcNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
)
{
    EFI_STATUS      Status;
    EFI_BOOT_MODE   BootMode;
 
    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
    if( EFI_ERROR(Status) || (BootMode != BOOT_ON_S3_RESUME) ) return EFI_SUCCESS;
    (*PeiServices)->NotifyPpi( PeiServices, MeFwBootOnFlashUpdateNotifyList );       
#if defined (ME_DFU_USE_RAW_FILE) && (ME_DFU_USE_RAW_FILE == 1)
    // Bios don't enter BOOT_ON_FLASH_UPDATE state if using RAW ME Firmware file,
    // Here creates a MemoryDiscovered Notify Event for Heci reset instead.
    (*PeiServices)->NotifyPpi( PeiServices, MeFwMemoryReadyNotifyList );       
#endif
    return EFI_SUCCESS;
}
/**
 * This hook is the Module PEI driver entry.
 * @param ImageHandle The handle associated with this image being loaded into memory
 * @param SystemTable Pointer to the system table
**/
EFI_STATUS
EFIAPI
MeFwCapsulePeiEntry(
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
)
{
    (*PeiServices)->NotifyPpi( PeiServices, MeFwBeforeMrcNotifyList );
    return EFI_SUCCESS;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
