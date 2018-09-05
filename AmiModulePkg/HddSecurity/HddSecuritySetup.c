//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file HddSecuritySetup.c
    HDD Security Setup Routines
    Use this hook to dynamically change HDD Security Driver Setup screen Strings.

**/

//---------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <HddSecurityCommon.h>
#include <Protocol/HiiString.h>
#if !defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0
#include <HddSecuritySetupVar.h>
// {2FF61C92-DB91-431d-94F9-8992C3615F48}
EFI_GUID  gHddSecurityLoadSetupDefaultsGuid = AMI_HDD_SECURITY_LOAD_SETUP_DEFAULTS_GUID;
EFI_EVENT gHddSecurityLoadSetupDefaultsEvent = NULL;
#endif

CHAR16*
Wcscpy(
        CHAR16 *,
        CHAR16 *
);

int
Wcscmp(
        CHAR16 *string1,
        CHAR16 *string2
);

VOID
RefreshHddSecuritySetupData(
        VOID
);

VOID 
EFIAPI
AmiTseHddNotifyInitString(
        IN EFI_EVENT Event, 
        IN VOID *Context
);

HDD_SECURITY_INTERNAL_DATA     *HddSecurityInternalData = NULL;
UINT16                         gHddSecurityCount = 0;
UINT16                         gValidHddSecurityCount;
#if !defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0
HDD_SECURITY_CONFIG            *gHddSecurityConfig = NULL;
#endif
static EFI_HII_STRING_PROTOCOL *HiiString      = NULL;

static EFI_GUID   EfiVariableGuid      = EFI_GLOBAL_VARIABLE;
EFI_GUID   gAmiHddSecurityProtocolGuid = AMI_HDD_SECURITY_PROTOCOL_GUID;
#if !defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0
EFI_GUID   gHddSecurityConfigGuid      = HDD_SECURITY_CONFIG_GUID;
#endif

EFI_HII_HANDLE HddSecurityHiiHandle = 0; 

// Store the names of HDDs present in the system. Dynamically updated.
STRING_REF gHddSecurityName[6] =
{
    STRING_TOKEN(STR_SECURITY_HDD_ONE), STRING_TOKEN(STR_SECURITY_HDD_TWO),
    STRING_TOKEN(STR_SECURITY_HDD_THREE), STRING_TOKEN(STR_SECURITY_HDD_FOUR),
    STRING_TOKEN(STR_SECURITY_HDD_FIVE), STRING_TOKEN(STR_SECURITY_HDD_SIX)
};

// Function Definitions

#if !defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0

#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
/**
    function to update the given password to non case sensitive

    @param CHAR16 *Password
    @param UINTN PwdLength

    @retval VOID

**/
static
VOID
UpdatePasswordToNonCaseSensitive(
        CHAR16 *Password,
        UINTN PwdLength
)
{
    UINTN Idx;
    for ( Idx = 0; Idx < PwdLength; Idx++ ) {
        Password[Idx] = ((Password[Idx]>=L'a')&&(Password[Idx]<=L'z'))?(Password[Idx]+L'A'-L'a'):Password[Idx];
    }
}
#endif

/**
    Notification function for "Load Default" action in BIOS setup. This function
    will re-initialize the HDD security setup data.

    @param EFI_EVENT Event - Event to signal
    @param VOID* Context - Event specific context

    @retval VOID

**/
VOID
EFIAPI
HddSecurityLoadSetupDefaults (
        IN EFI_EVENT Event,
        IN VOID *Context
)
{
    RefreshHddSecuritySetupData();
}

/**
    return the Security Status Information

    @param HddSecurityProtocol
    @param ReqStatus
    @param Mask

    @retval BOOLEAN      status in ReqStatus is valid

**/
BOOLEAN
CheckSecurityStatus (
    IN  AMI_HDD_SECURITY_PROTOCOL   *HddSecurityProtocol,
    IN  OUT BOOLEAN                 *ReqStatus,
    IN  UINT16                      Mask
)
{
    UINT16      SecurityStatus = 0;
    EFI_STATUS  Status;

    //
    //get the security status of the device
    //
    Status = HddSecurityProtocol->ReturnSecurityStatus( HddSecurityProtocol, &SecurityStatus );

    if ( EFI_ERROR( Status ))
        return FALSE; // Unable to get security status
    // Mask security status based on supplied mask data
    *ReqStatus = (BOOLEAN)((SecurityStatus & Mask) ? TRUE : FALSE );
    return TRUE;
}
#endif

#if (!defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0) || (defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE == 1)
/**
    return the String based on StringId

    @param HiiHandle
    @param StringId

    @retval EFI_STRING

**/
EFI_STRING
HddHiiGetString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  EFI_STRING_ID   StringId
)
{
    EFI_STRING String = NULL;
    UINTN StringSize = 0;
    EFI_STATUS Status;
    CHAR8* PlatformLang = NULL;
    UINTN Size = 0;

    if(HiiString == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
        if(EFI_ERROR(Status))
            return NULL;
    }

    //language not provided - try platform language
    Status = GetEfiVariable(L"PlatformLang", &EfiVariableGuid, NULL, &Size, &PlatformLang);
    if(!EFI_ERROR(Status)) {
        Status = HiiString->GetString(HiiString, PlatformLang, HiiHandle, StringId, String, &StringSize, NULL);
        if(Status == EFI_BUFFER_TOO_SMALL) {
            Status = pBS->AllocatePool(EfiBootServicesData, StringSize, (VOID**)&String);
            if(!EFI_ERROR(Status))
                Status = HiiString->GetString(HiiString, PlatformLang, HiiHandle, StringId, String, &StringSize, NULL);
        }
        pBS->FreePool(PlatformLang);
    }
    return (EFI_ERROR(Status)) ? NULL : String;
}

/**
    Locate the Security Protocols and return the information

    @param VOID

    @retval VOID

**/
UINT16
InitHddSecurityInternalDataPtr(
    VOID
)
{
    EFI_STATUS                      Status;
    AMI_HDD_SECURITY_INTERFACE      *Security = NULL;
    UINTN                           i;
    VOID                            *TempPtr;

    Status = pBS->LocateProtocol(
                    &gAmiHddSecurityProtocolGuid,
                    NULL,
                    (VOID**) &Security);

    if ( !EFI_ERROR( Status )) {
        gHddSecurityCount = *((UINT16*)Security->PostHddCount);
        HddSecurityInternalData = *((HDD_SECURITY_INTERNAL_DATA**)Security->PostHddData);

        gValidHddSecurityCount = gHddSecurityCount;

        for(i=0;i<gHddSecurityCount;i++) {

            if( (HddSecurityInternalData+i) != NULL ) {
                Status = pBS->HandleProtocol(
                                HddSecurityInternalData[i].DeviceHandle,
                                &gAmiHddSecurityProtocolGuid,
                                &TempPtr
                                );

                if( EFI_ERROR(Status) ) {
                    TempPtr = 0;
                    gValidHddSecurityCount--;
                }
            }
        }
		
        return gValidHddSecurityCount;
    }

    return 0;
}

/**
    Get the HDD security internal Data pointer

    @param Index - Index of HDD

    @retval VOID

**/
VOID*
GetHddSecurityInternalDataPtr (
    UINTN   Index
)
{
    HDD_SECURITY_INTERNAL_DATA *DataPtr;

    if ( gValidHddSecurityCount==0 || gHddSecurityCount == 0 || HddSecurityInternalData == NULL ) {
        //
        //try to initialize, if not initialized
        //
        gValidHddSecurityCount = InitHddSecurityInternalDataPtr( );
    }
    if ( gValidHddSecurityCount == 0 || HddSecurityInternalData == NULL || Index >=
         gHddSecurityCount ) {
        return NULL;
    }
    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)HddSecurityInternalData;

    return (VOID*)&DataPtr[Index];
}
#endif

/**
    This function initializes the HddSecurity setup option values or registers
    an event that will later initialize security strings.

    @param  HiiHandle   Handle to HII database
    @param  Class       Indicates the setup class

    @retval VOID

**/

VOID
InitHddSecurityStrings (
    EFI_HII_HANDLE  HiiHandle,
    UINT16          Class
)
{
    EFI_STATUS    Status;
    UINTN         BufferSize = sizeof(UINTN);
    VOID          *NotifyRegistration;
    EFI_EVENT     NotifyEvent;
    EFI_HANDLE    TseHddNotify;
    EFI_GUID      AmiTseHddNotifyGuid = AMI_TSE_HDD_NOTIFY_GUID;
    
    HddSecurityHiiHandle = HiiHandle; // Store HII handle needed for SETUP items
    
    Status = pBS->LocateHandle ( ByProtocol,
                                 &AmiTseHddNotifyGuid,
                                 NULL,
                                 &BufferSize,
                                 &TseHddNotify );
    
    if (Status == EFI_SUCCESS) {
    //If HDD post data already initialized then Init Setup Strings here.
        AmiTseHddNotifyInitString (NULL, NULL);        
    } else if (Status == EFI_NOT_FOUND) {
        //Else register notify for AmiTseHddNotifyGuid
        Status = pBS->CreateEvent(  EFI_EVENT_NOTIFY_SIGNAL,
                                    TPL_CALLBACK,
                                    AmiTseHddNotifyInitString,
                                    &NotifyRegistration,
                                    &NotifyEvent );
        if(EFI_ERROR(Status)) {
           return;
        }
        
        pBS->RegisterProtocolNotify( &AmiTseHddNotifyGuid,
                                              NotifyEvent,
                                              &NotifyRegistration );
    }
}

/**
    This Event notifies String initialization for HddSecurity for  can be processed
    
    @param  Event                 Event whose notification function is being invoked.
    @param  Context               The pointer to the notification function's context,
                                  which is implementation-dependent.

    @retval VOID

**/
VOID 
EFIAPI
AmiTseHddNotifyInitString(
    IN EFI_EVENT Event, 
    IN VOID *Context
) 
{
#if !defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0
    EFI_STATUS                 Status;
    UINTN                      i;
    CHAR16                     *Name;
    HDD_SECURITY_INTERNAL_DATA *DataPtr;
    UINT16                     HddSecurityCount;
    
    // Initialize HDD security internal data structure and gHddSecurityCount
    GetHddSecurityInternalDataPtr(0);
    // Allocate memory for setup data
    if(NULL == gHddSecurityConfig) {
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    sizeof(HDD_SECURITY_CONFIG),
                                    (VOID**)&gHddSecurityConfig);
        if(EFI_ERROR(Status) || NULL == gHddSecurityConfig) {
            return;
        }
    }

    // Update HDD counts in setup data so that setup data for only connected HDDs will be visible
    gHddSecurityConfig->Count = gValidHddSecurityCount;
    HddSecurityCount = gValidHddSecurityCount;

    // Set the new setup configuration data
    Status = pRS->SetVariable(L"HDDSecConfig",
                                &gHddSecurityConfigGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                sizeof(HDD_SECURITY_CONFIG),
                                gHddSecurityConfig);

    // Update HDD Names
    for(i = 0; (i<gHddSecurityCount) && HddSecurityCount ; i++) {
        DataPtr = GetHddSecurityInternalDataPtr(i);
        if (DataPtr == NULL) continue;
        Name = HddHiiGetString( DataPtr->PostHiiHandle, DataPtr->PromptToken);
        InitString(HddSecurityHiiHandle, gHddSecurityName[i], L"%s", Name);
        HddSecurityCount--;
    }

    if(gHddSecurityLoadSetupDefaultsEvent == NULL) {
      Status = pBS->CreateEventEx(
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     HddSecurityLoadSetupDefaults,
                     NULL,
                     &gHddSecurityLoadSetupDefaultsGuid,
                     &gHddSecurityLoadSetupDefaultsEvent);
      ASSERT_EFI_ERROR(Status);
    }
#else
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    UINTN                      i;
    CHAR16                     *Name;
    HDD_SECURITY_INTERNAL_DATA *DataPtr;
    UINT16                     HddSecurityCount = gValidHddSecurityCount;

    // Initialize HDD security internal data structure and gHddSecurityCount
    GetHddSecurityInternalDataPtr(0);

    // Update HDD Names
    for(i = 0; (i<gHddSecurityCount) && HddSecurityCount; i++) {
        DataPtr = GetHddSecurityInternalDataPtr(i);
        if (DataPtr == NULL) continue;
        Name = HddHiiGetString( DataPtr->PostHiiHandle, DataPtr->PromptToken);
        InitString(HddSecurityHiiHandle, gHddSecurityName[i], L"%s", Name);
        HddSecurityCount--;
    }
#endif
#endif
    return;
}

#if !defined(SETUP_IDE_SECURITY_SUPPORT) || SETUP_IDE_SECURITY_SUPPORT == 0

/**
    Initializes the structure HDD_SECURITY_CONFIG for the current
    HDD if the data pointer to the structure HDD_SECURITY_INTERNAL_DATA is
    initialized already.

    @param HDD_SECURITY_CONFIG *HddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
UpdateSetupDataByHddIndex(
    VOID    *TempHddSecurityConfig,
    UINTN   value
)
{
    HDD_SECURITY_INTERNAL_DATA    *DataPtr             = NULL;
    AMI_HDD_SECURITY_PROTOCOL     *HddSecurityProtocol = NULL;
    BOOLEAN                       Status;
    UINT32                        IdePasswordFlags = 0;
    EFI_STATUS                    ReturnStatus;
    HDD_SECURITY_CONFIG           *HddSecurityConfig
        = (HDD_SECURITY_CONFIG*)TempHddSecurityConfig;


    //
    //Set current HDD security page
    //
    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)GetHddSecurityInternalDataPtr( value );
    
    if (DataPtr == NULL) return;

    if ( DataPtr ) {
        HddSecurityProtocol = DataPtr->HddSecurityProtocol;

        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecuritySupportedMask );
        HddSecurityConfig->Supported[value] = Status ? 1 : 0;
        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityEnabledMask );
        HddSecurityConfig->Enabled[value] = Status ? 1 : 0;
        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityLockedMask );
        HddSecurityConfig->Locked[value] = Status ? 1 : 0;
        CheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityFrozenMask );
        HddSecurityConfig->Frozen[value] = Status ? 1 : 0;
        ReturnStatus         = HddSecurityProtocol->ReturnIdePasswordFlags(
            HddSecurityProtocol,
            &IdePasswordFlags );

        if ( EFI_ERROR( ReturnStatus )) {
            return;
        }

        HddSecurityConfig->UserPasswordStatus[value]
            = (IdePasswordFlags & 0x00020000) ? 1 : 0;
        HddSecurityConfig->MasterPasswordStatus[value]
            = (IdePasswordFlags & 0x00010000) ? 1 : 0;

        HddSecurityConfig->ShowMaster[value] = 0x0000;

        if ( HddSecurityConfig->Locked[value] ) {
            HddSecurityConfig->ShowMaster[value] = 0x0001;
        } else if ( (DataPtr->LoggedInAsMaster)) {
            HddSecurityConfig->ShowMaster[value] = 0x0001;
        } else if ( !(HddSecurityConfig->UserPasswordStatus[value])) {
            HddSecurityConfig->ShowMaster[value] = 0x0001;
        }
        
    }// end if
    return;
}

/**
    Initializes the structure HDD_SECURITY_CONFIG for all the
    HDDs present if the data pointer to the structure
    HDD_SECURITY_INTERNAL_DATA is initialized already.

    @param HDD_SECURITY_CONFIG *HddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
UpdateSetupDataForAllHdd(
    VOID    *HddSecurityConfig,
    UINTN   value
)
{
    UINTN i;

    for ( i = 0; i < gHddSecurityCount; i++ ) {
        UpdateSetupDataByHddIndex( HddSecurityConfig, i);
    }
    return;
}

/**
    Hook function to Initialize the structure HDD_SECURITY_CONFIG
    for the HDDs based on the token ALL_HDD_SAME_PW.

    @param HDD_SECURITY_CONFIG *HddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
UpdateHddSecuritySetupData(
    VOID    *HddSecurityConfig,
    UINTN   value
)
{
    #if ALL_HDD_SAME_PW
     UpdateSetupDataForAllHdd( HddSecurityConfig, value);
    #else
     UpdateSetupDataByHddIndex( HddSecurityConfig, value);
    #endif

}

/**
    function to update the setup configuration page after HDD password update

    @param VOID

    @retval VOID

**/
VOID
RefreshHddSecuritySetupData (
        VOID
)
{
    EFI_STATUS          Status;
    UINT8               HardDiskNumber = 0xFF;

    // Allocate memory for setup configuration data
    if(NULL == gHddSecurityConfig) {
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    sizeof(HDD_SECURITY_CONFIG),
                                    (VOID**)&gHddSecurityConfig);
        if(EFI_ERROR(Status) || NULL == gHddSecurityConfig) {
            return;
        }
    }

    gHddSecurityConfig->Count =  InitHddSecurityInternalDataPtr( );

    // Update setup data for all HDDs
    for( HardDiskNumber = 0 ; HardDiskNumber < gHddSecurityCount ; HardDiskNumber++ )
    {
        UpdateHddSecuritySetupData( (VOID*)gHddSecurityConfig, HardDiskNumber );
    }

    // Set the new setup configuration data
    pRS->SetVariable(L"HDDSecConfig",
                    &gHddSecurityConfigGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(HDD_SECURITY_CONFIG),
                    gHddSecurityConfig);

    // Refresh browser data
    HiiLibSetBrowserData( sizeof(HDD_SECURITY_CONFIG),
                        gHddSecurityConfig,
                        &gHddSecurityConfigGuid,
                        L"HDDSecConfig");

}

/**
    Validates the password for the current HDD alone.

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
HddPasswordAuthenticateHdd (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    UINT16            Control = 0;
    EFI_STATUS        Status;
    UINT8             Buffer[IDE_PASSWORD_LENGTH + 1];

    #if !SETUP_SAME_SYS_HDD_PW
    UINTN             i;
    #endif

    HDD_SECURITY_INTERNAL_DATA * DataPtr = (HDD_SECURITY_INTERNAL_DATA*)Ptr;

#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
    UpdatePasswordToNonCaseSensitive(Password, Wcslen(Password));
#endif

    MemSet( &Buffer, IDE_PASSWORD_LENGTH + 1, 0 );

    #if !SETUP_SAME_SYS_HDD_PW

    for ( i = 0; i < IDE_PASSWORD_LENGTH + 1; i++ ) {
        Buffer[i] = (UINT8)Password[i];

        if ( Password[i] == L'\0' ) {
            break;
        }
    }
    #else
    pBS->CopyMem( Buffer, Password, IDE_PASSWORD_LENGTH + 1 );
    #endif

    Control = bCheckUser ? 0 : 1;

    Status = (DataPtr->HddSecurityProtocol)->SecurityUnlockPassword(
        DataPtr->HddSecurityProtocol,
        Control,
        Buffer );

    if ( EFI_ERROR( Status )) {
        return EFI_ACCESS_DENIED;
    }

    //
    //save password in case we need to disable it during the setup
    //
    pBS->CopyMem( &(DataPtr->PWD), &Buffer, IDE_PASSWORD_LENGTH + 1 );
//    DataPtr->Locked = FALSE;

    if ( !bCheckUser ) {
        DataPtr->LoggedInAsMaster = TRUE;
    }

    return EFI_SUCCESS;
}

/**
    Validates the password for all the HDDs Present.

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
HddPasswordAuthenticateAllHdd (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    HDD_SECURITY_INTERNAL_DATA *DataPtr;
    UINTN i;
    EFI_STATUS        Status=EFI_NOT_FOUND;

        //
        //For all drives
        //
        DataPtr = HddSecurityInternalData;

        if(DataPtr == NULL) {
            return EFI_NOT_FOUND;
        }

        for ( i = 0; i < gHddSecurityCount; i++ ) {

            Status = HddPasswordAuthenticateHdd( Password,
                                              DataPtr,
                                              bCheckUser );
            if ( EFI_SUCCESS != Status ) {
                //
                // Unlock failed.
                //
                LibReportStatusCode( EFI_ERROR_CODE | EFI_ERROR_MAJOR,
                                        DXE_INVALID_IDE_PASSWORD,
                                        0,
                                        NULL,
                                        NULL );
            }
            DataPtr++;
        }
    return Status;
}

/**
    Hook function to validate Password for the HDDs based on
    the token ALL_HDD_SAME_PW

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
HddPasswordAuthenticate (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    #if ALL_HDD_SAME_PW
     return HddPasswordAuthenticateAllHdd( Password, Ptr, bCheckUser);
    #else
     return HddPasswordAuthenticateHdd( Password, Ptr, bCheckUser);
    #endif
}

/**
    Updates the password for the current HDD alone.

    @param UINT32 Index,
    @param CHAR16 *Password,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
HddPasswordUpdateHdd (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    AMI_HDD_SECURITY_PROTOCOL   *HddSecurityProtocol = NULL;
    UINT16                      Control              = bCheckUser ? 0 : 1;
    UINT8                       RevisionCode         = 0;
    BOOLEAN                     Locked, Enabled;
    EFI_STATUS                  Status = EFI_UNSUPPORTED;
    UINT8                       Buffer[IDE_PASSWORD_LENGTH + 1];
    #if !SETUP_SAME_SYS_HDD_PW
    UINTN                       ii;
    #endif
    HDD_SECURITY_INTERNAL_DATA  *DataPtr;

#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
    UpdatePasswordToNonCaseSensitive(Password, Wcslen(Password));
#endif

    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)GetHddSecurityInternalDataPtr( Index );

    if ( DataPtr == NULL ) {
        return FALSE;
    }
    HddSecurityProtocol = DataPtr->HddSecurityProtocol;

    //
    //get the status of the device
    //
    if ( !(
                CheckSecurityStatus(
                                     HddSecurityProtocol, &Locked,
                                     SecurityLockedMask )
             && CheckSecurityStatus( HddSecurityProtocol, &Enabled,
                                     SecurityEnabledMask ))) {
        return FALSE;
    }

    if ( !Locked ) {
        if ( Password[0] == 0 ) {
            //
            //empty string is entered -> disable password
            //
            Status = HddSecurityProtocol->SecurityDisablePassword(
                HddSecurityProtocol,
                Control,
                HddSecurityInternalData[Index].PWD );
        } else {
            //
            //set new password
            //
            MemSet( &Buffer, IDE_PASSWORD_LENGTH + 1, 0 );
            #if !SETUP_SAME_SYS_HDD_PW

            for ( ii = 0; ii < IDE_PASSWORD_LENGTH + 1; ii++ ) {
                Buffer[ii] = (UINT8)Password[ii];

                if ( Password[ii] == L'\0' ) {
                    break;
                }
            }// end of for
            #else
            pBS->CopyMem( Buffer, Password, IDE_PASSWORD_LENGTH + 1 );
            #endif

            Status = HddSecurityProtocol->SecuritySetPassword(
                HddSecurityProtocol,
                Control,
                Buffer,
                RevisionCode );
        }
    }// end if(!Locked)

    if ( EFI_ERROR( Status )) {
        return FALSE;
    }

    return TRUE;
}

/**
    Updates the HDD password for all the HDDs present.

    @param UINT32 Index,
    @param CHAR16 *Password,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
HddPasswordUpdateAllHdd (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    UINTN       i;
    BOOLEAN     Status = FALSE;
        for ( i = 0; i < gHddSecurityCount; i++ ) {
            Status = HddPasswordUpdateHdd( (UINT32)i, Password, bCheckUser);
        }
    return Status;
}

/**
    Hook function to update the password for the HDDs based
    on the token ALL_HDD_SAME_PW.

    @param  UINT32 Index,
    @param  CHAR16 *Password,
    @param  BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
HddPasswordUpdate (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    #if ALL_HDD_SAME_PW
     return HddPasswordUpdateAllHdd( Index, Password, bCheckUser);
    #else
     return HddPasswordUpdateHdd( Index, Password, bCheckUser);
    #endif
}

/**
    This function handles password operations (check/validate/change/clear)
    for all HDD password fields

    @param  EFI_HII_HANDLE HiiHandle - HII handle of formset
    @param  UINT16 Class             - class of formset
    @param  UINT16 SubClass          - subclass of formset
    @param  UINT16 Key               - Id of setup control

    @retval EFI_STATUS

    @note
    Case1: Check password status
        EFI_SUCCESS    - No password
        EFI_NOT_FOUND  - Password exists
    Case2: Validate password
        EFI_SUCCESS    - Password is correct
        EFI_NOT_READY  - Password is wrong
    Case3: Set/Clear password
        EFI_SUCCESS    - Operation successful
        EFI_NOT_READY  - Operation failed
**/
EFI_STATUS
HddPasswordCallback(
        EFI_HII_HANDLE HiiHandle,
        UINT16         Class,
        UINT16         SubClass,
        UINT16         Key
)
{
    EFI_STATUS          Status;
    CALLBACK_PARAMETERS *CallbackParameters;
    CHAR16              PasswordBuffer[IDE_PASSWORD_LENGTH+1];
    static CHAR16       OldPasswordBuffer[IDE_PASSWORD_LENGTH+1];
    BOOLEAN             bCheckUser = FALSE;
    BOOLEAN             IsSecurityEnabled = FALSE;
    static BOOLEAN      IsPasswordValidated = FALSE;
    static BOOLEAN      NoPasswordExistedOnFirstCall = FALSE;
    static UINT8        LastHardDiskNumber = 0xFF;
    UINT8               CurrentHardDiskNumber;
    UINTN               size;
    VOID*               DataPtr;

    // Get actual parameter passed by browser
    CallbackParameters = GetCallbackParameters();

    // Check for supported browser actions
    if(!CallbackParameters || ((CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) && (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) && (CallbackParameters->Action != EFI_BROWSER_ACTION_DEFAULT_STANDARD))) {
        return EFI_UNSUPPORTED;
    }

    // Always return success here
    if((CallbackParameters->Action == EFI_BROWSER_ACTION_CHANGED) || (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD )) {
        return EFI_SUCCESS;
    }

    // Check which password field was selected
    switch(Key)
    {
    case HDD_SECURITY_GET_USERPASSWORD1_KEY:
        bCheckUser = TRUE;
        CurrentHardDiskNumber = 0;
        break;
#if defined(ALL_HDD_SAME_PW) && ALL_HDD_SAME_PW == 0
    case HDD_SECURITY_GET_USERPASSWORD2_KEY:
        bCheckUser = TRUE;
        CurrentHardDiskNumber = 1;
        break;
    case HDD_SECURITY_GET_USERPASSWORD3_KEY:
        bCheckUser = TRUE;
        CurrentHardDiskNumber = 2;
        break;
    case HDD_SECURITY_GET_USERPASSWORD4_KEY:
        bCheckUser = TRUE;
        CurrentHardDiskNumber = 3;
        break;
    case HDD_SECURITY_GET_USERPASSWORD5_KEY:
        bCheckUser = TRUE;
        CurrentHardDiskNumber = 4;
        break;
    case HDD_SECURITY_GET_USERPASSWORD6_KEY:
        bCheckUser = TRUE;
        CurrentHardDiskNumber = 5;
        break;
#endif //ALL_HDD_SAME_PW
#if defined(DISPLAY_MASTER_PASSWORD) && DISPLAY_MASTER_PASSWORD
    case HDD_SECURITY_GET_MASTERPASSWORD1_KEY:
        bCheckUser = FALSE;
        CurrentHardDiskNumber = 0;
        break;
#if defined(ALL_HDD_SAME_PW) && ALL_HDD_SAME_PW == 0
    case HDD_SECURITY_GET_MASTERPASSWORD2_KEY:
        bCheckUser = FALSE;
        CurrentHardDiskNumber = 1;
        break;
    case HDD_SECURITY_GET_MASTERPASSWORD3_KEY:
        bCheckUser = FALSE;
        CurrentHardDiskNumber = 2;
        break;
    case HDD_SECURITY_GET_MASTERPASSWORD4_KEY:
        bCheckUser = FALSE;
        CurrentHardDiskNumber = 3;
        break;
    case HDD_SECURITY_GET_MASTERPASSWORD5_KEY:
        bCheckUser = FALSE;
        CurrentHardDiskNumber = 4;
        break;
    case HDD_SECURITY_GET_MASTERPASSWORD6_KEY:
        bCheckUser = FALSE;
        CurrentHardDiskNumber = 5;
        break;
#endif //ALL_HDD_SAME_PW
#endif //DISPLAY_MASTER_PASSWORD
    default:
        CurrentHardDiskNumber = 0xFF;
        LastHardDiskNumber = 0xFF;
        break;
    }

    // Not a valid HDD number
    if( CurrentHardDiskNumber == 0xFF ) {
        return EFI_NOT_FOUND;
    }

    // Just to make sure that two subsequent calls are for same HDD
    if(LastHardDiskNumber != 0xFF && LastHardDiskNumber != CurrentHardDiskNumber) {
        IsPasswordValidated = FALSE;
        LastHardDiskNumber = 0xFF;
        NoPasswordExistedOnFirstCall = FALSE;
        OldPasswordBuffer[0] = L'\0';
    }

    // Allocate memory for setup data
    if(NULL == gHddSecurityConfig) {
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    sizeof(HDD_SECURITY_CONFIG),
                                    (VOID**)&gHddSecurityConfig);
        if(EFI_ERROR(Status) || NULL == gHddSecurityConfig) {
            return EFI_NOT_FOUND;
        }
    }

    // Get setup data
    size = sizeof(HDD_SECURITY_CONFIG);
    Status = pRS->GetVariable(  L"HDDSecConfig",
                                &gHddSecurityConfigGuid,
                                NULL,
                                &size,
                                gHddSecurityConfig);
    if(EFI_ERROR(Status)) {
        return EFI_NOT_FOUND;
    }

    IsSecurityEnabled = gHddSecurityConfig->Enabled[CurrentHardDiskNumber] ? TRUE : FALSE;

    // Get string from HII database using input string token
    if(CallbackParameters->Value->string != 0) {
        size = sizeof(PasswordBuffer);
        Status = HiiLibGetString(   HiiHandle,
                                    CallbackParameters->Value->string,
                                    &size,
                                    (EFI_STRING)PasswordBuffer);
        if(EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }
    }
    else {
        return EFI_NOT_FOUND;
    }

    // Return password status
    if (PasswordBuffer[0] == L'\0' && (IsPasswordValidated == FALSE) && (NoPasswordExistedOnFirstCall == FALSE)) {
        LastHardDiskNumber = CurrentHardDiskNumber;
        OldPasswordBuffer[0] = L'\0';
        if(IsSecurityEnabled == TRUE) {
            NoPasswordExistedOnFirstCall = FALSE;
            return EFI_NOT_FOUND;   // Pre-existing password
        }
        else {
            NoPasswordExistedOnFirstCall = TRUE;
            return EFI_SUCCESS;    // No pre-existing password.
        }
    }
    else if(IsSecurityEnabled == TRUE && IsPasswordValidated == FALSE) { // Validate password
        DataPtr = GetHddSecurityInternalDataPtr( CurrentHardDiskNumber );
        Status = HddPasswordAuthenticate( PasswordBuffer, DataPtr, bCheckUser ); //Authenticate it
        if(EFI_ERROR( Status )) {
            return EFI_NOT_READY;   // Password is wrong.
        }
        else {
            IsPasswordValidated = TRUE; // Set it so that password could be changed/cleared on next call
            Wcscpy(OldPasswordBuffer,PasswordBuffer); // Save existing HDD password
            return EFI_SUCCESS;    // Password is correct.
        }
    }
    else { // Set new password or clear old one
        // Reset static variables
        IsPasswordValidated = FALSE;
        NoPasswordExistedOnFirstCall = FALSE;

/* Commenting below code. Now Master password will be cleared on ESC when user password is not set
        // If existing password is equals to new password then do nothing
        // because of below code master password cannot be removed when security is disabled
        if(Wcscmp(OldPasswordBuffer,PasswordBuffer)==0) {
            OldPasswordBuffer[0] = L'\0';
            return EFI_SUCCESS;
        }
*/

        OldPasswordBuffer[0] = L'\0';

        // Set/Clear HDD password
        if(!HddPasswordUpdate( (UINT32)CurrentHardDiskNumber, PasswordBuffer, bCheckUser )) {
        	if(bCheckUser) {
        		return EFI_NOT_READY; // Not updated
        	}
        }

        //Update NVRAM data and browser data
        RefreshHddSecuritySetupData();
    }
    return EFI_SUCCESS;
}

/**
    This function will update security information for all HDDs

    @param  EFI_HII_HANDLE HiiHandle - HII handle of formset
    @param  UINT16 Class             - class of formset
    @param  UINT16 SubClass          - subclass of formset
    @param  UINT16 Key               - Id of setup control

    @retval EFI_SUCCESS      - Operation finished
    @retval EFI_UNSUPPORTED  - Operation not supported on current event

**/
EFI_STATUS
HddSecurityUpdateFormCallback(
        EFI_HII_HANDLE HiiHandle,
        UINT16         Class,
        UINT16         SubClass,
        UINT16         Key
)
{
    CALLBACK_PARAMETERS *CallbackParameters;

    // Get actual parameter passed by browser
    CallbackParameters = GetCallbackParameters();

    // Check for supported browser actions
    if(!CallbackParameters || ((CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) && (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED)) )
        return EFI_UNSUPPORTED;

    // Update security information (NVRAM data and browser data) for all HDDs
    RefreshHddSecuritySetupData();

    return EFI_SUCCESS;
}
#endif

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
