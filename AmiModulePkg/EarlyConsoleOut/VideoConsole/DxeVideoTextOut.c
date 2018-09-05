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

/** @file DxeVideoTextOut.c
    This file contains the Protocol functions for video text out.

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AmiDxeTextOut.h>
#include <AmiVideoTextOutLib.h>
#include <AmiTextOutHob.h>
#include <Protocol/GraphicsOutput.h>

extern EFI_STATUS 
AmiVideoControllerReset(
    VOID
);

BOOLEAN             VideoDeviceInUse=FALSE;

typedef struct {
    AMI_DXE_TEXT_OUT            Protocol;
    UINT8                       UpperCursorRow;
    UINT8                       UpperCursorCol;
    UINT8                       LowerCursorRow;
    UINT8                       LowerCursorCol;
    BOOLEAN                     NextLine; 
} AMI_VIDEO_TEXT_OUT_PRIVATE_DATA;

/**
    Clears the output device(s) display to the currently selected background 
    color.
      
    @param   This   The Protocol instance pointer.

    @retval  EFI_SUCCESS      The operation completed successfully.
    @retval  EFI_DEVICE_ERROR The device had an error and could not complete the request.
    @retval  EFI_UNSUPPORTED  The output device is not in a valid text mode.

**/

EFI_STATUS
EFIAPI
VideoClearScreen(
    IN AMI_DXE_TEXT_OUT    *This
)
{
    if(VideoDeviceInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }
    
    return AmiVideoClear();
}

/**
    Returns information for an available text mode that the output device(s)
    supports.

    @param  This       The Protocol instance pointer.
    @param  ModeNumber The mode number to return information on.
    @param  Columns    Returns the geometry of the text output device for the
                     requested ModeNumber.
    @param  Rows       Returns the geometry of the text output device for the
                     requested ModeNumber.
                                          
    @retval EFI_SUCCESS      The requested mode information was returned.
    @retval EFI_DEVICE_ERROR The device had an error and could not complete the request.
    @retval EFI_UNSUPPORTED  The mode number was not valid.

**/
EFI_STATUS
EFIAPI
VideoQueryMode(
    IN AMI_DXE_TEXT_OUT    *This,
    IN UINT8                Mode,
    IN OUT UINT8            *Col,
    IN OUT UINT8            *Row
)
{
    return EFI_UNSUPPORTED;
}


/**
    Sets the current coordinates of the cursor position

    @param  This        The Protocol instance pointer.
    @param  Column      The position to set the cursor to. Must be greater than or
                        equal to zero and less than the number of columns and rows
                        by QueryMode ().
    @param  Row         The position to set the cursor to. Must be greater than or
                        equal to zero and less than the number of columns and rows
                        by QueryMode ().

    @retval EFI_SUCCESS      The operation completed successfully.
    @retval EFI_DEVICE_ERROR The device had an error and could not complete the request.
    @retval EFI_UNSUPPORTED  The output device is not in a valid text mode, or the
                             cursor position is invalid for the current mode.

**/

EFI_STATUS
EFIAPI
VideoSetCursor(
    IN AMI_DXE_TEXT_OUT    *This, 
    IN UINT8                Col,
    IN UINT8                Row
)
{
    if ( Col >= MAX_COLS || Row >= MAX_ROWS) {
        return EFI_INVALID_PARAMETER;
    }
 
    if(VideoDeviceInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }
    
    return AmiVideoGotoXY(Col, Row);
}

/**
    Write a string to the output device.

    @param  This   The Protocol instance pointer.
    @param  Panel  Panel number of the display device 
    @param  Text   The NULL-terminated string to be displayed on the output
                   device(s). All output devices must also support the Unicode
                   drawing character codes defined in this file.

    @retval EFI_SUCCESS             The string was output to the device.
    @retval EFI_DEVICE_ERROR        The device reported an error while attempting to output
                                  the text.
    @retval EFI_UNSUPPORTED         The output device's mode is not currently in a
                                  defined text mode.
**/

EFI_STATUS
EFIAPI
VideoWriteText(
    IN AMI_DXE_TEXT_OUT    *This,
    IN UINT8               Side,
    IN UINT8               *Text
)
{
    EFI_STATUS Status;
    AMI_VIDEO_TEXT_OUT_PRIVATE_DATA    *VideoPrivate = (AMI_VIDEO_TEXT_OUT_PRIVATE_DATA *)This; 
    UINT8                           Row;
    UINT8                           Col;
    
    if (Side > 1) {
        return EFI_INVALID_PARAMETER;
    }

    if(VideoDeviceInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }

    // Set the cursor position according to the Side.
    if (Side == 0) { 
        VideoSetCursor (This, VideoPrivate->UpperCursorCol, VideoPrivate->UpperCursorRow );
    } else if (Side == 1) { 
        VideoSetCursor (This, VideoPrivate->LowerCursorCol, VideoPrivate->LowerCursorRow);
    }
    
    Status = AmiVideoPrint(Side, Text);
    
    // Get the new Cursor Position and Save it in the HOB
    AmiVideoGetCursorPosition(&Col, &Row);
    
    if (Side == 0) { 
        VideoPrivate->UpperCursorRow = Row;
        VideoPrivate->UpperCursorCol = Col;
    } else if (Side == 1) {
        VideoPrivate->LowerCursorRow = Row;
        VideoPrivate->LowerCursorCol = Col;
    }
    
    return Status;
}

/**
    Sets the background and foreground colors for the OutputString () and
    ClearScreen () functions.

    @param  This        The Protocol instance pointer.
    @param  Foreground  Foreground color
    @param  Background  Background color
    @param  Blink       Blink text

    @retval EFI_SUCCESS       The attribute was set.
    @retval EFI_DEVICE_ERROR  The device had an error and could not complete the request.
    @retval EFI_UNSUPPORTED   The attribute requested is not defined.

**/

EFI_STATUS
EFIAPI
VideoSetAttribute(
    IN AMI_DXE_TEXT_OUT    *This, 
    IN UINT8               Foreground,
    IN UINT8               Background,
    IN BOOLEAN             Blink
)
{
    UINT8   TextAttribute;

    if(VideoDeviceInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }
    //    Bits 0..3 are the foreground color, and
    //    bits 4..6 are the background color. All other bits are undefined
    //    and must be zero.
    Foreground = Foreground & FG_COLOR_MASK;
    Background = Background & BG_COLOR_MASK;

    TextAttribute = ((Foreground) | (Background << 4) | (Blink << 7));

    return AmiVideoSetColor(TextAttribute);
}

/**
    Makes the cursor visible or invisible

    @param  This    The Protocol instance pointer.
    @param  Enable  If TRUE, the cursor is set to be visible. If FALSE, the cursor is
                    set to be invisible.

    @retval EFI_SUCCESS      The operation completed successfully.
    @retval EFI_DEVICE_ERROR The device had an error and could not complete the
                             request, or the device does not support changing
                             the cursor mode.
    @retval EFI_UNSUPPORTED  The output device is not in a valid text mode.

**/
EFI_STATUS
EFIAPI
VideoEnableCursor(
    IN AMI_DXE_TEXT_OUT    *This,       
    IN BOOLEAN             Enable
)
{
    return EFI_UNSUPPORTED;
}


/**
    Gets Early Video HOB and transfers video library to DXE protocol

    @param ImageHandle 
    @param SystemTable 

    @retval 
        EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

**/
EFI_STATUS
EFIAPI
VideoTextOutDxeEntryPoint (
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS          Status;
    EFI_HANDLE          TextOutHandle= NULL;
    AMI_VIDEO_TEXT_OUT_PRIVATE_DATA *VideoTextOutPrivate = NULL;
    EFI_GUID            HobListGuid = HOB_LIST_GUID;
    EFI_GUID            AmiVideoConsoleHobGuid = AMI_TEXT_OUT_HOB_GUID;
    AMI_TEXT_OUT_HOB    *AmiVideoConsoleHob = NULL;


    InitAmiLib( ImageHandle, SystemTable );

    // Get the Video Hob Produced in the PEI
    AmiVideoConsoleHob = (AMI_TEXT_OUT_HOB*)GetEfiConfigurationTable(SystemTable,&HobListGuid);
    if ( AmiVideoConsoleHob == NULL ){
        return EFI_NOT_FOUND;
    }

    Status = FindNextHobByGuid( &AmiVideoConsoleHobGuid, (VOID**)&AmiVideoConsoleHob);
    if (EFI_ERROR( Status )){
        return Status;
    }
    
    VideoTextOutPrivate = AllocateZeroPool (sizeof (AMI_DXE_TEXT_OUT));
    
    if (VideoTextOutPrivate == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }
    
    // Initialize the structure from the HOB data
    VideoTextOutPrivate->UpperCursorRow	= AmiVideoConsoleHob->UpperCursorRow;
    VideoTextOutPrivate->UpperCursorCol = AmiVideoConsoleHob->UpperCursorCol;
    VideoTextOutPrivate->LowerCursorRow	= AmiVideoConsoleHob->LowerCursorRow;
    VideoTextOutPrivate->LowerCursorCol = AmiVideoConsoleHob->LowerCursorCol;
    VideoTextOutPrivate->NextLine = AmiVideoConsoleHob->NextLine;

    // Initialize the Protocol API's
    VideoTextOutPrivate->Protocol.ClearScreen = VideoClearScreen;
    VideoTextOutPrivate->Protocol.WriteString = VideoWriteText;
    VideoTextOutPrivate->Protocol.SetCursorPosition = VideoSetCursor;
    VideoTextOutPrivate->Protocol.SetAttribute = VideoSetAttribute;
    VideoTextOutPrivate->Protocol.QueryMode = VideoQueryMode;
    VideoTextOutPrivate->Protocol.EnableCursor = VideoEnableCursor;
    
    // Install the Protocol for the Text Out
    Status = pBS->InstallMultipleProtocolInterfaces ( &TextOutHandle,
                                                      &gAmiDxeTextOutProtocolGuid,
                                                      &VideoTextOutPrivate->Protocol,
                                                      NULL
                                                     );
   
    
    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    // Porting function for OEM to install the callback and in the callback function
    // Reset the Video Controller programming. 
    
    AmiVideoControllerReset();
    
    return Status;
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
