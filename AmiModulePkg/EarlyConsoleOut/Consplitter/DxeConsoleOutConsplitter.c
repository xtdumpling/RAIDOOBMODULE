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

/**
  @file  DxeConsoleOutConsplitter.c
  This file contains the Protocol functions to for the Consplitter driver for the TextOutProtocol. 
*/


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AmiDxeTextOut.h>
#include <Library/BaseLib.h>
#include <AmiDxeLib.h>

AMI_DXE_TEXT_OUT    ConsplitterTextOut;


typedef struct _TEXT_OUT_RECORD{
    DLINK                   Link;
    AMI_DXE_TEXT_OUT        *TextOut;
} TEXT_OUT_RECORD;

DLIST               gTextOutData;
TEXT_OUT_RECORD     *TextOutRecord;
VOID                *gProtocolNotifyRegistration;

VOID
AddTextOutProtocol(
    IN AMI_DXE_TEXT_OUT *TextOutProtocol
);

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
ConsplitterClearScreen(
  IN AMI_DXE_TEXT_OUT *This
)
{
    EFI_STATUS Status; 
    TEXT_OUT_RECORD  *TextOutRecord = OUTTER(gTextOutData.pHead, Link, TEXT_OUT_RECORD);

    if (TextOutRecord == NULL)
        return EFI_DEVICE_ERROR;

    // we need to loop through all the registered text out devices
    //	and call each of their ClearScree function
    while ( TextOutRecord != NULL) {
        Status = TextOutRecord->TextOut->ClearScreen(TextOutRecord->TextOut);
        TextOutRecord = OUTTER(TextOutRecord->Link.pNext, Link, TEXT_OUT_RECORD);
    }
   	
    return Status; 
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
ConsplitterQueryMode(
    AMI_DXE_TEXT_OUT *This,
    UINT8           Mode,
    UINT8           *Col,
    UINT8           *Rows
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

    @param  This   The Protocol instance pointer.
    @param  Panel  Panel number of the display device 
    @param  String The NULL-terminated string to be displayed on the output
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
ConsplitterWriteString(
    AMI_DXE_TEXT_OUT    *This,
    UINT8               Panel,
    UINT8               *Text
)
{
    EFI_STATUS Status;
    TEXT_OUT_RECORD  *TextOutRecord = OUTTER(gTextOutData.pHead, Link, TEXT_OUT_RECORD);

    if (TextOutRecord == NULL)
        return EFI_DEVICE_ERROR;

    // we need to loop through all the registered text out devices
    //  and call each of their WriteString function
    while ( TextOutRecord != NULL) {
        Status = TextOutRecord->TextOut->WriteString(TextOutRecord->TextOut, Panel,Text);
        TextOutRecord = OUTTER(TextOutRecord->Link.pNext, Link, TEXT_OUT_RECORD);
    }
    
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
ConsplitterSetCursorPosition(
    AMI_DXE_TEXT_OUT    *This,
    IN UINT8            Column,
    IN UINT8            Row 
)
{
    EFI_STATUS Status;
    TEXT_OUT_RECORD  *TextOutRecord = OUTTER(gTextOutData.pHead, Link, TEXT_OUT_RECORD);

    if (TextOutRecord == NULL)
        return EFI_DEVICE_ERROR;

    // we need to loop through all the registered text out devices
    //  and call each of their SetCursorPosition function
    while ( TextOutRecord != NULL) {
        Status = TextOutRecord->TextOut->SetCursorPosition(TextOutRecord->TextOut, Column,Row);
        TextOutRecord = OUTTER(TextOutRecord->Link.pNext, Link, TEXT_OUT_RECORD);
    }    

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
ConsplitterSetAttribute(
    AMI_DXE_TEXT_OUT    *This,
    UINT8               Foreground,
    UINT8               Background,
    BOOLEAN             Blink
)
{

    EFI_STATUS Status=EFI_NOT_FOUND;
    TEXT_OUT_RECORD  *TextOutRecord = OUTTER(gTextOutData.pHead, Link, TEXT_OUT_RECORD);

    if (TextOutRecord == NULL)
        return EFI_DEVICE_ERROR;

    // we need to loop through all the registered text out devices
    //  and call each of their SetAttribute function
    while ( TextOutRecord != NULL) {
        Status = TextOutRecord->TextOut->SetAttribute(TextOutRecord->TextOut, Foreground, Background,Blink);
        TextOutRecord = OUTTER(TextOutRecord->Link.pNext, Link, TEXT_OUT_RECORD);
    }
    
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
ConsplitterEnableCursor(
    AMI_DXE_TEXT_OUT *This,
    BOOLEAN         Enable
)
{
    return EFI_UNSUPPORTED;
}

/**
    Notification function for TextOut Protocol 
    When the TextOut Protocol is installed, this gets notified and added the Protocol
    into the Linked list

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @return VOID

**/
VOID 
EFIAPI
TextOutCallBack (
    IN EFI_EVENT        Event,
    IN VOID             *Context 
)
{
    EFI_STATUS              Status;
    EFI_HANDLE              Handle;
    UINTN                   BufferSize = 20 * sizeof(EFI_HANDLE);
    AMI_DXE_TEXT_OUT        *TextOutProtocol;
    
    Status = gBS->LocateHandle( ByRegisterNotify, 
                                NULL, 
                                gProtocolNotifyRegistration, 
                                &BufferSize, 
                                &Handle 
                                );
    
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }
    
    // Locate TextOut protocol installed on Handle
    
    Status = gBS->HandleProtocol( Handle, 
                                &gAmiDxeTextOutProtocolGuid, 
                                &TextOutProtocol 
                                );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }    
    
    AddTextOutProtocol(TextOutProtocol);
    
  	return;
}
	
/**
    Add the TextOut Protocol into Linked List 

    @param  TextOutPpi  The TextOut PPI instance pointer.

    @retval None
**/

VOID
AddTextOutProtocol(
    IN AMI_DXE_TEXT_OUT *TextOutProtocol
)
{

    EFI_STATUS	Status;

    // Create database record and add to database
    Status = gBS->AllocatePool ( EfiBootServicesData,
                                 sizeof (TEXT_OUT_RECORD),
                                 (VOID**)&TextOutRecord
                                 );
    
    if(EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
    	return;
    }
    
    TextOutRecord->TextOut   = TextOutProtocol;

    DListAdd (&gTextOutData, &(TextOutRecord->Link));
    
    return;

}


/**
    This function is the entry point for Dxe Consplitter driver.
    This installs the Consplitter Text Out protocol 

    @param ImageHandle Variable of EFI_HANDLE.
    @param SystemTable Pointer to EFI_SYSTEM_TABLE

    @return EFI_STATUS
    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
EFIAPI
DxeConsoleOutConsplitterEntry (
    IN EFI_HANDLE              ImageHandle,
    IN EFI_SYSTEM_TABLE        *SystemTable
)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  Handle = NULL;
    EFI_HANDLE                  *TextOutHandle;
    UINTN                       NumberOfHandles;
    UINTN                       Index;
    AMI_DXE_TEXT_OUT            *TextOutProtocol;
    EFI_EVENT                   Event;

    // Initilize the Protocol API functions
    ConsplitterTextOut.ClearScreen = ConsplitterClearScreen;
    ConsplitterTextOut.QueryMode = ConsplitterQueryMode;
    ConsplitterTextOut.WriteString = ConsplitterWriteString;
    ConsplitterTextOut.SetCursorPosition = ConsplitterSetCursorPosition;
    ConsplitterTextOut.SetAttribute = ConsplitterSetAttribute;
    ConsplitterTextOut.EnableCursor = ConsplitterEnableCursor;
    
    // Install the Protocol Interface 
    Status = gBS->InstallProtocolInterface ( 
                               &Handle,
                               &gAmiDxeConsplitterTextOutProtocolGuid,
                               EFI_NATIVE_INTERFACE,
                               &ConsplitterTextOut
                               );
    
    // Initilize the linked List 
    DListInit(&gTextOutData);

    
    // Locate all the existing TextOut Protocols.
    Status = gBS->LocateHandleBuffer(
                                ByProtocol,
                                &gAmiDxeTextOutProtocolGuid, 
                                NULL, 
                                &NumberOfHandles, 
                                &TextOutHandle
                                );
    
    for (Index=0; Index<NumberOfHandles; Index++) {
        Status= gBS->HandleProtocol(TextOutHandle[Index], 
                                    &gAmiDxeTextOutProtocolGuid, 
                                    (VOID**) &TextOutProtocol);
        
        if (EFI_ERROR(Status)) { 
        	continue;
        }
    
        // Add them to Linked List
        AddTextOutProtocol(TextOutProtocol);
    }

    // Register callback notification on TextOut Protocol 
    Status = gBS->CreateEvent( EVT_NOTIFY_SIGNAL,
                                TPL_NOTIFY,
                                TextOutCallBack,
                                NULL,
                                &Event );
    
    ASSERT_EFI_ERROR(Status);

    Status = gBS->RegisterProtocolNotify(
                                &gAmiDxeTextOutProtocolGuid,
                                Event,
                                &gProtocolNotifyRegistration);

    
    ASSERT_EFI_ERROR(Status);
    
    return EFI_SUCCESS;
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
