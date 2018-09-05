//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file UsbMass.h
    AMI USB Mass Storage support header

**/

#ifndef __USBMASS__H__
#define __USBMASS__H__
#ifdef __cplusplus
extern "C" {
#endif

/**
    USB mass device boot only protocol status block structure.
    Referred as CSW - Command status wrapper. Refer Bulk-Only
    transport specification for more detail.

 Fields:   Name       Type    Description
      ------------------------------------------------------------
      dCswSignature   DWORD   CBS signature "USBS"
      dCswTag     DWORD   Tag used to link the command with status response
      dCswDataResidue DWORD   Size of remaining data that is not processed in this transfer
      bmCswStatus BYTE    CSW status byte

**/
#pragma pack(push, 1)

typedef struct {
    UINT32      dCswSignature;      // 0-3h, CSW signature = "USBS"
    UINT32      dCswTag;            // 4-7h, Tag
    UINT32      dCswDataResidue;    //  8-0Bh, Data residue
    UINT8       bmCswStatus;        // 0Ch, CSW status
} BOT_STATUS_BLOCK;

#pragma pack(pop)

// Common device specific command structures and values
//----------------------------------------------------------------------------
#define COMMON_INQUIRY_OPCODE                           0x12
#define COMMON_READ_CAPACITY_10_OPCODE                  0x25
#define COMMON_READ_CAPACITY_16_OPCODE                  0x9E
#define COMMON_READ_10_OPCODE                           0x28
#define COMMON_READ_16_OPCODE                           0x88
#define COMMON_WRITE_10_OPCODE                          0x2A
#define COMMON_WRITE_16_OPCODE                          0x8A
#define COMMON_MODE_SENSE_6_OPCODE                      0x1A
#define COMMON_MODE_SENSE_10_OPCODE                     0x5A
#define COMMON_SEND_DIAGNOSTIC_OPCODE                   0x1D
#define COMMON_TEST_UNIT_READY_OPCODE                   0x00
#define COMMON_REQUEST_SENSE_OPCODE                     0x03
#define COMMON_START_STOP_UNIT_OPCODE                   0x1B
#define COMMON_FORMAT_UNIT_OPCODE                       0x04
#define COMMON_VERIFY_10_OPCODE                         0x2F
#define COMMON_VERIFY_16_OPCODE                         0x8F
#define COMMON_READ_FORMAT_CAPACITY_OPCODE              0x23
#define COMMON_GET_CONFIGURATION_OPCODE                 0x46

#pragma pack(push, 1)

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT8           bPageCode;      // 02h, Page Code
    UINT8           bReserved;      // 03h, Reserved
    UINT8           bAllocLength;   // 04h, Allocation Length
    UINT8           bControl;       // 05h, Control/Reserved
    UINT8           aReserved[6];   // 06h-0Bh, Reserved
} COMMON_INQ_CMD;

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code
    UINT8           bLun;       // 01h, Logical Unit Number, etc
    UINT32          dLba;       // 02h-05h, Logical Block Address
    UINT16          wbReserved; // 06h-07, Reserved
    UINT8           bPMI;       // 08h, PMI - bit 0
    UINT8           bControl;   // 09h, Control/Reserved
    UINT16          wdReserved; // 0Ah-0Bh, Reserved
} COMN_READ_CAPACITY_10_CMD;

typedef struct {
    UINT8           OpCode;         // 00h, Operation Code
    UINT8           ServiceAction;  // 01h, Service Action - bit0..bit4
    UINT64          Lba;            // 02h-09h, Logical Block Address
    UINT32          AllocLength;    // 10h-0Dh, Allocation Length
    UINT8           PMI;            // 0Eh, PMI - bit 0
    UINT8           Control;        // 0Fh, Control
} COMN_READ_CAPACITY_16_CMD;

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT8           aReserved[5];   // 02-06h, Reserved
    UINT16          wAllocLength;   // 07h-08h, Allocation Length(BIG ENDIAN)
    UINT8           aReserved1[3];  // 09h-0Bh, Reserved
} COMN_READ_FMT_CAPACITY;

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT32          dLba;           // 02h-05h, Logical Block Address
    UINT8           bReserved;      // 06h, Reserved
    UINT16          wTransferLength;// 07h-08h, Transfer Length(BIG ENDIAN)
    UINT8           bControl;       // 09h, Control/Reserved
    UINT16          wReserved;      // 0Ah-0Bh, Reserved
} COMN_RWV_10_CMD;

typedef struct {
    UINT8           OpCode;             // 00h, Operation Code
    UINT8           Lun;                // 01h, Logical Unit Number, etc
    UINT64          Lba;                // 02h-09h, Logical Block Address
    UINT32          TransferLength;     // 0Ah-0Dh, Transfer Length(BIG ENDIAN)
    UINT8           GroupNum;           // 0Eh, Group Number, etc
    UINT8           Control;            // 0Fh, Control
} COMN_RWV_16_CMD;

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT8           bPageCode;      // 02h, Page Code
    UINT8           bReserved;      // 03h, Reserved
    UINT8           bAllocLength;   // 04h, Allocation Length
    UINT8           bControl;       // 05h, Control/Reserved
} COMN_MODE_SENSE_6CMD;

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT8           bPageCode;      // 02h, Page Code
    UINT32          dReserved;      // 03-06h, Reserved
    UINT16          wAllocLength;   // 07h-08h, Allocation Length(BIG ENDIAN)
    UINT8           bControl;       // 09h, Control/Reserved
    UINT16          wReserved;      // 0Ah-0Bh, Reserved
} COMN_MODE_SENSE_10CMD;

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code (1D)
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT8           bReserved[10];  // 02h-0Bh, Reserved
} COMN_SEND_DIAG_CMD;

typedef struct {
    UINT8           bOpCode;            // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number
    UINT8           bReserved[10];  // 02h-0Bh, Reserved
} COMN_TEST_UNIT_READY_CMD;

typedef struct {
    UINT8           bOpCode;        //00h, Operation Code
    UINT8           bLun;           //01h, Logical Unit Number
    UINT16          wReserved;      //02-03h, Reserved
    UINT8           bAllocLength;   //04h, Allocation Length
    UINT8           aReserved[7];   //05h-0Bh, Reserved
} COMMON_REQ_SENSE_CMD;

typedef struct {
    UINT8           bOpCode;            // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT16          wReserved;      // 02-03h, Reserved
    UINT8           bStart;         // 04h, LoEj, Start bits
    UINT8           aReserved[7];   // 05h-0Bh, Reserved
} COMMON_START_STOP_UNIT_CMD;

typedef struct {
    UINT8           OpCode;                 // 00h, Operation Code
    UINT8           Rt;                     // 01h, Logical Unit Number, etc
    UINT16          StartingFeatureNumber;  // 02h, StartingFeatureNumber
    UINT8           Reserved[3];            // 03-06h, Reserved
    UINT16          AllocLength;            // 07h, Allocation Length
    UINT8           Control;                // 09h, Control
} COMMON_GET_CONFIGURATION;

typedef struct {
    UINT8           bOpCode;        // 00h, Operation Code
    UINT8           bLun;           // 01h, Logical Unit Number, etc
    UINT8           bTrackNumber;   // 02h, Track Number
    UINT16          wInterleave;    // 03h-04h, Interleave (BIG ENDIAN)
    UINT16          wReserved;      // 05-06h, Reserved
    UINT16          wParamLength;   // 07h-08h, Parameter List Length(BIG ENDIAN)
    UINT8           aReserved[3];   // 09h-0Bh, Reserved
} UFI_FMT_UNIT_CMD;

typedef struct {
    UINT16          wLength;
    UINT8           bMediaType;
    UINT8           aReserved[3];
    UINT16          wBlkDescSize;
} MODE_SENSE_10_HEADER;

typedef struct {
    UINT32          dCbwSignature;  // 0-3h, CBW signature = "USBC"
    UINT32          dCbwTag;        // 4-7h, Tag
    UINT32          dCbwDataLength; // 8-0Bh, Data transfer length
    UINT8           bmCbwFlags;     // 0Ch, Flags
    UINT8           bCbwLun;        // 0Dh, Logical unit number
    UINT8           bCbwLength;     // 0Eh, Command block length
    UINT8           aCBWCB[16];     // 0Fh-1Eh, Command block buffer
} BOT_CMD_BLK;

typedef struct {
    UINT8           bPageCode;
    UINT8           bPageSize;
    UINT16          wXferRate;
    UINT8           bHeads;
    UINT8           bSectors;
    UINT16          wBlockSize;
    UINT16          wCylinders;
    UINT8           aReserved[22];
} PAGE_CODE_5;

typedef struct {
    UINT8           aReserved[3];
    UINT8           bCapListLength; // Amount of capacity format data after the header
} READ_FMT_CAP_HDR;

typedef struct {
    READ_FMT_CAP_HDR    stCapHeader;
    UINT32          dNumBlocks;
    UINT8           bDescCode;
    UINT8           bReserved;
    UINT16          wBlockSize;
}READ_FMT_CAPACITY;

#pragma pack(pop)

#define MODE_SENSE_COMMAND_EXECUTED         BIT0
#define READ_CAPACITY_COMMAND_EXECUTED      BIT1

//----------------------------------------------------------------------------
//      Physical Device Type Constants for BIOS internal use
//----------------------------------------------------------------------------
#define USB_MASS_DEV_TYPE_FLOPPY        1   // USB FD drive
#define USB_MASS_DEV_TYPE_CDROM         2   // USB CDROM drive
#define USB_MASS_DEV_TYPE_ZIP           3   // USB ZIP drive
#define USB_MASS_DEV_TYPE_ZIP_FLOPPY    4   // USB ZIP drive as floppy emulated
#define USB_MASS_DEV_TYPE_SUPER_DISK    5   // USB super disk (LS120/240)
#define USB_MASS_DEV_TYPE_HARD_DISK     6   // USB HD drive
#define USB_MASS_DEV_TYPE_DISKONKEY     7   // USB Disk On Key

//----------------------------------------------------------------------------
//      USB Mass Storage Related Data Structures and Equates
//----------------------------------------------------------------------------
//#define MAX_SIZE_FOR_FLOPPY_EMULATION   530         // 530MB  //(EIP80382-)
//#define MAX_LBA_FOR_FLOPPY_EMULATION    0x109000    // 530MB  //(EIP80382-)

// LBA to CHS conversion parameters
#define USB_FIXED_LBA_HPT_ABOVE_1GB 0xFF
#define USB_FIXED_LBA_SPT_ABOVE_1GB 0x3F
#define USB_FIXED_LBA_HPT_BELOW_1GB 0x40
#define USB_FIXED_LBA_SPT_BELOW_1GB 0x20

// Parameters regarding 1.44MB FDD
#define USB_144MB_FDD_MAX_LBA           0xB40
#define USB_144MB_FDD_MEDIA_TYPE        0x94
#define USB_144MB_FDD_MAX_HEADS         0x02
#define USB_144MB_FDD_MAX_SECTORS       0x12
#define USB_144MB_FDD_MAX_CYLINDERS     0x50
#define USB_144MB_FDD_MAX_BLOCK_SIZE    0x200

// Parameters regarding 720KB FDD
#define USB_720KB_FDD_MAX_LBA           0x5A0
#define USB_720KB_FDD_MEDIA_TYPE        0x1E
#define USB_720KB_FDD_MAX_HEADS         0x02
#define USB_720KB_FDD_MAX_SECTORS       0x09
#define USB_720KB_FDD_MAX_CYLINDERS     0x50
#define USB_720KB_FDD_MAX_BLOCK_SIZE    0x200

#define USB_UNKNOWN_MEDIA_TYPE  0x0F0

//-----------------------------------------------
//      ATAPI ERROR CODE REPORTED TO CALLER
//-----------------------------------------------
#define USB_ATA_WRITE_PROTECT_ERR   0x003   // Write protect error
#define USB_ATA_TIME_OUT_ERR        0x080   // Command timed out error
#define USB_ATA_DRIVE_NOT_READY_ERR 0x0AA   // Drive not ready error
#define USB_ATA_DATA_CORRECTED_ERR  0x011   // Data corrected error
#define USB_ATA_PARAMETER_FAILED    0x007   // Bad parameter error
#define USB_ATA_MARK_NOT_FOUND_ERR  0x002   // Address mark not found error
#define USB_ATA_NO_MEDIA_ERR        0x031   // No media in drive
#define USB_ATA_READ_ERR            0x004   // Read error
#define USB_ATA_UNCORRECTABLE_ERR   0x010   // Uncorrectable data error
#define USB_ATA_BAD_SECTOR_ERR      0x00A   // Bad sector error
#define USB_ATA_GENERAL_FAILURE     0x020   // Controller general failure

#define atapi_cntlr_not_ready_err   0x080   // ATAPI controller not ready/media not present
#define atapi_check_condition_err   0x0fd   // check condition reported by ATAPI controller
#define atapi_resend_err            0x0fe   // resend the command to ATAPI controller
#define atapi_media_change_err      0x006   // ATAPI media change error
#define atapi_reset_err             0x005   // ATAPI reset error
#define atapi_invalid_func          0x001   // ATAPI invalid function

#define atapi_volume_not_locked_err     0x0B0   // volume not locked in drive
#define atapi_volume_locked_err         0x0B1   // volume locked in drive
#define atapi_volume_not_removable_err  0x0B2   // volume not removable
#define atapi_volume_in_use_err         0x0B3   // volume in use
#define atapi_lock_count_exceeded_err   0x0B4   // lock count exceeded
#define atapi_eject_request_failed_err  0x0B5   // valid eject request failed

#define atapi_sense_failed              0x0ff   // Sense key command failed

//-----------------------------------------------------------------------
// Bulk-Only Mass Storage Reset (Class Specific Request)
//-----------------------------------------------------------------------
#define BOT_RESET_REQUEST_CODE          0x0FF   // Reset Request code

//-----------------------------------------------------------------------
// Bulk-Only Get Max Lun (Class Specific Request)
//-----------------------------------------------------------------------
#define BOT_GET_MAX_LUN_REQUEST_CODE    0x0FE   // Get Max Lun Request code

//-----------------------------------------------------------------------
// USB Command Status Wrapper Structure
//-----------------------------------------------------------------------
#define BOT_CSW_SIGNATURE       0x53425355  // 0-3h, signature = "USBS"

//-----------------------------------------------------------------------
// USB Command Block Wrapper Structure
//-----------------------------------------------------------------------
#define BOT_CBW_SIGNATURE       0x43425355  // 0-3h, signature = "USBC"

#pragma pack(push, 1)

typedef struct {
    UINT8  BootIndicator;
    UINT8  StartHead;
    UINT8  StartSector;
    UINT8  StartTrack;
    UINT8  OSType;
    UINT8  EndHead;
    UINT8  EndSector;
    UINT8  EndTrack;
    UINT32 StartingLba;
    UINT32 SizeInLba;
} MBR_PARTITION;

typedef struct {
    UINT8         BootCode[440];
    UINT32        UniqueMbrSig;
    UINT16        Unknown;
    MBR_PARTITION PartRec[4];
    UINT16        Sig;
} MASTER_BOOT_RECORD;

typedef struct {
    UINT8  jmp[3];               //0x0
    CHAR8  OEMName[8];           //0x3
    UINT16 BytsPerSec;           //0xB
    UINT8  SecPerClus;           //0xD
    UINT16 RsvdSecCnt;           //0xE
    UINT8  NumFATs;              //0x10
    UINT16 RootEntCnt;           //0x11
    UINT16 TotSec16;             //0x13
    UINT8  Media;                //0x15
    UINT16 FATSz16;              //0x16
    UINT16 SecPerTrk;            //0x18
    UINT16 NumHeads;             //0x1A
    UINT32 HiddSec;              //0x1C
    UINT32 TotSec32;             //0x20
    union {
        struct {
			UINT8  DrvNum;			//0x24
			UINT8  Reserved1;		//0x25
			UINT8  BootSig;			//0x26
			UINT32 VolID;			//0x27
			CHAR8  VolLab[11];		//0x2B
			CHAR8  FilSysType[8];	//0x36
            UINT8  Padding[510 - 0x3E];	//0x3E
        } Fat16;
        struct {
            UINT32 FATSz32;			//0x24
            UINT16 ExtFlags;		//0x28
			UINT16 FSVer;			//0x2A
			UINT32 RootClus;		//0x2C
			UINT16 FSInfo;			//0x30
			UINT16 BkBootSec;		//0x32
			UINT8  Reserved[12];	//0x34
			UINT8  DrvNum;			//0x40
			UINT8  Reserved1;		//0x41
			UINT8  BootSig;			//0x42
			UINT32 VolID;			//0x43
			CHAR8  VolLab[11];		//0x47
			CHAR8  FilSysType[8];	//0x52
            UINT8  Padding2[510 - 0x5A];	//0x5A
        } Fat32;
    } Fat;
    UINT16 Signature;
} BOOT_SECTOR;

#pragma pack(pop)

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
