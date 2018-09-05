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
  @file  DxeSerialTextOutt.c
  This file contains the Protocol functions to use Serial device.
*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AmiDxeTextOut.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <SmcSerialTextOut.h>
#include <Guid/HobList.h>
#include <Library/BaseMemoryLib.h>
VOID * GetEfiConfigurationTable(
	IN EFI_SYSTEM_TABLE *SystemTable,
	IN EFI_GUID			*Guid
);

AMI_DXE_TEXT_OUT    SerialTextOut;
BOOLEAN             SerialPortInUse=FALSE;
VOID                *gSimpleTextOutNotifyReg;
SMC_SERIAL_TEXT_OUT_HOB  gTerminalInfo;

VOID
SmcInitialSerialConsole (
  IN SERIAL_CONSOLE_CONFIGURATION  *Config
  ) {
   UINTN      Divisor;
   UINTN      Remainder;	
   UINT8      Temp;
   UINTN      BaudRateTable [] = {0, 1, 2, 9600, 19200, 38400, 57600, 115200}; 
      
   Temp= IoRead8(Config->BaseAddress + LCR_OFFSET);
   //
   // Map 5..8 to 0..3
   //
   Temp |= DLAB;
   IoWrite8 (Config->BaseAddress + LCR_OFFSET, Temp);
   //
   // Calculate divisor for baud generator
   //
   Divisor = (UINT32) Div64 (1843200, (BaudRateTable [Config->BaudRate] * 16), &Remainder);
   if ( Remainder ) {
       Divisor += 1;
   }

   //
   // Configure baud rate
   //
   //  
   IoWrite8 (Config->BaseAddress + BAUD_HIGH_OFFSET, (UINT8) (Divisor >> 8));
   IoWrite8 (Config->BaseAddress + BAUD_LOW_OFFSET, (UINT8) (Divisor & 0xff));
    //
    // Set communications format
    //
    //
   
   
    Temp &= ~DLAB;    // Kill divisor latch bit.
	       
    // Set parity bits.
    switch (Config->Parity) {
           case NoParity:
               Temp &= ~(PAREN | EVENPAR | STICPAR);
           break;
           case EvenParity:
               Temp |= (PAREN | EVENPAR);
               Temp &= ~STICPAR;
           break;
           case OddParity:
               Temp |= PAREN;
               Temp &= ~(EVENPAR | STICPAR);
           break;
           case SpaceParity:
               Temp |= (PAREN | EVENPAR | STICPAR);
           break;
           case MarkParity:
               Temp |= (PAREN | STICPAR);
               Temp &= ~EVENPAR;
           break;
           default:
           break;
    }
   //
   // Set stop bits.
   //
   switch (Config->StopBits) {
           case OneStopBit :
               Temp &= ~STOPB;
           break;
           case OneFiveStopBits :
           case TwoStopBits :
               Temp |= STOPB;
           break;
           default:
           break;
  }
  //
  // Set data bits. 
  //
  Temp &= ~SERIALDB;
  Temp |= (UINT8)((Config->DataBits - 5) & 0x03);
  IoWrite8 (Config->BaseAddress + LCR_OFFSET, Temp);
}

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
SerialClearScreen(
  IN AMI_DXE_TEXT_OUT *This
)
{
    EFI_STATUS Status; 
    UINTN      i;
    if(SerialPortInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }
    
    for (i = 0; i < gTerminalInfo.PortNumber; i++) {	
       SmcInitialSerialConsole (&gTerminalInfo.PortInfo[i]);
       Status = TerminalClearScreen(&gTerminalInfo.PortInfo[i]);
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
SerialQueryMode(
    IN AMI_DXE_TEXT_OUT     *This,
    IN UINT8                Mode,
    IN OUT UINT8            *Col,
    IN OUT UINT8            *Rows
)
{
    return EFI_UNSUPPORTED;
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
SerialWriteString(
    IN AMI_DXE_TEXT_OUT    *This,
    IN UINT8               Panel,
    IN UINT8               *Text
)
{
    EFI_STATUS   Status;
    UINTN        i;
    if(SerialPortInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }
    for (i = 0; i < gTerminalInfo.PortNumber; i++) {    
        SmcInitialSerialConsole (&gTerminalInfo.PortInfo[i]);	    
        Status = TerminalOutputStringHelper(&gTerminalInfo.PortInfo[i], Text);
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
SerialSetCursorPosition(
    IN AMI_DXE_TEXT_OUT    *This,
    IN UINT8               Column,
    IN UINT8               Row 
)
{
    EFI_STATUS Status; 
    UINTN      i;
    if(SerialPortInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }
    for (i = 0; i < gTerminalInfo.PortNumber; i++) {  
        SmcInitialSerialConsole (&gTerminalInfo.PortInfo[i]);	    
        Status = TerminalSetCursorPosition (&gTerminalInfo.PortInfo[i], Column, Row);
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
SerialSetAttribute(
    IN AMI_DXE_TEXT_OUT    *This,
    IN UINT8               Foreground,
    IN UINT8               Background,
    IN BOOLEAN             Blink
)
{

    EFI_STATUS Status;
    UINT8      TextAttribute;
    UINTN      i;

    if(SerialPortInUse == TRUE) {
        return EFI_DEVICE_ERROR;
    }
    //    Bits 0..3 are the foreground color, and
    //    bits 4..6 are the background color. All other bits are undefined
    //    and must be zero.
    Foreground = Foreground & 0x0F;
    Background = Background & 0x07;

    TextAttribute = ((Foreground) | (Background << 4));
    for (i = 0; i < gTerminalInfo.PortNumber; i++) {  
       SmcInitialSerialConsole (&gTerminalInfo.PortInfo[i]);	    
       Status = TerminalSetAttribute(&gTerminalInfo.PortInfo[i],TextAttribute);
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
SerialEnableCursor(
    IN AMI_DXE_TEXT_OUT     *This,
    IN BOOLEAN              Enable
)
{
    return EFI_UNSUPPORTED;
}

/**
    Notification function for SimpleTextOut Protocol 
    This is stop using the Serial device 

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @return VOID

**/

VOID
EFIAPI
SimpleTextOutCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *TextOutContext 
)
{

    EFI_STATUS                  Status = EFI_SUCCESS;
    BOOLEAN                     SimpleTextOutOnSerialIo=FALSE;
    EFI_DEVICE_PATH_PROTOCOL    *TerminalDevicePath;
    EFI_HANDLE                  Handle;
    UINTN                       Size = sizeof(EFI_HANDLE);
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *TxtOut;

   
    Status = gBS->LocateHandle(ByRegisterNotify,
                                NULL, 
                                gSimpleTextOutNotifyReg, 
                                &Size, 
                                &Handle);

    if(EFI_ERROR(Status)) {
        return;
    }
    
    // Check If the SimpleTextout Installed on the Handle 
    Status=gBS->HandleProtocol( Handle,
                                &gEfiSimpleTextOutProtocolGuid,
                                &TxtOut
                                );
    if (EFI_ERROR(Status)) {
        return;
    }
    
    // Get the Device Path Protocol 
    Status=gBS->HandleProtocol( Handle,
                                &gEfiDevicePathProtocolGuid,
                                (VOID**)&TerminalDevicePath );
    
    if (EFI_ERROR(Status)) {
        return;
    }

    // Check for the UART Node
    while(TerminalDevicePath->Type != END_DEVICE_PATH_TYPE){
        if ((TerminalDevicePath->Type == MESSAGING_DEVICE_PATH) && 
            (TerminalDevicePath->SubType == MSG_UART_DP) ) {
            // UART node found. The SimpleTextout created from UART device.
            SimpleTextOutOnSerialIo = TRUE;
            break;
        }
        TerminalDevicePath = NextDevicePathNode(TerminalDevicePath);
    }

    // Return if the SimpleTextOut is not from UART device. 
    if(SimpleTextOutOnSerialIo == FALSE) {
        return;
    }

    SerialPortInUse=TRUE;
    
    // Kill the Event
    gBS->CloseEvent(Event);
    return;
}

/**
    This function is the entry point for this DXE driver.
    This installs the Serial TextOut Protocol.

    @param ImageHandle Variable of EFI_HANDLE.
    @param SystemTable Pointer to EFI_SYSTEM_TABLE

    @return EFI_STATUS
    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
EFIAPI
SmcDxeSerialTextOutEntry (
    IN EFI_HANDLE              ImageHandle,
    IN EFI_SYSTEM_TABLE        *SystemTable
)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  Handle = NULL;
    EFI_EVENT                   SimpleTextOutEvent;
    EFI_HOB_GUID_TYPE           *Hob;
    EFI_GUID                    SmcSerialTextOutHobGuid = SMC_SERIAL_TEXT_OUT_HOB_GUID;
    EFI_GUID                    gHobListGuid = HOB_LIST_GUID;
    
    Hob = GetEfiConfigurationTable(SystemTable, &gHobListGuid);
    if (Hob) {
       while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION,&Hob)) ) {
    	  if (CompareGuid (&Hob->Name,&SmcSerialTextOutHobGuid)) {
    	      CopyMem (&gTerminalInfo, (VOID *)((UINTN)Hob + sizeof (EFI_HOB_GUID_TYPE)), sizeof(SMC_SERIAL_TEXT_OUT_HOB));
              break;
    	  }
       }    		
    } else {
	return EFI_SUCCESS;    	    
    }
    if (EFI_ERROR (Status)) {

	return Status;    
    }

    SerialTextOut.ClearScreen = SerialClearScreen;
    SerialTextOut.QueryMode = SerialQueryMode;
    SerialTextOut.WriteString = SerialWriteString;
    SerialTextOut.SetCursorPosition = SerialSetCursorPosition;
    SerialTextOut.SetAttribute = SerialSetAttribute;
    SerialTextOut.EnableCursor = SerialEnableCursor;
   
    Status = gBS->InstallProtocolInterface ( 
                               &Handle,
                               &gAmiDxeTextOutProtocolGuid,
                               EFI_NATIVE_INTERFACE,
                               &SerialTextOut
                               );

    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    // Create the notification and register callback function on the SimpleTextOut Protocol installation
    Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, 
                                TPL_CALLBACK,
                                SimpleTextOutCallBack, 
                                NULL, 
                                &SimpleTextOutEvent);
    
    ASSERT_EFI_ERROR(Status);
    
    if (!EFI_ERROR(Status)) {

        Status = gBS->RegisterProtocolNotify (
                                    &gEfiSimpleTextOutProtocolGuid,
                                    SimpleTextOutEvent, 
                                    &gSimpleTextOutNotifyReg
                                    );
    
        ASSERT_EFI_ERROR(Status);
    }
    
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
