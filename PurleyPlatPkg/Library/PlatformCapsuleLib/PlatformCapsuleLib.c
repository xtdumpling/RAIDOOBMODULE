/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/** @file
  Capsule Library instance to update capsule image to flash. 

Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/
#include <PiDxe.h>
#include <Register/PchRegsPmc.h>
#include <Guid/ServerCapsuleGuid.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CapsuleLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GlobalNvsArea.h>

#include <BiosGuardDefinitions.h>
#include <Cpu/CpuRegs.h>

extern	VOID SendCapsuleSmi(UINTN Addr);
extern  VOID GetUpdateStatusSmi(UINTN Addr);

// APTIOV_SERVER_OVERRIDE_RC_START
// extern BOOLEAN BiosGuardEnabled;
BOOLEAN     BiosGuardEnabled = FALSE;
// APTIOV_SERVER_OVERRIDE_RC_END

/**
  Those capsules supported by the firmwares.

  @param  CapsuleHeader    Points to a capsule header.

  @retval EFI_SUCESS       Input capsule is supported by firmware.
  @retval EFI_UNSUPPORTED  Input capsule is not supported by the firmware.
**/
EFI_STATUS
EFIAPI
SupportCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  UINTN           ArrayNumber;
  EFI_STATUS      Status = EFI_UNSUPPORTED;
  BGUP_HEADER     *BgupHeader = NULL;
  UINT8           PlatIdStr[16] = "PURLEY";
  
  if (BiosGuardEnabled) {
    //
    // Quick check to verify the image passed in is possibly a good BG update package
    //
    Status = EFI_SUCCESS;
    BgupHeader = (BGUP_HEADER*)CapsuleHeader;
    for (ArrayNumber = 0; ArrayNumber < 16; ArrayNumber++) {
      if (BgupHeader->PlatId[ArrayNumber] != PlatIdStr[ArrayNumber]) {
          Print ( L"Error: BIOS Guard is enabled, but invalid BG Update package file supplied.\n");
          Status = EFI_UNSUPPORTED;
          break;
      }
    }
  } else {
    if (CompareGuid (&gEfiServerCapsuleGuid, &CapsuleHeader->CapsuleGuid)) {
      Status = EFI_SUCCESS;
    }
  }
  return Status;
}

/**
  The firmware implements to process the capsule image.

  @param  CapsuleHeader         Points to a capsule header.
  
  @retval EFI_SUCESS            Process Capsule Image successfully. 
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.
  @retval EFI_VOLUME_CORRUPTED  FV volume in the capsule is corrupted.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory.
**/
EFI_STATUS
EFIAPI
ProcessCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  CAPSULE_INFO_PACKET                 *CapsuleInfoPacketPtr;
  CAPSULE_FRAGMENT                    *CapsuleFragmentPtr;
  UPDATE_STATUS_PACKET                *UpdateStatusPacketPtr;
  UINT32                              RegValue;
  UINT32                              OrigRegValue;
  EFI_STATUS                          Status;
  UINT16                              PmBase;
  UINTN                               CurrentBlock = 0;
  EFI_GLOBAL_NVS_AREA_PROTOCOL        *GlobalNvsAreaProtocol;
  BIOS_ACPI_PARAM                     *GlobalNvsArea;
  UINT32                              BiosGuardStatus;
  UINT32                              BiosGuardImageSize;
  BGUP_HEADER                         *BgupHeader = NULL;
  CHAR16                              *BgErrorStr;
  
  if (SupportCapsuleImage (CapsuleHeader) != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  CapsuleFragmentPtr    = (CAPSULE_FRAGMENT *) AllocateZeroPool (sizeof (CAPSULE_FRAGMENT));
  ASSERT (CapsuleFragmentPtr != NULL);
  if (CapsuleFragmentPtr == NULL) {
      return EFI_UNSUPPORTED;
  }

  CapsuleInfoPacketPtr  = (CAPSULE_INFO_PACKET *) AllocateZeroPool (sizeof (CAPSULE_INFO_PACKET));
  ASSERT (CapsuleInfoPacketPtr != NULL);
  if (CapsuleInfoPacketPtr == NULL) {
      return EFI_UNSUPPORTED;
  }

  UpdateStatusPacketPtr = (UPDATE_STATUS_PACKET *) AllocateZeroPool (sizeof (UPDATE_STATUS_PACKET));
  ASSERT (UpdateStatusPacketPtr != NULL);  
  if (UpdateStatusPacketPtr == NULL) {
      return EFI_UNSUPPORTED;
  } 
  
  DEBUG ((EFI_D_INFO, "CapsuleImage Address is %10p, CapsuleImage Size is %8x\n", CapsuleHeader, CapsuleHeader->CapsuleImageSize));
  DEBUG ((EFI_D_INFO, "CapsuleFragment Address is %10p, CapsuleInfo Address is %10p\n", CapsuleFragmentPtr, CapsuleInfoPacketPtr));

  //
  // Prepare structures to store capsule image.
  //
  CapsuleFragmentPtr->Address      = (UINT64) (UINTN) CapsuleHeader;
  CapsuleFragmentPtr->BufferOffset = 0;
  CapsuleFragmentPtr->Size         = CapsuleHeader->CapsuleImageSize;
  CapsuleFragmentPtr->Flags        = BIT0;

  CapsuleInfoPacketPtr->CapsuleLocation = (UINTN) CapsuleFragmentPtr;
  CapsuleInfoPacketPtr->CapsuleSize     = CapsuleHeader->CapsuleImageSize;
  CapsuleInfoPacketPtr->Status          = EFI_SUCCESS;

  PmBase = PcdGet16 (PcdPchAcpiIoPortBaseAddress);
  RegValue     = IoRead32 (PmBase + R_PCH_SMI_EN);
  OrigRegValue = RegValue;

  //
  // Temp disable USB SMI in order to avoid Capsule update SMI is interrupted.
  //
  RegValue = RegValue & (~(B_PCH_SMI_EN_LEGACY_USB | B_PCH_SMI_EN_INTEL_USB2 | B_PCH_SMI_EN_LEGACY_USB2));
  IoWrite32 (PmBase + R_PCH_SMI_EN, RegValue);
  
  if (BiosGuardEnabled) {
    //
    // Locate Global NVS to get BiosGuardBaseAddress, size, and I/O port
    //
    Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &GlobalNvsAreaProtocol);

    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_INFO, "Unable to find Global NVS BIOS Guard values.\n"));
      UpdateStatusPacketPtr->Status = EFI_UNSUPPORTED;
    } else {
      BgupHeader = (BGUP_HEADER*)CapsuleHeader;
      GlobalNvsArea = GlobalNvsAreaProtocol->Area;
      Print (L"Performing BG flash update.  This will take a few minutes.\n");
      //
      // Calculate BG image size (0x20c Certificate size)
      //
      BiosGuardImageSize = sizeof(BGUP_HEADER) + BgupHeader->ScriptSectionSize + 
                      BgupHeader->DataSectionSize + 0x20c;
      //
      // Copy full image to BIOS Guard DMA protected memory location
      //
      gBS->CopyMem((VOID*)GlobalNvsArea->BiosGuardMemAddress, CapsuleHeader, BiosGuardImageSize);

      //
      // Trigger BIOS Guard Update SMI handler
      //
      IoRead8 (GlobalNvsArea->BiosGuardIoTrapAddress);
      
      //
      // BG Update copies the status to the first 64 bits of the mem address, only need first 32
      //
      BiosGuardStatus = *(UINT32*)GlobalNvsArea->BiosGuardMemAddress;
      if (BiosGuardStatus == 0) {
        UpdateStatusPacketPtr->Status = EFI_SUCCESS;
        Print (L"BIOS Guard flash updating is complete!\n");
      } else {
        switch (BiosGuardStatus & 0x0FFFF) {
          case ERR_BAD_BGUP:
            switch ((BiosGuardStatus & 0x0FFFF0000) >> 16) {
              case 0x4:
                BgErrorStr = L"Incorrect update package version";
                break;
              case 0x5:
                BgErrorStr = L"Incorrect Platform ID in update package";
                break;
              default:
                BgErrorStr = L"Invalid update package used";
            }
            break;
          case ERR_SCRIPT_SYNTAX:
            BgErrorStr = L"Invalid script syntax used in update package";
            break;
          case ERR_UNDEFINED_FLASH_OBJECT:
            BgErrorStr = L"Undefined flash object was referenced";
            break;
          case ERR_INVALID_LINE:
            BgErrorStr = L"Invalid line in the script buffer";
            break;
          case ERR_BAD_BGUPC:
            switch ((BiosGuardStatus & 0x0FFFF0000) >> 16) {
              case 0x4:
                BgErrorStr = L"Certificate is an incorrect version";
                break;
              case 0x9:
                BgErrorStr = L"Certificate contains bad key hash";
                break;
              case 0xA:
                BgErrorStr = L"Certificate contains bad signature";
                break;
              default:
                BgErrorStr = L"Invalid update package certificate supplied";
            }
            break;
          case ERR_BAD_SVN:
            BgErrorStr = L"Invalid SVN. Update image version provided is\r\nolder than allowed";
            break;
          default:
            BgErrorStr = L"Internal error";
        }
        Print (L"BIOS Guard Flash update failed. %s.\n", BgErrorStr);
        Print (L"BG Error Code: 0x%x, BG Additional Data: 0x%x\n", (BiosGuardStatus & 0x0FFFF), ((BiosGuardStatus & 0x0FFFF0000) >> 16)); 
        DEBUG((EFI_D_INFO, "BG Error           = 0x%x\n", BiosGuardStatus & 0x0FFFF));
        DEBUG((EFI_D_INFO, "BG Additional Data = 0x%x\n", ((BiosGuardStatus & 0x0FFFF0000) >> 16)));
        UpdateStatusPacketPtr->Status = EFI_UNSUPPORTED;
      }
    }
  } else {
    Print (L"Start to update capsule image!\n");
    //
    // Trig SMI to update Capsule image.
    //
    SendCapsuleSmi((UINTN)CapsuleInfoPacketPtr);

    UpdateStatusPacketPtr->BlocksCompleted = 0;
    UpdateStatusPacketPtr->TotalBlocks = (UINTN) -1;
    UpdateStatusPacketPtr->Status = EFI_SUCCESS;
    
    do{
      gBS->Stall (200000);
      GetUpdateStatusSmi((UINTN)UpdateStatusPacketPtr);
    } while (UpdateStatusPacketPtr->TotalBlocks == (UINTN) -1);
    
    Print (L"Updating %d blocks...\n#", UpdateStatusPacketPtr->TotalBlocks);

    do {

      if (UpdateStatusPacketPtr->TotalBlocks != (UINTN) -1 && CurrentBlock < UpdateStatusPacketPtr->BlocksCompleted) {
        CurrentBlock = UpdateStatusPacketPtr->BlocksCompleted;
        Print (L"#");
      }
      //
      // Trig SMI to get the status to updating capsule image.
      //
      gBS->Stall (200000);
      GetUpdateStatusSmi((UINTN)UpdateStatusPacketPtr);
      
    } while ((UpdateStatusPacketPtr->Status == EFI_SUCCESS) && (UpdateStatusPacketPtr->BlocksCompleted < UpdateStatusPacketPtr->TotalBlocks));
    Print (L"\n");
  }  
  //
  // Restore SMI enable setting.
  //
  IoWrite32 (PmBase + R_PCH_SMI_EN, OrigRegValue);
  
  Status = UpdateStatusPacketPtr->Status;
  if(!BiosGuardEnabled) {
    if (EFI_ERROR (Status)) {
      Print (L"Invalid capsule format. Please furnish a valid capsule. Return status is %r!\n", UpdateStatusPacketPtr->Status);
    } else {
      Print (L"Capsule image updating is complete!\n");
    }
  }
  
  FreePool (CapsuleFragmentPtr);
  FreePool (CapsuleInfoPacketPtr);
  FreePool (UpdateStatusPacketPtr);

  return Status;
}

