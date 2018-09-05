//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

/** @file AmiSmbios.h
    AMI Smbios Header file.

**/

#ifndef _Smbios_DRIVER_H
#define _Smbios_DRIVER_H

#include <Token.h>
#include <Protocol/Smbios.h>

#define AMI_SMBIOS_PROTOCOL_GUID    {0x5e90a50d, 0x6955, 0x4a49, { 0x90, 0x32, 0xda, 0x38, 0x12, 0xf8, 0xe8, 0xe5 }}
#define	AMI_SMBIOS_POINTER_GUID		{0xc4f26989, 0xc1cf, 0x438b, { 0x88, 0x7c, 0x53, 0xd0, 0x98, 0x7d, 0x67, 0x72 }}
#define SMBIOS_3_TABLE_GUID			{0xf2fd1544, 0x9794, 0x4a2c, { 0x99, 0x2e, 0xe5, 0xbb, 0xcf, 0x20, 0xe3, 0x94 }}

extern EFI_GUID gAmiSmbiosProtocolGuid;
extern EFI_GUID gAmiSmbiosBoardProtocolGuid;
extern EFI_GUID gAmiSmbiosStaticDataGuid;
extern EFI_GUID gAmiSmbiosNvramGuid;
extern EFI_GUID gAmiSmbrunGuid;

//**********************************************************************//
//  DMI ERROR Codes
//**********************************************************************//

#define DMI_SUCCESS                 0x00
#define DMI_UNKNOWN_FUNCTION        0x81
#define DMI_FUNCTION_NOT_SUPPORTED  0x82
#define DMI_INVALID_HANDLE          0x83
#define DMI_BAD_PARAMETER           0x84
#define DMI_INVALID_SUBFUNCTION     0x85
#define DMI_NO_CHANGE               0x86
#define DMI_ADD_STRUCTURE_FAILED    0x87
#define DMI_READ_ONLY               0x8D
#define DMI_LOCK_NOT_SUPPORTED      0x90
#define DMI_CURRENTLY_LOCKED        0x91
#define DMI_INVALID_LOCK            0x92
#define DMI_BUFFER_TOO_SMALL        0x93
#define DMI_BUFFER_NOT_ALLOCATED    0x94

//**********************************************************************//

#pragma pack(1)

/**
    SMBIOS Structure Table Entry Point structure
**/
typedef struct {
    UINT8   AnchorString[5];
    UINT8   EntryPointStructureChecksum;
    UINT8   EntryPointLength;
    UINT8   MajorVersion;
    UINT8   MinorVersion;
    UINT8   Docrev;
    UINT8   EntryPointRevision;
    UINT8   Reserved;
    UINT32  TableMaximumSize;
    UINT64  TableAddress;
} SMBIOS_3X_TABLE_ENTRY_POINT;

#ifndef __SMBIOS_STANDARD_H__
typedef struct {
    UINT8   AnchorString[4];
    UINT8   EntryPointStructureChecksum;
    UINT8   EntryPointLength;
    UINT8   MajorVersion;
    UINT8   MinorVersion;
    UINT16  MaxStructureSize;
    UINT8   EntryPointRevision;
    UINT8   FormattedArea[5];
    UINT8   IntermediateAnchorString[5];
    UINT8   IntermediateChecksum;
    UINT16  TableLength;
    UINT32  TableAddress;
    UINT16  NumberOfSmbiosStructures;
    UINT8   SmbiosBcdRevision;
} SMBIOS_TABLE_ENTRY_POINT;
#endif

/**
    SMBIOS Structure Header - Common for all structures
    Describing the type, size of the fixed area, and handle
    of the structure.
**/
typedef struct {
    UINT8   Type;
    UINT8   Length;
    UINT16  Handle;
} SMBIOS_STRUCTURE_HEADER;

/**
    BIOS Information Structure (Type 0)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     BiosVendor;                     // String number
    UINT8                     BiosVersion;                    // String number
    UINT16                    BiosStartingAddrSegment;
    UINT8                     BiosReleaseDate;                // String number
    UINT8                     BiosRomSize;
    UINT32                    BiosChar_1;
    UINT32                    BiosChar_2;
    UINT8                     BiosCharExtByte1;
    UINT8                     BiosCharExtByte2;
    UINT8                     SystemBiosMajorRelease;
    UINT8                     SystemBiosMinorRelease;
    UINT8                     ECFirmwareMajorRelease;
    UINT8                     ECFirmwareMinorRelease;
#if SMBIOS_SPEC_VERSION > 300
    UINT16					  ExtendedBiosRomSize;
#endif
} SMBIOS_BIOS_INFO;                                           // TYPE 0

/**
    System Information Structure (Type 1)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Manufacturer;                   // String number
    UINT8                     ProductName;                    // String number
    UINT8                     Version;                        // String number
    UINT8                     SerialNumber;                   // String number
    EFI_GUID                  Uuid;
    UINT8                     WakeupType;
    UINT8                     SkuNumber;
    UINT8                     Family;
} SMBIOS_SYSTEM_INFO;                                         // TYPE 1

#if BASE_BOARD_INFO
/**
    Base Board (or Module) Information Structure (Type 2)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Manufacturer;                   // String number
    UINT8                     ProductName;                    // String number
    UINT8                     Version;                        // String number
    UINT8                     SerialNumber;                   // String number
    UINT8                     AssetTag;                       // String number
    UINT8                     FeatureTag;
    UINT8                     Location;                       // String number
    UINT16                    ChassisHandle;
    UINT8                     BoardType;
    UINT8                     NumberOfObjectHandles;
#if NUMBER_OF_OBJECT_HANDLES != 0
    UINT8                     ObjectHandles[NUMBER_OF_OBJECT_HANDLES];
#endif
} SMBIOS_BASE_BOARD_INFO;                                     // TYPE 2
#endif

/**
    System Enclosure or Chassis Information Structure (Type 3)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Manufacturer;                   // String number
    UINT8                     Type;
    UINT8                     Version;                        // String number
    UINT8                     SerialNumber;                   // String number
    UINT8                     AssetTag;                       // String number
    UINT8                     BootupState;
    UINT8                     PowerSupplyState;
    UINT8                     ThermalState;
    UINT8                     SecurityStatus;
    UINT32                    OemDefined;
    UINT8                     Height;
    UINT8                     NumberOfPowerCord;
    UINT8                     ElementCount;
    UINT8                     ElementRecordLength;
#if ((ELEMENT_COUNT != 0) && (ELEMENT_LEN != 0))
    UINT8                     Elements[ELEMENT_COUNT * ELEMENT_LEN];
#endif
    UINT8                     SkuNumber;
} SMBIOS_SYSTEM_ENCLOSURE_INFO;                               // TYPE 3

/**
    Processor Information Structure (Type 4)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     SocketDesignation;              // String number
    UINT8                     ProcessotType;
    UINT8                     Family;
    UINT8                     ProcessorManufacturer;          // String number
    UINT32                    ProcessorID_1;
    UINT32                    ProcessorID_2;
    UINT8                     ProcessorVersion;               // String number
    UINT8                     Voltage;
    UINT16                    ExtClockFreq;
    UINT16                    MaxSpeed;
    UINT16                    CurrentSpeed;
    UINT8                     Status;
    UINT8                     Upgrade;
    UINT16                    L1CacheHandle;
    UINT16                    L2CacheHandle;
    UINT16                    L3CacheHandle;
    UINT8                     SerialNumber;                   // String number
    UINT8                     AssetTag;                       // String number
    UINT8                     PartNumber;                     // String number
    UINT8                     CoreCount;                      // Number of cores per processor socket
    UINT8                     CoreEnabled;                    // Number of enabled cores per processor socket
    UINT8                     ThreadCount;                    // Number of threads per processor socket
    UINT16                    ProcessorChar;                  // Defines which functions the processor supports
    UINT16                    Family2;
    UINT16                    CoreCount2;                     // Number of cores per processor socket for Core Counts > 255
    UINT16                    CoreEnabled2;                   // Number of enabled cores per processor socket for Core Counts > 255
    UINT16                    ThreadCount2;                   // Number of threads per processor socket for Core Count > 255
} SMBIOS_PROCESSOR_INFO;                                      // TYPE 4

#if MEM_CTRL_INFO
/**
    Memory Controller Information Structure (Type 5)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     ErrDetMethod;
    UINT8                     ECC;
    UINT8                     SupportInterleave;
    UINT8                     CurrentInterleave;
    UINT8                     MaxMemModuleSize;
    UINT16                    SupportSpeed;
    UINT16                    SupportMemTypes;
    UINT8                     MemModuleVoltage;
    UINT8                     NumberMemSlots;
    UINT16                    MemModuleConfigHandle[NUMBER_OF_MEM_MODULE];
    UINT8                     EnabledECC;
} SMBIOS_MEM_CONTROLLER_INFO;                                 // TYPE 5
#endif

#if MEM_MODULE_INFO
/**
    Memory Module Information Structure (Type 6)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     SocketDesignation;
    UINT8                     BankConnections;
    UINT8                     CurrentSpeed;
    UINT16                    CurrentMemType;
    UINT8                     InstalledSize;
    UINT8                     EnabledSize;
    UINT8                     ErrorStatus;
} SMBIOS_MEM_MODULE_INFO;                                     // TYPE 6
#endif

/**
    Cache Information Structure (Type 7)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     SocketDesignation;
    UINT16                    CacheConfig;
    UINT16                    MaxCacheSize;
    UINT16                    InstalledSize;
    UINT16                    SupportSRAM;
    UINT16                    CurrentSRAM;
    UINT8                     CacheSpeed;
    UINT8                     ErrorCorrectionType;
    UINT8                     SystemCacheType;
    UINT8                     Associativity;
#if SMBIOS_SPEC_VERSION > 300
    UINT32					  MaxCacheSize2;
    UINT32					  InstalledCacheSize2;
#endif
} SMBIOS_CACHE_INFO;                                          // TYPE 7

#if PORT_CONNECTOR_INFO
/**
    Port Connector Information Structure (Type 8)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     IntRefDesig;
    UINT8                     IntConnType;
    UINT8                     ExtRefDesig;
    UINT8                     ExtConnType;
    UINT8                     PortType;
} SMBIOS_PORT_CONN_INFO;                                      // TYPE 8
#endif

/**
    System Slot Information Structure (Type 9)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     SlotDesig;
    UINT8                     SlotType;
    UINT8                     SlotDataBusWidth;
    UINT8                     CurrentUsage;
    UINT8                     SlotLength;
    UINT16                    SlotID;
    UINT8                     SlotChar_1;
    UINT8                     SlotChar_2;
    UINT16                    SegGroupNumber;
    UINT8                     BusNumber;
    UINT8                     DevFuncNumber;
} SMBIOS_SYSTEM_SLOTS_INFO;                                   // TYPE 9

#if ONBOARD_DEVICE_INFO
/**
    Single Device Information
**/
typedef struct {
    UINT8                     DeviceType;
    UINT8                     DescStringNum;
} SINGLE_DEV_INFO;

/**
    On Board Devices Information Structure (Type 10)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    SINGLE_DEV_INFO    OnBoardDev;
} SMBIOS_ONBOARD_DEV_INFO;                                    // TYPE 10
#endif

#if OEM_STRING_INFO
/**
    OEM Strings Structure (Type 11)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Count;
} SMBIOS_OEM_STRINGS_INFO;                                    // TYPE 11
#endif

#if SYSTEM_CONFIG_OPTION_INFO
/**
    System Configuration Options (Type 12)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Count;
} SMBIOS_SYSTEM_CONFIG_INFO;                                  // TYPE 12
#endif

#if BIOS_LANGUAGE_INFO
/**
    BIOS Language Information (Type 13)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     InstallableLang;
    UINT8                     Flags;
    UINT8                     Reserved[15];
    UINT8                     CurrentLang;
} SMBIOS_BIOS_LANG_INFO;                                      // TYPE 13
#endif

#if EVENT_LOG_INFO
/**
    Event Log Descriptor - Part of the System Event Log (Type 15)
**/
typedef struct {
    UINT8                     LogType;
    UINT8                     VariableDataFormatType;
} EVENT_LOG_DESCRIPTOR;

/**
    System Event Log (Type 15)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT16                    LogAreaLength;
    UINT16                    LogHeaderStartOffset;
    UINT16                    LogDataStartOffset;
    UINT8                     AccessMethod;
    UINT8                     LogStatus;
    UINT32                    LogChangeToken;
    UINT32                    AccesMethodAddr;
    UINT8                     LogHeaderFormat;
    UINT8                     NumbetOfLogTypeDesc;
    UINT8                     LengthOfLogTypeDesc;
    EVENT_LOG_DESCRIPTOR      LogDescriptors[NO_OF_SUPPORTED_EVENTS];
} SMBIOS_EVENT_LOG_INFO;                                      // TYPE 15
#endif

/**
    Physical Memory Array (Type 16)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Location;
    UINT8                     Use;
    UINT8                     MemErrorCorrection;
    UINT32                    MaxCapacity;
    UINT16                    MemErrInfoHandle;
    UINT16                    NumberOfMemDev;
    UINT64                    ExtMaxCapacity;
} SMBIOS_PHYSICAL_MEM_ARRAY_INFO;                             // TYPE 16

/**
    Memory Device (Type 17)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT16                    PhysicalMemArrayHandle;
    UINT16                    MemErrorInfoHandle;
    UINT16                    TotalWidth;
    UINT16                    DataWidth;
    UINT16                    Size;
    UINT8                     FormFactor;
    UINT8                     DeviceSet;
    UINT8                     DeviceLocator;
    UINT8                     BankLocator;
    UINT8                     MemoryType;
    UINT16                    TypeDetail;
    UINT16                    Speed;
    UINT8                     Manufacturer;
    UINT8                     SerialNumber;
    UINT8                     AssetTag;
    UINT8                     PartNumber;
    UINT8                     Attributes;
    UINT32                    ExtendedSize;
    UINT16                    ConfMemClkSpeed;
    UINT16                    MinimumVoltage;
    UINT16                    MaximumVoltage;
    UINT16                    ConfiguredVoltage;
} SMBIOS_MEMORY_DEVICE_INFO;                                  // TYPE 17

#if MEMORY_ERROR_INFO
/**
    32-bit Memory Error Information (Type 18)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     ErrorType;
    UINT8                     ErrorGranularity;
    UINT8                     ErrorOperation;
    UINT32                    VendorSyndrome;
    UINT32                    MemArrayErrorAddress;
    UINT32                    DeviceErrorAddress;
    UINT32                    ErrorResolution;
} SMBIOS_MEMORY_ERROR_INFO;                                   // TYPE 18
#endif

/**
    Memory Array Mapped Address (Type 19)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT32                    StartingAddress;
    UINT32                    EndingAddress;
    UINT16                    MemoryArrayHandle;
    UINT8                     PartitionWidth;
    UINT64                    ExtendedStartAddr;
    UINT64                    ExtendedEndAddr;
} SMBIOS_MEM_ARRAY_MAP_ADDR_INFO;                             // TYPE 19

/**
    Memory Device Mapped Address (Type 20)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT32                    StartingAddress;
    UINT32                    EndingAddress;
    UINT16                    MemoryDeviceHandle;
    UINT16                    MemoryArrayMapAddrHandle;
    UINT8                     PartitionRowPosition;
    UINT8                     InterleavePosition;
    UINT8                     InterleaveDataDepth;
    UINT64                    ExtendedStartAddr;
    UINT64                    ExtendedEndAddr;
} SMBIOS_MEM_DEV_MAP_ADDR_INFO;                               // TYPE 20

#if BUILTIN_POINTING_DEVICE_INFO
/**
    Built-in Pointing Device (Type 21)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Type;
    UINT8                     Interface;
    UINT8                     NumberOfButtons;
} SMBIOS_POINTING_DEV_INFO;                                   // TYPE 21
#endif

#if PORTABLE_BATTERY_INFO
/**
    Portable Battery (Type 22)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Location;
    UINT8                     Manufacturer;
    UINT8                     ManufacturerData;
    UINT8                     SerialNumber;
    UINT8                     DeviceName;
    UINT8                     DeviceChemistry;
    UINT16                    DesignCapacity;
    UINT16                    DesignVoltage;
    UINT8                     SBDSVersion;
    UINT8                     MaxErrorInBatteryData;
    UINT16                    SBDSSerialNumber;
    UINT16                    SBDSManufacturerDate;
    UINT8                     SBDSDeviceChecmistry;
    UINT8                     DesignCapabilityMult;
    UINT32                    OEMSpecific;
} SMBIOS_PORTABLE_BATTERY_INFO;                               // TYPE 22
#endif

#if SYSTEM_RESET_INFO
/**
    System Reset (Type 23)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Capabilities;
    UINT16                    ResetCount;
    UINT16                    ResetLimit;
    UINT16                    TimerInterval;
    UINT16                    TimeOut;
} SMBIOS_SYSTEM_RESET_INFO;                                   // TYPE 23
#endif

#if HARDWARE_SECURITY_INFO
/**
    Hardware Security (Type 24)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     HardwareSecSettings;
} SMBIOS_HARDWARE_SECURITY_INFO;                              // TYPE 24
#endif

#if SYSTEM_POWER_CONTROLS_INFO
/**
    System Power Control (Type 25)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     NextSchedulePwrOnMonth;
    UINT8                     NextSchedulePwrOnDayOfMonth;
    UINT8                     NextSchedulePwrOnHour;
    UINT8                     NextSchedulePwrOnMinute;
    UINT8                     NextSchedulePwrOnSecond;
} SMBIOS_SYSTEM_PWR_CONTROL_INFO;                             // TYPE 25
#endif

#if VOLTAGE_PROBE_INFO
/**
    Voltage Probe (Type 26)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Description;
    UINT8                     LocationAndStatus;
    UINT16                    MaxValue;
    UINT16                    MinValue;
    UINT16                    Resolution;
    UINT16                    Tolerance;
    UINT16                    Accuracy;
    UINT32                    OEMDefine;
    UINT16                    NorminalValue;
} SMBIOS_VOLTAGE_PROBE_INFO;                                  // TYPE 26
#endif

#if COOLING_DEVICE_INFO
/**
    Cooling Device (Type 27)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT16                    TempProbeHandle;
    UINT8                     DevTypeAndStatus;
    UINT8                     CoolingUnitGroup;
    UINT32                    OEMDefine;
    UINT16                    NorminalSpeed;
    UINT8                     Description;
} SMBIOS_COOLING_DEV_INFO;                                    // TYPE 27
#endif

#if TEMPERATURE_PROBE_INFO
/**
    Temperature Probe (Type 28)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Description;
    UINT8                     LocationAndStatus;
    UINT16                    MaxValue;
    UINT16                    MinValue;
    UINT16                    Resolution;
    UINT16                    Tolerance;
    UINT16                    Accuracy;
    UINT32                    OEMDefine;
    UINT16                    NorminalValue;
} SMBIOS_TEMPERATURE_PROBE_INFO;                              // TYPE 28
#endif

#if ELECTRICAL_PROBE_INFO
/**
    Electrical Current Probe (Type 29)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Description;
    UINT8                     LocationAndStatus;
    UINT16                    MaxValue;
    UINT16                    MinValue;
    UINT16                    Resolution;
    UINT16                    Tolerance;
    UINT16                    Accuracy;
    UINT32                    OEMDefine;
    UINT16                    NorminalValue;
} SMBIOS_ELECT_CURRENT_PROBE_INFO;                            // TYPE 29
#endif

#if OUT_OF_BAND_REMOTE_ACCESS_INFO
/**
    Out-of-Band Remote Access (Type 30)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     ManufacturerName;
    UINT8                     Connections;
} SMBIOS_OUT_OF_BAND_REMOTE_ACCESS_INFO;                      // TYPE 30
#endif

#if MKF_BIS_INFO
/**
    Boot Integrity Services (BIS) Entry Point (Type 31)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Checksum;
    UINT8                     Reserved_1;
    UINT16                    Reserved_2;
    UINT32                    BISEntrySt;
    UINT32                    BISEntryTt;
    UINT64                    Reserved_3;
    UINT32                    Reserved_4;
} SMBIOS_BIS_ENTRYPOINT_INFO;                                 // TYPE 31
#endif

/**
    System Boot Information (Type 32)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Reserved[6];
    UINT8                     BootStatus[10];
} SMBIOS_SYSTEM_BOOT_INFO;                                    // TYPE 32

#if SIXTY_FOURBIT_MEMORY_ERROR_INFO
/**
    64-bit Memory Error Information (Type 33)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     ErrorType;
    UINT8                     ErrorGranularity;
    UINT8                     ErrorOperation;
    UINT32                    VendorSyndrome;
    UINT64                    MemoryArrayErrAddr;
    UINT64                    DeviceErrAddr;
    UINT32                    ErrorResolution;
} SMBIOS_SIXTYFOUR_MEM_ERROR_INFO;                            // TYPE 33
#endif

#if MANAGEMENT_DEVICE_INFO
/**
    Management Device (Type 34)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     Description;
    UINT8                     Type;
    UINT32                    Address;
    UINT8                     AddressType;
} SMBIOS_MANAGEMENT_DEV_INFO;                                 // TYPE 34
#endif

#if MEMORY_CHANNEL_INFO
/**
    Type 37 Memory Device
**/
typedef struct {
    UINT8                     MemDevLoad;
    UINT16                    MemDevHandle;
} TYPE37_MEMORY_DEVICE;

/**
    Memory Channel (Type 37)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     ChannelType;
    UINT8                     MaxChannelLoad;
    UINT8                     MemDeviceCount;
    TYPE37_MEMORY_DEVICE      MemoryDevice[NUMBER_OF_MEMORY_CHANNELS];
} SMBIOS_MEMORY_CHANNEL_INFO;                                 // TYPE 37
#endif

#if IPMI_DEVICE_INFO
/**
    IPMI Device Information (Type 38)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     InterfaceType;
    UINT8                     IPMISpecRevision;
    UINT8                     I2CSlaveAddr;
    UINT8                     NVStorageDevAddr;
    UINT64                    BaseAddress;
} SMBIOS_IPMI_DEV_INFO;                                       // TYPE 38
#endif

#if SYSTEM_POWER_SUPPLY_INFO
/**
    System Power Supply (Type 39)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     PwrUnitGroup;
    UINT8                     Location;
    UINT8                     DeviceName;
    UINT8                     Manufacturer;
    UINT8                     SerialNumber;
    UINT8                     AssetTagNumber;
    UINT8                     ModelPartNumber;
    UINT8                     RevisionLevel;
    UINT16                    MaxPwrCapacity;
    UINT16                    PwrSupplyChar;
    UINT16                    InputVoltProbeHandle;
    UINT16                    CoolingDevHandle;
    UINT16                    InputCurrentProbeHandle;
} SMBIOS_SYSTEM_PWR_SUPPY_INFO;                               // TYPE 39
#endif

#if ADDITIONAL_INFO
/**
    System Power Supply Entry
**/
typedef struct {
    UINT8                     EntryLength;
    UINT16                    RefHandle;
    UINT8                     RefOffset;
    UINT8                     StringNum;
    union {
        UINT8                 Value8;
        UINT16                Value16;
        UINT32                Value32;
    };
} ADDITIONAL_INFO_ENTRY;

/**
    System Power Supply (Type 40)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     NumAdditionalInfoEntries;
    ADDITIONAL_INFO_ENTRY     AdditionalEntries[ADDITIONAL_INFO_COUNT];
} SMBIOS_ADDITIONAL_INFO;                                     // TYPE 40
#endif

#if ONBOARD_DEVICE_EXTENDED_INFO
/**
    Onboard Devices Extended Information (Type 41)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     RefDesignation;
    UINT8                     DeviceType;
    UINT8                     DeviceTypeInstance;
    UINT16                    SegGroupNumber;
    UINT8                     BusNumber;
    UINT8                     DevFuncNumber;
} SMBIOS_ONBOARD_DEV_EXT_INFO;                                // TYPE 41
#endif

/**
    End of Table (Type 127)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
} SMBIOS_END_OF_TABLE;                                        // TYPE 127

//**********************************************************************//
// SMBIOS NVRAM DATA
//**********************************************************************//
/**
    DMI Type X data
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[32];
} SMBIOS_NVRAM_TYPE;

/**
    DMI Type 0 data
    UINT32   Flag;
              Flag:Bit0 = Vendor string changed
              Flag:Bit1 = BIOS version string changed
              Flag:Bit2 = BIOS Release Date string changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[3];
} SMBIOS_NVRAM_TYPE0;

/**
    DMI Type 1 data
    UINT32   Flag;
              Flag:Bit0  = Manufacturer string changed
              Flag:Bit1  = Product Name string changed
              Flag:Bit2  = Version string changed
              Flag:Bit3  = Serial Number string changed
              Flag:Bit4  = SKU string changed
              Flag:Bit5  = Family string changed
              Flag:Bit16 = UUID changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[6];
    EFI_GUID                  Uuid;
} SMBIOS_NVRAM_TYPE1;

/**
    DMI Type 2 data
    UINT32   Flag;
              Flag:Bit0 = Manufacturer string changed
              Flag:Bit1 = Product Name string changed
              Flag:Bit2 = Version string changed
              Flag:Bit3 = Serial Number string changed
              Flag:Bit4 = Asset Tag string changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[6];
    UINT16                    Handle;
} SMBIOS_NVRAM_TYPE2;

/**
    DMI Type 3 data
    UINT32   Flag;
              Flag:Bit0  = Manufacturer string changed
              Flag:Bit1  = Version string changed
              Flag:Bit2  = Serial Number string changed
              Flag:Bit3  = Asset Tag string changed
              Flag:Bit4  = SKU Number string changed
              Flag:Bit16 = Chassis Type changed
              Flag:Bit17 = OEM-defined changed
              Flag:Bit18 = Number of Power Cords changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[5];
    UINT8                     Type;
    UINT32                    OemDefined;
    UINT16                    Handle;
    UINT8                     NumberOfPowerCord;
} SMBIOS_NVRAM_TYPE3;

/**
    DMI Type 4 data
    UINT32   Flag;
              Flag:Bit0 = Serial Number string changed
              Flag:Bit1 = Asset Tag string changed
              Flag:Bit2 = Part Number string changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[6];
} SMBIOS_NVRAM_TYPE4;

/**
    DMI Type 11 data
    UINT32   Flag;
              Flag:Bit(x) = String #(x) changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[32];
} SMBIOS_NVRAM_TYPE11;

/**
    DMI Type 12 data
    UINT32   Flag;
              Flag:Bit(x) = String #(x) changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[32];
} SMBIOS_NVRAM_TYPE12;

/**
    DMI Type 22 data
    UINT8    Flag;
              Flag:Bit0  = Location string changed
              Flag:Bit1  = Manufacturer string changed
              Flag:Bit2  = Manufacturer Date changed
              Flag:Bit3  = Serial Number string changed
              Flag:Bit4  = Device Name string changed
              Flag:Bit5  = SBDS Version Number string changed
              Flag:Bit6  = SBDS Device Chemistry string changed
              Flag:Bit16 = Device Chemistry changed
              Flag:Bit17 = Design Capacity changed
              Flag:Bit18 = Design Voltage changed
              Flag:Bit19 = Maximum Error in Battery Data changed
              Flag:Bit20 = SBDS Serial Number changed
              Flag:Bit21 = SBDS Manufacturer Date changed
              Flag:Bit22 = Design Capacity Multiplier changed
              Flag:Bit23 = OEM-specific changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[7];
    UINT8                     DeviceChemistry;
    UINT16                    DesignCapacity;
    UINT16                    DesignVoltage;
    UINT8                     MaxErrorInBatteryData;
    UINT16                    SbdsSerialNumber;
    UINT16                    SbdsManufacturerDate;
    UINT8                     DesignCapacityMultiplier;
    UINT32                    OemSpecific;
    UINT16                    Handle;
} SMBIOS_NVRAM_TYPE22;

/**
    DMI Type 39 data
    UINT8    Flag;
              Flag:Bit0  = Location string changed
              Flag:Bit1  = Device Name string changed
              Flag:Bit2  = Manufacturer string changed
              Flag:Bit3  = Serial Number string changed
              Flag:Bit4  = Asset Tag string changed
              Flag:Bit5  = Model Part Number string changed
              Flag:Bit6  = Revision Level string changed
              Flag:Bit16 = Power Unit Group changed
              Flag:Bit17 = Max Power Capacity changed
              Flag:Bit18 = Power Supply Characteristics changed
              Flag:Bit19 = Input Voltage Probe Handle changed
              Flag:Bit20 = Cooling Device Handle changed
              Flag:Bit21 = Input Current Probe Handle changed
**/
typedef struct {
    UINT32                    Flag;
    CHAR8                     *StringSet[7];
    UINT8                     PwrUnitGroup;
    UINT16                    MaxPwrCapacity;
    UINT16                    PwrSupplyChar;
    UINT16                    InputVoltageProbeHandle;
    UINT16                    CoolingDevHandle;
    UINT16                    InputCurrentProbeHandle;
    UINT16                    Handle;
} SMBIOS_NVRAM_TYPE39;

/**
    Structure and operational info
    UINT8    Flag;
              Flag:Bit0 = Write Once
              Flag:Bit1 = Delete Structure
              Flag:Bit2 = Add structure
**/
typedef struct {
    UINT8                   Type;
    UINT16                  Handle;
    UINT8                   Offset;
    UINT8                   Flags;      // Bit0 = Write Once
                                        // Bit1 = Delete Structure
                                        // Bit2 = Add structure
} DMI_VAR;

/**
    String table with current string value
**/
typedef struct {
    UINT8                   Offset;
    UINT8                   SpecStrNum;
    UINT8                   CurStrValue;
} STRING_TABLE;

/**
    Smbios Operation Enumeration
**/
typedef enum {
    ADD_STRUCTURE,
    DELETE_STRUCTURE,
    ADD_STRUCTURE_BY_INDEX,
} SMBIOS_OPERATION;

typedef struct {
    UINT8                   DestStrucType;
    UINT8                   DestInstance;
    UINT8					DestOffset;
    UINT8                   SrcStrucType;
    UINT8                   SrcInstance;
} HANDLE_FIXUP_TABLE;

#pragma pack()

//**********************************************************************//
// AMI PI Smbios Protocol
//**********************************************************************//

#pragma pack(1)

/**
    ProducerHandle/SmbiosHandle association
**/
typedef struct {
    EFI_SMBIOS_HANDLE       SmbiosHandle;
    EFI_HANDLE              ProducerHandle;
} EFI_PRODUCER_HANDLE;

#pragma pack()

//**********************************************************************//
// AMI PI SMBIOS Function Declarations
//**********************************************************************//

EFI_STATUS
SmbiosPiAddStructure (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN EFI_HANDLE                   ProducerHandle OPTIONAL,
    IN OUT EFI_SMBIOS_HANDLE        *SmbiosHandle,
    IN EFI_SMBIOS_TABLE_HEADER      *Record
);

EFI_STATUS
SmbiosPiUpdateString (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN EFI_SMBIOS_HANDLE            *SmbiosHandle,
    IN UINTN                        *StringNumber,
    IN CHAR8                        *String
);

EFI_STATUS
SmbiosPiRemoveStructure (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN EFI_SMBIOS_HANDLE            SmbiosHandle
);

EFI_STATUS
SmbiosPiGetNextStructure (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN OUT EFI_SMBIOS_HANDLE        *SmbiosHandle,
    IN EFI_SMBIOS_TYPE              *Type OPTIONAL,
    OUT EFI_SMBIOS_TABLE_HEADER     **Record,
    OUT EFI_HANDLE                  *ProducerHandle OPTIONAL
);

//**********************************************************************//
// AMI Smbios Protocol
//**********************************************************************//

typedef VOID* (EFIAPI *EFI_SMBIOS_GET_TABLE_ENTRY) (
);

typedef VOID* (EFIAPI *EFI_SMBIOS_GET_SCRATCH_BUFFER) (
);

typedef UINT16 (EFIAPI *EFI_SMBIOS_GET_BUFFER_MAX_SIZE) (
);

typedef UINT16 (EFIAPI *EFI_SMBIOS_GET_FREE_HANDLE) (
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_ADD_STRUCTURE) (
    IN UINT8        *Buffer,
    IN UINT16       Size
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_ADD_STRUC_HANDLE) (
    IN UINT16       Handle,
    IN UINT8        *Buffer,
    IN UINT16       Size
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_DELETE_STRUCTURE) (
    IN UINT16       Handle
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_READ_STRUCTURE) (
    IN      UINT16  Handle,
    IN OUT  UINT8   **BufferPtr,
    IN OUT  UINT16  *BufferSize
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_READ_STRUC_TYPE) (
    IN UINT8        Type,
    IN UINT8        Instance,
    IN UINT8        **BufferPtr,
    IN UINT16       *BufferSize
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_WRITE_STRUCTURE) (
    IN UINT16       Handle,
    IN UINT8        *BufferPtr,
    IN UINT16       BufferSize
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_ADD_STRUC_INDEX) (
    IN UINT16       Handle,
    IN UINT8        *Buffer,
    IN UINT16       Size,
    IN UINT16       Index
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_UPDATE_HEADER) (
);

typedef VOID* (EFIAPI *EFI_SMBIOS_GET_VER_TABLE_ENTRY) (
    IN UINT8                  SmbiosMajorVersion
);

typedef struct {
    EFI_SMBIOS_GET_TABLE_ENTRY      SmbiosGetTableEntryPoint;   // Get SMBIOS V2 Table Entry Point
    EFI_SMBIOS_GET_SCRATCH_BUFFER   SmbiosGetScratchBufferPtr;  // Scratch Buffer of maximum table size
    EFI_SMBIOS_GET_BUFFER_MAX_SIZE  SmbiosGetBufferMaxSize;     // Maximum SMBIOS Table Size
    EFI_SMBIOS_GET_FREE_HANDLE      SmbiosGetFreeHandle;        // Get available free handle
    EFI_SMBIOS_ADD_STRUCTURE        SmbiosAddStructure;         // Add structure
    EFI_SMBIOS_ADD_STRUC_HANDLE     SmbiosAddStrucByHandle;     // Add structure (by handle)
    EFI_SMBIOS_DELETE_STRUCTURE     SmbiosDeleteStructure;      // Delete structure (by handle)
    EFI_SMBIOS_READ_STRUCTURE       SmbiosReadStructure;        // Read structure. Caller is responsible
                                                                // for deallocating the memory
    EFI_SMBIOS_READ_STRUC_TYPE      SmbiosReadStrucByType;      // Read structure by type. Caller is
                                                                // responsible for deallocating the memory
    EFI_SMBIOS_WRITE_STRUCTURE      SmbiosWriteStructure;       // Write structure
    EFI_SMBIOS_UPDATE_HEADER        SmbiosUpdateHeader;         // Update SMBIOS Table Header
    EFI_SMBIOS_ADD_STRUC_INDEX      SmbiosAddStrucByIndex;      // Add structure
    EFI_SMBIOS_GET_VER_TABLE_ENTRY  SmbiosGetVerTableEntryPoint;// Get input version of SMBIOS Table Entry Point
} AMI_SMBIOS_PROTOCOL;

//**********************************************************************//
// AMI SMBIOS Functions
//**********************************************************************//

VOID*
GetSmbiosTableEntryPoint(
);

VOID*
GetScratchBufferPtr(
);

UINT16
GetBufferMaxSize(
);

UINT16
GetFreeHandle(
);

EFI_STATUS
AddStructure(
    IN UINT8                  *Buffer,
    IN UINT16                 Size
);

EFI_STATUS
AddStructureByHandle(
    IN UINT16                 Handle,
    IN UINT8                  *Buffer,
    IN UINT16                 Size
);

EFI_STATUS
AddStructureByIndex(
    IN UINT16                 Handle,
    IN UINT8                  *Buffer,
    IN UINT16                 Size,
    IN UINT16                 Index
);

EFI_STATUS
DeleteStructureByHandle(
    IN UINT16                 Handle
);

EFI_STATUS
ReadStructureByHandle(
    IN      UINT16            Handle,
    IN OUT  UINT8             **BufferPtr,
    IN OUT  UINT16            *BufferSize
);

EFI_STATUS
ReadStructureByType(
    IN      UINT8             Type,
    IN      UINT8             Instance,
    IN OUT  UINT8             **BufferPtr,
    IN OUT  UINT16            *BufferSize
);

EFI_STATUS
WriteStructureByHandle(
    IN UINT16                 Handle,
    IN UINT8                  *BufferPtr,
    IN UINT16                 BufferSize
);

VOID
ClearStringNumber (
    IN STRING_TABLE     *StringTablePtr,
    IN UINT8            Index
);

EFI_STATUS
ReplaceString(
    IN UINT8    *DestStructPtr,
    IN UINT8    StringNum,
    IN UINT8    *StringData
);

EFI_STATUS
UpdateSmbiosTableHeader(
);

UINT16
GetStructureLength(
    IN UINT8    *BufferStart
);

BOOLEAN
FindStructureHandle(
    IN OUT UINT8    **Buffer,
    IN     UINT16   Handle
);

#endif                              // _Smbios_DRIVER_H

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
