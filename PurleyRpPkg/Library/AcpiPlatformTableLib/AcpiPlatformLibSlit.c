/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2016, Intel Corporation.

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


  @file AcpiPlatformLibSlit.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"

#pragma optimize ("",off)

extern struct SystemMemoryMapHob    *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL         *mIioUds;


EFI_STATUS
PatchSLitTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   ) 
{ 
  UINT8                                       Index;
  UINT8                                       NodeCount;
  UINT8                                       SourceNode;
  UINT8                                       MaxTableEntries;
  UINT8                                       NumCpus;
  UINT8                                       NumClusters;
  UINT8                                       SourceSocket;
  UINT8                                       SourceCluster;
  UINT8                                       TargetSocket;
  UINT8                                       TargetCluster;
  UINT8                                       PeerSocket;
  UINT8                                       LinkValid;
  UINT8                                       PeerSocId;
  EFI_ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE  *SLitAcpiTable;

  NumCpus = mIioUds->IioUdsPtr->SystemStatus.numCpus;
  NumClusters = mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster;
  if (NumClusters == 0) {
    NumClusters = 1;
  }
  NodeCount = NumCpus * NumClusters;

  SLitAcpiTable = (EFI_ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE *)Table;

  //
  // Modify SLIT table
  //
  //DEBUG(( EFI_D_ERROR, "SLIT: Updating SLIT! NumEntries: %d, SncEn: %d, OutNumClusters: %d, NumCpus: %d\n", mSystemMemoryMap->numberEntries, mIioUds->IioUdsPtr->SystemStatus.OutSncEn, mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster, mIioUds->IioUdsPtr->SystemStatus.numCpus ));

  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Changes done to publish only valid SLIT entries
  //
  
  //SLitAcpiTable->Header.Header.Length = sizeof(EFI_ACPI_SYSTEM_LOCALITY_INFORMATION_TABLE);
  MaxTableEntries = (NodeCount*NodeCount);
  SLitAcpiTable->Header.Header.Length = \
		  sizeof(EFI_ACPI_3_0_SYSTEM_LOCALITY_DISTANCE_INFORMATION_TABLE_HEADER) + \
		             ( MaxTableEntries*sizeof(EFI_ACPI_NUMBER_OF_SYSTEM_LOCALITIES_STRUCTURE) );
  //
  // APTIOV_SERVER_OVERRIDE_RC_END : Changes done to publish only valid SLIT entries
  //
  
  SLitAcpiTable->Header.NumberOfSystemLocalities = NodeCount;

  //1) Initialize all entries to 0xFF
  Index = 0;
  while (Index < EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT) {
    SLitAcpiTable->NumSlit[Index].Entry = 0xFF;
    Index++;
  }

  //2) Update SLIT table with data about nodes on same socket
  for (SourceNode = 0; SourceNode < NodeCount; SourceNode++) {
    SourceSocket = SourceNode / NumClusters;
    SourceCluster = SourceNode % NumClusters;
    for (TargetSocket = 0; TargetSocket < NumCpus; TargetSocket++) {
      for (TargetCluster = 0; TargetCluster < NumClusters; TargetCluster++) {
        Index = (SourceNode * (NodeCount)) + (TargetSocket * NumClusters) + TargetCluster;
        if (SourceSocket == TargetSocket) {
          //Source and target are nodes on same socket
          if (SourceCluster == TargetCluster) {
            //a) Same socket same cluster
            SLitAcpiTable->NumSlit[Index].Entry = ZERO_HOP;
          } else {
            //b) Same socket different cluster
            SLitAcpiTable->NumSlit[Index].Entry = ONE_ONE;
          }
        }
      }
    }
  }

  //3) Update table with links between sockets by examining PeerInfo structure
  for (SourceNode = 0; SourceNode < NodeCount; SourceNode++) {
    SourceSocket = SourceNode / NumClusters;
    SourceCluster = SourceNode % NumClusters;
    for (PeerSocket = 0; PeerSocket < MAX_KTI_PORTS; PeerSocket++) {
      LinkValid = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SourceSocket].PeerInfo[PeerSocket].Valid;
      PeerSocId = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SourceSocket].PeerInfo[PeerSocket].PeerSocId;
      if (LinkValid && (PeerSocId < NumCpus)) {
        //Link found between source and peer socket, assign 1 hop to this table entry
        TargetSocket = PeerSocId;
        for (TargetCluster = 0; TargetCluster < NumClusters; TargetCluster++) {
          Index = (SourceNode * (NodeCount)) + (TargetSocket * NumClusters) + TargetCluster;
          SLitAcpiTable->NumSlit[Index].Entry = ONE_HOP;
        }
      }
    }
  }

  //4) Fill in the rest of the SLit table, 2 hops between any remaining valid nodes
  Index = 0;
  MaxTableEntries = NodeCount * NodeCount;
  DEBUG((EFI_D_INFO, "SLIT Dump:\n"));
  while (Index < MaxTableEntries) {
    if (SLitAcpiTable->NumSlit[Index].Entry == 0xFF) {
      //This entry has not been filled yet, assign 2 hop to this table entry
      SLitAcpiTable->NumSlit[Index].Entry = TWO_HOP;
    }
    
    //Print the entire SLIT table:
    DEBUG((EFI_D_INFO, "%02X ", SLitAcpiTable->NumSlit[Index].Entry));
    if ((Index % NodeCount) == (NodeCount - 1)) {
      DEBUG((EFI_D_INFO, "\n"));
    }
    Index++;
  }
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Changes done to publish only valid SLIT entries
  //
#if 0  
  //5) Zero out the unused nodes in SLIT.
  for (Index = MaxTableEntries; Index < EFI_ACPI_SYSTEM_LOCALITIES_ENTRY_COUNT; Index++) {
    SLitAcpiTable->NumSlit[Index].Entry = 0;
  }
#endif

  //
  // APTIOV_SERVER_OVERRIDE_RC_END : Changes done to publish only valid SLIT entries
  //
  
  return EFI_SUCCESS;
}

