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
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPei/xTcgPei.c 1     10/08/13 12:02p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:02p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgPei/xTcgPei.c $
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
#include <Efi.h>
#include <AmiTcg/TcgCommon12.h>
#include <AmiTcg/TCGMisc.h>
#include <Ppi/AmiTcgPlatformPpi.h>
#include "Ppi/TcgService.h"
#include "Ppi/TpmDevice.h"
#include "Ppi/CpuIo.h"
#include "Ppi/LoadFile.h"
#include "Ppi/TcgPlatformSetupPeiPolicy.h"
#include <AmiTcg/Tpm20.h>
#include <Library/DebugLib.h>
#include <Guid/AmiTcgGuidIncludes.h>
#include <Guid/HobList.h>
// APTIOV_SERVER_OVERRIDE_START : System Hangs at TcgPei during Dwr flow when PTT is enabled
#include <Library/PchPmcLib.h>
// APTIOV_SERVER_OVERRIDE_END : System Hangs at TcgPei during Dwr flow when PTT is enabled

extern EFI_GUID gTcgPeiPolicyGuid;
extern EFI_GUID gAmiPlatformSecurityChipGuid;
extern EFI_GUID gAmiGlobalVariableGuid;

EFI_STATUS
EFIAPI TpmPeiEntry (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES    **PeiServices );

EFI_STATUS
EFIAPI TcmPeiEntry (
    IN EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES    **PeiServices );


EFI_STATUS
EFIAPI TcgPeiEntry (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES    **PeiServices );

EFI_STATUS
EFIAPI TcgTcmPeiEntry (
    IN EFI_PEI_FILE_HANDLE *FileHandle,
    IN CONST EFI_PEI_SERVICES    **PeiServices );

EFI_STATUS TcgPeiBuildHobGuid(
    IN CONST  EFI_PEI_SERVICES **PeiServices,
    IN EFI_GUID         *Guid,
    IN UINTN            DataLength,
    OUT VOID            **Hob );

EFI_STATUS
EFIAPI Tpm20CrbEntry(
    IN EFI_PEI_FILE_HANDLE *FileHandle,
    IN CONST EFI_PEI_SERVICES    **PeiServices );

UINT8 GetPlatformSupportType()
{
    return (IsTcmSupportType());
}

static AMI_TCG_PLATFORM_PPI PlatformTypePpi =
{
    GetPlatformSupportType
};


static EFI_PEI_PPI_DESCRIPTOR mPlatformPpiList[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAmiPlatformSecurityChipGuid,
        &PlatformTypePpi
    }
};


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
CommonTcgPeiEntryPoint(
    IN   IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES    **PeiServices )
{
    EFI_STATUS                     Status;
    TCG_PLATFORM_SETUP_INTERFACE   *TcgPeiPolicy = NULL;
    TCG_CONFIGURATION              ConfigFlags;
    CHAR16                         Monotonic[] = L"MonotonicCounter";
#if defined(CORE_COMBINED_VERSION) && (CORE_COMBINED_VERSION > 262797)
    EFI_GUID                       Guid        = gAmiGlobalVariableGuid;
#else
    EFI_GUID                       Guid        = gEfiGlobalVariableGuid;
#endif
    UINTN                          Size        = sizeof(UINT32);
    UINT32                         Counter;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
    EFI_HOB_GUID_TYPE               *Hob;
    BOOLEAN                         ResetAllTcgVar = FALSE;


    Status = (*PeiServices)->InstallPpi( PeiServices, &mPlatformPpiList[0] );
    if ( EFI_ERROR( Status ))
    {
        return EFI_UNLOAD_IMAGE;
    }

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(EFI_ERROR(Status) || TcgPeiPolicy == NULL )return Status;

    Status = TcgPeiPolicy->getTcgPeiPolicy( (EFI_PEI_SERVICES    **)PeiServices, &ConfigFlags);

    if(ConfigFlags.DeviceType == 1)return EFI_SUCCESS;


    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gEfiPeiReadOnlyVariable2PpiGuid,
                 0, NULL,
                 &ReadOnlyVariable
             );

    if(EFI_ERROR(Status) || ReadOnlyVariable == NULL )return Status;

    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, Monotonic, &Guid,
                                           NULL, &Size, &Counter );


    if ( EFI_ERROR( Status ))
    {

        ResetAllTcgVar = TRUE;
        Status         = TcgPeiBuildHobGuid(
                             PeiServices,
                             &AmiTcgResetVarHobGuid,
                             sizeof (BOOLEAN),
                             &Hob );

        Hob++;
        (*PeiServices)->CopyMem( Hob, &ResetAllTcgVar, sizeof (ResetAllTcgVar));
    }

    // APTIOV_SERVER_OVERRIDE_START : System Hangs at TcgPei during Dwr flow when PTT is enabled
    if (PchIsDwrFlow()) {
      DEBUG((DEBUG_ERROR, "[TcgPei] WARNING: DWR detected - skip Tpm access in TcgPei module\n"));
      return EFI_UNSUPPORTED;
    }
    // APTIOV_SERVER_OVERRIDE_END : System Hangs at TcgPei during Dwr flow when PTT is enabled

    if(!IsTcmSupportType())
    {
        Status = TpmPeiEntry( FileHandle, PeiServices );
        DEBUG((DEBUG_INFO, "TpmPeiEntry results = %r \n", Status));
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }
    }
    else
    {
        Status = TcmPeiEntry( FileHandle, PeiServices );
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }
    }

    if ( ConfigFlags.TpmSupport == 0x00  || EFI_ERROR( Status ))
    {
        DEBUG((DEBUG_ERROR, "ConfigFlags.TpmSupport == 0x00  || EFI_ERROR( Status )\n"));
        return EFI_SUCCESS;
    }


    DEBUG((DEBUG_INFO,  "TcgPeiEntry processing\n"));
    if(!IsTcmSupportType())
    {
        Status = TcgPeiEntry( FileHandle, PeiServices );
    }
    else
    {
        Status = TcgTcmPeiEntry( FileHandle, PeiServices );
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
