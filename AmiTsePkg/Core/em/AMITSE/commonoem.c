//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.01
//       Bug fixed:   Do not allow press "Enter" to enter BIOS SETUP menu if User Password not exist.
//       Reason:      SMC Policy
//       Auditor:     Kasber Chen (refer Npales)
//       Date:        Jun/05/2017
//
//  Rev. 1.00
//    Bug Fix:  Fixed can enter setup menu without password when system only has ADMIN password problem.
//    Reason:   Still need to add setup option to define prompt password feature for "Always" or "Setup".
//    Auditor:  Jacker Yeh
//    Date:     Oct/15/2016
//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/commonoem.c $
//
// $Author: Arunsb $
//
// $Revision: 39 $
//
// $Date: 5/29/12 3:18a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file commonoem.c
    contains default implementation of TSE hooks

**/

#ifdef TSE_FOR_APTIO_4_50

#include "Token.h"
#include <Efi.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/EfiOemBadging.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/EsaTseInterfaces.h>
#include "AMITSEStrTokens.h"
#include "AMITSEElinks.h"
#include <AmiDxeLib.h>

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#include "Protocol/AmiKeycode.h"
#else
#ifndef _AMI_EFI_KEY_DATA_
#define _AMI_EFI_KEY_DATA_
typedef struct {
  EFI_INPUT_KEY  Key;
} AMI_EFI_KEY_DATA;
#endif
#endif

#pragma pack (1)
#ifndef _TSE_EFI_IFR_FORM_SET
#define _TSE_EFI_IFR_FORM_SET
typedef struct _TSE_EFI_IFR_FORM_SET {
  EFI_IFR_OP_HEADER        Header;
  EFI_GUID                 Guid;
  EFI_STRING_ID            FormSetTitle;
  EFI_STRING_ID            Help;
  UINT8                    Flags;
  EFI_GUID                 ClassGuid[1];
} TSE_EFI_IFR_FORM_SET;
#endif
#pragma pack ()

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
EFI_GUID gAmiEfiKeycodeProtocolGuid = AMI_EFIKEYCODE_PROTOCOL_GUID;
#endif

VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  );

#else //#ifdef TSE_FOR_APTIO_4_50

#include "minisetup.h"
#include "AMITSEElinks.h"
#endif //#ifdef TSE_FOR_APTIO_4_50

#ifdef TSE_FOR_APTIO_4_50
// Definitions of keys are different in Alaska and Tiano. So for
// Alaska define Tiano key definitions.

#define CharIsUpper(c) ((c >= L'A') && (c <= L'Z'))
#define CharIsLower(c) ((c >= L'a') && (c <= L'z'))
#define CharIsAlpha(c) (CharIsUpper(c) || CharIsLower(c)) 
#define CharIsNumeric(c) ((c >= L'0') && (c <= L'9'))

#define CHAR_NULL             0x0000
#define CHAR_BACKSPACE        0x0008
#define CHAR_TAB              0x0009
#define CHAR_LINEFEED         0x000A
#define CHAR_CARRIAGE_RETURN  0x000D

#ifndef SCAN_NULL
#define SCAN_NULL       EFI_SCAN_NULL
#endif

#ifndef SCAN_UP
#define SCAN_UP         EFI_SCAN_UP
#endif

#ifndef SCAN_DOWN
#define SCAN_DOWN       EFI_SCAN_DN
#endif

#ifndef SCAN_RIGHT
#define SCAN_RIGHT      EFI_SCAN_RIGHT
#endif

#ifndef SCAN_LEFT
#define SCAN_LEFT       EFI_SCAN_LEFT
#endif

#ifndef SCAN_HOME
#define SCAN_HOME       EFI_SCAN_HOME
#endif

#ifndef SCAN_END
#define SCAN_END        EFI_SCAN_END
#endif

#ifndef SCAN_INSERT
#define SCAN_INSERT     EFI_SCAN_INS
#endif

#ifndef SCAN_DELETE
#define SCAN_DELETE     EFI_SCAN_DEL
#endif

#ifndef SCAN_PAGE_UP
#define SCAN_PAGE_UP    EFI_SCAN_PGUP
#endif

#ifndef SCAN_PAGE_DOWN
#define SCAN_PAGE_DOWN  EFI_SCAN_PGDN
#endif

#ifndef SCAN_F1
#define SCAN_F1         EFI_SCAN_F1
#endif

#ifndef SCAN_F2
#define SCAN_F2         EFI_SCAN_F2
#endif

#ifndef SCAN_F3
#define SCAN_F3         EFI_SCAN_F3
#endif

#ifndef SCAN_F4
#define SCAN_F4         EFI_SCAN_F4
#endif

#ifndef SCAN_F5
#define SCAN_F5         EFI_SCAN_F5
#endif

#ifndef SCAN_F6
#define SCAN_F6         EFI_SCAN_F6
#endif

#ifndef SCAN_F7
#define SCAN_F7         EFI_SCAN_F7
#endif

#ifndef SCAN_F8
#define SCAN_F8         EFI_SCAN_F8
#endif

#ifndef SCAN_F9
#define SCAN_F9         EFI_SCAN_F9
#endif

#ifndef SCAN_F10
#define SCAN_F10        EFI_SCAN_F10
#endif

#ifndef SCAN_F11
#define SCAN_F11        EFI_SCAN_F11
#endif

#ifndef SCAN_F12
#define SCAN_F12        EFI_SCAN_F12
#endif

#ifndef SCAN_ESC
#define SCAN_ESC        EFI_SCAN_ESC
#endif


#endif //#ifdef TSE_FOR_APTIO_4_50
#include "commonoem.h"
#include "LogoLib.h"
#include "mem.h"
#include "HiiLib.h"
#include "PwdLib.h"
#include "Keymon.h"
#include "bootflow.h"
#include "variable.h"
#include "TseElinks.h"
#include "AMIVfr.h"
#if MINISETUP_MOUSE_SUPPORT
#include "Include/Protocol/MouseProtocol.h"
extern DXE_MOUSE_PROTOCOL *TSEMouse;
#endif
#include <setupdata.h>
#include <AutoId.h> // for MAIN_MAIN

// Build time file generated from AMITSE_OEM_HEADER_LIST elink.
#include "AMITSEOem.h"		

#ifndef EFI_DEFAULT_BMP_LOGO_GUID
#define EFI_DEFAULT_BMP_LOGO_GUID \
  {0x7BB28B99,0x61BB,0x11d5,0x9A,0x5D,0x00,0x90,0x27,0x3F,0xC1,0x4D}
#endif

typedef struct {
	UINT8 CtrlAction;
	CHAR16 UnicodeChar;
	UINT16 ScanCode;
	UINT32 KeyShiftState;
}	USER_CONTROL_KEY_DATA;

extern PAGE_LIST           *gPages;
extern UINT8         *gApplicationData;
extern PAGE_ID_LIST  *gPageIdList;

typedef struct _HII_FORM_ADDRESS
{
	EFI_GUID formsetGuid; // Required
	UINT16		formId;	// Required
	VOID *		Handle;	// Optional
}HII_FORM_ADDRESS;

#define NULL_GUID \
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }

HII_FORM_ADDRESS RootPageList[] = {	AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST { NULL_GUID, 0, NULL}, };
HII_FORM_ADDRESS HiddenPageList[] = { AMITSE_HIDDEN_PAGE_LIST { NULL_GUID, 0, NULL}, };
HII_FORM_ADDRESS RootPageOrder[] = { AMITSE_ROOT_PAGE_ORDER  };
EFI_GUID gSuppressDynamicFormsetList[] = { AMITSE_SUPPRESS_DYNAMIC_FORMSET_LIST NULL_GUID };
EFI_GUID gFormSetClassGuidList[] = {AMITSE_FILTER_CLASSGUID_FORMSETS, NULL_GUID};
VAR_DYNAMICPARSING_HANDLESUPPRESS gHandleSuppressVarList [] = {AMITSE_DYNAMICPARSING_HANDLE_SUPPRESS_LIST {NULL_GUID, ""}};

extern BOOLEAN
EfiCompareGuid (
  IN EFI_GUID *Guid1,
  IN EFI_GUID *Guid2
  );


extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;
extern BOOLEAN gEnterSetup;
extern BOOLEAN gEnterBoot;
extern BOOLEAN gQuietBoot;
extern STYLECOLORS  Colors;
extern VOID StyleOverRideComponent(VOID);
VOID ConfigPostScrollArea(VOID);
extern UINT32 gBootFlow;
extern EFI_GUID _gBootFlowGuid;
extern UINT16 gDbgPrint ;
///Assign the Exit Page options info. from Elink...
EXIT_PAGE_OPTIONS gExitPageOptions[] = {
	EXIT_PAGE_OPTIONS_LIST
};
extern BOOLEAN GifImageFlag;	//flag to denote the gif animation
VOID ContribBGRTTableToAcpi (BOOLEAN);
extern UINT8 gAddBgrtResolutions;
BOOLEAN     CheckforHotKey (AMI_EFI_KEY_DATA);
VOID TSEStringReadLoopEntryHook(VOID);
BOOLEAN TSEMouseIgnoreMouseActionHook(VOID);
VOID StopClickEvent(VOID);
VOID TSEStringReadLoopExitHook(VOID);
VOID MouseStop(VOID);
VOID MouseRefresh(VOID);
VOID SwitchToPostScreenHook(VOID);

VOID UpdateGoPUgaDraw (VOID);
extern BOOLEAN   IsWaitForKeyEventSupport (VOID);
BOOLEAN IsSetupPrintEvalMessage();

#ifndef KEY_STATE_EXPOSED
#define KEY_STATE_EXPOSED   0x40
#endif
static BOOLEAN gPostScreenMsg = FALSE;

BOOLEAN gIsRootPageOrderPresent = FALSE;
//UINT16 *gRootPageOrder;
HII_FORM_ADDRESS *gRootPageOrder;
UINT16 gRootPageOrderIndex = 0;
UINTN	CurrentScreenresolutionX, CurrentScreenresolutionY;
extern UINTN gPostStatus;
extern BOOLEAN gOsRecoverySupported;
extern BOOLEAN gPlatformRecoverySupported;
extern BOOLEAN IsTSEGopNotificationSupport();

extern VOID DecrementDynamicPageCount(UINT16 PageClass);
extern BOOLEAN FormBrowserHandleValid();
/**
    This function is the generic implementation of
    drawing quiet boot logo. This function is available
    as an ELINK.

    @param VOID

    @retval VOID

**/
EFI_STATUS SetNativeResFromEdid (VOID);
VOID DrawQuietBootLogo(VOID)
{
    EFI_STATUS 	Status;
    BOOLEAN 	LogoToDo = TRUE;
    EFI_OEM_BADGING_PROTOCOL *Badging = NULL;
    UINT32 		Instance = 0;
    EFI_BADGING_FORMAT Format;
    UINT8 		*ImageData = NULL;
    UINTN 		ImageSize = 0;
    EFI_BADGING_DISPLAY_ATTRIBUTE Attribute;
    INTN 		CoordinateX = 0;
    INTN 		CoordinateY = 0;
    EFI_GUID 	LogoFile = EFI_DEFAULT_BMP_LOGO_GUID;
    UINTN 		Width, Height;
    BOOLEAN 	AdjustSize = TRUE;
	UINTN		NoOfHandles = 0;
	EFI_HANDLE 	*Handles;
	UINTN		HandleIndex = 0;
	BOOLEAN		AddBgrtToAcpi = TRUE;
    CHAR16 *EvalText = NULL;
	
	TRACE((-1,"\n[TSE] DrawQuietBootLogo() Entering :\n"));
	// Draw the Logo
#if TSE_SUPPORT_NATIVE_RESOLUTION
	SetNativeResFromEdid ();		
#endif
	Status = GetGraphicsBitMapFromFV( &LogoFile, (VOID**)&ImageData, &ImageSize );
	TRACE((-1,"\n[TSE] GetGraphicsBitMapFromFV : Status = %r  ImageData = %x ImageSize = %d\n",Status,ImageData,ImageSize));
	MouseStop();
  	if ( !EFI_ERROR(Status) )
	{
        //No need for initializing CoordinateX and CoordinateY
        //because image will be cenetered and they will be ignored
        //anyways
        Attribute = EfiBadgingDisplayAttributeCenter;
		gAddBgrtResolutions = 1;
        Status = DrawImage( 
                        ImageData,
                        ImageSize,
                        (CO_ORD_ATTRIBUTE)Attribute,
                        CoordinateX,
                        CoordinateY,
                        AdjustSize,
                        &Width,
                        &Height
                        );
        TRACE((-1, "\n[TSE] DrawImage() : Status = %r \n",Status));
        AdjustSize = FALSE;
		if (EFI_SUCCESS != Status)
		{
			AddBgrtToAcpi = FALSE;			//Not Adding BGRT table to ACPI
		}
		
        if ( !EFI_ERROR(Status) && IsTSEGopNotificationSupport() )
            SaveGraphicsScreen();
        
        //In case of gif animation not to clear the image data
        if (!GifImageFlag)
	        MemFreePointer((VOID **)&ImageData);
	}

    //Locate Badging protocol Handles
	Status = gBS->LocateHandleBuffer(ByProtocol,&gEfiOEMBadgingProtocolGuid,NULL,&NoOfHandles,&Handles);
	TRACE((-1,"\n[TSE] LocateHandleBuffer(gEfiOEMBadgingProtocolGuid) : Status = %r NoOfHandles = %d\n",Status,NoOfHandles));
	if (!EFI_ERROR (Status))
	{
		if (!gAddBgrtResolutions)		//to support oem logo module. If logo.ffs not present from TSE then this wiil set the gAddBgrtResolutions.
		{
			gAddBgrtResolutions = 1;
		}
		TRACE((-1,"\n[TSE] gAddBgrtResolutions = %d\n",gAddBgrtResolutions));

		// for each badging protocol
		for(HandleIndex=0; HandleIndex<NoOfHandles;HandleIndex++)
		{
			Badging = NULL;
			Instance = 0;
			gBS->HandleProtocol(Handles[HandleIndex],&gEfiOEMBadgingProtocolGuid,(VOID**)&Badging);
			if(Badging != NULL)
				LogoToDo = TRUE;

		    //Draw all Logo obtained from Badging protocol or FV
			while ( LogoToDo )
		    {
	            Status = Badging->GetImage (
	                            Badging,
	                            &Instance,
	                            &Format,
	                            &ImageData,
	                            &ImageSize,
	                            &Attribute,
	                            (UINTN*)&CoordinateX,
	                            (UINTN*)&CoordinateY
	                            );
	            if(EFI_ERROR (Status))
	            {
	                LogoToDo = FALSE;
	                break;
	            }
		        Status = DrawImage( 
		                        ImageData,
		                        ImageSize,
		                        (CO_ORD_ATTRIBUTE)Attribute,
		                        CoordinateX,
		                        CoordinateY,
		                        AdjustSize,
		                        &Width,
		                        &Height
		                        );
				if (EFI_SUCCESS != Status)			//Not Adding BGRT table to ACPI
				{
					AddBgrtToAcpi = FALSE;
				}
		        AdjustSize = FALSE;
		
		        if ( !EFI_ERROR(Status) && IsTSEGopNotificationSupport() )
		            SaveGraphicsScreen();
		        
		        //In case of gif animation not to clear the image data
		        if(!GifImageFlag)
		            MemFreePointer((VOID **)&ImageData);	        
			}
	  	}
		if(NoOfHandles)
			gBS->FreePool(Handles);
	}
	gAddBgrtResolutions = 0;
	if (TRUE == AddBgrtToAcpi)
	{
		ContribBGRTTableToAcpi (GifImageFlag);
	}
	MouseRefresh();
	GetScreenResolution(&CurrentScreenresolutionX, &CurrentScreenresolutionY);
	TRACE((-1,"\n[TSE] CurrentScreenresolutionX = %d  CurrentScreenresolutionY = %d \n",CurrentScreenresolutionX,CurrentScreenresolutionY));
	TRACE(( -1, "\n[TSE] DrawQuietBootLogo() Exiting :\n"));

	if (IsSetupPrintEvalMessage())
    {
        EvalText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_EVAL_MSG));
        if ( EvalText != NULL ){
            EFI_GUID       AmiPostManagerProtocolGuid =  AMI_POST_MANAGER_PROTOCOL_GUID;
            AMI_POST_MANAGER_PROTOCOL    *AmiPostMgr = NULL;
            EFI_UGA_PIXEL ForeGround = {0xFF,0xFF,0xFF,0};
            EFI_UGA_PIXEL BackGround = {0,0,0,0};
                
            Status = gBS->LocateProtocol( (EFI_GUID *)&AmiPostManagerProtocolGuid,
                                                             (void*)NULL,
                                                             (void**)&AmiPostMgr );
            
            if (!EFI_ERROR (Status)) {
                AmiPostMgr->DisplayQuietBootMessage(EvalText, 0, 0, CA_AttributeLeftTop,
                                                        ForeGround, BackGround
                                                        );
            }
        }
        MemFreePointer((VOID **) &EvalText);  
    }
}

/**
    This function is a hook called when TSE determines
    that console is available. This function is available
    as ELINK. In the generic implementation boot password
    is prompted in this function.

    @param VOID

    @retval BOOLEAN. Should return TRUE if the screen was used to
        ask password; FALSE if the screen was not used to ask
        password.

**/
extern EFI_STATUS InitEsaTseInterfaces (void);	
extern VOID MouseInit(VOID);

BOOLEAN ProcessConInAvailability(VOID)
{
	UINTN NoOfRetries;
	UINT32 PasswordInstalled = AMI_PASSWORD_NONE;
	UINTN Index;
	EFI_INPUT_KEY Key;
	BOOLEAN bScreenUsed = FALSE;
	 UINT32	PasswordType = 0;
	 EFI_STATUS Status = EFI_SUCCESS;
	 EFI_TPL OldTpl;
#if SETUP_PRINT_EVAL_MSG || SETUP_PRINT_ENTER_SETUP_MSG
	CHAR16 *text = NULL;
#endif
	if (!gPostScreenMsg)							//ProcessConInAvailability needs protection against getting called twice
	{
#if SETUP_PRINT_EVAL_MSG
    //Print evaluation message here
    	text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_EVAL_MSG));
    	if ( text != NULL )
			PostManagerDisplayPostMessage(text);
		MemFreePointer( (VOID **)&text );
#endif
#if SETUP_PRINT_ENTER_SETUP_MSG
		text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_DEL_ENTER_SETUP) );
		if ( text != NULL )
			PostManagerDisplayPostMessage(text);
		MemFreePointer( (VOID **)&text );
#endif
		gPostScreenMsg = TRUE;
	}
	// if gPasswordType is not AMI_PASSWORD_NONE, Already got the Password.
	// Don't Ask for Password if it is already entered. 
	if ( gPasswordType == AMI_PASSWORD_NONE )
	{	
		PasswordInstalled = PasswordCheckInstalled();
		NoOfRetries = 3;
	
		if(CheckSystemPasswordPolicy(PasswordInstalled))
		{
			  bScreenUsed = TRUE;
		  
			  Status =	InitEsaTseInterfaces ();
			  
			  
			  
#if SETUP_GIF_LOGO_SUPPORT
			  GifImageFlag = FALSE;
#endif
			  					
			  if (!EFI_ERROR (Status))
				{
					PasswordType = gEsaInterfaceForTSE->CheckSystemPassword (AMI_PASSWORD_NONE, &NoOfRetries, NULL);
					 gPasswordType = PasswordType; // setting the gPasswordType in Esa Boot Only.
					 MouseInit();
				}
				else{
					PasswordType = CheckSystemPassword( AMI_PASSWORD_NONE, &NoOfRetries, NULL);
				}
				if(AMI_PASSWORD_NONE == PasswordType)
				{
					
					OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
				    gBS->RestoreTPL (TPL_APPLICATION);
								  
					while(1)
					{
						//Patch
						//Ctl-Alt-Del is not recognized by core unless a
						//ReadKeyStroke is issued
						gBS->WaitForEvent( 1, &(gST->ConIn->WaitForKey), &Index );
						gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
					}
					
					gBS->RaiseTPL (TPL_HIGH_LEVEL);
					gBS->RestoreTPL (OldTpl);
				}
				
							  
        }
    }
    return bScreenUsed;
}

/**
    This function is a hook called when TSE determines
    that SETUP utility has to be displayed. This function
    is available as ELINK. In the generic implementation
    setup password is prompted in this function.

    @param VOID

    @retval VOID

**/
VOID ProcessEnterSetup(VOID)
{
    UINTN NoOfRetries;
    UINT32 PasswordInstalled = AMI_PASSWORD_NONE;
    UINTN Index;
    EFI_INPUT_KEY Key;
    EFI_TPL OldTpl;
    
    if ( gPasswordType == AMI_PASSWORD_NONE )
    {

      	PasswordInstalled = PasswordCheckInstalled();

        if ( !( PasswordInstalled & AMI_PASSWORD_ADMIN ) )
        {
            SetPasswordType( AMI_PASSWORD_ADMIN );
        }
        else
        {
        	   EFI_STATUS Status = EFI_SUCCESS;
        	   UINT32		PasswordType = 0;
            NoOfRetries = 3;

            MouseStop ();              //Stopping before clearing the screen
            CleanUpLogo();
            MouseRefresh (); 
         
            Status =	InitEsaTseInterfaces ();
            
           
            
            if (!EFI_ERROR (Status))
      	    {
#if SMCPKG_SUPPORT
      	    		PasswordType = gEsaInterfaceForTSE->CheckSystemPassword (AMI_PASSWORD_NONE, &NoOfRetries, NULL);
#else
      	    		PasswordType = gEsaInterfaceForTSE->CheckSystemPassword (AMI_PASSWORD_USER, &NoOfRetries, NULL);
#endif
					gPasswordType = PasswordType; // setting the gPasswordType in Esa Boot Only.
      	    }
      	    else
      	    {
#if SMCPKG_SUPPORT
      	    	PasswordType = CheckSystemPassword (AMI_PASSWORD_NONE, &NoOfRetries, NULL);
#else
      	    	PasswordType = CheckSystemPassword (AMI_PASSWORD_USER, &NoOfRetries, NULL);
#endif
      	    }
            if(AMI_PASSWORD_NONE == PasswordType)
            {
			
				OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
				gBS->RestoreTPL (TPL_APPLICATION);
				 
                while(1)
                {
                    //Patch
                    //Ctl-Alt-Del is not recognized by core unless a
                    //ReadKeyStroke is issued
                    gBS->WaitForEvent( 1, &(gST->ConIn->WaitForKey), &Index );
                    gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
                }
				
				gBS->RaiseTPL (TPL_HIGH_LEVEL);
            	gBS->RestoreTPL (OldTpl);
                        
            }
            
        }
    }
}

/**
    This function is a hook called when TSE determines
    that it has to load the boot options in the boot
    order. This function is available as ELINK. OEM
    may decide to prompt for boot password in this
    function.

    @param VOID

    @retval VOID

**/
VOID ProcessProceedToBoot(VOID)
{
}

///Control key handling using Elinks...

USER_CONTROL_KEY_DATA	gUserCtrlKeyData[]= { CONTROL_KEY_MAP_LIST };

/**
    Function to validate the selected key based the key data.

    @param UserCtrlKeyData: User initialized data for the key 
					Key: Key provided by SimpleTextIn protocol
					KeyCodeProtocolSupport: flag to verify KeyShiftState.

    @retval TRUE/FALSE

**/
BOOLEAN GetKeySelection(USER_CONTROL_KEY_DATA UserCtrlKeyData, AMI_EFI_KEY_DATA AmiKey, BOOLEAN KeyCodeProtocolSupport)
{
	if (KeyCodeProtocolSupport)
	{ 
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
		/// validate the key including KeyShiftState.
		if ((TSE_CHECK_SHIFTSTATE(AmiKey.KeyState.KeyShiftState, UserCtrlKeyData.KeyShiftState) )&&
			(AmiKey.Key.UnicodeChar == UserCtrlKeyData.UnicodeChar ) && 
			(AmiKey.Key.ScanCode == UserCtrlKeyData.ScanCode ) )
		{
			return 1;
		}
#endif
	}
	/// validate the key without KeyShiftState
	else if( (AmiKey.Key.UnicodeChar == UserCtrlKeyData.UnicodeChar ) && 
		 		(AmiKey.Key.ScanCode == UserCtrlKeyData.ScanCode ) )
	{
		return 1;		
	}
	return 0;	
}

/**
    This function is a hook called inside setup utility
    to determine the action to be taken for a particular
    key press. This function is available as ELINK. OEMs
    may choose to have a different action or a different
    key. With this function OEMs can change key mappings
    for the controls in the setup utility. Post hot keys
    and Setup utility hot keys are out of this functions
    scope.

    @param Key: Key provided by SimpleTextIn protocol

    @retval CONTROL_ACTION: enumeration defined in commonoem.h.
        Input EFI_INPUT_KEY has to mapped to one of the
        enumerations in CONTROL_ACTION.

**/
CONTROL_ACTION MapControlKeys(AMI_EFI_KEY_DATA key)
{
	UINT8 CtrlCnt=0;
	UINT32 UserCtrlKeyCount=0;
	UserCtrlKeyCount = ( sizeof(gUserCtrlKeyData) / sizeof(USER_CONTROL_KEY_DATA));

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
	///To get the selected key data with shiftstate using Keycode protocol. 
	for( CtrlCnt=0; CtrlCnt<UserCtrlKeyCount; CtrlCnt++ )
	{
		if( GetKeySelection (gUserCtrlKeyData[CtrlCnt], key, 1) )		
		{
	        return gUserCtrlKeyData[CtrlCnt].CtrlAction;
		}
	}
#else
    ///To get the selected key data without shiftstate 
	for( CtrlCnt=0; CtrlCnt<UserCtrlKeyCount; CtrlCnt++ )
	{
		if( GetKeySelection (gUserCtrlKeyData[CtrlCnt], key, 0) )		
		{
	        return gUserCtrlKeyData[CtrlCnt].CtrlAction;
		}
	}
#endif    
	
 	if( CharIsAlpha(key.Key.UnicodeChar) ) 
        return ControlActionAlpha;
	if( CharIsNumeric(key.Key.UnicodeChar) )
        return ControlActionNumeric;

    return ControlActionUnknown;
}

/**
    Adds OSIndication support

    @param VOID

    @retval VOID

**/
VOID SupportOSIndication (VOID)
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	EFI_GUID 	EfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
	UINT64 		OsIndicationsSupported = EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
	UINT64 		OsIndications = 0;
	UINTN 		DataSize = sizeof (UINT64);
	UINT32 		Attributes = 0;
	UINT32      OsIndicationsSupportAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
	UINT32 		BootFlow = BOOT_FLOW_CONDITION_FIRST_BOOT;
	EFI_GUID 	BootFlowGuid = BOOT_FLOW_VARIABLE_GUID;


	Status = pRS->GetVariable (L"OsIndicationsSupported",
						&EfiGlobalVariableGuid,
						&(OsIndicationsSupportAttributes),
						&DataSize,
						(VOID *)&OsIndicationsSupported);
	if(!EFI_ERROR (Status) && DataSize)
	{
		OsIndicationsSupported = OsIndicationsSupported|EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
	}
	Status = pRS->SetVariable (L"OsIndicationsSupported", &EfiGlobalVariableGuid, OsIndicationsSupportAttributes, DataSize, (VOID *)&OsIndicationsSupported);
	if (!EFI_ERROR (Status))
	{
		DataSize = sizeof (UINT64);
		Status = pRS->GetVariable (L"OsIndications", &EfiGlobalVariableGuid, &Attributes, &DataSize, (VOID *)&OsIndications);
		if (!EFI_ERROR (Status))
		{
			if(OsIndications & EFI_OS_INDICATIONS_START_OS_RECOVERY)
			{
				gOsRecoverySupported=TRUE;
			}
			if(OsIndications & EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY)
			{
				gPlatformRecoverySupported=TRUE;
			}
			if (OsIndications & EFI_OS_INDICATIONS_BOOT_TO_FW_UI)
			{
				Status = pRS->SetVariable (L"BootFlow", 
								&BootFlowGuid, 
								EFI_VARIABLE_BOOTSERVICE_ACCESS,
								sizeof (BootFlow),
								&BootFlow);
				OsIndications = OsIndications & (~EFI_OS_INDICATIONS_BOOT_TO_FW_UI);
				Status = pRS->SetVariable (L"OsIndications", &EfiGlobalVariableGuid, Attributes, DataSize, (VOID *)&OsIndications);
			}
		}
	}
}

#if TSE_BUILD_AS_APPLICATION
extern UINTN 		gArgCount;
extern 	CHAR16 	**gArgv;
UINTN atoi_base (CHAR16 *string, UINT8 base);
#endif

/**
    This function is a hook called at the end of TSE
    driver entry. This function is available as ELINK.
    In the generic implementation TSE installs Key
    monitoring protocol. OEMs may choose to do additional
    logic here.

    @param VOID

    @retval VOID

**/
VOID MinisetupDriverEntryHook(VOID)
{
#if TSE_DEBUG_MESSAGES
    EFI_STATUS 	Status;
    EFI_GUID    guidDbgPrint = TSE_DEBUG_MESSAGES_GUID; 
    UINTN dbgVarSize = sizeof(gDbgPrint); 
    UINT16 FeatureBit ;
    
#endif

#if SETUP_SUPPORT_KEY_MONITORING
    InstallKeyMonProtocol();
#endif
	StyleOverRideComponent();
	ConfigPostScrollArea();

#if TSE_DEBUG_MESSAGES
           
    Status = pRS->GetVariable(
		L"TseDebugPrint",
		&guidDbgPrint,
		NULL,
		&dbgVarSize,
		&FeatureBit) ;

    if(!EFI_ERROR( Status ) )
		gDbgPrint = FeatureBit ;
#if TSE_BUILD_AS_APPLICATION
    if (1 == gArgCount)
    {
	    gDbgPrint = 0;
    }
    else
    {
	    gDbgPrint = (UINT16)atoi_base (gArgv [1], 10);
    }
#endif    
#endif
#if OSIndication_SUPPORT
	SupportOSIndication ();
#endif
}

/**
    This function is a hook called at the begining of the
    PostManagerHandShake protocol function. This function
    is available as ELINK. In the generic implementation
    TSE gets keys from key monitor filter. OEMs may choose
    to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID PostManagerHandShakeHook(VOID)
{
#if SETUP_SUPPORT_KEY_MONITORING
    GetKeysFromKeyMonFilter();
#endif
}


/**
    Displays STR_ACK_BBS_POPUP message in screen	

    @param VOID

    @retval VOID

**/
VOID PrintEnterBBSPopupMessage ()
{
    if(gEnterBoot != TRUE) // Print the "Entering Boot" message only once
    {
#if SETUP_PRINT_ENTER_BBSPOPUP_MSG
        CHAR16 *text = NULL;

        //Print Entering Boot Menu here
        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ACK_BBS_POPUP) );
        if ( text != NULL )
            PostManagerDisplayPostMessage(text);
        MemFreePointer( (VOID **)&text );
#endif
        gEnterBoot = TRUE;
    }
}

/**
    Displays STR_ACK_ENTER_SETUP message in screen	

    @param VOID

    @retval VOID

**/
void PrintEnterSetupMessage()
{
   	if(gEnterSetup != TRUE) // To print the "Entering setup" message only once
	{
#if SETUP_PRINT_ENTER_SETUP_MSG
		CHAR16 *text = NULL;

	        //Print Entering setup here
		text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ACK_ENTER_SETUP) );
		if ( text != NULL )
				PostManagerDisplayPostMessage(text);
	    		MemFreePointer( (VOID **)&text );
#endif
		gEnterSetup = TRUE;
	}
}

/**
    This function is  called to check and Deactivate the Softkbd

    @param VOID

    @retval VOID

**/
void CheckandDeactivateSoftkbd()
{
	 if(TSEMouseIgnoreMouseActionHook() == TRUE)
		TSEStringReadLoopExitHook();
}

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
BOOLEAN CheckOEMKey(AMI_EFI_KEY_DATA* AmiKey, UINT16 UnicodeCharSDL, UINT16 ScanCodeSDL,
                EFI_KEY EfiKeySDL, UINT32 ShiftStateSDL, EFI_KEY_TOGGLE_STATE ToggleStateSDL)
{
    BOOLEAN bRet = FALSE;
    BOOLEAN bKeyMatch = FALSE;
    BOOLEAN bShiftStateMatch = FALSE;
    BOOLEAN bToggleStateMatch = FALSE;
    
     if ( ( ((AmiKey->Key.UnicodeChar == UnicodeCharSDL)&&(UnicodeCharSDL != 0))
		|| ((AmiKey->Key.ScanCode == ScanCodeSDL)&&(ScanCodeSDL!=0)) )
        ^((AmiKey->EfiKey == EfiKeySDL)&&(EfiKeySDL != 0)) )    
        bKeyMatch = TRUE;

    //find if the shift key is valid and matching
    /*if (!(ShiftStateSDL & SHIFT_STATE_VALID))//we don't have to check
        bShiftStateMatch = TRUE;*/
    if (AmiKey->KeyState.KeyShiftState & SHIFT_STATE_VALID)
    {
        bShiftStateMatch = CheckAdvShiftState(AmiKey->KeyState.KeyShiftState, ShiftStateSDL); 
       /* if( ((AmiKey->KeyState.KeyShiftState &(~SHIFT_STATE_VALID)) & ShiftStateSDL)
            || ((AmiKey->KeyState.KeyShiftState == SHIFT_STATE_VALID) 
                  && (ShiftStateSDL == SHIFT_STATE_VALID)) )
            bShiftStateMatch = TRUE;*/
    }
    //find if the toggle key is valid and matching
    if (!(ToggleStateSDL & TOGGLE_STATE_VALID))//we don't have to check
        bToggleStateMatch = TRUE; 
    else if (AmiKey->KeyState.KeyToggleState & TOGGLE_STATE_VALID)
    {
        if( ((AmiKey->KeyState.KeyToggleState &(~TOGGLE_STATE_VALID)) & ToggleStateSDL)
            || ((AmiKey->KeyState.KeyToggleState == TOGGLE_STATE_VALID) 
                  && (ToggleStateSDL == TOGGLE_STATE_VALID)) )
            bToggleStateMatch = TRUE;
    }
    
    bRet = (bKeyMatch && bShiftStateMatch && bToggleStateMatch) ? TRUE : FALSE;

    return bRet;   
}
#else
BOOLEAN CheckOEMKey(AMI_EFI_KEY_DATA* AmiKey, UINT16 UnicodeCharSDL, UINT16 ScanCodeSDL)
{
    if ( ( AmiKey->Key.UnicodeChar == UnicodeCharSDL ) 
        && ( AmiKey->Key.ScanCode == ScanCodeSDL ) )
        return TRUE;
    else 
        return FALSE;
}
#endif
typedef BOOLEAN (OEM_KEY_CALLBACK_CHECK_FN) (  CHAR16 UnicodeChar, UINT16 ScanCode, UINT32 ShiftState, OUT VOID **OemKeyCallbackFn );

extern OEM_KEY_CALLBACK_CHECK_FN OEM_KEY_CALLBACK_FN EndOfOemCallbackList;
OEM_KEY_CALLBACK_CHECK_FN * OemKeyCheckFnList[] = {  OEM_KEY_CALLBACK_FN NULL };
extern BOOT_FLOW	*gBootFlowTable;

/**
    This function to check OEMKey is consumed by any of the modules that
    has child elink to OEM_KEY_CALLBACK_LIST. If it wants to handle the key
    It can return success and callback funciton. The callback function will be 
    called in Bootflowentry.

    @param AMI_EFI_KEY_DATA

    @retval BOOLEAN 

**/
BOOLEAN CheckOEMKeyCallback(AMI_EFI_KEY_DATA* AmiKey)
{
	UINT32 ShiftState = 0;
	VOID *CallbackFunction=NULL;
	UINTN i;
	BOOT_FLOW *bootFlowPtr;

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
	ShiftState = AmiKey->KeyState.KeyShiftState;
#endif
	for( i=0;OemKeyCheckFnList[i];i++)
	{
		if(OemKeyCheckFnList[i](AmiKey->Key.UnicodeChar, AmiKey->Key.ScanCode, ShiftState, &CallbackFunction))
		{
			// OEMKey Claimed 
			if(CallbackFunction != NULL)
			{
				bootFlowPtr = gBootFlowTable;
				for ( bootFlowPtr = gBootFlowTable;
					bootFlowPtr->Condition != BOOT_FLOW_CONDITION_NULL; bootFlowPtr++ )
				{
					if(bootFlowPtr->Condition == BOOT_FLOW_CONDITION_OEM_KEY_CALLBACK)
					{
						// Update the OEM Key Callback function.
						bootFlowPtr->CallbackFunction = (BOOT_FLOW_CALLBACK)(UINTN)CallbackFunction;
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}




/**
    This function is a hook called to perform specific
    action for a POST hot key. This function is called
    periodically. This function is available as ELINK.

    @param Event: Timer event.
              Context: Event context; always NULL

    @retval VOID

**/
VOID CheckForKey (EFI_EVENT Event, VOID *Context)
{
	EFI_STATUS Status;
    AMI_EFI_KEY_DATA AmiKey;

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
	AMI_EFI_KEY_DATA KeyFlush;
	EFI_STATUS StatusFlush;
    AMI_EFIKEYCODE_PROTOCOL *pKeyCodeProtocol;

    Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gAmiEfiKeycodeProtocolGuid, (void*)&pKeyCodeProtocol);
        if (EFI_ERROR(Status)) return;
#endif  

	if ( gST->ConIn == NULL )
		return;

    if ( gEnterSetup || gBootFlow )
        return;
	do
	{
#if SETUP_SUPPORT_KEY_MONITORING
        if(gLegacyKeys)
        {
            if(gLegacyKeys & SETUP_ENTRY_KEY_BIT)
                PrintEnterSetupMessage();
#if SETUP_BBS_POPUP_ENABLE
            else if (gLegacyKeys & POPUP_ENTRY_KEY_BIT) {
                PrintEnterBBSPopupMessage ();
                gBootFlow = BOOT_FLOW_CONDITION_BBS_POPUP;
            }
#endif
#if SETUP_OEM_KEY1_ENABLE
            else if (gLegacyKeys & OEM_KEY1_BIT)
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY1;
#endif
#if SETUP_OEM_KEY2_ENABLE
            else if (gLegacyKeys & OEM_KEY2_BIT)
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY2;
#endif
#if SETUP_OEM_KEY3_ENABLE
            else if (gLegacyKeys & OEM_KEY3_BIT)
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY3;
#endif
#if SETUP_OEM_KEY4_ENABLE
            else if (gLegacyKeys & OEM_KEY4_BIT)
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY4;
#endif
         }
#endif

		if ( IsWaitForKeyEventSupport() )
		{
			//
			// Check whether the keystroke is present in the system with the help of boot-service in the higher TPL at which 
			// console driver's 'WaitForKey' event's  notification function is registered . 
			// If keystroke is not present , do not do anything at the lower TPL at which code is running
			//
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL	
			Status = gBS->CheckEvent(pKeyCodeProtocol->WaitForKeyEx);	
#else
			Status = gBS->CheckEvent(gST->ConIn->WaitForKey);
#endif
			if (Status == EFI_NOT_READY) 
				return;
		}
		
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL	
		Status = pKeyCodeProtocol->ReadEfikey( pKeyCodeProtocol, &AmiKey );

        // If it Partial Key make the Status as Error to ignore 
        // the Partial Key.
		if((AmiKey.KeyState.KeyToggleState & KEY_STATE_EXPOSED) ==  KEY_STATE_EXPOSED)			// Ignoring Partial keys
			if( (!EFI_ERROR( Status )) && (AmiKey.Key.ScanCode==0) && (AmiKey.Key.UnicodeChar==0))
				Status = EFI_NOT_READY;

		if ( ! EFI_ERROR( Status ) )
		{
			do
			{
				if ( IsWaitForKeyEventSupport() )
				{
					if ( EFI_NOT_READY == gBS->CheckEvent(pKeyCodeProtocol->WaitForKeyEx) )
					{
						break;
					}
				}
				StatusFlush = pKeyCodeProtocol->ReadEfikey( pKeyCodeProtocol, &KeyFlush );
                // If it Partial Key make the Status as Error to ignore 
                // the Partial Key.
				if((KeyFlush.KeyState.KeyToggleState & KEY_STATE_EXPOSED) ==  KEY_STATE_EXPOSED)			//Ignoring Partial keys
					if( (!EFI_ERROR( Status )) && (KeyFlush.Key.ScanCode==0) && (KeyFlush.Key.UnicodeChar==0))
						break;
			} while ( ! EFI_ERROR( StatusFlush ) );
	}

#else
         Status = gST->ConIn->ReadKeyStroke( gST->ConIn, &AmiKey.Key );
#endif

		if ( ! EFI_ERROR( Status ))
		{
			if(
				( (SETUP_ENTRY_UNICODE	== AmiKey.Key.UnicodeChar)
				&&(SETUP_ENTRY_SCAN	== AmiKey.Key.ScanCode)
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
				&& (TSE_CHECK_SHIFTSTATE(AmiKey.KeyState.KeyShiftState, SETUP_ENTRY_SHIFT_STATE))
#endif
				)
#if SETUP_ALT_ENTRY_ENABLE 
				|| ( (SETUP_ALT_ENTRY_UNICODE == AmiKey.Key.UnicodeChar)
				&&  (SETUP_ALT_ENTRY_SCAN == AmiKey.Key.ScanCode)
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL				
				&& (TSE_CHECK_SHIFTSTATE(AmiKey.KeyState.KeyShiftState, SETUP_ALT_ENTRY_SHIFT_STATE))
#endif	    
				)
#endif   
	           )

			{
				CheckandDeactivateSoftkbd();
                PrintEnterSetupMessage();
			}
#if SETUP_BBS_POPUP_ENABLE
	        else if ( (AmiKey.Key.UnicodeChar == POPUP_MENU_ENTRY_UNICODE) 
             && (AmiKey.Key.ScanCode == POPUP_MENU_ENTRY_SCAN)
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#ifdef POPUP_MENU_ENTRY_SHIFT_STATE
			 && (TSE_CHECK_SHIFTSTATE (AmiKey.KeyState.KeyShiftState, POPUP_MENU_ENTRY_SHIFT_STATE))
#endif
#endif
			)
			{
				CheckandDeactivateSoftkbd();
				PrintEnterBBSPopupMessage ();
				gBootFlow = BOOT_FLOW_CONDITION_BBS_POPUP;
			}
#endif
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#if SETUP_OEM_KEY1_ENABLE
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY1_UNICODE, SETUP_OEM_KEY1_SCAN,
                     SETUP_OEM_KEY1_EFIKEY, SETUP_OEM_KEY1_SHIFT, SETUP_OEM_KEY1_TOGGLE))
   				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY1;
#endif

#if SETUP_OEM_KEY2_ENABLE
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY2_UNICODE, SETUP_OEM_KEY2_SCAN,
                     SETUP_OEM_KEY2_EFIKEY, SETUP_OEM_KEY2_SHIFT, SETUP_OEM_KEY2_TOGGLE))
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY2;
#endif

#if SETUP_OEM_KEY3_ENABLE
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY3_UNICODE, SETUP_OEM_KEY3_SCAN,
                     SETUP_OEM_KEY3_EFIKEY, SETUP_OEM_KEY3_SHIFT, SETUP_OEM_KEY3_TOGGLE))
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY3;
#endif

#if SETUP_OEM_KEY4_ENABLE
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY4_UNICODE, SETUP_OEM_KEY4_SCAN,
                     SETUP_OEM_KEY4_EFIKEY, SETUP_OEM_KEY4_SHIFT, SETUP_OEM_KEY4_TOGGLE))
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY4;
#endif

#else // if no TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#if SETUP_OEM_KEY1_ENABLE
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY1_UNICODE, SETUP_OEM_KEY1_SCAN))
    				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY1;
#endif

#if SETUP_OEM_KEY2_ENABLE
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY2_UNICODE, SETUP_OEM_KEY2_SCAN))
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY2;
#endif

#if SETUP_OEM_KEY3_ENABLE 
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY3_UNICODE, SETUP_OEM_KEY3_SCAN))
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY3;
#endif

#if SETUP_OEM_KEY4_ENABLE
			else if (CheckOEMKey(&AmiKey, SETUP_OEM_KEY4_UNICODE, SETUP_OEM_KEY4_SCAN))
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY4;
#endif
#endif
			else if (CheckOEMKeyCallback(&AmiKey) )
				gBootFlow = BOOT_FLOW_CONDITION_OEM_KEY_CALLBACK;
#ifndef STANDALONE_APPLICATION
			else if ( ( AmiKey.Key.UnicodeChar == SETUP_TOGGLE_KEY_UNICODE ) 
                && ( AmiKey.Key.ScanCode == SETUP_TOGGLE_KEY_SCANCODE) )
			{
				if ( gQuietBoot )
				{
					gQuietBoot = FALSE;
					MouseStop();//EIP 62763 : Stopping the mouse before cleanuplogo when TAB key pressed
					UpdateGoPUgaDraw ();		//In legacy option rom launch GOP will be removed so updating here else it will crash
					CleanUpLogo();
					SwitchToPostScreenHook();//EIP-111415 SwitchToPostScreenHook
					InitPostScreen();
				}
			}
#endif
#if EFI_SPECIFICATION_VERSION>0x20000
			else if (CheckforHotKey (AmiKey))
			{
				gBootFlow = BOOT_FLOW_HOTKEY_BOOT;
			} 
#endif
		}
	} while ( ! EFI_ERROR( Status ) );


}

/**
    This function is a hook called to perform specific
    action for a POST hot key. This function is called
    periodically. This function is available as ELINK.

    @param Event: Timer event.
              Context: Event context; always NULL

    @retval VOID

**/
VOID CheckForClick (EFI_EVENT Event, VOID *Context)
{
#if MINISETUP_MOUSE_SUPPORT
	INT32 Button_Status=0;
	
	if(TSEMouseIgnoreMouseActionHook())
	{
		return;
	}
     if(TSEMouse!=NULL)
     {
	    TSEMouse->MousePoll(TSEMouse);
	  	TSEMouse->GetButtonStatus(TSEMouse,&Button_Status);

		if (TSE_POST_STATUS_PROCEED_TO_BOOT == gPostStatus)
			StopClickEvent();

		if(((Button_Status == TSEMOUSE_RIGHT_CLICK)||(Button_Status == TSEMOUSE_LEFT_DCLICK)||(Button_Status == TSEMOUSE_LEFT_CLICK))&&(!TSEMouseIgnoreMouseActionHook()))
		{
			TSEStringReadLoopEntryHook();
			StopClickEvent();
		}

     }
#endif

}

/**
    This function is a hook called before launching
    legacy boot option. This function is available as
    ELINK. In the generic implementation this function is
    empty. OEMs may choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID BeforeLegacyBootLaunch(VOID)
{
}

/**
    This function is a hook called after launching
    legacy boot option. This function is available as
    ELINK. In the generic implementation this function is
    empty. OEMs may choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID AfterLegacyBootLaunch(VOID)
{
}
VOID MouseDestroy(VOID);
/**
    This function is a hook called before launching EFI
    boot option. This function is available as ELINK. In
    the generic implementation this function is empty.
    OEMs may choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID BeforeEfiBootLaunch(VOID)
{
	StopClickEvent();
	MouseDestroy();

}

/**
    This function is a hook called after launching EFI
    boot option. This function is available as ELINK. In
    the generic implementation this function is empty.
    OEMs may choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID AfterEfiBootLaunch(VOID)
{
}

/**
    This function is a hook called after setup utility
    saves changes based on user input. This function is
    available as ELINK. In the generic implementation
    this function is empty. OEMs may choose to do
    additional logic here.

    @param VOID

    @retval VOID

**/
VOID SavedConfigChanges(VOID)
{
}

/**
    This function is a hook called after setup utility
    loaded config defaults based on user input. This
    function is available as ELINK. In the generic
    implementation this function is empty. OEMs may
    choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID LoadedConfigDefaults(VOID)
{
}

/**
    This function is a hook called in every iteration
    (not every second) while TSE is waiting for POST time
    out. This function is available as ELINK. In the generic
    implementation this function is empty. OEMs may
    choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID TimeOutLoopHook(VOID)
{
}

VOID HiiGetEfiKey(CHAR16 *PwKey);


//PasswordEncode function moved to PasswordEncode.c

/**
    This function is a hook called when colour of a
    control has to be decided. This function is available
    as ELINK. OEMs may choose to use different colour
    combination.

    @param BGColor: Back ground colour
              FGColor: Fore ground colour
              SecBGColor: Secondary back ground colour used for
              time, date and menu
              SecFGColor: Secondary fore ground colour used for
              time, date and menu
              SelBGColor: Selected back ground colour
              SelFGColor: Selected fore ground colour
              NSelBGColor: Not selected back ground colour
              NSelFGColor: Not selected fore ground colour
              LabelBGColor: Label back ground colour
              LabelFGColor: Label fore ground colour
              NSelLabelFGColor: Not selected label fore ground
              EditBGColor: Edit box back ground
              EditFGColor: Edit box fore ground
              PopupFGColor: Popup fore ground
              PopupBGColor: Popup back ground

    @retval EFI_STATUS: always EFI_SUCCESS

**/
EFI_STATUS SetControlColors(UINT8 *BGColor, UINT8 *FGColor, UINT8 *SecBGColor, UINT8 *SecFGColor, 
								 UINT8 *SelBGColor, UINT8 *SelFGColor, UINT8 *NSelBGColor, UINT8 *NSelFGColor,
								 UINT8 *LabelBGColor, UINT8 *LabelFGColor,UINT8 *NSelLabelFGColor, UINT8 *EditBGColor, UINT8 *EditFGColor,
								 UINT8 *PopupFGColor, UINT8 *PopupBGColor)
{
	if(FGColor !=NULL)	*FGColor = Colors.FGColor ;
	if(BGColor !=NULL)	*BGColor = Colors.BGColor ;	

	if(SecFGColor !=NULL)	*SecFGColor = Colors.SecFGColor ;
	if(SecBGColor !=NULL)	*SecBGColor = Colors.SecBGColor ;	

	if(SelFGColor !=NULL)	*SelFGColor = Colors.SelFGColor ;
	if(SelBGColor !=NULL) 	*SelBGColor = Colors.SelBGColor ;

	if(NSelFGColor !=NULL)	*NSelFGColor = Colors.NSelFGColor ;
	if(NSelBGColor !=NULL) 	*NSelBGColor = Colors.NSelBGColor ;

	if(LabelFGColor !=NULL)	*LabelFGColor = Colors.LabelFGColor ;
	if(LabelBGColor !=NULL)	*LabelBGColor = Colors.LabelBGColor ;
	
	if(NSelLabelFGColor !=NULL)	*NSelLabelFGColor = Colors.NSelLabelFGColor ;

	if(EditFGColor !=NULL)	*EditFGColor = Colors.EditFGColor ;
	if(EditBGColor !=NULL)	*EditBGColor = Colors.EditBGColor ;

	if(PopupFGColor !=NULL)	*PopupFGColor = Colors.PopupFGColor ;
	if(PopupBGColor !=NULL)	*PopupBGColor = Colors.PopupBGColor ;

	return EFI_SUCCESS;
}

/**
    This function is the hook call the appropriate InvalidActions function.
    OEMs may choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID InvalidActionHook(VOID)
{
	/// Call the Invalid action function (Ex: Beep )
}

/**
    This function is a hook called after setup utility
    loaded user defaults based on user input. This
    function is available as ELINK. In the generic
    implementation this function is empty. OEMs may
    choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID LoadedUserDefaults(VOID)
{
}

/**
    This function is a hook called after setup utility
    loaded Oem defaults based on user input. This
    function is available as ELINK. In the generic
    implementation this function is empty. OEMs may
    choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID LoadedBuildDefaults(VOID)
{
}

/**
    This function is a hook called after setup utility
    loaded previously saved values based on user input. This
    function is available as ELINK. In the generic
    implementation this function is empty. OEMs may
    choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID LoadedPreviousValues(VOID)
{
	//UpdateControlStatus(TRUE);
}

/**
    This function is a hook called after some control 
    modified in the setup utility by user. This
    function is available as ELINK. In the generic
    implementation this function is empty. OEMs may
    choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID SetupConfigModified(VOID)
{
	//UpdateControlStatus(TRUE);
}

/**
    This function is a hook called when user loads the manufacturing
    or optimal defaults. This function is
    available as ELINK. OEMs may override the function and  
    decide the policy.

    @param defaults : (NVRAM_VARIABLE *)optimal or manufacturing
        data : Messagebox

    @retval VOID

**/
VOID _LoadDefaults( NVRAM_VARIABLE *defaults, UINTN data );
VOID LoadSetupDefaults (VOID *defaults, UINTN data )
{

	_LoadDefaults((NVRAM_VARIABLE *)defaults, data );
}

/**
    This function is a hook called after some control 
    modified in the setup utility by user. This
    function is available as ELINK. In the generic
    implementation this function is empty. OEMs may
    choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID PreSystemResetHook(VOID)
{
	///to have the oem customizations just before resetting
}

/**
    This function is a hook called when user activates
    configurable post hot key 1. This function is
    available as ELINK. Generic implementation is empty.
    OEMs may choose to use different logic here.

    @param bootFlowPtr: Boot flow entry that triggered this call

    @retval always EFI_SUCCESS

**/
EFI_STATUS	OemKey1Hook ( BOOT_FLOW *bootFlowPtr )
{
	EFI_STATUS Status = EFI_SUCCESS;
	//
	// Add code here on enabling SETUP_OEM_KEY1_ENABLE for callback

	return Status;
}

/**
    This function is a hook called when user activates
    configurable post hot key 2. This function is
    available as ELINK. Generic implementation is empty.
    OEMs may choose to use different logic here.

    @param bootFlowPtr: Boot flow entry that triggered this call

    @retval always EFI_SUCCESS

**/
EFI_STATUS	OemKey2Hook ( BOOT_FLOW *bootFlowPtr )
{
	EFI_STATUS Status = EFI_SUCCESS;
	//
	// Add code here on enabling SETUP_OEM_KEY2_ENABLE for callback

	return Status;
}

/**
    This function is a hook called when user activates
    configurable post hot key 3. This function is
    available as ELINK. Generic implementation is empty.
    OEMs may choose to use different logic here.

    @param bootFlowPtr: Boot flow entry that triggered this call

    @retval always EFI_SUCCESS

**/
EFI_STATUS	OemKey3Hook ( BOOT_FLOW *bootFlowPtr )
{
	EFI_STATUS Status = EFI_SUCCESS;
	//
	// Add code here on enabling SETUP_OEM_KEY3_ENABLE for callback

	return Status;
}

/**
    This function is a hook called when user activates
    configurable post hot key 4. This function is
    available as ELINK. Generic implementation is empty.
    OEMs may choose to use different logic here.

    @param bootFlowPtr: Boot flow entry that triggered this call

    @retval always EFI_SUCCESS

**/
EFI_STATUS	OemKey4Hook ( BOOT_FLOW *bootFlowPtr )
{
	EFI_STATUS Status = EFI_SUCCESS;
	//
	// Add code here on enabling SETUP_OEM_KEY4_ENABLE for callback

	return Status;
}

/**
    This function is a hook called inside setup utility
    to determine the action to be taken for a particular
    Mouse Click. This function is available as ELINK. OEMs
    may choose to have a different action or a different
    mouse click. 

    @param pMouseInfo 

    @retval CONTROL_ACTION: enumeration defined in commonoem.h.
        Input EFI_INPUT_KEY has to mapped to one of the
        enumerations in CONTROL_ACTION.

**/ 
CONTROL_ACTION MapControlMouseAction(VOID *pTempInfo)
{
    MOUSE_INFO *pMouseInfo = (MOUSE_INFO *)pTempInfo ;
    
    if(pMouseInfo!=NULL)
	{
	    if(TSEMOUSE_RIGHT_CLICK == pMouseInfo->ButtonStatus)
			return ControlActionAbort;
#if SINGLE_CLICK_ACTIVATION
		if(TSEMOUSE_LEFT_DCLICK == pMouseInfo->ButtonStatus || TSEMOUSE_LEFT_CLICK == pMouseInfo->ButtonStatus)
			return ControlActionSelect;
#else
		if(TSEMOUSE_LEFT_CLICK == pMouseInfo->ButtonStatus)
			return ControlActionChoose;

		if(TSEMOUSE_LEFT_DCLICK == pMouseInfo->ButtonStatus)
			return ControlActionSelect;
#endif
		if(TSEMOUSE_LEFT_DOWN == pMouseInfo->ButtonStatus)		
			return ControlMouseActionLeftDown;

		if(TSEMOUSE_LEFT_UP == pMouseInfo->ButtonStatus)		
			return ControlMouseActionLeftUp;

	}
    return ControlActionUnknown;
}

/**
    This function is a hook called when Boots from 
    BBS popup or Bootoverride page. 

    @param VOID

    @retval VOID

**/
VOID ProcessProceedToBootNow(VOID)
{
}
/**
    This function is a hook called when TSE determines
    that console out is available. This function is available
    as ELINK. 

    @param VOID

    @retval VOID

**/
VOID ProcessConOutAvailable(VOID)
{
}

/**
    This function Fixes the SubPage as root pages from
    AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST

    @param VOID

    @retval VOID

**/
VOID FixSubPageAsRootPageList()
{
	UINTN RootPageListCount = sizeof(RootPageList) / sizeof (HII_FORM_ADDRESS);
	UINTN i,j;
	PAGE_INFO *info;
	PAGE_ID_INFO *pageIdInfo;
	// Support to Handle some child pages as root pages.
	for(j=0; j<RootPageListCount;j++)
	{
	    for ( i = 0; i < (UINTN)gPages->PageCount; i++ ) 
		{
			info = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[i]);

			// To Make Page as root
			if( info->PageFormID == RootPageList[j].formId)
			{
				// If Handle is not null compare. If it null Ignore
				if(RootPageList[j].Handle != NULL)
				{
					if(info->PageHandle != RootPageList[j].Handle)
						continue;
				}
				pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[info->PageIdIndex]);

				if( EfiCompareGuid(&pageIdInfo->PageGuid,&RootPageList[j].formsetGuid) )//Compare Guid
					info->PageParentID =0;
			}
		}
	}
}

/**
    This function Fixes the Hides pages from
    AMITSE_HIDDEN_PAGE_LIST

    @param VOID

    @retval VOID

**/
VOID FixHiddenPageList(VOID)
{
	UINTN HiddenPageListCount = sizeof(HiddenPageList) / sizeof (HII_FORM_ADDRESS);
	UINTN i,j;
	PAGE_INFO *info;
	PAGE_ID_INFO *pageIdInfo;
	// To Hide Pages
	for(j=0; j<HiddenPageListCount;j++)
	{
		for ( i = 0; i < (UINTN)gPages->PageCount; i++ ) 
		{
			info = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[i]);

			if( info->PageFormID == HiddenPageList[j].formId)
			{
				// If Handle is not null compare. If it null Ignore
				if(RootPageList[j].Handle != NULL)
				{
					if(info->PageHandle != HiddenPageList[j].Handle)
						continue;
				}
				pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[info->PageIdIndex]);

				if( EfiCompareGuid(&pageIdInfo->PageGuid,&HiddenPageList[j].formsetGuid) )//Compare Guid
				{
					if(info->PageFlags.PageDynamic) // Decrement DynamicPageCount if already added in it
						DecrementDynamicPageCount(pageIdInfo->PageClass);
					
                   info->PageFlags.PageVisible = TRUE;
                   /* This is needed as the PageDynamic is true if the IFR page  is added dynamically. Then it will be shown under Dynamic pages. By making PageDynamic as FALSE we can make sure it is not shown anywere. */
                   info->PageFlags.PageDynamic = FALSE;
				}
			}
		}
	}
}

VOID TseUpdateRootPageOrder(VOID);
/**
    This function is a hook called just before creating
    the setup application for oem customizations. This function is
    available as ELINK. OEMs may choose to do additional logic here. 

    @param VOID

    @retval VOID

**/
VOID ProcessUIInitHook(VOID)
{
	CheckandDeactivateSoftkbd ();
#if AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST_SUPPORT
	FixSubPageAsRootPageList ();
#endif
#if AMITSE_HIDE_ROOT_PAGE
	FixHiddenPageList ();
#endif
	TseUpdateRootPageOrder();
}

/**
    This function is a hook called After post screen is initilzed.
    This function is available as ELINK. In the generic implementation this function
    is empty. OEMs may choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID AfterInitPostScreen(VOID)
{

}
/**
    This function is to update the Root page order from the specified elink.

    @param VOID

    @retval VOID

**/
VOID TseUpdateRootPageOrder(VOID)
{

#if AMITSE_ROOT_PAGE_ORDER_SUPPORT

	UINTN RootPageOrderCount = sizeof(RootPageOrder) / sizeof (HII_FORM_ADDRESS);
	UINTN j,k;
	EFI_GUID gSetupPageOrderGuid = SETUP_ROOT_PAGE_ORDER_GUID, NullGuid = NULL_GUID;
	UINT32 *currentBootFlow;
	UINTN size = 0;
	UINT32 BootFlow = BOOT_FLOW_CONDITION_NORMAL;

	//gBootFlow is changing constantly so better get it from NVRAM and proceed
	currentBootFlow = VarGetNvramName( L"BootFlow", &_gBootFlowGuid, NULL, &size );
	if (NULL != currentBootFlow)
	{
		BootFlow = *currentBootFlow;
		MemFreePointer ((VOID **)&currentBootFlow);
	}

	if((1 == RootPageOrderCount)&&(EfiCompareGuid(&NullGuid,&RootPageOrder[0].formsetGuid)))
		return;

	if(gRootPageOrder!=NULL)
		return;

	gRootPageOrder = EfiLibAllocateZeroPool (RootPageOrderCount * sizeof (HII_FORM_ADDRESS));
	if (NULL == gRootPageOrder)
		return;
	// To Hide Pages
	
	
	for(j=0; j<RootPageOrderCount;j++)
	{
		if ( (EfiCompareGuid (&gSetupPageOrderGuid,&RootPageOrder[j].formsetGuid)) && (RootPageOrder[j].formId == BootFlow) )
		{
			k=++j;
			while (!( (EfiCompareGuid (&gSetupPageOrderGuid,&RootPageOrder[k].formsetGuid)) || (EfiCompareGuid(&NullGuid,&RootPageOrder[k].formsetGuid)) ) )
			{
				gBS->CopyMem (&gRootPageOrder[gRootPageOrderIndex], &RootPageOrder[k], sizeof (HII_FORM_ADDRESS));
				gRootPageOrderIndex ++;
				gIsRootPageOrderPresent = TRUE;
				k ++;
			}
			break;
		}
	}
	//If root page ordering is not found for any bootflow then trying for BOOT_FLOW_CONDITION_NORMAL bootflow. Helpful for BOOT_FLOW_CONDITION_OEM_KEY1 etc.,,
	if ( (FALSE == gIsRootPageOrderPresent) && (BOOT_FLOW_CONDITION_NORMAL != BootFlow) )
	{
		for(j=0; j<RootPageOrderCount;j++)
		{
			if ( (EfiCompareGuid (&gSetupPageOrderGuid,&RootPageOrder[j].formsetGuid)) && (RootPageOrder[j].formId == BOOT_FLOW_CONDITION_NORMAL) )
			{
				k=++j;
				while (!( (EfiCompareGuid (&gSetupPageOrderGuid,&RootPageOrder[k].formsetGuid)) || (EfiCompareGuid(&NullGuid,&RootPageOrder[k].formsetGuid)) ) )
				{
					gBS->CopyMem (&gRootPageOrder[gRootPageOrderIndex], &RootPageOrder[k], sizeof (HII_FORM_ADDRESS));
					gRootPageOrderIndex ++;
					gIsRootPageOrderPresent = TRUE;
					k ++;
				}
				break;
			}
		}
	}
#endif
}
/**
    This function test for SubPage as root pages from
    AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST

    @param info 

    @retval BOOLEAN

**/
BOOLEAN isSubPageRootPage(PAGE_INFO *info)
{
    UINTN RootPageListCount = sizeof(RootPageList) / sizeof (HII_FORM_ADDRESS);
    UINTN j;
    PAGE_ID_INFO *pageIdInfo;
    EFI_GUID NullGuid = NULL_GUID;
    // Support to Handle some child pages as root pages.
    for(j=0; j<RootPageListCount;j++)
    {
        if( info->PageFormID == RootPageList[j].formId)
        {
            // If Handle is not null compare. If it null Ignore
            if(RootPageList[j].Handle != NULL)
            {
                if(info->PageHandle != RootPageList[j].Handle)
                    continue;
            }
            
            pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[info->PageIdIndex]);
                        
            if( EfiCompareGuid(&pageIdInfo->PageGuid,&RootPageList[j].formsetGuid) )//Compare Guid
            {
               return TRUE ;
            }
        }
    }
    return FALSE ;
}
/**
    This function returns the number of SubPage as root pages from
    AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST.

    @param VOID

    @retval UINTN Number of SubPages as root pages

**/
UINTN GetSubPageAsRootPageCount(VOID)
{
    UINTN RootPageListCount = 0 ;  

    RootPageListCount = sizeof(RootPageList) / sizeof (HII_FORM_ADDRESS);
    
    return RootPageListCount ;
}
/**
    This function is a hook called during SwitchToPostScreen post.
    This function is available as ELINK. In the generic implementation this function
    is empty. OEMs may choose to do additional logic here.

    @param VOID

    @retval VOID

**/
VOID SwitchToPostScreen(VOID)
{

}

/**
    This function filter formset and display formset based on ClassGUID mentioned in elink AMITSE_FILTER_CLASSGUID_FORMSETS 

    @param EFI_IFR_FORMSET. Formset to compare with elink classGuid.

    @retval BOOLEAN. Return TRUE formset matches with elink ClassGuid else FALSE

**/
BOOLEAN ShowClassGuidFormsets (TSE_EFI_IFR_FORM_SET *FormSet )
{
#if SHOW_FORMSETS_WITH_CLASSGUID
	UINT16 FilterListcount = sizeof(gFormSetClassGuidList) / sizeof (EFI_GUID);
	UINT16 index = 0;

	if(FormBrowserHandleValid())
		return TRUE; // don't filter formset based on Calssguid for Sendform's Formset.
	
	if (FormSet->ClassGuid)
	{
		for (index = 0; index < (FilterListcount-1); index++)//(FilterListcount-1) To avoid check with NULL guid
		{
			if ( EfiCompareGuid ( FormSet->ClassGuid, &gFormSetClassGuidList[index])  )
				return TRUE;
		}
		return FALSE;
	}
#endif
	return TRUE;//if token is disable
}


/**
    This function suppress dynamic formset mentioned in the elink AMITSE_SUPPRESS_DYNAMIC_FORMSET_LIST

    @param GUID. Formset guid to compare with gSuppressDynamicFormsetList elink guids.

    @retval BOOLEAN. Return TRUE if any guid matches with gSuppressDynamicFormsetList elink else FALSE

**/
BOOLEAN HideDynamicFormsets (EFI_GUID *FormSetGuid )
{
#if AMITSE_SUPPRESS_DYNAMIC_FORMSET
	UINTN suppressDynamicFormsetListCount = sizeof(gSuppressDynamicFormsetList) / sizeof (EFI_GUID);
	UINT16 index = 0;

	if (FormSetGuid)
	{
		for (index = 0; index < (suppressDynamicFormsetListCount-1); index++)
		{
			if ( EfiCompareGuid ( FormSetGuid, &gSuppressDynamicFormsetList[index])  )
				return TRUE;
		}
	}
#endif
	return FALSE;
}

/**
    This function informs to match the handle or not for creating new variable using VAR_DYNAMICPARSING_HANDLESUPPRESS_LIST ELink

    @param EFI_GUID *, CHAR16	*

    @retval BOOLEAN. Return TRUE handle matches with elink else FALSE

**/
CHAR8* StrDup16to8(CHAR16 *String);
INTN EfiStrCmp (IN CHAR16   *String, IN CHAR16   *String2);
BOOLEAN VariableHandleSuppressed (EFI_GUID *VariableGuid, CHAR16	*VariableName)
{
#if SUPPRESS_HANDLE_FOR_VAR_CREATION	
	UINTN HandleSuppressListCount = sizeof (gHandleSuppressVarList)/sizeof (VAR_DYNAMICPARSING_HANDLESUPPRESS);
	UINTN i = 0;
	CHAR8 *VariableNameStr8 = (CHAR8 *)NULL;
	
	VariableNameStr8 = StrDup16to8 (VariableName);
	if (NULL == VariableNameStr8)
	{
		return FALSE;
	}
	for(i=0; i < HandleSuppressListCount; i++)
	{
		if ( (EfiCompareGuid(VariableGuid, &(gHandleSuppressVarList [i].VariableGuid))) && (Strcmp (VariableNameStr8, gHandleSuppressVarList [i].VariableName) ==0) )
		{
			return TRUE;
		}
	}
#endif	
	return FALSE;
}
BOOLEAN IsTseBuild()
{
    BOOLEAN   TseBuildStatus = TRUE;
#ifndef BUILD_FOR_ESA
    TseBuildStatus = TRUE;
#else
    TseBuildStatus = FALSE;
#endif
    return TseBuildStatus;
}


BOOLEAN DefaultEntryStatus()
{
	BOOLEAN   ErrorInEntryStatus = TRUE;
	
	//IsTseBuild() can be used , but it will add both code
#ifndef BUILD_FOR_ESA	
		ErrorInEntryStatus = FALSE;
#else
		ErrorInEntryStatus = TRUE;
#endif
	return ErrorInEntryStatus;
}

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
