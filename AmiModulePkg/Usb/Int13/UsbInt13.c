//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file UsbInt13.c
    USB Int13 driver

**/

#include <Token.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "Protocol/AmiUsbController.h"
#include <Protocol/ComponentName.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/LegacyBios.h>
#include "UsbInt13.h"
#include <Pci.h>
#include "../Rt/UsbDef.h"
#include <Protocol/UsbPolicy.h>
#include <AmiDxeLib.h>

EFI_STATUS InitInt13RuntimeImage();

EFI_LEGACY_BIOS_EXT_PROTOCOL        *gBiosExtensions = NULL;
LEGACY16_TO_EFI_DATA_TABLE_STRUC    *gLegacy16Data = 0;
BOOLEAN                             gCdromInstalled = FALSE;
UINT13_DATA                         *gI13BinData = NULL;
EFI_USB_PROTOCOL                    *gAmiUsb = NULL;
UINT8                               gBootOverrideDeviceIndx = 0;
USB_GLOBAL_DATA                     *gUsbData = NULL;


UINT8   gHotplugFddName[] = "USB Hotplug FDD";
USB_MASS_DEV gHotplugFloppy;

UINT8   gHotplugHddName[] = "USB Hotplug HDD";
USB_MASS_DEV gHotplugHardDrive;

UINT8   gHotplugCdromName[] = "USB Hotplug CDROM";
USB_MASS_DEV gHotplugCDROM;

USB_PCI_LOCATION* gUsbPciLocationTable = NULL;
            
/**
    USB INT13 driver entry point. Installs callback notification
    on gEfiUSBProtocolGuid installation.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
UsbInt13EntryPoint(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_EVENT   Event;
    EFI_STATUS  Status;

    InitAmiLib(ImageHandle, SystemTable);
    Status = gBS->LocateProtocol(&gEfiUsbProtocolGuid, NULL, &gAmiUsb);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    gUsbData = gAmiUsb->USBDataPtr;

    Status = InitInt13RuntimeImage();
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    gAmiUsb->InstallUsbLegacyBootDevices = InstallUsbLegacyBootDevices;
    gAmiUsb->UsbInstallLegacyDevice = UsbInstallLegacyDevice;
    gAmiUsb->UsbUninstallLegacyDevice = UsbUninstallLegacyDevice;

    Status = EfiCreateEventReadyToBootEx(TPL_CALLBACK, ReadyToBootNotify, NULL, &Event);
    ASSERT_EFI_ERROR(Status);

    return Status;
}


/**
    Initialization of data structures and placement of runtime
    code of USB INT13

**/

EFI_STATUS
InitInt13RuntimeImage()
{
    EFI_STATUS  Status;
    VOID        *Image;
    UINTN       ImageSize = 0;

    //
    // Check the version of CSM16, support is available for ver 7.55 or later
    //
    {
        UINT8 MjCsmVer = *(UINT8*)0xF0018;
        UINT8 MnCsmVer = *(UINT8*)0xF0019;

        if (MjCsmVer<8 && MnCsmVer<0x55) return EFI_UNSUPPORTED;
    }

    gLegacy16Data = (LEGACY16_TO_EFI_DATA_TABLE_STRUC*)(UINTN)(0xF0000 + *(UINT16*)0xFFF4C);

    //
    // Get the USB INT13 runtime image
    //
    Status = gBS->LocateProtocol(
        &gEfiLegacyBiosExtProtocolGuid, NULL, &gBiosExtensions);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBiosExtensions->GetEmbeddedRom(
        CSM16_MODULEID, CSM16_VENDORID, CSM16_USB_RT_DID, &Image, &ImageSize);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Do the necessary RT data initialization here using Image before it is shadowed
    //..............................
    {
#pragma pack(push, 1)
        // Update USB SMI information
        typedef struct _USB_SMM_RTS {
            UINT8   MiscInfo;
            UINT16  SmmAttr;
            UINT32  SmmPort;
            UINT32  SmmData;
        } USB_SMM_RTS;
    
        static USB_SMM_RTS UsbSmmRt = {1, 0, SW_SMI_IO_ADDRESS, USB_SWSMI};
    
        *(USB_SMM_RTS*)((UINTN)Image + ((UINT13_DATA*)Image)->UsbSmmDataOffset) = UsbSmmRt;

#pragma pack(pop)
    }

    // Copy image to shadow E000/F000 area
    gI13BinData = (UINT13_DATA *)gBiosExtensions->CopyLegacyTable(Image, (UINT16)ImageSize, 0x10, 2);
    if (gI13BinData == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    gUsbPciLocationTable =
        (USB_PCI_LOCATION*)((UINTN)gI13BinData + ((UINT13_DATA*)gI13BinData)->UsbPciLocationTableOffset);

    return EFI_SUCCESS;
}



BOOLEAN
HotplugEnabled (
    HOTPLUG_DEVICE DeviceType
)
{
    if (DeviceType == Floppy)
    {
        return ((gUsbData->fdd_hotplug_support == SETUP_DATA_HOTPLUG_ENABLED) ||
            (gUsbData->fdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO));
    }

    if (DeviceType == HardDrive)
    {
        return ((gUsbData->hdd_hotplug_support == SETUP_DATA_HOTPLUG_ENABLED) ||
            (gUsbData->hdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO));
    }

    if (DeviceType == CDROM)
    {
        return ((gUsbData->cdrom_hotplug_support == SETUP_DATA_HOTPLUG_ENABLED) ||
            (gUsbData->cdrom_hotplug_support == SETUP_DATA_HOTPLUG_AUTO));
    }

    return FALSE;
}



EFI_STATUS
InitializeHotplugDevices()
{
    gHotplugFloppy.DevInfo = &gUsbData->FddHotplugDev;
    gHotplugFloppy.LogicalAddress = USB_HOTPLUG_FDD_ADDRESS;
    gHotplugFloppy.Handle = NULL;
    gHotplugFloppy.PciBDF = 0xffff;
    gHotplugFloppy.DevString = gHotplugFddName;
    gHotplugFloppy.StorageType = USB_MASS_DEV_ARMD;

    gHotplugHardDrive.DevInfo = &gUsbData->HddHotplugDev;
    gHotplugHardDrive.LogicalAddress = USB_HOTPLUG_HDD_ADDRESS;
    gHotplugHardDrive.Handle = NULL;
    gHotplugHardDrive.PciBDF = 0xffff;
    gHotplugHardDrive.DevString = gHotplugHddName;
    gHotplugHardDrive.StorageType = USB_MASS_DEV_HDD;

    gHotplugCDROM.DevInfo = &gUsbData->CdromHotplugDev;
    gHotplugCDROM.LogicalAddress = USB_HOTPLUG_CDROM_ADDRESS;
    gHotplugCDROM.Handle = NULL;
    gHotplugCDROM.PciBDF = 0xffff;
    gHotplugCDROM.DevString = gHotplugCdromName;
    gHotplugCDROM.StorageType = USB_MASS_DEV_CDROM;

    if (HotplugEnabled(Floppy))
        UsbInstallLegacyDevice(&gHotplugFloppy);

    if (HotplugEnabled(HardDrive))
        UsbInstallLegacyDevice(&gHotplugHardDrive);

    if (HotplugEnabled(CDROM))
        UsbInstallLegacyDevice(&gHotplugCDROM);

    return EFI_SUCCESS;
}


/**
    READY_TO_BOOT event notification callback. It locates BBS
    table and changes the priority of device located at index
    gBootOverrideDeviceIndx to 0 (highest). It also verifies
    the hotplug devices are properly installed.

    @param Event - event signaled by the DXE Core upon installation
        Context - event context

    @retval VOID

**/

VOID
EFIAPI
ReadyToBootNotify(
    EFI_EVENT Event, 
    VOID *Context)
{
    UINT16      Priority;
    UINT16      Index, Index1;
    BBS_TABLE   *BbsTable = NULL;
    UINT16      HddCount;
    HDD_INFO    *HddInfo;
    UINT16      BbsCount;
    EFI_STATUS  Status;
    EFI_LEGACY_BIOS_PROTOCOL        *Bios = NULL;

    //
    // Find BBS table pointer
    //
    Status = gBS->LocateProtocol(
        &gEfiLegacyBiosProtocolGuid, NULL, &Bios);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return;
    }

    Status = Bios->GetBbsInfo(Bios, &HddCount, &HddInfo, &BbsCount, &BbsTable);
    ASSERT_EFI_ERROR(Status);

    //
    // Kill the Event.
    //
    gBS->CloseEvent( Event );
    
    //
    // Report BBS_USB type devices as other normal Boot devices
    // like HDD/CDROM/Floppy based on the storagetype by changing
    // the devicetype in the BBS table.
    //
#if BBS_USB_DEVICE_TYPE_SUPPORT
    for(Index=0; Index<BbsCount; Index++) {
        if(BbsTable[Index].DeviceType == BBS_USB) {
            switch( (((BbsTable[Index].InitPerReserved) >> 24) & 0xf) ) {
                case BAID_TYPE_RMD_FDD:
                    BbsTable[Index].DeviceType = BBS_FLOPPY;
                    break;
                case BAID_TYPE_RMD_HDD:
                    BbsTable[Index].DeviceType = BBS_HARDDISK;
                    break;
                case BAID_TYPE_CDROM:
                    BbsTable[Index].DeviceType = BBS_CDROM;
                    break;
                default:
                    BbsTable[Index].DeviceType = BBS_UNKNOWN;
            }
        }
    }
#endif

    if (gBootOverrideDeviceIndx == 0)
    {
        return; // No override needed
    }
    

    //
    // Find a device with the highest priority and swap it with priority of
    // a device located at gBootOverrideDeviceIndex.
    //
    Priority = BbsTable[gBootOverrideDeviceIndx].BootPriority;

    for (Index = 0, Index1 = MAX_BBS_ENTRIES_NO;
        Index < MAX_BBS_ENTRIES_NO;
        Index++)
    {
        if (Index == gBootOverrideDeviceIndx) continue;

        if (BbsTable[Index].BootPriority < Priority)
        {
            Index1 = Index;
        }
    }
    //
    // Index1 has entry with lowest priority, otherwise MAX_BBS_ENTRIES_NO
    //
    if (Index1 < MAX_BBS_ENTRIES_NO)
    {
        BbsTable[gBootOverrideDeviceIndx].BootPriority = BbsTable[Index1].BootPriority;
        BbsTable[Index1].BootPriority = Priority;
    }
}


/**
    This function retrieves USB device name, copies it into
    lower memory and returns a pointer to the string.

**/

EFI_STATUS
CreateDeviceName(
    UINT8   DevIndex,
    UINT8   *DevNameStringSrc,
    UINT16  *StringDestinationSegment,
    UINT16  *StringDestinationOffset,
    UINT16  *MfgStringDestinationSegment,
    UINT16  *MfgStringDestinationOffset
)
{
    UINT8 *DevName = (gI13BinData->UsbMassI13Dev)[DevIndex].DeviceNameString;
    UINT8 i;

    //
    // Copy the string, compact it on the way (no more that one ' ' in a row)
    //
    for (i=0; i < 63 && *DevNameStringSrc != 0; i++, DevNameStringSrc++)
    {
        if ((*DevNameStringSrc == 0x20) && (*(DevNameStringSrc-1) == 0x20)) continue;
        *DevName++ = *DevNameStringSrc;  // DevNameStringSrc incremented unconditionally
    }
    *DevName = 0;   // string terminator

    DevName = (gI13BinData->UsbMassI13Dev)[DevIndex].DeviceNameString;

    *StringDestinationSegment = (UINT16)(((UINTN)DevName & 0xf0000) >> 4);
    *StringDestinationOffset = (UINT16)((UINTN)DevName & 0xffff);

    *MfgStringDestinationSegment = (UINT16)(((UINTN)gI13BinData->MfgGenericName & 0xf0000) >> 4);
    *MfgStringDestinationOffset = (UINT16)((UINTN)gI13BinData->MfgGenericName & 0xffff);

    return EFI_SUCCESS;
}


/**
    This function takes the device index within USBMASS_INT13_DEV
    list and prepares BBS entry for this device.

**/

EFI_STATUS
CreateBbsEntry(
    UINT8           DevIndex,
    IN USB_MASS_DEV *UsbMassDevice,
    OUT BBS_TABLE   *BbsEntry
)
{
    EFI_STATUS  Status;
    UINT8   Handle;
    UINT8   DevAndSysType;
    UINT8   BaidDeviceType = 0;					//(EIP73024)
    BBS_STATUS_FLAGS    StatusFlags = {0};		//(EIP73024)
    UINT16  CheckForUsbCdromOffset;
    UINT32  CheckForUsbCdromAddress;
    UINT8   *PatchAddr;

    ASSERT(DevIndex < USBDEVS_MAX_ENTRIES);

    if (gBiosExtensions == NULL) return EFI_NOT_FOUND;

    gBS->SetMem(BbsEntry, sizeof(BBS_TABLE), 0);

    //
    // Get the HC PCI location
    //
    BbsEntry->Bus = (UINT32)(UsbMassDevice->PciBDF >> 8);
    BbsEntry->Device = (UINT32)((UsbMassDevice->PciBDF & 0xFF) >> 3);
    BbsEntry->Function = (UINT32)(UsbMassDevice->PciBDF & 7);

    //
    // Update class/subclass information
    //
    BbsEntry->Class = PCI_CL_SER_BUS;
    BbsEntry->SubClass = PCI_CL_SER_BUS_SCL_USB;

    StatusFlags.Enabled = 1; StatusFlags.MediaPresent = 1;
    BbsEntry->StatusFlags = StatusFlags;  // Enabled, Unknown media

    //
    // Copy the device name string into low memory at gLegacyMemoryAddress, and
    // update the string pointer in BBS table entry
    //
    Status = CreateDeviceName(
                DevIndex,
                UsbMassDevice->DevString,
                &BbsEntry->DescStringSegment,
                &BbsEntry->DescStringOffset,
                &BbsEntry->MfgStringSegment,
                &BbsEntry->MfgStringOffset
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    DevAndSysType = 0x11;    
    Handle = (UINT8)UsbMassDevice->LogicalAddress;

    switch (UsbMassDevice->StorageType) {
        case USB_MASS_DEV_ARMD:
#if BBS_USB_DEVICE_TYPE_SUPPORT
            BbsEntry->DeviceType = BBS_USB;
#else
            BbsEntry->DeviceType = BBS_FLOPPY;
#endif
            BaidDeviceType = BAID_TYPE_RMD_FDD;
            BbsEntry->BootHandlerSegment = (UINT16)((UINTN)gI13BinData >> 4);
            BbsEntry->BootHandlerOffset = gI13BinData->BcvOffset + DevIndex*4;
            break;

        case USB_MASS_DEV_HDD:
#if BBS_USB_DEVICE_TYPE_SUPPORT
            BbsEntry->DeviceType = BBS_USB;
#else
            BbsEntry->DeviceType = BBS_HARDDISK;
#endif
            BaidDeviceType = BAID_TYPE_RMD_HDD;
            Handle |= 0x80;
            BbsEntry->BootHandlerSegment = (UINT16)((UINTN)gI13BinData >> 4);
            BbsEntry->BootHandlerOffset = gI13BinData->BcvOffset + DevIndex*4;
            break;

        case USB_MASS_DEV_CDROM:
#if BBS_USB_DEVICE_TYPE_SUPPORT
            BbsEntry->DeviceType = BBS_USB;
#else
            BbsEntry->DeviceType = BBS_CDROM;
#endif
            BaidDeviceType = BAID_TYPE_CDROM;            
            BbsEntry->BootHandlerSegment = 0xf000;
            BbsEntry->BootHandlerOffset = gLegacy16Data->CdrBevOffset;
            if (gCdromInstalled) break;
            //
            // Patch farReturnCDROMSupportAPIPointer routine with "call farCheckForUSBCdrom"
            //
            CheckForUsbCdromOffset = gI13BinData->CheckForUsbCDROMOffset;
            CheckForUsbCdromAddress =
                (UINT32)((UINTN)gI13BinData<<12) + (UINT32)CheckForUsbCdromOffset;
        
            PatchAddr = (UINT8*)(UINTN)(0xF0000+gLegacy16Data->CDROMSupportAPIOfs+5);
            *PatchAddr++ = 0x9A; // far call opcode
            *(UINT32*)PatchAddr = CheckForUsbCdromAddress;
            gCdromInstalled = TRUE;
            break;
        default:
            BbsEntry->DeviceType = BBS_UNKNOWN;
    }

    BbsEntry->InitPerReserved = ((UINT32)BaidDeviceType<<24)
                        +((UINT32)Handle<<8)
                        +(UINT32)DevAndSysType;

    *(UINTN*)(&BbsEntry->IBV1) = (UINTN)UsbMassDevice->Handle;

    return EFI_SUCCESS;
}

/**
    This function installs USB INT13 devices

**/

EFI_STATUS
EFIAPI
InstallUsbLegacyBootDevices(
    VOID
)
{
    EFI_STATUS  Status;
    UINTN       NumberOfHandles = 0;
    EFI_HANDLE  *HandleBuffer = NULL;
    UINTN       Index;
    EFI_USB_IO_PROTOCOL     *UsbIo;
    EFI_USB_INTERFACE_DESCRIPTOR InterfaceDesc;
    EFI_BLOCK_IO_PROTOCOL   *BlkIo;
    USB_MASS_DEV            *MassDev;
    DEV_INFO                *DevInfo;

    if (gUsbData->dUSBStateFlag & USB_FLAG_DISABLE_LEGACY_SUPPORT) {
        return EFI_UNSUPPORTED;
    }

	InitializeHotplugDevices();

	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiBlockIoProtocolGuid, 
                NULL, &NumberOfHandles, &HandleBuffer);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    for (Index = 0; Index < NumberOfHandles; Index++) {
        Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiUsbIoProtocolGuid, &UsbIo);
        if (EFI_ERROR(Status)) {
            continue;
        }

        Status = UsbIo->UsbGetInterfaceDescriptor(UsbIo, &InterfaceDesc);
        if (EFI_ERROR(Status)) {
            continue;
        }
        if (InterfaceDesc.InterfaceClass != BASE_CLASS_MASS_STORAGE) {
            continue;
        }

        Status = gBS->HandleProtocol(HandleBuffer[Index], 
                                &gEfiBlockIoProtocolGuid, &BlkIo);
        if (EFI_ERROR(Status)) {
            continue;
        }

        MassDev = (USB_MASS_DEV*)BlkIo;
        DevInfo = (DEV_INFO*)MassDev->DevInfo;
        if ((DevInfo->bPhyDevType != USB_MASS_DEV_UNKNOWN) &&
            !(DevInfo->bPhyDevType != USB_MASS_DEV_CDROM && 
            (DevInfo->wBlockSize > 0x200 && DevInfo->wBlockSize != 0xFFFF))){
            UsbInstallLegacyDevice(MassDev);
        }
    }
       
    if (HandleBuffer != NULL) {
        gBS->FreePool(HandleBuffer);
    }

	return EFI_SUCCESS;
}

/**
    This function installs USB INT13 device

**/

EFI_STATUS
EFIAPI
UsbInstallLegacyDevice(
    USB_MASS_DEV    *UsbMassDevice
)
{
    BBS_TABLE   BbsEntry;
    EFI_STATUS  Status;
    UINT8       EntryNumber = 0xff;
    UINT8       Index;
    DEV_INFO    *Device;
    UINT8       HcIndx;
    UINT8       PortIndx;

    //
    // See if device is already in the list, if yes - return error.
    //
    for (Index=0; Index<USBDEVS_MAX_ENTRIES; Index++) {
        if ((gI13BinData->UsbMassI13Dev)[Index].Handle == (UINT8)UsbMassDevice->LogicalAddress) {
            ASSERT(FALSE);  // ERROR: Device already exists
            return EFI_INVALID_PARAMETER;
        }
    }
    //
    // Look for an empty slot in BcvLookupTable
    //
    for (Index=0; Index<USBDEVS_MAX_ENTRIES; Index++) {
        if ((gI13BinData->UsbMassI13Dev)[Index].Handle == 0) {
            break;
        }
    }
    ASSERT(Index<USBDEVS_MAX_ENTRIES);
    if (Index==USBDEVS_MAX_ENTRIES) return EFI_OUT_OF_RESOURCES;

    Status = gBiosExtensions->UnlockShadow(0, 0, 0, 0);
    ASSERT_EFI_ERROR(Status);

    Status = CreateBbsEntry(Index, UsbMassDevice, &BbsEntry);
    ASSERT_EFI_ERROR(Status);

    Status = gBiosExtensions->InsertBbsEntryAt(gBiosExtensions,
            &BbsEntry,
            &EntryNumber);  // This function returns EntryNumber
    //ASSERT_EFI_ERROR(Status);

    if (EFI_ERROR(Status)) {
        gBiosExtensions->LockShadow(0, 0);
        return Status;
    }
    
    //
    // Entry has been successfully added, update the lookup table
    //
    (gI13BinData->UsbMassI13Dev)[Index].Handle = (UINT8)UsbMassDevice->LogicalAddress;
    (gI13BinData->UsbMassI13Dev)[Index].BbsEntryNo = EntryNumber;
    (gI13BinData->UsbMassI13Dev)[Index].DevBaidType = (UINT8)(BbsEntry.InitPerReserved>>24);

    //
    // Update device geometry related information
    //
    Device = (DEV_INFO*)UsbMassDevice->DevInfo;
    (gI13BinData->UsbMassI13Dev)[Index].NumHeads = Device->NonLBAHeads;
    (gI13BinData->UsbMassI13Dev)[Index].LBANumHeads = Device->Heads;
    (gI13BinData->UsbMassI13Dev)[Index].NumCylinders = Device->wNonLBACylinders;
    (gI13BinData->UsbMassI13Dev)[Index].LBANumCyls = Device->wCylinders;
    (gI13BinData->UsbMassI13Dev)[Index].NumSectors = Device->bNonLBASectors;
    (gI13BinData->UsbMassI13Dev)[Index].LBANumSectors = Device->bSectors;
    (gI13BinData->UsbMassI13Dev)[Index].BytesPerSector = Device->wBlockSize;
    (gI13BinData->UsbMassI13Dev)[Index].MediaType = Device->bMediaType;
    (gI13BinData->UsbMassI13Dev)[Index].LastLBA = Device->MaxLba;
    (gI13BinData->UsbMassI13Dev)[Index].BpbMediaDesc = Device->BpbMediaDesc;

    // Update PCI location of the controller this device is connected to
    gUsbPciLocationTable[Index].Handle = (UINT8)UsbMassDevice->LogicalAddress;
    gUsbPciLocationTable[Index].PciLocation = UsbMassDevice->PciBDF;

    Status = gBiosExtensions->LockShadow(0, 0);
    ASSERT_EFI_ERROR(Status);

	// Set the device as registered
	Device->Flag |= DEV_INFO_MASS_DEV_REGD;

    //
    // See if OEM asks for USB boot override for this device. If yes, store
    // BBS index of this device, later at READY_TO_BOOT this BBS device will
    // be assigned priority 0 (highest).
    //
    // Note1: There is no need to check if gAmiUsb is valid, because this
    // function itself is one of the members of gAmiUsb structure.
    //
    // Note2: This feature will only be available for the devices connected
    // directly to the root port; devices behind the hub(s) will be ignored.
    //
    if (Device->bHubDeviceNumber & 0x80) {
        Status = gAmiUsb->UsbGetAssignBootPort(&HcIndx, &PortIndx);
        if ((!EFI_ERROR(Status)) && (gBootOverrideDeviceIndx == 0)) {
            DEBUG((DEBUG_INFO,"OemUsbGetAssignBootPort: HC %d, Port %d; current HC %d, Port %d\n",
                HcIndx, PortIndx, Device->bHCNumber, Device->bHubPortNumber));

            if ((Device->bHCNumber == HcIndx) && (Device->bHubPortNumber == PortIndx)) {
                DEBUG((DEBUG_INFO,"---OemUsbGetAssignBootPort: BBS Entry# %d\n", EntryNumber));

                gBootOverrideDeviceIndx = EntryNumber;
            }
        }
    }

    //
    // Process the "Auto" settings of Hotplug devices: if the device being installed
    // have already had a "Hotplug" clone, uninstall the clone.
    //

    // Process hotplug floppy
    if ((UsbMassDevice != &gHotplugFloppy) &&
         (UsbMassDevice->StorageType == USB_MASS_DEV_ARMD) &&
         (gUsbData->fdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO)) {
        DEBUG((DEBUG_INFO, "Uninstalling Hotplug Floppy (Setup 'Auto' option)\n"));
        UsbUninstallLegacyDevice(&gHotplugFloppy);    // Okay not to be successful
    }

    // Process hotplug HDD
    if ((UsbMassDevice != &gHotplugHardDrive) &&
         (UsbMassDevice->StorageType == USB_MASS_DEV_HDD) &&
         (gUsbData->hdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO)) {
        DEBUG((DEBUG_INFO, "Uninstalling Hotplug HDD (Setup 'Auto' option)\n"));
        UsbUninstallLegacyDevice(&gHotplugHardDrive);    // Okay not to be successful
    }
    // Process hotplug CDROM
    if ((UsbMassDevice != &gHotplugCDROM) &&
         (UsbMassDevice->StorageType == USB_MASS_DEV_CDROM) &&
         (gUsbData->cdrom_hotplug_support == SETUP_DATA_HOTPLUG_AUTO)) {
        DEBUG((DEBUG_INFO, "Uninstalling Hotplug CDROM (Setup 'Auto' option)\n"));
        UsbUninstallLegacyDevice(&gHotplugCDROM);    // Okay not to be successful
    }

    return EFI_SUCCESS;
}


/**
    This function uninstalls USB INT13 device

**/

EFI_STATUS
EFIAPI
UsbUninstallLegacyDevice(
    USB_MASS_DEV*   UsbMassDevice
)
{
    EFI_STATUS  Status;
    UINT8       Index;

    DEBUG((DEBUG_INFO, "Uninstalling INT13 device %x\n", UsbMassDevice));

    Status = gBiosExtensions->UnlockShadow(0, 0, 0, 0);
    ASSERT_EFI_ERROR(Status);

    for (Index=0; Index<USBDEVS_MAX_ENTRIES; Index++) {
        if ((gI13BinData->UsbMassI13Dev)[Index].Handle == (UINT8)UsbMassDevice->LogicalAddress) {
            (gI13BinData->UsbMassI13Dev)[Index].Handle = 0; // Mark as unused
            Status = gBiosExtensions->RemoveBbsEntryAt(
                        gBiosExtensions,
                        (gI13BinData->UsbMassI13Dev)[Index].BbsEntryNo
            );
            //ASSERT_EFI_ERROR(Status);
            if (EFI_ERROR(Status)) {
                break;
            }

            if ((gBootOverrideDeviceIndx != 0)
                && (gBootOverrideDeviceIndx == (gI13BinData->UsbMassI13Dev)[Index].BbsEntryNo))
            {
                gBootOverrideDeviceIndx = 0;
            }
            gUsbPciLocationTable[Index].Handle = 0; // Make invalid handle

            break;
        }
    }
    //ASSERT_EFI_ERROR(Status);

    gBiosExtensions->LockShadow(0, 0);

    if (EFI_ERROR(Status)) {
        return Status;
    }

	((DEV_INFO*)UsbMassDevice->DevInfo)->Flag &= ~(DEV_INFO_VALID_STRUC | DEV_INFO_MASS_DEV_REGD);

    //
    // Process the "Auto" settings of Hotplug devices: if the device being uninstalled
    // is the last one of a kind, and "Auto" Setup option is selected for the hotplug
    // device of this kind, then install the hotplug device.
    //

    // Process hotplug floppy
    if ((UsbMassDevice != &gHotplugFloppy) &&
         (UsbMassDevice->StorageType == USB_MASS_DEV_ARMD) &&
         (gUsbData->fdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO) &&
         (gUsbData->NumberOfFDDs == 0)) {
        DEBUG((DEBUG_INFO, "Installing Hotplug Floppy (Setup 'Auto' option)\n"));
        UsbInstallLegacyDevice(&gHotplugFloppy);
    }

    if ((UsbMassDevice != &gHotplugHardDrive) &&
         (UsbMassDevice->StorageType == USB_MASS_DEV_HDD) &&
         (gUsbData->hdd_hotplug_support == SETUP_DATA_HOTPLUG_AUTO) &&
         (gUsbData->NumberOfHDDs == 0)) {
        DEBUG((DEBUG_INFO, "Installing Hotplug HDD (Setup 'Auto' option)\n"));
        UsbInstallLegacyDevice(&gHotplugHardDrive);
    }
    if ((UsbMassDevice != &gHotplugCDROM) &&
         (UsbMassDevice->StorageType == USB_MASS_DEV_CDROM) &&
         (gUsbData->cdrom_hotplug_support == SETUP_DATA_HOTPLUG_AUTO) &&
         (gUsbData->NumberOfCDROMs == 0)) {
        DEBUG((DEBUG_INFO, "Installing Hotplug CDROM (Setup 'Auto' option)\n"));
        UsbInstallLegacyDevice(&gHotplugCDROM);
    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
