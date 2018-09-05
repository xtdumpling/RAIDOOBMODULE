//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  This file contains the Graphics console driver that produces the Simple Text Out interface

*/

//-----------------------------------------------------------------------------
#include "GraphicsConsole.h"

//-----------------------------------------------------------------------------
// Function Prototypes

EFI_STATUS	DriverBindingSupported ( 
	IN EFI_DRIVER_BINDING_PROTOCOL    *This,
	IN EFI_HANDLE                     ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath);

EFI_STATUS	DriverBindingStart ( 
	IN EFI_DRIVER_BINDING_PROTOCOL    *This,
	IN EFI_HANDLE                     ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath);

EFI_STATUS	DriverBindingStop ( 
	IN EFI_DRIVER_BINDING_PROTOCOL	*This,
	IN EFI_HANDLE			        ControllerHandle,
	IN  UINTN			            NumberOfChildren,
	IN  EFI_HANDLE			        *ChildHandleBuffer);

//******************** Simple Text Output protocol functions prototypes ***********

EFI_STATUS GCReset(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN BOOLEAN ExtendedVerification);

EFI_STATUS GCOutputString(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN CHAR16 *String);

EFI_STATUS GCTestString(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN CHAR16 *String);

EFI_STATUS GCQueryMode(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN UINTN ModeNum, 
    OUT UINTN *Col, 
    OUT UINTN *Row);

EFI_STATUS GCSetMode(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN UINTN ModeNum);

EFI_STATUS GCSetAttribute(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN UINTN Attribute);

EFI_STATUS GCClearScreen(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);

EFI_STATUS GCSetCursorPosition(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN UINTN Column, 
    IN UINTN Row);

EFI_STATUS GCEnableCursor(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    IN BOOLEAN Visible);

//******************** Service functions prototypes ********************************

EFI_STATUS GetColorFromAttribute(
    IN UINT32 Attribute, 
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Foreground,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Background);

EFI_STATUS GetGraphicsModeNumber (
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL	*GraphicsOutput, 
    IN UINT32                       HorRes,
    IN UINT32                       VerRes,
	OUT UINT32                      *ModeNum,
    IN  BOOLEAN                     ExactMatch,
    OUT UINT32                      *ActualHorRes OPTIONAL,
    OUT UINT32                      *ActualVerRes OPTIONAL );

EFI_STATUS SetupGraphicsDevice(
    IN GC_DATA *GcData);

VOID EFIAPI BlinkCursorEvent ( IN EFI_EVENT Event, IN VOID *Context );

VOID DrawCursor(
    IN GC_DATA *GcData,
    IN BOOLEAN Visible);

VOID ScrollUp(
    IN GC_DATA *GcData);

VOID SaveCursorImage(
    IN GC_DATA *GcData);

VOID ShiftCursor(
    IN GC_DATA *GcData,
    IN UINT16 Step);

VOID AddChar(
    IN GC_DATA *GcData,
    IN CHAR16 Char,
    IN UINT16 Width);

VOID FlushString(
    IN GC_DATA *GcData);

//********************** Hooks prototypes ******************************************

VOID GcUpdateBltBuffer (
	IN     GC_DATA 			             *This,			    //pointer to internal structure
	IN     UINT32			             Width,	            //width of the buffer in pixels
    IN     UINT32                        Height,            //height of the buffer in pixels
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer	        //pointer to BLT buffer to update
	);

VOID GcInternalClearScreen (
	IN OUT GC_DATA *This
	);

//-----------------------------------------------------------------------------
// Globals
/// Array that lists the current modes defined by the BIOS
const TEXT_MODE TextModeArray[] = {GC_MODE_LIST};
/// Value that defines the total number of text modes defined in the TextModeArray
const INT32 MaxTextMode=(sizeof(TextModeArray)/sizeof(TEXT_MODE));
/// Array of the colors that are used in the system
const EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorArray[] = {			
	{ GC_COLOR_BLACK       ,   0}, // case EFI_BLACK: 		// case EFI_BACKGROUND_BLACK >> 4
	{ GC_COLOR_BLUE        ,   0}, // case EFI_BLUE : 		// case EFI_BACKGROUND_BLUE >> 4
	{ GC_COLOR_GREEN       ,   0}, // case EFI_GREEN : 	    // case EFI_BACKGROUND_GREEN >> 4
	{ GC_COLOR_CYAN        ,   0}, // case EFI_CYAN : 		// case EFI_BACKGROUND_CYAN >> 4
	{ GC_COLOR_RED         ,	0}, // case EFI_RED : 		// case EFI_BACKGROUND_RED >> 4
	{ GC_COLOR_MAGENTA     ,	0}, // case EFI_MAGENTA : 	// case EFI_BACKGROUND_MAGENTA >> 4
	{ GC_COLOR_BROWN       ,	0}, // case EFI_BROWN : 	// case EFI_BACKGROUND_BROWN >> 4
	{ GC_COLOR_LIGHTGRAY   ,	0}, // case EFI_LIGHTGRAY : // case EFI_BACKGROUND_LIGHTGRAY >> 4
	{ GC_COLOR_DARKGRAY    ,	0}, // case EFI_DARKGRAY : 
	{ GC_COLOR_LIGHTBLUE   ,	0}, // case EFI_LIGHTBLUE : 
	{ GC_COLOR_LIGHTGREEN  ,	0}, // case EFI_LIGHTGREEN : 
	{ GC_COLOR_LIGHTCYAN   ,	0}, // case EFI_LIGHTCYAN : 
	{ GC_COLOR_LIGHTRED    ,	0}, // case EFI_LIGHTRED : 
	{ GC_COLOR_LIGHTMAGENTA,	0}, // case EFI_LIGHTMAGENTA : 
	{ GC_COLOR_YELLOW      ,	0}, // case EFI_YELLOW : 
	{ GC_COLOR_WHITE       ,	0}  // case EFI_WHITE : 
};

/// Pointer to the buffer that holds currently processed string
static CHAR16 *TextBuffer = NULL;
/// Size of text buffer
static UINT32 TextBufferSize = 0;
/// Current position within text buffer
static UINT32 Position = 0;
/// Width in pixels of currently processed string
static UINT32 StringWidth = 0;

/// Default instance of AMI_TEXT_OUT_PROTOCOL
extern AMI_TEXT_OUT_PROTOCOL AmiTextOutProtocol;

//-----------------------------------------------------------------------------
// Protocol implementation

/// Protocol instance of the simple Text Output Protocol
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	mGCProtocol	=
	{
	GCReset,
	GCOutputString,
	GCTestString,
	GCQueryMode,
	GCSetMode,
	GCSetAttribute,
	GCClearScreen,
	GCSetCursorPosition,
	GCEnableCursor,
	NULL
	};


//-----------------------------------------------------------------------------
// Driver Binding Protocol
/// Driver Binding protocol for the Graphics Console Driver
EFI_DRIVER_BINDING_PROTOCOL gGraphicsConsoleDriverBindingProtocol = {
	DriverBindingSupported,
	DriverBindingStart,
	DriverBindingStop,
	0x10,
	NULL,
	NULL
	};


//-----------------------------------------------------------------------------
// Function Definitions
#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID //old Core
#ifndef LANGUAGE_CODE_ENGLISH
/// language code used to define english strings
#define LANGUAGE_CODE_ENGLISH "eng"
#endif
/// function to verify if two language codes are equal
static BOOLEAN LanguageCodesEqual(
    CONST CHAR8* LangCode1, CONST CHAR8* LangCode2
){
    return    LangCode1[0]==LangCode2[0] 
           && LangCode1[1]==LangCode2[1]
           && LangCode1[2]==LangCode2[2];
}
static EFI_GUID gEfiComponentName2ProtocolGuid = EFI_COMPONENT_NAME_PROTOCOL_GUID;
#endif
/// Driver Name used to identify the driver
static UINT16 *gDriverName=L"AMI Graphic Console Driver";

/**
	This function is supposed to return the Controller name of the Handle that is passed in

	@param[in] This - pointer to protocol instance
	@param[in] Controller - controller handle
	@param[in] ChildHandle - child handle
	@param[in] Language - pointer to language description
	@param[out] ControllerName - pointer to store pointer to controller name

	@retval EFI_SUCCESS - controller name returned
	@retval EFI_INVALID_PARAMETER - language undefined
	@retval EFI_UNSUPPORTED - given language not supported
*/
static EFI_STATUS GraphicsConsoleGetControllerName (
		IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
		IN  EFI_HANDLE                   ControllerHandle,
 		IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
  		IN  CHAR8                        *Language,
  		OUT CHAR16                       **ControllerName
)
{
	return EFI_UNSUPPORTED;
}


/**
	This function is supposed to return the Driver name of this Driver

	@param[in] This - pointer to EFI_COMPONENT_NAME2_PROTOCOL protocol instance
	@param[in] Language - pointer to language description
	@param[out] DriverName - pointer that will point to the Driver Name if return
						value is EFI_SUCCESS

	@retval EFI_SUCCESS - driver name returned
	@retval EFI_INVALID_PARAMETER - language undefined
	@retval EFI_UNSUPPORTED - given language not supported
*/
static EFI_STATUS GraphicsConsoleGetDriverName(
    IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
	IN  CHAR8                        *Language,
	OUT CHAR16                       **DriverName
)
{
	//Supports only English
	if(!Language || !DriverName) 
        return EFI_INVALID_PARAMETER;

	if (!LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH)) 
        return EFI_UNSUPPORTED;
	else 
        *DriverName=gDriverName;
	
	return EFI_SUCCESS;
}

//Component Name Protocol
static EFI_COMPONENT_NAME2_PROTOCOL gGraphicsConsole = {
  GraphicsConsoleGetDriverName,
  GraphicsConsoleGetControllerName,
  LANGUAGE_CODE_ENGLISH
};

/**
  HII Database Protocol notification event handler.

  This function registers the default font package when the HII Database Protocol is installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
*/
VOID EFIAPI RegisterFontPackage (IN EFI_EVENT Event, IN VOID *Context){
/// Font File Guid
#define FONT_FFS_FILE_GUID { 0xdac2b117, 0xb5fb, 0x4964, { 0xa3, 0x12, 0xd, 0xcc, 0x77, 0x6, 0x1b, 0x9b } }
/// Font File Section Guid
#define FONT_FFS_FILE_SECTION_GUID { 0xa7fe7491, 0x2c54, 0x4f72, { 0xb8, 0xe, 0x76, 0x49, 0xa8, 0x21, 0x1, 0x93 } }

static EFI_GUID FontFfsFileGuid = FONT_FFS_FILE_GUID;
static EFI_GUID FontFfsFileSectionGuid = FONT_FFS_FILE_SECTION_GUID;
extern UINT8 UsStdNarrowGlyphData[];
	EFI_FIRMWARE_VOLUME2_PROTOCOL *pFV;
	UINTN DataSize;
	EFI_GUID *pSectionGuid = NULL;
	UINT32 Authentication;
	EFI_HANDLE *pHandle;
	UINTN Number,i;
    EFI_STATUS FontStatus;
    EFI_HII_PACKAGE_HEADER *FontPackage;
    EFI_HII_HANDLE HiiFontHandle;

    FontPackage = (EFI_HII_PACKAGE_HEADER*)&UsStdNarrowGlyphData;
	FontStatus = pBS->LocateHandleBuffer(ByProtocol,&gEfiFirmwareVolume2ProtocolGuid, NULL, &Number, &pHandle);
	if (EFI_ERROR(FontStatus)) return;
	for(i=0;i<Number; i++)
	{
		FontStatus=pBS->HandleProtocol(pHandle[i], &gEfiFirmwareVolume2ProtocolGuid, &pFV);
		if (EFI_ERROR(FontStatus)) continue;
		DataSize=0;
		//NOTE: Only one section per FFS file is supported
		FontStatus=pFV->ReadSection (
			pFV,&FontFfsFileGuid,
			EFI_SECTION_FREEFORM_SUBTYPE_GUID,0, &pSectionGuid, &DataSize,
			&Authentication
	 	);
		if (!EFI_ERROR(FontStatus))
		{
			if (guidcmp(pSectionGuid,&FontFfsFileSectionGuid)){
				pBS->FreePool(pSectionGuid);
				pSectionGuid = NULL;
				continue;
			}
			FontPackage=(EFI_HII_PACKAGE_HEADER*)(pSectionGuid+1);
			break;
		} else pSectionGuid = NULL;
	}
	FontStatus = HiiLibPublishPackages(&FontPackage, 1, &FontFfsFileGuid, NULL, &HiiFontHandle);
	if (pSectionGuid!=NULL) pBS->FreePool(pSectionGuid);
	pBS->FreePool(pHandle);
	pBS->CloseEvent(Event);
}


/**  
	Entry point for the Graphics Console Module.  

	This function performs two actions:
	1) Installs a callback to register a font package
	2) Installs the DriverBindingProtocol for the Graphics Console

	@param[in] ImageHandle - driver image handle
	@param[in] SystemTable - pointer to system table
	
	@retval EFI_SUCCESS - Driver binding protocol was installed
*/

EFI_STATUS	EFIAPI GCEntryPoint (
	IN EFI_HANDLE           ImageHandle,
	IN EFI_SYSTEM_TABLE     *SystemTable
)
{
	EFI_STATUS	Status;
    EFI_EVENT   DatabaseProtocolEvent;

	InitAmiLib(ImageHandle, SystemTable);
    
    //
    // Register notify function on HII Database Protocol to add default font.
    //
    EfiCreateProtocolNotifyEvent (
        &gEfiHiiDatabaseProtocolGuid,
        TPL_CALLBACK,
        RegisterFontPackage,
        NULL,
        &DatabaseProtocolEvent
    );

	// initiaize the ImageHandle and DriverBindingHandle
	gGraphicsConsoleDriverBindingProtocol.DriverBindingHandle = NULL;
	gGraphicsConsoleDriverBindingProtocol.ImageHandle = ImageHandle;

	// Install driver binding protocol here
	Status = pBS->InstallMultipleProtocolInterfaces (
						&gGraphicsConsoleDriverBindingProtocol.DriverBindingHandle,
						&gEfiDriverBindingProtocolGuid, &gGraphicsConsoleDriverBindingProtocol,
						&gEfiComponentName2ProtocolGuid, &gGraphicsConsole,
						NULL);

	return Status;
}

/**
  Checks to see if this driver can be used

	@param[in] This - pointer to protocol instance
	@param[in] ControllerHandle - handle of controller to install driver on
	@param[in] RemainingDevicePath - pointer to device path

	@retval EFI_SUCCESS - Driver supports the Device
	@retval EFI_NOT_SUPPORTED - Driver cannot support the Device 
*/
EFI_STATUS	DriverBindingSupported (
	IN EFI_DRIVER_BINDING_PROTOCOL    *This,
	IN EFI_HANDLE                     ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
	EFI_STATUS                   Status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput = NULL;
	EFI_HII_FONT_PROTOCOL		 *HiiFont;

	Status = pBS->OpenProtocol(
                        ControllerHandle, 
                        &gEfiGraphicsOutputProtocolGuid,
    	                &GraphicsOutput, 
                        This->DriverBindingHandle,
		                ControllerHandle, 
                        EFI_OPEN_PROTOCOL_BY_DRIVER);
	if (EFI_ERROR(Status))
        return Status;
    else
		pBS->CloseProtocol( 
                        ControllerHandle, 
                        &gEfiGraphicsOutputProtocolGuid,
					    This->DriverBindingHandle, 
                        ControllerHandle);

	Status = pBS->OpenProtocol(
                        ControllerHandle, 
                        &gEfiDevicePathProtocolGuid,
    	                NULL, 
                        This->DriverBindingHandle,
		                ControllerHandle, 
                        EFI_OPEN_PROTOCOL_TEST_PROTOCOL);
	if (EFI_ERROR(Status)) 
        return Status;

	Status = pBS->LocateProtocol ( &gEfiHiiFontProtocolGuid, NULL, &HiiFont);

	return Status;
}

/**
	This function grabs needed protocols and initializes the supported text modes

	@param[in] This - pointer to protocol instance
	@param[in] ControllerHandle - handle of controller to install driver on
	@param[in] RemainingDevicePath - pointer to device path

	@retval EFI_SUCCESS - SimpleTextOut Protocol installed
	@retval EFI_NOT_SUPPORTED - SimpleTextOut Protocol not installed
*/  
EFI_STATUS	DriverBindingStart (
	IN EFI_DRIVER_BINDING_PROTOCOL *This,
	IN EFI_HANDLE                  ControllerHandle,
	IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
)
{
	EFI_STATUS			Status;
    UINT32              StartMode;

#if (CURSOR_BLINK_ENABLE != 0)
	EFI_STATUS			EventStatus;
#endif
	GC_DATA				*GcData = NULL;

	// Create private data structure and fill with proper data
	Status = pBS->AllocatePool(EfiBootServicesData, sizeof(GC_DATA), &GcData);
	if (EFI_ERROR(Status))
        return Status;

	pBS->CopyMem( &(GcData->SimpleTextOut), 
                  &mGCProtocol, 
				  sizeof(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL) );

	Status = pBS->AllocatePool(EfiBootServicesData, 
                               sizeof(SIMPLE_TEXT_OUTPUT_MODE), 
                               &(GcData->SimpleTextOut.Mode));
	// Initialize mode with the invalid value.
	// The correct value will be assigned by GcSetMode (called by GcReset)
	GcData->SimpleTextOut.Mode->Mode = -1;
    if(EFI_ERROR(Status))
    {
        pBS->FreePool(GcData);
        return Status;
    }

	Status = pBS->AllocatePool(EfiBootServicesData, 
                               sizeof(GC_TEXT_MODE) * MaxTextMode, 
                               &(GcData->SupportedModes));
    if(EFI_ERROR(Status))
    {
        pBS->FreePool(GcData->SimpleTextOut.Mode);
        pBS->FreePool(GcData);
        return Status;
    }

	Status = pBS->OpenProtocol( 
                            ControllerHandle, 
                            &gEfiGraphicsOutputProtocolGuid,
					        &(GcData->GraphicsOutput), 
                            This->DriverBindingHandle,
					        ControllerHandle, 
                            EFI_OPEN_PROTOCOL_BY_DRIVER );
	if (EFI_ERROR(Status))
    {
        pBS->FreePool(GcData->SupportedModes);
        pBS->FreePool(GcData->SimpleTextOut.Mode);
        pBS->FreePool(GcData);
		return Status;
    }

	// Find the Hii Protocol and attach it to the datastructure
	Status = pBS->LocateProtocol ( &gEfiHiiFontProtocolGuid, NULL, &(GcData->HiiFont));

	if (EFI_ERROR(Status))
	{
		pBS->CloseProtocol( 
                        ControllerHandle, 
                        &gEfiGraphicsOutputProtocolGuid,
					    This->DriverBindingHandle, 
                        ControllerHandle);
        pBS->FreePool(GcData->SupportedModes);
        pBS->FreePool(GcData->SimpleTextOut.Mode);
        pBS->FreePool(GcData);
		return EFI_UNSUPPORTED;
	}

    Status = SetupGraphicsDevice(GcData);
    if(EFI_ERROR(Status))
	{
		pBS->CloseProtocol( 
                        ControllerHandle, 
                        &gEfiGraphicsOutputProtocolGuid,
					    This->DriverBindingHandle, 
                        ControllerHandle);
        pBS->FreePool(GcData->SupportedModes);
        pBS->FreePool(GcData->SimpleTextOut.Mode);
        pBS->FreePool(GcData);
		return EFI_UNSUPPORTED;
	}	

    //initialize porting hooks and signature
    GcData->Signature = 0x54445348;
    GcData->Version = 1;
    GcData->OemUpdateBltBuffer = GcUpdateBltBuffer;
    GcData->OemClearScreen = GcInternalClearScreen;
    GcData->OemScrollUp = NULL;
    GcData->DeltaX = 0;
    GcData->DeltaY = 0;
    GcData->MaxColumns = 0;
    GcData->MaxRows = 0;
    GcData->AmiTextOut = AmiTextOutProtocol;

	// Default the cursor blink to the show cursor state
	GcData->BlinkVisible = TRUE;


   (GcData->SimpleTextOut.Mode)->CursorVisible = FALSE;    //since initial position of window is undefined we cannot draw cursor yet

    Status = GCSetAttribute(&(GcData->SimpleTextOut), EFI_BACKGROUND_BLACK | EFI_WHITE);  //set default attributes

    // Initialize the correct Mode for SimpleTextOut.  
    // Check current mode to be valid (already defined).  If so set the mode, otherwise set mode to 0
   StartMode = (GC_INITIAL_MODE < MaxTextMode) ? GC_INITIAL_MODE : 0;
    
    Status = GCSetMode(&(GcData->SimpleTextOut), StartMode); 
    if (!EFI_ERROR(Status)) {    //suitable mode found
      GCEnableCursor(&(GcData->SimpleTextOut), TRUE);    //enable cursor
    }

	// install the simple text out protocol
	Status = pBS->InstallMultipleProtocolInterfaces ( 
                    &ControllerHandle,            
                    &gEfiSimpleTextOutProtocolGuid, 
                    &GcData->SimpleTextOut,
                    &gAmiTextOutProtocolGuid,
                    &GcData->AmiTextOut,
                    NULL);
	
	if (EFI_ERROR(Status))
	{
		// close protocols and free allocated memory
		pBS->CloseProtocol( 
                        ControllerHandle, 
                        &gEfiGraphicsOutputProtocolGuid,
					    This->DriverBindingHandle, 
                        ControllerHandle);

        pBS->FreePool(GcData->SupportedModes);
        pBS->FreePool(GcData->SimpleTextOut.Mode);
		pBS->FreePool(GcData);
        return EFI_UNSUPPORTED;
    }

#if (CURSOR_BLINK_ENABLE != 0)
	EventStatus = pBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
					TPL_NOTIFY,
					BlinkCursorEvent,
					&(GcData->SimpleTextOut),
					&(GcData->CursorEvent)
					);
	if (!EFI_ERROR(EventStatus))
	{
		pBS->SetTimer(GcData->CursorEvent, TimerPeriodic, 5000000);
	}
#endif
	
	return Status;
}

/**
	Uninstalls the driver from given ControllerHandle.

	@param[in] This - pointer to protocol instance
	@param[in] ControllerHandle - controller handle to uninstall driver from
	@param[in] NumberOfChildren - number of children supported by this driver
	@param[in] ChildHandleBuffer  - pointer to child handles buffer

	@retval EFI_SUCCESS	- driver uninstalled from controller
	@retval EFI_NOT_STARTED - driver was not started
*/
EFI_STATUS	DriverBindingStop ( 
	IN EFI_DRIVER_BINDING_PROTOCOL *This,
	IN EFI_HANDLE                  ControllerHandle,
	IN UINTN                       NumberOfChildren,
	IN EFI_HANDLE                  *ChildHandleBuffer
)
{
	EFI_STATUS						Status;
	GC_DATA							*GcData = NULL;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	*SimpleTextOut = NULL;

	Status = pBS->OpenProtocol (
                            ControllerHandle, 
                            &gEfiSimpleTextOutProtocolGuid, 
					        &SimpleTextOut, 
                            This->DriverBindingHandle, 
					        ControllerHandle, 
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	if (EFI_ERROR (Status))
		return EFI_NOT_STARTED;

	Status = pBS->CloseProtocol (
                            ControllerHandle, 
                            &gEfiSimpleTextOutProtocolGuid, 
					        This->DriverBindingHandle, 
                            ControllerHandle);

	GcData = (GC_DATA *) SimpleTextOut;	

	// uninstall the simple text out protocol
	Status = pBS->UninstallMultipleProtocolInterfaces ( 
                            ControllerHandle,            
                            &gEfiSimpleTextOutProtocolGuid, 
                            &GcData->SimpleTextOut,
                            &gAmiTextOutProtocolGuid,
                            &GcData->AmiTextOut,
                            NULL);
	if (EFI_ERROR (Status))
		return Status;

#if (CURSOR_BLINK_ENABLE != 0)
	pBS->SetTimer(GcData->CursorEvent, TimerCancel, 0);
	pBS->CloseEvent(GcData->CursorEvent);
#endif

	Status = pBS->CloseProtocol( 
                            ControllerHandle, 
                            &gEfiGraphicsOutputProtocolGuid,
						    This->DriverBindingHandle, 
                            ControllerHandle);

    pBS->FreePool(GcData->SupportedModes);
    pBS->FreePool(GcData->SimpleTextOut.Mode);
    pBS->FreePool(GcData);

	return EFI_SUCCESS;
}

/**
    Resets the text output device

        
    @param[in] This pointer to the protocol instance
    @param[in] ExtendedVerification indicates that the driver should preform more
        exhausted verification of the device during reset

         
    @retval EFI_SUCCESS device reset properly
    @retval EFI_DEVICE_ERROR Device is not functioning properly
		
**/
EFI_STATUS  GCReset(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN BOOLEAN ExtendedVerification
)
{
    // Reset should:
    // - set the cursor back to (0,0)
    // - Clear the screen 
    This->SetAttribute(This, EFI_BACKGROUND_BLACK | EFI_WHITE);
    This->ClearScreen(This);
    This->SetCursorPosition(This, 0, 0);
    This->EnableCursor(This, PcdGetBool(PcdDefaultCursorState));
    return	EFI_SUCCESS;
}

/**
    Writes a string to the output device and advances the cursor 
    as necessary.

        
    @param[in] This - pointer to the protocol instance
    @param[in] String - pointer to string to be displayed to the screen

         
    @retval EFI_SUCCESS device reset properly
    @retval EFI_DEVICE_ERROR Device reported an Error while outputting a string
    @retval EFI_UNSUPPORTED The output device's mode is not currently in a defined state
    @retval EFI_WARN_UNKNOWN_GLYPH This warning code indicates that some of the 
        characters in the unicode string could not be rendered and were skipped
		
*/
EFI_STATUS GCOutputString(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN CHAR16                          *String
)
{
	EFI_STATUS		        Status;
	GC_DATA			        *GcData;
    BOOLEAN                 UnknownGlyph = FALSE;
    BOOLEAN                 CursorVisible;
    EFI_IMAGE_OUTPUT        *Glyph = NULL;
	
	// find private data structure	
	GcData = (GC_DATA *) This;	
	
    CursorVisible = (GcData->SimpleTextOut.Mode)->CursorVisible;
	if (CursorVisible)    
        This->EnableCursor(This, FALSE);
	
	// now loop through the string and display it to the output device
	while (*String != 0)
	{
		switch (*String)
		{
			case CARRIAGE_RETURN:
			    FlushString(GcData);
                This->SetCursorPosition(
                                    This, 
                                    0, 
                                    (GcData->SimpleTextOut.Mode)->CursorRow);
				break;

			case LINE_FEED:
			    FlushString(GcData);
                if((GcData->SimpleTextOut.Mode)->CursorRow == (GcData->MaxRows - 1))
                {
                    ScrollUp(GcData);
                    //cursor position not changed, but image under it does - save new image
                    SaveCursorImage(GcData);
                }
                else
                    This->SetCursorPosition(
                                    This, 
                                    (GcData->SimpleTextOut.Mode)->CursorColumn, 
                                    (GcData->SimpleTextOut.Mode)->CursorRow + 1);
				break;
			
			case BACKSPACE:
			    FlushString(GcData);
                if((GcData->SimpleTextOut.Mode)->CursorColumn != 0)
                    This->SetCursorPosition(
                                    This, 
                                    (GcData->SimpleTextOut.Mode)->CursorColumn - 1, 
                                    (GcData->SimpleTextOut.Mode)->CursorRow);
				break;
			
			default:
                Status = GcData->HiiFont->GetGlyph(
                                                GcData->HiiFont,
                                                *String,
                                                NULL,
                                                &Glyph,
                                                0);
                if(EFI_ERROR(Status)) {
                    return Status;
                }

                if(Status == EFI_WARN_UNKNOWN_GLYPH){
                	 UnknownGlyph = TRUE;
                	 if (( Glyph->Width != WIDE_GLYPH_WIDTH) && (Glyph->Width != NARROW_GLYPH_WIDTH))
                		 Glyph->Width = NARROW_GLYPH_WIDTH; // In case of incorrect  Glyph->Width returned
                }
                
                AddChar(GcData, *String, Glyph->Width / 8);

                if(Glyph != NULL) {
                    pBS->FreePool(Glyph->Image.Bitmap);
                    pBS->FreePool(Glyph);
                    Glyph = NULL; 
                }
                
				break; // end of default case
		}

		String++;
	}
	FlushString(GcData);
	if (CursorVisible)    
        This->EnableCursor(This, TRUE);

	return (UnknownGlyph) ? EFI_WARN_UNKNOWN_GLYPH : EFI_SUCCESS;
}

/**
    Tests to see if the String has the glyphs that correspond to
    each character in the string

    @param[in] This - pointer to the protocol instance
    @param[in] String - pointer to a string that needs to be tested
         
    @retval EFI_SUCCESS all characters can be drawn
    @retval EFI_UNSUPPORTED there are characters that cannot be drawn
*/
EFI_STATUS  GCTestString(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN CHAR16                          *String
)
{
	GC_DATA		*GcData;
    EFI_STATUS  Status;
    EFI_IMAGE_OUTPUT *Glyph = NULL;
	
	// find private data structure	
	GcData = (GC_DATA *) This;	
	
	// now loop through the string
	while (*String != 0)
	{
		switch (*String)
		{
			case CARRIAGE_RETURN: case LINE_FEED: case BACKSPACE:
				break;
			default:
                Status = GcData->HiiFont->GetGlyph(
                                                GcData->HiiFont,
                                                *String,
                                                NULL,
                                                &Glyph,
                                                0);
                if(EFI_ERROR(Status))
                    return Status;

                pBS->FreePool(Glyph->Image.Bitmap);
                pBS->FreePool(Glyph);
                if(Status == EFI_WARN_UNKNOWN_GLYPH)
                    return EFI_UNSUPPORTED;
                Glyph = NULL; 
				break;
		}
		String++;
	}
	return EFI_SUCCESS;
}

/**
    Returns information for an available text mode that the output
    device supports

    @param[in] This - pointer to the protocol instance
    @param[in] ModeNum - The mode to return information on
    @param[out] Col - the number of columns supported
    @param[out] Row - the number of rows supported

    @retval EFI_SUCCESS device reset properly
    @retval EFI_DEVICE_ERROR Device reported an Error while outputting a string
    @retval EFI_UNSUPPORTED The ModeNumber is not supported
*/
EFI_STATUS GCQueryMode(
	IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN  UINTN                           ModeNum,
	OUT UINTN                           *Col,
	OUT UINTN                           *Row
)
{
	GC_DATA		*GcData;
    INT32      i = 0;
	
	GcData = (GC_DATA *) This;	

    if(ModeNum >= (UINTN)(GcData->SimpleTextOut.Mode)->MaxMode)
        return EFI_UNSUPPORTED;

    while(i<MaxTextMode && GcData->SupportedModes[i].ModeNum != ModeNum)
        i++;

    if(i>=MaxTextMode || !GcData->SupportedModes[i].Supported)
        return EFI_UNSUPPORTED;

	// if the mode is a valid mode, return the data from the array of
	//	for the height and width
	*Col = GcData->SupportedModes[i].Col;
	*Row = GcData->SupportedModes[i].Row;
	
	return EFI_SUCCESS;
}

/**
    Sets the text mode to the requested mode.  It checks to see if
    it is a valid mode

    @param[in] This - pointer to the protocol instance
    @param[in] ModeNum - mode number to change to

    @retval EFI_SUCCESS the new mode is valid and has been set
    @retval EFI_UNSUPPORTED the new mode is not valid
*/
EFI_STATUS GCSetMode(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN                           ModeNum
)
{
	EFI_STATUS	Status;
	GC_DATA		*GcData;
    BOOLEAN     CursorVisible;
    INT32      i = 0;
    UINT32      DeltaX, DeltaY;
    UINT32      SaveX, SaveY;

	GcData = (GC_DATA *) This;	

    if(ModeNum >= (UINTN)(GcData->SimpleTextOut.Mode)->MaxMode)
        return EFI_UNSUPPORTED;

    while(i<MaxTextMode && GcData->SupportedModes[i].ModeNum != ModeNum)
        i++;

    if(i>=MaxTextMode || !GcData->SupportedModes[i].Supported)
        return EFI_UNSUPPORTED;

    SaveX = GcData->DeltaX;
    SaveY = GcData->DeltaY;

    DeltaX = (GcData->SupportedModes[i].VideoCol - 
              GcData->SupportedModes[i].Col * NARROW_GLYPH_WIDTH) / 2;
    DeltaY = (GcData->SupportedModes[i].VideoRow - 
              GcData->SupportedModes[i].Row * EFI_GLYPH_HEIGHT) / 2;

    //save cursor status and hide it if nesessary
    CursorVisible = (GcData->SimpleTextOut.Mode)->CursorVisible;
	if (CursorVisible)
		This->EnableCursor(This, FALSE);

    if(GcData->GraphicsOutput->Mode->Mode != GcData->SupportedModes[i].GraphicsMode)
    {
        Status = GcData->GraphicsOutput->SetMode(GcData->GraphicsOutput,
                                                 GcData->SupportedModes[i].GraphicsMode);
        if(EFI_ERROR(Status))
            return Status;

        if(GcData->OemClearScreen != GcInternalClearScreen) //we have porting hook installed - call it
            This->ClearScreen(This);
        else
        	GcData->OemClearScreen(GcData);
    }
    else
        This->ClearScreen(This);    //call clear screen with old values

    if(GcData->DeltaX == SaveX && GcData->DeltaY == SaveY)
    {                               //initial position was not changed by porting hook inside C
                                    //ClearScreen function
        GcData->DeltaX = DeltaX;
        GcData->DeltaY = DeltaY;
    }

    GcData->MaxColumns = GcData->SupportedModes[i].Col;
    GcData->MaxRows = GcData->SupportedModes[i].Row;
    (GcData->SimpleTextOut.Mode)->Mode = GcData->SupportedModes[i].ModeNum;

    // Set the cursor position to 0,0
    GCSetCursorPosition(This, 0, 0);

	//  restore cursor at new position
	if (CursorVisible)
		This->EnableCursor(This, TRUE);
	
    if(TextBuffer == NULL || TextBufferSize < GcData->MaxColumns) {
        if(TextBuffer)
            pBS->FreePool(TextBuffer);

        Status = pBS->AllocatePool(EfiBootServicesData, (GcData->MaxColumns + 1) * sizeof(CHAR16), &TextBuffer);
        if(EFI_ERROR(Status))
            return Status;

        TextBufferSize = GcData->MaxColumns + 1;    //additional space for null-terminator
        Position = 0;
        StringWidth = 0;
    }

	return EFI_SUCCESS;
}

/**
    Sets the foreground color and background color for the screen

    @param[in] This - pointer to the protocol instance
    @param[in] Attribute - the attributes to set

    @retval EFI_SUCCESS the attribute was changed successfully
    @retval EFI_DEVICE_ERROR The device had an error
    @retval EFI_UNSUPPORTED The attribute is not supported
*/
EFI_STATUS  GCSetAttribute(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN                           Attribute
)
{
	GC_DATA	*GcData;
	
	if ((Attribute | 0xFF) != 0xFF) 
	{
	    return EFI_UNSUPPORTED;
	}
	
	GcData = (GC_DATA *) This;
	
	(GcData->SimpleTextOut.Mode)->Attribute = (INT32) (0xFF & Attribute);

	return EFI_SUCCESS;
}

/**
    Clears the text screen

    @param[in] This - pointer to the protocol instance

    @retval EFI_SUCCESS the screen was cleared
*/
EFI_STATUS  GCClearScreen(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
)
{
	GC_DATA			              *GcData;
	INT32 i=0;

	GcData = (GC_DATA *) This;
	while(i<MaxTextMode && GcData->SupportedModes[i].ModeNum != (GcData->SimpleTextOut.Mode)->Mode)
		i++;
	if(i<MaxTextMode && GcData->GraphicsOutput->Mode->Mode != GcData->SupportedModes[i].GraphicsMode)
		GcData->GraphicsOutput->SetMode(
			GcData->GraphicsOutput, GcData->SupportedModes[i].GraphicsMode
		);

    GcData->OemClearScreen(GcData);

	return EFI_SUCCESS;
}

/**
    This function sets the position of the cursor

    @param[in] This - pointer to the protocol instance
    @param[in] Column - the new column
    @param[in] Row - The new row

    @retval EFI_SUCCESS the cursor position was changed
    @retval EFI_DEVICE_ERROR The device had an error
    @retval EFI_UNSUPPORTED The device is not in a valid text mode or the 
            cursor position is not valid
*/
EFI_STATUS GCSetCursorPosition(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN                           Column,
	IN UINTN                           Row
)
{
	GC_DATA	*GcData;
    BOOLEAN CursorVisible;
	
	GcData = (GC_DATA *) This;
	
	// check for a valid text mode and check for a valid position 
	//	on the screen
	
	if ( ((UINT32)Column >= GcData->MaxColumns) || 
		 ((UINT32)Row >= GcData->MaxRows) )
		return EFI_UNSUPPORTED;
		
	
    //save cursor status and hide it if nesessary
    CursorVisible = (GcData->SimpleTextOut.Mode)->CursorVisible;
	if (CursorVisible)
		This->EnableCursor(This, FALSE);
	
	(GcData->SimpleTextOut.Mode)->CursorColumn = (INT32)Column;
	(GcData->SimpleTextOut.Mode)->CursorRow = (INT32)Row;

	
	//  restore cursor at new position
	if (CursorVisible)
		This->EnableCursor(This, TRUE);
	
	return EFI_SUCCESS;
}

/**
    Makes cursor invisible or visible

    @param[in] This - pointer to the protocol instance
    @param[in] Visible - a boolean that if TRUE the cursor will be visible
        if FALSE the cursor will be invisible

    @retval EFI_SUCCESS the cursor visibility was set correctly
    @retval EFI_DEVICE_ERROR The device had an error
    @retval EFI_UNSUPPORTED The device does not support visibilty control
            for the cursor
*/
EFI_STATUS  GCEnableCursor(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN BOOLEAN                         Visible
)
{
	GC_DATA	*GcData;
	
	GcData = (GC_DATA *) This;

	// check to see if the we are already set to the same cursor visibility mode
	if (Visible != (GcData->SimpleTextOut.Mode)->CursorVisible)
	{
		(GcData->SimpleTextOut.Mode)->CursorVisible = Visible;
//if we put cursor back we have to update image under it in order it contains older data
        if(Visible)
            SaveCursorImage(GcData);

		DrawCursor(GcData, Visible);
	}
	
	return EFI_SUCCESS;
}


/**
    Turns color attributes into Pixel values

    @param[in] Attribute - The color to set
    @param[out] Foreground - foreground color
    @param[out] Background - background color

    @retval EFI_SUCCESS valid colors returned
*/
EFI_STATUS GetColorFromAttribute(
    IN  UINT32                         Attribute, 
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Foreground, 
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Background
)
{
	UINT8 IndexF;
	UINT8 IndexB;

    IndexF = (UINT8)(Attribute & 0x0f);
    IndexB = (UINT8)((Attribute >> 4) & 0x0f);

	*Foreground = ColorArray[IndexF];
	*Background = ColorArray[IndexB];

	return EFI_SUCCESS;
}

/**
    This function draws /hides the cursor in the current cursor position

    @param[in] GcData Private data structure for SimpleTextOut interface
    @param[in] Visible if TRUE - draws cursor, if FALSE - hides cursor

*/
VOID DrawCursor(
    IN GC_DATA *GcData, 
    IN BOOLEAN Visible
)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Fill;

    if(Visible)
    {
        Fill = ColorArray[((GcData->SimpleTextOut.Mode)->Attribute & 0x0f)];    //get foreground color
        GcData->GraphicsOutput->Blt(
            GcData->GraphicsOutput,
            &Fill,
            EfiBltVideoFill,
            0,
            0,                   
            GcData->DeltaX + (GcData->SimpleTextOut.Mode)->CursorColumn * NARROW_GLYPH_WIDTH,
            GcData->DeltaY + (GcData->SimpleTextOut.Mode)->CursorRow * EFI_GLYPH_HEIGHT + CURSOR_OFFSET,
            NARROW_GLYPH_WIDTH,
            CURSOR_THICKNESS,
            0); 
    }
    else
    {
        GcData->GraphicsOutput->Blt(
            GcData->GraphicsOutput,
            GcData->Cursor,
            EfiBltBufferToVideo,
            0,
            0,                   
            GcData->DeltaX + (GcData->SimpleTextOut.Mode)->CursorColumn * NARROW_GLYPH_WIDTH,
            GcData->DeltaY + (GcData->SimpleTextOut.Mode)->CursorRow * EFI_GLYPH_HEIGHT + CURSOR_OFFSET,
            NARROW_GLYPH_WIDTH,
            CURSOR_THICKNESS,
            NARROW_GLYPH_WIDTH * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)); 
    }                                   
}

/**
    This is the function that makes the cursor blink. A timer event 
    is created that will cause this function to be called

    @param[in] Event - event that was triggered
    @param[in] Context - pointer to the event context

*/
VOID EFIAPI BlinkCursorEvent(
	IN EFI_EVENT Event,
	IN VOID      *Context
)
{
	GC_DATA	*GcData;
	
	GcData = (GC_DATA *) Context;

	if (!(GcData->SimpleTextOut.Mode)->CursorVisible)
		return;

	if (GcData->BlinkVisible)
	{
		// remove the cursor from the screen
		DrawCursor(GcData, FALSE);
        GcData->BlinkVisible = FALSE;
	}
	else		
	{
		// put cursor back
		DrawCursor(GcData, TRUE);
        GcData->BlinkVisible = TRUE;
	}
}

/**
    This function returns graphics mode number, correspondend with given
    horizontal and vertical resolution. If either HorRes or VerRes are equal
    to MAX_RES, the highest supported resolution will be returned.

    @param[in] GraphicsOutput - pointer to Gop driver
    @param[in] HorRes - required horizontal resolution
    @param[in] VerRes - required vertical resolution
    @param[out] ModeNum - returned graphics mode number 
    @param[in] ExactMatch - TRUE indicates that only an exact match should succeed. FALSE indicates that if a higher resolution is supported, it can be substituted.
    @param[out] ActualHorRes OPTIONAL - On return, holds the actual horizontal resolution that was found to match
    @param[out] ActualVerRes OPTIONAL - On return, holds the actual vertical resolution that was found to match

    @retval EFI_SUCCESS correct mode number returned
    @retval EFI_NOT_FOUND mode number not found for given resolution
*/
EFI_STATUS GetGraphicsModeNumber (
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput, 
    IN  UINT32                       HorRes,
    IN  UINT32                       VerRes,
	OUT UINT32                       *ModeNum,
    IN  BOOLEAN                      ExactMatch,
    OUT UINT32                       *ActualHorRes OPTIONAL,
    OUT UINT32                       *ActualVerRes OPTIONAL
)
{
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION	Info;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION	*pInfo = &Info;
	EFI_STATUS				                Status;
	UINTN							        SizeOfInfo;
	UINT32							        i;
    UINT32                                  MaxHorizontalResolution = 0;
    UINT32                                  MaxVerticalResolution = 0;
    UINT32                                  MaxModeNumber = 0;

	for(i = 0; i < GraphicsOutput->Mode->MaxMode; i++) {
		Status = GraphicsOutput->QueryMode(GraphicsOutput, i, &SizeOfInfo, &pInfo);

		if (!EFI_ERROR(Status)) 
		{ 
            if(HorRes == MAX_RES || VerRes == MAX_RES )
            {
                if( MultU64x32(pInfo->HorizontalResolution, pInfo->VerticalResolution) > MultU64x32(MaxHorizontalResolution, MaxVerticalResolution))
                {
                    MaxHorizontalResolution = pInfo->HorizontalResolution;
                    MaxVerticalResolution = pInfo->VerticalResolution;
                    MaxModeNumber = i;
                }
                continue;
            }
            
            if (ExactMatch && pInfo->HorizontalResolution == HorRes && pInfo->VerticalResolution == VerRes )
            {
			    MaxHorizontalResolution = pInfo->HorizontalResolution;
                MaxVerticalResolution = pInfo->VerticalResolution;
                MaxModeNumber = i;
                break;
            }
            if(!ExactMatch && HorRes <= pInfo->HorizontalResolution && VerRes <= pInfo->VerticalResolution && 
                MultU64x32(pInfo->HorizontalResolution, pInfo->VerticalResolution) > MultU64x32(HorRes, VerRes))
            {
                if(MaxHorizontalResolution == 0 || MaxVerticalResolution == 0 || 
                    ( MultU64x32(MaxHorizontalResolution, MaxVerticalResolution) > MultU64x32(pInfo->HorizontalResolution, pInfo->VerticalResolution))
                  )
            	{
			    MaxHorizontalResolution = pInfo->HorizontalResolution;
                MaxVerticalResolution = pInfo->VerticalResolution;
                MaxModeNumber = i;
                }
            }
		}
	}
    
    if (MaxHorizontalResolution == 0 || MaxVerticalResolution == 0)
        return EFI_NOT_FOUND;
        
    if (ActualHorRes != NULL)
        *ActualHorRes = MaxHorizontalResolution;
    if (ActualVerRes != NULL)
        *ActualVerRes = MaxVerticalResolution;

    *ModeNum = MaxModeNumber;
    return EFI_SUCCESS;
}


/**
    This function fills array of supported text modes

    @param[in] GcData pointer to private protocol data structure

    @retval EFI_SUCCESS function executed successfully
    @retval EFI_UNSUPPORTED no supported modes found
*/
EFI_STATUS SetupGraphicsDevice(
    IN GC_DATA *GcData
)
{
    EFI_STATUS Status;
    INT32 i;
    UINT32 GraphicsModeNumber;
    INT32 MaxSupportedModes = 1;
    BOOLEAN DefaultModeNotSupported = FALSE;
    INT32 ModeZeroIndex = 0;
    UINT32 VideoCol, VideoRow;

    for(i = 0; i < MaxTextMode; i++) {
        if (TextModeArray[i].VideoCol == MIN_RES ||
            TextModeArray[i].VideoRow == MIN_RES)
        {
            // Get the minimum video resolution that supports YxZ rows x columns
            Status = GetGraphicsModeNumber (
                GcData->GraphicsOutput,
                TextModeArray[i].Col * NARROW_GLYPH_WIDTH,
                TextModeArray[i].Row * EFI_GLYPH_HEIGHT,
                &GraphicsModeNumber,
                FALSE,
                &VideoCol,
                &VideoRow
            );
        } else
        {
            // Get the requested video resolution
            Status = GetGraphicsModeNumber (
                GcData->GraphicsOutput,
                TextModeArray[i].VideoCol,
                TextModeArray[i].VideoRow,
                &GraphicsModeNumber,
                TRUE,
                &VideoCol,
                &VideoRow
            );
        }
        
        if(!EFI_ERROR(Status)) {
            GcData->SupportedModes[i].Supported = TRUE;
            GcData->SupportedModes[i].GraphicsMode = GraphicsModeNumber;
            GcData->SupportedModes[i].ModeNum = TextModeArray[i].ModeNum;
            
            if( TextModeArray[i].Col == MAX_RES || 
                TextModeArray[i].Col == MIN_RES )
                // Calculate the optimal number of text columns based on horizontal resolution
                GcData->SupportedModes[i].Col = VideoCol / NARROW_GLYPH_WIDTH;
            else
                GcData->SupportedModes[i].Col = TextModeArray[i].Col;

            if( TextModeArray[i].Row == MAX_RES ||
                TextModeArray[i].Row == MIN_RES )
                // Calculate the optimal number of text rows based on vertical resolution
                GcData->SupportedModes[i].Row = VideoRow / EFI_GLYPH_HEIGHT;
            else
                GcData->SupportedModes[i].Row = TextModeArray[i].Row;

            // Save the video resolution for this mode
            GcData->SupportedModes[i].VideoCol = VideoCol;
            GcData->SupportedModes[i].VideoRow = VideoRow;

            MaxSupportedModes = (TextModeArray[i].ModeNum >= MaxSupportedModes) ? 
                                 TextModeArray[i].ModeNum + 1 : MaxSupportedModes;
            
            // Ensure there are enough pixels for the number of rows/cols
            if (VideoCol / NARROW_GLYPH_WIDTH < (UINT32)GcData->SupportedModes[i].Col ||
                VideoRow / EFI_GLYPH_HEIGHT < (UINT32)GcData->SupportedModes[i].Row)
                GcData->SupportedModes[i].Supported = FALSE;
        } else {
            GcData->SupportedModes[i].Supported = FALSE;
            GcData->SupportedModes[i].ModeNum = TextModeArray[i].ModeNum;

            if(TextModeArray[i].ModeNum == 0) {
                ModeZeroIndex = i;
                DefaultModeNotSupported = TRUE;
            }
        }
    }

    if(DefaultModeNotSupported) {
        // Fallback to minimum specified by UEFI spec
        Status = GetGraphicsModeNumber(GcData->GraphicsOutput,
                                       MODE_ZERO_MIN_HOR_RES,
                                       MODE_ZERO_MIN_VER_RES,
                                       &GraphicsModeNumber,
                                       FALSE,
                                       &(GcData->SupportedModes[ModeZeroIndex].VideoCol),
                                       &(GcData->SupportedModes[ModeZeroIndex].VideoRow));
        if(EFI_ERROR(Status)) {
            (GcData->SimpleTextOut.Mode)->MaxMode = 0;
            return EFI_UNSUPPORTED;
        }

        GcData->SupportedModes[ModeZeroIndex].Supported = TRUE;
        GcData->SupportedModes[ModeZeroIndex].GraphicsMode = GraphicsModeNumber;
        GcData->SupportedModes[ModeZeroIndex].Row = 25;
        GcData->SupportedModes[ModeZeroIndex].Col = 80;
		GcData->SupportedModes[ModeZeroIndex].ModeNum = TextModeArray[ModeZeroIndex].ModeNum;
    }

    (GcData->SimpleTextOut.Mode)->MaxMode = MaxSupportedModes;
    return EFI_SUCCESS;
}


/**
    This function scrolls screen one row up and clears bottom row

    @param[in] GcData - pointer to private protocol data structure

*/
VOID ScrollUp(
    IN GC_DATA *GcData
)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Fill;

    GcData->GraphicsOutput->Blt(GcData->GraphicsOutput,
                                &Fill,
                                EfiBltVideoToVideo,
                                GcData->DeltaX,
                                GcData->DeltaY + EFI_GLYPH_HEIGHT,                   
                                GcData->DeltaX,
                                GcData->DeltaY,
                                GcData->MaxColumns * NARROW_GLYPH_WIDTH,
                                (GcData->MaxRows - 1) * EFI_GLYPH_HEIGHT,
                                0);
//clear bottom line
    Fill = ColorArray[(((GcData->SimpleTextOut.Mode)->Attribute >> 4) & 0xf)];
    GcData->GraphicsOutput->Blt(GcData->GraphicsOutput,
                                &Fill,
                                EfiBltVideoFill,
                                0,
                                0,                   
                                GcData->DeltaX,
                                GcData->DeltaY + (GcData->MaxRows - 1) * EFI_GLYPH_HEIGHT,
                                GcData->MaxColumns * NARROW_GLYPH_WIDTH,
                                EFI_GLYPH_HEIGHT,
                                0);
}


/**
    This function saves image under cursor to restore, when cursor moves

    @param[in] GcData - pointer to private protocol data structure
      
*/
VOID SaveCursorImage(
    IN GC_DATA *GcData
)
{
    GcData->GraphicsOutput->Blt(
            GcData->GraphicsOutput,
            GcData->Cursor,
            EfiBltVideoToBltBuffer,
            GcData->DeltaX + (GcData->SimpleTextOut.Mode)->CursorColumn * NARROW_GLYPH_WIDTH,
            GcData->DeltaY + (GcData->SimpleTextOut.Mode)->CursorRow * EFI_GLYPH_HEIGHT + CURSOR_OFFSET,
            0,
            0,                   
            NARROW_GLYPH_WIDTH,
            CURSOR_THICKNESS,
            NARROW_GLYPH_WIDTH * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
}    


/**
    This function shifts cursor right to number of columns defined in Step
    If cursor reaches right edge of the screen it moves one line down, scrolling screen
    if nesessary

    @param[in] GcData - pointer to private protocol data structure
    @param[in] Step - number of columns to shift cursor right

*/
VOID ShiftCursor(
    IN GC_DATA *GcData,
    IN UINT16  Step
)
{
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This = (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *)GcData;

    if(((GcData->SimpleTextOut.Mode)->CursorColumn + Step) >= (INT32)GcData->MaxColumns)
    {
        if((GcData->SimpleTextOut.Mode)->CursorRow == GcData->MaxRows - 1)
        {
            ScrollUp(GcData);
            This->SetCursorPosition(
                              This, 
                              0, 
                              (GcData->SimpleTextOut.Mode)->CursorRow);
        }
        else
        {
            This->SetCursorPosition(
                              This, 
                              0, 
                              (GcData->SimpleTextOut.Mode)->CursorRow + 1);
        }
    }
    else
    {
        This->SetCursorPosition(
                              This, 
                              (GcData->SimpleTextOut.Mode)->CursorColumn + Step, 
                              (GcData->SimpleTextOut.Mode)->CursorRow);
    }
}


/**
    This function is a porting hook to implement specific action on the Blt buffer before put it on screen
               
    @param[in] This - pointer to internal structure
    @param[in] Width - width of passed buffer in pixels
    @param[in] Height - height of passed buffer in pixels
    @param[inout] BltBuffer - pointer to Blt buffer to perform action upon
         
*/
VOID GcUpdateBltBuffer (
	IN     GC_DATA 			             *This,
	IN     UINT32			             Width,
    IN     UINT32                        Height,
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer
)
{
    return;
}


/**
    This function is a porting hook to implement specific action when
    clear screen operation is needed

    @param[in] This pointer to private protocol data structure
               
*/
VOID GcInternalClearScreen (
	IN OUT GC_DATA *This
)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Fill;
    EFI_STATUS Status;
    UINT32 StartX;
    UINT32 StartY;
    UINT32 SizeX;
    UINT32 SizeY;

    if(This->MaxColumns == 0 || This->MaxRows == 0) { //this is the first invocation
        StartX = 0;
        StartY = 0;
        SizeX = This->GraphicsOutput->Mode->Info->HorizontalResolution;
        SizeY = This->GraphicsOutput->Mode->Info->VerticalResolution;
    } else {
        StartX = This->DeltaX;
        StartY = This->DeltaY;
        SizeX = This->MaxColumns * NARROW_GLYPH_WIDTH;
        SizeY = This->MaxRows * EFI_GLYPH_HEIGHT;
    }

    Fill = ColorArray[(((This->SimpleTextOut.Mode)->Attribute >> 4) & 0xf)];
    This->GraphicsOutput->Blt(This->GraphicsOutput,
                                &Fill,
                                EfiBltVideoFill,
                                0,
                                0,                   
                                StartX,
                                StartY,
                                SizeX,
                                SizeY,
                                0);

	Status = This->SimpleTextOut.SetCursorPosition(&(This->SimpleTextOut), 0, 0);
    if(EFI_ERROR(Status)) { //on first invocation this failed because MaxRows = MaxCols = 0
        (This->SimpleTextOut.Mode)->CursorColumn = 0;
	    (This->SimpleTextOut.Mode)->CursorRow = 0;
    }
}

/**
    This function adds another character to text buffer

    @param[in] GcData Pointer to private protocol data structure
    @param[in] Char   Character to add
    @param[in] Width  Character width (1 - normal, 2 - wide)
               
*/
VOID AddChar(
    IN GC_DATA *GcData,
    IN CHAR16 Char,
    IN UINT16 Width
)
{
    UINT32 CurrentWidth;

    CurrentWidth = (GcData->SimpleTextOut.Mode)->CursorColumn + StringWidth;
    if((CurrentWidth + Width) > GcData->MaxColumns) {
        FlushString(GcData);

        if(Width == 2)
            ShiftCursor(GcData, 1);
    }

    TextBuffer[Position] = Char;
    Position++;
    StringWidth += Width;
}

/**
    This function puts prepared text buffer to the screen

    @param[in] GcData Pointer to private protocol data structure
               
*/
VOID FlushString(
    IN GC_DATA *GcData
)
{
    EFI_STATUS Status;
    EFI_FONT_DISPLAY_INFO FontInfo;
    EFI_IMAGE_OUTPUT      *Image = NULL;
    
    if(Position == 0)
        return;

//retreive colors
    GetColorFromAttribute(
                        (GcData->SimpleTextOut.Mode)->Attribute,
                        &FontInfo.ForegroundColor,
                        &FontInfo.BackgroundColor);
//use system font
    FontInfo.FontInfoMask =   EFI_FONT_INFO_SYS_FONT 
                            | EFI_FONT_INFO_SYS_SIZE 
                            | EFI_FONT_INFO_SYS_STYLE;

/* put NULL-terminator */
    TextBuffer[Position] = 0;
    
    Status = GcData->HiiFont->StringToImage(GcData->HiiFont,
                                    0,
                                    TextBuffer,
                                    &FontInfo,
                                    &Image,
                                    0, 0, NULL, NULL, NULL);

    GcData->OemUpdateBltBuffer(GcData, Image->Width, Image->Height, Image->Image.Bitmap);

    GcData->GraphicsOutput->Blt(
                    GcData->GraphicsOutput,
                    Image->Image.Bitmap,
                    EfiBltBufferToVideo,
                    0,
                    0,                   
                    GcData->DeltaX + (GcData->SimpleTextOut.Mode)->CursorColumn * NARROW_GLYPH_WIDTH,
                    GcData->DeltaY + (GcData->SimpleTextOut.Mode)->CursorRow * EFI_GLYPH_HEIGHT,
                    Image->Width,
                    Image->Height,
                    Image->Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));     

    ShiftCursor(GcData, (Image->Width / 8));

    pBS->FreePool(Image->Image.Bitmap);
    pBS->FreePool(Image);

    Position = 0;
    StringWidth = 0;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
