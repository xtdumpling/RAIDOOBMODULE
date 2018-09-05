//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiPeiTextOut.h
    AMI defined PPI header file for the Text Out PPI
 **/

#ifndef __AMI_PEI_TEXT_OUT_PPI__H__
#define __AMI_PEI_TEXT_OUT_PPI__H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AMI_PEI_TEXT_OUT AMI_PEI_TEXT_OUT;

/**
    Clears the output device(s) display to the currently selected background 
    color.
      
    @param   This   The PPI instance pointer.

    @retval  EFI_SUCCESS      The operation completed successfully.
    @retval  EFI_DEVICE_ERROR The device had an error and could not complete the request.
    @retval  EFI_UNSUPPORTED  The output device is not in a valid text mode.

**/
typedef 
EFI_STATUS (EFIAPI *AMI_PEI_CLEAR_SCREEN)(
   IN AMI_PEI_TEXT_OUT  *This
);

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
typedef 
EFI_STATUS (EFIAPI *AMI_PEI_QUERY_MODE)(
    IN AMI_PEI_TEXT_OUT     *This,
    IN UINT8                Mode,
    IN OUT UINT8            *Col,
    IN OUT UINT8            *Rows
);

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
typedef 
EFI_STATUS (EFIAPI *AMI_PEI_WRITE_STRING)(
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Panel,
    IN UINT8               *Text
);

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
typedef 
EFI_STATUS (EFIAPI *AMI_PEI_SET_CURSOR_POSITION)(
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Col,
    IN UINT8               Row
);

/**
    Sets the background and foreground colors for the OutputString () and
    ClearScreen () functions.

    @param  This       The PPI instance pointer.
    @param  Foreground Foreground color
    @param  Background Background color
    @param  Blink      Blink text

    @retval EFI_SUCCESS       The attribute was set.
    @retval EFI_DEVICE_ERROR  The device had an error and could not complete the request.
    @retval EFI_UNSUPPORTED   The attribute requested is not defined.

**/
typedef 
EFI_STATUS (EFIAPI *AMI_PEI_SET_ATTRIBUTE)(
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Foreground,
    IN UINT8               Background,
    IN BOOLEAN             Blink
);

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
typedef 
EFI_STATUS (EFIAPI *AMI_PEI_ENABLE_CURSOR)(
    IN AMI_PEI_TEXT_OUT     *This,
    IN BOOLEAN              Enable
);

struct _AMI_PEI_TEXT_OUT {
    AMI_PEI_CLEAR_SCREEN                ClearScreen;
    AMI_PEI_QUERY_MODE                  QueryMode;
    AMI_PEI_WRITE_STRING                WriteString;
    AMI_PEI_SET_CURSOR_POSITION         SetCursorPosition;
    AMI_PEI_SET_ATTRIBUTE               SetAttribute;
    AMI_PEI_ENABLE_CURSOR               EnableCursor;
};

extern EFI_GUID gAmiPeiTextOutPpiGuid;
extern EFI_GUID gAmiPeiConsplitterTextOutPpiGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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
