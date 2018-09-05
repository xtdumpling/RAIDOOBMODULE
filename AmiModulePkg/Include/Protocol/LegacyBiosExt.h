//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  @brief AMI CSM extension interfaces and definitions
**/

#ifndef __LEGACY_BIOS_EXT_PROTOCOL_H__
#define __LEGACY_BIOS_EXT_PROTOCOL_H__

#include <Protocol/LegacyRegion.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/PciIo.h>

#define EFI_LEGACY_BIOS_EXT_PROTOCOL_GUID            \
  { 0x8e008510, 0x9bb1, 0x457d, { 0x9f, 0x70, 0x89, 0x7a, 0xba, 0x86, 0x5d, 0xb9 } }

extern EFI_GUID gEfiLegacyBiosExtProtocolGuid;
extern EFI_GUID gAmiLegacyBootProtocolGuid;
extern EFI_GUID gAmiOemCsm16BinaryGuid;
extern EFI_GUID gAmiPciIrqProgramGuid;
extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;
extern EFI_GUID gAmiExtPciBusProtocolGuid;
extern EFI_GUID gAmiCsmThunkDriverGuid;
extern EFI_GUID gWinCeGuid;
extern EFI_GUID gAmiCsmStartedProtocolGuid;
extern EFI_GUID gOpromStartEndProtocolGuid;
extern EFI_GUID gAmiOpromPolicyProtocolGuid;
extern EFI_GUID gAmiPciOpromDataProtocolGuid;

#pragma pack (1)

typedef struct _EFI_LEGACY_BIOS_EXT_PROTOCOL EFI_LEGACY_BIOS_EXT_PROTOCOL;

#define MAX_BBS_ENTRIES_NO  256

//****************************************************************************
// Embedded Legacy ROM ID definitions
//****************************************************************************
#define CSM16_VENDORID      0
#define CSM16_DEVICEID      0
#define CSM16_AHCI_RT_DID   1
#define CSM16_USB_RT_DID    2
#define CSM16_PNP_RT_DID    3
#define CSM16_SD_BOOT_DID   4

//****************************************************************************
// OEM Module ID definitions. Note: use CSM16_VENDORID and CSM16_DEVICEID as
// vendor/device ID for these modules.
//****************************************************************************
#define CSM16_MODULEID      1
#define SERVICEROM_MODULEID 2
#define OEM_MODULEID        3

#define OPROM_MODULEID          0x20
#define OPROM_MULTIPLE_MODULEID (0xa0 << 8)

//****************************************************************************
//		CSM16 API equates
//****************************************************************************
#define CSM16_OEM_HW_RESET          1
#define CSM16_OEM_NMI               2
#define CSM16_SMBIOS_PNP            3
#define CSM16_OEM_AFTER_SWSMI       4
#define CSM16_OEM_BEFORE_SWSMI      5
#define CSM16_OEM_ON_BOOT_FAIL      6
#define CSM16_CSP_AHCI_ACCESSHBA    7
#define CSM16_OEM_BEFORE_CALL_BOOT_VECTOR    8
#define CSM16_OEM_PROBE_KBC         9
#define CSM16_OEM_VIRUS_WARNING     10
#define CSM16_OEM_SERIAL_ACCESS     11
#define CSM16_OEM_BEFORE_CALL_INT19 12
#define CSM16_OEM_TRY_NEXT_BOOT     14

#define MAX_OEM16_FUNC_NUM          14

//****************************************************************************
//		BDA data structure
//****************************************************************************
typedef struct _BDA_DATA {

UINT16  rs_232_address[4];  // 00 - RS232 adapters addresses
UINT16  printer_address[3]; // 08 - Printer addresses
UINT16  ext_bios_data_seg;  // 0E - Segment pointer to EBDA
UINT16  machine_config;     // 10 - Hardware Equipment Status
UINT8   test_scratch;       // 12 - Scratch byte during init
UINT16  system_memory;      // 13 - System Memory in KBytes
UINT8   sys_info;           // 15 - miscellaneous system info
UINT8   dummy_byte_1;       // 16
UINT8   kb_stat_flag;       // 17 - Keyboard Status Flag
UINT8   kb_hold_flag;       // 18 - Toggle key's hold flag
UINT8   kb_alt_char;        // 19 - Alt_character byte
UINT16  kb_buf_head_ptr;    // 1A - Keyboard buffer head pointer
UINT16  kb_buf_tail_ptr;    // 1C - Keyboard buffer tail pointer
UINT16  kb_buffer[16];      // 1E - Keyboard buffer (15 char max)
UINT8   recal_int_flag;     // 3E - Status for recalibration
UINT8   motor_status_flag;  // 3F - Status for drive motors
UINT8   motor_time_count;   // 40 - Turn off time for motor
UINT8   floppy_io_status;   // 41 - Return code status byte
UINT8   task_file[7];       // 42 - A.K.A. fdc_8272_status
UINT8   mode_of_vdo;        // 49 - Current Video Mode (set by user)
UINT16  columns_of_vdo;     // 4A - Number of columns per row
UINT16  page_length;        // 4C - Number of bytes per Video Page
UINT16  page_start;         // 4E - Start address of Active Video Page
UINT16  position_of_cursor[8];  // 50 - Cursor position for each Video Page
UINT16  mode_of_cursor;     // 60 - Current cursor mode setting
UINT8   active_page_no;     // 62 - Current display page no
UINT16  address_of_6845;    // 63 - Address of 6845 index reg
UINT8   current_mode_set;   // 65 - Mode set in 6845 control port
UINT8   current_palette;    // 66 - Current palette set
//---------------------------------------
//  POST DATA STORAGES
//---------------------------------------
UINT16  offset_shutdown;    // 67 - Offset of shutdown routine
UINT16  segment_shutdown;   // 69 - Segment of shutdown routine
UINT8   interrupt_flag;     // 6B - Interrupt flag
//----------------------------------------------------------------------------
//  TIMER DATA STORAGES
//----------------------------------------------------------------------------
//UINT16    timer_low_count;    // 6C - Low Word of timer count
//UINT16    timer_high_count;   // 6E - High Word of timer count
UINT32  timer_count;    // 6C - timer count
UINT8   timer_count_rolled; // 70 - Timer Rolled Over
//----------------------------------------------------------------------------
//  BIOS DATA STORAGES EXTENDED
//----------------------------------------------------------------------------
UINT8   break_flag;         // 71 - Flag for CTRL-BREAK key key
UINT16  dummy_word;         // 72 - Unused data word
//----------------------------------------------------------------------------
//  WINCHESTER DATA STORAGES
//----------------------------------------------------------------------------
UINT8   winch_status;       // 74 - Status of Hard Disk operation
UINT8   winch_number;       // 75 - Number of Hard Disk drives
UINT8   winch_control_byte; // 76
UINT8   dummy_byte_2;       // 77
//----------------------------------------------------------------------------
//  PRINTER & RS232 DATA STORAGES
//----------------------------------------------------------------------------
UINT8   print_time_max[4];  // 78 - Maximum time for printing
UINT8   rs_232_time_max[4]; // 7C - Maximum transmission time for COM ports
//----------------------------------------------------------------------------
//  KEYBOARD DATA STORAGES EXTENDED
//----------------------------------------------------------------------------
UINT16  kb_buff_begin;      // 80 - Start address of Keyboard Buffer
UINT16  kb_buff_end;        // 82 - End address of Keyboard Buffer
UINT8   rows_of_vdo[7];     // 84 - EGA/VGA control state
//----------------------------------------------------------------------------
//  FLOPPY DATA STORAGES EXTENDED
//----------------------------------------------------------------------------
UINT8   last_xfer_rate;     // 8B - Last data transfer rate (Floppy)
//----------------------------------------------------------------------------
//  WINCHESTER DATA STORAGES EXTENDED
//----------------------------------------------------------------------------
UINT8   winch_stat_reg;     // 8C - Hard Disk status register
UINT8   winch_error_reg;    // 8D - Hard Disk error register, a.k.a. iraid_int_flag
UINT8   winch_int_flag;     // 8E - Hard Disk interrupt status
//----------------------------------------------------------------------------
//  FLOPPY DATA STORAGES EXTENDED
//----------------------------------------------------------------------------
UINT8   duel_card;          // 8F - Availability of 1.2M Floppy Drive
UINT8   current_floppy_media[2];    // 90 - Type of floppy drive/media
UINT8   run_scratch1;       // 92 - Runtime Scratch data byte 1
UINT8   run_scratch2;       // 93 - Runtime Scratch data byte 1
UINT8   current_cylinder[2];// 94 - Cylinder position of Floppy Disk
//----------------------------------------------------------------------------
//  KEYBOARD DATA STORAGES EXTENDED
//----------------------------------------------------------------------------
UINT8   dummy_byte_3;       // 96 - Keyboard Type byte
UINT8   kb_leds_flag;       // 97 - Keyboard LED's status byte
//----------------------------------------------------------------------------
//  REAL TIME CLOCK DATA STORAGES
//----------------------------------------------------------------------------
UINT16  user_flag_offset;   // 98 - Offset of user defined flag
UINT16  user_flag_segment;  // 9A - Segment of user defined flag
UINT16  user_wait_low;      // 9C - Low WORD of user defined time
UINT16  user_wait_high;     // 9E - High WORD of user defined time
UINT8   user_wait_flag[23]; // A0 - User defined time active flag, Video data area
//----------------------------------------------------------------------------
//  APM HANDLING BIOS DATA AREA USED FOR GREEN PC/NOTE BOOK
//----------------------------------------------------------------------------
UINT8   apm_data[73];       // B7
UINT8   prtsc_status_flag;  // 100 - Print Screen status byte

} BDA_DATA;

typedef struct{
    UINT16 ModuleId, VendorId, DeviceId;
    UINT32 Size;
} OPROM_HEADER;

typedef struct {
    UINT16 ModuleId;
    UINT16 VendorId;
    UINT16 DeviceId;
} PARTIAL_OPROM_HEADER;


typedef EFI_STATUS (EFIAPI *EFI_AMICSM_GET_EMBEDDED_ROM) (
    IN UINT16 ModuleId,
    IN UINT16 VendorId,
    IN UINT16 DeviceId,
    OUT VOID **ImageStart,
    OUT UINTN *ImageSize
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_GET_BBS_TABLE) (
    IN OUT BBS_TABLE **BbsTable,
    IN OUT UINT8 *BbsCount
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_ADD_BBS_ENTRY) (
    IN BBS_TABLE *BbsEntry
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_INSERT_BBS_ENTRY_AT) (
    IN EFI_LEGACY_BIOS_EXT_PROTOCOL *This,
    IN OUT BBS_TABLE *BbsEntry,
    IN OUT UINT8 *EntryNumber
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_REMOVE_BBS_ENTRY_AT) (
    IN EFI_LEGACY_BIOS_EXT_PROTOCOL *This,
    IN UINT8 EntryNumber
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_ALLOCATE_EBDA) (
    IN EFI_LEGACY_BIOS_EXT_PROTOCOL *This,
    IN UINT8    RequestedSizeKB,
    OUT UINT32  *NewEbdaAddress,
    OUT UINT32  *EbdaStartOffset
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_UNLOCK_SHADOW) (
    IN UINT8    *Address,
    IN UINTN    Size,
    OUT UINT32  *LockUnlockAddr,
    OUT UINT32  *LockUnlockSize
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_LOCK_SHADOW) (
    IN UINT32  LockAddr,
    IN UINT32  LockSize
);

typedef struct _LEGACY16_TO_EFI_DATA_TABLE_STRUC {
    UINT32 MiscInfoAddress;         // 32-bit address of misc_info variable
    UINT32 RtCmosByteAddress;       // 32-bit address of rt_cmos_byte variable
    UINT32 SmiFlashTableAddress;    // 32-bit address of RUN_CSEG:smiflash_table variable
    UINT16 CdrBevOffset;
    UINT16 CDROMSupportAPIOfs;
    UINT16 AddedPnpFunctionsOfs;
    UINT16 BaidRmdVectorCntOfs;
    UINT16 ReverseThunkPtrOfs;
    UINT16 ReverseThunkStackPtrOfs;
    UINT16 Compatibility16TableOfs;
    UINT16 SmbiosTableOfs;
    UINT16 StartOfFreeE000Ofs;
    UINT16 ReadjustHddPrioritiesOfs;
    UINT16 BootBufferPtrOfs;
} LEGACY16_TO_EFI_DATA_TABLE_STRUC;

typedef EFI_STATUS (*EFI_AMICSM_GET_16BIT_FUNC_ADDRESS)(UINT16 id, UINT32 *Addr);

typedef UINTN (*EFI_AMICSM_COPY_LEGACY_TABLE)(
    IN VOID *Table,
    IN UINT16 TableSize,
    IN UINT16 Alignment,
    IN UINT16 Csm16LocationAttribute
);

typedef EFI_STATUS (*EFI_AMICSM_INSTALL_ISA_ROM)(
    IN EFI_LEGACY_BIOS_EXT_PROTOCOL *This,
    IN UINTN    RomAddress
);

typedef EFI_STATUS (*EFI_AMICSM_GET_SHADOWRAM_ADDRESS)(
    IN OUT UINT32   *AddrMin,
    IN UINT32       AddrMax,
    IN UINT32       Size,
    IN UINT32       Alignment
);

#define F0000_BIT       1
#define E0000_BIT       2

typedef struct {
    VOID    *PciIo;
    UINT8   Irq;
} PROGRAMMED_PCIIRQ_CTX;

VOID SignalAmiLegacyBootEvent(VOID);

// CLP support related definitions

// The following PCIR data structure is the extended version of PCI_DATA_STRUCTURE
// from PCI.H. It can be removed once PCI header file is generically updated.
typedef struct {
    UINT32          Signature;
    UINT16          VendorId;
    UINT16          DeviceId;
    UINT16          Reserved0;
    UINT16          Length;
    UINT8           Revision;
    UINT8           ClassCode[3];
    UINT16          ImageLength;
    UINT16          CodeRevision;
    UINT8           CodeType;
    UINT8           Indicator;
    UINT16          MaxRtImageLength;
    UINT16          CfgUtilPointer;
    UINT16          ClpEntryPoint;
} PCI_PCIR30_DATA_STRUCTURE;

#define EFI_CLP_PROTOCOL_GUID \
    {0xcbbee336, 0x2682, 0x4cd6, { 0x81, 0x8b, 0xa, 0xd, 0x96, 0x7e, 0x5a, 0x67 } }

typedef struct _EFI_CLP_PROTOCOL EFI_CLP_PROTOCOL;

typedef EFI_STATUS (*EFI_CLP_EXECUTE) (
    IN EFI_CLP_PROTOCOL *This,
    IN UINT8            *CmdInputLine,
    IN UINT8            *CmdResponseBuffer,
    OUT UINT32          *CmdStatus
);

typedef struct _EFI_CLP_PROTOCOL {
    EFI_HANDLE      Handle;
    UINT16          EntrySeg;
    UINT16          EntryOfs;
    EFI_CLP_EXECUTE Execute;
} EFI_CLP_PROTOCOL;

typedef struct _CSM_PLATFORM_POLICY_DATA {
    EFI_PCI_IO_PROTOCOL *PciIo;
	VOID				**Rom;
    BOOLEAN             ExecuteThisRom;
    EFI_HANDLE          PciHandle;
} CSM_PLATFORM_POLICY_DATA;

typedef struct _AMI_PCI_OPROM_VALIDATION_DATA {
    BOOLEAN     OemPciRomOverride;
    EFI_HANDLE  Handle;
    VOID        *PciRom;
    UINTN       PciRomSize;
    BOOLEAN     RomIsValid;
} AMI_PCI_OPROM_VALIDATION_DATA;

typedef	struct _AMI_OPROM_POLICY_PROTOCOL AMI_OPROM_POLICY_PROTOCOL;

typedef EFI_STATUS (*CHECK_UEFI_OPROM_POLICY) (
    IN	AMI_OPROM_POLICY_PROTOCOL *This,
    IN	UINT8                     PciClass
);

typedef VOID (*EXECUTE_OPROM_POLICY)(
    IN OPTIONAL CSM_PLATFORM_POLICY_DATA *OpRomStartEndProtocol
);

struct _AMI_OPROM_POLICY_PROTOCOL {
    CHECK_UEFI_OPROM_POLICY CheckUefiOpromPolicy;
    EXECUTE_OPROM_POLICY ProcessOpRom;
};

/**
    CSM16_CONFIGURATION definitions
**/
typedef enum {
    Csm16FeatureGet,
    Csm16FeatureSet,
    Csm16FeatureReset
} CSM16_CONFIGURATION_ACTION;

typedef enum {
    Csm16HeadlessI19Retry,
    Csm16I13MbrWriteProtect,
    Csm16LegacyFree,
    Csm16ZipHddEmulation,
    Csm16NoUsbBoot,
    Csm16I19Trap,
    Csm16Flexboot,
    Csm16GA20,
    Csm16SkipGA20Deactivation,
    Csm16Nmi,
    Csm16CpuRealModeOnBoot,
    Csm16PS2,
    Csm16LegacyToEfi,
    Csm16LegacyToEfiPonr,
    Csm16FeatureMax
} CSM16_FEATURE;

typedef EFI_STATUS (*EFI_AMICSM16_CONFIGURATION) (
    IN CSM16_CONFIGURATION_ACTION ConfigAction,
    IN CSM16_FEATURE Param,
    OUT OPTIONAL UINT32 *Data
);

/**
    OEM function prototypes
**/

#define CSM_OEM_FUNCTION_PROTOTYPES

typedef struct {
   UINTN        Seg;
   UINTN        Bus;
   UINTN        Dev;
   UINTN        Fun;
   UINTN        Flags;
   UINT8        DiskFrom;
   UINT8        DiskTo;
   VOID         *RomAddress;
   UINT32       RomSize;
} AMI_CSM_EXECUTED_PCI_ROM;

typedef struct {
    struct {
        UINT16  Offset;
        UINT16  Segment;
    } FarCall;
    EFI_IA32_REGISTER_SET   Regs;
    struct {
        UINT32  Stack;
        UINT32  StackSize;
    } StackData;
    BOOLEAN     isFarCall;  //if false, then INT86.
    UINT8       BiosInt;
} AMI_CSM_THUNK_DATA;

typedef EFI_STATUS (AMI_CSM_GET_OPROM_VIDEO_SWITCHING_MODE)(
    IN EFI_PCI_IO_PROTOCOL  *PciIo  OPTIONAL,
    IN UINT16   TextSwitchingMode,
    OUT UINTN   *SetTxtSwitchingMode
);
typedef EFI_STATUS (AMI_CSM_GET_CUSTOM_PCI_PIRQ_MASK)(
    IN EFI_PCI_IO_PROTOCOL  *PciIo,
    IN UINT16   IrqMask,
    OUT UINTN   *ModifiedIrqMask
);

typedef EFI_STATUS (AMI_CSM_GET_GATE_A20_INFORMATION)(
    OUT UINTN   *GateA20Info
);

typedef EFI_STATUS (AMI_CSM_GET_NMI_INFORMATION)(
    OUT UINTN   *NmiInfo
);

typedef EFI_STATUS (AMI_CSM_GET_PLATFORM_HANDLE)(
    EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *This,
    EFI_GET_PLATFORM_HANDLE_MODE    Mode,
    UINT16      Type,
    EFI_HANDLE  **HandleBuffer,
    UINTN       *HandleCount,
    VOID        OPTIONAL **AdditionalData
);

typedef EFI_STATUS (AMI_CSM_GET_PLATFORM_EMBEDDED_ROM)(
    UINT16      ModuleId,
    UINT16      VendorId,
    UINT16      DeviceId,
    VOID        **ImageStart,
    UINTN       *ImageSize
);

typedef EFI_STATUS (AMI_CSM_GET_PLATFORM_PCI_EMBEDDED_ROM)(
    IN EFI_PCI_IO_PROTOCOL *PciIo,
    OUT VOID    **PciRom,
    OUT UINTN   *PciRomSize
);

typedef EFI_STATUS (AMI_CSM_CHECK_OEM_PCI_SIBLINGS)(
    EFI_PCI_IO_PROTOCOL *PciIo,
    AMI_CSM_EXECUTED_PCI_ROM *ExecutedRom
);

typedef EFI_STATUS (AMI_CSM_ENABLE_OEM_PCI_SIBLINGS)(
    EFI_PCI_IO_PROTOCOL *PciIo
);

typedef EFI_STATUS (AMI_CSM_16_CALL_COMPANION)(
    IN OUT AMI_CSM_THUNK_DATA *ThunkData,
    IN BOOLEAN Priority
);

typedef EFI_STATUS (AMI_CSM_GET_ROUTING_TABLE)(
    IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *This,
    OUT VOID  **RoutingTable,
    OUT UINTN *RoutingTableEntries,
    OUT VOID  **LocalPirqTable, OPTIONAL
    OUT UINTN *PirqTableSize, OPTIONAL
    OUT VOID  **LocalIrqPriorityTable, OPTIONAL
    OUT UINTN *IrqPriorityTableEntries OPTIONAL);

typedef EFI_STATUS (AMI_CSM_BSP_UPDATE_PRT)(
    IN EFI_LEGACY_BIOS_PROTOCOL *This, IN  VOID *RoutingTable
);

typedef EFI_STATUS (AMI_CSM_BSP_PREPARE_TO_BOOT)(
    IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
    IN  BBS_BBS_DEVICE_PATH *BbsDevicePath,
    IN  VOID                *BbsTable,
    IN  UINT32              LoadOptionsSize,
    IN  VOID                *LoadOptions,
    IN  VOID                *EfiToLegacyBootTable
);

/**
    EFI_LEGACY_BIOS_PLATFORM_GET_PLATFORM_INFO extension definitions
**/
typedef enum {
  EfiGetPlatformOpromVideoMode,
  EfiGetPlatformIntSaveRestoreTable,
  EfiGetPlatformPciIrqMask,
  EfiGetPlatformGateA20Information,
  EfiGetPlatformNmiInformation,
  EfiGetBcvSkipDeviceList,
} EFI_GET_PLATFORM_INFO_EXT_MODE;

EFI_STATUS GetPlatformInfoEx (
  IN  EFI_LEGACY_BIOS_EXT_PROTOCOL      *This,
  IN  EFI_GET_PLATFORM_INFO_EXT_MODE    Mode,
  IN OUT VOID                           **Table,
  IN OUT UINTN                          *TableSize,
  IN OUT UINTN                          *Location,
  OUT UINTN                             *Alignment,
  IN UINT16                             LegacySegment,
  IN UINT16                             LegacyOffset
);

typedef EFI_STATUS (EFIAPI *EFI_AMICSM_GET_PLATFORM_INFO) (
    IN  EFI_LEGACY_BIOS_EXT_PROTOCOL    *This,
    IN  EFI_GET_PLATFORM_INFO_EXT_MODE  Mode,
    IN OUT VOID                         **Table,
    IN OUT UINTN                        *TableSize,
    IN OUT UINTN                        *Location,
    OUT UINTN                           *Alignment,
    IN UINT16                           LegacySegment,
    IN UINT16                           LegacyOffset
);

/**
    EFI_LEGACY_BIOS_EXT_PROTOCOL: extends LegacyBios protocol with the 
    AMI implementation specific CSM functions and data.
**/

typedef struct _EFI_LEGACY_BIOS_EXT_PROTOCOL {
  EFI_AMICSM_GET_EMBEDDED_ROM GetEmbeddedRom;
  EFI_AMICSM_ADD_BBS_ENTRY AddBbsEntry;
  EFI_AMICSM_INSERT_BBS_ENTRY_AT InsertBbsEntryAt;
  EFI_AMICSM_REMOVE_BBS_ENTRY_AT RemoveBbsEntryAt;
  EFI_AMICSM_ALLOCATE_EBDA AllocateEbda;
  EFI_AMICSM_UNLOCK_SHADOW UnlockShadow;
  EFI_AMICSM_LOCK_SHADOW LockShadow;
  EFI_AMICSM_GET_16BIT_FUNC_ADDRESS Get16BitFuncAddress;
  EFI_AMICSM_COPY_LEGACY_TABLE CopyLegacyTable;
  EFI_AMICSM_INSTALL_ISA_ROM InstallIsaRom;
  EFI_AMICSM_GET_SHADOWRAM_ADDRESS GetShadowRamAddress;
  EFI_AMICSM_GET_BBS_TABLE GetBbsTable;
  EFI_AMICSM_GET_PLATFORM_INFO GetPlatformInfoEx;
  EFI_AMICSM16_CONFIGURATION Csm16Configuration;
} EFI_LEGACY_BIOS_EXT_PROTOCOL;

#pragma pack ()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
