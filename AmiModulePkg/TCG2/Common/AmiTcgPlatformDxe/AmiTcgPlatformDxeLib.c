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
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformDxeLib.c 1     10/08/13 12:04p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:04p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/AmiTcgPlatform/AmiTcgPlatformDxeLib.c $
//
// 1     10/08/13 12:04p Fredericko
// Initial Check-In for Tpm-Next module
//
// 1     7/10/13 5:55p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 7     10/30/12 10:47a Fredericko
//
// 6     5/20/12 2:14p Fredericko
//
// 5     9/27/11 10:33p Fredericko
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
// 4     7/25/11 3:46a Fredericko
// [TAG]        EIP65177
// [Category]   Spec Update
// [Severity]   Minor
// [Description]    Tcg Ppi Spec ver 1.2 update
//
// 3     3/29/11 5:52p Fredericko
// Changes for core 464 and ACPI tables support
//
// 2     3/29/11 2:28p Fredericko
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
// Name:    AmiTcgPlatformDxeLib.c
//
// Description: Function file for AmiTcgPlatformDxe library
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiTcg/AmiTcgPlatformDxeLib.h>
#include<Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/TcgTcmService.h>
#include <Guid/AmiTcgGuidIncludes.h>
#include <AmiTcg/tcg.h>

#if defined (CORE_BUILD_NUMBER) && (CORE_BUILD_NUMBER > 0xA) && NVRAM_VERSION > 6
#include <Protocol/VariableLock.h>
#endif


extern EFI_GUID gEfiHobListGuid;

EFI_STATUS getSetupData (
    SETUP_DATA** ppsd,
    UINT32    * pattr,
    UINTN     * psz );

//**********************************************************************
//                  TCG_Helper functions
//**********************************************************************
#pragma pack(1)
typedef struct _TCG_DXE_PRIVATE_DATA
{
    EFI_TCG_PROTOCOL        TcgServiceProtocol;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;
} TCG_DXE_PRIVATE_DATA;

typedef struct _TCM_DXE_PRIVATE_DATA
{
    EFI_TCM_PROTOCOL        TcgServiceProtocol;
    EFI_TPM_DEVICE_PROTOCOL *TpmDevice;
} TCM_DXE_PRIVATE_DATA;
#pragma pack()

#define _CR( Record, TYPE,\
       Field )((TYPE*) ((CHAR8*) (Record) - (CHAR8*) &(((TYPE*) 0)->Field)))

#define TCG_DXE_PRIVATE_DATA_FROM_THIS( This )  \
    _CR( This, TCG_DXE_PRIVATE_DATA, TcgServiceProtocol )

#define TCM_DXE_PRIVATE_DATA_FROM_THIS( This )  \
    _CR( This, TCM_DXE_PRIVATE_DATA, TcgServiceProtocol )


EFI_STATUS
EFIAPI TcgCommonPassThrough(
    IN VOID                    *Context,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers )
{
    TCG_DXE_PRIVATE_DATA *Private;

    Private = TCG_DXE_PRIVATE_DATA_FROM_THIS( Context );
    return Private->TpmDevice->Transmit(
               Private->TpmDevice,
               NoInputBuffers,
               InputBuffers,
               NoOutputBuffers,
               OutputBuffers
           );
}



EFI_STATUS TcgSetVariableWithNewAttributes(
    IN CHAR16 *Name, IN EFI_GUID *Guid, IN UINT32 Attributes,
    IN UINTN DataSize, IN VOID *Data
)
{
    EFI_STATUS Status;
    Status = gRT->SetVariable(Name, Guid, Attributes, DataSize, Data);
    if (!EFI_ERROR(Status) || Status != EFI_INVALID_PARAMETER){
        if(Status == EFI_NOT_FOUND)//if not found; just set the variable
        {
            gRT->SetVariable(Name, Guid, Attributes, DataSize, Data);
        }
        return Status;
    }

    Status = gRT->SetVariable(Name, Guid, 0, 0, NULL);
    if (EFI_ERROR(Status)) return Status;

    return gRT->SetVariable(Name, Guid, Attributes, DataSize, Data);
}




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcmCommonPassThrough
//
// Description: Helper function for TCM transmit command
//
// Input:       VOID *Context
//              UINT32 NoInputBuffers
//              TPM_TRANSMIT_BUFFER InputBuffers
//              UINT32 NoOutputBuffers
//              TPM_TRANSMIT_BUFFER OutputBuffers
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
EFIAPI TcmCommonPassThrough(
    IN VOID                    *Context,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers )
{
    TCM_DXE_PRIVATE_DATA *Private;

    Private = TCM_DXE_PRIVATE_DATA_FROM_THIS( Context );
    return Private->TpmDevice->Transmit(
               Private->TpmDevice,
               NoInputBuffers,
               InputBuffers,
               NoOutputBuffers,
               OutputBuffers
           );
}





//*******************************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetTcgWakeEventType
//
// Description: Reads and Reports the source of the wake-up event.
//
// Input:       IN OUT UINT8   *pWake   - output parameter returns the indication of the
//                                        type of the wakup source:
//                              one of the following:
//                              SMBIOS_WAKEUP_TYPE_OTHERS
//                              SMBIOS_WAKEUP_TYPE_UNKNOWN
//                              SMBIOS_WAKEUP_TYPE_APM_TIMER
//                              SMBIOS_WAKEUP_TYPE_MODEM_RING
//                              SMBIOS_WAKEUP_TYPE_LAN_REMOTE
//                              SMBIOS_WAKEUP_TYPE_POWER_SWITCH
//                              SMBIOS_WAKEUP_TYPE_PCI_PME
//                              SMBIOS_WAKEUP_TYPE_AC_POWER_RESTORED
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//******************************************************************************
EFI_STATUS GetTcgWakeEventType(
    IN OUT UINT8 *pWake  )
{
    EFI_BOOT_MODE           *BootMode = NULL;
    UINTN                   NoTableEntries;
    EFI_CONFIGURATION_TABLE *ConfigTable;
    VOID                    *HobStart;

    *pWake = SMBIOS_WAKEUP_TYPE_UNKNOWN;

    NoTableEntries = gST->NumberOfTableEntries,
    ConfigTable    = gST->ConfigurationTable;

    while ( NoTableEntries > 0 )
    {
        NoTableEntries--;

        if ( !CompareMem(
                    &ConfigTable[NoTableEntries].VendorGuid,
                    &gEfiHobListGuid, sizeof(EFI_GUID)
                ))
        {
            HobStart = ConfigTable[NoTableEntries].VendorTable;

            if ( !EFI_ERROR(
                        TcgGetNextGuidHob( &HobStart,
                                           &gEfiTcgWakeEventDataHobGuid,
                                           &BootMode, NULL )
                    ))
            {
                break;
            }
        }
    }

    if ( BootMode != NULL )
    {
        if ( *BootMode == BOOT_ON_S4_RESUME || *BootMode == BOOT_ON_S5_RESUME
                || *BootMode == BOOT_WITH_FULL_CONFIGURATION )
        {
            *pWake = (UINT8)SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
        }
    }
    return EFI_SUCCESS;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  read_PPI_request
//
// Description: Reads and returns TCG PPI requests Value
//
//
// Input:
//
// Output:      UINT8
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8 ReadPpiRequest()
{
    UINTN          Size = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR Temp;
    EFI_STATUS     Status;

    Status = gRT->GetVariable( L"AMITCGPPIVAR", \
                               &AmitcgefiOsVariableGuid, \
                               NULL, \
                               &Size, \
                               &Temp );

    if(Status == EFI_NOT_FOUND)
    {
        Temp.RQST    = 0;
        Temp.RCNT    = 0;
        Temp.ERROR   = 0;
        Temp.Flag    = 0;
        Temp.AmiMisc = 0;

#if NVRAM_VERSION > 6
        Status = TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
                  Size, &Temp);
#else
        Status = TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
                                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                                        Size, &Temp );
#endif

    }

    return Temp.RQST;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:  ReadPpiRequestEx
//
// Description: Reads and returns TCG PPI requests Value
//
//
// Input:
//
// Output:      UINT8
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
UINT8 ReadPpiRequestEx(UINT32 *Optionaldata )
{
    UINTN          Size = sizeof(AMI_PPI_NV_VAR);
    UINTN          Size2 = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR Temp;
    AMI_PPI_NV_VAR Temp2;
    EFI_STATUS     Status;
    UINT32         Attributes = 0;
    UINT32         Attributes2 = 0;

    Status = gRT->GetVariable( L"AMITCGPPIVAR", \
                               &AmitcgefiOsVariableGuid, \
                               &Attributes, \
                               &Size, \
                               &Temp );

    if(Status == EFI_NOT_FOUND)
    {
        Temp.RQST    = 0;
        Temp.RCNT    = 0;
        Temp.ERROR   = 0;
        Temp.Flag    = 0;
        Temp.AmiMisc = 0;
        Temp.OptionalData = 0;

#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)

        Status = TcgSetVariableWithNewAttributes( L"AMITCGPPIVAR", \
                                   &AmitcgefiOsVariableGuid, \
                                   EFI_VARIABLE_NON_VOLATILE | \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                   EFI_VARIABLE_RUNTIME_ACCESS, \
                                   Size, \
                                   &Temp );
#else
        Status = TcgSetVariableWithNewAttributes( L"AMITCGPPIVAR", \
                                   &AmitcgefiOsVariableGuid, \
                                   EFI_VARIABLE_NON_VOLATILE | \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS, \
                                   Size, \
                                   &Temp );
#endif

        return Temp.RQST;
    }

    if(Temp.RQST == 0)
    {
        //check for protocol interface request
        Status = gRT->GetVariable( L"AMITCGPPIVAR2", \
                                   &AmitcgefiOsVariableGuid, \
                                   &Attributes2, \
                                   &Size2, \
                                   &Temp2);

        if(!EFI_ERROR(Status) && Temp2.RQST == TCPA_PPIOP_SET_PCR_BANKS)
        {
            Temp.RQST = Temp2.RQST;
            Temp.RCNT = Temp2.RCNT;
            Temp.OptionalData = Temp2.OptionalData;
			//SetVariable "AMITCGPPIVAR" will fail since Attributes is different from GetVariable's
            Status = TcgSetVariableWithNewAttributes( L"AMITCGPPIVAR", \
                                       &AmitcgefiOsVariableGuid, \
                                       Attributes, \
                                       Size, \
                                       &Temp );
            
            Temp2.RQST = 0; //TPM20_PP_NO_ACTION
            Status = TcgSetVariableWithNewAttributes( L"AMITCGPPIVAR2", \
                                       &AmitcgefiOsVariableGuid, \
                                       Attributes2, \
                                       Size2, \
                                       &Temp2 );
        }
    }

    *Optionaldata = Temp.OptionalData;
    return Temp.RQST;
}

//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: Update_PpiVar
//
// Description: Updates TCG PPI variable in NVRAM
//
//
// Input:       IN  UINT8 value
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
void Update_PpiVar(
    IN UINT8 value )
{
    UINTN          Size = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR Temp;
    EFI_STATUS     Status;

    //now set variable to data
    Status = gRT->GetVariable( L"AMITCGPPIVAR", \
                               &AmitcgefiOsVariableGuid, \
                               NULL, \
                               &Size, \
                               &Temp );

    if ( EFI_ERROR( Status ))
    {
        return;
    }

    //get current value and set new value
    Temp.RQST = value;

#if NVRAM_VERSION > 6
    Status =  TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
              Size, &Temp);

#else
    TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
                                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                                    Size, &Temp );
#endif
}

//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: write_PPI_result
//
// Description: Updates TCG PPI variable in NVRAM
//
//
// Input:       IN  UINT8 last_op,
//              IN  UINT16 status
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
void WritePpiResult(
    IN UINT8  last_op,
    IN UINT16 status )
{
    UINTN          Size = sizeof(AMI_PPI_NV_VAR);
    AMI_PPI_NV_VAR Temp;
    EFI_STATUS     Status;
    UINT8          Manip = 0;

    Status = gRT->GetVariable( L"AMITCGPPIVAR", \
                               &AmitcgefiOsVariableGuid, \
                               NULL, \
                               &Size, \
                               &Temp );
    if ( EFI_ERROR( Status ))
    {
        return;
    }

    //now set variable to data
    Temp.RQST  = Manip;
    Manip      = (UINT8)( status & 0xFFFF );
    Temp.ERROR = Manip;

#if NVRAM_VERSION > 6
    TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
              Size, &Temp );
#else
    TcgSetVariableWithNewAttributes(L"AMITCGPPIVAR", &AmitcgefiOsVariableGuid, \
                                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,\
                                    Size, &Temp );
#endif
}

//****************************************************************************************
//<AMI_PHDR_START>
//
// Procedure: getSetupData
//
// Description: Retrieved SETUP_DATA structure from NVRAM
//
//
// Input:       IN  OUT   SETUP_DATA** ppsd,
//              IN  UINT32* pattr,
//              IN  UINTN* psz
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//****************************************************************************************
EFI_STATUS getSetupData(
    IN OUT SETUP_DATA** ppsd,
    IN UINT32        * pattr,
    IN UINTN         * psz )
{
    EFI_STATUS Status;
    UINTN      sz = 0;

    *ppsd  = NULL;
    Status = gRT->GetVariable( L"Setup", &gSetupVariableGuid, pattr, &sz, *ppsd );

    if ( !EFI_ERROR( Status ))
    {
        return Status;
    }

    if ( Status == EFI_BUFFER_TOO_SMALL )
    {
        Status = gBS->AllocatePool( EfiBootServicesData, sz, ppsd );

        if ( !(*ppsd))
        {
            return EFI_OUT_OF_RESOURCES;
        }
        Status = gRT->GetVariable( L"Setup", &gSetupVariableGuid, pattr, &sz, *ppsd );
    }

    if ( psz != NULL )
    {
        *psz = sz;
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
