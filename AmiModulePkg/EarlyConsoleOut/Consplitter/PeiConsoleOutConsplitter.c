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
  @file  PeiConsoleOutConsplitter.c
  This file contains the PPI functions for the Consplitter for the PEI Text out drivers 
*/

#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/AmiPeiTextOut.h>
#include <Ppi/MemoryDiscovered.h>
#include <Library/HobLib.h>


EFI_GUID  gAmiConsplitterConOutHobGuid   = { 0xd4e88d49, 0x7e1e, 0x4c10, { 0x8f, 0xa3, 0xb3, 0xa1, 0xff, 0x36, 0x7, 0x37 } };


EFI_STATUS 
EFIAPI
TextOutNotifyCallBack (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *Ppi
);

EFI_STATUS 
EFIAPI
PeiTextOutHobMemoryInstalledCallBack (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *Ppi
);

EFI_PEI_NOTIFY_DESCRIPTOR mTextOutPpiNotifyDescs = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gAmiPeiTextOutPpiGuid,
    TextOutNotifyCallBack
};

EFI_PEI_NOTIFY_DESCRIPTOR MemoryInstalledPpiNotifyDescs = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMemoryDiscoveredPpiGuid,
    PeiTextOutHobMemoryInstalledCallBack
};


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
ConsplitterClearScreen(
  IN AMI_PEI_TEXT_OUT *This
)
{
    EFI_STATUS 						Status; 
    EFI_PEI_HOB_POINTERS            Hob;
    UINTN                           *NumberOfEntries;
    AMI_PEI_TEXT_OUT        		**HobTextOutPpi;
    UINTN							Index=0;

    // Get the First Hob
    Hob.Raw  = GetFirstGuidHob (&gAmiConsplitterConOutHobGuid);
    
    // Return if there is no HOB present	
    if(Hob.Raw == NULL) {
    	return EFI_SUCCESS;
    }

    // Get the Number of entries presnet in the HOB 
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);
    
    // Pointer to where the Data Starts
    HobTextOutPpi   = (AMI_PEI_TEXT_OUT **) (NumberOfEntries + 1);
    
    // Go through Each TextOutPpi and call the API function's.
    for (Index = 0; Index < *NumberOfEntries; Index++) {
    	Status=HobTextOutPpi[Index]->ClearScreen(HobTextOutPpi[Index]);
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
ConsplitterQueryMode(
    AMI_PEI_TEXT_OUT *This,
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

    @param  This   The PPI instance pointer.
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
    AMI_PEI_TEXT_OUT    *This,
    UINT8               Panel,
    UINT8               *Text
)
{
    EFI_STATUS 						Status; 
    EFI_PEI_HOB_POINTERS            Hob;
    UINTN                           *NumberOfEntries;
    AMI_PEI_TEXT_OUT        		**HobTextOutPpi;
    UINTN							Index=0;

    // Get the First Hob
    Hob.Raw  = GetFirstGuidHob (&gAmiConsplitterConOutHobGuid);
    
    // Return if there is no HOB present	
    if(Hob.Raw == NULL) {
    	return EFI_SUCCESS;
    }

    // Get the Number of entries present in the HOB 
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);
    
    // Pointer to where the Data Starts
    HobTextOutPpi   = (AMI_PEI_TEXT_OUT **) (NumberOfEntries + 1);
    
    // Go through Each TextOutPpi and call the API function's.
    for (Index = 0; Index < *NumberOfEntries; Index++) {
    	Status=HobTextOutPpi[Index]->WriteString(HobTextOutPpi[Index], Panel, Text);
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
    AMI_PEI_TEXT_OUT    *This,
    IN UINT8            Column,
    IN UINT8            Row 
)
{
    EFI_STATUS                      Status; 
    EFI_PEI_HOB_POINTERS            Hob;
    UINTN                           *NumberOfEntries;
    AMI_PEI_TEXT_OUT                **HobTextOutPpi;
    UINTN                           Index=0;

    // Get the First Hob
    Hob.Raw  = GetFirstGuidHob (&gAmiConsplitterConOutHobGuid);
    
    // Return if there is no HOB present	
    if(Hob.Raw == NULL) {
    	return EFI_SUCCESS;
    }

    // Get the Number of entries presnet in the HOB 
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);
    
    // Pointer to where the Data Starts
    HobTextOutPpi   = (AMI_PEI_TEXT_OUT **) (NumberOfEntries + 1);
    
    // Go through Each TextOutPpi and call the API function's.
    for (Index = 0; Index < *NumberOfEntries; Index++) {
    	Status=HobTextOutPpi[Index]->SetCursorPosition(HobTextOutPpi[Index], Column, Row);
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
    AMI_PEI_TEXT_OUT    *This,
    UINT8               Foreground,
    UINT8               Background,
    BOOLEAN             Blink
)
{

    EFI_STATUS                      Status; 
    EFI_PEI_HOB_POINTERS            Hob;
    UINTN                           *NumberOfEntries;
    AMI_PEI_TEXT_OUT                **HobTextOutPpi;
    UINTN                           Index=0;

    // Get the First Hob
    Hob.Raw  = GetFirstGuidHob (&gAmiConsplitterConOutHobGuid);
    
    // Return if there is no HOB present	
    if(Hob.Raw == NULL) {
    	return EFI_SUCCESS;
    }

    // Get the Number of entries presnet in the HOB 
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);
    
    // Pointer to where the Data Starts
    HobTextOutPpi   = (AMI_PEI_TEXT_OUT **) (NumberOfEntries + 1);
    
    // Go through Each TextOutPpi and call the API function's.
    for (Index = 0; Index < *NumberOfEntries; Index++) {
    	Status=HobTextOutPpi[Index]->SetAttribute(HobTextOutPpi[Index], Foreground, Background, Blink);
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
    AMI_PEI_TEXT_OUT *This,
    BOOLEAN         Enable
)
{
    return EFI_UNSUPPORTED;
}

/**
    Creates HOB to save the TextOut PPI 
    
    @param PeiServices Pointer to the PEI Core data Structure

    @retval Return the Address of the HOB Data 

**/
UINTN*
CreateTextOutPpiHob ( 
    IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
    UINTN  *NumberOfEntries;
    EFI_HOB_GUID_TYPE *Hob;
    EFI_STATUS        Status;

    // Build GUID'ed HOB with PCD defined size.
    Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, 
                            (UINT16)(sizeof (AMI_PEI_TEXT_OUT *) * 3 
                        + sizeof (UINTN)+ sizeof (EFI_HOB_GUID_TYPE)), (VOID**)&Hob);
  
    if (EFI_ERROR(Status)) { 
    	return NULL;
    }

    CopyMem (&Hob->Name, &gAmiConsplitterConOutHobGuid, sizeof (EFI_GUID));

    NumberOfEntries = (UINTN*)(Hob + 1);                    
    *NumberOfEntries = 0;

    return NumberOfEntries;
}

/**
    Save the TextOut PPI into HOB Data Area 

    @param  TextOutPpi  The TextOut PPI instance pointer.
    @param  PeiServices Pointer to the PEI Core data Structure

    @retval None
**/
VOID
AddTextOutPpiInHob ( 
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN AMI_PEI_TEXT_OUT		    *TextOutPpi
  )
{
    UINTN                       *NumberOfEntries;
    EFI_PEI_HOB_POINTERS        Hob;
    AMI_PEI_TEXT_OUT            **HobTextOutPpi;

    // Get the Hob Address 
    Hob.Raw  = GetFirstGuidHob (&gAmiConsplitterConOutHobGuid);

    // Return if there is no HOB present    
    if(Hob.Raw == NULL) {
        return ;
    }
       
    // Move to the Data area. 
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);

    // First entry is number of data present.
    // 2nd entry is TextOutPpi Address
    HobTextOutPpi = (AMI_PEI_TEXT_OUT **) (NumberOfEntries + 1);
    
    // Save the TextOutPpi into HOB Area
    HobTextOutPpi[*NumberOfEntries] = TextOutPpi;
    
    // Incremen the Number of the Entry
    *NumberOfEntries += 1;

    return;
}

/**

    Provide a callback for when the TextoutPpi is installed 
  
    @param PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
    @param NotifyDescriptor   The descriptor for the notification event.
    @param Ppi                Pointer to the PPI in question.

    @return Always success

**/

EFI_STATUS 
EFIAPI
TextOutNotifyCallBack (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *Ppi
)
{

    AddTextOutPpiInHob(PeiServices, (AMI_PEI_TEXT_OUT*)Ppi);
    return EFI_SUCCESS;
}

/**

    Provide a callback for when the TextoutPpi is installed 
  
    @param PeiServices        An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
    @param NotifyDescriptor   The descriptor for the notification event.
    @param Ppi                Pointer to the PPI in question.

    @return Always success

**/

EFI_STATUS 
EFIAPI
PeiTextOutHobMemoryInstalledCallBack (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *Ppi
)
{
    
    UINTN                       *NumberOfEntries;
    EFI_PEI_HOB_POINTERS        Hob;
    AMI_PEI_TEXT_OUT            **HobTextOutPpi;
    UINTN                       Index=0;
    EFI_STATUS                  Status;
    AMI_PEI_TEXT_OUT            *TextOutPpi;

    // Get the Hob Address 
    Hob.Raw  = GetFirstGuidHob (&gAmiConsplitterConOutHobGuid);

    // Return if there is no HOB present    
    if(Hob.Raw == NULL) {
        return EFI_SUCCESS;
    }
    
    // Move to the Data area. 
    NumberOfEntries = GET_GUID_HOB_DATA (Hob.Raw);    
    
    *NumberOfEntries = 0;

    // First entry is number of data present.
    // 2nd entry is TextOutPpi Address
    HobTextOutPpi = (AMI_PEI_TEXT_OUT **) (NumberOfEntries + 1);

    do {

        // Locate the TextOut PPI
        Status = (*PeiServices)->LocatePpi(
                            PeiServices,
                            &gAmiPeiTextOutPpiGuid,
                            Index,
                            NULL,
                            &TextOutPpi);        
        
        if(!EFI_ERROR(Status)) {
            // Save the TextOutPpi into HOB Area
            HobTextOutPpi[*NumberOfEntries] = TextOutPpi;
        
            // Increment the Number of the Entry
            *NumberOfEntries += 1;  
        }
        
        Index++;
        
    } while(!EFI_ERROR(Status));
    
    return EFI_SUCCESS;
}

/**
    This function is the entry point for this PEI.
    This installs the Consplitter PPI for the Console Out device 

    @param FileHandle Pointer to image file handle.
    @param PeiServices Pointer to the PEI Core data Structure

    @return EFI_STATUS
    @retval EFI_SUCCESS Successful driver initialization

**/
EFI_STATUS
EFIAPI
PeiConsoleOutConsplitterEntry (
  IN       EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
)
{
    EFI_STATUS                  Status;
    EFI_PEI_PPI_DESCRIPTOR      *ConsplitterTextOutDescriptor;
    AMI_PEI_TEXT_OUT            *ConsplitterTextOutPpi = NULL;
    EFI_HANDLE                  Handle = NULL;

    // Allocate the buffer for the PPI
    Status = (*PeiServices)->AllocatePool(
                    PeiServices,
                    sizeof(EFI_PEI_PPI_DESCRIPTOR),
                    &ConsplitterTextOutDescriptor);

    if (EFI_ERROR(Status)) { 
        return Status;
    }

    Status = (*PeiServices)->AllocatePool(
                    PeiServices,
                    sizeof(AMI_PEI_TEXT_OUT),
                    &ConsplitterTextOutPpi);

    if (EFI_ERROR(Status)){ 
        return Status;
    }

    // Create the HOB to save the TextOutPpi
    CreateTextOutPpiHob(PeiServices);

    // Create callback for the TextPpi. 
    Status = (*PeiServices)->NotifyPpi(PeiServices, &mTextOutPpiNotifyDescs);
    ASSERT_EFI_ERROR(Status);    

    // Create callback for the TextPpi. 
    Status = (*PeiServices)->NotifyPpi(PeiServices, &MemoryInstalledPpiNotifyDescs);
    ASSERT_EFI_ERROR(Status);    

    
    // Initialize the Consplitter TextOut PPI
    ConsplitterTextOutDescriptor->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    ConsplitterTextOutDescriptor->Guid = &gAmiPeiConsplitterTextOutPpiGuid;
    ConsplitterTextOutDescriptor->Ppi = ConsplitterTextOutPpi;

    ConsplitterTextOutPpi->ClearScreen = ConsplitterClearScreen;
    ConsplitterTextOutPpi->QueryMode = ConsplitterQueryMode;
    ConsplitterTextOutPpi->WriteString = ConsplitterWriteString;
    ConsplitterTextOutPpi->SetCursorPosition = ConsplitterSetCursorPosition;
    ConsplitterTextOutPpi->SetAttribute = ConsplitterSetAttribute;
    ConsplitterTextOutPpi->EnableCursor = ConsplitterEnableCursor;

    // Install the Consplitter TextOut PPI
    Status = (*PeiServices)->InstallPpi(PeiServices, ConsplitterTextOutDescriptor);
    ASSERT_EFI_ERROR(Status);
    
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
