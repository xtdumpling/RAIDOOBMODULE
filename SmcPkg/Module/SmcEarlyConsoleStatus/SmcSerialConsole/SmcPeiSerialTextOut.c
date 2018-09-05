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
#include <SmcSerialTextOut.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/AmiPeiTextOut.h>
#include <Build/SioBrdSdl.c>
#include <Protocol\AmiBoardInfo2.h>
#include <Library\IoLib.h>
#include <Library\AmiSioPeiLib.h>
#include <Library\PcdLib.h>
#include <Library\BaseMemoryLib.h>
#include <Ppi\ReadOnlyVariable2.h>
#include <Setup.h>
#include <SioSetup.h>
#include <AmiProgressErrorCodeLib.h>
extern unsigned char SioSdlData[];
typedef struct _SMC_SERIAL_TEXTOUT_INSTANCE_ {
  SMC_SERIAL_TEXT_OUT_HOB       Terminal; 	
  AMI_PEI_TEXT_OUT              PpiInterface;
} SMC_SERIAL_TEXTOUT_INSTANCE;
#define _CR(Record, TYPE, Field) ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#define SMC_SERIAL_TEXTOUT_INSTANCE_FROM_THIS(this) _CR(this,SMC_SERIAL_TEXTOUT_INSTANCE,PpiInterface)
UINT16 IoBase [] = {0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x3F0, 0x2F0};


/**
  Read one byte from a Serial Port Register.

  @param[in] Offset             The offset to read.

  @retval                       The value read.

**/
UINT8
SmcSerialPortReadRegister (
  UINT16 Base,		
  UINTN  Offset
  )
{
  return IoRead8 (Base + Offset);
}

/**
  Write one byte to a Serial Port Register.

  @param[in] Offset             The offset to write.
  @param[in] Value              The value to write.

  @retval                       The value written.

**/
UINT8
SmcSerialPortWriteRegister (
  UINT16 Base,		
  UINTN  Offset,
  UINT8  Value
  )
{
  return IoWrite8 (Base + Offset, Value);
}


IndexIoWrite (
  UINT16    Base,		
  UINT8	    Offset,
  UINT8     Data
  ) {
  IoWrite8 (Base, Offset);
  IoWrite8 (Base + 1, Data);
}

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

VOID
InitializeTerminal (
  IN SMC_SERIAL_TEXTOUT_INSTANCE  *Instance,
  IN UINTN                        Index,
  IN UINT16                       IoBase,
  IN SETUP_DATA	                  *SetupData
) {
  Instance->Terminal.PortInfo[Index].BaseAddress = IoBase;
  Instance->Terminal.PortInfo[Index].TerminalType = SetupData->TerminalType[Index];
  Instance->Terminal.PortInfo[Index].RecorderMode = SetupData->RecorderMode[Index];                                          
  Instance->Terminal.PortInfo[Index].Resolution = SetupData->Resolution[Index];                    
  Instance->Terminal.PortInfo[Index].BaudRate = SetupData->BaudRate[Index];   
  Instance->Terminal.PortInfo[Index].StopBits = SetupData->StopBits[Index];   
  Instance->Terminal.PortInfo[Index].DataBits = SetupData->DataBits[Index];   
  Instance->Terminal.PortInfo[Index].Parity = SetupData->Parity[Index];   
  Instance->Terminal.PortNumber ++;
  SmcInitialSerialConsole (&Instance->Terminal.PortInfo[Index]);
}

EFI_STATUS
SmcSerialPortInitialize(
  IN EFI_PEI_SERVICES            **PeiServices,		
  IN SMC_SERIAL_TEXTOUT_INSTANCE *Instance
) {
  EFI_STATUS                           Status;	
  AMI_SDL_SIO_DATA	               *SioDataHeader;
  AMI_SDL_SIO_CHIP_INFO                *SioChipPtr;
  AMI_SDL_LOGICAL_DEV_INFO             *LogicalDevicePtr;
  UINTN                                SioIndex, LogicalDevIndex, ComPortIndex, ResourceIndex, OpIndex, i, j;
  BOOLEAN                              ComPortPresent;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI      *ReadOnlyVariable2;
  SETUP_DATA	                       SetupData;
  UINTN	                               VariableSize;
  COM_NV_DATA                          ComVariable;
  CHAR16                               VariableName[20];
  EFI_GUID                             gEfiSetupGuid = SETUP_GUID;
  EFI_GUID                             SuperIoVariableGuid = SIO_VAR_GUID;
  struct {
    AMI_SDL_SIO_CHIP_INFO    *SioInfo;
    AMI_SDL_LOGICAL_DEV_INFO *DevInfo;
  } COMInfo [TOTAL_SIO_SERIAL_PORTS], Tmp;
  
  Status = (*PeiServices)->LocatePpi (
              PeiServices,
              &gEfiPeiReadOnlyVariable2PpiGuid,
              0,
              NULL,
              (VOID **)&ReadOnlyVariable2);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  VariableSize = sizeof(SETUP_DATA);      
  Status = ReadOnlyVariable2->GetVariable(
  				 ReadOnlyVariable2,
  				 L"Setup",
  				 &gEfiSetupGuid,
  				 NULL,
  				 &VariableSize,  
  				 &SetupData
  				 );
  if (EFI_ERROR (Status)) {
    return Status;	  
  }
  SioDataHeader = (AMI_SDL_SIO_DATA*)(UINTN)SioSdlData;
  SioChipPtr = SioDataHeader->SioDev;
  ComPortIndex = 0;
  SetMem (COMInfo, sizeof(COMInfo), 0x00);
  for (SioIndex = 0; SioIndex < SioDataHeader->SioCount; SioIndex ++) {
     //
     // Check if COM port supported by this SIO.
     //
     LogicalDevicePtr = SioChipPtr->LogicalDev;
     ComPortPresent = FALSE;
     for (LogicalDevIndex = 0; LogicalDevIndex < SioChipPtr->LogicalDevCount; LogicalDevIndex++) {
	 if (LogicalDevicePtr->Type==0x06) {
            COMInfo [ComPortIndex].SioInfo = SioChipPtr; 
            COMInfo [ComPortIndex].DevInfo = LogicalDevicePtr;
            ComPortIndex ++;
	    ComPortPresent = TRUE;
	 }
	 LogicalDevicePtr ++;		 
     }
     
     if (ComPortPresent) {
	//
	//There's COM port in this super IO, enable the Index/Data port.
	//	     
        AmiSioLibSetLpcDeviceDecoding (NULL, SioChipPtr->SioIndex, 0, 0xFF);
     }
     SioChipPtr = (AMI_SDL_SIO_CHIP_INFO *)((UINTN)SioChipPtr + sizeof(AMI_SDL_SIO_CHIP_INFO) + sizeof(AMI_SDL_LOGICAL_DEV_INFO) * SioChipPtr->LogicalDevCount);
  }
  
  ResourceIndex = 0;
  for (i = 0; i < ComPortIndex; i++) {	  
    for (j = i; j < ComPortIndex; j++) {
      if (COMInfo[i].DevInfo->Uid > COMInfo[j].DevInfo->Uid) {
	 Tmp.SioInfo = COMInfo[i].SioInfo;
	 Tmp.DevInfo = COMInfo[i].DevInfo;
	 COMInfo[i].SioInfo = COMInfo[j].SioInfo;
         COMInfo[i].DevInfo = COMInfo[j].DevInfo;
	 COMInfo[j].SioInfo = Tmp.SioInfo;
         COMInfo[j].DevInfo = Tmp.DevInfo;
      }
    }
    if (SetupData.ConsoleRedirectionEnable[i] == 0){
      continue;
    }
    
    VariableSize = sizeof (COM_NV_DATA);
    Swprintf(VariableName, L"PNP0501_%d_NV", i);
    Status = ReadOnlyVariable2->GetVariable (
		          ReadOnlyVariable2,
                          VariableName,
                          &SuperIoVariableGuid,
                          NULL,
                          &VariableSize,
                          &ComVariable
                          );
    if (EFI_ERROR (Status) || ComVariable.DevEnable == 0) {
       continue;	    
    }
 
    for (OpIndex = 0; OpIndex < COMInfo[i].SioInfo->EnterCfgMode[1]; OpIndex++) {
       if (COMInfo[i].SioInfo->EnterCfgMode [2 + OpIndex * 3] == 1 && COMInfo[i].SioInfo->EnterCfgMode [3 + OpIndex * 3] == 1) {
           IoWrite8 (COMInfo[i].SioInfo->SioIndex, COMInfo[i].SioInfo->EnterCfgMode[4 + OpIndex * 3]);
       }
    }    
    AmiSioLibSetLpcDeviceDecoding (NULL, IoBase [i], i + 1, 0x06);
    IndexIoWrite (COMInfo[i].SioInfo->SioIndex, COMInfo[i].SioInfo->DevSelReg, COMInfo[i].DevInfo->Ldn);
    IndexIoWrite (COMInfo[i].SioInfo->SioIndex, COMInfo[i].SioInfo->Base1LoReg, (UINT8)(IoBase [i] & 0xFF));
    IndexIoWrite (COMInfo[i].SioInfo->SioIndex, COMInfo[i].SioInfo->Base1HiReg, (UINT8)(IoBase [i] >> 8));
    IndexIoWrite (COMInfo[i].SioInfo->SioIndex, COMInfo[i].SioInfo->ActivateReg, COMInfo[i].SioInfo->ActivateVal);
    InitializeTerminal (Instance, ResourceIndex, IoBase[i], &SetupData);
    ResourceIndex ++;

     for (OpIndex = 0; OpIndex < COMInfo[i].SioInfo->ExitCfgMode[1]; OpIndex++) {
        if (COMInfo[i].SioInfo->ExitCfgMode [2 + OpIndex * 3] == 1 && COMInfo[i].SioInfo->ExitCfgMode [3 + OpIndex * 3] == 1) {
            IoWrite8 (COMInfo[i].SioInfo->SioIndex, COMInfo[i].SioInfo->ExitCfgMode[4 + OpIndex * 3]);
        }
     } 


  }
  return EFI_SUCCESS;
}

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
    SMC_SERIAL_TEXTOUT_INSTANCE   *Instance;
    UINTN                         i;
    
    Instance = SMC_SERIAL_TEXTOUT_INSTANCE_FROM_THIS(This);
    for (i = 0; i < Instance->Terminal.PortNumber; i++) {
       SmcInitialSerialConsole (&Instance->Terminal.PortInfo[i]);	    
       Status = TerminalClearScreen(&Instance->Terminal.PortInfo[i]);
    }
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
    SMC_SERIAL_TEXTOUT_INSTANCE   *Instance;
	    
    Instance = SMC_SERIAL_TEXTOUT_INSTANCE_FROM_THIS (This);	
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
    EFI_STATUS                    Status;
    SMC_SERIAL_TEXTOUT_INSTANCE   *Instance;
    UINTN                         i;

    Instance = SMC_SERIAL_TEXTOUT_INSTANCE_FROM_THIS (This);
    
    for (i = 0; i < Instance->Terminal.PortNumber; i++) {    
       SmcInitialSerialConsole (&Instance->Terminal.PortInfo[i]);	    	    
       Status = TerminalOutputStringHelper(&Instance->Terminal.PortInfo[i], Text);
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
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Column,
    IN UINT8               Row 
)
{
    EFI_STATUS Status; 
    SMC_SERIAL_TEXTOUT_INSTANCE   *Instance;
    UINTN                          i;        
    Instance = SMC_SERIAL_TEXTOUT_INSTANCE_FROM_THIS (This);


    // Call the SerialTextOut LIB to set the cursor position 
    for (i = 0; i < Instance->Terminal.PortNumber; i++) {
        SmcInitialSerialConsole (&Instance->Terminal.PortInfo[i]);	    	    
        Status = TerminalSetCursorPosition (&Instance->Terminal.PortInfo[i], Column, Row);
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
SerialSetAttribute(
    IN AMI_PEI_TEXT_OUT    *This,
    IN UINT8               Foreground,
    IN UINT8               Background,
    IN BOOLEAN             Blink
)
{

    EFI_STATUS Status;
    UINT8      TextAttribute;
    SMC_SERIAL_TEXTOUT_INSTANCE   *Instance;
    UINTN                          i;
    Instance = SMC_SERIAL_TEXTOUT_INSTANCE_FROM_THIS (This);    
    //    Bits 0..3 are the foreground color, and
    //    bits 4..6 are the background color. All other bits are undefined
    //    and must be zero.
    Foreground = Foreground & 0x0F;
    Background = Background & 0x07;

    TextAttribute = ((Foreground) | (Background << 4));

    // Call the SerialTextOut LIB to set the attribute
    for (i = 0; i < Instance->Terminal.PortNumber; i++) {
       SmcInitialSerialConsole (&Instance->Terminal.PortInfo[i]);	    	    
       Status = TerminalSetAttribute(&Instance->Terminal.PortInfo[i],TextAttribute);
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
    IN AMI_PEI_TEXT_OUT     *This,
    IN BOOLEAN              Enable
)
{
    SMC_SERIAL_TEXTOUT_INSTANCE   *Instance;
	    
    Instance = SMC_SERIAL_TEXTOUT_INSTANCE_FROM_THIS (This);	
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
SmcPeiSerialTextOutEntry (
  IN       EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
)
{
    EFI_STATUS                  Status;
    EFI_PEI_PPI_DESCRIPTOR      *SerialTextOutDescriptor;
    AMI_PEI_TEXT_OUT            *SerialTextOutPpi = NULL;
    EFI_HANDLE                  Handle = NULL;
    SMC_SERIAL_TEXTOUT_INSTANCE *Instance;
    EFI_HOB_GUID_TYPE           *Hob;
    EFI_GUID                    SmcSerialTextOutHobGuid = SMC_SERIAL_TEXT_OUT_HOB_GUID;

    Status = (*PeiServices)->AllocatePool(
                    PeiServices,
                    sizeof(EFI_PEI_PPI_DESCRIPTOR),
                    &SerialTextOutDescriptor);
    if (EFI_ERROR(Status)) { 
        return Status;
    }

    Status = (*PeiServices)->AllocatePool(
                    PeiServices,
                    sizeof(SMC_SERIAL_TEXTOUT_INSTANCE),
                    &Instance);
    if (EFI_ERROR(Status)){ 
        return Status;
    }

    Instance->Terminal.PortNumber = 0;

    Status = SmcSerialPortInitialize(PeiServices, Instance);

    if (EFI_ERROR(Status) || Instance->Terminal.PortNumber == 0){ 
        return Status;
    }
    SerialTextOutPpi = &(Instance->PpiInterface);

    Status = (*PeiServices)->CreateHob (PeiServices, 
		                        EFI_HOB_TYPE_GUID_EXTENSION, 
                                        (UINT16)(sizeof(SMC_SERIAL_TEXT_OUT_HOB) + sizeof (EFI_HOB_GUID_TYPE)), 
                                         (VOID**)&Hob);

    CopyMem (&Hob->Name, &SmcSerialTextOutHobGuid, sizeof (EFI_GUID));
    CopyMem ((VOID *)((UINTN)Hob + sizeof (EFI_HOB_GUID_TYPE)), &Instance->Terminal, sizeof(SMC_SERIAL_TEXT_OUT_HOB));
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

    SerialTextOutPpi->ClearScreen (SerialTextOutPpi);
    SerialTextOutPpi->SetAttribute (SerialTextOutPpi, POST_MSG_FOREGROUND, POST_MSG_BACKGROUND,DISPLAY_BLINK);    
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
