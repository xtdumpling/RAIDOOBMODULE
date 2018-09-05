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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/postmgmtext.c $
//
// $Author: Premkumara $
//
// $Revision: 3 $
//
// $Date: 5/28/12 11:27a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file postmgmtext.c
    This file contains code extended post management operations

**/

#include "minisetup.h"

////////////////////////////////////////////////////////////////////
//		Function Declarations
////////////////////////////////////////////////////////////////////
UINT8 GetPasswordPopupTextBoxColor(VOID);
UINT8 GetPasswordPopupWindowColor(VOID);
UINT8 GetPasswordReportInboxcolor(VOID);
BOOLEAN IsMouseSupported(VOID);
BOOLEAN IsSoftKbdSupported(VOID);
VOID SetPwdKeyboardLayout(VOID);
VOID ResetPwdKeyboardLayout(VOID);
BOOLEAN IsTSECursorSupport(); 
void 			ClearGrphxScreen (void);
BOOLEAN  IsTSEMultilineControlSupported (VOID);
EFI_STATUS SaveCurrentTextGOP ( UINTN *currenttextModeCols, UINTN *currenttextModeRows, UINT32 *currentGOPMode );
EFI_STATUS RestoreTextGOPMode ( UINTN prevTextModeCols, UINTN prevTextModeRows, UINT32 prevGOPMode );

VOID InvalidateStatusInBgrtWrapper (VOID);
BOOLEAN   IsWaitForKeyEventSupport (VOID);
EFI_STATUS MouseReadInfo(VOID *MouseInfo);
BOOLEAN   IsANSIEscapeCodeSupported(VOID);
VOID StopClickEvent(VOID);
extern UINTN       gSkipEscCode;
extern BOOLEAN 	 gPostPasswordFlag;
/**
    function to set the system access  Value

    @param sysAccessValue 

    @retval void

**/
VOID SetSystemAccessValue(UINT8 sysAccessValue)
{
	EFI_GUID sysAccessGuid = SYSTEM_ACCESS_GUID;
	if(NoVarStoreSupport())
	{
		NoVarStoreUpdateSystemAccess(sysAccessValue);
	}
	else
		VarSetNvramName( L"SystemAccess", &sysAccessGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &sysAccessValue, sizeof(sysAccessValue) );
}

/**
    function to set the type of password

    @param PasswordType 

    @retval void

**/
VOID SetPasswordType( UINT32 PasswordType )
{
	UINT8 sysAccessValue = SYSTEM_PASSWORD_ADMIN;

	gPasswordType = PasswordType;

	if ( gPasswordType == AMI_PASSWORD_USER )
		sysAccessValue = SYSTEM_PASSWORD_USER;

	if ( gPasswordType != AMI_PASSWORD_NONE )
	{
		if(ItkSupport())
		{
			SetSystemAccessValueItk(sysAccessValue);
		}
		else
		{	
			SetSystemAccessValue(sysAccessValue);
		}
	}

	return;
}
/*Removed from BootOnly
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	CheckEnableQuietBoot
//
// Description:	Function to check the quick boot enable option
//
// Input:	void
//
// Output:	void
//
// Notes		:	if NVRAM variable field AMISilentBoot = 0, then disable silent
//					mode else enable In case of error reading the Setup variable,
//					then return as silent boot disable
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CheckEnableQuietBoot( VOID )
{
#ifndef STANDALONE_APPLICATION
	if(ItkSupport())
	    gQuietBoot = TRUE;
	else
	{
		UINT8 *setupvar=NULL;
		UINT8 *setup=NULL;
		UINTN size = 0;
		UINTN offset;
	
		GetAMITSEVariable((AMITSESETUP**)&setup,&setupvar,&size);
	
		offset = STRUCT_OFFSET( AMITSESETUP, AMISilentBoot );
		if ( ( setup == NULL ) || ( size < offset ) )
			gQuietBoot = FALSE;
		else
			gQuietBoot = (BOOLEAN)setup[offset];
	
		if(setupvar)
			MemFreePointer( (VOID **)&setupvar );
		else
			MemFreePointer( (VOID **)&setup );
	}
#endif //#ifndef STANDALONE_APPLICATION
}
*/
/**
    Function to check the system password

    @param EmptyPasswordType UINTN *NoOfRetries, UINTN *TimeOut

    @retval PasswordCheck

**/
UINT32 CheckSystemPassword(UINT32 EmptyPasswordType, UINTN *NoOfRetries, UINTN *TimeOut)
{
	UINTN PasswordCheck = AMI_PASSWORD_NONE;
	UINTN CurrXPos, CurrYPos;
	CHAR16 *PasswordEntered;
	UINT32 PasswordInstalled;
	SCREEN_BUFFER *TempScreenBuffer = (SCREEN_BUFFER*)NULL;
	CHAR16 *OrgBoxStr=NULL;
	CHAR16 *BoxStr=NULL;
	UINTN BoxLength = TsePasswordLength;

	UINTN currenttextModeCols = 0, currenttextModeRows = 0;
	UINT32 currentGOPMode = 0;

	EFI_STATUS Status = EFI_SUCCESS;
	
	EFI_GUID AmitsePasswordPromptEnterGuid = AMITSE_PASSWORD_PROMPT_ENTER_GUID;
	EFI_GUID AmitsePasswordPromptExitGuid = AMITSE_PASSWORD_PROMPT_EXIT_GUID;
	EFI_GUID AmitseUserPasswordValidGuid = AMITSE_USER_PASSWORD_VALID_GUID;
	EFI_GUID AmitseAdminPasswordValidGuid = AMITSE_ADMIN_PASSWORD_VALID_GUID;
	EFI_GUID AmitseInvalidPasswordGuid = AMITSE_INVALID_PASSWORD_GUID;
	EFI_GUID AmitseAfterPostInvPwdPromptGuid = AMITSE_AFTER_POST_INVPwD_PROMPT_GUID;

    EfiLibNamedEventSignal (&AmitsePasswordPromptEnterGuid);
    gPostPasswordFlag = TRUE;
	// Report the Status code DXE_SETUP_VERIFYING_PASSWORD
	EfiLibReportStatusCode(EFI_PROGRESS_CODE, DXE_SETUP_VERIFYING_PASSWORD,0,NULL,NULL);

    CheckForKeyHook( (EFI_EVENT)NULL, NULL );
	if( IsMouseSupported() && IsSoftKbdSupported())
    	CheckForClickHook( (EFI_EVENT)NULL, NULL );

	gST->ConIn->Reset( gST->ConIn, FALSE );

    // Copy the contents of Active buffer so password code can use it
	TempScreenBuffer = EfiLibAllocateZeroPool( sizeof(SCREEN_BUFFER) );
	if(TempScreenBuffer != NULL)
		MemCopy(TempScreenBuffer, gActiveBuffer, sizeof(SCREEN_BUFFER));
	 OrgBoxStr = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ERROR_PSWD));
    if ( OrgBoxStr ){
   	 //Need to skip esc code while calculating width of passwordwindow
			BoxStr = SkipEscCode(OrgBoxStr);
        if ((TestPrintLength( BoxStr ) / NG_SIZE) > BoxLength ){
            BoxLength = TestPrintLength( BoxStr ) / NG_SIZE;
        }
        MemFreePointer((VOID**) &OrgBoxStr );
        MemFreePointer((VOID**) &BoxStr );
    }
    
	SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode); 

    //Draw password window
    _DrawPasswordWindow(STRING_TOKEN(STR_PASSWORD_PROMPT), BoxLength, &CurrXPos, &CurrYPos);

    PasswordEntered = EfiLibAllocatePool((TsePasswordLength + 1)*sizeof(CHAR16));
    PasswordInstalled = PasswordCheckInstalled();
    PasswordCheck = AMI_PASSWORD_NONE;

    //Give retries based on NoOfRetries
    while(*NoOfRetries)
    {
		//Handling ESC key for the the input password
		Status = _GetPassword (PasswordEntered, TsePasswordLength, CurrXPos, CurrYPos, TimeOut);
		if (EFI_ABORTED == Status)		//Status should be aborted, success or timeout. If time out we should not go for another iteration
		{								//If user presses ESC TSE handle that as invalid case so let it for another iteration.	
			continue;					
		}
		else if (EFI_TIMEOUT == Status)
		{
			break;
		}
        PasswordCheck = PasswordAuthenticate( PasswordEntered );
        PasswordCheck &= PasswordInstalled;

        if((PasswordEntered[0] == L'\0') && (AMI_PASSWORD_NONE == PasswordCheck))
        {
            //Honour EmptyPasswordType
            if(
                (!(PasswordInstalled & AMI_PASSWORD_USER ))&&
                (EmptyPasswordType & AMI_PASSWORD_USER)
                )
                PasswordCheck = AMI_PASSWORD_USER;
            if(
                (!(PasswordInstalled & AMI_PASSWORD_ADMIN ))&&
                (EmptyPasswordType & AMI_PASSWORD_ADMIN)
                )
                PasswordCheck = AMI_PASSWORD_ADMIN;
        }

        if(AMI_PASSWORD_NONE != PasswordCheck)
            break;
        else
        {
            if (1 == *NoOfRetries) // Do not wait for key if it is the lastest try
                _ReportInBox( TsePasswordLength, STRING_TOKEN(STR_ERROR_PSWD), CurrXPos, CurrYPos, FALSE);
            else
                _ReportInBox( TsePasswordLength, STRING_TOKEN(STR_ERROR_PSWD), CurrXPos, CurrYPos, TRUE);
        }

        (*NoOfRetries)--;
    }

    MemFreePointer((VOID **)&PasswordEntered);

    if(AMI_PASSWORD_NONE == PasswordCheck)
    {
		EfiLibNamedEventSignal (&AmitseInvalidPasswordGuid);
		EfiLibReportStatusCode(EFI_ERROR_CODE| EFI_ERROR_MAJOR, DXE_INVALID_PASSWORD,0,NULL,NULL);

        //Report Invalid password
		_ReportInBox( TsePasswordLength, STRING_TOKEN(STR_ERROR_PSWD), CurrXPos, CurrYPos, FALSE);
		
		//
		//Triggering Event after showing Invalid post password prompt during final attempt
		//
		EfiLibNamedEventSignal (&AmitseAfterPostInvPwdPromptGuid);
		
    }
    else
    {
        MouseStop();
    	ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );
		DoRealFlushLines();
        MouseRefresh();
        if(IsMouseSupported()&& IsSoftKbdSupported())//Initializing the mouse at post when mouse and softkbd present
        	MouseInit();
        if (!( PasswordInstalled & AMI_PASSWORD_ADMIN ))
		    PasswordCheck = AMI_PASSWORD_ADMIN;
	    SetPasswordType( (UINT32)PasswordCheck );

      	if(AMI_PASSWORD_ADMIN == PasswordCheck)
    		EfiLibNamedEventSignal (&AmitseAdminPasswordValidGuid);

    	if(AMI_PASSWORD_USER == PasswordCheck)
		    EfiLibNamedEventSignal (&AmitseUserPasswordValidGuid);
    }

	RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);
    EfiLibNamedEventSignal (&AmitsePasswordPromptExitGuid);
    gPostPasswordFlag = FALSE; 
   	//Copy the original active buffer contents
	if(TempScreenBuffer != NULL)
	{
		MemCopy(gActiveBuffer, TempScreenBuffer, sizeof(SCREEN_BUFFER));
		MemFreePointer((VOID **) &TempScreenBuffer);
	}
	return ((UINT32)PasswordCheck);
}

/**
    Function to draw password window with attributes

    @param PromptToken UINTN PasswordLength, UINTN *CurrXPos, UINTN *CurrYPos

    @retval void

**/
VOID _DrawPasswordWindow(UINT16 PromptToken, UINTN PasswordLength, UINTN *CurrXPos, UINTN *CurrYPos)
{
	UINTN	DlogWidth = PasswordLength + 4;
	UINTN Height=1;
   CHAR16 *PasswordPrompt = NULL;
 	CHAR16 *Title = NULL;
	EFI_STATUS Status = EFI_SUCCESS;
	
   InvalidateStatusInBgrtWrapper ();
   
   Status =	InitEsaTseInterfaces ();	

   if (!EFI_ERROR (Status))
	{
		gEsaInterfaceForTSE->DrawPasswordWindow (PromptToken, PasswordLength, CurrXPos, CurrYPos);
	}
	else
	{
#ifndef STANDALONE_APPLICATION
		if ( gConsoleControl != NULL )
			gConsoleControl->SetMode( gConsoleControl, EfiConsoleControlScreenText );
#endif

    //Set desired Mode.
#if !APTIO_4_00
		SetDesiredTextMode();
#else
		//  In Aptio use the Text mode as is it in post screen 
		if(gST->ConOut!=NULL)
		{
			UINTN Rows, Cols;
			if(EFI_ERROR(gST->ConOut->QueryMode( gST->ConOut, gST->ConOut->Mode->Mode, &Cols, &Rows )))
			{ 
				gMaxRows = STYLE_STD_MAX_ROWS; 
				gMaxCols = STYLE_STD_MAX_COLS; 
			}
			 else if ((gMaxRows != Rows) || (gMaxCols != Cols))
			{
				//Setting the Desired text mode as is it in post screen 
				SetDesiredTextMode ();
			}
	
		}
		else
		{ 
			gMaxRows = STYLE_STD_MAX_ROWS; 
			gMaxCols = STYLE_STD_MAX_COLS; 
		}
	#endif
	if(IsMouseSupported()&& IsSoftKbdSupported())//Initializing the mouse at post when mouse and softkbd present
		MouseInit();
		MouseStop();
		// Clear the Screen
		ClearGrphxScreen ();//To clear screen while drawing logo at (0,0) co-ordinates
		ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );
		DoRealFlushLines();
		MouseRefresh();

		Title = HiiGetString( gHiiHandle, PromptToken);

		//Add space before and after string 
		if(Title != NULL)
			PasswordPrompt = EfiLibAllocatePool((StrLen(Title)+3)*sizeof(CHAR16));
		
		if ( PasswordPrompt != NULL )
		{
			SPrint( PasswordPrompt, ( StrLen(Title) * sizeof(CHAR16)), L" %s ",Title );
			if ( (TestPrintLength(PasswordPrompt) / NG_SIZE +6) > DlogWidth )
				DlogWidth = TestPrintLength(PasswordPrompt) / NG_SIZE +6;
		}
		if ((PasswordPrompt != NULL) && (DlogWidth >= (gMaxCols-2))) //If string is more than gMaxCols 80/100
		{
			if ( IsTSEMultilineControlSupported() ) //If multiline supported
				{
					//Height=(DlogWidth / (gMaxCols-2))+(((DlogWidth % (gMaxCols-2)) > 0)?1:0);
					//Height++;
					DlogWidth = gMaxCols-2;
					StringWrapText(PasswordPrompt,(UINT16)DlogWidth,(UINT16*)(&(Height)));
				}
			else
				{
					DlogWidth = gMaxCols-2;
					PasswordPrompt[DlogWidth-4]=L'\0';
					PasswordPrompt[DlogWidth-5]=L'.';
					PasswordPrompt[DlogWidth-6]=L'.';
					PasswordPrompt[DlogWidth-7]=L'.';
				}
		}

		*CurrYPos = (gMaxRows - 5) / 2;
		*CurrXPos = (gMaxCols-2 - DlogWidth) / 2;
	
		//Set Password Window color
		//DrawWindow( *CurrXPos, *CurrYPos, DlogWidth, 5, EFI_BACKGROUND_BLUE | EFI_WHITE, TRUE, FALSE );
		DrawWindow( *CurrXPos, *CurrYPos, DlogWidth, 4+Height, GetPasswordPopupWindowColor(), TRUE, FALSE );	
	
		if ( PasswordPrompt != NULL )
		{
			if ( IsTSEMultilineControlSupported() ) //If multiline supported
			{
					if ((TestPrintLength(PasswordPrompt) / NG_SIZE +6) <= (gMaxCols-2))
						DrawStringJustified( 0, gMaxCols-2, *CurrYPos, JUSTIFY_CENTER, PasswordPrompt );
					else 
						DrawMultiLineStringWithAttribute ((*CurrXPos)+2, *CurrYPos, DlogWidth-3, Height, PasswordPrompt, 0);
			}
			else
				DrawStringJustified( 0, gMaxCols-2, *CurrYPos, JUSTIFY_CENTER, PasswordPrompt );
		}
		if(Title)
			MemFreePointer( (VOID **)&Title );
		if(PasswordPrompt)
			MemFreePointer( (VOID **)&PasswordPrompt );

	    //Record position of the Box
   	*CurrXPos = ((gMaxCols - DlogWidth) / 2)+1;
		*CurrYPos += Height+1;

		FlushLines( *CurrYPos - Height-1, *CurrYPos + 2);
	    MouseStop();
		DoRealFlushLines();
	    MouseRefresh();
	}
}

/**
    Function to report using Box

    @param PasswordLength UINT16 BoxToken, UINTN CurrXPos,
    @param CurrYPos BOOLEAN bWaitForReturn

    @retval void

**/
VOID _ReportInBox(
        UINTN PasswordLength,
        UINT16 BoxToken,
        UINTN CurrXPos,
        UINTN CurrYPos,
        BOOLEAN bWaitForReturn
        )
{
    CHAR16 *OrgBoxStr=NULL;
    CHAR16 *BoxStr=NULL;
   	EFI_INPUT_KEY Key;
   	EFI_STATUS Status = EFI_SUCCESS;
   	
	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		gEsaInterfaceForTSE->ReportInBox (
		        PasswordLength,
		        BoxToken,
		        CurrXPos,
		        CurrYPos,
		        bWaitForReturn
		        );
	}
	else
	{	
		//Draw Box
		//Set Password ReportInbox color
		//DrawBox( CurrXPos, CurrYPos,  PasswordLength, 1, EFI_BACKGROUND_BLUE | EFI_WHITE );
		DrawBox( CurrXPos, CurrYPos,  PasswordLength, 1, GetPasswordReportInboxcolor());
		//Draw Box string
		if(BoxToken != INVALID_TOKEN)
		{
			OrgBoxStr = HiiGetString( gHiiHandle, BoxToken );
			//If escape code is support is off, then escape characters should be removed before 
			//displaying the string. Else esccape chars are also displayed and password box is corrupted
			if(!IsANSIEscapeCodeSupported())
			{
				BoxStr = SkipEscCode(OrgBoxStr);
			}
			else
			{
				BoxStr = StrDup(OrgBoxStr);
			}
			DrawString( CurrXPos, CurrYPos, BoxStr);
		}
		 
		 SetPwdKeyboardLayout();
		TSEStringReadLoopEntryHook();
	
		FlushLines( CurrYPos - 2, CurrYPos + 2 );
		MouseStop();
		DoRealFlushLines();
		MouseRefresh();
	
		if(bWaitForReturn)
		{
			UINT16 DrawLength = 0;
			CHAR16 *TempStr=NULL;
			
			//If esc code is supported, then esc code should be removed from BoxStr before 
			// filling with spaces. else password window border is corrupted.
			gSkipEscCode = 1;
			DrawLength = (UINT16)(TestPrintLength(BoxStr)/ NG_SIZE);
			
			//The string length for invalid password in Chinese is returning double the original length.
			//overriding the BoxStr variable with L' ' causing Crash while freeing BoxStr.
			//So created a Variable TempStr,It was Allocating with DrawLength memory and filled the spaces and displaying on screen
			TempStr=EfiLibAllocateZeroPool((DrawLength+1)*sizeof(CHAR16));
			MemFillUINT16Buffer(TempStr, DrawLength, L' ');
			if(DrawLength)
				TempStr[DrawLength] = 0x0;					
			gSkipEscCode = 0;
			Key.UnicodeChar = 0;
			do
			{
				//Don't break this loop untill ENTER key is pressed.		
				if ( IsWaitForKeyEventSupport() )
				{
					Status = gBS->CheckEvent(gST->ConIn->WaitForKey);
					if (Status == EFI_NOT_READY) 
						continue;

					//
					//If current TPL is not TPL_APPLICATION then we'll get status as EFI_UNSUPPORTED from CoreWaitForEvent().
					//If no KEY is pressed, we'll get EFI_NOT_READY and this status will be handled in 
					//CoreWaitForEvent() itself. 
					//So when any key is pressed, finally we'll get status other than EFI_NOT_READY
					
				}
								
				gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
			} while( Key.UnicodeChar != CHAR_CARRIAGE_RETURN );
	
			DrawString( CurrXPos, CurrYPos, TempStr);
			MemFreePointer((VOID **)&TempStr);
		}
	
		TSEStringReadLoopExitHook();
		ResetPwdKeyboardLayout();
		if(BoxToken != INVALID_TOKEN)
		{
			MemFreePointer((VOID **)&OrgBoxStr);
			MemFreePointer((VOID **)&BoxStr);
		}
	}
}

VOID SetPwdTimeOut( EFI_EVENT Event, BOOLEAN *timeout )
{
	if ( timeout != NULL )
		*timeout = TRUE;
}

/**
    Function to get password from the user

    @param PasswordEntered UINTN PasswordLength, UINTN CurrXPos,
    @param CurrYPos UINTN *pTimeOut

    @retval status

**/
EFI_STATUS _GetPassword(
        CHAR16 *PasswordEntered,
        UINTN PasswordLength,
        UINTN CurrXPos,
        UINTN CurrYPos,
        UINTN *pTimeOut
        )
{
   	EFI_INPUT_KEY Key = {0,0};
	UINTN StrIndex;
    CHAR16 *StrStar;
    UINTN i;
    volatile BOOLEAN bTimeOut = FALSE;
    UINTN TimeOutValue;
    EFI_EVENT timer;
    EFI_STATUS Status;
    AMI_EFI_KEY_DATA    KeyData;
    EFI_TPL CurrentTpl = 0;
    MOUSE_INFO MouseInfo;
    BOOLEAN ClickTimer = FALSE,KeyTimer = FALSE;

   
	if (gKeyTimer)
	{
		TimerStopTimer( &gKeyTimer );
		KeyTimer  = TRUE;
	}
	if(gClickTimer)
	{
		StopClickEvent();
		ClickTimer = TRUE;
	}
	
	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->GetPassword (
		        PasswordEntered,
		        PasswordLength,
		        CurrXPos,
		        CurrYPos,
		        pTimeOut
		        );
		goto DONE;
		
	}
	else
	{
		MemSet( PasswordEntered, (PasswordLength + 1)*sizeof(CHAR16), 0 );
		StrStar = EfiLibAllocatePool((PasswordLength + 1)*sizeof(CHAR16));
		StrIndex = 0;
		
		//Set Password PopupTextBox color
		//DrawBox( CurrXPos, CurrYPos,  PasswordLength, 1, EFI_BACKGROUND_LIGHTGRAY | EFI_BLUE );
		DrawBox( CurrXPos, CurrYPos,  PasswordLength, 1, GetPasswordPopupTextBoxColor() );	
		
		FlushLines( CurrYPos, CurrYPos );
		MouseStop();
		DoRealFlushLines();
		MouseRefresh();
	
		SetPwdKeyboardLayout();
	   TSEStringReadLoopEntryHook();
	
		if (IsTSECursorSupport()) 
		{
			gST->ConOut->SetCursorPosition( gST->ConOut, CurrXPos, CurrYPos);	// Cursor support for msg boxes
			gST->ConOut->EnableCursor(gST->ConOut, TRUE);// Enable Cursor support for msg boxes
		}
		do
		{
			TimeOutValue = pTimeOut ? *pTimeOut : 0;
	
			if(TimeOutValue)
			{
				timer = NULL;
				Status = TimerCreateTimer( &timer, (EFI_EVENT_NOTIFY)SetPwdTimeOut, (VOID *)&bTimeOut, TimerRelative, TimeOutValue * TIMER_ONE_SECOND, EFI_TPL_NOTIFY );
			}
	
			// While the timeout has not expired
			 //Spec recommends to use simple input protocol in application level. So changing TPL here. HDD module will call _GetPassword in notfiy TPL level.
			CurrentTpl = gBS->RaiseTPL( TPL_HIGH_LEVEL ); //we store the current TPL
			gBS->RestoreTPL( TPL_APPLICATION); //setting the TPL level as application Level TPL
			
			while ( ! bTimeOut )
			{
				Status = AMIReadKeyStroke(&Key,&KeyData);
				if ( !(EFI_ERROR(Status)) )
					break;
				// read mouse clicks if mouse is present without soft kbd. If not read, mouse pointer hangs until this click is consumed
				if( IsMouseSupported() && (!IsSoftKbdSupported()) )
					MouseReadInfo(&MouseInfo);
			}
			gBS->RaiseTPL( TPL_HIGH_LEVEL ); // Raising the TPL before reverting back as TPL may be changes in between
			gBS->RestoreTPL( CurrentTpl );
	
			if(TimeOutValue)
				TimerStopTimer( &timer );
	
			if(bTimeOut)
			{
				Status = gST->ConIn->Reset( gST->ConIn, FALSE );
				MemFreePointer((VOID **)&StrStar);
				PasswordEntered[0] = L'\0';
				break;
			}
	
			switch (Key.UnicodeChar)
			{
				case CHAR_NULL:
					if (Key.ScanCode == SCAN_ESC) 
				{
					Status = gST->ConIn->Reset( gST->ConIn, FALSE );
						MemFreePointer((VOID **)&StrStar);
						PasswordEntered[0] = L'\0';
						TSEStringReadLoopExitHook();
						ResetPwdKeyboardLayout();
					   Status =  EFI_ABORTED;
					   goto DONE;
				}
					if (Key.ScanCode != SCAN_NULL) {
						if(HelperIsPasswordCharValid(&Key,&KeyData,StrIndex,PasswordLength,&PasswordEntered[StrIndex]) == EFI_SUCCESS)
							StrIndex++;
					}
					else {
						InvalidActionHookHook();
					}
					break;
	
				case CHAR_CARRIAGE_RETURN:
					Status = gST->ConIn->Reset( gST->ConIn, FALSE );
				MemFreePointer((VOID **)&StrStar);
				TSEStringReadLoopExitHook();
				ResetPwdKeyboardLayout();
				Status =  EFI_SUCCESS;
				goto DONE;
	
				case CHAR_BACKSPACE:
					if ( StrIndex != 0 )
						PasswordEntered[--StrIndex] = L'\0';	
					break;
				
				default:
					if(HelperIsPasswordCharValid(&Key,&KeyData,StrIndex,PasswordLength,&PasswordEntered[StrIndex]) == EFI_SUCCESS)
						StrIndex++;
					break; 
			}
			if ( StrIndex > PasswordLength  ) {
				InvalidActionHookHook();
			}
			//Draw stars for entered characters
			for ( i = 0; i < PasswordLength; i++ )
				StrStar[i] = (i<StrIndex) ? L'*': L' ';
			StrStar[PasswordLength] = L'\0';
	
			DrawString( CurrXPos, CurrYPos, StrStar );
			FlushLines( CurrYPos, CurrYPos );
			MouseStop();
			DoRealFlushLines();
	
			if (IsTSECursorSupport()) 
			{
				gST->ConOut->SetCursorPosition( gST->ConOut, ((PasswordLength!=StrIndex)?(CurrXPos + StrIndex):(CurrXPos + StrIndex-1)), CurrYPos);	// For restricting the cursor within the limit
				gST->ConOut->EnableCursor(gST->ConOut, TRUE);		//Enable Cursor support for msg boxes
			}
			MouseRefresh();
		}
		while( 1 );
	
		TSEStringReadLoopExitHook();
		ResetPwdKeyboardLayout();
	
		if (pTimeOut)
			*pTimeOut = 0;
		Status = EFI_TIMEOUT;
		goto DONE;
	}
DONE:
	//gBS->RaiseTPL( TPL_HIGH_LEVEL ); // Raising the TPL before reverting back as TPL may be changes in between
	//gBS->RestoreTPL( CurrentTpl );	//reverting back the original TPL
	if(KeyTimer)
	{
		InstallKeyHandlers();
	}		
	if(ClickTimer)
	{
		InstallClickHandlers();
	}
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
