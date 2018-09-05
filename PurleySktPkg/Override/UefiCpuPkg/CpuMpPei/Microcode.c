//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Add EIP 290865 workaround.
//      Reason:     To patch Intel CPU bug cause MCE BANK 9/10/11 error when enter OS.
//      Auditor:    Jacker Yeh
//      Date:       Sep/12/2016
//
//*****************************************************************************
/** @file
  Implementation of loading microcode on processors.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CpuMpPei.h"

// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Token.h>
#include <Cpu/CpuRegs.h>

static EFI_GUID gMicrocodeFfsGuid =
    {0x17088572, 0x377F, 0x44ef, 0x8F, 0x4E, 0xB0, 0x9F, 0xFF, 0x46, 0xA0, 0x70};

extern VOID* mMicrocodePatchAddress;

/**
  Get microcode from the FV .

  @return  Microcode Pointer
           Microcode Size.

**/
EFI_CPU_MICROCODE_HEADER * GetMicrocode(
  IN      EFI_FIRMWARE_VOLUME_HEADER    *Fv,
  IN OUT  UINT32                        *pUcSize
  )
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

    pFfs = (UINT8*)(((UINT32)pFfs + 7)& ~7);    //Align to 8 bytes

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
    
    *pUcSize = UcSize;
    return pUc;
}
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV

/**
  Get microcode update signature of currently loaded microcode update.

  @return  Microcode signature.

**/
UINT32
GetCurrentMicrocodeSignature (
  VOID
  )
{
  UINT64 Signature;

  AsmWriteMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  Signature = AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID);
  return (UINT32) RShiftU64 (Signature, 32);
}

/**
  Detect whether specified processor can find matching microcode patch and load it.

**/
VOID
MicrocodeDetect (
  VOID
  )
{
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
// UINT64                                  MicrocodePatchAddress;
// UINT64                                  MicrocodePatchRegionSize;
// UINT32                                  ExtendedTableLength;
// UINT32                                  ExtendedTableCount;
// EFI_CPU_MICROCODE_EXTENDED_TABLE        *ExtendedTable;
// EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *ExtendedTableHeader;
    EFI_CPU_MICROCODE_HEADER                *MicrocodeEntryPoint = NULL;
// UINTN                                   MicrocodeEnd;
// UINTN                                   Index;
// UINT8                                   PlatformId;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
  UINT32                                  RegEax;
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV 
  UINT32                                  LatestRevision = 0;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
  UINTN                                   TotalSize;
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV 
// UINT32                                  CheckSum32;
// BOOLEAN                                 CorrectMicrocode;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV 
  INT32                                   CurrentSignature;
  MICROCODE_INFO                          MicrocodeInfo;

#if SMCPKG_SUPPORT && EIP290865_WORKAROUND
  // AptioV Server Override Start: Skip MicrocodeDetect() if MSR 0x8B is non-zero.
  if (!AsmReadMsr32 (EFI_MSR_IA32_BIOS_SIGN_ID)) return;
  // AptioV Server Override End: Skip MicrocodeDetect() if MSR 0x8B is non-zero.
#endif
  ZeroMem (&MicrocodeInfo, sizeof (MICROCODE_INFO));
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
#if 0
  MicrocodePatchAddress    = PcdGet64 (PcdCpuMicrocodePatchAddress);
  MicrocodePatchRegionSize = PcdGet64 (PcdCpuMicrocodePatchRegionSize);
  if (MicrocodePatchRegionSize == 0) {
    //
    // There is no microcode patches
    //
    return;
  }

  ExtendedTableLength = 0;
  #endif
 // APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
  //
  // Here data of CPUID leafs have not been collected into context buffer, so
  // GetProcessorCpuid() cannot be used here to retrieve CPUID data.
  //
  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
 // APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
#if 0
  //
  // The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
  //
  PlatformId = (UINT8) AsmMsrBitFieldRead64 (EFI_MSR_IA32_PLATFORM_ID, 50, 52);

  LatestRevision = 0;
  MicrocodeEnd = (UINTN) (MicrocodePatchAddress + MicrocodePatchRegionSize);
  MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (UINTN) MicrocodePatchAddress;
  do {
    //
    // Check if the microcode is for the Cpu and the version is newer
    // and the update can be processed on the platform
    //
    CorrectMicrocode = FALSE;
    if (MicrocodeEntryPoint->HeaderVersion == 0x1) {
      //
      // It is the microcode header. It is not the padding data between microcode patches
      // becasue the padding data should not include 0x00000001 and it should be the repeated
      // byte format (like 0xXYXYXYXY....).
      //
      if (MicrocodeEntryPoint->ProcessorId == RegEax &&
          MicrocodeEntryPoint->UpdateRevision > LatestRevision &&
          (MicrocodeEntryPoint->ProcessorFlags & (1 << PlatformId))
          ) {
        if (MicrocodeEntryPoint->DataSize == 0) {
          CheckSum32 = CalculateSum32 ((UINT32 *)MicrocodeEntryPoint, 2048);
        } else {
          CheckSum32 = CalculateSum32 ((UINT32 *)MicrocodeEntryPoint, MicrocodeEntryPoint->DataSize + sizeof(EFI_CPU_MICROCODE_HEADER));
        }
        if (CheckSum32 == 0) {
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
          // Calculate Extended Checksum
          //
          if ((ExtendedTableLength % 4) == 0) {
            CheckSum32 = CalculateSum32 ((UINT32 *)ExtendedTableHeader, ExtendedTableLength);
            if (CheckSum32 == 0) {
              //
              // Checksum correct
              //
              ExtendedTableCount = ExtendedTableHeader->ExtendedSignatureCount;
              ExtendedTable      = (EFI_CPU_MICROCODE_EXTENDED_TABLE *)(ExtendedTableHeader + 1);
              for (Index = 0; Index < ExtendedTableCount; Index ++) {
                CheckSum32 = CalculateSum32 ((UINT32 *)ExtendedTable, sizeof(EFI_CPU_MICROCODE_EXTENDED_TABLE));
                if (CheckSum32 == 0) {
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
    MicrocodeEntryPoint = mMicrocodePatchAddress;
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
//	 if (CorrectMicrocode) {
    if (MicrocodeEntryPoint) {
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
      LatestRevision = MicrocodeEntryPoint->UpdateRevision;
      MicrocodeInfo.MicrocodeData = (VOID *)((UINTN)MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER));
      MicrocodeInfo.MicrocodeSize = TotalSize;
      MicrocodeInfo.ProcessorId = RegEax;
    }
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load Microcode from FV
//    MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (((UINTN) MicrocodeEntryPoint) + TotalSize);
//  } while (((UINTN) MicrocodeEntryPoint < MicrocodeEnd));
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load Microcode from FV
  if (LatestRevision > 0) {
    //
    // Get microcode update signature of currently loaded microcode update
    //
    CurrentSignature = GetCurrentMicrocodeSignature ();
    //
    // If no microcode update has been loaded, then trigger microcode load.
    //
    if (CurrentSignature == 0) {
      AsmWriteMsr64 (
        EFI_MSR_IA32_BIOS_UPDT_TRIG,
        (UINT64) (UINTN) MicrocodeInfo.MicrocodeData
        );
      MicrocodeInfo.Load = TRUE;
    } else {
      MicrocodeInfo.Load = FALSE;
    }
  }
}
