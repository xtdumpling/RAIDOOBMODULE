/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2015, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file AcpiPlatformLibMcfg.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


extern BIOS_ACPI_PARAM              *mAcpiParameter;
extern EFI_IIO_UDS_PROTOCOL         *mIioUds;
extern SYSTEM_CONFIGURATION         mSystemConfiguration;

EFI_STATUS
PatchMcfgAcpiTable (
  IN OUT  EFI_ACPI_COMMON_HEADER  *Table
  )
{
  UINT8     NodeId;
  UINT8     NodeCount;
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *McfgTable;
  
  McfgTable = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE *)Table;

  //
  // mAcpiParameter memory buffer has been zero'ed out, so mAcpiParameter->PcieSegNum[] are 0's
  // Patch \_SB.PSYS.SGEN with User Setup Option data
  //
  //
  // dynamically allow multi-seg support
  //
  mAcpiParameter->PCIe_MultiSeg_Support = 0;
  for (NodeId = 0; NodeId < MAX_SOCKET; NodeId++) {
    if ((UINT16)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment) > 0) {
      mAcpiParameter->PCIe_MultiSeg_Support = 1;
      break;
    }
  }

  //
  // Update MCFG table entries (segment number, base addr and start/end bus numbers)
  //
  if ( mAcpiParameter->PCIe_MultiSeg_Support == 0) {

    // Original code for single  PCIe segment start
    McfgTable->Segment[0].BaseAddress = mIioUds->IioUdsPtr->PlatformData.PciExpressBase;
    McfgTable->Segment[0].EndBusNumber = (UINT8)RShiftU64 (mIioUds->IioUdsPtr->PlatformData.PciExpressSize, 20) - 1;
    // Original code for single  PCIe segment end

    //
    // Single segment with segment number as 0
    //
    McfgTable->Segment[0].PciSegmentGroupNumber = 0;
    NodeCount = 1;

  } else {
    //
    // PCIe Multi-Segment handling - Assume each CPU socket as a segment, and copy Segement info from IioUds HOB to MCFG table entries
    //

    //
    // Segment count = 0
    //
    NodeCount = 0;

    for (NodeId = 0; NodeId < MAX_SOCKET; NodeId++) {

      //
      // Skip a socket if it does not exist or does not contain valid bus range data
      //
      if ( (UINT8)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketLastBus) ==
            (UINT8)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketFirstBus) ) {
        continue; 
      }

      //
      // Copy PCIe Segement info from IioUds HOB to MCFG table entries
      //
      McfgTable->Segment[NodeCount].PciSegmentGroupNumber = (UINT16)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment);

      McfgTable->Segment[NodeCount].BaseAddress = \
        LShiftU64 (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SegMmcfgBase.hi, 32) + \
        (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SegMmcfgBase.lo) ;

      McfgTable->Segment[NodeCount].StartBusNumber = (UINT8)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketFirstBus);

      McfgTable->Segment[NodeCount].EndBusNumber = (UINT8)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketLastBus);

      //
      // Update segment number returned by AML  _SEG() .  It resides in mAcpiParameter region now.
      //
      mAcpiParameter->PcieSegNum[NodeId] = (UINT8)(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment);

      //
      // Update count of valid segments
      //
      NodeCount++;
    }
  }

  //
  // Set MCFG table "Length" field based on the number of PCIe segments enumerated so far
  //
  McfgTable->Header.Header.Length = \
    sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER) + \
    sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE) * NodeCount;

  //
  // Debug dump of MCFG table
  //
  DEBUG(( EFI_D_ERROR, "ACPI MCFG table @ address 0x%x\n", Table ));
  DEBUG(( EFI_D_ERROR, "  Multi-Seg Support = %x\n", mAcpiParameter->PCIe_MultiSeg_Support));
  DEBUG(( EFI_D_ERROR, "  Number of Segments (sockets): %2d\n", NodeCount ));
  DEBUG(( EFI_D_ERROR, "  Table Length = 0x%x\n\n", McfgTable->Header.Header.Length ));
  for (NodeId = 0; NodeId < NodeCount; NodeId ++) {
    DEBUG(( EFI_D_ERROR, "   Segment[%2d].BaseAddress = %x\n",  NodeId, McfgTable->Segment[NodeId].BaseAddress));
    DEBUG(( EFI_D_ERROR, "   Segment[%2d].PciSegmentGroupNumber = %x\n", NodeId, McfgTable->Segment[NodeId].PciSegmentGroupNumber));
    DEBUG(( EFI_D_ERROR, "   Segment[%2d].StartBusNumber = %x\n", NodeId, McfgTable->Segment[NodeId].StartBusNumber));
    DEBUG(( EFI_D_ERROR, "   Segment[%2d].EndBusNumber = %x\n\n", NodeId, McfgTable->Segment[NodeId].EndBusNumber));
  }

  return EFI_SUCCESS;
}