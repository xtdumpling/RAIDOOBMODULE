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

/** @file BmcWarmReset.c
    Sends Warm Reset command to BMC.

**/

//----------------------------------------------------------------------

#include <Protocol/IPMITransportProtocol.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Include/ServerMgmtSetupVariable.h>
#include <ServerMgmtSetup.h>
#include <Library/HiiLib.h>

//----------------------------------------------------------------------

#define STRING_BUFFER_LENGTH    4000

/**
    This function sends warm reset command to BMC

    @param This - IpmiTransportProtocol pointer.

    @return EFI_STATUS Return Status.
**/

EFI_STATUS
SendWarmResetCommandToBMC(
 IN EFI_IPMI_TRANSPORT        *This )
{
    EFI_STATUS          Status;
    UINT8               ResponseDataSize = 0x00;

    Status = This->SendIpmiCommand(
                This,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_WARM_RESET,
                NULL,
                0,
                NULL,
                &ResponseDataSize );

    return  Status;
}

/**
    This setup callback function sends warm reset command to BMC
    as per user choice.

    @param HiiHandle A handle that was previously registered in the
                     HII Database.
    @param Class    Formset Class of the Form Callback Protocol passed in
    @param SubClass Formset sub Class of the Form Callback Protocol passed in
    @param Key Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS Return Status

**/

EFI_STATUS
BmcWarmResetCallbackFunction(
 IN  EFI_HII_HANDLE     HiiHandle,
 IN  UINT16             Class,
 IN  UINT16             SubClass,
 IN  UINT16             Key )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_IPMI_TRANSPORT          *IpmiTransport;
    AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr = NULL;
    UINT8                       MsgBoxSel;
    CHAR16                      OutputString[STRING_BUFFER_LENGTH];
    CALLBACK_PARAMETERS         *CallbackParameters;
    CHAR16                      CharPtr[STRING_BUFFER_LENGTH];
    UINTN                       BufferLength = STRING_BUFFER_LENGTH;
    EFI_STATUS                  CharPtrStatus = EFI_SUCCESS;
    EFI_STATUS                  OutputStrStatus = EFI_SUCCESS;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Key Value : %x\n", __FUNCTION__, Key));

    //
    // Continue only for BMC_WARM_RESET_KEY
    //
    if ( Key != BMC_WARM_RESET_KEY ) {
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO,"\n CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n",Status));
        return  EFI_SUCCESS;
    }

    //
    // Locate AmiPostMgrProtocol to display the message boxes in setup
    //
    Status = gBS->LocateProtocol (
                &gAmiPostManagerProtocolGuid,
                NULL,
                (VOID **)&AmiPostMgr);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gAmiPostManagerProtocolGuid locateProtocol Status: %r\n", Status));

    if( EFI_ERROR(Status)) {
        return  Status;
    }

    //
    // Locate IpmiTransport protocol
    //
    Status = gBS->LocateProtocol (
                 &gEfiDxeIpmiTransportProtocolGuid,
                 NULL,
                 (VOID **)&IpmiTransport);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));

    //
    // If locate protocol fails, display error message and return
    //
    if(EFI_ERROR(Status)) {
        OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_PROTOCOL_NOT_FOUND), OutputString, &BufferLength, NULL);
        BufferLength = STRING_BUFFER_LENGTH;
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMC_WARM_RESET_ERROR_INFO), CharPtr, &BufferLength, NULL);
        if((!EFI_ERROR(OutputStrStatus)) && (!EFI_ERROR(CharPtrStatus))) {
            AmiPostMgr->DisplayMsgBox (
                CharPtr,
                OutputString,
                MSGBOX_TYPE_OK,
                &MsgBoxSel );
            return  Status;
         }
    }

    //
    // Get confirmation from the user before doing warm reset
    //
    BufferLength = STRING_BUFFER_LENGTH;
    OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_WARM_RESET_BMC), OutputString, &BufferLength, NULL);
    BufferLength = STRING_BUFFER_LENGTH;
    CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMC_WARM_RESET_INFO), CharPtr, &BufferLength, NULL);
    if((!EFI_ERROR(OutputStrStatus)) && (!EFI_ERROR(CharPtrStatus))) {
        Status = AmiPostMgr->DisplayMsgBox (
                    CharPtr,
                    OutputString,
                    MSGBOX_TYPE_YESNO,
                    &MsgBoxSel );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "DisplayMsgBox return Status: %r\n", Status));
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "MsgBoxSel: %x\n", MsgBoxSel));
    }


    //
    // if user said no, then EFI_SUCCESS
    //
    if((EFI_ERROR(Status)) || MsgBoxSel == MSGBOX_NO) {
        return  EFI_SUCCESS;
    }

    //
    // Send warm reset command to BMC
    //
    Status = SendWarmResetCommandToBMC (IpmiTransport);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " SendWarmResetCommandToBMC Status: %r \n", Status));
    if(EFI_ERROR(Status)) {
        BufferLength = STRING_BUFFER_LENGTH;
        OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMC_WARM_RESET_FAIL), OutputString, &BufferLength, NULL);
        BufferLength = STRING_BUFFER_LENGTH;
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMC_WARM_RESET_ERROR_INFO), CharPtr, &BufferLength, NULL);
        if((!EFI_ERROR(OutputStrStatus)) && (!EFI_ERROR(CharPtrStatus))) {
            AmiPostMgr->DisplayMsgBox (
                CharPtr,
                OutputString,
                MSGBOX_TYPE_OK,
                &MsgBoxSel );
            return  Status;
        }
    }

    //
    // If command success, display the message and return
    //
    BufferLength = STRING_BUFFER_LENGTH;
    OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMC_WARM_RESET_DONE), OutputString, &BufferLength, NULL);
    BufferLength = STRING_BUFFER_LENGTH;
    CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMC_WARM_RESET_INFO), CharPtr, &BufferLength, NULL);
    if((!EFI_ERROR(OutputStrStatus)) && (!EFI_ERROR(CharPtrStatus))) {
        AmiPostMgr->DisplayMsgBox (
            CharPtr,
            OutputString,
            MSGBOX_TYPE_OK,
            &MsgBoxSel );
    }
    return  Status;
}

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
