//*******************************************************************
//*******************************************************************
//**                                                               **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.   **
//**                                                               **
//*******************************************************************
//*******************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Fixed cannot enter setup menu when press "DEL" key if there is no boot devices case.
//    Reason:
//    Auditor:  Jacker Yeh
//    Date:     Aug/17/2017
//
//  Rev. 1.02
//    Bug Fix:  Add no legacy bootable device prompt message function.
//    Reason:
//    Auditor:  Kasber Chen (from Greenlow)
//    Date:     Apr/12/2016
//
//  Rev. 1.01
//      Bug Fix:  Add uEFI re-try boot feature (Refer from Greenlow)
//      Reason:   
//      Auditor:  Jacker Yeh
//      Date:     Mar/18/16
//
//  Rev. 1.00
//      Bug Fixed:  Fixed DisplayQuietBootMessage function not work as expected problem.
//      Reason:     Temp solution before AMI fix kernel issue, it seems code algorithm is worng.
//      Auditor:    Jacker Yeh
//      Date:       Mar/02/2016
//
//*******************************************************************
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/protocol.c $
//
// $Author: Premkumara $
//
// $Revision: 35 $
//
// $Date: 9/24/12 9:14a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file protocol.c
    This file contains code for TSE exported protocol
    functions.

**/

#include "minisetup.h"
#include "boot.h"
#include "Include/Protocol/AMIPostMgr.h"
#include "Include/Protocol/AMIScreenMgmt.h"
//R1.00 - Start
#include <SetupDataDefinition.h>

#define SETUP_GUID { 0xEC87D643, 0xEBA4, 0x4BB5, 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }
EFI_GUID    guidSetup = SETUP_GUID;
SETUP_DATA  gSetup;
//R1.00 - End
UINT32 gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
BOOLEAN gBootNextFlag = FALSE;
UINT16 *pOSRecoveryOrder = (UINT16 *)NULL;
BOOLEAN OsRecoveryFlag = FALSE; //To Check OS recovery Options are Executed.
BOOLEAN PlatformRecoveryFlag = FALSE;//To Check Platform recovery Options are Executed.
BOOLEAN gOsRecoverySupported=FALSE; //OS recovery Support 
BOOLEAN DoNormalBootFlag=TRUE;
BOOLEAN gPlatformRecoverySupported=FALSE;
BOOLEAN gDoNotBoot = FALSE;
BOOLEAN gLaunchOtherSetup = FALSE;
BOOLEAN gDrawQuietBootMessage = FALSE;
UINT8 gRecoveryBootingType = 0; /* 0 - Normal boot, 1 - OsRecovery Boot and 2 - PlatformRecovery Boot */
extern UINT16 gGifSrcX;
extern UINT16 gGifSrcY;
extern BOOLEAN GifImageFlag;
extern BOOLEAN ActivateInputDone;
BOOT_DATA *RecoveryGetRecoveryData( UINT16 Option, UINT8 RecoveryBootingType);
EFI_STATUS ProcessRecovery(UINT16 *pRecoveryOrder,UINTN size, UINT8 RecoveryBootingType);
VOID StopClickEvent(VOID);
void ClearGrphxScreen (void);

static EFI_HANDLE gProtocolHandle = NULL;
//EFI_GUID gAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
BOOLEAN TseIgnoreKeyForFastBoot();
BOOLEAN GetBBSOptionStatus(BOOT_DATA *pBootData); 
extern VOID *SavePostScreen( UINTN *SizeOfX, UINTN *SizeOfY );
extern VOID RestorePostScreen( VOID *UgaBlt, UINTN SizeOfX, UINTN SizeOfY );
extern BOOLEAN IsTSEGopNotificationSupport();
BOOLEAN IsSetupPrintEvalMessage();
VOID InvalidateStatusInBgrtWrapper (VOID);
static AMI_POST_MANAGER_PROTOCOL	gPostManagerProtocol =
{
	PostManagerHandshake,
	PostManagerDisplayPostMessage,
    PostManagerDisplayPostMessageEx,
    PostManagerDisplayQuietBootMessage,
    PostManagerDisplayMsgBox,
    PostManagerSwitchToPostScreen,
	PostManagerSetCursorPosition,
	PostManagerGetCursorPosition,
	PostManagerInitProgressBar,
	PostManagerSetProgressBarPosition,
	PostManagerGetPostStatus,
	PostManagerDisplayInfoBox,
	PostManagerSetAttribute,
	PostManagerDisplayMenu,
	PostManagerDisplayMsgBoxEx,
	PostManagerDisplayProgress,
	PostManagerGetAttribute, 	
	PostManagerDisplayTextBox 	
};

static AMI_TSE_SCREEN_MGMT_PROTOCOL	gScreenMgmtProtocol =
{
		ScreenMgmtSaveTseScreen, 	
		ScreenMgmtRestoreTseScreen,
		ScreenMgmtClearScreen
};

static UINTN	gScreenWidth;
static UINTN 	gScreenHeight;
static EFI_UGA_PIXEL *gScreenBuffer;
static SCREEN_BUFFER *gScreenBuf = NULL;

EFI_STATUS InvalidateBgrtStatusByProtocol (VOID);
TSE_INVALIDATE_BGRT_STATUS_PROTOCOL gInvalidateBgrtStatus = {InvalidateBgrtStatusByProtocol};
EFI_STATUS InstallInvalBGRTStatusProtocol (EFI_HANDLE Handle);

EFI_STATUS ShowPostMsgBox(IN CHAR16  *MsgBoxTitle,IN CHAR16  *Message,IN UINT8  MsgBoxType, UINT8 *pSelection);


EFI_STATUS ShowPostMsgBoxEx(
	IN CHAR16			*Title,
 	IN CHAR16			*Message,
 	IN CHAR16			*Legend,
 	IN MSGBOX_EX_CATAGORY	 	MsgBoxExCatagory,
 	IN UINT8	 		MsgBoxType,
    	IN UINT16			*OptionPtrTokens,	// Valid only with MSGBOX_TYPE_CUSTOM 
    	IN UINT16	 		OptionCount,		// Valid only with MSGBOX_TYPE_CUSTOM
    	IN AMI_POST_MGR_KEY		*HotKeyList, 		// NULL - AnyKeyPress closes
    	IN UINT16 			HotKeyListCount, 
    	OUT UINT8			*MsgBoxSel,
    	OUT AMI_POST_MGR_KEY		*OutKey
	);
EFI_STATUS ShowInfoBox(IN CHAR16  *InfoBoxTitle, IN CHAR16  *Message, IN UINTN   TimeLimit, EFI_EVENT  *RetEvent);

EFI_STATUS HiiString2BltBuffer(	CHAR16 *Message,
								EFI_UGA_PIXEL Foreground, 
								EFI_UGA_PIXEL Background, 
								OUT	UINTN *Width,
								OUT EFI_UGA_PIXEL **BltBuffer,
								OUT UINTN *BltGlyphWidth);
EFI_STATUS DrawHiiStringBltBuffer(CHAR16 *Message, INTN CoOrdX, INTN CoOrdY, CO_ORD_ATTRIBUTE Attribute, EFI_UGA_PIXEL Foreground, EFI_UGA_PIXEL Background);
EFI_STATUS ShowPostMenu (
    IN VOID	*HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN UINT16 	LegendToken,	  
    IN POSTMENU_TEMPLATE *MenuData,
    IN UINT16 	MenuCount,
    OUT UINT16  *pSelection
);

EFI_STATUS ShowPostTextBox(
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE DisplayTextKeyValidate
);

EFI_STATUS ShowPostProgress(
    IN UINT8			ProgressBoxState, 
    IN CHAR16			*Title,
    IN CHAR16			*Message,
    IN CHAR16			*Legend,	
    IN UINTN 			Percent,	
    IN OUT VOID			**Handle,	
    OUT AMI_POST_MGR_KEY	*OutKey
);
BOOLEAN   	IsTseBestTextGOPModeSupported (VOID);
EFI_STATUS	SaveCurrentTextGOP ( UINTN *currenttextModeCols, UINTN *currenttextModeRows, UINT32 *currentGOPMode );
EFI_STATUS	RestoreTextGOPMode ( UINTN prevTextModeCols, UINTN prevTextModeRows, UINT32 prevGOPMode ); 
VOID SaveGraphicsScreen(VOID);

/**
    This function installs different protocols exported.

    @param VOID

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS InstallProtocol( VOID )
{
	EFI_STATUS Status;

	Status = gBS->InstallMultipleProtocolInterfaces(
			&gProtocolHandle,
			&gAmiPostManagerProtocolGuid, &gPostManagerProtocol,
#ifdef USE_COMPONENT_NAME
			&gEfiComponentNameProtocolGuid, &gComponentName,
#endif
			NULL
			);
	if ( !EFI_ERROR( Status ) )
	{
		Status = InstallFormBrowserProtocol(gProtocolHandle);
		Status = InstallInvalBGRTStatusProtocol (gProtocolHandle);
		Status = InstallScreenMgmtProtocol (gProtocolHandle);
	}

	return Status;
}

/**
    This function uninstalls different protocols exported.

    @param VOID

    @retval VOID

**/
VOID UninstallProtocol( VOID )
{
	gBS->UninstallMultipleProtocolInterfaces(
			&gProtocolHandle,
			&gAmiPostManagerProtocolGuid, &gPostManagerProtocol,
#ifdef USE_COMPONENT_NAME
			&gEfiComponentNameProtocolGuid, &gComponentName,
#endif
			NULL
			);

	UnInstallFormBrowserProtocol(gProtocolHandle);
	UninstallScreenMgmtProtocol(gProtocolHandle);

}

/**
    This function installs Screen management protocol.

    @param EFI_HANDLE Handle

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS InstallScreenMgmtProtocol( EFI_HANDLE Handle)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	Status = gBS->InstallMultipleProtocolInterfaces (
				  &Handle,
				  &gTSEScreenMgmtProtocolGuid,
				  &gScreenMgmtProtocol,
				  NULL
				  );
	
	return Status;
}

/**
    This function uninstalls screen management protocol.

    @param EFI_HANDLE Handle

    @retval VOID

**/
VOID UninstallScreenMgmtProtocol( EFI_HANDLE Handle)
{
	gBS->UninstallMultipleProtocolInterfaces(
			&Handle,
			&gTSEScreenMgmtProtocolGuid, 
			&gScreenMgmtProtocol,
			NULL
			);
}

VOID SetBootTimeout( EFI_EVENT Event, BOOLEAN *timeout )
{
	if ( timeout != NULL )
		*timeout = TRUE;
}

VOID AboartFastBootPath(VOID)
{
	gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
	gPostManagerHandshakeCallIndex--;
    gRT->SetVariable(
    			L"BootFlow",
    			&_gBootFlowGuid,
    			EFI_VARIABLE_BOOTSERVICE_ACCESS,
    			sizeof(gBootFlow),
    			&gBootFlow
    			);
}
BOOLEAN IsBootTimeOutValueZero(VOID);
/**
    This function is the handshake function to which BDS
    hands-off.

    @param VOID

    @retval This function never returns. It only boots different
        options.

**/
EFI_STATUS PostManagerHandshake( VOID )
{
	EFI_EVENT timer = NULL;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	volatile BOOLEAN bootTimeout = FALSE;
	UINT16  Value = 1;
    UINT64  TimeoutValue;
	UINTN   size = 0;
	EFI_GUID    AmitseAfterFirstBootOptionGuid = AMITSE_AFTER_FIRST_BOOT_OPTION_GUID;
	EFI_GUID AmiTseBeforeTimeOutGuid = AMITSE_BEFORE_TIMEOUT_GUID;
	EFI_GUID AmiTseAfterTimeOutGuid = AMITSE_AFTER_TIMEOUT_GUID;
	BOOLEAN AfterFirstBootSignalled = FALSE;
	UINT16  *pBootOrder=NULL;
	UINT16  u16BootCount = 0,i;				//Dont change i data type, in infinite loop it is reinitialized to -1.
	UINT16  BootNext, *pBootNext = NULL;
	UINTN   RecoverySize = 0;
	UINT16 *pRecoveryOrder=NULL;
	BOOLEAN BootSuccess=FALSE;
	UINT64 		OsIndications = 0;
	UINT32 		Attributes = 0;
	UINTN 		DataSize = sizeof (UINT64);  
#if SMCPKG_SUPPORT
    UINTN   Size=sizeof(SETUP_DATA);      //R1.00
    CHAR16	*text = L"Reboot and Select proper Boot device\n\rPlease press the <DEL> key into BIOS setup menu.\n\ror Press the <CTRL-ALT-DEL> key to reboot system.\n\n\r";
    UINT8	clear_fg = 0;
#endif

	// Performance measurement starta
	PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);

	RUNTIME_DEBUG( L"mre" );

	gPostManagerHandshakeCallIndex++;

	if(gPostManagerHandshakeCallIndex!=1)
	{
		gEnterSetup = TRUE;
		goto _ShowMainMenu;
	}

    PostManagerHandShakeHookHook();

#ifndef STANDALONE_APPLICATION
	if ( ! gConsoleControl )
	{
		//All necessary protocols are not available yet.
		//We can still proceed if Uga Draw is the only protocol not available yet.
		ActivateApplication();
		if (!IsDelayLogoTillInputSupported())
		{
			ActivateInputDone = TRUE;
			ActivateInput();
		}
	}
#endif

    if(!gVariableList)
        VarLoadVariables( (VOID **)&gVariableList, NULL );

	TimerStopTimer( &gKeyTimer );

	// this *MUST* be run a EFI_TPL_APPLICATION
	gBS->RaiseTPL( EFI_TPL_HIGH_LEVEL );	// guarantees that RestoreTPL won't ASSERT
	gBS->RestoreTPL( EFI_TPL_APPLICATION );

	// Performance measurement Pause
//	PERF_END (0,L"Boot", NULL, 0);
	// Exclude IDEPasswordCheck IDEPasswordCheck that it may get the use input.
	// IDE password Module takes care of the Password check.
	//TSEIDEPasswordCheck(); 

	// Performance measurement continue
//	PERF_START (0, L"Boot", NULL, 0);    

    // get the current boot options and languages
	BootGetBootOptions();
	BootGetLanguages();

	StyleUpdateVersionString();

	gPostStatus = TSE_POST_STATUS_IN_BOOT_TIME_OUT;

	//Providing boot time out value depending on TSE_BOOT_TIME_OUT_AS_ZERO token
	if(!IsBootTimeOutValueZero())
		Value = GetBootTimeOut(Value);
	else
		Value = 0;

#ifdef STANDALONE_APPLICATION
    Value = 0;
    gEnterSetup = TRUE;
#endif

	if ( Value == 0 )
		bootTimeout = TRUE;

	if ( !TseIgnoreKeyForFastBoot() ) 
	{
		CheckForKeyHook( (EFI_EVENT)NULL, NULL );
	}

    if((BOOT_FLOW_CONDITION_NORMAL != gBootFlow) || (gEnterSetup==TRUE))
    {
        UINT32 condition = BOOT_FLOW_CONDITION_NORMAL;
        UINT32 *conditionPtr;

        size = 0;
        conditionPtr = VarGetNvramName( L"BootFlow", &_gBootFlowGuid, NULL, &size );
        if ( conditionPtr != NULL )
        	condition = *conditionPtr;
        
        MemFreePointer( (VOID **)&conditionPtr );

		if((condition == BOOT_FLOW_CONDITION_FAST_BOOT) && ((gBootFlow != condition)||(gEnterSetup==TRUE)))
		{
			// Take the Normal boot path as Fast boot path is altered by CheckForKeyHook().
			AboartFastBootPath();
			return EFI_UNSUPPORTED;
		}

        if(BOOT_FLOW_CONDITION_NORMAL == condition)
        {
            gRT->SetVariable(
    					L"BootFlow",
    					&_gBootFlowGuid,
    					EFI_VARIABLE_BOOTSERVICE_ACCESS,//BootFlow wil use RS attribute to avoid inconsistent attributes
    					sizeof(UINT32),
    					&gBootFlow
    					);
        }
        else
            gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
    }

	Status = BootFlowManageEntry();
	if (Status == EFI_UNSUPPORTED)
    {
        Value = 0xFFFF;
        gDoNotBoot = TRUE;
    }
	if(Status == EFI_NOT_STARTED)
		bootTimeout = TRUE;

	if(gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT)
	{
		UINT32 PasswordInstalled = PasswordCheckInstalled();
		
		if((gPasswordType == AMI_PASSWORD_NONE) && (PasswordInstalled != AMI_PASSWORD_NONE))
		{
			if(CheckSystemPasswordPolicy(PasswordInstalled))
			{
				// Take the Normal boot path as Password is installed and not validated by the user yet
				AboartFastBootPath();
				return EFI_UNSUPPORTED;
			}
		}
	}
    else
    {
	// Reinit Before Boottimeout So any action can set the boot flow again.
	gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
    }

	if ( Value != 0xFFFF )
    {
        TimeoutValue = (UINT64) TIMER_TENTH_SECOND;
        TimeoutValue = MultU64x32( TimeoutValue, (UINT32) Value );

        TimerCreateTimer( &timer, (EFI_EVENT_NOTIFY) SetBootTimeout, (VOID *)&bootTimeout, TimerRelative, TimeoutValue, EFI_TPL_CALLBACK );
    }

	// Performance measurement Pause
	PERF_END (0, AMITSE_TEXT("Boot"), NULL, 0);

	// While the boot timeout has not expired
	EfiLibNamedEventSignal (&AmiTseBeforeTimeOutGuid);
	while ( ! bootTimeout )
	{
		if ( gEnterSetup || gBootFlow )
		{
			bootTimeout = TRUE;
            continue;
		}
		if ( !TseIgnoreKeyForFastBoot() ) 
		{
			// check for 'hotkey' actions that would invoke something
			CheckForKeyHook( (EFI_EVENT)NULL, NULL );
		}

        TimeOutLoopHookHook();
	}
	EfiLibNamedEventSignal (&AmiTseAfterTimeOutGuid);

	// Performance measurement continue
	PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);

	TimerStopTimer( &timer );

    if ((!gDoNotBoot) && (gBootFlow))
    {
        gRT->SetVariable(
					L"BootFlow",
					&_gBootFlowGuid,
					EFI_VARIABLE_BOOTSERVICE_ACCESS,//BootFlow wil use RS attribute to avoid inconsistent attributes
					sizeof(UINT32),
					&gBootFlow
					);
        BootFlowManageEntry();
    }

_ShowMainMenu:

	if ( gEnterSetup )
	{
    	// Performance measurement Pause
    	PERF_END (0, AMITSE_TEXT("Boot"), NULL, 0);
    	// Disable access to the post manager protocol display post message functions and to 
        //  the switch to post screen functions of post manager protocol
    	gPostMsgProtocolActive = FALSE;

		gPostStatus = TSE_POST_STATUS_ENTERING_TSE;
		gSetupContextActive = TRUE;

		Status = gST->ConIn->Reset( gST->ConIn, FALSE );
		Status = MainSetupLoopHook();
		gSetupContextActive = FALSE;
		

    	// Reenable access to the post manager protocol display post message functions and to 
        //  the switch to post screen functions of post manager protocol
    	gPostMsgProtocolActive = TRUE;
    	// Performance measurement continue
    	PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);
	}


	if(gPostManagerHandshakeCallIndex!=1)
	{
		// In case of not a first call
		// Don't do the Boot Manager Work just exit the to caller.
		gPostManagerHandshakeCallIndex--;
		return Status;
	}

	// Pass control to the boot process to handle the selected boot option
	// if the boot is allowed
    if (gDoNotBoot)
   {
		if(!ItkSupport())
		{
			gEnterSetup = TRUE;
			goto _ShowMainMenu;
		}
		else
			gEnterSetup = FALSE;
    }

	if(!NoVarStoreSupport())
	{
		size = 0;
		//VarGetNvram( VARIABLE_ID_AMITSESETUP, &size );
		HelperGetVariable( VARIABLE_ID_AMITSESETUP, (CHAR16 *)NULL, (EFI_GUID *)NULL, NULL, &size );
	
		if(size < sizeof(AMITSESETUP))
		{
			Status = MiniSetupEntry();
			MiniSetupExit( Status );
		}
	}

	gPostStatus = TSE_POST_STATUS_PROCEED_TO_BOOT;

	// LoadDriverOrder();	//Remove Load Driver Option from TSE 2.x as it is handling from Core.

    ProcessProceedToBootHook();

    TSEUnlockHDD();

	if(ItkSupport())
	    BbsItkBoot();
	else
	{
		//Get the Boot options Count first,Because under Platform recovery and BootFlow InfiniteLoop
		//we need to execute Platform Recovery Options First and normal boot options Next.
	    size =0 ;
	    pBootOrder = (UINT16 *)VarGetNvramName( L"BootOrder", &gEfiGlobalVariableGuid, NULL, &size );
	
	#ifdef EFI_NT_EMULATOR
	    if(!pBootOrder || !gBootData)
	    {
	        VarSetNvramName(L"BootOrder",
	        &gEfiGlobalVariableGuid,
	        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
	        NULL,
	        0);
	
	        return Status;
	    }
	#endif
	
	    //Get enabled options count
	    for(u16BootCount=0; u16BootCount<(UINT16)(size/sizeof(UINT16)); u16BootCount++ )
	    {
            if(!IsPreservedDisabledBootOptionOrder())
            {
    	        BOOT_DATA *pBootData;

	            pBootData = BootGetBootData(pBootOrder[u16BootCount]);
	            if(!(pBootData->Active & LOAD_OPTION_ACTIVE))
	                break;
            }
	    }
#if SMCPKG_SUPPORT	
	Status = gRT->GetVariable(L"Setup",&guidSetup, NULL, &Size, &gSetup);       //R1.00
#endif
	    
	    
		//Checks for PlatfromRecovery Bit.if The bit is set,platform will boot PlatformRecovery boot options
		//it won't execute BootNext,Boot Options,OS options when PlatfromRecovery Bit set		
		if(IsRecoverySupported() && gPlatformRecoverySupported && !PlatformRecoveryFlag)
		{
			goto _ProcessToPlatformRecovery;
		}
		
	    //Try boot next first
	    size = 0;
	    pBootNext = (UINT16 *)VarGetNvramName(L"BootNext", &gEfiGlobalVariableGuid, NULL, &size);
	    if(pBootNext && (size == sizeof(UINT16)))
	    {
	        BootNext = *pBootNext;
	        //Clear Boot next
	        VarSetNvramName(L"BootNext",
	                        &gEfiGlobalVariableGuid,
	                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
	                        pBootNext,
	                        0);
	        if( gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT) { 
				// FastBoot module normally checks for Bootnext before taking the FastBoot path
				// If the Boot next is set in FastBoot case then do the full boot.
				    AboartFastBootPath();
					return Status;
				}
			else {
				BBSSetBootNowPriorityForBootNext(BootNext, NULL, 0);
			}
		}
	
	    if(pBootNext)
	        MemFreePointer((void **) &pBootNext);

		if(!u16BootCount)
	    {
			gPostStatus = TSE_POST_STATUS_NO_BOOT_OPTION_FOUND;
	        //No valid/enabled boot option
			Status = BootFlowManageExit();
	            //infinite loop
	            while(EFI_NOT_STARTED == Status)
	            {
#if SMCPKG_SUPPORT
	            	if(clear_fg != 1){
	            		clear_fg = 1;
	            		CleanUpLogo();
	            		InitPostScreen();
	            	}
#else	        
                    CHAR16 *text = NULL;
                    
                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_NO_BOOT_OPTIONS));
#endif
                    if ( ( text != NULL ) && (gST->ConOut != NULL))
                    {
                    	gST->ConOut->OutputString(gST->ConOut, text);
#if SMCPKG_SUPPORT == 0//To avoid assert when enable debug mode
                    	MemFreePointer( (VOID **)&text );
#endif
                    }
                    
                    //Wait for key
                    gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &size );
                    CheckForKeyHook( (EFI_EVENT)NULL, NULL );
                    if (TRUE == gEnterSetup)                          //If setup key detects then launch the setup
                    {
#if SMCPKG_SUPPORT
					  if ((gSetup.SmcReTryBoot == 0) || (!u16BootCount))	//If there is no bootable devices, then allow to enter setup menu.
#endif
                        goto _ShowMainMenu;
                    }
                    Status = BootFlowManageExit ();                 //If user plugs the any thumb drive after boot then TSE will try to launch the image from that file system using the path efi\boot\bootxxxx.efi
					if( gBootFlow )// Checking for gBootFlow, making gBootFlow to normal such that the key will be consumed in chcekforkey
						gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
	            }
	    }
	
		if ( !gEnterSetup )// If no key is pressed to go to setup then stopping MousePointingDevice and stopping ClickEvents
		{
			StopClickEvent();
			MouseDestroy();
		}
//If Recovery support is disabled, normal boot options only should be processed
        if(IsRecoverySupported() && gOsRecoverySupported && !OsRecoveryFlag && !BootSuccess)
      	  DoNormalBootFlag = FALSE;

//Check for OS Recovery and process normal boot if unavailable
_ProcessBootOptions:			
SETUP_DEBUG_TSE("\n[TSE] _ProcessBootOptions Label Entering:\n DoNormalBootFlag = %d u16BootCount = %d\n",DoNormalBootFlag,u16BootCount);
	    for ( i = 0; (DoNormalBootFlag) && (i < u16BootCount); i++)
	    {
	        BOOT_DATA *pBootData;

	        pBootData = BootGetBootData(pBootOrder[i]);
	        SETUP_DEBUG_TSE("[TSE] i = %d\n",i);
	        if(pBootData)
	        {
	        	SETUP_DEBUG_TSE("\n[TSE] pBootData->Name = %s,pBootData->Option = %d,pBootData->Active = %x,BBSValidDevicePath = %d\n",pBootData->Name,pBootData->Option,pBootData->Active,BBSValidDevicePath(pBootData->DevicePath));
	        }
  	        if(pBootData == NULL || !(pBootData->Active & LOAD_OPTION_ACTIVE))
  	        {
  		        SETUP_DEBUG_TSE("[TSE] Invalid/Disabled boot option\n",i);
                continue;
  	        }

			if ( BBSValidDevicePath(pBootData->DevicePath) ) 
				if ( GetBBSOptionStatus (pBootData) )
				{
	  		        SETUP_DEBUG_TSE("[TSE] Disbaled Legacy option\n",i);
					continue;
				}

	        Status = BootLaunchBootOption(pBootOrder[i], pBootOrder+i, u16BootCount - i);
	        SETUP_DEBUG_TSE("[TSE] BootLaunchBootOption Status = %r\n",Status);
	        if(!EFI_ERROR(Status))
	        	BootSuccess=TRUE;
	        if ( !AfterFirstBootSignalled )
			{
				EfiLibNamedEventSignal ( &AmitseAfterFirstBootOptionGuid );

				if( gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT) {
				    AboartFastBootPath();
					return Status;
				}	

				AfterFirstBootSignalled = TRUE;
			}
	
	        if((i+1) >= u16BootCount)
	        {
	        	gPostStatus = TSE_POST_STATUS_ALL_BOOT_OPTIONS_FAILED;
#if SMCPKG_SUPPORT
				if ((gSetup.SmcReTryBoot == 0) || (gSetup.SmcReTryBoot == 1))
				{
	        	Status = BootFlowManageExit();
	        	if(!EFI_ERROR(Status))  //On BootFlowManage Success it won't execute the reset of Boot Cases.
		        	BootSuccess=TRUE;
				}
				i = 0;

#else
	            Status = BootFlowManageExit();
	        	if(!EFI_ERROR(Status))  //On BootFlowManage Success it won't execute the reset of Boot Cases.
		        	BootSuccess=TRUE;
#endif
	        	//if the BootFlow is in InfiniteLoop and OS recovery and PlatfromRecovey is not performed,Infinite loop code won't execute
	        	if ( (EFI_NOT_STARTED == Status)  && 
	        			(!IsRecoverySupported() || ((OsRecoveryFlag == TRUE) && (PlatformRecoveryFlag == TRUE)))
	        			)
				{
                    CheckForKeyHook( (EFI_EVENT)NULL, NULL );
                    if (TRUE == gEnterSetup)                          //If setup key detects then launch the setup
                    {
#if SMCPKG_SUPPORT
						if ((gSetup.SmcReTryBoot == 0) || (gSetup.SmcReTryBoot == 1))
#endif
                        goto _ShowMainMenu;
                    }
					// Infinite loop is true so we start again
					i = -1;											//Changed 0 to -1. Then only in next iteration it will be zero otherwise it will be 1.
				}													//If it is i = 0 then it will miss first boot option
	        }
	    }
	    if(IsRecoverySupported())
	    {
            //OSRecovery is performed if first time boot failed or gOsRecoverySupported flag is true
            if( (!OsRecoveryFlag) && (!BootSuccess ) )
            {
            	 DoNormalBootFlag = TRUE;
                Status = pRS->GetVariable (L"OsIndications", &gEfiGlobalVariableGuid, &Attributes, &DataSize, (VOID *)&OsIndications);
                if (!EFI_ERROR (Status))
                {
                   OsIndications = OsIndications & (~EFI_OS_INDICATIONS_START_OS_RECOVERY);
                   Status = pRS->SetVariable (L"OsIndications", &gEfiGlobalVariableGuid, Attributes, DataSize, (VOID *)&OsIndications);
                   
                   pOSRecoveryOrder  = (UINT16 *)VarGetNvramName( L"OsRecoveryOrder", &gEfiGlobalVariableGuid, NULL, &RecoverySize );
                   if(pOSRecoveryOrder != NULL && RecoverySize){
                		gRecoveryBootingType = OS_RECOVERY_OPTION_BOOTING;
                		Status= ProcessRecovery(pOSRecoveryOrder ,RecoverySize,gRecoveryBootingType);
                		gRecoveryBootingType = BOOT_OPTION_BOOTING;
                        if (!EFI_ERROR (Status))
                        BootSuccess=TRUE;
                   }
                }	
            }
            //It will again try normal boot for second time when OS Recovery failed.
            if( (!OsRecoveryFlag) && (!BootSuccess) ){
            	 OsRecoveryFlag = TRUE;                
                goto _ProcessBootOptions;
            }
            
_ProcessToPlatformRecovery:
				//Platform Recovery is performed when boot order failed for second time also or OSRecovery failed
				if(!PlatformRecoveryFlag && !BootSuccess)
            {
                Status = pRS->GetVariable (L"OsIndications", &gEfiGlobalVariableGuid, &Attributes, &DataSize, (VOID *)&OsIndications);
                if (!EFI_ERROR (Status))
                {
                   OsIndications = OsIndications & (~EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY);
                   Status = pRS->SetVariable (L"OsIndications", &gEfiGlobalVariableGuid, Attributes, DataSize, (VOID *)&OsIndications);
                   
                   pRecoveryOrder  = (UINT16 *)VarGetNvramName( L"PlatformRecoveryOrder", &gPlatformRecoveryOrderGuid, NULL, &RecoverySize );
                   if(pRecoveryOrder != NULL && RecoverySize)
                   {
                	   gRecoveryBootingType = PLATFORM_RECOVERY_OPTION_BOOTING;
                	   Status= ProcessRecovery(pRecoveryOrder ,RecoverySize,gRecoveryBootingType);
                	   gRecoveryBootingType = BOOT_OPTION_BOOTING;
                     if (!EFI_ERROR (Status))
                         BootSuccess=TRUE; 
                     else
                     {
                     	gPostStatus = TSE_POST_STATUS_ALL_PLATFORM_RECOVERY_OPTIONS_FAILED;
                     	Status = BootFlowManageExit();
                     	if (!EFI_ERROR (Status))
                     	{
                     		BootSuccess=TRUE;
                     	}
                     	
                     }
   
                   }
                }	
            }
				
            //To Avoid Executing the Os Recovery and Platfrom Recovery on Next Boot[After exiting from setup]
            PlatformRecoveryFlag = TRUE;
            OsRecoveryFlag = TRUE;
				
			//checking for Boot Flow InfiniteLoop value,if it is InfiniteLoop and all PlatformRecovery options failed,executing Normal Boot Options    
            if( IsBootFlowInfinite() && (!BootSuccess))
            {
            	goto _ProcessBootOptions;
            }
	    }
	}
#ifndef STANDALONE_APPLICATION
#if SMCPKG_SUPPORT
if (gSetup.SmcReTryBoot == 0)			//R1.00
#endif
    gEnterSetup = TRUE;
    goto _ShowMainMenu;
#else
	return Status;
#endif

}

/**
    This function is the protocol to display messages in
    the post screen.

    @param message : Unicode string to be displayed.

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS PostManagerDisplayPostMessage( CHAR16 *message )
{
	if ( ! gPostMsgProtocolActive )
		return EFI_UNSUPPORTED;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	return PrintPostMessage(message, TRUE);
}

/**
    This function is the protocol to display messages with
    attributes in the post screen.

    @param message : Unicode string to be displayed.
    @param Attribute : Attribute for the message

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS PostManagerDisplayPostMessageEx( CHAR16 *message, UINTN Attribute )
{
	if ( ! gPostMsgProtocolActive )
		return EFI_UNSUPPORTED;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	switch(Attribute)
    {
        case PM_EX_DONT_ADVANCE_TO_NEXT_LINE:
            return PrintPostMessage(message, FALSE);
    }

    return EFI_UNSUPPORTED;
}

/**
    This function is the protocol to switch to post screen

    @param VOID

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS PostManagerSwitchToPostScreen( VOID )
{
	EFI_GUID tSwitchToPoscreenGuid = AMITSE_SWITCHING_TO_POST_SCREEN_GUID;

#ifdef STANDALONE_APPLICATION
	return EFI_UNSUPPORTED;
#else
	if ( ! gPostMsgProtocolActive )
		return EFI_UNSUPPORTED;

	//Signalling event while using SwitchingToPostScreen interface
	EfiLibNamedEventSignal (&tSwitchToPoscreenGuid);
	
    if ( gQuietBoot )
	{
		UpdateGoPUgaDraw();
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
      if (!gGOP)
#else
      if (!gUgaDraw)
#endif
		 return EFI_UNSUPPORTED;
	
		gQuietBoot = FALSE;
		CleanUpLogo();
		//If Post message printed in the Quiteboot by using Set cur position
		// It is better to flash all the screen.
		//FlushLines( 0, gMaxRows - 1 );
		//DoRealFlushLines(); 
		InitPostScreen();
      return EFI_SUCCESS;
	}

    return EFI_UNSUPPORTED;
#endif
}
/**
    Initializes the PostManagerProgressBar

    @param x Column or Left screen position of ProgressBar in pixels
    @param y Row or Top screen position of ProgressBar in pixels
    @param w Width of ProgressBar in pixels
    @param h Height of ProgressBar in pixels
    @param iterate Total number of iterations or Calls
        to advanced the progressbar to 100% of the given width.
        The delta or changed value will be computed.								 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/

EFI_STATUS PostManagerInitProgressBar(
	UINTN x,
	UINTN y,
	UINTN w,
	UINTN h,
	UINTN iterate)
{
	EFI_STATUS Status = 0;
	EFI_UGA_PIXEL	*BGColor;
	EFI_UGA_PIXEL	*BDRColor;
	EFI_UGA_PIXEL	*FillColor;

	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->InitProgressBar (
					x,
					y,
					w,
					h,
					iterate);
	}
	else
	{
		BGColor = EfiLibAllocateZeroPool(sizeof (EFI_UGA_PIXEL));
		BDRColor = EfiLibAllocateZeroPool(sizeof (EFI_UGA_PIXEL));
		FillColor = EfiLibAllocateZeroPool(sizeof (EFI_UGA_PIXEL));
		
		GetProgressColor(BGColor,BDRColor,FillColor);
		
		gProgress->delta = w/iterate;
		gProgress->w = gProgress->delta*iterate;
		gProgress->h = h;
		gProgress->x = x;
		gProgress->y = y;
		gProgress->backgroundColor = BGColor;
		gProgress->borderColor = BDRColor;
		gProgress->fillColor = FillColor;
		gProgress->quiteBootActive = FALSE ;
		gProgress->active = TRUE ;         // progressbar has been initialized
	}
	return Status;
}

/**
    Increments the PostManagerProgressBar

    @param VOID
				 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/

EFI_STATUS PostManagerSetProgressBarPosition()
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->SetProgressBarPosition ();
	}
	else
	{
		if(gProgress->active)
			DrawBltProgressBar();
		else
			Status = EFI_UNSUPPORTED;
	}
	return Status;
}

/**
    Wrapper function for SetCurPos

    @param UINTN X - Value of the column 
        UINTN Y - Number of row below the last written line

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/

EFI_STATUS PostManagerSetCursorPosition(UINTN X, UINTN Y)
{
	return SetCurPos(X,Y);
}

/**
    Wrapper function for GetCurPos
    Writes cursor position into given X and Y locations.

    @param pX Pointer to storage for current column value 
    @param pY Pointer to storage for current row value

    @retval EFI_STATUS status - EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/

EFI_STATUS PostManagerGetCursorPosition(UINTN *pX, UINTN *pY)
{
	return GetCurPos(pX, pY);
}

/**
 

    @param MsgBoxTitle 
    @param Message 
    @param MsgBoxType 

    @retval EFI_STATUS status and UINT8 MsgBoxSel

**/
VOID SaveGraphicsScreen(VOID);
VOID RestoreGraphicsScreen(VOID);
EFI_STATUS
PostManagerDisplayMsgBox (
    IN CHAR16  *MsgBoxTitle,
    IN CHAR16  *Message,
    IN UINT8   MsgBoxType,
    OUT UINT8  *MsgBoxSel
	)
{

	EFI_STATUS Status = EFI_UNSUPPORTED;
	BOOLEAN ClickTimer = FALSE,KeyTimer = FALSE;
	EFI_TPL OldTpl;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;
	
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
	
	if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
		OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
		gBS->RestoreTPL (TPL_APPLICATION);
	}
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->DisplayMsgBox (
			    MsgBoxTitle,
			    Message,
			    MsgBoxType,
			    MsgBoxSel
			    );
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
#if SMCPKG_SUPPORT
		if( gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN )
#else
		if( gPostStatus <= TSE_POST_STATUS_ENTERING_TSE )
#endif
			SaveGraphicsScreen();

		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);
		if (EFI_ERROR(Status))
			goto DONE;

		Status = ShowPostMsgBox(MsgBoxTitle, Message, MsgBoxType,MsgBoxSel);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);
#if SMCPKG_SUPPORT
		if( gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN )
#else
		if( gPostStatus <= TSE_POST_STATUS_ENTERING_TSE )
#endif
			RestoreGraphicsScreen();
	}
	
DONE:
	if(KeyTimer)
	{
		InstallKeyHandlers();
	}		
	if(ClickTimer)
	{
		InstallClickHandlers();
	}	
	
	if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
		gBS->RaiseTPL (TPL_HIGH_LEVEL);
		gBS->RestoreTPL (OldTpl);
	}
	return Status;
}

/**
    Function for the quit booting display.

    @param Message INTN CoOrdX, INTN CoOrdY,
    @param Attribute EFI_UGA_PIXEL Foreground,
    @param Background 

    @retval Status

**/
EFI_STATUS
PostManagerDisplayQuietBootMessage(
    CHAR16 *Message,
    INTN CoOrdX,
    INTN CoOrdY,
    CO_ORD_ATTRIBUTE Attribute,
    EFI_UGA_PIXEL Foreground,
    EFI_UGA_PIXEL Background
    )
{
#ifdef STANDALONE_APPLICATION
	return EFI_UNSUPPORTED;
#elif defined TSE_FOR_APTIO_4_50
    UINTN BltGlyphWidth=0;
    EFI_UGA_PIXEL *BltBuffer=NULL;
    UINTN BltIndex;
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN SizeOfX, SizeOfY;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

    UpdateGoPUgaDraw();
    if(EFI_SUCCESS != HiiInitializeProtocol())
        return EFI_UNSUPPORTED;

    if(
        (EFI_SUCCESS != GetScreenResolution(&SizeOfX, &SizeOfY))||
        (!gQuietBoot)
        )
        return EFI_UNSUPPORTED;

    if ((Message != NULL) && (EfiStrLen(Message) == 0)) 
		return EFI_SUCCESS;
    BltIndex = 0;

	Status = HiiString2BltBuffer(Message,Foreground,Background,&BltIndex,&BltBuffer,&BltGlyphWidth);
	gDrawQuietBootMessage = TRUE;
    if(GifImageFlag) {
    	gGifSrcX = 0; 
    	gGifSrcY = 0;
    }
	if(Status == EFI_SUCCESS)
	    DrawBltBuffer(BltBuffer, Attribute, BltIndex, HiiGetGlyphHeight(), CoOrdX, CoOrdY, BltGlyphWidth);
	gDrawQuietBootMessage = FALSE;
    //Free BltBuffer
    MemFreePointer((VOID **)&BltBuffer);
    if ( IsTSEGopNotificationSupport() )
    	SaveGraphicsScreen();
    if (IsSetupPrintEvalMessage())
    	InvalidateStatusInBgrtWrapper();
    return Status;
#else
// For EDK Nt32 support.
//    Status = DrawHiiStringBltBuffer(Message, CoOrdX, CoOrdY, Attribute, Foreground, Background);
	return EFI_UNSUPPORTED;
#endif
}

/**
    Function returns current Post status.

    @param VOID

        One of the TSE_POST_STATUS
    @retval TSE_POST_STATUS_BEFORE_POST_SCREEN TSE Loaded and yet goto post screen
    @retval TSE_POST_STATUS_IN_POST_SCREEN In the post screen
    @retval TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN In the quite boot screen
    @retval TSE_POST_STATUS_IN_BOOT_TIME_OUT Witing for Boot timeout
    @retval TSE_POST_STATUS_ENTERING_TSE Entering TSE
    @retval TSE_POST_STATUS_IN_TSE Inside TSE
    @retval TSE_POST_STATUS_IN_BBS_POPUP Inside BBS Poupup
    @retval TSE_POST_STATUS_PROCEED_TO_BOOT Outside TSE and Booting or in Shell

**/
TSE_POST_STATUS	PostManagerGetPostStatus(VOID)
{
//	Status =	InitEsaTseInterfaces ();
	if ((TRUE == gLaunchOtherSetup) && (NULL != gEsaInterfaceForTSE))
	{
		return gEsaInterfaceForTSE->GetPostStatus ();
	}
	else
	{
		return gPostStatus;
	}
}


/**
    Function to display the Information box

    @param VOID

        One of the TSE_POST_STATUS
    @retval TSE_POST_STATUS_BEFORE_POST_SCREEN TSE Loaded and yet goto post screen
    @retval TSE_POST_STATUS_IN_POST_SCREEN In the post screen
    @retval TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN In the quite boot screen
    @retval TSE_POST_STATUS_IN_BOOT_TIME_OUT Witing for Boot timeout
    @retval TSE_POST_STATUS_ENTERING_TSE Entering TSE
    @retval TSE_POST_STATUS_IN_TSE Inside TSE
    @retval TSE_POST_STATUS_IN_BBS_POPUP Inside BBS Poupup
    @retval TSE_POST_STATUS_PROCEED_TO_BOOT Outside TSE and Booting or in Shell

**/
EFI_STATUS
PostManagerDisplayInfoBox(	IN CHAR16		*InfoBoxTitle,
							IN CHAR16		*InfoString,
							IN UINTN		Timeout,
							OUT EFI_EVENT	*Event
						 )
{
	EFI_STATUS Status = EFI_SUCCESS;
	BOOLEAN ClickTimer = FALSE,KeyTimer = FALSE;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;
	
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
		Status = gEsaInterfaceForTSE->DisplayInfoBox (
				InfoBoxTitle,
				InfoString,
				Timeout,
				Event
				);
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;

		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
			goto DONE;

		Status = ShowInfoBox(InfoBoxTitle, InfoString, Timeout, Event);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);		
	}
DONE : 
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


/**
    Function to set color

    @param Attrib Text color 
			 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/
EFI_STATUS PostManagerSetAttribute(UINT8 Attrib)
{
	gPostMgrAttribute = Attrib ;
	return EFI_SUCCESS ;
}

/**
    Function to Get color

    @param Attrib Pointer to get Text color 
			 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/
EFI_STATUS PostManagerGetAttribute(UINT8 *Attrib)
{
	*Attrib = gPostMgrAttribute ;
	return EFI_SUCCESS ;
}

/**
    Wrapper function for Display OEM Message box. 
               

    @param MsgBoxTitle Caption of the Message Box
    @param Message String to be displayed by the Message Box
    @param Legend Legend String to be displayed by the Message Box
    @param MsgBoxType Message Box type

    @retval EFI_STATUS status - EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/
EFI_STATUS PostManagerDisplayMsgBoxEx (
    IN CHAR16			*Title,
    IN CHAR16			*Message,
    IN CHAR16			*Legend,
    IN MSGBOX_EX_CATAGORY	MsgBoxExCatagory,
    IN UINT8	 		MsgBoxType,
    IN UINT16			*OptionPtrTokens,	// Valid only with MSGBOX_TYPE_CUSTOM 
    IN UINT16	 		OptionCount,		// Valid only with MSGBOX_TYPE_CUSTOM
    IN AMI_POST_MGR_KEY		*HotKeyList, 		// NULL - AnyKeyPress closes
    IN UINT16 			HotKeyListCount, 
    OUT UINT8			*MsgBoxSel,
    OUT AMI_POST_MGR_KEY	*OutKey
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	BOOLEAN ClickTimer = FALSE,KeyTimer = FALSE;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

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
		Status = gEsaInterfaceForTSE->DisplayMsgBoxEx (
			    Title,
			    Message,
			    Legend,
			    MsgBoxExCatagory,
			    MsgBoxType,
			    OptionPtrTokens,	// Valid only with MSGBOX_TYPE_CUSTOM 
			    OptionCount,		// Valid only with MSGBOX_TYPE_CUSTOM
			    HotKeyList, 		// NULL - AnyKeyPress closes
			    HotKeyListCount, 
			    MsgBoxSel,
			    OutKey
				);
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
		
		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
			goto DONE;

		Status = ShowPostMsgBoxEx (Title, Message, Legend, MsgBoxExCatagory,
											MsgBoxType, OptionPtrTokens, OptionCount,
											HotKeyList, HotKeyListCount, MsgBoxSel, OutKey
											);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);

		
	}
DONE:
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
/**
    Wrapper function for Display OEM Message box. 
               

    @param Percent Percent Complete

    @retval EFI_STATUS status - EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/
EFI_STATUS PostManagerDisplayProgress (
    IN UINT8	ProgressBoxState, 
    IN CHAR16	*Title,
    IN CHAR16	*Message,
    IN CHAR16	*Legend,
    IN UINTN 	Percent,	// 0 - 100
    IN OUT VOID	**Handle,	//Out HANDLE. Valid Handle for update and close
    OUT AMI_POST_MGR_KEY	*OutKey	//Out Key 
)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	BOOLEAN ClickTimer = FALSE,KeyTimer = FALSE;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;
	
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
		Status = gEsaInterfaceForTSE->DisplayProgress (
			    ProgressBoxState, 
			    Title,
			    Message,
			    Legend,
			    Percent,
			    Handle,
			    OutKey
				);
	}
	else
	{
		Status =  ShowPostProgress( ProgressBoxState, Title, Message, Legend, Percent, Handle, OutKey) ;		
	}

	if(KeyTimer)
	{
		InstallKeyHandlers();
	}		
	if(ClickTimer)
	{
		InstallClickHandlers();
	}	
	return Status ;
}
/**
    Wrapper function for Display Menu. 
               

    @param HiiHandle - Handle that contains String
    @param TitleToken - Menu Title Token
    @param LegendToken - Menu Legend Token  
    @param MenuData - Menu Items to be displayed 
    @param MenuCount - Number of Menu items
    @param pSelection Menu selection (DEFAULT)

    @retval EFI_STATUS status 		- EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/
EFI_STATUS PostManagerDisplayMenu (
    IN VOID	*HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN UINT16 	LegendToken,	  
    IN POSTMENU_TEMPLATE *MenuData,
    IN UINT16 	MenuCount,
    OUT UINT16  *pSelection
)
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	UINTN SizeOfX;
	UINTN SizeOfY; 
	EFI_UGA_PIXEL *UgaBlt = NULL;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	if(gPostStatus < TSE_POST_STATUS_IN_BOOT_TIME_OUT)
		return EFI_NOT_READY;
	
	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		//Set the tse data to esa
		/*Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (TSEHANDLE, 1, &gHiiHandle, 0);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (BOOTDATA,1, &gBootData, &gBootOptionCount);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (LANGDATA,1, &gLanguages, &gLangCount);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (GOP, 1, &gGOP, 0);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (PostStatus, 1, 0, &gPostStatus);*/
//		gEsaInterfaceForTSE->InitGraphicsLibEntry (gImageHandle, gST);
		
		return gEsaInterfaceForTSE->DisplayPostMenu (
					HiiHandle, 	
					TitleToken, 
					LegendToken,	  
					MenuData,
					MenuCount,
					pSelection
				);
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
		 if (TSE_POST_STATUS_IN_POST_SCREEN == gPostStatus||TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN == gPostStatus)
		 {
			 UgaBlt = SavePostScreen(&SizeOfX, &SizeOfY);
		 }
		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
			return Status;
		
		Status = ShowPostMenu(HiiHandle, TitleToken, LegendToken, MenuData, MenuCount, pSelection);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);
		 if (TSE_POST_STATUS_IN_POST_SCREEN == gPostStatus||TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN == gPostStatus)
		 {
			 RestorePostScreen(UgaBlt,SizeOfX,SizeOfY);
		 }

		return Status;

	}
}

/**
    Function to display text entry interface
 

    @param HiiHandle 
    @param TitleToken 
    @param InputData 
    @param ItemCount 
    @param ValidateKeyFunc 

    @retval EFI_STATUS

**/
EFI_STATUS
PostManagerDisplayTextBox (
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE ValidateKeyFunc
	)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	EFI_TPL OldTpl;
	BOOLEAN ClickTimer = FALSE,KeyTimer = FALSE;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

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
		Status = gEsaInterfaceForTSE->DisplayTextBox (
				HiiHandle, 	
			    TitleToken, 
			    InputData,
			    ItemCount,
			    ValidateKeyFunc
				);
		goto DONE;
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
		if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
				OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
				gBS->RestoreTPL (TPL_APPLICATION);
			}
		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
		{
			if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
			gBS->RaiseTPL (TPL_HIGH_LEVEL);
			gBS->RestoreTPL (OldTpl);
			}
			goto DONE;
		}

		Status = ShowPostTextBox(HiiHandle, TitleToken, InputData, ItemCount, ValidateKeyFunc);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);
		
		if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
				gBS->RaiseTPL (TPL_HIGH_LEVEL);
				gBS->RestoreTPL (OldTpl);
			}
		goto DONE;

	}
	DONE : 
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
/**
    Function to Stop the Click Event
               
    @param VOID

    @retval VOID

**/
VOID StopClickEvent(VOID)
{
	if(gClickTimer)
	{
		gBS->SetTimer ( gClickTimer,TimerCancel,0);
		TimerStopTimer( &gClickTimer );
	}
}


/**
    Protocol interface to invalidate BGRT status
 

    @param VOID
 

    @retval EFI_STATUS

**/
VOID InvalidateStatusInBgrtWrapper (VOID);
EFI_STATUS InvalidateBgrtStatusByProtocol (VOID)
{
	InvalidateStatusInBgrtWrapper();
	return EFI_SUCCESS;
}

/**
    Install Invalidate BGRT status protocol

    @param Handle 

    @retval EFI_STATUS
**/
EFI_STATUS InstallInvalBGRTStatusProtocol (EFI_HANDLE Handle)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gTSEInvalidateBgrtStatusProtocolGuid,
                  &gInvalidateBgrtStatus,
                  NULL
                  );

  return Status;
}

/**
        Name				:	SaveCurrentTextGOP

        Description		:	Function to Save current TextMode and GOP
               
        Input				:  UINTN currentTextModCols, UINTN currentTextModRows, UINT32 currentGOPMode

        Output			:	EFI_STATUS

**/
EFI_STATUS SaveCurrentTextGOP ( UINTN *currenttextModeCols, UINTN *currenttextModeRows, UINT32 *currentGOPMode )
{

	EFI_STATUS 	Status = EFI_SUCCESS;
	
	if ( IsTseBestTextGOPModeSupported() && TSE_POST_STATUS_IN_TSE != gPostStatus )
	{
		if (gGOP)
		{
			*currentGOPMode = gGOP->Mode->Mode;
		}

		if (TSE_BEST_HORIZONTAL_RESOLUTION != gGOP->Mode->Info->HorizontalResolution || TSE_BEST_VERTICAL_RESOLUTION != gGOP->Mode->Info->VerticalResolution)
		{
			SetScreenResolution(TSE_BEST_HORIZONTAL_RESOLUTION,TSE_BEST_VERTICAL_RESOLUTION); //If any postinterface is invoked during QuietBoot
		}

		Status = gST->ConOut->QueryMode( gST->ConOut, gST->ConOut->Mode->Mode, currenttextModeCols, currenttextModeRows);
		if (EFI_ERROR(Status))
			return EFI_NOT_FOUND;

		if ((STYLE_FULL_MAX_COLS != *currenttextModeCols)  || (STYLE_FULL_MAX_ROWS != *currenttextModeRows))
		{
			gMaxRows = STYLE_FULL_MAX_ROWS;
			gMaxCols = STYLE_FULL_MAX_COLS;
			SetDesiredTextMode ();
		}
	}
	return Status;
}

/**
        Name				:	RestoreTextGOPMode

        Description		:	Function to Save current TextMode and GOP
               
        Input				:  UINTN currentTextModCols, UINTN currentTextModRows, UINT32 currentGOPMode

        Output			:	EFI_STATUS

**/
EFI_STATUS RestoreTextGOPMode ( UINTN prevTextModeCols, UINTN prevTextModeRows, UINT32 prevGOPMode )
{
	EFI_STATUS 	Status = EFI_SUCCESS;

	if ( IsTseBestTextGOPModeSupported() && TSE_POST_STATUS_IN_TSE != gPostStatus )
	{
		if ( (gMaxRows != prevTextModeRows) || (gMaxCols != prevTextModeCols) )
		{
			gMaxRows = prevTextModeRows; 
			gMaxCols = prevTextModeCols;
			SetDesiredTextMode (); //Restoring to previous textmode if any changed
		}

		if (gGOP && prevGOPMode != gGOP->Mode->Mode)
		{
			Status = gGOP->SetMode (gGOP, prevGOPMode);	//In some case changing text mode will change the graphcis mode, so reverting here.
		}
	}
	return Status;
}
/**
        Name				:	ProcessRecovery

        Description		:	Function ProcessRecovery attempt
               
        Input				:  UINT16 *pRecoveryOrder, UINTN size, 
        Input				:  BOOLEAN - TRUE for OsRecovery, FALSE for PlatformRecovery

        Output			:	EFI_STATUS

**/
EFI_STATUS ProcessRecovery(UINT16 *pRecoveryOrder,UINTN size, UINT8 RecoveryBootingType)
{

	UINT16  u16RecoveryCount = 0,i;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	u16RecoveryCount = (UINT16)(size/sizeof(UINT16));
	for ( i = 0; i < u16RecoveryCount; i++)
	{
		 BOOT_DATA *pRecoveryData;	    
		 pRecoveryData = RecoveryGetRecoveryData(pRecoveryOrder[i],RecoveryBootingType);
		 if(pRecoveryData == NULL || !(pRecoveryData->Active & LOAD_OPTION_ACTIVE))
	                continue;
		 Status = BootLaunchBootOption(pRecoveryOrder[i], pRecoveryOrder+i, u16RecoveryCount - i);
		 if (!EFI_ERROR (Status))
			 return Status;
	}
	 
	return Status;
}

/**
        Name			:	ScreenMgmtSaveTseScreen

        Description		:	Save the current post screen
               
        Output			:	EFI_STATUS

**/
EFI_STATUS ScreenMgmtSaveTseScreen ()
{
	if (EFI_ERROR(InitEsaTseInterfaces()))
	{
		gScreenBuffer = (EFI_UGA_PIXEL *)SavePostScreen(&gScreenWidth, &gScreenHeight);	//Saving buffer of Quiteboot logo
		
		gScreenBuf = EfiLibAllocatePool( sizeof(SCREEN_BUFFER) );

		if (gScreenBuf && gActiveBuffer)
		{
			MemCopy( gScreenBuf, gActiveBuffer, sizeof(SCREEN_BUFFER) ); //Saving buffer of PostMessage, which will used to show message normal boot\quite boot
		}
		return EFI_SUCCESS;
	}
	else
	{
		return EFI_UNSUPPORTED;
	}
}

/**
        Name			:	ScreenMgmtRestoreTseScreen

        Description		:	Restore the saved post screen.
               
        Output			:	EFI_STATUS

**/
EFI_STATUS ScreenMgmtRestoreTseScreen ()
{

	if (EFI_ERROR(InitEsaTseInterfaces())) 
	{
		if (gActiveBuffer && gScreenBuf)
		{
			MemSet(gActiveBuffer, sizeof(SCREEN_BUFFER), 0);
			MemCopy( gActiveBuffer, gScreenBuf, sizeof(SCREEN_BUFFER) );	//Restoring Post message in active buffer. This will be drawn when DoRealFlushLines happens
		}
		RestorePostScreen(gScreenBuffer,gScreenWidth,gScreenHeight); //Restoring Quiteboot logo

		if (gScreenBuf)
			MemFreePointer((VOID **)&gScreenBuf);
			
		return EFI_SUCCESS;
	}
	else
	{
		return EFI_UNSUPPORTED;
	}
}

/**
        Name			:	ScreenMgmtClearScreen

        Description		:	Cleans the current screen
               
        Output			:	EFI_STATUS

**/
EFI_STATUS ScreenMgmtClearScreen (
		IN UINT8 Attribute
		)
{
	if (EFI_ERROR(InitEsaTseInterfaces())) 
	{
		ClearGrphxScreen ();//To clear screen while drawing logo at (0,0) co-ordinates
		ClearScreen( Attribute );
		DoRealFlushLines();
		return EFI_SUCCESS;
	}
	else
	{
		return EFI_UNSUPPORTED;
	}
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2015, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
