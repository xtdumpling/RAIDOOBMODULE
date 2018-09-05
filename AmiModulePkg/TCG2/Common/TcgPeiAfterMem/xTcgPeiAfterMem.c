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
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPei/xTcgPeiAfterMem.c 1     10/08/13 12:02p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:02p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPei/xTcgPeiAfterMem.c $
//
// 1     10/08/13 12:02p Fredericko
// Initial Check-In for Tpm-Next module
//
// 1     7/10/13 5:51p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 4     12/12/11 3:31p Fredericko
// [TAG]        EIP76865
// [Category]   Improvement
// [Description]    Dual Support for TCM and TPM. System could hang in TXT
// if txt is enabled in setup
// [Files]          AmiTcgPlatfompeilib.c, AmiTcgPlatformPpi.cif,
// AmiTcgPlatformPpi.h, AmiTcgPlatformProtocol.cif,
// AmiTcgPlatformProtocol.h,
// EMpTcmPei.c, TcgDxe.cif, TcgPei.cif, TcgPeiAfterMem.cif,
// TcgPeiAfterMem.mak, TcgTcmPeiAfterMem.c, xTcgDxe.c, xTcgPei.c,
// xTcgPeiAfterMem.c
//
// 3     9/27/11 10:22p Fredericko
// [TAG]        EIP67286
// [Category]   Improvement
// [Description]    changes for Tcg Setup policy
//
// [Files]          Tcg.sdl
// TcgPei.cif
// TcgPei.mak
// xtcgPei.c
// xTcgPeiAfterMem.c
// TcgPeiAfterMem.mak
// TcgDxe.cif
// TcgDxe.mak
// xTcgDxe.c
// AmiTcgPlatformPeilib.c
// AmiTcgPlatformDxelib.c
//
// 2     9/03/11 8:04p Fredericko
//
// 1     8/22/11 1:45p Fredericko
// [TAG]        EIP61168
// [Category]   Improvement
// [Description]    Reload TCG PPI from memory when Memory is Installed
// [Files]          TcgPeiAfterMem.cif
// TcgPeiAfterMem.c
// xTcgPeiAfterMem.c
// TcgPeiAfterMem.mak
//
// 32    4/27/11 3:01p Fredericko
// removed VFR compile directive
//
// 31    3/29/11 12:57p Fredericko
//
// 30    3/28/11 2:14p Fredericko
// [TAG]        EIP 54642
// [Category] Improvement
// [Description] 1. Checkin Files related to TCG function override
// 2. Include TCM and TPM auto detection
// [Files] Affects all TCG files
//
// 29    5/19/10 5:53p Fredericko
// Included File Header
// Included File Revision History
// Updated AMI Function Headers
// Code Beautification
// EIP 37653
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  xTcgPei.c
//
// Description:
//        Contians entry point function for TcgPei Subcomponent
//
//<AMI_FHDR_END>
//*************************************************************************
#include <EFI.h>
#include <AmiTcg\TcgCommon12.h>
#include <AmiTcg\TcgMisc.h>
#include <AmiTcg\TpmLib.h>
#include "PPI\TcgService.h"
#include "PPI\TpmDevice.h"
#include <Library/DebugLib.h>
#include <Ppi\ReadOnlyVariable2.h>
#include <Ppi\AmiTcgPlatformPpi.h>
#include "Ppi\TcgPlatformSetupPeiPolicy.h"
#include <Ppi\MemoryDiscovered.h>


EFI_STATUS
EFIAPI TcgPeiMemoryCallbackEntry(
    IN EFI_PEI_SERVICES    **PeiServices
);

EFI_STATUS
EFIAPI TcgTcmPeiMemoryCallbackEntry(
    IN EFI_PEI_SERVICES    **PeiServices
);


typedef struct _TCG_PEI_MEMORY_CALLBACK
{
    EFI_PEI_NOTIFY_DESCRIPTOR NotifyDesc;
    EFI_FFS_FILE_HEADER       *FfsHeader;
} TCG_PEI_MEMORY_CALLBACK;


EFI_STATUS
EFIAPI  TcgPeiMemoryEntry(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi );


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
EFIAPI TcgPeiMemoryEntry(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi )
{
    EFI_STATUS                 Status;
    PEI_TCG_PPI                 *TcgPpi = NULL;

    Status =  (*PeiServices)->LocatePpi (
                  PeiServices,
                  &gPeiTcgPpiGuid ,
                  0,
                  NULL,
                  &TcgPpi);

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    if(IsTcmSupportType())
    {
        TcgTcmPeiMemoryCallbackEntry(PeiServices);
    }
    else
    {
        TcgPeiMemoryCallbackEntry(PeiServices);
    }

    return (Status);
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   CommonTcgPeiEntryPoint
//
// Description: Entry point for Tcg PEI component
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
ReInstallTcgServiceAfterMem(
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN CONST EFI_PEI_SERVICES    **PeiServices )
{
    EFI_STATUS                     Status;
    //TCG_PEI_MEMORY_CALLBACK         *MemCallback;
    //TCG_CONFIGURATION              ConfigFlags;

    Status = IsTpmPresent((TPM_1_2_REGISTERS_PTR)(UINTN )PORT_TPM_IOMEMBASE);
    if(EFI_ERROR(Status))return Status;

    return EFI_SUCCESS;

    /*Status = (*PeiServices)->LocatePpi(
                PeiServices,
                &gTcgPeiPolicyGuid,
                0, NULL,
                &TcgPeiPolicy);

    if(EFI_ERROR(Status))return Status;

    Status = TcgPeiPolicy->getTcgPeiPolicy((EFI_PEI_SERVICES    **)PeiServices, &ConfigFlags);

    if (ConfigFlags.TpmSupport == 0x00  || EFI_ERROR( Status ))
    {
        return EFI_SUCCESS;
    }


    Status = (**PeiServices).AllocatePool(
                    PeiServices,
                    sizeof (TCG_PEI_MEMORY_CALLBACK),
                    &MemCallback);

    if ( !EFI_ERROR( Status ))
    {
        MemCallback->NotifyDesc.Flags
            = (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
               | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
        MemCallback->NotifyDesc.Guid   = &gCacheInstallGuid;
        MemCallback->NotifyDesc.Notify = TcgPeiMemoryEntry;
        MemCallback->FfsHeader         = FfsHeader;

        Status = (*PeiServices)->NotifyPpi( PeiServices,
                                  &MemCallback->NotifyDesc );
    }

    return Status;*/
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
