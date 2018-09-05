//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file BootGuardPei.c
    Chain of trust for Pei

**/
//*************************************************************************

#include "BootGuardPei.h"
#include <CpuRegs.h>

EFI_GUID ReserveBootGuardFvMainHashKeyGuid = RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY_FFS_FILE_RAW_GUID;

#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT == 1)
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  BootGuardVerificationForPeiToDxeHandoffEndOfPeiNotifyDesc = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    BootGuardVerificationForPeiToDxeHandoffEndOfPei
};
#else
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  BootGuardVerificationForPeiToDxeHandoffEndOfPeiNotifyDesc = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
#ifdef ECP_FLAG
    &gEfiPeiEndOfPeiPhasePpiGuid,
#else
    &gEfiEndOfPeiSignalPpiGuid,
#endif
    BootGuardVerificationForPeiToDxeHandoffEndOfPei
};
#endif

CONST EFI_PEI_SERVICES  **ppPS;

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
IsBootGuardSupported (
  VOID
  )
{
  UINT64          BootGuardBootStatus;
  UINT32          BootGuardAcmStatus;
  UINT64          BootGuardCapability;

  BootGuardBootStatus = *(UINT64 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
  BootGuardAcmStatus  = *(UINT32 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
  BootGuardCapability = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_CAPABILITY;

  if (BootGuardCapability != 0) {
    DEBUG ((DEBUG_INFO, "Processor supports Boot Guard.\n"));
    return TRUE;
  } else {
    DEBUG ((DEBUG_ERROR, "Processor does not support Boot Guard.\n"));
    return FALSE;
  }
}

//**********************************************************************
/**
    Loads binary from RAW section of X firwmare volume


    @retval Buffer returns a pointer to allocated memory. Caller
        must free it when done.
    @retval Size returns the size of the binary loaded into the
        buffer.

**/
//**********************************************************************
EFI_STATUS
LocateBootGuardFvMainHashKey (
    IN EFI_PEI_SERVICES  **PpSv,
    IN OUT VOID          **Buffer
)
{
    EFI_STATUS                    Status;
    EFI_FIRMWARE_VOLUME_HEADER    *pFV;
    UINTN                         FvNum=0;
    EFI_FFS_FILE_HEADER           *ppFile=NULL;
    BOOLEAN                       Found = FALSE;

    Status = (*PpSv)->FfsFindNextVolume (PpSv, FvNum, &pFV);

    while ( TRUE ) {
        Status = (*PpSv)->FfsFindNextVolume( PpSv, FvNum, &pFV );
        if ( EFI_ERROR( Status ) ) {
            return Status;
        }

        ppFile = NULL;

        while ( TRUE ) {
            Status = (*PpSv)->FfsFindNextFile( PpSv,
                                               EFI_FV_FILETYPE_FREEFORM,
                                               pFV,
                                               &ppFile );

            if ( Status == EFI_NOT_FOUND ) {
                break;
            }

            if (CompareGuid( &ppFile->Name, &ReserveBootGuardFvMainHashKeyGuid )) {
                Found = TRUE;
                break;
            }
        }

        if ( Found ) {
            break;
        } else {
            FvNum++;
        }
    }

    Status = (*PpSv)->FfsFindSectionData( PpSv,
                                          EFI_SECTION_RAW,
                                          ppFile,
                                          Buffer );

    if ( EFI_ERROR( Status ) ) {
        return EFI_NOT_FOUND;
    }

    return Status;
}

//**********************************************************************
/**
    BootGuardVerificationForPeiToDxeHandoffEndOfPei at end of Pei
    handler.

    @retval PeiServices Pointer to PEI Services Table.
    @retval NotifyDesc Pointer to the descriptor for the Notification
        event that caused this function to execute.
    @retval Ppi Pointer to the PPI data associated with
        this function.

**/
//**********************************************************************
STATIC
EFI_STATUS
BootGuardVerificationForPeiToDxeHandoffEndOfPei (
    IN      EFI_PEI_SERVICES          **PeiServices,
    IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN      VOID                      *Ppi
)
{
    EFI_STATUS                             Status;
    UINTN                                  BootGuardHashDataSize = 0, i;
    UINT8                                  CurrentBootGuardFvMainHash256Val[32];
    UINT8                                  CurrentBootGuardFvMainHash256Val2[32];    
    VOID                                   *BootGuardSha256Context;
    UINT8                                  *BootGuardOrgFvMainHash256;
    AMI_BOOT_GUARD_HOB                     *AmiBootGuardHobPtr;
    EFI_GUID                               AmiBootGuardHobGuid = AMI_BOOT_GUARD_HOB_GUID;
    EFI_BOOT_MODE                          BootMode;
    RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY    *ReserveBootGuardFvMainHashKey;

    Status = PeiServicesGetBootMode (&BootMode);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] Get Boot Mode is fail\n"));
        return   Status;
    }

    if ( (BootMode == BOOT_IN_RECOVERY_MODE) || (BootMode == BOOT_ON_FLASH_UPDATE) ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] In the BOOT_IN_RECOVERY_MODE or BOOT_ON_FLASH_UPDATE\n"));
        return   Status;
    }

    if ( BootMode == BOOT_ON_S3_RESUME ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] In the BOOT_ON_S3_RESUME\n"));
        return   Status;
    }

    Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof (AMI_BOOT_GUARD_HOB), (VOID **) &AmiBootGuardHobPtr);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] CreateHob is fail for AmiBootGuardHobPtr\n"));
        return   Status;
    }

    AmiBootGuardHobPtr->EfiHobGuidType.Name = AmiBootGuardHobGuid;
    AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag = 0;
    BootGuardHashDataSize = Sha256GetContextSize ();
    Status = ((*PeiServices)->AllocatePool) (PeiServices, BootGuardHashDataSize, &BootGuardSha256Context);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] AllocatePool is fail for BootGuardSha256Context\n"));
        return   Status;
    }

    BootGuardOrgFvMainHash256 = AllocateZeroPool (sizeof(RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY));
    if (BootGuardOrgFvMainHash256 == NULL) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] AllocateZeroPool is fail for RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY\n"));
        return Status;
    }

    Status  = LocateBootGuardFvMainHashKey(PeiServices , &BootGuardOrgFvMainHash256);
    if ( EFI_ERROR( Status ) ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] LocateBootGuardFvMainHashKey is fail\n"));
        return   Status;
    }

    ReserveBootGuardFvMainHashKey = (RESERVE_BOOT_GUARD_FV_MAIN_HASH_KEY*)BootGuardOrgFvMainHash256;

    for ( i = 0; i < sizeof(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey0); i++ ) {
        DEBUG ((EFI_D_INFO, "[BootGuardPei.c] BootGuardFvMainHashKey0[%x]= %x.\n", i, ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey0[i]));
    }
    DEBUG ((EFI_D_INFO, "\n[BootGuardPei.c] BootGuardFvMainSegmentBase0= %x.\n", ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase0));
    DEBUG ((EFI_D_INFO, "\n[BootGuardPei.c] BootGuardFvMainSegmentSize0= %x.\n", ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize0));

    Sha256Init (BootGuardSha256Context);
    Sha256Update (BootGuardSha256Context, (UINT8 *)(ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase0), (UINTN)ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize0);
    Sha256Final (BootGuardSha256Context,  CurrentBootGuardFvMainHash256Val);

    for ( i = 0; i < sizeof (CurrentBootGuardFvMainHash256Val); i++ ) {
        DEBUG ((EFI_D_INFO, "[BootGuardPei.c] CurrentBootGuardFvMainHash256Val[%x]= %x.\n", i, CurrentBootGuardFvMainHash256Val[i]));
    }

    if ( !CompareMem(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey0, CurrentBootGuardFvMainHash256Val, 32) ) {
        AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag = 1;
    } else {
        AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag = 0;
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag= %x.\n", AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag));        
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT == 1)
        PeiServicesSetBootMode(BOOT_IN_RECOVERY_MODE);                                
        PeiServicesInstallPpi(&RecoveryModePpi);
#endif        
        return   EFI_SUCCESS;
    }
    
    if ( !((ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase1 == 0) || (ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize1 == 0)) ) {
        for ( i = 0; i < sizeof(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey1); i++ ) {
            DEBUG ((EFI_D_INFO, "[BootGuardPei.c] BootGuardFvMainHashKey1[%x]= %x.\n", i, ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey1[i]));
        }
        DEBUG ((EFI_D_INFO, "\n[BootGuardPei.c] BootGuardFvMainSegmentBase1= %x.\n", ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase1));
        DEBUG ((EFI_D_INFO, "\n[BootGuardPei.c] BootGuardFvMainSegmentSize1= %x.\n", ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize1));

        Sha256Init (BootGuardSha256Context);
        Sha256Update (BootGuardSha256Context, (UINT8 *)(ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentBase1), (UINTN)ReserveBootGuardFvMainHashKey->BootGuardFvMainSegmentSize1);
        Sha256Final (BootGuardSha256Context,  CurrentBootGuardFvMainHash256Val2);

        for ( i = 0; i < sizeof (CurrentBootGuardFvMainHash256Val2); i++ ) {
            DEBUG ((EFI_D_INFO, "[BootGuardPei.c] CurrentBootGuardFvMainHash256Val2[%x]= %x.\n", i, CurrentBootGuardFvMainHash256Val2[i]));
        }

        if ( !CompareMem(ReserveBootGuardFvMainHashKey->BootGuardFvMainHashKey1, CurrentBootGuardFvMainHash256Val2, 32) ) {
            AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag = 1;
        } else {
            AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag = 0;
            DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag= %x.\n", AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag));        
#if (defined INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT) && (INTEL_BOOT_GUARD_CHAIN_OF_TRUST_RECOVERY_SUPPORT == 1)
        PeiServicesSetBootMode(BOOT_IN_RECOVERY_MODE);                                
        PeiServicesInstallPpi(&RecoveryModePpi);
#endif  
            return   EFI_SUCCESS;        
        }    
    }
    DEBUG ((EFI_D_INFO, "[BootGuardPei.c] AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag= %x.\n", AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag));

    return   Status;
}

#ifdef ECP_FLAG
EFI_STATUS
BootGuardPeiEntryPoint (
  IN  EFI_FFS_FILE_HEADER  *FfsHeader,
  IN  EFI_PEI_SERVICES     **PeiServices
  )
#else
EFI_STATUS
BootGuardPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
#endif
{
    EFI_STATUS                      Status;
    ppPS = PeiServices; 
    
    if ( IsBootGuardSupported() == FALSE ) {
        return   EFI_SUCCESS;
    }

    if ( (UINT32)AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) == 0 ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardPei.c] Boot Guard is disabled by Boot Guard Profile Configuration in the Intel Fitc\n"));
        return   EFI_SUCCESS;
    }

    Status = PeiServicesNotifyPpi (&BootGuardVerificationForPeiToDxeHandoffEndOfPeiNotifyDesc);
    if ( EFI_ERROR( Status ) ) {
        return   Status;
    }

    return   Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
