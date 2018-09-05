//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SystemBoardPpi.h

  Platform Stage1 header file

**/

#ifndef _EFI_SYSTEM_BOARD_PPI_H_
#define _EFI_SYSTEM_BOARD_PPI_H_

#include <Platform.h>
#include <BoardTypes.h>
#include <Library/MmPciBaseLib.h>
#include <Register/PchRegsLpc.h>
#include <SioRegs.h>
#include <SysFunc.h>
//#include <QpiHost.h>
#include <Pi/PiHob.h>


// GUID
#include <Guid/SetupVariable.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SocketIioVariable.h>

// PPI
#include <Ppi/PchPolicy.h>
#include <Ppi/SystemBoard.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Smbus2.h>


// Library
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/MeTypeLib.h>
#include <Library/SetupLib.h>
#include <Library/GpioLib.h>
#include <Library/PchPlatformPolicyInitCommonLib.h>
#include <Library/PlatformClocksLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <GpioInitData.h>
#include <Ppi/SpiSoftStraps.h>
//
// UBA_START
//
#include <Library/UbaPlatLib.h>
#include <Library/UbaUsbOcUpdateLib.h>
//
// UBA_END
//
#include <Library/SetupLib.h>
#include "SystemBoardBifurcationSlotTables.h"

// CMOS access Port address
#define LAST_CMOS_BYTE          0x7F
#define NMI_OFF                 0x80
#define B_PCH_RTC_REGB_SRBRST   0x02  // Value to be reset to during POST
#define R_PCH_RTC_REGD          0x0D  // CMOS Register D Status
#define R_PCH_RTC_REGE          0x0E  // CMOS Register E Status
#define B_PCH_RTC_REGE_INVTIM   0x04  // CMOS invalid time found

#define TIMER1_CONTROL_PORT 0x43
#define TIMER1_COUNT_PORT   0x41
#define LOAD_COUNTER1_LSB   0x54
#define COUNTER1_COUNT      0x12
//
// Reset Generator I/O Port
//
#define RESET_GENERATOR_PORT  0xCF9

//-----------------------------------------------------------------------;
// PCH: Chipset Configuration Register Equates
//-----------------------------------------------------------------------;
#define ICH_RCRB_IRQ0                       0
#define ICH_RCRB_IRQA                       1
#define ICH_RCRB_IRQB                       2
#define ICH_RCRB_IRQC                       3
#define ICH_RCRB_IRQD                       4
#define ICH_RCRB_PIRQA                      0
#define ICH_RCRB_PIRQB                      1
#define ICH_RCRB_PIRQC                      2
#define ICH_RCRB_PIRQD                      3
#define ICH_RCRB_PIRQE                      4
#define ICH_RCRB_PIRQF                      5
#define ICH_RCRB_PIRQG                      6
#define ICH_RCRB_PIRQH                      7

//
// From WBG Soft Straps WIP.xlsx
//
#define WBG_DOWNSKU_STRAP_DSKU      0x80046000
#define WBG_DOWNSKU_STRAP_BSKU      0x8004E003
#define WBG_DOWNSKU_STRAP_TSKU      0x00044000

#define CLOCK_GENERATOR_SETTINGS_CK505       {0x00, 0xF3, 0x0F, 0xFE, 0x98, 0x02, 0x08, 0x26, 0x7C, 0xE7, 0x0F, 0xFE, 0x08}

#define PCHSTRAP_9   9
#define PCHSTRAP_10  10
#define PCHSTRAP_16  16
#define PCHSTRAP_17  17

#define RESET_PORT                0x0CF9
#define CLEAR_RESET_BITS          0x0F1
#define COLD_RESET                0x02  // Set bit 1 for cold reset
#define RST_CPU                   0x04  // Setting this bit triggers a reset of the CPU
#define FULL_RESET                0x08  // Set bit 4 with bit 1 for full reset

//
// PPI functions
//
VOID
PchPlatformPolicyInit (
  IN PCH_POLICY_PPI *PchPlatformPolicyPpi

);
VOID
GetUsbConfig (
  OUT VOID **Usb20OverCurrentMappings,
  OUT VOID **Usb30OverCurrentMappings,
  OUT VOID **Usb20AfeParams
  );

VOID
 InstallPlatformHsioPtssTable(
  PCH_RC_CONFIGURATION         *PchSetup,
  PCH_POLICY_PPI               *PchPolicy
);


VOID
GetBw5Id (
  IN IIO_GLOBALS *IioGlobalData,
  IN  OUT BW5_BIFURCATION_DATA_STRUCT        *Bw5Id
  );

VOID
SetBifurcations(
    IN OUT IIO_GLOBALS *IioGlobalData,
    IN     IIO_BIFURCATION_ENTRY *BifurcationTable,
    IN     UINT8                  BifurcationEntries
);

VOID
EnableHotPlug (
    IN OUT IIO_GLOBALS *IioGlobalData,
    IN UINT8 Port,
    IN UINT8 VppPort,
    IN UINT8 VppAddress,
    IN UINT8 PortOwnership
  );


VOID
ConfigSlots (
    IN OUT IIO_GLOBALS        *IioGlobalData,
    IN IIO_SLOT_CONFIG_ENTRY  *Slot,
    IN UINT8                  SlotEntries
  );

VOID
OverrideConfigSlots (
    IN OUT IIO_GLOBALS        *IioGlobalData,
    IN IIO_SLOT_CONFIG_ENTRY  *Slot,
    IN UINT8                  SlotEntries
  );
  
VOID
CalculatePEXPHideFromIouBif (
    IN UINT8 Iou,
    IN UINT8 IioIndex,
    IN OUT IIO_GLOBALS *IioGlobalData
);

VOID
DumpIioConfiguration(
    IN UINT8 iio,
    IN IIO_GLOBALS *IioGlobalData
);

VOID
OverrideDefaultBifSlots(
    IN IIO_GLOBALS *IioGlobalData,
    IN UINT8       BoardId
);

UINT8
GetUplinkPortInformationCommon (
    IN UINT8 BoardId,
    IN UINT8 IioIndex
);

VOID
SystemIioPortBifurcationInitCommon (
    UINT8 BoardId,
    IIO_GLOBALS *IioGlobalData,
    IIO_BIFURCATION_ENTRY         **BifurcationTable,
    UINT8                         *BifurcationEntries,
    IIO_SLOT_CONFIG_ENTRY         **SlotTable,
    UINT8                         *SlotEntries
);

VOID
SystemHideIioPortsCommon(
    IIO_GLOBALS *IioGlobalData,
    UINT8       IioIndex
);

UINT8
GetUplinkPortInformation (
    IN UINT8 IioIndex
);

VOID
SystemIioPortBifurcationInit (
  IN IIO_GLOBALS  *IioGlobalData
  );

EFI_STATUS
OutputDataToPlatformLcd (
    IN     CONST EFI_PEI_SERVICES               **PeiServices
  );

VOID
HandleBootMode (
  IN EFI_BOOT_MODE          BootMode
  );

EFI_STATUS
LanEarlyInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN SYSTEM_CONFIGURATION       *SystemConfiguration
  );

UINT32
FeaturesBasedOnPlatform (
  VOID
  );

EFI_STATUS
ConfigurePlatformClocks (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *Ppi
 );

EFI_STATUS
HandleAcPowerRecovery (
  IN EFI_PEI_SERVICES	   **PeiServices
  );

UINT8
SystemBoardIdValue (VOID);

UINT16
SystemBoardIdSkuValue (VOID);

UINT32
SystemBoardRevIdValue (VOID);
UINT8
GetBmcPciePort(VOID);
BOOLEAN
PcieSataPortSelectionWA(VOID);



EFI_STATUS
GetFpgaHssiCardBbsInfo (
  IN  UINT8       SocketNum,
  OUT UINT8       *HssiCardBbsIndex,
  OUT UINT8       *HssiCardID
  );

EFI_STATUS
SetFpgaGpioOff (UINT8 SocketNum);

UINT8
GetDefaultFpgaBbsIndex (UINT8 SocketNum);

VOID
SystemPchUsbInitRecovery (
  IN VOID                      *OverCurrentMappings,
  IN UINTN                     *OverCurrentMappingsSize,
  IN VOID                      *PortLength
);

VOID
PchGpioInit (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  );

#endif
