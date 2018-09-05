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


  @file AcpiPlatformLibSrat.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


#include <Library/CpuConfigLib.h>

#define MSR_LOGICAL_THREAD_ID       0x53

extern BOOLEAN                      mX2ApicEnabled;
extern struct SystemMemoryMapHob    *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL         *mIioUds;
extern SOCKET_MEMORY_CONFIGURATION  mSocketMemoryConfiguration;
extern UINT32                       mNumOfBitShift;
extern const UINT32                 *mApicIdMap;
extern UINTN                        mNumberOfCPUs;
extern EFI_CPU_ID_ORDER_MAP         mCpuApicIdOrderTable[];
UINT32 mPackageChaTotal[MAX_SOCKET];

struct CPU_LOGICAL_THREAD_ID_TABLE{
  UINT32 ApicId;
  UINT32 Msr53value;
  UINT32 SNCProximityDomain;
}mCpuMsr53ValueTable[MAX_CPU_NUM];

VOID CollectMsr53Data();
VOID PrintMsr53ValueTable();
UINT32 GetProximityDomainForSNC(UINT32 ApicId);


/**

    Update the SRAT APIC IDs.

    @param *SRAAcpiTable   - The table to be set

    @retval EFI_SUCCESS -  Returns Success

**/
EFI_STATUS
PatchSRATableAllApicIds(
   IN OUT   EFI_ACPI_4_0_STATIC_RESOURCE_AFFINITY_TABLE  *SRAAcpiTable
   ) 
{
  UINT16                      CpuThreadIndex;   // Support more than 256 threads (8S case)
  UINT8                       *ApicTablePtr;
  UINT8                       socket;
  UINT8                       CoreThreadIndex;
  ApicTablePtr = (UINT8 *)mApicIdMap;
  CpuThreadIndex = 0;
  for (socket = 0; socket < MAX_SOCKET; socket++) {
    //Even IDs must be list first
    for (CoreThreadIndex = 0; CoreThreadIndex < MAX_CORE * MAX_THREAD; CoreThreadIndex += 2) {
      SRAAcpiTable->Apic[CpuThreadIndex].ApicId = (UINT8)mApicIdMap[CoreThreadIndex] + (socket << mNumOfBitShift);
      SRAAcpiTable->Apic[CpuThreadIndex].ProximityDomain = socket; //as ProxDomain are all 0, or we can come up some algorithm if there is any dependency
      if ((UINT8)mApicIdMap[CoreThreadIndex] != 0xff) {
        SRAAcpiTable->Apic[CpuThreadIndex].Flags = EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
      }
      CpuThreadIndex++;
    }  //end of for coreThreadIndex
  } //end for socket

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    // for odd APICID and must be after all even APICIDs
    for (CoreThreadIndex = 1; CoreThreadIndex < MAX_CORE * MAX_THREAD; CoreThreadIndex += 2) {
      SRAAcpiTable->Apic[CpuThreadIndex].ApicId = (UINT8)mApicIdMap[CoreThreadIndex] + (socket << mNumOfBitShift);
      SRAAcpiTable->Apic[CpuThreadIndex].ProximityDomain = socket; //as ProxDomain are all 0, or we can come up some algorithm if there is any dependency
      if ((UINT8)mApicIdMap[CoreThreadIndex] != 0xff) {
        SRAAcpiTable->Apic[CpuThreadIndex].Flags = EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
      }
      CpuThreadIndex++;
    }
  }  //end of for coreThreadIndex
  ASSERT(CpuThreadIndex == MAX_CPU_NUM);
  return EFI_SUCCESS;
}

EFI_STATUS
PatchSratTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   )
{
  UINT8                                     Index;
  UINT8                                     NewIndex;
  UINT8                                     LegacyNodeId;
  UINT16                                    CpuThreadIndex;   // Support more than 256 threads (8S case)
  UINT8                                     NodeId;
  UINT64                                    LastEntryBase;
  UINT64                                    MemoryAddress;
  UINT64                                    MemorySize;
  UINTN                                     HighTopMemory;
  UINTN                                     HotPlugBase;
  UINTN                                     HotPlugLen;
  EFI_ACPI_4_0_STATIC_RESOURCE_AFFINITY_TABLE *SratTable;

  SratTable = (EFI_ACPI_4_0_STATIC_RESOURCE_AFFINITY_TABLE *)Table;

  CollectMsr53Data();

  if (mSystemMemoryMap != NULL) {


    if (mSocketMemoryConfiguration.SratCpuHotPlug) {
      PatchSRATableAllApicIds(SratTable);
    } else {
      DEBUG(( EFI_D_INFO, "-------- SRAT TABLE ---------- %x\n",  EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT));
      for (CpuThreadIndex = 0; CpuThreadIndex < EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE_COUNT; CpuThreadIndex++) {
          
        if (CpuThreadIndex < mNumberOfCPUs) {
          if(mX2ApicEnabled) {
            //
            // If SNC is enabled and 2 clusters, there is 1 extra Proximity Domain per socket
            // SNC cannot exist unless all HA's have memory
            //
            if(mIioUds->IioUdsPtr->SystemStatus.OutSncEn && (mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster ==2)) {
              //
              // Get the Proximity Domain from the SortedMsr53ValueTable
              //
              SratTable->x2Apic[CpuThreadIndex].ProximityDomain = (UINT8)GetProximityDomainForSNC(mCpuApicIdOrderTable[CpuThreadIndex].ApicId);
            } else {
              SratTable->x2Apic[CpuThreadIndex].ProximityDomain = (UINT8)mCpuApicIdOrderTable[CpuThreadIndex].SocketNum;
            }
            SratTable->x2Apic[CpuThreadIndex].X2ApicId = mCpuApicIdOrderTable[CpuThreadIndex].ApicId;
            SratTable->x2Apic[CpuThreadIndex].Flags = EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
          } else {

            //
            // If SNC is enabled and 2 clusters, there is 1 extra Proximity Domain per socket
            // SNC cannot exist unless all HA's have memory
            //
            if (mIioUds->IioUdsPtr->SystemStatus.OutSncEn && (mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster == 2)) {
              //
              // Get the Proximity Domain from the SortedMsr53ValueTable
              //
              SratTable->Apic[CpuThreadIndex].ProximityDomain = (UINT8)GetProximityDomainForSNC(mCpuApicIdOrderTable[CpuThreadIndex].ApicId);

            } else {
              SratTable->Apic[CpuThreadIndex].ProximityDomain = (UINT8)mCpuApicIdOrderTable[CpuThreadIndex].SocketNum;
            }

            SratTable->Apic[CpuThreadIndex].ApicId = (UINT8)mCpuApicIdOrderTable[CpuThreadIndex].ApicId;
            SratTable->Apic[CpuThreadIndex].Flags = EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED;
          }
        } else {
          if(mX2ApicEnabled) {
            SratTable->x2Apic[CpuThreadIndex].X2ApicId = (UINT32)-1;
          } else {
            SratTable->Apic[CpuThreadIndex].ApicId = 0xFF;
          }
        }

        if(mX2ApicEnabled) {
            DEBUG(( EFI_D_INFO, "\nSRAT: CpuThreadIndex\t%x, ApicId\t%x,  Flags\t%x!\n",  CpuThreadIndex, SratTable->x2Apic[CpuThreadIndex].X2ApicId,SratTable->x2Apic[CpuThreadIndex].Flags));
        } else {
            DEBUG(( EFI_D_INFO, "\nSRAT: CpuThreadIndex\t%x, ApicId\t%x,  Flags\t%x, ProximityDomain\t%x!\n",  CpuThreadIndex, (UINT8)mCpuApicIdOrderTable[CpuThreadIndex].ApicId,SratTable->Apic[CpuThreadIndex].Flags, SratTable->Apic[CpuThreadIndex].ProximityDomain ));
        }   
      }
    }

    LegacyNodeId = 0xFF;
    LastEntryBase = (UINT64)(-1);
    DEBUG(( EFI_D_INFO, "\nSRAT: Updating SRAT memory information!\n" ));
    for (Index = 0; Index < mSystemMemoryMap->numberEntries; Index++) {
      NodeId = mSystemMemoryMap->Element[Index].NodeId;

      ASSERT (NodeId >= 0 && NodeId < MC_MAX_NODE);

      if (Index >= EFI_ACPI_MEMORY_AFFINITY_STRUCTURE_COUNT) {
        ASSERT (0);
        break;
      }

      //
      // As the HOB has base addr in 64 MB chunks
      //
      MemoryAddress = ((UINT64)mSystemMemoryMap->Element[Index].BaseAddress << MEM_ADDR_SHFT_VAL);
      if (MemoryAddress == LastEntryBase) {
        //
        // Skip duplicate entries if applicable
        //
        continue;
      }

      SratTable->Memory[Index].AddressBaseLow = (UINT32)(MemoryAddress & 0xFFFFFFFF);
      SratTable->Memory[Index].AddressBaseHigh  = (UINT32)((UINT64)MemoryAddress >> 32);
      LastEntryBase = MemoryAddress;

      //
      // As the HOB has Length in 64 MB chunks
      //
      MemorySize  = ((UINT64)mSystemMemoryMap->Element[Index].ElementSize << MEM_ADDR_SHFT_VAL);
      SratTable->Memory[Index].LengthLow = (UINT32)(MemorySize & 0xFFFFFFFF);
      SratTable->Memory[Index].LengthHigh = (UINT32)((UINT64)MemorySize >> 32);

      //
      // Find Proximity Domain from NodeId
      // NodeId = {0, 4, 1, 5, 2, 6, 3, 7} for 8 Memory controllers on 4 socket SNC/2Cluster system
      //
      if ((mIioUds->IioUdsPtr->SystemStatus.OutSncEn != 0) && (mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster == 2)){
        //
        // In SNC cases, interbitmap is either 0x01 or 0x10. imcinterbitmap will always have two bits
        //
        SratTable->Memory[Index].ProximityDomain = (mSystemMemoryMap->Element[Index].SocketId * MAX_IMC) + (mSystemMemoryMap->Element[Index].ImcInterBitmap >> 1); 
      }
      else{      
        //
        // Fill in Proximity Domain for memory node
        //
        SratTable->Memory[Index].ProximityDomain = mSystemMemoryMap->Element[Index].SocketId; // This is new and fixed for purley based on the new memmap changes
      }

      if (MemoryAddress == 0 && MemorySize > 0) {
        LegacyNodeId = NodeId;
      }

      //
      // Enable the Memory structure
      //
      if ((LegacyNodeId == NodeId) || (!mSocketMemoryConfiguration.SratMemoryHotPlug)) {
        SratTable->Memory[Index].Flags = EFI_ACPI_MEMORY_ENABLED;
        DEBUG(( EFI_D_INFO, "\nEntry %d in SRAT Enabled. Base - %08x%08x, Length - %08x%08x Proximity - %d\n", Index,
                (UINT64)SratTable->Memory[Index].AddressBaseHigh,
                SratTable->Memory[Index].AddressBaseLow,
                (UINT64)SratTable->Memory[Index].LengthHigh,
                SratTable->Memory[Index].LengthLow,
                SratTable->Memory[Index].ProximityDomain));
      } else {
        SratTable->Memory[Index].Flags = EFI_ACPI_MEMORY_ENABLED | EFI_ACPI_MEMORY_HOT_REMOVE_SUPPORTED;
      }
    }

    HotPlugBase = 0x100;
    if (mSocketMemoryConfiguration.MemoryHotPlugBase == 0) { // Auto
      // Read the actual TOHM and set it as the hot memory base
      HighTopMemory  = (UINT64)mIioUds->IioUdsPtr->SystemStatus.tohmLimit << 26;
      HotPlugBase = (HighTopMemory >> 32);
      if ((UINT32)HighTopMemory > 0) {
        HotPlugBase++;
      }
    } else if (mSocketMemoryConfiguration.MemoryHotPlugBase) { // Number
      HotPlugBase = mSocketMemoryConfiguration.MemoryHotPlugBase * 0x80;
    }
    HotPlugLen = (mSocketMemoryConfiguration.MemoryHotPlugLen + 1) * 0x10;

    if (mSocketMemoryConfiguration.SratMemoryHotPlug) {
      for (NodeId = 0; NodeId < MC_MAX_NODE; NodeId++) {
        NewIndex = Index + NodeId;
        if (NewIndex >= EFI_ACPI_MEMORY_AFFINITY_STRUCTURE_COUNT) {
          ASSERT (0);
          break;
        }
        //
        // As the HOB has base addr in 1 GB chunks
        //
        SratTable->Memory[NewIndex].ProximityDomain = (NodeId >> 1);
        SratTable->Memory[NewIndex].AddressBaseLow  = 0;
        SratTable->Memory[NewIndex].AddressBaseHigh = (UINT32)(HotPlugBase +  NodeId * HotPlugLen);
        SratTable->Memory[NewIndex].LengthLow  = 0;
        SratTable->Memory[NewIndex].LengthHigh = (UINT32)HotPlugLen;
        SratTable->Memory[NewIndex].Flags = EFI_ACPI_MEMORY_ENABLED | EFI_ACPI_MEMORY_HOT_REMOVE_SUPPORTED;
      }
    }
  }
  return EFI_SUCCESS;
}

/**

    Find the processor index of the thread running and obtain MSR 53 and ApicId data
    to populate mCpuMsr53ValueTable array.

    @param None

    @retval None

**/
VOID
FillMsr53ValueTable(
  VOID
)
{
  UINTN     ProcessorNumber;

  mMpService->WhoAmI(
                mMpService,
                &ProcessorNumber
                );

  mCpuMsr53ValueTable[ProcessorNumber].Msr53value = AsmReadMsr32(MSR_LOGICAL_THREAD_ID);

  mCpuMsr53ValueTable[ProcessorNumber].ApicId   = mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[ProcessorNumber].CpuMiscData.ApicID;

}

VOID CollectMsr53Data()
/*++

  Routine Description:

    Collect MSR53 Value for all the AP's and sorts it by the MSR53 Value. 
    The Sorted Table, for every socket the first half of the threads will be Mapped to 1st HA
    The second half f threads in every socket will mapped to 2nd HA.

  Arguments:

    NONE

  Returns:

    NONE

--*/{

  UINT32 SocketIndex;
  UINT32 ThreadIndex;


  //
  // Initialize with safe defaults
  //
  for(ThreadIndex = 0; ThreadIndex < MAX_CPU_NUM; ThreadIndex++) {
    mCpuMsr53ValueTable[ThreadIndex].ApicId = (UINT32)-1;
    mCpuMsr53ValueTable[ThreadIndex].Msr53value = 0xFF;
    mCpuMsr53ValueTable[ThreadIndex].SNCProximityDomain = 0;
  }

  //
  // Get total number of CHAs per socket
  //
  for(SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    mPackageChaTotal[SocketIndex] = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].TotCha;
  }

  //
  // Collect MSR 53 value and ApicId for each thread
  //
  mMpService->StartupAllAPs (
                mMpService,
                (EFI_AP_PROCEDURE)FillMsr53ValueTable,
                FALSE,
                NULL,
                0,
                NULL,
                NULL
                );

  FillMsr53ValueTable();

  //
  // Set SNC2 Proximity Domain variables for each thread
  //
  for(ThreadIndex = 0; ThreadIndex < MAX_CPU_NUM; ThreadIndex++) {
    if (mCpuMsr53ValueTable[ThreadIndex].Msr53value == 0xff) {
      continue;
    }

    //
    // We divide lower half logical thread IDs to cluster 0 and upper half to cluster 1.
    // CHA number is used instead of active threads to account for when cores or
    // threads are disabled.
    //
    SocketIndex = mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[ThreadIndex].ProcessorLocation.Package;
    if (mCpuMsr53ValueTable[ThreadIndex].Msr53value < mPackageChaTotal[SocketIndex]) {
      mCpuMsr53ValueTable[ThreadIndex].SNCProximityDomain = (SocketIndex * 2);
    } else {
      mCpuMsr53ValueTable[ThreadIndex].SNCProximityDomain = (SocketIndex * 2) + 1;
    }
  }
  //DEBUG((EFI_D_ERROR, "SORTED CPU LOGICAL THREAD ID TABLE with proximity domain flag for 1st and 2nd HA updated\n"));
  PrintMsr53ValueTable();

}

VOID PrintMsr53ValueTable(){

  UINT32 SocketIndex, ThreadIndex;

  for(SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++){
    DEBUG((EFI_D_INFO, "Socket %d: mPackageChaTotal %d\n", SocketIndex, mPackageChaTotal[SocketIndex]));
  }

  DEBUG((EFI_D_ERROR, "Socket\t Thread#\t ApicId\t Msr53Id\t SNC2ProximityDomain\n"));
  for(ThreadIndex = 0; ThreadIndex < MAX_CPU_NUM; ThreadIndex++){
    if (mCpuMsr53ValueTable[ThreadIndex].Msr53value != 0xff) {
      SocketIndex = mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[ThreadIndex].ProcessorLocation.Package;
      DEBUG((EFI_D_INFO, "%d\t 0x%x\t 0x%x\t 0x%x\t %d\n", SocketIndex, ThreadIndex, mCpuMsr53ValueTable[ThreadIndex].ApicId, mCpuMsr53ValueTable[ThreadIndex].Msr53value, mCpuMsr53ValueTable[ThreadIndex].SNCProximityDomain));
    }
  }
}

//Return Value - Proximity Domain Flag 
//- 0 Denotes upper half of the sorted thread needs to be mapped to 1st HA. 
//1 Denotes lower half and mapped to 2nd HA
UINT32 GetProximityDomainForSNC(
  UINT32 ApicId)
/*++

  Routine Description:

    Return the Domain Flag value of the specific APICID

  Arguments:

    NONE

  Returns:

    Proximity Domain Value for the thread within a Socket.

--*/
{
  UINT32 ThreadIndex     = 0;
  static UINT32 SncIndex = 0;
  //
  // The ApicIds are in order. Saving the index will reduce loop iterations
  //
  for(ThreadIndex = SncIndex; ThreadIndex < MAX_CPU_NUM; ThreadIndex++) {
    if(mCpuMsr53ValueTable[ThreadIndex].ApicId == ApicId){
      SncIndex = ThreadIndex + 1;
      return mCpuMsr53ValueTable[ThreadIndex].SNCProximityDomain;
    } else if (mCpuMsr53ValueTable[ThreadIndex].ApicId == (UINT32) -1) {
      //
      // We have reached the end of the populated threads
      //
      break;
    }
  }

  //
  // Start again from the beginning if we made it to the end of the array
  //
  for(ThreadIndex = 0; ThreadIndex < MAX_CPU_NUM; ThreadIndex++) {
    if(mCpuMsr53ValueTable[ThreadIndex].ApicId == ApicId){
      SncIndex = ThreadIndex + 1;
      return mCpuMsr53ValueTable[ThreadIndex].SNCProximityDomain;
    }
  }

  DEBUG((EFI_D_ERROR, "APICID not found in CpuMsr53ValueTable\n"));
  ASSERT(FALSE);
  ThreadIndex--;
  return mCpuMsr53ValueTable[ThreadIndex].SNCProximityDomain;
}