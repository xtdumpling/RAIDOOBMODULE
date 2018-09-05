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

/** @file AhciInt13Smm.h
    This file contains the definitions of function prototype, constant and 
    data structure for AhciInt13Smm module
**/


#ifndef __AINT13_SMM_HEADER__
#define __AINT13_SMM_HEADER__

#define AHCI_INT13_SMM_DATA_GUID \
    { 0xF4F63525, 0x281E, 0x4040, 0xA3, 0x13, 0xC1, 0xD6, 0x76, 0x63, 0x84, 0xBE }

// PciExpressBaseAddress uses PCD data
#define PCI_CFG_ADDR(bus,dev,func,reg) \
    ((VOID*)(UINTN) (PciExpressBaseAddress + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

#define     AHCI_CONTROLLER_SCC     0x06
#define     RAID_CONTROLLER_SCC     0x04
#define     MASS_STORAGE            0x01
#define     PCI_ABAR                0x24
#define     HDD_BLOCK_SIZE          512

#define     READ_SECTORS            0x20
#define     READ_SECTORS_EXT        0x24
#define     READ_MULTIPLE           0xC4
#define     READ_MULTIPLE_EXT       0x29
#define     WRITE_SECTORS           0x30
#define     WRITE_SECTORS_EXT       0x34
#define     WRITE_MULTIPLE          0xC5
#define     WRITE_MULTIPLE_EXT      0x39
#define     READ_DMA                0xC8
#define     READ_DMA_EXT            0x25
#define     WRITE_DMA               0xCA
#define     WRITE_DMA_EXT           0x35

// Int13 parameter definition
// function(AH) definition
#define     READ_SECTOR             0x02
#define     WRITE_SECTOR            0x03
#define     EXT_READ                0x42
#define     EXT_WRITE               0x43

// Prevent compiler from padding the structures
#pragma pack(1)

typedef struct {
    DLINK     dLink;
    UINT8     DriveNum;       // Int13's Drive Number(dl)
    UINT8     PMPortNum;      // PMPort number on HBA
    UINT8     PortNum;        // Port number on HBA
    UINT8     BusNo;          // Bus# of Controller  
    UINT8     DevNo;          // Device# of Controller  
    UINT8     FuncNo;         // Function# of Controller  
    UINT8     DeviceType;     // 0 ATA, 1 ATAPI
    UINT8     Lun;            // ATAPI LUN
    UINT16    BlockSize;      // ATAPI Block Size
    UINT16    wMAXCYL;        // maximum no. of cylinders..INT13 interface. (logical)
    UINT8     bMAXHN;         // maximum no. of heads..INT13 interface. (logical)
    UINT8     bMAXSN;         // no. of sectors per track..INT13 interface. (logical)
    UINT16    wLBACYL;        // #of cylinders drive is configured for. (physical)
    UINT8     bLBAHD;         // #of heads drive is configured for. (physical)
    UINT8     bLBASPT;        // #of sectors per track drive is configured for. (physical)
    UINT8     WCommand;       // write command
    UINT8     RCommand;       // read command
} SMM_AINT13_DRIVE_INFO;

typedef struct {
    UINT8                  DriveCount;
    SMM_AINT13_DRIVE_INFO  DriveInfo[32];       // Sync array length with SATA_PORT_COUNT of Aint13.h
} AHCI_INT13_SMM_DATA;

typedef struct {
    UINT8       PackageSize;    // size of package(10h or 18h)
    UINT8       Reserved;       // reserved
    UINT16      XferSector;     // transfer sectors
    UINT32      Buffer;         // 32 bit address transfer buffer
    UINT64      StartLba;       // Start LBA sectors;
    UINT64      Buffer64;       // 64 bit address transfer buffer(option);
} DISK_ADDRESS_PACKAGE;

// DO NOT MODIFY BELOW STRUCTURE
// NOTE: If modified, AINT13.ASM (of AI13.bin) also needs to be modified
typedef struct {
    UINT32          StackEDI;
    UINT32          StackESI;
    UINT32          StackEBP;
    UINT32          StackESP;
    UINT32          StackEBX;
    UINT32          StackEDX;
    UINT32          StackECX;
    UINT32          StackEAX;
    UINT16          StackDS;
    UINT16          StackES;  
    UINT16          StackGS;
    UINT16          StackFS;
    EFI_FLAGS_REG   StackFlags;
} INT13_TO_SMI_EXREGS;

#pragma pack()

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
