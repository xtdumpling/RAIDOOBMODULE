//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file FboGroupForm.c

    This file resolve group form call back and event.
*/
#include <AmiDxeLib.h>
#include <Protocol/HiiUtilities.h>
#include "../FixedBootOrder.h"
#include "FixedBootOrderHii.h"
#include "Token.h"
#include <setup.h>

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

#if CSM_SUPPORT
#include <Protocol/LegacyBios.h>
#endif

typedef struct
{
    UINT32 Type;
    UINT16 Length;
    UINT16 Device[1];
} LEGACY_DEVICE_ORDER;

EFI_STATUS FboGroupFormCallback(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest) ;


static EFI_GUID gFixedBootOrderGuid = FIXED_BOOT_ORDER_GUID ;
static EFI_GUID gFboGroupFormFormSetGuid = FBO_GROUP_FORM_FORM_SET_GUID;

EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = {NULL, NULL, FboGroupFormCallback};
CALLBACK_INFO SetupCallBack[] =
{
    { &gFboGroupFormFormSetGuid, &CallBack, FBO_GROUP_FORM_FORM_SET_CLASS, 0, 0},
};


FBODevMap *gFboLegacyDevMap = NULL;
FBODevMap *gFboUefiDevMap = NULL;
FBOHiiMap *gFboHiiMap = NULL;
UINT16 *gBootOptionTokenMap = NULL;

FIXED_BOOT_GROUP *gFixedBootGroup = NULL;
FIXED_BOOT_SETUP *gFixedBoot = NULL;

        
/**
    Get the FBODevMap data amount.

    @param FBODevMap FBODevMap

    @retval UINT16   Number of FBODevMap
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
    Search HiiMap data index by device type.

    @param DevType  Device Type want to Search

    @retval UINT16  Index of the Device Type
    @note  None
**/
UINT16 SearchHiiMapByType(UINT16 DevType)
{
    UINT16 i = 0;

    do
    {
        if (gFboHiiMap[i].DevType == 0) break;  //end of data
        if (gFboHiiMap[i].DevType == DevType)
            return i;
        i++;
    }
    while (1);

    return 0xffff;
}

/**
    Search DevMap data index by device type.

    @param  FBODevMap   Device Map
    @param  DevType     Device Type want to search 

    @retval UINT16      Index of Device Type in Device Map
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
    Avoid each group boot priorities from being duplicated to each
    other by using INTERACTIVE callback.

    @param NvRamBootDevice - 
    @param OldBootDevice - 
    @param DeviceIndex - 
    @param DeviceCount - 

    @retval EFI_STATUS - 
**/
EFI_STATUS 
GroupkeyProcess
(
  IN OUT UINT8 *NewBootDevice, 
  IN UINT8 *OldBootDevice, 
  IN OUT UINT8 DeviceIndex, 
  IN UINT8 DeviceCount
)
{
    UINTN i, j;
    UINT8 NewVal = NewBootDevice[DeviceIndex];
    UINT8 OldVal = OldBootDevice[DeviceIndex];


    for (i = 0; i < DeviceCount ; i++)
        TRACE((FBO_TRACE_LEVEL, "%d ", NewBootDevice[i]));

    TRACE((FBO_TRACE_LEVEL, "\n"));
    
    // Prepare For a correct value for Disabled Item
    if(OldVal == FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
    {
        if(NewVal == 0)
        {
            // + is pressed
            for(i = 0 ; i < DeviceCount ; i ++)
            {
                for(j = 0 ; j < DeviceCount ; j++)
                {
                    if(j == DeviceIndex)
                        continue;
                    if(i == NewBootDevice[j])
                        break;                        
                }
                if(j == DeviceCount)
                    break;
            }
        }else
        {
            // - is pressed
            for(i = DeviceCount - 1 ; i >= 0  ; i--)
            {
                for(j = 0 ; j < DeviceCount ; j++)
                {
                    if(j == DeviceIndex)
                        continue;
                    if(i == NewBootDevice[j])
                        break;    
                }
                if(j == DeviceCount)
                    break;
            }
        }
        NewBootDevice[DeviceIndex] = (UINT8)i;
        NewVal = (UINT8)i;
    }

    if(OldVal == NewVal)
    {
        return EFI_SUCCESS;
    }
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
    else if( OldVal == FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
    {
        //
        //  Disabled to Enabled
        //
        for(i = 0; i < DeviceIndex; i++)
        {
            //
            //  Duplicated value
            //
            if(OldBootDevice[i] == NewVal)
            {
                NewBootDevice[DeviceIndex] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                break;
            }

            //
            //  Find first disabled device then exchange
            //
            if(NewBootDevice[i] == FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
            {
                NewBootDevice[i] = NewVal;
                NewBootDevice[DeviceIndex] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                break;
            }
        }
    }
    else if(NewVal == FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
    {
        //
        //  Enabled to Disabled
        //

        UINTN j = DeviceIndex;
        for(i = j + 1; i < DeviceCount; i++)
        {
            //
            //  Move disabled one to last step by step.
            //
            if(NewBootDevice[i] != FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
            {
                NewBootDevice[j] = NewBootDevice[i];
                NewBootDevice[i] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                j = i;
            }

        }
    }
#endif
    else
    {
        //
        //  Both are not equal and disabled.
        //
        for(i = 0; i < DeviceCount; i++)
        {
            if(i == DeviceIndex)
                continue;
            if(NewVal == NewBootDevice[i])
                NewBootDevice[i] = OldVal;
        }
    }
            

    for(i=0;i< DeviceCount ;i++)
    {
        OldBootDevice[i] = NewBootDevice[i];
        TRACE((-1,"%d ",NewBootDevice[i]));
    }
    TRACE((FBO_TRACE_LEVEL, "\n"));

    return EFI_SUCCESS;
}

/**
    New the string with language that was first found.

    @param Handle  
    @param StrId 
    @param String 

    @retval EFI_STATUS

    @note  None
**/
EFI_STATUS 
NewStringWithFirstFoundLanguage(
  IN EFI_HANDLE Handle, 
  IN EFI_STRING_ID* StrId, 
  IN CHAR16 *String)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    UINTN  i;
    UINTN LangSize = 0;
    EFI_HII_STRING_PROTOCOL *pHiiString ;
    //
    // Get Hii String Protocol
    //
    Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
    if (EFI_ERROR(Status)) return Status;

    //
    // Get Languages
    //
    Status = pHiiString->GetLanguages(pHiiString, Handle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return Status;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, Handle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status)) {
        return Status;
    }

    // Search for ';'
    for(i = 0 ; (i < LangSize) && (*(Languages + i) != ';') ; i ++);                
    if(i != LangSize)
        // Find ';' , set a null-terminated
        *(Languages + i) = 0;

    Status = pHiiString->NewString(pHiiString, Handle, StrId, Languages, NULL, String, NULL);
    pBS->FreePool(Languages);

    return Status;
}

/**
    Get Supported Language from List

    @param Handle
    @param LanguageList Language List
    @retval Language    Language supported
**/
EFI_STATUS
GetSupportedLanguage(
    IN  EFI_HII_HANDLE   Handle,
    IN  CHAR8*           LanguageList,
    OUT CHAR8**          SupportedLang)
{
    CHAR8       *TempLanguage = LanguageList, *pLanguage = LanguageList;
    UINTN       Size = 0x100;
    CHAR16      BootOptionStr[0x100+1];
    EFI_STATUS  Status;
    
    EFI_HII_STRING_PROTOCOL *pHiiString ;
    
    Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
    if (EFI_ERROR(Status))return Status;
    
    while (*pLanguage != 0)
    {
        if((*pLanguage == ';'))
        {
            CHAR8    TempChar = *pLanguage;
            
            *pLanguage = 0;
            Status = pHiiString->GetString(pHiiString, 
                                           TempLanguage, 
                                           Handle, 
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
                         Handle, 
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

/**
    Install multi-language for STR_BOOT_OPTION.

    @param Handle           Hii Handle
    @param BootOptionNumber BootOption Number
    
    @retval STRING_REF      String reference
**/
STRING_REF
HiiAddLanguageBootOptionString(
  IN EFI_HII_HANDLE Handle, 
  IN UINT16 BootOptionNumber)
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

    EFI_HII_STRING_PROTOCOL *pHiiString ;
    
    Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &pHiiString);
    if (EFI_ERROR(Status)) return 0;

    Status = pHiiString->GetLanguages(pHiiString, Handle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return 0;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, Handle, Languages, &LangSize);
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
        int lc = 0;

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
                                       Handle, 
                                       STRING_TOKEN(STR_BOOT_OPTION), 
                                       BootOptionStr, &Size, NULL);
        if (EFI_ERROR(Status))   // Language not defined in .uni
        {
            CHAR8*  LangBuffer = NULL;
            // Get support in OrgLanguages
            Status = GetSupportedLanguage(Handle, OrgLanguages, &LangBuffer);
            if(!EFI_ERROR(Status))
            {
                Status = pHiiString->GetString(
                         pHiiString, LangBuffer, Handle, 
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
                                 pHiiString, Handle, &StringId, 
                                 CurrentLanguage, NULL, BootStr, NULL);
        else
            Status = pHiiString->SetString(
                                 pHiiString, Handle, StringId, 
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

/**
    Copy Hii String to new Handle

    @param  OrgHandle       Source Handle
    @param  Org             Source
    @param  NewHandle       Destination Handle 
    @param  New             Destination
    @param  HiiTokenSize    Source String Token Count
    
    @retval New HII string with new Handle
**/
VOID
CopyHiiStringToNewHandle(
    IN  EFI_HANDLE OrgHandle,
    IN  HII_DEVICE_INFO *Org,
    IN  OUT EFI_HANDLE NewHandle,
    IN  OUT HII_DEVICE_INFO *New,
    IN  UINTN HiiTokenSize)
{
    HII_DEVICE_INFO *OrgPtr, *NewPtr ;
    EFI_STATUS Status ;
    UINT16 i ;
    
    for (OrgPtr = Org, NewPtr = New ;
         (UINT8*)OrgPtr < (UINT8*)Org + HiiTokenSize ;
         OrgPtr = NEXT_HII_DEVICE_INFO(OrgPtr), NewPtr = NEXT_HII_DEVICE_INFO(NewPtr))
    {
        for (i = 0; i < HII_TOKEN_COUNT(OrgPtr); i++)
        {
            STRING_REF Strid = 0;
            UINTN Size = 0x100;
            CHAR16 String[0x100]= L"";
            Status = HiiLibGetString(OrgHandle, OrgPtr->StrToken[i], &Size, String);
            if (EFI_ERROR(Status)) continue ;
            
            Status = NewStringWithFirstFoundLanguage(NewHandle, &Strid, String);
            if (EFI_ERROR(Status)) continue ;
            
            NewPtr->StrToken[i] = Strid ;
        }
    }
}

/**
    Initial Legacy Group Form page

    @param  SetupHandle     Setup Hii Handle
    @param  GroupFormHandle GroupForm Hii Handle
    
    @retval NONE
**/
#if CSM_SUPPORT
EFI_STATUS
InitialLegacyHiiGroupForm( 
    EFI_HANDLE SetupHandle, 
    EFI_HANDLE GroupFormHandle)
{
    // Gf = Group Form
    EFI_STATUS Status = EFI_SUCCESS;
    UINT16 i, j;    
    UINT16 VarFixedBootID = 0xffff;
    UINTN OrgHiiTokenSize = 0 ;
    UINTN DevOrderSize = 0;
    
    LEGACY_DEVICE_ORDER *DevOrder = NULL, *DevOrderBuffer = NULL;
    HII_DEVICE_INFO *OrgHiiToken = NULL, *OrgHiiTokenPtr = NULL ;
    HII_DEVICE_INFO *GfHiiToken = NULL, *GfHiiTokenPtr = NULL;
    UINT8 *IfrTemp = NULL, *IfrTempPtr = NULL;
    EFI_HII_UPDATE_DATA Update;

        
    // Find Variable ID.
    InitHiiData(GroupFormHandle);
    VarFixedBootID = FIndVariableID("FixedBoot");
    if (VarFixedBootID == 0xffff) return EFI_NOT_FOUND ;
    FreeHiiData();
    
    // Check HiiMap and LegacyDevMap
    if (!gFboHiiMap || !gFboLegacyDevMap) return EFI_NOT_FOUND;
        
    // Check variable status    
    Status = GetEfiVariable(L"FixedHiiLegacyDevData",
                            &gFixedBootOrderGuid,
                            NULL,
                            &OrgHiiTokenSize,
                            &OrgHiiToken) ;
    if (EFI_ERROR(Status)) goto Exit ;
    
    Status = GetEfiVariable(L"FboLegacyDevOrder", 
                            &gFixedBootOrderGuid, 
                            NULL, 
                            &DevOrderSize, 
                            &DevOrderBuffer);
    if (EFI_ERROR(Status) || DevOrderSize <= EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER,Device))
        goto Exit ;

    // Allocate necessary memory.
    GfHiiTokenPtr = GfHiiToken = MallocZ(OrgHiiTokenSize) ;
    IfrTemp = IfrTempPtr = MallocZ(
                  (sizeof(EFI_IFR_ONE_OF)
                  +sizeof(EFI_IFR_ONE_OF_OPTION)*(FIXED_BOOT_ORDER_SUBMENU_MAX_NUM+1)  // include one disable option
                  +sizeof(EFI_IFR_END))
                  *FIXED_BOOT_ORDER_SUBMENU_MAX_NUM);
    if (!IfrTempPtr || !GfHiiToken) goto Exit ;
    
    MemCpy(GfHiiToken, OrgHiiToken, OrgHiiTokenSize) ;
    
    // Copy HII device string into new handle.
    CopyHiiStringToNewHandle(SetupHandle, OrgHiiToken, GroupFormHandle, GfHiiTokenPtr, OrgHiiTokenSize) ;
    

    for (DevOrder = DevOrderBuffer ; 
         (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + DevOrderSize ; 
         DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        UINT16 SubmenuIndex;

        if(DevOrder->Type == BBS_UNKNOWN) continue;

        SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
        if (gFboHiiMap[SubmenuIndex].FormID == 0) continue;          //this type no Submenu

        // Use DevOrder->Type to find GfHiiToken->Type
        for (GfHiiTokenPtr = GfHiiToken ;
             (UINT8*)GfHiiTokenPtr < (UINT8*)GfHiiToken + OrgHiiTokenSize ;
             GfHiiTokenPtr = NEXT_HII_DEVICE_INFO(GfHiiTokenPtr))
        {
            if (GfHiiTokenPtr->Type == DevOrder->Type) 
                break ; 
        }
            
        Update.Data = IfrTempPtr = IfrTemp;           //reset IFR data memory.
        for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
        {
            UINT16 VarOffset;
            UINT16 CallBackKey;
            UINT16 DevDataIndex = 0;
            UINT16 Strid ;
                
            if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            DevDataIndex = SearchDevMapByType(gFboLegacyDevMap, GfHiiTokenPtr->Type);
            VarOffset=(DevDataIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) + i;//offset to FIXED_BOOT_SETUP.LegacyDevice[]
            CallBackKey=(DevDataIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) + i + LegacyForms_StartKey;

            if (gBootOptionTokenMap == NULL)                                //(EIP123284+)
                Strid = HiiAddLanguageBootOptionString(GroupFormHandle, i + 1);   //(EIP101408+)
            else                                                            //(EIP123284+)
                Strid = gBootOptionTokenMap[i];                             //(EIP123284+)
                
            Status = CreateHiiOneOfItem(&IfrTempPtr, VarFixedBootID, VarOffset, CallBackKey, Strid, STR_BOOT_OPTION_HELP, 0x10);

            for (j = 0; j < HII_TOKEN_COUNT(GfHiiTokenPtr) ; j++)
            {
                if (j >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
                Status = CreateHiiOneOfOptionItem(&IfrTempPtr, 0, j, GfHiiTokenPtr->StrToken[j]);     //UINT8
            }
                
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            Status = CreateHiiOneOfOptionItem(&IfrTempPtr, 0, FIXED_BOOT_ORDER_SUBMENU_MAX_NUM, STR_DISABLED);
#endif
            CreateHiiIfrEndItem(&IfrTempPtr);
        }

        Update.Offset = (UINT32)((UINT8*)IfrTempPtr - (UINT8*)Update.Data); //Calc Hii data length.
        TRACE((FBO_TRACE_LEVEL, "Update.Data...............\n"));

        Status = IfrLibUpdateForm(
                             GroupFormHandle,
                             NULL,
                             gFboHiiMap[SubmenuIndex].FormID,
                             gFboHiiMap[SubmenuIndex].Label,
                             TRUE,
                             &Update);
        TRACE((FBO_TRACE_LEVEL, "IfrLibUpdateForm (%r)\n", Status)); 
        
        pRS->SetVariable(L"FixedGfHiiLegacyDevData",
                         &gFixedBootOrderGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         OrgHiiTokenSize,
                         GfHiiToken);
    }
    
Exit:
    pBS->FreePool(IfrTemp) ;
    pBS->FreePool(DevOrderBuffer) ;
    pBS->FreePool(OrgHiiToken) ;
    pBS->FreePool(GfHiiToken) ;
    return Status;
}
#endif

/**
    Initial UEFI Group Form page

    @param  SetupHandle     Setup Hii Handle
    @param  GroupFormHandle GroupForm Hii Handle
    
    @retval NONE
**/
EFI_STATUS
InitialUefiHiiGroupForm( 
    EFI_HANDLE SetupHandle, 
    EFI_HANDLE GroupFormHandle)
{
    // Gf = Group Form
    EFI_STATUS Status = EFI_SUCCESS;
    UINT16 i, j;    
    UINT16 VarFixedBootID = 0xffff;
    UINTN OrgHiiTokenSize = 0 ;
    UINTN DevOrderSize = 0;
    
    UEFI_DEVICE_ORDER *DevOrder = NULL, *DevOrderBuffer = NULL;
    HII_DEVICE_INFO *OrgHiiToken = NULL, *OrgHiiTokenPtr = NULL ;
    HII_DEVICE_INFO *GfHiiToken = NULL, *GfHiiTokenPtr = NULL;
    UINT8 *IfrTemp = NULL, *IfrTempPtr = NULL;
    EFI_HII_UPDATE_DATA Update;
    

    // Find Variable ID.
    InitHiiData(GroupFormHandle);
    VarFixedBootID = FIndVariableID("FixedBoot");
    if (VarFixedBootID == 0xffff) return EFI_NOT_FOUND ;
    FreeHiiData();
    
    // Check HiiMap and LegacyDevMap
    if (!gFboHiiMap || !gFboUefiDevMap) return EFI_NOT_FOUND;
        
    // Check variable status    
    Status = GetEfiVariable(L"FixedHiiUefiDevData",
                            &gFixedBootOrderGuid,
                            NULL,
                            &OrgHiiTokenSize,
                            &OrgHiiToken) ;
    if (EFI_ERROR(Status)) goto Exit ;
    
    Status = GetEfiVariable(L"UefiDevOrder", 
                            &gFixedBootOrderGuid, 
                            NULL, 
                            &DevOrderSize, 
                            &DevOrderBuffer);
    if (EFI_ERROR(Status) || DevOrderSize <= EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER,Device))
        goto Exit ;

    // Allocate necessary memory.
    GfHiiTokenPtr = GfHiiToken = MallocZ(OrgHiiTokenSize) ;
    IfrTemp = IfrTempPtr = MallocZ(
                  (sizeof(EFI_IFR_ONE_OF)
                  +sizeof(EFI_IFR_ONE_OF_OPTION)*(FIXED_BOOT_ORDER_SUBMENU_MAX_NUM+1)  // include one disable option
                  +sizeof(EFI_IFR_END))
                  *FIXED_BOOT_ORDER_SUBMENU_MAX_NUM);
    if (!IfrTempPtr || !GfHiiToken) goto Exit ;
    
    MemCpy(GfHiiToken, OrgHiiToken, OrgHiiTokenSize) ;

    // Copy HII device string into new handle.
    CopyHiiStringToNewHandle(SetupHandle, OrgHiiToken, GroupFormHandle, GfHiiTokenPtr, OrgHiiTokenSize) ;    
    
    for (DevOrder = DevOrderBuffer ; 
         (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + DevOrderSize ; 
         DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        UINT16 SubmenuIndex;

        SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
        if (gFboHiiMap[SubmenuIndex].FormID == 0) continue;          //this type no Submenu

        // Use DevOrder->Type to find GfHiiToken->Type
        for (GfHiiTokenPtr = GfHiiToken ;
             (UINT8*)GfHiiTokenPtr < (UINT8*)GfHiiToken + OrgHiiTokenSize ;
             GfHiiTokenPtr = NEXT_HII_DEVICE_INFO(GfHiiTokenPtr))
        {
            if (GfHiiTokenPtr->Type == DevOrder->Type) 
                break ; 
        }
        
        Update.Data = IfrTempPtr = IfrTemp;           //reset IFR data memory.
        for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
        {
            UINT16 VarOffset;
            UINT16 CallBackKey;
            UINT16 DevDataIndex = 0;
            UINT16 Strid ;
                
            if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            DevDataIndex = SearchDevMapByType(gFboUefiDevMap, GfHiiTokenPtr->Type);
            VarOffset=(DevDataIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) + EFI_FIELD_OFFSET(FIXED_BOOT_SETUP, UefiDevice) +i;
            CallBackKey=(DevDataIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) + i + UefiForms_StartKey;

            if (gBootOptionTokenMap == NULL)                                //(EIP123284+)
                Strid = HiiAddLanguageBootOptionString(GroupFormHandle, i + 1);   //(EIP101408+)
            else                                                            //(EIP123284+)
                Strid = gBootOptionTokenMap[i];                             //(EIP123284+)
                
            Status = CreateHiiOneOfItem(&IfrTempPtr, VarFixedBootID, VarOffset, CallBackKey, Strid, STR_BOOT_OPTION_HELP, 0x10);

            for (j = 0; j < HII_TOKEN_COUNT(GfHiiTokenPtr) ; j++)
            {
                if (j >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
                Status = CreateHiiOneOfOptionItem(&IfrTempPtr, 0, j, GfHiiTokenPtr->StrToken[j]);     //UINT8
            }    

#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            Status = CreateHiiOneOfOptionItem(&IfrTempPtr, 0, FIXED_BOOT_ORDER_SUBMENU_MAX_NUM, STR_DISABLED);
#endif
            CreateHiiIfrEndItem(&IfrTempPtr);
        }

        Update.Offset = (UINT32)((UINT8*)IfrTempPtr - (UINT8*)Update.Data); //Calc Hii data length.
        TRACE((FBO_TRACE_LEVEL, "Update.Data...............\n"));

        Status = IfrLibUpdateForm(
                             GroupFormHandle,
                             NULL,
                             gFboHiiMap[SubmenuIndex].FormID,
                             gFboHiiMap[SubmenuIndex].Label,
                             TRUE,
                             &Update);
        TRACE((FBO_TRACE_LEVEL, "IfrLibUpdateForm (%r)\n", Status)); 
        
        pRS->SetVariable(L"FixedGfHiiUefiDevData",
                         &gFixedBootOrderGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         OrgHiiTokenSize,
                         GfHiiToken);
    }
    
Exit:
    pBS->FreePool(IfrTemp) ;
    pBS->FreePool(DevOrderBuffer) ;
    pBS->FreePool(OrgHiiToken) ;
    pBS->FreePool(GfHiiToken) ;
    return Status; 
}

/**
    Initial necessary HII data and global parameter 
    with Group Form Handle

    @param  SetupHandle     - Setup Hii Handle
    @param  GroupFormHandle - GroupForm Hii Handle
    
    @retval NONE
**/
VOID InitGroupFormData(
    EFI_HII_HANDLE GroupFormHandle,
    CALLBACK_INFO *pCallBackFound)
{
    UINT16 Class ;
    EFI_HII_HANDLE SetupHandle ;
    UINTN VarSize = sizeof(EFI_HII_HANDLE) ;
    EFI_STATUS Status ;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FboProtocol ;

    UINTN GroupSize = 0, FixedBootSize = 0 ;
    EFI_GUID SetupGuid = SETUP_GUID;
    
    if (!pCallBackFound || !pCallBackFound->HiiHandle) return;
    
    Class = pCallBackFound->Class ;
    if (Class != FBO_GROUP_FORM_FORM_SET_CLASS) return ;
    
    Status = pRS->GetVariable(L"FixedBootOrderHii", &gFixedBootOrderGuid, NULL, &VarSize, &SetupHandle) ;
    if (EFI_ERROR(Status)) return ;
    
    // Initial FixedBootOrder map.
    Status = pBS->LocateProtocol(&gFixedBootOrderGuid, NULL, &FboProtocol);
    if (!EFI_ERROR(Status))
    {
        gFboLegacyDevMap = FboProtocol->GetLegacyDevMap();
        gFboUefiDevMap = FboProtocol->GetUefiDevMap();
        gFboHiiMap = FboProtocol->GetHiiMap();
        //(EIP123284+)>
#if FBO_USE_BOOT_OPTION_NUMBER_UNI
        gBootOptionTokenMap = FboProtocol->GetBootOptionTokenMap();
#else
        gBootOptionTokenMap = NULL;
#endif
        //<(EIP123284+)
    }

    Status = GetEfiVariable(L"FixedBootGroup",
                            &SetupGuid,
                            NULL,
                            &GroupSize,
                            &gFixedBootGroup) ;
    if (EFI_ERROR(Status)) return ;

    Status = GetEfiVariable(L"FixedBoot",
                            &gFboGroupFormFormSetGuid,
                            NULL,
                            &FixedBootSize,
                            &gFixedBoot) ;
    if (EFI_ERROR(Status)) return ;
    

#if CSM_SUPPORT
    InitialLegacyHiiGroupForm( SetupHandle, GroupFormHandle ) ;
#endif
    InitialUefiHiiGroupForm( SetupHandle, GroupFormHandle ) ;

    pRS->SetVariable(L"FboGfHiiHandle",
                     &gFixedBootOrderGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS,
                     sizeof(EFI_HII_HANDLE),
                     &GroupFormHandle);
    return ;
}

/**
    FixedBootOrder Group Form page callback

    @param  This            Protocol pointer
    @param  Action          Browser action
    @param  KeyValue        Callback key
    @param  Type            Value type
    @param  Value           Changed Value
    @param  ActionRequest   Action feeback
        
    @retval Status
**/
EFI_STATUS FboGroupFormCallback(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest)
{
    UINTN BufferSize = sizeof(FIXED_BOOT_SETUP);
    UINT16 DevTypeIndex = 0;
    UINT16 DevType = 0;
    EFI_GUID SetupGuid = SETUP_GUID;
    FIXED_BOOT_SETUP *IfrNvDataSubMenu = NULL; 
    EFI_STATUS Status ;

    UINT8 *Group = (UINT8*)gFixedBootGroup;
    UINT16 HiiDevIndex = 0xffff;
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
    EFI_FIXED_BOOT_ORDER_SETUP_PROTOCOL *FboSetup ;
    EFI_GUID FboSetupGuid = FIXED_BOOT_ORDER_SETUP_GUID ;
#endif
    if (ActionRequest) 
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

    if (Action == EFI_BROWSER_ACTION_CHANGING)
        return EFI_SUCCESS;
    else if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
    {
#if !DONOT_LOAD_DEFAULT_IN_SETUP
        Value->u8 = (KeyValue - LegacyForms_StartKey) % FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
        if(KeyValue >= UefiForms_StartKey)
        {
            gFixedBoot->UefiDevice[(KeyValue - UefiForms_StartKey)]
                    = Value->u8;
        }else if (KeyValue >= LegacyForms_StartKey)
        {
            gFixedBoot->UefiDevice[(KeyValue - LegacyForms_StartKey)]
                    = Value->u8;            
        }

#endif
        return EFI_SUCCESS;
    }
    else if (Action != EFI_BROWSER_ACTION_CHANGED)
        return EFI_INVALID_PARAMETER;
    
    TRACE((-1, "FboGroupFormCallback : Callback key:%X\n", KeyValue)) ; 
    if (KeyValue < FixedBootOrderStratKey
        || KeyValue > (UefiForms_StartKey+FIXED_BOOT_ORDER_TOTAL_DEVICES_MAX_NUM))
        return EFI_UNSUPPORTED;
        
    if (KeyValue >= UefiForms_StartKey)
    {
        //--------------------------------------
        //SubMenu UEFI Boot device change process
        //--------------------------------------
        //
        // Retrive uncommitted data from Browser
        //
        pBS->AllocatePool(EfiBootServicesData, BufferSize, &IfrNvDataSubMenu);
        ASSERT(IfrNvDataSubMenu != NULL);

        Status = HiiLibGetBrowserData(&BufferSize, IfrNvDataSubMenu, &gFboGroupFormFormSetGuid, L"FixedBoot");
        TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c] HiiLibGetBrowserData(%r) BufferSize=%x\n", Status, BufferSize));
        if (EFI_ERROR(Status))
        {
            pBS->FreePool(IfrNvDataSubMenu);
            return Status;
        }
        
        DevTypeIndex = (KeyValue - UefiForms_StartKey) / FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
        HiiDevIndex = SearchHiiMapByType(gFboUefiDevMap[DevTypeIndex].DevType);
        
        if (HiiDevIndex != 0xffff)
        {
            GroupkeyProcess( &IfrNvDataSubMenu->UefiDevice[DevTypeIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM],
                         &(gFixedBoot->UefiDevice[DevTypeIndex*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM]),
                         (KeyValue - UefiForms_StartKey) % FIXED_BOOT_ORDER_SUBMENU_MAX_NUM,
                         Group[gFboHiiMap[HiiDevIndex].DeviceCountOffset] );
        }
        
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
        Status = pBS->LocateProtocol(&FboSetupGuid, NULL, &FboSetup);
        if (!EFI_ERROR(Status))
        {
            DevType=gFboUefiDevMap[DevTypeIndex].DevType;
            FboSetup->UpdateUefiHiiDevString(DevType, gFixedBoot->UefiDevice[DevTypeIndex*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM]) ;
        }
#endif

        Status = HiiLibSetBrowserData(BufferSize, IfrNvDataSubMenu, &gFboGroupFormFormSetGuid, L"FixedBoot");
        TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c] HiiLibSetBrowserData(%r) BufferSize=%x\n", Status, BufferSize));
        
        pBS->FreePool(IfrNvDataSubMenu);

        return Status;
    }
    else if (KeyValue >= LegacyForms_StartKey)
    {  
        //--------------------------------------
        //SubMenu Legacy Boot device change process
        //--------------------------------------
        //
        // Retrive uncommitted data from Browser
        //
        pBS->AllocatePool(EfiBootServicesData, BufferSize, &IfrNvDataSubMenu);
        ASSERT(IfrNvDataSubMenu != NULL);
        Status = HiiLibGetBrowserData(&BufferSize, IfrNvDataSubMenu, &gFboGroupFormFormSetGuid, L"FixedBoot");
        TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c] HiiLibGetBrowserData(%r) BufferSize=%x\n", Status, BufferSize));
        if (EFI_ERROR(Status))
        {
            pBS->FreePool(IfrNvDataSubMenu);
            return Status;
        }
        
        DevTypeIndex = (KeyValue - LegacyForms_StartKey) / FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
        HiiDevIndex = SearchHiiMapByType(gFboLegacyDevMap[DevTypeIndex].DevType);
        if (HiiDevIndex != 0xffff)
        {
            GroupkeyProcess(&IfrNvDataSubMenu->LegacyDevice[DevTypeIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM],
                            &(gFixedBoot->LegacyDevice[DevTypeIndex*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM]),
                            (KeyValue - LegacyForms_StartKey) % FIXED_BOOT_ORDER_SUBMENU_MAX_NUM,
                            Group[gFboHiiMap[HiiDevIndex].DeviceCountOffset] );
        }
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
        Status = pBS->LocateProtocol(&FboSetupGuid, NULL, &FboSetup);
        if (!EFI_ERROR(Status))
        {
            DevType=gFboLegacyDevMap[DevTypeIndex].DevType;
            FboSetup->UpdateLegacyHiiDevString(DevType, gFixedBoot->LegacyDevice[DevTypeIndex*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM]) ;
        }
#endif

        Status = HiiLibSetBrowserData(BufferSize, IfrNvDataSubMenu, &gFboGroupFormFormSetGuid, L"FixedBoot");
        TRACE((FBO_TRACE_LEVEL, "[FixedBootOrder.c] HiiLibSetBrowserData(%r) BufferSize=%x\n", Status, BufferSize));
        
        pBS->FreePool(IfrNvDataSubMenu);

        return Status;
    }
    return EFI_SUCCESS ;
}

/**
    Group form notify function, install group form handle.

    @param  Event
    @param  Context
        
    @retval None
**/
VOID FboGroupFormNotify(
    IN EFI_EVENT Event, 
    IN VOID *Context)
{
    LoadResources(Context, 
                  sizeof(SetupCallBack) / sizeof(CALLBACK_INFO), 
                  SetupCallBack, 
                  InitGroupFormData);
}

/**
    Group Form driver entry point

    @param  ImageHandle
    @param  SystemTable
        
    @retval Status
**/
EFI_STATUS FboGroupFormEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status=EFI_SUCCESS;
    EFI_EVENT Event;
    VOID *Registration;
    VOID *Context ;
    EFI_GUID FixedBootOrderSetupGuid = FIXED_BOOT_ORDER_SETUP_GUID;
    
    InitAmiLib(ImageHandle,SystemTable);
    
    Context = ImageHandle ;
    Status = RegisterProtocolCallback(&FixedBootOrderSetupGuid, 
                                      FboGroupFormNotify, 
                                      Context, 
                                      &Event, 
                                      &Registration);
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
