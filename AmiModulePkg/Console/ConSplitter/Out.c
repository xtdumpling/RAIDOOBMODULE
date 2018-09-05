//***************************************************************************
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//***************************************************************************
//
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Comment out the "EnableCursor" to prevent cursor blinking under logo
//    Reason:   
//    Auditor:  Mark Chen(Mike Tsai)
//    Date:     Dec/01/2016
//
//  Rev. 1.02
//    Bug Fix:  Fix SOL will happen garbage message when no LOGO.
//    Reason:   Add token.h to associate SMC R1.00 to fix this issue.
//    Auditor:  LiYen Chiu
//    Date:     Nov/26/15
//
//  Rev. 1.01
//    Bug Fix:  Initialize RowOffset and LastColumn to zero
//    Reason:   There is no init value if optimization is enabled. 
//              Then EFI driver (e.g. NvmExpressDxe) will get E exception when calling it.
//    Auditor:  AMI
//    Date:     Mar/20/14
//
//  Rev. 1.00
//    Bug Fix:  Fix console screen corrupte issue. (Refer from Romely)
//    Reason:   The RestoreTextScreen routine does not change row but only output buffer string cause this issue.
//    Auditor:  Jacker Yeh
//    Date:     Dec/31/13
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
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
    File contains the Simple Text Output functionality for the
    Console Splitter Driver

**/

#include "ConSplit.h"
#include <Protocol/GraphicsOutput.h>
#include <Token.h>

/// Pointer to the SupportedModes table that contains the list of supported modes 
SUPPORT_RES *SupportedModes = NULL;

/// The Consplitter's Simple Text Out Mode data
SIMPLE_TEXT_OUTPUT_MODE	MasterMode =
	{
	1, 		// MaxMode
	0, 		// Mode  - defines the current mode for the system
	0x0F,	// Attribute
	0, 		// Column
	0, 		// Row
	1  		// CursorVisible
	};

/// The ConSpitter's simple text out protocol
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	mCSOutProtocol	=
	{
	CSReset,
	CSOutputString,
	CSTestString,
	CSQueryMode,
	CSSetMode,
	CSSetAttribute,
	CSClearScreen,
	CSSetCursorPosition,
	CSEnableCursor,
	&MasterMode
	};

/// Buffer pointed used to keep a memory copy of the strings currently being displayed on the screen
CHAR16 *ScreenBuffer =  NULL;

/// Pointer to the end of the current screen buffer
CHAR16 *EndOfTheScreen = NULL;

/// Pointer used to save the current screen when disconnecting the console out devices
CHAR16 *SaveScreenBuffer =  NULL;

/// Buffer used to save the current screen attributes
INT32 *AttributeBuffer = NULL;

/// Buffer used to save the current screen attributes when disconnecting the console out devices
INT32 *SaveAttributeBuffer = NULL;

/// Value used as the length of a row of characters. Used as part of the offset into the saved screen buffer and attribute buffers
INT32 Columns = 0;

/// Variable that tracks if the screen is currently empty.
BOOLEAN BlankScreen = TRUE;

/// Variable used to store the current GOP screen when disconnecting the graphcis console
EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelBuffer = NULL;

/// Variable for storing the current graphics mode
UINT32 GraphicsMode = 0;

/// Variable for storing the current text mode
UINT32 TextMode = 0;

/**
    Function to scroll the screen and attribute buffers one line up.
**/
VOID ScrollScreen(VOID)
{
    INT32   i;
    CHAR16 *pChar = EndOfTheScreen - Columns;
    INT32  *pAttr = AttributeBuffer + (pChar-ScreenBuffer);
    
    UINT32 SizeOfScreen = (UINT32)((UINT8*)EndOfTheScreen-(UINT8*)ScreenBuffer);
    UINT32 SizeOfAttribute = (UINT32)SizeOfScreen*(sizeof(INT32)/sizeof(CHAR16));
    
    pBS->CopyMem(ScreenBuffer, ScreenBuffer+Columns, (SizeOfScreen-(sizeof(CHAR16)*Columns)));
    pBS->CopyMem(AttributeBuffer, (AttributeBuffer+Columns), (SizeOfAttribute-(sizeof(UINT32)*Columns)));

    for(i=0; i<Columns; i++)
    {
        *pChar++ = ' ';
        *pAttr++ = MasterMode.Attribute;
    }
}

/**
    Function to output the passed string into the internal ConsoleSplitter
    memory buffer.

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param String Pointer to the string
    @param RowOffset Pointer to store value of row offset after string output
    @param LastColumn Ppointer to store value of column after string output

    @retval EFI_SUCCESS function executed successfully
**/
EFI_STATUS MemOutputString(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN CHAR16                          *String,
    OUT INT32                          *RowOffset,
    OUT INT32                          *LastColumn
)
{
    INT32 RowOff = 0;
	INT32 Col = MasterMode.CursorColumn;
	UINTN Offset = MasterMode.CursorRow * Columns;
	CHAR16 *pChar = ScreenBuffer + Offset + Col;
	INT32 *pAttr =  AttributeBuffer + Offset + Col;

	BlankScreen = FALSE;


	while(TRUE){
		switch(*String){
		case 0:
		    if(RowOffset)
		        *RowOffset = RowOff;
		    if(LastColumn)
		        *LastColumn = Col;

			return EFI_SUCCESS;
    		case '\n':
	    		if ((pChar + Columns) >= EndOfTheScreen)
                    ScrollScreen();
    			else
                {
                    pChar += Columns;
                    pAttr += Columns;
                    RowOff++;
                }
    	    	break;

            case '\r':
                pChar -= Col;
                pAttr -= Col;
                Col = 0;
                break;

            case '\b':
                if (Col)
                {
                    pChar--;
                    pAttr--;
                    Col--;
                }
                break;

            default:
                if ((pChar + 1) >= EndOfTheScreen)
                {
                    ScrollScreen();
                    pChar -= Col;
                    pAttr -= Col;
                    Col = 0;
                }
                else
                {
                    *pChar++ = *String;
                    *pAttr++ = MasterMode.Attribute;
                    Col++;
                    if (Col >= Columns) {
                        RowOff++;
                        Col = 0;
                    }
                }
        		break;
		}
		String++;
	}

	return EFI_SUCCESS;
}

/**
    Function to restore the items being displayed on the screen by outputting
    from the internal memory buffer of the screen to the actual output devices.

    @param pScreen Pointer to the Console Splitter's Simple Text Output protocol
**/
VOID RestoreTextScreen(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *pScreen)
{
	CHAR16 *pChar = ScreenBuffer, *pStr = ScreenBuffer;
	INT32 Col = 0, *pAttr = AttributeBuffer, Attr;
// Supermicro +
#if SMCPKG_SUPPORT
	UINT8 i = 0;		//R1.00+
#endif
// Supermicro -
	CHAR16 c;

	if (BlankScreen)
        return;

	Attr = *pAttr;
	pScreen->SetCursorPosition(pScreen,0,0);
	pScreen->SetAttribute(pScreen,Attr);

	for(; pChar<EndOfTheScreen; pChar++, pAttr++)
	{
		if (Col == Columns || *pAttr != Attr)
		{
			c = *pChar;
			*pChar = 0;
			pScreen->OutputString(pScreen, pStr);
			*pChar = c;
			pStr = pChar;
			if (*pAttr != Attr)
			{
				Attr = *pAttr;
				pScreen->SetAttribute(pScreen, Attr);
			}
			else
			{
				Col=1;
// Supermicro +
#if SMCPKG_SUPPORT
				i++;		//R1.00+
				pScreen->SetCursorPosition(pScreen, 0, i );	//R1.00+
#endif
// Supermicro -
                continue;
			}
		}
		Col++;
	}
	//print last row
	c = *--pChar;
	*pChar = 0;
	pScreen->OutputString(pScreen, pStr);
	*pChar = c;
}

/**
    This function clears internal internal memory buffer that stores the text screen.

    @retval EFI_SUCCESS function executed successfully
**/
EFI_STATUS  MemClearScreen(VOID)
{
	CHAR16  *pChar;
	INT32 *pAttr;
	for(  pChar = ScreenBuffer, pAttr =  AttributeBuffer
		; pChar < EndOfTheScreen
		; pChar++, pAttr++	)
    {
        *pChar = ' ';
        *pAttr = MasterMode.Attribute;
    }
	BlankScreen = TRUE;
	return EFI_SUCCESS;
}

/**
    Function to reset the internal Console Splitter Buffer

    @param This to the Console Splitter's Simple Text Output protocol
    @param EV Extended verification flag

    @retval EFI_SUCCESS function executed successfully
**/
EFI_STATUS MemReset(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	*This,
    IN BOOLEAN                          EV
)
{
	return MemClearScreen();
}

/**
    This function restores the Graphics Screen from the global variable
    PixelBuffer that is crated when the SaveUga function is called.

    @param Gop  Pointer to the Graphics Output Protocol
**/
VOID RestoreUgaScreen(IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop)
{
    EFI_STATUS Status;
    if(PixelBuffer != NULL)
	{
        Status = Gop->SetMode(Gop, GraphicsMode);
        if (!EFI_ERROR(Status))
		    Gop->Blt(   Gop,
                        PixelBuffer,
                        EfiBltBufferToVideo,
                        0,
                        0,
                        0,
                        0,
                        Gop->Mode->Info->HorizontalResolution,
                        Gop->Mode->Info->VerticalResolution,
                        0);
		pBS->FreePool(PixelBuffer);
        PixelBuffer = NULL;
	}
}

/**
    This function uses the passed Gop protocol to dump the current graphics screen
    to the global variable buffer PixelBuffer.

    @param Gop  Pointer to the Graphics Output Protocol
**/
VOID SaveUgaScreen(IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop)
{
    if (PixelBuffer)
        pBS->FreePool(PixelBuffer);

    GraphicsMode=Gop->Mode->Mode;
	PixelBuffer = Malloc(Gop->Mode->Info->VerticalResolution*Gop->Mode->Info->HorizontalResolution*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

	if (PixelBuffer)
    {
		EFI_STATUS Status=Gop->Blt( Gop,
		                            PixelBuffer,
		                            EfiBltVideoToBltBuffer,
		                            0,
		                            0,
		                            0,
		                            0,
                                    Gop->Mode->Info->HorizontalResolution,
                                    Gop->Mode->Info->VerticalResolution,
                                    0);

		if (EFI_ERROR(Status))
        {
            pBS->FreePool(PixelBuffer);
            PixelBuffer = NULL;
        }
	}
}

/**
    Function to check if the output screen has changed since the screen buffer
    was last dumped. If anything has changed, then the output will be updated with the
    changes.

    @param pScreen pScreen Pointer to the Console Splitter's Simple Text Output protocol
**/
VOID RestoreTextScreenDelta(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *pScreen)
{
	UINT32 i;
	CHAR16 StrBuffer[2] = {0, 0};
	CONST UINT32 SizeOfScreen = (UINT32)((UINT8*)EndOfTheScreen-(UINT8*)ScreenBuffer) / sizeof(CHAR16);


	// Check for differences from the old state, one by one
	// If anything is different, update the screen with Simple Out protocol
	for(i = 0; i < SizeOfScreen; i++)
	{
		if( (*(SaveScreenBuffer+i) != *(ScreenBuffer + i)) ||
			(*(SaveAttributeBuffer+i) != *(AttributeBuffer + i)) )
		{
			StrBuffer[0] = *(ScreenBuffer + i);
			pScreen->SetCursorPosition(pScreen, i % Columns, i / Columns);
			pScreen->SetAttribute(pScreen, (UINT8)*(AttributeBuffer + i) );
			pScreen->OutputString(pScreen, StrBuffer );
		}
	}
}

/**
    Function that restores the screen of the output device.

    @param ControllerHandle Handle of the output device to restore
    @param SimpleOut Pointer to the Console Splitter's Simple Text Output protocol
**/
VOID RestoreTheScreen(
    IN EFI_HANDLE                      ControllerHandle,
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *SimpleOut
)
{
    EFI_STATUS Status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;

    Status = pBS->HandleProtocol(ControllerHandle, &gEfiGraphicsOutputProtocolGuid, (VOID**)&Gop);
	if(EFI_ERROR(Status))
		RestoreTextScreen(SimpleOut);
	else
	{
		RestoreUgaScreen(Gop);
		if(SaveScreenBuffer != NULL)
		{
			RestoreTextScreenDelta(SimpleOut);
			pBS->FreePool(SaveScreenBuffer);
			SaveScreenBuffer = NULL;

			pBS->FreePool(SaveAttributeBuffer);
			SaveAttributeBuffer = NULL;
		}
	}
//SMCPKG_SUPPORT	SimpleOut->SetAttribute(SimpleOut,MasterMode.Attribute);
//SMCPKG_SUPPORT	SimpleOut->EnableCursor(SimpleOut,MasterMode.CursorVisible);
//SMCPKG_SUPPORT	SimpleOut->SetCursorPosition(SimpleOut,MasterMode.CursorColumn,MasterMode.CursorRow);
}

/**
    This function saves the screen of the output device associated with ControllerHandle

    @param ControllerHandle Handle of output device to save
    @param SimpleOut Pointer to the Console Splitter's Simple Text Output protocol

    @retval VOID
**/
VOID SaveTheScreen(
    IN EFI_HANDLE                      ControllerHandle,
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *SimpleOut)
{
    EFI_STATUS Status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
	UINT32 SizeOfScreen, SizeOfAttribute;
	EFI_TPL     OldTpl;

    Status = pBS->HandleProtocol(ControllerHandle, &gEfiGraphicsOutputProtocolGuid, (VOID**)&Gop);
	if(!EFI_ERROR(Status))
	{
		SizeOfScreen = (UINT32)((UINT8*)EndOfTheScreen-(UINT8*)ScreenBuffer);
		SizeOfAttribute = (UINT32)SizeOfScreen*(sizeof(INT32)/sizeof(CHAR16));

		// Allocate memory for saving the screen state, error out if not enough memory
        SaveScreenBuffer = Malloc(SizeOfScreen);
        if(SaveScreenBuffer == NULL)
            return;

        SaveAttributeBuffer = Malloc(SizeOfAttribute);
		if(SaveAttributeBuffer == NULL)
		{
			pBS->FreePool(SaveScreenBuffer);
			SaveScreenBuffer = NULL;
            return;
		}

		// Make this high priority so that the saved text screen is exactly
		//  the same as graphics screen
		OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

		// Perform using MemCpy (instead of pBS->MemCopy) to prevent TPL assert
		MemCpy(SaveScreenBuffer, ScreenBuffer, SizeOfScreen);
		MemCpy(SaveAttributeBuffer, AttributeBuffer, SizeOfAttribute);

		pBS->RestoreTPL(OldTpl);
		SaveUgaScreen(Gop);
	}
    return;
}

/**
    This function saves the graphics mode of the output device before switching modes

    @param ControllerHandle handle of output device to save
**/
VOID SaveUgaMode(IN EFI_HANDLE ControllerHandle)
{
    EFI_STATUS Status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;

    Status = pBS->HandleProtocol(ControllerHandle, &gEfiGraphicsOutputProtocolGuid, (VOID**)&Gop);
    if(!EFI_ERROR(Status))
    	TextMode = Gop->Mode->Mode;

    return;
}

/**
    Function to restore the Gop screen the appropiate mode and clear the screen.
    Note: Function assumes that the Gop->SetMode will clear the screen

    @param ControllerHandle handle of output device to restore

    @retval VOID
**/
VOID RestoreUgaMode(IN EFI_HANDLE ControllerHandle)
{
    EFI_STATUS Status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Buffer = {0,0,0,0};

    Status = pBS->HandleProtocol(ControllerHandle, &gEfiGraphicsOutputProtocolGuid, (VOID**)&Gop);
    if(!EFI_ERROR(Status))
    {
    	if(TextMode != Gop->Mode->Mode)
            Gop->SetMode(Gop,TextMode);
        else
        {
            //Just clear the screen
 	        Gop->Blt(   Gop,
 	                    &Buffer,
 	                    EfiBltVideoFill,
 	                    0,
 	                    0,
 	                    0,
 	                    0,
            		    Gop->Mode->Info->HorizontalResolution,
            		    Gop->Mode->Info->VerticalResolution,
            		    0);
        }
    }
    return;
}

/**
    Function that resets all the Simple Text Output devices.  Additionally, this function
    will set the MasterMode attributes to Black Background and White Text and reset the
    cursor positions to 0,0.

    @param This pointer to protocol instance
    @param EV Extended verification flags

    @retval EFI_SUCCESS all devices, handled by splitter were reset successfully
    @retval EFI_DEVICE_ERROR error occured during resetting the device
**/
EFI_STATUS  EFIAPI
CSReset(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN BOOLEAN                         EV
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_STATUS ManagedDeviceStatus;
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	if(SimpleOut == NULL)
	{
        MasterMode.Attribute = EFI_BACKGROUND_BLACK | EFI_WHITE;
        MasterMode.CursorColumn = 0;
        MasterMode.CursorRow = 0;
        MemReset(This, EV);
		return EFI_DEVICE_ERROR;
    }

	// we need to loop through all the registered simple text out devices
	//	and call each of their Reset function
	while( SimpleOut != NULL)
	{
        if(SimpleOut->Active)
        {
		    ManagedDeviceStatus = SimpleOut->SimpleOut->Reset(SimpleOut->SimpleOut, EV);
            if (EFI_ERROR(ManagedDeviceStatus))
                Status = ManagedDeviceStatus;
        }
		SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
	}
	SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	UpdateMasterMode(SimpleOut->SimpleOut->Mode);

	MemReset(This,EV);

	return Status;
}

/**
    Function that writes a unicode string to all the output devices managed by the
    ConsoleSplitter driver.

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param String Pointer to the string to output

    @retval EFI_SUCCESS function executed successfully
    @retval EFI_DEVICE_ERROR error occured during output string
    @retval EFI_WARN_UNKNOWN_GLYPH some of characters were skipped during output
**/
EFI_STATUS EFIAPI
CSOutputString(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN CHAR16                          *String
)
{
	EFI_STATUS	Status = EFI_SUCCESS, TestStatus;
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);
// Supermicro +
    INT32 RowOffset = 0;
    INT32 LastColumn = 0;
// Supermicro -

	MemOutputString(This, String, &RowOffset, &LastColumn);

	if (SimpleOut == NULL) {
        MasterMode.CursorColumn = LastColumn;
        MasterMode.CursorRow += RowOffset;
		return EFI_DEVICE_ERROR;
    }

	// we need to loop through all the registered simple text out devices
	//	and call each of their OutputString function
	while ( SimpleOut != NULL)
	{
	    if(SimpleOut->Active)
        {
		    TestStatus = SimpleOut->SimpleOut->OutputString(SimpleOut->SimpleOut, String);
            if (EFI_ERROR(TestStatus))
			    Status = TestStatus;
        }
        SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
	}

	SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);
	UpdateMasterMode(SimpleOut->SimpleOut->Mode);

	return Status;
}

/**
    Function that determines if all the characters in the string are able to be
    displayed on the output device.

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param String Pointer to the string to test

    @retval EFI_SUCCESS All characters are supposed by the output device
    @retval EFI_UNSUPPORTED There are characters that are not supported by the output device
**/
EFI_STATUS  EFIAPI
CSTestString(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	*This,
	IN CHAR16 *String)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_STATUS ManagedDeviceStatus;
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	if (SimpleOut == NULL)
		return EFI_DEVICE_ERROR;

	// we need to loop through all the registered simple text out devices
	//	and call each of their TestString function
	while ( SimpleOut != NULL)
	{
    	if(SimpleOut->Active)
        {
		    ManagedDeviceStatus = SimpleOut->SimpleOut->TestString(SimpleOut->SimpleOut, String);
		    if (EFI_ERROR(ManagedDeviceStatus))
			    Status = ManagedDeviceStatus;
        }
        SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
	}

	SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	UpdateMasterMode(SimpleOut->SimpleOut->Mode);
	return Status;
}

/**
    This function returns information about the text mode specified in ModeNum.

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param ModeNum Mode number to obtain information about
    @param Col Number of columns supported by this mode
    @param Row Number of rows supported by this mode

    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_UNSUPPORTED Given mode unsupported
    @retval EFI_DEVICE_ERROR Error occured during execution
**/
EFI_STATUS EFIAPI
CSQueryMode(
	IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL	*This,
	IN  UINTN                           ModeNum,
	OUT UINTN                           *Col,
	OUT UINTN                           *Row
)
{
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	if(ModeNum >= (UINTN)MasterMode.MaxMode)
        return EFI_UNSUPPORTED;

    #if defined(FORCE_COMMON_MODE_FOR_DEVICES)&&(FORCE_COMMON_MODE_FOR_DEVICES==1)
    if(!SupportedModes[ModeNum].AllDevices)
        return EFI_UNSUPPORTED;
    #endif

	if(SimpleOut == NULL)
	{
		// Since we use a default text mode, return that value
		*Col = 80;
		*Row = 25;
		return EFI_DEVICE_ERROR;
	}

	*Col = SupportedModes[ModeNum].Columns;
	*Row = SupportedModes[ModeNum].Rows;

	return EFI_SUCCESS;
}

/**
    This function sets text mode referred by ModeNumber to all the output devices.
    Since actual output devices reported modes vary from device to device, this
    function will attempt to map the Console Splitter's mode to the equivalent device
    mode by matching the columns and rows to the individual devices.

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param ModeNum Mode numer of the Console Splitter to map to each device

    @retval EFI_SUCCESS function executed successfully
    @retval EFI_UNSUPPORTED given mode unsupported
    @retval EFI_DEVICE_ERROR error occured during execution
**/
EFI_STATUS EFIAPI
CSSetMode(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN                           ModeNum
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_STATUS ManagedDeviceStatus;
	INT32 DeviceMode;

	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	if((ModeNum >= (UINTN)MasterMode.MaxMode))
		return EFI_UNSUPPORTED;

    #if defined(FORCE_COMMON_MODE_FOR_DEVICES)&&(FORCE_COMMON_MODE_FOR_DEVICES==1)
    if(!SupportedModes[ModeNum].AllDevices)
        return EFI_UNSUPPORTED;
    #endif
	if (SimpleOut == NULL)
		return EFI_DEVICE_ERROR;

	Status = ResizeSplitterBuffer((INT32)ModeNum);
	if(EFI_ERROR(Status))
		return Status;

	MasterMode.Mode = (INT32)ModeNum;
	// we need to loop through all the registered simple text out devices
	//	and call each of their SetMode function
	while ( SimpleOut != NULL)
	{
		ManagedDeviceStatus = IsModeSupported(SimpleOut->SimpleOut, ModeNum, &DeviceMode);
		if(EFI_ERROR(ManagedDeviceStatus)) {
		    SimpleOut->Active = FALSE;
        } else {
    		ManagedDeviceStatus = SimpleOut->SimpleOut->SetMode(SimpleOut->SimpleOut, DeviceMode);
            if(EFI_ERROR(ManagedDeviceStatus)) {
                Status = ManagedDeviceStatus;
                SimpleOut->Active = FALSE;
            } else {
                SimpleOut->Active = TRUE;
            }

        }
        SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);

	}

	SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	UpdateMasterMode(SimpleOut->SimpleOut->Mode);
	UpdateAbsolutePointerInformation();
	return Status;
}

/**
    Function to set the foreground and background color of the console device

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param Attribute Attributes to set

    @retval EFI_SUCCESS attribute was changed successfully
    @retval EFI_DEVICE_ERROR device had an error
    @retval EFI_UNSUPPORTED attribute is not supported
**/
EFI_STATUS  EFIAPI
CSSetAttribute(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN                           Attribute
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_STATUS TestStatus;
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

    MasterMode.Attribute = (INT32)Attribute;
	if (SimpleOut == NULL)
		return EFI_DEVICE_ERROR;

	// we need to loop through all the registered simple text out devices
	//	and call each of their SetAttribute function
	while ( SimpleOut != NULL)
	{
        if(SimpleOut->Active)
        {
		    TestStatus = SimpleOut->SimpleOut->SetAttribute(SimpleOut->SimpleOut, Attribute);
		    if (EFI_ERROR(TestStatus))
			    Status = TestStatus;
        }
        SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
	}

	SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	UpdateMasterMode(SimpleOut->SimpleOut->Mode);

	return Status;
}

/**
    This function clears screen of output device

    @param This Pointer to the Console Splitter's Simple Text Output protocol

    @retval EFI_SUCCESS function executed successfully
    @retval EFI_DEVICE_ERROR error occured during execution
**/
EFI_STATUS  EFIAPI
CSClearScreen(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This)
{
	EFI_STATUS	Status = EFI_SUCCESS, TestStatus;
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	if (SimpleOut == NULL) {
        MasterMode.CursorColumn = 0;
        MasterMode.CursorRow = 0;
        MemClearScreen();
		return EFI_DEVICE_ERROR;
    }

	// we need to loop through all the registered simple text out devices
	//	and call each of their ClearScreen function
	while ( SimpleOut != NULL)
	{
        if(SimpleOut->Active)
        {
            TestStatus = SimpleOut->SimpleOut->ClearScreen(SimpleOut->SimpleOut);
            if (EFI_ERROR(TestStatus))
			    Status = TestStatus;
        }
        SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
	}

	SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	UpdateMasterMode(SimpleOut->SimpleOut->Mode);

	MemClearScreen();
	return Status;
}

/**
    Function that sets the Cursor Position for all the managed devices to the
    passed parameters.

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param Column Column position to set.
    @param Row Row position to set

    @retval EFI_SUCCESS function executed successfully
    @retval EFI_DEVICE_ERROR error occured during execution
    @retval EFI_UNSUPPORTED given position cannot be set
**/
EFI_STATUS EFIAPI
CSSetCursorPosition(
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
	IN UINTN                           Column,
	IN UINTN                           Row
)
{
	EFI_STATUS	Status = EFI_SUCCESS, TestStatus;
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	if(SimpleOut == NULL) {
        if(Column < 80 && Row < 25) {
            MasterMode.CursorColumn = (INT32)Column;
            MasterMode.CursorRow = (INT32)Row;
        }
		return EFI_DEVICE_ERROR;
    }

	// we need to loop through all the registered simple text out devices
	//	and call each of their SetCursorPosition function
	while(SimpleOut != NULL)
	{
        if(SimpleOut->Active)
        {
            TestStatus = SimpleOut->SimpleOut->SetCursorPosition(SimpleOut->SimpleOut, Column, Row);
            if (EFI_ERROR(TestStatus))
                Status = TestStatus;
        }
        SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
	}

	SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	UpdateMasterMode(SimpleOut->SimpleOut->Mode);

	return Status;
}

/**
    Function that enables or disables the cursor on each of the devices
    managed by the ConsoleSplitter driver.

    @param This Pointer to the Console Splitter's Simple Text Output protocol
    @param Visible If TRUE cursor will be visible. If FALSE the cursor will not be visible

    @retval EFI_SUCCESS function executed successfully
    @retval EFI_DEVICE_ERROR error occured during execution
**/
EFI_STATUS  EFIAPI
CSEnableCursor(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, IN BOOLEAN Visible	)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_STATUS TestStatus;
	CON_SPLIT_OUT *SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

	if (SimpleOut == NULL)
		return EFI_DEVICE_ERROR;

	// we need to loop through all the registered simple text out devices
	//	and call each of their SetCursorPosition function
	while ( SimpleOut != NULL)
	{
        if(SimpleOut->Active)
        {
		    TestStatus = SimpleOut->SimpleOut->EnableCursor(SimpleOut->SimpleOut, Visible);
            if (EFI_ERROR(TestStatus))
                Status = TestStatus;
        }
		SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
	}

    MasterMode.CursorVisible = Visible;
	return Status;
}

/**
    Function to update the master mode values to match the ConsoleSplitter's values.

    @param Mode Pointer to mode values that should be used to update the master mode
**/
VOID UpdateMasterMode(SIMPLE_TEXT_OUTPUT_MODE *Mode)
{
	MasterMode.Attribute = Mode->Attribute;
	MasterMode.CursorColumn = Mode->CursorColumn;
	MasterMode.CursorRow = Mode->CursorRow;

    while(MasterMode.CursorColumn >= SupportedModes[MasterMode.Mode].Columns)
    {
        MasterMode.CursorColumn -= SupportedModes[MasterMode.Mode].Columns;
        MasterMode.CursorRow++;
    }
    if (MasterMode.CursorRow>=SupportedModes[MasterMode.Mode].Rows)
        MasterMode.CursorRow=SupportedModes[MasterMode.Mode].Rows - 1;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
