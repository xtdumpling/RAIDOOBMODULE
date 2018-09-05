//**********************************************************************
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
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/FormBrowser2.c $
//
// $Author: Arunsb $
//
// $Revision: 27 $
//
// $Date: 9/20/12 11:15a $
//
//*****************************************************************//
/** @file FormBrowser2.c

**/
//*************************************************************************

//----------------------------------------------------------------------------
#include "minisetup.h"
#include "FormBrowser2.h"
#include "TseUefiHii.h"
#include "Token.h"
//----------------------------------------------------------------------------
extern BOOLEAN gPackUpdatePending;
extern BOOLEAN gEnableDrvNotification; //TRUE if allow notification function to process action, FALSE to ignore the notification
extern UINTN HpkFileCount;

extern DYNAMIC_PAGE_GROUP *gDynamicPageGroup;
extern REFRESH_ID_INFO 	*gRefreshIdInfo;
extern VOID SaveSetupData(TSE_SETUP_GLOBAL_DATA *Data);
extern VOID RestoreSetupData(TSE_SETUP_GLOBAL_DATA *Data);
extern VOID RestoreTseMouseInSoftkbd();
VOID SaveSetupGobalDataHook(VOID);
VOID RestoreSetupGobalDataHook(VOID);
extern BOOLEAN IsTSEBuild();
//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------
BOOLEAN gRequireSysReboot = FALSE;
VOID    **gSfHandles;
UINTN   gSfHandleCount;
EFI_GUID *gFSetGuid = NULL;
UINT16   gSfFormId = 0;
UINT8 *gSfNvMap;
extern BOOLEAN gBrowserCallbackEnabled; //Allow external drivers to change ASL cache only if it's TRUE; ignore browser callback otherwise
 extern BOOLEAN gEnterSetup;
extern EFI_EVENT gKeyTimer;
EFI_BROWSER_ACTION gBrowserCallbackAction = 0 ;//Contains the BrowserCallback action when a callback is in progress.
//static CONTROL_INFO *gCallbackControl = NULL; unused
static EFI_HANDLE gCurrVarHandle = (EFI_HANDLE)NULL;
static UINT32 gCurrVarIndex = 0;
#define BROWSERCALLBACK_MAX_ENTRIES 10
static EFI_HANDLE  gCallBackHandleStack[BROWSERCALLBACK_MAX_ENTRIES]; 
static UINT32 gCallBackVarIndexStack[BROWSERCALLBACK_MAX_ENTRIES];
BOOLEAN gHiiFormNotFound = FALSE;
//EFI_GUID gEfiFormBrowser2ProtocolGuid = EFI_FORM_BROWSER2_PROTOCOL_GUID;

TSE_SETUP_GLOBAL_DATA *CurrentBuffer;

EFI_STATUS SaveTseSetupGlobalData();
EFI_STATUS RestoreTseSetupGlobalData();
VOID RestoreRefreshIdEvents();
VOID StopRefreshIdEvent();
VOID PushGlobalBuffer (TSE_SETUP_GLOBAL_DATA *NewBuffer);
VOID PopGlobalBuffer(TSE_SETUP_GLOBAL_DATA** Buffer);
BOOLEAN IsGlobalBufferStackEmpty();
VOID RestoreVariablesInNVRAM();

//---------------------------------------------------------------------------
// Structure
//---------------------------------------------------------------------------
EFI_FORM_BROWSER2_PROTOCOL FormBrowser2 =
{
  SendForm,
  BrowserCallback
};

//---------------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------------
EFI_STATUS _StorageToConfigResp(VARIABLE_INFO *VariableInfo, VOID *Buffer, CHAR16 *ConfigHdr, CHAR16 **ConfigResp);
EFI_STATUS _ConfigRespToStorage(VARIABLE_INFO *varInfo, UINT8 **buffer, UINTN *Size, EFI_STRING configResp);
BOOLEAN EDKVersion_1_05_RetrieveData (VOID);
EFI_STATUS CleanNotificationQueue (VOID);
VOID SaveGraphicsScreen(VOID);
VOID RestoreGraphicsScreen (VOID);
//---------------------------------------------------------------------------
// Function Implementation
//---------------------------------------------------------------------------

/**
    Set the variable inforamtion

    @param VarHandle Variable handle to process
    @param VarIndex Variable index in the gVariables->VariableList

    @retval VOID
**/
VOID SetCallBackControlInfo(EFI_HANDLE VarHandle, UINT32 VarIndex)
{
    static int ContextVar=0;
    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] Entering SetCallBackControlInfo, HIIHandle =%x \n\n", VarHandle ); 
    if(VarHandle != NULL){ //If the varaible handle is valid, set handle and variable index
        if(ContextVar >= BROWSERCALLBACK_MAX_ENTRIES)
        {
            SETUP_DEBUG_TSE ("\n[TSE] Too many nested Browser Callbacks!\n\n");
            ASSERT (0);    
        }
        gCurrVarHandle = VarHandle;
        gCurrVarIndex = VarIndex;
        //Use Stack to manage gCurrVarHandle and gCurrVarIndex
        gCallBackHandleStack[ContextVar] = gCurrVarHandle;
        gCallBackVarIndexStack[ContextVar] = gCurrVarIndex;
        ContextVar++;
        
    } else{ //Else reset global the varaible handle and variable index
	
        if(ContextVar == 0)
            return;
                    
        ContextVar--;
        
        if(ContextVar == 0)
        {
            gCurrVarHandle = (EFI_HANDLE)NULL;
            gCurrVarIndex = 0;
        }else{
            //Reset gCurrVarHandle and gCurrVarIndex to previous stack entry
            gCurrVarHandle = gCallBackHandleStack[ContextVar-1] ;
            gCurrVarIndex = gCallBackVarIndexStack[ContextVar-1] ;
        }
            
    }
    
    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] Exiting SetCallBackControlInfo HIIHandle =%x \n\n", VarHandle ); 
}

/**
    Register ReadyToBoot event to notify DefaultsEvaluateExpression().

    @param VOID 

    @retval EFI_STATUS
**/
EFI_STATUS CreateReadyToBootEventForEvaluateDefault()
{
	static EFI_EVENT ReadyToBootEvent = NULL;	
	EFI_STATUS Status = EFI_UNSUPPORTED;
	void 	*defaults4FirstBootVar = NULL;
	UINTN 		varSize = 0;

	defaults4FirstBootVar = (VOID *)VarGetNvramName (L"EvaluateDefaults4FirstBoot", &gEvaluateDefaults4FirstBootGuid, NULL, &varSize);
	
	if((defaults4FirstBootVar == NULL) && (ReadyToBootEvent == NULL))
	{
		Status = TseEfiCreateEventReadyToBoot(
	                EFI_TPL_CALLBACK,
	                DefaultsEvaluateExpression,
	                NULL,
	                &ReadyToBootEvent
	                );
	}
	else
	{
		MemFreePointer((VOID**)&defaults4FirstBootVar);
	}

	return Status;
}

/**
    Install FormBrowser Protocol

    @param Handle 

    @retval EFI_STATUS
**/
EFI_STATUS InstallFormBrowserProtocol(EFI_HANDLE Handle)
{
  EFI_STATUS status = EFI_SUCCESS;
 
  if(IsDefaultConditionalExpression())
	CreateReadyToBootEventForEvaluateDefault();

  UnInstallFormBrowserProtocol(Handle);
  status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiFormBrowser2ProtocolGuid,
                  &FormBrowser2,
                  NULL
                  );

  return status;
}

/**
    UnInstall FormBrowser Protocol

    @param Handle 

    @retval VOID
**/
VOID UnInstallFormBrowserProtocol(EFI_HANDLE Handle)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_HANDLE handle;
  VOID *iface;
  UINTN size = 0;
  
  pBS = gBS;
  status = gBS->LocateHandle (
                    ByProtocol,
                    &gEfiFormBrowser2ProtocolGuid,
                    NULL,
                    &size,
                    &handle
                    );
  if(status == EFI_BUFFER_TOO_SMALL)
  {
    handle = EfiLibAllocateZeroPool(size);
    if (handle == NULL)
    {
      status = EFI_OUT_OF_RESOURCES;
      goto DONE;
    }

    status = gBS->LocateHandle (
                      ByProtocol,
                      &gEfiFormBrowser2ProtocolGuid,
                      NULL,
                      &size,
                      &handle
                      );
  }

  if(EFI_ERROR(status))
  {
    goto DONE;
  }
  status = gBS->LocateProtocol (
                    &gEfiFormBrowser2ProtocolGuid,
                    NULL,
                    &iface
                    );
  if(EFI_ERROR(status))
  {
    goto DONE;
  }
  status = gBS->UninstallMultipleProtocolInterfaces (
                  handle,
                  &gEfiFormBrowser2ProtocolGuid,
                  iface,
                  NULL
                  );

  if(EFI_ERROR(status))
  {
    goto DONE;
  }

DONE:
  return;
}

/**
    This is the routine which an external caller uses to direct
    the browser where to obtain it's information.

    @param This The Form Browser protocol instanse.
    @param Handles A pointer to an array of Handles. If
        HandleCount > 1 we display a list of the formsets for the
        handles specified.
    @param HandleCount The number of Handles specified in Handle.
    @param FormSetGuid [OPTIONAL] - This field points to the EFI_GUID
        which must match the Guid field in the EFI_IFR_FORM_SET
    @param op code for the specified forms-based package. If
        FormSetGuid is NULL, then this function will display the
        first found forms package.
    @param FormId [OPTIONAL] - This field specifies which EFI_IFR_FORM
        to render as the first displayable page. If this field has
        a value of 0x0000, then the forms browser will render the
        specified forms in their encoded order.
    @param ScreenDimenions [OPTIONAL] - Points to
        recommended form dimensions, including any non-content
        area, in characters. This allows the browser to be called
        so that it occupies a portion of the physical screen
        instead of dynamically determining the screen dimensions.
    @param ActionRequest [OPTIONAL] - Points to
        the action recommended by the form.

        EFI_STATUS status - EFI_SUCCESS, The function completed successfully.
    @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
    @retval EFI_NOT_FOUND No valid forms could be found to display.
**/
EFI_STATUS
EFIAPI
SendForm (
  IN  CONST EFI_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                   *Handles,
  IN  UINTN                            HandleCount,
  IN  EFI_GUID                         *FormSetGuid, OPTIONAL
  IN  UINT16                           FormId, OPTIONAL
  IN  CONST EFI_SCREEN_DESCRIPTOR      *ScreenDimensions, OPTIONAL
  OUT EFI_BROWSER_ACTION_REQUEST       *ActionRequest  OPTIONAL
  )
{
	EFI_STATUS Status = EFI_SUCCESS,GlobalBufferStatus = EFI_SUCCESS;
	UINTN i;
	BOOLEAN BackUpgEnterSetup = FALSE;
	UINTN MaxCols = 0,MaxRows = 0;
	SCREEN_BUFFER *ActBuf=NULL,*FluBuf=NULL;
	UINT32 CurrentGopMode=0,CurrentTextMode=0; 
	UINT8 Sel=0;
	AMI_POST_MANAGER_PROTOCOL 	*PostMgr = NULL;	
	BOOLEAN SendFormBeforeSetup = TRUE; 
	EFI_UGA_PIXEL *PostscreenwithlogoBackup = NULL;
	UINTN PostStatusBackup = TSE_POST_STATUS_BEFORE_POST_SCREEN;

#if !SETUP_FORM_BROWSER_SUPPORT
	SETUP_DEBUG_TSE ("[TSE] SendForm not supported with SETUP_FORM_BROWSER_SUPPORT token disabled\n");
	return EFI_UNSUPPORTED;
#endif
	SETUP_DEBUG_TSE("[TSE] SendForm() Entering\n");
    UpdateGoPUgaDraw();
	if( Handles == NULL || HandleCount == 0 )
	{
		Status = EFI_INVALID_PARAMETER;
		goto DONE;
	}

	if(gApp != NULL || gSetupContextActive)
	{
		SETUP_DEBUG_TSE("[TSE] SendForm() called inside setup...\n");
		// Inside Setup. Setupdata contexts valid.
		if( IsSetupFormBrowserNestedSendFormSupport() )		
		{	
			GlobalBufferStatus = SaveTseSetupGlobalData();
			if( EFI_ERROR ( GlobalBufferStatus ) )
				goto NOT_SUPPORTED;
			
			Status = InitApplicationData( gImageHandle );
			if( EFI_ERROR ( Status ) )
			{
				TSE_SETUP_GLOBAL_DATA *temp = NULL;
				
				do
				{
					PopGlobalBuffer(&temp);
					MemFreePointer((VOID**)&temp);
				}while(temp);
				
				goto NOT_SUPPORTED;
			}
			InitGlobalPointers();
			RTIfrUnRegFormNotificationWrapper();
			SendFormBeforeSetup = FALSE;
		}
		else
		{
NOT_SUPPORTED:
			Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &PostMgr);
			if(Status == EFI_SUCCESS)
			{
				//PostManagerDisplayMsgBox (L"SendForm Unsupported",L"SendForm Not suppored Nestedly or from Valid Setup Context!",MSGBOX_TYPE_OK,&Sel);
				PostMgr->DisplayMsgBox(  L"SendForm Unsupported",  L"SendForm Not suppored Nestedly or from Valid Setup Context!", MSGBOX_TYPE_OK,NULL);
			}
			return EFI_UNSUPPORTED;
		}
	}
	
	/*
	 * For Updating the Global pointers in ESA side when sendform called before Setup 
	 * */
    if (!gApplicationData)
    {
        Status = InitApplicationData (gImageHandle);
        if (EFI_ERROR (Status))
        {
            return Status;
        }
        InitGlobalPointers();
    }
#ifndef STANDALONE_APPLICATION
#if !TSE_FOR_EDKII_SUPPORT
    //Skipping this case in ESA side to avoid redrawing the post once again,since gConsoleControl is not reflecting in ESA side
    if ( IsTSEBuild() && ! gConsoleControl )
    {
        //All necessary protocols are not available yet.
        //We can still proceed if Uga Draw is the only protocol not available yet.
        ActivateApplication();

			if (!IsDelayLogoTillInputSupported())	
				ActivateInput();
    }
#endif    
#endif

	//Initialize send form global variables
	if(HandleCount)
	{
		gSfHandles = EfiLibAllocateZeroPool( HandleCount * sizeof(VOID*));
		gSfHandleCount = 0;
        gFSetGuid = FormSetGuid;
        gSfFormId = FormId;
		for(i=0;i<HandleCount;i++)
		{
			UINT8 *FormSet = NULL;
			UINTN Length = 0;

			// Check if the Handle Has Forms to Display
			FormSet = HiiGetForm( Handles[i], 0, &Length);
			if( FormSet != NULL )
			{
				gSfHandles[gSfHandleCount] = (VOID*)(UINTN)Handles[i];
				MemFreePointer((void**) &FormSet );
				gSfHandleCount++;
			}
		}
		SETUP_DEBUG_TSE("[TSE] HandleCount = %d  gSfHandleCount = %d\n",HandleCount,gSfHandleCount);
		if( gSfHandleCount == 0 )
		{
			MemFreePointer( (VOID**)&gSfHandles );
			Status = EFI_NOT_FOUND;
			goto DONE;
		}
	}
	gSfNvMap = NULL;//NvMapOverride;

    if(!gVariableList)
	{
	    CleanTempNvramVariableList();// Reset to read the variable from NVRAM
        VarLoadVariables( (VOID **)&gVariableList, NULL );
	}
	// this *MUST* be run a EFI_TPL_APPLICATION
	gBS->RaiseTPL( EFI_TPL_HIGH_LEVEL );	// guarantees that RestoreTPL won't ASSERT
	gBS->RestoreTPL( EFI_TPL_APPLICATION );

	// Get BootOption() if Save&exit is called for SendForm, BootOrder variable will be updated.
	BootGetBootOptions();
	
#if TSE_FOR_EDKII_SUPPORT
	if (NULL == gLanguages)
	{
		BootGetLanguages();
	}
#endif
	StyleUpdateVersionString();

	//Show forms
	Status = gST->ConIn->Reset( gST->ConIn, FALSE );
	BackUpgEnterSetup = gEnterSetup;
	if (gKeyTimer)
	{
		TimerStopTimer( &gKeyTimer );
	}
	gHiiFormNotFound = FALSE;
	
	//
	// Save TSE graphics and Text Mode contexts
	//
	MaxCols = gMaxCols;
	MaxRows = gMaxRows;
	if(gGOP)
	{
		CurrentGopMode=gGOP->Mode->Mode;
	}
	if(gST && gST->ConOut)
	{
		CurrentTextMode = gST->ConOut->Mode->Mode;
	}
	SaveGraphicsScreen();
	if(SendFormBeforeSetup)
	{
		// Take backup of gPostscreenwithlogo and revert it to NULL since it is clearing inside Setup.
		PostscreenwithlogoBackup = gPostscreenwithlogo;
		gPostscreenwithlogo = NULL;
		//Taking backup of gPostStatus to avoid drawing Softkbd,ESC icon in POST if Softkbd enabled.
		PostStatusBackup = gPostStatus;
	}
	
	ActBuf = EfiLibAllocateZeroPool( sizeof(SCREEN_BUFFER) );
	FluBuf = EfiLibAllocateZeroPool( sizeof(SCREEN_BUFFER) );
	if ((NULL != ActBuf) && gActiveBuffer) //Taking Backup only if gActiveBuffer present.This will be NULL for ESA before setup
	{
		MemCopy( ActBuf, gActiveBuffer, sizeof(SCREEN_BUFFER) );
	}
	if((NULL != FluBuf) && gFlushBuffer)//Taking Backup only if gFlushBuffer present.This will be NULL for ESA before setup
	{
		MemCopy( FluBuf, gFlushBuffer, sizeof(SCREEN_BUFFER) );
	}
	
	Status = MainSetupLoopHook();//Modified MainSetupLoop as board module hook
	

	//
	// 1.  restore TSE text mode contexts. it is needed for TSE PostManager protocols
	//
	  gMaxCols = MaxCols;
	  gMaxRows = MaxRows;
	  
	if(gST && gST->ConOut && (CurrentTextMode != gST->ConOut->Mode->Mode)) 
	{
		gST->ConOut->SetMode(gST->ConOut, CurrentTextMode); 
	}
	if((NULL != ActBuf) && gActiveBuffer) //Restore only if gActiveBuffer present.This will be NULL for ESA before setup
	{
		MemCopy( gActiveBuffer, ActBuf, sizeof(SCREEN_BUFFER) );
		MemFreePointer((VOID *)&ActBuf);
	}
    if((NULL != FluBuf) && gFlushBuffer)//Restore only if gFlushBuffer present.This will be NULL for ESA before setup
	{
		MemSet(gFlushBuffer, sizeof(SCREEN_BUFFER), 0); // to redraw the entire screen
		MemFreePointer((VOID *)&FluBuf);
	}
	
	//
	// 2.  then restore TSE orignal graphics screen lively. it is needed for restoring logo or other OEM custome paintings etc
	//
	if(gGOP && (CurrentGopMode != gGOP->Mode->Mode)) 
	{
		gGOP->SetMode(gGOP,CurrentGopMode);
	}
	
	if(SendFormBeforeSetup)
	{
		if(PostscreenwithlogoBackup)
			gPostscreenwithlogo = PostscreenwithlogoBackup;
		
		gPostStatus = PostStatusBackup;
	}
	PostscreenwithlogoBackup = NULL;
	
	RestoreGraphicsScreen();

	gEnterSetup = BackUpgEnterSetup;

DONE:

	if(IsSetupFormBrowserNestedSendFormSupport() && gResetRequired && IsGlobalBufferStackEmpty())// If SendForm Called before TSE, Reset the system. otherwise it will be reset in Root TSE.
	{
		UINT8 ResetType;
		SETUP_DEBUG_TSE("[TSE] Resetting System in SendForm()\n");
		///Hook function to update setup before resetting the system
		PreSystemResetHookHook();	
		ResetType=TseGetResetType();
		gRT->ResetSystem( ResetType, EFI_SUCCESS, 0, NULL );
	}

	if(gRequireSysReboot){ // If system reboot was set, return ACTION_REQUEST_RESET
		if(ActionRequest)
			*ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
		gRequireSysReboot = FALSE; //Reset system reboot variable
	}
	if(gPackUpdatePending) //If IFR notifications are pending clear the queue before exiting SendForm
	{
		CleanNotificationQueue();
	}
	gPackUpdatePending = FALSE;
	CleanTempNvramVariableList();// Reset to read the variable from NVRAM
	if (TRUE == gHiiFormNotFound)			//If form is not found then return EFI_NOT_FOUND
	{
		Status = EFI_NOT_FOUND;
	}
	
	//Reset send form global variables
 	//clear SendFrom data moved here since CleanNotificationQueue required to know last SendForm's handles
	//gSfHandles = (VOID**)NULL;
	MemFreePointer( (VOID**)&gSfHandles );
	gSfHandles = NULL;
	gSfFormId = 0;
	gSfHandleCount = 0;
	gSfNvMap = NULL;
	
	if(IsSetupFormBrowserNestedSendFormSupport())
	{
		// Restore Global buffer from the stack if available.
		GlobalBufferStatus = RestoreTseSetupGlobalData();
		if(GlobalBufferStatus == EFI_SUCCESS)
		{
			RTIfrRegFormNotificationWrapper();
			// Process the pending pack update notification belongs to current context
			if(gEnableDrvNotification && !gEnableProcessPack)
			{
				SETUP_DEBUG_TSE("[TSE] Processing pack notification of Current context after returing from Sendform operation\n");
				gEnableProcessPack = TRUE;
				ProcessPackNotificationHook ();
				gEnableProcessPack = FALSE;
			}
		}
	}

	if(IsMouseSupported()&& IsSoftKbdSupported())// Re-Initializing the mouse when mouse and softkbd present
	{
		if(SendFormBeforeSetup)
		{
			RestoreTseMouseInSoftkbd();
			InstallClickHandlers (); 	//install click handlers in notification itself. In other places installing click handler is removed.
		}
		MouseInit();					//If mouse is notified then Initializing the mouse
		MouseRefresh();
	}
	SETUP_DEBUG_TSE("[TSE] SendForm() Exiting with Status = %r\n",Status);
	return Status;
}

/**
    This function is called by a callback handler to retrieve
    uncommitted state data from the browser.

           
    @param This A pointer to the
    @param instance .
    @param ResultsDataSize A pointer to the size of the
        buffer associated with ResultsData.
    @param ResultsData A string returned from an IFR
        browser or equivalent. The results string will have no
        routing information in them.
    @param RetrieveData A BOOLEAN field which allows an agent
        to retrieve (if RetrieveData = TRUE) data from the
        uncommitted browser state information or set 
        (if RetrieveData = FALSE) data in the uncommitted browser
        state information.
    @param VariableGuid OPTIONAL - An optional field to
        indicate the target variable GUID name to use.
    @param VariableName OPTIONAL - An optional field to
        indicate the target human-readable variable name.

    @retval EFI_STATUS status - EFI_SUCCESS, The results have been distributed
        or are awaiting distribution.
        EFI_BUFFER_TOO_SMALL, The ResultsDataSize
        specified was too small to contain the results data.
**/
EFI_STATUS
EFIAPI
BrowserCallback (
  IN CONST EFI_FORM_BROWSER2_PROTOCOL  *This,
  IN OUT UINTN                         *ResultsDataSize,
  IN OUT EFI_STRING                    ResultsData,
  IN BOOLEAN                           RetrieveData,
  IN CONST EFI_GUID                    *VariableGuid, OPTIONAL
  IN CONST CHAR16                      *VariableName  OPTIONAL
  )
{
  EFI_STATUS    status = EFI_SUCCESS;
//  CONTROL_INFO  *ctrlInfo = NULL; Unused
  VARIABLE_INFO *varInfo = NULL;
  BOOLEAN       found;
  CHAR16        *configResp = NULL;
  CHAR16        *strPtr = NULL;
  CHAR16        *configHdr = NULL;
  UINT8         *buffer = NULL;
  UINTN         bufferSize;
  UINTN         size = 0;
  UINTN         length = 0;
  UINT32         varIndex = 0;
  UINT32         index = 0;
  UINT16		CurrentBootOption = 0;

	if((gBrowserCallbackEnabled == FALSE) && (gEnableDrvNotification == FALSE)) { 	//Allow Browser callback for AMI ExecuteCallback/AccessConfig/RouteConfig calls
		status = EFI_UNSUPPORTED;
		goto DONE;
	}

	if(ResultsDataSize == NULL || (*ResultsDataSize && ResultsData == NULL))
	{
		status = EFI_INVALID_PARAMETER;
		goto DONE;
	}
	
	SETUP_DEBUG_UEFI_CALLBACK ( "[TSE] Entering BrowserCallback()\n\n" );

  //
  // Find target storage
  //
  if(VariableGuid != NULL)
  {
    //
    // Try to find target storage
    //
    found = FALSE;
    for(index = 0; index < gVariables->VariableCount; index++)
    {
		varInfo = (VARIABLE_INFO *)VarGetVariableInfoIndex(index);
		if(NULL == varInfo){
			continue;
		}
      if(EfiCompareGuid(&varInfo->VariableGuid, (EFI_GUID *)VariableGuid))
      {
        if((varInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE) != VARIABLE_ATTRIBUTE_NAMEVALUE)
        {
          //
          // Buffer storage require both GUID and Name
          //
          if (VariableName == NULL)
          {
            status = EFI_NOT_FOUND;
            goto DONE;
          }

          if (EfiStrCmp (varInfo->VariableName, (CHAR16 *)VariableName) == 0)
          {
				found = TRUE;
				varIndex = index;
				SETUP_DEBUG_UEFI_CALLBACK ( "[TSE] Comparing gCurrVarHandle = %x to variableHandle %x\n\n", gCurrVarHandle, varInfo->VariableHandle  );
				if (varInfo->VariableHandle == gCurrVarHandle)					//Checking for same variable names for two different handles, if match not found continue with default
				{
	            break;
				}
          }
        }
      }
    }

    if (!found)
    {
      status = EFI_NOT_FOUND;
      goto DONE;
    }
	else						//If gCurrVarHandle is not matched then varInfo will have last index data so again filling VarInfo
	{
		//Updating the varinfo with the varIndex
		varInfo = (VARIABLE_INFO *)VarGetVariableInfoIndex(varIndex);
	}	
  } else
  {
    // GUID/Name is not specified, store variable index
    varIndex = gCurrVarIndex;
    varInfo = (VARIABLE_INFO *)VarGetVariableInfoIndex(varIndex);
    if(varInfo)
    {
   	 SETUP_DEBUG_UEFI_CALLBACK ( "[TSE] Comparing gCurrVarHandle = %x to variableHandle %x\n\n", gCurrVarHandle, varInfo->VariableHandle );
   	 if(varInfo->VariableHandle != gCurrVarHandle)
   	 {
   		 status = EFI_NOT_FOUND;
   		 goto DONE;   		 
   	 }
    }
  }
	if (NULL == varInfo){
		goto DONE;
	}

  //
  //  Retrive NVRam Buffer
  //
  buffer = (UINT8 *)VarGetVariable(varIndex, &size);
  if(buffer == NULL)
  {
    goto DONE;
  }

  //
  // Generate <ConfigHdr>
  //
  SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] GetConfigHeader()\n" );  
  status = GetConfigHeader(varInfo, &configHdr, &length);
  SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] GetConfigHeader retuned,  status = 0x%x \n\n" , status );    
	if(EFI_ERROR(status) || (NULL == configHdr))
	{
		goto DONE;
	}

  if(RetrieveData)
  {
    //
    // Generate <ConfigResp>
    //
    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] _StorageToConfigResp()\n" );  
    status = _StorageToConfigResp(varInfo, buffer, configHdr, &configResp);
    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] _StorageToConfigResp returned,  status = 0x%x \n\n" , status ); 
    if (EFI_ERROR(status))
    {
       goto DONE;
    }

    //
    // Skip <ConfigHdr> and '&' to point to <ConfigBody>
    //
    if (EDKVersion_1_05_RetrieveData ())
    {
        strPtr = configResp + EfiStrLen (configHdr) + 1;
    }
    else
    {
        strPtr = configResp;
    }
    bufferSize = EfiStrSize (strPtr);
    if (*ResultsDataSize < bufferSize)
    {
      *ResultsDataSize = bufferSize;

     // gBS->FreePool (configResp);
      status = EFI_BUFFER_TOO_SMALL;
      goto DONE;
    }

    *ResultsDataSize = bufferSize;
    EfiCopyMem (ResultsData, strPtr, bufferSize);

    SETUP_DEBUG_UEFI_CALLBACK ( "[TSE] Displaying Retrieve Data Buffer, VariableName: %s, VariableHandle: 0x%x\n\n", varInfo->VariableName, varInfo->VariableHandle  );

#if TSE_DEBUG_MESSAGES
    DebugShowBufferContent(bufferSize, ResultsData);
#endif

    //gBS->FreePool(configResp);
  } else
  {
    //
    // Prepare <configResp>
    //

#if TSE_DEBUG_MESSAGES
	{
		CHAR16 BrowserCallbackFilename[50];		
		CHAR16 BrowserCallbackContent[200];
		CHAR16 *BrowserCallbackBuffer = NULL ;
		UINTN Len=0;
	
  		
		SPrint(BrowserCallbackFilename,50,L"BROWSERCALLBACK_%s_%03d.txt", varInfo->VariableName, HpkFileCount);
		SPrint(BrowserCallbackContent, 200, L"VariableName: %s\nVariableGuid: %g\nResultsDataSize: 0x%x\n", VariableName, &VariableGuid, *ResultsDataSize);
		Len = (EfiStrLen (BrowserCallbackContent) + EfiStrLen (ResultsData) + 1) * sizeof (CHAR16);
		BrowserCallbackBuffer = EfiLibAllocateZeroPool (Len);

		if (BrowserCallbackBuffer != NULL)
		{
			EfiStrCpy (BrowserCallbackBuffer, BrowserCallbackContent);
			EfiStrCat (BrowserCallbackBuffer, ResultsData);

    		status = WriteDataToFile(BrowserCallbackFilename, BrowserCallbackBuffer, Len,0) ;// resolve build issue
			if (!EFI_ERROR (status))
			{
				HpkFileCount++ ;
			}

			MemFreePointer((void**)&BrowserCallbackBuffer);
		}
	}
#endif

	
    bufferSize = (EfiStrLen (ResultsData) + EfiStrLen (configHdr) + 2) * sizeof (CHAR16);
    configResp = EfiLibAllocateZeroPool (bufferSize);
    ASSERT (configResp != NULL);

    EfiStrCpy (configResp, configHdr);
    EfiStrCat (configResp, L"&");
    EfiStrCat (configResp, ResultsData);

    //
    // Update Browser uncommited data
    //
    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] _ConfigRespToStorage()\n" );  
    status = _ConfigRespToStorage (varInfo, &buffer, &size, configResp);
    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] _ConfigRespToStorage returned,   status = 0x%x \n\n" , status );
    
    if (EFI_ERROR (status))
    {
		goto DONE;
    }

    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] Displaying Browser Uncommited Data Buffer, VariableName: %s, VariableHandle: 0x%x\n\n", varInfo->VariableName, varInfo->VariableHandle  );

#if TSE_DEBUG_MESSAGES
    DebugShowBufferContent(size, buffer);
#endif

    SETUP_DEBUG_UEFI_CALLBACK (  "\n[TSE] Updating Browser Uncommited Data\n" );
    if (varIndex == VARIABLE_ID_BBS_ORDER)
    {
		if(gCurrLegacyBootData)
		{
			UINT16 CurLegBootDataValid = 0;
			UINT32 i;
			for (i = 0; i < gBootOptionCount; i++ )
			{
				if ( &gBootData[i] == gCurrLegacyBootData )
					CurLegBootDataValid = 1;
			}
			if(CurLegBootDataValid)
				CurrentBootOption = gCurrLegacyBootData->Option;
			else
				gCurrLegacyBootData = NULL;
		}
		gCurrLegacyBootData = BootGetBootData(CurrentBootOption);
		if(gCurrLegacyBootData)
			status = VarSetValue(varIndex, 0, size, buffer);
    }
    else
    status = VarSetValue(varIndex, 0, size, buffer);
    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] Updating Browser Uncommited Data returned,  status = 0x%x \n\n" , status );
  }
DONE:
//MEMFREE
  if(configResp)
  {
    MemFreePointer((void**)&configResp);
  }

  if(buffer)
  {
    MemFreePointer((void**)&buffer);
  }

  if(configHdr)
  {
    MemFreePointer((void**)&configHdr);
  }
  
  SETUP_DEBUG_UEFI_CALLBACK ( "\n[TSE] Exiting BrowserCallback(),  status = 0x%x \n\n" , status );  
      
  return status;
}

/**

    @param VariableInfo 
    @param Buffer 
    @param ConfigHdr 
    @param ConfigResp 

    @retval EFI_STATUS
**/
EFI_STATUS _StorageToConfigResp(VARIABLE_INFO *VariableInfo, VOID *Buffer, CHAR16 *ConfigHdr, CHAR16 **ConfigResp)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_STRING  Progress;
  CHAR16  *configHdr = NULL;
  UINTN   length = 0;

  length  = EfiStrLen(ConfigHdr) * sizeof(CHAR16);
  configHdr = (CHAR16*)EfiLibAllocateZeroPool(length + 2);
  if(configHdr == NULL)
  {
    status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }

  MemCopy(configHdr, ConfigHdr, length);

  //
  // Generate <BlockName>
  //
  status = GetBlockName(&configHdr, &length, VAR_ZERO_OFFSET, VariableInfo->VariableSize);
  if(EFI_ERROR(status))
  {
    goto DONE;
  }

  if((VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE) == VARIABLE_ATTRIBUTE_NAMEVALUE)
  {
    status = EFI_UNSUPPORTED;
    goto DONE;
  }else
  {
    //
    //  VARIABLE_ATTRIBUTE_VARSTORE
    //
    status = HiiInitializeProtocol();
    if(EFI_ERROR(status))
    {
      goto DONE;
    }

    status = gHiiConfigRouting->BlockToConfig(gHiiConfigRouting, configHdr, Buffer,
      VariableInfo->VariableSize, ConfigResp, &Progress);
  }

DONE:
  if(configHdr)			
  {
    MemFreePointer((void**)&configHdr);
  }
  return status;
}

/**

    @param VariableInfo 
    @param Buffer 
    @param Size 
    @param ConfigResp 

    @retval EFI_STATUS
**/
EFI_STATUS _ConfigRespToStorage(VARIABLE_INFO *VariableInfo, UINT8 **Buffer, UINTN *Size, EFI_STRING ConfigResp)
{
  EFI_STATUS status = EFI_SUCCESS;

  if((VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE) == VARIABLE_ATTRIBUTE_NAMEVALUE)
  {
    status = EFI_UNSUPPORTED;
    goto DONE;
  }else
  {
    EFI_STRING stringPtr = NULL;
    EFI_STRING tempPtr = NULL;
    UINTN length = 0;
    UINT16 *sizeBuf = NULL;

    stringPtr = ConfigResp;
    //
    // Get Buffer data Size
    //
    for(stringPtr = ConfigResp; *stringPtr != 0 && EfiStrnCmp(stringPtr, L"&WIDTH=", EfiStrLen(L"&WIDTH=")) != 0; stringPtr++);
    stringPtr += EfiStrLen(L"&WIDTH=");
    tempPtr = stringPtr;
    for(; *stringPtr != 0 && EfiStrnCmp(stringPtr, L"&VALUE=", EfiStrLen(L"&VALUE=")) != 0; length++, stringPtr++);
    sizeBuf = (CHAR16*)EfiLibAllocateZeroPool(length + 1);
    if(sizeBuf == NULL)
	{
      status = EFI_OUT_OF_RESOURCES;
	  goto DONE;
	}
    EfiCopyMem(sizeBuf, tempPtr, length);
	status = GetHexStringAsBuffer(Buffer, sizeBuf);
    MemFreePointer((void**)&sizeBuf);
    if(EFI_ERROR(status))
    {
      *Size = 0;
      goto DONE;
    }
    //
    //  Get Buffer Data
    //
    for(; *stringPtr != 0 && EfiStrnCmp(stringPtr, L"&VALUE=", EfiStrLen(L"&VALUE=")) != 0; stringPtr++);
    stringPtr += EfiStrLen(L"&VALUE=");
    status = GetHexStringAsBuffer(Buffer, stringPtr);
  }

DONE:
  return status;
}

/**
        Procedure	:	FormBrowserHandleValid

        Description	:	return True Browser is showing forms from SendForm interface.

        Input		:	none

        Output		:	BOOLEAN

**/
BOOLEAN FormBrowserHandleValid(VOID)
{
#if SETUP_FORM_BROWSER_SUPPORT
	if(gSfHandles)
		return TRUE;
#endif
	return FALSE;
}

/**
        Procedure	:	FormBrowserLocateSetupHandles

        Description	:	return Handles and count that is passed to SendForm interface.

        Input		:	OUT handleBuffer and count

        Output		:	Status

**/
EFI_STATUS FormBrowserLocateSetupHandles(VOID*	**handleBuffer,UINT16 *count)
{
  EFI_STATUS status = EFI_SUCCESS;

#if SETUP_FORM_BROWSER_SUPPORT
  if(gSfHandles)
  {
    *handleBuffer = (VOID**)gSfHandles;
    *count = (UINT16)gSfHandleCount;
  }
  else
  {
    status =  EFI_NOT_FOUND;
  }
#else
  status = EFI_UNSUPPORTED;
#endif
  return status;
}

/**
        Procedure	:	SaveTseSetupGlobalData

        Description	:	Saves the Global buffer data into the GlobalBuffer stack

        Input		:	none

        Output		:	EFI_STATUS

**/
EFI_STATUS SaveTseSetupGlobalData()
{
	TSE_SETUP_GLOBAL_DATA *temp;
	EFI_STATUS Status = EFI_UNSUPPORTED;
	
	if(!IsSetupFormBrowserNestedSendFormSupport())
		return EFI_UNSUPPORTED;
	
	temp = EfiLibAllocateZeroPool(sizeof(TSE_SETUP_GLOBAL_DATA));
	
	if(NULL == temp)
		return EFI_OUT_OF_RESOURCES;
	SETUP_DEBUG_TSE ("[TSE] SaveTseSetupGlobalData(): Entering\n");
	StopRefreshIdEvent();/* This stops all current context's refreshId events to avoid notified inside Sendform*/

////////Variables available only for TSE Starts///////////

	Status = SaveTseLiteGlobalData((VOID*)temp);
	if(EFI_ERROR(Status))
		goto ERROR;
	
	Status = SaveTseAdvancedGlobalData((VOID*)temp);
	if(EFI_ERROR(Status))
		goto ERROR;		
////////Variables available only for TSE Ends///////////		

////////Variables available for both TSE and ESA Starts///////////  
//AppsData Starts
	temp->AppsData.ApplicationData = gApplicationData;
	gApplicationData = (UINT8*)NULL;
	gPages = NULL;

	temp->AppsData.FirstPageOffset = FirstPageOffset;
	FirstPageOffset = 0;

	temp->AppsData.AllocatedFirstPageSize = AllocatedFirstPageSize;
	AllocatedFirstPageSize = 0;

	temp->AppsData.PageInfoOffset = PageInfoOffset;
	PageInfoOffset = 0;

	temp->AppsData.PageInfoSize = PageInfoSize;
	PageInfoSize = 0;

	temp->AppsData.PageListOffset = PageListOffset;
	PageListOffset = 0;

	temp->AppsData.PageListSize = PageListSize;
	PageListSize = 0;
//AppsData Ends

//SetupData Starts	
	SaveSetupData(temp);
//SetupData Ends	

//SfData Starts
	temp->SfData.SfHandles = gSfHandles;
	gSfHandles = (VOID **)NULL;
	
	temp->SfData.SfHandleCount = gSfHandleCount;
	gSfHandleCount = 0;
	
	temp->SfData.SfNvMap = gSfNvMap;
	gSfNvMap = (UINT8*)NULL;
	
	temp->SfData.FSetGuid = gFSetGuid;
	gFSetGuid = (EFI_GUID*)NULL;
//SfData Ends	
	
//GuidData Starts	
	temp->GuidData.GuidDump = gGuidDump;
	gGuidDump = (EFI_GUID*)NULL;

	temp->GuidData.GuidDumpCount = gGuidDumpCount;
	gGuidDumpCount = 0;
//GuidData Ends	

//ControlData Starts
	temp->ControlData.ControlListOffset = ControlListOffset;
	ControlListOffset = 0;

	temp->ControlData.ControlListSize = ControlListSize;
	ControlListSize = 0;
//ControlData Ends

//VaribaleData Starts
	temp->VariableData.VariableList = gVariableList;		
	gVariableList = (NVRAM_VARIABLE*)NULL;

	temp->VariableData.VariableListOffset = VariableListOffset;
	VariableListOffset = 0;

	temp->VariableData.VariableListSize = VariableListSize;
	VariableListSize = 0;

	temp->VariableData.VariableInfoOffset = VariableInfoOffset;
	VariableInfoOffset = 0;

	temp->VariableData.VariableInfoSize = VariableInfoSize;
	VariableInfoSize = 0;
//VairbaleData Ends	

//DefaultData Starts
	temp->DefaultData.OptimalDefaults = gOptimalDefaults;
	gOptimalDefaults = (NVRAM_VARIABLE*)NULL;

	temp->DefaultData.FailsafeDefaults = gFailsafeDefaults;
	gFailsafeDefaults = (NVRAM_VARIABLE*)NULL;
//DefaultData Ends

//PageIdData Starts	
	temp->PageIdData.PageIdInfoOffset = PageIdInfoOffset;
	PageIdInfoOffset = 0;

	temp->PageIdData.PageIdInfoSize = PageIdInfoSize;
	PageIdInfoSize = 0;

	temp->PageIdData.PageIdListOffset = PageIdListOffset;
	PageIdListOffset = 0;

	temp->PageIdData.PageIdListSize = PageIdListSize;
	PageIdListSize = 0;
//PageIdData Ends	

//DynamicData Starts
	temp->DynamicData.DynamicPageCount = gDynamicPageCount;
	gDynamicPageCount = 0;

	temp->DynamicData.DynamicPageGroup = (VOID*)gDynamicPageGroup;
	gDynamicPageGroup = (DYNAMIC_PAGE_GROUP *)NULL;

	temp->DynamicData.DynamicPageGroupCount = gDynamicPageGroupCount;
	gDynamicPageGroupCount = 0;

	temp->DynamicData.CurrDynamicPageGroup = gCurrDynamicPageGroup;
	gCurrDynamicPageGroup = 0;

	temp->DynamicData.CurrDynamicPageGroupClass = gCurrDynamicPageGroupClass;
	gCurrDynamicPageGroupClass = 0;
//DynamicData Ends

//BootOptionData Starts
	temp->BootOptionData.BootData = gBootData;
	gBootData = (BOOT_DATA*)NULL;

	temp->BootOptionData.BootOptionCount = gBootOptionCount;
	gBootOptionCount = 0;

	temp->BootOptionData.DriverData = gDriverData;
	gDriverData = (BOOT_DATA*)NULL;

	temp->BootOptionData.DriverOptionCount = gDriverOptionCount;
	gDriverOptionCount = 0;

	temp->BootOptionData.CurrLegacyBootData = gCurrLegacyBootData;
	gCurrLegacyBootData = (BOOT_DATA*)NULL;
//BootOptionData Ends

//Notification Handler Starts
	temp->NotifHandler.EnableDrvNotification = gEnableDrvNotification;
	gEnableDrvNotification = FALSE;
	
	temp->NotifHandler.EnableProcessPack = gEnableProcessPack;
	gEnableProcessPack = FALSE;
	
	temp->NotifHandler.PackUpdatePending = gPackUpdatePending;
	gPackUpdatePending = FALSE;
	
	temp->NotifHandler.BrowserCallbackEnabled = gBrowserCallbackEnabled;
	gBrowserCallbackEnabled = FALSE;
//Notification Handler Ends

	temp->TotalRootPages = TotalRootPages;
	TotalRootPages = 0;

	temp->RefData = gRefData;
	gRefData = (EFI_IFR_REF *)NULL;

	temp->PostStatus = gPostStatus;
////////Variables available for both TSE and ESA Ends///////////
	SaveSetupGobalDataHook();
	PushGlobalBuffer(temp);
	SETUP_DEBUG_TSE ("[TSE] SaveTseSetupGlobalData(): Exiting with status EFI_SUCCESS\n");
	return EFI_SUCCESS;

ERROR:
	MemFreePointer((VOID**)&temp); //Free the node
	SETUP_DEBUG_TSE ("[TSE] SaveTseSetupGlobalData(): Exiting with status EFI_UNSUPPORTED\n");
	return EFI_UNSUPPORTED;
}

/**
        Procedure	:	RestoreTseSetupGlobalData

        Description	:	Restores the Global buffer data from the GlobalBuffer stack

        Input		:	none

        Output		:	none

**/
EFI_STATUS RestoreTseSetupGlobalData()
{

	TSE_SETUP_GLOBAL_DATA *temp = NULL;
	
	if(!IsSetupFormBrowserNestedSendFormSupport())
		return EFI_UNSUPPORTED;
	
	SETUP_DEBUG_TSE ("[TSE] RestoreTseSetupGlobalData(): Entering\n");
	
	PopGlobalBuffer(&temp);

	if(temp == NULL)
	{
		SETUP_DEBUG_TSE ("[TSE] RestoreTseSetupGlobalData() Exiting - No Buffer found in Stack\n");
		return EFI_NOT_FOUND;
	}

////////Variables available for both TSE and ESA Starts///////////	

//AppsData Starts
	gApplicationData = temp->AppsData.ApplicationData;
	gPages = (PAGE_LIST	*)gApplicationData;

	FirstPageOffset = temp->AppsData.FirstPageOffset;

	AllocatedFirstPageSize = temp->AppsData.AllocatedFirstPageSize;

	PageInfoOffset = temp->AppsData.PageInfoOffset;

	PageInfoSize = temp->AppsData.PageInfoSize;

	PageListOffset = temp->AppsData.PageListOffset;

	PageListSize = temp->AppsData.PageListSize;
//AppsData Ends

//SetupData Starts	
	RestoreSetupData(temp);
//SetupData Ends	

//SfData Starts
	gSfHandles = temp->SfData.SfHandles;

	gSfHandleCount = temp->SfData.SfHandleCount;

	gSfNvMap = temp->SfData.SfNvMap;

	gFSetGuid = temp->SfData.FSetGuid;
//SfData Ends	

//GuidData Starts	
	gGuidDump = temp->GuidData.GuidDump;

	gGuidDumpCount = temp->GuidData.GuidDumpCount;
//GuidData Ends	

//ControlData Starts
	ControlListOffset = temp->ControlData.ControlListOffset;

	ControlListSize = temp->ControlData.ControlListSize;
//ControlData Ends

//VaribaleData Starts
	gVariableList = temp->VariableData.VariableList;		

	VariableListOffset = temp->VariableData.VariableListOffset;

	VariableListSize = temp->VariableData.VariableListSize;

	VariableInfoOffset = temp->VariableData.VariableInfoOffset;

	VariableInfoSize = temp->VariableData.VariableInfoSize;
//VairbaleData Ends	

//DefaultData Starts
	gOptimalDefaults = temp->DefaultData.OptimalDefaults;

	gFailsafeDefaults = temp->DefaultData.FailsafeDefaults;
//DefaultData Ends

//PageIdData Starts	
	PageIdInfoOffset = temp->PageIdData.PageIdInfoOffset;

	PageIdInfoSize = temp->PageIdData.PageIdInfoSize;

	PageIdListOffset = temp->PageIdData.PageIdListOffset;

	PageIdListSize = temp->PageIdData.PageIdListSize;
//PageIdData Ends	

//DynamicData Starts
	gDynamicPageCount = temp->DynamicData.DynamicPageCount;
	
	gDynamicPageGroup = (DYNAMIC_PAGE_GROUP *)temp->DynamicData.DynamicPageGroup;
	
	gDynamicPageGroupCount = temp->DynamicData.DynamicPageGroupCount;
	
	gCurrDynamicPageGroup = temp->DynamicData.CurrDynamicPageGroup;

	gCurrDynamicPageGroupClass = temp->DynamicData.CurrDynamicPageGroupClass;
//DynamicData Ends		

//BootOptionData Starts
	gBootData = temp->BootOptionData.BootData;

	gBootOptionCount = temp->BootOptionData.BootOptionCount;

	gDriverData = temp->BootOptionData.DriverData;

	gDriverOptionCount = temp->BootOptionData.DriverOptionCount;

	gCurrLegacyBootData = temp->BootOptionData.CurrLegacyBootData;
//BootOptionData Ends

//Notification Handler Starts
	gEnableDrvNotification = temp->NotifHandler.EnableDrvNotification;
	
	gEnableProcessPack = temp->NotifHandler.EnableProcessPack;
	
	gPackUpdatePending = temp->NotifHandler.PackUpdatePending;
	
	gBrowserCallbackEnabled = temp->NotifHandler.BrowserCallbackEnabled;
//Notification Handler Ends	
	
	TotalRootPages = temp->TotalRootPages;

	gRefData = temp->RefData;

	gPostStatus = temp->PostStatus;

////////Variables available for both TSE and ESA Ends///////////
		
////////Variables available only for TSE Starts///////////
//TSELite Globals Starts
	RestoreTseLiteGlobalData((VOID*)temp);
//TSELite Globals Ends	

//TSEAdvanced Globals Starts
	RestoreTseAdvancedGlobalData((VOID*)temp);
//TSEAdvanced Globals Ends
////////Variables available only for TSE Ends///////////		
	RestoreSetupGobalDataHook();

	RestoreRefreshIdEvents();/* This recreates current context's refresh Id events.*/

	RestoreVariablesInNVRAM();
	
	MemFreePointer((VOID**)&temp); //Free the node
	SETUP_DEBUG_TSE ("[TSE] RestoreTseSetupGlobalData() Exiting\n");
	return EFI_SUCCESS;
}

/**
        Procedure	:	PushGlobalBuffer

        Description	:	Pushes GlobalBuffer data into the stack

        Input		:	TSE_SETUP_GLOBAL_DATA *

        Output		:	none

**/
VOID PushGlobalBuffer (TSE_SETUP_GLOBAL_DATA *NewBuffer)
{
	if(NewBuffer)
	{
		NewBuffer->Next = CurrentBuffer;
		CurrentBuffer = NewBuffer;
	}
}

/**
        Procedure	:	PopGlobalBuffer

        Description	:	Pops GlobalBuffer data from the stack

        Input		:	TSE_SETUP_GLOBAL_DATA**

        Output		:	none

**/
VOID PopGlobalBuffer(TSE_SETUP_GLOBAL_DATA** Buffer)
{
	if(Buffer != NULL)
	{
		*Buffer = CurrentBuffer;
		
		if(CurrentBuffer)
		{
			CurrentBuffer = CurrentBuffer->Next;
		}
	}
}

/**
        Procedure	:	IsGlobalBufferStackEmpty

        Description	:	Check whether the GlobalBuffer Stack is Empty.

        Input		:	none

        Output		:	BOOLEAN

**/
BOOLEAN IsGlobalBufferStackEmpty()
{
	TSE_SETUP_GLOBAL_DATA* Buffer = NULL;
	
	PopGlobalBuffer(&Buffer);
	
	if(Buffer)
	{
		PushGlobalBuffer(Buffer);
		return FALSE;
	}
	
	return TRUE;	
}

/**
        Procedure	:	StopRefreshIdEvent

        Description	:	Close all RefershId Event before starts SendForm operation

        Input		:	none

        Output		:	none

**/

VOID StopRefreshIdEvent()
{
	UINTN index;
	EFI_STATUS Status = EFI_UNSUPPORTED;
	
	SETUP_DEBUG_TSE ("[TSE] StopRefreshIdEvent(): Entering\n");
    //Close the Event and Free the NotifyContext created for Controls with Refresh Id.
    if(gRefreshIdInfo) {
        for(index = 0; index < gRefreshIdCount; index++) 
        {
            if(gRefreshIdInfo[index].pEvent)//Close the event
            {
	            Status = gBS->CloseEvent(gRefreshIdInfo[index].pEvent);
	            if(Status == EFI_SUCCESS)
                {
		            gRefreshIdInfo[index].pEvent = NULL;
	            }		
            }
            
            MemFreePointer( (VOID **)&gRefreshIdInfo[index].pNotifyContext );
        }
        MemFreePointer( (VOID **)&gRefreshIdInfo );
        gRefreshIdCount =0;
    }
	SETUP_DEBUG_TSE ("[TSE] StopRefreshIdEvent(): Exiting\n");
}


/**
        Procedure	:	RestoreRefreshIdEvents

        Description	:	Recreate all RefreshId Events for All pages/controls after SendForm operation performed

        Input		:	none

        Output		:	none

**/
VOID RestoreRefreshIdEvents()
{
	UINTN Page,Control;
	
	if(gApplicationData && gPages && gPageIdList)
	{
		SETUP_DEBUG_TSE ("[TSE] RestoreRefreshIdEvents(): Entering\n");
		for(Page = 0; Page < gPages->PageCount ; Page++)
		{
			PAGE_INFO *PageInfo = (PAGE_INFO*) NULL;
			PAGE_ID_INFO* PageIdInfo = (PAGE_ID_INFO *)NULL;
			UINT16 link = 0;

			PageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[Page]);	
			
			if(!PageInfo || PageInfo->PageHandle == NULL)
				continue;
			
			PageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[PageInfo->PageIdIndex]);
			
			if(!PageIdInfo)
				continue;
			
			link = _HiiGetLinkIndex( &(PageIdInfo->PageGuid),
					PageIdInfo->PageClass,
					PageIdInfo->PageSubClass,
					PageInfo->PageFormID);

			if ( link == (gSetupCount - 1) )
			{
				continue;
			}
			
			for(Control = 0; Control < PageInfo->PageControls.ControlCount; Control++)
			{
				CONTROL_INFO *ControlInfo = (CONTROL_INFO*)((UINT8 *)gControlInfo + PageInfo->PageControls.ControlList[Control]);

				if(ControlInfo && ControlInfo->ControlFlags.RefreshID)
					CreateEventforIFR(ControlInfo);
			}
			if(PageInfo->PageFlags.PageRefreshID)
			{
				CheckRefreshIDForSFPage(PageInfo,link);
			}
		
		}
	}
	SETUP_DEBUG_TSE ("[TSE] RestoreRefreshIdEvents(): Exiting\n");
}


VOID RestoreVariablesInNVRAM()
{
	EFI_GUID DynamicPageGuid = DYNAMIC_PAGE_COUNT_GUID;
    EFI_GUID DynamicPageGroupGuid = DYNAMIC_PAGE_GROUP_COUNT_GUID ;
    EFI_GUID DynamicPageGroupClassGuid = DYNAMIC_PAGE_GROUP_CLASS_COUNT_GUID ;
    EFI_GUID DynamicPageDeviceGuid = DYNAMIC_PAGE_DEVICE_COUNT_GUID;
    EFI_GUID DrvHealthCtrlGuid = AMITSE_DRIVER_HEALTH_CTRL_GUID;
	EFI_GUID DrvHealthGuid = AMITSE_DRIVER_HEALTH_GUID;
	EFI_GUID DrvHlthEnableGuid = AMITSE_DRIVER_HEALTH_ENB_GUID;
	DYNAMIC_PAGE_GROUP_CLASS DynamicPageGroupClass = {0};
	UINTN CountN = 0;
	UINT16 Count16 = 0;

	SETUP_DEBUG_TSE ("[TSE] RestoreVariablesInNVRAM(): Entering\n");

	CountN = gDynamicPageCount;
	SETUP_DEBUG_TSE ("[TSE] Restoring DynamicPageCount = %d into NVRAM\n",CountN);
	VarSetNvramName( L"DynamicPageCount", &DynamicPageGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &CountN, sizeof(UINTN) );
	
	Count16 = (UINT16)gDynamicPageGroupCount;
	SETUP_DEBUG_TSE ("[TSE] Restoring DynamicPageCount = %d into NVRAM\n",CountN);
	VarSetNvramName (L"DynamicPageGroupCount", &DynamicPageGroupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &Count16, sizeof (UINT16));
	
	Count16	= 0;
	VarGetValue(VARIABLE_ID_DYNAMIC_PAGE_DEVICE,0,sizeof(UINT16),&Count16);
	SETUP_DEBUG_TSE ("[TSE] Restoring DynamicPageDeviceCount = %d into NVRAM\n",Count16);
    VarSetNvramName( L"DynamicPageDeviceCount", &DynamicPageDeviceGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &Count16, sizeof(UINT16) );
	
    VarGetValue(VARIABLE_ID_DYNAMIC_PAGE_GROUP_CLASS,0,sizeof(DYNAMIC_PAGE_GROUP_CLASS),&DynamicPageGroupClass);
	SETUP_DEBUG_TSE ("[TSE] Restoring DynamicPageGroupClass into NVRAM\n");
	VarSetNvramName (L"DynamicPageGroupClass", &DynamicPageGroupClassGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &DynamicPageGroupClass, sizeof (DYNAMIC_PAGE_GROUP_CLASS));

	CountN = 0;
	VarGetValue(VARIABLE_ID_DRV_HLTH_COUNT,0,sizeof(UINTN),&CountN);
	SETUP_DEBUG_TSE ("[TSE] Restoring DriverHealthCount = %d into NVRAM\n",CountN);
	VarSetNvramName (L"DriverHealthCount", &DrvHealthGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &CountN, sizeof (UINTN));

	CountN = 0;
	VarGetValue(VARIABLE_ID_DRV_HLTH_ENB,0,sizeof(UINTN),&CountN);
	SETUP_DEBUG_TSE ("[TSE] Restoring DriverHlthEnable = %d into NVRAM\n",CountN);
	VarSetNvramName (L"DriverHlthEnable", &DrvHlthEnableGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &CountN, sizeof (UINTN));

	CountN = 0;
	VarGetValue(VARIABLE_ID_DRV_HLTH_CTRL_COUNT,0,sizeof(UINTN),&CountN);
	SETUP_DEBUG_TSE ("[TSE] Restoring DrvHealthCtrlCnt = %d into NVRAM\n",CountN);
	VarSetNvramName (L"DrvHealthCtrlCnt", &DrvHealthCtrlGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &CountN, sizeof (UINTN));

	SETUP_DEBUG_TSE ("[TSE] RestoreVariablesInNVRAM(): Exiting\n");
}

/**
        Procedure	:	GetSfStartPageId

        Description	:	To get the first display PageId from gSfFormId for SendForm.

        Param		:	StartPageId

        Return		:	Status

**/
EFI_STATUS GetSfStartPageId(UINT16 *StartPageId)
{  
    UINTN page,i;
    EFI_STATUS Status = EFI_NOT_FOUND;
    
    if(!FormBrowserHandleValid() || !gPages || !gApplicationData)
    {
    	SETUP_DEBUG_TSE("[TSE] GetSfStartPageId() Exiting: Status = EFI_UNSUPPORTED\n");
        return EFI_UNSUPPORTED;
    }    
    if(!StartPageId)
    {
    	SETUP_DEBUG_TSE("[TSE] GetSfStartPageId() Exiting: Status = EFI_INVALID_PARAMETER\n");
        return EFI_INVALID_PARAMETER;
    }
    
    SETUP_DEBUG_TSE("[TSE] GetSfStartPageId() Entering: gSfFormId = %d\n",gSfFormId);
    *StartPageId = 0xffff;
    
    if(gSfFormId == 0)
    {
        goto END;
    }
    for(page = 0 ; page < gPages->PageCount ; page++)
    {
        PAGE_INFO* PageInfo = (PAGE_INFO*)(UINTN) (gApplicationData + gPages->PageList[page]);

        if(!PageInfo || PageInfo->PageHandle == NULL || PageInfo->PageHandle == (VOID*)0xffff)
            continue;

        if(PageInfo->PageFormID == gSfFormId)
        {
            for(i = 0; i < gSfHandleCount; i++)
            {
                if(PageInfo->PageHandle == gSfHandles[i])
                {
                    *StartPageId = PageInfo->PageID;
                    Status = EFI_SUCCESS;
                    goto END;
                }
            }
        }
    }
    
END:    
	SETUP_DEBUG_TSE("[TSE] GetSfStartPageId() Exiting: Status = %r  StartPageId = %d\n",Status,StartPageId);
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
