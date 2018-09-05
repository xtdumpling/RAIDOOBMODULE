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

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: HiiUtilities.c
//
// Description: 
//  Contains generic EFI library functions for HII access.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiDxeLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiPackageList.h>
#include <Protocol/HiiString.h>

typedef struct _NAME_PATTERN
{
    UINTN Length;
    CHAR16 *Tag;
} NAME_PATTERN;

typedef struct _PACKAGE_LIST_RECORD {
    DLINK		 Link;
	UINT32		 Signature;
	EFI_GUID	 Guid;
    EFI_HANDLE	 DriverHandle;
	DLIST		 PackageList;
	DLIST		 FontList;
	DLIST		 LanguageList;
	DBE_DATABASE StringDb;
} PACKAGE_LIST_RECORD;

typedef struct _PACKAGE_LINK {
	DLINK					Link;
	EFI_HII_PACKAGE_HEADER  Package;
} PACKAGE_LINK;

extern const char *DefaultLanguageCode;

const NAME_PATTERN GuidPattern   = { 10, L"GUID=" };
const NAME_PATTERN NamePattern   = { 10, L"NAME=" };
const NAME_PATTERN PathPattern   = { 10, L"PATH=" };
const NAME_PATTERN AltCfgPattern = { 14, L"ALTCFG=" };
const NAME_PATTERN OffsetPattern = { 14, L"OFFSET=" };
const NAME_PATTERN WidthPattern  = { 12, L"WIDTH=" };
const NAME_PATTERN ValuePattern  = { 12, L"VALUE=" };

// ===========================
// In Aptio4, these are externals defined in the AmiProtocolLib.
// In AptioV, there is no AmiProtocolLib.
// We could've listed these in AmiDxeLib.inf, but may break the
// build of some of the Aptio4 modules due to multiple defined symbols.
static EFI_GUID gLocalHiiUtilitiesProtocolGuid = HII_UTILITIES_PROTOCOL_GUID;
static EFI_GUID gLocalEfiHiiConfigAccessProtocolGuid = EFI_HII_CONFIG_ACCESS_PROTOCOL_GUID;
static EFI_GUID gLocalEfiHiiConfigRoutingProtocolGuid = EFI_HII_CONFIG_ROUTING_PROTOCOL_GUID;
static EFI_GUID gLocalEfiDevicePathProtocolGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;
static EFI_GUID gLocalEfiHiiStringProtocolGuid = EFI_HII_STRING_PROTOCOL_GUID;
static EFI_GUID gLocalEfiFormBrowser2ProtocolGuid = EFI_FORM_BROWSER2_PROTOCOL_GUID;
static EFI_GUID gLocalEfiHiiPackageListProtocolGuid = EFI_HII_PACKAGE_LIST_PROTOCOL_GUID;
static EFI_GUID gLocalEfiHiiDatabaseProtocolGuid = EFI_HII_DATABASE_PROTOCOL_GUID;

static EFI_HII_DATABASE_PROTOCOL *HiiDatabase=NULL;
static EFI_HII_STRING_PROTOCOL *HiiString=NULL;
static EFI_FORM_BROWSER2_PROTOCOL  *FormBrowser = NULL;
static EFI_HII_CONFIG_ROUTING_PROTOCOL *HiiConfigRouting = NULL;
static CHAR16 DummyConfigHeader[] = L"GUID=00000000000000000000000000000000&NAME=0000&PATH=00&";

// *************************** Service functions (not for public use) **********************
CHAR8 *HiiLibGetPlatformLanguageInternal(BOOLEAN Refresh){
	static EFI_GUID EfiVariableGuid = EFI_GLOBAL_VARIABLE;
	static CHAR8 *PlatformLang = NULL;	
    
    if (PlatformLang==NULL || Refresh){
    	UINTN Size = 0;
    	if (PlatformLang==(CHAR8*)DefaultLanguageCode) PlatformLang = NULL;
        if (EFI_ERROR(GetEfiVariable(
            L"PlatformLang", &EfiVariableGuid, NULL, &Size, &PlatformLang
        ))) PlatformLang = (CHAR8*)DefaultLanguageCode;
    }    
    return PlatformLang;
}

BOOLEAN CheckIfHexDigit(CHAR16 Char)
{
    if(Char < 0x30 || Char > 0x66)
        return FALSE;

    if(Char > 0x39 && Char < 0x41)
        return FALSE;

    if(Char > 0x46 && Char < 0x61)
        return FALSE;

    return TRUE;
}

UINT8 HexValue(CHAR8 Char)
{
    if(Char >= 0x30 && Char <= 0x39)        //symbols 0...9
        return Char - 0x30;

    if(Char >= 0x41 && Char <= 0x46)        //symbols A...F
        return Char - 0x37;

    return Char - 0x57;                     //symbols a...f
}

static BOOLEAN CheckPattern(CHAR16 *String, NAME_PATTERN *Pattern)
{
    if(MemCmp(String, Pattern->Tag, Pattern->Length))
        return FALSE;
    else
        return TRUE;
}

VOID StringToChar16(
    IN CHAR16 *String,
    IN OUT UINTN  *Size,
    OUT CHAR16 *Block
)
{
    UINTN i, j;

    i = 0;
    j = 0;

    while( CheckIfHexDigit(String[i]) && CheckIfHexDigit(String[i + 1]) && 
           CheckIfHexDigit(String[i + 2]) && CheckIfHexDigit(String[i + 3]) &&
           j < *Size - 1 )
    {
        Block[j]  = HexValue((UINT8)String[i]) & 0x000f;
        Block[j] <<= 4;
        Block[j] |= HexValue((UINT8)String[i + 1]);
        Block[j] <<= 4;
        Block[j] |= HexValue((UINT8)String[i + 2]);
        Block[j] <<= 4;
        Block[j] |= HexValue((UINT8)String[i + 3]);

        i += 4;
        j++;
    }
    *Size = j;      //number of words processed
    Block[j] = 0;   //put null-terminator
}

static UINT32 StrSize16(CHAR16 *String)
{
	UINT32 Size = 2;
	while(*String++)
		Size += 2;
	return Size;
}

VOID StringToBlock(
    IN CHAR16 *String,
    IN UINTN StringSize,
    OUT UINT8 *Block)
{
    UINT8 LowNibble, HiNibble;
    UINTN i, j;

    j = 0;

    for(i = 0; i < StringSize; i += 2)
    {
        if(i == StringSize - 1) {   //odd number need to fill with zeroes
            HiNibble = 0;
            LowNibble = HexValue((UINT8)String[i]);
        } else {
            HiNibble = HexValue((UINT8)String[i]);
            LowNibble = HexValue((UINT8)String[i + 1]);
        }
        Block[j] = (HiNibble << 4) | LowNibble;
        j++;
    }
}

static EFI_STATUS ExtractDevicePath(CHAR16 *ConfigString, UINT8 **DevicePath, UINTN *DpLength)
{
    EFI_STATUS Status;
    UINTN Length = 0;
    UINTN i = 5;        //set pointer to skip first four symbols "PATH="

    while(CheckIfHexDigit(ConfigString[i]))      //determine device path length in characters
    {
        Length++;
        i++;
    }

    Status = pBS->AllocatePool(
                            EfiBootServicesData, 
        					(Length / 2), 
					        DevicePath);
	if (EFI_ERROR(Status))
		return Status;

    StringToBlock(&ConfigString[5], Length, *DevicePath);
    *DpLength = Length / 2;

    return EFI_SUCCESS;
}

static BOOLEAN CheckDevicePath(
    IN UINT8 *DevicePath,
    IN UINTN DpLength)
{
    EFI_DEVICE_PATH_PROTOCOL* TempDp;

    TempDp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
    
    if (DevicePath == NULL)
        return FALSE;
        
    // Valid device path must have at least one node
    if (DpLength < 4)
        return FALSE;
    
    // Valid device path protocol cannot be longer than binary data
    if (*(UINT16*)TempDp->Length > DpLength)
        return FALSE;
        
    // Valide device path types
    if (TempDp->Type != HARDWARE_DEVICE_PATH    &&
        TempDp->Type != ACPI_DEVICE_PATH        &&
        TempDp->Type != MESSAGING_DEVICE_PATH   &&
        TempDp->Type != MEDIA_DEVICE_PATH       &&
        TempDp->Type != BBS_DEVICE_PATH         &&
        TempDp->Type != END_DEVICE_PATH_TYPE)
    {
        return FALSE;
    }

    return TRUE;
}

static EFI_STATUS FindDeviceHandle(
    IN CHAR16 *ConfigString,
    OUT EFI_HANDLE *Handle)
{
    UINTN i = 0;
    UINTN DpLength;
    UINT8 *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL* TempDp;
    EFI_STATUS Status;

//scan config string to find PATH pattern
    do
    {
        while((ConfigString[i] != L'&') && (ConfigString[i] != 0))
            i++;

        if(ConfigString[i] == 0)
            return EFI_NOT_FOUND;

        i++;            //shift pointer to next symbol
        if(CheckPattern(&ConfigString[i], &PathPattern))
            break;
    } while(1);

//if we get here, we have pointer set on PATH pattern
    Status = ExtractDevicePath(&ConfigString[i], &DevicePath, &DpLength);
    if(EFI_ERROR(Status))
        return Status;
        
    if (CheckDevicePath(DevicePath, DpLength)) {
        TempDp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
        Status = pBS->LocateDevicePath (
                          &gLocalEfiDevicePathProtocolGuid,
                          (EFI_DEVICE_PATH_PROTOCOL**)&TempDp,
                          Handle
                          );
    } else {
        Status = EFI_NOT_FOUND;
    }
    
    pBS->FreePool(DevicePath);

    return Status;
}

static VOID StripValuePattern(
    IN OUT EFI_STRING String,
    IN UINTN          Size
)
{
    EFI_STATUS Status;
    EFI_STRING NewString;
    UINTN i = 0;
    UINTN j = 0;

    Status = pBS->AllocatePool(EfiBootServicesData, Size, &NewString);

    while(String[i] != 0) {
        if(String[i] == L'&') {
            if(CheckPattern(&String[i + 1], &ValuePattern)) {
                i++;
                while((String[i] != L'&') && (String[i] != 0))
                    i++;
                if(String[i] == 0)
                    break;
            }
        }

        NewString[j++] = String[i++];
    }

    NewString[j] = 0;
    MemCpy(String, NewString, (j + 1) * 2);
    pBS->FreePool(NewString);
}
//*************************************************************************

static EFI_STATUS HiiAccessExtractConfig(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Request,
    OUT EFI_STRING *Progress,
    OUT EFI_STRING *Results
)
{
    EFI_STATUS  Status;
    UINTN       Length;
    EFI_GUID    Guid;
    CHAR16      Name[80];
    CHAR16      *Pointer = Request;
    UINT8       *Var = NULL;
    EFI_HII_CONFIG_ROUTING_PROTOCOL *ConfigRouting;
    CHAR16      *ErrorMarker;

    if(Request == NULL) {
        *Progress = NULL;
        return EFI_INVALID_PARAMETER;
    }

    Status = pBS->LocateProtocol(&gLocalEfiHiiConfigRoutingProtocolGuid, NULL, &ConfigRouting);
    if(EFI_ERROR(Status))
        return Status;

//check syntax for guid
    if(!CheckPattern(Request, &GuidPattern)) {
        *Progress = Request;
        return EFI_INVALID_PARAMETER;
    }
//extract guid
    ErrorMarker = Pointer;
    Pointer += 5;       //skip "GUID=" pattern
    Length = 32;        //length of guid in symbols
    StringToBlock(Pointer, Length, (UINT8 *)&Guid);
    Pointer += Length;
    if(*Pointer != L'&') {
        TRACE((DEBUG_ERROR, "HII Extract Config: Guid extraction failed\n"));
        *Progress = ErrorMarker;
        return EFI_INVALID_PARAMETER;
    }

//check syntax for name
    if(!CheckPattern(Pointer + 1, &NamePattern)) {
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }
//extract name
    ErrorMarker = Pointer;
    Pointer += 6;       //skip '&NAME=' pattern
    Length = 80;
    StringToChar16(Pointer, &Length, Name);
    Pointer += (Length * 4);
    if(*Pointer != L'&') {
        TRACE((DEBUG_ERROR, "HII Extract Config: Name extraction failed\n"));
        *Progress = ErrorMarker;
        return EFI_INVALID_PARAMETER;
    }

//get variable by name and guid from nvram

    Length = 0;
    Status = GetEfiVariable(Name, &Guid, NULL, &Length, &Var);
    if(EFI_ERROR(Status)) {
        *Progress = Request;
        return Status;
    }

//find start of OFFSET/WIDTH/VALUE block
    Pointer++;      //skip '&'
    while((*Pointer != L'&') && (*Pointer != 0))    //skip PATH
            Pointer++;

    if(*Pointer == 0) {                     //OFFSET/WIDTH blocks are missed
        UINTN TempSize; 
        CHAR16 *TempString;
        CHAR16 *TempString2;

        TempSize = (StrLen(L"&OFFSET=0&WIDTH=") + sizeof(Length) + 1) * sizeof(CHAR16);
        TempString = MallocZ(TempSize);
        if(TempString == NULL)
            return EFI_OUT_OF_RESOURCES;

        Swprintf_s(TempString, TempSize, L"&OFFSET=0&WIDTH=%x", Length);

        TempSize += StrSize(Request);
        TempString2 = MallocZ(TempSize);
        if(TempString2 == NULL)
            return EFI_OUT_OF_RESOURCES;

        StrCat(TempString2, Request);
        StrCat(TempString2, TempString);

        Status = ConfigRouting->BlockToConfig(ConfigRouting, TempString2, Var, Length, Results, Progress);
        *Progress = Request + StrLen(Request); //set Progress to end of ConfigHdr
        pBS->FreePool(TempString);
        pBS->FreePool(TempString2);
    } else {                                        //OFFSET/WIDTH block found, let's check if it's correct
        if(!CheckPattern(Pointer + 1, &OffsetPattern)) {
            *Progress = Pointer;
            return EFI_INVALID_PARAMETER;
        }
        Status = ConfigRouting->BlockToConfig(ConfigRouting, Request, Var, Length, Results, Progress);
    }
        
    pBS->FreePool(Var);

    return Status;
}

static EFI_STATUS HiiAccessRouteConfig(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN CONST EFI_STRING Configuration,
    OUT EFI_STRING *Progress
)
{
    EFI_STATUS  Status;
    UINTN       Length;
    UINTN       VarSize;
    EFI_GUID    Guid;
    CHAR16      Name[80];
    CHAR16      *Pointer = Configuration;
    UINT8       *Var = NULL;
    EFI_HII_CONFIG_ROUTING_PROTOCOL *ConfigRouting;
    EFI_HANDLE  Handle;
    UINT32      Attributes;

    if(Configuration == NULL)
    {
        *Progress = NULL;
        return EFI_INVALID_PARAMETER;
    }

    Status = pBS->LocateProtocol(&gLocalEfiHiiConfigRoutingProtocolGuid, NULL, &ConfigRouting);
    if(EFI_ERROR(Status))
        return Status;

//check syntax for guid
    if(!CheckPattern(Configuration, &GuidPattern))
    {
        *Progress = Configuration;
        return EFI_INVALID_PARAMETER;
    }

//check proper device path
    Status = FindDeviceHandle(Configuration, &Handle);
    if(EFI_ERROR(Status))
        return EFI_NOT_FOUND;

//extract guid
    Pointer += 5;       //skip "GUID=" pattern
    Length = 32;        //length of guid in bytes
    StringToBlock(Pointer, Length, (UINT8 *)&Guid);
    Pointer += Length;
    if(*Pointer != L'&')
    {
        TRACE((DEBUG_ERROR, "HII Route Config: Guid extraction failed\n"));
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }

//check syntax for name
    if(!CheckPattern(Pointer + 1, &NamePattern))
    {
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }
//extract name
    Pointer += 6;       //skip '&NAME=' pattern
    Length = 80;
    StringToChar16(Pointer, &Length, Name);
    Pointer += (Length * 4);
    if(*Pointer != L'&')
    {
        TRACE((DEBUG_ERROR, "HII Route Config: Name extraction failed\n"));
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }

//get variable by name and guid from nvram

    Length = 0;
    Status = GetEfiVariable(Name, &Guid, &Attributes, &Length, &Var);
    if(EFI_ERROR(Status))   //requested variable not found
    {
        if(Var != NULL)
            pBS->FreePool(Var);
        
        Var = NULL;
        Length = 0;
// APTIOV_SERVER_OVERRIDE_START: Security fix
#if (defined(RT_ACCESS_SUPPORT_IN_HPKTOOL) && (RT_ACCESS_SUPPORT_IN_HPKTOOL == 1))
        {
            Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
        }
#else
        {
            Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
        }
#endif
// APTIOV_SERVER_OVERRIDE_END: Security fix
    }
    VarSize = Length;

    Status = ConfigRouting->ConfigToBlock(ConfigRouting, Configuration, Var, &Length, Progress);
    while(Status == EFI_DEVICE_ERROR || (Status == EFI_INVALID_PARAMETER && Var == NULL)) //block not large enough
    {
        if(Var != NULL)
            pBS->FreePool(Var);

        Status = pBS->AllocatePool(EfiBootServicesData, Length, &Var);
        if(EFI_ERROR(Status))
            return Status;

        VarSize = Length;
        Status = ConfigRouting->ConfigToBlock(ConfigRouting, Configuration, Var, &Length, Progress);
    }

    if(EFI_ERROR(Status))
        return Status;

    pRS->SetVariable(Name, &Guid, Attributes, VarSize, Var);

    pBS->FreePool(Var);

    return EFI_SUCCESS;
}

static EFI_STATUS HiiAccessFormCallback(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID QuestionId,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
)
{
    return EFI_UNSUPPORTED;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: SetCallback
//
// Description:
//  EFI_STATUS SetCallback(EFI_HII_IFR_PACK *pIfr,
// UINTN NumberOfCallbacks, CALLBACK_INFO *CallbackInfo,
// CALLBACK_INFO **CallBackFound)
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS SetCallback(
    EFI_LOADED_IMAGE_PROTOCOL *Image,
	EFI_IFR_FORM_SET *FormSet, UINTN NumberOfCallbacks,
	CALLBACK_INFO *CallbackInfo, CALLBACK_INFO **CallBackFound,
    EFI_HANDLE *CallbackHandle 
)
{
	UINTN i;
	EFI_STATUS Status;
    EFI_HANDLE Handle=NULL;
    EFI_DEVICE_PATH_PROTOCOL *pPath, *pPath2;
    CALLBACK_INFO *CallbackInfoPtr;

    static EFI_HII_CONFIG_ACCESS_PROTOCOL DefaultConfigAccess = {
        HiiAccessExtractConfig, HiiAccessRouteConfig, HiiAccessFormCallback
    };
    static CALLBACK_INFO DefaultCallbackInfo = {
	    NULL,&DefaultConfigAccess, 0, 0, NULL
    };
    static VENDOR_DEVICE_PATH FormSetDevicePathNode = {
        {
            MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP,
            sizeof(VENDOR_DEVICE_PATH)
        },
        {0}
    };

	for(  i=0
        ;    i<NumberOfCallbacks
          && (      CallbackInfo[i].pGuid == NULL
                ||  guidcmp(&FormSet->Guid,CallbackInfo[i].pGuid) != 0
             )
        ; i++
    );
    if (i==NumberOfCallbacks){
        CallbackInfoPtr = &DefaultCallbackInfo;
    }else{
        CallbackInfoPtr = &CallbackInfo[i];
        if (CallbackInfoPtr->pFormCallback == NULL){
            CallbackInfoPtr->pFormCallback = &DefaultConfigAccess;  
        }else{
            if (CallbackInfoPtr->pFormCallback->ExtractConfig==NULL)
                CallbackInfoPtr->pFormCallback->ExtractConfig = 
                    HiiAccessExtractConfig;
            if (CallbackInfoPtr->pFormCallback->RouteConfig==NULL)
                CallbackInfoPtr->pFormCallback->RouteConfig = 
                    HiiAccessRouteConfig;
            if (CallbackInfoPtr->pFormCallback->Callback==NULL)
                CallbackInfoPtr->pFormCallback->Callback = 
                    HiiAccessFormCallback;
        }
    }
            Status=pBS->HandleProtocol(Image->DeviceHandle, &guidDevicePath, &pPath);
            if (EFI_ERROR(Status)) pPath=NULL;
            pPath = DPAddNode(pPath, Image->FilePath);
            FormSetDevicePathNode.Guid = FormSet->Guid;
            pPath2 = DPAddNode(pPath, &FormSetDevicePathNode.Header);
            pBS->FreePool(pPath);
            Status = pBS->InstallMultipleProtocolInterfaces(
                &Handle, 
                &guidDevicePath, pPath2,
                &gLocalEfiHiiConfigAccessProtocolGuid, CallbackInfoPtr->pFormCallback,
                NULL
            );
			if (EFI_ERROR(Status)) return Status;
    if (CallbackHandle) *CallbackHandle=Handle;
    if (CallBackFound){
        *CallBackFound=  (CallbackInfoPtr == &DefaultCallbackInfo) 
                       ? NULL 
                       : CallbackInfoPtr;
    }
	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: HiiLibPublishPackages
//
// Description: Publishes submitted array of HII packages into HII database.
//
// Input:
//  IN VOID *PackagePointers - array of pointers to the HII packages
//  IN UINTN NumberOfPackages - number of pointers in the PackagePointers array
//  IN EFI_GUID *PackageGuid - package list GUID
//  IN EFI_HANDLE DriverHandle OPTIONAL - Handle to associate with the package list
//  OUT EFI_HII_HANDLE *HiiHandle - HII handle of the package list
//
// Output:
//  EFI_STATUS
//            
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS HiiLibPublishPackages(
    IN VOID *PackagePointers, IN UINTN NumberOfPackages, 
    IN EFI_GUID *PackageGuid, IN EFI_HANDLE DriverHandle OPTIONAL,
    OUT EFI_HII_HANDLE *HiiHandle
){
	EFI_STATUS Status;
    EFI_HII_PACKAGE_LIST_HEADER *PackageList;
    UINTN i;
    UINT32 PackageLength; 
    UINT8 *PackagePtr;
    EFI_HII_PACKAGE_HEADER **Packages = PackagePointers;
    if (NumberOfPackages==0) return EFI_SUCCESS;
    if (   !HiiDatabase 
        && EFI_ERROR(Status=pBS->LocateProtocol(
                        &gLocalEfiHiiDatabaseProtocolGuid, NULL, &HiiDatabase
           ))
    ) return Status;
    //calculate the package list length
    PackageLength =   sizeof(EFI_HII_PACKAGE_LIST_HEADER) 
                    + sizeof(EFI_HII_PACKAGE_HEADER); //package list terminator
    for(i=0; i<NumberOfPackages; i++){
        PackageLength += Packages[i]->Length;
    }
    PackageList = Malloc(PackageLength);
    if(PackageList==NULL) return EFI_OUT_OF_RESOURCES;
    PackageList->PackageListGuid = *PackageGuid;
    PackageList->PackageLength = PackageLength;

    PackagePtr = (UINT8*)(PackageList+1);
    for(  i=0; i<NumberOfPackages; i++){
        pBS->CopyMem(PackagePtr, Packages[i], Packages[i]->Length);
        PackagePtr += Packages[i]->Length;
    }
    ((EFI_HII_PACKAGE_HEADER *)PackagePtr)->Length = sizeof(EFI_HII_PACKAGE_HEADER);
    ((EFI_HII_PACKAGE_HEADER *)PackagePtr)->Type = EFI_HII_PACKAGE_END;

    Status = HiiDatabase->NewPackageList(
        HiiDatabase, PackageList, DriverHandle, HiiHandle
    );
    pBS->FreePool(PackageList);
    return Status;
}

/**
  Returns a pointer to an allocated buffer that contains the best matching language 
  from a set of supported languages.  
  
  This function works with RFC 4646 language codes. The language 
  code returned is allocated using AllocatePool().  The caller is responsible for 
  freeing the allocated buffer using FreePool().  This function supports a variable
  argument list that allows the caller to pass in a prioritized list of language 
  codes to test against all the language codes in SupportedLanguages 

  @param[in]  SupportedLanguages  A pointer to a Null-terminated ASCII string that
                                  contains a set of semicolon separated RFC 4646 language codes.
  @param[in]  ...                 A variable argument list that contains pointers to 
                                  Null-terminated ASCII strings that contain one or more
                                  RFC 4646 language codes.
                                  The first language code from each of these language
                                  code lists is used to determine if it is an exact or
                                  close match to any of the language codes in 
                                  SupportedLanguages.  The matching algorithm from RFC 4647
                                  is used to determine if a close match is present.  If 
                                  an exact or close match is found, then the matching
                                  language code from SupportedLanguages is returned.  If
                                  no matches are found, then the next variable argument
                                  parameter is evaluated.  The variable argument list 
                                  is terminated by a NULL.

  @retval NULL   The best matching language could not be found in SupportedLanguages.
  @retval NULL   There are not enough resources available to return the best matching 
                 language.
  @retval Other  A pointer to a Null-terminated ASCII string that is the best matching 
                 language in SupportedLanguages.
**/
static CHAR8* FindBestLanguage (CHAR8 *SupportedLanguages,  ... ) {
#define IsEndOfLanguage(Lang) (*(Lang) == 0 || *(Lang) == ';')
    VA_LIST Args;
    CHAR8 *Language, *BestMatchStart, *BestMatchEnd, *Supported, *Lang;
    UINTN Length;
	
	if (SupportedLanguages == NULL) return NULL;
    VA_START (Args, SupportedLanguages);
    while ((Language = VA_ARG (Args, CHAR8 *)) != NULL) {
		Supported = SupportedLanguages;
		BestMatchStart = NULL;
		do{
			BOOLEAN PartialMatch = FALSE;
			CHAR8 *SupportedStart, *BestMatch;			
			BestMatch = Language;
			SupportedStart = Supported;
			for(  Lang = Language
				; !IsEndOfLanguage(Lang)
				; Lang++, Supported++
			){
				if (*Lang =='-') PartialMatch = TRUE;
				if  (*Lang != *Supported || IsEndOfLanguage(Supported)) break;
			}
			while(!IsEndOfLanguage(Supported)) Supported++;
			if ((PartialMatch || IsEndOfLanguage(Lang))&& BestMatch-Language < Lang-Language){
				BestMatch = Lang;
				BestMatchStart = SupportedStart;
				BestMatchEnd = Supported;
			}
			if (*Supported==';') Supported++;
		}while (*Supported != 0 && !IsEndOfLanguage(Lang));
		if (BestMatchStart != NULL) break;
	}
    VA_END (Args);
	// No matches were found 
	if (BestMatchStart == NULL) return NULL;
	Length = BestMatchEnd - BestMatchStart;
    Language = Malloc (Length + 1);
    if (Language == NULL)  return NULL;
    MemCpy (Language, BestMatchStart, Length);
	Language[Length]=0;
    return Language;
}

static CHAR8* GetSupportedLanguages (IN EFI_HII_HANDLE   HiiHandle) {
    EFI_STATUS  Status;
    CHAR8 *SupportedLanguages = NULL;
	UINTN Size = 0;
    
    // Retrieve list of supported languages
	if (HiiHandle == NULL) return NULL;
    if (   !HiiString
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiHiiStringProtocolGuid, NULL, &HiiString))
    ) return NULL;
	
    Status = HiiString->GetLanguages(HiiString, HiiHandle, SupportedLanguages, &Size);
    if (Status != EFI_BUFFER_TOO_SMALL) return NULL;
    SupportedLanguages = Malloc(Size);
    Status = HiiString->GetLanguages(HiiString, HiiHandle, SupportedLanguages, &Size);
	if (EFI_ERROR(Status)){
		pBS->FreePool(SupportedLanguages);
		return NULL;
	}
	return SupportedLanguages;
}

static EFI_STATUS GetAnyLanguageString(
	CHAR8 *SupportedLanguages, EFI_HII_STRING_PROTOCOL *HiiString,
	IN EFI_HII_HANDLE HiiHandle, IN STRING_REF StringId, 
	IN OUT UINTN *StringSize, OUT EFI_STRING String
){
	EFI_STATUS Status;
	CHAR8 *LangStart, *LangEnd;
	//Find string representation by trying all the supported languages
	
	//Iterate over languages in the SupportedLanguages list
	for(LangStart = SupportedLanguages; *LangStart!=0; ){
		CHAR8 Char;
		//Find the end of a language code
		for(LangEnd = LangStart; *LangEnd!=';'&& *LangEnd!=0; LangEnd++);
		//Terminate current language with 0 to call GetString
		Char = *LangEnd; // save the original char
		*LangEnd = 0;
		Status = HiiString->GetString(
			HiiString, LangStart, HiiHandle,
			StringId, String, StringSize, NULL
		);
		if (Status!=EFI_INVALID_LANGUAGE) break;
		if (Char==0) break;
		LangStart = LangEnd + 1; // continue with the next language
	}
	return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: HiiLibGetString
//
// Description: Reads string from the HII database in current language.
//
// Input:
//  IN EFI_HII_HANDLE HiiHandle - Package list handle
//  IN EFI_STRING_ID StringId - ID of the string to read
//  IN OUT UINTN *StringSize - On entry, points to the size of the buffer pointed to by String, 
//      in bytes. On return, points to the length of the string, in bytes.
//  OUT EFI_STRING String - Points to the output null-terminated string.
//
// Output:
//   EFI_STATUS
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS HiiLibGetString(
	IN EFI_HII_HANDLE HiiHandle, IN STRING_REF StringId, 
	IN OUT UINTN *StringSize, OUT EFI_STRING String
){
    EFI_STATUS Status;
	CHAR8 *SupportedLanguages, *BestLanguage;
	CHAR8 *PlatformLang;
    
    if (   !HiiString 
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiHiiStringProtocolGuid, NULL, &HiiString))
    ) return EFI_NOT_FOUND;
    PlatformLang = HiiLibGetPlatformLanguageInternal(TRUE);
	SupportedLanguages = GetSupportedLanguages (HiiHandle);
	if (SupportedLanguages == NULL) return EFI_NOT_FOUND;
	// Find the best supported language.
    // If the string representation is not defined for a PlatformLang, default language or English,
    // use the first supported language.
	BestLanguage = FindBestLanguage(
		SupportedLanguages, 
		PlatformLang, 
		(CHAR8*)DefaultLanguageCode, LANGUAGE_CODE_ENGLISH, 
		SupportedLanguages,
		NULL
	);
	if (BestLanguage == NULL){
		pBS->FreePool(SupportedLanguages);
		return EFI_NOT_FOUND;
	}

    Status = HiiString->GetString(
        HiiString, BestLanguage, HiiHandle,StringId, String, StringSize, NULL
    );
	pBS->FreePool(BestLanguage);
    // If the string representation for the PlatformLang is not defined,
    // try all the languages supported by the package.
    // If still no luck, try English
    if (Status==EFI_INVALID_LANGUAGE)
		Status = GetAnyLanguageString(
			SupportedLanguages,HiiString,
			HiiHandle,StringId,StringSize,String
		);
	pBS->FreePool(SupportedLanguages);
    return Status;
}

EFI_STATUS HiiLibGetStringEx(
	IN EFI_HII_HANDLE HiiHandle, IN STRING_REF StringId, IN CHAR8 *Language,
	IN OUT UINTN *StringSize, OUT EFI_STRING String
){
    EFI_STATUS Status;
	CHAR8 *SupportedLanguages, *BestLanguage;
	CHAR8 *PlatformLang;
    
    if (   !HiiString 
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiHiiStringProtocolGuid, NULL, &HiiString))
    ) return EFI_NOT_FOUND;
    PlatformLang = HiiLibGetPlatformLanguageInternal(FALSE);
	SupportedLanguages = GetSupportedLanguages (HiiHandle);
	if (SupportedLanguages == NULL) return EFI_NOT_FOUND;
	// Find the best supported language.
    // If the string representation is not defined for a PlatformLang, default language or English,
    // use the first supported language.
	BestLanguage = FindBestLanguage(
		SupportedLanguages, 
		Language, PlatformLang, 
		(CHAR8*)DefaultLanguageCode, LANGUAGE_CODE_ENGLISH,
		SupportedLanguages,
		NULL
	);
	if (BestLanguage == NULL){
		pBS->FreePool(SupportedLanguages);
		return EFI_NOT_FOUND;
	}

    Status = HiiString->GetString(
        HiiString, BestLanguage, HiiHandle,StringId, String, StringSize, NULL
    );
	pBS->FreePool(BestLanguage);
    // If the string representation for the PlatformLang is not defined,
    // try all the languages supported by the package.
    // If still no luck, try English
    if (Status==EFI_INVALID_LANGUAGE)
		Status = GetAnyLanguageString(
			SupportedLanguages,HiiString,
			HiiHandle,StringId,StringSize,String
		);
	pBS->FreePool(SupportedLanguages);
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: HiiLibSetString
//
// Description: Updates string in the HII database in all languages supported by the package list.
//
// Input:
//  IN EFI_HII_HANDLE HiiHandle - Package list handle
//  IN EFI_STRING_ID StringId - ID of the string to udpate
//  IN EFI_STRING String - Points to the new null-terminated string.
//
// Output:
//   EFI_STATUS
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS HiiLibSetString(
	IN EFI_HII_HANDLE HiiHandle, IN STRING_REF StringId, IN EFI_STRING String
){
	CHAR8 *LanguageList = NULL;
	UINTN Size = 0;
    EFI_STATUS Status;

    if (   !HiiString 
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiHiiStringProtocolGuid, NULL, &HiiString))
    ) return EFI_NOT_FOUND;

    Status = HiiString->GetLanguages(HiiString, HiiHandle, LanguageList, &Size);
    if (Status!=EFI_BUFFER_TOO_SMALL) return Status;
    LanguageList = Malloc(Size);
    Status = HiiString->GetLanguages(HiiString, HiiHandle, LanguageList, &Size);
    if (!EFI_ERROR(Status)){
        CHAR8 *CurrentLang, *EndOfCurrentLang;
        EndOfCurrentLang = LanguageList;
        while(EndOfCurrentLang < LanguageList+Size){
            CHAR8 OriginalChar;
            CurrentLang = EndOfCurrentLang;
            while(*EndOfCurrentLang!=';'&&*EndOfCurrentLang!=0) EndOfCurrentLang++;
            OriginalChar = *EndOfCurrentLang;
            *EndOfCurrentLang = 0;
            Status = HiiString->SetString(
                HiiString, HiiHandle, StringId, CurrentLang, String, NULL
            );
            if (EFI_ERROR(Status)) break;
            *EndOfCurrentLang = OriginalChar;
            EndOfCurrentLang++;
        }
    }
    pBS->FreePool(LanguageList);
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: HiiLibGetBrowserData
//
// Description: Retrieves uncommitted state data from the HII browser.
//  Only works in UEFI 2.1 mode.
//
// Input:
//   IN OUT UINTN *BufferSize - A pointer to the size of the buffer associated with Buffer. 
//     On input, the size in bytes of Buffer. On output, the size of data returned in Buffer.
//   OUT VOID *Buffer - A data returned from an IFR browser. 
//   IN CONST EFI_GUID *VarStoreGuid OPTIONAL - An optional field to indicate the target variable GUID name to use.
//   IN CONST CHAR16 *VarStoreName  OPTIONAL  - An optional field to indicate the target variable name.
//
// Output:
//   EFI_STATUS
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS HiiLibGetBrowserData(
    IN OUT UINTN *BufferSize, OUT VOID *Buffer, 
    IN CONST EFI_GUID *VarStoreGuid OPTIONAL,
    IN CONST CHAR16 *VarStoreName  OPTIONAL
){
    UINTN StringDataSize = 0;
    CHAR16 *StringData = NULL;
	CHAR16 *ConfigResp;
	UINTN ConfigRespSize;
    EFI_STATUS Status;
    EFI_STRING Progress;

    if (   !FormBrowser 
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiFormBrowser2ProtocolGuid, NULL, &FormBrowser))
    ) return EFI_NOT_FOUND;
    if (   !HiiConfigRouting 
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiHiiConfigRoutingProtocolGuid, NULL, &HiiConfigRouting))
    ) return EFI_NOT_FOUND;

    Status = FormBrowser->BrowserCallback(
        FormBrowser, &StringDataSize, StringData, TRUE, 
        VarStoreGuid, VarStoreName
    );
    if (Status!=EFI_BUFFER_TOO_SMALL) return Status;
	// We'll need to construct a proper <ConfigResp> string by 
	// appending dummy <ConfigHdr> at the beginning of a string returned by BrowserCallback.
	// This is required because
	// BrowserCallback returns string without <ConfigHdr>
	// and ConfigToBlock expects sring with <ConfigHdr>.
	
	// <DataSize> + <Dummy ConfigHdr Size> - <Terminating zero>
	ConfigRespSize = StringDataSize + sizeof(DummyConfigHeader)-sizeof(CHAR16);
    ConfigResp = Malloc(ConfigRespSize);
	StringData = ConfigResp + Swprintf_s(
		ConfigResp, ConfigRespSize, L"%s",DummyConfigHeader
	);
    Status = FormBrowser->BrowserCallback(
        FormBrowser, &StringDataSize, StringData, TRUE, 
        VarStoreGuid, VarStoreName
    );
    if (!EFI_ERROR(Status)){
        StringDataSize = *BufferSize;       //preserve passed buffer size value, as 
                                            //ConfigToBlock may change it as per UEFI 2.1 spec
                                            //it will contain index of last updated byte
        Status = HiiConfigRouting->ConfigToBlock(
            HiiConfigRouting, ConfigResp, Buffer, BufferSize, &Progress
        );
        *BufferSize = StringDataSize;       //restore original value
    }
    pBS->FreePool(ConfigResp);
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: HiiLibSetBrowserData
//
// Description: Updates uncommitted state data of the HII browser.
//  Only works in UEFI 2.1 mode.
//
// Input:
//   IN UINTN BufferSize - Size of the buffer associated with Buffer. 
//   IN VOID *Buffer - A data to send to an IFR browser. 
//   IN CONST EFI_GUID *VarStoreGuid OPTIONAL - An optional field to indicate the target variable GUID name to use.
//   IN CONST CHAR16 *VarStoreName  OPTIONAL  - An optional field to indicate the target variable name.
//
// Output:
//   EFI_STATUS
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS HiiLibSetBrowserData(
    IN UINTN BufferSize, IN VOID *Buffer, 
    IN CONST EFI_GUID *VarStoreGuid, OPTIONAL 
    IN CONST CHAR16 *VarStoreName  OPTIONAL
){
    UINTN StringDataSize = 0;
    CHAR16 *StringData = NULL;
    EFI_STATUS Status;
    EFI_STRING Config;
    EFI_STRING Progress;
	CHAR16 *ConfigResp;
	UINTN ConfigRespSize;
	
    if (   !FormBrowser 
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiFormBrowser2ProtocolGuid, NULL, &FormBrowser))
    ) return EFI_NOT_FOUND;
    if (   !HiiConfigRouting 
        && EFI_ERROR(pBS->LocateProtocol(&gLocalEfiHiiConfigRoutingProtocolGuid, NULL, &HiiConfigRouting))
    ) return EFI_NOT_FOUND;

    Status = FormBrowser->BrowserCallback(
        FormBrowser, &StringDataSize, StringData, TRUE, 
        VarStoreGuid, VarStoreName
    );
    if (Status!=EFI_BUFFER_TOO_SMALL) return Status;
	// We'll need to construct a proper <ConfigResp> string by 
	// appending dummy <ConfigHdr> at the beginning of a string returned by BrowserCallback.
	// This is required because
	// BrowserCallback returns string without <ConfigHdr>
	// and BlockToConfig expects sring with <ConfigHdr>.
	
	// <DataSize> + <Dummy ConfigHdr Size> - <Terminating zero>
	ConfigRespSize = StringDataSize + sizeof(DummyConfigHeader)-sizeof(CHAR16);
    ConfigResp = Malloc(ConfigRespSize);
	StringData = ConfigResp + Swprintf_s(
		ConfigResp, ConfigRespSize, L"%s",DummyConfigHeader
	);
    Status = FormBrowser->BrowserCallback(
        FormBrowser, &StringDataSize, StringData, TRUE, 
        VarStoreGuid, VarStoreName
    );
    if (EFI_ERROR(Status)){
		pBS->FreePool(ConfigResp);
        return Status;
	}

//we have StringData in format &OFFSET=XXXX&WIDTH=XXXX&VALUE=XXXX&OFFSET=...
//in order for function BlockToConfig to work we need to modify StringData as follows:
//&OFFSET=XXXX&WIDTH=XXXX&OFFSET=... (i.e. remove all &VALUE=XXX patterns)

    StripValuePattern(ConfigResp, ConfigRespSize);

    Status = HiiConfigRouting->BlockToConfig(
        HiiConfigRouting, ConfigResp, Buffer, BufferSize,
        &Config, &Progress
    );
    if(!EFI_ERROR(Status)) {
        StringDataSize = StrSize16(Config);
        Status = FormBrowser->BrowserCallback(
            FormBrowser, &StringDataSize, Config, FALSE, 
            VarStoreGuid, VarStoreName
        );
        pBS->FreePool(Config);
    }

    pBS->FreePool(ConfigResp);
    return Status;
}


HII_UTILITIES_PROTOCOL *GetHiiUtilitiesInterface(){
    static HII_UTILITIES_PROTOCOL *HiiUtilities = NULL;
    if (   HiiUtilities == NULL
        && EFI_ERROR(pBS->LocateProtocol(
                        &gLocalHiiUtilitiesProtocolGuid, NULL, &HiiUtilities
           ))
    //if LocateProtocol has failed, HiiUtilities is undefined.
    //set it to NULL.
    ) HiiUtilities = NULL; 
    return HiiUtilities;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LoadStrings
//
// Description:
//  EFI_STATUS LoadStrings(EFI_HANDLE ImageHandle,
// EFI_HII_HANDLE *pHiiHandle) - loads HII string packages associated with 
// the specified image and publishes them to the HII database
//
// Input:
//  EFI_HANDLE ImageHandle - Image Handle
//  EFI_HII_HANDLE *pHiiHandle - HII package list handle
//
// Output:
//   EFI_STATUS
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS LoadStrings(
	EFI_HANDLE ImageHandle, EFI_HII_HANDLE *pHiiHandle
)
{
    EFI_STATUS                      Status;
    EFI_HII_PACKAGE_LIST_HEADER     *PackageList;
    EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;
    
    //
    // Retrieve HII package list from ImageHandle
    //
    Status = pBS->OpenProtocol (
        ImageHandle,
        &gLocalEfiHiiPackageListProtocolGuid,
        (VOID **) &PackageList,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR (Status)) {
    	TRACE((DEBUG_ERROR,"gEfiHiiPackageListProtocolGuid protocol is not found\n"));
        return Status;
    }
  
    Status = pBS->LocateProtocol (
        &gLocalEfiHiiDatabaseProtocolGuid,
        NULL,
        &HiiDatabase
    );
    if (EFI_ERROR (Status)) {
    	TRACE((DEBUG_ERROR,"gEfiHiiDatabaseProtocolGuid protocol is not found\n"));
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
    TRACE((DEBUG_ERROR,"NewPackageList status: %r\n",Status));
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LoadResources
//
// Description:
//  EFI_STATUS LoadResources(EFI_HANDLE ImageHandle,
// UINTN NumberOfCallbacks, CALLBACK_INFO *pCallBack,
// INIT_HII_PACK InitFunction) - loads HII packages associated with 
// the specified image and publishes them to the HII database
//
// Input:
//  IN EFI_HANDLE ImageHandle - Image Handle
//  IN UINTN NumberOfCallbacks - Number of the structures in the pCallBack array
//	IN OUT CALLBACK_INFO *pCallBack - Array of IFR package descriptors.
//  IN INIT_HII_PACK InitFunction - initialization function to be launched once resources are published.
//
// Output:
//   EFI_STATUS
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS LoadResources(
	EFI_HANDLE ImageHandle, UINTN NumberOfCallbacks,
	CALLBACK_INFO *pCallBack, INIT_HII_PACK InitFunction
)
{
    EFI_STATUS                      Status;
    EFI_HII_PACKAGE_LIST_HEADER     *PackageList;
    EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;
	EFI_LOADED_IMAGE_PROTOCOL       *Image;
    EFI_HII_HANDLE                  mHiiHandle;
    static EFI_GUID gLocalEfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    
	if (EFI_ERROR(Status=pBS->HandleProtocol(ImageHandle, &gLocalEfiLoadedImageProtocolGuid, &Image))) return Status;
    
    //
    // Retrieve HII package list from ImageHandle
    //
    Status = pBS->OpenProtocol (
        ImageHandle,
        &gLocalEfiHiiPackageListProtocolGuid,
        (VOID **) &PackageList,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR (Status)) {
        return Status;
    }
  
    Status = pBS->LocateProtocol (
        &gLocalEfiHiiDatabaseProtocolGuid,
        NULL,
        &HiiDatabase
    );
    if (EFI_ERROR (Status)) {
        return Status;
    }
  
    //for(i=0; i<NumberOfIfrPackages; i++)
    {
        EFI_HANDLE CallbackHandle = NULL;
        EFI_HII_PACKAGE_HEADER *IfrPackagePtr;
        CALLBACK_INFO *pCallBackFound = NULL;

        for (IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(PackageList+1);
             IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)PackageList+PackageList->PackageLength);
             IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr+IfrPackagePtr->Length))
        {
            if (IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) {
                break;
            }
        }
    
        SetCallback(
            Image, (EFI_IFR_FORM_SET*)(IfrPackagePtr+1),
            NumberOfCallbacks,pCallBack,&pCallBackFound,&CallbackHandle
        );
        //
        // Publish HII package list to HII Database.
        //
        Status = HiiDatabase->NewPackageList (
            HiiDatabase,
            PackageList,
            CallbackHandle,
            &mHiiHandle
        );
        if (EFI_ERROR (Status)) {
            return Status;
        }
        
		if (pCallBackFound) pCallBackFound->HiiHandle = mHiiHandle;
		if (InitFunction) InitFunction(mHiiHandle, pCallBackFound);
		pCallBackFound = NULL;
    }

  return EFI_SUCCESS;
}

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
