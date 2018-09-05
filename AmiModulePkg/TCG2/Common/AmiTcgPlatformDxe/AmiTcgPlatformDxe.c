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
// $Header: /Alaska/SOURCE/Modules/TCG/AmiTcgPlatform/AmiTcgPlatformDxe/AmiTcgPlatformDxe.c 37    5/09/12 3:59p Fredericko $
//
// $Revision: 37 $
//
// $Date: 5/09/12 3:59p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    AmiTcgPlatformDxe.c
//
// Description: Function file for AmiTcgPlatformDxe
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Token.h>
#include "Protocol/TcgPlatformSetupPolicy.h"
#include <AmiTcg/AmiTcgPlatformDxe.h>
#include<AmiTcg/sha1.h>
#include <IndustryStandard/PeImage.h>
#include <Protocol/AmiTpmSupportTypeProtocol.h>
#include<Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include<Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <AmiTcg/AmiTcgPlatformDxeLib.h>
#include "AmiTcgPlatformDxeStrDefs.h"
#include <Protocol/AMIPostMgr.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiLib.h>
#include<Library/HiiLib.h>
#include <Protocol/HiiPackageList.h>
#include <Protocol/HiiDatabase.h>
#include <Acpi.h>
#include <Library/TimerLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiSupport.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Protocol/LegacyBiosExt.h>
#include <Library/PrintLib.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/TcgPlatformSetupPolicy.h>

#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
#include <Protocol/VariableLock.h>
#endif
#include <AmiTcg/AmiTpmStatusCodes.h>
#include <Guid/AmiTcgGuidIncludes.h>


#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))
#include <ImageAuthentication.h>
#endif

#define     BIT00   0x00000001
#define EFI_ACPI_TABLE_VERSION_X        (EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0)
#define EFI_ACPI_TABLE_VERSION_ALL      (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_X)

extern EFI_GUID gAmiTcgPlatformProtocolguid;

extern MEASURE_PCI_OPTION_ROM_FUNC_PTR  MEASURE_PCI_OPTION_ROM_DXE_FUNCTION;
MEASURE_PCI_OPTION_ROM_FUNC_PTR *MeasurePCIOpromsFuncPtr = MEASURE_PCI_OPTION_ROM_DXE_FUNCTION;

extern MEASURE_HANDOFF_TABLES_FUNC_PTR MEASURE_HANDOFF_TABLES_DXE_FUNCTION;
MEASURE_HANDOFF_TABLES_FUNC_PTR *MeasureHandoffTablesFuncPtr = MEASURE_HANDOFF_TABLES_DXE_FUNCTION;

extern MEASURE_CPU_MICROCODE_FUNC_PTR MEASURE_CPU_MICROCODE_DXE_FUNCTION;
MEASURE_CPU_MICROCODE_FUNC_PTR *MeasureCpuMicroCodeFuncPtr = MEASURE_CPU_MICROCODE_DXE_FUNCTION;

extern MEASURE_BOOT_VAR_FUNC_PTR MEASURES_BOOT_VARIABLES_DXE_FUNCTION;
MEASURE_BOOT_VAR_FUNC_PTR *MeasureAllBootVariablesFuncPtr = MEASURES_BOOT_VARIABLES_DXE_FUNCTION;

#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))
extern MEASURE_SECURE_BOOT_FUNC_PTR MEASURE_SECURE_BOOT_DXE_FUNCTION;
MEASURE_SECURE_BOOT_FUNC_PTR *MeasureSecurebootVariablesFuncPtr = MEASURE_SECURE_BOOT_DXE_FUNCTION;
#endif

extern MEASURES_TCG_BOOT_SEPARATORS_DXE_FUNC_PTR MEASURES_TCG_BOOT_SEPARATORS_DXE_FUNCTION;
MEASURES_TCG_BOOT_SEPARATORS_DXE_FUNC_PTR *MeasureSeparatorsFuncPtr = MEASURES_TCG_BOOT_SEPARATORS_DXE_FUNCTION;

extern MEASURE_WAKE_EVENT_DXE_FUNC_PTR MEASURE_WAKE_EVENT_DXE_FUNCTION;
MEASURE_WAKE_EVENT_DXE_FUNC_PTR *MeasureWakeEventFuncPtr = MEASURE_WAKE_EVENT_DXE_FUNCTION;

extern MEASURE_PLATFORM_CONFIG_FUNC_PTR MEASURE_PLATFORM_CONFIG_FUNCTION;
MEASURE_PLATFORM_CONFIG_FUNC_PTR *MeasureConfigurationInfoFuncPtr = MEASURE_PLATFORM_CONFIG_FUNCTION;

EFI_STATUS TcgUpdateAslNameObject(ACPI_HDR *PDsdt, UINT8 *ObjName, UINT64 Value);
EFI_STATUS TcgLibGetDsdt(EFI_PHYSICAL_ADDRESS *DsdtAddr, EFI_ACPI_TABLE_VERSION Version);

extern EFI_GUID gEfiAcpiSupportGuid;
//extern SKIP_PHYSICAL_PRESENCE_LOCK_PTR SKIP_PHYSICAL_PRESENCE_LOCK_FUNCTION;
//SKIP_PHYSICAL_PRESENCE_LOCK_PTR *DummySkipPhysicalPresencePtr = SKIP_PHYSICAL_PRESENCE_LOCK_FUNCTION;

static     TCG_PROTOCOL_NOTIFY     Ctx; //TODO: This might not be needed
static     void     *OpromContext = NULL;
typedef    UINT32   extended_request;
AMI_POST_MANAGER_PROTOCOL *pAmiPostMgr = NULL;
EFI_HII_HANDLE            gHiiHandle;
EFI_EVENT                 OpromEvent;
VOID                     *ProcessOpRomRegistration;
EFI_EVENT                   gLegacyBootEvent;
EFI_EVENT                   gExitBootServicesEvent;

#define AMI_PASSWORD_ADMIN  2
#if EFI_SPECIFICATION_VERSION>0x20000
#else
EFI_HII_PROTOCOL          *Hii;
#endif

static      UINT8       ppi_request;
static      EFI_EVENT   ReadyToBootEvent;

#if TPM_PASSWORD_AUTHENTICATION
BOOLEAN                   AuthenticateSet;
BOOLEAN                   AdminPasswordValid;
BOOLEAN                   PasswordSupplied;

VOID SignalProtocolEvent(IN EFI_GUID *TcgPasswordAuthenticationGuid);

#endif

static BOOLEAN IsRunPpiUIAlreadyDone = FALSE;

void run_PPI_UI(
    IN EFI_EVENT ev,
    IN VOID      *ctx);

EFI_STATUS GetTcgWakeEventType(
    IN OUT UINT8 *pWake  );


EFI_STATUS TcgSetVariableWithNewAttributes(
    IN CHAR16 *Name, IN EFI_GUID *Guid, IN UINT32 Attributes,
    IN UINTN DataSize, IN VOID *Data
);
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureVariable
//
// Description: Measures a provided variable
//
// Input:       IN      TPM_PCRINDEX              PCRIndex,
//              IN      TCG_EVENTTYPE             EventType,
//              IN      CHAR16                    *VarName,
//              IN      EFI_GUID                  *VendorGuid,
//              IN      VOID                      *VarData,
//              IN      UINTN                     VarSize
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
EFI_STATUS MeasureVariable(
    IN TPM_PCRINDEX  PCRIndex,
    IN TCG_EVENTTYPE EventType,
    IN CHAR16        *VarName,
    IN EFI_GUID      *VendorGuid,
    IN VOID          *VarData,
    IN UINTN         VarSize )
{
    EFI_STATUS            Status;
    EFI_TCG_PROTOCOL      *tcgSvc=NULL;
    TCG_PCR_EVENT         *TcgEvent = NULL;
    UINTN                 VarNameLength;
    TCG_EFI_VARIABLE_DATA *VarLog=NULL;
    EFI_PHYSICAL_ADDRESS  Last;
    UINT32                evNum;

    Status = gBS->LocateProtocol(
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc );

    ASSERT( !EFI_ERROR( Status ));

    VarNameLength = StrLen( VarName );

    Status        = gBS->AllocatePool( EfiBootServicesData,
                                       _TPM_STRUCT_PARTIAL_SIZE( TCG_PCR_EVENT,Event )
                                       + (UINT32)(sizeof(*VarLog) + VarNameLength
                                               * sizeof(*VarName) + VarSize - 3 ),
                                       &TcgEvent );

    if( EFI_ERROR( Status ) || TcgEvent == NULL )return Status;

    TcgEvent->PCRIndex  = PCRIndex;
    TcgEvent->EventType = EventType;
    TcgEvent->EventSize = (UINT32)( sizeof (*VarLog) + VarNameLength
                                    * sizeof (*VarName) + VarSize - 3 );

    Status = gBS->AllocatePool( EfiBootServicesData, TcgEvent->EventSize, &VarLog );

    if (EFI_ERROR( Status ) || VarLog == NULL )
    {
        gBS->FreePool(TcgEvent);
        return EFI_OUT_OF_RESOURCES;
    }

    VarLog->VariableName       = *VendorGuid;
    VarLog->UnicodeNameLength  = VarNameLength;
    VarLog->VariableDataLength = VarSize;

    gBS->CopyMem(
        (CHAR16*)(VarLog->UnicodeName),
        VarName,
        VarNameLength * sizeof (*VarName)
    );

    gBS->CopyMem(
        (CHAR16*)(VarLog->UnicodeName) + VarNameLength,
        VarData,
        VarSize);

    gBS->CopyMem( TcgEvent->Event,
                  VarLog,
                  TcgEvent->EventSize );

    Status = tcgSvc->HashLogExtendEvent(
                 tcgSvc,
                 (EFI_PHYSICAL_ADDRESS)VarLog,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last );

    if ( TcgEvent != NULL )
    {
        gBS->FreePool( TcgEvent );
    }

    ASSERT( !EFI_ERROR( Status ));

    if ( VarLog != NULL )
    {
        gBS->FreePool( VarLog );
    }

    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmMeasureVariable
//
// Description: Measures a provided variable
//
// Input:       IN      TPM_PCRINDEX              PCRIndex,
//              IN      TCG_EVENTTYPE             EventType,
//              IN      CHAR16                    *VarName,
//              IN      EFI_GUID                  *VendorGuid,
//              IN      VOID                      *VarData,
//              IN      UINTN                     VarSize
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
EFI_STATUS TcmMeasureVariable(
    IN TPM_PCRINDEX  PCRIndex,
    IN TCG_EVENTTYPE EventType,
    IN CHAR16        *VarName,
    IN EFI_GUID      *VendorGuid,
    IN VOID          *VarData,
    IN UINTN         VarSize )
{
    EFI_STATUS            Status;
    EFI_TCM_PROTOCOL      *tcgSvc=NULL;
    TCM_PCR_EVENT         *TcgEvent = NULL;
    UINTN                 VarNameLength;
    TCG_EFI_VARIABLE_DATA *VarLog=NULL;
    EFI_PHYSICAL_ADDRESS  Last;
    UINT32                evNum;

    Status = gBS->LocateProtocol(
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc );

    ASSERT( !EFI_ERROR( Status ));

    VarNameLength = StrLen( VarName );

    Status        = gBS->AllocatePool( EfiBootServicesData,
                                       _TPM_STRUCT_PARTIAL_SIZE( TCM_PCR_EVENT,Event )
                                       + (UINT32)(sizeof(*VarLog) + VarNameLength
                                               * sizeof(*VarName) + VarSize - 3 ),
                                       &TcgEvent );

    if( EFI_ERROR( Status ) || TcgEvent == NULL )return Status;

    TcgEvent->PCRIndex  = PCRIndex;
    TcgEvent->EventType = EventType;
    TcgEvent->EventSize = (UINT32)( sizeof (*VarLog) + VarNameLength
                                    * sizeof (*VarName) + VarSize - 3 );

    Status = gBS->AllocatePool( EfiBootServicesData, TcgEvent->EventSize, &VarLog );

    if ( EFI_ERROR( Status )|| VarLog == NULL )
    {
        gBS->FreePool(TcgEvent);
        return EFI_OUT_OF_RESOURCES;
    }

    VarLog->VariableName       = *VendorGuid;
    VarLog->UnicodeNameLength  = VarNameLength;
    VarLog->VariableDataLength = VarSize;

    gBS->CopyMem(
        (CHAR16*)(VarLog->UnicodeName),
        VarName,
        VarNameLength * sizeof (*VarName)
    );

    gBS->CopyMem(
        (CHAR16*)(VarLog->UnicodeName) + VarNameLength,
        VarData,
        VarSize
    );

    gBS->CopyMem( TcgEvent->Event,
                  VarLog,
                  TcgEvent->EventSize );

    Status = tcgSvc->HashLogExtendEvent(
                 tcgSvc,
                 (EFI_PHYSICAL_ADDRESS)VarLog,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last );

    if ( TcgEvent != NULL )
    {
        gBS->FreePool( TcgEvent );
    }

    ASSERT( !EFI_ERROR( Status ));

    if ( VarLog != NULL )
    {
        gBS->FreePool( VarLog );
    }

    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  confirmUser
//
// Description:  Wait on key press from ConIn; Accept Esc or F10.
//               Timeout if user doesn't respond
//
// INPUT:
//
// OUTPUT:       UINT8
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8 confirmUser( )
{
    EFI_STATUS    Status = EFI_SUCCESS;
    EFI_INPUT_KEY key;
    AMI_CONFIRMATION_OVERRIDE_PROTOCOL *ConfirmOverride;
    EFI_TPL CurrentTpl = 0;
    UINT8   ReturnCode=0;
    UINTN   Count = 0;

    Status = gBS->LocateProtocol( &gAmiOsPpiConfirmationOverrideGuid, NULL, &ConfirmOverride);

    if(!EFI_ERROR(Status))
    {
        return(ConfirmOverride->ConfirmUser());
    }

#if (AUTO_ACCEPT_PPI) == 1
    return TRUE;
#endif

    if ( gST->ConIn )
    {
        while ( Count < 15000)
        {
            CurrentTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);
            gBS->RestoreTPL (TPL_APPLICATION);

            Status =  gBS->CheckEvent( gST->ConIn->WaitForKey );
            if(Status ==  EFI_NOT_READY)
            {
                MicroSecondDelay(20000); //20ms
                Count+=1;
                continue;
            }

            Status = gST->ConIn->ReadKeyStroke( gST->ConIn, &key );

            if ( Status == EFI_SUCCESS )
            {
                if ( ( ppi_request == TCPA_PPIOP_CLEAR_ENACT || ppi_request ==
                        TCPA_PPIOP_CLEAR  ||  ppi_request == TCPA_PPIOP_SETNOPPICLEAR_FALSE
                        || ppi_request == TCPA_PPIOP_ENABLE_ACTV_CLEAR
                        || ppi_request == TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV) )
                {
                    if ( key.ScanCode == TCG_CLEAR_REQUEST_KEY )
                    {
                        ReturnCode = TRUE;
                        goto RestoreTpl;
                    }
                }
                else
                {

                    if ( key.ScanCode == TCG_CONFIGURATION_ACCEPT_KEY )
                    {
                        ReturnCode = TRUE;
                        goto RestoreTpl;
                    }
                }
                if ( key.ScanCode == TCG_CONFIGURATION_IGNORE_KEY )
                {
                    ReturnCode = FALSE;
                    goto RestoreTpl;
                }
            }

            MicroSecondDelay(20000); //20ms
            Count+=1;
        }
    }
    ReturnCode = FALSE;
RestoreTpl:
    gBS->RaiseTPL( TPL_HIGH_LEVEL );
    gBS->RestoreTPL( CurrentTpl );
    return ReturnCode;
}




EFI_STATUS LogTcgEvent( TCG_PCR_EVENT  *TcgEvent,
                        EFI_TCG_PROTOCOL    *tcgSvc)
{
    UINT32 EventNumber;

    if(tcgSvc == NULL) return EFI_INVALID_PARAMETER;

    return( tcgSvc->LogEvent(tcgSvc,
                             TcgEvent,
                             &EventNumber,
                             0x01));

}


EFI_STATUS ExtendEvent(
    IN EFI_TCG_PROTOCOL    *tcgSvc,
    IN TPM_PCRINDEX         PCRIndex,
    IN TCG_DIGEST           *Digest,
    OUT TCG_DIGEST          *NewPCRValue )
{
    struct
    {
        TPM_1_2_CMD_HEADER  hdr;
        TPM_PCRINDEX        PcrIndex;
        UINT8               Digest[20];
    } ExtendCmd;

    UINT8 result[100];

    if(tcgSvc == NULL) return EFI_INVALID_PARAMETER;

    ExtendCmd.hdr.Tag = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    ExtendCmd.hdr.ParamSize = TPM_H2NL(sizeof(TPM_1_2_CMD_HEADER) + sizeof(TPM_PCRINDEX) + 20);
    ExtendCmd.hdr.Ordinal = TPM_H2NL( TPM_ORD_Extend );
    ExtendCmd.PcrIndex    = TPM_H2NL( PCRIndex );

    gBS->CopyMem( ExtendCmd.Digest,
                  Digest->digest,
                  20);

    return (tcgSvc->PassThroughToTpm( tcgSvc,
                                      sizeof(TPM_1_2_CMD_HEADER) + sizeof(TPM_PCRINDEX) + 20,
                                      (UINT8 *)&ExtendCmd,
                                      sizeof (result),
                                      (UINT8*)&result));
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureSeparatorEvent
//
// Description: Measure Tcg Event Separator
//
// Input:       IN      TPM_PCRINDEX              PCRIndex,
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
EFI_STATUS MeasureSeparatorEvent(
    IN TPM_PCRINDEX PCRIndex )
{
    TCG_PCR_EVENT        *TcgEvent = NULL;
    EFI_TCG_PROTOCOL    *tcgSvc = NULL;
    UINT32               evNum;
    EFI_PHYSICAL_ADDRESS Last;
    EFI_STATUS           Status;
    UINT8                SeparatorInfo[36];
#if USE_ZERO_SEPARATOR == 1
    UINT32               EventData = 0;
#else
    UINT32               EventData = -1;
#endif

    Status = gBS->LocateProtocol(
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc );

    TcgEvent = (TCG_PCR_EVENT *)SeparatorInfo;

    TcgEvent->PCRIndex  = PCRIndex;
    TcgEvent->EventType = EV_SEPARATOR;
    TcgEvent->EventSize = (UINT32)sizeof (EventData);

    gBS->CopyMem( TcgEvent->Event,
                  &EventData,
                  TcgEvent->EventSize);

    Status = tcgSvc->HashLogExtendEvent(
                 tcgSvc,
                 (EFI_PHYSICAL_ADDRESS)&EventData,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last );

    return Status;

}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmMeasureSeparatorEvent
//
// Description: Measure Tcg Event Separator
//
// Input:       IN      TPM_PCRINDEX              PCRIndex,
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
EFI_STATUS TcmMeasureSeparatorEvent(
    IN TPM_PCRINDEX PCRIndex )
{
    TCM_PCR_EVENT        *TcgEvent = NULL;
    EFI_TCM_PROTOCOL     *tcgSvc;
    UINT32               evNum;
    EFI_PHYSICAL_ADDRESS Last;
    EFI_STATUS           Status;
#if USE_ZERO_SEPARATOR == 1
    UINT32               EventData = 0;
#else
    UINT32               EventData = -1;
#endif

    Status = gBS->LocateProtocol(
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc );

    ASSERT( !EFI_ERROR( Status ));

    Status = gBS->AllocatePool( EfiBootServicesData,
                                _TPM_STRUCT_PARTIAL_SIZE( TCM_PCR_EVENT, Event )
                                + sizeof (EventData),
                                &TcgEvent );

    if( EFI_ERROR( Status ) || TcgEvent == NULL )return Status;

    TcgEvent->PCRIndex  = PCRIndex;
    TcgEvent->EventType = EV_SEPARATOR;
    TcgEvent->EventSize = (UINT32)sizeof (EventData);

    gBS->CopyMem( TcgEvent->Event,
                  &EventData,
                  sizeof (EventData));

    Status = tcgSvc->HashLogExtendEvent(
                 tcgSvc,
                 (EFI_PHYSICAL_ADDRESS)&EventData,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last );

    if ( TcgEvent != NULL )
    {
        gBS->FreePool( TcgEvent );
    }

    return Status;
}







//**********************************************************************
//<AMI_PHDR_START>
//
// Name: doCpuMicrocodeTcgEvent
//
// Description: Measures EV_CPU_MICROCODE event
//
// Input:       IN      Buffer
//              IN      size
//
// Output:      Device path size
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS doCpuMicrocodeTcgEvent(
    IN VOID  * buffer,
    IN UINTN size )
{
    EFI_STATUS        Status;
    EFI_TCG_PCR_EVENT ev;
    UINT32            EventNum;
    EFI_TCG_PROTOCOL  *tcg;


    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcg );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    ev.Header.PCRIndex               = 1;
    ev.Header.EventType              = EV_CPU_MICROCODE;
    ev.Header.EventDataSize          = sizeof(ev.Event.Mcu);
    ev.Event.Mcu.MicrocodeEntrypoint = (EFI_PHYSICAL_ADDRESS)( UINTN ) buffer;

    Status = tcg->HashLogExtendEvent(
                 tcg,
                 ev.Event.Mcu.MicrocodeEntrypoint,
                 (UINT64)size,
                 TCG_ALG_SHA,
                 (TCG_PCR_EVENT*)&ev,
                 &EventNum,
                 0
             );
    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Name: doTcmCpuMicrocodeTcgEvent
//
// Description: Measures EV_CPU_MICROCODE event
//
// Input:       IN      Buffer
//              IN      size
//
// Output:      Device path size
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS doTcmCpuMicrocodeTcgEvent(
    IN VOID  * buffer,
    IN UINTN size )
{
    EFI_STATUS        Status;
    EFI_TCM_PCR_EVENT ev;
    UINT32            EventNum;
    EFI_TCM_PROTOCOL  *tcg;


    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcg );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    ev.Header.PCRIndex               = 1;
    ev.Header.EventType              = EV_CPU_MICROCODE;
    ev.Header.EventDataSize          = sizeof(ev.Event.Mcu);
    ev.Event.Mcu.MicrocodeEntrypoint = (EFI_PHYSICAL_ADDRESS)( UINTN ) buffer;

    Status = tcg->HashLogExtendEvent(
                 tcg,
                 ev.Event.Mcu.MicrocodeEntrypoint,
                 (UINT64)size,
                 TCG_ALG_SHA,
                 (TCM_PCR_EVENT*)&ev,
                 &EventNum,
                 0
             );
    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Name: measureCpuMicroCode
//
// Description: Locates CPU Microcode update and measures it as a TCG event
//
// Input:       NONE
//
// Output:      Device path size
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS MeasureCpuMicroCode( )
{
    EFI_STATUS                   Status;
    VOID                         *KeyBuffer= NULL;
    EFI_GUID                     FileName;
    EFI_FV_FILE_ATTRIBUTES       FileAttr;
    UINTN                        FileSize;
    VOID                         *FileBuffer=NULL;
    VOID                         *FileBufS=NULL;
    UINT32                       AuthStat;
    EFI_HANDLE                   *Handles=NULL;
    UINTN                        NumHandles;
    EFI_FIRMWARE_VOLUME_PROTOCOL *FwVol=NULL;
    EFI_FIRMWARE_VOLUME_HEADER   *FvHdr=NULL;
    BOOLEAN                      Nested = TRUE;

    UINT32                       gMicrocodeFlashSize =0;
    EFI_FV_FILETYPE                             FileType;
    AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL      *POverride=NULL;

    Status = gBS->LocateProtocol(
                 &gAmiMeasureCPUMicrocodeGuid,
                 NULL,
                 &POverride );

    if(!EFI_ERROR(Status))
    {
        return (POverride->Function());
    }

    Status = gBS->LocateHandleBuffer(
                 ByProtocol,
                 &gEfiFirmwareVolumeProtocolGuid,
                 NULL,
                 &NumHandles,
                 &Handles
             );
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    for (; NumHandles > 0; NumHandles-- )
    {
        Status = gBS->HandleProtocol(
                     Handles[NumHandles - 1],
                     &gEfiFirmwareVolumeProtocolGuid,
                     &FwVol
                 );

        if ( EFI_ERROR( Status ))
        {
            break;
        }

        Status = gBS->AllocatePool( EfiBootServicesData,
                                    FwVol->KeySize,
                                    &KeyBuffer );

        if ( KeyBuffer == NULL )
        {
            return EFI_OUT_OF_RESOURCES;
        }
        gBS->SetMem( KeyBuffer, FwVol->KeySize, 0 );

        do
        {
            FileType = EFI_FV_FILETYPE_RAW;
            Status   = FwVol->GetNextFile(
                           FwVol,
                           KeyBuffer,
                           &FileType,
                           &FileName,
                           &FileAttr,
                           &FileSize );

            if ( !EFI_ERROR( Status )
                    && CompareMem( &FileName, &gMicrocodeGuid,
                                   sizeof(gMicrocodeGuid)) == 0 )
            {
                Nested     = FALSE;
                FileBuffer = NULL;
                Status     = FwVol->ReadFile(
                                 FwVol,
                                 &FileName,
                                 &FileBuffer,
                                 &FileSize,
                                 &FileType,
                                 &FileAttr,
                                 &AuthStat
                             );
                ASSERT( !EFI_ERROR( Status ));

                DEBUG((DEBUG_INFO, "CPU Microcode found: %x size %x\n",
                       FileBuffer, FileSize));

                if(IsTcmSupportType())
                {
                    Status = doTcmCpuMicrocodeTcgEvent( FileBuffer, FileSize );
                }
                else
                {
                    Status = doCpuMicrocodeTcgEvent( FileBuffer, FileSize );
                }

                DEBUG((DEBUG_INFO, "\tMeasured: %x\n", Status));
                gBS->FreePool( FileBuffer );
                goto Exit;
            }
        }
        while ( !EFI_ERROR( Status ));
    }
Exit:
    gBS->FreePool( Handles );

    if ( KeyBuffer )
    {
        gBS->FreePool( KeyBuffer );
    }
    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   SendTpmCommand
//
// Description: Sends a command to the TPM
//
// Input:       IN      EFI_TCG_PROTOCOL *tcg,
//              IN      UINT32           ord,
//              IN      int dataSize,
//              IN OUT  VOID  *data
//
// Output:      TPM_RESULT
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************s
TPM_RESULT SendTpmCommand(
    IN      EFI_TCG_PROTOCOL *tcg,
    IN      UINT32           ord,
    IN      int              dataSize,
    IN OUT  void             * data )
{
    EFI_STATUS         Status;
    static UINT8       result[0x400];
    TPM_1_2_RET_HEADER * tpmResult;

    struct
    {
        TPM_1_2_CMD_HEADER hdr;
        UINT8              data[0x100];
    } cmd;

    cmd.hdr.Tag       = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmd.hdr.ParamSize = TPM_H2NL( sizeof (TPM_1_2_RET_HEADER) + dataSize );
    cmd.hdr.Ordinal   = TPM_H2NL( ord );

    gBS->CopyMem( cmd.data, data, dataSize );

    Status = tcg->PassThroughToTpm( tcg,
                                    sizeof (TPM_1_2_CMD_HEADER) + dataSize,
                                    (UINT8*)&cmd,
                                    sizeof (result),
                                    (UINT8*)&result );

    tpmResult = (TPM_1_2_RET_HEADER*)result;
    DEBUG((DEBUG_INFO, "TCG: TPM Result = (%x)\n",
           tpmResult->RetCode));

    DEBUG((DEBUG_INFO, "TCG: TPM Status = (%r)\n",Status));

    return tpmResult->RetCode;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   DoResetNow
//
// Description: Callback function to execute TPM reset
//
// Input:       IN EFI_EVENT ev,
//              IN VOID      *ctx
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
EFI_STATUS DoResetNow(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    EFI_RESET_TYPE resetType = *(EFI_RESET_TYPE*)ctx;

    DEBUG((DEBUG_INFO, "TCG: DoResetNow resets the system: type(%d)\n",
           resetType));

    DEBUG((DEBUG_INFO, "TCG: DoResetNow resets the system: type(%x)\n",
           (EFI_RESET_TYPE*)ctx));

    gRT->ResetSystem( resetType, 0, 0, NULL );
    DEBUG((DEBUG_ERROR, "\tError: Reset failed???\n"));
    return EFI_SUCCESS;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GET_PFA
//
// Description: Returns PCI device Bus Device Function infomation
//
//
// Input:       EFI_PCI_IO_PROTOCOL   *pciIo
//
// Output:      UINT16
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT16 GET_PFA(
    IN EFI_PCI_IO_PROTOCOL *pciIo )
{
    UINTN      seg;
    UINTN      bus;
    UINTN      dev;
    UINTN      func;
    EFI_STATUS Status;

    Status = pciIo->GetLocation( pciIo, &seg, &bus, &dev, &func );
    ASSERT( !EFI_ERROR( Status ));

    if (     EFI_ERROR( Status ))
    {
        return 0;
    }
    ASSERT( func < 8 );
    ASSERT( dev < 32 );
    ASSERT( bus < 256 );
    return (UINT16)((bus << 8) | (dev << 3) | func );
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   ClearFastBootLastBootFailedFlag
//
// Description: FastBoot clear boot fail flag callback
//
// Input:
//  IN EFI_EVENT Event - Callback event
//  IN VOID *Context - pointer to calling context
//
// Output:      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ClearFastBootLastBootFailedFlag()
{
    EFI_STATUS Status;
    UINT32 LastBootFailed;
    UINTN Size = sizeof(UINT32);

    Status = gRT->GetVariable(L"LastBootFailed", &FastBootVariableGuid, NULL, &Size, &LastBootFailed);
    if(!EFI_ERROR(Status))
    {
        Status = gRT->SetVariable(L"LastBootFailed",
                                  &FastBootVariableGuid,
                                  EFI_VARIABLE_NON_VOLATILE,
                                  0,
                                  &LastBootFailed);

        DEBUG((DEBUG_INFO, "Status : = %r\n", Status));

    }
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   RequestSystemReset
//
// Description: Requests for system reset through core else creates a call
//                back to execute reset
//
// Input:       IN EFI_RESET_TYPE type
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
void RequestSystemReset(
    IN EFI_RESET_TYPE type )
{
    EFI_STATUS            Status;
    static EFI_EVENT      ev;
    static void           * reg;
    static EFI_RESET_TYPE ptype;

    ptype = type;

    DEBUG((DEBUG_INFO, "TCG: Resets the system: type(%d)\n", type));
    DEBUG((DEBUG_INFO, "TCG: Resets the system: typeaddress(%x)\n",
           &type));

    ClearFastBootLastBootFailedFlag();

    gRT->ResetSystem( type, 0, 0, NULL );
    DEBUG((DEBUG_ERROR, "\tError: Reset failed???\n"));

    Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                               EFI_TPL_CALLBACK, DoResetNow, (void*)&ptype, &ev);

    if(EFI_ERROR(Status))return;
    ASSERT( !EFI_ERROR( Status ));
    Status = gBS->RegisterProtocolNotify(&gEfiResetArchProtocolGuid, ev, &reg);
    DEBUG((DEBUG_INFO,
           "\tRegister DoResetNow after Reset Architecture driver\n"));
    if(EFI_ERROR(Status))return;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   read_TPM_capabilities
//
// Description: Executes TPM operation to read capabilities
//
// Input:       IN EFI_TCG_PROTOCOL* tcg
//
// Output:      TPM capabilities structure
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
TPM_Capabilities_PermanentFlag read_TPM_capabilities(
    IN EFI_TCG_PROTOCOL* tcg )
{
    TPM_Capabilities_PermanentFlag * cap;
    EFI_STATUS                     Status;
    TPM_GetCapabilities_Input      cmdGetCap;
    static UINT8                   result[0x100];

    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   = TPM_H2NL( sizeof (cmdGetCap));

    if(IsTcmSupportType())
    {
        cmdGetCap.CommandCode = TPM_H2NL( TCM_ORD_GetCapability );
        cmdGetCap.CommandCode = TPM_H2NL( TCM_ORD_GetCapability );
        cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    }
    else
    {
        cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability );
        cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability );
        cmdGetCap.caparea     = TPM_H2NL( TPM_CAP_FLAG );
    }

    cmdGetCap.subCapSize  = TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      = TPM_H2NL( TPM_CAP_FLAG_PERMANENT );

    Status = tcg->PassThroughToTpm( tcg,
                                    sizeof (cmdGetCap),
                                    (UINT8*)&cmdGetCap,
                                    sizeof (result),
                                    result );

    cap = (TPM_Capabilities_PermanentFlag*)result;

    DEBUG((DEBUG_INFO,
           "GetCapability: %r; size: %x; retCode:%x; tag:%x; bytes %08x\n", Status,
           TPM_H2NL(cap->ParamSize ), TPM_H2NL(cap->RetCode ),
           (UINT32) TPM_H2NS(cap->tag ), TPM_H2NL( *(UINT32*)&cap->disabled )));

    return *cap;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   CheckTpmOwnership
//
// Description: Executes TPM operation to read capabilities
//
// Input:       IN EFI_TCG_PROTOCOL *tcg
//
// Output:      void
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8 CheckTpmOwnership(
    IN EFI_TCG_PROTOCOL *tcg )
{
    EFI_STATUS                Status;
    TPM_GetCapabilities_Input cmdGetCap;
    static UINT8              Ownership_Result[0x200];
    UINT8                     TPM_Ownership;

    cmdGetCap.Tag         = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    cmdGetCap.ParamSize   =      TPM_H2NL( sizeof (cmdGetCap));

    if(IsTcmSupportType())
    {
        cmdGetCap.CommandCode = TPM_H2NL( TCM_ORD_GetCapability );
    }
    else
    {
        cmdGetCap.CommandCode = TPM_H2NL( TPM_ORD_GetCapability );
    }

    cmdGetCap.caparea     =      TPM_H2NL( TPM_CAP_PROPERTY );
    cmdGetCap.subCapSize  =      TPM_H2NL( 4 ); // subCap is always 32bit long
    cmdGetCap.subCap      =      TPM_H2NL( TPM_CAP_PROP_OWNER );
    Status                = tcg->PassThroughToTpm( tcg,
                            sizeof (cmdGetCap),
                            (UINT8*)&cmdGetCap,
                            sizeof (Ownership_Result),
                            Ownership_Result );
    if(EFI_ERROR(Status))return 0;
    TPM_Ownership = (UINT8 )Ownership_Result[0x0E];
    return TPM_Ownership;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TpmSetCapabilityDefferedPP
//
// Description: Executes TPM operation to set Capability
//              Set TPM_STCLEAR_DATA->deferredPhysicalPresence to
//              TPM_SetCapability->setValue.
//
// Input:       IN EFI_TCG_PROTOCOL *tcg
//
// Output:      TPM_RESULT
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
TPM_RESULT TpmSetCapabilityDefferedPP(
    IN EFI_TCG_PROTOCOL *tcg )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    TPM_SetCapability_Input     SetCapFwUpgrade;
    TPM_1_2_RET_HEADER          *pRetHeader = NULL;
    UINT8                       retHdrBuffer[0x100];

    SetCapFwUpgrade.tag             = TPM_H2NS( TPM_TAG_RQU_COMMAND );
    SetCapFwUpgrade.paramSize       = TPM_H2NL( sizeof(SetCapFwUpgrade) );
    SetCapFwUpgrade.ordinal         = TPM_H2NL( TPM_ORD_SetCapability );
    SetCapFwUpgrade.capArea         = TPM_H2NL( TPM_SET_STCLEAR_DATA );
    SetCapFwUpgrade.subCapSize      = TPM_H2NL( 4 );
    SetCapFwUpgrade.subCap          = TPM_H2NL( TPM_SD_DEFERREDPHYSICALPRESENCE );
    SetCapFwUpgrade.setValueSize    = TPM_H2NL( 4 );
    SetCapFwUpgrade.setValue        = TPM_H2NL( 0x01 ); //bit 0: unownedFieldUpgrade

    pRetHeader = (TPM_1_2_RET_HEADER*)&retHdrBuffer[0];

    Status = tcg->PassThroughToTpm( tcg,
                                    sizeof (SetCapFwUpgrade),
                                    (UINT8*)&SetCapFwUpgrade,
                                    sizeof(retHdrBuffer),
                                    (UINT8*)pRetHeader );

    DEBUG((DEBUG_INFO, "SetCapFwUpgrade: %r; size: %x; retCode:%x; tag:%x\n", Status,
           TPM_H2NL(pRetHeader->ParamSize), TPM_H2NL(pRetHeader->RetCode),
           (UINT32) TPM_H2NS(pRetHeader->Tag) ));

    return pRetHeader->RetCode;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   EfiLibGetSystemConfigurationTable
//
// Description: Get table from configuration table by name
//
// Input:       IN EFI_GUID *TableGuid,
//              IN OUT VOID **Table
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
EFI_STATUS EfiLibGetSystemConfigurationTable(
    IN EFI_GUID *TableGuid,
    IN OUT VOID **Table )
{
    UINTN Index;

    *Table = NULL;

    for ( Index = 0; Index < gST->NumberOfTableEntries; Index++ )
    {
        if ( !CompareMem( TableGuid, &(gST->ConfigurationTable[Index].VendorGuid),
                          sizeof(EFI_GUID)))
        {
            *Table = gST->ConfigurationTable[Index].VendorTable;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}





//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureHandoffTables
//
// Description: Measures, Logs and Extends EFI Handoff Tables. eg: SMBIOS
//
// Input:       VOID
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
EFI_STATUS MeasureHandoffTables()
{
    EFI_STATUS                     Status = EFI_SUCCESS;    
#if (defined(Measure_Smbios_Tables) && (Measure_Smbios_Tables!= 0))
    EFI_TCG_PROTOCOL               *tcgSvc;
    TCG_PCR_EVENT                  *ev = NULL;
    TCG_EFI_HANDOFF_TABLE_POINTERS HandoffTables;
    SMBIOS_TABLE_ENTRY_POINT       *SmbiosTable;
    EFI_PHYSICAL_ADDRESS           Last;
    UINT32                         evNum;

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid,
                                  NULL,
                                  &tcgSvc );
    ASSERT( !EFI_ERROR( Status ));

    Status = EfiLibGetSystemConfigurationTable(
                 &gEfiSmbiosTableGuid,
                 &SmbiosTable
             );

    if ( !EFI_ERROR( Status ))
    {
        Status = gBS->AllocatePool( EfiBootServicesData,
                                    _TPM_STRUCT_PARTIAL_SIZE( TCG_PCR_EVENT, Event )
                                    + sizeof (HandoffTables), &ev );

        ASSERT( !EFI_ERROR( Status ));

        ev->PCRIndex  = 1;
        ev->EventType = EV_EFI_HANDOFF_TABLES;
        ev->EventSize = sizeof (HandoffTables);

        HandoffTables.NumberOfTables            = 1;
        HandoffTables.TableEntry[0].VendorGuid  = gEfiSmbiosTableGuid;
        HandoffTables.TableEntry[0].VendorTable = SmbiosTable;

        gBS->CopyMem( ev->Event,
                      &HandoffTables,
                      sizeof (HandoffTables));

        Status = tcgSvc->HashLogExtendEvent(
                     tcgSvc,
                     (EFI_PHYSICAL_ADDRESS)SmbiosTable->TableAddress,
                     SmbiosTable->TableLength,
                     TCG_ALG_SHA,
                     ev,
                     &evNum,
                     &Last );
    }

    if ( ev != NULL )
    {
        gBS->FreePool( ev );
    }
#endif
    return Status;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmMeasureHandoffTables
//
// Description: Measures, Logs and Extends EFI Handoff Tables. eg: SMBIOS
//
// Input:       VOID
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
EFI_STATUS TcmMeasureHandoffTables(
    VOID )
{
    return EFI_SUCCESS;
}




//----------------------------------------------------------------------------
// Procedure:   TcgMeasureAction
// Description: Measure a Tcg Action
// INPUT:       String
// OUTPUT:      EFI_STATUS
//----------------------------------------------------------------------------
EFI_STATUS
EFIAPI
TcgMeasureActionI (
    IN      CHAR8             *String,
    IN      UINT32            PCRIndex
)
{

    TCG_PCR_EVENT                     *TcgEvent = NULL;
    EFI_PHYSICAL_ADDRESS              Last;
    EFI_TCG_PROTOCOL                  *tcgSvc;
    UINT32                            evNum;
    UINT32                            Len;
    EFI_STATUS                        Status;


    Status = gBS->LocateProtocol (
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc);

    ASSERT(!EFI_ERROR(Status));

    Len = (UINT32)AsciiStrLen(String);
    Status = gBS->AllocatePool (EfiBootServicesData,
                                _TPM_STRUCT_PARTIAL_SIZE (TCG_PCR_EVENT, Event) +
                                Len,
                                &TcgEvent);

    if( EFI_ERROR( Status ) || TcgEvent == NULL )return Status;

    TcgEvent->PCRIndex    = PCRIndex;
    TcgEvent->EventType   = EV_EFI_ACTION;
    TcgEvent->EventSize   = Len;

    gBS->CopyMem (TcgEvent->Event,
                  String,
                  Len);

    Status = tcgSvc->HashLogExtendEvent (
                 tcgSvc,
                 (EFI_PHYSICAL_ADDRESS)String,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last);

    if(TcgEvent!=NULL)
    {
        gBS->FreePool (TcgEvent);
    }

    return Status;
}



//----------------------------------------------------------------------------
// Procedure:   TcgMeasureAction
// Description: Measure a Tcg Action
// INPUT:       String
// OUTPUT:      EFI_STATUS
//----------------------------------------------------------------------------
EFI_STATUS
EFIAPI
TcmMeasureActionI (
    IN      CHAR8             *String,
    IN      UINT32            PCRIndex
)
{

    TCM_PCR_EVENT                     *TcgEvent = NULL;
    EFI_PHYSICAL_ADDRESS              Last;
    EFI_TCM_PROTOCOL                  *tcgSvc;
    UINT32                            evNum;
    UINT32                            Len;
    EFI_STATUS                        Status;


    Status = gBS->LocateProtocol (
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc);

    ASSERT(!EFI_ERROR(Status));

    Len = (UINT32)AsciiStrLen(String);
    Status = gBS->AllocatePool (EfiBootServicesData,
                                _TPM_STRUCT_PARTIAL_SIZE (TCM_PCR_EVENT, Event) +
                                Len,
                                &TcgEvent);

    ASSERT(!EFI_ERROR(Status));

    TcgEvent->PCRIndex    = PCRIndex;
    TcgEvent->EventType   = EV_EFI_ACTION;
    TcgEvent->EventSize   = Len;

    gBS->CopyMem (TcgEvent->Event,
                  String,
                  Len);

    Status = tcgSvc->HashLogExtendEvent (
                 tcgSvc,
                 (EFI_PHYSICAL_ADDRESS)String,
                 TcgEvent->EventSize,
                 TCG_ALG_SHA,
                 TcgEvent,
                 &evNum,
                 &Last);

    if(TcgEvent!=NULL)
    {
        gBS->FreePool (TcgEvent);
    }

    return Status;
}





//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ReadVariable
//
// Description: Reads Boot Variable
//
// Input:        IN      CHAR16      *VarName,
//               IN      EFI_GUID    *VendorGuid,
//               OUT     UINTN       *VarSize
//
// Output:      VOID*
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
VOID* ReadVariable(
    IN CHAR16   *VarName,
    IN EFI_GUID *VendorGuid,
    OUT UINTN   *VarSize )
{
    EFI_STATUS Status;
    VOID       *VarData=NULL;

    *VarSize = 0;
    Status   = gRT->GetVariable(
                   VarName,
                   VendorGuid,
                   NULL,
                   VarSize,
                   NULL
               );

    if ( Status != EFI_BUFFER_TOO_SMALL )
    {
        return NULL;
    }


    gBS->AllocatePool( EfiBootServicesData, *VarSize, &VarData );

    if ( VarData != NULL )
    {
        Status = gRT->GetVariable(
                     VarName,
                     VendorGuid,
                     NULL,
                     VarSize,
                     VarData
                 );

        if ( EFI_ERROR( Status ))
        {
            gBS->FreePool( VarData );
            VarData  = NULL;
            *VarSize = 0;
        }
    }
    return VarData;
}



EFI_STATUS Tpm12PlatformHashConfig()
{
    return EFI_SUCCESS;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ReadAndMeasureBootVariable
//
// Description: Read and Measures Boot Variable
//
// Input:       IN      CHAR16      *VarName,
//              IN      EFI_GUID    *VendorGuid,
//              OUT     UINTN       *VarSize
//              OUT     VOID        **VarData
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
EFI_STATUS ReadAndMeasureBootVariable(
    IN CHAR16   *VarName,
    IN EFI_GUID *VendorGuid,
    OUT UINTN   *VarSize,
    OUT VOID    **VarData )
{
    EFI_STATUS Status;

    *VarData = ReadVariable( VarName, VendorGuid, VarSize );

    if ( *VarData == NULL )
    {
        return EFI_NOT_FOUND;
    }

    if(IsTcmSupportType())
    {
        Status = TcmMeasureVariable(
                     PCR_FOR_BOOT_VARIABLES,
                     EV_EFI_VARIABLE_BOOT,
                     VarName,
                     VendorGuid,
                     *VarData,
                     *VarSize);

    }
    else
    {
        Status = MeasureVariable(
                     PCR_FOR_BOOT_VARIABLES,
                     EV_EFI_VARIABLE_BOOT,
                     VarName,
                     VendorGuid,
                     *VarData,
                     *VarSize);
    }
    return Status;
}


#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))

UINTN
TcgGetImageExeInfoTableSize (
    EFI_IMAGE_EXECUTION_INFO_TABLE        *ImageExeInfoTable
)
{
    UINTN                     Index;
    EFI_IMAGE_EXECUTION_INFO  *ImageExeInfoItem;
    UINTN                     TotalSize;

    if (ImageExeInfoTable == NULL)
    {
        return 0;
    }

    ImageExeInfoItem  = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoTable + sizeof (EFI_IMAGE_EXECUTION_INFO_TABLE));
    TotalSize         = sizeof (EFI_IMAGE_EXECUTION_INFO_TABLE);
    for (Index = 0; Index < ImageExeInfoTable->NumberOfImages; Index++)
    {
        TotalSize += ImageExeInfoItem->InfoSize;
        ImageExeInfoItem = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoItem + ImageExeInfoItem->InfoSize);
    }

    return TotalSize;
}


EFI_STATUS MeasureSecurebootVariables(
    VOID
)
{
    EFI_STATUS      Status;
    UINT8           *VarData = NULL;
    CHAR16          *SecurityVar[]=
    {
        L"SecureBoot",
        L"PK",
        L"KEK",
        L"db",
        L"dbx",
        L"ImageExecTable"
    };

    EFI_GUID        Varguid;
    TCG_EVENTTYPE   Eventtypes[]=
    {
        EV_EFI_VARIABLE_DRIVER_CONFIG,
        EV_EFI_VARIABLE_DRIVER_CONFIG,
        EV_EFI_VARIABLE_DRIVER_CONFIG,
        EV_EFI_VARIABLE_DRIVER_CONFIG,
        EV_EFI_VARIABLE_DRIVER_CONFIG,
        EV_EFI_EXECUTION_INFO_TABLE
    };

    UINTN           VarSize  = 0;
    UINTN           i=0;
    TCG_PCR_EVENT        *TcgEvent = NULL;
    EFI_TCG_PROTOCOL     *tcgSvc;
#if 0
    UINT32               evNum;
    EFI_PHYSICAL_ADDRESS Last;
    EFI_SIGNATURE_LIST   *pList;
    UINTN                Tablesize;
#endif
    EFI_IMAGE_EXECUTION_INFO_TABLE  *ImageExeInfoTable = NULL;
    static BOOLEAN        Skip = FALSE;

    Status = gBS->LocateProtocol(
                 &gEfiTcgProtocolGuid,
                 NULL,
                 &tcgSvc );

    ASSERT( !EFI_ERROR( Status ));

    for(i=0; i<5; i++)
    {
        if(i==3 || i==4)
        {
            Varguid = gEfiImageSecurityDatabaseguid;
        }
        else
        {
            Varguid = gEfiGlobalVariableGuid;
        }

        VarData = ReadVariable( SecurityVar[i], &Varguid, &VarSize );

        if ( VarData == NULL )
        {
#if (defined(UnconfiguredSecureBootVariables) && (UnconfiguredSecureBootVariables != 0))
            VarSize = 0;
#else
            continue;
#endif
        }

#if (defined(UnconfiguredSecureBootVariables) && (UnconfiguredSecureBootVariables == 0))
        if( i == 0 && ((UINT8)(*VarData)) == 0) Skip = TRUE;
        if (Skip == TRUE) continue;
#endif
        // Add for TCM MeasureVariable, fix for some TCM platform hanging problem.
        if(!IsTcmSupportType())
        {
            Status = MeasureVariable(PCRi_HOST_PLATFORM_MANUFACTURER_CONTROL,
                                 Eventtypes[i],
                                 SecurityVar[i],
                                 &Varguid,
                                 VarData,
                                 VarSize);
        }
        else
        {
            Status = TcmMeasureVariable(
                            PCRi_HOST_PLATFORM_MANUFACTURER_CONTROL,
                            Eventtypes[i],
                            SecurityVar[i],
                            &Varguid,
                            VarData,
                            VarSize);
        }

        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_SECURE_BOOT_VARIABLES_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);
        gBS->FreePool(VarData);
    }

    return Status;
}


#endif


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasureAllBootVariables
//
// Description: Measures all the boot Variables
//
// Input:
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
EFI_STATUS MeasureAllBootVariables(
    VOID )
{
    EFI_STATUS Status;
    CHAR16     BootVarName[] = L"BootOrder";
    UINT16     *BootOrder;
    UINTN      BootCount;
    UINTN      Index;
    VOID       *BootVarData = NULL;
    UINTN      Size;

    Status = ReadAndMeasureBootVariable(
                 BootVarName,
                 &gEfiGlobalVariableGuid,
                 &BootCount,
                 &BootOrder
             );

    if ( Status == EFI_NOT_FOUND )
    {
        return EFI_SUCCESS;
    }

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    BootCount /= sizeof (*BootOrder);

    for ( Index = 0; Index < BootCount; Index++ )
    {

#if defined (TSE_CAPITAL_BOOT_OPTION) && (TSE_CAPITAL_BOOT_OPTION == 0)
        UnicodeSPrint( BootVarName, sizeof(BootVarName), L"Boot%04x",
                       BootOrder[Index] );
#else
        UnicodeSPrint( BootVarName, sizeof(BootVarName), L"Boot%04X",
                       BootOrder[Index] );
#endif

        Status = ReadAndMeasureBootVariable(
                     BootVarName,
                     &gEfiGlobalVariableGuid,
                     &Size,
                     &BootVarData
                 );

        if ( !EFI_ERROR( Status ))
        {
            if ( BootVarData != NULL )
            {
                gBS->FreePool( BootVarData );
            }
        }
    }

    gBS->FreePool( BootOrder );
    return EFI_SUCCESS;
}




#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetPCIOPROMImage
//
// Description: Finds Onboard Option ROM Images
//
//
// Input:       IN  EFI_HANDLE   PciHandle,
//              OUT VOID         *RomImage,
//              OUT UINTN        *Romsize,
//              OUT UINTN        *Flags
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
EFI_STATUS GetPCIOPROMImage(
    IN EFI_HANDLE PciHandle,
    OUT VOID      *RomImage,
    OUT UINTN     *Romsize,
    OUT UINTN     *Flags )
{
    EFI_LEGACY_BIOS_PROTOCOL *LgBP;
    EFI_STATUS               status;

    status = gBS->LocateProtocol(
                 &gEfiLegacyBiosProtocolGuid,
                 NULL,
                 &LgBP );

    if(EFI_ERROR(status))return status;

    status = LgBP->CheckPciRom( LgBP,
                                PciHandle,
                                RomImage,
                                Romsize,
                                Flags );
    return status;
}
#endif


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasurePciOptionRom
//
// Description: Measures EV_ID_OPROM_EXECUTE event
//              address (PFA) and with digest of the specified PCI device
//              OpRom image
//
//
// Input:       IN VOID  *pImage,
//              IN UINTN len,
//              IN UINT16   pfa
// Output:
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS MeasurePciOptionRom(
    IN VOID  *pImage,
    IN UINTN len,
    UINT16   pfa   )
{
    EFI_STATUS           Status;
    EFI_TCG_PROTOCOL     *tcgSvc;
    EFI_TCM_PROTOCOL     *tcmSvc;
    EFI_TCG_PCR_EVENT    ev;
    EFI_TCM_PCR_EVENT    TcmEvent;
    UINT32               evNum;
    EFI_PHYSICAL_ADDRESS Last;
    UINT64               digestLen;
    UINT8                *pDigest;

    DEBUG((DEBUG_INFO, "Measuring image @ %x, image len = %x, pfa = %x  \n", pImage, len, pfa));

    if(!IsTcmSupportType())
    {
        Status = gBS->LocateProtocol(
                     &gEfiTcgProtocolGuid,
                     NULL,
                     &tcgSvc
                 );

        if ( EFI_ERROR( Status ))
        {
            return EFI_SUCCESS;
        }

        ev.Header.PCRIndex      = PCRi_OPROM_CODE;
        ev.Header.EventType     = EV_EVENT_TAG;
        ev.Event.Tagged.EventID = EV_ID_OPROM_EXECUTE;
        ev.Event.Tagged.EventSize =
            sizeof(ev.Event.Tagged.EventData.OptionRomExecute);
        ev.Header.EventDataSize = _TPM_STRUCT_PARTIAL_SIZE(struct _EFI_TCG_EV_TAG,
                                  EventData ) + ev.Event.Tagged.EventSize;

        ev.Event.Tagged.EventData.OptionRomExecute.PFA      = pfa;
        ev.Event.Tagged.EventData.OptionRomExecute.Reserved = 0;
        digestLen = sizeof (ev.Event.Tagged.EventData.OptionRomExecute.Hash);
        pDigest   = ev.Event.Tagged.EventData.OptionRomExecute.Hash.digest;

        Status = tcgSvc->HashAll(
                     tcgSvc,
                     (UINT8*)pImage,
                     len,
                     TCG_ALG_SHA,
                     &digestLen,
                     &pDigest);
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }

        Status = tcgSvc->HashLogExtendEvent(
                     tcgSvc,
                     (EFI_PHYSICAL_ADDRESS)&ev.Event, ev.Header.EventDataSize,
                     TCG_ALG_SHA,(TCG_PCR_EVENT*)&ev,
                     &evNum,&Last);
    }
    else
    {

        Status = gBS->LocateProtocol (
                     &gEfiTcgProtocolGuid,
                     NULL,
                     &tcmSvc);

        if ( EFI_ERROR( Status ))
        {
            return EFI_SUCCESS;
        }

        TcmEvent.Header.PCRIndex      = PCRi_OPROM_CODE;
        TcmEvent.Header.EventType     = EV_EVENT_TAG;
        TcmEvent.Event.Tagged.EventID = EV_ID_OPROM_EXECUTE;
        TcmEvent.Event.Tagged.EventSize =
            sizeof(TcmEvent.Event.Tagged.EventData.OptionRomExecute);
        TcmEvent.Header.EventDataSize = _TPM_STRUCT_PARTIAL_SIZE(struct _EFI_TCG_EV_TAG,
                                        EventData ) + TcmEvent.Event.Tagged.EventSize;

        TcmEvent.Event.Tagged.EventData.OptionRomExecute.PFA      = pfa;
        TcmEvent.Event.Tagged.EventData.OptionRomExecute.Reserved = 0;
        digestLen = sizeof (TcmEvent.Event.Tagged.EventData.OptionRomExecute.Hash);
        pDigest   = TcmEvent.Event.Tagged.EventData.OptionRomExecute.Hash.digest;

        Status = tcmSvc->HashAll(
                     tcmSvc,
                     (UINT8*)pImage,
                     len,
                     TCG_ALG_SHA,
                     &digestLen,
                     &pDigest);
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }

        Status = tcmSvc->HashLogExtendEvent(
                     tcmSvc,
                     (EFI_PHYSICAL_ADDRESS)&TcmEvent.Event, TcmEvent.Header.EventDataSize,
                     TCG_ALG_SHA,(TCM_PCR_EVENT*)&TcmEvent,
                     &evNum,&Last);
    }

    return Status;
}



EFI_STATUS ResetMorVariable()
{
    EFI_STATUS      Status;
    UINT32 Attribs   = EFI_VARIABLE_NON_VOLATILE
                       | EFI_VARIABLE_BOOTSERVICE_ACCESS
                       | EFI_VARIABLE_RUNTIME_ACCESS;

    UINT8 Temp       = 0xFE;
    UINTN TempSize = sizeof (UINT8);



    Status = gRT->GetVariable(
                 L"MemoryOverwriteRequestControl",
                 &gEfiMemoryOverwriteControlDataGuid,
                 &Attribs,
                 &TempSize,
                 &Temp );

    if ( EFI_ERROR( Status ) || ((Temp & 01)!= 0) )
    {
        Temp &= 0xFE;
        Status = gRT->SetVariable(
                     L"MemoryOverwriteRequestControl",
                     &gEfiMemoryOverwriteControlDataGuid,
                     Attribs,
                     sizeof (UINT8),
                     &Temp );
    }
    
    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_MOR_BIT_CLEARED | EFI_SOFTWARE_DXE_BS_DRIVER);
    return Status;
}


EFI_STATUS MeasureWakeEvent()
{
    UINT8   nWake;
    EFI_STATUS Status;
    CHAR8   WakeString[0xD] = "Wake Event  ";

    Status = GetTcgWakeEventType( &nWake );
    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    DEBUG((DEBUG_INFO, "TCG: GetTcgWakeEventType: Status=%r; WakeType=%x\n",
           Status, nWake));

    WakeString[0xB] = nWake;

    if(IsTcmSupportType())
    {
        Status =  TcmMeasureActionI( WakeString, (UINT32)PCRi_STATE_TRANSITION );
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }
    }
    else
    {
        Status = TcgMeasureActionI( WakeString, (UINT32)PCRi_STATE_TRANSITION );
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }
    }

    return Status;
}




EFI_STATUS MeasureSeparators()
{
    TPM_PCRINDEX   PcrIndex = 0;
    EFI_STATUS     Status;

    if(IsTcmSupportType())
    {
        for ( PcrIndex = 0; PcrIndex < 8; PcrIndex++ )
        {
            Status = TcmMeasureSeparatorEvent( PcrIndex );
        }
    }
    else
    {
        for ( PcrIndex = 0; PcrIndex < 7; PcrIndex++ )
        {
            Status = MeasureSeparatorEvent( PcrIndex );
        }
    }

    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_UEFI_SEPARATORS_MEASURED | EFI_SOFTWARE_DXE_BS_DRIVER);
    return Status;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   DummySkipPhysicalPresencePtr
//
// Description: Always return false. Can be overridden with OEM function
//
// Input:       IN  EFI_EVENT       efiev
//              IN  VOID            *ctx
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
UINT8 DummySkipPhysicalPresence()
{
//    return FALSE;
    return TRUE;
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   tcgReadyToBoot
//
// Description: Generic Measurements done before EFI boots OS
//
// Input:       IN  EFI_EVENT       efiev
//              IN  VOID            *ctx
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
void tcgReadyToBoot(
    IN EFI_EVENT efiev,
    IN VOID      *ctx )
{
    static UINTN                   mBootAttempts = 0;

    EFI_STATUS                     Status;
    EFI_TCG_PROTOCOL               *tcg;
    EFI_TPM_DEVICE_PROTOCOL        *TpmDevice;
    BOOLEAN                        Support = FALSE;
    UINT16                         physical_presence;
#if SET_LIFETIME_PPLOCK == 1
    TPM_Capabilities_PermanentFlag cap;
#endif
#if UNLOCK_PP_IN_MANUFACTURING_MODE
    BOOLEAN                        SkipPpLock = FALSE;
    BOOLEAN                       *ResetAllTcgVar = NULL;
    Manufac_HOB                   *TcgMfgModeVar = NULL;
    void                          **DummyPtr;
#else
    TPM_RESULT                    SendCmdResults;
#endif

    AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL      *Tpm12PPILockPOverride;
    AMI_TCG2_DXE_FUNCTION_OVERRIDE_PROTOCOL     *Tpm12MeasureConfigurationInfoFunc;

//    PERF_START(0,L"TcgReadyToboot",NULL,0);

    DEBUG((DEBUG_INFO, "TcgReady to boot entry\n"));

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcg );

    Support = IsTcmSupportType();

    Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid, NULL, &TpmDevice );

    if ( EFI_ERROR( Status ))
    {
        return;
    }

    if ( mBootAttempts == 0 )
    {
        ResetMorVariable();

#if defined (WAKE_EVENT_MEASUREMENT) && (WAKE_EVENT_MEASUREMENT != 0)
        MeasureWakeEventFuncPtr();
#endif
//       PERF_START(0,L"SelfTest",NULL,0);
#if defined DONT_SEND_SELFTEST_TILL_READY_TO_BOOT && DONT_SEND_SELFTEST_TILL_READY_TO_BOOT == 1
        if(*(UINT16 *)(UINTN)(PORT_TPM_IOMEMBASE + 0xF00) == SELF_TEST_VID)
        {
            SendTpmCommand( tcg,  TPM_ORD_ContinueSelfTest,0, 0);
        }
#endif
//        PERF_END(0,L"SelfTest",NULL,0);
        //
        // Measure handoff tables
        //

        Status = gBS->LocateProtocol(
                     &Tpm12MeasureConfigurationInfoFuncGuid,
                     NULL,
                     &Tpm12MeasureConfigurationInfoFunc );
        if(!EFI_ERROR(Status))
        {
            Tpm12MeasureConfigurationInfoFunc->Function();
        }
        else
        {
            MeasureConfigurationInfoFuncPtr ();
        }

        if(Support)
        {
            Status = TcmMeasureActionI (
                         "Calling EFI Application from Boot Option",
                         (UINT32)PCRi_IPL_CONFIG_AND_DATA );
        }
        else
        {
            Status = TcgMeasureActionI (
                         "Calling EFI Application from Boot Option",
                         (UINT32)PCRi_IPL_CONFIG_AND_DATA );
        }

        if ( EFI_ERROR( Status ))
        {
            return;
        }
        //
        // Measure BootOrder & Boot#### variables
        //

#if ( defined(Measure_Boot_Data) && (Measure_Boot_Data!= 0) )
        Status = MeasureAllBootVariablesFuncPtr( );


        if ( EFI_ERROR( Status ))
        {
            DEBUG((DEBUG_INFO, "Boot Variables not Measured. Error!\n"));
        }
#endif

        //
        // 4. Measure PE/COFF OS loader, would be done by DxeCore
        //
//        PERF_START(0,L"OsSeparators",NULL,0);
        MeasureSeparatorsFuncPtr();
//        PERF_END(0,L"OsSeparators",NULL,0);

        gBS->CloseEvent(OpromEvent);
    }
    else
    {
        //.0

        // 8. Not first attempt, meaning a return from last attempt
        //
        if(Support)
        {
            Status = TcmMeasureActionI (
                         "Returning from EFI Application from Boot Option",
                         (UINT32)PCRi_IPL_CONFIG_AND_DATA );
        }
        else
        {
            Status = TcgMeasureActionI (
                         "Returning from EFI Application from Boot Option",
                         (UINT32)PCRi_IPL_CONFIG_AND_DATA );
        }
        if ( EFI_ERROR( Status ))
        {
            return;
        }
    }

    //
    // Increase boot attempt counter
    //
    if ( mBootAttempts == 0 )  //do this once
    {
#if SET_LIFETIME_PPLOCK == 1

        cap = read_TPM_capabilities( tcg );

        if ( cap.physicalPresenceLifetimeLock == 0)
        {
            if(IsTcmSupportType())
            {
                physical_presence = TPM_H2NS(TPM_PHYSICAL_PRESENCE_CMD_ENABLE );
                if(cap.physicalPresenceCMDEnable == 0 && cap.physicalPresenceHWEnable == 0)
                {
                    SendTpmCommand( tcg,  TCM_TSC_ORD_PhysicalPresence,
                                    sizeof(physical_presence), &physical_presence );

                }
                physical_presence = TPM_H2NS( TPM_PHYSICAL_PRESENCE_LIFETIME_LOCK );
                SendTpmCommand( tcg, TCM_TSC_ORD_PhysicalPresence,
                                sizeof(physical_presence), &physical_presence );
            }
            else
            {
                physical_presence = TPM_H2NS(TPM_PHYSICAL_PRESENCE_CMD_ENABLE );
                if(cap.physicalPresenceCMDEnable == 0 && cap.physicalPresenceHWEnable == 0)
                {
                    SendTpmCommand( tcg,  TSC_ORD_PhysicalPresence,
                                    sizeof(physical_presence), &physical_presence );

                }
                physical_presence = TPM_H2NS( TPM_PHYSICAL_PRESENCE_LIFETIME_LOCK );
                SendTpmCommand( tcg, TSC_ORD_PhysicalPresence,
                                sizeof(physical_presence), &physical_presence );
            }


        }
#endif

        Status = gBS->LocateProtocol(
                     &Tpm12PPILockOverrideguid,
                     NULL,
                     &Tpm12PPILockPOverride );
        if(!EFI_ERROR(Status))
        {
            Tpm12PPILockPOverride->Function();
        }
        else
        {
#if (defined(UNLOCK_PP_IN_MANUFACTURING_MODE) && (UNLOCK_PP_IN_MANUFACTURING_MODE != 0))
            DummyPtr       = &TcgMfgModeVar;
            TcgMfgModeVar  = (Manufac_HOB *)LocateATcgHob(
                                 gST->NumberOfTableEntries,
                                 gST->ConfigurationTable,
                                 &gNvramHobGuid);

            if(*DummyPtr != NULL)
            {
                if(TcgMfgModeVar->NvramMode & NVRAM_MODE_MANUFACTORING )
                {
                    SkipPpLock = TRUE;
                }
            }

            if( SkipPpLock == FALSE)
            {
                //always lock at the end of boot
                physical_presence = TPM_H2NS( TPM_PHYSICAL_PRESENCE_LOCK );
                SendTpmCommand( tcg, TSC_ORD_PhysicalPresence,
                                sizeof(physical_presence), &physical_presence );
            }
#else
            if(!IsTcmSupportType())
            {
                //always lock at the end of boot
                physical_presence = TPM_H2NS( TPM_PHYSICAL_PRESENCE_LOCK );
                SendCmdResults = SendTpmCommand( tcg, TSC_ORD_PhysicalPresence, sizeof(physical_presence), &physical_presence );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_PP_LOCK_CMD_SENT | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(SendCmdResults){
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_1_2_PP_LOCK_ERROR | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
            }
#endif
        }
    }


    mBootAttempts++;
//    PERF_END(0,L"TcgReadyToboot",NULL,0);
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   SetTcgReadyToBoot
//
// Description: Sets ready to boot callback on ready to boot for security device
//
// Input:   NONE
//
// Output:   EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI SetTcgReadyToBoot()
{
    EFI_STATUS Status;
    AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL      *POverride;

    Status = gBS->LocateProtocol(&AmiSetTcgReadytobootGuid,
                                 NULL,
                                 &POverride );

    if(!EFI_ERROR(Status))
    {
        return (POverride->Function());
    }

#if defined(EFI_EVENT_SIGNAL_READY_TO_BOOT)\
        && EFI_SPECIFICATION_VERSION < 0x20000

    Status = gBS->CreateEvent( EFI_EVENT_SIGNAL_READY_TO_BOOT,
                               EFI_TPL_CALLBACK,
                               tcgReadyToBoot, NULL, &ReadyToBootEvent );

#else
    Status = EfiCreateEventReadyToBootEx( EFI_TPL_CALLBACK,
                                          tcgReadyToBoot,
                                          NULL,
                                          &ReadyToBootEvent );
#endif

    return Status;
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   OnPciIOInstalled
//
// Description: Checks if PCI device has an Option Rom and initiates the
//              Option rom measurment
//
// Input:
//              IN  EFI_EVENT       ev
//              IN  VOID            *ctx
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
EFI_STATUS OnPciIOInstalled(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    EFI_STATUS                   Status;
    CSM_PLATFORM_POLICY_DATA    *Opromcntxt = ctx;
    UINTN                        handlesSize = sizeof(EFI_HANDLE);
    EFI_HANDLE                   handles;
    CSM_PLATFORM_POLICY_DATA    *OpRomStartEndProtocol=NULL;

    DEBUG((DEBUG_INFO, "OnPciIOInstalled\n"));

    Status = gBS->LocateHandle(ByRegisterNotify,
                               NULL,
                               OpromContext,
                               &handlesSize,
                               &handles );

    DEBUG((DEBUG_INFO, "OnPciIOInstalled LocateHandle = %r \n", Status));

    Status = gBS->HandleProtocol(handles, &gOpromStartEndProtocolGuid, &OpRomStartEndProtocol);

    DEBUG((DEBUG_INFO, "OnPciIOInstalled HandleProtocol = %r \n", Status));

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    DEBUG((DEBUG_INFO, "OpRomStartEndProtocol located \n"));

    if((OpRomStartEndProtocol == NULL))
    {
        return Status;
    }

    if(OpRomStartEndProtocol->PciIo == NULL)
    {
        return Status;
    }

    DEBUG((DEBUG_INFO, "OpRomStartEndProtocol->PciIo->RomImage = %x \n", OpRomStartEndProtocol->PciIo->RomImage));
    DEBUG((DEBUG_INFO, "OpRomStartEndProtocol->PciIo->RomSize = %x \n", OpRomStartEndProtocol->PciIo->RomSize));


    DEBUG((DEBUG_INFO, "Measuring legacy image \n"));

    Status = MeasurePciOptionRom(OpRomStartEndProtocol->PciIo->RomImage,
                                 (UINT32) OpRomStartEndProtocol->PciIo->RomSize,
                                 GET_PFA(OpRomStartEndProtocol->PciIo ));

    return Status;
#else
    return EFI_SUCCESS;
#endif

}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   MeasurePCIOproms
//
// Description: Sets callback to measure PCI option roms that are given control
//
// Input:       NONE
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
MeasurePCIOproms()
{
    EFI_STATUS Status;
    AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL      *POverride;

    Status = gBS->LocateProtocol(
                 &AmiMeasurePciopromGuid,
                 NULL,
                 &POverride );

    if(!EFI_ERROR(Status))
    {
        return (POverride->Function());
    }

    OpromContext = (void*)&Ctx;
    Status    = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                  TPL_NOTIFY,
                                  OnPciIOInstalled,
                                  OpromContext,
                                  &OpromEvent );

    ASSERT( !EFI_ERROR( Status ));
    Status = gBS->RegisterProtocolNotify( &gOpromStartEndProtocolGuid,
                                          OpromEvent,
                                          &OpromContext );

    return EFI_SUCCESS;
}




EFI_STATUS
EFIAPI GetProtocolVersion(
    AMI_TCG_PROTOCOL_VERSION *VerInf)
{
    VerInf->MajorVersion = 1;
    VerInf->MinorVersion = 0;
    VerInf->Reserve      = 0;
    VerInf->Flag         = 0;
    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  extend_request
//
// Description: Convert the opcode into array of 2-bit values.For each 2-bit: 0 - no change; 1 - turn off; 2 - turn on
//               indexes: 0 - enable flag; 1 - active flag; 2 - onwership
//               3 - clear
//
// INPUT:       IN UINT8 rqst
//
// OUTPUT:      extended_request
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
extended_request extend_request(
    IN UINT8 rqst )
{

    if ( rqst )
    {
        switch ( rqst )
        {
            case TCPA_PPIOP_ENABLE:
                return PPIXOP_ENABLE;
            case TCPA_PPIOP_DISABLE:
                return PPIXOP_DISABLE;
            case TCPA_PPIOP_ACTIVATE:
                return PPIXOP_ACTIVATE;
            case TCPA_PPIOP_DEACTIVATE:
                return PPIXOP_DEACTIVATE;
            case TCPA_PPIOP_CLEAR:
                return PPIXOP_CLEAR;
            case TCPA_PPIOP_ENABLE_ACTV:
                return (PPIXOP_ENABLE | PPIXOP_ACTIVATE);
            case TCPA_PPIOP_DEACT_DSBL:
                return (PPIXOP_DISABLE | PPIXOP_DEACTIVATE);
            case TCPA_PPIOP_OWNER_ON:
                return PPIXOP_OWNER_ON;
            case TCPA_PPIOP_OWNER_OFF:
                return PPIXOP_OWNER_OFF;
            case TCPA_PPIOP_ENACTVOWNER:
                return (PPIXOP_ENABLE | PPIXOP_ACTIVATE | PPIXOP_OWNER_ON);
            case TCPA_PPIOP_DADISBLOWNER:
                return (PPIXOP_DISABLE | PPIXOP_DEACTIVATE | PPIXOP_OWNER_OFF);
            case TCPA_PPIOP_CLEAR_ENACT:
                return (PPIXOP_CLEAR| PPIXOP_ENABLE | PPIXOP_ACTIVATE );
            case  TCPA_PPIOP_ENABLE_ACTV_CLEAR:
                return (PPIXOP_ENABLE | PPIXOP_ACTIVATE | PPIXOP_CLEAR);
            case TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV:
                return((PPIXOP_ENABLE<<8) |(PPIXOP_ACTIVATE<<8) | PPIXOP_CLEAR | PPIXOP_ENABLE | PPIXOP_ACTIVATE);
            default:
                return 0;
        }
    }
    else
    {
        return 0;
    }
}





//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  execute_request
//
// Description: Execute TPM operation
//
// INPUT:       IN UINT8 rqst
//
// OUTPUT:      error code if any as result of executing the operation
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8 expecting_reset = FALSE;
TPM_RESULT execute_request(
    IN UINT8 rqst )
{
    extended_request               erqst;
    EFI_STATUS                     Status;
    EFI_TCG_PROTOCOL               *tcg;
    EFI_TPM_DEVICE_PROTOCOL        *TpmDevice;
    TPM_RESULT                     tpmStatus = 0;
    UINT8                          zero = 0;
    UINT8                          one  = 1;
    UINT32                         Temp = 0;


    erqst = extend_request( rqst );

    DEBUG((DEBUG_INFO, "erqst: %x\n",erqst));

    Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid,
                                  NULL, &TpmDevice );

    if ( EFI_ERROR( Status ))
    {
        return TCPA_PPI_BIOSFAIL;
    }

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcg );

    if ( EFI_ERROR( Status ))
    {
        DEBUG((DEBUG_ERROR, "Error: failed to locate TCG protocol: %r\n"));
        return TCPA_PPI_BIOSFAIL;
    }

    if(!IsTcmSupportType())
    {
        TpmDevice->Init( TpmDevice );
    }

    read_TPM_capabilities( tcg );

    if(!IsTcmSupportType())
    {
        //use switch case
        switch( rqst)
        {
            case 1:
                // send tpm command to enable the TPM
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalEnable, 0, 0 );
                if(tpmStatus){
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }else{
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ENABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                break;
            case 2:
                //disable TPM
                tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalDisable, 0, 0 );
                if(tpmStatus){
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_DISABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }else{
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_DISABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                break;
            case 3:
                //Activate TPM
                tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&zero );
                if(tpmStatus){
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ACTIVATE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }else{
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                break;
            case 4:
                //Dectivate TPM
                tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&one );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_DEACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                break;
            case 5:
                //force clear
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_ForceClear, 0, 0 );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_DEVICE_CLEARED | EFI_SOFTWARE_DXE_BS_DRIVER);
                break;
            case 6:
                //Enable + Activate
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalEnable, 0, 0 );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ENABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&zero );
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                    if(tpmStatus){
                        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ACTIVATE_FAIL);
                    }else{
                        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }
                }else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                break;
            case 7:
                //Deactivate + Disable
                tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&one );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_DEACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalDisable, 0, 0 );
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_DISABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_DISABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                break;
            case 8:
                //set Owner Install true
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_SetOwnerInstall,1, &one );
                break;
            case 9:
                //set Owner Install False
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_SetOwnerInstall,1, &zero );
                break;
            case 10:
                //Enable + Activate + set Owner Install true
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalEnable, 0, 0 );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ENABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&zero );
                    if(tpmStatus){
                        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ACTIVATE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }else{
                        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }
                }else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_SetOwnerInstall,1, &one );
                if((TPM_H2NL( tpmStatus ) & TCG_DEACTIVED_ERROR) == TCG_DEACTIVED_ERROR )
                {
                    Temp = TCPA_PPIOP_OWNER_ON | (rqst << 04);

                    Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                              sizeof (UINT32), &Temp );

                    if(!IsTcmSupportType())
                    {
                        TpmDevice->Close( TpmDevice );
                    }
                    WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
                    RequestSystemReset( EfiResetCold );
                }
                break;
            case 11:
                //Setownerfalse + Deactivate + disable
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_SetOwnerInstall,1, &zero );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&one );
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_DEACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalDisable, 0, 0 );
                    if(!tpmStatus)
                    {
                        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_DISABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }
                    else{
                        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_DISABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }
                }
                break;
            case 14:
                //clear + Enable + Activate
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_ForceClear, 0, 0 );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_DEVICE_CLEARED | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalEnable, 0, 0 );
                    if(tpmStatus){
                        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ACTIVATE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }else{
                        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }
                }
                else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&zero );
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                break;
            case 12:
                tpmStatus = TpmSetCapabilityDefferedPP(tcg);
                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }

                WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
                RequestSystemReset( EfiResetCold );
                break;
            case 13:
                //not supported
                //cases 15-20 are handles elsewhere
                break;
            case 21:
                //Enable + Activate + clear
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalEnable, 0, 0 );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ENABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&zero );
                    if(tpmStatus){
                        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ACTIVATE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }else{
                        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }
                }else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }

                Temp = TCPA_PPIOP_CLEAR | (rqst << 04);

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }

                WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
                RequestSystemReset( EfiResetCold );
                break;
            case 22:
                //Enable + Activate + clear + Enable + Activate
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalEnable, 0, 0 );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ENABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&zero );
                    if(tpmStatus){
                        TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ACTIVATE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }else{
                        TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                    }
                }
                else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TPM_ORD_ForceClear, 0, 0 );
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_DEVICE_CLEARED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }

                //after force clear, update request and reset the system for
                // tcg flags to be updated
                Temp = TCPA_PPIOP_ENABLE_ACTV | (rqst << 04);

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }

                WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
                RequestSystemReset( EfiResetCold );
                break;
            default:
                break;

        }
    }
    else
    {

        //use switch case
        switch( rqst)
        {
            case 1:
                // send tpm command to enable the TPM
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalEnable, 0, 0 );
                break;
            case 2:
                //disable TPM
                tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalDisable, 0, 0 );
                break;
            case 3:
                //Activate TPM
                tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&zero );
                break;
            case 4:
                //Dectivate TPM
                tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&one );
                break;
            case 5:
                //force clear
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_ForceClear, 0, 0 );
                break;
            case 6:
                //Enable + Activate
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalEnable, 0, 0 );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&zero );
                }
                break;
            case 7:
                //Deactivate + Disable
                tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&one );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalDisable, 0, 0 );
                }
                break;
            case 8:
                //set Owner Install true
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_SetOwnerInstall,1, &one );
                break;
            case 9:
                //set Owner Install False
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_SetOwnerInstall,1, &zero );
                break;
            case 10:
                //Enable + Activate + set Owner Install true
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalEnable, 0, 0 );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&zero );
                }
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_SetOwnerInstall,1, &one );
                if((TPM_H2NL( tpmStatus ) & TCG_DEACTIVED_ERROR) == TCG_DEACTIVED_ERROR )
                {
                    Temp = TCPA_PPIOP_OWNER_ON | (rqst << 04);

                    Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                              sizeof (UINT32), &Temp );

                    if(!IsTcmSupportType())
                    {
                        TpmDevice->Close( TpmDevice );
                    }
                    WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
                    RequestSystemReset( EfiResetCold );
                }
                break;
            case 11:
                //Setownerfalse + Deactivate + disable
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_SetOwnerInstall,1, &zero );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&one );

                }
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalDisable, 0, 0 );
                }
                break;
            case 14:
                //clear + Enable + Activate
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_ForceClear, 0, 0 );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalEnable, 0, 0 );
                }
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&zero );
                }
                break;
            case 12:
            case 13:
                //not supported
                //cases 15-20 are handles elsewhere
                break;
            case 21:
                //Enable + Activate + clear
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalEnable, 0, 0 );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&zero );
                }

                Temp = TCPA_PPIOP_CLEAR | (rqst << 04);

                TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                                                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                                                sizeof (UINT32), &Temp );

                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }

                WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
                RequestSystemReset( EfiResetCold );
                break;
            case 22:
                //Enable + Activate + clear + Enable + Activate
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalEnable, 0, 0 );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&zero );
                }
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg, TCM_ORD_ForceClear, 0, 0 );
                }

                //after force clear, update request and reset the system for
                // tcg flags to be updated
                Temp = TCPA_PPIOP_ENABLE_ACTV | (rqst << 04);

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                DEBUG((DEBUG_INFO, "TcgSetVariableWithNewAttributes Status: %r\n",Status));
                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }

                WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
                RequestSystemReset( EfiResetCold );
                break;
            default:
                break;

        }
    }

    WritePpiResult( rqst, (UINT16)( TPM_H2NL( tpmStatus )));
    //
    // System may need reset so that TPM reload permanent flags
    //
    return tpmStatus;
}


#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
VOID
AcpiOnVariableLockProtocolGuid (
    IN      EFI_EVENT                 Event,
    IN      VOID                      *Context
)
{
    EDKII_VARIABLE_LOCK_PROTOCOL    *LockProtocol;
    EFI_STATUS                      Status;

    DEBUG((DEBUG_INFO, "Tpm20OnVariableLockProtocolGuid callback entry\n"));

    Status =  gBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid, NULL, &LockProtocol);
    if(!EFI_ERROR(Status))
    {
        Status = LockProtocol->RequestToLock(LockProtocol, L"TpmServFlags", &FlagsStatusguid);
        ASSERT_EFI_ERROR(Status);
    }
}
#endif


UINT8 SupportInfo = 0;

VOID UpdateAslCallback(
    IN EFI_EVENT ev,
    IN VOID      *ctx)
{
    EFI_PHYSICAL_ADDRESS            dsdtAddress=0;
    ACPI_HDR                        *dsdt;
    EFI_STATUS                      Status;
    UINTN                           Value=0;
    AMI_ASL_PPI_NV_VAR              *MemoryAddress;
    EFI_PHYSICAL_ADDRESS             VarLoc;
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
    EDKII_VARIABLE_LOCK_PROTOCOL    *LockProtocol;
    EFI_EVENT                       VarLockEvent;
static VOID                         *VarLockreg;
#endif

    DEBUG((DEBUG_INFO, "\n UpdateAslCallback \n"));
    Status = TcgLibGetDsdt(&dsdtAddress, EFI_ACPI_TABLE_VERSION_ALL);
    if (!EFI_ERROR(Status))
    {
        dsdt = (ACPI_HDR*)dsdtAddress;
        Status=TcgUpdateAslNameObject(dsdt, "TPMF", (UINT64)SupportInfo);
        if(EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "\n Failure updating TPMF flag \n"));
        }

        // Update the TPMM Base Addr
        Status=TcgUpdateAslNameObject(dsdt, "TPMM", (UINT64)PORT_TPM_IOMEMBASE);
        if(EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "\n Failure updating TPMM flag \n"));
        }

        Status=TcgUpdateAslNameObject(dsdt, "TTDP", (UINT64)Value);
        if(EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "\n Failure updating TTDP flag \n"));
        }

        Status=TcgUpdateAslNameObject(dsdt, "TTPF", (UINT64)Value);
        if (EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "TrEEUpdateTpmDeviceASL::Failure setting ASL value %r \n", Status));
        }

        if(IsTcmSupportType())
        {
            Value=1;
        }
        else
        {
            Value=0;
        }

        Status=TcgUpdateAslNameObject(dsdt, "TCMF", (UINT64)Value);
        if (EFI_ERROR(Status))
        {
            DEBUG((DEBUG_ERROR, "\n Failure updating TCMF flag \n"));
        }

        Status=TcgUpdateAslNameObject(dsdt, "PPIV", 0);

        Status = gBS->AllocatePool (EfiACPIMemoryNVS, sizeof(AMI_ASL_PPI_NV_VAR), &MemoryAddress);
        if(!EFI_ERROR(Status))
        {
            ZeroMem (MemoryAddress, sizeof(AMI_ASL_PPI_NV_VAR));

            Status=TcgUpdateAslNameObject(dsdt, "PPIM", (UINT32)MemoryAddress);
            ASSERT_EFI_ERROR (Status);
            Status=TcgUpdateAslNameObject(dsdt, "PPIL", (UINT32)(sizeof(AMI_ASL_PPI_NV_VAR)));
            ASSERT_EFI_ERROR (Status);

            VarLoc = (EFI_PHYSICAL_ADDRESS)MemoryAddress;
//gEdkiiVariableLockProtocolGuid is defined from core ver 5.11 so we need to use CORE_BUILD_NUMBER > 0xA .
//Variable Lock Protocol is implemented from NVRAM label 7 so we need to use NVRAM_VERSION > 6
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6

            Status = TcgSetVariableWithNewAttributes(L"TpmServFlags",
                                      &FlagsStatusguid,
                                      EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                      EFI_VARIABLE_NON_VOLATILE |
                                      EFI_VARIABLE_RUNTIME_ACCESS,
                                      sizeof (EFI_PHYSICAL_ADDRESS),
                                      &VarLoc );

            //Lock the service flags variable as well
              Status =  gBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid, NULL, &LockProtocol);
              if(!EFI_ERROR(Status)){
                  Status = LockProtocol->RequestToLock(LockProtocol, L"TpmServFlags", &FlagsStatusguid);
                  ASSERT_EFI_ERROR(Status);
              }else{
                  //setcallback
                  Status = gBS->CreateEvent (EFI_EVENT_NOTIFY_SIGNAL,
                                             EFI_TPL_CALLBACK,
                                             AcpiOnVariableLockProtocolGuid,
                                             NULL,
                                             &VarLockEvent);
                  if(!EFI_ERROR(Status))
                  {
                      Status = gBS->RegisterProtocolNotify(
                                    &gEdkiiVariableLockProtocolGuid,
                                    VarLockEvent,
                                    &VarLockreg );
                  }
              }

              ASSERT_EFI_ERROR (Status);
#else
              Status = TcgSetVariableWithNewAttributes(L"TpmServFlags",
                                     &FlagsStatusguid,
                                     EFI_VARIABLE_BOOTSERVICE_ACCESS |
                                     EFI_VARIABLE_NON_VOLATILE,
                                     sizeof (EFI_PHYSICAL_ADDRESS),
                                     &VarLoc );

              ASSERT_EFI_ERROR (Status);
#endif
        }else{
            //use defaults form SDL token.
            DEBUG ((DEBUG_ERROR, "Allocate PPI Memory failed Status = %r\n", Status));
        }
    }
    
    if(ev != NULL) gBS->CloseEvent(ev);

}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ProcessTcgSetup
//
// Description: Handles Tcg Setup functionality
//
// Input:       NONE
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
EFIAPI ProcessTcgSetup()
{
    UINT8                           Disable = 0;
    EFI_STATUS                      Status;
    EFI_TCG_PROTOCOL                *tcg;
    EFI_TPM_DEVICE_PROTOCOL         *TpmDevice;
    TPM_Capabilities_PermanentFlag  cap;
    UINT8                           Temp = 0;
    UINT8                           TpmOwner;
    EFI_PHYSICAL_ADDRESS            dsdtAddress=0;
    ACPI_HDR                        *dsdt;
    EFI_EVENT                       Event;
    static VOID                     *Dsdtreg;
#if (defined(CONFIRM_SETUP_CHANGE) && (CONFIRM_SETUP_CHANGE == 1))
    EFI_EVENT                       ev;
    void                            *SimpleIn = NULL;
    static void                     *reg;
    BOOLEAN             NvRamFlag = FALSE;
#endif
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;

    UINT32  TpmOldVarAttributes      = EFI_VARIABLE_BOOTSERVICE_ACCESS|\
                                       EFI_VARIABLE_NON_VOLATILE;

    UINTN                           Size = sizeof(UINT8);
    TCG_CONFIGURATION               Config;
    UINT8                           TpmOldVar = 0;
    AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL      *POverride;

    Status = gBS->LocateProtocol(&AmiProcessTcgSetupGuid,
                                 NULL,
                                 &POverride );

    if(!EFI_ERROR(Status))
    {
        return (POverride->Function());
    }

    Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid, NULL, &TpmDevice );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcg );

    if ( EFI_ERROR( Status ))
    {
        DEBUG((DEBUG_ERROR, "Error: failed to locate TCG protocol: %r\n"));
        return Status;
    }

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    cap = read_TPM_capabilities( tcg );
    TpmOwner = CheckTpmOwnership( tcg );

    gBS->CopyMem(&Config, &ProtocolInstance->ConfigFlags, sizeof(TCG_CONFIGURATION));

#if (defined(EXPOSE_FORCE_TPM_ENABLE) && (EXPOSE_FORCE_TPM_ENABLE == 1))
    DEBUG((DEBUG_INFO, "\n cap.disabled = %x, cap.deactivated=%x \n",cap.disabled ,cap.deactivated));
    DEBUG((DEBUG_INFO, "\n Config.Reserved4= % x \n",Config.Reserved4));
    if(Config.Reserved4)
    {
        if(cap.disabled != 1 && cap.deactivated != 1)
        {
            if ( execute_request(TCPA_PPIOP_ENABLE_ACTV )!= 0 )
            {
                //Do nothing but update setup to display TPM ERROR on execution of
                //setup command and continue to boot
                DEBUG((DEBUG_ERROR, "\n Error enabling TPM \n"));
                Config.TpmError    = AMI_TPM_HARDWARE_SETUP_REQUEST_ERROR;
                ProtocolInstance->UpdateStatusFlags(&Config, FALSE);
            }
            else
            {
                DEBUG((DEBUG_INFO, "\n TPM enabled \n"));
                Config.Reserved4   = 0;
                Config.TpmEnable   = 1;

                TpmOldVar = Config.TpmEnable;

                //set variable
                Status = gRT->SetVariable (
                             L"TpmOldvar",
                             &gTcgEfiGlobalVariableGuid,
                             TpmOldVarAttributes,
                             Size,
                             &TpmOldVar);

                ProtocolInstance->UpdateStatusFlags(&Config, TRUE);
                RequestSystemReset( EfiResetCold );
            }
        }
        else
        {
            Config.Reserved4   = 0;
            Config.TpmEnable   = 1;
            ProtocolInstance->UpdateStatusFlags(&Config, TRUE);
        }
    }
#endif

    //update onwership and Deactivated statuses
    Config.TpmEnaDisable = cap.disabled;
    Config.TpmActDeact   = cap.deactivated;
    Config.TpmOwnedUnowned  = TpmOwner;
    Config.TpmError    = 0;

    Status = gRT->GetVariable (
                 L"TpmOldvar",
                 &gTcgEfiGlobalVariableGuid,
                 &TpmOldVarAttributes,
                 &Size,
                 &TpmOldVar);

    if(EFI_ERROR(Status))
    {
        TpmOldVar = Config.TpmEnable;
        //set variable
        Status = gRT->SetVariable (
                     L"TpmOldvar",
                     &gTcgEfiGlobalVariableGuid,
                     TpmOldVarAttributes,
                     Size,
                     &TpmOldVar);
    }
    else
    {
        if(TpmOldVar == Config.TpmEnable)
        {
            if(Config.TpmEnable != (~( cap.disabled | cap.deactivated )& BIT00))
            {
                Config.PpiSetupSyncFlag = 1;
            }
        }
        else
        {
            TpmOldVar = Config.TpmEnable;
            Status =  TcgSetVariableWithNewAttributes(L"TpmOldvar", &gTcgEfiGlobalVariableGuid, \
                      TpmOldVarAttributes,\
                      Size, &TpmOldVar );
        }
    }

    if(Config.PpiSetupSyncFlag != 0)
    {
        //ppi request happened so sync setup variables
        DEBUG((DEBUG_INFO, "\n Setup and PPi request sync \n"));
        Config.TpmEnable = (~( cap.disabled | cap.deactivated )& BIT00);
        Config.TpmOperation = 0;
        Config.PpiSetupSyncFlag = 0;

        TpmOldVar = Config.TpmEnable;

        Status =  TcgSetVariableWithNewAttributes(L"TpmOldvar", &gTcgEfiGlobalVariableGuid, \
                  TpmOldVarAttributes,\
                  Size, &TpmOldVar );

    }


    SupportInfo = Config.TpmSupport;

    Status = TcgLibGetDsdt(&dsdtAddress, EFI_ACPI_TABLE_VERSION_ALL);

    if (!EFI_ERROR(Status))
    {
        dsdt = (ACPI_HDR*)dsdtAddress;
        // Update and Sync the whole ASL variable.
        UpdateAslCallback( NULL, NULL);
    }
    else
    {
        Status = gBS->CreateEventEx( EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_CALLBACK, UpdateAslCallback, NULL, &gEfiAcpiTableGuid, &Event );
    }

    ProtocolInstance->UpdateStatusFlags(&Config, FALSE);

#if (defined(CONFIRM_SETUP_CHANGE) && (CONFIRM_SETUP_CHANGE == 0))
    
    if(Config.TpmEnable != (~( cap.disabled | cap.deactivated )& BIT00))
    {
        DEBUG((DEBUG_INFO, "\n TMP_ENABLE != Setup in setup \n"));
        
        if ( execute_request( Config.TpmEnable ? TCPA_PPIOP_ENABLE_ACTV :
                              TCPA_PPIOP_DEACT_DSBL ) != 0 )
        {
            //Do nothing but update setup to display TPM ERROR on execution of
            //setup command and continue to boot
            Config.TpmError    = AMI_TPM_HARDWARE_SETUP_REQUEST_ERROR;
            ProtocolInstance->UpdateStatusFlags(&Config, FALSE);
            return EFI_SUCCESS;
        }
        else
        {
            ProtocolInstance->UpdateStatusFlags(&Config, TRUE);
            RequestSystemReset( EfiResetCold );
        }
    }
    else if ( Config.TpmOperation )
    {
        DEBUG((DEBUG_INFO, "\n ENABLE == Setup in setup \n"));

        if ( execute_request( Config.TpmOperation ) != 0 )
        {
            //Do nothing but update setup to display TPM ERROR on execution of
            //setup command and continue to boot
            //Do nothing but update setup to display TPM ERROR on execution of
            //setup command and continue to boot
            Config.TpmError    = AMI_TPM_HARDWARE_SETUP_REQUEST_ERROR;
            Config.TpmOperation = 0;
            ProtocolInstance->UpdateStatusFlags(&Config, FALSE);
            return EFI_SUCCESS;
        }
        else
        {
            //reset to update setup
//After clear TPM, cap.disabled and cap.deactivate would be 1, then Config.TpmEnable would be updated above (next boot).
//So, this code is redundant.
            //if(Config.TpmOperation == TCPA_PPIOP_CLEAR)
            //{
            //    Config.TpmEnable = 0;
            //}
            Config.TpmOperation = 0;
            ProtocolInstance->UpdateStatusFlags(&Config, TRUE);
            RequestSystemReset( EfiResetCold );
        }
    }
#else

    Status = gBS->LocateProtocol(&gEfiSimpleTextInProtocolGuid,NULL,&SimpleIn );

    if(Config.TpmEnable != (~( cap.disabled | cap.deactivated )& BIT00)) {
        
        DEBUG((DEBUG_INFO, "\n TMP_ENABLE != Setup in setup \n"));
        if ( Config.TpmEnable )
        {
            ppi_request = TCPA_PPIOP_ENABLE_ACTV;
        }
        else if ( !Config.TpmEnable )
        {
            ppi_request = TCPA_PPIOP_DEACT_DSBL;
        }
    } else if ( Config.TpmOperation ) {
        
       DEBUG((DEBUG_INFO, "\n ENABLE == Setup in setup \n"));
     
       ppi_request = Config.TpmOperation;

       Config.TpmOperation = 0;
    }
   
    DEBUG((DEBUG_INFO, "ppi_request %x \n",ppi_request));
    
    if(ppi_request > 0  && ppi_request <= TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV)
    {
      Config.Reserved5 = TRUE;
      ProtocolInstance->UpdateStatusFlags(&Config, NvRamFlag);
    
    if (SimpleIn != NULL) {
        
      run_PPI_UI( NULL, NULL );
      
    } else {
        
      Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                             EFI_TPL_CALLBACK,
                             run_PPI_UI,
                             0,
                             &ev );
      ASSERT( !EFI_ERROR( Status ));

      Status = gBS->RegisterProtocolNotify(
                             &gBdsAllDriversConnectedProtocolGuid,
                             ev,
                             &reg );
        ASSERT( !EFI_ERROR( Status ));
      }
    }  
#endif
    
    return Status;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetStringFromToken
//
// Description: Gets a UNI string by Token
//
// Input:       IN      STRING_REF                Token,
//              OUT     CHAR16                    **String
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
EFI_STATUS GetStringFromToken(
    IN STRING_REF Token,
    OUT CHAR16    **String )
{
//    EFI_STATUS Status;
//    UINTN      StringBufferLength;
//    UINT16     *Temp;
#if EFI_SPECIFICATION_VERSION>0x20000
    EFI_STRING InternalString;

    InternalString = HiiGetString (
                         gHiiHandle,
                         Token, NULL   );

    *String = (CHAR16 *)InternalString;
#else
    if ( Hii == NULL )
    {
        return EFI_NOT_FOUND;
    }

    Status = Hii->GetString(
                 Hii,
                 gHiiHandle,
                 Token,
                 TRUE,
                 NULL,
                 &StringBufferLength,
                 *String
             );

    if ( EFI_ERROR( Status ))
    {
        gBS->FreePool( *String );
        return EFI_NOT_FOUND;
    }
#endif

    return EFI_SUCCESS;
}

STRING_REF ppi_op_names[] =
{
    STRING_TOKEN( STR_TCG_ENABLE ),
    STRING_TOKEN( STR_TCG_DISABLE ),

    STRING_TOKEN( STR_TCG_ACTIVATE ),
    STRING_TOKEN( STR_TCG_DEACTIVATE ),

    STRING_TOKEN( STR_TCG_ALLOW ),
    STRING_TOKEN( STR_TCG_DISALLOW ),

    STRING_TOKEN( STR_TCG_CLEAR ),
    STRING_TOKEN( STR_TCG_NOTCLEAR ),

    STRING_TOKEN( STR_TCG_ENABLE ),
    STRING_TOKEN( STR_TCG_DISABLE ),

    STRING_TOKEN( STR_TCG_ACTIVATE ),
    STRING_TOKEN( STR_TCG_DEACTIVATE ),

};


STRING_REF NV_op_names[] =
{
    STRING_TOKEN( STR_TCG_SETNOPPIPROVISION ),
    STRING_TOKEN( STR_TCG_SETNOPPICLEAR ),
    STRING_TOKEN( STR_TCG_SETNOPPIMAINTENANCE ),
    STRING_TOKEN( STR_TCG_SETNOPPIPROVISIONACCPET ),
    STRING_TOKEN( STR_TCG_SETNOPPICLEARACCPET ),
    STRING_TOKEN( STR_TCG_SETNOPPIMAINTENANCEACCEPT ),
};

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   displOperations
//
// Description: Display requested actions as a list of operations
//
// INPUT:       IN extended_request erqst,
//              IN int              count,
//              IN CHAR16           *DesStr
//
// OUTPUT:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
void displOperations(
    IN extended_request erqst,
    IN int              count,
    IN CHAR16           *DesStr )
{
    int    i, j;
    CHAR16 * strDelim = L", ";
    UINT32 r;
    CHAR16 *String;

    if ( count == 2 )
    {
        strDelim = L" and ";
    }

    for ( i = 0, r = erqst, j = 0; i <= PPI_MAX_BASIC_OP; i++, r >>= 2 )
    {
        if ( r & PPI_FEATURE_CHANGE )
        {
            GetStringFromToken( ppi_op_names[(i* 2) + ((r & PPI_FEATURE_ON) ? 0 : 1)],
                                &String );

            StrCat( DesStr + StrLen( DesStr ), String );
            j++;
            gBS->FreePool(String);
            String = NULL;
            if ( j < count )
            {
                StrCat( DesStr + StrLen( DesStr ), strDelim );
            }

            if ( j == count - 2 )
            {
                strDelim = L", and ";
            }
        }
    }
}


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   displTpmNvOperations
//
// Description: Display requested actions as a list of operations
//
// INPUT:       IN extended_request erqst,
//              IN CHAR16           *DesStr
//
// OUTPUT:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
void displTpmNvOperations(
    IN UINT8        erqst,
    IN CHAR16           *DesStr )
{
    CHAR16 *String;
    EFI_STATUS Status;

    Status = GetStringFromToken( STRING_TOKEN(STR_TCG_CONFIGURATION), &String );
    if(EFI_ERROR(Status)) return;
    if(erqst == TCPA_PPIOP_SETNOPPIPROVISION_TRUE)
    {

        StrCat( DesStr + StrLen( DesStr ), String );
        gBS->FreePool(String);
        String = NULL;
        Status = GetStringFromToken( STRING_TOKEN(STR_TCG_SETNOPPIPROVISION) , &String );
        if(EFI_ERROR(Status)) return;
        StrCat( DesStr + StrLen( DesStr ), String );
        gBS->FreePool(String);

    }
    else if(erqst == TCPA_PPIOP_SETNOPPICLEAR_TRUE)
    {

        StrCat( DesStr + StrLen( DesStr ), String );
        gBS->FreePool(String);
        String = NULL;
        Status = GetStringFromToken( STRING_TOKEN(STR_TCG_SETNOPPICLEAR) , &String );
        if(EFI_ERROR(Status)) return;
        StrCat( DesStr + StrLen( DesStr ), String );
        gBS->FreePool(String);
        String = NULL;

    }
    else if(erqst == TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE)
    {

        StrCat( DesStr + StrLen( DesStr ), String );
        gBS->FreePool(String);
        String = NULL;
        Status = GetStringFromToken( STRING_TOKEN(STR_TCG_SETNOPPIMAINTENANCE) , &String );
        if(EFI_ERROR(Status)) return;
        StrCat( DesStr + StrLen( DesStr ), String );
        gBS->FreePool(String);
        String = NULL;

    }

}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   prompt_operation
//
// Description: Display information on the requested TPM operation to the user;
//
// INPUT:       IN int rqst
//
// OUTPUT:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
void prompt_operation(
    IN int rqst )
{
    int              i;
    UINT32           r;
    int              count = 0;
    EFI_STATUS       Status;
    CHAR16           TempChar;
    CHAR16           *StrBuffer = NULL;
    CHAR16           *String;
    UINT8            rq = (UINT8) rqst;
    UINTN            CurX, CurY;
    AMITSESETUP      TSEVar;
    UINTN            Size;

    extended_request erqst;

    erqst = extend_request( rq );

    //
    // Allocate the memory for the string buffer
    //
    Status = gBS->AllocatePool(
                 EfiBootServicesData,
                 sizeof (CHAR16) * 0x100,
                 (VOID*) &StrBuffer
             );

    if ( EFI_ERROR( Status ) || StrBuffer == NULL )
    {
        return;
    }

    gBS->SetMem( StrBuffer, sizeof (CHAR16) * 0x100, 0 );

    Status = gBS->LocateProtocol( &gAmiPostManagerProtocolGuid,
                                  NULL,
                                  &pAmiPostMgr );

    if ( EFI_ERROR( Status ))
    {
        return;
    }
    //
    // If we are silent mode switch to Bios post mode
    //

#if EFI_SPECIFICATION_VERSION<0x2000A
    Status = gBS->LocateProtocol (&gEfiHiiProtocolGuid, NULL, &Hii);
    ASSERT(!EFI_ERROR(Status));
#endif

    pAmiPostMgr->SwitchToPostScreen( );

    Size = sizeof (AMITSESETUP);

    Status = gRT->GetVariable(L"AMITSESetup", \
                              &AmiTseSetupGuid, \
                              NULL, \
                              &Size, \
                              &TSEVar );

    //
    // Count number of simple operations
    //
    for ( i = 0, r = erqst; i <= PPI_MAX_BASIC_OP; i++, r >>= 2 )
    {
        if ( r & PPI_FEATURE_CHANGE )
        {
            count++;
        }
    }
    DEBUG((DEBUG_INFO, "PPI operations count: %d\n", count));

    //
    // Display the string
    //
    if(Status == EFI_SUCCESS)
    {
        if(TSEVar.AMISilentBoot == 0x00)
        {
            pAmiPostMgr->DisplayPostMessage( StrBuffer );
        }
    }

//CurX, CurY;
    pAmiPostMgr->GetCurPos(&CurX, &CurY);

    CurX =  0;
    CurY -= PPI_DISPLAY_OFFSET;

    pAmiPostMgr->SetCurPos(CurX, CurY);
    Status = GetStringFromToken( STRING_TOKEN( STR_TCG_BLANK ), &String );
    if(EFI_ERROR(Status)) return;
    StrCat( StrBuffer, String );
    gBS->FreePool(String);
    String = NULL;

    for(i=0; i<PPI_DISPLAY_OFFSET; i++)
    {
        pAmiPostMgr->DisplayPostMessage( StrBuffer );
    }

    gBS->SetMem( StrBuffer, sizeof (CHAR16) * 0x100, 0 );

    if (rq < TCPA_PPIOP_SETNOPPIPROVISION_FALSE || rq > TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE)
    {
        Status =  GetStringFromToken( STRING_TOKEN( STR_TCG_CONFIGURATION ), &String );
        if(EFI_ERROR(Status)) return;
        StrCat( StrBuffer + StrLen( StrBuffer ), String );
        gBS->FreePool(String);
        String = NULL;
        displOperations( erqst, count, StrBuffer );
        Status = GetStringFromToken( STRING_TOKEN( STR_TPM ), &String );
        if(EFI_ERROR(Status)) return;
        StrCat( StrBuffer + StrLen( StrBuffer ), String );
        gBS->FreePool(String);
        String = NULL;
    }
    else
    {
        displTpmNvOperations( rq , StrBuffer );
    }

    if ( StrLen( StrBuffer ) > 79 )
    {
        for ( i = 79; i > 1; i-- )
        {
            if ( StrBuffer[i] == 0x0020 )
            {
                break;
            }
        }

        TempChar         = StrBuffer[i + 1];
        StrBuffer[i + 1] = 0000;
        //
        // Display the string
        //
        pAmiPostMgr->DisplayPostMessage( StrBuffer );
        StrBuffer[i + 1] = TempChar;
        pAmiPostMgr->DisplayPostMessage( &StrBuffer[i + 1] );
    }
    else
    {
        //
        // Display the string
        //
        pAmiPostMgr->DisplayPostMessage( StrBuffer );
    }

    //take care of Note messages
    if ( ppi_request == TCPA_PPIOP_ENABLE_ACTV
            || ppi_request == TCPA_PPIOP_ENACTVOWNER )
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_NOTE ), &String );
        if(EFI_ERROR(Status)) return;
        pAmiPostMgr->DisplayPostMessage( String );
        gBS->FreePool(String);
        String = NULL;
    }

    if ( ppi_request == TCPA_PPIOP_DEACT_DSBL
            || ppi_request == TCPA_PPIOP_DADISBLOWNER)
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_NOTE1 ), &String );
        if(EFI_ERROR(Status)) return;
        pAmiPostMgr->DisplayPostMessage( String );
        gBS->FreePool(String);
        String = NULL;
    }

    if ( ppi_request == TCPA_PPIOP_CLEAR_ENACT
            || ppi_request == TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV)
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_NOTE2 ), &String );
        if(EFI_ERROR(Status)) return;
        pAmiPostMgr->DisplayPostMessage( String );
        gBS->FreePool(String);
        String = NULL;
    }

    Status = GetStringFromToken( STRING_TOKEN( STR_TCG_BLANK ), &String );
    if(EFI_ERROR(Status)) return;
    pAmiPostMgr->DisplayPostMessage( String );
    gBS->FreePool(String);
    String = NULL;

    //take care of warning messages
    if ( ppi_request == TCPA_PPIOP_CLEAR_ENACT
            || ppi_request == TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV )
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_WARNING ), &String );
        if(EFI_ERROR(Status)) return;
        pAmiPostMgr->DisplayPostMessage( String );
        gBS->FreePool(String);
        String = NULL;
    }

    if ( ppi_request == TCPA_PPIOP_CLEAR
            || ppi_request == TCPA_PPIOP_ENABLE_ACTV_CLEAR)
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_WARNING1 ), &String );
        if(EFI_ERROR(Status)) return;
        pAmiPostMgr->DisplayPostMessage( String );
        gBS->FreePool(String);
        String = NULL;
    }

    if ( ppi_request == TCPA_PPIOP_DISABLE ||  ppi_request == TCPA_PPIOP_DEACTIVATE
            || ppi_request == TCPA_PPIOP_DEACT_DSBL || ppi_request == TCPA_PPIOP_DADISBLOWNER)
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_WARNING2 ), &String );
        if(EFI_ERROR(Status)) return;
        pAmiPostMgr->DisplayPostMessage( String );
        gBS->FreePool(String);
        String = NULL;
    }

    Status = GetStringFromToken( STRING_TOKEN( STR_TCG_BLANK ), &String );
    if(EFI_ERROR(Status)) return;
    pAmiPostMgr->DisplayPostMessage( String );
    gBS->FreePool(String);
    String = NULL;


    gBS->SetMem( StrBuffer, sizeof (CHAR16) * 0x100, 0 );

    //
    // Display the one line space
    //
    pAmiPostMgr->DisplayPostMessage( StrBuffer );

    if ( ppi_request == TCPA_PPIOP_CLEAR_ENACT || ppi_request ==
            TCPA_PPIOP_CLEAR  ||  ppi_request == TCPA_PPIOP_SETNOPPICLEAR_FALSE
            || ppi_request == TCPA_PPIOP_ENABLE_ACTV_CLEAR
            || ppi_request == TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV)
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_KEY1 ), &String );
        if(EFI_ERROR(Status)) return;
    }
    else
    {
        Status = GetStringFromToken( STRING_TOKEN( STR_TCG_KEY2 ), &String );
        if(EFI_ERROR(Status)) return;
    }

    StrCat( StrBuffer + StrLen( StrBuffer ), String );
    gBS->FreePool(String);
    String = NULL;

    //
    // Display the string
    //
    if (rq < TCPA_PPIOP_SETNOPPIPROVISION_FALSE || rq > TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE)
    {
        displOperations( erqst, count, StrBuffer );
        Status = GetStringFromToken( STRING_TOKEN( STR_TPM ), &String );
        if(EFI_ERROR(Status)) return;
        StrCat( StrBuffer + StrLen( StrBuffer ), String );
        gBS->FreePool(String);
        String = NULL;
    }
    else
    {
        if(rq == TCPA_PPIOP_SETNOPPIPROVISION_TRUE)
        {
            Status = GetStringFromToken( STRING_TOKEN(STR_TCG_SETNOPPIPROVISIONACCPET) , &String );
            if(EFI_ERROR(Status)) return;
            StrCat( StrBuffer + StrLen( StrBuffer ), String );
            gBS->FreePool(String);
            String = NULL;

        }
        else if(rq == TCPA_PPIOP_SETNOPPICLEAR_TRUE)
        {
            Status = GetStringFromToken( STRING_TOKEN(STR_TCG_SETNOPPICLEARACCPET) , &String );
            if(EFI_ERROR(Status)) return;
            StrCat( StrBuffer + StrLen( StrBuffer ), String );
            gBS->FreePool(String);
            String = NULL;
        }

        else if(rq == TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE){
            Status = GetStringFromToken( STRING_TOKEN(STR_TCG_SETNOPPIMAINTENANCEACCEPT) , &String );
            if(EFI_ERROR(Status)) return;
            StrCat( StrBuffer + StrLen( StrBuffer ), String );
            gBS->FreePool(String);
            String = NULL;
        }

    }

    if (StrLen( StrBuffer ) > 79 )
    {
        for ( i = 79; i > 1; i-- )
        {
            if ( StrBuffer[i] == 0x0020 )

            {
                break;
            }
        }

        TempChar         = StrBuffer[i + 1];
        StrBuffer[i + 1] = 0000;
        //
        // Display the string
        //
        pAmiPostMgr->DisplayPostMessage( StrBuffer );
        StrBuffer[i + 1] = TempChar;
        pAmiPostMgr->DisplayPostMessage( &StrBuffer[i + 1] );
    }
    else
    {
        //
        // Display the string
        //
        pAmiPostMgr->DisplayPostMessage( StrBuffer );
    }


    Status = GetStringFromToken( STRING_TOKEN( STR_TCG_CONFIRAMATION ), &String );
    if(EFI_ERROR(Status)) return;
    pAmiPostMgr->DisplayPostMessage( String );
    gBS->FreePool(String);
    String = NULL;

    gBS->SetMem( StrBuffer, sizeof (CHAR16) * 0x100, 0 );
    //StrCat (StrBuffer + StrLen (StrBuffer), String);

    //
    // Display the one line space
    //
    pAmiPostMgr->DisplayPostMessage( StrBuffer );

    gBS->FreePool( StrBuffer );

    return;
}

#if TPM_PASSWORD_AUTHENTICATION
BOOLEAN AdminUserActionRequired()
{

    EFI_STATUS Status;
    UINTN AdminPassInstallSize = sizeof(UINT32);
    UINT32 AdminPsswdInstallVar;

    Status = gRT->GetVariable(L"AmiTcgAdminPasswdIsInstalled",
                              &TcgPasswordAuthenticationGuid,
                              NULL,
                              &AdminPassInstallSize,
                              &AdminPsswdInstallVar);

    if(EFI_ERROR(Status))
    {
        DEBUG((DEBUG_INFO, "AdminUserActionRequire return FALSE\n"));
        return FALSE;
    }
    if(AdminPsswdInstallVar & AMI_PASSWORD_ADMIN)
    {
        DEBUG((DEBUG_INFO, "AdminUserActionRequire return TRUE\n"));
        return TRUE;
    }

    DEBUG((DEBUG_INFO, "AdminUserActionRequire return FALSE\n"));
    return FALSE;
}
#endif




void run_PPI_UI(
    IN EFI_EVENT ev,
    IN VOID      *ctx)
{

    static UINT8     RequestConfirmed  = FALSE;
    PERSISTENT_BIOS_TPM_MANAGEMENT_FLAGS_PROTOCOL *OemTpmBiosPolicy;
    PERSISTENT_BIOS_TPM_FLAGS  TpmNvflags;
    EFI_STATUS Status = EFI_NOT_FOUND;
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;
    TCG_CONFIGURATION               Config;
#if TPM_PASSWORD_AUTHENTICATION
    UINT32     GlobalVariable;
    UINTN      Size;


#if TPM_PASSWORD_AUTHENTICATION
    if ( PasswordSupplied  && AdminUserActionRequired())
    {
        RequestConfirmed = TRUE;
        goto CheckConfirm;
    }
    else if(PasswordSupplied && !(AdminUserActionRequired()))
    {
        goto CheckConfirm;
    }
#endif
    if(IsRunPpiUIAlreadyDone ==TRUE)
    {
        return;
    }

    IsRunPpiUIAlreadyDone = TRUE;

    if ( PasswordSupplied )
    {
        goto CheckConfirm;
    }
#else
    if(IsRunPpiUIAlreadyDone ==TRUE)
    {
        return;
    }
#endif


    if (pAmiPostMgr == NULL)
    {
        Status = gBS->LocateProtocol( &gAmiPostManagerProtocolGuid,
                                      NULL,
                                      &pAmiPostMgr );

        if (EFI_ERROR(Status))
        {
            return;
        }
    }


    //
    // Calling GetPostStatus() to check current TSE_POST_STATUS
    //
    if (PcdGetBool(PcdPostStatusCheck))
    {
        DEBUG((DEBUG_INFO, "\n PostStatus Check is True \n"));
        if(pAmiPostMgr->GetPostStatus() < TSE_POST_STATUS_IN_POST_SCREEN) return;
    }

    IsRunPpiUIAlreadyDone = TRUE;

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status))
    {
        return;
    }


    gBS->CopyMem(&Config, &ProtocolInstance->ConfigFlags, sizeof(TCG_CONFIGURATION));

    Config.PpiSetupSyncFlag = TRUE;

    ProtocolInstance->UpdateStatusFlags(&Config, TRUE);

    Status = gBS->LocateProtocol( &AmiBiosPpiFlagsManagementGuid, NULL, &OemTpmBiosPolicy);
    if(!EFI_ERROR(Status))
    {
        Status = OemTpmBiosPolicy->ReadBiosTpmflags(&TpmNvflags);
        if(!EFI_ERROR(Status))
        {

            switch( ppi_request)
            {
                case 1:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 2:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 3:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 4:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 5:
                    if(TpmNvflags.NoPpiClear == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 6:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 7:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 8:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 9:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 10:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 11:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 12:
                    //TPM_SetCapability command
                    if(TpmNvflags.NoPpiMaintenance == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 13:
                    if(TpmNvflags.NoPpiProvision == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 14:
                    if(TpmNvflags.NoPpiProvision == TRUE   &&   TpmNvflags.NoPpiClear == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 15:
                    //SetNoPpiProvision_false
                    RequestConfirmed = TRUE;
                    break;
                case 16:
                    RequestConfirmed = FALSE;
                    break;
                case 17:
                    //SetNoPpiClear_false
                    RequestConfirmed = TRUE;
                    break;
                case 18:
                    //SetNoPpiClear_True
                    RequestConfirmed = FALSE;
                    break;
                case 19:
                    //SetNoPpiMaintenance_False
                    RequestConfirmed = TRUE;
                    break;
                case 20:
                    RequestConfirmed = FALSE;
                    break;
                case 21:
                    //Enable + Activate + Clear
                    if(TpmNvflags.NoPpiClear == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                case 22:
                    //Enable + Activate + clear + Enable + Activate
                    if(TpmNvflags.NoPpiProvision == TRUE   &&   TpmNvflags.NoPpiClear == TRUE)
                    {
                        RequestConfirmed = TRUE;
                    }
                    break;
                default:
                    break;

            }

        }
    }

#if (defined(CONFIRM_SETUP_CHANGE) && (CONFIRM_SETUP_CHANGE == 1))
    if(Config.Reserved5 == TRUE)
    {
        Config.Reserved5 = FALSE;
        RequestConfirmed = FALSE;
        ProtocolInstance->UpdateStatusFlags(&Config, TRUE);
    }
#endif

#if TPM_PASSWORD_AUTHENTICATION
    prompt_operation( ppi_request );
    RequestConfirmed = confirmUser( );
#else
    if(RequestConfirmed == FALSE)
    {
        prompt_operation( ppi_request );
        RequestConfirmed = confirmUser( );
    }
#endif

#if TPM_PASSWORD_AUTHENTICATION
    if ( check_authenticate_set( ) && RequestConfirmed )
    {
        GlobalVariable = 0x58494d41; // "AMIX"
        Status         = gRT->SetVariable(
                             L"AskPassword",
                             &gTcgEfiGlobalVariableGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS,
                             sizeof (UINT32),
                             &GlobalVariable
                         );

        if ( EFI_ERROR( Status ))
        {
            Status = gRT->GetVariable(
                         L"AskPassword",
                         &gTcgEfiGlobalVariableGuid,
                         NULL,
                         &Size,
                         &GlobalVariable
                     );
            GlobalVariable = 0x58494d41; // "AMIX"
            Status         = gRT->SetVariable(
                                 L"AskPassword",
                                 &gTcgEfiGlobalVariableGuid,
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 Size,
                                 &GlobalVariable
                             );
        }
#if TPM_PASSWORD_AUTHENTICATION
        SignalProtocolEvent(&TcgPasswordAuthenticationGuid);
#endif
    }

    if(AdminUserActionRequired())
    {
        DEBUG((DEBUG_INFO, "Admin password is set"));
        return;
    }
    else if(check_authenticate_set( ) == TRUE && RequestConfirmed == TRUE)
    {
        goto AuthCheckEnabledNoAdmin;
    }

CheckConfirm:
#endif

    if ( !RequestConfirmed )
    {
        DEBUG((DEBUG_INFO, "\tPPI request was turned down: user cancel\n"));
        DEBUG((DEBUG_INFO, "Another key pressed for PPI setup, Write_result"));
        WritePpiResult( ppi_request, TCPA_PPI_USERABORT );
        return;
    }

#if TPM_PASSWORD_AUTHENTICATION

    if ( check_authenticate_set( ))
    {
        GlobalVariable = 0;
        Status         = gRT->SetVariable(
                             L"AskPassword",
                             &gTcgEfiGlobalVariableGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS,
                             sizeof (UINT32),
                             &GlobalVariable
                         );

        if ( EFI_ERROR( Status ))
        {
            Status = gRT->GetVariable(
                         L"AskPassword",
                         &gTcgEfiGlobalVariableGuid,
                         NULL,
                         &Size,
                         &GlobalVariable
                     );
            GlobalVariable = 0;
            Status         = gRT->SetVariable(
                                 L"AskPassword",
                                 &gTcgEfiGlobalVariableGuid,
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 Size,
                                 &GlobalVariable
                             );
        }

        if ( !check_user_is_administrator( ))
        {
            WritePpiResult( ppi_request, TCPA_PPI_USERABORT );
            return;
        }
    }


AuthCheckEnabledNoAdmin:
#endif
    DEBUG((DEBUG_INFO, "F10 pressed for PPI setup, execute request"));

    //verify and do TPM related Ppi over here
    if(ppi_request >= TCPA_PPIOP_SETNOPPIPROVISION_FALSE &&
            ppi_request <= TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE )
    {
        if(ppi_request == TCPA_PPIOP_SETNOPPIPROVISION_FALSE)
        {
            if(TpmNvflags.NoPpiProvision != FALSE)
            {
                TpmNvflags.NoPpiProvision = 0;
                Status = OemTpmBiosPolicy->SetBiosTpmflags(&TpmNvflags);
                if(Status)
                {
                    WritePpiResult( ppi_request, TCPA_PPI_BIOSFAIL );
                }
                else
                {
                    WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
                }
            }
            else
            {
                WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
            }
        }
        else if(ppi_request == TCPA_PPIOP_SETNOPPIPROVISION_TRUE)
        {
            if(TpmNvflags.NoPpiProvision != TRUE)
            {
                TpmNvflags.NoPpiProvision = TRUE;
                Status = OemTpmBiosPolicy->SetBiosTpmflags(&TpmNvflags);
                if(Status)
                {
                    WritePpiResult( ppi_request, TCPA_PPI_BIOSFAIL );
                }
                else
                {
                    WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
                }
            }
            else
            {
                WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
            }
        }
        else if(ppi_request == TCPA_PPIOP_SETNOPPICLEAR_FALSE)
        {
            if(TpmNvflags.NoPpiClear != FALSE)
            {
                TpmNvflags.NoPpiClear = 0;
                Status = OemTpmBiosPolicy->SetBiosTpmflags(&TpmNvflags);
                if(Status)
                {
                    WritePpiResult( ppi_request, TCPA_PPI_BIOSFAIL );
                }
                else
                {
                    WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
                }
            }
            else
            {
                WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
            }
        }
        else if(ppi_request == TCPA_PPIOP_SETNOPPICLEAR_TRUE)
        {
            if(TpmNvflags.NoPpiClear != TRUE)
            {
                TpmNvflags.NoPpiClear = TRUE;
                Status = OemTpmBiosPolicy->SetBiosTpmflags(&TpmNvflags);
                if(Status)
                {
                    WritePpiResult( ppi_request, TCPA_PPI_BIOSFAIL );
                }
                else
                {
                    WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
                }
            }
            else
            {
                WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
            }

        }
        else if(ppi_request == TCPA_PPIOP_SETNOPPIMAINTENANCE_FALSE)
        {
            if(TpmNvflags.NoPpiMaintenance != FALSE)
            {
                TpmNvflags.NoPpiMaintenance = FALSE;
                Status = OemTpmBiosPolicy->SetBiosTpmflags(&TpmNvflags);
                if(Status)
                {
                    WritePpiResult( ppi_request, TCPA_PPI_BIOSFAIL );
                }
                else
                {
                    WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
                }
            }
            else
            {
                WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
            }
        }
        else if(ppi_request == TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE)
        {
            if(TpmNvflags.NoPpiMaintenance != TRUE)
            {
                TpmNvflags.NoPpiMaintenance = TRUE;
                Status = OemTpmBiosPolicy->SetBiosTpmflags(&TpmNvflags);
                if(Status)
                {
                    WritePpiResult( ppi_request, TCPA_PPI_BIOSFAIL );
                }
                else
                {
                    WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
                }
            }
            else
            {
                WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
            }
        }
        else
        {
            WritePpiResult( ppi_request, (UINT16)EFI_SUCCESS );
        }

        RequestSystemReset( EfiResetCold);
    }
    else
    {
        if(ppi_request != 0)
        {
            if ( execute_request( ppi_request ) == 0 )
            {
                ppi_request = 0;
                RequestSystemReset( EfiResetCold);
            }
            else
            {
                gST->ConOut->OutputString(
                    gST->ConOut,
                    L"\n\r Error trying to complete TPM request.\n\r" );
                RequestSystemReset( EfiResetCold);
            }
        }
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SignalProtocolEvent
//
// Description:
//  Internal function that installs/uninstalls protocol with a specified
//  GUID and NULL interface. Such protocols can be used as event
//  signaling mechanism.
//
// Input:
//      IN EFI_GUID *TcgPasswordAuthenticationGuid - Pointer to the protocol GUID
//
// Output:
//     VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

#if TPM_PASSWORD_AUTHENTICATION

VOID
SignalProtocolEvent (
    IN EFI_GUID    *TcgPasswordAuthenticationGuid )
{
    EFI_HANDLE  Handle = NULL;
    gBS->InstallProtocolInterface (
        &Handle, TcgPasswordAuthenticationGuid, EFI_NATIVE_INTERFACE, NULL
    );
    gBS->UninstallProtocolInterface (
        Handle, TcgPasswordAuthenticationGuid, NULL
    );
    return;
}
#endif

#if TPM_PASSWORD_AUTHENTICATION
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   OnAdminPasswordValid
//
// Description: Sets AdminPasswordValid to TRUE [If TPM_PASSWORD_AUTHENTICATION]
//
//
// Input:       IN EFI_EVENT ev,
//              IN VOID *ctx
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
EFI_STATUS OnAdminPasswordValid(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    AdminPasswordValid = TRUE;
    return EFI_SUCCESS;
}



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   OnPasswordSupplied
//
// Description: Sets PasswordSupplied to TRUE and runs PPI User Interface
//
//
// Input:       IN EFI_EVENT ev,
//              IN VOID *ctx
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
EFI_STATUS OnPasswordSupplied(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    DEBUG((DEBUG_INFO, "OnPasswordSupplied Entry\n"));
    PasswordSupplied = TRUE;
    run_PPI_UI( ev, ctx);
    return EFI_SUCCESS;
}


//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: check_authenticate_set
//
// Description: checks if password authentication set in Setup
//
//
// Input:
//
// Output:      BOOLEAN
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
BOOLEAN check_authenticate_set( )
{
    EFI_STATUS Status;
    SETUP_DATA *sd           = NULL;
    BOOLEAN    CheckPassword = FALSE;

    if ( AuthenticateSet )
    {
        return TRUE;
    }
    Status = getSetupData( &sd, NULL, NULL );

    if ( !EFI_ERROR( Status ))
    {
        CheckPassword = sd->TpmAuthenticate;
    }

    if ( CheckPassword )
    {
        AuthenticateSet = TRUE;
        gBS->FreePool(sd);
        return TRUE;
    }
    gBS->FreePool(sd);
    return FALSE;
}

//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: check_user_is_administrator
//
// Description: check if user was authenticated as an administrator (optional)
//
//
// Input:
//
// Output:      BOOLEAN
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
BOOLEAN check_user_is_administrator( )
{
    if ( check_authenticate_set( ))
    {
        if ( AdminPasswordValid )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}


EFI_STATUS PasswordAuthHelperFunction( )
{
    UINT32      GlobalVariable = 0;
    EFI_STATUS  Status;
    EFI_EVENT   ev;
    static VOID *reg;

    //
    // Hook Up Admin Password Notification ;
    //
    AuthenticateSet    = check_authenticate_set( );
    AdminPasswordValid = FALSE;
    PasswordSupplied   = FALSE;
    {
        Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_NOTIFY,
                                   OnAdminPasswordValid,
                                   &reg,
                                   &ev );
        ASSERT( !EFI_ERROR( Status ));
        Status = gBS->RegisterProtocolNotify( &gAmiTseAdminPasswordValidGuid,
                                              ev,
                                              &reg );
    }

    {
        Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_NOTIFY,
                                   OnPasswordSupplied,
                                   &reg,
                                   &ev );
        ASSERT( !EFI_ERROR( Status ));
        Status = gBS->RegisterProtocolNotify( &gAmiTsePasswordPromptExitGuid,
                                              ev,
                                              &reg );
    }

    Status = gRT->SetVariable(
                 L"AskPassword",
                 &gTcgEfiGlobalVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                 sizeof (UINT32),
                 &GlobalVariable );

    return Status;
}



#endif


//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ResetOSTcgVar
//
// Description: Function to reset TCG variables on certain scenerions
//
// Input:
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
void ResetOSTcgVar( )
{
    EFI_STATUS Status;
    AMI_PPI_NV_VAR Temp;
    UINT32 Attribs = EFI_VARIABLE_NON_VOLATILE
                     | EFI_VARIABLE_BOOTSERVICE_ACCESS
                     | EFI_VARIABLE_RUNTIME_ACCESS;
    UINTN TempSize = sizeof (AMI_PPI_NV_VAR);

    Temp.RQST    = 0;
    Temp.RCNT    = 0;
    Temp.ERROR   = 0;
    Temp.Flag    = 0;
    Temp.AmiMisc = 0;

    Status = gRT->GetVariable(
                 L"AMITCGPPIVAR",
                 &AmitcgefiOsVariableGuid,
                 &Attribs,
                 &TempSize,
                 &Temp );

    if ( EFI_ERROR( Status ) || Temp.RQST != 0 )
    {
        Temp.RQST    = 0;
        Temp.RCNT    = 0;
        Temp.ERROR   = 0;
        Temp.Flag    = 0;
        Temp.AmiMisc = 0;

#if NVRAM_VERSION > 6
        Status =  TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
                  sizeof (AMI_PPI_NV_VAR), &Temp);

#else
        Status =  TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                  sizeof (AMI_PPI_NV_VAR), &Temp);
#endif

        DEBUG((DEBUG_INFO, "ResetOSTcgVar::TcgSetVariableWithNewAttributes Status =%r",Status));
    }
}



#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   Tpm20OnVariableLockProtocolGuid
//
// Description: Lock Tcg Nvram Variable
//
// Input:       NONE
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
VOID
EFIAPI
Tpm20OnVariableLockProtocolGuid (
    IN      EFI_EVENT                 Event,
    IN      VOID                      *Context
)
{
    EDKII_VARIABLE_LOCK_PROTOCOL    *LockProtocol;
    EFI_STATUS                      Status;

    DEBUG((DEBUG_INFO, "Tpm20OnVariableLockProtocolGuid callback entry\n"));

    Status =  gBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid, NULL, &LockProtocol);
    if(!EFI_ERROR(Status))
    {
        Status = LockProtocol->RequestToLock(LockProtocol, L"TPMPERBIOSFLAGS", &FlagsStatusguid);
        ASSERT_EFI_ERROR(Status);
        Status = LockProtocol->RequestToLock(LockProtocol, L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid);
    }
    ASSERT_EFI_ERROR(Status);
}
#endif




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   ProcessTcgPpiRequest
//
// Description: Process Tcg Ppi requests
//
// Input:       NONE
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
EFIAPI ProcessTcgPpiRequest()
{
    EFI_STATUS                  Status;
    UINT32                      Temp = 0;
    UINT32                      Attribs = EFI_VARIABLE_NON_VOLATILE
                                          | EFI_VARIABLE_BOOTSERVICE_ACCESS
                                          | EFI_VARIABLE_RUNTIME_ACCESS;
    UINTN                       TempSize = sizeof (UINT32);
    AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL      *POverride;
    TCG_PLATFORM_SETUP_PROTOCOL     *ProtocolInstance;
    TCG_CONFIGURATION               Config;
    PERSISTENT_BIOS_TPM_MANAGEMENT_FLAGS_PROTOCOL *OemTpmBiosPolicy;
    PERSISTENT_BIOS_TPM_FLAGS  TpmNvflags;
    EFI_TCG_PROTOCOL            *tcg;
    EFI_TPM_DEVICE_PROTOCOL     *TpmDevice;
    TPM_RESULT                  tpmStatus = 0;
    UINT32                      Intrqst;
    UINT8                       one  = 1;
    UINT8                       zero  = 0;
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
    EFI_EVENT           VarLockEvent;
    static VOID         *VarLockreg;
#endif



    Status = gBS->LocateProtocol(&AmiProcessTcgPpiRequestGuid,
                                 NULL,
                                 &POverride );

    if(!EFI_ERROR(Status))
    {
        return (POverride->Function());
    }

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &ProtocolInstance);
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    //copy NV confirmation flags for O.S. request
    Status = gBS->LocateProtocol( &AmiBiosPpiFlagsManagementGuid, NULL, &OemTpmBiosPolicy);
    if(!EFI_ERROR(Status))
    {
        Status = OemTpmBiosPolicy->ReadBiosTpmflags(&TpmNvflags);
        if(EFI_ERROR(Status))
        {
            TpmNvflags.NoPpiProvision = 0;
            TpmNvflags.NoPpiClear = 0;
            TpmNvflags.NoPpiMaintenance = 0;
        }
    }
    else
    {
        //all request require confirmation
        TpmNvflags.NoPpiProvision = 0;
        TpmNvflags.NoPpiClear = 0;
        TpmNvflags.NoPpiMaintenance = 0;
    }

#if NVRAM_VERSION > 6
    Status =  TcgSetVariableWithNewAttributes(L"TPMPERBIOSFLAGS", &FlagsStatusguid, \
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
              sizeof (PERSISTENT_BIOS_TPM_FLAGS), &TpmNvflags);
#else
    Status =  TcgSetVariableWithNewAttributes(L"TPMPERBIOSFLAGS", &FlagsStatusguid, \
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
              sizeof (PERSISTENT_BIOS_TPM_FLAGS), &TpmNvflags );
#endif


#if TPM_PASSWORD_AUTHENTICATION
    Status = PasswordAuthHelperFunction( );
#endif



    ppi_request  = ReadPpiRequest( );
    ppi_request &= 0xFF;

    Status = gRT->GetVariable(
                 L"TcgINTPPI",
                 &gTcgEfiGlobalVariableGuid,
                 &Attribs,
                 &TempSize,
                 &Temp );

    if(EFI_ERROR(Status))
    {
        //if error do nothing. It is alright for
        //this variable to not exist.
        Temp = 0;
    }


    DEBUG((DEBUG_INFO, "\n PPI_request is:  %x \n", ppi_request));

    if (Temp != 0)
    {
        gBS->CopyMem(&Config, &ProtocolInstance->ConfigFlags, sizeof(TCG_CONFIGURATION));

        Config.PpiSetupSyncFlag = TRUE;

        ProtocolInstance->UpdateStatusFlags(&Config, TRUE);

        Status = gBS->LocateProtocol( &gEfiTpmDeviceProtocolGuid,NULL, &TpmDevice);
        if ( EFI_ERROR( Status ))
        {
            return Status;
        }

        Status = gBS->LocateProtocol( &gEfiTcgProtocolGuid, NULL, &tcg );

        if ( EFI_ERROR( Status ))
        {
            return Status;
        }

        if(!IsTcmSupportType())
        {
            TpmDevice->Init( TpmDevice );

            Intrqst  = (TCPA_PPIOP_ENACTVOWNER << 4 | TCPA_PPIOP_OWNER_ON);
            if ( Temp == Intrqst)
            {
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_SetOwnerInstall, 1, &one );
                WritePpiResult( ppi_request >> 04, (UINT16)( TPM_H2NL( tpmStatus )));
                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }
                Temp = 0;

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                RequestSystemReset( EfiResetCold );
            }
            //no ppi request so  check for setup request
            Intrqst  = (TCPA_PPIOP_ENABLE_ACTV_CLEAR << 4 | TCPA_PPIOP_CLEAR);
            if ( Temp == Intrqst)
            {
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_ForceClear, 0, 0 );

                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }
                Temp = 0;

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                RequestSystemReset( EfiResetCold );
            }

            //no ppi request so  check for setup request
            Intrqst  = (TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV << 4 | TCPA_PPIOP_ENABLE_ACTV);
            if ( Temp == Intrqst)
            {
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_PhysicalEnable, 0, 0 );
                TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ENABLED | EFI_SOFTWARE_DXE_BS_DRIVER);
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TPM_ORD_PhysicalSetDeactivated, 1,&zero );
                }else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                WritePpiResult( ppi_request >> 04, (UINT16)( TPM_H2NL( tpmStatus )));
                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }
                Temp = 0;

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                RequestSystemReset( EfiResetCold );
            }
        }
        else
        {
            Intrqst  = (TCPA_PPIOP_ENACTVOWNER << 4 | TCPA_PPIOP_OWNER_ON);
            if ( Temp == Intrqst)
            {
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_SetOwnerInstall, 1, &one );
                WritePpiResult( ppi_request >> 04, (UINT16)( TPM_H2NL( tpmStatus )));
                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }
                Temp = 0;

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                RequestSystemReset( EfiResetCold );
            }
            //no ppi request so  check for setup request
            Intrqst  = (TCPA_PPIOP_ENABLE_ACTV_CLEAR << 4 | TCPA_PPIOP_CLEAR);
            if ( Temp == Intrqst)
            {
                tpmStatus = SendTpmCommand( tcg, TPM_ORD_ForceClear, 0, 0 );

                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }
                Temp = 0;

                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                RequestSystemReset( EfiResetCold );
            }

            //no ppi request so  check for setup request
            Intrqst  = (TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV << 4 | TCPA_PPIOP_ENABLE_ACTV);
            if ( Temp == Intrqst)
            {
                tpmStatus = SendTpmCommand( tcg, TCM_ORD_PhysicalEnable, 0, 0 );
                if(!tpmStatus)
                {
                    tpmStatus = SendTpmCommand( tcg,TCM_ORD_PhysicalSetDeactivated, 1,&zero );
                    TpmDxeReportStatusCode(EFI_PROGRESS_CODE, AMI_SPECIFIC_TPM_1_2_DEVICE_ACTIVATED | EFI_SOFTWARE_DXE_BS_DRIVER);
                }else{
                    TpmDxeReportStatusCode(EFI_ERROR_CODE|EFI_ERROR_MAJOR, AMI_SPECIFIC_TPM_ERR_TPM_1_2_ENABLE_FAIL | EFI_SOFTWARE_DXE_BS_DRIVER);
                }
                WritePpiResult( ppi_request >> 04, (UINT16)( TPM_H2NL( tpmStatus )));
                if(!IsTcmSupportType())
                {
                    TpmDevice->Close( TpmDevice );
                }
                Temp = 0;
                Status =  TcgSetVariableWithNewAttributes(L"TcgINTPPI", &gTcgEfiGlobalVariableGuid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                          sizeof (UINT32), &Temp );

                RequestSystemReset( EfiResetCold );
            }
        }
    }

    if(ppi_request == TCPA_PPIOP_SETOPAUTH)
    {
        //these commands are optional and not supported
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
        Status = gBS->CreateEvent (EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_CALLBACK,
                                   Tpm20OnVariableLockProtocolGuid,
                                   NULL,
                                   &VarLockEvent);

        if(!EFI_ERROR(Status))
        {
            Status = gBS->RegisterProtocolNotify(
                         &gEdkiiVariableLockProtocolGuid,
                         VarLockEvent,
                         &VarLockreg );
        }
#endif
        return EFI_SUCCESS;
    }


    if(ppi_request > 0  && ppi_request <= TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV)
    {
        EFI_EVENT   ev;
        static VOID *reg;

#if TPM_PASSWORD_AUTHENTICATION

        AuthenticateSet = check_authenticate_set( );

#endif

        Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_CALLBACK,
                                   run_PPI_UI,
                                   0,
                                   &ev );

        if(EFI_ERROR(Status))
        {
            return Status;
        }

        Status = gBS->RegisterProtocolNotify(
                     &gBdsAllDriversConnectedProtocolGuid,
                     ev,
                     &reg );
        if(EFI_ERROR(Status))
        {
            return Status;
        }
    }
    else
    {
#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
        Status = gBS->CreateEvent (EFI_EVENT_NOTIFY_SIGNAL,
                                   EFI_TPL_CALLBACK,
                                   Tpm20OnVariableLockProtocolGuid,
                                   NULL,
                                   &VarLockEvent);

        if(!EFI_ERROR(Status))
        {
            Status = gBS->RegisterProtocolNotify(
                         &gEdkiiVariableLockProtocolGuid,
                         VarLockEvent,
                         &VarLockreg );
        }
#endif

    }

    return Status;
}

VOID OnLegacyBoot(
    IN EFI_EVENT    Event,
    IN VOID       *Context
)
{
    BOOLEAN                        Support = FALSE;
    EFI_STATUS      Status;
    //
    // Measure handoff tables
    //
    Support = IsTcmSupportType();

    if(Support){
       Status = TcmMeasureHandoffTables( );
    }else{
       Status = MeasureHandoffTablesFuncPtr( );
    }
    return;
}
AMI_TCG_PLATFORM_PROTOCOL   AmiTcgPlatformProtocol =
{
    MEASURE_CPU_MICROCODE_DXE_FUNCTION,
    MEASURE_PCI_OPTION_ROM_DXE_FUNCTION,
    ProcessTcgSetup,
    ProcessTcgPpiRequest,
    SetTcgReadyToBoot,
    GetProtocolVersion,
    ResetOSTcgVar
};

EFI_STATUS AmiTcgPlatformLoadStrings(
    EFI_HANDLE ImageHandle, EFI_HII_HANDLE *pHiiHandle
)
{
    EFI_STATUS                      Status;
    EFI_HII_PACKAGE_LIST_HEADER     *PackageList;
    EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;

    //
    // Retrieve HII package list from ImageHandle
    //
    Status = gBS->OpenProtocol (
                 ImageHandle,
                 &gEfiHiiPackageListProtocolGuid,
                 (VOID **) &PackageList,
                 ImageHandle,
                 NULL,
                 EFI_OPEN_PROTOCOL_GET_PROTOCOL
             );
    if (EFI_ERROR (Status))
    {
        DEBUG((DEBUG_INFO,"gEfiHiiPackageListProtocolGuid protocol is not found\n"));
        return Status;
    }

    Status = gBS->LocateProtocol (
                 &gEfiHiiDatabaseProtocolGuid,
                 NULL,
                 &HiiDatabase
             );
    if (EFI_ERROR (Status))
    {
        DEBUG((DEBUG_INFO,"gEfiHiiDatabaseProtocolGuid protocol is not found\n"));
        return Status;
    }

    //
    // Publish HII package list to HII Database.
    //

    Status = HiiDatabase->NewPackageList (
                 HiiDatabase,
                 PackageList,
                 NULL,
                 pHiiHandle
             );
    DEBUG((DEBUG_INFO,"NewPackageList status: %r\n",Status));
    return Status;
}


EFI_STATUS
EFIAPI AmiTcgPlatformDXE_Entry(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status;
    
    Status = gBS->InstallProtocolInterface(
                 &ImageHandle,
                 &gAmiTcgPlatformProtocolguid,
                 EFI_NATIVE_INTERFACE,
                 &AmiTcgPlatformProtocol);

    
    Status = EfiCreateEventLegacyBootEx (EFI_TPL_NOTIFY,
                       OnLegacyBoot,
                       NULL,
                       &gLegacyBootEvent );

   
    ASSERT_EFI_ERROR(Status);

    Status = AmiTcgPlatformLoadStrings( ImageHandle, &gHiiHandle );

    ASSERT_EFI_ERROR(Status);
       
#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))
        Status = MeasureSecurebootVariablesFuncPtr ();

        if ( EFI_ERROR( Status ))
        {
            DEBUG((DEBUG_ERROR, "Error Measuring Secure Vars\n"));
        }

        Status = MeasureSeparatorEvent(7);
#endif

      return EFI_SUCCESS;
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
