//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  Rev. 1.02
//    Bug Fix:  Fixed AMIBCP default password function can not work.
//    Reason:   
//    Auditor:  Alan Chen
//    Date:     May/09/2017
//
//  Rev. 1.01
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//  Rev. 1.00
//    Bug Fix:  Set default passwrod by AMIBCP tool.
//    Reason:   
//    Auditor:  Jimmy Chiu (Refer Grantley)
//    Date:     Mar/23/16
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/**
 * @file
 * This file contains supporting functions, data types and data that belong to the Setup driver.
 */
#include <Token.h>
#include <AutoId.h>
#include <SetupStrDefs.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>

#include <Guid/GlobalVariable.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>
#include <Protocol/Cpu.h>
#include <Protocol/SimpleTextOut.h>
#include <Library/AmiSetupLibrary.h>

#include <TimeStamp.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>

#include <SetupCallbackList.h>

#include <Protocol/HiiPackageList.h>
#include <Guid/ZeroGuid.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>

// AmiCompatibilityPkg - Future Removal
#include <Setup.h>
#include <AmiDxeLib.h>

#include "SmcSetupModify.h" // Supermicro

//=======================================================================
// MACROs
//#define CONVERT_TO_WSTRING(a)   L###a
#define STR                     CONVERT_TO_WSTRING
//#define CONVERT_TO_STRING(a)    #a
#define LastLangCodes           L"PlatformLastLangCodes"

//=======================================================================
extern EFI_GUID gAmiTseSetupEnterGuid;
static EFI_GUID AmiSetupGuid = SETUP_GUID;

/// Global pointer to the Handle associated with this driver
EFI_HANDLE ThisImageHandle = NULL;

/// Linked list variable used to keep track of the Ami Callback functions
LIST_ENTRY AmiCallbackList;

/// Linked list variable used to keep track of the Ami String Initilization functions
LIST_ENTRY AmiStringInitializationList;

/// Linked list variable used to keep track of the Ami Extract Config List
LIST_ENTRY AmiExtractConfigList;

/// Linked list variabled used to keep track of the Ami Route Config List
LIST_ENTRY AmiRouteConfigList;

EFI_STATUS HiiAccessExtractConfig(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Request,
    OUT EFI_STRING *Progress,
    OUT EFI_STRING *Results
);

EFI_STATUS HiiAccessRouteConfig(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Configuration,
    OUT EFI_STRING *Progress
);

EFI_STATUS EFIAPI HiiAccessExtractConfigWrapper(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Request,
    OUT EFI_STRING *Progress,
    OUT EFI_STRING *Results
);

EFI_STATUS EFIAPI HiiAccessRouteConfigWrapper(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Configuration,
    OUT EFI_STRING *Progress
);

EFI_STATUS EFIAPI CallbackFunction(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
);

/// Private data structure to keep track of data associated with the setup form
typedef struct
{
    EFI_HII_CONFIG_ACCESS_PROTOCOL HiiAccessProtocol;   ///< Instance of the HiiAccessProtocol that is produced by this module
    EFI_HANDLE CallbackHandle;                          ///< Handle that the HiiAccessProtocol is installed upon
    EFI_HII_HANDLE HiiHandle;                           ///< HiiHandle returned when NewPack is called when installing the String Database
    EFI_GUID FormGuid;                                  ///< GUID of the formset produced by this module
} SETUP_FORM_PRIVATE_DATA;

#ifndef SETUP_ITEM_CALLBACK_DEFINED
/// Legacy callback structure used to store callback functions
typedef struct{
    UINT16 Class;                               ///< Class associated with this callback
    UINT16 SubClass;                            ///< Subclass associated with this callback
    UINT16 Key;                                 ///< Key (QuestionId) associated with this callback
    SETUP_ITEM_CALLBACK_HANDLER *UpdateItem;    ///< Callback function pointer
} SETUP_ITEM_CALLBACK;
#endif

/// Global pointer used to store the call back paramters so they can be consumed by the legacy callbacks
CALLBACK_PARAMETERS *CallbackParametersPtr = NULL;

/**
 * Function for use by legacy callbacks to retrieve the full callback paramters
 * @return CALLBACK_PARAMETERS Pointer to the CALLBACK_PARAMTER structure for consumption by legacy callbacks
 */
CALLBACK_PARAMETERS* GetCallbackParameters(){
    return CallbackParametersPtr;
}

static SETUP_FORM_PRIVATE_DATA FormData = {{HiiAccessExtractConfigWrapper, HiiAccessRouteConfigWrapper, CallbackFunction}, NULL, NULL, ZERO_GUID};

// Declare list of string initialization functions
/// function pointer declaration for the string initialization functions
typedef VOID (STRING_INIT_FUNC)(EFI_HII_HANDLE HiiHandle, UINT16 Class);
extern STRING_INIT_FUNC SETUP_STRING_INIT_LIST EndOfFunctionList;
static STRING_INIT_FUNC *StringInitFunc[] = { SETUP_STRING_INIT_LIST NULL };

// Brings the definitions of the SDL token defined list of callbacks into this
//  file as a list of functions that can be called
#define ITEM_CALLBACK(Class,Subclass,Key,Callback) Callback
extern SETUP_ITEM_CALLBACK_HANDLER SETUP_ITEM_CALLBACK_LIST EndOfList;
#undef ITEM_CALLBACK

// This creates an array of callbacks to be used
#define ITEM_CALLBACK(Class,Subclass,Key,Callback) {Class,Subclass,Key,&Callback}
static SETUP_ITEM_CALLBACK SetupItemCallback[] = { SETUP_ITEM_CALLBACK_LIST {0,0,0,NULL} };

/// Function prototype for external fucntions called as part of the initialization.
typedef VOID (SETUP_ENTRY_HOOK_LIST_FUNC)(VOID);
static SETUP_ENTRY_HOOK_LIST_FUNC *SetupPostHiiInstallFunctionList[] = { SETUP_MODULE_POST_INSTALL_HII_PACKAGES_HOOK NULL };

/**
 * Function that updates a string defined by the StrRef parameter into the Hii Database
 * using the string generated from the sFormat parameters
 *
 * @param HiiHandle Handle of the HiiDatabase to store the string
 * @param StrRef String identifier in the database to update with the string
 * @param sFormat String with format descriptors in it
 * @param ... Extra parameters that define data to be used to replace the format descriptors
 * @return
 */
VOID InitString(EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StrRef, CHAR16 *sFormat, ...)
{
    CHAR16 s[1024];
    
    VA_LIST  ArgList;
    VA_START(ArgList, sFormat);
    UnicodeVSPrint(s, sizeof(s), sFormat, ArgList);
    VA_END(ArgList);
    
    HiiSetString(HiiHandle, StrRef, s, NULL);
}

#if SMCPKG_SUPPORT
VOID
AmiBcpDefaultPassword(
  IN EFI_HII_HANDLE HiiHandle
  )
{
    UINTN            VariableSize;
    EFI_STATUS       Status;
    EFI_GUID         AmiTseGuid = AMITSESETUP_GUID; 
    SETUP_DATA*      AmiTseData = NULL;
    UINT8            i, D4Update=0;
    AMITSESETUP      AMISetupData;
    UINTN            Size=512;
    CHAR16           D4AdminPassword[512]=L"",D4UserPassword[512]=L"";
    UINTN            SetupDataSize = sizeof(SETUP_DATA);  
    UINT8            *pStdDefaults = NULL;
    UINTN            StdDefaultsSize = 0; 
    
    VariableSize = sizeof(AMITSESETUP);
    Status = pRS->GetVariable(
	    		L"AMITSESetup",
				&AmiTseGuid,
				NULL,
				&VariableSize,
				&AMISetupData
				);

    if (!EFI_ERROR(Status))
    {
        Status = HiiLibGetString(HiiHandle, STRING_TOKEN(STR_D4_ADMIN_PSWD), &Size, D4AdminPassword);
        if ((AMISetupData.AdminPassword[0]==0) && (UINT8)(D4AdminPassword[0]!=0) && (!EFI_ERROR(Status)))
        {
           D4Update =1;
           for(i=0;i<80;i++)
           {
           	if(D4AdminPassword[i]<0x3A) D4AdminPassword[i] = D4AdminPassword[i] - 0x30;
           	else if(D4AdminPassword[i]<0x5B) D4AdminPassword[i] = D4AdminPassword[i] - 0x37;
           	else if(D4AdminPassword[i]<0x7B) D4AdminPassword[i] = D4AdminPassword[i] - 0x57;
           }
	   for (i = 0; i < PASSWORD_MAX_SIZE; i++) AMISetupData.AdminPassword[i] = (UINT16)(D4AdminPassword[4*i]*16 + D4AdminPassword[4*i+1] + D4AdminPassword[4*i+2]*4096 + D4AdminPassword[4*i+3]*256);
        }

        Size = 512;
    	Status = HiiLibGetString(HiiHandle, STRING_TOKEN(STR_D4_USER_PSWD), &Size, D4UserPassword);
    	if ((AMISetupData.UserPassword[0]==0) && (UINT8)(D4UserPassword[0]!=0) && (!EFI_ERROR(Status)))
        {
           D4Update =1;
	   for(i=0;i<80;i++)
	   {
		if(D4UserPassword[i]<0x3A) D4UserPassword[i] = D4UserPassword[i] - 0x30;
		else if(D4UserPassword[i]<0x5B) D4UserPassword[i] = D4UserPassword[i] - 0x37;
		else if(D4UserPassword[i]<0x7B) D4UserPassword[i] = D4UserPassword[i] - 0x57;
	   }

	   for (i = 0; i < PASSWORD_MAX_SIZE; i++) 
               AMISetupData.UserPassword[i] = (UINT16)(D4UserPassword[4*i]*16 + D4UserPassword[4*i+1] + D4UserPassword[4*i+2]*4096+ D4UserPassword[4*i+3]*256);
	}
	
        if (D4Update==1)
        {
            Status = pRS->SetVariable(L"AMITSESetup",
	                              &AmiTseGuid,
	                              EFI_VARIABLE_NON_VOLATILE |
	                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
	                              EFI_VARIABLE_RUNTIME_ACCESS,
	                              sizeof(AMITSESETUP),
	                              &AMISetupData);
            IoWrite8(0xcf9, 0x06);
        }
    }   
}
#endif //SMCPKG_SUPPORT

/**
 * Update the string database with information obtained from querying the system.
 * The strings that are updated contain information about the BIOS version, the
 * system table information and project revisions.
 *
 * @param HiiHandle Handle of the HiiDatabase entry
 */
EFI_STATUS InitSystemVersionString(EFI_HII_HANDLE HiiHandle)
{
    CHAR16 *FirmwareVendor  = (gST->FirmwareVendor) ? gST->FirmwareVendor : CONVERT_TO_WSTRING(CORE_VENDOR);
    UINT32 FirmwareRevision =  (gST->FirmwareRevision) ? gST->FirmwareRevision : CORE_COMBINED_VERSION;

#if SMCPKG_SUPPORT
    AmiBcpDefaultPassword (HiiHandle);
#endif //SMCPKG_SUPPORT

    InitString(HiiHandle, STRING_TOKEN(STR_BIOS_VENDOR_VALUE), L"%s", FirmwareVendor);
    if(CORE_COMBINED_VERSION >= 0x5000b)
        InitString(HiiHandle, STRING_TOKEN(STR_BIOS_CORE_VERSION_VALUE), L"%d.%02d", ((UINT16*)&FirmwareRevision)[1], *(UINT16*)&FirmwareRevision);
    else
        InitString(HiiHandle, STRING_TOKEN(STR_BIOS_CORE_VERSION_VALUE), L"%d.%03d", ((UINT16*)&FirmwareRevision)[1], *(UINT16*)&FirmwareRevision);

    InitString(
        HiiHandle,STRING_TOKEN(STR_BIOS_VERSION_VALUE),
#if PROJECT_BUILD_NUMBER_IN_TITLE_SUPPORTED && defined(PROJECT_BUILD)
#ifdef EFIx64
        L"%s %d.%02d.%03d x64",
#else
        L"%s %d.%02d.%03d",
#endif
        STR(PROJECT_TAG), PROJECT_MAJOR_VERSION, PROJECT_MINOR_VERSION, PROJECT_BUILD
#else //#if PROJECT_BUILD_NUMBER_IN_TITLE_SUPPORTED && defined(PROJECT_BUILD)
#ifdef EFIx64
        L"%s %d.%02d x64",
#else
        L"%s %d.%02d",
#endif
        STR(PROJECT_TAG), PROJECT_MAJOR_VERSION, PROJECT_MINOR_VERSION
#endif//#if PROJECT_BUILD_NUMBER_IN_TITLE_SUPPORTED && defined(PROJECT_BUILD)
    );

    InitString(HiiHandle, STRING_TOKEN(STR_BIOS_DATE_VALUE), L"%s %s", L_TODAY, L_NOW);

    InitString( HiiHandle,
                STRING_TOKEN(STR_BIOS_COMPLIANCY_VALUE),
                L"UEFI %d.%d; PI %d.%d",
                ((UINT16*)&gST->Hdr.Revision)[1],
                ((UINT16*)&gST->Hdr.Revision)[0] / 10,
                ((UINT16*)&gDS->Hdr.Revision)[1],
                ((UINT16*)&gDS->Hdr.Revision)[0] / 10);

    InitString(HiiHandle, STRING_TOKEN(STR_MIN_PASSWORD_LENGTH__VALUE), L"%d", PASSWORD_MIN_SIZE);
    InitString(HiiHandle, STRING_TOKEN(STR_MAX_PASSWORD_LENGTH__VALUE), L"%d", PASSWORD_MAX_SIZE);

    return EFI_SUCCESS;
}

/**
 * Function elinked that will register the string initialization functions
 */
VOID RegisterSetupItems(VOID)
{
//    RegisterExtractConfig(HiiAccessExtractConfig);
//    RegisterAccessRouteConfig(HiiAccessRouteConfig);
	AmiSetupRegisterStringInitializer(InitSystemVersionString);
    return;
}

/**
 * Determine the current language that will be used based on the language related
 * UEFI NVRAM variables
 *
 * @param HiiString Pointer to the Hii String protocol function
 * @param HiiHandle Handle of the HiiDatabase entry
 * @return
 */
VOID InitLanguages(EFI_HII_STRING_PROTOCOL *HiiString, EFI_HII_HANDLE HiiHandle)
{
    EFI_STATUS Status;
    UINTN Size = 0;
    CHAR8 *LangCodes = NULL;
#if defined(FILTER_CUSTOM_LANGUAGE_CODES)&&(FILTER_CUSTOM_LANGUAGE_CODES)
    UINTN i = 0;
    UINTN j;
#endif

    Status = HiiString->GetLanguages(HiiString, HiiHandle, LangCodes, &Size);
    if(Status == EFI_BUFFER_TOO_SMALL)
    {
        gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&LangCodes);
        Status = HiiString->GetLanguages(HiiString, HiiHandle, LangCodes, &Size);
    }
    if(EFI_ERROR(Status))
    {
        Size = sizeof(CONVERT_TO_STRING(DEFAULT_LANGUAGE_CODE));
        gBS->AllocatePool(EfiBootServicesData, Size, (VOID**)&LangCodes);
        gBS->CopyMem(LangCodes, CONVERT_TO_STRING(DEFAULT_LANGUAGE_CODE), Size);
    }

#if defined(FILTER_CUSTOM_LANGUAGE_CODES)&&(FILTER_CUSTOM_LANGUAGE_CODES)
    // Go through the LangCodes and ensure the x-AMI and x-OEM languages are not included
    //  in the LastLangCodes list. Only match x-* and remove those elements.
    while((i < Size) && LangCodes[i] != '\0')
    {
        // Start j at the current offset into the LangCodes string
        j = i;

        // Go through the LangCodes and find the next terminator from the current
        //  offset. Each language code is separated by a ';'. The end of the string
        //  is a null terminator
        while((j < Size) && LangCodes[j] != ';' && LangCodes[j] != '\0')
            j++;

        // Check if the current language is of the form "x-"
        if(LangCodes[i] == 'x' && LangCodes[i+1] == '-')
        {
            // The current offset is of the form "x-", check if the following section is another
            //  language or if it is the end of the string
            if(LangCodes[j] == ';')
            {
                // its another language, copy, starting at the next section of the string, over
                //  the current language string
                gBS->CopyMem( &(LangCodes[i]), &(LangCodes[j+1]), Size-(j+1));
                Size = Size - (j-i-1);
                continue;
            }
            else if(LangCodes[j] == '\0')
            {
                // this language code was the last language code,
                if((i > 0) && LangCodes[i-1] == ';')
                {
                    LangCodes[i-1] = '\0';
                    Size = i-1;
                }
            }
        }
        i = j + 1;
    }
    Size = AsciiStrLen(LangCodes) + 1;
#endif
    //LangCodes is a volatile variable and needs to be initialized during every boot
    //However, this routine is not invoked during every boot,
    //It is always invoked during the first boot.
    //During subsequent boots it is only invoked when user is trying to enter Setup
    //In order to initialize LanCodes when this routine is not invoked, let's create
    //a non-volatile variable LastLangCodes with the copy of LanCodes.
    //This copy is used to initialize LanCodes when this routine is not invoked.
    //(This code is part of SetupEntry routine)
    gRT->SetVariable( LastLangCodes,
                      &AmiSetupGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      Size,
                      LangCodes );

    if(LangCodes != NULL)
        gBS->FreePool(LangCodes);
}

/**
 * Function that will publish all Hii resources. Normally this function is executed
 * on a callback when TSE notifies the system that the user has requested to enter
 * into setup, but there is a token ALWAYS_PUBLISH_HII_RESOURCES that will force
 * the system to always publish this information.
 *
 * @param Event Event that triggered this callback
 * @param Context Context for this callback
 */
VOID SetupNotificationFunction(IN EFI_EVENT Event, IN VOID *Context)
{
    static BOOLEAN ResourcesLoaded = FALSE;

    EFI_STATUS Status;

    UINTN i = 0;

    EFI_HII_STRING_PROTOCOL *HiiString = NULL;
    EFI_HII_DATABASE_PROTOCOL *HiiDatabase = NULL;

    EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList = NULL;

    EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;

    VENDOR_DEVICE_PATH FormSetDevicePathNode =
    {
        {
            MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP,
            sizeof(VENDOR_DEVICE_PATH)
        },
        {0}
    };

    // Ensure Resources have no been loaded
    if(!ResourcesLoaded)
    {
        EFI_HII_PACKAGE_HEADER *IfrPackagePtr = NULL;
        EFI_DEVICE_PATH_PROTOCOL *NewDp = NULL;

        // Locate the HiiString Protocol
        Status = gBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID**)&HiiString);
        if(EFI_ERROR(Status))
            return;

        // Locate the HiiDatabase Protocol
        Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&HiiDatabase);
        if(EFI_ERROR(Status))
            return;

        // Get the HiiPackageListProtocol from the Image's Handle
        Status = gBS->HandleProtocol(ThisImageHandle, &gEfiHiiPackageListProtocolGuid, (VOID**)&HiiPackageList);
        if(EFI_ERROR(Status))
            return;

        // Get the DevicPathProtocol from the Image's handle
        Status = gBS->HandleProtocol(ThisImageHandle, &gEfiLoadedImageDevicePathProtocolGuid, (VOID**)&Dp);
        if(EFI_ERROR(Status))
            return;

        // Go through the HiiPackageList data and find the FormSet Guid from this setup page then add it to
        //  the local variable FormSetDevicePathNode, and into the FormData global varibale
        for(IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList+1);
            IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList+HiiPackageList->PackageLength);
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr+IfrPackagePtr->Length))
        {
            if(IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS)
            {
                EFI_IFR_FORM_SET *FormSet = (EFI_IFR_FORM_SET*)(IfrPackagePtr+1);

                gBS->CopyMem(&(FormSetDevicePathNode.Guid), &(FormSet->Guid), sizeof(EFI_GUID));
                gBS->CopyMem(&(FormData.FormGuid), &(FormSet->Guid), sizeof(EFI_GUID));
                break;
            }
        }

        // Create a new device path by appending the FormSetDevicePathNode to to the loaded image device path
        NewDp = AppendDevicePathNode(Dp, (EFI_DEVICE_PATH_PROTOCOL*)&FormSetDevicePathNode);

        // Install the new device path protocol, and the HiiConfigAccessProtocol onto a new handle
        Status = gBS->InstallMultipleProtocolInterfaces(&(FormData.CallbackHandle),
                                                        &gEfiDevicePathProtocolGuid, NewDp,
                                                        &gEfiHiiConfigAccessProtocolGuid, &(FormData.HiiAccessProtocol),
                                                        NULL);
        if(EFI_ERROR(Status))
            return;

        // then use that handle to install the HiiPackageList into the HiiDatabase
        Status = HiiDatabase->NewPackageList(   HiiDatabase,
                                                HiiPackageList,
                                                FormData.CallbackHandle,
                                                &(FormData.HiiHandle));
        if(!EFI_ERROR(Status))
        {
            AMI_STRING_INITIALIZATION_ENTRY *NextLink = NULL;
            // Update the flag used to prevent duplicate submitting of the HiiPackages to the database
            ResourcesLoaded = TRUE;

            // If this code was executed as part of a callback, close the callback event
            if(Event)
                gBS->CloseEvent(Event);

            // Call the hook functions to allow other modules to register string functions, callbacks,
            //  and hiiaccess functions
            for(i = 0; SetupPostHiiInstallFunctionList[i] != NULL; i++)
                SetupPostHiiInstallFunctionList[i]();

            // Go through the legacy String Initialization functions to allow updating of the strings
            //  in the database
            for(i = 0; StringInitFunc[i]!=NULL; i++)
                StringInitFunc[i](FormData.HiiHandle, SETUP_FORM_SET_CLASS);

            // Go through the String Initialization functions registered using the SetupLibrary functions and
            //  allow them to update the strings in the database
            for(NextLink = (AMI_STRING_INITIALIZATION_ENTRY*)GetFirstNode(&AmiStringInitializationList);
                !IsNull(&AmiStringInitializationList, &NextLink->Link); 
                NextLink = (AMI_STRING_INITIALIZATION_ENTRY*)GetNextNode(&AmiStringInitializationList, &NextLink->Link)
            )  NextLink->Function(FormData.HiiHandle);

            // Get list of available languages and initialize Lang and LangCodes variables
            InitLanguages(HiiString, FormData.HiiHandle);
        }
    }
}

/**
 * Return the language variables stored in NVRAM
 *
 * @param VariableName Human readable name of the language variable
 * @param VariableGuid Pointer to the variable GUID
 * @param VariableSize Pointer to the size of the buffer that the output has been stored in
 * @return CHAR8 The buffer that contains the languages
 */
CHAR8* GetLangVariable(CHAR16 *VariableName, EFI_GUID *VariableGuid, UINTN *VariableSize)
{
    UINTN Size = 0;
    CHAR8 *Buffer = NULL;
    EFI_STATUS Status;

    Status = GetVariable2(VariableName, VariableGuid, (VOID**)&Buffer, &Size);
    if(EFI_ERROR(Status))
        Buffer = NULL;
    else if(VariableSize!=NULL)
        *VariableSize=Size;
    return Buffer;
}

/**
 * Store the passed L"PlatformLangCodes" into NVRAM
 *
 * @param VariableName Human-readable name of language variable
 * @param LangBuffer Pointer to list of language codes to store
 * @param LangBufferSize Size of the passed buffer
 * @return
 */
VOID SetLangCodes(CHAR16 *VariableName, CHAR8* LangBuffer, UINTN LangBufferSize)
{
    if(LangBuffer == NULL || LangBufferSize == 0)
        return;
    gRT->SetVariable(   VariableName,
                        &gEfiGlobalVariableGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        LangBufferSize,
                        LangBuffer
    );
}

/**
 * Helper routine that determines if the variable exists
 *
 * @param VariableName Name of the variable to check on
 * @param VendorGuid GUID of the variable to check on
 *
 * @return BOOLEAN
 * @retval TRUE The variable exists
 * @retval FALSE The variable does not exist
 */
BOOLEAN DoesVariableExist(IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid)
{
    UINTN Size=0;
    return(gRT->GetVariable(VariableName, VendorGuid, NULL, &Size, NULL) == EFI_BUFFER_TOO_SMALL);
}

/**
 * This function is the entry point for the Setup driver. It registers the the Setup
 * This callback functions and then it checks if the "Setup" and "Lang" variables are
 * defined. If not it is a first boot (first flash or first boot after BIOS upgrade)
 * and these variables will need to be defined. If "Setup" and "Lang" variables are
 * defined, then make sure the language variables all agree and then return
 *
 * @param ImageHandle Image handle
 * @param SystemTable Pointer to the UEFI System Table
 *
 * @return EFI_STATUS
 * @retval EFI_SUCCESS
 */
EFI_STATUS EFIAPI SetupEntry(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
#if !defined(ALWAYS_PUBLISH_HII_RESOURCES)||(ALWAYS_PUBLISH_HII_RESOURCES==0)
    static EFI_EVENT SetupEnterEvent;
#endif

#if defined(FORCE_USER_TO_SETUP_ON_FIRST_BOOT)&&(FORCE_USER_TO_SETUP_ON_FIRST_BOOT)
    static UINT32 BootFlow = BOOT_FLOW_CONDITION_FIRST_BOOT;
    static EFI_GUID guidBootFlow = BOOT_FLOW_VARIABLE_GUID;
#endif

	CHAR8 *LangCodesBuffer;
	UINTN LangCodesSize;
// Supermicro +
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL* SmcSetupModifyProtocol;
// Supermicro -

    ThisImageHandle = ImageHandle;

    // AmiCompatibilityPkg - Future Removal
    InitAmiLib(ImageHandle,SystemTable);
    InitializeListHead(&AmiCallbackList);
    InitializeListHead(&AmiStringInitializationList);
    InitializeListHead(&AmiExtractConfigList);
    InitializeListHead(&AmiRouteConfigList);

    if(!DoesVariableExist(LastLangCodes, &AmiSetupGuid))
    {   //If LastLangCodes Variable is not found, this is first boot after FW upgrade.
        // We have to submit resources to HII to get Setup defaults and list of supported languages.

        // Submit resources to HII and set PlatformLangCodes
        SetupNotificationFunction(NULL, NULL);

        #if defined(FORCE_USER_TO_SETUP_ON_FIRST_BOOT)&&(FORCE_USER_TO_SETUP_ON_FIRST_BOOT)
        // If the token is set, set the L"BootFlow" variable to force entry into setup
        if(!DoesVariableExist(L"BootFlow", &guidBootFlow))
            gRT->SetVariable(L"BootFlow", &guidBootFlow, EFI_VARIABLE_BOOTSERVICE_ACCESS, sizeof(BootFlow), &BootFlow);
        #endif
    }
    else
    {
        // Register setup callbacks to submit resources to HII only if/when setup is launched
        #if defined(ALWAYS_PUBLISH_HII_RESOURCES)&&(ALWAYS_PUBLISH_HII_RESOURCES)
        SetupNotificationFunction(NULL, NULL);
        #else
        VOID *pSetupRegistration;
        RegisterProtocolCallback(&gAmiTseSetupEnterGuid, SetupNotificationFunction, NULL, &SetupEnterEvent, &pSetupRegistration);
        #endif
    }
    //Set PlatformLanCodes variable
    LangCodesBuffer = GetLangVariable(LastLangCodes, &AmiSetupGuid, &LangCodesSize);
    SetLangCodes(L"PlatformLangCodes", LangCodesBuffer, LangCodesSize);
    gBS->FreePool(LangCodesBuffer);

// Supermicro +
    //
    // Update HII database by SMC's protocol.
    // The action must do at all HPK list being produced.
    //
    Status = gBS->LocateProtocol(&gSetupModifyProtocolGuid , NULL, &SmcSetupModifyProtocol);
    if (!EFI_ERROR (Status)) {
        SmcSetupModifyProtocol->Update ();
    }
// Supermicro -

    return EFI_SUCCESS;
}

/**
 * Wrapper function used to call the HiiAccessExtractConfig. Will be expanded in 
 * in the future
 * 
 * @param This Pointer to the EFI_HII_CONFIG_ACCESS_PROTOCOL
 * @param Request Pointer to the string buffer that contains the request for information
 * @param Progress Pointer that contains the progress of parsing the Request string
 * @param Results Pointer to a newly created string that contains the requested information
 *
 * @return EFI_STATUS
 * @retval EFI_INVALID_PARAMETER A parameter was not valid, or an error occurred while processing the request
 * @retval EFI_SUCCESS The Results pointer contains the requested information
 */
EFI_STATUS EFIAPI HiiAccessExtractConfigWrapper(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Request,
    OUT EFI_STRING *Progress,
    OUT EFI_STRING *Results
)
{
    EFI_STATUS Status;
    Status = HiiAccessExtractConfig(This, Request, Progress, Results);
    return Status;
}

/**
 * Wrapper function used to call the HiiAccessRouteConfig. Will be expanded in 
 * in the future
 * 
 * @param This Pointer to the EFI_HII_CONFIG_ACCESS_PROTOCOL
 * @param Configuration Pointer to the Null terminated string that represents the configuration request
 * @param Progress A pointer to the progress of parsing the Configuration request
 *
 * @return EFI_STATUS
 * @retval EFI_INVALID_PARAMETER A parameter was not valid, or an error occurred while processing the request
 * @retval EFI_SUCCESS The Results pointer contains the requested information
 */
EFI_STATUS EFIAPI HiiAccessRouteConfigWrapper(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Configuration,
    OUT EFI_STRING *Progress
)
{
    EFI_STATUS Status;
    Status = HiiAccessRouteConfig(This, Configuration, Progress);
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    CallbackFunction
//
// Description:
//  This function is used to identify the function to call when an interactive
//  item has been triggered in the setup browser based on the information in
//  the Callback protocol and the SetupCallBack Array
//
// Input:
//  IN EFI_FORM_CALLBACK_PROTOCOL *This - Pointer to the instance of the callback
//                                        protocol
//  IN UINT16 KeyValue - Unique value that defines the type of data to expect in
//                       the Data parameter
//  IN EFI_IFR_DATA_ARRAY *Data - Data defined by KeyValue Parameter
//  OUT EFI_HII_CALLBACK_PACKET **Packet - Data passed from the Callback back to
//                                         the setup Browser
//
// Output:
//  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
/**
 * This function is used to identify the function to call when an interactive item has 
 * been triggered in the setup browser based on the information in the Callback protocol 
 * and the SetupCallBack Array
 * 
 * @param This Pointer to the instance of the callback protocol
 * @param Action The that caused the callback function to be called
 * @param KeyValue Unique value that defines the type of data to expect in the Data parameter
 * @param Type The type of value for the question
 * @param Value A pointer to the data being sent to the original exporting driver. The Type is specified by type.
 * @param ActionRequest On return, points to the action requested by the callback function. 
 * 
 * @return EFI_STATUS
 * @retval EFI_DEVICE_ERROR The variable could not be saved
 * @retval EFI_UNSUPPORTED The specified action is not supported by the callback
 * @retval EFI_OUT_OF_RESOURCES not enough storage is available to hold the variable and its data
 * @retval EFI_SUCCESS The callback successfully handled the action
 */
EFI_STATUS EFIAPI CallbackFunction(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
)
{
    EFI_STATUS Status;
    UINTN i = 0;

    AMI_CALLBACK_LIST_ENTRY *NextLink = NULL;

    AMI_HII_CALLBACK_PARAMETERS AmiCallbackParameters =
    {
        This,
        Action,
        KeyValue,
        Type,
        Value,
        ActionRequest,
        FormData.HiiHandle
    };

    if(ActionRequest)
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

    Status = EFI_UNSUPPORTED;
    CallbackParametersPtr = (CALLBACK_PARAMETERS*)&AmiCallbackParameters;

    // Perform callbacks for built in items
    for(i = 0; SetupItemCallback[i].UpdateItem != NULL; i++)
    {
        if(SetupItemCallback[i].Key == KeyValue)
        {
            Status = SetupItemCallback[i].UpdateItem(FormData.HiiHandle, SETUP_FORM_SET_CLASS, 0, KeyValue);
            if(Status != EFI_UNSUPPORTED)
                break;
        }
    }

    // Perform callbacks for items registered through the Setup Library hooks
    for(NextLink = (AMI_CALLBACK_LIST_ENTRY*)GetFirstNode(&AmiCallbackList);
        !IsNull(&AmiCallbackList, &NextLink->Link); 
        NextLink = (AMI_CALLBACK_LIST_ENTRY*)GetNextNode(&AmiCallbackList, &NextLink->Link)
    ){
        if((NextLink->QuestionId == KeyValue))
        {
            Status = NextLink->Function(&AmiCallbackParameters);
            if(Status != EFI_UNSUPPORTED)
                break;
        }
    }

    CallbackParametersPtr = NULL;
    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
