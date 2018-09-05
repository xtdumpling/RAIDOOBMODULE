//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.12
//    Bug Fix:  Modify setup template Json code in HII for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/28/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcInBandHii.c
//
// Description: In-Band HII feature.
//
//<AMI_FHDR_END>
//**********************************************************************
//#include "Token.h"

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiPackageList.h>
#include <Protocol/HiiString.h>
#include "SmcInBandHii.h"
#include <Library/BaseMemoryLib.h>

#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

CHAR16* BuildPrefix16 (CHAR16* StrBuff, UINT32 Count)
{
    UINT32 i = 0;
    CHAR16* pStrBuff = StrBuff;
    for (i = 0; i < Count; i++) {
        *pStrBuff = ' ';
        *pStrBuff++;
        *pStrBuff = ' ';
        *pStrBuff++;
        *pStrBuff = ' ';
        *pStrBuff++;
        *pStrBuff = ' ';
        *pStrBuff++;
    }
    *pStrBuff = 0;
    return StrBuff;
}
CHAR8 IFROPSTRING[100][40] = {
    "EMPTY",
    "EFI_IFR_FORM_OP",
    "EFI_IFR_SUBTITLE_OP",
    "EFI_IFR_TEXT_OP",
    "EFI_IFR_IMAGE_OP",
    "EFI_IFR_ONE_OF_OP",
    "EFI_IFR_CHECKBOX_OP",
    "EFI_IFR_NUMERIC_OP",
    "EFI_IFR_PASSWORD_OP",
    "EFI_IFR_ONE_OF_OPTION_OP",
    "EFI_IFR_SUPPRESS_IF_OP",
    "EFI_IFR_LOCKED_OP",
    "EFI_IFR_ACTION_OP",
    "EFI_IFR_RESET_BUTTON_OP",
    "EFI_IFR_FORM_SET_OP",
    "EFI_IFR_REF_OP",
    "EFI_IFR_NO_SUBMIT_IF_OP",
    "EFI_IFR_INCONSISTENT_IF_OP",
    "EFI_IFR_EQ_ID_VAL_OP",
    "EFI_IFR_EQ_ID_ID_OP",
    "EFI_IFR_EQ_ID_VAL_LIST_OP",
    "EFI_IFR_AND_OP",
    "EFI_IFR_OR_OP",
    "EFI_IFR_NOT_OP",
    "EFI_IFR_RULE_OP",
    "EFI_IFR_GRAY_OUT_IF_OP",
    "EFI_IFR_DATE_OP",
    "EFI_IFR_TIME_OP",
    "EFI_IFR_STRING_OP",
    "EFI_IFR_REFRESH_OP",
    "EFI_IFR_DISABLE_IF_OP",
    "EFI_IFR_ANIMATION_OP",
    "EFI_IFR_TO_LOWER_OP",
    "EFI_IFR_TO_UPPER_OP",
    "EFI_IFR_MAP_OP",
    "EFI_IFR_ORDERED_LIST_OP",
    "EFI_IFR_VARSTORE_OP",
    "EFI_IFR_VARSTORE_NAME_VALUE_OP",
    "EFI_IFR_VARSTORE_EFI_OP",
    "EFI_IFR_VARSTORE_DEVICE_OP",
    "EFI_IFR_VERSION_OP",
    "EFI_IFR_END_OP",
    "EFI_IFR_MATCH_OP",
    "EFI_IFR_GET_OP",
    "EFI_IFR_SET_OP",
    "EFI_IFR_READ_OP",
    "EFI_IFR_WRITE_OP",
    "EFI_IFR_EQUAL_OP",
    "EFI_IFR_NOT_EQUAL_OP",
    "EFI_IFR_GREATER_THAN_OP",
    "EFI_IFR_GREATER_EQUAL_OP",
    "EFI_IFR_LESS_THAN_OP",
    "EFI_IFR_LESS_EQUAL_OP",
    "EFI_IFR_BITWISE_AND_OP",
    "EFI_IFR_BITWISE_OR_OP",
    "EFI_IFR_BITWISE_NOT_OP",
    "EFI_IFR_SHIFT_LEFT_OP",
    "EFI_IFR_SHIFT_RIGHT_OP",
    "EFI_IFR_ADD_OP",
    "EFI_IFR_SUBTRACT_OP",
    "EFI_IFR_MULTIPLY_OP",
    "EFI_IFR_DIVIDE_OP",
    "EFI_IFR_MODULO_OP",
    "EFI_IFR_RULE_REF_OP",
    "EFI_IFR_QUESTION_REF1_OP",
    "EFI_IFR_QUESTION_REF2_OP",
    "EFI_IFR_UINT8_OP",
    "EFI_IFR_UINT16_OP",
    "EFI_IFR_UINT32_OP",
    "EFI_IFR_UINT64_OP",
    "EFI_IFR_TRUE_OP",
    "EFI_IFR_FALSE_OP",
    "EFI_IFR_TO_UINT_OP",
    "EFI_IFR_TO_STRING_OP",
    "EFI_IFR_TO_BOOLEAN_OP",
    "EFI_IFR_MID_OP",
    "EFI_IFR_FIND_OP",
    "EFI_IFR_TOKEN_OP",
    "EFI_IFR_STRING_REF1_OP",
    "EFI_IFR_STRING_REF2_OP",
    "EFI_IFR_CONDITIONAL_OP",
    "EFI_IFR_QUESTION_REF3_OP",
    "EFI_IFR_ZERO_OP",
    "EFI_IFR_ONE_OP",
    "EFI_IFR_ONES_OP",
    "EFI_IFR_UNDEFINED_OP",
    "EFI_IFR_LENGTH_OP",
    "EFI_IFR_DUP_OP",
    "EFI_IFR_THIS_OP",
    "EFI_IFR_SPAN_OP",
    "EFI_IFR_VALUE_OP",
    "EFI_IFR_DEFAULT_OP",
    "EFI_IFR_DEFAULTSTORE_OP",
    "EFI_IFR_FORM_MAP_OP",
    "EFI_IFR_CATENATE_OP",
    "EFI_IFR_GUID_OP",
    "EFI_IFR_SECURITY_OP",
    "EFI_IFR_MODAL_TAG_OP",
    "EFI_IFR_REFRESH_ID_OP",
    "EFI_IFR_WARNING_IF_OP"
};


VOID DisplayHiiPackageDebugMessage (EFI_HII_HANDLE *HiiHandle, EFI_HII_PACKAGE_HEADER *IfrPackagePtr)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CHAR16 StrBuffWide[MAX_TITLE_SIZE];
    UINTN StrBuffWideSize;
    UINT32 ScopeLevel = 0;
    CHAR16 StrPrefix[64] = {0};
    EFI_IFR_OP_HEADER* pIfrOpHeader;

    for (
        pIfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + sizeof (EFI_HII_PACKAGE_HEADER));
        pIfrOpHeader < (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + IfrPackagePtr->Length);
        pIfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)pIfrOpHeader + pIfrOpHeader->Length))
    {
        AsciiStrToUnicodeStr (IFROPSTRING[pIfrOpHeader->OpCode], StrBuffWide);
        OOB_DEBUG((TRACE_ALWAYS, "%sOp=0x%02x %s,Scope=%d",
                   BuildPrefix16(StrPrefix, ScopeLevel),
                   pIfrOpHeader->OpCode,
                   StrBuffWide,
                   pIfrOpHeader->Scope));

        switch(pIfrOpHeader->OpCode) {
        case EFI_IFR_ONE_OF_OP :
        {
            EFI_IFR_ONE_OF* pOneOf = (EFI_IFR_ONE_OF*)pIfrOpHeader;
            MemSet ((UINT8*)StrBuffWide, MAX_TITLE_SIZE * sizeof (CHAR16), 0);
            StrBuffWideSize = MAX_TITLE_SIZE;
            Status = HiiLibGetString (
                         HiiHandle,
                         pOneOf->Question.Header.Prompt,
                         &StrBuffWideSize,
                         StrBuffWide);
            OOB_DEBUG((TRACE_ALWAYS, ",StrId=0x%04x,%s", pOneOf->Question.Header.Prompt, StrBuffWide));
            break;
        }
        case EFI_IFR_ONE_OF_OPTION_OP :
        {
            EFI_IFR_ONE_OF_OPTION* pOneOfOption = (EFI_IFR_ONE_OF_OPTION*)pIfrOpHeader;
            MemSet ((UINT8*)StrBuffWide, MAX_TITLE_SIZE * sizeof (CHAR16), 0);
            StrBuffWideSize = MAX_TITLE_SIZE;
            Status = HiiLibGetString (
                         HiiHandle,
                         pOneOfOption->Option,
                         &StrBuffWideSize,
                         StrBuffWide);
            OOB_DEBUG((TRACE_ALWAYS, ",Flags=0x%04x,%s", pOneOfOption->Flags, StrBuffWide));
            break;
        }
        case EFI_IFR_CHECKBOX_OP :
        {
            EFI_IFR_CHECKBOX* pCheckBox = (EFI_IFR_CHECKBOX*)pIfrOpHeader;
            MemSet ((UINT8*)StrBuffWide, MAX_TITLE_SIZE * sizeof (CHAR16), 0);
            StrBuffWideSize = MAX_TITLE_SIZE;
            Status = HiiLibGetString (
                         HiiHandle,
                         pCheckBox->Question.Header.Prompt,
                         &StrBuffWideSize,
                         StrBuffWide);
            OOB_DEBUG((TRACE_ALWAYS, ",StrId=0x%04x,%s", pCheckBox->Question.Header.Prompt, StrBuffWide));
            break;
        }
        case EFI_IFR_NUMERIC_OP :
        {
            EFI_IFR_NUMERIC* pNumeric = (EFI_IFR_NUMERIC*)pIfrOpHeader;
            MemSet ((UINT8*)StrBuffWide, MAX_TITLE_SIZE * sizeof (CHAR16), 0);
            StrBuffWideSize = MAX_TITLE_SIZE;
            Status = HiiLibGetString (
                         HiiHandle,
                         pNumeric->Question.Header.Prompt,
                         &StrBuffWideSize,
                         StrBuffWide);
            OOB_DEBUG((TRACE_ALWAYS, ",StrId=0x%04x,%s", pNumeric->Question.Header.Prompt, StrBuffWide));
            break;
        }
        case EFI_IFR_REF_OP :
        {
            EFI_IFR_REF* pRef = (EFI_IFR_REF*)pIfrOpHeader;
            MemSet ((UINT8*)StrBuffWide, MAX_TITLE_SIZE * sizeof (CHAR16), 0);
            StrBuffWideSize = MAX_TITLE_SIZE;
            Status = HiiLibGetString (
                         HiiHandle,
                         pRef->Question.Header.Prompt,
                         &StrBuffWideSize,
                         StrBuffWide);
            OOB_DEBUG((TRACE_ALWAYS, ",StrId=0x%04x,%s", pRef->Question.Header.Prompt, StrBuffWide));
            break;
        }
        case EFI_IFR_SUBTITLE_OP:
        {
            EFI_IFR_SUBTITLE* pSubTitle = (EFI_IFR_SUBTITLE*)pIfrOpHeader;
            MemSet ((UINT8*)StrBuffWide, MAX_TITLE_SIZE * sizeof (CHAR16), 0);
            StrBuffWideSize = MAX_TITLE_SIZE;
            Status = HiiLibGetString (
                         HiiHandle,
                         pSubTitle->Statement.Prompt,
                         &StrBuffWideSize,
                         StrBuffWide);
            OOB_DEBUG((TRACE_ALWAYS, ",StrId=0x%04x,%s", pSubTitle->Statement.Prompt, StrBuffWide));
            break;
        }
        }
        OOB_DEBUG((TRACE_ALWAYS, "\n"));

        if (pIfrOpHeader->OpCode == EFI_IFR_END_OP) {
            ScopeLevel--;
        }

        if (pIfrOpHeader->Scope)
            ScopeLevel++;
    }
}
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1


