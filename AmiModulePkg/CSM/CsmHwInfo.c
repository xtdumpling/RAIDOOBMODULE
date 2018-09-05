//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
     CSM specific hardware information routines
*/

#include "Csm.h"
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/BlockIo.h>
#include <AmiDxeLib.h>
#include "Pci.h"
#include <AcpiRes.h>
#include <Protocol/LegacyAhci.h>

#define MASTER_DRIVE        0
#define SLAVE_DRIVE         1
#define PRIMARY_CHANNEL     0
#define SECONDARY_CHANNEL   1
#define MAX_IDE_PCI_CONTROLLER (MAX_IDE_CONTROLLER << 1)

// Controller information where IDE devices are connected
IDE_CONTROLLER_INFO_STRUC          gIdeControllerInfo[MAX_IDE_PCI_CONTROLLER];
UINT8                              gIdeController = 0;

UINT16 aInstalledPciIrq[MAX_IDE_PCI_CONTROLLER];

EFI_GUID    gAcpiRsdtPtr = ACPI_20_TABLE_GUID;
EFI_GUID    gAcpiRsdtPtr1_0 = ACPI_10_TABLE_GUID;
EFI_GUID    gSmbiosTable = SMBIOS_TABLE_GUID;

typedef struct {
  ATAPI_IDENTIFY    *IdentifyPtr;
  EFI_HANDLE        DriveHandle;            
} CSM_DRIVE_HANDLE_LIST;

CSM_DRIVE_HANDLE_LIST DriveHandle[MAX_IDE_CONTROLLER*2] ;

EFI_STATUS UpdateCsm16Configuration(EFI_TO_COMPATIBILITY16_BOOT_TABLE*);
VOID GetExtendedMemSize(UINT32*);

#ifdef GenericSio_SUPPORT
VOID GetSioDeviceStatus(SIO_DEV_STATUS *devices);
BOOLEAN IsSioDevicePluggedIn(SIO_DEV_STATUS *plugged_devices, EFI_HANDLE *Handle);
#endif

extern  BIOS_INFO *gCoreBiosInfo;
extern  UINTN gMaxOpRomAddress;


/**

    This function updates Csm16BootTable and BDA. It can be called multiple
    times during POST as the new hardware is discovered and configured.

    @param[in] Csm16BootTable  pointer to EFI_TO_COMPATIBILITY16_BOOT_TABLE

    @retval    EFI_SUCCESS Table is updated successfully.
*/

EFI_STATUS
UpdateCsm16Configuration(
    IN OUT EFI_TO_COMPATIBILITY16_BOOT_TABLE    *Csm16BootTable
)
{
    BDA_DATA    *bda;
    UINT16      machineconfig = 0;
    
    //
    // Fill in EFI_TO_COMPATIBILITY16_BOOT_TABLE
    // Note: CSM specification 0.96 defines the pointer as UINT32 - it is
    //       assumed BbsTable pointer is within 4GB address space.
    Csm16BootTable->BbsTable = (UINT32)gCoreBiosInfo->BbsTable;
    Csm16BootTable->NumberBbsEntries = gCoreBiosInfo->BbsEntriesNo;

    //
    // Updade SIO status in Csm16BootTable->SioData and in BDA
    //
    bda = (BDA_DATA*)((UINTN) 0x400);

#ifdef GenericSio_SUPPORT
    {
        EFI_HANDLE      Handle;
        EFI_STATUS      Status;
        SIO_DEV_STATUS  sio_devices;
        SIO_DEV_STATUS  sio_devices_plugged_in;
        UINT8           Irq;
        UINT8           ComPortIndex;
        UINT16          Address;

// The following definitions are from the up-to-date GenericSio.h
// they can be used after GenericSio.h will be labeled with these changes.
//#define SIO_DEV_COM3        0x1000
//#define SIO_DEV_COM4        0x2000

        sio_devices.DEV_STATUS = SIO_DEV_COM1 | SIO_DEV_COM2 | 0x1000 | 0x2000 |
                                SIO_DEV_LPT  | SIO_DEV_FDC | SIO_DEV_PS2MS;
        GetSioDeviceStatus(&sio_devices);
    
        if (sio_devices.Fdd) {
            Csm16BootTable->SioData.Floppy.NumberOfFloppy = 1;
            machineconfig |= 1;
        }
    
        for(ComPortIndex = 0; ComPortIndex < 4; ComPortIndex ++){
            Status = GetComPortResource(ComPortIndex, &Address, &Irq);
            if (!EFI_ERROR(Status))
            {
                Csm16BootTable->SioData.Serial[ComPortIndex].Address = Address;
                Csm16BootTable->SioData.Serial[ComPortIndex].Irq = Irq;
                Csm16BootTable->SioData.Serial[ComPortIndex].Mode = DEVICE_SERIAL_MODE_NORMAL;
                bda->rs_232_address[ComPortIndex] = Address;
                machineconfig += 0x200;
            }
        }
        // Note that SIO_DEV_STATUS limits the number of LPT to one
        if (sio_devices.Lpt) {
            Status = GetLptResource(&Address, &Irq);
            if (!EFI_ERROR(Status)){
               Csm16BootTable->SioData.Parallel[0].Address = Address;
               Csm16BootTable->SioData.Parallel[0].Irq = Irq;
               Csm16BootTable->SioData.Parallel[0].Mode = DEVICE_PARALLEL_MODE_MODE_OUTPUT_ONLY;
               bda->printer_address[0] = Address;
               machineconfig |= 0x4000;
           }
        }
    
        if ((bda->machine_config & 4) || sio_devices.Ps2Mouse) {
            machineconfig |= 0x4;
    
            sio_devices_plugged_in.DEV_STATUS = 0;
            sio_devices_plugged_in.Ps2Mouse = 1;
            if (IsSioDevicePluggedIn(&sio_devices_plugged_in, &Handle)) {
                Csm16BootTable->SioData.MousePresent = 1;
            }
        }
    }
#endif
    
    machineconfig |= 2; // coprocessor is always reported present

    bda->machine_config = machineconfig;

    GetExtendedMemSize(&Csm16BootTable->OsMemoryAbove1Mb);
    Csm16BootTable->NumberE820Entries = gCoreBiosInfo->NumberE820Entries;

    //
    // Get ACPI&SMBIOS pointers
    //
    //NOTE: CSM specification 0.97 defines the pointers as UINT32
    Csm16BootTable->AcpiTable = (UINT32)GetEfiConfigurationTable(pST, &gAcpiRsdtPtr);
    if (Csm16BootTable->AcpiTable == 0)
         Csm16BootTable->AcpiTable = (UINT32)GetEfiConfigurationTable(pST, &gAcpiRsdtPtr1_0);

    TRACE((-1,"CSM - GetEfiConfigurationTable: ACPI table is at %x\n", Csm16BootTable->AcpiTable));

    Csm16BootTable->SmbiosTable = (UINT32)GetEfiConfigurationTable(pST, &gSmbiosTable);

    return EFI_SUCCESS;
}


/**
    This function returns the status of the floppy drive.
*/
FDD_STATUS GetFDDStatus()
{
#ifdef GenericSio_SUPPORT
    SIO_DEV_STATUS      sio_devices;
    EFI_STATUS          Status;
    FDD_STATUS          DriveStatus = NO_FLOPPY_DRIVE;
    EFI_HANDLE          Handle;
    EFI_BLOCK_IO_PROTOCOL *blkiop;

    sio_devices.DEV_STATUS = 0;
    sio_devices.Fdd = 1;

    if (!IsSioDevicePluggedIn(&sio_devices, &Handle)) return NO_FLOPPY_DRIVE;

    Status = pBS->HandleProtocol(Handle,&gEfiBlockIoProtocolGuid, &blkiop);
    ASSERT_EFI_ERROR(Status);

    if (!EFI_ERROR(Status)) {
        DriveStatus = blkiop->Media->MediaPresent? FLOPPY_INSERTED : NO_FLOPPY_DISK;
    }

    return DriveStatus;
#else
    return NO_FLOPPY_DRIVE;
#endif
}


/**
    This function makes InstallPciHandler CSM16 call

	The input is PCI IDE controller data needed for the IRQ installation
*/

EFI_STATUS
InstallIdePciHandler(
    HDD_INFO* HddInfo1,
    HDD_INFO* HddInfo2
)
{
    EFI_IA32_REGISTER_SET RegSet;
    EFI_STATUS Status;
    UINT8 i;
    EFI_LEGACY_INSTALL_PCI_HANDLER *PciHandlerData = &gCoreBiosInfo->Thunk->PciHandler;
    UINT16 wPciDev = 0xFFFF;
    BOOLEAN IsNativeMode;
    UINT32 Granularity;

    //
    // Check whether PCI IRQ handler for this PCI device has been installed, exit if so.
    //
    for (i = 0; i < MAX_IDE_PCI_CONTROLLER; i++) {
        wPciDev = (UINT16)((HddInfo1->Bus << 8) | (HddInfo1->Device << 3) | HddInfo1->Function);
        if (!aInstalledPciIrq[i]) break;    // New entry found
        if (wPciDev == aInstalledPciIrq[i]) return EFI_SUCCESS;
    }
    ASSERT(i < MAX_IDE_PCI_CONTROLLER);
    if (i == MAX_IDE_PCI_CONTROLLER) return EFI_OUT_OF_RESOURCES;   // Error - no free entries

    //
    // Prepare PCI handler data in Thunk memory
    //
    PciHandlerData->PciBus = (UINT8)HddInfo1->Bus;
    PciHandlerData->PciDeviceFun = (UINT8)((HddInfo1->Device << 3) | HddInfo1->Function);
    PciHandlerData->PciSegment = 0;
    PciHandlerData->PciClass = PCI_CL_MASS_STOR;
    PciHandlerData->PciSubclass = PCI_CL_MASS_STOR_SCL_IDE;
    IsNativeMode = HddInfo1->HddIrq != 14;
    PciHandlerData->PciInterface = (IsNativeMode)?0x8F:0x8A;

    //
    // Primary section
    //
    PciHandlerData->PrimaryIrq = HddInfo1->HddIrq;
    PciHandlerData->PrimaryReserved = 0;
    PciHandlerData->PrimaryControl = HddInfo1->ControlBaseAddress;
    PciHandlerData->PrimaryBase = HddInfo1->CommandBaseAddress;
    PciHandlerData->PrimaryBusMaster = HddInfo1->BusMasterAddress;
    //
    // Secondary section
    //
    PciHandlerData->SecondaryIrq = HddInfo2->HddIrq;
    PciHandlerData->SecondaryReserved = 0;
    PciHandlerData->SecondaryControl = HddInfo2->ControlBaseAddress;
    PciHandlerData->SecondaryBase = HddInfo2->CommandBaseAddress;
    PciHandlerData->SecondaryBusMaster = HddInfo2->BusMasterAddress;

    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy16InstallPciHandler;
    RegSet.X.ES = EFI_SEGMENT (PciHandlerData);
    RegSet.X.BX = EFI_OFFSET (PciHandlerData);

    Status = gCoreBiosInfo->iRegion->UnLock (gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );
    ASSERT_EFI_ERROR(Status);

    FarCall86 (&gCoreBiosInfo->iBios,
                gCoreBiosInfo->Csm16EntrySeg,
                gCoreBiosInfo->Csm16EntryOfs,
                &RegSet,
                NULL,
                0);
    Status = (RegSet.X.AX)? ((RegSet.X.AX & 0x7FFF) | EFI_ERROR_BIT) : EFI_SUCCESS;
    ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR(Status)) {
        aInstalledPciIrq[i] = wPciDev;  // Save the new PCI device entry
    }

    Status = gCoreBiosInfo->iRegion->Lock (gCoreBiosInfo->iRegion,
        (UINT32)gMaxOpRomAddress+1,
        0xFFFFF-(UINT32)gMaxOpRomAddress,
        &Granularity
    );
    ASSERT_EFI_ERROR(Status);

    return EFI_SUCCESS;
}

/**
    This function will check if the BusDevFunc is existing in 
    Idecontrollerinfo structure array,if it exist then it return. 
    If it is not exist, it will add BusDevFunc into IdeControllerInfo structure.

    @param[in]  IdeBusDevFun - BusDevFunc number for a device
*/

void UpdateIdeControllerInfo(UINT16 IdeBusDevFun)
{
    UINTN   i;
    
    for(i = 0; i < gIdeController; i++) {
        if(gIdeControllerInfo[i].BusDevFun == IdeBusDevFun) {
            /// Controller detected 
            return;
        }
    }

    //
    // A new controller is found so BusDevFunc and ControllerNo is added
    // into the array of structure.
    //
    gIdeControllerInfo[gIdeController].BusDevFun = IdeBusDevFun;
    gIdeControllerInfo[gIdeController].ControllerNo = gIdeController;
    gIdeController++;
    
    return;
    
}


/**
    This function collects the information about currently connected ATA/ATAPI
    devices. It stores this information in the HDD_INFO data structure and
    installs legacy interrupt handlers.

    @param DiskInfoHandles - array of handles with DiskInfo protocol
    @param Info - pointer to HDD_INFO array to be updated
*/

EFI_STATUS GetAtaAtapiInfo(
    IN OUT  EFI_HANDLE  **DiskInfoHandles,
    OUT HDD_INFO* Info
)
{
    EFI_STATUS  Status;
    UINTN       i, j, HandlesNo;
    EFI_DISK_INFO_PROTOCOL      *pDiskInfo;
    EFI_DEVICE_PATH_PROTOCOL    *pDevicePath;
    EFI_PCI_IO_PROTOCOL         *pPciIo;
    EFI_BLOCK_IO_PROTOCOL 		*BlkIo;
    EFI_HANDLE  Handle;
    UINTN       Seg, Bus, Dev, Func;
    HDD_INFO    *HddInfo;
    UINT32      PriSec, MasterSlave;
    UINT8       pciCfg[0x40];
    UINT16      priCmdIoAddr, secCmdIoAddr;
    UINT16      priCtlIoAddr, secCtlIoAddr;
    UINT16      bmIoAddr;
    UINT8       priIrq, secIrq;
    BOOLEAN     IsNativeMode, IsAtapiDevice;
    ATAPI_IDENTIFY  *pAtapiIdentifyBuffer;
    UINT32      DataCount = sizeof (ATAPI_IDENTIFY);
    CSM_DRIVE_HANDLE_LIST *pDriveHandle = DriveHandle;
	
    // Clear drive handles list
    pBS->SetMem(pDriveHandle, sizeof(CSM_DRIVE_HANDLE_LIST)*MAX_IDE_CONTROLLER*2, 0);

    //
    // Get the list of DiskInfo handles
    //
    Status = pBS->LocateHandleBuffer(
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandlesNo,
                    DiskInfoHandles);
    if (EFI_ERROR(Status)) return EFI_NOT_FOUND;


    for (i = 0; i < HandlesNo; i++) {
        // Check DiskInfo.Interface field
        static EFI_GUID DiIntrfGuid = EFI_DISK_INFO_IDE_INTERFACE_GUID;

        Status = pBS->HandleProtocol (
            (*DiskInfoHandles)[i],
            &gEfiDiskInfoProtocolGuid,
            &pDiskInfo);    // Get DiskInfo protocol
        ASSERT_EFI_ERROR (Status);
        
        if (guidcmp(&pDiskInfo->Interface, &DiIntrfGuid)) continue;
        

        Status = pBS->HandleProtocol((*DiskInfoHandles)[i],
        								&gEfiBlockIoProtocolGuid, 
        								&BlkIo);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR(Status)) { 
        	continue;    
        }

        Status = pBS->HandleProtocol ((*DiskInfoHandles)[i],
            &gEfiDevicePathProtocolGuid,
            (VOID*)&pDevicePath);
        ASSERT_EFI_ERROR(Status);

        //
        // Get PCI device/function info out of DiskInfo device path
        //
        Status = pBS->LocateDevicePath(
            &gEfiPciIoProtocolGuid,    // Search key
            &pDevicePath,       // Will be updated with PCI device path
            &Handle);           // Will be updated with PCI device handle
        if (EFI_ERROR(Status)) continue;    // PCI device path is not found

        Status = pBS->HandleProtocol (
            Handle,
            &gEfiPciIoProtocolGuid,
            &pPciIo);           // Get PciIo protocol
        ASSERT_EFI_ERROR (Status);

        pDiskInfo->WhichIde(pDiskInfo, &PriSec, &MasterSlave);  // Device/Channel info
        Status = pPciIo->GetLocation(pPciIo, &Seg, &Bus, &Dev, &Func);   // Location on PCI bus      
        ASSERT_EFI_ERROR(Status);                         
       
        /// Update controller Number for the IDE device
        UpdateIdeControllerInfo(((UINT16)Bus<<8) + ((UINT16)Dev<<3) + (UINT16)Func);
                
        pPciIo->Pci.Read(
            pPciIo,
            EfiPciIoWidthUint32,
            0,
            0x10,
            &pciCfg);   // Get 40h bytes of PCI device configuration registers

        if (pciCfg[0xB] != PCI_CL_MASS_STOR) continue;

        if (pciCfg[0xA] == PCI_CL_MASS_STOR_SCL_RAID) continue;

        if (pciCfg[0xA] == 6) {

            EFI_DEVICE_PATH_PROTOCOL *DevicePath;
            //
            // SATA controller is in AHCI mode; we can only handle it if
            // AE bit is not set and controller is operated using legacy ATA/ATAPI
            // mechanisms, not AHCI descriptors. Verify this by checking if
            // SATA device path is installed on this device.
            //
            // Note: previously we were checking for the status of AE in controller's
            // MMIO (ABAR at PciCfg[0x24]+4, Bit31). It was found that for some
            // controllers it is not safe to access MMIO directly. Example: Intel
            // ESB2 controller loses index/data access functionality after any
            // AHCI MMIO register is read directly.
            //
            DevicePath = DPGetLastNode(pDevicePath);
            if (DevicePath->Type == MESSAGING_DEVICE_PATH &&
                DevicePath->SubType == MSG_USB_SATA_DP) {
                continue;
            }
        }

        IsNativeMode = pciCfg[9] & 1;

        if (IsNativeMode) { // for native mode get data from PCI config space
            priCmdIoAddr = *(UINT16*)&pciCfg[0x10] & 0xFFFE;
            secCmdIoAddr = *(UINT16*)&pciCfg[0x18] & 0xFFFE;
            priCtlIoAddr = (*(UINT16*)&pciCfg[0x14] & 0xFFFE) + 2;
            secCtlIoAddr = (*(UINT16*)&pciCfg[0x1C] & 0xFFFE) + 2;
            priIrq = secIrq = pciCfg[0x3C];
        }
        else {  // for legacy mode use hardcoded data
            priCmdIoAddr = 0x1F0;
            secCmdIoAddr = 0x170;
            priCtlIoAddr = 0x3F6;
            secCtlIoAddr = 0x376;
            priIrq = 0xE;
            secIrq = 0xF;
        }
        bmIoAddr =  *(UINT16*)&pciCfg[0x20] & 0xFFFE;

        //
        // All necessary HW data is received; the following loop searches for the
        // next available HddInfo, fills in HDD_INFO structure and installs legacy
        // PCI interrupt if needed.
        //
        for (j = 0, HddInfo = Info; j < MAX_IDE_CONTROLLER; j++, HddInfo++) {
            if (!HddInfo->Bus && !HddInfo->Device && !HddInfo->Function) {
                HddInfo->Status = HDD_PRIMARY;
                HddInfo->Bus = (UINT32)Bus;
                HddInfo->Device = (UINT32)Dev;
                HddInfo->Function = (UINT32)Func;
                HddInfo->CommandBaseAddress = priCmdIoAddr;
                HddInfo->ControlBaseAddress = priCtlIoAddr;
                HddInfo->BusMasterAddress = bmIoAddr;
                HddInfo->HddIrq = priIrq;
                HddInfo++;
                HddInfo->Status = HDD_SECONDARY;
                HddInfo->Bus = (UINT32)Bus;
                HddInfo->Device = (UINT32)Dev;
                HddInfo->Function = (UINT32)Func;
                HddInfo->CommandBaseAddress = secCmdIoAddr;
                HddInfo->ControlBaseAddress = secCtlIoAddr;
                HddInfo->BusMasterAddress = bmIoAddr+8;
                HddInfo->HddIrq = secIrq;
                if (PriSec == PRIMARY_CHANNEL) HddInfo--;
                break;
            }
            if (Bus == HddInfo->Bus && Dev == HddInfo->Device && Func == HddInfo->Function) {
                if (PriSec == SECONDARY_CHANNEL) HddInfo++;
                break;
            }
        }

        //
        // Copy the IDENTIFY_DRIVE information into appropriate HDD_INFO field
        //
        pAtapiIdentifyBuffer = HddInfo->IdentifyDrive;
        if (MasterSlave != MASTER_DRIVE) pAtapiIdentifyBuffer++;
        pDiskInfo->Identify(pDiskInfo, pAtapiIdentifyBuffer, &DataCount);

        //
        // Check whether device is ATA or ATAPI - WORD 0 bits 14 and 15
        //
        IsAtapiDevice = (BOOLEAN)((*(UINT16*)pAtapiIdentifyBuffer & 0xC000) == 0x8000);

        // Skip ATA devices with the block size other than 512 Bytes.
        if ((IsAtapiDevice == FALSE) && (BlkIo->Media->BlockSize != 512)) {
            continue;
        }

        pDriveHandle->IdentifyPtr = pAtapiIdentifyBuffer;
        pDriveHandle->DriveHandle = (*DiskInfoHandles)[i];
        pDriveHandle++;
        //
        // Update HDD_INFO status
        //
        if (PriSec == PRIMARY_CHANNEL) {
            HddInfo->Status |= HDD_PRIMARY;
        } else {
            HddInfo->Status |= HDD_SECONDARY;
        }
        if (MasterSlave == MASTER_DRIVE) {
            if (IsAtapiDevice) {
                HddInfo->Status |= HDD_MASTER_ATAPI_CDROM;
            } else {
                HddInfo->Status |= HDD_MASTER_IDE;
            }
        } else {    // SLAVE_DRIVE
            if (IsAtapiDevice) {
                HddInfo->Status |= HDD_SLAVE_ATAPI_CDROM;
            } else {
                HddInfo->Status |= HDD_SLAVE_IDE;
            }
        }
//        if (IsSataDevice) HddInfo->Status |= HDD_SATA_PATA;
        gCoreBiosInfo->HddCount++;
    }

    return EFI_SUCCESS;
}

            //(EIP20813+)>
/**
    Change INT0F Vector To E000 Segment

    If the IRQ7 vector is in F000 segment, MS-DOS will take over the IRQ7 ISR
    and cause HDDs not working. This routine places IRQ7 handler in E000 segment
    to avoid this problem.

    @return Success of failure of the operation.
*/
#pragma pack(1)
typedef struct
{
    UINT8  Far_JMP;
    UINT32 Address;
}E0000_Int0f;
#pragma pack()

EFI_STATUS
SetINT0F_E0000()
{
    EFI_STATUS  Status;
    E0000_Int0f *P_E0000_Int0f;
    UINT32      LockUnlockAddr, LockUnlockSize;
    UINT32      *PInt15;

    PInt15=(UINT32*)(0x0f*4);

    //
    // Allocate legacy region in E000 segment; store SEG:OFS of the allocated
    // memory in global variables
    //
    Status = GetLegacyRegion(&gCoreBiosInfo->iBios, sizeof(E0000_Int0f), E0000_BIT, 0x10, &P_E0000_Int0f);

    if(EFI_ERROR(Status)) return Status;

    UnlockShadow((UINT8*)P_E0000_Int0f, sizeof(E0000_Int0f), &LockUnlockAddr, &LockUnlockSize);

    P_E0000_Int0f->Far_JMP=0xEA;        //far jump
    P_E0000_Int0f->Address=*PInt15;     //save original vector
    *PInt15= EFI_SEGMENT(P_E0000_Int0f) * 0x10000 | EFI_OFFSET (P_E0000_Int0f); //Set New INT0F Vector

    LockShadow(LockUnlockAddr, LockUnlockSize);

    return EFI_SUCCESS;
}

            //<(EIP20813+)

/**
    This function installs PCI interrupts for all PATA and SATA controllers.

    Note: HDD_INFO is expected to be populated before this call.
*/
EFI_STATUS  InstallIdeInterrupts(
    IN HDD_INFO *HddInfo
)
{
    UINT8 j;
    UINT8 hdd_irq7_set=0;
    HDD_INFO *HddInfo1 = HddInfo;
    HDD_INFO *HddInfo2 = HddInfo;
    EFI_STATUS Status;

    for (j = 0; j < (MAX_IDE_CONTROLLER/2); j++) {
        HddInfo2 = HddInfo1+1;
        if ((HddInfo1->Status &
                (HDD_MASTER_ATAPI_CDROM | HDD_SLAVE_ATAPI_CDROM |
                    HDD_MASTER_IDE | HDD_SLAVE_IDE))||
            (HddInfo2->Status &
                (HDD_MASTER_ATAPI_CDROM | HDD_SLAVE_ATAPI_CDROM |
                    HDD_MASTER_IDE | HDD_SLAVE_IDE)))
        {
            Status = InstallIdePciHandler(HddInfo1, HddInfo2);
            ASSERT_EFI_ERROR(Status);

            //if (EFI_ERROR(Status)) return Status; //(EIP20813-)

                        //(EIP20813+)>

            if( HddInfo1->HddIrq==0x07 || HddInfo2->HddIrq==0x07)
                hdd_irq7_set=1;

            if (EFI_ERROR(Status)) break;

                        //<(EIP20813+)
        }
        HddInfo1+=2;
    }
                        //(EIP20813+)>
    if(hdd_irq7_set)
        SetINT0F_E0000();

                        //<(EIP20813+)
    return EFI_SUCCESS;
}


/**
    This function prepares the description string for the ATA/ATAPI drive and
    places its pointer in the BBS entry.

    Notes:
      1) DriveDisplayName must be located below 1MB
      2) BbsDescStringSegment is UINT16 followed by BbsDescStringOffset

    @param IdentifyDriveData - IDENTIFY_DRIVE data for this device
    @param DriveDisplayName - pointer to the string to be updated
    @param BbsDescStringOffset - pointer to the string description offset in the
                                corresponding BBS entry
*/
VOID
CreateDriveString (
    IN UINT16   *IdentifyDriveData,
    IN UINT16   ChannelInfo,
    OUT DRIVE_DISPLAY_NAME  *DriveDisplayName,
    OUT UINT16  *BbsDescStringOffset,
    UINTN       DeviceAddress
)
{
#if CSM_CREATES_ATA_ATAPI_STRINGS
    UINT8   i, data8;
    UINT16  data16 = IdentifyDriveData[76];	
    UINT8   s[MAX_DRIVE_NAME] = {0};
    // Temp variable introduced in order to reduce the Code length
    UINTN    Temp = 0;

    if(gIdeController > 1) {
        pBS->CopyMem(&s[0], "xC::APAT     :",14);          // "PATA: "
        Temp = 4;
      
        /// find and replace the Controller Number in String
        for(i=0; i < gIdeController; i++) {
            if(gIdeControllerInfo[i].BusDevFun == DeviceAddress) {            
                //// Controller for the AHCI Device found            
                s[0] = gIdeControllerInfo[i].ControllerNo + 0x30; 
                break;
            }
        }  
    }    
    else {
        pBS->CopyMem(&s[0], "APAT     :",10);               // "PATA: "   
    }
      
    // Check Word76 for BIT1 and BIT2; set for SATA drives
    if ((data16 != 0xFFFF) && (data16 & 6)) {
        s[1 + Temp] = 'S';
    }

    if(ChannelInfo & HDD_PRIMARY) {
        s[7 + Temp]='P';
        if(ChannelInfo & (HDD_MASTER_IDE | HDD_MASTER_ATAPI_CDROM )) {
            s[6 + Temp]='M';
        } else {
            s[6 + Temp]='S';
        }
    } else if(ChannelInfo & HDD_SECONDARY) {
        s[7 + Temp]='S';
        if(ChannelInfo & (HDD_MASTER_IDE | HDD_MASTER_ATAPI_CDROM )) {
            s[6 + Temp]='M';
        } else {
            s[6 + Temp]='S';
        }
    }
    
    // Get the drive name out of IdentifyDrive data word 27..46 (upto 40 chars)
    pBS->CopyMem(&s[10 + Temp], IdentifyDriveData+27, MAX_DRIVE_NAME- (11+Temp) );
    // Swap the bytes
    for (i=0; i<MAX_DRIVE_NAME; i+=2) {
        data8=s[i];
        s[i]=s[i+1];
        s[i+1]=data8;
    }

    pBS->CopyMem(DriveDisplayName, s, MAX_DRIVE_NAME);
    // Update string pointer in BBS after converting it to SEG:OFS format
    *BbsDescStringOffset = (UINT16)(UINTN)DriveDisplayName;
    *(BbsDescStringOffset+1) = (UINT16)(((UINTN)DriveDisplayName >> 4) & 0xF000);
#endif
}

/**
  This function returns ATAPI device type depending on the information provided
  by IDENDIFY_DRIVE data. It could call CSM platform functions to return platform
  specific ATAPI device type or if the type is selectable in System Setup.
*/
UINT16
GetAtapiDeviceType(
    IN ATAPI_IDENTIFY *IdentifyDriveData
)
{
    UINT16      DevType;
//    EFI_STATUS  Status;
//    CSM_PLATFORM_PROTOCOL *CsmPlatformProtocol;
/*
    pBS->LocateProtocol(&gCsmPlatformProtocolGuid, NULL, &CsmPlatformProtocol);
    //
    // LocateProtocol will return CsmPlatformProtocol = NULL if protocol is not found.
    //
    if (CsmPlatformProtocol) {
        Status = CsmPlatformProtocol->GetAtapiDeviceType(CsmPlatformProtocol, IdentifyDriveData, &DevType);
        if (!EFI_ERROR(Status)) return DevType; // Identified
    }
*/
    //
    // Use default, "generic" method of device identification
    //
    DevType = BBS_FLOPPY;

    if ((IdentifyDriveData->Raw[0] & 0x1F00) == 0x500)
    {
        DevType = BBS_CDROM;
    }
    return DevType;
}


/**
    This function searches for the IDE mass storage device handle in the list
    of handles and returns the one that matches with the given IdentifyDrive
    information.
*/
EFI_HANDLE  GetDriveHandle(ATAPI_IDENTIFY *IdentifyPtr)
{
    UINT8   i;
    for (i=0; i<MAX_IDE_CONTROLLER*2; i++){
        if (DriveHandle[i].IdentifyPtr == IdentifyPtr)
            return DriveHandle[i].DriveHandle;
    }

    return NULL;
}


/**
  This function prepares BBS table, inserts FDD/IDE/SATA entries in the table
  and calls CSM16->UpdateBbs function to bring up non-BBS compliant entries as well.

 Note:
  This function should be executed only once, when DiskInfo handles are ready.
*/
VOID InstallLegacyMassStorageDevices()
{
    HDD_INFO    *AtaAtapiInfo;
    UINT8       IdeCtl, Indx;
    EFI_STATUS  Status;
    UINT8       count = 0;
    EFI_HANDLE  *DiskInfoHandles = 0;

    BBS_TABLE   *BbsEntry = gCoreBiosInfo->BbsTable;

    //
    // Insert ATA/ATAPI devices into gCoreBiosInfo->Thunk->Csm16BootTable.HddInfo
    //
//    for (Indx = 0; Indx < MAX_IDE_PCI_CONTROLLER; Indx++) aInstalledPciIrq[Indx] = 0;

    AtaAtapiInfo = gCoreBiosInfo->Thunk->Csm16BootTable.HddInfo;

    Status = GetAtaAtapiInfo(&DiskInfoHandles, AtaAtapiInfo);

    if (!EFI_ERROR(Status)) {   // some IDE device are connected
        InstallIdeInterrupts(AtaAtapiInfo);
        //
        // Update BBS table with controller/device information
        //
        for (IdeCtl = 0; IdeCtl < MAX_IDE_CONTROLLER; IdeCtl++) {
            if (!(AtaAtapiInfo[IdeCtl].Status &
                ( HDD_MASTER_ATAPI_CDROM | HDD_SLAVE_ATAPI_CDROM | HDD_MASTER_IDE | HDD_SLAVE_IDE ))) {
                continue;
            }
            Indx = IdeCtl*2 + 1;    // 1st entry is taken for floppy

            BbsEntry[Indx].Bus = BbsEntry[Indx+1].Bus = AtaAtapiInfo[IdeCtl].Bus;
            BbsEntry[Indx].Device = BbsEntry[Indx+1].Device = AtaAtapiInfo[IdeCtl].Device;
            BbsEntry[Indx].Function = BbsEntry[Indx+1].Function = AtaAtapiInfo[IdeCtl].Function;
            BbsEntry[Indx].Class = BbsEntry[Indx+1].Class = 1;
            BbsEntry[Indx].SubClass = BbsEntry[Indx+1].SubClass = 1;

            if (AtaAtapiInfo[IdeCtl].CommandBaseAddress) {  // real controller
                //
                // Create entry for master device connected to this controller
                //
                if (AtaAtapiInfo[IdeCtl].Status & (HDD_MASTER_IDE | HDD_MASTER_ATAPI_CDROM)) {
                    BbsEntry[Indx].BootPriority = BBS_UNPRIORITIZED_ENTRY;
                    if (AtaAtapiInfo[IdeCtl].Status & HDD_MASTER_ATAPI_CDROM) {
                        BbsEntry[Indx].DeviceType = GetAtapiDeviceType(&AtaAtapiInfo[IdeCtl].IdentifyDrive[0]);
                    } else {
                        BbsEntry[Indx].DeviceType = BBS_HARDDISK;
                    }
                    CreateDriveString(
                        AtaAtapiInfo[IdeCtl].IdentifyDrive[0].Raw,
                        AtaAtapiInfo[IdeCtl].Status & ~(HDD_SLAVE_IDE | HDD_SLAVE_ATAPI_CDROM),
                        &gCoreBiosInfo->Thunk->DriveDisplayName[count++],
                        &BbsEntry[Indx].DescStringOffset,                        
                        ((UINT16)AtaAtapiInfo[IdeCtl].Bus << 8) +((UINT16)AtaAtapiInfo[IdeCtl].Device<<3)\
                          + ((UINT16) AtaAtapiInfo[IdeCtl].Function )                      
                    );
                    //TRACE((-1, "i=%d, bbsentry=%x, diskinfo=%x\n", Indx, &BbsEntry[Indx], *DiskInfoHandles));
                    *(UINTN*)(&BbsEntry[Indx].IBV1) = (UINTN)GetDriveHandle(AtaAtapiInfo[IdeCtl].IdentifyDrive);//(EIP71972)
                }
                //
                // Create entry for slave device connected to this controller
                //
                if (AtaAtapiInfo[IdeCtl].Status & (HDD_SLAVE_IDE | HDD_SLAVE_ATAPI_CDROM)) {
                    BbsEntry[Indx+1].BootPriority = BBS_UNPRIORITIZED_ENTRY;
                    if (AtaAtapiInfo[IdeCtl].Status & HDD_SLAVE_ATAPI_CDROM) {
                        BbsEntry[Indx+1].DeviceType = GetAtapiDeviceType(&AtaAtapiInfo[IdeCtl].IdentifyDrive[1]);
                    } else {
                        BbsEntry[Indx+1].DeviceType = BBS_HARDDISK;
                    }
                    CreateDriveString(
                        AtaAtapiInfo[IdeCtl].IdentifyDrive[1].Raw,
                        AtaAtapiInfo[IdeCtl].Status & ~(HDD_MASTER_IDE | HDD_MASTER_ATAPI_CDROM),
                        &gCoreBiosInfo->Thunk->DriveDisplayName[count++],
                        &BbsEntry[Indx+1].DescStringOffset,                       
                        ((UINT16)AtaAtapiInfo[IdeCtl].Bus << 8) +((UINT16)AtaAtapiInfo[IdeCtl].Device<<3)\
                          + ((UINT16) AtaAtapiInfo[IdeCtl].Function )                   
                    );
                    //TRACE((-1, "i=%d, bbsentry=%x, diskinfo=%x\n", Indx, &BbsEntry[Indx+1], *DiskInfoHandles));
                    *(UINTN*)(&BbsEntry[Indx+1].IBV1) = (UINTN)GetDriveHandle(&AtaAtapiInfo[IdeCtl].IdentifyDrive[1]);//(EIP71972)
                }
            }
        }
    }
	{
		AMI_AHCI_INT13_INIT_PROTOCOL    *Aint13;
		Status = pBS->LocateProtocol(&gAint13ProtocolGuid, NULL, &Aint13);
		if (!EFI_ERROR(Status)) {
			Aint13->InitAhciInt13Support();
		}
	}
}


/**
    This function returns system memory map and count of the memory entries in
    the map.
*/
EFI_STATUS GetSystemMemoryMap(
    OUT EFI_MEMORY_DESCRIPTOR **MemMap,
    OUT UINTN *MemDescSize,
    OUT UINTN *MemEntriesCount
)
{
    EFI_STATUS Status;
    UINTN MemMapSize, MemMapKey;
    UINT32 MemDescVer;
    *MemMap = NULL;

    MemMapSize = 0; // GetMemoryMap will return the size needed for the map
    Status = pBS->GetMemoryMap(&MemMapSize, *MemMap,
                    &MemMapKey, MemDescSize, &MemDescVer);

    // The following memory allocation may alter memory map.
    // Let's add space for 5 more descriptors to be sure buffer is big enough
    if (Status != EFI_BUFFER_TOO_SMALL) return Status;
    MemMapSize += 5 * *MemDescSize;
    Status = pBS->AllocatePool(EfiBootServicesData, MemMapSize, MemMap);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->GetMemoryMap(&MemMapSize, *MemMap,
                    &MemMapKey, MemDescSize, &MemDescVer);
    ASSERT_EFI_ERROR(Status);

    *MemEntriesCount = (UINT16)(MemMapSize / *MemDescSize);
    return Status;
}

/**
  This function looks through the system memory map, finds the available memory
  regions and returns the size in bytes of the system memory above 1 MB.

 Notes:
  If there is more than 4 GB of memory installed, then function will return 4GB-1MB.
  If the amount of memory is 4GB or more, some memory is remapped above 4GB by the
  chipset. In this case this function will return the amount of memory before the
  remapped area.
*/
VOID GetExtendedMemSize(UINT32* ExtMemSize)
{
    UINTN MemDescSize;
    EFI_MEMORY_DESCRIPTOR *MemMap;
    EFI_MEMORY_DESCRIPTOR *mm;
    UINTN count = 0;
    UINTN MemEntriesCount;
    UINT64 cs = 0x100000;

    GetSystemMemoryMap(&MemMap, &MemDescSize, &MemEntriesCount);

    for (mm = MemMap; count < MemEntriesCount; mm=(EFI_MEMORY_DESCRIPTOR*)((UINT8*)mm+MemDescSize), count++) {
        if (mm->PhysicalStart < 0x100000) continue; // skip low memory entries

        if (mm->Type == EfiReservedMemoryType ||
            mm->Type == EfiMemoryMappedIO ||
            mm->Type == EfiMemoryMappedIOPortSpace ||
            mm->Type == EfiRuntimeServicesCode ||
            mm->Type == EfiRuntimeServicesData ||
            mm->Type == EfiUnusableMemory ||
            mm->Type == EfiPalCode ||
            mm->Type == EfiACPIReclaimMemory ||
            mm->Type == EfiACPIMemoryNVS)
        {
            break;
        }

        cs = mm->PhysicalStart + Shl64(mm->NumberOfPages, EFI_PAGE_SHIFT);
    }
    pBS->FreePool(MemMap);

    *ExtMemSize = (UINT32)cs - 0x100000;
}


/**
  This function fills in the E820 table entry and adjusts the input entry
  pointer. If the new entry is the extension of the previous one, then entry
  is "extended".
*/
VOID
InsertE820Entry (
    E820_ENTRY **Entry,
    UINT64 Address,
    UINT64 Size,
    E820_MEMORY_TYPE Type,
    UINT16 *Count)
{
    E820_ENTRY *E820Entry = *Entry;
    E820_ENTRY *PrevE820Entry = E820Entry - 1;

    if (((PrevE820Entry->Adr + PrevE820Entry->Len) == Address) &&
        ((PrevE820Entry->Type == Type)
#if AGGRESSIVELY_JOINED_E820_ENTRIES
        || ((Type == MemRangeAvl) && ((Address != 0x100000) && (Address < 0x100000000)))
#endif
))
    {
        // extend the current entry without touching Adr and Type
        E820Entry = PrevE820Entry;
        E820Entry->Len += Size;
    } else {    // new entry
        E820Entry->Adr = Address;
        E820Entry->Len = Size;
        E820Entry->Type = Type;
        (*Count)++;
    }
    E820Entry->ExtendedAttr = E820_EXTATTR_ADDRESS_RANGE_ENABLED;
    *Entry = ++E820Entry;
}


/**
 This function retrieves the system memory map and converts it into E820 map format.

 @param     e820_entry Pointer to the 1st E820 entry in BIOS_INFO data structure
 @return    Number of E820 memory map entries
*/

UINT32
UpdateE820Map(E820_ENTRY *e820_entry)
{
    UINT16 Count = 0;
    UINTN MemDescSize;
    EFI_MEMORY_DESCRIPTOR *MemMap;
    EFI_MEMORY_DESCRIPTOR *mm;
    UINTN MemEntriesCount;
    UINTN i;
    E820_ENTRY *E820Entry = e820_entry;
    E820_MEMORY_TYPE MemType;
    UINT32                  EbdaAddress;
    BDA_DATA                *bda;

    GetSystemMemoryMap(&MemMap, &MemDescSize, &MemEntriesCount);

    bda = (BDA_DATA*)((UINTN) 0x400);   // 40:0

    EbdaAddress = (bda->ext_bios_data_seg)<<4; // 40:0e
    //
    // Start creating E820 table entries
    //
    InsertE820Entry(&E820Entry, 0,  (640 - ((UINT8)((0xA0000 - EbdaAddress) >> 10)) )<< 10, MemRangeAvl, &Count);
    InsertE820Entry(&E820Entry, (640 - ((UINT8)((0xA0000 - EbdaAddress) >> 10)) ) << 10, ((UINT8)((0xA0000 - EbdaAddress) >> 10)) << 10, MemRangeRes, &Count);
    InsertE820Entry(&E820Entry, 0xE0000, 0x20000, MemRangeRes, &Count);     // E0000..FFFFF
    //
    // Insert entries according to memory map; EFI memory types are mapped to ACPI address
    // range types according to ACPI 3.0 Specification, Chapter 14.3.
    //
    for (mm = MemMap, i = 0; i < MemEntriesCount; i++, mm=(EFI_MEMORY_DESCRIPTOR*)((UINT8*)mm+MemDescSize)) {
        if (mm->PhysicalStart < 0x100000) continue; // skip low memory entries
        switch (mm->Type) {
            case EfiMemoryMappedIO:
                    if (!(mm->Attribute & EFI_MEMORY_RUNTIME)) continue;
            case EfiReservedMemoryType:
            case EfiRuntimeServicesCode:
            case EfiRuntimeServicesData:
            case EfiUnusableMemory:
            case EfiMemoryMappedIOPortSpace:
            case EfiPalCode:
                    MemType = MemRangeRes;
                    break;
            case EfiACPIReclaimMemory:
                    MemType = MemRangeAcpiReclaim;
                    break;
            case EfiACPIMemoryNVS:
                    MemType = MemRangeAcpiNVS;
                    break;
            case EfiPersistentMemory:
                    MemType = MemRangePersistentMemory;
                    break;
            default: MemType = MemRangeAvl;
        }
        InsertE820Entry(&E820Entry,
                        mm->PhysicalStart,
                        Shl64(mm->NumberOfPages, EFI_PAGE_SHIFT),
                        MemType,
                        &Count);
        if (Count == MAX_E820_ENTRIES) break;
    }
    pBS->FreePool(MemMap);
    return Count;
}


/**
    CMOS access helper
*/

EFI_STATUS CmosRW(
    BOOLEAN rw,
    UINT32 reg,
    UINT8 *data
)
{
    UINT32 reg_;
    UINT8 data_;
    EFI_STATUS Status;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRB;

    Status = pBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid,
        NULL,
        &PciRB
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) return Status;

    reg_ = reg;

    PciRB->Io.Write (PciRB, EfiPciWidthUint8, 0x70, 1, &reg_);  // Set index

    if (rw) {   // read
        PciRB->Io.Read (PciRB, EfiPciWidthUint8, 0x71, 1, &data_);
        *data = data_;
    } else {        // write
        data_ = *data;
        PciRB->Io.Write (PciRB, EfiPciWidthUint8, 0x71, 1, &data_);
    }
    return EFI_SUCCESS;
}


/**
    This function updates CMOS before booting to Legacy OS
*/

EFI_STATUS UpdateCmos()
{
//  UINT8 reg;
    UINT8 data;
    //
    // Clear bits 0..3 on the CMOS ofs 0xE
    //
    CmosRW(TRUE, 0xE, &data);   // Read register E
    data &= 0xF0;
    CmosRW(FALSE, 0xE, &data);

    return EFI_SUCCESS;
}



/**
    This function enables or disables NMI according to setup question.
*/
EFI_STATUS
EnableDisableNmi()
{
    UINT16 data16;
    UINT8 data8;
    EFI_STATUS Status;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRB;

    Status = pBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid,
        NULL,
        &PciRB
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) return Status;

    //
    // Update IO reg 61h - NMI_SC-NMI Status and Control Register
    //
    PciRB->Io.Read (PciRB, EfiPciWidthUint8, 0x61, 1, &data8);
    data8 &= 0xF3;
    data16 = 0xD;	//    data16 = NmiToBeEnabled()? 0xD : 0xC8D;
    data8 |= (data16 >> 8);
    PciRB->Io.Write (PciRB, EfiPciWidthUint8, 0x61, 1, &data8);
    //
    // Update IO reg 70h - NMI_EN-NMI Enable(and Real Time Clock Index)
    //
    data8 = (UINT8)data16;
    PciRB->Io.Write (PciRB, EfiPciWidthUint8, 0x70, 1, &data8);

    return EFI_SUCCESS;
}


/**
    This callback function then will process the list of root bridges
    and will update the LastPciBus field of EFI_COMPATIBILITY16_TABLE
    accordingly.
*/
VOID
UpdatePciLastBusCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
)
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *RBProtocol;
    ASLR_QWORD_ASD      *Descriptors;
    UINTN               Count = 0;
    UINTN               i = 0;
    EFI_HANDLE          *Buffer;
    UINT8               MaxBus=0;
    EFI_STATUS          Status;
    UINT32              Granularity;
    LEGACY16_TO_EFI_DATA_TABLE_EXT *Csm16Data;
    UINT8               *NextRootBridgeBus;
    UINT8               CsmRbCount = 0;
    BOOLEAN             Csm16Is75Plus;
    
    // UnLock E000 and F000 segments
    Status = gCoreBiosInfo->iRegion->UnLock (
             gCoreBiosInfo->iRegion,
             (UINT32)0xe0000,
             (UINT32)0x20000,
             &Granularity
             );
    ASSERT_EFI_ERROR(Status);

    // Locate the RootBridge protocol
    Status = pBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &Count,
                  &Buffer
                  );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;

    Csm16Is75Plus = *(UINT8*)0xf0019 > 0x75;   // CSM version 76 or later

    if (Csm16Is75Plus) {
        Csm16Data = (LEGACY16_TO_EFI_DATA_TABLE_EXT*)(UINTN)(0xf0000 + *(UINT16*)0xfff4c);
        NextRootBridgeBus = (UINT8*)((UINTN)Csm16Data->RbMinBusArrayOfs + 0xf0000);
    }

    for (i = 0; i < Count; i++) {
        Status = pBS->HandleProtocol (
                    Buffer[i],
                    &gEfiPciRootBridgeIoProtocolGuid,
                    &RBProtocol
                    );

        if (EFI_ERROR(Status)) continue;
        //
        // Get the Configuration
        //
        RBProtocol->Configuration(
                  RBProtocol,
                  &Descriptors
                  );

        if ((Descriptors)->Hdr.HDR != ASLV_END_TAG_HDR) {
            //
            // go till we get the Resource type = Bus Number range
            //
            while (Descriptors->Hdr.HDR != ASLV_END_TAG_HDR) {

                if (Descriptors->Type == ASLRV_SPC_TYPE_BUS) {
                    //
                    // We got the type;update the LastPCiBus of csm16header
                    //
                    if (Csm16Is75Plus) {
                        if (++CsmRbCount < Csm16Data->RbArrayCount) {
                            *NextRootBridgeBus++ = (UINT8)Descriptors->_MIN;
                        }
                    }
                    if((UINT8)Descriptors->_MAX > MaxBus ) {
                        MaxBus =(UINT8)Descriptors->_MAX;
                    }
                }
                Descriptors++;
            }
        }
    }

    // Update Pci Last Bus in Csm16 header
    // Note: do not report FF; this is XP limitation, EIP#24852
    gCoreBiosInfo->Csm16Header->LastPciBus = (MaxBus == 0xff)? 0xfe : MaxBus;

    // Update CSM16 header checksum as LastPciBus field of it has been changed
    ChecksumCSM16Header(gCoreBiosInfo->Csm16Header);

    // Look for _32_ structure and update offset 16 (0x10) with the MaxBus
    {
        UINTN Adr = 0xe0000;

        TRACE((-1,"CSM - Attempting to update RT32 binary with the MaxBus (0x%x)\n", MaxBus));

        for (; Adr < 0x100000; Adr += 0x10) {
            if (*(UINT32*)Adr == 0x5f32335f) {
                // found '_32_', verify checksum
                UINT8 i;
                UINT8 CheckSum = 0;
                TRACE((-1,"_32_ signature is found at %x,", Adr));
                for (i=0; i<0x10; i++) {
                    CheckSum += *((UINT8*)Adr+i);
                }
                if (CheckSum == 0) {
                    TRACE((-1,"checksum is OK\n"));
                    *((UINT8*)Adr+0x10) = MaxBus;
                }
                else {
                    TRACE((-1,"checksum is invalid.\n"));
                }
            }
        }
    }
    
    //
    // Lock E000 and F000 segments
    //
    Status = gCoreBiosInfo->iRegion->Lock (
            gCoreBiosInfo->iRegion,
            (UINT32)0xe0000,
            (UINT32)0x20000,
            &Granularity
            );
    ASSERT_EFI_ERROR (Status);

    // Close event, this function is to be executed once.
    if (Event != NULL) {
        pBS->CloseEvent(Event);
    }
}

/**
 * Goes through memory map looking for the requested memory block within 1MB..2GB range
 * 
 * @param[in] MemDesc   Starting memory descriptor of the memory map
 * @param[in] MemEntriesCount   Count of the memory map entries
 * @param[in] MemDescSize       Size of the memory descriptor
 * @param[in] DesiredBlockSize  Number of pages requested to be found
 * @param[out] MemAddress       Found memory block
 * @param[out] NumberOfPages    Found memory block size
 * @retval EFI_SUCCESS          Memory block is found
 * @retval EFI_NOT_FOUND        Memory block is not found
 */
EFI_STATUS FindMemoryBlockForHiPmm(
    EFI_MEMORY_DESCRIPTOR *MemMap,
    UINTN MemEntriesCount,
    UINTN MemDescSize,
    UINTN DesiredBlockSize,
    EFI_PHYSICAL_ADDRESS *MemAddress,
    UINT64 *NumberOfPages
)
{
    EFI_MEMORY_DESCRIPTOR *mm;
    UINTN count = 0;
    
    for (mm = MemMap; count < MemEntriesCount; mm=(EFI_MEMORY_DESCRIPTOR*)((UINT8*)mm+MemDescSize), count++) {
        if (mm->PhysicalStart < 0x100000) continue; // skip low memory entries
        // Skip above 2GB memory entries:
        // CORE0292.1 - value above 2GB will be assumed a negative number in find_free_memory algorithm
        if (mm->PhysicalStart > 0x7fffffff) continue; 

        if (mm->Type == EfiConventionalMemory && (mm->NumberOfPages >= DesiredBlockSize))
        {
            *MemAddress = mm->PhysicalStart;
            *NumberOfPages = mm->NumberOfPages;
            break;
        }
    }

    return (count == MemEntriesCount)? EFI_NOT_FOUND : EFI_SUCCESS;
}

/**
  Allocates the memory at 1MB..2GB that can be used for high memory PMM allocations.

  This EfiBootServicesData memory block should be outside the "main" BS memory
  so that it can be freed during READY_TO_BOOT. For that we go through EFI memory
  map looking for unallocated region of a given size.

  @param[in]    BlockSize     The number of pages to be allocated.
  @param[out]   BlockAddr     The address of the allocated block.

  @retval EFI_SUCCESS   Memory block is allocated.
  @retval Error Value   Memory can not be allocated.

*/
EFI_STATUS AllocateHiMemPmmBlock(
    IN  UINTN   BlockSize,
    OUT UINTN   *BlockAddr
)
{
    UINTN MemDescSize;
    EFI_MEMORY_DESCRIPTOR *MemMap;
    EFI_MEMORY_DESCRIPTOR *mm;
    UINTN count = 0;
    UINTN MemEntriesCount;
    EFI_PHYSICAL_ADDRESS HiPmmMemory;
    EFI_STATUS Status;
    UINT64 NumberOfPages;
    UINT64 BlockLength;
    
    GetSystemMemoryMap(&MemMap, &MemDescSize, &MemEntriesCount);

    // print memory map
    for (mm = MemMap; count < MemEntriesCount; mm=(EFI_MEMORY_DESCRIPTOR*)((UINT8*)mm+MemDescSize), count++) {
        TRACE((-1, "%02d: %016lx, %05x, ", count, mm->PhysicalStart, mm->NumberOfPages)); // AptioV Purley Override: AEP changes
        switch (mm->Type) {
            case EfiReservedMemoryType: TRACE((-1, "EfiReservedMemoryType\n")); break;
            case EfiLoaderCode: TRACE((-1, "EfiLoaderCode\n")); break;
            case EfiLoaderData: TRACE((-1, "EfiLoaderData\n")); break;
            case EfiBootServicesCode: TRACE((-1, "EfiBootServicesCode\n")); break;
            case EfiBootServicesData: TRACE((-1, "EfiBootServicesData\n")); break;
            case EfiRuntimeServicesCode: TRACE((-1, "EfiRuntimeServicesCode\n")); break;
            case EfiRuntimeServicesData: TRACE((-1, "EfiRuntimeServicesData\n")); break;
            case EfiConventionalMemory: TRACE((-1, "EfiConventionalMemory\n")); break;
            case EfiUnusableMemory: TRACE((-1, "EfiUnusableMemory\n")); break;
            case EfiACPIReclaimMemory: TRACE((-1, "EfiACPIReclaimMemory\n")); break;
            case EfiACPIMemoryNVS: TRACE((-1, "EfiACPIMemoryNVS\n")); break;
            case EfiMemoryMappedIO: TRACE((-1, "EfiMemoryMappedIO\n")); break;
            case EfiMemoryMappedIOPortSpace: TRACE((-1, "EfiMemoryMappedIOPortSpace\n")); break;
            case EfiPalCode: TRACE((-1, "EfiPalCode\n")); break;
            case EfiPersistentMemory:  TRACE((-1, "EfiPersistentMemory\n")); break; // AptioV Purley Override: AEP changes
            default: TRACE((-1, "%x\n", mm->Type));
        }
    }

    // Try to allocate Size*4 block and use the middle of it; if not found, then try to allocate Size*2
    // and use the end
    Status = FindMemoryBlockForHiPmm(MemMap, MemEntriesCount, MemDescSize, BlockSize*4, &HiPmmMemory, &NumberOfPages);
    if (!EFI_ERROR(Status)) {
        BlockLength = Shl64(NumberOfPages, 11); // Middle of the block
    } else {
        Status = FindMemoryBlockForHiPmm(MemMap, MemEntriesCount, MemDescSize, BlockSize*2, &HiPmmMemory, &NumberOfPages);
        BlockLength = Shl64(NumberOfPages, 12); // End of the block
    }
    ASSERT_EFI_ERROR(Status);
    
    HiPmmMemory += BlockLength;

    pBS->FreePool(MemMap);


    Status = pBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData, BlockSize*2, &HiPmmMemory);
    *BlockAddr = (UINTN)HiPmmMemory;

    return Status;
}

// The following definition is from DebugLib.h; currently DebugLib.h can not be included
// as it clashes with AmiDxeLib.h definitions
#define DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED       0x08
BOOLEAN
ClearMemoryEnabled (
  VOID
  )
{
  return (BOOLEAN) ((PcdGet8 (PcdDebugPropertyMask) & DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED) != 0);
}

/**
  READY_TO_BOOT notification callback that frees high memory PMM allocations.
  
  Hi memory PMM is a double buffer. First half is a real PMM memory, second half is
  a backup. Before freeing make a backup copy, then free the first block, then restore
  memory, then free the backup block. This is done to prevent clearing memory while
  executing FreePages.
  Note: double buffering is only needed when DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED is 
  set in PcdDebugPropertyMask.
*/
VOID  FreePmmBeforeBoot (  
    IN EFI_EVENT            Event,
    IN VOID                 *Context
)
{
    EFI_STATUS Status;
    UINTN   NumberOfPages = (gCoreBiosInfo->Thunk->Csm16InitTable.HiPmmMemorySizeInBytes >> 12) + 1;
    UINTN   Size = NumberOfPages << 12;
    UINTN   Address = gCoreBiosInfo->Thunk->Csm16InitTable.HiPmmMemory;

    if (Address != 0)
    {
        if (ClearMemoryEnabled ())
        {
            pBS->CopyMem((VOID*)(Address+Size), (VOID*)Address, Size);
            Status = pBS->FreePages(Address, NumberOfPages);
            ASSERT_EFI_ERROR(Status);
            pBS->CopyMem((VOID*)Address, (VOID*)(Address+Size), Size);
            Status = pBS->FreePages(Address+Size, NumberOfPages);
            ASSERT_EFI_ERROR(Status);
        }
        else 
        {
            Status = pBS->FreePages(Address, NumberOfPages*2);
            ASSERT_EFI_ERROR(Status);
        }
        TRACE((-1, "Free HI PMM memory: %r\n", Status));
    }
    pBS->CloseEvent(Event);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
