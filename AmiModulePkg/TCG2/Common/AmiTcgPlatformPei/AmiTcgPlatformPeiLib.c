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
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformPeiLib.c 1     10/08/13 12:04p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:04p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformPeiLib.c $
//
// 1     10/08/13 12:04p Fredericko
// Initial Check-In for Tpm-Next module
//
// 1     7/10/13 5:54p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 19    3/31/13 7:40p Fredericko
// [TAG]        EIP118211
// [Category]   Improvement
// [Description]    Implement ability to skip Physical presence lock in
// manufacturing mode
// [Files]          TcgPei.c,  AmiTcgPlatformDxe.c, AmiTcgNvflagSample.c
//
// [TAG]        EIP
// [Category]   Improvement
// [Description]    Timing policy changes for slower TPMs (ability to skip
// setting of physical presence in PEI to DXE)
// [Files]      AmiTcgPlatformDxe.c, AmiTcgNvflagSample.c,
// AmiTcgPlatformPei.c
//
// 18    3/06/13 3:23p Fredericko
// [TAG]        EIP112717
// [Category]   Improvement
// [Description]    Change code to use PCR from token definition file
// instead of using numbers
// [Files]          xTcgDxe.c
// AmiTcgPlatformPeiLib.c
// AmiTcgPlatformDxe.c
//
// 17    11/05/12 11:12a Fredericko
// Continue Selftest Vendor ID Token added
//
// 16    4/27/12 6:16p Fredericko
// 1. Remove unused functions.
//
// 15    3/19/12 6:47p Fredericko
// Changes for Tcg Performance Metrics Improvement.
// Files Changed: Tcg.sdl, TcgMisc.h, TcgDxe.c, TcgCommon.c, TcgCommon.h,
// AmiTcgPlatformPeiLib.c, AmiTcgPlatformDxe.c, TcgDxe.dxs
//
// 14    2/03/12 5:52p Fredericko
// [TAG]        EIP81665
// [Category]   Improvement
// [Description]    Support for MOR feature improvement. Removed unneed
// functions.
// EIP: 80813: System will assert in AmiTcgPlatformPei.lib if PeiRamboot
// module is not included in the project
// [Files]          Tcg.sdl, AmiTcgPlatformDxe.c, Tcgdxe.c, Tcglegacy.c
//
// 13    12/30/11 4:58p Fredericko
// [TAG]        EIP78141
// [Category]   New Feature
// [Description]    Added hooks to override generic TPM platform hash
// functions.
// [Files]          1. AmiTcgPlatform.sdl
// 2. AmiTcgPlatformPei.h
// 3. AmiTcgPlatformPeiLib.c
// 4. AmiTcgPlatformPeiAfterMem.c
// 5. AmiTcgPlatformDxe.c
// 6. AmiTcgPlatformDxe.h
//
// 12    12/18/11 10:27p Fredericko
// Changes to support TcgplatformPeiPolicy in relation to O.S. requests.
//
// 11    12/12/11 3:52p Fredericko
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
// 10    9/27/11 10:28p Fredericko
// [TAG]        EIP67286
// [Category]   Improvement
// [Description]    changes for Tcg Setup policy
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
// 9     9/01/11 2:25p Fredericko
// [TAG]        EIP66113
// [Category]   Improvement
// [Description]    Support LTsx on server platforms where startup commands
// are sent by the ACM binaries.
// [Files]          AmiTcgPlatformPeiBoardBeforeMem.c
// AmiTcgPlatformPeiLib.c
// AmiTcgPlatformPei.h
//
// 8     8/26/11 3:00p Fredericko
//
// 7     8/09/11 6:30p Fredericko
// [TAG]        EIP66468
// [Category]   Spec Update
// [Severity]   Minor
// [Description]    1. Changes for Tcg Ppi 1.2 support.
// [Files]          1 TcgSmm.h
// 2.TcgSmm.c
// 3.Tcg_ppi1_2.asl
// 4. AmiTcgNvflagsSample.c
// 5. AmiTcgPlatformPeiLib.c
// 6. AmiTcgPlatformDxe.sdl
// 7. AmiTcgPlatformDxe.c
//
// 6     7/25/11 3:23a Fredericko
// [TAG]        EIP65177
// [Category]   Spec Update
// [Severity]   Minor
// [Description]    TCG Ppi Sec ver 1.2 update
//
// 5     4/05/11 8:08p Fredericko
// Changes for the measurement of FVMAIN in the case of the trusted
// cryptographic module
//
// 4     4/04/11 2:17p Fredericko
// Measurement of Dxe FVol commented back into code
//
// 3     3/29/11 9:20p Fredericko
// Handle TPM startup and selftest errors as fatal errors. Don't continue
// with any TPM initialization.
//
// 2     3/29/11 2:20p Fredericko
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
// Name:    AmiTcgPlatformPeilib.c
//
// Description: Function file that contains library files for AmiTcgPlatformPei
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Uefi.h>
#include <Token.h>
#include <AmiTcg/TcgCommon12.h>
#include <AmiTcg/Tpm20Pei.h>
#include <AmiTcg/TCGMisc.h>
#include <Ppi/ReadOnlyVariable2.h>
#include "AmiTcg/AmiTcgPlatformPei.h"
#include <Ppi/TcgPlatformSetupPeiPolicy.h>
#include <Ppi/AmiTcgPlatformPpi.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <AmiTcg/AmiTpmStatusCodes.h>
#include <Guid/AmiTcgGuidIncludes.h>
#include <Hob.h>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

extern EFI_GUID gTcgPeiPolicyGuid;

//*************************************************************************
//                      GLOBAL DEFINITIONS
//*************************************************************************
typedef struct
{
    TPM_1_2_CMD_HEADER hdr;
    UINT32             pcr;
    TCG_DIGEST         digest;
} TPM_EXTEND_CMD;

typedef struct
{
    TPM_1_2_RET_HEADER rethdr;
    TCG_DIGEST         Outdigest;
} TPM_EXTEND_RET;

//**********************************************************************
//                  Links
//**********************************************************************
extern MEASURE_CRTM_VERSION_PEI_FUNC_PTR  MEASURE_CRTM_VERSION_PEI_FUNCTION;
MEASURE_CRTM_VERSION_PEI_FUNC_PTR *MeasureCRTMVersionFuncPtr = MEASURE_CRTM_VERSION_PEI_FUNCTION;

EFI_STATUS 
EFIAPI
TpmPeiReportStatusCode(  IN EFI_STATUS_CODE_TYPE   Type,
                         IN EFI_STATUS_CODE_VALUE  Value);

//**********************************************************************
//                  TCG_Helper functions
//**********************************************************************
#pragma pack(1)
typedef struct _TCG_PEI_CALLBACK_CONTEXT
{
    PEI_TPM_PPI      *TpmDevice;
    EFI_PEI_SERVICES **PeiServices;
} TCG_PEI_CALLBACK_CONTEXT;
#pragma pack()


EFI_STATUS
EFIAPI TcgCommonPassThrough(
    IN VOID                    *CallbackContext,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers )
{
    TCG_PEI_CALLBACK_CONTEXT *Ctx;

    Ctx = (TCG_PEI_CALLBACK_CONTEXT*)CallbackContext;

    return Ctx->TpmDevice->Transmit(
               Ctx->TpmDevice,
               Ctx->PeiServices,
               NoInputBuffers,
               InputBuffers,
               NoOutputBuffers,
               OutputBuffers
           );
}


EFI_STATUS
EFIAPI TcmCommonPassThrough(
    IN VOID                    *CallbackContext,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers )
{
    TCG_PEI_CALLBACK_CONTEXT *Ctx;

    Ctx = (TCG_PEI_CALLBACK_CONTEXT*)CallbackContext;

    return Ctx->TpmDevice->Transmit(
               Ctx->TpmDevice,
               Ctx->PeiServices,
               NoInputBuffers,
               InputBuffers,
               NoOutputBuffers,
               OutputBuffers
           );
}


BOOLEAN 
EFIAPI
IsMfgMode(
    IN EFI_PEI_SERVICES **PeiServices,
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
);


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgPeiBuildHobGuid
//
// Description: Internal abstracted function to create a Hob
//
// Input:       IN  EFI_PEI_SERVICES  **PeiServices,
//              IN  EFI_GUID          *Guid,
//              IN  UINTN             DataLength,
//              OUT VOID              **Hob
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
EFI_STATUS TcgPeiBuildHobGuid(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_GUID         *Guid,
    IN UINTN            DataLength,
    OUT VOID            **Hob )
{
    EFI_STATUS Status;

    Status = (*PeiServices)->CreateHob(
                 PeiServices,
                 EFI_HOB_TYPE_GUID_EXTENSION,
                 (UINT16) ( sizeof (EFI_HOB_GUID_TYPE) + DataLength ),
                 Hob
             );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    ((EFI_HOB_GUID_TYPE*)(*Hob))->Name = *Guid;

    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FillDriverLocByFile
//
// Description: Helper function to locate a Driver by guid and fill in
//              Offset data about it. Mainly for MA Driver
//
//
// Input:       IN OUT UINT32* Offset,
//              IN     EFI_PEI_SERVICES  **ps,
//              IN     EFI_GUID *Driveguid,
//              IN OUT VOID **MAStart,
//              IN OUT UINTN *MASize
//
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS FillDriverLocByFile(
    IN OUT UINT32       * Offset,
    IN EFI_PEI_SERVICES **ps,
    IN EFI_GUID         *Driveguid,
    IN OUT VOID         **MAStart,
    IN OUT UINTN        *MASize )
{
    UINT8                      * Temp;
    EFI_FIRMWARE_VOLUME_HEADER *pFV;
    EFI_FFS_FILE_HEADER        *ppFile;
    EFI_STATUS                 Status;
    UINTN                      Instance = 0;
    MPDRIVER_LEGHEADER         *Buffer;
    UINT32                     CodeSec = 0;

    while ( TRUE )
    {
        Status = (*ps)->FfsFindNextVolume((CONST EFI_PEI_SERVICES **)ps, Instance, &pFV );

        if ( EFI_ERROR( Status ))
        {
            return  Status;
        }

        ppFile = NULL;
        //
        // Start new search in volume
        //
        while ( TRUE )
        {
            Status = (*ps)->FfsFindNextFile( (CONST EFI_PEI_SERVICES **)ps,
                                             EFI_FV_FILETYPE_FREEFORM,
                                             pFV,
                                             &ppFile );

            if ( Status == EFI_NOT_FOUND )
            {
                break;
            }

            if ( CompareMem( &(ppFile->Name), Driveguid, sizeof(EFI_GUID) ) == 0)
            {
                Temp = ((UINT8*) ppFile + sizeof (EFI_FFS_FILE_HEADER)
                        + sizeof(ppFile->Size));
                Buffer   = (MPDRIVER_LEGHEADER*)(Temp + 1);
                CodeSec  = Buffer->CodeP;
                *Offset  = (UINT32)Buffer + CodeSec;
                *MAStart = Buffer;
                *MASize  = (UINTN)Buffer->Size;
                return Status;
            }
        }
        Instance += 1;
    }
}

EFI_STATUS InternalFindNextHobByType(IN UINT16   Type, IN OUT VOID **Hob)
{
    EFI_HOB_GENERIC_HEADER *ThisHob;
    if (Hob == NULL) return EFI_INVALID_PARAMETER;
    ThisHob = *Hob;
    while(ThisHob->HobType != EFI_HOB_TYPE_END_OF_HOB_LIST)
    {
        ThisHob=NextHob(ThisHob,EFI_HOB_GENERIC_HEADER);
        if (ThisHob->HobType==Type)
        {
            *Hob=ThisHob;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FillDriverLoc
//
// Description: Minor function to fill in MPDriver Offsets for TPM
//              Device PPI
//
//
// Input:       IN  OUT  UINT32* Offset,
//              IN       EFI_PEI_SERVICES  **ps,
//              IN       EFI_GUID *Driveguid
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
VOID FillDriverLoc(
    IN OUT UINT32       * Offset,
    IN EFI_PEI_SERVICES **ps,
    IN EFI_GUID         *Driveguid )
{
    EFI_HOB_GUID_TYPE *DrvHob;
    UINT8             * Temp;

    (*ps)->GetHobList( ps, &DrvHob );
    while ( !EFI_ERROR( InternalFindNextHobByType( EFI_HOB_TYPE_GUID_EXTENSION, &DrvHob )))
    {
        if ((DrvHob->Header.HobType == EFI_HOB_TYPE_GUID_EXTENSION)
                && (CompareMem( &DrvHob->Name, Driveguid, sizeof(EFI_GUID) )) == 0 )
        {
            Temp    = (UINT8*)++DrvHob;
            *Offset = *(UINT32*) Temp;
            break;
        }
    }
}






//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   LocateTcgPpi
//
// Description: Locates and initializes TCG Ppi
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
EFIAPI
LocateTcgPpi(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_TPM_PPI      **gTpmDevicePpi,
    IN PEI_TCG_PPI      **gTcgPpi
)
{
    EFI_STATUS      Status;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gPeiTpmPpiGuid,
                 0, NULL,
                 gTpmDevicePpi);


    if(EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "gTpmDevicePpiguid NOT found %r \n", Status));
        return Status;
    }

    Status =  (*PeiServices)->LocatePpi(
                  PeiServices,
                  &gPeiTcgPpiGuid,
                  0, NULL,
                  gTcgPpi);

    if(EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "gTcgPpiguid NOT found %r \n", Status));
        return Status;
    }

    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   LocateTcgPpi
//
// Description: Locates and initializes TCG Ppi
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
EFIAPI LocateTcmPpi(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_TPM_PPI      **gTpmDevicePpi,
    IN PEI_TCM_PPI      **gTcmPpi
)
{
    EFI_STATUS      Status;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gPeiTpmPpiGuid,
                 0, NULL,
                 gTpmDevicePpi);


    if(EFI_ERROR(Status))return Status;

    Status =  (*PeiServices)->LocatePpi(
                  PeiServices,
                  &gPeiTcgPpiGuid,
                  0, NULL,
                  gTcmPpi);

    return Status;
}









//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgPeiTpmStartup
//
// Description: Sends Initial TPM Startup Command
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//              IN      EFI_BOOT_MODE                BootMode
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
EFIAPI
TcgPeiTpmStartup(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_BOOT_MODE    BootMode )
{
    EFI_STATUS               Status;
    TPM_1_2_CMD_STARTUP      cmdStartup;
    TPM_1_2_RET_HEADER       retHeader;
    TPM_STARTUP_TYPE         TpmSt;
    PEI_TPM_PPI              *TpmPpi = NULL;
    PEI_TCG_PPI              *TcgPpi = NULL;
    TCG_PLATFORM_SETUP_INTERFACE   *TcgPeiPolicy = NULL;
    TCG_CONFIGURATION              ConfigFlags;

    TpmSt = TPM_ST_CLEAR;

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(EFI_ERROR(Status) || TcgPeiPolicy == NULL )return Status;

    Status = TcgPeiPolicy->getTcgPeiPolicy(PeiServices, &ConfigFlags);

    if(EFI_ERROR(Status))return Status;

    if(ConfigFlags.DisallowTpm == 1)
    {
        BootMode = BOOT_IN_RECOVERY_MODE;  //deactivate the TPM
    }

    if ( BootMode == BOOT_ON_S3_RESUME )
    {
        TpmSt = TPM_ST_STATE;
    }

#if (TCG_CONVENTIONAL_BIOS_6_1)

    if ( BootMode == BOOT_IN_RECOVERY_MODE )
    {
        TpmSt = TPM_ST_DEACTIVATED;
    }
#endif

    Status = LocateTcgPpi(PeiServices,&TpmPpi, &TcgPpi);
    if(EFI_ERROR(Status))return EFI_NOT_FOUND;

    Status = TpmPpi->Init(TpmPpi, PeiServices );
    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    cmdStartup.Header.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdStartup.Header.ParamSize = TPM_H2NL( sizeof (cmdStartup));
    cmdStartup.Header.Ordinal   = TPM_H2NL( TPM_ORD_Startup );
    cmdStartup.StartupType      = TPM_H2NS( TpmSt );

    Status = TcgPpi->TCGPassThroughToTpm(
                 TcgPpi,
                 PeiServices,
                 sizeof (cmdStartup),
                 (UINT8*)&cmdStartup,
                 sizeof (retHeader),
                 (UINT8*)&retHeader);

    DEBUG((DEBUG_INFO,
           "Tcg Startup Command Return Code: size: %x; retCode:%x; tag:%x; bytes %08x\n",
           TPM_H2NL(retHeader.ParamSize ),
           TPM_H2NL(retHeader.RetCode ),
           (UINT32)
           TPM_H2NS(retHeader.Tag )));

    if ( retHeader.ParamSize == 0 )
    {
        DEBUG((DEBUG_ERROR, "Tcg Startup: returning device error\n"));
        return EFI_DEVICE_ERROR;

    }

    if(retHeader.RetCode != 0)
    {

        DEBUG((DEBUG_ERROR, "Error sending TPM Startup CMD\n"));
        return EFI_DEVICE_ERROR;
    }

    Status = TpmPpi->Close(TpmPpi, PeiServices );
    if (EFI_ERROR( Status ))
    {
        return Status;
    }

    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmPeiStartup
//
// Description: Sends Initial TPM Startup Command
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//              IN      EFI_BOOT_MODE                BootMode
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
static
EFI_STATUS
EFIAPI TcmPeiStartup(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_BOOT_MODE    BootMode )
{
    EFI_STATUS               Status;
    TPM_1_2_CMD_STARTUP      cmdStartup;
    TPM_1_2_RET_HEADER       retHeader;
    TPM_STARTUP_TYPE         TpmSt;
    PEI_TPM_PPI              *TpmPpi = NULL;
    PEI_TCM_PPI              *TcgPpi = NULL;

    TpmSt = TPM_ST_CLEAR;

    if ( BootMode == BOOT_ON_S3_RESUME )
    {
        TpmSt = TPM_ST_STATE;
    }

#if (TCG_CONVENTIONAL_BIOS_6_1)

    if ( BootMode == BOOT_IN_RECOVERY_MODE )
    {
        TpmSt = TPM_ST_DEACTIVATED;
    }
#endif

    Status = LocateTcmPpi(PeiServices,&TpmPpi, &TcgPpi);
    if(EFI_ERROR(Status))return EFI_NOT_FOUND;

    Status = TpmPpi->Init(TpmPpi, PeiServices );
    if ( EFI_ERROR( Status ))
    {
        return Status;
    }


    cmdStartup.Header.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdStartup.Header.ParamSize = TPM_H2NL( sizeof (cmdStartup));
    cmdStartup.Header.Ordinal   = TPM_H2NL( TCM_ORD_Startup );

    cmdStartup.StartupType      = TcgCommonH2NS( TpmSt );

    Status = TcgPpi->TCMPassThroughToTcm(
                 TcgPpi,
                 PeiServices,
                 sizeof (cmdStartup),
                 (UINT8*)&cmdStartup,
                 sizeof (retHeader),
                 (UINT8*)&retHeader);


    Status = TpmPpi->Close(TpmPpi, PeiServices );
    if (EFI_ERROR( Status ))
    {
        return Status;
    }

    DEBUG((DEBUG_INFO,
           "Tcg TCM Startup Command Return Code: size: %x; retCode:%x; tag:%x; bytes %08x\n",
           TPM_H2NL(retHeader.ParamSize ),
           TPM_H2NL(retHeader.RetCode ),
           (UINT32)
           TPM_H2NS(retHeader.Tag )));

    if ( retHeader.ParamSize == 0 )
    {
        return EFI_DEVICE_ERROR;
    }
    return (Status | TPM_H2NL( retHeader.RetCode ));
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ContinueTPMSelfTest
//
// Description: Executes ContinueSelfTest operation on TPM. Certain TPM
//              operation require this operation to be execute before.
//
// Input:       IN  EFI_PEI_SERVICES **ps
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
EFIAPI ContinueTPMSelfTest(
    IN EFI_PEI_SERVICES **ps )
{
    TPM_1_2_CMD_HEADER  cmd;
    TPM_1_2_RET_HEADER  result;
    EFI_STATUS          Status;
    PEI_TPM_PPI         *TpmPpi = NULL;
    PEI_TCG_PPI         *TcgPpi = NULL;


#if defined DONT_SEND_SELFTEST_TILL_READY_TO_BOOT && DONT_SEND_SELFTEST_TILL_READY_TO_BOOT == 1
    if(*(UINT16 *)(UINTN)(PORT_TPM_IOMEMBASE + 0xF00) == SELF_TEST_VID)
    {
        return EFI_SUCCESS;
    }
#endif

    Status = LocateTcgPpi(ps,&TpmPpi, &TcgPpi);
    if(EFI_ERROR(Status))return EFI_NOT_FOUND;

    Status = TpmPpi->Init(TpmPpi, ps );
    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    cmd.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmd.ParamSize = TPM_H2NL( sizeof (cmd));
    cmd.Ordinal   = TPM_H2NL( TPM_ORD_ContinueSelfTest );

    Status =  TcgPpi->TCGPassThroughToTpm(
                  TcgPpi,
                  ps,
                  sizeof (cmd),
                  (UINT8*)&cmd,
                  sizeof (result),
                  (UINT8*)&result );

    DEBUG((DEBUG_INFO, "TCG Pei: Self Test : status=%x; RetCode=%x\n", Status,
           TPM_H2NL( result.RetCode )));

    Status = TpmPpi->Close(TpmPpi, ps );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }
    else if ( result.RetCode != 0 )
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ContinueTCMSelfTest
//
// Description: Executes ContinueSelfTest operation on TPM. Certain TPM
//              operation require this operation to be execute before.
//
// Input:       IN  EFI_PEI_SERVICES **ps
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
EFIAPI 
ContinueTCMSelfTest(
    IN EFI_PEI_SERVICES **ps )
{
    TPM_1_2_CMD_HEADER       cmd;
    TPM_1_2_RET_HEADER       result;
    EFI_STATUS               Status;
    PEI_TPM_PPI         *TpmPpi = NULL;
    PEI_TCG_PPI         *TcgPpi = NULL;


    Status = LocateTcgPpi(ps,&TpmPpi, &TcgPpi);
    if(EFI_ERROR(Status))return EFI_NOT_FOUND;

    Status = TpmPpi->Init(TpmPpi, ps );
    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    cmd.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmd.ParamSize = TPM_H2NL( sizeof (cmd));
    cmd.Ordinal   = TPM_H2NL(TCM_ORD_ContinueSelfTest );

    Status =  TcgPpi->TCGPassThroughToTpm(
                  TcgPpi,
                  ps,
                  sizeof (cmd),
                  (UINT8*)&cmd,
                  sizeof (result),
                  (UINT8*)&result );

    DEBUG((DEBUG_INFO, "TCG TCM Pei: Self Test : status=%x; RetCode=%x\n", Status,
           TPM_H2NL( result.RetCode )));

    Status = TpmPpi->Close(TpmPpi, ps );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    if ( result.RetCode != 0 )
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   SendStartupandSelftest
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
EFI_STATUS SendStartupandSelftest(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_BOOT_MODE    BootMode
)
{
    EFI_STATUS      Status;
    BOOLEAN         SkipTpmStartup = FALSE;
    AMI_TCG_PEI_FUNCTION_OVERRIDE_PPI *PpiOverride;
    TCG_PLATFORM_SETUP_INTERFACE   *TcgPeiPolicy = NULL;
    TCG_CONFIGURATION              ConfigFlags;

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
                 &gTcgPeiPolicyGuid,
                 0, NULL,
                 &TcgPeiPolicy);

    if(EFI_ERROR(Status) || TcgPeiPolicy == NULL )return Status;

    Status = TcgPeiPolicy->getTcgPeiPolicy(PeiServices, &ConfigFlags);

    if(EFI_ERROR(Status))return Status;

    if(ConfigFlags.DisallowTpm == 1)
    {
        BootMode = BOOT_IN_RECOVERY_MODE;  //deactivate the TPM
        Status = TcgPeiTpmStartup( PeiServices, BootMode );
        return Status;
    }

    if(!IsTcmSupportType())
    {
        if (!SkipTpmStartup)
        {
            Status = TcgPeiTpmStartup( PeiServices, BootMode );
            if(Status)
            {
                //sartup command failed
                TpmPeiReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_1_2_STARTUP_ERROR | EFI_SOFTWARE_PEI_MODULE);   
                return EFI_DEVICE_ERROR;
            }
            TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_STARTUP_CMD_SENT | EFI_SOFTWARE_PEI_MODULE);
        }

        Status = ContinueTPMSelfTest( PeiServices );
        if(Status)
        {
            //Selftest command failed
            return EFI_DEVICE_ERROR;
        }else{
            TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_SELFTEST_CMD_SENT | EFI_SOFTWARE_PEI_MODULE);   
        }
    }
    else
    {
        Status = TcmPeiStartup( PeiServices, BootMode );
        if(Status)
        {
            //sartup command failed
            return EFI_DEVICE_ERROR;
        }

        Status = ContinueTCMSelfTest( PeiServices );
        if(Status)
        {
            //selftest command failed
            return EFI_DEVICE_ERROR;
        }
    }
    return Status;
}








//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TCGPEI_GETCAP
//
// Description: Executes TPM operation to read capabilities
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      TPM_Capabilities_PermanentFlag
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
TPM_Capabilities_PermanentFlag INTTCGPEI_GETCAP(
    IN EFI_PEI_SERVICES **PeiServices )
{
    TPM_Capabilities_PermanentFlag  *cap = NULL;
    EFI_STATUS                      Status;
    TPM_GetCapabilities_Input       cmdGetCap;
    UINT8                           result[0x100];
    PEI_TPM_PPI                     *TpmPpi = NULL;
    PEI_TCG_PPI                     *TcgPpi = NULL;

    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   = TPM_H2NL( sizeof (cmdGetCap));
    cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability );
    cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    cmdGetCap.subCapSize  = TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      = TPM_H2NL( TPM_CAP_FLAG_PERMANENT );

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gPeiTpmPpiGuid,
                 0, NULL,
                 &TpmPpi);

    ASSERT_EFI_ERROR( Status );

    Status = TpmPpi->Init(TpmPpi, PeiServices);

    if(Status)
    {
        SetMem(result,sizeof(TPM_Capabilities_PermanentFlag), 0);
        cap = (TPM_Capabilities_PermanentFlag*)result;
        return *cap;
    }

    Status =  (*PeiServices)->LocatePpi(
                  PeiServices,
                  &gPeiTcgPpiGuid,
                  0, NULL,
                  &TcgPpi);

    ASSERT_EFI_ERROR(  Status );


    Status = TcgPpi->TCGPassThroughToTpm(
                 TcgPpi,
                 PeiServices,
                 sizeof(cmdGetCap),
                 (UINT8*)&cmdGetCap,
                 0x100,
                 result );

    cap = (TPM_Capabilities_PermanentFlag*)result;

    DEBUG((DEBUG_INFO,
           "GetCapability: %r; size: %x; retCode:%x; tag:%x; bytes %08x\n",
           Status,TPM_H2NL( cap->ParamSize ), TPM_H2NL(cap->RetCode ),
           (UINT32)TPM_H2NS(cap->tag ), TPM_H2NL( *(UINT32*)&cap->disabled )));


    TpmPpi->Close(TpmPpi, PeiServices);

    return *cap;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TCGPEI_GETCAP
//
// Description: Executes TPM operation to read capabilities
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      TPM_Capabilities_PermanentFlag
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
TPM_Capabilities_PermanentFlag NuvotonProprietaryGetFlags(
    IN EFI_PEI_SERVICES **PeiServices )
{
    TPM_Capabilities_PermanentFlag  cap;
    EFI_STATUS                      Status;
    TPM_RQU_COMMAND_HDR             cmdGetTpmStatus;
    UINT8                           result[0x100];
    PEI_TPM_PPI                     *TpmPpi = NULL;
    PEI_TCG_PPI                     *TcgPpi = NULL;

    cmdGetTpmStatus.tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetTpmStatus.paramSize   = TPM_H2NL( sizeof (TPM_RQU_COMMAND_HDR));
    cmdGetTpmStatus.ordinal     = TPM_H2NL( NTC_ORD_GET_TPM_STATUS );

    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gPeiTpmPpiGuid,
                 0, NULL,
                 &TpmPpi);

    ASSERT_EFI_ERROR(  Status );

    Status = TpmPpi->Init(TpmPpi, PeiServices);

    SetMem(&cap,sizeof(TPM_Capabilities_PermanentFlag), 0);

    if(Status)
    {
        return cap;
    }

    Status =  (*PeiServices)->LocatePpi(
                  PeiServices,
                  &gPeiTcgPpiGuid,
                  0, NULL,
                  &TcgPpi);

    ASSERT_EFI_ERROR( Status );


    TcgPpi->TCGPassThroughToTpm(
        TcgPpi,
        PeiServices,
        sizeof(cmdGetTpmStatus),
        (UINT8*)&cmdGetTpmStatus,
        0x100,
        result );

    if(((NUVOTON_SPECIFIC_FLAGS *)result)->RetCode == 0)
    {
        if(((NUVOTON_SPECIFIC_FLAGS *)result)->isdisabled)
        {
            cap.disabled = 1;
        }

        if(((NUVOTON_SPECIFIC_FLAGS *)result)->isdeactivated)
        {
            cap.deactivated = 1;
        }

        if(((NUVOTON_SPECIFIC_FLAGS *)result)->isOwnerSet)
        {
            cap.ownership = 1;
        }
    }
    else
    {
        cap.RetCode = ((NUVOTON_SPECIFIC_FLAGS *)result)->RetCode;
    }


    TpmPpi->Close(TpmPpi, PeiServices);

    return cap;
}








//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TCGPEI_GETCAP
//
// Description: Executes TPM operation to read capabilities
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      TPM_Capabilities_PermanentFlag
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
TCM_Capabilities_PermanentFlag TCMPEI_GETCAP(
    IN EFI_PEI_SERVICES **PeiServices )
{
    TCM_Capabilities_PermanentFlag * cap = NULL;
    EFI_STATUS                     Status;
    TPM_GetCapabilities_Input      cmdGetCap;
    UINT8                          result[0x100];
    PEI_TPM_PPI                    *TpmPpi = NULL;
    PEI_TCM_PPI                    *TcgPpi = NULL;


    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   = TPM_H2NL( sizeof (cmdGetCap));
    cmdGetCap.CommandCode = TPM_H2NL( TCM_ORD_GetCapability );

    cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    cmdGetCap.subCapSize  = TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      = TPM_H2NL( TPM_CAP_FLAG_PERMANENT );

    Status = LocateTcmPpi(PeiServices, &TpmPpi, &TcgPpi);
    ASSERT_EFI_ERROR ( Status);

    Status = TpmPpi->Init(TpmPpi, PeiServices );
    ASSERT_EFI_ERROR ( Status);

    Status = TcgPpi->TCMPassThroughToTcm(
                 TcgPpi,
                 PeiServices,
                 sizeof(cmdGetCap),
                 (UINT8*)&cmdGetCap,
                 0x100,
                 result );

    cap = (TCM_Capabilities_PermanentFlag*)result;

    DEBUG((DEBUG_INFO,
           "GetCapability: %r; size: %x; retCode:%x; tag:%x; bytes %08x\n",
           Status,TPM_H2NL( cap->ParamSize ), TPM_H2NL(cap->RetCode ),
           (UINT32)TPM_H2NS(cap->tag ), TPM_H2NL( *(UINT32*)&cap->disabled )));

    Status = TpmPpi->Close(TpmPpi, PeiServices );
    ASSERT_EFI_ERROR ( Status);

    return *cap;
}






//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureCRTMVersion
//
// Description: Measures EFI CRTM Version
//              Demo Version[546BFB1E1D0C4055A4AD4EF4BF17B83A]
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
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
MeasureCRTMVersion(
    IN EFI_PEI_SERVICES **PeiServices )
{
    EFI_TCG_PCR_EVENT TcgEvent;
    UINT32            EventNum;
    UINTN             Len         = sizeof(EFI_GUID);
    EFI_GUID          CrtmVersion = CRTM_GUID;
    PEI_TPM_PPI       *TpmPpi = NULL;
    PEI_TCG_PPI       *TcgPpi = NULL;
    EFI_STATUS        Status;

    TcgEvent.Header.PCRIndex   = PCRi_CRTM_AND_POST_BIOS;
    TcgEvent.Header.EventType     = EV_S_CRTM_VERSION;
    TcgEvent.Header.EventDataSize = Len;

    Status = LocateTcgPpi(PeiServices,&TpmPpi, &TcgPpi);
    if(EFI_ERROR(Status))return EFI_NOT_FOUND;

    CopyMem(
        &TcgEvent.Event.SCrtmVersion,
        &CrtmVersion,
        Len
    );

    Status =  TcgPpi->TCGHashLogExtendEvent(
               TcgPpi,
               PeiServices,
               (UINT8*)&TcgEvent.Event,
               TcgEvent.Header.EventDataSize,
               (TCG_PCR_EVENT*)&TcgEvent,
               &EventNum
           );
    
    TpmPeiReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_CRTM_VERSION_MEASURED | EFI_SOFTWARE_PEI_MODULE);
    
    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureTcmCRTMVersion
//
// Description: Measures EFI CRTM Version
//              Demo Version[546BFB1E1D0C4055A4AD4EF4BF17B83A]
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
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
MeasureTcmCRTMVersion(
    IN EFI_PEI_SERVICES **PeiServices )
{
    EFI_TCM_PCR_EVENT TcmEvent;
    UINT32            EventNum;
    UINTN             Len         = sizeof(EFI_GUID);
    EFI_GUID          CrtmVersion = CRTM_GUID;
    PEI_TPM_PPI       *TpmPpi = NULL;
    PEI_TCM_PPI       *TcgPpi = NULL;
    EFI_STATUS        Status;

    TcmEvent.Header.PCRIndex      = PCRi_CRTM_AND_POST_BIOS;
    TcmEvent.Header.EventType     = EV_S_CRTM_VERSION;
    TcmEvent.Header.EventDataSize = Len;

    CopyMem(&TcmEvent.Event.SCrtmVersion,
            &CrtmVersion, Len);

    Status = LocateTcmPpi(PeiServices, &TpmPpi, &TcgPpi);
    ASSERT_EFI_ERROR ( Status);

    return TcgPpi->TCMHashLogExtendEvent(TcgPpi,
                                         PeiServices,
                                         (UINT8*)&TcmEvent.Event,
                                         TcmEvent.Header.EventDataSize,
                                         (TCM_PCR_EVENT*)&TcmEvent,
                                         &EventNum);
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureTCGPcClientSpecID
//
// Description: Includes a measurement of the TcgSpecID into PCR[0]
//
//
// Input:       IN  PEI_TCG_PPI* tcg,
//              IN  EFI_PEI_SERVICES      **ps
//
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS MeasureTCGPcClientSpecID(
    IN CONST EFI_PEI_SERVICES **ps,
    IN PEI_TCG_PPI      *tcg )
{
    TCG_PCClientSpecIDEventStruct TcgInfo;
    UINT8                         *EvntData;
    EFI_STATUS                    status;
    CHAR8                         *biosVenderName = CONVERT_TO_STRING(TcgLog_BiosName);
    CHAR8                         *oemVenderName  = CONVERT_TO_STRING(TcgLog_VendorName);
    CHAR8                         Signature[] = "Spec ID Event00";
    UINT32                        n=0;
    UINTN                         Size =  sizeof(TCG_PCClientSpecIDEventStruct) +
                                          AsciiStrLen(oemVenderName) + AsciiStrLen(biosVenderName);

    DEBUG((DEBUG_INFO, "TCG Pei: TCG_PcClientSpecID\n"));

    status = (*ps)->AllocatePool(ps,Size, (void **)&EvntData);

    if(EFI_ERROR(status))return status;

    SetMem(EvntData, Size, 0);
    SetMem(&TcgInfo, sizeof(MiscPCR_EVENT_HDR), 0);

    CopyMem(
        EvntData + sizeof(MiscPCR_EVENT_HDR),
        Signature,
        AsciiStrLen(Signature));


    //fill in pc-client spec id
    TcgInfo.PlatformClass     = TCG_PLATFORM_CLASS;
    TcgInfo.SpecVersionMajor  = 1; 
    TcgInfo.SpecVersionMinor  = 2;
    TcgInfo.SpecErrata        = 1;

#if defined TCGx64Build &&  TCGx64Build == 1
    TcgInfo.uintnSize         = sizeof(UINTN) * 2;
#else
    TcgInfo.uintnSize         = sizeof(UINTN);
#endif
    TcgInfo.VendorInfoSize    = Size - sizeof(TCG_PCClientSpecIDEventStruct);


    TcgInfo.evtHdr.PCRIndex   = PCRi_CRTM_AND_POST_BIOS;
    TcgInfo.evtHdr.EventType  = EV_NO_ACTION;
    TcgInfo.evtHdr.EventSize  = Size - sizeof(MiscPCR_EVENT_HDR);


    CopyMem(
        EvntData + sizeof(TCG_PCClientSpecIDEventStruct),
        biosVenderName,
        AsciiStrLen(biosVenderName));

    CopyMem(
        EvntData + sizeof(TCG_PCClientSpecIDEventStruct) + AsciiStrLen(biosVenderName),
        oemVenderName,
        AsciiStrLen(oemVenderName));

    CopyMem(
        EvntData + sizeof(MiscPCR_EVENT_HDR) + AsciiStrLen(Signature),
        &TcgInfo.PlatformClass,
        sizeof(TCG_PCClientSpecIDEventStruct)- sizeof(MiscPCR_EVENT_HDR) - AsciiStrLen(Signature));

    CopyMem(
        EvntData,
        &TcgInfo,
        sizeof(MiscPCR_EVENT_HDR));

    status = tcg->TCGHashLogExtendEvent(tcg,
                                        (EFI_PEI_SERVICES **)ps,
                                        EvntData,
                                        Size,
                                        (TCG_PCR_EVENT *)EvntData,
                                        &n );

    return status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureDxeCoreFwVol
//
// Description: Measures the firmware volume as a EV_POST_CODE event
//
//
// Input:       IN    PEI_TCG_PPI* tcg,
//              IN    EFI_PEI_SERVICES              **ps,
//              IN    EFI_FIRMWARE_VOLUME_HEADER    *fwv
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
#if defined(TcgNistSp800_155_SUPPORT) && (TcgNistSp800_155_SUPPORT == 1)
EFI_STATUS MeasureNISTManifest(
    IN EFI_PEI_SERVICES **ps,
    IN PEI_TCG_PPI      *tcg )
{
    TCGNISTSpecPlatformId         TcgManifestInfo;
    UINT8                         *EvntData=NULL;
    UINT32                        n;
    EFI_STATUS                    status;
    EFI_GUID                      ManifestGuid = ReferenceManifestGuid_TcgLog;
    UINTN                         Size = sizeof(TCGNISTSpecPlatformId);

    DEBUG((DEBUG_INFO,  "TCG Pei: MeasureNISTManifest\n"));

    TcgManifestInfo.ReferenceManifest = ManifestGuid;
    TcgManifestInfo.VendorId = (UINT32)TcgOemVenderID;


    TcgManifestInfo.evtHdr.PCRIndex   = PCRi_CRTM_AND_POST_BIOS;
    TcgManifestInfo.evtHdr.EventType  = EV_NO_ACTION;
    TcgManifestInfo.evtHdr.EventSize  = sizeof (TCGNISTSpecPlatformId);

    status = tcg->TCGHashLogExtendEvent(tcg,
                                        ps,
                                        EvntData,
                                        Size,
                                        (TCG_PCR_EVENT *)&TcgManifestInfo,
                                        &n );

    return status;
}
#endif


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmMeasureDxeCoreFwVol
//
// Description: Measures the firmware volume as a EV_POST_CODE event
//
//
// Input:       IN    PEI_TCM_PPI* tcg,
//              IN    EFI_PEI_SERVICES              **ps,
//              IN    EFI_FIRMWARE_VOLUME_HEADER    *fwv
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
EFI_STATUS TCMMeasureDxeCoreFwVol(
    IN PEI_TCM_PPI                * tcg,
    IN EFI_PEI_SERVICES           **ps,
    IN EFI_FIRMWARE_VOLUME_HEADER *fwv )
{
    TCM_PEI_EFI_POST_CODE   tcmev;
    UINT32            n;
    EFI_STATUS        status;
    TCG_DIGEST        TempDigest;
    void              *Context = NULL;;


    DEBUG((DEBUG_INFO, "TCG Pei: measure FwMain: at %x size %d\n", fwv,
           fwv->FvLength));

    tcmev.Header.PCRIndex      = PCRi_CRTM_AND_POST_BIOS;
    tcmev.Header.EventType     = EV_POST_CODE;
    tcmev.Header.EventDataSize = sizeof (EFI_TCG_EV_POST_CODE );

    SHA1HashAll( Context, fwv, (UINTN)fwv->FvLength, TempDigest.digest);

    tcmev.Event.PostCodeAddress = (EFI_PHYSICAL_ADDRESS)&TempDigest.digest;
#if x64_TCG
    tcmev.Event.PostCodeLength = (UINT64)TPM_SHA1_160_HASH_LEN;
#else
    tcmev.Event.PostCodeLength = (UINTN)TPM_SHA1_160_HASH_LEN;
#endif

    status = tcg->TCMHashLogExtendEvent( tcg, ps, (UINT8*)&TempDigest,
                                         (UINT32)tcmev.Event.PostCodeLength,
                                         (TCM_PCR_EVENT*)&tcmev, &n );

    return status;
}






//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   Lock_TPMPhysicalPresence
//
// Description: check whether to Lock TPM physical Presence
//
//
// Input:       IN      EFI_PEI_SERVICES          **PeiServices,
//
// Output:      BOOLEAN
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
BOOLEAN Lock_TPMPhysicalPresence(
    IN EFI_PEI_SERVICES **PeiServices )
{
    return FALSE;
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
