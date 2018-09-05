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

/** @file Spmi.h
    Definitions related to SPMI support

**/

#ifndef _SPMI_H_
#define _SPMI_H_

//---------------------------------------------------------------------------

#include "IpmiBmc.h"
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <AcpiOemElinks.h>
#include <Include/AcpiRes.h>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constants Definitions
//---------------------------------------------------------------------------

// APTIOV_SERVER_OVERRIDE_START : SPMI RegBitWidth value as 8.
//
// SPMI Version (as defined in ACPI 3.0 Specification.)
//
//#define EFI_ACPI_3_0_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_VERSION  0x02
// APTIOV_SERVER_OVERRIDE_END : SPMI RegBitWidth value as 8.

//
// "SPMI" Service processor Management Interface Table
//
#define EFI_ACPI_3_0_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_SIGNATURE  0x494d5053
//
// SPMI REV 5
//
#define EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_REVISION 0x05

#define EFI_ACPI_OEM_SPMI_REVISION 0x00000000

//
//Interface Types
//
# define KEYBOARD_CONTROLLER_STYLE                   0x01
# define SERVER_MANAGEMENT_INTERFACE_CHIP            0x02
# define BLOCK_TRANSFER                              0x03
# define SMBUS_SYSTEM_INTERFACE                      0x04

//
//Register bit width
//
#define REG_BIT_WIDTH_BYTE        8
#define REG_BIT_WIDTH_WORD        16
#define REG_BIT_WIDTH_DWORD       32
#define REG_BIT_WIDTH_QWORD       64

//
//Register bit offset
//
#define REG_BIT_OFFSET0           0

#define SPMI_ACCESS_BYTE        0x1
#define SPMI_ACCESS_WORD        0x2
#define SPMI_ACCESS_DWORD       0x3
#define SPMI_ACCESS_QWORD       0x4

//
//UID
//
#define UID_BYTE1    0
#define UID_BYTE2    0
#define UID_BYTE3    0
#define UID_BYTE4    0
#define SPMI_GPE     0
#define SPMI_INTERRUPT_TYPE 0

#define GLOBAL_SYSTEM_INTERRUPT    0
#define PCI_DEVICE_FLAG            0

#pragma pack (1)

typedef struct _GAS_30 {
  UINT8            AddrSpcID;          ///< The address space where the data structure or register exists.
                                       ///< Defined values are above
  UINT8            RegBitWidth;        ///< The size in bits of the given register.
                                       ///< When addressing a data structure, this field must be zero.
  UINT8            RegBitOffs;         ///< The bit offset of the given register at the given address.
                                       ///< When addressing a data structure, this field must be zero.
  UINT8            AccessSize;         ///< Data Access size  
  UINT64           Address;            ///< The 64-bit address of the data structure or register in
                                       ///< the given address space (relative to the processor).
} GAS_30;

//
// SERVICE PROCESSOR MANAGEMENT INTERFACE Structure Definition
//
typedef struct {
  UINT8   InterfaceType;
  UINT8   Reserved;
  UINT16  SpecificationRevision;
  UINT8   IntType;
  UINT8   Gpe;
  UINT8   Reserved1;
  UINT8   PciDeviceFlag;
  UINT32  GlobalSystemInt;
  GAS_30  BaseAddress;
  UINT8   PciSegOrUID1;
  UINT8   PciBusOrUID2;
  UINT8   PciDevOrUID3;
  UINT8   PciFunOrUID4;
  UINT8   Reserved2;
} EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_STRUCTURE;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER              AcpiHeader;
  EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_STRUCTURE           SpmiData;
} EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE;

#pragma pack ()

//---------------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------------

EFI_STATUS
SpmiSupport (
  VOID
  );

//
// Ipmi OEM SPMI update hook.
//
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
typedef EFI_STATUS (IPMI_OEM_SPMI_UPDATE_HOOK)(
  IN CONST EFI_BOOT_SERVICES            *BootServices,
  IN       VOID                         *SpmiData
);
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
