//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AcpiPlatformLibLocal.h

--*/

#ifndef _ACPI_PLATFORM__LIB_LOCAL_H_
#define _ACPI_PLATFORM__LIB_LOCAL_H_

//
// Statements that include other header files
//
#include <Library/PcdLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AcpiPlatformLib.h>
#include <Library/CpuConfigLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SetupLib.h>

#include <Guid/PlatformInfo.h>
#include <Guid/MemoryMapData.h>
#include <Guid/GlobalVariable.h>

#include <Protocol/CpuCsrAccess.h>
#include <Protocol/CrystalRidge.h>
#include <Protocol/JedecNvDimm.h>
#include <Protocol/NgnAcpiSmmProtocol.h>
#include <Protocol/Smbios.h>

#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>

#include <Platform.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/IioUds.h>
#include <Protocol/RasfProtocol.h>
#include <Protocol/LpcPolicy.h>
#include <Protocol/SystemBoard.h>
#include <Protocol/CpuIo.h>
#include <Protocol/SerialIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LpcPolicy.h>
#include <Protocol/SvSmmProtocol.h>
#include <Cpu/CpuRegs.h>
#include <Acpi/Bdat.h>
#include <Acpi/Hpet.h>
#include <Acpi/Mcfg.h>
#include <Acpi/Msct.h>
#include <Acpi/Pcct.h>
#include <Acpi/Slit.h>
#include <Acpi/Srat.h>
#include <Acpi/Migt.h>
#include <Acpi/Svos.h>
#include <Acpi/Nfit.h>
#include <Acpi/Pcat.h>
#include <Acpi/Pmtt.h>
#include <Acpi/Wsmt.h>
#include <Acpi/amlresrc.h> // came from https://acpica.org/downloads/source_code.php acpica-win-20130214.zip/source/include
#include <IndustryStandard/AcpiAml.h>

#include <Guid/SocketMpLinkVariable.h>
#include <Guid/SocketIioVariable.h>
#include <Guid/SocketPowermanagementVariable.h>
#include <Guid/SocketCommonRcVariable.h>

#include "Register/PchRegsUsb.h"
#include "RcRegs.h"

#include <Library/FpgaConfigurationLib.h>
#include <Guid/FpgaSocketVariable.h>
#include <Fpga.h>

#define OEM1_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '1')
#define OEM2_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '2')
#define OEM3_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '3')
#define OEM4_SSDT_TABLE_SIGNATURE  SIGNATURE_32('O', 'E', 'M', '4')

#define AML_NAME_OP           0x08
#define AML_NAME_PREFIX_SIZE  0x06
#define AML_NAME_DWORD_SIZE   0x0C

// mIioUds->IioUdsPtr->SystemStatus.CpuPCPSInfo DEFINITIONS
#define B_PCPS_SPD_ENABLE                  (1 << 16) // Bit 16
#define B_PCPS_HT_ENABLE                   (1 << 17) // Bit 17

#define PCIE_BUS_0         0x00
#define PCIE_BUS_1         0x01
#define PCIE_BUS_2         0x02
#define PCIE_BUS_3         0x03
#define PCIE_BUS_4         0x04
#define PCIE_BUS_5         0x05

#define PORTS_PER_SOCKET   0x0F
#define PCIE_PORT_ALL_FUNC 0x00

#define PCIE_PORT_0_DEV    0x00

#define PCIE_PORT_1A_DEV   0x00
#define PCIE_PORT_1B_DEV   0x01
#define PCIE_PORT_1C_DEV   0x02
#define PCIE_PORT_1D_DEV   0x03

#define PCIE_PORT_2A_DEV   0x00
#define PCIE_PORT_2B_DEV   0x01
#define PCIE_PORT_2C_DEV   0x02
#define PCIE_PORT_2D_DEV   0x03

#define PCIE_PORT_3A_DEV   0x00
#define PCIE_PORT_3B_DEV   0x01
#define PCIE_PORT_3C_DEV   0x02
#define PCIE_PORT_3D_DEV   0x03

#define PCIE_PORT_4_DEV   0x00
#define PCIE_PORT_5_DEV   0x00

#define R_ICH_RCRB_HPTC   0x3404         // High Performance Timer Configuration

#ifdef BDAT_SUPPORT
#define BDAT_PRIMARY_VER    0x0004
#define BDAT_SECONDARY_VER  0x0000
#define SCHEMA_LIST_LENGTH  3
#endif

#pragma pack(1)

typedef struct _PCIE_PORT_INFO {
    UINT8       Device;
    UINT8       Stack;
} PCIE_PORT_INFO;

typedef struct {
  UINT32   AcpiProcessorId;
  UINT32   ApicId;
  UINT32   Flags;
  UINT32   SwProcApicId;
  UINT32   SocketNum;
} EFI_CPU_ID_ORDER_MAP;

typedef struct {
  UINT32             Index;
  UINT32             SortId;
} EFI_CPU_LOCATION_LIST;

typedef struct {
  UINT8   AcpiProcessorId;
  UINT8   ApicId;
  UINT16  Flags;
} EFI_CPU_ID_MAP;

typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   Size;
  UINT32  Value;      
} EFI_ACPI_NAMEPACK_DWORD;

typedef struct {
  UINT8   StartByte;
  UINT32  NameStr;
  UINT8   OpCode;
  UINT16  Size;                     // Hardcode to 16bit width because the table we use is fixed size
  UINT8   NumEntries;
} EFI_ACPI_NAME_COMMAND;

typedef struct {
  UINT8   PackageOp;
  UINT8   PkgLeadByte;
  UINT8   NumEntries;
  UINT8   DwordPrefix0;
  UINT32  CoreFreq;
  UINT8   DwordPrefix1;
  UINT32  Power;
  UINT8   DwordPrefix2;
  UINT32  TransLatency;
  UINT8   DwordPrefix3;
  UINT32  BMLatency;
  UINT8   DwordPrefix4;
  UINT32  Control;
  UINT8   DwordPrefix5;
  UINT32  Status;
} EFI_PSS_PACKAGE;

typedef struct {
  UINT8 PackageOp;
  UINT8 PkgLeadByte;
  UINT8 NumEntries;
  UINT8 BytePrefix0;
  UINT8 Entries;
  UINT8 BytePrefix1;
  UINT8 Revision;
  UINT8 BytePrefix2;
  UINT8 Domain;
  UINT8 BytePrefix3;
  UINT8 Coordinate;
  UINT8 BytePrefix4;
  UINT8 ProcNumber;
} EFI_PSD_PACKAGE;

typedef struct {
  UINT8     NameOp;           // 08h ;First opcode is a NameOp.
  UINT32    PackageName;      // PSDC/PSDD/PSDE 
  UINT8     Length;           // 12h
  UINT8     DwordPrefix1;     // 0Fh
  UINT8     Revision;         // 01h
  UINT8     PackageOp;        // 12h
  UINT8     PackageLen;       // 0Ch
  UINT8     PackLen;          // 05h
  UINT16    WordValue1;       // 0A05h
  UINT16    WordValue2;       // 0A00h
  UINT8     BytePrefix2;      // 00h
  UINT8     Domain;           // 08h
  UINT8     BytePrefix3;      // 0Ah
  UINT8     CoordType;        // 0FCh/0FDh/0FEh
  UINT8     BytePrefix4;      // 0Ah
  UINT8     NumProcessors;    // 10h
} PSD_PACKAGE_LAYOUT;

struct CpcRegPack {
  UINT8    ResourceOp;           // 11h 
  UINT8    Length;               // 14h
  UINT32   FFixedHW;            
  UINT32   RegisterBitsUsage;        
  UINT32   RegisterBitsShift;       
  UINT32   RegisterOffset;        
  UINT32   RegisterByteSize;        
};

typedef struct {
  UINT8   BytePrefix0;
  UINT8   NumEntries;
  UINT8   BytePrefix1;
  UINT8   Revision;
  struct CpcRegPack HighestPerformance;
  UINT32  NominalPerf;
} EFI_CPC_PACKAGE;

#define ACPI_NAME_COMMAND_FROM_NAME_STR(a)  BASE_CR (a, EFI_ACPI_NAME_COMMAND, NameStr)
#define ACPI_NAME_COMMAND_FROM_NAMEPACK_STR(a)  BASE_CR (a, EFI_ACPI_NAMEPACK_DWORD, NameStr)

//
// Private Driver Data
//
//
// Define Union of IO APIC & Local APIC structure;
//
typedef union {
  EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE AcpiLocalApic;
  EFI_ACPI_4_0_IO_APIC_STRUCTURE              AcpiIoApic;
  EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE AcpiLocalx2Apic;
  struct {
    UINT8 Type;
    UINT8 Length;
  } AcpiApicCommon;
} ACPI_APIC_STRUCTURE_PTR;


#ifdef BDAT_SUPPORT

typedef struct {
  UINT8   BiosDataSignature[8]; // "BDATHEAD"
  UINT32  BiosDataStructSize;   // sizeof BDAT_STRUCTURE + sizeof BDAT_MEMORY_DATA_STRUCTURE + sizeof BDAT_RMT_STRUCTURE
  UINT16  Crc16;                // 16-bit CRC of BDAT_STRUCTURE (calculated with 0 in this field)
  UINT16  Reserved;
  UINT16  PrimaryVersion;       // Primary version
  UINT16  SecondaryVersion;     // Secondary version
  UINT32  OemOffset;            // Optional offset to OEM-defined structure
  UINT32  Reserved1;
  UINT32  Reserved2;
} BDAT_HEADER_STRUCTURE;

typedef struct bdatSchemaList {
  UINT16                       SchemaListLength; //Number of Schemas present
  UINT16                       Reserved;
  UINT16                       Year;
  UINT8                        Month;
  UINT8                        Day;
  UINT8                        Hour;
  UINT8                        Minute;
  UINT8                        Second;
  UINT8                        Reserved1;
  UINT32                       Schemas[SCHEMA_LIST_LENGTH]; //List of Bdat Memory Schemas
} BDAT_SCHEMA_LIST_STRUCTURE;

//BDAT Header Struct which contains information all exisitng BDAT Schemas
typedef struct bdatStruct {
  BDAT_HEADER_STRUCTURE        BdatHeader;
  BDAT_SCHEMA_LIST_STRUCTURE   BdatSchemas;
} BDAT_STRUCTURE;


#endif
#pragma pack()

EFI_STATUS
PatchBdatAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

//Function prototypes
EFI_STATUS
PatchMadtTable(
   IN OUT   EFI_ACPI_COMMON_HEADER          *MadtTable
   );

EFI_STATUS
PatchFadtTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS
PatchDsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchMcfgAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

EFI_STATUS
PatchMsctAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchPlatformCommunicationsChannelTable(
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchSLitTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS
PatchSpcrAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  );

EFI_STATUS
PatchSratTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS
PatchSsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
    ,IN OUT EFI_ACPI_TABLE_VERSION     *Version
  );

EFI_STATUS
PatchCpuPmSsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem1SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem2SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem3SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchOem4SsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchMigtAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  );

EFI_STATUS
PatchSvosAcpiTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  );

EFI_STATUS
UpdateNfitTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS
UpdatePcatTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   );

EFI_STATUS 
PatchPlatformMemoryTopologyTable ( 
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table 
   ); 

//AcpiPlatformTableLib private share
extern EFI_MP_SERVICES_PROTOCOL    *mMpService;
extern BOOLEAN                     mCpuOrderSorted;
extern EFI_CPU_ID_ORDER_MAP        mCpuApicIdOrderTable[];
extern UINTN                       mNumberOfCPUs;
extern UINTN                       mNumberOfEnabledCPUs;
extern AML_OFFSET_TABLE_ENTRY      *mAmlOffsetTablePointer;

#endif
