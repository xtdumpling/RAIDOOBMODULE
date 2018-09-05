//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Code File for CPU SNC

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CpuSncInit.c

**/

#include  "MpService.h"
#include <Protocol/CpuCsrAccess.h>
#include <RcRegs.h>
#include "KtiSi.h"
#include <Library/PlatformHooksLib.h> //SKX_TODO for IsSimicsPlatform() will remove when Simic fixIsSimicsPlatform
// APTIOV_SERVER_OVERRIDE_RC_START: To resolve Optimization build error
// Define intrinsic functions to satisfy the .NET 2008 compiler with size optimization /O1
// compiler automatically inserts memcpy/memset fn calls in certain cases
void *memcpy(void *dst, void *src, size_t cnt);
void *memset(void *dst, char value, size_t cnt);
// APTIOV_SERVER_OVERRIDE_RC_END: To resolve Optimization build error

#define MSR_SNC_CONFIG          0x00000152
#define B_SNC_LOCK              BIT28

#define MSR_SNC_RANGE0_BASE     0x00000153
#define MSR_SNC_RANGE1_BASE     0x00000154
#define MSR_SNC_RANGE2_BASE     0x00000155
#define MSR_SNC_RANGE3_BASE     0x00000156
#define MSR_SNC_RANGE4_BASE     0x00000157


typedef struct _SNC_CSR_DATA_STRUCT {
  UINT32  SncConfig;
  UINT32  SncBase[5];
} SNC_CSR_DATA_STRUCT;

typedef union _SNC_MSR_DATA_STRUCT {
  UINT64  Qword;

struct _DWORDS {
  UINT32  Low;
  UINT32  High;
} Dwords;
} SNC_MSR_DATA_STRUCT;

typedef struct _SNC_REG_DATA_STRUCT {
  SNC_CSR_DATA_STRUCT  CsrRegs[MAX_SOCKET];
  SNC_MSR_DATA_STRUCT  MsrRegs;
} SNC_REG_DATA_STRUCT;

extern EFI_CPU_CSR_ACCESS_PROTOCOL         *mCpuCsrAccess;
extern EFI_IIO_UDS_PROTOCOL                *mIioUds;

SNC_REG_DATA_STRUCT    mSncRegs;

/**

    GC_TODO: add routine description

    @param  None

    @retval None

**/
VOID
CpuGetSncCsrs (
  VOID
)
{
  UINT8    Socket, Cluster;

  for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
    mSncRegs.CsrRegs[Socket].SncConfig = 0;
    for (Cluster = 0; Cluster <= MAX_CLUSTERS; ++Cluster) {
      mSncRegs.CsrRegs[Socket].SncBase[Cluster] = 0;
    }
  }

  for(Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].Valid) {
      // Get SNC registers in CHABC
      mSncRegs.CsrRegs[Socket].SncConfig = mCpuCsrAccess->ReadCpuCsr(Socket, 0, SNC_CONFIG_CHABC_PMA_REG);
      for (Cluster = 0; Cluster <= MAX_CLUSTERS; ++Cluster) {
        mSncRegs.CsrRegs[Socket].SncBase[Cluster] = mCpuCsrAccess->ReadCpuCsr(Socket, 0, SNC_BASE_1_CHABC_PMA_REG + (Cluster * 4)) & 0x0000FFFF;
      }
    }
  } //end of socket
  return;
}


/**

    GC_TODO: add routine description

    @param - None

    @retval None

**/
VOID
CpuProgramSncMsrs (
  UINTN   ProcessorNumber
  )
{
  UINT32               ApicId;
  UINT32               Socket;
  SNC_MSR_DATA_STRUCT  MsrValue;
 
  ApicId = mCpuConfigContextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData.ApicID;
  Socket = mCpuConfigContextBuffer.CollectedDataBuffer[ProcessorNumber].ProcessorLocation.Package;

  if((ApicId & 0x1) == 0) { //only let primary thread in the core to do the work
    MsrValue.Qword = AsmReadMsr64(MSR_SNC_CONFIG);

    if((MsrValue.Dwords.Low & 3) != 0) { //if SNC not enabled, don't program MSRs

      MsrValue.Qword = AsmReadMsr64(MSR_SNC_RANGE0_BASE);
      MsrValue.Dwords.Low = mSncRegs.CsrRegs[Socket].SncBase[0] & 0x0000FFFF;
      AsmWriteMsr64(MSR_SNC_RANGE0_BASE, MsrValue.Qword);
      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, MSR_SNC_RANGE0_BASE, 0, 64, MsrValue.Qword);

      MsrValue.Qword = AsmReadMsr64(MSR_SNC_RANGE1_BASE);
      MsrValue.Dwords.Low = mSncRegs.CsrRegs[Socket].SncBase[1] & 0x0000FFFF;
      AsmWriteMsr64(MSR_SNC_RANGE1_BASE, MsrValue.Qword);
      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, MSR_SNC_RANGE1_BASE, 0, 64, MsrValue.Qword);

      MsrValue.Qword = AsmReadMsr64(MSR_SNC_RANGE2_BASE);
      MsrValue.Dwords.Low = mSncRegs.CsrRegs[Socket].SncBase[2] & 0x0000FFFF;
      AsmWriteMsr64(MSR_SNC_RANGE2_BASE, MsrValue.Qword);
      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, MSR_SNC_RANGE2_BASE, 0, 64, MsrValue.Qword);

      MsrValue.Qword = AsmReadMsr64(MSR_SNC_RANGE3_BASE);
      MsrValue.Dwords.Low = mSncRegs.CsrRegs[Socket].SncBase[3] & 0x0000FFFF;
      AsmWriteMsr64(MSR_SNC_RANGE3_BASE, MsrValue.Qword);
      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, MSR_SNC_RANGE3_BASE, 0, 64, MsrValue.Qword);

      MsrValue.Qword = AsmReadMsr64(MSR_SNC_RANGE4_BASE);
      MsrValue.Dwords.Low = mSncRegs.CsrRegs[Socket].SncBase[4] & 0x0000FFFF;
      AsmWriteMsr64(MSR_SNC_RANGE4_BASE, MsrValue.Qword);
      //save for S3
      WriteRegisterTable(ProcessorNumber, Msr, MSR_SNC_RANGE4_BASE, 0, 64, MsrValue.Qword);
    }
    
    MsrValue.Qword = AsmReadMsr64(MSR_SNC_CONFIG);
    MsrValue.Dwords.Low = mSncRegs.CsrRegs[Socket].SncConfig;
    MsrValue.Dwords.Low |= B_SNC_LOCK;
    AsmWriteMsr64(MSR_SNC_CONFIG, MsrValue.Qword);

    //save for S3
    WriteRegisterTable(ProcessorNumber, Msr, MSR_SNC_CONFIG, 0, 64, MsrValue.Qword);

  }
  return;

}

/**
  This function program CPU SNC MSRs

  @param  None

  @retval None

**/
VOID
EFIAPI
CpuSncInitialize (
  VOID
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;

  GetProcessorVersionInfo (mCpuConfigContextBuffer.BspNumber, &FamilyId, &ModelId, NULL, NULL);
  if ((FeaturePcdGet (PcdCpuSkylakeFamilyFlag) && IS_SKYLAKE_PROC (FamilyId, ModelId))) {
    if (IsSimicsPlatform() == TRUE) { //SKX TODO, will remove this when Simics fixed
      return;             //also remove include header in this file and PlatformLib in inf
    }

    CpuGetSncCsrs();

    DispatchAPAndWait (
      TRUE,
      0,
      CpuProgramSncMsrs
    );

    CpuProgramSncMsrs(mCpuConfigContextBuffer.BspNumber);

  }
  return;
}

