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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/special.c $
//
// $Author: Premkumara $
//
// $Revision: 35 $
//
// $Date: 4/03/12 12:40a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file special.c
    This file contains code to handle special Operations

**/

#include "minisetup.h"
#include "special.h"  

#define BOOT_OPTION_SIZE sizeof(UINT16)

extern VOID UefiSetHelpField(VOID *IfrPtr,UINT16 Token);
EFI_STATUS GetFormSetTitleAndHelpWrapper (VOID *Handle, UINT16 *Title, UINT16 *Help);
extern BOOLEAN gSetupUpdated;
VOID _SpecialControlExtras(CONTROL_DATA *ctrlData);
VOID _SpecialFixupBootOrder( CONTROL_INFO *control , UINT16 value);
VOID _SpecialFixupLanguages( CONTROL_INFO *control );
VOID _SpecialFixupBBSOrder( CONTROL_INFO *control, UINT16 value );
VOID _SpecialFixupBBSCondition(UINT16 ControlIndex,CONTROL_INFO *control,UINT16 PrevControlQuestionID);
VOID _SpecialFixupBootCondition(UINT16 ControlIndex,CONTROL_INFO *control,UINT16 PrevControlQuestionID);

UINT32 FindVarFromITKQuestionId(UINT16 QuestionId);
VOID _SpecialFixupAddBootOption( CONTROL_INFO *control);
UINT16 GetNoVarStoreBootCountOffset();

UINT16 BBSGetNonStandardGroupType(UINT16 DeviceType);	
extern UINT16 GetMaxBasedOnClassCode(CONTROL_INFO *controlInfo, UINT16 *Offset);

#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
extern UINT16 OEMSpecialGetControlCount(CONTROL_INFO *controlInfo);
extern VOID OEMSpecialOneOfFixup( CONTROL_INFO *control , UINT16 value );
extern VOID OEMSpecialGotoFixup(CONTROL_INFO *control, UINT16 value );
#endif //#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
extern STYLECOLORS  Colors;
extern UINT16 gCurrDynamicPageGroup ;
extern DYNAMIC_PAGE_GROUP *gDynamicPageGroup ;
extern UINT16 gCurrDynamicPageGroupClass ;
extern UINT32			gtempCurrentPage;
CHAR16 *FindDriverHealthDriverName (UINT16);
CHAR16 *GetCtrlNameAndHealth (UINT16 ControllerEntry);
VOID	*gDriverPageHandle = NULL;
UINTN 	gDriverOptionTokenCount;
UINT16 	*gDriverOptionTokenArray;
BOOLEAN CheckForAddDelDriverOption (VOID);
VOID _SpecialFixupDriverOrder (CONTROL_INFO *control, UINT16 value);
BOOLEAN IsRefXControlInCurrentPage(UINTN PageId);
VOID UpdateFrameControlDataForCurrentPage(UINT32 CurrentPage, UINT16 CurrentFrame);

/**
    function to handle the operation of getting the Ifr Pointer.

    @param conditional 
    @param variable 
    @param guidInfo 

    @retval VOID

**/
VOID *_SpecialGetIfrPtr( CONTROL_INFO *controlInfo, UINT32 * variable, GUID_INFO **guidInfo )
{
    *variable = (UINT32)GetControlConditionVarId(controlInfo);
	return UefiGetSpecialEqIDIfrPtr(controlInfo, variable, guidInfo);
}

/**
    function to handle the operation of getting the Ifr Value.

    @param control 
    @param guidInfo 

    @retval UINT16

**/
UINT16 SpecialGetValue( CONTROL_DATA *control, GUID_INFO **guidInfo )
{
	return UefiGetSpecialEqIDValue(&control->ControlData, guidInfo);
}

/**
    function to handle the operation of getting the Ifr Value.

    @param controlinfo 
    @param guidInfo 

    @retval UINT16

**/
UINT16 _SpecialGetValue( CONTROL_INFO *controlinfo, GUID_INFO **guidInfo )
{
	return UefiGetSpecialEqIDValue(controlinfo, guidInfo);
}


/**
    function to Handle adding a control.

    @param frame 
    @param controlinfo 
    @param SetFocus 

    @retval EFI_STATUS

**/
EFI_STATUS _SpecialAddControl(FRAME_DATA *frame , CONTROL_INFO *ControlInfo, BOOLEAN *SetFocus)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	STYLECOLORS saveColors ;
	

	//Save original style colors
	saveColors = Colors ; 

	//Special control color scheme...
	StyleControlColor(frame, &Colors);
		
	//Adds controls to this frame...
	Status = gFrame.AddControl( frame, ControlInfo  );
	
	//Restore original style colors
	Colors = saveColors;	

	if ( EFI_ERROR(Status) )
		return Status;

	if ( *SetFocus  )
	{
        if ( _FrameSetControlFocus( frame, frame->ControlCount-1 ) )
		{
            frame->CurrentControl = frame->ControlCount-1;
			*SetFocus = FALSE;
		}
	}

	return Status;

}
/**

    @param newControlInfo 


    @retval VOID

**/
VOID DoAddBootOptionFixup( CONTROL_INFO *newControlInfo )
{
		_SpecialFixupAddBootOption(newControlInfo);
}

/**
    Adds Multiple controls for special controls

    @param frame CONTROL_INFO *controlInfo, GUID_INFO *guidInfo, BOOLEAN *SetFocus 

    @retval VOID

**/
VOID _SpecialAddMultipleControls( FRAME_DATA *frame , CONTROL_INFO *controlInfo, GUID_INFO *guidInfo, BOOLEAN *SetFocus )
{
	CONTROL_INFO *newControlInfo;
	VARIABLE_INFO *varInfo = NULL;
	UINT8 *dataPtr;
  	VOID *ifrPtr;
	CHAR16 *name = NULL;

	UINT16 count, max = 0, value = 0, HiddenBootOptionCount=0;
	UINT16 PrevControlQuestionID=0;

	UINT32 varnum=0;
	UINTN condSize;
	BOOLEAN HiddenFound = FALSE;

	if ( frame == NULL )
		return;

	if ( guidInfo->GuidFlags.GuidLaunchCallbackUpdateTemplate )
    {
		UefiSpecialGuidCallback( controlInfo->ControlHandle, 0xFFFE, &guidInfo->GuidValue);
   	}

    varnum = GetControlConditionVarId(controlInfo); //controlInfo->ControlConditionalVariable[0];
    ifrPtr = _SpecialGetIfrPtr( controlInfo, &varnum, NULL ); // TO get the special variable.

  if(NoVarStoreSupport())
  {
    //More info needed.
    if(varnum == VARIABLE_ID_SETUP)
    {
      ifrPtr = _SpecialGetIfrPtr( controlInfo, &varnum, NULL );

      //The variable supplied is setup find the right variable
      varnum = FindVarFromITKQuestionId(UefiGetEqIDQuestionID(ifrPtr));
    }
  }

	if( varnum == VARIABLE_ID_ERROR_MANAGER )  // if it is error manager variable, get the real updated nvram data.
		VarUpdateVariable(varnum);

  	varInfo = ((VARIABLE_INFO *)((UINT8 *)gVariables + gVariables->VariableList[varnum]));
  	name = GetGUIDNameByID(VARIABLE_ID_LANGUAGE);
    //The following is replaced by Name comparision
    //if ( ((VARIABLE_INFO *)((UINT8 *)gVariables + gVariables->VariableList[varnum]))->VariableID == LANGUAGE_CODES_KEY_ID )
    if(EfiStrCmp(varInfo->VariableName, name) == 0)
    {
        max = 1;
    }
    else if (
			(
			CheckForAddDelBootOption () &&
            ((VARIABLE_ID_DEL_BOOT_OPTION  == controlInfo->ControlVariable) || (VARIABLE_ID_ADD_BOOT_OPTION == controlInfo->ControlVariable))
			) ||
			(
			CheckForAddDelDriverOption () &&			
            ((VARIABLE_ID_DEL_DRIVER_OPTION == controlInfo->ControlVariable) || (VARIABLE_ID_ADD_DRIVER_OPTION == controlInfo->ControlVariable))
			)
			) 
	{
        max = 1;
	}
#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
    else if(varnum == VARIABLE_ID_OEM_TSE_VAR )
	{
        max = OEMSpecialGetControlCount(controlInfo);
	}
#endif
	else if (varnum == VARIABLE_ID_DYNAMIC_PAGE_GROUP_CLASS)
	{
		UINT16 Offset = 0;
		max = GetMaxBasedOnClassCode(controlInfo, &Offset);
		switch(Offset)
		{
		case 0:
			gCurrDynamicPageGroupClass = 0x00;
			break;
		case 2: 
			gCurrDynamicPageGroupClass = 0x01;
			break;
		case 4:
			gCurrDynamicPageGroupClass = 0x02;
			break;
		case 6: 
			gCurrDynamicPageGroupClass = 0x04;
			break;
		case 8:
			gCurrDynamicPageGroupClass = 0x08;
			break;
		case 10: 
			gCurrDynamicPageGroupClass = 0x10;
			break;
		case 12: 
			gCurrDynamicPageGroupClass = 0x20;
			break;
		}
	}
	else if (varnum == VARIABLE_ID_DYNAMIC_PAGE_COUNT && IsGroupDynamicPagesByClass())
	{
		max = 0;
	}
    else
	{
        VarGetValue( varnum, 0, sizeof(UINT16), &max );
	}
	MemFreePointer((void**)&name);
	
	for (count = 0; count < max; count++)
	{
    	if ( ( controlInfo->ControlPtr == 0 ) || ( controlInfo->ControlConditionalPtr == 0 ) )
      		continue;
		if (
			(gLoadOptionHidden && ((VARIABLE_ID_BOOT_MANAGER == varnum) || (VARIABLE_ID_BOOT_NOW == varnum) || (VARIABLE_ID_DRIVER_MANAGER == varnum))) &&
			(VARIABLE_ID_DEL_BOOT_OPTION  != controlInfo->ControlVariable) && 		//For deleting the boot/driver option max is 1 so if first option is hidden nothing will proceed
			(VARIABLE_ID_ADD_BOOT_OPTION != controlInfo->ControlVariable) &&
			(VARIABLE_ID_DEL_DRIVER_OPTION != controlInfo->ControlVariable) && 
			(VARIABLE_ID_ADD_DRIVER_OPTION != controlInfo->ControlVariable)
			)
		{
			if (gShowAllBbsDev && (VARIABLE_ID_BOOT_NOW == varnum))
			{
				if (NULL == BootGetBootNowName (count, gShowAllBbsDev, BootNowInBootOrderSupport()))
				{
					HiddenBootOptionCount ++; // If the Boot option has the attribute Hidden increment the HiddenBootOptionCount 
					continue;
				}
			}
			else if (count < gBootOptionCount)			// To avoid comparing garbage gBootData for LoadOptionHidden when SETUP_SHOW_ALL_BBS_DEVICES = 1
			{
			 	if ((VARIABLE_ID_DRIVER_MANAGER == varnum) ? (LoadOptionhidden (count, DRIVER_ORDER_OPTION)) : (LoadOptionhidden (count, BOOT_ORDER_OPTION)))
				{
					HiddenBootOptionCount ++; // If the Boot option has the attribute Hidden increment the HiddenBootOptionCount 
					continue;
				}
			}
		}
		//check for legacy group hidden options starts
		else if (gLoadOptionHidden && (VARIABLE_ID_LEGACY_DEV_INFO == varnum))
		{
			UINTN 	LegDevIndex = 0;
			UINTN 	LegDevCount = 0;
			BOOT_DATA *bootData;
			
            for (LegDevIndex = 0; LegDevIndex < gBootOptionCount; LegDevIndex ++)
            {
                bootData = &(gBootData[LegDevIndex]);
                
                if (BBSValidDevicePath (bootData->DevicePath))
                {
					if (gLoadOptionHidden && (bootData->Active & LOAD_OPTION_HIDDEN) && (LegDevCount == count))
					{
						HiddenFound = TRUE;
						break;
					}
                    LegDevCount ++;
                }
            }
		}
        if (TRUE == HiddenFound)
        {
        	continue;
        }
        //check for legacy group hidden options ends
        
		newControlInfo = EfiLibAllocatePool( sizeof( CONTROL_INFO ) );
		if ( newControlInfo == NULL )
			continue;

    	MemCopy( newControlInfo, controlInfo, sizeof( CONTROL_INFO ) );
    	condSize = (UINTN)controlInfo->ControlPtr - (UINTN)controlInfo->ControlConditionalPtr;

    	// 256 should be replaced by ControlDataLength, but HPKTool currently does not set it
    	dataPtr = EfiLibAllocateZeroPool( 256 + condSize );
    	if(dataPtr == NULL)
    	{
     	 	MemFreePointer((void**)&newControlInfo);
      		continue;
    	}

		newControlInfo->ControlConditionalPtr = dataPtr;
		MemCopy( dataPtr, (VOID *)controlInfo->ControlConditionalPtr, condSize );
		dataPtr += condSize;
		newControlInfo->ControlPtr = dataPtr;
		MemCopy( dataPtr, (VOID *)controlInfo->ControlPtr, 256 );

		ifrPtr = _SpecialGetIfrPtr( newControlInfo, &varnum, NULL );

		if ( ifrPtr != NULL )
		{
			//if TSE_BOOT_NOW_IN_BOOT_ORDER = 1 then,set the ID Value as (count - HiddenBootOptionCount)
			//since TSE using Boot order variable to display the option and hiddens boot options are present at the end of the Boot order var
			if ((BootNowInBootOrderSupport()) || (UefiIsOneOfControl ((VOID *)dataPtr)) && ((VARIABLE_ID_BOOT_MANAGER == varnum) || (VARIABLE_ID_DRIVER_MANAGER == varnum)))
			{
        		UefiSetEqIDValue (ifrPtr, count - HiddenBootOptionCount);
			}
			else
			{
				UefiSetEqIDValue (ifrPtr, count);
			}
		}
		ifrPtr = (VOID *)dataPtr;

		//Depending on the value of the token hide of the disabled boot options is done
		if (gSetupHideDisableBootOptions)
		{
			if(((VARIABLE_ID_BOOT_MANAGER == varnum)||(VARIABLE_ID_BOOT_NOW == varnum))&&(!(UefiIsOneOfControl( ifrPtr ))))
			{		
				//if TSE_BOOT_NOW_IN_BOOT_ORDER = 1 then,set the BootOptionIndex Value as (count - HiddenBootOptionCount)
				//since TSE using Boot order variable to display the option and hiddens boot options are present at the end of the Boot order var
				UINT16 BootOptionIndex = 0;
				BootOptionIndex = ((BootNowInBootOrderSupport())? count - HiddenBootOptionCount : count);
				if (IsBootDeviceEnabled (BootOptionIndex, gShowAllBbsDev, BootNowInBootOrderSupport (), 1))
					continue;
			}
		}
		//Checking for the  Bootmanger variable and the token for Load Option hidden
		
	    if (UefiIsOneOfControl (ifrPtr))
		{
			UINTN size;
			CHAR16 *newText;
			CHAR16 *text ;

	      	text = HiiGetString( controlInfo->ControlHandle, UefiGetPromptField(ifrPtr) );
			if (NULL == text) {
				text = EfiLibAllocateZeroPool(2*sizeof(CHAR16));
				if(!text)
					return;
				EfiStrCpy(text,L" ");
	        }
	
			size = sizeof(CHAR16) * ( EfiStrLen( text ) + 10 );
			newText = EfiLibAllocateZeroPool( size );

			SPrint( newText, size, text, count + 1 - HiddenBootOptionCount );
			
			value = _SpecialGetValue( newControlInfo, &guidInfo );
		    UefiSetPromptField(ifrPtr, HiiAddString( controlInfo->ControlHandle, newText ));

			// update boot order , bbs and language tokens, prevent double initialization
			// this code was part of AddControlExtras().
      		UefiUpdateControlVarOffset (newControlInfo->ControlPtr, value * sizeof(UINT16));

       	     switch ( guidInfo->GuidKey )
       	     {
                 case GUID_KEY_BOOT_NOW:
			        if (NoVarStoreSupport())
			        {
			          _SpecialFixupBootOrder (newControlInfo, value);
			        }
			        else
					{
	                    if (newControlInfo->ControlVariable == VARIABLE_ID_BOOT_ORDER)
						{
					          _SpecialFixupBootOrder (newControlInfo, value);
					          //if count not eq to 0 and count-1 Control is Disabled then supress the current control.
							  _SpecialFixupBootCondition(count,newControlInfo,PrevControlQuestionID);
							  PrevControlQuestionID = UefiGetQuestionID(newControlInfo);
						}
	                    else if (newControlInfo->ControlVariable == VARIABLE_ID_DEL_BOOT_OPTION)
	                    {
							TSESpecialFixupDelBootOption (newControlInfo);
	                    }
					}
					break;
                case GUID_KEY_ADD_BOOT_OPTION:
                    if ((CheckForAddDelBootOption()) && (newControlInfo->ControlVariable == VARIABLE_ID_ADD_BOOT_OPTION))
                    {
						UefiUpdateControlVarOffset(newControlInfo->ControlPtr, UefiGetQuestionOffset(controlInfo->ControlPtr));
						TseDoAddBootOptionFixup( newControlInfo );
                    }
					break;
			
				case GUID_KEY_LANG_CODES:
					if ( gLangCount > 0 )
						_SpecialFixupLanguages( newControlInfo );
					break;

				case GUID_KEY_BBS_ORDER:
					_SpecialFixupBBSOrder( newControlInfo, value);
			        if(!NoVarStoreSupport())
			        {
			          //if count not eq to 0 and count-1 Control is Disabled then supress the current control.
			          _SpecialFixupBBSCondition(count,newControlInfo,PrevControlQuestionID);
					  PrevControlQuestionID = UefiGetQuestionID(newControlInfo);
			        }
					break;
				case GUID_KEY_DRIVER_OPTION:			
                    if (VARIABLE_ID_DRIVER_ORDER == newControlInfo->ControlVariable)
					{
				          _SpecialFixupDriverOrder (newControlInfo, value);
				          //if count not eq to 0 and count-1 Control is Disabled then supress the current control.
						  _SpecialFixupBootCondition (count, newControlInfo, PrevControlQuestionID);
						  PrevControlQuestionID = UefiGetQuestionID(newControlInfo);
					}
					else if (VARIABLE_ID_DEL_DRIVER_OPTION == newControlInfo->ControlVariable)
					{
						TSESpecialFixupDelDriverOption (newControlInfo);
					}
					break;
#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
				case GUID_KEY_OEM_CONTROL:
					// Do the OEM Special Control's OneOf Fixup
					OEMSpecialOneOfFixup( newControlInfo, value);
					break;
#endif
				default:
					break;
			}			

			MemFreePointer( (VOID **)&newText );
			MemFreePointer( (VOID **)&text );
		}
	    SETUP_DEBUG_TSE ("\n[TSE] Adding Special Control Ptr:%X",newControlInfo->ControlConditionalPtr);
		_SpecialAddControl( frame, newControlInfo, SetFocus );
		// update optional info specially for boot order controls
        _SpecialControlExtras((frame->ControlList[frame->ControlCount-1]));
		MemFreePointer( (VOID **)&newControlInfo );
	}

	gSetupUpdated = FALSE; //Reset the SetupUpdated Flag here.
}

/**
    function to fixup the languages

    @param control 

    @retval VOID

**/
VOID _SpecialFixupLanguages( CONTROL_INFO *control )
{
	VOID *ifrPtr, *templatePtr, *oneOf, *temp;
	VOID *tempPtr;
	UINTN i,  length = 0;
  	BOOLEAN flag = FALSE;

	ifrPtr = (VOID *)control->ControlPtr;
	length = UefiGetIfrLength(ifrPtr);
	temp = (VOID *)control->ControlConditionalPtr;
	
	if(PlatformLangVerSupport())	
 	 	UefiSetWidth(ifrPtr, 6);
	else
 	 	UefiSetWidth(ifrPtr, 3);

	tempPtr = UefiCreateOneOfWithOptionsTemplate( gLangCount, control );
	if ( tempPtr == NULL )
		return;

	templatePtr = (UINT8*)ifrPtr + length;
	oneOf = (UINT8*)tempPtr + length;
  	length = UefiGetIfrLength(templatePtr);
	for ( i = 0; i < gLangCount; i++ )
	{
		MemCopy( oneOf, templatePtr, length );
    	flag = 0;
        gLanguages[i].Token = HiiChangeString( gHiiHandle, gLanguages[i].Token, gLanguages[i].LangString );
    	UefiSetOneOfOption(oneOf, i, BOOT_OPTION_SIZE, flag, gLanguages[i].Token);
    	oneOf = (UINT8*)oneOf +length;
	}

  	// Add an End op to close scope of the One of control
  	UefiAddEndOp(oneOf);

	tempPtr = HiiGetString( control->ControlHandle, UefiGetPromptField(ifrPtr) );
	UefiSetPromptField (control->ControlPtr,HiiAddString( gHiiHandle, tempPtr ));

	MemFreePointer( (VOID **)&tempPtr );
	tempPtr = HiiGetString( control->ControlHandle, UefiGetHelpField(ifrPtr) );

	control->ControlHelp = HiiAddString( gHiiHandle, tempPtr );
	control->ControlHandle = gHiiHandle;

	MemFreePointer( (VOID **)&tempPtr );
	MemFreePointer( (VOID **)&temp );
}


static VOID		*gBootPageHandle = NULL;
UINTN 			gBootOptionTokenCount;
UINT16 			*gBootOptionTokenArray;

/**
    function to get the boot option token properly when loading the boot page

    @param handle UINTN Index

    @retval TOKEN

**/
UINT16 GetSpecialBootoptionToken(VOID* handle, UINTN Index) 
{
	UINTN i;

	// TO make sure it is called by _SpecialFixupBootOrder() and Boot page not modified
	if (gBootPageHandle == handle)
	{
		if((FALSE == gSetupUpdated) || !gBootOptionTokenArray)
		{
			if(!gBootOptionTokenCount || !gBootOptionTokenArray)
			{
				// Create the gBootOptionTokenArray
				gBootOptionTokenArray = EfiLibAllocateZeroPool (sizeof(UINT16) * gBootOptionCount);
				gBootOptionTokenCount = gBootOptionCount;
				for(i=0;i< gBootOptionTokenCount;i++)
				{
					gBootOptionTokenArray[i] = INVALID_TOKEN;
				}
			}
			else if(gBootOptionTokenCount < gBootOptionCount)
			{ // Boot options added. Update gBootOptionTokenArray
				gBootOptionTokenArray = MemReallocateZeroPool(gBootOptionTokenArray, (sizeof(UINT16) * gBootOptionTokenCount), (sizeof(UINT16) * gBootOptionCount));
				for(i= gBootOptionTokenCount; i< gBootOptionCount ;i++)
				{
					gBootOptionTokenArray[i] = INVALID_TOKEN;
				}
				gBootOptionTokenCount = gBootOptionCount;
			}
			
			if(gBootOptionTokenArray[Index] == INVALID_TOKEN)
				gBootOptionTokenArray[Index] = HiiAddString(handle, BootGetOptionName( &(gBootData[Index]) ) );
            else
            {
                // Check if Update is needed
                CHAR16 *text;
                text = HiiGetString( handle, gBootOptionTokenArray[Index]);
					 
				if (NULL == text) 
					return (UINT16)EFI_OUT_OF_RESOURCES;
						 
                if(EfiStrCmp(text, BootGetOptionName( &(gBootData[Index]) )) != 0)
                {
                    // need to update the token with new BootoptionName.
                    UINT16  Token = HiiChangeString( handle, gBootOptionTokenArray[Index], BootGetOptionName( &(gBootData[Index]) ));
                    if(Token != gBootOptionTokenArray[Index])
                        gBootOptionTokenArray[Index] = Token;
                }
                MemFreePointer((VOID **)&text);
            }
		}
		else {
			//If the some control modified with respect to boot page..
	 		gBootOptionTokenArray[Index] = HiiAddString(handle, BootGetOptionName( &(gBootData[Index]) ) );
		}
		return gBootOptionTokenArray[Index];

	}
	else
		//If the call is not from _SpecialFixupBootOrder() 
	 	return	HiiAddString(handle, BootGetOptionName( &(gBootData[Index]) ) );
}

/**
    function to get the boot option token properly when loading the boot page

    @param handle UINTN Index

    @retval TOKEN

**/
UINT16 _GetSpecialDriveroptionToken (VOID* handle, UINTN Index) 
{
	UINTN i;

	if (gDriverPageHandle == handle)
	{
		if(FALSE == gSetupUpdated || !gDriverOptionTokenArray)
		{
			if (!gDriverOptionTokenCount || !gDriverOptionTokenArray)
			{
				// Create the gDriverOptionTokenArray
				gDriverOptionTokenArray = EfiLibAllocateZeroPool (sizeof(UINT16) * gDriverOptionCount);
				gDriverOptionTokenCount = gDriverOptionCount;
				for (i = 0; i < gDriverOptionTokenCount; i ++)
				{
					gDriverOptionTokenArray [i] = INVALID_TOKEN;
				}
			}
			else if (gDriverOptionTokenCount < gDriverOptionCount)
			{ // Drivers options added. Update gDriverOptionTokenArray
				gDriverOptionTokenArray = MemReallocateZeroPool (gDriverOptionTokenArray, (sizeof (UINT16) * gDriverOptionTokenCount), (sizeof (UINT16) * gDriverOptionCount) );
				for (i = gDriverOptionTokenCount; i < gDriverOptionCount ; i ++)
				{
					gDriverOptionTokenArray [i] = INVALID_TOKEN;
				}
				gDriverOptionTokenCount = gDriverOptionCount;
			}
			
			if (gDriverOptionTokenArray [Index] == INVALID_TOKEN)
				gDriverOptionTokenArray [Index] = HiiAddString (handle, DriverGetOptionName (&(gDriverData [Index]) ) );
			else
            {
                // Check if Update is needed
                CHAR16 *text;
                text = HiiGetString( handle, gDriverOptionTokenArray [Index]);
                if(EfiStrCmp(text, DriverGetOptionName ( &(gDriverData[Index]) )) != 0)
                {
                    // need to update the token with new DriveroptionName.
                    UINT16  Token = HiiChangeString( handle, gDriverOptionTokenArray [Index], DriverGetOptionName( &(gDriverData[Index]) ));
                    if(Token != gDriverOptionTokenArray [Index])
                        gDriverOptionTokenArray [Index] = Token;
                }
                MemFreePointer((VOID **)&text);
            }
		}
		else {
			//If the some control modified with respect to boot page..
	 		gDriverOptionTokenArray [Index] = HiiAddString (handle, DriverGetOptionName (&(gDriverData [Index]) ) );
		}
		return gDriverOptionTokenArray [Index];

	}
	else
		//If the call is not from _SpecialFixupBootOrder() 
	 	return	HiiAddString(handle, DriverGetOptionName (&(gDriverData [Index]) ) );
}

/**
    function to fixup the Boot device order

    @param control 
    @param value 

    @retval VOID

**/
VOID _SpecialFixupBootOrder (CONTROL_INFO *control, UINT16 value)
{
	VOID 	*tempPtr, *ifrPtr, *temp;
	VOID 	*templatePtr, *oneOf;
	UINTN 	i;
	CHAR16 	*text;
	UINT8 	length = 0;
	UINT16 	option = 0;
	UINT8 	flag = 0;

	ifrPtr = control->ControlPtr;
	length = UefiGetIfrLength(ifrPtr);
	temp = (VOID *)control->ControlConditionalPtr;
	//Create one one_of_option for each boot option + 1 for Disable option
	tempPtr = UefiCreateOneOfWithOptionsTemplate (gBootOptionCount+1, control);
	if ( tempPtr == NULL )
		return;

	templatePtr = ((UINT8*)ifrPtr + length);
	oneOf = ((UINT8*)tempPtr + length);
  length = UefiGetIfrLength(templatePtr); 

	gBootPageHandle = control->ControlHandle;		//++
	for ( i = 0; i < gBootOptionCount; i++ ) {
		flag = (i == value)? (UINT8)HiiGetDefaultMask() : 0;
		//option = HiiAddString( control->ControlHandle, BootGetOptionName( &(gBootData[i]) ) );		// --
		// Checking the  LOAD_OPTION_HIDDEN for the boot option
		if (gLoadOptionHidden && (gBootData [i].Active & LOAD_OPTION_HIDDEN) )
			continue;
		option = GetSpecialBootoptionToken (control->ControlHandle, i);
		MemCopy (oneOf, templatePtr, length);
		// Add one of option
		UefiSetOneOfOption (oneOf, gBootData[i].Option, BOOT_OPTION_SIZE, flag, option);
		oneOf = (UINT8*)oneOf +length;
	}

  //Create one of option for Disabled
	flag = 0;
	MemCopy (oneOf, templatePtr, length);
	text = HiiGetString (gHiiHandle, STR_CTRL_CHKBOX_DISABLE);
	option = HiiAddString (control->ControlHandle, text);

	//UefiSetOneOfOption (oneOf, 0xFFFF, BOOT_OPTION_SIZE, flag, option); 
	UefiSetOneOfOption (oneOf, DISABLED_BOOT_OPTION, BOOT_OPTION_SIZE, flag, option);

	MemFreePointer((VOID **)&text);
	oneOf = (UINT8*)oneOf +length;

	// Add an End op to close scope of the One of control
	UefiAddEndOp(oneOf);

	ifrPtr = control->ControlPtr;
	control->ControlHelp = UefiGetHelpField(ifrPtr);

	if(NoVarStoreSupport())
	{
		VOID *ifrPtr;
		UINT16  QuestionId;
		
		control->ControlVariable = VARIABLE_ID_BOOT_ORDER;
		control->ControlConditionalVariable[0] = VARIABLE_ID_BOOT_MANAGER;
		
		ifrPtr = _SpecialGetIfrPtr( control,  VARIABLE_ID_SETUP, NULL);
		QuestionId = UefiGetEqIDQuestionID(ifrPtr);
		QuestionId = QuestionId - GetNoVarStoreBootCountOffset();
		UefiSetEqIDQuestionID(ifrPtr, QuestionId);
	}
	MemFreePointer((VOID **)&temp);
}

/**
    Function to fixup the driver order

    @param control 
    @param value 

    @retval VOID

**/
VOID _SpecialFixupDriverOrder (CONTROL_INFO *control, UINT16 value)
{
	VOID 	*tempPtr, *ifrPtr, *temp;
	VOID 	*templatePtr, *oneOf;
	UINTN 	i;
	CHAR16 	*text;
	UINT8 	length = 0;
	UINT16 	option = 0;
	UINT8 	flag = 0;

	ifrPtr = control->ControlPtr;
	length = UefiGetIfrLength(ifrPtr);
	temp = (VOID *)control->ControlConditionalPtr;
	//Create one one_of_option for each driver option + 1 for Disable option
	tempPtr = UefiCreateOneOfWithOptionsTemplate (gDriverOptionCount+1, control);
	if (NULL == tempPtr)
		return;
	templatePtr = ((UINT8*)ifrPtr + length);
	oneOf = ((UINT8*)tempPtr + length);
	length = UefiGetIfrLength(templatePtr); 
	gDriverPageHandle = control->ControlHandle;
	
	for ( i = 0; i < gDriverOptionCount; i++ )
	{
		flag = (i == value)? (UINT8)HiiGetDefaultMask() : 0;
		if (gLoadOptionHidden && (gDriverData [i].Active & LOAD_OPTION_HIDDEN))
			continue;
		option = _GetSpecialDriveroptionToken (control->ControlHandle, i);
		MemCopy (oneOf, templatePtr, length);
			// Add one of option
		UefiSetOneOfOption (oneOf, gDriverData [i].Option, BOOT_OPTION_SIZE, flag, option);
		oneOf = (UINT8*)oneOf +length;
	}
	//Create one of option for Disabled
	flag = 0;
	MemCopy (oneOf, templatePtr, length);
	text = HiiGetString (gHiiHandle, STR_CTRL_CHKBOX_DISABLE);
	option = HiiAddString (control->ControlHandle, text);

	//UefiSetOneOfOption (oneOf, 0xFFFF, BOOT_OPTION_SIZE, flag, option); 
	UefiSetOneOfOption (oneOf, DISABLED_DRIVER_OPTION, BOOT_OPTION_SIZE, flag, option);

	MemFreePointer((VOID **)&text);
	oneOf = (UINT8*)oneOf +length;

	// Add an End op to close scope of the One of control
	UefiAddEndOp(oneOf);

	ifrPtr = control->ControlPtr;
	control->ControlHelp = UefiGetHelpField(ifrPtr);
	MemFreePointer((VOID **)&temp);
}

/**
    This function updates the Condition to avoid more then one 
    disabled option in Boot order.

    @param ControlIndex, Control Info, PrevControlQuestionID

    @retval VOID

**/
VOID _SpecialFixupBootCondition (UINT16 ControlIndex,CONTROL_INFO *control,UINT16 PrevControlQuestionID)
{
	// Don't Update the Condition for First option
    if(!ControlIndex)
		return;
	else
	{
        UINT8 u8ChkResult = CheckControlCondition( control );
        if(u8ChkResult == COND_NONE)
        {
    		VARIABLE_INFO *VarInfo;
    		VarInfo = VarGetVariableInfoIndex(control->ControlVariable);
			if(NULL != VarInfo){
				UefiCreateDynamicControlCondition(control,(UINT16)VarInfo->VariableID,PrevControlQuestionID,DISABLED_BOOT_OPTION);
			}
        }
	}

}

/**
    This function updates the Condition to avoid more then one 
    disabled option in BBS Dev order.

    @param ControlIndex, Control Info, PrevControlQuestionID

    @retval VOID

**/
VOID _SpecialFixupBBSCondition(UINT16 ControlIndex,CONTROL_INFO *control,UINT16 PrevControlQuestionID)
{
	// Don't Update the Condition for First option
    if(!ControlIndex)
		return;
	else
	{
        UINT8 u8ChkResult = CheckControlCondition( control );
        if(u8ChkResult == COND_NONE)
        {
    		VARIABLE_INFO *VarInfo;
    		VarInfo = VarGetVariableInfoIndex(control->ControlVariable);
			if(NULL != VarInfo){
	    		UefiCreateDynamicControlCondition(control,(UINT16)VarInfo->VariableID,PrevControlQuestionID,DISABLED_BOOT_OPTION);
			}
        }
	}

}

/**
    function to fixup the Legacy Boot device order

    @param control 
    @param value 

    @retval VOID

**/
VOID _SpecialFixupBBSOrder( CONTROL_INFO *control, UINT16 value )
{
	VOID *ifrPtr, *templatePtr, *oneOf, *tempPtr, *temp;
	UINTN i;
	UINT16 count,option;
  UINT16 length = 0;
  CHAR16 *text;
	UINT8 flag;

	count = gCurrLegacyBootData->LegacyDevCount;
	temp = (VOID *)control->ControlConditionalPtr;
	ifrPtr = (VOID *)control->ControlPtr;
  length = UefiGetIfrLength(ifrPtr);
	//Create one one_of_option for each device + 1 for Disable option
  tempPtr = UefiCreateOneOfWithOptionsTemplate(count + 1, control);
	if ( tempPtr == NULL )
		return;


	// set real offset into bbs variable for this control. 
  BBSUpdateControlOffset(control);

	templatePtr = (UINT8*)ifrPtr + length;
	oneOf = (UINT8*)tempPtr + length;
  length = UefiGetIfrLength(templatePtr);

	for ( i = 0; i < count; i++)
	{
    UINT16 bbsOptionVal = 0;

		MemCopy( oneOf, templatePtr, length );
    flag = (i == value)? (UINT8)HiiGetDefaultMask() : 0;
		option = HiiAddString( control->ControlHandle, BBSGetOptionName( &bbsOptionVal,(UINT16)i ) );
		MemCopy( oneOf, templatePtr, length );
    // Add one of option
    UefiSetOneOfOption(oneOf, bbsOptionVal, BOOT_OPTION_SIZE, flag, option);
    	oneOf = (UINT8*)oneOf +length;
	}

  //Create one of option for Disabled
  flag = 0;
  MemCopy( oneOf, templatePtr, length );
  text = HiiGetString(gHiiHandle,STR_CTRL_CHKBOX_DISABLE);
  option = HiiAddString( control->ControlHandle, text );

	//UefiSetOneOfOption(oneOf, 0xFFFF, BOOT_OPTION_SIZE, flag, option);
	UefiSetOneOfOption(oneOf, DISABLED_BOOT_OPTION, BOOT_OPTION_SIZE, flag, option);

  MemFreePointer((VOID **)&text);
 	oneOf = (UINT8*)oneOf +length;

  // Add an End op to close scope of the One of control
  UefiAddEndOp(oneOf);

	ifrPtr = control->ControlPtr;
	control->ControlHelp = UefiGetHelpField(ifrPtr);
	MemFreePointer((VOID **)&temp);
}

/**
    function to fixup the deletion of a boot device option

    @param control 

    @retval VOID

**/
VOID SpecialFixupDelBootOption (CONTROL_INFO *control)
{
	VOID *ifrPtr, *templatePtr, *oneOf, *temp;
	VOID *tempPtr;
	UINTN i, length = 0;
	
	ifrPtr = (VOID *)control->ControlPtr;
	length = UefiGetIfrLength(ifrPtr);
	temp = (VOID *)control->ControlConditionalPtr;
	//Create one one_of_option for each boot option + 1 for Disable option
	tempPtr = UefiCreateOneOfWithOptionsTemplate (gBootOptionCount + 1, control);
	if ( tempPtr == NULL )
		return;
	
	templatePtr = (UINT8*)ifrPtr + length;
	oneOf = (UINT8*)tempPtr + length;
	length = UefiGetIfrLength (templatePtr);
	
	//Copy the first option and skip to next
	MemCopy( oneOf, templatePtr, length );
	oneOf = (UINT8*)oneOf +length;
	
	for ( i = 0; i < gBootOptionCount; i++ )
	{
		if(
			((gBootData[i].DevicePath->Type == BBS_DEVICE_PATH) && (gBootData[i].DevicePath->SubType == BBS_BBS_DP)) ||
			(gLoadOptionHidden && (gBootData [i].Active & LOAD_OPTION_HIDDEN))	
			)
		{
			continue;
		}
		else
		{
			UINT16 option = 0;
			UINT8 flag = 0;
	
			MemCopy( oneOf, templatePtr, length );
			option = HiiAddString (control->ControlHandle, BootGetOptionName (&(gBootData[i])));
			// Add one of option
			UefiSetOneOfOption (oneOf, gBootData[i].Option, BOOT_OPTION_SIZE, flag, option);
			oneOf = (UINT8*)oneOf +length;
		}
	}

	// Add an End op to close scope of the One of control
	UefiAddEndOp (oneOf);
	ifrPtr = control->ControlPtr;
	control->ControlHelp = UefiGetHelpField (ifrPtr);
	MemFreePointer((VOID **)&temp);
}
/**
    function to fixup the adding of a boot device option

    @param control 

    @retval VOID

**/
VOID _SpecialFixupAddBootOption( CONTROL_INFO *control)
{
	VOID *ifrPtr, *templatePtr, *oneOf, *temp;
	VOID *tempPtr;
	UINTN i,  length = 0;

	ifrPtr = (VOID *)control->ControlPtr;
   	length = UefiGetIfrLength(ifrPtr);
   	temp = (VOID *)control->ControlConditionalPtr;
  	//Create one one_of_option for each file sys
	tempPtr = UefiCreateOneOfWithOptionsTemplate( gFsCount, control );
	if ( tempPtr == NULL )
		return;

	UefiUpdateControlVarOffset(control, STRUCT_OFFSET(NEW_BOOT_OPTION, SelFs));

	templatePtr = (UINT8*)ifrPtr + length;
	oneOf = (UINT8*)tempPtr + length;
  	length = UefiGetIfrLength(templatePtr);

	for ( i = 0; i < gFsCount; i++)
	{
    UINT16 option = 0;
    UINT8 flag = 0;

    MemCopy( oneOf, templatePtr, length );
    // Add one of option
		option = HiiAddString( control->ControlHandle, gFsList[i].FsId);
    UefiSetOneOfOption(oneOf, (UINT16)i, BOOT_OPTION_SIZE, flag, option);
     	oneOf = (UINT8*)oneOf +length;

	}

  // Add an End op to close scope of the One of control
  UefiAddEndOp(oneOf);
	ifrPtr = control->ControlPtr;
	control->ControlHelp = UefiGetHelpField(ifrPtr);
	MemFreePointer((VOID **)&temp);
}

/**
    Function to fixup the deletion of a driver option

    @param control 

    @retval VOID

**/
VOID SpecialFixupDelDriverOption (CONTROL_INFO *control)
{
	VOID *ifrPtr, *templatePtr, *oneOf, *temp;
	VOID *tempPtr;
	UINTN i, length = 0;

	ifrPtr = (VOID *)control->ControlPtr;
	length = UefiGetIfrLength (ifrPtr);
	temp = (VOID *)control->ControlConditionalPtr;
	//Create one one_of_option for each driver option + 1 for Disable option
	tempPtr = UefiCreateOneOfWithOptionsTemplate (gDriverOptionCount + 1, control);
	if (NULL == tempPtr)
		return;
	
	templatePtr = (UINT8*)ifrPtr + length;
	oneOf = (UINT8*)tempPtr + length;
	length = UefiGetIfrLength (templatePtr);
	
	//Copy the first option and skip to next
	MemCopy (oneOf, templatePtr, length);
	oneOf = (UINT8*)oneOf +length;
	
	for (i = 0; i < gDriverOptionCount; i++)
	{
		if(
			((gDriverData [i].DevicePath->Type == BBS_DEVICE_PATH) && (gDriverData [i].DevicePath->SubType == BBS_BBS_DP)) ||
			(gLoadOptionHidden && (gDriverData [i].Active & LOAD_OPTION_HIDDEN))	
			)
		{
			continue;
		}
		else
		{
			UINT16 	option = 0;
			UINT8 	flag = 0;
			MemCopy (oneOf, templatePtr, length);
			option = HiiAddString (control->ControlHandle, DriverGetOptionName (&(gDriverData [i])));
			// Add one of option
			UefiSetOneOfOption (oneOf, gDriverData [i].Option, BOOT_OPTION_SIZE, flag, option);
			oneOf = (UINT8*)oneOf +length;
		}
	}
	// Add an End op to close scope of the One of control
	UefiAddEndOp (oneOf);
	ifrPtr = control->ControlPtr;
	control->ControlHelp = UefiGetHelpField(ifrPtr);
	MemFreePointer((VOID **)&temp);
}

/**
    function to explode a perticular contorller

    @param frame 
    @param ctrlData 
    @param SetFocus 

    @retval control Value

**/
INT16 _SpecialExplodeControl( FRAME_DATA *frame, CONTROL_INFO *ctrlData, BOOLEAN *SetFocus )
{
	UINT16 value;
	GUID_INFO *guidInfo;

	value = _SpecialGetValue( ctrlData, &guidInfo );
	if ( value == (UINT16)-1 )
		_SpecialAddMultipleControls( frame,ctrlData, guidInfo, SetFocus );

	return value;
}

/**
    special handling of extra operations

    @param ctrlData 

    @retval VOID

**/
VOID _SpecialControlExtras(CONTROL_DATA *ctrlData)
{
	GUID_INFO *guidInfo = NULL;
  UINT16 token = 0;

	UINT16 value = _SpecialGetValue( &ctrlData->ControlData, &guidInfo );
    if ( value != (UINT16)-2 )
	{
		switch(ctrlData->ControlData.ControlType) 
		{
            case CONTROL_TYPE_SUBMENU:// submenu control
				if (guidInfo->GuidKey != GUID_KEY_ADD_BOOT_OPTION)//To draw GEOMETRICSHAPE_RIGHT_TRIANGLE for AddBootOption
                	((SUBMENU_DATA*)ctrlData)->SubMenuType = 2;
                if (guidInfo->GuidKey == GUID_KEY_BOOT_NOW)
                {
                    token = HiiAddString( ctrlData->ControlData.ControlHandle, BootGetBootNowName(value,gShowAllBbsDev,BootNowInBootOrderSupport()));
                  //Update the submenu prompt field.
                  UefiSetPromptField(ctrlData->ControlData.ControlPtr, token);
                }
		        else if ( guidInfo->GuidKey == GUID_KEY_ERROR_MANAGER )
		        {
			        UINTN size = 0;
                    ERROR_MANAGER *errManager = VarGetNvram( VARIABLE_ID_ERROR_MANAGER, &size );
                    
                    if ( ( errManager != NULL ) && ( value < errManager->ErrorCount ) )
                    {
                        ERROR_LIST *errList = &(errManager->ErrorList[value]);

                        // Update the submenu prompt field.
                        UefiSetPromptField(ctrlData->ControlData.ControlPtr, errList->Summary);
                        ctrlData->ControlData.ControlHelp = errList->DetailInfo;
                        ctrlData->ControlData.ControlHandle = (VOID*)errList->Handle;
                        
			        }
                    if( errManager != NULL )
                	    MemFreePointer( (VOID **)&errManager );
		        }
                else if (guidInfo->GuidKey == GUID_KEY_DYNAMIC_PAGE)
				{
					UINTN i = 0,j;
					UINTN Index=0;
                    PAGE_INFO *pageInfo;
                    PAGE_INFO *tmppageInfo;
				    PAGE_DATA **page;

                    //To maintain consistency, show arrow when displaying submenu (SubMenuType=0).  
                    ((SUBMENU_DATA*)ctrlData)->SubMenuType = 0; 

					//Scan for dynamic pages					
					for(i = 0; i < gPages->PageCount; i++)
					{
						pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[i]);
            

						if((pageInfo->PageFlags.PageDynamic == TRUE) && (pageInfo->PageHandle != NULL))
					 	{
							if(Index == value)
							{
								
                                CHAR16 *text;
                                UINT16 Title=0, Help=0 ;
                                
                                // Get Formset Title and Help for Dynamic page
                                GetFormSetTitleAndHelpWrapper (pageInfo->PageHandle, &Title, &Help);
                                
								// Add the Prompt
								text = HiiGetString( pageInfo->PageHandle, pageInfo->PageSubTitle);
		                        token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
                        		UefiSetPromptField(ctrlData->ControlData.ControlPtr, token);
                        		 MemFreePointer( (VOID **)&text ); 
                                // Display Formset Help for Dynamic page
                                if(Help != 0)
                                {
                        			 //MemFreePointer( (VOID **)&text );
                                    text = HiiGetString( pageInfo->PageHandle, Help);
		                            token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
                        		    UefiSetHelpField(ctrlData->ControlData.ControlPtr, token);
                                    ctrlData->ControlData.ControlHelp = token ;
                        			 MemFreePointer( (VOID **)&text ); 
                                }
                            
								// Fix the parent pageID
								ctrlData->ControlData.ControlDestPageID = pageInfo->PageID ;
								pageInfo->PageParentID = gApp->CurrentPage;
                                gDynamicParentPage = gApp->CurrentPage;
                                // Reinitilize the Page.
            					for(j = i; j < gPages->PageCount; j++)
            					{
                                    tmppageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[j]);
                                    if(pageInfo->PageHandle == tmppageInfo->PageHandle )
                                    {
                                        page = &gApp->PageList[tmppageInfo->PageID];
                                        gPage.Destroy( *page, FALSE );
                                        gPage.Initialize( *page, tmppageInfo );
                                    }
                                }
                    
                            }
                            Index++;
                        }
                    }
                    
                }
                else if (guidInfo->GuidKey == GUID_KEY_DYNAMIC_PAGE_DEVICE)
				{
					UINTN i = 0,j;
					UINTN Index=0;
                    PAGE_INFO *pageInfo;
                    PAGE_INFO *tmppageInfo;
				    PAGE_DATA **page;

                    //To maintain consistency, show arrow when displaying submenu (SubMenuType=0).  
                    ((SUBMENU_DATA*)ctrlData)->SubMenuType = 0; 

					//Scan for dynamic pages					
					for(i = 0; i < gPages->PageCount; i++)
					{
						pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[i]);
            
						if((pageInfo->PageFlags.PageDynamic == TRUE) && (pageInfo->PageHandle != NULL))
					 	{
						    if(IsGroupDynamicPagesByClass() && (gCurrDynamicPageGroup == gPageIdInfo[pageInfo->PageIdIndex].PageClass ))
						    {    
						        if(Index == value)
                                {
                                    CHAR16 *text;
                                    UINT16 Title=0, Help=0 ;
                                    
                                    // Get Formset Title and Help for Dynamic page
                                    GetFormSetTitleAndHelpWrapper (pageInfo->PageHandle, &Title, &Help);
                                    
                                    // Add the Prompt
                                    text = HiiGetString( pageInfo->PageHandle, pageInfo->PageSubTitle);
                                    token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
                                    UefiSetPromptField(ctrlData->ControlData.ControlPtr, token);
                                     MemFreePointer( (VOID **)&text ); 
                                    // Display Formset Help for Dynamic page
                                    if(Help != 0)
                                    {
                                         //MemFreePointer( (VOID **)&text );
                                        text = HiiGetString( pageInfo->PageHandle, Help);
                                        token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
                                        UefiSetHelpField(ctrlData->ControlData.ControlPtr, token);
                                        ctrlData->ControlData.ControlHelp = token ;
                                         MemFreePointer( (VOID **)&text ); 
                                    }
                                
                                    // Fix the parent pageID
                                    ctrlData->ControlData.ControlDestPageID = pageInfo->PageID ;
                                    pageInfo->PageParentID = gApp->CurrentPage;
                                    gDynamicParentPage = gApp->CurrentPage;
                                    // Reinitilize the Page.
                                    for(j = i; j < gPages->PageCount; j++)
                                    {
                                        tmppageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[j]);
                                        if(pageInfo->PageHandle == tmppageInfo->PageHandle )
                                        {
                                            page = &gApp->PageList[tmppageInfo->PageID];
                                            gPage.Destroy( *page, FALSE );
                                            gPage.Initialize( *page, tmppageInfo );
                                        }
                                    }
                                    break ;
                                }
							    Index++;
					 	    }
					    }
					}
					
				}
				else if (guidInfo->GuidKey == GUID_KEY_DYNAMIC_PAGE_CLASS)
				{
					UINTN i = 0,j;
					UINTN Index=0;
					PAGE_INFO *pageInfo;
					PAGE_INFO *tmppageInfo;
					PAGE_DATA **page;
	
					//To maintain consistency, show arrow when displaying submenu (SubMenuType=0).  
					((SUBMENU_DATA*)ctrlData)->SubMenuType = 0; 
	
					//Scan for dynamic pages					
					for(i = 0; i < gPages->PageCount; i++)
					{
						pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[i]);
	
						if((pageInfo->PageFlags.PageDynamic == TRUE) && (pageInfo->PageHandle != NULL))
						{
							if(IsGroupDynamicPagesByClass() && (gCurrDynamicPageGroupClass == gPageIdInfo[pageInfo->PageIdIndex].PageClass ))
							{    
								if(Index == value)
								{
									CHAR16 *text;
									UINT16 Title=0, Help=0 ;
	
									// Get Formset Title and Help for Dynamic page
									GetFormSetTitleAndHelpWrapper (pageInfo->PageHandle, &Title, &Help);
	
									// Add the Prompt
									text = HiiGetString( pageInfo->PageHandle, pageInfo->PageSubTitle);
									token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
									UefiSetPromptField(ctrlData->ControlData.ControlPtr, token);
									MemFreePointer( (VOID **)&text ); 
									// Display Formset Help for Dynamic page
									if(Help != 0)
									{
										//MemFreePointer( (VOID **)&text );
										text = HiiGetString( pageInfo->PageHandle, Help);
										token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
										UefiSetHelpField(ctrlData->ControlData.ControlPtr, token);
										ctrlData->ControlData.ControlHelp = token ;
										MemFreePointer( (VOID **)&text ); 
									}
	
									// Fix the parent pageID
									ctrlData->ControlData.ControlDestPageID = pageInfo->PageID ;
									pageInfo->PageParentID = gApp->CurrentPage;
									gDynamicParentPage = gApp->CurrentPage;
									// Reinitilize the Page.
									for(j = i; j < gPages->PageCount; j++)
									{
										tmppageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[j]);
										if(pageInfo->PageHandle == tmppageInfo->PageHandle )
										{
											page = &gApp->PageList[tmppageInfo->PageID];
											gPage.Destroy( *page, FALSE );
											gPage.Initialize( *page, tmppageInfo );
										}
									}
									break ;
								}
								Index++;
							}
						}
					}
	
				}
				else if (guidInfo->GuidKey == GUID_KEY_IDE_SECURITY)
				{             
					CHAR16 *text;
					UINT16 IDEStrToken = TSEIDEPasswordGetName(value);
					

					if(IDEStrToken)
					{
						UINTN size;						
						IDE_SECURITY_CONFIG *ideSecConfig;						
						size = 0;
						//Update mem copy of the IDE sec variable
						ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );					
						if (NULL == ideSecConfig) {
							break;
						}
						TSEIDEUpdateConfig(ideSecConfig, value);
						VarSetValue( VARIABLE_ID_IDE_SECURITY, 0, size, ideSecConfig ); //to update VARIABLE_ID_IDE_SECURITY variable when HDD is frozen in setup
						MemFreePointer((VOID **)&ideSecConfig);
						
						//VarUpdateVariable (VARIABLE_ID_IDE_SECURITY);
						text = HiiGetString( gHiiHandle, IDEStrToken);
						token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
						//Update the submenu prompt field.
						UefiSetPromptField(ctrlData->ControlData.ControlPtr, token);
						MemFreePointer( (VOID **)&text );
					}
				}
#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
		        else if ( guidInfo->GuidKey == GUID_KEY_OEM_CONTROL )
				{
					OEMSpecialGotoFixup(&ctrlData->ControlData,value);
				}
#endif
/**/
                else if (guidInfo->GuidKey == GUID_KEY_BBS_ORDER)
                {
                    CHAR16 *text = NULL;
                    UINTN i,j;
                    BOOT_DATA *bootData;

                    for(i=0,j=0; i<gBootOptionCount; i++)
                    {
                        bootData = &(gBootData[i]);
						if(gLoadOptionHidden && (bootData->Active & LOAD_OPTION_HIDDEN))
						  continue;
				
                        if ( BBSValidDevicePath(bootData->DevicePath) )
                        {
                            if(value == j)
                            {
                                BBS_BBS_DEVICE_PATH *DevPath = (BBS_BBS_DEVICE_PATH *)bootData->DevicePath;

                                switch(DevPath->DeviceType)
                                {
                                    case BBS_TYPE_FLOPPY:
                                        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_FLOPPY_DISK_ORDER));
                                    break;
                                    case BBS_TYPE_HARDDRIVE:
                                        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_HARD_DISK_ORDER));
                                    break;
                                    case BBS_TYPE_CDROM:
                                        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_CDROM_ORDER));
                                    break;

                                    case BBS_TYPE_PCMCIA:
                                        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_PCMCIA_ORDER));
                                    break;

                                    case BBS_TYPE_USB:
                                        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_USB_ORDER));
                                    break;

                                    case BBS_TYPE_EMBEDDED_NETWORK:
                                        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_NETWORK_ORDER));
                                    break;
                                    case BBS_TYPE_DEV:
                                        text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_BEV_ORDER));
                                    break;
                                    default:
                                        /// Updated to get the BBS group type from the Board module in case of a non standard type.
                                        text = HiiGetString( gHiiHandle, BBSGetNonStandardGroupType(DevPath->DeviceType));
                                    break;
                                }

                                token = HiiAddString( ctrlData->ControlData.ControlHandle, text);
                                //Update the submenu prompt field.
                                UefiSetPromptField(ctrlData->ControlData.ControlPtr, token);

                      			MemFreePointer( (VOID **)&text );

                                break;
                            }
                            j++;
                        }
                    }
                }
				else if (GUID_KEY_DRIVER_HEALTH == guidInfo->GuidKey)
				{
					CHAR16	*text;
					((SUBMENU_DATA*)ctrlData)->SubMenuType = 0;		//showing submenu triangle
					text = FindDriverHealthDriverName (value);
					if (NULL != text)
					{
						token = HiiAddString (ctrlData->ControlData.ControlHandle, text);
						UefiSetPromptField (ctrlData->ControlData.ControlPtr, token);
					}
					MemFreePointer( (VOID **)&text );
				}
				else if (GUID_KEY_DRV_HEALTH_CTRL_COUNT == guidInfo->GuidKey)
				{
					CHAR16	*text;
					text = GetCtrlNameAndHealth (value);
					if (NULL != text)
					{
						token = HiiAddString (ctrlData->ControlData.ControlHandle, text);
						UefiSetPromptField (ctrlData->ControlData.ControlPtr, token);
					}
					MemFreePointer( (VOID **)&text );
				}
				else if (GUID_KEY_DRIVER_HEALTH_ENB == guidInfo->GuidKey)
				{
					((SUBMENU_DATA*)ctrlData)->SubMenuType = 0;				//showing submenu triangle
				}
				else if (GUID_KEY_DYNAMIC_PAGE_GROUP == guidInfo->GuidKey)
                {
                    CHAR16  *text = NULL, *help = NULL;
                    UINT16 kIndex = 0, jIndex = 0 ;
                    
                    //To maintain consistency, show arrow when displaying submenu (SubMenuType=0).  
                    ((SUBMENU_DATA*)ctrlData)->SubMenuType = 0; 

                    for (jIndex = 0; jIndex < DYNAMIC_PAGE_GROUPS_SIZE ; jIndex ++)
                    {
                        if (gDynamicPageGroup [jIndex].Count)
                        {
                            kIndex ++;
                        }
                        if (value + 1 == kIndex)
                        {
                            switch(jIndex)
                            {
                                case 0x0  : 
                                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_NON_DEVICE_GROUP));
                                    break ;
                                case 0x1 :       
                                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_DISK_DEVICE_GROUP));
                                    break ;
                                case 0x2 :      
                                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_VIDEO_DEVICE_GROUP));
                                    break ;
                                case 0x3 :    
                                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_NETWORK_DEVICE_GROUP));
                                    break ;
                                case 0x4 :      
                                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_INPUT_DEVICE_GROUP));
                                    break ;
                                case 0x5 :    
                                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ONBOARD_DEVICE_GROUP));
                                     break ;
                                case 0x6 :      
                                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_OTHER_DEVICE_GROUP));
                                    break;
                            
                                default:
                                    break;
                            }
                            
                            break;
                        }
                    }
                    
                    if (NULL != text)
                    {
                        token = HiiAddString (ctrlData->ControlData.ControlHandle, text);
                        UefiSetPromptField (ctrlData->ControlData.ControlPtr, token);
                    }
                    MemFreePointer( (VOID **)&text );
                                        
                    help = HiiGetString( gHiiHandle, STRING_TOKEN(STR_DEVICE_GROUP_HELP));
                    
                    if (NULL != help)
                    {
                        token = HiiAddString (ctrlData->ControlData.ControlHandle, help);
                        UefiSetHelpField (ctrlData->ControlData.ControlPtr, token);
                        ctrlData->ControlData.ControlHelp = token ;
                    }
                    
                    MemFreePointer( (VOID **)&help );
                }
				break;
			default:
				break;
		}
	}
}

/**
    Function to free memory for special controls

    @param UINT32 - CurrentPage index

    @retval VOID

**/
VOID FreeControlConditionPtr(UINT32 CurrentPage)
{
	PAGE_INFO *thisPage;
	UINT16 i=MAIN_FRAME,CurrentControl=0, Index=0;
	CONTROL_INFO *ControlInfo;
	CONTROL_DATA **ControlData;
	GUID_INFO *guidInfo,*TempguidInfo;
	UINT16 value;
	
	i = (UINT16) StyleFrameIndexOf(MAIN_FRAME) ;
	thisPage = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[CurrentPage]);
	
	//if the Current Page's control is a special control and it is matching with 
	//frame's control(showing in UI),free the particular control. 
	for(CurrentControl=0;CurrentControl < thisPage->PageControls.ControlCount;CurrentControl++)
	{
		guidInfo = (GUID_INFO *)NULL;
		ControlInfo = (CONTROL_INFO*)((UINT8 *)(gControlInfo) + thisPage->PageControls.ControlList[CurrentControl]);
		value = _SpecialGetValue( ControlInfo, &guidInfo );
		if(value == (UINT16)-1)
		{
			ControlData = ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[i]->ControlList;
			for(Index=0; Index < ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[i]->ControlCount; Index++,ControlData++)
			{
				if(ControlData ==NULL)
					break;
				if(((*ControlData)->ControlData.ControlIndex  == ControlInfo->ControlIndex) &&
						((*ControlData)->ControlData.PageIdIndex  == ControlInfo->PageIdIndex))
				{
					TempguidInfo = (GUID_INFO *)NULL;
					_SpecialGetValue(&(*ControlData)->ControlData,&TempguidInfo);
				
#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
					if(((*ControlData)->ControlData.ControlVariable == VARIABLE_ID_OEM_TSE_VAR) &&
							((TempguidInfo!=NULL) && (TempguidInfo->GuidKey == GUID_KEY_OEM_CONTROL))
							)
					{
						continue;	
					}
#endif					
					if( ((*ControlData)->ControlData.ControlVariable == VARIABLE_ID_LANGUAGE) ||
							((TempguidInfo!=NULL)&&(EfiCompareGuid(&guidInfo->GuidValue,&TempguidInfo->GuidValue)))
							)
					{
						SETUP_DEBUG_TSE ("\n[TSE] Freeing Special Control Ptr:%X",(*ControlData)->ControlData.ControlConditionalPtr);
						MemFreePointer( (VOID **)&(*ControlData)->ControlData.ControlConditionalPtr );
					}								
		
				}				
			}			
		}		
	}		
}
			


/**
    special fucntion to update the page controls

    @param CurrentPage 

    @retval VOID

**/
VOID SpecialUpdatePageControls(UINT32 CurrentPage)
{
	PAGE_INFO *thisPage;
    UINT16 i=MAIN_FRAME,j=0, k=0;
	INT16 Value=0;
    CONTROL_INFO *control;
	CONTROL_DATA **ControlData;
	CONTROL_INFO *ControlInfo;
    GUID_INFO *guidInfo;
    FRAME_DATA *fdata;
    BOOLEAN ExitUpdate=FALSE, SetFocus =TRUE;
	UINT32			tmpControlCount,tmpCurrentControl,tmpFirstVisibleCtrl;
	BOOLEAN IsSpecialControlAvailable  = FALSE;
	i = (UINT16) StyleFrameIndexOf(MAIN_FRAME) ;
	thisPage = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[CurrentPage]);
	// find if controls in page need to be exploded 
	// i=0;
    //while(i< thisPage->FrameCount )
	//{
      // for now search main frame only, if needed uncomment code above to do this for all frames in page
			//FreeControlConditionPtr(CurrentPage);
			
			for(j=0;j < thisPage->PageControls.ControlCount;j++)
			{
				control = (CONTROL_INFO*)((UINT8 *)(gControlInfo) + thisPage->PageControls.ControlList[j]);

				switch( _SpecialGetValue( control, &guidInfo ))
				{
				    case (UINT16)-2:
					     break;
				    default:
						// special case, need to reinitialize this frame
						//pdata = (PAGE_DATA*)gApp->PageList[CurrentPage];
						fdata = ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[i]; 
						//clist = ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[i]->ControlList;
						ControlData = ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[i]->ControlList;

						tmpControlCount = fdata->ControlCount;
						tmpCurrentControl = fdata->CurrentControl;
						tmpFirstVisibleCtrl = fdata->FirstVisibleCtrl;
	                    for ( k = 0; k < fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->ControlCount; k++, ControlData++ )
                            (*ControlData)->Methods->Destroy( *ControlData, TRUE );
						
                        fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->ControlCount =0;
					    fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->CurrentControl =0;
                        fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->NullCount =0;
						fdata->FirstVisibleCtrl = 0;

					    // loop for al acontrols to either add it "as is" or explode it
	                    for ( k = 0; k < thisPage->PageControls.ControlCount; k++  )
						{
                            ControlInfo = (CONTROL_INFO*)((UINT8 *)gControlInfo + (UINT32)thisPage->PageControls.ControlList[k]);
                            switch(Value=_SpecialExplodeControl(fdata,ControlInfo, &SetFocus))
							{
							case -1:
								break;
							default:
						        if( _SpecialAddControl(fdata,ControlInfo, &SetFocus ) == EFI_SUCCESS)
                                    _SpecialControlExtras((fdata->ControlList[fdata->ControlCount-1])); 
	                           break;
							}

						}
//	To retains the Control Focues in page with has special control
// 	If the control count is different the we will focus to first control as we don't know 
// 	What is added or missing.
						if(fdata->ControlCount == tmpControlCount)
						{
							fdata->CurrentControl=tmpCurrentControl;
							fdata->FirstVisibleCtrl=tmpFirstVisibleCtrl;
							_FrameSetControlFocus( fdata, tmpCurrentControl );
						}
						IsSpecialControlAvailable = TRUE;
						ExitUpdate =TRUE;
					     break;
				}
				if(ExitUpdate)
					break;
			}
    //}
			
			if(FALSE == IsSpecialControlAvailable)
			{
				if(IsRefXControlInCurrentPage(CurrentPage))/* Update frame controls even if a page has Ref3/4/5 controls.*/
				{
					UpdateFrameControlDataForCurrentPage(CurrentPage,(UINT16) StyleFrameIndexOf(MAIN_FRAME));
				}
			}			
			
			
}
//#endif

/**
    special fucntion to update the Frame controls

    @param CurrentPage, CurrentFrame

    @retval VOID

**/

VOID UpdateFrameControlDataForCurrentPage(UINT32 CurrentPage, UINT16 CurrentFrame)
{
	PAGE_INFO *thisPage = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[CurrentPage]);
	CONTROL_DATA **ControlData;
	CONTROL_INFO *ControlInfo;
    FRAME_DATA *fdata;
    BOOLEAN SetFocus =TRUE;
    UINT32	tmpControlCount,tmpCurrentControl,tmpFirstVisibleCtrl,k;
    INT16 Value=0;
    
    // special case, need to reinitialize this frame
	//pdata = (PAGE_DATA*)gApp->PageList[CurrentPage];
	fdata = ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[CurrentFrame]; 
	//clist = ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[i]->ControlList;
	ControlData = ((PAGE_DATA*)gApp->PageList[CurrentPage])->FrameList[CurrentFrame]->ControlList;

	tmpControlCount = fdata->ControlCount;
	tmpCurrentControl = fdata->CurrentControl;
	tmpFirstVisibleCtrl = fdata->FirstVisibleCtrl;
    for ( k = 0; k < fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->ControlCount; k++, ControlData++ )
        (*ControlData)->Methods->Destroy( *ControlData, TRUE );
	
    fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->ControlCount =0;
    fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->CurrentControl =0;
    fdata/*gApp->PageList[CurrentPage]->FrameList[i]*/->NullCount =0;
	fdata->FirstVisibleCtrl = 0;

    // loop for al acontrols to either add it "as is" or explode it
    for ( k = 0; k < thisPage->PageControls.ControlCount; k++  )
	{
        ControlInfo = (CONTROL_INFO*)((UINT8 *)gControlInfo + (UINT32)thisPage->PageControls.ControlList[k]);
        switch(Value=_SpecialExplodeControl(fdata,ControlInfo, &SetFocus))
		{
		case -1:
			break;
		default:
	        if( _SpecialAddControl(fdata,ControlInfo, &SetFocus ) == EFI_SUCCESS)
                _SpecialControlExtras((fdata->ControlList[fdata->ControlCount-1])); 
           break;
		}

	}
//	To retains the Control Focues in page with has special control
// 	If the control count is different the we will focus to first control as we don't know 
// 	What is added or missing.
	if(fdata->ControlCount == tmpControlCount)
	{
		fdata->CurrentControl=tmpCurrentControl;
		fdata->FirstVisibleCtrl=tmpFirstVisibleCtrl;
		_FrameSetControlFocus( fdata, tmpCurrentControl );
	}
}

/**
    fucntion to find whether the current page has Ref3/4/5 controls.

    @param PageId

    @retval BOOLEAN

**/

BOOLEAN IsRefXControlInCurrentPage(UINTN PageId)
{
	PAGE_INFO* PageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[PageId]);
	UINTN i ;
	
	for (i = 0; i < PageInfo->PageControls.ControlCount ; i++ )
	{
		CONTROL_INFO *CtrlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + PageInfo->PageControls.ControlList[i]);
		
		if(CtrlInfo == NULL || CtrlInfo->ControlHandle == NULL || CtrlInfo->ControlPtr == NULL)
			continue;
		if(((EFI_IFR_OP_HEADER*)CtrlInfo->ControlPtr)->OpCode != EFI_IFR_REF_OP)
			continue;
		
		if(((EFI_IFR_OP_HEADER*)CtrlInfo->ControlPtr)->Length == sizeof(AMI_EFI_IFR_REF5) || ((EFI_IFR_OP_HEADER*)CtrlInfo->ControlPtr)->Length >= sizeof(EFI_IFR_REF3))
			return TRUE;
	}
	
	return FALSE;	
}

/**
    Takes backup of TseAdvanced global variables into Tse global buffer.

    @param TSE_SETUP_GLOBAL_DATA*

    @retval VOID

**/
EFI_STATUS SaveTseAdvancedGlobalData(VOID *temp)
{
	TSE_SETUP_GLOBAL_DATA* Data = temp;
	
	Data->BootOptionData.BootOptionTokenArray = gBootOptionTokenArray;
	gBootOptionTokenArray = (UINT16*)NULL;

	Data->BootOptionData.BootOptionTokenCount = gBootOptionTokenCount;
	gBootOptionTokenCount = 0;
	
	Data->BootOptionData.DriverOptionTokenArray = gDriverOptionTokenArray;
	gDriverOptionTokenArray = (UINT16*)NULL;

	Data->BootOptionData.DriverOptionTokenCount = gDriverOptionTokenCount;
	gDriverOptionTokenCount = 0;
	
	Data->tempCurrentPage = gtempCurrentPage;
	gtempCurrentPage = 0;
	
	return EFI_SUCCESS;
}

/**
    restores TseAdvanced global variables from Tse global buffer.

    @param TSE_SETUP_GLOBAL_DATA*

    @retval VOID

**/
VOID RestoreTseAdvancedGlobalData(VOID *temp)
{
	TSE_SETUP_GLOBAL_DATA *Data = temp;
	
	gBootOptionTokenArray = Data->BootOptionData.BootOptionTokenArray;

	gBootOptionTokenCount = Data->BootOptionData.BootOptionTokenCount;

	gDriverOptionTokenArray = Data->BootOptionData.DriverOptionTokenArray;

	gDriverOptionTokenCount = Data->BootOptionData.DriverOptionTokenCount;

	gtempCurrentPage = Data->tempCurrentPage;

}
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**           5555 Oakbrook Pkwy, Norcross, Georgia 30093       **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
