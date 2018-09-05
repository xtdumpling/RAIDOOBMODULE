/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

  Copyright (c) 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


    @file CpuS3MsrLib.c

**/

#include <PiPei.h>
#include <Cpu/CpuRegs.h>

#include <Ppi/MpService.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/CpuS3MsrLib.h>
#include <Library/PlatformCpuLib.h>

#include <Setup/IioUniversalData.h>

/**
  Programs the MSRs for required Cpu MSRs on every thread

  @param EnabledCpuFeatures - Passes in bitmap of the locks that are enabled

  @retval EFI_SUCCESS  MSR lock operation completed

**/
EFI_STATUS
RestoreCpuMsrs (
  VOID    *EnabledCpuFeatures
  )
{
  UINT64 MsrData = 0;

  if(*(UINT32*)EnabledCpuFeatures & B_PROC_MSR_LOCK_CTRL){
    //
    // Program PMG_CST_CONFIG MSR lock bit for this thread.
    //
    MsrData = AsmReadMsr64(EFI_IA32_PMG_CST_CONFIG);
    if ((MsrData & B_EFI_CST_CONTROL_LOCK) == 0) {
      MsrData |= B_EFI_CST_CONTROL_LOCK;
      AsmWriteMsr64 (EFI_IA32_PMG_CST_CONFIG, MsrData);
    }
    //
    // Program IA32_FEATURE_CONTROL MSR lock bit for this thread.
    //
    MsrData = AsmReadMsr64(EFI_MSR_IA32_FEATURE_CONTROL);
    if ((MsrData & B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
      MsrData |= B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK;
      AsmWriteMsr64 (EFI_MSR_IA32_FEATURE_CONTROL, MsrData);
    }
  }

  //
  // Re-enable Autonomous C-states if enabled in setup
  //
  if(*(UINT32*)EnabledCpuFeatures & B_PROC_MSR_ACC_EN) {
    MsrData = AsmReadMsr64(EFI_IA32_PMG_CST_CONFIG);
    MsrData |= B_EFI_ACC_ENABLE;
    AsmWriteMsr64 (EFI_IA32_PMG_CST_CONFIG, MsrData);
  }

  return EFI_SUCCESS;
}

/**
  Programs MSRs on S3 resume on every package, instead of every thread. Meant
  for package level MSRs that introduce race condition issues in multi-thread
  parallel execution.

  @param EnabledCpuFeatures - Passes in bitmap of the locks that are enabled

  @retval EFI_SUCCESS  MSR lock operation completed

**/
EFI_STATUS
RestoreCpuPkgMsrs (
  VOID    *EnabledCpuFeatures
  )
{
  UINT64 MsrData = 0;
  //
  // Program MISC_PWR_MGMT MSR lock bit for this thread
  //
  MsrData = AsmReadMsr64(EFI_MSR_MISC_PWR_MGMT);
  if ((MsrData & B_EFI_MSR_MISC_PWR_MGMT_LOCK) == 0) {
    MsrData |= B_EFI_MSR_MISC_PWR_MGMT_LOCK;
    AsmWriteMsr64 (EFI_MSR_MISC_PWR_MGMT, MsrData);
  }
  return EFI_SUCCESS;
}

/**

  Restores required MSR settings on S3 Resume. This is used for MSRs
  that must be set late in S3 resume such as lock bits or MSRs that cannot
  be handled by WriteRegisterTable restore method.

  @param PeiServices   - Pointer to PEI Services Table.

  @retval EFI_SUCCESS in all cases.

**/
EFI_STATUS
EFIAPI
S3RestoreCpuMsrs (
  IN      EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS            Status;
  PEI_MP_SERVICES_PPI   *PeiMpServices;
  UINT32                EnabledCpuFeatures = 0;
  EFI_GUID              gUniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  IIO_UDS               *IioUds;
  EFI_HOB_GUID_TYPE     *GuidHob;
  UINT8                 SocketNumber = 0;
  UINT8                 Index;
  UINTN                 NumberOfProcessors;
  UINTN                 NumberEnabledProcessors;
  UINT16                PackageDoneBitmap;
  EFI_PROCESSOR_INFORMATION ProcInfo;

  EnabledCpuFeatures = GetPlatformEnabledCpuFeatures();

  //
  // If ACC is enabled by user, make sure HW supports
  //
  GuidHob = GetFirstGuidHob (&gUniversalDataGuid);
  ASSERT (GuidHob != NULL);
  if(GuidHob == NULL) {
    EnabledCpuFeatures &= ~B_PROC_MSR_ACC_EN;
    SocketNumber = MAX_SOCKET;
  } else {
    IioUds  = GET_GUID_HOB_DATA(GuidHob);
    if (EnabledCpuFeatures & B_PROC_MSR_ACC_EN) {
      if (!IioUds->SystemStatus.CpuAccSupport) {
        EnabledCpuFeatures &= ~B_PROC_MSR_ACC_EN;
      }
    }
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if(IioUds->SystemStatus.socketPresentBitMap & (1 << Index)){
        SocketNumber++;
      }
    }
  }

  Status = (*PeiServices)->LocatePpi(
                      PeiServices,
                      &gEfiPeiMpServicesPpiGuid,
                      0,
                      NULL,
                      &PeiMpServices
                      );

  if(!EFI_ERROR(Status)){
    //
    // Set lock bits on Cpu MSRs on BSP, then APs
    //
    RestoreCpuMsrs((VOID*)&EnabledCpuFeatures);
    Status = PeiMpServices->StartupAllAPs (
              PeiServices,
              PeiMpServices,
              RestoreCpuMsrs,
              FALSE,
              0,
              (VOID*)&EnabledCpuFeatures
              );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Error: Not able to relock Processor MSRs.  Status: %r\n", Status));
    }

    PackageDoneBitmap = 0;
    PeiMpServices->GetNumberOfProcessors(
                  PeiServices,
                  PeiMpServices,
                  &NumberOfProcessors,
                  &NumberEnabledProcessors
                  );
    for(Index = 0; Index < NumberOfProcessors; Index++) {
      PeiMpServices->GetProcessorInfo(
                      PeiServices,
                      PeiMpServices,
                      Index,
                      &ProcInfo
                      );
      //
      // Only call RestoreCpuPkgMsrs on each socket one time
      //
      if(!(PackageDoneBitmap & (1 << ProcInfo.Location.Package)) && (ProcInfo.StatusFlag & PROCESSOR_ENABLED_BIT)){
        PackageDoneBitmap |= (1 << ProcInfo.Location.Package);
        SocketNumber--;
        if(ProcInfo.StatusFlag & PROCESSOR_AS_BSP_BIT) {
          RestoreCpuPkgMsrs((VOID *)&EnabledCpuFeatures);
        } else {
          PeiMpServices->StartupThisAP(PeiServices,
                                    PeiMpServices,
                                    RestoreCpuPkgMsrs,
                                    Index,
                                    0,
                                    (VOID *)&EnabledCpuFeatures
                                    );
        }
        //
        // If we have reached the last socket, break loop
        //
        if(SocketNumber == 0){
          break;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Programs the MSRs for required Cpu MSRs on every thread.

  @param EnabledCpuFeatures - Passes in bitmap of the features enabled

  @retval EFI_NOT_FOUND Not able to locate IIO UDS Hob
  @retval EFI_SUCCESS   Late PEI Cpu initialization operation completed

**/
EFI_STATUS
LatePeiCpuThreadInit (
  VOID    *EnabledCpuFeatures
  )
{
  MSR_REGISTER        MsrData;
  EFI_GUID            gUniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  IIO_UDS             *IioUds;
  EFI_HOB_GUID_TYPE   *GuidHob;

  GuidHob = GetFirstGuidHob (&gUniversalDataGuid);
  ASSERT (GuidHob != NULL);
  if(GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  IioUds  = GET_GUID_HOB_DATA(GuidHob);

  //
  // Set LLC Prefetch with requested value.  Only supported on SKX H0 or later, on patch 0x05+
  //
  if (IioUds->SystemStatus.MinimumCpuStepping >= H0_REV_SKX && IioUds->SystemStatus.cpuType == CPU_SKX) {
    MsrData.Qword = AsmReadMsr64(EFI_MSR_IA32_BIOS_SIGN_ID);
    if ((MsrData.Dwords.High & 0x0FF) >= 0x05) {
      MsrData.Qword = ((*(UINT32*)EnabledCpuFeatures & B_PROC_LLC_PREFETCH_EN) ? 0 : 1);
      AsmWriteMsr64 (MSR_VIRTUAL_MSR_LLC_PREFETCH, MsrData.Qword);
    }
  }

  return EFI_SUCCESS;
}

/**

  Performs any PEI CPU Initialization needed in PEI phase that
  requires multi-threaded execution. This is executed on normal
  boots and S3 resume.

  @param PeiServices   - Pointer to PEI Services Table.

  @retval EFI_SUCCESS in all cases.

**/
EFI_STATUS
EFIAPI
LatePeiCpuInit (
  IN      EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS            Status;
  PEI_MP_SERVICES_PPI   *PeiMpServices;
  UINT32                EnabledCpuFeatures = 0;

  EnabledCpuFeatures = GetPlatformEnabledCpuFeatures();

  Status = (*PeiServices)->LocatePpi(
                      PeiServices,
                      &gEfiPeiMpServicesPpiGuid,
                      0,
                      NULL,
                      &PeiMpServices
                      );

  if(!EFI_ERROR(Status)){
    //
    // Perform late Cpu Pei Init on BSP, then APs
    //
    Status = LatePeiCpuThreadInit((VOID*)&EnabledCpuFeatures);
    if(!EFI_ERROR(Status)) {
      Status = PeiMpServices->StartupAllAPs (
                PeiServices,
                PeiMpServices,
                LatePeiCpuThreadInit,
                FALSE,
                0,
                (VOID*)&EnabledCpuFeatures
                );
    }
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Error: Not able to execute Late PEI thread init.  Status: %r\n", Status));
    }
  }

  return EFI_SUCCESS;
}
