/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for microcode update

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Microcode.c

**/

#include "Cpu.h"
#include "Microcode.h"

// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Token.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
MICROCODE_INFO *mMicrocodeInfo;
//
// PURLEY_OVERRIDE_BEGIN
//
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//extern VOID* mMicrocodePatchAddress;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
//
// PURLEY_OVERRIDE_END
//
/**
  Verify the DWORD type checksum.
  
  This function verifies the DWORD type checksum.

  @param  ChecksumAddr   The handle number of specified processor.
  @param  ChecksumLen    The handle number of specified processor.

**/
EFI_STATUS
Checksum32Verify (
  IN UINT32 *ChecksumAddr,
  IN UINT32 ChecksumLen
  )
{
  UINT32   Checksum;
  UINT32   Index;
  
  Checksum = 0;
  
  for (Index = 0; Index < ChecksumLen; Index ++) {
    Checksum += ChecksumAddr[Index];
  }
  
  return (Checksum == 0) ? EFI_SUCCESS : EFI_CRC_ERROR;
}

/**
  Get microcode update signature of currently loaded microcode udpate.
**/
INT32
GetCurrentMicrocodeSignature (
  VOID
  )
{
  UINT64 Signature;

  AsmWriteMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  Signature = AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID);
  return (INT32) RShiftU64 (Signature, 32);
}

// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
static EFI_GUID gMicrocodeFfsGuid =
    {0x17088572, 0x377F, 0x44ef, 0x8F, 0x4E, 0xB0, 0x9F, 0xFF, 0x46, 0xA0, 0x70};

EFI_CPU_MICROCODE_HEADER * GetMicrocode(EFI_FIRMWARE_VOLUME_HEADER *Fv)
{
	UINT8  *pEndOfFv = (UINT8*)Fv + Fv->FvLength - 1;
	UINT8  *pFfs;
	UINT8  *pEndOfFfs;
	UINT32 FfsSize;

	EFI_CPU_MICROCODE_HEADER  *pUc;
	UINT32 UcSize;
	UINT32 CpuSignature;
	UINT8  CpuFlags = (UINT8)(RShiftU64(AsmReadMsr64(0x17), 50)) & 7;
	UINT8  UcFlag = 1 << CpuFlags;

	AsmCpuid (EFI_CPUID_VERSION_INFO, &CpuSignature, NULL, NULL, NULL);

	//Check for corrupt firmware volume.
	if (Fv->Signature != 'HVF_') return 0;

	if (Fv->ExtHeaderOffset != 0) {
		//Skip extended Header.
		EFI_FIRMWARE_VOLUME_EXT_HEADER *ExtHdr =
				(EFI_FIRMWARE_VOLUME_EXT_HEADER *)((UINT8*)Fv + Fv->ExtHeaderOffset);

		pFfs = (UINT8*)ExtHdr + ExtHdr->ExtHeaderSize;
	} else

	pFfs = (UINT8*)Fv + Fv->HeaderLength;

	pFfs = (UINT8*)(((UINT32)pFfs + 7)& ~7);	//Align to 8 bytes

	if (pEndOfFv <= pFfs) return 0;

	while(TRUE) {
		if ((pFfs + sizeof(EFI_FFS_FILE_HEADER)) >= pEndOfFv) return 0;

		if (((EFI_FFS_FILE_HEADER*)pFfs)->Type != EFI_FV_FILETYPE_FFS_PAD) {
			if (*(UINT32*)pFfs == 0xffffffff) return 0;
		}

		FfsSize = *(UINT32*)&((EFI_FFS_FILE_HEADER*)pFfs)->Size & 0xffffff;

		//Find Microcode file
		if (CompareGuid(&((EFI_FFS_FILE_HEADER*)pFfs)->Name, &gMicrocodeFfsGuid))
			break;
		if ((pFfs + FfsSize) <= pFfs) return 0; //Corruption?
		pFfs +=  FfsSize;  //Next file.
		pFfs = (UINT8*)(((UINT32)pFfs + 7) & ~7);  //Align on 8 byte boundary.
	}

	pEndOfFfs = pFfs + FfsSize;
#if MPDTable_CREATED
	pEndOfFfs -= *(UINT16*)(pEndOfFfs - 2);  //Last 2 bytes is MPDT Length. Don't look in the MPDT for uC.
#endif

	//Find Microcode
	pUc = (EFI_CPU_MICROCODE_HEADER*)(pFfs + sizeof(EFI_FFS_FILE_HEADER));

	while(TRUE) {
		if (pUc->HeaderVersion != 1) return 0;  //End of microcode or corrupt.
		UcSize = pUc->DataSize ? pUc->TotalSize : 2048;

		if (pUc->ProcessorId == CpuSignature && (pUc->ProcessorFlags & UcFlag))
			break;

		if (pUc->TotalSize > (pUc->DataSize + 48)) {        //Extended signature count.
			EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *SigTable = (EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER*)((UINT8*)pUc + pUc->DataSize + 48);
			EFI_CPU_MICROCODE_EXTENDED_TABLE *ExtTable;
			UINT32 ExtSigCount = SigTable->ExtendedSignatureCount;
			UINT8 i;

			if (ExtSigCount >= 20) return 0;    //Corrupt microcode.

			ExtTable = (EFI_CPU_MICROCODE_EXTENDED_TABLE*)(SigTable + 1);
			for (i = 0; i < ExtSigCount; ++i) {
				if (ExtTable[i].ProcessorSignature == CpuSignature && (ExtTable[i].ProcessorFlag & UcFlag)) break;
			}
		}

#if PACK_MICROCODE
		UcSize = (UcSize + (16 - 1)) & ~(16 - 1);
#else
		UcSize = (UcSize + (MICROCODE_BLOCK_SIZE - 1)) & ~(MICROCODE_BLOCK_SIZE - 1);
#endif
		if (((UINT64)(UINTN)pUc + UcSize) >= (UINT64)(UINTN)pEndOfFfs) return 0;   //End of uc or corrupt

		pUc = (EFI_CPU_MICROCODE_HEADER*)((UINT8*)pUc + UcSize);
	}
	return pUc;
}
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV

/**
  Allocate buffer for Microcode configuration.
  
**/
VOID
MicrocodeAllocateBuffer (
  VOID
  )
{
  mMicrocodeInfo = (MICROCODE_INFO *)AllocatePool (
                                       sizeof (MICROCODE_INFO) * mCpuConfigContextBuffer.NumberOfProcessors
                                       );
  ASSERT (mMicrocodeInfo != NULL);
}

/**
  Detect whether specified processor can find matching mirocode patch.
  
  This function detects whether specified processor can find matching mirocode patch.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeDetect (
  IN UINTN   ProcessorNumber,
  IN BOOLEAN FirstLoad
  )
{
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//  UINT32                                  ExtendedTableLength;
//  UINT32                                  ExtendedTableCount;
//  EFI_CPU_MICROCODE_EXTENDED_TABLE        *ExtendedTable;
//  EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *ExtendedTableHeader;
  EFI_CPU_MICROCODE_HEADER                *MicrocodeEntryPoint = 0;
//  UINTN                                   MicrocodeEnd;
//  UINTN                                   Index;
//  UINT8                                   PlatformId;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
  UINT32                                  RegEax;
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
  UINT32                                  LatestRevision = 0;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
  UINTN                                   TotalSize;
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//  EFI_STATUS                              Status;
//  BOOLEAN                                 CorrectMicrocode;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
  INT32                                   CurrentSignature;
  INT32                                   UpdateRevision;
  MICROCODE_INFO                          *MicrocodeInfo;

  if (FirstLoad) {
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//    ExtendedTableLength = 0;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
    //
    // Here data of CPUID leafs have not been collected into context buffer, so
    // GetProcessorCpuid() cannot be used here to retrieve CPUID data.
    //
    AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
#if 0
    //
    // The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
    //
    PlatformId = (UINT8) (RShiftU64 (AsmReadMsr64 (EFI_MSR_IA32_PLATFORM_ID), 50) & 0x07);

    LatestRevision = 0;
    MicrocodeEnd = (UINTN) (PcdGet64 (PcdCpuMicrocodePatchAddress) + PcdGet64 (PcdCpuMicrocodePatchRegionSize));
//
// PURLEY_OVERRIDE_BEGIN
//
    //MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (UINTN) PcdGet64 (PcdCpuMicrocodePatchAddress);
    //MicrocodeEnd = (UINTN) (PcdGet64 (PcdCpuMicrocodePatchAddress) + PcdGet64 (PcdCpuMicrocodePatchRegionSize));
	MicrocodeEntryPoint = mMicrocodePatchAddress;
    MicrocodeEnd = (UINTN) ((UINT64) MicrocodeEntryPoint + PcdGet64 (PcdCpuMicrocodePatchRegionSize));
//
// PURLEY_OVERRIDE_END
//
    do {
      //
      // Check if the microcode is for the Cpu and the version is newer
      // and the update can be processed on the platform
      //
      CorrectMicrocode = FALSE;

      if (MicrocodeEntryPoint->HeaderVersion == 0x1) {
        //
        // It is the microcode header. It is not the padding data between microcode patches
        // because the padding data should not include 0x00000001 and it should be the repeated
        // byte format (like 0xXYXYXYXY....).
        //
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
      } else {
        //
        // It is the padding data between the microcode patches for microcode patches alignment.
        // Because the microcode patch is the multiple of 1-KByte, the padding data should not
        // exist if the microcode patch alignment value is not larger than 1-KByte. So, the microcode
        // alignment value should be larger than 1-KByte. We could skip SIZE_1KB padding data to
        // find the next possible microcode patch header.
        //
        MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (((UINTN) MicrocodeEntryPoint) + SIZE_1KB);
        continue;
      }
#endif

#if MICROCODE_SPLIT_BB_UPDATE
      MicrocodeEntryPoint = GetMicrocode((EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FV_MICROCODE_UPDATE_BASE);
      if (MicrocodeEntryPoint == NULL)
          MicrocodeEntryPoint = GetMicrocode((EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FV_MICROCODE_BASE);
#else      
	  MicrocodeEntryPoint = GetMicrocode((EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FV_MICROCODE_BASE);
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
      //
      // Get the next patch.
      //
      if (MicrocodeEntryPoint->DataSize == 0) {
        TotalSize = 2048;
      } else {
        TotalSize = MicrocodeEntryPoint->TotalSize;
      }
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//      if (CorrectMicrocode) {
	  if (MicrocodeEntryPoint) {
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
          LatestRevision = MicrocodeEntryPoint->UpdateRevision;
          mMicrocodeInfo[ProcessorNumber].MicrocodeData = (VOID *)((UINTN)MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER));
          mMicrocodeInfo[ProcessorNumber].MicrocodeSize = TotalSize;
          mMicrocodeInfo[ProcessorNumber].ProcessorId = RegEax;
       }
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//      MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (((UINTN) MicrocodeEntryPoint) + TotalSize);
//    } while (((UINTN) MicrocodeEntryPoint < MicrocodeEnd));
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
    if (LatestRevision > 0) {
//
// PURLEY_OVERRIDE_BEGIN
//
//
// PURLEY_OVERRIDE_END
//
      //
      // Get microcode update signature of currently loaded microcode update
      //
      CurrentSignature = GetCurrentMicrocodeSignature ();
      //
      // If no microcode update has been loaded, then trigger first time microcode load.
      //
      if (CurrentSignature == 0) {        
//
// PURLEY_OVERRIDE_BEGIN
//        
      //This change has been made to load both Patch23 and Microcode when enabled from Setup.  
//
// PURLEY_OVERRIDE_END
//
        AsmWriteMsr64 (
          EFI_MSR_IA32_BIOS_UPDT_TRIG, 
          (UINT64) (UINTN) mMicrocodeInfo[ProcessorNumber].MicrocodeData
          );
        mMicrocodeInfo[ProcessorNumber].Load = TRUE;
//
// PURLEY_OVERRIDE_BEGIN
//
      } else {
        mMicrocodeInfo[ProcessorNumber].Load = FALSE;
      }
//
// PURLEY_OVERRIDE_END
//
      //
      // If the patch is qualified, set corresponding capability, together with the microcode address
      // and the index of the patch.
      //
      SetProcessorFeatureCapability (ProcessorNumber, Microcode, &mMicrocodeInfo[ProcessorNumber]);
    }
  } else {
    //
    // second time microcode update load
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, Microcode, (VOID**) &MicrocodeInfo)) {
      CurrentSignature    = GetCurrentMicrocodeSignature ();
      MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *)((UINT8 *)(MicrocodeInfo->MicrocodeData) - sizeof (EFI_CPU_MICROCODE_HEADER));
      UpdateRevision      = (INT32) MicrocodeEntryPoint->UpdateRevision;
      
      if (UpdateRevision < 0 || (UpdateRevision > 0 && UpdateRevision > CurrentSignature)) {
        AsmWriteMsr64 (
          EFI_MSR_IA32_BIOS_UPDT_TRIG, 
          (UINT64) (UINTN) MicrocodeInfo->MicrocodeData
          );
        SetProcessorFeatureCapability (ProcessorNumber, Microcode2, MicrocodeInfo->MicrocodeData);
      }
    }
  }
}

/**
  Generate entry for microcode load in S3 register table.
  
  For first time microcode load, if needed, an entry is generated in the pre-SMM-init register table.
  For second time microcode load, if needed, an entry is generated in the post-SMM-init register table.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeReg (
  IN UINTN    ProcessorNumber,
  IN BOOLEAN  FirstLoad
  )
{
  MICROCODE_INFO  *MicrocodeInfo;
  VOID            *MicrocodeData;

  if (FirstLoad) {
    if (GetProcessorFeatureCapability (ProcessorNumber, Microcode, (VOID **)&MicrocodeInfo) && MicrocodeInfo->Load) {
      WritePreSmmInitRegisterTable (
        ProcessorNumber,
        Msr,
        EFI_MSR_IA32_BIOS_UPDT_TRIG,
        0,
        64,
        (UINT64)(UINTN)MicrocodeInfo->MicrocodeData
        );
    }
  } else {
    if (GetProcessorFeatureCapability (ProcessorNumber, Microcode2, (VOID **)&MicrocodeData)) {
      WriteRegisterTable (
        ProcessorNumber,
        Msr,
        EFI_MSR_IA32_BIOS_UPDT_TRIG,
        0,
        64,
        (UINT64)(UINTN)MicrocodeData
        );
    }
  }
}

/**
  Measure loaded CPU microcode.
  
**/
VOID
MeasureMicrocode (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_TCG_PROTOCOL          *TcgProtocol;
  UINTN                     ProcessorNumber;
  UINTN                     Index;
  MICROCODE_INFO            *MicrocodeInfo;
  TCG_PCR_EVENT             *TcgEvent;
  EFI_PHYSICAL_ADDRESS      EventLogLastEntry;
  UINT32                    EventNumber;
  CHAR8                     MicrocodeDescriptor[MAX_MICROCODE_DESCRIPTOR_LENGTH];
  EFI_CPU_MICROCODE_HEADER  *MicrocodeEntryPoint;
  UINTN                     DescriptorLen;

  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    //
    // If EFI TCG Protocol is not present, microcode won't be measured
    //
    Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **)&TcgProtocol);
    if (EFI_ERROR (Status)) {
      return ;
    }

    for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
      //
      // Measure any microcode loaded by this driver or earlier by other modules
      //
      if (GetProcessorFeatureCapability (ProcessorNumber, Microcode, (VOID **)&MicrocodeInfo)) {
        for (Index = 0; Index < ProcessorNumber; Index++) {
          if (MicrocodeInfo->MicrocodeData == mMicrocodeInfo[Index].MicrocodeData) {
            break;
          }
        }
        //
        //Same microcode won't be measured multiple times
        //
        if (Index >= ProcessorNumber) {
          MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *)MicrocodeInfo->MicrocodeData;
          DescriptorLen = AsciiSPrint (
                            MicrocodeDescriptor,
                            MAX_MICROCODE_DESCRIPTOR_LENGTH,
                            "Load microcode revision %8X for processor %8X",
                            MicrocodeEntryPoint->UpdateRevision,
                            MicrocodeInfo->ProcessorId
                            );

          TcgEvent = (TCG_PCR_EVENT *)AllocateZeroPool (sizeof (TCG_PCR_EVENT) + DescriptorLen);
          ASSERT (TcgEvent != NULL);

          //
          // Measure microcode into PCR[1] using event type EV_CPU_MICROCODE.
          // The digest field contains the SHA-1 hash of the microcode patch applied.
          // The event field contains a descriptor of the microcode patch.
          //
          TcgEvent->PCRIndex   = 1;
          TcgEvent->EventType  = EV_CPU_MICROCODE;
          TcgEvent->EventSize  = (UINT32)(DescriptorLen + 1);
          CopyMem (&TcgEvent->Event[0], MicrocodeDescriptor, DescriptorLen + 1);
          EventNumber = 1;
          Status = TcgProtocol->HashLogExtendEvent (
                     TcgProtocol,
                     (EFI_PHYSICAL_ADDRESS) (UINTN) (VOID *) MicrocodeEntryPoint,
                     (UINT64) MicrocodeInfo->MicrocodeSize,
                     TPM_ALG_SHA,
                     TcgEvent,
                     &EventNumber,
                     &EventLogLastEntry
                     );

          FreePool (TcgEvent);
        }
      }
    }
  }
}

