//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Platform.h

  Platform specific information

**/
#include "BackCompatible.h"
#include "Uefi.h"
#include "Guid/SetupVariable.h"
#include "UncoreCommonIncludes.h"
#include <Library/PcdLib.h>

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// APTIOV_SERVER_OVERRIDE_RC_START
#include <Token.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#define EFI_VARIABLE_RUNTIME_ALLOW_RECLAIM 0x00001000

//#define EFI_FIRMWARE_VENDOR L"Intel Corporation"

//
// MMIO space allocation
// 0x00000000 -- 0xC0000000 (TOLM)
// 0xC0000000 -- 0xD0000000 (PCIe map  space)
// 0xD0000000 -- 0xFE000000 (PCI space)
//
//
// Onboard / Integrated Graphics Device
//

#define IGD_VID                         0x102B  // Matrox
#define IGD_DID                         0x0522  // iBMC Video

#define VIDEO_DEV_NUM                   0x00
#define VIDEO_FUNC_NUM                  0x00
#define VIDEO_ROOTPORT_DEV_NUM          PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS
#define VIDEO_ROOTPORT_FUNC_NUM         PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5

#define ASPEED2500_VID                  0x1A03
#define ASPEED2500_DID                  0x2000

//
// Onboard NIC device's root port info; the IO riser on SR has two NICs each supporting two LAN ports.
//
#define SR_NIC0_RP_DEV_NUM              PCIE_PORT_1_DEV
#define SR_NIC0_RP_FUNC_NUM             PCIE_PORT_1A_FUNC
#define SR_NIC1_RP_DEV_NUM              PCIE_PORT_1_DEV
#define SR_NIC1_RP_FUNC_NUM             PCIE_PORT_1B_FUNC

//
// 82576 (Kawela) & 82598 (Oplin) NIC Device IDs
//
#define NIC_82598EB_XFSR_DUAL_DIDVID    0x10C68086
#define NIC_82598EB_XFSR_SNGL_DIDVID    0x10C78086
#define NIC_82598EB_AT_SNGL_DIDVID      0x10C88086
#define NIC_82598EB_CX4_DUAL_DIDVID     0x10EC8086
#define NIC_82598EB_AFDA_DUAL_DIDVID    0x10F18086
#define NIC_82598EB_XFLR_SNGL_DIDVID    0x10F48086
#define NIC_82576_ET_DUAL_DIDVID        0x10C98086
#define NIC_82576_EF_DUAL_DIDVID        0x10E68086

//
// HECI BAR space, 16B for each HECI interface. However, a limitation in the
// hardware cause that not all valid addresses actualy work for HECI MBARs.
// The below defined addresses have been tested and work.
// NOTE: The HECI2_BASE_ADDRESS value is used in the me.asi for HEC2 object.
//
// the address that worked for Tylersburd doesnt work for Boxboro-EX
//#define HECI1_BASE_ADDRESS              0xFE900000 // HECI-1
//#define HECI2_BASE_ADDRESS              0xFEA00000 // HECI-2
// the address that worked for Tylersburd doesnt work for Boxboro-EX, below are recommended by ME SV folks
#define HECI1_BASE_ADDRESS              0xFED12000 // HECI-1 
#define HECI2_BASE_ADDRESS              0xFED12010 // HECI-2

//
// Number of P & T states supported
//
#define NPTM_P_STATES_SUPPORTED         16
#define NPTM_T_STATES_SUPPORTED         0

//
// Assigning default ID and base addresses here, these definitions are used by ACPI tables
// 

#define PCH_IOAPIC_ID                   0x08
#define PCH_IOAPIC_ADDRESS              0xFEC00000 // This must get range from Legacy IIO
#define PCH_INTERRUPT_BASE              0


#if MAX_SOCKET > 4
#define PCH_IOAPIC                      0
#else
#define PCH_IOAPIC                      (1 << 0)
#endif

#define PC00_IOAPIC                     (1 << (0 + PCH_IOAPIC))
#define PC00_IOAPIC_ID                  0x09
#define PC00_IOAPIC_ADDRESS             0xFEC01000
#define PC00_INTERRUPT_BASE             24

#define PC01_IOAPIC                     (1 << (1 + PCH_IOAPIC))
#define PC01_IOAPIC_ID                  0x0A
#define PC01_IOAPIC_ADDRESS             0xFEC08000
#define PC01_INTERRUPT_BASE             32

#define PC02_IOAPIC                     (1 << (2 + PCH_IOAPIC))
#define PC02_IOAPIC_ID                  0x0B
#define PC02_IOAPIC_ADDRESS             0xFEC10000
#define PC02_INTERRUPT_BASE             40

#define PC03_IOAPIC                     (1 << (3 + PCH_IOAPIC))
#define PC03_IOAPIC_ID                  0x0C
#define PC03_IOAPIC_ADDRESS             0xFEC18000
#define PC03_INTERRUPT_BASE             48

#define PC04_IOAPIC                     (1 << (4 + PCH_IOAPIC))
#define PC04_IOAPIC_ID                  0x0D
#define PC04_IOAPIC_ADDRESS             0xFEC20000
#define PC04_INTERRUPT_BASE             56

#define PC05_IOAPIC                     (1 << (5 + PCH_IOAPIC))
#define PC05_IOAPIC_ID                  0x0E
#define PC05_IOAPIC_ADDRESS             0xFEC28000
#define PC05_INTERRUPT_BASE             64

#define PC06_IOAPIC                     (1 << (6 + PCH_IOAPIC))
#define PC06_IOAPIC_ID                  0x0F
#define PC06_IOAPIC_ADDRESS             0xFEC30000
#define PC06_INTERRUPT_BASE             72

#define PC07_IOAPIC                     (1 << (7 + PCH_IOAPIC))
#define PC07_IOAPIC_ID                  0x10
#define PC07_IOAPIC_ADDRESS             0xFEC38000
#define PC07_INTERRUPT_BASE             80

#define PC08_IOAPIC                     (1 << (8 + PCH_IOAPIC))
#define PC08_IOAPIC_ID                  0x11
#define PC08_IOAPIC_ADDRESS             0xFEC40000
#define PC08_INTERRUPT_BASE             88

#define PC09_IOAPIC                     (1 << (9 + PCH_IOAPIC))
#define PC09_IOAPIC_ID                  0x12
#define PC09_IOAPIC_ADDRESS             0xFEC48000
#define PC09_INTERRUPT_BASE             96

#define PC10_IOAPIC                     (1 << (10 + PCH_IOAPIC))
#define PC10_IOAPIC_ID                  0x13
#define PC10_IOAPIC_ADDRESS             0xFEC50000
#define PC10_INTERRUPT_BASE             104

#define PC11_IOAPIC                     (1 << (11 + PCH_IOAPIC))
#define PC11_IOAPIC_ID                  0x14
#define PC11_IOAPIC_ADDRESS             0xFEC58000
#define PC11_INTERRUPT_BASE             112

#define PC12_IOAPIC                     (1 << (12 + PCH_IOAPIC))
#define PC12_IOAPIC_ID                  0x15
#define PC12_IOAPIC_ADDRESS             0xFEC60000
#define PC12_INTERRUPT_BASE             120

#define PC13_IOAPIC                     (1 << (13 + PCH_IOAPIC))
#define PC13_IOAPIC_ID                  0x16
#define PC13_IOAPIC_ADDRESS             0xFEC68000
#define PC13_INTERRUPT_BASE             128

#define PC14_IOAPIC                     (1 << (14 + PCH_IOAPIC))
#define PC14_IOAPIC_ID                  0x17
#define PC14_IOAPIC_ADDRESS             0xFEC70000
#define PC14_INTERRUPT_BASE             136

#define PC15_IOAPIC                     (1 << (15 + PCH_IOAPIC))
#define PC15_IOAPIC_ID                  0x18
#define PC15_IOAPIC_ADDRESS             0xFEC78000
#define PC15_INTERRUPT_BASE             144

#define PC16_IOAPIC                     (1 << (16 + PCH_IOAPIC))
#define PC16_IOAPIC_ID                  0x19
#define PC16_IOAPIC_ADDRESS             0xFEC80000
#define PC16_INTERRUPT_BASE             152

#define PC17_IOAPIC                     (1 << (17 + PCH_IOAPIC))
#define PC17_IOAPIC_ID                  0x1A
#define PC17_IOAPIC_ADDRESS             0xFEC88000
#define PC17_INTERRUPT_BASE             160

#define PC18_IOAPIC                     (1 << (18 + PCH_IOAPIC))
#define PC18_IOAPIC_ID                  0x1B
#define PC18_IOAPIC_ADDRESS             0xFEC90000
#define PC18_INTERRUPT_BASE             168

#define PC19_IOAPIC                     (1 << (19 + PCH_IOAPIC))
#define PC19_IOAPIC_ID                  0x1C
#define PC19_IOAPIC_ADDRESS             0xFEC98000
#define PC19_INTERRUPT_BASE             176

#define PC20_IOAPIC                     (1 << (20 + PCH_IOAPIC))
#define PC20_IOAPIC_ID                  0x1D
#define PC20_IOAPIC_ADDRESS             0xFECA0000
#define PC20_INTERRUPT_BASE             184

#define PC21_IOAPIC                     (1 << (21 + PCH_IOAPIC))
#define PC21_IOAPIC_ID                  0x1E
#define PC21_IOAPIC_ADDRESS             0xFECA8000
#define PC21_INTERRUPT_BASE             192

#define PC22_IOAPIC                     (1 << (22 + PCH_IOAPIC))
#define PC22_IOAPIC_ID                  0x1F
#define PC22_IOAPIC_ADDRESS             0xFECB0000
#define PC22_INTERRUPT_BASE             200

#define PC23_IOAPIC                     (1 << (23 + PCH_IOAPIC))
#define PC23_IOAPIC_ID                  0x20
#define PC23_IOAPIC_ADDRESS             0xFECB8000
#define PC23_INTERRUPT_BASE             208

#define PC24_IOAPIC                     (1 << (24 + PCH_IOAPIC))
#define PC24_IOAPIC_ID                  0x21
#define PC24_IOAPIC_ADDRESS             0xFECC0000
#define PC24_INTERRUPT_BASE             216

#define PC25_IOAPIC                     (1 << (25 + PCH_IOAPIC))
#define PC25_IOAPIC_ID                  0x22
#define PC25_IOAPIC_ADDRESS             0xFECC8000
#define PC25_INTERRUPT_BASE             224

#define PC26_IOAPIC                     (1 << (26 + PCH_IOAPIC))
#define PC26_IOAPIC_ID                  0x23
#define PC26_IOAPIC_ADDRESS             0xFECD0000
#define PC26_INTERRUPT_BASE             232

#define PC27_IOAPIC                     (1 << (27 + PCH_IOAPIC))
#define PC27_IOAPIC_ID                  0x24
#define PC27_IOAPIC_ADDRESS             0xFECD8000
#define PC27_INTERRUPT_BASE             240

#define PC28_IOAPIC                     (1 << (28 + PCH_IOAPIC))
#define PC28_IOAPIC_ID                  0x25
#define PC28_IOAPIC_ADDRESS             0xFECE0000
#define PC28_INTERRUPT_BASE             248

#define PC29_IOAPIC                     (1 << (29 + PCH_IOAPIC))
#define PC29_IOAPIC_ID                  0x26
#define PC29_IOAPIC_ADDRESS             0xFECE8000
#define PC29_INTERRUPT_BASE             256

#define PC30_IOAPIC                     (UINT32)(1 << (30 + PCH_IOAPIC))	// Avoid compiler warning!
#define PC30_IOAPIC_ID	                0x27
#define PC30_IOAPIC_ADDRESS             0xFECF0000
#define PC30_INTERRUPT_BASE				264

#define PC31_IOAPIC                     (UINT32)(1 << 31)	// Note: Make sure not overflow (8S only where PCH_IOAPIC = 0). Avoid compiler warning!
#define PC31_IOAPIC_ID	                0x28
#define PC31_IOAPIC_ADDRESS             0xFECF8000  // MAX IOAPIC LIMIT = 0xFECFFFFF 
#define PC31_INTERRUPT_BASE				272

//
// Define platform base
// Note: All the PCH devices must get Legacy IO resources within first 16KB 
// since QPIRC allcoates range 0-16KB for the legacy IIO.
//
#define BMC_KCS_BASE_ADDRESS            0x0CA0
#define BMC_KCS_BASE_MASK               0xFFF0
#define PCH_ACPI_BASE_ADDRESS           PcdGet16 (PcdPchAcpiIoPortBaseAddress)      // ACPI Power Management I/O Register Base Address

#define PCH_TCO_BASE_ADDRESS            PcdGet16 (PcdTcoBaseAddress)
#define PCH_PWRM_BASE_ADDRESS           0xFE000000
#define PCH_PWRM_MMIO_SIZE              0x00010000

// APTIOV_SERVER_OVERRIDE_RC_START
// Use SDL token for PM base address.
#ifndef PM_BASE_ADDRESS
#define PM_BASE_ADDRESS                 PCH_ACPI_BASE_ADDRESS
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
#define PCH_ACPI_TIMER_ADDRESS          (PCH_ACPI_BASE_ADDRESS + 0x08)
#define PCH_ACPI_TIMER_MAX_VALUE        0x1000000   // The timer is 24 bit overflow

#define PCH_TBARB_ADDRESS               0xFED08000  // 4K for Pch Thermal Sensor
#define HPET_BASE_ADDRESS	              0xFED00000
// APTIOV_SERVER_OVERRIDE_RC_START
#ifndef SIO_GPIO_BASE_ADDRESS
#define SIO_GPIO_BASE_ADDRESS           0x0800
#endif
// APTIOV_SERVER_OVERRIDE_RC_END

//
// Local and I/O APIC addresses.
//
#define EFI_APIC_LOCAL_ADDRESS          0xFEE00000
#define LOCAL_APIC_ADDRESS              0xFEE00000

//
// Constant definition
//
#define  MAX_SMRAM_RANGES    4
//
// This structure stores the base and size of the ACPI reserved memory used when
// resuming from S3.  This region must be allocated by the platform code.
//
typedef struct {
  UINT32  AcpiReservedMemoryBase;
  UINT32  AcpiReservedMemorySize;
  UINT32  SystemMemoryLength;
} RESERVED_ACPI_S3_RANGE;

#define RESERVED_ACPI_S3_RANGE_OFFSET (EFI_PAGE_SIZE - sizeof (RESERVED_ACPI_S3_RANGE))

//
// SMBUS Data
//
#define PCH_SMBUS_BUS_DEV_FUNC          0x1F0300
 // APTIOV_SERVER_OVERRIDE_RC_START : Program GPIO, PMBASE and SMBUS base address from SDL values                            
//#define PCH_SMBUS_BASE_ADDRESS          0x0780
#define PCH_SMBUS_BASE_ADDRESS          SMBS
// APTIOV_SERVER_OVERRIDE_RC_END : Program GPIO, PMBASE and SMBUS base address from SDL values                            
#define PLATFORM_NUM_SMBUS_RSVD_ADDRESSES   4
#define SMBUS_ADDR_CH_A_1                   0xA0
#define SMBUS_ADDR_CH_A_2                   0xA2
#define SMBUS_ADDR_CH_B_1                   0xA4
#define SMBUS_ADDR_CH_B_2                   0xA6
#define SMBUS_IO_EXPANDER_ADDRESS           0x38

//
// CMOS usage
//
#define CMOS_CPU_BSP_SELECT             0x10
#define CMOS_CPU_UP_MODE                0x11
#define CMOS_CPU_RATIO_OFFSET           0x12

#define CMOS_CPU_CORE_HT_OFFSET         0x13
#define CMOS_CPU1_CORE_HT_OFFSET        0x14
#define CMOS_CPU2_CORE_HT_OFFSET        0x15
#define CMOS_CPU3_CORE_HT_OFFSET        0x16

#define CMOS_CPU0_CORE_DISABLE       	 0x1B            //Processor0Core Offset
#define CMOS_CPU1_CORE_DISABLE           0x1C            //Processor1Core Offset
#define CMOS_CPU2_CORE_DISABLE           0x1D            //Processor2Core Offset
#define CMOS_CPU3_CORE_DISABLE           0x1E            //Processor3Core Offset
#define CMOS_CPUx_CORE8_9_DISABLE        0x1F            //Processor[i]Core[9:8] Offset, Proc[i].Core[9:8] at this CMOS bits[x+1:x], where x = i * 2

// Second bank
//
#define CMOS_CPU_HT_OFFSET              0x14             //Processor Hyperthreading Offset
#define CMOS_CPU_BIST_OFFSET            0x15
#define CMOS_CPU_VMX_OFFSET             0x16
#define CMOS_EFI_DEBUG                  0x17
#define CMOS_PLATFORM_ID_LO             0x18            // Second bank CMOS location of Platform ID
#define CMOS_PLATFORM_ID_HI             0x19            //
#define CMOS_PCH_PORT80_OFFSET          0x1A
#define CMOS_WHEA_OS_ENABLE             0x1B            // Must match WIWH definition in Whea.asi
#define TCG_CMOS_AREA_OFFSET            0x60                          // Also Change in Universal\Security\Tpm\PhysicalPresence\Dxe\PhysicalPresence.c &
                                                                      // PlatformPkg AcpiTables\Dsdt\Tpm.asi
#define TCG_CMOS_MOR_AREA_OFFSET        (TCG_CMOS_AREA_OFFSET + 0x05) // Also Change in // PlatformPkg AcpiTables\Dsdt\Tpm.asi

#define CMOS_DEBUG_PRINT_LEVEL_REG      0x4C             // Debug Mask saved in CMOS
#define CMOS_DEBUG_PRINT_LEVEL_1_REG    0x4D
#define CMOS_DEBUG_PRINT_LEVEL_2_REG    0x4E
#define CMOS_DEBUG_PRINT_LEVEL_3_REG    0x4F


#define CMOS_WARM_RESET_COUNTER_OFFSET       0xBD    // 1 byte CMOS Space for passing warm reset counter to Dxe
                                                     //   due to reset in MRC Dxe always thinks that warm reset occurs
                                                     //   counter > 1 -> means WarmReset
#define CMOS_DRAM_SHARED_MAILBOX_ADDR_REG    0xF0    // 2 bytes CMOS Space for the DRAM Share Mailbox address [31:16]

//
// GPIO Index Data Structure
//
typedef struct {
  UINT32  Value;
  UINT8   Register;
} PCH_GPIO_INDEX_DATA;

//
// Define platform type register
// feature space location for GPI register in FWH
//
#define FWH_GPI_BASE_ADDRESS  0xFFBC0100
#define PLATFORM_TYPE_REG     (*(UINT8 volatile *) FWH_GPI_BASE_ADDRESS)

//
// COM (Serial) Port Base address
//
#define  COM1_BASE                      0x3F8
#define  COM2_BASE                      0x2F8

//
// LPT (Parallel) Port Base address
//
#define  LPT1_BASE                      0x378


#define MEM64_LEN                       0x00100000000
#define RES_MEM64_39_BASE               0x08000000000 - MEM64_LEN   // 2^39
#define RES_MEM64_39_LIMIT              0x08000000000 - 1           // 2^39
#define RES_MEM64_40_BASE               0x10000000000 - MEM64_LEN   // 2^40
#define RES_MEM64_40_LIMIT              0x10000000000 - 1           // 2^40

//
// Misc. platform definitions
//
#define CPU_GENERIC_UNCORE_DEV_FUNC     0
#define PCI_REVISION_ID_REG             8

//
// Need min. of 24 MB PEI phase
//
#define PEI_MIN_MEMORY_SIZE             (EFI_PHYSICAL_ADDRESS) ((320 * 0x100000))

#define HPET_BLOCK_ADDRESS                            0x0FED00000

#define PCI_BUS_NUMBER_PCH_HPET                       0x0
#define PCI_DEVICE_NUMBER_PCH_HPET                    0x1F

#define PCI_FUNCTION_NUMBER_PCH_HPET0                 0x00
#define PCI_FUNCTION_NUMBER_PCH_HPET1                 0x01
#define PCI_FUNCTION_NUMBER_PCH_HPET2                 0x02
#define PCI_FUNCTION_NUMBER_PCH_HPET3                 0x03
#define PCI_FUNCTION_NUMBER_PCH_HPET4                 0x04
#define PCI_FUNCTION_NUMBER_PCH_HPET5                 0x05
#define PCI_FUNCTION_NUMBER_PCH_HPET6                 0x06
#define PCI_FUNCTION_NUMBER_PCH_HPET7                 0x07
#define R_PCH_LPC_H0BDF                               0x70
#define R_PCH_LPC_H1BDF                               0x72
#define R_PCH_LPC_H2BDF                               0x74
#define R_PCH_LPC_H3BDF                               0x76
#define R_PCH_LPC_H4BDF                               0x78
#define R_PCH_LPC_H5BDF                               0x7A
#define R_PCH_LPC_H6BDF                               0x7C
#define R_PCH_LPC_H7BDF                               0x7E

#define PCI_BUS_NUMBER_PCH_IOAPIC                     0xF0
#define PCI_DEVICE_NUMBER_PCH_IOAPIC                  0x1F

#define PCI_FUNCTION_NUMBER_PCH_IOAPIC                0x0

#define PXE_BASE_OPTION_ROM_FILE_GUID \
  { \
    0x6f2bc426, 0x8d83, 0x4f17, 0xbe, 0x2c, 0x6b, 0x1f, 0xcb, 0xd1, 0x4c, 0x80 \
  }

//
// LT Equates
//
#ifdef LT_FLAG
  #define ACM_BASE                      FLASH_REGION_BIOS_ACM_BASE
  #define ACM_SIZE                      0x00020000
  #define LT_PLATFORM_DPR_SIZE          0x00600000
// APTIOV_SERVER_OVERRIDE_RC_START - USING AMI SDL TOKEN
//  #define CMOS_LTSX_OFFSET              0x2A
  #define CMOS_LTSX_OFFSET                LTSX_TXT_CMOS_ADDRESS
// APTIOV_SERVER_OVERRIDE_RC_END -  - USING AMI SDL TOKEN
  #define ACM_VERSION_OFFSET            0x4E4    //Acm Architectural Header (0x284) + Scratch Area 4*(2*64 + 15) + Offset 0x24
#endif

//EDK2_TODO Need to check with CCB to see if this can be moved to PiBootmode.h
#define BOOT_WITH_MFG_MODE_SETTINGS                   0x07


#define SW_SMI_OS_REQUEST		  0x83  // OS transition request.
#define MEM_ADDR_SHFT_VAL         26    // For 64 MB granularity

#endif
