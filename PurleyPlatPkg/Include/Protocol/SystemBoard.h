//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2005-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SystemBoard.h

  This protocol is EFI compatible.


**/


#ifndef _DXE_SYSTEM_BOARD_H_
#define _DXE_SYSTEM_BOARD_H_

#include <PlatPirqData.h>
#include <PlatDevData.h>
#include <IioPlatformData.h>
#include <KtiHost.h>
#include <Ppi/PchPolicy.h>
#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <Guid/SocketIioVariable.h>
#include <Guid/PlatformInfo.h>
#include <IndustryStandard/Acpi.h>

#define PCI_DEVICE_NUMBER_IMC0_CH_0     0x08
#define PCI_FUNCTION_NUMBER_IMC0_CH_0   0
#define PCI_DEVICE_ID_IMC0_CH_0         0x2014
#define BIOSGUARD_SUPPORT_ENABLED BIT0
#define OC_SUPPORT_ENABLED   BIT1

#ifndef __AML_OFFSET_TABLE_H
#define __AML_OFFSET_TABLE_H

typedef struct {
    char                   *Pathname;      /* Full pathname (from root) to the object */
    unsigned short         ParentOpcode;   /* AML opcode for the parent object */
    unsigned long          NamesegOffset;  /* Offset of last nameseg in the parent namepath */
    unsigned char          Opcode;         /* AML opcode for the data */
    unsigned long          Offset;         /* Offset for the data */
    unsigned long long     Value;          /* Original value of the data (as applicable) */
} AML_OFFSET_TABLE_ENTRY;
#endif

struct sysHost;

///
/// Global ID for EFI_DXE_BOARD_INFO_PPI
///
//
//#define EFI_DXE_BOARD_INFO_GUID \
// { 0xa57c1118, 0x6afc, 0x46d2, { 0xba, 0xe6, 0x92, 0x92, 0x62, 0xd3, 0xeb, 0x1e } };

///
/// The forward declaration for SYSTEM_BOARD_PROTOCOL.
///
typedef struct _SYSTEM_BOARD_PROTOCOL  SYSTEM_BOARD_PROTOCOL;

/**
DXE_GET_USB_CONFIG

  GetUsbConfig gets the USB platform configuraiton.

  @param *SystemBoard - This SystemBoard protocol
  @param *Usb20OverCurrentMappings,
  @param *Usb30OverCurrentMappings,

  @retval *Usb20OverCurrentMappings,
  @retval *Usb30OverCurrentMappings,

**/
typedef
VOID
 (EFIAPI DXE_GET_USB_CONFIG) (
  IN SYSTEM_BOARD_PROTOCOL *This,
  OUT VOID **Usb20OverCurrentMappings,
  OUT VOID **Usb30OverCurrentMappings,
  OUT VOID **Usb20AfeParams
  );

/**

  SystemIioPortBifurcationInit is used to handle updating the UDS datastructure with DXE IIO init variables.
  SLOT config data
  Bifurcation config data

  @param *mSB - pointer to this protocol

  @retval *IioUds updated with SLOT and Bifurcation information updated.

**/
typedef
VOID
  (EFIAPI DXE_SYSTEM_IIO_PORT_BIF_INIT) (
  IN SYSTEM_BOARD_PROTOCOL *This,
  IN IIO_GLOBALS *IioGlobalData
  );

/**


  GetUplinkPortInformation is used to get board based uplink port information

  @param IioIndex - Socket ID

  @retval Port Index for uplink.

**/
typedef
UINT8
  (EFIAPI DXE_GET_UPLINK_PORT_INFORMATION) (
  IN SYSTEM_BOARD_PROTOCOL *This,
  IN UINT8 IioIndex
  );
  
/**

  SetIioPortDisableMap


  @param *This - pointer to this protocol
  @param Iio - Cpu socket number index
  @param *IioGlobalData Pointer to IIO_GLOBAL structure

  @retval IioPortDisableBitMap

**/
typedef
VOID
  (EFIAPI DXE_IIO_PORT_DISABLE_MAP) (
  IN SYSTEM_BOARD_PROTOCOL *This,
  IN  UINTN     Iio,
  IN IIO_GLOBALS *IioGlobalData,
  OUT UINT32    *IioPortDisableBitMap
  );

/**

  PlatformThermalDeviceStatus is used to Update the SetupVariables

  @param *SetupVariable - Pointer to the setup variable to be updated

  @retval ThermalDevice Enable\Disable

**/
typedef
UINT8
 (EFIAPI DXE_GET_THERMAL_DEVICE_STATUS) (
  IN PCH_RC_CONFIGURATION                       *SetupVariables
);

/**

  UpdateIioDefaultConfig


  @param *Default

  @retval None

**/
typedef
VOID
 (EFIAPI DXE_UPDATE_IIO_DEFAULT_CONFIG) (
  IN SYSTEM_CONFIGURATION              *Default
);

/**

  UpdateSystemAcpiTable is used to Update the Platform based ACPI table updates


  @param *This - pointer to this protocol
  @param *TableHeader - ACPI Table header pointer
  @param *mAmlOffsetTablePointer - Aml Offser Table Entry

  @retval EFI_STATUS

**/
typedef
EFI_STATUS
 (EFIAPI DXE_UPDATE_SYSTEM_ACPI_TABLE) (
  IN EFI_ACPI_DESCRIPTION_HEADER                *TableHeader,
  IN AML_OFFSET_TABLE_ENTRY                     **mAmlOffsetTablePointer
);

/**

    FeaturesBasedOnPlatform returns feature flag based on Platform ID

    @param VOID

    @retval PlatformFlags

**/
typedef
UINT32
 (EFIAPI DXE_FEATURES_BASED_ON_PLATFORM) (VOID);

/**

  UpdateCpuSmbiosData is used to Update Cpu Smbios data


  @param *This - pointer to this protocol
  @param *CpuSocketCount - Socketcount


  @retval None

**/
typedef
VOID
 (EFIAPI DXE_UPDATE_CPU_SMBIOS_DATA) (
  IN UINT32				                    	*CpuSocketCount
);

/**

  Update XHCI Table OEM ID

  @param *This - pointer to this protocol
  @param *XhciAcpiTable = XHCI ACPI Table pointer


  @retval BOOLEAN

**/
typedef
BOOLEAN
 (EFIAPI DXE_UPDATE_OEM_TABLE_ID_XHCI) (
  IN UINT64                             *XhciAcpiTable
);

/*
Routine Description:
  Gets KTI Link EParam table pointer and size

Arguments:
  Pointer to KTI Eparam Table
  Size of KTI Eparam Table
  
Returns:
  VOID
*/
typedef
VOID
(EFIAPI DXE_GET_KTI_LANE_EPARAM) (
  ALL_LANES_EPARAM_LINK_INFO **Ptr,
  UINT32 *Size,
  struct sysHost *host
);

/**

  Get the board type bitmask.
    Bits[3:0]   - Socket0
    Bits[7:4]   - Socket1
    Bits[11:8]  - Socket2
    Bits[15:12] - Socket3
    Bits[19:16] - Socket4
    Bits[23:20] - Socket5
    Bits[27:24] - Socket6
    Bits[31:28] - Socket7

  Within each Socket-specific field, bits mean:
    Bit0 = CPU_TYPE_STD support; always 1 on Socket0
    Bit1 = CPU_TYPE_F support
    Bit2 = CPU_TYPE_P support
    Bit3 = reserved

  @param host - pointer to syshost

  @retval board type bitmask

**/
typedef
UINT32
(EFIAPI DXE_GET_BOARD_TYPE_BITMASK) (
  struct sysHost *host);

/**

  SystemPcieRootPortCheck


  @param  Bus
  @param  PcieSlotOpromBitMap


  @retval Boolean

**/
typedef
BOOLEAN
 (EFIAPI DXE_SYSTEM_PCIE_ROOT_PORT_CHECK) (
  IN  UINTN                             Bus,
  IN  UINT32                            PcieSlotOpromBitMap
);

/**

  SystemSetupPcieSlotNumber

  @param *PcieSlotItemCtrl


  @retval None

**/
typedef
VOID
 (EFIAPI DXE_SYSTEM_SETUP_PCIE_SLOT_NUMBER) (
    OUT  UINT8                   *PcieSlotItemCtrl
);

/**

  UpdatePlatUsbSettings

  @param *SystemConfiguration  pointer to SYSTEM_CONFIGURATION structure


  @retval None

**/
typedef
VOID
 (EFIAPI DXE_UPDATE_PLAT_USB_SETTINGS) (
  IN SYSTEM_CONFIGURATION             *SystemConfiguration,
  IN PCH_RC_CONFIGURATION             *PchConfiguration

  );

/**

  SystemAssertPostGpio

  @param None


  @retval None

**/
typedef
VOID
 (EFIAPI DXE_SYSTEM_ASSERT_POST_GPIO) (VOID);


/**

  InitPlatformName update the platform name string

  @param *PlatformName  Pointer variable to update the platform name

  @retval VOID

**/
typedef
VOID
 (EFIAPI DXE_INIT_PLATFORM_NAME) (
  IN   CHAR16                          *PlatformName
);

/**

  UpdateMcuInfo provides if MCU is supported

  @param VOID

  @retval None

**/
typedef
VOID
 (EFIAPI DXE_UPDATE_MCU_INFO) (VOID);

/**

  SystemBoardIdValue returns board ID value
  @param VOID

  @retval Boolean

**/
typedef
UINT8
 (EFIAPI DXE_SYSTEM_BOARD_ID_VALUE) (VOID);
 
 /**

  SystemBoardIdSkuValue returns board ID Subtype value
  @param VOID

  @retval Boolean

**/
typedef
UINT16
 (EFIAPI DXE_SYSTEM_BOARD_ID_SKU_VALUE) (VOID);

//
// Global variables for Option ROMs
//
#define NULL_ROM_FILE_GUID \
  { \
    0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
  }

//
// {7301762E-4BF3-4b06-ADA0-766041BE36AF}
//
#define ONBOARD_VIDEO_OPTION_ROM_FILE_GUID \
  { \
    0x7301762e, 0x4bf3, 0x4b06, 0xad, 0xa0, 0x76, 0x60, 0x41, 0xbe, 0x36, 0xaf \
  }


//
// {688021EF-3A1B-468d-80D8-A888674B1B42}
//
#define ASPEED_VIDEO_OPTION_ROM_FILE_GUID \
  { \
    0x688021ef, 0x3a1b, 0x468d, 0x80, 0xd8, 0xa8, 0x88, 0x67, 0x4b, 0x1b, 0x42 \
  }

//
// {A6F63D76-6975-4144-96B3-26D624189B2B}
//
#define PXE_UNDI_OPTION_ROM_FILE_GUID \
  { \
    0xa6f63d76, 0x6975, 0x4144, 0x96, 0xb3, 0x26, 0xd6, 0x24, 0x18, 0x9b, 0x2b \
  }

#define PXE_BASE_OPTION_ROM_FILE_GUID \
  { \
    0x6f2bc426, 0x8d83, 0x4f17, 0xbe, 0x2c, 0x6b, 0x1f, 0xcb, 0xd1, 0x4c, 0x80 \
  }

//
// {7A39517D-DD10-4abb-868F-B859FBBBBA87}
//
#define IDE_RAID_OPTION_ROM_FILE_GUID \
  { \
    0x7a39517d, 0xdd10, 0x4abb, 0x86, 0x8f, 0xb8, 0x59, 0xfb, 0xbb, 0xba, 0x87 \
  }

#define BARTON_HILLS_DEVICE_ID 0x150E

#define SATA_RAID_OPTION_ROM_FILE_GUID \
  { \
    0xbd5d4ca5, 0x674f, 0x4584, 0x8c, 0xf9, 0xce, 0x4e, 0xa1, 0xf5, 0x4d, 0xd1 \
  }
#define SSATA_RAID_OPTION_ROM_FILE_GUID \
  { \
    0x2CC25173, 0xBD9F, 0x4C89, 0x89, 0xCC, 0x29, 0x25, 0x6A, 0x3F, 0xD9, 0xC3 \
  }

typedef struct {
  EFI_GUID  FileName;
  UINTN     Segment;
  UINTN     Bus;
  UINTN     Device;
  UINTN     Function;
  UINT16    VendorId;
  UINT16    DeviceId;
} PCI_OPTION_ROM_TABLE;

//
// System board information table
//
typedef struct {
  //
  // Pci option ROM data
  //
  PCI_OPTION_ROM_TABLE          *PciOptionRomTable;

  //
  // System CPU data
  //
  UINT32                        CpuSocketCount;

  //
  // System device and irq routing data
  //
  DEVICE_DATA                   *DeviceData;
  DEVICE_UPDATE_DATA            *DeviceUpdateData;
  PLATFORM_PIRQ_DATA            *SystemPirqData;
} DXE_SYSTEM_BOARD_INFO;

typedef
struct _SYSTEM_BOARD_PROTOCOL {
    DXE_SYSTEM_BOARD_INFO                *SystemBoardInfo;       // System board information
    DXE_GET_USB_CONFIG                   *GetUsbConfig;          // Get the platforms USB configuration
    DXE_SYSTEM_IIO_PORT_BIF_INIT         *SystemIioPortBifurcationInit;   // Update OEM IIO Port Bifurcation based on PlatformConfiguration
    DXE_GET_UPLINK_PORT_INFORMATION      *GetUplinkPortInformation;   // Get Uplink Port information
    //DXE_IIO_PORT_DISABLE_MAP           *SetIioPortDisableMap;  //
    DXE_GET_THERMAL_DEVICE_STATUS        *PlatformThermalDeviceStatus;
    DXE_UPDATE_IIO_DEFAULT_CONFIG        *UpdateIioDefaultConfig;
    DXE_UPDATE_SYSTEM_ACPI_TABLE         *UpdateSystemAcpiTable;
    DXE_UPDATE_CPU_SMBIOS_DATA           *UpdateCpuSmbiosData;
    DXE_SYSTEM_PCIE_ROOT_PORT_CHECK      *SystemPcieRootPortCheck;
    DXE_SYSTEM_SETUP_PCIE_SLOT_NUMBER    *SystemSetupPcieSlotNumber;
    DXE_SYSTEM_ASSERT_POST_GPIO          *SystemAssertPostGpio;
    DXE_INIT_PLATFORM_NAME               *InitPlatformName;
    DXE_UPDATE_MCU_INFO                  *UpdateMcuInfo;
    DXE_FEATURES_BASED_ON_PLATFORM       *FeaturesBasedOnPlatform;
    DXE_SYSTEM_BOARD_ID_VALUE            *SystemBoardIdValue;
    DXE_SYSTEM_BOARD_ID_SKU_VALUE        *SystemBoardIdSkuValue;
    DXE_UPDATE_PLAT_USB_SETTINGS         *UpdatePlatUsbSettings;
    DXE_UPDATE_OEM_TABLE_ID_XHCI         *UpdateOemTableIdXhci;
};

extern EFI_GUID gEfiDxeSystemBoardProtocolGuid;

#endif
