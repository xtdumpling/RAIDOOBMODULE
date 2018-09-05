/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemoryPeim.c
   
  Tiano PEIM to provide the platform support functionality on the Bridgeport.
  This file implements the Platform Memory Range PPI

**/

#include "PlatformEarlyInit.h"
// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
#include <Token.h>
#include "Guid/AmiGlobalVariable.h"
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.

EFI_MEMORY_TYPE_INFORMATION mDefaultMemoryTypeInformation[] = {
// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
#if AlternativeDefaultMemoryQuota_SUPPORT
#include <AlternativeDefaultMemoryQuota.h>
#else
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.
  {
    EfiACPIReclaimMemory,
    0x100
  },
  {
    EfiACPIMemoryNVS,
    0xA30
  },
  {
    EfiReservedMemoryType,
    0x100
  },
  {
    EfiRuntimeServicesCode,
    0x100
  },
  {
    EfiRuntimeServicesData,
    0x100
  },
  {
    EfiMaxMemoryType,
    0
  }
// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
#endif
};

EFI_STATUS
MemoryPeimEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  MemoryPeimEndOfPeiCallback
};

EFI_STATUS
MemoryPeimEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_HOB_GUID_TYPE             *MemoryInformationHob;

  for(MemoryInformationHob = GetFirstGuidHob(&gEfiMemoryTypeInformationGuid)
        ; MemoryInformationHob != NULL
        ; MemoryInformationHob = GetNextGuidHob(&gEfiMemoryTypeInformationGuid, MemoryInformationHob)
  ) MemoryInformationHob->Header.HobType = EFI_HOB_TYPE_UNUSED;

  DEBUG((EFI_D_ERROR, "MemoryPeimEndOfPeiCallback: Create Default Memory Map for DXE. \n"));

  // Build Memory Information HOB for DXE Core.
  BuildGuidDataHob(
    &gEfiMemoryTypeInformationGuid,
    mDefaultMemoryTypeInformation,
    sizeof (mDefaultMemoryTypeInformation)
  );

  return Status;
}
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.

/**

  Build Memory type information.

  @param PeiServices - Pointer to Pei Services.
  @param This        - GC_TODO: add argument description
  @param MemorySize  - GC_TODO: add argument description

  @retval EFI_SUCCESS  -  Success.
  @retval Others       -  Errors have occurred.
**/
EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN      EFI_PEI_SERVICES                       **PeiServices,
  IN      PEI_PLATFORM_MEMORY_SIZE_PPI           *This,
  IN OUT  UINT64                                 *MemorySize
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  UINTN                           DataSize;
  EFI_MEMORY_TYPE_INFORMATION     MemoryData[EfiMaxMemoryType + 1];
  UINTN                           Index;
  EFI_BOOT_MODE                   BootMode;
  UINTN                           IndexNumber;
  UINTN                           *HobPointer;
// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
  UINT32                          AvailableMemoryBelow4G = (PcdGet32(PcdAvailableMemoryBelow4G));
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.

  *MemorySize = PEI_MIN_MEMORY_SIZE;
  Status = PeiServicesLocatePpi (
           &gEfiPeiReadOnlyVariable2PpiGuid,
            0,
            NULL,
            &Variable
            );

  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  DataSize = sizeof (MemoryData);

  Status = Variable->GetVariable (
                      Variable,
                      EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                      // APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
                      &gAmiGlobalVariableGuid,
                      // APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.
                      NULL,
                      &DataSize,
                      &MemoryData
                      );

  IndexNumber = sizeof (mDefaultMemoryTypeInformation) / sizeof (EFI_MEMORY_TYPE_INFORMATION);

  //
  // Accumulate maximum amount of memory needed
  //
  
  DEBUG((EFI_D_ERROR, "PEI_MIN_MEMORY_SIZE:%dKB \n", DivU64x32(*MemorySize,1024)));
  DEBUG((EFI_D_ERROR, "IndexNumber:%d MemoryDataNumber%d \n", IndexNumber,DataSize/ sizeof (EFI_MEMORY_TYPE_INFORMATION)));
  if (EFI_ERROR (Status)) {
// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
  DefaultMemoryTypeInformation:
    IndexNumber = sizeof (mDefaultMemoryTypeInformation) / sizeof (EFI_MEMORY_TYPE_INFORMATION);
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.
    //
    // Start with minimum memory
    //
    for (Index = 0; Index < IndexNumber; Index++) {
      DEBUG((EFI_D_ERROR, "Index[%d].Type = %d .NumberOfPages=0x%x\n", Index,mDefaultMemoryTypeInformation[Index].Type,mDefaultMemoryTypeInformation[Index].NumberOfPages));
      *MemorySize += mDefaultMemoryTypeInformation[Index].NumberOfPages * EFI_PAGE_SIZE;
    }
    DEBUG((EFI_D_ERROR, "No memory type,  Total platform memory:%dKB \n", DivU64x32(*MemorySize,1024)));
    //
    // Build the GUID'd HOB for DXE
    //
    HobPointer = BuildGuidDataHob (
                  &gEfiMemoryTypeInformationGuid,
                  mDefaultMemoryTypeInformation,
                  sizeof (mDefaultMemoryTypeInformation)
                  );
  } else {
    // APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
    IndexNumber = DataSize / sizeof (EFI_MEMORY_TYPE_INFORMATION);
    // APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.

    //
    // Start with at least 0x200 pages of memory for the DXE Core and the DXE Stack
    //
    for (Index = 0; Index < IndexNumber; Index++) {
      DEBUG((EFI_D_ERROR, "Index[%d].Type = %d .NumberOfPages=0x%x\n", Index,MemoryData[Index].Type,MemoryData[Index].NumberOfPages));
      *MemorySize += MemoryData[Index].NumberOfPages * EFI_PAGE_SIZE;
      
    }
    DEBUG((EFI_D_ERROR, "has memory type,  Total platform memory:%dKB \n", DivU64x32(*MemorySize,1024)));

// APTIOV_SERVER_OVERRIDE_RC_START: Fall Back to default Memory Map once Memory Limit is reached.
    if (*MemorySize > AvailableMemoryBelow4G) {
      *MemorySize = PEI_MIN_MEMORY_SIZE;
      //
      // Install notify PPIs
      //
      Status = PeiServicesNotifyPpi (&mNotifyList);
      ASSERT_EFI_ERROR (Status);
      DEBUG((EFI_D_ERROR, "Memory Limit Reached. Fall back to Default Memory Map. \n"));
      goto DefaultMemoryTypeInformation;
    }
// APTIOV_SERVER_OVERRIDE_RC_END: Fall Back to default Memory Map once Memory Limit is reached.

    //
    // Build the GUID'd HOB for DXE
    //
    HobPointer = BuildGuidDataHob (
                  &gEfiMemoryTypeInformationGuid,
                  MemoryData,
                  DataSize
                  );
  }

  ASSERT (HobPointer);
  return EFI_SUCCESS;
}

/**

  This function checks the memory range in PEI. 

  @param PeiServices     Pointer to PEI Services.
  @param This            Pei memory test PPI pointer.
  @param BeginAddress    Beginning of the memory address to be checked.
  @param MemoryLength    Bytes of memory range to be checked.
  @param Operation       Type of memory check operation to be performed.
  ErrorAddress    Return the address of the error memory address.
    
  @retval EFI_SUCCESS         The operation completed successfully.
  @retval EFI_DEVICE_ERROR    Memory test failed. It's not safe to use this range of memory.

**/
EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN  EFI_PEI_SERVICES                   **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI           *This,
  IN  EFI_PHYSICAL_ADDRESS               BeginAddress,
  IN  UINT64                             MemoryLength,
  IN  PEI_MEMORY_TEST_OP                 Operation,
  OUT EFI_PHYSICAL_ADDRESS               *ErrorAddress
  )
{
  UINT32                TestPattern;
  UINT32                TestMask;
  UINT32                SpanSize;
  EFI_PHYSICAL_ADDRESS  TempAddress;

  ReportStatusCode (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_MEMORY + EFI_CU_MEMORY_PC_TEST
    );

  TestPattern = MEMORY_TEST_PATTERN;
  TestMask    = 0;
  SpanSize    = 0;

  //
  // Make sure we don't try and test anything above the max physical address range
  //
  ASSERT (BeginAddress + MemoryLength < EFI_MAX_ADDRESS);

  switch (Operation) {
  case Extensive:
    SpanSize = 0x4;
    break;

  case Sparse:
  case Quick:
    SpanSize = MEMORY_TEST_COVER_SPAN;
    break;

  case Ignore:
    goto Done;
    break;
  }
  //
  // Write the test pattern into memory range
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength) {
    (*(UINT32 *) (UINTN) TempAddress) = TestPattern;
    TempAddress += SpanSize;
  }
  //
  // Read pattern from memory and compare it
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength) {
    if ((*(UINT32 *) (UINTN) TempAddress) != TestPattern) {
      *ErrorAddress = TempAddress;
      ReportStatusCode (
        EFI_ERROR_CODE + EFI_ERROR_UNRECOVERED,
        EFI_COMPUTING_UNIT_MEMORY + EFI_CU_MEMORY_EC_UNCORRECTABLE
        );
      return EFI_DEVICE_ERROR;
    }

    TempAddress += SpanSize;
  }

Done:

  return EFI_SUCCESS;
}
