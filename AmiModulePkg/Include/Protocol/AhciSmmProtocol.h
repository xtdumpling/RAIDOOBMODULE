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

/** @file AhciSmmProtocol.h
    Protocol definition for Ahci SMM 

**/

#ifndef _AMI_AHCI_SMM_PROTOCOLS_H_
#define _AMI_AHCI_SMM_PROTOCOLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/AmiAhciBus.h>

#define AMI_AHCI_SMM_PROTOCOL_GUID \
        {0xB2FA5764, 0x3B6E, 0x43D3, { 0x91, 0xDF, 0x87, 0xD1, 0x5A, 0x3E, 0x56, 0x68 } }

typedef struct _AMI_AHCI_BUS_SMM_PROTOCOL AMI_AHCI_BUS_SMM_PROTOCOL;


/**
    Initilize the Sata port on S3 resume

    @param SataDevInterface 
    @param Port,

    @retval 
        EFI_STATUS

**/ 
typedef 
EFI_STATUS 
(EFIAPI *AMI_AHCI_SMM_INIT_ON_S3) (
    IN AMI_AHCI_BUS_SMM_PROTOCOL                *SataDevInterface, 
    IN UINT8                                    Port
);


/**
    Execute the PIO data command
                   
    @param SataDevInterface 
    @param CommandStructure 
    @param PortNumber,
    @param PMPortNumber, 
    @param DeviceType,
    @param READWRITE 

    @retval 
        EFI_STATUS

**/ 
typedef 
EFI_STATUS
(EFIAPI *AMI_AHCI_SMM_SATA_DEV_PIO_DATA_IN) (
    IN AMI_AHCI_BUS_SMM_PROTOCOL                *SataDevInterface, 
    IN OUT COMMAND_STRUCTURE                    *CommandStructure,
    UINT8                                       PortNumber,
    UINT8                                       PMPortNumber, 
    DEVICE_TYPE                                 DeviceType,
    IN BOOLEAN                                  READWRITE
);

/**
    Execute the DMA data command
                   
    @param SataDevInterface 
    @param CommandStructure 
    @param PortNumber,
    @param PMPortNumber, 
    @param DeviceType,
    @param READWRITE 

    @retval 
        EFI_STATUS

**/ 
typedef 
EFI_STATUS
(EFIAPI *AMI_AHCI_SMM_SATA_DEV_DMA_DATA_IN) (
    IN AMI_AHCI_BUS_SMM_PROTOCOL                *SataDevInterface, 
    IN OUT COMMAND_STRUCTURE                    *CommandStructure,
    UINT8                                       PortNumber,
    UINT8                                       PMPortNumber, 
    DEVICE_TYPE                                 DeviceType,
    IN BOOLEAN                                  READWRITE
);


/**
    Execute the Non Data command 

    @param SataDevInterface 
    @param CommandStructure 
    @param PortNumber,
    @param PMPortNumber, 
    @param DeviceType,

    @retval 
        EFI_STATUS

**/ 
typedef 
EFI_STATUS 
(EFIAPI *AMI_AHCI_SMM_SATA_DEV_NON_DATA_CMD) (
    IN AMI_AHCI_BUS_SMM_PROTOCOL                *SataDevInterface, 
    IN COMMAND_STRUCTURE                        CommandStructure,
    UINT8                                       PortNumber,
    UINT8                                       PMPortNumber, 
    DEVICE_TYPE                                 DeviceType
);

/**
    Execute a Atapi Packet command
                   
    @param SataDevInterface 
    @param CommandStructure 
    @param READWRITE
    @param PortNumber,
    @param PMPortNumber, 
    @param DeviceType

    @retval 
        EFI_STATUS

    @note  
  1. Stop the Controller
  2. Check if the device is ready to accept a Command. 
  3. Build Command list
  4. Start the Controller.
  5. Wait till command completes. Check for errors.

**/ 
typedef 
EFI_STATUS 
(EFIAPI *AMI_AHCI_SMM_SATA_DEV_PACKET_CMD) (
    IN AMI_AHCI_BUS_SMM_PROTOCOL                *SataDevInterface, 
    IN COMMAND_STRUCTURE                        *CommandStructure,
    IN BOOLEAN                                  READWRITE,
    UINT8                                       PortNumber,
    UINT8                                       PMPortNumber, 
    DEVICE_TYPE                                 DeviceType
);

struct _AMI_AHCI_BUS_SMM_PROTOCOL{
    UINT32                                      AhciBaseAddress;
    UINT64                                      PortCommandTableBaseAddr;
    UINT64                                      PortCommandListBaseAddr;
    UINT64                                      PortFISBaseAddr;
    UINT8                                       PortNumber;
    UINT8                                       PMPortNumber; 
    DEVICE_TYPE                                 DeviceType;
    ATAPI_DEVICE                                AtapiDevice;
    AMI_AHCI_SMM_INIT_ON_S3                     AhciSmmInitPortOnS3Resume;
    AMI_AHCI_SMM_SATA_DEV_PIO_DATA_IN           AhciSmmExecutePioDataCommand;
    AMI_AHCI_SMM_SATA_DEV_DMA_DATA_IN           AhciSmmExecuteDmaDataCommand;
    AMI_AHCI_SMM_SATA_DEV_NON_DATA_CMD          AhciSmmExecuteNonDataCommand;
    AMI_AHCI_SMM_SATA_DEV_PACKET_CMD            AhciSmmExecutePacketCommand;
};

extern  EFI_GUID    gAmiAhciSmmProtocolGuid;

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
