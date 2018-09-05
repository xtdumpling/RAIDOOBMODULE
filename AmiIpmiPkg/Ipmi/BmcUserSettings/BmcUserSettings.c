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

/** @file BmcUserSettings.c
    Provides functionality to Add/Delete/Change BMC user
    settings.

**/

//----------------------------------------------------------------------

#include "BmcUserSettings.h"

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//
// Global variables
//
EFI_IPMI_TRANSPORT          *gIpmiTransport;
BMC_USER_DETAILS_LIST       gBmcUserDetailsList[MAX_BMC_USER_COUNT];
AMI_POST_MANAGER_PROTOCOL   *gAmiPostMgr = NULL;
UINT8                       gAllowableUser = 0;

BMC_USER_DETAILS            gBmcAddUserDetails;
BMC_USER_DETAILS            gBmcDeleteUserDetails;
BMC_USER_DETAILS            gBmcChangeUserDetails;

//----------------------------------------------------------------------

/**
    Converts Unicode to ASCII Characters.

    @param UnicodeStr The Unicode string to be written to. The buffer must
                      be large enough.
    @param AsciiStr The ASCII string to be converted.

    @retval VOID
 
**/

VOID
IpmiUnicode2Ascii (
  IN      CHAR16         *UnicodeStr,
  IN OUT  CHAR8          *AsciiStr
   )
{

    while (*UnicodeStr != '\0') {
      *(AsciiStr++) = (CHAR8) *(UnicodeStr++);
    }

    *AsciiStr = '\0';

    return;
}

/**
    This function collects number of users allowed BMC.

    @param VOID

    @retval VOID
**/

VOID
GetMaxUserAllowable(
 VOID )
{

    EFI_STATUS                        Status;
    UINT8                             ChannelNumber;
    IPMI_GET_CHANNEL_INFO_RESPONSE    GetChannelInfoResponse = {0};
    EFI_GET_USER_ACCESS_COMMAND       GetUserAccessCommand = {0};
    EFI_GET_USER_ACCESS_RESPONSE      GetUserAccessResponse = {0};
    UINT8                             GetChannelInfoResponseSize;
    UINT8                             GetUserAccessResponseSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));
    for (ChannelNumber = 0; ChannelNumber <= MAX_CHANNEL_NUMBER; ChannelNumber++) {

        GetChannelInfoResponseSize = sizeof (GetChannelInfoResponse);
        Status = gIpmiTransport->SendIpmiCommand(
                          gIpmiTransport,
                          IPMI_NETFN_APP,
                          BMC_LUN,
                          IPMI_APP_GET_CHANNEL_INFO,
                          (UINT8*)&ChannelNumber,
                          sizeof (ChannelNumber),
                          (UINT8*)&GetChannelInfoResponse,
                          &GetChannelInfoResponseSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of Channel %02x Get Channel Info Command: %r \n", ChannelNumber, Status));
        if (!EFI_ERROR(Status)) {
            if (GetChannelInfoResponse.ChannelMediumType == LAN_CHANNEL_TYPE) {
                GetUserAccessCommand.ChannelNo = ChannelNumber;
                GetUserAccessCommand.UserId = 1;
                GetUserAccessResponseSize = sizeof (GetUserAccessResponse);
                Status = gIpmiTransport->SendIpmiCommand(
                                                 gIpmiTransport,
                                                 IPMI_NETFN_APP,
                                                 BMC_LUN,
                                                 IPMI_APP_GET_USER_ACCESS,
                                                 (UINT8 *)&GetUserAccessCommand,
                                                 sizeof (GetUserAccessCommand),
                                                 (UINT8*)&GetUserAccessResponse,
                                                 &GetUserAccessResponseSize );
                SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of Get User Access Command: %r \n", Status));
                if (!EFI_ERROR(Status)) {
                    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Maximum User Id allowable: %04x \n", GetUserAccessResponse.MaxUserId));
                    gAllowableUser = GetUserAccessResponse.MaxUserId;
                    return;
                }
            }
        }
    }
}
/**
    This function collects all user names from BMC.

    @param VOID

    @retval VOID
**/

VOID
CollectAllBmcUserDetails (
  VOID )
{
    UINT8                       i;
    EFI_STATUS                  Status;
    EFI_GET_USER_NAME_COMMAND   GetUserNameCommandData = {0};
    EFI_GET_USER_NAME_RESPONSE  GetUserNameResponseData = {0};
    UINT8                       GetUserNameResponseDataSize;
    CHAR8                       *InvalidUserName = "Invalid";
    CHAR8                       *Reserved = "Reserved";

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));

    //
    // Send Get User Name Command to BMC for all possible User IDs
    //
    GetUserNameResponseDataSize = sizeof (GetUserNameResponseData);

    for(i = 0; i <= gAllowableUser; i++) {
        gBmcUserDetailsList[i].UserId = i;
        if ((i == 0) || (i == 1)) {// User Id 0 & 1 are reserved as per IPMI specification
            CopyMem (gBmcUserDetailsList[i].UserName, Reserved, AsciiStrLen (Reserved));
            continue;
        }

        CopyMem ( gBmcUserDetailsList[i].UserName, InvalidUserName, AsciiStrLen (InvalidUserName));

        GetUserNameCommandData.UserId = i;
        GetUserNameResponseDataSize = sizeof (GetUserNameResponseData);
        ZeroMem (GetUserNameResponseData.UserName, MAX_BMC_USER_NAME_LENGTH );

        Status = gIpmiTransport->SendIpmiCommand(
                                       gIpmiTransport,
                                       IPMI_NETFN_APP,
                                       BMC_LUN,
                                       IPMI_APP_GET_USER_NAME,
                                       (UINT8 *)&GetUserNameCommandData,
                                       sizeof (GetUserNameCommandData),
                                       GetUserNameResponseData.UserName,
                                       &GetUserNameResponseDataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of Get User Name Command: %r \n", Status));
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));

        if (!EFI_ERROR (Status)) {
            //
            // Copy the user name in User Details Global List
            //
            if (GetUserNameResponseData.UserName[0] != 0x00) {
                CopyMem ( gBmcUserDetailsList[i].UserName, GetUserNameResponseData.UserName, sizeof(GetUserNameResponseData));
            } 
        }
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User Id: %x \n", gBmcUserDetailsList[i].UserId));
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User Name: %s \n", gBmcUserDetailsList[i].UserName));
    }

    return;
}

/**
    Checks the given user name already available in global user list

    @param UserDetails Contains user name

    @retval TRUE User already available.
    @retval FALSE User not available.
**/

BOOLEAN
CheckUserNameAvailability (
  IN OUT BMC_USER_DETAILS   *UserDetails )
{
    UINT8       i;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: UserName: %s", UserDetails->UserName));

    for (i = 0; i <= gAllowableUser; i++) {

        if (AsciiStrCmp (gBmcUserDetailsList[i].UserName, UserDetails->UserName) == 0x00) {
            UserDetails->UserId = gBmcUserDetailsList[i].UserId;
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Available UserId: %x", UserDetails->UserId));
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning TRUE from %a\n", __FUNCTION__));
            return TRUE;
        }
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning FALSE from %a\n", __FUNCTION__));
    return FALSE;
}

/**
    Checks add new user is possible or not.

    @param VOID

    @retval TRUE Add new user is possible.
    @retval FALSE Add new user is not possible.
**/

BOOLEAN
CheckAddNewUserPossibility (
  VOID )
{
    UINT8           i;
    CHAR8           *InvalidUserName = "Invalid";

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));
    for (i = 0; i <= gAllowableUser; i++){
        if (AsciiStrCmp (gBmcUserDetailsList[i].UserName, InvalidUserName) == 0x00) {
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning TRUE from %a\n", __FUNCTION__));
            return TRUE;
        }
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning FALSE from %a\n", __FUNCTION__));
    return FALSE;
}

/**
    Send Set User Name command to BMC.

    @param UserId User Id.
    @param UserName User Name.

    @return EFI_STATUS
**/

EFI_STATUS
SendSetUserNameCommandToBmc(
  IN UINT8      UserId,
  IN CHAR8      *UserName)
{
    EFI_STATUS                      Status;
    EFI_SET_USER_NAME_COMMAND       SetUserName = {0};
    UINT8                           ResponseDataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User Id: %x\n", UserId));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User Name: %s\n", UserName));

    SetUserName.UserId = UserId;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: AsciiStrLen(UserName): %x\n", AsciiStrLen(UserName)));
    CopyMem (&SetUserName.UserName, UserName, AsciiStrLen (UserName));
    ResponseDataSize = 0;

    Status = gIpmiTransport->SendIpmiCommand(
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_USER_NAME,
                (UINT8 *)&SetUserName,
                sizeof (SetUserName),
                NULL,
                &ResponseDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Status of Set User Name Command: %r \n", Status));

    return Status; 
}

/**
    Delete user details from global list.

    @param UserName User Name.

    @retval EFI_SUCCESS User details successfully deleted from list.
    @retval EFI_NOT_FOUND User is not available in list.
**/

EFI_STATUS
DeleteUserEntryFromList(
  IN CHAR8      *UserName )
{
    UINT8           i;
    CHAR8           *InvalidUserName = "Invalid";

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));
    for (i = 0; i <= gAllowableUser; i++) {

        if (AsciiStrCmp (gBmcUserDetailsList[i].UserName, UserName) == 0x00) {
            AsciiStrCpy (gBmcUserDetailsList[i].UserName, InvalidUserName);
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: gBmcUserDetailsList[%x].UserName: %s\n", i, gBmcUserDetailsList[i].UserName));
            return EFI_SUCCESS;
        }

    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return EFI_NOT_FOUND;
}

/**
    Add user details to global list.

    @param UserDetails User details.

    @retval VOID
**/

VOID
AddUserEntryToList(
  IN BMC_USER_DETAILS      *UserDetails )
{
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));

    gBmcUserDetailsList[UserDetails->UserId].UserId = UserDetails->UserId;
    CopyMem ( gBmcUserDetailsList[UserDetails->UserId].UserName, UserDetails->UserName, sizeof(UserDetails->UserName));

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: gBmcUserDetailsList[UserDetails->UserId].UserId:%x\n", gBmcUserDetailsList[UserDetails->UserId].UserId));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: gBmcUserDetailsList[UserDetails->UserId].UserName:%s\n", gBmcUserDetailsList[UserDetails->UserId].UserName));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));

}

/**
    Parse the global list and get the available used id for new user.

    @param UserId User Id.

    @retval EFI_SUCCESS ID is available.
    @retval EFI_NOT_FOUND No more id is available.
**/

EFI_STATUS
GetAvailableUserId(
  IN OUT UINT8      *UserId)
{
    UINT8       i;
    CHAR8       *InvalidUserName = "Invalid";

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));

    for (i = 0; i <= gAllowableUser; i++){
        if (AsciiStrCmp (gBmcUserDetailsList[i].UserName, InvalidUserName) == 0x00) {
            *UserId = gBmcUserDetailsList[i].UserId;
            SERVER_IPMI_DEBUG ((EFI_D_INFO,"IPMI: User Id: %x", *UserId));
            SERVER_IPMI_DEBUG ((EFI_D_INFO,"IPMI: Returning EFI_SUCCESS from %a\n", __FUNCTION__));
            return EFI_SUCCESS;
        }
    }
    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Returning EFI_NOT_FOUND from %a\n", __FUNCTION__));
    return  EFI_NOT_FOUND;
}

/**
    Displays error message pop up in setup.

    @param HiiHandle - Handle to HII database.
    @param Message - Error message token value.

    @retval VOID
**/

VOID
DisplayErrorMessagePopUp(
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Message )
{
    UINT8       MsgBoxSel;
    CHAR16     CharPtr[STRING_BUFFER_LENGTH];
    CHAR16     MessagePtr[STRING_BUFFER_LENGTH];
    EFI_STATUS CharPtrStatus;
    EFI_STATUS MessagePtrStatus;
    UINTN      BufferLength = STRING_BUFFER_LENGTH;

    CharPtrStatus = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_BMC_USER_SETTINGS_INFO), CharPtr, &BufferLength, NULL);
    BufferLength = STRING_BUFFER_LENGTH;
    MessagePtrStatus = IpmiHiiGetString ( HiiHandle, Message, MessagePtr, &BufferLength, NULL);
    if ( (!EFI_ERROR(CharPtrStatus)) && (!EFI_ERROR(MessagePtrStatus))) {
        gAmiPostMgr->DisplayMsgBox (
            CharPtr,
            MessagePtr,
            MSGBOX_TYPE_OK,
            &MsgBoxSel );
    }
    return;
}

/**
    Send Set user password command as per input data.

    @param UserId User Id.
    @param PasswordSize Password size(16 or 20 byte).
    @param Operation Tells what operation to perform.
    @param UserPassword Password.

    @return EFI_STATUS
**/

EFI_STATUS
PerformGivenUserPasswordAction(
  IN UINT8      UserId,
  IN UINT8      PasswordSize,
  IN UINT8      Operation,
  IN CHAR8      *UserPassword )
{
    EFI_SET_USER_PASSWORD_COMMAND_OF_SIZE_TWENTY        SetUserPassword = {0};
    EFI_STATUS                                          Status;
    UINT8                                               DataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));

    ZeroMem (SetUserPassword.PasswordData, sizeof (SetUserPassword.PasswordData));
    CopyMem (SetUserPassword.PasswordData, UserPassword, AsciiStrLen(UserPassword));

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: UserId: %x\n", UserId));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: IsPasswordSize20: %x\n", PasswordSize));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Operation: %x\n", Operation));

    SetUserPassword.UserId = UserId;
    SetUserPassword.PasswordSize = PasswordSize;
    SetUserPassword.Operation = Operation;
    DataSize = 0;

    Status = gIpmiTransport->SendIpmiCommand(
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_USER_PASSWORD,
                (UINT8 *)&SetUserPassword,
                sizeof (SetUserPassword),
                NULL,
                &DataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Set User Password Command Status: %r\n", Status));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));

    if((!EFI_ERROR(Status)) && (gIpmiTransport->CommandCompletionCode == 0x80)) {
        return EFI_INVALID_PARAMETER;
    }
    return Status;
}

/**
    Deletes user name from BMC.

    @param UserId User Id.

    @return EFI_STATUS
**/

EFI_STATUS
DeleteUserNameFromBmc(
  IN UINT8      UserId )
{
    EFI_STATUS                  Status;
    CHAR8                       DeleteUserName[MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER];

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User Id: %x\n", UserId));

    SetMem (DeleteUserName, MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER, IPMI_BMC_DELETE_USER_DATA);
    Status = SendSetUserNameCommandToBmc ( UserId, DeleteUserName);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Set User Name Command Status: %r\n", Status));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return Status;

}

/**
    Displays text box in setup to get user password.

    @param HiiHandle HII Handle.
    @param TitleToken Display title token.
    @param UserPassword User password.

    @retval VOID
**/

VOID
DisplayTextBox(
  IN  VOID        *HiiHandle,
  IN  UINT16      TitleToken,
  IN  CHAR16      **UserPassword )
{
    EFI_STATUS              Status;
    TEXT_INPUT_TEMPLATE     BmcUserPasswordPrompt = {0};

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Inside %a\n", __FUNCTION__));

    //
    // Fill the Text input template
    //
    BmcUserPasswordPrompt.ItemToken = STR_BMC_USER_PASSWORD;
    BmcUserPasswordPrompt.MaxSize = BMC_USER_PASSWORD_MAX_SIZE;
    BmcUserPasswordPrompt.FilterKey = TSE_FILTER_KEY_NULL;
    BmcUserPasswordPrompt.Hidden = TRUE;
    BmcUserPasswordPrompt.Value = *UserPassword;

    //
    // Call DisplayTextBox() function to display the text box in setup and
    // get the Secret string.
    //
    Status = gAmiPostMgr->DisplayTextBox(
                HiiHandle,
                TitleToken,
                &BmcUserPasswordPrompt,
                1,
                NULL );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of DisplayTextBox: %r\n", Status));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));

    return;

}

/**
    This function collects all BMC user details from BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcUserSettingsCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    CALLBACK_PARAMETERS         *CallbackParameters;
    STATIC BOOLEAN              IsFirstTimeCallBack = TRUE;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x\n", __FUNCTION__, Key));

    //
    // Continue only for BMC_USER SETTING_KEY
    //
    if ( Key != BMC_USER_SETTINGS_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action: %x\n", CallbackParameters->Action));
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: IsFirstTimeCallBack: %x\n", IsFirstTimeCallBack));
#if COLLECT_BMC_USER_DETAILS_FOR_EVERY_BMC_USER_SETTING_CALLBACK == 0
    if (IsFirstTimeCallBack == FALSE) {
        return EFI_SUCCESS;
    }
#endif

    //
    // Locate AmiPostMgrProtocol to display the message boxes in setup 
    //
    Status = gBS->LocateProtocol (
                &gAmiPostManagerProtocolGuid,
                NULL,
                (VOID **)&gAmiPostMgr);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: gAmiPostManagerProtocolGuid locateProtocol Status: %r\n", Status));

    if(EFI_ERROR (Status)) {
        return  Status;
    }

    //
    // Locate IpmiTransport protocol
    //
    Status = gBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI: gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));

    //
    // If locate protocol fails, display error message and return
    //
    if(EFI_ERROR (Status)) {
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_IPMI_PROTOCOL_ERROR) );
        return Status;
    }

    //
    // Collect maximum user allowable 
    //
    GetMaxUserAllowable ();

    if(IsFirstTimeCallBack) {
        CollectAllBmcUserDetails ();
    } else {
#if COLLECT_BMC_USER_DETAILS_FOR_EVERY_BMC_USER_SETTING_CALLBACK
        CollectAllBmcUserDetails ();
#endif
    }

    IsFirstTimeCallBack = FALSE;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return  EFI_SUCCESS;
}

/**
    This function validates whether add new user is possible or not.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcUserSettingsAddUserCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    EFI_STATUS                         Status = EFI_SUCCESS;
    CALLBACK_PARAMETERS                *CallbackParameters;
    BOOLEAN                            IsAddNewUserPossible;
    SERVER_MGMT_CONFIGURATION_DATA     *ServerMgmtConfiguration = NULL;
    UINTN                               SelectionBufferSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x\n", __FUNCTION__, Key));

    //
    // Continue only for BMC_ADD USER KEY
    //
    if (Key != ADD_BMC_USER_KEY) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action: %x\n", CallbackParameters->Action));
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    IsAddNewUserPossible = CheckAddNewUserPossibility ();

    if(IsAddNewUserPossible == FALSE) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Add New User Not Possible!!!\n"));
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_NEW_USER_ADD_ERROR) );
        *(CallbackParameters->ActionRequest) = EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT;
        return  EFI_SUCCESS;
    }
    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                   EfiBootServicesData,
                   SelectionBufferSize,
                   (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                   &SelectionBufferSize,
                   ServerMgmtConfiguration,
                   &gEfiServerMgmtSetupVariableGuid,
                   L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    ZeroMem (ServerMgmtConfiguration->AddBmcUserName, MAX_BMC_USER_NAME_LENGTH);
    ServerMgmtConfiguration->AddBmcChannelNo = 0;
    ServerMgmtConfiguration->AddBmcUserPrivilegeLimit  = 0;

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                   SelectionBufferSize,
                   ServerMgmtConfiguration,
                   &gEfiServerMgmtSetupVariableGuid,
                   L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));

    FreePool (ServerMgmtConfiguration);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return  EFI_SUCCESS;
}

/**
    This function send set user name to BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcAddUserNameCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    BOOLEAN                             UserNameAlreadyAvailable;
    CALLBACK_PARAMETERS                 *CallbackParameters;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if (Key != ADD_BMC_USER_NAME_KEY) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO,"IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    //
    // Convert User name format from UNICODE to ASCII
    //
    ZeroMem (gBmcAddUserDetails.UserName, MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER);
    IpmiUnicode2Ascii (ServerMgmtConfiguration->AddBmcUserName, gBmcAddUserDetails.UserName);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: AddUserName: %s\n", gBmcAddUserDetails.UserName));

    //
    // Check the user name is already available in User Details list
    //
    UserNameAlreadyAvailable = CheckUserNameAvailability (&gBmcAddUserDetails);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: UserNameAlreadyAvailable:%x\n", UserNameAlreadyAvailable));

    if(UserNameAlreadyAvailable) {
        //
        // Display User Name already available pop up message
        //
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_NAME_EXISTS));
        ZeroMem (ServerMgmtConfiguration->AddBmcUserName, MAX_BMC_USER_NAME_LENGTH);
        ZeroMem (&gBmcAddUserDetails, sizeof (gBmcAddUserDetails.UserName));
        ServerMgmtConfiguration->AddBmcChannelNo = 0; 
        ServerMgmtConfiguration->AddBmcUserPrivilegeLimit  = 0; 
    } else {
        Status = GetAvailableUserId (&(gBmcAddUserDetails.UserId));
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of GetAvailableUserId Function = %r\n", Status));
        if (EFI_ERROR (Status)) {
            DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_ADD_NOT_POSSIBLE));
        } else {
            //
            // Send Set User Name Command
            //
            Status = SendSetUserNameCommandToBmc (gBmcAddUserDetails.UserId, gBmcAddUserDetails.UserName);
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Set User Name Command Status: %r\n", Status));
            if (EFI_ERROR (Status)){
                DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_SET_USER_NAME_CMD_FAIL) );
            }
        }
        if (EFI_ERROR (Status)) {
            ZeroMem (ServerMgmtConfiguration->AddBmcUserName, MAX_BMC_USER_NAME_LENGTH);
            ZeroMem (&gBmcAddUserDetails, sizeof (BMC_USER_DETAILS));
            ServerMgmtConfiguration->AddBmcChannelNo = 0;
            ServerMgmtConfiguration->AddBmcUserPrivilegeLimit = 0;
        }
    }

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));

    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function adds user password to BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS

**/
EFI_STATUS
BmcAddUserPasswordCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != ADD_BMC_USER_PASSWORD_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    //
    // Convert User Password format from UNICODE to ASCII
    //
    ZeroMem (gBmcAddUserDetails.UserPassword, MAX_BMC_USER_PASSWORD_LENGTH_WITH_NULL_CHARACTER);
    IpmiUnicode2Ascii (ServerMgmtConfiguration->AddBmcUserPassword, gBmcAddUserDetails.UserPassword);

    Status = PerformGivenUserPasswordAction (gBmcAddUserDetails.UserId, TwentyByte, SetPassword, (CHAR8 *)gBmcAddUserDetails.UserPassword);
    if( EFI_ERROR (Status) ) {

        //
        // Display User Name already available pop up message
        //
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_SET_USER_PASSWORD_FAIL) );
        ZeroMem (ServerMgmtConfiguration->AddBmcUserPassword, MAX_BMC_USER_PASSWORD_LENGTH);
        ZeroMem (&gBmcAddUserDetails.UserPassword, sizeof (gBmcAddUserDetails.UserPassword));
    } else {
        AddUserEntryToList (&gBmcAddUserDetails);
        ZeroMem (ServerMgmtConfiguration->AddBmcUserPassword, MAX_BMC_USER_PASSWORD_LENGTH); //Clearing the password from browser data
    }

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));
    FreePool (ServerMgmtConfiguration);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return EFI_SUCCESS;
}

/**
    This function sends get channel access command to BMC for given input.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcAddUserChannelCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;
    IPMI_GET_CHANNEL_ACCESS_REQUEST     GetChannelAccessCommand = {0};
    IPMI_GET_CHANNEL_ACCESS_RESPONSE    GetChannelAccessResponse = {0};
    UINT8                               GetChannelAccessResponseDataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));
    //
    // Check for the key and Return if Key value does not match
    //
    if (Key != ADD_BMC_USER_CHANNEL_NO_KEY) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO,"IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User entered channel no = %x\n", ServerMgmtConfiguration->AddBmcChannelNo));

    GetChannelAccessCommand.ChannelNo = ServerMgmtConfiguration->AddBmcChannelNo;
    GetChannelAccessCommand.MemoryType = NonVolatile;// Non Volatile channel Access
    GetChannelAccessResponseDataSize = sizeof (GetChannelAccessResponse);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_CHANNEL_ACCESS,
                (UINT8 *)&GetChannelAccessCommand,
                sizeof (GetChannelAccessCommand),
                (UINT8 *)&GetChannelAccessResponse,
                &GetChannelAccessResponseDataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of IPMI_APP_GET_CHANNEL_ACCESS Command: %r \n", Status));

    if ((EFI_ERROR (Status)) || (gIpmiTransport->CommandCompletionCode == 0x82)) {
        DisplayErrorMessagePopUp (HiiHandle, STRING_TOKEN(STR_GET_CHANNEL_ACCESS_CMD_FAIL));
        ServerMgmtConfiguration->AddBmcChannelNo = 0;
    } else {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: GetChannelAccessResponse.ChannelPriviledgeLimit: %x \n", GetChannelAccessResponse.ChannelPriviledgeLimit));
        ServerMgmtConfiguration->AddBmcUserMaximumPossibleUserPrivilegeLevel = GetChannelAccessResponse.ChannelPriviledgeLimit;
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: AddBmcUserMaximumPossibleUserPrivilegeLevel(Before Set Browser data) = %x\n", ServerMgmtConfiguration->AddBmcUserMaximumPossibleUserPrivilegeLevel));

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: AddBmcUserMaximumPossibleUserPrivilegeLevel(After Set Browser data) = %x\n", ServerMgmtConfiguration->AddBmcUserMaximumPossibleUserPrivilegeLevel));

    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function sends set user access command to BMC for given input.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcAddUserChannelPrivilegeLimitCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;
    EFI_SET_USER_ACCESS_COMMAND         SetUserAccess = {0};
    UINT8                               DataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if (Key != ADD_BMC_USER_PRIVILEGE_LIMIT_KEY) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup");
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: AddBmcUserPrivilegeLimit = %x\n", ServerMgmtConfiguration->AddBmcUserPrivilegeLimit));

    //
    // Byte1
    //
    SetUserAccess.ChannelNo = ServerMgmtConfiguration->AddBmcChannelNo;
    SetUserAccess.EnableIpmiMessaging = 1;
    SetUserAccess.EnableUserLinkAuthetication = 1;
    SetUserAccess.UserRestrictedToCallback = 0;
    SetUserAccess.EnableChangeBitsInByte1 = 1;

    //
    // Byte2
    //
    SetUserAccess.UserID = gBmcAddUserDetails.UserId;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: SetUserAccess.UserID = %x\n", SetUserAccess.UserID));

    //
    // Byte3
    //
    SetUserAccess.UserPrivilegeLimit = ServerMgmtConfiguration->AddBmcUserPrivilegeLimit;

    //
    // Byte4
    //
    SetUserAccess.UserSimultaneousSessionLimit = 0;
    DataSize = 0;

    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_USER_ACCESS,
                (UINT8 *)&SetUserAccess,
                sizeof (SetUserAccess),
                NULL,
                &DataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of IPMI_APP_SET_USER_ACCESS Command: %r \n", Status));

    if(EFI_ERROR (Status)) {
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_SET_USER_ACCESS_CMD_FAIL) );
        ServerMgmtConfiguration->AddBmcUserPrivilegeLimit = 0;

        //
        // Set Browser DATA
        //
        Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));
        FreePool (ServerMgmtConfiguration);
        return EFI_SUCCESS;
    }

    DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_SET_USER_ACCESS_CMD_PASS) );
    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function Clears previous NVRAM value before performing delete user action.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcDeleteUserCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    EFI_STATUS                         Status = EFI_SUCCESS;
    CALLBACK_PARAMETERS                *CallbackParameters;
    SERVER_MGMT_CONFIGURATION_DATA     *ServerMgmtConfiguration = NULL;
    UINTN                              SelectionBufferSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key Value: %x\n", __FUNCTION__, Key));

    //
    // Continue only for BMC_ADD USER KEY
    //
    if ( Key != DELETE_BMC_USER_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action: %x\n", CallbackParameters->Action));
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                   EfiBootServicesData,
                   SelectionBufferSize,
                   (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                   &SelectionBufferSize,
                   ServerMgmtConfiguration,
                   &gEfiServerMgmtSetupVariableGuid,
                   L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    ZeroMem (ServerMgmtConfiguration->DeleteBmcUserName, MAX_BMC_USER_NAME_LENGTH); 

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                   SelectionBufferSize,
                   ServerMgmtConfiguration,
                   &gEfiServerMgmtSetupVariableGuid,
                   L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));

    FreePool (ServerMgmtConfiguration);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return  EFI_SUCCESS;
}

/**
    This function gets the user name for deleting a user from BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcDeleteUserNameCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    BOOLEAN                             UserNameAlreadyAvailable;
    CALLBACK_PARAMETERS                 *CallbackParameters;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != DELETE_BMC_USER_NAME_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    //
    // Convert User name format from UNICODE to ASCII
    //
    ZeroMem (gBmcDeleteUserDetails.UserName, MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER);
    IpmiUnicode2Ascii (ServerMgmtConfiguration->DeleteBmcUserName, gBmcDeleteUserDetails.UserName);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: gBmcDeleteUserDetails.UserName = %s\n", gBmcDeleteUserDetails.UserName));

    //
    // Check the user name is already available in User Details list
    //
    UserNameAlreadyAvailable = CheckUserNameAvailability (&gBmcDeleteUserDetails);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: UserNameAlreadyAvailable:%x\n", UserNameAlreadyAvailable));

    if( !UserNameAlreadyAvailable ) {

        //
        // Display User Name not available pop up message
        //
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_NAME_NOT_AVAILABLE) );
        ZeroMem (ServerMgmtConfiguration->DeleteBmcUserName, MAX_BMC_USER_NAME_LENGTH);
        ZeroMem (&gBmcDeleteUserDetails, sizeof (gBmcDeleteUserDetails));

        //
        // Set Browser DATA
        //
        Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));
    }

    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function gets the user password for deleting a user from BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
BmcDeleteUserPasswordCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA     *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;
    TEXT_INPUT_TEMPLATE                 BmcDeleteUserPasswordPrompt = {0};
    CHAR16                              *UserPassword = NULL;
    CHAR16                              Password[MAX_BMC_USER_PASSWORD_LENGTH];

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != DELETE_BMC_USER_PASSWORD_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR,"IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    //
    // Call DisplayTextBox function to get user password
    //
    UserPassword = Password;
    ZeroMem (UserPassword, MAX_BMC_USER_PASSWORD_LENGTH);
    DisplayTextBox (HiiHandle, STR_BMC_USER_PASSWORD, (CHAR16 **)&UserPassword );
    IpmiUnicode2Ascii ((CHAR16 *)UserPassword, gBmcDeleteUserDetails.UserPassword);

    if ( ( (AsciiStrLen (gBmcDeleteUserDetails.UserPassword) < BMC_USER_PASSWORD_MIN_SIZE) || \
           (AsciiStrLen (gBmcDeleteUserDetails.UserPassword) > BMC_USER_PASSWORD_MAX_SIZE) ) ) {
        //
        // Display User Name not available pop up message
        //
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_INVALID_SIZE) );
        ZeroMem (gBmcDeleteUserDetails.UserPassword, MAX_BMC_USER_PASSWORD_LENGTH_WITH_NULL_CHARACTER);
        return EFI_INVALID_PARAMETER;
    }

    Status = PerformGivenUserPasswordAction ( gBmcDeleteUserDetails.UserId, TwentyByte, TestPassword, gBmcDeleteUserDetails.UserPassword);
    if (EFI_ERROR (Status)){
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_MISMATCH) );
        return Status;
    }

    Status = DeleteUserNameFromBmc (gBmcDeleteUserDetails.UserId);
    if ( EFI_ERROR(Status)){
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_NAME_DELETE_FAIL) );
        return Status;
    }

    Status = DeleteUserEntryFromList (gBmcDeleteUserDetails.UserName);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "\nStatus of DeleteUserEntryFromList = %r\n", Status));
    if (EFI_ERROR (Status)){
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_NAME_DELETE_FAIL) );
        return Status;
    }

    DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_DELETED) );

    ZeroMem (&gBmcDeleteUserDetails, sizeof (gBmcDeleteUserDetails));
    ZeroMem (ServerMgmtConfiguration->DeleteBmcUserName, MAX_BMC_USER_NAME_LENGTH);

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));

    FreePool (ServerMgmtConfiguration);

    return EFI_SUCCESS;
}

/**
    This function load default values to controls.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
ChangeUserSettingsCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA     *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != CHANGE_BMC_USER_SETTINGS_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

        ZeroMem (ServerMgmtConfiguration->ChangeUserSettingsBmcCurrentUserName, MAX_BMC_USER_NAME_LENGTH);
        ServerMgmtConfiguration->ChangeUserSettingsValidBmcUser = 0;
        ServerMgmtConfiguration->ChangeUserSettingsBmcUser = 0;
        ServerMgmtConfiguration->ChangeUserSettingsValidBmcUserCredentials = 0;
        ServerMgmtConfiguration->ChangeUserSettingsBmcChannelNo = 0;
        ServerMgmtConfiguration->ChangeUserSettingsBmcUserMaximumPossibleUserPrivilegeLevel = 0;
        ServerMgmtConfiguration->ChangeUserSettingsBmcUserPrivilegeLimit = 0;

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));

    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function gets the current user name for changing a user details from BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
ChangeUserSettingsCurrentBmcUserNameCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA     *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    BOOLEAN                             UserNameAlreadyAvailable;
    CALLBACK_PARAMETERS                 *CallbackParameters;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != CHANGE_USER_SETTINGS_BMC_CURRENT_USER_NAME_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n",Status));
    ASSERT_EFI_ERROR (Status);

    //
    // Convert User name format from UNICODE to ASCII
    //
    ZeroMem (gBmcChangeUserDetails.UserName, MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER);
    IpmiUnicode2Ascii (ServerMgmtConfiguration->ChangeUserSettingsBmcCurrentUserName, gBmcChangeUserDetails.UserName);

    //
    // Check the user name is already available in User Details list
    //
    UserNameAlreadyAvailable = CheckUserNameAvailability (&gBmcChangeUserDetails);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: UserNameAlreadyAvailable:%x\n", UserNameAlreadyAvailable));

    if( !UserNameAlreadyAvailable ) {
        //
        // Display User Name not available pop up message
        //
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_NAME_NOT_FOUND) );
        ZeroMem (ServerMgmtConfiguration->ChangeUserSettingsBmcCurrentUserName, MAX_BMC_USER_NAME_LENGTH);
        ZeroMem (gBmcChangeUserDetails.UserName, sizeof (gBmcChangeUserDetails.UserName));
        ServerMgmtConfiguration->ChangeUserSettingsValidBmcUser = 0;
        ServerMgmtConfiguration->ChangeUserSettingsValidBmcUserCredentials = 0;
     
    } else {
        ServerMgmtConfiguration->ChangeUserSettingsValidBmcUser = 1;
        ServerMgmtConfiguration->ChangeUserSettingsValidBmcUserCredentials = 0;
    }

    ServerMgmtConfiguration->ChangeUserSettingsBmcChannelNo = 0;
    ServerMgmtConfiguration->ChangeUserSettingsBmcUserMaximumPossibleUserPrivilegeLevel = 0;
    ServerMgmtConfiguration->ChangeUserSettingsBmcUserPrivilegeLimit = 0;
    ServerMgmtConfiguration->ChangeUserSettingsBmcUser = 0;
    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));

    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function gets the current user password for changing a user details from BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
ChangeUserSettingsCurrentBmcUserPasswordCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;
    TEXT_INPUT_TEMPLATE                 ChangeUserSettingsCurrentBmcUserPasswordPrompt = {0};
    CHAR16                              *UserPassword;
    CHAR16                              Password[MAX_BMC_USER_PASSWORD_LENGTH];

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != CHANGE_USER_SETTINGS_BMC_USER_CURRENT_PASSWORD_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    //
    // Call DisplayTextBox function to get user password
    //
    UserPassword = Password;
    ZeroMem (UserPassword, MAX_BMC_USER_PASSWORD_LENGTH);
    DisplayTextBox (HiiHandle, STR_BMC_USER_PASSWORD, (CHAR16 **)&UserPassword );
    IpmiUnicode2Ascii ((CHAR16 *)UserPassword, gBmcChangeUserDetails.UserPassword);

    if ( ( (AsciiStrLen (gBmcChangeUserDetails.UserPassword) < BMC_USER_PASSWORD_MIN_SIZE) || \
           (AsciiStrLen (gBmcChangeUserDetails.UserPassword) > BMC_USER_PASSWORD_MAX_SIZE) ) ) {
        //
        // Display User Name not available pop up message
        //
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_INVALID_USER_PASSWORD) );
        ZeroMem (gBmcChangeUserDetails.UserPassword, sizeof (gBmcChangeUserDetails.UserPassword));
        return EFI_INVALID_PARAMETER;
    }

    Status = PerformGivenUserPasswordAction ( gBmcChangeUserDetails.UserId, TwentyByte, TestPassword, gBmcChangeUserDetails.UserPassword);
    if (EFI_ERROR (Status)){
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_MISMATCH) );
        ZeroMem (gBmcChangeUserDetails.UserPassword, sizeof (gBmcChangeUserDetails.UserPassword));
        return Status;
    } else {
        ServerMgmtConfiguration->ChangeUserSettingsValidBmcUserCredentials = 1;
    }

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "\nStatus of HiiLibSetBrowserData() = %r\n",Status));

    pBS->FreePool (ServerMgmtConfiguration);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return EFI_SUCCESS;
}

/**
    This function gets new user password for changing a user details in BMC.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
ChangeUserSettingsBmcUserPasswordCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != CHANGE_USER_SETTINGS_BMC_USER_PASSWORD_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    //
    // Convert User Password format from UNICODE to ASCII
    //
    ZeroMem (gBmcChangeUserDetails.UserPassword, MAX_BMC_USER_PASSWORD_LENGTH_WITH_NULL_CHARACTER);
    IpmiUnicode2Ascii (ServerMgmtConfiguration->ChangeUserSettingsBmcUserPassword, gBmcChangeUserDetails.UserPassword);

    Status = PerformGivenUserPasswordAction (gBmcChangeUserDetails.UserId, TwentyByte, SetPassword, (CHAR8 *)gBmcChangeUserDetails.UserPassword);
    if( EFI_ERROR (Status) ) {

        //
        // Display User Name already available pop up message
        //
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_SET_USER_PASSWORD_FAIL) );
        ZeroMem (ServerMgmtConfiguration->ChangeUserSettingsBmcUserPassword, MAX_BMC_USER_PASSWORD_LENGTH);
        ZeroMem (&gBmcChangeUserDetails.UserPassword, sizeof(gBmcChangeUserDetails.UserPassword));
    } else {
        ZeroMem (ServerMgmtConfiguration->ChangeUserSettingsBmcUserPassword, MAX_BMC_USER_PASSWORD_LENGTH); //Clearing the password from browser data
    }

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));
    FreePool (ServerMgmtConfiguration);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Returning from %a\n", __FUNCTION__));
    return EFI_SUCCESS;
}

/**
    This function enables/disables a user in BMC as per given input.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
ChangeUserSettingsEnableOrDisableBmcUserCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( Key != CHANGE_USER_SETTINGS_BMC_USER_KEY ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters();
    if ( CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "\n CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "\n ServerMgmtConfiguration->ChangeUserSettingsBmcUser: %x\n", ServerMgmtConfiguration->ChangeUserSettingsBmcUser));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI:gBmcChangeUserDetails.UserId %x\n", gBmcChangeUserDetails.UserId));
    Status = PerformGivenUserPasswordAction( gBmcChangeUserDetails.UserId, TwentyByte, ServerMgmtConfiguration->ChangeUserSettingsBmcUser, gBmcChangeUserDetails.UserPassword);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User Enable/Disable Status: %r\n", Status));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));
    if (EFI_ERROR (Status)) {
        if (ServerMgmtConfiguration->ChangeUserSettingsBmcUser) {
            DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_ENABLE_OPERATION_FAIL) );
        } else {
            DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_DISABLE_OPERATION_FAIL) );
        }
        ServerMgmtConfiguration->ChangeUserSettingsBmcUser = 0;

        //
        // Set Browser DATA
        //
        Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "\nStatus of HiiLibSetBrowserData() = %r\n", Status));
    } else {
        if (ServerMgmtConfiguration->ChangeUserSettingsBmcUser) {
            DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_ENABLE_OPERATION_PASS) );
        } else {
            DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_USER_DISABLE_OPERATION_PASS) );
        }
    }

    pBS->FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function sends get channel access command for the given channel and
    determines the maximum possible privilege level for the channel.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
ChangeUserSettingsBmcUserChannelCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;
    IPMI_GET_CHANNEL_ACCESS_REQUEST     GetChannelAccessCommand = {0};
    IPMI_GET_CHANNEL_ACCESS_RESPONSE    GetChannelAccessResponse = {0};
    UINT8                               GetChannelAccessResponseDataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if (Key != CHANGE_USER_SETTINGS_BMC_USER_CHANNEL_NO_KEY) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Get Browser Data
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: User entered channel no = %x\n", ServerMgmtConfiguration->ChangeUserSettingsBmcChannelNo));

    GetChannelAccessCommand.ChannelNo = ServerMgmtConfiguration->ChangeUserSettingsBmcChannelNo;
    GetChannelAccessCommand.MemoryType = NonVolatile;// Non Volatile channel Access
    GetChannelAccessResponseDataSize = sizeof (GetChannelAccessResponse);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_CHANNEL_ACCESS,
                (UINT8 *)&GetChannelAccessCommand,
                sizeof (GetChannelAccessCommand),
                (UINT8 *)&GetChannelAccessResponse,
                &GetChannelAccessResponseDataSize);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of IPMI_APP_GET_CHANNEL_ACCESS Command: %r \n", Status));

    if(EFI_ERROR(Status)) {
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_GET_CHANNEL_ACCESS_CMD_FAIL) );
        ServerMgmtConfiguration->ChangeUserSettingsBmcChannelNo = 0;
    } else {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: GetChannelAccessResponse.ChannelPriviledgeLimit: %x \n", GetChannelAccessResponse.ChannelPriviledgeLimit));
        ServerMgmtConfiguration->ChangeUserSettingsBmcUserMaximumPossibleUserPrivilegeLevel = GetChannelAccessResponse.ChannelPriviledgeLimit;
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: AddBmcUserMaximumPossibleUserPrivilegeLevel(Before Set Browser data) = %x\n", ServerMgmtConfiguration->ChangeUserSettingsBmcUserMaximumPossibleUserPrivilegeLevel));

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: AddBmcUserMaximumPossibleUserPrivilegeLevel(After Set Browser data) = %x\n", ServerMgmtConfiguration->ChangeUserSettingsBmcUserMaximumPossibleUserPrivilegeLevel));

    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
}

/**
    This function sets the privilege level for a user as per the given input.

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Class of the Form Callback Protocol passed in
    @param SubClass - Sub Class of the Form Callback Protocol passed in
    @param Key - Key of the Form Callback Protocol passed in

    @return EFI_STATUS
**/

EFI_STATUS
ChangeUserSettingsBmcUserChannelPrivilegeLimitCallbackFunction(
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CALLBACK_PARAMETERS                 *CallbackParameters;
    EFI_SET_USER_ACCESS_COMMAND         SetUserAccess = {0};
    UINT8                               DataSize;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if (Key != CHANGE_USER_SETTINGS_BMC_USER_PRIVILEGE_LIMIT_KEY) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters ();
    if (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so return EFI_UNSUPPORTED\n"));
        return  EFI_UNSUPPORTED;
    }
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibGetBrowserData() = %r\n", Status));
    ASSERT_EFI_ERROR(Status);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: ChangeUserSettingsBmcUserPrivilegeLimit = %x\n", ServerMgmtConfiguration->ChangeUserSettingsBmcUserPrivilegeLimit));

    
    //
    // Byte1
    //
    SetUserAccess.ChannelNo = ServerMgmtConfiguration->ChangeUserSettingsBmcChannelNo;
    SetUserAccess.EnableIpmiMessaging = 1;
    SetUserAccess.EnableUserLinkAuthetication = 1;
    SetUserAccess.UserRestrictedToCallback = 0;
    SetUserAccess.EnableChangeBitsInByte1 = 1;

    //
    // Byte2
    //
    SetUserAccess.UserID = gBmcChangeUserDetails.UserId;
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: SetUserAccess.UserID = %x\n", SetUserAccess.UserID));

    //
    // Byte3
    //
    SetUserAccess.UserPrivilegeLimit = ServerMgmtConfiguration->ChangeUserSettingsBmcUserPrivilegeLimit;

    //
    // Byte4
    //
    SetUserAccess.UserSimultaneousSessionLimit = 0;

    DataSize = 0;
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_USER_ACCESS,
                (UINT8 *)&SetUserAccess,
                sizeof(SetUserAccess),
                NULL,
                &DataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: CommandCompletionCode: %x \n", gIpmiTransport->CommandCompletionCode));
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of IPMI_APP_SET_USER_ACCESS Command: %r \n", Status));

    if(EFI_ERROR(Status)) {
        DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_SET_USER_ACCESS_CMD_FAIL) );
        ServerMgmtConfiguration->ChangeUserSettingsBmcUserPrivilegeLimit = 0;

        //
        // Set Browser DATA
        //
        Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI: Status of HiiLibSetBrowserData() = %r\n", Status));
        FreePool (ServerMgmtConfiguration);
        return EFI_SUCCESS;
    }
    DisplayErrorMessagePopUp ( HiiHandle, STRING_TOKEN(STR_SET_USER_ACCESS_CMD_PASS) );
    FreePool (ServerMgmtConfiguration);
    return EFI_SUCCESS;
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
