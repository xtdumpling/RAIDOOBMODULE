//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Hide IPMI page when BMC isn't present.
//    Reason:   Refer from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jun/07/2016
//
//****************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file ServerMgmtSetup.c
    ServerMgmtSetup driver to initialize server management
    setup screen

**/

//----------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <UefiHii.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiUtilities.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include "ServerMgmtSetup.h"
#include "ServerMgmtSetupCallbackList.h"

#if SMCPKG_SUPPORT
#include "SmcPkg\Include\SuperMDxeDriver.h"

EFI_GUID gEfiSuperMDriverProtocolGuid = EFI_SUPERM_DRIVER_PROTOCOL_GUID;
static  SUPERMDRIVER_PROTOCOL *gSuperMProtocol;
#endif
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//
// Macro Definition
//
#define NUMBER_OF_FORMSETS (sizeof(gSetupCallBack)/sizeof(CALLBACK_INFO))

//
// Global variables
//
EFI_HANDLE                          gThisImageHandle = NULL;
SERVER_MGMT_CONFIGURATION_DATA      gServerMgmtConfiguration;
static EFI_HII_STRING_PROTOCOL      *gHiiString = NULL;
static EFI_HII_DATABASE_PROTOCOL    *gHiiDatabase = NULL;
//
// Declare list of string initialization functions
//
typedef VOID (STRING_INIT_FUNC)(
    EFI_HII_HANDLE HiiHandle, UINT16 Class
);

extern STRING_INIT_FUNC SERSVER_MGMT_STRING_INIT_LIST EndOfFunctionList;
STRING_INIT_FUNC *gStringInitFunc[] = { SERSVER_MGMT_STRING_INIT_LIST NULL };

/**

    These Data Structure define a structure used to match a specific
    Callback Protocol to an HII Form through the use of Class and SubClass
    values

**/
typedef struct {
    EFI_HII_CONFIG_ACCESS_PROTOCOL Callback;
    UINT16 Class, SubClass;
} SETUP_CALLBACK;

EFI_STATUS Callback(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
);

/**
    These Variable definitions define the different formsets and what Callback
    protocol should be used for each one

**/
SETUP_CALLBACK gServerMgmtCallbackProtocol = {{NULL, NULL, Callback}, SERVER_MGMT_CLASS_ID, 0};

/**

    This array contains the different HII packages that are used in the system

  @note
  The HiiHandle is updated in the LoadResources function when the HII Packages
  are loaded
**/
CALLBACK_INFO gSetupCallBack[] = {
    // Last field in every structure will be filled by the Setup
    { &gEfiServerMgmtSetupVariableGuid, &gServerMgmtCallbackProtocol.Callback, SERVER_MGMT_CLASS_ID, 0, 0}
};

//----------------------------------------------------------------------

/**
    This function updates a string defined by the StrRef Parameter in the HII
    database with the string and data passed in.

    @param HiiHandle - handle that indicates which HII Package that
                   is being used
    @param StrRef  - String Token defining which string in the
        database to update
    @param sFormat - string with format descriptors in it
    @param  ...    - extra parameters that define data that correlate
                  to the format descriptors in the String

    @retval VOID

**/

VOID
InitString (
  IN EFI_HII_HANDLE HiiHandle,
  IN STRING_REF     StrRef,
  IN CHAR16         *sFormat,
  IN ... )
{
    CHAR16 s[1024];
    VA_LIST ArgList;
    VA_START(ArgList,sFormat);
    Swprintf_s_va_list(s,sizeof(s),sFormat,ArgList);
    VA_END(ArgList);
    HiiSetString(HiiHandle, StrRef, s, NULL);
}
#if    CORE_COMBINED_VERSION < 0x50007
/**

    This Data Structure is used by the setup infrastructure to define 
    callback functions that should be used for interacting with setup forms
    or individual questions.

**/
typedef struct{
    UINT16 Class, SubClass, Key;
    SETUP_ITEM_CALLBACK_HANDLER *UpdateItem;
} SETUP_ITEM_CALLBACK;
#endif

//
// Brings the definitions of the SDL token defined list of callbacks into this
//  file as a list of functions that can be called
#define ITEM_CALLBACK(Class,Subclass,Key,Callback) Callback
extern SETUP_ITEM_CALLBACK_HANDLER SERVER_MGMT_SETUP_ITEM_CALLBACK_LIST EndOfList;
#undef ITEM_CALLBACK

//
// This creates an array of callbacks to be used
//
#define ITEM_CALLBACK(Class,Subclass,Key,Callback) {Class,Subclass,Key,&Callback}
SETUP_ITEM_CALLBACK ServerMgmtSetupItemCallback[] = { SERVER_MGMT_SETUP_ITEM_CALLBACK_LIST {0, 0, 0, NULL} };

CALLBACK_PARAMETERS *CallbackParametersPtr = NULL;
CALLBACK_PARAMETERS* GetCallbackParameters(){
    return CallbackParametersPtr;
}

/**
    This function is used to identify the function to call when an interactive 
    item has been triggered in the setup browser based on the information in
    the Callback protocol and the SetupCallBack Array

    @param This Pointer to the instance of the callback
                protocol
    @param KeyValue Unique value that defines the type of data to expect
                    in the Data parameter
    @param Data Data defined by KeyValue Parameter
    @param Packet Data passed from the Callback back to
               the setup Browser

    @retval EFI_SUCCESS

**/

EFI_STATUS Callback(
    IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
    IN        EFI_BROWSER_ACTION               Action,
    IN        EFI_QUESTION_ID                  KeyValue,
    IN        UINT8                            Type,
    IN        EFI_IFR_TYPE_VALUE               *Value,
    OUT       EFI_BROWSER_ACTION_REQUEST       *ActionRequest )
{
    SETUP_CALLBACK *pCallback = (SETUP_CALLBACK*)This;
    CALLBACK_PARAMETERS CallbackParameters;
    EFI_STATUS Status;
    SETUP_ITEM_CALLBACK *pItemCallback = ServerMgmtSetupItemCallback;

    CallbackParametersPtr = &CallbackParameters;

    CallbackParameters.This = (VOID*)This;
    CallbackParameters.Action = Action;
    CallbackParameters.KeyValue = KeyValue;
    CallbackParameters.Type = Type;
    CallbackParameters.Value = Value;
    CallbackParameters.ActionRequest = ActionRequest;
    if (ActionRequest) *ActionRequest=EFI_BROWSER_ACTION_REQUEST_NONE;
    Status = EFI_UNSUPPORTED;

    while(pItemCallback->UpdateItem) {

        if ( pItemCallback->Class == pCallback->Class
            &&  pItemCallback->SubClass == pCallback->SubClass
            &&  pItemCallback->Key == KeyValue ) {

            Status = pItemCallback->UpdateItem(
                        gSetupCallBack[0].HiiHandle,
                        pItemCallback->Class, pItemCallback->SubClass,
                        KeyValue );
            if (Status != EFI_UNSUPPORTED) break;
        }
        pItemCallback++;
    }
    CallbackParametersPtr = NULL;
    return Status;
}
/**
    This function is called for each Formset and initializes strings based on
    the porting provided and then updates the HII database

    @param HiiHandle - handle that indicates which HII Package
                       that is being used
    @param pCallBackFound - pointer to an instance of CALLBACK_INFO
                       that works with HiiHandle

    @retval VOID

**/

VOID
InitFunction (
  IN EFI_HII_HANDLE     HiiHandle,
  IN CALLBACK_INFO      *pCallBackFound )
{
    UINT16  i;

    if (!pCallBackFound || !pCallBackFound->HiiHandle) {
        return;
    }

    for (i = 0; gStringInitFunc[i] != NULL; i++) {
        gStringInitFunc[i] (HiiHandle, pCallBackFound->Class);
    }

    return;
}
/**
    This function publishes all HII resources and initializes the HII databases
    There is a token ALWAYS_PUBLISH_HII_RESOURCES that would call this function
    on every boot not just when the user tries to enter Setup

    @param Event Event that was triggered
    @param Context data pointer to information that is defined when the 
        event is registered

    @retval VOID

**/

VOID
EFIAPI
SetupCallback (
  IN EFI_EVENT      Event,
  IN VOID           *Context )
{
#if SMCPKG_SUPPORT	
    EFI_STATUS	Status;		
    SUPERMBOARDINFO	gSuperMBoardInfo;
#endif
    static BOOLEAN ResourcesLoaded = FALSE;

    //
    // Locate the HII based protocols
    //
    if ( !gHiiString
        && EFI_ERROR(pBS->LocateProtocol(
                          &gEfiHiiStringProtocolGuid,
                          NULL,
                          (VOID **)&gHiiString)) ) {
        return ;
    }
    if ( !gHiiDatabase
        && EFI_ERROR(pBS->LocateProtocol(
                        &gEfiHiiDatabaseProtocolGuid,
                        NULL,
                        (VOID **)&gHiiDatabase)) ) {
        return ;
    }
    if (Event) {
        pBS->CloseEvent(Event);
    }
    if (ResourcesLoaded) {
        return;
    }
    ResourcesLoaded = TRUE;
    LoadResources(
        gThisImageHandle,
        NUMBER_OF_FORMSETS,
        gSetupCallBack,
        InitFunction);
    
#if SMCPKG_SUPPORT
    Status = pBS->LocateProtocol(&gEfiSuperMDriverProtocolGuid, NULL, &gSuperMProtocol);
    if(Status != EFI_SUCCESS){
	return;
    }
    Status = gSuperMProtocol->GetSuperMBoardInfo(&gSuperMBoardInfo);
    if(gSuperMBoardInfo.BMC_Present == 0){
        gHiiDatabase->RemovePackageList(gHiiDatabase, gSetupCallBack[0].HiiHandle);
    }
#endif  
    
    return;
}

/**
    Entry point of the Server Management screen setup driver.

    
    @param  ImageHandle A handle for the image that is
                  initializing this driver
    @param SystemTable A pointer to the EFI system table
            event is registered

    @retval EFI_STATUS Driver initialized successfully

**/

EFI_STATUS
InstallServerMgmtSetupScreen (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable )
{
    UINTN       Size;
    EFI_STATUS  Status;
    UINT32      Attributes;

    InitAmiLib(ImageHandle,SystemTable);

    gThisImageHandle = ImageHandle;
    Size = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
    Status = pRS->GetVariable (
                L"ServerSetup",
                &gEfiServerMgmtSetupVariableGuid,
                &Attributes,
                &Size,
                &gServerMgmtConfiguration );
    if (Status == EFI_NOT_FOUND) {
        SetupCallback(NULL,NULL);
        pRS->SetVariable(
            L"ServerSetup",
            &gEfiServerMgmtSetupVariableGuid,
            Attributes,
            sizeof(SERVER_MGMT_CONFIGURATION_DATA),
            &gServerMgmtConfiguration );
    } else {
#if ALWAYS_PUBLISH_HII_RESOURCES
    {
        VOID            *BdsEventBeforeSetupRegistration;
        EFI_EVENT       BdsEventBeforeSetup;
        EFI_GUID        guidBdsBeforeSetup = EFI_BDS_EVENT_BEFORE_SETUP_GUID;
        //
        // Register call back notification on "BDS event before setup GUID".
        //
        RegisterProtocolCallback(
            &guidBdsBeforeSetup,
            SetupCallback,
            NULL,
            &BdsEventBeforeSetup,
            &BdsEventBeforeSetupRegistration );

    }
#else 
    {
        VOID            *pSetupRegistration;
        EFI_EVENT       SetupEnterEvent;
        EFI_GUID        guidSetupEnter = AMITSE_SETUP_ENTER_GUID;
        //
        // Register callback notification on "Setup Enter GUID".
        //
        RegisterProtocolCallback(
            &guidSetupEnter,
            SetupCallback,
            NULL,
            &SetupEnterEvent,
            &pSetupRegistration );
    }
#endif
    }
    return EFI_SUCCESS;
}

/**
  Retrieves a string from a string package in a specific language.  

  @param  HiiHandle    HII Handle value.
  @param  StringId     The identifier of the string to retrieved from the string 
                         package associated with HiiHandle.
  @param  String       Pointer to store the retrieved String.
  @param  StringSize   Size of the required string.   
  @param  Language     The language of the string to retrieve.  If this parameter 
                         is NULL, then the current platform language is used.  The 
                         format of Language must follow the language format assumed 
                         the HII Database.

  @retval EF_SUCCESS  The string specified by StringId is present in the string package.
  @retval Other       The string was not returned.

**/

EFI_STATUS
EFIAPI
IpmiHiiGetString (
  IN       EFI_HII_HANDLE  HiiHandle,
  IN       EFI_STRING_ID   StringId,
  IN       EFI_STRING      String,
  IN       UINTN           *StringSize,
  IN CONST CHAR8           *Language  OPTIONAL
  )
{
    EFI_HII_STRING_PROTOCOL *HiiString = NULL;
    EFI_STATUS  Status;
    CHAR8       *SupportedLanguages;
    CHAR8       *PlatformLanguage;
    CHAR8       *BestLanguage;
    CHAR8       *StartLang;
    CHAR8       *TempLanguage;
    UINTN       TempStrSize;

    ASSERT (StringId != 0);

    //
    // Initialize all allocated buffers to NULL
    //
    SupportedLanguages = NULL;
    PlatformLanguage   = NULL;
    BestLanguage       = NULL;

    //
    // Get the languages that the package specified by HiiHandle supports
    //
    SupportedLanguages = HiiGetSupportedLanguages (HiiHandle);
    if (SupportedLanguages == NULL) {
        goto Error;
    }

    //
    // Get the current platform language setting
    //
    GetEfiGlobalVariable2 (L"PlatformLang", (VOID**)&PlatformLanguage, NULL);

    //
    // If Language is NULL, then set it to an empty string, so it will be 
    // skipped by GetBestLanguage()
    //
    if (Language == NULL) {
        Language = "";
    }

    //
    // Get the best matching language from SupportedLanguages
    //
    BestLanguage = GetBestLanguage (
                        SupportedLanguages, 
                        FALSE,                                             // RFC 4646 mode
                        Language,                                          // Highest priority 
                        PlatformLanguage != NULL ? PlatformLanguage : "",  // Next highest priority
                        SupportedLanguages,                                // Lowest priority 
                        NULL
                        );
    if(BestLanguage == NULL) {
        goto Error;
    }

    TempStrSize = *StringSize;
    //
    // Retrieve string in the string package for the BestLanguage
    //
    Status = gHiiString->GetString (
                         gHiiString,
                         BestLanguage,
                         HiiHandle,
                         StringId,
                         String,
                         &TempStrSize,
                         NULL
                         );

    if(Status == EFI_INVALID_LANGUAGE) {

        //
        // Loop through each language that the string supports
        //
        for (StartLang = SupportedLanguages; *StartLang != '\0'; ) {
            //
            // Cache a pointer to the beginning of the current language in the list of languages
            //
            TempLanguage = StartLang;
            TempStrSize = *StringSize;
            //
            // Search for the next language separator and replace it with a Null-terminator
            //
            for (; *StartLang != 0 && *StartLang != ';'; StartLang++);
            if (*StartLang != 0) {
                *(StartLang++) = '\0';
            }

            //
            // Retrieve the size of the string in the string package for the BestLanguage
            //
            Status = gHiiString->GetString (
                                    gHiiString,
                                    TempLanguage,
                                    HiiHandle,
                                    StringId,
                                    String,
                                    &TempStrSize,
                                    NULL
                                    );
            if(Status == EFI_SUCCESS) {
                break;
            }
        }
    }

Error:
    //
    // Free allocated buffers
    //
    if (SupportedLanguages != NULL) {
        FreePool (SupportedLanguages);
    }
    if (PlatformLanguage != NULL) {
        FreePool (PlatformLanguage);
    }
    if (BestLanguage != NULL) {
        FreePool (BestLanguage);
    }

    //
    // Return the status
    //
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
