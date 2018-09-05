//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TcgPlatformSetupPolicy/TcgPlatformSetupPolicy.c 6     12/15/11 3:30p Fredericko $
//
// $Revision: 6 $
//
// $Date: 12/15/11 3:30p $
//**********************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
// Name: TcgPlatformSetupPolicy.c
//
// Description: Policy file to allow reading and update of TCG policy
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Uefi.h>
#include <Token.h>
#include <Library/IoLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/TcgPlatformSetupPolicy.h>
#include <AmiTcg/TCGMisc.h>
#include <AmiTcg/TcgCommon20.h>
#include <Protocol/LegacyBiosExt.h>
#include <Guid/AmiTcgGuidIncludes.h>

TCG_PLATFORM_SETUP_PROTOCOL *TcgPlatformSetupInstance  = NULL;

EFI_HANDLE gImageHandle;
static TCG_CONFIGURATION  InitialConfigFlags;


EFI_STATUS
UpdateTcgStatusFlags (TCG_CONFIGURATION *StatusFlags, BOOLEAN UpdateNvram)

{
    EFI_STATUS              Status=EFI_SUCCESS;
    SETUP_DATA              SetupDataBuffer;
    UINTN                   SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                  SetupVariableAttributes;


    if(InitialConfigFlags.DisallowTpm == 1)return EFI_INVALID_PARAMETER;

    if(StatusFlags == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    TcgPlatformSetupInstance->ConfigFlags.TpmSupport               = InitialConfigFlags.TpmSupport;
    TcgPlatformSetupInstance->ConfigFlags.TpmEnable                = StatusFlags->TpmEnable ;
    TcgPlatformSetupInstance->ConfigFlags.TpmAuthenticate          = InitialConfigFlags.TpmAuthenticate;
    TcgPlatformSetupInstance->ConfigFlags.TpmOperation             = StatusFlags->TpmOperation;
    TcgPlatformSetupInstance->ConfigFlags.DisallowTpm              = 0;
    TcgPlatformSetupInstance->ConfigFlags.Tcg2SpecVersion                = InitialConfigFlags.Tcg2SpecVersion;
    TcgPlatformSetupInstance->ConfigFlags.DeviceType               = InitialConfigFlags.DeviceType;
    TcgPlatformSetupInstance->ConfigFlags.TpmHardware              = StatusFlags->TpmHardware;
    TcgPlatformSetupInstance->ConfigFlags.TpmEnaDisable            = StatusFlags->TpmEnaDisable;
    TcgPlatformSetupInstance->ConfigFlags.TpmActDeact              = StatusFlags->TpmActDeact;
    TcgPlatformSetupInstance->ConfigFlags.TpmOwnedUnowned          = StatusFlags->TpmOwnedUnowned;
    TcgPlatformSetupInstance->ConfigFlags.TcgSupportEnabled        = StatusFlags->TcgSupportEnabled ;
    TcgPlatformSetupInstance->ConfigFlags.TcmSupportEnabled        = StatusFlags->TcmSupportEnabled ;
    TcgPlatformSetupInstance->ConfigFlags.TpmError                 = StatusFlags->TpmError;
    TcgPlatformSetupInstance->ConfigFlags.PpiSetupSyncFlag         = StatusFlags->PpiSetupSyncFlag;
    TcgPlatformSetupInstance->ConfigFlags.Reserved3                = StatusFlags->Reserved3;

    TcgPlatformSetupInstance->ConfigFlags.Reserved4              = StatusFlags->Reserved4;
    TcgPlatformSetupInstance->ConfigFlags.Reserved5              = StatusFlags->Reserved5;
    TcgPlatformSetupInstance->ConfigFlags.Tpm20Device            = StatusFlags->Tpm20Device;

    TcgPlatformSetupInstance->ConfigFlags.EndorsementHierarchy   = StatusFlags->EndorsementHierarchy;
    TcgPlatformSetupInstance->ConfigFlags.StorageHierarchy       = StatusFlags->StorageHierarchy;
    TcgPlatformSetupInstance->ConfigFlags.PlatformHierarchy      = StatusFlags->PlatformHierarchy;
    TcgPlatformSetupInstance->ConfigFlags.InterfaceSel           = StatusFlags->InterfaceSel;
    TcgPlatformSetupInstance->ConfigFlags.PcrBanks               = StatusFlags->PcrBanks;

    if(UpdateNvram)
    {

        Status = gRT->GetVariable (
                     L"Setup",
                     &gSetupVariableGuid,
                     &SetupVariableAttributes,
                     &SetupVariableSize,
                     &SetupDataBuffer);

        SetupDataBuffer.TpmEnable            =   TcgPlatformSetupInstance->ConfigFlags.TpmEnable;
        SetupDataBuffer.TpmSupport           =   TcgPlatformSetupInstance->ConfigFlags.TpmSupport;
        SetupDataBuffer.TpmAuthenticate      =   TcgPlatformSetupInstance->ConfigFlags.TpmAuthenticate;
        SetupDataBuffer.TpmOperation         =   TcgPlatformSetupInstance->ConfigFlags.TpmOperation;
        SetupDataBuffer.TpmEnaDisable        =   TcgPlatformSetupInstance->ConfigFlags.TpmEnaDisable;
        SetupDataBuffer.TpmActDeact          =   TcgPlatformSetupInstance->ConfigFlags.TpmActDeact;
        SetupDataBuffer.TpmHrdW              =   TcgPlatformSetupInstance->ConfigFlags.TpmHardware;
        SetupDataBuffer.TpmOwnedUnowned      =   TcgPlatformSetupInstance->ConfigFlags.TpmOwnedUnowned;
        SetupDataBuffer.TpmError             =   TcgPlatformSetupInstance->ConfigFlags.TpmError;
        SetupDataBuffer.TcgSupportEnabled    =   TcgPlatformSetupInstance->ConfigFlags.TcgSupportEnabled;
        SetupDataBuffer.TcmSupportEnabled    =   TcgPlatformSetupInstance->ConfigFlags.TcmSupportEnabled;
        SetupDataBuffer.Tpm20Device          =   TcgPlatformSetupInstance->ConfigFlags.Tpm20Device;
        SetupDataBuffer.Tcg2SpecVersion            =   TcgPlatformSetupInstance->ConfigFlags.Tcg2SpecVersion;
        SetupDataBuffer.DeviceType           =   TcgPlatformSetupInstance->ConfigFlags.DeviceType;
        SetupDataBuffer.EndorsementHierarchy   = TcgPlatformSetupInstance->ConfigFlags.EndorsementHierarchy;
        SetupDataBuffer.StorageHierarchy       = TcgPlatformSetupInstance->ConfigFlags.StorageHierarchy;
        SetupDataBuffer.PlatformHierarchy      = TcgPlatformSetupInstance->ConfigFlags.PlatformHierarchy;
        SetupDataBuffer.InterfaceSel           = TcgPlatformSetupInstance->ConfigFlags.InterfaceSel;
#if (defined(EXPOSE_FORCE_TPM_ENABLE) && (EXPOSE_FORCE_TPM_ENABLE == 1))
        SetupDataBuffer.ForceTpmEnable         = TcgPlatformSetupInstance->ConfigFlags.Reserved4;
#endif

        Status = gRT->SetVariable (
                     L"Setup",
                     &gSetupVariableGuid,
                     SetupVariableAttributes,
                     SetupVariableSize,
                     &SetupDataBuffer);

        SetupVariableAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE;

        Status = gRT->SetVariable (
                     L"TcgInternalSyncFlag",
                     &gTcgPpiSyncFlagGuid,
                     SetupVariableAttributes,
                     sizeof(UINT8),
                     &(TcgPlatformSetupInstance->ConfigFlags.PpiSetupSyncFlag));

        if(Status == EFI_INVALID_PARAMETER)
        {
            Status = gRT->SetVariable( L"TcgInternalSyncFlag", \
                                       &gTcgPpiSyncFlagGuid, \
                                       0, \
                                       0, \
                                       NULL);

            if(EFI_ERROR(Status))return Status;

            Status = gRT->SetVariable( L"TcgInternalSyncFlag", \
                                       &gTcgPpiSyncFlagGuid, \
                                       SetupVariableAttributes, \
                                       sizeof(UINT8), \
                                       &(TcgPlatformSetupInstance->ConfigFlags.PpiSetupSyncFlag));
        }
    }

    return Status;
}



VOID
EFIAPI
TCGSyncUpdateData (
    IN      EFI_EVENT                 Event,
    IN      VOID                      *Context
)
{
    EFI_STATUS                      Status;
    TCG_PLATFORM_SETUP_PROTOCOL     *PolicyInstance;

    TCG_CONFIGURATION               Config;

    Status = gBS->LocateProtocol (&gTcgPlatformSetupPolicyGuid,  NULL, &PolicyInstance);
    if (EFI_ERROR (Status))
    {
        return;
    }

    gBS-> CopyMem(&Config, &PolicyInstance->ConfigFlags, sizeof(TCG_CONFIGURATION));
    PolicyInstance->UpdateStatusFlags(&Config, TRUE);

    gBS->CloseEvent(Event);
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgPlatformSetupPolicyEntryPoint
//
// Description:  Entry point for TcgPlatformSetupPolicyEntryPoint
//
// Input:       ImageHandle       Image handle of this driver.
//              SystemTable       Global system service table.
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
TcgPlatformSetupPolicyEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                    Status;
    UINTN                         VariableSize  = 0;
    SETUP_DATA                    SetupDataBuffer;
    SETUP_DATA                   *SetupData = &SetupDataBuffer;
    UINTN                         SetupVariableSize;
    UINT32                        SetupVariableAttributes;

    UINT8                         SyncVar;
    UINT8                         DisallowTpmFlag=0;
    UINTN                         TempSizeofSyncVar = sizeof(UINT8);
    EFI_EVENT                     ev;
    static VOID                   *reg;

    SetupVariableSize = sizeof (SETUP_DATA);

    Status = gBS->AllocatePool (
                 EfiBootServicesData,
                 sizeof (TCG_PLATFORM_SETUP_PROTOCOL),
                 (VOID**)&TcgPlatformSetupInstance
             );


    if (EFI_ERROR(Status))
    {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = gRT->GetVariable (
                 L"InternalDisallowTpmFlag",
                 &gTcgInternalflagGuid,
                 &SetupVariableAttributes,
                 &TempSizeofSyncVar,
                 &DisallowTpmFlag);


    if(EFI_ERROR(Status))
    {
        DisallowTpmFlag = 0;
        Status = EFI_SUCCESS;
    }
    else if(DisallowTpmFlag == 1)
    {
        SetMem(&TcgPlatformSetupInstance->ConfigFlags, sizeof(TCG_CONFIGURATION), 0);
        TcgPlatformSetupInstance->ConfigFlags.DisallowTpm              = 1;
        SetMem(&InitialConfigFlags, sizeof(TCG_CONFIGURATION), 0);

        InitialConfigFlags.DisallowTpm = 1;

        TcgPlatformSetupInstance->Revision = TCG_PLATFORM_SETUP_PROTOCOL_REVISION_1;
        TcgPlatformSetupInstance->UpdateStatusFlags = UpdateTcgStatusFlags;

        Status = gRT->GetVariable (
                     L"Setup",
                     &gSetupVariableGuid,
                     &SetupVariableAttributes,
                     &SetupVariableSize,
                     &SetupDataBuffer);


        SetupDataBuffer.TpmEnable            =   0;
        SetupDataBuffer.TpmSupport           =   0;
        SetupDataBuffer.TpmAuthenticate      =   0;
        SetupDataBuffer.TpmOperation         =   0;
        SetupDataBuffer.TpmEnaDisable        =   0;
        SetupDataBuffer.TpmActDeact          =   0;
        SetupDataBuffer.TpmHrdW              =   0;
        SetupDataBuffer.TpmOwnedUnowned      =   0;
        SetupDataBuffer.TpmError             =   0;
        SetupDataBuffer.SuppressTcg          =   DisallowTpmFlag;
        SetupDataBuffer.TcgSupportEnabled    =   0;
        SetupDataBuffer.TcmSupportEnabled    =   0;

        Status = gRT->SetVariable (
                     L"Setup",
                     &gSetupVariableGuid,
                     SetupVariableAttributes,
                     SetupVariableSize,
                     &SetupDataBuffer);

        return Status;
    }


    SetupVariableAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE;

    Status = gRT->GetVariable (
                 L"TcgInternalSyncFlag",
                 &gTcgPpiSyncFlagGuid,
                 &SetupVariableAttributes,
                 &TempSizeofSyncVar,
                 &SyncVar);

    if(EFI_ERROR(Status))
    {
        SyncVar = 0;
        Status = EFI_SUCCESS;
    }


    Status = gRT->GetVariable (
                 L"Setup",
                 &gSetupVariableGuid,
                 &SetupVariableAttributes,
                 &SetupVariableSize,
                 &SetupDataBuffer);

    TcgPlatformSetupInstance->Revision = TCG_PLATFORM_SETUP_PROTOCOL_REVISION_1;

    if (EFI_ERROR(Status))
    {
        SetMem(&TcgPlatformSetupInstance->ConfigFlags, sizeof(TCG_CONFIGURATION), 0);
        SetupData = NULL;
    }
    else
    {

        SetMem(&TcgPlatformSetupInstance->ConfigFlags, sizeof(TCG_CONFIGURATION), 0);
        TcgPlatformSetupInstance->ConfigFlags.TpmSupport       = (SetupData->TpmSupport);
        TcgPlatformSetupInstance->ConfigFlags.TpmEnable        = (SetupData->TpmEnable);
        TcgPlatformSetupInstance->ConfigFlags.TpmAuthenticate  = (SetupData->TpmAuthenticate);
        TcgPlatformSetupInstance->ConfigFlags.TpmOperation             = (SetupData->TpmOperation);
        TcgPlatformSetupInstance->ConfigFlags.PpiSetupSyncFlag = SyncVar;
        TcgPlatformSetupInstance->ConfigFlags.Tcg2SpecVersion        = (SetupData->Tcg2SpecVersion);
        TcgPlatformSetupInstance->ConfigFlags.DeviceType           =   SetupData->DeviceType;
        TcgPlatformSetupInstance->ConfigFlags.DisallowTpm      = DisallowTpmFlag;
        TcgPlatformSetupInstance->ConfigFlags.TpmHardware     = 1;
        TcgPlatformSetupInstance->ConfigFlags.TpmEnaDisable   = (SetupData->TpmEnaDisable);
        TcgPlatformSetupInstance->ConfigFlags.TpmActDeact     = (SetupData->TpmActDeact);
        TcgPlatformSetupInstance->ConfigFlags.TpmOwnedUnowned     = (SetupData->TpmOwnedUnowned);
        TcgPlatformSetupInstance->ConfigFlags.TcgSupportEnabled   =0;

        TcgPlatformSetupInstance->ConfigFlags.TcmSupportEnabled   = 0;
        TcgPlatformSetupInstance->ConfigFlags.TpmError            = (SetupData->TpmError);
#if (defined(TPM2_DISABLE_PLATFORM_HIERARCHY_RANDOMIZATION) && (TPM2_DISABLE_PLATFORM_HIERARCHY_RANDOMIZATION == 1))
        TcgPlatformSetupInstance->ConfigFlags.Reserved3           = (SetupData->PhRandomization);
#else
        TcgPlatformSetupInstance->ConfigFlags.Reserved3           = 0;
#endif
#if (defined(EXPOSE_FORCE_TPM_ENABLE) && (EXPOSE_FORCE_TPM_ENABLE == 1))
        TcgPlatformSetupInstance->ConfigFlags.Reserved4           = (SetupData->ForceTpmEnable);
#else
        TcgPlatformSetupInstance->ConfigFlags.Reserved4           = 0;
#endif
        TcgPlatformSetupInstance->ConfigFlags.Reserved5 = 0;
        TcgPlatformSetupInstance->ConfigFlags.Tpm20Device = 0;

        TcgPlatformSetupInstance->ConfigFlags.EndorsementHierarchy   = SetupData->EndorsementHierarchy;
        TcgPlatformSetupInstance->ConfigFlags.StorageHierarchy       = SetupData->StorageHierarchy;
        TcgPlatformSetupInstance->ConfigFlags.PlatformHierarchy      = SetupData->PlatformHierarchy;
        TcgPlatformSetupInstance->ConfigFlags.InterfaceSel           = SetupData->InterfaceSel;
        TcgPlatformSetupInstance->ConfigFlags.PcrBanks               = (SetupData->Sha1 & 0x01) | (SetupData->Sha256 & 0x02) |\
                                                                        (SetupData->Sha384 & 0x04) | ( SetupData->Sha512 & 0x08 );
        if(SetupData->SM3 == 1)
        {
            TcgPlatformSetupInstance->ConfigFlags.PcrBanks |= 0x10;
        }
    }

    TcgPlatformSetupInstance->UpdateStatusFlags = UpdateTcgStatusFlags;

    InitialConfigFlags.TpmSupport               =     TcgPlatformSetupInstance->ConfigFlags.TpmSupport;
    InitialConfigFlags.TpmEnable                =     TcgPlatformSetupInstance->ConfigFlags.TpmEnable;
    InitialConfigFlags.TpmAuthenticate          =     TcgPlatformSetupInstance->ConfigFlags.TpmAuthenticate;
    InitialConfigFlags.TpmOperation             =     TcgPlatformSetupInstance->ConfigFlags.TpmOperation;
    InitialConfigFlags.Tcg2SpecVersion               =     TcgPlatformSetupInstance->ConfigFlags.Tcg2SpecVersion ;
    InitialConfigFlags.DeviceType               =     TcgPlatformSetupInstance->ConfigFlags.DeviceType;

    InitialConfigFlags.TpmHardware              =     TcgPlatformSetupInstance->ConfigFlags.TpmHardware;
    InitialConfigFlags.TpmEnaDisable            =    TcgPlatformSetupInstance->ConfigFlags.TpmEnaDisable;
    InitialConfigFlags.TpmActDeact              =    TcgPlatformSetupInstance->ConfigFlags.TpmActDeact;
    InitialConfigFlags.TpmOwnedUnowned          =    TcgPlatformSetupInstance->ConfigFlags.TpmOwnedUnowned;
    InitialConfigFlags.TcgSupportEnabled        =    TcgPlatformSetupInstance->ConfigFlags.TcgSupportEnabled;
    InitialConfigFlags.TcmSupportEnabled        =    TcgPlatformSetupInstance->ConfigFlags.TcmSupportEnabled;
    InitialConfigFlags.TpmError                 =    TcgPlatformSetupInstance->ConfigFlags.TpmError;
    InitialConfigFlags.PpiSetupSyncFlag         =    TcgPlatformSetupInstance->ConfigFlags.PpiSetupSyncFlag;
    InitialConfigFlags.Reserved3                =    TcgPlatformSetupInstance->ConfigFlags.Reserved3;

    InitialConfigFlags.Reserved4                 =   TcgPlatformSetupInstance->ConfigFlags.Reserved4;
    InitialConfigFlags.Reserved5                 =   TcgPlatformSetupInstance->ConfigFlags.Reserved5;

    InitialConfigFlags.Tpm20Device               = TcgPlatformSetupInstance->ConfigFlags.Tpm20Device;

    InitialConfigFlags.EndorsementHierarchy      = TcgPlatformSetupInstance->ConfigFlags.EndorsementHierarchy;
    InitialConfigFlags.StorageHierarchy          = TcgPlatformSetupInstance->ConfigFlags.StorageHierarchy;
    InitialConfigFlags.PlatformHierarchy         = TcgPlatformSetupInstance->ConfigFlags.PlatformHierarchy;
    InitialConfigFlags.InterfaceSel              = TcgPlatformSetupInstance->ConfigFlags.InterfaceSel;
    InitialConfigFlags.PcrBanks                  = TcgPlatformSetupInstance->ConfigFlags.PcrBanks;
    //
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                 &ImageHandle,
                 &gTcgPlatformSetupPolicyGuid,
                 TcgPlatformSetupInstance,
                 NULL
             );

    gImageHandle  = ImageHandle;

    Status = gBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                               EFI_TPL_CALLBACK,
                               &TCGSyncUpdateData,
                               0,
                               &ev );

    if(!EFI_ERROR(Status))
    {
        Status = gBS->RegisterProtocolNotify(
                     &gBdsAllDriversConnectedProtocolGuid,
                     ev,

                     &reg );
    }

    return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
