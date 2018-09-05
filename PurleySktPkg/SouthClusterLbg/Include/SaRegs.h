/** @file
  Register names for System Agent (SA) registers
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  - In general, SA registers are denoted by "_SA_" in register names
  - Registers / bits that are different between SA generations are denoted by
    "_SA_[generation_name]_" in register/bit names. e.g., "_SA_HSW_"
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SA generation will be just named
    as "_SA_" without [generation_name] inserted.

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _SA_REGS_H_
#define _SA_REGS_H_

//
// Equates to convert Device IDs to Platform IDs
//
#define PLATFORM_ID_MOBILE    1
#define PLATFORM_ID_DESKTOP   0
#define PLATFORM_ID_UP_SERVER 2

//
// DEVICE 0 (Memory Controller Hub)
//
#define SA_MC_BUS          0x00
#define SA_MC_DEV          0x00
#define SA_MC_FUN          0x00
#define V_SA_MC_VID        0x8086
#define R_SA_MC_DEVICE_ID  0x02
#define R_SA_MC_CAPID0_B   0xE8

//
// Macros that judge which type a device ID belongs to
//

//
// CPU Mobile SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_SKL_MB_ULT_1 0x1904   ///< Skylake Ult (OPI) (2+1F/1.5F/2F/3/3E) Mobile SA DID
#define V_SA_DEVICE_ID_SKL_MB_ULX_2 0x190C   ///< Skylake Ulx (OPI) (2+1F/1.5F/2) SA DID
#define V_SA_DEVICE_ID_SKL_MB_ULX_3 0x1924   ///< Skylake Ulx (OPI)
//
// CPU Halo SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_SKL_HALO_1   0x1900   ///< Skylake Halo (2+2/1) SA DID
#define V_SA_DEVICE_ID_SKL_HALO_2   0x1910   ///< Skylake Halo (4+2/4E/3FE) SA DID
//
// CPU Desktop SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_SKL_DT_1     0x190F   ///< Skylake Desktop (2+1F/1.5F/2) SA DID
#define V_SA_DEVICE_ID_SKL_DT_2     0x191F   ///< Skylake Desktop (4+2/4) SA DID
//
// CPU Server SA Device IDs B0:D0:F0
//
#define V_SA_DEVICE_ID_SKL_SVR_1    0x1908   ///< Skylake Server (2+2/3E) SA DID
#define V_SA_DEVICE_ID_SKL_SVR_2    0x1918   ///< Skylake Server (4+1/2/4E) SA DID

///
/// Device IDs that are Mobile specific B0:D0:F0
///
#define IS_SA_DEVICE_ID_MOBILE(DeviceId) \
    ( \
      (DeviceId == V_SA_DEVICE_ID_SKL_MB_ULT_1) || \
      (DeviceId == V_SA_DEVICE_ID_SKL_MB_ULX_2) || \
      (DeviceId == V_SA_DEVICE_ID_SKL_MB_ULX_3) \
    )

///
/// Device IDs that are Desktop specific B0:D0:F0
///
#define IS_SA_DEVICE_ID_DESKTOP(DeviceId) \
    ( \
      (DeviceId == V_SA_DEVICE_ID_SKL_DT_1) || \
      (DeviceId == V_SA_DEVICE_ID_SKL_DT_2) \
    )

///
/// Device IDS that are Server specific B0:D0:F0
///
#define IS_SA_DEVICE_ID_SERVER(DeviceId) \
    ( \
      (DeviceId == V_SA_DEVICE_ID_SKL_SVR_1) || \
      (DeviceId == V_SA_DEVICE_ID_SKL_SVR_2) \
    )

///
/// Device IDs that are Halo specific B0:D0:F0
///
#define IS_SA_DEVICE_ID_HALO(DeviceId) \
    ( \
      (DeviceId == V_SA_DEVICE_ID_SKL_HALO_1) || \
      (DeviceId == V_SA_DEVICE_ID_SKL_HALO_2) \
    )

/**
 <b>Description</b>:
  This is the base address for the PCI Express Egress Port MMIO Configuration space.  There is no physical memory within this 4KB window that can be addressed.  The 4KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space.  On reset, the EGRESS port MMIO configuration space is disabled and must be enabled by writing a 1 to PXPEPBAREN [Dev 0, offset 40h, bit 0].
  All the bits in this register are locked in LT mode.
**/
#define R_SA_PXPEPBAR  (0x40)
//
// Description of PXPEPBAREN (0:0)
// - 0:  PXPEPBAR is disabled and does not claim any memory
// - 1:  PXPEPBAR memory mapped accesses are claimed and decoded appropriately
// - This register is locked by LT.
//
#define N_SA_PXPEPBAR_PXPEPBAREN_OFFSET  (0x0)
#define S_SA_PXPEPBAR_PXPEPBAREN_WIDTH   (0x1)
#define B_SA_PXPEPBAR_PXPEPBAREN_MASK    (0x1)
#define V_SA_PXPEPBAR_PXPEPBAREN_DEFAULT (0x0)
//
// Description of PXPEPBAR (12:38)
// - This field corresponds to bits 38 to 12 of the base address PCI Express Egress Port MMIO configuration space.  BIOS will program this register resulting in a base address for a 4KB block of contiguous memory address space.  This register ensures that a naturally aligned 4KB space is allocated within the first 512GB of addressable memory space.  System Software uses this base address to program the PCI Express Egress Port MMIO register set.  All the bits in this register are locked in LT mode.
//
#define N_SA_PXPEPBAR_PXPEPBAR_OFFSET  (0xc)
#define S_SA_PXPEPBAR_PXPEPBAR_WIDTH   (0x1b)
#define B_SA_PXPEPBAR_PXPEPBAR_MASK    (0x7ffffff000)
#define V_SA_PXPEPBAR_PXPEPBAR_DEFAULT (0x0)

/**
 <b>Description</b>:
 - This is the base address for the Host Memory Mapped Configuration space.  There is no physical memory within this 32KB window that can be addressed.  The 32KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space.  On reset, the Host MMIO Memory Mapped Configuation space is disabled and must be enabled by writing a 1 to MCHBAREN [Dev 0, offset48h, bit 0].
 - All the bits in this register are locked in LT mode.
 - The register space contains memory control, initialization, timing, and buffer strength registers; clocking registers; and power and thermal management registers.
**/
#define R_SA_MCHBAR  (0x48)
/**
 Description of MCHBAREN (0:0)
 - 0: MCHBAR is disabled and does not claim any memory
 - 1: MCHBAR memory mapped accesses are claimed and decoded appropriately
 - This register is locked by LT.
**/
#define N_SA_MCHBAR_MCHBAREN_OFFSET  (0x0)
#define S_SA_MCHBAR_MCHBAREN_WIDTH   (0x1)
#define B_SA_MCHBAR_MCHBAREN_MASK    (0x1)
#define V_SA_MCHBAR_MCHBAREN_DEFAULT (0x0)
/**
 Description of MCHBAR (15:38)
 - This field corresponds to bits 38 to 15 of the base address Host Memory Mapped configuration space.  BIOS will program this register resulting in a base address for a 32KB block of contiguous memory address space.  This register ensures that a naturally aligned 32KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the Host Memory Mapped register set. All the bits in this register are locked in LT mode.
**/
#define N_SA_MCHBAR_MCHBAR_OFFSET  (0xf)
#define S_SA_MCHBAR_MCHBAR_WIDTH   (0x18)
#define B_SA_MCHBAR_MCHBAR_MASK    (0x7fffff8000ULL)
#define V_SA_MCHBAR_MCHBAR_DEFAULT (0x0)

/**
 <b>Description</b>:
 - All the bits in this register are LT lockable.
**/
#define R_SA_GGC (0x50)
/**
 Description of GGCLCK (0:0)
 - When set to 1b, this bit will lock all bits in this register.
**/
#define N_SA_GGC_GGCLCK_OFFSET   (0x0)
#define S_SA_GGC_GGCLCK_WIDTH    (0x1)
#define B_SA_GGC_GGCLCK_MASK     (0x1)
#define V_SA_GGC_GGCLCK_DEFAULT  (0x0)
/**
 Description of IVD (1:1)
 - 0: Enable.  Device 2 (IGD) claims VGA memory and IO cycles, the Sub-Class Code within Device 2 Class Code register is 00.
 - 1: Disable.  Device 2 (IGD) does not claim VGA cycles (Mem and IO), and the Sub- Class Code field within Device 2 function 0 Class Code register is 80.
 - BIOS Requirement:  BIOS must not set this bit to 0 if the GMS field (bits 7:3 of this register) pre-allocates no memory.
 - This bit MUST be set to 1 if Device 2 is disabled either via a fuse or fuse override (CAPID0[46] = 1) or via a register (DEVEN[3] = 0).
 - This register is locked by LT lock.
**/
#define N_SA_GGC_IVD_OFFSET  (0x1)
#define S_SA_GGC_IVD_WIDTH   (0x1)
#define B_SA_GGC_IVD_MASK    (0x2)
#define V_SA_GGC_IVD_DEFAULT (0x0)
#define V_SA_GGC_GMS_DIS     0
#define V_SA_GGC_GMS_32MB    1
#define V_SA_GGC_GMS_64MB    2
/**
 For SKL
 Description of GMS (8:15)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics device in VGA (non-linear) and Native (linear) modes.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - This register is also LT lockable.
 - Valid options are 0 (0x0) to 2048MB (0x40) in multiples of 32 MB
 - Default is 64MB
 - All other values are reserved
 - Hardware does not clear or set any of these bits automatically based on IGD being disabled/enabled.
 - BIOS Requirement: BIOS must not set this field to 0h if IVD (bit 1 of this register) is 0.
**/
#define N_SKL_SA_GGC_GMS_OFFSET  (0x8)
#define S_SKL_SA_GGC_GMS_WIDTH   (0x8)
#define B_SKL_SA_GGC_GMS_MASK    (0xff00)
#define V_SKL_SA_GGC_GMS_DEFAULT (0x01)

/**
 For SKL
 Description of GGMS (6:7)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics Translation Table.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - GSM is assumed to be a contiguous physical DRAM space with DSM, and BIOS needs to allocate a contiguous memory chunk.  Hardware will derive the base of GSM from DSM only using the GSM size programmed in the register.
 - Valid options:
 - 0h: 0 MB of memory pre-allocated for GTT.
 - 1h: 2 MB of memory pre-allocated for GTT.
 - 2h: 4 MB of memory pre-allocated for GTT. (default)
 - 3h: 8 MB of memory pre-allocated for GTT.
 - Others: Reserved
 - Hardware functionality in case of programming this value to Reserved is not guaranteed.
**/
#define N_SKL_SA_GGC_GGMS_OFFSET  (0x6)
#define S_SKL_SA_GGC_GGMS_WIDTH   (0x2)
#define B_SKL_SA_GGC_GGMS_MASK    (0xc0)
#define V_SKL_SA_GGC_GGMS_DEFAULT (2)
#define V_SKL_SA_GGC_GGMS_DIS     0
#define V_SKL_SA_GGC_GGMS_2MB     1
#define V_SKL_SA_GGC_GGMS_4MB     2
#define V_SKL_SA_GGC_GGMS_8MB     3

/**
 Description of VAMEN (14:14)
 - Enables the use of the iGFX enbines for Versatile Acceleration.
 - 1 - iGFX engines are in Versatile Acceleration Mode.  Device 2 Class Code is 048000h.
 - 0 - iGFX engines are in iGFX Mode.  Device 2 Class Code is 030000h.
**/
#define N_SA_GGC_VAMEN_OFFSET  (0xe)
#define S_SA_GGC_VAMEN_WIDTH   (0x1)
#define B_SA_GGC_VAMEN_MASK    (0x4000)
#define V_SA_GGC_VAMEN_DEFAULT (0x0)

/**
 Description:
 - Allows for enabling/disabling of PCI devices and functions that are within the CPU package. The table below the bit definitions describes the behavior of all combinations of transactions to devices controlled by this register.
  All the bits in this register are LT Lockable.
**/
#define R_SA_DEVEN (0x54)
/**
 Description of D0EN (0:0)
 - Bus 0 Device 0 Function 0 may not be disabled and is therefore hardwired to 1.
**/
#define N_SA_DEVEN_D0EN_OFFSET   (0x0)
#define S_SA_DEVEN_D0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D0EN_MASK     (0x1)
#define V_SA_DEVEN_D0EN_DEFAULT  (0x1)
/**
 Description of D2EN (4:4)
 - 0:  Bus 0 Device 2 is disabled and hidden
 - 1:  Bus 0 Device 2 is enabled and visible
 - This bit will remain 0 if Device 2 capability is disabled.
**/
#define N_SA_DEVEN_D2EN_OFFSET   (0x4)
#define S_SA_DEVEN_D2EN_WIDTH    (0x1)
#define B_SA_DEVEN_D2EN_MASK     (0x10)
#define V_SA_DEVEN_D2EN_DEFAULT  (0x10)
/**
 Description of D3EN (5:5)
 - 0:  Bus 0 Device 3 is disabled and hidden
 - 1:  Bus 0 Device 3 is enabled and visible
 - This bit will remain 0 if Device 3 capability is disabled.
**/
#define N_SA_DEVEN_D3EN_OFFSET   (0x5)
#define S_SA_DEVEN_D3EN_WIDTH    (0x1)
#define B_SA_DEVEN_D3EN_MASK     (0x20)
#define V_SA_DEVEN_D3EN_DEFAULT  (0x20)
/**
 Description of D4EN (7:7)
 - 0: Bus 0 Device 4 is disabled and not visible.
 - 1: Bus 0 Device 4 is enabled and visible.
 - This bit will remain 0 if Device 4 capability is disabled.
**/
#define N_SA_DEVEN_D4EN_OFFSET   (0x7)
#define S_SA_DEVEN_D4EN_WIDTH    (0x1)
#define B_SA_DEVEN_D4EN_MASK     (0x80)
#define V_SA_DEVEN_D4EN_DEFAULT  (0x80)
/**
 Description of D7EN (14:14)
 - 0: Bus 0 Device 7 is disabled and not visible.
 - 1: Bus 0 Device 7 is enabled and visible.
 - Non-production BIOS code should provide a setup option to enable Bus 0 Device 7.  When enabled, Bus 0 Device 7 must be initialized in accordance to standard PCI device initialization procedures.
**/
#define N_SA_DEVEN_D7EN_OFFSET   (0xe)
#define S_SA_DEVEN_D7EN_WIDTH    (0x1)
#define B_SA_DEVEN_D7EN_MASK     (0x4000)
#define V_SA_DEVEN_D7EN_DEFAULT  (0x0)
/**
 Description of EPBAREN (27:27)
 - 0:  EPBAR is disabled and does not claim any memory.
 - 1:  EPBAR memory mapped accesses are claimed and decoded approprately. This bit is Intel Reserved
**/
#define N_SA_DEVEN_EPBAREN_OFFSET  (0x1b)
#define S_SA_DEVEN_EPBAREN_WIDTH   (0x1)
#define B_SA_DEVEN_EPBAREN_MASK    (0x8000000)
#define V_SA_DEVEN_EPBAREN_DEFAULT (0x0)
/**
 Description of MCHBAREN (28:28)
 - 0:  MCHBAR is disabled and does not claim any memory.
 - 1:  MCHBAR memory mapped accesses are claimed and decoded approprately. This bit is Intel Reserved
**/
#define N_SA_DEVEN_MCHBAREN_OFFSET   (0x1c)
#define S_SA_DEVEN_MCHBAREN_WIDTH    (0x1)
#define B_SA_DEVEN_MCHBAREN_MASK     (0x10000000)
#define V_SA_DEVEN_MCHBAREN_DEFAULT  (0x0)
/**
 Description of DMIBAREN (29:29)
 - 0:  DMIBAR is disabled and does not claim any memory.
 - 1:  DMIBAR memory mapped accesses are claimed and decoded approprately. This bit is Intel Reserved
**/
#define N_SA_DEVEN_DMIBAREN_OFFSET   (0x1d)
#define S_SA_DEVEN_DMIBAREN_WIDTH    (0x1)
#define B_SA_DEVEN_DMIBAREN_MASK     (0x20000000)
#define V_SA_DEVEN_DMIBAREN_DEFAULT  (0x0)
/**
 Description of PCIEXBAREN (31:31)
 - 0:  The PCIEXBAR register is disabled. Memory read and write transactions proceed as if there were no PCIEXBAR register. PCIEXBAR bits 31:28 are R/W with no functionality behind them.
 - 1:  The PCIEXBAR register is enabled. Memory read and write transactions whose address bits 31:28 match PCIEXBAR 31:28 will be translated to configuration reads and writes within the GMCH. These translated cycles are routed as shown in the table above.
**/
#define N_SA_DEVEN_PCIEXBAREN_OFFSET   (0x1f)
#define S_SA_DEVEN_PCIEXBAREN_WIDTH    (0x1)
#define B_SA_DEVEN_PCIEXBAREN_MASK     (0x80000000)
#define V_SA_DEVEN_PCIEXBAREN_DEFAULT  (0x0)
/**
 Description of RSVD (31:31)
**/
#define N_SA_DEVEN_RSVD_OFFSET   (0x1f)
#define S_SA_DEVEN_RSVD_WIDTH    (0x1)
#define B_SA_DEVEN_RSVD_MASK     (0x80000000)
#define V_SA_DEVEN_RSVD_DEFAULT  (0x0)

/**
 Description
 - Protected Audio Video Path Control
 - All the bits in this register are locked by LT.  When locked the R/W bits are RO.
**/
#define R_SA_PAVPC (0x58)
/**
 Description of PCME (0:0)
 - This field enables Protected Content Memory within Graphics Stolen Memory.
 - This register is locked (becomes read-only) when PAVPLCK = 1b.
 - This register is read-only (stays at 0b) when PAVP fuse is set to "disabled"
 - 0: Protected Content Memory is disabled
 - 1: Protected Content Memory is enabled
**/
#define N_SA_PAVPC_PCME_OFFSET   (0x0)
#define S_SA_PAVPC_PCME_WIDTH    (0x1)
#define B_SA_PAVPC_PCME_MASK     (0x1)
#define V_SA_PAVPC_PCME_MASK     (0x0)
/**
 Description of PAVPE (1:1)
 - 0: PAVP path is disabled
 - 1: PAVP path is enabled
 - This register is locked (becomes read-only) when PAVPLCK = 1b
 - This register is read-only (stays at 0b) when PAVP capability is set to "disabled" as defined by CAPID0_B[PAVPE].
**/
#define N_SA_PAVPC_PAVPE_OFFSET  (0x1)
#define S_SA_PAVPC_PAVPE_WIDTH   (0x1)
#define B_SA_PAVPC_PAVPE_MASK    (0x2)
#define V_SA_PAVPC_PAVPE_DEFAULT (0x0)
/**
 Description of PAVPLCK (2:2)
 - This bit will lock all writeable contents in this register when set (including itself).
 - This bit will be locked if PAVP is fused off.
**/
#define N_SA_PAVPC_PAVPLCK_OFFSET  (0x2)
#define S_SA_PAVPC_PAVPLCK_WIDTH   (0x1)
#define B_SA_PAVPC_PAVPLCK_MASK    (0x4)
#define V_SA_PAVPC_PAVPLCK_DEFAULT (0x0)
/**
 Description of HVYMODSEL (3:3)
 - 0b   Lite Mode
**/
#define N_SA_PAVPC_HVYMODSEL_OFFSET  (0x3)
#define S_SA_PAVPC_HVYMODSEL_WIDTH   (0x1)
#define B_SA_PAVPC_HVYMODSEL_MASK    (0x8)
#define V_SA_PAVPC_HVYMODSEL_DEFAULT (0x0)
/**
 Description of WOPCMSZ (8:7)
 - 00b - 1MB (default)
**/
#define N_SA_PAVPC_WOPCMSZ_OFFSET  (0x7)
#define S_SA_PAVPC_WOPCMSZ_WIDTH   (0x2)
#define B_SA_PAVPC_WOPCMSZ_MASK    (0x180)
#define V_SA_PAVPC_WOPCMSZ_DEFAULT (0x0)
/**
 Description of PCMBASE (20:31)
 - This field is used to set the base of Protected Content Memory.
 - This corresponds to bits 31:20 of the system memory address range, giving a 1MB granularity. This value MUST be at least 1MB above the base and below the top of stolen memory.
 - This register is locked (becomes read-only) when PAVPE = 1b.
**/
#define N_SA_PAVPC_PCMBASE_OFFSET  (0x14)
#define S_SA_PAVPC_PCMBASE_WIDTH   (0xc)
#define B_SA_PAVPC_PCMBASE_MASK    (0xfff00000)
#define V_SA_PAVPC_PCMBASE_DEFAULT (0x0)

#define R_SA_DPR (0x5c) ///< DMA protected range register
/**
 Description of LOCK (0:0)
 - This bit will lock all writeable settings in this register, including itself.
**/
#define N_SA_DPR_LOCK_OFFSET   (0x0)
#define S_SA_DPR_LOCK_WIDTH    (0x1)
#define B_SA_DPR_LOCK_MASK     (0x1)
#define V_SA_DPR_LOCK_DEFAULT  (0x0)
/**
 Description of PRS (1:1)
 - This field indicates the status of DPR.
 - 0: DPR protection disabled
 - 1: DPR protection enabled
**/
#define N_SA_DPR_PRS_OFFSET   (0x1)
#define S_SA_DPR_PRS_WIDTH    (0x1)
#define B_SA_DPR_PRS_MASK     (0x2)
#define V_SA_DPR_PRS_DEFAULT  (0x0)
/**
 Description of EPM (2:2)
 - This field controls DMA accesses to the DMA Protected Range (DPR) region.
 - 0: DPR is disabled
 - 1: DPR is enabled.  All DMA requests accessing DPR region are blocked.
 - HW reports the status of DPR enable/disable through the PRS field in this register.
**/
#define N_SA_DPR_EPM_OFFSET  (0x2)
#define S_SA_DPR_EPM_WIDTH   (0x1)
#define B_SA_DPR_EPM_MASK    (0x4)
#define V_SA_DPR_EPM_DEFAULT (0x0)
/**
 Description of DPRSIZE (11:4)
 - This field is used to specify the size of memory protected from DMA access in MB
 - The maximum amount of memory that will be protected is 255MB
 - The Top of protected range is the base of TSEG-1
**/
#define N_DPR_DPRSIZE_OFFSET  (0x4)
#define V_DPR_DPRSIZE_WIDTH   (0x8)
#define V_DPR_DPRSIZE_MASK    (0xFF0)
#define V_DPR_DPRSIZE_DEFAULT (0x0)
/**
 Description of TOPOFDPR (31:20)
 - This is the Top address 1 of DPR - Base of TSEG
**/
#define N_SA_DPR_TOPOFDPR_OFFSET  (20)
#define S_SA_DPR_TOPOFDPR_WIDTH   (0xC)
#define B_SA_DPR_TOPOFDPR_MASK    (0xFFF00000)
#define V_SA_DPR_TOPOFDPR_DEFAULT (0x0)

/**
 Description:
  This is the base address for the PCI Express configuration space.  This window of addresses contains the 4KB of configuration space for each PCI Express device that can potentially be part of the PCI Express Hierarchy associated with the Uncore.  There is no actual physical memory within this window of up to 256MB that can be addressed.  The actual size of this range is determined by a field in this register.
  Each PCI Express Hierarchy requires a PCI Express BASE register.  The Uncore supports one PCI Express Hierarchy.  The region reserved by this register does not alias to any PCI2.3 compliant memory mapped space.  For example, the range reserved for MCHBAR is outside of PCIEXBAR space.
  On reset, this register is disabled and must be enabled by writing a 1 to the enable field in this register.  This base address shall be assigned on a boundary consistent with the number of buses (defined by the length field in this register), above TOLUD and still within 39-bit addressable memory space.
  The PCI Express Base Address cannot be less than the maximum address written to the Top of physical memory register (TOLUD).  Software must guarantee that these ranges do not overlap with known ranges located above TOLUD.
  Software must ensure that the sum of the length of the enhanced configuration region + TOLUD + any other known ranges reserved above TOLUD is not greater than the 39-bit addessable limit of 512GB.  In general, system implementation and the number of PCI/PCI Express/PCI-X buses supported in the hierarchy will dictate the length of the region.
  All the bits in this register are locked in LT mode.
**/
#define R_SA_PCIEXBAR  (0x60)
/**
 Description of PCIEXBAREN (0:0)
 - 0: The PCIEXBAR register is disabled.  Memory read and write transactions proceed s if there were no PCIEXBAR register.  PCIEXBAR bits 38:26 are R/W with no functionality behind them.
 - 1: The PCIEXBAR register is enabled.  Memory read and write transactions whose address bits 38:26 match PCIEXBAR will be translated to configuration reads and writes within the Uncore.  These Translated cycles are routed as shown in the above table.
 - This register is locked by LT.
**/
#define N_SA_PCIEXBAR_PCIEXBAREN_OFFSET  (0x0)
#define S_SA_PCIEXBAR_PCIEXBAREN_WIDTH   (0x1)
#define B_SA_PCIEXBAR_PCIEXBAREN_MASK    (0x1)
#define V_SA_PCIEXBAR_PCIEXBAREN_DEFAULT (0x0)
/**
 Description of LENGTH (1:2)
 - This field describes the length of this region.
 - 00: 256MB (buses 0-255).  Bits 38:28 are decoded in the PCI Express Base Address Field.
 - 01: 128MB (buses 0-127).  Bits 38:27 are decoded in the PCI Express Base Address Field.
 - 10: 64MB (buses 0-63).  Bits 38:26 are decoded in the PCI Express Base Address Field.
 - 11: Reserved.
 - This register is locked by LT.
**/
#define N_SA_PCIEXBAR_LENGTH_OFFSET  (0x1)
#define S_SA_PCIEXBAR_LENGTH_WIDTH   (0x2)
#define B_SA_PCIEXBAR_LENGTH_MASK    (0x6)
#define V_SA_PCIEXBAR_LENGTH_DEFAULT (0x0)
/**
 Description of ADMSK64 (26:26)
  This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/
#define N_SA_PCIEXBAR_ADMSK64_OFFSET   (0x1a)
#define S_SA_PCIEXBAR_ADMSK64_WIDTH    (0x1)
#define B_SA_PCIEXBAR_ADMSK64_MASK     (0x4000000)
#define V_SA_PCIEXBAR_ADMSK64_DEFAULT  (0x0)
/**
 Description of ADMSK128 (27:27)
  This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/
#define N_SA_PCIEXBAR_ADMSK128_OFFSET  (0x1b)
#define S_SA_PCIEXBAR_ADMSK128_WIDTH   (0x1)
#define B_SA_PCIEXBAR_ADMSK128_MASK    (0x8000000)
#define V_SA_PCIEXBAR_ADMSK128_DEFAULT (0x0)
/**
 Description of PCIEXBAR (28:38)
 - This field corresponds to bits 38 to 28 of the base address for PCI Express enhanced configuration space.  BIOS will program this register resulting in a base address for a contiguous memory address space.  The size of the range is defined by bits [2:1] of this register.
 - This Base address shall be assigned on a boundary consistent with the number of buses (defined by the Length field in this register) above TOLUD and still within the 39-bit addressable memory space.  The address bits decoded depend on the length of the region defined by this register.
 - This register is locked by LT.
 - The address used to access the PCI Express configuration space for a specific device can be determined as follows:
 - PCI Express Base Address + Bus Number * 1MB + Device Number * 32KB + Function Number * 4KB
 - This address is the beginning of the 4KB space that contains both the PCI compatible configuration space and the PCI Express extended configuration space.
**/
#define N_SA_PCIEXBAR_PCIEXBAR_OFFSET  (0x1c)
#define S_SA_PCIEXBAR_PCIEXBAR_WIDTH   (0xb)
#define B_SA_PCIEXBAR_PCIEXBAR_MASK    (0x7ff0000000)
#define V_SA_PCIEXBAR_PCIEXBAR_DEFAULT (0x0)

/**
 This is the base address for the Root Complex configuration space. This window of addresses contains the Root Complex Register set for the PCI Express Hierarchy associated with the Host Bridge. There is no physical memory within this 4KB window that can be addressed. The 4KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space. On reset, the Root Complex configuration space is disabled and must be enabled by writing a 1 to DMIBAREN [Dev 0, offset 68h, bit 0] All the bits in this register are locked in LT mode.
**/
#define R_SA_DMIBAR  (0x68)
/**
 Description of DMIBAREN (0:0)
 - 0: DMIBAR is disabled and does not claim any memory
 - 1: DMIBAR memory mapped accesses are claimed and decoded appropriately
 - This register is locked by LT.
**/
#define N_SA_DMIBAR_DMIBAREN_OFFSET  (0x0)
#define S_SA_DMIBAR_DMIBAREN_WIDTH   (0x1)
#define B_SA_DMIBAR_DMIBAREN_MASK    (0x1)
#define V_SA_DMIBAR_DMIBAREN_DEFAULT (0x0)
/**
 Description of DMIBAR (12:38)
 - This field corresponds to bits 38 to 12 of the base address DMI configuration space. BIOS will program this register resulting in a base address for a 4KB block of contiguous memory address space. This register ensures that a naturally aligned 4KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the DMI register set. All the Bits in this register are locked in LT mode.
**/
#define N_SA_DMIBAR_DMIBAR_OFFSET  (0xc)
#define S_SA_DMIBAR_DMIBAR_WIDTH   (0x1b)
#define B_SA_DMIBAR_DMIBAR_MASK    (0x7ffffff000)
#define V_SA_DMIBAR_DMIBAR_DEFAULT (0x0)

/**
 Description:
 - This register determines the Base Address register of the memory range that is pre-allocated to the Manageability Engine.  Together with the MESEG_MASK register it controls the amount of memory allocated to the ME.
 - This register must be initialized by the configuration software.  For the purpose of address decode address bits A[19:0] are assumed to be 0. Thus, the bottom of the defined memory address range will be aligned to a 1MB boundary.
 - This register is locked by LT.
**/
#define R_SA_MESEG_BASE  (0x70)
/**
 Description of MEBASE (20:38)
 - Corresponds to A[38:20] of the base address memory range that is allocated to the ME.
**/
#define N_SA_MESEG_BASE_MEBASE_OFFSET  (0x14)
#define S_SA_MESEG_BASE_MEBASE_WIDTH   (0x13)
#define B_SA_MESEG_BASE_MEBASE_MASK    (0x7ffff00000)
#define V_SA_MESEG_BASE_MEBASE_DEFAULT (0xfffff00000)

/**
 Description:
 - This register determines the Mask Address register of the memory range that is pre-allocated to the Manageability Engine.  Together with the MESEG_BASE register it controls the amount of memory allocated to the ME.
 - This register is locked by LT.
**/
#define R_SA_MESEG_MASK  (0x78)
/**
 Description of MELCK (10:10)
 - This field indicates whether all bits in the MESEG_BASE and MESEG_MASK registers are locked.  When locked, updates to any field for these registers must be dropped.
**/
#define N_SA_MESEG_MASK_MELCK_OFFSET   (0xa)
#define S_SA_MESEG_MASK_MELCK_WIDTH    (0x1)
#define B_SA_MESEG_MASK_MELCK_MASK     (0x400)
#define V_SA_MESEG_MASK_MELCK_DEFAULT  (0x0)
/**
 Description of ME_STLEN_EN (11:11)
 - Indicates whether the ME stolen  Memory range is enabled or not.
**/
#define N_SA_MESEG_MASK_ME_STLEN_EN_OFFSET   (0xb)
#define S_SA_MESEG_MASK_ME_STLEN_EN_WIDTH    (0x1)
#define B_SA_MESEG_MASK_ME_STLEN_EN_MASK     (0x800)
#define V_SA_MESEG_MASK_ME_STLEN_EN_DEFAULT  (0x0)
/**
 Description of MEMASK (20:38)
 - This field indicates the bits that must match MEBASE in order to qualify as an ME Memory Range access.
 - For example, if the field is set to 7FFFFh, then ME Memory is 1MB in size.
 - Another example is that if the field is set to 7FFFEh, then ME Memory is 2MB in size.
 - In other words, the size of ME Memory Range is limited to power of 2 times 1MB.
**/
#define N_SA_MESEG_MASK_MEMASK_OFFSET  (0x14)
#define S_SA_MESEG_MASK_MEMASK_WIDTH   (0x13)
#define B_SA_MESEG_MASK_MEMASK_MASK    (0x7ffff00000)
#define V_SA_MESEG_MASK_MEMASK_DEFAULT (0x0)

/**
 Description:
 - This register controls the read, write and shadowing attributes of the BIOS range from F_0000h to F_FFFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
 - Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
 - RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
 - WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
 - The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
**/
#define R_SA_PAM0  (0x80)
///
/// Description of HIENABLE (4:5)
///  This field controls the steering of read and write cycles that address the BIOS area from 0F_0000h to 0F_FFFFh.
///  00: DRAM Disabled.  All accesses are directed to DMI.
///  01: Read Only.  All reads are sent to DRAM, all writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM, all reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM0_HIENABLE_OFFSET  (0x4)
#define S_SA_PAM0_HIENABLE_WIDTH   (0x2)
#define B_SA_PAM0_HIENABLE_MASK    (0x30)
#define V_SA_PAM0_HIENABLE_DEFAULT (0x0)

///
/// Description:
///  This register controls the read, write and shadowing attributes of the BIOS range from C_0000h to C_7FFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
///  Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
///  RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
///  WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
///  The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
///
#define R_SA_PAM1  (0x81)
///
/// Description of LOENABLE (0:1)
///  This field controls the steering of read and write cycles that address the BIOS area from 0C0000h to 0C3FFFh.
///  00: DRAM Disabled.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  01: Read Only.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM.  All reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM1_LOENABLE_OFFSET  (0x0)
#define S_SA_PAM1_LOENABLE_WIDTH   (0x2)
#define B_SA_PAM1_LOENABLE_MASK    (0x3)
#define V_SA_PAM1_LOENABLE_DEFAULT (0x0)
///
/// Description of HIENABLE (4:5)
///  This field controls the steering of read and write cycles that address the BIOS area from 0C_4000h to 0C_7FFFh.
///  00: DRAM Disabled.  All accesses are directed to DMI.
///  01: Read Only.  All reads are sent to DRAM, all writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM, all reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM1_HIENABLE_OFFSET  (0x4)
#define S_SA_PAM1_HIENABLE_WIDTH   (0x2)
#define B_SA_PAM1_HIENABLE_MASK    (0x30)
#define V_SA_PAM1_HIENABLE_DEFAULT (0x0)

///
/// Description:
///  This register controls the read, write and shadowing attributes of the BIOS range from C_8000h to C_FFFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
///  Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
///  RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
///  WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
///  The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
///
#define R_SA_PAM2  (0x82)
///
/// Description of LOENABLE (0:1)
///  This field controls the steering of read and write cycles that address the BIOS area from 0C8000h to 0CBFFFh.
///  00: DRAM Disabled.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  01: Read Only.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM.  All reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM2_LOENABLE_OFFSET  (0x0)
#define S_SA_PAM2_LOENABLE_WIDTH   (0x2)
#define B_SA_PAM2_LOENABLE_MASK    (0x3)
#define V_SA_PAM2_LOENABLE_DEFAULT (0x0)
///
/// Description of HIENABLE (4:5)
///  This field controls the steering of read and write cycles that address the BIOS area from 0CC000h to 0CFFFFh.
///  00: DRAM Disabled.  All accesses are directed to DMI.
///  01: Read Only.  All reads are sent to DRAM, all writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM, all reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM2_HIENABLE_OFFSET  (0x4)
#define S_SA_PAM2_HIENABLE_WIDTH   (0x2)
#define B_SA_PAM2_HIENABLE_MASK    (0x30)
#define V_SA_PAM2_HIENABLE_DEFAULT (0x0)

///
/// Description:
///  This register controls the read, write and shadowing attributes of the BIOS range from D0000h to D7FFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
///  Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
///  RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
///  WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
///  The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
///
#define R_SA_PAM3  (0x83)
///
/// Description of LOENABLE (0:1)
///  This field controls the steering of read and write cycles that address the BIOS area from 0D0000h to 0D3FFFh.
///  00: DRAM Disabled.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  01: Read Only.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM.  All reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM3_LOENABLE_OFFSET  (0x0)
#define S_SA_PAM3_LOENABLE_WIDTH   (0x2)
#define B_SA_PAM3_LOENABLE_MASK    (0x3)
#define V_SA_PAM3_LOENABLE_DEFAULT (0x0)
///
/// Description of HIENABLE (4:5)
///  This field controls the steering of read and write cycles that address the BIOS area from 0D4000h to 0D7FFFh.
///  00: DRAM Disabled.  All accesses are directed to DMI.
///  01: Read Only.  All reads are sent to DRAM, all writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM, all reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM3_HIENABLE_OFFSET  (0x4)
#define S_SA_PAM3_HIENABLE_WIDTH   (0x2)
#define B_SA_PAM3_HIENABLE_MASK    (0x30)
#define V_SA_PAM3_HIENABLE_DEFAULT (0x0)

///
/// Description:
///  This register controls the read, write and shadowing attributes of the BIOS range from D8000h to DFFFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
///  Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
///  RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
///  WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
///  The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
///
#define R_SA_PAM4  (0x84)
///
/// Description of LOENABLE (0:1)
///  This field controls the steering of read and write cycles that address the BIOS area from 0D8000h to 0DBFFFh.
///  00: DRAM Disabled.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  01: Read Only.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM.  All reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM4_LOENABLE_OFFSET  (0x0)
#define S_SA_PAM4_LOENABLE_WIDTH   (0x2)
#define B_SA_PAM4_LOENABLE_MASK    (0x3)
#define V_SA_PAM4_LOENABLE_DEFAULT (0x0)
///
/// Description of HIENABLE (4:5)
///  This field controls the steering of read and write cycles that address the BIOS area from 0DC000h to 0DFFFFh.
///  00: DRAM Disabled.  All accesses are directed to DMI.
///  01: Read Only.  All reads are sent to DRAM, all writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM, all reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM4_HIENABLE_OFFSET  (0x4)
#define S_SA_PAM4_HIENABLE_WIDTH   (0x2)
#define B_SA_PAM4_HIENABLE_MASK    (0x30)
#define V_SA_PAM4_HIENABLE_DEFAULT (0x0)

///
/// Description:
///  This register controls the read, write and shadowing attributes of the BIOS range from E_0000h to E_7FFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
///  Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
///  RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
///  WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
///  The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
///
#define R_SA_PAM5  (0x85)
///
/// Description of LOENABLE (0:1)
///  This field controls the steering of read and write cycles that address the BIOS area from 0E0000h to 0E3FFFh.
///  00: DRAM Disabled.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  01: Read Only.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM.  All reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM5_LOENABLE_OFFSET  (0x0)
#define S_SA_PAM5_LOENABLE_WIDTH   (0x2)
#define B_SA_PAM5_LOENABLE_MASK    (0x3)
#define V_SA_PAM5_LOENABLE_DEFAULT (0x0)
///
/// Description of HIENABLE (4:5)
///  This field controls the steering of read and write cycles that address the BIOS area from 0E4000h to 0E7FFFh.
///  00: DRAM Disabled.  All accesses are directed to DMI.
///  01: Read Only.  All reads are sent to DRAM, all writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM, all reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM5_HIENABLE_OFFSET  (0x4)
#define S_SA_PAM5_HIENABLE_WIDTH   (0x2)
#define B_SA_PAM5_HIENABLE_MASK    (0x30)
#define V_SA_PAM5_HIENABLE_DEFAULT (0x0)

///
/// Description:
///  This register controls the read, write and shadowing attributes of the BIOS range from E_8000h to E_FFFFh.  The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range.  Seven Programmable Attribute Map (PAM) registers are used to support these features.  Cacheability of these areas is controlled via the MTRR register in the core.
///  Two bits are used to specify memory attributes for each memory segment.  These bits apply to host accesses to the PAM areas.  These attributes are:
///  RE - Read Enable.  When RE=1, the host read accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when RE=0, the host read accesses are directed to DMI.
///  WE - Write Enable.  When WE=1, the host write accesses to the corresponding memory segment are claimed by the Uncore and directed to main memory.  Conversely, when WE=0, the host read accesses are directed to DMI.
///  The RE and WE attributes permit a memory segment to be Read Only, Write Only, Read/Write or Disabled.  For example, if a memory segment has RE=1 and WE=0, the segment is Read Only.
///
#define R_SA_PAM6  (0x86)
///
/// Description of LOENABLE (0:1)
///  This field controls the steering of read and write cycles that address the BIOS area from 0E8000h to 0EBFFFh.
///  00: DRAM Disabled.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  01: Read Only.  All reads are sent to DRAM.  All writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM.  All reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM6_LOENABLE_OFFSET  (0x0)
#define S_SA_PAM6_LOENABLE_WIDTH   (0x2)
#define B_SA_PAM6_LOENABLE_MASK    (0x3)
#define V_SA_PAM6_LOENABLE_DEFAULT (0x0)
///
/// Description of HIENABLE (4:5)
///  This field controls the steering of read and write cycles that address the BIOS area from 0EC000h to 0EFFFFh.
///  00: DRAM Disabled.  All accesses are directed to DMI.
///  01: Read Only.  All reads are sent to DRAM, all writes are forwarded to DMI.
///  10: Write Only.  All writes are sent to DRAM, all reads are serviced by DMI.
///  11: Normal DRAM Operation.  All reads and writes are serviced by DRAM.
///  This register is locked by LT.
///
#define N_SA_PAM6_HIENABLE_OFFSET  (0x4)
#define S_SA_PAM6_HIENABLE_WIDTH   (0x2)
#define B_SA_PAM6_HIENABLE_MASK    (0x30)
#define V_SA_PAM6_HIENABLE_DEFAULT (0x0)

#define R_SA_MC_LAC_OFFSET         0x87  ///< Legacy Access Control Register
#define R_SA_B_HEN                 0x80  ///< RW
///
/// Description:
///  The SMRAMC register controls how accesses to Compatible SMRAM spaces are treated.  The Open, Close and Lock bits function only when G_SMRAME bit is set to 1.  Also, the Open bit must be reset before the Lock bit is set.
///
#define R_SA_SMRAMC  (0x88)
///
/// Description of C_BASE_SEG (0:2)
///  This field indicates the location of SMM space.  SMM DRAM is not remapped.  It is simply made visible if the conditions are right to access SMM space, otherwise the access is forwarded to DMI.  Only SMM space bewteen A_0000h and B_FFFFh is supported, so this field is hardwired to 010b.
///
#define N_SA_SMRAMC_C_BASE_SEG_OFFSET  (0x0)
#define S_SA_SMRAMC_C_BASE_SEG_WIDTH   (0x3)
#define B_SA_SMRAMC_C_BASE_SEG_MASK    (0x7)
#define V_SA_SMRAMC_C_BASE_SEG_DEFAULT (0x2)
///
/// Description of G_SMRAME (3:3)
///  If set to '1', then Compatible SMRAM functions are enabled, providing 128KB of DRAM accessible at the A_0000h address while in SMM.  To enable Extended SMRAM function this bit has to be set to 1.  Once D_LCK is set, this bit becomes RO.
///
#define N_SA_SMRAMC_G_SMRAME_OFFSET  (0x3)
#define S_SA_SMRAMC_G_SMRAME_WIDTH   (0x1)
#define B_SA_SMRAMC_G_SMRAME_MASK    (0x8)
#define V_SA_SMRAMC_G_SMRAME_DEFAULT (0x0)
///
/// Description of D_LCK (4:4)
///  When D_LCK=1, then D_OPEN is reset to 0 and all writeable fields in this register are locked (become RO).  D_LCK can be set to 1 via a normal configuration space write but can only be cleared by a Full Reset.
///  The combination of D_LCK and D_OPEN provide convenience with security.  The BIOS can use the D_OPEN function to initialize SMM space and then use D_LCK to "lock down" SMM space in the future so that no application software (or even BIOS itself) can violate the integrity of SMM space, even if the program has knowledge of the D_OPEN function.
///
#define N_SA_SMRAMC_D_LCK_OFFSET   (0x4)
#define S_SA_SMRAMC_D_LCK_WIDTH    (0x1)
#define B_SA_SMRAMC_D_LCK_MASK     (0x10)
#define V_SA_SMRAMC_D_LCK_DEFAULT  (0x0)
///
/// Description of D_CLS (5:5)
///  When D_CLS = 1, SMM DRAM space is not accessible to data references, even if SMM decode is active.  Code references may still access SMM DRAM space.  This will allow SMM software to reference through SMM space to update the display even when SMM is mapped over the VGA range.  Software should ensure that D_OPEN = 1 and D_CLS = 1 are not set at the same time.
///
#define N_SA_SMRAMC_D_CLS_OFFSET   (0x5)
#define S_SA_SMRAMC_D_CLS_WIDTH    (0x1)
#define B_SA_SMRAMC_D_CLS_MASK     (0x20)
#define V_SA_SMRAMC_D_CLS_DEFAULT  (0x0)
///
/// Description of D_OPEN (6:6)
///  When D_OPEN = 1 and D_LCK = 0, the SMM DRAM space is made visible even when SMM decode is not active.  This is intended to help BIOS initialize SMM space.  Software should ensure that D_OPEN = 1 and D_CLS = 1 are not set at the same time.
///
#define N_SA_SMRAMC_D_OPEN_OFFSET  (0x6)
#define S_SA_SMRAMC_D_OPEN_WIDTH   (0x1)
#define B_SA_SMRAMC_D_OPEN_MASK    (0x40)
#define V_SA_SMRAMC_D_OPEN_DEFAULT (0x0)

///
/// Description:
///
#define R_SA_REMAPBASE (0x90)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_REMAPBASE_LOCK_OFFSET   (0x0)
#define S_SA_REMAPBASE_LOCK_WIDTH    (0x1)
#define B_SA_REMAPBASE_LOCK_MASK     (0x1)
#define V_SA_REMAPBASE_LOCK_DEFAULT  (0x0)
///
/// Description of REMAPBASE (20:35)
///  The value in this register defines the lower boundary of the Remap window. The Remap window is inclusive of this address. In the decoder A[19:0] of the Remap Base Address are assumed to be 0's. Thus the bottom of the defined memory range will be aligned to a 1MB boundary.
///  When the value in this register is greater than the value programmed into the Remap Limit register, the Remap window is disabled.
///  These bits are LT lockable.
///
#define N_SA_REMAPBASE_REMAPBASE_OFFSET  (0x14)
#define S_SA_REMAPBASE_REMAPBASE_WIDTH   (0x10)
#define B_SA_REMAPBASE_REMAPBASE_MASK    (0xffff00000)
#define V_SA_REMAPBASE_REMAPBASE_DEFAULT (0xffff00000)

///
/// Description:
///
#define R_SA_REMAPLIMIT  (0x98)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_REMAPLIMIT_LOCK_OFFSET  (0x0)
#define S_SA_REMAPLIMIT_LOCK_WIDTH   (0x1)
#define B_SA_REMAPLIMIT_LOCK_MASK    (0x1)
#define V_SA_REMAPLIMIT_LOCK_DEFAULT (0x0)
///
/// Description of REMAPLMT (20:35)
///  The value in this register defines the upper boundary of the Remap window. The Remap window is inclusive of this address. In the decoder A[19:0] of the remap limit address are assumed to be F's. Thus the top of the defined range will be one byte less than a 1MB boundary.
///  When the value in this register is less than the value programmed into the Remap Base register, the Remap window is disabled.
///  These Bits are LT lockable.
///
#define N_SA_REMAPLIMIT_REMAPLMT_OFFSET  (0x14)
#define S_SA_REMAPLIMIT_REMAPLMT_WIDTH   (0x10)
#define B_SA_REMAPLIMIT_REMAPLMT_MASK    (0xffff00000)
#define V_SA_REMAPLIMIT_REMAPLMT_DEFAULT (0x0)

///
/// Description:
///  This Register contains the size of physical memory.  BIOS determines the memory size reported to the OS using this Register.
///
#define R_SA_TOM (0xa0)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_TOM_LOCK_OFFSET   (0x0)
#define S_SA_TOM_LOCK_WIDTH    (0x1)
#define B_SA_TOM_LOCK_MASK     (0x1)
#define V_SA_TOM_LOCK_DEFAULT  (0x0)

///
/// Description of TOM (20:38)
///  This register reflects the total amount of populated physical memory. This is NOT necessarily the highest main memory address (holes may exist in main memory address map due to addresses allocated for memory mapped IO). These bits correspond to address bits 38:20 (1MB granularity). Bits 19:0 are assumed to be 0. All the bits in this register are locked in LT mode.
///
#define N_SA_TOM_TOM_OFFSET  (0x14)
#define S_SA_TOM_TOM_WIDTH   (0x13)
#define B_SA_TOM_TOM_MASK    (0x7ffff00000)
#define V_SA_TOM_TOM_DEFAULT (0x7ffff00000)

///
/// Description:
///  This 64 bit register defines the Top of Upper Usable DRAM.
///  Configuration software must set this value to TOM minus all EP stolen memory if reclaim is disabled.  If reclaim is enabled, this value must be set to reclaim limit + 1byte, 1MB aligned, since reclaim limit is 1MB aligned. Address bits 19:0 are assumed to be 000_0000h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register and greater than or equal to 4GB.
///  BIOS Restriction: Minimum value for TOUUD is 4GB.
///  These bits are LT lockable.
///
#define R_SA_TOUUD (0xa8)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_TOUUD_LOCK_OFFSET   (0x0)
#define S_SA_TOUUD_LOCK_WIDTH    (0x1)
#define B_SA_TOUUD_LOCK_MASK     (0x1)
#define V_SA_TOUUD_LOCK_DEFAULT  (0x0)
///
/// Description of TOUUD (20:38)
///  This register contains bits 38  to 20 of an address one byte above the maximum DRAM memory above 4G that is usable by the operating system. Configuration software must set this value to TOM minus all EP stolen memory if reclaim is disabled. If reclaim is enabled, this value must be set to reclaim limit 1MB aligned since reclaim limit + 1byte is 1MB aligned. Address bits 19:0 are assumed to be 000_0000h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register and greater than 4GB.
///  All the bits in this register are locked in LT mode.
///
#define N_SA_TOUUD_TOUUD_OFFSET  (0x14)
#define S_SA_TOUUD_TOUUD_WIDTH   (0x13)
#define B_SA_TOUUD_TOUUD_MASK    (0x7ffff00000ULL)
#define V_SA_TOUUD_TOUUD_DEFAULT (0x0)

///
/// Description:
///  This register contains the base address of graphics data stolen DRAM memory. BIOS determines the base of graphics data stolen memory by subtracting the graphics data stolen memory size (PCI Device 0 offset 52 bits 7:4) from TOLUD (PCI Device 0 offset BC bits 31:20).
///
#define R_SA_BDSM  (0xb0)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_BDSM_LOCK_OFFSET  (0x0)
#define S_SA_BDSM_LOCK_WIDTH   (0x1)
#define B_SA_BDSM_LOCK_MASK    (0x1)
#define V_SA_BDSM_LOCK_DEFAULT (0x0)
///
/// Description of BDSM (20:31)
///  This register contains bits 31 to 20 of the base address of stolen DRAM memory. BIOS determines the base of graphics stolen memory by subtracting the graphics stolen memory size (PCI Device 0 offset 52 bits 6:4) from TOLUD (PCI Device 0 offset BC bits 31:20).
///
#define N_SA_BDSM_BDSM_OFFSET  (0x14)
#define S_SA_BDSM_BDSM_WIDTH   (0xc)
#define B_SA_BDSM_BDSM_MASK    (0xfff00000)
#define V_SA_BDSM_BDSM_DEFAULT (0x0)

///
/// Description:
///  This register contains the base address of stolen DRAM memory for the GTT. BIOS determines the base of GTT stolen memory by subtracting the GTT graphics stolen memory size (PCI Device 0 offset 52 bits 9:8) from the Graphics Base of  Data Stolen Memory (PCI Device 0 offset B0 bits 31:20).
///
#define R_SA_BGSM  (0xb4)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_BGSM_LOCK_OFFSET  (0x0)
#define S_SA_BGSM_LOCK_WIDTH   (0x1)
#define B_SA_BGSM_LOCK_MASK    (0x1)
#define V_SA_BGSM_LOCK_DEFAULT (0x0)
///
/// Description of BGSM (20:31)
///  This register contains the base address of stolen DRAM memory for the GTT. BIOS determines the base of GTT stolen memory by subtracting the GTT graphics stolen memory size (PCI Device 0 offset 52 bits 11:8) from the Graphics Base of  Data Stolen Memory (PCI Device 0 offset B0 bits 31:20).
///
#define N_SA_BGSM_BGSM_OFFSET  (0x14)
#define S_SA_BGSM_BGSM_WIDTH   (0xc)
#define B_SA_BGSM_BGSM_MASK    (0xfff00000)
#define V_SA_BGSM_BGSM_DEFAULT (0x0)

///
/// Description:
///  This register contains the base address of TSEG DRAM memory. BIOS determines the base of TSEG memory which must be at or below Graphics Base of GTT Stolen Memory (PCI Device 0 Offset B4 bits 31:20).
///
#define R_SA_TSEGMB  (0xb8)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_TSEGMB_LOCK_OFFSET  (0x0)
#define S_SA_TSEGMB_LOCK_WIDTH   (0x1)
#define B_SA_TSEGMB_LOCK_MASK    (0x1)
#define V_SA_TSEGMB_LOCK_DEFAULT (0x0)
///
/// Description of TSEGMB (20:31)
///  This register contains the base address of TSEG DRAM memory. BIOS determines the base of TSEG memory which must be at or below Graphics Base of GTT Stolen Memory (PCI Device 0 Offset B4 bits 31:20).
///
#define N_SA_TSEGMB_TSEGMB_OFFSET  (0x14)
#define S_SA_TSEGMB_TSEGMB_WIDTH   (0xc)
#define B_SA_TSEGMB_TSEGMB_MASK    (0xfff00000)
#define V_SA_TSEGMB_TSEGMB_DEFAULT (0x0)

///
/// Description:
///  This register contains the Top of low memory address.
///
#define R_SA_TOLUD (0xbc)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_TOLUD_LOCK_OFFSET   (0x0)
#define S_SA_TOLUD_LOCK_WIDTH    (0x1)
#define B_SA_TOLUD_LOCK_MASK     (0x1)
#define V_SA_TOLUD_LOCK_DEFAULT  (0x0)
///
/// Description of TOLUD (20:31)
///  This register contains bits 31 to 20 of an address one byte above the maximum DRAM memory below 4G that is usable by the operating system. Address bits 31 down to 20 programmed to 01h implies a minimum memory size of 1MB. Configuration software must set this value to the smaller of the following 2 choices: maximum amount memory in the system minus ME stolen memory plus one byte or the minimum address allocated for PCI memory. Address bits 19:0 are assumed to be 0_0000h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register.
///  The Top of Low Usable DRAM is the lowest address above both Graphics Stolen memory and Tseg. BIOS determines the base of Graphics Stolen Memory by subtracting the Graphics Stolen Memory Size from TOLUD and further decrements by Tseg size to determine base of Tseg. All the Bits in this register are locked in LT mode.
///  This register must be 1MB aligned when reclaim is enabled.
///
#define N_SA_TOLUD_TOLUD_OFFSET    (0x14)
#define S_SA_TOLUD_TOLUD_WIDTH     (0xc)
#define B_SA_TOLUD_TOLUD_MASK      (0xfff00000)
#define V_SA_TOLUD_TOLUD_DEFAULT   (0x100000)

#define R_SA_MC_ERRSTS_OFFSET      0xC8    ///< Error Status Register
#define B_GMSWSMI                  0x1000  ///< Software SMI
#define R_SA_MC_ERRCMD_OFFSET      0xCA    ///< Error Command Register
#define R_SA_MC_SKPD_OFFSET        0xDC    ///< Scratch Pad Data
#define R_SA_MC_CAPID0_OFFSET      0xE0    ///< Capability Identification @bug reserved in EDS
#define R_SA_MC_CAPID0_A_OFFSET    0xE4

//
// MCHBAR IO Register Offset Equates
//
#define R_SA_MCHBAR_PAVPC_OFFSET                   0x34    ///< PAPVC
#define R_SA_MCHBAR_MC_LOCK_OFFSET                 0x50FC
#define R_SA_MCHBAR_PCIE_CODE_VERSION_OFFSET_HSW   0x5434
#define R_SA_MCHBAR_BIOS_RESET_CPL_OFFSET          0x5DA8
#define R_SA_MCHBAR_SAPMCTL_OFFSET                 0x5F00
#define R_SA_MCHBAR_GDXCBAR_OFFSET                 0x5420
#define R_SA_MCHBAR_EDRAMBAR_OFFSET                0x5408
#define R_SA_MCHBAR_ILTR_OVRD_OFFSET               0x5D30
#define R_SA_MCHBAR_EDRAM_RATIO_OFFSET             0x5E08
#define R_SA_MCHBAR_UMAGFXCTL                      0x6020
#define R_SA_MCHBAR_VTDTRKLCK                      0x63FC
#define R_SA_MCHBAR_RESERVED_0_OFFSET              0x6800
#define R_SA_MCHBAR_RESERVED_1_OFFSET              0x7000
#define R_SA_MCHBAR_CRDTCTL0_OFFSET                0x7400
#define R_SA_MCHBAR_CRDTCTL1_OFFSET                0x7404
#define R_SA_MCHBAR_CRDTCTL2_OFFSET                0x7408
#define R_SA_MCHBAR_CRDTCTL3_OFFSET                0x740C
#define R_SA_MCHBAR_CRDTCTL4_OFFSET                0x7410
#define R_SA_MCHBAR_CRDTCTL6_OFFSET                0x7418
#define R_SA_MCHBAR_CRDTCTL8_OFFSET                0x7454
#define R_SA_MCHBAR_RESERVED_2_OFFSET              0x77FC
#define R_SA_MCHBAR_RESERVED_3_OFFSET              0x7BFC

#define V_SA_LTR_MAX_SNOOP_LATENCY_VALUE           0x0846  ///< Intel recommended maximum value for Snoop Latency (70us)
#define V_SA_LTR_MAX_NON_SNOOP_LATENCY_VALUE       0x0846  ///< Intel recommended maximum value for Non-Snoop Latency (70us)
#define B_SA_MCHBAR_EDRAM_RATIO_MASK               0x0000007F

#define R_SA_MCHBAR_HDAUDRID_IMPH                  0x6008
#define N_SA_MCHBAR_HDAUDRID_IMPH_DEVNUM_OFFSET    3
#define B_SA_MCHBAR_HDAUDRID_IMPH_DEVNUM_MASK      0x000000F8

//
// Thermal Management Controls
//

//Description:
//  This is the MSR address that enables configuring the recipe for PEG and DMI private space registers.
#define R_SA_MSRIO_ADDRESS 0x791

//
// Device 1 Memory Mapped IO Register Offset Equates
//
#define V_SA_PEG_VID     0x8086
#define V_SA_PEG_DID     0x2A41

//
// Temporary Device & Function Number used for Switchable Graphics DGPU
//
#define SA_TEMP_DGPU_DEV      0x00
#define SA_TEMP_DGPU_FUN      0x00

///
/// Device 1 Register Equates
///
#define R_SA_PEG_VID_OFFSET            0x00  ///< Vendor ID
#define R_SA_PEG_DID_OFFSET            0x02  ///< Device ID
#define R_SA_PEG_BCTRL_OFFSET          0x3E  ///< Bridge Control
#define R_SA_PEG_SSCAPID_OFFSET        0x88  ///< Subsystem ID and Vendor ID Capabilities IS
#define R_SA_PEG_SS_OFFSET             0x8C  ///< Subsystem ID
#define R_SA_PEG_MSICAPID_OFFSET       0x90  ///< MSI Capabilities ID
#define R_SA_PEG_MC_OFFSET             0x92  ///< Message Control
#define R_SA_PEG_MA_OFFSET             0x94  ///< Message Address
#define R_SA_PEG_MD_OFFSET             0x98  ///< Message Data
#define R_SA_PEG_CAPL_OFFSET           0xA0  ///< PEG Capabilities List
#define R_SA_PEG_CAP_OFFSET            0xA2  ///< PEG Capabilities
#define R_SA_PEG_DCAP_OFFSET           0xA4  ///< Device Capabilities
#define R_SA_PEG_DCTL_OFFSET           0xA8  ///< Device Control
#define R_SA_PEG_DSTS_OFFSET           0xAA  ///< Device Status
#define R_SA_PEG_LCAP_OFFSET           0xAC  ///< Link Capabilities
#define R_SA_PEG_LCTL_OFFSET           0xB0  ///< Link Control
#define R_SA_PEG_LSTS_OFFSET           0xB2  ///< Link Status
#define R_SA_PEG_SLOTCAP_OFFSET        0xB4  ///< Slot Capabilities
#define R_SA_PEG_SLOTCTL_OFFSET        0xB8  ///< Slot Control
#define R_SA_PEG_SLOTSTS_OFFSET        0xBA  ///< Slot Status
#define R_SA_PEG_RCTL_OFFSET           0xBC  ///< Root Control
#define R_SA_PEG_RSTS_OFFSET           0xC0  ///< Root Status
#define R_SA_PEG_DCAP2_OFFSET          0xC4  ///< Device Capability 2
#define R_SA_PEG_DCTL2_OFFSET          0xC8  ///< Device Control 2
#define R_SA_PEG_LCTL2_OFFSET          0xD0  ///< Link Control 2
#define R_SA_PEG_LSTS2_OFFSET          0xD2  ///< Link Status 2
#define R_SA_PEG_PEGLC_OFFSET          0xEC  ///< PEG Legacy Control
#define R_SA_PEG_VCECH_OFFSET          0x100 ///< PEG Virtual Channel Enhanced Capability
#define R_SA_PEG_VCCAP1_OFFSET         0x104 ///< PEG Port VC Capability
#define R_SA_PEG_VCCAP2_OFFSET         0x108 ///< PEG Port VC Capability 2
#define R_SA_PEG_VCCTL_OFFSET          0x10C ///< PEG Port VC Control
#define R_SA_PEG_VC0RCAP_OFFSET        0x110 ///< PEG VC0 Resource Capability
#define R_SA_PEG_VC0RCTL0_OFFSET       0x114 ///< PEG VC0 Resource Control
#define R_SA_PEG_VC0RSTS_OFFSET        0x11A ///< PEG VC0 Resource Status
#define R_SA_PEG_RCLDECH_OFFSET        0x140 ///< PEG Root Complex Link Declaration
#define R_SA_PEG_ESD_OFFSET            0x144 ///< PEG Element Self Description
#define R_SA_PEG_LE1D_OFFSET           0x150 ///< PEG Link Entry 1 Description
#define R_SA_PEG_LE1A_OFFSET           0x158 ///< PEG Link Entry 1 Address
#define R_SA_PEG_PEGUESTS_OFFSET       0x1C4 ///< PEG Error Status
#define R_SA_PEG_PEGCESTS_OFFSET       0x1D0 ///< PEG Error Status
#define R_SA_PEG_PEGCC_OFFSET          0x208 ///< PEG Countdown Control
#define R_SA_PEG_PEGSTS_OFFSET         0x214 ///< PEG Status
#define R_SA_PEG_LTSSMC_OFFSET         0x224 ///< PEG LTSSMC Control
#define R_SA_PEG_L0SLAT_OFFSET         0x22C ///< PEG L0s Control
#define R_SA_PEG_CFG2_OFFSET           0x250 ///< PEG Config 2
#define R_SA_PEG_CFG4_OFFSET           0x258 ///< PEG Config 4
#define R_SA_PEG_CFG5_OFFSET           0x25C ///< PEG Config 5
#define R_SA_PEG_CFG6_OFFSET           0x260 ///< PEG Config 6
#define R_SA_PEG_VC0PRCA_OFFSET        0x308 ///< PEG VC0 Posted
#define R_SA_PEG_VC0NPRCA_OFFSET       0x314 ///< PEG VC0 Non-Posted
#define R_SA_PEG_VC0CCL_OFFSET         0x31C ///< PEG Completion Credit Limit
#define R_SA_PEG_VC01CL_OFFSET         0x320 ///< PEG Chaining Limit
#define R_SA_PEG_VC1PRCA_OFFSET        0x32C ///< PEG VC1 Posted
#define R_SA_PEG_VC1NPRCA_OFFSET       0x330 ///< PEG VC1 Non-Posted
#define R_SA_PEG_EQPH3_OFFSET          0x384 ///< PEG Phase 3
#define R_SA_PEG_REUT_PH_CTR_OFFSET    0x444 ///< PEG PHY Control
#define R_SA_PEG_REUT_PH_CTR1_OFFSET   0x448 ///< PEG PHY Control 1
#define R_SA_PEG_REUT_PH1_PIS_OFFSET   0x464 ///< PEG PH1 PIS
#define R_SA_PEG_REUT_OVR_CTL_OFFSET   0x490 ///< PEG REUT Override
#define R_SA_PEG_FUSESCMN_OFFSET       0x504 ///< PEG Fuses
#define R_SA_PEG_AFELN0VMTX2_OFFSET    0x70C ///< PEG AFE Lane VMTX2
#define R_SA_PEG_AFELN0IOCFG0_OFFSET   0x808 ///< PEG AFE Lane IO Config 0
#define R_SA_PEG_AFEBND0CFG0_OFFSET    0x900 ///< PEG AFE Bundle Config 0
#define R_SA_PEG_AFEBND0CFG1_OFFSET    0x904 ///< PEG AFE Bundle Config 1
#define R_SA_PEG_AFEBND0CFG2_OFFSET    0x908 ///< PEG AFE Bundle Config 2
#define R_SA_PEG_AFEBND0CFG3_OFFSET    0x90C ///< PEG AFE Bundle Config 3
#define R_SA_PEG_AFEBND0CFG4_OFFSET    0x910 ///< PEG AFE Bundle Config 4
#define R_SA_PEG_LOADBUSCTL0_OFFSET    0x914 ///< PEG Load Bus Control
#define R_SA_PEG_G3CTL0_OFFSET         0x918 ///< PEG Gen3 Control
#define R_SA_PEG_BND0SPARE_OFFSET      0x91C ///< PEG Bundle 0 Spare Register
#define R_SA_PEG_AFELN0CFG0_OFFSET     0xA00 ///< PEG AFE Lane Config 0
#define R_SA_PEG_AFELN0CFG1_OFFSET     0xA04 ///< PEG AFE Lane Config 1
#define R_SA_PEG_AFEOVR_OFFSET         0xC20 ///< PEG AFE Override
#define R_SA_PEG_AFE_PWRON_OFFSET      0xC24 ///< PEG AFE Power-on
#define R_SA_PEG_AFE_PM_TMR_OFFSET     0xC28 ///< PEG AFE PM Timer
#define R_SA_PEG_CMNRXERR_OFFSET       0xC34 ///< PEG Common Error
#define R_SA_PEG_CMNSPARE_OFFSET       0xC38 ///< PEG Common Spare
#define R_SA_PEG_G3PLINIT_OFFSET       0xCD4 ///< PEG Gen3 Physical Init
#define R_SA_PEG_PEGTST_OFFSET         0xD0C ///< PEG TEST
#define R_SA_PEG_DEBUP2_OFFSET         0xD10 ///< PEG Debug
#define R_SA_PEG_DEBUP3_OFFSET         0xD14 ///< PEG Debug
#define R_SA_PEG_PEGCOMLCGCTRL_OFFSET  0xD20 ///< PEG Clock Gating Control
#define R_SA_PEG_FCLKGTTLLL_OFFSET     0xD24 ///< PEG FCLK Clock Gating
#define R_SA_PEG_PEGCLKGTCMN_OFFSET    0xD2C ///< PEG Spine Clock Gating
#define R_SA_PEG_LCTL3_OFFSET          0xD98 ///< PEG Link Control 3
#define R_SA_PEG_LNERRSTAT_OFFSET      0xD9C ///< PEG Lane Error Status
#define R_SA_PEG_EQCTL0_1_OFFSET       0xDA0 ///< PEG Lane Equalization
#define R_SA_PEG_EQPRESET1_2_OFFSET    0xDC0 ///< PEG Coefficients for P1 and P2
#define R_SA_PEG_EQPRESET2_3_4_OFFSET  0xDC4 ///< PEG Coefficients for P2, P3, and P4
#define R_SA_PEG_EQPRESET4_5_OFFSET    0xDC8 ///< PEG Coefficients for P4 and P5
#define R_SA_PEG_EQPRESET6_7_OFFSET    0xDCC ///< PEG Coefficients for P6 and P7
#define R_SA_PEG_EQPRESET7_8_9_OFFSET  0xDD0 ///< PEG Coefficients for P7, P8, and P9
#define R_SA_PEG_EQPRESET9_10_OFFSET   0xDD4 ///< PEG Coefficients for P9 and P10
#define R_SA_PEG_EQCFG_OFFSET          0xDD8 ///< PEG Equalization Config
#define R_SA_PEG_EQPRESET11_OFFSET     0xDDC ///< PEG Coefficients for P11
#define R_SA_PEG_AFEBND0CFG5_OFFSET    0xE04 ///< PEG AFE Bundle Config 5
#define R_SA_PEG_PLUCTLH0_OFFSET       0xE0C ///< PEG Lane Config

#define R_SA_MSRIO_PEG_AFE_PH3_CFG0_BND_0_OFFSET  0x200 ///< PEG AFE PH3 Bundle Config 0 
#define R_SA_MSRIO_PEG_AFE_PH3_CFG1_BND_0_OFFSET  0x210 ///< PEG AFE PH3 Bundle Config 1 
#define R_SA_MSRIO_PEG_AFE_PH3_CFG2_BND_0_OFFSET  0x204 ///< PEG AFE PH3 Bundle Config 2 

///
/// Device 2 Register Equates
//
// The following equates must be reviewed and revised when the specification is ready.
//
#define SA_IGD_BUS           0x00
#define SA_IGD_DEV           0x02
#define SA_IGD_FUN_0         0x00
#define SA_IGD_FUN_1         0x01
#define SA_IGD_DEV_FUN       (SA_IGD_DEV << 3)
#define SA_IGD_BUS_DEV_FUN   (SA_MC_BUS << 8) + SA_IGD_DEV_FUN

#define V_SA_IGD_VID         0x8086
#define V_SA_IGD_DID         0x2A42
#define V_SA_IGD_DID_MB      0x0106
#define V_SA_IGD_DID_MB_1    0x0116
#define V_SA_IGD_DID_MB_2    0x0126
#define V_SA_IGD_DID_DT      0x0102
#define V_SA_IGD_DID_DT_1    0x0112
#define V_SA_IGD_DID_DT_2    0x0122
#define V_SA_IGD_DID_DT_3    0x010A

///
/// For SKL IGD
///
#define V_SA_PCI_DEV_2_GT1_SULTM_ID    0x01906 ///< Dev2-SKL ULT GT1 (2+1F) Mobile
#define V_SA_PCI_DEV_2_GT15F_SULTM_ID  0x01913 ///< Dev2-SKL ULT GT1.5 (2+1.5F) Mobile
#define V_SA_PCI_DEV_2_GT2_SULTM_ID    0x01916 ///< Dev2-SKL ULT GT2 (2+2) Mobile
#define V_SA_PCI_DEV_2_GT2F_SULTM_ID   0x01921 ///< Dev2-SKL ULT GT2 (2+2F) Mobile
#define V_SA_PCI_DEV_2_GT3_SULTM_ID    0x01926 ///< Dev2-SKL ULT GT3 (3+3/3E) Mobile
#define V_SA_PCI_DEV_2_GT1_SHALM_ID    0x0190B ///< Dev2-SKL Halo GT1 (2+1)
#define V_SA_PCI_DEV_2_GT2_SHALM_ID    0x0191B ///< Dev2-SKL Halo GT2 (4/2+2)
#define V_SA_PCI_DEV_2_GT3_SHALM_ID    0x0192B ///< Dev2-SKL Halo GT3 (4+3FE)
#define V_SA_PCI_DEV_2_GT4_SHALM_ID    0x0193B ///< Dev2-SKL Halo GT4 (4+4E)
#define V_SA_PCI_DEV_2_GT1_SULXM_ID    0x0190E ///< Dev2-SKL ULX GT1(2+1F) Mobile
#define V_SA_PCI_DEV_2_GT15_SULXM_ID   0x01915 ///< Dev2-SKL ULX GT1.5(2+1.5F) Mobile
#define V_SA_PCI_DEV_2_GT2_SULXM_ID    0x0191E ///< Dev2-SKL ULX GT2 (2+2)Mobile
#define V_SA_PCI_DEV_2_GT1_SSR_ID      0x0190A ///< Dev2-SKL GT1 (4+1F) Server
#define V_SA_PCI_DEV_2_GT2_SSR_ID      0x0191A ///< Dev2-SKL GT2 (4/2+2) Server
#define V_SA_PCI_DEV_2_GT3_SSR_ID      0x0192A ///< Dev2-SKL GT3 (2+3E) Server
#define V_SA_PCI_DEV_2_GT4_SSR_ID      0x0193A ///< Dev2-SKL GT4 (4+4E) Server
#define V_SA_PCI_DEV_2_GT1_SDT_ID      0x01902 ///< Dev2-SKL GT1 (2+1F) Desktop
#define V_SA_PCI_DEV_2_GT2_SDT_ID      0x01912 ///< Dev2-SKL GT2 (4/2+2) Desktop
#define V_SA_PCI_DEV_2_GT15_SDT_ID     0x01917 ///< Dev2-SKL GT1.5 (2+1.5F) Desktop
#define V_SA_PCI_DEV_2_GT4_SDT_ID      0x01932 ///< Dev2-SKL GT4 (4+4) Desktop

#define R_SA_IGD_VID               0x00
#define R_SA_IGD_DID               0x02
#define R_SA_IGD_CMD               0x04
///
/// GTTMMADR for SKL is 16MB alignment (Base address = [38:24])
///
#define R_SA_IGD_GTTMMADR          0x10
#define R_SA_IGD_GMADR             0x18
#define R_SA_IGD_IOBAR             0x20

#define R_SA_IGD_BSM_OFFSET        0x005C  ///< Base of Stolen Memory
#define R_SA_IGD_MSAC_OFFSET       0x0062  ///< Multisize Aperture Control
#define R_SA_IGD_MSICAPID_OFFSET   0x0090  ///< MSI Capabilities ID
#define R_SA_IGD_MC_OFFSET         0x0092  ///< Message Control
#define R_SA_IGD_MA_OFFSET         0x0094  ///< Message Address
#define R_SA_IGD_MD_OFFSET         0x0098  ///< Message Data
#define R_SA_IGD_SWSCI_OFFSET      0x00E8
#define R_SA_IGD_ASLS_OFFSET       0x00FC  ///< ASL Storage

#define R_SA_IGD_AUD_FREQ_CNTRL_OFFSET 0x65900 ///< iDisplay Audio BCLK Frequency Control
#define B_SA_IGD_AUD_FREQ_CNTRL_TMODE  BIT15   ///< T-Mode: 0b - 2T, 1b - 1T
#define B_SA_IGD_AUD_FREQ_CNTRL_96MHZ  BIT4    ///< 96 MHz BCLK
#define B_SA_IGD_AUD_FREQ_CNTRL_48MHZ  BIT3    ///< 48 MHz BCLK

#define R_SA_GTTMMADR_GTDOORBELL_OFFSET 0x124828  ///< iTouch GT Doorbell Register
#define B_SA_GTTMMADR_GTDOORBELL_LOCK   BIT40     ///< 1b - locks BDF

#define R_SA_GTTMMADR_GSA_TOUCH_OFFSET 0x1300B4  ///< iTouch GSA Touch Register
#define B_SA_GTTMMADR_GSA_TOUCH_LOCK   BIT0      ///< 1b - locks BDF



///
/// Maximum number of SDRAM channels supported by the memory controller
///
///
/// Maximum number of SDRAM channels supported by the memory controller
///
#define SA_MC_MAX_CHANNELS 2
///
/// Maximum number of DIMM sockets supported by each channel
///
#define SA_MC_MAX_SLOTS 2

///
/// Maximum number of sides supported per DIMM
///
#define SA_MC_MAX_SIDES 2

///
/// Maximum number of DIMM sockets supported by the memory controller
///
#define SA_MC_MAX_SOCKETS (SA_MC_MAX_CHANNELS * SA_MC_MAX_SLOTS)

///
/// Maximum number of rows supported by the memory controller
///
#define SA_MC_MAX_RANKS (SA_MC_MAX_SOCKETS * SA_MC_MAX_SIDES)

///
/// Maximum number of rows supported by the memory controller
///
#define SA_MC_MAX_ROWS (SA_MC_MAX_SIDES * SA_MC_MAX_SOCKETS)

///
/// Maximum memory supported by the memory controller
/// 4 GB in terms of KB
///
#define SA_MC_MAX_MEM_CAPACITY (4 * 1024 * 1024)

///
/// Define the SPD Address for DIMM 0
///
#define SA_MC_DIMM0_SPD_ADDRESS 0xA0

///
/// Define the maximum number of data bytes on a system with no ECC memory support.
///
#define SA_MC_MAX_BYTES_NO_ECC (8)

///
/// Define the maximum number of SPD data bytes on a DIMM.
///
#define SA_MC_MAX_SPD_SIZE (512)

///
/// Internal Graphics Display and Render Clock Freqencies
///
#define R_SA_MC_MMIO_GCFGC       0xC8C                           ///< Graphics Clock Frequency and Gating Control
#define R_SA_IGD_GCFGC_OFFSET    0x00F0                          ///< Graphics Clock Frequency and Gating Control
#define N_SA_GCFGC_GCRC          BIT9                            ///< Gate Core Display ClockGate Core Display Clock
#define B_SA_GCFGC_GCSCS_MASK    (BIT19 | BIT18 | BIT17 | BIT16) ///< Graphics Core Sampler Clock Select (GCSCS)
#define B_SA_GCFGC_GCDCS_MASK    (BIT6 | BIT5 | BIT4)            ///< Graphics Core Display Clock Select (GCDCS)
#define B_SA_GCFGC_GCRCS_MASK    (BIT3 | BIT2 | BIT1 | BIT0)     ///< Graphics Core Render Clock Select (GCRCS)
#define R_SA_MC_MMIO_IOCKTRR1    0x0B68                          ///< IO Circuit Reserve Register 1
#define R_SA_MCHBAR_GFXPLL0      0x2C30                          ///< GFX PLL BIOS
#define R_SA_MCHBAR_GFXPLL1      0x2C32                          ///< GFX PLL BIOS
#define R_SA_MCHBAR_GFXPLL9      0x2C44                          ///< GFX PLL Frequency Change Control
#define R_SA_MCHBAR_RGVSWCTL     0x1170
#define R_SA_MCHBAR_VIDCTL       0x11C0
#define R_SA_MCHBAR_PMCFG        0x1210

///
/// DMIBAR registers
///
#define R_SA_DMIBAR_DMIPVCCAP1_OFFSET      0x4       ///< DMI VC Capabilities 1
#define R_SA_DMIBAR_DMIVC0RCTL_OFFSET      0x14      ///< DMI VC0 Resource Control
#define R_SA_DMIBAR_DMIVC0RSTS_OFFSET      0x1A      ///< DMI VC0 Status
#define R_SA_DMIBAR_DMIVC1RCTL_OFFSET      0x20      ///< DMI VC1 Resource Control
#define R_SA_DMIBAR_DMIVC1RSTS_OFFSET      0x26      ///< DMI VC1 Status
#define R_SA_DMIBAR_DMIVCMRCTL_OFFSET      0x38      ///< DMI VCm Resource Control
#define R_SA_DMIBAR_DMIVCMRSTS_OFFSET      0x3E      ///< DMI VCm Status
#define R_SA_DMIBAR_LCAP_OFFSET            0x84      ///< DMI Link Capabilities
#define R_SA_DMIBAR_LCTL_OFFSET            0x88      ///< DMI Link Control
#define R_SA_DMIBAR_LSTS_OFFSET            0x8A      ///< DMI Link Status
#define R_SA_DMIBAR_LCTL2_OFFSET           0x98      ///< DMI Link Control 2
#define R_SA_DMIBAR_LSTS2_OFFSET           0x9A      ///< Link Status 2
#define R_SA_DMIBAR_DCAP_OFFSET            0xA4      ///< DMI Device Capabilities
#define R_SA_DMIBAR_DCTL_OFFSET            0xA8      ///< DMI Device Control
#define R_SA_DMIBAR_DMICC_OFFSET           0x208     ///< DMI Config Control
#define R_SA_DMIBAR_DMISTS_OFFSET          0x214     ///< DMI Status
#define R_SA_DMIBAR_LTSSMC_OFFSET          0x224     ///< DMI LTSSMC Control
#define R_SA_DMIBAR_L0SLAT_OFFSET          0x22C     ///< DMI L0s Control
#define R_SA_DMIBAR_CFG4_OFFSET            0x258     ///< DMI Config 4
#define R_SA_DMIBAR_CFG5_OFFSET            0x25C     ///< DMI Config 5
#define R_SA_DMIBAR_CFG6_OFFSET            0x260     ///< DMI Config 6
#define R_SA_DMIBAR_PCIELOCK2_OFFSET       0x328     ///< DMI PCIELOCK2
#define R_SA_DMIBAR_AFELN0VMTX2_OFFSET     0x70C     ///< DMI AFE Lane VMTX2
#define R_SA_DMIBAR_AFELN0IOCFG0_OFFSET    0x808     ///< DMI AFE Lane IO Config 0
#define R_SA_DMIBAR_AFEBND0CFG1_OFFSET     0x904     ///< DMI AFE Bundle Config 1
#define R_SA_DMIBAR_AFEBND0CFG2_OFFSET     0x908     ///< DMI AFE Bundle Config 2
#define R_SA_DMIBAR_AFELN0CFG0_OFFSET      0xA00     ///< DMI AFE Lane Config 0
#define R_SA_DMIBAR_AFECMNCFG0_OFFSET      0xC00     ///< DMI AFE Common Config 0
#define R_SA_DMIBAR_AFECMNCFG1_OFFSET      0xC04     ///< DMI AFE Common Config 1
#define R_SA_DMIBAR_AFECMNCFG2_OFFSET      0xC08     ///< DMI AFE Common Config 2
#define R_SA_DMIBAR_AFECMNCFG3_OFFSET      0xC0C     ///< DMI AFE Common Config 3
#define R_SA_DMIBAR_AFE_PM_TMR_OFFSET      0xC28     ///< DMI AFE PM Timer
#define R_SA_DMIBAR_CMNSPARE_OFFSET        0xC38     ///< DMI Common Spare
#define R_SA_DMIBAR_DMITST_OFFSET          0xD0C     ///< DMI TEST
#define R_SA_DMIBAR_SCRATCHPAD0_OFFSET     0xD64     ///< DMI Scratchpad 0
#define R_SA_DMIBAR_SCRATCHPAD1_OFFSET     0xD68     ///< DMI Scratchpad 1
#define N_SA_DMIBAR_DMIVCCTL_EN            BIT31
#define V_SA_DMIBAR_DMIVCCTL_ID            (7 << 24) ///< Bit[26:24]
#define B_SA_DMIBAR_DMIVCCTL_TVM_MASK      0xFE
#define N_SA_DMIBAR_DMIVCCTL_ID            24
#define B_SA_DMIBAR_DMISTS_NP              BIT1
#define B_SA_DMIBAR_DCAP_OFFSET_LOCK       0x7         //(BIT2|BIT1|BIT0)
#define B_SA_DMIBAR_DCTL_OFFSET_MASK       0xFFFFFF1F  //~(BIT7|BIT6|BIT5)

#define R_SA_MSRIO_DMIBAR_AFE_PH3_CFG0_BND_0_OFFSET  0xA00 ///< DMI AFE PH3 Bundle Config 0 
#define R_SA_MSRIO_DMIBAR_AFE_PH3_CFG1_BND_0_OFFSET  0xA10 ///< DMI AFE PH3 Bundle Config 1 
#define R_SA_MSRIO_DMIBAR_AFE_PH3_CFG2_BND_0_OFFSET  0xA04 ///< DMI AFE PH3 Bundle Config 2 

///
/// Vt-d Engine base address.
///
#define R_SA_MCHBAR_VTD1_OFFSET  0x5400  ///< HW UNIT2 for IGD
#define R_SA_MCHBAR_VTD2_OFFSET  0x5410  ///< HW UNIT3 for all other - PEG, USB, SATA etc

//
// VCU Mailbox Definitions
//
#define R_SA_MCHBAR_VCU_MAILBOX_INTERFACE_OFFSET  0x6C00
#define R_SA_MCHBAR_VCU_MAILBOX_DATA_OFFSET       0x6C04
//
//
#define B_SA_MCHBAR_VCU_STATUS_RUN_BUSY           BIT31
//
//

#define V_SA_VCU_OPCODE_OPEN_SEQ                  0x2
#define V_SA_VCU_OPCODE_CLOSE_SEQ                 0x3
#define V_SA_VCU_OPCODE_READ_VCU_API_VER_ID       0x1
#define V_SA_VCU_OPCODE_READ_DATA                 0x7
#define V_SA_VCU_OPCODE_WRITE_DATA                0x8
#define V_SA_VCU_OPCODE_READ_CSR                  0x13
#define V_SA_VCU_OPCODE_WRITE_CSR                 0x14
#define V_SA_VCU_OPCODE_READ_MMIO                 0x15
#define V_SA_VCU_OPCODE_WRITE_MMIO                0x16

//
//

#define V_SA_VCU_SEQID_READ_CSR              0x1
#define V_SA_VCU_SEQID_WRITE_CSR             0x2
#define V_SA_VCU_SEQID_READ_MMIO             0x3
#define V_SA_VCU_SEQID_WRITE_MMIO            0x4

//
//
#define V_SA_VCU_RESPONSE_SUCCESS            0x40
#define V_SA_VCU_RESPONSE_BUSY               0x80
#define V_SA_VCU_RESPONSE_THREADUNAVAILABLE  0x82
#define V_SA_VCU_RESPONSE_ILLEGAL            0x90
//
//


///
/// VCU Miscellaneous Controls
///
#define V_SA_VCU_STATUS_BUSY_LIMIT     100
#define V_SA_VCU_RESPONSE_RETRY_LIMIT  10

//
// Device 5 Equates
//
#define SA_IMGU_BUS_NUM    0x00
#define SA_IMGU_DEV_NUM    0x05
#define SA_IMGU_FUN_NUM    0x00

//
// Device 8 Equates
//
#define SA_GMM_BUS_NUM    0x00
#define SA_GMM_DEV_NUM    0x08
#define SA_GMM_FUN_NUM    0x00
#define R_SA_GMM_GMMBAL   0x10
#define R_SA_GMM_CTRL     0x4

#endif
