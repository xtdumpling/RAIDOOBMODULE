//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformPeiBeforeMem.c 1     10/08/13 12:04p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:04p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformPeiBeforeMem.c $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    AmiTcgPlatformPeiBeforeMem.c
//
// Description: Function file for AmiTcgPlatformPeiBeforeMem
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Efi.h>
#include <AmiTcg/TcgCommon12.h>
#include "AmiTcg/sha.h"
#include <AmiTcg/TCGMisc.h>
#include <AmiTcg/TpmLib.h>
#include <Token.h>
#include "Ppi/TcgService.h"
#include "Ppi/TpmDevice.h"
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/AmiTcgPlatformPpi.h>
#include <AmiTcg/AmiTcgPlatformPei.h>
#include <AmiTcg/AmiTpmStatusCodes.h>
#include <Guid/AmiTcgGuidIncludes.h>

extern EFI_GUID gAmiTcgPlatformPpiBeforeMem;


EFI_STATUS 
EFIAPI
TpmPeiReportStatusCode(  IN EFI_STATUS_CODE_TYPE   Type,
                         IN EFI_STATUS_CODE_VALUE  Value);

static EFI_PEI_PPI_DESCRIPTOR Tpm_Initialized[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gPeiTpmInitializedPpiGuid, NULL
    }
};


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MemoryAbsentEntry
//
// Description: This function performs TPM MA initialization
//
//
// Input:       IN      EFI_FFS_FILE_HEADER       *FfsHeader
//              IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
MemoryAbsentEntry(
    IN EFI_PEI_SERVICES    **PeiServices )
{
    EFI_STATUS              Status;
    EFI_BOOT_MODE           BootMode;
    void                    *TcgDrvBuffer = NULL;
    UINTN                   Pages         = 0;
    EFI_HOB_GUID_TYPE       *MAHobType;
    MASTRUCT                MAHob;
    FAR32LOCALS             CommonLegX;
    AMI_TCG_PEI_FUNCTION_OVERRIDE_PPI       *PpiOverride;
    BOOLEAN                 SkipTpmStartup = FALSE;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gSkipTpmStartupGuid,
                 0, NULL,
                 &PpiOverride);

    if(!EFI_ERROR(Status))
    {
        SkipTpmStartup = TRUE;
        TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_STARTUP_CMD_SKIP | EFI_SOFTWARE_PEI_MODULE);   
    }

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &AmiMemoryAbsentOverrideGuid,
                 0, NULL,
                 &PpiOverride);

    if(!EFI_ERROR(Status))
    {
        return (PpiOverride->Function(PeiServices));
    }

    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
    ASSERT_EFI_ERROR(  Status );

#if (StartupCmd_SelfTest_State == 1)
    if(!IsTcmSupportType())
    {
        if (!SkipTpmStartup)
        {
            Status = TcgPeiTpmStartup( PeiServices, BootMode );
            DEBUG((DEBUG_INFO, "TcgPeiTpmStartup Status = %r \n", Status));
            if(EFI_ERROR(Status)){
                TpmPeiReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_1_2_STARTUP_ERROR | EFI_SOFTWARE_PEI_MODULE);   
                return Status;
            }
            TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_STARTUP_CMD_SENT | EFI_SOFTWARE_PEI_MODULE);
        }

        DEBUG((DEBUG_INFO, "Continue SelfTest = %r \n", Status));
        Status = ContinueTPMSelfTest( PeiServices );
        if(!EFI_ERROR(Status)){
            TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_SELFTEST_CMD_SENT | EFI_SOFTWARE_PEI_MODULE);   
        }

        if(!EFI_ERROR(Status))
        {
            (*PeiServices)->InstallPpi(PeiServices, Tpm_Initialized);
        }

        if ((BootMode == BOOT_ON_S3_RESUME) || (BootMode == BOOT_IN_RECOVERY_MODE))
        {
            return Status;
        }
    }
#else
    if ((BootMode == BOOT_ON_S3_RESUME) || (BootMode == BOOT_IN_RECOVERY_MODE))
    {
        return Status;
    }
#endif

    if(IsTcmSupportType())
    {
        Status = FillDriverLocByFile(&CommonLegX.Offset,PeiServices,&gEfiTcgMADriverGuid,&TcgDrvBuffer,&Pages);
        if(EFI_ERROR(Status))return EFI_NOT_FOUND;
        if ( CommonLegX.Offset == 0 )
        {
            return EFI_NOT_FOUND;
        }

        MAHob.Offset   = CommonLegX.Offset;
        MAHob.Selector = SEL_flatCS;
        MAHob.Codep    = ((MPDRIVER_LEGHEADER*)((UINT8*)(TcgDrvBuffer)))->CodeP;

        Status = TcgPeiBuildHobGuid(
                     PeiServices,
                     &gEfiTcgMADriverHobGuid,
                     sizeof (MASTRUCT),
                     &MAHobType );

        ASSERT_EFI_ERROR(  Status );
        MAHobType++;

        (*PeiServices)->CopyMem( MAHobType, &MAHob, sizeof (MASTRUCT));
        return Status;
    }

    return EFI_SUCCESS;
}




static AMI_TCG_PLATFORM_PPI_BEFORE_MEM  mAmiTcgPlatformPPI =
{
    MemoryAbsentEntry,
};


static EFI_PEI_PPI_DESCRIPTOR mAmiTcgPlatformPPIListBeforeMem[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        & gAmiTcgPlatformPpiBeforeMem ,
        &mAmiTcgPlatformPPI
    }
};




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   AmiTcgPlatformPEI_EntryBeforeMem
//
// Description: Installs AMIplatform PPI for initialization in PEI before
//              memory is installed
//
// Input:        IN EFI_FFS_FILE_HEADER *FfsHeader,
//               IN EFI_PEI_SERVICES    **PeiServices
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI AmiTcgPlatformPEI_EntryBeforeMem(
    IN EFI_PEI_FILE_HANDLE FfsHeader,
    IN CONST EFI_PEI_SERVICES    **PeiServices
)
{
    EFI_STATUS Status;

    Status = (*PeiServices)->InstallPpi( PeiServices, mAmiTcgPlatformPPIListBeforeMem );
    return Status;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
