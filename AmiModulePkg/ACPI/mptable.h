//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file mptable.h
    Header file for MP table builder

**/

#ifndef __MPTABLE__H__
#define __MPTABLE__H__
#ifdef __cplusplus
extern "C" {
#endif

//------------------------ Include files ------------------------------
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Token.h>

//------------------------ Type declarations --------------------------
#define MP_TABLE_MAX_BUS_ENTRIES        0xFF //from 0 to 0xFE, FF reserved means no parent bus
#define MP_TABLE_MAX_IO_APIC_ENTRIES    0x10

#define MP_FLOATING_POINTER_SIGNATURE 0x5F504D5F  // _MP_
#define MP_CONFIGURATION_TABLE_SIGNATURE 0x504D4350     // PCMP

#define MP_TABLE_VERSION_1_1    0x1
#define MP_TABLE_VERSION_1_4    0x4

#define MP_TABLE_CPU_ENTRY_TYPE                  0
#define MP_TABLE_BUS_ENTRY_TYPE                  1
#define MP_TABLE_IO_APIC_ENTRY_TYPE              2
#define MP_TABLE_INT_ASSIGN_ENTRY_TYPE           3
#define MP_TABLE_LOCAL_INT_ASSIGN_ENTRY_TYPE     4

#define MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY_TYPE              128
#define MP_EX_TABLE_BUS_HIERARCHY_ENTRY_TYPE                129
#define MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY_TYPE  130

#define IO_APIC_BASE_ADDRESS_BOTTOM	APCB        //defined earlier
#define IO_APIC_SEARCH_STEP		    0x1000

#define IO_APIC_INDEX_REG		0x0
#define IO_APIC_DATA_REG		0x4 //(0x10 in bytes = 0x4 in dwords

#define IO_APIC_MAP_ID_REG		    0x0
#define IO_APIC_MAP_VERSION_REG		0x1

#define INT_TYPE_INT        0
#define INT_TYPE_NMI        1
#define INT_TYPE_SMI        2
#define INT_TYPE_EXT_INT    3

#define POLARITY_CONFORM_SPEC   0
#define POLARITY_ACTIVE_HIGH    1
#define POLARITY_ACTIVE_LOW     3

#define TRIGGER_MODE_CONFORM_SPEC    0
#define TRIGGER_MODE_EDGE_TRIGGERED  1
#define TRIGGER_MODE_LEVEL_TRIGGERED 3

#define BUS_TYPE_ISA    5

#pragma pack(1)

/**
    This structure represents MP table floating pointer format
    as defined in MPS specification v1.4

**/

typedef struct _MP_FOATING_POINTER
{
    UINT32  Signature; 
    UINT32  PhysicalAddresPointer;
    UINT8   Length;
    UINT8   VersionNumber;
    UINT8   CheckSum;
    UINT8   DefaultConfiguration;
    UINT8   IMCRflag;
    UINT8   Reserved[3];
} MP_FLOATING_POINTER;

/**
    This structure represents MP configuration table header format
    as defined in MPS specification v1.4

**/

typedef struct _MP_CONFIGURATION_TABLE_HEADER
{
    UINT32  Signature;
    UINT16  BaseTableLength;
    UINT8   VersionNumber;
    UINT8   CheckSum;
    UINT8   OemId[8];
    UINT8   ProductId[12];
    UINT32  OemTablePointer;
    UINT16  OemTableSize;
    UINT16  BaseTableEntryCount;
    UINT32  LocalApicAddress;
    UINT16  ExtendedTableLength;
    UINT8   ExtendedCheckSum;
    UINT8   Reserved;
} MP_CONFIGURATION_TABLE_HEADER;

/**
    This structure represents MP configuration table CPU entry format
    as defined in MPS specification v1.4

**/

typedef struct _MP_TABLE_CPU_ENTRY
{
    UINT8   EntryType;
    UINT8   LocalApicId;
    UINT8   LocalApicVersion;
    struct  
    {
        UINT8   CpuEnable    : 1;
        UINT8   CpuBootStrap : 1;
        UINT8   Reserved     : 6;
    } CpuFlags;
    UINT32  CpuSignature;
    UINT32  FeatureFlags;
    UINT32  Reserved[2];
} MP_TABLE_CPU_ENTRY;

/**
    This structure represents MP configuration table bus entry format
    as defined in MPS specification v1.4

**/

typedef struct _MP_TABLE_BUS_ENTRY
{
    UINT8   EntryType;
    UINT8   BusId;
    UINT8   BusTypeString[6];
} MP_TABLE_BUS_ENTRY;

/**
    This structure represents MP configuration table IO APIC entry format
    as defined in MPS specification v1.4

**/

typedef struct _MP_TABLE_IO_APIC_ENTRY
{
    UINT8   EntryType;
    UINT8   IoApicId;
    UINT8   IoApicVersion;
    UINT8   Flags;

    UINT32  IoApicAddress;
} MP_TABLE_IO_APIC_ENTRY;

typedef union
{
    UINT8   IsaBusIrq;
    struct
    {
        UINT8   PciIntSignal    : 2;
        UINT8   PciDeviceNumber : 5;
        UINT8   Reserved        : 1;
    } PciBusIrq;
} SOURCE_IRQ;

/**
    This structure represents MP configuration table interrupt assingment
    entry format as defined in MPS specification v1.4

**/

typedef struct _MP_TABLE_INT_ASSIGN_ENTRY
{
    UINT8   EntryType;
    UINT8   InterruptType;
    UINT16  Flags;    
    UINT8   SourceBusId;
    SOURCE_IRQ   SourceBusIrq;
    UINT8   DestIoApicId;
    UINT8   DestIoApicItin;
} MP_TABLE_INT_ASSIGN_ENTRY;

/**
    This structure represents MP table local interrupt assingment
    entry format as defined in MPS specification v1.4

**/

typedef struct _MP_TABLE_LOCAL_INT_ASSIGN_ENTRY
{
    UINT8   EntryType;
    UINT8   InterruptType;
    struct
    {
        UINT16  Polarity    : 2;
        UINT16  TriggerMode : 2;
        UINT16  Reserved    : 12;
    } InterruptSignal;
    UINT8   SourceBusId;
    UINT8   SourceBusIrq;
    UINT8   DestLocalApicId;
    UINT8   DestLocalApicItin;
} MP_TABLE_LOCAL_INT_ASSIGN_ENTRY;

/**
    This structure represents MP table system address map extended
    entry format as defined in MPS specification v1.4

**/

typedef struct _MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY
{
    UINT8 EntryType;
    UINT8 EntryLength;
    UINT8 BusId;
    UINT8 AddressType;
    UINT64 AddressBase;
    UINT64 AddressLength;
} MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY;

/**
    This structure represents MP table bus hierarchy extended
    entry format as defined in MPS specification v1.4

**/

typedef struct _MP_EX_TABLE_BUS_HIERARCHY_ENTRY
{
    UINT8 EntryType;
    UINT8 EntryLength;
    UINT8 BusId;
    UINT8 BusInfo;
    UINT8 ParentBusId;
    UINT8 Reserved[3];
} MP_EX_TABLE_BUS_HIERARCHY_ENTRY;

/**
    This structure represents MP table compatibility bus address modifier 
    extended entry format as defined in MPS specification v1.4

**/

typedef struct _MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY
{
    UINT8 EntryType;
    UINT8 EntryLength;
    UINT8 BusId;
    UINT8 AddressModifier;
    UINT32 RangeList;
} MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY;

//----------------------- Additional structures -----------------------

/**
    PCI bus description data structure
 
**/

typedef struct _BUS_INFO
{
    UINT8   BusId;
    UINT8   ParentBusId;
    UINT8   BusType;
    UINT8   Flags;
} BUS_INFO;

/**
    This structure represents ACPI address space descriptor
    format as defined in ACPI specification v3.0

**/

typedef struct _ACPI_ADDRESS_SPACE_DESC
{
	UINT8	Signature;
	UINT16	DescLength;
	UINT8	ResourceType;
	UINT8	GeneralFlags;
	UINT8	TypeSpecificFlags;
	UINT64	AddressSpaceGranularity;
	UINT64	AddressRangeMin;
	UINT64	AddressRangeMax;
	UINT64	AddressTranslationOffset;
	UINT64	AddressLength;
} ACPI_ADDRESS_SPACE_DESC;

/**
    Device interrupt destination description data structure
 
**/

typedef struct _IO_APIC_DEST
{
    UINT8   IoApicItin;
    UINT8   IoApicId;
} IO_APIC_DEST;

#if AmiBoardInfo_SUPPORT == 0
/**
    PCI interrupt routing description data structure
 
**/

typedef struct _PCI_IRQ_APIC_ROUTE
{
    UINT8   PciBusNumber;
    UINT8   DeviceNumber;
    IO_APIC_DEST Intn[4];
    UINT8   Reserved;
} PCI_IRQ_APIC_ROUTE;
#endif


/**
    IO APIC description data structure
 
**/

typedef struct _IO_APIC_INFO
{
    UINT8   IoApicId;
    UINT8   IoApicVersion;
    UINT8   IoApicFlags;
    UINT8   MaxRedirectionEntries;
    UINT32  IoApicAddress;
} IO_APIC_INFO;

#pragma pack()

//------------------------ Global variables ---------------------------


//------------------------ Functions prototypes -----------------------

EFI_STATUS MpsTableBuilderInit(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
    );

EFI_STATUS MpsTableFloatPointerModify(
    VOID
    );

EFI_STATUS MpsTableCreate(
    VOID
    );

VOID MpsTableReadyToBootNotify(
	IN EFI_EVENT Event, 
    IN VOID *Context
    );

EFI_STATUS MpsTableAddHeader(
    VOID
    );

UINT16 MpsTableAddCpuEntry(
    VOID
    );

UINT16 MpsTableAddBusEntry(
    VOID
    );

UINT16 MpsTableAddIoApicEntry(
    VOID
    );

UINT16 MpsTableAddIntAssignEntry(
    VOID
    );

UINT16 MpsTableAddLocalIntAssignEntry(
    VOID
    );

VOID MpsTableAddExtendedTable(
    VOID
    );

VOID MpsExTableAddSysAddressMapEntry(
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *pPciRootBridgeIoProtocol,
    IN UINT8 BusId,
    IN UINT8 BusAttributes
    );

VOID MpsExTableAddBusHierarchyEntry(
    VOID
    );

VOID MpsExTableAddCompatBusAddressModifierEntry(
    VOID
    );


//----------------------- Additional functions -------------------------

VOID QueryBusInfo(
    VOID
    );

VOID CollectIoApicInfo(
    IN EFI_PCI_IO_PROTOCOL *pPciIoProtocol OPTIONAL,
    IN UINT32 BaseAddress OPTIONAL
    );

VOID ArrangeIoApicEntries(
    VOID
    );

VOID GetIoApicId(
    IN UINT16   SysVectorNumber,
    OUT UINT8 *IoApicId,
    OUT UINT8 *IoApicItin
    );

EFI_STATUS GetCpuInfo(
    IN UINTN Cpu,
    OUT UINT8 *Cores,
    OUT UINT8 *Threads
    );

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
