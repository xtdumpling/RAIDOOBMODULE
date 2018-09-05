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

/** @file Csm.h
    AMI CSM header file.

**/
//**********************************************************************

#ifndef __CSM_HEADER__
#define __CSM_HEADER__

#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyRegion2.h>
#include <Protocol/Legacy8259.h>
#include <Protocol/LegacyInterrupt.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/PciIo.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/DiskInfo.h>
#include "Token.h"
#ifdef GenericSio_SUPPORT
#include <GenericSio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//****************************************************
// CSM16 related equates
//****************************************************
// misc_info bits
#define CSM16_HEADLESS_INT19_RETRY_BIT  0x80
#define CSM16_I19_TRAP_BIT              0x40
#define CSM16_I13_HDD_MBR_WP_BIT        0x20
#define CSM16_NO_USB_BOOT_BIT           0x10
#define CSM16_NO_KBC_PRESENT_BIT        0x08
#define CSM16_ZIP_HDD_EMUL_BIT          0x04
#define CSM16_FLEXBOOT_ENABLE_BIT       0x02
#define CSM16_FLEXBOOT_ENABLE_SHIFT_CNT 1
#define CSM16_FAST_GATE_A20_BIT         0x01

// rt_cmos_byte bits
#define CSM_RT_CMOS_LTE_PONR_BIT        0x02
#define CSM_RT_CMOS_PARITY_BIT          0x08
#define CSM_RT_CMOS_SKIP_GA20_DEACTIVATION_BIT  0x10
#define CSM_RT_CPU_RM_ONBOOT_BIT        0x20
#define CSM_RT_CMOS_PS2_BIT             0x40
#define CSM_RT_CMOS_LTE_BIT             0x80

// ACPI timer 
#define ACPI_TMR_SIGNATURE              0x544D // Signature used is 'TM'
#define ACPI_TMR_WIDTH_SHIFT            31
#define ACPI_TMR_SIGNATURE_SHIFT        16

BOOLEAN EFIAPI Int86 (
    IN EFI_LEGACY_BIOS_PROTOCOL   *This,
    IN  UINT8                     BiosInt,
    IN  EFI_IA32_REGISTER_SET     *Regs
);

BOOLEAN EFIAPI FarCall86 (
  IN EFI_LEGACY_BIOS_PROTOCOL     *This,
  IN  UINT16                      Segment,
  IN  UINT16                      Offset,
  IN  EFI_IA32_REGISTER_SET       *Regs,
  IN  VOID                        *Stack,
  IN  UINTN                       StackSize
);

EFI_STATUS EFIAPI CheckPciRom (
  IN EFI_LEGACY_BIOS_PROTOCOL     *This,
  IN  EFI_HANDLE                  PciHandle,
  OUT VOID                        **RomImage, OPTIONAL
  OUT UINTN                       *RomSize,   OPTIONAL
  OUT UINTN                       *Flags
);

EFI_STATUS EFIAPI InstallPciRom (
  IN EFI_LEGACY_BIOS_PROTOCOL      *This,
  IN  EFI_HANDLE                   PciHandle,
  IN  VOID                         **RomImage,
  OUT UINTN                        *Flags,
  OUT UINT8                        *DiskStart, OPTIONAL
  OUT UINT8                        *DiskEnd, OPTIONAL
  OUT VOID                         **RomShadowAddress, OPTIONAL
  OUT UINT32                       *ShadowedRomSize OPTIONAL
);

EFI_STATUS EFIAPI
ShadowAllLegacyOproms (
    IN EFI_LEGACY_BIOS_PROTOCOL *This
);

EFI_STATUS FindEmbeddedRom(
    UINT16 ModuleId, UINT16 VendorId, UINT16 DeviceId,
    VOID **ImageStart, UINTN *ImageSize
);

EFI_STATUS
LoadFileSection(
    IN      EFI_GUID                  *Guid,
    IN OUT  VOID                      **Buffer,
    IN OUT  UINTN                     *BufferSize
);

EFI_STATUS EFIAPI
GetLegacyRegion (
  IN EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN UINTN                             LegacyMemorySize,
  IN UINTN                             Region,
  IN UINTN                             Alignment,
  OUT VOID                             **LegacyMemoryAddress
);

EFI_STATUS EFIAPI
CopyLegacyRegion (
  IN EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN UINTN                             LegacyMemorySize,
  IN VOID                              *LegacyMemoryAddress,
  IN VOID                              *LegacyMemorySourceAddress
);

EFI_STATUS EFIAPI
UnlockShadow(
    IN UINT8    *Address,
    IN UINTN    Size,
    OUT UINT32  *LockUnlockAddr,
    OUT UINT32  *LockUnlockSize
);

EFI_STATUS EFIAPI
LockShadow(
    IN UINT32  LockAddr,
    IN UINT32  LockSize
);

EFI_STATUS
RemoveDecodingForShadow(
    IN UINT32  DecodeAddr
);

// Aptio V Server override : to fix the build error in csm.c
EFI_STATUS GetSystemMemoryMap(
    OUT EFI_MEMORY_DESCRIPTOR **MemMap,
    OUT UINTN *MemDescSize,
    OUT UINTN *MemEntriesCount
);
typedef enum {FLOPPY_INSERTED, NO_FLOPPY_DISK, NO_FLOPPY_DRIVE} FDD_STATUS;

#pragma pack (1)

#define MAX_EXECUTED_OPROMS 32

/**
    AMI IRQ Routing Table Header Structure

**/

typedef struct {
    UINT32  dSignature;         ///< '$IRT' signature
    UINT8   bTotalEntries;      ///< total number of slot entries present in the table
    UINT8   bUsedEntries;       ///< number of used slot entries in the table
    UINT16  wExclusiveIRQ;      ///< bitmap of IRQs used exclusively for PCI
    UINT32  dPtrIRQSlotStart;   ///< pointer to start of IRQ slot entries
} AMIIRQ_HEADER;


/**
    Table that holds the information about the buses generated by
    P2P bridges that are not listed in $PIR table

**/

typedef struct {
  UINT8                    Bus;
  EFI_LEGACY_PIRQ_ENTRY    PirqEntry[4];
} EFI_ADDON_PCIBUS_TABLE;




#define MAX_DRIVE_NAME 32

typedef struct _DRIVE_DISPLAY_NAME {
    UINT8   DriveName[MAX_DRIVE_NAME];
} DRIVE_DISPLAY_NAME;

typedef struct _IDE_CONTROLLER_INFO_STRUC {
    UINT16      BusDevFun;                  ///< Bus, Device and Function number
    UINT8       ControllerNo;               ///< Controller number for IDE controller
} IDE_CONTROLLER_INFO_STRUC;

typedef struct {
    UINT8   ThunkCode[0x3000];              ///< 12KB = 4KB of code + 8KB of stack
    UINT8   InterruptRedirectionCode[24];   ///< 3 bytes of code per interrupt
    EFI_TO_COMPATIBILITY16_INIT_TABLE   Csm16InitTable;
    EFI_TO_COMPATIBILITY16_BOOT_TABLE   Csm16BootTable;
    EFI_LEGACY_INSTALL_PCI_HANDLER  PciHandler;
    EFI_DISPATCH_OPROM_TABLE        DispatchOpromTable;
    DRIVE_DISPLAY_NAME DriveDisplayName[2*MAX_IDE_CONTROLLER];
} THUNK_MEMORY;

#define MAX_E820_ENTRIES 256
typedef enum {
    MemRangeAvl         = 1,
    MemRangeRes         = 2,
    MemRangeAcpiReclaim = 3,
    MemRangeAcpiNVS     = 4,
    MemRangePersistentMemory = 7
} E820_MEMORY_TYPE;

#define E820_EXTATTR_ADDRESS_RANGE_ENABLED 1
#define E820_EXTATTR_ADDRESS_RANGE_NONVOLATILE 2
typedef struct {
    UINT64              Adr;
    UINT64              Len;
    E820_MEMORY_TYPE    Type;
    UINT32              ExtendedAttr;
} E820_ENTRY;

#define PMM_TABLE_SIZE 0x1000
#define PMM_SYSTEM_HANDLE 0x24494D41        ///< Handle used by AMIBIOS 'AMI$'
#define EFI_MEMORY_HANDLE 0x24494645        ///< Handle used by EFI Memory 'EFI$'
typedef struct {
    UINT32              StartAddress;       ///< Start address of the memory block
    UINT32              Size;               ///< Size of the memory block in bytes
    UINT32              Handle;             ///< Associated handle
    UINT8               Reserved[4];        ///< Reserved for future use
} MEMORY_ALLOCATION_ENTRY;

typedef struct {
    UINT32              Signature;
    UINT8               Revision;
    UINT8               Length;
    UINT16              NextHeaderOffset;
    UINT8               Reserved;
    UINT8               Checksum;
    UINT32              DevId;
    UINT16              MfgPtr;
    UINT16              ProductNamePtr;
    UINT8               Class;
    UINT8               SubClass;
    UINT8               Interface;
    UINT8               DevIndicators;
    UINT16              BCV;
    UINT16              DisconnectVector;
    UINT16              BEV;
    UINT16              Reserved1;
    UINT16              StaticResourceVector;
} PCI_PNP_EXPANSION_HEADER;

#define MAX_BCV_OPROMS  32
typedef struct {
    UINT8   *Address;
    UINT8   *Data;
    UINT8   *rtData;
    UINT8   *rtDataAddr;
    UINT32  rtDataSize;
    BOOLEAN isEbda;
    UINT32  ebdaOffset;
} SAVED_PCI_ROM;

typedef struct {
    UINT8   irq_num;
    UINT32  irq_adr;
} IVT_SAVE_RESTORE;


/**
    This structure is not specified in CSM specification. It defines the useful 
    fields for the global variable to be used throughout POST.

**/

typedef struct {
    EFI_LEGACY_BIOS_PROTOCOL            iBios;          ///< LegacyBios instance pointer
    EFI_LEGACY_REGION2_PROTOCOL         *iRegion;       ///< LegacyRegion instance pointer
    EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *iBiosPlatform; ///< LegacyPlatform instance pointer
    EFI_LEGACY_INTERRUPT_PROTOCOL       *iInterrupt;    ///< LegacyInterrupt instance pointer
    EFI_LEGACY_8259_PROTOCOL            *i8259;         ///< Legacy8259 instance pointer
    EFI_HANDLE                          hBios;          ///<
    EFI_HANDLE                          hImage;         ///< This driver LoadedImage handle
    EFI_HANDLE                          hVga;           ///< Video controller handle
    EFI_COMPATIBILITY16_TABLE           *Csm16Header;   ///< Pointer to CSM16 header
    THUNK_MEMORY                        *Thunk;         ///< Pointer to thunk memory area
    UINT16                              Csm16EntrySeg;  ///< CSM16 entry point segment
    UINT16                              Csm16EntryOfs;  ///< CSM16 entry point offset
    BBS_TABLE                           *BbsTable;      ///< BBS table pointer
    UINT8                               BbsEntriesNo;   ///< Number of BBS entries
    UINT8                               HddCount;       ///< Number of hard disks in the system
    E820_ENTRY                          E820Map[MAX_E820_ENTRIES];  ///< E820 memory map entries
    UINT16                              NumberE820Entries;  ///< Number of E820 entries populated
} BIOS_INFO;

typedef struct {
    EFI_HANDLE  hImage;
    EFI_HANDLE  hBios;
    EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   iBiosPlatform;
} PLATFORM_BIOS_INFO;

EFI_STATUS BspUpdatePrt(
  IN  EFI_LEGACY_BIOS_PROTOCOL   *This,
  IN  VOID                       *RoutingTable
);

typedef struct _OEMINT_FUNC {
    UINT16  interrupt;
    UINT16  funcoffset;
} OEMINT_FUNC;

typedef struct _OEM16_FUNC {
    UINT16  funcID;
    UINT16  funcOffset;
} OEM16_FUNC;

typedef struct _LEGACY16_TO_EFI_DATA_TABLE_EXT {
    LEGACY16_TO_EFI_DATA_TABLE_STRUC    PublicData;
    UINT16  RbMinBusArrayOfs;
    UINT8   RbArrayCount;
} LEGACY16_TO_EFI_DATA_TABLE_EXT;


#ifndef CSM_OEM_FUNCTION_PROTOTYPES

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
    IN UINT16               TextSwitchingMode,
    OUT UINTN               *SetTxtSwitchingMode
);

typedef EFI_STATUS (AMI_CSM_GET_CUSTOM_PCI_PIRQ_MASK)(
    IN EFI_PCI_IO_PROTOCOL  *PciIo,
    IN UINT16               IrqMask,
    OUT UINTN               *ModifiedIrqMask
);

typedef EFI_STATUS (AMI_CSM_GET_GATE_A20_INFORMATION)(
    OUT UINTN *GateA20Info
);

typedef EFI_STATUS (AMI_CSM_GET_NMI_INFORMATION)(
    OUT UINTN   *NmiInfo
);

typedef EFI_STATUS (AMI_CSM_GET_PLATFORM_HANDLE)(
    EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *This,
    EFI_GET_PLATFORM_HANDLE_MODE      Mode,
    UINT16                            Type,
    EFI_HANDLE                        **HandleBuffer,
    UINTN                             *HandleCount,
    VOID                              OPTIONAL **AdditionalData
);

typedef EFI_STATUS (AMI_CSM_GET_PLATFORM_EMBEDDED_ROM)(
    UINT16 ModuleId,
    UINT16 VendorId,
    UINT16 DeviceId,
    VOID **ImageStart,
    UINTN *ImageSize
);

typedef EFI_STATUS (AMI_CSM_GET_PLATFORM_PCI_EMBEDDED_ROM)(
    IN EFI_PCI_IO_PROTOCOL *PciIo,
    OUT VOID **PciRom,
    OUT UINTN *PciRomSize
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
#endif

#pragma pack ()

EFI_STATUS  EFIAPI Get16BitFuncAddress (UINT16, UINT32*);
UINTN       EFIAPI CopyLegacyTable(VOID*, UINT16, UINT16, UINT16);
EFI_STATUS  EFIAPI GetEmbeddedRom(UINT16, UINT16, UINT16, VOID**, UINTN*);
EFI_STATUS  EFIAPI GetBbsTable (BBS_TABLE**, UINT8*);
EFI_STATUS  EFIAPI GetPlatformInformationEx (EFI_LEGACY_BIOS_EXT_PROTOCOL*, EFI_GET_PLATFORM_INFO_EXT_MODE, VOID**, UINTN*, UINTN*, UINTN*, UINT16, UINT16);
EFI_STATUS  EFIAPI AddBbsEntry(BBS_TABLE*);
EFI_STATUS  EFIAPI InsertBbsEntryAt(EFI_LEGACY_BIOS_EXT_PROTOCOL*, BBS_TABLE*, UINT8*);
EFI_STATUS  EFIAPI RemoveBbsEntryAt(EFI_LEGACY_BIOS_EXT_PROTOCOL*, UINT8);
EFI_STATUS  EFIAPI AllocateEbda(EFI_LEGACY_BIOS_EXT_PROTOCOL*, UINT8, UINT32*, UINT32*);
EFI_STATUS  EFIAPI UnlockShadow(UINT8*, UINTN, UINT32*, UINT32*);
EFI_STATUS  EFIAPI LockShadow(UINT32, UINT32);
EFI_STATUS  InitCompatibility16(BIOS_INFO*, EFI_SYSTEM_TABLE*);
EFI_STATUS  InitializeThunk(BIOS_INFO*);
EFI_STATUS  InitializeLegacyMemory(BIOS_INFO*);
EFI_STATUS  EFIAPI PrepareToBoot(EFI_LEGACY_BIOS_PROTOCOL*, UINT16*, BBS_TABLE**);
EFI_STATUS  EFIAPI LegacyBoot(EFI_LEGACY_BIOS_PROTOCOL*, BBS_BBS_DEVICE_PATH*, UINT32, VOID*);
EFI_STATUS  EFIAPI GetBbsInfo(EFI_LEGACY_BIOS_PROTOCOL*, UINT16*, HDD_INFO**, UINT16*, BBS_TABLE**);
EFI_STATUS  EFIAPI UpdateKeyboardLedStatus(EFI_LEGACY_BIOS_PROTOCOL*, UINT8);
EFI_STATUS  EnableDisableNmi();
VOID        HaltApsBeforeBoot();
VOID        InstallLegacyMassStorageDevices();
VOID        GetExtendedMemSize(UINT32*);
UINT32      UpdateE820Map(E820_ENTRY *);
EFI_STATUS  UpdateCsm16Configuration(EFI_TO_COMPATIBILITY16_BOOT_TABLE*);
EFI_STATUS  InitCsmBoard(EFI_HANDLE, EFI_SYSTEM_TABLE*);
EFI_STATUS  InitCsmSimpleIn(EFI_HANDLE, EFI_SYSTEM_TABLE*);
EFI_STATUS  UpdateCmos();
EFI_STATUS  InitializePortingHooks (BIOS_INFO*);
OEM16_FUNC* InitializeOemInt(UINT16*);
UINTN       InitializeOem16(UINT16*);
FDD_STATUS  GetFDDStatus();
VOID        AllConnectedCallback (EFI_EVENT, VOID*);
EFI_STATUS  GetPlatformEmbeddedRom(UINT16, UINT16, UINT16, VOID**, UINTN*);
EFI_STATUS  GetComPortResource(UINT8, UINT16*, UINT8*);
EFI_STATUS  GetLptResource(UINT16*, UINT8*);
EFI_STATUS  UpdatePciLastBus();
VOID        UpdatePciLastBusCallback(EFI_EVENT, VOID*);
EFI_STATUS  GetPlatformPciEmbeddedRom(EFI_PCI_IO_PROTOCOL*, VOID**, UINTN*);
EFI_STATUS  CheckOemPciSiblings(EFI_PCI_IO_PROTOCOL*, AMI_CSM_EXECUTED_PCI_ROM*);
EFI_STATUS  EnableOemPciSiblings(EFI_PCI_IO_PROTOCOL*);
EFI_STATUS  EFIAPI InstallIsaRom(EFI_LEGACY_BIOS_EXT_PROTOCOL*, UINTN);
EFI_STATUS  EFIAPI GetShadowRamAddress(UINT32*, UINT32, UINT32, UINT32);
EFI_STATUS  GetOpromVideoSwitchingMode(EFI_PCI_IO_PROTOCOL*, UINT16, UINTN*);

VOID        DisconnectSerialIO();
VOID        ConnectSerialIO();
EFI_STATUS  LockConsole();
EFI_STATUS  UnlockConsole();
BOOLEAN     IsAMICSM16(EFI_COMPATIBILITY16_TABLE*);
UINT8       ChecksumCSM16Header(EFI_COMPATIBILITY16_TABLE*);
VOID        UpdateEbdaMap(UINT32);
BOOLEAN     Check30ROM(VOID*, UINTN*, EFI_HANDLE);
EFI_STATUS  GetGateA20Information(UINTN*);
EFI_STATUS  GetNmiInformation(UINTN*);
EFI_STATUS  GetCustomPciIrqMask (EFI_PCI_IO_PROTOCOL*, UINT16, UINTN*);
BOOLEAN     CsmIsTobeLoaded(EFI_HANDLE);
VOID        SignalProtocolEvent(EFI_GUID*, VOID*);
EFI_STATUS  PreProcessOpRom(EFI_HANDLE, VOID**);
EFI_STATUS  PostProcessOpRom();
EFI_STATUS  EFIAPI  Csm16Configuration(CSM16_CONFIGURATION_ACTION, CSM16_FEATURE, UINT32*);
EFI_STATUS  Csm16CallCompanion(AMI_CSM_THUNK_DATA*, BOOLEAN);

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
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
