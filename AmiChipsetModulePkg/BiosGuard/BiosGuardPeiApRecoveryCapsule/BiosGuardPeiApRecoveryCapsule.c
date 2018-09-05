//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file BiosGuardPeiApRecoveryCapsule.c
    This file is used for BIOS Guard flash tool executed "/recovery" and "/capsule".

**/

#include "BiosGuardPeiApRecoveryCapsule.h"

BOOLEAN
IsFlashUpdate (
    EFI_PEI_SERVICES    **PeiServices,
    EFI_BOOT_MODE       *BootMode
) {
    EFI_STATUS                            Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI       *ReadOnlyVariable;
    UINTN                                 BiosGuardCapsuleVarSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
    EFI_GUID                              BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;
    AMI_BIOS_GUARD_RECOVERY               ApRecoveryAddress;

    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    Status = ReadOnlyVariable->GetVariable (
                 ReadOnlyVariable,
                 L"BiosGuardCapsuleVariable",
                 &BiosGuardCapsuleVariableGuid,
                 NULL,
                 &BiosGuardCapsuleVarSize,
                 &ApRecoveryAddress
             );

    if ( !EFI_ERROR( Status ) ) {
        if( ApRecoveryAddress.ReoveryMode == BiosGuardRecoveryMode ) {
            *BootMode = BOOT_IN_RECOVERY_MODE;
            DEBUG ((EFI_D_INFO, "AP executed the command of /recovery\n"));
            return TRUE;
        } else if( ApRecoveryAddress.ReoveryMode == BiosGuardCapsuleMode ) {
            *BootMode = BOOT_ON_FLASH_UPDATE;
            DEBUG ((EFI_D_INFO, "AP executed the command of /capsule\n"));
            return TRUE;
        } else {
            DEBUG ((EFI_D_INFO, "ApRecoveryAddress.ReoveryMode = %X\n", ApRecoveryAddress.ReoveryMode));
        }
    }

    return FALSE;
}

EFI_STATUS
BiosGuardCapsuleChangeBootModeAfterEndofMrc (
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
    IN VOID                          *InvokePpi
)
{
    EFI_STATUS           Status;
    EFI_BOOT_MODE        BootMode;

    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] Get Boot Mode is fail\n"));
#endif
        return   Status;
    }

#if BIOS_GUARD_DEBUG_MODE
    DEBUG ((EFI_D_INFO, "BiosGuardRecovery : BootMode = %X\n", BootMode));
#endif

    if ( !EFI_ERROR(Status) && (BootMode == BOOT_ON_S3_RESUME) ) {
        //
        // Change the Boot Mode to BOOT_ON_FLASH_UPDATE from S3_RESUME
        //
        if ( IsFlashUpdate(PeiServices, &BootMode) ) {
            PeiServicesSetBootMode(BootMode);
            return Status;
        }

        if ( BootMode == BOOT_ON_FLASH_UPDATE ) {
            PeiServicesSetBootMode(BootMode);
            DEBUG ((EFI_D_INFO, "BiosGuardRecovery : Change boot mode to BOOT_ON_FLASH_UPDATE\n"));
        }
    }

    return Status;
}

EFI_STATUS
GetBiosGuardCapsuleInfo (
    IN EFI_CAPSULE_BLOCK_DESCRIPTOR   *Desc,
    IN OUT UINTN                      *NumDescriptors OPTIONAL,
    IN OUT UINTN                      *CapsuleSize OPTIONAL,
    IN OUT UINTN                      *CapsuleNumber OPTIONAL
)
{
    UINTN                          Count;
    UINTN                          Size;
    UINTN                          Number;
    UINTN                          ThisCapsuleImageSize;
    EFI_CAPSULE_HEADER             *BiosGuardCapsuleHeader;

    DEBUG ((EFI_D_INFO, "GetBiosGuardCapsuleInfo Enter\n"));

    ASSERT (Desc != NULL);

    Count = 0;
    Size  = 0;
    Number = 0;
    ThisCapsuleImageSize = 0;

    while (Desc->Union.ContinuationPointer != (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
        if (Desc->Length == 0) {
            //
            // Descriptor points to another list of block descriptors somewhere
            //
            if ( Desc == (EFI_CAPSULE_BLOCK_DESCRIPTOR  *) (UINTN) Desc->Union.ContinuationPointer ||
                    Desc->Union.ContinuationPointer > MAX_ADDRESS ) {
                Count = 0;
                break;
            }

            Desc = (EFI_CAPSULE_BLOCK_DESCRIPTOR  *) (UINTN) Desc->Union.ContinuationPointer;
        } else {
            //
            // Sanity Check
            // It is needed, because ValidateCapsuleIntegrity() only validate one individual capsule Size.
            // While here we need check all capsules size.
            //
            if (Desc->Length >= (MAX_ADDRESS - Size)) {
                DEBUG ((EFI_D_ERROR, "ERROR: Desc->Length(0x%lx) >= (MAX_ADDRESS - Size(0x%x))\n", Desc->Length, Size));
                return EFI_OUT_OF_RESOURCES;
            }
            Size += (UINTN) Desc->Length;
            Count++;

            //
            // See if this is first capsule's header
            //
            if (ThisCapsuleImageSize == 0) {
                BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)Desc->Union.DataBlock);

                //
                // This has been checked in ValidateCapsuleIntegrity()
                //
                Number++;
                ThisCapsuleImageSize = BiosGuardCapsuleHeader->CapsuleImageSize;
            }

            //
            // This has been checked in ValidateCapsuleIntegrity()
            //
            ASSERT (ThisCapsuleImageSize >= Desc->Length);
            ThisCapsuleImageSize = (UINTN)(ThisCapsuleImageSize - Desc->Length);

            //
            // Move to next
            //
            Desc++;
        }
    }
    //
    // If no descriptors, then fail
    //
    if (Count == 0) {
        DEBUG ((EFI_D_ERROR, "ERROR: Count == 0\n"));
        return EFI_NOT_FOUND;
    }

    //
    // checked in ValidateCapsuleIntegrity()
    //
    ASSERT (ThisCapsuleImageSize == 0);

    if (NumDescriptors != NULL) {
        *NumDescriptors = Count;
    }

    if (CapsuleSize != NULL) {
        *CapsuleSize = Size;
    }

    if (CapsuleNumber != NULL) {
        *CapsuleNumber = Number;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
BiosGuardFindCapHdrFFS (
    IN  VOID    *pCapsule,
    OUT UINT8   **pFfsData
)
{
    UINT32                          Signature;
    UINT32                          *SearchPointer;
    AMI_FFS_COMMON_SECTION_HEADER   *FileSection;
    APTIO_FW_CAPSULE_HEADER         *pFwCapHdr;

    SearchPointer = (UINT32 *)((UINT8 *)pCapsule - sizeof(AMI_FFS_COMMON_SECTION_HEADER) + FLASH_SIZE);
    Signature = FwCapFfsGuid.Data1;

    do {
        if ( *SearchPointer == Signature ) {
            FileSection = (AMI_FFS_COMMON_SECTION_HEADER *)SearchPointer;
            if ( CompareGuid(&FwCapFfsGuid, &(FileSection->FfsHdr.Name)) && CompareGuid(&FwCapSectionGuid, &(FileSection->SectionGuid)) ) {
                pFwCapHdr = (APTIO_FW_CAPSULE_HEADER*)(FileSection->FwCapHdr);
                // just a sanity check - Cap Size must match the Section size
                if (((*(UINT32 *)FileSection->FfsHdr.Size) & 0xffffff) >= pFwCapHdr->CapHdr.HeaderSize + sizeof(AMI_FFS_COMMON_SECTION_HEADER) &&
                        CompareGuid((EFI_GUID*)&pFwCapHdr->CapHdr.CapsuleGuid, &FWCapsuleGuid)) {
                    *pFfsData = (UINT8*)pFwCapHdr;
                    return EFI_SUCCESS;
                }
            }
        }
    } while ( SearchPointer-- != pCapsule );

    return EFI_NOT_FOUND;
}

#if defined (BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)
EFI_STATUS
VerifyMeImage(
  IN EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PHYSICAL_ADDRESS   MeImageAddress,
  IN UINT32                 MeImageSize
)
{
    EFI_STATUS  Status;
    UINT32      FailedStage;
    
    Status = VerifyFwImage(PeiServices, (VOID**)&MeImageAddress, (UINT32*)&MeImageSize,(UINT32*)&FailedStage ); 
    
    return Status;
}
#endif

EFI_STATUS
BiosGuardGetCapsuleImage (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
    IN VOID                               *InvokePpi
)
{
    EFI_STATUS                            Status;
    EFI_PEI_HOB_POINTERS                  HobPointer;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI       *ReadOnlyVariable;
    EFI_GUID                              BiosGuardCapsuleGuid = BIOS_GUARD_CAPSULE_GUID;
    EFI_GUID                              BiosGuardCapsuleVariableGuid = BIOS_GUARD_CAPSULE_VARIABLE_GUID;
    EFI_GUID                              gBiosGuardRecoveryImageHobGuid = AMI_BIOSGUARD_RECOVERY_IMAGE_HOB_GUID;
    RECOVERY_IMAGE_HOB                    *BiosGuardRecoveryImageHob;
    APTIO_FW_CAPSULE_HEADER               *FWCapsuleHdr;
    EFI_CAPSULE_HEADER                    *BiosGuardCapsuleHeader;
    BGUP_HEADER                           *BgupHdr;
    AMI_BIOS_GUARD_RECOVERY               ApRecoveryAddress;
    EFI_CAPSULE_BLOCK_DESCRIPTOR          *BiosGuardCapsuleBlockDesc;
    EFI_PHYSICAL_ADDRESS                  SavedBiosGuardImage = 0, BackupSavedBiosGuardImage = 0, SaveCapsuleImage = 0, BackupSaveCapsuleImage = 0, CapsuleAddress = 0;
    UINT8                                 AmiBiosGuardSign[] = "_AMIPFAT";
    UINT8                                 *RecoveryRomAddress;
    UINT32                                AmiBiosGuardHeaderLength = 0, CurrentBGUPAddress = 0;
    UINTN                                 CapsuleSize = 0, CapsuleNumber = 0, NumDescriptors = 0, i = 0, j=0;
    UINTN                                 BiosGuardCapsuleVarSize = sizeof(AMI_BIOS_GUARD_RECOVERY);
    BOOLEAN                               MeUpdate = FALSE;
    UINT32                                ImageSize;
#if defined (BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)
    UINT32                                MeImageSize = 0;
    UINT8                                 *MePayload;
#endif	

    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    Status = ReadOnlyVariable->GetVariable (
                 ReadOnlyVariable,
                 L"BiosGuardCapsuleVariable",
                 &BiosGuardCapsuleVariableGuid,
                 NULL,
                 &BiosGuardCapsuleVarSize,
                 &ApRecoveryAddress
             );

    if ( !EFI_ERROR( Status ) && (ApRecoveryAddress.Signature != 0) ) {
#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((EFI_D_INFO, "ApRecoveryAddress = %X \n", ApRecoveryAddress));
        DEBUG ((EFI_D_INFO, "ApRecoveryAddress.Signature = %lX\n", ApRecoveryAddress.Signature));
        DEBUG ((EFI_D_INFO, "ApRecoveryAddress.Version = %X\n", ApRecoveryAddress.Version));
        DEBUG ((EFI_D_INFO, "ApRecoveryAddress.ReoveryMode = %X\n", ApRecoveryAddress.ReoveryMode));
        DEBUG ((EFI_D_INFO, "ApRecoveryAddress.ReoveryAddress = %X\n", ApRecoveryAddress.ReoveryAddress));
#endif
        if ( (ApRecoveryAddress.ReoveryAddress == 0) || (ApRecoveryAddress.ReoveryAddress == 0xFFFFFFF) ) return EFI_SUCCESS;

        BiosGuardCapsuleBlockDesc = (EFI_CAPSULE_BLOCK_DESCRIPTOR*)ApRecoveryAddress.ReoveryAddress;
        BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)BiosGuardCapsuleBlockDesc->Union.DataBlock);
#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((EFI_D_INFO, "BiosGuardCapsuleHeader->CapsuleGuid : %X \n", BiosGuardCapsuleHeader->CapsuleGuid));
        DEBUG ((EFI_D_INFO, "BiosGuardCapsuleHeader->HeaderSize : %X \n", BiosGuardCapsuleHeader->HeaderSize));
        DEBUG ((EFI_D_INFO, "BiosGuardCapsuleHeader->Flags : %X \n", BiosGuardCapsuleHeader->Flags));
#endif

        if( ApRecoveryAddress.ReoveryMode == BiosGuardRecoveryMode ) {
            if ( !CompareGuid (&BiosGuardRecoveryGuid, &BiosGuardCapsuleHeader->CapsuleGuid) ) {
#if BIOS_GUARD_DEBUG_MODE
                DEBUG ((EFI_D_INFO, "Stop to do BIOS GUARD Recovery, BiosGuardRecoveryGuid is Not the same.\n"));
#endif
                return EFI_SUCCESS;
            }

            PeiServicesSetBootMode(BOOT_IN_RECOVERY_MODE);
            PeiServicesInstallPpi(&RecoveryModePpi);
        } else if( ApRecoveryAddress.ReoveryMode == BiosGuardCapsuleMode ) {
            if ( !CompareGuid (&BiosGuardCapsuleGuid, &BiosGuardCapsuleHeader->CapsuleGuid) ) {
#if BIOS_GUARD_DEBUG_MODE
                DEBUG ((EFI_D_INFO, "Stop to do BIOS GUARD Capsule, BiosGuardCapsuleGuid is Not the same.\n"));
#endif
                return EFI_SUCCESS;
            }

            GetBiosGuardCapsuleInfo(BiosGuardCapsuleBlockDesc, &NumDescriptors, &CapsuleSize, &CapsuleNumber);
#if BIOS_GUARD_DEBUG_MODE
            DEBUG ((EFI_D_INFO, "NumDescriptors = %X\n", NumDescriptors));
            DEBUG ((EFI_D_INFO, "CapsuleSize = %X\n", CapsuleSize));
            DEBUG ((EFI_D_INFO, "CapsuleNumber = %X\n", CapsuleNumber));
#endif

            if ( CapsuleSize != (BIOSGUARD_RECOVERY_IMAGE_SIZE + sizeof(EFI_CAPSULE_HEADER)) ) {
#if BIOS_GUARD_DEBUG_MODE
                DEBUG ((EFI_D_INFO, "CapsuleSize != BIOSGUARD_RECOVERY_IMAGE_SIZE + sizeof(EFI_CAPSULE_HEADER)\n"));
#endif
                return EFI_UNSUPPORTED;
            }

            //
            // Allocate memory for saving BiosGuard image.
            //
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (BIOSGUARD_RECOVERY_IMAGE_SIZE), &SavedBiosGuardImage);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            DEBUG ((EFI_D_INFO, "Saved BiosGuard capsule Image : %lx\n", SavedBiosGuardImage));
            BackupSavedBiosGuardImage = SavedBiosGuardImage;

            for ( i =0; i < NumDescriptors ; i++) {
#if BIOS_GUARD_DEBUG_MODE
                DEBUG ((EFI_D_INFO, "BiosGuardCapsuleBlockDesc[%d]->Length = %X\n", i, BiosGuardCapsuleBlockDesc->Length));
                DEBUG ((EFI_D_INFO, "BiosGuardCapsuleBlockDesc[%d]->Union.ContinuationPointer = %X\n", i, BiosGuardCapsuleBlockDesc->Union.ContinuationPointer));
                DEBUG ((EFI_D_INFO, "BiosGuardCapsuleBlockDesc[%d]->Union.DataBlock = %X\n", i, BiosGuardCapsuleBlockDesc->Union.DataBlock));
#endif
                if ( i != 0 ) {    //Skip EFI_CAPSULE_HEADER
                    CopyMem ((UINT8*)SavedBiosGuardImage, (UINT8*)BiosGuardCapsuleBlockDesc->Union.DataBlock, (UINTN)BiosGuardCapsuleBlockDesc->Length);
                    SavedBiosGuardImage += BiosGuardCapsuleBlockDesc->Length;
                }
                BiosGuardCapsuleBlockDesc++;
            }

            SavedBiosGuardImage = BackupSavedBiosGuardImage;

            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (BIOSGUARD_RECOVERY_IMAGE_SIZE), &SaveCapsuleImage);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }
#if BIOS_GUARD_DEBUG_MODE
            DEBUG ((EFI_D_INFO, "SaveCapsuleImage = %x\n\n", SaveCapsuleImage));
#endif
            BackupSaveCapsuleImage = SaveCapsuleImage;
            RecoveryRomAddress = (UINT8*)SavedBiosGuardImage;

            //
            // Checking the recovery ROM whether is AMI BIOS GUARD ROM or Not.
            //
            if ( CompareMem ( RecoveryRomAddress + 8, AmiBiosGuardSign, 8 ) ) {
                DEBUG ((EFI_D_INFO, "This is Not BIOS Guard ROM.\n"));
                ASSERT_EFI_ERROR (EFI_UNSUPPORTED);
                return EFI_UNSUPPORTED;
            }

            AmiBiosGuardHeaderLength = *(UINT32*)RecoveryRomAddress;
            BgupHdr = (BGUP_HEADER*)(RecoveryRomAddress + AmiBiosGuardHeaderLength);
            do {
                if ( BgupHdr->ScriptSectionSize == 0xFFFFFFFF ) break;

                //
                // Skip ScriptSection and BGUP Header
                //
                AmiBiosGuardHeaderLength += (BgupHdr->ScriptSectionSize + sizeof(BGUP_HEADER));

                //
                // Collect DataSection only to buffer.
                //
                CopyMem ((UINT8*)SaveCapsuleImage + CurrentBGUPAddress, (UINT8*)RecoveryRomAddress + AmiBiosGuardHeaderLength, BgupHdr->DataSectionSize);

#if BIOS_GUARD_DEBUG_MODE
                DEBUG ((EFI_D_INFO, "ROM Address : %X \n", CurrentBGUPAddress));
                DEBUG ((EFI_D_INFO, "ROM Length  : %X \n\n", BgupHdr->DataSectionSize));
#endif
                
                //
                // Pointer to next BGUP block.
                //
                AmiBiosGuardHeaderLength += (BgupHdr->DataSectionSize + BIOSGUARD_PACKAGE_CERT);
                CurrentBGUPAddress += BgupHdr->DataSectionSize;

                BgupHdr = (BGUP_HEADER*)(RecoveryRomAddress + AmiBiosGuardHeaderLength);
            } while ( AmiBiosGuardHeaderLength < BIOSGUARD_RECOVERY_IMAGE_SIZE );

            FWCapsuleHdr = (APTIO_FW_CAPSULE_HEADER*)(SaveCapsuleImage + (CurrentBGUPAddress - FLASH_SIZE));
            if ( EFI_ERROR(BiosGuardFindCapHdrFFS((VOID*)(SaveCapsuleImage + (CurrentBGUPAddress - FLASH_SIZE)), (UINT8**)&FWCapsuleHdr)) ) {
                ASSERT_EFI_ERROR (EFI_SECURITY_VIOLATION);
                return EFI_SECURITY_VIOLATION;
            }

            //
            // Skip ME Region if needed (CurrentBGUPAddress = Original image size).
            //
            CopyMem ((UINT8*)SaveCapsuleImage, FWCapsuleHdr, FWCAPSULE_MAX_HDR_SIZE);
            CopyMem ((UINT8*)(SaveCapsuleImage + FWCAPSULE_MAX_HDR_SIZE), (UINT8*)SaveCapsuleImage + (CurrentBGUPAddress - FLASH_SIZE), FLASH_SIZE );

            BuildCvHob (SaveCapsuleImage, FLASH_SIZE + FWCAPSULE_MAX_HDR_SIZE);
#if BIOS_GUARD_DEBUG_MODE
            DEBUG ((EFI_D_INFO, "APTIO_FW_CAPSULE_HEADER\n\n"));
            for ( i = 0; i < FWCAPSULE_MAX_HDR_SIZE/16; i++ ) {
                for ( j = 0; j < 16; j++ ) {
                    DEBUG ((EFI_D_INFO, "%02X ", *(UINT8*)SaveCapsuleImage++));
                }
                DEBUG ((EFI_D_INFO, "\n"));
            }

            DEBUG ((EFI_D_INFO, "\nBIOS Data\n"));
            for ( i = 0; i < 16; i++ ) {
                for ( j = 0; j < 16; j++ ) {
                    DEBUG ((EFI_D_INFO, "%02X ", *(UINT8*)SaveCapsuleImage++));
                }
                DEBUG ((EFI_D_INFO, "\n"));
            }

            DEBUG ((EFI_D_INFO, "\nEnd of BIOS\n"));

            SaveCapsuleImage = BackupSaveCapsuleImage;
            SaveCapsuleImage += FWCAPSULE_MAX_HDR_SIZE;
            SaveCapsuleImage += FLASH_SIZE;
            SaveCapsuleImage -= 0x100;

            for ( i = 0; i < 16; i++ ) {
                for ( j = 0; j < 16; j++ ) {
                    DEBUG ((EFI_D_INFO, "%02X ", *(UINT8*)SaveCapsuleImage++));
                }
                DEBUG ((EFI_D_INFO, "\n"));
            }
#endif
            //
            // Save BiosGuard image to HOB.
            //
            Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, sizeof (RECOVERY_IMAGE_HOB), (VOID **) &BiosGuardRecoveryImageHob);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }

            //
            // Create BiosGuard recovery Image HOB for DXE used.
            //
            BiosGuardRecoveryImageHob->Header.Name = gBiosGuardRecoveryImageHobGuid;
            BiosGuardRecoveryImageHob->Address = SavedBiosGuardImage;

            PeiServicesSetBootMode(BOOT_ON_FLASH_UPDATE);
            PeiServicesInstallPpi(&FlashUpdateModePpi);
        }
    }

    //
    // Windows firmware update for BIOS Guard
    //
    HobPointer.Raw = GetHobList ();
    while ( (HobPointer.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, HobPointer.Raw)) != NULL ) {
        BiosGuardCapsuleHeader = (EFI_CAPSULE_HEADER*)((UINTN)HobPointer.Capsule->BaseAddress);
        CapsuleAddress = (EFI_PHYSICAL_ADDRESS)BiosGuardCapsuleHeader;
        RecoveryRomAddress = (UINT8*)(CapsuleAddress + (EFI_PHYSICAL_ADDRESS)BiosGuardCapsuleHeader->HeaderSize);

        //
        // Checking the recovery ROM whether is AMI BIOS GUARD ROM or Not.
        //
        if ( CompareMem ( RecoveryRomAddress + 8, AmiBiosGuardSign, 8 ) ) {
            DEBUG ((EFI_D_INFO, "This is Not BIOS Guard ROM.\n"));
            HobPointer.Raw = GET_NEXT_HOB (HobPointer);
            continue;
        }

        ImageSize = BiosGuardCapsuleHeader->CapsuleImageSize - BiosGuardCapsuleHeader->HeaderSize;
        DEBUG ((EFI_D_INFO, "CapsuleImageSize = %x\n", ImageSize));
        
        if (ImageSize == BIOSGUARD_RECOVERY_IMAGE_SIZE)
        {
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (BIOSGUARD_RECOVERY_IMAGE_SIZE), &SaveCapsuleImage);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }
#if defined (BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)
        } else {
            DEBUG ((EFI_D_INFO, "BiosGuard Binary + MeFwSign.cap\n"));
            // BiosGuard Binary + MeFwSign.cap
            
            Status = VerifyMeImage(PeiServices, (EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + BIOSGUARD_MEUD_BIOS_SIZE, ImageSize - BIOSGUARD_MEUD_BIOS_SIZE);
            if (EFI_ERROR(Status)) {
                DEBUG ((EFI_D_INFO, "Verify ME Image fail(%r)\n", Status));
                return Status;
            }
            
            MeUpdate = TRUE;
            Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (BIOSGUARD_MEUD_BIOS_SIZE + FV_MEFW_CAPSULE_SIZE), &SaveCapsuleImage);
            if ( EFI_ERROR( Status ) ) {
                ASSERT_EFI_ERROR (Status);
                return Status;
            }            
            
            MePayload = (UINT8 *)((EFI_PHYSICAL_ADDRESS)RecoveryRomAddress + BIOSGUARD_MEUD_BIOS_SIZE + ((APTIO_FW_CAPSULE_HEADER *)(RecoveryRomAddress + BIOSGUARD_MEUD_BIOS_SIZE))->RomImageOffset);
            
            MeImageSize = ImageSize - BIOSGUARD_MEUD_BIOS_SIZE;
            DEBUG ((EFI_D_INFO, "MeImageSize = %x\n", MeImageSize));
            ImageSize = BIOSGUARD_MEUD_BIOS_SIZE;
        }
#else            
        } else {
            DEBUG ((EFI_D_INFO, "BiosGuard check size fail.\n"));
            return EFI_UNSUPPORTED;
        }
#endif        

        Status = PeiServicesAllocatePages (EfiBootServicesData, EFI_SIZE_TO_PAGES (BiosGuardCapsuleHeader->CapsuleImageSize - BiosGuardCapsuleHeader->HeaderSize), &SavedBiosGuardImage);
        if ( EFI_ERROR( Status ) ) {
            ASSERT_EFI_ERROR (Status);
            return Status;
        }

        CopyMem ((UINT8*)SavedBiosGuardImage, (UINT8*)RecoveryRomAddress, BiosGuardCapsuleHeader->CapsuleImageSize - BiosGuardCapsuleHeader->HeaderSize);

#if BIOS_GUARD_DEBUG_MODE
        DEBUG ((EFI_D_INFO, "SaveCapsuleImage = %x\n\n", SaveCapsuleImage));
#endif

        AmiBiosGuardHeaderLength = *(UINT32*)RecoveryRomAddress;
        BgupHdr = (BGUP_HEADER*)(RecoveryRomAddress + AmiBiosGuardHeaderLength);
        do {
            if ( BgupHdr->ScriptSectionSize == 0xFFFFFFFF ) break;

            //
            // Skip ScriptSection and BGUP Header
            //
            AmiBiosGuardHeaderLength += (BgupHdr->ScriptSectionSize + sizeof(BGUP_HEADER));

            //
            // Collect DataSection only to buffer.
            //
            CopyMem ((UINT8*)SaveCapsuleImage + CurrentBGUPAddress, (UINT8*)RecoveryRomAddress + AmiBiosGuardHeaderLength, BgupHdr->DataSectionSize);

            //
            // Pointer to next BGUP block.
            //
            AmiBiosGuardHeaderLength += (BgupHdr->DataSectionSize + BIOSGUARD_PACKAGE_CERT);
            CurrentBGUPAddress += BgupHdr->DataSectionSize;
#if BIOS_GUARD_DEBUG_MODE
            DEBUG ((EFI_D_INFO, "ROM Address : %X \n", CurrentBGUPAddress));
            DEBUG ((EFI_D_INFO, "ROM Length  : %X \n\n", BgupHdr->DataSectionSize));
#endif
            BgupHdr = (BGUP_HEADER*)(RecoveryRomAddress + AmiBiosGuardHeaderLength);
        } while ( AmiBiosGuardHeaderLength < ImageSize );

        if (MeUpdate == FALSE) {
            //
            // Skip ME Region if needed (CurrentBGUPAddress = Original image size).
            //
            CopyMem ((UINT8*)RecoveryRomAddress, (UINT8*)SaveCapsuleImage + (CurrentBGUPAddress - FLASH_SIZE), FLASH_SIZE );
            HobPointer.Capsule->Length = (UINT64)BiosGuardCapsuleHeader->HeaderSize + FLASH_SIZE;            
        } 
        
#if defined(BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)
        if (MeUpdate == TRUE) {
            // Copy ME capsule image to the end of SaveCapsuleImage
            CopyMem ((UINT8*)SaveCapsuleImage + CurrentBGUPAddress, MePayload, FV_MEFW_CAPSULE_SIZE);
            //
            // Skip ME Region if needed (CurrentBGUPAddress = Original image size).
            //
            CopyMem ((UINT8*)RecoveryRomAddress, (UINT8*)SaveCapsuleImage + (CurrentBGUPAddress - FLASH_SIZE), FLASH_SIZE );
            CopyMem ((UINT8*)(RecoveryRomAddress + FLASH_SIZE), (UINT8*)SaveCapsuleImage + CurrentBGUPAddress, FV_MEFW_CAPSULE_SIZE );
            HobPointer.Capsule->Length = (UINT64)BiosGuardCapsuleHeader->HeaderSize + FLASH_SIZE + FV_MEFW_CAPSULE_SIZE;            
        }
#endif        

#if BIOS_GUARD_DEBUG_MODE
        SaveCapsuleImage = HobPointer.Capsule->BaseAddress;

        DEBUG ((EFI_D_INFO, "APTIO_FW_CAPSULE_HEADER\n\n"));
        for ( i = 0; i < FWCAPSULE_MAX_HDR_SIZE/16; i++ ) {
            for ( j = 0; j < 16; j++ ) {
                DEBUG ((EFI_D_INFO, "%02X ", *(UINT8*)SaveCapsuleImage++));
            }
            DEBUG ((EFI_D_INFO, "\n"));
        }

        DEBUG ((EFI_D_INFO, "\nBIOS Data\n"));
        for ( i = 0; i < 16; i++ ) {
            for ( j = 0; j < 16; j++ ) {
                DEBUG ((EFI_D_INFO, "%02X ", *(UINT8*)SaveCapsuleImage++));
            }
            DEBUG ((EFI_D_INFO, "\n"));
        }
#endif
        //
        // Save BiosGuard image to HOB.
        //
        Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, sizeof (RECOVERY_IMAGE_HOB), (VOID **) &BiosGuardRecoveryImageHob);
        if ( EFI_ERROR( Status ) ) {
            ASSERT_EFI_ERROR (Status);
            return Status;
        }

        //
        // Create BiosGuard recovery Image HOB for DXE used.
        //
        BiosGuardRecoveryImageHob->Header.Name = gBiosGuardRecoveryImageHobGuid;
        BiosGuardRecoveryImageHob->Address = SavedBiosGuardImage;
        if (MeUpdate == FALSE) {
            BiosGuardRecoveryImageHob->ImageSize = ImageSize;
        }
#if defined(BIOS_GUARD_MEUD_SUPPORT) && (BIOS_GUARD_MEUD_SUPPORT == 1)        
        else {
            BiosGuardRecoveryImageHob->ImageSize = ImageSize + FV_MEFW_CAPSULE_SIZE;
        }
#endif        
        break;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BiosGuardPeiApRecoveryCapsuleEntry (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS                       Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
    SYSTEM_CONFIGURATION             SetupData;
    UINTN                            VariableSize= sizeof (SYSTEM_CONFIGURATION);

    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    Status = ReadOnlyVariable->GetVariable (
                 ReadOnlyVariable,
                 L"IntelSetup",
                 &gEfiSetupVariableGuid,
                 NULL,
                 &VariableSize,
                 &SetupData
             );

    if ( EFI_ERROR( Status ) ) {
        return Status;
    }

    if ( SetupData.BiosGuardEnabled == Enable ) {
        PeiServicesNotifyPpi(BiosGuardGetCapsuleImageNotifyList);
        PeiServicesNotifyPpi(BiosGuardCapsuleChangeBootModeAfterEndofMrcNotifyList);
    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
