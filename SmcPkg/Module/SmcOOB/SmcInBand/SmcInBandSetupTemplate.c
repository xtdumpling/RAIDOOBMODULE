//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.11
//    Bug Fix:  Fix lost page problem to SUM.
//    Reason:  
//    Auditor:  Durant Lin
//    Date:     Jul/25/2018
//
//  Rev. 1.10
//    Bug Fix:  Fix report Dynamic page <goto Advanced> show in SUM.
//    Reason:   Support examined Form ID in Hide Form For Sum.
//    Auditor:  Durant Lin
//    Date:     Jun/14/2018
//
//  Rev. 1.09
//    Bug Fix:  Fix uncorrect memory access problem, it may cause system hang up. 
//    Reason:   [Mehlow]
//    Auditor:  Durant Lin
//    Date:     Dec/20/2017
//
//  Rev. 1.08
//    Bug Fix:  Expose "BMC Network Configuration" for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/15/2017
//
//  Rev. 1.07
//    Bug Fix:  Update HideItem data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/14/2017
//
//  Rev. 1.06
//    Bug Fix:  Hide 'Add New Boot Option' form for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jan/26/2017
//
//  Rev. 1.05
//    Bug Fix:  Hide unused forms to fix sum default value errors.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/19/2016
//
//  Rev. 1.04
//    Bug Fix:  Fix some errors for suppressif expression calculation.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/02/2016
//
//  Rev. 1.03
//    Bug Fix:  Hide some forms for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/30/2016
//
//  Rev. 1.02
//    Bug Fix:  Correct setup template Json code in HII for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/29/2016
//
//  Rev. 1.01
//    Bug Fix:  Modify setup template Json code in HII for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/28/2016
//
//  Rev. 1.00
//    Bug Fix:  Add setup template Json code in HII for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/23/2016
//
//****************************************************************************

//<AMI_FHDR_START>
//
// Name:  SmcInBandSetupTemplate.c
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************
#include "efi.h"
//#include "Token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiPackageList.h>
#include <Protocol/HiiString.h>
#include "SmcLib.h"
#include "SmcStack.h"
#include "SmcInBandSetupTemplate.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>

SMC_GUID_HANDLE GuidHandleMap[MAX_GUID_HANDLE_COUNT] = {0};

#define MAX_FORM_COUNT 1024 // Current count is ~ 425
SMC_FORM_DATA* FormData = NULL;

#define MAX_REF_COUNT 2048 // Current count is ~ 780
SMC_OP_FLAG* OpFlag = NULL;

#define MAX_LEVEL_SIZE 64

EFI_GUID FormLayoutGuid = {0xfdb0897d,0xb499,0x4b9f,{0x96,0xf7,0x50,0xc6,0x99,0x68,0x35,0xf5}};

extern SMC_OOB_PLATFORM_POLICY      *mSmcOobPlatformPolicyPtr;

#define HTMDEBUG2 0
#if HTMDEBUG2 == 1
//#include "a7printf.h"
//VOID a7printf(const char *Format, ...);
//a7printf ("SmcInBandCallback : 00\n");

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

#endif // #if HTMDEBUG2 == 1



CHAR8* BuildPrefix (CHAR8* StrBuff, UINT32 Count)
{
    UINT32 i = 0;
    CHAR8* pStrBuff = StrBuff;
    for (i = 0; i < Count; i++) {
        *pStrBuff = ' ';
        *pStrBuff++;
        *pStrBuff = ' ';
        *pStrBuff++;
    }
    *pStrBuff = 0;
    return StrBuff;
}

CHAR16* BuildPrefixW (CHAR16* StrBuff, UINT32 Count)
{
    UINT32 i = 0;
    CHAR16* pStrBuff = StrBuff;
    for (i = 0; i < Count; i++) {
        *pStrBuff = ' ';
        *pStrBuff++;
        *pStrBuff = ' ';
        *pStrBuff++;
    }
    *pStrBuff = 0;
    return StrBuff;
}

CHAR8* ToGuidString (CHAR8* StringBuffer, EFI_GUID* Guid)
{
    Sprintf (StringBuffer,
             "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
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
             Guid->Data4[7]
            );
    return StringBuffer;
}

EFI_STATUS SmcGetString (EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StrId, CHAR8* StrBuff)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CHAR16 Data16[128] = {0};
    UINTN StrLen = 128;

    Status = HiiLibGetString (
                 HiiHandle,
                 StrId,
                 &StrLen,
                 Data16);
    if (!EFI_ERROR (Status)) {
        UnicodeStrToAsciiStr (&(Data16[0]), StrBuff);
    }
    return Status;
}

VOID AddGuidHandle (EFI_GUID* Guid, EFI_HII_HANDLE Handle)
{
    UINT32 Index = 0;
    for (Index = 0; Index < MAX_GUID_HANDLE_COUNT; Index++) {
        if (GuidHandleMap[Index].Guid.Data1 == 0 && GuidHandleMap[Index].Guid.Data2 == 0 && GuidHandleMap[Index].Guid.Data3 == 0) {
            MemCpy (&(GuidHandleMap[Index].Guid), Guid, sizeof (EFI_GUID));
            GuidHandleMap[Index].Handle = Handle;
            break;
        }
    }
}

EFI_HII_HANDLE GetGuidHandle (EFI_GUID* Guid)
{
    UINT32 Index = 0;
    for (Index = 0; Index < MAX_GUID_HANDLE_COUNT; Index++) {
        if (MemCmp (&(GuidHandleMap[Index].Guid), Guid, sizeof (EFI_GUID)) == 0) {
            return GuidHandleMap[Index].Handle;
            break;
        }
    }
    return 0;
}

VOID AddForm (EFI_GUID* Guid, UINT16 FormId, EFI_IFR_OP_HEADER* OpPointer, SMC_FORM_DATA* FormArr, UINT32 FormArrSize)
{
    UINT32 Index = 0;
    for (Index = 0; Index < FormArrSize; Index++) {
        if (FormArr[Index].FormId == 0 && FormArr[Index].OpPointer == 0) {
            MemCpy (&(FormArr[Index].Guid), Guid, sizeof (EFI_GUID));
            FormArr[Index].FormId = FormId;
            FormArr[Index].OpPointer = OpPointer;
            break;
        }
    }
}

EFI_IFR_OP_HEADER* GetForm (EFI_GUID* Guid, UINT16 FormId, SMC_FORM_DATA* FormArr, UINT32 FormArrSize)
{
    UINT32 Index = 0;
    for (Index = 0; Index < FormArrSize; Index++) {
        if (
            MemCmp (&(FormArr[Index].Guid), Guid, sizeof (EFI_GUID)) == 0 &&
            FormArr[Index].FormId == FormId)
        {
            return FormArr[Index].OpPointer;
            break;
        }
    }
    return NULL;
}

SMC_ROOT_FORM_DATA* GetRootForm (EFI_GUID* Guid, CHAR8* FormTitle)
{
    UINT32 Index = 0;
    for (Index = 0; Index < (mSmcOobPlatformPolicyPtr->OobConfigurations.RootFormCount); Index++) {
        if (
            MemCmp (&(mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].Guid), Guid, sizeof (EFI_GUID)) == 0 &&
            Strcmp (mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].FormTitle, FormTitle) == 0)
        {
            return &(mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index]);
        }
    }
    return NULL;
}

BOOLEAN IsHideFormForSum (EFI_GUID* Guid, CHAR8* FormTitle, UINT16	TargetFormId)
{
    UINT32 Index = 0;
    for (Index = 0; Index < (mSmcOobPlatformPolicyPtr->OobConfigurations.HideFormCount); Index++) {

        if (
            MemCmp (&(mSmcOobPlatformPolicyPtr->OobConfigurations.HideFormForSum[Index].Guid), Guid, sizeof (EFI_GUID)) == 0 &&
            Strcmp (mSmcOobPlatformPolicyPtr->OobConfigurations.HideFormForSum[Index].FormTitle, FormTitle) == 0
		)
        {
			if((mSmcOobPlatformPolicyPtr->OobConfigurations.HideFormForSum[Index].FormId == 0))
            	return TRUE;
			else {
				if((mSmcOobPlatformPolicyPtr->OobConfigurations.HideFormForSum[Index].FormId == TargetFormId))
					return TRUE;	
			}
        }
    }
    return FALSE;
}

VOID AddOpFlag (UINT8* OpPointer, UINT8 Flag)
{
    UINT32 Index = 0;
    for (Index = 0; Index < MAX_REF_COUNT; Index++) {
        if (OpFlag[Index].Pointer == NULL) {
            OpFlag[Index].Pointer = OpPointer;
            OpFlag[Index].Flag = Flag;
            break;
        }
    }
}

UINT8 GetOpFlag (UINT8* OpPointer)
{
    UINT32 Index = 0;
    for (Index = 0; Index < MAX_REF_COUNT; Index++) {
        if (OpFlag[Index].Pointer == OpPointer) {
            return OpFlag[Index].Flag;
        }
    }
    return 0;
}

#if HTMDEBUG2 == 1

UINT8 DumpOpString (UINT32 Level, EFI_IFR_OP_HEADER* Op)
{
    EFI_IFR_OP_HEADER* pOp;
    UINT32 ii = 0;
    CHAR8 StrPrefix[64] = {0};

    pOp = Op;

    for (ii = 0; ii < 8; ii++) {
        a7printf ("%s%s:", BuildPrefix(StrPrefix, Level + 1), IFROPSTRING[pOp->OpCode]);
        if (pOp->OpCode == EFI_IFR_UINT8_OP) {
            a7printf ("Val=0x%x,", ((EFI_IFR_UINT8*)pOp)->Value);
        }
        else if (pOp->OpCode == EFI_IFR_UINT16_OP) {
            a7printf ("Val=0x%x,", ((EFI_IFR_UINT16*)pOp)->Value);
        }
        else if (pOp->OpCode == EFI_IFR_UINT32_OP) {
            a7printf ("Val=0x%x,", ((EFI_IFR_UINT32*)pOp)->Value);
        }
        else if (pOp->OpCode == EFI_IFR_UINT64_OP) {
            a7printf ("Val=0x%x,", ((EFI_IFR_UINT64*)pOp)->Value);
        }
        else if (pOp->OpCode == EFI_IFR_CHECKBOX_OP) {
            a7printf ("Flags=0x%x,", ((EFI_IFR_CHECKBOX*)pOp)->Flags);
        }
        a7printf ("Scope=%d\n", pOp->Scope);
        pOp = (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length);
    }
    return 1;
}

#endif // #if HTMDEBUG2 == 1

EFI_IFR_OP_HEADER* GetScopeEnd(EFI_IFR_OP_HEADER* IfrOpCodeHeader)
{
    EFI_IFR_OP_HEADER* pIfrOpCodeHeader = IfrOpCodeHeader;
    UINTN Scope = 1;
    UINT32 ii = 0;

    while(TRUE) {
        if (ii > 1000000) { // Prevent dead loop.
            //DumpOpString (0, IfrOpCodeHeader);
#if HTMDEBUG2 == 1
            a7printf ("GetScopeEnd : ERROR 01\n");
#endif // #if HTMDEBUG2 == 1
            break;
        }
        if (pIfrOpCodeHeader->Scope != 0) {
            Scope++;
        }
        else if (pIfrOpCodeHeader->OpCode == EFI_IFR_END_OP) {
            Scope--;
            if ( Scope == 0 )
                return pIfrOpCodeHeader;
        }
        pIfrOpCodeHeader = (EFI_IFR_OP_HEADER *) ((UINTN) pIfrOpCodeHeader + pIfrOpCodeHeader->Length);
        ii++;
    }
    //Never Reach Here
#if HTMDEBUG2 == 1
    a7printf ("GetScopeEnd : ERROR 02\n");
#endif // #if HTMDEBUG2 == 1
    return NULL;
}

//
// Calculate expression value.
//
UINT64 CalExpValue (UINT32 Level, EFI_IFR_OP_HEADER* Op)
{
    EFI_IFR_OP_HEADER* pOp;
    EFI_IFR_OP_HEADER* pScopeEnd;
    UINT64 Val1 = 0;
    UINT64 Val2 = 0;
    UINT8 StackStatus = 0;
    CHAR8 StrPrefix[64] = {0};

    pOp = Op;
    pScopeEnd = GetScopeEnd (pOp);

    if (pScopeEnd == NULL) {
#if HTMDEBUG2 == 1
        a7printf ("%sERROR : pScopeEnd is NULL.\n", BuildPrefix(StrPrefix, Level));
#endif // #if HTMDEBUG2 == 1
        return 0;
    }

    InitSmcStack ();

    while (pOp < pScopeEnd) {
#if HTMDEBUG2 == 1
        a7printf ("%s-%s\n", BuildPrefix(StrPrefix, Level), IFROPSTRING[pOp->OpCode]);
#endif // #if HTMDEBUG2 == 1
        if (pOp->OpCode == EFI_IFR_TRUE_OP) {
            SmcPush64 (1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_FALSE_OP) {
            SmcPush64 (0, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_UINT8_OP) {
            SmcPush64 (((EFI_IFR_UINT8*)pOp)->Value, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_UINT16_OP) {
            SmcPush64 (((EFI_IFR_UINT16*)pOp)->Value, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_UINT32_OP) {
            SmcPush64 (((EFI_IFR_UINT32*)pOp)->Value, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_UINT64_OP) {
            SmcPush64 (((EFI_IFR_UINT64*)pOp)->Value, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_EQ_ID_VAL_OP) {
            return 0; // No Support
        }
        else if (pOp->OpCode == EFI_IFR_EQ_ID_VAL_LIST_OP) {
            return 0; // No Support
        }
        else if (pOp->OpCode == EFI_IFR_QUESTION_REF1_OP) {
            return 0; // No Support
        }
        else if (pOp->OpCode == EFI_IFR_EQ_ID_ID_OP) {
            return 0; // No Support
        }
        else if (pOp->OpCode == EFI_IFR_AND_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val1 && Val2;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_OR_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val1 || Val2;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_NOT_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = !Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_GREATER_THAN_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 > Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_GREATER_EQUAL_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 >= Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_LESS_THAN_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 < Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_LESS_EQUAL_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 >= Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_ADD_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 + Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_SUBTRACT_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 - Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_MULTIPLY_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 * Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else if (pOp->OpCode == EFI_IFR_DIVIDE_OP) {
            Val1 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val2 = SmcPop64 (&StackStatus);
            if (!StackStatus) {
                return 0; // Error
            }
            Val1 = Val2 / Val1;
            SmcPush64 (Val1, &StackStatus);
        }
        else {
            break;
        }

        pOp = (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length);
    }

    Val1 = SmcPop64 (&StackStatus);
    if (!StackStatus) {
#if HTMDEBUG2 == 1
        a7printf ("CalExpValue : ERROR SmcStackSize () = %d\n", SmcStackSize ());
#endif // #if HTMDEBUG2 == 1
        return 0; // Error
    }

    return Val1;
}

UINT8 IsHidden (UINT8* LevelFlag, UINT32 CurrentLevel)
{
    UINT32 ii = 0;
    for (ii = 0; ii < MAX_LEVEL_SIZE; ii++) {
        if (ii < CurrentLevel) {
            if (LevelFlag[ii]) return 1;
        }
        else
            break;
    }
    return 0;
}


EFI_IFR_OP_HEADER* ParseOpNode (UINT32* Level, UINT8* LevelFlag, EFI_HII_HANDLE HiiHandle, EFI_GUID* FormsetGuid, EFI_IFR_OP_HEADER* Op)
{
    EFI_IFR_OP_HEADER* pOp;
#if HTMDEBUG2 == 1
    CHAR8 StrBuff[128] = {0};
    CHAR8 StrBuff2[128] = {0};
    CHAR8 StrPrefix[64] = {0};
#endif // #if HTMDEBUG2 == 1

    pOp = Op;
    while (1) {
        if (pOp->OpCode == EFI_IFR_FORM_SET_OP) {
#if HTMDEBUG2 == 1
            SmcGetString (HiiHandle, ((EFI_IFR_FORM_SET*)pOp)->FormSetTitle, StrBuff2);
            ToGuidString (StrBuff, &((EFI_IFR_FORM_SET*)pOp)->Guid);
            a7printf ("%sFormSetTitleId=0x%04x, FormSetTitle=%s, GUID=%s, HiiHandle=0x%x\n", BuildPrefix(StrPrefix, *Level), ((EFI_IFR_FORM_SET*)pOp)->FormSetTitle, StrBuff2, StrBuff, HiiHandle);
#endif // #if HTMDEBUG2 == 1
        }
        if (pOp->OpCode == EFI_IFR_FORM_OP) {
#if HTMDEBUG2 == 1
            SmcGetString (HiiHandle, ((EFI_IFR_FORM*)pOp)->FormTitle, StrBuff2);
            a7printf ("%sFORM - FormId=0x%04x, FormTitle=%s\n", BuildPrefix(StrPrefix, *Level), ((EFI_IFR_FORM*)pOp)->FormId, StrBuff2);
#endif // #if HTMDEBUG2 == 1
        }
        else if (pOp->OpCode == EFI_IFR_REF_OP && pOp->Length == sizeof (EFI_IFR_REF)) {
            if (IsHidden (LevelFlag, *Level)) {
                AddOpFlag ((UINT8*)pOp, 1);
#if HTMDEBUG2 == 1
                a7printf (
                    "%sIsHidden=0x%x\n",
                    BuildPrefix(StrPrefix, *Level),
                    1);
#endif // #if HTMDEBUG2 == 1
            }
#if HTMDEBUG2 == 1
            SmcGetString (HiiHandle, ((EFI_IFR_REF*)pOp)->Question.Header.Prompt, StrBuff2);
            a7printf (
                "%sREF - FormId=0x%04x, %s, 0x%x, OpFlag=0x%x\n",
                BuildPrefix(StrPrefix, *Level),
                ((EFI_IFR_REF*)pOp)->FormId,
                StrBuff2,
                pOp,
                GetOpFlag ((UINT8*)pOp));
#endif // #if HTMDEBUG2 == 1
        }
        else if (pOp->OpCode == EFI_IFR_REF_OP && pOp->Length == sizeof (EFI_IFR_REF3)) {
            if (IsHidden (LevelFlag, *Level)) {
                AddOpFlag ((UINT8*)pOp, 1);
#if HTMDEBUG2 == 1
                a7printf (
                    "%sIsHidden=0x%x\n",
                    BuildPrefix(StrPrefix, *Level),
                    1);
#endif // #if HTMDEBUG2 == 1
            }
#if HTMDEBUG2 == 1
            SmcGetString (HiiHandle, ((EFI_IFR_REF3*)pOp)->Question.Header.Prompt, StrBuff2);
            ToGuidString (StrBuff, &((EFI_IFR_REF3*)pOp)->FormSetId);
            a7printf (
                "%sREF3 - FormId=0x%04x, GUID=%s, %s, 0x%x, OpFlag=0x%x\n",
                BuildPrefix(StrPrefix, *Level),
                ((EFI_IFR_REF3*)pOp)->FormId,
                StrBuff,
                StrBuff2,
                pOp,
                GetOpFlag ((UINT8*)pOp));
#endif // #if HTMDEBUG2 == 1
        }
        else if (pOp->OpCode == EFI_IFR_SUPPRESS_IF_OP) {
#if HTMDEBUG2 == 1
            a7printf ("%sSUPPRESS_IF\n", BuildPrefix(StrPrefix, *Level));
#endif // #if HTMDEBUG2 == 1
            LevelFlag[*Level] = (UINT8)CalExpValue (*Level, (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length));
#if HTMDEBUG2 == 1
            a7printf ("%sLevelFlag[%d]=0x%x\n", BuildPrefix(StrPrefix, *Level), *Level, LevelFlag[*Level]);
#endif // #if HTMDEBUG2 == 1
        }
        else if (pOp->OpCode == EFI_IFR_DISABLE_IF_OP) {
#if HTMDEBUG2 == 1
            a7printf ("%sDISABLE_IF\n", BuildPrefix(StrPrefix, *Level));
#endif // #if HTMDEBUG2 == 1
            LevelFlag[*Level] = (UINT8)CalExpValue (*Level, (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length));
#if HTMDEBUG2 == 1
            a7printf ("%sLevelFlag[%d]=0x%x\n", BuildPrefix(StrPrefix, *Level), *Level, LevelFlag[*Level]);
#endif // #if HTMDEBUG2 == 1
        }
        else if (pOp->OpCode == EFI_IFR_END_OP) {
			(*Level) = ((*Level) > 0) ? (*Level)-1 : 0; 
            if (LevelFlag[*Level]) LevelFlag[*Level] = 0;
            break;
        }

        if (pOp->Scope == 1) {
            (*Level)++;
            pOp = ParseOpNode (Level, LevelFlag, HiiHandle, FormsetGuid, (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length));
        }

        pOp = (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length);
    }

    return pOp;
}

EFI_IFR_OP_HEADER* ParseRefNode (
    IN OUT UINT8* BuffAddress,
    IN OUT UINTN* BuffSize,
    UINT32* Level,
    EFI_HII_HANDLE HiiHandle,
    EFI_GUID* FormsetGuid,
    UINT16 FormId,
    EFI_IFR_OP_HEADER* Op)
{
    EFI_IFR_OP_HEADER* pOp;
    UINT32 OpScope = 0;
    UINT8* pBuffAddress = NULL;
    UINT32 RefCount = 0;
    UINTN TmpSize = 0;
    UINT8* pOriBuffAddress = NULL;
    CHAR8 GuidStrBuff[128] = {0};
    CHAR8 TitleStrBuff[128] = {0};
    CHAR8 StrPrefix[64] = {0};
#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
    CHAR16 StrPrefixW[64] = {0};
    CHAR16 StrBuffW[128] = {0};
    CHAR16 StrBuffGuidW[128] = {0};
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

    pBuffAddress = BuffAddress;

    //
    // Produce Json code.
    //
    TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"Form\":[\n", BuildPrefix(StrPrefix, *Level - 1));
    pBuffAddress += TmpSize;

    pOp = Op;
    while (1) {
        if (pOp->OpCode == EFI_IFR_REF_OP && pOp->Length == sizeof (EFI_IFR_REF)) {
            EFI_IFR_OP_HEADER* pTarget = NULL;

            if (!GetOpFlag ((UINT8*)pOp)) {
                //
                // Target Form ID  - ((EFI_IFR_REF*)pOp)->FormId
                // Current Form ID - FormId
                // Target Form ID must be different from Current Form ID.
                // Prevent to goto itself.
                //
                if (((EFI_IFR_REF*)pOp)->FormId != FormId) {
                    SmcGetString (HiiHandle, ((EFI_IFR_REF*)pOp)->Question.Header.Prompt, TitleStrBuff);
                    ToGuidString (GuidStrBuff, FormsetGuid);
#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
                    AsciiStrToUnicodeStr (TitleStrBuff, StrBuffW);
                    AsciiStrToUnicodeStr (GuidStrBuff, StrBuffGuidW);
                    OOB_DEBUG((-1,
                               "%sREF - FormId=0x%04x, GUID=%s, %s, 0x%x\n",
                               BuildPrefixW(StrPrefixW, *Level),
                               ((EFI_IFR_REF*)pOp)->FormId,
                               StrBuffGuidW,
                               StrBuffW,
                               pOp));
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
                    if (!IsHideFormForSum (FormsetGuid, TitleStrBuff,((EFI_IFR_REF*)pOp)->FormId)) {

                        pOriBuffAddress = pBuffAddress;

                        //
                        // Produce Json code.
                        //
                        TmpSize = Sprintf_s (pBuffAddress, 128, "%s{\n", BuildPrefix(StrPrefix, *Level));
                        pBuffAddress += TmpSize;

                        TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"GUID\": \"%s\",\n", BuildPrefix(StrPrefix, *Level), GuidStrBuff);
                        pBuffAddress += TmpSize;

                        TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"FormId\": \"0x%x\",\n", BuildPrefix(StrPrefix, *Level), ((EFI_IFR_REF*)pOp)->FormId);
                        pBuffAddress += TmpSize;

                        TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"Title\": \"%s\",\n", BuildPrefix(StrPrefix, *Level), TitleStrBuff);
                        pBuffAddress += TmpSize;

                        RefCount++;

                        pTarget = GetForm (FormsetGuid, ((EFI_IFR_REF*)pOp)->FormId, FormData, MAX_FORM_COUNT);
                        if (pTarget != NULL) {
                            (*Level)++;
                            TmpSize = 0;
                            ParseRefNode (
                                pBuffAddress,
                                &TmpSize,
                                Level,
                                HiiHandle,
                                FormsetGuid,
                                ((EFI_IFR_REF*)pOp)->FormId,
                                (EFI_IFR_OP_HEADER*)((UINT8*)pTarget + pTarget->Length));
                            if (TmpSize) {
                                pBuffAddress += TmpSize;
                            }
                            else {
                                //
                                // Clear tail ',' char.
                                //
                                pBuffAddress--;
                                *pBuffAddress = 0;
                                *(pBuffAddress - 1) = '\n';
                            }

                            //
                            // Produce Json code.
                            //
                            TmpSize = Sprintf_s (pBuffAddress, 128, "%s},\n", BuildPrefix(StrPrefix, *Level));
                            pBuffAddress += TmpSize;
                        }
                        else {

                            OOB_DEBUG((-1, "%spTarget == NULL\n", BuildPrefixW(StrPrefixW, *Level)));

                            //
                            // No target form, clear current ref Json code.
                            //
                            MemSet (pOriBuffAddress, (UINT32)(pBuffAddress - pOriBuffAddress), 0);
                            pBuffAddress = pOriBuffAddress;
                            RefCount--;
                        }
                    }
                }
            }
        }
        else if (pOp->OpCode == EFI_IFR_REF_OP && pOp->Length == sizeof (EFI_IFR_REF3)) {
            EFI_IFR_OP_HEADER* pTarget = NULL;
            EFI_HII_HANDLE TargetHiiHandle = NULL;

            if (!GetOpFlag ((UINT8*)pOp)) {
                SmcGetString (HiiHandle, ((EFI_IFR_REF3*)pOp)->Question.Header.Prompt, TitleStrBuff);
                ToGuidString (GuidStrBuff, &((EFI_IFR_REF3*)pOp)->FormSetId);

#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
                AsciiStrToUnicodeStr (TitleStrBuff, StrBuffW);
                AsciiStrToUnicodeStr (GuidStrBuff, StrBuffGuidW);
                OOB_DEBUG((-1,
                           "%sREF3 - FormId=0x%04x, GUID=%s, %s, 0x%x\n",
                           BuildPrefixW(StrPrefixW, *Level),
                           ((EFI_IFR_REF3*)pOp)->FormId,
                           StrBuffGuidW,
                           StrBuffW,
                           pOp));
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

                if (!IsHideFormForSum (&((EFI_IFR_REF3*)pOp)->FormSetId, TitleStrBuff,((EFI_IFR_REF*)pOp)->FormId)) {

                    pOriBuffAddress = pBuffAddress;

                    //
                    // Produce Json code.
                    //
                    TmpSize = Sprintf_s (pBuffAddress, 128, "%s{\n", BuildPrefix(StrPrefix, *Level));
                    pBuffAddress += TmpSize;

                    TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"GUID\": \"%s\",\n", BuildPrefix(StrPrefix, *Level), GuidStrBuff);
                    pBuffAddress += TmpSize;

                    TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"FormId\": \"0x%x\",\n", BuildPrefix(StrPrefix, *Level), ((EFI_IFR_REF3*)pOp)->FormId);
                    pBuffAddress += TmpSize;

                    TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"Title\": \"%s\",\n", BuildPrefix(StrPrefix, *Level), TitleStrBuff);
                    pBuffAddress += TmpSize;

                    RefCount++;

                    pTarget = GetForm (&((EFI_IFR_REF3*)pOp)->FormSetId, ((EFI_IFR_REF*)pOp)->FormId, FormData, MAX_FORM_COUNT);
                    if (pTarget != NULL) {
                        (*Level)++;
                        TargetHiiHandle = GetGuidHandle (&((EFI_IFR_REF3*)pOp)->FormSetId);
                        TmpSize = 0;
                        ParseRefNode (
                            pBuffAddress,
                            &TmpSize,
                            Level,
                            TargetHiiHandle,
                            &((EFI_IFR_REF3*)pOp)->FormSetId,
                            ((EFI_IFR_REF*)pOp)->FormId,
                            (EFI_IFR_OP_HEADER*)((UINT8*)pTarget + pTarget->Length));
                        if (TmpSize) {
                            pBuffAddress += TmpSize;
                        }
                        else {
                            //
                            // Clear tail ',' char.
                            //
                            pBuffAddress--;
                            *pBuffAddress = 0;
                            *(pBuffAddress - 1) = '\n';
                        }

                        //
                        // Produce Json code.
                        //
                        TmpSize = Sprintf_s (pBuffAddress, 128, "%s},\n", BuildPrefix(StrPrefix, *Level));
                        pBuffAddress += TmpSize;
                    }
                    else {

                        OOB_DEBUG((-1, "%spTarget == NULL\n", BuildPrefixW(StrPrefixW, *Level)));

                        //
                        // No target form, clear current ref Json code.
                        //
                        MemSet (pOriBuffAddress, (UINT32)(pBuffAddress - pOriBuffAddress), 0);
                        pBuffAddress = pOriBuffAddress;
                        RefCount--;
                    }
                }
            }
        }
        else if (pOp->OpCode == EFI_IFR_END_OP) {
            if (OpScope == 0) { // Reach the form's end, break loop.
                (*Level)--;
                break;
            }
            OpScope--;
        }

        if (pOp->Scope == 1) {
            OpScope++;
        }

        pOp = (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length);
    }

    if (RefCount) {
        //
        // Clear tail ',' char.
        //
        pBuffAddress--;
        *pBuffAddress = 0;
        *(pBuffAddress - 1) = '\n';

        //
        // Produce Json code.
        //
        TmpSize = Sprintf_s (pBuffAddress, 128, "%s]\n", BuildPrefix(StrPrefix, *Level));
        pBuffAddress += TmpSize;
    }
    else {
        //
        // Clear the string - "%s\"Form\":[\n".
        //
        MemSet (BuffAddress, (UINT32)(pBuffAddress - BuffAddress), 0);
        pBuffAddress = BuffAddress;
    }

    //
    // Calculate total size.
    //
    *BuffSize = (UINT32)(pBuffAddress - BuffAddress);

    return pOp;
}

EFI_STATUS InitFormData (UINT8* BuffAddress, UINT32 BuffSize)
{
    EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList = NULL;
    EFI_HII_PACKAGE_HEADER *IfrPackagePtr = NULL;
    UINT8* pHpkData = NULL;
    EFI_IFR_OP_HEADER* pEfiIfrOpHeader;
    EFI_IFR_FORM_SET* pFormSet = NULL;
    CHAR8 FormSetTitle[64] = {0};
    CHAR8 FormTitle[64] = {0};
    EFI_HII_HANDLE HiiHandle;
#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
    CHAR8 StrBuff[128] = {0};
    CHAR16 StrBuffW[128] = {0};
    CHAR16 StrBuffGuidW[128] = {0};
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

    OOB_DEBUG((-1, "InitFormData entry.\n"));

    //
    // Clear form list data.
    //
    MemSet ((UINT8*)&(FormData[0]), sizeof (SMC_FORM_DATA) * MAX_FORM_COUNT, 0);

    //
    // Clear main form list data.
    //
    //SetMem ((UINT8*)&(MainFormData[0]), sizeof (SMC_FORM_DATA) * MAX_MAIN_FORM_COUNT, 0);

#if HTMDEBUG2 == 1
    a7printf ("InitFormData : 05 FormData = 0x%x\n", FormData);
    a7printf ("InitFormData : 06 &(FormData[0]) = 0x%x\n", &(FormData[0]));
#endif // #if HTMDEBUG2 == 1

    //
    // Loop all HPK list.
    //
    HiiPackageList = (EFI_HII_PACKAGE_LIST_HEADER*)BuffAddress;

    OOB_DEBUG((-1, "BuffSize = 0x%x\n", BuffSize));

    while ((UINT8*)HiiPackageList - BuffAddress < BuffSize) {

        OOB_DEBUG((-1, "(UINT8*)HiiPackageList - BuffAddress = 0x%x\n", (UINT8*)HiiPackageList - BuffAddress));

        for (
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList+1);
            IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList + HiiPackageList->PackageLength);
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr + IfrPackagePtr->Length)) {
            OOB_DEBUG((-1, "  IfrPackagePtr->Type = 0x%x\n", IfrPackagePtr->Type));
            if (IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) {

                pFormSet = NULL;
                HiiHandle = NULL;
                MemSet ((UINT8*)&(FormSetTitle[0]), sizeof (CHAR8) * 64, 0);

                pHpkData = (UINT8*)IfrPackagePtr + sizeof (EFI_HII_PACKAGE_HEADER);
                pEfiIfrOpHeader = (EFI_IFR_OP_HEADER*)pHpkData;
                if (pEfiIfrOpHeader->OpCode == EFI_IFR_FORM_SET_OP) {
                    //
                    // Record formset pointer.
                    //
                    pFormSet = (EFI_IFR_FORM_SET*)pEfiIfrOpHeader;
                    HiiHandle = GetGuidHandle (&(pFormSet->Guid));
                    SmcGetString (HiiHandle, pFormSet->FormSetTitle, FormSetTitle);
#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
                    AsciiStrToUnicodeStr (FormSetTitle, StrBuffW);
                    ToGuidString (StrBuff, &(pFormSet->Guid));
                    AsciiStrToUnicodeStr (StrBuff, StrBuffGuidW);
                    OOB_DEBUG((-1,
                               "  FormSetTitle=%s, GUID=%s, HiiHandle=0x%x\n",
                               StrBuffW,
                               StrBuffGuidW,
                               HiiHandle));
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
                }

                //
                // Search for all forms.
                //
                OOB_DEBUG((-1, "  Search for all forms.\n"));
                for (
                    pEfiIfrOpHeader = (EFI_IFR_OP_HEADER*)pHpkData;
                    pEfiIfrOpHeader < (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + IfrPackagePtr->Length);
                    pEfiIfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)pEfiIfrOpHeader + pEfiIfrOpHeader->Length)) {
                    if (pEfiIfrOpHeader->OpCode == EFI_IFR_FORM_OP) {
                        if (pFormSet != NULL && HiiHandle != NULL) {
                            SMC_ROOT_FORM_DATA* pRootForm = NULL;
                            MemSet ((UINT8*)&(FormTitle[0]), sizeof (CHAR8) * 64, 0);
                            SmcGetString (HiiHandle, ((EFI_IFR_FORM*)pEfiIfrOpHeader)->FormTitle, FormTitle);
                            /*if (AsciiStrCmp (FormSetTitle, FormTitle) == 0) { // If FormTitle is same as FormSetTitle.

                                //
                                // Add main form data to list.
                                //
                                AddForm (
                                    &(pFormSet->Guid),
                                    ((EFI_IFR_FORM*)pEfiIfrOpHeader)->FormId,
                                    pEfiIfrOpHeader,
                                    MainFormData,
                                    MAX_MAIN_FORM_COUNT);
                            }*/
							
                            pRootForm = GetRootForm (&(pFormSet->Guid), FormTitle);
                            if (pRootForm != NULL) {
                                pRootForm->FormId = ((EFI_IFR_FORM*)pEfiIfrOpHeader)->FormId;
                                pRootForm->OpPointer = pEfiIfrOpHeader;
								DEBUG((-1,"[SMC_OOB] :: pRootForm != NULL, FormId = %x, OpPointer = %x\n",pRootForm->FormId,(UINT32)pRootForm->OpPointer));
                            }

#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
                            AsciiStrToUnicodeStr (FormTitle, StrBuffW);
                            OOB_DEBUG ((-1,
                                        "    FormTitle=%s, FormId=0x%04x\n",
                                        StrBuffW,
                                        ((EFI_IFR_FORM*)pEfiIfrOpHeader)->FormId));
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

                            //
                            // Add form data to list.
                            //
                            AddForm (
                                &(pFormSet->Guid),
                                ((EFI_IFR_FORM*)pEfiIfrOpHeader)->FormId,
                                pEfiIfrOpHeader,
                                FormData,
                                MAX_FORM_COUNT);
                        }
                    }
                }
            }
        }

        HiiPackageList = (EFI_HII_PACKAGE_LIST_HEADER*)((UINT8*)HiiPackageList + HiiPackageList->PackageLength);
    }

    OOB_DEBUG((-1, "InitFormData end.\n"));

    return EFI_SUCCESS;
}

EFI_STATUS GenFormLayout (
    IN UINT8* HiiData,
    IN UINT32 HiiDataSize,
    OUT UINT8* BuffAddress,
    OUT UINT32* BuffSize)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList = NULL;
    EFI_HII_PACKAGE_HEADER *IfrPackagePtr = NULL;
    UINT8* pHpkData = NULL;
    EFI_IFR_OP_HEADER* pEfiIfrOpHeader;
    EFI_HII_HANDLE HiiHandle;
    UINT32 Level = 0;
    UINT8 LevelFlag[MAX_LEVEL_SIZE] = {0};
    UINT32 Index = 0;
    UINT8* pBuffAddress = NULL;
    EFI_HII_PACKAGE_LIST_HEADER* pFlHpkList = NULL;
    EFI_HII_PACKAGE_HEADER* pFlHpk = NULL;
    UINTN TmpSize = 0;

    UINT32 RefCount = 0;
    CHAR8 StrBuff[128] = {0};
    CHAR8 GuidStrBuff[128] = {0};
    CHAR8 StrPrefix[64] = {0};
#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
    CHAR8* pTemp = NULL;
    CHAR8 StrBuff2[128] = {0};
    CHAR16 StrBuffW[128] = {0};
    CHAR16 StrBuffGuidW[128] = {0};
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

    OOB_DEBUG((-1, "GenFormLayout entry.\n"));

    //
    // Copy HII data to output buffer.
    //
    MemCpy (BuffAddress, HiiData, HiiDataSize);
    pBuffAddress = BuffAddress + HiiDataSize;

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof (SMC_FORM_DATA) * MAX_FORM_COUNT, &FormData);
    if (EFI_ERROR (Status)) {
        OOB_DEBUG((-1, "ERROR : Allocating memory for FormData is failed. Status = %r\n", Status));
        return Status;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof (SMC_OP_FLAG) * MAX_REF_COUNT, &OpFlag);
    if (EFI_ERROR (Status)) {
        OOB_DEBUG((-1, "ERROR : Allocating memory for OpFlag is failed. Status = %r\n", Status));
        if (FormData != NULL) pBS->FreePool((UINT8*)FormData);
        return Status;
    }

    //
    // Clear OpFlag.
    //
    MemSet ((UINT8*)OpFlag, sizeof (SMC_OP_FLAG) * MAX_REF_COUNT, 0);

    //
    // Init form list data.
    //
    InitFormData (HiiData, HiiDataSize);

    OOB_DEBUG((-1, "\n"));
    OOB_DEBUG((-1, "Loop all HPK list...\n"));

    //
    // Loop all HPK list.
    //
    HiiPackageList = (EFI_HII_PACKAGE_LIST_HEADER*)HiiData;

    while ((UINT8*)HiiPackageList - HiiData < HiiDataSize) {

        for (
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList+1);
            IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList + HiiPackageList->PackageLength);
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr + IfrPackagePtr->Length)) {
            if (IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) {
                pHpkData = (UINT8*)IfrPackagePtr + sizeof (EFI_HII_PACKAGE_HEADER);
                pEfiIfrOpHeader = (EFI_IFR_OP_HEADER*)pHpkData;
                HiiHandle = 0;
                if (pEfiIfrOpHeader->OpCode == EFI_IFR_FORM_SET_OP) {
#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
                    HiiHandle = GetGuidHandle (&(((EFI_IFR_FORM_SET*)pEfiIfrOpHeader)->Guid));
                    SmcGetString (HiiHandle, ((EFI_IFR_FORM_SET*)pEfiIfrOpHeader)->FormSetTitle, StrBuff2);
                    ToGuidString (StrBuff, &((EFI_IFR_FORM_SET*)pEfiIfrOpHeader)->Guid);
                    AsciiStrToUnicodeStr (StrBuff2, StrBuffW);
                    AsciiStrToUnicodeStr (StrBuff, StrBuffGuidW);
                    OOB_DEBUG((-1,
                               "FormSetTitleId=0x%04x, FormSetTitle=%s, GUID=%s, HiiHandle=0x%x\n",
                               ((EFI_IFR_FORM_SET*)pEfiIfrOpHeader)->FormSetTitle,
                               StrBuffW,
                               StrBuffGuidW,
                               HiiHandle));
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

                    //
                    // Clear LevelFlag.
                    //
                    MemSet ((UINT8*)&(LevelFlag[0]), sizeof (UINT8) * MAX_LEVEL_SIZE, 0);
                    Level = 0;

                    ParseOpNode (
                        &Level,
                        LevelFlag,
                        HiiHandle,
                        &(((EFI_IFR_FORM_SET*)pEfiIfrOpHeader)->Guid), // FormSet GUID
                        (EFI_IFR_OP_HEADER*)((UINT8*)pEfiIfrOpHeader + pEfiIfrOpHeader->Length) // FormSet Content pointer
                    );
                }
            }
        }

        HiiPackageList = (EFI_HII_PACKAGE_LIST_HEADER*)((UINT8*)HiiPackageList + HiiPackageList->PackageLength);
    }

    OOB_DEBUG((-1, "\n"));
    OOB_DEBUG((-1, "Loop all HPK list is done\n"));

    //
    // Create HII package list header.
    //
    pFlHpkList = (EFI_HII_PACKAGE_LIST_HEADER*)pBuffAddress;
    MemCpy (&(pFlHpkList->PackageListGuid), &FormLayoutGuid, sizeof (EFI_GUID));
    pBuffAddress += sizeof (EFI_HII_PACKAGE_LIST_HEADER);

    //
    // Create HII package header.
    //
    pFlHpk = (EFI_HII_PACKAGE_HEADER*)pBuffAddress;
    pFlHpk->Type = 0xF0;
    pBuffAddress += sizeof (EFI_HII_PACKAGE_HEADER);

#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
    pTemp = pBuffAddress;
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

    //
    // Produce Json code.
    //
    TmpSize = Sprintf_s (pBuffAddress, 128, "{\n");
    pBuffAddress += TmpSize;
    TmpSize = Sprintf_s (pBuffAddress, 128, "  \"FormLayout\":{\n");
    pBuffAddress += TmpSize;

    //
    // Loop all root forms.
    //

    //
    // Produce Json code.
    //
    TmpSize = Sprintf_s (pBuffAddress, 128, "    \"Form\":[\n");
    pBuffAddress += TmpSize;
	
	DEBUG((-1,"[SMC_OOB] :: RootFormCount = %x\n",mSmcOobPlatformPolicyPtr->OobConfigurations.RootFormCount));

    for (Index = 0; Index < (mSmcOobPlatformPolicyPtr->OobConfigurations.RootFormCount); Index++) {
		
		DEBUG((-1,"[SMC_OOB] :: RootForm :: Index[%x], Guid = %g, FormTitle = %a\n",Index,
				mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].Guid,
				mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].FormTitle));

        if (mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].OpPointer == NULL || mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].FormId == 0)
            continue;

        HiiHandle = GetGuidHandle (&(mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].Guid));
#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
        SmcGetString (HiiHandle, ((EFI_IFR_FORM*)(mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].OpPointer))->FormTitle, StrBuff);
        AsciiStrToUnicodeStr (StrBuff, StrBuffW);
        OOB_DEBUG((-1,
                   "GenFormLayout : 01 %s, RootForm[%d].FormId = 0x%04x\n",
                   StrBuffW,
                   Index,
                   mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].FormId));
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

        //
        // Produce Json code.
        //
        TmpSize = Sprintf_s (pBuffAddress, 128, "%s{\n", BuildPrefix(StrPrefix, 3));
        pBuffAddress += TmpSize;

        ToGuidString (GuidStrBuff, &(mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].Guid));
        TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"GUID\": \"%s\",\n", BuildPrefix(StrPrefix, 4), GuidStrBuff);
        pBuffAddress += TmpSize;

        TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"FormId\": \"0x%x\",\n", BuildPrefix(StrPrefix, 4), mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].FormId);
        pBuffAddress += TmpSize;

        TmpSize = Sprintf_s (pBuffAddress, 128, "%s\"Title\": \"%s\",\n", BuildPrefix(StrPrefix, 4), StrBuff);
        pBuffAddress += TmpSize;

        Level = 5;

        ParseRefNode (
            pBuffAddress,
            &TmpSize,
            &Level,
            HiiHandle,
            &(mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].Guid),
            mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].FormId,
            (EFI_IFR_OP_HEADER*)((UINT8*)(mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].OpPointer) + mSmcOobPlatformPolicyPtr->OobConfigurations.RootForm[Index].OpPointer->Length));
        if (TmpSize) {
            pBuffAddress += TmpSize;
        }
        else {
            //
            // Clear tail ',' char.
            //
            pBuffAddress--;
            *pBuffAddress = 0;
            *(pBuffAddress - 1) = '\n';
        }

        //
        // Produce Json code.
        //
        TmpSize = Sprintf_s (pBuffAddress, 128, "%s},\n", BuildPrefix(StrPrefix, 3));
        pBuffAddress += TmpSize;

        RefCount++;
    }

    if (RefCount) {
        //
        // Clear tail ',' char.
        //
        pBuffAddress--;
        *pBuffAddress = 0;
        *(pBuffAddress - 1) = '\n';
    }

    //
    // Produce Json code.
    //
    TmpSize = Sprintf_s (pBuffAddress, 128, "    ]\n");
    pBuffAddress += TmpSize;

    TmpSize = Sprintf_s (pBuffAddress, 128, "  }\n");
    pBuffAddress += TmpSize;

    TmpSize = Sprintf_s (pBuffAddress, 128, "}\n");
    pBuffAddress += TmpSize;

    //
    // Fill header size.
    //
    pFlHpk->Length = (UINT32)(pBuffAddress - (UINT8*)pFlHpk);
    pFlHpkList->PackageLength = (UINT32)(pBuffAddress - (UINT8*)pFlHpkList);

    //
    // Calculate total size.
    //
    *BuffSize = (UINT32)(pBuffAddress - BuffAddress);

#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
    OOB_DEBUG((-1, "===== Json Begin ===== \n"));
    {
        UINT8* ppTemp = pTemp;
        CHAR8 StrBuff5[33];
        while (ppTemp < pBuffAddress) {
            MemCpy (StrBuff5, ppTemp, 32);
            StrBuff5[32] = 0;
            AsciiStrToUnicodeStr (StrBuff5, StrBuffW);
            OOB_DEBUG((-1, "%s", StrBuffW));
            ppTemp+=32;
        }
    }
    OOB_DEBUG((-1, "===== Json End ===== \n"));
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

    if (FormData != NULL) pBS->FreePool((UINT8*)FormData);
    if (OpFlag != NULL) pBS->FreePool((UINT8*)OpFlag);

    OOB_DEBUG((-1, "GenFormLayout end.\n"));

    return EFI_SUCCESS;
}


