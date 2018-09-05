//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//   Rev. 1.00
//     Bug Fix: Fix kernel panic with SUSE 12 SP1 intermittently.
//     Reason:  Some kernel will use boot service memory area which maybe cause 
//              code of boot serivce area corrupt.
//              Based on UEFI spec, UEFI runtime service should not call boot service
//              routine.
//              Verify SUSE 12 SP1 & RHEL 7.3 & WIN2016
//     Auditor: Leon Xu
//     Date:    May/26/2017
//***************************************************************************

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

/** @file  DxeReportStatusConOut.c
 Defines Status code function to display Progress and Error code to the Conout device.
**/

#include <Token.h>
#include <SmcDxeReportStatusConOut.h>
#include <AmiStatusCodes.h>
#include <Protocol/SimpleTextOut.h>
#include <Library\BaseMemoryLib.h>

AMI_DXE_TEXT_OUT    *gTextOut = NULL;


UnregisterReportHandlers (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_RSC_HANDLER_PROTOCOL        *RscHandlerProtocol = NULL;
    
    //Locate the Report Status Code Handler Protocol
    Status = gBS->LocateProtocol (
                            &gEfiRscHandlerProtocolGuid,
                            NULL,
                            (VOID **) &RscHandlerProtocol
                            );
    
    ASSERT_EFI_ERROR (Status);

    if(RscHandlerProtocol)
        RscHandlerProtocol->Unregister(DxeReportStatusCode);
}



/**

    This function Registers the callback function for the ReportStatusCode 

    @param ImageHandle Handle for the image of this driver
    @param SystemTable Pointer to the EFI System Table

    @retval EFI_SUCCESS Handler Registered Successfully 
    @retval EFI_NOT_FOUND Conout Protocol not found 

**/

EFI_STATUS
EFIAPI
SmcInitializeDxeReportStatus (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable 
  )
{

    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_RSC_HANDLER_PROTOCOL        *RscHandlerProtocol = NULL;
    EFI_EVENT                    ExitBootServicesEvent = NULL;

    //
    // Locate the Consplitter TextOut or Device TextOut Protocol 
    Status = gBS->LocateProtocol (
#if ConsoleOutConsplitter_SUPPORT 
                        &gAmiDxeConsplitterTextOutProtocolGuid,
#else            
                        &gAmiDxeTextOutProtocolGuid,
#endif                        
                        NULL,
                        (VOID **)&gTextOut 
                        );

    // Return from the driver if not found
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //Locate the Report Status Code Handler Protocol
    Status = gBS->LocateProtocol (
                            &gEfiRscHandlerProtocolGuid,
                            NULL,
                            (VOID **) &RscHandlerProtocol
                            );
    
    ASSERT_EFI_ERROR (Status);
    
    if (!EFI_ERROR (Status)) {
        // Register a handler for Status Code
        Status = RscHandlerProtocol->Register (DxeReportStatusCode, TPL_HIGH_LEVEL);
    }
    
    Status = gBS->CreateEventEx(
            EVT_NOTIFY_SIGNAL,
            TPL_NOTIFY,
            UnregisterReportHandlers,
            NULL,
            &gEfiEventExitBootServicesGuid,
            &ExitBootServicesEvent);
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

/**

    Display status code check point and description string.

    @param Value    - EFI status code Value
    @param CodeType - EFI status code type
    @param Instance - The enumeration of a hardware or software entity 
                      within the system. A system may contain multiple entities that
                      match a class/subclass pairing. The instance differentiates
                      between them. An instance of 0 indicates that instance
                      information is unavailable, not meaningful, or not relevant.
                      Valid instance numbers start with 1.
    @param CallerId - This optional parameter may be used to identify the caller.
                      This parameter allows the status code driver to apply different
                      rules to different callers.
    @param Data - This optional parameter may be used to pass additional data.

    @return EFI_STATUS
    @retval EFI_SUCCESS Status code displayed successfully.

**/

EFI_STATUS
EFIAPI
DxeReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *ErrorData OPTIONAL )
{

    UINTN               CheckPointCode;
    CHAR8               *ProgressCodeString=NULL;
    CHAR8               *ErrorCodeString=NULL;
    CHAR8               *PossibleCauseString=NULL;
    CHAR8               *PossibleSolutionString=NULL;
    CHAR8               CheckPointString[5];
    CHAR8               ErrorString[13];
    CHAR8               TextString[160];
    
    EFI_STATUS          Status;

    // Check if the type is Progress Code
    if((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE ) {
        
        // Get the CheckPoint value for the EFI Progress Code
        CheckPointCode= GetAmiProgressCodeCheckPoint (Value);
        if(CheckPointCode == 0) {
            // If no CheckPoint found return it
            return EFI_SUCCESS;
        }
        
        // Get the Progress code String for the 
        ProgressCodeString= GetAmiProgressCodeString (Value);
        
        gTextOut->SetAttribute (gTextOut, POST_MSG_FOREGROUND, POST_MSG_BACKGROUND,DISPLAY_BLINK);
        SetMem (TextString, 60, ' ');
        TextString[60] = 0;
        gTextOut->SetCursorPosition (gTextOut, 00, 24);
        gTextOut->WriteString (gTextOut, PANEL_DISPLAY, TextString);
                
       
          // Add the Progress Code string along with CheckPoint
          if (ProgressCodeString != NULL) {
             // Write String to ConOut Device 
             AsciiSPrint (TextString, 160, "%a", ProgressCodeString);
             gTextOut->SetCursorPosition (gTextOut, 00, 24);
             gTextOut->WriteString (gTextOut, PANEL_DISPLAY, TextString);
          }

          AsciiSPrint (CheckPointString, 5, "%2x", CheckPointCode);
          gTextOut->SetCursorPosition (gTextOut, 78, 24);
          gTextOut->WriteString (gTextOut, PANEL_DISPLAY, CheckPointString);

          // Set the Original Attribute 
        gTextOut->SetAttribute (gTextOut, POST_MSG_FOREGROUND, POST_MSG_BACKGROUND,DISPLAY_BLINK);
    } else if( (CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {

       
        // Get the Check Point value for the EFI Error Code
        CheckPointCode= GetAmiErrorCodeCheckPoint (Value);
        if(CheckPointCode == 0) {
            // If no CheckPoint found return it
            return EFI_SUCCESS;
        }
        
        // Get the Error Code String for the 
        Status= GetAmiErrorCodeString (Value, &ErrorCodeString,&PossibleCauseString,&PossibleSolutionString );
        AsciiSPrint (CheckPointString, 5, "0x%X", CheckPointCode);

        // Set the Attribute based on the Error Type
        if ( (CodeType & EFI_STATUS_CODE_SEVERITY_MASK) == EFI_ERROR_MINOR) {
            gTextOut->SetAttribute (gTextOut, MINOR_ERROR_FOREGROUND, MINOR_ERROR_BACKGROUND, DISPLAY_BLINK);
        } else { 
            gTextOut->SetAttribute (gTextOut, MAJOR_ERROR_FOREGROUND, MAJOR_ERROR_BACKGROUND, DISPLAY_BLINK);
        }

        // Add the Error String along with CheckPoint Value
        if(!EFI_ERROR(Status) && ErrorCodeString != NULL) {
            AsciiSPrint (TextString, 160, "%a : %a \n \r", CheckPointString, ErrorCodeString);
        }
        
        //Write the String to Text Out Device
        gTextOut->WriteString (gTextOut, PANEL_DISPLAY, TextString);
        
        if(ErrorData != NULL) { 
            // Display the Error Code Data
            AsciiSPrint (ErrorString, 13,  "%x %x %x %x ", ((*(UINT32*)(ErrorData + 1)) >> 24) & 0xFF, 
                    ((*(UINT32*)(ErrorData + 1)) >> 16) & 0xFF, ((*(UINT32*)(ErrorData + 1)) >> 8) & 0xFF, 
                    (*(UINT32*)(ErrorData + 1)) & 0xFF);
        
            gTextOut->WriteString(gTextOut, PANEL_DISPLAY, ErrorString);
        }
        
        //Display the Possible Cause String if available
        if(!EFI_ERROR(Status) && PossibleCauseString != NULL) {
            AsciiSPrint (TextString, 160, "%a \n \r", PossibleCauseString);
            gTextOut->WriteString (gTextOut, PANEL_DISPLAY,TextString);
        }
        
        //Display the Possible Solution String if available
        if(!EFI_ERROR(Status) && PossibleSolutionString != NULL) {
            AsciiSPrint (TextString, 160, "%a \n \r", PossibleSolutionString);
            gTextOut->WriteString (gTextOut, PANEL_DISPLAY,TextString);
        }
        
        // Set the Original Attribute
        gTextOut->SetAttribute (gTextOut, POST_MSG_FOREGROUND, POST_MSG_BACKGROUND,DISPLAY_BLINK);
        
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
