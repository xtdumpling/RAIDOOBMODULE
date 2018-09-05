//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  Update HideItem data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/14/2017
//
//  Rev. 1.05
//    Bug Fix:  Change HideItem policy to fix TXT can being enabled.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/07/2017
//
//  Rev. 1.04
//    Bug Fix:  Fix HII format error of hide item.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/30/2016
//
//  Rev. 1.03
//    Bug Fix:  Extend SetupMenuHideItemNumber to 256. 100 is not enough.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/17/2016
//
//  Rev. 1.02
//    Bug Fix:  Fix system hanging at ASSERT when enabling DEBUG_MODE.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/26/2016
//
//  Rev. 1.01
//    Bug Fix:  Add sub title hide item.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Sep/26/2016
//
//  Rev. 1.00
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include "AmiDxeLib.h"
#include <Library/HiiLib.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiPackageList.h>
#include "SmcSetupModify.h"

#define MAX_PACKAGELISTS_SIZE 0x180000

static HiddenItemMap* HiddenItemList = NULL;
static SetItemStringMap* SetItemStringList = NULL;

CHAR8* ToGuidString(
    CHAR8* StringBuffer,
    EFI_GUID* Guid
)
{
    Sprintf(StringBuffer,
            "0x%08x-0x%04x-0x%04x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x",
            Guid->Data1,
            Guid->Data2,
            Guid->Data3,
            Guid->Data4[0],
            Guid->Data4[1],
            Guid->Data4[2],
            Guid->Data4[3],
            Guid->Data4[4],
            Guid->Data4[5],
            Guid->Data4[6],
            Guid->Data4[7]);

    return StringBuffer;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : Add2HideList
//
// Description : Add setup item into hidden list
//
// Parameters  :
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS Add2HideList(
    IN CHAR8 *FormsetTitle,
    IN EFI_STRING_ID StringId
)
{
    UINT32 ii = 0;

    for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
        if((HiddenItemList + ii)->StringId == 0xFFFF) {
            (HiddenItemList + ii)->StringId = StringId;
            AsciiStrToUnicodeStr (FormsetTitle, (HiddenItemList + ii)->FormsetTitle);
            break;
        }
    }

    if (ii >= SetupMenuHideItemNumber) {
        DEBUG((-1, "ERROR : SmcSetupModify.Add2HideList : SetupMenuHideItemNumber is too small. ii=%d, SetupMenuHideItemNumber=%d\n", ii, SetupMenuHideItemNumber));
        EFI_DEADLOOP ();
    }

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SearchForHideList
//
// Description : Search for FormsetTitle and StringId in HideList.
//
// Parameters  :
//
// Returns     :  TRUE  - FormsetTitle and StringId are in HideList.
//                FALSE - FormsetTitle and StringId are not in HideList.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
BOOLEAN SearchForHideList(
    IN EFI_HII_HANDLE HiiHandle,
    IN EFI_STRING_ID StringId
)
{
    UINT32 ii;

    for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
        if((HiddenItemList + ii)->StringId == 0xFFFF)
            break;

        if((HiddenItemList + ii)->StringId == StringId && (HiddenItemList + ii)->HiiHandle == HiiHandle)
            return TRUE;
    }

    return FALSE;
}

BOOLEAN SearchForHideListByString (
    IN CHAR16 *FormsetTitleWide,
    IN EFI_STRING_ID StringId
)
{
    UINT32 ii;

    for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
        if((HiddenItemList + ii)->StringId == 0xFFFF)
            break;

        if((HiddenItemList + ii)->StringId == StringId && Wcscmp((HiddenItemList + ii)->FormsetTitle, FormsetTitleWide) == 0)
            return TRUE;
    }

    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : InitString
//
// Description : Add setup item into changing string list
//
// Parameters  :
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS SetString(
    IN CHAR8 *FormsetTitle,
    IN EFI_STRING_ID StringId,
    IN CHAR8 *ToString
)
{
    UINT32 ii = 0;

    for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
        if((SetItemStringList + ii)->StringId == 0xFFFF) {
            (SetItemStringList + ii)->StringId = StringId;
            AsciiStrToUnicodeStr (FormsetTitle, (SetItemStringList + ii)->FormsetTitle);
            Strcpy((SetItemStringList + ii)->ToString, ToString);
            break;
        }
    }

    if (ii >= SetupMenuHideItemNumber) {
        DEBUG((-1, "ERROR : SmcSetupModify.SetString : SetupMenuHideItemNumber is too small. ii=%d, SetupMenuHideItemNumber=%d\n", ii, SetupMenuHideItemNumber));
        EFI_DEADLOOP ();
    }

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : UpdateStringInHii
//
// Description :
//
// Parameters  :  FormsetTitleWide - The formset's title.
//                HiiHandle        - The formset's HII handle.
//
// Returns     :  None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID UpdateStringInHii(
    IN CHAR16 *FormsetTitleWide,
    IN EFI_HII_HANDLE HiiHandle
)
{
    UINT32 ii = 0;
    CHAR16 StrBuff[MAX_ITEM_STRING_SIZE] = {0};

    //
    // Loop the SetString list.
    //
    for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
        if((SetItemStringList + ii)->StringId == 0xFFFF)
            break;

        if(Wcscmp((SetItemStringList + ii)->FormsetTitle, FormsetTitleWide) == 0) {
            AsciiStrToUnicodeStr ((SetItemStringList + ii)->ToString, StrBuff);
            DEBUG((-1, "SmcSetupModify.UpdateStringInHii : SetString - %s, %s\n", FormsetTitleWide, StrBuff));
            //
            // Update string into HII database.
            //
            HiiSetString(HiiHandle, (SetItemStringList + ii)->StringId, StrBuff, NULL);
        }
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : Update
//
// Description : Update HiddenItemList and SetItemStringList into HII database.
//
// Parameters  :  None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS Update(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HII_DATABASE_PROTOCOL *HiiDatabase;
    UINTN HandleBufferLength;
    EFI_HII_HANDLE  TempHiiHandleBuffer;
    EFI_HII_HANDLE *HiiHandleBuffer = NULL;
    UINTN Index1;
    UINTN BufferSize;
    EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList;
    EFI_HII_PACKAGE_HEADER *IfrPackagePtr;
    EFI_IFR_OP_HEADER* pIfrOpHeader;
    CHAR8 StrBuff[MAX_STRING_SIZE] = {0};
    CHAR8 FormSetTitle[MAX_STRING_SIZE] = {0};
    UINTN FormSetTitleWideSize = MAX_STRING_SIZE;
    CHAR16 FormSetTitleWide[MAX_STRING_SIZE] = {0};
    UINT32 ii = 0;

    Status = pBS->LocateProtocol(
                 &gEfiHiiDatabaseProtocolGuid,
                 NULL,
                 &HiiDatabase);

    if(EFI_ERROR(Status)) {
        DEBUG((-1, "ERROR : Locate protocol - gEfiHiiDatabaseProtocolGuid failed.\n"));
        goto ErrorFreeMem;
    }

    //
    // Retrieve the size required for the buffer of all HII handles.
    //
    HandleBufferLength = 0;
    Status = HiiDatabase->ListPackageLists(
                 HiiDatabase,
                 EFI_HII_PACKAGE_TYPE_ALL,
                 NULL,
                 &HandleBufferLength,
                 &TempHiiHandleBuffer);

    //
    // If ListPackageLists() returns EFI_SUCCESS for a zero size,
    // then there are no HII handles in the HII database.  If ListPackageLists()
    // returns an error other than EFI_BUFFER_TOO_SMALL, then there are no HII
    // handles in the HII database.
    //
    if(Status != EFI_BUFFER_TOO_SMALL) {
        //
        // Return NULL if the size can not be retrieved, or if there are no HII
        // handles in the HII Database
        //
        DEBUG((-1, "ERROR : The size of all HII handles can not be retrieved.\n"));
        goto ErrorFreeMem;
    }

    //
    // Allocate the array of HII handles to hold all the HII Handles and a NULL terminator
    //
    pBS->AllocatePool(EfiBootServicesData, HandleBufferLength + sizeof(EFI_HII_HANDLE), &((UINT8*)HiiHandleBuffer));
    MemSet((UINT8*)HiiHandleBuffer, (UINT32)(HandleBufferLength + sizeof(EFI_HII_HANDLE)), 0);
    if (HiiHandleBuffer == NULL) {
        //
        // Return NULL if allocation fails.
        //
        DEBUG((-1, "ERROR : Allocate the array of HII handles failed.\n"));
        goto ErrorFreeMem;
    }

    //
    // Retrieve the array of HII Handles in the HII Database
    //
    Status = HiiDatabase->ListPackageLists(
                 HiiDatabase,
                 EFI_HII_PACKAGE_TYPE_ALL,
                 NULL,
                 &HandleBufferLength,
                 HiiHandleBuffer);

    if(EFI_ERROR(Status)) {
        DEBUG((-1, "ERROR : Retrieve the array of HII Handles in the HII Database failed.\n"));
        goto ErrorFreeMem;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, MAX_PACKAGELISTS_SIZE, &((UINT8*)HiiPackageList));
    if (EFI_ERROR (Status)) {
        TRACE ((-1, "Can't allocate memory for 'HiiPackageList'. Status = %r\n", Status));
        goto ErrorFreeMem;
    }

    for (Index1 = 0; ; Index1++) { // FOR - Loop all HPK list
        if(HiiHandleBuffer[Index1] == NULL || Index1 >= (HandleBufferLength/sizeof(EFI_HANDLE))) {
            DEBUG((-1, "Break from getting HPK.\n"));
            break;
        }

        //
        // Get HII PackageList
        //
        BufferSize = MAX_PACKAGELISTS_SIZE;

        Status = HiiDatabase->ExportPackageLists(HiiDatabase, HiiHandleBuffer[Index1], &BufferSize, HiiPackageList);
        DEBUG((-1, "HiiDatabase->ExportPackageLists : Status = %r\n", Status));
        if (EFI_ERROR (Status)) {
            continue;
        }

        for (
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList + 1);
            IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList + HiiPackageList->PackageLength);
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr + IfrPackagePtr->Length))
        { // FOR - Loop all HPK
            MemSet(&(FormSetTitle[0]), sizeof (FormSetTitle), 0);

            if(IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) {
                pIfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + sizeof(EFI_HII_PACKAGE_HEADER));
                if(pIfrOpHeader->OpCode == EFI_IFR_FORM_SET_OP) {
                    //
                    // Get formset title of the HPK list
                    //
                    MemSet((UINT8*)FormSetTitleWide, sizeof(FormSetTitleWide), 0);
                    FormSetTitleWideSize = MAX_STRING_SIZE;
                    Status = HiiLibGetString(
                                 HiiHandleBuffer[Index1],
                                 ((EFI_IFR_FORM_SET*)pIfrOpHeader)->FormSetTitle,
                                 &FormSetTitleWideSize,
                                 FormSetTitleWide);

                    if(!EFI_ERROR (Status)) {

                        UnicodeStrToAsciiStr(&(FormSetTitleWide[0]), &(FormSetTitle[0]));

#if DEBUG_MODE == 1
                        {
                            CHAR16 StrGuidBuff[128] = {0};
                            AsciiStrToUnicodeStr (ToGuidString (StrBuff, &((EFI_IFR_FORM_SET*)pIfrOpHeader)->Guid), StrGuidBuff);
                            DEBUG((-1, "SmcSetupModify.Update : HiiHandle = 0x%x\n", HiiHandleBuffer[Index1]));
                            DEBUG((-1, "SmcSetupModify.Update : FormSetTitle = %s\n", FormSetTitleWide));
                            DEBUG((-1, "SmcSetupModify.Update : Guid = %s\n", StrGuidBuff));
                            DEBUG((-1, "SmcSetupModify.Update : IfrPackagePtr->Length = 0x%x\n", IfrPackagePtr->Length));
                        }
#endif // #if DEBUG_MODE == 1

                        //
                        // Update HII handle to "HiddenItemList".
                        //
                        for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
                            if((HiddenItemList + ii)->StringId == 0xFFFF)
                                break;
                            if(Wcscmp((HiddenItemList + ii)->FormsetTitle, FormSetTitleWide) == 0) {
                                (HiddenItemList + ii)->HiiHandle = HiiHandleBuffer[Index1];
                            }
                        }
                    }
                }
            }
        } // FOR - Loop all HPK

        //
        // Update the formset's string list.
        //
        UpdateStringInHii(FormSetTitleWide, HiiHandleBuffer[Index1]);

        DEBUG((-1, "\n"));

    } // FOR - Loop all HPK list

ErrorFreeMem:

    if(HiiHandleBuffer != NULL)	pBS->FreePool((UINT8*)HiiHandleBuffer);
    if (HiiPackageList != NULL) pBS->FreePool((UINT8*)HiiPackageList);

    return EFI_SUCCESS;
}

SMC_SETUP_MODIFY_PROTOCOL SetupModifyProtocol =
{
    Add2HideList,
    SetString,
    Update,
    SearchForHideList,
    SearchForHideListByString
};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcSetupModifyInit
//
// Description : Project DXE initial code
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
SmcSetupModifyInit(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE Handle = NULL;
    UINT32 ii;

    InitAmiLib(ImageHandle, SystemTable);

    DEBUG((-1, "SmcSetupModifyInit Init\n"));

    //
    // Allocate memory for HiddenItemList.
    //
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(HiddenItemMap) * SetupMenuHideItemNumber, &((UINT8*)HiddenItemList));
    if(EFI_ERROR(Status)) {
        DEBUG((-1, "Memory allocating is failed for HiddenItemList.\n"));
        return EFI_SUCCESS;
    }

    //
    // Clear HiddenItemList memory.
    //
    MemSet((UINT8*)HiddenItemList, sizeof(HiddenItemMap) * SetupMenuHideItemNumber, 0);
    for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
        (HiddenItemList + ii)->StringId = 0xFFFF;
    }

    //
    // Allocate memory for SetItemStringList.
    //
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof (SetItemStringMap) * SetupMenuHideItemNumber, &((UINT8*)SetItemStringList));
    if(EFI_ERROR (Status)) {
        DEBUG((-1, "Memory allocating is failed for SetItemStringList.\n"));
        if(HiddenItemList != NULL) pBS->FreePool((UINT8*)HiddenItemList);
        return EFI_SUCCESS;
    }

    //
    // Clear SetItemStringList memory.
    //
    MemSet((UINT8*)SetItemStringList, sizeof(SetItemStringMap) * SetupMenuHideItemNumber, 0);
    for(ii = 0; ii < SetupMenuHideItemNumber; ii++) {
        (SetItemStringList + ii)->StringId = 0xFFFF;
    }

    Status = pBS->InstallProtocolInterface(
                 &Handle,
                 &gSetupModifyProtocolGuid ,
                 EFI_NATIVE_INTERFACE,
                 &SetupModifyProtocol);

    if(EFI_ERROR (Status)) {
        DEBUG((-1, "Installing gSetupModifyProtocolGuid is failed.\n"));
        if(HiddenItemList != NULL) pBS->FreePool((UINT8*)HiddenItemList);
        if(SetItemStringList != NULL) pBS->FreePool((UINT8*)SetItemStringList);
        return EFI_SUCCESS;
    }

    DEBUG((-1, "Exit SmcSetupModifyInit Init\n"));

    return Status;
}
