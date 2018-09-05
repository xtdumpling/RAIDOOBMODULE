//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code for CPU Miscellaneous MSRs programming Interface Feature

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  SktScopeMSRInit.c

**/

#include "Cpu.h"
#include "MpService.h"
#include "Protocol/CpuCsrAccess.h"
#include <Library/PlatformHooksLib.h> //for IsSimicsPlatform()

extern  MP_SYSTEM_DATA                      mMPSystemData;
extern  CPU_CONFIG_CONTEXT_BUFFER           mCpuConfigContextBuffer;

extern  UINT64                              mCpuIioLlcWaysBitMask;
extern  UINT64                              mCpuExpandedIioLlcWaysBitMask;
extern  UINT64                              mCpuRemoteWaysBitMask;
extern  UINT64                              mCpuQlruCfgBitMask;
extern  BOOLEAN                             mCpuHaswellFamilyFlag;
extern  BOOLEAN                             mCpuSkylakeFamilyFlag;
extern  UINT64                              mCpuRemoteRequestBitMask;
extern  UINT32                              mCpuCapid5PcuFun3[MAX_SOCKET];


/**
    This function returns TRUE if current CPU is HSX server CPU, FALSE otherwise

    @param None

    @retval  TRUE if current CPU is HSX server CPU, FALSE otherwise

**/
BOOLEAN
IsHsxCpu (
  )
{

  EFI_CPUID_REGISTER      CpuidRegisters;
  UINT32                  FamilyId;
  UINT32                  ModelId;
  UINT32                  ExtendedFamilyId = 0;
  UINT32                  ExtendedModelId = 0;

/*
  This simiple version does not work because  CPU_FAMILY_HSX is defined in a file that should not be included by this file
    AsmCpuid (EFI_CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);

    if ((CpuidRegisters.RegEax >> 4) == CPU_FAMILY_HSX) {  
      return TRUE;
    }
    else {
      return FALSE;
    }
  */

  AsmCpuid (EFI_CPUID_VERSION_INFO, &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  //
  // The Extended Family ID needs to be examined only when the Family ID is 0FH.
  // If Family ID is 0FH, Family ID = Family ID + Extended Family ID.
  //
  FamilyId = BitFieldRead32 (CpuidRegisters.RegEax, 8, 11);
    if (FamilyId == 0x0f) {
        ExtendedFamilyId = BitFieldRead32 (CpuidRegisters.RegEax, 20, 27);
      //FamilyId += ExtendedFamilyId;
    }

  //
  // The Extended Model ID needs to be examined only when the Family ID is 06H or 0FH.
  // If Family ID is 06H or 0FH, Model ID = Model ID + (Extended Model ID << 4).
  //
    ModelId = BitFieldRead32 (CpuidRegisters.RegEax, 4, 7);
  if (FamilyId == 0x06 || FamilyId == 0x0f) {
    ExtendedModelId = BitFieldRead32 (CpuidRegisters.RegEax, 16, 19);
    ModelId += (ExtendedModelId << 4);
  }

  FamilyId += ExtendedFamilyId;

  if (mCpuHaswellFamilyFlag && IS_HASWELL_SERVER_PROC (FamilyId, ModelId)) {
    return TRUE;
  } else {
    return FALSE;
  }
}



/**

    This function programs package-scope MSR CBO_SLICE0_CR_IIO_LLC_WAYS

    @param ProcessorNumber -   Current Processor Number
    @param SocketId        -  Current Porcessor's socket ID

    @retval None

**/
VOID
ProgramIioLlcWaysMsr (
         UINTN ProcessorNumber, 
         UINT8 SocketId
  )
{
  UINT32    Data32;
  UINT64    MsrQword;
  UINT64    Ingress_Spare;
  UINT64    RRQ_VMsr;
  UINT32    RegEax;
  UINT8     CpuStepping;
  UINT32    curCPUID;

  if (SocketId == 0) {
    DEBUG ((EFI_D_ERROR, "   :: Socket %2d Processor # %2d in ProgramIioLlcWaysMsr()\n", SocketId, ProcessorNumber));
  }

  //
  // Check if the MSR is supported, i.e., (capid5.iio_llcconfig_en) 
  //
  Data32 = mCpuCapid5PcuFun3[SocketId];
  if (Data32 & BIT1) {
    //
    // Proggram MSR CBO_SLICE0_CR_IIO_LLC_WAYS based on PCD bitmask data
    //
    if (mCpuIioLlcWaysBitMask) {
      MsrQword = AsmReadMsr64 (MSR_CBO_SLICE0_CR_IIO_LLC_WAYS);
      MsrQword &= ~MASK_CBO_SLICE0_CR_IIO_LLC_WAYS;
      MsrQword |= mCpuIioLlcWaysBitMask & MASK_CBO_SLICE0_CR_IIO_LLC_WAYS;
      AsmWriteMsr64 (MSR_CBO_SLICE0_CR_IIO_LLC_WAYS, MsrQword);
      if (SocketId == 0) {
        DEBUG ((EFI_D_INFO, "    :: Set MSR_CBO_SLICE0_CR_IIO_LLC_WAYS  = %x\n", MsrQword));
      }
      //
      // 4929403: BIOS programming and knob for expanded-IO ways
      // Need to program the upper 11 bits of IIO_LLC_WAYS into Ingress_Spare MSR
      //
      Ingress_Spare = AsmReadMsr64 (MSR_CBO_SLICE0_CR_INGRESS_SPARE);
      Ingress_Spare &= ~MASK_CBO_SLICE0_CR_INGRESS_SPARE;
      Ingress_Spare |= (MsrQword >> 12) & MASK_CBO_SLICE0_CR_INGRESS_SPARE;
      AsmWriteMsr64 (MSR_CBO_SLICE0_CR_INGRESS_SPARE, Ingress_Spare);
      if (SocketId == 0) {
        DEBUG ((EFI_D_INFO, "    :: Set MSR_CBO_SLICE0_CR_INGRESS_SPARE = %x\n", Ingress_Spare));
      }
    }
    if (mCpuExpandedIioLlcWaysBitMask) {
      Ingress_Spare = AsmReadMsr64 (MSR_CBO_SLICE0_CR_INGRESS_SPARE);
      Ingress_Spare &= ~MASK_CBO_SLICE0_CR_INGRESS_SPARE;
      Ingress_Spare |= mCpuExpandedIioLlcWaysBitMask & MASK_CBO_SLICE0_CR_INGRESS_SPARE;
      AsmWriteMsr64 (MSR_CBO_SLICE0_CR_INGRESS_SPARE, Ingress_Spare);
      if (SocketId == 0) {
        DEBUG ( (EFI_D_INFO, "    :: Set MSR_CBO_SLICE0_CR_INGRESS_SPARE = %x\n", Ingress_Spare));
      }
    }

    //
    // 4930178: BIOS programming and knob for remote_ways
    //
    if (mCpuRemoteWaysBitMask) {
      Ingress_Spare = AsmReadMsr64 (MSR_CBO_SLICE0_CR_INGRESS_SPARE);
      Ingress_Spare &= ~MASK_CBO_SLICE0_CR_REMOTE_WAYS;
      Ingress_Spare |= (mCpuRemoteWaysBitMask << 16) & MASK_CBO_SLICE0_CR_REMOTE_WAYS;
      AsmWriteMsr64 (MSR_CBO_SLICE0_CR_INGRESS_SPARE, Ingress_Spare);
      if (SocketId == 0) {
        DEBUG ( (EFI_D_INFO, "    :: Set MSR_CBO_SLICE0_CR_INGRESS_SPARE = %x\n", Ingress_Spare));
      }
    }

    //
    // 4930188: Ucode Virtual MSR needed to configure for huge multisocket perf 
    //          imbalance caused by remote reads filling TOR. 
    //          This is for SKX B0+
    //
    AsmCpuid (0x01, &RegEax, NULL, NULL, NULL);
    curCPUID    = (UINT32)((RegEax & 0xFFFF0) >> 4);
    CpuStepping = (UINT8)(RegEax & 0xF);
    if ((curCPUID == CPU_FAMILY_SKX) && (CpuStepping >= B0_CPU_STEP)) {
      if (mCpuRemoteRequestBitMask) {
        RRQ_VMsr = AsmReadMsr64 (MSR_RRQ_VIRTUAL_MSR);
        RRQ_VMsr &= ~MASK_RRQ_VIRTUAL_MSR_RRQ_CT;
        RRQ_VMsr |= (mCpuRemoteRequestBitMask << 1) & MASK_RRQ_VIRTUAL_MSR_RRQ_CT;
        RRQ_VMsr |= B_RRQ_VIRTUAL_MSR_LOCK;
        AsmWriteMsr64 (MSR_RRQ_VIRTUAL_MSR, RRQ_VMsr);
        if (SocketId == 0) {
          DEBUG ( (EFI_D_INFO, "    :: Set MSR_RRQ_VIRTUAL_MSR = %x\n", RRQ_VMsr));
        }
      }
    }
  }
}

/**

    This function programs package-scope MSR VIRTUAL_MSR_CR_QLRU_CONFIG

    @param ProcessorNumber -   Current Processor Number
    @param SocketId        -  Current Porcessor's socket ID

    @retval None

**/
VOID
ProgramQlruMsr (
         UINTN ProcessorNumber, 
         UINT8 SocketId
  )
/**
  This function programs package-scope MSR VIRTUAL_MSR_CR_QLRU_CONFIG
++*/

{
  UINT32    Data32;
  UINT64    MsrQword;

  if (SocketId == 0) {
    DEBUG ((EFI_D_INFO, "   :: Socket %2d Processor # %2d in ProgramIioQlruMsr()\n", SocketId, ProcessorNumber));
  }
  //
  // Check if the MSRs is supported, i.e.,   capid5.iio_llcconfig_en  = 1
  //
  Data32 = mCpuCapid5PcuFun3[SocketId];
  if (Data32 & BIT1) {
    //
    // Proggram MSR VIRTUAL_MSR_CR_QLRU_CONFIG based on PCD bitmask data
    //
    if (mCpuQlruCfgBitMask) {
       MsrQword = AsmReadMsr64 (MSR_VIRTUAL_MSR_CR_QLRU_CONFIG);
       MsrQword &= ~MASK_VIRTUAL_MSR_CR_QLRU_CONFIG;
       MsrQword |= mCpuQlruCfgBitMask & MASK_VIRTUAL_MSR_CR_QLRU_CONFIG;
       AsmWriteMsr64 (MSR_VIRTUAL_MSR_CR_QLRU_CONFIG, MsrQword);
       if (SocketId == 0) {
         DEBUG ( (EFI_D_INFO, "    :: Set MSR_VIRTUAL_MSR_CR_QLRU_CONFIG = %x\n", MsrQword));
       }
    }
  }
}


VOID
EarlyMpInitPbsp (
  UINTN ProcessorNumber
  )
 /**

  This function is called on the package BSP thread to perform package-scope programming.

  Assumptions:
     mCpuConfigContextBuffer has been initialized with proper data

  @param ProcessorNumber:   Current processor number

  @retval None

 **/
{
  UINT8 SocketId;
  if( mCpuConfigContextBuffer.CollectedDataBuffer[ProcessorNumber].PackageBsp) {

    SocketId = (UINT8) mCpuConfigContextBuffer.CollectedDataBuffer[ProcessorNumber].ProcessorLocation.Package;

    if (SocketId == 0) {
      DEBUG ( (EFI_D_INFO, " \n:: Processor # %d, Package %d BSP in EarlyMpInitPbsp() \n", ProcessorNumber, SocketId));
    }

    if (IsHsxCpu ()) {
      ProgramIioLlcWaysMsr (ProcessorNumber, SocketId);
      ProgramQlruMsr (ProcessorNumber, SocketId);
    } else {
      //
      // SKX CPU
      //
      ProgramIioLlcWaysMsr (ProcessorNumber, SocketId);
    }
  }
  return;
}


