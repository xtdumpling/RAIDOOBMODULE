//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file AtaPassThru.c
    AtaPassThru Protocol Implementation

**/

//---------------------------------------------------------------------------

#include  "AtaPassThruSupport.h"

//---------------------------------------------------------------------------

#ifndef EXECUTE_DEVICE_DIAGNOSTIC
#define EXECUTE_DEVICE_DIAGNOSTIC       0x90
#endif

EFI_HANDLE                      Handle;
AMI_ATA_PASS_THRU_INIT_PROTOCOL *gAtaPassThruInitProtocol;
ATA_PASS_THRU_PROTOCOL          *AtaPassThruInterface;
EFI_ATA_PASS_THRU_MODE          *Mode;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
PLATFORM_IDE_PROTOCOL                   *gPlatformIdeProtocol;
#endif

/**
    Driver Entry Point provides services allowing ATA Pass Thru
    commands to be sent to ATA Devices attached to controller

    @param    ImageHandle
    @param    *SystemTable

    @retval   EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
EFIAPI AtaPassThruEntry (
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable
)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  Handle = NULL;

    InitAmiLib(ImageHandle,SystemTable);

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(AMI_ATA_PASS_THRU_INIT_PROTOCOL),
                                (VOID**)&gAtaPassThruInitProtocol );

    if(EFI_ERROR(Status)) return Status;

    // Install AtaPAssThruInit Protocol.
    gAtaPassThruInitProtocol->InstallAtaPassThru        = InstallAtaPassThru;
    gAtaPassThruInitProtocol->StopAtaPassThruSupport    = StopAtaPassThruSupport;

    Status = pBS->InstallProtocolInterface(
                                &Handle,
                                &gAmiAtaPassThruInitProtocolGuid,
                                EFI_NATIVE_INTERFACE,
                                gAtaPassThruInitProtocol
                                );
    ASSERT_EFI_ERROR( Status );

    return Status;
}

/**
    Installs AtaPassThru APIs provides services allowing
    ATA commands to be sent to ATA Devices attached to controller

    @param    IN VOID       *Controller,
    @param    IN BOOLEAN    ModeFlag

    @retval   EFI_SUCCESS or EFI_OUT_OF_RESOURCES

**/

EFI_STATUS
InstallAtaPassThru (
  IN    EFI_HANDLE              Controller,
  IN    BOOLEAN                 ModeFlag
)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  DeviceHandle;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *pSataDevInterface   = NULL;
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *pIdeBusInterface    = NULL;
#endif
    UINT16                      DevCount = 0;


    DeviceHandle = (EFI_HANDLE)Controller;

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(ATA_PASS_THRU_PROTOCOL),
                                (VOID**)&AtaPassThruInterface
                                );
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(EFI_ATA_PASS_THRU_MODE),
                                (VOID**)&Mode
                                );
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;

    // Initialize all the protocol members
    AtaPassThruInterface->EfiAtaPassThru.Mode              =   Mode;
    AtaPassThruInterface->EfiAtaPassThru.PassThru          =   PassThru;
    AtaPassThruInterface->EfiAtaPassThru.GetNextPort       =   GetNextPort;
    AtaPassThruInterface->EfiAtaPassThru.GetNextDevice     =   GetNextDevice;
    AtaPassThruInterface->EfiAtaPassThru.BuildDevicePath   =   BuildDevicePath;
    AtaPassThruInterface->EfiAtaPassThru.GetDevice         =   GetDevice;
    AtaPassThruInterface->EfiAtaPassThru.ResetPort         =   ResetPort;
    AtaPassThruInterface->EfiAtaPassThru.ResetDevice       =   ResetDevice;

    AtaPassThruInterface->ModeFlag                         =   ModeFlag;
    AtaPassThruInterface->ControllerHandle                 =   Controller;

    // Set the attributes accordingly
    Mode->Attributes    = EFI_ATA_PASS_THRU_ATTRIBUTES_PHYSICAL | EFI_ATA_PASS_THRU_ATTRIBUTES_LOGICAL;
    Mode->IoAlign       = 0;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if(ModeFlag){
        // Get list of handles on which AMI_AHCI_BUS_PROTOCOL is installed and number of drives connected
        Status = GetSataDevCountandAddressBuffer((EFI_ATA_PASS_THRU_PROTOCOL *)AtaPassThruInterface,&pSataDevInterface,&DevCount);
        if (EFI_ERROR(Status)) {
            AtaPassThruInterface->AtaDeviceBuffer   = NULL;
            AtaPassThruInterface->AtaDeviceCount    = 0;
        }else{
            AtaPassThruInterface->AtaDeviceBuffer   = (VOID *)pSataDevInterface;
            AtaPassThruInterface->AtaDeviceCount    = DevCount;
        }
    }
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if(!ModeFlag){
        // Get list of handles on which AMI_IDE_BUS_INIT_PROTOCOL is installed and number of drives connected
        Status = GetIdeDevCountandAddressBuffer((EFI_ATA_PASS_THRU_PROTOCOL *)AtaPassThruInterface, &pIdeBusInterface,&DevCount);
        if (EFI_ERROR(Status)) {
            AtaPassThruInterface->AtaDeviceBuffer   = NULL;
            AtaPassThruInterface->AtaDeviceCount    = 0;
        }else{
            AtaPassThruInterface->AtaDeviceBuffer   = (VOID *)pIdeBusInterface;
            AtaPassThruInterface->AtaDeviceCount    = DevCount;
        }
    }
#endif
    Status = pBS->InstallProtocolInterface(
                &DeviceHandle,
                &gEfiAtaPassThruProtocolGuid,
                EFI_NATIVE_INTERFACE,
                (ATA_PASS_THRU_PROTOCOL*)&(AtaPassThruInterface->EfiAtaPassThru)
                );

    return Status;

}

/**
    Stops the AtaPassThru Support security interface.

    @param    IN VOID       *Controller,
    @param    IN BOOLEAN    ModeFlag

    @retval

**/

EFI_STATUS
StopAtaPassThruSupport (
    IN  EFI_HANDLE          Controller,
    IN  BOOLEAN             ModeFlag
)
{
    EFI_STATUS              Status;
    EFI_HANDLE              DeviceHandle;

    DeviceHandle = (EFI_HANDLE)Controller;

    //  Uninstall AtaPassThru Protocol for the controller.
    Status = pBS->UninstallMultipleProtocolInterfaces(
        DeviceHandle,
        &gEfiAtaPassThruProtocolGuid,
        (ATA_PASS_THRU_PROTOCOL*)&(AtaPassThruInterface->EfiAtaPassThru),
        NULL
        );

    if(Status == EFI_SUCCESS ) {
        pBS->FreePool(AtaPassThruInterface->AtaDeviceBuffer);
        pBS->FreePool(AtaPassThruInterface);
        pBS->FreePool(Mode);
    }

    return Status;
}

/**
    Search device path by specific Type and SubType

    @param    IN    OUT     EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    @param    IN    UINT8   Type,
    @param    IN    UINT8   SubType

    @retval   EFI_DEVICE_PATH_PROTOCOL - Device path found and the pointer of device path returned
    @retval   NULL  - Specific device path not found

**/

EFI_DEVICE_PATH_PROTOCOL
*SearchDevicePath (
  IN    OUT     EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN    UINT8   Type,
  IN    UINT8   SubType
)
{
    if (DevicePath == NULL) {
        return NULL;
    }

    while (!IsDevicePathEnd (DevicePath)) {
        if ((DevicePathType (DevicePath) == Type) && (DevicePathSubType (DevicePath) == SubType)) {
            return DevicePath;
        }
        DevicePath = NextDevicePathNode (DevicePath);
    }

    return NULL;
}

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
/**
    Checks PM Port

    @param    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    @param    IN  UINT16                      Port,
    @param    IN  UINT16                      PortMultiplierPort

    @retval   SATA_DEVICE_INTERFACE

**/

SATA_DEVICE_INTERFACE
*GetSataDevInterfaceThruPortPMPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  UINT16                      Port,
    IN  UINT16                      PortMultiplierPort
)
{
    SATA_DEVICE_INTERFACE   *SataDevInterface = NULL;
    SATA_DEVICE_INTERFACE   *pSataDevInterface = NULL;
    UINT16                  DevCount = 0;
    UINTN                   Index = 0;

    pSataDevInterface = (SATA_DEVICE_INTERFACE *)((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceBuffer;
    if(pSataDevInterface == NULL) return NULL;

    DevCount = ((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceCount;

    // Returns the Device Interface of Corresponding Port/PM
    for(Index = 0; Index < DevCount; Index++){
        SataDevInterface = ((SATA_DEVICE_INTERFACE **)pSataDevInterface)[Index];

        if((UINT8)Port == SataDevInterface->PortNumber) {
            if((UINT8)PortMultiplierPort == SataDevInterface->PMPortNumber) {
                return SataDevInterface;
            }
        }
    }

    return NULL;
}

/**
    Returns  Total drives present and their respective device addresses

    @param    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    @param    OUT SATA_DEVICE_INTERFACE       **SataAddressBuffer,
    @param    OUT UINT16                      *NumberOfDevices

    @retval   EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
GetSataDevCountandAddressBuffer (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    OUT SATA_DEVICE_INTERFACE       **SataAddressBuffer,
    OUT UINT16                      *NumberOfDevices
)
{
    EFI_STATUS              Status  = 0;
    UINT16                  DevCount = 0;
    SATA_DEVICE_INTERFACE   *Devices = NULL;
    EFI_HANDLE              *ControllerHandle = ((ATA_PASS_THRU_PROTOCOL*)This)->ControllerHandle;

    DevCount = CountAhciDrives(ControllerHandle, NULL);
    if(-1 == DevCount){
        return EFI_NOT_FOUND;
    }
    // For the 1st time CountAhciDrives was called to get the number of connected
    // drives; For the 2nd time it will actually return the device information
    if (DevCount == 0)  {
        *SataAddressBuffer = NULL;
        *NumberOfDevices = 0;
        return EFI_SUCCESS;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(SATA_DEVICE_INTERFACE*) * DevCount, (VOID **)&Devices);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    *SataAddressBuffer = Devices;
    CountAhciDrives(ControllerHandle, (SATA_DEVICE_INTERFACE**)Devices);
    *NumberOfDevices = DevCount;

    return EFI_SUCCESS;
}

/**
    This routine returns the number of connected AHCI drives as well
    as it fills the optional SATA_DEVICE_INTERFACE* buffer with the
    device information

    @param  IN  EFI_HANDLE                  *HandleBuffer,
    @param  IN  OUT SATA_DEVICE_INTERFACE   **Devices

    @retval UINT16 DevCount

**/

UINT16
CountAhciDrives (
    IN  EFI_HANDLE                  *HandleBuffer,
    IN  OUT SATA_DEVICE_INTERFACE   **Devices
){
    EFI_STATUS              Status;
    UINT16                  DevCount = 0;
    AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface;
    SATA_DEVICE_INTERFACE   *SataDevInterface;
    DLINK                   *dlink;

    // With the Controller handle, find the total devices present and their Address
    Status = pBS->HandleProtocol(HandleBuffer, &gAmiAhciBusProtocolGuid, (VOID **)&AhciBusInterface);
    if(EFI_ERROR(Status)){
        return -1;
    }

    for (dlink = AhciBusInterface->SataDeviceList.pHead; dlink; dlink = dlink->pNext) {
        SataDevInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);

#if ATAPI_COMMANDS_SUPPORT_IN_ATAPASSTHRU == 1
        if (SataDevInterface->DeviceState == DEVICE_CONFIGURED_SUCCESSFULLY ) {
#else
        if (SataDevInterface->DeviceState == DEVICE_CONFIGURED_SUCCESSFULLY &&  SataDevInterface->DeviceType == ATA) {
#endif
            if (Devices != NULL) {
                Devices[DevCount] = SataDevInterface;
            }
            DevCount++;
        }
    }

    return DevCount;
}
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
/**
    Checks PM Port

    @param  IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    @param  IN  UINT16                      Port,
    @param  IN  UINT16                      PortMultiplierPort

    @retval AMI_IDE_BUS_PROTOCOL

**/

AMI_IDE_BUS_PROTOCOL
*GetIdeBusInterfaceThruPortPMPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  UINT16                      Port,
    IN  UINT16                      PortMultiplierPort
  )
{
    AMI_IDE_BUS_PROTOCOL    *IdeBusInterface = NULL;
    AMI_IDE_BUS_PROTOCOL    *pIdeBusInterface = NULL;
    UINT16                  DevCount = 0;
    UINTN                   Index = 0;

    pIdeBusInterface = (AMI_IDE_BUS_PROTOCOL *)((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceBuffer;
    if(pIdeBusInterface == NULL) return NULL;

    DevCount = ((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceCount;

    // Run through the total number of drives present
    for(Index = 0; Index < DevCount; Index++){
        IdeBusInterface = ((AMI_IDE_BUS_PROTOCOL **)pIdeBusInterface)[Index];
         if((UINT8)Port == IdeBusInterface->IdeDevice.Channel) {
            if((UINT8)PortMultiplierPort == IdeBusInterface->IdeDevice.Device) {
                return IdeBusInterface;
            }
        }
    }

    return NULL;
}

/**
    Returns  Total drives present and their respective device addresses.

    @param  IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    @param  OUT AMI_IDE_BUS_PROTOCOL        **IdeAddressBuffer,
    @param  OUT UINT16                      *NumberOfDevices

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
GetIdeDevCountandAddressBuffer (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    OUT AMI_IDE_BUS_PROTOCOL        **IdeAddressBuffer,
    OUT UINT16                      *NumberOfDevices
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT16                  DevCount = 0;
    AMI_IDE_BUS_PROTOCOL    *Devices = NULL;
    EFI_HANDLE              *ControllerHandle = ((ATA_PASS_THRU_PROTOCOL*)This)->ControllerHandle;

    DevCount = CountIdeDrives(ControllerHandle, NULL);
    if(-1 == DevCount){
        return EFI_NOT_FOUND;
    }
    // For the 1st time CountIdeDrives was called to get the number of connected
    // drives; for the 2nd time it will actually return the device information
    if (DevCount == 0)  {
        *IdeAddressBuffer = NULL;
        *NumberOfDevices = 0;
        return EFI_SUCCESS;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(AMI_IDE_BUS_PROTOCOL*) * DevCount, (VOID **)&Devices);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    *IdeAddressBuffer = Devices;
    CountIdeDrives(ControllerHandle, (AMI_IDE_BUS_PROTOCOL**)Devices);
    *NumberOfDevices = DevCount;

    return EFI_SUCCESS;
}

/**
    This routine returns the number of connected IDE drives as well
    as it fills the optional AMI_IDE_BUS_PROTOCOL buffer with the
    device information.

    @param  IN  EFI_HANDLE              *HandleBuffer,
    @param  IN  AMI_IDE_BUS_PROTOCOL    **Devices

    @retval UINT16 DevCount

**/

UINT16
CountIdeDrives (
    IN  EFI_HANDLE              *HandleBuffer,
    IN  AMI_IDE_BUS_PROTOCOL    **Devices
){
    EFI_STATUS                  Status;
    UINT8                       PrimarySecondary;
    UINT8                       MasterSlave;
    UINT16                      DevCount = 0;
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = NULL;
    AMI_IDE_BUS_INIT_PROTOCOL   *IdeBusInitProtocol;

    // With the Controller handle, find the total devices present and their Address
    Status = pBS->HandleProtocol(HandleBuffer,&gAmiIdeBusInitProtocolGuid,(VOID **)&IdeBusInitProtocol);
    if(EFI_ERROR(Status)){
        return -1;
    }
    for (PrimarySecondary = 0;PrimarySecondary < 2;PrimarySecondary++){
        for (MasterSlave = 0;MasterSlave < 2;MasterSlave++) {
            if (IdeBusInitProtocol->IdeBusInitData[PrimarySecondary][MasterSlave] == DEVICE_CONFIGURED_SUCCESSFULLY) {

                IdeBusInterface = IdeBusInitProtocol->pIdeBusProtocol[PrimarySecondary][MasterSlave];
#if ATAPI_COMMANDS_SUPPORT_IN_ATAPASSTHRU == 0
                if (IdeBusInterface->IdeDevice.DeviceType == ATAPI) continue;
#endif
                if (Devices != NULL) {
                    Devices[DevCount] = IdeBusInterface;
                }

                DevCount++;
            }
        }
    }

    return DevCount;
}

/**
    Reads 1 Byte of data from the IO port

    @param  IN  EFI_PCI_IO_PROTOCOL     *PciIO,
    @param  IN  UINT16                  Register,
    @param  OUT UINT8                   *Data8

    @retval OUT UINT8   *Data8

**/

EFI_STATUS
IdeReadByte(
    IN  EFI_PCI_IO_PROTOCOL     *PciIO,
    IN  UINT16                  Register,
    OUT UINT8                   *Data8
) {
    PciIO->Io.Read(
        PciIO,
        EfiPciIoWidthFifoUint8,
        EFI_PCI_IO_PASS_THROUGH_BAR,
        Register,
        1,
        Data8 );

    return EFI_SUCCESS;
}

/**
    Get the ATA Status Block

    @param  IN  AMI_IDE_BUS_PROTOCOL        *IdeBusInterface,
    @param  IN  OUT EFI_ATA_STATUS_BLOCK    *Asb

    @retval None

**/

VOID
GetIdeAsbStatus (
    IN  AMI_IDE_BUS_PROTOCOL        *IdeBusInterface,
    IN  OUT EFI_ATA_STATUS_BLOCK    *Asb
  )
{
    IO_REGS     Regs = IdeBusInterface->IdeDevice.Regs;

    ZeroMemory (Asb, sizeof (EFI_ATA_STATUS_BLOCK));

    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.StatusReg,       &Asb->AtaStatus);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.ErrorReg,        &Asb->AtaError);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.SectorCountReg,  &Asb->AtaSectorCount);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.SectorCountReg,  &Asb->AtaSectorCountExp);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.LBALowReg,       &Asb->AtaSectorNumber);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.LBALowReg,       &Asb->AtaSectorNumberExp);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.LBAMidReg,       &Asb->AtaCylinderLow);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.LBAMidReg,       &Asb->AtaCylinderLowExp);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.LBAHighReg,      &Asb->AtaCylinderHigh);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.LBAHighReg,      &Asb->AtaCylinderHighExp);
    IdeReadByte( IdeBusInterface->PciIO, Regs.CommandBlock.DeviceReg,       &Asb->AtaDeviceHead);

    return;
}
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
/**
    Get the ATA Status Block in AHCI

    @param  IN    SATA_DEVICE_INTERFACE       *SataDevInterface,
    @param  IN    OUT EFI_ATA_STATUS_BLOCK    *Asb

    @retval EFI_STATUS

**/

VOID
GetAhciAsbStatus (
  IN    SATA_DEVICE_INTERFACE       *SataDevInterface,
  IN    OUT EFI_ATA_STATUS_BLOCK    *Asb
) {

    UINT32  Data = 0;
    volatile    AHCI_RECEIVED_FIS *FISReceiveAddress;

    UINT64    AhciBaseAddr = SataDevInterface->AhciBusInterface->AhciBaseAddress;

    Data = HBA_PORT_REG32 (AhciBaseAddr, SataDevInterface->PortNumber, HBA_PORTS_TFD);
    if (Asb != NULL) {
        ZeroMemory (Asb, sizeof (EFI_ATA_STATUS_BLOCK));

        Asb->AtaStatus  = (UINT8)Data;
        if ((Asb->AtaStatus & BIT0) != 0) {
            Asb->AtaError = (UINT8)(Data >> 8);
        }

        FISReceiveAddress = (AHCI_RECEIVED_FIS*)SataDevInterface->PortFISBaseAddr;

        // Get the pointer to RFIS
        if (FISReceiveAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H) {
            Asb->AtaStatus          = FISReceiveAddress->Ahci_Rfis[2];
            Asb->AtaError           = FISReceiveAddress->Ahci_Rfis[3];
            Asb->AtaSectorCount     = FISReceiveAddress->Ahci_Rfis[12];
            Asb->AtaSectorCountExp  = FISReceiveAddress->Ahci_Rfis[13];
            Asb->AtaSectorNumber    = FISReceiveAddress->Ahci_Rfis[4];
            Asb->AtaSectorNumberExp = FISReceiveAddress->Ahci_Rfis[8];
            Asb->AtaCylinderLow     = FISReceiveAddress->Ahci_Rfis[5];
            Asb->AtaCylinderLowExp  = FISReceiveAddress->Ahci_Rfis[9];
            Asb->AtaCylinderHigh    = FISReceiveAddress->Ahci_Rfis[6];
            Asb->AtaCylinderHighExp = FISReceiveAddress->Ahci_Rfis[10];
            Asb->AtaDeviceHead      = FISReceiveAddress->Ahci_Rfis[7];
            return;
        }

        // Get the pointer to PFIS
        if (FISReceiveAddress->Ahci_Psfis[0] == FIS_PIO_SETUP) {
            Asb->AtaStatus          = FISReceiveAddress->Ahci_Psfis[2];
            Asb->AtaError           = FISReceiveAddress->Ahci_Psfis[3];
            Asb->AtaSectorCount     = FISReceiveAddress->Ahci_Psfis[12];
            Asb->AtaSectorCountExp  = FISReceiveAddress->Ahci_Psfis[13];
            Asb->AtaSectorNumber    = FISReceiveAddress->Ahci_Psfis[4];
            Asb->AtaSectorNumberExp = FISReceiveAddress->Ahci_Psfis[8];
            Asb->AtaCylinderLow     = FISReceiveAddress->Ahci_Psfis[05];
            Asb->AtaCylinderLowExp  = FISReceiveAddress->Ahci_Psfis[9];
            Asb->AtaCylinderHigh    = FISReceiveAddress->Ahci_Psfis[6];
            Asb->AtaCylinderHighExp = FISReceiveAddress->Ahci_Psfis[10];
            Asb->AtaDeviceHead      = FISReceiveAddress->Ahci_Psfis[7];
            return;
        }
    }
    return;
}
#endif

/**
    Sends an ATA command to an ATA device that is attached
    to the ATA controller

    @param  IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    @param  IN  UINT16                      Port,
    @param  IN  UINT16                      PortMultiplierPort,
    @param  IN  OUT                         EFI_ATA_PASS_THRU_COMMAND_PACKET *Packet,
    @param  IN  EFI_EVENT                   Event OPTIONAL

    @retval EFI_STATUS

    @note   A pointer to the ATA command to send to the ATA device specified by Port
        and PortMultiplierPort.
**/

EFI_STATUS
PassThru (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  UINT16                      Port,
    IN  UINT16                      PortMultiplierPort,
    IN  OUT                         EFI_ATA_PASS_THRU_COMMAND_PACKET *Packet,
    IN  EFI_EVENT                   Event OPTIONAL
)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE           *SataDevInterface = NULL;
    AMI_AHCI_BUS_PROTOCOL           *AhciBusInterface = NULL;
    COMMAND_STRUCTURE               CommandStructure;
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL            *IdeBusInterface = NULL;
#endif
    EFI_ATA_PASS_THRU_PROTOCOL      *PassThruProtocol = This;
    BOOLEAN                         READWRITE;
    UINT32                          AlignmentBoundry;
    UINT32                          MaxSectorCount;
    BOOLEAN                         ModeFlag = ((ATA_PASS_THRU_PROTOCOL*)This)->ModeFlag;
    UINT16                          IdentifyDataWord83 = 0;
    #if (( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) ) || ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) ))
    UINT64                          IdentifyData_LBA_48;
    #endif
    UINT32                          SectorSize = ATA_SECTOR_BYTES;

    //Check Command is for read or for write
    READWRITE = (Packet->InDataBuffer) ? 0  :  1 ;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
#endif

    // Check InDataBuffer,OutDataBuffer are properly aligned or  not
    if(PassThruProtocol->Mode->IoAlign>1){
        // Should be align in 2's power
        AlignmentBoundry = 1 << PassThruProtocol->Mode->IoAlign;
        //Get what is the number in 2 to the power Mode->IoAlign
        if((0 !=  ((UINTN)(READWRITE ? Packet->OutDataBuffer:Packet->InDataBuffer) % AlignmentBoundry)) ||
           (0 !=  ((UINTN)(READWRITE ? Packet->OutTransferLength:Packet->InTransferLength) % AlignmentBoundry)) ||
           (0 != ((UINTN)Packet->Acb  % AlignmentBoundry)) || (0 != ((UINTN)Packet->Asb  % AlignmentBoundry))) {
            return EFI_INVALID_PARAMETER;
        }
    }

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if(ModeFlag){
        //If Port or PortMultiplierPort are not in a valid range for the ATA
        //controller, then EFI_INVALID_PARAMETER is returned
        SataDevInterface = GetSataDevInterfaceThruPortPMPort(This,Port,PortMultiplierPort);
        if( NULL == SataDevInterface){
            return EFI_INVALID_PARAMETER;
        }
        AhciBusInterface=SataDevInterface->AhciBusInterface;

        if( NULL == AhciBusInterface){
            return EFI_INVALID_PARAMETER;
        }

        IdentifyDataWord83 = SataDevInterface->IdentifyData.Command_Set_Supported_83;
        IdentifyData_LBA_48 = SataDevInterface->IdentifyData.LBA_48;

        if((SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT14) &&       // WORD 106 valid? - BIT 14 - 1
            (!(SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT15)) &&   // WORD 106 valid? - BIT 15 - 0
            (SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT12)) {      // WORD 106 bit 12 - SectorSize > 256 words
            // The sector size is in words 117-118.
            SectorSize = (UINT32)(SataDevInterface->IdentifyData.Reserved_104_126[13] +
                                  (SataDevInterface->IdentifyData.Reserved_104_126[14] << 16)) * 2;
        }
    }
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if(!ModeFlag){

        IdeBusInterface = GetIdeBusInterfaceThruPortPMPort(This,Port,PortMultiplierPort);
        if(NULL == IdeBusInterface){
            return  EFI_INVALID_PARAMETER;
        }

        IdentifyDataWord83 = IdeBusInterface->IdeDevice.IdentifyData.Command_Set_Supported_83;
        IdentifyData_LBA_48 = IdeBusInterface->IdeDevice.IdentifyData.LBA_48;

        if((IdeBusInterface->IdeDevice.IdentifyData.Reserved_104_126[2] & BIT14) &&     // WORD 106 valid? - BIT 14 - 1
           (!(IdeBusInterface->IdeDevice.IdentifyData.Reserved_104_126[2] & BIT15)) &&  // WORD 106 valid? - BIT 15 - 0
           (IdeBusInterface->IdeDevice.IdentifyData.Reserved_104_126[2] & BIT12)) {     // WORD 106 bit 12 - SectorSize > 256 words
            // The sector size is in words 117-118.
            SectorSize = (UINT32)(IdeBusInterface->IdeDevice.IdentifyData.Reserved_104_126[13] +
                                  (IdeBusInterface->IdeDevice.IdentifyData.Reserved_104_126[14] << 16)) * 2;
        }
    }
#endif

    // Transfer Length from sector count to byte.
    if (((Packet->Length & EFI_ATA_PASS_THRU_LENGTH_BYTES) == 0) &&
      (Packet->InTransferLength != 0)) {
        Packet->InTransferLength = Packet->InTransferLength * SectorSize;
    }

    // Transfer Length from sector count to byte.
    if (((Packet->Length & EFI_ATA_PASS_THRU_LENGTH_BYTES) == 0) &&
     (Packet->OutTransferLength != 0)) {
        Packet->OutTransferLength = Packet->OutTransferLength * SectorSize;
    }

    // If, word83: bit15 is zero and bit14 is one and bit10 is one, then ATA device support 48-bit addressing.
    MaxSectorCount = 0x100;
    if ((IdentifyDataWord83 & (BIT10 | BIT15 | BIT14)) == 0x4400) {
        if (IdentifyData_LBA_48 > 0xFFFFFFF) {
            // Capacity exceeds 120GB. 48-bit addressing is really needed
            // In this case, the max sector count is 0x10000
            MaxSectorCount = 0x10000;
        }
    }

    // If the data buffer described by InDataBuffer and InTransferLength is too big to be transferred in a single command,
    // then no data is transferred and EFI_BAD_BUFFER_SIZE is returned. The number of bytes that can be transferred in a
    // single command are returned in InTransferLength. If the data buffer described by OutDataBuffer and OutTransferLength
    // is too big to be transferred in a single command, then no data is transferred and EFI_BAD_BUFFER_SIZE is returned.
    if (((Packet->InTransferLength != 0) && (Packet->InTransferLength > MaxSectorCount * SectorSize)) ||
        ((Packet->OutTransferLength != 0) && (Packet->OutTransferLength > MaxSectorCount * SectorSize))) {
        Packet->InTransferLength = MaxSectorCount * SectorSize;
        return EFI_BAD_BUFFER_SIZE;
    }

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    //  Frame Command Buffer
    CommandStructure.SectorCount    = (UINT16)Packet->Acb->AtaSectorCount;
    CommandStructure.LBALow         = Packet->Acb->AtaSectorNumber;
    CommandStructure.LBAMid         = Packet->Acb->AtaCylinderLow;
    CommandStructure.LBAHigh        = Packet->Acb->AtaCylinderHigh;
    CommandStructure.Command        = Packet->Acb->AtaCommand;
    CommandStructure.Features       = Packet->Acb->AtaFeatures;
	
	if(Packet->Timeout == 0) {
	    CommandStructure.Timeout        = -1; // Setting max value of Timeout 
	} else {
	    if(Packet->Timeout < 10000) {
	        CommandStructure.Timeout = 1;
	    } else {
			CommandStructure.Timeout        = DivU64x32(Packet->Timeout, 10000); // Converting to milli Seconds
	    }	
	}


    // By default initialize with In data. Based on the PassThru protocol
    // these fields will be initialized again.
    CommandStructure.Buffer         = Packet->InDataBuffer;
    CommandStructure.ByteCount      = Packet->InTransferLength;

    if (Check48BitCommand (Packet->Acb->AtaCommand)) {
        CommandStructure.LBALowExp = Packet->Acb->AtaSectorNumberExp;
        CommandStructure.LBAMidExp = Packet->Acb->AtaCylinderLowExp;
        CommandStructure.LBAHighExp = Packet->Acb->AtaCylinderHighExp;
        CommandStructure.Device = 0x40;
    } else {
        CommandStructure.Device = ((UINT8) (Packet->Acb->AtaSectorNumberExp & 0x0f) | 0x40);
    }

    if(ModeFlag) {
        switch(Packet->Protocol){

            case  EFI_ATA_PASS_THRU_PROTOCOL_ATA_HARDWARE_RESET:
                    Status = AhciBusInterface->GeneratePortReset ( AhciBusInterface,
                                                                SataDevInterface,
                                                                (UINT8)Port,
                                                                (UINT8)PortMultiplierPort,
                                                                0,
                                                                0
                                                                );

                    break;

            case  EFI_ATA_PASS_THRU_PROTOCOL_ATA_SOFTWARE_RESET:
                    Status = AhciBusInterface->GeneratePortSoftReset ( SataDevInterface,
                                                                    (UINT8)PortMultiplierPort
                                                                    );

                    break;

            case  EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA:

                    Status = AhciBusInterface->ExecuteNonDataCommand (  SataDevInterface,
                                                                        CommandStructure
                                                                        );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN:
                
                   
                    Status = AhciBusInterface->SataPioDataOut(  SataDevInterface,
                                                                   CommandStructure,
                                                                   FALSE
                                                                );
                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT:
                    CommandStructure.Buffer         = Packet->OutDataBuffer;
                    CommandStructure.ByteCount      = Packet->OutTransferLength;
                    
                    Status = AhciBusInterface->SataPioDataOut(  SataDevInterface,
                                                                CommandStructure,
                                                                TRUE
                                                                );
                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_UDMA_DATA_IN:

                    Status = AhciBusInterface->ExecuteDmaDataCommand ( SataDevInterface,
                                                                    &CommandStructure,
                                                                    FALSE
                                                                    );
                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_UDMA_DATA_OUT:

                    // Initialize the Out Command Data
                    CommandStructure.Buffer         = Packet->OutDataBuffer;
                    CommandStructure.ByteCount      = Packet->OutTransferLength;

                    Status = AhciBusInterface->ExecuteDmaDataCommand ( SataDevInterface,
                                                                    &CommandStructure,
                                                                    TRUE
                                                                    );
                    break;
            case EFI_ATA_PASS_THRU_PROTOCOL_DMA:

                    if(Packet->InTransferLength == 0) {
                        // Initialize the Out Command Data
                        CommandStructure.Buffer         = Packet->OutDataBuffer;
                        CommandStructure.ByteCount      = Packet->OutTransferLength;
                    }

                    //For all write and non data commands where InTransferLength is 0
                    Status = AhciBusInterface->ExecuteDmaDataCommand ( SataDevInterface,
                                                                    &CommandStructure,
                                                                    Packet->InTransferLength == 0 ? TRUE : FALSE
                                                                    );
                    break;
            case EFI_ATA_PASS_THRU_PROTOCOL_DEVICE_DIAGNOSTIC:

                    // Send Device Diagnostic command.
                    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
                    CommandStructure.Command = EXECUTE_DEVICE_DIAGNOSTIC;
                    Status = AhciBusInterface->ExecuteNonDataCommand (  SataDevInterface,
                                                                        CommandStructure
                                                                        );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_DEVICE_RESET:

                    // Device Reset is not supported for ATA devices.
                    if(SataDevInterface->DeviceType == ATA) {
                        return EFI_UNSUPPORTED;
                    }

                    // Send Device Reset command.
                    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
                    CommandStructure.Command = DEVICE_RESET;
                    Status = AhciBusInterface->ExecuteNonDataCommand (  SataDevInterface,
                                                                        CommandStructure
                                                                        );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_RETURN_RESPONSE:
                    GetAhciAsbStatus(SataDevInterface, Packet->Asb);
                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_DMA_QUEUED:
            case EFI_ATA_PASS_THRU_PROTOCOL_FPDMA:

            default:
                   return EFI_UNSUPPORTED;
        }
        
        GetAhciAsbStatus(SataDevInterface, Packet->Asb);
        CommandStructure.Timeout = 0;

    }
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if(!ModeFlag){
        
        Status = pBS->LocateProtocol(
                                    &gAmiPlatformIdeProtocolGuid,
                                    NULL,
                                    (void**)&gPlatformIdeProtocol
                                    );
        if (EFI_ERROR (Status)) {
            // If Error Status, Set Command timeout to default sdl value.
            gPlatformIdeProtocol->CommandTimeout = 0;
        } else {
        	if(Packet->Timeout == 0) {
		        gPlatformIdeProtocol->CommandTimeout = -1;  // Setting max value of Timeout 
			} else {
			    
		        if(Packet->Timeout < 10000) {
		            gPlatformIdeProtocol->CommandTimeout = 1;
		        } else {
	                gPlatformIdeProtocol->CommandTimeout = DivU64x32(Packet->Timeout, 10000); // Converting to milli Seconds
		        }   
			}
		}	

        switch(Packet->Protocol){

            case EFI_ATA_PASS_THRU_PROTOCOL_ATA_HARDWARE_RESET:
                    Status = IdeBusInterface->IdeSoftReset( IdeBusInterface
                                                            );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_ATA_SOFTWARE_RESET:

                    return EFI_UNSUPPORTED;

            case  EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA:

                    Status = IdeBusInterface->IdeNonDataCommand(  IdeBusInterface,
                                                                Packet->Acb->AtaFeatures,
                                                                Packet->Acb->AtaSectorCount,
                                                                Packet->Acb->AtaSectorCountExp,
                                                                Packet->Acb->AtaSectorNumber,
                                                                Packet->Acb->AtaSectorNumberExp,
                                                                Packet->Acb->AtaCylinderLow,
                                                                Packet->Acb->AtaCylinderLowExp,
                                                                Packet->Acb->AtaCylinderHigh,
                                                                Packet->Acb->AtaCylinderHighExp,
                                                                Packet->Acb->AtaDeviceHead,
                                                                Packet->Acb->AtaCommand
                                                             );
                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN:

                    Status = IdeBusInterface->AtaPioDataOut ( IdeBusInterface,
                                                            Packet->InDataBuffer,
                                                            Packet->InTransferLength,
                                                            Packet->Acb->AtaFeatures,
                                                            (UINT8)Packet->Acb->AtaSectorCount,
                                                            Packet->Acb->AtaSectorNumber,
                                                            Packet->Acb->AtaSectorNumberExp,
                                                            Packet->Acb->AtaCylinderLow,
                                                            Packet->Acb->AtaCylinderLowExp,
                                                            Packet->Acb->AtaCylinderHigh,
                                                            Packet->Acb->AtaCylinderHighExp,
                                                            Packet->Acb->AtaDeviceHead,
                                                            Packet->Acb->AtaCommand,
                                                            FALSE,
                                                            FALSE
                                                          );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT:

                    Status = IdeBusInterface->AtaPioDataOut ( IdeBusInterface,
                                                            Packet->OutDataBuffer,
                                                            Packet->OutTransferLength,
                                                            Packet->Acb->AtaFeatures,
                                                            (UINT8)Packet->Acb->AtaSectorCount,
                                                            Packet->Acb->AtaSectorNumber,
                                                            Packet->Acb->AtaSectorNumberExp,
                                                            Packet->Acb->AtaCylinderLow,
                                                            Packet->Acb->AtaCylinderLowExp,
                                                            Packet->Acb->AtaCylinderHigh,
                                                            Packet->Acb->AtaCylinderHighExp,
                                                            Packet->Acb->AtaDeviceHead,
                                                            Packet->Acb->AtaCommand,
                                                            TRUE,
                                                            FALSE
                                                          );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_UDMA_DATA_IN:

                    Status = IdeBusInterface->AtaAtapiDmaDataCommand ( IdeBusInterface,
                                                            Packet->InDataBuffer,
                                                            Packet->InTransferLength,
                                                            Packet->Acb->AtaFeatures,
                                                            (UINT32)Packet->Acb->AtaSectorCount,
                                                            Packet->Acb->AtaSectorNumber,
                                                            Packet->Acb->AtaSectorNumberExp,
                                                            Packet->Acb->AtaCylinderLow,
                                                            Packet->Acb->AtaCylinderLowExp,
                                                            Packet->Acb->AtaCylinderHigh,
                                                            Packet->Acb->AtaCylinderHighExp,
                                                            Packet->Acb->AtaDeviceHead,
                                                            Packet->Acb->AtaCommand,
                                                            FALSE
                                                          );
                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_UDMA_DATA_OUT:

                    Status = IdeBusInterface->AtaAtapiDmaDataCommand ( IdeBusInterface,
                                                            Packet->OutDataBuffer,
                                                            Packet->OutTransferLength,
                                                            Packet->Acb->AtaFeatures,
                                                            (UINT32)Packet->Acb->AtaSectorCount,
                                                            Packet->Acb->AtaSectorNumber,
                                                            Packet->Acb->AtaSectorNumberExp,
                                                            Packet->Acb->AtaCylinderLow,
                                                            Packet->Acb->AtaCylinderLowExp,
                                                            Packet->Acb->AtaCylinderHigh,
                                                            Packet->Acb->AtaCylinderHighExp,
                                                            Packet->Acb->AtaDeviceHead,
                                                            Packet->Acb->AtaCommand,
                                                            TRUE
                                                          );
                    break;


            case EFI_ATA_PASS_THRU_PROTOCOL_DMA:
                    if(Packet->InTransferLength == 0) {

                        Status = IdeBusInterface->AtaAtapiDmaDataCommand ( IdeBusInterface,
                                                            Packet->OutDataBuffer,
                                                            Packet->OutTransferLength,
                                                            Packet->Acb->AtaFeatures,
                                                            (UINT32)Packet->Acb->AtaSectorCount,
                                                            Packet->Acb->AtaSectorNumber,
                                                            Packet->Acb->AtaSectorNumberExp,
                                                            Packet->Acb->AtaCylinderLow,
                                                            Packet->Acb->AtaCylinderLowExp,
                                                            Packet->Acb->AtaCylinderHigh,
                                                            Packet->Acb->AtaCylinderHighExp,
                                                            Packet->Acb->AtaDeviceHead,
                                                            Packet->Acb->AtaCommand,
                                                            TRUE
                                                          );


                    } else {

                        Status = IdeBusInterface->AtaAtapiDmaDataCommand ( IdeBusInterface,
                                                            Packet->InDataBuffer,
                                                            Packet->InTransferLength,
                                                            Packet->Acb->AtaFeatures,
                                                            (UINT32)Packet->Acb->AtaSectorCount,
                                                            Packet->Acb->AtaSectorNumber,
                                                            Packet->Acb->AtaSectorNumberExp,
                                                            Packet->Acb->AtaCylinderLow,
                                                            Packet->Acb->AtaCylinderLowExp,
                                                            Packet->Acb->AtaCylinderHigh,
                                                            Packet->Acb->AtaCylinderHighExp,
                                                            Packet->Acb->AtaDeviceHead,
                                                            Packet->Acb->AtaCommand,
                                                            FALSE
                                                          );
                    }

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_DEVICE_DIAGNOSTIC:

                    Status = IdeBusInterface->IdeNonDataCommand(  IdeBusInterface,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                EXECUTE_DEVICE_DIAGNOSTIC
                                                             );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_DEVICE_RESET:

                    if(IdeBusInterface->IdeDevice.DeviceType == ATA) {
                        return EFI_UNSUPPORTED;
                    }

                    Status = IdeBusInterface->IdeNonDataCommand(  IdeBusInterface,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                0,
                                                                DEVICE_RESET
                                                             );

                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_RETURN_RESPONSE:
                    GetIdeAsbStatus(IdeBusInterface, Packet->Asb);
                    break;

            case EFI_ATA_PASS_THRU_PROTOCOL_DMA_QUEUED:
            case EFI_ATA_PASS_THRU_PROTOCOL_FPDMA:

            default:
                   return EFI_UNSUPPORTED;
        }

        GetIdeAsbStatus(IdeBusInterface, Packet->Asb);
        gPlatformIdeProtocol->CommandTimeout = 0;
    }
#endif
   return Status;

}

/**
    Retrieves the list of port numbers for ATA devices on the
    ATA controller.

    @param  IN EFI_ATA_PASS_THRU_PROTOCOL   *This,
    @param  IN OUT UINT16   *Port

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
GetNextPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  OUT UINT16                  *Port
)
{
    UINT16                  DevCount = 0;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE   *SataDevices = NULL;
    SATA_DEVICE_INTERFACE   *pSataDevices = NULL;
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL    *IdeDevices = NULL;
    AMI_IDE_BUS_PROTOCOL    *pIdeDevices = NULL;
#endif
    UINT8                   DevIndex = 0;
    UINT16                  PreviousPort = 0;
    BOOLEAN                 ModeFlag = ((ATA_PASS_THRU_PROTOCOL*)This)->ModeFlag;

    // Port is NULL return Invalid Parameter.
    if(Port == NULL){
        return EFI_INVALID_PARAMETER;
    }

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if(ModeFlag) {

        pSataDevices = (SATA_DEVICE_INTERFACE *)((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceBuffer;
        if(pSataDevices == NULL) return EFI_NOT_FOUND;

        DevCount = ((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceCount;

        SataDevices = ((SATA_DEVICE_INTERFACE **)pSataDevices)[DevIndex];
        if((SataDevices == NULL) || (DevCount == 0)) {
            return EFI_NOT_FOUND;
        }

        PreviousPort = SataDevices->AhciBusInterface->PrevPortNum;
        // If Port is not 0xFFFF or Previous Port Number returned on earlier call
        // return invalid parameter
        if(( 0xFFFF != *Port) && (PreviousPort != *Port)) {
            return EFI_INVALID_PARAMETER;
        }
        // If on input Port is 0xFFFF, then the port number of the first port on the
        // ATA controller is returned in Port and EFI_SUCCESS is returned.
        if( 0xFFFF == *Port){
            SataDevices->AhciBusInterface->PrevPortNum = SataDevices->PortNumber;
            *Port = SataDevices->PortNumber;
            return EFI_SUCCESS;
        }

        // If Port is a port number that was returned on a previous call to GetNextPort(), then
        // the port number of the next port on the ATA controller is returned in Port, and
        // EFI_SUCCESS is returned.
        if( PreviousPort == *Port){
            for(DevIndex = 0; DevIndex < DevCount; DevIndex++){
                SataDevices = ((SATA_DEVICE_INTERFACE **)pSataDevices)[DevIndex];
                if(SataDevices->PortNumber > *Port) {
                    SataDevices->AhciBusInterface->PrevPortNum = SataDevices->PortNumber;
                    *Port = SataDevices->PortNumber;
                    return EFI_SUCCESS;
                }
            }
        }

        return EFI_NOT_FOUND;

    }
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if(!ModeFlag){

        pIdeDevices = (AMI_IDE_BUS_PROTOCOL *)((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceBuffer;
        if(pIdeDevices == NULL) return EFI_NOT_FOUND;

        DevCount = ((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceCount;

        IdeDevices = ((AMI_IDE_BUS_PROTOCOL **)pIdeDevices)[DevIndex];
        if((IdeDevices == NULL) || (DevCount == 0)) {
            return EFI_NOT_FOUND;
        }

        PreviousPort = IdeDevices->IdeBusInitInterface->PrevPortNum;
        // If Port is not 0xFFFF or Previous Port Number returned on earlier call
        // return invalid parameter
        if(( 0xFFFF != *Port) && (PreviousPort != *Port)) {
            return EFI_INVALID_PARAMETER;
        }

        // If on input Port is 0xFFFF, then the port number of the first port on the
        // ATA controller is returned in Port and EFI_SUCCESS is returned.
        if( 0xFFFF == *Port){
            IdeDevices->IdeBusInitInterface->PrevPortNum = IdeDevices->IdeDevice.Channel;
            *Port = IdeDevices->IdeDevice.Channel;
            return EFI_SUCCESS;
        }

        // If Port is a port number that was returned on a previous call to GetNextPort(), then
        // the port number of the next port on the ATA controller is returned in Port, and
        // EFI_SUCCESS is returned.
        if( PreviousPort == *Port){
            for(DevIndex = 0; DevIndex < DevCount; DevIndex++){
                IdeDevices = ((AMI_IDE_BUS_PROTOCOL **)pIdeDevices)[DevIndex];
                if(IdeDevices->IdeDevice.Channel > *Port) {
                    IdeDevices->IdeBusInitInterface->PrevPortNum = IdeDevices->IdeDevice.Channel;
                    *Port = IdeDevices->IdeDevice.Channel;
                    return EFI_SUCCESS;
                }
            }
        }

        return EFI_NOT_FOUND;
    }
#endif

    return EFI_SUCCESS;
}

/**
    This function  Retrieves the list of legal port multiplier port
    numbers for ATA devices on a port of an ATA controller.

    @param  IN EFI_ATA_PASS_THRU_PROTOCOL   *This,
    @param  IN UINT16                       Port,
    @param  IN OUT UINT16                   *PortMultiplierPort

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/
EFI_STATUS
GetNextDevice (
    IN EFI_ATA_PASS_THRU_PROTOCOL   *This,
    IN UINT16                       Port,
    IN OUT UINT16                   *PortMultiplierPort
)
{
    UINT16                  CurrentPortNumber = 0;
    UINT16                  CurrentPortMultiplierNumber = 0;
    UINT16                  PreviousPortMultiplierNumber = 0;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE   *SataDevices = 0;
    SATA_DEVICE_INTERFACE   *pSataDevices = 0;
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL    *IdeDevices = 0;
    AMI_IDE_BUS_PROTOCOL    *pIdeDevices = 0;
#endif
    UINT16                  DevCount = 0;
    UINT8                   DevIndex = 0;
    BOOLEAN                 ModeFlag = ((ATA_PASS_THRU_PROTOCOL*)This)->ModeFlag;

    // If PMPort is NULL, return Invalid parameter
    if (PortMultiplierPort == NULL) {
        return EFI_INVALID_PARAMETER;
    }

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if(ModeFlag){

        pSataDevices = (SATA_DEVICE_INTERFACE *)((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceBuffer;
        if(pSataDevices == NULL) return EFI_NOT_FOUND;

        DevCount = ((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceCount;

        SataDevices = ((SATA_DEVICE_INTERFACE **)pSataDevices)[DevIndex];
        if((SataDevices == NULL) || (DevCount == 0)) {
            return EFI_NOT_FOUND;
        }

        PreviousPortMultiplierNumber = SataDevices->AhciBusInterface->PrevPortMultiplierPortNum;

        // If PortMultiplier is not 0xFFFF or Previous Port multiplier Number returned on earlier call
        // return invalid parameter
        if(( 0xFFFF != *PortMultiplierPort) && (PreviousPortMultiplierNumber != *PortMultiplierPort)) {
            return EFI_INVALID_PARAMETER;
        }
        // If PortMultiplierPort points to 0xFFFF, then the port multiplier port number of the first ATA device on port
        // of the ATA controller is returned in PortMultiplierPort and EFI_SUCCESS is returned.
        if(0xFFFF == *PortMultiplierPort) {
            for(DevIndex = 0; DevIndex < DevCount; DevIndex++){
                SataDevices = ((SATA_DEVICE_INTERFACE **)pSataDevices)[DevIndex];
                CurrentPortNumber = SataDevices->PortNumber;
                CurrentPortMultiplierNumber = SataDevices->PMPortNumber;

                if( CurrentPortNumber == Port) {
                    SataDevices->AhciBusInterface->PrevPortMultiplierPortNum = CurrentPortMultiplierNumber;
                    *PortMultiplierPort = CurrentPortMultiplierNumber;
                    return EFI_SUCCESS;
                }
            }
        }

        // If PortMultiplierPort points to a port multiplier port number value that was returned on a previous call to
        // GetNextDevice(), then the port multiplier port number of the next ATA device on the port of the
        // ATA controller is returned in PortMultiplierPort, and EFI_SUCCESS is returned.
        if(PreviousPortMultiplierNumber == *PortMultiplierPort) {
            for(DevIndex = 0; DevIndex < DevCount; DevIndex++){
                SataDevices = ((SATA_DEVICE_INTERFACE **)pSataDevices)[DevIndex];
                CurrentPortNumber = SataDevices->PortNumber;
                CurrentPortMultiplierNumber = SataDevices->PMPortNumber;

                if(( CurrentPortNumber == Port) && (CurrentPortMultiplierNumber > *PortMultiplierPort)) {
                    SataDevices->AhciBusInterface->PrevPortMultiplierPortNum = CurrentPortMultiplierNumber;
                    *PortMultiplierPort = CurrentPortMultiplierNumber;
                    return EFI_SUCCESS;
                }
            }
        }

        return EFI_NOT_FOUND;

    }
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if(!ModeFlag){

        pIdeDevices = (AMI_IDE_BUS_PROTOCOL *)((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceBuffer;
        if(pIdeDevices == NULL) return EFI_NOT_FOUND;

        DevCount = ((ATA_PASS_THRU_PROTOCOL*)This)->AtaDeviceCount;

        IdeDevices = ((AMI_IDE_BUS_PROTOCOL **)pIdeDevices)[DevIndex];
        if((IdeDevices == NULL) || (DevCount == 0)) {
            return EFI_NOT_FOUND;
        }


        PreviousPortMultiplierNumber = IdeDevices->IdeBusInitInterface->PrevPortMultiplierPortNum;
        // If PortMultiplier is not 0xFFFF or Previous Port multiplier Number returned on earlier call
        // return invalid parameter
        if(( 0xFFFF != *PortMultiplierPort) && (PreviousPortMultiplierNumber != *PortMultiplierPort)) {
            return EFI_INVALID_PARAMETER;
        }

        // If PortMultiplierPort points to 0xFFFF, then the port multiplier port number of the first ATA device on port
        // of the ATA controller is returned in PortMultiplierPort and EFI_SUCCESS is returned.
        if( 0xFFFF == *PortMultiplierPort){

            for(DevIndex = 0; DevIndex < DevCount; DevIndex++){
                IdeDevices = ((AMI_IDE_BUS_PROTOCOL **)pIdeDevices)[DevIndex];
                CurrentPortNumber = IdeDevices->IdeDevice.Channel;
                CurrentPortMultiplierNumber = IdeDevices->IdeDevice.Device;

                if( CurrentPortNumber == Port) {
                    IdeDevices->IdeBusInitInterface->PrevPortMultiplierPortNum = CurrentPortMultiplierNumber;
                    *PortMultiplierPort = CurrentPortMultiplierNumber;
                    return EFI_SUCCESS;
                }
            }
        }

        // If PortMultiplierPort points to a port multiplier port number value that was returned on a previous call to
        // GetNextDevice(), then the port multiplier port number of the next ATA device on the port of the
        // ATA controller is returned in PortMultiplierPort, and EFI_SUCCESS is returned.
        if(PreviousPortMultiplierNumber == *PortMultiplierPort) {
            for(DevIndex = 0; DevIndex < DevCount; DevIndex++){
                IdeDevices = ((AMI_IDE_BUS_PROTOCOL **)pIdeDevices)[DevIndex];
                CurrentPortNumber = IdeDevices->IdeDevice.Channel;
                CurrentPortMultiplierNumber = IdeDevices->IdeDevice.Device;

                if(( CurrentPortNumber == Port) && (CurrentPortMultiplierNumber > *PortMultiplierPort)) {
                    IdeDevices->IdeBusInitInterface->PrevPortMultiplierPortNum = CurrentPortMultiplierNumber;
                    *PortMultiplierPort = CurrentPortMultiplierNumber;
                    return EFI_SUCCESS;
                }
            }
        }

        return EFI_NOT_FOUND;
    }
#endif

    return EFI_SUCCESS;
}

/**
    This function allocates and builds a device path node for
    an ATA device on an ATA controller.

    @param  IN  EFI_ATA_PASS_THRU_PROTOCOL      *This,
    @param  IN  UINT16                          Port,
    @param  IN  UINT16                          PortMultiplierPort,
    @param  IN  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
BuildDevicePath (
    IN  EFI_ATA_PASS_THRU_PROTOCOL      *This,
    IN  UINT16                          Port,
    IN  UINT16                          PortMultiplierPort,
    IN  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath
)
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    SATA_DEVICE_PATH            *SataDevicePath = NULL;
    ATAPI_DEVICE_PATH           *AtapiDevicePath = NULL;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = NULL;
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = NULL;
#endif
    EFI_DEVICE_PATH_PROTOCOL    *DevPath;
    BOOLEAN                     ModeFlag = ((ATA_PASS_THRU_PROTOCOL*)This)->ModeFlag;

    // If DevicePath is null return EFI_INVALID_PARAMETER
    if(NULL == DevicePath){
        return  EFI_INVALID_PARAMETER;
    }

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if(ModeFlag) {
        // Allocate memory
        Status = pBS->AllocatePool(EfiBootServicesData,sizeof(SATA_DEVICE_PATH),(VOID**)&SataDevicePath);
        if(EFI_ERROR(Status)){
            return EFI_OUT_OF_RESOURCES;
        }

        // If Port Number and PortMultiplierPort is invalid, return with EFI_INVALID_PARAMETER
        SataDevInterface = GetSataDevInterfaceThruPortPMPort(This,Port,PortMultiplierPort);
        if(NULL == SataDevInterface){
            return  EFI_NOT_FOUND;
        }

        // Get SATA Device Path Node
        DevPath = SearchDevicePath (SataDevInterface->DevicePathProtocol, MESSAGING_DEVICE_PATH, MSG_SATA_DP);
        if(NULL == DevPath){
            return EFI_NOT_FOUND;
        }
        MemCpy(SataDevicePath,DevPath,sizeof(EFI_DEVICE_PATH_PROTOCOL));

        if (!(SataDevicePath == NULL)){
            SataDevicePath->HBAPortNumber = Port;
            SataDevicePath->PortMultiplierPortNumber = PortMultiplierPort;
            SataDevicePath->Lun = 0;
        }

        // Update The Device path
        *DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) SataDevicePath;

    }
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if(!ModeFlag){
        // Allocate memory
        Status = pBS->AllocatePool(EfiBootServicesData,sizeof(ATAPI_DEVICE_PATH),(VOID**)&AtapiDevicePath);
        if(EFI_ERROR(Status)){
            return EFI_OUT_OF_RESOURCES;
        }
        // If Port Number and PortMultiplierPort is invalid, return with EFI_INVALID_PARAMETER
        IdeBusInterface = GetIdeBusInterfaceThruPortPMPort(This,Port,PortMultiplierPort);
        if(NULL == IdeBusInterface){
            return  EFI_NOT_FOUND;
        }

        // Get ATAPI Device Path Node
        DevPath = SearchDevicePath (IdeBusInterface->DevicePathProtocol, MESSAGING_DEVICE_PATH, MSG_ATAPI_DP);
        if(NULL == DevPath){
            return EFI_NOT_FOUND;
        }
        MemCpy(AtapiDevicePath,DevPath,sizeof(EFI_DEVICE_PATH_PROTOCOL));

        if (!(AtapiDevicePath == NULL)){
            AtapiDevicePath->PrimarySecondary = (UINT8)Port;
            AtapiDevicePath->SlaveMaster = (UINT8)PortMultiplierPort;
            AtapiDevicePath->Lun = 0;
        }

        // Update The Device path
        *DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AtapiDevicePath;
    }
#endif

    return EFI_SUCCESS;
}

/**
   This function translates a device path node to a port number
    and port multiplier port number.

    @param  IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    @param  IN  EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
    @param  OUT UINT16                      *Port,
    @param  OUT UINT16                      *PortMultiplierPort

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
GetDevice (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
    OUT UINT16                      *Port,
    OUT UINT16                      *PortMultiplierPort
)
{
    EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath = DevicePath;
    UINT16                       DevicePathLength = 0;
    BOOLEAN                      ModeFlag = ((ATA_PASS_THRU_PROTOCOL*)This)->ModeFlag;
    SATA_DEVICE_PATH            *SataDevicePath = NULL;
    ATAPI_DEVICE_PATH           *AtapiDevicePath = NULL;

    if((DevicePath == NULL) || (Port == NULL)||(PortMultiplierPort == NULL)){
        return EFI_INVALID_PARAMETER;
    }

    DevicePathLength = (DevicePath)->Length[0] | (DevicePath)->Length[1] << 8;

    //
    // Check for Validity
    //
    if(ModeFlag) {
        if ((DevicePath->Type != MESSAGING_DEVICE_PATH) ||
            (DevicePath->SubType != MSG_SATA_DP) ||
            (DevicePathLength != sizeof(SATA_DEVICE_PATH))) {
            return EFI_UNSUPPORTED;
        }

        SataDevicePath = (SATA_DEVICE_PATH *)RemainingDevicePath;
        if (!(SataDevicePath == NULL)){
            *Port                = SataDevicePath->HBAPortNumber;
            *PortMultiplierPort  = SataDevicePath->PortMultiplierPortNumber;
            return EFI_SUCCESS;
        }
    } else {
        if ((DevicePath->Type != MESSAGING_DEVICE_PATH) ||
            (DevicePath->SubType != MSG_ATAPI_DP) ||
            (DevicePathLength != sizeof(ATAPI_DEVICE_PATH))) {
            return EFI_UNSUPPORTED;
        }

        AtapiDevicePath = (ATAPI_DEVICE_PATH *)RemainingDevicePath;
        if (!(AtapiDevicePath == NULL)){
            *Port                = AtapiDevicePath->PrimarySecondary;
            *PortMultiplierPort  = AtapiDevicePath->SlaveMaster;
            return EFI_SUCCESS;
        }
    }

    //
    //No Valid Translation is found
    //
    return EFI_NOT_FOUND;
}

/**
    Resets a specific port on the ATA controller. This operation
    also resets all the ATA devices connected to the port

    @param  IN EFI_ATA_PASS_THRU_PROTOCOL   *This,
    @param  IN UINT16                       Port

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
ResetPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  UINT16                      Port
)
{
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    EFI_STATUS              Status;
    SATA_DEVICE_INTERFACE   *SataDevInterface;
    AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface;
    BOOLEAN                 ModeFlag = ((ATA_PASS_THRU_PROTOCOL*)This)->ModeFlag;

    if(ModeFlag){
        //
        //Get SATA device interface
        //
        SataDevInterface = GetSataDevInterfaceThruPortPMPort(This,Port,0xFFFF);
        if(NULL == SataDevInterface){
            return  EFI_INVALID_PARAMETER;
        }

        AhciBusInterface = SataDevInterface->AhciBusInterface;
        Status = SataDevInterface->AhciBusInterface->GeneratePortReset( AhciBusInterface,
                                                                        SataDevInterface,(UINT8) Port,
                                                                        (UINT8)0xFF,
                                                                        0x00,
                                                                        0x00
                                                                        );
        return Status;
    }
#endif
    return EFI_UNSUPPORTED;
}

/**
    Resets an ATA device that is connected to an ATA controller

    @param  IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    @param  IN  UINT16                      Port,
    @param  IN  UINT16                      PortMultiplierPort

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
ResetDevice (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  UINT16                      Port,
    IN  UINT16                       PortMultiplierPort
)
{
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    EFI_STATUS              Status;
    SATA_DEVICE_INTERFACE   *SataDevInterface;
    BOOLEAN                 ModeFlag = ((ATA_PASS_THRU_PROTOCOL*)This)->ModeFlag;

    if(ModeFlag){
        //
        // Get SATA device interface
        //
        SataDevInterface = GetSataDevInterfaceThruPortPMPort(This,Port,PortMultiplierPort);
        if(NULL == SataDevInterface){
            return  EFI_INVALID_PARAMETER;
        }

        Status = SataDevInterface->AhciBusInterface->GeneratePortReset( SataDevInterface->AhciBusInterface,
                                                                        SataDevInterface,(UINT8) Port,
                                                                        (UINT8) PortMultiplierPort,
                                                                        0x00,
                                                                        0x00
                                                                      );

        return Status;
    }
#endif
    return EFI_UNSUPPORTED;

}

/**
    Checks if the command is for 48-bit LBA

    @param  IN  UINT8       Command

    @retval TRUE or FALSE

**/
BOOLEAN
Check48BitCommand (
    IN UINT8        Command
 )
{
    if ( Command == READ_SECTORS_EXT ||
         Command == READ_MULTIPLE_EXT ||
         Command == WRITE_SECTORS_EXT ||
         Command == WRITE_MULTIPLE_EXT ||
         Command == READ_DMA_EXT    ||
         Command == WRITE_DMA_EXT )
        return TRUE;
    else
        return FALSE;
}

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

