//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Add suffix strings in Setup BBS pages for SUM to identify boot devices if they have the same device name.
//    Reason:   Help SUM to identify boot devices which have the same device name.
//    Auditor:  Jason Hsu
//    Date:     Jun/13/2017
//
//****************************************************************************

//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file FixedBootOrder.c

    This file is linked to Setup Binary for form callback
*/
#include <token.h>
#include <efi.h>
#include <AmiDxeLib.h>
#include <setup.h>

#if CSM_SUPPORT
#include <Protocol/LegacyBios.h>
#endif

#if EFI_SPECIFICATION_VERSION>0x20000
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigRouting.h>
#else   //#if EFI_SPECIFICATION_VERSION>0x20000         
#include <FormCallback.h>
#include <HIIDB.h>
#include <Protocol/ExtHii.h>
#endif

#include <FixedBootOrder.h>
#include <DefaultFixedBootOrder.h>
#if FBO_RUNTIME_CALLBACK_REGISTRATION
#include <Library/AmiSetupLibrary.h>
#endif
#include <AUTOID.h>
#include <Library/BaseMemoryLib.h>

#pragma pack(1)

typedef struct
{
    UINT32 Type;
    UINT16 Length;
    UINT16 Device[1];
} LEGACY_DEVICE_ORDER;

typedef struct
{
    UINT8 FormSet ;
    UINT16 FormMain ;
} FBO_SETUP_PAGE_INFORM;

#pragma pack()

BOOLEAN LoadedDefault = FALSE;

#if CSM_SUPPORT
BBS_TABLE *BbsTable = NULL;
HDD_INFO  *HddInfo;
UINT16    HddCount = 0;
UINT16    BbsCount = 0;
#endif

EFI_HII_STRING_PROTOCOL *pHiiString = NULL;

EFI_HII_HANDLE FixedBoot_HiiHandle;

EFI_GUID EfiVariableGuid = EFI_GLOBAL_VARIABLE;
EFI_GUID LegacyDevOrderGuid = LEGACY_DEV_ORDER_GUID;
EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
EFI_GUID DefaultLegacyDevOrderGuid = DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;

FBODevMap *pFBODualDevMap = NULL;
FBODevMap *pFBOLegacyDevMap = NULL;
FBODevMap *pFBOUefiDevMap = NULL;
FBOHiiMap *pFBOHiiMap = NULL;
UINT16 *pBootOptionTokenMap = NULL;     //(EIP123284+)

#if !FBO_RUNTIME_CALLBACK_REGISTRATION
CALLBACK_PARAMETERS *CallbackValues = NULL;
CALLBACK_PARAMETERS *GetCallbackParameters();
#endif

UINT8 gHiiSkipFBOModule = 0 ;
UINT8 BootDeviceNum;
UINT8 TypeLegacyDeviceOptions[FIXED_BOOT_ORDER_GROUP_MAX_NUM][FIXED_BOOT_ORDER_SUBMENU_MAX_NUM];
UINT8 TypeLegacyDeviceCount[FIXED_BOOT_ORDER_GROUP_MAX_NUM];        //GROUP
UINT8 TypeUefiDeviceOptions[FIXED_BOOT_ORDER_GROUP_MAX_NUM][FIXED_BOOT_ORDER_SUBMENU_MAX_NUM];
UINT8 TypeUefiDeviceCount[FIXED_BOOT_ORDER_GROUP_MAX_NUM];          //GROUP

typedef EFI_STATUS(FBO_INITIAL_GROUP)();
extern FBO_INITIAL_GROUP INITIAL_LEGCAY_GROUP_FUNCTION;
FBO_INITIAL_GROUP *InitLegacyGroup = INITIAL_LEGCAY_GROUP_FUNCTION;
extern FBO_INITIAL_GROUP INITIAL_UEFI_GROUP_FUNCTION;
FBO_INITIAL_GROUP *InitUefiGroup = INITIAL_UEFI_GROUP_FUNCTION;


FBO_SETUP_PAGE_INFORM FboSetupPageInform[] = {FIXED_BOOT_ORDER_SETUP_INFORM {0,0}} ;
/**
    Get the FBODevMap data amount.

    @param FBODevMap - FBODevMap

    @retval UINT16 - Number of FBODevMap

    @note  None

**/
UINT16 GetDevMapDataCount(FBODevMap *pFBODevMap)
{
    UINT16 count = 0;

    if (!pFBODevMap) return 0;           //end of data

    do
    {
        if (pFBODevMap[count].DevType == 0) break;
        count++;
    }
    while (1);

    return count;
}

/**
    get the FBOHiiMap data amount.

    @retval UINT16 - Number of FBODevMap

    @note  None

**/
UINT16 GetHiiMapDataCount()
{
    UINT16 count = 0;

    if (!pFBOHiiMap) return 0;           //end of data

    do
    {
        if (pFBOHiiMap[count].DevType == 0) break;
        count++;
    }
    while (1);

    return count;
}

/**
    Search DevMap data index by device type.

        
    @param  FBODevMap - Device Map
    @param  DevType - Device Type want to search 

    @retval UINT16 - Index of Device Type in Device Map

    @note  None

**/
UINT16 SearchDevMapByType(FBODevMap *pFBODevMap, UINT16 DevType)
{
    UINT16 i = 0;

    do
    {
        if (pFBODevMap[i].DevType == 0) break;  //end of data
        if (pFBODevMap[i].DevType == DevType)
            return i;
        i++;
    }
    while (1);

    return 0xffff;
}

/**
    Search HiiMap data index by device type.

        
    @param DevType - Device Type want to Search

    @retval UINT16 - Index of the Device Type

    @note  None

**/
UINT16 SearchHiiMapByType(UINT16 DevType)
{
    UINT16 i = 0;

    do
    {
        if (pFBOHiiMap[i].DevType == 0) break;  //end of data
        if (pFBOHiiMap[i].DevType == DevType)
            return i;
        i++;
    }
    while (1);

    return 0xffff;
}

/**
    Compare whether two HDD device paths are the same.

    @param  DevDp1  Device path in comparison.
    @param  DevDp2  Device path in comparison.

    @retval  EFI_SUCCESS    Two HDD paths are the same.
    @retval  EFI_NOT_FOUND  Two Hdd paths are not the same.
*/
EFI_STATUS CompareHddDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevDp1, EFI_DEVICE_PATH_PROTOCOL *DevDp2 )
{

    if ( DevDp1->Type == MEDIA_DEVICE_PATH &&
            DevDp1->SubType == MEDIA_HARDDRIVE_DP )
    {
        if (MemCmp(DevDp1+1, DevDp2+1, sizeof(HARDDRIVE_DEVICE_PATH)-sizeof(EFI_DEVICE_PATH_PROTOCOL)) == 0) //Skip Header EFI_DEVICE_PATH_PROTOCOL.
        {
            DevDp1 = NEXT_NODE(DevDp1);
            if ( DevDp1->Type == MEDIA_DEVICE_PATH &&
                    DevDp1->SubType == MEDIA_FILEPATH_DP ) Wcsupr( (CHAR16*)DevDp1+1 );

            DevDp2 = NEXT_NODE(DevDp2);
            if ( DevDp2->Type == MEDIA_DEVICE_PATH &&
                    DevDp2->SubType == MEDIA_FILEPATH_DP ) Wcsupr( (CHAR16*)DevDp2+1 );

            if (MemCmp(DevDp1, DevDp2, DPLength(DevDp2)) == 0)
                return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}

/**
    Judge Dp1 and Dp2 is same device path

        
    @param Dp1 - Device Path want to compare 
    @param Dp2 - Device Path want to compare 
    
    @retval TRUE - Same Device Path.
    @retval FALSE - Not same.
    
    @note  EIP 143657

**/
BOOLEAN IsDevicePathMatch(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp1, 
    IN EFI_DEVICE_PATH_PROTOCOL *Dp2)
{
    // Check null device path
    if (!Dp1 || !Dp2)
        return FALSE ;

    if (DPLength(Dp1) != DPLength(Dp2))
        return FALSE ;
    
    if (!MemCmp(Dp1, Dp2, DPLength(Dp1)))
        return TRUE ;
    else if (!CompareHddDevicePath(Dp1, Dp2))
        return TRUE ;

    return FALSE ;
}

/**
    Change Uefi Device option name in setup utility

        
    @param Handle - The Handle of Device
    @param DevName - Device Name Buffer
 
    @retval DevName - Device Name from Description Protocol
    @retval TRUE - Changed.
    @retval FALSE - Not Changed.
    
    @note  EIP 143657

**/
BOOLEAN ChangeLegacyDeviceOptionName(
    IN EFI_HANDLE Handle, 
    IN OUT CHAR16 *DevName)
{
    EFI_STATUS Status ;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FBOProtocol ; 
    Status = pBS->LocateProtocol (&FixedBootOrderGuid, NULL, &FBOProtocol);
    if (!EFI_ERROR(Status))
    {       
        EFI_DEVICE_PATH_PROTOCOL *Dp ;
        Status = pBS->HandleProtocol(Handle, &gEfiDevicePathProtocolGuid, &Dp);
        if (!EFI_ERROR(Status))
        {
            FBO_DEVICE_INFORM *DescriptionPtr ;
            DescriptionPtr = FBOProtocol->GetNewDescription() ;
            while( DescriptionPtr )
            {
                if (IsDevicePathMatch(Dp, DescriptionPtr->DevPath))
                {
                    MemSet(DevName,0x100,0) ;
                    StrCpy(DevName, DescriptionPtr->DevName) ;
                    TRACE((-1,"Set New Legacy Device Name: %S", DevName)) ;
                    return TRUE ;
                }
                DescriptionPtr = DescriptionPtr->Next ;
            }
        }
    }
    return FALSE ;
    
}

/**
    Change Uefi Device option name in setup utility

        
    @param BootOption - BootOption to be updated
    @param DevName - Device Name Buffer

    @retval DevName - Device Name
    @retval TRUE - Changed.
    @retval FALSE - Not Changed.
    
    @note  EIP 143657

**/
BOOLEAN ChangeUefiDeviceOptionName(
    IN EFI_LOAD_OPTION *BootOption, 
    IN OUT VOID **DevName)
{
    EFI_STATUS Status ;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FBOProtocol ;
    Status = pBS->LocateProtocol (&FixedBootOrderGuid, NULL, &FBOProtocol);
    if (!EFI_ERROR(Status))
    {
        FBO_DEVICE_INFORM *DescriptionPtr ;
        UINT8 *FilePathList ;
        UINTN OldNameSize ;
        OldNameSize = (Wcslen((CHAR16*)(BootOption+1))+1)*sizeof(CHAR16);
        FilePathList = (UINT8*)(BootOption+1)+OldNameSize;
        DescriptionPtr = FBOProtocol->GetNewDescription() ;
        while( DescriptionPtr )
        {
            if (IsDevicePathMatch((EFI_DEVICE_PATH_PROTOCOL*)FilePathList, DescriptionPtr->DevPath))
            {
                *DevName = DescriptionPtr->DevName ;
                TRACE((-1,"Set New Uefi Device Name: %S", (CHAR16*)*DevName)) ;
                return TRUE ;
            }
            DescriptionPtr = DescriptionPtr->Next ;
        }
    }
    return FALSE ;
}

/**
    Change FBO Group Name
 
    @note  EIP 143657

**/
VOID
FBO_AdjustGroupName()
{
    EFI_STATUS Status ;
    CHAR8* Languages = NULL, *OrgLanguages = NULL;
    UINT16 x ;
    UINT16 HiiSize = GetHiiMapDataCount() ;
    UINT32 Attr ;
    UINTN *GroupPtrAddr = NULL;
    UINTN LangSize = 0, Size = 0 ;
    UINTN OptionMaxSize = 0x100;
    BOOLEAN LastLanguage = FALSE;
    FBO_GROUP_OPTION *Group = NULL, *PreGroup = NULL ;
    
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status))return;
    }

    Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status)) return;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status)) return;  
    
    // Get Pointer Address 
    Status = GetEfiVariable(L"FboGroupNameData", &FixedBootOrderGuid, &Attr, &Size, &GroupPtrAddr);
    if ( EFI_ERROR(Status)) return ;
    
    OrgLanguages = Languages ;
    while(!LastLanguage) 
    {
        CHAR8* CurrentLanguage, StrSaveTemp ;
        CurrentLanguage = Languages;        //point CurrentLanguage 
        while (*Languages != ';' && *Languages != 0)
            Languages++;

        StrSaveTemp = *Languages;
        *Languages = 0;         //put null-terminator
        
        Group = (FBO_GROUP_OPTION*)*GroupPtrAddr ;
        while (Group)
        {
            for ( x=0; x < HiiSize ; x++ )
            {
                if (Group->StrToken == pFBOHiiMap[x].StrToken )
                {
                    TRACE((-1,"FixedBootOrder.c AdjustFBOGroupName Change Group Name(StrToken)\n")) ;
                    Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, pFBOHiiMap[x].StrToken, 
                                               CurrentLanguage, Group->NewGroupName, NULL);
                }
                else
                {
                    CHAR16 *TempString = MallocZ(OptionMaxSize) ;
                    Size = OptionMaxSize ;  
                    Status = pHiiString->GetString(pHiiString, CurrentLanguage, FixedBoot_HiiHandle, 
                                                   pFBOHiiMap[x].StrToken, TempString, &Size, NULL);
                    // if string match, update the new group name.
                    if (Group->OrgGroupName && !StrCmp(TempString, Group->OrgGroupName))
                    {
                        TRACE((-1,"FixedBootOrder.c AdjustFBOGroupName Change Group Name(OrgName)\n")) ;
                        Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, pFBOHiiMap[x].StrToken, 
                                                       CurrentLanguage, Group->NewGroupName, NULL);
                    }
                    pBS->FreePool(TempString) ;
                }   
            }     
            Group = Group->Next ;
        }
        *Languages = StrSaveTemp;
        
        if (*Languages == 0)    //last language in language list
            LastLanguage = TRUE ;
        else
            Languages++ ;
    }
    
    
    // Free all pointer and content
    while (Group)
    {
       PreGroup = Group;
       Group = Group->Next;
       pBS->FreePool(PreGroup);
    }
    Status = pRS->SetVariable(L"FboGroupNameData", &FixedBootOrderGuid, 0, 0, NULL);
    pBS->FreePool(OrgLanguages) ;
    pBS->FreePool(GroupPtrAddr) ;
}

/**
    New the string with language that was first found.

        
    @param HiiHandle  
    @param StrId 
    @param String 

    @retval EFI_STATUS

    @note  None

**/
EFI_STATUS 
NewStringWithFirstFoundLanguage
(
  IN EFI_HANDLE HiiHandle, 
  IN EFI_STRING_ID* StrId, 
  IN CHAR16 *String
)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    UINTN  i;
    UINTN LangSize = 0;

    //
    // Get Hii String Protocol
    //
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status)) return Status;
    }

    //
    // Get Languages
    //
    Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return Status;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status)) {
        return Status;
    }

    // Search for ';'
    for(i = 0 ; (i < LangSize) && (*(Languages + i) != ';') ; i ++);                
    if(i != LangSize)
        // Find ';' , set a null-terminated
        *(Languages + i) = 0;

    Status = pHiiString->NewString(pHiiString, HiiHandle, StrId, Languages, NULL, String, NULL);
    pBS->FreePool(Languages);

    return Status;
}

/**
    Get Variable Boot#### and return data pointer.

        
    @param BootIndex - Boot Variable Number 

    @retval EFI_LOAD_OPTION - Pointer of Load Option
    @retval NULL - Load Option not found

**/
EFI_LOAD_OPTION* 
GetUEFIBootName
(
  IN UINT16 BootIndex
)
{
    EFI_STATUS Status;
    UINTN OptionSize = 0;
    CHAR16 BootVarName[15];
    EFI_LOAD_OPTION *BootOption = NULL;

    Swprintf(BootVarName, gBootName, BootIndex);             //(EIP35562)

    Status = GetEfiVariable(BootVarName, &EfiVariableGuid, NULL, &OptionSize, &BootOption);
    if (!EFI_ERROR(Status))
        return BootOption;

    return NULL;

}
//(EIP101408+)>
/**
    Update device item multi-language Hii string

    @param StrToken - Sring Token to be updated
    @param *DeviceName - Device Name

**/
VOID 
UpdateDeviceHiiString
(
  IN UINT16 StrToken, 
  IN CHAR16 *DeviceName
)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    CHAR8* OrgLanguages = NULL;
    UINTN LangSize = 0;
    BOOLEAN LastLanguage = FALSE;
    CHAR8 StrSaveTemp;
    UINTN  i, Size = 0;
    CHAR16 OldStr[0x100+1], DeviceStr[0x100+1];

    if (StrToken < 0x100)
    {
        ASSERT(StrToken > 0x100) ;
        return ;
    }
    
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) & pHiiString);
        if (EFI_ERROR(Status))
            return;
    }

    Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status))
        return;

    OrgLanguages = Languages;
    while (!LastLanguage)
    {
        CHAR8* CurrentLanguage;
        CurrentLanguage = Languages;        //point CurrentLanguage to start of new language
        while (*Languages != ';' && *Languages != 0)
            Languages++;

        StrSaveTemp = *Languages;
        *Languages = 0;         //put null-terminator
        Size = 0x100;
        TRACE((FBO_TRACE_LEVEL, "CurrentLanguage=%s\n", CurrentLanguage));
        Status = pHiiString->GetString(pHiiString, CurrentLanguage, FixedBoot_HiiHandle, StrToken, OldStr, &Size, NULL);
        *Languages = StrSaveTemp;

        if (EFI_ERROR(Status))       //Language not define in .uni
        {
            if (*Languages == 0)    //last language in language list
                LastLanguage = TRUE;
            else
            {
                *Languages = ';';       //restore original character
                Languages++;
            }
            continue;
        }


        //removec char ":"
        for (i = 0; i < Wcslen(OldStr); i++)
        {
            if (OldStr[i] == 0x003A)
            {
                OldStr[i] = (CHAR16)0;
                break;
            }
        }

        if (DeviceName == NULL)
            Swprintf(DeviceStr, L"%s", OldStr);
        else
            Swprintf(DeviceStr, L"%s:%s", OldStr, DeviceName);

        if (*Languages == 0)        //last language in language list
        {
            LastLanguage = TRUE;
            Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, StrToken, CurrentLanguage, DeviceStr, NULL);
            if (EFI_ERROR(Status))
            {
                pBS->FreePool(OrgLanguages);
                return;
            }
        }
        else
        {
            *Languages = 0;         //put null-terminator
            Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, StrToken, CurrentLanguage, DeviceStr, NULL);
            *Languages = ';';       //restore original character
            Languages++;
            if (EFI_ERROR(Status))
            {
                pBS->FreePool(OrgLanguages);
                return;
            }
        }
    }

    if (OrgLanguages)
        pBS->FreePool(OrgLanguages);
}


/**
    Sync legacy string to Group form HII handle.

    @param  Offset      
    @param  DeviceName      New string.

    @retval EFI_SUCCESS    Update new string to Group form success.
*/
EFI_STATUS
SyncLegacyStringWithGroupFormHii(
    IN UINT16   Offset, 
    IN CHAR16   *DeviceName)
{
    EFI_STATUS Status ;
    EFI_HII_HANDLE FboGfHiiHandle ;
    HII_DEVICE_INFO *LegacyHiiToken = NULL ;
    UINTN Size = sizeof(EFI_HII_HANDLE) ;
    UINTN LegacyHiiSize = 0 ;
    
    Status = pRS->GetVariable(L"FboGfHiiHandle", &FixedBootOrderGuid, NULL, &Size, &FboGfHiiHandle );
    if (EFI_ERROR(Status)) return Status ;
    
    Status = GetEfiVariable(L"FixedGfHiiLegacyDevData", 
                            &FixedBootOrderGuid, 
                            NULL, 
                            &LegacyHiiSize, 
                            &LegacyHiiToken);
    if (EFI_ERROR(Status)) return Status ;
    
    InitString(FboGfHiiHandle, (UINT16)(*((UINT8*)LegacyHiiToken + Offset)), L"%s", DeviceName);
    pBS->FreePool(LegacyHiiToken) ;
    return Status ;
}

/**
    Sync UEFI string to Group form HII handle.

    @param  Offset      
    @param  DeviceName      New string.

    @retval EFI_SUCCESS    Update new string to Group form success.
*/
EFI_STATUS
SyncUefiStringWithGroupFormHii(
    IN UINT16   Offset, 
    IN CHAR16   *DeviceName)
{
    EFI_STATUS Status ;
    EFI_HII_HANDLE FboGfHiiHandle ;
    HII_DEVICE_INFO *UefiHiiToken = NULL ;
    UINTN Size = sizeof(EFI_HII_HANDLE) ;
    UINTN UefiHiiSize = 0 ;
    
    Status = pRS->GetVariable(L"FboGfHiiHandle", &FixedBootOrderGuid, NULL, &Size, &FboGfHiiHandle );
    if (EFI_ERROR(Status)) return Status ;
    
    Status = GetEfiVariable(L"FixedGfHiiUefiDevData", 
                            &FixedBootOrderGuid, 
                            NULL, 
                            &UefiHiiSize, 
                            &UefiHiiToken);
    if (EFI_ERROR(Status)) return Status ;
    
    InitString(FboGfHiiHandle, (UINT16)(*((UINT8*)UefiHiiToken + Offset)), L"%s", DeviceName);
    pBS->FreePool(UefiHiiToken) ;
    return Status ;
}

/**
    Adjust DefaultLegacyDevOrder value.

    @param **DevOrderBuffer
    @param *Size
    
    @retval NONE

**/
VOID
AdjustLegacyDevOrder(
    VOID**  DevOrderBuffer,
    UINTN   *Size)
{
#if CSM_SUPPORT
    LEGACY_DEVICE_ORDER     *DevOrder = *DevOrderBuffer;
    UINT16                  BBSIndex = 0;
    UINTN                   Size1 = 0, i = 0;
    VOID                    *DevOrderBuffer1 = NULL;
    LEGACY_DEVICE_ORDER     *DevOrder1 = NULL;
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_GUID                FboBdsGuid = FIXED_BOOT_ORDER_BDS_GUID ;
    EFI_FIXED_BOOT_ORDER_BDS_PROTOCOL *FboBdsProtocol = NULL ;
    
    if(BbsTable == NULL) return;
    
#if GROUP_BOOT_OPTIONS_BY_TAG
    Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size1, &DevOrderBuffer1);
    if(EFI_ERROR(Status)) return;
    for (
            ; (UINT8*)DevOrder < (UINT8*)*DevOrderBuffer + *Size
            ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        // Use the first one to check the device type
        BBSIndex = DevOrder->Device[0] & FBO_LEGACY_ORDER_MASK;
        for (DevOrder1 = DevOrderBuffer1
             ; (UINT8*)DevOrder1 < (UINT8*)DevOrderBuffer1 + Size1
             ; DevOrder1 = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder1 + DevOrder1->Length + sizeof(DevOrder1->Type)))
        {
            for (i = 0; i < DEVORDER_COUNT(DevOrder1); i++)
            {
                if(BBSIndex == (DevOrder1->Device[i] & FBO_LEGACY_ORDER_MASK))
                {
                    DevOrder->Type = DevOrder1->Type;
                    // break these 2 loop
                    (UINT8*)DevOrder1 = (UINT8*)DevOrderBuffer1 + Size1;
                    break;
                }
            }
        }
    }
    pBS->FreePool(DevOrderBuffer1);  
#else
    Status = pBS->LocateProtocol(&FboBdsGuid, NULL, &FboBdsProtocol) ;
    if (EFI_ERROR(Status)) return;
     
    Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size1, &DevOrderBuffer1);
    if(EFI_ERROR(Status)) return;
     
    for (DevOrder1 = DevOrderBuffer1
         ; (UINT8*)DevOrder1 < (UINT8*)DevOrderBuffer1 + Size1
         ; DevOrder1 = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder1))
    {
        UINT16 Idx = 0 ;
        for (DevOrder = *DevOrderBuffer
             ; (UINT8*)DevOrder < (UINT8*)*DevOrderBuffer + *Size
             ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
        {
            UINT16 LegacyFboType = FboBdsProtocol->GetLegacyTag(DevOrder->Device[0]) ;
            if (LegacyFboType == INVALID_FBO_TAG) continue ;
            if (LegacyFboType == (UINT16)DevOrder1->Type)
            {
                DevOrder1->Device[Idx] = DevOrder->Device[0] ;
                Idx++ ;
            }
        }
    }
     
    pBS->FreePool(*DevOrderBuffer);
    *DevOrderBuffer = DevOrderBuffer1 ;
    *Size = Size1 ;
#endif  //#if GROUP_BOOT_OPTIONS_BY_TAG
#endif  //#if CSM_SUPPORT
}

/**
   Initialize Legacy Hii String with OriFboLegacyDevOrder variable.

   @retval EFI_SUCCESS    fucntion works successfully.
   @retval !=EFI_SUCCESS  GetEfiVariable failed.
**/
EFI_STATUS
InitFboLegacyHiiString()
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    VOID       *DevOrderBuffer = NULL;
    UINT16     i, j;
    UINTN      BBSNamePtr;
    UINTN      Size = 0;
    CHAR16     DeviceName[0x100+1];
    UINTN      HiiDevDataSize = 0;
    HII_DEVICE_INFO *pHiiDevTokenData = NULL;

    Status = GetEfiVariable(L"OriFboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, (VOID**)&DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER, Device))
    {
        LEGACY_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiLegacyDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        HiiDevOrder = pHiiDevTokenData;
        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;

            if(DevOrder->Type == BBS_UNKNOWN) continue;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {
                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                j = DevOrder->Device[i] & FBO_LEGACY_ORDER_MASK; // high byte is used to indicate if it is disabled

                if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                {
                    BBSNamePtr = (UINTN)(((UINTN)BbsTable[j].DescStringSegment << 4)
                                         + BbsTable[j].DescStringOffset);

                    Swprintf(DeviceName, L"%a", BBSNamePtr);
                    ChangeLegacyDeviceOptionName((EFI_HANDLE)BbsTable[j].IBV1,(CHAR16*)&DeviceName) ;//(EIP143657+)
                    InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", DeviceName);

                    TRACE((FBO_TRACE_LEVEL, "BBS DEVICE(%d):%S \n", i, DeviceName));
                }
            }
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);
#endif //#if CSM_SUPPORT
    return EFI_SUCCESS;

}

/**
   Initialize Uefi Hii String with OriFboUefiDevOrder variable.

   @retval EFI_SUCCESS    fucntion works successfully.
   @retval !=EFI_SUCCESS  GetEfiVariable failed.
**/
EFI_STATUS
InitFboUefiHiiString()
{
    EFI_STATUS        Status;
    EFI_GUID          FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    VOID              *DevOrderBuffer = NULL;
    UINT16            i, j;
    UINTN             Size = 0;
    HII_DEVICE_INFO   *pHiiDevTokenData = NULL;
    UINTN             HiiDevDataSize = 0;

    Status = GetEfiVariable(L"OriUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER, Device))
    {
        UEFI_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiUefiDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {

                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                EFI_LOAD_OPTION *BootOption = NULL;

                j = DevOrder->Device[i] & FBO_UEFI_ORDER_MASK; // highest byte is used to indicate if it is disabled

                BootOption = GetUEFIBootName(j);
                if (BootOption)
                {
                    if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                    {
                        CHAR16 *NameStr;

                        NameStr = (CHAR16*)((UINT8*)BootOption + sizeof(EFI_LOAD_OPTION));
                        ChangeUefiDeviceOptionName(BootOption,&NameStr) ;
                        InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", NameStr);

                        TRACE((FBO_TRACE_LEVEL, "UEFI DEVICE(%d):%S \n", i, NameStr));
                    }
                    pBS->FreePool(BootOption);
                }
            }
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);

    return EFI_SUCCESS;
}

//<(EIP101408+)
/**
    Update boot option type string token

        
    @param DevType 
    @param Index 

**/
VOID 
UpdateLegacyDeviceHiiString
(
  IN UINT16 DevType, 
  IN UINT8 Index
)
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    VOID *DevOrderBuffer = NULL;
    UINTN BBSNamePtr;
    UINTN Size = 0;
    CHAR16 DeviceName[0x100+1];
    UINT16 HiiDevMapIndex, HiiDevMapCount;

    HiiDevMapCount = GetHiiMapDataCount();
    for (HiiDevMapIndex = 0; HiiDevMapIndex < HiiDevMapCount; HiiDevMapIndex++)
    {
        if (pFBOHiiMap[HiiDevMapIndex].DevType == DevType)
            break;
    }

    if (Index >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
    {
        UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, NULL);     //(EIP101408+)
    }
    else
    {
        if (LoadedDefault)
        {
            Status = GetEfiVariable(L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid, NULL, &Size, &DevOrderBuffer);
            AdjustLegacyDevOrder(&DevOrderBuffer, &Size);
        }
        else
            Status = GetEfiVariable(L"OriFboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

        if (!EFI_ERROR(Status) && Size >= EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER,Device))
        {
            UINTN j;
            LEGACY_DEVICE_ORDER *DevOrder;

            for (DevOrder = DevOrderBuffer
                            ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                    ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
            {

                if (DevOrder->Type != DevType) continue;
                if (DevOrder->Length <= sizeof(DevOrder->Length)) continue;

                j = DevOrder->Device[Index] & FBO_LEGACY_ORDER_MASK; // high byte is used to indicate if it is disabled for LegacyDevOrder

                {
                    char *pStr;
                    UINTN x ;
                    BBSNamePtr = (UINTN)(((UINTN)BbsTable[j].DescStringSegment << 4)
                                         + BbsTable[j].DescStringOffset);

                    pStr = (char *)BBSNamePtr;
                    for (x = 0; x < 10 ; x++)
                    {
                        if (*pStr == ':')
                        {
                            BBSNamePtr += x + 1;
                            break;
                        }
                        else
                            pStr++;
                    }
                    Swprintf(DeviceName, L"%a", BBSNamePtr);
                    ChangeLegacyDeviceOptionName((EFI_HANDLE)BbsTable[j].IBV1,(CHAR16*)&DeviceName) ; 
                    UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, DeviceName);
                }
                break;
            } //for(  DevOrder = DevOrderBuffer..........
        } //if (!EFI_ERROR(Status) && Size>=6)

        if (DevOrderBuffer != NULL)
            pBS->FreePool(DevOrderBuffer);
    }
#endif //#if CSM_SUPPORT
}

/**
    Update boot option type string token

        
    @param DevType 
    @param Index 

**/
void UpdateUefiDeviceHiiString(IN UINT16 DevType, IN UINT8 Index)
{
    EFI_STATUS Status;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    VOID *DevOrderBuffer = NULL;
    UINTN Size = 0;
    UINT16 HiiDevMapIndex, HiiDevMapCount;

    HiiDevMapCount = GetHiiMapDataCount();
    for (HiiDevMapIndex = 0; HiiDevMapIndex < HiiDevMapCount; HiiDevMapIndex++)
    {
        if (pFBOHiiMap[HiiDevMapIndex].DevType == DevType)
            break;
    }

    if (Index >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
    {
        UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, NULL);         //(EIP101408+)
    }
    else
    {
        if (LoadedDefault)
            Status = GetEfiVariable(L"DefaultUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);
        else
            Status = GetEfiVariable(L"OriUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

        if (!EFI_ERROR(Status) && Size >= EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER,Device))
        {
            UINTN j;
            UEFI_DEVICE_ORDER *DevOrder;

            for (DevOrder = DevOrderBuffer
                            ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                    ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
            {

                if (DevOrder->Type != DevType) continue;
                if (DevOrder->Length <= sizeof(DevOrder->Length)) continue;

                j = DevOrder->Device[Index] & FBO_UEFI_ORDER_MASK;   // [31:24] is disabled flag

                {
                    EFI_LOAD_OPTION *BootOption = NULL;
                    BootOption = GetUEFIBootName((UINT16)j);
                    if (BootOption)
                    {
                        UINT8 *NameStr;

                        NameStr = ((UINT8*)BootOption) + sizeof(EFI_LOAD_OPTION);
                        ChangeUefiDeviceOptionName(BootOption,&NameStr) ;
                        UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, (CHAR16*)NameStr);
                        pBS->FreePool(BootOption);
                    }
                }
                break;
            } //for(  DevOrder = DevOrderBuffer..........
        } //if (!EFI_ERROR(Status) && Size>=6)

        if (DevOrderBuffer != NULL)
            pBS->FreePool(DevOrderBuffer);
    }
}


#if FBO_RUNTIME_CALLBACK_REGISTRATION
/**
    Fixed Boot Order Setup callback Function.

    @param CallbackValues - Callback variable.

    @retval EFI_SUCCESS - Success
**/
EFI_STATUS FixedBootOrderFormCallBack(
    IN AMI_HII_CALLBACK_PARAMETERS *CallbackValues
)
#else
/**
    Fixed Boot Order Setup callback Function.

    @param HiiHandle - Hii Handle
    @param Class - Class
    @param SubClass - SubClass
    @param KeyValue - KeyValue

    @retval EFI_SUCCESS - Success
**/
EFI_STATUS FixedBootOrderFormCallBack(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class,
    IN UINT16 SubClass,
    IN UINT16 KeyValue
)
#endif
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_GUID    gSetupGuid = SETUP_GUID;
    UINT16      Current, Swap = 0xffff, MapPri = 0;
    UINT16      Mapped[16];
    UINT16      i = 0, DeviceCount;
    BOOLEAN     Uefi_Boot_item = FALSE;
#if FBO_DUAL_MODE
    BOOLEAN     Dual_Boot_item = FALSE;
#endif
#if FBO_RUNTIME_CALLBACK_REGISTRATION
    UINT16      KeyValue;
#endif
    SETUP_DATA  pSetup;
    SETUP_DATA  *IfrNvData = &pSetup;
    UINTN       BufferSize;
    BufferSize = sizeof(SETUP_DATA);

    if (gHiiSkipFBOModule) return EFI_SUCCESS;

#if FBO_RUNTIME_CALLBACK_REGISTRATION
    KeyValue  = (UINT16)(CallbackValues->QuestionId);
#else
    CallbackValues = GetCallbackParameters();
    KeyValue = CallbackValues->KeyValue;
#endif

#if (EFI_SPECIFICATION_VERSION >= 0x2000A)
    if (CallbackValues->Action == EFI_BROWSER_ACTION_CHANGING)
        return EFI_SUCCESS;
    else if (CallbackValues->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
        return EFI_UNSUPPORTED;
    else if (CallbackValues->Action != EFI_BROWSER_ACTION_CHANGED)
        return EFI_INVALID_PARAMETER;
#endif

    TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c] ***** INTERACTIVE callback.  KEY=%X\n", KeyValue));

    //Check Is FixedBootOrder assigned keys?
    if (KeyValue < FixedBootOrderStratKey 
        || KeyValue > (UefiForms_StartKey+FIXED_BOOT_ORDER_TOTAL_DEVICES_MAX_NUM)) 
        return EFI_UNSUPPORTED;

    //--------------------------------------
    //Legacy & Uefi Boot item change process
    //--------------------------------------
#if FBO_DUAL_MODE
    if (KeyValue >= DualBootItemKey)
    {
        DeviceCount = GetDevMapDataCount(pFBODualDevMap);
        Dual_Boot_item = TRUE;
    }
    else
#endif
        if (KeyValue >= UefiBootItemKey)
        {
            DeviceCount = GetDevMapDataCount(pFBOUefiDevMap);
            Uefi_Boot_item = TRUE;
        }
        else
        {
            DeviceCount = GetDevMapDataCount(pFBOLegacyDevMap);
            Uefi_Boot_item = FALSE;
        }

    KeyValue = (KeyValue & 0x7f) + 1;
    Current = CallbackValues->Value->u16;  // current user selection
    //
    // Retrive uncommitted data from Browser
    //
    Status = HiiLibGetBrowserData(&BufferSize, IfrNvData, &gSetupGuid, L"Setup");
    if (EFI_ERROR(Status))
    {
        TRACE((FBO_TRACE_LEVEL, "FixedBootOrder.c HiiLibGetBrowserData (%r)\n", Status));
        return Status;
    }

    TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c] Current=%X\n", Current));

    // Init all Mapped to FALSE
    for (i = 0; i < DeviceCount; i++)
        Mapped[i] = FALSE;

    // Find duplicated boot order to (Swap)
    for (i = 0; i < DeviceCount; i++)
    {
#if FBO_DUAL_MODE
        if (Dual_Boot_item)
            MapPri = IfrNvData->DualPriorities[i];
        else
#endif
            if (Uefi_Boot_item)
                MapPri = IfrNvData->UefiPriorities[i];
            else
                MapPri = IfrNvData->LegacyPriorities[i];

        TRACE((FBO_TRACE_LEVEL, "[FixedBoot] Priorities[%d]=%d\n", i, MapPri));

        if (MapPri == Current && i + 1 != KeyValue && Current < DeviceCount)
        {
            // i = duplicated index
            Swap = i;
            TRACE((FBO_TRACE_LEVEL, "[FixedBoot] Need to change Priorities[%d]\n", Swap));
        }
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
        if (MapPri == DeviceCount) continue;     //is Disable?
#endif
        Mapped[MapPri] = TRUE;      // TRUE = priority set
    }

    if (Swap == 0xffff)     // not found duplicated?
        return EFI_SUCCESS;

    // Change the duplicated value according to Mapped[]
    for (i = 0; i < DeviceCount; i++)
    {
        if (Mapped[i] == FALSE)     // This priority #i is not set yet
        {
#if FIXED_BOOT_SWAP_POLICY == 0        // just swap the duplicated
#if FBO_DUAL_MODE
            if (Dual_Boot_item)
                IfrNvData->DualPriorities[Swap] = i; // not minus nor plus, just make no duplication
            else
#endif
                if (Uefi_Boot_item)
                    IfrNvData->UefiPriorities[Swap] = i; // not minus nor plus, just make no duplication
                else
                    IfrNvData->LegacyPriorities[Swap] = i; // not minus nor plus, just make no duplication
#endif  //#if FIXED_BOOT_SWAP_POLICY == 0

#if FIXED_BOOT_SWAP_POLICY == 1        // move up or down
            UINT16      Val1;
            UINT16      Val2;
            BOOLEAN     PlusWrap;
            BOOLEAN     MinusWrap;
            UINT16      NewVal;
#if FBO_DUAL_MODE
            if (Dual_Boot_item)
                NewVal = IfrNvData->DualPriorities[Swap];  // new value
            else
#endif
                if (Uefi_Boot_item)
                    NewVal = IfrNvData->UefiPriorities[Swap];
                else
                    NewVal = IfrNvData->LegacyPriorities[Swap];  // new value

            if (MinusWrap = (i == 0 && NewVal == DeviceCount - 1))    // old = 0, new = last, wrapped?
            {
#if FBO_DUAL_MODE
                if (Dual_Boot_item)
                    IfrNvData->DualPriorities[KeyValue-1] = 0;
                else
#endif
                    if (Uefi_Boot_item)
                        IfrNvData->UefiPriorities[KeyValue-1] = 0;
                    else
                        IfrNvData->LegacyPriorities[KeyValue-1] = 0;
            }

            if (PlusWrap = (i == DeviceCount - 1 && NewVal == 0))     // old = last, new = 0, wrapped?
            {
#if FBO_DUAL_MODE
                if (Dual_Boot_item)
                    IfrNvData->DualPriorities[KeyValue-1] = i;
                else
#endif
                    if (Uefi_Boot_item)
                        IfrNvData->UefiPriorities[KeyValue-1] = i;
                    else
                        IfrNvData->LegacyPriorities[KeyValue-1] = i;
            }
            // Prohibit wrapping

            if (KeyValue == 1)      // first one?
            {
                if (MinusWrap)      // wrap?
                    continue;       // skip
                if (NewVal == i - 1)  // -1?
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                        IfrNvData->DualPriorities[KeyValue-1] = NewVal + 1;
                    else
#endif
                        if (Uefi_Boot_item)
                            IfrNvData->UefiPriorities[KeyValue-1] = NewVal + 1;
                        else
                            IfrNvData->LegacyPriorities[KeyValue-1] = NewVal + 1;
                    continue;
                }
            }

            if (KeyValue == DeviceCount)        // last one?
            {
                if (PlusWrap)       // wrap?
                    continue;       // skip
                if (NewVal == i + 1)  // +1?
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                        IfrNvData->DualPriorities[KeyValue-1] = NewVal - 1;
                    else
#endif
                        if (Uefi_Boot_item)
                            IfrNvData->UefiPriorities[KeyValue-1] = NewVal - 1;
                        else
                            IfrNvData->LegacyPriorities[KeyValue-1] = NewVal - 1;
                    continue;
                }
            }

            if (NewVal == i - 1 || MinusWrap)     // new = old-1 or wrapped?
            {
                // Minus
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                    {
                        if (!MinusWrap)
                            IfrNvData->DualPriorities[KeyValue-1]++;   // +1 because it was -1 by "-" key

                        Val1 = IfrNvData->DualPriorities[KeyValue-2];    // previous
                        Val2 = IfrNvData->DualPriorities[KeyValue-1];  // current
                        IfrNvData->DualPriorities[KeyValue-2] = Val2;
                        IfrNvData->DualPriorities[KeyValue-1] = Val1;
                    }
                    else
#endif
                        if (Uefi_Boot_item)
                        {
                            if (!MinusWrap)
                                IfrNvData->UefiPriorities[KeyValue-1]++;   // +1 because it was -1 by "-" key

                            Val1 = IfrNvData->UefiPriorities[KeyValue-2];    // previous
                            Val2 = IfrNvData->UefiPriorities[KeyValue-1];  // current
                            IfrNvData->UefiPriorities[KeyValue-2] = Val2;
                            IfrNvData->UefiPriorities[KeyValue-1] = Val1;
                        }
                        else
                        {
                            if (!MinusWrap)
                                IfrNvData->LegacyPriorities[KeyValue-1]++;   // +1 because it was -1 by "-" key

                            Val1 = IfrNvData->LegacyPriorities[KeyValue-2];    // previous
                            Val2 = IfrNvData->LegacyPriorities[KeyValue-1];  // current
                            IfrNvData->LegacyPriorities[KeyValue-2] = Val2;
                            IfrNvData->LegacyPriorities[KeyValue-1] = Val1;
                        }
                }
            }
            else
            {
                if (NewVal == i + 1 || PlusWrap)              // new = old+1 or wrapped?
                {
                    // Plus
                    {
#if FBO_DUAL_MODE
                        if (Dual_Boot_item)
                        {

                            if (!PlusWrap)
                                IfrNvData->DualPriorities[KeyValue-1]--;      // -1 because it was +1 by "+" key

                            Val1 = IfrNvData->DualPriorities[KeyValue-1];    // current
                            Val2 = IfrNvData->DualPriorities[KeyValue];    // next
                            IfrNvData->DualPriorities[KeyValue-1] = Val2;
                            IfrNvData->DualPriorities[KeyValue] = Val1;
                        }
                        else
#endif
                            if (Uefi_Boot_item)
                            {
                                if (!PlusWrap)
                                    IfrNvData->UefiPriorities[KeyValue-1]--;      // -1 because it was +1 by "+" key

                                Val1 = IfrNvData->UefiPriorities[KeyValue-1];    // current
                                Val2 = IfrNvData->UefiPriorities[KeyValue];    // next
                                IfrNvData->UefiPriorities[KeyValue-1] = Val2;
                                IfrNvData->UefiPriorities[KeyValue] = Val1;
                            }
                            else
                            {
                                if (!PlusWrap)
                                    IfrNvData->LegacyPriorities[KeyValue-1]--;      // -1 because it was +1 by "+" key

                                Val1 = IfrNvData->LegacyPriorities[KeyValue-1];    // current
                                Val2 = IfrNvData->LegacyPriorities[KeyValue];    // next
                                IfrNvData->LegacyPriorities[KeyValue-1] = Val2;
                                IfrNvData->LegacyPriorities[KeyValue] = Val1;
                            }
                    }
                }
                else
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                        IfrNvData->DualPriorities[Swap] = i; // not minus nor plus, just make no duplication
                    else
#endif
                        if (Uefi_Boot_item)
                            IfrNvData->UefiPriorities[Swap] = i; // not minus nor plus, just make no duplication
                        else
                            IfrNvData->LegacyPriorities[Swap] = i; // not minus nor plus, just make no duplication
                }
            }
#endif  //#if FIXED_BOOT_SWAP_POLICY == 1
#if FBO_DUAL_MODE
            if (Dual_Boot_item)
                TRACE((FBO_TRACE_LEVEL, "[FixedBoot] NEW DualPriorities[%d]=%d\n", Swap, IfrNvData->DualPriorities[Swap]));
            else
#endif
                if (Uefi_Boot_item)
                    TRACE((FBO_TRACE_LEVEL, "[FixedBoot] NEW UefiPriorities[%d]=%d\n", Swap, IfrNvData->UefiPriorities[Swap]));
                else
                    TRACE((FBO_TRACE_LEVEL, "[FixedBoot] NEW LegacyPriorities[%d]=%d\n", Swap, IfrNvData->LegacyPriorities[Swap]));
            break;
        }

    }

#if EFI_SPECIFICATION_VERSION>0x20000
    Status = HiiLibSetBrowserData(BufferSize, IfrNvData, &gSetupGuid, L"Setup");
    TRACE((FBO_TRACE_LEVEL, "FixedBootOrder.c HiiLibSetBrowserData (%r)\n", Status));
#endif
    return Status;
}

#if SMC_HII_BBS_SUFFIX_STRING_SUPPORT
EFI_STATUS
SmcHiiBbsSuffixString (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevPath,
  OUT CHAR16                   *Buffer,
  IN  UINTN                    BufferSize
  )
{
  UINT8                    DevPathCounts;
  UINT32                   AcpiHid;
  UINT32                   AcpiUid;
  UINT8                    PciDevNum;
  UINT8                    PciFunNum;
  UINT8                    IsNvmeDevPath;
  CHAR16                   TempBuffer[256];

  if (Buffer == NULL || BufferSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  MemSet (Buffer, BufferSize, 0);
  MemSet (TempBuffer, sizeof(CHAR16) * 256, 0);
  DevPathCounts = 0;
  AcpiHid       = 0xFFFFFFFF;
  AcpiUid       = 0xFFFFFFFF;
  PciDevNum     = 0xFF;
  PciFunNum     = 0xFF;
  IsNvmeDevPath = FALSE;

  while (!(IsDevicePathEnd (DevPath))) {   
    if (DevPath->Type == MESSAGING_DEVICE_PATH) {
      if (DevPathCounts > 0 && DevPathCounts < 5) {
        switch (DevPath->SubType) {
          case MSG_ATAPI_DP:
          case MSG_SCSI_DP:
          case MSG_FIBRECHANNEL_DP:
          case MSG_1394_DP:
          case MSG_I2O_DP:
          case MSG_USB_DP:
          case MSG_INFINIBAND_DP:
          case MSG_VENDOR_DP:
          case MSG_MAC_ADDR_DP:
//          case MSG_IPv4_DP:
//          case MSG_IPv6_DP:
          case MSG_UART_DP:
          case MSG_USB_CLASS_DP:
          case MSG_USB_WWID_DP:
          case MSG_DEVICE_LOGICAL_UNIT_DP:
          case MSG_SATA_DP:
          case MSG_ISCSI_DP:
          case MSG_VLAN_DP:
          case MSG_FIBRECHANNELEX_DP:
          case MSG_SASEX_DP:
          case MSG_NVME_NAMESPACE_DP:
            Swprintf (TempBuffer, L"%s/", TempBuffer);
            break;

          default:
            break;
        }
      } else if (DevPathCounts >= 5) {
        break;
      }
      
      switch (DevPath->SubType) {
        case MSG_ATAPI_DP:
        {
          UINT8 PrimarySecondary;
          UINT8 SlaveMaster;

          PrimarySecondary = ((ATAPI_DEVICE_PATH *) DevPath)->PrimarySecondary;
          SlaveMaster = ((ATAPI_DEVICE_PATH *) DevPath)->SlaveMaster;
          Swprintf (TempBuffer, L"%sATAPI,PS:%d,SM%d", TempBuffer, PrimarySecondary, SlaveMaster);
          DevPathCounts++;
        }
        break;
        
        case MSG_SCSI_DP:
        {
          UINT16 TargetId;

          TargetId = ((SCSI_DEVICE_PATH *) DevPath)->Pun;
          Swprintf (TempBuffer, L"%sSCSI,PUN:%d", TempBuffer, TargetId);
          DevPathCounts++;
        }
        break;
        
        case MSG_FIBRECHANNEL_DP:
        {
          UINT64 WorldWideName;
          
          WorldWideName = ((FIBRECHANNEL_DEVICE_PATH *) DevPath)->WWN;
          Swprintf (TempBuffer, L"%sFibre,WWN:0x%x", TempBuffer, WorldWideName);
          DevPathCounts++;
        }
        break;

        case MSG_1394_DP:
        {
          UINT64 GlobalUniqueId;
          
          GlobalUniqueId = ((F1394_DEVICE_PATH *) DevPath)->Guid;
          Swprintf (TempBuffer, L"%s1394,GUID:0x%x", TempBuffer, GlobalUniqueId);
          DevPathCounts++;
        }
        break;

        case MSG_I2O_DP:
        {
          UINT32 Tid;
 
          Tid = ((I2O_DEVICE_PATH *) DevPath)->Tid;
          Swprintf (TempBuffer, L"%sI2O,TID:0x%x", TempBuffer, Tid);
          DevPathCounts++;
        }
        break;
        
        case MSG_USB_DP:
        {
          UINT8 ParentPortNumber;
          
          ParentPortNumber = ((USB_DEVICE_PATH *) DevPath)->ParentPortNumber;
          Swprintf (TempBuffer, L"%sUSB,Port:%d", TempBuffer, ParentPortNumber);
          DevPathCounts++;
        }
        break;

        case MSG_INFINIBAND_DP:
        {
          UINT64 DeviceId;

          DeviceId = ((INFINIBAND_DEVICE_PATH *) DevPath)->DeviceId;
          Swprintf (TempBuffer, L"%sInfiniBand,DID:0x%x", TempBuffer, DeviceId);
          DevPathCounts++;
        }
        break;

        case MSG_VENDOR_DP:
        {
          EFI_GUID VendorGuid;

          CopyMem (&VendorGuid, &(((VENDOR_DEVICE_PATH *) DevPath)->Guid), sizeof (EFI_GUID));
          Swprintf (TempBuffer, L"%sVendor,GUID:%g", TempBuffer, &VendorGuid);
          DevPathCounts++;
        }
        break;

        case MSG_MAC_ADDR_DP:
        {
          EFI_MAC_ADDRESS	Mac;

          CopyMem (&Mac, &(((MAC_ADDR_DEVICE_PATH *) DevPath)->MacAddress), sizeof (EFI_MAC_ADDRESS));
          Swprintf (TempBuffer, L"%sMAC,Address:%02x%02x%02x%02x%02x%02x", TempBuffer, Mac.Addr[0], Mac.Addr[1], Mac.Addr[2], Mac.Addr[3], Mac.Addr[4], Mac.Addr[5]);
          DevPathCounts++;
        }
        break;

//        case MSG_IPv4_DP:
//        {
//          Swprintf (Buffer, L"%sIPv4", Buffer);
//          DevPathCounts++;
//        }
//        break;

//        case MSG_IPv6_DP:
//        {
//          Swprintf (Buffer, L"%sIPv6", Buffer);
//          DevPathCounts++;
//        }
//        break;

        case MSG_UART_DP:
        {
          UINT64 BaudRate;

          BaudRate = ((UART_DEVICE_PATH *) DevPath)->BaudRate;
          
          Swprintf (TempBuffer, L"%sUART,BR:%d", TempBuffer, BaudRate);
          DevPathCounts++;
        }
        break;

        case MSG_USB_CLASS_DP:
        {
          UINT16 VendorId;
          UINT16 ProductId;
          
          VendorId = ((USB_CLASS_DEVICE_PATH *) DevPath)->VendorId;
          ProductId = ((USB_CLASS_DEVICE_PATH *) DevPath)->ProductId;

          Swprintf (TempBuffer, L"%sUSB Class,VID:0x%x,PID:0x%x", TempBuffer, VendorId, ProductId);
          DevPathCounts++;
        }
        break;

        case MSG_USB_WWID_DP:
        {
          UINT16 VendorId;
          UINT16 ProductId;
          
          VendorId = ((USB_WWID_DEVICE_PATH *) DevPath)->VendorId;
          ProductId = ((USB_WWID_DEVICE_PATH *) DevPath)->ProductId;

          Swprintf (TempBuffer, L"%sUSB WWID,VID:0x%x,PID:0x%x", TempBuffer, VendorId, ProductId);
          DevPathCounts++;
        }
        break;

        case MSG_DEVICE_LOGICAL_UNIT_DP:
        {
          UINT8 LogicalUnitNumber;

          LogicalUnitNumber = ((DEVICE_LOGICAL_UNIT_DEVICE_PATH *) DevPath)->Lun;
          Swprintf (TempBuffer, L"%sLogical,LUN:%d", TempBuffer, LogicalUnitNumber);
          DevPathCounts++;
        }

        break;
        
        case MSG_SATA_DP:
        {
          UINT16 HbaPortNumber;
          
          HbaPortNumber = ((SATA_DEVICE_PATH *) DevPath)->HBAPortNumber;
          Swprintf (TempBuffer, L"%sSATA,Port:%d", TempBuffer, HbaPortNumber);
          DevPathCounts++;
        }
        break;


        case MSG_ISCSI_DP:
        {
          UINT64 LogicalUnitNumber;
          
          LogicalUnitNumber = ((ISCSI_DEVICE_PATH *) DevPath)->Lun;
          Swprintf (TempBuffer, L"%siSCSI,LUN:0x%x", TempBuffer, LogicalUnitNumber);
          DevPathCounts++;
        }
        break;
 
        case MSG_VLAN_DP:
        {
          UINT16 VlanId;
          
          VlanId = ((VLAN_DEVICE_PATH *) DevPath)->VlanId;
          Swprintf (TempBuffer, L"%sVLAN,ID:%d", TempBuffer, VlanId);
          DevPathCounts++;
        }
        break;

        case MSG_FIBRECHANNELEX_DP:
        {
          UINT64 WorldWideName;
          
          WorldWideName = ((FIBRECHANNEL_DEVICE_PATH *) DevPath)->WWN;
          Swprintf (TempBuffer, L"%sFibre Ex,WWN:0x%x", TempBuffer, WorldWideName);
          DevPathCounts++;
        }
        break;

        case MSG_SASEX_DP:
        {
          UINT64 LogicalUnitNumber;

          CopyMem (&LogicalUnitNumber, (((SASEX_DEVICE_PATH *) DevPath)->Lun), 8 * sizeof (UINT8));
          Swprintf (TempBuffer, L"%sSas Ex,LUN:0x%x", TempBuffer, LogicalUnitNumber);
          DevPathCounts++;
        }
        break; 
        
        case MSG_NVME_NAMESPACE_DP:
        {
          UINT32 NsidNumber;
          
          NsidNumber = ((NVME_NAMESPACE_DEVICE_PATH *) DevPath)->NamespaceId;
          Swprintf (TempBuffer, L"%sNVMe,NSID:0x%x", TempBuffer, NsidNumber);
          IsNvmeDevPath = TRUE;
          DevPathCounts++;
        }
        break;
       
        default:
          break;
      }
    } else if (DevPath->Type == HARDWARE_DEVICE_PATH) {
      switch (DevPath->SubType) {
        case HW_PCI_DP:
        {
          if (PciDevNum == 0xFF && PciFunNum == 0xFF) {
            PciDevNum = ((PCI_DEVICE_PATH *) DevPath)->Device;
            PciFunNum = ((PCI_DEVICE_PATH *) DevPath)->Function;
          }
        }
        break;

        default:
          break;
      }
    } else if ((DevPath->Type == ACPI_DEVICE_PATH) && 
               (DevPath->SubType == ACPI_DP)) {
      AcpiHid = (((ACPI_HID_DEVICE_PATH *) DevPath)->HID) >> 16;     
      if ((AcpiHid == 0x0A03 || AcpiHid == 0x0A08) && AcpiUid == 0xFFFFFFFF) {
        AcpiUid = ((ACPI_HID_DEVICE_PATH *) DevPath)->UID;
      }
    }
    
    DevPath = NextDevicePathNode(DevPath);
  }

  if (IsNvmeDevPath) {
    Swprintf (Buffer, L"PciRoot%d/PCI,Dev:%d,Fun:%d/%s", AcpiUid, PciDevNum, PciFunNum, TempBuffer);
  } else {
    Swprintf (Buffer, L"%s", TempBuffer);
  }

  if (DevPathCounts == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
SmcChangeLegacyDeviceOptionName (
  IN     EFI_HANDLE Handle, 
  IN OUT CHAR16     *DevName
  )
{
  EFI_DEVICE_PATH_PROTOCOL *DevPath = NULL;
	EFI_STATUS               Status;
  CHAR16                   DevPathString[256];

  if (Handle == NULL || DevName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = pBS->HandleProtocol (Handle, &gEfiDevicePathProtocolGuid, (VOID**) &DevPath);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  
  Status = SmcHiiBbsSuffixString (DevPath, DevPathString, sizeof(CHAR16) * 256);
  if (!EFI_ERROR (Status)) {
    Swprintf (DevName, L"%s(%s)", DevName, DevPathString);
  }
  
  return EFI_SUCCESS;
}

CHAR16 mNameString[256];

EFI_STATUS
SmcChangeUefiDeviceOptionName (
  IN     EFI_LOAD_OPTION *BootOption, 
  IN OUT VOID            **DevName
  )
{
  EFI_DEVICE_PATH_PROTOCOL *DevPath = NULL;
  EFI_DEVICE_PATH_PROTOCOL *FsDevPath = NULL;
  EFI_DEVICE_PATH_PROTOCOL *FsDevPathHead = NULL;
  EFI_DEVICE_PATH_PROTOCOL *RefDevPath = NULL;
	EFI_STATUS               Status;
  CHAR16                   DevPathString[256];
  UINT8                    *FilePathList;
  UINTN                    NameSize;
  CHAR16                   *pDevName;
  EFI_HANDLE               *HandleBuffer = NULL;
  UINTN                    HandleCount;
  UINTN                    Index;

  if (BootOption == NULL || *DevName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MemSet (mNameString, sizeof(CHAR16) * 256, 0);
  NameSize = (Wcslen ((CHAR16 *) (BootOption + 1)) + 1) * sizeof(CHAR16);
  FilePathList = (UINT8 *)(BootOption + 1) + NameSize;
  RefDevPath = (EFI_DEVICE_PATH_PROTOCOL *) FilePathList;
  pDevName = (CHAR16 *) *DevName;

  DevPath = (EFI_DEVICE_PATH_PROTOCOL *) FilePathList;
  while (!(IsDevicePathEnd (DevPath))) {   
    if (DevPath->Type == MEDIA_DEVICE_PATH && DevPath->SubType == MEDIA_HARDDRIVE_DP) {
      Status = pBS->LocateHandleBuffer (
                      ByProtocol,
                      &gEfiSimpleFileSystemProtocolGuid,
                      NULL,
                      &HandleCount,
                      &HandleBuffer
                      );
      if (EFI_ERROR (Status)) {
        break;
      }
      
      for (Index = 0; Index < HandleCount; Index++) {
        Status = pBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID**) &FsDevPath);
        if (EFI_ERROR (Status)) {
          continue;
        }
        
        FsDevPathHead = FsDevPath;
        while (!(IsDevicePathEnd (FsDevPath))) {
          if (FsDevPath->Type == MEDIA_DEVICE_PATH && FsDevPath->SubType == MEDIA_HARDDRIVE_DP) {
            if (MemCmp (FsDevPath, DevPath, sizeof (HARDDRIVE_DEVICE_PATH)) == 0) {
              RefDevPath = FsDevPathHead;
              break;
            }
          }
          
          FsDevPath = NextDevicePathNode (FsDevPath);
        }
      }
    }
    
    DevPath = NextDevicePathNode (DevPath);
  }
  
  Status = SmcHiiBbsSuffixString (RefDevPath, DevPathString, sizeof(CHAR16) * 256);
  if (!EFI_ERROR (Status)) {
    Swprintf (mNameString, L"%s(%s)", pDevName, DevPathString);
    *DevName = (VOID *) mNameString;
  }

  return EFI_SUCCESS;
}
#endif

/**
    Each group device name Hii String Update, and fixed HiiDB Oneof
    Data.

    @retval EFI_SUCCESS - SUCCESS
**/
EFI_STATUS
InitLegacyGroupDefault
(
  VOID
)
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    VOID *DevOrderBuffer = NULL;
    UINT16 i, j, DevMapIndex;
    UINTN BBSNamePtr;
    UINTN Size = 0;
    UINT8 DeviceItem = 0, DisableItem = 0;
    CHAR16 DeviceName[0x100+1];
    HII_DEVICE_INFO *pHiiDevTokenData = NULL;
    UINTN HiiDevDataSize = 0;

    if (LoadedDefault)
    {
        Status = GetEfiVariable(L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid, NULL, &Size, &DevOrderBuffer);
        AdjustLegacyDevOrder(&DevOrderBuffer, &Size);
    }
    else
        Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER, Device))
    {
        LEGACY_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiLegacyDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        HiiDevOrder = pHiiDevTokenData;
        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;
            DeviceItem = 0;
            DisableItem = 0;

            if(DevOrder->Type == BBS_UNKNOWN) continue;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            DevMapIndex = SearchDevMapByType(pFBOLegacyDevMap, DevOrder->Type);
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {
                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                j = DevOrder->Device[i] & FBO_LEGACY_ORDER_MASK; // high byte is used to indicate if it is disabled

                if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                {
                    BBSNamePtr = (UINTN)(((UINTN)BbsTable[j].DescStringSegment << 4)
                                         + BbsTable[j].DescStringOffset);

                    Swprintf(DeviceName, L"%a", BBSNamePtr);
                    ChangeLegacyDeviceOptionName((EFI_HANDLE)BbsTable[j].IBV1,(CHAR16*)&DeviceName) ;//(EIP143657+)
#if SMC_HII_BBS_SUFFIX_STRING_SUPPORT
                    SmcChangeLegacyDeviceOptionName ((EFI_HANDLE) BbsTable[j].IBV1,(CHAR16*)&DeviceName);
#endif
                    ASSERT(HiiDevOrder->StrToken[i] > 0x100) ;
                    if (HiiDevOrder->StrToken[i] > 0x100)
                    {
                        // Hii index checking.
                        InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", DeviceName);
                        SyncLegacyStringWithGroupFormHii((UINT16)((UINT8*)(&HiiDevOrder->StrToken[i]) - (UINT8*)pHiiDevTokenData), DeviceName) ;
                        TRACE((FBO_TRACE_LEVEL, "BBS DEVICE(%d):%S \n", i, DeviceName));
                    }
                }
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
                if (DevOrder->Device[i] & FBO_LEGACY_DISABLED_MASK)   // for BBS indexes in LegacyDevOrder, high byte = 0xff means disabled
                {
                    TypeLegacyDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                    DisableItem++;
                }
                else
#endif
                {
                    TypeLegacyDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = DeviceItem + DisableItem;
                    DeviceItem++;
                }
                if ((DeviceItem + DisableItem) >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;

            }



            TRACE((FBO_TRACE_LEVEL, "[FixedBoot] DeviceItem=%d DisableItem=%d \n", DeviceItem, DisableItem));

#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
            if (DeviceItem)
                UpdateLegacyDeviceHiiString(DevOrder->Type, 0);
            else
                UpdateLegacyDeviceHiiString(DevOrder->Type, FIXED_BOOT_ORDER_SUBMENU_MAX_NUM);       //no device, show type
#endif

            TypeLegacyDeviceCount[DevMapIndex] = DeviceItem;
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            if (DisableItem)
                TypeLegacyDeviceCount[DevMapIndex] += DisableItem;
#endif
            TRACE((FBO_TRACE_LEVEL, "[FixedBoot] Type(%x)DevCount=%x\n", DevMapIndex, TypeLegacyDeviceCount[DevMapIndex]));
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);
#endif //#if CSM_SUPPORT
    return EFI_SUCCESS;
}

/**
    Each group device name Hii String Update, and fixed HiiDB Oneof
    Data.

    @retval EFI_STATUS
**/
EFI_STATUS
InitUefiGroupDefault
(
  VOID
)
{
    EFI_STATUS      Status;
    EFI_GUID        FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    VOID            *DevOrderBuffer = NULL;
    UINT16          i, j, DevMapIndex;
    UINTN           Size = 0;
    UINT8           DeviceItem = 0, DisableItem = 0;
    HII_DEVICE_INFO     *pHiiDevTokenData = NULL;
    UINTN           HiiDevDataSize = 0;

    if (LoadedDefault)
        Status = GetEfiVariable(L"DefaultUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);
    else
        Status = GetEfiVariable(L"UefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER, Device))
    {
        UEFI_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiUefiDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;
            DeviceItem = 0;
            DisableItem = 0;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            DevMapIndex = SearchDevMapByType(pFBOUefiDevMap, DevOrder->Type);
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {

                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                EFI_LOAD_OPTION *BootOption = NULL;

                j = DevOrder->Device[i] & FBO_UEFI_ORDER_MASK; // highest byte is used to indicate if it is disabled

                BootOption = GetUEFIBootName(j);
                if (BootOption)
                {
                    if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                    {
                        CHAR16 *NameStr;
                        NameStr = (CHAR16*)((UINT8*)BootOption + sizeof(EFI_LOAD_OPTION));
                        ChangeUefiDeviceOptionName(BootOption,&NameStr) ;
#if SMC_HII_BBS_SUFFIX_STRING_SUPPORT
                        SmcChangeUefiDeviceOptionName (BootOption,&NameStr);
#endif
                        ASSERT(HiiDevOrder->StrToken[i] > 0x100) ;
                        if (HiiDevOrder->StrToken[i] > 0x100)
                        {
                            // Hii index checking.
                            InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", NameStr);
                            SyncUefiStringWithGroupFormHii((UINT16)((UINT8*)(&HiiDevOrder->StrToken[i]) - (UINT8*)pHiiDevTokenData), NameStr) ;
                            TRACE((FBO_TRACE_LEVEL, "UEFI DEVICE(%d):%S \n", i, NameStr));
                        }
                    }
                    pBS->FreePool(BootOption);
                }
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
                if (DevOrder->Device[i] & FBO_UEFI_DISABLED_MASK)   // for UefiDevOrder, highest byte = 0xff means disabled
                {
                    TypeUefiDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                    DisableItem++;
                }
                else
#endif
                {
                    TypeUefiDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = DeviceItem + DisableItem;
                    DeviceItem++;
                }
                if ((DeviceItem + DisableItem) >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            }
            TRACE((FBO_TRACE_LEVEL, "[FixedBoot] DeviceItem=%d DisableItem=%d \n", DeviceItem, DisableItem));

#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
            if (DeviceItem)
                UpdateUefiDeviceHiiString(DevOrder->Type, 0);
            else
                UpdateUefiDeviceHiiString(DevOrder->Type, FIXED_BOOT_ORDER_SUBMENU_MAX_NUM);     //no device, show type
#endif

            TypeUefiDeviceCount[DevMapIndex] = DeviceItem;
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            if (DisableItem)
                TypeUefiDeviceCount[DevMapIndex] += DisableItem;
#endif
            TRACE((FBO_TRACE_LEVEL, "[FixedBoot] Type(%x)DevCount=%x\n", DevMapIndex, TypeUefiDeviceCount[DevMapIndex]));
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);

    return EFI_SUCCESS;
}
/**
    Get Supported Language from List

    @param LanguageList - Language List
    @retval Language - Language supported
**/
EFI_STATUS
GetSupportedLanguage
(
    IN  EFI_HII_HANDLE   HiiHandle,
    IN  CHAR8*           LanguageList,
    OUT CHAR8**          SupportedLang
)
{
    CHAR8       *TempLanguage = LanguageList, *pLanguage = LanguageList;
    UINTN       Size = 0x100;
    CHAR16      BootOptionStr[0x100+1];
    EFI_STATUS  Status;
    
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid,
                                     NULL, 
                                     (VOID **) & pHiiString);
        if (EFI_ERROR(Status))
            return Status;
    }
    
    while (*pLanguage != 0)
    {
        if((*pLanguage == ';'))
        {
            CHAR8    TempChar = *pLanguage;
            
            *pLanguage = 0;
            Status = pHiiString->GetString(pHiiString, 
                                           TempLanguage, 
                                           HiiHandle, 
                                           STRING_TOKEN(STR_BOOT_OPTION), 
                                           BootOptionStr, &Size, NULL);
            
            if(!EFI_ERROR(Status))
            {
                // Supported Language found, dont need pLanguage anymore
                Status = pBS->AllocatePool(EfiBootServicesData, 
                                           Strlen(TempLanguage) + 1, 
                                           SupportedLang);
                // If fail, return Orginal pointer
                if(EFI_ERROR(Status))
                {
                    *pLanguage = TempChar;
                    return Status;
                }
                
                pBS->CopyMem(*SupportedLang, 
                             TempLanguage, 
                             Strlen(TempLanguage) + 1);
                *pLanguage = TempChar;
                return EFI_SUCCESS;
            }
            *pLanguage = TempChar;
            TempLanguage = pLanguage + 1;
        }
        pLanguage++;
    }
    
    Status = pHiiString->GetString(
                         pHiiString, 
                         TempLanguage, 
                         HiiHandle, 
                         STRING_TOKEN(STR_BOOT_OPTION), 
                         BootOptionStr, 
                         &Size, 
                         NULL);
    if(EFI_ERROR(Status))
        return Status;

    Status = pBS->AllocatePool(EfiBootServicesData, Strlen(TempLanguage) + 1, SupportedLang);
    if(EFI_ERROR(Status))
        return Status;
    
    pBS->CopyMem(*SupportedLang, TempLanguage, Strlen(TempLanguage) + 1);
    // The Last Language, just return the status
    return EFI_SUCCESS;
}
//(EIP101408+)>
/**
    Install multi-language for STR_BOOT_OPTION.

        
    @param HiiHandle - Hii Handle
    @param BootOptionNumber - BootOption Number
    
    @retval STRING_REF - String reference

**/
STRING_REF
HiiAddLanguageBootOptionString
(
  IN EFI_HII_HANDLE HiiHandle, 
  IN UINT16 BootOptionNumber
)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    CHAR8 *CurrentLanguage = NULL;
    CHAR8* OrgLanguages = NULL;
    UINTN LangSize = 0;
    BOOLEAN LastLanguage = FALSE;
    EFI_STRING_ID  StringId = 0;

    UINTN  Size = 0;
    CHAR16 BootOptionStr[0x100+1];
    CHAR16 BootStr[0x100+1];

    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) & pHiiString);
        if (EFI_ERROR(Status))
            return 0;
    }

    Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return 0;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status))
        return 0;

    // CurrentLanguage contains the individual language code inside Languages
    Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &CurrentLanguage);
    if (EFI_ERROR(Status))
        return 0;


    OrgLanguages = Languages;
    while (!LastLanguage)
    {
        UINTN lc = 0;

        // Copy language code to CurrentLanguage
        while (*Languages != ';' && *Languages != 0 && lc < LangSize)
        {
            CurrentLanguage[lc++] = *Languages;
            Languages++;
        }
        CurrentLanguage[lc] = 0;

        if (*Languages == 0)    // last language in language list
            LastLanguage = TRUE;

        Size = 0x100;
        Status = pHiiString->GetString(pHiiString, 
                                       CurrentLanguage, 
                                       HiiHandle, 
                                       STRING_TOKEN(STR_BOOT_OPTION), 
                                       BootOptionStr, &Size, NULL);
        if (EFI_ERROR(Status))   // Language not defined in .uni
        {
            CHAR8*  LangBuffer = NULL;
            // Get support in OrgLanguages
            Status = GetSupportedLanguage(HiiHandle, OrgLanguages, &LangBuffer);
            if(!EFI_ERROR(Status))
            {
                Status = pHiiString->GetString(
                         pHiiString, LangBuffer, HiiHandle, 
                         STRING_TOKEN(STR_BOOT_OPTION), 
                         BootOptionStr, &Size, NULL);
                pBS->FreePool(LangBuffer);
            }
            if (EFI_ERROR(Status))
            {
                Languages++;    // skip ';'
                continue;
            }
        }

        Swprintf(BootStr, BootOptionStr, BootOptionNumber);

        if (StringId == 0)
            Status = pHiiString->NewString(
                                 pHiiString, HiiHandle, &StringId, 
                                 CurrentLanguage, NULL, BootStr, NULL);
        else
            Status = pHiiString->SetString(
                                 pHiiString, HiiHandle, StringId, 
                                 CurrentLanguage, BootStr, NULL);
            
        Languages++;
        if (EFI_ERROR(Status))
        {
            pBS->FreePool(OrgLanguages);
            return 0;
        }
    }

    if (OrgLanguages)
        pBS->FreePool(OrgLanguages);

    if (CurrentLanguage)
        pBS->FreePool(CurrentLanguage);

    return StringId;
}
//<(EIP101408+)

/**
    Create legacy boot item device Hii Item data.

    @param HiiHandle - HiiHandle
    
    @retval EFI_STATUS

**/
EFI_STATUS
InitialLegacyDevHiiToken(EFI_HANDLE HiiHandle)
{
#if CSM_SUPPORT
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    UINTN DevOrderSize = 0, HiiDevInfoSize = 0;
    STRING_REF Strid = 0;
    HII_DEVICE_INFO *pHiiDevTokenData = NULL, *pHiiDevTokenData2 = NULL;
    LEGACY_DEVICE_ORDER *DevOrder = NULL, *DevOrderBuffer = NULL;
    
    if (pFBOHiiMap == NULL)
    {
        TRACE((FBO_TRACE_LEVEL, "pFBOHiiMap == NULL\n"));
        ASSERT(pFBOHiiMap);
    }
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status)) return Status ;
    }
    
    Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &DevOrderSize, &DevOrderBuffer);
    if (EFI_ERROR(Status) || DevOrderSize <= EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER,Device))  // any order existed?
        return Status;

    pHiiDevTokenData2 = pHiiDevTokenData = MallocZ(DevOrderSize);
    if (pHiiDevTokenData == NULL) return EFI_OUT_OF_RESOURCES;

    for (DevOrder = DevOrderBuffer ; 
         (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + DevOrderSize ; 
         DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        UINT16 i;

        if(DevOrder->Type == BBS_UNKNOWN) continue;

        pHiiDevTokenData->Type = DevOrder->Type;
        pHiiDevTokenData->Length = sizeof(pHiiDevTokenData->Length) + DEVORDER_COUNT(DevOrder) * sizeof(pHiiDevTokenData->StrToken);
                
        TRACE((-1, "pHiiDevTokenData->Length:%X",pHiiDevTokenData->Length)) ;
        //Create BootOption item string token ID.
        for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
        {
            if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            //
            // This string would be updated later by InitLegacyGroup.
            //
            Status = NewStringWithFirstFoundLanguage(HiiHandle, &Strid, L" ");
            if (EFI_ERROR(Status)) return Status;
            
            pHiiDevTokenData->StrToken[i] = Strid;
        }
        pHiiDevTokenData = (HII_DEVICE_INFO*)((UINT8*)pHiiDevTokenData + pHiiDevTokenData->Length + sizeof(pHiiDevTokenData->Type));
    }
    
    HiiDevInfoSize = (UINT16)((UINT8*)pHiiDevTokenData - (UINT8*)pHiiDevTokenData2);
    if (HiiDevInfoSize)
    {
        pRS->SetVariable(L"FixedHiiLegacyDevData",
                         &FixedBootOrderGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         HiiDevInfoSize,
                         pHiiDevTokenData2);
    }
    pBS->FreePool(pHiiDevTokenData2);
#endif
    return EFI_SUCCESS;
}


/**
    Create legacy boot item device Hii Item data.

    @param HiiHandle - HiiHandle
    
    @retval EFI_STATUS

**/
EFI_STATUS
InitialUefiDevHiiToken(EFI_HANDLE HiiHandle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    UINTN DevOrderSize = 0, HiiDevInfoSize = 0;
    STRING_REF Strid = 0;
    HII_DEVICE_INFO *pHiiDevTokenData = NULL, *pHiiDevTokenData2 = NULL;
    UEFI_DEVICE_ORDER *DevOrder = NULL, *DevOrderBuffer = NULL;
    
    if (pFBOHiiMap == NULL)
    {
        TRACE((FBO_TRACE_LEVEL, "pFBOHiiMap == NULL\n"));
        ASSERT(pFBOHiiMap);
    }
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status)) return Status ;
    }

     Status = GetEfiVariable(L"UefiDevOrder", &FixedBootOrderGuid, NULL, &DevOrderSize, &DevOrderBuffer);
    if (EFI_ERROR(Status) || DevOrderSize <= EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER,Device))  // any order existed?
        return Status;

    pHiiDevTokenData2 = pHiiDevTokenData = MallocZ(DevOrderSize);
    if (pHiiDevTokenData == NULL) return EFI_OUT_OF_RESOURCES;

    for (DevOrder = DevOrderBuffer ; 
         (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + DevOrderSize ; 
         DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        UINT16 i;

        pHiiDevTokenData->Type = DevOrder->Type;
        pHiiDevTokenData->Length = sizeof(pHiiDevTokenData->Length) + DEVORDER_COUNT(DevOrder) * sizeof(pHiiDevTokenData->StrToken);
                
        TRACE((-1, "pHiiDevTokenData->Length:%X",pHiiDevTokenData->Length)) ;
        //Create BootOption item string token ID.
        for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
        {
            if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            //
            // This string would be updated later by InitLegacyGroup.
            //
            Status = NewStringWithFirstFoundLanguage(HiiHandle, &Strid, L" ");
            if (EFI_ERROR(Status)) return Status;
            
            pHiiDevTokenData->StrToken[i] = Strid;
        }
        pHiiDevTokenData = (HII_DEVICE_INFO*)((UINT8*)pHiiDevTokenData + pHiiDevTokenData->Length + sizeof(pHiiDevTokenData->Type));
    }
    
    HiiDevInfoSize = (UINT16)((UINT8*)pHiiDevTokenData - (UINT8*)pHiiDevTokenData2);
    if (HiiDevInfoSize)
    {
        pRS->SetVariable(L"FixedHiiUefiDevData",
                         &FixedBootOrderGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         HiiDevInfoSize,
                         pHiiDevTokenData2);
    }
    pBS->FreePool(pHiiDevTokenData2);
    return EFI_SUCCESS;
}

/**
    This function called after setup utility loaded config defaults
    based on user input. change fixedbootorder display default
    devcie string.

    @param LoadDefaultString - Load Default Flag

    @retval EFI_STATUS

**/
EFI_STATUS 
load_default_string()
{
    TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c]Load Default String\n"));

    LoadedDefault = TRUE;

    //Initial Legacy Device HiiString name.
    InitLegacyGroup();
    //Initial Uefi Device HiiString.
    InitUefiGroup();

    return EFI_SUCCESS;
}

/**
    Elink to roll some specially previous values back for FixedBootOrder. 

    @param TseFixedBoot  FixedBoot variable from TSE. 

    @retval EFI_SUCCESS            Function works successfully
    @retval EFI_INVALID_PARAMETER  Parameter TseFixedBoot is NULL
    @return GetEfiVariable error

**/
EFI_STATUS 
load_previous_string
(
  VOID    *TseFixedBoot,
  BOOLEAN LoadedDefaultAndSaved
)
{
    FIXED_BOOT_SETUP     *FixedBoot = (FIXED_BOOT_SETUP*)TseFixedBoot;

    TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c]Load Previous String\n"));

    //
    // Setup data had been loaded back and needs to sync with
    // TypeUefiDeviceOptions and TypeUefiDeviceOptions variables
    // for correct operation of exchanging items.
    //
    if(FixedBoot)
    {
        MemCpy(TypeUefiDeviceOptions, FixedBoot->UefiDevice
                ,FIXED_BOOT_ORDER_GROUP_MAX_NUM * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM * sizeof(FixedBoot->UefiDevice[0]));
#if CSM_SUPPORT
        MemCpy(TypeLegacyDeviceOptions, FixedBoot->LegacyDevice
                ,FIXED_BOOT_ORDER_GROUP_MAX_NUM * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM * sizeof(FixedBoot->LegacyDevice[0]));
#endif
    }
    else
    {
        TRACE((-1, "FixedBootOrder.c(load_previous_string):: FixedBoot is NULL\n"));
        return EFI_INVALID_PARAMETER;
    }

    //
    // if loaded default before, it had initialized the strings
    // by default variable, and here to roll them back.
    //
    if(LoadedDefault && !LoadedDefaultAndSaved) // loaded default but not save yet.
    {
        InitFboUefiHiiString();
#if CSM_SUPPORT
        InitFboLegacyHiiString();
#endif

        LoadedDefault = FALSE;
    }
    
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
    {
        UINTN                Size = 0;
        EFI_STATUS           Status;
        UEFI_DEVICE_ORDER    *UefiGroup, *UefiDevOrder = NULL;

        //
        // If user loaded default without save, then it should not 
        // load previous values by default variable, otherwise it
        // fails to load previous valued at first boot.
        //
        if(LoadedDefaultAndSaved)
        {
            Status = GetEfiVariable(L"DefaultUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &UefiDevOrder);
        }
        else
        {
            Status = GetEfiVariable(L"OriUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &UefiDevOrder);
        }

        if(EFI_ERROR(Status))
            return Status;

        for (UefiGroup = UefiDevOrder
            ; (UINT8*)UefiGroup < (UINT8*)UefiDevOrder + Size
            ; UefiGroup = (UEFI_DEVICE_ORDER*)((UINT8*)UefiGroup
                    + UefiGroup->Length + sizeof(UefiGroup->Type)))
        {
            UpdateUefiDeviceHiiString(UefiGroup->Type, TypeUefiDeviceOptions[SearchDevMapByType(pFBOUefiDevMap, UefiGroup->Type)][0]);
        }

        if(UefiDevOrder)
            pBS->FreePool(UefiDevOrder);

#if CSM_SUPPORT

        {
            LEGACY_DEVICE_ORDER  *LegacyGroup, *LegacyDevOrder = NULL;

            if(LoadedDefaultAndSaved)
            {
                Status = GetEfiVariable(L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid, NULL, &Size, &LegacyDevOrder);
                AdjustLegacyDevOrder(&LegacyDevOrder, &Size);
            }
            else
            {
                Status = GetEfiVariable(L"OriFboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, (VOID**)&LegacyDevOrder);
            }

            if(EFI_ERROR(Status))
                    return Status;

            for (LegacyGroup = LegacyDevOrder
                ; (UINT8*)LegacyGroup < (UINT8*)LegacyDevOrder + Size
                ; LegacyGroup = (LEGACY_DEVICE_ORDER*)((UINT8*)LegacyGroup
                        + LegacyGroup->Length + sizeof(LegacyGroup->Type)))
            {
                UpdateLegacyDeviceHiiString(LegacyGroup->Type, TypeLegacyDeviceOptions[SearchDevMapByType(pFBOLegacyDevMap, LegacyGroup->Type)][0]);
            }

            if(LegacyDevOrder)
                pBS->FreePool(LegacyDevOrder);
        }
#endif //CSM_SUPPORT
    }
#endif //FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP

    return EFI_SUCCESS;
}

static EFI_FIXED_BOOT_ORDER_SETUP_PROTOCOL gFixedBootOrderSetupProtocol =
{
    load_default_string,
    load_previous_string,
    UpdateLegacyDeviceHiiString,
    UpdateUefiDeviceHiiString
};

/**
    Set Hii Skip FBO Module Flag

**/
VOID
HiiSetSkipFBOModuleFlag
(
  VOID
)
{
    EFI_STATUS Status;
    UINTN  Size = 0;
    UINT32 Attr;
    UINT8 *Flag = NULL ;
    Status = GetEfiVariable(L"CurrentSkipFboModule", &FixedBootOrderGuid, &Attr, &Size, &Flag);
    if (!EFI_ERROR(Status))
    {
        if (*Flag) gHiiSkipFBOModule = 1 ;
        else gHiiSkipFBOModule = 0 ;
        pBS->FreePool(Flag) ;
    }
    else
        gHiiSkipFBOModule = 0 ;
}

/**
    String Init function.

        
    @param HiiHandle - HiiHandle 
    @param Class - Class

    @retval EFI_STATUS

**/
EFI_STATUS FixedBootOrderStringInit(
    IN EFI_HII_HANDLE   HiiHandle,
    IN UINT16       Class
)
{
#if CSM_SUPPORT
    EFI_GUID gEfiLegacyBiosProtocolGuid = EFI_LEGACY_BIOS_PROTOCOL_GUID;
    EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios = NULL;
#endif
    EFI_GUID gSetupGuid = SETUP_GUID;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *pFBO = NULL;
    FIXED_BOOT_GROUP FixedBootGroup;
    UINT8 *pFixedBootGroup = (UINT8*) & FixedBootGroup;
    UINT8 i, j;
    UINT16 QuestionKey = FixedBootOrderStratKey;
    BOOLEAN FormSetFlag = FALSE ;
    EFI_GUID FboGroupFormFormSetGuid = FBO_GROUP_FORM_FORM_SET_GUID ;
    
    HiiSetSkipFBOModuleFlag() ;
    if (gHiiSkipFBOModule) return EFI_SUCCESS;
    // if (Class != FBO_STRING_INIT_FORM_SET_CLASS) return EFI_SUCCESS;
    
    for ( i = 0 ; FboSetupPageInform[i].FormSet ; i++ )
        if ( Class == FboSetupPageInform[i].FormSet ) 
            FormSetFlag = TRUE ;
    if (!FormSetFlag) return EFI_SUCCESS;
    
    pRS->SetVariable(
        L"FixedBootOrderHii",
        &FixedBootOrderGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(EFI_HII_HANDLE),
        &HiiHandle);

    TRACE((FBO_TRACE_LEVEL, "FixedBootOrderStringInit HiiHanlde = %x\n", HiiHandle));
    FixedBoot_HiiHandle = HiiHandle;

#if CSM_SUPPORT
    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
    if (!EFI_ERROR(Status))
    {

        Status = LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);
        if (EFI_ERROR(Status)) return Status;
    }
#endif
    Status = pBS->LocateProtocol(&FixedBootOrderGuid, NULL, &pFBO);
    if (!EFI_ERROR(Status))
    {
        pFBOLegacyDevMap = pFBO->GetLegacyDevMap();
        pFBOUefiDevMap = pFBO->GetUefiDevMap();
        pFBODualDevMap = pFBO->GetDualDevMap();
        pFBOHiiMap = pFBO->GetHiiMap();
        FBO_AdjustGroupName() ;
        //(EIP123284+)>
#if FBO_USE_BOOT_OPTION_NUMBER_UNI
        pBootOptionTokenMap = pFBO->GetBootOptionTokenMap();
#else
        pBootOptionTokenMap = NULL;
#endif
        //<(EIP123284+)
    }

    pBS->SetMem(TypeLegacyDeviceCount, sizeof(TypeLegacyDeviceCount), 0);
    pBS->SetMem(TypeUefiDeviceCount, sizeof(TypeUefiDeviceCount), 0);
    pBS->SetMem(&FixedBootGroup, sizeof(FIXED_BOOT_GROUP), 0);

#if CSM_SUPPORT
    //Initial Legacy Device HiiString name.
    InitialLegacyDevHiiToken(HiiHandle) ;
    InitLegacyGroup();
#endif
    
    //Initial Uefi Device HiiString.
    InitialUefiDevHiiToken(HiiHandle) ;
    InitUefiGroup();

#if FBO_RUNTIME_CALLBACK_REGISTRATION
    for( ; QuestionKey < LegacyForms_StartKey ; QuestionKey++)
    {
        Status = AmiSetupRegisterCallbackHandler(QuestionKey, FixedBootOrderFormCallBack); 
        if(EFI_ERROR(Status))
            TRACE((FBO_TRACE_LEVEL, "Register Setup Callback Error at Key : %X\n", LegacyBootItemKey + i));
    }
#endif

    //Set all types device count and set variable FixedBootGroup.
    {
        UINT16 DevMapCount;
        UINT16 HiiDevIndex;
        UINT16 DeviceCountOffset;


        DevMapCount = GetDevMapDataCount(pFBOLegacyDevMap);
        for (i = 0; i < DevMapCount; i++)
        {
            HiiDevIndex = SearchHiiMapByType(pFBOLegacyDevMap[i].DevType);
            if (HiiDevIndex == 0xffff) continue ;
            if (pFBOHiiMap[HiiDevIndex].FormID == 0) continue;  //no group submenu
            DeviceCountOffset = pFBOHiiMap[HiiDevIndex].DeviceCountOffset;
            pFixedBootGroup[DeviceCountOffset] = TypeLegacyDeviceCount[i];
        }

        DevMapCount = GetDevMapDataCount(pFBOUefiDevMap);
        for (i = 0; i < DevMapCount; i++)
        {
            HiiDevIndex = SearchHiiMapByType(pFBOUefiDevMap[i].DevType);
            if (HiiDevIndex == 0xffff) continue ;
            if (pFBOHiiMap[HiiDevIndex].FormID == 0) continue;  //no group submenu
            DeviceCountOffset = pFBOHiiMap[HiiDevIndex].DeviceCountOffset;
            pFixedBootGroup[DeviceCountOffset] = TypeUefiDeviceCount[i];
        }

        pRS->SetVariable(
            L"FixedBootGroup",
            &gSetupGuid,
            BOOT_NV_ATTRIBUTES,
            sizeof(FIXED_BOOT_GROUP),
            &FixedBootGroup);

    }

    //Initial ALL "FixedBoot" data.
    {
        FIXED_BOOT_SETUP pFixedBoot;

        for (i = 0; i < FIXED_BOOT_ORDER_GROUP_MAX_NUM; i++)
            for (j = 0; j < FIXED_BOOT_ORDER_SUBMENU_MAX_NUM; j++)
                pFixedBoot.LegacyDevice[(i*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)+j] = TypeLegacyDeviceOptions[i][j];
        for (i = 0; i < FIXED_BOOT_ORDER_GROUP_MAX_NUM; i++)
            for (j = 0; j < FIXED_BOOT_ORDER_SUBMENU_MAX_NUM; j++)
                pFixedBoot.UefiDevice[(i*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)+j] = TypeUefiDeviceOptions[i][j];

        Status = pRS->SetVariable(L"FixedBoot",
                                  &FboGroupFormFormSetGuid,
#if FboSce_SUPPORT
                                  VARIABLE_ATTRIBUTES,
#else
                                  BOOT_NV_ATTRIBUTES,
#endif
                                  sizeof(pFixedBoot),
                                  &pFixedBoot);
    }
    
    {
        EFI_HANDLE Handle = NULL;
        EFI_GUID FixedBootOrderSetupGuid = FIXED_BOOT_ORDER_SETUP_GUID;

        Status = pBS->InstallProtocolInterface(
                     &Handle,
                     &FixedBootOrderSetupGuid,
                     EFI_NATIVE_INTERFACE,
                     &gFixedBootOrderSetupProtocol);
    }
    return Status;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
