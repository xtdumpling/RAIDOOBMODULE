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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/TseAdvanced.c $
//
// $Author: Rajashakerg $
//
// $Revision: 43 $
//
// $Date: 5/28/12 6:23a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file TseAdvanced.c
    This file contains code to handle the Advanced TSE Operations

**/

#include "minisetup.h"
#include "special.h"  
#include "TseElinks.h"

//////////////////////////////////////////////////
/////// INTERNAL VARIABLE DECLARATION
//////////////////////////////////////////////////
///Exit Page options info. from Elink...
extern EXIT_PAGE_OPTIONS gExitPageOptions[END_OF_EXIT_PAGE_OPTION_LIST];
extern APPLICATION_DATA *gApp;
extern UINTN gSaveUserMsgBox;
extern UINTN gLoadUserMsgBox;
extern UINTN gBootLaunchFailedMsgBox;
extern UINTN gInvalidPasswordFailMsgBox;
extern UINT16   gCurrDynamicPageGroup ;
extern DYNAMIC_PAGE_GROUP *gDynamicPageGroup ;
extern UINTN gSaveMsgBox;

extern BOOLEAN _SaveValues( UINTN data );

#define AMITSE_PRINT_SCREEN_PRESS_GUID \
{ 0x71030b81, 0xf72, 0x4b48, 0xb5, 0xe8, 0xbc, 0xcd, 0x51, 0x78, 0x60, 0x9e }


#define	CONSTRUCT_DEFAULTS_FIRST_BOOT_GUID	\
{ 0xc5912ed9, 0x83c2, 0x4bff, 0x99, 0x36, 0x23, 0x1f, 0xeb, 0x85, 0xf3, 0xe8 }

MSGBOX_DATA *gInfoBox = NULL;
UINT32			gtempCurrentPage;
BOOLEAN TimerEnabled;
//////////////////////////////////////////////////
/////// INTERNAL FUNCTIONS DECLARATION
//////////////////////////////////////////////////
///Exit page options handling using Elinks...
VOID HandleSaveAndExit(VOID);
VOID HandleExitApplication(VOID);
VOID HandleSaveAndReset(VOID);
VOID HandleResetSys(VOID);
VOID HandleSaveWithoutExit(VOID);
VOID HandleLoadPreviousValues(VOID);
VOID HandleLoadOptimalDefaults(VOID);
VOID HandleSaveUserDefaults(VOID);
VOID HandleRestoreUserDefaults(VOID);
VOID HandleBootAddBootOption(VOID);
VOID HandleDriverAddDriverOption(VOID);

VOID HandleLaunchFileSystem(VOID);
VOID HandleLaunchFileSystemDriver(VOID);

VOID SaveUserDefaults( VOID );
VOID LoadUserDefaults( VOID );
BOOLEAN CheckIsAllowedPasswordChar(CHAR16 Char);
BOOLEAN CheckIsAllowedChar(CHAR16 Char);
BOOLEAN  IsTSECursorSupport (VOID) ;

//////////////////////////////////////////////////
/////// EXTERN FUNCTIONS DECLARATION
//////////////////////////////////////////////////
extern VOID 			*UefiCreateStringTemplate(UINT16 Token);
extern EFI_STATUS 	ReadImageResource(EFI_HANDLE ImageHandle, EFI_GUID *pGuid, VOID **ppData, UINTN *pDataSize);
extern EFI_STATUS 	FvReadResources (EFI_GUID *FileGuid, EFI_GUID *pSectionGuid, VOID **pSectionData, UINTN *pSectionSize);
extern BOOLEAN   		IsLoadSysteAccessOnUserDefault(VOID);
extern BOOLEAN   		IsSupportDefaultForStringControl (VOID);
extern EFI_STATUS 	GetSetDefaultsFromControlPtr(NVRAM_VARIABLE *optimalDefaultlist, NVRAM_VARIABLE *failSafeDefaultlist);
extern BOOLEAN   		IsWaitForKeyEventSupport (VOID);

UINTN 	GetANSIEscapeCode(CHAR16 *String,UINT8 *Bold,UINT8 *Foreground, UINT8 *Background);
VOID 		OEMSpecialGotoSelect(UINT16 value);
BOOLEAN 	IsShowPromptStringAsTitle(VOID);

typedef UINT16 VAR_SIZE_TYPE;
#pragma pack(push)
#pragma pack(1)
typedef struct{
	UINT32 signature;
	VAR_SIZE_TYPE size;
	UINT32 next:24;
	UINT32 flags:8;
//  guid and name are there only if NVRAM_FLAG_DATA_ONLY is not set
//	UINT8 guid; 
//	CHAR8 or CHAR16 name[...];
//  UINT8 data[...];
//  if NVRAM_FLAG_EXT_HEDER is set
//  UINT8 extflags;
//  UINT8 extdata[...];
//  VAR_SIZE_TYPE extsize;
}NVAR;
#pragma pack(pop)

typedef struct _NVRAM_STORE_INTERFACE NVRAM_STORE_INTERFACE;
typedef struct {
    UINT8 *NvramAddress;
    UINTN NvramSize;
    EFI_GUID* NvramGuidsAddress;
    UINT8 *pEndOfVars, *pFirstVar;
    INT16 NextGuid;
    VAR_SIZE_TYPE LastVarSize;
    NVAR *pLastReturned;
    UINT8 Flags;
    UINT32 HeaderLength;
    NVRAM_STORE_INTERFACE *Interface;
} NVRAM_STORE_INFO;

VOID NvInitInfoBuffer(
    IN NVRAM_STORE_INFO *pInfo, UINTN HeaderSize, UINT8 Flags,
    NVRAM_STORE_INTERFACE *Interface
);
EFI_STATUS NvGetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data,
    IN NVRAM_STORE_INFO *pInfo, OUT UINT8 *Flags
);
BOOLEAN IsTseLoadPasswordOnDefaults();

#define NVRAM_STORE_FLAG_NON_VALATILE 1

#define DEFAULTS_GUID {0x4599d26f, 0x1a11, 0x49b8, 0xb9, 0x1f, 0x85, 0x87, 0x45, 0xcf, 0xf8, 0x24}

EFI_GUID DefaultsGuid = DEFAULTS_GUID;
VOID SetDrvHealthHandleAndCtrlCount (UINT16);
VOID DoDriverHealthOperation (CONTROL_INFO *, UINT16);
BOOLEAN   FlushKeysAfterRead(void);
EFI_STATUS 	ShowBootOverrideSelectOption (VOID);
BOOLEAN  	IsBootOverrideBootNextSupport (VOID);
#define BOOT_OVERRIDE_OPTION_MENU_COUNT 3

#ifndef KEY_STATE_EXPOSED
#define KEY_STATE_EXPOSED   0x40  
#endif

BOOLEAN DefaultSetupPwdAtFirstBootOnly (void);
BOOLEAN TseDefaultSetupPasswordSupported(VOID);
EFI_STATUS GetDefaultPassword (UINT32 PasswordInstalled, void **DefaultPassword);
BOOLEAN   IsIsolateSetupDataSupported (VOID);

BOOLEAN TseLiteIsSpecialOptionList(CONTROL_DATA *ControlData)
{
	// For TSE Advanced we papulate and add the control dynamically.
	// So it is not TSE lite Special control
	// TSE Lite special controls are patched instad of removing and adding.
	return FALSE;
}


/**
    Function to update the boot/driver order

    @param buffer 
    @param offset 
    @param size 
    @param realBuffer 

    @retval VOID

**/
VOID BootUpdateOrder(UINT16 *buffer,UINT32 *offset,UINTN *size, VOID **realBuffer)
{
	UINTN i, j, optionNumber = *offset / sizeof(UINT16);
	UINT16 index = *(UINT16 *)buffer;
	UINT16 newOption = index;
	UINT16 oldOption, *optionList;

#if TSE_SEPERATE_EFI_LEGACY_OPTIONS
	UINT16 *OrgOptionList;
	OrgOptionList = EfiLibAllocateZeroPool( *size );
	MemCopy( OrgOptionList, gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer, *size );
#endif

	*realBuffer = EfiLibAllocateZeroPool( *size );
	if (NULL == *realBuffer)
	{
		return;
	}
	*offset = 0;
	MemCopy( *realBuffer, gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer, *size );

	optionList = (UINT16 *)*realBuffer;
	oldOption = optionList[ optionNumber ];

	if ( newOption == oldOption )
		return;

    if(DISABLED_BOOT_OPTION == newOption)
    {
        for (i = optionNumber, j = 0; i < gBootOptionCount; i++)
        {
            if ((i+1) < gBootOptionCount)
			{
				if ( (CheckHiddenforBootDriverOption (optionList [i + 1], BOOT_ORDER_OPTION))  && gLoadOptionHidden )
				{
					j ++;
					continue;
				}
                optionList [i] = optionList [i + 1];
			}
            else
                optionList [i-j] = newOption; 	// or DISABLED_BOOT_OPTION; boot order will be like active boot option
        }										//then hidden options, so hidden option will always maintain at same place(last positions).
    }
    else if (DISABLED_BOOT_OPTION == oldOption)
    {
        //Find newOption's position
        for(i=0;i<gBootOptionCount;i++)
        {
            if(newOption == optionList[i])
                break;
        }

        if(i < gBootOptionCount)//New option found
        {
            //Move new option to the last of enabled devices
            for(j=i; j<gBootOptionCount; j++)
            {
                if( DISABLED_BOOT_OPTION != optionList[j+1])
                    optionList[j] = optionList[j+1];
                else
                {
                    optionList[j] = newOption;
                    break;
                }
            }
        }
        else//New option not enabled
        {
            //Replace first disabled with new one
            for(i=0;i<gBootOptionCount;i++)
            {
                if(DISABLED_BOOT_OPTION == optionList[i])
                    break;
            }
            optionList[i] = newOption;
        }
#if TSE_SEPERATE_EFI_LEGACY_OPTIONS
        ReGroupOptions(optionList,NULL);
#endif
    }
    else
    {
        optionList[optionNumber] = newOption;
        for(i=0;i<gBootOptionCount;i++)
        {
            if(i == optionNumber)
                continue;
            if(newOption == optionList[i])
            {
                optionList[i] = oldOption;
                break;
            }
        }
#if TSE_SEPERATE_EFI_LEGACY_OPTIONS
        ReGroupOptions(optionList,OrgOptionList);
#endif
    }
#if TSE_SEPERATE_EFI_LEGACY_OPTIONS
	MemFreePointer( (VOID **)&OrgOptionList );
#endif
}
/**
    Function to update the driver order

    @param buffer 
    @param offset 
    @param size 
    @param realBuffer 

    @retval VOID

**/
VOID DriverUpdateOrder (UINT16 *buffer,UINT32 *offset,UINTN *size, VOID **realBuffer)
{
	UINTN i, j, optionNumber = *offset / sizeof(UINT16);
	UINT16 index = *(UINT16 *)buffer;
	UINT16 newOption = index;
	UINT16 oldOption, *optionList;

	*realBuffer = EfiLibAllocateZeroPool (*size);
	if (NULL == *realBuffer)
	{
		return;
	}
	*offset = 0;

	MemCopy (*realBuffer, gVariableList [VARIABLE_ID_DRIVER_ORDER].Buffer, *size);
	optionList = (UINT16 *)*realBuffer;
	oldOption = optionList [optionNumber];

	if (newOption == oldOption)
		return;

    if (DISABLED_BOOT_OPTION == newOption)
    {
        for (i = optionNumber, j = 0; i < gDriverOptionCount; i++)
        {
            if ((i+1) < gDriverOptionCount)
			{
				if ( (CheckHiddenforBootDriverOption (optionList [i + 1], DRIVER_ORDER_OPTION))  && gLoadOptionHidden )
				{
					j ++;
					continue;
				}
                optionList [i] = optionList [i + 1];
			}
            else
                optionList [i-j] = newOption; 	// or DISABLED_BOOT_OPTION; boot order will be like active boot option
        }										//then hidden options, so hidden option will always maintain at same place(last positions).
    }
    else if (DISABLED_BOOT_OPTION == oldOption)
    {
        //Find newOption's position
        for (i = 0; i < gDriverOptionCount; i ++)
        {
            if (newOption == optionList [i])
                break;
        }
        if (i < gDriverOptionCount)//New option found
        {
            //Move new option to the last of enabled devices
            for (j = i; j < gDriverOptionCount; j ++)
            {
                if (DISABLED_BOOT_OPTION != optionList [j+1])
                    optionList [j] = optionList [j+1];
                else
                {
                    optionList [j] = newOption;
                    break;
                }
            }
        }
        else//New option not enabled
        {
            //Replace first disabled with new one
            for (i = 0; i < gDriverOptionCount; i ++)
            {
                if (DISABLED_BOOT_OPTION == optionList [i])
                    break;
            }
            optionList [i] = newOption;
        }
    }
    else
    {
        optionList[optionNumber] = newOption;
        for (i = 0; i < gDriverOptionCount; i ++)
        {
            if(i == optionNumber)
                continue;
            if(newOption == optionList[i])
            {
                optionList[i] = oldOption;
                break;
            }
        }
    }
}

/// functions to handle the exit page options
VOID HandleSaveAndExit()
{
	gApp->Quit = SaveAndExit();
}

VOID HandleExitApplication()
{
	gApp->Quit = ExitApplication();
}

VOID HandleSaveAndReset()
{
	gApp->Quit = SaveAndReset();
}

VOID HandleResetSys()
{
	gApp->Quit = ResetSys();
}

VOID HandleSaveWithoutExit()
{
	SaveWithoutExit();
	gUserTseCacheUpdated = FALSE;		//Already modification saved in NVRAM, No need to keep the flag true
}

VOID HandleLoadPreviousValues()
{
	LoadPreviousValues( TRUE );
	gUserTseCacheUpdated = FALSE;		//Modifications reverted back to previous values, No need to keep the flag true
}

VOID HandleLoadOptimalDefaults()
{
	LoadOptimalDefaults();
}

VOID HandleSaveUserDefaults()
{
	SaveUserDefaults();
}

VOID HandleRestoreUserDefaults(VOID)
{
	LoadUserDefaults();
}

VOID HandleBootAddBootOption(VOID)
{
	TseBootAddBootOption();
}

VOID HandleDriverAddDriverOption(VOID)
{
	TseDriverAddDriverOption();
}

// Function to handle selection of the file system
VOID HandleLaunchFileSystem(VOID)
{
	TseLaunchFileSystem(VARIABLE_ID_ADD_BOOT_OPTION);
}

VOID HandleLaunchFileSystemDriver(VOID)
{
	TseLaunchFileSystem(VARIABLE_ID_ADD_DRIVER_OPTION);
}

/**
    Function implemented to handle the Exit page 
    options using Elinks.

    @param value index of the exit page option

    @retval TRUE

**/
BOOLEAN _SubMenuAmiCallback( UINT16 value )
{
	UINT8 idx=0;

	for(idx=0;idx<(sizeof(gExitPageOptions)/sizeof(EXIT_PAGE_OPTIONS));idx++)
	{
		if( gExitPageOptions[idx].OptIdx == END_OF_EXIT_PAGE_OPTION_LIST ) {
			break;
		}
		else if(value == gExitPageOptions[idx].OptIdx) {
			gExitPageOptions[idx].ExitPageCallBack();
			break;
		}
	}

	return TRUE;
}

/**
    Sub menu handle for Special operations.

    @param Submenu data

    @retval Bool Interpreted by the Caller.

**/
BOOLEAN _SubMenuHandleSpecialOp( SUBMENU_DATA *submenu )
{
	BOOLEAN status = FALSE;
	BOOLEAN BootLaunchFailed = FALSE;
	BOOLEAN StyleExitDone = FALSE;
	UINT16 value;
	GUID_INFO *guidInfo;
	EFI_STATUS Status;

	VOID *ScreenBuf;


	value = SpecialGetValue( (CONTROL_DATA *)submenu, &guidInfo );
	if ( value != (UINT16)-2 )
	{
		if ( guidInfo->GuidFlags.GuidLaunchCallback )
		{
			switch ( guidInfo->GuidKey )
			{
				case GUID_KEY_BOOT_NOW:
					if ( !gResetRequired && IsBootOverrideBootNextSupport () )
					{
						UINT16 *pBootNext = NULL;
						UINTN size = 0;
						pBootNext = (UINT16 *)VarGetNvramName(L"BootNext", &gEfiGlobalVariableGuid, NULL, &size);
						if ( pBootNext && (size == sizeof(UINT16)))//If BootNext Variable is present
						{					
							Status = ShowBootOverrideSelectOption ();//To show Override menu option for BootNext variable
							if ( Status != EFI_SUCCESS )//Exit menu and retain in override menu
							{
								status = TRUE;
								break;
							}
						}
						if(pBootNext)
	        				MemFreePointer((void **) &pBootNext);
					}
					if(!gResetRequired)
					{
                        if(!gDoNotBoot)
                        {
                        	if (!gUserTseCacheUpdated || _SaveValues( (UINTN)gSaveMsgBox )) 	//Check as Setup question is modified or not
                        	{
                        		gUserTseCacheUpdated = FALSE;
								gApp->ClearScreen = TRUE;
								StyleExit();
								if (gST->ConOut != NULL)
								{
									//Disable cursor, set desired attributes and clear screen
									gST->ConOut->EnableCursor( gST->ConOut, FALSE );
									gST->ConOut->SetAttribute( gST->ConOut, (EFI_BACKGROUND_BLACK | EFI_WHITE));
									gST->ConOut->ClearScreen( gST->ConOut);
								}
							
								BootLaunchBootNow( value, gShowAllBbsDev/*SETUP_SHOW_ALL_BBS_DEVICES*/ );
								StyleInit();
                        	}
                        }
					}
					else
					{
						gApp->Quit = SaveAndReset();
					}
					status = TRUE;
					break;

				case GUID_KEY_AMI_CALLBACK:
					status = _SubMenuAmiCallback( value );
					break;

				case GUID_KEY_ERROR_MANAGER:
					{
					UefiSpecialGuidCallback(submenu->ControlData.ControlHandle, value, &guidInfo->GuidValue);
					}
					break;
#if SETUP_OEM_SPECIAL_CONTROL_SUPPORT
		        case GUID_KEY_OEM_CONTROL:
					{
						OEMSpecialGotoSelect(value);
						break;
					}
#endif
                case GUID_KEY_IDE_SECURITY:
                    {
                        UINTN size;
                        IDE_SECURITY_CONFIG *ideSecConfig;


                        //Update mem copy of the IDE sec variable
                        size = 0;
                        ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );
                        if (NULL == ideSecConfig) {
                            break;
                        }

                        TSEIDEUpdateConfig(ideSecConfig, value);

                        VarSetValue( VARIABLE_ID_IDE_SECURITY, 0, size, ideSecConfig );
                        MemFreePointer((VOID **)&ideSecConfig);
                    }
                break;
                case GUID_KEY_BBS_ORDER:
                {
                    BBSUpdateBootData(value);
                    VarSetValue(VARIABLE_ID_LEGACY_GROUP_INFO, 0, sizeof(UINT16), &(gCurrLegacyBootData->LegacyDevCount));
                }
                break;
				case GUID_KEY_DRIVER_HEALTH:
				{
					SetDrvHealthHandleAndCtrlCount (value);
					break;
				}
				case GUID_KEY_DRV_HEALTH_CTRL_COUNT:
				{
					DoDriverHealthOperation ((VOID *)&((CONTROL_DATA *)submenu)->ControlData, value);
					break;	
				}
				case GUID_KEY_DYNAMIC_PAGE_GROUP:
                {
                    UINT16 kIndex = 0, jIndex = 0, count = 0 ;
                    EFI_GUID DynamicPageDeviceGuid = DYNAMIC_PAGE_DEVICE_COUNT_GUID;
                    
                    gCurrDynamicPageGroup = 0xFFFF ;

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
                                case 0x0 : gCurrDynamicPageGroup = EFI_NON_DEVICE_CLASS ; break ;
                                case 0x1 : gCurrDynamicPageGroup = EFI_DISK_DEVICE_CLASS ; break ;
                                case 0x2 : gCurrDynamicPageGroup = EFI_VIDEO_DEVICE_CLASS ; break ;
                                case 0x3 : gCurrDynamicPageGroup = EFI_NETWORK_DEVICE_CLASS ; break ;
                                case 0x4 : gCurrDynamicPageGroup = EFI_INPUT_DEVICE_CLASS ; break ;       
                                case 0x5 : gCurrDynamicPageGroup = EFI_ON_BOARD_DEVICE_CLASS ; break ; 
                                case 0x6 : gCurrDynamicPageGroup = EFI_OTHER_DEVICE_CLASS ; break ; 
                            
                                default:
                                    break;
                            }
                            
                            count = gDynamicPageGroup [jIndex].Count ;

                            VarSetNvramName( L"DynamicPageDeviceCount", &DynamicPageDeviceGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &count, sizeof(count) );
                            VarUpdateVariable(VARIABLE_ID_DYNAMIC_PAGE_DEVICE);   //Update nvram cache for DynamicPageDeviceCount
                            break;
                        }
                    }
                }
                break;
				default:
					break;
			}
		}
		else
		{
			EFI_STATUS Status = EFI_UNSUPPORTED;
			
			//Save the screen buffer to restore later if required
			ScreenBuf = SaveScreen();
			
			if ( guidInfo->GuidFlags.GuidLaunchGuid )
			{
				if(!gResetRequired)
				{
					if(!gDoNotBoot)
					{
						if (!gUserTseCacheUpdated || _SaveValues( (UINTN)gSaveMsgBox )) 	//Check as Setup question is modified or not
						{
							gUserTseCacheUpdated = FALSE;
							gApp->ClearScreen = TRUE;
							StyleExit();
							if (gST->ConOut != NULL)
							{
								//Disable cursor, set desired attributes and clear screen
								gST->ConOut->EnableCursor( gST->ConOut, FALSE );
								gST->ConOut->SetAttribute( gST->ConOut, (EFI_BACKGROUND_BLACK | EFI_WHITE));
								gST->ConOut->ClearScreen( gST->ConOut);
							}
							StyleExitDone = TRUE;				
							Status = BootLaunchGuid( &guidInfo->GuidValue );
							if ( ! EFI_ERROR( Status ) )
							{
								gApp->ClearScreen = TRUE;
								status = TRUE;
							}
							else
								BootLaunchFailed = TRUE;
						}
					}
				}
				else
				{
					gApp->Quit = SaveAndReset();
					status = TRUE;
				}
			}
			if (( guidInfo->GuidFlags.GuidLaunchFilename ) && (FALSE == status))
			{
				if(!gResetRequired)
				{
					if(!gDoNotBoot)
     				{
						if (!gUserTseCacheUpdated || _SaveValues( (UINTN)gSaveMsgBox )) 	//Check as Setup question is modified or not
						{
							CHAR16 FileName[VARIABLE_NAME_LENGTH];
							UINTN i = 0, j;
							
							gUserTseCacheUpdated = FALSE;
							
							while(guidInfo->GuidName[i])
							{
								j=0;
								for(;guidInfo->GuidName[i];i++)
								{
									if(L'|' == guidInfo->GuidName[i])
									{
										i++;
										break;
									}
									FileName[j] = guidInfo->GuidName[i];
									j++;
								}
								FileName[j] = 0;
								if(!StyleExitDone){		
									gApp->ClearScreen = TRUE;
									
									StyleExit();
									if (gST->ConOut != NULL)
									{
										//Disable cursor, set desired attributes and clear screen
										gST->ConOut->EnableCursor( gST->ConOut, FALSE );
										gST->ConOut->SetAttribute( gST->ConOut, (EFI_BACKGROUND_BLACK | EFI_WHITE));
										gST->ConOut->ClearScreen( gST->ConOut);
									}
				
									StyleExitDone = TRUE;
								}  					
								Status = BootLaunchFilename( FileName );
								if ( ! EFI_ERROR( Status ) )
								{
									gApp->ClearScreen = TRUE;
									status = TRUE;
									BootLaunchFailed = FALSE;
									break;
								}else
									BootLaunchFailed = TRUE;
							}
						//BootLaunchFailed = TRUE;
						}
     				}
				}
				else {
					gApp->Quit = SaveAndReset();
					status = TRUE;
				}
			}

			if(StyleExitDone)
				StyleInit();

			if ( BootLaunchFailed ){
				
				//Restore the Original screen before boot launch 
				RestoreScreen( ScreenBuf );
				CallbackShowMessageBox( (UINTN)gBootLaunchFailedMsgBox, MSGBOX_TYPE_OK );
			}
			else {
				//In case Boot Launch not failed, free the saved screen buffer
				MemFreePointer((VOID **) &ScreenBuf);
			}
		}
	}
	return status;
}

#if TSE_BUILD_AS_APPLICATION || TSE_FOR_EDKII_SUPPORT
#define	SETUP_VARIABLE_GUID	\
	{ 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } }


static SETUP_PKG _SetupPackageTemplate = {
		{'$', 'S', 'P', 'F'}, //PackageSignature[4];
		0x0102, 			//StructureRevision;
		0x100,				//PackageVersion;
		{0,0,0,0,0,0},		//Reserved[6];
		SETUP_VARIABLE_GUID, //PackageGuid;
		0, 					//PackageVariable;
		0, 					//PackageScreen;
		0, 					//PackagePageList;
		0, 					//PackageControlOffset;
		0, 					//PackageVariableList;
		0, 					//PackageHpkData;
		0, 					//PackageGuidList;
		0, 					//PackagePageIdList;
		0 					//PackageIfrData;
};

static SCREEN_INFO _SetupPackageScreen = {
	{0},					//MainHandle;
	STR_MAIN_TITLE,		//MainTitle;
	STR_MAIN_COPYRIGHT, //MainCopyright;
	STR_EVAL_MSG,		//MainHelpTitle;
	0,					//MainMenuCount;
	0					//MainNavCount;
};


static PAGE_INFO _SetupPageInfo =
{
		{0},			//		PageHandle;
		0,			//		PageIdIndex;
		0,			//		PageFormID;
		0,			//		PageTitle;
		0,			//		PageSubTitle;
		0,			//		PageID;
		0,			//		PageParentID;
		{0},			//		PAGE_FLAGS		PageFlags;
		0,			//		PageVariable;
		{0},		//		PageControls;
};

static CONTROL_INFO _SetupControlInfo_N_N = {
		{0},			//	EFI_HII_HANDLE	ControlHandle;
		0,			//	UINT32			ControlVariable;
		{0},			//	UINT32			ControlConditionalVariable[2];
		0,			//	UINT16			ControlType;
		0,			//	UINT16			ControlPageID;
		0xFFFF,			//	UINT16			ControlDestPageID;
		{0},		//	CONTROL_FLAGS	ControlFlags;
		0,			//	UINT16			ControlHelp;
		0,			//	UINT16			ControlLabel;
		0,			//	UINT16			ControlIndex;
		0,			//	UINT16			ControlLabelCount;
		{0},			//	union {
					//	UINTN			ControlPtr;
					//	UINT64	res1;
					//	};
		{0},			//	union {
					//	INTN			ControlConditionalPtr;
					//	UINT64	res2;
					//	};A
		0,			//	UINT16			ControlDataLength;
		0,			//	UINT16			ControlDataWidth;
		0			//	UINT16			QuestionId;
};

#define _VARIABLE_COUNT		58
#define _GUID_COUNT			26

static VARIABLE_INFO _SetupVariableInfo[_VARIABLE_COUNT] = {
	{SETUP_VARIABLE_GUID , 						L"Setup",						0x07,0},			//0
	{EFI_GLOBAL_VARIABLE_GUID , 					L"PlatformLang",				0x17,0},			//1
	{EFI_GLOBAL_VARIABLE_GUID , 					L"Timeout",						0x07,0},			//2
	{USER_DEFAULTS_GUID , 						L"UserDefaults",				0x07,0},			//3
	{ERROR_MANAGER_GUID , 						L"ErrorManager",				0x07,0},			//4
	{AMITSESETUP_GUID , 							L"AMITSESetup",				0x07,0},			//5
	{IDE_SECURITY_CONFIG_GUID , 					L"IDESecDev",					0x07,0},			//6
	{EFI_GLOBAL_VARIABLE_GUID , 					L"BootOrder",					0x17,0},			//7
	{LEGACY_DEV_ORDER_GUID , 					L"LegacyDevOrder",			0x17,0},			//8
	{DEL_BOOT_OPTION_GUID , 						L"DelBootOption",				0x17,0},			//9
	{ADD_BOOT_OPTION_GUID , 						L"AddBootOption",				0x17,0},			//10
	{BOOT_MANAGER_GUID , 							L"BootManager",				0x07,0},			//11
	{BOOT_NOW_COUNT_GUID , 						L"BootNowCount",				0x07,0},			//12
	{LEGACY_DEV_ORDER_GUID , 					L"LegacyDev",					0x01,0},			//13
	{AMI_CALLBACK_GUID , 							L"AMICallback",				0x01,0},			//14
	{LEGACY_DEV_ORDER_GUID , 					L"LegacyGroup",				0x01,0},			//15
	{BOOT_NOW_COUNT_GUID , 						L"SetupSaveState",				0x01,0},			//16
	{OEM_TSE_VAR_GUID , 							L"OemTSEVar",				0x17,0},			//17
	{DYNAMIC_PAGE_COUNT_GUID , 					L"DynamicPageCount",			0x17,0},			//18
	{AMITSE_DRIVER_HEALTH_ENB_GUID , 			L"DriverHlthEnable",			0x17,0},			//19
	{AMITSE_DRIVER_HEALTH_GUID , 				L"DriverHealthCount",			0x17,0},			//20
	{AMITSE_DRIVER_HEALTH_CTRL_GUID , 		L"DrvHealthCtrlCnt",				0x17,0},			//21
	{DRIVER_MANAGER_GUID , 						L"DriverManager",				0x17,0},			//22
	{EFI_GLOBAL_VARIABLE_GUID , 					L"DriverOrder",					0x17,0},			//23
	{ADD_DRIVER_OPTION_GUID , 					L"AddDriverOption",			0x17,0},		//24
	{DEL_DRIVER_OPTION_GUID , 					L"DelDriverOption",				0x17,0},		//25
	{OEM_PORT_VARIABEL1 , 						L"OemPortVar1",				0x17,0},			//26
	{OEM_PORT_VARIABEL2 , 						L"OemPortVar2",				0x17,0},			//27
	{OEM_PORT_VARIABEL3 , 						L"OemPortVar3",				0x02,0},			//28
	{OEM_PORT_VARIABEL4 , 						L"OemPortVar4",				0x02,0},			//29
	{OEM_PORT_VARIABEL5 , 						L"OemPortVar5",				0x02,0},			//30
	{OEM_PORT_VARIABEL6 , 						L"OemPortVar6",				0x02,0},			//31
	{OEM_PORT_VARIABEL7 , 						L"OemPortVar7",				0x02,0},			//32
	{OEM_PORT_VARIABEL8 , 						L"OemPortVar8",				0x02,0},			//33
	{OEM_PORT_VARIABEL9 , 						L"OemPortVar9",				0x02,0},			//34
	{OEM_PORT_VARIABEL10 , 						L"OemPortVar10",				0x02,0},			//35
	{OEM_PORT_VARIABEL11 , 						L"OemPortVar11",				0x02,0},			//36
	{OEM_PORT_VARIABEL12 , 						L"OemPortVar12",				0x02,0},			//37
	{OEM_PORT_VARIABEL13 , 						L"OemPortVar13",				0x02,0},			//38
	{OEM_PORT_VARIABEL14 , 						L"OemPortVar14",				0x02,0},			//39
	{OEM_PORT_VARIABEL15 , 						L"OemPortVar15",				0x02,0},			//40
	{OEM_PORT_VARIABEL16 , 						L"OemPortVar16",				0x02,0},			//41
	{OEM_PORT_VARIABEL17 , 						L"OemPortVar17",				0x02,0},			//42
	{OEM_PORT_VARIABEL18 , 						L"OemPortVar18",				0x02,0},			//43
	{OEM_PORT_VARIABEL19 , 						L"OemPortVar19",				0x02,0},			//44
	{OEM_PORT_VARIABEL20 , 						L"OemPortVar20",				0x02,0},			//45
	{OEM_PORT_VARIABEL21 , 						L"OemPortVar21",				0x02,0},			//46
	{OEM_PORT_VARIABEL22 , 						L"OemPortVar22",				0x02,0},			//47
	{OEM_PORT_VARIABEL23 , 						L"OemPortVar23",				0x02,0},			//48
	{OEM_PORT_VARIABEL24 , 						L"OemPortVar24",				0x02,0},			//49
	{OEM_PORT_VARIABEL25 , 						L"OemPortVar25",				0x02,0},			//50
	{OEM_PORT_VARIABEL26 , 						L"OemPortVar26",				0x02,0},			//51
	{OEM_PORT_VARIABEL27 , 						L"OemPortVar27",				0x02,0},			//52
	{OEM_PORT_VARIABEL28 , 						L"OemPortVar28",				0x02,0},			//53
	{OEM_PORT_VARIABEL29 , 						L"OemPortVar29",				0x02,0},			//54
	{DYNAMIC_PAGE_GROUP_COUNT_GUID , 			L"DynamicPageGroupCount",		0x02,0},			//55
	{DYNAMIC_PAGE_DEVICE_COUNT_GUID , 			L"DynamicPageDeviceCount",		0x02,0},		//56
	{DYNAMIC_PAGE_GROUP_CLASS_COUNT_GUID , 		L"DynamicPageGroupClass",		0x02,0}		//57

};

#define	AMIDIAG_GUID	\
	{ 0x8BDDF24B, 0x2C8A, 0x4360, 0xA3, 0xBB, 0xAE, 0xF0, 0x94, 0x98, 0x3B, 0xEA }

#define	AMI_RESCUE_GUID	\
	{ 0x5B4C6B7E, 0x352A, 0x4b3e, 0x9A, 0x04, 0x74, 0x08, 0xAD, 0xBD, 0x98, 0xC1 }

#define	AMI_FWUPDATE_GUID	\
	{ 0x1DE64B8E, 0x138B, 0x4258, 0xB7, 0xDD, 0xF2, 0xD8, 0xEC, 0x14, 0x2A, 0x9E }

#define	AMI_DIAGGUI_GUID	\
	{ 0x9EF7A7E5, 0x957C, 0x4410, 0xB5, 0x1A, 0xD0, 0xDD, 0xFF, 0x37, 0xC8, 0x7C }

static GUID_INFO _SetupGUIDInfo[_GUID_COUNT] = {
	// Needs to be first on List for patching up later.
	{EFI_GLOBAL_VARIABLE_GUID ,			L"Dummy",				{1},5}, // For lang codes/platformlangcode	0
	{BOOT_MANAGER_GUID , 					L"BootManager",		{1},1},	//1
	{BOOT_NOW_COUNT_GUID , 				L"BootNowCount",		{1},1},	//2
	{LEGACY_DEV_ORDER_GUID ,			L"LegacyDevOrder",	{1},7},	//3
	{AMI_CALLBACK_GUID , 					L"AMICallback",		{1},2},	//4
	{ERROR_MANAGER_GUID ,				L"ErrorManager",		{9},3},	//5
	{EFI_GLOBAL_VARIABLE ,					L"PlatformLangCodes",	{7},5},	//6
	{EFI_SHELL_FILE_GUID ,					L"shellx64.efi",			{6},0},	//7
	{EFI_SHELL_FILE_GUID ,					L"shellpbm.efi",			{6},0},	//8
	{IDE_SECURITY_CONFIG_GUID ,			L"IDESecDev",			{1},8},	//9
	{ADD_BOOT_OPTION_GUID ,				L"AddBootOption",		{0},9},	//10
	{OEM_TSE_VAR_GUID ,					L"OemTSEVar",		{1},10},	//11
	{DYNAMIC_PAGE_COUNT_GUID ,			L"DynamicPageCount",	{0},14},	//12
	{AMITSE_DRIVER_HEALTH_ENB_GUID ,	L"DriverHlthEnable",	{1},15},	//13
	{AMITSE_DRIVER_HEALTH_GUID ,		L"DriverHealthCount",	{1},16},	//14
	{AMITSE_DRIVER_HEALTH_CTRL_GUID ,	L"DrvHealthCtrlCnt",		{1},17},	//15
	{DRIVER_MANAGER_GUID ,				L"DriverManager",		{0},18},	//16
	{OEM_PORT_GUID1 ,						L"OemPortGuid1",		{0},19},	//17
	{OEM_PORT_GUID2 ,						L"OemPortGuid2",		{0},20},	//18
	{AMIDIAG_GUID ,							L"AMIDiag",			{2},21},	//19
	{AMI_RESCUE_GUID ,						L"AMIRescue",			{2},0xF080},	//20
	{AMI_FWUPDATE_GUID ,					L"AMIFWUpdate",			{2},0xF081},	//21
	{AMI_DIAGGUI_GUID ,						L"AMIDiagGUI",			{2},0xF082},	//22
	{DYNAMIC_PAGE_GROUP_COUNT_GUID ,							L"DynamicPageGroupCount",			{1},22},	//23
	{DYNAMIC_PAGE_DEVICE_COUNT_GUID ,							L"DynamicPageDeviceCount",			{1},23},	//24
	{DYNAMIC_PAGE_GROUP_CLASS_COUNT_GUID ,						L"DynamicPageGroupClass",		{1},24}	//25
};

#pragma pack(8)
typedef struct _TSE_SETUP_PACKAGE
{
	SETUP_PKG 		SetupPkg;
	SCREEN_INFO 	ScreenInfo;
	PAGE_LIST		PageList;
	UINT32			PageOffset[1]; // Increase if you have more pages - Dummy
	PAGE_INFO		PageInfo[1];   // Increase if you have more pages - Dummy

	CONTROL_INFO	ControlInfo[1];// Increase if controls	- Dummy

	VARIABLE_LIST	VariableList;
	UINT32			VariableOffset[_VARIABLE_COUNT]; // Increase if you have more Variable
	VARIABLE_INFO	VariableInfo[_VARIABLE_COUNT];
	HPK_INFO		HpkInfo[1]; //- Dummy
	GUID_LIST		GuidList;
	UINT32			GuidOffset[_GUID_COUNT]; // Increase if you have more GUID
	GUID_INFO		GuidInfo[_GUID_COUNT];
	PAGE_ID_LIST	PageIdList;
	UINT32			PageIdOffset[1]; // Increase if you have more Page - Dummy
	PAGE_ID_INFO	PageIdInfo[1];	// Increase if you have more Page - Dummy
}TSE_SETUP_PACKAGE;

#pragma pack()

#ifdef TSE_FOR_APTIO_4_50
/**
    function to initialize the application data

    @param ImageHandle 

    @retval EFI_STATUS

**/
EFI_STATUS	InitApplicationData(EFI_HANDLE ImageHandle)
{
	EFI_STATUS	Status=EFI_SUCCESS;
	UINTN i;
	CHAR16 *name = NULL;

	// Init the Application data
	// Hardcoded Application data for the TSE Lite
	gApplicationData = EfiLibAllocateZeroPool( 0x5000 ); // Allocate more of you add more

	// SETUP_PKG
	MemCopy(gApplicationData,&_SetupPackageTemplate, sizeof(_SetupPackageTemplate) );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageVariable = 0;
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageScreen = STRUCT_OFFSET( TSE_SETUP_PACKAGE, ScreenInfo );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackagePageList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, PageList );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageControlOffset = STRUCT_OFFSET( TSE_SETUP_PACKAGE, ControlInfo );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageVariableList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, VariableList );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageHpkData = STRUCT_OFFSET( TSE_SETUP_PACKAGE, HpkInfo );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageGuidList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, GuidList );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackagePageIdList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, PageIdList );
	((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageIfrData = sizeof(TSE_SETUP_PACKAGE);

	//SCREEN_INFO
	MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->ScreenInfo,&_SetupPackageScreen,sizeof(SCREEN_INFO));

	//PAGE_INFO
	MemSet(&_SetupPageInfo,sizeof(_SetupPageInfo),0);
	MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->PageInfo,&_SetupPageInfo,sizeof(_SetupPageInfo));

	//PAGE_LIST
	((TSE_SETUP_PACKAGE *)gApplicationData)->PageList.PageCount = 1;
	((TSE_SETUP_PACKAGE *)gApplicationData)->PageList.PageList[0] = (UINT32)((UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->PageInfo[0])-(UINTN)(gApplicationData));

	//CONTROL_INFO
	MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->ControlInfo,&_SetupControlInfo_N_N,sizeof(CONTROL_INFO));

	//VARIABLE_INFO
	name = GetVariableNameByID(VARIABLE_ID_LANGUAGE);
	EfiStrCpy(_SetupVariableInfo[VARIABLE_ID_LANGUAGE].VariableName, name);
	MemFreePointer(&name);
	MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->VariableInfo,&_SetupVariableInfo,sizeof(_SetupVariableInfo));

	//VARIABLE_LIST
	((TSE_SETUP_PACKAGE *)gApplicationData)->VariableList.VariableCount = _VARIABLE_COUNT;
	for(i=0;i<_VARIABLE_COUNT;i++)
		((TSE_SETUP_PACKAGE *)gApplicationData)->VariableList.VariableList[i] = (UINT32)((UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->VariableInfo[i])-(UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->VariableList));

	//GUID_INFO
	name = GetGUIDNameByID(VARIABLE_ID_LANGUAGE);
	EfiStrCpy(_SetupGUIDInfo[0].GuidName , name);
	MemFreePointer(&name);
	MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->GuidInfo,_SetupGUIDInfo,sizeof(_SetupGUIDInfo));

	//GUID_LIST
	((TSE_SETUP_PACKAGE *)gApplicationData)->GuidList.GuidCount = _GUID_COUNT;
	for(i=0;i<_GUID_COUNT;i++)
		((TSE_SETUP_PACKAGE *)gApplicationData)->GuidList.GuidList[i] = (UINT32)((UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->GuidInfo[i])-(UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->GuidList));

	return Status;
}
#endif		//For TSE_FOR_APTIO_4_50
#else 
#ifdef TSE_FOR_APTIO_4_50
/**
    function to initialize the application data

    @param ImageHandle 

    @retval EFI_STATUS

**/
EFI_STATUS	InitApplicationData(EFI_HANDLE ImageHandle)
{
	EFI_STATUS	Status = EFI_SUCCESS;
#if APTIO_4_00 || SETUP_USE_GUIDED_SECTION || TSE_FOR_64BIT
	VOID *pSection;
	UINTN SectionSize;
	EFI_GUID dataGuid = MINI_SETUP_DATA_GUID;

	if ( TRUE == IsIsolateSetupDataSupported() )
	{
		Status = FvReadResources (&dataGuid, &dataGuid,(VOID**)&pSection,&SectionSize);
	}
	else
	{
		Status=ReadImageResource(ImageHandle,&dataGuid,&pSection,&SectionSize);
	}
	
	if ( EFI_ERROR( Status ) ) {
		return Status;
	}
	gApplicationData = (UINT8 *)pSection + sizeof(EFI_GUID);
#else
#ifdef USE_DEPRICATED_INTERFACE

	// Read in the data files from the GUIDed section
	Status = LoadGuidedDataDriverLib( gImageHandle, dataGuid, &gApplicationData );
	if ( EFI_ERROR( Status ) )
	{
		return Status;
	}
#else
#ifndef STANDALONE_APPLICATION
	gApplicationData = (UINT8 *)&_SetupPackage;
#else
	gApplicationData = (UINT8 *)&DummySetupData;
#endif	//#ifndef STANDALONE_APPLICATION
#endif	//#ifdef USE_DEPRICATED_INTERFACE
#endif	//#if APTIO_4_00 || SETUP_USE_GUIDED_SECTION || TSE_FOR_64BIT

	return Status;
}

#endif			//For TSE_FOR_APTIO_4_50
#endif			//For TSE_BUILD_AS_APPLICATION

/**
    Function to load the user set defaults Silently

    @param VOID

    @retval VOID

**/
VOID LoadUserDefaultsSilently(VOID)
{
	NVRAM_VARIABLE *nvramVar, *tmpPtr;
	UINTN 			i = 0;
	UINT8 			*buffer;
	UINT16 			size;
	UINT32 			count;
   VARIABLE_INFO *varInfo = NULL;
	EFI_GUID sysAccessGuid = SYSTEM_ACCESS_GUID;
	
	VARIABLE_INFO *InnerLoopvarInfo = NULL;
	UINT32 			InnerLoop;
	BOOLEAN 			MatchFound = FALSE;
	nvramVar = &gVariableList [VARIABLE_ID_USER_DEFAULTS];
	if ( ( nvramVar->Size == 0 ) || ( nvramVar->Buffer == NULL ) )
		return;

	buffer = nvramVar->Buffer;
	count = *(UINT32 *)buffer;
	buffer += sizeof(UINT32);

	for ( i = 0; i < count; i++ )
	{
		varInfo = VarGetVariableInfoIndex (*(UINT32 *)buffer);
		tmpPtr = &gVariableList[ *(UINT32 *)buffer ];

		if ( IsLoadSysteAccessOnUserDefault() ) //Skip loading SYSTEM_ACCESS variable for user defaults	
		{
			if ( !EfiStrCmp(varInfo->VariableName, L"SystemAccess") &&  EfiCompareGuid(&sysAccessGuid, &(varInfo->VariableGuid)) )//UserDefault - Skip to load SystemAccess variable on pressing Restore User Defaults
			{
				buffer += sizeof(UINT32);
				size = *(UINT16 *)buffer;
				buffer += sizeof(UINT16);
				buffer += (sizeof (EFI_GUID) + sizeof (CHAR16) * VARIABLE_NAME_LENGTH); //Move to next variable
				buffer += size;
				continue;
			}
		}

		buffer += sizeof(UINT32);
		size = *(UINT16 *)buffer;
		buffer += sizeof(UINT16);

		if (
				(size == tmpPtr->Size) &&
				(!MemCmp (&varInfo->VariableGuid, buffer, sizeof (EFI_GUID) )) &&
				(!EfiStrCmp (varInfo->VariableName, (CHAR16 *)(buffer + sizeof (EFI_GUID)) ))
			)
		{
			buffer += (sizeof (EFI_GUID) + sizeof (CHAR16) * VARIABLE_NAME_LENGTH);
			MemCopy (tmpPtr->Buffer, buffer, size);
		}
		else					//If any variable deleted then order wont be same, so trying in all possible variable and loading the user defaults
		{
			MatchFound = FALSE;	
			for (InnerLoop = 0; InnerLoop < gVariables->VariableCount; InnerLoop ++)
			{
				InnerLoopvarInfo = VarGetVariableInfoIndex (InnerLoop);
				if ( NULL != InnerLoopvarInfo)
				{
					if (
						(size == InnerLoopvarInfo->VariableSize) && 
						(!MemCmp (&InnerLoopvarInfo->VariableGuid, buffer, sizeof(EFI_GUID))) && 
						(!EfiStrCmp (InnerLoopvarInfo->VariableName, (CHAR16 *)(buffer + sizeof (EFI_GUID)) ))
						)
					{
						buffer += (sizeof(EFI_GUID)+ sizeof(CHAR16) * VARIABLE_NAME_LENGTH);
						tmpPtr = &gVariableList [InnerLoop];
						MemCopy (tmpPtr->Buffer, buffer, size);
						MatchFound = TRUE;
						break;
					}
				}
			}
			if (FALSE == MatchFound)
			{
				SETUP_DEBUG_TSE ("\nERROR : Variable %s has been modified so skipping the USER default loading for this variable alone.", varInfo->VariableName );
				buffer += (sizeof(EFI_GUID)+ sizeof(CHAR16) * VARIABLE_NAME_LENGTH);
			}
		}
		buffer += size;
	}
	LoadedUserDefaultsHook();
	gResetRequired = TRUE;
}
/**
    Function to load the user set defaults

    @param VOID

    @retval VOID

**/
VOID LoadUserDefaults( VOID )
{

	if ( CallbackShowMessageBox( (UINTN)gLoadUserMsgBox, MSGBOX_TYPE_YESNO ) != MSGBOX_YES )
		return;

	LoadUserDefaultsSilently();
}
/**
    Function to save the user set defaults Silently

    @param VOID

    @retval VOID

**/
VOID SaveUserDefaultsSilently(VOID)
{
	NVRAM_VARIABLE *nvramVar, *tmpPtr;
	UINT32 i,count;
	UINT8 *buffer;
   VARIABLE_INFO *varInfo = NULL;
	
	nvramVar = &gVariableList[ VARIABLE_ID_USER_DEFAULTS ];
	if ( ( nvramVar->Size != 0 ) || ( nvramVar->Buffer != NULL ) )
		MemFreePointer( (VOID **)&(nvramVar->Buffer) );

	nvramVar->Size = sizeof(UINT32);
    count = 0;
	for ( i = 0; i < gVariables->VariableCount; i++ ) {
            if ( (i == VARIABLE_ID_USER_DEFAULTS) ||
             (i == VARIABLE_ID_ERROR_MANAGER) ||
             (i == VARIABLE_ID_BOOT_ORDER) ||
			 (i == VARIABLE_ID_BBS_ORDER) ||
             (i == VARIABLE_ID_BOOT_MANAGER) ||
             (i == VARIABLE_ID_BOOT_NOW))
            continue;
		nvramVar->Size += sizeof(UINT32) + sizeof(UINT16) + sizeof (EFI_GUID) + (sizeof (CHAR16) * VARIABLE_NAME_LENGTH) + gVariableList[ i ].Size;
        count++;
    }

	buffer = EfiLibAllocateZeroPool( nvramVar->Size );
	if ( buffer == NULL )
		return;

	nvramVar->Buffer = buffer;

	*(UINT32 *)buffer = count;
	buffer += sizeof(UINT32);

	for ( i = 0, tmpPtr = gVariableList; i < gVariables->VariableCount; i++, tmpPtr++ )
    {
        if ( (i == VARIABLE_ID_USER_DEFAULTS) ||
             (i == VARIABLE_ID_ERROR_MANAGER) ||
             (i == VARIABLE_ID_BOOT_ORDER) ||
			 (i == VARIABLE_ID_BBS_ORDER) ||
             (i == VARIABLE_ID_BOOT_MANAGER) ||
             (i == VARIABLE_ID_BOOT_NOW))
            continue;
		*(UINT32 *)buffer = i;
		buffer += sizeof(UINT32);
		*(UINT16 *)buffer = (UINT16)tmpPtr->Size;
		buffer += sizeof(UINT16);

		varInfo = VarGetVariableInfoIndex (i);
		MemCopy (buffer, &varInfo->VariableGuid, sizeof (EFI_GUID));
		buffer += sizeof (EFI_GUID);
	
		EfiStrCpy ((CHAR16 *)buffer, varInfo->VariableName);
//		MemCopy( buffer, varInfo->VariableName, sizeof(CHAR16) * VARIABLE_NAME_LENGTH );
		buffer +=  sizeof(CHAR16) * VARIABLE_NAME_LENGTH;

		MemCopy( buffer, tmpPtr->Buffer, tmpPtr->Size );
		buffer += tmpPtr->Size;
	}

	VarSetNvram( VARIABLE_ID_USER_DEFAULTS, nvramVar->Buffer, nvramVar->Size );

}
/**
    Function to save the user specified defaults

    @param VOID

    @retval VOID

**/
VOID SaveUserDefaults( VOID )
{

	if ( CallbackShowMessageBox( (UINTN)gSaveUserMsgBox, MSGBOX_TYPE_YESNO ) != MSGBOX_YES )
		return;
		
	SaveUserDefaultsSilently();
}

/**
    Return Size of Escape Code and gets the FG,BGColor.

    @param String 
    @param Bold 
    @param Foreground 
    @param Background 

    @retval UINTN Escape code length (in CHAR16), 0 if not an escape code
        Bold, Forground, Background - Filled with -1 if value not found in Escape Code

**/
UINTN GetANSIEscapeCode(CHAR16 *String,UINT8 *Bold,UINT8 *Foreground, UINT8 *Background)
{
	CHAR16 *TempStr;
	UINTN i;
	BOOLEAN Bld=FALSE,Fg=FALSE;
	UINT8 Num=0xff;

	//Is ESC Code
	if((String[0]==L'\x1b')&&(String[1]==L'['))
	{	
		TempStr = &String[2];

		//  ESC [ x ; x x ; x x  m 	= Color esc code format
		//	  0 1 2 3 4 5 6 7 8  9 
		//	Check till Code m or Upto 10 char

		for(i=2;(*TempStr!=L'm')&&(i<10);i++,TempStr++)
		{
			if(*TempStr == L';')
			{
				if(!Bld)
				{	
					*Bold = Num; 
					Num = 0xFF; 
					Bld = TRUE;
				}
				else if(!Fg)
				{
					*Foreground = Num; 
					Num = 0xFF; 
					Fg = TRUE;
				}
			}	
			else
			{
				if(Num==0xFF)
					Num =(UINT8)(*TempStr-L'0');
				else
					Num = (Num*10)+(UINT8)(*TempStr-L'0');
			}
			if(TempStr[1]==L'm')
				*Background=Num;
		}
		if(*TempStr==L'm')
			return (TempStr-String);
	}

	return 0;
}

/**
    function to perform the esc key operation

    @param string 

    @retval string

**/
CHAR16 *SkipEscCode(CHAR16 *String)
{
	CHAR16 	  *TempString;
	UINTN	  i=0,j=0;

	if (NULL == String) {
		return NULL;	
	}
	// Check For escape Code and remove that from the string
	TempString = StrDup(String);
	while(String[i])
	{
		if((String[i] == 0x001b) && (String[i+1] == L'[')) // ESC Code
		{	
			while(String[i] != L'm')i++;
			i++;
		}
		TempString[j] = String[i];
		i++; j++;
	}
	TempString[j]=0;	

	return TempString;
}
static UINT8 result = (UINT8)-1;
static CALLBACK_MESSAGEBOX  gCbMsgBox = { { CALLBACK_TYPE_MESSAGEBOX, sizeof(CALLBACK_MESSAGEBOX) }, 0 }; 

VOID _CallbackMsgboxEx( VOID *container, VOID *object, VOID *cookie )
{
	if ( (cookie != NULL) && ( ((CALLBACK_MESSAGEBOX*)cookie)->Header.Type == CALLBACK_TYPE_MESSAGEBOX) )
		result = (UINT8)(((CALLBACK_MESSAGEBOX*)cookie)->Result);
}
/**
    Callback function to get the message box return value

    @param msgbox 

    @retval Message box return value

**/
UINTN _CallbackGetValueEx( MSGBOX_DATA *msgbox )
{
	ACTION_DATA *action = gMsgBoxAction;

	result = (UINT8)-1;
	gMsgBox.SetCallback( msgbox, NULL, _CallbackMsgboxEx, &gCbMsgBox );

	while ( result == (UINT8)-1 )
	{
		if ( action->Input.Type != ACTION_TYPE_NULL )
			gAction.ClearAction( action );

		gMsgBox.Draw( msgbox );

		if ( gAction.GetAction( action ) != EFI_SUCCESS )
			continue;

		gMsgBox.HandleAction( msgbox, action );
		DoRealFlushLines();
	}

	return result;
}


/**
    Function to display the message box 

    @param MsgBoxTitle 
    @param Message 
    @param MsgBoxType 
    @param pSelection 

    @retval EFI_STATUS

**/
EFI_STATUS ShowPostMsgBoxEx(
    IN CHAR16			*Title,
    IN CHAR16			*Message,
    IN CHAR16			*Legend,
    IN MSGBOX_EX_CATAGORY	MsgBoxExCatagory,
    IN UINT8	 		MsgBoxType,
    IN UINT16			*OptionPtrTokens,	
    IN UINT16	 		OptionCount,		
    IN AMI_POST_MGR_KEY		*HotKeyList, 		
    IN UINT16 			HotKeyListCount, 
    OUT UINT8			*MsgBoxSel,
    OUT AMI_POST_MGR_KEY	*OutKey
)
{
    static AMI_IFR_MSGBOX MsgBox = { 0, 0, 0, INVALID_HANDLE, 0 };
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 retValue = (UINT8)-1;
    MSGBOX_DATA *msgbox = NULL;
    CONTROL_INFO dummy;
    ACTION_DATA *action = gMsgBoxAction;	
    UINT16 i = 0;	

    if (NULL == gST->ConIn)
        return EFI_NOT_READY;

    if(Title == NULL || Message == NULL || 
	//Note: Messagebox should not require a legend 
	/*((Legend == NULL ) && (MsgBoxType == MSGBOX_TYPE_NULL)) ||*/
	    ((MsgBoxType == MSGBOX_TYPE_CUSTOM) && (OptionPtrTokens == NULL)) ||
        ((OptionCount > 0) && (OptionPtrTokens == NULL)) ||
        ((HotKeyListCount > 0) && (HotKeyList == NULL)))
        return EFI_INVALID_PARAMETER;
    
    if((MsgBoxType == MSGBOX_TYPE_CUSTOM) && (OptionCount > MSGBOX_MAX_OPTIONS))
    	return EFI_UNSUPPORTED ;	

    MsgBox.Title = HiiChangeString(gHiiHandle, MsgBox.Title, Title);
    MsgBox.Text = HiiChangeString(gHiiHandle, MsgBox.Text, Message);

    if(!gApp)
    {
        Status = gAction.Create( &gMsgBoxAction );
    	if ( EFI_ERROR( Status ) )
			return Status;
    	action = gMsgBoxAction;					// If NULL while initialization then this assignment will be helpful
    }


	Status = gMsgBox.Create( (void**)&msgbox ); 
	if ( EFI_ERROR( Status ) )
		return retValue;

	MemSet( &dummy, sizeof(dummy), 0 );
	dummy.ControlHandle = (VOID*)(UINTN)INVALID_HANDLE;
	dummy.ControlPtr = (VOID*)&MsgBox ; //data;
	dummy.ControlFlags.ControlVisible = TRUE;

	msgbox->MsgBoxCatagory = (MsgBoxExCatagory & (~MSGBOX_EX_ATTRIB_CLEARSCREEN)) ;
	
	msgbox->Legend = StrDup (Legend);				
	if (OptionCount)										
	{
		msgbox->PtrTokens = EfiLibAllocateZeroPool (sizeof (UINT16) * OptionCount);
		if (NULL == msgbox->PtrTokens)
		{
			return EFI_OUT_OF_RESOURCES;
		}
		MemCopy (msgbox->PtrTokens, OptionPtrTokens, sizeof (UINT16) * OptionCount);
	}
	else
	{
		msgbox->PtrTokens = NULL;
	}
//	msgbox->PtrTokens = OptionPtrTokens;
	msgbox->OptionCount = OptionCount ;

	Status = gMsgBox.Initialize( msgbox, &dummy );
	if ( EFI_ERROR( Status ) )
		goto Done;

	gMsgBox.SetType( msgbox, MsgBoxType );
	
	//*MsgBoxSel = (UINT8)_CallbackGetValueEx( msgbox );

	result = (UINT8)-1;
	gMsgBox.SetCallback( msgbox, NULL, _CallbackMsgboxEx, &gCbMsgBox );

	if((MsgBoxExCatagory & MSGBOX_EX_ATTRIB_CLEARSCREEN) == MSGBOX_EX_ATTRIB_CLEARSCREEN)
			ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );

	while ( result == (UINT8)-1 )
	{
		if ( action->Input.Type != ACTION_TYPE_NULL )
			gAction.ClearAction( action );

		gMsgBox.Draw( msgbox );

		if ( gAction.GetAction( action ) != EFI_SUCCESS )
			continue;

		gMsgBox.HandleAction( msgbox, action );
		DoRealFlushLines();
		
		if(HotKeyList!=NULL && HotKeyListCount>0)
		{
			for(i=0 ; i<HotKeyListCount; i++)
			{
				if( (HotKeyList[i].Key.ScanCode == action->Input.Data.AmiKey.Key.ScanCode ) &&
				    (HotKeyList[i].Key.UnicodeChar == action->Input.Data.AmiKey.Key.UnicodeChar) &&
				    (TseCheckShiftState( action->Input.Data.AmiKey, HotKeyList[i].KeyShiftState)  ) )
				{
					// Verify passed argument is not NULL before attempting to assign to it
					if(OutKey != NULL)
					{
						OutKey->Key.ScanCode = action->Input.Data.AmiKey.Key.ScanCode ; 
						OutKey->Key.UnicodeChar = action->Input.Data.AmiKey.Key.UnicodeChar; 
					
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
						OutKey->KeyShiftState = (UINT32)action->Input.Data.AmiKey.KeyState.KeyShiftState ;
#else
						OutKey->KeyShiftState = 0 ;
#endif
					}
					goto Done;
					
				}
		
			}
			
		}
		else
		{
			if((msgbox->MsgBoxType == MSGBOX_TYPE_NULL) && (action ->Input.Type == ACTION_TYPE_KEY))
				goto Done;

		}

	}

    if((result != -1) && (MsgBoxSel != NULL))
	*MsgBoxSel = (UINT8) result;


Done:
	gMsgBox.Destroy( msgbox, TRUE );


    if(!gApp)
    {
        gAction.Destroy( gMsgBoxAction, TRUE );
	gMsgBoxAction = NULL ;
    }
   

    return Status;
}
/**
    Function to display the progress bar 

    @param ProgressBoxState Progress bar state 
    @param Title Progress bar title 	
    @param Message - Progress bar message string
    @param Legend - Progress bar legend string
    @param Percent - Percent Complete (0 - 100)
    @param Handle - HANDLE. Valid Handle for update and close
    @param OutKey Out Key

    @retval EFI_STATUS

**/
EFI_STATUS ShowPostProgress(
    IN UINT8			ProgressBoxState, 
    IN CHAR16			*Title,
    IN CHAR16			*Message,
    IN CHAR16			*Legend,	
    IN UINTN 			Percent,	
    IN OUT VOID			**Handle,	
    OUT AMI_POST_MGR_KEY	*OutKey
)
{
    	static AMI_IFR_MSGBOX MsgBox = { 0, 0, 0, INVALID_HANDLE, 0 };
    	EFI_STATUS Status = EFI_SUCCESS;
    	MSGBOX_DATA *msgbox = NULL;
    	CONTROL_INFO dummy;
	ACTION_DATA *action=NULL;
	UINT16 MessageToken = 0 ;
	MEMO_DATA *memo = NULL;

	switch(ProgressBoxState)
        {
            case AMI_PROGRESS_BOX_INIT:

		Status = gMsgBox.Create( (void**) &msgbox ); 
		if ( EFI_ERROR( Status ) )
			return EFI_UNSUPPORTED;

	    	MsgBox.Title = HiiChangeString(gHiiHandle, STRING_TOKEN(STR_MSGBOX_PROGRESS_TITLE), Title);
    		MsgBox.Text = HiiChangeString(gHiiHandle, STRING_TOKEN(STR_MSGBOX_PROGRESS_TEXT), Message);

		MemSet( &dummy, sizeof(dummy), 0 );
		dummy.ControlHandle = (VOID*)(UINTN)INVALID_HANDLE;
		dummy.ControlPtr = (VOID*)&MsgBox ; //data;
		dummy.ControlFlags.ControlVisible = TRUE;

		msgbox->MsgBoxCatagory = SetMessageBoxProgressType () ;

		Status = gMsgBox.Initialize( msgbox, &dummy );

		if ( EFI_ERROR( Status ) )
		{
			gMsgBox.Destroy( msgbox, TRUE );
			return Status;
		}

		*Handle = msgbox ;
		msgbox->Legend = Legend ;
		msgbox->Percent = 0 ;
		
		/*Fixing the message box width only in case of progress box.Incrementing progress for every 2%. Hence width will be 50+2
		 *The dimensions of inner memo also needs to be adjusted based on new width. */
		msgbox->Width = 52;
		msgbox->Memo->Width = msgbox->Width - 2;
		gMemo.GetControlHeight((CONTROL_DATA*)msgbox->Memo, NULL, &msgbox->Height );
		msgbox->Height += 4;
		gMemo.SetDimensions( (CONTROL_DATA*) msgbox->Memo, msgbox->Width-2 , msgbox->Height-3 );
		gMsgBox.SetType( msgbox, MSGBOX_TYPE_PROGRESS );
		
		gMsgBox.Draw( msgbox );
		DoRealFlushLines();
		
            	break;

            case AMI_PROGRESS_BOX_UPDATE:

		if(Handle == NULL)
			return EFI_UNSUPPORTED;
		
		msgbox = *Handle ;
	        msgbox->Percent = Percent ;

		if(Message != NULL)
		{
		     memo = msgbox->Memo;
		     MessageToken = HiiChangeString(gHiiHandle, MessageToken, Message);
		     UefiSetSubTitleField((VOID *)memo->ControlData.ControlPtr, MessageToken);
		}

		gMsgBox.Draw( msgbox );	
		DoRealFlushLines();
		
		//Output the key
		if (! EFI_ERROR( gAction.Create( &action ) ) )
		{
                if(!EFI_ERROR(gAction.GetAction( action )))
                {
                    OutKey->Key.ScanCode = action->Input.Data.AmiKey.Key.ScanCode ;
                    OutKey->Key.UnicodeChar = action->Input.Data.AmiKey.Key.UnicodeChar;

                #if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
                    OutKey->KeyShiftState = action->Input.Data.AmiKey.KeyState.KeyShiftState ;
                #else
                    OutKey->KeyShiftState = 0 ;
                #endif
                }
                Status = gAction.Destroy(action, TRUE);
            }
              	break;

            case AMI_PROGRESS_BOX_CLOSE:
            	msgbox = *Handle ;
		gMsgBox.Destroy( msgbox, TRUE );
		//restore the screen
    	       	break;
        }

    	return Status;
}
/**
    Function to display the message box 

    @param MsgBoxTitle 
    @param Message 
    @param MsgBoxType 
    @param pSelection 

    @retval EFI_STATUS

**/
EFI_STATUS ShowPostMsgBox(IN CHAR16  *MsgBoxTitle,IN CHAR16  *Message,IN UINT8  MsgBoxType, UINT8 *pSelection)
{
    static AMI_IFR_MSGBOX MsgBox = { 0, 0, 0, INVALID_HANDLE, 0 };
    EFI_STATUS Status = EFI_SUCCESS;
	UINT8 Selection;

    MsgBox.Title = HiiChangeString(gHiiHandle, MsgBox.Title, MsgBoxTitle);
    MsgBox.Text = HiiChangeString(gHiiHandle, MsgBox.Text, Message);

  	if(!gApp)
    {
        Status = gAction.Create( &gMsgBoxAction );
    	if ( EFI_ERROR( Status ) )
			return Status;
    }

    Selection = CallbackShowMessageBox((UINTN)&MsgBox, MsgBoxType);

  	if(!gApp)
    {
        gAction.Destroy( gMsgBoxAction, TRUE );
		gMsgBoxAction = NULL;
    }
    
    if(pSelection!=NULL)
	    *pSelection = Selection;

	return Status;
}

#define     PADDING     8
#define     GUTTER      2
#define     BRACKET     1
UINT16 gPostLabelMargin = 0 ;
extern EFI_STATUS _CBPopupStringEdit(VOID *container, VOID *object, VOID *cookie);
extern UINT8 res ;
extern CALLBACK_VARIABLE  gPopupEditCb ;
/**
    Callback function for text entry popup

    @param container VOID *object, VOID *cookie

    @retval EFI_STATUS

**/
EFI_STATUS _PostEditCallback(VOID *container, VOID *object, VOID *cookie)
{
	EFI_STATUS Status = EFI_SUCCESS;
  
/*  
	if(cookie!=NULL)
	{
       res=0;
	}
	else
 	   res=1;
*/    
	return Status;
}
/**
    Function to draw text entry edit box

    @param popup 

    @retval EFI_STATUS

**/
EFI_STATUS _PostEditDraw( POPUPEDIT_DATA *popup )
{
	UINT16 Length = 0 ;
 StyleGetShadowSupport() ;
     
	//DrawWindow( popup->Left, popup->Top, popup->Width, popup->Height, (UINT8)(popup->BGColor | popup->FGColor) , FALSE, FALSE );

	if ( popup->Title != NULL )
    {
        Length = (UINT16)TestPrintLength( popup->Title ) / (NG_SIZE);
        DrawStringWithAttribute( popup->Left - Length - GUTTER, popup->Top, popup->Title, (popup->ControlActive?EFI_WHITE:popup->FGColor) | popup->BGColor );
    }
		
    Length = (UINT16)UefiGetMaxValue(popup->ControlData.ControlPtr);
    DrawStringWithAttribute( popup->Left, popup->Top, L"[", (popup->ControlActive?EFI_WHITE:popup->FGColor) | popup->BGColor );
    DrawStringWithAttribute( popup->Left+Length+1   , popup->Top, L"]", (popup->ControlActive?EFI_WHITE:popup->FGColor) | popup->BGColor );
    if(popup->ControlActive)
        DrawWindow( popup->Left+1, popup->Top, Length, popup->Height, (UINT8)(EFI_BLACK | popup->FGColor) , FALSE, FALSE );

	return gControl.Draw( popup );
}
/**
    Function to draw text entry popup window

    @param PopupEdit 

    @retval EFI_STATUS

**/
EFI_STATUS _PostPopupEditDraw( POPUPEDIT_DATA *PopupEdit )
{
	EFI_STATUS Status;
	CHAR16 *text = NULL, *newText = NULL;
	CHAR16 *line;
	UINT16 pos, height,i,ii=0;;

	
	text = StrDup(PopupEdit->Text);

	Status = _PostEditDraw( PopupEdit );
	if(EFI_ERROR(Status))
		return Status;

	// clear out old wrapped string
	for ( pos = ii; pos < PopupEdit->Height-2; pos++ )
		DrawStringWithAttribute( PopupEdit->Left+1, PopupEdit->Top + pos, PopupEdit->EmptyString, PopupEdit->FGColor | PopupEdit->BGColor );

	newText = StringWrapText( text, PopupEdit->Width-1, &height );
	if ( newText != text )
		MemFreePointer( (VOID **)&text );

	line = newText;

	for ( pos = ii; pos < PopupEdit->Height; pos++ )
	{
		CHAR16 save;

		text = line;
		if ( *line == L'\0' )
			break;

		while ( ( *line != L'\n' ) &&( *line != L'\r' ) && ( *line != L'\0' ) )
			line++;

		save = *line;
		*line = L'\0';
		i=0;
		while( (text[i] != L'\0') && (PopupEdit->Chr !=0x0 ) )
		{		
//			if(text[i] != L' ') //all chars should be treated the same
				text[i] = PopupEdit->Chr;
			i++;
		}

		if(PopupEdit->ControlActive)
			DrawStringWithAttribute( PopupEdit->Left+1, PopupEdit->Top + pos, text, EFI_WHITE | EFI_BLACK );
		else
			DrawStringWithAttribute( PopupEdit->Left+1, PopupEdit->Top + pos, text, PopupEdit->FGColor | PopupEdit->BGColor );

		if ( ( *line = save ) != L'\0' )
		{
			line++;
			if ( ( *line == L'\r' ) || ( *line == L'\n' ) )
				line++;
		}
	}
	MemFreePointer( (VOID **)&newText );

	if ( pos < height )
		height = pos;

	PopupEdit->DisplayLines = height;
	FlushLines( 0, 24  );
	//FlushLines( PopupEdit->Top, PopupEdit->Top + PopupEdit->Height  );
	return  EFI_SUCCESS;

}
/**
    Function to draw text entry popup window

    @param popup 
    @param editList 
    @param InputData 
    @param ItemCount 
    @param ValidateKeyFunc 

    @retval UINTN 0/1 (success/abort)

**/
UINTN _PostPopupEditGetValues(
    POPUP_DATA          *popup, 
    POPUPEDIT_DATA      *editList, 
    TEXT_INPUT_TEMPLATE *InputData,
    UINT16              ItemCount, 
    DISPLAY_TEXT_KEY_VALIDATE ValidateKeyFunc
)
{
	EFI_STATUS Status;
    POPUPEDIT_DATA *ptrEdit=NULL; 
    CONTROL_ACTION Action = ControlActionUnknown;
    ACTION_DATA *action=NULL ;
    UINT16 i=0, Length=0, CurPos=0;
    UINT16 count = 0;
    res = (UINT8)-1;
    

    if(!gApp)
    {
        Status = gAction.Create( &action );
    	if ( EFI_ERROR( Status ) )
        {
		    res=0;
            goto Done;
        }
    } else {
        action = gApp->Action;
    }

          
    gPopup.Draw( popup );

    //1. Position Child items
    for ( i = 0; i < ItemCount; i++)
	{
        Length = (UINT16)TestPrintLength( editList[i].Title ) / (NG_SIZE);  
        editList[i].Top = popup->Top+1+i ;
        editList[i].Left = popup->Left + gPostLabelMargin ;
        editList[i].Height = 1 ;  
    }		
    if (Length)
    {
	// added fake use     
    }
    ;

    res = (UINT8)-1;

    while ( res == (UINT8)-1 )
	{
		//2. Clear the action
        if ( action->Input.Type != ACTION_TYPE_NULL ) { 
			gAction.ClearAction( action );
        }
		
        //3. Draw the controls
        gPopup.Draw( popup );

        ptrEdit = editList;

        for ( i = 0; i < ItemCount; i++, ptrEdit++)
	    {
            _PostPopupEditDraw( ptrEdit );
        }	

        FlushLines( popup->Top-2, popup->Top + popup->Height  );
        DoRealFlushLines();

        //4. Get the action
        Status = gAction.GetAction( action );
        if( Status != EFI_SUCCESS ) {
			continue;
        }

        //5. Set the count to control the active item
        if (action->Input.Type == ACTION_TYPE_KEY)
        {
            Action = MapControlKeysHook(action->Input.Data.AmiKey) ;
       
            switch(Action)
            {
                case ControlActionHome:
                    count = 0;
                    break;
                
                case ControlActionEnd:
                    count = ItemCount-1;
                    break;

                case ControlActionNextUp:
                    if (count > 0 ) 
                        count--;
                    break;

	            case ControlActionNextSelection:    
                case ControlActionNextDown:
                    if (count < ItemCount-1 ) 
                        count++;
                    break;
              /*       
                case ControlActionSelect:
                    count++;
                    if(count >= ItemCount)
                        res=0;
                    break;
              */          
                case ControlActionAbort:
                    res=1;
                    break;
                default:
                    break;
            }
        }

        ptrEdit = editList;

        //6. Scan Items, Set Active Status and handle the action
        for ( i = 0; i < ItemCount; i++, ptrEdit++)
	    {
            if(count == i)
		    {		
                //Handle the action ONLY for the active item 
                ptrEdit->ControlActive = TRUE ;
              
                //Check filter key, skip the character/Action if required
                if( (action->Input.Data.AmiKey.Key.UnicodeChar) && 
                    (Action != ControlActionBackSpace)          &&
                    (Action != ControlActionAbort)              &&
                    //(Action != ControlActionSelect)           &&
                    (Action != ControlActionNextSelection)      
                   /* (action->Input.Data.AmiKey.Key.UnicodeChar  != 0x20 )*/   )
	            {
                    
                    if(InputData[i].FilterKey == TSE_FILTER_KEY_NUMERIC)
                    { 
                        if(!CharIsNumeric(action->Input.Data.AmiKey.Key.UnicodeChar))
                            continue ;
                    }
                    else if(InputData[i].FilterKey == TSE_FILTER_KEY_ALPHA)
                    {
                         if(!CharIsAlpha(action->Input.Data.AmiKey.Key.UnicodeChar))
                            continue ;
                    }
                    else if(InputData[i].FilterKey == TSE_FILTER_KEY_ALPHANUMERIC)
                    {
                         if(!CharIsAlphaNumeric(action->Input.Data.AmiKey.Key.UnicodeChar))
                            continue ;
                    }
                    else if((InputData[i].FilterKey == TSE_FILTER_KEY_CUSTOM) && (ValidateKeyFunc!=NULL))
                    {
                         if(!(ValidateKeyFunc(i, action->Input.Data.AmiKey.Key.UnicodeChar, ptrEdit->Text)))
                            continue ;
                    }
                }
                
                gPopupEdit.HandleAction( ptrEdit, action );
                CurPos = (UINT16)EfiStrLen(ptrEdit->Text)+1 ;
                gST->ConOut->SetAttribute ( gST->ConOut, ptrEdit->FGColor | ptrEdit->BGColor );
                gST->ConOut->SetCursorPosition( gST->ConOut,
                    ptrEdit->Left + CurPos, ptrEdit->Top );
                if(IsTSECursorSupport())
                {
						 if(CurPos <= ptrEdit->TextWidth)
						 {
							 gST->ConOut->EnableCursor(gST->ConOut, TRUE);
						 }
						 else
						 {  	 
							  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
						 }
                }
                
                                
		    }
            else
            {
                ptrEdit->ControlActive = FALSE ;
            }

        }

        if(Action == ControlActionSelect)
        {
            count++;
            
            if(count >= ItemCount)
            {
                res=0;
            }
        }

        Action = ControlActionUnknown ;
        
    }

Done:
    if(!gApp)
    {
        gAction.Destroy( action, TRUE );
	    action = NULL ;
    }
    
 
	return res;
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
EFI_STATUS ShowPostTextBox(
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE FilterKeyFunc)
{
    
    EFI_STATUS Status = EFI_SUCCESS;
    UINT16 i=0, Length=0, MaxLabelWidth=0, MaxControlWidth=0;   
    UINT8 retValue = (UINT8)-1;
    VOID *OriginalPopupEditDraw = NULL ;
    VOID *IFRString ;
    CONTROL_INFO dummy;
   
    POPUP_DATA *popup = NULL ;
    POPUPEDIT_DATA *PopupEditList=NULL, *ptrEdit=NULL; 
    CALLBACK_VARIABLE *PopupEditCb=NULL, *ptrCb=NULL ;
    CONTROL_INFO *dummys=NULL, *ptrDummy=NULL;
    EFI_TPL OrgTPL;
    BOOLEAN PreviousCursorState = gST->ConOut->Mode->CursorVisible;

    if(HiiHandle == NULL)
    {
        return EFI_UNSUPPORTED ;
    } 

    Status = gPopup.Create( (void**)&popup );
	if ( EFI_ERROR( Status ) )
    {
		return Status;
    }

    // this *MUST* be run a EFI_TPL_APPLICATION
    OrgTPL = gBS->RaiseTPL( EFI_TPL_HIGH_LEVEL );               // guarantees that RestoreTPL won't ASSERT
    gBS->RestoreTPL( EFI_TPL_APPLICATION );

    PopupEditList = EfiLibAllocateZeroPool( sizeof(POPUPEDIT_DATA) * ItemCount);
    PopupEditCb = EfiLibAllocateZeroPool( sizeof(CALLBACK_VARIABLE) * ItemCount);
    dummys = EfiLibAllocateZeroPool( sizeof(CONTROL_INFO) * ItemCount);

    ptrEdit = PopupEditList;
    ptrDummy = dummys ;
    ptrCb = PopupEditCb ;  

    IFRString = UefiCreateSubTitleTemplate(TitleToken);
    //((EFI_IFR_STRING*)IFRString)->MaxSize = 10 ;
    
    MemSet( &dummy, sizeof(dummy), 0 );
    dummy.ControlPtr = (VOID*)IFRString;
	dummy.ControlHandle = HiiHandle;
	dummy.ControlType = CONTROL_TYPE_POPUP_STRING;
    dummy.ControlFlags.ControlVisible = TRUE;
    

    Status = gPopup.Initialize( popup, &dummy );
	if ( EFI_ERROR( Status ) )
		goto Done;

    SetControlColorsHook( NULL, NULL , NULL, NULL , NULL, NULL ,
	                      NULL , NULL,  NULL,NULL ,
						  NULL,NULL ,NULL,&(popup->FGColor),&(popup->BGColor)  );

    popup->Border  = TRUE ;
    popup->Title = HiiGetString(HiiHandle, TitleToken); 
    popup->Style  = POPUP_STYLE_BBS ;

    if ( popup->Title == NULL )
    {
		Status = EFI_UNSUPPORTED ;
        goto Done;
    }
    

    for ( i = 0; i < ItemCount; i++, ptrEdit++, ptrDummy++, ptrCb++)
	{
	    
        if((InputData[i].Value == NULL) || (InputData[i].MaxSize < 1 )) 
        {
            Status = EFI_INVALID_PARAMETER;
            goto Done;
        }

        // Create PopupEdit Control
     //   Status = gPopupEdit.Create( &((POPUPEDIT_DATA *)ptrEdit) );
        Status = gPopupEdit.Create( ((void **)&ptrEdit) );
	    if ( EFI_ERROR( Status ) )
		    goto Done;

        MemSet( ptrDummy, sizeof(CONTROL_INFO), 0 );
        ptrDummy->ControlPtr = (VOID*)UefiCreateStringTemplate(InputData[i].ItemToken);
	    ptrDummy->ControlHandle = HiiHandle;
	    ptrDummy->ControlType = CONTROL_TYPE_POPUP_STRING;
        ptrDummy->ControlFlags.ControlVisible = TRUE;
        ((EFI_IFR_STRING*)ptrDummy->ControlPtr)->MaxSize = InputData[i].MaxSize ;

		//Set the text, caller may want to display default value 
		gPopupEdit.SetText(ptrEdit, InputData[i].Value);
        gPopupEdit.SetType( ptrEdit, InputData[i].Hidden?POPUPEDIT_TYPE_PASSWORD:POPUPEDIT_TYPE_STRING );

        // Initialize PopupEdit Control
        Status = gPopupEdit.Initialize( ptrEdit, ptrDummy ) ;
        
	    if ( EFI_ERROR( Status ) )
		    goto Done;		

        ptrEdit->Border  = FALSE ;
	    ptrEdit->Style  = POPUP_STYLE_NORMAL ;    
        
        gPopupEdit.SetCallback( ptrEdit, NULL, (OBJECT_CALLBACK)_PostEditCallback, ptrCb );
        
        // Item width = PromptStringLength + "[" + ControlWidth +"]"

        //Get maximum label width
        Length = (UINT16)TestPrintLength( ptrEdit->Title ) / (NG_SIZE);
        if(Length > MaxLabelWidth)
            MaxLabelWidth = Length ; 

        
        //Get maximum control width
        Length = (UINT16)UefiGetMaxValue(ptrEdit->ControlData.ControlPtr); 
        if(Length > MaxControlWidth)
            MaxControlWidth = Length ; 
	}
    
    //popup->width = MaxLabelWidth + GUTTER + "["   + MaxControlWidth + "]"
    popup->Width = MaxLabelWidth + GUTTER + BRACKET + MaxControlWidth + BRACKET ;
    popup->Width += PADDING ;    

    Length = (UINT16)TestPrintLength( popup->Title ) / (NG_SIZE);  
    
    //Check if the title exceeds the popup window width
    if(Length > popup->Width)
    {
        //Adjust the popup window width
        popup->Width = Length + PADDING ; 
        //Set the label margin 
        gPostLabelMargin = (popup->Width - (MaxLabelWidth + GUTTER + MaxControlWidth))/2 + MaxLabelWidth  ;
    }
    else
    {
        //Set the label margin 
        gPostLabelMargin = MaxLabelWidth + GUTTER + PADDING/2; 
    }
  
    //Set the popup window height
    popup->Height = ItemCount + 2	;

    //Check screen overflow
    if( (popup->Width > gMaxCols - 2)   ||
        (popup->Height > gMaxRows - 2)   )
    {
        Status = EFI_UNSUPPORTED;
        goto Done;	   
    }

   	// Save gPopupEdit.Draw (It may be an override function)
    OriginalPopupEditDraw = gPopupEdit.Draw ;

    // Override gPopupEdit.Draw with our functiion (It will be called by the PopupEdit control)
  	gPopupEdit.Draw	= (OBJECT_METHOD_DRAW)_PostPopupEditDraw ;
  	if (IsTSECursorSupport())
  	    gST->ConOut->EnableCursor(gST->ConOut, TRUE);
    gST->ConOut->SetAttribute ( gST->ConOut, popup->FGColor | popup->BGColor );

    // Launch the textbox and get the values. 
    retValue = (UINT8)_PostPopupEditGetValues( popup, PopupEditList, InputData, ItemCount, FilterKeyFunc );
    if (IsTSECursorSupport())
        gST->ConOut->EnableCursor(gST->ConOut, PreviousCursorState);

    // Restore gPopupEdit.Draw to the original
   	gPopupEdit.Draw	= OriginalPopupEditDraw ;


    if(retValue == 0)
	{
		for ( i = 0; i < ItemCount; i++)
	    {
            if(InputData[i].Value == NULL)
            {
                // Allocate space for the user entered value
                //EntryData[i].Value = EfiLibAllocateZeroPool( (PopupEditCb[i].Length+1) * sizeof(CHAR16) );
                
                //if(EntryData[i].Value == NULL)
                Status = EFI_INVALID_PARAMETER;
                goto Done;	
		    }
            // Get the user entered value
            EfiStrCpy( InputData[i].Value, PopupEditList[i].Text );
            //EfiStrCpy( InputData[i].Value, PopupEditCb[i].Data );	
        }		
	}

Done:
  
    
   gPopupEdit.Destroy( PopupEditList, TRUE );//Destroy text box before message box
     
    // Free allocated init data
  	
  	MemFreePointer((VOID**)&PopupEditCb);
      MemFreePointer((VOID**)&dummys);
	gPopup.Destroy( popup, TRUE );

    if (gApp != NULL) {
        gApp->CompleteRedraw = TRUE ; //CompleteRedraw;
        gApp->OnRedraw = SUBMENU_COMPLETE_REDRAW;
    }

    gBS->RaiseTPL( EFI_TPL_HIGH_LEVEL ); // guarantees that RestoreTPL won't ASSERT
    gBS->RestoreTPL( OrgTPL );

    if (res == 1) {
        Status = EFI_ABORTED;
    }

	return Status;
}
/**
    Timer notify fucntion

    @param Event 
    @param Context 

    @retval VOID

**/
VOID _DisplayInfoCancelTimer( EFI_EVENT Event, VOID *Context )
{
	if ( gInfoBox != NULL )
		gMsgBox.Destroy( gInfoBox, TRUE );
	TimerEnabled = FALSE;
	gInfoBox = NULL;
	TimerStopTimer( &Event );
}

/**
    fucntiont to display the information box

    @param InfoBoxTitle 
    @param Message 
    @param TimeLimit 
    @param RetEvent )

    @retval EFI_STATUS

**/
EFI_STATUS ShowInfoBox(IN CHAR16  *InfoBoxTitle, IN CHAR16  *Message, IN UINTN   TimeLimit, EFI_EVENT  *RetEvent)
{
   EFI_STATUS Status;
	static AMI_IFR_MSGBOX msgbox = { 0, 0, 0, INVALID_HANDLE, 0 };
	CONTROL_INFO dummy;
	ACTION_DATA *action = gMsgBoxAction;
	UINT32 EventType = EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL;		//|EFI_EVENT_NOTIFY_WAIT, EFI_EVENT_NOTIFY_SIGNAL|
	
	msgbox.Title = HiiChangeString(gHiiHandle, msgbox.Title, InfoBoxTitle);
   msgbox.Text = HiiChangeString(gHiiHandle, msgbox.Text, Message);
    
	Status = gMsgBox.Create((void**) &gInfoBox );
	if ( EFI_ERROR( Status ) )
		return 1;

	MemSet( &dummy, sizeof(dummy), 0 );

	dummy.ControlHandle = INVALID_HANDLE;
	dummy.ControlPtr = (VOID*)(UINTN) &msgbox;
	dummy.ControlFlags.ControlVisible = TRUE;

	gMsgBox.Initialize( gInfoBox, &dummy );

	gMsgBox.SetType( gInfoBox, MSGBOX_TYPE_NULL );
	gMsgBox.Draw( gInfoBox );
	if(TimeLimit != 0)
	{
		Status = gBS->CreateEvent(
									EventType,
									EFI_TPL_CALLBACK,
									_DisplayInfoCancelTimer,
									NULL,
									RetEvent
					 			 );	

		if ( EFI_ERROR( Status ) )
			return Status;

		Status = gBS->SetTimer( *RetEvent, TimerRelative, MultU64x32((UINT64)TIMER_ONE_SECOND, (UINT32)TimeLimit) );

		if ( EFI_ERROR( Status ) )
			TimerStopTimer( RetEvent );
	}
		TimerEnabled = TRUE;
		//TimerEnabled will change its value when _DisplayInfoCancelTimer is called.
		//_DisplayInfoCancelTimer is called if timer expires or a key is pressed
		while( TRUE == TimerEnabled) 
		{
					if ( action->Input.Type != ACTION_TYPE_NULL )
						gAction.ClearAction( action );
					if ( gAction.GetAction( action ) != EFI_SUCCESS )
							continue;
					Status = gMsgBox.HandleAction( gInfoBox, action );
					if(EFI_SUCCESS == Status)
								_DisplayInfoCancelTimer( *RetEvent, NULL);
		}
	return Status;
}

#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
/**
    fucntion to read the a key using AMI Key Code Protocol

    @param pAmiKey 
    @param Timeout 

    @retval EFI_STATUS

**/
EFI_STATUS ActionReadKeyWithAMIKeyCode( AMI_EFI_KEY_DATA *pAmiKey, UINT64 Timeout )
{
	EFI_STATUS Status;
	EFI_STATUS StatusFlush;
	EFI_EVENT EventList[2] = { NULL, NULL };
	UINTN Count = 1, Index = 1;
    AMI_EFI_KEY_DATA    KeyFlush;

    AMI_EFIKEYCODE_PROTOCOL *pKeyCodeProtocol;
    Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gAmiEfiKeycodeProtocolGuid, (void*)&pKeyCodeProtocol);
        if (EFI_ERROR(Status)) return Status;

	if ( Timeout )
	{
		EventList[0] = pKeyCodeProtocol->WaitForKeyEx;

		Status = TimerCreateTimer( &EventList[1], NULL, NULL, TimerRelative, Timeout, EFI_TPL_CALLBACK );
		if ( EFI_ERROR( Status ) )
			return Status;

		Count++;

		gBS->WaitForEvent( Count, EventList, &Index );
	
		TimerStopTimer( &EventList[1] );

		if ( Index == 1 )
			return EFI_NOT_READY;
	}

    Status = pKeyCodeProtocol->ReadEfikey( pKeyCodeProtocol, pAmiKey );
    //// If it Partial Key make the Status as Error to ignore 
    // the Partial Key.
    if((pAmiKey->KeyState.KeyToggleState & KEY_STATE_EXPOSED) ==  KEY_STATE_EXPOSED) {
        if( (!EFI_ERROR( Status )) && (pAmiKey->Key.ScanCode==0) && (pAmiKey->Key.UnicodeChar==0)) {
            Status = EFI_NOT_READY;
        }
    }

	if ( ! EFI_ERROR( Status ) && FlushKeysAfterRead())//check for the FLUSH_KEYS_AFTER_READ token before flushing the keys
	{
		do
		{
        	StatusFlush = pKeyCodeProtocol->ReadEfikey( pKeyCodeProtocol, &KeyFlush );
            //// If it Partial Key make the Status as Error to ignore 
            // the Partial Key.
            if((KeyFlush.KeyState.KeyToggleState & KEY_STATE_EXPOSED) ==  KEY_STATE_EXPOSED) {
                if( (!EFI_ERROR( Status )) && (KeyFlush.Key.ScanCode==0) && (KeyFlush.Key.UnicodeChar==0)) {
                    StatusFlush = EFI_NOT_READY;
                }
            }
		} while ( ! EFI_ERROR( StatusFlush ) );

	}

	return Status;
}
#endif

/**
    fucntion to handle the read key action

    @param pAmiKey 
    @param Timeout 

    @retval EFI_STATUS

**/
EFI_STATUS ActionReadKey( AMI_EFI_KEY_DATA *pAmiKey, UINT64 Timeout )
{
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
	return ActionReadKeyWithAMIKeyCode( pAmiKey, Timeout );
#else
	return _ActionReadKey( pAmiKey, Timeout );
#endif

}

/**
    function to read a key stroke

    @param Key 
    @param KeyData 

    @retval EFI_STATUS

**/
EFI_STATUS AMIReadKeyStroke(EFI_INPUT_KEY *Key,AMI_EFI_KEY_DATA *KeyData)
{
	EFI_STATUS Status;
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
    AMI_EFIKEYCODE_PROTOCOL *pKeyCodeProtocol;

    Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gAmiEfiKeycodeProtocolGuid, (void*)&pKeyCodeProtocol);
    if (EFI_ERROR(Status))
   	 return Status;

	
    if ( IsWaitForKeyEventSupport() )
    {
		//
		// Check whether the keystroke is present in the system with the help of boot-service in the higher TPL at which 
		// console driver's 'WaitForKey' event's  notification function is registered . 
		// If keystroke is not present , do not do anything at the lower TPL at which code is running
		// 	
	   	Status = gBS->CheckEvent(pKeyCodeProtocol->WaitForKeyEx);
	   	if (Status == EFI_NOT_READY) 
				return Status;    		
    }
	
	 Status = pKeyCodeProtocol->ReadEfikey( pKeyCodeProtocol, KeyData );
	
	 // If it Partial Key make the Status as Error to ignore 
	 // the Partial Key.
	 if((KeyData->KeyState.KeyToggleState & KEY_STATE_EXPOSED) ==  KEY_STATE_EXPOSED) {
		  if( (!EFI_ERROR( Status )) && (KeyData->Key.ScanCode==0) && (KeyData->Key.UnicodeChar==0)) {
				Status = EFI_NOT_READY;
		  } 
	 }
	 *Key = KeyData->Key;
#else
    if ( IsWaitForKeyEventSupport() )
    {
		//
		// Check whether the keystroke is present in the system with the help of boot-service in the higher TPL at which 
		// console driver's 'WaitForKey' event's  notification function is registered . 
		// If keystroke is not present , do not do anything at the lower TPL at which code is running
		//		
   	Status = gBS->CheckEvent( gST->ConIn->WaitForKey );
		if (Status == EFI_NOT_READY) 
			return Status;
    }
    Status = gST->ConIn->ReadKeyStroke( gST->ConIn, Key );
#endif
	return Status;
}


/**
    function to check whether the password char is valid

    @param Key 
    @param KeyData 
    @param StrIndex 
    @param PasswordLength 
    @param OutChar 

    @retval EFI_STATUS

**/
EFI_STATUS HelperIsPasswordCharValid(EFI_INPUT_KEY *Key,AMI_EFI_KEY_DATA *KeyData,UINTN StrIndex,UINTN PasswordLength, CHAR16 *OutChar)
{
#if (TSE_USE_AMI_EFI_KEYCODE_PROTOCOL )
	switch( SETUP_STORE_KEYCODE_PASSWORD )
	{
		case PW_EFI_KEY:
			if ( (StrIndex < PasswordLength) && (KeyData->EfiKeyIsValid) ) {
				/// Get EFI Key with the help of AMI key code protocol
				*OutChar = (0x8000)^(CHAR16)(KeyData->EfiKey);
				return EFI_SUCCESS;
			}
		break;
		case PW_SCAN_CODE:
			if ( (StrIndex < PasswordLength) && (KeyData->PS2ScanCodeIsValid) ) {
				*OutChar = (CHAR16)(KeyData->PS2ScanCode);
				return EFI_SUCCESS;
			}
		break;
		default:
			break;
	}

#endif
	// if tokens not enabled then, check whether the character is alphanumeric and return the key
	if ( CheckIsAllowedPasswordChar(Key->UnicodeChar) && ( StrIndex < PasswordLength ) )
	{
		*OutChar = Key->UnicodeChar;
		return EFI_SUCCESS;
	}

	// If AMI key code support TOKEN not enabled and the character is not alphanumeric then return as Unsupported.
    return EFI_UNSUPPORTED;
}

/**
    function to handle the popup password edit

    @param pPopupEdit 
    @param pData 

    @retval EFI_STATUS

**/
EFI_STATUS PopupEditHandlePassword( VOID *pPopupEdit, VOID *pData)
{
#if (TSE_USE_AMI_EFI_KEYCODE_PROTOCOL && SETUP_STORE_KEYCODE_PASSWORD)
	POPUPEDIT_DATA *PopupEdit = pPopupEdit;
	ACTION_DATA *Data = pData;

	// Check whether the popup edit type is Password, 
    if(( PopupEdit->PopupEditType == POPUPEDIT_TYPE_PASSWORD )&&( EfiStrLen(PopupEdit->Text) < PopupEdit->TextWidth ))
    {
        if(PopupEdit->ControlActive ==FALSE)
		{
			// copy the password text into temp and make the control active.
            MemCopy(PopupEdit->TempText,PopupEdit->Text,PopupEdit->TextWidth* sizeof(UINT16));
			PopupEdit->ControlActive = TRUE; 
		}
		switch( SETUP_STORE_KEYCODE_PASSWORD )
		{
			case PW_EFI_KEY:
				if(Data->Input.Data.AmiKey.EfiKeyIsValid)
					PopupEdit->Text[EfiStrLen(PopupEdit->Text)] = 0x8000 ^(CHAR16)(Data->Input.Data.AmiKey.EfiKey); 			
				else
					return EFI_UNSUPPORTED;
			break;
	
			case PW_SCAN_CODE:
				if(Data->Input.Data.AmiKey.PS2ScanCodeIsValid)
					PopupEdit->Text[EfiStrLen(PopupEdit->Text)] = (CHAR16)(Data->Input.Data.AmiKey.PS2ScanCode);
				else
					return EFI_UNSUPPORTED;
			break;
		}
		// set end of string and return success
        PopupEdit->Text[EfiStrLen(PopupEdit->Text)] = 0x0 ; 

		return EFI_SUCCESS;
    }
#endif
    	return EFI_UNSUPPORTED;
}

//TSE_CLEAR_USERPW_WITH_ADMINPW
/**
    fucntion to clear the user password if admin password is empty,
    Only Valid with Password Local

    @param callbackData 
    @param saveData 

    @retval VOID

**/
extern BOOLEAN TSEPwdSavetoNvram(VOID);
VOID ClearUserPasswordPolicy(CALLBACK_PASSWORD *callbackData,CHAR16 *saveData)
{
    if(
        (VARIABLE_ID_AMITSESETUP == callbackData->Variable)&&
        (STRUCT_OFFSET(AMITSESETUP,AdminPassword) == callbackData->Offset)&&
        (0 == ((CHAR16 *)callbackData->Data)[0])
    )
    {
        //Admin password is being cleared
		//Clear user pw; write the same thing for user
    	if((TSEPwdSavetoNvram())&&( VARIABLE_ID_AMITSESETUP == callbackData->Variable ))
		{
		    AMITSESETUP *TempSaveData = (AMITSESETUP *)NULL;
			UINTN size=0;
			
			//TempSaveData = (AMITSESETUP *) EfiLibAllocateZeroPool(sizeof(AMITSESETUP));  //Memory leak fix
			size = sizeof(AMITSESETUP);

			TempSaveData = VarGetNvram(VARIABLE_ID_AMITSESETUP, &size);
			MemCopy( ((UINT8*)(TempSaveData))+(STRUCT_OFFSET(AMITSESETUP,UserPassword)), saveData, callbackData->Length );
			VarSetNvram( VARIABLE_ID_AMITSESETUP, TempSaveData, sizeof(AMITSESETUP));
			MemFreePointer( (VOID **)&TempSaveData );	       	
		}
    	 else{
    		 VarSetValue(VARIABLE_ID_AMITSESETUP, STRUCT_OFFSET(AMITSESETUP,UserPassword), callbackData->Length, saveData);
    	 }
    }
}

/**
    function to update the ide password

    @param control 
    @param saveData 

    @retval EFI_STATUS

**/
EFI_STATUS TseFramePwdCallbackIdePasswordUpdate ( CONTROL_DATA *control,CHAR16 *saveData)
{
    BOOLEAN bCheckUser = FALSE;
	VOID * data =control->ControlData.ControlPtr;

	if (control->ControlData.ControlVariable == VARIABLE_ID_IDE_SECURITY )  // IDE Password
	{
        if(/*data->QuestionId*/UefiGetQuestionOffset(data) == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword))
            bCheckUser = TRUE;
		TSEIDEPasswordUpdate( (UINT32)gCurrIDESecPage, saveData, bCheckUser );
		return EFI_SUCCESS;
	}
	else
    	return EFI_UNSUPPORTED;
}

/**
    Function to tell the IDE password installed or not

    @param popuppassword 
				

    @retval EFI_STATUS

**/
EFI_STATUS TseCheckForIDEPasswordInstalled(POPUP_PASSWORD_DATA *popuppassword)
{
	  EFI_STATUS Status = EFI_UNSUPPORTED;
	  VOID* ControlPtr = popuppassword->ControlData.ControlPtr;
	  
	    
	    if(popuppassword->ControlData.ControlVariable == VARIABLE_ID_IDE_SECURITY ) 
	    {
			//if drive is locked ask for the old password to unlock the drive
			VOID* DataPtr = TSEIDEPasswordGetDataPtr(gCurrIDESecPage);
			BOOLEAN bCheckUser = FALSE;
			BOOLEAN EnabledBit = FALSE;
			UINTN size = 0;
			IDE_SECURITY_CONFIG *ideSecConfig;
		
			ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );
				// Suppress the warnings from static code analyzer
			if (NULL == ideSecConfig) {
			    return EFI_NOT_FOUND;
			}
			if(ideSecConfig->Enabled) EnabledBit = TRUE;
		
			if(EnabledBit)
			{
			    if(UefiGetQuestionOffset(ControlPtr) /*data->QuestionId*/ == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword))
				bCheckUser = TRUE;
		
			    if(bCheckUser || ideSecConfig->MasterPasswordStatus)
			    {
					MemFreePointer((VOID **) &ideSecConfig);
					return EFI_SUCCESS;
					
			    }
			}
			else{
				return EFI_UNSUPPORTED;
			}
			return EFI_SUCCESS;
		}
		return EFI_UNSUPPORTED;
}
/**
    Function to authenticate the IDE password

    @param popuppassword 
    @param AbortUpdate 
    @param data 

    @retval EFI_STATUS

**/
EFI_STATUS TsePopupPwdAuthenticateIDEPwd(POPUP_PASSWORD_DATA *popuppassword, BOOLEAN *AbortUpdate,VOID *data)
{
   	EFI_STATUS Status = EFI_UNSUPPORTED;
    CHAR16 *Text=NULL;
	if(popuppassword->ControlData.ControlVariable == VARIABLE_ID_IDE_SECURITY ) 
	{
		//if drive is locked ask for the old password to unlock the drive
		VOID* DataPtr = TSEIDEPasswordGetDataPtr(gCurrIDESecPage);
        BOOLEAN bCheckUser = FALSE;
        BOOLEAN EnabledBit = FALSE;
        UINTN size = 0;
        IDE_SECURITY_CONFIG *ideSecConfig;

        ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );
        if (NULL == ideSecConfig) {
            return EFI_NOT_FOUND;
        }
        if(ideSecConfig->Enabled) EnabledBit = TRUE;

        if(EnabledBit)
        {
            if(UefiGetQuestionOffset(data) /*data->QuestionId*/ == STRUCT_OFFSET(IDE_SECURITY_CONFIG,IDEUserPassword))
                bCheckUser = TRUE;

            if(bCheckUser || ideSecConfig->MasterPasswordStatus)
            {
    			Status = _DoPopupEdit( popuppassword, (IsShowPromptStringAsTitle() ? STRING_TOKEN(STR_OLD_PSWD_LABEL): STRING_TOKEN(STR_OLD_PSWD)), &Text);
                if(EFI_SUCCESS != Status )
                {
                    *AbortUpdate = TRUE;
                }
    			else
    			{
                    Status = TSEIDEPasswordAuthenticate( Text, DataPtr, bCheckUser );
                    if(EFI_ERROR( Status ))
        			{
        				CallbackShowMessageBox( (UINTN)gInvalidPasswordFailMsgBox, MSGBOX_TYPE_OK );
        				*AbortUpdate = TRUE;
        			}
					StringZeroFreeMemory ((VOID **)&Text);
    			}
            }
		}

        MemFreePointer((VOID **) &ideSecConfig);
		return EFI_SUCCESS;
	}
	return EFI_UNSUPPORTED;
}

/**
    function to update the IDE password

    @param VOID

    @retval VOID

**/
VOID TsePopupPwdUpdateIDEPwd (VOID)
{
    UINTN size = 0;
    IDE_SECURITY_CONFIG *ideSecConfig;

    ideSecConfig = VarGetVariable( VARIABLE_ID_IDE_SECURITY, &size );
    if (NULL == ideSecConfig) {
        return;
    }
    TSEIDEUpdateConfig((VOID*)ideSecConfig, gCurrIDESecPage);
    VarSetValue (VARIABLE_ID_IDE_SECURITY, 0, size, ideSecConfig);		//Updating the cache
	if (gApp != NULL)
		gApp->CompleteRedraw = TRUE;	// Complete redraw bcoz if User password set it should grayout the Master password.
    MemFreePointer((VOID **)&ideSecConfig);		
}


#if TSE_SEPERATE_EFI_LEGACY_OPTIONS
/**
    function to check for legacy option

    @param Option 

    @retval TRUE/FALSE

**/
BOOLEAN IsLegacyOption(UINT16 Option)
{
    BOOT_DATA *pBootData;

    pBootData = BootGetBootData(Option);

    if(pBootData)
    {
        if(BBSValidDevicePath( pBootData->DevicePath ))
            return TRUE;
    }
	
    return FALSE;
}

/**
    function to regroup the boot options

    @param optionList 
    @param OrgOptionList 

    @retval VOID

**/
VOID ReGroupOptions(UINT16 *optionList,UINT16 *OrgOptionList)
{
    UINTN src, dest, i;
    UINT16 *TempList;
	UINTN LegDevStartIndex=0xFFFF,LegDevStopIndex=0xFFFF;

    TempList = EfiLibAllocatePool(gBootOptionCount * sizeof(UINT16));
    MemCopy(TempList, optionList, gBootOptionCount * sizeof(UINT16));

    dest = 0;
    //Copy all first EFI option
    for(src=0; (src<gBootOptionCount)&&(DISABLED_BOOT_OPTION!=TempList[src]); src++)
    {
        if(IsLegacyOption(TempList[src]))
            break;

        optionList[dest] = TempList[src];
        dest++;
    }

    //Gather all Legacy option
    for(i = src; (i<gBootOptionCount)&&(DISABLED_BOOT_OPTION!=TempList[i]); i++)
    {
        if(IsLegacyOption(TempList[i]))
        {
            optionList[dest] = TempList[i];
            dest++;
			if(LegDevStartIndex == 0xFFFF)
				LegDevStartIndex = i;		// Start Index
			LegDevStopIndex = i;			// End Index Updated for Each Legacy Entry
        }
    }

    //Gather all EFI option
    for(i = src; (i<gBootOptionCount)&&(DISABLED_BOOT_OPTION!=TempList[i]); i++)
    {
        if(!IsLegacyOption(TempList[i]))
        {
			if(OrgOptionList != NULL)
			// Don't Allow Mixing of Legacy and UEFI Boot Options
				if( (i > LegDevStartIndex) && (i < LegDevStopIndex))
				{
					// Restore the Orginal Boot Options
					MemCopy( optionList, OrgOptionList, gBootOptionCount*sizeof(UINT16));
					break;
				}

            optionList[dest] = TempList[i];
            dest++;
        }
    }

    MemFreePointer((VOID **) &TempList);
}
#endif


#ifdef TSE_FOR_APTIO_4_50
/**
    reads the NV Varaiable from Nram buffer using NVLib.

    @param variable NVRAM_STORE_INFO *pInfo , UINTN *size

    @retval VOID

**/
VOID *TSEGetNVVariable( UINT32 variable, NVRAM_STORE_INFO *pInfo , UINTN *size )
{
	VOID *buffer = NULL;
#if TSE_NVRAM_DEFAULTS_SUPPORT
	EFI_STATUS Status = EFI_UNSUPPORTED;
	VARIABLE_INFO *varInfo;
 //UINT8 Flags;  To fix the CPU hangs when Core is update  to 4.6.4.1 

	varInfo = VarGetVariableInfoIndex( variable );
	if ( varInfo == NULL )
		return buffer;

	*size=0;
	Status = NvGetVariable(
		varInfo->VariableName,
		&varInfo->VariableGuid,
		NULL,
		size,
		buffer,
		pInfo, /*&Flags*/ NULL 			//To fix the CPU hangs when Core is update  to 4.6.4.1 
		 
		);

	if ( Status != EFI_BUFFER_TOO_SMALL )
		return buffer;

	buffer = EfiLibAllocatePool( *size );
	if ( buffer == NULL )
		return buffer;

	Status = NvGetVariable(
		varInfo->VariableName,
		&varInfo->VariableGuid,
		NULL,
		size,
		buffer,
		pInfo,/*&Flags*/ NULL 			//To fix the CPU hangs when Core is update  to 4.6.4.1 
		);
	    
	if ( EFI_ERROR( Status ) )
	    MemFreePointer( (VOID *)&buffer );

#endif //TSE_NVRAM_DEFAULTS_SUPPORT
	return buffer;
}

/**
    Updated the Variable buffer with NVRam Defaults buffer.

    @param index NVRAM_STORE_INFO *NvInfo, NVRAM_VARIABLE * VarPtr

    @retval VOID

**/
VOID UpdateNVDefautls(UINT32 index, NVRAM_STORE_INFO *NvInfo, NVRAM_VARIABLE * VarPtr)
{
	UINTN Nvsize;
	UINT8 * NvVarBuffer;

	Nvsize = 0;
	NvVarBuffer = TSEGetNVVariable( index, NvInfo , &Nvsize );
	if(NvVarBuffer && Nvsize)
	{
		if(index == VARIABLE_ID_AMITSESETUP)
		{
			if(!IsTseLoadPasswordOnDefaults())
			{
				// To preserve password
				if(Nvsize>=sizeof(AMITSESETUP))
				{
					if(VarPtr->Buffer && (VarPtr->Size>=sizeof(AMITSESETUP)))
					{
						MemCopy(((AMITSESETUP*)NvVarBuffer)->UserPassword, ((AMITSESETUP*)VarPtr->Buffer)->UserPassword,TsePasswordLength*sizeof(CHAR16));
						MemCopy(((AMITSESETUP*)NvVarBuffer)->AdminPassword, ((AMITSESETUP*)VarPtr->Buffer)->AdminPassword,TsePasswordLength*sizeof(CHAR16));
					}
				}
			}
		}

		if(VarPtr->Size >= Nvsize)
		{
			MemCopy(VarPtr->Buffer,NvVarBuffer,Nvsize);
			MemFreePointer( (VOID **)&NvVarBuffer );
		}
		else
		{
			MemFreePointer( (VOID **)&VarPtr->Buffer);
			VarPtr->Buffer = NvVarBuffer;
			VarPtr->Size = Nvsize;
		}
	}

}
#endif


/**
    function build the AMI defaults

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS TseVarBuildAMIDefaults(VOID)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32	page, control;
	VOID *defaults4FirstBootVar = NULL;
#if TSE_NVRAM_DEFAULTS_SUPPORT
	NVRAM_VARIABLE *OptvarPtr;
	NVRAM_VARIABLE *FailsafevarPtr;
	UINT32 index;

#ifdef TSE_FOR_APTIO_4_50
	NVRAM_STORE_INFO NvInfo={0};
	UINTN Nvsize;
	UINT8 * NvVarBuffer=NULL;
	UINT32 attribute=7;
#endif
#endif
	//Setting "ConstructDefaults4FirstBoot" variable in NVRAM for first boot only
	if ( IsSupportDefaultForStringControl() )
	{
		EFI_GUID ConstructDefaults4FirstBootGuid = CONSTRUCT_DEFAULTS_FIRST_BOOT_GUID;
		UINTN varSize = 0;
		
		if(FormBrowserHandleValid())
			defaults4FirstBootVar = (VOID*)1;
		else
		{	
			defaults4FirstBootVar = (VOID *)VarGetNvramName (L"ConstructDefaults4FirstBoot", &ConstructDefaults4FirstBootGuid, NULL, &varSize);
			if (NULL == defaults4FirstBootVar)
			{
				VarSetNvramName( L"ConstructDefaults4FirstBoot", &ConstructDefaults4FirstBootGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, &varSize, sizeof (varSize) );
			}
		}
 	}
	
	gFailsafeDefaults = EfiLibAllocateZeroPool( sizeof(NVRAM_VARIABLE) * gVariables->VariableCount );
	if ( gFailsafeDefaults == NULL )
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
	gOptimalDefaults = EfiLibAllocateZeroPool( sizeof(NVRAM_VARIABLE) * gVariables->VariableCount );
	if (gOptimalDefaults == NULL )
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
#if TSE_NVRAM_DEFAULTS_SUPPORT
	// Load the Current NVRAM variable then update the defaults.
	OptvarPtr = gOptimalDefaults;
	FailsafevarPtr = gFailsafeDefaults;
	for ( index = 0; index < gVariables->VariableCount; index++, OptvarPtr++, FailsafevarPtr++)
	{
		if ((VARIABLE_ID_AMITSESETUP == index) && (TseDefaultSetupPasswordSupported ()))
		{
			OptvarPtr->Buffer = VarGetNvram( index, &OptvarPtr->Size );
			if (OptvarPtr->Buffer)
			{
				MemSet(OptvarPtr->Buffer, ((TsePasswordLength * 2) * sizeof(CHAR16)), 0 );		//Preserving quietboot details
				if ((!DefaultSetupPwdAtFirstBootOnly ()))
				{
					GetDefaultPassword (AMI_PASSWORD_NONE, &OptvarPtr->Buffer);
				}
			}
		}
		else
		{
			OptvarPtr->Buffer = VarGetNvram( index, &OptvarPtr->Size );
		}
		if(OptvarPtr->Size)
		{
			// init mfg defautls from Std Defaults 
			FailsafevarPtr->Buffer = EfiLibAllocateZeroPool (OptvarPtr->Size);
			if (NULL == FailsafevarPtr->Buffer)
				continue;
			if (NULL == OptvarPtr->Buffer)			//Add the fix for: If name value string is not able to retrieve then system hang
			{
				OptvarPtr->Buffer = EfiLibAllocateZeroPool (OptvarPtr->Size);
				if (NULL == OptvarPtr->Buffer)
					continue;
			}
			MemCopy(FailsafevarPtr->Buffer, OptvarPtr->Buffer,OptvarPtr->Size);
		}
		else
			FailsafevarPtr->Buffer = NULL;
		FailsafevarPtr->Size = OptvarPtr->Size;
	}
#endif	//TSE_NVRAM_DEFAULTS_SUPPORT

	// this is the AMIBCP compatible user defaults
	for ( page = 0; page < gPages->PageCount; page++ )
	{
		PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[page]);

		if(pageInfo->PageHandle ==0)
			continue;
		if (NULL == gApp)
		{
			gtempCurrentPage = page;					//Will be used in UefiGetQuestionOffset. Since gApp will not be filled but we need handle to find name value variable so
		}												//saving current page
		else
		{
			gApp->CurrentPage = page;
		}
			
		for ( control= 0; control < pageInfo->PageControls.ControlCount; control++ )
		{
			CONTROL_INFO *controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

			if ( ( controlInfo->ControlVariable == VARIABLE_ID_LANGUAGE ) || ( controlInfo->ControlVariable == VARIABLE_ID_BOOT_ORDER ) || (controlInfo->ControlVariable == VARIABLE_ID_BBS_ORDER) )
				continue;

			//This will construct the defaults for controls having valid size and no conditional expression
			//This section will be overridden inside the GetSetDefaultsFromControlPtr() call
			if ( controlInfo->ControlDataWidth > 0  && 0 == controlInfo->ControlFlags.ControlEvaluateDefault)
			{
				VOID *ifrData = controlInfo->ControlPtr;
				UINT8 *failValue = NULL, *optValue = NULL;
				UINT32 ControlVarOffset = 0;

				if ( ifrData == NULL )
					continue;

				failValue = (UINT8 *)controlInfo + sizeof(CONTROL_INFO);
				optValue = failValue + controlInfo->ControlDataWidth;

				ControlVarOffset = UefiGetQuestionOffset(ifrData);
				//Updating gOptimalDefaults and gFailSafeDefaults for stringType controls based on controlPtr
				if (IsSupportDefaultForStringControl () && controlInfo->ControlType == CONTROL_TYPE_POPUP_STRING)
				{
					CHAR16 *failDefStr = NULL, *optimalDefStr = NULL;
					UINTN Size = 0;
					
					if (*(UINT16*)(failValue))
					{
						failDefStr = (VOID*)HiiGetString( controlInfo->ControlHandle, *(UINT16*)(failValue));
						if (NULL == failDefStr)
							continue;
						
						if (!EfiStrLen(failDefStr)) 
                        {
                            Size = sizeof(CHAR16);                          //Since empty string is setting as default we can allocate 2 bytes
                            MemFreePointer( (VOID **)&failDefStr);
                            failDefStr = (CHAR16 *)EfiLibAllocateZeroPool (sizeof(CHAR16));
                        }
                        else
                            Size  = EfiStrLen(failDefStr)*sizeof(CHAR16);
						
						//Updating gFailsafeDefaults buffer.
						_VarGetSetValue( VAR_COMMAND_SET_VALUE, gFailsafeDefaults, controlInfo->ControlVariable, ControlVarOffset, Size, (VOID*)failDefStr );
						if (failDefStr)
							MemFreePointer( (VOID **)&failDefStr );
						
					}
					
					if (*(UINT16*)(optValue))
					{
						optimalDefStr = (VOID*)HiiGetString( controlInfo->ControlHandle, *(UINT16*)(optValue));
						if (NULL == optimalDefStr)
							continue;
						
						if (!EfiStrLen(optimalDefStr)) 
						{
						    Size = sizeof(CHAR16);                          //Since empty string is setting as default we can allocate 2 bytes
						    MemFreePointer( (VOID **)&optimalDefStr);
						    optimalDefStr = (CHAR16 *)EfiLibAllocateZeroPool (sizeof(CHAR16));
						}
						else
						    Size  = EfiStrLen(optimalDefStr)*sizeof(CHAR16);
						
						//Updating gOptimalDefaults buffer.
						_VarGetSetValue( VAR_COMMAND_SET_VALUE, gOptimalDefaults, controlInfo->ControlVariable, ControlVarOffset, Size, (VOID*)optimalDefStr );
					}
					
					if (NULL == defaults4FirstBootVar && optimalDefStr) //Modify NVRAM for first time for string type controls based on defaults from controlPtr
					{
						VOID *optiBuffer = NULL, *failBuffer = NULL;
						VARIABLE_INFO 	*varInfo = (VARIABLE_INFO *)NULL;
						UINTN size = 0;
						EFI_STATUS status = EFI_SUCCESS;
						
						varInfo = VarGetVariableInfoIndex (controlInfo->ControlVariable);
						
						optiBuffer = VarGetNvram( controlInfo->ControlVariable, &size );						
						if (NULL == optiBuffer)
						{
							continue;
						}
						
                        if (!EfiStrLen(optimalDefStr)) 
                        {
                            Size = sizeof(CHAR16);                          //Since empty string is setting as default we can allocate 2 bytes
                            MemFreePointer( (VOID **)&optimalDefStr);
                            optimalDefStr = (CHAR16 *)EfiLibAllocateZeroPool (sizeof(CHAR16));
                        }
                        else
                            Size  = EfiStrLen(optimalDefStr)*sizeof(CHAR16);

						MemCopy ( ((UINT8*)optiBuffer)+ControlVarOffset, (UINT8*)optimalDefStr, Size );
						
						status = VarSetNvramName( varInfo->VariableName, &varInfo->VariableGuid, varInfo->VariableAttributes, optiBuffer, size );
						if (optiBuffer)
							MemFreePointer( (VOID **)&optiBuffer );
					}
					if (optimalDefStr)
						MemFreePointer( (VOID **)&optimalDefStr );
					
				}
				//EIP-93340 End
				else
				{				
					_VarGetSetValue( VAR_COMMAND_SET_VALUE, gFailsafeDefaults, controlInfo->ControlVariable, ControlVarOffset, controlInfo->ControlDataWidth, failValue );
					_VarGetSetValue( VAR_COMMAND_SET_VALUE, gOptimalDefaults, controlInfo->ControlVariable, ControlVarOffset, controlInfo->ControlDataWidth, optValue );
				}
			}
		}
	}

#ifdef TSE_FOR_APTIO_4_50
#if TSE_NVRAM_DEFAULTS_SUPPORT	//EIP-47260: To build TSE without NVRAM module support.
	// StdDefaults
	Nvsize = 0;
	NvVarBuffer = VarGetNvramName( L"StdDefaults", &DefaultsGuid, &attribute, &Nvsize );
	if(NvVarBuffer && Nvsize)
	{
		NvInfo.NvramAddress = NvVarBuffer;
		NvInfo.NvramSize = Nvsize;
		NvInitInfoBuffer(&NvInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );

		// Update the defaults.
		OptvarPtr = gOptimalDefaults;
		for ( index = 0; index < gVariables->VariableCount; index++, OptvarPtr++)
		{
			UpdateNVDefautls(index, &NvInfo, OptvarPtr);
		}
		MemFreePointer( (VOID **)&NvVarBuffer );
	}

	// MfgDefaults
	Nvsize = 0;
	NvVarBuffer = VarGetNvramName( L"MfgDefaults", &DefaultsGuid, &attribute, &Nvsize );
	if(NvVarBuffer && Nvsize)
	{
		NvInfo.NvramAddress = NvVarBuffer;
		NvInfo.NvramSize = Nvsize;
		NvInitInfoBuffer(&NvInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );

		FailsafevarPtr = gFailsafeDefaults;
		for ( index = 0; index < gVariables->VariableCount; index++, FailsafevarPtr++ )
		{
			UpdateNVDefautls(index, &NvInfo, FailsafevarPtr);
		}
		MemFreePointer( (VOID **)&NvVarBuffer );
	}
	//Updating optimalDefaults and gFailsafeDefaults for STRING_TYPE controls 
	//based on defalut stringID present in controlPtr
		GetSetDefaultsFromControlPtr(gOptimalDefaults, gFailsafeDefaults);
#endif	//TSE_NVRAM_DEFAULTS_SUPPORT
#endif
DONE:
    return Status;
}

/**
    function build the AMI defaults

    @param VOID

    @retval boot order string

**/
UINT16 * TseBootNowinBootOrderInit(VOID)
{
    UINT16 *BootOrder=NULL;
    UINTN size = 0;
    UINTN j,k,i;

	BootOrder = HelperGetVariable(VARIABLE_ID_BOOT_ORDER, L"BootOrder", &gEfiGlobalVariableGuid, NULL, &size );
	if (NULL == BootOrder) {
		ASSERT (0);
		return NULL;
	}
    //Find the first disabled option
    for ( i = 0; i < gBootOptionCount; i++ )
    {
        if ( DISABLED_BOOT_OPTION == BootOrder[i] )
            break;
    }

    if(i<gBootOptionCount)
    {
        //There are disabled options replace them with valid options
        for(j=0;j<gBootOptionCount;j++)
        {
            for(k=0;k<gBootOptionCount;k++)
            {
                if(BootOrder[k] == gBootData[j].Option)
                    break;
            }

            if(k >= gBootOptionCount)
            {
                //gBootData[j].Option is not present in BootOrder; fill it
                BootOrder[i] = gBootData[j].Option;
                i++;
            }
        }
    }
	return BootOrder;
}

/**
    function to check the shift state value

    @param VOID

    @retval 1 valid shift state
    @retval 0 invalid shift state

**/
BOOLEAN CheckShiftState(UINT32 KeyShiftState, UINT32 HotkeyState)
{
	if( KeyShiftState == HotkeyState)
		return 1;
	else
		return 0;
}

/**
    function to support the print screen.

    @param VOID

    @retval VOID

**/
VOID SupportPrintScreen()
{
	EFI_GUID guid = AMITSE_PRINT_SCREEN_PRESS_GUID;
	// Install the PrintScreen Notification. 
    EfiLibNamedEventSignal (&guid);
}

/**
    function to check the print screen supprot event.

    @param ScanCode 

    @retval TRUE/FALSE

**/
BOOLEAN SupportPrintScreenEvent(UINT16 ScanCode)
{
	if( ScanCode == TSE_PRN_SCRN_KEY_SCAN )
		return 1;
	else 
		return 0;
}

/**
    function to check Is any of ToggleStateKey (CapsLock,Numlock ScrollLock) Keys pressed.

    @param Data 

    @retval TRUE/FALSE

**/
BOOLEAN IsToggleStateKey(ACTION_DATA *Data)
{
	if(Data->Input.Type == ACTION_TYPE_KEY)
	{
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
		if(Data->Input.Data.AmiKey.EfiKey == EfiKeyCapsLock || Data->Input.Data.AmiKey.EfiKey ==  EfiKeySLck || Data->Input.Data.AmiKey.EfiKey ==  EfiKeyNLck)
			return TRUE;
		else
#endif
		{
			// Normal EFI_KEY will return ScanCode and Unicode as 0 for this toggleStateKeys.
			if(Data->Input.Data.AmiKey.Key.ScanCode || Data->Input.Data.AmiKey.Key.UnicodeChar)
				return FALSE;
			else
				return TRUE;
		}
	}
	else
		return FALSE;
}

/**
    function to Show menu with below option on selecting options under Boot Override 
    1) Override now and clear the BootNext data
    2) Override now but keep the BootNext data
    3) Boot to BootNext and clear BootNext data
    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS ShowBootOverrideSelectOption (VOID)
{

	UINT16 itemSelected = 0;
	UINT16	Title, Legend;
	UINT8 i;
	EFI_STATUS Status;
	POSTMENU_TEMPLATE *BootNextOverrideOptionsMenu = NULL; 

	UINT16 BootNextOverrideOptionString[3] = {STRING_TOKEN (STR_OVERRIDE_BOOTNEXT_OPTION_1), STRING_TOKEN (STR_OVERRIDE_BOOTNEXT_OPTION_2), STRING_TOKEN (STR_OVERRIDE_BOOTNEXT_OPTION_3)};

	BootNextOverrideOptionsMenu = EfiLibAllocateZeroPool ( sizeof( POSTMENU_TEMPLATE ) * BOOT_OVERRIDE_OPTION_MENU_COUNT);

	for ( i = 0; i < BOOT_OVERRIDE_OPTION_MENU_COUNT; i++ )
	{
		BootNextOverrideOptionsMenu[i].ItemToken = BootNextOverrideOptionString[i];
		BootNextOverrideOptionsMenu[i].Callback = (VOID*)NULL ;
		BootNextOverrideOptionsMenu[i].CallbackContext = (VOID*)NULL ;
		BootNextOverrideOptionsMenu[i].Attribute = AMI_POSTMENU_ATTRIB_FOCUS;
		BootNextOverrideOptionsMenu[i].Key.Key.ScanCode = 0 ;
		BootNextOverrideOptionsMenu[i].Key.Key.UnicodeChar = 0 ;
		BootNextOverrideOptionsMenu[i].Key.KeyShiftState = 0 ;
	}


	Title = STRING_TOKEN (STR_OVERRIDE_OPTION_MENU_TITLE) ;
	Legend = STRING_TOKEN (STR_OVERRIDE_OPTION_MENU_LEGEND) ;

	//To show menu with options
	Status = PostManagerDisplayMenu (gHiiHandle, Title, Legend, BootNextOverrideOptionsMenu, BOOT_OVERRIDE_OPTION_MENU_COUNT, &itemSelected);
	
	if( Status == EFI_SUCCESS )
	{
		UINTN size = 0;
		UINT16  *pBootNext = NULL;
		pBootNext = (UINT16 *)VarGetNvramName (L"BootNext", &gEfiGlobalVariableGuid, NULL, &size);

		//Override now and clear the BootNext data
		if ( BootNextOverrideOptionsMenu[itemSelected].ItemToken == BootNextOverrideOptionString[0] )
		{
		   if( pBootNext && (size == sizeof(UINT16)))
		   {	       
		   	//Clear Boot next variable
		    	VarSetNvramName (L"BootNext",
		     							&gEfiGlobalVariableGuid,
		                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
		                        NULL,
		                        0);
			}
			if(pBootNext)
				MemFreePointer((void **) &pBootNext);
			Status = EFI_SUCCESS;		
		}
		//Override now but keep the BootNext data
		else if(BootNextOverrideOptionsMenu[itemSelected].ItemToken == BootNextOverrideOptionString[1])
		{
			Status = EFI_SUCCESS;
			return Status;
		}
		//Boot to BootNext and clear BootNext data
		else if(BootNextOverrideOptionsMenu[itemSelected].ItemToken == BootNextOverrideOptionString[2])
		{
			if( pBootNext && (size == sizeof(UINT16)))
			{
				//Clear Boot next variable
		    	VarSetNvramName (L"BootNext",
		     							&gEfiGlobalVariableGuid,
		                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
		                        NULL,
		                        0);
				if(!gResetRequired)
				{
					if(!gDoNotBoot)
     				{
						if (!gUserTseCacheUpdated || _SaveValues( (UINTN)gSaveMsgBox )) //Check as Setup question is modified or not
						{
							gUserTseCacheUpdated = FALSE;
							gApp->ClearScreen = TRUE;
							StyleExit();
							if (gST->ConOut != NULL)
							{             	 		
								gST->ConOut->EnableCursor( gST->ConOut, FALSE );
								gST->ConOut->SetAttribute( gST->ConOut, (EFI_BACKGROUND_BLACK | EFI_WHITE));
								gST->ConOut->ClearScreen( gST->ConOut);
							}
							BootLaunchBootOption(*pBootNext, NULL, 0); //Boot to BootNext option
							StyleInit();
						}
     				}
				}				
			}
			if(pBootNext)
				MemFreePointer((void **) &pBootNext);

			Status = EFI_UNSUPPORTED;
		}
	}
	return Status;
}

/**
    function to Disaplay the information box and return the key pressed
    @param Attribut: Foreground and background color
    	 	  Message : Unicode string to be displayed.
    	 	  Key 	 : A pointer to the EFI_KEY value of the key that was pressed.

    @retval EFI_STATUS

**/
EFI_STATUS  ShowPopUpInfoBox(IN UINTN Attribute,IN CHAR16  *Message,OUT EFI_INPUT_KEY  *Key)
{
   EFI_STATUS Status;
   AMI_IFR_MSGBOX msgbox = { 0, 0, 0, INVALID_HANDLE, 0 };
   CONTROL_INFO dummy;
   ACTION_DATA *action = gMsgBoxAction;
   AMI_EFI_KEY_DATA    KeyData;
   
	msgbox.Title = 0;
	if(Message)
		msgbox.Text = HiiChangeString(gHiiHandle, msgbox.Text, Message);
    
	Status = gMsgBox.Create((void**) &gInfoBox );
	if ( EFI_ERROR( Status ) )
		return 1;

	MemSet( &dummy, sizeof(dummy), 0 );

	dummy.ControlHandle = INVALID_HANDLE;
	dummy.ControlPtr = (VOID*)(UINTN) &msgbox;
	dummy.ControlFlags.ControlVisible = TRUE;

	gMsgBox.Initialize( gInfoBox, &dummy );

	gMsgBox.SetType( gInfoBox, MSGBOX_TYPE_NULL );
	gMsgBox.Draw( gInfoBox );
	if(Key != NULL)
	{
		do {
			Status = AMIReadKeyStroke(Key,&KeyData);
		}while(EFI_ERROR(Status));
	}
	
	if ( gInfoBox != NULL )
		gMsgBox.Destroy( gInfoBox, TRUE );
	gInfoBox = NULL;
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
