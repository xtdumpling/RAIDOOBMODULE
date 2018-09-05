//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file HddSecurity.h
    This file contains the Includes, Definitions, typedefs,
    Variable and External Declarations, Structure and
    function prototypes needed for the IdeSecurity driver

**/

#ifndef _HDD_SECURITY_H_
#define _HDD_SECURITY_H_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include "Protocol/PciIo.h"
#include "Protocol/DevicePath.h"
#include "Protocol/DriverBinding.h"
#include "Protocol/BlockIo.h"
#include <IndustryStandard/AmiAtaAtapi.h>
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
#include <Protocol/AmiIdeBus.h>
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
#include <Protocol/AmiAhciBus.h>
#endif
#include <Protocol/AmiHddSecurity.h>
#include <Protocol/ComponentName.h>
#include <Protocol/BootScriptSave.h>
#include <Protocol/SmmBase.h>
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
#include <Protocol/SmmControl2.h>
#else
#include <Protocol/SmmControl.h>
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
#include <Protocol/AhciSmmProtocol.h>
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
#include "Nvme/NvmeController.h"
#include <Protocol/AmiNvmeController.h>
#if defined (MDE_PKG_VERSION) && (MDE_PKG_VERSION >= 10) \
      && (defined NVME_DRIVER_VERSION && NVME_DRIVER_VERSION >= 10)
    #define USE_MDE_PKG_NVME_PASSTHRU_CHANGES 1
    #include "MdePkg/Include/Protocol/NvmExpressPassthru.h"
#elif (defined NVME_DRIVER_VERSION && NVME_DRIVER_VERSION >= 9)
    #include "Protocol/AmiNvmExpressPassThru.h"
#else
#include "AmiModulePkg/Include/Protocol/NvmExpressPassThru.h"
#endif
#include <Nvme/NvmePassthru.h>
#endif

#include <Protocol/SmmCommunication.h>
#include "HddSecurityCommon.h"

//---------------------------------------------------------------------------

#define IDE_SECURITY_PWNV_GUID \
    { 0x69967a8c, 0x1159, 0x4522, 0xaa, 0x89, 0x74, 0xcd, 0xc6, 0xe5, 0x99, 0xa0}

#define EFI_SMM_SAVE_HDD_PASSWORD_GUID \
    { 0xeedcf975, 0x4dd3, 0x4d94, 0x96, 0xff, 0xaa, 0xca, 0x83, 0x53, 0xb8, 0x7b }

#define EFI_SMM_REMOVE_HDD_PASSWORD_GUID \
    { 0xc2b1e795, 0xf9c5, 0x4829, 0x8a, 0x42, 0xc0, 0xb3, 0xfe, 0x57, 0x15, 0x17 }

#define AMI_LOCATE_IDE_AHCI_PROTOCOL_GUID \
    { 0xb3f096e9, 0x2d46, 0x4e8e, 0xa2, 0x2c, 0x7d, 0xe8, 0xb1, 0x6b, 0x3a, 0x5b }

#define AMI_HDD_SECURITY_BOOTSCRIPT_SAVE_TRIGGER_GUID \
    { 0x60b0760c, 0x7d1b, 0x43f3, 0x95, 0x25, 0x60, 0x77, 0xbe, 0x41, 0x37, 0xe2 }

#define AMI_NVME_SECURITY_BOOTSCRIPT_SAVE_TRIGGER_GUID \
    { 0x3fb7e17f, 0x1172, 0x4e2a, 0x9a, 0x25, 0xba, 0x5f, 0xe6, 0x2c, 0xc7, 0xc8 }


// Size of SMM communicate header, without including the Data.
#define SMM_COMMUNICATE_HEADER_SIZE  (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data))

// Size of HDD Password DATA size
#define HDD_PASSWORD_SIZE  sizeof(HDD_PASSWORD)
#define SATA_COMMAND_TIMEOUT                    5   // 5 Sec

EFI_GUID gSaveHddPasswordGuid=EFI_SMM_SAVE_HDD_PASSWORD_GUID;
EFI_GUID gRemoveHddPasswordGuid=EFI_SMM_REMOVE_HDD_PASSWORD_GUID;
EFI_GUID gEfiSmmCommunicationProtocolGuid=EFI_SMM_COMMUNICATION_PROTOCOL_GUID;
EFI_GUID gLocateIdeAhciSmmProtocolGuid=AMI_LOCATE_IDE_AHCI_PROTOCOL_GUID;
EFI_GUID gHddSecurityBootScriptSaveTriggerGuid=AMI_HDD_SECURITY_BOOTSCRIPT_SAVE_TRIGGER_GUID;
EFI_GUID gNvmeSecurityBootScriptSaveTriggerGuid=AMI_NVME_SECURITY_BOOTSCRIPT_SAVE_TRIGGER_GUID;

#define HDD_PWD_ENCRYPTION_KEY      "H?p1mA*k920_84o3d^!z@L.x4$kY64"

#define EFI_SEGMENT( _Adr )     (UINT16) ((UINT16) (((UINTN) (_Adr))\
                                                    >> 4) & 0xf000)
#define EFI_OFFSET( _Adr )      (UINT16) (((UINT16) ((UINTN) (_Adr))) & 0xffff)

#define     ZeroMemory( Buffer, Size ) pBS->SetMem( Buffer, Size, 0 )

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
#define GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT(NvmePassThru, SecReceiveBufferElement) \
    (UINT16)((((NVM_EXPRESS_PASS_THRU_INSTANCE*)NvmePassThru)->\
     NvmeControllerProtocol->SecReceiveBuffer.SecReceiveBufferElement>>8) | \
     (((NVM_EXPRESS_PASS_THRU_INSTANCE*)NvmePassThru)->\
     NvmeControllerProtocol->SecReceiveBuffer.SecReceiveBufferElement<<8))
#else
#define GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT(NvmePassThru,SecReceiveBufferElement) 0
#endif

#define SECURITY_PROTOCOL_INFORMATION        0x00

// TODO Need to define code to Nvme related file
#define  NVME_ATA_SECURITY_SET_PASSWORD      0x1
#define  NVME_ATA_SECURITY_UNLOCK            0x2
#define  NVME_ATA_ECURITY_ERASE_PREPARE      0x3
#define  NVME_ATA_SECURITY_ERASE_UNIT        0x4
#define  NVME_ATA_SECURITY_FREEZE_LOCK       0x5
#define  NVME_ATA_SECURITY_DISABLE_PASSWORD  0x6

//
// Storage Security Protocol
//
#define SECURITY_PROTOCOL_INFORMATION                   0x00
#define SECURITY_PROTOCOL_1                             0x01
#define SECURITY_PROTOCOL_2                             0x02
#define SECURITY_PROTOCOL_SDCARD_TRUSTED_FLASH          0xED
#define SECURITY_PROTOCOL_IEEE1667                      0xEE

#pragma pack(1)

typedef 
struct _SP0_TRUSTED_RECEIVE_PARAMETER_DATA { 
    UINT8      Reserved[6]; 
    UINT8      ListLengthHigh; 
    UINT8      ListLengthLow; 
    UINT8      SupportedSPList[502]; 
    UINT8      PadBytesHigh; 
    UINT8      PadBytesLow; 
}SP0_TRUSTED_RECEIVE_PARAMETER_DATA;

typedef struct
{
    UINT8  Bus;
    UINT8  Device;
    UINT8  Function;
    UINT8  Controller;
    UINT32 Reserved;
} EDD_PCI;

typedef struct
{
    UINT16 Base;
    UINT16 Reserved;
    UINT32 Reserved2;
} EDD_LEGACY;

typedef union
{
    EDD_PCI    Pci;
    EDD_LEGACY Legacy;
} EDD_INTERFACE_PATH;

typedef struct
{
    UINT8 Master;
    UINT8 Reserved[15];
} EDD_ATA;

typedef struct
{
    UINT8 Master;
    UINT8 Lun;
    UINT8 Reserved[14];
} EDD_ATAPI;

typedef struct
{
    UINT16 TargetId;
    UINT64 Lun;
    UINT8  Reserved[6];
} EDD_SCSI;

typedef struct
{
    UINT64 SerialNumber;
    UINT64 Reserved;
} EDD_USB;

typedef struct
{
    UINT64 Guid;
    UINT64 Reserved;
} EDD_1394;

typedef struct
{
    UINT64 Wwn;
    UINT64 Lun;
} EDD_FIBRE;

typedef struct
{
    UINT8 bPortNum;
    UINT8 Reserved[15];
} EDD_SATA;

typedef union
{
    EDD_ATA   Ata;
    EDD_ATAPI Atapi;
    EDD_SCSI  Scsi;
    EDD_USB   Usb;
    EDD_1394  FireWire;
    EDD_FIBRE FibreChannel;
    EDD_SATA  Sata;
} EDD_DEVICE_PATH;

typedef struct
{
    UINT16             StructureSize;
    UINT16             Flags;
    UINT32             MaxCylinders;
    UINT32             MaxHeads;
    UINT32             SectorsPerTrack;
    UINT64             PhysicalSectors;
    UINT16             BytesPerSector;
    UINT32             FDPT;
    UINT16             Key;
    UINT8              DevicePathLength;
    UINT8              Reserved1;
    UINT16             Reserved2;
    CHAR8              HostBusType[4];
    CHAR8              InterfaceType[8];
    EDD_INTERFACE_PATH InterfacePath;
    EDD_DEVICE_PATH    DevicePath;
    UINT8              Reserved3;
    UINT8              Checksum;
} EDD_DRIVE_PARAMETERS;

typedef struct _HDDSECDATA
{
    UINT16 UserMaster;
    UINT32 PasswordLength;
    UINT8  HddUserPassword[IDE_PASSWORD_LENGTH];
    UINT8  HddMasterPassword[IDE_PASSWORD_LENGTH];
} HDDSECDATA;

#pragma pack()

EFI_STATUS
EFIAPI
InstallSecurityInterface (
    IN  VOID                       *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
);

EFI_STATUS
EFIAPI
StopSecurityModeSupport (
    IN  VOID                       *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
);

EFI_STATUS
EFIAPI
ReturnSecurityStatus (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      *SecurityStatus
);

EFI_STATUS
EFIAPI
SecuritySetPassword (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      Control,
    UINT8                       *Buffer,
    UINT16                      RevisionCode
);

EFI_STATUS
EFIAPI
SecurityUnlockPassword (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      Control,
    UINT8                       *Buffer
);

EFI_STATUS
EFIAPI
SecurityDisablePassword (
    IN  AMI_HDD_SECURITY_PROTOCOL *This,
    UINT16                        Control,
    UINT8                         *Buffer
);

EFI_STATUS
SetDefaultMasterPassword (
    IN  AMI_HDD_SECURITY_PROTOCOL *This
);

EFI_STATUS
SecuritySetDefaultMasterPassword (
    IN  AMI_HDD_SECURITY_PROTOCOL *This,
    UINT16                        Control,
    UINT8                         *Buffer,
    UINT16                        RevisionCode
);

EFI_STATUS
EFIAPI
SecurityFreezeLock (
    IN  AMI_HDD_SECURITY_PROTOCOL *This
);

EFI_STATUS
EFIAPI
SecurityEraseUnit (
    IN AMI_HDD_SECURITY_PROTOCOL *This,
    UINT16                       Control,
    UINT8                        *Buffer
);

EFI_STATUS
EFIAPI
ReturnIdePasswordFlags (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT32                          *IdePasswordFlags
);

EFI_STATUS
SecurityCommonRoutine (
    IN  VOID                   *IdeBusInterface,
    UINT16                     Control,
    UINT8                      *Buffer,
    UINT8                      SecurityCommand,
    UINT16                     RevisionCode,
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
);

EFI_STATUS 
IsSecurityProtocolSupported (
    IN     VOID     *BusInterface,
    IN     AMI_STORAGE_BUS_INTERFACE  BusInterfaceType,
    OUT    BOOLEAN  *SAT3Supported,
    OUT    BOOLEAN  *TcgSupported
);

EFI_STATUS 
IsNvmeSecurityProtocolSupported (
        IN     VOID     *BusInterface,
        OUT    BOOLEAN  *SAT3Supported,
        OUT    BOOLEAN  *TcgSupported
);

EFI_STATUS 
GetNvmeSecurityProtocolInformation (
    IN     VOID     *BusInterface,
    IN OUT VOID     *Buffer,
    IN OUT UINT32    BufferLength 
);

EFI_STATUS
GetNvmeDeviceSecurityData (
     VOID        *BusInterface,
     UINT8       *Buffer,
     UINT32      Bufferlength
);

EFI_STATUS
NvmeSecurityRecieveRoutine(
    VOID        *BusInterface,
    UINT8       *Buffer,
    UINT32       Bufferlength,
    UINT32      CDW10_Value
);

EFI_STATUS
NvmeSecuritySendCommandCommonRoutine (
    IN  VOID                   *BusInterface,
    UINT16                     Control,
    UINT8                      *Buffer,
    UINT8                      SecurityCommand,
    UINT16                     RevisionCode,
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
);

EFI_STATUS
ConnectController (
    IN  EFI_HANDLE                                   ControllerHandle,
    IN  EFI_HANDLE*DriverImageHandle                 OPTIONAL,
    IN  EFI_DEVICE_PATH_PROTOCOL*RemainingDevicePath OPTIONAL,
    IN  BOOLEAN                                      Recursive
);

EFI_STATUS
GatherIdeInfo (
    IN  VOID                       *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType,
    OUT UINT32                     *HddDataId
);

EFI_STATUS
UpdateIdentifyData (
    IN  VOID                       *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
);

EFI_STATUS
SetHddSecDataInNvram (
    IN  VOID    *BusInterface,
    UINT32      *HddDataId,
    UINT16      Control,
    UINT8       *Buffer
);

EFI_STATUS 
GetHddSecDataFromNvram(
    IN VOID                        *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType,
    UINT8                          *Buffer 
);

EFI_STATUS
TransferPwDataFromNvramToSmram (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This
);

VOID
ConvertHddDataIdToString (
    IN  UINT32  DataId,
    OUT CHAR16  *String
);

VOID
EncodeDecodePassword (
    IN  UINT8   *InputString,
    OUT UINT8   *OutputString,
    IN  UINT32  StringLength
);

EFI_STATUS
OEMSetMasterPassword (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This
);

EFI_STATUS
SmmHDDPasswordInterface (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                          Control,
    UINT8                           *Buffer,
    UINT8                           Action
);

VOID
EFIAPI
CallbackFuncToStoreBootScriptForS3Resume (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
);

VOID
IdeBusMiscSmmFeatureCallback (
    IN  EFI_EVENT   Event,
    IN   VOID       *Context
);

UINTN
EfiValueToString (
    IN  OUT CHAR16 *Buffer,
    IN  INT64      Value,
    IN  UINTN      Flags,
    IN  UINTN      Width
);

EFI_STATUS
CommonNonDataHook (
    IN  VOID                       *BusInterface,
    IN  COMMAND_STRUCTURE          CommandStructure,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
);

EFI_STATUS
CommonPioDataHook (
    IN  VOID                       *BusInterface,
    IN  COMMAND_STRUCTURE          CommandStructure,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
);

EFI_STATUS
CommonReadWritePioHook (
    IN  VOID                      *BusInterface,
    IN  OUT VOID                  *Buffer,
    IN  UINTN                     ByteCount,
    IN  UINT64                    LBA,
    IN  UINT8                     ReadWriteCommand,
    IN  BOOLEAN                   ReadWrite,
    IN  AMI_STORAGE_BUS_INTERFACE BusInterfaceType
);

EFI_STATUS
CommonWfccHook (
    IN  VOID                      *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE BusInterfaceType
);

VOID
EFIAPI
LocateAhciSmmServiceEvent (
    EFI_EVENT   Event,
    VOID        *Context
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
