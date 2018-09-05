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

/** @file AmiAhciBusProtocol.h
    AMI defined Protocol header file for the SATA Controllers in AHCI mode
 **/

#ifndef _AMI_AHCIBUS_PROTOCOL_H
#define _AMI_AHCIBUS_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include <Library/UefiLib.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#include <Protocol/AmiHddSecurity.h>
#include <Protocol/AmiHddSmart.h>
#include <Protocol/AmiHddPowerMgmt.h>
#include <Protocol/StorageSecurityCommand.h>
#include <Protocol/BlockIo.h>
#include <Protocol/PciIo.h>

//---------------------------------------------------------------------------

// Forward reference for pure ANSI compatibility
typedef struct _SATA_DEVICE_INTERFACE SATA_DEVICE_INTERFACE;
typedef struct _AMI_AHCI_BUS_PROTOCOL AMI_AHCI_BUS_PROTOCOL;
typedef struct _AHCI_ATAPI_COMMAND AHCI_ATAPI_COMMAND;

typedef enum {  
  NON_DATA_CMD = 0,
  PIO_DATA_IN_CMD = 1,
  PIO_DATA_OUT_CMD = 2,
  DMA_DATA_IN_CMD = 3,
  DMA_DATA_OUT_CMD = 4,
  PACKET_PIO_DATA_IN_CMD = 5,
  PACKET_PIO_DATA_OUT_CMD = 6,
  PACKET_DMA_DATA_IN_CMD = 7,
  PACKET_DMA_DATA_OUT_CMD = 8,
} COMMAND_TYPE;

/*
 * AHCI Specification Defined Structures.
 */

#pragma pack(1)

typedef struct {
  DLIST    AhciControllerList;
  DLINK    AhciControllerLink;
} AHCI_CONTOLLER_LINKED_LIST;

struct _AHCI_ATAPI_COMMAND{
    UINT8               Ahci_Atapi_Command[0x10];
};

typedef struct {
    VOID                 *Buffer;
    UINT32                ByteCount;
    UINT8                 Features;
    UINT8                 FeaturesExp;
    UINT16                SectorCount;
    UINT8                 LBALow;
    UINT8                 LBALowExp;
    UINT8                 LBAMid;
    UINT8                 LBAMidExp;
    UINT8                 LBAHigh;
    UINT8                 LBAHighExp;
    UINT8                 Device;
    UINT8                 Command;
    UINT8                 Control;
    UINT64                Timeout;
    AHCI_ATAPI_COMMAND    AtapiCmd;
} COMMAND_STRUCTURE;

#pragma pack()

/*
 * Ami Ahci Bus Protocol Definition
 */

/**
    Issues Read/Write Command and Read/Write the data from/to the ATA device

    @param    IdeBusInterface 
    @param    Buffer 
    @param    ByteCount
    @param    LBA
    @param    ReadWriteCommand 
    @param    ReadWrite Read/Write = 0/1

    @retval   *Buffer, EFI_STATUS

    @note  
          1. Get the Max. number of sectors that can be Read/written in one Read/Write PIO command
          2. Update the Command Structure
          3. Issue ExecutePioDataCommand.
          4. If all the bytes are read exit else goto step 2 

**/

typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_DEV_RAED_WRITE_PIO) (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN OUT VOID                 *Buffer,
    IN UINTN                    ByteCount,
    IN UINT64                   LBA,
    IN UINT8                    ReadWriteCommand,
    IN BOOLEAN                  READWRITE
);

/**
    Execute PIO Data In/Out command
    
    @param    SataDevInterface 
    @param    CommandStructure 
    @param    READWRITE

    @retval    EFI_STATUS, CommandStructure->ByteCount 

    @note  
          1. Stop the Controller
          2. Check if the device is ready to accept a Command. 
          3. Build Command list
          4. Start the Controller.
          5. Wait till command completes. Check for errors.

**/ 

typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_DEV_PIO_DATA_IN) (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN OUT COMMAND_STRUCTURE    *CommandStructure,
    IN BOOLEAN                  READWRITE 
);

/**
    Issues Read/Write Command with SubCommand feature
    and Reads/Writes data to the ATA device.
        
    @param    SataDevInterface 

    @param    CommandStructure


    @param    READWRITE

    @retval    EFI_STATUS

    @note  
      1. Get the Max. number of sectors that can be transferred in one Read/Write PIO command
      2. Update the Command Structure
      3. Issue ExecutePioDataCommand.

**/

typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_DEV_PIO_DATA_OUT) (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN OUT COMMAND_STRUCTURE    CommandStructure,
    IN BOOLEAN                  READWRITE
);

/**
    Issue a  Non-Data command to the SATA Device
                   
    @param    SataDevInterface 
    @param    CommandStructure 

    @retval   EFI_STATUS     

    @note  
          1. Stop the Controller
          2. Check if the device is ready to accept a Command. 
          3. Build Command list
          4. Start the Controller.
          5. Wait till command completes. Check for errors.

**/ 

typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_DEV_NON_DATA_CMD) (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        CommandStructure
);

/**
    Execute the ATA/ATAPI commands in the DMA mode
                   
    @param  SataDevInterface 
    @param  CommandStructure 
    @param  READWRITE

    @retval EFI_STATUS

    @note  
      1. Stop the Controller
      2. Check if the device is ready to accept a Command. 
      3. Build Command list
      4. Start the Controller.
      5. Wait till command completes. Check for errors.

**/ 
typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_DEV_DMA_DATA_CMD) (
    IN SATA_DEVICE_INTERFACE                *SataDevInterface, 
    IN OUT COMMAND_STRUCTURE                *CommandStructure,
    IN BOOLEAN                              READWRITE

);

/**
    Wait till command completes 

    @param    SataDevInterface 
    @param    CommandType 
    @param    TimeOut 

    @retval   EFI_STATUS

    @note  
          1. Check for SError bits. If set return error.
          2. For PIO IN/Out and Packet IN/OUT command wait till PIO Setup FIS is received
          3. If D2H register FIS is received, exit the loop.
          4. Check for SError and TFD bits.

**/

typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_DEV_WAIT_FOR_CMD_COMPLETE) (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN COMMAND_TYPE             CommandType,
    IN UINTN                    TimeOut    
);

/**
    Issue a Port Reset

    @param    AhciBusInterface 
    @param    SataDevInterface 
    @param    CurrentPort 
    @param    Speed 
    @param    PowerManagement 

    @retval   EFI_STATUS

    @note  
          1. Issue port reset by setting DET bit in SControl register
          2. Call HandlePortComReset to check the status of the reset.

**/ 
typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_GENERATE_PORT_RESET) (
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    SATA_DEVICE_INTERFACE    *SataDevInterface, 
    UINT8                    Port,
    UINT8                    PMPort,
    UINT8                    Speed,
    UINT8                    PowerManagement   
);

/**
    Generate Soft Reset

    @param SataDevInterface 
    @param In UINT8                                PMPort

    @retval EFI_STATUS

    @note  
  1. Issue a Control register update, H2D register FIS with reset bit set.
  2. Wait for 100usec
  3. Issue a Control register update, H2D register FIS with reset bit reset.

**/ 
typedef 
EFI_STATUS 
(EFIAPI *AMI_SATA_GENERATE_PORT_SOFT_RESET) (
    SATA_DEVICE_INTERFACE               *SataDevInterface, 
    UINT8                               PMPort
);


/**
    Execute a Atapi Packet command
                   
    @param    SataDevInterface 
    @param    CommandStructure 
    @param    READWRITE

    @retval    EFI_STATUS

    @note  
          1. Stop the Controller
          2. Check if the device is ready to accept a Command. 
          3. Build Command list
          4. Start the Controller.
          5. Wait till command completes. Check for errors.

**/ 

typedef 
EFI_STATUS 
(EFIAPI *AMI_EXECUTE_PACKET_COMMAND) (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        *CommandStructure,
    IN BOOLEAN                  READWRITE
);

struct _AMI_AHCI_BUS_PROTOCOL{
    EFI_HANDLE                          ControllerHandle;
    UINT64                              AhciBaseAddress;
    UINT32                              AhciVersion;
    UINT32                              HBACapability;
    UINT32                              HBAPortImplemented;        // Bit Map
    UINT64                              PortCommandListBaseAddr;
    UINT32                              PortCommandListLength;
    UINT64                              PortCommandTableBaseAddr;
    UINT32                              PortCommandTableLength;
    UINT64                              PortFISBaseAddr;
    UINT64                              PortFISBaseAddrEnd;
    DLIST                               SataDeviceList;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePathProtocol;
    EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface;
    EFI_PCI_IO_PROTOCOL                 *PciIO;
    UINT64                              PortFISBaseAddress;                   // Unmodified PortFISBaseAddr
    UINT64                              PortCommandListBaseAddress;           // Unmodified PortCommandListBaseAddr
    UINT8                               NumberofPortsImplemented;   // 1 based Count
    BOOLEAN                             AHCIRAIDMODE;				// Set to TRUE in AHCI mode, FALSE in RAID mode

    AMI_SATA_DEV_RAED_WRITE_PIO         SataReadWritePio;
    AMI_SATA_DEV_PIO_DATA_IN            ExecutePioDataCommand;
    AMI_SATA_DEV_PIO_DATA_OUT           SataPioDataOut;
    AMI_SATA_DEV_NON_DATA_CMD           ExecuteNonDataCommand;
    AMI_SATA_DEV_DMA_DATA_CMD           ExecuteDmaDataCommand;
    AMI_SATA_DEV_WAIT_FOR_CMD_COMPLETE  WaitforCommandComplete;
    AMI_SATA_GENERATE_PORT_RESET        GeneratePortReset;
    AMI_SATA_GENERATE_PORT_SOFT_RESET   GeneratePortSoftReset;
    AMI_EXECUTE_PACKET_COMMAND          ExecutePacketCommand;

    BOOLEAN                             Acoustic_Enable;            // Acoustic Support
    UINT8                               Acoustic_Management_Level;  // Acoustic Level
    UINT8                               DiPM;
    UINT16                              PrevPortNum;
    UINT16                              PrevPortMultiplierPortNum;

};

/*
 * AHCI Platform Policy Protocol Definition
*/

typedef struct {
    BOOLEAN         RaidDriverMode;               // Set to TRUE For UEFI Raid driver and FALSE for Legacy Raid option ROM 
    BOOLEAN         AhciBusAtapiSupport;          // Set to FALSE For UEFI Raid driver and TRUE for Legacy Raid option ROM 
    BOOLEAN         DriverLedOnAtapiEnable;       // Set to TRUE to Enable the Drive LED on ATAPI Enable (DLAE) bit
    BOOLEAN         PowerUpInStandbySupport;      // Set to TRUE to Support PUIS.
    BOOLEAN         PowerUpInStandbyMode;         // Set to TRUE to Enable PUIS.
    BOOLEAN         DipmSupport;                  // Set to TRUE to Support the Device initiated power management.
    BOOLEAN         DipmEnable;                   // Set to TRUE to Enable the Device initiated power management.
    BOOLEAN         DeviceSleepSupport;           // Set to TRUE to Support the Device Sleep
    BOOLEAN         DeviceSleepEnable;            // Set to TRUE to Enable the Device Sleep
    BOOLEAN         PciMapAddressForDataTransfer; // Set to True to use PCIIO mapped address for DMA or PIO data transfer
} AHCI_PLATFORM_POLICY_PROTOCOL;

/*
 * Ami Ahci Data Structures used AHCI driver and other Hdd Feature related drivers
*/

typedef struct _SATA_DISK_INFO { 
    EFI_DISK_INFO_PROTOCOL              DiskInfo;               // should be the first Entry
    SATA_DEVICE_INTERFACE               *SataDevInterface;
} SATA_DISK_INFO;

typedef struct _SATA_BLOCK_IO { 
    EFI_BLOCK_IO_PROTOCOL               BlkIo;                  // should be the first Entry
    SATA_DEVICE_INTERFACE               *SataDevInterface;
} SATA_BLOCK_IO;

struct _SATA_DEVICE_INTERFACE{
    EFI_HANDLE                            IdeDeviceHandle;
    UINT8                                 PortNumber;
    UINT8                                 PMPortNumber; 
    UINT8                                 NumPMPorts;         // Number of Ports in PM, Valid for PMPORT only
    UINT8                                 DeviceState;
    BOOLEAN                               IsDeviceFeatureDone;
    UINT32                                Signature;
    UINT32                                SControl;
    DEVICE_TYPE                           DeviceType;

    UINT8                                 PIOMode;
    UINT8                                 SWDma;
    UINT8                                 MWDma;
    UINT8                                 UDma;
    UINT8                                 IORdy;
    UINT8                                 ReadCommand;
    UINT8                                 WriteCommand;
    IDENTIFY_DATA                         IdentifyData;
    EFI_UNICODE_STRING_TABLE              *UDeviceName;
    ATAPI_DEVICE                          *AtapiDevice;
    UINT8                                 AtapiSenseData[256];
    UINT8                                 AtapiSenseDataLength;

    UINT64                                PortCommandListBaseAddr;
    UINT64                                PortFISBaseAddr;

    AMI_AHCI_BUS_PROTOCOL                 *AhciBusInterface;
    EFI_DEVICE_PATH_PROTOCOL              *DevicePathProtocol; 
    SATA_BLOCK_IO                         *SataBlkIo;
    SATA_DISK_INFO                        *SataDiskInfo;
    AMI_HDD_SECURITY_PROTOCOL             *IdeSecurityInterface;
    AMI_HDD_SMART_PROTOCOL                *SmartInterface;
    AMI_HDD_POWER_MGMT_PROTOCOL           *PowerMgmtInterface;
    EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *StorageSecurityInterface;
    VOID                                  *OpalConfig;               // Pointer to OPAL_DEVICE


    DLINK                                 SataDeviceLink; 
    DLIST                                 PMSataDeviceList;         
    DLINK                                 PMSataDeviceLink; 
};

extern EFI_GUID gAmiAhciBusProtocolGuid;
extern EFI_GUID gAmiAhciPlatformPolicyProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

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
