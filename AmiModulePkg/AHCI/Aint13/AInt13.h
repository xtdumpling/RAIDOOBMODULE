//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AInt13.h
    Definitions and structures for AHCI INT13

**/
//****************************************************************************

#ifndef __AI13_HEADER__
#define __AI13_HEADER__
//---------------------------------------------------------------------------

#include "Protocol/PciIo.h"
//---------------------------------------------------------------------------

#pragma pack(1)


typedef struct _DEV_INFO_STRUC {
    UINT8         bDetectType;    // Detected Device Type (Details Below)
    UINT8         bInstalledType; // Device Installed Type (See Below)
    UINT16        wStatus;        // Bit-mapped device Init Status (Details Below)
    UINT8         bInt13Num;      // Device# for INT13 (8xh)
    UINT8         bPMNum;         // Port Multiplier Port #
    UINT8         bPortNum;       // SATA Port# (0-Based) where device is present
    UINT16        wBusDevFunc;    // Bus#, Dev#, Func# of Controller
    UINT8         bControllerNo;  // Ahci Controller number
    UINT32        dHbaBase;       // HBA Base Address of Generic Host Control Registers
    UINT32        dHbaCap;        // HBA Capabilities
    UINT32        dPortBase;      // Base Address of SATA port where device is present
    UINT8         bSelector;      // Device selector value
    UINT8         bIrq;           // IRQ used by this device
    UINT8         bInfoFlag;      // Information Flag (details below)
    UINT8         bSectorSizeN;   // N value of Sector size 2^N ;For 512bytes sector, N = 9
    UINT16        wSmartInfo;     // SMART info (details below)
    UINT32        dTotalSectorsLo;// Total #of sectors in device (1-based)
    UINT32        dTotalSectorsHi;
    UINT16        wXferCount;     // Device transfer count. Used for ATAPI packer size
    UINT8         bBlockInfo;
    UINT8         b32BitInfo;
    UINT8         bUDMAInfo;
    UINT8         bPIOInfo;
} DEV_INFO_STRUC;

typedef struct _CONTROLLER_INFO_STRUC
{
    UINT16      BusDevFun;		// Bus, Device and Function number for a controller
    UINT8		ControllerNo;	// Controller number for a controller
}CONTROLLER_INFO_STRUC;

// Details of bDetectType

#define DETECT_NO           0
#define DETECT_ATA          1
#define DETECT_ATAPI_CD     2
#define DETECT_ATAPI_ARMD   3

// Details of bInstalledType

#define INSTALLED_NO        0
#define INSTALLED_HDD       1
#define INSTALLED_CDROM     2
#define INSTALLED_ARMD_FDD  3
#define INSTALLED_ARMD_HDD  4

// Details of wStatus

//  Bit 0 = 1, successful (device is installed in this case)
//  Bit 1 = 1, Identify Device Failed
//  Bit 2 = 1, Invalid Device Geometry
//  Bit 3 = 1, Init device parameter failed
//  Bit 4 = 1, Re calibrate failed
//  Bit 5 = 1, SMART failed
//  Bit 6 = 1, Verify failed
//  Bit 7 = 1, Security Freeze Lock failed
//  Bit 8 = 1, Port Reset failed
//  Bit 9 = 1, SMART Event was found
//  Bit 10 = 1, Device will be supported by RIAD OROM, not AHCI OROM (ex. HDD)
//  Bit 14-11,  Reserved for future use
//  Bit 15 = 1, Device not supported

#define ST_SUCCESS              0x0001
#define ST_ID_DEV_ERR           0x0002
#define ST_DEV_GEOMETRY_ERR     0x0004
#define ST_INIT_DEV_PARAM_ERR   0x0008
#define ST_RECALI_ERR           0x0010
#define ST_SMART_ERR            0x0020
#define ST_VERIFY_ERR           0x0040
#define ST_FREEZE_LOCK_ERR      0x0080
#define ST_PORT_RESET_ERR       0x0100
#define ST_SMART_EVENT          0x0200
#define ST_RAID_SUPPORT_ERR     0x0400
#define ST_NOT_SUPPORT_ERR      0x8000

// Details of bInt13Num

// Bit 7 = 1, this bit must be set for CDs/HDDs
// AHCI_CD_CSM_ID_OFFSET, PATA CDs/DVDs use the device handle range 0x80-0x8F
//                        so this value will be added to ensure AHCI CDs/DVDs 
//                        are above that range

#define AHCI_CD_CSM_ID_OFFSET  0x10  


// Details of bInfoFlag, a bit-mapped field

//  Bit 0 = 1, Device supports removable media
//  Bit 1 = 1, 48bit LBA enabled
//  Bit 2 = 1, Device uses IRQ; 0, Device uses DRQ
//  Bit 6-2,   Reserved
//  Bit 7 = 1, ATAPI Device; 0, ATA Device

#define INFO_REMOVABLE  0x01
#define INFO_LBA_48     0x02
#define INFO_IRQ        0x04
#define INFO_ATAPI      0x80

// Details wSmartInfo

//  Bit 0 = 0/1, SMART (Not Supported/Supported)
//  Bit 1 = 0/1, SMART (Disabled/Enabled)
//  Bit 2 = 0/1, Device Status Good/Bad
//  Bit 7 = 0/1, SMART Execution Successful/Error
//  Bit 15-8,    Reserved

#define AHCI_SMART_SUPPORT          0x01
#define AHCI_SMART_ENABLE           0x02
#define AHCI_SMART_EN               0x02
#define AHCI_SMART_STATUS           0x04
#define AHCI_SMART_COMMAND_STATUS   0x80

#define SECTOR_LENGTH_N         9
#define SELECTOR_NON_LBA        0xA0
#define SELECTOR_LBA            0xE0
#define MAX_STND_XFER_SECTOR    0x80
#define MAX_EXTD_XFER_SECTOR    0x7F
#define BLOCK_SIZE              0x80
#define BLOCK_SIZE_N            0xB

typedef struct _DEV_PTR_STRUC {
    UINT8         bInt13Num;      // INT13 Drive# for this Port
    UINT8         bDetectType;    // Detected Device Type
    UINT8         bPMnum;         // Port Multiplier port #
    UINT8         bPortNum;       // Port# (0-based) on Controller
    UINT32        dParamTablePtr; // Ptr to device parameter table
    UINT32        dInfoTablePtr;  // Ptr to device info table
} DEV_PTR_STRUC;

typedef struct _DEV_PARAM_STRUC {
    UINT16        wMAXCYL;    // maximum no. of cylinders..INT13 interface. (logical)
    UINT8         bMAXHN;     // maximum no. of heads..INT13 interface. (logical)
    UINT8         bLBASIG;    // LBA signature
    UINT8         bLBASPT;    // #of sectors per track drive is configured for. (physical)
    UINT16        wWPCYL;     // start write precomp cyl no.
    UINT8         bReserved;  // reserved
    UINT8         bCBYTE;     // bit 3 for more than 8 heads
    UINT16        wLBACYL;    // #of cylinders drive is configured for. (physical)
    UINT8         bLBAHD;     // #of heads drive is configured for. (physical)
    UINT16        wLANDZ;     // Landing zone
    UINT8         bMAXSN;     // no. of sectors per track..INT13 interface. (logical)
    UINT8         bLBACHK;    // checksum..LBA
} DEV_PARAM_STRUC;

//
// The following definitions should be in sync with 16-bit definition
//
#define SATA_PORT_COUNT 32
#define MAX_DESCRIPTION_STRLEN 32

typedef struct _AHCI_SMM_RTS {
    UINT8   MiscInfo;
    UINT16  SmmAttr;
    UINT32  SmmPort;
    UINT32  SmmData;
} AHCI_SMM_RTS;

typedef struct _AHCI_INT13_HOOK {
    UINT16  FunctionOffs;
    UINT16  FunctionSeg;
} AHCI_INT13_HOOK;

typedef struct _AHCI_RT_MISC_DATA {
    UINT8           NumAhciDevice;      // #of AHCI device installed by BIOS
    UINT8           RunAttribute;       // Bit-mapped information about runtime environment
    UINT8           AhciEbdaSizeK;      // Size of EBDA in unit of 1k that is created by AHCI init
    UINT32          AhciEbdaStart;      // Start offset of AHCI communication area in EBDA
    AHCI_SMM_RTS    AhciSmmRt;          // Port and Data information to generate software SMI
    AHCI_INT13_HOOK PreInt13Hook;       // Pre-process of AhciInt13 hook
    AHCI_INT13_HOOK PostInt13Hook;      // Post-process of AhciInt13 hook
    UINT32          PortBaseAddress;    // AHCI_ACCESS Structre Address
} AHCI_RT_MISC_DATA;

typedef struct _DEV_BBS_OUTFIT {
    UINT8   DescString[MAX_DESCRIPTION_STRLEN];
} DEV_BBS_OUTFIT;

typedef struct _AHCI_I13_RTDATA {
    DEV_PARAM_STRUC DevParam[SATA_PORT_COUNT];
    DEV_INFO_STRUC  DevInfo[SATA_PORT_COUNT+1];
    DEV_PTR_STRUC   DevPtr[SATA_PORT_COUNT];
    AHCI_RT_MISC_DATA   AhciRtMiscData;
    DEV_BBS_OUTFIT  DevOutfit[SATA_PORT_COUNT];
} AHCI_I13_RTDATA;

typedef struct _AHCI_I13_DATA {
    AHCI_I13_RTDATA rtdata;
    UINT16          CheckForAhciCdromOffset;
    UINT16          AhciBcvOffset;
} AHCI_I13_DATA;

// Details of bSignature field:	A signature Axh indicates that the table is translated.
#define DPTBL_SIG_MASK		0x0F0
// A0h signature indicating LBA translation
#define LBA_SIGNATURE       0x0A0

//-------------------------------------------------------------------------
// Port registers
//
#define PORT_REGISTER_START         0x100
#define PORT_REGISTER_SET_SIZE      0x80
#define PORT_REGISTER_SET_SIZE_N    7

#define SIZE_CLCTFIS_AREA_K           4
#define A_EBDA_USED                   1
#define MMIO_THRU_SWSMI               BIT2
#define MMIO_THRU_SINGLE_BIGREAL_MODE BIT3
#define SMM_MODE_CHECK                BIT4

#define BAID_TYPE_HDD       1
#define BAID_TYPE_RMD_HDD   2
#define BAID_TYPE_CDROM	    3
#define BAID_TYPE_RMD_FDD   4
#define BAID_TYPE_FDD       5

#define DEFAULT_DEVICE_STATUS	0x50
#define SYSTYPE_ATA             0
#define DEVTYPE_SYS             1

EFI_STATUS  GetAccessInfo (EFI_PCI_IO_PROTOCOL*, UINT16*, UINT16*);
EFI_STATUS  InitCspData (UINT16, UINT16,UINT32,UINT8);
UINT16 CountDrives(IN EFI_HANDLE *HandleBuffer,
                   IN UINTN        HandleCount,
                   IN VOID         *Devices
                   );
#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
