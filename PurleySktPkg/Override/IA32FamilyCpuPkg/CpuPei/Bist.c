/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
Implementation of CPU driver for PEI phase.

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

Module Name: Bist.c

**/

#include "CpuPei.h"
#include "Bist.h"

EFI_SEC_PLATFORM_INFORMATION_PPI mSecPlatformInformationPpi = {
  SecPlatformInformation
};

EFI_PEI_PPI_DESCRIPTOR mPeiSecPlatformInformationPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiSecPlatformInformationPpiGuid,
  &mSecPlatformInformationPpi
};

EFI_SEC_PLATFORM_INFORMATION2_PPI mSecPlatformInformation2Ppi = {
  SecPlatformInformation2
};

EFI_PEI_PPI_DESCRIPTOR mPeiSecPlatformInformation2Ppi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiSecPlatformInformation2PpiGuid,
  &mSecPlatformInformation2Ppi
};

/**
  Worker function to parse CPU BIST information from Guided HOB.

  @param  StructureSize         Pointer to the variable describing size of the input buffer.
  @param  StructureBuffer       Pointer to the buffer save CPU BIST information.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.

**/
EFI_STATUS
GetBistFromHob (
  IN OUT UINT64           *StructureSize,
  IN OUT VOID             *StructureBuffer
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  VOID                    *DataInHob;
  UINTN                   DataSize;

  GuidHob = GetFirstGuidHob (&gEfiCallerIdGuid);
  if (GuidHob == NULL) {
    *StructureSize = 0;
    return EFI_SUCCESS;
  }

  DataInHob = GET_GUID_HOB_DATA (GuidHob);
  DataSize  = GET_GUID_HOB_DATA_SIZE (GuidHob);

  //
  // return the information from BistHob
  //
  if ((*StructureSize) < (UINT64) DataSize) {
    *StructureSize = (UINT64) DataSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StructureSize = (UINT64) DataSize;
  CopyMem (StructureBuffer, DataInHob, DataSize);
  return EFI_SUCCESS;
}

/**
  Implementation of the PlatformInformation service in EFI_SEC_PLATFORM_INFORMATION_PPI.

  @param  PeiServices                Pointer to the PEI Services Table.
  @param  StructureSize              Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord  Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

  @retval EFI_SUCCESS                The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL       The buffer was too small.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                  **PeiServices,
  IN OUT UINT64                              *StructureSize,
     OUT EFI_SEC_PLATFORM_INFORMATION_RECORD *PlatformInformationRecord
  )
{
  return GetBistFromHob (StructureSize, PlatformInformationRecord);
}

/**
  Implementation of the PlatformInformation2 service in EFI_SEC_PLATFORM_INFORMATION2_PPI.

  @param  PeiServices                The pointer to the PEI Services Table.
  @param  StructureSize              The pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord2 The pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD2.

  @retval EFI_SUCCESS                The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL       The buffer was too small. The current buffer size needed to
                                     hold the record is returned in StructureSize.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation2 (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN OUT UINT64                               *StructureSize,
     OUT EFI_SEC_PLATFORM_INFORMATION_RECORD2 *PlatformInformationRecord2
  )
{
  return GetBistFromHob (StructureSize, PlatformInformationRecord2);
}

/**
  Worker function to get CPUs' BIST by calling SecPlatformInformationPpi
  or SecPlatformInformation2Ppi.

  @param  PeiServices         Pointer to PEI Services Table
  @param  Guid                PPI Guid
  @param  PpiDescriptor       Return a pointer to instance of the
                              EFI_PEI_PPI_DESCRIPTOR  
  @param  BistInformationSize Return the size in bytes of BIST information
  @param  BistInformationData Pointer to BIST information data

  @retval EFI_SUCCESS         Retrieve of the BIST data successfully
  @retval EFI_NOT_FOUND       No sec platform information(2) ppi export
  @retval EFI_DEVICE_ERROR    Failed to get CPU Information

**/
EFI_STATUS
GetBistInfoFromPpi (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN CONST EFI_GUID             *Guid,
     OUT EFI_PEI_PPI_DESCRIPTOR **PpiDescriptor,
  IN OUT UINT64                 *BistInformationSize,
     OUT VOID                   **BistInformationData
  )
{
  EFI_STATUS                            Status;
  EFI_SEC_PLATFORM_INFORMATION2_PPI     *SecPlatformInformation2Ppi;
  EFI_SEC_PLATFORM_INFORMATION_RECORD2  *SecPlatformInformation2;
  UINT64                                InformationSize;

  Status = PeiServicesLocatePpi (
             Guid,                                // GUID
             0,                                   // INSTANCE
             PpiDescriptor,                       // EFI_PEI_PPI_DESCRIPTOR
             (VOID **)&SecPlatformInformation2Ppi // PPI
             );
  if (Status == EFI_NOT_FOUND) {
    return EFI_NOT_FOUND;
  }

  if (Status == EFI_SUCCESS) {
    //
    // Get the size of the sec platform information2(BSP/APs' BIST data)
    //
    InformationSize         = 0;
    SecPlatformInformation2 = NULL;
    Status = SecPlatformInformation2Ppi->PlatformInformation2 (
                                           PeiServices,
                                           &InformationSize,
                                           SecPlatformInformation2
                                           );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Status = PeiServicesAllocatePool (
                 (UINTN) InformationSize,
                 (VOID **) &SecPlatformInformation2
                 );
      if (Status == EFI_SUCCESS) {
        //
        // Retrieve BIST data
        //
        Status = SecPlatformInformation2Ppi->PlatformInformation2 (
                                               PeiServices,
                                               &InformationSize,
                                               SecPlatformInformation2
                                               );
        if (Status == EFI_SUCCESS) {
          *BistInformationSize = InformationSize;
          *BistInformationData = SecPlatformInformation2;
          return EFI_SUCCESS;
        }
      }
    }
  }

  return EFI_DEVICE_ERROR;
}

/**
  Get CPUs' BIST by calling SecPlatformInformationPpi/SecPlatformInformation2Ppi.

  @param  PeiServices      Pointer to PEI Services Table

  @retval EFI_SUCCESS      Retrieve of the BIST data successfully
  @retval EFI_SUCCESS      No sec platform information(2) ppi export
  @retval others           Failed to get CPU Information.

**/
EFI_STATUS
RetrieveCpuInfo (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  UINT64                                BistInformationSize;
  VOID                                  *BistInformationData;
  EFI_PEI_PPI_DESCRIPTOR                *SecInformationDescriptor;

  Status = GetBistInfoFromPpi (
             PeiServices,
             &gEfiSecPlatformInformation2PpiGuid,
             &SecInformationDescriptor,
             &BistInformationSize,
             &BistInformationData
             );
  if (Status == EFI_SUCCESS) {
    BuildGuidDataHob (
      &gEfiCallerIdGuid,
      BistInformationData,
      (UINTN) BistInformationSize
      );
    //
    // The old SecPlatformInformation Ppi is on CAR.
    // After memory discovered, we should never get it from CAR, or the data will be crashed.
    // So, we reinstall SecPlatformInformation PPI here.
    //
    Status = PeiServicesReInstallPpi (
               SecInformationDescriptor,
               &mPeiSecPlatformInformation2Ppi
               );
  } if (Status == EFI_NOT_FOUND) {
    //
    // The old SecPlatformInformation2 Ppi is on CAR.
    // After memory discovered, we should never get it from CAR, or the data will be crashed.
    // So, we reinstall SecPlatformInformation2 PPI here.
    //
    Status = GetBistInfoFromPpi (
               PeiServices,
               &gEfiSecPlatformInformationPpiGuid,
               &SecInformationDescriptor,
               &BistInformationSize,
               &BistInformationData
               );
    if (Status == EFI_SUCCESS) {
      BuildGuidDataHob (
        &gEfiCallerIdGuid,
        BistInformationData,
        (UINTN) BistInformationSize
        );
      //
      // The old SecPlatformInformation Ppi is on CAR.
      // After memory discovered, we should never get it from CAR, or the data will be crashed.
      // So, we reinstall SecPlatformInformation PPI here.
      //
      Status = PeiServicesReInstallPpi (
                 SecInformationDescriptor,
                 &mPeiSecPlatformInformationPpi
                 );
    } else if (Status == EFI_NOT_FOUND) {
      return EFI_SUCCESS;
    }
  }

  return Status;
}

/**
  A callback function to build CPUs information Guided HOB.

  This function is a callback function to build one Guided HOB to pass
  CPUs' BIST information to DXE phase.

  @param  PeiServices      Pointer to PEI Services Table
  @param  NotifyDescriptor Address if the notification descriptor data structure
  @param  Ppi              Address of the PPI that was installed

  @retval EFI_SUCCESS      Retrieve of the BIST data successfully
  @retval EFI_SUCCESS      No sec platform information(2) ppi export

**/
EFI_STATUS
EFIAPI
BuildBistAndCpuInfoHob (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *Ppi
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_MODE                         BootMode;
  UINT64                                BistInformationSize;
  VOID                                  *BistInformationData;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  UINT32                                RegEax;
  UINT8                                 PhysicalAddressBits;

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR (Status);
  //
  // We do not build CPU Hobs on S3 path, because the data are only needed by DXE drivers.
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = GetBistInfoFromPpi (
               (CONST EFI_PEI_SERVICES **) PeiServices,
               &gEfiSecPlatformInformation2PpiGuid,
               NULL,
               &BistInformationSize,
               &BistInformationData
               );
    if (Status == EFI_SUCCESS) {
      BuildGuidDataHob (
        &gEfiSecPlatformInformation2PpiGuid,
        BistInformationData,
        (UINTN) BistInformationSize
        );
    } if (Status == EFI_NOT_FOUND) {
      GuidHob = GetFirstGuidHob (&gEfiCallerIdGuid);
      ASSERT (GuidHob != NULL);
      BistInformationData = GET_GUID_HOB_DATA (GuidHob);
      BistInformationSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
      BuildGuidDataHob (
        &gEfiHtBistHobGuid,
        BistInformationData,
        (UINTN) BistInformationSize
        );
      BuildGuidDataHob (
        &gEfiSecPlatformInformationPpiGuid,
        BistInformationData,
        (UINTN) BistInformationSize
        );
    }

    //
    //Local APIC range
    //
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_MAPPED_IO,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
      0xFEC80000,
      SIZE_512KB
      );
    BuildMemoryAllocationHob (
      0xFEC80000,
      SIZE_512KB,
      EfiMemoryMappedIO
      );

    //
    // build CPU memory space and IO space hob
    //
    AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
    if (RegEax >= 0x80000008) {
      AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
      PhysicalAddressBits = (UINT8) RegEax;
    } else {
      //
      //APTIOV_SERVER_OVERRIDE_RC_START : Added Maximum Processor physical address bits
      //
      PhysicalAddressBits  = PcdGet8(PcdMaxProcessorPhysicalAddressBits);
      //
      //APTIOV_SERVER_OVERRIDE_RC_END : Added Maximum Processor physical address bits
      //
    }
    //
    // Create a CPU hand-off information
    //
    BuildCpuHob (PhysicalAddressBits, 16);
  }
  return EFI_SUCCESS;
}
