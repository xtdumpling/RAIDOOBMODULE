//***********************************************************************
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

/** @file FixedBootOrderStyle.c

    Replace TSE funtion Popupsel.Draw PopupSel.HandleAction
    Frame.HandleAction

*/

#include <minisetup.h>
#include <autoid.h>
#include <FixedBootOrder.h>
#include <Setup.h>

#define	VARIABLE_ID_SETUP 0

UINT16 LegacyBootItemCount=1;
UINT16 UefiBootItemCount=1;
UINT16 DualBootItemCount=1;

extern NVRAM_VARIABLE *gVariableList;
extern UINT8 gTseSkipFBOModule ;

VOID _PopupSelGetSelection( POPUPSEL_DATA *popupSel );
EFI_STATUS _FrameHandleKey( FRAME_DATA *frame, AMI_EFI_KEY_DATA Key );
EFI_STATUS _FrameScroll( FRAME_DATA *frame, BOOLEAN bScrollUp );

EFI_STATUS 
CheckFixedBootOrder
( 
  CONTROL_DATA *control, 
  FRAME_DATA *frame, 
  AMI_EFI_KEY_DATA key
)
{
    POPUPSEL_DATA *popupsel=NULL;
    CONTROL_ACTION Action;
    UINT16 StartKey, EndKey;

    popupsel = (POPUPSEL_DATA*)control;

    //Get mapping
    Action = MapControlKeysHook(key);

    if ( Action == ControlActionDecreament
            || Action == ControlActionIncreament )
    {
        if ( popupsel->CallBackKey >= LegacyBootItemKey
                && popupsel->CallBackKey < LegacyBootItemKey+LegacyBootItemCount )
        {
            StartKey=LegacyBootItemKey;
            EndKey=(LegacyBootItemKey+LegacyBootItemCount)-1;
        }
        else if ( popupsel->CallBackKey >= UefiBootItemKey
                  && popupsel->CallBackKey < UefiBootItemKey+UefiBootItemCount )
        {
            StartKey=UefiBootItemKey;
            EndKey=(UefiBootItemKey+UefiBootItemCount)-1;
        }
#if FBO_DUAL_MODE
        else if ( popupsel->CallBackKey >= DualBootItemKey
                  && popupsel->CallBackKey < DualBootItemKey+DualBootItemCount )
        {
            StartKey=DualBootItemKey;
            EndKey=(DualBootItemKey+DualBootItemCount)-1;
            TRACE((FBO_TRACE_LEVEL,"CheckFixedBootOrder DUAL..\n"));
        }
#endif
    }


    switch (Action)
    {
    case ControlActionDecreament:
        if (popupsel->CallBackKey == StartKey)
            break;
        _FrameScroll( frame, TRUE);
        break;

    case ControlActionIncreament:
        if (popupsel->CallBackKey == EndKey)
            break;
        _FrameScroll( frame, FALSE);
        break;

    default:
        return EFI_ABORTED;
    }

    return EFI_SUCCESS;
}

EFI_STATUS OemBootOrderPopupSelDraw( POPUPSEL_DATA *popupSel )
{
    CHAR16 *text=NULL,*text1=NULL;
    UINTN i,Len=0;
    EFI_STATUS Status = EFI_SUCCESS;
    UINT16 Index = 0;
    UINT16 *OptionList=NULL;
    UINT64 *ValueList=NULL;
    SETUP_DATA *pSetup;
    UINT8 ColorNSel = popupSel->NSelFGColor;
    UINT8 ColorLabel = (popupSel->ControlFocus) ? popupSel->LabelFGColor : popupSel->NSelLabelFGColor ;

    pSetup = (SETUP_DATA*)gVariableList[VARIABLE_ID_SETUP].Buffer;

    if ( !pSetup->BootMode )			//LEGACY BOOT ITEM
    {
        if (!((popupSel->CallBackKey >= LegacyBootItemKey) &&
                (popupSel->CallBackKey < LegacyBootItemKey+LegacyBootItemCount )))
        {
            return PopupSelDraw(popupSel);
        }
    }
    else if ( pSetup->BootMode == 1 )		//UEFI BOOT ITEM
    {
        if (!((popupSel->CallBackKey >= UefiBootItemKey) &&
                (popupSel->CallBackKey < UefiBootItemKey+UefiBootItemCount )))
        {
            return PopupSelDraw(popupSel);
        }
    }
#if FBO_DUAL_MODE
    else if ( pSetup->BootMode == 2 )		//DUAL BOOT ITEM
    {
        if (!((popupSel->CallBackKey >= DualBootItemKey) &&
                (popupSel->CallBackKey < DualBootItemKey+DualBootItemCount )))
        {
            return PopupSelDraw(popupSel);
        }
    }
#endif

    if (  popupSel->ListBoxCtrl != NULL)
    {
        gListBox.Draw( popupSel->ListBoxCtrl );
    }
    else
    {
        // check conditional ptr if necessary
        if ( popupSel->ControlData.ControlConditionalPtr != 0x0)
        {
            switch ( CheckControlCondition( &popupSel->ControlData ) )
            {
            case COND_NONE:
                break;
            case COND_GRAYOUT:
                Status = EFI_WARN_WRITE_FAILURE;
                ColorNSel =  ColorLabel = CONTROL_GRAYOUT_COLOR;
                break;
            default:
                return EFI_UNSUPPORTED;
                break;
            }
        }
        // If not Special
        if (TseLiteIsSpecialOptionList((CONTROL_DATA *)popupSel) != TRUE)
        {
            Status = UefiGetOneOfOptions(&popupSel->ControlData,&popupSel->PopupSelHandle,
                                         &OptionList, &ValueList, &popupSel->ItemCount,NULL,NULL);
            if (EFI_ERROR(Status))
                return Status;
            for ( Index = 0; Index < popupSel->ItemCount; Index++ )
            {
                popupSel->PtrTokens[Index].Option = OptionList[Index];
                popupSel->PtrTokens[Index].Value = ValueList[Index];
            }
            MemFreePointer( (VOID **)&(OptionList));
            MemFreePointer( (VOID **)&(ValueList));
        }

        _PopupSelGetSelection( popupSel );
        text1 = HiiGetString( popupSel->ControlData.ControlHandle, popupSel->Title );
        if ( text1 != NULL )
        {
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
            if (popupSel->LabelMargin != SET_BOOT_OPTION_TEXT_POSITION)
            {
                popupSel->LabelMargin = SET_BOOT_OPTION_TEXT_POSITION;
                popupSel->Left = popupSel->Left + SET_BOOT_OPTION_NUMBER_POSITION;
                popupSel->Width = popupSel->Width - SET_BOOT_OPTION_NUMBER_POSITION;
            }
#endif

#if TSE_MULTILINE_CONTROLS
            if (popupSel->Height>1)
            {
                DrawMultiLineStringWithAttribute( (UINTN)popupSel->Left, (UINTN) popupSel->Top,
                                                  (UINTN)(popupSel->LabelMargin - popupSel->Left),(UINTN) popupSel->Height,
                                                  text1, popupSel->BGColor |  ColorLabel );
            }
            else
#endif
            {
                // boundary overflow  check
                if ((TestPrintLength( text1) / (NG_SIZE))> (UINTN)(popupSel->LabelMargin - popupSel->Left  ))
                    text1[HiiFindStrPrintBoundary(text1 ,(UINTN)(popupSel->LabelMargin - popupSel->Left  ))] = L'\0';

                DrawStringWithAttribute( popupSel->Left , popupSel->Top, (CHAR16*)text1,
                                         popupSel->BGColor |  ColorLabel  );

            }
            MemFreePointer( (VOID **)&text1 );
        }

        text1 = HiiGetString( popupSel->PopupSelHandle, popupSel->PtrTokens[popupSel->Sel].Option);

        if (!text1)
            text1 = EfiLibAllocateZeroPool(2);
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
        popupSel->LabelMargin = SET_BOOT_OPTION_TEXT_POSITION;
#endif
        Len = TestPrintLength( text1 ) / (NG_SIZE);
        // XXX: may be broken for wide character languages !!!
        if ( Len > (UINTN)(popupSel->Width - popupSel->LabelMargin -2) )
            EfiStrCpy(&text1[HiiFindStrPrintBoundary(text1 ,(UINTN)(popupSel->Width - popupSel->LabelMargin -5))],L"..." );


        text = EfiLibAllocateZeroPool( (Len+3)*2 );
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
        SPrint( text,(UINTN)( (Len+3)*2 ), L"%s", text1 );
#else
        SPrint( text,(UINTN)( (Len+3)*2 ), L"[%s]", text1 );
#endif
        MemFreePointer( (VOID **)&text1 );
        DrawStringWithAttribute( popupSel->Left + popupSel->LabelMargin  , popupSel->Top, (CHAR16*)text,
                                 (UINT8)( (popupSel->ControlFocus) ?
                                          popupSel->SelBGColor  | popupSel->SelFGColor :
                                          popupSel->BGColor  | ColorNSel   ));


        i=TestPrintLength(text) / (NG_SIZE);
        MemFreePointer( (VOID **)&text );

        // erase extra spaces if neccessary
        for (; (UINT16)(popupSel->Left + popupSel->LabelMargin + i) <= (popupSel->Width-1); i++)
            DrawStringWithAttribute( popupSel->Left + popupSel->LabelMargin +i, popupSel->Top, L" ",
                                     popupSel->BGColor  |  ColorNSel );

        FlushLines( popupSel->Top, popupSel->Top+popupSel->Height );
    }

    return Status;
}

EFI_STATUS OemBootOrderFrameHandleAction( FRAME_DATA *frame, ACTION_DATA *action )
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    CONTROL_DATA **control;


    if ( frame->ControlCount == 0 )
        return Status;

    if (frame->CurrentControl == FRAME_NONE_FOCUSED)
        control = NULL;
    else
        control = &frame->ControlList[frame->CurrentControl];

    if ( control != NULL )
    {
        SETUP_DATA *pSetup;
        POPUPSEL_DATA *popupsel=NULL;
        popupsel = (POPUPSEL_DATA*)(*control);

        pSetup = (SETUP_DATA*)gVariableList[VARIABLE_ID_SETUP].Buffer;

        if ( pSetup->BootMode == 0 )
        {
            if (!((popupsel->CallBackKey >= LegacyBootItemKey) &&
                    (popupsel->CallBackKey < LegacyBootItemKey+LegacyBootItemCount)))
            {
                return FrameHandleAction(frame, action);
            }
        }
        else if ( pSetup->BootMode == 1 )
        {
            if (!((popupsel->CallBackKey >= UefiBootItemKey) &&
                    (popupsel->CallBackKey < UefiBootItemKey+UefiBootItemCount)))
            {
                return FrameHandleAction(frame, action);
            }
        }
#if FBO_DUAL_MODE
        else if ( pSetup->BootMode == 2 )
        {
            if (!((popupsel->CallBackKey >= DualBootItemKey) &&
                    (popupsel->CallBackKey < DualBootItemKey+DualBootItemCount)))
            {
                return FrameHandleAction(frame, action);
            }
        }
#endif
    }

    switch ( action->Input.Type )
    {

    case ACTION_TYPE_MOUSE:
        Status = MouseFrameHandleAction(frame,action,control);
        break;

    case ACTION_TYPE_KEY:
        if ( control != NULL )
        {
            Status = (*control)->Methods->HandleAction( *control, action );
            CheckFixedBootOrder( *control, frame, action->Input.Data.AmiKey );  //(EIP10461+)(EIP19998)+)
        }

        if (EFI_ERROR(Status))
            Status = StyleFrameHandleKey( frame, action->Input.Data.AmiKey.Key );

        if (EFI_ERROR(Status))
            Status = _FrameHandleKey( frame, action->Input.Data.AmiKey );
        break;

    case ACTION_TYPE_TIMER:
    {
        UINT32 i;
        control = frame->ControlList;
        for ( i = 0; i < frame->ControlCount; i++, control++ )
        {
            EFI_STATUS TempStatus = (*control)->Methods->HandleAction( *control, action );
            if ( Status != EFI_SUCCESS )
                Status = TempStatus;
        }
        // To kill the timer action. Pass it to all the controls and frame.
        //	gAction.ClearAction( action );
    }

    if (EFI_ERROR(Status))
        Status = StyleFrameHandleTimer( frame, action);
    break;
    default:
        break;
    }

    return Status;
}

EFI_STATUS OemPopupSelHandleAction( POPUPSEL_DATA *popupSel, ACTION_DATA *Data)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    AMI_EFI_KEY_DATA key=Data->Input.Data.AmiKey;

    // Disable enter key in FixedBootOrder item
    if ((popupSel->CallBackKey >= LegacyBootItemKey) &&
            (popupSel->CallBackKey < LegacyBootItemKey+LegacyBootItemCount))
    {
        if (Data->Input.Type == ACTION_TYPE_KEY)
            if (MapControlKeysHook(key) == ControlActionSelect)
                return Status;
    }
    else if ((popupSel->CallBackKey >= UefiBootItemKey) &&
             (popupSel->CallBackKey < UefiBootItemKey+UefiBootItemCount))
    {
        if (Data->Input.Type == ACTION_TYPE_KEY)
            if (MapControlKeysHook(key) == ControlActionSelect)
                return Status;
    }
#if FBO_DUAL_MODE
    else if ((popupSel->CallBackKey >= DualBootItemKey) &&
             (popupSel->CallBackKey < DualBootItemKey+DualBootItemCount))
    {
        if (Data->Input.Type == ACTION_TYPE_KEY)
            if (MapControlKeysHook(key) == ControlActionSelect)
                return Status;
    }
#endif

    Status = PopupSelHandleAction(popupSel, Data);

    return Status;
}

UINT16 GetDevMapDataCount(FBODevMap *pFBODevMap);	//at file FixedBootOrderTSE.c

VOID 
FixedBootOrderStyleProcessEnterSetup
(
  VOID
)
{
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    EFI_STATUS Status;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *pFBO=NULL;

    if (gTseSkipFBOModule) return ;
    gFrame.HandleAction = OemBootOrderFrameHandleAction;
    gPopupSel.Draw = OemBootOrderPopupSelDraw;
    gPopupSel.HandleAction = OemPopupSelHandleAction;

    Status = pBS->LocateProtocol( &FixedBootOrderGuid, NULL, &pFBO );
    if (EFI_ERROR(Status)) return;

    LegacyBootItemCount = GetDevMapDataCount(pFBO->GetLegacyDevMap());
    UefiBootItemCount = GetDevMapDataCount(pFBO->GetUefiDevMap());
#if FBO_DUAL_MODE
    DualBootItemCount = GetDevMapDataCount(pFBO->GetDualDevMap());
#endif
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
