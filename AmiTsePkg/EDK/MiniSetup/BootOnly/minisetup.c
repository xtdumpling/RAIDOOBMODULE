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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/minisetup.c $
//
// $Author: Rajashakerg $
//
// $Revision: 9 $
//
// $Date: 4/27/12 5:16a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file minisetup.c
    This file contains code for entrypoint and setup loop

**/

#include "minisetup.h"
#include "Library/PcdLib.h"

#if TSE_BUILD_AS_APPLICATION
#include "AMITSEElinks.h"
#endif

#if APTIO_4_00 || SETUP_USE_GUIDED_SECTION
// from Resources.c
#if TSE_USE_EDK_LIBRARY
EFI_STATUS LoadStrings(
	EFI_HANDLE ImageHandle, /*EFI_HII_HANDLE*/VOID* *pHiiHandle
);
#endif
#endif

#if APTIO_4_00 || SETUP_USE_GUIDED_SECTION || TSE_FOR_64BIT
EFI_STATUS ReadImageResource(
	EFI_HANDLE ImageHandle, EFI_GUID *pGuid, 
	VOID **ppData, UINTN *pDataSize
);
#endif

//Internal function definitions
#if APTIO_4_00 != 1 && SETUP_USE_GUIDED_SECTION !=1
EFI_STATUS InitMiniSetupStrings( VOID );
#endif

EFI_DRIVER_ENTRY_POINT (MiniSetupApplication)

/////////////////////////////////////////////////////////
//	FUNCTION DECLARATION
////////////////////////////////////////////////////////
VOID InitGlobalPointers( VOID );
VOID UpdategScreenParams (VOID);
VOID StopClickEvent(VOID); 
AMI_SET_VERSION(TSE_MAJOR,TSE_MINOR,TSE_BUILD,TSE);
VOID        InvalidateStatusInBgrtWrapper (VOID);

BOOLEAN 	TseDefaultSetupPasswordSupported(VOID);
EFI_STATUS 	SetDefaultPassword (VOID);
EFI_STATUS NotificatonFunctionForGop();
BOOLEAN   TSEBreakInteractiveBbsPopup (void);
void PrintGopAndConInDetails();
extern EFI_GUID gEfiShellFileGuid;
#define	TSE_FIRST_BOOT_GUID	\
	{ 0xc5912ed9, 0x83c2, 0x4bff, 0x99, 0x36, 0x23, 0x1f, 0xeb, 0x85, 0xf3, 0xe8 }

CHAR16 *gMainTitle =  L"Aptio Setup Utility - Copyright (C) %04x American Megatrends, Inc.";
CHAR16 *gMainCopyright = L"Version %x.%02x.%04x. Copyright (C) %04x American Megatrends, Inc.";
BOOLEAN IsTseGopNotificiationFuncitonInstalled =FALSE;
extern BOOLEAN IsTSEGopNotificationSupport();
VOID RestorePasswordScreen();
extern BOOLEAN gPostPasswordFlag;
extern BOOLEAN	stPostScreenActive ;
extern CHAR16 gPostStatusArray[12][100];

#if TSE_BUILD_AS_APPLICATION
EFI_STATUS TSELoadStrings(
	EFI_HANDLE ImageHandle, EFI_HII_HANDLE *pHiiHandle
);
VOID GetArgumentsFromImage(EFI_HANDLE ImageHandle, CHAR16 ***pArgv, UINTN *pArgc);
UINTN 			gArgCount = 0;
CHAR16		**gArgv;

/**
    Returns CommandLine Arguments.

    @param ImageHandle

    @retval Fills the Argv and Argc pointers

**/
VOID GetArgumentsFromImage(EFI_HANDLE ImageHandle, CHAR16 ***pArgv, UINTN *pArgc)
{
    EFI_STATUS	status;
    EFI_LOADED_IMAGE	*image;
    EFI_GUID	loadedImageProtocol = LOADED_IMAGE_PROTOCOL;
    CHAR16	*cmdLine;
    UINTN	argc = 0, count;

    status = gBS->HandleProtocol( ImageHandle, &loadedImageProtocol, &image );

    if ( ! EFI_ERROR(status) )
    {
		UINTN	i;

		if ( image->LoadOptions == NULL )
		{
		    *pArgc = 0;
		    return;
		}

		cmdLine = (CHAR16 *)image->LoadOptions;
		count = StrLen (cmdLine);
		//count = image->LoadOptionsSize / sizeof(CHAR16);
		for ( i = 0; i < count; i++ )
		{
		    if ( cmdLine[i] == L' ' )
		    {
				cmdLine[i] = L'\0';
				argc++;
		    }
		}

		*pArgv = EfiLibAllocatePool  ((argc + 1) * sizeof (CHAR16 *));
		if ( *pArgv != NULL )
		{
		    for ( i = 0; i < argc; i++ )
		    {
				(*pArgv)[i] = cmdLine;
				cmdLine += StrLen (cmdLine) + 1;
		    }

		    (*pArgv)[i] = NULL;
		}
    }

    *pArgc = argc;

//	FreePool( cmdLine );
}

//*************************************************************************
/**
    EFI_STATUS LoadStrings(EFI_HANDLE ImageHandle,
    EFI_HII_HANDLE *pHiiHandle) - loads HII string packages associated with
    the specified image and publishes them to the HII database

        
    @param ImageHandle Image Handle
    @param pHiiHandle HII package list handle

    @retval 
        EFI_STATUS

**/
//*************************************************************************
#define EFI_HII_PACKAGE_LIST_PROTOCOL_GUID \
  { 0x6a1ee763, 0xd47a, 0x43b4, {0xaa, 0xbe, 0xef, 0x1d, 0xe2, 0xab, 0x56, 0xfc}}
static EFI_GUID gEfiHiiPackageListProtocolGuid = EFI_HII_PACKAGE_LIST_PROTOCOL_GUID;

EFI_STATUS TSELoadStrings(
	EFI_HANDLE ImageHandle, EFI_HII_HANDLE *pHiiHandle
)
{
    EFI_STATUS                      Status;
    EFI_HII_PACKAGE_LIST_HEADER     *PackageList;
    EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;

    //
    // Retrieve HII package list from ImageHandle
    //
    Status = pBS->OpenProtocol (
        ImageHandle,
        &gEfiHiiPackageListProtocolGuid,
        (VOID **) &PackageList,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = pBS->LocateProtocol (
        &gEfiHiiDatabaseProtocolGuid,
        NULL,
        &HiiDatabase
    );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Publish HII package list to HII Database.
    //
    Status = HiiDatabase->NewPackageList (
        HiiDatabase,
        PackageList,
        ImageHandle,
        pHiiHandle
    );

	return Status;
}
#endif   //For TSE_BUILD_AS_APPLICATION

/**
    This function is the entry point for TSE. It loads
    resources like strings and setup-data. It registers
    notification for console protocols. It Installs TSE
    protocols.

    @param ImageHandle 
    @param SystemTable 

    @retval Return Status based on errors that occurred in library
        functions.

**/

EFI_GUID TSEGetPCDptr();
EFI_STATUS MiniSetupApplication (
		EFI_HANDLE ImageHandle,
		EFI_SYSTEM_TABLE *SystemTable )
{
	EFI_STATUS	Status;
    UINTN 		OptionSize = 0;
    void 		*FirstBoot = NULL;
    EFI_GUID	TseFirstBootGuid = TSE_FIRST_BOOT_GUID;
    
	gImageHandle = ImageHandle;

	EfiInitializeDriverLib ( ImageHandle, SystemTable );
	
	RUNTIME_DEBUG( L"entry" );
	if(TRUE == SetupEntryHook())
		return EFI_UNSUPPORTED;
#if APTIO_4_00 || SETUP_USE_GUIDED_SECTION
#if TSE_USE_EDK_LIBRARY
	LoadStrings(ImageHandle,&gHiiHandle);
#else
#if TSE_BUILD_AS_APPLICATION	
	TSELoadStrings (ImageHandle,(EFI_HII_HANDLE*)&gHiiHandle);
#else
	LoadStrings (ImageHandle,(EFI_HII_HANDLE*)&gHiiHandle);
#endif	
#endif
#else
#ifdef USE_DEPRICATED_INTERFACE
	// Read in the strings from the GUIDed section
	Status = LoadStringsDriverLib( ImageHandle, &STRING_ARRAY_NAME );
	if ( EFI_ERROR( Status ) )
	{
		return Status;
	}
#endif

	Status = InitMiniSetupStrings();
	if ( EFI_ERROR( Status ) )
	{
		return Status;
	}
#endif
	gEfiShellFileGuid = TSEGetPCDptr();
	Status = HiiInitializeProtocol();
	if ( EFI_ERROR ( Status ) )
		return Status;

	//Override the STR_MAIN_TITLE and STR_MAIN_COPYRIGHT tokens to avoid the changes from uni.
	OverrideTitleString();
	
	// initialize screen buffer
	RUNTIME_DEBUG( L"screen" );
	InitializeScreenBuffer( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );
	if(IsTSEGopNotificationSupport())
	{
		if(!IsTseGopNotificiationFuncitonInstalled)
		{
			NotificatonFunctionForGop();
			IsTseGopNotificiationFuncitonInstalled =TRUE;
		}
	}
	RUNTIME_DEBUG( L"guid" );

	Status = InitApplicationData(ImageHandle);
	if ( EFI_ERROR( Status ) )
	{
		return MiniSetupExit( Status );
	}

	RUNTIME_DEBUG( L"globals" );
	InitGlobalPointers();
    UpdategScreenParams ();

	Status = VarLoadVariables( (VOID **)&gVariableList, NULL );
	if ( EFI_ERROR( Status ) )
	{
		return Status;
	}
#if TSE_BUILD_AS_APPLICATION
	GetArgumentsFromImage (ImageHandle, &gArgv, &gArgCount);
#endif	
    MinisetupDriverEntryHookHook();

	gPostStatus = TSE_POST_STATUS_BEFORE_POST_SCREEN;

#ifndef STANDALONE_APPLICATION
	// Install our handshake protocol
	InstallProtocol();

	// Register any notification 'callbacks' that we need
#if !TSE_FOR_EDKII_SUPPORT	
	Status = RegisterNotification();
	if ( EFI_ERROR( Status ) )
		UninstallProtocol();
#endif	
#else
	PostManagerHandshake();
#endif // STANDALONE_APPLICATION

    if (TseDefaultSetupPasswordSupported ())
    {
    	FirstBoot = (BOOT_OPTION *)VarGetNvramName (L"TseFirstBootFlag", &TseFirstBootGuid, NULL, &OptionSize);
    	if (NULL == FirstBoot)
    	{
    		SetDefaultPassword (); //Get default password, if any present in SDL, and set default pass to NVRAM 
    		VarSetNvramName( L"TseFirstBootFlag", &TseFirstBootGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, &OptionSize, sizeof (OptionSize) );
    	}
    }
	return Status;
}

/**
    This function is responsible for releasing the memory
    allocated during MinisetuEntry(). It destroys the
    application hierarchy; clears the memory used for
    variables; clears the memory used for fixup of
    setup-data.

    @param Status : Guideline status for MiniSetupExit
        to return with.

    @retval Return Status based on input status and errors that
        occurred in library functions.

**/
EFI_STATUS MiniSetupExit( EFI_STATUS Status )
{
	MiniSetupUIExit();

	if(gST->ConOut!=NULL)
	{
		gST->ConOut->Reset( gST->ConOut, FALSE );
		gST->ConOut->ClearScreen( gST->ConOut);
		gST->ConOut->EnableCursor( gST->ConOut, TRUE );
	}

	return Status;
}


/**
    This function runs the loop for Pop up boot menu.
    This function is responsible for creating the list
    box control. It also gathers actions and passes them
    to list box control and initiates draw sequence.

    @param BootFlow : Ptr to BOOT_FLOW responsible for
        the call.

    @retval Return Status based on errors that occurred in
        library functions.

**/
EFI_STATUS DoPopup(BOOT_FLOW *BootFlow)
{
	EFI_STATUS Status = EFI_SUCCESS;

	gPostStatus = TSE_POST_STATUS_IN_BBS_POPUP;

	DoBbsPopupInit() ; 	//minisetupext.c

	if (gClickTimer)
		StopClickEvent();

	//TSEStringReadLoopEntryHook();
	while(gEnterSetup != TRUE)
	{
			DrawPopupMenuHook() ;	//HookAnchor.c -> DrawBootOnlyBbsPopupMenu (minisetupext.c)
			BbsBootHook() ; //HookAnchor.c -> BbsBoot (minisetupext.c)
			if (TSEBreakInteractiveBbsPopup ())
			{
				break;
			}
	}
		
	return Status;
}
/**
    Main function that initializes the setup Loops.

    @param void

    @retval void

**/
VOID MainSetupLoopInit(VOID)
{
#ifndef STANDALONE_APPLICATION
//	EFI_CONSOLE_CONTROL_SCREEN_MODE ScreenMode;
#endif
	EFI_GUID AmitseSetupEnterGuid = AMITSE_SETUP_ENTER_GUID;
	static int Initdone =0;

	if(Initdone)
	{
		EfiLibNamedEventSignal ( &AmitseSetupEnterGuid );
		return;
	}
	Initdone = 1;
	
	InvalidateStatusInBgrtWrapper ();					//Anyway entering into setup/bbs popup so invalidating

/*	
#ifndef STANDALONE_APPLICATION
	if ( gConsoleControl != NULL )
	{
		gConsoleControl->GetMode(gConsoleControl, &ScreenMode, NULL, NULL);
		if(EfiConsoleControlScreenGraphics == ScreenMode)
		{
			gConsoleControl->SetMode( gConsoleControl, EfiConsoleControlScreenText );
			InvalidateStatusInBgrtWrapper ();
		}
	}
#endif
*/
	MouseInit();
	ProcessEnterSetupHook();
	
	TSEUnlockHDD();			///Modified as a hook 

	EfiLibNamedEventSignal ( &AmitseSetupEnterGuid );
	// Report the Status code DXE_SETUP_START
	EfiLibReportStatusCode(EFI_PROGRESS_CODE, DXE_SETUP_START,0,NULL,NULL);
}

/**
    Getting default password from SDL token based on USER/ADMIN token

    @param UINT32, AMITSESETUP*

    @retval EFI_STATUS

**/
CHAR16 *GetDefaultPasswordFromTokens (UINT32 PasswordType);
EFI_STATUS GetDefaultPassword (UINT32 PasswordInstalled, void **DefaultPassword)
{
	CHAR16 *adminPwd = NULL, *usrPwd = NULL;
	EFI_STATUS Status = EFI_NOT_FOUND;
	CHAR16 *tmpPasswd = NULL;
	
	if ((PasswordInstalled == AMI_PASSWORD_ADMIN) || (AMI_PASSWORD_NONE == PasswordInstalled))
	{
		usrPwd = GetDefaultPasswordFromTokens (AMI_PASSWORD_USER);
		if ((NULL != usrPwd) && (EfiStrCmp (usrPwd, L"\"\"")))
		{
			tmpPasswd =  (CHAR16 *)EfiLibAllocateZeroPool (sizeof (AMITSESETUP));
			if (NULL == tmpPasswd)
			{
				return EFI_OUT_OF_RESOURCES;
			}
			EfiStrCpy (tmpPasswd, usrPwd);
			MemFreePointer ((VOID **) &usrPwd);
			PasswordEncodeHook (tmpPasswd, TsePasswordLength*sizeof(CHAR16));
			MemCopy (*DefaultPassword, tmpPasswd, TsePasswordLength*sizeof(CHAR16));
			MemFreePointer ((VOID **) &tmpPasswd);
			Status = EFI_SUCCESS;
		}
	}

	if ((PasswordInstalled == AMI_PASSWORD_USER) || (AMI_PASSWORD_NONE == PasswordInstalled))
	{
		adminPwd = GetDefaultPasswordFromTokens (AMI_PASSWORD_ADMIN);
		if ((NULL != adminPwd) && (EfiStrCmp (adminPwd, L"\"\"")))
		{
			tmpPasswd = (CHAR16 *) EfiLibAllocateZeroPool (sizeof (AMITSESETUP));
			if (NULL == tmpPasswd)
			{
				return EFI_OUT_OF_RESOURCES;
			}
			EfiStrCpy (tmpPasswd, adminPwd);
			MemFreePointer ((VOID **) &adminPwd);
			PasswordEncodeHook (tmpPasswd, TsePasswordLength*sizeof(CHAR16));
			MemCopy ((CHAR16*)(*DefaultPassword) + TsePasswordLength, tmpPasswd, TsePasswordLength*sizeof(CHAR16));
			MemFreePointer ((VOID **) &tmpPasswd);			
			Status = EFI_SUCCESS;
		}
	}
	return Status;
}

/**
    Function to set defaults password in NVRAM, if any present in SDL

    @param void

    @retval EFI_STATUS

**/
BOOLEAN   TseRtAccessSupport (VOID);
EFI_STATUS SetDefaultPassword (VOID)
{
	UINTN 		size = 0;
	UINT32		Installed = AMI_PASSWORD_NONE;
	void 		*DefaultPassword = NULL;
	UINT32 		PasswordInstalled;
	EFI_STATUS  Status = EFI_SUCCESS;
	AMITSESETUP *pSetupVariable = (AMITSESETUP *)NULL;
    EFI_GUID 	amitsesetupGuid = AMITSESETUP_GUID;

	pSetupVariable = HelperGetVariable( VARIABLE_ID_AMITSESETUP, (CHAR16 *)NULL, (EFI_GUID *)NULL, NULL, &size );

	if(size < sizeof(AMITSESETUP))
	{
		pSetupVariable = (AMITSESETUP *) EfiLibAllocateZeroPool(sizeof(AMITSESETUP));
		if(gVariableList && (gVariableList[VARIABLE_ID_AMITSESETUP].Buffer))
			MemCopy( pSetupVariable, gVariableList[VARIABLE_ID_AMITSESETUP].Buffer, gVariableList[VARIABLE_ID_AMITSESETUP].Size );
      if (TseRtAccessSupport ())
      {
   		VarSetNvramName( L"AMITSESetup", &amitsesetupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS, pSetupVariable, sizeof (AMITSESETUP));
      }
      else
      {
         VarSetNvramName( L"AMITSESetup", &amitsesetupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, pSetupVariable, sizeof (AMITSESETUP));
      }  
//		VarSetNvram( VARIABLE_ID_AMITSESETUP, pSetupVariable, sizeof(AMITSESETUP));
	}
  	PasswordInstalled = PasswordCheckInstalled();
	
	if (AMI_PASSWORD_ANY == PasswordInstalled)
	{
		return EFI_ALREADY_STARTED;
	}
	
	Status = GetDefaultPassword (PasswordInstalled, &pSetupVariable);
	
	if (!EFI_ERROR (Status))
	{
      if (TseRtAccessSupport ())
      {
		   VarSetNvramName( L"AMITSESetup", &amitsesetupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS, pSetupVariable, sizeof (AMITSESETUP));
      }
      else
      {
         VarSetNvramName( L"AMITSESetup", &amitsesetupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, pSetupVariable, sizeof (AMITSESETUP));
      }
//      VarUpdateVariable(VARIABLE_ID_AMITSESETUP);
//		VarSetNvram( VARIABLE_ID_AMITSESETUP, pSetupVariable, sizeof(AMITSESETUP));
//		VarUpdateDefaults(VARIABLE_ID_AMITSESETUP);
	}
	return EFI_SUCCESS;
}

/**
    Function to Override the STR_MAIN_TITLE and the STR_MAIN_COPYRIGHT tokens to avoid the changes from uni.

    @param void

    @retval void

**/



VOID OverrideTitleString(void)
{
	CHAR16 *Language = (CHAR16*)NULL;
	CHAR8 *LangString =(CHAR8*)NULL;
	CHAR8 *Supportedlanguage = (CHAR8*)NULL,Lang[10] = " ";
		
	Supportedlanguage = _GetSupportedLanguages(gHiiHandle);
	if(Supportedlanguage == NULL)
		return ;
	LangString = Supportedlanguage;
	for (; *Supportedlanguage != '\0'; ) 
	{
		_GetNextLanguage(&Supportedlanguage,(CHAR8*)&Lang );
		Language = StrDup8to16((CHAR8*)&Lang);
		
		HiiChangeStringLanguage( gHiiHandle, STR_MAIN_TITLE,Language ,gMainTitle );
		HiiChangeStringLanguage( gHiiHandle, STR_MAIN_COPYRIGHT,Language ,gMainCopyright );
	}
	if(LangString)
		MemFreePointer((VOID*)&LangString);
	if(Language)
		MemFreePointer((VOID*)&Language);
		
}

/**
     Notification function for TSE GOP

    @param void

    @retval void

**/
VOID InTseGopNotificiationFunciton( EFI_EVENT Event, VOID *NotificationContext )
{
	SETUP_DEBUG_TSE ("\n[TSE] InTseGopNotificiationFunciton(), gPostStatus = %s\n", gPostStatusArray[gPostStatus]);
    UpdateGoPUgaDraw();    
    UpdateScreen();
    PrintGopAndConInDetails();
    if(gFlushBuffer && gActiveBuffer)
    {
        ClearFlushBuffer(STYLE_CLEAR_SCREEN_COLOR);
        if(TSE_POST_STATUS_PROCEED_TO_BOOT == gPostStatus)
        {  //do not update the screen
        	SETUP_DEBUG_TSE ("[TSE] Not restoring any screen, proceed to boot \n");
        	return;
        }
        if(TSE_POST_STATUS_IN_POST_SCREEN == gPostStatus)
        {
      	  stPostScreenActive = FALSE;
      	  if(!gPostPasswordFlag)
      	  {
			  SETUP_DEBUG_TSE ("[TSE] Drawing/Redrawing Post screen \n");
        		//To draw logo after gop is installed
      		  InitPostScreen();
      	  }
      	  else
      	  {
			  SETUP_DEBUG_TSE ("[TSE] Restoring Post password screen \n");
      		  RestorePasswordScreen();// For USER POST Password
      	  }
        }
        else if(TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN == gPostStatus)
        {                       
				if(!gPostPasswordFlag)
				{
					if(NULL != gPostscreenwithlogo)
					{
						SETUP_DEBUG_TSE ("[TSE] Restoring Post screen \n");
						RestoreGraphicsScreen ();
					}
					else
					{
						SETUP_DEBUG_TSE ("[TSE] Drawing/Redrawing Quietboot screen \n");
						DrawQuietBootLogoHook();
					}
				}
				else
				{
					SETUP_DEBUG_TSE ("[TSE] Restoring Post password screen \n");
					RestorePasswordScreen();// For USER POST Password
				}
        }
        else if(TSE_POST_STATUS_IN_BOOT_TIME_OUT == gPostStatus)
        {
      	  stPostScreenActive =FALSE;
      	  if(gQuietBoot)
      	  {
      		  if(NULL != gPostscreenwithlogo)
      		  {
				  SETUP_DEBUG_TSE ("[TSE] Restoring Post screen \n");
      			  RestoreGraphicsScreen ();
      		  }
      		  else
      		  {
				  SETUP_DEBUG_TSE ("[TSE] Drawing/Redrawing Quietboot screen \n");
      			  DrawQuietBootLogoHook();
      		  }
      	  }
      	  else
      	  {       		 
			  SETUP_DEBUG_TSE ("[TSE] Drawing/Redrawing Post screen \n");
      		  InitPostScreen();
      	  }       	 
        }
        else
        {
      	  if(!gPostPasswordFlag)
      	  {  
			  SETUP_DEBUG_TSE ("[TSE] Restoring current screen \n");
      		  FlushLines( 0, gMaxRows - 1 );
              DoRealFlushLines();
      	  }
      	  else
      	  {
			  SETUP_DEBUG_TSE ("[TSE] Restoring Post password screen \n");
      		  RestorePasswordScreen();// For Admin POST Password
      	  }
        }
    }
    SETUP_DEBUG_TSE ("[TSE] InTseGopNotificiationFunciton() Exiting : \n");
}

/**
    Creating Notification for TSE GOP

    @param void

    @retval EFI_STATUS

**/
EFI_STATUS NotificatonFunctionForGop()
{
	EFI_STATUS 	Status = EFI_UNSUPPORTED;
	EFI_EVENT 	GopEvent=NULL;
	VOID 		*GopRegistration = NULL;
	
	Status = gBS->CreateEvent(
					EFI_EVENT_NOTIFY_SIGNAL,
					EFI_TPL_CALLBACK,
					InTseGopNotificiationFunciton,
					NULL,
					&GopEvent);
	
	if (!EFI_ERROR (Status))
	{
		Status = gBS->RegisterProtocolNotify(
					&gEfiGraphicsOutputProtocolGuid,
					GopEvent,
					&GopRegistration
					);
	}
	return Status;	
}


void PrintGopAndConInDetails()
{
#if TSE_DEBUG_MESSAGES  
	UINTN i, SizeOfInfo = 0,ModeCols = 0, ModeRows = 0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION    *pModeInfo;
    EFI_STATUS Status;
  
    SETUP_DEBUG_TSE ("\n[TSE] PrintGopAndConInDetails() \n");

	if(gGOP && (gGOP->Mode->MaxMode != -1) &&  (gGOP->Mode->Mode != -1))
	{	
		SETUP_DEBUG_TSE ("[TSE] Gop Details: gGOP = %x\n", gGOP);
		SETUP_DEBUG_TSE ("[TSE] gGOP->Mode->MaxMode = %d  gGOP->Mode->Mode = %d\n",gGOP->Mode->MaxMode,gGOP->Mode->Mode);
		for(i = 0; i<gGOP->Mode->MaxMode;i++)
		{
			Status = gGOP->QueryMode(gGOP,(UINT32)i,&SizeOfInfo,&pModeInfo);
			SETUP_DEBUG_TSE ("[TSE] Mode : %d  QueryMode's Status = %r\n",i,Status);
			if((EFI_SUCCESS == Status) && SizeOfInfo)
	        {
				SETUP_DEBUG_TSE ("[TSE] HorizontalResolution = %d  VerticalResolution = %d \n",pModeInfo->HorizontalResolution,pModeInfo->VerticalResolution);
	        }
		}
	}

	if(gST->ConOut &&(gST->ConOut->Mode->MaxMode != -1) && (gST->ConOut->Mode->Mode != -1))	
	{
		SETUP_DEBUG_TSE ("[TSE] ConOut Details: gST->ConOut = %x\n", gST->ConOut);
		SETUP_DEBUG_TSE ("[TSE] gST->ConOut->Mode->MaxMode = %d  gST->ConOut->Mode->Mode = %d\n",gST->ConOut->Mode->MaxMode,gST->ConOut->Mode->Mode);
		for ( i = 0; i < (UINTN)gST->ConOut->Mode->MaxMode; i++ )
		{
			Status = gST->ConOut->QueryMode( gST->ConOut, i, &ModeCols, &ModeRows );
			SETUP_DEBUG_TSE ("[TSE] Mode : %d  QueryMode's Status = %r\n",i,Status);
			if(Status == EFI_SUCCESS)
				SETUP_DEBUG_TSE ("[TSE] ModeCols = %d   ModeRows = %d\n",ModeCols,ModeRows);
		}
	}
	
	SETUP_DEBUG_TSE ("[TSE] PrintGopAndConInDetails() Exiting :\n");
#endif
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
