//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Change HideItem policy to fix TXT can being enabled.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/07/2017
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include "minisetup.h"
#include "SmcSetupModify.h" // Supermicro

extern CONTROL_METHODS gControl;


SMC_SETUP_MODIFY_PROTOCOL* SmcSetupModifyProtocol = NULL;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  OemInitialize
//
// Description:
//  OEM init function for "gControl".
//
//  Steps:
//  1. Get the hide item list from "gEfiSuperMDriverGuid".
//  2. Check the item is in the list. If it's true, return "EFI_UNSUPPORTED".
//
// Input:
//      CONTROL_DATA *control
//      VOID *data
//
// Output:
//      EFI_STATUS (return) -
//          EFI_SUCCESS : Success
//          EFI_UNSUPPORTED : Hide the item
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
OemInitialize(
    CONTROL_DATA	*control,
    VOID		*data
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CONTROL_INFO *CtrlData;
    CHAR8 *formSetTitle = NULL;
    EFI_IFR_OP_HEADER* pIfrOpHeader = NULL;
    UINT16 StrId = 0;

    Status = gObject.Initialize(control, data);
    if(EFI_ERROR(Status)) return Status;

    CtrlData = (CONTROL_INFO*)data;

    if((UINT32)(CtrlData->ControlHandle) == 0xFFFFFFFF)	goto Done;

    pIfrOpHeader = (EFI_IFR_OP_HEADER*)(CtrlData->ControlPtr);

    switch(pIfrOpHeader->OpCode) {
    case EFI_IFR_ONE_OF_OP :
    {
        EFI_IFR_ONE_OF* pOneOf = (EFI_IFR_ONE_OF*)pIfrOpHeader;
        StrId = pOneOf->Question.Header.Prompt;
        break;
    }
    case EFI_IFR_CHECKBOX_OP :
    {
        EFI_IFR_CHECKBOX* pCheckBox = (EFI_IFR_CHECKBOX*)pIfrOpHeader;
        StrId = pCheckBox->Question.Header.Prompt;
        break;
    }
    case EFI_IFR_NUMERIC_OP :
    {
        EFI_IFR_NUMERIC* pNumeric = (EFI_IFR_NUMERIC*)pIfrOpHeader;
        StrId = pNumeric->Question.Header.Prompt;
        break;
    }
    case EFI_IFR_REF_OP :
    {
        EFI_IFR_REF* pRef = (EFI_IFR_REF*)pIfrOpHeader;
        StrId = pRef->Question.Header.Prompt;
        break;
    }
    case EFI_IFR_SUBTITLE_OP:
    {
        EFI_IFR_SUBTITLE* pSubTitle = (EFI_IFR_SUBTITLE*)pIfrOpHeader;
        StrId = pSubTitle->Statement.Prompt;
        break;
    }
    }

    //
    // Check the control is in the hide item list. If yes, hide it.
    //
    //if (SmcSetupModifyProtocol->SearchForHideList (CtrlData->ControlHandle, CtrlData->QuestionId)) {
    if (SmcSetupModifyProtocol->SearchForHideList (CtrlData->ControlHandle, StrId)) {
        CtrlData->ControlFlags.ControlVisible = 0;
#if TSE_DEBUG_MESSAGES
        DEBUG((-1, "OemInitialize HideItem - 0x%x,0x%04x,0x%04x\n", CtrlData->ControlHandle, CtrlData->QuestionId, StrId));
#endif
    }

Done:

    if(CtrlData->ControlPtr == NULL) return EFI_UNSUPPORTED;

    if(!CtrlData->ControlFlags.ControlVisible)
        Status = EFI_UNSUPPORTED;
    else
        MemCopy(&control->ControlData, data, sizeof(CONTROL_INFO));

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  OemMinisetupDriverEntryHook
//
// Description:
//  Hook the function after "MinisetupDriverEntryHookHook".
//
// Input:
//      None
//
// Output:
//      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
OemMinisetupDriverEntryHook(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;

#if TSE_DEBUG_MESSAGES
    DEBUG((-1, "OemMinisetupDriverEntryHook Start.\n"));
#endif

    gControl.Initialize = OemInitialize;

    //
    // Locate protocol - gSetupModifyProtocolGuid.
    //
    Status = pBS->LocateProtocol(&gSetupModifyProtocolGuid , NULL, &SmcSetupModifyProtocol);

#if TSE_DEBUG_MESSAGES
    DEBUG((-1, "OemMinisetupDriverEntryHook End.\n"));
#endif
}
