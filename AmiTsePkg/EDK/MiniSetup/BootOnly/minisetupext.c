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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/minisetupext.c $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 4/25/12 6:08a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file minisetupext.c
    This file contains code for entrypoint and setup loop

**/

#include "minisetup.h"


//**************************** BBS POPUP CODE ********************************//

//#if SETUP_BBS_POPUP_ENABLE
extern EFI_GUID gBootNowCountGuid;
UINT16 gBBSPopupSel;

extern NVRAM_VARIABLE	*gOptimalDefaults;
extern UINTN CurrentScreenresolutionX, CurrentScreenresolutionY;

struct DevName
{
	CHAR16 *String;
	int  Index;
} *gDevNames;

int gHiStr		= 0;
int gStrIdx		= 0;
int gNumOfDev	= 0;
CHAR16 *gBlankSpace = L"                                           ";
#define START_ROW_IDX	10
#define START_COL_IDX	20
#define MAX_STR_LEN		28
#define MAX_LENGTH		(MAX_STR_LEN+7)
#define MAX_DISP_LINES	8

#define _ENTER_KEY		0x0D
#define _ESC_KEY		0x170000
#define _UP_ARROW		0x10000
#define _DOWN_ARROW		0x20000

void CheckStrings(void);
void SetScrollPosition(int Index, int DirFlg);
INTN BBS_GetKey(VOID);
EFI_STATUS BbsPopupMenu( VOID );
void HighLightLine(int UpDown, int NumLines);
void DownDisplayList(int Start, int End, int NumLines);
void UpDisplayList(int Start, int End, int flag, int NumLines);
VOID MainSetupLoopInitHook(VOID);
UINT16 _BootSetBootNowCount(VOID);
CHAR16 *HiiGetStringLanguage( VOID* handle, UINT16 token, CHAR16 *lang );
UINT16 HiiChangeStringLanguage( VOID* handle, UINT16 token, CHAR16 *lang, CHAR16 *string );
VOID GetTseBuildVersion(UINTN *TseMajor, UINTN *TseMinor, UINTN *TseBuild);
VOID GetTseBuildYear(UINT16 *TseBuildYear);
BOOLEAN 	IsMouseSupported(VOID);
BOOLEAN 	IsSoftKbdSupported(VOID);
BOOLEAN IsBootToDisabledBootOption(void);
BOOLEAN  IsTSECursorSupport (VOID);
/**
    Function to clear the graphics screen.

    @param void

    @retval void

**/
void ClearGrphxScreen(void)
{
	EFI_UGA_PIXEL UgaBlt={0,0,0,0};
	UINT32 HorizontalResolution = 0;
	UINT32 VerticalResolution = 0;

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
	if(gGOP)
	{
		HorizontalResolution = gGOP->Mode->Info->HorizontalResolution;
		VerticalResolution = gGOP->Mode->Info->VerticalResolution;
	    gGOP->Blt (
	    	gGOP,
	     	&UgaBlt,
	      	EfiBltVideoFill,
	       	0, 0,
	        0, 0,
	        HorizontalResolution, VerticalResolution,
	        0);
	}
#else
	if ( gUgaDraw != NULL )
	{
		gUgaDraw->GetMode (gUgaDraw, &HorizontalResolution, &VerticalResolution, &ColorDepth, &RefreshRate);
	    gUgaDraw->Blt (
	    	gUgaDraw,
	     	&UgaBlt,
	      	EfiUgaVideoFill,           
	       	0, 0,
	        0, 0,
	        HorizontalResolution, VerticalResolution,
	        0);
	}
#endif
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
EFI_STATUS DoBbsPopupInit(VOID)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT16  i=0, u16BootCount = 2, *pu16BootCount = NULL;
	UINTN	size = 0;
	UINT16 count;
	EFI_CONSOLE_CONTROL_SCREEN_MODE ScreenMode;

	PERF_END (0, AMITSE_TEXT("Boot"), NULL, 0);

	MainSetupLoopInitHook();

	MouseStop ();		//Logo is not clearing fully so adding here
	CleanUpLogo();

#ifndef STANDALONE_APPLICATION
	if ( gConsoleControl != NULL )
	{
		gConsoleControl->GetMode(gConsoleControl, &ScreenMode, NULL, NULL);
		if(EfiConsoleControlScreenGraphics == ScreenMode)
		{
			gConsoleControl->SetMode( gConsoleControl, EfiConsoleControlScreenText );
		}
	}
#endif
	MouseRefresh ();
	count = (UINT16)gBootOptionCount;

	if(gPopupMenuShowAllBbsDev)
	    count = _BootSetBootNowCount();

	VarSetNvramName( L"BootNowCount", &gBootNowCountGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &count, sizeof(count) );
	
	Status = VarLoadVariables( (VOID **)&gVariableList, (NVRAM_VARIABLE *)gOptimalDefaults );
	if ( EFI_ERROR( Status ) )
		return Status;

#if APTIO_4_00
		gMaxRows = MAX_ROWS;
		gMaxCols = MAX_COLS;
#endif

	pu16BootCount = VarGetNvramName( L"BootNowCount", &gBootNowCountGuid, NULL, &size );
	if(pu16BootCount)
		u16BootCount = *pu16BootCount;
	MemFreePointer((void**)&pu16BootCount);

	//For Boot Only TSE
	if (u16BootCount) 
	{
        gDevNames = EfiLibAllocatePool(u16BootCount * (sizeof(*gDevNames)));
        for (i=0; i<u16BootCount; i++) 
        {
            gDevNames[i].String = BootGetBootNowName(i,gPopupMenuShowAllBbsDev,BootNowInBootOrderSupport());
            gDevNames[i].Index = i;
        }
	}

	gNumOfDev = u16BootCount;

	return Status;
}
/**
    function for the BBS Popup menu

    @param void

    @retval void

**/
VOID DrawBootOnlyBbsPopupMenu( VOID )
{
	int		Row = 0;
	INTN	Key;
	int 	DispLines=0;
	int		EnoughOfThis = 0;

	if(gST->ConOut == NULL)
		return;

	SetDesiredTextMode();
	// Clean of the Graphics Screen
	ClearGrphxScreen();
	ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );

	//Initialise the global buffers
	InitializeScreenBuffer( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );

	//check the lengh of device strings, and truncate if it is out of boundaries
	CheckStrings();

	//Create heading
	DrawStringWithAttribute( START_COL_IDX+(MAX_STR_LEN/2)-4, START_ROW_IDX-2, L"BBS MENU", EFI_WHITE);
	DrawBorder( START_COL_IDX-3, START_ROW_IDX-3, MAX_LENGTH, 3);

	if( gNumOfDev < MAX_DISP_LINES )
	{
		DispLines = gNumOfDev;
	}
	else {
		DispLines = MAX_DISP_LINES;
	}

	if (gDevNames) 
	{
        //print all the avilable device info
        for( Row=START_ROW_IDX; Row < START_ROW_IDX+DispLines; Row++ ) {
            DrawStringWithAttribute( START_COL_IDX, Row, gDevNames[Row-START_ROW_IDX].String, EFI_LIGHTGRAY);
        }
	}
	DrawBorder( START_COL_IDX-3, START_ROW_IDX-1, MAX_LENGTH, DispLines+2);

	//keep the cursor in the staring line of strings display
	gST->ConOut->SetCursorPosition( gST->ConOut, START_COL_IDX, START_ROW_IDX );

	if (gDevNames)
	{
        //highlight first option in the list by default
        DrawStringWithAttribute( START_COL_IDX, START_ROW_IDX, gDevNames[0].String, EFI_WHITE);
	}
	DrawBorder( START_COL_IDX-3, START_ROW_IDX-1, MAX_LENGTH, DispLines+2);

	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX+MAX_LENGTH-4),BOXDRAW_VERTICAL_LEFT, EFI_WHITE );
	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX-3),BOXDRAW_VERTICAL_RIGHT, EFI_WHITE );

	SetScrollPosition(DispLines, 2);
	FlushLines( 0, gMaxRows - 1 );
	DoRealFlushLines();

	while(TRUE)
	{
		//Get input key
		Key = BBS_GetKey();

		if(_ESC_KEY == Key) {
			PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);
			gBBSPopupSel = 0 ;
			EnoughOfThis = 1 ;
			ProcessProceedToBootHook();
			break;
		}

		switch(Key)
		{
			case _DOWN_ARROW:
				if(gStrIdx<(gNumOfDev-1)) {
					gStrIdx++;
					if( gHiStr >= (DispLines-1) ) {
						DownDisplayList(gStrIdx-(DispLines-1), gStrIdx, DispLines);
					}
					else {
						HighLightLine(0, DispLines);
					}
				}
			break;

			case _UP_ARROW:

				if(gStrIdx > 0) {
					gStrIdx--;
					if( gHiStr == 0 ) {
						if(gStrIdx>=0)
							UpDisplayList(gStrIdx, gStrIdx+(DispLines-1), 0, DispLines);
					}
					else if( gHiStr > 0 ) {
						HighLightLine(1, DispLines);
					}
				}
			break;

			case _ENTER_KEY:
				if (gDevNames) {
					if( ( gStrIdx >= 0) && ( gStrIdx <= gNumOfDev) ) {
						//call the function with the selected device index
						PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);
						gBBSPopupSel = (UINT16) gStrIdx ;
						EnoughOfThis = 1 ;
					}
				}
				break;
		}
		DoRealFlushLines();
		if(EnoughOfThis)
			break ;
	}
}
/**
    This function is a hook called after launching
    BBS Popup Menu. This function is available as
    ELINK. In the generic implementation this function is
    empty. OEMs may choose to do additional logic here.

        index: The nth option in the boot now menu.
    @param ShowAllBbsDev SETUP_SHOW_ALL_BBS_DEVICES

    @retval VOID

**/

VOID BbsBoot(VOID)
{
	UINT8 index;
	
	//boot to the disabled boot option even if all the options are disabled when TSE_BOOT_TO_DISABLED_BOOT_OPTIONS enabled  
	if(!IsBootToDisabledBootOption())
	{
		for (index=0; index < gBootOptionCount; index++)
		{
			if (gBootData[index].Active)
			break;
		}
		if (index == gBootOptionCount)
			gEnterSetup = TRUE;
	}
	if(gEnterSetup != TRUE)
		BootLaunchBootNow(gBBSPopupSel, gPopupMenuShowAllBbsDev) ;
}

/**
    function the string copy

    @param Dst CHAR16  *Src, UINTN   Length

    @retval void

**/
VOID BbsStrnCpy ( CHAR16  *Dst, CHAR16  *Src, UINTN   Length )
{
	UINTN Index;
	UINTN SrcLen;

	SrcLen = EfiStrLen (Src);

	Index = 0;
	while (Index < Length && Index < SrcLen) {
		Dst[Index] = Src[Index];
		Index++;
	}

	for (Index = SrcLen; Index < Length; Index++) {
		Dst[Index] = 0;
	}
}

/**
    function to read the input key

    @param void

    @retval ScanCode/UniCode

**/
INTN BBS_GetKey(VOID)
{
    EFI_INPUT_KEY	    Key;
    UINTN Index;

    // flush keyboard buffer
    while ( gST->ConIn->ReadKeyStroke(gST->ConIn,&Key) == EFI_SUCCESS );

    gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &Index );
    gST->ConIn->ReadKeyStroke(gST->ConIn,&Key);
    return (Key.ScanCode<<16)|Key.UnicodeChar;
}

/**
    function to validate the stings

    @param void

    @retval void

**/
void CheckStrings(void)
{
	int idx;
	UINTN Length=0;
	CHAR16  TmpStr[50];

	for( idx=0; idx < gNumOfDev; idx++ ) {


		Length = EfiStrLen( gDevNames[idx].String );

		if(Length > MAX_STR_LEN)
		{
			BbsStrnCpy(TmpStr,L" ",MAX_STR_LEN);
			BbsStrnCpy(TmpStr, gDevNames[idx].String, (MAX_STR_LEN-3));
			EfiStrCpy (TmpStr + EfiStrLen(TmpStr),L"...");

			BbsStrnCpy(gDevNames[idx].String,L" ",Length);
			BbsStrnCpy(gDevNames[idx].String, TmpStr, MAX_STR_LEN);
		}
	}
}

/**
    function to set the position of the Scroll bar

    @param int Index, int DirFlg

    @retval void

**/
void SetScrollPosition(int Index, int DirFlg)
{
	if(gNumOfDev > MAX_DISP_LINES)
	{
		if(DirFlg==2) {
			WriteCharWithAttribute( (START_ROW_IDX+Index-1), (START_COL_IDX+MAX_LENGTH-5),GEOMETRICSHAPE_DOWN_TRIANGLE, EFI_WHITE );
		}
		else if( (DirFlg==0) && (gStrIdx>Index-1) ) {
			if(gStrIdx!= gNumOfDev-1)
				WriteCharWithAttribute( (START_ROW_IDX+Index-1), (START_COL_IDX+MAX_LENGTH-5),GEOMETRICSHAPE_DOWN_TRIANGLE, EFI_WHITE );
			WriteCharWithAttribute( START_ROW_IDX, (START_COL_IDX+MAX_LENGTH-5),GEOMETRICSHAPE_UP_TRIANGLE, EFI_WHITE );
		}
		else if( (DirFlg==1) && (gStrIdx<gNumOfDev-Index) ) {
			WriteCharWithAttribute( (START_ROW_IDX+Index-1), (START_COL_IDX+MAX_LENGTH-5),GEOMETRICSHAPE_DOWN_TRIANGLE, EFI_WHITE );
			if(gStrIdx!= 0)
				WriteCharWithAttribute( START_ROW_IDX, (START_COL_IDX+MAX_LENGTH-5),GEOMETRICSHAPE_UP_TRIANGLE, EFI_WHITE );
		}
		else if(DirFlg==0) {
			WriteCharWithAttribute( (START_ROW_IDX+Index-1), (START_COL_IDX+MAX_LENGTH-5),GEOMETRICSHAPE_DOWN_TRIANGLE, EFI_WHITE );
		}
		else if(DirFlg==1) {
			WriteCharWithAttribute( START_ROW_IDX, (START_COL_IDX+MAX_LENGTH-5),GEOMETRICSHAPE_UP_TRIANGLE, EFI_WHITE );
		}
	}
}

/**
    function to display the upside list

    @param int Start, int End, int flag, int NumLines

    @retval void

**/
void UpDisplayList(int Start, int End, int flag, int NumLines)
{
	int RowNum; 

    if (gDevNames) 
    {
        for( RowNum=START_ROW_IDX; Start<=End; RowNum++ ) {
            DrawStringWithAttribute( START_COL_IDX, RowNum, gBlankSpace, 0);
            DrawStringWithAttribute( START_COL_IDX, RowNum, gDevNames[Start++].String, EFI_LIGHTGRAY);
        }
        DrawStringWithAttribute( START_COL_IDX, (RowNum-NumLines), gBlankSpace, 0);
        DrawStringWithAttribute( START_COL_IDX, (RowNum-NumLines), gDevNames[Start-NumLines].String, EFI_WHITE);
        DrawBorder( START_COL_IDX-3, START_ROW_IDX-1, MAX_LENGTH, (NumLines+2));
    }

	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX+MAX_LENGTH-4),BOXDRAW_VERTICAL_LEFT, EFI_WHITE );
	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX-3),BOXDRAW_VERTICAL_RIGHT, EFI_WHITE );

	SetScrollPosition(NumLines, 1);

	FlushLines( 0, gMaxRows - 1 );

	if(gHiStr) gHiStr--;
}

/**
    function to display the downside list

    @param int Start, int End, int NumLines

    @retval void

**/
void DownDisplayList(int Start, int End, int NumLines)
{
	int RowNum;

	if (gDevNames) 
	{
        for( RowNum=START_ROW_IDX; Start<End; RowNum++ ) {
            DrawStringWithAttribute( START_COL_IDX, RowNum, gBlankSpace, 0);
            DrawStringWithAttribute( START_COL_IDX, RowNum, gDevNames[Start++].String, EFI_LIGHTGRAY);
        }
        DrawStringWithAttribute( START_COL_IDX, RowNum, gBlankSpace, 0);
        DrawStringWithAttribute( START_COL_IDX, RowNum, gDevNames[End].String, EFI_WHITE);
        DrawBorder( START_COL_IDX-3, START_ROW_IDX-1, MAX_LENGTH, (NumLines+2));
	}

	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX+MAX_LENGTH-4),BOXDRAW_VERTICAL_LEFT, EFI_WHITE );
	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX-3),BOXDRAW_VERTICAL_RIGHT, EFI_WHITE );

	SetScrollPosition(NumLines, 0);
	FlushLines( 0, gMaxRows - 1 );

	if(gHiStr<(NumLines-1)) gHiStr++;
}

/**
    function to hilight a perticular line

    @param int UpDown, int NumLines

    @retval void

**/
void HighLightLine(int UpDown, int NumLines)
{

    if (gDevNames) 
    {
        if(UpDown) {
            DrawStringWithAttribute( START_COL_IDX, (START_ROW_IDX+gHiStr-1), gDevNames[gStrIdx].String, EFI_WHITE);
            DrawStringWithAttribute( START_COL_IDX, (START_ROW_IDX+gHiStr), gDevNames[gStrIdx+1].String, EFI_LIGHTGRAY);
            DrawBorder( START_COL_IDX-3, START_ROW_IDX-1, MAX_LENGTH, (NumLines+2));
        }
        else {
            DrawStringWithAttribute( START_COL_IDX, (START_ROW_IDX+gHiStr+1), gDevNames[gStrIdx].String, EFI_WHITE);
            DrawStringWithAttribute( START_COL_IDX, (START_ROW_IDX+gHiStr), gDevNames[gStrIdx-1].String, EFI_LIGHTGRAY);
            DrawBorder( START_COL_IDX-3, START_ROW_IDX-1, MAX_LENGTH, (NumLines+2));
        }
    }
	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX+MAX_LENGTH-4),BOXDRAW_VERTICAL_LEFT, EFI_WHITE );
	WriteCharWithAttribute( START_ROW_IDX-1, (START_COL_IDX-3),BOXDRAW_VERTICAL_RIGHT, EFI_WHITE );

	SetScrollPosition(NumLines, UpDown);
	FlushLines( 0, gMaxRows - 1 );

	if(UpDown) {
		if( gHiStr>0 ) gHiStr--;
	}
	else{
		if( gHiStr<(NumLines-1) ) gHiStr++;
	}
}

/**

    @param No Arguments

    @retval void

**/
VOID InitgProgress( VOID )
{
	gProgress = EfiLibAllocateZeroPool(sizeof (PROGRESSBAR_INFO) );
	gProgress->quiteBootActive = FALSE ;
	gProgress->active = FALSE ;
	gProgress->completed = 0 ;
	gProgress->delta = 0 ;
}


/**
    Function to update version string

    @param VOID

    @retval VOID

**/
VOID BootOnlyUpdateVersionString( VOID )
{
	CHAR16 *string = NULL, *newString = NULL;
	UINTN length,TseMajor,TseMinor, TseBuild;
	UINT16 TseBuildYear=0;
	UINTN i;

	for ( i = 0; i < gLangCount; i++ )
	{
		string = HiiGetStringLanguage( gHiiHandle, STRING_TOKEN(STR_MAIN_COPYRIGHT), gLanguages[i].Unicode );
		if ( string == NULL )
			return;

		length = EfiStrLen(string) + 10;
		newString = EfiLibAllocateZeroPool( length * sizeof(CHAR16) );
		if ( newString != NULL )
		{			
			GetTseBuildVersion(&TseMajor,&TseMinor,&TseBuild);
			GetTseBuildYear(&TseBuildYear);
			SPrint( newString, length * sizeof(CHAR16), string, TseMajor, TseMinor, TseBuild,TseBuildYear );
			HiiChangeStringLanguage( gHiiHandle, STRING_TOKEN(STR_MAIN_COPYRIGHT), gLanguages[i].Unicode, newString );
		}

		MemFreePointer( (VOID **)&newString );
		MemFreePointer( (VOID **)&string );
	}
}

/**
    Function to save the post screen

    @param SizeOfX UINTN *SizeOfY 

    @retval VOID*

**/
VOID *SavePostScreen( UINTN *SizeOfX, UINTN *SizeOfY )
{
	EFI_UGA_PIXEL *UgaBlt = NULL;
	UINTN UgaBltSize;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	UpdateGoPUgaDraw();

	MouseStop();
	if ( gQuietBoot )
	{
	     if(EFI_SUCCESS == GetScreenResolution(SizeOfX,SizeOfY))
	     {
	         UgaBltSize = (*SizeOfX) * (*SizeOfY) * sizeof(EFI_UGA_PIXEL);
	         UgaBlt = EfiLibAllocatePool(UgaBltSize);
	
	         if(UgaBlt)
	         {
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
	             Status = gGOP->Blt(
	                     gGOP,
	                     UgaBlt,
	                     EfiBltVideoToBltBuffer,
	                     0, 0,
	                     0, 0,
	                     *SizeOfX, *SizeOfY,
	                     0);
#else
	             Status = gUgaDraw->Blt(
	                     gUgaDraw,
	                     UgaBlt,
	                     EfiBltVideoToBltBuffer,           
	                     0, 0,
	                     0, 0,
	                     *SizeOfX, *SizeOfY,
	                     0);
#endif
	         }
	     }
	}	

	MouseRefresh();
	return (VOID *)UgaBlt;
}

/**
    Function to restore the saved screen

    @param UgaBlt UINTN SizeOfX, UINTN SizeOfY 

    @retval VOID

**/
VOID RestorePostScreen( VOID *UgaBlt, UINTN SizeOfX, UINTN SizeOfY )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	EFI_UGA_PIXEL *Buffer = (EFI_UGA_PIXEL *)UgaBlt;

	MouseStop();	
	if ( Buffer )
    {
		if(gConsoleControl != NULL)
            gConsoleControl->SetMode( gConsoleControl, EfiConsoleControlScreenGraphics );
			
		if( (NULL != gGOP) && (SizeOfX && SizeOfY) && //If it has vaild gGOP and Resolution
			( (SizeOfX != gGOP->Mode->Info->HorizontalResolution) || (SizeOfY != gGOP->Mode->Info->VerticalResolution) ) //If QuietBoot resolution is differ
		)
		{
			GOPSetScreenResolution(&SizeOfX, &SizeOfY);
		}
		if(IsMouseSupported()&& IsSoftKbdSupported())//Initializing the mouse at post when mouse and softkbd present
			MouseInit();
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
        Status = gGOP->Blt (
                gGOP,
                Buffer,
                EfiBltBufferToVideo,
                0, 0,
                0, 0,
                SizeOfX, SizeOfY,
                0);
#else
        Status = gUgaDraw->Blt (
                gUgaDraw,
                Buffer,
                EfiUgaBltBufferToVideo,           
                0, 0,
                0, 0,
                SizeOfX, SizeOfY,
                0);
#endif
    }
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL	
	else if(gQuietBoot && gGOP && (gST->ConOut != NULL))
#else
	else if ( gQuietBoot && UgaDraw && (gST->ConOut != NULL))
#endif
	{
		DrawQuietBootLogoHook();
	}

	MouseRefresh();
	// If ConOut is available in System Table the activate the post screen.
	if ( gST->ConOut != NULL ) 
    {
    	//Show post screen if it is not quiet boot or if it is a headless system
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
        if ( ! gQuietBoot || !gGOP)
#else
    	if ( ! gQuietBoot || !gUgaDraw)
#endif
    	{
    		//Clear the flush buffer to avoid messages missing
    		if ( NULL != gFlushBuffer)
				MemSet(gFlushBuffer, sizeof(SCREEN_BUFFER), 0);
    
    		CleanUpLogo();
    		InitPostScreen();
    	}
    }    
	MemFreePointer((VOID **) &Buffer);

}

/**
    Function to restore the Password screen

    @param VOID

    @retval VOID

**/
VOID RestorePasswordScreen()
{
	UINTN CurrXPos,CurrYPos,Attribute;
	BOOLEAN PreviousCursorState=FALSE;
	SETUP_DEBUG_TSE ("\n[TSE] RestorePasswordScreen() Entering\n");
	if(gST && gST->ConOut)
	{
		//Taking the Backup of Current Attribute,CursorColumn,CursorRow,CursorVisible
		Attribute = gST->ConOut->Mode->Attribute;
		CurrXPos = gST->ConOut->Mode->CursorColumn;
		CurrYPos = gST->ConOut->Mode->CursorRow;
		PreviousCursorState = gST->ConOut->Mode->CursorVisible;
	}
	FlushLines( 0, gMaxRows - 1 );
	DoRealFlushLines();
	if (IsTSECursorSupport() && (gST && gST->ConOut)) 
	{
		//Restoring the Backup of  Attribute,CursorColumn,CursorRow,CursorVisible
		gST->ConOut->SetAttribute( gST->ConOut, Attribute );
		gST->ConOut->SetCursorPosition( gST->ConOut, CurrXPos, CurrYPos);	// Cursor support for msg boxes
		gST->ConOut->EnableCursor(gST->ConOut, PreviousCursorState);// Enable Cursor support for msg boxes
	}	
	SETUP_DEBUG_TSE ("\n[TSE] RestorePasswordScreen() Exiting\n");
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
