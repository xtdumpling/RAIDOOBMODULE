//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IdeRecovery.h
    Header file for IdeRecovery
**/

#ifndef __IDE_RECOVERY__H__
#define __IDE_RECOVERY__H__
#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------------------------------------------------

#include <Token.h>
#include <Pei.h>
#include <AmiPeiLib.h> 
#include <AmiLib.h> 
#include <Ppi/BlockIo.h>
#include <Ppi/Stall.h>
#include <Protocol/BlockIo.h>
#include <Ppi/AtaController.h>

//----------------------------------------------------------------------

//
// Atapi command definitions
//

#define EFI_MIN( a, b )       (((a) < (b)) ? (a) : (b))

#define ATAPI_SIGNATURE 0xEB14

#define CD_ROM_DEVICE 0x5

#define SK_NO_SENSE 0
#define SK_NOT_READY    2
#define SK_UNIT_ATTENTION   6

#define MAX_SENSE_KEY_COUNT 6

#define MAX_SECTOR_COUNT_PIO            255 
#define MAX_SECTOR_COUNT_PIO_48BIT      65535

// This Macro value should not be changed till PI spec comes with new PPI for BlockIo that can support larger size Hdd's.
// Right now existing BlockIo PPI, lastBlock is defined as UINTN( UINT32) that can't save value more than 4GB.

#define LBA48_SUPPORT 0

#pragma pack(1)

typedef struct
{
	UINT16	GeneralConfiguration_0;
	UINT16	Reserved_1;
	UINT16	Special_Config_2;
	UINT16	Reserved_3;
	UINT16	Reserved_4;
	UINT16	Reserved_5;
	UINT16	Reserved_6;
	UINT16	Reserved_7;
	UINT16	Reserved_8;
	UINT16	Reserved_9;
	UINT8	Serial_Number_10[20];
	UINT16	Reserved_20;
	UINT16	Reserved_21;
	UINT16	Reserved_22;
	UINT8	Firmware_Revision_23[8];
	UINT8	Model_Number_27[40];
	UINT16	Maximum_Sector_Multiple_Command_47;
	UINT16	Trusted_Computing_Support;
	UINT16	Capabilities_49;
	UINT16	Capabilities_50;
	UINT16	PIO_Mode_51;
	UINT16	Reserved_52;
	UINT16	Valid_Bits_53;
	UINT16	Reserved_54_58[5];
	UINT16	Valid_Bits_59;
	UINT32	TotalUserAddressableSectors_60;
	UINT16	SingleWord_DMA_62;
	UINT16	MultiWord_DMA_63;
	UINT16	PIO_Mode_64;
	UINT16	Min_Multiword_DMA_timing_65;
	UINT16	Manuf_Multiword_DMA_timing_66;
	UINT16	Min_PIO_Mode_timing_67;
	UINT16	Min_PIO_Mode_timing_68;
	UINT16	Reserved_69_74[6];
	UINT16	Queue_Depth_75;
	UINT16	Reserved_76_79[4];
	UINT16	Major_Revision_80;
	UINT16	Minor_Revision_81;
	UINT16	Command_Set_Supported_82;
	UINT16	Command_Set_Supported_83;
	UINT16	Command_Set_Supported_84;
	UINT16	Command_Set_Enabled_85;
	UINT16	Command_Set_Enabled_86;
	UINT16	Command_Set_Enabled_87;
	UINT16	UDMA_Mode_88;
	UINT16	Time_security_Earse_89;
	UINT16	Time_Esecurity_Earse_90;
	UINT16	Current_Power_Level_91;
	UINT16	Master_Password_Rev_92;
	UINT16	Hard_Reset_Value_93;
	UINT16	Acoustic_Level_94;
	UINT16	Reserved_95_99[5];
	UINT64	LBA_48;
	UINT16	Reserved_104_126[23];
	UINT16	Status_Notification_127;
	UINT16	Security_Status_128;
	UINT16	Reserved_129_159[31];
	UINT16	CFA_Power_Mode_160;
	UINT16	Reserved_161_175[15];
	UINT16	Media_Serial_Number_176_205[30];
	UINT16	Reserved_206_254[49];
	UINT16	Checksum_255;
} ATA_IDENTIFY_DATA;

typedef struct
{
    UINT8 PeripheralDeviceType_0 : 5;
    UINT8 Reserved_0             : 3;
    UINT8 Data_1_95[95];
} INQUIRY_DATA;

typedef struct
{
    UINT8 Data_0_1[2];
    UINT8 SenseKey_2 : 4;
    UINT8 Data_2     : 4;
    UINT8 Data_3_6[4];
    UINT8 AdditionalSenseLength_7;
    UINT8 Data_8_11[4];
    UINT8 AdditionalSenseCode_12;
    UINT8 AdditionalSenseCodeQualifier_13;
    UINT8 Data_14_17[4];
} REQUEST_SENSE_DATA;

typedef struct
{
    UINT8 LastLba0;
    UINT8 LastLba1;
    UINT8 LastLba2;
    UINT8 LastLba3;
    UINT8 BlockSize3;
    UINT8 BlockSize2;
    UINT8 BlockSize1;
    UINT8 BlockSize0;
} READ_CAPACITY_DATA;

#pragma pack()

#define PACKET_CMD                0xA0
#define ATA_IDENTIFY_DEVICE_CMD   0xEC
#define ATA_READ_SECTOR_CMD       0x20
#if LBA48_SUPPORT
#define ATA_READ_SECTOR_EXT_CMD   0x24
#endif

#pragma pack(1)

typedef struct
{
    UINT8 OperationCode_0;
    UINT8 Data_1_3[3];
    UINT8 AllocationLength_4;
    UINT8 Data_5_11[7];
} GENERIC_CMD;

typedef struct
{
    UINT8 OperationCode_0;
    UINT8 Data_1;
    UINT8 Lba0;
    UINT8 Lba1;
    UINT8 Lba2;
    UINT8 Lba3;
    UINT8 Data_6;
    UINT8 TransferLengthMSB;
    UINT8 TransferLengthLSB;
    UINT8 Data_9_11[3];
} READ10_CMD;

typedef union
{
    UINT16      Data16[6];
    GENERIC_CMD Cmd;
    READ10_CMD  Read10;
} ATAPI_PACKET_COMMAND;

#pragma pack()

#define TEST_UNIT_READY       0x00
#define REQUEST_SENSE         0x03
#define INQUIRY               0x12
#define READ_CAPACITY         0x25
#define READ_10               0x28

//
// default content of device control register, disable INT\
//
#define DEFAULT_CTL           (0x0a)  
#define DEFAULT_CMD           (0xa0)

#define MAX_ATAPI_BYTE_COUNT  (0xfffe)

#pragma pack(1)

//
// IDE registers bit definitions
//

//
// Err Reg
//
#define ABRT_ERR  BIT02    // Aborted Command

//
// Device/Head Reg
//
#define LBA_MODE  BIT06

//
// Status Reg
//
#define BSY   BIT07     // Controller Busy
#define DRDY  BIT06     // Drive Ready
#define DWF   BIT05     // Drive Write Fault
#define DRQ   BIT03     // Data Request
#define CORR  BIT02     // Corrected Data
#define ERR   BIT00     // Error

#pragma pack()

#define STALL_1_MILLI_SECOND  1000  // stall 1 ms

#define TIMEOUT      1000               // 1 second
#define COMMAND_COMPLETE_TIMEOUT 10000  // 10 seconds
#define LONGTIMEOUT  5000               // 5 seconds

//----------------------------------------------------------------------

typedef enum {
    EnumerateAtapi, EnumerateAta
} ENUMERATE_TYPE;

typedef enum {
    IdeLegacy, IdeNative, IdeMaxMode
} EFI_IDE_MODE;

typedef enum {
    IdePrimary, IdeSecondary, IdeMaxChannel
} EFI_IDE_CHANNEL;

typedef enum {
    IdeMaster, IdeSlave, IdeMaxDevice
} EFI_IDE_DEVICE;

//
// IDE Registers
//
typedef union
{
    UINT16  Command;       // when write
    UINT16  Status;        // when read
} IDE_CMD_OR_STATUS;

typedef union
{
    UINT16  Error;         // when read
    UINT16  Feature;       // when write
} IDE_ERROR_OR_FEATURE;

typedef union
{
    UINT16  AltStatus;     // when read
    UINT16  DeviceControl; // when write
} IDE_AltStatus_OR_DeviceControl;


//
// IDE registers set
//
typedef struct
{
    UINT16                  Data;
    IDE_ERROR_OR_FEATURE    Reg1;
    UINT16                  SectorCount;
    UINT16                  SectorNumber;
    UINT16                  CylinderLsb;
    UINT16                  CylinderMsb;
    UINT16                  Head;
    IDE_CMD_OR_STATUS       Reg;
    IDE_AltStatus_OR_DeviceControl Alt;
    UINT16                         DriveAddress;
} IDE_BASE_REGISTERS;

typedef struct
{
    UINT8                   Device;
    BOOLEAN                 LookedForMedia;
#if LBA48_SUPPORT
    BOOLEAN                 Lba48Bit;
#endif
    IDE_BASE_REGISTERS      *IdeIoPortRegisters;
    EFI_PEI_BLOCK_IO_MEDIA  MediaInfo;
} IDE_RECOVERY_DEVICE_INFO;

#define MAX_DEVICE_COUNT    8

typedef struct
{
    EFI_PEI_RECOVERY_BLOCK_IO_PPI   RecoveryBlkIo;
    BOOLEAN                         HaveEnumeratedDevices;
    UINTN                           DeviceCount;
    IDE_RECOVERY_DEVICE_INFO        *DeviceInfo[MAX_DEVICE_COUNT];
} IDE_RECOVERY_BLK_IO_DEV;

//
// PEI Recovery Block I/O PPI
//

EFI_STATUS 
AtaAtapiCommonInit (
    IN  EFI_PEI_SERVICES    **PeiServices
);

EFI_STATUS 
EFIAPI
Atapi_GetNumberOfBlockDevices (
    IN  EFI_PEI_SERVICES    **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    OUT UINTN   *NumberBlockDevices
);

EFI_STATUS 
EFIAPI
Atapi_GetBlockDeviceMediaInfo (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA          *MediaInfo
);

EFI_STATUS 
EFIAPI
Atapi_ReadBlocks (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    IN  EFI_PEI_LBA                     StartLba,
    IN  UINTN                           BufferSize,
    OUT VOID                            *Buffer
);

EFI_STATUS 
EFIAPI
Ata_GetNumberOfBlockDevices (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    OUT UINTN                           *NumberBlockDevices
);

EFI_STATUS 
EFIAPI
Ata_GetBlockDeviceMediaInfo (
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN UINTN                         DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA       *MediaInfo 
);

EFI_STATUS 
EFIAPI
Ata_ReadBlocks (
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN  UINTN                         DeviceIndex,
    IN  EFI_PEI_LBA                   StartLba,
    IN  UINTN                         BufferSize,
    OUT VOID                         *Buffer
);
//
// Private functions
//
VOID 
EnumerateDevices (
    IN  IDE_RECOVERY_BLK_IO_DEV *IdeRecoveryBlkIoDev,
    ENUMERATE_TYPE              EnumerateType
);

BOOLEAN 
Atapi_DiscoverDevice (
    IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
);

BOOLEAN 
Ata_DiscoverDevice (
    IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
);

EFI_STATUS 
AtapiPacketCommandIn (
    IN  IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  ATAPI_PACKET_COMMAND        *Packet,
    IN  UINT16                      *Buffer,
    IN  UINT32                      ByteCount,
    IN  UINTN                       TimeoutInMilliSeconds
);

EFI_STATUS 
Inquiry (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
);

EFI_STATUS 
Atapi_DetectMedia (
    IN  OUT IDE_RECOVERY_DEVICE_INFO *DeviceInfo
);

EFI_STATUS 
TestUnitReady (
    IN  IDE_RECOVERY_DEVICE_INFO *DeviceInfo
);

EFI_STATUS 
RequestSense (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  REQUEST_SENSE_DATA              *SenseBuffers,
    IN  OUT UINT8                       *SenseCounts
);

EFI_STATUS 
ReadCapacity (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
);

EFI_STATUS 
Atapi_ReadSectors (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  VOID                            *Buffer,
    IN  EFI_PEI_LBA                     StartLba,
    IN  UINTN                           NumberOfBlocks,
    IN  UINTN                           BlockSize
);

EFI_STATUS 
AtaPioDataIn (
    IN  IDE_BASE_REGISTERS  *IdeIoPortRegisters,
    OUT VOID                *Buffer,
    IN  UINT32              ByteCount,
    IN  UINT8               SectorCount,
    IN  UINT8               SectorNumber,
    IN  UINT8               CylinderLsb,
    IN  UINT8               CylinderMsb,
    IN  UINT8               Device,
    IN  UINT8               Command
);

#if LBA48_SUPPORT
EFI_STATUS 
AtaPioDataIn48 (
    IN IDE_BASE_REGISTERS *IdeIoPortRegisters,
    OUT VOID              *Buffer,
    IN UINT32             ByteCount,
    IN UINT16             SectorCount,
    IN UINT8              LbaLow,
    IN UINT8              LbaMid,
    IN UINT8              LbaHigh,
    IN UINT8              Lba48Low,
    IN UINT8              Lba48Mid,
    IN UINT8              Lba48High,
    IN UINT8              Device,
    IN UINT8              Command 
);
#endif

EFI_STATUS 
AtaIdentify (
    IN  IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    OUT ATA_IDENTIFY_DATA           *AtaIdentifyData
);

EFI_STATUS 
Ata_ReadSectors (
    IN  IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  OUT VOID                    *Buffer,
    IN  EFI_PEI_LBA                 StartLba,
    IN  UINTN                       NumberOfBlocks,
    IN  UINTN                       BlockSize
);

EFI_STATUS
WaitForBitsToClear (
    UINT16 Register,
    UINT8  Bits,
    UINTN  TimeoutInMilliSeconds
);

EFI_STATUS 
WaitForBSYClear (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
);

EFI_STATUS 
DRQClear (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
);

EFI_STATUS 
DRQClear2 (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
);

EFI_STATUS 
DRQReadyHelper (
    UINT16      StatusRegister,
    UINT16      ErrorRegister,
    IN  UINTN   TimeoutInMilliSeconds
);

EFI_STATUS 
DRQReady (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
);

EFI_STATUS 
DRQReady2 (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
);

EFI_STATUS 
CheckErrorStatus (
    IN  UINT16  StatusReg
);

static VOID 
ZeroMem (
    IN  VOID    *Buffer,
    IN  UINTN   Size
);

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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
