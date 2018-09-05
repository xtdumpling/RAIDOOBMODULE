/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MicrocodeLoad.c

Abstract:

--*/

#include "CpuHotAdd.h"

EFI_STATUS
Checksum32Verify (
  IN UINT32 *ChecksumAddr,
  IN UINT32 ChecksumLen
  )
/*++

Routine Description:

  Verify the DWORD type checksum

Arguments:
  ChecksumAddr  - The start address to be checkumed

  ChecksumType  - The type of data to be checksumed

Returns:

  EFI_SUCCESS           - Checksum correct
  EFI_CRC_ERROR         - Checksum incorrect

--*/
{
  UINT32   Checksum;
  UINT32   Index;

  Checksum = 0;

  for (Index = 0; Index < ChecksumLen; Index ++) {
    Checksum += ChecksumAddr[Index];
  }

  return (Checksum == 0) ? EFI_SUCCESS : EFI_CRC_ERROR;
}


VOID
LoadMicrocodeEx (
  )
{
  UINT32                                  ExtendedTableLength;
  UINT32                                  ExtendedTableCount;
  EFI_CPU_MICROCODE_EXTENDED_TABLE        *ExtendedTable;
  EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *ExtendedTableHeader;
  EFI_CPU_MICROCODE_HEADER                *MicrocodeEntryPoint;
  UINTN                                   MicrocodeEnd;
  UINTN                                   Index;
  UINT8                                   PlatformId;
  UINT32                                  RegEax;
  UINT32                                  LatestRevision;
  UINTN                                   TotalSize;
  EFI_STATUS                              Status;
  BOOLEAN                                 CorrectMicrocode;


  ExtendedTableLength = 0;
  //
  // Here data of CPUID leafs have not been collected into context buffer, so
  // GetProcessorCpuid() cannot be used here to retrieve CPUID data.
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);

  //
  // The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
  //
  PlatformId = (UINT8) (RShiftU64 (AsmReadMsr64 (EFI_MSR_IA32_PLATFORM_ID), 50) & 0x07);

  LatestRevision = 0;
  MicrocodeEnd = (UINTN) (PcdGet64 (PcdCpuMicrocodePatchAddress) + PcdGet64 (PcdCpuMicrocodePatchRegionSize));
  MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (UINTN) PcdGet64 (PcdCpuMicrocodePatchAddress);
  do {
    //
    // Check if the microcode is for the Cpu and the version is newer
    // and the update can be processed on the platform
    //
    CorrectMicrocode = FALSE;

    if (MicrocodeEntryPoint->HeaderVersion == 0x1) {
      if (MicrocodeEntryPoint->ProcessorId == RegEax &&
          MicrocodeEntryPoint->UpdateRevision > LatestRevision &&
            (MicrocodeEntryPoint->ProcessorFlags & (1 << PlatformId))
          ) {
          if (MicrocodeEntryPoint->DataSize == 0) {
            Status = Checksum32Verify ((UINT32 *)MicrocodeEntryPoint, 2048 / sizeof(UINT32));
          } else {
            Status = Checksum32Verify ((UINT32 *)MicrocodeEntryPoint, (MicrocodeEntryPoint->DataSize + sizeof(EFI_CPU_MICROCODE_HEADER)) / sizeof(UINT32));
          }
          if (!EFI_ERROR (Status)) {
            CorrectMicrocode = TRUE;
          }
      } else if ((MicrocodeEntryPoint->DataSize != 0) &&
                 (MicrocodeEntryPoint->UpdateRevision > LatestRevision)) {

          ExtendedTableLength = MicrocodeEntryPoint->TotalSize - (MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER));
          if (ExtendedTableLength != 0) {
            //
            // Extended Table exist, check if the CPU in support list
            //
            ExtendedTableHeader = (EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *)((UINT8 *)(MicrocodeEntryPoint) + MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER));
            //
            // Calulate Extended Checksum
            //
            if ((ExtendedTableLength % 4) == 0) {
              Status = Checksum32Verify ((UINT32 *)ExtendedTableHeader, ExtendedTableLength / sizeof(UINT32));
              if (!EFI_ERROR (Status)) {
                //
                // Checksum correct
                //
                ExtendedTableCount = ExtendedTableHeader->ExtendedSignatureCount;
                ExtendedTable      = (EFI_CPU_MICROCODE_EXTENDED_TABLE *)(ExtendedTableHeader + 1);
                for (Index = 0; Index < ExtendedTableCount; Index ++) {
                  Status = Checksum32Verify ((UINT32 *)ExtendedTable, sizeof(EFI_CPU_MICROCODE_EXTENDED_TABLE) / sizeof(UINT32));
                  if (!EFI_ERROR (Status)) {
                    //
                    // Verify Header
                    //
                    if ((ExtendedTable->ProcessorSignature == RegEax) &&
                        (ExtendedTable->ProcessorFlag & (1 << PlatformId)) ) {
                      //
                      // Find one
                      //
                      CorrectMicrocode = TRUE;
                      break;
                    }
                  }
                  ExtendedTable ++;
                }
              }
            }
         }

      }
    }

     if (CorrectMicrocode) {
        //
        // If the patch is qualified, set corresponding capability, together with the microcode address
        // and the index of the patch.
        //
        LatestRevision = MicrocodeEntryPoint->UpdateRevision;
        //SetProcessorFeatureCapability (ProcessorNumber, Microcode, (VOID *)((UINTN)MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER)));

        AsmWriteMsr64 (
          EFI_MSR_IA32_BIOS_UPDT_TRIG,
          (UINT64) ((UINTN) MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER))
          );
     }

    //
    // Get the next patch.
    //
    if (MicrocodeEntryPoint->DataSize == 0) {
      TotalSize = 2048;
    } else {
      TotalSize = MicrocodeEntryPoint->TotalSize;
    }
    MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (((UINTN) MicrocodeEntryPoint) + TotalSize);
  } while (((UINTN) MicrocodeEntryPoint < MicrocodeEnd) && (MicrocodeEntryPoint->TotalSize != 0xFFFFFFFF));
}


