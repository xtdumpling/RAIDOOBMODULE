//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Configuration.h

  Driver configuration include file.

Revision History:

**/

#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

// APTIOV_SERVER_OVERRIDE_RC_START : Resolving redefinition error for VFR_FORMID_INTELRCSETUP
#include <token.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Resolving redefinition error for VFR_FORMID_INTELRCSETUP

#define EFI_NON_DEVICE_CLASS            0x00
#define EFI_DISK_DEVICE_CLASS           0x01
#define EFI_VIDEO_DEVICE_CLASS          0x02
#define EFI_NETWORK_DEVICE_CLASS        0x04
#define EFI_INPUT_DEVICE_CLASS          0x08
#define EFI_ON_BOARD_DEVICE_CLASS       0x10
#define EFI_OTHER_DEVICE_CLASS          0x20

#define VFR_MAIN_CLASS                  0x01
// APTIOV_SERVER_OVERRIDE_RC_START
//#define VFR_ADVANCED_CLASS              0x02
#define VFR_INTELRCSETUP_CLASS              0x02 // Changed to INTELRCSETUP
// APTIOV_SERVER_OVERRIDE_RC_END
#define VFR_BOOT_CLASS                  0x03
#define BOOT_OPTIONS_CLASS              0x04
#define VFR_SYSINFO_CLASS               0x05

#define VFR_SERVER_MGMT_CLASS           0x06
// APTIOV_SERVER_OVERRIDE_RC_START
//#define VFR_FORMID_ADVANCED             0x01 // Commented to avoid redefinition errors when including Token.h
// APTIOV_SERVER_OVERRIDE_RC_END
//#define VFR_FORMID_PROCESSOR            0x02
#define VFR_FORMID_PCH                  0x06
#define VFR_FORMID_IDE                  0x07
#define VFR_FORMID_ELOG                 0x08
#define VFR_FORMID_MISC                 0x09
#define VFR_FORMID_USB                  0x0A
#define VFR_FORMID_NETWORK              0x0B
#define VFR_FORMID_BMCLAN               0x0C
//#define VFR_FORMID_PWRMGT               0x0C


#define VFR_FORMID_AMT_ME               0x50
#define VFR_FORMID_AMT_FWDEBUG          0x51
#define VFR_FORMID_AMT_FWDEBUG_GEN      0x52
#define VFR_FORMID_AMT_ICC_OVERRIDE     0x53
#define VFR_FORMID_SPS_ME               0x54
#define VFR_FORMID_SPS_FWDEBUG          0x55
#define VFR_FORMID_SPS_FWDEBUG_GEN      0x56
#define VFR_FORMID_SPS_FWDEBUG_NM       0x57
#define VFR_FORMID_DFX_FWDEBUG          0x58
#define VFR_FORMID_DFX_FWDEBUG_GEN      0x59
#define VFR_FORMID_SPS_ICC_OVERRIDE     0x5A
#define VFR_FORMID_ICC_SPREAD           0x5B

//#define VFR_FORMID_COMMONRC             0x17
#define MAIN_PAGE_SYS_INFO              0x18
#define VFR_RESERVE_MEMORY              0x19
#define VFR_FORMID_WHEA                 0x1a
#define VFR_FORMID_MEM_ERR    0x1b
#define VFR_FORMID_PCH_ERR    0x1d
#define VFR_FORMID_KTI_ERR    0x1e
#define VFR_FORMID_PROC_ERR   0x1f
#define VFR_FORMID_PCI_ERR    0x20
#define VFR_FORMID_ME_SMBIOS  0x21
#define VFR_FORMID_IE         0x22
#define VFR_FORMID_RESET_TESTING   0x23
#define DEBUG_INTERFACE_FORM_ID    0x24


#define VFR_FORMID_IRRP_ERR     0x400

#define VFR_FORMID_EMCA_SETTING         0x501


#define VFR_FORMID_CONSOLE_REDIRECTION_CONFIGURATION 0x88


#define VFR_FORMID_AMT                  0x01A8
#define VFR_FORMID_MEBX                 0x01A9
#define KEY_ICC_PROFILE                 0x10B0
#define KEY_ICC_FREQ2                   0x10B1
#define KEY_ICC_FREQ3                   0x10B2
#define KEY_ICC_SPREAD2                 0x10B3
#define KEY_ICC_SPREAD3                 0x10B4
#define KEY_ICC_RATIO2                  0x10B5
#define KEY_SERIAL_DEBUG_MSGLVL         0x10B6
#define KEY_CLOUD_PROFILE               0x10B7

#define ICC_OVERRIDE_FORM_AMT_QUESTIONID     0x10C8
#define ICC_OVERRIDE_FORM_SPS_QUESTIONID     0x10C9

//
// PCH Form IDs
//
#define PCH_OPTIONS_FORM_ID             0x60
#define PCH_LAN_OPTIONS_FORM_ID         0x61
#define PCH_PCIERP1_OPTIONS_FORM_ID     0x62
#define PCH_PCIERP2_OPTIONS_FORM_ID     0x63
#define PCH_PCIERP3_OPTIONS_FORM_ID     0x64
#define PCH_PCIERP4_OPTIONS_FORM_ID     0x65
#define PCH_PCIERP5_OPTIONS_FORM_ID     0x66
#define PCH_PCIERP6_OPTIONS_FORM_ID     0x67
#define PCH_PCIERP7_OPTIONS_FORM_ID     0x102
#define PCH_PCIERP8_OPTIONS_FORM_ID     0x103
#define PCH_PCIERP9_OPTIONS_FORM_ID     0x69
#define PCH_PCIERP10_OPTIONS_FORM_ID     0x6A
#define PCH_PCIERP11_OPTIONS_FORM_ID     0x6B
#define PCH_PCIERP12_OPTIONS_FORM_ID     0x6C
#define PCH_PCIERP13_OPTIONS_FORM_ID     0x6D
#define PCH_PCIERP14_OPTIONS_FORM_ID     0x6E
#define PCH_PCIERP15_OPTIONS_FORM_ID     0x6F
#define PCH_PCIERP16_OPTIONS_FORM_ID     0x70
#define PCH_PCIERP17_OPTIONS_FORM_ID     0x71
#define PCH_PCIERP18_OPTIONS_FORM_ID     0x72
#define PCH_PCIERP19_OPTIONS_FORM_ID     0x73
#define PCH_PCIERP20_OPTIONS_FORM_ID     0x74
#define PCH_SATA_OPTIONS_FORM_ID        0x68
#define PCH_EVA_OPTIONS_FORM_ID         0x104
#define PCH_USB_OPTIONS_FORM_ID         0x107
#define PCH_EVA_CLOCK_GATING_FORM_ID    0x10A
#define PCH_AZALIA_OPTIONS_FORM_ID      0x10B
#define PCH_THERMAL_FORM_ID             0x10C
#define PCH_SATA_SFMS_OPTIONS_FORM_ID   0x10D
#define PCH_PCIE_OPTIONS_FORM_ID        0x10E
#define PCH_SECURITY_FORM_ID            0x10F
#define PCH_LPSS_OPTIONS_FORM_ID        0x110
#define PCH_SSATA_OPTIONS_FORM_ID       0x119
#define PCH_SSATA_SFMS_OPTIONS_FORM_ID  0x11A
#define PCH_SMBUS_FORM_ID               0x11B
#define PCH_CPM_FORM_ID                 0x11C
#define PCH_TRACE_HUB_OPTIONS_FORM_ID   0x11D
#define PCHDFX_CONFIG_FORM_ID           0x11E
#define PCH_PCIE_LANE_EQ_FORM_ID        0x121
#define PCHDFX_LTR_CONFIG_FORM_ID       0x122
#define PCH_DWR_CONFIG_FORM_ID          0x123
#define PCH_PCIE_EQ_CONF_FORM_ID        0x124
#define PCH_PCIE_SW_HW_EQ_OVERRIDE_FORM_ID 0x125
#define PCH_PCIE_EQ_TX_OVERRIDE_FORM_ID 0x126

#define EFI_UEFIOPTIMIZEDBOOT_TOGGLE_ENABLE  0x1
#define EFI_UEFIOPTIMIZEDBOOT_TOGGLE_DISABLE 0x2

//
// Processor labels
//
#define PROCESSOR_HT_MODE               0x0100
#define PROCESSOR_FSB_MULTIPLIER        0x0101

#define BOOT_TIMEOUT_KEY_ID             0x1
#define BOOT_MANAGER_KEY_ID             0x2
#define BOOT_ORDER_KEY_ID               0x3
#define LEGACY_DEV_KEY_ID               0x4
#define LEGACY_ORDER_KEY_ID             0x5
#define AMI_CALLBACK_KEY_ID             0x6
#define EFI_SHELL_KEY_ID                0x7
#define ICC_KEY_ID                      0x8

#define VFR_FORMID_CPUCORE       0x83
//
// Memory labels
//
#define MEMORY_SLOT1_SPEED              0x0200
#define MEMORY_SLOT2_SPEED              0x0201
#define MEMORY_SLOT3_SPEED              0x0202
#define MEMORY_SLOT4_SPEED              0x0203
#define END_MEMORY_SLOT_SPEED           0x020F

#define VAR_EQ_USER_DECIMAL_NAME        L"85"

//
// New variables used for VFR
//
#define VAR_EQ_PORT_WIDTH_NAME          0x000A
#define VAR_EQ_PORT_WIDTH_DECIMAL_NAME  L"10"

//Boot All Options Form
#define VFR_BOOT_ALL_OPTIONS                0x3D1

// HEDT Form ID's
#define VFR_FORMID_OVERCLOCKING             0x3F0
#define MEMORY_CONFIG_FORM_ID               0x3F1
#define GT_CONFIG_FORM_ID                   0x3F2
#define CLR_CONFIG_FORM_ID                  0x3F3
#define UNCORE_CONFIG_FORM_ID               0x3F4
#define SVID_FIVR_CONFIG_FORM_ID            0x3F5

#define PROCESSOR_CONFIG_FORM_ID            0x3F6

#define SYSTEM_CONFIGURATION_KEY 0xFAFE
//BMC LAN Form
#define BMC_LAN_CONFIGURATION_DATA_PWMAX    15
#define BMC_LAN_CONFIGURATION_DATA_PWMIN    4
#define BMC_LAN_CFG_MIN_KEY_ID              0x600
#define ONBOARD_NIC_IPV4_ADDRESS_KEY_ID     0x601 // BMC_LAN_CONFIGURATION_DATA.NicIpv4.Address
#define ONBOARD_NIC_IPV4_SUBNET_MASK_KEY_ID 0x602 // BMC_LAN_CONFIGURATION_DATA.NicIpv4.SubnetMask
#define ONBOARD_NIC_IPV4_GATEWAY_KEY_ID     0x603 // BMC_LAN_CONFIGURATION_DATA.NicIpv4.Gateway
#define ONBOARD_NIC_IPV6_ADDRESS_KEY_ID     0x604 // BMC_LAN_CONFIGURATION_DATA.NicIpv6.Address
#define ONBOARD_NIC_IPV6_GATEWAY_KEY_ID     0x605 // BMC_LAN_CONFIGURATION_DATA.NicIpv6.Gateway
#define RMM_NIC_IPV4_ADDRESS_KEY_ID         0x606 // BMC_LAN_CONFIGURATION_DATA.RmmIpv4.Address
#define RMM_NIC_IPV4_SUBNET_MASK_KEY_ID     0x607 // BMC_LAN_CONFIGURATION_DATA.RmmIpv4.SubnetMask
#define RMM_NIC_IPV4_GATEWAY_KEY_ID         0x608 // BMC_LAN_CONFIGURATION_DATA.RmmIpv4.Gateway
#define RMM_NIC_IPV6_ADDRESS_KEY_ID         0x609 // BMC_LAN_CONFIGURATION_DATA.RmmIpv6.Address
#define RMM_NIC_IPV6_GATEWAY_KEY_ID         0x60a // BMC_LAN_CONFIGURATION_DATA.RmmIpv6.Gateway
#define BMC_LAN_DHCP_NAME_KEY_ID            0x60b // BMC_LAN_CONFIGURATION_DATA.DhcpName
#define BMC_LAN_CONFIGURATION_DATA_USER1    0x610 // BMC_LAN_CONFIGURATION_DATA.User1
#define BMC_LAN_CONFIGURATION_DATA_USER2    0x611 // BMC_LAN_CONFIGURATION_DATA.User1
#define BMC_LAN_CONFIGURATION_DATA_USER3    0x612 // BMC_LAN_CONFIGURATION_DATA.User1
#define BMC_LAN_CONFIGURATION_DATA_USER4    0x613 // BMC_LAN_CONFIGURATION_DATA.User1
#define BMC_LAN_CONFIGURATION_DATA_USER5    0x614 // BMC_LAN_CONFIGURATION_DATA.User1
#define BMC_LAN_CFG_MAX_KEY_ID              0x620

#define SERVER_MGMT_CLEAR_SEL               0xF100

// New Eventlog Forms
//TODO need to remove old ELOG forms
#define VFR_FORMID_EVENT_LOG            0x500
#define VFR_FORMID_EMCA_SETTING         0x501
#define VFR_FORMID_WHEA_SETTING         0x502
#define VFR_FORMID_ERROR_INJ            0x503
#define VFR_FORMID_QPI_ERROR            0x504
#define VFR_FORMID_MEM_ERROR            0x505
#define VFR_FORMID_IIO_ERROR            0x506
#define VFR_FORMID_PCI_ERROR            0x507
#define VFR_FORMID_PLATFORM_ERROR       0x508

#endif // #ifndef _CONFIGURATION_H
