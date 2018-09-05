/** @file
  PlatformInfo GUID

  These GUIDs point the ACPI tables as defined in the ACPI specifications.
  ACPI 2.0 specification defines the ACPI 2.0 GUID. UEFI 2.0 defines the
  ACPI 2.0 Table GUID and ACPI Table GUID.

  Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Revision Reference:
  GUIDs defined in UEFI 2.0 spec.

**/

#ifndef _PLATFORM_INFO_GUID_H_
#define _PLATFORM_INFO_GUID_H_

extern EFI_GUID gEfiPlatformInfoGuid;
extern CHAR16   EfiPlatformInfoVariable[];

#pragma pack(1)

typedef struct {
  UINT32 ReservedPci;
} EFI_PLATFORM_PCI_DATA;

typedef struct {
  UINT32 ReservedCpu;
} EFI_PLATFORM_CPU_DATA;

typedef struct {
  UINT32  SysIoApicEnable;
  UINT8   SysSioExist;
  UINT8   IsocEn;       // Isoc Enabled
  UINT8   MeSegEn;      // Me Seg Enabled.
  UINT8   DmiVc1;       // 0 - Disable, 1 - Enable
//
// 4926908: SKX not support Vcp
//
  UINT8   DmiVcm;       // 0 - Disable, 1 - Enable
  UINT8   DmiReserved;  // for alignment
} EFI_PLATFORM_SYS_DATA;

typedef struct {
  UINT32  BiosGuardPhysBase;
  UINT32  BiosGuardMemSize;
  UINT32  MemTsegSize;
  UINT32  MemIedSize;
#ifdef LT_FLAG
  UINT32  MemLtMemSize; //Below TSEG, contains (high to low: (Heap, SinitMem, reserved)3M, LtDevMemNonDMA 2M)
  UINT32  MemDprMemSize;//6M DPR contained in LtMem
  UINT32  MemLtMemAddress;
#endif
#if ME_SUPPORT_FLAG
  UINT64  MemMESEGBase;
  UINT64  MemMESEGSize;
#endif //ME_SUPPORT_FLAG
} EFI_PLATFORM_MEM_DATA;

typedef struct {
  UINT8   PchPciePortCfg1;   // PCIE Port Configuration Strap 1
  UINT8   PchPciePortCfg2;   // PCIE Port Configuration Strap 2
  UINT8   PchPciePortCfg3;   // PCIE Port Configuration Strap 3
  UINT8   PchPciePortCfg4;   // PCIE Port Configuration Strap 4
  UINT8   PchPciePortCfg5;   // PCIE Port Configuration Strap 5
  UINT8   PchPcieSBDE;
  UINT8   LomLanSupported;   // Indicates if PCH LAN on board is supported or not
  UINT8   GbePciePortNum;    // Indicates the PCIe port qhen PCH LAN on board is connnected.
  UINT8   GbeRegionInvalid;
  BOOLEAN GbeEnabled;        // Indicates if the GBE is SS disabled
  UINT8   PchStepping;
} EFI_PLATFORM_PCH_DATA;
//
// This HOB definition must be consistent with what is created in the
// PlatformInfo protocol definition.  This way the information in the
// HOB can be directly copied upon the protocol and only the strings
// will need to be updated.
//
typedef struct _EFI_PLATFORM_INFO_HOB {
  UINT8                       SystemUuid[16];     // 16 bytes
  UINT32                      Signature;          // "$PIT" 0x54495024
  UINT32                      Size;               // Size of the table
  UINT16                      Revision;           // Revision of the table
  UINT16                      Type;               // Platform Type
  UINT32                      CpuType;            // Cpu Type
  UINT8                       CpuStepping;        // Cpu Stepping
  UINT32                      TypeRevisionId;     // Board Revision ID
  UINT8                       BoardId;            // Board ID
  UINT16                      IioSku;
  UINT16                      IioRevision;
  UINT16                      PchSku;
  UINT16                      PchRevision;
  UINT16                      PchType;            // Retrive PCH SKU type installed
  BOOLEAN                     ExtendedInfoValid;  // If TRUE then below fields are Valid
  UINT8                       Checksum;           // Checksum minus SystemUuid is valid in DXE only.
  UINT64                      TypeStringPtr;
  UINT64                      IioStringPtr;
  UINT64                      PchStringPtr;
  EFI_PLATFORM_PCI_DATA       PciData;
  EFI_PLATFORM_CPU_DATA       CpuData;
  EFI_PLATFORM_MEM_DATA       MemData;
  EFI_PLATFORM_SYS_DATA       SysData;
  EFI_PLATFORM_PCH_DATA       PchData;
  UINT8                       IioRiserId;
  UINT8                       PcieRiser1Type;
  UINT8                       PcieRiser2Type;
} EFI_PLATFORM_INFO;

#ifdef PC_HOOK
#define   PLATFORM_SPD_RAW_DATA_GUID { 0xaa7e60ee, 0xa309, 0x40a5, { 0x8f, 0xfa, 0x36, 0x6b, 0xc1, 0x3c, 0xb8, 0xf0 } }

typedef struct _PLATFORM_SPD_RAW_DATA_HOB {
  UINT32                     Size;
  UINT8                      Socket;
  UINT8                      Channel;
  UINT8                      Dimm;
  // UINT8                      Data[]
} PLATFORM_SPD_RAW_DATA_HOB;

#endif

#pragma pack()

#endif
