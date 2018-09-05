/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  OpromUpdateDxe.c

Abstract:

--*/
#include "OpromUpdateDxe.h"
#include <IioRegs.h>
#include <Library/UbaOpromUpdateLib.h>

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>

#undef PCI_PCIE_ADDR

#define IIO_BUS_NUM  0

typedef union {
  
  UINT32 PcieSlotInfo;
  
  struct {
    UINT32 RtPrtFunNum : 8;
    UINT32 RtPrtDevNum : 8;
    UINT32 CpuNum : 8;
    UINT32 Dummy : 8;
    /* Slot definition for the root ports of Grantley CRBs
     */
  } RootPrt;  
} PLATFORM_PCIE_SLOT_STRUCT;

//
// Describes PCI-E slot location with each SRP (SKX_TODO: Need to be updated for NeonCity and Lightning Ridge)
//
PLATFORM_PCIE_SLOT_STRUCT  MYEXRP [] =  {0x00000300,  //Slot 1
                                         0x00800100,  //Slot 3
                                         0x00800200,  //Slot 4
                                         0x00800302,  //Slot 6
                                         0x00000200,  //Slot 10
                                         0x00000100,  //Slot 11
                                         0x00001C05,  //Slot 12
                                         0x00800300   //Slot 14
                                        };



                                        
BOOLEAN  UpdateHideSetupDone  = FALSE;

BOOLEAN
CheckDeviceByRp (
  IN UINTN                      Bus,
  IN UINT32                     CpuNum,
  IN UINT32                     RtPrtDevNum,
  IN UINT32                     RtPrtFunNum
  );

BOOLEAN
CheckPcieRootPort (
  IN  UINTN                     Bus,
  IN  UINT32                    PcieSlotOpromBitMap
)
{
  UINT16                              TableCount;
  BOOLEAN                             OnSlot = FALSE;
  UINT8                               PcieSlotOpromInfo[8] = {0};
  UINT16                              Count;

  //
  // Check the platform type and set PCI-E slot describes.
  //
  TableCount = (( sizeof ( MYEXRP )) / ( sizeof ( UINT32 )));
  //DEBUG((EFI_D_INFO, "TableCount - %x\n", TableCount));

  for ( Count = 0 ; Count < TableCount ; Count++ ) {
    PcieSlotOpromInfo[Count] = (PcieSlotOpromBitMap >> Count) & BIT0;
    if ( !PcieSlotOpromInfo[Count] ) {
      OnSlot = CheckDeviceByRp (
                  Bus,
                  MYEXRP[Count].RootPrt.CpuNum,
                  MYEXRP[Count].RootPrt.RtPrtDevNum,
                  MYEXRP[Count].RootPrt.RtPrtFunNum
                );
    }
  }

  return OnSlot;
}

PC_PCI_OPTION_ROM_TABLE      DefaultPciOptionRomTable[] = {
  
  //
  // End of OptionROM Entries
  //
  {
    NULL_ROM_FILE_GUID, // Guid
    0,                  // Segment
    0,                  // Bus Number
    0,                  // Device Number
    0,                  // Function Number
    0xffff,             // Vendor ID
    0xffff              // Device ID
  }
};

EFI_STATUS
GetOptionRomTable (
  IN  PC_PCI_OPTION_ROM_TABLE         **OptionRomTable
)
{
  *OptionRomTable = DefaultPciOptionRomTable;
  return EFI_SUCCESS;
}


NIC_SETUP_CONFIGURATION_STUCT  mDefaultNicConfigurationTable[]  = {
  //NicType,    NicIndex ,       RootPortBusNo       ,              RootPortDevNo            ,          RootPortFunNo                 ,  NicVID,        NicDID            ,PortNumbers,DescriptionIndex,IsGpioCtrl,GpioForDev,GpioForDevValue,GpioForPort1,GpioForPort1Value,GpioForPort2,GpioForPort2Value,GpioForPort3,GpioForPort3Value,GpioForPort4,GpioForPort4Value
  { ONBOARD_NIC,  1      ,        IIO_BUS_NUM       ,               PCIE_PORT_2_DEV ,               PCIE_PORT_2C_FUNC,                  0x8086,         0x1528           , 2         ,   8             ,   1        , 72       , 1             ,     45     ,  1              , 44         ,  1              , 46         ,  0xFF            ,  47        ,    0xFF            },
  { ONBOARD_NIC,  1      ,        IIO_BUS_NUM       ,               PCIE_PORT_2_DEV ,               PCIE_PORT_2C_FUNC,                  0x8086,         0x1521           , 2         ,   1             ,   1        , 72       , 1             ,     45     ,  1              , 44         ,  1              , 46         ,  0xFF            ,  47        ,    0xFF            },
  
  //IOM for 82599 Dual-Port 10 Gigabit SFP+ Module
  //
  //ONBOARD_NIC_START
  //
  { IO_MODULE_NIC, 1   ,      CPU0_IIO_BUS       ,            IIO_PCIE_PORT_2A_DEV       ,        IIO_PCIE_PORT_2A_FUN,              0x8086,         0x10FB           , 2         ,   5             ,   0xFF     , 0xFF     , 0xFF          ,     0xFF   ,  0xFF           , 0xFF       ,  0xFF           , 0xFF       ,  0xFF            ,  0xFF      ,    0xFF            },
  //IOM for I350 Quad-port Gigabit Ethernet Module
  { IO_MODULE_NIC,  1,     CPU0_IIO_BUS,  IIO_PCIE_PORT_2A_DEV,   IIO_PCIE_PORT_2A_FUN,  0x8086,OB_NIC_POWERVILLE_DID,                 4         ,   4            ,   0xFF        , 0xFF      , 0xFF            ,     0xFF     ,  0xFF              , 0xFF         ,  0xFF             , 0xFF         ,  0xFF              ,  0xFF        ,    0xFF            },
  //IOM for  X540 10 Gigabit RJ-45 Module
  { IO_MODULE_NIC,  1,     CPU0_IIO_BUS,  IIO_PCIE_PORT_2A_DEV      ,   IIO_PCIE_PORT_2A_FUN,              0x8086,         0x1528           , 2         ,   9             ,   0xFF     , 0xFF     , 0xFF          ,     0xFF   ,  0xFF           , 0xFF       ,  0xFF           , 0xFF       ,  0xFF            ,  0xFF      ,    0xFF            },
  // 
  //ONBOARD_NIC_END
  //

  //OnBoard Nic 1 End

  { ONBOARD_NIC,  2      ,        CPU0_IIO_BUS       ,     IIO_PCIE_PORT_2A_DEV,             IIO_PCIE_PORT_2A_FUN,          VENDOR_ID_MELLANOX,       DEVICE_ID_MELLANOX  , 1         ,   6             ,   0        , 0xFF     , 0xFF          , 0xFF       , 0xFF            , 0xFF       , 0xFF            , 0xFF       , 0xFF             , 0xFF       ,    0xFF            }
  //OnBoard Nic 2 End

//{ IO_MODULE_NIC,  1,     CPU0_IIO_BUS,  IIO_PCIE_PORT_1B_DEV,IIO_PCIE_PORT_1B_FUN,  0x8086,OB_NIC_POWERVILLE_DID, 4         ,   3             ,   1        , 72       , 1             ,     44     ,  1              , 45         ,  1              , 46         ,  1              ,  47        ,    1            }
  //IO Module Nic 1 End

//{ IO_MODULE_NIC,  2,     CPU0_IIO_BUS,  IIO_PCIE_PORT_1B_DEV,IIO_PCIE_PORT_1B_FUN,  0x8086,OB_NIC_POWERVILLE_DID, 4         ,   3             ,   1        , 72       , 1             ,     44     ,  1              , 45         ,  1              , 46         ,  1              ,  47        ,    1            }
  //IO Module Nic 2 End
};

NIC_OPTIONROM_CAPBILITY_STRUCT mDefaultNicCapbilityTable[] = {
  //NicDID,   SubDID, NIC10Gb? PXE? iSCSI? FCoE? InfiniBand?
  { 0x10D3,   0x3580,   0,      1,   1,      0 ,   0 }, //Intel (R) 82754 Gigabit Ethernet Controller 
  { 0x1521,   0x3580,   0,      1,   1,      0 ,   0 }, //Intel (R) I350 Gigabit Ethernet Controller
  { 0x1521,   0x35B0,   0,      1,   1,      0 ,   0 }, //Intel (R) I350 Gigabit Ethernet Controller
  { 0x1522,   0x3580,   0,      1,   1,      0 ,   0 }, //Intel (R) I350 Gigabit Ethernet Controller
  { 0x1521,   0x3556,   0,      1,   1,      0 ,   0 }, //Intel (R) I350 Quad-port Gigabit Ethernet Module
  { 0x10fb,   0x3557,   1,      1,   1,      1 ,   0 }, //Intel (R) 82599 10 Gigabit SFP+ Module
  { 0x1528,   0x3558,   1,      1,   1,      1 ,   0 }, //Intel (R) X540 10 Gigabit RJ-45 Module
//
//OPTION_ROM_START
//
  { 0x1528,   0x35C5,   1,      1,   1,      1 ,   0 }, //Intel (R) X540 10 Gigabit RJ-45 Module
//
//OPTION_ROM_END
//
  { 0x1003,   0x1003,   0,      0,   0,      0 ,   1 }, //Mellanox* ConnectX-3* Single-Port InfiniBand FD14
  { 0x1528,   0x35A0,   1,      1,   1,      1 ,   0 }  //Intel (R) X540 10 Gigabit RJ-45 Module on LHP
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EFI_STATUS
GetNicSetupConfigTable (
  IN  NIC_SETUP_CONFIGURATION_STUCT     **NicSetupConfigTable,
  IN  UINTN                             *NumOfConfig
)
{
  *NicSetupConfigTable  = mDefaultNicConfigurationTable;
  *NumOfConfig          = sizeof(mDefaultNicConfigurationTable)/ sizeof(NIC_SETUP_CONFIGURATION_STUCT);

  return EFI_SUCCESS;
}

EFI_STATUS
GetNicCapabilityTable (
  IN  NIC_OPTIONROM_CAPBILITY_STRUCT    **NicCapabilityTable,
  IN  UINTN                             *NumOfNicCapTable
)
{
  *NicCapabilityTable = mDefaultNicCapbilityTable;
  *NumOfNicCapTable   = sizeof(mDefaultNicCapbilityTable)/sizeof(NIC_OPTIONROM_CAPBILITY_STRUCT);
  
  return EFI_SUCCESS;
}

EFI_STATUS
SetPcieSlotNumber (
    OUT  UINT8                   *PcieSlotItemCtrl
  )
{
  *PcieSlotItemCtrl = 0;
  DEBUG((EFI_D_ERROR, "[UBA]:SetPcieSlotNumber callback - %d\n", *PcieSlotItemCtrl));
  return EFI_SUCCESS;
}


/**

  Use the PCI Root Bridge protocol and check the Bus number be match Root Bridge or not.

  @param Bus           -  PCI Bus number.
  @param CpuNum        -  Number of Slot Bus number.
  @param RtPrtDevNum   -  Number of Slot Device number.
  @param RtPrtFunNum   -  Number of Slot Function number.

  @retval TRUE    -  This device on the PCI-E slot.
  @retval FALSE   -  This is onboard device.

**/
BOOLEAN
CheckDeviceByRp (
  IN UINTN                      Bus,
  IN UINT32                     CpuNum,
  IN UINT32                     RtPrtDevNum,
  IN UINT32                     RtPrtFunNum
  )
{
  EFI_STATUS                          Status;
  BOOLEAN                             OnSlot = FALSE;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *gPciRootBridgeIo;
  UINT64                              Address;
  UINT8                               SlotSecBusNum;
  UINT8                               SlotSubBusNum;

  //
  // Get PCI Root Bridge Io Protocol
  //
  Status = gBS->LocateProtocol(
              &gEfiPciRootBridgeIoProtocolGuid,
              NULL,
              &gPciRootBridgeIo
            );

//
// Get the slot sub bus number and second bus number,
// If the device on the slot it's bus number will between sub bus and second bus.
//
  Address = EFI_PCI_ADDRESS (
              CpuNum,
              RtPrtDevNum,
              RtPrtFunNum,
              PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET
            );

  Status = gPciRootBridgeIo->Pci.Read(
              gPciRootBridgeIo,
              EfiPciWidthUint8,
              Address,
              1,
              &SlotSecBusNum
            );

  Address = EFI_PCI_ADDRESS (
              CpuNum,
              RtPrtDevNum,
              RtPrtFunNum,
              PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET
            );

  Status = gPciRootBridgeIo->Pci.Read(
              gPciRootBridgeIo,
              EfiPciWidthUint8,
              Address,
              1,
              &SlotSubBusNum
            );

  if ( ( Bus >= SlotSecBusNum ) && ( Bus <= SlotSubBusNum ) ) {
    OnSlot = TRUE;
  }

  return OnSlot;
}

PLATFORM_OPTION_ROM_UPDATE_DATA  OptionRomHelperUpdateTable = 
{
  PLATFORM_OPTION_ROM_UPDATE_SIGNATURE,
  PLATFORM_OPTION_ROM_UPDATE_VERSION,
  
  CheckPcieRootPort,
  GetOptionRomTable,
  GetNicSetupConfigTable,
  GetNicCapabilityTable,
  SetPcieSlotNumber
};


/**
  The Driver Entry Point.
  
  The function is the driver Entry point.
  
  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
OpromUpdateEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  EFI_STATUS                                Status;
  UBA_CONFIG_DATABASE_PROTOCOL        *UbaConfigProtocol = NULL;

  DEBUG((EFI_D_ERROR, "UBA:OpromUpdate-TypeLightningRidgeEXECB1\n"));
  Status = gBS->LocateProtocol (
                  &gUbaConfigDatabaseProtocolGuid,
                  NULL,
                  &UbaConfigProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = UbaConfigProtocol->AddData (
                                     UbaConfigProtocol,
                                     &gPlatformOptionRomUpdateConfigDataGuid, 
                                     &OptionRomHelperUpdateTable, 
                                     sizeof(OptionRomHelperUpdateTable)
                                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  return Status;
}
