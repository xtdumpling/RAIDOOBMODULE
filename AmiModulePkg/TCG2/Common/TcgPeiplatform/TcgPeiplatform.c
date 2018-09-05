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
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPeiPlatform/TcgPeiplatform.c 1     10/08/13 12:04p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:04p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPeiPlatform/TcgPeiplatform.c $
//
// 1     10/08/13 12:04p Fredericko
// Initial Check-In for Tpm-Next module
//
// 1     7/10/13 5:56p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 5     4/26/11 1:46p Fredericko
// Function call changes to match changes to function header definition
// for Memoryabsent function.
//
// 4     4/04/11 2:14p Fredericko
// Removed #pragma optimization directives
//
// 3     3/29/11 9:18p Fredericko
// Handle TPM memory present errors
//
// 2     3/29/11 1:19p Fredericko
// [TAG]        EIP 54642
// [Category] Improvement
// [Description] 1. Checkin Files related to TCG function override
// 2. Include TCM and TPM auto detection
// [Files] Affects all TCG files
//
//
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    TcgPeiPlatform.c
//
// Description: Function file for TcgPeiPlatform
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Efi.h>
#include <Pei.h>
#include <AmiTcg/TcgCommon12.h>
#include <AmiTcg/TCGMisc.h>
#include <Ppi/TcgTcmService.h>
#include <Ppi/TcgService.h>
#include <Ppi/TpmDevice.h>
#include "Ppi/CpuIo.h"
#include "Ppi/LoadFile.h"
#include <Ppi/ReadOnlyVariable.h>
#include <AmiTcg/AmiTcgPlatformPei.h>
#include <Library/DebugLib.h>

extern EFI_GUID gAmiTcgPlatformPpiAfterMem;
extern EFI_GUID gAmiTcgPlatformPpiBeforeMem;

EFI_STATUS
EFIAPI OnMemoryDiscovered(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi );


static EFI_PEI_NOTIFY_DESCRIPTOR TcgAmiPlatformInitNotify[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
        EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAmiTcgPlatformPpiAfterMem,
        OnMemoryDiscovered
    }
};

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   OnMemoryDiscovered
//
// Description: Call Memory Present initialization on memory Installation
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//              IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
//              IN      VOID                      *Ppi
//
// Output:      EFI STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI OnMemoryDiscovered(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi )
{
    EFI_STATUS                 Status;
    AMI_TCG_PLATFORM_PPI_AFTER_MEM    *AmiTcgPlatformPPI = NULL;
    EFI_BOOT_MODE              BootMode;


    Status =  (*PeiServices)->LocatePpi (
                  PeiServices,
                  &gAmiTcgPlatformPpiAfterMem ,
                  0,
                  NULL,
                  &AmiTcgPlatformPPI);

    DEBUG((DEBUG_INFO, "Locate PpiAfterMem Status = %r \n", Status));

    if ( EFI_ERROR( Status ))
    {
        Status = (*PeiServices)->NotifyPpi (PeiServices, \
                                            TcgAmiPlatformInitNotify);

        return Status;
    }

    ASSERT_EFI_ERROR( Status );

    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
    ASSERT_EFI_ERROR( Status );

    Status = AmiTcgPlatformPPI->VerifyTcgVariables(PeiServices);
    Status = AmiTcgPlatformPPI->MemoryPresentFunctioOverride(PeiServices);
    if(EFI_ERROR(Status))return Status;

    if((BootMode == BOOT_ON_S3_RESUME) || (BootMode == BOOT_IN_RECOVERY_MODE))
    {
        return EFI_SUCCESS;
    }

    Status = AmiTcgPlatformPPI->SetPhysicalPresence(PeiServices);
    return (Status);
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgPeiPlatformEntry
//
// Description:
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
EFIAPI TcgPeiplatformEntry(
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST  EFI_PEI_SERVICES    **PeiServices )
{

    AMI_TCG_PLATFORM_PPI_BEFORE_MEM     *AmiTcgPlatformPPI = NULL;
    EFI_STATUS                 Status;
    TCG_PEI_MEMORY_CALLBACK    *MemCallback;

    Status =  (*PeiServices)->LocatePpi (
                  PeiServices,
                  &gAmiTcgPlatformPpiBeforeMem,
                  0,
                  NULL,
                  &AmiTcgPlatformPPI);


    if(EFI_ERROR(Status))
    {
        return EFI_SUCCESS;
    }

    AmiTcgPlatformPPI->MemoryAbsentFunctionOverride( (EFI_PEI_SERVICES    **)PeiServices);

    Status = (**PeiServices).AllocatePool(
                 PeiServices,
                 sizeof (TCG_PEI_MEMORY_CALLBACK),
                 &MemCallback);

    if ( !EFI_ERROR( Status ))
    {
        MemCallback->NotifyDesc.Flags
            = (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
               | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
        MemCallback->NotifyDesc.Guid   = &gEfiPeiMemoryDiscoveredPpiGuid;
        MemCallback->NotifyDesc.Notify = OnMemoryDiscovered;
        MemCallback->FfsHeader         = FileHandle;

        Status = (*PeiServices)->NotifyPpi( PeiServices,
                                            &MemCallback->NotifyDesc );
    }

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
//**********************************************************************
