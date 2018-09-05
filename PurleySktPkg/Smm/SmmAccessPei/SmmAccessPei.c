/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights 
reserved This software and associated documentation (if any) is 
furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by 
such license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file SmmAccessPei.c

  This is the driver that publishes the SMM Access Ppi
  instance for the SNB-IIO chipset.

Revision History:

**/
#include <PiPei.h>
#include <Ppi/SmmAccess.h>
#include <Guid/SmramMemoryReserve.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PciLib.h>
#include <Library/PeiServicesLib.h>
#include "IioUniversalData.h"

#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a) \
  CR ( \
  a, \
  SMM_ACCESS_PRIVATE_DATA, \
  SmmAccess, \
  SMM_ACCESS_PRIVATE_DATA_SIGNATURE \
  )

//
// Constant definition
//
#define  MAX_SMRAM_RANGES    4

typedef struct {
  UINTN                            Signature;
  EFI_HANDLE                       Handle;
  PEI_SMM_ACCESS_PPI               SmmAccess;
  UINTN                            NumberRegions;
  EFI_SMRAM_DESCRIPTOR             SmramDesc[MAX_SMRAM_RANGES];
  UINT8                            TsegSize;
} SMM_ACCESS_PRIVATE_DATA;

#define  SMM_ACCESS_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('i', 's', 'm', 'a')

EFI_GUID  mUniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;

/**

  This routine accepts a request to "open" a region of SMRAM.  The
  region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
  The use of "open" means that the memory is visible from all PEIM
  and SMM agents.

  @param PeiServices      - General purpose services available to every PEIM.
  @param This             -  Pointer to the SMM Access Interface.
  @param DescriptorIndex  -  Region of SMRAM to Open.

  @retval EFI_SUCCESS            -  The region was successfully opened.
  @retval EFI_DEVICE_ERROR       -  The region could not be opened because locked by
                            @retval chipset.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_SMM_ACCESS_PPI         *This,
  IN UINTN                      DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_LOCKED) {
    return EFI_DEVICE_ERROR;
  }

  SmmAccess->SmramDesc[DescriptorIndex].RegionState &= ~(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_OPEN;
  SmmAccess->SmmAccess.OpenState = TRUE;

  return EFI_SUCCESS;
}

/**

  This routine accepts a request to "close" a region of SMRAM.  This is valid for
  compatible SMRAM region.

  @param PeiServices      - General purpose services available to every PEIM.
  @param This             -  Pointer to the SMM Access Interface.
  @param DescriptorIndex  -  Region of SMRAM to Close.

  @retval EFI_SUCCESS            -  The region was successfully closed.
  @retval EFI_DEVICE_ERROR       -  The region could not be closed because locked by
                            @retval chipset.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_PEI_SERVICES        **PeiServices,
  IN PEI_SMM_ACCESS_PPI      *This,
  IN UINTN                   DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  BOOLEAN                 OpenState;
  UINTN                   Index;


  SmmAccess     = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_LOCKED) {
    return EFI_DEVICE_ERROR;
  }

  if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_CLOSED) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Find out if any regions are still open
  //
  OpenState = FALSE;
  for (Index = 0; Index < SmmAccess->NumberRegions; Index++) {
    if ((SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_OPEN) == EFI_SMRAM_OPEN) {
      OpenState = TRUE;
    }
  }

  SmmAccess->SmmAccess.OpenState = OpenState;

  return EFI_SUCCESS;
}

/**

  This routine accepts a request to "lock" SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "lock" means that the memory can no longer be opened
  to PEIM.

  @param PeiServices      - General purpose services available to every PEIM.
  @param This             -  Pointer to the SMM Access Interface.
  @param DescriptorIndex  -  Region of SMRAM to Lock.

  @retval EFI_SUCCESS            -  The region was successfully locked.
  @retval EFI_DEVICE_ERROR       -  The region could not be locked because at least
                            @retval one range is still open.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_SMM_ACCESS_PPI        *This,
  IN UINTN                     DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  
  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmmAccess.OpenState) {
    return EFI_DEVICE_ERROR;
  }

  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_LOCKED;
  SmmAccess->SmmAccess.LockState                     = TRUE;

  return EFI_SUCCESS;
}

/**

  This routine services a user request to discover the SMRAM
  capabilities of this platform.  This will report the possible
  ranges that are possible for SMRAM access, based upon the
  memory controller capabilities.

  @param PeiServices   - General purpose services available to every PEIM.
  @param This          -  Pointer to the SMRAM Access Interface.
  @param SmramMapSize  -  Pointer to the variable containing size of the
                          buffer to contain the description information.
  @param SmramMap      -  Buffer containing the data describing the Smram
                          region descriptors.
  @retval EFI_BUFFER_TOO_SMALL  -  The user did not provide a sufficient buffer.
  @retval EFI_SUCCESS           -  The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN PEI_SMM_ACCESS_PPI              *This,
  IN OUT UINTN                       *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR        *SmramMap
  )
{
  EFI_STATUS                Status;
  SMM_ACCESS_PRIVATE_DATA  *SmmAccess;
  UINTN                     BufferSize;

  SmmAccess           = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);
  BufferSize          = SmmAccess->NumberRegions * sizeof (EFI_SMRAM_DESCRIPTOR);

  if (*SmramMapSize < BufferSize) {
    Status = EFI_BUFFER_TOO_SMALL;
  } else {
    CopyMem (SmramMap, SmmAccess->SmramDesc, *SmramMapSize);
    Status = EFI_SUCCESS;
  }

  *SmramMapSize = BufferSize;

  return Status;
}


/**

    This is the constructor for the SMM Access Ppi

    @param FfsHeader       - FfsHeader.
    @param PeiServices     - General purpose services available to every PEIM.

  @retval EFI_SUCCESS     -  Protocol successfully started and installed.
  @retval EFI_UNSUPPORTED -  Protocol can't be started.
**/
EFI_STATUS
EFIAPI
SmmAccessPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{

  EFI_STATUS                      Status;
  UINTN                           Index;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *DescriptorBlock;
  SMM_ACCESS_PRIVATE_DATA         *SmmAccessPrivate;
  EFI_PEI_PPI_DESCRIPTOR          *PpiList;
  EFI_HOB_GUID_TYPE               *GuidHob;
  UINT32                          TsegSize;
  UINT32                          TsegBase;
  UINT32                          IedSize;
  IIO_UDS                         *IioUds;

  //
  // Initialize private data
  //
  SmmAccessPrivate = AllocatePool (sizeof(*SmmAccessPrivate));
  ASSERT(SmmAccessPrivate);
  if (SmmAccessPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PpiList = AllocatePool (sizeof(*PpiList));
  ASSERT (PpiList);
  if (PpiList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Build SMM related information
  //
  SmmAccessPrivate->Signature = SMM_ACCESS_PRIVATE_DATA_SIGNATURE;

  //
  // Get Hob list
  //
  GuidHob    = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  DescriptorBlock = GET_GUID_HOB_DATA(GuidHob); 
  //
  // Get IIO Hob list
  //
  GuidHob    = GetFirstGuidHob (&mUniversalDataGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  IioUds = GET_GUID_HOB_DATA(GuidHob); 

  //
  // Use the hob to publish SMRAM capabilities
  //
  ASSERT (DescriptorBlock->NumberOfSmmReservedRegions <= MAX_SMRAM_RANGES);
  for (Index = 0; Index < DescriptorBlock->NumberOfSmmReservedRegions; Index++) {
    SmmAccessPrivate->SmramDesc[Index].PhysicalStart = DescriptorBlock->Descriptor[Index].PhysicalStart;
    SmmAccessPrivate->SmramDesc[Index].CpuStart      = DescriptorBlock->Descriptor[Index].CpuStart;
    SmmAccessPrivate->SmramDesc[Index].PhysicalSize  = DescriptorBlock->Descriptor[Index].PhysicalSize;
    SmmAccessPrivate->SmramDesc[Index].RegionState   = DescriptorBlock->Descriptor[Index].RegionState;
  }

  SmmAccessPrivate->NumberRegions              = Index;
  SmmAccessPrivate->SmmAccess.Open             = Open;
  SmmAccessPrivate->SmmAccess.Close            = Close;
  SmmAccessPrivate->SmmAccess.Lock             = Lock;
  SmmAccessPrivate->SmmAccess.GetCapabilities  = GetCapabilities;
  SmmAccessPrivate->SmmAccess.LockState        = FALSE;
  SmmAccessPrivate->SmmAccess.OpenState        = FALSE;

  PpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiList->Guid  = &gPeiSmmAccessPpiGuid;
  PpiList->Ppi   = &SmmAccessPrivate->SmmAccess;

  Status      = (**PeiServices).InstallPpi (PeiServices, PpiList);
  ASSERT_EFI_ERROR(Status);

  //
  // We require the last entry of SMRAM must be TSEG smram (not including IED region)
  // TSEG base must be the same as SMRAM base
  // IED  base is right after SMRAM, and it extends to TOLM.
  //
  TsegBase = (UINT32)(SmmAccessPrivate->SmramDesc[SmmAccessPrivate->NumberRegions-1].PhysicalStart);
  TsegSize = (UINT32)(IioUds->PlatformData.MemTsegSize);
  DEBUG ((EFI_D_ERROR, "TSEG Base: %08X\n", TsegBase));
  DEBUG ((EFI_D_ERROR, "TSEG Size: %08X\n", TsegSize));

  IedSize = (UINT32)(IioUds->PlatformData.MemIedSize);
  DEBUG ((EFI_D_ERROR, "IED  Base: %08X\n", TsegBase + (UINT32)(SmmAccessPrivate->SmramDesc[SmmAccessPrivate->NumberRegions-1].PhysicalSize)));
  DEBUG ((EFI_D_ERROR, "IED  Size: %08X\n", IedSize));

  DEBUG ((EFI_D_ERROR, "SMM  Base: %08X\n", (UINT32)(SmmAccessPrivate->SmramDesc[SmmAccessPrivate->NumberRegions-1].PhysicalStart)));
  DEBUG ((EFI_D_ERROR, "SMM  Size: %08X\n", (UINT32)(SmmAccessPrivate->SmramDesc[SmmAccessPrivate->NumberRegions-1].PhysicalSize)));


  SmmAccessPrivate->TsegSize = (UINT8)TsegSize;
  //
  // T Seg setting done in QPI RC
  //
  return EFI_SUCCESS;
}

