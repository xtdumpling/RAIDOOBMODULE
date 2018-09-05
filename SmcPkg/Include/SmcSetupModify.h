//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.02
//    Bug Fix:  Update HideItem data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/14/2017
//
//  Rev. 1.01
//    Bug Fix:  Change HideItem policy to fix TXT can being enabled.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/07/2017
//
//  Rev. 1.00
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//***************************************************************************
#ifndef _H_SMCSETUPMODIFY_
#define _H_SMCSETUPMODIFY_

#define MAX_STRING_SIZE	256
#define MAX_ITEM_STRING_SIZE 64


#pragma pack(1)

typedef struct {
    EFI_HII_HANDLE HiiHandle;
    CHAR16 FormsetTitle[MAX_STRING_SIZE];
    EFI_STRING_ID StringId;
} HiddenItemMap;

typedef struct {
    CHAR16 FormsetTitle[MAX_STRING_SIZE];
    EFI_STRING_ID StringId;
    CHAR8 ToString[MAX_ITEM_STRING_SIZE];
} SetItemStringMap;

#pragma pack()

typedef EFI_STATUS (EFIAPI *SMCSETUPMODIFY_ADD_2_HIDE_LIST) (IN CHAR8 *FormsetTitle, IN EFI_STRING_ID StringId);
typedef EFI_STATUS (EFIAPI *SMCSETUPMODIFY_SET_STRING)(IN CHAR8 *FormsetTitle, IN EFI_STRING_ID StringId, IN CHAR8 *ToString);
typedef EFI_STATUS (EFIAPI *SMCSETUPMODIFY_UPDATE)(VOID);
typedef BOOLEAN (EFIAPI *SMCSETUPMODIFY_SEARCHFORHIDELIST)(IN EFI_HII_HANDLE HiiHandle, IN EFI_STRING_ID StringId);
typedef BOOLEAN (EFIAPI *SMCSETUPMODIFY_SEARCHFORHIDELISTBYSTRING)(IN EFI_HII_HANDLE HiiHandle, IN EFI_STRING_ID StringId);

typedef struct _SMC_SETUP_MODIFY_PROTOCOL {
    SMCSETUPMODIFY_ADD_2_HIDE_LIST Add2HideList;
    SMCSETUPMODIFY_SET_STRING SetString;
    SMCSETUPMODIFY_UPDATE Update;
    SMCSETUPMODIFY_SEARCHFORHIDELIST SearchForHideList;
    SMCSETUPMODIFY_SEARCHFORHIDELISTBYSTRING SearchForHideListByString;
} SMC_SETUP_MODIFY_PROTOCOL;

#endif // #ifndef _H_SMCSETUPMODIFY_

