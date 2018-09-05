/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 2016, Intel Corporation.

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


  @file AcpiPlatformLibPmtt.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"
extern struct SystemMemoryMapHob   *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;

EFI_STATUS
PatchPlatformMemoryTopologyTable (
  IN OUT   EFI_ACPI_COMMON_HEADER  *Table
  )
{
  EFI_STATUS                              Status = EFI_SUCCESS;
  UINT8                                   Socket, Channel, Dimm;
  UINT64                                  TempOemTableId;
  BOOLEAN                                 SmBiosProtocolFound = FALSE;
  SMBIOS_TABLE_TYPE17                     *SmbiosType17Record;
  EFI_SMBIOS_HANDLE                       SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                     *mSmbios;
  EFI_SMBIOS_TYPE                         SmbiosType;
  EFI_SMBIOS_TABLE_HEADER                 *SmbiosRecord;
  EFI_ACPI_PLATFORM_MEMORY_TOPOLOGY_TABLE *PmttTable = (EFI_ACPI_PLATFORM_MEMORY_TOPOLOGY_TABLE*)Table;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;

  DEBUG((EFI_D_INFO,"\n  <PMTT ACPI TABLE> \n"));
  DEBUG((EFI_D_INFO,"\n  mPmttAcpiTable Addr:  %lx \n", PmttTable));

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &mSmbios);
  if (!(EFI_ERROR(Status))) {
    SmBiosProtocolFound = TRUE;
  }

  // Update OEM Id and OEM Table Id
  TempOemTableId = PcdGet64(PcdAcpiDefaultOemTableId);
  CopyMem (PmttTable->Header.OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(PmttTable->Header.OemId));
  CopyMem (&PmttTable->Header.OemTableId, &TempOemTableId, sizeof(PmttTable->Header.OemTableId));

  //
  // Update PMTT main Header
  //
/*
skt0{
    mem Channel{
        dimm
        dimm...
        }
    mem Channel{
        dimm
        dimm...
        }
    }
  ..
  ...
skt1{
    mem{
..
...
}
....
*/
      
  DEBUG((EFI_D_LOAD|EFI_D_INFO,"  mPmttAcpiTable Starting Length:  %lx \n", PmttTable->Header.Length));
  for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mIioUds->IioUdsPtr->SystemStatus.socketPresentBitMap & (1 << Socket)) {
      PmttTable->TopLevelmemAggrDevStruc[Socket].Flag = EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_TLAD | EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_PHYELEMENT | EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_VOLATILE ;
    } else {
      PmttTable->TopLevelmemAggrDevStruc[Socket].Flag = 0 ;
    }
    PmttTable->TopLevelmemAggrDevStruc[Socket].Type = EFI_ACPI_TOP_LEVEL_MEMORY_TYPE_SOCKET;
    PmttTable->TopLevelmemAggrDevStruc[Socket].SckIdent = Socket;
    PmttTable->TopLevelmemAggrDevStruc[Socket].Length   = sizeof(EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC);
  for (Channel = 0; Channel < MAX_CH; Channel++) {
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].Type   = EFI_ACPI_TOP_LEVEL_SKT_MEM_STRUC_TLAD;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].Length = sizeof (EFI_ACPI_MEM_CTRL_STRUC);
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].Flag   = 0 ;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].NumProxDomains  = 01;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].ProxDomain      = 0xFF; //For empty channel
      if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].Enabled) {
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].MemCtrlCharTable.ReadLatency         = PMTT_TBI;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].MemCtrlCharTable.WriteLatency        = PMTT_TBI;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].MemCtrlCharTable.ReadBW              = PMTT_TBI;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].MemCtrlCharTable.WriteBW             = PMTT_TBI;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].MemCtrlCharTable.OptimalAccUnit      = PMTT_TBI;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].MemCtrlCharTable.OptimalAccAlignment = PMTT_TBI;
        PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].ProxDomain      = Socket;

      }

      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].Present == 0) {
          PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].PhyCompIdentStruc[Dimm].Flag   = 0;
        } else {
          PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].PhyCompIdentStruc[Dimm].Flag   = 2 ;
          PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].PhyCompIdentStruc[Dimm].PhyCompIdentifier = (Channel << 0x08) | Dimm;
          PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].PhyCompIdentStruc[Dimm].Size = (mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].DimmSize) * 64; // Dimm Capacity in GB. Memsize is in MB.
        }
          PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].PhyCompIdentStruc[Dimm].Type   = PHYSICAL_COMPONENT_IDENTIFIER_TYPE_DIMM;
          PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].PhyCompIdentStruc[Dimm].Length = sizeof (EFI_ACPI_PHYSICAL_COMP_IDENTIFIER_STRUC);
          if (SmBiosProtocolFound == TRUE) {
            SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
            Status = mSmbios->GetNext (mSmbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
            if (!(EFI_ERROR(Status))) {
              SmbiosType17Record = (SMBIOS_TABLE_TYPE17 *) SmbiosRecord;
              PmttTable->TopLevelmemAggrDevStruc[Socket].MemCtrlStruc[Channel].PhyCompIdentStruc[Dimm].SmBiosHandle = (SmbiosType17Record->Hdr.Handle);
            }
          }
      }   
    }
  }

  return Status;
}

