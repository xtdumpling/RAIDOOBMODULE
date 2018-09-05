/** @file
***** Dell designed file (Proposal for Intel to add this for PlatformPkg forward) *****

  Provides the services required to access system board information.

 * Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Revision Reference:
  This PPI is defined in UEFI Platform Initialization Specification 1.2 Volume 1:
  Pre-EFI Initalization Core Interface.

**/


#ifndef _PEI_SYSTEM_BOARD__H_
#define _PEI_SYSTEM_BOARD__H_

#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <Pi/PiBootMode.h>
#include <Platform.h>
#include <Ppi/SpiSoftStraps.h>
#include <SioRegs.h>
#include <SysHost.h>
#include <Ppi/PchPolicy.h>
#include <Library/GpioLib.h>

#define BIOSGUARD_SUPPORT_ENABLED BIT0
#define OC_SUPPORT_ENABLED   BIT1

///
/// FPGA Hssi Card slot inforamtion on board
///
typedef struct {
  UINT8           BoardId;                  // Board ID
  UINT8           FpgaSktIndex;             // Socket index for the FPGA which have the HSSI card slot
  GPIO_PAD        GpioHssiCardID0;          // Gpio Number of RC_CARD_ID[0]
  GPIO_PAD        GpioHssiCardID1;          // Gpio Number of RC_CARD_ID[1]
  GPIO_PAD        GpioHssiCardID2;          // Gpio Number of RC_CARD_ID[2]
} FPGA_HSSI_DATA;

typedef union {
  struct {
    UINT8 CardID0:1;
    UINT8 CardID1:1;
    UINT8 CardID2:1;
  } Bits;
  UINT8 Data;
} FPGA_HSSI_CARD_ID;

///
/// FPGA Hssi Card slot inforamtion on board
///
typedef struct {
  UINT8           BoardId;                  // Board ID
  UINT8           HssiCardID;             // HSSI card ID
  UINT8           BBStype;                // Blue Bit Stream to be loaded
} FPGA_HSSI_BBS_Map;

///
/// FPGA Blue Bit Stream Type
///
typedef enum {
  BBS_Gbe = 0,      
  BBS_Pcie,     
  BBS_None    = 0xff
} FPGA_BLUE_BIT_STREAM;


///
/// GPIO table for FPGA socket power off
///
typedef struct {
  UINT8           BoardId;                  // Board ID
  UINT8           FpgaSktIndex;             // Socket index for the FPGA which have the HSSI card slot
  GPIO_PAD        FpgaPowerOffGpio;         // Blue Bit Stream to be loaded
} FPGA_SOCKET_POWEROFF_MAP;


///
/// FPGA Blue Bit Stream Platfrom default map
///
typedef struct {
  UINT8           BoardId;                  // Board ID
  UINT8           FpgaSktIndex;             // Socket index for the FPGA which have the HSSI card slot
  UINT8           Bbstype;                  // Blue Bit Stream to be loaded
} FPGA_PLATFORM_BBS_MAP;

///
/// The forward declaration for SYSTEM_BOARD_INFO_PPI.
///
typedef struct _SYSTEM_BOARD_PPI  SYSTEM_BOARD_PPI;

typedef
VOID
(EFIAPI *PEI_PCH_PLATFORM_POLICY_INIT) (
  IN PCH_POLICY_PPI *PchPlatformPolicyPpi
  );



/**
PEI_GET_USB_CONFIG

  GetUsbConfig gets the USB platform configuraiton.

  @param *Usb20OverCurrentMappings,
  @param *Usb30OverCurrentMappings,
  @param *UsbPortLengthTable,
  @param *UsbEyeDiagramParams

  @retval *Usb20OverCurrentMappings,
  @retval *Usb30OverCurrentMappings,
  @retval *UsbPortLengthTable,
  @retval *UsbEyeDiagramParams

**/

typedef
VOID
 (EFIAPI *PEI_GET_USB_CONFIG) (
  OUT VOID **Usb20OverCurrentMappings,
  OUT VOID **Usb30OverCurrentMappings,
  OUT VOID **Usb20AfeParams
  );

typedef
VOID
 (EFIAPI *PEI_PCH_INSTALL_PTSS_TABLE) (
  IN          PCH_RC_CONFIGURATION         *PchSetup,
  IN OUT      PCH_POLICY_PPI               *PchPolicy
);


/**

  SystemIioPortBifurcationInit is used to updating the IIO_GLOBALS Data Structure with IIO
  SLOT config data
  Bifurcation config data

  @param *mSB - pointer to this protocol

  @retval *IioUds updated with SLOT and Bifurcation information updated.

**/
typedef
VOID
  (EFIAPI *PEI_SYSTEM_IIO_PORT_BIF_INIT) (
    IN IIO_GLOBALS *IioGlobalData
  );
/**

  GetUplinkPortInformation is used to get board based uplink port information

  @param IioIndex - Socket ID

  @retval PortIndex for uplink.

**/
typedef
UINT8
  (EFIAPI *PEI_GET_UPLINK_PORT_INFORMATION) (
    IN UINT8 IioIndex
  );
/**
PEI_OUTPUT_TO_PLATFORM_LCD

   OutputDataToPlatformLcd outputs data to the platform LCD.

  @param **PeiServices - PEI services

  @retval EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *PEI_OUTPUT_TO_PLATFORM_LCD) (
  IN CONST EFI_PEI_SERVICES **PeiServices
);

typedef
EFI_STATUS
(EFIAPI *PEI_LAN_EARLY_INIT) (
  IN EFI_PEI_SERVICES **PeiServices,
  //
  // PCH_VARIABLE
  // Add the PCH Variable till we move to PCH pkg
  //
  IN SYSTEM_CONFIGURATION *SystemConfiguration
);

/*
PEI_HANDLE_BOOT_MODE

  HandleBootMode is here to allow the platform to make any system board updates, etc... for a given boot mode.

  @param *SystemBoard - This PPI
  @param BootMode - Boot mode variable, which contains the current boot mode.

*/
typedef
VOID
 (EFIAPI *PEI_HANDLE_BOOT_MODE) (
  IN EFI_BOOT_MODE    BootMode
);

typedef
EFI_STATUS
 (EFIAPI *PEI_HANDLE_AC_POWER_RECOVERY) (
  IN EFI_PEI_SERVICES  **PeiServices
  );


/*
PEI_CONFIGURE_PLATFORM_CLOCK

  HandleClock is used to update any specific clock data bytes before it is written to the
  clock generator.

  @param *SystemBoard - This PPI
  @param *ClockData - Points to the clock data read from clock generator chip.

  @retval Any updates to the clock data that will be written to the clock generator.

*/
typedef
EFI_STATUS
 (EFIAPI *PEI_CONFIGURE_PLATFORM_CLOCK) (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *SmbusPpi
);

/*
Routine Description:
  This routine is used to get the BoardId value

Arguments:
  VOID

Returns:
  BoardId
*/
typedef
UINT8
(EFIAPI *PEI_SYSTEM_BOARD_ID) (VOID);

/*
Routine Description:
  This routine is used to get the BoardId value

Arguments:
  VOID

Returns:
  BoardId
*/
typedef
UINT16
(EFIAPI *PEI_SYSTEM_BOARD_ID_SKU) (VOID);

/*
Routine Description:
  This routine is used to get the BoardRevId value

Arguments:
  VOID

Returns:
  BoardRevId
*/
typedef
UINT32
(EFIAPI *PEI_SYSTEM_BOARD_REV_ID) (VOID);



/*++
Routine Description:
  Used to update USB Current Mappings

Arguments:
  *OverCurrentMappings,
  *OverCurrentMappings,
  *PortLength

Returns:
  VOID


--*/
typedef
VOID
 (EFIAPI *PEI_PCH_USB_INIT_RECOVERY) (
  IN VOID                      *OverCurrentMappings,
  IN UINTN                     *OverCurrentMappingsSize,
  IN VOID                      *PortLength
);

typedef
VOID
 (EFIAPI *PEI_PCH_GPIO_INIT) (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
);

typedef
UINT32
 (EFIAPI *PEI_FEATURES_BASED_ON_PLATFORM) (VOID);

typedef struct _SOFTSTRAP_FIXUP_ENTRY
{
  UINT8  BoardId;
  UINT8  SoftStrapNumber;
  UINT8  BitfieldOffset;
  UINT8  BitfieldLength;
  UINT32 Value;
} SOFTSTRAP_FIXUP_ENTRY;

/**

  Force BMC PCIE port to Gen1

  @param *This - pointer to this protocol

  @retval Port Number

**/
typedef
UINT8
 (EFIAPI *PEI_GET_BMC_PCIE_PORT) (
  VOID
);

/**

  Is PCIE SATA Port Selection WA required

  @param *This - pointer to this protocol

  @retval BOOLEAN

**/
typedef
BOOLEAN
 (EFIAPI *PEI_PCIE_SATA_PORT_SELECTION_WA) (
  VOID
);

/**
  Gets FPGA BBS index and card ID according to the HSSI card type

  @param SocketNum           Socket Number
  @param HssiCardBbsIndex    HSSI card BBS Index
  @param HssiCardID          HSSI card ID

  @retval EFI_SUCCESS:       The function executed successfully
  @retval Others             Internal error when execute the function

**/
typedef
EFI_STATUS
(EFIAPI *PEI_GET_FPGA_HSSICARD_BBS_INFO) (
  UINT8 SocketNum,
  UINT8 *HssiCardBbsIndex,
  UINT8 *HssiCardID
  );

/*
  Set FPGA Power Control GPIO

  @param SocketNum           Socket Number
    
  @retval EFI_SUCCESS        Operation completed successfully.
  @retval Otherwise          Operation failed.

*/
typedef
EFI_STATUS
(EFIAPI *PEI_SET_FPGA_POWER_GPIO) (
  UINT8 SocketNum
  );

/**
  Gets FPGA default BBS index 

  @param SocketNum           Socket Number

  @retval  Bbs Index

**/
typedef
UINT8
(EFIAPI *PEI_GET_DEFAULT_BBS_INDEX) (
   UINT8 SocketNum
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
(EFIAPI *PEI_GET_KTI_LANE_EPARAM) (
  ALL_LANES_EPARAM_LINK_INFO **Ptr,
  UINT32 *Size,
  struct sysHost    *host
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
(EFIAPI *PEI_GET_BOARD_TYPE_BITMASK) (
  struct sysHost *host);



//
// System board information table
//
typedef struct {
  //
  // Gpio data
  //
  //VOID                    *mSystemGpioInitData;        // VOID type to allow for platform type casting override.

  //
  // Pch softstrap fixup table
  //
  SOFTSTRAP_FIXUP_ENTRY   *SoftstrapFixupTable;

  //
  // SIO initialization table
  //
  UINT8                   SioIndexPort;               // SIO Index Port value
  UINT8                   SioDataPort;                // SIO Data Port value
  SIO_INDEX_DATA          *mSioInitTable;             // SIO init table
  UINT8                   NumSioItems;                // Number of items in the SIO init table.
} PEI_SYSTEM_BOARD_INFO;


struct _SYSTEM_BOARD_PPI {
  PEI_SYSTEM_BOARD_INFO             *SystemBoardInfo;       // System board information
  PEI_PCH_PLATFORM_POLICY_INIT      PchPlatformPolicyInit;
  PEI_GET_USB_CONFIG                GetUsbConfig;           // Get the platforms USB configuration
  PEI_SYSTEM_IIO_PORT_BIF_INIT      SystemIioPortBifurcationInit; // Update OEM IIO Port Bifurcation based on PlatformConfiguration
  PEI_GET_UPLINK_PORT_INFORMATION   GetUplinkPortInformation; // Get Uplink port information
  PEI_OUTPUT_TO_PLATFORM_LCD        OutputDataToPlatformLcd;// Output data to the platform LCD
  PEI_LAN_EARLY_INIT                LanEarlyInit;           // Initialize LAN
  PEI_HANDLE_BOOT_MODE              HandleBootMode;         // Handle any special boot mode changes for the system.
  PEI_CONFIGURE_PLATFORM_CLOCK      ConfigurePlatformClocks;
  PEI_HANDLE_AC_POWER_RECOVERY      HandleAcPowerRecovery;
  PEI_SYSTEM_BOARD_ID               SystemBoardIdValue;
  PEI_SYSTEM_BOARD_ID_SKU           SystemBoardIdSkuValue;  // BoardId Subtype
  PEI_PCH_USB_INIT_RECOVERY         SystemPchUsbInitRecovery;
  PEI_FEATURES_BASED_ON_PLATFORM    FeaturesBasedOnPlatform;
  PEI_PCH_GPIO_INIT                 PchGpioInit;
  PEI_PCH_INSTALL_PTSS_TABLE        InstallPlatformHsioPtssTable;
  PEI_SYSTEM_BOARD_REV_ID           SystemBoardRevIdValue;
  PEI_GET_BMC_PCIE_PORT             GetBmcPciePort;
  PEI_PCIE_SATA_PORT_SELECTION_WA   PcieSataPortSelectionWA;
  PEI_GET_FPGA_HSSICARD_BBS_INFO    GetFpgaHssiCardBbsInfo;
  PEI_SET_FPGA_POWER_GPIO           SetFpgaGpioOff;
  PEI_GET_DEFAULT_BBS_INDEX         GetDefaultFpgaBbsIndex;
};

extern EFI_GUID gEfiPeiSystemBoardPpiGuid;

#endif
