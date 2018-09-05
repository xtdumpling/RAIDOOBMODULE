//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
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

Module Name: Bist.h

**/

#ifndef _CPU_BIST_H_
#define _CPU_BIST_H_

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
  );

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
  );

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
  );

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
  );

#endif
