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

/** @file SmbiosDynamicData.h
    AMI Smbios Dynamic Data Header file.

**/

#ifndef _SMIBios_Dynamic_H
#define _SMIBios_Dynamic_H

#include <Token.h>
#include <AmiHobs.h>
#include <Protocol/SmBus.h>

#if SMBIOS_SUPPORT
#define EFI_SMBIOS_DYNAMIC_DATA_GUID \
    {0xe380280c, 0x4c35, 0x4aa3, { 0xb9, 0x61, 0x7a, 0xe4, 0x89, 0xa2, 0xb9, 0x26 }}

#define AMI_SMBIOS_CPU_INFO_PROTOCOL_GUID \
    {0x3d6cdb0a, 0x5b1f, 0x43a3, 0xa4, 0x3b, 0x44, 0x12, 0x67, 0xf9, 0xda, 0xb5}

extern EFI_GUID gAmiSmbiosDynamicDataGuid;

#define MAX_NUMBER_OF_MEM_MODULE A1_MEMORY_SOCKETS
#if NO_OF_PHYSICAL_MEMORY_ARRAY >= 2
    #if A2_MEMORY_SOCKETS > MAX_NUMBER_OF_MEM_MODULE
        #undef MAX_NUMBER_OF_MEM_MODULE
        #define MAX_NUMBER_OF_MEM_MODULE A2_MEMORY_SOCKETS
    #endif
#endif
#if NO_OF_PHYSICAL_MEMORY_ARRAY >= 3
    #if A3_MEMORY_SOCKETS > MAX_NUMBER_OF_MEM_MODULE
        #undef MAX_NUMBER_OF_MEM_MODULE
        #define MAX_NUMBER_OF_MEM_MODULE A3_MEMORY_SOCKETS
    #endif
#endif
#if NO_OF_PHYSICAL_MEMORY_ARRAY == 4
    #if A4_MEMORY_SOCKETS > MAX_NUMBER_OF_MEM_MODULE
        #undef MAX_NUMBER_OF_MEM_MODULE
        #define MAX_NUMBER_OF_MEM_MODULE A4_MEMORY_SOCKETS
    #endif
#endif

static CHAR16 *SmbiosBaseBoardVar = L"BASEBOARD DATA";
static CHAR16 *SmbiosChassisVar = L"CHASSIS DATA";
static CHAR16 *SmbiosCpuVar = L"CPU DATA";
static CHAR16 *SmbiosSlotsVar = L"SLOTS DATA";
static CHAR16 *SmbiosOnBoardVar = L"ONBOARD DEV DATA";
static CHAR16 *SmbiosMemVar = L"MEMORY DATA";
static CHAR16 *SmbiosBatteryVar = L"BATTERY DATA";
static CHAR16 *SmbiosOnBoardExtVar = L"ONBOARD DEV EXTENDED DATA";

#pragma pack(1)

//**********************************************************************//
// Base Board Structure
//**********************************************************************//
#if BASE_BOARD_INFO
/**
    Base Board Information Structure (Type 2)
**/
typedef struct {                                        // Type 2
    CHAR8          *BoardManufacturer;
    CHAR8          *BoardName;
} BASE_BOARD_DATA;
#endif //BASE_BOARD_INFO

//**********************************************************************//
// Chassis Structure
//**********************************************************************//
#if SYS_CHASSIS_INFO
/**
    System Enclosure or Chassis Information Structure (Type 3)
**/
typedef struct {                                        // Type 3
    UINT8                     Type;
} CHASSIS_DATA;
#endif //SYS_CHASSIS_INFO

//**********************************************************************//
// CPU Structure
//**********************************************************************//

/**
    Processor Information Structure (Type 4)
**/
typedef struct {                                        // Type 4
    UINT8                     Family;                   // 3.3.5.2
    UINT32                    ProcessorID_1;            // 3.3.5.3 - 1st DWORD
    UINT32                    ProcessorID_2;            // 3.3.5.3 - 2nd DWORD
    UINT8                     Voltage;                  // 3.3.5.4
    UINT16                    ExtClockFreq;             // in MHz
    UINT16                    MaxSpeed;                 // in MHz
    UINT16                    CurrentSpeed;             // in MHz
    UINT8                     Status;                   // pg 43
    UINT8                     Upgrade;                  // 3.3.5.5
    UINT8                     Manufacturer[18];
    UINT8                     Version[48];
    UINT16                    CoreCount;                // 3.3.5.6
    UINT16                    CoreEnabled;              // 3.3.5.7
    UINT16                    ThreadCount;              // 3.3.5.8
    UINT16                    Family2;                  // 3.3.5.2
} CPU_DATA;

/**
    Processor Cache Information Structure (Type 7)
**/
typedef struct {                                        // Type 7
    UINT16                    CacheConfig;              // pg 52
    UINT16                    MaxCacheSize;             // pg 52
    UINT16                    InstalledSize;            // pg 52
    UINT16                    SupportSRAM;              // 3.3.8.1
    UINT16                    CurrentSRAM;              // 3.3.8.1
    UINT8                     CacheSpeed;               // in nS
    UINT8                     ErrorCorrectionType;      // 3.3.8.2
    UINT8                     SystemCacheType;          // 3.3.8.3
    UINT8                     Associativity;            // 3.3.8.4
    UINT32                    MaxCacheSize2;
    UINT32                    InstalledCacheSize2;
} CACHE_DATA;

/**
    Single Processor Information Structure
**/
typedef struct {                                        // One for each CPU
    CPU_DATA                  CpuData;
    CACHE_DATA                L1Cache;
    CACHE_DATA                L2Cache;
    CACHE_DATA                L3Cache;
} SINGLE_CPU_DATA;

/**
    Processor Dynamic Data structure
**/
typedef struct {                                        // CPU data structure
    UINT8                     NumberCPU;
    SINGLE_CPU_DATA           CpuData[NO_OF_PROCESSOR_SOCKETS];
} CPU_DYNAMIC_DATA;

typedef CPU_DYNAMIC_DATA SMBIOS_CPU_INFO_PROTOCOL;

//**********************************************************************//
// Slot Structure
//**********************************************************************//

/**
    System Slot Dynamic Data structure
**/
typedef struct {                                        // Type 9
    UINT8                     CurrentUsage[NUMBER_OF_SYSTEM_SLOTS];             // 3.3.10.3
    UINT8                     BusNumber[NUMBER_OF_SYSTEM_SLOTS];
} SYSTEM_SLOT_DYNAMIC_DATA;

//**********************************************************************//
// On-Board Device Structure
//**********************************************************************//

#if ONBOARD_DEVICE_INFO
/**
    On-board device Dynamic Data structure
**/
typedef struct {                                        // Type 10
    UINT8                     OnBoardDev[NUMBER_OF_ONBOARD_DEVICES];            // 3.3.11.1
} ONBOARD_DEV_DYNAMIC_DATA;
#endif

//**********************************************************************//
// Memory Structure
//**********************************************************************//

/**
    Memory Error Dynamic Data structure
**/
typedef struct {                                        // Type 18
    UINT8                     ErrorType;                // 3.3.19.1
    UINT8                     ErrorGranularity;         // 3.3.19.2
    UINT8                     ErrorOperation;           // 3.3.19.3
    UINT32                    MemArrayErrorAddress;     // 32-bit physical address (8000 0000h if unknown)
    UINT32                    DeviceErrorAddress;       // 32-bit physical address (8000 0000h if unknown)
    UINT32                    ErrorResolution;          // in bytes
} MEM_ERROR;

/**
    Memory Array Map Dynamic Data structure
**/
typedef struct {                                        // Type 19
    UINT32                    StartingAddress;          // in KB
    UINT32                    EndingAddress;            // in KB
    UINT8                     PartitionWidth;           //
    UINT64                    ExtendedStartAddr;        // in bytes
    UINT64                    ExtendedEndAddr;          // in bytes
} MEM_ARRAY_MAP_ADDR;

#if MEMORY_DEVICE_INFO
/**
    Memory Device Map Dynamic Data structure
**/
typedef struct {                                        // Type 20
    UINT32                    StartingAddress;          // in KB
    UINT32                    EndingAddress;            // in KB
    UINT8                     PartitionRowPosition;     //
    UINT8                     InterleavePosition;       //
    UINT8                     InterleaveDataDepth;      //
    UINT64                    ExtendedStartAddr;        // in bytes
    UINT64                    ExtendedEndAddr;          // in bytes
} MEM_DEV_MAP_ADDR;
#endif

/**
    Memory Device Dynamic Data structure
**/
typedef struct {                                        // Type 17
    UINT16                    MemErrInfoHandle;         // Type 18
    UINT16                    TotalWidth;               // in bits (FFFFh if unknown)
    UINT16                    Size;                     // FFFFh if unknown, 7FFFh if size is >= 32GB - 1MB
    UINT8                     DeviceSet;                //
    UINT8                     MemoryType;               //
    UINT16                    Speed;                    // in MHz
    UINT32                    ExtendedSize;             // in megabytes when Size field = 7FFFh, 0 otherwise
    UINT16                    ConfMemClkSpeed;          // in MHz, 0 if unknown
    UINT16                    TypeDetail;               //
    UINT16                    MinimumVoltage;           // Minimum operating voltage in millivolts, 0 if unknown
    UINT16                    MaximumVoltage;           // Maximum operating voltage in millivolts, 0 if unknown
    UINT16                    ConfiguredVoltage;        // Configured voltage in millivolts, 0 if unknown
} AMI_MEMORY_DEVICE;

/**
    Memory Device Group Dynamic Data structure
**/
typedef struct {
    AMI_MEMORY_DEVICE         Type17;                   // Type 17
    MEM_ERROR                 Type18;                   // Type 18
#if MEMORY_DEVICE_INFO
    MEM_DEV_MAP_ADDR          Type20;                   // Type 20
#endif
} MEMORY_DEVICE_GROUP;

/**
    Physical Memory Array Dynamic Data structure
**/
typedef struct {                                        // Type 16
    UINT32                    MaxCapacity;              // in KB (8000 0000h if unknown)
    UINT16                    MemErrInfoHandle;         // Type 18
    MEM_ERROR                 ArrayMemoryError;         // Type 18
    MEM_ARRAY_MAP_ADDR        MemArrayMapAddr;          // Type 19
    MEMORY_DEVICE_GROUP       MemoryDeviceData[MAX_NUMBER_OF_MEM_MODULE];           // Type 17 group
    EFI_SMBUS_DEVICE_ADDRESS  SpdSmBusAddr[MAX_NUMBER_OF_MEM_MODULE];
    UINT64                    ExtMaxCapacity;           // in Bytes when MaxCapacity = 8000 0000h, 0 otherwise
} PHYSICAL_MEM_ARRAY;

/**
    System Memory Array Dynamic Data structure
**/
typedef struct {
    PHYSICAL_MEM_ARRAY        PhysicalMemArray[NO_OF_PHYSICAL_MEMORY_ARRAY];
} SYSTEM_MEM_ARRAY_DYNAMIC_DATA;

#ifndef AMI_SMBIOS_MEMORY_INFO_HOB_GUID
  #define AMI_SMBIOS_MEMORY_INFO_HOB_GUID \
    {0x7d6b8734, 0xb754, 0x443f, { 0xb5, 0x88, 0x77, 0x43, 0x84, 0x3a, 0xd3, 0xf1 }}
#endif

/**
    Memory SPD Data structure
**/
typedef struct {
    UINT8 Byte2;                                        // Memory Type
    UINT8 Byte5To8[9 - 5];                              // Attribute, Total Width, Data Width (DDR2 & 3)
    UINT8 Byte11To14[15 - 11];                          // ECC Data Width, Data Width (DDR4)
    UINT8 Byte64To71[72 - 64];                          // Manufacturer (DDR2)
    UINT8 Byte73To90[91 - 73];                          // Part Number (DDR2)
    UINT8 Byte95To98[99 - 95];                          // Serial Number (DDR2)
    UINT8 Byte117To118[119 - 117];                      // Manufacturer (DDR3)
    UINT8 Byte122To125[126 - 122];                      // Serial Number (DDR3)
    UINT8 Byte128To145[146 - 128];                      // Part Number (DDR3)
    UINT8 Byte320To321[322 - 320];                      // Manufacturer (DDR4)
    UINT8 Byte325To328[329 - 325];                      // Serial Number (DDR4)
    UINT8 Byte329To348[349 - 329];                      // Part Number (DDR4)
} MEM_SPD_DATA;

//**********************************************************************//
// SPD data needed from NB driver
//**********************************************************************//
//typedef struct {
//	  EFI_HOB_GUID_TYPE           EfiHobGuidType;
//    MEM_SPD_DATA                SpdDataArray[NO_OF_PHYSICAL_MEMORY_ARRAY][MAX_NUMBER_OF_MEM_MODULE];
//} AMI_SMBIOS_MEMORY_INFO_HOB;

//**********************************************************************//
// Portable Battery Structure
//**********************************************************************//

#if PORTABLE_BATTERY_INFO
/**
    System Portable Battery Dynamic Info structure
**/
typedef struct {                                        // One for each Portable Battery
        UINT8                   DeviceName[65];
        UINT16                  DesignCapacity;
        UINT16                  DesignVoltage;
        UINT8                   SbdsVersion[65];
        UINT16                  SbdsSerialNumber;
        UINT16                  SbdsManufacturedDate;
        UINT8                   DesignCapacityMult;
        BOOLEAN					Valid;
} SMBIOS_PORTABLE_BATTERY_DYNAMIC_INFO;

/**
    Battery Dynamic Data structure
**/
typedef struct {                                        // Type 22
  SMBIOS_PORTABLE_BATTERY_DYNAMIC_INFO  BatteryDynamicData[NO_OF_PORTABLE_BATTERY];
} BATTERY_DYNAMIC_DATA;
#endif

//**********************************************************************//
// On-Board Device Extended Information Structure
//**********************************************************************//

#if ONBOARD_DEVICE_EXTENDED_INFO
/**
    On-board Device Extended Dynamic Data structure
**/
typedef struct {                                        // Type 41
    UINT8                     OnBoardDev[ONBOARD_DEVICE_EXT_COUNT];            // 3.3.42
    UINT8                     BusNumber[ONBOARD_DEVICE_EXT_COUNT];
} ONBOARD_DEV_EXT_DYNAMIC_DATA;
#endif

#pragma pack()

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_CREATE_BASE_BOARD_DATA_STRUCTURE) (
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_CREATE_CHASSIS_DATA_STRUCTURE) (
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_CREATE_CPU_DATA_STRUCTURE) (
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_CREATE_SYSTEM_SLOT_DATA_STRUCTURE) (
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_CREATE_ONBOARD_DEV_DATA_STRUCTURE) (
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_CREATE_BATTERY_DEV_DATA_STRUCTURE) (
);

typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_CREATE_ONBOARD_DEV_EXT_STRUCTURE) (
);
typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_ENABLE_SMBUS_CONTROLLER) (
);
typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_RESTORE_SMBUS_CONTROLLER) (
);
typedef EFI_STATUS (EFIAPI *EFI_SMBIOS_READ_SPD) (
    EFI_SMBUS_DEVICE_ADDRESS    Addr,
    UINTN                       Offset,
    UINT8                       *Data
);
typedef VOID (EFIAPI *EFI_SMBIOS_OEM_UPDATE) (
);

/**
    AMI Smbios Board Protocol
**/
typedef struct {
    UINT8                                           BaseBoardInfoSupport;
    UINT8                                           SysChassisInfoSupport;
    UINT8                                           ProcessorDmiEditSupport;
    UINT8                                           OemStringInfoSupport;
    UINT8                                           SystemConfigOptionInfoSupport;
    UINT8                                           MemoryErrorInfoSupport;
    UINT8                                           OnboardDeviceInfoSupport;
    UINT8                                           PortableBatteryInfoSupport;
    UINT8                                           AdditionalInfoSupport;
    UINT8                                           OnboardDeviceExtInfoSupport;
    UINT8                                           SystemPowerSupplyInfoSupport;
    UINT8                                           NumberOfCPU;
    UINT8                                           NumberOfSystemSlots;
    UINT8                                           NumberOfOnBoardDevices;
    UINT8                                           NumberOfPhysicalMemoryArray;
    UINT8                                           NumberOfBatteries;
    UINT8                                           NumberOfOnboardDevExt;
    EFI_SMBIOS_CREATE_BASE_BOARD_DATA_STRUCTURE     SmbiosCreateBaseBoardData;      // Create Base Board dynamic data
    EFI_SMBIOS_CREATE_CHASSIS_DATA_STRUCTURE        SmbiosCreateChassisData;        // Create Chassis dynamic data
    EFI_SMBIOS_CREATE_CPU_DATA_STRUCTURE            SmbiosCreateCPUData;            // Create CPU dynamic data
    EFI_SMBIOS_CREATE_SYSTEM_SLOT_DATA_STRUCTURE    SmbiosCreateSystemSlotData;     // Create system slot dynamic data
    EFI_SMBIOS_CREATE_ONBOARD_DEV_DATA_STRUCTURE    SmbiosCreateOnBoardDevData;     // Create on-board device dynamic data
    EFI_SMBIOS_CREATE_BATTERY_DEV_DATA_STRUCTURE    SmbiosCreateBatteryDevData;     // Create Portable Battery device dynamic data
    EFI_SMBIOS_CREATE_ONBOARD_DEV_EXT_STRUCTURE     SmbiosCreateOnBoardDevExtInfo;
    EFI_SMBIOS_ENABLE_SMBUS_CONTROLLER              EnableSmbusController;
    EFI_SMBIOS_RESTORE_SMBUS_CONTROLLER             RestoreSmbusController;
    EFI_SMBIOS_READ_SPD                             GetSpdByte;
    EFI_SMBIOS_OEM_UPDATE                           OemUpdate;
    UINT8                                           NumberOfMemorySlots[NO_OF_PHYSICAL_MEMORY_ARRAY];
} EFI_SMBIOS_BOARD_PROTOCOL;

EFI_STATUS CreateBaseBoardDataForSMBios();
EFI_STATUS CreateChassisDataForSMBios();
EFI_STATUS CreateCPUDataForSMBios();
EFI_STATUS CreateSlotDataForSMBios();
EFI_STATUS CreateOnBoardDevDataForSMBios();
EFI_STATUS CreateBatteryDataForSMBios();
EFI_STATUS CreateOnBoardDevExtInfoForSMBios();
EFI_STATUS EnableSmbusController();
EFI_STATUS RestoreSmbusController();
EFI_STATUS GetSpdByte(EFI_SMBUS_DEVICE_ADDRESS SpdAddr, UINTN Offset, UINT8 *Data);
VOID       OemUpdate();

#endif    // if SMBIOS_SUPPORT

#endif

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
