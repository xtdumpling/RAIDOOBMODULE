//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CsmEdd.h
    EDD support definitions file

**/

#ifndef _CSM_EDD_H_
#define _CSM_EDD_H_

#pragma pack(1)

typedef struct {
    UINT8   Bus;
    UINT8   Device;
    UINT8   Function;
    UINT8   Controller;
    UINT32  Reserved;
} EDD_PCI;

typedef struct {
    UINT16  Base;
    UINT16  Reserved;
    UINT32  Reserved2;
} EDD_LEGACY;

typedef union {
    EDD_PCI     Pci;
    EDD_LEGACY  Legacy;
} EDD_INTERFACE_PATH;

typedef struct {
    UINT8   Master;
    UINT8   Reserved[15];
} EDD_ATA;

typedef struct {
    UINT8   Master;
    UINT8   Lun;
    UINT8   Reserved[14];
} EDD_ATAPI;

typedef struct {
    UINT16  TargetId;
    UINT64  Lun;
    UINT8   Reserved[6];
} EDD_SCSI;

typedef struct {
    UINT64   SerialNumber;
    UINT64   Reserved;
} EDD_USB;

typedef struct {
    UINT64  Guid;
    UINT64  Reserved;
} EDD_1394;

typedef struct {
    UINT64  Wwn;
    UINT64  Lun;
} EDD_FIBRE;

typedef union {
    EDD_ATA     Ata;
    EDD_ATAPI   Atapi;
    EDD_SCSI    Scsi;
    EDD_USB     Usb;
    EDD_1394    FireWire;
    EDD_FIBRE   FibreChannel;
} EDD_DEVICE_PATH;

typedef struct _UNKNOWN_VENDOR_DEVICE_PATH {
  VENDOR_DEVICE_PATH              DevicePath;
  UINT8                           LegacyDriveLetter;
} UNKNOWN_DEVICE_VENDOR_DEVICE_PATH;

/* defined in MdePkh
#define HARDWARE_DEVICE_PATH      0x01
#define HW_VENDOR_DP              0x04
#define MESSAGING_DEVICE_PATH     0x03
#define MSG_ATAPI_DP              0x01
#define HW_CONTROLLER_DP          0x05
#define MSG_SCSI_DP               0x02
#define MSG_1394_DP               0x04
#define MSG_FIBRECHANNEL_DP       0x03
*/
typedef struct {
    UINT16              StructureSize;
    UINT16              Flags;
    UINT32              MaxCylinders;
    UINT32              MaxHeads;
    UINT32              SectorsPerTrack;
    UINT64              PhysicalSectors;
    UINT16              BytesPerSector;
    UINT32              FDPT;
    UINT16              Key;
    UINT8               DevicePathLength;
    UINT8               Reserved1;
    UINT16              Reserved2;
    CHAR8               HostBusType[4];
    CHAR8               InterfaceType[8];
    EDD_INTERFACE_PATH  InterfacePath;
    EDD_DEVICE_PATH     DevicePath;
    UINT8               Reserved3;
    UINT8               Checksum;
} EDD_DRIVE_PARAMETERS;

// Flag definitions for above
#define EDD_GEOMETRY_VALID          0x02
#define EDD_DEVICE_REMOVABLE        0x04
#define EDD_WRITE_VERIFY_SUPPORTED  0x08
#define EDD_DEVICE_CHANGE           0x10
#define EDD_DEVICE_LOCKABLE         0x20

// For WIN98 limitation
#define EDD_DEVICE_GEOMETRY_MAX     0x40


/**
    Device address packet used during EDD data transfers

**/

typedef struct {
    UINT8   PacketSizeInBytes; ///< 0x18
    UINT8   Zero;
    UINT8   NumberOfBlocks; ///< # of blocks to transfer, Max 0x7f
    UINT8   Zero2;
    UINT32  SegOffset;  ///< Data address below 1MB
    UINT64  LBA;        ///< Device Logical Block Address
    UINT64  TransferBuffer;     ///< Transfer Buffer
    UINT32  ExtendedBlockCount; ///< Transferred blocks counter, Max 0xffffffff
    UINT32  Zero3;
} EDD_DEVICE_ADDRESS_PACKET;

#define UNKNOWN_DEVICE_GUID \
  { 0xcf31fac5, 0xc24e, 0x11d2,  0x85, 0xf3, 0x0, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b  }

#define EDD_VERSION_30  0x30

// Int 13 Error Messages
#define ERR_PASS                   0x00
#define ERR_WRITE_PROTECTED        0x03
#define ERR_SECTOR_NOT_FOUND       0x04
#define ERR_RESET_FAILED           0x05
#define ERR_DISK_CHANGED           0x06
#define ERR_DRIVE_DOES_NOT_EXIST   0x07
#define ERR_DMA_ERROR              0x08
#define ERR_DATA_BOUNADRY_ERROR    0x09
#define ERR_BAD_SECTOR             0x0a
#define ERR_BAD_TRACK              0x0b
#define ERR_MEDIA_TYPE_NOT_FOUND   0x0c
#define ERR_INVALID_FORMAT         0x0d
#define ERR_ECC_ERROR              0x10
#define ERR_ECC_CORRECTED_ERROR    0x11
#define ERR_HARD_DRIVE_FAILURE     0x20
#define ERR_SEEK_FAILED            0x40
#define ERR_DRIVE_TIMEOUT          0x80
#define ERR_DRIVE_NOT_READY        0xaa
#define ERR_UNDEFINED_ERROR        0xbb
#define ERR_WRITE_FAULT            0xcc
#define ERR_SENSE_FAILED           0xff

#define MAX_EDD11_XFER  0xfe00

// PCI classes not defined in PCI.h
#define PCI_BASE_CLASS_INTELLIGENT  0x0e
#define PCI_SUB_CLASS_INTELLIGENT   0x00

//#define EFI_SEGMENT(_Adr) (UINT16)((UINT16) (((UINTN)(_Adr)) >> 4) & 0xf000)
//#define EFI_OFFSET(_Adr)  (UINT16)(((UINT16)((UINTN)_Adr)) & 0xffff)

#pragma pack()

// Local data structures

/**
    Hard drive data structure

**/

typedef struct {
    CHAR8                   Letter;     ///< Drive letter
    UINT8                   Number;     ///< INT13 drive handle
    UINT8                   EddVersion; ///< EDD version supported by the drive
    BOOLEAN                 ExtendedInt13;  ///< Extended INT13 support status
    BOOLEAN                 DriveLockingAndEjecting;    ///< Locking/Ejecting support
    BOOLEAN                 Edd;    ///< EDD support status
    BOOLEAN                 Extensions64Bit;    ///< 64 bit extension support
    BOOLEAN                 ParametersValid;    ///< Valid parameters (Parameters field) indicator
    UINT8                   ErrorCode;      ///< Error code field
    VOID                    *FdptPointer;   ///< Pointer to FDPT
    BOOLEAN                 Floppy;     ///< Device-is-a-floppy indicator
    BOOLEAN                 AtapiFloppy;    ///< Device-is-an-ATAPI-floppy indicator
    UINT8                   MaxHead;    ///< Number of heads
    UINT8                   MaxSector;  ///< Number of sectors
    UINT16                  MaxCylinder;    ///< Number of cylinders
    UINT16                  Pad;
    EDD_DRIVE_PARAMETERS    Parameters;
} CSM_LEGACY_DRIVE;


/**
    CSM BlockIo device properies

**/

typedef struct {
    EFI_BLOCK_IO_PROTOCOL       BlockIo;            ///< BlockIo protocol instance
    EFI_HANDLE                  Handle;             ///< EFI device handle
    EFI_HANDLE                  ControllerHandle;   ///< EFI controller handle
    EFI_BLOCK_IO_MEDIA          BlockMedia;         ///< BlockIo device media
    EFI_DEVICE_PATH_PROTOCOL    *DevicePath;        ///< Device path
    EFI_PCI_IO_PROTOCOL         *PciIo;             ///< Device PciIo
    EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;        ///< Legacy Bios instance pointer
    CSM_LEGACY_DRIVE            Drive;              ///< Drive data pointer
    UINT8                       HwInt;              ///< Hardware interrupt used by this device
    UINT32                      HwIntHandler;       ///< Storage for the original HW interrupt
    EFI_EVENT                   Legacy2EfiEvent;
    UINT16                      BcvSegment;
    UINT16                      BcvOffset;
} CSM_BLOCK_IO_DEV;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
