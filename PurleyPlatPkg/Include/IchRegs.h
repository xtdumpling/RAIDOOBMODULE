//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 1999 - 2007, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IchRegs.h

  Register names for ICH9 and ICH10.
  - Common registers are denoted with "_ICH_".
  - Registers that are different betweeen ICH9 and ICH10, ICH9 unique, or ICH10
    unique are denoted with "_ICH9_" or "_ICH10_".
  - If same bit / register name is used with different
    address, width, or bit posistions in different types, the type names are used,
    e.g.,  "_CORPORATE" and "_CONSUMER"

**/

#ifndef _ICH_REGS_H_
#define _ICH_REGS_H_

//
// Definitions beginning with "R_" are registers
// Definitions beginning with "B_" are bits within registers
// Definitions beginning with "V_" are meaningful values of bits within the registers
// Definitions beginning with "S_" are register sizes
// Definitions beginning with "N_" are the bit position
//

#define V_INTEL_VENDOR_ID                              0x8086
#define V_ICH_DEFAULT_SID                              0x7270
#define V_ICH_DEFAULT_SSID                             (V_INTEL_VENDOR_ID + (V_ICH_DEFAULT_SID << 16))

#define PCI_BUS_NUMBER_ICH                             0x00

//
// Macros that judge which ICH generation / type a device ID belongs to
//

//
// Any device ID that is ICH9 or ICH10
// ESS_OVERRIDE Added V_ICH10_LPC_DEVICE_ID_8
//
#define IS_ICH9_OR_ICH10_DEVICE_ID(DeviceId) ( \
     (DeviceId) == V_ICH_PCI_PCI_DEVICE_ID_0                  \
  || (DeviceId) == V_ICH_PCI_PCI_DEVICE_ID_1                  \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_1                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_2                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_3                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_4                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_5                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_6                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_7                     \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_2                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_3                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_4                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_5                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_6                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_7                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_8                    \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_IDE_4_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_IDE_2_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_AHCI_6_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_AHCI_4_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_RAID               \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_RAID_ALT           \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_IDE_2_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_AHCI_4_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_RAID               \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_RAID_ALT           \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_IDE_4_PORTS       \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_IDE_2_PORTS       \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_AHCI_6_PORTS      \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_AHCI_4_PORTS      \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_RAID              \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_RAID_ALT_1        \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_RAID_ALT_2        \
  || (DeviceId) == V_ICH9_SIDE2_DEVICE_ID_D_IDE_2_PORTS       \
  || (DeviceId) == V_ICH9_SIDE2_DEVICE_ID_M_IDE_2_PORTS       \
  || (DeviceId) == V_ICH10_SIDE2_DEVICE_ID_D_IDE_2_PORTS      \
  || (DeviceId) == V_ICH9_THERMAL_DEVICE_ID_0                 \
  || (DeviceId) == V_ICH10_THERMAL_DEVICE_ID_0                \
  || (DeviceId) == V_ICH9_SMBUS_DEVICE_ID                     \
  || (DeviceId) == V_ICH10_SMBUS_DEVICE_ID                    \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_1                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_2                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_3                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_4                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_5                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_6                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_7                     \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_2                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_3                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_4                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_5                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_6                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_6_1                  \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_7                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_7_1                  \
  || (DeviceId) == V_ICH9_LAN_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH10_LAN_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH9_HDA_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_HDA_DEVICE_ID_1                     \
  || (DeviceId) == V_ICH10_HDA_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH10_HDA_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_2                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_3                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_4                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_5                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_6                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_7                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_8                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_9                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_A                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_B                    \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_0                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_1                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_2                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_3                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_4                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_5                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_6                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_7                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_8                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_9                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_A                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_B                   \
  )

//
// Device ID that is ICH9 specific (not shared between ICH9 and ICH10)
//
#define IS_ICH9_DEVICE_ID(DeviceId) ( \
     (DeviceId) == V_ICH9_LPC_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_1                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_2                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_3                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_4                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_5                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_6                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_7                     \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_IDE_4_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_IDE_2_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_AHCI_6_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_AHCI_4_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_RAID               \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_RAID_ALT           \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_IDE_2_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_AHCI_4_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_RAID               \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_RAID_ALT           \
  || (DeviceId) == V_ICH9_SIDE2_DEVICE_ID_D_IDE_2_PORTS       \
  || (DeviceId) == V_ICH9_SIDE2_DEVICE_ID_M_IDE_2_PORTS       \
  || (DeviceId) == V_ICH9_THERMAL_DEVICE_ID_0                 \
  || (DeviceId) == V_ICH9_SMBUS_DEVICE_ID                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_1                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_2                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_3                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_4                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_5                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_6                     \
  || (DeviceId) == V_ICH9_USB_DEVICE_ID_7                     \
  || (DeviceId) == V_ICH9_LAN_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_HDA_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_HDA_DEVICE_ID_1                     \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_2                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_3                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_4                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_5                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_6                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_7                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_8                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_9                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_A                    \
  || (DeviceId) == V_ICH9_PCIE_DEVICE_ID_B                    \
  )

//
// Device ID that is ICH9 specific and Desktop specific
//
#define IS_ICH9_DEVICE_ID_DESKTOP(DeviceId) ( \
     (DeviceId) == V_ICH9_LPC_DEVICE_ID_0                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_2                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_3                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_4                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_6                     \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_IDE_4_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_IDE_2_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_AHCI_6_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_AHCI_4_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_RAID               \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_D_RAID_ALT           \
  || (DeviceId) == V_ICH9_SIDE2_DEVICE_ID_D_IDE_2_PORTS       \
  )

//
// Device ID that is ICH9 specific and Mobile specific
//
#define IS_ICH9_DEVICE_ID_MOBILE(DeviceId) ( \
     (DeviceId) == V_ICH9_LPC_DEVICE_ID_1                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_5                     \
  || (DeviceId) == V_ICH9_LPC_DEVICE_ID_7                     \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_IDE_2_PORTS        \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_AHCI_4_PORTS       \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_RAID               \
  || (DeviceId) == V_ICH9_SIDE_DEVICE_ID_M_RAID_ALT           \
  || (DeviceId) == V_ICH9_SIDE2_DEVICE_ID_M_IDE_2_PORTS       \
  )

//
// Device ID that is ICH10 specific (not shared between ICH9 and ICH10)
// ESS_OVERRIDE Added V_ICH10_LPC_DEVICE_ID_8
//
#define IS_ICH10_DEVICE_ID(DeviceId) ( \
     (DeviceId) == V_ICH10_LPC_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_2                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_3                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_4                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_5                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_6                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_7                    \
  || (DeviceId) == V_ICH10_LPC_DEVICE_ID_8                    \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_IDE_4_PORTS       \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_IDE_2_PORTS       \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_AHCI_6_PORTS      \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_AHCI_4_PORTS      \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_RAID              \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_RAID_ALT_1        \
  || (DeviceId) == V_ICH10_SIDE_DEVICE_ID_D_RAID_ALT_2        \
  || (DeviceId) == V_ICH10_SIDE2_DEVICE_ID_D_IDE_2_PORTS      \
  || (DeviceId) == V_ICH10_THERMAL_DEVICE_ID_0                \
  || (DeviceId) == V_ICH10_SMBUS_DEVICE_ID                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_2                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_3                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_4                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_5                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_6                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_6_1                  \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_7                    \
  || (DeviceId) == V_ICH10_USB_DEVICE_ID_7_1                  \
  || (DeviceId) == V_ICH10_LAN_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH10_HDA_DEVICE_ID_0                    \
  || (DeviceId) == V_ICH10_HDA_DEVICE_ID_1                    \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_0                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_1                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_2                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_3                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_4                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_5                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_6                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_7                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_8                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_9                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_A                   \
  || (DeviceId) == V_ICH10_PCIE_DEVICE_ID_B                   \
  )

//
// Device ID that is ICH10 Corporate specific (non-shared between Consumer
// and Corporate)
//
#define IS_ICH10_DEVICE_ID_CORPORATE(DeviceId) ( \
          (DeviceId) == V_ICH10_LPC_DEVICE_ID_0 \
        )

//
// Device ID that is ICH10 Consumer specific (non-shared between Consumer
// and Corporate)
//
#define IS_ICH10_DEVICE_ID_CONSUMER(DeviceId) ( \
          !((DeviceId) == V_ICH10_LPC_DEVICE_ID_0) \
        )

//
// PCI to PCI Bridge Registers (D30:F0)
//
#define PCI_DEVICE_NUMBER_ICH_PCI_PCI                  30
#define PCI_FUNCTION_NUMBER_ICH_PCI_PCI                0

#define R_ICH_PCI_PCI_VID                              0x00
#define   V_ICH_PCI_PCI_VENDOR_ID                        V_INTEL_VENDOR_ID
#define R_ICH_PCI_PCI_DID                              0x02
#define   V_ICH_PCI_PCI_DEVICE_ID_0                      0x2448
#define   V_ICH_PCI_PCI_DEVICE_ID_1                      0x244E

#define R_ICH_PCI_PCI_PCICMD                           0x04
#define   B_ICH_PCI_PCI_PCICMD_ID                        BIT10
#define   B_ICH_PCI_PCI_PCICMD_FBE                       BIT9
#define   B_ICH_PCI_PCI_PCICMD_SERR_EN                   BIT8
#define   B_ICH_PCI_PCI_PCICMD_WCC                       BIT7
#define   B_ICH_PCI_PCI_PCICMD_PER                       BIT6
#define   B_ICH_PCI_PCI_PCICMD_VPS                       BIT5
#define   B_ICH_PCI_PCI_PCICMD_MWE                       BIT4
#define   B_ICH_PCI_PCI_PCICMD_SCE                       BIT3
#define   B_ICH_PCI_PCI_PCICMD_BME                       BIT2
#define   B_ICH_PCI_PCI_PCICMD_MSE                       BIT1
#define   B_ICH_PCI_PCI_PCICMD_IOSE                      BIT0
#define R_ICH_PCI_PCI_PSTS                             0x06
#define   B_ICH_PCI_PCI_PSTS_DPE                         BIT15
#define   B_ICH_PCI_PCI_PSTS_SSE                         BIT14
#define   B_ICH_PCI_PCI_PSTS_RMA                         BIT13
#define   B_ICH_PCI_PCI_PSTS_RTA                         BIT12
#define   B_ICH_PCI_PCI_PSTS_STA                         BIT11
#define   B_ICH_PCI_PCI_PSTS_DPD                         BIT8
#define   B_ICH_PCI_PCI_PSTS_CLIST                       BIT4
#define   B_ICH_PCI_PCI_PSTS_IS                          BIT3
#define R_ICH_PCI_PCI_RID                              0x08
#define R_ICH_PCI_PCI_PI                               0x09
#define R_ICH_PCI_PCI_SCC                              0x0A
#define R_ICH_PCI_PCI_BCC                              0x0B
#define R_ICH_PCI_PCI_PMLT                             0x0D
#define   B_ICH_PCI_PCI_PMLT_MLTC                        0xF8
#define R_ICH_PCI_PCI_HEADTYP                          0x0E
#define   B_ICH_PCI_PCI_HEADTYP_MFD                      0x80
#define   B_ICH_PCI_PCI_HEADTYP_HTYPE                    0x7F
#define R_ICH_PCI_PCI_PBN                              0x18
#define R_ICH_PCI_PCI_SCBN                             0x19
#define R_ICH_PCI_PCI_SBBN                             0x1A
#define R_ICH_PCI_PCI_SMLT                             0x1B
#define   B_ICH_PCI_PCI_SMLT_MLTC                        0xF8
#define R_ICH_PCI_PCI_IO_BASE                          0x1C
#define   B_ICH_PCI_PCI_IO_BASE_IOBA                     0xF0
#define   B_ICH_PCI_PCI_IO_BASE_IOBC                     0x0F
#define R_ICH_PCI_PCI_IO_LIMIT                         0x1D
#define   B_ICH_PCI_PCI_IO_LIMIT_IOLA                    0xF0
#define   B_ICH_PCI_PCI_IO_LIMIT_IOLC                    0x0F
#define R_ICH_PCI_PCI_SECSTS                           0x1E
#define   B_ICH_PCI_PCI_SECSTS_DPE                       BIT15
#define   B_ICH_PCI_PCI_SECSTS_SSE                       BIT14
#define   B_ICH_PCI_PCI_SECSTS_RMA                       BIT13
#define   B_ICH_PCI_PCI_SECSTS_RTA                       BIT12
#define   B_ICH_PCI_PCI_SECSTS_STA                       BIT11
#define   B_ICH_PCI_PCI_SECSTS_DEVT                      BIT10 | BIT9
#define   B_ICH_PCI_PCI_SECSTS_DPD                       BIT8
#define   B_ICH_PCI_PCI_SECSTS_FBC                       BIT7
#define   B_ICH_PCI_PCI_SECSTS_66MHZ_CAP                 BIT5
#define R_ICH_PCI_PCI_MEM_BASE                         0x20
#define   B_ICH_PCI_PCI_MEM_BASE_MB                      0xFFF0
#define R_ICH_PCI_PCI_MEM_LIMIT                        0x22
#define   B_ICH_PCI_PCI_MEM_LIMIT_ML                     0xFFF0
#define R_ICH_PCI_PCI_PREF_MEM_BASE                    0x24
#define   B_ICH_PCI_PCI_PREF_MEM_BASE_PMB                0xFFF0
#define R_ICH_PCI_PCI_PREF_MEM_LIMIT                   0x26
#define   B_ICH_PCI_PCI_PREF_MEM_LIMIT_PML               0xFFF0
#define R_ICH_PCI_PCI_PMBU32                           0x28
#define R_ICH_PCI_PCI_PMLU32                           0x2C
#define R_ICH_PCI_PCI_CAPP                             0x34
#define R_ICH_PCI_PCI_INTR                             0x3C
#define   B_ICH_PCI_PCI_INTR_IPIN                        0xFF00
#define   B_ICH_PCI_PCI_INTR_ILINE                       0x00FF
#define R_ICH_PCI_PCI_BCTRL                            0x3E
#define   B_ICH_PCI_PCI_BCTRL_DTE                        BIT11
#define   B_ICH_PCI_PCI_BCTRL_DTS                        BIT10
#define   B_ICH_PCI_PCI_BCTRL_SDT                        BIT9
#define   B_ICH_PCI_PCI_BCTRL_PDT                        BIT8
#define   B_ICH_PCI_PCI_BCTRL_FBE                        BIT7
#define   B_ICH_PCI_PCI_BCTRL_SBR                        BIT6
#define   B_ICH_PCI_PCI_BCTRL_MAM                        BIT5
#define   B_ICH_PCI_PCI_BCTRL_V16D                       BIT4
#define   B_ICH_PCI_PCI_BCTRL_VGAE                       BIT3
#define   B_ICH_PCI_PCI_BCTRL_IE                         BIT2
#define   B_ICH_PCI_PCI_BCTRL_SEE                        BIT1
#define   B_ICH_PCI_PCI_BCTRL_PERE                       BIT0
#define R_ICH_PCI_PCI_SPDH                             0x40
#define   B_ICH_PCI_PCI_SPDH_HD3                         BIT3
#define   B_ICH_PCI_PCI_SPDH_HD2                         BIT2
#define   B_ICH_PCI_PCI_SPDH_HD1                         BIT1
#define   B_ICH_PCI_PCI_SPDH_HD0                         BIT0
#define R_ICH_PCI_PCI_DTC                              0x44
#define   B_ICH_PCI_PCI_DTC_DDT                          BIT31
#define   B_ICH_PCI_PCI_DTC_BDT                          BIT30
#define   B_ICH_PCI_PCI_DTC_MDT                          (BIT7|BIT6)
#define   B_ICH_PCI_PCI_DTC_AFADE                        BIT4
#define   B_ICH_PCI_PCI_DTC_NP                           BIT3
#define   B_ICH_PCI_PCI_DTC_MRMPD                        BIT2
#define   B_ICH_PCI_PCI_DTC_MRLPD                        BIT1
#define   B_ICH_PCI_PCI_DTC_MRPD                         BIT0
#define R_ICH_PCI_PCI_BPS                              0x48
#define   B_ICH_PCI_PCI_BPS_PAD                          BIT16
#define   B_ICH_PCI_PCI_BPS_NPT                          (BIT6|BIT5|BIT4)
#define   B_ICH_PCI_PCI_BPS_NAT                          (BIT1|BIT0)
#define R_ICH_PCI_PCI_BPC                              0x4C
#define   B_ICH_PCI_PCI_BPC_URLT                         0x00003F00
#define   B_ICH_PCI_PCI_BPC_SDP                          BIT7
#define   B_ICH_PCI_PCI_BPC_PSE                          BIT6
#define   B_ICH_PCI_PCI_BPC_SDTT                         BIT5
#define   B_ICH_PCI_PCI_BPC_PDE                          BIT2
#define   B_ICH_PCI_PCI_BPC_RTAE                         BIT0
#define R_ICH_PCI_PCI_SVCAP                            0x50
#define   B_ICH_PCI_PCI_SVCAP_NEXT                       0xFF00
#define   B_ICH_PCI_PCI_SVCAP_CID                        0x00FF
#define R_ICH_PCI_PCI_SVID                             0x54
#define   B_ICH_PCI_PCI_SVID_SID                         0xFFFF0000
#define   B_ICH_PCI_PCI_SVID_SVID                        0x0000FFFF

//
// PCI to LPC Bridge Registers (D31:F0)
//

#define PCI_DEVICE_NUMBER_ICH_LPC                      31
#define PCI_FUNCTION_NUMBER_ICH_LPC                    0

#define R_ICH_LPC_VENDOR_ID                          0x00
#define   V_ICH_LPC_VENDOR_ID                          V_INTEL_VENDOR_ID
#define R_ICH_LPC_DEVICE_ID                          0x02

#define   V_ICH9_LPC_DEVICE_ID_0                        0x2910   // ICH9 A0, A1 and A2 Full Featured Desktop Engineering Sample
#define   V_ICH9_LPC_DEVICE_ID_1                        0x2911   // ICH9M A2 Full Featured Mobile Engineering Sample
#define   V_ICH9_LPC_DEVICE_ID_2                        0x2912   // ICH9DH Engineering Sample
#define   V_ICH9_LPC_DEVICE_ID_3                        0x2914   // ICH9DO Engineering Sample
#define   V_ICH9_LPC_DEVICE_ID_4                        0x2916   // ICH9R Engineering Sample
#define   V_ICH9_LPC_DEVICE_ID_5                        0x2917   // ICH9M Enhanced Engineering Sample
#define   V_ICH9_LPC_DEVICE_ID_6                        0x2918   // ICH9 Base Engineering Sample
#define   V_ICH9_LPC_DEVICE_ID_7                        0x2919   // ICH9M Base Engineering Sample

//
// ESS_OVERRIDE Added V_ICH10_LPC_DEVICE_ID_8
//
#define   V_ICH10_LPC_DEVICE_ID_0                       0x3A10   // ICH10 Corporate
#define   V_ICH10_LPC_DEVICE_ID_1                       0x3A11   // ICH10 D
#define   V_ICH10_LPC_DEVICE_ID_2                       0x3A12   // ICH10 DH
#define   V_ICH10_LPC_DEVICE_ID_3                       0x3A14   // ICH10 DO
#define   V_ICH10_LPC_DEVICE_ID_4                       0x3A16   // ICH9 R
#define   V_ICH10_LPC_DEVICE_ID_5                       0x3A17   // ICH9 M Enhanced
#define   V_ICH10_LPC_DEVICE_ID_6                       0x3A18   // ICH9 D Base
#define   V_ICH10_LPC_DEVICE_ID_7                       0x3A19   // ICH9 M Base
#define   V_ICH10_LPC_DEVICE_ID_8                       0x3A1E   // ICH10 Consumer Eng Sample ESS_OVERRIDE

#define R_ICH_LPC_COMMAND                            0x04
#define   B_ICH_LPC_COMMAND_FBE                        0x0200
#define   B_ICH_LPC_COMMAND_SERR_EN                    0x0100
#define   B_ICH_LPC_COMMAND_WCC                        0x0080
#define   B_ICH_LPC_COMMAND_PER                        0x0040
#define   B_ICH_LPC_COMMAND_VPS                        0x0020
#define   B_ICH_LPC_COMMAND_PMWE                       0x0010
#define   B_ICH_LPC_COMMAND_SCE                        0x0008
#define   B_ICH_LPC_COMMAND_BME                        0x0004
#define   B_ICH_LPC_COMMAND_MSE                        0x0002
#define   B_ICH_LPC_COMMAND_IOSE                       0x0001
#define R_ICH_LPC_DEV_STS                            0x06
#define   B_ICH_LPC_DEV_STS_DPE                        0x8000
#define   B_ICH_LPC_DEV_STS_SSE                        0x4000
#define   B_ICH_LPC_DEV_STS_RMA                        0x2000
#define   B_ICH_LPC_DEV_STS_RTA                        0x1000
#define   B_ICH_LPC_DEV_STS_STA                        0x0800
#define   B_ICH_LPC_DEV_STS_DEVT_STS                   0x0600
#define   B_ICH_LPC_DEV_STS_MDPED                      0x0100
#define   B_ICH_LPC_DEV_STS_FB2B                       0x0080
#define   B_ICH_LPC_DEV_STS_UDF                        0x0040
#define   B_ICH_LPC_DEV_STS_66MHZ_CAP                  0x0020
#define R_ICH_LPC_RID                                0x08
#define   B_ICH_LPC_RID_0                              0xFF
#define     V_ICH_LPC_RID_0                              0x00
#define     V_ICH_LPC_RID_1                              0x01
#define R_ICH_LPC_PI                                 0x09
#define R_ICH_LPC_SCC                                0x0A
#define R_ICH_LPC_BCC                                0x0B
#define R_ICH_LPC_PLT                                0x0D
#define R_ICH_LPC_HEADTYP                            0x0E
#define   B_ICH_LPC_HEADTYP_MFD                        BIT7
#define   B_ICH_LPC_HEADTYP_HT                         0x7F
#define R_ICH_LPC_SS                                 0x2C
#define   B_ICH_LPC_SS_SSID                            0xFFFF0000
#define   B_ICH_LPC_SS_SSVID                           0x0000FFFF
#define R_ICH_LPC_ACPI_BASE                          0x40
#define   B_ICH_LPC_ACPI_BASE_BAR                      0x0000FF80
#define   B_ICH_LPC_ACPI_BASE_EN                       BIT0
#define R_ICH_LPC_ACPI_CNT                           0x44
#define   B_ICH_LPC_ACPI_CNT_ACPI_EN                   0x80
#define   B_ICH_LPC_ACPI_CNT_SCI_IRG_SEL               0x07
#define R_ICH_LPC_GPIO_BASE                          0x48
#define   B_ICH_LPC_GPIO_BASE_BAR                      0x0000FFC0
#define R_ICH_LPC_GPIO_CNT                           0x4C
#define   B_ICH_LPC_GPIO_CNT_GPIO_EN                   0x10
#define   B_ICH_LPC_GPIO_LOCKDOWN_EN                   0x01
#define R_ICH_LPC_PIRQA_ROUT                         0x60
#define R_ICH_LPC_PIRQB_ROUT                         0x61
#define R_ICH_LPC_PIRQC_ROUT                         0x62
#define R_ICH_LPC_PIRQD_ROUT                         0x63
#define R_ICH_LPC_C4_TIMING_CNT                      0xAA

//
// Bit values are the same for R_ICH_LPC_PIRQA_ROUT to R_ICH_LPC_PIRQH_ROUT
//
#define   B_ICH_LPC_PIRQX_ROUT_IRQEN                   0x80
#define   B_ICH_LPC_PIRQX_ROUT                         0x0F
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_3                   0x03
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_4                   0x04
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_5                   0x05
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_6                   0x06
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_7                   0x07
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_9                   0x09
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_10                  0x0A
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_11                  0x0B
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_12                  0x0C
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_14                  0x0E
#define    V_ICH_LPC_PIRQX_ROUT_IRQ_15                  0x0F
#define R_ICH_LPC_SERIRQ_CNT                         0x64
#define   B_ICH_LPC_SERIRQ_CNT_SIRQEN                  0x80
#define   B_ICH_LPC_SERIRQ_CNT_SIRQMD                  0x40
#define   B_ICH_LPC_SERIRQ_CNT_SIRQSZ                  0x3C
#define   N_ICH_LPC_SERIRQ_CNT_SIRQSZ                  2
#define   B_ICH_LPC_SERIRQ_CNT_SFPW                    0x03
#define   N_ICH_LPC_SERIRQ_CNT_SFPW                    0
#define R_ICH_LPC_PIRQE_ROUT                         0x68
#define R_ICH_LPC_PIRQF_ROUT                         0x69
#define R_ICH_LPC_PIRQG_ROUT                         0x6A
#define R_ICH_LPC_PIRQH_ROUT                         0x6B
#define R_ICH_LPC_IOXAPIC                            0x6C
#define   B_ICH_LPC_IOXAPIC_BUS                        0xFF00
#define   B_ICH_LPC_IOXAPIC_DEVICE                     0x00F8
#define   B_ICH_LPC_IOXAPIC_FUNC                       0x0007
#define R_ICH_LPC_IO_DEC                             0x80
#define   B_ICH_LPC_FDD_DEC                            0x1000
#define   B_ICH_LPC_LPT_DEC                            0x0300
#define   B_ICH_LPC_COMB_DEC                           0x0070
#define     V_ICH_LPC_COMB_3F8                           0x00
#define     V_ICH_LPC_COMB_2F8                           0x10
#define     V_ICH_LPC_COMB_220                           0x20
#define     V_ICH_LPC_COMB_228                           0x30
#define     V_ICH_LPC_COMB_238                           0x40
#define     V_ICH_LPC_COMB_2E8                           0x50
#define     V_ICH_LPC_COMB_338                           0x60
#define     V_ICH_LPC_COMB_3E8                           0x70
#define   B_ICH_LPC_COMA_DEC                           0x0007
#define     V_ICH_LPC_COMA_3F8                           0x00
#define     V_ICH_LPC_COMA_2F8                           0x01
#define     V_ICH_LPC_COMA_220                           0x02
#define     V_ICH_LPC_COMA_228                           0x03
#define     V_ICH_LPC_COMA_238                           0x04
#define     V_ICH_LPC_COMA_2E8                           0x05
#define     V_ICH_LPC_COMA_338                           0x06
#define     V_ICH_LPC_COMA_3E8                           0x07
#define R_ICH_LPC_ENABLES                            0x82
#define   B_ICH_LPC_ENABLES_CNF2_EN                    0x2000
#define   B_ICH_LPC_ENABLES_CNF1_EN                    0x1000
#define   B_ICH_LPC_ENABLES_MC_EN                      0x0800
#define   B_ICH_LPC_ENABLES_KBC_EN                     0x0400
#define   B_ICH_LPC_ENABLES_GAMEH_EN                   0x0200
#define   B_ICH_LPC_ENABLES_GAMEL_EN                   0x0100
#define   B_ICH_LPC_ENABLES_FDD_EN                     0x0008
#define   B_ICH_LPC_ENABLES_LPT_EN                     0x0004
#define   B_ICH_LPC_ENABLES_COMB_EN                    0x0002
#define   B_ICH_LPC_ENABLES_COMA_EN                    0x0001
#define R_ICH_LPC_GEN1_DEC                           0x84
#define   B_ICH_LPC_GEN1_DEC_IODRA                     0x00FC0000
#define   B_ICH_LPC_GEN1_DEC_IOBAR                     0x0000FFFC
#define   B_ICH_LPC_GEN1_DEC_EN                        0x00000001
#define R_ICH_LPC_GEN2_DEC                           0x88
#define R_ICH_LPC_GEN3_DEC                           0x8C
#define R_ICH_LPC_GEN4_DEC                           0x90
#define R_ICH_LPC_FWH_BIOS_SEL                       0xD0
#define   B_ICH_LPC_FWH_BIOS_SEL_F8                    0xF0000000
#define   B_ICH_LPC_FWH_BIOS_SEL_F0                    0x0F000000
#define   B_ICH_LPC_FWH_BIOS_SEL_E8                    0x00F00000
#define   B_ICH_LPC_FWH_BIOS_SEL_E0                    0x000F0000
#define   B_ICH_LPC_FWH_BIOS_SEL_D8                    0x0000F000
#define   B_ICH_LPC_FWH_BIOS_SEL_D0                    0x00000F00
#define   B_ICH_LPC_FWH_BIOS_SEL_C8                    0x000000F0
#define   B_ICH_LPC_FWH_BIOS_SEL_C0                    0x0000000F
#define R_ICH_LPC_FWH_BIOS_SEL2                      0xD4
#define   B_ICH_LPC_FWH_BIOS_SEL2_70                   0xF000
#define   B_ICH_LPC_FWH_BIOS_SEL2_60                   0x0F00
#define   B_ICH_LPC_FWH_BIOS_SEL2_50                   0x00F0
#define   B_ICH_LPC_FWH_BIOS_SEL2_40                   0x000F
#define R_ICH_LPC_FWH_BIOS_DEC                       0xD8
#define   B_ICH_LPC_FWH_BIOS_DEC_F8                    0x8000
#define   B_ICH_LPC_FWH_BIOS_DEC_F0                    0x4000
#define   B_ICH_LPC_FWH_BIOS_DEC_E8                    0x2000
#define   B_ICH_LPC_FWH_BIOS_DEC_E0                    0x1000
#define   B_ICH_LPC_FWH_BIOS_DEC_D8                    0x0800
#define   B_ICH_LPC_FWH_BIOS_DEC_D0                    0x0400
#define   B_ICH_LPC_FWH_BIOS_DEC_C8                    0x0200
#define   B_ICH_LPC_FWH_BIOS_DEC_C0                    0x0100
#define   B_ICH_LPC_FWH_BIOS_LEG_F                     0x0080
#define   B_ICH_LPC_FWH_BIOS_LEG_E                     0x0040
#define   B_ICH_LPC_FWH_BIOS_DEC_70                    0x0008
#define   B_ICH_LPC_FWH_BIOS_DEC_60                    0x0004
#define   B_ICH_LPC_FWH_BIOS_DEC_50                    0x0002
#define   B_ICH_LPC_FWH_BIOS_DEC_40                    0x0001
#define R_ICH_LPC_BIOS_CNTL                          0xDC
#define  S_ICH_LPC_BIOS_CNTL                          1
#define   B_ICH_LPC_BIOS_CNTL_TSS                      0x10
#define   V_ICH_LPC_BIOS_CNTL_SRC                      0x0C
#define    V_ICH_SRC_PREF_EN_CACHE_EN                  0x08
#define    V_ICH_SRC_PREF_DIS_CACHE_DIS                0x04
#define    V_ICH_SRC_PREF_DIS_CACHE_EN                 0x00
#define   B_ICH_LPC_BIOS_CNTL_BLE                      0x02
#define   B_ICH_LPC_BIOS_CNTL_BIOSWE                   0x01
#define   N_ICH_LPC_BIOS_CNTL_BLE                      1
#define   N_ICH_LPC_BIOS_CNTL_BIOSWE                   0

#define R_ICH_LPC_FDCAP                              0xE0
#define   B_ICH_LPC_FDCAP_NEXT                         0xFF00
#define   B_ICH_LPC_FDCAP_CID                          0x00FF
#define R_ICH_LPC_FDLEN                              0xE2
#define   B_ICH_LPC_FDLEN                              0xFF
#define R_ICH_LPC_FDVER                              0xE3
#define   B_ICH_LPC_FDVER_VSCID                        0xF0
#define   B_ICH_LPC_FDVER_CV                           0x0F
#define R_ICH_LPC_FDVCT                              0xE4
#define   B_ICH_LPC_FDVCT_IAMT_CAP                     0x0000006000000000
#define   B_ICH_LPC_FDVCT_SATA_PORT23_CAP              0x0000000008000000
#define   B_ICH_LPC_FDVCT_MOBILE_FEAT_CAP              0x0000000000000200
#define   B_ICH_LPC_FDVCT_SATA_RAID_CAP                0x0000000000000020
#define R_ICH_LPC_GEN_PMCON_1                        0xA0
#define   B_ICH_LPC_GEN_PMCON_PER_SMI_SEL              0x0003
#define    V_PER_SMI_64S                               0x0000
#define    V_PER_SMI_32S                               0x0001
#define    V_PER_SMI_16S                               0x0002
#define    V_PER_SMI_8S                                0x0003
#define   B_ICH_LPC_GEN_PMCON_CLKRUN_EN                0x0004
#define   B_ICH_LPC_GEN_PMCON_SS_EN                    0x0008
#define   B_ICH_LPC_GEN_PMCON_SMI_LOCK                 0x0010
#define   B_ICH_LPC_GEN_PMCON_CPUSLP_EN                0x0020
#define   B_ICH_LPC_GEN_PMCON_A64_EN                   0x0040
#define   B_ICH_LPC_GEN_PMCON_C4ONC3_EN                0x0080
#define   B_ICH_LPC_GEN_PMCON_PWRBTN_LVL               0x0200
#define   B_ICH_LPC_GEN_PMCON_BIOS_PCI_EXP_EN          0x0400
#define   B_ICH_LPC_GEN_PMCON_C4_DISABLE               0x1000
#define R_ICH_LPC_GEN_PMCON_2                        0xA2
#define   B_ICH_LPC_GEN_PMCON_PWROK_FLR                0x01
#define   B_ICH_LPC_GEN_PMCON_CPUPWR_FLR               0x02
#define   B_ICH_LPC_GEN_PMCON_MIN_SLP_S4               0x04
#define   B_ICH_LPC_GEN_PMCON_CTS                      0x08
#define   B_ICH_LPC_GEN_PMCON_SRS                      0x10
#define   B_ICH_LPC_GEN_PMCON_CPLT                     0x60
#define   B_ICH_LPC_GEN_PMCON_DRAM_INIT                0x80

#define R_ICH9_LPC_GEN_PMCON_3                        0xA4
#define   B_ICH9_LPC_GEN_PMCON_GEN_RST_STS              BIT9
#define   B_ICH9_LPC_GEN_PMCON_S4_STATE_PIN_DIS         BIT8
#define   B_ICH9_LPC_GEN_PMCON_SWSMI_RTSL                0xC0
#define     V_ICH9_LPC_GEN_PMCON_SWSMI_RTSL_64MS          0xC0
#define     V_ICH9_LPC_GEN_PMCON_SWSMI_RTSL_32MS          0x80
#define     V_ICH9_LPC_GEN_PMCON_SWSMI_RTSL_16MS          0x40
#define     V_ICH9_LPC_GEN_PMCON_SWSMI_RTSL_1_5MS         0x00
#define   B_ICH9_LPC_GEN_PMCON_SLP_S4_MAW               0x30
#define     V_ICH9_LPC_GEN_PMCON_SLP_S4_MAW_1_2S          0x30
#define     V_ICH9_LPC_GEN_PMCON_SLP_S4_MAW_2_3S          0x20
#define     V_ICH9_LPC_GEN_PMCON_SLP_S4_MAW_3_4S          0x10
#define     V_ICH9_LPC_GEN_PMCON_SLP_S4_MAW_4_5S          0x00
#define   B_ICH9_LPC_GEN_PMCON_SLP_S4_ASE               0x08
#define   B_ICH9_LPC_GEN_PMCON_RTC_PWR_STS              0x04
#define   B_ICH9_LPC_GEN_PMCON_PWR_FLR                  0x02
#define   B_ICH9_LPC_GEN_PMCON_AFTERG3_EN               0x01

#define R_ICH10_LPC_GEN_PMCON_3                        0xA4
#define   B_ICH10_LPC_GEN_PMCON_PME_B0_S5_DIS            BIT15
#define   B_ICH10_LPC_GEN_PMCON_MAC_POWERDN_WOL_DIS      BIT14
#define   B_ICH10_LPC_GEN_PMCON_WOL_ENABLE_OVERRIDE      BIT13
#define   B_ICH10_LPC_GEN_PMCON_DISABLE_S4_STRETCH       BIT12
#define   B_ICH10_LPC_GEN_PMCON_SLP_S3_MAW               0xC000
#define     V_ICH10_LPC_GEN_PMCON_SLP_S3_MAW_60US          0x0000
#define     V_ICH10_LPC_GEN_PMCON_SLP_S3_MAW_1MS           0x4000
#define     V_ICH10_LPC_GEN_PMCON_SLP_S3_MAW_50MS          0x8000
#define     V_ICH10_LPC_GEN_PMCON_SLP_S3_MAW_2S            0xC000
#define   B_ICH10_LPC_GEN_PMCON_GEN_RST_STS              BIT9
#define   B_ICH10_LPC_GEN_PMCON_S4_STATE_PIN_DIS         BIT8
#define   B_ICH10_LPC_GEN_PMCON_SWSMI_RTSL               0xC0
#define     V_ICH10_LPC_GEN_PMCON_SWSMI_RTSL_64MS          0xC0
#define     V_ICH10_LPC_GEN_PMCON_SWSMI_RTSL_32MS          0x80
#define     V_ICH10_LPC_GEN_PMCON_SWSMI_RTSL_16MS          0x40
#define     V_ICH10_LPC_GEN_PMCON_SWSMI_RTSL_1_5MS         0x00
#define   B_ICH10_LPC_GEN_PMCON_SLP_S4_MAW               0x30
#define     V_ICH10_LPC_GEN_PMCON_SLP_S4_MAW_1_2S          0x30
#define     V_ICH10_LPC_GEN_PMCON_SLP_S4_MAW_2_3S          0x20
#define     V_ICH10_LPC_GEN_PMCON_SLP_S4_MAW_3_4S          0x10
#define     V_ICH10_LPC_GEN_PMCON_SLP_S4_MAW_4_5S          0x00
#define   B_ICH10_LPC_GEN_PMCON_SLP_S4_ASE               0x08
#define   B_ICH10_LPC_GEN_PMCON_RTC_PWR_STS              0x04
#define   B_ICH10_LPC_GEN_PMCON_PWR_FLR                  0x02
#define   B_ICH10_LPC_GEN_PMCON_AFTERG3_EN               0x01

#define R_ICH_LPC_GEN_PMCON_LOCK                     0xA6
#define   B_ICH_LPC_GEN_PMCON_LOCK_UCST                BIT7 //Unlocked C-State Tansition
#define   B_ICH_LPC_GEN_PMCON_LOCK_S4_STRET_LD         BIT2 //Lock down SLP_S4 Minimum Assertion with
#define   B_ICH_LPC_GEN_PMCON_LOCK_ABASE_LK            BIT1 //Lock ACPI BASE at 0x40, only cleared by reset when set
#define   B_ICH_LPC_GEN_PMCON_LOCK_CST_CONF_LK         BIT0 //Lock down C-State configuration parameters
#define R_ICH_LPC_C5_EXIT_TIMING_CNT                 0xA8
#define   B_ICH_LPC_SLOW_C5_EXIT_EN                    BIT6
#define   B_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK             0x38
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_16_17PCLK   0x00
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_80_86US     0x08
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_99_105US    0x10
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_118_124US   0x18
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_18_24US     0x20
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_38_44US     0x28
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_56_62US     0x30
#define    V_ICH_LPC_C5_EXT_PMSYNC_TO_SPCLK_199_205US   0x38
#define   B_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU            0x07
#define    V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_22_28US    0x00
#define    V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_34_40US    0x01
#define    V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_56_62US    0x02
#define    V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_95_102US   0x03
#define    V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_138_144US  0x05
#define    V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_72_78US    0x06
#define    V_ICH_LPC_C5_EXIT_DPRSTP_TO_SPCPU_199_205US  0x07
#define R_ICH_LPC_C_STATE_CNF                        0xA9
#define   B_ICH_LPC_C_STATE_SP                         BIT7
#define   B_ICH_LPC_C_STATE_POPDOWN_EN                 BIT4
#define   B_ICH_LPC_C_STATE_POPUP_EN                   BIT3
#define   B_ICH_LPC_C_STATE_BM_STS_ZERO_EN             BIT2
#define R_ICH_LPC_C4_CNT                             0xAA
#define   B_ICH_LPC_C4_CNT                             BIT6
#define   B_ICH_LPC_C4_CNT_DPRSLPVR_TO_STPCPU          (BIT3 | BIT2)
#define   B_ICH_LPC_C4_CNT_DPSLP_TO_SLP                (BIT1 | BIT0)
#define R_ICH_LPC_BM_BREAK_EN                        0xAB
#define   B_ICH_LPC_BM_BREAK_EN_STORAGE                BIT7
#define   B_ICH_LPC_BM_BREAK_EN_PCIE                   BIT6
#define   B_ICH_LPC_BM_BREAK_EN_PCI                    BIT5
#define   B_ICH_LPC_BM_BREAK_EN_EHCI                   BIT2
#define   B_ICH_LPC_BM_BREAK_EN_UHCI                   BIT1
#define   B_ICH_LPC_BM_BREAK_EN_HDA                    BIT0
#define R_ICH_LPC_PMIR                               0xAC
#define   B_ICH_LPC_PMIR_FIELD_2                       BIT30
#define   B_ICH_LPC_CF9GR                              BIT20
#define   B_ICH_LPC_CWORWRE                            BIT18  // ESS_OVERRIDE
#define   B_ICH_LPC_PMIR_FIELD_0                       (BIT9 | BIT8)
#define R_ICH_LPC_MSC_FUN                            0xAD
#define   B_ICH_LPC_USB_TDD                            (BIT1 | BIT0)
#define R_ICH_LPC_QRT_STS                            0xB0
#define   B_ICH_LPC_QRT_SCI_NOW_STS                    BIT4
#define   B_ICH_LPC_QRT_PB_SCI_STS                     BIT3
#define   B_ICH_LPC_QRT_PB_SMI_STS                     BIT0
#define R_ICH_LPC_QRT_CNT1                           0xB1
#define   B_ICH_LPC_QRT_SMI_OPTION_CNT                 BIT9
#define   B_ICH_LPC_QRT_SCI_NOW_CNT                    BIT8
#define   B_ICH_LPC_PWRBTN_INT_EN                      BIT7
#define   B_ICH_LPC_PWRBTN_EVNT                        BIT6
#define   B_ICH_LPC_QRT_STATE1_CNT                     BIT5 | BIT4
#define   B_ICH_LPC_QRT_STATE0_CNT                     BIT3 | BIT2
#define   B_ICH_LPC_QRT_LED_OWN                        BIT1
#define R_ICH_LPC_QRT_CNT2                           0xB3
#define   B_ICH_LPC_QRT_EN                             BIT0
#define R_ICH_LPC_GPI_ROUT                           0xB8
#define   B_ICH_LPC_GPI_ROUT_0                         0x00000003
#define   B_ICH_LPC_GPI_ROUT_1                         0x0000000C
#define   B_ICH_LPC_GPI_ROUT_2                         0x00000030
#define   B_ICH_LPC_GPI_ROUT_3                         0x000000C0
#define   B_ICH_LPC_GPI_ROUT_4                         0x00000300
#define   B_ICH_LPC_GPI_ROUT_5                         0x00000C00
#define   B_ICH_LPC_GPI_ROUT_6                         0x00003000
#define   B_ICH_LPC_GPI_ROUT_7                         0x0000C000
#define   B_ICH_LPC_GPI_ROUT_8                         0x00030000
#define   B_ICH_LPC_GPI_ROUT_9                         0x000C0000
#define   B_ICH_LPC_GPI_ROUT_10                        0x00300000
#define   B_ICH_LPC_GPI_ROUT_11                        0x00C00000
#define   B_ICH_LPC_GPI_ROUT_12                        0x03000000
#define   B_ICH_LPC_GPI_ROUT_13                        0x08000000
#define   B_ICH_LPC_GPI_ROUT_14                        0x30000000
#define   B_ICH_LPC_GPI_ROUT_15                        0xC0000000

//
// ACPI and legacy I/O register offsets from PMBASE
//

#define R_ACPI_PM1_STS                               0x00
#define  S_ACPI_PM1_STS                               2
#define   B_WAK_STS                                    0x8000
#define   B_PRBTNOR_STS                                0x0800
#define   B_RTC_STS                                    0x0400
#define   B_ME_STS                                     0x0200
#define   B_PWRBTN_STS                                 0x0100
#define   B_GBL_STS                                    0x0020
#define   B_BM_STS                                     0x0010
#define   B_TMROF_STS                                  0x0001
#define   N_WAK_STS                                    15
#define   N_PRBTNOR_STS                                11
#define   N_RTC_STS                                    10
#define   N_ME_STS                                     9
#define   N_PWRBTN_STS                                 8
#define   N_GBL_STS                                    5
#define   N_BM_STS                                     4
#define   N_TMROF_STS                                  0

#define R_ACPI_PM1_EN                                0x02
#define  S_ACPI_PM1_EN                                2
#define   B_RTC_EN                                     0x0400
#define   B_PWRBTN_EN                                  0x0100
#define   B_GBL_EN                                     0x0020
#define   B_TMROF_EN                                   0X0001
#define   N_RTC_EN                                     10
#define   N_PWRBTN_EN                                  8
#define   N_GBL_EN                                     5
#define   N_TMROF_EN                                   0

#define R_ACPI_PM1_CNT                               0x04
#define  S_ACPI_PM1_CNT                               4
#define   B_SLP_EN                                     0x00002000
#define   B_SLP_TYP                                    0x00001C00
#define   V_S0                                         0x00000000
#define   V_S1_DESK                                    0x00000400
#define   V_S1_MOBL                                    0x00000400
#define   V_S3                                         0x00001400
#define   V_S4                                         0x00001800
#define   V_S5                                         0x00001C00
#define   B_GBL_RLS                                    0x00000004
#define   B_BM_RLD                                     0x00000002
#define   B_SCI_EN                                     0x00000001

#define R_ACPI_PM1_TMR                               0x08
#define   V_ACPI_TMR_FREQUENCY                         3579545
#define   V_ACPI_PM1_TMR_MAX_VAL                       0x1000000 // The timer is 24 bit overflow
#define R_ACPI_PROC_CNT                              0x10
#define  B_THTL_STS                                    0x00020000
#define  B_FORCE_THTL                                  0x00000100
#define  B_THRM_DTY                                    0x000000E0
#define  B_THTL_EN                                     0x00000010
#define  B_THTL_DTY                                    0x0000000E
#define R_ACPI_LV2                                   0x14
#define R_ACPI_LV3                                   0x15
#define R_ACPI_LV4                                   0x16

#define R_ACPI_GPE0_STS                              0x20
#define  S_ACPI_GPE0_STS                              8
#define  B_GPInn_STS                                 0xFFFF0000
#define  B_USB6_STS                                    BIT32
#define  B_GPI0_STS                                    BIT16
#define  B_SATA_SCI_STS                                BIT15
#define  B_USB4_STS                                    BIT14
#define  B_PME_B0_STS                                  BIT13
#define  B_USB3_STS                                    BIT12
#define  B_PME_STS                                     BIT11
#define  B_BATLOW_STS                                  BIT10
#define  B_PCI_EXP_STS                                 BIT9
#define  B_RI_STS                                      BIT8
#define  B_SMB_WAK_STS                                 BIT7
#define  B_TC0SCI_STS                                  BIT6
#define  B_USB5_STS                                    BIT5
#define  B_USB2_STS                                    BIT4
#define  B_USB1_STS                                    BIT3
#define  B_SWGPE_STS                                   BIT2
#define  B_HOT_PLUG_STS                                BIT1
#define  B_THRM_STS                                    BIT0
#define  N_USB6_STS                                    32
#define  N_SATA_SCI_STS                                15
#define  N_USB4_STS                                    14
#define  N_PME_B0_STS                                  13
#define  N_USB3_STS                                    12
#define  N_PME_STS                                     11
#define  N_BATLOW_STS                                  10
#define  N_PCI_EXP_STS                                 9
#define  N_RI_STS                                      8
#define  N_SMB_WAK_STS                                 7
#define  N_TC0SCI_STS                                  6
#define  N_USB5_STS                                    5
#define  N_USB2_STS                                    4
#define  N_USB1_STS                                    3
#define  N_SWGPE_STS                                   2
#define  N_HOT_PLUG_STS                                1
#define  N_THRM_STS                                    0

#define R_ACPI_GPE0_EN                               0x28
#define  S_ACPI_GPE0_EN                               8
#define  B_GPInn_EN                                    0xFFFF0000
#define  B_USB6_EN                                     BIT32
#define  B_GPI0_EN                                     BIT16
#define  B_USB4_EN                                     BIT14
#define  B_PME_B0_EN                                   BIT13
#define  B_USB3_EN                                     BIT12
#define  B_PME_EN                                      BIT11
#define  B_BATLOW_EN                                   BIT10
#define  B_PCI_EXP_EN                                  BIT9
#define  B_RI_EN                                       BIT8
#define  B_TC0SCI_EN                                   BIT6
#define  B_USB5_EN                                     BIT5
#define  B_USB2_EN                                     BIT4
#define  B_USB1_EN                                     BIT3
#define  B_SWGPE_EN                                    BIT2
#define  B_HOT_PLUG_EN                                 BIT1
#define  B_THRM_EN                                     BIT0
#define  N_USB6_EN                                     32
#define  N_USB4_EN                                     14
#define  N_PME_B0_EN                                   13
#define  N_USB3_EN                                     12
#define  N_PME_EN                                      11
#define  N_BATLOW_EN                                   10
#define  N_PCI_EXP_EN                                  9
#define  N_RI_EN                                       8
#define  N_TC0SCI_EN                                   6
#define  N_USB5_EN                                     5
#define  N_USB2_EN                                     4
#define  N_USB1_EN                                     3
#define  N_SWGPE_EN                                    2
#define  N_HOT_PLUG_EN                                 1
#define  N_THRM_EN                                     0

#define R_ACPI_SMI_EN                                0x30
#define  S_ACPI_SMI_EN                                4
#define   B_GPIO_UNLOCK_SMI_EN                         BIT27
#define   B_EL_SMI_EN                                  BIT25
#define   B_INTEL_USB2_EN                              BIT18
#define   B_LEGACY_USB2_EN                             BIT17
#define   B_PERIODIC_EN                                BIT14
#define   B_TCO_EN                                     BIT13
#define   B_MCSMI_EN                                   BIT11
#define   B_BIOS_RLS                                   BIT7
#define   B_SWSMI_TMR_EN                               BIT6
#define   B_APMC_EN                                    BIT5
#define   B_SMI_ON_SLP_EN                              BIT4
#define   B_LEGACY_USB_EN                              BIT3
#define   B_BIOS_EN                                    BIT2
#define   B_EOS                                        BIT1
#define   B_GBL_SMI_EN                                 BIT0
#define   N_GPIO_UNLOCK_SMI_EN                         27
#define   N_INTEL_USB2_EN                              18
#define   N_LEGACY_USB2_EN                             17
#define   N_PERIODIC_EN                                14
#define   N_TCO_EN                                     13
#define   N_MCSMI_EN                                   11
#define   N_BIOS_RLS                                   7
#define   N_SWSMI_TMR_EN                               6
#define   N_APMC_EN                                    5
#define   N_SMI_ON_SLP_EN                              4
#define   N_LEGACY_USB_EN                              3
#define   N_BIOS_EN                                    2
#define   N_EOS                                        1
#define   N_GBL_SMI_EN                                 0

#define R_ACPI_SMI_STS                               0x34
#define  S_ACPI_SMI_STS                               4
#define   B_GPIO_UNLOCK_SMI_STS                        BIT27
#define   B_SPI_SMI_STS                                BIT26
#define   B_INTERNAL_TT_STS                            BIT22
#define   B_MONITOR_STS                                BIT21
#define   B_PCI_EXP_SMI_STS                            BIT20
#define   B_PATCH_SMI_STS                              BIT19
#define   B_INTEL_USB2_STS                             BIT18
#define   B_LEGACY_USB2_STS                            BIT17
#define   B_SMBUS_SMI_STS                              BIT16
#define   B_SERIRQ_SMI_STS                             BIT15
#define   B_PERIODIC_STS                               BIT14
#define   B_TCO_STS                                    BIT13
#define   B_DEVMON_STS                                 BIT12
#define   B_MCSMI_STS                                  BIT11
#define   B_GPE1_STS                                   BIT10
#define   B_GPE0_STS                                   BIT9
#define   B_PM1_STS_REG                                BIT8
#define   B_SWSMI_TMR_STS                              BIT6
#define   B_APM_STS                                    BIT5
#define   B_SMI_ON_SLP_EN_STS                          BIT4
#define   B_LEGACY_USB_STS                             BIT3
#define   B_BIOS_STS                                   BIT2
#define   N_GPIO_UNLOCK_SMI_STS                        27
#define   N_SPI_SMI_STS                                26
#define   N_INTERNAL_TT_STS                            22
#define   N_MONITOR_STS                                21
#define   N_PCI_EXP_SMI_STS                            20
#define   N_PATCH_SMI_STS                              19
#define   N_INTEL_USB2_STS                             18
#define   N_LEGACY_USB2_STS                            17
#define   N_SMBUS_SMI_STS                              16
#define   N_SERIRQ_SMI_STS                             15
#define   N_PERIODIC_STS                               14
#define   N_TCO_STS                                    13
#define   N_DEVMON_STS                                 12
#define   N_MCSMI_STS                                  11
#define   N_GPE1_STS                                   10
#define   N_GPE0_STS                                   9
#define   N_PM1_STS_REG                                8
#define   N_SWSMI_TMR_STS                              6
#define   N_APM_STS                                    5
#define   N_SMI_ON_SLP_EN_STS                          4
#define   N_LEGACY_USB_STS                             3
#define   N_BIOS_STS                                   2

#define R_ACPI_ALT_GP_SMI_EN                         0x38
#define  S_ACPI_ALT_GP_SMI_EN                         2
#define R_ACPI_ALT_GP_SMI_STS                        0x3A
#define  S_ACPI_ALT_GP_SMI_STS                        2
//
// USB Per-Port Registers Write Control
//
#define R_ACPI_UPRWC                                 0x3C
#define  S_ACPI_UPRWC                                 2
#define   B_ACPI_UPRWC_WR_EN_SMI_STS                   0x0100
#define   B_ACPI_UPRWC_WR_EN                           0x0002
#define   B_ACPI_UPRWC_WR_EN_SMI_EN                    0x0001

#define R_ACPI_GPE_CNTL                              0x42
#define   B_ACPI_GPE_CNTL_SWGPE_CTRL                   BIT1
#define   B_ACPI_GPE_CNTL_THRM_POL                     BIT0
#define R_ACPI_DEVACT_STS                            0x44
#define  S_ACPI_DEVACT_STS                            2
#define B_ACPI_DEVACT_STS_MASK                         0x13C1
#define   B_KBC_ACT_STS                                0x1000
#define   B_PIRQDH_ACT_STS                             0x0200
#define   B_PIRQCG_ACT_STS                             0x0100
#define   B_PIRQBF_ACT_STS                             0x0080
#define   B_PIRQAE_ACT_STS                             0x0040
#define   B_IDE_ACT_STS                                0x0001
#define   N_KBC_ACT_STS                                12
#define   N_PIRQDH_ACT_STS                             9
#define   N_PIRQCG_ACT_STS                             8
#define   N_PIRQBF_ACT_STS                             7
#define   N_PIRQAE_ACT_STS                             6
#define   N_IDE_ACT_STS                                0
#define R_ACPI_PM2_CNT                               0x50
#define  B_ARB_DIS                                     0x01
#define R_ACPI_GYS_CNT                               0x50
#define   B_G_STATE                                    0x01
#define R_ACPI_C3_RES                                0x54
#define   B_ACPI_CS_RES_C3_RESIDENCY                   0x00FFFFFF
#define R_ACPI_C5_RES                                0x58
#define   B_ACPI_C5_RES_C5_RESIDENCY                   0x00FFFFFF

//
// TCO register I/O map
//

#define  TCO_BASE                                    0x60
#define  R_TCO_RLD                                   0x0000
#define  R_TCO_DAT_IN                                0x02
#define  R_TCO_DAT_OUT                               0x03

#define  R_TCO1_STS                                  0x04
#define   S_TCO1_STS                                   2
#define    B_DMISERR_STS                                0x1000
#define    B_DMISMI_STS                                 0x0400
#define    B_DMISCI_STS                                 0x0200
#define    B_BIOSWR_STS                                 0x0100
#define    B_NEWCENTURY_STS                             0x0080
#define    B_TIMEOUT                                    0x0008
#define    B_TCO_INT_STS                                0x0004
#define    B_SW_TCO_SMI                                 0x0002
#define    B_NMI2SMI_STS                                0x0001
#define    N_DMISMI_STS                                 10
#define    N_BIOSWR_STS                                 8
#define    N_NEWCENTURY_STS                             7
#define    N_TIMEOUT                                    3
#define    N_SW_TCO_SMI                                 1
#define    N_NMI2SMI_STS                                0

#define  R_TCO2_STS                                  0x06
#define   S_TCO2_STS                                  2
#define    B_ME_WAKE_STS                               0x0020
#define    B_SMLINK_SLV_SMI_STS                        0x0010
#define    B_BOOT_STS                                  0x0004
#define    B_SECOND_TO_STS                             0x0002
#define    B_INTRD_DET                                 0x0001
#define    N_INTRD_DET                                 0

#define  R_TCO1_CNT                                  0x08
#define   S_TCO1_CNT                                  2
#define    B_TCO_LOCK                                  BIT12
#define    B_TCO_TMR_HLT                               BIT11
#define    B_SEND_NOW                                  BIT10
#define    B_NMI2SMI_EN                                BIT9
#define    B_NMI_NOW                                   BIT8
#define    N_NMI2SMI_EN                                9

#define  R_TCO2_CNT                                  0x0A
#define   S_TCO2_CNT                                  2
#define    B_OS_POLICY                                 0x0030
#define    B_GPI11_ALERT_DISABLE                       0x0008
#define    B_INTRD_SEL                                 0x0006
#define    N_INTRD_SEL                                 2

#define  R_TCO_MESSAGE1                              0x0C
#define  R_TCO_MESSAGE2                              0x0D
#define  R_TCO_WDCNT                                 0x0E
#define  R_SW_IRQ_GEN                                0x10
#define    B_TCO_IRQ12_CAUSE                           BIT1
#define    B_TCO_IRQ1_CAUSE                            BIT0
#define  R_TCO_TMR                                   0x12

//
// GPIO Init register offsets from GPIOBASE
//
#define R_GPIO_USE_SEL                               0x00
#define R_GPIO_IO_SEL                                0x04
#define R_GPIO_LVL                                   0x0C
#define R_GPIO_USE_SEL_OVERRIDE_LOW                  0x10
#define R_GPIO_BLINK                                 0x18
#define R_GPIO_SER_BLINK                             0x1C
#define R_GPIO_SB_CMDSTS                             0x20
#define   B_GPIO_SB_CMDSTS_DLS_MASK                    0x00C00000 //Data length select
#define   B_GPIO_SB_CMDSTS_DRS_MASK                    0x003F0000 //Data rate select
#define   B_GPIO_SB_CMDSTS_BUSY                        BIT8
#define   B_GPIO_SB_CMDSTS_GO                          BIT0
#define R_GPIO_SB_DATA                               0x24
#define R_GPIO_INV                                   0x2C
#define R_GPIO_USE_SEL2                              0x30
#define R_GPIO_IO_SEL2                               0x34
#define R_GPIO_LVL2                                  0x38

#define R_GPIO_USE_SEL3                              0x40
#define R_GPIO_IO_SEL3                               0x44
#define R_GPIO_LVL3                                  0x48

#define R_GPIO_RST_SELECT                            0x60

//
// If platform passes into invalid GPIO, ICH should not use it.
//
#define  PLATFORM_INVALID_GPIO                          0xFF
//
// Processor interface registers
//
#define R_NMI_SC                                     0x61
#define   B_NMI_SC_SERR_NMI_STS                        BIT7
#define   B_NMI_SC_IOCHK_NMI_STS                       BIT6
#define   B_NMI_SC_TMR2_OUT_STS                        BIT5
#define   B_NMI_SC_REF_TOGGLE                          BIT4
#define   B_NMI_SC_IOCHK_NMI_EN                        BIT3
#define   B_NMI_SC_PCI_SERR_EN                         BIT2
#define   B_NMI_SC_SPKR_DAT_EN                         BIT1
#define   B_NMI_SC_TIM_CNT2_EN                         BIT0
#define R_NMI_EN                                     0x70
#define   B_NMI_EN_NMI_EN                              BIT7
#define R_NMI_ALT_EN                                 0x74   // ESS_OVERRIDE

//
// RTC register
//
#define R_ICH_RTC_INDEX                              0x70
#define R_ICH_RTC_TARGET                             0x71
#define R_ICH_RTC_EXT_INDEX                          0x72
#define R_ICH_RTC_EXT_TARGET                         0x73
#define R_ICH_RTC_REGA                               0x0A
#define   B_ICH_RTC_REGA_UIP                           0x80
#define R_ICH_RTC_REGB                               0x0B
#define   B_ICH_RTC_REGB_SET                           0x80

//
//  SATA Controller 1 Registers (D31:F2)
//
#define PCI_DEVICE_NUMBER_ICH_SIDE                      31
#define PCI_FUNCTION_NUMBER_ICH_SIDE                    2
#define R_ICH_SIDE_VENDOR_ID                         0x00
#define   V_ICH_SIDE_VENDOR_ID                          V_INTEL_VENDOR_ID
#define R_ICH_SIDE_DEVICE_ID                         0x02
#define   V_ICH_SIDE_DEVICE_ID_BASE_1                   0x2920    // ESS_OVERRIDE
#define   V_ICH_SIDE_DEVICE_ID_BASE_2                   0x3A00    // ESS_OVERRIDE
#define   V_ICH_SIDE_DEVICE_ID_BASE_3                   0x2820    // ESS_OVERRIDE
#define   V_ICH_SIDE_DEVICE_ID_MASK                     0xFFF0    // ESS_OVERRIDE

#define   V_ICH9_SIDE_DEVICE_ID_D_IDE_4_PORTS            0x2920    // Desktop Non-AHCI and Non-RAID Mode (Ports 0,1, 2 and 3)
#define   V_ICH9_SIDE_DEVICE_ID_D_IDE_2_PORTS            0x2921    // Desktop Non-AHCI and Non-RAID Mode (Ports 0 and 1)
#define   V_ICH9_SIDE_DEVICE_ID_D_AHCI_6_PORTS           0x2922    // Desktop AHCI Mode (Ports 0-5)
#define   V_ICH9_SIDE_DEVICE_ID_D_AHCI_4_PORTS           0x2923    // Desktop AHCI Mode (Ports 0,1,4 and 5)
#define   V_ICH9_SIDE_DEVICE_ID_D_RAID                   0x2822    // Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define   V_ICH9_SIDE_DEVICE_ID_D_RAID_ALT               0x2925    // Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define   V_ICH9_SIDE_DEVICE_ID_M_IDE_2_PORTS            0x2928    // Mobile Non-AHCI and Non-RAID Mode (Ports 0 and 1)
#define   V_ICH9_SIDE_DEVICE_ID_M_AHCI_4_PORTS           0x2929    // Mobile AHCI Mode (Ports 0,1,4 and 5)
#define   V_ICH9_SIDE_DEVICE_ID_M_RAID                   0x282A    // Mobile RAID 0/1/5/10, based on D31:F2:9Ch[7]
#define   V_ICH9_SIDE_DEVICE_ID_M_RAID_ALT               0x292C    // Mobile RAID 0/1/5/10, based on D31:F2:9Ch[7]

#define   V_ICH10_SIDE_DEVICE_ID_D_IDE_4_PORTS           0x3A20    // Desktop IDE Mode (Ports 0,1, 2 and 3) ESS_OVERRIDE
#define   V_ICH10_SIDE_DEVICE_ID_D_IDE_2_PORTS           0x3A01    // Desktop IDE Mode (Ports 0 and 1)
#define   V_ICH10_SIDE_DEVICE_ID_D_AHCI_6_PORTS          0x3A22    // Desktop AHCI Mode (Ports 0-5) ESS_OVERRIDE
#define   V_ICH10_SIDE_DEVICE_ID_D_AHCI_4_PORTS          0x3A03    // Desktop AHCI Mode (Ports 0,1,4 and 5)
#define   V_ICH10_SIDE_DEVICE_ID_D_RAID                  0x2822    // Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define   V_ICH10_SIDE_DEVICE_ID_D_RAID_ALT_1            0x3A04    // Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define   V_ICH10_SIDE_DEVICE_ID_D_RAID_ALT_2            0x3A25    // Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7] ESS_OVERRIDE

//
//  SATA Controller 2 Registers (D31:F5)
//
#define PCI_DEVICE_NUMBER_ICH_SIDE2                     31
#define PCI_FUNCTION_NUMBER_ICH_SIDE2                   5
#define R_ICH_SIDE2_VENDOR_ID                        0x00
#define   V_ICH_SIDE2_VENDOR_ID                         V_INTEL_VENDOR_ID
#define R_ICH_SIDE2_DEVICE_ID                        0x02

#define   V_ICH9_SIDE2_DEVICE_ID_D_IDE_2_PORTS          0x2926
#define   V_ICH9_SIDE2_DEVICE_ID_M_IDE_2_PORTS          0x292D

#define   V_ICH10_SIDE2_DEVICE_ID_D_IDE_2_PORTS         0x3A26  // ESS_OVERRIDE

//
//  SATA Controller common Registers
//
#define R_ICH_SIDE_COMMAND                            0x04
#define   B_ICH_SIDE_COMMAND_INT_DIS                  BIT10
#define   B_ICH_SIDE_COMMAND_FBE                      BIT9
#define   B_ICH_SIDE_COMMAND_SERR_EN                  BIT8
#define   B_ICH_SIDE_COMMAND_WCC                      BIT7
#define   B_ICH_SIDE_COMMAND_PER                      BIT6
#define   B_ICH_SIDE_COMMAND_VPS                      BIT5
#define   B_ICH_SIDE_COMMAND_PMWE                     BIT4
#define   B_ICH_SIDE_COMMAND_SCE                      BIT3
#define   B_ICH_SIDE_COMMAND_BME                      BIT2
#define   B_ICH_SIDE_COMMAND_MSE                      BIT1
#define   B_ICH_SIDE_COMMAND_IOSE                     BIT0
#define R_ICH_SIDE_PCISTS                             0x06
#define   B_ICH_SIDE_PCISTS_DPE                       BIT15
#define   B_ICH_SIDE_PCISTS_RMA                       BIT13
#define   B_ICH_SIDE_PCISTS_DEV_STS_MASK              (BIT10 | BIT9)
#define   B_ICH_SIDE_PCISTS_DPED                      BIT8
#define   B_ICH_SIDE_PCISTS_CAP_LIST                  BIT4
#define   B_ICH_SIDE_PCISTS_ITNS                      BIT3
#define R_ICH_SIDE_RID                                0x08
#define R_ICH_SIDE_PI_REGISTER                        0x09
#define   B_ICH_SIDE_PI_REGISTER_SNC                  BIT3
#define   B_ICH_SIDE_PI_REGISTER_SNE                  BIT2
#define   B_ICH_SIDE_PI_REGISTER_PNC                  BIT1
#define   B_ICH_SIDE_PI_REGISTER_PNE                  BIT0
#define R_ICH_SIDE_SUB_CLASS_CODE                     0x0A
#define   V_ICH_SIDE_SUB_CLASS_CODE_IDE               0x01 // && MAP:D31F2R90[7:6]= 00b
#define   V_ICH_SIDE_SUB_CLASS_CODE_AHCI              0x06 // && MAP:D31F2R90[7:6]= 01b
#define   V_ICH_SIDE_SUB_CLASS_CODE_RAID              0x04 // && MAP:D31F2R90[7:6]= 10b
#define R_ICH_SIDE_BCC                                0x0B
#define   B_ICH_SIDE_BCC                                0xFF
#define R_ICH_SIDE_PMLT                               0x0D
#define   B_ICH_SIDE_PMLT                               0xFF
#define R_ICH_SIDE_HTYPE                              0x0E
#define   B_ICH_SIDE_HTYPE_MFD                          BIT7
#define   B_ICH_SIDE_HTYPE_HL                           0x7F
#define R_ICH_SIDE_PCMD_BAR                           0x10
#define   B_ICH_SIDE_PCMD_BAR_BA                        0x0000FFF8
#define   B_ICH_SIDE_PCMD_BAR_RTE                       BIT0
#define R_ICH_SIDE_PCNL_BAR                           0x14
#define   B_ICH_SIDE_PCNL_BAR_BA                        0x0000FFFC
#define   B_ICH_SIDE_PCNL_BAR_RTE                       BIT0
#define R_ICH_SIDE_SCMD_BAR                           0x18
#define   B_ICH_SIDE_SCMD_BAR_BA                        0x0000FFF8
#define   B_ICH_SIDE_SCMD_BAR_RTE                       BIT0
#define R_ICH_SIDE_SCNL_BAR                           0x1C
#define   B_ICH_SIDE_SCNL_BAR_BA                        0x0000FFFC
#define   B_ICH_SIDE_SCNL_BAR_RTE                       BIT0
#define R_ICH_SIDE_BUS_MASTER_BAR                     0x20
#define   B_ICH_SIDE_BUS_MASTER_BAR_BA                  0x0000FFF0
#define   B_ICH_SIDE_BUS_MASTER_BAR_RTE                 BIT0
#define R_ICH_SIDE_AHCI_BAR                           0x24
#define   B_ICH_SIDE_AHCI_BAR_BA                        0xFFFFF000
#define   B_ICH_SIDE_AHCI_BAR_PF                        BIT3
#define   B_ICH_SIDE_AHCI_BAR_TP                        (BIT2 | BIT1)
#define   B_ICH_SIDE_AHCI_BAR_RTE                       BIT0
#define R_ICH_SIDE_AHCI_SVID                          0x2C
#define   B_ICH_SIDE_AHCI_SVID                          0xFFFF
#define R_ICH_SIDE_AHCI_SID                           0x2E
#define   B_ICH_SIDE_AHCI_SID                           0xFFFF
#define R_ICH_SIDE_AHCI_CAP                           0x34
#define   B_ICH_SIDE_AHCI_CAPPTR                        0xFF
#define R_ICH_SIDE_AHCI_INTLN                         0x3C
#define   B_ICH_SIDE_AHCI_INTLN                         0xFF
#define R_ICH_SIDE_AHCI_INTPN                         0x3D
#define    B_ICH_SIDE_AHCI_INTPN                         0xFF
#define R_ICH_SIDE_TIMP                               0x40
#define R_ICH_SIDE_TIMS                               0x42
#define   B_ICH_SIDE_TIM_IDE                          BIT15 // IDE Decode Enable
#define   B_ICH_SIDE_TIM_SITRE                        BIT14 // Drive 1 Timing Register Enable
#define   B_ICH_SIDE_TIM_ISP_MASK                     (BIT13 | BIT12) //IORDY Sample Point
#define     V_ICH_SIDE_TIM_ISP_5_CLOCK                0x00   //IORDY Sample Point = 5 clocks
#define     V_ICH_SIDE_TIM_ISP_4_CLOCK                0x01   //IORDY Sample Point = 4 clocks
#define     V_ICH_SIDE_TIM_ISP_3_CLOCK                0x02   //IORDY Sample Point = 3 clocks
#define   B_ICH_SIDE_TIM_RCT_MASK                     (BIT9 | BIT8) //Recovery time
#define     V_ICH_SIDE_TIM_RCT_4_CLOCK                0x00   //Recovery time = 4 clocks
#define     V_ICH_SIDE_TIM_RCT_3_CLOCK                0x01   //Recovery time = 3 clocks
#define     V_ICH_SIDE_TIM_RCT_2_CLOCK                0x02   //Recovery time = 2 clocks
#define     V_ICH_SIDE_TIM_RCT_1_CLOCK                0x03   //Recovery time = 1 clock
#define   B_ICH_SIDE_TIM_DTE1                         BIT7 //Drive 1 DMA Timing Enable
#define   B_ICH_SIDE_TIM_PPE1                         BIT6 //Drive 1 Prefetch/Posting Enable
#define   B_ICH_SIDE_TIM_IE1                          BIT5 //Drive 1 IORDY Sample Point Enable
#define   B_ICH_SIDE_TIM_TIME1                        BIT4 //Drive 1 Fast Timing Bank
#define   B_ICH_SIDE_TIM_DTE0                         BIT3 //Drive 0 DMA Timing Enable
#define   B_ICH_SIDE_TIM_PPE0                         BIT2 //Drive 0 Prefetch/Posting Enable
#define   B_ICH_SIDE_TIM_IE0                          BIT1 //Drive 0 IORDY Sample Point Enable
#define   B_ICH_SIDE_TIM_TIME0                        BIT0 //Drive 0 Fast Timing Bank
#define R_ICH_SIDE_SIDETIM                            0x44
#define   B_ICH_SIDE_SIDETIM_SISP1_MASK               (BIT7 | BIT6) //IORDY Sample Point
#define   B_ICH_SIDE_SIDETIM_SRCT1_MASK               (BIT5 | BIT4) //Recovery time
#define   B_ICH_SIDE_SIDETIM_PISP1_MASK               (BIT3 | BIT2) //IORDY Sample Point
#define   B_ICH_SIDE_SIDETIM_PRCT1_MASK               (BIT1 | BIT0) //Recovery time
#define R_ICH_SIDE_SDMA_CNT                           0x48
#define   B_ICH_SIDE_SDMA_CNT_SSDE1                   BIT3
#define   B_ICH_SIDE_SDMA_CNT_SSDE0                   BIT2
#define   B_ICH_SIDE_SDMA_CNT_PSDE1                   BIT1
#define   B_ICH_SIDE_SDMA_CNT_PSDE0                   BIT0
#define R_ICH_SIDE_SDMA_TIM                           0x4A
#define   B_ICH_SDMA_TIM_SCT1_MASK                    0x3000
#define   B_ICH_SDMA_TIM_SCT0_MASK                    0x0300
#define   B_ICH_SDMA_TIM_PCT1_MASK                    0x0030
#define   B_ICH_SDMA_TIM_PCT0_MASK                    0x0003
#define     V_ICH_SIDE_SDMA_TIM_CT4_RP6               0x00
#define     V_ICH_SIDE_SDMA_TIM_CT3_RP5               0x01
#define     V_ICH_SIDE_SDMA_TIM_CT2_RP4               0x02
#define     V_ICH_SIDE_SDMA_TIM_CT3_RP8               0x01
#define     V_ICH_SIDE_SDMA_TIM_CT2_RP8               0x02
#define     V_ICH_SIDE_SDMA_TIM_CT3_RP16              0x01
#define R_ICH_SIDE_IDE_CONFIG                         0x54
#define   B_ICH_SIDE_IDE_CONFIG_SEC_SIG_MODE_MASK     (BIT19 | BIT18)
#define   B_ICH_SIDE_IDE_CONFIG_PRIM_SIG_MODE_MASK    (BIT17 | BIT16)
#define   B_ICH_SIDE_IDE_CONFIG_FAST_SCB1             BIT15
#define   B_ICH_SIDE_IDE_CONFIG_FAST_SCB0             BIT14
#define   B_ICH_SIDE_IDE_CONFIG_FAST_PCB1             BIT13
#define   B_ICH_SIDE_IDE_CONFIG_FAST_PCB0             BIT12
#define   B_ICH_SIDE_IDE_CONFIG_SCB1                  BIT3
#define   B_ICH_SIDE_IDE_CONFIG_SCB0                  BIT2
#define   B_ICH_SIDE_IDE_CONFIG_PCB1                  BIT1
#define   B_ICH_SIDE_IDE_CONFIG_PCB0                  BIT0
#define R_ICH_SIDE_PID                                0x70
#define   B_ICH_SIDE_PID_NEXT                           0xFF00
#define     V_ICH_SIDE_PID_NEXT_0                         0x0000
#define     V_ICH_SIDE_PID_NEXT_1                         0xA800
#define   B_ICH_SIDE_PID_CID                            0x00FF
#define R_ICH_SIDE_PC                                 0x72
#define  S_ICH_SIDE_PC                                 2
#define   B_ICH_SIDE_PC_PME                             (BIT15 | BIT14 | BIT13 | BIT12 | BIT11)
#define     V_ICH_SIDE_PC_PME_0                           0x0000
#define     V_ICH_SIDE_PC_PME_1                           0x8000
#define   B_ICH_SIDE_PC_D2_SUP                          BIT10
#define   B_ICH_SIDE_PC_D1_SUP                          BIT9
#define   B_ICH_SIDE_PC_AUX_CUR                         (BIT8 | BIT7 | BIT6)
#define   B_ICH_SIDE_PC_DSI                             BIT5
#define   B_ICH_SIDE_PC_PME_CLK                         BIT3
#define   B_ICH_SIDE_PC_VER                             (BIT2 | BIT1 | BIT0)
#define R_ICH_SIDE_PMCS                               0x74
#define   B_ICH_SIDE_PMCS_PMES                          BIT15
#define   B_ICH_SIDE_PMCS_PMEE                          BIT8
#define   B_ICH_SIDE_PMCS_NSFRST                        BIT3
#define     V_ICH_SIDE_PMCS_NSFRST_1                      0x01
#define     V_ICH_SIDE_PMCS_NSFRST_0                      0x00
#define   B_ICH_SIDE_PMCS_PS                            (BIT1 | BIT0)
#define     V_ICH_SIDE_PMCS_PS_3                        0x03
#define     V_ICH_SIDE_PMCS_PS_0                        0x00
#define R_ICH_SIDE_MID                                0x80
#define   B_ICH_SIDE_MID_NEXT                           0xFF00
#define   B_ICH_SIDE_MID_CID                            0x00FF
#define R_ICH_SIDE_MC                                 0x82
#define   B_ICH_SIDE_MC_C64                             BIT7
#define   B_ICH_SIDE_MC_MME                             (BIT6 | BIT5 | BIT4)
#define     V_ICH_SIDE_MC_MME_4                         0x04
#define     V_ICH_SIDE_MC_MME_2                         0x02
#define     V_ICH_SIDE_MC_MME_1                         0x01
#define     V_ICH_SIDE_MC_MME_0                         0x00
#define   B_ICH_SIDE_MC_MMC                             (BIT3 | BIT2 | BIT1)
#define     V_ICH_SIDE_MC_MMC_4                           0x04
#define     V_ICH_SIDE_MC_MMC_0                           0x00
#define   B_ICH_SIDE_MC_MSIE                            BIT0
#define     V_ICH_SIDE_MC_MSIE_1                          0x01
#define     V_ICH_SIDE_MC_MSIE_0                          0x00
#define R_ICH_SIDE_MA                                 0x84
#define   B_ICH_SIDE_MA                                 0xFFFFFFFC
#define R_ICH_SIDE_MD                                 0x88
#define   B_ICH_SIDE_MD_MSIMD                           0xFFFF
#define R_ICH_SIDE_MAP                                0x90
#define   B_ICH_SIDE_PORT5_DISABLED                   BIT13
#define   B_ICH_SIDE_PORT4_DISABLED                   BIT12
#define   B_ICH_SIDE_PORT3_DISABLED                   BIT11
#define   B_ICH_SIDE_PORT2_DISABLED                   BIT10
#define   B_ICH_SIDE_PORT1_DISABLED                   BIT9
#define   B_ICH_SIDE_PORT0_DISABLED                   BIT8
#define   B_ICH_SIDE2_PORT5_DISABLED                  BIT9
#define   B_ICH_SIDE2_PORT4_DISABLED                  BIT8
#define   B_ICH_SIDE_MAP_SMS_MASK                     (BIT7 | BIT6)
#define     V_ICH_SIDE_MAP_SMS_IDE                      0x00
#define     V_ICH_SIDE_MAP_SMS_AHCI                     0x40
#define     V_ICH_SIDE_MAP_SMS_RAID                     0x80
#define   B_ICH_SIDE_PORT_TO_CONTROLLER_CFG           BIT5
//
// ESS_OVERRIDE_START
//
#define   B_ICH_SIDE_MAP_MSM_MASK                       0x3FE0    //   BIT13:8| BIT7:6 | BIT5
#define     V_ICH_SIDE_MAP_MSM_IDE                        0x0000
#define     V_ICH_SIDE_MAP_MSM_AHCI                       0x0060  // AHCI mode, SATA2 disabled, SATA1 control 6 SATA ports
#define     V_ICH_SIDE_MAP_MSM_RAID                       0x00A0  // RAID mode, SATA2 disabled, SATA1 control 6 SATA ports
//
// ESS_OVERRIDE_START
//
#define R_ICH_SIDE_PCS                                0x92
#define   S_ICH_SIDE_PCS                              0x2
#define    B_ICH_SIDE_PCS_OOB_RETRY                   BIT15
#define    B_ICH_SIDE_PCS_PORT5_DET                   BIT13
#define    B_ICH_SIDE_PCS_PORT4_DET                   BIT12
#define    B_ICH_SIDE_PCS_PORT3_DET                   BIT11
#define    B_ICH_SIDE_PCS_PORT2_DET                   BIT10
#define    B_ICH_SIDE_PCS_PORT1_DET                   BIT9
#define    B_ICH_SIDE_PCS_PORT0_DET                   BIT8
#define    B_ICH_SIDE_PCS_PORT5_EN                    BIT5
#define    B_ICH_SIDE_PCS_PORT4_EN                    BIT4
#define    B_ICH_SIDE_PCS_PORT3_EN                    BIT3
#define    B_ICH_SIDE_PCS_PORT2_EN                    BIT2
#define    B_ICH_SIDE_PCS_PORT1_EN                    BIT1
#define    B_ICH_SIDE_PCS_PORT0_EN                    BIT0
#define    B_ICH_SIDE2_PCS_PORT5_DET                  BIT9
#define    B_ICH_SIDE2_PCS_PORT4_DET                  BIT8
#define    B_ICH_SIDE2_PCS_PORT5_EN                   BIT1
#define    B_ICH_SIDE2_PCS_PORT4_EN                   BIT0
#define R_ICH_SIDE_SCLKCG                             0x94
#define    B_ICH_SIDE_SCLKCG_PORT5_PCD                BIT29
#define    B_ICH_SIDE_SCLKCG_PORT4_PCD                BIT28
#define    B_ICH_SIDE_SCLKCG_PORT3_PCD                BIT27
#define    B_ICH_SIDE_SCLKCG_PORT2_PCD                BIT26
#define    B_ICH_SIDE_SCLKCG_PORT1_PCD                BIT25
#define    B_ICH_SIDE_SCLKCG_PORT0_PCD                BIT24
#define R_ICH_SIDE_SCLKGC                             0x9C
#define    B_ICH_SIDE_SCLKGC_AIE                      BIT7
#define    B_ICH_SIDE_SCLKGC_SATA2PIND                BIT1
#define    B_ICH_SIDE_SCLKGC_SATA4PMIND               BIT0
#define R_ICH_SIDE_SIRI                               0xA0
#define   B_ICH_SIDE_SIRI_IDX                           0xFC
#define R_ICH_SIDE_STRD                               0xA4
#define   B_ICH_SIDE_STRD_DTA                           0xFFFFFFFF
#define R_ICH_SIDE_CR0                                0xA8
#define   B_ICH_SIDE_CR0_MAJREV                         0x00F00000
#define   B_ICH_SIDE_CR0_MINREV                         0x000F0000
#define   B_ICH_SIDE_CR0_NEXT                           0x0000FF00
#define   B_ICH_SIDE_CR0_CAP                            0x000000FF
#define R_ICH_SIDE_CR1                                0xAC
#define   B_ICH_SIDE_CR1_BAROFST                        0xFFF0
#define   B_ICH_SIDE_CR1_BARLOC                         0x000F
#define R_ICH_SIDE_FLR_CID                            0xB0
#define   B_ICH_SIDE_FLR_CID_NEXT                       0xFF00
#define   B_ICH_SIDE_FLR_CID                            0x00FF
#define     V_ICH_SIDE_FLR_CID_1                          0x0013
#define     V_ICH_SIDE_FLR_CID_0                          0x0009
#define R_ICH_SIDE_FLR_CLV                            0xB2
#define   B_ICH_SIDE_FLR_CLV_FLRC_FLRCSSEL_0            BIT9
#define   B_ICH_SIDE_FLR_CLV_TXPC_FLRCSSEL_0            BIT8
#define   B_ICH_SIDE_FLR_CLV_VSCID_FLRCSSEL_0           0x00FF
#define   B_ICH_SIDE_FLR_CLV_CID_FLRCSSEL_1             0xF000
#define     V_ICH_SIDE_FLR_CLV_CID_FLRCSSEL_1             0x2000
#define   B_ICH_SIDE_FLR_CLV_CV_FLRCSSEL_1              0x0F00
#define   B_ICH_SIDE_FLR_CLV_VSCID_FLRCSSEL_1           0x00FF
#define   V_ICH_SIDE_FLR_CLV_VSCID_FLRCSSEL             0x0006
#define R_ICH_SIDE_FLRC                               0xB4
#define   B_ICH_SIDE_FLRC_TXP                           BIT8
#define   B_ICH_SIDE_FLRC_INITFLR                       BIT0
#define R_ICH_SIDE_ATC                                0xC0
#define   B_ICH_SIDE_ATC_SST                            BIT3
#define   B_ICH_SIDE_ATC_SPT                            BIT2
#define   B_ICH_SIDE_ATC_PST                            BIT1
#define   B_ICH_SIDE_ATC_PMT                            BIT0
#define R_ICH_SIDE_ATS                                0xC4
#define   B_ICH_SIDE_ATS_SST                            BIT3
#define   B_ICH_SIDE_ATS_SPT                            BIT2
#define   B_ICH_SIDE_ATS_PST                            BIT1
#define   B_ICH_SIDE_ATS_PMT                            BIT0
#define R_ICH_SIDE_SP                                 0xD0
#define   B_ICH_SIDE_SP                                 0xFFFFFFFF
#define R_ICH_SIDE_BFCS                               0xE0
#define   B_ICH_SIDE_BFCS_P2BFI                         BIT12
#define   B_ICH_SIDE_BFCS_P2BFS                         BIT11
#define   B_ICH_SIDE_BFCS_P2BFF                         BIT10
#define   B_ICH_SIDE_BFCS_P1BFI                         BIT9
#define   B_ICH_SIDE_BFCS_P0BFI                         BIT8
#define   B_ICH_SIDE_BFCS_BIST_FIS_T                    BIT7
#define   B_ICH_SIDE_BFCS_BIST_FIS_A                    BIT6
#define   B_ICH_SIDE_BFCS_BIST_FIS_S                    BIT5
#define   B_ICH_SIDE_BFCS_BIST_FIS_L                    BIT4
#define   B_ICH_SIDE_BFCS_BIST_FIS_F                    BIT3
#define   B_ICH_SIDE_BFCS_BIST_FIS_P                    BIT2
#define R_ICH_SIDE_BFTD1                              0xE4
#define   B_ICH_SIDE_BFTD1                              0xFFFFFFFF
#define R_ICH_SIDE_BFTD2                              0xE8
#define   B_ICH_SIDE_BFTD2                              0xFFFFFFFF

//
// SATA Indexed Registers
//
#define R_SATA_INDEXED_REG_STTT1                        0x00
#define   B_PORT1_TX_TERM_TEST_ENABLE                     BIT1
#define   B_PORT0_TX_TERM_TEST_ENABLE                     BIT0
#define R_SATA_INDEXED_REG_SIR18                        0x18
#define   B_PORT1_GEN2_TX_INIT_FIELD1_MASK                (BIT5 | BIT4 | BIT3)
#define   B_PORT0_GEN2_TX_INIT_FIELD1_MASK                (BIT2 | BIT1 | BIT0)
#define   N_PORT1_GEN2_TX_INIT_FIELD1                     3
#define   N_PORT0_GEN2_TX_INIT_FIELD1                     0
#define    V_P0P1G2F1_SHORT                               0x02
#define    V_P0P1G2F1_INTERNAL_SATA                       0x03
#define    V_P0P1G2F1_MOBILE_DIRECT_CONNECT               0x03
#define    V_P0P1G2F1_MOBILE_DOCKING                      0x03
#define    V_P0P1G2F1_ESATA                               0x04
#define R_SATA_INDEXED_REG_STME                         0x1C
#define   B_TEST_MODE_ENABLE_BIT                          BIT18
#define R_SATA_INDEXED_REG_SIR28                        0x28
#define R_SATA_INDEXED_REG_SIR40                        0x40
#define R_SATA_INDEXED_REG_STTT2                        0x74
#define   B_PORT3_TX_TERM_TEST_ENABLE                     BIT17
#define   B_PORT2_TX_TERM_TEST_ENABLE                     BIT16
#define R_SATA_INDEXED_REG_SIR78                        0x78
#define R_SATA_INDEXED_REG_SIR84                        0x84
#define   B_PORT3_GEN2_TX_INIT_FIELD1_MASK                (BIT5 | BIT4 | BIT3)
#define   B_PORT2_GEN2_TX_INIT_FIELD1_MASK                (BIT2 | BIT1 | BIT0)
#define   N_PORT3_GEN2_TX_INIT_FIELD1                     3
#define   N_PORT2_GEN2_TX_INIT_FIELD1                     0
#define    V_P2P3G2F1_SHORT                               0x02
#define    V_P2P3G2F1_INTERNAL_SATA                       0x03
#define    V_P2P3G2F1_ESATA                               0x04
#define R_SATA_INDEXED_REG_SIR88                        0x88
#define   B_PORT3_GEN2_TX_INIT_FIELD2_MASK                (BIT29 | BIT28 | BIT27)
#define   B_PORT2_GEN2_TX_INIT_FIELD2_MASK                (BIT26 | BIT25 | BIT24)
#define   N_PORT3_GEN2_TX_INIT_FIELD2                     27
#define   N_PORT2_GEN2_TX_INIT_FIELD2                     24
#define    V_P2P3G2F2_SHORT                               0x02
#define    V_P2P3G2F2_INTERNAL_SATA                       0x04
#define    V_P2P3G2F2_ESATA                               0x04
#define   B_PORT1_GEN2_TX_INIT_FIELD2_MASK                (BIT21 | BIT20 | BIT19)
#define   B_PORT0_GEN2_TX_INIT_FIELD2_MASK                (BIT18 | BIT17 | BIT16)
#define   N_PORT1_GEN2_TX_INIT_FIELD2                     19
#define   N_PORT0_GEN2_TX_INIT_FIELD2                     16
#define    V_P0P1G2F2_SHORT                               0x02
#define    V_P0P1G2F2_INTERNAL_SATA                       0x04
#define    V_P0P1G2F2_ESATA                               0x04
#define    V_P0P1G2F2_MOBILE_DIRECT_CONNECT               0x04
#define    V_P0P1G2F2_MOBILE_DOCKING                      0x04
#define   B_PORT3_GEN1_TX_INIT_FIELD2_MASK                (BIT13 | BIT12 | BIT11)
#define   B_PORT2_GEN1_TX_INIT_FIELD2_MASK                (BIT10 | BIT9 | BIT8)
#define   N_PORT3_GEN1_TX_INIT_FIELD2                     11
#define   N_PORT2_GEN1_TX_INIT_FIELD2                     8
#define    V_P2P3G1F2_SHORT                               0x01
#define    V_P2P3G1F2_INTERNAL_SATA                       0x02
#define    V_P2P3G1F2_ESATA                               0x02
#define   B_PORT1_GEN1_TX_INIT_FIELD2_MASK                (BIT5 | BIT4 | BIT3)
#define   B_PORT0_GEN1_TX_INIT_FIELD2_MASK                (BIT2 | BIT1 | BIT0)
#define   N_PORT1_GEN1_TX_INIT_FIELD2                     3
#define   N_PORT0_GEN1_TX_INIT_FIELD2                     0
#define    V_P0P1G1F2_SHORT                               0x01
#define    V_P0P1G1F2_INTERNAL_SATA                       0x02
#define    V_P0P1G1F2_ESATA                               0x02
#define    V_P0P1G1F2_MOBILE_DIRECT_CONNECT               0x04
#define    V_P0P1G1F2_MOBILE_DOCKING                      0x04
#define R_SATA_INDEXED_REG_SIR8C                        0x8C
#define   B_PORT3_GEN1_TX_INIT_FIELD1_MASK                (BIT29 | BIT28 | BIT27)
#define   B_PORT2_GEN1_TX_INIT_FIELD1_MASK                (BIT26 | BIT25 | BIT24)
#define   N_PORT3_GEN1_TX_INIT_FIELD1                     27
#define   N_PORT2_GEN1_TX_INIT_FIELD1                     24
#define    V_P2P3G1F1_SHORT                               0x01
#define    V_P2P3G1F1_INTERNAL_SATA                       0x02
#define    V_P2P3G1F1_ESATA                               0x02
#define   B_PORT1_GEN1_TX_INIT_FIELD1_MASK                (BIT21 | BIT20 | BIT19)
#define   B_PORT0_GEN1_TX_INIT_FIELD1_MASK                (BIT18 | BIT17 | BIT16)
#define   N_PORT1_GEN1_TX_INIT_FIELD1                     19
#define   N_PORT0_GEN1_TX_INIT_FIELD1                     16
#define    V_P0P1G1F1_SHORT                               0x01
#define    V_P0P1G1F1_INTERNAL_SATA                       0x02
#define    V_P0P1G1F1_ESATA                               0x02
#define    V_P0P1G1F1_MOBILE_DIRECT_CONNECT               0x02
#define    V_P0P1G1F1_MOBILE_DOCKING                      0x01
#define R_SATA_INDEXED_REG_STTT3                        0x90
#define   B_PORT5_TX_TERM_TEST_ENABLE                     BIT1
#define   B_PORT4_TX_TERM_TEST_ENABLE                     BIT0
#define R_SATA_INDEXED_REG_SIR94                        0x94
#define R_SATA_INDEXED_REG_SIRA0                        0xA0
#define   B_PORT5_GEN2_TX_INIT_FIELD1_MASK                (BIT5 | BIT4 | BIT3)
#define   B_PORT4_GEN2_TX_INIT_FIELD1_MASK                (BIT2 | BIT1 | BIT0)
#define   N_PORT5_GEN2_TX_INIT_FIELD1                     3
#define   N_PORT4_GEN2_TX_INIT_FIELD1                     0
#define    V_P4P5G2F1_SHORT                               0x01
#define    V_P4P5G2F1_INTERNAL_SATA                       0x02
#define    V_P4P5G2F1_MOBILE_DIRECT_CONNECT               0x02
#define    V_P4P5G2F1_MOBILE_DOCKING                      0x02
#define    V_P4P5G2F1_ESATA                               0x01
#define R_SATA_INDEXED_REG_SIRA8                        0xA8
#define   B_PORT5_GEN2_TX_INIT_FIELD2_MASK                (BIT21 | BIT20 | BIT19)
#define   B_PORT4_GEN2_TX_INIT_FIELD2_MASK                (BIT18 | BIT17 | BIT16)
#define   N_PORT5_GEN2_TX_INIT_FIELD2                     19
#define   N_PORT4_GEN2_TX_INIT_FIELD2                     16
#define    V_P4P5G2F2_SHORT                               0x02
#define    V_P4P5G2F2_INTERNAL_SATA                       0x04
#define    V_P4P5G2F2_ESATA                               0x04
#define    V_P4P5G2F2_MOBILE_DIRECT_CONNECT               0x04
#define    V_P4P5G2F2_MOBILE_DOCKING                      0x04
#define   B_PORT5_GEN1_TX_INIT_FIELD2_MASK                (BIT5 | BIT4 | BIT3)
#define   B_PORT4_GEN1_TX_INIT_FIELD2_MASK                (BIT2 | BIT1 | BIT0)
#define   N_PORT5_GEN1_TX_INIT_FIELD2                     3
#define   N_PORT4_GEN1_TX_INIT_FIELD2                     0
#define    V_P4P5G1F2_SHORT                               0x01
#define    V_P4P5G1F2_INTERNAL_SATA                       0x02
#define    V_P4P5G1F2_ESATA                               0x02
#define    V_P4P5G1F2_MOBILE_DIRECT_CONNECT               0x04
#define    V_P4P5G1F2_MOBILE_DOCKING                      0x04
#define R_SATA_INDEXED_REG_SIRAC                        0xAC
#define   B_PORT5_GEN1_TX_INIT_FIELD1_MASK                (BIT21 | BIT20 | BIT19)
#define   B_PORT4_GEN1_TX_INIT_FIELD1_MASK                (BIT18 | BIT17 | BIT16)
#define   N_PORT5_GEN1_TX_INIT_FIELD1                     19
#define   N_PORT4_GEN1_TX_INIT_FIELD1                     16
#define    V_P4P5G1F1_SHORT                               0x01
#define    V_P4P5G1F1_INTERNAL_SATA                       0x02
#define    V_P4P5G1F1_ESATA                               0x02
#define    V_P4P5G1F1_MOBILE_DIRECT_CONNECT               0x02
#define    V_P4P5G1F1_MOBILE_DOCKING                      0x01

//
// AHCI BAR Area related Registers
//
#define R_ICH_SATA_AHCI_CAP                           0x0
#define   B_ICH_SATA_AHCI_CAP_S64A                      BIT31
#define   B_ICH_SATA_AHCI_CAP_SCQA                      BIT30
#define   B_ICH_SATA_AHCI_CAP_SSNTF                     BIT29
#define   B_ICH_SATA_AHCI_CAP_SIS                       BIT28 // Supports Interlock Switch
#define   B_ICH_SATA_AHCI_CAP_SSS                       BIT27 // Supports Stagger Spin-up
#define   B_ICH_SATA_AHCI_CAP_SALP                      BIT26
#define   B_ICH_SATA_AHCI_CAP_SAL                       BIT25
#define   B_ICH_SATA_AHCI_CAP_SCLO                      BIT24 // Supports Command List Override
#define   B_ICH_SATA_AHCI_CAP_ISS_MASK                  (BIT23 | BIT22 | BIT21 | BIT20) // Interface Speed Support
#define     N_ICH_SATA_AHCI_CAP_ISS                       20
#define     V_ICH_SATA_AHCI_CAP_ISS_1_5_G                 0x01
#define     V_ICH_SATA_AHCI_CAP_ISS_3_0_G                 0x02
#define   B_ICH_SATA_AHCI_CAP_SNZO                      BIT19
#define   B_ICH_SATA_AHCI_CAP_SAM                       BIT18
#define   B_ICH_SATA_AHCI_CAP_PMS                       BIT17 // Supports Port Multiplier
#define   B_ICH_SATA_AHCI_CAP_PMFS                      BIT16
#define   B_ICH_SATA_AHCI_CAP_PMD                       BIT15 // PIO Multiple DRQ Block
#define   B_ICH_SATA_AHCI_CAP_SSC                       BIT14
#define   B_ICH_SATA_AHCI_CAP_PSC                       BIT13
#define   B_ICH_SATA_AHCI_CAP_NCS                       0x1F00
#define   B_ICH_SATA_AHCI_CAP_CCCS                      BIT7
#define   B_ICH_SATA_AHCI_CAP_EMS                       BIT6
#define   B_ICH_SATA_AHCI_CAP_SXS                       BIT5 // External SATA is supported
#define   B_ICH_SATA_AHCI_CAP_NPS                       0x0010

#define R_ICH_SATA_AHCI_GHC                           0x04
#define   B_ICH_SATA_AHCI_GHC_AE                        BIT31
#define   B_ICH_SATA_AHCI_GHC_MRSM                      BIT2
#define   B_ICH_SATA_AHCI_GHC_IE                        BIT1
#define   B_ICH_SATA_AHCI_GHC_HR                        BIT0

#define R_ICH_SATA_AHCI_IS                            0x08
#define   B_ICH_SATA_AHCI_IS_PORT5                      BIT5
#define   B_ICH_SATA_AHCI_IS_PORT4                      BIT4
#define   B_ICH_SATA_AHCI_IS_PORT3                      BIT3
#define   B_ICH_SATA_AHCI_IS_PORT2                      BIT2
#define   B_ICH_SATA_AHCI_IS_PORT1                      BIT1
#define   B_ICH_SATA_AHCI_IS_PORT0                      BIT0
#define R_ICH_SATA_AHCI_PI                            0x0C
#define   B_ICH_SATA_PORT_MASK                          0x3F
#define   B_ICH_SATA_PORT5_IMPLEMENTED                  BIT5
#define   B_ICH_SATA_PORT4_IMPLEMENTED                  BIT4
#define   B_ICH_SATA_PORT3_IMPLEMENTED                  BIT3
#define   B_ICH_SATA_PORT2_IMPLEMENTED                  BIT2
#define   B_ICH_SATA_PORT1_IMPLEMENTED                  BIT1
#define   B_ICH_SATA_PORT0_IMPLEMENTED                  BIT0
#define R_ICH_SATA_AHCI_VS                            0x10
#define   B_ICH_SATA_AHCI_VS_MJR                        0xFFFF0000
#define   B_ICH_SATA_AHCI_VS_MNR                        0x0000FFFF
#define R_ICH_SATA_AHCI_CCC_CTL                       0x14
#define  S_ICH_SATA_AHCI_CCC_CTL                       4
#define   B_ICH_SATA_AHCI_CCC_CTL_TV                    0xFFFF0000
#define   B_ICH_SATA_AHCI_CCC_CTL_CC                    0x0000FF00
#define   B_ICH_SATA_AHCI_CCC_CTL_INT                   0x00000000F8
#define   B_ICH_SATA_AHCI_CCC_CTL_EN                    BIT0
#define R_ICH_SATA_AHCI_CCC_PORTS                     0x18
#define   B_ICH_SATA_AHCI_CCC_PORTS                     0xFFFFFFFF
#define R_ICH_SATA_AHCI_EM_LOC                        0x1C
#define   B_ICH_SATA_AHCI_EM_LOC_OFST                   0xFFFF0000
#define   B_ICH_SATA_AHCI_EM_LOC_SZ                     0x0000FFFF
#define R_ICH_SATA_AHCI_EM_CTRL                       0x20
#define   B_ICH_SATA_AHCI_EM_CTRL_ATTR_ALHD             BIT26
#define   B_ICH_SATA_AHCI_EM_CTRL_ATTR_XMT              BIT25
#define   B_ICH_SATA_AHCI_EM_CTRL_ATTR_SMB              BIT24
#define   B_ICH_SATA_AHCI_EM_CTRL_SUPP_SGPIO            BIT19
#define   B_ICH_SATA_AHCI_EM_CTRL_SUPP_SES2             BIT18
#define   B_ICH_SATA_AHCI_EM_CTRL_SUPP_SAFTE            BIT17
#define   B_ICH_SATA_AHCI_EM_CTRL_SUPP_LED              BIT16
#define   B_ICH_SATA_AHCI_EM_CTRL_RST                   BIT9
#define   B_ICH_SATA_AHCI_EM_CTRL_CTL_TM                BIT8
#define   B_ICH_SATA_AHCI_EM_CTRL_STS_MR                BIT0
#define R_ICH_SATA_AHCI_VSP                           0xA0
#define   B_ICH_SATA_AHCI_VSP                           BIT0

//
// AHCI port offset values
//
#define EFI_AHCI_PORT_START                           0x0100
#define EFI_AHCI_PORT_REG_WIDTH                       0x0080
#define EFI_AHCI_PORT_CLB                             0x0000
#define EFI_AHCI_PORT_CLBU                            0x0004
#define EFI_AHCI_PORT_FB                              0x0008
#define EFI_AHCI_PORT_FBU                             0x000C
#define EFI_AHCI_PORT_IS                              0x0010
#define EFI_AHCI_PORT_IE                              0x0014
#define EFI_AHCI_PORT_CMD                             0x0018

#define R_ICH_SATA_AHCI_P0CLB                         0x100
#define R_ICH_SATA_AHCI_P1CLB                         0x180
#define R_ICH_SATA_AHCI_P2CLB                         0x200
#define R_ICH_SATA_AHCI_P3CLB                         0x280
#define R_ICH_SATA_AHCI_P4CLB                         0x300
#define R_ICH_SATA_AHCI_P5CLB                         0x380
#define   B_ICH_SATA_AHCI_PXCLB                         0xFFFFFC00
#define R_ICH_SATA_AHCI_P0CLBU                        0x104
#define R_ICH_SATA_AHCI_P1CLBU                        0x184
#define R_ICH_SATA_AHCI_P2CLBU                        0x204
#define R_ICH_SATA_AHCI_P3CLBU                        0x284
#define R_ICH_SATA_AHCI_P4CLBU                        0x304
#define R_ICH_SATA_AHCI_P5CLBU                        0x384
#define   B_ICH_SATA_AHCI_PXCLBU                        0xFFFFFFFF
#define R_ICH_SATA_AHCI_P0FB                          0x108
#define R_ICH_SATA_AHCI_P1FB                          0x188
#define R_ICH_SATA_AHCI_P2FB                          0x208
#define R_ICH_SATA_AHCI_P3FB                          0x288
#define R_ICH_SATA_AHCI_P4FB                          0x308
#define R_ICH_SATA_AHCI_P5FB                          0x388
#define   B_ICH_SATA_AHCI_PXFB                          0xFFFFFF00
#define R_ICH_SATA_AHCI_P0FBU                         0x10C
#define R_ICH_SATA_AHCI_P1FBU                         0x18C
#define R_ICH_SATA_AHCI_P2FBU                         0x20C
#define R_ICH_SATA_AHCI_P3FBU                         0x28C
#define R_ICH_SATA_AHCI_P4FBU                         0x30C
#define R_ICH_SATA_AHCI_P5FBU                         0x38C
#define   B_ICH_SATA_AHCI_PXFBU                         0xFFFFFFF8
#define R_ICH_SATA_AHCI_P0IS                          0x110
#define R_ICH_SATA_AHCI_P1IS                          0x190
#define R_ICH_SATA_AHCI_P2IS                          0x210
#define R_ICH_SATA_AHCI_P3IS                          0x290
#define R_ICH_SATA_AHCI_P4IS                          0x310
#define R_ICH_SATA_AHCI_P5IS                          0x390
#define   B_ICH_SATA_AHCI_PXIS_CPDS                     BIT31
#define   B_ICH_SATA_AHCI_PXIS_TFES                     BIT30
#define   B_ICH_SATA_AHCI_PXIS_HBFS                     BIT29
#define   B_ICH_SATA_AHCI_PXIS_HBDS                     BIT28
#define   B_ICH_SATA_AHCI_PXIS_IFS                      BIT27
#define   B_ICH_SATA_AHCI_PXIS_INFS                     BIT26
#define   B_ICH_SATA_AHCI_PXIS_OFS                      BIT24
#define   B_ICH_SATA_AHCI_PXIS_IPMS                     BIT23
#define   B_ICH_SATA_AHCI_PXIS_PRCS                     BIT22
#define   B_ICH_SATA_AHCI_PXIS_DIS                      BIT7
#define   B_ICH_SATA_AHCI_PXIS_PCS                      BIT6
#define   B_ICH_SATA_AHCI_PXIS_DPS                      BIT5
#define   B_ICH_SATA_AHCI_PXIS_UFS                      BIT4
#define   B_ICH_SATA_AHCI_PXIS_SDBS                     BIT3
#define   B_ICH_SATA_AHCI_PXIS_DSS                      BIT2
#define   B_ICH_SATA_AHCI_PXIS_PSS                      BIT1
#define   B_ICH_SATA_AHCI_PXIS_DHRS                     BIT0
#define R_ICH_SATA_AHCI_P0IE                          0x114
#define R_ICH_SATA_AHCI_P1IE                          0x194
#define R_ICH_SATA_AHCI_P2IE                          0x214
#define R_ICH_SATA_AHCI_P3IE                          0x294
#define R_ICH_SATA_AHCI_P4IE                          0x314
#define R_ICH_SATA_AHCI_P5IE                          0x394
#define   B_ICH_SATA_AHCI_PXIE_CPDE                     BIT31
#define   B_ICH_SATA_AHCI_PXIE_TFEE                     BIT30
#define   B_ICH_SATA_AHCI_PXIE_HBFE                     BIT29
#define   B_ICH_SATA_AHCI_PXIE_HBDE                     BIT28
#define   B_ICH_SATA_AHCI_PXIE_IFE                      BIT27
#define   B_ICH_SATA_AHCI_PXIE_INFE                     BIT26
#define   B_ICH_SATA_AHCI_PXIE_OFE                      BIT24
#define   B_ICH_SATA_AHCI_PXIE_IPME                     BIT23
#define   B_ICH_SATA_AHCI_PXIE_PRCE                     BIT22
#define   B_ICH_SATA_AHCI_PXIE_DIE                      BIT7
#define   B_ICH_SATA_AHCI_PXIE_PCE                      BIT6
#define   B_ICH_SATA_AHCI_PXIE_DPE                      BIT5
#define   B_ICH_SATA_AHCI_PXIE_UFIE                     BIT4
#define   B_ICH_SATA_AHCI_PXIE_SDBE                     BIT3
#define   B_ICH_SATA_AHCI_PXIE_DSE                      BIT2
#define   B_ICH_SATA_AHCI_PXIE_PSE                      BIT1
#define   B_ICH_SATA_AHCI_PXIE_DHRE                     BIT0
#define R_ICH_SATA_AHCI_P0CMD                         0x118
#define R_ICH_SATA_AHCI_P1CMD                         0x198
#define R_ICH_SATA_AHCI_P2CMD                         0x218
#define R_ICH_SATA_AHCI_P3CMD                         0x298
#define R_ICH_SATA_AHCI_P4CMD                         0x318
#define R_ICH_SATA_AHCI_P5CMD                         0x398
#define   B_ICH_SATA_AHCI_PxCMD_ICC                     (BIT31 | BIT30 | BIT29 | BIT28)
#define   B_ICH_SATA_AHCI_PxCMD_ASP                     BIT27
#define   B_ICH_SATA_AHCI_PxCMD_ALPE                    BIT26
#define   B_ICH_SATA_AHCI_PxCMD_DLAE                    BIT25
#define   B_ICH_SATA_AHCI_PxCMD_ATAPI                   BIT24
#define   B_ICH_SATA_AHCI_PxCMD_MASK                    (BIT21 | BIT19 | BIT18 )
#define   B_ICH_SATA_AHCI_PxCMD_HPCP                    BIT18 // Hotplug capable
#define   B_ICH_SATA_AHCI_PxCMD_ISP                     BIT19 // Interlock switch attached
#define   B_ICH_SATA_AHCI_PxCMD_ESP                     BIT21 // Used with an external SATA device
#define   B_ICH_SATA_AHCI_PxCMD_PMA                     BIT17
#define   B_ICH_SATA_AHCI_PxCMD_PMFSE                   BIT16
#define   B_ICH_SATA_AHCI_PxCMD_CR                      BIT15
#define   B_ICH_SATA_AHCI_PxCMD_FR                      BIT14
#define   B_ICH_SATA_AHCI_PxCMD_ISS                     BIT13
#define   B_ICH_SATA_AHCI_PxCMD_CCS                     0x00001F00
#define   B_ICH_SATA_AHCI_PxCMD_FRE                     BIT4
#define   B_ICH_SATA_AHCI_PxCMD_CLO                     BIT3
#define   B_ICH_SATA_AHCI_PxCMD_POD                     BIT2
#define   B_ICH_SATA_AHCI_PxCMD_SUD                     BIT1
#define   B_ICH_SATA_AHCI_PxCMD_ST                      BIT0
#define R_ICH_SATA_AHCI_P0TFD                         0x120
#define R_ICH_SATA_AHCI_P1TFD                         0x1A0
#define R_ICH_SATA_AHCI_P2TFD                         0x220
#define R_ICH_SATA_AHCI_P3TFD                         0x2A0
#define R_ICH_SATA_AHCI_P4TFD                         0x320
#define R_ICH_SATA_AHCI_P5TFD                         0x3A0
#define   B_ICH_SATA_AHCI_PXTFD_ERR                     0x0000FF00
#define   B_ICH_SATA_AHCI_PXTFD_STS                     0x000000FF
#define R_ICH_SATA_AHCI_P0SIG                         0x124
#define R_ICH_SATA_AHCI_P1SIG                         0x1A4
#define R_ICH_SATA_AHCI_P2SIG                         0x224
#define R_ICH_SATA_AHCI_P3SIG                         0x2A4
#define R_ICH_SATA_AHCI_P4SIG                         0x324
#define R_ICH_SATA_AHCI_P5SIG                         0x3A4
#define   B_ICH_SATA_AHCI_PXSIG_LBA_HR                0xFF000000
#define   B_ICH_SATA_AHCI_PXSIG_LBA_MR                0x00FF0000
#define   B_ICH_SATA_AHCI_PXSIG_LBA_LR                0x0000FF00
#define   B_ICH_SATA_AHCI_PXSIG_SCR                   0x000000FF
#define R_ICH_SATA_AHCI_P0SSTS                        0x128
#define R_ICH_SATA_AHCI_P1SSTS                        0x1A8
#define R_ICH_SATA_AHCI_P2SSTS                        0x228
#define R_ICH_SATA_AHCI_P3SSTS                        0x2A8
#define R_ICH_SATA_AHCI_P4SSTS                        0x328
#define R_ICH_SATA_AHCI_P5SSTS                        0x3A8
#define   B_ICH_SATA_AHCI_PXSSTS_IPM_0                  0x00000000
#define   B_ICH_SATA_AHCI_PXSSTS_IPM_1                  0x00000100
#define   B_ICH_SATA_AHCI_PXSSTS_IPM_2                  0x00000200
#define   B_ICH_SATA_AHCI_PXSSTS_IPM_6                  0x00000600
#define   B_ICH_SATA_AHCI_PXSSTS_SPD_0                  0x00000000
#define   B_ICH_SATA_AHCI_PXSSTS_SPD_1                  0x00000010
#define   B_ICH_SATA_AHCI_PXSSTS_SPD_2                  0x00000020
#define   B_ICH_SATA_AHCI_PXSSTS_DET_0                  0x00000000
#define   B_ICH_SATA_AHCI_PXSSTS_DET_1                  0x00000001
#define   B_ICH_SATA_AHCI_PXSSTS_DET_3                  0x00000003
#define   B_ICH_SATA_AHCI_PXSSTS_DET_4                  0x00000004
#define R_ICH_SATA_AHCI_P0SCTL                        0x12C
#define R_ICH_SATA_AHCI_P1SCTL                        0x1AC
#define R_ICH_SATA_AHCI_P2SCTL                        0x22C
#define R_ICH_SATA_AHCI_P3SCTL                        0x2AC
#define R_ICH_SATA_AHCI_P4SCTL                        0x32C
#define R_ICH_SATA_AHCI_P5SCTL                        0x3AC
#define   B_ICH_SATA_AHCI_PXSCTL_IPM                    0x00000F00
#define     V_ICH_SATA_AHCI_PXSCTL_IPM_0                  0x00000000
#define     V_ICH_SATA_AHCI_PXSCTL_IPM_1                  0x00000100
#define     V_ICH_SATA_AHCI_PXSCTL_IPM_2                  0x00000200
#define     V_ICH_SATA_AHCI_PXSCTL_IPM_3                  0x00000300
#define   B_ICH_SATA_AHCI_PXSCTL_SPD                    0x000000F0
#define     V_ICH_SATA_AHCI_PXSCTL_SPD_0                  0x00000000
#define     V_ICH_SATA_AHCI_PXSCTL_SPD_1                  0x00000010
#define     V_ICH_SATA_AHCI_PXSCTL_SPD_2                  0x00000020
#define   B_ICH_SATA_AHCI_PXSCTL_DET                    0x0000000F
#define     V_ICH_SATA_AHCI_PXSCTL_DET_0                  0x00000000
#define     V_ICH_SATA_AHCI_PXSCTL_DET_1                  0x00000001
#define     V_ICH_SATA_AHCI_PXSCTL_DET_4                  0x00000004
#define R_ICH_SATA_AHCI_P0SERR                        0x130
#define R_ICH_SATA_AHCI_P1SERR                        0x1B0
#define R_ICH_SATA_AHCI_P2SERR                        0x230
#define R_ICH_SATA_AHCI_P3SERR                        0x2B0
#define R_ICH_SATA_AHCI_P4SERR                        0x330
#define R_ICH_SATA_AHCI_P5SERR                        0x3B0
#define   B_ICH_SATA_AHCI_PXSERR_EXCHG                  BIT26
#define   B_ICH_SATA_AHCI_PXSERR_UN_FIS_TYPE            BIT25
#define   B_ICH_SATA_AHCI_PXSERR_TRSTE_24               BIT24
#define   B_ICH_SATA_AHCI_PXSERR_TRSTE_23               BIT23
#define   B_ICH_SATA_AHCI_PXSERR_HANDSHAKE              BIT22
#define   B_ICH_SATA_AHCI_PXSERR_CRC_ERROR              BIT21
#define   B_ICH_SATA_AHCI_PXSERR_10B_8B_DECERR          BIT19
#define   B_ICH_SATA_AHCI_PXSERR_COMM_WAKE              BIT18
#define   B_ICH_SATA_AHCI_PXSERR_PHY_ERROR              BIT17
#define   B_ICH_SATA_AHCI_PXSERR_PHY_RDY_CHG            BIT16
#define   B_ICH_SATA_AHCI_PXSERR_INTRNAL_ERROR          BIT11
#define   B_ICH_SATA_AHCI_PXSERR_PROTOCOL_ERROR         BIT10
#define   B_ICH_SATA_AHCI_PXSERR_PCDIE                  BIT9
#define   B_ICH_SATA_AHCI_PXSERR_TDIE                   BIT8
#define   B_ICH_SATA_AHCI_PXSERR_RCE                    BIT1
#define   B_ICH_SATA_AHCI_PXSERR_RDIE                   BIT0
#define R_ICH_SATA_AHCI_P0SACT                        0x134
#define R_ICH_SATA_AHCI_P1SACT                        0x1B4
#define R_ICH_SATA_AHCI_P2SACT                        0x234
#define R_ICH_SATA_AHCI_P3SACT                        0x2B4
#define R_ICH_SATA_AHCI_P4SACT                        0x334
#define R_ICH_SATA_AHCI_P5SACT                        0x3B4
#define   B_ICH_SATA_AHCI_PXSACT_DS                     0xFFFFFFFF
#define R_ICH_SATA_AHCI_P0CI                          0x138
#define R_ICH_SATA_AHCI_P1CI                          0x1B8
#define R_ICH_SATA_AHCI_P2CI                          0x238
#define R_ICH_SATA_AHCI_P3CI                          0x2B8
#define R_ICH_SATA_AHCI_P4CI                          0x338
#define R_ICH_SATA_AHCI_P5CI                          0x3B8
#define   B_ICH_SATA_AHCI_PXCI                          0xFFFFFFFF

//
// Macros of ICH capabilities for SATA controller which are used by SATA controller driver
//
//
//
// Define the individual capabilities of each sata controller
//
#define  ICH_SATA_MAX_CONTROLLERS                      2 // max sata controllers number supported
#define  ICH_SATA_MAX_CHANNELS                         2 // max channels number of single sata controller
#define  ICH_SATA_MAX_DEVICES                          2 // max devices number of single sata channel
#define  ICH_SATA_MAX_PORTS                            32 // number of sata ports in ICH

#define  ICH_SATA_DEVICE_ID_INVALID                    0xFFFF

#define  ICH_SATA_1_DEVICE_NUMBER                      PCI_DEVICE_NUMBER_ICH_SIDE
#define  ICH_SATA_1_FUNCTION_NUMBER                    PCI_FUNCTION_NUMBER_ICH_SIDE
#ifdef   ICH9_MOBILE
#define  ICH_SATA_1_MAX_PORTS                          4 // max number of ports in sata1 in ICH9M
#else
#define  ICH_SATA_1_MAX_PORTS                          6 // max number of ports in sata1 in ICH9
#endif
#define  ICH_SATA_1_MAX_CHANNELS                       2
#define  ICH_SATA_1_MAX_DEVICES                        2

#define  ICH_SATA_2_DEVICE_NUMBER                      PCI_DEVICE_NUMBER_ICH_SIDE2
#define  ICH_SATA_2_FUNCTION_NUMBER                    PCI_FUNCTION_NUMBER_ICH_SIDE2
#define  ICH_SATA_2_MAX_PORTS                          2 // number of ports in sata2 in ICH
#define  ICH_SATA_2_MAX_CHANNELS                       2
#define  ICH_SATA_2_MAX_DEVICES                        2

//
//  Thermal Device Registers (D31:F6)
//
#define PCI_DEVICE_NUMBER_ICH_THERMAL                  31
#define PCI_FUNCTION_NUMBER_ICH_THERMAL                6
#define R_ICH_THERMAL_VENDOR_ID                        0x00
#define   V_ICH_THERMAL_VENDOR_ID                        V_INTEL_VENDOR_ID
#define R_ICH_THERMAL_DEVICE_ID                        0x02
#define   V_ICH9_THERMAL_DEVICE_ID_0                     0x2932

#define   V_ICH10_THERMAL_DEVICE_ID_0                    0x3A32 // ESS_OVERRIDE
#define R_ICH_THERMAL_TBAR                             0x10
#define R_ICH_THERMAL_TBARH                            0x14
#define   V_ICH_THERMAL_TBARB_SIZE                       (1 << 12)
#define R_ICH_THERMAL_TBARB                            0x40
#define   B_ICH_THERMAL_SPTYPEN                          BIT0

#define R_ICH_TBARB_TS0E                               (0x01)
#define R_ICH_TBARB_TS0TTP                             (0x04)
#define R_ICH_TBARB_TS0CO                              (0x08)
#define R_ICH_TBARB_TS0PC                              (0x0E)
#define R_ICH_TBARB_TS0LOCK                            (0x83)
#define R_ICH_TBARB_TS1E                               (0x41)
#define R_ICH_TBARB_TS1TTP                             (0x44)
#define R_ICH_TBARB_TS1CO                              (0x48)
#define R_ICH_TBARB_TS1PC                              (0x4E)
#define R_ICH_TBARB_TS1LOCK                            (0xC3)

//
// SMBus Controller Registers (D31:F3)
//
#define PCI_DEVICE_NUMBER_ICH_SMBUS                    31
#define PCI_FUNCTION_NUMBER_ICH_SMBUS                  3
#define   V_ICH_SMBUS_SVID                             V_INTEL_VENDOR_ID
#define   V_ICH_SMBUS_SID                              V_ICH_DEFAULT_SID

#define R_ICH_SMBUS_VID                               0x00
#define   V_ICH_SMBUS_VID                             0x8086
#define R_ICH_SMBUS_DID                               0x02
#define R_ICH_SMBUS_VENDOR_ID                         0x00
#define   V_ICH_SMBUS_VENDOR_ID                       0x8086
#define R_ICH_SMBUS_DEVICE_ID                         0x02
#define   V_ICH9_SMBUS_DEVICE_ID                      0x2930
#define   V_ICH10_SMBUS_DEVICE_ID                     0x3A30  // ESS_OVERRIDE
#define R_ICH_SMBUS_PCICMD                            0x04
#define   B_ICH_SMBUS_PCICMD_INTR_DIS                 BIT10
#define   B_ICH_SMBUS_PCICMD_FBE                      BIT9
#define   B_ICH_SMBUS_PCICMD_SERR_EN                  BIT8
#define   B_ICH_SMBUS_PCICMD_WCC                      BIT7
#define   B_ICH_SMBUS_PCICMD_PER                      BIT6
#define   B_ICH_SMBUS_PCICMD_VPS                      BIT5
#define   B_ICH_SMBUS_PCICMD_PMWE                     BIT4
#define   B_ICH_SMBUS_PCICMD_SCE                      BIT3
#define   B_ICH_SMBUS_PCICMD_BME                      BIT2
#define   B_ICH_SMBUS_PCICMD_MSE                      BIT1
#define   B_ICH_SMBUS_PCICMD_IOSE                     BIT0
#define R_ICH_SMBUS_PCISTS                          0x06
#define   B_ICH_SMBUS_PCISTS_DPE                      BIT15
#define   B_ICH_SMBUS_PCISTS_SSE                      BIT14
#define   B_ICH_SMBUS_PCISTS_RMA                      BIT13
#define   B_ICH_SMBUS_PCISTS_RTA                      BIT12
#define   B_ICH_SMBUS_PCISTS_STA                      BIT11
#define   B_ICH_SMBUS_PCISTS_DEVT                     (BIT10 | BIT9)
#define   B_ICH_SMBUS_PCISTS_DPED                     BIT8
#define   B_ICH_SMBUS_PCISTS_FB2BC                    BIT7
#define   B_ICH_SMBUS_PCISTS_UDF                      BIT6
#define   B_ICH_SMBUS_PCISTS_66MHZ_CAP                BIT5
#define   B_ICH_SMBUS_PCISTS_CAP_LIST                 BIT4
#define   B_ICH_SMBUS_PCISTS_INTS                     BIT3
#define R_ICH_SMBUS_RID                             0x08
#define   B_ICH_SMBUS_RID                             0xFF
#define R_ICH_SMBUS_SCC                             0x0A
#define   V_ICH_SMBUS_SCC                             0x05
#define R_ICH_SMBUS_BCC                             0x0B
#define   V_ICH_SMBUS_BCC                             0x0C
#define R_ICH_SMBUS_BAR0                            0x10
#define   B_ICH_SMBUS_BAR0_BA                         0xFFFFFF00
#define   B_ICH_SMBUS_BAR0_PREF                       BIT3
#define   B_ICH_SMBUS_BAR0_ADDRNG                     (BIT2 | BIT1)
#define   B_ICH_SMBUS_BAR0_MSI                        BIT0
#define R_ICH_SMBUS_BAR1                            0x14
#define   B_ICH_SMBUS_BAR1_BA                         0xFFFFFFFF
#define R_ICH_SMBUS_BASE                            0x20
#define   V_ICH_SMBUS_BASE_SIZE                       (1 << 5)
#define R_ICH_SMBUS_SVID                            0x2C
#define   B_ICH_SMBUS_SVID                            0xFFFF
#define R_ICH_SMBUS_SID                             0x2E
#define   B_ICH_SMBUS_SID                             0xFFFF
#define R_ICH_SMBUS_INT_LN                          0x3C
#define   B_ICH_SMBUS_INT_LN                          0xFF
#define R_ICH_SMBUS_INT_PN                          0x3D
#define   B_ICH_SMBUS_INT_PN                          0xFF
#define R_ICH_SMBUS_HOSTC                           0x40
#define   B_ICH_SMBUS_HOSTC_SSRESET                   BIT3
#define   B_ICH_SMBUS_HOSTC_I2C_EN                    BIT2
#define   B_ICH_SMBUS_HOSTC_SMI_EN                    BIT1
#define   B_ICH_SMBUS_HOSTC_HST_EN                    BIT0

//
// SMBus I/O Registers
//
#define SMBUS_R_HSTS                                0x00    // Host Status Register R/W
#define   SMBUS_B_HBSY                                0x01
#define   SMBUS_B_INTR                                0x02
#define   SMBUS_B_DERR                                0x04
#define   SMBUS_B_BERR                                0x08
#define   SMBUS_B_FAIL                                0x10
#define   SMBUS_B_SMBALERT_STS                        BIT5
#define   SMBUS_B_IUS                                 0x40
#define   SMBUS_B_BYTE_DONE_STS                       BIT7
#define   SMBUS_B_HSTS_ALL                            0xFF
#define SMBUS_R_HCTL                                0x02    // Host Control Register R/W
#define   SMBUS_B_INTREN                              0x01
#define   SMBUS_B_KILL                                0x02
#define   SMBUS_B_SMB_CMD                             0x1C
#define   SMBUS_V_SMB_CMD_QUICK                       0x00
#define   SMBUS_V_SMB_CMD_BYTE                        0x04
#define   SMBUS_V_SMB_CMD_BYTE_DATA                   0x08
#define   SMBUS_V_SMB_CMD_WORD_DATA                   0x0C
#define   SMBUS_V_SMB_CMD_PROCESS_CALL                0x10
#define   SMBUS_V_SMB_CMD_BLOCK                       0x14
#define   SMBUS_V_SMB_CMD_IIC_READ                    0x18
#define   SMBUS_V_SMB_CMD_BLOCK_PROCESS               0x1C  // ESS_OVERRIDE
#define   SMBUS_B_LAST_BYTE                           BIT5
#define   SMBUS_B_START                               0x40
#define   SMBUS_B_PEC_EN                              0x80
#define SMBUS_R_HCMD                                0x03    // Host Command Register R/W
#define SMBUS_R_TSA                                 0x04    // Transmit Slave Address Register R/W
#define   SMBUS_B_RW_SEL                              0x01
#define   SMBUS_B_ADDRESS                             0xFE
#define SMBUS_R_HD0                                 0x05    // Data 0 Register R/W
#define SMBUS_R_HD1                                 0x06    // Data 1 Register R/W
#define SMBUS_R_HBD                                 0x07    // Host Block Data Register R/W
#define SMBUS_R_PEC                                 0x08    // Packet Error Check Data Register R/W
#define SMBUS_R_RSA                                 0x09    // Receive Slave Address Register R/W
#define   SMBUS_B_SLAVE_ADDR                          0x7F
#define SMBUS_R_SD                                  0x0A    // Receive Slave Data Register R/W
#define SMBUS_R_AUXS                                0x0C    // Auxiliary Status Register R/WC
#define   SMBUS_B_CRCE                                0x01
#define SMBUS_R_AUXC                                0x0D    // Auxiliary Control Register R/W
#define   SMBUS_B_AAC                                 0x01
#define   SMBUS_B_E32B                                0x02
#define SMBUS_R_SMLC                                0x0E    // SMLINK Pin Control Register R/W
#define   SMBUS_B_SMLINK0_CUR_STS                     0x01
#define   SMBUS_B_SMLINK1_CUR_STS                     0x02
#define   SMBUS_B_SMLINK_CLK_CTL                      0x04
#define SMBUS_R_SMBC                                0x0F    // SMBus Pin Control Register R/W
#define   SMBUS_B_SMBCLK_CUR_STS                      0x01
#define   SMBUS_B_SMBDATA_CUR_STS                     0x02
#define   SMBUS_B_SMBCLK_CTL                          0x04
#define SMBUS_R_SSTS                                0x10    // Slave Status Register R/WC
#define   SMBUS_B_HOST_NOTIFY_STS                     0x01
#define SMBUS_R_SCMD                                0x11    // Slave Command Register R/W
#define   SMBUS_B_HOST_NOTIFY_INTREN                  0x01
#define   SMBUS_B_HOST_NOTIFY_WKEN                    0x02
#define   SMBUS_B_SMBALERT_DIS                        0x04
#define SMBUS_R_NDA                                 0x14    // Notify Device Address Register RO
#define   SMBUS_B_DEVICE_ADDRESS                      0xFE
#define SMBUS_R_NDLB                                0x16    // Notify Data Low Byte Register RO
#define SMBUS_R_NDHB                                0x17    // Notify Data High Byte Register RO


//
// USB Definitions
//

#define PCI_DEVICE_NUMBER_ICH_USB                      29
#define PCI_FUNCTION_NUMBER_ICH_UHCI1                  0
#define PCI_FUNCTION_NUMBER_ICH_UHCI2                  1
#define PCI_FUNCTION_NUMBER_ICH_UHCI3                  2
#define PCI_FUNCTION_NUMBER_ICH_UHCI6_REMAPPED         3
#define PCI_FUNCTION_NUMBER_ICH_EHCI                   7

#define PCI_DEVICE_NUMBER_ICH_USB_EXT                  26
#define PCI_FUNCTION_NUMBER_ICH_UHCI4                  0
#define PCI_FUNCTION_NUMBER_ICH_UHCI5                  1
#define PCI_FUNCTION_NUMBER_ICH_UHCI6                  2
#define PCI_FUNCTION_NUMBER_ICH_EHCI2                  7

#define R_ICH_USB_VENDOR_ID                         0x00
#define   V_ICH_USB_VENDOR_ID                         V_INTEL_VENDOR_ID
#define R_ICH_USB_DEVICE_ID                         0x02
#define   V_ICH9_USB_DEVICE_ID_0                       0x2934 // UHCI#1
#define   V_ICH9_USB_DEVICE_ID_1                       0x2935 // UHCI#2
#define   V_ICH9_USB_DEVICE_ID_2                       0x2936 // UHCI#3
#define   V_ICH9_USB_DEVICE_ID_3                       0x2937 // UHCI#4
#define   V_ICH9_USB_DEVICE_ID_4                       0x2938 // UHCI#5
#define   V_ICH9_USB_DEVICE_ID_5                       0x2939 // UHCI#6
#define   V_ICH9_USB_DEVICE_ID_6                       0x293A // EHCI#1
#define   V_ICH9_USB_DEVICE_ID_7                       0x293C // EHCI#2

#define   V_ICH10_USB_DEVICE_ID_0                      0x3A34 // UHCI#1 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_1                      0x3A35 // UHCI#2 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_2                      0x3A36 // UHCI#3 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_3                      0x3A37 // UHCI#4 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_4                      0x3A38 // UHCI#5 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_5                      0x3A39 // UHCI#6 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_6                      0x3A3A // EHCI#1 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_6_1                    0x3A3B // EHCI#1 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_7                      0x3A3C // EHCI#2 ESS_OVERRIDE
#define   V_ICH10_USB_DEVICE_ID_7_1                    0x3A3D // EHCI#2 ESS_OVERRIDE

#define R_ICH_UHCI_COMMAND_REGISTER                 0x04
#define   B_ICH_UHCI_COMMAND_INTR_DIS                 BIT10
#define   B_ICH_UHCI_COMMAND_FBE                      BIT9
#define   B_ICH_UHCI_COMMAND_SERR_EN                  BIT8
#define   B_ICH_UHCI_COMMAND_WCC                      BIT7
#define   B_ICH_UHCI_COMMAND_PER                      BIT6
#define   B_ICH_UHCI_COMMAND_VPS                      BIT5
#define   B_ICH_UHCI_COMMAND_PMWE                     BIT4
#define   B_ICH_UHCI_COMMAND_SCE                      BIT3
#define   B_ICH_UHCI_COMMAND_BME                      BIT2
#define   B_ICH_UHCI_COMMAND_MSE                      BIT1
#define   B_ICH_UHCI_COMMAND_IOSE                     BIT0
#define   B_ICH_EHCI_COMMAND_INTR_DIS                 BIT10
#define   B_ICH_EHCI_COMMAND_SERR_EN                  BIT8
#define   B_ICH_EHCI_COMMAND_BME                      BIT2
#define   B_ICH_EHCI_COMMAND_MSE                      BIT1
#define   B_ICH_EHCI_COMMAND_IOSE                     BIT0
#define R_ICH_UHCI_PCISTS                           0x06
#define   B_ICH_UHCI_PCISTS_DPE                       BIT15
#define   B_ICH_UHCI_PCISTS_RMA                       BIT13
#define   B_ICH_UHCI_PCISTS_STA                       BIT11
#define   B_ICH_UHCI_PCISTS_DEV_STS                   (BIT10 | BIT9)
#define   B_ICH_UHCI_PCISTS_DPED                      BIT8
#define   B_ICH_UHCI_PCISTS_FB2BC                     BIT7
#define   B_ICH_UHCI_PCISTS_UDF                       BIT6
#define   B_ICH_UHCI_PCISTS_66MHZ_CAP                 BIT5
#define   B_ICH_UHCI_PCISTS_CAP_LST                   BIT4
#define   B_ICH_UHCI_PCISTS_INTR_STS                  BIT3
#define R_ICH_UHCI_RID                              0x08
#define   B_ICH_UHCI_RID                              0xFF
#define R_ICH_UHCI_PI                               0x09
#define   B_ICH_UHCI_PI                               0xFF
#define R_ICH_UHCI_SCC                              0x0A
#define   B_ICH_UHCI_SCC                              0xFF
#define R_ICH_UHCI_BCC                              0x0B
#define   B_ICH_UHCI_BCC                              0xFF
#define R_ICH_UHCI_MLT                              0x0D
#define   B_ICH_UHCI_MLT                              0xFF
#define R_ICH_UHCI_HEADTYPE                         0x0E
#define   B_ICH_UHCI_HEADTYPE                         BIT7
#define R_ICH_UHCI_BASE_ADDRESS_REGISTER            0x20
#define   B_ICH_UHCI_BAR_BA                           0xFFFFFFE0
#define   B_ICH_UHCI_BAR_RTE                          BIT0
#define R_ICH_UHCI_SVID                             0x2C
#define   B_ICH_UHCI_SVID                             0xFFFF
#define R_ICH_UHCI_SID                              0x2E
#define   B_ICH_UHCI_SID                              0xFFFF
#define R_ICH_UHCI_CAP_PTR                          0x34
#define   B_ICH_UHCI_CAP_PTR                          0xFF
#define R_ICH_UHCI_INT_LN                           0x3C
#define   B_ICH_UHCI_INT_LN                           0xFF
#define R_ICH_UHCI_INT_PN                           0x3D
#define   B_ICH_UHCI_INT_PN                           0xFF
#define R_ICH_UHCI_FLRCID                           0x50
#define   B_ICH_UHCI_FLRCID                           0xFF
#define     V_ICH_UHCI_FLRCID_13                        0x13
#define     V_ICH_UHCI_FLRCID_09                        0x09
#define R_ICH_UHCI_FLRNCP                           0x51
#define   B_ICH_UHCI_FLRNCP                           0xFF
#define R_ICH_UHCI_FLRCLV                           0x52
#define   B_ICH_UHCI_FLRCLV_FLR_CAP_SSEL0             BIT9
#define   B_ICH_UHCI_FLRCLV_TXP_CAP_SSEL0             BIT8
#define   B_ICH_UHCI_FLRCLV_VS_CAP_SSEL1              0xF000
#define   B_ICH_UHCI_FLRCLV_CAP_VER_SSEL1             0x0F00
#define   B_ICH_UHCI_FLRCLV_CAP_LNG                   0xFF
#define R_ICH_UHCI_FLRCR                            0x54
#define   B_ICH_UHCI_FLRCR_INIT_FLR                   BIT0
#define R_ICH_UHCI_FLRSR                            0x55
#define   B_ICH_UHCI_FLRSR_TXP                        BIT0
#define R_ICH_UHCI_USBRNR                           0x60
#define   B_ICH_UHCI_USBRNR                           0xFF
#define R_ICH_UHCI_LEGACY_CONTROL_REGISTER          0xC0
#define   B_ICH_UHCI_LEGKEY_SMIBYENDPS                BIT15
#define   B_ICH_UHCI_LEGKEY_PIRQEN                    BIT13
#define   B_ICH_UHCI_LEGKEY_SMIBYUSB                  BIT12
#define   B_ICH_UHCI_LEGKEY_TRAPBY64W                 BIT11
#define   B_ICH_UHCI_LEGKEY_TRAPBY64R                 BIT10
#define   B_ICH_UHCI_LEGKEY_TRAPBY60W                 BIT9
#define   B_ICH_UHCI_LEGKEY_TRAPBY60R                 BIT8
#define   B_ICH_UHCI_LEGKEY_SMIATENDPS                BIT7
#define   B_ICH_UHCI_LEGKEY_PSTATE                    BIT6
#define   B_ICH_UHCI_LEGKEY_A20PASSEN                 BIT5
#define   B_ICH_UHCI_LEGKEY_USBSMIEN                  BIT4
#define   B_ICH_UHCI_LEGKEY_64WEN                     BIT3
#define   B_ICH_UHCI_LEGKEY_64REN                     BIT2
#define   B_ICH_UHCI_LEGKEY_60WEN                     BIT1
#define   B_ICH_UHCI_LEGKEY_60REN                     BIT0
#define R_ICH_UHCI_RESUME_ENABLE_REGISTER           0xC4
#define   B_ICH_UHCI_RES_PORT1EN                      BIT1
#define   B_ICH_UHCI_RES_PORT0EN                      BIT0
#define R_ICH_UHCI_CWP                              0xC8
#define   B_ICH_UHCI_CWP_HCAE                         BIT2
#define   B_ICH_UHCI_CWP_HBM                          BIT1
#define   B_ICH_UHCI_CWP_SBMSPE                       BIT0
#define R_ICH_UHCI_UCR1                             0xCA
#define   B_ICH_UHCI_UCR1_ITAD                        BIT0

#define R_ICH_EHCI_MEM_BASE                         0x10
#define   V_ICH_EHCI_MEM_LENGTH                       0x400
#define   N_ICH_EHCI_MEM_ALIGN                         10
#define R_ICH_EHCI_FLRCID                           0x50
#define   B_ICH_EHCI_FLRCID                           0xFF
#define R_ICH_EHCI_NXT_PTR1                         0x51
#define   B_ICH_EHCI_NXT_PTR1                         0xFF
#define R_ICH_EHCI_PWR_CAP                          0x52
#define   B_ICH_EHCI_PWR_CAP_PME_SUP                  0xF800
#define   B_ICH_EHCI_PWR_CAP_D2_SUP                   BIT10
#define   B_ICH_EHCI_PWR_CAP_D1_SUP                   BIT9
#define   B_ICH_EHCI_PWR_CAP_AUX_CUR                  (BIT8 | BIT7 | BIT6)
#define   B_ICH_EHCI_PWR_CAP_DSI                      BIT5
#define   B_ICH_EHCI_PWR_CAP_PME_CLK                  BIT3
#define   B_ICH_EHCI_PWR_CAP_VER                      (BIT2 | BIT1 | BIT0)
#define R_ICH_EHCI_PWR_CNTL_STS                     0x54
#define   B_ICH_EHCI_PWR_CNTL_STS_PME_STS             BIT15
#define   B_ICH_EHCI_PWR_CNTL_STS_DATASCL             (BIT14 | BIT13)
#define   B_ICH_EHCI_PWR_CNTL_STS_DATASEL             0x1E00
#define   B_ICH_EHCI_PWR_CNTL_STS_PME_EN              BIT8
#define   B_ICH_EHCI_PWR_CNTL_STS_PWR_STS             (BIT1 | BIT0)
#define R_ICH_EHCI_DBG_CAPID                        0x58
#define   B_ICH_EHCI_DBG_CAPID                        0xFF
#define R_ICH_EHCI_NXT_PTR2                         0x59
#define   B_ICH_EHCI_NXT_PTR2                         0xFF
#define R_ICH_EHCI_DBG_BASE                         0x5A
#define   B_ICH_EHCI_DBG_BASE_BAR_NUM                  0xE000
#define   B_ICH_EHCI_DBG_BASE_PORT_OFFSET              0x1FFF
#define R_ICH_EHCI_USBRNR                           0x60
#define   B_ICH_EHCI_USBRNR                           0xFF
#define R_ICH_EHCI_FL_ADJ                           0x61
#define   B_ICH_EHCI_FL_ADJ                           0x3F
#define R_ICH_EHCI_PWAKE_CAP                        0x62
#define   B_ICH_EHCI_PWAKE_CAP_D29_MASK               0x007E
#define   B_ICH_EHCI_PWAKE_CAP_D26_MASK               0x001E
#define   B_ICH_EHCI_PWAKE_CAP_PWK_IMP                BIT0
#define R_ICH_EHCI_LEGEXT_CAP                       0x68
#define   B_ICH_EHCI_LEGEXT_CAP_HCOS                  BIT24
#define   B_ICH_EHCI_LEGEXT_CAP_HCBIOS                BIT16
#define   B_ICH_EHCI_LEGEXT_CAP_NEXT                  0x0000FF00
#define   B_ICH_EHCI_LEGEXT_CAP_CAPID                 0x000000FF
#define R_ICH_EHCI_LEGEXT_CS                        0x6C
#define   B_ICH_EHCI_LEGEXT_CS_SMIBAR                 BIT31
#define   B_ICH_EHCI_LEGEXT_CS_SMIPCI                 BIT30
#define   B_ICH_EHCI_LEGEXT_CS_SMIOS                  BIT29
#define   B_ICH_EHCI_LEGEXT_CS_SMIAA                  BIT21
#define   B_ICH_EHCI_LEGEXT_CS_SMIHSE                 BIT20
#define   B_ICH_EHCI_LEGEXT_CS_SMIFLR                 BIT19
#define   B_ICH_EHCI_LEGEXT_CS_SMIPCD                 BIT18
#define   B_ICH_EHCI_LEGEXT_CS_SMIERR                 BIT17
#define   B_ICH_EHCI_LEGEXT_CS_SMICOMP                BIT16
#define   B_ICH_EHCI_LEGEXT_CS_SMIBAR_EN              BIT15
#define   B_ICH_EHCI_LEGEXT_CS_SMIPCI_EN              BIT14
#define   B_ICH_EHCI_LEGEXT_CS_SMIOS_EN               BIT13
#define   B_ICH_EHCI_LEGEXT_CS_SMIAA_EN               BIT5
#define   B_ICH_EHCI_LEGEXT_CS_SMIHSE_EN              BIT4
#define   B_ICH_EHCI_LEGEXT_CS_SMIFLR_EN              BIT3
#define   B_ICH_EHCI_LEGEXT_CS_SMIPCD_EN              BIT2
#define   B_ICH_EHCI_LEGEXT_CS_SMIERR_EN              BIT1
#define   B_ICH_EHCI_LEGEXT_CS_SMICOMP_EN             BIT0
#define R_ICH_EHCI_SPCSMI                           0x70
#define   B_ICH_EHCI_SPCSMI_D29_PO                    0x0FC00000
#define   B_ICH_EHCI_SPCSMI_D26_PO                    0x03C00000
#define   B_ICH_EHCI_SPCSMI_PMCSR                     BIT21
#define   B_ICH_EHCI_SPCSMI_ASYNC                     BIT20
#define   B_ICH_EHCI_SPCSMI_PERIODIC                  BIT19
#define   B_ICH_EHCI_SPCSMI_CF                        BIT18
#define   B_ICH_EHCI_SPCSMI_HCHALT                    BIT17
#define   B_ICH_EHCI_SPCSMI_HCRESET                   BIT16
#define   B_ICH_EHCI_SPCSMI_PO_EN                     0x00003FC0
#define   B_ICH_EHCI_SPCSMI_PMCSR_EN                  BIT5
#define   B_ICH_EHCI_SPCSMI_ASYNC_EN                  BIT4
#define   B_ICH_EHCI_SPCSMI_PERIODIC_EN               BIT3
#define   B_ICH_EHCI_SPCSMI_CF_EN                     BIT2
#define   B_ICH_EHCI_SPCSMI_HCHALT_EN                 BIT1
#define   B_ICH_EHCI_SPCSMI_HCRESET_EN                BIT0
#define R_ICH_EHCI_ACCESS_CNTL                      0x80
#define   B_ICH_EHCI_ACCESS_CNTL_ENABLE                BIT0
#define     V_ICH_EHCI_ACCESS_CNTL_ENABLE                0x01
#define     V_ICH_EHCI_ACCESS_CNTL_DISABLE               0x00
#define R_ICH_EHCI_EHCIIR1                          0x84
#define   B_ICH_EHCI_EHCIIR1                          BIT4
#define R_ICH_EHCI_FLR_CID                          0x98
#define   B_ICH_EHCI_FLR_CID                          0xFF
#define R_ICH_EHCI_FLR_NEXT                         0x99
#define   B_ICH_EHCI_FLR_NEXT                         0xFF
#define R_ICH_EHCI_FLR_CLV                          0x9A
#define   B_ICH_EHCI_FLR_CLV_CAP_SSEL0                BIT9
#define   B_ICH_EHCI_FLR_CLV_TXP_SSEL0                BIT8
#define   B_ICH_EHCI_FLR_CLV_VSCID_SSEL1              0xF000
#define   B_ICH_EHCI_FLR_CLV_CAPVER_SSEL1             0x0F00
#define   B_ICH_EHCI_FLR_CLV_LNG                      0x00FF
#define R_ICH_EHCI_FLR_CTRL                         0x9C
#define   B_ICH_EHCI_FLR_CTRL_INITFLR                 BIT0
#define R_ICH_EHCI_FLR_STS                          0x9D
#define   B_ICH_EHCI_FLR_STS_TXP                      BIT0
#define R_ICH_EHCI_EHCIIR1_FLD                      0xFC
#define   B_ICH_EHCI_EHCIIR1_FIELD_2                  BIT29
#define   B_ICH_EHCI_EHCIIR1_FIELD_1                  BIT17

#define R_ICH_HCSPARAMS                             0x04
#define   N_ICH_HCSPARAMS_DP_N                         20
#define   N_ICH_HCSPARAMS_N_CC                         12
#define   N_ICH_HCSPARAMS_N_PCC                        8
#define   N_ICH_HCSPARAMS_N_PORTS                      0

#define MAX_UHCI_CONTROLLERS                        6

typedef enum {
  Uhci1 = 0,
  Uhci2,
  Uhci3,
  Uhci4,
  Uhci5,
  Uhci6,
  Ehci1,
  Ehci2,
  NUMBER_USB_CONTROLLERS
} USB_CONTROLLER_TYPE;

#define ICH_UHCI1_BUS_DEV_FUNC          ((UINT64) ((((UINTN) PCI_DEVICE_NUMBER_ICH_USB) << 16) + (((UINTN) PCI_FUNCTION_NUMBER_ICH_UHCI1) << 8)))
#define ICH_UHCI2_BUS_DEV_FUNC          ((UINT64) ((((UINTN) PCI_DEVICE_NUMBER_ICH_USB) << 16) + (((UINTN) PCI_FUNCTION_NUMBER_ICH_UHCI2) << 8)))
#define ICH_UHCI3_BUS_DEV_FUNC          ((UINT64) ((((UINTN) PCI_DEVICE_NUMBER_ICH_USB) << 16) + (((UINTN) PCI_FUNCTION_NUMBER_ICH_UHCI3) << 8)))
#define ICH_UHCI4_BUS_DEV_FUNC          ((UINT64) ((((UINTN) PCI_DEVICE_NUMBER_ICH_USB_EXT) << 16) + (((UINTN) PCI_FUNCTION_NUMBER_ICH_UHCI4) << 8)))
#define ICH_UHCI5_BUS_DEV_FUNC          ((UINT64) ((((UINTN) PCI_DEVICE_NUMBER_ICH_USB_EXT) << 16) + (((UINTN) PCI_FUNCTION_NUMBER_ICH_UHCI5) << 8)))
#define ICH_UHCI6_BUS_DEV_FUNC_REMAPPED ((UINT64) ((((UINTN) PCI_DEVICE_NUMBER_ICH_USB) << 16) + (((UINTN) PCI_FUNCTION_NUMBER_ICH_UHCI6_REMAPPED) << 8)))
#define ICH_UHCI6_BUS_DEV_FUNC_DEFAULT  ((UINT64) ((((UINTN) PCI_DEVICE_NUMBER_ICH_USB_EXT) << 16) + (((UINTN) PCI_FUNCTION_NUMBER_ICH_UHCI6) << 8)))

#define UHCI_PCI_ADDR( Controller ) \
  ((Controller == Uhci1) ? ICH_UHCI1_BUS_DEV_FUNC : \
   (Controller == Uhci2) ? ICH_UHCI2_BUS_DEV_FUNC : \
   (Controller == Uhci3) ? ICH_UHCI3_BUS_DEV_FUNC : \
   (Controller == Uhci4) ? ICH_UHCI4_BUS_DEV_FUNC : \
   (Controller == Uhci5) ? ICH_UHCI5_BUS_DEV_FUNC : \
   (Controller == Uhci6) ? ICH_UHCI6_BUS_DEV_FUNC_DEFAULT : EFI_UNSUPPORTED)

//
// LAN Controller Registers (D25:F0)
//
#define PCI_BUS_NUMBER_ICH_LAN                         0
#define PCI_DEVICE_NUMBER_ICH_LAN                      25
#define PCI_FUNCTION_NUMBER_ICH_LAN                    0

#define R_ICH_LAN_VENDOR_ID                         0x00
#define   V_ICH_LAN_VENDOR_ID                          V_INTEL_VENDOR_ID
#define R_ICH_LAN_DEVICE_ID                         0x02
#define   V_ICH9_LAN_DEVICE_ID_0                       0x294C
#define   V_ICH10_LAN_DEVICE_ID_0                      0x3A4C //ESS_OVERRIDE
#define R_ICH_LAN_CMD                              0x04
#define   B_ICH_LAN_CMD_INTR_DIS                     BIT10
#define   B_ICH_LAN_CMD_FBE                          BIT9
#define   B_ICH_LAN_CMD_SERR_EN                      BIT8
#define   B_ICH_LAN_CMD_WCC                          BIT7
#define   B_ICH_LAN_CMD_PER                          BIT6
#define   B_ICH_LAN_CMD_PSE                          BIT5
#define   B_ICH_LAN_CMD_PMWE                         BIT4
#define   B_ICH_LAN_CMD_SCE                          BIT3
#define   B_ICH_LAN_CMD_BME                          BIT2
#define   B_ICH_LAN_CMD_MSE                          BIT1
#define   B_ICH_LAN_CMD_IOSE                         BIT0
#define R_ICH_LAN_STS                              0x06
#define   B_ICH_LAN_STS_DPE                          BIT15
#define   B_ICH_LAN_STS_SSE                          BIT14
#define   B_ICH_LAN_STS_RMA                          BIT13
#define   B_ICH_LAN_STS_RTA                          BIT12
#define   B_ICH_LAN_STS_STA                          BIT11
#define   B_ICH_LAN_STS_DEV_STS                      (BIT10 | BIT9)
#define   B_ICH_LAN_STS_DPED                         BIT8
#define   B_ICH_LAN_STS_FB2BC                        BIT7
#define   B_ICH_LAN_STS_66MHZ_CAP                    BIT5
#define   B_ICH_LAN_STS_CAP_LST                      BIT4
#define   B_ICH_LAN_STS_INTR_STS                     BIT3
#define R_ICH_LAN_RID                              0x08
#define   B_ICH_LAN_RID                              0xFF
#define R_ICH_LAN_CC                               0x09
#define  S_ICH_LAN_CC                               3
#define   B_ICH_LAN_CC                              0xFFFFFF
#define R_ICH_LAN_CLS                              0x0C
#define   B_ICH_LAN_CLS                              0xFF
#define R_ICH_LAN_PLT                               0x0D
#define   B_ICH_LAN_PLT                               0xFF
#define R_ICH_LAN_HEADTYPE                         0x0E
#define   B_ICH_LAN_HEADTYPE                         0xFF
#define R_ICH_LAN_MEM_BASE_A                        0x10
#define   B_ICH_LAN_MBARA_BA                         0xFFFF8000
#define   B_ICH_LAN_MBARA_MSIZE                      0x00007FF0
#define   B_ICH_LAN_MBARA_PM                         BIT3
#define   B_ICH_LAN_MBARA_MT                         (BIT2 | BIT1)
#define   B_ICH_LAN_MBARA_MIOS                       BIT0
#define R_ICH_LAN_MBARB                            0x14
#define   B_ICH_LAN_MBARB_BA                         0xFFFFF000
#define   B_ICH_LAN_MBARB_MSIZE                      0x00000FF0
#define   B_ICH_LAN_MBARB_PM                         BIT3
#define   B_ICH_LAN_MBARB_MT                         (BIT2 | BIT1)
#define   B_ICH_LAN_MBARB_MIOS                       BIT0
#define R_ICH_LAN_MBARC                            0x18
#define   B_ICH_LAN_MBARC_BA                         0xFFFFFFE0
#define   B_ICH_LAN_MBARC_IOSIZE                     0x0000001E
#define   B_ICH_LAN_MBARC_MIOS                       BIT0
#define R_ICH_LAN_SVID                             0x2C
#define   B_ICH_LAN_SVID                             0xFFFF
#define R_ICH_LAN_SID                              0x2E
#define   B_ICH_LAN_SID                              0xFFFF
#define R_ICH_LAN_ERBA                             0x30
#define   B_ICH_LAN_ERBA                             0xFFFFFFFF
#define R_ICH_LAN_CAP_PTR                          0x34
#define   B_ICH_LAN_CAP_PTR                          0xFF
#define R_ICH_LAN_INTR                             0x3C
#define   B_ICH_LAN_INTR_IPIN                        0xFF00
#define   B_ICH_LAN_INTR_ILINE                       0x00FF
#define   V_ICH_LAN_MEM_LENGTH                        0x8000
#define   N_ICH_LAN_MEM_ALIGN                         15
#define R_ICH_LAN_CLIST1                           0xC8
#define   B_ICH_LAN_CLIST1_NEXT                      0xFF00
#define   B_ICH_LAN_CLIST1_CID                       0x00FF
#define R_ICH_LAN_PMC                              0xCA
#define   B_ICH_LAN_PMC_PMES                         0xF800
#define   B_ICH_LAN_PMC_D2S                          BIT10
#define   B_ICH_LAN_PMC_D1S                          BIT9
#define   B_ICH_LAN_PMC_AC                           (BIT8 | BIT7 | BIT6)
#define   B_ICH_LAN_PMC_DSI                          BIT5
#define   B_ICH_LAN_PMC_PMEC                         BIT3
#define   B_ICH_LAN_PMC_VS                           (BIT2 | BIT1 | BIT0)
#define R_ICH_LAN_PMCS                             0xCC
#define   B_ICH_LAN_PMCS_PMES                        BIT15
#define   B_ICH_LAN_PMCS_DSC                         (BIT14 | BIT13)
#define   B_ICH_LAN_PMCS_DSL                         0x1E00
#define     V_ICH_LAN_PMCS_DSL0                      0x0000
#define     V_ICH_LAN_PMCS_DSL3                      0x0600
#define     V_ICH_LAN_PMCS_DSL4                      0x0800
#define     V_ICH_LAN_PMCS_DSL7                      0x0E00
#define     V_ICH_LAN_PMCS_DSL8                      0x1000
#define   B_ICH_LAN_PMCS_PMEE                        BIT8
#define   B_ICH_LAN_PMCS_PS                          (BIT1 | BIT0)
#define     V_ICH_LAN_PMCS_PS0                         0x00
#define     V_ICH_LAN_PMCS_PS3                         0x03
#define R_ICH_LAN_DR                                0xCF
#define   B_ICH_LAN_DR                                0xFF
#define R_ICH_LAN_CLIST2                            0xD0
#define   B_ICH_LAN_CLIST2_NEXT                       0xFF00
#define   B_ICH_LAN_CLIST2_CID                        0x00FF
#define R_ICH_LAN_MCTL                              0xD2
#define   B_ICH_LAN_MCTL_CID                          BIT7
#define   B_ICH_LAN_MCTL_MME                          (BIT6 | BIT5 | BIT4)
#define   B_ICH_LAN_MCTL_MMC                          (BIT3 | BIT2 | BIT1)
#define   B_ICH_LAN_MCTL_MSIE                         BIT0
#define R_ICH_LAN_MADDL                             0xD4
#define   B_ICH_LAN_MADDL                             0xFFFFFFFF
#define R_ICH_LAN_MADDH                             0xD8
#define   B_ICH_LAN_MADDH                             0xFFFFFFFF
#define R_ICH_LAN_MDAT                              0xDC
#define   B_ICH_LAN_MDAT                              0xFFFFFFFF
#define R_ICH_LAN_FLRCAP                            0xE0
#define   B_ICH_LAN_FLRCAP_NEXT                       0xFF00
#define   B_ICH_LAN_FLRCAP_CID                        0x00FF
#define     V_ICH_LAN_FLRCAP_CID_SSEL0                  0x13
#define     V_ICH_LAN_FLRCAP_CID_SSEL1                  0x09
#define R_ICH_LAN_FLRCLV                            0xE2
#define   B_ICH_LAN_FLRCLV_FLRC_SSEL0                 BIT9
#define   B_ICH_LAN_FLRCLV_TXP_SSEL0                  BIT8
#define   B_ICH_LAN_FLRCLV_VSCID_SSEL1                0xF000
#define   B_ICH_LAN_FLRCLV_CAPVER_SSEL1               0x0F00
#define   B_ICH_LAN_FLRCLV_CAPLNG                     0x00FF
#define R_ICH_LAN_DEVCTRL                           0xE4
#define   B_ICH_LAN_DEVCTRL                           BIT0

//
// The followings are the definitions for internal LAN MBARA registers
//
#define R_ICH_LAN_MBARA_LDR4                           0x0000     // LAN Device Initialization Register 4
#define   B_ICH_LAN_MBARA_LDR4_FIELD1                    BIT24    // LDR4 Field 1
#define R_ICH_LAN_MBARA_LDR3                           0x0008     // LAN Device Initialization Register 3
#define   B_ICH_LAN_MBARA_LDR3_FIELD1                    BIT31    // LDR3 Field 1
#define R_ICH_LAN_MBARA_LDCR2                          0x0018     // LAN Device Control Register 2
#define   B_ICH_LAN_MBARA_LDCR2_LPPDE                    BIT20    // LAN PHY Power Down Enable
#define   B_ICH_LAN_MBARA_LDCR2_FIELD1                   BIT19    // LDCR2 Field 1
#define R_ICH_LAN_MBARA_LDCR4                          0x0020     // LAN Device Control Register 4
#define R_ICH_LAN_MBARA_LDR5                           0x0F00     // LAN Device Control Register 5
#define   B_ICH_LAN_MBARA_LDR5_FIELD1                    BIT5     // LDR5 Field 1
#define R_ICH_LAN_MBARA_LDR2                           0x3004     // LAN Device Initialization Register 2
#define   B_ICH_LAN_MBARA_LDR2_FIELD1_MASK               (BIT19 | BIT18 | BIT17 | BIT16)      // LDR2 Field 1
#define R_ICH_LAN_MBARA_LDR1                           0x3024     // LAN Device Initialization Register 1
#define   B_ICH_LAN_MBARA_LDR1_FIELD1                    BIT16    // LDR1 Field 1


//
// Azalia Controller Registers (D27:F0)
//
#define PCI_DEVICE_NUMBER_ICH_AZALIA                   27
#define PCI_FUNCTION_NUMBER_ICH_AZALIA                 0

#define R_ICH_HDA_VENDOR_ID                         0x00
#define   V_ICH_HDA_VENDOR_ID                         V_INTEL_VENDOR_ID
#define R_ICH_HDA_DEVICE_ID                         0x02
#define   V_ICH9_HDA_DEVICE_ID_0                      0x293E
#define   V_ICH9_HDA_DEVICE_ID_1                      0x293F
#define   V_ICH10_HDA_DEVICE_ID_0                     0x3A3E // ESS_OVERRIDE
#define   V_ICH10_HDA_DEVICE_ID_1                     0x3A3F // ESS_OVERRIDE
#define R_ICH_HDA_COMMAND                           0x04
#define   B_ICH_HDA_COMMAND_INTR_DIS                  BIT10
#define   B_ICH_HDA_COMMAND_FBE                       BIT9
#define   B_ICH_HDA_COMMAND_SERR_EN                   BIT8
#define   B_ICH_HDA_COMMAND_WCC                       BIT7
#define   B_ICH_HDA_COMMAND_PER                       BIT6
#define   B_ICH_HDA_COMMAND_VPS                       BIT5
#define   B_ICH_HDA_COMMAND_MWIE                      BIT4
#define   B_ICH_HDA_COMMAND_SCE                       BIT3
#define   B_ICH_HDA_COMMAND_BME                       BIT2
#define   B_ICH_HDA_COMMAND_MSE                       BIT1
#define   B_ICH_HDA_COMMAND_IOSE                      BIT0
#define R_ICH_HDA_STS                              0x06
#define   B_ICH_HDA_STS_DPE                          BIT15
#define   B_ICH_HDA_STS_SSE                          BIT14
#define   B_ICH_HDA_STS_RMA                          BIT13
#define   B_ICH_HDA_STS_RTA                          BIT12
#define   B_ICH_HDA_STS_STA                          BIT11
#define   B_ICH_HDA_STS_DEV_STS                      (BIT10 | BIT9)
#define   B_ICH_HDA_STS_DPED                         BIT8
#define   B_ICH_HDA_STS_FB2BC                        BIT7
#define   B_ICH_HDA_STS_66MHZ_CAP                    BIT5
#define   B_ICH_HDA_STS_CAP_LST                      BIT4
#define   B_ICH_HDA_STS_INTR_STS                     BIT3
#define R_ICH_HDA_RID                              0x08
#define   B_ICH_HDA_RID                              0xFF
#define R_ICH_HDA_PI                               0x09
#define   B_ICH_HDA_PI                               0xFF
#define R_ICH_HDA_SCC                              0x0A
#define   B_ICH_HDA_SCC                              0xFF
#define R_ICH_HDA_BCC                              0x0B
#define   B_ICH_HDA_BCC                              0xFF
#define R_ICH_HDA_CLS                              0x0C
#define   B_ICH_HDA_CLS                              0xFF
#define R_ICH_HDA_LT                               0x0D
#define   B_ICH_HDA_LT                               0xFF
#define R_ICH_HDA_HEADTYPE                         0x0E
#define   B_ICH_HDA_HEADTYPE                         0xFF
#define R_ICH_HDA_HDBARL                           0x10
#define   B_ICH_HDA_HDBARL_LBA                       0xFFFFC000
#define   B_ICH_HDA_HDBARL_PREF                      BIT3
#define   B_ICH_HDA_HDBARL_ADDRNG                    (BIT2 | BIT1)
#define   B_ICH_HDA_HDBARL_SPTYP                     BIT0
#define   V_ICH_HDA_HDBAR_SIZE                        (1 << 14)
#define R_ICH_HDA_HDBARU                            0x14
#define   B_ICH_HDA_HDBARU_UBA                        0xFFFFFFFF
#define R_ICH_HDA_SVID                              0x2C
#define   B_ICH_HDA_SVID                              0xFFFF
#define R_ICH_HDA_SID                               0x2E
#define   B_ICH_HDA_SID                               0xFFFF
#define R_ICH_HDA_CAPPTR                            0x34
#define   B_ICH_HDA_CAPPTR                            0xFF
#define R_ICH_HDA_INTLN                             0x3C
#define   B_ICH_HDA_INTLN                             0xFF
#define R_ICH_HDA_INTPN                             0x3D
#define   B_ICH_HDA_INTPN                             0x0F
#define R_ICH_HDA_HDCTL                             0x40
#define   B_ICH_HDA_HDCTL_MODE                        BIT0
#define R_ICH_HDA_TCSEL                             0x44
#define   B_ICH_HDA_TCSEL                             (BIT2 | BIT1 | BIT0)
#define     V_ICH_HDA_TCSEL_TC0                       0x00
#define     V_ICH_HDA_TCSEL_TC1                       0x01
#define     V_ICH_HDA_TCSEL_TC2                       0x02
#define     V_ICH_HDA_TCSEL_TC3                       0x03
#define     V_ICH_HDA_TCSEL_TC4                       0x04
#define     V_ICH_HDA_TCSEL_TC5                       0x05
#define     V_ICH_HDA_TCSEL_TC6                       0x06
#define     V_ICH_HDA_TCSEL_TC7                       0x07
#define R_ICH_HDA_DCKCTL                            0x4C
#define   B_ICH_HDA_DCKCTL_DA                         BIT0
#define R_ICH_HDA_DCKSTS                            0x4D
#define   B_ICH_HDA_DCKSTS_DS                         BIT7
#define   B_ICH_HDA_DCKSTS_DM                         BIT0
#define R_ICH_HDA_PID                               0x50
#define   B_ICH_HDA_PID_NEXT                          0xFF00
#define   B_ICH_HDA_PID_CAP                           0x00FF
#define R_ICH_HDA_PC                                0x52
#define   B_ICH_HDA_PC_PME                            0xF800
#define   B_ICH_HDA_PC_D2_SUP                         BIT10
#define   B_ICH_HDA_PC_D1_SUP                         BIT9
#define   B_ICH_HDA_PC_AUX                            (BIT8 | BIT7 | BIT6)
#define   B_ICH_HDA_PC_DSI                            BIT5
#define   B_ICH_HDA_PC_PMEC                           BIT3
#define   B_ICH_HDA_PC_VER                            (BIT2 | BIT1 | BIT0)
#define R_ICH_HDA_PCS                               0x54
#define   B_ICH_HDA_PCS_DATA                          0xFF000000
#define   B_ICH_HDA_PCS_CCE                           BIT23
#define   B_ICH_HDA_PCS_PMES                          BIT15
#define   B_ICH_HDA_PCS_PMEE                          BIT8
#define   B_ICH_HDA_PCS_PS                            (BIT1 | BIT0)
#define     V_ICH_HDA_PCS_PS0                         0x00
#define     V_ICH_HDA_PCS_PS3                         0x03
#define R_ICH_HDA_MID                               0x60
#define   B_ICH_HDA_MID_NEXT                          0xFF00
#define   B_ICH_HDA_MID_CAP                           0x00FF
#define R_ICH_HDA_MMC                               0x62
#define   B_ICH_HDA_MMC_64ADD                         BIT7
#define   B_ICH_HDA_MMC_MME                           (BIT6 | BIT5 | BIT4)
#define   B_ICH_HDA_MMC_MMC                           (BIT3 | BIT2 | BIT1)
#define   B_ICH_HDA_MMC_ME                            BIT0
#define R_ICH_HDA_MMLA                              0x64
#define   B_ICH_HDA_MMLA                              0xFFFFFFFC
#define R_ICH_HDA_MMUA                              0x68
#define   B_ICH_HDA_MMUA                              0xFFFFFFFF
#define R_ICH_HDA_MMD                               0x6C
#define   B_ICH_HDA_MMD                               0xFFFF
#define R_ICH_HDA_PXID                              0x70
#define   B_ICH_HDA_PXID_NEXT                         0xFF00
#define   B_ICH_HDA_PXID_CAP                          0x00FF
#define R_ICH_HDA_PXC                               0x72
#define   B_ICH_HDA_PXC_IMN                           0x3E00
#define   B_ICH_HDA_PXC_SI                            BIT8
#define   B_ICH_HDA_PXC_DPT                           0x00F0
#define   B_ICH_HDA_PXC_CV                            0x000F
#define R_ICH_HDA_DEVCAP                            0x74
#define   B_ICH_HDA_DEVCAP_FLR                        BIT28
#define   B_ICH_HDA_DEVCAP_SPLS                       (BIT27 | BIT26)
#define   B_ICH_HDA_DEVCAP_SPLV                       0x03FC0000
#define   B_ICH_HDA_DEVCAP_PWRIP                      BIT14
#define   B_ICH_HDA_DEVCAP_ATTNIP                     BIT13
#define   B_ICH_HDA_DEVCAP_ATTNBP                     BIT12
#define   B_ICH_HDA_DEVCAP_EL1AL                      0x00000E00
#define   B_ICH_HDA_DEVCAP_EL0AL                      0x000001C0
#define   B_ICH_HDA_DEVCAP_ETFS                       BIT5
#define   B_ICH_HDA_DEVCAP_PFS                        (BIT4 | BIT3)
#define   B_ICH_HDA_DEVCAP_MPSS                       0x00000007
#define R_ICH_HDA_DEVC                              0x78
#define   B_ICH_HDA_DEVC_IF                           BIT15
#define   B_ICH_HDA_DEVC_MRRS                         (BIT13 | BIT12 | BIT11)
#define   B_ICH_HDA_DEVC_NSNPEN                       BIT11
#define   B_ICH_HDA_DEVC_APE                          BIT10
#define   B_ICH_HDA_DEVC_PFE                          BIT9
#define   B_ICH_HDA_DEVC_ETFE                         BIT8
#define   B_ICH_HDA_DEVC_MPS                          (BIT7 | BIT6 | BIT5)
#define   B_ICH_HDA_DEVC_ERO                          BIT4
#define   B_ICH_HDA_DEVC_URRE                         BIT3
#define   B_ICH_HDA_DEVC_FERE                         BIT2
#define   B_ICH_HDA_DEVC_NFERE                        BIT1
#define   B_ICH_HDA_DEVC_CERE                         BIT0
#define R_ICH_HDA_DEVS                              0x7A
#define   B_ICH_HDA_DEVS_TP                           BIT5
#define   B_ICH_HDA_DEVS_AUXPD                        BIT4
#define   B_ICH_HDA_DEVS_URD                          BIT3
#define   B_ICH_HDA_DEVS_FED                          BIT2
#define   B_ICH_HDA_DEVS_NFED                         BIT1
#define   B_ICH_HDA_DEVS_CED                          BIT0
#define R_ICH_HDA_VCCAP                             0x100
#define   B_ICH_HDA_VCCAP_NCO                         0xFFF00000
#define   B_ICH_HDA_VCCAP_CAPVER                      0x000F0000
#define   B_ICH_HDA_VCCAP_PCIEEC                      0x0000FFFF
#define R_ICH_HDA_PVCCAP1                           0x104
#define   B_ICH_HDA_PVCCAP1_PATES                     0xFFF00000
#define   B_ICH_HDA_PVCCAP1_RC                        0x00000300
#define   B_ICH_HDA_PVCCAP1_LPEVCC                    0x00000070
#define   B_ICH_HDA_PVCCAP1_EVCC                      0x00000007
#define R_ICH_HDA_PVCCAP2                           0x108
#define   B_ICH_HDA_PVCCAP2_VCATO                     0xFF000000
#define   B_ICH_HDA_PVCCAP2_VCAC                      0x000000FF
#define R_ICH_HDA_PVCCTL                            0x10C
#define   B_ICH_HDA_PVCCTL_VCAS                       0x000E
#define   B_ICH_HDA_PVCCTL_LVCAT                      0x0001
#define R_ICH_HDA_PVCSTS                            0x10E
#define   B_ICH_HDA_PVCSTS_VCATS                      0x0001
#define R_ICH_HDA_VC0CAP                            0x110
#define  S_ICH_HDA_VC0CAP                            4
#define   B_ICH_HDA_VC0CAP_PATO                       0xFF000000
#define   B_ICH_HDA_VC0CAP_MTS                        0x007F0000
#define   B_ICH_HDA_VC0CAP_RST                        BIT15
#define   B_ICH_HDA_VC0CAP_APS                        BIT14
#define   B_ICH_HDA_VC0CAP_PAC                        0x000000FF
#define R_ICH_HDA_VC0CTL                            0x114
#define  S_ICH_HDA_VC0CTL                            4
#define   B_ICH_HDA_VC0CTL_VC0EN                      BIT31
#define   B_ICH_HDA_VC0CTL_VC0ID                      0x07000000
#define   B_ICH_HDA_VC0CTL_PAS                        0x000E0000
#define   B_ICH_HDA_VC0CTL_LPAT                       BIT16
#define   B_ICH_HDA_VC0CTL_TCVC0_MAP                  0x000000FF
#define R_ICH_HDA_VC0STS                            0x11A
#define  S_ICH_HDA_VC0STS                            2
#define   B_ICH_HDA_VC0STS_VC0NP                      BIT1
#define   B_ICH_HDA_VC0STS_PATS                       BIT0
#define R_ICH_HDA_VCICAP                            0x11C
#define  S_ICH_HDA_VCICAP                            4
#define   B_ICH_HDA_VCICAP_PATO                       0xFF000000
#define   B_ICH_HDA_VCICAP_MTS                        0x007F0000
#define   B_ICH_HDA_VCICAP_RST                        BIT15
#define   B_ICH_HDA_VCICAP_APS                        BIT14
#define   B_ICH_HDA_VCICAP_PAC                        0x000000FF
#define R_ICH_HDA_VCICTL                            0x120
#define  S_ICH_HDA_VCICTL                            4
#define   B_ICH_HDA_VCICTL_EN                         BIT31
#define   B_ICH_HDA_VCICTL_ID                         (BIT26 | BIT25 | BIT24)
#define   V_ICH_HDA_VCICTL_PAS                        0x000E0000
#define   V_ICH_HDA_VCICTL_LPAT                       BIT16
#define   B_ICH_HDA_VCICTL_TCVCI_MAP                  0x000000FF
#define   V_ICH_HDA_VCICTL_ID_MASK                    0x07000000
#define   V_ICH_HDA_VCICTL_TVM_MASK                   0xFF
#define R_ICH_HDA_VCISTS                            0x126
#define  S_ICH_HDA_VCISTS                            1
#define   B_ICH_HDA_VCISTS_VCINP                      BIT1
#define   B_ICH_HDA_VCISTS_PATS                       BIT0
#define R_ICH_HDA_RCCAP                             0x130
#define   B_ICH_HDA_RCCAP_NCO                         0xFFF00000
#define   B_ICH_HDA_RCCAP_CV                          0x000F0000
#define   B_ICH_HDA_RCCAP_PCIEECID                    0x0000FFFF
#define R_ICH_HDA_ESD                               0x134
#define   B_ICH_HDA_ESD_PN                            0xFF000000
#define   B_ICH_HDA_ESD_CID                           0x00FF0000
#define   B_ICH_HDA_ESD_NOLE                          0x0000FF00
#define   B_ICH_HDA_ESD_ELTYP                         0x0000000F
#define R_ICH_HDA_L1DESC                            0x140
#define  S_ICH_HDA_L1DESC                            4
#define   B_ICH_HDA_LIDESC_TPN                        0xFF000000
#define   B_ICH_HDA_LIDESC_TCID                       0x00FF0000
#define   B_ICH_HDA_LIDESC_LT                         BIT1
#define   B_ICH_HDA_LIDESC_LV                         BIT0
#define R_ICH_HDA_L1ADDL                            0x148
#define   B_ICH_HDA_L1ADDL_LNK1LA                     0xFFFFC000
#define R_ICH_HDA_L1ADDU                            0x14C
#define   B_ICH_HDA_L1ADDU                            0xFFFFFFFF
//
// Intel High Definition Audio Memory Mapped Configuration Registers
//
#define R_HDA_GCAP                                  0x00
#define  S_HDA_GCAP                                  2
#define   B_HDA_GCAP_NOSSUP                           0xF000
#define   B_HDA_GCAP_NISSUP                           0x0F00
#define   B_HDA_GCAP_NBSSUP                           0x00F8
#define   B_HDA_GCAP_NSDOS                            BIT1
#define   B_HDA_GCAP_64ADSUP                          BIT0
#define R_HDA_VMIN                                  0x02
#define   B_HDA_VMIN_MV                               0xFF
#define R_HDA_VMAJ                                  0x03
#define   B_HDA_VMAJ_MV                               0xFF
#define R_HDA_OUTPAY                                0x04
#define   B_HDA_OUTPAY_CAP                            0x007F
#define R_HDA_INPAY                                 0x06
#define   B_HDA_INPAY_CAP                             0x007F
#define R_HDA_GCTL                                  0x08
#define   B_HDA_GCTL_AURE                             BIT8
#define   B_HDA_GCTL_FC                               BIT1
#define   B_HDA_GCTL_CRST                             BIT0
#define R_HDA_WAKEEN                                0x0C
#define   B_HDA_WAKEEN_SDI_3                          BIT3
#define   B_HDA_WAKEEN_SDI_2                          BIT2
#define   B_HDA_WAKEEN_SDI_1                          BIT1
#define   B_HDA_WAKEEN_SDI_0                          BIT0
#define R_HDA_STATESTS                              0x0E
#define   B_HDA_STATESTS_SDIN3                        BIT3
#define   B_HDA_STATESTS_SDIN2                        BIT2
#define   B_HDA_STATESTS_SDIN1                        BIT1
#define   B_HDA_STATESTS_SDIN0                        BIT0
#define R_HDA_GSTS                                  0x10
#define   B_HDA_GSTS_DMIS                             BIT3
#define   B_HDA_GSTS_DM                               BIT2
#define   B_HDA_GSTS_FS                               BIT1
#define R_HDA_ECAP                                  0x14
#define   B_HDA_ECAP_DS                               BIT0
#define R_HDA_OUTSTRMPAY                            0x18
#define  S_HDA_OUTSTRMPAY                            2
#define   B_HDA_OUTSTRMPAY_OPADTYPE                   0xC000
#define     V_HDA_OUTSTRMPAY_OPADTYPE3                  0xC000
#define     V_HDA_OUTSTRMPAY_OPADTYPE2                  0x8000
#define     V_HDA_OUTSTRMPAY_OPADTYPE0                  0x0000
//#define   B_HDA_OUTSTRMPAY_OUTSTRMPAY                 0x3FFF
#define R_HDA_INSTRMPAY                             0x1A
#define   B_HDA_INSTRMPAY_IPADTYPE                    0xC000
#define     V_HDA_INSTRMPAY_IPADTYPE3                   0xC000
#define     V_HDA_INSTRMPAY_IPADTYPE2                   0x8000
#define     V_HDA_INSTRMPAY_IPADTYPE0                   0x0000
//#define   B_HDA_INSTRMPAY_INSTRMPAY                   0x3FFF
#define R_HDA_INTCTL                                0x20
#define   B_HDA_INTCTL_GIE                            BIT31
#define   B_HDA_INTCTL_CIE                            BIT30
#define   B_HDA_INTCTL_SIE_OS4                        BIT7
#define   B_HDA_INTCTL_SIE_OS3                        BIT6
#define   B_HDA_INTCTL_SIE_OS2                        BIT5
#define   B_HDA_INTCTL_SIE_OS1                        BIT4
#define   B_HDA_INTCTL_SIE_IS4                        BIT3
#define   B_HDA_INTCTL_SIE_IS3                        BIT2
#define   B_HDA_INTCTL_SIE_IS2                        BIT1
#define   B_HDA_INTCTL_SIE_IS1                        BIT0
#define R_HDA_INTSTS                                0x24
#define   B_HDA_INTSTS_GIS                            BIT31
#define   B_HDA_INTSTS_CIS                            BIT30
#define   B_HDA_INTSTS_SIS_OS4                        BIT7
#define   B_HDA_INTSTS_SIS_OS3                        BIT6
#define   B_HDA_INTSTS_SIS_OS2                        BIT5
#define   B_HDA_INTSTS_SIS_OS1                        BIT4
#define   B_HDA_INTSTS_SIS_IS4                        BIT3
#define   B_HDA_INTSTS_SIS_IS3                        BIT2
#define   B_HDA_INTSTS_SIS_IS2                        BIT1
#define   B_HDA_INTSTS_SIS_IS1                        BIT0
#define R_HDA_WALCLK                                0x30
#define   B_HDA_WALCLK_WCC                            0xFFFFFFFF
//#define R_HDA_SSYNC                                 0x34
#define  S_HDA_SSYNC                                 4
#define   B_HDA_SSYNC_OS2                             BIT5
#define   B_HDA_SSYNC_OS1                             BIT4
#define   B_HDA_SSYNC_IS4                             BIT3
#define   B_HDA_SSYNC_IS3                             BIT2
#define   B_HDA_SSYNC_IS2                             BIT1
#define   B_HDA_SSYNC_IS1                             BIT0
#define R_HDA_CORBLBASE                             0x40
#define   B_HDA_CORBLBASE_BA                          0xFFFFFF80
#define   B_HDA_CORBLBASE_UB                          0x0000007F
#define R_HDA_CORBUBASE                             0x44
#define   B_HDA_CORBUBASE_BA                          0xFFFFFFFF
#define R_HDA_CORBWP                                0x48
#define   B_HDA_CORBWP                                0x000000FF
#define R_HDA_CORBRP                                0x4A
#define   B_HDA_CORBRP_PRST                           BIT15
#define   B_HDA_CORBRP_P                              0x00FF
#define R_HDA_CORBCTL                               0x4C
#define   B_HDA_CORBCTL_DMA_EN                        BIT1
#define   B_HDA_CORBCTL_MEMERRINTR_EN                 BIT0
#define R_HDA_CORBST                                0x4D
#define   B_HDA_CORBST_CMEI                           BIT0
#define R_HDA_CORBSIZE                              0x4E
#define   B_HDA_CORBSIZE_CAP                          0xF0
//#define   B_HDA_CORBSIZE_SIZE                         0x0F
#define R_HDA_RIRBLBASE                             0x50
#define   B_HDA_RIRBLBASE_BA                          0xFFFFFF80
#define   B_HDA_RIRBLBASE_UB                          0x0000007F
#define R_HDA_RIRBUBASE                             0x54
#define   B_HDA_RIRBUBASE_BA                          0xFFFFFFFF
#define R_HDA_RIRBWP                                0x58
#define   B_HDA_RIRBWP_RST                            BIT15
#define   B_HDA_RIRBWP_WP                             0x00FF
#define R_HDA_RINTCNT                               0x5A
#define   B_HDA_RINTCNT                               0x00FF
#define R_HDA_RIRBCTL                               0x5C
#define   B_HDA_RIRBCTL_ROIC                          BIT2
#define   B_HDA_RIRBCTL_DMA                           BIT1
#define   B_HDA_RIRBCTL_RIC                           BIT0
#define R_HDA_RIRBSTS                               0x5D
#define   B_HDA_RIRBSTS_ROIS                          BIT2
#define   B_HDA_RIRBSTS_RI                            BIT0
#define R_HDA_RIRBSIZE                              0x5E
#define   B_HDA_RIRBSIZE_CAP                          0xF0
#define   B_HDA_RIRBSIZE_SIZE                         0x03
#define R_HDA_IC                                    0x60
#define   B_HDA_IC                                    0xFFFFFFFF
#define R_HDA_IR                                    0x64
#define   B_HDA_IR                                    0xFFFFFFFF
#define R_HDA_IRS                                   0x68
#define   B_HDA_IRS_IRV                               BIT1
#define   B_HDA_IRS_ICB                               BIT0
#define R_HDA_DPLBASE                               0x70
#define   B_HDA_DPLBASE_LBA                           0xFFFFFF80
#define   B_HDA_DPLBASE_LBU                           0x0000007E
#define   B_HDA_DPLBASE_BUF_EN                        0x00000001
#define R_HDA_DPUBASE                               0x74
#define   B_HDA_DPUBASE_UBA                           0xFFFFFFFF
#define R_HDA_SDCTL_IN_0                            0x80
#define R_HDA_SDCTL_IN_1                            0xA0
#define R_HDA_SDCTL_IN_2                            0xC0
#define R_HDA_SDCTL_IN_3                            0xE0
#define R_HDA_SDCTL_OUT_0                           0x100
#define R_HDA_SDCTL_OUT_1                           0x120
#define R_HDA_SDCTL_OUT_2                           0x140
#define R_HDA_SDCTL_OUT_3                           0x160
#define   B_HDA_SDCTL_IO_SN                           (BIT23 | BIT22 | BIT21 | BIT20)
#define   B_HDA_SDCTL_IO_BDC                          BIT19
#define   B_HDA_SDCTL_IO_TP                           BIT18
#define   B_HDA_SDCTL_IO_SC                           (BIT17 | BIT16)
#define   B_HDA_SDCTL_IO_DEIE                         BIT4
#define   B_HDA_SDCTL_IO_FIFO_ERRINT_EN               BIT3
#define   B_HDA_SDCTL_IO_INTR_ONCOMP_EN               BIT2
#define   B_HDA_SDCTL_IO_RUN                          BIT1
#define   B_HDA_SDCTL_IO_SRST                         BIT0
#define R_HDA_SDSTS_IN_0                            0x83
#define R_HDA_SDSTS_IN_1                            0xA3
#define R_HDA_SDSTS_IN_2                            0xC3
#define R_HDA_SDSTS_IN_3                            0xE3
#define R_HDA_SDSTS_OUT_0                           0x103
#define R_HDA_SDSTS_OUT_1                           0x123
#define R_HDA_SDSTS_OUT_2                           0x143
#define R_HDA_SDSTS_OUT_3                           0x163
#define   B_HDA_SDSTS_IO_FIFORDY                      BIT5
#define   B_HDA_SDSTS_IO_DE                           BIT4
#define   B_HDA_SDCTL_IO_FIFO_ERROR                   BIT3
#define   B_HDA_SDCTL_IO_BUF_COMPINTR_STS             BIT2
#define R_HDA_SDLPIB_IN_0                            0x84
#define R_HDA_SDLPIB_IN_1                            0xA4
#define R_HDA_SDLPIB_IN_2                            0xC4
#define R_HDA_SDLPIB_IN_3                            0xE4
#define R_HDA_SDLPIB_OUT_0                           0x104
#define R_HDA_SDLPIB_OUT_1                           0x124
#define R_HDA_SDLPIB_OUT_2                           0x144
#define R_HDA_SDLPIB_OUT_3                           0x164
#define   B_HDA_SDLPIB_BUFFER                        0xFFFFFFFF
#define R_HDA_SDCBL_IN_0                            0x88
#define R_HDA_SDCBL_IN_1                            0xA8
#define R_HDA_SDCBL_IN_2                            0xC8
#define R_HDA_SDCBL_IN_3                            0xE8
#define R_HDA_SDCBL_OUT_0                           0x108
#define R_HDA_SDCBL_OUT_1                           0x128
#define R_HDA_SDCBL_OUT_2                           0x148
#define R_HDA_SDCBL_OUT_3                           0x168
#define   B_HDA_SDCBL_BUFLNG                          0xFFFFFFFF
#define R_HDA_SDLVI_IN_0                            0x8C
#define R_HDA_SDLVI_IN_1                            0xAC
#define R_HDA_SDLVI_IN_2                            0xCC
#define R_HDA_SDLVI_IN_3                            0xEC
#define R_HDA_SDLVI_OUT_0                           0x10C
#define R_HDA_SDLVI_OUT_1                           0x12C
#define R_HDA_SDLVI_OUT_2                           0x14C
#define R_HDA_SDLVI_OUT_3                           0x16C
#define   B_HDA_SDLVI_LVI                             0x0000
#define R_HDA_SDFIFOW_IN_0                          0x8E
#define R_HDA_SDFIFOW_IN_1                          0xAE
#define R_HDA_SDFIFOW_IN_2                          0xCE
#define R_HDA_SDFIFOW_IN_3                          0xEE
#define R_HDA_SDFIFOW_OUT_0                         0x10E
#define R_HDA_SDFIFOW_OUT_1                         0x12E
#define R_HDA_SDFIFOW_OUT_2                         0x14E
#define R_HDA_SDFIFOW_OUT_3                         0x16E
#define   B_HDA_SDFIFOW_FIFOW                        0x0003
#define     V_HDA_SDFIFOW_FIFOW2                       0x0002
#define     V_HDA_SDFIFOW_FIFOW3                       0x0003
#define     V_HDA_SDFIFOW_FIFOW4                       0x0004
#define     V_HDA_SDFIFOW_FIFOW5                       0x0005
#define R_HDA_SDFIFOS_IN_0                          0x90
#define R_HDA_SDFIFOS_IN_1                          0xB0
#define R_HDA_SDFIFOS_IN_2                          0xD0
#define R_HDA_SDFIFOS_IN_3                          0xF0
#define   V_HDA_SDFIFOS_IN_FIFOSIZE120B               0x0077
#define   V_HDA_SDFIFOS_IN_FIFOSIZE160B               0x009F
#define R_HDA_SDFIFOS_OUT_0                         0x110
#define R_HDA_SDFIFOS_OUT_1                         0x130
#define R_HDA_SDFIFOS_OUT_2                         0x150
#define R_HDA_SDFIFOS_OUT_3                         0x170
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE16B               0x000F
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE32B               0x001F
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE64B               0x003F
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE128B              0x007F
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE192B              0x00BF
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE256B              0x00FF
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE384B              0x017F
#define   B_HDA_SDFIFOS_OUT_FIFOSIZE512B              0x01FF
#define R_HDA_SDFMT_IN_0                          0x92
#define R_HDA_SDFMT_IN_1                          0xB2
#define R_HDA_SDFMT_IN_2                          0xD2
#define R_HDA_SDFMT_IN_3                          0xF2
#define R_HDA_SDFMT_OUT_0                         0x112
#define R_HDA_SDFMT_OUT_1                         0x132
#define R_HDA_SDFMT_OUT_2                         0x152
#define R_HDA_SDFMT_OUT_3                         0x172
#define   B_HDA_SDFMT_SBR                           BIT14
#define   B_HDA_SDFMT_SBRM                          (BIT13 | BIT12 | BIT11)
#define   B_HDA_SDFMT_SBRD                          (BIT10 | BIT9 | BIT8)
#define   B_HDA_SDFMT_BITS                          (BIT6 | BIT5 | BIT4)
#define   B_HDA_SDFMT_CHAN                          (BIT3 | BIT2 | BIT1 | BIT0)
#define R_HDA_SDBDPL_IN_0                          0x98
#define R_HDA_SDBDPL_IN_1                          0xB8
#define R_HDA_SDBDPL_IN_2                          0xD8
#define R_HDA_SDBDPL_IN_3                          0xF8
#define R_HDA_SDBDPL_OUT_0                         0x118
#define R_HDA_SDBDPL_OUT_1                         0x138
#define R_HDA_SDBDPL_OUT_2                         0x158
#define R_HDA_SDBDPL_OUT_3                         0x178
#define   B_HDA_SDBDPL_LBA                           0xFFFFFF80
#define R_HDA_SDBDPU_IN_0                          0x9C
#define R_HDA_SDBDPU_IN_1                          0xBC
#define R_HDA_SDBDPU_IN_2                          0xDC
#define R_HDA_SDBDPU_IN_3                          0xFC
#define R_HDA_SDBDPU_OUT_0                         0x11C
#define R_HDA_SDBDPU_OUT_1                         0x13C
#define R_HDA_SDBDPU_OUT_2                         0x15C
#define R_HDA_SDBDPU_OUT_3                         0x17C
#define   B_HDA_SDBDPU_LBA                           0xFFFFFFFF

//
// ICH9M Pci Express Root Ports (D28:F0~5)
//
#define PCI_DEVICE_NUMBER_ICH_ROOTPORT                 28
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_1             0
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_2             1
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_3             2
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_4             3
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_5             4
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_6             5
#define R_ICH_PCIE_VENDOR_ID                        0x00
#define   V_ICH_PCIE_VENDOR_ID                         V_INTEL_VENDOR_ID
#define R_ICH_PCIE_DEVICE_ID                        0x02
#define   V_ICH9_PCIE_DEVICE_ID_0                      0x2940
#define   V_ICH9_PCIE_DEVICE_ID_1                      0x2941
#define   V_ICH9_PCIE_DEVICE_ID_2                      0x2942
#define   V_ICH9_PCIE_DEVICE_ID_3                      0x2943
#define   V_ICH9_PCIE_DEVICE_ID_4                      0x2944
#define   V_ICH9_PCIE_DEVICE_ID_5                      0x2945
#define   V_ICH9_PCIE_DEVICE_ID_6                      0x2946
#define   V_ICH9_PCIE_DEVICE_ID_7                      0x2947
#define   V_ICH9_PCIE_DEVICE_ID_8                      0x2948
#define   V_ICH9_PCIE_DEVICE_ID_9                      0x2949
#define   V_ICH9_PCIE_DEVICE_ID_A                      0x294A
#define   V_ICH9_PCIE_DEVICE_ID_B                      0x294B

#define   V_ICH10_PCIE_DEVICE_ID_0                     0x3A40 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_1                     0x3A41 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_2                     0x3A42 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_3                     0x3A43 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_4                     0x3A44 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_5                     0x3A45 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_6                     0x3A46 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_7                     0x3A47 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_8                     0x3A48 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_9                     0x3A49 //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_A                     0x3A4A //ESS_OVERRIDE
#define   V_ICH10_PCIE_DEVICE_ID_B                     0x3A4B //ESS_OVERRIDE

#define R_ICH_PCIE_PCICMD                              0x04
#define  S_ICH_PCIE_PCICMD                              2
#define   B_ICH_PCIE_PCICMD_ID                            BIT10
#define   B_ICH_PCIE_PCICMD_FBE                           BIT9
#define   B_ICH_PCIE_PCICMD_SEE                           BIT8
#define   B_ICH_PCIE_PCICMD_WCC                           BIT7
#define   B_ICH_PCIE_PCICMD_PER                           BIT6
#define   B_ICH_PCIE_PCICMD_VPS                           BIT5
#define   B_ICH_PCIE_PCICMD_PMWE                          BIT4
#define   B_ICH_PCIE_PCICMD_SCE                           BIT3
#define   B_ICH_PCIE_PCICMD_BME                           BIT2
#define   B_ICH_PCIE_PCICMD_MSE                           BIT1
#define   B_ICH_PCIE_PCICMD_IOSE                          BIT0
#define R_ICH_PCIE_PCISTS                           0x06
#define  S_ICH_PCIE_PCISTS                           2
#define   B_ICH_PCIE_PCISTS_DPE                        BIT15
#define   B_ICH_PCIE_PCISTS_SSE                        BIT14
#define   B_ICH_PCIE_PCISTS_RMA                        BIT13
#define   B_ICH_PCIE_PCISTS_RTA                        BIT12
#define   B_ICH_PCIE_PCISTS_STA                        BIT11
#define   B_ICH_PCIE_PCISTS_DEV_STS                    (BIT10 | BIT9)
#define   B_ICH_PCIE_PCISTS_DPED                       BIT8
#define   B_ICH_PCIE_PCISTS_FB2BC                      BIT7
#define   B_ICH_PCIE_PCISTS_66MHZ_CAP                  BIT5
#define   B_ICH_PCIE_PCISTS_CAP_LST                    BIT4
#define   B_ICH_PCIE_PCISTS_INTR_STS                   BIT3
#define R_ICH_PCIE_RID                              0x08
#define   B_ICH_PCIE_RID                               0xFF
#define R_ICH_PCIE_PI                               0x09
#define   B_ICH_PCIE_PI                                0xFF
#define R_ICH_PCIE_SCC                              0x0A
#define   B_ICH_PCIE_SCC                               0xFF
#define     V_ICH_PCIE_SCC_04                            0x04
#define     V_ICH_PCIE_SCC_00                            0x00
#define R_ICH_PCIE_BCC                              0x0B
#define   B_ICH_PCIE_BCC                               0xFF
#define R_ICH_PCIE_CLS                              0x0C
#define   B_ICH_PCIE_CLS                               0xFF
#define R_ICH_PCIE_PLT                              0x0D
#define   B_ICH_PCIE_PLT_LC                            0xF8
#define R_ICH_PCIE_HEADTYPE                         0x0E
#define   B_ICH_PCIE_HEADTYPE_MFD                      BIT7
#define   B_ICH_PCIE_HEADTYPE_CL                       0x7F
#define     V_ICH_PCIE_HEADTYPE_CL_01                    0x01
#define     V_ICH_PCIE_HEADTYPE_CL_00                    0x00
#define R_ICH_PCIE_BNUM                             0x18
#define   B_ICH_PCIE_BNUM_SBBN                         0x00FF0000
#define   B_ICH_PCIE_BNUM_SCBN                         0x0000FF00
#define   B_ICH_PCIE_BNUM_PBN                          0x000000FF
#define R_ICH_PCIE_SLT                              0x1B
#define   B_ICH_PCIE_SLT                               0xFF
#define R_ICH_PCIE_IOBL                             0x1C
#define   B_ICH_PCIE_IOBL_IOLA                         0xF000
#define   B_ICH_PCIE_IOBL_IOLC                         0x0F00
#define   B_ICH_PCIE_IOBL_IOBA                         0x00F0
#define   B_ICH_PCIE_IOBL_IOBC                         0x000F
#define R_ICH_PCIE_SSTS                             0x1E
#define  S_ICH_PCIE_SSTS                             2
#define   B_ICH_PCIE_SSTS_DPE                          BIT15
#define   B_ICH_PCIE_SSTS_RSE                          BIT14
#define   B_ICH_PCIE_SSTS_RMA                          BIT13
#define   B_ICH_PCIE_SSTS_RTA                          BIT12
#define   B_ICH_PCIE_SSTS_STA                          BIT11
#define   B_ICH_PCIE_SSTS_SDTS                         (BIT10 | BIT9)
#define   B_ICH_PCIE_SSTS_DPD                          BIT8
#define   B_ICH_PCIE_SSTS_SFBC                         BIT7
#define   B_ICH_PCIE_SSTS_SC66                         BIT5
#define R_ICH_PCIE_MBL                              0x20
#define   B_ICH_PCIE_MBL_ML                            0xFFF00000
#define   B_ICH_PCIE_MBL_MB                            0x0000FFF0
#define R_ICH_PCIE_PMBL                             0x24
#define   B_ICH_PCIE_PMBL_PML                          0xFFF00000
#define   B_ICH_PCIE_PMBL_I64L                         0x000F0000
#define   B_ICH_PCIE_PMBL_PMB                          0x0000FFF0
#define   B_ICH_PCIE_PMBL_I64B                         0x0000000F
#define R_ICH_PCIE_PMBU32                           0x28
#define   B_ICH_PCIE_PMBU32                            0xFFFFFFFF
#define R_ICH_PCIE_PMLU32                           0x2C
#define   B_ICH_PCIE_PMLU32                            0xFFFFFFFF
#define R_ICH_PCIE_CAPP                             0x34
#define   B_ICH_PCIE_CAPP                              0xFF
#define R_ICH_PCIE_INTR                             0x3C
#define   B_ICH_PCIE_INTR_IPIN                         0xFF00
#define   B_ICH_PCIE_INTR_ILINE                        0x00FF
#define R_ICH_PCIE_BCTRL                            0x3E
#define  S_ICH_PCIE_BCTRL                            2
#define   B_ICH_PCIE_BCTRL_DTSE                        BIT11
#define   B_ICH_PCIE_BCTRL_DTS                         BIT10
#define   B_ICH_PCIE_BCTRL_SDT                         BIT9
#define   B_ICH_PCIE_BCTRL_PDT                         BIT8
#define   B_ICH_PCIE_BCTRL_FBE                         BIT7
#define   B_ICH_PCIE_BCTRL_SBR                         BIT6
#define   B_ICH_PCIE_BCTRL_MAM                         BIT5
#define   B_ICH_PCIE_BCTRL_V16                         BIT4
#define   B_ICH_PCIE_BCTRL_VE                          BIT3
#define   B_ICH_PCIE_BCTRL_IE                          BIT2
#define   B_ICH_PCIE_BCTRL_SE                          BIT1
#define   B_ICH_PCIE_BCTRL_PERE                        BIT0
#define R_ICH_PCIE_CLIST                            0x40
#define   B_ICH_PCIE_CLIST_NEXT                        0xFF00
#define   B_ICH_PCIE_CLIST_CID                         0x00FF
#define   V_ICH_PCIE_CLIST_CID                         0x10   // ESS_OVERRIDE
#define R_ICH_PCIE_XCAP                             0x42
#define   V_ICH_PCIE_XCAP_DT                           0x004  // ESS_OVERRIDE
#define  S_ICH_PCIE_XCAP                             2
#define   B_ICH_PCIE_XCAP_IMN                          0x3E00
#define   B_ICH_PCIE_XCAP_SI                           BIT8
#define   B_ICH_PCIE_XCAP_DT                           0x00F0
#define   B_ICH_PCIE_XCAP_CV                           0x000F
#define R_ICH_PCIE_DCAP                             0x44
#define  S_ICH_PCIE_DCAP                             4
#define   B_ICH_PCIE_DCAP_CSPS                         0x0C000000
#define   B_ICH_PCIE_DCAP_CSPV                         0x03FC0000
#define   B_ICH_PCIE_DCAP_RBER                         BIT15
#define   B_ICH_PCIE_DCAP_PIP                          BIT14
#define   B_ICH_PCIE_DCAP_AIP                          BIT13
#define   B_ICH_PCIE_DCAP_ABP                          BIT12
#define   B_ICH_PCIE_DCAP_E1AL                         0x00000E00
#define   B_ICH_PCIE_DCAP_E0AL                         0x000001C0
#define   B_ICH_PCIE_DCAP_ETFS                         BIT5
#define   B_ICH_PCIE_DCAP_PFS                          0x00000018
#define   B_ICH_PCIE_DCAP_MPS                          0x00000007
#define R_ICH_PCIE_DCTL                             0x48
#define  S_ICH_PCIE_DCTL                             2
#define   B_ICH_PCIE_DCTL_MRRS                         0x7000
#define   B_ICH_PCIE_DCTL_ENS                          BIT11
#define   B_ICH_PCIE_DCTL_APME                         BIT10
#define   B_ICH_PCIE_DCTL_PFE                          BIT9
#define   B_ICH_PCIE_DCTL_ETFE                         BIT8
#define   B_ICH_PCIE_DCTL_MPS                          (BIT7 | BIT6 | BIT5)
#define   B_ICH_PCIE_DCTL_ERO                          BIT4
#define   B_ICH_PCIE_DCTL_URE                          BIT3
#define   B_ICH_PCIE_DCTL_FEE                          BIT2
#define   B_ICH_PCIE_DCTL_NFE                          BIT1
#define   B_ICH_PCIE_DCTL_CEE                          BIT0
#define R_ICH_PCIE_DSTS                             0x4A
#define   B_ICH_PCIE_DSTS_TDP                          BIT5
#define   B_ICH_PCIE_DSTS_APD                          BIT4
#define   B_ICH_PCIE_DSTS_URD                          BIT3
#define   B_ICH_PCIE_DSTS_FED                          BIT2
#define   B_ICH_PCIE_DSTS_NFED                         BIT1
#define   B_ICH_PCIE_DSTS_CED                          BIT0
#define R_ICH_PCIE_LCAP                             0x4C
#define   B_ICH_PCIE_LCAP_PN                           0xFF000000
#define     V_ICH_PCIE_LCAP_PN1                          0x01000000
#define     V_ICH_PCIE_LCAP_PN2                          0x02000000
#define     V_ICH_PCIE_LCAP_PN3                          0x03000000
#define     V_ICH_PCIE_LCAP_PN4                          0x04000000
#define     V_ICH_PCIE_LCAP_PN5                          0x05000000
#define     V_ICH_PCIE_LCAP_PN6                          0x06000000
#define   B_ICH_PCIE_LCAP_LARC                         BIT20
#define   B_ICH_PCIE_LCAP_EL1                          0x00038000
#define     V_ICH_PCIE_LCAP_EL0_0                        0x00000000
#define     V_ICH_PCIE_LCAP_EL0_1                        0x00001000
#define   B_ICH_PCIE_LCAP_APMS                         0x00000C00
#define     V_ICH_PCIE_LCAP_APMS_0                       0x00000000
#define     V_ICH_PCIE_LCAP_APMS_1                       0x00000400
#define     V_ICH_PCIE_LCAP_APMS_2                       0x00000800
#define     V_ICH_PCIE_LCAP_APMS_3                       0x00000C00
#define   B_ICH_PCIE_LCAP_MLW                          0x000003F0
#define   B_ICH_PCIE_LCAP_MLS                          0x0000000F
#define R_ICH_PCIE_LCTL                             0x50
#define   B_ICH_PCIE_LCTL_ES                           BIT7
#define   B_ICH_PCIE_LCTL_CCC                          BIT6
#define   B_ICH_PCIE_LCTL_RL                           BIT5
#define   B_ICH_PCIE_LCTL_LD                           BIT4
#define   B_ICH_PCIE_LCTL_RCBC                         BIT3
#define   B_ICH_PCIE_LCTL_APMC                         (BIT1 | BIT0)
#define R_ICH_PCIE_LSTS                             0x52
#define  S_ICH_PCIE_LSTS                             2
#define   B_ICH_PCIE_LSTS_DLLA                         BIT13
#define   B_ICH_PCIE_LSTS_SCC                          BIT12
#define   B_ICH_PCIE_LSTS_LT                           BIT11
#define   B_ICH_PCIE_LSTS_LTE                          BIT10
#define   B_ICH_PCIE_LSTS_NLW                          0x03F0
#define     V_ICH_PCIE_LSTS_NLW_1                        0x0010
#define     V_ICH_PCIE_LSTS_NLW_2                        0x0020
#define     V_ICH_PCIE_LSTS_NLW_4                        0x0040
#define   B_ICH_PCIE_LSTS_LS                           0x000F
#define R_ICH_PCIE_SLCAP                            0x54
#define  S_ICH_PCIE_SLCAP                            4
#define   B_ICH_PCIE_SLCAP_PSN                         0xFFF80000
#define   B_ICH_PCIE_SLCAP_SLS                         0x00018000
#define   B_ICH_PCIE_SLCAP_SLV                         0x00007F80
#define   B_ICH_PCIE_SLCAP_HPC                         BIT6
#define   B_ICH_PCIE_SLCAP_HPS                         BIT5
#define   B_ICH_PCIE_SLCAP_PIP                         BIT4
#define   B_ICH_PCIE_SLCAP_AIP                         BIT3
#define   B_ICH_PCIE_SLCAP_MSP                         BIT2
#define   B_ICH_PCIE_SLCAP_PCP                         BIT1
#define   B_ICH_PCIE_SLCAP_ABP                         BIT0
#define R_ICH_PCIE_SLCTL                            0x58
#define  S_ICH_PCIE_SLCTL                            2
#define   B_ICH_PCIE_SLCTL_LACE                        BIT12
#define   B_ICH_PCIE_SLCTL_PCC                         BIT10
#define   B_ICH_PCIE_SLCTL_PIC                         (BIT9 | BIT8)
#define   B_ICH_PCIE_SLCTL_AIC                         (BIT7 | BIT6)
#define   B_ICH_PCIE_SLCTL_HPE                         BIT5
#define   B_ICH_PCIE_SLCTL_CCE                         BIT4
#define   B_ICH_PCIE_SLCTL_PDE                         BIT3
#define   B_ICH_PCIE_SLCTL_MSE                         BIT2
#define   B_ICH_PCIE_SLCTL_PFE                         BIT1
#define   B_ICH_PCIE_SLCTL_ABE                         BIT0
#define R_ICH_PCIE_SLSTS                            0x5A
#define  S_ICH_PCIE_SLSTS                            2
#define   B_ICH_PCIE_SLSTS_LASC                        BIT8
#define   B_ICH_PCIE_SLSTS_PDS                         BIT6
#define   B_ICH_PCIE_SLSTS_CC                          BIT4
#define   B_ICH_PCIE_SLSTS_PDC                         BIT3
#define   B_ICH_PCIE_SLSTS_ABP                         BIT0
#define R_ICH_PCIE_RCTL                             0x5C
#define  S_ICH_PCIE_RCTL                             2
#define   B_ICH_PCIE_RCTL_PME                          BIT3
#define   B_ICH_PCIE_RCTL_SFE                          BIT2
#define   B_ICH_PCIE_RCTL_SNE                          BIT1
#define   B_ICH_PCIE_RCTL_SCE                          BIT0
#define R_ICH_PCIE_RSTS                             0x60
#define  S_ICH_PCIE_RSTS                             4
#define   B_ICH_PCIE_RSTS_PP                           BIT17
#define   B_ICH_PCIE_RSTS_PS                           BIT16
#define   B_ICH_PCIE_RSTS_RID                          0x0000FFFF
#define R_ICH_PCIE_MID                              0x80
#define  S_ICH_PCIE_MID                              2
#define   B_ICH_PCIE_MID_NEXT                          0xFF00
#define   B_ICH_PCIE_MID_CID                           0x00FF
#define R_ICH_PCIE_MC                               0x82
#define  S_ICH_PCIE_MC                               2
#define   B_ICH_PCIE_MC_C64                            BIT7
#define   B_ICH_PCIE_MC_MME                            (BIT6 | BIT5 | BIT4)
#define   B_ICH_PCIE_MC_MMC                            0x000E
#define   B_ICH_PCIE_MC_MSIE                           BIT0
#define R_ICH_PCIE_MA                               0x84
#define  S_ICH_PCIE_MA                               4
#define   B_ICH_PCIE_MA_ADDR                           0xFFFFFFFC
#define R_ICH_PCIE_MD                               0x88
#define  S_ICH_PCIE_MD                               2
#define   B_ICH_PCIE_MD_DATA                           0xFFFF
#define R_ICH_PCIE_SVCAP                            0x90
#define  S_ICH_PCIE_SVCAP                            2
#define   B_ICH_PCIE_SVCAP_NEXT                        0xFF00
#define   B_ICH_PCIE_SVCAP_CID                         0x00FF
#define R_ICH_PCIE_SVID                             0x94
#define  S_ICH_PCIE_SVID                             4
#define   B_ICH_PCIE_SVID_SID                          0xFFFF0000
#define   B_ICH_PCIE_SVID_SVID                         0x0000FFFF
#define R_ICH_PCIE_PMCAP                            0xA0
#define  S_ICH_PCIE_PMCAP                            2
#define   B_ICH_PCIE_PMCAP_NEXT                        0xFF00
#define   B_ICH_PCIE_PMCAP_CID                         0x00FF
#define R_ICH_PCIE_PMC                              0xA2
#define  S_ICH_PCIE_PMC                              2
#define   B_ICH_PCIE_PMC_PMES                          0xF800
#define   B_ICH_PCIE_PMC_AC                            0x01C0
#define   B_ICH_PCIE_PMC_DSI                           BIT5
#define   B_ICH_PCIE_PMC_PMEC                          BIT3
#define   B_ICH_PCIE_PMC_VS                            0x0007
#define R_ICH_PCIE_PMCS                             0xA4
#define  S_ICH_PCIE_PMCS                             4
#define   B_ICH_PCIE_PMCS_BPCE                         BIT23
#define   B_ICH_PCIE_PMCS_B23S                         BIT22
#define   B_ICH_PCIE_PMCS_PMES                         BIT15
#define   B_ICH_PCIE_PMCS_PMEE                         BIT8
#define   B_ICH_PCIE_PMCS_PS                           (BIT1 | BIT0)
#define    V_ICH_PCIE_PMCS_D0                            0x00
#define    V_ICH_PCIE_PMCS_D3                            0x03
#define R_ICH_PCIE_MPC2                             0xD4
#define  S_ICH_PCIE_MPC2                             4
#define   B_ICH_PCIE_MPC2_EOIFD                        BIT1
#define   B_ICH_PCIE_MPC2_L1CTM                        BIT0
#define R_ICH_PCIE_MPC                              0xD8
#define  S_ICH_PCIE_MPC                              4
#define   B_ICH_PCIE_MPC_PMCE                          BIT31
#define   B_ICH_PCIE_MPC_HPCE                          BIT30
#define   B_ICH_PCIE_MPC_LHO                           BIT29
#define   B_ICH_PCIE_MPC_ATE                           BIT28
#define   B_ICH_PCIE_MPC_LR                            BIT27
#define   B_ICH_PCIE_MPC_IRBNCE                        BIT26
#define   B_ICH_PCIE_MPC_IRRCE                         BIT25
#define   B_ICH_PCIE_MPC_BMERCE                        BIT24
#define   B_ICH_PCIE_MPC_FORCEDET                      BIT22
#define   B_ICH_PCIE_MPC_FCDL1E                        BIT21
#define   B_ICH_PCIE_MPC_UCEL                          (BIT20 | BIT19 | BIT18)
#define   B_ICH_PCIE_MPC_CCEL                          (BIT17 | BIT16 | BIT15)
#define   B_ICH_PCIE_MPC_PAE                           BIT7
#define   B_ICH_PCIE_MPC_BT                            BIT2
#define   B_ICH_PCIE_MPC_HPME                          BIT1
#define   B_ICH_PCIE_MPC_PMME                          BIT0
#define R_ICH_PCIE_SMSCS                            0xDC
#define  S_ICH_PCIE_SMSCS                            4
#define   B_ICH_PCIE_SMSCS_PMCS                        BIT31
#define   B_ICH_PCIE_SMSCS_HPCS                        BIT30
#define   B_ICH_PCIE_SMSCS_HPLAS                       BIT4
#define   B_ICH_PCIE_SMSCS_HPCCM                       BIT3
#define   B_ICH_PCIE_SMSCS_HPABM                       BIT2
#define   B_ICH_PCIE_SMSCS_HPPDM                       BIT1
#define   B_ICH_PCIE_SMSCS_PMMS                        BIT0
#define R_ICH_PCIE_RPDCGEN                          0xE1
#define   B_ICH_PCIE_RPDCGEN_SRDLC                     BIT3
#define   B_ICH_PCIE_RPDCGEN_SRDBC                     BIT2
#define   B_ICH_PCIE_RPDCGEN_RPDLC                     BIT1
#define   B_ICH_PCIE_RPDCGEN_RPDBC                     BIT0
#define R_ICH_PCIE_IPWS                             0xE2
#define  S_ICH_PCIE_IPWS                             2
#define   B_ICH_PCIE_IPWS_IPWSTAT                      BIT15
#define R_ICH_PCIE_PECR1                            0xE8
#define   B_ICH_PCIE_PECR1_FIELD_2                     BIT1
#define   B_ICH_PCIE_PECR1_FIELD_1                     BIT0
#define R_ICH_PCIE_VCH                              0x100
#define   B_ICH_PCIE_VCH_NCO                           0xFFF00000
#define   B_ICH_PCIE_VCH_CV                            0x000F0000
#define   B_ICH_PCIE_VCH_CID                           0x0000FFFF
#define R_ICH_PCIE_VCAP2                            0x108
#define   B_ICH_PCIE_VCAP2_ATO                         0xFF000000
#define R_ICH_PCIE_PVC                              0x10C
#define  S_ICH_PCIE_PVC                              2
#define   B_ICH_PCIE_PVC_AS                            (BIT3 | BIT2 | BIT1)
#define   B_ICH_PCIE_PVC_LAT                           BIT0
#define R_ICH_PCIE_PVS                              0x10E
#define  S_ICH_PCIE_PVS                              2
#define   B_ICH_PCIE_PVS_VAS                           BIT0
#define R_ICH_PCIE_V0CAP                            0x110
#define  S_ICH_PCIE_V0CAP                            4
#define   B_ICH_PCIE_V0CAP_AT                          0xFF000000
#define   B_ICH_PCIE_V0CAP_MTS                         0x007F0000
#define   B_ICH_PCIE_V0CAP_RTS                         BIT15
#define   B_ICH_PCIE_V0CAP_APS                         BIT14
#define   B_ICH_PCIE_V0CAP_PAC                         0x000000FF
#define R_ICH_PCIE_V1CAP                            0x114
#define  S_ICH_PCIE_V1CAP                            4
#define   B_ICH_PCIE_V1CAP_EN                          BIT31
#define   B_ICH_PCIE_V1CAP_VCID                        0x07000000
#define   B_ICH_PCIE_V1CAP_PAS                         0x000E0000
#define   B_ICH_PCIE_V1CAP_LAT                         BIT16
#define   B_ICH_PCIE_V1CAP_TVM                         0x000000FF
#define     V_ICH_PCIE_V1CAP_TVM_TC7                      BIT7
#define     V_ICH_PCIE_V1CAP_TVM_TC6                      BIT6
#define     V_ICH_PCIE_V1CAP_TVM_TC5                      BIT5
#define     V_ICH_PCIE_V1CAP_TVM_TC4                      BIT4
#define     V_ICH_PCIE_V1CAP_TVM_TC3                      BIT3
#define     V_ICH_PCIE_V1CAP_TVM_TC2                      BIT2
#define     V_ICH_PCIE_V1CAP_TVM_TC1                      BIT1
#define     V_ICH_PCIE_V1CAP_TVM_TC0                      BIT0
#define R_ICH_PCIE_V0STS                            0x11A
#define  S_ICH_PCIE_V0STS                            2
#define   B_ICH_PCIE_V0STS_NP                          BIT1
#define   B_ICH_PCIE_V0STS_ATS                         BIT0
#define R_ICH_PCIE_UES                              0x144
#define  S_ICH_PCIE_UES                              4
#define   B_ICH_PCIE_UES_URE                           BIT20
#define   B_ICH_PCIE_UES_EE                            BIT19
#define   B_ICH_PCIE_UES_MT                            BIT18
#define   B_ICH_PCIE_UES_RO                            BIT17
#define   B_ICH_PCIE_UES_UC                            BIT16
#define   B_ICH_PCIE_UES_CA                            BIT15
#define   B_ICH_PCIE_UES_CT                            BIT14
#define   B_ICH_PCIE_UES_FCPE                          BIT13
#define   B_ICH_PCIE_UES_PT                            BIT12
#define   B_ICH_PCIE_UES_DLPE                          BIT4
#define   B_ICH_PCIE_UES_TE                            BIT0
#define R_ICH_PCIE_UEM                              0x148
#define  S_ICH_PCIE_UEM                              4
#define   B_ICH_PCIE_UEM_URE                           BIT20
#define   B_ICH_PCIE_UEM_EE                            BIT19
#define   B_ICH_PCIE_UEM_MT                            BIT18
#define   B_ICH_PCIE_UEM_RO                            BIT17
#define   B_ICH_PCIE_UEM_UC                            BIT16
#define   B_ICH_PCIE_UEM_CA                            BIT15
#define   B_ICH_PCIE_UEM_CT                            BIT14
#define   B_ICH_PCIE_UEM_FCPE                          BIT13
#define   B_ICH_PCIE_UEM_PT                            BIT12
#define   B_ICH_PCIE_UEM_DLPE                          BIT4
#define   B_ICH_PCIE_UEM_TE                            BIT0
#define R_ICH_PCIE_UEV                              0x14C
#define  S_ICH_PCIE_UEV                              4
#define   B_ICH_PCIE_UEV_URE                           BIT20
#define   B_ICH_PCIE_UEV_EE                            BIT19
#define   B_ICH_PCIE_UEV_MT                            BIT18
#define   B_ICH_PCIE_UEV_RO                            BIT17
#define   B_ICH_PCIE_UEV_UC                            BIT16
#define   B_ICH_PCIE_UEV_CA                            BIT15
#define   B_ICH_PCIE_UEV_CT                            BIT14
#define   B_ICH_PCIE_UEV_FCPE                          BIT13
#define   B_ICH_PCIE_UEV_PT                            BIT12
#define   B_ICH_PCIE_UEV_DLPE                          BIT4
#define   B_ICH_PCIE_UEV_TE                            BIT0
#define R_ICH_PCIE_CES                              0x150
#define  S_ICH_PCIE_CES                              4
#define   B_ICH_PCIE_CES_ANFES                         BIT13
#define   B_ICH_PCIE_CES_RTT                           BIT12
#define   B_ICH_PCIE_CES_RNR                           BIT8
#define   B_ICH_PCIE_CES_BD                            BIT7
#define   B_ICH_PCIE_CES_BT                            BIT6
#define   B_ICH_PCIE_CES_RE                            BIT0
#define R_ICH_PCIE_CEM                              0x154
#define  S_ICH_PCIE_CEM                              4
#define   B_ICH_PCIE_CEM_ANFEM                         BIT13
#define   B_ICH_PCIE_CEM_RTT                           BIT12
#define   B_ICH_PCIE_CEM_RNR                           BIT8
#define   B_ICH_PCIE_CEM_BD                            BIT7
#define   B_ICH_PCIE_CEM_BT                            BIT6
#define   B_ICH_PCIE_CEM_RE                            BIT0
#define R_ICH_PCIE_AECC                             0x158
#define  S_ICH_PCIE_AECC                             4
#define   B_ICH_PCIE_AECC_ECE                          BIT8
#define   B_ICH_PCIE_AECC_ECC                          BIT7
#define   B_ICH_PCIE_AECC_EGE                          BIT6
#define   B_ICH_PCIE_AECC_EGC                          BIT5
#define   B_ICH_PCIE_AECC_FEP                          0x0000001F
#define R_ICH_PCIE_RES                              0x170
#define  S_ICH_PCIE_RES                              4
#define   B_ICH_PCIE_RES_AEMN                          0xF8000000
#define   B_ICH_PCIE_RES_MENR                          BIT3
#define   B_ICH_PCIE_RES_ENR                           BIT2
#define   B_ICH_PCIE_RES_MCR                           BIT1
#define   B_ICH_PCIE_RES_CR                            BIT0
#define R_ICH_PCIE_RCTCL                            0x180
#define  S_ICH_PCIE_RCTCL                            4
#define   B_ICH_PCIE_RCTCL_NEXT                        0xFFF00000
#define   B_ICH_PCIE_RCTCL_CV                          0x000F0000
#define   B_ICH_PCIE_RCTCL_CID                         0x0000FFFF
#define R_ICH_PCIE_ESD                              0x184
#define  S_ICH_PCIE_ESD                              4
#define   B_ICH_PCIE_ESD_PN                            0xFF000000
#define     V_ICH_PCIE_ESD_PN1                           0x01000000
#define     V_ICH_PCIE_ESD_PN2                           0x02000000
#define     V_ICH_PCIE_ESD_PN3                           0x03000000
#define     V_ICH_PCIE_ESD_PN4                           0x04000000
#define     V_ICH_PCIE_ESD_PN5                           0x05000000
#define     V_ICH_PCIE_ESD_PN6                           0x06000000
#define   B_ICH_PCIE_ESD_CID                           0x00FF0000
#define   B_ICH_PCIE_ESD_NLE                           0x0000FF00
#define   B_ICH_PCIE_ESD_ET                            0x0000000F
#define R_ICH_PCIE_ULD                              0x190
#define  S_ICH_PCIE_ULD                              4
#define   B_ICH_PCIE_ULD_PN                            0xFF000000
#define   B_ICH_PCIE_ULD_TCID                          0x00FF0000
#define   B_ICH_PCIE_ULD_LT                            BIT1
#define   B_ICH_PCIE_ULD_LV                            BIT0
#define R_ICH_PCIE_ULBA                             0x198
#define  S_ICH_PCIE_ULBA                             8
#define   B_ICH_PCIE_ULBA_BAU                          0xFFFFFFFF00000000
#define   B_ICH_PCIE_ULBA_BAL                          0x00000000FFFFFFFF
#define R_ICH_PCIE_PECR2                            0x300
#define   B_ICH_PCIE_PECR2_FIELD_1                     BIT21
#define R_ICH_PCIE_PEETM                            0x318
#define  S_ICH_PCIE_PEETM                            1
#define   B_ICH_PCIE_PEETM_BAU                         BIT2

//
// Misc PCI register offsets and sizes
//
#define R_EFI_PCI_SVID                              0x2C
#define   V_EFI_PCI_SVID_SIZE                         0x2
#define R_EFI_PCI_SID                               0x2E
#define   V_EFI_PCI_SID_SIZE                          0x2
#define R_ICH_USB_DRIVE_STRENGTH                    0xD8

#define IO_APIC_INDEX_REGISTER                      0xFEC00000
#define IO_APIC_DATA_REGISTER                       0xFEC00010

#define IO_APIC_BOOT_CONFIG_INDEX                   0x03
#define IO_APIC_USE_SYSTEM_BUS                      0x01


#define R_ICH_IO_APIC_VER                           0x01
#define   N_ICH_IO_APIC_MRE                         BIT16 // 0x16 ESS_OVERRIDE
#define   B_ICH_IO_APIC_MRE_MASK                    (0xff << N_ICH_IO_APIC_MRE)
#define   V_ICH_IO_APIC_MRE                         0x17

#define R_APM_CNT                                   0xB2
#define R_APM_STS                                   0xB3

//
// Chipset configuration registers (Memory space)
// RCBA
//
#define R_ICH_RCRB_VCH                              0x0000
#define   B_ICH_RCRB_VCH_NCO                          0xFFF00000
#define   B_ICH_RCRB_VCH_CV                           0x000F0000
#define   B_ICH_RCRB_VCH_CID                          0x0000FFFF
#define R_ICH_RCRB_VCAP1                            0x0004
#define   B_ICH_RCRB_VCAP1_PATS                       (BIT11 | BIT10)
#define   B_ICH_RCRB_VCAP1_RC                         (BIT9 | BIT8)
#define   B_ICH_RCRB_VCAP1_LPEVC                      (BIT6 | BIT5 | BIT4)
#define   B_ICH_RCRB_VCAP1_EVC                        (BIT2 | BIT1 | BIT0)
#define R_ICH_RCRB_VCAP2                            0x0008
#define   B_ICH_RCRB_VCAP2_ATO                        0xFF000000
#define   B_ICH_RCRB_VCAP2_AC                         0x000000FF
#define R_ICH_RCRB_PVC                              0x000C
#define   B_ICH_RCRB_PVC_AS                           (BIT3 | BIT2 | BIT1)
#define   B_ICH_RCRB_PVC_LAT                          BIT0
#define R_ICH_RCRB_PVS                              0x000E
#define   B_ICH_RCRB_PVS_VAS                          BIT0
#define R_ICH_RCRB_V0CAP                            0x0010   // Virtual channel 0 resource capability
#define   B_ICH_RCRB_V0CAP_AT                         0xFF000000
#define   B_ICH_RCRB_V0CAP_MTS                        0x007F0000
#define   B_ICH_RCRB_V0CAP_RTS                        BIT15
#define   B_ICH_RCRB_V0CAP_APS                        BIT14
#define   B_ICH_RCRB_V0CAP_PAC                        0x000000FF
#define R_ICH_RCRB_V0CTL                            0x0014   // Virtual channel 0 resource control
#define   B_ICH_RCRB_V0CTL_EN                             BIT31
#define   V_ICH_RCRB_V0CTL_ID_MASK                        (7 << 24)
#define   B_ICH_RCRB_V0CTL_PAS                            (BIT19 | BIT18 | BIT17)
#define   B_ICH_RCRB_V0CTL_LAT                            BIT16
#define   V_ICH_RCRB_V0CTL_TVM_MASK                       0xFE
#define R_ICH_RCRB_V0STS                            0x001A   // Virtual channel 0 status
#define   B_ICH_RCRB_V0STS_NP                         BIT1
#define   B_ICH_RCRB_V0STS_ATS                        BIT0
#define R_ICH_RCRB_V1CAP                            0x001C   // Virtual channel 1 resource capability
#define   B_ICH_RCRB_V1CAP_AT                         0xFF000000
#define   B_ICH_RCRB_V1CAP_MTS                        0x007F0000
#define   B_ICH_RCRB_V1CAP_RTS                        BIT15
#define   B_ICH_RCRB_V1CAP_APS                        BIT14
#define   B_ICH_RCRB_V1CAP_PAC                        0x000000FF
#define R_ICH_RCRB_V1CTL                            0x0020   // Virtual channel 1 resource control
#define   B_ICH_RCRB_V1CTL_EN                         BIT31
#define   V_ICH_RCRB_V1CTL_ID_MASK                    (7 << 24)
#define   B_ICH_RCRB_V1CTL_PAS                        (BIT19 | BIT18 | BIT17)
#define   B_ICH_RCRB_V1CTL_LAT                        BIT16
#define   V_ICH_RCRB_V1CTL_TVM_MASK                   0xFE
#define R_ICH_RCRB_V1STS                            0x0026   // Virtual channel 1 status
#define   B_ICH_RCRB_V1STS_NP                             BIT1
#define   B_ICH_RCRB_V1STS_ATS                            BIT0
#define R_ICH_RCRB_V1PAT                            0x0030   // Virtual channel 1 port arbitration table
//
// ESS_OVERRIDE_START
//
#define R_ICH_RCRB_AECH                             0x80      // Advanced Error Reporting Capabilities header
#define R_ICH_RCRB_UES                              0x84      // Uncorrectable Error Status
#define R_ICH_RCRB_UEM                              0x88      // Uncorrectable Error Mask
#define R_ICH_RCRB_UEV                              0x8c      // Uncorrectable Error Severity
#define   B_ICH_RCRB_UE_TE                            BIT0    // Training Error
#define   B_ICH_RCRB_UE_DLPE                          BIT4    // Data Link Protocol Error
#define   B_ICH_RCRB_UE_PT                            BIT12   // Poisoned TLP
#define   B_ICH_RCRB_UE_FCPE                          BIT13   // Flow Control Protocol Error
#define   B_ICH_RCRB_UE_CT                            BIT14   // Completion Timeout
#define   B_ICH_RCRB_UE_CA                            BIT15   // Completer Abort
#define   B_ICH_RCRB_UE_UC                            BIT16   // Unexpected Completion
#define   B_ICH_RCRB_UE_RO                            BIT17   // Receiver Overflow
#define   B_ICH_RCRB_UE_MT                            BIT18   // Malformed TLP
#define   B_ICH_RCRB_UE_EE                            BIT19   // ECRC Error
#define   B_ICH_RCRB_UE_URE                           BIT20   // Unsupported Request Error
#define R_ICH_RCRB_CES                              0x90      // Correctable Error Status
#define R_ICH_RCRB_CEM                              0x94      // Correctable Error Mask
#define   B_ICH_RCRB_CE_RE                            BIT0    // Indicates a receiver error
#define   B_ICH_RCRB_CE_BT                            BIT6    // Bad TLP
#define   B_ICH_RCRB_CE_BD                            BIT7    // Bad DLLP
#define   B_ICH_RCRB_CE_RNR                           BIT8    // Replay Number Rollover
//
// ESS_OVERRIDE_END
//
#define R_ICH_RCRB_CIR1                             0x0088
#define   B_ICH_RCRB_CIR1_FIELD_3_MASK                BIT20
#define   B_ICH_RCRB_CIR1_FIELD_2_MASK                BIT15
#define   B_ICH_RCRB_CIR1_FIELD_1_MASK                BIT12
#define R_ICH_RCRB_REC                              0x00AC
#define   B_ICH_RCRB_REC_CERE                         BIT0    // ESS_OVERRIDE
#define   B_ICH_RCRB_REC_NERE                         BIT1    // ESS_OVERRIDE
#define   B_ICH_RCRB_REC_FERE                         BIT2    // ESS_OVERRIDE
#define   B_ICH_RCRB_REC_DPDP                         BIT31
#define R_ICH_RCRB_RES                              0x00B0    // ESS_OVERRIDE
#define   B_ICH_RCRB_RES_CR                         BIT0      // ESS_OVERRIDE
#define   B_ICH_RCRB_RES_ENR                        BIT2      // ESS_OVERRIDE
#define R_ICH_RCRB_RCTCL                            0x0100
#define   B_ICH_RCRB_RCTCL_NEXT                       0xFFF00000
#define   B_ICH_RCRB_RCTCL_CV                         0x000F0000
#define   B_ICH_RCRB_RCTCL_CID                        0x0000FFFF
#define R_ICH_RCRB_ESD                              0x0104   // Element Self Description
#define   B_ICH_RCRB_ESD_PN                           0xFF000000
#define   B_ICH_RCRB_ESD_CID                          0x00FF0000
#define   B_ICH_RCRB_ESD_NLE                          0x0000FF00
#define   B_ICH_RCRB_ESD_ET                           0x0000000F
#define R_ICH_RCRB_ULD                              0x0110   // Upstream Link Description
#define   B_ICH_RCRB_ULD_PN                           0xFF000000
#define   B_ICH_RCRB_ULD_TCID                         0x00FF0000
#define   B_ICH_RCRB_ULD_LT                           BIT1
#define   B_ICH_RCRB_ULD_LV                           BIT0
#define R_ICH_RCRB_ULBA                             0x0118   // Upstream Link Base Address
#define   S_ICH_RCRB_ULBA                             8
#define   B_ICH_RCRB_ULBA_BAU                         0xFFFFFFFF00000000
#define   B_ICH_RCRB_ULBA_BAL                         0x00000000FFFFFFFF
#define R_ICH_RCRB_RP1D                             0x0120
#define   B_ICH_RCRB_RP1D_PN                          0xFF000000
#define   B_ICH_RCRB_RP1D_TCID                        0x00FF0000
#define   B_ICH_RCRB_RP1D_LT                          BIT1
#define   B_ICH_RCRB_RP1D_LV                          BIT0
#define R_ICH_RCRB_RP1BA                            0x0128
#define   B_ICH_RCRB_RP1BA_BN                         0x000000000FF00000
#define   B_ICH_RCRB_RP1BA_DN                         0x00000000000F8000
#define   B_ICH_RCRB_RP1BA_FN                         0x0000000000007000
#define R_ICH_RCRB_RP2D                             0x0130
#define   B_ICH_RCRB_RP2D_PN                          0xFF000000
#define   B_ICH_RCRB_RP2D_TCID                        0x00FF0000
#define   B_ICH_RCRB_RP2D_LT                          BIT1
#define   B_ICH_RCRB_RP2D_LV                          BIT0
#define R_ICH_RCRB_RP2BA                            0x0138
#define   B_ICH_RCRB_RP2BA_BN                         0x000000000FF00000
#define   B_ICH_RCRB_RP2BA_DN                         0x00000000000F8000
#define   B_ICH_RCRB_RP2BA_FN                         0x0000000000007000
#define R_ICH_RCRB_RP3D                             0x0140
#define   B_ICH_RCRB_RP3D_PN                          0xFF000000
#define   B_ICH_RCRB_RP3D_TCID                        0x00FF0000
#define   B_ICH_RCRB_RP3D_LT                          BIT1
#define   B_ICH_RCRB_RP3D_LV                          BIT0
#define R_ICH_RCRB_RP3BA                            0x0148
#define   B_ICH_RCRB_RP3BA_BN                         0x000000000FF00000
#define   B_ICH_RCRB_RP3BA_DN                         0x00000000000F8000
#define   B_ICH_RCRB_RP3BA_FN                         0x0000000000007000
#define R_ICH_RCRB_RP4D                             0x0150
#define   B_ICH_RCRB_RP4D_PN                          0xFF000000
#define   B_ICH_RCRB_RP4D_TCID                        0x00FF0000
#define   B_ICH_RCRB_RP4D_LT                          BIT1
#define   B_ICH_RCRB_RP4D_LV                          BIT0
#define R_ICH_RCRB_RP4BA                            0x0158
#define   B_ICH_RCRB_RP4BA_BN                         0x000000000FF00000
#define   B_ICH_RCRB_RP4BA_DN                         0x00000000000F8000
#define   B_ICH_RCRB_RP4BA_FN                         0x0000000000007000
#define R_ICH_RCRB_HDD                              0x0160
#define   B_ICH_RCRB_HDD_PN                           0xFF000000
#define   B_ICH_RCRB_HDD_TCID                         0x00FF0000
#define   B_ICH_RCRB_HDD_LT                           BIT1
#define   B_ICH_RCRB_HDD_LV                           BIT0
#define R_ICH_RCRB_HDBA                             0x0168
#define   B_ICH_RCRB_HDBA_BN                          0x000000000FF00000
#define   B_ICH_RCRB_HDBA_DN                          0x00000000000F8000
#define   B_ICH_RCRB_HDBA_FN                          0x0000000000007000
#define R_ICH_RCRB_RP5D                             0x0170
#define   B_ICH_RCRB_RP5D_PN                          0xFF000000
#define   B_ICH_RCRB_RP5D_TCID                        0x00FF0000
#define   B_ICH_RCRB_RP5D_LT                          BIT1
#define   B_ICH_RCRB_RP5D_LV                          BIT0
#define R_ICH_RCRB_RP5BA                            0x0178
#define   B_ICH_RCRB_RP5BA_BN                         0x000000000FF00000
#define   B_ICH_RCRB_RP5BA_DN                         0x00000000000F8000
#define   B_ICH_RCRB_RP5BA_FN                         0x0000000000007000
#define R_ICH_RCRB_RP6D                             0x0180
#define   B_ICH_RCRB_RP6D_PN                          0xFF000000
#define   B_ICH_RCRB_RP6D_TCID                        0x00FF0000
#define   B_ICH_RCRB_RP6D_LT                          BIT1
#define   B_ICH_RCRB_RP6D_LV                          BIT0
#define R_ICH_RCRB_RP6BA                            0x0188
#define   B_ICH_RCRB_RP6BA_BN                         0x000000000FF00000
#define   B_ICH_RCRB_RP6BA_DN                         0x00000000000F8000
#define   B_ICH_RCRB_RP6BA_FN                         0x0000000000007000
#define R_ICH_RCRB_ILCL                             0x01A0
#define   B_ICH_RCRB_ILCL_NEXT                        0xFFF00000
#define   B_ICH_RCRB_ILCL_CV                          0x000F0000
#define   B_ICH_RCRB_ILCL_CID                         0x0000FFFF
#define R_ICH_RCRB_LCAP                             0x01A4   // Link capabilities
#define   B_ICH_RCRB_LCAP_EL1                         (BIT17 | BIT16 | BIT15)
#define   B_ICH_RCRB_LCAP_EL0                         (BIT14 | BIT13 | BIT12)
#define   B_ICH_RCRB_LCAP_APMS                        (BIT11 | BIT10)   // L0 is supported on DMI
#define   B_ICH_RCRB_LCAP_APMS                            (BIT11 | BIT10)   // L0 is supported on DMI
#define   B_ICH_RCRB_LCAP_MLW                         0x000003F0
#define   B_ICH_RCRB_LCAP_MLS                         0x0000000F
#define R_ICH_RCRB_LCTL                             0x01A8   // Link control
#define   B_ICH_RCRB_LCTL_ES                          BIT7
#define   B_ICH_RCRB_LCTL_APMC                        (BIT1 | BIT0)
#define    V_ICH_RCRB_LCTL_APMC_DIS                   0x00
#define    V_ICH_RCRB_LCTL_APMC_EN                    0x01
#define R_ICH_RCRB_LSTS                             0x01AA
#define   B_ICH_RCRB_LSTS_NLW                         0x03F0
#define   B_ICH_RCRB_LSTS_LS                          0x000F
#define R_ICH_RCRB_CIR2                             0x01F4
#define   B_ICH_RCRB_CIR2_FIELD_1_MASK                0xFFFFFFFF
#define R_ICH_RCRB_CIR_1FA                          0x01FA // ESS_OVERRIDE
#define R_ICH_RCRB_CIR3                             0x01FC
#define   B_ICH_RCRB_CIR3_FIELD_3_MASK                (BIT10 | BIT9 | BIT8)
#define   B_ICH_RCRB_CIR3_FIELD_2_MASK                BIT3
#define   B_ICH_RCRB_CIR3_FIELD_1_MASK                (BIT1 | BIT0)
#define R_ICH_RCRB_VPCTL                            0x01F4  // Private Virtual Channel Resouce Control
#define   B_ICH_RCRB_VPCTL_EN                         BIT31 // Virtual Channel Enable
#define   B_ICH_RCRB_VPCTL_LAT                        BIT16 // Load Port Arbitration Table
#define R_ICH_RCRB_VPR                              0x01FC  // Private Virtual Channel Routing
#define   B_ICH_RCRB_VPR_ZPE                          BIT3  // HDA VCp Enable
#define   B_ICH_RCRB_VPR_EPE                          BIT1  // EHCI VCp Enable
#define   B_ICH_RCRB_VPR_UPE                          BIT0  // UHCI VCp Enable
#define R_ICH_RCRB_L3A1                             0x0200  // Level 3 Backbone Arbiter Configuration
#define R_ICH_RCRB_UMR                              0x0218  // Upstream Minimum Reserved
#define R_ICH_RCRB_BCR                              0x0220  // Backbone Configuration Register
#define   B_ICH_RCRB_BCR_FIELD_2_MASK                 BIT6
#define   B_ICH_RCRB_BCR_FIELD_1_MASK                 (BIT2 | BIT1 | BIT0)
#define R_ICH_RCRB_RPC                              0x0224   // Root port configuration
#define   B_ICH_RCRB_RPC_HPE                          BIT7
#define   B_ICH_RCRB_RPC_HPP                          0x00000070
#define   B_ICH_RCRB_RPC_PC2                          BIT2
#define   B_ICH_RCRB_RPC_PC                           (BIT1 | BIT0)
#define R_ICH_RCRB_BAC                              0x0228  // Bandwidth allocation configuration
#define R_ICH_RCRB_DMI_CNTL                         0x0234   // DMI control register
#define   B_ICH_RCRB_DMI_CNTL_DMILCGEN                BIT1  // DMI Link Dynamic Clock Gate Enable
#define   B_ICH_RCRB_DMI_CNTL_DMIBCGEN                BIT0  // DMI Backbone Dynamic Clock Gate Enable

#define R_ICH_RCRB_RPFN                             0x0238   // Root Port Function Number & Hide for PCI Express Root Ports
#define   B_ICH_RCRB_RPFN_RP6CH                       BIT23 // Root Port 6 Hide
#define   B_ICH_RCRB_RPFN_RP6FN                       (BIT22 | BIT21 | BIT20) // Root Port 6 Function Number
#define   B_ICH_RCRB_RPFN_RP5CH                       BIT19 // Root Port 5 Hide
#define   B_ICH_RCRB_RPFN_RP5FN                       (BIT18 | BIT17 | BIT16) // Root Port 5 Function Number
#define   B_ICH_RCRB_RPFN_RP4CH                       BIT15 // Root Port 4 Hide
#define   B_ICH_RCRB_RPFN_RP4FN                       (BIT14 | BIT13 | BIT12) // Root Port 4 Function Number
#define   B_ICH_RCRB_RPFN_RP3CH                       BIT11 // Root Port 3 Hide
#define   B_ICH_RCRB_RPFN_RP3FN                       (BIT10 | BIT9 | BIT8) // Root Port 3 Function Number
#define   B_ICH_RCRB_RPFN_RP2CH                       BIT7  // Root Port 2 Hide
#define   B_ICH_RCRB_RPFN_RP2FN                       (BIT6 | BIT5 | BIT4) // Root Port 2 Function Number
#define   B_ICH_RCRB_RPFN_RP1CH                       BIT3  // Root Port 1 Hide
#define   B_ICH_RCRB_RPFN_RP1FN                       (BIT2 | BIT1 | BIT0) // Root Port 1 Function Number

#define R_ICH_RCRB_FLRSTAT                          0x0290   // FLR Pending Status Register
#define   B_ICH_RCRB_FLRSTAT_D29F7                    BIT23
#define   B_ICH_RCRB_FLRSTAT_D29F3                    BIT19
#define   B_ICH_RCRB_FLRSTAT_D29F2                    BIT18
#define   B_ICH_RCRB_FLRSTAT_D29F1                    BIT17
#define   B_ICH_RCRB_FLRSTAT_D29F0                    BIT16
#define   B_ICH_RCRB_FLRSTAT_D26F7                    BIT15
#define   B_ICH_RCRB_FLRSTAT_D26F2                    BIT10
#define   B_ICH_RCRB_FLRSTAT_D26F1                    BIT9
#define   B_ICH_RCRB_FLRSTAT_D26F0                    BIT8
#define R_ICH_RCRB_CIR13                            0x0F20
#define   B_ICH_RCRB_CIR13_FIELD_1_MASK               (BIT19 | BIT18 | BIT17 | BIT16)
#define R_ICH_RCRB_CIR5                             0x1D40
#define   V_ICH_RCRB_CIR5_FLD1                        0x0000000000000001

#define R_ICH_RCRB_TRSR                             0x1E00   // Trap Status Register
#define   B_ICH_RCRB_TRSR_CTSS                        0x000F   // Cycle Trap SMI# Status mask
#define R_ICH_RCRB_TRCR                             0x1E10   // Trapped Cycle Register
#define   S_ICH_RCRB_TRCR                             8
#define   B_ICH_RCRB_TRCR_RWI                         BIT24
#define   B_ICH_RCRB_TRCR_AHBE                        0x00000000000F0000
#define   B_ICH_RCRB_TRCR_TIOA                        0x000000000000FFFC
#define R_ICH_RCRB_TRWDR                            0x1E18   // Trap Write Data Register
#define   S_ICH_RCRB_TRWDR                            8
#define   B_ICH_RCRB_TRWDR_TIOD                       0x00000000FFFFFFFF
#define R_ICH_RCRB_IO_TRAP_0                        0x1E80   // Trap Configuration Register
#define R_ICH_RCRB_IO_TRAP_1                        0x1E88   // Trap Configuration Register
#define R_ICH_RCRB_IO_TRAP_2                        0x1E90   // Trap Configuration Register
#define R_ICH_RCRB_IO_TRAP_3                        0x1E98   // Trap Configuration Register
#define   B_ICH_RCRB_IO_TRAP_RWM                      BIT49
#define   B_ICH_RCRB_IO_TRAP_RWIO                     BIT48
#define   B_ICH_RCRB_IO_TRAP_BEM                      0x000000F000000000
#define   B_ICH_RCRB_IO_TRAP_TBE                      0x0000000F00000000
#define   B_ICH_RCRB_IO_TRAP_ADMA                     0x0000000000FC0000
#define   B_ICH_RCRB_IO_TRAP_IOAD                     0x000000000000FFFC
#define   B_ICH_RCRB_IO_TRAP_TRSE                     BIT0   // Trap and SMI# Enable
#define R_ICH_RCRB_DMI_MISC_CNTL                    0x2010  // DMI Miscellaneous Control Register
#define   B_ICH_RCRB_DMI_MISC_CTRL_FIELD_1            BIT19 // DMI Misc. Control Register Field 1
#define R_ICH_RCRB_CIR6                             0x2024
#define   B_ICH_RCRB_CIR6_FIELD_2_MASK                (BIT23 | BIT22 | BIT21)
#define   B_ICH_RCRB_CIR6_FIELD_1_MASK                BIT7
#define R_ICH_RCRB_DMI_DBG_FTS                      0x2027  // DMI debug and configuration, Fast Training Sequence byte
#define R_ICH_RCRB_CIR7                             0x2034  // Chipset Initialization Register 7
#define   B_ICH_RCRB_CIR7_FIELD_1_MASK                (BIT19 | BIT18 | BIT17 | BIT16)
#define R_ICH_RCRB_CIR11                            0x20C4  // Chipset Initialization Register 11
#define R_ICH_RCRB_CIR12                            0x20E4  // Chipset Initialization Register 12
#define R_ICH_RCRB_TCO_CNTL                         0x3000   // TCO control register
#define   B_ICH_RCRB_TCO_CNTL_IE                      BIT7   // TCO IRQ Enable
#define   B_ICH_RCRB_TCO_CNTL_IRQS                    (BIT2 | BIT1 | BIT0) // TCO IRQ Select
#define     V_ICH_RCRB_TCO_CNTL_IRQ_9                 0x00
#define     V_ICH_RCRB_TCO_CNTL_IRQ_10                0x01
#define     V_ICH_RCRB_TCO_CNTL_IRQ_11                0x02
#define     V_ICH_RCRB_TCO_CNTL_IRQ_20                0x04   // only if APIC enabled
#define     V_ICH_RCRB_TCO_CNTL_IRQ_21                0x05   // only if APIC enabled
#define     V_ICH_RCRB_TCO_CNTL_IRQ_22                0x06   // only if APIC enabled
#define     V_ICH_RCRB_TCO_CNTL_IRQ_23                0x07   // only if APIC enabled
#define     V_ICH_RCRB_IP_NO_INT                      0x0  // Designate "No interrupt" in Interrupt Pin Registers
#define R_ICH_RCRB_D31IP                            0x3100   // Device 31 interrupt pin
#define   B_ICH_RCRB_D31IP_TTIP                       (BIT27 | BIT26 | BIT25 | BIT24)
#define     V_ICH_RCRB_D31IP_TTIP_INTA                  (1 << 24)
#define     V_ICH_RCRB_D31IP_TTIP_INTB                  (2 << 24)
#define     V_ICH_RCRB_D31IP_TTIP_INTC                  (3 << 24)
#define     V_ICH_RCRB_D31IP_TTIP_INTD                  (4 << 24)
#define   B_ICH_RCRB_D31IP_SIP2                       (BIT23 | BIT22 | BIT21 | BIT20)
#define     V_ICH_RCRB_D31IP_SIP2_INTA                  (1 << 20)
#define     V_ICH_RCRB_D31IP_SIP2_INTB                  (2 << 20)
#define     V_ICH_RCRB_D31IP_SIP2_INTC                  (3 << 20)
#define     V_ICH_RCRB_D31IP_SIP2_INTD                  (4 << 20)
#define   B_ICH_RCRB_D31IP_SMIP                       (BIT15 | BIT14 | BIT13 | BIT12)
#define     V_ICH_RCRB_D31IP_SMIP_INTA                  (1 << 12)
#define     V_ICH_RCRB_D31IP_SMIP_INTB                  (2 << 12)
#define     V_ICH_RCRB_D31IP_SMIP_INTC                  (3 << 12)
#define     V_ICH_RCRB_D31IP_SMIP_INTD                  (4 << 12)
#define   B_ICH_RCRB_D31IP_SIP                        (BIT11 | BIT10 | BIT9 | BIT8)
#define     V_ICH_RCRB_D31IP_SIP_INTA                   (1 << 8)
#define     V_ICH_RCRB_D31IP_SIP_INTB                   (2 << 8)
#define     V_ICH_RCRB_D31IP_SIP_INTC                   (3 << 8)
#define     V_ICH_RCRB_D31IP_SIP_INTD                   (4 << 8)
#define   B_ICH_RCRB_D31IP_LIP                        (BIT3 | BIT2 | BIT1 | BIT0)
#define R_ICH_RCRB_D30IP                            0x3104   // Device 30 interrupt pin
#define   B_ICH_RCRB_D30IP_PIP                        0x0000000F
#define R_ICH_RCRB_D29IP                            0x3108   // Device 29 interrupt pin
#define   B_ICH_RCRB_D29IP_EIP                        (BIT31 | BIT30 | BIT29 | BIT28)
#define     V_ICH_RCRB_D29IP_EIP_INTA                   (1 << 28)
#define     V_ICH_RCRB_D29IP_EIP_INTB                   (2 << 28)
#define     V_ICH_RCRB_D29IP_EIP_INTC                   (3 << 28)
#define     V_ICH_RCRB_D29IP_EIP_INTD                   (4 << 28)
#define   B_ICH_RCRB_D29IP_U3P                        (BIT15 | BIT14 | BIT13 | BIT12)
#define     V_ICH_RCRB_D29IP_U3P_INTA                   (1 << 12)
#define     V_ICH_RCRB_D29IP_U3P_INTB                   (2 << 12)
#define     V_ICH_RCRB_D29IP_U3P_INTC                   (3 << 12)
#define     V_ICH_RCRB_D29IP_U3P_INTD                   (4 << 12)
#define   B_ICH_RCRB_D29IP_U2P                        (BIT11 | BIT10 | BIT9 | BIT8)
#define     V_ICH_RCRB_D29IP_U2P_INTA                   (1 << 8)
#define     V_ICH_RCRB_D29IP_U2P_INTB                   (2 << 8)
#define     V_ICH_RCRB_D29IP_U2P_INTC                   (3 << 8)
#define     V_ICH_RCRB_D29IP_U2P_INTD                   (4 << 8)
#define   B_ICH_RCRB_D29IP_U1P                        (BIT7 | BIT6 | BIT5 | BIT4)
#define     V_ICH_RCRB_D29IP_U1P_INTA                   (1 << 4)
#define     V_ICH_RCRB_D29IP_U1P_INTB                   (2 << 4)
#define     V_ICH_RCRB_D29IP_U1P_INTC                   (3 << 4)
#define     V_ICH_RCRB_D29IP_U1P_INTD                   (4 << 4)
#define   B_ICH_RCRB_D29IP_U0P                        (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_ICH_RCRB_D29IP_U0P_INTA                   (1 << 0)
#define     V_ICH_RCRB_D29IP_U0P_INTB                   (2 << 0)
#define     V_ICH_RCRB_D29IP_U0P_INTC                   (3 << 0)
#define     V_ICH_RCRB_D29IP_U0P_INTD                   (4 << 0)
#define R_ICH_RCRB_D28IP                            0x310C   // Device 28 interrupt pin
#define   B_ICH_RCRB_D28IP_P6IP                       (BIT23 | BIT22 | BIT21 | BIT20)
#define     V_ICH_RCRB_D28IP_P6IP_INTA                  (1 << 20)
#define     V_ICH_RCRB_D28IP_P6IP_INTB                  (2 << 20)
#define     V_ICH_RCRB_D28IP_P6IP_INTC                  (3 << 20)
#define     V_ICH_RCRB_D28IP_P6IP_INTD                  (4 << 20)
#define   B_ICH_RCRB_D28IP_P5IP                       (BIT19 | BIT18 | BIT17 | BIT16)
#define     V_ICH_RCRB_D28IP_P5IP_INTA                  (1 << 16)
#define     V_ICH_RCRB_D28IP_P5IP_INTB                  (2 << 16)
#define     V_ICH_RCRB_D28IP_P5IP_INTC                  (3 << 16)
#define     V_ICH_RCRB_D28IP_P5IP_INTD                  (4 << 16)
#define   B_ICH_RCRB_D28IP_P4IP                       (BIT15 | BIT14 | BIT13 | BIT12)
#define     V_ICH_RCRB_D28IP_P4IP_INTA                  (1 << 12)
#define     V_ICH_RCRB_D28IP_P4IP_INTB                  (2 << 12)
#define     V_ICH_RCRB_D28IP_P4IP_INTC                  (3 << 12)
#define     V_ICH_RCRB_D28IP_P4IP_INTD                  (4 << 12)
#define   B_ICH_RCRB_D28IP_P3IP                       (BIT11 | BIT10 | BIT9 | BIT8)
#define     V_ICH_RCRB_D28IP_P3IP_INTA                  (1 << 8)
#define     V_ICH_RCRB_D28IP_P3IP_INTB                  (2 << 8)
#define     V_ICH_RCRB_D28IP_P3IP_INTC                  (3 << 8)
#define     V_ICH_RCRB_D28IP_P3IP_INTD                  (4 << 8)
#define   B_ICH_RCRB_D28IP_P2IP                       (BIT7 | BIT6 | BIT5 | BIT4)
#define     V_ICH_RCRB_D28IP_P2IP_INTA                  (1 << 4)
#define     V_ICH_RCRB_D28IP_P2IP_INTB                  (2 << 4)
#define     V_ICH_RCRB_D28IP_P2IP_INTC                  (3 << 4)
#define     V_ICH_RCRB_D28IP_P2IP_INTD                  (4 << 4)
#define   B_ICH_RCRB_D28IP_P1IP                       (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_ICH_RCRB_D28IP_P1IP_INTA                  (1 << 0)
#define     V_ICH_RCRB_D28IP_P1IP_INTB                  (2 << 0)
#define     V_ICH_RCRB_D28IP_P1IP_INTC                  (3 << 0)
#define     V_ICH_RCRB_D28IP_P1IP_INTD                  (4 << 0)
#define R_ICH_RCRB_D27IP                            0x3110   // Device 27 interrupt pin
#define   B_ICH_RCRB_D27IP_ZIP                        (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_ICH_RCRB_D27IP_ZIP_INTA                   (1 << 0)
#define     V_ICH_RCRB_D27IP_ZIP_INTB                   (2 << 0)
#define     V_ICH_RCRB_D27IP_ZIP_INTC                   (3 << 0)
#define     V_ICH_RCRB_D27IP_ZIP_INTD                   (4 << 0)
#define R_ICH_RCRB_D26IP                            0x3114   // Device 26 interrupt pin
#define   B_ICH_RCRB_D26IP_E2IP                       (BIT31 | BIT30 | BIT29 | BIT28)
#define     V_ICH_RCRB_D26IP_E2IP_INTA                  (1 << 28)
#define     V_ICH_RCRB_D26IP_E2IP_INTB                  (2 << 28)
#define     V_ICH_RCRB_D26IP_E2IP_INTC                  (3 << 28)
#define     V_ICH_RCRB_D26IP_E2IP_INTD                  (4 << 28)
#define   B_ICH_RCRB_D26IP_U2P                        (BIT11 | BIT10 | BIT9 | BIT8)
#define     V_ICH_RCRB_D26IP_U2P_INTA                   (1 << 8)
#define     V_ICH_RCRB_D26IP_U2P_INTB                   (2 << 8)
#define     V_ICH_RCRB_D26IP_U2P_INTC                   (3 << 8)
#define     V_ICH_RCRB_D26IP_U2P_INTD                   (4 << 8)
#define   B_ICH_RCRB_D26IP_U1P                        (BIT7 | BIT6 | BIT5 | BIT4)
#define     V_ICH_RCRB_D26IP_U1P_INTA                   (1 << 4)
#define     V_ICH_RCRB_D26IP_U1P_INTB                   (2 << 4)
#define     V_ICH_RCRB_D26IP_U1P_INTC                   (3 << 4)
#define     V_ICH_RCRB_D26IP_U1P_INTD                   (4 << 4)
#define   B_ICH_RCRB_D26IP_U0P                        (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_ICH_RCRB_D26IP_U0P_INTA                   (1 << 0)
#define     V_ICH_RCRB_D26IP_U0P_INTB                   (2 << 0)
#define     V_ICH_RCRB_D26IP_U0P_INTC                   (3 << 0)
#define     V_ICH_RCRB_D26IP_U0P_INTD                   (4 << 0)
#define R_ICH_RCRB_D25IP                            0x3118   // Device 25 interrupt pin
#define   B_ICH_RCRB_D25IP_LIP                        (BIT3 | BIT2 | BIT1 | BIT0)
#define     V_ICH_RCRB_D25IP_LIP_INTA                   (1 << 0)
#define     V_ICH_RCRB_D25IP_LIP_INTB                   (2 << 0)
#define     V_ICH_RCRB_D25IP_LIP_INTC                   (3 << 0)
#define     V_ICH_RCRB_D25IP_LIP_INTD                   (4 << 0)

#define R_ICH_RCRB_D31IR                            0x3140   // Device 31 interrupt route
#define R_ICH_RCRB_D30IR                            0x3142         // Device 30 interrupt route
#define R_ICH_RCRB_D29IR                            0x3144         // Device 29 interrupt route
#define R_ICH_RCRB_D28IR                            0x3146         // Device 28 interrupt route
#define R_ICH_RCRB_D27IR                            0x3148         // Device 27 interrupt route
#define R_ICH_RCRB_D26IR                            0x314C         // Device 26 interrupt route
#define R_ICH_RCRB_D25IR                            0x3150         // Device 25 interrupt route
#define   B_ICH_RCRB_DXXIR_IDR_MASK                   (BIT14 | BIT13 | BIT12)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQA                  (0)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQB                  (BIT12)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQC                  (BIT13)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQD                  (BIT13 | BIT12)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQE                  (BIT14)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQF                  (BIT14 | BIT12)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQG                  (BIT14 | BIT13)
#define   V_ICH_RCRB_DXXIR_IDR_PIRQH                  (BIT14 | BIT13 | BIT12)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQA                  (0)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQB                  (BIT8)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQC                  (BIT9)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQD                  (BIT9 | BIT8)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQE                  (BIT10)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQF                  (BIT10| BIT8)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQG                  (BIT10| BIT9)
#define   V_ICH_RCRB_DXXIR_ICR_PIRQH                  (BIT10| BIT9 | BIT8)
#define   V_ICH_RCRB_DXXIR_IBR_PIRQA                  (0)
#define   V_ICH_RCRB_DXXIR_IBR_PIRQB                  (BIT4)
#define   V_ICH_RCRB_DXXIR_IBR_PIRQC                  (BIT5)
#define   V_ICH_RCRB_DXXIR_IBR_PIRQD                  (BIT5 | BIT4)
#define   V_ICH_RCRB_DXXIR_IBR_PIRQE                  (BIT6 )
#define   V_ICH_RCRB_DXXIR_IBR_PIRQF                  (BIT6 | BIT4)
#define   V_ICH_RCRB_DXXIR_IBR_PIRQG                  (BIT6 | BIT5)
#define   V_ICH_RCRB_DXXIR_IBR_PIRQH                  (BIT6 | BIT5 | BIT4)
#define   V_ICH_RCRB_DXXIR_IAR_PIRQA                  (0)
#define   V_ICH_RCRB_DXXIR_IAR_PIRQB                  (BIT0)
#define   V_ICH_RCRB_DXXIR_IAR_PIRQC                  (BIT1)
#define   V_ICH_RCRB_DXXIR_IAR_PIRQD                  (BIT1 | BIT0)
#define   V_ICH_RCRB_DXXIR_IAR_PIRQE                  (BIT2 )
#define   V_ICH_RCRB_DXXIR_IAR_PIRQF                  (BIT2 | BIT0)
#define   V_ICH_RCRB_DXXIR_IAR_PIRQG                  (BIT2 | BIT1)
#define   V_ICH_RCRB_DXXIR_IAR_PIRQH                  (BIT2 | BIT1 | BIT0)
#define R_ICH9_RCRB_OIC                             0x31FF    // Other Interrupt Control
#define   B_ICH9_RCRB_OIC_ASEL                         (BIT7 | BIT6 | BIT5 | BIT4)
#define   B_ICH9_RCRB_OIC_CEN                          BIT1         // Coprocessor Error Enable
#define   B_ICH9_RCRB_OIC_AEN                          BIT0         // APIC Enable
#define R_ICH10_RCRB_OIC_CONSUMER                   0x31FF    // Other Interrupt Control
#define   B_ICH10_RCRB_OIC_ASEL_CONSUMER               (BIT7 | BIT6 | BIT5 | BIT4)
#define   B_ICH10_RCRB_OIC_CEN_CONSUMER                BIT1         // Coprocessor Error Enable
#define   B_ICH10_RCRB_OIC_AEN_CONSUMER                BIT0         // APIC Enable
#define R_ICH10_RCRB_OIC_CORPORATE                  0x31FF    // Other Interrupt Control
#define   B_ICH10_RCRB_OIC_ASEL_CORPORATE              0xFF
#define   B_ICH10_RCRB_OIC_CEN_CORPORATE               BIT9         // Coprocessor Error Enable
#define   B_ICH10_RCRB_OIC_AEN_CORPORATE               BIT8         // APIC Enable
#define R_ICH_RCRB_SBEMC3                           0x3300
#define   B_ICH_RCRB_SBEMC3_C3FS_LT                    0x007F0000
#define   B_ICH_RCRB_SBEMC3_C2FS_LT                    0x00007F00
#define   B_ICH_RCRB_SBEMC3_C0FS_LT                    0x0000007F
#define R_ICH_RCRB_SBEMC4                           0x3304
#define   B_ICH_RCRB_SBEMC4_C4FS_LT                    0x7F000000
#define   B_ICH_RCRB_SBEMC4_C3FS_LT                    0x007F0000
#define   B_ICH_RCRB_SBEMC4_C2FS_LT                    0x00007F00
#define   B_ICH_RCRB_SBEMC4_C0FS_LT                    0x0000007F
#define R_ICH10_RCRB_PRSTS                          0x3310
#define   B_ICH10_RCRB_PRSTS_WOL_OVR_WK_STS            BIT5
#define   B_ICH10_RCRB_PRSTS_ME_HOST_PWRDN             BIT3
#define   B_ICH10_RCRB_PRSTS_ME_HRST_WARM_STS          BIT2
#define   B_ICH10_RCRB_PRSTS_ME_HRST_COLD_STS          BIT1
#define   B_ICH10_RCRB_PRSTS_ME_WAKE_STS               BIT0
#define R_ICH_RCRB_RTC_CONF                         0x3400         // RTC Configuration register
#define  S_ICH_RCRB_RTC_CONF                          4
#define   B_ICH_RCRB_RTC_CONF_UCMOS_LOCK                 BIT4
#define   B_ICH_RCRB_RTC_CONF_LCMOS_LOCK                 BIT3
#define   B_ICH_RCRB_RTC_CONF_UCMOS_EN                   BIT2         // Upper CMOS bank enable
#define R_ICH_RCRB_HPTC                             0x3404         // High Performance Timer Configuration
#define   B_ICH_RCRB_HPTC_AS                             (BIT1 | BIT0) // Address selection
#define   B_ICH_RCRB_HPTC_AE                             BIT7         // Address enable
#define R_ICH_RCRB_GCS                              0x3410         // General Control and Status
#define   B_ICH_RCRB_GCS_FLRCSSEL                     BIT12
#define   B_ICH_RCRB_GCS_BBS                          (BIT11 | BIT10) // Boot BIOS straps
#define   V_ICH_RCRB_GCS_BBS_SPI                      BIT10   // Boot BIOS strapped to SPI
#define   V_ICH_RCRB_GCS_BBS_PCI                      BIT11   // Boot BIOS strapped to PCI
#define   V_ICH_RCRB_GCS_BBS_LPC                      (BIT11 | BIT10) // Boot BIOS strapped to LPC
#define   B_ICH_RCRB_GCS_SERM                         BIT9    // Server Error Reporting Mode
#define   B_ICH_RCRB_GCS_MICLD                        BIT7    // Mobile IDE Configuration Lock Down
#define   B_ICH_RCRB_GCS_FME                          BIT6    // FERR# MUX Enable
#define   B_ICH_RCRB_GCS_NR                           BIT5    // No Reboot strap
#define   B_ICH_RCRB_GCS_AME                          BIT4    // Alternate Access Mode Enable
#define   B_ICH_RCRB_GCS_SPS                          BIT3    // Shutdown Policy Select
#define   B_ICH_RCRB_GCS_RPR                          BIT2    // Reserved Page Route
#define   B_ICH_RCRB_GCS_BILD                         BIT0    // BIOS Interface Lock-Down
#define   N_ICH_RCRB_GCS_BBS                          BIT10   // Boot BIOS straps offset
#define R_ICH_RCRB_BUC                              0x3414         // Backed Up Control
#define   B_ICH_RCRB_BUC_LAN_DIS                      BIT5    // LAN Disable
#define   B_ICH_RCRB_BUC_SDO                          BIT4    // Daylight Savings Override
#define   B_ICH_RCRB_BUC_CBE                          BIT2    // CPU BIST Enable
#define   B_ICH_RCRB_BUC_PRS                          BIT1    // PATA Reset State
#define   B_ICH_RCRB_BUC_TS                           BIT0    // Top Swap
#define R_ICH_RCRB_FUNC_DIS                         0x3418         // Function Disable Register
#define   B_ICH_RCRB_FUNC_DIS_FUNCTION_0              BIT0    // Function 0 disable (cannot be disabled per ICH BIOS specification, present for SV only)
#define   B_ICH_RCRB_FUNC_DIS_PATA                    BIT1    // Parallel ATA disable
#define   B_ICH_RCRB_FUNC_DIS_SATA1                   BIT2    // Serial ATA disable
#define   B_ICH_RCRB_FUNC_DIS_SMBUS                   BIT3    // SMBUS disable
#define   B_ICH_RCRB_FUNC_DIS_AZALIA                  BIT4    // Azalia disable
#define   B_ICH_RCRB_FUNC_DIS_UHCI1                   BIT8    // UHCI controller 1 disable
#define   B_ICH_RCRB_FUNC_DIS_UHCI2                   BIT9    // UHCI controller 2 disable
#define   B_ICH_RCRB_FUNC_DIS_UHCI3                   BIT10   // UHCI controller 3 disable
#define   B_ICH_RCRB_FUNC_DIS_UHCI4                   BIT11   // UHCI controller 4 disable
#define   B_ICH_RCRB_FUNC_DIS_UHCI5                   BIT12   // UHCI controller 5 disable
#define   B_ICH_RCRB_FUNC_DIS_UHCI6                   BIT7    // UHCI controller 6 disable
#define   B_ICH_RCRB_FUNC_DIS_EHCI2                   BIT13   // EHCI controller 2 disable
#define   B_ICH_RCRB_FUNC_DIS_LPC_BRIDGE              BIT14   // LPC Bridge disable
#define   B_ICH_RCRB_FUNC_DIS_EHCI1                   BIT15   // EHCI controller 1 disable
#define   B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT1            BIT16   // PCI Express port 1 disable
#define   B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT2            BIT17   // PCI Express port 2 disable
#define   B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT3            BIT18   // PCI Express port 3 disable
#define   B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT4            BIT19   // PCI Express port 4 disable
#define   B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT5            BIT20   // PCI Express port 5 disable
#define   B_ICH_RCRB_FUNC_DIS_PCI_EX_PORT6            BIT21   // PCI Express port 6 disable
#define   B_ICH_RCRB_FUNC_DIS_THERMAL                 BIT24   // Thermal Throttle Disable
#define   B_ICH_RCRB_FUNC_DIS_SATA2                   BIT25   // Serial ATA 2 disable

#define R_ICH_RCRB_PRC                              0x341C         // Power Reduction Control
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_LPC             BIT31   // Legacy(LPC) Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_PATA            BIT30   // PATA Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_UHCI            (BIT29|BIT28)   // USB UHCI Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA3           BIT27   // SATA Port3 Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA2           BIT26   // SATA Port2 Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA1           BIT25   // SATA Port1 Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA0           BIT24   // SATA Port0 Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_SCLK_GATE_LAN             BIT23   // LAN Static Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_HDA             BIT22   // HDA Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_SCLK_GATE_HDA             BIT21   // HDA Static Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_SCLK_GATE_EHCI            BIT20   // USB EHCI Static Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_EHCI            BIT19   // USB EHCI Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA5           BIT18   // SATA Port5 Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_SATA4           BIT17   // SATA Port4 Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_PCI             BIT16   // PCI Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_DIS_IDE_C3_BIASING           BIT15   // IDE C3 HV IO Biasing Disable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_SMBUS           BIT5    // SMBUS Clock Gating Enable
#define   B_ICH_RCRB_PRC_EN_PCIE_RX_GATE              BIT4    // PCIE Rx Clock Gating Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_RX              BIT3    // DMI and PCIE Rx Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_PCIE_TX         BIT2    // PCIE Tx Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_DCLK_GATE_DMI_TX          BIT1    // DMI Tx Dynamic Clock Gate Enable
#define   B_ICH_RCRB_PRC_EN_SCLK_GATE_PCIE_RP         BIT0    // PCIE Root Port Static Clock Gate Enable

#define R_ICH_RCRB_FDSW                             0x3420    // Function Disable SUS well
#define   B_ICH_RCRB_FDSW_FDSWL                       BIT7    // Function Disable SUS well lockdown
#define   B_ICH_RCRB_FDSW_LAN_DIS                     BIT0    // LAN Disable

#define R_ICH_RCRB_CIR8                             0x3430    // Chipset Initialization Register 8
#define   B_ICH_RCRB_CIR8_FIELD_1_MASK                (BIT1 | BIT0)   // Chipset Initialization Register 8 Field 1
#define R_ICH_RCRB_TSPC0                            0x3E0E    // Thermal Sensor Control Policy  0
#define R_ICH_RCRB_TSPC1                            0x3E4E    // Thermal Sensor Control Policy  1
#define R_ICH_RCRB_RECO                             0x3FFC    // Resume Well ECO

#define R_ICH_RCRB_USBIR1                           0x3504    // USB Initialization Register 1
#define   B_ICH_RCRB_USBIR1_PORT9_INIT_FIELD_MASK     (BIT29 | BIT28 | BIT27)
#define   B_ICH_RCRB_USBIR1_PORT8_INIT_FIELD_MASK     (BIT26 | BIT25 | BIT24)
#define   B_ICH_RCRB_USBIR1_PORT7_INIT_FIELD_MASK     (BIT23 | BIT22 | BIT21)
#define   B_ICH_RCRB_USBIR1_PORT6_INIT_FIELD_MASK     (BIT20 | BIT19 | BIT18)
#define   B_ICH_RCRB_USBIR1_PORT5_INIT_FIELD_MASK     (BIT17 | BIT16 | BIT15)
#define   B_ICH_RCRB_USBIR1_PORT4_INIT_FIELD_MASK     (BIT14 | BIT13 | BIT12)
#define   B_ICH_RCRB_USBIR1_PORT3_INIT_FIELD_MASK     (BIT11 | BIT10 | BIT9)
#define   B_ICH_RCRB_USBIR1_PORT2_INIT_FIELD_MASK     (BIT8 | BIT7 | BIT6)
#define   B_ICH_RCRB_USBIR1_PORT1_INIT_FIELD_MASK     (BIT5 | BIT4 | BIT3)
#define   B_ICH_RCRB_USBIR1_PORT0_INIT_FIELD_MASK     (BIT2 | BIT1 | BIT0)
#define R_ICH_RCRB_USBIR2                           0x3508    // USB Initialization Register 2
#define   B_ICH_RCRB_USBIR2_PORT11_INIT_FIELD_MASK    (BIT5 | BIT4 | BIT3)
#define   B_ICH_RCRB_USBIR2_PORT10_INIT_FIELD_MASK    (BIT2 | BIT1 | BIT0)

#define R_ICH_RCRB_CIR9                             0x350C         // Chipset Initialization Register 9
#define  S_ICH_RCRB_CIR9                              4
#define   B_ICH_RCRB_CIR9_FIELD1                       (BIT27 | BIT26) // BIOS must program this filed to 10b
#define     V_ICH_RCRB_CIR9_FIELD1                     0x08000000
#define R_ICH_RCRB_FUNC_PDO                         0x3518         // Usb Port Disable Override Register
#define  S_ICH_RCRB_FUNC_PDO                          2              // Size = 2 bytes
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT11               BIT11   // Usb Port 11 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT10               BIT10   // Usb Port 10 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT9                BIT9    // Usb Port 9 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT8                BIT8    // Usb Port 8 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT7                BIT7    // Usb Port 7 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT6                BIT6    // Usb Port 6 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT5                BIT5    // Usb Port 5 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT4                BIT4    // Usb Port 4 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT3                BIT3    // Usb Port 3 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT2                BIT2    // Usb Port 2 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT1                BIT1    // Usb Port 1 disable
#define   B_ICH_RCRB_FUNC_PDO_DIS_PORT0                BIT0    // Usb Port 0 disable
#define R_ICH_RCRB_FUNC_PPO                         0x3524         // Usb Port Power Off Register
#define  S_ICH_RCRB_FUNC_PDO                          2              // Size = 2 bytes
#define   B_ICH_RCRB_FUNC_PPO_PORT11                   BIT11   // Usb Port 11 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT10                   BIT10   // Usb Port 10 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT9                    BIT9    // Usb Port 9 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT8                    BIT8    // Usb Port 8 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT7                    BIT7    // Usb Port 7 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT6                    BIT6    // Usb Port 6 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT5                    BIT5    // Usb Port 5 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT4                    BIT4    // Usb Port 4 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT3                    BIT3    // Usb Port 3 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT2                    BIT2    // Usb Port 2 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT1                    BIT1    // Usb Port 1 power-off
#define   B_ICH_RCRB_FUNC_PPO_PORT0                    BIT0    // Usb Port 0 power-off
#define R_ICH_RCRB_CIR10                             0x352C    // Chipset Initialization Register 10
#define   B_ICH_RCRB_CIR10_FIELD_1_MASK                (BIT17 | BIT16)    // Chipset Initialization Register 10 Field 1
#define R_ICH_RCRB_REMAP_CONTROL                    0x35F0    // Remap Control Register
#define   B_ICH_RCRB_UHCI6_REMAP                       BIT0    // UHCI Controller 6 remap

//
// SPI Host Interface Registers
//
#define R_ICH_RCRB_SPI_BASE                         0x3800                        // Base address of the SPI host interface registers
#define R_ICH_SPI_BFPR                              (R_ICH_RCRB_SPI_BASE + 0x00)  // BIOS Flash Primary Region Register(32bits), which is RO and contains the same value from FREG1
#define R_ICH_SPI_HSFS                              (R_ICH_RCRB_SPI_BASE + 0x04)  // Hardware Sequencing Flash Status Register(16bits)
#define   B_ICH_SPI_HSFS_FLOCKDN                      BIT15                       // Flash Configuration Lock-Down
#define   B_ICH_SPI_HSFS_FDV                          BIT14                       // Flash Descriptor Valid, once valid software can use hareware sequencing regs
#define   B_ICH_SPI_HSFS_FDOPSS                       BIT13                       // Flash Descriptor Override Pin-Strap Status, 0: Override strap is set; 1: No override.OverrideCpuData
#define   B_ICH_SPI_HSFS_SCIP                         BIT5                        // SPI cyble in progress
#define   B_ICH_SPI_HSFS_BERASE_MASK                  (BIT4|BIT3)                 // Block/Sector Erase Size
#define   V_ICH_SPI_HSFS_BERASE_256B                  0x00                        // Block/Sector = 256 Bytes
#define   V_ICH_SPI_HSFS_BERASE_4K                    0x01                        // Block/Sector = 4K Bytes
#define   V_ICH_SPI_HSFS_BERASE_64K                   0x11                        // Block/Sector = 64K Bytes
#define   B_ICH_SPI_HSFS_AEL                          BIT2                        // Access Error Log
#define   B_ICH_SPI_HSFS_FCERR                        BIT1                        // Flash Cycle Error
#define   B_ICH_SPI_HSFS_FDONE                        BIT0                        // Flash Cycle Done
#define R_ICH_SPI_HSFC                              (R_ICH_RCRB_SPI_BASE + 0x06)  // Hardware Sequencing Flash Control Register(16bits)
#define   B_ICH_SPI_HSFC_FSMIE                        BIT15                       // Flash SPI SMI# Enable
#define   B_ICH_SPI_HSFC_FDBC_MASK                    0x3F00                      // Flash Data Byte Count ( <= 64), Count = (Value in this field) + 1.
#define   V_ICH_SPI_SPID_MAX                          0x40                        // Maximum number of SPI data allowed = N(16) * 4
#define   B_ICH_SPI_HSFC_FCYCLE_MASK                  0x0006                      // Flash Cycle.
#define   V_ICH_SPI_HSFC_FCYCLE_READ                  0                           // Flash Cycle Read
#define   V_ICH_SPI_HSFC_FCYCLE_WRITE                 2                           // Flash Cycle Write
#define   V_ICH_SPI_HSFC_FCYCLE_ERASE                 3                           // Flash Cycle Block Erase
#define   B_ICH_SPI_HSFC_FCYCLE_FGO                   BIT0                        // Flash Cycle Go.
#define R_ICH_SPI_FADDR                             (R_ICH_RCRB_SPI_BASE + 0x08)  // SPI Flash Address
#define   B_ICH_SPI_FADDR_MASK                         0x01FFFFFF                  // SPI Flash Address Mask (0~24bit)
#define R_ICH_SPI_FDATA00                             (R_ICH_RCRB_SPI_BASE + 0x10)  // SPI Data 00 (32 bits)
#define   B_ICH_SPI_FDATA00_FD                        BIT15                       // Flash Data0, less sig byte first, msb in byte first.
#define R_ICH_SPI_FDATA01                           (R_ICH_RCRB_SPI_BASE + 0x14)  // SPI Data 01
#define R_ICH_SPI_FDATA02                           (R_ICH_RCRB_SPI_BASE + 0x18)  // SPI Data 02
#define R_ICH_SPI_FDATA03                           (R_ICH_RCRB_SPI_BASE + 0x1C)  // SPI Data 03
#define R_ICH_SPI_FDATA04                           (R_ICH_RCRB_SPI_BASE + 0x20)  // SPI Data 04
#define R_ICH_SPI_FDATA05                           (R_ICH_RCRB_SPI_BASE + 0x24)  // SPI Data 05
#define R_ICH_SPI_FDATA06                           (R_ICH_RCRB_SPI_BASE + 0x28)  // SPI Data 06
#define R_ICH_SPI_FDATA07                           (R_ICH_RCRB_SPI_BASE + 0x2C)  // SPI Data 07
#define R_ICH_SPI_FDATA08                           (R_ICH_RCRB_SPI_BASE + 0x30)  // SPI Data 08
#define R_ICH_SPI_FDATA09                           (R_ICH_RCRB_SPI_BASE + 0x34)  // SPI Data 09
#define R_ICH_SPI_FDATA10                           (R_ICH_RCRB_SPI_BASE + 0x38)  // SPI Data 10
#define R_ICH_SPI_FDATA11                           (R_ICH_RCRB_SPI_BASE + 0x3C)  // SPI Data 11
#define R_ICH_SPI_FDATA12                           (R_ICH_RCRB_SPI_BASE + 0x40)  // SPI Data 12
#define R_ICH_SPI_FDATA13                           (R_ICH_RCRB_SPI_BASE + 0x44)  // SPI Data 13
#define R_ICH_SPI_FDATA14                           (R_ICH_RCRB_SPI_BASE + 0x48)  // SPI Data 14
#define R_ICH_SPI_FDATA15                           (R_ICH_RCRB_SPI_BASE + 0x4C)  // SPI Data 15
#define R_ICH_SPI_FRAP                              (R_ICH_RCRB_SPI_BASE + 0x50)  // SPI Flash Regions Access Permisions Register
#define   B_ICH_SPI_FRAP_BMWAG_MASK                   0xFF000000                  // Master Write Access Grant MASK
#define   B_ICH_SPI_FRAP_BMWAG_BIOS                   BIT25                       // Master write access grant for Host CPU/BIOS
#define   B_ICH_SPI_FRAP_BMWAG_ME                     BIT26                       // Master write access grant for ME
#define   B_ICH_SPI_FRAP_BMWAG_GBE                    BIT27                       // Master write access grant for Host CPU/GbE
#define   B_ICH_SPI_FRAP_BMRAG_MASK                   0x00FF0000                  // Master Write Access Grant MASK
#define   B_ICH_SPI_FRAP_BMRAG_BIOS                   BIT17                       // Master write access grant for Host CPU/BIOS
#define   B_ICH_SPI_FRAP_BMRAG_ME                     BIT18                       // Master write access grant for ME
#define   B_ICH_SPI_FRAP_BMRAG_GBE                    BIT19                       // Master write access grant for Host CPU/GbE
#define   B_ICH_SPI_FRAP_BRWA_MASK                    0x0000FF00                  // BIOS Regsion Write Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: ...
#define   B_ICH_SPI_FRAP_BRWA_FLASHD                  BIT8                        // Region write access for Region0 Flash Descriptor
#define   B_ICH_SPI_FRAP_BRWA_BIOS                    BIT9                        // Region write access for Region1 BIOS
#define   B_ICH_SPI_FRAP_BRWA_ME                      BIT10                       // Region write access for Region2 ME
#define   B_ICH_SPI_FRAP_BRWA_GBE                     BIT11                       // Region write access for Region3 GbE
#define   B_ICH_SPI_FRAP_BRRA_MASK                    0x000000FF                  // BIOS Regsion Read Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: ...
#define   B_ICH_SPI_FRAP_BRRA_FLASHD                  BIT0                        // Region read access for Region0 Flash Descriptor
#define   B_ICH_SPI_FRAP_BRRA_BIOS                    BIT1                        // Region read access for Region1 BIOS
#define   B_ICH_SPI_FRAP_BRRA_ME                      BIT2                        // Region read access for Region2 ME
#define   B_ICH_SPI_FRAP_BRRA_GBE                     BIT3                        // Region read access for Region3 GbE
#define R_ICH_SPI_FREG0_FLASHD                      (R_ICH_RCRB_SPI_BASE + 0x54)  // Flash Region 0(Flash Descriptor)(32bits)
#define   B_ICH_SPI_FREG0_LIMIT_MASK                  0x1FFF0000                  // Size, [28:16] here represents limit[24:12]
#define   B_ICH_SPI_FREG0_BASE_MASK                   0x00001FFF                  // Base, [12:0]  here represents base [24:12]
#define R_ICH_SPI_FREG1_BIOS                        (R_ICH_RCRB_SPI_BASE + 0x58)  // Flash Region 1(BIOS)(32bits)
#define   B_ICH_SPI_FREG1_LIMIT_MASK                  0x1FFF0000                  // BIOS size(24:12) = 0x1FFF * 4K = 32M
#define   B_ICH_SPI_FREG1_BASE_MASK                   0x00001FFF                  // BIOS base(24:12)
#define R_ICH_SPI_FREG2_ME                          (R_ICH_RCRB_SPI_BASE + 0x5C)  // Flash Region 2(ME)(32bits)
#define   B_ICH_SPI_FREG2_LIMIT_MASK                  0x1FFF0000                  // Size, [28:16] here represents limit[24:12]
#define   B_ICH_SPI_FREG2_BASE_MASK                   0x00001FFF                  // Base, [12:0]  here represents base [24:12]
#define R_ICH_SPI_FREG3_GBE                         (R_ICH_RCRB_SPI_BASE + 0x60)  // Flash Region 3(GbE)(32bits)
#define   B_ICH_SPI_FREG3_LIMIT_MASK                  0x1FFF0000                  // Size, [28:16] here represents limit[24:12]
#define   B_ICH_SPI_FREG3_BASE_MASK                   0x00001FFF                  // Base, [12:0]  here represents base [24:12]
#define R_ICH_SPI_FREG4_PLATFORM_DATA               (R_ICH_RCRB_SPI_BASE + 0x64)  // Flash Region 4(Platform Data)(32bits)
#define   B_ICH_SPI_FREG4_LIMIT_MASK                  0x1FFF0000                  // Size, [28:16] here represents limit[24:12]
#define   B_ICH_SPI_FREG4_BASE_MASK                   0x00001FFF                  // Base, [12:0]  here represents base [24:12]
#define R_ICH_SPI_PR0                               (R_ICH_RCRB_SPI_BASE + 0x74)  // Protected Region 0 Register
#define   B_ICH_SPI_PR0_WPE                           BIT31                       // Write Protection Enable
#define   B_ICH_SPI_PR0_PRL_MASK                      0x1FFF0000                  // Protected Range Limit Mask, [28:16] here represents upper limit of address [24:12],
#define   B_ICH_SPI_PR0_RPE                           BIT15                       // Read Protection Enable
#define   B_ICH_SPI_PR0_PRB_MASK                      0x00001FFF                  // Protected Range Base Mask, [12:0] here represents base limit of address [24:12],
#define R_ICH_SPI_PR1                               (R_ICH_RCRB_SPI_BASE + 0x78)  // Protected Region 1 Register
#define   B_ICH_SPI_PR1_WPE                           BIT31                       // Write Protection Enable
#define   B_ICH_SPI_PR1_PRL_MASK                      0x1FFF0000                  // Protected Range Limit Mask
#define   B_ICH_SPI_PR1_RPE                           BIT15                       // Read Protection Enable
#define   B_ICH_SPI_PR1_PRB_MASK                      0x00001FFF                  // Protected Range Base Mask
#define R_ICH_SPI_PR2                               (R_ICH_RCRB_SPI_BASE + 0x7C)  // Protected Region 2 Register
#define   B_ICH_SPI_PR2_WPE                           BIT31                       // Write Protection Enable
#define   B_ICH_SPI_PR2_PRL_MASK                      0x1FFF0000                  // Protected Range Limit Mask
#define   B_ICH_SPI_PR2_RPE                           BIT15                       // Read Protection Enable
#define   B_ICH_SPI_PR2_PRB_MASK                      0x00001FFF                  // Protected Range Base Mask
#define R_ICH_SPI_PR3                               (R_ICH_RCRB_SPI_BASE + 0x80)  // Protected Region 3 Register
#define   B_ICH_SPI_PR3_WPE                           BIT31                       // Write Protection Enable
#define   B_ICH_SPI_PR3_PRL_MASK                      0x1FFF0000                  // Protected Range Limit Mask,
#define   B_ICH_SPI_PR3_RPE                           BIT15                       // Read Protection Enable
#define   B_ICH_SPI_PR3_PRB_MASK                      0x00001FFF                  // Protected Range Base Mask
#define R_ICH_SPI_PR4                               R_ICH_RCRB_SPI_BASE + 0x84)   // Protected Region 4 Register
#define   B_ICH_SPI_PR4_WPE                           BIT31                       // Write Protection Enable
#define   B_ICH_SPI_PR4_PRL_MASK                      0x1FFF0000                  // Protected Range Limit Mask,
#define   B_ICH_SPI_PR4_RPE                           BIT15                       // Read Protection Enable
#define   B_ICH_SPI_PR4_PRB_MASK                      0x00001FFF                  // Protected Range Base Mask
#define R_ICH_SPI_SSFS                              (R_ICH_RCRB_SPI_BASE + 0x90)  // Software Sequencing Flash Status Register(8bits)
#define   B_ICH_SPI_SSFS_AEL                          BIT4                        // Access Error Log
#define   B_ICH_SPI_SSFS_FCERR                        BIT3                        // Flash Cycle Error
#define   B_ICH_SPI_SSFS_CDS                          BIT2                        // Cycle Done Status
#define   B_ICH_SPI_SSFS_SCIP                         BIT0                        // SPI Cycle in Progress
#define R_ICH_SPI_SSFC                              (R_ICH_RCRB_SPI_BASE + 0x91)  // Software Sequencing Flash Control(24bits)
#define   B_ICH_SPI_SSFC_SCF_MASK                     (BIT18 | BIT17 | BIT16)     // SPI Cycle Frequency
#define   V_ICH_SPI_SSFC_SCF_20MHZ                    0                           // SPI Cycle Frequency = 20MHz
#define   V_ICH_SPI_SSFC_SCF_33MHZ                    1                           // SPI Cycle Frequency = 33MHz
#define   V_ICH_SPI_SSFC_SCF_66MHZ                    2                           // SPI Cycle Frequency = 66MHz
#define   B_ICH_SPI_SSFC_SME                          BIT15                       // SPI SMI# Enable
#define   B_ICH_SPI_SSFC_DS                           BIT14                       // SPI Data Cycle
#define   B_ICH_SPI_SSFC_DBC_MASK                     0x3F00                      // SPI Data Byte Count (value here + 1 = count)
#define   B_ICH_SPI_SSFC_COP                          0x0070                      // Cycle Opcode Pointer
#define   B_ICH_SPI_SSFC_SPOP                         BIT3                        // Sequence Prefix Opcode Pointer
#define   B_ICH_SPI_SSFC_ACS                          BIT2                        // Atomic Cycle Sequence
#define   B_ICH_SPI_SSFC_SCGO                         BIT1                        // SPI Cycle Go
#define R_ICH_SPI_PREOP                             (R_ICH_RCRB_SPI_BASE + 0x94)  // Prefix Opcode Configuration Register(16 bits)
#define   B_ICH_SPI_PREOP1_MASK                       0xFF00                      // Prefix Opcode 1 Mask
#define   B_ICH_SPI_PREOP0_MASK                       0x00FF                      // Prefix Opcode 0 Mask
#define R_ICH_SPI_OPTYPE                            (R_ICH_RCRB_SPI_BASE + 0x96)  // Opcode Type Configuration
#define   B_ICH_SPI_OPTYPE0_MASK                      (BIT1 | BIT0 )              // Opcode Type 0 Mask
#define   B_ICH_SPI_OPTYPE1_MASK                      (BIT3 | BIT2 )              // Opcode Type 1 Mask
#define   B_ICH_SPI_OPTYPE2_MASK                      (BIT5 | BIT4 )              // Opcode Type 2 Mask
#define   B_ICH_SPI_OPTYPE3_MASK                      (BIT7 | BIT6 )              // Opcode Type 3 Mask
#define   B_ICH_SPI_OPTYPE4_MASK                      (BIT9 | BIT8 )              // Opcode Type 4 Mask
#define   B_ICH_SPI_OPTYPE5_MASK                      (BIT11| BIT10)              // Opcode Type 5 Mask
#define   B_ICH_SPI_OPTYPE6_MASK                      (BIT13| BIT12)              // Opcode Type 6 Mask
#define   B_ICH_SPI_OPTYPE7_MASK                      (BIT15| BIT14)              // Opcode Type 7 Mask
#define   V_ICH_SPI_OPTYPE_RDNOADDR                   0x00                        // Read cycle type without address
#define   V_ICH_SPI_OPTYPE_WRNOADDR                   0x01                        // Write cycle type without address
#define   V_ICH_SPI_OPTYPE_RDADDR                     0x02                        // Address required; Read cycle type
#define   V_ICH_SPI_OPTYPE_WRADDR                     0x03                        // Address required; Write cycle type
#define R_ICH_SPI_OPMENU                            (R_ICH_RCRB_SPI_BASE + 0x98)  // Opcode Menu Configuration (64bits)
#define R_ICH_SPI_BBAR                              (R_ICH_RCRB_SPI_BASE + 0xA0)  // BIOS Base Address Configuration
#define R_ICH_SPI_FDOC                              (R_ICH_RCRB_SPI_BASE + 0xB0)  // Flash Descriptor Observability Control Register(32 bits)
#define   B_ICH_SPI_FDOC_FDSS_MASK                    (BIT14 | BIT13 | BIT12)     // Flash Descritor Section Select
#define   V_ICH_SPI_FDOC_FDSS_FSDM                    0x0000                      // Flash Signature and Descriptor Map
#define   V_ICH_SPI_FDOC_FDSS_COMP                    0x1000                      // Component
#define   V_ICH_SPI_FDOC_FDSS_REGN                    0x2000                      // Region
#define   V_ICH_SPI_FDOC_FDSS_MSTR                    0x3000                      // Master
#define   V_ICH_SPI_FDOC_FDSS_ICHS                    0x4000                      // ICH soft straps
#define   B_ICH_SPI_FDOC_FDSI_MASK                    0x0FFC                      // Flash Descriptor Section Index
#define R_ICH_SPI_FDOD                              (R_ICH_RCRB_SPI_BASE + 0xB4)  // Flash Descriptor Observability Data Register(32 bits)
#define R_ICH_RCRB_SPI_AFC                          (R_ICH_RCRB_SPI_BASE + 0xC0)  // Additional Flash Control Register
#define   B_ICH_SPI_AFC_INTF_DCLK_GATE_EN             (BIT2 | BIT1)               // Flash Controller Interface Dynamic Clock Gating Enable
#define   B_ICH_SPI_AFC_CORE_DCLK_GATE_EN             BIT0                        // Flash Controller Core Dynamic Clock Gating Enable
#define R_ICH_SPI_LVSCC                             (R_ICH_RCRB_SPI_BASE + 0xC4)  // Vendor Specific Component Capabilities Register(32 bits)
#define   B_ICH_SPI_LVSCC_VCL                         BIT23                       // Vendor Component Lock
#define   B_ICH_SPI_LVSCC_EO_MASK                     0x0000FF00                  // Erase Opcode
#define   B_ICH_SPI_LVSCC_WEWS                        BIT4                        // Write Enable on Write Status
#define   B_ICH_SPI_LVSCC_WSR                         BIT3                        // Write Status Required
#define   B_ICH_SPI_LVSCC_WG_64B                      BIT2                        // Write Granularity, 0: 1 Byte; 1: 64 Bytes
#define   B_ICH_SPI_LVSCC_BSES_MASK                   (BIT1 | BIT0)               // Block/Sector Erase Size
#define   V_ICH_SPI_LVSCC_BSES_256B                   0x0                         // Block/Sector Erase Size = 256 Bytes
#define   V_ICH_SPI_LVSCC_BSES_4K                     0x1                         // Block/Sector Erase Size = 4K Bytes
#define   V_ICH_SPI_LVSCC_BSES_64K                    0x3                         // Block/Sector Erase Size = 64K Bytes
#define R_ICH_SPI_UVSCC                             (R_ICH_RCRB_SPI_BASE + 0xC8)  // Vendor Specific Component Capabilities Register(32 bits)
#define   B_ICH_SPI_UVSCC_VCL                         BIT23                       // Vendor Component Lock
#define   B_ICH_SPI_UVSCC_EO_MASK                     0x0000FF00                  // Erase Opcode
#define   B_ICH_SPI_UVSCC_WEWS                        BIT4                        // Write Enable on Write Status
#define   B_ICH_SPI_UVSCC_WSR                         BIT3                        // Write Status Required
#define   B_ICH_SPI_UVSCC_WG_64B                      BIT2                        // Write Granularity, 0: 1 Byte; 1: 64 Bytes
#define   B_ICH_SPI_UVSCC_BSES_MASK                   (BIT1 | BIT0)               // Block/Sector Erase Size
#define   V_ICH_SPI_UVSCC_BSES_256B                   0x0                         // Block/Sector Erase Size = 256 Bytes
#define   V_ICH_SPI_UVSCC_BSES_4K                     0x1                         // Block/Sector Erase Size = 4K Bytes
#define   V_ICH_SPI_UVSCC_BSES_64K                    0x3                         // Block/Sector Erase Size = 64K Bytes
#define R_ICH_SPI_FPB                               (R_ICH_RCRB_SPI_BASE + 0xD0)  // Flash Partition Boundary
#define   B_ICH_SPI_FPB_FPBA_MASK                     0x00001FFF                  // Flash Partition Boundary Address Mask, reflecting FPBA[24:12]

//
// Flash Descriptor Base Address Region (FDBAR) from Flash Region 0
//
#define R_SPI_FDBAR_FLVALSIG                            0x00
#define    V_SPI_FDBAR_FLVALSIG                         0x0FF0A55A
#define R_SPI_FDBAR_FLASH_MAP0                            0x04
#define   B_SPI_FDBAR_FCBA                              0x000000FF    // Flash Component Base Address
#define   B_SPI_FDBAR_NC                                0x00000300    // Number Of Components
#define   B_SPI_FDBAR_FRBA                              0x00FF0000    // Flash Region Base Address
#define   B_SPI_FDBAR_NR                                0x07000000    // Number Of Regions
#define R_SPI_FDBAR_FLASH_MAP1                            0x08
#define   B_SPI_FDBAR_FMBA                              0x000000FF    // Flash Master Base Address
#define   B_SPI_FDBAR_NM                                0x00000700    // Number Of Masters
#define   B_SPI_FDBAR_FISBA                             0x00FF0000    // Flash ICH Strap Base Address
#define   B_SPI_FDBAR_ISL                               0xFF000000    // ICH Strap Length
#define R_SPI_FDBAR_FLASH_MAP2                            0x0C
#define   B_SPI_FDBAR_FMSBA                             0x000000FF    // Flash MCH Strap Base Address
#define   B_SPI_FDBAR_MSL                               0x0000FF00    // MCH Strap Length
#define R_SPI_FDBAR_FLASH_UPPER_MAP1                      0xEFC         // Flash Upper Map 1
#define   B_SPI_FDBAR_VTBA                              0x000000FF    // VSCC Table Base Address
#define   B_SPI_FDBAR_VTL                               0x0000FF00    // VSCC Table Length
#define R_SPI_FDBAR_VTBA_JID0                       0x00          // JEDEC-ID 0 Register
#define   S_SPI_FDBAR_VTBA_JID0                       0x04
#define   B_SPI_VTBA_JID0_VENDOR_ID                   0x000000FF
#define   B_SPI_VTBA_JID0_DEVICE_ID0                  0x0000FF00
#define   B_SPI_VTBA_JID0_DEVICE_ID1                  0x00FF0000
#define   N_SPI_VTBA_JID0_DEVICE_ID0                  0x08
#define   N_SPI_VTBA_JID0_DEVICE_ID1                  0x10
#define R_SPI_FDBAR_VTBA_VSCC0                      0x04
#define   S_SPI_FDBAR_VTBA_VSCC0                      0x04
#define   B_SPI_VTBA_VSCC0_UCAPS                      0x0000FFFF
#define   B_SPI_VTBA_VSCC0_LCAPS                      0xFFFF0000

//
// Reset Generator I/O Port
//
#define RST_CNT                                       0xCF9
#define   B_RST_CNT_FULL_RST                           BIT3
#define   B_RST_CNT_RST_CPU                            BIT2
#define   B_RST_CNT_SYS_RST                            BIT1
#define    V_RST_CNT_POWERGOODRESET                     0x0E                        // ESS_OVERRIDE
#define    V_RST_CNT_HARDRESET                          0x06
#define    V_RST_CNT_SOFTRESET                          0x04
#define    V_RST_CNT_HARDSTARTSTATE                     0x02
#define    V_RST_CNT_SOFTSTARTSTATE                     0x00

#define PCI_BUS_NUMBER_ICH_IOAPIC                      0xF0                         // ESS_OVERRIDE
#define PCI_DEVICE_NUMBER_ICH_IOAPIC                   0x1F                         // ESS_OVERRIDE
#define PCI_FUNCTION_NUMBER_ICH_IOAPIC                 0x07                         // ESS_OVERRIDE
#define R_ICH_LPC_IBDF                                0x6C                          // ESS_OVERRIDE

#include "IchRegsAliases.h"

#define PCI_ICH_LPC_ADDRESS(Register) \
  ((UINTN)(PCI_LIB_ADDRESS (PCI_BUS_NUMBER_ICH, PCI_DEVICE_NUMBER_ICH_LPC, PCI_FUNCTION_NUMBER_ICH_LPC, Register)))


#endif
