//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file VideoCtrlReset.c
    LIB driver for Resetting the Video Controller Programming done in the PEI 

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <Include/AmiLib.h>
#include <Library/DebugLib.h>

extern BOOLEAN      VideoDeviceInUse;
VOID                *gGopProtocolNotifyReg;

/**
    Notification function for Gop Protocol 
    This is stop using the Video device  

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @return VOID

**/

VOID
EFIAPI
GopProtocolCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *TextOutContext 
)
{

    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_HANDLE                  Handle;
    UINTN                       Size = sizeof(EFI_HANDLE);
    VOID                        *TxtOut;

   
    Status = pBS->LocateHandle(ByRegisterNotify,
                                NULL, 
                                gGopProtocolNotifyReg, 
                                &Size, 
                                &Handle);

    if(EFI_ERROR(Status)) {
        return;
    }
    
    // Check If the GOP Protocol Installed on the Handle 
    Status=pBS->HandleProtocol( Handle,
                                &gEfiGraphicsOutputProtocolGuid,
                                &TxtOut
                                );
    if (EFI_ERROR(Status)) {
        return;
    }

    VideoDeviceInUse=TRUE;
    
    // Kill the Event
    pBS->CloseEvent(Event);
    return;
}


/**
    Install the Callback to Reset the Video Controller Programming done in the PEI Phase. 

    @param   None 

    @retval  EFI_NOT_FOUND
**/
EFI_STATUS 
AmiVideoControllerReset(
    VOID
)
{
    EFI_STATUS          Status;
    EFI_EVENT           GopProtocolEvent;
    
    // Install the Callback function to Reset the Video Controller programming. 
    // Porting Required.
    //
    
    // By default the Early Video stopped once the GOP protocol is installed. In case if the 
    // Early Video stopped before that, action done inside the GopProtocolCallBack can be moved
    // to new callback created for stopping the video early 
    
    // Create the notification and register callback function on the GOP Protocol installation
    Status = pBS->CreateEvent (EVT_NOTIFY_SIGNAL, 
                                TPL_CALLBACK,
                                GopProtocolCallBack, 
                                NULL, 
                                &GopProtocolEvent);
    
    ASSERT_EFI_ERROR(Status);
    
    if (!EFI_ERROR(Status)) {

        Status = pBS->RegisterProtocolNotify (
                                    &gEfiGraphicsOutputProtocolGuid,
                                    GopProtocolEvent, 
                                    &gGopProtocolNotifyReg
                                    );
    
        ASSERT_EFI_ERROR(Status);
    }    
    
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
