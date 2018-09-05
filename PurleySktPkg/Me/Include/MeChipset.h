/**
/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2010 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MeChipset.h

  Chipset definition for ME Devices.

**/
#ifndef _ME_CHIPSET_H_
#define _ME_CHIPSET_H_

#include "Library/MeChipsetLib.h"

#define ME_BUS                0
#define ME_DEVICE_NUMBER      22
#define HECI_MIN_FUNC         0
#define HECI_MAX_FUNC         5

#define HECI_FUNCTION_NUMBER  0x00
#define HECI2_FUNCTION_NUMBER 0x01
#define IDER_FUNCTION_NUMBER  0x02
#define SOL_FUNCTION_NUMBER   0x03
#define HECI3_FUNCTION_NUMBER 0x04

#define SOL_BUS_NUMBER        ME_BUS
#define SOL_DEVICE_NUMBER     ME_DEVICE_NUMBER
#define HECI_DEV_FCN          ((ME_DEVICE_NUMBER) << 3 | (HECI_FUNCTION_NUMBER))
#define HECI2_DEV_FCN         ((ME_DEVICE_NUMBER) << 3 | (HECI2_FUNCTION_NUMBER))
#define SOL_DEV_FCN           ((ME_DEVICE_NUMBER) << 3 | (SOL_FUNCTION_NUMBER))

typedef enum {
  HECI1_DEVICE = 0,
  HECI2_DEVICE = 1,
  HECI3_DEVICE = 4
} HECI_DEVICE;

#define HECI1_PEI_DEFAULT_MBAR 0xFEDB0000
#define HECI2_PEI_DEFAULT_MBAR 0xFEDC0000

///
/// Convert to HECI# defined in BWG from Fun#
///
#define HECI_NAME_MAP(a) ((a < 2) ? (a + 1) : 3)

//
// ME PCI Device IDs
//
#define V_INTEL_VENDOR_ID         0x8086
#define V_ME_HECI_VENDOR_ID       V_INTEL_VENDOR_ID
#define V_ME_SOL_VENDOR_ID        V_INTEL_VENDOR_ID

#define V_ME_SPTLP_HECI_DEVICE_ID   0x9D3A
#define V_ME_SPTLP_HECI2_DEVICE_ID  0x9D3B
#define V_ME_SPTLP_HECI3_DEVICE_ID  0x9D3E
#define V_ME_SPTLP_SOL_DEVICE_ID    0x9D3D

#define V_ME_SPTH_HECI_DEVICE_ID   0xA13A
#define V_ME_SPTH_HECI2_DEVICE_ID  0xA13B
#define V_ME_SPTH_HECI3_DEVICE_ID  0xA13E
#define V_ME_SPTH_SOL_DEVICE_ID    0xA13D
#define V_ME_LBG_PROD_SKU_HECI_DEVICE_ID   0xA1BA
#define V_ME_LBG_SUPER_SKU_HECI_DEVICE_ID  0xA23A
#define V_ME_LBG_PROD_SKU_HECI2_DEVICE_ID  0xA1BB
#define V_ME_LBG_SUPER_SKU_HECI2_DEVICE_ID 0xA23B
#define V_ME_LBG_PROD_SKU_HECI3_DEVICE_ID  0xA1BE
#define V_ME_LBG_SUPER_SKU_HECI3_DEVICE_ID 0xA23E

#define V_ME_LBG_SOL_PROD_SKU_DEVICE_ID    0xA1BD
#define V_ME_LBG_SOL_SUPER_SKU_DEVICE_ID   0xA23D


#define PCH_USB_DEVICE_ID_11      0x3B3F  // UHCI#9
#define PCH_USB_DEVICE_ID_12      0x3B40  // UHCI#8
#define IDER_VENDOR_ID            V_INTEL_VENDOR_ID
#define SOL_VENDOR_ID             V_INTEL_VENDOR_ID
#define SOL_DEVICE_ID             KT_DEVICE_ID
#define KT_VENDOR_ID              V_INTEL_VENDOR_ID
#define HECI_VID                  V_INTEL_VENDOR_ID
#define USBR_VENDOR_ID            V_INTEL_VENDOR_ID

#define HECI_DID                  PCH_HECI_DID
#define HECI2_DID                 PCH_HECI2_DID
#define IDER_DEVICE_ID            PCH_IDER_DEVICE_ID
#define KT_DEVICE_ID              PCH_SOL_DEVICE_ID

#define R_ME_HFS                  0x40
#define R_ME_MISC_SHDW            0x44
#define R_ME_GS_SHDW              0x48
#define R_ME_PMCSR                0x54
#define V_ME_PMCSR                0x03
#define R_ME_H_GS                 0x4C
#define R_ME_HFS_3                0x60
#define R_ME_HFS_4                0x64
#define R_ME_HFS_5                0x68
#define R_ME_HFS_6                0x6C
#define B_BOOT_GUARD_ENF_MASK     0x0200
#define B_TPM_DISCONNECT          0x1000
#define B_TPM1_2_DEACTIVATED      0x0100

#define R_ME_H_GS                 0x4C
#define B_ME_DID_RAPID_START_BIT  BIT23
#define B_ME_DID_TYPE_MASK        BIT28
#define R_ME_H_GS2                0x70
#define B_ME_MBP_GIVE_UP          BIT0
#define R_ME_HIDM                 0xA0
#define V_ME_HIDM_MSI             0
#define V_ME_HIDM_SCI             1
#define V_ME_HIDM_SMI             2
#define V_ME_HIDM_LOCK            4
#define R_ME_HERS                 0xBC
#define B_ME_EXTEND_REG_VALID     BIT31
#define B_EXTEND_REG_ALGORITHM    (BIT0 | BIT1 | BIT2 | BIT3)
#define V_ME_SHA_1                0x00
#define V_ME_SHA_256              0x02

#define B_HOST_RESET_WARN_TYPE_MASK BIT31

#define R_ME_HER1                 0xC0
#define R_ME_HER2                 0xC4
#define R_ME_HER3                 0xC8
#define R_ME_HER4                 0xCC
#define R_ME_HER5                 0xD0
#define R_ME_HER6                 0xD4
#define R_ME_HER7                 0xD8
#define R_ME_HER8                 0xDC

#define V_ME_SPTLP_USBR1_PORT_NUMBER 0xA
#define V_ME_SPTLP_USBR2_PORT_NUMBER 0xB
#define V_ME_SPTH_USBR1_PORT_NUMBER  0xE
#define V_ME_SPTH_USBR2_PORT_NUMBER  0xF

#ifndef BIT0
//
// bit definition
//
#define BIT0  (1 << 0)
#define BIT1  (1 << 1)
#define BIT2  (1 << 2)
#define BIT3  (1 << 3)
#define BIT4  (1 << 4)
#define BIT5  (1 << 5)
#define BIT6  (1 << 6)
#define BIT7  (1 << 7)
#define BIT8  (1 << 8)
#define BIT9  (1 << 9)
#define BIT10 (1 << 10)
#define BIT11 (1 << 11)
#define BIT12 (1 << 12)
#define BIT13 (1 << 13)
#define BIT14 (1 << 14)
#define BIT15 (1 << 15)
#define BIT16 (1 << 16)
#define BIT17 (1 << 17)
#define BIT18 (1 << 18)
#define BIT19 (1 << 19)
#define BIT20 (1 << 20)
#define BIT21 (1 << 21)
#define BIT22 (1 << 22)
#define BIT23 (1 << 23)
#define BIT24 (1 << 24)
#define BIT25 (1 << 25)
#define BIT26 (1 << 26)
#define BIT27 (1 << 27)
#define BIT28 (1 << 28)
#define BIT29 (1 << 29)
#define BIT30 (1 << 30)
#define BIT31 (1 << 31)
#define BIT32 0x100000000
#endif
//
// ME-related Chipset Definition
//
#define HeciEnable()    MeDeviceControl (HECI1, Enabled);
#define Heci2Enable()   MeDeviceControl (HECI2, Enabled);
#define Heci3Enable()   MeDeviceControl (HECI3, Enabled);
#define IderEnable()    MeDeviceControl (IDER, Enabled);
#define SolEnable()     MeDeviceControl (SOL, Enabled);
#define Usbr1Enable()   MeDeviceControl (USBR1, Enabled);
#define Usbr2Enable()   MeDeviceControl (USBR2, Enabled);

#define Heci1Disable()  MeDeviceControl (HECI1, Disabled);
#define Heci2Disable()  MeDeviceControl (HECI2, Disabled);
#define Heci3Disable()  MeDeviceControl (HECI3, Disabled);
#define IderDisable()   MeDeviceControl (IDER, Disabled);
#define SolDisable()    MeDeviceControl (SOL, Disabled);
#define Usbr1Disable()  MeDeviceControl (USBR1, Disabled);
#define Usbr2Disable()  MeDeviceControl (USBR2, Disabled);
#define DisableAllMeDevices() \
  Heci1Disable (); \
  Heci2Disable (); \
  Heci3Disable (); \
  IderDisable (); \
  SolDisable ();

#define R_PCH_RCRB_FUNC_EN          0x3590
#define B_PCH_RCRB_FUNC_EN_USBR2    BIT0                          // EHCI #2 USB-R (in device 26)
#define B_PCH_RCRB_FUNC_EN_USBR1    BIT1                          // EHCI #1 USB-R (in device 29)
#define R_PCH_RCRB_FUNC_DIS2        0x3428
#define B_PCH_RCRB_FUNC_DIS2_MEF3D  BIT4                          // Me Function 3 Disable
#define B_PCH_RCRB_FUNC_DIS2_MEF2D  BIT3                          // Me Function 2 Disable
#define B_PCH_RCRB_FUNC_DIS2_MEF1D  BIT2                          // Me Function 1 Disable
#define B_PCH_RCRB_FUNC_DIS2_MEF0D  BIT1                          // Me Function 0 Disable
#define B_PCH_RCRB_FUNC_DIS2_DBDFEN BIT0                          // Display BDF Enable
/*
#define R_PCH_RCRB_SPI_BASE         0x3800                        // Base address of the SPI host interface registers
#define R_PCH_SPI_BFPR              (R_PCH_RCRB_SPI_BASE + 0x00)  // BIOS Flash Primary Region Register(32bits), which is RO and contains the same value from FREG1
#define R_PCH_SPI_HSFS              (R_PCH_RCRB_SPI_BASE + 0x04)  // Hardware Sequencing Flash Status Register(16bits)
#define B_PCH_SPI_HSFS_FLOCKDN      BIT15                         // Flash Configuration Lock-Down
#define B_PCH_SPI_HSFS_FDOPSS       BIT13                         // Flash Descriptor Override Pin-Strap Status, 0: Override strap is set; 1: No override.OverrideCpuData
#define R_PCH_SPI_FRAP              (R_PCH_RCRB_SPI_BASE + 0x50)  // SPI Flash Regions Access Permisions Register
#define B_PCH_SPI_FRAP_BMWAG_MASK   0xFF000000                    // Master Write Access Grant MASK
#define B_PCH_SPI_FRAP_BMWAG_BIOS   BIT25                         // Master write access grant for Host CPU/BIOS
#define B_PCH_SPI_FRAP_BMWAG_ME     BIT26                         // Master write access grant for ME
#define B_PCH_SPI_FRAP_BMWAG_GBE    BIT27                         // Master write access grant for Host CPU/GbE
#define B_PCH_SPI_FRAP_BMRAG_MASK   0x00FF0000                    // Master Write Access Grant MASK
#define B_PCH_SPI_FRAP_BMRAG_BIOS   BIT17                         // Master write access grant for Host CPU/BIOS
#define B_PCH_SPI_FRAP_BMRAG_ME     BIT18                         // Master write access grant for ME
#define B_PCH_SPI_FRAP_BMRAG_GBE    BIT19                         // Master write access grant for Host CPU/GbE
#define B_PCH_SPI_FRAP_BRWA_MASK    0x0000FF00                    // BIOS Regsion Write Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: ...
#define B_PCH_SPI_FRAP_BRWA_FLASHD  BIT8                          // Region write access for Region0 Flash Descriptor
#define B_PCH_SPI_FRAP_BRWA_BIOS    BIT9                          // Region write access for Region1 BIOS
#define B_PCH_SPI_FRAP_BRWA_ME      BIT10                         // Region write access for Region2 ME
#define B_PCH_SPI_FRAP_BRWA_GBE     BIT11                         // Region write access for Region3 GbE
#define B_PCH_SPI_FRAP_BRRA_MASK    0x000000FF                    // BIOS Regsion Read Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: ...
#define B_PCH_SPI_FRAP_BRRA_FLASHD  BIT0                          // Region read access for Region0 Flash Descriptor
#define B_PCH_SPI_FRAP_BRRA_BIOS    BIT1                          // Region read access for Region1 BIOS
#define B_PCH_SPI_FRAP_BRRA_ME      BIT2                          // Region read access for Region2 ME
#define B_PCH_SPI_FRAP_BRRA_GBE     BIT3                          // Region read access for Region3 GbE
#define R_PCH_SPI_FREG2_ME          (R_PCH_RCRB_SPI_BASE + 0x5C)  // Flash Region 2(ME)(32bits)
//#define B_PCH_SPI_FREG2_LIMIT_MASK  0x1FFF0000                    // Size, [28:16] here represents limit[24:12]
//#define B_PCH_SPI_FREG2_BASE_MASK   0x00001FFF                    // Base, [12:0]  here represents base [24:12]
#define R_PCH_SPI_PR0               (R_PCH_RCRB_SPI_BASE + 0x74)  // Protected Region 0 Register
#define B_PCH_SPI_PR0_WPE           BIT31                         // Write Protection Enable
//#define B_PCH_SPI_PR0_PRL_MASK      0x1FFF0000                    // Protected Range Limit Mask, [28:16] here represents upper limit of address [24:12],
#define B_PCH_SPI_PR0_RPE           BIT15                         // Read Protection Enable
//#define B_PCH_SPI_PR0_PRB_MASK      0x00001FFF                    // Protected Range Base Mask, [12:0] here represents base limit of address [24:12],
*/

//
// PCH-related registers
//
#ifndef _PCH_ACCESS_H_
#define PCI_BUS_NUMBER_PCH  0x00

#ifndef _PCH_REGS_LPC_H_
#define PCI_DEVICE_NUMBER_PCH_LPC   31
#define PCI_FUNCTION_NUMBER_PCH_LPC 0

//
// PM PCI registers
//
#define R_PCH_LPC_ACPI_BASE             0x40
#define B_PCH_LPC_ACPI_BASE_BAR         0xFFFC
#define B_PCH_LPC_ACPI_BASE_EN          BIT0

#define R_PCH_LPC_ACPI_CNT              0x44
#define B_PCH_LPC_ACPI_CNT_ACPI_EN      0x80
#define B_PCH_LPC_ACPI_CNT_SCI_IRG_SEL  0x07

#define R_PCH_OC_WDT_CTL                0x54
#define B_PCH_OC_WDT_CTL_RLD            BIT31
#define B_PCH_OC_WDT_CTL_ICCSURV_STS    BIT25
#define B_PCH_OC_WDT_CTL_NO_ICCSURV_STS BIT24
#define B_PCH_OC_WDT_CTL_EN             BIT14
#define B_PCH_OC_WDT_CTL_ICCSURV        BIT13
#define B_PCH_OC_WDT_CTL_LCK            BIT12
#define B_PCH_OC_WDT_CTL_TOV_MASK       0x3FF
#define B_PCH_OC_WDT_CTL_FAILURE_STS    BIT23
#define B_PCH_OC_WDT_CTL_UNXP_RESET_STS BIT22
#define B_PCH_OC_WDT_CTL_AFTER_POST     0x3F0000

#define R_PCH_LPC_PM_INIT               0xAC
#define B_PCH_LPC_PMIR_FIELD_1          (BIT31 | BIT30) // PMIR Field 1
#define B_PCH_LPC_PMIR_FIELD_0          0x0FFFFF        // PMIR Field 0
#define B_PCH_LPC_GLOBAL_RESET_CF9      0x00100000
#define B_PCH_LPC_GLOBAL_RESET_LOCK     0x80000000

#define R_PCH_LPC_GEN_PMCON_2                     0xA2
#define B_PCH_LPC_GEN_PMCON_PWROK_FLR             0x01
#define B_PCH_LPC_GEN_PMCON_SYSPWR_FLR            0x02
#define B_PCH_LPC_GEN_PMCON_MIN_SLP_S4            0x04
#define B_PCH_LPC_GEN_PMCON_CTS                   0x08
#define B_PCH_LPC_GEN_PMCON_SRS                   0x10
#define B_PCH_LPC_GEN_PMCON_MEM_SR                0x20
#define B_PCH_LPC_GEN_PMCON_DRAM_INIT             0x80
#define B_PCH_LPC_GEN_PMCON_SX_PP_EN              0x0800
#define B_PCH_LPC_GEN_PMCON_AG3_PP_EN             0x1000
#define B_PCH_LPC_GEN_PMCON_DSX_PP_DIS            0x2000
#define B_PCH_LPC_GEN_PMCON_DC_PP_DIS             0x4000

#define R_PCH_LPC_BIOS_CNTL             0xDC
#define S_PCH_LPC_BIOS_CNTL             1
#define V_PCH_SRC_PREF_EN_CACHE_EN      0x08
#define V_PCH_SRC_PREF_DIS_CACHE_DIS    0x04
#define V_PCH_SRC_PREF_DIS_CACHE_EN     0x00

#endif // _PCH_REGS_LPC_H_
//
// PM Base
//
#define R_ACPI_PM1_STS  0x00
#define S_ACPI_PM1_STS  2
#define B_WAK_STS       0x8000
#define B_PRBTNOR_STS   0x0800
#define B_RTC_STS       0x0400
#define B_ME_STS        0x0200
#define B_PWRBTN_STS    0x0100
#define B_GBL_STS       0x0020
#define B_BM_STS        0x0010
#define B_TMROF_STS     0x0001
#define N_WAK_STS       15
#define N_PRBTNOR_STS   11
#define N_RTC_STS       10
#define N_ME_STS        9
#define N_PWRBTN_STS    8
#define N_GBL_STS       5
#define N_BM_STS        4
#define N_TMROF_STS     0

#define R_ACPI_PM1_CNT  0x04
#define S_ACPI_PM1_CNT  4
#define B_SLP_EN        0x00002000
#define B_SLP_TYP       0x00001C00
#define V_S0            0x00000000
#define V_S1_DESK       0x00000400
#define V_S1_MOBL       0x00000400
#define V_S3            0x00001400
#define V_S4            0x00001800
#define V_S5            0x00001C00
#define B_GBL_RLS       0x00000004
#define B_BM_RLD        0x00000002
#define B_SCI_EN        0x00000001

#define R_ACPI_PM1_TMR  0x08

#define R_ACPI_GPE0_EN  0x28
//
// HPTC
//
#define R_PCH_RCRB_HPTC     0x3404        // High Performance Timer Configuration
#define B_PCH_RCRB_HPTC_AS  (BIT1 | BIT0) // Address selection
#define B_PCH_RCRB_HPTC_AE  BIT7          // Address enable
#endif // _PCH_ACCESS_H_


#define IS_PCH_SPTH_HECI_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTH_HECI_DEVICE_ID) \
     )

#define IS_PCH_SPTLP_HECI_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTLP_HECI_DEVICE_ID) \
     )

#define IS_PCH_SPT_HECI_DEVICE_ID(DeviceId) \
     ( \
      IS_PCH_SPTH_HECI_DEVICE_ID(DeviceId) || \
      IS_PCH_SPTLP_HECI_DEVICE_ID(DeviceId) \
     )

#define IS_PCH_SPTH_HECI2_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTH_HECI2_DEVICE_ID) \
     )

#define IS_PCH_SPTLP_HECI2_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTLP_HECI2_DEVICE_ID) \
     )

#define IS_PCH_SPT_HECI2_DEVICE_ID(DeviceId) \
     ( \
      IS_PCH_SPTH_HECI2_DEVICE_ID(DeviceId) || \
      IS_PCH_SPTLP_HECI2_DEVICE_ID(DeviceId) \
     )

#define IS_PCH_SPTH_HECI3_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTH_HECI3_DEVICE_ID) \
     )

#define IS_PCH_SPTLP_HECI3_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTLP_HECI3_DEVICE_ID) \
     )

#define IS_PCH_SPT_HECI3_DEVICE_ID(DeviceId) \
     ( \
      IS_PCH_SPTH_HECI3_DEVICE_ID(DeviceId) || \
      IS_PCH_SPTLP_HECI3_DEVICE_ID(DeviceId) \
     )

#define IS_PCH_SPTH_SOL_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTH_SOL_DEVICE_ID) \
     )

#define IS_PCH_LBG_SOL_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_LBG_SOL_SUPER_SKU_DEVICE_ID) || \
      (DeviceId == V_ME_LBG_SOL_PROD_SKU_DEVICE_ID) \
     )

#define IS_PCH_SPTLP_SOL_DEVICE_ID(DeviceId) \
     (  \
      (DeviceId == V_ME_SPTLP_SOL_DEVICE_ID) \
     )

#define IS_PCH_SPT_SOL_DEVICE_ID(DeviceId) \
     ( \
      IS_PCH_LBG_SOL_DEVICE_ID(DeviceId)  || \
      IS_PCH_SPTH_SOL_DEVICE_ID(DeviceId) || \
      IS_PCH_SPTLP_SOL_DEVICE_ID(DeviceId) \
     )

#define IS_PCH_SOL_DEVICE_ID(DeviceId) \
     ( \
      IS_PCH_SPT_SOL_DEVICE_ID(DeviceId) \
     )

///
/// Function Disable SUS well lockdown
///
#define FunctionDisableWellLockdown() MeDeviceControl (FDSWL, Enabled);

#endif // _ME_CHIPSET_H_
