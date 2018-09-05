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

/** @file AtaPassThruSupport.h
    Header file for AtaPassthru Driver

**/

#ifndef _AtaPassThruSupport_
#define _AtaPassThruSupport_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/DevicePath.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/AmiAtaPassThruInit.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
#include <Protocol/AmiIdeBus.h>
#include <Protocol/AmiIdeBusBoard.h>
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
#include <Protocol/AmiAhciBus.h>
#include <Protocol/LegacyAhci.h>
#endif

//---------------------------------------------------------------------------

#ifndef END_DEVICE_PATH_TYPE
#define END_DEVICE_PATH_TYPE                0x7f
#endif

#define END_ENTIRE_DEVICE_PATH_SUBTYPE      0xFF
#define EFI_DP_TYPE_MASK                    0x7F
#define EFI_DP_TYPE_UNPACKED                0x80
#define END_INSTANCE_DEVICE_PATH_SUBTYPE    0x01


#define DP_IS_END_TYPE(a)
#define DP_IS_END_SUBTYPE(a)        ( ((a)->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE )

#define DevicePathType(a)           ( ((a)->Type) & EFI_DP_TYPE_MASK )
#define DevicePathSubType(a)        ( (a)->SubType )
#define DevicePathNodeLength(a)     ( ((a)->Length[0]) | ((a)->Length[1] << 8) )

#define IsDevicePathEndType(a)      ( DevicePathType(a) == END_DEVICE_PATH_TYPE )
#define IsDevicePathEndSubType(a)   ( (a)->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE )
#define IsDevicePathEnd(a)          ( IsDevicePathEndType(a) && IsDevicePathEndSubType(a) )
#define NextDevicePathNode(a)       ( (EFI_DEVICE_PATH_PROTOCOL *) ( ((UINT8 *) (a)) + DevicePathNodeLength(a)))

#define IsDevicePathEnd(a)          ( IsDevicePathEndType(a) && IsDevicePathEndSubType(a) )


#define ZeroMemory( Buffer, Size ) pBS->SetMem( Buffer, Size, 0 )

//Device States
#define     DEVICE_DISABLED                     0
#define     DEVICE_IN_RESET_STATE               1
#define     DEVICE_DETECTION_FAILED             2
#define     DEVICE_DETECTED_SUCCESSFULLY        3
#define     DEVICE_CONFIGURED_SUCCESSFULLY      4
#define     DEVICE_REMOVED                      5
#define     CONTROLLER_NOT_PRESENT              0xff

#define     HBA_PORTS_TFD                       0x0020
#define     HBA_PORTS_START                     0x0100
#define     HBA_PORTS_REG_WIDTH                 0x0080
#define     FIS_REGISTER_D2H                    0x34
#define     FIS_PIO_SETUP                       0x5F

#define     MmAddress( BaseAddr, Register ) \
            ((UINTN)(BaseAddr) + \
            (UINTN)(Register) \
             )

#define     Mm32Ptr( BaseAddr, Register ) \
            ((volatile UINT32 *)MmAddress (BaseAddr, Register ))

#define     HBA_REG32( BaseAddr, Register ) \
            (*Mm32Ptr ((BaseAddr), (Register)))

#define     HBA_PORT_REG_BASE(Port) \
             (UINTN) (Port * HBA_PORTS_REG_WIDTH + HBA_PORTS_START)

#define     HBA_PORT_REG32(BaseAddr, Port, Register) \
            (HBA_REG32 ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port))))

typedef struct {
    UINT8           Ahci_Dsfis[0x1C];               // DMA Setup FIS
    UINT8           Ahci_Dsfis_Rsvd[0x04];
    UINT8           Ahci_Psfis[0x14];               // PIO Setup FIS
    UINT8           Ahci_Psfis_Rsvd[0x0C];
    UINT8           Ahci_Rfis[0x14];                // D2H Register FIS
    UINT8           Ahci_Rfis_Rsvd[0x04];
    UINT64          Ahci_Sdbfis;                     // Set Device Bits FIS
    UINT8           Ahci_Ufis[0x40];                 // Unknown FIS
    UINT8           Ahci_Ufis_Rsvd[0x60];
} AHCI_RECEIVED_FIS;

EFI_STATUS
PassThru (
    IN  EFI_ATA_PASS_THRU_PROTOCOL              *This,
    IN  UINT16                                  Port,
    IN  UINT16                                  PortMultiplierPort,
    IN  OUT EFI_ATA_PASS_THRU_COMMAND_PACKET   *Packet,
    IN  EFI_EVENT                               Event OPTIONAL
);

EFI_STATUS
GetNextPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  OUT UINT16                  *Port
);

EFI_STATUS
GetNextDevice (
    IN  EFI_ATA_PASS_THRU_PROTOCOL  *This,
    IN  UINT16                      Port,
    IN  OUT UINT16                  *PortMultiplierPort
);

EFI_STATUS
BuildDevicePath (
    IN  EFI_ATA_PASS_THRU_PROTOCOL          *This,
    IN  UINT16                              Port,
    IN  UINT16                              PortMultiplierPort,
    IN  OUT EFI_DEVICE_PATH_PROTOCOL        **DevicePath
);

EFI_STATUS
GetDevice (
    IN  EFI_ATA_PASS_THRU_PROTOCOL            *This,
    IN  EFI_DEVICE_PATH_PROTOCOL              *DevicePath,
    OUT UINT16                                *Port,
    OUT UINT16                                *PortMultiplierPort
);

EFI_STATUS
ResetPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL             *This,
    IN  UINT16                                 Port
);

EFI_STATUS
ResetDevice (
    IN  EFI_ATA_PASS_THRU_PROTOCOL             *This,
    IN  UINT16                                 Port,
    IN  UINT16                                 PortMultiplierPort
);

EFI_DEVICE_PATH_PROTOCOL
*SearchDevicePath (
    IN  OUT EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
    IN  UINT8                           Type,
    IN  UINT8                           SubType
);

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
VOID
GetAhciAsbStatus (
    IN  SATA_DEVICE_INTERFACE               *SataDevInterface,
    IN  OUT EFI_ATA_STATUS_BLOCK            *Asb
);

SATA_DEVICE_INTERFACE
*GetSataDevInterfaceThruPortPMPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL            *This,
    IN  UINT16                                Port,
    IN  UINT16                                PortMultiplierPort
);

EFI_STATUS
GetSataDevCountandAddressBuffer (
    IN  EFI_ATA_PASS_THRU_PROTOCOL          *This,
    OUT SATA_DEVICE_INTERFACE               **SataAddressBuffer,
    OUT UINT16                              *NumberOfDevices
);

UINT16
CountAhciDrives (
    IN  EFI_HANDLE                          *HandleBuffer,
    IN  OUT SATA_DEVICE_INTERFACE           **Devices
);
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
VOID
GetIdeAsbStatus (
    IN  AMI_IDE_BUS_PROTOCOL                *IdeBusInterface,
    IN  OUT EFI_ATA_STATUS_BLOCK            *Asb
);

EFI_STATUS
IdeReadByte (
    IN  EFI_PCI_IO_PROTOCOL                 *PciIO,
    IN  UINT16                              Register,
    OUT UINT8                               *Data8
);

AMI_IDE_BUS_PROTOCOL
*GetIdeBusInterfaceThruPortPMPort (
    IN  EFI_ATA_PASS_THRU_PROTOCOL            *This,
    IN  UINT16                                Port,
    IN  UINT16                                PortMultiplierPort
);

EFI_STATUS
GetIdeDevCountandAddressBuffer (
    IN  EFI_ATA_PASS_THRU_PROTOCOL              *This,
    OUT AMI_IDE_BUS_PROTOCOL                    **IdeAddressBuffer,
    OUT UINT16                                  *NumberOfDevices
);

UINT16
CountIdeDrives (
    IN  EFI_HANDLE                              *HandleBuffer,
    IN  AMI_IDE_BUS_PROTOCOL                    **Devices
);
#endif

EFI_STATUS
InstallAtaPassThru (
  IN    EFI_HANDLE                              Controller,
  IN    BOOLEAN                                 ModeFlag
);

EFI_STATUS
StopAtaPassThruSupport (
    IN  EFI_HANDLE                              Controller,
    IN  BOOLEAN                                 ModeFlag
);

EFI_STATUS
EFIAPI AtaPassThruEntry (
    IN  EFI_HANDLE                             ImageHandle,
    IN  EFI_SYSTEM_TABLE                       *SystemTable
);

BOOLEAN
Check48BitCommand (
    IN  UINT8                                   Command
);


typedef
struct _ATA_PASS_THRU_PROTOCOL {
    EFI_ATA_PASS_THRU_PROTOCOL          EfiAtaPassThru;
    BOOLEAN                             ModeFlag;
    VOID                                *ControllerHandle;
    VOID                                *AtaDeviceBuffer;
    UINT16                              AtaDeviceCount;
}ATA_PASS_THRU_PROTOCOL;

#ifdef __cplusplus
}
#endif

#endif

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

