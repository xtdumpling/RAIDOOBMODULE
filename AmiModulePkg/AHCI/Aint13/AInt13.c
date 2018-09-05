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

/** @file AInt13.c
    AHCI Int13 structures initialization routines

**/
//---------------------------------------------------------------------------

#include "AInt13.h"
#include "Protocol/LegacyBiosExt.h"
#include "Protocol/LegacyAhci.h"
#include "Protocol/BlockIo.h"
#include "Protocol/DevicePath.h"
#include "AhciBus.h"

//---------------------------------------------------------------------------

#if INDEX_DATA_PORT_ACCESS
extern      InitilizeIndexDataPortAddress();
#endif

EFI_STATUS  InitDrivesInformation (VOID*, UINT16);
EFI_STATUS EFIAPI  InitAhciInt13Support();

UINT32      EbdaStartOffset;
VOID        *gLegacyMemoryAddress;
VOID        *gImage;
UINT8       *gHddReadData;
BOOLEAN     gAint13InstallationExecuted = FALSE;
UINT8       gController = 0;

AMI_AHCI_INT13_INIT_PROTOCOL        gAint13;
LEGACY16_TO_EFI_DATA_TABLE_STRUC    *gLegacy16Data = 0;
EFI_PCI_IO_PROTOCOL                 *gPciIo = NULL;
CONTROLLER_INFO_STRUC               ControllerInfo[AHCI_CONTROLLER_COUNT];
EFI_LEGACY_BIOS_EXT_PROTOCOL        *gBiosExtensions = NULL;

#define     MmAddress( BaseAddr, Register ) \
            ((UINT64)(BaseAddr) + \
            (UINTN)(Register) \
             )
#define     Mm16Ptr( BaseAddr, Register ) \
            ((volatile UINT16 *)MmAddress (BaseAddr, Register ))
/**
    Installs gAhciInt13Init protocol

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/

EFI_STATUS EFIAPI Ai13EntryPoint(
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS      Status;

    InitAmiLib(ImageHandle, SystemTable);

    gAint13.InitAhciInt13Support = InitAhciInt13Support;

    Status = pBS->InstallProtocolInterface(&ImageHandle,
                                           &gAint13ProtocolGuid,
                                           EFI_NATIVE_INTERFACE,
                                           (VOID **)&gAint13);
    return Status;
}

/**
    This routine returns the number of connected AHCI drives as well
    as it fills the optional SATA_DEVICE_INTERFACE* buffer with the
    device information.

**/

UINT16
CountDrives(
    IN EFI_HANDLE    *HandleBuffer,
    IN UINTN         HandleCount,
    IN VOID          *Devices
)
{
    EFI_STATUS               Status;
    DLINK                    *dlink;
    SATA_DEVICE_INTERFACE    *SataDevInterface = NULL;
    UINT16  Drives = 0;
    UINTN   n;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface;

    //
    // Go through all controllers, count the number of valid connected drives
    for (n = 0; n < HandleCount; n++) {
        Status = pBS->HandleProtocol(HandleBuffer[n], &gAmiAhciBusProtocolGuid,  (VOID **)&AhciBusInterface);
        if (EFI_ERROR(Status)) return Drives;

        for (dlink = AhciBusInterface->SataDeviceList.pHead; dlink; dlink = dlink->pNext) {
            SataDevInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);
            if (SataDevInterface->DeviceState == DEVICE_CONFIGURED_SUCCESSFULLY && 
                    (SataDevInterface->DeviceType == ATA || SataDevInterface->DeviceType == ATAPI)) {
                if (Devices != NULL) {
                    *((UINTN*)Devices) = (UINTN)SataDevInterface;
                  Devices = (UINTN*)Devices + 1;
                }
                Drives++;
            }
        }
    }

    return Drives;
}

/**
    This function will be called when Legacy boot Event Signaled
    This function will set the EBDA address for PxCLB and PxFB

    @param    Event signaled event
    @param    Context calling context

    @retval   VOID

**/

VOID
EFIAPI 
SetEbdaAddressForPort (
    IN EFI_EVENT    Event, 
    IN VOID         *Context
)
{
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface=(AMI_AHCI_BUS_PROTOCOL*)Context;
    UINT32                   PortsImplemented;
    UINT8                    i, PortNumber;
    UINT32                   CurrentEbda;
    UINT64                   PortCommandListBaseAddr=0;
    UINT64                   PortFISBaseAddr=0;

#if INDEX_DATA_PORT_ACCESS
    InitilizeIndexDataPortAddress (AhciBusInterface->PciIO);
#endif
    
    PortsImplemented = AhciBusInterface->HBAPortImplemented;
    PortNumber = 0;

    // Find the EBDA Address from the EbdaStartOffset 
    if (EbdaStartOffset) {
        CurrentEbda = (UINT32)(UINTN)(*(UINT16*)0x40e << 4);
        EbdaStartOffset += CurrentEbda;
        EbdaStartOffset += (1024-1);
        PortCommandListBaseAddr = EbdaStartOffset & (~(1024-1));
        PortFISBaseAddr=PortCommandListBaseAddr + 1024;
    }

    for (i=0; PortsImplemented; PortsImplemented>>=1, PortNumber++){
        if (PortsImplemented & 1) { 

            // Clear Start
            HBA_PORT_REG32_AND (AhciBusInterface, PortNumber, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));

            // Program PxCLB and PxFB
            HBA_PORT_WRITE_REG64 (AhciBusInterface, 
                                  PortNumber, 
                                  HBA_PORTS_CLB,
                                  PortCommandListBaseAddr);

            HBA_PORT_WRITE_REG64 (AhciBusInterface, 
                                  PortNumber, HBA_PORTS_FB, 
                                  PortFISBaseAddr);
            i++;
        }
    }
    pBS->CloseEvent(Event);

    return;
}

/**
    LegacyAhci driver API function. It initializes drive information
    and places it in BBS table as well as in AHCI INT13 runtime area.

**/

EFI_STATUS
EFIAPI 
InitAhciInt13Support()
{
    EFI_HANDLE              *HandleBuffer = 0;
    UINTN                   HandleCount = 0;
    UINT16                  DevCount = 0;
    VOID                    *Devices;
    EFI_STATUS              Status;
    EFI_EVENT               LegacyEvent;
    AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface;
    SATA_DEVICE_INTERFACE   *Device;

    // First, find out if we have been called already. If yes - do not do anything
    // since INT13 installation is not intended to be called multiple times.
    if (gAint13InstallationExecuted) {
        return EFI_SUCCESS;
    }

    Status = pBS->LocateHandleBuffer(ByProtocol,
                                     &gAmiAhciBusProtocolGuid,
                                     NULL,
                                     &HandleCount,
                                     &HandleBuffer);
    if (EFI_ERROR (Status)) {
        return EFI_SUCCESS;
    }

    DevCount = CountDrives(HandleBuffer, HandleCount, NULL);
    // For the 1st time CountDrives was called to get the number of connected
    // drives; for the 2nd time it will actually return the device information
    if (DevCount == 0) {
        return EFI_SUCCESS;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(VOID*)*DevCount, &Devices);
    ASSERT_EFI_ERROR(Status);

    CountDrives(HandleBuffer, HandleCount, Devices);

    Status = InitDrivesInformation(Devices, DevCount);
    ASSERT_EFI_ERROR(Status);

    // Program the EBDA Address on Legacy boot Event.
    Device = *((SATA_DEVICE_INTERFACE**)Devices);
    AhciBusInterface=Device->AhciBusInterface;
    CreateLegacyBootEvent(TPL_CALLBACK, &SetEbdaAddressForPort, (VOID*)AhciBusInterface, &LegacyEvent);

    pBS->FreePool(Devices);

    gAint13InstallationExecuted = TRUE;

    return EFI_SUCCESS;
}

/**
    Returns the total number of sectors from IDENTIFY_DATA

    @param IdendifyData - pointer to the IDENTIFY_DATA array

    @retval Total0 lower DWORD of the total number of blocks
    @retval Total1 higher WORD of the total number of blocks

**/

VOID
GetNumberOfSectors (
    IN UINT16   *IdentifyData,
    OUT UINT32  *Total0,
    OUT UINT16  *Total1
)
{
    UINT32  d0 = 0;
    UINT16  d1 = 0;

    // Get total #of sectors
    // Word-61, Word-60 = Total #of user addressable sectors
    // Word-103, Word-102, Word-101, Word-100 = Max user LBA for 48bit LBA
    d0 = *(UINT32*)((UINTN)IdentifyData+60*2);   // get Words 60-61

    // If 48-bit LBA enabled then get total number of sectors
    // from words 100..102
    if (IdentifyData[86] & 0x400) {
        d0 = *(UINT32*)((UINTN)IdentifyData+100*2);    // get Words 100-101
        d1 = *(UINT16*)((UINTN)IdentifyData+102*2);    // get Word 102
    }

    *Total0 = d0;
    *Total1 = d1;

}

/**
    Returns the CHS-translated number of cylinders

**/

VOID
CalcTranslatedCyl (
    IN OUT UINT16   *Cyl,
    IN OUT UINT8    *Heads,
    IN OUT UINT8    *Sectors,
    IN UINT32       TotalBlocks
)
{
    UINT8 heads = *Heads;
    UINT8 sec = *Sectors;
    UINT16 cyl;

    for (cyl=TotalBlocks/(UINT16)heads*(UINT16)sec; cyl>1024;) {
        // Current xlat scheme does not make #of cylinders <= 1024, try next scheme
        cyl = 1024;
        if (heads == 0xFF) {
            break;  // Head limit reached
        }
        heads <<= 1;    // Double the heads
        if (heads == 0) heads = 0xFF;
    }
    *Heads = heads;
    *Sectors = sec;
    *Cyl = cyl;
}

/**
    This function returns the translated max #of heads (1-based), 
    number of cylinders and sectors/track.

    @param    Heads   - Maximum head number (1-based)
    @param    Sectors - Sectors/track
    @param    Cyl     - Number of cylinders
    @param    TotalBlocks - Total number of sectors

    @retval    Cyl Translated number of cylinders
    @retval    Heads Translated max head number (1-based)
    @retval    Sectors Translated number of sectors/track

    @note  - If total #of sectors < 1032192 (i.e. FC000 hex),
                translation is not needed.
              - If total #of sectors <= 2064384 (i.e. 1F8000 hex),
                use 63 SPT and 32 HD for translation.
              - If total #of sectors <= 4128768 (i.e. 3F0000 hex),
                use 63 SPT and 64 HD for translation.
              - If total #of sectors <= 8257536 (i.e. 7E0000 hex),
                use 63 SPT and 128 HD for translation, else
                use 63 SPT and 255 HD for translation.
              - In any case, check the parameters for maximum values allowed
                by BIOS and ATA specs (i.e. 1024 cyl, 16 heads, 63 sec/track)
**/

VOID
TranslateHdSec (
    IN OUT UINT16    *Cyl,
    IN OUT UINT8     *Heads,
    IN OUT UINT8     *Sectors,
    IN UINT32        TotalBlocks
)
{
    UINT8 heads = *Heads;
    UINT8 sec = *Sectors;
    UINT16 cyl = *Cyl;

    if (TotalBlocks >= 0xFC000) {
        heads = 32; sec = 63;
        if (TotalBlocks >= 0x1F8000) {
            heads = 64; sec = 63;
            if (TotalBlocks >= 0x3F0000) {
                heads = 128; sec = 63;
                if (TotalBlocks >= 0x7E0000) {
                    heads = 255; sec = 63;
                }
            }
        }
        CalcTranslatedCyl(&cyl, &heads, &sec, TotalBlocks);
    }
    else {
        // hd <= 528MB, no translation is needed normally
        // check maxm value of the parameters
        if (cyl>1024 || heads>16 || sec>63) {
            heads = 32; sec = 63;
            CalcTranslatedCyl(&cyl, &heads, &sec, TotalBlocks);
        }
    }
    *Heads = heads;
    *Sectors = sec;
    *Cyl = cyl;
}

/**
    This function fills the DEV_PARAM information for the drive.

**/

VOID
ConfigureHddParameter (
    IN EFI_BLOCK_IO_PROTOCOL    *BlkIo,
    IN UINT16                   *IdentifyData,
    IN OUT DEV_PARAM_STRUC      *DevParam,
    IN DEV_INFO_STRUC           *DevInfo,
    IN DEV_PTR_STRUC            *DevPtr
)
{
    EFI_STATUS    Status;
    UINT8         PartNo;
    UINT8         *PartPtr;
    UINT8         *NonActivePartPtr = NULL;
    UINT32        OemName;
    BOOLEAN       ValidTable = FALSE;
    BOOLEAN       ValidMBR=FALSE;
    UINT8         heads, sec;
    UINT16        cyl;
    UINT16        SecTimesHeads;
    UINT32        TotalBlocks;

    // Check whether the hard disk is already formatted. If already formatted,
    // find out what mode it was formatted with.
    if(BlkIo == NULL ) {
        return;
    }
    Status = BlkIo->ReadBlocks(BlkIo, BlkIo->Media->MediaId, 0, 0x200, gHddReadData);
    if (!EFI_ERROR(Status) && (*(UINT16*)(gHddReadData+0x1FE) == 0xAA55)) {
        // Drive has a partition table, take parameter from 1st bootable partition
        PartPtr = gHddReadData+0x1BE;

        for (PartNo=0; PartNo<4; PartNo++, PartPtr+=0x10) {
            if ((*PartPtr) == 0x80) {
                ValidTable = TRUE;
                break;
            }
            if (((*(UINT16*)(PartPtr+5)) & 0x3F00) == 0) {
                // not well formed partitioned table, end sector# = 0
                continue;
            }
            if (NonActivePartPtr == NULL) {
                NonActivePartPtr = PartPtr;
            }
        }

        if (!ValidTable && (NonActivePartPtr != NULL)) {
            // No active partition found, use 1st non-active valid partition
            PartPtr = NonActivePartPtr; 
            ValidTable = TRUE;
        }

        if (ValidTable) {
            // Either active or first non-active entry found at PartPtr
            // Read it at gHddReadData + 0xE00
            Status = BlkIo->ReadBlocks(BlkIo, BlkIo->Media->MediaId, 
                        *(UINT32*)(PartPtr+8), 0x200, gHddReadData+0x800);
            if (!EFI_ERROR(Status) && (*(UINT16*)(gHddReadData+0x800+0x1FE) == 0xAA55)) {
                // Read succeeded so the drive is formatted; check for
                // valid MSDOS/MSWIN/NTFS boot record
                OemName = *(UINT32*)(gHddReadData+0x800+3);

                // 'ODSM' for MSDO
                // 'IWSM' for MSWI
                // 'SFTN' for NTFS
                if((OemName == 0x4F44534D) || (OemName == 0x4957534D) || (OemName == 0x5346544E)) {
                    ValidMBR=TRUE;
                } else {
                    // Check for valid FAT,FAT16,FAT32 boot records
                    *(gHddReadData+0x800+0x36+3) = 0x20; // Ignore the 4th byte and fill it with space
                    if ((*(UINT32*)(gHddReadData+0x800+0x36) == 0x20544146)      // " TAF" for FATx
                        || (*(UINT32*)(gHddReadData+0x800+0x52) == 0x33544146)) // "3TAF" for FAT3
                    {
                        ValidMBR = TRUE;
                    }
                }
            }
            if (ValidMBR) {
                heads = *(gHddReadData+0x800+0x1A); // number of heads
                sec = *(gHddReadData+0x800+0x18);   // Sectors/track
                SecTimesHeads = heads * sec;
                if (SecTimesHeads == 0) {
                    ValidMBR = FALSE;
                } else {
                    // Here we have heads/sectors info from partition boot record of known OSes.
                    // NOTE: sectors/track and #of heads is the information with which
                    // the concerned partition was created. This information must be used to
                    // calculate #of cylinders to maintain the current translation scheme.

                    // Find the total #of sectors in the hard disk C * H * S
                    TotalBlocks = DevParam->bMAXHN * DevParam->bMAXSN * DevParam->wMAXCYL;
                    // Calculate #of cylinders using the same translation scheme
                    cyl = TotalBlocks / SecTimesHeads;
                    if (cyl>1024) cyl = 1024;
                }
            }
        }
    }

    if (!ValidMBR) {
        // Hard disk was not formatted OR no valid partition table entry was found;
        // find the translation scheme which will be used for the hard disk.
        cyl = DevParam->wMAXCYL;
        heads = DevParam->bMAXHN;
        sec = DevParam->bMAXSN;
        TotalBlocks = cyl * sec * heads;

        TranslateHdSec(&cyl, &heads, &sec, TotalBlocks);
    }

    DevParam->wLBACYL = DevParam->wMAXCYL;
    DevParam->bLBAHD = DevParam->bMAXHN;
    DevParam->bLBASPT = DevParam->bMAXSN;

    DevParam->wMAXCYL = cyl;
    DevParam->bMAXHN = heads;
    DevParam->bMAXSN = sec;

    if (heads > 8) {
        DevParam->bCBYTE = 8;    // More than 8 heads indication
    }
}


/**
    This function initializes the AHCI HD drive specific information.

**/

VOID
InitAhciHddDev ( 
    IN SATA_DEVICE_INTERFACE    *Dev,
    IN DEV_PARAM_STRUC          *DevParam,
    IN DEV_INFO_STRUC           *DevInfo,
    IN DEV_PTR_STRUC            *DevPtr,
    AHCI_RT_MISC_DATA           *AhciRtMiscData
)
{
    UINT16                 *IdentifyData;
    UINT32                 d0;
    UINT16                 d1;
    UINT8                  DevNo;
    UINT32                 SegOfs;
    UINT16                 heads, spt, cyl;
    EFI_BLOCK_IO_PROTOCOL  *BlkIo;
    UINT32                 Temp;
    UINT8                  Temp2 = 0;

    IdentifyData = (UINT16*)&Dev->IdentifyData;

    heads = IdentifyData[3];
    spt = IdentifyData[6];
    ASSERT((heads<255) && (spt<255));   // Both heads & sectors/track must be 8-bit

    // Save AHCI HD Mode Information
    DevInfo->bPIOInfo = (UINT8)IdentifyData[64];
    DevInfo->bUDMAInfo = (UINT8)IdentifyData[88];

    cyl = IdentifyData[1];

    DevParam->wMAXCYL = cyl;
    DevParam->bMAXHN = (UINT8)heads;
    DevParam->bMAXSN = (UINT8)spt;

    GetNumberOfSectors(IdentifyData, &d0, &d1);
    DevInfo->dTotalSectorsLo = d0;
    DevInfo->dTotalSectorsHi = (UINT32)d1;

    DevNo = AhciRtMiscData->NumAhciDevice++;
    DevInfo->bInt13Num = DevNo | 0x80;
    DevPtr->bInt13Num = DevNo | 0x80;
    DevPtr->bPMnum = Dev->PMPortNumber;
    DevPtr->bPortNum = Dev->PortNumber;

    // Update SEG:OFS address for current DevParam and DevInfo
    SegOfs = ((UINT32)gLegacyMemoryAddress<<12)+(UINT32)((UINTN)DevParam-(UINTN)gImage);
    DevPtr->dParamTablePtr = SegOfs;

    SegOfs = ((UINT32)gLegacyMemoryAddress<<12)+(UINT32)((UINTN)DevInfo-(UINTN)gImage);
    DevPtr->dInfoTablePtr = SegOfs;

    // Indicate device is LBA capable
    // Note: Non-LBA device are not supported in this release
    DevInfo->bSelector |= SELECTOR_LBA;

    // Check whether device is 48-bit LBA capable
    if (IdentifyData[86] & 0x400) {
        DevInfo->bInfoFlag |= INFO_LBA_48;
    }
    // Update sector size if necessary
    if ( (IdentifyData[106] & 0x4000) && (!(IdentifyData[106] & 0x8000)) && (IdentifyData[106] & 0x1000) ) {
        Temp = (UINT32)(IdentifyData[117] + (IdentifyData[118] << 16)) * 2;
        while (Temp > 1) {
            Temp = Temp >> 1;
            Temp2++;
        }
        DevInfo->bSectorSizeN = Temp2;
    }
    // Note: Interrupt driven mechanism is not supported
    DevInfo->bInfoFlag &= ~(INFO_IRQ);

    // Do the LBA to CHS translation; use BlkIo to read partition/boot record
    BlkIo = &Dev->SataBlkIo->BlkIo;
    ConfigureHddParameter(BlkIo, IdentifyData, DevParam, DevInfo, DevPtr);
}

/**
    This function initializes AHCI CD/DVDROM device information.

**/

VOID
InitAhciCd (
    IN SATA_DEVICE_INTERFACE    *Dev,
    IN DEV_PARAM_STRUC          *DevParam,
    IN DEV_INFO_STRUC           *DevInfo,
    IN DEV_PTR_STRUC            *DevPtr,
    AHCI_RT_MISC_DATA           *AhciRtMiscData
)
{
    UINT8   DevNo;
    UINT32  SegOfs;

    DevNo = AhciRtMiscData->NumAhciDevice++;
    DevInfo->bInt13Num = (DevNo + AHCI_CD_CSM_ID_OFFSET) | BIT7 ;
    DevPtr->bInt13Num = (DevNo + AHCI_CD_CSM_ID_OFFSET) | BIT7;
    DevPtr->bPMnum = Dev->PMPortNumber;
    DevPtr->bPortNum = Dev->PortNumber;

    // Update SEG:OFS address for current DevParam and DevInfo
    SegOfs = ((UINT32)gLegacyMemoryAddress<<12)+(UINT32)((UINTN)DevParam-(UINTN)gImage);
    DevPtr->dParamTablePtr = SegOfs;

    SegOfs = ((UINT32)gLegacyMemoryAddress<<12)+(UINT32)((UINTN)DevInfo-(UINTN)gImage);
    DevPtr->dInfoTablePtr = SegOfs;
}

/**
    This function will check if the BusDevFunc is existing in 
    the array, if it exist then it returns EFI_SUCCESS and if not then
    it will add BusDevFunc into the array and returns EFI_NOT_FOUND.  

    @param Dev      - Structure pointing to SATA_DEVICE_INTERFACE
        DevInfo - Structure that maintain device information
        wBusDevFunc  - It contain the BusDevFunc number for a device

    @retval EFI_NOT_FOUND If wBusDevFunc is not present in the array.
        i.e a new controller is found
    @retval EFI_SUCCESS If wBusDevFunc is present in the array

**/
VOID 
UpdateControllerInfoToLegacy (
    IN SATA_DEVICE_INTERFACE    *Dev,
    IN DEV_INFO_STRUC           *DevInfo,
    IN UINT16                   wBusDevFunc
    )
{
    UINT8       i;
    EFI_STATUS  Status;
    UINT32      Addr16=0;
    UINT16      *TempAddress;

    for(i=0;i<gController;i++) {
        if(ControllerInfo[i].BusDevFun == wBusDevFunc) {
            // Updating the respective controller number.
            DevInfo->bControllerNo=ControllerInfo[i].ControllerNo;
            return;
        }
    }
    
    if(!(gController < AHCI_CONTROLLER_COUNT)){
        DevInfo->bControllerNo= AHCI_CONTROLLER_COUNT;
        return;
    }

    // A new controller is found so BusDevFunc and ControllerNo is added
    // into the array of structure.
    ControllerInfo[gController].BusDevFun = wBusDevFunc;
    ControllerInfo[gController].ControllerNo = gController;
    DevInfo->bControllerNo= gController++;

    // Store the current controller's PciIO in global variable
    gPciIo = Dev->AhciBusInterface->PciIO;
    Status = gBiosExtensions->Get16BitFuncAddress(
            CSM16_CSP_AHCI_ACCESSHBA,
            &Addr16
            );

    //At 'Addr16+2' there is a pointer that point to the structure 
    //that maintains index, data and base address for a controller
    TempAddress=(UINT16*)(Addr16+2);
    Addr16=Addr16 + (*TempAddress);

    // No Addr16 points to ReadWriteRegisterDword routine implemented in 
    // AHCIACC.ASM in CSP module part
    if (!EFI_ERROR(Status)) {
        Status = InitCspData((UINT16)((Addr16>>4) & 0xF000),
                             (UINT16)Addr16,
                             (UINT32)(Dev->AhciBusInterface->AhciBaseAddress),
                             DevInfo->bControllerNo);
        ASSERT_EFI_ERROR(Status);
    }

    return;    
}

/**
    This function initializes AHCI device specific information.

**/

VOID
InitAhciDev (
    IN SATA_DEVICE_INTERFACE    *Dev,
    IN DEV_PARAM_STRUC          *DevParam,
    IN DEV_INFO_STRUC           *DevInfo,
    IN DEV_PTR_STRUC            *DevPtr,
    AHCI_RT_MISC_DATA           *AhciRtMiscData
)
{
    EFI_PCI_IO_PROTOCOL    *PciIo;
    UINTN                  seg, bus, dev, func;
    UINT8                  bIrq;
    UINT32                 dHbaBase;
    UINT8                  bPort;
    EFI_STATUS             Status;

    PciIo = Dev->AhciBusInterface->PciIO;

    bPort = Dev->PortNumber;
    DevInfo->bPortNum = bPort;
    DevInfo->bPMNum = Dev->PMPortNumber;

    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x3C, 1, &bIrq);
    ASSERT_EFI_ERROR(Status);
    DevInfo->bIrq = bIrq;
    
    Status = PciIo->GetLocation(PciIo, &seg, &bus, &dev, &func);
    ASSERT_EFI_ERROR(Status);
    DevInfo->wBusDevFunc = ((UINT16)bus<<8) + ((UINT16)dev<<3) + (UINT16)func;

    // Pass the Controller Information to Legacy.
    UpdateControllerInfoToLegacy(Dev,DevInfo,DevInfo->wBusDevFunc);

    DevInfo->dHbaCap = Dev->AhciBusInterface->HBACapability;
    DevInfo->bSelector = SELECTOR_NON_LBA;
    DevInfo->bSectorSizeN = SECTOR_LENGTH_N;    // N value of default sector size 2^N;
    dHbaBase = (UINT32)Dev->AhciBusInterface->AhciBaseAddress;
    DevInfo->dHbaBase = dHbaBase;
    DevInfo->dPortBase = dHbaBase + PORT_REGISTER_START + ((UINT32)bPort<<PORT_REGISTER_SET_SIZE_N);

    if (Dev->DeviceType == ATAPI) {
        DevInfo->bInfoFlag |= INFO_ATAPI;
        InitAhciCd(Dev, DevParam, DevInfo, DevPtr, AhciRtMiscData);
    } else {
        InitAhciHddDev(Dev, DevParam, DevInfo, DevPtr, AhciRtMiscData);
    }
}

/**
    This function generates the readable string with the name of
    device and the SATA port number this device is connected to.

**/

VOID
CreateAhciDriveString (
    IN OUT UINT8    *DescString,
    IN UINT16       *IdentifyData,
    IN UINT8        DevicePortNum,
    IN UINT8        DevicePortMulNum
)
{
    UINT8    s[MAX_DESCRIPTION_STRLEN] = "xP :";
    UINT8    i, data8;
    UINT8    DescStrLen;
    s[0] = DevicePortNum+0x30;

    // Get the drive name out of IdentifyDrive data word 27..46 (up to 40 chars)
    pBS->CopyMem(&s[4], IdentifyData+27, MAX_DESCRIPTION_STRLEN-4);
    // Swap the bytes
    for (i=0; i<MAX_DESCRIPTION_STRLEN; i+=2) {
        data8=s[i];
        s[i]=s[i+1];
        s[i+1]=data8;
    }
    
    s[MAX_DESCRIPTION_STRLEN-1] = 0;    // terminate with zero
    
    // Since the last one will be null character, start parsing from the before one
    DescStrLen= MAX_DESCRIPTION_STRLEN -2 ; 

    //Removing trial space characters
    while(DescStrLen>0 && ( s[DescStrLen]==0x20 )) 
        DescStrLen--;

    s[DescStrLen+1]=0; 

    pBS->CopyMem(DescString, s, DescStrLen+1);

}

/**
    This function creates BBS entry with the device data filled
    according to the actual device information.

**/

EFI_STATUS
CreateBbsEntry (
    IN SATA_DEVICE_INTERFACE    *Dev,
    IN OUT DEV_BBS_OUTFIT       *DevOutfit,
    IN OUT BBS_TABLE            *BbsEntry,
    IN UINT8                    Int13Handle,
    IN UINT16                   BcvOffset
)
{
    EFI_STATUS              Status;
    EFI_PCI_IO_PROTOCOL    *PciIo;
    UINTN                   seg, bus, dev, func;
    UINT8                   dData[4];
    UINT8                   bDevAndSysType;
    UINT8                   bHandle;
    UINT8                   bMasterSlave = 0;
    UINT8                   bBaidDeviceType;

    pBS->SetMem(BbsEntry, sizeof(BBS_TABLE), 0);

    // Get PCI location
    PciIo = Dev->AhciBusInterface->PciIO;
    Status = PciIo->GetLocation(PciIo, &seg, &bus, &dev, &func);
    ASSERT_EFI_ERROR(Status);
    BbsEntry->Bus = (UINT32)bus;
    BbsEntry->Device = (UINT32)dev;
    BbsEntry->Function = (UINT32)func;

    // Get class/subclass information
    Status = PciIo->Pci.Read(
        PciIo,
        EfiPciIoWidthUint32,
        8,  // offset
        1,  // width
        &dData);
    ASSERT_EFI_ERROR(Status);

    BbsEntry->Class = dData[3];
    BbsEntry->SubClass = dData[2];

    CreateAhciDriveString(DevOutfit->DescString,
                          (UINT16*)&Dev->IdentifyData,
                          Dev->PortNumber,
                          Dev->PMPortNumber);

    BbsEntry->DescStringSegment = (UINT16)((UINTN)gLegacyMemoryAddress >> 4);
    BbsEntry->DescStringOffset = (UINT16)((UINTN)DevOutfit->DescString-(UINTN)gImage);

    bDevAndSysType = (SYSTYPE_ATA << 4)+DEVTYPE_SYS;
    bHandle = Int13Handle;

    if (Dev->DeviceType == ATAPI) {
        bBaidDeviceType = BAID_TYPE_CDROM;
        BbsEntry->DeviceType = BBS_CDROM;
        BbsEntry->BootHandlerSegment = 0xF000;
        BbsEntry->BootHandlerOffset = gLegacy16Data->CdrBevOffset;

    } else {
        BbsEntry->DeviceType = BBS_HARDDISK;
        bBaidDeviceType = BAID_TYPE_HDD;
        BbsEntry->BootHandlerSegment = (UINT16)((UINTN)gLegacyMemoryAddress >> 4);
        BbsEntry->BootHandlerOffset = BcvOffset + (Int13Handle & 0x7F)*4;
    }

    BbsEntry->InitPerReserved = ((UINT32)bBaidDeviceType<<24)
                        +((UINT32)bMasterSlave<<16)
                        +((UINT32)bHandle<<8)
                        +(UINT32)bDevAndSysType;

    *(UINTN*)(&BbsEntry->IBV1) = (UINTN)Dev->IdeDeviceHandle;

    return EFI_SUCCESS;
}

/**
    Initialization of data structures and placement of runtime
    code of AHCI INT13

    @param Devices - pointer to the beginning of the instances of
        SATA_DEVICE_INTERFACE*
        DeviceCount - number of valid AHCI devices pointed by
        Devices parameter

**/

EFI_STATUS
InitDrivesInformation (
    IN VOID     *Devices,
    IN UINT16   DeviceCount
)
{
    EFI_LEGACY_BIOS_PROTOCOL        *Bios = NULL;
    UINTN                           ImageSize = 0;
    EFI_STATUS                      Status;
    AHCI_I13_RTDATA                 *Ai13Data;
    UINTN                           Count, i;
    UINT32                          EbdaAddress;
    BBS_TABLE                       BbsEntry;
    UINT32                          LockUnlockAddr, LockUnlockSize;
    UINT16                          CheckForAhciCdromOffset;
    UINT32                          CheckForAhciCdromAddress;
    UINT8                           *addr;
    SATA_DEVICE_INTERFACE           *Device;
    UINT8                           PciConfig[16];
    EFI_TPL                         OldTpl;
#if AVOID_MULTIPLE_BIG_REAL_MODE_SWITCH    
    UINT32                          Address32;
    UINT16                          *TempAddress;
    UINT16                          AhciApiStartOffs;
    UINT32                          AhciAccBufStartOffs;
    UINT32                          AhciAccEbdaAddress;
#endif    
    
    if (DeviceCount == 0) return EFI_SUCCESS;   // No devices connected

    Status = pBS->LocateProtocol(
        &gEfiLegacyBiosProtocolGuid, NULL,  (VOID **)&Bios);
    if (EFI_ERROR(Status)) return Status;

    // Get the AHCI INT13 runtime image
    Status = pBS->LocateProtocol(
        &gEfiLegacyBiosExtProtocolGuid, NULL,  (VOID **)&gBiosExtensions);
    if (EFI_ERROR(Status)) return Status;

    Status = gBiosExtensions->GetEmbeddedRom(
        CSM16_MODULEID, CSM16_VENDORID, CSM16_AHCI_RT_DID, &gImage, &ImageSize);
    if (EFI_ERROR(Status)) return Status;

    // Instantiate data area
    Ai13Data = (AHCI_I13_RTDATA*)gImage;

    // Fill the data area with the init values
    pBS->SetMem(Ai13Data, sizeof (AHCI_I13_RTDATA), 0);
    pBS->SetMem(Ai13Data->DevPtr, sizeof(DEV_PTR_STRUC)*SATA_PORT_COUNT, 0xFF);

    // Allocate legacy region in E000 segment; store SEG:OFS of the allocated
    // memory in global variables
    Status = Bios->GetLegacyRegion(Bios, ImageSize, 2, 0x10, &gLegacyMemoryAddress);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    // Allocate EBDA area for Command List/Command Table/FIS
    Status = gBiosExtensions->AllocateEbda(
                gBiosExtensions,
                SIZE_CLCTFIS_AREA_K+1,  // 1K extra for stack
                &EbdaAddress,
                &EbdaStartOffset );
    ASSERT_EFI_ERROR(Status);
    
#if AHCI_INT13_SMM_SUPPORT
    Ai13Data->AhciRtMiscData.AhciSmmRt.MiscInfo = 1;
    Ai13Data->AhciRtMiscData.AhciSmmRt.SmmAttr = 0;
    Ai13Data->AhciRtMiscData.AhciSmmRt.SmmPort = SW_SMI_IO_ADDRESS;
    Ai13Data->AhciRtMiscData.AhciSmmRt.SmmData = AHCI_INT13_SMM_SWSMI_VALUE;
    Ai13Data->AhciRtMiscData.RunAttribute |= SMM_MODE_CHECK;
#if ACCESS_MMIO_THROUGH_SWSMI
    Ai13Data->AhciRtMiscData.RunAttribute |=  MMIO_THRU_SWSMI;
#endif
#endif
    // Avoid Multiple big real mode change.
#if AVOID_MULTIPLE_BIG_REAL_MODE_SWITCH
    Ai13Data->AhciRtMiscData.RunAttribute |= MMIO_THRU_SINGLE_BIGREAL_MODE;
    Status = gBiosExtensions->AllocateEbda(
                    gBiosExtensions,
                    1,  // 1K extra for data buffer(Byte 0 - Real Mode changing scratch)
                    &AhciAccEbdaAddress,
                    &AhciAccBufStartOffs );
    ASSERT_EFI_ERROR(Status);
        
    Status = gBiosExtensions->Get16BitFuncAddress(
                    CSM16_CSP_AHCI_ACCESSHBA,
                    &Address32
                    );
    if (EFI_ERROR(Status)) 
    return Status;
    TempAddress=(UINT16*)(Address32+2);
    Ai13Data->AhciRtMiscData.PortBaseAddress = (Address32 + *TempAddress);// Storing the address AHCI_ACCESS structure
    
    AhciApiStartOffs=*(UINT16*)(Address32+4);    // "dw AhciApiModuleStart"
    
    TempAddress=(UINT16*)(Address32+6);          // point of "Switch_Big_Real_Mode_FAR - AhciApiModuleStart" in AhciAcc.csm16
    Ai13Data->AhciRtMiscData.PreInt13Hook.FunctionOffs = AhciApiStartOffs + *TempAddress; 
    Ai13Data->AhciRtMiscData.PreInt13Hook.FunctionSeg = (UINT16)((Address32 - AhciApiStartOffs) >> 4);
    
    TempAddress=(UINT16*)(Address32+8);         // point of "Switch_Original_Mode_FAR - AhciApiModuleStart" in AhciAcc.csm16
    Ai13Data->AhciRtMiscData.PostInt13Hook.FunctionOffs = AhciApiStartOffs + *TempAddress; 
    Ai13Data->AhciRtMiscData.PostInt13Hook.FunctionSeg = (UINT16)((Address32 - AhciApiStartOffs) >> 4);
    
    gBiosExtensions->UnlockShadow((UINT8*)((Address32 >> 4) & 0xF000), 0, &LockUnlockAddr, &LockUnlockSize);
    *(UINT16*)(Address32+10) = (UINT16)AhciAccBufStartOffs;
    gBiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize); 
    
#endif

    Ai13Data->AhciRtMiscData.AhciEbdaStart = EbdaStartOffset;
    Ai13Data->AhciRtMiscData.RunAttribute |= A_EBDA_USED;

    gBiosExtensions->UnlockShadow(
        (UINT8*)gLegacyMemoryAddress, ImageSize,
        &LockUnlockAddr, &LockUnlockSize);

    gLegacy16Data = (LEGACY16_TO_EFI_DATA_TABLE_STRUC*)(UINTN)(0xF0000 + *(UINT16*)0xFFF4C);

    Status = pBS->AllocatePool(EfiBootServicesData, 0x1000, &gHddReadData);
    ASSERT_EFI_ERROR(Status);
    

    // Here is a tricky part: we will have to convert the data passed by Ahci Bus
    // driver to the data used by INT13
     for (Count=0, i=0; i<DeviceCount; i++, Devices = ((UINTN*)Devices + 1)) {
        Device = *((SATA_DEVICE_INTERFACE**)Devices);
        
        // If the ATA device Block size is more than 512 bytes 
        // Don't add the device as INT13 device ( Legacy supported device).
        if(Device->DeviceType == ATA && Device->SataBlkIo->BlkIo.Media->BlockSize != 512 ) {
            continue;
        }
                
        InitAhciDev(Device,
                    &Ai13Data->DevParam[Count],
                    &Ai13Data->DevInfo[Count],
                    &Ai13Data->DevPtr[Count],
                    &Ai13Data->AhciRtMiscData);

       Status = gPciIo->Pci.Read (gPciIo,
                                  EfiPciIoWidthUint8,		
                                  0,
                                  sizeof (PciConfig),
                                  PciConfig);

        //Under Raid mode ATA devices are not added into BBS table.
        if((PciConfig [IDE_SUB_CLASS_CODE]!= SCC_RAID_CONTROLLER) || (Device->DeviceType != ATA)) {
            Status = CreateBbsEntry(Device,
                                    &Ai13Data->DevOutfit[Count],
                                    &BbsEntry,
                                    Ai13Data->DevPtr[Count].bInt13Num,
                                    ((AHCI_I13_DATA*)Ai13Data)->AhciBcvOffset);
            ASSERT_EFI_ERROR(Status);

            // Raise TPL to avoid BBS entry corruption
            OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
            Status = gBiosExtensions->AddBbsEntry(&BbsEntry);
            pBS->RestoreTPL(OldTpl);
            ASSERT(Status == EFI_SUCCESS || Status == EFI_ACCESS_DENIED);
            if(Status == EFI_ACCESS_DENIED) {
                gBiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize); 
                pBS->FreePool(gHddReadData);
                return EFI_SUCCESS;
            }
        }

        Count++;
        if (Count==SATA_PORT_COUNT) break;
    }

    // Patch farReturnCDROMSupportAPIPointer routine with "call farCheckForAHCICdrom"
    CheckForAhciCdromOffset = ((AHCI_I13_DATA*)Ai13Data)->CheckForAhciCdromOffset;
    CheckForAhciCdromAddress =
        (UINT32)((UINTN)gLegacyMemoryAddress<<12) + (UINT32)CheckForAhciCdromOffset;

    addr = (UINT8*)(UINTN)(0xF0000+gLegacy16Data->CDROMSupportAPIOfs+10);
    *addr++ = 0x9A; // far call opcode
    *(UINT32*)addr = CheckForAhciCdromAddress;

    // Copy the image into the shadow
    pBS->CopyMem(gLegacyMemoryAddress, gImage, ImageSize);

    gBiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize);
    pBS->FreePool(gHddReadData);

    return EFI_SUCCESS;
}

/**
    Initialization of chipset specific fields of AHCI runtime
    (INT 13) code.

    @param Seg16, Ofs16 - 16-bit address of chipset specific 16-bit code/data.

    @retval Status of operation

**/

EFI_STATUS
InitCspData (
    IN UINT16  Seg16,
    IN UINT16  Ofs16,
    IN UINT32  BaseAddress,
    IN UINT8  ControllerNumber
)
{
    UINT16      indx, data;
    UINT16      *addr;
    EFI_STATUS  Status;
    UINT16      IoData = 0;
    UINT16      Data = 0;
    
    
    ASSERT(gPciIo);

    // Calculate the address according to the segment ,offset and controller
    // number.
    addr = (UINT16*)(UINTN)(((UINTN)Seg16<<4)+(UINTN)Ofs16)+ 4*(ControllerNumber);

    // Update the Ahci Base Address
    *(UINT32*)(addr+2)=BaseAddress;

    Status = GetAccessInfo (gPciIo, &indx, &data);
    if (EFI_ERROR(Status)) { 
        return Status;
    }

    // Checking whether Index/Data Port is valid
    IoWrite32(indx, 0);
    IoData = (UINT16)IoRead32(data);

    Data = (*Mm16Ptr (BaseAddress, 0));
    if (Data == IoData) { 
    // Update the Index and Data Port
        *addr++ = indx;
        *addr = data;
    }
    
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
