//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/PopupPassword.c $
//
// $Author: Rajashakerg $
//
// $Revision: 27 $
//
// $Date: 9/17/12 6:20a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file PopupPassword.c
    This file contains code to handle Popup Passwords

**/

#include "minisetup.h"
////////////////////////////////////////////////////////////////
////	Extern Variables
////////////////////////////////////////////////////////////////
extern UINTN gInvalidPasswordFailMsgBox;
extern UINTN gClearPasswordMsgBox;
extern UINTN gInvalidRangeFailMsgBox;
extern UINTN gClearLabelPasswordMsgBox;
extern UINTN gCannotChangePasswordMsgBox;
extern UINTN gCursorLeft,gCursorTop; 
extern BOOLEAN SoftKbdRefresh;
extern BOOLEAN CompleteReDrawFlag;

CALLBACK_VARIABLE  gPopupEditCb = { { CALLBACK_TYPE_VARIABLE, sizeof(CALLBACK_VARIABLE) }, 0,0,0,NULL };
UINT8 res = (UINT8)-1;

POPUP_PASSWORD_METHODS gPopupPassword =
{
		(OBJECT_METHOD_CREATE)PopupPasswordCreate,
		(OBJECT_METHOD_DESTROY)PopupPasswordDestroy,
		(OBJECT_METHOD_INITIALIZE)PopupPasswordInitialize,
		(OBJECT_METHOD_DRAW)PopupPasswordDraw,
		(OBJECT_METHOD_HANDLE_ACTION)PopupPasswordHandleAction,
		(OBJECT_METHOD_SET_CALLBACK)PopupPasswordSetCallback,
		(CONTROL_METHOD_SET_FOCUS)PopupPasswordSetFocus,
		(CONTROL_METHOD_SET_POSITION)PopupPasswordSetPosition,
		(CONTROL_METHOD_SET_DIMENSIONS)PopupPasswordSetDimensions,
		(CONTROL_METHOD_SET_ATTRIBUTES)PopupPasswordSetAttributes,
		(CONTROL_METHOD_GET_CONTROL_HIGHT)PopupPasswordGetControlHeight

};
VOID 	GetCoordinates(INT32 *x, INT32 *y, INT32 *z);
VOID 	SetPwdKeyboardLayout(VOID);
VOID 	ResetPwdKeyboardLayout(VOID);
BOOLEAN IsTSECursorSupport(); 
VOID 	SavePswdString (CONTROL_INFO *ControlData, CHAR16 *String);
BOOLEAN	IsPasswordSupportNonCaseSensitive();
BOOLEAN	IsShowPromptStringAsTitle(VOID);
BOOLEAN IsMouseOnSoftkbd(VOID);
BOOLEAN IsMouseSupported(VOID);
INT32 GetactualScreentop(VOID);
INT32 GetactualScreenLeft(VOID);
VOID SetSoftKbdPosition(UINT32 Height, UINT32 Top);
BOOLEAN IsCharacteronSoftKbd(UINT32 Left,UINT32 TOP);
UINT32 GetUefiSpecVersion (VOID);
BOOLEAN IsMouseActionForSoftKbd =0;
/**
    this function uses the create function of control
    and creates the popup password

    @param object 

    @retval status

**/
EFI_STATUS PopupPasswordCreate( POPUP_PASSWORD_DATA **object )
{
	EFI_STATUS Status = EFI_OUT_OF_RESOURCES;

	if ( *object == NULL )
	{
		*object = EfiLibAllocateZeroPool( sizeof(POPUP_PASSWORD_DATA) );

		if ( *object == NULL )
			return Status;
	}

	Status = gControl.Create( (void**)object );
	if ( ! EFI_ERROR(Status) )
		(*object)->Methods = &gPopupPassword;

	return Status;
}

/**
    this function uses the destroy function of control
    and destroys the popup password

    @param popuppassword BOOLEAN freeMem

    @retval status

**/
EFI_STATUS PopupPasswordDestroy( POPUP_PASSWORD_DATA *popuppassword, BOOLEAN freeMem )
{
	if(NULL == popuppassword)
	  return EFI_SUCCESS;

	gControl.Destroy( popuppassword, FALSE );

	MemFreePointer( (VOID **)&popuppassword->Text );

	if( freeMem )
		MemFreePointer( (VOID **)&popuppassword );

	return EFI_SUCCESS;
}

/**
    this function uses the initialize function of control
    and initializes the popup password

    @param popuppassword VOID *data

    @retval status

**/
EFI_STATUS PopupPasswordInitialize( POPUP_PASSWORD_DATA *popuppassword, VOID *data )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	Status = gControl.Initialize( popuppassword, data );
	if (EFI_ERROR(Status))
		return Status;

	// add extra initialization here...
	SetControlColorsHook(NULL, NULL,  
				NULL, NULL,
				&(popuppassword->SelBGColor), &(popuppassword->SelFGColor),
			        &(popuppassword->BGColor), &(popuppassword->FGColor),
				NULL, NULL,
				NULL,
				NULL ,NULL,
				NULL,NULL );

	popuppassword->ControlData.ControlHelp = UefiGetHelpField(popuppassword->ControlData.ControlPtr);

	return EFI_SUCCESS;
}

/**
    function to draw the popup password with attributes

    @param popuppassword 

    @retval status

**/
EFI_STATUS PopupPasswordDraw(POPUP_PASSWORD_DATA *popuppassword )
{
	CHAR16 *text;
	EFI_STATUS Status = EFI_SUCCESS;

	UINT8 ColorLabel = popuppassword->FGColor;
	// check conditional ptr if necessary
	//if( popuppassword->ControlData.ControlConditionalPtr != 0x0)
	//{
		switch( CheckControlCondition( &popuppassword->ControlData ) )
		{
			case COND_NONE:
				break;
			case COND_GRAYOUT:
				Status = EFI_WARN_WRITE_FAILURE;
				ColorLabel = CONTROL_GRAYOUT_COLOR;
				break;
			default:
				return EFI_UNSUPPORTED;
				break;
		}
	//}

	text = HiiGetString( popuppassword->ControlData.ControlHandle, UefiGetPromptField(popuppassword->ControlData.ControlPtr));
	if ( text == NULL )
		return EFI_OUT_OF_RESOURCES;

	if(popuppassword->Height>1 && IsTSEMultilineControlSupported())
	{
		DrawMultiLineStringWithAttribute( popuppassword->Left , popuppassword->Top, 
				(UINTN)(popuppassword->Width),(UINTN) popuppassword->Height,
				text,(UINT8)( (popuppassword->ControlFocus) ? 
						 popuppassword->SelBGColor  | popuppassword->SelFGColor : 
						 popuppassword->BGColor | ColorLabel));
	}
	else
	{
		// use frame width minus margins as available space
		// boundary overflow  check
		if ( (TestPrintLength( text ) / (NG_SIZE)) > (UINTN)(popuppassword->Width  ))
				text[HiiFindStrPrintBoundary(text,(UINTN)(popuppassword->Width  ))] = L'\0';
	
		DrawStringWithAttribute( popuppassword->Left  , popuppassword->Top, text, 
				(UINT8)( (popuppassword->ControlFocus) ? 
						 popuppassword->SelBGColor  | popuppassword->SelFGColor : 
						 popuppassword->BGColor | ColorLabel   ));
	}
	
	MemFreePointer( (VOID **)&text );
	FlushLines( popuppassword->Top  , popuppassword->Top );

	return Status;
}

EFI_STATUS PopupPasswordSetCallback( POPUP_PASSWORD_DATA *popuppassword, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie )
{
	return gControl.SetCallback( popuppassword, container, callback, cookie );
}

/**
    function to check the local installation of popup password

    @param popuppassword 

    @retval True/False

**/
UINT32 PopupPasswordCheckInstalledLocal(POPUP_PASSWORD_DATA *popuppassword)
{
    VOID *data = popuppassword->ControlData.ControlPtr;
    CHAR16 *RealPassword=NULL;
	UINT32 Installed=0;
	// read real password from nvram
	RealPassword = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
	if ( RealPassword == NULL )
        return Installed;
	MemSet( RealPassword, ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16), 0 );

	VarGetValue( popuppassword->ControlData.ControlVariable, 
		UefiGetQuestionOffset(data), 
		(UINT8)UefiGetMaxValue(data)*2, RealPassword );

	if ( RealPassword[0] != L'\0' )
		Installed = 1;
	MemFreePointer( (VOID **)&RealPassword );

	return Installed;
}

/**
    function to check authentication of the password

    @param popuppassword CHAR16 *Password

    @retval TRUE/FALSE

**/
BOOLEAN PopupPasswordAuthenticateLocal( POPUP_PASSWORD_DATA *popuppassword, CHAR16 *Password )
{
    VOID *data = popuppassword->ControlData.ControlPtr;
    CHAR16 *RealPassword=NULL,*TempEncoded=NULL;
	BOOLEAN AuthenticateStatus=FALSE;
	UINTN	ii;

	// read real password from nvram
	RealPassword = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
	if ( RealPassword == NULL )
        return AuthenticateStatus;
	MemSet( RealPassword, ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16), 0 );

	VarGetValue( popuppassword->ControlData.ControlVariable, 
		UefiGetQuestionOffset(data), 
		(UINT8)UefiGetMaxValue(data)*2, RealPassword );

	TempEncoded = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
	if ( TempEncoded == NULL )
        return AuthenticateStatus;

	MemSet( TempEncoded, ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16), 0 );
	EfiStrCpy(TempEncoded,Password);

	if (
      (IsPasswordSupportNonCaseSensitive ()) && 
      ( (VARIABLE_ID_AMITSESETUP == popuppassword->ControlData.ControlVariable) || (VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable) )
      )
	{
		for (ii = 0; ii < (UefiGetMaxValue(data)); ii++)
			TempEncoded [ii] = ((TempEncoded [ii]>=L'a')&&(TempEncoded [ii]<=L'z'))?(TempEncoded [ii]+L'A'-L'a'):TempEncoded [ii];
	}
	//PasswordEncodeHook( TempEncoded, (UINT8)UefiGetMaxValue(data) * sizeof(CHAR16));
	if( IsPasswordEncodeEnabled( &popuppassword->ControlData )){
		PasswordEncodeHook( TempEncoded, (UINT8)UefiGetMaxValue(data) * sizeof(CHAR16));
	}
    if( EfiCompareMem(TempEncoded,RealPassword,(UINT8)UefiGetMaxValue(data) * sizeof(CHAR16)))
		AuthenticateStatus = FALSE;
	else
		AuthenticateStatus = TRUE;

	MemFreePointer( (VOID **)&RealPassword );
	MemFreePointer( (VOID **)&TempEncoded );

	return AuthenticateStatus;
}

/**
    function to update the given password to non case sensitive

    @param Password UINTN PwdLength

    @retval VOID

**/
VOID UpdatePasswordToNonCaseSensitive(CHAR16 *Password, UINTN PwdLength)
{
	UINTN Idx;
	for ( Idx = 0; Idx < PwdLength; Idx++ )
		Password[Idx] = ((Password[Idx]>=L'a')&&(Password[Idx]<=L'z'))?(Password[Idx]+L'A'-L'a'):Password[Idx];
}

/**
    function to update the title and text using the input

    @param Title CONTROL_INFO *ControlData, UINT16 *newtoken, UINT16* orgtext

    @retval EFI_STATUS

**/
EFI_STATUS SetTitleUsingControlPtr(UINTN Title, CONTROL_INFO *ControlData, UINT16 *newtoken, UINT16* orgtext )
{

	if ( IsShowPromptStringAsTitle() )
	{
		CHAR16 *temptext = NULL, *tText = NULL;
		CHAR16 *tempTitle = NULL;
		UINTN strlen = 0;
		
		
		//Taking backup of original StringID
		*orgtext = ((AMI_IFR_MSGBOX*)(Title))->Text;
	
		//Get TSE string
		temptext = HiiGetString( (VOID*)(UINTN)INVALID_HANDLE, *orgtext);
		strlen = EfiStrLen(temptext);
	
		tempTitle = EfiLibAllocateZeroPool((strlen+2)*sizeof(CHAR16));
		
		if (NULL == tempTitle)
		{
			if(temptext)
				MemFreePointer( (VOID **)&temptext );
			return EFI_OUT_OF_RESOURCES;
		}
	
		EfiStrCpy (tempTitle,temptext);
		strlen = EfiStrLen(tempTitle);
		
		MemFreePointer( (VOID **)&temptext );
	
		//Get prompt string from controlptr
		temptext = HiiGetString( ControlData->ControlHandle, UefiGetPromptField(ControlData->ControlPtr));
		strlen = EfiStrLen(temptext+2);
	
		tempTitle = MemReallocateZeroPool (
									tempTitle,
									( (EfiStrLen (tempTitle) + 2) * sizeof (CHAR16) ),
									( ((EfiStrLen (tempTitle) + 2) * sizeof (CHAR16)) + ((EfiStrLen (temptext) + 2) * sizeof (CHAR16)) )    //2 for /n and NULL character
									);
	
		if (NULL == tempTitle)
		{
			if(temptext)
				MemFreePointer( (VOID **)&temptext );
			return EFI_OUT_OF_RESOURCES;
		}
	
		//Eliminate white space character if any precedes
		tText = temptext;
		while(*temptext == L' ')
		{
			temptext++;
		}
	
		//Appending tse string with controlptr prompt string
		EfiStrCat (tempTitle,temptext);
		EfiStrCat (tempTitle,L"?");
	
		//Adding string to hiistring
		*newtoken = HiiAddString( gHiiHandle, tempTitle );
	
		//Setting new stringID as current stringID
		((AMI_IFR_MSGBOX*)(Title))->Text = *newtoken;
	
		MemFreePointer( (VOID **)&tText );
		MemFreePointer( (VOID **)&tempTitle );
	}
	return EFI_SUCCESS;
}

/**
    function to call the callback on ESC on interactive password popup

    @param popuppassword CHAR16 *Text

    @retval EFI_STATUS

**/
EFI_STATUS HandleESCOnInteractivePassword(POPUP_PASSWORD_DATA *popuppassword,CHAR16 *Text )
{
	UINT16 Key = UefiGetControlKey(&(popuppassword->ControlData));
	VOID *data = popuppassword->ControlData.ControlPtr;
	UINTN OldPwLen = 0;
	 CHAR16 *TempEncoded=NULL;
	 EFI_STATUS Status = EFI_SUCCESS;
	if((Text != NULL) ){
		OldPwLen = EfiStrLen(Text);
		if(OldPwLen){
			 TempEncoded = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
			if ( TempEncoded == NULL )
				return EFI_NOT_READY;
			EfiStrCpy(TempEncoded,Text);
			
			if( IsPasswordEncodeEnabled( &popuppassword->ControlData )){
					PasswordEncodeHook( TempEncoded, (UINT8)UefiGetMaxValue(data) * sizeof(CHAR16));
				}
			SavePswdString(&popuppassword->ControlData, TempEncoded);
			StringZeroFreeMemory( (VOID **)&TempEncoded );
		}
		Status =  CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE);
      }
       else
       {
		SavePswdString(&popuppassword->ControlData, NULL);// kept text as NULL
		Status =  CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE);		
       }
	return Status;
}
/**
    function to set and activate a password

    @param popuppassword 

    @retval status

**/
EFI_STATUS _PopupPasswordActivate(POPUP_PASSWORD_DATA *popuppassword)
{
   	EFI_STATUS Status = EFI_UNSUPPORTED;
	BOOLEAN AbortUpdate = FALSE, PasswordInstalled = FALSE;
	CHAR16 *Text=NULL,*NewPswd=NULL,*ConfirmNewPswd=NULL;
	VOID *data = popuppassword->ControlData.ControlPtr;
	UINTN NewPwLen = 0, OldPwLen = 0;
	UINT16 orgtext = 0;
	UINT16 newtoken = 0;
	 CHAR16 *TempEncoded=NULL;
	UINT16 Key = UefiGetControlKey(&(popuppassword->ControlData));
	
	if(UefiIsInteractive(&popuppassword->ControlData))
	{
		
		SavePswdString(&popuppassword->ControlData, NULL);// kept text as NULL, as we are checking for preexisitng password present or not
		Status = CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE); // need to pass NULL as we are checking for pre existing password
		if(Status == EFI_SUCCESS)
		{
			// no pre existing password is present
			//	goto NewPassword;
			PasswordInstalled = FALSE;
			goto Password;
		}
		//As per Spec 2.5: Driver returns EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to terminate password processing and other error for pre existing password.
		else if( (GetUefiSpecVersion() >= 0x20032) &&( (EFI_UNSUPPORTED == Status) || (EFI_NOT_AVAILABLE_YET == Status) ))
		{
			goto Done;
		}			
		else
		{
			Status = _DoPopupEdit( popuppassword, (IsShowPromptStringAsTitle() ? STRING_TOKEN(STR_OLD_PSWD_LABEL): STRING_TOKEN(STR_OLD_PSWD)), &Text);// prompt for old password
			//now we got old password
			if(Text)
			{
				OldPwLen = EfiStrLen(Text);
			}
			if((Status == EFI_SUCCESS) && (*Text != L'\0')) 
			{
				SavePswdString(&popuppassword->ControlData, Text);
				
				TempEncoded = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
				if ( TempEncoded == NULL )
					return EFI_NOT_READY;
				EfiStrCpy(TempEncoded,Text);
				
				if( IsPasswordEncodeEnabled( &popuppassword->ControlData )){
						PasswordEncodeHook( TempEncoded, (UINT8)UefiGetMaxValue(data) * sizeof(CHAR16));
					}
				SavePswdString(&popuppassword->ControlData, TempEncoded);
				StringZeroFreeMemory( (VOID **)&TempEncoded );
				
				Status = CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE); // need to pass the old password to get it authenticated
				if(Status == EFI_NOT_READY)
				{
					// wrong old password
					// show error alert message box and exit
					// return;
					CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK );
					goto Done;
					
				}
				else if(Status == EFI_SUCCESS) //correct old password. Now we will prompt new password popup
				{
					PasswordInstalled = TRUE;
					goto Password;
				}
				else //Error from driver side that password cant be changed
				{
					//CallbackShowMessageBox( (UINTN)gCannotChangePasswordMsgBox, MSGBOX_TYPE_OK );
					goto Done;
				}
				
			}
			else if((*Text == L'\0') && (Status == EFI_SUCCESS)) //for empty old password
			{
				CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK );
				goto Done;
				// handle for Enpty password in old password prompt
			}
			else //for ESC
			{
				goto Done;
				// handle for ESC in old password prompt
			}
		}
	}
	else if( PopupPwdAuthenticateIDEPwd(popuppassword,&AbortUpdate,data) != EFI_SUCCESS) //for normal password if its not interactive
	{
		if(PopupPasswordCheckInstalled(popuppassword)) //check for password installed
		{
			// ask for old password (popupedit)
			Status = _DoPopupEdit( popuppassword, (IsShowPromptStringAsTitle() ? STRING_TOKEN(STR_OLD_PSWD_LABEL): STRING_TOKEN(STR_OLD_PSWD)), &Text);
			if(Status)
				goto Done; //if status not success then goto done
			if(Text)
				OldPwLen = EfiStrLen(Text); //getting old password length
			if(!Status)
			{
				if(!PopupPasswordAuthenticate( popuppassword, Text ))
				{   
					// optional message to user: "wrong password" and  exit                   
					CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK ); //if invalid password then goto done
					goto Done;
				}
				else
				{
					PasswordInstalled = TRUE;
					goto Password; //If old password is authenticated then prompt for new passowrd
				}
			}
		}
	}
	if(AbortUpdate == FALSE)
	{
Password:
		//ask for new password(1)
		Status = _DoPopupEdit( popuppassword, (IsShowPromptStringAsTitle() ? STRING_TOKEN(STR_NEW_PSWD_LABEL): STRING_TOKEN(STR_NEW_PSWD)), &NewPswd);
	
		if(!Status) //If status is success
		{
			if(NewPswd)
			{
				NewPwLen = EfiStrLen(NewPswd); //getting new password length
			}
			if (
				    (IsPasswordSupportNonCaseSensitive ()) && 
				    (NewPwLen != 0) &&
				    ( (VARIABLE_ID_AMITSESETUP == popuppassword->ControlData.ControlVariable) || (VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable) )
			    )
			 {
				UpdatePasswordToNonCaseSensitive (NewPswd, NewPwLen);
			 }
			if ( (VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable) || (PopupPasswordCheckInstalled (popuppassword)) )
			{
				if ((PasswordInstalled || (CheckForIDEPasswordInstalled(popuppassword) == EFI_SUCCESS))  && NewPwLen == 0) //if only new passwd length is 0, we will ask to clear the old password
				{
					//call function
					SetTitleUsingControlPtr(gClearLabelPasswordMsgBox, &popuppassword->ControlData, &newtoken, &orgtext);
	
					//Report Message box for Clearing Old password
					if(CallbackShowMessageBox( (IsShowPromptStringAsTitle() ? (UINTN)gClearLabelPasswordMsgBox : (UINTN)gClearPasswordMsgBox), MSGBOX_TYPE_YESNO )!= MSGBOX_YES)
					{
						if ( IsShowPromptStringAsTitle() )
						{
							//Resetting original token
							((AMI_IFR_MSGBOX*)(gClearLabelPasswordMsgBox))->Text = orgtext;
							//Delete created token
							HiiRemoveString(gHiiHandle, newtoken);
						}
	
						goto Password; //again prompt for new password
						//Status = EFI_UNSUPPORTED; //Do not clear the password
					} 
					else 
					{
						if ( IsShowPromptStringAsTitle() )
						{
							//Resetting original token
							((AMI_IFR_MSGBOX*)(gClearLabelPasswordMsgBox))->Text = orgtext;
							//Delete created token
							HiiRemoveString(gHiiHandle, newtoken);
						}
	
						ConfirmNewPswd = EfiLibAllocateZeroPool( sizeof(CHAR16) );
						
						if(ConfirmNewPswd) 
						{
							*ConfirmNewPswd = L'\0'; //Set confirmation password to null string
							goto SavePassword; //now we will save null as password for clearing already installed password
						} 
						else 
						{
							Status = EFI_OUT_OF_RESOURCES;
							goto Done; //not able to allocate memory so we will exit.
						}
					}
				}
			}
			if(
				( (NewPwLen < (UINT8)UefiGetMinValue (popuppassword->ControlData.ControlPtr)) ||
				  (NewPwLen > (UINT8)UefiGetMaxValue (popuppassword->ControlData.ControlPtr)) )
			)
			{
				//Report Message box
				CallbackShowMessageBox( (UINTN)gInvalidRangeFailMsgBox, MSGBOX_TYPE_OK );
				Status = EFI_UNSUPPORTED;
				goto Done;
			}
		}
		else //handling for ESC over new password window.
		{
			if(UefiIsInteractive(&popuppassword->ControlData)){//if only interactive we will send old password to save again
				
				HandleESCOnInteractivePassword(popuppassword, Text);
			}
			goto Done;
			
		}
		// ask for new password(2)again
		if ( ((!Status) && (NewPwLen != 0)) || (((UefiIsInteractive(&popuppassword->ControlData)) || (VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable)) && (!Status)) )
		{
			Status = _DoPopupEdit(  popuppassword, (IsShowPromptStringAsTitle() ? STRING_TOKEN(STR_CONFIRM_NEW_PSWD_LABEL) : STRING_TOKEN(STR_CONFIRM_NEW_PSWD)), &ConfirmNewPswd);			
		}
	
SavePassword:
		if ( !Status )
		{
			if (
					(IsPasswordSupportNonCaseSensitive ()) &&
					( NULL!=ConfirmNewPswd ) && (EfiStrLen (ConfirmNewPswd)) && //Avoid crashing when give empty password
					( (VARIABLE_ID_AMITSESETUP == popuppassword->ControlData.ControlVariable) || (VARIABLE_ID_IDE_SECURITY == popuppassword->ControlData.ControlVariable) )
			)
			{
				NewPwLen = EfiStrLen (ConfirmNewPswd);
				UpdatePasswordToNonCaseSensitive (ConfirmNewPswd, NewPwLen);
			}
			if ( ( NULL!=ConfirmNewPswd ) && EfiStrCmp(NewPswd, ConfirmNewPswd)==0 ) // Avoid crashing when give empty password
			{
					//Support for Interactive Password control
					if(popuppassword->Text != NULL){
						MemFreePointer( (VOID **)&popuppassword->Text );
					}
					popuppassword->Text = (CHAR16 *) EfiLibAllocateZeroPool((NewPwLen + 1) * sizeof(CHAR16));
					EfiStrCpy(popuppassword->Text, NewPswd);
					SavePswdString(&(popuppassword->ControlData),NewPswd);
					//save new password
					if ( ( popuppassword->Callback != NULL ) && ( popuppassword->Cookie != NULL ) )
					{
						VOID *ifrData = popuppassword->ControlData.ControlPtr;
						CALLBACK_PASSWORD *callbackData = (CALLBACK_PASSWORD *)popuppassword->Cookie;
	
						callbackData->Variable = popuppassword->ControlData.ControlVariable;
						callbackData->Offset = UefiGetQuestionOffset(ifrData);
						callbackData->Length = (UINT8)UefiGetMaxValue(ifrData) * sizeof(CHAR16);
						callbackData->Data = (VOID *)NewPswd;
						
						UefiPreControlUpdate(&(popuppassword->ControlData));
						popuppassword->Callback( popuppassword->Container,popuppassword, popuppassword->Cookie );
	
						PopupPwdUpdateIDEPwd ();
					}
	
	
					//For interactive control invoke the formcallback function.
					if(UefiIsInteractive(&popuppassword->ControlData))
					{
							VOID *Handle = popuppassword->ControlData.ControlHandle;
							UINT16 Key = UefiGetControlKey(&(popuppassword->ControlData));
							CONTROL_DATA *Control = (CONTROL_DATA *)NULL;
							if(NewPwLen){
								TempEncoded = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
								if ( TempEncoded == NULL )
									return EFI_NOT_READY;
								EfiStrCpy(TempEncoded,NewPswd);
								
								if( IsPasswordEncodeEnabled( &popuppassword->ControlData )){
										PasswordEncodeHook( TempEncoded, (UINT8)UefiGetMaxValue(data) * sizeof(CHAR16));
								}
								SavePswdString(&(popuppassword->ControlData),TempEncoded);
								StringZeroFreeMemory( (VOID **)&TempEncoded );
							}
							//Call the callback passing the input value and value size.
							Status = CallFormCallBack( &(popuppassword->ControlData),Key,0,AMI_CALLBACK_CONTROL_UPDATE);
	
							Control = GetUpdatedControlData((CONTROL_DATA*)popuppassword, CONTROL_TYPE_PASSWORD, Handle, Key);
							if(Control == NULL){ //Control deleted.
							return EFI_SUCCESS;
						}
						if(Control != (CONTROL_DATA *)popuppassword){
						popuppassword = (POPUP_PASSWORD_DATA *)Control; //Control Updated.
						}
					}
	
					popuppassword->ControlActive = FALSE;
				}
				else//if both new and confirm password are not same
				{
	
					if(UefiIsInteractive(&popuppassword->ControlData)){ //if we have old password we will save old password again
						HandleESCOnInteractivePassword(popuppassword, Text);
					}
					CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK );
					goto Done;
				}
		}
		else //handling ESC over Confirm password
		{
	
			if(UefiIsInteractive(&popuppassword->ControlData)){
				HandleESCOnInteractivePassword(popuppassword, Text);
			}
			goto Done;
		   }
	}	
	
Done:
	
	StringZeroFreeMemory ((VOID **)&Text);		
	StringZeroFreeMemory ((VOID **)&NewPswd);
	StringZeroFreeMemory ((VOID **)&ConfirmNewPswd);

    return EFI_SUCCESS;
}

/**
    function to handle the password actions

    @param popuppassword , ACTION_DATA *Data

    @retval status

**/
EFI_STATUS _PopupPasswordHandleAction( POPUP_PASSWORD_DATA *popuppassword, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
    UINT8 u8ChkResult;

	if ( Data->Input.Type == ACTION_TYPE_TIMER )
	   return Status;
	
    u8ChkResult = CheckControlCondition( &popuppassword->ControlData );
	 
	 //Not to perform action for control when token TSE_SETUP_GRAYOUT_SELECTABLE is enable to set focus for GrayoutIf control
    if ( IsGrayoutSelectable() && (COND_GRAYOUT == u8ChkResult) ) {
        return EFI_UNSUPPORTED;
    }

	if ( Data->Input.Type == ACTION_TYPE_KEY )
    {
        CONTROL_ACTION Action;

        //Get mapping
        Action = MapControlKeysHook(Data->Input.Data.AmiKey);

        if(ControlActionSelect == Action)
        {
			if(UefiIsInteractive(&popuppassword->ControlData))
				if(PopupPasswordFormCallback(&(popuppassword->ControlData),UefiGetControlKey(&(popuppassword->ControlData)),0) == EFI_SUCCESS)
					return EFI_SUCCESS;

            return _PopupPasswordActivate(popuppassword);
        }
    }

    if(Data->Input.Type == ACTION_TYPE_MOUSE)
	{
		if(ControlActionSelect == MapControlMouseActionHook(&Data->Input.Data.MouseInfo))
		{
			
			//check whether MouseTop is within the Height and Width of Password Control or not
			if((Data->Input.Data.MouseInfo.Top >= (UINT32)popuppassword->Top) && (Data->Input.Data.MouseInfo.Top < (UINT32)(popuppassword->Top+popuppassword->Height)) &&
			  (Data->Input.Data.MouseInfo.Left >= (UINT32)popuppassword->Left) && (Data->Input.Data.MouseInfo.Left < (UINT32)(popuppassword->Left+popuppassword->Width)))
			{
				SetPwdKeyboardLayout();
			   IsMouseActionForSoftKbd =1;
				if(UefiIsInteractive(&popuppassword->ControlData))
				{
					if(PopupPasswordFormCallback(&(popuppassword->ControlData),UefiGetControlKey(&(popuppassword->ControlData)),0) == EFI_SUCCESS)
					{
						IsMouseActionForSoftKbd =0;
						return EFI_SUCCESS;
					}
				}
				Status =_PopupPasswordActivate(popuppassword);
				ResetPwdKeyboardLayout();
				IsMouseActionForSoftKbd =0;
				return Status;
	        }
		}
	}
	return Status;
}
/**
    Function to handle the PopupPassword Actions

    @param popuppassword ACTION_DATA *Data

    @retval Status

**/
EFI_STATUS PopupPasswordHandleAction( POPUP_PASSWORD_DATA *popuppassword, ACTION_DATA *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	
	Status = PopupPwdHandleActionOverRide(popuppassword, Data);

	return Status;
}
/**
    Function to set focus

    @param popuppassword BOOLEAN focus

    @retval Status

**/
EFI_STATUS PopupPasswordSetFocus(POPUP_PASSWORD_DATA *popuppassword, BOOLEAN focus)
{
    UINT8 u8ChkResult;

	if(focus != FALSE)
	{
        u8ChkResult = CheckControlCondition( &popuppassword->ControlData );
		if ( ((u8ChkResult != COND_NONE) && (u8ChkResult != COND_GRAYOUT)) ||
             (!IsGrayoutSelectable() && (u8ChkResult == COND_GRAYOUT)) )
			return EFI_UNSUPPORTED;
	}

	if( !(popuppassword->ControlFocus && focus) )
		popuppassword->ControlFocus = focus;
	return EFI_SUCCESS;
}
/**
    Function to set position.

    @param popuppassword UINT16 Left, UINT16 Top

    @retval STATUS

**/
EFI_STATUS PopupPasswordSetPosition(POPUP_PASSWORD_DATA *popuppassword, UINT16 Left, UINT16 Top )
{
	return gControl.SetPosition( (CONTROL_DATA*)popuppassword, Left, Top );
}
/**
    Function to set dimension.

    @param popuppassword UINT16 Width, UINT16 Height

    @retval STATUS

**/
EFI_STATUS PopupPasswordSetDimensions(POPUP_PASSWORD_DATA *popuppassword, UINT16 Width, UINT16 Height )
{
	return gControl.SetDimensions( (CONTROL_DATA*) popuppassword, Width, Height );
}
/**
    Function to set attributes.

    @param popuppassword UINT8 FGColor, UINT8 BGColor

    @retval STATUS

**/
EFI_STATUS PopupPasswordSetAttributes(POPUP_PASSWORD_DATA *popuppassword, UINT8 FGColor, UINT8 BGColor )
{
	return gControl.SetAttributes(  (CONTROL_DATA*)popuppassword, FGColor, BGColor );
}

/**
    function to get the height of the label

    @param popuppassword frame, UINT16 *height

    @retval status

**/
EFI_STATUS PopupPasswordGetControlHeight(POPUP_PASSWORD_DATA *popuppassword, VOID *frame, UINT16 *height)
{
	if(IsTSEMultilineControlSupported())
	{
		CHAR16 *newText = NULL,*text=NULL;
		UINT16 Width;
	
		Width = ((FRAME_DATA*)frame)->FrameData.Width - 2;

		text = HiiGetString( popuppassword->ControlData.ControlHandle, UefiGetPromptField(popuppassword->ControlData.ControlPtr));
		if ( text == NULL )
			return EFI_OUT_OF_RESOURCES;
	
		newText = StringWrapText( text, Width, height );
	
	    (*height) = (*height) ? (*height):1;
	
	    MemFreePointer( (VOID **)&newText );
		MemFreePointer( (VOID **)&text );
	}
	else
	{
		*height = 1;
	}
	return EFI_SUCCESS;
}





/**
    Function for PopupEdit callback.

    @param container VOID *object, VOID *cookie

    @retval STATUS

**/
EFI_STATUS _CBPopupEdit(VOID *container, VOID *object, VOID *cookie)
{
	EFI_STATUS Status = EFI_SUCCESS;
	if(cookie!=NULL)
	{

		res=0;
	}
	else
 	   res=1; // exit with no changes to string

	return Status;
}

/**
    Function to get edit value.

    @param popupedit 

    @retval UINTN

**/
UINTN _PopupEditGetValue( POPUPEDIT_DATA *popupedit )
{
	ACTION_DATA *action = gApp->Action;
	EFI_STATUS 	Status = EFI_SUCCESS;
	BOOLEAN 	DrawCursor = TRUE;
	INT32  	MousePointerX = 0,MousePointerY = 0,MousePointerZ = 0; // Modified Declaration for GCC Build 
	INT32		OldMousePointerX=1,OldMousePointerY=1,Screen_Left,Screen_Top;
	UINTN glyphWidth = 0, glyphHeight = 0;
	
	gPopupEdit.SetCallback( popupedit, NULL,(OBJECT_CALLBACK) _CBPopupEdit, &gPopupEditCb );
	gPopupEdit.SetDimensions( (CONTROL_DATA*)popupedit, popupedit->Width , popupedit->Height );
	res = (UINT8)-1;

	if(IsMouseActionForSoftKbd)
	{
		SetSoftKbdPosition(popupedit->Height+1, (UINT32)((gMaxRows - popupedit->Height)/2));
		TSEStringReadLoopEntryHook();
	}
	Screen_Top=GetactualScreentop();
	Screen_Left=GetactualScreenLeft();
	
	glyphWidth = HiiGetGlyphWidth();
	glyphHeight = HiiGetGlyphHeight();

	while ( res == (UINT8)-1 )
	{
		if ( action->Input.Type != ACTION_TYPE_NULL )
			gAction.ClearAction( action );

		GetCoordinates (&MousePointerX, &MousePointerY, &MousePointerZ);
		if (gST->ConOut)
		{
			if ( 	
					( ((MousePointerX/glyphWidth) <= (UINT32)gST->ConOut->Mode->CursorColumn+(Screen_Left/glyphWidth)+1) && ((MousePointerX/glyphWidth) >= (UINT32)gST->ConOut->Mode->CursorColumn+(Screen_Left/glyphWidth)-2) ) &&
					( ((MousePointerY/glyphHeight) <= (UINT32)gST->ConOut->Mode->CursorRow+(Screen_Top/glyphHeight)+1) && ((MousePointerY/glyphHeight) >= (UINT32)gST->ConOut->Mode->CursorRow+(Screen_Top/glyphHeight)-1) )
			)
			{
				MouseStop ();
				OldMousePointerX=MousePointerX;
				OldMousePointerY=MousePointerY;
			}
			else
			{
				if ( !IsMouseOnSoftkbd() )  
					MouseFreeze ();
			}
		}

		gPopupEdit.Draw( popupedit );
		if((FALSE == DrawCursor) && CompleteReDrawFlag)
			DrawCursor =TRUE;
		DoRealFlushLines();

		if (IsTSECursorSupport())
		{
			if (TRUE == DrawCursor)	// Enabling at valid times
			{
				gST->ConOut->SetAttribute (gST->ConOut, popupedit->FGColor | popupedit->BGColor);
				gST->ConOut->SetCursorPosition (gST->ConOut, gCursorLeft, gCursorTop);
				if( !IsCharacteronSoftKbd((UINT32)gCursorLeft,(UINT32)gCursorTop))
					gST->ConOut->EnableCursor (gST->ConOut, TRUE);
				DrawCursor = FALSE;
			}
		}
		if  ( !IsMouseOnSoftkbd() )  //Avoid mouse flickers If mouse pointer is on softkbd area
		{
			SoftKbdRefresh = FALSE;       
			MouseRefresh();              
		}
		
		
Action:
		if ( gAction.GetAction( action ) != EFI_SUCCESS )
			continue;
		Status = gPopupEdit.HandleAction( popupedit, action );
		
		if(IsMouseSupported())
		{
			if((OldMousePointerX ==MousePointerX) && (OldMousePointerY ==MousePointerY))
			{
				GetCoordinates (&MousePointerX, &MousePointerY, &MousePointerZ);
				if(action->Input.Type != ACTION_TYPE_KEY)
				{
					if ( action->Input.Type != ACTION_TYPE_NULL )
						gAction.ClearAction( action );
					goto Action;
				}
			}
		}
	
		if (IsTSECursorSupport() && !(EFI_ERROR (Status)))
		{
			DrawCursor = TRUE;			// If any valid action performed then draw the cursor, usefull for blinking
		}
	}
	TSEStringReadLoopExitHook();
	return res;
}

/**
    function to perform the edit operations on popup

    @param PopupPassword UINT16 Title, CHAR16 **Text

    @retval status

**/
EFI_STATUS _DoPopupEdit( POPUP_PASSWORD_DATA *PopupPassword, UINT16 Title, CHAR16 **Text)
{
	static UINT8 gIFRPopupEdit[50];
	EFI_STATUS Status= EFI_SUCCESS;
	UINT8 retValue = (UINT8)-1;
	UINT16 newtoken = 0;

	POPUPEDIT_DATA *popupedit = NULL;

	CONTROL_INFO dummy;
    BOOLEAN 		PreviousCursorState = gST->ConOut->Mode->CursorVisible;

	Status = gPopupEdit.Create( (void**)&popupedit );
	if ( EFI_ERROR( Status ) )
		return Status;

	MemSet( &dummy, sizeof(dummy), 0 );

    MemCopy( &gIFRPopupEdit, (VOID*)PopupPassword->ControlData.ControlPtr, UefiGetIfrLength(PopupPassword->ControlData.ControlPtr) );

	//To show Control prompt string as PopupPassword String instead from AMITSEStr.uni file
	if (IsShowPromptStringAsTitle())
	{
		UINTN strlen = 0;
		CHAR16 *temptext, *tempTitle = NULL, *tText = NULL;		
		dummy.ControlHandle = PopupPassword->ControlData.ControlHandle;
		
		//Get TSE string
		temptext = HiiGetString( (VOID*)(UINTN)INVALID_HANDLE, Title);
		strlen = EfiStrLen(temptext);

		tempTitle = EfiLibAllocateZeroPool((strlen+2)*sizeof(CHAR16));
		
		if (NULL == tempTitle)
		{
			if(temptext)
				MemFreePointer( (VOID **)&temptext );
			return EFI_OUT_OF_RESOURCES;
		}

		EfiStrCpy (tempTitle,temptext);
		strlen = EfiStrLen(tempTitle);

		MemFreePointer( (VOID **)&temptext );

		//Get Prompt String from ControlPtr
		temptext = HiiGetString( PopupPassword->ControlData.ControlHandle, UefiGetPromptField(PopupPassword->ControlData.ControlPtr));
		strlen = EfiStrLen(temptext+2);

		tempTitle = MemReallocateZeroPool (
									tempTitle,
									( (EfiStrLen (tempTitle) + 2) * sizeof (CHAR16) ),
									( ((EfiStrLen (tempTitle) + 2) * sizeof (CHAR16)) + ((EfiStrLen (temptext) + 2) * sizeof (CHAR16)) )    //2 for /n and NULL character
									);

		if (NULL == tempTitle)
		{
			if(temptext)
				MemFreePointer( (VOID **)&temptext );
			return EFI_OUT_OF_RESOURCES;
		}

		//Eliminate white space character if any precedes
		tText = temptext;
		while(*temptext == L' ')
		{
			temptext++;
		}

		//Appending TSE string with controlptr string
		EfiStrCat (tempTitle,temptext);
		
		//Create new token for newly created string
		newtoken = HiiAddString( PopupPassword->ControlData.ControlHandle, tempTitle );

		//Set control prompt with newly created token
		UefiSetPromptField ((VOID*)&gIFRPopupEdit,newtoken);

		MemFreePointer( (VOID **)&tText );
		MemFreePointer( (VOID **)&tempTitle );
	}
	else
	{
		dummy.ControlHandle = (VOID*)(UINTN)INVALID_HANDLE;
		UefiSetPromptField ((VOID*)&gIFRPopupEdit,Title);
	}

	dummy.ControlPtr = (VOID*)&gIFRPopupEdit;
	dummy.ControlFlags.ControlVisible = TRUE;
	dummy.ControlFlags.ControlInteractive = PopupPassword->ControlData.ControlFlags.ControlInteractive;
   
	//gPopupEdit.SetTitle(popupedit,Title);
    gPopupEdit.SetText(popupedit,*Text);
	gPopupEdit.SetType( popupedit, POPUPEDIT_TYPE_PASSWORD );
	Status = gPopupEdit.Initialize( popupedit, &dummy );
	if ( EFI_ERROR( Status ) )
		goto Done;

	//gPopupEdit.SetType( popupedit, POPUPEDIT_TYPE_PASSWORD );
	retValue = (UINT8)_PopupEditGetValue( popupedit );
	if (IsTSECursorSupport())
	{
		gST->ConOut->EnableCursor (gST->ConOut, PreviousCursorState);
	}
	if(retValue ==0)
	{
		// string changed, save to nvram or do whatever needs to be done
	    MemFreePointer( (VOID **)Text );
	    *Text = EfiLibAllocateZeroPool( (gPopupEditCb.Length+1) * sizeof(CHAR16) );
	     if ( *Text == NULL )
		return EFI_OUT_OF_RESOURCES;

	    EfiStrCpy( *Text, gPopupEditCb.Data );			

	}
	else
		Status = EFI_UNSUPPORTED;
Done:
	gPopupEdit.Destroy( popupedit, TRUE );
	if ( IsShowPromptStringAsTitle() )
		HiiRemoveString(gHiiHandle, newtoken);//Delete created token
	return Status;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
