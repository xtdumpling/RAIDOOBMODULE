//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.01
//   Bug Fix:  Increase "AcsCapCount" to "UINT16" and "AcsDevArray" size to "500"
//             to support JBOF case
//   Reason:   
//   Auditor:  Stephen Chen
//   Date:     Aug/21/2017
//
//  Rev. 1.00
//   Bug Fix:  1.Fix NVIDIA GPU run p2pBandwidthLatencyTest issue.
//   Reason:   
//   Auditor:  Jimmy Chiu (Refer Grantley Hartmann)
//   Date:     Oct/21/2016
//
//***************************************************************************
/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**

Copyright (c)  2012 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file AcpiPlatformVTDHooks.c

  ACPI Platform Driver VT-D Hooks

**/
// APTIOV_SERVER_OVERRIDE_RC_START : For filling Reserved Base Address and Limit for RMRR structure of DMAR table
#include <Token.h>
#define __PLATFORM_STATUS_CODES_INTERNAL_H__
// APTIOV_SERVER_OVERRIDE_RC_END
#include "AcpiPlatform.h"
#include "AcpiPlatformHooks.h"
#include <Protocol/PciRootBridgeIo.h>
// APTIOV_SERVER_OVERRIDE_RC_START
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER >= 10
#undef EFI_ACPI_TABLE_VERSION_ALL //undefined here as it is defined from AmiDxeLib.h which is included in AmiUsbController.h
#endif
#include <Protocol\AmiUsbController.h>
#endif
// OEM VTD RMRR support
#if defined OemVtdRmrr_SUPPORT && OemVtdRmrr_SUPPORT == 1
#include <Protocol/OemVtdRmrrProtocol.h>
#endif
// APTIOV_SERVER_OVERRIDE_RC_END

#if SMCPKG_SUPPORT
typedef struct {
  UINT8 Bus;
  UINT8 Dev;
  UINT8 Fun;
  UINT8 SecBus;
  UINT8 DisAcs;
} SMC_BDF_ACS;
#endif

extern EFI_PLATFORM_INFO         *mPlatformInfo;
extern BIOS_ACPI_PARAM           *mAcpiParameter;
extern EFI_IIO_UDS_PROTOCOL   *mIioUds;
extern SYSTEM_CONFIGURATION  mSystemConfiguration;
extern SOCKET_IIO_CONFIGURATION     mSocketIioConfiguration;
extern SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;
BOOLEAN VtdIntRemapEnFalg = FALSE;
extern EFI_GUID mSystemConfigurationGuid;
// APTIOV_SERVER_OVERRIDE_RC_START : Adding RHSA structure in DMAR table
extern EFI_GUID gEfiSocketCommonRcVariableGuid;
// APTIOV_SERVER_OVERRIDE_RC_END : Adding RHSA structure in DMAR table

#define EFI_PCI_CAPABILITY_PTR              0x34
#define EFI_PCIE_CAPABILITY_BASE_OFFSET     0x100
#define EFI_PCIE_CAPABILITY_ID_ACS          0x000D
#define EFI_PCI_CAPABILITY_ID_PCIEXP        0x10
#define EFI_PCI_EXPRESS_CAPABILITY_REGISTER 0x02

#define ACS_CAPABILITY_REGISTER      0x04
#define ACS_SOURCE_VALIDATION        BIT0
#define ACS_P2P_REQUEST_REDIRECT     BIT2
#define ACS_P2P_COMPLETION_REDIRECT  BIT3
#define ACS_UPSTREAM_FORWARDING      BIT4

#define ACS_CONTROL_REGISTER                0x06
#define ACS_SOURCE_VALIDATION_ENABLE        BIT0
#define ACS_P2P_REQUEST_REDIRECT_ENABLE     BIT2
#define ACS_P2P_COMPLETION_REDIRECT_ENABLE  BIT3
#define ACS_UPSTREAM_FORWARDING_ENABLE      BIT4

#define R_VTD_GCMD_REG                            0x18
#define R_VTD_GSTS_REG                            0x1C
#define R_VTD_IQT_REG                             0x88
#define R_VTD_IQA_REG                             0x90
#define R_VTD_IRTA_REG                            0xB8

#define VTD_ISOCH_ENGINE_OFFSET             0x1000

//
// a flag to indicate if we should disable Vt-d for ACS WA
//
BOOLEAN DisableVtd = FALSE;

EFI_DMA_REMAP_PROTOCOL    DmaRemapProt;
#define  VTD_SUPPORT_INSTANCE_FROM_THIS(a) CR(a, EFI_VTD_SUPPORT_INSTANCE, DmaRemapProt, EFI_ACPI_DMAR_DESCRIPTION_TABLE_SIGNATURE)
// #define  DMAR_HOST_ADDRESS_WIDTH   46
#define  EFI_PCI_CAPABILITY_ID_ATS  0x0F

#define SEGMENT0           0x00
#define MEM_BLK_COUNT      0x140
#define INTRREMAP          BIT3

PCI_NODE  mPciPath0_1[]   = {
  {PCI_DEVICE_NUMBER_PCH_HDA, PCI_FUNCTION_NUMBER_PCH_HDA},
  {(UINT8) -1,  (UINT8) -1},
};

//
// IOAPIC1  - PCH IoApic
//


//
// IOAPIC2  - IIO IoApic
//
PCI_NODE  mPciPath1_2[]   = {
  {APIC_DEV_NUM, APIC_FUNC_NUM},
  {(UINT8) -1,  (UINT8) -1},
};

PCI_NODE  mPciPath2_0[]   = {
  {PCIE_PORT_0_DEV, PCIE_PORT_0_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_1[]   = {
  {PCIE_PORT_1A_DEV, PCIE_PORT_1A_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_2[]   = {
  {PCIE_PORT_1B_DEV, PCIE_PORT_1B_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_3[]   = {
  {PCIE_PORT_1C_DEV, PCIE_PORT_1C_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_4[]   = {
  {PCIE_PORT_1D_DEV, PCIE_PORT_1D_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_5[]   = {
  {PCIE_PORT_2A_DEV, PCIE_PORT_2A_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_6[]   = {
  {PCIE_PORT_2B_DEV, PCIE_PORT_2B_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_7[]   = {
  {PCIE_PORT_2C_DEV, PCIE_PORT_2C_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_8[]   = {
  {PCIE_PORT_2D_DEV, PCIE_PORT_2D_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_9[]   = {
  {PCIE_PORT_3A_DEV, PCIE_PORT_3A_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_10[]   = {
  {PCIE_PORT_3B_DEV, PCIE_PORT_3B_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_11[]   = {
  {PCIE_PORT_3C_DEV, PCIE_PORT_3C_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_12[]  = {
  {PCIE_PORT_3D_DEV, PCIE_PORT_3D_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_13[]  = {
  {PCIE_PORT_4A_DEV, PCIE_PORT_4A_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_14[]  = {
  {PCIE_PORT_4B_DEV, PCIE_PORT_4B_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_15[]  = {
  {PCIE_PORT_4C_DEV, PCIE_PORT_4C_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_16[]  = {
  {PCIE_PORT_4D_DEV, PCIE_PORT_4D_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_17[]  = {
  {PCIE_PORT_5A_DEV, PCIE_PORT_5A_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_18[]  = {
  {PCIE_PORT_5B_DEV, PCIE_PORT_5B_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_19[]  = {
  {PCIE_PORT_5C_DEV, PCIE_PORT_5C_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
PCI_NODE  mPciPath2_20[]  = {
  {PCIE_PORT_5D_DEV, PCIE_PORT_5D_FUNC},
  {(UINT8) -1,  (UINT8) -1},
};
DEVICE_SCOPE              mDevScopeDRHD[] = {
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT,    // Device type - HD Audio
    00,                                           // Enumeration ID
    DEFAULT_PCI_BUS_NUMBER_PCH,                           // Start Bus Number
    &mPciPath0_1[0]
  },
};

DEVICE_SCOPE              mDevScopeDRHD_INT[] = {
  {
    00,                                           // Device type
    00,                                           // Enumeration ID - ApicID
    00,                                           // Start Bus Number
    &mPciPath1_2[0]
  },
};

DEVICE_SCOPE              mDevScopeATSR[] = {
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port1
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_0[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port1
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_1[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port2
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_2[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port3
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_3[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port4
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_4[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port5
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_5[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port6
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_6[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port7
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_7[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port8
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_8[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port9
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_9[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port10
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_10[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port11
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_11[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port12
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_12[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port13
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_13[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port14
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_14[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port15
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_15[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port16
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_16[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port17
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_17[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port18
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_18[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port19
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_19[0]
  },
  {
    EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE,      // Pcie Port20
    00,                                           // Enumeration ID
    DMI_BUS_NUM,
    &mPciPath2_20[0]
  }
};

DMAR_DRHD                 mDrhd = {
  DRHD_SIGNATURE,
  00,                                             // Flags
  SEGMENT0,                                       // Segment number
  00,                                             // Base Address
  00,                                             // Number of dev scope structures
  &mDevScopeDRHD[0]
};

DMAR_DRHD                 mDrhdIsoc = {
  DRHD_SIGNATURE,
  00,                                             // Flags
  SEGMENT0,                                       // Segment number
  00,                                             // Base Address
  00,                                             // Number of dev scope structures
  &mDevScopeDRHD[0]
};

DMAR_ATSR                 mAtsr = {
  ATSR_SIGNATURE,
  SEGMENT0,                                       // Segment number
  (UINT8) EFI_ACPI_DMAR_ATSR_FLAGS_ALL_PORTS_CLEAR,
  NUMBER_PORTS_PER_SOCKET-1,
  00,
  &mDevScopeATSR[0]
};

PCI_NODE     PciPath[] = {
  { 00,      00},
  { (UINT8)-1,   (UINT8)-1},
};

UINT32  IoApicEnable[] = {
   PC00_IOAPIC, PC01_IOAPIC, PC02_IOAPIC, PC03_IOAPIC, PC04_IOAPIC, PC05_IOAPIC,
   PC06_IOAPIC, PC07_IOAPIC, PC08_IOAPIC, PC09_IOAPIC, PC10_IOAPIC, PC11_IOAPIC,
   PC12_IOAPIC, PC13_IOAPIC, PC14_IOAPIC, PC15_IOAPIC, PC16_IOAPIC, PC17_IOAPIC,
   PC18_IOAPIC, PC19_IOAPIC, PC20_IOAPIC, PC21_IOAPIC, PC22_IOAPIC, PC23_IOAPIC,
   PC24_IOAPIC, PC25_IOAPIC, PC26_IOAPIC, PC27_IOAPIC, PC28_IOAPIC, PC29_IOAPIC,
   PC30_IOAPIC, PC31_IOAPIC
};
UINT8  IoApicID[] = {
   PC00_IOAPIC_ID, PC01_IOAPIC_ID, PC02_IOAPIC_ID, PC03_IOAPIC_ID, PC04_IOAPIC_ID, PC05_IOAPIC_ID,
   PC06_IOAPIC_ID, PC07_IOAPIC_ID, PC08_IOAPIC_ID, PC09_IOAPIC_ID, PC10_IOAPIC_ID, PC11_IOAPIC_ID,
   PC12_IOAPIC_ID, PC13_IOAPIC_ID, PC14_IOAPIC_ID, PC15_IOAPIC_ID, PC16_IOAPIC_ID, PC17_IOAPIC_ID,
   PC18_IOAPIC_ID, PC19_IOAPIC_ID, PC20_IOAPIC_ID, PC21_IOAPIC_ID, PC22_IOAPIC_ID, PC23_IOAPIC_ID,
   PC24_IOAPIC_ID, PC25_IOAPIC_ID, PC26_IOAPIC_ID, PC27_IOAPIC_ID, PC28_IOAPIC_ID, PC29_IOAPIC_ID,
   PC30_IOAPIC_ID, PC31_IOAPIC_ID
};

PCI_NODE     PciPath7[] = {
  { PCI_DEVICE_NUMBER_PCH_XHCI,      PCI_FUNCTION_NUMBER_PCH_XHCI  },
  { (UINT8)-1,   (UINT8)-1},
};
DEVICE_SCOPE DevScopeRmrr[] = {
  {
    1,                                  // RMRR dev Scope - XHCI
    0,                                  // Enumeration ID
    0,                                  // Start Bus Number
    &PciPath7[0]
  },
};

DMAR_RMRR    mRmrr = {
  RMRR_SIGNATURE,                       // Signature
  SEGMENT0,                             // Segment number
  ' ',                                  // Reserved Memory RegionBase Address
  ' ',                                  // Reserved Memory RegionLimit Address
  ' ',                                  // Number of Dev Scope structures
  &DevScopeRmrr[0]
};

typedef struct {
    UINT8   aBuf[32];
} MEM_BLK;

DMAR_RHSA                 mRhsa;


EFI_STATUS
LocateCapRegBlock(
  IN     EFI_PCI_IO_PROTOCOL  *PciIo,
  IN     UINT8                CapID,
  OUT    UINT8                *PciExpressOffset,
  OUT    UINT8                *NextRegBlock
  );

EFI_STATUS
LocatePciExpressCapRegBlock (
  IN     EFI_PCI_IO_PROTOCOL  *PciIo,
  IN     UINT16               CapID,
  OUT    UINT32               *Offset,
  OUT    UINT32               *NextRegBlock
);

DMAR_DRHD                 mDrhd;

DMAR_ATSR                 mAtsr;
DMAR_RHSA                 mRhsa;

/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
UpdateVtdIntRemapFlag (
  UINT8 VTdSupport,
  UINT8 InterruptRemap
  )
{
  VtdIntRemapEnFalg = FALSE;
  if( (mSocketProcessorCoreConfiguration.ProcessorX2apic)  &&
    (mSocketProcessorCoreConfiguration.ProcessorVirtualWireMode == 01)  &&   // procesoor should boot in virtual wire B mode.
    (VTdSupport) && (InterruptRemap) )
    {
      VtdIntRemapEnFalg = TRUE;
    }
}

/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
EnableVtdIntRemapping(
  )
{
  EFI_STATUS                  Status;
  UINT64                      *xApicAddr;
  UINT64                      *IRTA;
  UINT64                      *Addr;
  UINT64                      Value=0;
  UINT16                      IRTECount;
  UINT16                      Count;
  UINT64                      IRTEValue;
  volatile UINT64             TempDWord[2] ={0,0};
  UINT8                       RemapEng;
  UINT8                       RemapEngCount;
  EFI_CPUID_REGISTER          CpuidRegisters;
  UINT32                      VtdBarAddress;
  EFI_IIO_SYSTEM_PROTOCOL      *mIioSystem;
  UINT8                       Stack;

  IRTEValue   = 00;
  RemapEng = 0;
  RemapEngCount = mIioUds->IioUdsPtr->PlatformData.numofIIO;

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &mIioSystem);
  ASSERT_EFI_ERROR (Status);
  //
  // Xapic tables update
  //
  IRTECount = 16 * 24;    // Total 24 IRTE entries with 128 bits each.
  // Allocate 4K alligned space for IRTE entries  Added extra space of 500 bytes.
  Status = gBS->AllocatePool (EfiACPIReclaimMemory, IRTECount + 0x1500, &xApicAddr);
  ASSERT_EFI_ERROR (Status);

  // Allocate IRT - Allocate zero-initialized, 4KB aligned, 4KB memory for interrupt-remap-table and mark this memory as "ACPI Reclaim Memory"
  xApicAddr = (UINT64 *)((UINT64)xApicAddr & (~0xFFF));
  ZeroMem (xApicAddr, IRTECount +0x1500);

  // 1. Program IRTE - Initialize the interrupt-remap-table as follows: (this table will be shared by all VT-d units)
  for(Count = 0; Count < 24; Count++)  {

    IRTEValue = 00;
    if (Count == 0) {
      IRTEValue = (7 << 05) + 03;    // Preset flag set, Ext int enabled, FPD set
    }

    AsmCpuid (
      EFI_CPUID_CORE_TOPOLOGY,
      &CpuidRegisters.RegEax,
      &CpuidRegisters.RegEbx,
      &CpuidRegisters.RegEcx,
      &CpuidRegisters.RegEdx
      );
    IRTEValue |= (UINT64)(((UINT64)CpuidRegisters.RegEdx) << 32);    // Destination Processor Apic ID

//            if(mSystemConfiguration.ForcePhysicalModeEnable == 00) {
//              IRTEValue |= 0x4;
//            }

    *(volatile UINT64 *)((UINT64)xApicAddr + (Count * 16))= IRTEValue;

    //Perform a CLFLUSH instruction for each cachline in this 4KB memory to ensure that updates to the interrupt-remap-table are visible in memory
    AsmFlushCacheLine ((VOID *)((UINT64)xApicAddr + (Count * 16)));
  }
  // 3. Program the VT-D remap engines
  for (RemapEng=0; RemapEng < RemapEngCount; RemapEng++) {
   for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
     // Check for valid stack
     if (!(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[RemapEng].stackPresentBitmap & (1 << Stack)))
       continue;

     VtdBarAddress = mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[RemapEng][Stack];
     if (VtdBarAddress) {
    // 2. For each VT-d unit in the platform, allocate and initialize the invalidation queue/commands as follows

    // Allocate memory for the queued invalidation.
    Status = gBS->AllocatePool (EfiACPIReclaimMemory, 0x1000 + 0x1000, &Addr);
    ASSERT_EFI_ERROR (Status);
    ZeroMem (Addr, 0x1000 + 0x1000);
    Addr = (UINT64 *)((UINT64)Addr & (~0xFFF));

// Submit two descriptors to the respective VT-d unit's invalidation queue as follows:
    //  ??????????????? Program 1st descriptor in invalidation-queue as Interrupt-Entry-Cache Invalidation Descriptor
    // with G (Granularity) field Clear
    Addr[0] = 0x04;     // Interrupt Entry Cache Invalidate Descriptor
    Addr[1] = 0x00;

    // Program 2nd descriptor in invalidation-queue as Invalidation-Wait-Descriptor as follows:          +Status-Data=1
    // +Status-Address=address of variable tmp[unit +SW=1 +FN=1 +IF=0

    Addr[2] = ((UINT64)1 << 32) + (06 << 04) + 05;      // Invalidation Wait Descriptor

    TempDWord[RemapEng] = 00;
    Addr[3] = (UINTN)&TempDWord[RemapEng];    // Status Address [63:2] bits[127:65]

    // 3. Program the IRTA register to point to the IRT table.
    // For each VT-d unit in the platform, program interrupt-remap-table address and enable extended-interrupt-mode as follows
    IRTA  = (UINT64 *)((UINT64)VtdBarAddress + R_VTD_IRTA_REG);
    // ???? *(volatile UINT64*)IRTA = 04  + 0x800 + (UINT64)xApicAddr ;   // [0:3] size = 2 Power (X+1). Bit11 =1 Xapic mode Bit[12:63] addrerss
    *(volatile UINT64*)IRTA = 07  + 0x800 + (UINT64)xApicAddr ;   // [0:3] size = 2 Power (X+1). Bit11 =1 Xapic mode Bit[12:63] addrerss

    // b. Set SIRTP in the command register.
    Count = 0x1000;
    Value = *(volatile UINT32 *)((UINT64)VtdBarAddress+ R_VTD_GSTS_REG);
    *(volatile UINT32 *)((UINT64)VtdBarAddress+ R_VTD_GCMD_REG) = (UINT32)(Value | BIT24);

    // Wait till the status bit is set indicating the completion of the SIRTP.
    while (Count)  {
      Count--;
      Value = *(volatile UINT32 *)((UINT64)VtdBarAddress + R_VTD_GSTS_REG);
      if (Value & BIT24) break;
    }
    if (Count == 0) CpuDeadLoop ();
    *(volatile UINT64 *)((UINT64)VtdBarAddress+ R_VTD_IQA_REG) = (UINT64)Addr;
    }// End of if (VtdBarAddress)
   } // End of for (Stack = 0; Stack < MAX_IIO_STACK; Stack++)
  }

  for (RemapEng=0; RemapEng < RemapEngCount; RemapEng++) {
   for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {

     // Check for valid stack
     if (!(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[RemapEng].stackPresentBitmap & (1 << Stack)))
       continue;

     VtdBarAddress = mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[RemapEng][Stack];
     if (VtdBarAddress) {

    // 4. For each VT-d unit in the platform, setup invalidation-queue base registers and enable invalidation as follows
    // Initialize a single descriptor which invalidates all the interrupt entries.
    // IQA register write (zeros IQH and IQT)

    // Enable queued invalidation in the command register.
    Count = 0x1000;
    Value = *(volatile UINT32 *)((UINT64)VtdBarAddress+ R_VTD_GSTS_REG);
    *(volatile UINT32 *)((UINT64)VtdBarAddress + R_VTD_GCMD_REG) = (UINT32)(Value | BIT26);

    while (Count)  {
      Count--;
      Value = *(volatile UINT32 *)((UINT64)VtdBarAddress+ R_VTD_GSTS_REG);
      if( Value & BIT26) break;
    }
    if (Count == 0) CpuDeadLoop ();

    // Start invalidations, program the IQT register
    // Write the invalidation queue tail (IQT_REG) register as follows to indicate to hardware two descriptors are submitted:
    // +Bits 63:19 are 0 +Bits 18:4 gets value of 2h +Bits 3:0 are 0

    *(volatile UINT64 *)((UINT64)VtdBarAddress + R_VTD_IQT_REG) = (02 << 04); // Set tail to 02
    } // End of if (VtdAddress)
   } //End of for (Stack = 0; Stack < MAX_IIO_STACK; Stack++)
  }

  for (RemapEng=0; RemapEng < RemapEngCount; RemapEng++) {
   for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {

     // Check for valid stack
     if (!(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[RemapEng].stackPresentBitmap & (1 << Stack)))
       continue;

     VtdBarAddress = mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[RemapEng][Stack];
     if (VtdBarAddress) {

    // 5. For each VT-d unit in the platform, wait for invalidation completion, and enable interrupt remapping as follows
    // Wait till the previously submitted invalidation commands are completed as follows

    // Poll on the variable tmp[unit] in memory, until its value is 1h.
    Count = 0x1000;
    while (Count)  {
      Count--;
      Value = TempDWord[RemapEng];
      if(Value & 01) break;
    }
    if (Count == 0) CpuDeadLoop ();
#if 0  // BIOS should not enable interrupt remapping, it's OS driver doing this.
    //
    // 4986502: Enable/Disable Interrupt remapping must be done by OS,
    //          BIOS just needs to expose/not-expose the interrupt remapping capability
    //

    // 4. Enable interrupt remapping


    // b. Set IRE in the command register
    Count = 0x1000;
    Value = *(volatile UINT32 *)((UINT64)VtdBarAddress + R_VTD_GSTS_REG);
    *(volatile UINT32 *)((UINT64)VtdBarAddress + R_VTD_GCMD_REG) = (UINT32)(Value | BIT25);

    while (Count)  {
      Count--;
      Value = *(volatile UINT32 *)((UINT64)VtdBarAddress + R_VTD_GSTS_REG);
      if( Value & BIT25) break;
    }
    if (Count == 0) CpuDeadLoop ();
#endif
    } // End of if VtdBarAddress
   } //End of for (Stack = 0; Stack < MAX_IIO_STACK; Stack++)
  }

  // 5. Enable external interrupts in the IOAPIC RTE entry 0
  *(volatile UINT32 *)((UINT64)PCH_IOAPIC_ADDRESS)        = 0x10; //
  *(volatile UINT32 *)((UINT64)PCH_IOAPIC_ADDRESS + 0x10) = 0x00; // Set index to the IRTE0

  *(volatile UINT32 *)((UINT64)PCH_IOAPIC_ADDRESS)        = 0x10+1;
  *(volatile UINT32 *)((UINT64)PCH_IOAPIC_ADDRESS + 0x10) = 0x10000;// Set Remap enable bit
}

/**

    Build DRHD entry into ACPI DMAR table for specific stack.
    Include IOxAPIC, PCIExpress ports, and CBDMA if C-STACK.

    @param DmaRemap        - pointer to DMA remapping protocol
    @param mIioSystem      - pointer to IIO system data structure
    @param IioIndex        - IIO index to be processed
    @param Stack           - stack index to be processed
    @param DevScope        - buffer for device scope data structure
    @param PciNode         - buffer for PCI node data structure
    @param PciRootBridgeIo - pointer to PciRootBridgeIo protocol for PCI access

    @retval EFI_SUCCESS - DRHD entry built successfully

**/
EFI_STATUS
BuildDRHDForStack (
  EFI_DMA_REMAP_PROTOCOL          *DmaRemap,
  EFI_IIO_SYSTEM_PROTOCOL         *mIioSystem,
  UINT8                           IioIndex,
  UINT8                           Stack,
  DEVICE_SCOPE                    *DevScope,
  PCI_NODE                        *PciNode,
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT8                       Bus;
  UINT8                       DevIndex;
  UINT8                       PciNodeIndex;
  UINT8                       PortIndex;
  UINT8                       CBIndex;
  UINT64                      VtdMmioExtCap;
  UINT32                      VtdBase;
  BOOLEAN                     IntrRemapSupport;
  UINT32                      VidDid;
  FPGA_CONFIGURATION          FpgaConfiguration;

  //
  // BIOS detect FPGA SKU
  //
  Status = FpgaConfigurationGetValues (&FpgaConfiguration);
  if(!EFI_ERROR (Status)) {
    // 
    // Build FPGA DRHD entry
    //
    if ((FpgaConfiguration.FpgaSktActive & (1 << IioIndex)) && ( Stack == IIO_PSTACK3)){   
      DEBUG((EFI_D_ERROR, " Build FPGA DRHD entry for MCP0 port @IIO[%d], Stack[%d].\n", IioIndex, Stack));  
      DevIndex                      = 00;
      PciNodeIndex                  = 00;
      mDrhd.DeviceScopeNumber       = 00;
      mDrhd.RegisterBase            = (UINT32)mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[IioIndex][Stack];
      ZeroMem (DevScope, 100 * sizeof (DEVICE_SCOPE));
      ZeroMem (PciNode, 100 * sizeof (PCI_NODE));    
      mDrhd.Flags       = EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_CLEAR;  // all non-legacy stack has INCLUDE_ALL flag cleared
      //
      //Report RCiEP0 device
      //
      DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
      DevScope[DevIndex].EnumerationID      = 00;
      DevScope[DevIndex].StartBusNumber     = mIioSystem->IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[IioIndex][Stack] - FPGA_PREAllOCATE_BUS_NUM + 0x01; 
      DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
      DevIndex++;      
      DEBUG((EFI_D_ERROR, " Build FPGA DRHD entry: VtdBarAddress =[0x%x] BusNumber = [0x%x].\n", mDrhd.RegisterBase , DevScope[DevIndex].StartBusNumber));  

      PciNode[PciNodeIndex].Device          = 0;
      PciNode[PciNodeIndex].Function        = 0;
      PciNodeIndex++;      
      PciNode[PciNodeIndex].Device    = (UINT8) -1;
      PciNode[PciNodeIndex].Function  = (UINT8) -1;
      PciNodeIndex++;
      mDrhd.DeviceScopeNumber++;
      //
      //Report VF device
      //
      DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
      DevScope[DevIndex].EnumerationID      = 00;
      DevScope[DevIndex].StartBusNumber     = mIioSystem->IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[IioIndex][Stack] - FPGA_PREAllOCATE_BUS_NUM + 0x01; 
      DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
      DevIndex++;

      PciNode[PciNodeIndex].Device          = 0;
      PciNode[PciNodeIndex].Function        = 1;
      PciNodeIndex++;
      PciNode[PciNodeIndex].Device    = (UINT8) -1;
      PciNode[PciNodeIndex].Function  = (UINT8) -1;
      PciNodeIndex++;
      mDrhd.DeviceScopeNumber++;

      DmaRemap->InsertDmaRemap (DmaRemap, DrhdType, &mDrhd);      
      return EFI_SUCCESS;
    }
  }

  DevIndex                      = 00;
  PciNodeIndex                  = 00;
  mDrhd.DeviceScopeNumber       = 00;
  mDrhd.RegisterBase            = (UINT32)mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[IioIndex][Stack];
  ZeroMem (DevScope, 100 * sizeof (DEVICE_SCOPE));
  ZeroMem (PciNode, 100 * sizeof (PCI_NODE));

  mDrhd.Flags       = EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_CLEAR;  // all non-legacy stack has INCLUDE_ALL flag cleared

  VtdBase = (UINT32)mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[IioIndex][Stack];
  VtdMmioExtCap     = *(volatile UINT64 *)(UINTN) (VtdBase + R_VTD_EXT_CAP_LOW);

  //
  // Check Interrupt Remap support.
  //
  IntrRemapSupport = FALSE;
  if ((VtdMmioExtCap) & INTRREMAP) {
    IntrRemapSupport = TRUE;
  }

  //
  // DRHD - IOxAPCI entry
  //
#if MAX_SOCKET > 4
  if ((mPlatformInfo->SysData.SysIoApicEnable & IoApicEnable[IioIndex*(TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET)+Stack])
#else
  if ((mPlatformInfo->SysData.SysIoApicEnable & IoApicEnable[IioIndex*MAX_IIO_STACK+Stack])
#endif
       && (DmaRemap->InterruptRemap) && (IntrRemapSupport)) {
    DEBUG((EFI_D_ERROR, " Build IOxAPIC DRHD entry IIO[%d], Stack[%d].\n", IioIndex, Stack));
    DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC;
#if MAX_SOCKET > 4
    DevScope[DevIndex].EnumerationID      = IoApicID[IioIndex*(TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET)+Stack];
#else
    DevScope[DevIndex].EnumerationID      = IoApicID[IioIndex*MAX_IIO_STACK+Stack];
#endif
    DevScope[DevIndex].StartBusNumber     = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[IioIndex].StackBus[Stack];
    DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
    DevIndex++;
    PciNode[PciNodeIndex].Device          = APIC_DEV_NUM;
    PciNode[PciNodeIndex].Function        = APIC_FUNC_NUM;
    PciNodeIndex++;
    PciNode[PciNodeIndex].Device    = (UINT8) -1;
    PciNode[PciNodeIndex].Function  = (UINT8) -1;
    PciNodeIndex++;
     mDrhd.DeviceScopeNumber++;
  }

  //
  // DRHD - CBDMA entry
  //
  if( Stack == IIO_CSTACK) {
      DEBUG((EFI_D_ERROR, "    CB: Build CB Index into DRHD for IIO[%d].\n", IioIndex));

      for(CBIndex = 0; CBIndex <= 7; CBIndex++) {

        DEBUG((EFI_D_ERROR, "    CB: Build CBIndex=%d on socket=%d.\n",CBIndex,IioIndex));

        //if( Array[CBIndex] == 0 ) continue;

        DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
        DevScope[DevIndex].EnumerationID      = 00;
        DevScope[DevIndex].StartBusNumber     = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[IioIndex].StackBus[IIO_CSTACK];
        DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
        DevIndex++;

        PciNode[PciNodeIndex].Device          = 04;
        PciNode[PciNodeIndex].Function        = CBIndex;
        PciNodeIndex++;
        PciNode[PciNodeIndex].Device    = (UINT8) -1;
        PciNode[PciNodeIndex].Function  = (UINT8) -1;
        PciNodeIndex++;

        mDrhd.DeviceScopeNumber++;
      } // End of for for(CBIndex = 0; CBIndex <= 07; CBIndex++)

  } //if( Stack == IIO_CSTACK)

  //
  // DRHD - PCI-Ex ports
  //
  for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    if( mIioSystem->IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex] != Stack)
    	 continue;

    Bus = mIioSystem->IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
    //
    // Skip root ports which do not respond to PCI configuration cycles.
    //
    Status = PciRootBridgeIo->Pci.Read (
                PciRootBridgeIo,
                EfiPciWidthUint32,
                EFI_PCI_ADDRESS (
                  Bus,
                  mDevScopeATSR [PortIndex].PciNode->Device,
                  mDevScopeATSR [PortIndex].PciNode->Function,
                  0),
                  1,
                  &VidDid);
      if (EFI_ERROR (Status) || VidDid == 0xffffffff) {
        continue;
    }

    DEBUG((EFI_D_ERROR, " PCIEX: Build PCIEX IIO%d, root port=%d.\n",IioIndex, PortIndex));
    DEBUG((EFI_D_ERROR, "        PXPMap.PciPortIndex=%d.\n",mIioSystem->IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex*(NUMBER_PORTS_PER_SOCKET))+PortIndex]));

    if(!mIioSystem->IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex*(NUMBER_PORTS_PER_SOCKET))+PortIndex]) continue;

    if ((mIioSystem->IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == NTB_PORT_DEF_NTB_NTB ) ||
        (mIioSystem->IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == NTB_PORT_DEF_NTB_RP )) {
      DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
    } else {
      DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE;
    }
    DEBUG((EFI_D_ERROR, " PCIEX: Device type=%d.\n",DevScope[DevIndex].DeviceType));
    DevScope[DevIndex].EnumerationID      = 00;
    DevScope[DevIndex].StartBusNumber     = Bus;
    DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
    DevIndex++;

    PciNode[PciNodeIndex].Device          = mDevScopeATSR[PortIndex].PciNode->Device;
    PciNode[PciNodeIndex].Function        = mDevScopeATSR[PortIndex].PciNode->Function;
    PciNodeIndex++;
    PciNode[PciNodeIndex].Device    = (UINT8) -1;
    PciNode[PciNodeIndex].Function  = (UINT8) -1;
    PciNodeIndex++;

    mDrhd.DeviceScopeNumber++;
  } // End of for(PciPortIndex = 0; PciPortIndex < NUMBER_PORTS_PER_SOCKET; PciPortIndex++)

  if ((Stack >= IIO_PSTACK0 ) && (Stack <= IIO_PSTACK2)){
     if(mIioSystem->IioGlobalData->SetupData.VMDEnabled[((IioIndex*VMD_STACK_PER_SOCKET)+ (Stack -1))]){
     DEBUG((EFI_D_ERROR, " Build DHRD VMD IIO%d, Stack=%d.\n",IioIndex, Stack));
     DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
     DevScope[DevIndex].EnumerationID      = 00;
     DevScope[DevIndex].StartBusNumber     = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[IioIndex].StackBus[Stack];
     DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
     DevIndex++;

     PciNode[PciNodeIndex].Device          = 05;
     PciNode[PciNodeIndex].Function        = 05;
     PciNodeIndex++;
     PciNode[PciNodeIndex].Device    = (UINT8) -1;
     PciNode[PciNodeIndex].Function  = (UINT8) -1;
     PciNodeIndex++;

     mDrhd.DeviceScopeNumber++;
     }
  }

  DmaRemap->InsertDmaRemap (DmaRemap, DrhdType, &mDrhd);

  return EFI_SUCCESS;
}
// APTIOV_SERVER_OVERRIDE_RC_START : Adding RHSA structure in DMAR table
/**
 * 
 * Method to add RHSA structure to DMAR for each stack
 * 
 * @param DmaRemap   Pointer to Dma Remap Protocol
 * @param SocketIndx Index of socket need to be reported 
 * @param StackIndx  Index of stack need to be reported
 * @return VOID
 */
 VOID
 BuildRhsaDmar (
  EFI_DMA_REMAP_PROTOCOL          *DmaRemap,
  UINT8                            SocketIndx,
  UINT8                            StackIndx
  )
{

  mRhsa.Domian = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndx].SocId;
  mRhsa.RegisterBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndx].StackRes[StackIndx].VtdBarAddress;
  DmaRemap->InsertDmaRemap(
             DmaRemap,
             RhsaType,
             &mRhsa);
}
// APTIOV_SERVER_OVERRIDE_RC_END : Adding RHSA structure in DMAR table
/**

    GC_TODO: add routine description

    @param DmaRemap - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
ReportDmar (
  EFI_DMA_REMAP_PROTOCOL      *DmaRemap
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT8                       SocketIndex, IioBusBase, Bus;
  UINT8                       DevIndex;
  UINT8                       PciNodeIndex;
  UINT8                       PciPortIndex;
  UINT64                      VtdMmioExtCap;
  UINT32                      VtdBase;
  EFI_VTD_SUPPORT_INSTANCE    *DmarPrivateData;
  UINT16                      NumberOfHpets;
  UINT16                      HpetCapIdValue;
  DEVICE_SCOPE                *DevScope;
  PCI_NODE                    *PciNode;
// APTIOV_SERVER_OVERRIDE_RC_START : For filling Reserved Base Address and Limit for RMRR structure of DMAR table
#if 0
  EFI_PHYSICAL_ADDRESS        Pointer;
  UINT32                      AlignedSize;
  UINT32                      NumberofPages;
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : For filling Reserved Base Address and Limit for RMRR structure of DMAR table
  BOOLEAN                     IntrRemapSupport;
  EFI_IIO_SYSTEM_PROTOCOL     *mIioSystem;
  EFI_CPUID_REGISTER          CpuidRegisters;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINT32                      VidDid;
  UINT8                       Index;
  UINT8                       Stack;
  // APTIOV_SERVER_OVERRIDE_RC_START
   //For filling Reserved Base Address and Limit for RMRR structure of DMAR table
  #if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
    static EFI_GUID UsbProtocolGuid = EFI_USB_PROTOCOL_GUID;
    EFI_USB_PROTOCOL *UsbProtocol;
    EFI_PHYSICAL_ADDRESS	StartAddress = 0, EndAddress = 0;
  #endif
   //Adding RHSA structure in DMAR table
    UINT8 SocketIndx = 0;
    UINT8 StackIndx;
    UINT8 NumaEnb = 0;
  // APTIOV_SERVER_OVERRIDE_RC_END
  DmarPrivateData = VTD_SUPPORT_INSTANCE_FROM_THIS (DmaRemap);
  //
  // Get DMAR_HOST_ADDRESS_WIDTH from CPUID.(EAX=80000008h) return the Phyical Address
  // Size in the EAX register. EAX[7:0]
  // Sync with Brickland code  DMAR_HOST_ADDRESS_WIDTH 45 = 46 - 1
  //
      AsmCpuid (
      EFI_CPUID_VIR_PHY_ADDRESS_SIZE,
      &CpuidRegisters.RegEax,
      &CpuidRegisters.RegEbx,
      &CpuidRegisters.RegEcx,
      &CpuidRegisters.RegEdx
      );

  DmarPrivateData->Dmar->HostAddressWidth = (UINT8)((CpuidRegisters.RegEax & 0xFF)-1);
  DmarPrivateData->Dmar->Flags = EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_CLEAR;

  //
  // Locate PCI root bridge I/O protocol, for confirming PCI functions respond
  // to PCI configuration cycles.
  //
  Status = gBS->LocateProtocol (
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &PciRootBridgeIo
                );
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate memory to DevScope structures
  //
  Status = gBS->AllocatePool (EfiACPIMemoryNVS, 100 * sizeof (DEVICE_SCOPE), &DevScope);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->AllocatePool (EfiACPIMemoryNVS, 100 * sizeof (PCI_NODE), &PciNode);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &mIioSystem);
  ASSERT_EFI_ERROR (Status);
//
// APTIOV_SERVER_OVERRIDE_RC_START : Fix issue of missing some valid CPU DRHD
//
  for ( Index = 1; Index <= MAX_SOCKET; Index++) {
//
// APTIOV_SERVER_OVERRIDE_RC_END : Fix issue of missing some valid CPU DRHD
//
   //
   // VT-d specification request that DHRD entry 0 should be the latest entry of the DMAR table.
   // To accomplish this, the following check will ensure that latest entry will be the one related to Socket 0.
   //
   //
   // APTIOV_SERVER_OVERRIDE_RC_START : Fix issue of missing some valid CPU DRHD
   //
    if (Index == MAX_SOCKET) {
   //
   // APTIOV_SERVER_OVERRIDE_RC_END : Fix issue of missing some valid CPU DRHD
   //
      SocketIndex = 0;
    } else {
      SocketIndex = Index;
    }

    if (SocketIndex >= MAX_SOCKET) {
      return EFI_INVALID_PARAMETER;
    }

    if(mIioSystem->IioGlobalData->IioVar.IioVData.SocketPresent[SocketIndex] != 1) {
      continue;
    }

    //
    // Fixup PciRootBridge IO protocol to use the correct segment for this RootBridge
    //
    PciRootBridgeIo->SegmentNumber  = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].PcieSegment;

    DevIndex                      = 00;
    PciNodeIndex                  = 00;

    mDrhd.Signature               = DRHD_SIGNATURE;
    mDrhd.SegmentNumber           = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].PcieSegment;
    mDrhd.DeviceScopeNumber       = 00;
    mDrhd.DeviceScope             = DevScope;
    mDrhd.RegisterBase            = mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[SocketIndex][IIO_CSTACK];
    ZeroMem (DevScope, 100 * sizeof (DEVICE_SCOPE));
    ZeroMem (PciNode, 100 * sizeof (PCI_NODE));

    VtdBase = (UINT32)mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[SocketIndex][IIO_CSTACK];
    VtdMmioExtCap     = *(volatile UINT64 *)(UINTN) (VtdBase + R_VTD_EXT_CAP_LOW);

    //
    // Check Interrupt Remap support.
    //
    IntrRemapSupport = FALSE;
    if ((VtdMmioExtCap) & INTRREMAP) {
      IntrRemapSupport = TRUE;
    }

    DEBUG((EFI_D_ERROR, "Build DRHD Table SockectIndex    =%d.\n", SocketIndex));
    DEBUG((EFI_D_ERROR, "        mIioSystem->IioGlobalData->IioVar.IioVData.SocketPresent[%d]=%d.\n", SocketIndex, mIioSystem->IioGlobalData->IioVar.IioVData.SocketPresent[SocketIndex]));

    if (SocketIndex == 0 ) {
      //
      // DRHD - Legacy IOH
      //
      // Build DRHD on IIO0 - Stack1 to Stack5, not include C-STACK
      for( Stack=1; Stack<MAX_IIO_STACK ; Stack++) {
          // Check for a valid stack
          if (!(mIioSystem->IioGlobalData->IioVar.IioVData.StackPresentBitmap[SocketIndex] & (1 << Stack)))
            continue;
          BuildDRHDForStack (DmaRemap, mIioSystem, SocketIndex, Stack, DevScope, PciNode, PciRootBridgeIo);
      } //for( StackIndex=1; StackIndex<MAX_IIO_STACK ; StackIndex++) {

      // re-initialize DRHD template for DRHD entry in legacy socket C-STACK
      DevIndex                      = 00;
      PciNodeIndex                  = 00;
      mDrhd.DeviceScopeNumber       = 00;
      mDrhd.RegisterBase            = (UINT32)mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[SocketIndex][IIO_CSTACK]; // C-Stack on legacy socket
      ZeroMem (DevScope, 100 * sizeof (DEVICE_SCOPE));
      ZeroMem (PciNode, 100 * sizeof (PCI_NODE));

      IioBusBase = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].StackBus[IIO_CSTACK]; // Stack 0
      mDrhd.Flags                   = EFI_ACPI_DMAR_DRHD_FLAGS_INCLUDE_ALL_SET;
      DEBUG((EFI_D_ERROR, "The InterruptRemapOption value is %02d\n", DmaRemap->InterruptRemap));
      if((DmaRemap->InterruptRemap) && (IntrRemapSupport)){

        DmarPrivateData->Dmar->Flags = EFI_ACPI_DMAR_TABLE_FLAGS_INTR_REMAP_SET | EFI_ACPI_DMAR_TABLE_FLAGS_X2APIC_OPT_OUT;

        if (Status == EFI_SUCCESS && mSocketProcessorCoreConfiguration.ProcessorX2apic == 1) {
          DmarPrivateData->Dmar->Flags &= ~EFI_ACPI_DMAR_TABLE_FLAGS_X2APIC_OPT_OUT;
        }

        // PCH - IOAPIC
        DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC;
        DevScope[DevIndex].EnumerationID      = PCH_IOAPIC_ID;
        DevScope[DevIndex].StartBusNumber     = PCI_BUS_NUMBER_PCH_IOAPIC;
        DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
        DevIndex++;

        PciNode[PciNodeIndex].Device          = (UINT8)PCI_DEVICE_NUMBER_PCH_IOAPIC;
        PciNode[PciNodeIndex].Function        = (UINT8)PCI_FUNCTION_NUMBER_PCH_IOAPIC;
        PciNodeIndex++;
        PciNode[PciNodeIndex].Device    = (UINT8) -1;
        PciNode[PciNodeIndex].Function  = (UINT8) -1;
        PciNodeIndex++;

        mDrhd.DeviceScopeNumber++;
        //
        // PC00, PC01, PC02, PC03, PC04, PC05 IOAPIC
        //
        if (mPlatformInfo->SysData.SysIoApicEnable & PC00_IOAPIC){

          DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_IOAPIC;
          DevScope[DevIndex].EnumerationID      = PC00_IOAPIC_ID;
          DevScope[DevIndex].StartBusNumber     = IioBusBase;
          DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
          DevIndex++;

          PciNode[PciNodeIndex].Device          = APIC_DEV_NUM;
          PciNode[PciNodeIndex].Function        = APIC_FUNC_NUM;
          PciNodeIndex++;
          PciNode[PciNodeIndex].Device    = (UINT8) -1;
          PciNode[PciNodeIndex].Function  = (UINT8) -1;
          PciNodeIndex++;

          mDrhd.DeviceScopeNumber++;
        } // End of if (mPlatformInfo->SysData.SysIoApicEnable & IIO0_IOAPIC)

        HpetCapIdValue = *(UINT16 *)(UINTN)(HPET_BLOCK_ADDRESS);
        NumberOfHpets = (HpetCapIdValue >> 0x08) & 0x1F;  // Bits [8:12] contaoins the number of Hpets

        if(NumberOfHpets && (NumberOfHpets != 0x1f) &&
          (*((volatile UINT32 *)(UINTN)(HPET_BLOCK_ADDRESS + 0x100)) & BIT15)) {

          DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_MSI_CAPABLE_HPET;
          DevScope[DevIndex].EnumerationID      = 00;
          DevScope[DevIndex].StartBusNumber     = PCI_BUS_NUMBER_PCH_HPET;
          DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
          DevIndex++;

          PciNode[PciNodeIndex].Device          = PCI_DEVICE_NUMBER_PCH_HPET;
          PciNode[PciNodeIndex].Function        = PCI_FUNCTION_NUMBER_PCH_HPET0;
          PciNodeIndex++;
          PciNode[PciNodeIndex].Device    = (UINT8) -1;
          PciNode[PciNodeIndex].Function  = (UINT8) -1;
          PciNodeIndex++;

          mDrhd.DeviceScopeNumber++;
        }
      } // DmaRemap->InterruptRemap

    DmaRemap->InsertDmaRemap (DmaRemap, DrhdType, &mDrhd);

      } else { // End of if (IioSocketId == 0)

      // Build DRHD on IIO1 - Stack0 to Stack5
      for( Stack=0; Stack<MAX_IIO_STACK ; Stack++ ) {
        // Check for a valid stack
        if (!(mIioSystem->IioGlobalData->IioVar.IioVData.StackPresentBitmap[SocketIndex] & (1 << Stack)))
          continue;
        BuildDRHDForStack (DmaRemap, mIioSystem, SocketIndex, Stack, DevScope, PciNode, PciRootBridgeIo);
      } //for( StackIndex=0; StackIndex<MAX_IIO_STACK ; StackIndex++) {
    } // End of if (IioSocketId == 0)

  } // End of for ( Index = 1; Index <= mIioUds->IioUdsPtr->SystemStatus.numCpus; Index++)

//
// For SKX, AzaliaUseNonISOCH need to be 1 all the time since Isoch engine 
// will not be used for SKX and Azalia traffic always use non-Isoch VT-d engine.
// HSD SKX Si : 1451396: Azalia Tag ID's used in the VTd engine 
// are not valid due to Azalia changes in Sunrisepoint
//
//
  // ATSR
  //
  if (DmaRemap->ATS) {
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Fix issue of missing some valid CPU ATSR
  //
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
  //
  // APTIOV_SERVER_OVERRIDE_RC_END : Fix issue of missing some valid CPU ATSR
  //
      DEBUG((EFI_D_ERROR, "T_TEST: Build ATSR SocketIndex=%d.\n",SocketIndex));
      DEBUG((EFI_D_ERROR, "        IIO_resource.valid=%d.\n",mIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid));

      if(SocketIndex >= MAX_SOCKET) {
        return EFI_INVALID_PARAMETER;
      }

      if (!mIioSystem->IioGlobalData->IioVar.IioVData.SocketPresent[SocketIndex]) {
        continue;
      }

      IioBusBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].BusBase;

      PciNodeIndex            = 00;
      DevIndex                = 00;

      ZeroMem (DevScope, 100 * sizeof (DEVICE_SCOPE));
      ZeroMem (PciNode, 100 * sizeof (PCI_NODE));

      mAtsr.Signature         = ATSR_SIGNATURE;
      mAtsr.Flags             = 00;
      mAtsr.SegmentNumber     = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].PcieSegment;
      mAtsr.DeviceScopeNumber = 00;
      mAtsr.DeviceScope       = DevScope;
      mAtsr.ATSRPresentBit    = (UINT32)-1;   // Not useful really Backwards project compatability (removwe it later)

        //
        // Loop From Port 1 to 15 for Legacy IOH and 0 to 15 for Non-Legacy IOH
        //
        for(PciPortIndex = 0; PciPortIndex < NUMBER_PORTS_PER_SOCKET; PciPortIndex++)  {
          if((SocketIndex == 00) && (PciPortIndex == 00)) continue;
          //
          // Check device IOTLBs supported or not in VT-d Extended capability register
          //
          Stack = mIioSystem->IioGlobalData->IioVar.IioVData.StackPerPort[SocketIndex][PciPortIndex];
          // Check for a valid stack
          if (!(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap & (1 << Stack)))
            continue;

          VtdBase = (UINT32)mIioSystem->IioGlobalData->IioVar.IioVData.VtdBarAddress[SocketIndex][Stack];

          if (VtdBase){
            VtdMmioExtCap     = *(volatile UINT64 *)(UINTN) (VtdBase + R_VTD_EXT_CAP_LOW);

            //
            // ATSR is applicable only for platform supporting device IOTLBs through the VT-d extended capability register
            //
            if ((VtdMmioExtCap & BIT2) != 0) {


              Bus = mIioSystem->IioGlobalData->IioVar.IioVData.SocketPortBusNumber[SocketIndex][PciPortIndex];
              //
              // Skip root ports which do not respond to PCI configuration cycles.
              //
              Status = PciRootBridgeIo->Pci.Read (
                        PciRootBridgeIo,
                        EfiPciWidthUint32,
                        EFI_PCI_ADDRESS (
                          Bus,
                          mDevScopeATSR [PciPortIndex].PciNode->Device,
                          mDevScopeATSR [PciPortIndex].PciNode->Function,
                          0),
                        1,
                        &VidDid);

              if (EFI_ERROR (Status) || VidDid == 0xffffffff) {
                continue;
              }

              if(mIioSystem->IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(SocketIndex*(NUMBER_PORTS_PER_SOCKET))+PciPortIndex]) {
                if ((mIioSystem->IioGlobalData->IioVar.IioOutData.PciePortOwnership[(SocketIndex * NUMBER_PORTS_PER_SOCKET) + PciPortIndex] == NTB_PORT_DEF_NTB_NTB ) ||
                    (mIioSystem->IioGlobalData->IioVar.IioOutData.PciePortOwnership[(SocketIndex * NUMBER_PORTS_PER_SOCKET) + PciPortIndex] == NTB_PORT_DEF_NTB_RP )) {
                  DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_ENDPOINT;
                } else {
                  DevScope[DevIndex].DeviceType         = EFI_ACPI_DEVICE_SCOPE_ENTRY_TYPE_BRIDGE;
                }
                 DevScope[DevIndex].EnumerationID      = 00;

                 DevScope[DevIndex].StartBusNumber     = Bus;
                 DevScope[DevIndex].PciNode            = &PciNode[PciNodeIndex];
                 DevIndex++;

                 PciNode[PciNodeIndex].Device          = mDevScopeATSR[PciPortIndex].PciNode->Device;
                 PciNode[PciNodeIndex].Function        = mDevScopeATSR[PciPortIndex].PciNode->Function;
                 PciNodeIndex++;
                 PciNode[PciNodeIndex].Device    = (UINT8) -1;
                 PciNode[PciNodeIndex].Function  = (UINT8) -1;
                 PciNodeIndex++;

                 mAtsr.DeviceScopeNumber++;
              } // (mIioSystem->IioGlobalData->IioVar.IioOutData.CurrentPXPMap[RootBridgeLoop][PciPortIndex])
            }// End of if ((VtdMmioExtCap & BIT2) != 0)
          } // End of if VtdBase
        } // End of for(PciPortIndex = 0; PciPortIndex < NUMBER_PORTS_PER_SOCKET; PciPortIndex++)
      if(mAtsr.DeviceScopeNumber)
        DmaRemap->InsertDmaRemap (DmaRemap, AtsrType, &mAtsr);
    } // End of for (RootBridgeLoop = 0; RootBridgeLoop < mIioUds->IioUdsPtr->PlatformData.numofIIO; RootBridgeLoop++)
  } // End of if (ATS) {

  //
  // RMRR
  //
// APTIOV_SERVER_OVERRIDE_RC_START : For filling Reserved Base Address and Limit for RMRR structure of DMAR table
  #if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
  	Status = gBS->LocateProtocol(&UsbProtocolGuid, NULL, &UsbProtocol);
  	if(!EFI_ERROR(Status))
  	{
  		Status = UsbProtocol->UsbGetRuntimeRegion(&StartAddress, &EndAddress);
  		DEBUG((EFI_D_ERROR, "The RMRR Mem Base is 0x%x\n", StartAddress));
  	}
  #endif

#if 0
  AlignedSize  = (MEM_BLK_COUNT * sizeof(MEM_BLK));
  if( AlignedSize % 0x1000)  AlignedSize  = ( (MEM_BLK_COUNT * sizeof(MEM_BLK)) & (~0xfff) ) + 0x1000; // aligend to 4k Boundary
  NumberofPages = AlignedSize/0x1000;
  //
  // Allocate memory (below 4GB)
  //
  Pointer = 0xffffffff;
  Status = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   NumberofPages,
                   &Pointer // Base address need to be 4K aligned for VT-d RMRR
                   );
  ASSERT_EFI_ERROR(Status);
  DEBUG((EFI_D_ERROR, "The RMRR Mem Base is 0x%x\n", Pointer));
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : For filling Reserved Base Address and Limit for RMRR structure of DMAR table

  if (DmaRemap->VTdSupport == TRUE) {

// APTIOV_SERVER_OVERRIDE_RC_START : For filling Reserved Base Address and Limit for RMRR structure of DMAR table
    #if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
    if ( StartAddress != 0 ) {
// APTIOV_SERVER_OVERRIDE_RC_END : For filling Reserved Base Address and Limit for RMRR structure of DMAR table
    //
    // RMRR
    //
    mRmrr.DeviceScope       = &DevScopeRmrr[0];
    // Calculate the right size of DevScope for mRmrr entry
    mRmrr.DeviceScopeNumber = sizeof(DevScopeRmrr)/sizeof(DEVICE_SCOPE);
// APTIOV_SERVER_OVERRIDE_RC_START
    // Use USB Runtime Start and End Address
    mRmrr.RsvdMemBase       = (UINT64)StartAddress;
    DEBUG((EFI_D_ERROR, "The RMRR Mem Base is 0x%x\n", mRmrr.RsvdMemBase));
    mRmrr.RsvdMemLimit      = EndAddress;
// APTIOV_SERVER_OVERRIDE_RC_END
    DmaRemap->InsertDmaRemap (DmaRemap, RmrrType, &mRmrr);
// APTIOV_SERVER_OVERRIDE_RC_START
    }
    #endif

  //OEM VTD RMRR support
        #if defined OemVtdRmrr_SUPPORT && OemVtdRmrr_SUPPORT == 1
        {
            EFI_OEM_VTD_RMRR_PROTOCOL *OemVtdRmrrProtocol;
            static EFI_GUID gEfiOemVtdRmrrProtocolGuid = EFI_OEM_VTD_RMRR_PROTOCOL_GUID;

            Status = gBS->LocateProtocol(&gEfiOemVtdRmrrProtocolGuid, NULL, &OemVtdRmrrProtocol);
            DEBUG((EFI_D_ERROR, "LocateProtocol gEfiOemVtdRmrrProtocolGuid. Status: %r \n", Status));
            if( !EFI_ERROR(Status) ) {
              Status = OemVtdRmrrProtocol->InsertOemVtdRmrr (DmaRemap);
              DEBUG((EFI_D_ERROR, "OemVtdRmrrProtocol->InsertOemVtdRmrr Status: %r \n", Status));
            }
        }
        #endif
  }
  // Adding RHSA structure in DMAR table
  Status = GetOptionData(
                 &gEfiSocketCommonRcVariableGuid,
                 OFFSET_OF(SOCKET_COMMONRC_CONFIGURATION, NumaEn),
                 &NumaEnb,
                 sizeof(NumaEnb));
  if ((!EFI_ERROR(Status)) && NumaEnb) {
     //
     //Initializing default values
     //
     mRhsa.Signature = RHSA_SIGNATURE;
     mRhsa.RhsaCount = 0;
     for ( ; SocketIndx < MAX_SOCKET; SocketIndx++) {
       for (StackIndx = 0; StackIndx < MAX_IIO_STACK; StackIndx++) {
         if (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndx].stackPresentBitmap & (1 << StackIndx)) {
            DEBUG((EFI_D_INFO, "Reporting DMAR RHSA For Socket = %x Stack = %x\n", SocketIndx, StackIndx));
            BuildRhsaDmar(DmaRemap, SocketIndx, StackIndx);
         }
       }
     }
  }
// APTIOV_SERVER_OVERRIDE_RC_END
  gBS->FreePool (PciNode);
  gBS->FreePool (DevScope);

  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param None

    @retval EFI_SUCCESS   - GC_TODO: add retval description
    @retval EFI_NOT_FOUND - GC_TODO: add retval description

**/
EFI_STATUS
VtdAcpiTablesUpdateFn (
  VOID
  )
{

  EFI_STATUS                  Status;
  EFI_ACPI_TABLE_VERSION      TableVersion;
  EFI_DMA_REMAP_PROTOCOL      *DmaRemap;
  UINTN                       TableHandle;
  EFI_ACPI_COMMON_HEADER      *CurrentTable;
  EFI_ACPI_TABLE_PROTOCOL     *AcpiTable;

  UINTN                       HandleCount;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       Index;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  PCI_TYPE01                  PciConfigHeader;
  UINTN                       Segment;
  UINTN                       Bus;
  UINTN                       Device;
  UINTN                       Function;
  UINT8                       PciExpressOffset;
  UINT32                      AcsOffset;
  UINT16                      PciExpressCapabilityReg;
  UINT16                      AcsCapCount;
  UINT16                      RequiredAcsCap;
  UINT32                      AcsCapRegValue;
  UINT16                      AcsConRegValue;
#if SMCPKG_SUPPORT
  USRA_PCIE_ADDR_TYPE          AcsDevArray[500];
#else
  USRA_PCIE_ADDR_TYPE          AcsDevArray[100];
#endif
#ifdef ACS_WA
  UINT8                       RootBridgeLoop;
  UINT32                      VtdBase;
  UINT8                       CORE20_BUSNUM=0;
#endif
  USRA_ADDRESS                Address;

#if SMCPKG_SUPPORT
  SMC_BDF_ACS DevList[64];
  UINT32 ii = 0;
  UINT8 FoundNvidiaGpu = 0;

  for (ii = 0; ii < 64; ii++) {
    DevList[ii].Bus = 0xff;
    DevList[ii].Dev = 0xff;
    DevList[ii].Fun = 0xff;
    DevList[ii].SecBus = 0xff;
    DevList[ii].DisAcs = 0;
  }
#endif
  
  PciExpressOffset = 0;
  AcsOffset        = 0;
  AcsCapCount      = 0;
  AcsCapRegValue   = 0;
  AcsConRegValue   = 0;
  RequiredAcsCap   =  ACS_SOURCE_VALIDATION | ACS_P2P_REQUEST_REDIRECT | ACS_P2P_COMPLETION_REDIRECT | ACS_UPSTREAM_FORWARDING;

  //
  // Locate all PciIo protocol
  //
  Status = gBS->LocateHandleBuffer (
               ByProtocol,
               &gEfiPciIoProtocolGuid,
               NULL,
               &HandleCount,
               &HandleBuffer
               );
  for (Index = 0; Index < HandleCount; Index ++) {
    gBS->HandleProtocol (
          HandleBuffer[Index],
          &gEfiPciIoProtocolGuid,
          &PciIo
          );
    PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint32,
                0,
                sizeof (PciConfigHeader) / sizeof (UINT32),
                &PciConfigHeader
                );
		
#if SMCPKG_SUPPORT
      PciIo->GetLocation (
              PciIo,
              &Segment,
              &Bus,
              &Device,
              &Function
              );

    //
    // Check VID is NVIDIA.
    //
    if (PciConfigHeader.Hdr.VendorId == 0x10DE) {
      for (ii = 0; ii < 64; ii++) {
        if (DevList[ii].Bus == 0xff && DevList[ii].Dev == 0xff && DevList[ii].Fun == 0xff) {
          break;
        }
        if (Bus == DevList[ii].SecBus) {
          DevList[ii].DisAcs = 1;
        }
      }
    }
#endif
		
    if ((PciConfigHeader.Hdr.ClassCode[0] == 0x00 || PciConfigHeader.Hdr.ClassCode[0] == 0x01) && PciConfigHeader.Hdr.ClassCode[1] == 0x04 && PciConfigHeader.Hdr.ClassCode[2] == 0x06) {
      //
      // 060400h or 060401h indicates it's PCI-PCI bridge, get its bus number, device number and function number
      //
      PciIo->GetLocation (
              PciIo,
              &Segment,
              &Bus,
              &Device,
              &Function
              );
      USRA_PCIE_SEG_ADDRESS(Address, UsraWidth16, Segment, Bus, Device, Function, 0);

#if SMCPKG_SUPPORT
      //
      // Add bridge.
      //
      for (ii = 0; ii < 64; ii++) {
        if (DevList[ii].Bus == 0xff && DevList[ii].Dev == 0xff && DevList[ii].Fun == 0xff) {
          DevList[ii].Bus = (UINT8)Bus;
          DevList[ii].Dev = (UINT8)Device;
          DevList[ii].Fun = (UINT8)Function;
          DevList[ii].SecBus = PciConfigHeader.Bridge.SecondaryBus;
          DevList[ii].DisAcs = 0;
          break;
        }
      }
#endif
      
      if (PciConfigHeader.Hdr.Status == EFI_PCI_STATUS_CAPABILITY) {
        //
        // the bridge support Capability list and offset 0x34 is the pointer to the data structure
        //
        //
        // Detect if PCI Express Device
        //
        Status = LocateCapRegBlock (PciIo, EFI_PCI_CAPABILITY_ID_PCIEXP, &PciExpressOffset, NULL);

        if (Status == EFI_SUCCESS) {
          //
          // this bridge device is a PCI Express bridge
          // Check if it is downstream port of PCIE switch
          //
          Address.Pcie.Offset = PciExpressOffset + EFI_PCI_EXPRESS_CAPABILITY_REGISTER;
          RegisterRead(&Address, &PciExpressCapabilityReg);

          //
          // BIT 7:4 indicate Device/port type, 0110b indicates downstream port of PCI express switch
          //
          if ((PciExpressCapabilityReg & 0x00F0) == 0x60) {
            //
            // it is downstream port of PCI Express switch
            // Look for ACS capability register in PCI express configuration space
            //
            Status = LocatePciExpressCapRegBlock (PciIo, EFI_PCIE_CAPABILITY_ID_ACS, &AcsOffset, NULL);
            DEBUG((EFI_D_ERROR, "ACS capable port is B%x.D%x.F%x - ACS Cap offset - 0x%x\n", Bus, Device, Function, AcsOffset));

            if (Status == EFI_SUCCESS) {
              //
              // Read ACS capability register
              //
              Address.Pcie.Offset = AcsOffset + ACS_CAPABILITY_REGISTER;
              Address.Attribute.AccessWidth = UsraWidth32;
              RegisterRead(&Address, &AcsCapRegValue);
              DEBUG((EFI_D_INFO, "Bus =%x, Device=%x, Function=%x, AcsCapRegValue = %x \n", Bus, Device, Function, AcsCapRegValue));

              if ((AcsCapRegValue & RequiredAcsCap) == RequiredAcsCap) {
                //
                // The PCI express downstream port support ACS, record this port
                //
                AcsDevArray[AcsCapCount].Bus = (UINT32)Bus;
                AcsDevArray[AcsCapCount].Dev = (UINT32)Device;
                AcsDevArray[AcsCapCount].Func = (UINT32)Function;
                AcsDevArray[AcsCapCount].Offset = AcsOffset;                
                AcsDevArray[AcsCapCount].Seg = (UINT32)Segment;
                AcsCapCount++;
#ifdef ACS_WA
              } else {
                //
                // Even though this PCIe switch downstream port support ACS, but it does not meet all the required capabilities, so also disable the VT-d
                //
                DisableVtd = TRUE;
                break;
              }
            } else {
              //
              // At least one PCIe switch downstream port does not support ACS, just disable the VT-d report
              //
              DisableVtd = TRUE;
              break;
            }
#else
          }
        }
#endif
          }
      }
    }
  }
  }  /// End for

  //
  //Free the Handle buffer
  //
  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
  }

#if SMCPKG_SUPPORT
  ASSERT(AcsCapCount <= 500);
#else
  ASSERT(AcsCapCount <= 100);  
#endif

#ifdef ACS_WA
  if (DisableVtd == TRUE ) {
    //
    // at least one PCIE downstream port does not support ACS so need to disable VT-d
    //
    //
    // Disable VT-d BAR
    //
    USRA_PCIE_SEG_ADDRESS(Address, UsraWidth32, Segment, Bus, CORE05_DEV_NUM, CORE05_FUNC_NUM, R_IIO_VTBAR);
    for ( RootBridgeLoop =0; RootBridgeLoop < mIioUds->IioUdsPtr->PlatformData.numofIIO; RootBridgeLoop++) {
      if(!mIioUds->IioUdsPtr->PlatformData.IIO_resource[RootBridgeLoop].Valid) continue;
      VtdBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[RootBridgeLoop].VtdBarAddress & (~B_IIO_VT_BAR_EN);
      Address.Pcie.Bus = mIioUds->IioUdsPtr->PlatformData.IIO_resource[RootBridgeLoop].BusBase;
      RegisterWrite(&Address, &VtdBase);
    }

    //
    // Disable Setup option of VT-d
    //
    mSocketIioConfiguration.VtdAcsWa = FALSE;

    Status = SetOptionData (&gEfiSocketIioVariableGuid, OFFSET_OF (SOCKET_IIO_CONFIGURATION, VtdAcsWa), mSocketIioConfiguration.VtdAcsWa, sizeof(mSocketIioConfiguration.VtdAcsWa));

    //
    // Do not report DMAR table to OS/VMM then OS/VMM have no knowledge of VT-d and VT-d2
    //
    goto DisableDMAR;
  } else {
#endif
    //
    // all PCI express switch downstream ports support ACS and meet the required ACS capabilities
    // for each downstream ports, enable the required Capabilities in ACS control register
    //
    Address.Attribute.AccessWidth = UsraWidth16;
    for (Index = 0; Index < AcsCapCount; Index ++) {
    
#if SMCPKG_SUPPORT
      FoundNvidiaGpu = 0;
      for (ii = 0; ii < 64; ii++) {
        if (DevList[ii].Bus == 0xff && DevList[ii].Dev == 0xff && DevList[ii].Fun == 0xff) {
          break;
        }
        if (DevList[ii].Bus == AcsDevArray[Index].Bus && DevList[ii].Dev == AcsDevArray[Index].Dev && DevList[ii].Fun == AcsDevArray[Index].Func) {
          if (DevList[ii].DisAcs == 1) {
            FoundNvidiaGpu = 1;
          }
        }
      }
      if (FoundNvidiaGpu) continue;
#endif
    
      //
      // Program the corresponding bits in ACS control register
      //
      Address.Pcie = AcsDevArray[Index];
      Address.Pcie.Offset += ACS_CONTROL_REGISTER;
      RegisterRead(&Address, &AcsConRegValue);
      DEBUG((EFI_D_ERROR, "AcsConRegValue is 0x%x\n", AcsConRegValue));
      AcsConRegValue |= (ACS_SOURCE_VALIDATION_ENABLE|ACS_P2P_REQUEST_REDIRECT_ENABLE|ACS_P2P_COMPLETION_REDIRECT_ENABLE|ACS_UPSTREAM_FORWARDING_ENABLE);
      DEBUG((EFI_D_ERROR, "After Enable BITs AcsConRegValue is 0x%x\n", AcsConRegValue));
      RegisterWrite(&Address, &AcsConRegValue);
      //
      // report VT-d and other features to OS/VMM, report DMAR and remapping engine to OS/VMM
      //
  } //
#ifdef ACS_WA
  }
#endif

  //
  // Find the AcpiSupport protocol
  //
  Status = LocateSupportProtocol (
            &gEfiAcpiTableProtocolGuid,
            gEfiAcpiTableStorageGuid,
            &AcpiTable,
            FALSE
            );
  ASSERT_EFI_ERROR (Status);

  TableVersion = EFI_ACPI_TABLE_VERSION_2_0;

  Status = gBS->LocateProtocol (
                  &gEfiDmaRemapProtocolGuid,
                  NULL,
                  &DmaRemap
                  );
  if (Status == EFI_SUCCESS) {
    if (DmaRemap->VTdSupport == TRUE) {

      ReportDmar (DmaRemap);
      Status = DmaRemap->GetDmarTable (DmaRemap, &CurrentTable);

      if (EFI_ERROR (Status)) {

        DEBUG ((EFI_D_ERROR, "\nWARNING: VT-D ACPI table was not found!\n"));
        ASSERT_EFI_ERROR (Status);

      } else {
        //
        // Perform any table specific updates.
        //
        Status = PlatformUpdateTables (CurrentTable, &TableVersion);
        ASSERT_EFI_ERROR (Status);

        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                            AcpiTable,
                            CurrentTable,
                            CurrentTable->Length,
                            &TableHandle
                            );
        ASSERT_EFI_ERROR (Status);
      }
    }
  } else {
    DEBUG ((EFI_D_ERROR, "\nWARNING: VT-D ACPI protocol not installed - Cannot find gEfiDmaRemapProtocolGuid!\n"));
  }

  // Enable VT-D interrupt remapping
  if(VtdIntRemapEnFalg == TRUE) {
    EnableVtdIntRemapping();
  }

  return EFI_SUCCESS;
#ifdef ACS_WA
DisableDMAR:
  return EFI_NOT_FOUND;
#endif
}


EFI_STATUS
LocateCapRegBlock(
  IN     EFI_PCI_IO_PROTOCOL  *PciIo,
  IN     UINT8                CapID,
  OUT    UINT8                *PciExpressOffset,
  OUT    UINT8                *NextRegBlock)
/**

    GC_TODO: add routine description

    @param PciIo            - GC_TODO: add arg description
    @param CapID            - GC_TODO: add arg description
    @param PciExpressOffset - GC_TODO: add arg description
    @param NextRegBlock     - GC_TODO: add arg description

    @retval EFI_SUCCESS   - GC_TODO: add retval description
    @retval EFI_NOT_FOUND - GC_TODO: add retval description

**/
{
  UINT16  CapabilityID;
  UINT32  Temp;
  UINT8   CapabilityPtr;
  UINT16  CapabilityEntry;

  PciIo->Pci.Read (
            PciIo,
            EfiPciIoWidthUint32,
            PCI_CAPBILITY_POINTER_OFFSET,
            1,
            &Temp
            );

  CapabilityPtr = (UINT8)Temp;

  while ((CapabilityPtr >= 0x40) && (CapabilityPtr < 0x100)) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFC;
    PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint16,
               CapabilityPtr,
               1,
               &CapabilityEntry
               );

    CapabilityID = (UINT8) CapabilityEntry;

    if (CapabilityID == CapID) {
      *PciExpressOffset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (UINT8) ((CapabilityEntry >> 8) & 0xFC);
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (UINT8) ((CapabilityEntry >> 8) & 0xFC);
  }

  return EFI_NOT_FOUND;
}


/**

    GC_TODO: add routine description

    @param PciIo        - GC_TODO: add arg description
    @param CapID        - GC_TODO: add arg description
    @param Offset       - GC_TODO: add arg description
    @param NextRegBlock - GC_TODO: add arg description

    @retval EFI_SUCCESS   - GC_TODO: add retval description
    @retval EFI_NOT_FOUND - GC_TODO: add retval description

**/
EFI_STATUS
LocatePciExpressCapRegBlock (
  IN     EFI_PCI_IO_PROTOCOL  *PciIo,
  IN     UINT16               CapID,
  OUT    UINT32               *Offset,
  OUT    UINT32               *NextRegBlock
)
{
  UINT32  CapabilityPtr;
  UINT32  CapabilityEntry;
  UINT16  CapabilityID;

  CapabilityPtr = EFI_PCIE_CAPABILITY_BASE_OFFSET;

  while ((CapabilityPtr != 0) && (CapabilityPtr < 0x1000)) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFFC;
    PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint32,
               CapabilityPtr,
               1,
               &CapabilityEntry
               );

    CapabilityID = (UINT16) CapabilityEntry;
    // APTIOV_SERVER_OVERRIDE_RC_START : Check if Extended PciExpress Config. Space is accessible.
    if(CapabilityID == 0xFFFF) break;
    //APTIOV_SERVER_OVERRIDE_RC_END : Check if Extended PciExpress Config. Space is accessible.

    if (CapabilityID == CapID) {
      *Offset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (CapabilityEntry >> 20) & 0xFFF;
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (CapabilityEntry >> 20) & 0xFFF;
  }

  return EFI_NOT_FOUND;
}


/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
DisableAriForwarding (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       HandleCount;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       Index;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  PCI_TYPE01                  PciConfigHeader;
  UINTN                       Segment;
  UINTN                       Bus;
  UINTN                       Device;
  UINTN                       Function;
  UINT8                       PciExpressOffset;
  UINT16                      DevCon2;
  // APTIOV_SERVER_OVERRIDE_RC_START : Do not disable ARI forwarding on PCI Express rev. 1.0 bridge.
  UINT16                      PcieCap;  
  // APTIOV_SERVER_OVERRIDE_RC_END : Do not disable ARI forwarding on PCI Express rev. 1.0 bridge.

  //
  // Disable ARI forwarding before handoff to OS, as it may not be ARI-aware
  //
  //
  // ARI forwarding exist in bridge
  //

  //
  // Locate all PciIo protocol
  //
  Status = gBS->LocateHandleBuffer (
               ByProtocol,
               &gEfiPciIoProtocolGuid,
               NULL,
               &HandleCount,
               &HandleBuffer
               );
  for (Index = 0; Index < HandleCount; Index ++) {
    gBS->HandleProtocol (
          HandleBuffer[Index],
          &gEfiPciIoProtocolGuid,
          &PciIo
          );
    PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint32,
                0,
                sizeof (PciConfigHeader) / sizeof (UINT32),
                &PciConfigHeader
                );
    if ((PciConfigHeader.Hdr.ClassCode[0] == 0x00 || PciConfigHeader.Hdr.ClassCode[0] == 0x01) && PciConfigHeader.Hdr.ClassCode[1] == 0x04 && PciConfigHeader.Hdr.ClassCode[2] == 0x06) {
      //
      // 060400h or 060401h indicates it's PCI-PCI bridge, get its bus number, device number and function number
      //
      PciIo->GetLocation (
              PciIo,
              &Segment,
              &Bus,
              &Device,
              &Function
              );
      if (PciConfigHeader.Hdr.Status == EFI_PCI_STATUS_CAPABILITY) {
        //
        // the bridge support Capability list and offset 0x34 is the pointer to the data structure
        //
        //
        // Detect if PCI Express Device
        //
        Status = LocateCapRegBlock (PciIo, EFI_PCI_CAPABILITY_ID_PCIEXP, &PciExpressOffset, NULL);
        if (Status == EFI_SUCCESS) {

// APTIOV_SERVER_OVERRIDE_RC_START : Do not disable ARI forwarding on PCI Express rev. 1.0 bridge.
          PciIo->Pci.Read (
                  PciIo,
                  EfiPciIoWidthUint16,
                  PciExpressOffset + 0x02,
                  1,
                  &PcieCap
                  );
          //
          // Check PCIe Capability Version
          // ARI Forwarding is not available in PCIe rev 1.0.
          //
          if ((PcieCap & 0x0F) < 2) continue;
// APTIOV_SERVER_OVERRIDE_RC_END : Do not disable ARI forwarding on PCI Express rev. 1.0 bridge.

          //
          // this bridge device is a PCI Express bridge, Check ARI forwarding bit in Device Control 2 register
          //
          PciIo->Pci.Read (
                  PciIo,
                  EfiPciIoWidthUint16,
                  PciExpressOffset + 0x28,
                  1,
                  &DevCon2
                  );
          if ((DevCon2 & BIT5) != 0) {
            //
            // ARI forwarding enable bit is set, we need to clear this bit before handing off control to OS
            // because OS may not ARI aware
            //
            DevCon2 = DevCon2 & (~BIT5);
            DEBUG((EFI_D_INFO, "ARI forwarding disable before booting OS,DevCon2 value is 0x%x\n", DevCon2));
            PciIo->Pci.Write (
                  PciIo,
                  EfiPciIoWidthUint16,
                  PciExpressOffset + 0x28,
                  1,
                  &DevCon2
                  );
          }
        }
      }
    }
  }
}

