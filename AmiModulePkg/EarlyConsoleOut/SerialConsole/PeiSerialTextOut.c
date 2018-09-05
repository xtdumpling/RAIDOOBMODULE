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

/**
  @file  PeiSerialTextOut.c
  This file contains the PPI functions to use serial post.
*/

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SerialPortLib.h>
#include <Ppi/AmiPeiTextOut.h>
#include <AmiSerialTextOutLib.h>
#include <AmiProgressErrorCodeLib.h>

/**
    Clears the output device(s) display to the currently selected background 
    color.
      
    @param   This   The PPI instance pointer.

    @retval  EFI_SUCCESS      The operation completed successfully.
    @retval  EFI_DEVICE_ERROR The device had an error and could not complete the request.
    @retval  EFI_UNSUPPORTED  The output device is not in a valid text mode.

**/
EFI_STATUS
EFIAPI
SerialClearScreen(
  IN AMI_PEI_TEXT_OUT *This
)
{
    EFI_STATUS Status; 

    Status = TerminalClearScreen();

    return Status; 
}

/**
    Returns information for an available text mode that the output device(s)
    supports.

    @param  This       The PPI instance pointer.
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
SerialQueryMode(
    IN AMI_PEI_TEXT_OUT     *This,
    IN UINT8                Mode,
    IN OUT UINT8            *Col,
    IN OUT UINT8            *Rows
)
{
    switch(Mode) {
    case 0:
        //Mode 0 is the only valid mode
        *Col = 80;
        *Rows = 25;
        break;
    default:
        *Col = 0;
        *Rows = 0;
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

/**
    Write a string to the output device.

    @param  This   The PPI instance pointer.
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
SerialWriteString(
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Panel,
    IN UINT8               *Text
)
{
    EFI_STATUS Status;

    Status = TerminalOutputStringHelper(Text);
    return Status;
}


/**
    Sets the current coordinates of the cursor position

    @param  This        The PPI instance pointer.
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
SerialSetCursorPosition(
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Column,
    IN UINT8               Row 
)
{
    EFI_STATUS Status; 

    if (Column >= MAX_COLUMNS || Row >= MAX_ROWS) {
        return EFI_UNSUPPORTED;
    }

    // Call the SerialTextOut LIB to set the cursor position 
    Status = TerminalSetCursorPosition (Column, Row);

    return Status; 
}

/**
    Sets the background and foreground colors for the OutputString () and
    ClearScreen () functions.

    @param  This        The PPI instance pointer.
    @param  Foreground  Foreground color
    @param  Background  Background color
    @param  Blink       Blink text

    @retval EFI_SUCCESS       The attribute was set.
    @retval EFI_DEVICE_ERROR  The device had an error and could not complete the request.
    @retval EFI_UNSUPPORTED   The attribute requested is not defined.

**/
EFI_STATUS 
EFIAPI
SerialSetAttribute(
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Foreground,
    IN UINT8               Background,
    IN BOOLEAN             Blink
)
{

    EFI_STATUS Status;
    UINT8      TextAttribute;
    //    Bits 0..3 are the foreground color, and
    //    bits 4..6 are the background color. All other bits are undefined
    //    and must be zero.
    Foreground = Foreground & 0x0F;
    Background = Background & 0x07;

    TextAttribute = ((Foreground) | (Background << 4));

    // Call the SerialTextOut LIB to set the attribute
    Status = TerminalSetAttribute(TextAttribute);

    return Status;
}

/**
    Makes the cursor visible or invisible

    @param  This    The PPI instance pointer.
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
SerialEnableCursor(
    IN AMI_PEI_TEXT_OUT     *This,
    IN BOOLEAN              Enable
)
{
    return EFI_UNSUPPORTED;
}

/**
    This function is the entry point for this PEI.
    This installs the Serial TextOut PPI

    @param FileHandle Pointer to image file handle.
    @param PeiServices Pointer to the PEI Core data Structure

    @return EFI_STATUS
    @retval EFI_SUCCESS Successful driver initialization

**/
EFI_STATUS
EFIAPI
PeiSerialTextOutEntry (
  IN       EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
)
{
    EFI_STATUS                  Status;
    EFI_PEI_PPI_DESCRIPTOR      *SerialTextOutDescriptor;
    AMI_PEI_TEXT_OUT            *SerialTextOutPpi = NULL;
    EFI_HANDLE                  Handle = NULL;

    SerialPortInitialize();

    Status = (*PeiServices)->AllocatePool(
                    PeiServices,
                    sizeof(EFI_PEI_PPI_DESCRIPTOR),
                    &SerialTextOutDescriptor);

    if (EFI_ERROR(Status)) { 
        return Status;
    }

    Status = (*PeiServices)->AllocatePool(
                    PeiServices,
                    sizeof(AMI_PEI_TEXT_OUT),
                    &SerialTextOutPpi);

    if (EFI_ERROR(Status)){ 
        return Status;
    }

    SerialTextOutDescriptor->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    SerialTextOutDescriptor->Guid = &gAmiPeiTextOutPpiGuid;
    SerialTextOutDescriptor->Ppi = SerialTextOutPpi;

    SerialTextOutPpi->ClearScreen = SerialClearScreen;
    SerialTextOutPpi->QueryMode = SerialQueryMode;
    SerialTextOutPpi->WriteString = SerialWriteString;
    SerialTextOutPpi->SetCursorPosition = SerialSetCursorPosition;
    SerialTextOutPpi->SetAttribute = SerialSetAttribute;
    SerialTextOutPpi->EnableCursor = SerialEnableCursor;
    
    Status = (*PeiServices)->InstallPpi(PeiServices, SerialTextOutDescriptor);

    // Clear the Screen
    SerialTextOutPpi->ClearScreen (SerialTextOutPpi);
    SerialSetCursorPosition(SerialTextOutPpi, 0, 0);
    
    // Set the Original Attribute
    SerialTextOutPpi->SetAttribute (SerialTextOutPpi, POST_MSG_FOREGROUND, POST_MSG_BACKGROUND,DISPLAY_BLINK);
    Status = SerialTextOutPpi->WriteString(SerialTextOutPpi, 0, "Copyright(c) 2015 American Megatrends, Inc. \n \r"); 
    
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
