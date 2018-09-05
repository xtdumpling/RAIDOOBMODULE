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

/** @file TerminalPostMsg.c
    Automatically display corresponding Serial Redirection Hotkeys used to enter Setup 
    and for BBS PopUp on post screen, if Redirection Enabled.

**/

#include "Token.h"
#include <AmiDxeLib.h>
#include <Protocol/SerialIo.h>
#include <Library/HiiLib.h>
#include <Protocol/AMIPostMgr.h>

VOID
EFIAPI
TerminalHotKeyPostMsgCallBack (
    IN EFI_EVENT  Event,
    IN VOID       *Context
);

extern EFI_HII_HANDLE        gHiiHandle;
EFI_EVENT                    TerminalHotKeyPostMsgEvent = NULL;
void                         *TerminalHotKeyPostMsgRegistration  = NULL;
CHAR16                       *TerminalHotKeyPostMsgtext = NULL;
AMI_POST_MANAGER_PROTOCOL    *gAmiPostMgr = NULL;

/**
    ELINK hook Function,Display's corresponding Serial Redirection Hotkeys 
    used to enter Setup and for BBS PopUp on post screen, if Redirection 
    Enabled.


    @param VOID

    @retval BOOLEAN

**/
BOOLEAN
TerminalHotKeyPostMsg(VOID)
{
    UINTN                                   HandleCount;
    UINTN                                   Index=0;
    UINTN                                   Index1=0;
    UINTN                                   EntryCount=0;
    EFI_HANDLE                              *HandleBuffer;
    EFI_STATUS                              Status;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY     *OpenInfoBuffer=NULL;
    BOOLEAN									TerminalDeviceFound = FALSE;
    static BOOLEAN                          TerminalPostScreenMsg = FALSE;
    //
    // Locate the AMI Post Manger protocol for displaying information on the 
    // post screen
    //
    Status = pBS->LocateProtocol( 
                                  &gAmiPostManagerProtocolGuid, 
                                  NULL,
                                  (void**) &gAmiPostMgr
                                 );
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) {
        return FALSE;
    }
    // Find the number of Handle that is supported by the serial IO protocol

    Status = pBS->LocateHandleBuffer ( 
                                       ByProtocol,
                                       &gEfiSerialIoProtocolGuid,
                                       NULL,
                                       &HandleCount,
                                       &HandleBuffer
                                       );
    if (EFI_ERROR (Status)) {
       if (TerminalHotKeyPostMsgEvent == NULL) {
           // Register CallBack to Display the Terminal POST message,
           // when Terminal's gEfiSimpleTextOutProtocolGuid is installed.
           RegisterProtocolCallback(
                                  &gEfiSimpleTextOutProtocolGuid, 
                                  TerminalHotKeyPostMsgCallBack,
                                  NULL, 
                                  &TerminalHotKeyPostMsgEvent,
                                  &TerminalHotKeyPostMsgRegistration
                                  );
       }
       return FALSE;
    }

    for (Index = 0; Index < HandleCount; Index++) {

        Status = pBS->OpenProtocolInformation (
                                              HandleBuffer[Index],
                                              &gEfiSerialIoProtocolGuid,
                                              &OpenInfoBuffer,
                                              &EntryCount
                                              );
        if (EFI_ERROR (Status)) {
           continue;
        }
        for (Index1 = 0; Index1 < EntryCount; Index1++) {
            if (OpenInfoBuffer[Index1].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) {
                // Queries each handle to determine if it supports
                // gEfiSimpleTextOutProtocolGuid protocol.
                Status = pBS->OpenProtocol ( OpenInfoBuffer[Index1].ControllerHandle,
                                             &gEfiSimpleTextOutProtocolGuid, 
                                             NULL, 
                                             NULL, 
                                             NULL, 
                                             EFI_OPEN_PROTOCOL_TEST_PROTOCOL 
                                             );
                if(Status == EFI_SUCCESS) {
                	TerminalDeviceFound = TRUE;
                	break;
                }
            }
        }
        if(TerminalDeviceFound) {
        	break;
        }
    }
    if(TerminalDeviceFound && !TerminalPostScreenMsg) {
#if (SETUP_ENTRY_SCAN == EFI_SCAN_ESC) || (SETUP_ALT_ENTRY_SCAN == EFI_SCAN_ESC)
        TerminalHotKeyPostMsgtext = HiiGetString( gHiiHandle,\
                                    STRING_TOKEN(STR_ESC_ENTER_SETUP_SREDIR),
                                    NULL
                                    );
#else
        TerminalHotKeyPostMsgtext = HiiGetString( gHiiHandle,\
                                            STRING_TOKEN(STR_DEL_ENTER_SETUP_SREDIR),
                                            NULL
                                            );
#endif
        
        if ( TerminalHotKeyPostMsgtext != NULL ) {
           gAmiPostMgr-> DisplayPostMessage(TerminalHotKeyPostMsgtext);
        }
#if defined(SETUP_BBS_POPUP_ENABLE) && (SETUP_BBS_POPUP_ENABLE == 1)
        TerminalHotKeyPostMsgtext = HiiGetString( gHiiHandle,\
                                          STRING_TOKEN(STR_BBS_POPUP_SREDIR),\
                                          NULL
                                          );
        if ( TerminalHotKeyPostMsgtext != NULL ) {
            gAmiPostMgr-> DisplayPostMessage(TerminalHotKeyPostMsgtext);
        }
#endif
        TerminalPostScreenMsg = TRUE;          // To avoid Displaying Terminal HotKey Post message Twice
    }
    
    // Free Up Resources Allocated
    if(TerminalHotKeyPostMsgtext!= NULL ) {
        pBS->FreePool(TerminalHotKeyPostMsgtext);
        TerminalHotKeyPostMsgtext = NULL;
    }
    if(OpenInfoBuffer!= NULL ) {
        pBS->FreePool(OpenInfoBuffer);
        OpenInfoBuffer = NULL;
    }
    
    return FALSE;
}

/**
    CallBack function Registered to check Terminal's 
    gEfiSimpleTextOutProtocolGuid is installed.


    @param VOID

    @retval BOOLEAN

**/
VOID
EFIAPI
TerminalHotKeyPostMsgCallBack (
    IN EFI_EVENT  Event,
    IN VOID       *Context
)
{
    // Displays Terminal Hot Key Messages to Enter SetUp and For BBS PopUp.
    TerminalHotKeyPostMsg();
    return;
}

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
