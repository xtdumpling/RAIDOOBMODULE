/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2006-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  PlatformDeviceDataSRP.c

Abstract:
  This file has platform specific data determined by the copper 
  wiring between components on the baseboard and daugther boards if any:
  1. The build-in devices and slots. 
  2. Rules for platform add-in devices.
  This is the only place where data describing a system
  have to change in order to port to a different platform.

  The HW in this file stands for Hard-Wired.

Revision History:

--*/

#include <PlatPirqData.h>
#include <PlatDevData.h>
#include "MpTable.h"
#include "PirqTable.h"
#include <IndustryStandard/LegacyBiosMpTable.h>

#ifndef V_INTEL_VID
#define V_INTEL_VID               0x8086
#endif
//
// Describes IO APICs when PEXH IOAPICs are enabled
//
STATIC DEVICE_DATA_HW_IO_APIC        DeviceDataHwIoApic1[] = {
    MPT_IO_APIC_DATA
};

//
// Describes PCI slot relationship with the IO APICs.
//
STATIC DEVICE_DATA_HW_PCI_SLOT       DeviceDataHwPciSlot1[] = { 
    MPT_SLOT_DATA
};

//
// Describes built-in device relationship with the IO APICs.
//
STATIC DEVICE_DATA_HW_BUILT_IN       DeviceDataHwBuiltIn1[] = {
    MPT_BUILD_IN_DATA
};

//
// Describes Local APICs' connections.
//
STATIC DEVICE_DATA_HW_LOCAL_INT      DeviceDataHwLocalInt1[] = {
  {
    {{0},{{0xFF,0},{0xFF,0},{0xFF,0}}},
    0x00,
    0xff,
    0x00,
    EfiLegacyMpTableEntryLocalIntTypeExtInt,
    EfiLegacyMpTableEntryLocalIntFlagsPolaritySpec,
    EfiLegacyMpTableEntryLocalIntFlagsTriggerSpec
  },
  {
    {{0},{{0xFF,0},{0xFF,0},{0xFF,0}}},
    0x00,
    0xff,
    0x01,
    EfiLegacyMpTableEntryLocalIntTypeInt,
    EfiLegacyMpTableEntryLocalIntFlagsPolaritySpec,
    EfiLegacyMpTableEntryLocalIntFlagsTriggerSpec
  },
};

//
// Describes system's address space mapping, specific to the system.
//
STATIC DEVICE_DATA_HW_ADDR_SPACE_MAPPING DeviceDataHwAddrSpace1[] = {
  //
  // Legacy IO addresses.
  //
  { {0}, EfiLegacyMpTableEntryExtSysAddrSpaceMappingIo,       0x0000,     0x1000    },
  /*
  { {0}, EfiLegacyMpTableEntryExtSysAddrSpaceMappingMemory,   0xXXXXXXXX, 0xXXXXXXXX},
  { {0}, EfiLegacyMpTableEntryExtSysAddrSpaceMappingPrefetch, 0xXXXXXXXX, 0xXXXXXXXX},
  */
};

//
// Describes PCI slot relationship with the IO APICs.
//
STATIC DEVICE_DATA_HW_PCI_SLOT       DeviceDataHwPciSlotUpdate1[] = { 
    MPT_SLOT_DATA_UPDATE
};

//
// Describes built-in device relationship with the IO APICs.
//
STATIC DEVICE_DATA_HW_BUILT_IN       DeviceDataHwBuiltInUpdate1[] = {
    MPT_BUILD_IN_DATA_UPDATE
};

//
// IRQ priority
//
STATIC EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY  IrqPriorityTable1[] = {
  11,
  0,
  10,
  0,
  9,
  0,
  5,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

//
// Bus translation table
//
STATIC UINT16 BusTranslationTbl1 [] = {
    BUS_TRANSLATION_DATA
};

//
// Note : UpdateBusNumbers updates the bus numeber
//
STATIC EFI_LEGACY_PIRQ_TABLE  PirqTableHead1 [] = {
  {
    {
      EFI_PIRQ_TABLE_SIGNATURE,
      00,
      01,
      0000,
      00,
      00,
      0000,
      V_INTEL_VID,
      30,
      00000000,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00,
      00
    },
    {
      PIRQ_DATA
    }
  }
};

//
// Instantiation of the system device data.
//
DEVICE_DATA           mDeviceDataPlatformSRP = {
  DeviceDataHwIoApic1,     sizeof (DeviceDataHwIoApic1)   / sizeof (DeviceDataHwIoApic1[0]),
  DeviceDataHwLocalInt1,   sizeof (DeviceDataHwLocalInt1) / sizeof (DeviceDataHwLocalInt1[0]),
  DeviceDataHwBuiltIn1,    sizeof (DeviceDataHwBuiltIn1)  / sizeof (DeviceDataHwBuiltIn1[0]),
  DeviceDataHwPciSlot1,    sizeof (DeviceDataHwPciSlot1)  / sizeof (DeviceDataHwPciSlot1[0]),
  DeviceDataHwAddrSpace1,  sizeof (DeviceDataHwAddrSpace1)/ sizeof (DeviceDataHwAddrSpace1[0])
};

//
// Instantiation of the system update device data.
//
DEVICE_UPDATE_DATA    mDeviceUpdateDataPlatformSRP = {
  DeviceDataHwPciSlotUpdate1,  sizeof (DeviceDataHwPciSlotUpdate1)  / sizeof (DeviceDataHwPciSlotUpdate1[0]),
  DeviceDataHwBuiltInUpdate1,  sizeof (DeviceDataHwBuiltInUpdate1)  / sizeof (DeviceDataHwBuiltInUpdate1[0])
};

//
// Instantiation of platform PIRQ data.
//
PLATFORM_PIRQ_DATA    mPlatformPirqDataPlatformSRP = {
  IrqPriorityTable1,    sizeof(IrqPriorityTable1) / sizeof(IrqPriorityTable1[0]),
  PirqTableHead1,       sizeof(PirqTableHead1) / sizeof(PirqTableHead1[0]),
  BusTranslationTbl1,   sizeof(BusTranslationTbl1) / sizeof(BusTranslationTbl1[0])
};

