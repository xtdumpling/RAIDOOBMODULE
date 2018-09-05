//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiVideoTextOut.c
    This PEIM initializes VGA device, produces PPI for Video Display 
    and creates HOB with the Video device information.

**/

#include "PeiVideoTextOut.h"

extern EFI_STATUS AmiVideoInit(
    IN  EFI_PEI_SERVICES  **PeiServices
);

EFI_GUID    gVideoTextOutHobGuid = AMI_TEXT_OUT_HOB_GUID;

typedef struct {
    AMI_PEI_TEXT_OUT            Ppi;
    UINT8                       UpperCursorRow;
    UINT8                       LowerCursorRow;
    BOOLEAN                     NextLine; 
} AMI_VIDEO_TEXT_OUT_PRIVATE_DATA;

/**
    Outputs system information to Video

    @param VideoTextOutPpi - PPI instance pointer.

    @retval EFI_STATUS
**/

EFI_STATUS
OutputBiosVersion (
    IN AMI_PEI_TEXT_OUT     *VideoTextOutPpi
)
{   

    VideoTextOutPpi->SetCursorPosition (VideoTextOutPpi, 0, 0);
    VideoTextOutPpi->SetAttribute (VideoTextOutPpi, POST_MSG_FOREGROUND, POST_MSG_BACKGROUND, 0);
    VideoTextOutPpi->WriteString (VideoTextOutPpi, 0, "Copyright(c) 2015 American Megatrends, Inc. \n \r");

    return EFI_SUCCESS;
}

/**
    Clears the Video device display to the currently selected background 
    color.
      
    @param   This   - PPI instance pointer.

    @retval  EFI_SUCCESS      The operation completed successfully.
**/

EFI_STATUS
EFIAPI
VideoConClearScreen (
    IN AMI_PEI_TEXT_OUT    *This
)
{
    return AmiVideoClear();
}

/**
    Returns information for an available text mode that the output device(s)
    supports.

    @param  This   -   PPI instance pointer.
    @param  Mode   -   Mode number to return information on.
    @param  Col    -   Returns Max Columns for the requested ModeNumber.
    @param  Row    -   Returns Max Rows for the requested ModeNumber.
                                          
    @retval EFI_SUCCESS      The requested mode information was returned.
    @retval EFI_UNSUPPORTED  The mode number was not valid.

**/
EFI_STATUS
EFIAPI
VideoConQueryMode (
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Mode,
    IN OUT UINT8           *Col,
    IN OUT UINT8           *Row
)
{
    return EFI_UNSUPPORTED;
}

/**
    Write a string to the output device.

    @param  This   -   PPI instance pointer.
    @param  Panel  -   Panel number of the display device
                        0 - Top
                        1 - Bottom
    @param  Text   -   NULL-terminated string display

    @retval EFI_SUCCESS           The string was output to the device.
    @retval EFI_INVALID_PARAMETER Panel number is invalid.
    @retval EFI_NOT_FOUND         HOB not Found.
**/

EFI_STATUS
EFIAPI
VideoConWriteString (
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Side,
    IN UINT8               *Text
)    
{
    AMI_VIDEO_TEXT_OUT_PRIVATE_DATA *VideoTextOutPrivate = (AMI_VIDEO_TEXT_OUT_PRIVATE_DATA *)This; 
    AMI_TEXT_OUT_HOB                *VideoConsoleHob = NULL;
    UINT32                          Status;
    UINT8                           Row;
    UINT8                           Col;
 
    if (Side > 1) {
        return EFI_INVALID_PARAMETER;
    }

    VideoConsoleHob = GetHobList();
    if (VideoConsoleHob == NULL) {
        return EFI_NOT_FOUND;
    }

    VideoConsoleHob = GetNextGuidHob (&gVideoTextOutHobGuid, VideoConsoleHob);
    if (VideoConsoleHob == NULL) {
        return EFI_NOT_FOUND;
    }

    if (Side == 0) { // Top
        VideoConSetCursorPosition (This, VideoConsoleHob->UpperCursorCol, VideoConsoleHob->UpperCursorRow );
    } else if (Side == 1) { // Bottom
        VideoConSetCursorPosition (This, VideoConsoleHob->LowerCursorCol, VideoConsoleHob->LowerCursorRow);
    }

    Status = AmiVideoPrint(Side, Text);
    
    if(EFI_ERROR(Status)) {
        return Status;
    }

    // Get the new Cursor Position and Save it in the HOB
    AmiVideoGetCursorPosition(&Col, &Row);
    if (Side == 0) {
        VideoConsoleHob->UpperCursorRow = Row;
        VideoConsoleHob->UpperCursorCol = Col;
    } else if (Side == 1) {
        VideoConsoleHob->LowerCursorRow = Row;
        VideoConsoleHob->LowerCursorCol = Col;
    }
    
    return Status;
}

/**
    Sets the current coordinates of the cursor position

    @param  This        - PPI instance pointer.
    @param  Column      - Position column number. Must be greater than or
                          equal to zero and less than the number of columns 
                          and rows by QueryMode ().
    @param  Row         - Position Row number. Must be greater than or
                          equal to zero and less than the number of columns 
                          and rows by QueryMode ().

    @retval EFI_SUCCESS             The operation completed successfully.
    @retval EFI_INVALID_PARAMETER   If Row or Col exceeds Max Row or Column.
**/

EFI_STATUS
EFIAPI
VideoConSetCursorPosition (
    IN AMI_PEI_TEXT_OUT    *This, 
    IN UINT8                Col,
    IN UINT8                Row
)
{
    if ( Col >= MAX_COLS || Row >= MAX_ROWS) {
        return EFI_INVALID_PARAMETER;
    }
    return AmiVideoGotoXY(Col, Row);
}

/**
    Sets the background and foreground colors for WriteString()

    @param  This        PPI instance pointer.
    @param  Foreground  Foreground color
    @param  Background  Background color
    @param  Blink       Blink text

    @retval EFI_SUCCESS       The attribute was set.

**/

EFI_STATUS
EFIAPI
VideoConSetAttribute (
    IN AMI_PEI_TEXT_OUT    *This, 
    IN UINT8               Foreground,
    IN UINT8               Background,
    IN BOOLEAN             Blink
)
{
    UINT8   TextAttribute;

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
VideoConEnableCursor (
    IN AMI_PEI_TEXT_OUT    *This,
    IN BOOLEAN              Enable
)
{
    return EFI_UNSUPPORTED;
}

/**
    Entry point for PEI Video Driver

    @param FileHandle  - Pointer to image file handle
    @param PeiServices - Pointer to the PEI Core data Structure

    @retval EFI_STATUS
**/

EFI_STATUS
EFIAPI
PeiVideoTextOutEntryPoint (
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
)
{
    AMI_VIDEO_TEXT_OUT_PRIVATE_DATA  *VideoTextOutPrivate;
    EFI_PEI_PPI_DESCRIPTOR           *VideoTextOutPpi;
    UINT16                           HobSize = sizeof(AMI_TEXT_OUT_HOB);
    AMI_TEXT_OUT_HOB                 *VideoConsoleHob;
    UINT32                           VideoStatus = EFI_NOT_FOUND;
    EFI_STATUS                       Status;

    Status = (*PeiServices)->AllocatePool( PeiServices,
                                           sizeof(AMI_VIDEO_TEXT_OUT_PRIVATE_DATA),
                                           &VideoTextOutPrivate
                                           );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }

    // Call the Video Controller Initialization 
    VideoStatus = AmiVideoInit( PeiServices); 

    if (VideoStatus != EFI_SUCCESS) {
        return EFI_NOT_FOUND;
    }    
    

    Status = (*PeiServices)->AllocatePool( PeiServices,
                                               sizeof(EFI_PEI_PPI_DESCRIPTOR),
                                               &VideoTextOutPpi
                                               );
    if (EFI_ERROR(Status)) { 
        return Status;
    }
        
    // Create HOB for the Video Data
    Status = (*PeiServices)->CreateHob( PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        HobSize,
                                        &VideoConsoleHob
                                        );
    
    // HOB Initilize
    VideoConsoleHob->Header.Name      = gVideoTextOutHobGuid;
    VideoConsoleHob->UpperCursorRow   = VideoTextOutPrivate->UpperCursorRow 
                                      = FIRST_SCREEN_START;
    VideoConsoleHob->UpperCursorCol   = 0; 
    VideoConsoleHob->LowerCursorRow   = VideoTextOutPrivate->LowerCursorRow 
                                      = SECOND_SCREEN_START;
    VideoConsoleHob->LowerCursorCol   = 0; 
    VideoConsoleHob->NextLine         = VideoTextOutPrivate->NextLine       
                                      = FALSE;
    
    // Produce the TextOut PPI for the Video Device
    VideoTextOutPpi->Flags            = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    VideoTextOutPpi->Guid             = &gAmiPeiTextOutPpiGuid;
    VideoTextOutPpi->Ppi              = &VideoTextOutPrivate->Ppi;
    
    VideoTextOutPrivate->Ppi.ClearScreen        = VideoConClearScreen;
    VideoTextOutPrivate->Ppi.QueryMode          = VideoConQueryMode;
    VideoTextOutPrivate->Ppi.WriteString        = VideoConWriteString;
    VideoTextOutPrivate->Ppi.SetCursorPosition  = VideoConSetCursorPosition;
    VideoTextOutPrivate->Ppi.SetAttribute       = VideoConSetAttribute;
    VideoTextOutPrivate->Ppi.EnableCursor       = VideoConEnableCursor;

    Status = (*PeiServices)->InstallPpi(PeiServices, VideoTextOutPpi);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    // Clear the Screen
    AmiVideoClear();

    // Set the Sign on Message attribute
    VideoConSetAttribute (&VideoTextOutPrivate->Ppi, 
                            VIDEO_COLOR_WHITE | VIDEO_INTENSITY, 
                            VIDEO_COLOR_BLACK, 
                            FALSE
                            );
    
    VideoConSetCursorPosition (&VideoTextOutPrivate->Ppi, 0, ROWS_SPLIT);
    
    //Display the Screen divider 
    AmiVideoPrint ( 1, "-----------------------------------------------------------------------\n\r");
    AmiVideoPrint ( 1, "System Boot Status\n");
    
    VideoConSetAttribute (&VideoTextOutPrivate->Ppi, 
                            VIDEO_COLOR_WHITE, 
                            VIDEO_COLOR_BLACK, 
                            FALSE
                            );

    
    // Display BIOS information
    OutputBiosVersion(&VideoTextOutPrivate->Ppi);

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
