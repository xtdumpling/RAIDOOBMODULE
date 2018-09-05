/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemCacheInit.c

  EFI 2.0 PEIM to set up memory type range registers.

**/

#include "MemCacheInit.h"

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  MemCacheEndOfPeiCallback
};

/**

  This function finds the start address and size of control or Block window region in the system.

  @param host - pointer to sysHost structure on stack
  @param memtype - Type of the memory range
  @param RangeSize - pointer to the variable to store the StartAddress
  @param RangeSize - pointer to the variable to store RangeSize

  @retval EFI_SUCCESS - success
          EFI_NOT_FOUND - Region not found.

**/
EFI_STATUS
GetMemoryRegionRange(
  struct SystemMemoryMapHob  *systemMemoryMap,
  UINT8                      memtype,
  EFI_PHYSICAL_ADDRESS       *StartAddress,
  EFI_PHYSICAL_ADDRESS       *RangeSize)
{
  UINT8         Index;
  EFI_STATUS    Status = EFI_NOT_FOUND;
  UINT8         socket = 0;
  EFI_PHYSICAL_ADDRESS Limit = 0;
  // APTIOV_SERVER_OVERRIDE_RC_START : To resolve Optimization Error
  UINTN MulVal = CONVERT_64MB_TO_BYTE; 
  // APTIOV_SERVER_OVERRIDE_RC_END : To resolve Optimization Error

  *RangeSize = 0;
  *StartAddress = 0;

  for(socket = 0; socket < MAX_SOCKET; socket++) {
    if (systemMemoryMap->Socket[socket].SAD[0].Enable == 0) {
      continue;
    }

    for (Index = 0; Index < SAD_RULES; Index++) {
      if (systemMemoryMap->Socket[socket].SAD[Index].Enable == 0) {
        continue;
      }
      if (systemMemoryMap->Socket[socket].SAD[Index].type == memtype) {
        if ((*StartAddress == 0) && (Index > 0)) {
          //Store the start address for the specified range in bytes
          // APTIOV_SERVER_OVERRIDE_RC_START : To resolve Optimization Error
          *StartAddress = (EFI_PHYSICAL_ADDRESS) (((UINT64)systemMemoryMap->Socket[socket].SAD[Index-1].Limit) * MulVal); 
          // APTIOV_SERVER_OVERRIDE_RC_END : To resolve Optimization Error
        }
        // APTIOV_SERVER_OVERRIDE_RC_START : To resolve Optimization Error
        if ((((UINT64)systemMemoryMap->Socket[socket].SAD[Index].Limit) * MulVal) > Limit) { 
          //Store/Update the end address for the specified range in bytes if greater than previous limit
          Limit = (EFI_PHYSICAL_ADDRESS)(((UINT64)systemMemoryMap->Socket[socket].SAD[Index].Limit) * MulVal);
          // APTIOV_SERVER_OVERRIDE_RC_END : To resolve Optimization Error
        }
      }
    }
  }

  if (Limit != 0) {
    *RangeSize = Limit - *StartAddress;
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**

  Funtion to set MSR to enable MCA on cacheable MMIO access. It
  is executed on all sockets.

  @param MemoryLength -  Memory length of entire memory region

  @retval EFI_SUCCESS -  Writes BIT0 of McaOnCacheableMmio

**/
EFI_STATUS
EnableMcaOnCacheableMmio(
    VOID
)
{
  AsmWriteMsr64 (VIRTUAL_MSR_MCA_ON_NON_NEW_CACHABLE_MMIO_EN_ADDR, MCAONNONNEMCACHEABLEMMIO_BIT);
  return EFI_SUCCESS;
}

/**

  Function to set all MTRRs on the current thread to the MTRR set passed in.

  @param MtrrSettings -  Pointer to MTRR Settings to use

  @retval None

**/
VOID
SetAllMtrrs(
    VOID    *MtrrSettings
)
{
  MtrrSetAllMtrrs((MTRR_SETTINGS*)MtrrSettings);
}

/**

  Function that analyzes memory length passed in to see if there is an
  appropriate address to perform a more efficient top down coverage of
  the memory range.  At this address bit, it is more efficient to overshoot
  memory range with WB cache type and cover the gap with UC cache type.

  @param MemoryLength -  Memory length of entire memory region

  @retval Zero            - No efficient top down approaches found.  More efficient
                            to cover memory range conventional way (bottom up).
          PwrOfTwo Value  - PowerOfTwo bit where the top down approach is most efficient

**/
UINT64
FindTopDownPowerOfTwo (
  IN UINT64               MemoryLength
  )
{
  UINT8   Index;
  BOOLEAN FoundOne = FALSE;
  UINT8   ZeroCount = 0;
  UINT8   OnesCount = 0;
  UINT8   ConsecutiveOnes = 0;
  UINT64  TopDownBit = 0;
  UINT8   MtrrSavings = 0;

  for(Index = 0; Index < 64; Index++){
    if(MemoryLength & LShiftU64(1, Index)) {
      ConsecutiveOnes++;
      OnesCount++;
      FoundOne = TRUE;
    } else {
      if(ConsecutiveOnes > 2) {
        //
        // Found candidate for more efficient top down approach.
        // If there are more zeros + 2 than ones between first 1 found
        // and top bit, bottom up approach is more efficient.
        // Continue to loop through memory length to look for more efficiencies
        // and compare them against previous sets found to pick best set.
        //
        if(((ZeroCount + 2) < OnesCount) && ((OnesCount - (ZeroCount + 2)) > MtrrSavings)) {
          TopDownBit = LShiftU64(1, (Index - 1));
          MtrrSavings = OnesCount - (ZeroCount + 2);
        }
      }
      if(FoundOne) {
        ZeroCount++;
      }
      ConsecutiveOnes = 0;
    }
  }

  //
  // MtrrLib can handle this case efficiently
  //
  if (TopDownBit == GetPowerOfTwo64(MemoryLength)) {
    TopDownBit = 0;
  }

  return TopDownBit;
}

/**

  End of PEI Callback to set up the MTRRs for system boot.

  @param PeiServices       -  General purpose services available to every PEIM.
  @param NotifyDescriptor  -  Not used.
  @param Ppi               -  Not used.

  @retval EFI_SUCCESS   - MTRRs successfully programmed
          EFI_NOT_FOUND - Required HOB not found

**/
EFI_STATUS
MemCacheEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS            Status;
  UINT64                LowUncableBase;
  UINT64                MemoryBase;
  UINT64                TempQword;
  UINT64                MemoryLength;
  UINT64                TopDownBit;
  EFI_PEI_HOB_POINTERS  Hob;
  EFI_PLATFORM_INFO     *PlatformInfo;
  EFI_HOB_GUID_TYPE     *GuidHob;
  BOOLEAN               MTRRDefTypeUncachable;
  UINT8                 MTRRDefTypeUncachableConfiguration;
  struct SystemMemoryMapHob   *systemMemoryMap;
  EFI_PHYSICAL_ADDRESS  StartAddress;
  EFI_PHYSICAL_ADDRESS  RangeSize;
  IIO_UDS               *IioUds;
  MTRR_SETTINGS         MtrrSettings;
  EFI_CPUID_REGISTER    Register;
  EFI_GUID              UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  EFI_PEI_MP_SERVICES_PPI  *mPeiMpServices = NULL;

  //
  // Get required HOBs to be used to generate MTRR programming
  //
  GuidHob       = GetFirstGuidHob (&UniversalDataGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  IioUds  = GET_GUID_HOB_DATA (GuidHob);

  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);

  //
  // Calculate the low uncacheable base address
  //
  if (IioUds->PlatformData.PciExpressBase < IioUds->PlatformData.PlatGlobalMmiolBase) {
    LowUncableBase = IioUds->PlatformData.PciExpressBase;
  } else {
    LowUncableBase = IioUds->PlatformData.PlatGlobalMmiolBase;
  }

  LowUncableBase &= (0x0FFF00000);

  MTRRDefTypeUncachable = TRUE;
  //
  // Get user prefered default cache type
  //
  Status = GetOptionData (
             &gEfiSocketProcessorCoreVarGuid,
             OFFSET_OF(SOCKET_PROCESSORCORE_CONFIGURATION, MTRRDefTypeUncachable),
             &MTRRDefTypeUncachableConfiguration,
             sizeof(UINT8)
             );

  if (!EFI_ERROR (Status)) {
      MTRRDefTypeUncachable = MTRRDefTypeUncachableConfiguration;
  }

  //
  // Reset all Mtrrs to 0 including fixed MTRR and variable MTRR
  //
  ZeroMem(&MtrrSettings, sizeof(MTRR_SETTINGS));
  MtrrSetAllMtrrs(&MtrrSettings);

  //
  // Set fixed cache for memory range below 1MB
  //
  Status = MtrrSetMemoryAttribute(
            0,
            0xA0000,
            EFI_CACHE_WRITEBACK
            );

  Status = MtrrSetMemoryAttribute(
            0xA0000,
            0x60000,
            EFI_CACHE_UNCACHEABLE
            );

  if (MTRRDefTypeUncachable) {

    //
    // Base set to 1mb due to MtrrLib programming method
    //
    MemoryBase = BASE_1MB;
    MemoryLength = LowUncableBase;

    Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
    while (!END_OF_HOB_LIST (Hob)) {
      if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
        if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) ||
            (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_MAPPED_IO &&
             (Hob.ResourceDescriptor->ResourceAttribute & EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE))) {
          if (Hob.ResourceDescriptor->PhysicalStart > EFI_MAX_ADDRESS) {
            TempQword = Hob.ResourceDescriptor->PhysicalStart + Hob.ResourceDescriptor->ResourceLength;
            if (TempQword > MemoryLength) {
              MemoryLength = TempQword;
            }
          }
        }
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }

    TopDownBit = FindTopDownPowerOfTwo(MemoryLength);

    DEBUG((EFI_D_INFO, "Total Memory size: 0x%lx\n", MemoryLength));

    //
    // If TopDownBit has a value, then we found a more efficient address length
    // to use a top down approach.  We will walk through the full address length to
    // program MTRRs individually.  BASE_1MB fixups are due to MtrrLib program method.
    //
    if(TopDownBit){
      DEBUG((EFI_D_INFO, "Efficient Top Down Power of Two = %lx\n\n", TopDownBit));
      while (MemoryLength != 0) {
        if (GetPowerOfTwo64(MemoryLength) == TopDownBit) {
          //
          // Overshoot address with WB and cover remaining gap with UC
          //
          TempQword = MemoryLength;
          MemoryLength = LShiftU64(GetPowerOfTwo64 (MemoryLength), 1);
          if(MemoryBase == BASE_1MB) {
            MemoryLength -= BASE_1MB;
          }
          Status = MtrrSetMemoryAttribute(
              MemoryBase,
              MemoryLength,
              EFI_CACHE_WRITEBACK
              );
          ASSERT_EFI_ERROR(Status);
          if(MemoryBase == BASE_1MB) {
            MemoryBase = 0;
            MemoryLength += BASE_1MB;
          }
          MemoryBase += TempQword;
          MemoryLength -= TempQword;

          //
          // Program UC region gap between top of memory and WB MTRR
          //
          Status = MtrrSetMemoryAttribute(
              MemoryBase,
              MemoryLength,
              EFI_CACHE_UNCACHEABLE
              );
          ASSERT_EFI_ERROR(Status);
          MemoryLength = 0;
        } else {
          //
          // Grow next power of two upwards and adjust base and length
          //
          TempQword = GetPowerOfTwo64(MemoryLength);
          MemoryLength -= TempQword;
          if(MemoryBase == BASE_1MB) {
            TempQword -= BASE_1MB;
          }
          Status = MtrrSetMemoryAttribute(
              MemoryBase,
              TempQword,
              EFI_CACHE_WRITEBACK
              );
          ASSERT_EFI_ERROR(Status);
          MemoryBase += TempQword;
        }
      }
    } else {
      //
      // Create a WB region for the entire memory region
      //
      Status = MtrrSetMemoryAttribute(
        MemoryBase,
        MemoryLength - BASE_1MB,
        EFI_CACHE_WRITEBACK
        );
      ASSERT_EFI_ERROR(Status);
    }

    //
    // Punch UC hole for lower MMIO region
    //
    Status = MtrrSetMemoryAttribute(
        LowUncableBase,
        EFI_MAX_ADDRESS - LowUncableBase + 1,
        EFI_CACHE_UNCACHEABLE
        );
    ASSERT_EFI_ERROR(Status);

#if ME_SUPPORT_FLAG
    //
    // Set MESEG memory range to be un-cachable if MESEG enabled
    //
    if( PlatformInfo->MemData.MemMESEGSize != 0 ) {
        Status = MtrrSetMemoryAttribute(
              (EFI_PHYSICAL_ADDRESS)PlatformInfo->MemData.MemMESEGBase,
              (UINT64)PlatformInfo->MemData.MemMESEGSize,
              EFI_CACHE_UNCACHEABLE
              );
        ASSERT_EFI_ERROR (Status);    // assert if MESEG enabled and no MTRR available to set it to UC.
    }
#endif //ME_SUPPORT_FLAG

  } else {
    //
    // If WB is default type, enable default MTRR type to be cachable
    // before setting variable MTRRs
    //
    TempQword = AsmReadMsr32 (EFI_CACHE_IA32_MTRR_DEF_TYPE);
    TempQword |= EFI_CACHE_WRITEBACK;
    AsmWriteMsr64 (EFI_CACHE_IA32_MTRR_DEF_TYPE, TempQword);

    //
    // Set the lower MMIO region as UC
    //
    Status = MtrrSetMemoryAttribute(
        LowUncableBase,
        EFI_MAX_ADDRESS - LowUncableBase + 1,
        EFI_CACHE_UNCACHEABLE
        );
    ASSERT_EFI_ERROR(Status);
    //
    // Set the 64-bit PCI/HEC resource as UC
    //
    if ((IioUds->PlatformData.PlatGlobalMmiohLimit + 1) > IioUds->PlatformData.PlatGlobalMmiohBase) {
      Status = MtrrSetMemoryAttribute(
          IioUds->PlatformData.PlatGlobalMmiohBase,
          (IioUds->PlatformData.PlatGlobalMmiohLimit + 1) - IioUds->PlatformData.PlatGlobalMmiohBase,
          EFI_CACHE_UNCACHEABLE
          );
      ASSERT_EFI_ERROR (Status);
    }

    GuidHob       = GetFirstGuidHob (&gEfiMemoryMapGuid);
    ASSERT(GuidHob != NULL);
    if (GuidHob == NULL) {
      return EFI_NOT_FOUND;
    }
    systemMemoryMap  = (struct SystemMemoryMapHob *)GET_GUID_HOB_DATA(GuidHob);

    //
    // Find the memory range that is mapped to DDRT control regions and add a descriptor for that range as reserved.
    //
    Status = GetMemoryRegionRange(systemMemoryMap, MEM_TYPE_CTRL, &StartAddress, &RangeSize);
    if (!EFI_ERROR (Status)) {
      Status = MtrrSetMemoryAttribute(
                StartAddress,
                RangeSize,
                EFI_CACHE_UNCACHEABLE
                );
      ASSERT_EFI_ERROR (Status);
    }

#if ME_SUPPORT_FLAG
    //
    // Set MESEG memory range to be un-cachable if MESEG enabled
    //
    if( PlatformInfo->MemData.MemMESEGSize != 0 ) {
        Status = MtrrSetMemoryAttribute(
              (EFI_PHYSICAL_ADDRESS)PlatformInfo->MemData.MemMESEGBase,
              (UINT64)PlatformInfo->MemData.MemMESEGSize,
              EFI_CACHE_UNCACHEABLE
              );
        ASSERT_EFI_ERROR (Status);    // assert if MESEG enabled and no MTRR available to set it to UC.
    }
#endif //ME_SUPPORT_FLAG

  }

  //
  // Gather MTRR settings from BSP
  //
  MtrrGetAllMtrrs(&MtrrSettings);

  Status = (*PeiServices)->LocatePpi(
                    PeiServices,
                    &gPeiMpServicePpiGuid,
                    0,
                    NULL,
                    &mPeiMpServices
                    );

  if(!EFI_ERROR(Status)){
    //
    // Sync all AP MTRRs with BSP
    //
    Status = mPeiMpServices->StartupAllAPs (
                  PeiServices,
                  mPeiMpServices,
                  (EFI_AP_PROCEDURE)SetAllMtrrs,
                  FALSE,
                  0,
                  (VOID*)&MtrrSettings
                  );
  }

  //
  // Check Skx
  // Enable mcaonnonnemcacheablemmio on all sockets.  This MSR cannot be read
  //
  if(!IsSimicsPlatform()) {
    AsmCpuid (EFI_CPUID_VERSION_INFO, &Register.RegEax, &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);
    if ((Register.RegEax >> 4) == CPU_FAMILY_SKX) {
      EnableMcaOnCacheableMmio();
      if(mPeiMpServices != NULL){
        Status = mPeiMpServices->StartupAllAPs (
                  PeiServices,
                  mPeiMpServices,
                  (EFI_AP_PROCEDURE)EnableMcaOnCacheableMmio,
                  FALSE,
                  0,
                  NULL
                  );
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  The Entry point of the Memory Cache PEIM.

  This function set up a callback for end of PEI to program MTRRs for
  the remainder of the boot process.

  @param  FileHandle    Handle of the file being invoked.
  @param  PeiServices   Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   Notify PPI is installed successfully.

**/
EFI_STATUS
EFIAPI
MemCacheInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;
  //
  // Install notify PPIs
  //
  Status = PeiServicesNotifyPpi (&mNotifyList);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
