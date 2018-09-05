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


  @file AcpiPlatformLibApic.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"

#pragma optimize("",off)

extern EFI_PLATFORM_INFO           *mPlatformInfo;
extern BIOS_ACPI_PARAM             *mAcpiParameter;
extern EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;

extern UINTN                mNumberOfCPUs;
extern UINT32               mNumOfBitShift;
extern BOOLEAN              mForceX2ApicId;

extern BOOLEAN                     mX2ApicEnabled;
extern BOOLEAN                     mCpuOrderSorted = FALSE;

extern EFI_CPU_ID_ORDER_MAP        mCpuApicIdOrderTable[];
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;

EFI_MP_SERVICES_PROTOCOL    *mMpService;
BOOLEAN                     mCpuOrderSorted;
EFI_CPU_ID_ORDER_MAP        mCpuApicIdOrderTable[MAX_CPU_NUM];
UINTN                       mNumberOfCPUs = 0;
UINTN                       mNumberOfEnabledCPUs = 0;

// following are possible APICID Map
static const UINT32 ApicIdMapA[] = {  //for SKUs have number of core > 16
  //it is 14 + 14 + 14 + 14 format
  0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007,
  0x00000008, 0x00000009, 0x0000000A, 0x0000000B, 0x0000000C, 0x0000000D, 0x00000010, 0x00000011,
  0x00000012, 0x00000013, 0x00000014, 0x00000015, 0x00000016, 0x00000017, 0x00000018, 0x00000019,
  0x0000001A, 0x0000001B, 0x0000001C, 0x0000001D, 0x00000020, 0x00000021, 0x00000022, 0x00000023,
  0x00000024, 0x00000025, 0x00000026, 0x00000027, 0x00000028, 0x00000029, 0x0000002A, 0x0000002B,
  0x0000002C, 0x0000002D, 0x00000030, 0x00000031, 0x00000032, 0x00000033, 0x00000034, 0x00000035,
  0x00000036, 0x00000037, 0x00000038, 0x00000039, 0x0000003A, 0x0000003B, 0x0000003C, 0x0000003D
};

static const UINT32 ApicIdMapB[] = { //for SKUs have number of cores <= 16 use 32 ID space
  //it is 16+16 format
  0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007,
  0x00000008, 0x00000009, 0x0000000A, 0x0000000B, 0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F,
  0x00000010, 0x00000011, 0x00000012, 0x00000013, 0x00000014, 0x00000015, 0x00000016, 0x00000017,
  0x00000018, 0x00000019, 0x0000001A, 0x0000001B, 0x0000001C, 0x0000001D, 0x0000001E, 0x0000001F,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};


static const UINT32 ApicIdMapC[] = { //for SKUs have number of cores <= 16 use 64 ID space
  //it is 16+0+16+0 format
  0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007,
  0x00000008, 0x00000009, 0x0000000A, 0x0000000B, 0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F,
  0x00000020, 0x00000021, 0x00000022, 0x00000023, 0x00000024, 0x00000025, 0x00000026, 0x00000027,
  0x00000028, 0x00000029, 0x0000002A, 0x0000002B, 0x0000002C, 0x0000002D, 0x0000002E, 0x0000002F,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

static const UINT32 ApicIdMapD[] = { //for SKUs have number of cores <= 8 use 16 ID space
  //it is 16 format
  0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007,
  0x00000008, 0x00000009, 0x0000000A, 0x0000000B, 0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

const UINT32 *mApicIdMap = NULL;

/**
  This function detect the APICID map and update ApicID Map pointer

  @param None

  @retval VOID

**/
VOID DetectApicIdMap(VOID)
{

  CAPID7_PCU_FUN3_STRUCT  	      LlcCoreCapId;
  UINT32                  CoreCount;
  UINT32                  i;


  CoreCount = 0;

  if(mApicIdMap != NULL) {
    return;   //aleady initialized
  }

  mApicIdMap = ApicIdMapA;  // default to > 16C SKUs

  LlcCoreCapId.Data = mCpuCsrAccess->ReadCpuCsr(0, 0, CAPID7_PCU_FUN3_REG);

  if(IsSimicsPlatform()) {  //SKX TODO: remove if Simics{ ... } when Simics sighting 5166111 resolved
    LlcCoreCapId.Data = mCpuCsrAccess->ReadCpuCsr(0, 0, FUSED_CORES_PCU_FUN3_REG);
  }

  for(i = 0; i < MAX_CORE; i++) {
    if((LlcCoreCapId.Bits.llc_ia_core_en >> i) & 1) {
      CoreCount++;
    }
  }

  //DEBUG((EFI_D_ERROR, ":: Default to use Map A @ %08X FusedCoreCount: %02d, sktlevel: %d\n",mApicIdMap, CoreCount, mNumOfBitShift));
  ASSERT (CoreCount != 0);

  if(CoreCount <= 16) {

    if(mNumOfBitShift == 4) {
      mApicIdMap = ApicIdMapD;
      //DEBUG((EFI_D_ERROR, ":: Use Map D @ %08X\n",mApicIdMap));
    }

    if(mNumOfBitShift == 5) {
      mApicIdMap = ApicIdMapB;
      //DEBUG((EFI_D_ERROR, ":: Use Map B @ %08X\n",mApicIdMap));
    }

    if(mNumOfBitShift == 6) {
      mApicIdMap = ApicIdMapA;
      //DEBUG((EFI_D_ERROR, ":: Use Map A @ %08X\n",mApicIdMap));
    }

  }

  return;
}

/**
  This function return the CoreThreadId of ApicId from ACPI ApicId Map array

  @param ApicId

  @retval Index of ACPI ApicId Map array

**/
UINT32
GetIndexFromApicId (
  UINT32 ApicId
  )
{
  UINT32 CoreThreadId;
  UINT32 i;

  ASSERT (mApicIdMap != NULL);
  if(mApicIdMap == NULL){
    DEBUG ((EFI_D_ERROR, "\n mApicIdMap NULL pointer.\n"));
    return 0;
  }

  CoreThreadId = ApicId & ((1 << mNumOfBitShift) - 1);

  for(i = 0; i < (MAX_CORE * MAX_THREAD); i++) {
    if(mApicIdMap[i] == CoreThreadId) {
      break;
    }
  }

  ASSERT (i <= (MAX_CORE * MAX_THREAD));

  return i;
}

/**

    GC_TODO: add routine description

    @param ApicId - GC_TODO: add arg description

    @retval Index       - GC_TODO: add retval description
    @retval (UINT32) -1 - GC_TODO: add retval description

**/
UINT32
ApicId2SwProcApicId (
  UINT32 ApicId
  )
{
  UINT32 Index;

  for (Index = 0; Index < MAX_CPU_NUM; Index++) {
    if ((mCpuApicIdOrderTable[Index].Flags == 1) && (mCpuApicIdOrderTable[Index].ApicId == ApicId)) {
      return Index;
    }
  }

  return (UINT32) -1;

}

/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
DebugDisplayReOrderTable(
  VOID
  )
{
  UINT32 Index;

  DEBUG ((EFI_D_ERROR, "Index  AcpiProcId  ApicId  Flags  SwApicId  Skt\n"));
  for (Index=0; Index<MAX_CPU_NUM; Index++) {
    DEBUG ((EFI_D_ERROR, " %02d       0x%02X      0x%02X      %d      0x%02X     %d\n",
                           Index, mCpuApicIdOrderTable[Index].AcpiProcessorId,
                           mCpuApicIdOrderTable[Index].ApicId,
                           mCpuApicIdOrderTable[Index].Flags,
                           mCpuApicIdOrderTable[Index].SwProcApicId,
                           mCpuApicIdOrderTable[Index].SocketNum));
  }


}


/**

    GC_TODO: add routine description

    @param ApicPtr          - GC_TODO: add arg description
    @param LocalApicCounter - GC_TODO: add arg description

    @retval Status - GC_TODO: add retval description

**/
EFI_STATUS
AppendCpuMapTableEntry (
    IN VOID   *ApicPtr,
    IN UINT32 LocalApicCounter
  )
{
  EFI_STATUS    Status;
  EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE *LocalApicPtr;
  EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE *LocalX2ApicPtr;
  UINT8         Type;

  Status = EFI_SUCCESS;
  Type = ((ACPI_APIC_STRUCTURE_PTR *)ApicPtr)->AcpiApicCommon.Type;
  LocalApicPtr = (EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC_STRUCTURE *)(&((ACPI_APIC_STRUCTURE_PTR *)ApicPtr)->AcpiLocalApic);
  LocalX2ApicPtr = (EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE *)(&((ACPI_APIC_STRUCTURE_PTR *)ApicPtr)->AcpiLocalx2Apic);

  if(Type == EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC) {
    if(!mX2ApicEnabled) {
      LocalApicPtr->Flags           = (UINT8)mCpuApicIdOrderTable[LocalApicCounter].Flags;
      LocalApicPtr->ApicId          = (UINT8)mCpuApicIdOrderTable[LocalApicCounter].ApicId;
      LocalApicPtr->AcpiProcessorId = (UINT8)mCpuApicIdOrderTable[LocalApicCounter].AcpiProcessorId;
    } else {
      LocalApicPtr->Flags           = 0;
      LocalApicPtr->ApicId          = 0xFF;
      LocalApicPtr->AcpiProcessorId = (UINT8)0xFF;
      Status = EFI_UNSUPPORTED;
    }
  } else if(Type == EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC) {
    if(mX2ApicEnabled) {
      LocalX2ApicPtr->Flags            = (UINT8)mCpuApicIdOrderTable[LocalApicCounter].Flags;
      LocalX2ApicPtr->X2ApicId         = mCpuApicIdOrderTable[LocalApicCounter].ApicId;
      LocalX2ApicPtr->AcpiProcessorUid = mCpuApicIdOrderTable[LocalApicCounter].AcpiProcessorId;
    } else {
      LocalX2ApicPtr->Flags            = 0;
      LocalX2ApicPtr->X2ApicId         = (UINT32)-1;
      LocalX2ApicPtr->AcpiProcessorUid = (UINT32)-1;
      Status = EFI_UNSUPPORTED;
    }
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;

}

/**

    GC_TODO: add routine description

    @param MpService    - GC_TODO: add arg description
    @param NumberOfCPUs - GC_TODO: add arg description

    @retval Status - GC_TODO: add retval description

**/
EFI_STATUS
SortCpuLocalApicInTable (
  VOID
  )
{
  EFI_STATUS                                Status;
  EFI_PROCESSOR_INFORMATION                 ProcessorInfoBuffer;
  UINTN                                     BufferSize;
  UINT32                                    Index;
  UINT32                                    CurrProcessor;
  UINT32                                    BspApicId;
  UINT32                                    TempVal = 0;
  EFI_CPU_ID_ORDER_MAP                      *CpuIdMapPtr;
  UINT32                                    CoreThreadMask;

  BufferSize = 0;
  Index      = 0;
  Status     = EFI_SUCCESS;

  CoreThreadMask = (UINT32) ((1 << mNumOfBitShift) - 1);

  if(!mCpuOrderSorted) {

    //Init ProcessorBitMask table
    for (Index = 0; Index < MAX_SOCKET; Index++) {
        mAcpiParameter->ProcessorBitMask[Index] = 0;
    }

    Index  = 0;

    for (CurrProcessor = 0; CurrProcessor < mNumberOfCPUs; CurrProcessor++) {
      Status = mMpService->GetProcessorInfo (
                                            mMpService,
                                            CurrProcessor,
                                            &ProcessorInfoBuffer
                                            );

      if ((ProcessorInfoBuffer.StatusFlag & PROCESSOR_ENABLED_BIT) != 0) {
        if(ProcessorInfoBuffer.ProcessorId & 1) { //is 2nd thread
          CpuIdMapPtr = (EFI_CPU_ID_ORDER_MAP *)&mCpuApicIdOrderTable[(Index - 1) + MAX_CPU_NUM / 2];
        } else { //is primary thread
          CpuIdMapPtr = (EFI_CPU_ID_ORDER_MAP *)&mCpuApicIdOrderTable[Index];
          Index++;
        }
        CpuIdMapPtr->ApicId  = (UINT32)ProcessorInfoBuffer.ProcessorId;
        CpuIdMapPtr->Flags   = ((ProcessorInfoBuffer.StatusFlag & PROCESSOR_ENABLED_BIT) != 0);
        CpuIdMapPtr->SocketNum = (UINT32)ProcessorInfoBuffer.Location.Package;
        CpuIdMapPtr->AcpiProcessorId = (CpuIdMapPtr->SocketNum * MAX_CORE *MAX_THREAD) + GetIndexFromApicId(CpuIdMapPtr->ApicId); //CpuIdMapPtr->ApicId;
        CpuIdMapPtr->SwProcApicId = ((UINT32)(ProcessorInfoBuffer.Location.Package << mNumOfBitShift) + (((UINT32)ProcessorInfoBuffer.ProcessorId) & CoreThreadMask));
        if(mX2ApicEnabled) { //if X2Apic, re-order the socket # so it starts from base 0 and contiguous
          //may not necessory!!!!!
        }

        //update processorbitMask
        if (CpuIdMapPtr->Flags == 1) {

          if(mForceX2ApicId) {
            CpuIdMapPtr->SocketNum &= 0x7;
            CpuIdMapPtr->AcpiProcessorId &= 0xFF; //keep lower 8bit due to use Proc obj in dsdt
            CpuIdMapPtr->SwProcApicId &= 0xFF;
          }
          mAcpiParameter->ProcessorBitMask[CpuIdMapPtr->SocketNum] |= LShiftU64(1, ((ProcessorInfoBuffer.Location.Core * 2) + ProcessorInfoBuffer.Location.Thread));
        }
      } else {  //not enabled
        CpuIdMapPtr = (EFI_CPU_ID_ORDER_MAP *)&mCpuApicIdOrderTable[Index];
        CpuIdMapPtr->ApicId  = (UINT32)-1;
        CpuIdMapPtr->Flags   = 0;
        CpuIdMapPtr->AcpiProcessorId = (UINT32)-1;
        CpuIdMapPtr->SwProcApicId = (UINT32)-1;
        CpuIdMapPtr->SocketNum = (UINT32)-1;
      } //end if PROC ENABLE
    } //end for CurrentProcessor

    //keep for debug purpose
    //DEBUG(( EFI_D_ERROR, "::ACPI::  APIC ID Order Table Init.   CoreThreadMask = %x,  mNumOfBitShift = %x\n", CoreThreadMask, mNumOfBitShift));
    // DEBUG(( EFI_D_ERROR, "::ACPI::                              GT16 = %x\n",  mAcpiParameter->IsGt16CoreSku));
    //DebugDisplayReOrderTable();
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorBitMask[0]: %x %x\n", (mAcpiParameter->ProcessorBitMask[0] >> 32), mAcpiParameter->ProcessorBitMask[0]));
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorBitMask[1]: %x %x\n", (mAcpiParameter->ProcessorBitMask[1] >> 32), mAcpiParameter->ProcessorBitMask[1]));
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorBitMask[2]: %x %x\n", (mAcpiParameter->ProcessorBitMask[2] >> 32), mAcpiParameter->ProcessorBitMask[2]));
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorBitMask[3]: %x %x\n", (mAcpiParameter->ProcessorBitMask[3] >> 32), mAcpiParameter->ProcessorBitMask[3]));
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorApicIdBase[0]: %x\n", mAcpiParameter->ProcessorApicIdBase[0]));
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorApicIdBase[1]: %x\n", mAcpiParameter->ProcessorApicIdBase[1]));
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorApicIdBase[2]: %x\n", mAcpiParameter->ProcessorApicIdBase[2]));
    //DEBUG ((EFI_D_ERROR, "::ACPI::  ProcessorApicIdBase[3]: %x\n", mAcpiParameter->ProcessorApicIdBase[3]));

    //make sure 1st entry is BSP
    if(mX2ApicEnabled) {
      BspApicId = (UINT32)AsmReadMsr64(0x802);
    } else {
      BspApicId = (*(volatile UINT32 *)(UINTN)0xFEE00020) >> 24;
    }

    if(mCpuApicIdOrderTable[0].ApicId != BspApicId) {
      //check to see if 1st entry is BSP, if not swap it
      Index = ApicId2SwProcApicId(BspApicId);

      if(MAX_CPU_NUM <= Index) {
        DEBUG ((EFI_D_ERROR, "Asserting the SortCpuLocalApicInTable Index Bufferflow\n"));
        ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
      }

      TempVal = mCpuApicIdOrderTable[Index].ApicId;
      mCpuApicIdOrderTable[Index].ApicId = mCpuApicIdOrderTable[0].ApicId;
      mCpuApicIdOrderTable[0].ApicId = TempVal;
      mCpuApicIdOrderTable[Index].Flags = mCpuApicIdOrderTable[0].Flags;
      mCpuApicIdOrderTable[0].Flags = 1;
      TempVal = mCpuApicIdOrderTable[Index].SwProcApicId;
      mCpuApicIdOrderTable[Index].SwProcApicId = mCpuApicIdOrderTable[0].SwProcApicId;
      mCpuApicIdOrderTable[0].SwProcApicId = TempVal;
      //swap AcpiProcId
      TempVal = mCpuApicIdOrderTable[Index].AcpiProcessorId;
      mCpuApicIdOrderTable[Index].AcpiProcessorId = mCpuApicIdOrderTable[0].AcpiProcessorId;
      mCpuApicIdOrderTable[0].AcpiProcessorId = TempVal;

    }

    //Make sure no holes between enabled threads
    for(CurrProcessor = 0; CurrProcessor < MAX_CPU_NUM; CurrProcessor++) {

      if(mCpuApicIdOrderTable[CurrProcessor].Flags == 0) {
        //make sure disabled entry has ProcId set to FFs
        mCpuApicIdOrderTable[CurrProcessor].ApicId = (UINT32)-1;
        mCpuApicIdOrderTable[CurrProcessor].AcpiProcessorId = (UINT32)-1;
        mCpuApicIdOrderTable[CurrProcessor].SwProcApicId = (UINT32)-1;

        for(Index = CurrProcessor+1; Index < MAX_CPU_NUM; Index++) {
          if(mCpuApicIdOrderTable[Index].Flags == 1) {
            //move enabled entry up
            mCpuApicIdOrderTable[CurrProcessor].Flags = 1;
            mCpuApicIdOrderTable[CurrProcessor].ApicId = mCpuApicIdOrderTable[Index].ApicId;
            mCpuApicIdOrderTable[CurrProcessor].AcpiProcessorId = mCpuApicIdOrderTable[Index].AcpiProcessorId;
            mCpuApicIdOrderTable[CurrProcessor].SwProcApicId = mCpuApicIdOrderTable[Index].SwProcApicId;
            mCpuApicIdOrderTable[CurrProcessor].SocketNum = mCpuApicIdOrderTable[Index].SocketNum;
            //disable moved entry
            mCpuApicIdOrderTable[Index].Flags = 0;
            mCpuApicIdOrderTable[Index].ApicId = (UINT32)-1;
            mCpuApicIdOrderTable[Index].AcpiProcessorId = (UINT32)-1;
            mCpuApicIdOrderTable[Index].SwProcApicId = (UINT32)-1;
            break;
          }
        }
      }
    }

    //keep for debug purpose
    DEBUG ((EFI_D_ERROR, "APIC ID Order Table ReOrdered\n"));
    DebugDisplayReOrderTable();

    mCpuOrderSorted = TRUE;
  }

  return Status;
}

EFI_STATUS
PatchMadtTable(
   IN OUT   EFI_ACPI_COMMON_HEADER          *MadtTable
   )
{
  UINT8                                     *CurrPtr = NULL;
  UINT8                                     *EndPtr  = NULL;
  ACPI_APIC_STRUCTURE_PTR                   *ApicPtr = NULL;
  EFI_STATUS                                Status;
  UINT32                                    LocalApicCounter = 0;
  UINT8                                     CurrSkt = 0, CurrStack = 0;

  DetectApicIdMap();

  // Call for Local APIC ID Reorder
  SortCpuLocalApicInTable ();


  CurrPtr = (UINT8 *) &((EFI_ACPI_DESCRIPTION_HEADER *) MadtTable)[1];
  CurrPtr = CurrPtr + 8;
  //
  // Size of Local APIC Address & Flag
  //
  EndPtr  = (UINT8 *) MadtTable;
  EndPtr  = EndPtr + MadtTable->Length;
  while (CurrPtr < EndPtr) {
    ApicPtr = (ACPI_APIC_STRUCTURE_PTR *) CurrPtr;
    switch (ApicPtr->AcpiApicCommon.Type) {
    case EFI_ACPI_4_0_PROCESSOR_LOCAL_APIC:
    case EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC:
      //
      // Fix for Ordering of MADT to be maintained as it is in MADT table.
      //
      // Update processor enabled or disabled and keep the local APIC
      // order in MADT intact
      //
      // Sanity check to make sure proc-id is not arbitrary
      //
      // If X2Apic disabled and Local APIC type X2APIC then return
      if (!mX2ApicEnabled && (ApicPtr->AcpiApicCommon.Type == EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC))
          break;
      ASSERT (ApicPtr->AcpiLocalApic.AcpiProcessorId < MAX_CPU_NUM);
      Status = AppendCpuMapTableEntry (
                  &(ApicPtr->AcpiLocalApic),
                  LocalApicCounter);

      if(Status == EFI_SUCCESS) {
        LocalApicCounter++;
        ASSERT (LocalApicCounter <= MAX_CPU_NUM);
      }
      //
      // If no APIC-ID match, the cpu may not be populated
      //
      break;

    case EFI_ACPI_4_0_IO_APIC:
      //
      // IO APIC entries can be patched here
      // Check if the IOAPIC has been enabled
      // Not enabled, remove it from the MADT by marking the entry as reserved
      // If enabled, set IO APIC base address and increment for the next enabled one
      // 0xFFFF8000 mask is used due to first IOAPIC being at 0x1000 offset (ex. 0xFEC01000)
      //
      if (ApicPtr->AcpiIoApic.IoApicId >= PC00_IOAPIC_ID) {
#if MAX_SOCKET > 4
        CurrSkt = (ApicPtr->AcpiIoApic.IoApicId - PC00_IOAPIC_ID) / (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET);
        CurrStack = (ApicPtr->AcpiIoApic.IoApicId - PC00_IOAPIC_ID) % (TOTAL_SYSTEM_IIO_STACKS / MAX_SOCKET);
#else
        CurrSkt = (ApicPtr->AcpiIoApic.IoApicId - PC00_IOAPIC_ID) / MAX_IIO_STACK;
        CurrStack = (ApicPtr->AcpiIoApic.IoApicId - PC00_IOAPIC_ID) % MAX_IIO_STACK;
#endif
      }

      if (CurrSkt >= MAX_SOCKET) {
        ApicPtr->AcpiIoApic.Type = 127;
        ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        break;
      }
        
      switch (ApicPtr->AcpiIoApic.IoApicId) {
      case PCH_IOAPIC_ID:
#if MAX_SOCKET <= 4
        if ((mPlatformInfo->SysData.SysIoApicEnable & PCH_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        }
#endif
        break;
      case PC00_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC00_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = PC00_IOAPIC_ADDRESS;          
        }
        break;
      case PC01_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC01_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC02_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC02_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC03_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC03_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC04_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC04_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC05_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC05_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC06_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC06_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC07_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC07_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC08_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC08_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC09_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC09_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC10_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC10_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC11_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC11_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC12_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC12_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC13_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC13_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC14_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC14_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC15_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC15_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC16_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC16_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC17_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC17_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC18_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC18_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC19_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC19_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC20_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC20_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC21_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC21_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC22_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC22_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC23_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC23_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC24_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC24_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC25_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC25_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC26_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC26_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC27_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC27_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC28_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC28_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC29_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC29_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC30_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC30_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      case PC31_IOAPIC_ID:
        if ((mPlatformInfo->SysData.SysIoApicEnable & PC31_IOAPIC) == 0) {
          ApicPtr->AcpiIoApic.Type = 127;
          ApicPtr->AcpiIoApic.GlobalSystemInterruptBase = 0;
        } else {
          ApicPtr->AcpiIoApic.IoApicAddress = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].IoApicBase;          
        }
        break;
      default:
        break;
      }
      break;
    }

    CurrPtr = CurrPtr + ApicPtr->AcpiApicCommon.Length;
  }

  return EFI_SUCCESS;
}
