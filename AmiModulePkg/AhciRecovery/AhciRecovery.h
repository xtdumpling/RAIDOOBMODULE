//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AhciRecovery.h
    Ahci Recovery function definition

**/
#ifndef _EFI_AHCI_RECOVERY_H_
#define _EFI_AHCI_RECOVERY_H_

#include    <Token.h>
#include    <AmiPeiLib.h>
#include    <AmiLib.h>
#include    <Ppi/DeviceRecoveryBlockIo.h>
#include    <Ppi/Stall.h>
#include    <Protocol/BlockIo.h>
#include    "AhciCtrl.h"
#include    <AmiCspLib.h>
#include    "AhciRecoveryElink.h"
#include    "Pci.h"
#include    <Library/PcdLib.h>
#include    <Library/PciLib.h>
#include    <Library/HobLib.h>
#include    <Ppi/AhciControllerRecovery.h>

#define AHCI_CLASS_CODE       0x01
#define AHCI_SUBCLASS_CODE    0x06
#define AHCI_HBA_PI           0x01
#define AHCI_PCI_MAX_SUBORDINATE_NUMBER 0x10
#define FIRST_SECONDARY_BUS_NUMBER      1

#define INVALID_VENDOR_ID              0x0FFFF
#define MASK_MEM_DECODE_RANGE          0x0FFFFFFF0
#define MASK_MEM_DECODE_RANGE_64BIT    0x0FFFFFFFFFFFFFFF0
#define AHCI_BAR_MASK                  0x0FFFFE000
#define CMD_ENABLE_MEM                 0x06
#define AHCI_DWORD_BAR_MASK            0xFFFFFFFF
#define AHCI_64BIT_BAR_MASK            0xFFFFFFFFFFFFFFFF
#define AHCI_MASK_MEM_BUS_MASTER       0xF9
#ifndef ABAR_OFFSET
#define ABAR_OFFSET                    PCI_BAR5 
#endif

EFI_STATUS
EFIAPI
Ahci_GetNumberOfBlockDevices (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    OUT UINTN                           *NumberBlockDevices
);

EFI_STATUS
EFIAPI
Ahci_GetBlockDeviceMediaInfo (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA          *MediaInfo
);

EFI_STATUS
EFIAPI
Ahci_ReadBlocks (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    IN  EFI_PEI_LBA                     StartLba,
    IN  UINTN                           BufferSize,
    OUT VOID                            *Buffer
);

VOID
AhciAtapiDetectMedia (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo
);

EFI_STATUS
AhciTestUnitReady (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo
);

EFI_STATUS
Ahci_AtapiReadSectors (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDevInfo,
    IN  OUT VOID                        *Buffer,
    IN  UINTN                           ByteCount,
    IN  UINT64                          LBA,
    IN  UINT32                          BlockSize
);

EFI_STATUS
DetectAtapiMedia (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo
);

EFI_STATUS
ExecutePacketCommand (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  COMMAND_STRUCTURE               *CommandStructure,
    IN  BOOLEAN                         READWRITE
);

EFI_STATUS
HandleAtapiError (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo
);

EFI_STATUS
EFIAPI
NotifyOnAhciRecoveryCapsuleLoaded (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);


EFI_STATUS
AhciEnumerateDevices (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV        *AhciRecInterface
);

#if OFFBOARD_AHCI_RECOVERY_SUPPORT
EFI_STATUS
EnumerateBus (
    IN  UINT8 Bus,
    IN  UINT32 BridgeBaseAddress
);

VOID
ProgramSubordinateBusForBridgeAbove (
    IN  UINT8   PrimaryBusNo,
    IN  UINT8   SubordinateBusNo
);

EFI_STATUS
CheckforProgrammedBridgeorDevice (
    IN  UINT8  Bus,
    IN  UINT8  Device,
    IN  UINT8  Function,
    IN  UINT8  SecondaryBusNo,
    IN  UINT8  BridgeAlreadyProgrammed,
    IN  UINT32 BridgeDevBaseAddress
);

EFI_STATUS
ProgramPciBridge (
    IN  UINT8   Bus,
    IN  UINT8   Device,
    IN  UINT8   Function,
    IN  UINT16  Address,
    IN  BOOLEAN IsMmio
);

BOOLEAN
CheckIsBridgeProgrammed(
IN  UINT8   Bus,
IN  UINT8   Device,
IN  UINT8   Function
);

EFI_STATUS
InitExtAhci (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV    *AhciRecInterface
);
#endif

EFI_STATUS
AhciRecInitController (
    IN  EFI_PEI_SERVICES            **PeiServices,
    AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    AHCI_PCI_DEVICE_INFO            *PciDeviceInfo
);
EFI_STATUS
AhciInitController (
    IN  EFI_PEI_SERVICES                  **PeiServices,
    IN  OUT AHCI_RECOVERY_CONTROLLER_INFO *AhciControllerInfo
);

EFI_STATUS
InitSBAhci (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV    *AhciRecInterface
);

VOID
PcdAhciRootBridgeResources(
    BOOLEAN Flag
);

UINT32
FindDeviceDecodeRange( 
    UINT8           Bus, 
    UINT8           Device, 
    UINT8           Function
);

VOID
FindPciMmioAddress(
    UINT64          *PciMmioStartAddress
);

EFI_STATUS
WaitForMemClear (
    IN  UINTN  BaseAddr,
    IN  UINT8  Port,
    IN  UINT8  Register,
    IN  UINT32 AndMask,
    IN  UINT32 WaitTimeInMs
);

EFI_STATUS
CheckDevicePresence (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort
);

EFI_STATUS
AhciDetectDevice (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV        *AhciRecInterface,
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort
);

EFI_STATUS
DetectAndConfigureDevice (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV        *AhciRecInterface,
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                            Port,
    IN  UINT8                            PMPort
);

EFI_STATUS
CheckPortImplemented (
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port
);

EFI_STATUS
CheckValidDevice (
    IN  AHCI_RECOVERY_CONTROLLER_INFO    *AhciControllerInfo,
    IN  UINT8                            Port,
    IN  UINT8                            PMPort
);

EFI_STATUS
WaitForMemSet (
    IN  UINTN   BaseAddr,
    IN  UINT8   Port,
    IN  UINT8   Register,
    IN  UINT32  AndMask,
    IN  UINT32  TestValue,
    IN  UINT32  WaitTimeInMs
);

EFI_STATUS
GetIdentifyData (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort,
    IN  ATA_IDENTIFY_DATA               *AtaIdentifyData
);

EFI_STATUS
ExecutePioDataCommand (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  OUT COMMAND_STRUCTURE           *CommandStructure,
    IN  BOOLEAN                         READWRITE
);

EFI_STATUS
WaitforCommandComplete (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  COMMAND_TYPE                    CommandType,
    IN  UINTN                           TimeOut
);

EFI_STATUS
BuildCommandFIS (
    IN  AHCI_RECOVERY_DEVICE_INFO        *AhciRecDeviceInfo,
    IN  COMMAND_STRUCTURE                CommandStructure,
    IN  AHCI_COMMAND_LIST                *CommandList,
    IN  AHCI_COMMAND_TABLE               *Commandtable
);

EFI_STATUS
BuildAtapiCMD(
    IN  AHCI_RECOVERY_DEVICE_INFO        *AhciRecDeviceInfo,
    IN  COMMAND_STRUCTURE                CommandStructure,
    IN  AHCI_COMMAND_LIST                *CommandList,
    IN  AHCI_COMMAND_TABLE               *Commandtable
);

EFI_STATUS
BuildPRDT (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  COMMAND_STRUCTURE               CommandStructure,
    IN  AHCI_COMMAND_LIST               *CommandList,
    IN  AHCI_COMMAND_TABLE              *Commandtable
);

EFI_STATUS
StartController (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  UINT32                          CIBitMask
);

EFI_STATUS
BuildCommandList (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  AHCI_COMMAND_LIST               *CommandList,
    IN  UINT32                          CommandTableBaseAddr
);

EFI_STATUS
ReadytoAcceptCmd (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo
);

EFI_STATUS
StopController (
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  BOOLEAN                         StartOrStop
);

VOID
CopyMemPei (
  IN    VOID   *DestinationBuffer,
  IN    VOID   *SourceBuffer,
  IN    UINTN  Length
);

EFI_STATUS
Ahci_AtaReadSectors (
    IN  AHCI_RECOVERY_DEVICE_INFO        *AhciRecDevInfo,    
    IN  OUT VOID                         *Buffer,
    IN  UINTN                            ByteCount,
    IN  UINT64                           LBA,
    IN  UINT32                           BlockSize
);

UINT32
ReadSCRRegister (
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort,
    IN  UINT8                           Register
);

EFI_STATUS
WriteSCRRegister (
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort,
    IN  UINT8                           Register,
    IN  UINT32                          Data32
);

EFI_STATUS
GeneratePortReset (
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    UINT8                               Port,
    UINT8                               PMPort,
    UINT8                               Speed,
    UINT8                               PowerManagement
);

EFI_STATUS
HandlePortComReset (
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort
);

#if RECOVERY_DATA_PORT_ACCESS
UINT32
ReadDataDword(
    IN  UINTN   BaseAddr,
    IN  UINTN   Index
);

UINT16
ReadDataWord(
    IN  UINTN   BaseAddr,
    IN  UINTN   Index
);

UINT8
ReadDataByte(
    IN  UINTN   BaseAddr,
    IN  UINTN   Index
);

VOID
WriteDataDword(
    IN  UINTN   BaseAddr,
    IN  UINTN   Index,
    IN  UINTN   Data
);

VOID
WriteDataWord (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index,
    IN  UINTN   Data
);

VOID
WriteDataByte (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index,
    IN  UINTN   Data
);
#endif

#endif //_EFI_AHCI_RECOVERY_H_

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
