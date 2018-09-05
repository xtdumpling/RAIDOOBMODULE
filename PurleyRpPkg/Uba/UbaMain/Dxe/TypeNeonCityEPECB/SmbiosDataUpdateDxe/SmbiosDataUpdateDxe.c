/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmbiosDataUpdateDxe.c

Abstract:

--*/
#include <Base.h>
#include <Uefi.h>
#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/Pci.h>

#include <Protocol/Smbios.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/IoLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>

#include <Protocol/IioUds.h>

#include <Library/UbaSmbiosUpdateLib.h>

//
// Ports 1A-1B
//
#define IIO_BIFURCATE_x4x4      0
#define IIO_BIFURCATE_xxx8      1

// Ports 2A-2D, 3A-3D
//
#define IIO_BIFURCATE_x4x4x4x4  0
#define IIO_BIFURCATE_x4x4xxx8  1
#define IIO_BIFURCATE_xxx8x4x4  2
#define IIO_BIFURCATE_xxx8xxx8  3
#define IIO_BIFURCATE_xxxxxx16  4

#define SMBIOS_TYPE_MAX_LENGTH    0x300


EFI_HII_HANDLE gSmbiosStringPackHandle = NULL;

//
// Default GUID, will update by Image file GUID later
//
// {E2D6FD7B-2E3B-46fd-BDED-512A7F032AA0}
#define THIS_SMBIOS_STRING_PACK_GUID \
{ 0xe2d6fd7b, 0x2e3b, 0x46fd, { 0xbd, 0xed, 0x51, 0x2a, 0x7f, 0x3, 0x2a, 0xa0 } }


EFI_GUID  gSmbiosStringPackGuid = THIS_SMBIOS_STRING_PACK_GUID;

typedef struct {
  EFI_STRING_ID   String1;
  EFI_STRING_ID   String2;
  UINT8           IntRefDesig;
  UINT8           IntConnType;
  UINT8           ExtRefDesig;
  UINT8           ExtConnType;
  UINT8           PortType;
} PORT_CONN_TABLE;

/**
  Update SMBIOS type 8 data.

  
  @param Smbios                     SMBIOS data structure buffer pointer
  @param BufferSize                 SMBIOS data structure buffer size
  @param Instance                   Instance number for this type

  @retval EFI_SUCCESS:              SMBIOS data update successfully
  @retval EFI_INVALID_PARAMETER:    Check your register data, it should be wrong

**/
EFI_STATUS
UpdateSmbiosType8 (
  IN  OUT SMBIOS_STRUCTURE_POINTER    Smbios,
  IN  OUT UINTN                       *BufferSize,
  IN      UINTN                       Instance
)
{
  CHAR16                              *TempString = NULL;
  PORT_CONN_TABLE                     PortConnData[]= {

  // External port
  {STRING_TOKEN (STR_ONBOARD_CONN1) , 0, 0,    0, 1, 0x08,    8}, // Serial Port A (BMC serial port A /J4A2) DB-9 male
  {STRING_TOKEN (STR_ONBOARD_CONN2) , 0, 0,    0, 1, 0x08,    8}, // Serial Port B (BMC serial port B /J126) DB-9

  {STRING_TOKEN (STR_ONBOARD_CONN3) , 0, 0,    0, 1, 0x12, 0x10}, // USB front panel header  (USB3 FP port /J1D1)  20pin
  {STRING_TOKEN (STR_ONBOARD_CONN4) , 0, 0,    0, 1, 0x12, 0x10}, // USB front panel header  (USB2 FP port /J92)  10pin
  {STRING_TOKEN (STR_ONBOARD_CONN5) , 0, 0,    0, 1, 0x12, 0x10}, // USB back panel connector (USB3 P03,04,05 BP / J4A1) 27pin


  {STRING_TOKEN (STR_ONBOARD_CONN6), 0, 0,    0, 1,    7, 0x1C}, // BMC VIDEO REAR PANEL VGA (J70) 15pin
  {STRING_TOKEN (STR_ONBOARD_CONN7), 0, 0,    0, 1,    7, 0x1C}, // BMC VIDEO FRONT PANEL VGA (J9K2) 14pin
  
  {STRING_TOKEN (STR_ONBOARD_CONN10), 0, 0,    0, 1, 0x0B, 0x1F}, // NIC 1  (10GBE port / J64) 20pin
  {STRING_TOKEN (STR_ONBOARD_CONN11), 0, 0,    0, 1, 0x0B, 0x1F}, // NIC 2  (10GBE port / J65) 20pin
  {STRING_TOKEN (STR_ONBOARD_CONN12), 0, 0,    0, 1, 0x0B, 0x1F}, // RMM NIC (JA2) 14pin
  // Internal port
  {STRING_TOKEN (STR_ONBOARD_CONN13), 0, 1, 0x12, 0,    0, 0x10}, // Internal USB (USB3 port /J4A3)  internal Type-A 1 9pin
  {STRING_TOKEN (STR_ONBOARD_CONN14), 0, 1, 0x12, 0,    0, 0x10}, // Internal USB (USB2 port /J82)   internal Type A 2 4 pin
  
  {STRING_TOKEN (STR_ONBOARD_CONN17), 0, 1, 0xFF, 0,    0, 0xFF}, // TPM ( SPI type / J101) 12pin
  
  {STRING_TOKEN (STR_ONBOARD_CONN21), 0, 1, 0x22, 0,    0, 0x20}, // SATA6G  S4   J76 7pin
  {STRING_TOKEN (STR_ONBOARD_CONN22), 0, 1, 0x22, 0,    0, 0x20}, // SATA6G  S5  J4B6 7pin
  {STRING_TOKEN (STR_ONBOARD_CONN23), 0, 1, 0x22, 0,    0, 0x20}, // SATA6G P0,P1,P2,P3  J77  36pin
  {STRING_TOKEN (STR_ONBOARD_CONN24), 0, 1, 0x22, 0,    0, 0x20}, // SATA6G P4,P5,P6,P7  J83  36pin

  {STRING_TOKEN (STR_ONBOARD_CONN25), 0, 1, 0x22, 0,    0, 0x20}, // SATA 4,5,6,7   J83  75pin
  {STRING_TOKEN (STR_ONBOARD_CONN26), 0, 1, 0x22, 0,    0, 0x20}, // SATA 8,9  J85  75pin

  {STRING_TOKEN (STR_ONBOARD_CONN27), 0, 1, 0x22, 0,    0, 0x20}, // SATA 0-3 (min-SAS) (J77) 36pin
  {STRING_TOKEN (STR_ONBOARD_CONN28), 0, 1, 0x22, 0,    0, 0x21}, // SAS (J10) 80pin


  };

  if (Instance >= (sizeof(PortConnData)/sizeof(PORT_CONN_TABLE))) {
    return EFI_INVALID_PARAMETER;
  }
  
  Smbios.Type8->Hdr.Handle    = SMBIOS_HANDLE_PI_RESERVED;
  Smbios.Type8->Hdr.Type      = EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION;
  Smbios.Type8->Hdr.Length    = sizeof (SMBIOS_TABLE_TYPE8);

  Smbios.Type8->InternalReferenceDesignator = PortConnData[Instance].IntRefDesig;
  Smbios.Type8->InternalConnectorType       = PortConnData[Instance].IntConnType;
  Smbios.Type8->ExternalReferenceDesignator = PortConnData[Instance].ExtRefDesig;
  Smbios.Type8->ExternalConnectorType       = PortConnData[Instance].ExtConnType;
  Smbios.Type8->PortType                    = PortConnData[Instance].PortType;

  if (PortConnData[Instance].String1) {
  
    TempString = HiiGetString (gSmbiosStringPackHandle, PortConnData[Instance].String1, NULL);
    if (TempString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if (PortConnData[Instance].IntRefDesig != 0) {
      PlatformSmbiosUpdateString (Smbios, *BufferSize, PortConnData[Instance].IntRefDesig, TempString);
    } else {
      PlatformSmbiosUpdateString (Smbios, *BufferSize, PortConnData[Instance].ExtRefDesig, TempString);
    }
    FreePool (TempString);
  }

  if (PortConnData[Instance].String2) {
    TempString = HiiGetString (gSmbiosStringPackHandle, PortConnData[Instance].String2, NULL);
    if (TempString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    PlatformSmbiosUpdateString (Smbios, *BufferSize, PortConnData[Instance].ExtRefDesig, TempString);
    FreePool (TempString);
  }
  
  return EFI_SUCCESS;
}

/**
  Update SMBIOS type 9 data.

  
  @param Smbios                     SMBIOS data structure buffer pointer
  @param BufferSize                 SMBIOS data structure buffer size
  @param Instance                   Instance number for this type

  @retval EFI_SUCCESS:              SMBIOS data update successfully
  @retval EFI_INVALID_PARAMETER:    Check your register data, it should be wrong

**/
EFI_STATUS
UpdateSmbiosType9 (
  IN  OUT SMBIOS_STRUCTURE_POINTER    Smbios,
  IN  OUT UINTN                       *BufferSize,
  IN      UINTN                       Instance
)
{
  UINT8                               BusNumber;
  CHAR16                              *TempString = NULL;
  UINT8                               IOU_BIFURCATE;
 /* This info comes from Wolfpass schematic (system diagram section)
  x24 PCIe Gen3 Rise1; CPU0 PCIE_1(gen3 x16) slot#1,   CPU1 PCIE_1(gen3 x8) slot #3
  x24 PCIe Gen3 Riser2; CPU1 PCIE_2(gen3 x16) slot #4, CPU1 PCIE_1(gen3 x8) slot #6
  x24 PCIe Gen3 Riser3; CPU1 DMI_3(gen3 x4) slot #7,    CPU1 PCIE_3(gen3 x8) slot #8
  TODO: replace with right DBF info
 */
  Smbios.Type9->Hdr.Handle    = SMBIOS_HANDLE_PI_RESERVED;
  Smbios.Type9->Hdr.Type      = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
  Smbios.Type9->Hdr.Length    = sizeof (SMBIOS_TABLE_TYPE9);
  
  Smbios.Type9->BusNum                                  = 0;
  Smbios.Type9->SlotDesignation                         = 1;
  Smbios.Type9->SlotLength                              = 4; //03 short 04 long
  Smbios.Type9->SlotCharacteristics1.Provides33Volts    = 1; //if 3.3V is supported
  Smbios.Type9->SlotCharacteristics2.PmeSignalSupported = 1; //support PME signal
  Smbios.Type9->SlotCharacteristics2.SmbusSignalSupported = 1;  //support SMBus Signal
  Smbios.Type9->SegmentGroupNum                         = 0; //Single-segment topology

  switch (Instance) {
    case 0:
      Smbios.Type9->SlotID            = 1;
      Smbios.Type9->SlotType          = 0xB6; // CPU0 PCIE_1 (gen3 x16)
      Smbios.Type9->SlotDataBusWidth  = 0x0D; // 0D - x16, 0B - x8
      Smbios.Type9->BusNum            = 0x00; // B:0x00
      Smbios.Type9->DevFuncNum        = 0x18; // D:3, F:0

      //
      // B:0x00, D:3, F:0, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0x00, 3, 0) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) == 0xFFFFFFFF) {
        Smbios.Type9->CurrentUsage = 3;  // Available
      } else {
        Smbios.Type9->CurrentUsage = 4;  // In use
      }

      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_SYSTEM_SLOT1), NULL);

      break;

    case 1:
      Smbios.Type9->SlotID            = 3;
      Smbios.Type9->SlotType          = 0xB5; // CPU1 PCIE_1 (gen3 x8)
      Smbios.Type9->SlotDataBusWidth  = 0x0B; // 0D - x16, 0B - x8
      Smbios.Type9->BusNum            = 0x00; // B:0x00
      Smbios.Type9->DevFuncNum        = 0x1A; // D:3, F:2

      if (MmioRead32(MmPciBase(0x00, 3, 2)) == 0xFFFFFFFF) {
        return EFI_DEVICE_ERROR;
      }

      //
      // B:0, D:3, F:2, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0x00, 3, 2) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) == 0xFFFFFFFF) {
        Smbios.Type9->CurrentUsage = 3;  // Available
      } else {
        Smbios.Type9->CurrentUsage = 4;  // In use
      }

      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_SYSTEM_SLOT1), NULL);
      
      break;

    case 2:
      Smbios.Type9->SlotID            = 4;
      Smbios.Type9->SlotType          = 0xB6; // CPU1 PCIE_2 (gen3 x16)
      Smbios.Type9->SlotDataBusWidth  = 0x0D; // 0D - x16, 0B - x8
      Smbios.Type9->BusNum            = 0x80; // B:0x80
      Smbios.Type9->DevFuncNum        = 0x08; // D:1, F:0

      //
      // B:0x80, D:1, F:0, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0x80, 1, 0) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) == 0xFFFFFFFF) {
        Smbios.Type9->CurrentUsage = 3;  // Available
      } else {
        Smbios.Type9->CurrentUsage = 4;  // In use
      }

      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_SYSTEM_SLOT1_2), NULL);
      
      break;

    case 3:
      Smbios.Type9->SlotID            = 6;
      Smbios.Type9->SlotType          = 0xB5; // CPU1 PCIE_1 (gen3 x8)
      Smbios.Type9->SlotDataBusWidth  = 0x0B; // 0D - x16, 0B - x8
      Smbios.Type9->BusNum            = 0x80; // B:0x80
      Smbios.Type9->DevFuncNum        = 0x10; // D:2, F:0

      IOU_BIFURCATE = MmioRead8(MmPciBase(0x80, 2, 0) + 0x190);

      if (IOU_BIFURCATE == IIO_BIFURCATE_xxx8xxx8) {
        Smbios.Type9->SlotID            = 5;
        Smbios.Type9->SlotType          = 0xB6; // B6 - PCIE Gen3 x16, B5 - PCIE Gen3 x8
        Smbios.Type9->SlotDataBusWidth  = 0x0B; // 0D - x16, 0B - x8
      }

      //
      // B:0x80, D:2, F:0, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0x80, 2, 0) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) == 0xFFFFFFFF) {
        Smbios.Type9->CurrentUsage = 3;  // Available
      } else {
        Smbios.Type9->CurrentUsage = 4;  // In use
      }

      if (IOU_BIFURCATE == IIO_BIFURCATE_xxx8xxx8) {
        TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_SYSTEM_SLOT2_1), NULL);
      }else{
        TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_SYSTEM_SLOT2), NULL);
      }
      
      break;

    case 4:
      Smbios.Type9->SlotID            = 7;
      Smbios.Type9->SlotType          = 0xB4; // DMI_3 (gen3 X4)   0xB4 - PCIE Gen3 x4
      Smbios.Type9->SlotDataBusWidth  = 0x0A; // 0D - x16, 0B - x8, 0a - x4
      Smbios.Type9->BusNum            = 0x80; // B:0x80
      Smbios.Type9->DevFuncNum        = 0x12; // D:2, F:2

      if (MmioRead32(MmPciBase(0x80, 2, 2)) == 0xFFFFFFFF) {
        return EFI_DEVICE_ERROR;
      }

      //
      // B:0x80, D:2, F:2, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0x80, 2, 2) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) == 0xFFFFFFFF) {
        Smbios.Type9->CurrentUsage = 3;  // Available
      } else {
        Smbios.Type9->CurrentUsage = 4;  // In use
      }

      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_SYSTEM_SLOT2), NULL);
      
      break;

    case 5:
      Smbios.Type9->SlotID            = 8;
      Smbios.Type9->SlotType          = 0xB5; // PCIE_3 (gen3 x8)
      Smbios.Type9->SlotDataBusWidth  = 0x0B; // 0D - x16, 0B - x8
      Smbios.Type9->BusNum            = 0x80; // B:
      Smbios.Type9->DevFuncNum        = 0x1A; // D:3, F:2

      //
      // B:80, D:3, F:2, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0x80, 3, 2) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) == 0xFFFFFFFF) {
        Smbios.Type9->CurrentUsage = 3;  // Available
      } else {
        Smbios.Type9->CurrentUsage = 4;  // In use
      }

      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_SYSTEM_SLOT2_2), NULL);
      break;

    default :
      return EFI_INVALID_PARAMETER;
  }

  if (TempString != NULL) {
    PlatformSmbiosUpdateString (Smbios, *BufferSize, Smbios.Type9->SlotDesignation, TempString);
    FreePool (TempString);
  }
  
  return EFI_SUCCESS;
}


/**
  Update SMBIOS type 41 data.

  
  @param Smbios                     SMBIOS data structure buffer pointer
  @param BufferSize                 SMBIOS data structure buffer size
  @param Instance                   Instance number for this type

  @retval EFI_SUCCESS:              SMBIOS data update successfully
  @retval EFI_INVALID_PARAMETER:    Check your register data, it should be wrong

**/
EFI_STATUS
UpdateSmbiosType41 (
  IN  OUT SMBIOS_STRUCTURE_POINTER    Smbios,
  IN  OUT UINTN                       *BufferSize,
  IN      UINTN                       Instance
)
{
  UINT8                               BusNumber;
  CHAR16                              *TempString = NULL;
  
  Smbios.Type41->Hdr.Handle    = SMBIOS_HANDLE_PI_RESERVED;
  Smbios.Type41->Hdr.Type      = EFI_SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION;
  Smbios.Type41->Hdr.Length    = sizeof (SMBIOS_TABLE_TYPE41);

  Smbios.Type41->ReferenceDesignation = 1;
  Smbios.Type41->SegmentGroupNum      = 0;

  switch (Instance) {
    case 0: // VGA
      //TODO: replace with right DBF info
      // ServerEngines Pilot III, B:0, D:28, F:3, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0, 28, 3) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) != 0xFFFFFFFF) {
        Smbios.Type41->DeviceType = 0x83;
      } else {
        Smbios.Type41->DeviceType = 0x03;
      }
      
      Smbios.Type41->BusNum     = BusNumber;
      Smbios.Type41->DevFuncNum = 0;
      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_ONBOARD_DEVICE1), NULL);
  
      break;
      
    case 1: // 10GE ethernet
      //TODO: replace with right DBF info
      // TwinVille, B:0, D:31, F:6, O:0x19 (Secondary Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0, 2, 2) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) != 0xFFFFFFFF) {
        Smbios.Type41->DeviceType = 0x85;
      } else {
        Smbios.Type41->DeviceType = 0x05;
      }
      
      Smbios.Type41->BusNum     = BusNumber;
      Smbios.Type41->DevFuncNum = 0;
      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_ONBOARD_DEVICE2), NULL);
      break;
      
    case 2: // SAS
      //
      // SAS, B:0, D:1, F:0, O:0x19 (Subordinate Bus Number)
      //
      BusNumber = MmioRead8(MmPciBase(0, 1, 0) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) != 0xFFFFFFFF) {
        Smbios.Type41->DeviceType = 0x8A;
      } else {
        Smbios.Type41->DeviceType = 0x0A;
      }

      Smbios.Type41->BusNum     = BusNumber;
      Smbios.Type41->DevFuncNum = 0;
      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_ONBOARD_DEVICE3), NULL);
      
      break;
      
    case 3: // PCH Integrated SATA Controller
      // TODO: replace with right DBF info
      // Integrated SATA Controller, B:0, D:31, F:2
      //
      if (MmioRead32(MmPciBase(0, 31, 2)) != 0xFFFFFFFF) {
        Smbios.Type41->DeviceType = 0x89;
      } else {
        Smbios.Type41->DeviceType = 0x09;
      }
      
      Smbios.Type41->BusNum     = 0;
      Smbios.Type41->DevFuncNum = 0xFA;
      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_ONBOARD_DEVICE4), NULL);
      
      break;
/*      
    case 4: //Gigabit Ethernet controller(D31:F6)  NIC
      //
      // IO Module, B:0, D:2, F:0, O:0x19 (Secondary Bus Number)
      // TODO: check IO module BDF info
      BusNumber = MmioRead8(MmPciBase(0, 2, 0) + 0x19);
      if (MmioRead32(MmPciBase(BusNumber, 0, 0)) != 0xFFFFFFFF) {
        Smbios.Type41->DeviceType = 0x85;
      } else {
        Smbios.Type41->DeviceType = 0x05;
      }
      
      Smbios.Type41->BusNum     = BusNumber;
      Smbios.Type41->DevFuncNum = 0;
      TempString = HiiGetString (gSmbiosStringPackHandle, STRING_TOKEN (STR_ONBOARD_DEVICE5), NULL);
      
      break;
*/      
    default:
      return EFI_INVALID_PARAMETER;
  }

  if (TempString != NULL) {
    PlatformSmbiosUpdateString (Smbios, *BufferSize, 1, TempString);
    FreePool (TempString);
  }  
  
  return EFI_SUCCESS;
}

/**
  Update Platform SMBIOS data.

  
  @param Smbios                     SMBIOS data structure buffer pointer
  @param BufferSize                 SMBIOS data structure buffer size
  @param Instance                   Instance number for this type

  @retval EFI_SUCCESS:              SMBIOS data update successfully
  @retval EFI_INVALID_PARAMETER:    Check your register data, it should be wrong

**/
EFI_STATUS
UpdateSmbios (
  VOID
)
{
  EFI_STATUS                          Status;
  SMBIOS_STRUCTURE_POINTER            SmbiosPtr;
  UINTN                               BufferSize;
  UINTN                               Instance = 0;

  BufferSize    = SMBIOS_TYPE_MAX_LENGTH;
  SmbiosPtr.Hdr = AllocateZeroPool (BufferSize);
  Status        = EFI_SUCCESS;
  
  for (Instance = 0; Instance < 30; Instance++)
  {
    BufferSize    = SMBIOS_TYPE_MAX_LENGTH;
    ZeroMem (SmbiosPtr.Hdr, BufferSize);
    
    Status = UpdateSmbiosType8 (SmbiosPtr, &BufferSize, Instance);
    if (!EFI_ERROR(Status)) {
      Status = PlatformSmbiosAddNew (SmbiosPtr);
    }
  }

  PlatformSmbiosRemoveAll (EFI_SMBIOS_TYPE_SYSTEM_SLOTS);
  for (Instance = 0; Instance < 8; Instance++)
  {
    BufferSize    = SMBIOS_TYPE_MAX_LENGTH;
    ZeroMem (SmbiosPtr.Hdr, BufferSize);
    
    Status = UpdateSmbiosType9 (SmbiosPtr, &BufferSize, Instance);
    if (!EFI_ERROR(Status)) {
      Status = PlatformSmbiosAddNew (SmbiosPtr);
    }
  }

  PlatformSmbiosRemoveAll (EFI_SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION);
  for (Instance = 0; Instance < 4; Instance++)
  {
    BufferSize    = SMBIOS_TYPE_MAX_LENGTH;
    ZeroMem (SmbiosPtr.Hdr, BufferSize);
    
    Status = UpdateSmbiosType41 (SmbiosPtr, &BufferSize, Instance);
    if (!EFI_ERROR(Status)) {
      Status = PlatformSmbiosAddNew (SmbiosPtr);
    }
  }

  FreePool (SmbiosPtr.Hdr);
 
  return Status;
}

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
SmbiosDataUpdateEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
  SMBIOS_UPDATE_DATA                      UpdateData;
  EFI_STATUS                              Status;
  EFI_LOADED_IMAGE_PROTOCOL               *LoadedImage;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH       *FvFile;

  //
  // Retrieve the Loaded Image Protocol
  //
  Status = gBS->HandleProtocol (
                ImageHandle,
                &gEfiLoadedImageProtocolGuid,
                (VOID*)&LoadedImage
                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FvFile = (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*) LoadedImage->FilePath;
  DEBUG ((EFI_D_ERROR, "UBA:SmbiosDataUpdateEntry Image GUID=%g\n", &FvFile->FvFileName));

  CopyGuid (&gSmbiosStringPackGuid, &FvFile->FvFileName);

  gSmbiosStringPackHandle = HiiAddPackages (
                              &gSmbiosStringPackGuid,
                              &ImageHandle,
                              STRING_ARRAY_NAME,
                              NULL
                              );

  ASSERT (gSmbiosStringPackHandle != NULL);

  ZeroMem (&UpdateData, sizeof (UpdateData));

  UpdateData.UpdateType   = SmbiosDelayUpdate;
  UpdateData.PlatformType = TypeNeonCityEPECB;
  UpdateData.CallUpdate   = UpdateSmbios;

  Status = PlatformRegisterSmbiosUpdate (&UpdateData);

  return Status;
}
