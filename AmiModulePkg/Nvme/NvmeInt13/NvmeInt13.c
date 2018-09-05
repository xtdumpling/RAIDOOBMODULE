//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug fixed:  Support Legacy NVMe and controlled by item.
//    Reason:   
//    Auditor:  Jimmy Chiu (Refer Grantley - Kasber Chen)
//    Date:     Jan/12/2017
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeInt13.c
    Nvme Driver for Legacy Mode. It installs the Int13 support for the 
    Nvme devices 

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include <IndustryStandard/Pci.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/ComponentName.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include "NvmeInt13.h"
#include <Setup.h> //SMCPKG_SUPPORT

//---------------------------------------------------------------------------

EFI_LEGACY_BIOS_EXT_PROTOCOL        *gBiosExtensions = NULL;
NVME_INT13_DATA                     *gNvmeInt13BinData = NULL;
AMI_NVME_LEGACY_PROTOCOL            gNvmeLegacyProtocol;

/**
    NVMe INT13 driver entry point. Installs call back notification
    on gAmiNvmeLegacyProtocolGuid installation.

    @param  ImageHandle,
    @param  *SystemTable

    @retval FI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeInt13EntryPoint (
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS  Status;
    EFI_HANDLE  NvmeInt13Handle=NULL;
    
#if SMCPKG_SUPPORT
#if NVMEINT13_SUPPORT
    EFI_GUID	SetupGuid = SETUP_GUID;
    UINTN	VarSize = sizeof(SETUP_DATA);
    SETUP_DATA	SetupData;
        
    Status = SystemTable->RuntimeServices->GetVariable(
    		    L"Setup",
    		    &SetupGuid,
    		    NULL,
    		    &VarSize,
    		    &SetupData);
        
    if(!Status && !SetupData.NvmeFWSource)	return Status;
#endif
#endif

    Status = InitInt13RuntimeImage();
    if (EFI_ERROR(Status)) { 
        return Status;
    }

    gNvmeLegacyProtocol.AddNvmeLegacyDevice = NvmeInstallLegacyDevice;

    Status = gBS->InstallProtocolInterface (
                                    &NvmeInt13Handle,
                                    &gAmiNvmeLegacyProtocolGuid,
                                    EFI_NATIVE_INTERFACE,
                                    &gNvmeLegacyProtocol
                                    );

    return Status;
}


/**
    Initialization of data structures and placement of runtime
    code of NVMe INT13

    @param  ImageHandle,
    @param  *SystemTable

    @retval EFI_STATUS

**/

EFI_STATUS
InitInt13RuntimeImage()
{
    EFI_STATUS  Status;
    VOID        *Image;
    UINTN       ImageSize = 0;


    //
    // Get the NVMe INT13 runtime image
    //
    Status = gBS->LocateProtocol(
        &gEfiLegacyBiosExtProtocolGuid, NULL, &gBiosExtensions);
    if (EFI_ERROR(Status)) return Status;

    Status = gBiosExtensions->GetEmbeddedRom(
        CSM16_MODULEID, CSM16_VENDORID, CSM16_NVME_RT_DID, &Image, &ImageSize);
    if (EFI_ERROR(Status)) return Status;

    //
    // Do the necessary RT data initialization here using Image before it is shadowed
    //..............................
    {
#pragma pack(push, 1)
        // Update NVMe SMI information
        typedef struct _NVME_SMM_RTS {
            UINT8   MiscInfo;
            UINT16  SmmAttr;
            UINT32  SmmPort;
            UINT32  SmmData;
        } NVME_SMM_RTS;

        static NVME_SMM_RTS NvmeSmmRt = {1, 0, SW_SMI_IO_ADDRESS, NVME_SWSMI};

        *(NVME_SMM_RTS*)((UINTN)Image + ((NVME_INT13_DATA*)Image)->NvmeSmmDataOffset) = NvmeSmmRt;
#pragma pack(pop)
    }

    // Copy image to shadow E000/F000 area
    gNvmeInt13BinData = (NVME_INT13_DATA *)gBiosExtensions->CopyLegacyTable(Image, (UINT16)ImageSize, 0x10, 2);

    DEBUG((EFI_D_VERBOSE, "gNvmeInt13BinData : %lX\n", gNvmeInt13BinData));
    
    return EFI_SUCCESS;
}

/**
    This function retrieves NVMe device name, copies it into
    lower memory and returns a pointer to the string.

    @param  DevIndex,
    @param  *DevNameStringSrc,
    @param  *StringDestinationSegment,
    @param  *StringDestinationOffset,
    @param  *MfgStringDestinationSegment,
    @param  *MfgStringDestinationOffset

    @retval EFI_STATUS

**/

EFI_STATUS
CreateDeviceName (
    UINT8   DevIndex,
    UINT8   *DevNameStringSrc,
    UINT16  *StringDestinationSegment,
    UINT16  *StringDestinationOffset,
    UINT16  *MfgStringDestinationSegment,
    UINT16  *MfgStringDestinationOffset
)
{
    UINT8 *DevName = (gNvmeInt13BinData->NvmeMassI13Dev)[DevIndex].DeviceNameString;
    UINT8 i;

    // Remove spaces at the beginning of the Nvme Device Namestring
    while( *DevNameStringSrc == 0x20 ) {
        DevNameStringSrc++;
    }

    // Copy the string, compact it on the way (no more that one ' ' in a row)
    for ( i=0; i<31 && *DevNameStringSrc != 0; DevNameStringSrc++ )
    {
        if ((*DevNameStringSrc == 0x20) && (*(DevNameStringSrc-1) == 0x20)) continue;
        *DevName++ = *DevNameStringSrc;  // DevNameStringSrc incremented unconditionally
        i++;
    }

    *DevName = 0;   // string terminator

    DevName = (gNvmeInt13BinData->NvmeMassI13Dev)[DevIndex].DeviceNameString;

    *StringDestinationSegment = (UINT16)(((UINTN)DevName & 0xf0000) >> 4);
    *StringDestinationOffset = (UINT16)((UINTN)DevName & 0xffff);

    *MfgStringDestinationSegment = (UINT16)(((UINTN)gNvmeInt13BinData->MfgGenericName & 0xf0000) >> 4);
    *MfgStringDestinationOffset = (UINT16)((UINTN)gNvmeInt13BinData->MfgGenericName & 0xffff);

    return EFI_SUCCESS;
}

/**
     This function takes the device index within NVMEMASS_INT13_DEV
     list and prepares BBS entry for this device.

        
    @param  DevIndex,
    @param  *NvmeLegacyMassDevice,
    @param  *BbsEntry

    @retval EFI_STATUS

**/
EFI_STATUS
CreateBbsEntry (
    UINT8                       DevIndex,
    IN NVME_LEGACY_MASS_DEVICE  *NvmeLegacyMassDevice,
    OUT BBS_TABLE               *BbsEntry
)
{
    EFI_STATUS  Status;
    UINT8       Handle;
    UINT8       DevAndSysType;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    UINT8       BaidDeviceType=0;
    BBS_STATUS_FLAGS    StatusFlags={0};
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.

    ASSERT(DevIndex < NVMEDEVS_MAX_ENTRIES);

    if (gBiosExtensions == NULL) { 
        return EFI_NOT_FOUND;
    }

    gBS->SetMem(BbsEntry, sizeof(BBS_TABLE), 0);

    //
    // Get the HC PCI location
    //
    BbsEntry->Bus = (UINT32)(NvmeLegacyMassDevice->PciBDF >> 8);
    BbsEntry->Device = (UINT32)((NvmeLegacyMassDevice->PciBDF & 0xFF) >> 3);
    BbsEntry->Function = (UINT32)(NvmeLegacyMassDevice->PciBDF & 7);

    //
    // Update class/subclass information
    //
    BbsEntry->Class = PCI_CLASS_MASS_STORAGE;
    BbsEntry->SubClass = PCI_CLASS_MASS_STORAGE_SOLID_STATE;

    StatusFlags.Enabled = 1; StatusFlags.MediaPresent = 1;
    BbsEntry->StatusFlags = StatusFlags;  // Enabled, Unknown media

    //
    // Copy the device name string into low memory at gLegacyMemoryAddress, and
    // update the string pointer in BBS table entry
    //
    Status = CreateDeviceName(
                DevIndex,
                NvmeLegacyMassDevice->DevString,
                &BbsEntry->DescStringSegment,
                &BbsEntry->DescStringOffset,
                &BbsEntry->MfgStringSegment,
                &BbsEntry->MfgStringOffset
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    DevAndSysType = (SYSTYPE_ATA << 4)+DEVTYPE_SYS;
    Handle = (UINT8)NvmeLegacyMassDevice->LogicalAddress;

    switch (NvmeLegacyMassDevice->StorageType) {

        case NVME_MASS_DEV_HDD:
            BbsEntry->DeviceType = BBS_HARDDISK;
            BaidDeviceType = BAID_TYPE_HDD;
            Handle |= 0x80;
            BbsEntry->BootHandlerSegment = (UINT16)((UINTN)gNvmeInt13BinData >> 4);
            BbsEntry->BootHandlerOffset = gNvmeInt13BinData->BcvOffset + DevIndex*4;
            break;

        default:
            BbsEntry->DeviceType = BBS_UNKNOWN;
    }

    BbsEntry->InitPerReserved = ((UINT32)BaidDeviceType<<24)
                        +((UINT32)Handle<<8)
                        +(UINT32)DevAndSysType;

    *(UINTN*)(&BbsEntry->IBV1) = (UINTN)NvmeLegacyMassDevice->Handle;

    return EFI_SUCCESS;
}


/**
     This API is called by NVMe bus driver. The device is added into CSM16 
     data area for legacy boot

    @param  *NvmeLegacyMassDevice

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeInstallLegacyDevice (
    NVME_LEGACY_MASS_DEVICE *NvmeLegacyMassDevice
)
{
    BBS_TABLE               BbsEntry;
    EFI_STATUS              Status;
    UINT8                   EntryNumber = 0xff;
    UINT8                   Index;
    NVME_MASS_DEV_INFO      *Device;
    NVME_DEV_PCI_LOCATION   *NvmePciDataOffset;

    DEBUG((EFI_D_VERBOSE, "Installing NVMe INT13 device %lx\n", NvmeLegacyMassDevice));

    //
    // See if device is already in the list, if yes - return error.
    //
    for (Index = 0; Index < NVMEDEVS_MAX_ENTRIES; Index++) {
        if ((gNvmeInt13BinData->NvmeMassI13Dev)[Index].Handle == (UINT8)NvmeLegacyMassDevice->LogicalAddress) {
            ASSERT(FALSE);  // ERROR: Device already exists
            return EFI_INVALID_PARAMETER;
        }
    }
    //
    // Look for an empty slot in BcvLookupTable
    //
    for (Index=0; Index<NVMEDEVS_MAX_ENTRIES; Index++) {
        if  ((gNvmeInt13BinData->NvmeMassI13Dev)[Index].Handle == 0) { 
            break;
        }
    }
    
    ASSERT(Index<NVMEDEVS_MAX_ENTRIES);

    if (Index==NVMEDEVS_MAX_ENTRIES) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = gBiosExtensions->UnlockShadow(0, 0, 0, 0);
    ASSERT_EFI_ERROR(Status);

    Status = CreateBbsEntry(Index, NvmeLegacyMassDevice, &BbsEntry);
    ASSERT_EFI_ERROR(Status);

    Status = gBiosExtensions->InsertBbsEntryAt(gBiosExtensions,
                                                &BbsEntry,
                                                &EntryNumber);
    ASSERT_EFI_ERROR(Status);

    //
    // Entry has been successfully added, update the lookup table
    //
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].Handle = (UINT8)NvmeLegacyMassDevice->LogicalAddress;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].BbsEntryNo = EntryNumber;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].DevBaidType = (UINT8)(BbsEntry.InitPerReserved>>24);

    //
    // Update device geometry related information
    //
    Device = (NVME_MASS_DEV_INFO*)NvmeLegacyMassDevice->DevInfo;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].NumHeads = Device->bNonLBAHeads;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].LBANumHeads = Device->bHeads;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].NumCylinders = Device->wNonLBACylinders;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].LBANumCyls = Device->wCylinders;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].NumSectors = Device->bNonLBASectors;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].LBANumSectors = Device->bSectors;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].BytesPerSector = Device->wBlockSize;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].LastLBA = Device->dMaxLba;
    (gNvmeInt13BinData->NvmeMassI13Dev)[Index].BpbMediaDesc = 0;

    // Update PCI Bus#
    NvmePciDataOffset = (NVME_DEV_PCI_LOCATION *)((UINTN)gNvmeInt13BinData + gNvmeInt13BinData->NvmePciDataOffset);
    NvmePciDataOffset[Index].Handle = (UINT8)NvmeLegacyMassDevice->LogicalAddress;
    NvmePciDataOffset[Index].PciBDF = NvmeLegacyMassDevice->PciBDF;
    
    Status = gBiosExtensions->LockShadow(0, 0);
    ASSERT_EFI_ERROR(Status);

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
