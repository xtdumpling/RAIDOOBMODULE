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


  @file SystemBoardDxe.h

  Platform Stage1 header file

**/

#ifndef _EFI_SYSTEM_BOARD_DXE_H_
#define _EFI_SYSTEM_BOARD_DXE_H_

#include "Platform.h"
#include <BoardTypes.h>
#include <KtiHost.h>

// GUID
#include <Guid/HobList.h>
#include <Guid/SetupVariable.h>
#include <Guid/GlobalVariable.h>
#include <Guid/SocketIioVariable.h>

// Protocol
#include <Protocol/TcgService.h>
#include <Protocol/CpuIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/IsaIo.h>
#include <Protocol/Ps2Policy.h>
#include <Protocol/PlatformPolicy.h>
#include <Ppi/PchPolicy.h>
#include <Protocol/SystemBoard.h>
#include <Protocol/SmbusHc.h>

// Library
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PchPlatformPolicyInitCommonLib.h>
#include <Library/SmbusLib.h>
#include <Library/MeTypeLib.h>
#include <Library/SetupLib.h>
//
// UBA_START
//
#include <Library/UbaPlatLib.h>
#include <Library/UbaUsbOcUpdateLib.h>
//
// UBA_END
//

#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>

#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/Pci30.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Change the name as per AMI build
//#include "EPRPPlatform.offset.h"
//#include "EXRPPlatform.offset.h"
#include "DsdtAsl.offset.h"
// APTIOV_SERVER_OVERRIDE_RC_END : Change the name as per AMI build

#include <IndustryStandard/LegacyBiosMpTable.h>
#include <IndustryStandard/Pci.h>
#include <Guid/SocketVariable.h>

#include "SystemBoardBifurcationSlotTables.h"

#define DXE_DEVICE_ENABLED  1
#define DXE_DEVICE_DISABLED 0

#define GP32_BIOS_POST_CMPLT BIT0

typedef union {

  UINT32 PcieSlotInfo;

  struct {
    UINT32 RtPrtFunNum : 8;
    UINT32 RtPrtDevNum : 8;
    UINT32 CpuNum : 8;
    UINT32 Dummy : 8;
    /* Slot definition for the root ports of Purley CRBs
     */
  } RootPrt;
} PLATFORM_PCIE_SLOT_STRUCT;

//
// Describes PCI-E slot location with each SRP (SKX_TODO: Need to be updated for NeonCity and Lightning Ridge)
//
PLATFORM_PCIE_SLOT_STRUCT  MYEPRP [] =  {0x00000300,  //Slot 1
                                         0x00800100,  //Slot 3
                                         0x00800200,  //Slot 4
                                         0x00800302,  //Slot 6
                                         0x00000200,  //Slot 10
                                         0x00000100,  //Slot 11
                                         0x00001C05,  //Slot 12
                                         0x00800300   //Slot 14
                                        };
PLATFORM_PCIE_SLOT_STRUCT  MYEXRP [] =  {0x00000300,  //Slot 1
                                         0x00800100,  //Slot 3
                                         0x00800200,  //Slot 4
                                         0x00800302,  //Slot 6
                                         0x00000200,  //Slot 10
                                         0x00000100,  //Slot 11
                                         0x00001C05,  //Slot 12
                                         0x00800300   //Slot 14
                                        };

//
// SystemBoard protocol level definitions
//
VOID
GetUsbConfig (
  IN SYSTEM_BOARD_PROTOCOL *mSystemBoard,
  OUT VOID                 **Usb20OverCurrentMappings,
  OUT VOID                 **Usb30OverCurrentMappings,
  OUT VOID                 **Usb20AfeParams
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
    IN SYSTEM_BOARD_PROTOCOL *mSB,
    IN UINT8 IioIndex
);

VOID
SystemIioPortBifurcationInit (
  IN SYSTEM_BOARD_PROTOCOL *mSB,
  IIO_GLOBALS  *IioGlobalData
  );
/*
VOID
SetIioPortDisableMap(
  IN SYSTEM_BOARD_PROTOCOL *mSB,
  IN  UINTN     Iio,
  IN IIO_GLOBALS *IioGlobalData,
  OUT UINT32    *IioPortDisableBitMap
  );
*/

UINT8
PlatformThermalDeviceStatus (
  IN PCH_RC_CONFIGURATION                       *SetupVariables
  );

VOID
UpdateIioDefaultConfig (
  IN SYSTEM_CONFIGURATION                      *Default
);

UINT32
FeaturesBasedOnPlatform (
  VOID
);

EFI_STATUS
UpdateSystemAcpiTable (
  IN EFI_ACPI_DESCRIPTION_HEADER                *TableHeader,
  IN AML_OFFSET_TABLE_ENTRY                     **mAmlOffsetTablePointer
);

VOID
UpdatePlatUsbSettings (
  IN SYSTEM_CONFIGURATION *SystemConfiguration,
  IN PCH_RC_CONFIGURATION *PchConfiguration
);

BOOLEAN
UpdateOemTableIdXhci (
  IN UINT64                             *XhciAcpiTable
);

VOID
UpdateCpuSmbiosData (
  IN UINT32					*CpuSocketCount
);

BOOLEAN
SystemPcieRootPortCheck (
  IN  UINTN                            Bus,
  IN  UINT32                           PcieSlotOpromBitMap
);

VOID
SystemSetupPcieSlotNumber (
  OUT  UINT8                          *PcieSlotItemCtrl
);

VOID
SystemAssertPostGpio (
  VOID
);

VOID
InitPlatformName (
  OUT   CHAR16                        *PlatformName
);

VOID
PlatformAssertPostGpio (
  EFI_EVENT  Event,
  VOID       *Context
  );

UINT8
SystemBoardIdValue (VOID);

UINT16
SystemBoardIdSkuValue (VOID);

VOID
UpdateMcuInfo (
  VOID
  );


/**

  Count of the PCI-E slot by CRB.

  @param mSystemConfiguration  -  Setup data.

  @retval VOID.

--*/
BOOLEAN
CheckDeviceByRp (
  IN UINTN                      Bus,
  IN UINT32                     CpuNum,
  IN UINT32                     RtPrtDevNum,
  IN UINT32                     RtPrtFunNum
  );

#endif
