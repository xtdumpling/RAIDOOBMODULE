/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2015, Intel Corporation.

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


  @file AcpiPlatformLibPcct.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


EFI_RASF_SUPPORT_PROTOCOL          mRasfProtocol;


/**

    GC_TODO: add routine description

    @param MpService    - GC_TODO: add arg description
    @param NumberOfCPUs - GC_TODO: add arg description

    @retval Status - GC_TODO: add retval description

**/
EFI_STATUS
AllocateRasfSharedMemory (
  VOID
)
{

  EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF    *RasfSharedMemoryRegion;
  EFI_STATUS                                Status = EFI_SUCCESS;
  EFI_HANDLE                                Handle = NULL;


  //Allocate ACPI Reserved Memory for RASF mail box  and update the required fields in the shared memory region
  Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof (EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF), (VOID **) &RasfSharedMemoryRegion);
  ASSERT_EFI_ERROR (Status);

  ZeroMem ((VOID *)RasfSharedMemoryRegion, sizeof(EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF));

  RasfSharedMemoryRegion->Signature = EFI_ACPI_PCC_SIGNATURE | EFI_ACPI_RASF_PCC_IDENTIFIER;

  RasfSharedMemoryRegion->Version = EFI_ACPI_RASF_PCC_VERSION;
  RasfSharedMemoryRegion->RasCapabilities[0] = (GET_DEVICE_CAPABILITY | DEVICE_ONLINE | DEVICE_OFFLINE | MEMORY_MIGRATION | PARTIAL_MIRROR | GET_SPARES);
  RasfSharedMemoryRegion->NumOfRasfParameterBlocks = EFI_ACPI_RASF_PCC_PARAMETER_BLOCKS_NUM;

  // update Get Device Capability Block
  RasfSharedMemoryRegion->GetDeviceCapabilityBlock.Type = RASF_TYPE_GET_DEVICE_CAPABILITY;
  RasfSharedMemoryRegion->GetDeviceCapabilityBlock.Version = EFI_ACPI_RASF_PCC_VERSION;
  RasfSharedMemoryRegion->GetDeviceCapabilityBlock.Length = sizeof (GET_DEVICE_CAPABILITY_PARAMETER_BLOCK);

  // update Online Capability Block
  RasfSharedMemoryRegion->OnlineCapabililtyBlock.Type = RASF_TYPE_DEVICE_ONLINE;
  RasfSharedMemoryRegion->OnlineCapabililtyBlock.Version = EFI_ACPI_RASF_PCC_VERSION;
  RasfSharedMemoryRegion->OnlineCapabililtyBlock.Length = sizeof (ONLINE_PARAMETER_BLOCK);

  // update Offline Capability Block
  RasfSharedMemoryRegion->OfflineCapabilityBlock.Type = RASF_TYPE_DEVICE_OFFLINE;
  RasfSharedMemoryRegion->OfflineCapabilityBlock.Version = EFI_ACPI_RASF_PCC_VERSION;
  RasfSharedMemoryRegion->OfflineCapabilityBlock.Length = sizeof (OFFLINE_PARAMETER_BLOCK);

  // update Memory Migration Capability Block
  RasfSharedMemoryRegion->MigrateMemoryCapabilityBlock.Type = RASF_TYPE_MIGRATE_MEMORY;
  RasfSharedMemoryRegion->MigrateMemoryCapabilityBlock.Version = EFI_ACPI_RASF_PCC_VERSION;
  RasfSharedMemoryRegion->MigrateMemoryCapabilityBlock.Length = sizeof (MIGRATE_MEMORY_PARAMETER_BLOCK);

  // Partial Address Mirroring.
  RasfSharedMemoryRegion->PartialMirroringParamBlock.Version= EFI_ACPI_RASF_PCC_VERSION;
  RasfSharedMemoryRegion->PartialMirroringParamBlock.Type= RASF_TYPE_PARTIAL_MIRRORING;
  RasfSharedMemoryRegion->PartialMirroringParamBlock.Length= sizeof(PARTIAL_MIRRORING_PARAMETER_BLOCK);
  RasfSharedMemoryRegion->PartialMirroringParamBlock.Flags= 0x1; // 0 to 4G range supported.
  RasfSharedMemoryRegion->PartialMirroringParamBlock.MaxMirroringRanges= MC_MAX_NODE ;

  // update Get Spares Command Block
  RasfSharedMemoryRegion->GetSparesParameterBlock.Type = RASF_TYPE_GET_SPARES;
  RasfSharedMemoryRegion->GetSparesParameterBlock.Version = EFI_ACPI_RASF_PCC_VERSION;
  RasfSharedMemoryRegion->GetSparesParameterBlock.Length = sizeof (GET_SPARES_PARAMETER_BLOCK);

  DEBUG((EFI_D_ERROR, ":::: RASF Shared Memory Region = %x\n", RasfSharedMemoryRegion));
  mRasfProtocol.RasfSharedMemoryAddress = RasfSharedMemoryRegion;

  Status = gBS->InstallProtocolInterface (&Handle, &gEfiRasfSupportProtocolGuid, EFI_NATIVE_INTERFACE, &mRasfProtocol);
  ASSERT_EFI_ERROR (Status);

  return Status;

}


/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
EFI_STATUS
PatchPlatformCommunicationsChannelTable(
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  )
{
  EFI_STATUS                               Status;
  EFI_ACPI_PLATFORM_COMMUNICATIONS_CHANNEL_TABLE   *PcctTable;

  PcctTable = (EFI_ACPI_PLATFORM_COMMUNICATIONS_CHANNEL_TABLE *)Table;

  //  Update RASF PCC
  Status = AllocateRasfSharedMemory();
  PcctTable->PccSubspaceStructure[EFI_ACPI_PCC_RASF_SUBSPACE_STRUCTURE_ID].BaseAddress  = (UINT64)(UINTN)(mRasfProtocol.RasfSharedMemoryAddress);
  PcctTable->PccSubspaceStructure[EFI_ACPI_PCC_RASF_SUBSPACE_STRUCTURE_ID].MemLength    = sizeof(EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF);

  return Status;
}
