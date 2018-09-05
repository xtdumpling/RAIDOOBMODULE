/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemoryCallback.c

  EFI 2.0 PEIM to provide the platform support functionality on the Bridgeport.

**/
// APTIOV_SERVER_OVERRIDE_RC_START
#define PEI_PROGRESS_CODE(PeiServices, Code)\
    (*PeiServices)->ReportStatusCode(PeiServices, EFI_PROGRESS_CODE, Code, 0, NULL, NULL)
// APTIOV_SERVER_OVERRIDE_RC_END

#include "PlatformEarlyInit.h"
#include <Guid/FirmwareFileSystem3.h>
#include <Guid/SetupVariable.h>
#include <Guid/MemoryMapData.h>
#include <Ppi/AtaPolicy.h>
#include <Ppi/Cache.h>

#include <Cpu/CpuRegs.h>
#include <FlashMap.h>

static EFI_PEI_FIRMWARE_VOLUME_INFO_PPI mFvMainInfoPpi = {
  EFI_FIRMWARE_FILE_SYSTEM3_GUID,
  (VOID*)FLASH_REGION_FVMAIN_BASE,
  FLASH_REGION_FVMAIN_SIZE,
  NULL,
  NULL
};

static EFI_PEI_PPI_DESCRIPTOR     mPpiList1 = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiFirmwareVolumeInfoPpiGuid,
  &mFvMainInfoPpi
};


/**

  Turn off system if needed.

  @param PeiServices Pointer to PEI Services
  @param CpuIo       Pointer to CPU I/O Protocol

  @retval None.

**/
VOID
CheckPowerOffNow (
  VOID
  )
{

  UINT16  Pm1Sts;

  //
  // Read and check the ACPI registers
  //
  Pm1Sts = IoRead16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS);
  DEBUG ((EFI_D_ERROR, "CheckPowerOffNow()- Pm1Sts= 0x%04x\n", Pm1Sts ));

// HSD: 5330644 enabling power button check in LBG systems for enabling power on requirement.
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_PWRBTN) == B_PCH_ACPI_PM1_STS_PWRBTN) {
    IoWrite16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_PWRBTN);
    IoWrite16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, V_PCH_ACPI_PM1_CNT_S5);
    IoWrite16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, V_PCH_ACPI_PM1_CNT_S5 + B_PCH_ACPI_PM1_CNT_SLP_EN);
  }
}

/**

  PEI termination callback.

  @param PeiServices       -  General purpose services available to every PEIM.
  @param NotifyDescriptor  -  Not uesed.
  @param Ppi               -  Not uesed.

  @retval EFI_SUCCESS  -  If the interface could be successfully
                  @retval installed.

**/
EFI_STATUS
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  IIO_UDS               *IioUds;

  GetIioUdsHob(PeiServices, &IioUds);

  if (IioUds != NULL ) {

    DEBUG ((EFI_D_ERROR, "Memory TOLM: %X\n", IioUds->PlatformData.MemTolm));
    DEBUG (
      (EFI_D_ERROR,
      "PCIE   BASE: %lX     Size : %X\n",
      IioUds->PlatformData.PciExpressBase,
      IioUds->PlatformData.PciExpressSize)
      );
    DEBUG (
      (EFI_D_ERROR,
      "PCI32  BASE: %X     Limit: %X\n",
      IioUds->PlatformData.PlatGlobalMmiolBase,
      IioUds->PlatformData.PlatGlobalMmiolLimit)
      );
    DEBUG (
      (EFI_D_ERROR,
      "PCI64  BASE: %lX     Limit: %lX\n",
      IioUds->PlatformData.PlatGlobalMmiohBase,
      IioUds->PlatformData.PlatGlobalMmiohLimit)
      );
    DEBUG ((EFI_D_ERROR, "UC    START: %lX     End  : %lX\n", IioUds->PlatformData.PlatGlobalMmiohBase, (IioUds->PlatformData.PlatGlobalMmiohLimit + 1)));
  } else {
    DEBUG ((EFI_D_ERROR, "Error: IioUds = NULL\n"));
  }

  return EFI_SUCCESS;
}
/**

    GC_TODO: add routine description

    @param PeiServices  - GC_TODO: add arg description
    @param VolumeHandle - GC_TODO: add arg description

    @retval None

**/
VOID
EFIAPI
InstallFvInfoPpi (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_FV_HANDLE           VolumeHandle
  )
{
  EFI_STATUS                     Status;
  EFI_PEI_PPI_DESCRIPTOR         *mPpiList;
  PEI_FV_INFO_PPI_PRIVATE        *FvInfoInstance;

  //
  // Prepare to install FirmwareVolumeInfo PPI to expose new FV to PeiCore.
  //
  FvInfoInstance =  AllocatePool (sizeof (PEI_FV_INFO_PPI_PRIVATE));

  if (FvInfoInstance != NULL) {


    mPpiList = &FvInfoInstance->PpiList;

    CopyMem (
      &FvInfoInstance->FvInfoPpi.FvFormat,
      &(((EFI_FIRMWARE_VOLUME_HEADER*)VolumeHandle)->FileSystemGuid),
      sizeof (EFI_GUID)
     );

    FvInfoInstance->FvInfoPpi.FvInfo = VolumeHandle;
    FvInfoInstance->FvInfoPpi.FvInfoSize = (UINT32)((EFI_FIRMWARE_VOLUME_HEADER*)VolumeHandle)->FvLength;
    FvInfoInstance->FvInfoPpi.ParentFvName = NULL;
    FvInfoInstance->FvInfoPpi.ParentFileName = NULL;

    //
    // Install FirmwareVolumeInfo PPI to export new Firmware Volume to Core.
    //
    mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    mPpiList->Guid  = &gEfiPeiFirmwareVolumeInfoPpiGuid;
    mPpiList->Ppi   = &FvInfoInstance->FvInfoPpi;
    Status          = (**PeiServices).InstallPpi (PeiServices, mPpiList);
    ASSERT_EFI_ERROR (Status);
  } else {
    DEBUG ((EFI_D_ERROR, "Error: FvInfoInstance = NULL\n")); 
    ASSERT(FvInfoInstance); 
  }

}

/**

  Install Firmware Volume Hob's once there is main memory

  @param PeiServices       General purpose services available to every PEIM.
  @param NotifyDescriptor  Notify that this module published.
  @param Ppi               PPI that was installed.

  @retval EFI_SUCCESS     The function completed successfully.

**/
EFI_STATUS
EFIAPI
SiliconRcHobsReadyPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                  Status;
  EFI_BOOT_MODE               BootMode;
  PEI_CACHE_PPI               *CachePpi;
  UINT64                      FlashBase;
  UINT64                      FlashSize;
  UINT64                      LowUncableBase;
  EFI_CPUID_REGISTER          FeatureInfo;
  UINT8                       CpuAddressWidth;
  UINT16                      Pm1Cnt;
  IIO_UDS                     *IioUds;
  EFI_PLATFORM_INFO           *PlatformInfo;
  EFI_HOB_GUID_TYPE           *GuidHob;

  GetIioUdsHob(PeiServices, &IioUds);

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);

  //
  // Check if user wants to turn off in PEI phase
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    CheckPowerOffNow ();
  } else {
    Pm1Cnt  = IoRead16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT);
    Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SLP_TYP;
    IoWrite16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
  }

  //
  // Set initial MTRRs for under 4GB
  //
  Status = PeiServicesLocatePpi (
            &gPeiCachePpiGuid,  // GUID
            0,                  // Instance
            NULL,               // PEI_PPI_DESCRIPTOR
            &CachePpi           // PPI
            );

  if (!EFI_ERROR (Status)) {
    //
    // Clear the CAR Settings
    //
    CachePpi->ResetCache (
                PeiServices,
                CachePpi
                );

    //
    // Set fixed cache for memory range below 1MB
    //
    CachePpi->SetCache (
                PeiServices,
                CachePpi,
                0,
                0xA0000,
                EFI_CACHE_WRITEBACK
                );

    CachePpi->SetCache (
                PeiServices,
                CachePpi,
                0xA0000,
                0x60000,
                EFI_CACHE_UNCACHEABLE
                );
    if (IioUds != NULL) {

      if (IioUds->PlatformData.PciExpressBase < IioUds->PlatformData.PlatGlobalMmiolBase) {
        LowUncableBase = IioUds->PlatformData.PciExpressBase;
      } else {
        LowUncableBase = IioUds->PlatformData.PlatGlobalMmiolBase;
      }
      LowUncableBase &= (0x0FFF00000);

      //
      // Cache memory under 4GB to WB
      //
      CachePpi->SetCache (
                  PeiServices,
                  CachePpi,
                  BASE_1MB,
                  LowUncableBase - BASE_1MB,
                  EFI_CACHE_WRITEBACK
                  );
    } else {
      DEBUG((EFI_D_ERROR, "Error: IioUds = NULL\n")); 
      ASSERT(IioUds); 
    }

#if ME_SUPPORT_FLAG
    //
    // Set MESEG memory range to be un-cachable if MESEG enabled
    //
    if( PlatformInfo->MemData.MemMESEGSize != 0 ) {
        CachePpi->SetCache (
                    PeiServices,
                    CachePpi,
                    (EFI_PHYSICAL_ADDRESS)PlatformInfo->MemData.MemMESEGBase,
                    (UINT64)PlatformInfo->MemData.MemMESEGSize,
                    EFI_CACHE_UNCACHEABLE
                    );
        ASSERT_EFI_ERROR (Status);    // assert if MESEG enabled and no MTRR available to set it to UC.
    }
#endif //ME_SUPPORT_FLAG

    //
    // Set flash region to WP until end of PEI
    //
    // APTIOV_SERVER_OVERRIDE_RC_START : Use FLASH SDL tokens
    #if 0
    FlashBase = FixedPcdGet32 (PcdFlashFdMainBase) + FixedPcdGet32 (PcdFlashFvMainSize);
    FlashSize = (UINT64)EFI_MAX_ADDRESS - FlashBase + 1;
    #else
    FlashBase = BIOS_AREA_BASE;
    FlashSize = BIOS_AREA_SIZE;
    #endif
    // APTIOV_SERVER_OVERRIDE_RC_END : Use FLASH SDL tokens
    CachePpi->SetCache (
        PeiServices,
        CachePpi,
        FlashBase,
        FlashSize,
        EFI_CACHE_WRITEPROTECTED
      );
  }

  //
  // Create the firmware volume HOB's
  //
  if (BootMode != BOOT_IN_RECOVERY_MODE) {
    DEBUG((EFI_D_ERROR, "=======================\n"));
    DEBUG((EFI_D_ERROR, " Boot in Normal mode\n"));
    DEBUG((EFI_D_ERROR, "=======================\n"));
// APTIOV_SERVER_OVERRIDE_RC_START : Do not build flash HOB's here. Flash region will be reserved through RSVChipset. 
#if 0
    BuildFvHob (
      FLASH_REGION_FV_SECPEI_BASE,
      FLASH_REGION_FV_SECPEI_SIZE
      );


    BuildFvHob (
      FLASH_REGION_FVMAIN_BASE,
      FLASH_REGION_FVMAIN_SIZE
      );

    //
    // Install EFI_PEI_FIRMWARE_VOLUME_INFO_PPI to add this Fv into FvList.
    //
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList1);
    ASSERT_EFI_ERROR (Status);

    BuildFvHob (
      FLASH_REGION_MICROCODE_FV_BASE,
      FLASH_REGION_MICROCODE_FV_SIZE
      );

    BuildFvHob (
      FLASH_REGION_OEM_FV_BASE,
      FLASH_REGION_OEM_FV_SIZE
      );
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Do not build flash HOB's here. Flash region will be reserved through RSVChipset.     
  } else {
    DEBUG((EFI_D_ERROR, "=======================\n"));
    DEBUG((EFI_D_ERROR, " Boot in Recovery mode\n"));
    DEBUG((EFI_D_ERROR, "=======================\n"));

    Status = PlatformPchUsbInit (PeiServices);
    ASSERT_EFI_ERROR(Status);

    //
    // Prepare the recovery service
    //
    Status = PeimInitializeRecovery (PeiServices);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Do not need to build HOB for NV Storage FV
  //
  /*BuildFvHob (
    FLASH_REGION_RUNTIME_UPDATABLE_BASE,
    (FLASH_REGION_RUNTIME_UPDATABLE_SIZE + FLASH_REGION_NV_FTW_SPARE_SIZE)
    );*/
  // APTIOV_SERVER_OVERRIDE_RC_START : Do not build flash HOB's here. Flash region will be reserved through RSVChipset.   
#if 0
  BuildResourceDescriptorHob (
    EFI_RESOURCE_FIRMWARE_DEVICE,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    FLASH_BASE,
    FLASH_SIZE
    );
#endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Do not build flash HOB's here. Flash region will be reserved through RSVChipset. 
  //
  // Create a CPU hand-off information
  //
  CpuAddressWidth = 32;
  AsmCpuid (EFI_CPUID_EXTENDED_FUNCTION, &FeatureInfo.RegEax, NULL, NULL, NULL);
  if (FeatureInfo.RegEax >= EFI_CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (EFI_CPUID_VIR_PHY_ADDRESS_SIZE, &FeatureInfo.RegEax, NULL, NULL, NULL);
    CpuAddressWidth = (UINT8) (FeatureInfo.RegEax & 0xFF);
  }

  DEBUG ((EFI_D_ERROR, "CpuAddressWidth: %d\n", CpuAddressWidth));

  BuildCpuHob (CpuAddressWidth, 16);

  ASSERT_EFI_ERROR (Status);

  return Status;

}

/**

  Check if the files in the Backup Blocks are valid.

  @param PeiServices  -  General purpose services available to every PEIM.
  @param Address      -  Address of the backup block.

  @retval TRUE   -  If all the files are valid.
  @retval FALSE  -  If one the files are invalid.

**/
BOOLEAN
CheckIfFvIsValid (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN UINTN                  Address
  )
{
  return TRUE;
}

/**

    GC_TODO: add routine description

    @param FwVolHeader - GC_TODO: add arg description

    @retval EFI_NOT_FOUND - GC_TODO: add retval description
    @retval EFI_SUCCESS   - GC_TODO: add retval description

**/
EFI_STATUS
ValidateFvHeader (
  IN EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader
  )
{
  UINT16  *Ptr;
  UINT16  HeaderLength;
  UINT16  Checksum;

  //
  // Verify the header revision, header signature, length
  // Length of FvBlock cannot be 2**64-1
  // HeaderLength cannot be an odd number
  //
  if ((FwVolHeader->Revision != EFI_FVH_REVISION) ||
      (FwVolHeader->Signature != EFI_FVH_SIGNATURE) ||
      (FwVolHeader->FvLength == ((UINT64) -1)) ||
      ((FwVolHeader->HeaderLength & 0x01) != 0)
      ) {
    return EFI_NOT_FOUND;
  }
  //
  // Verify the header checksum
  //
  HeaderLength  = (UINT16) (FwVolHeader->HeaderLength / 2);
  Ptr           = (UINT16 *) FwVolHeader;
  Checksum      = 0;
  while (HeaderLength > 0) {
    Checksum = *Ptr++;
    HeaderLength--;
  }

  if (Checksum != 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

