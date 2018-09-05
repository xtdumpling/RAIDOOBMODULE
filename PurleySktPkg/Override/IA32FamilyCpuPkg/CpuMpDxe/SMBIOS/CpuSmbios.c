/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Code to log processor and cache subclass data to smbios record.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "Processor.h"
#include "Cache.h"

EFI_SMBIOS_PROTOCOL     *mSmbios;
EFI_HII_HANDLE          mStringHandle;
UINT32                  mPopulatedSocketCount;
//
// PURLEY_OVERRIDE_BEGIN
//
extern EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;
//
// PURLEY_OVERRIDE_END
//

/**
  Add SMBIOS Processor Type and Cache Type tables for the CPU.
**/
VOID
AddCpuSmbiosTables (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_SMBIOS_HANDLE     L1CacheHandle;
  EFI_SMBIOS_HANDLE     L2CacheHandle;
  EFI_SMBIOS_HANDLE     L3CacheHandle;
  UINT32                PreviousPackageNumber;
  UINT32                PackageNumber;
  UINTN                 ProcessorIndex;
  UINTN                 *SocketProcessorNumberTable;
  UINT32                SocketIndex;
//
// PURLEY_OVERRIDE_BEGIN
//
  UINTN                 *PackageNumberList;
//
// PURLEY_OVERRIDE_END
//

  L1CacheHandle = 0xFFFF;
  L2CacheHandle = 0xFFFF;
  L3CacheHandle = 0xFFFF;

  //
  // Initialize the mSmbios to contain the SMBIOS protocol,
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &mSmbios);
  ASSERT_EFI_ERROR (Status);

//
// PURLEY_OVERRIDE_BEGIN
//
  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);
//
// PURLEY_OVERRIDE_END
//

  //
  // Initialize strings to HII database
  //
  mStringHandle = HiiAddPackages (
                    &gEfiCallerIdGuid,
                    NULL,
                    CpuMpDxeStrings,
                    NULL
                    );
  ASSERT (mStringHandle != NULL);

  SocketProcessorNumberTable = AllocateZeroPool (mCpuConfigContextBuffer.NumberOfProcessors * sizeof (UINTN));
  ASSERT (SocketProcessorNumberTable != NULL);

//
// PURLEY_OVERRIDE_BEGIN
//
  PackageNumberList = NULL;
  PackageNumberList = AllocateZeroPool (PcdGet32 (PcdPlatformCpuSocketCount) * sizeof (UINTN));
  ASSERT (PackageNumberList != NULL);
//
// PURLEY_OVERRIDE_END
//

  //
  // Detect populated sockets (comparing the processors' PackangeNumber) and record their ProcessorNumber.
  // For example:
  //   ProcessorNumber: 0 1 2 3 (PackageNumber 0) 4 5 6 7 (PackageNumber 1)
  //   And then, populated socket count will be 2 and record ProcessorNumber 0 for Socket 0, ProcessorNumber 4 for Socket 1
  //

  //
  // System has 1 populated socket at least, initialize mPopulatedSocketCount to 1 and record ProcessorNumber 0 for it.
  //
  mPopulatedSocketCount = 1;
  SocketProcessorNumberTable[0] = 0;
  GetProcessorLocation (0, &PreviousPackageNumber, NULL, NULL);
//
// PURLEY_OVERRIDE_BEGIN
//
  PackageNumberList[0] = PreviousPackageNumber;
//
// PURLEY_OVERRIDE_END
//

  //
  // Scan and compare the processors' PackageNumber to find the populated sockets.
  //
  for (ProcessorIndex = 1; ProcessorIndex < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorIndex++) {
    GetProcessorLocation (ProcessorIndex, &PackageNumber, NULL, NULL);
    if (PackageNumber != PreviousPackageNumber) {
      //
      // Found a new populated socket.
      //
      PreviousPackageNumber = PackageNumber;
      mPopulatedSocketCount++;
      SocketProcessorNumberTable[mPopulatedSocketCount - 1] = ProcessorIndex;
//
// PURLEY_OVERRIDE_BEGIN
//
      PackageNumberList[mPopulatedSocketCount - 1] = PackageNumber;
//
// PURLEY_OVERRIDE_END
//
    }
  }

  //
  // Add SMBIOS tables for populated sockets.
  //
  for (SocketIndex = 0; SocketIndex < mPopulatedSocketCount; SocketIndex++) {
    AddSmbiosCacheTypeTable (SocketProcessorNumberTable[SocketIndex], &L1CacheHandle, &L2CacheHandle, &L3CacheHandle);
    AddSmbiosProcessorTypeTable (SocketProcessorNumberTable[SocketIndex], L1CacheHandle, L2CacheHandle, L3CacheHandle);
  }
//
// PURLEY_OVERRIDE_BEGIN
//
  FreePool (PackageNumberList);
//
// PURLEY_OVERRIDE_END
//
  FreePool (SocketProcessorNumberTable);

  //
  // Register notification functions for Smbios Processor Type.
  //
  SmbiosProcessorTypeTableCallback ();
}

