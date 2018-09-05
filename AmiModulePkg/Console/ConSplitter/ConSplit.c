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
/** @file 
    Console Splitter driver that creates a centralized input and
    output console so that the correct data is going to and coming
    from the correct devices
**/

#include <ConSplit.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/LoadedImage.h>
#include <Setup.h>
#include <Dxe.h>
#include <Hob.h>
#include <Token.h>
#include <Guid/HiiKeyBoardLayout.h>

/// Handle used as the System Table's ConInHandle
EFI_HANDLE ConSplitHandle = NULL;

/// Linked list used to keep track of the Console In Devices
DLIST       ConInList;

/// Linked list used to keep track of the Console Out Devices
DLIST       ConOutList;

/// Linked list used to keep track of the Simple Pointer Devices
DLIST       ConPointerList;

/// Linked list used to keep track of the Key Notification functions
DLIST       KeyNotifyList;

/// Linked list used to keep track of the Absolute Pointer Devices
DLIST       ConAbsolutePointerList;

/// Global variable used to keep track of the ConSplitter's toggle states
EFI_KEY_TOGGLE_STATE mCSToggleState = TOGGLE_STATE_VALID;

/// Global variable used to keep track of the num lock state
BOOLEAN NumLockSet = FALSE;

/// Variable used to track if the SupportedModes information is valid
static BOOLEAN InitModesTableCalled = FALSE;

/// Global variable used to store the HiiDatabase protocol pointer
EFI_HII_DATABASE_PROTOCOL *HiiDatabase      = NULL;

/// Global variable used to store the current keyboard layout 
EFI_HII_KEYBOARD_LAYOUT *gKeyDescriptorList = NULL;

/// Global variable that stores the number of keys in the current keyboard layout
UINT16 KeyDescriptorListSize                = 0;

/// The current console control screen mode
static EFI_CONSOLE_CONTROL_SCREEN_MODE ScreenMode = EfiConsoleControlScreenText;

/// Variable tracking if the cursor visbility settings
static BOOLEAN CursorVisible = TRUE;

/// Variable tracking the current standard in status
BOOLEAN CurrentStdInStatus = FALSE;

/// variable to lock out a disconnect call while currently executing within an input function
UINT8 LockConIn = 0;
UINT8 LockPointer = 0;
BOOLEAN BlockConInStop = FALSE; 
BOOLEAN BlockPointerStop = FALSE;


/// Variable used to store the LockStdIn password CRC
static UINT32 LockCrcValue = 0;

extern SIMPLE_TEXT_OUTPUT_MODE MasterMode;
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelBuffer;

EFI_STATUS InstallConOutDevice(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *SimpleOut,
    IN EFI_HANDLE                      Handle
    );

EFI_STATUS InstallConInDevice(
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *SimpleIn,
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleInEx,
    IN AMI_EFIKEYCODE_PROTOCOL           *KeycodeIn,
    IN EFI_HANDLE                        Handle
    );

EFI_STATUS InstallSimplePointerDevice(
    IN EFI_SIMPLE_POINTER_PROTOCOL       *SimplePointer,
    IN EFI_HANDLE                        Handle
    );

EFI_STATUS InstallAbsolutePointerDevice(
    IN EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointer,
    IN EFI_HANDLE Handle );

EFI_STATUS ConOutHandleCheck(IN EFI_HANDLE Handle);
EFI_STATUS ConInHandleCheck(IN EFI_HANDLE Handle);
VOID EFIAPI CSSetKbLayoutNotifyFn(IN EFI_EVENT Event, IN VOID *Context);

EFI_STATUS UnRegisterAllKeyNotifyHandlers(VOID);
EFI_STATUS ReRegisterAllKeyNotifyHandlers(VOID);

static EFI_STATUS EFIAPI
ComponentNameGetDriverName(
    IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
);

static EFI_STATUS EFIAPI
ComponentNameGetControllerName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN  EFI_HANDLE                   ControllerHandle,
    IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **ControllerName
);

EFI_STATUS ConSimplePointerHandleCheck( IN EFI_HANDLE Handle );
EFI_STATUS ConAbsolutePointerHandleCheck( IN EFI_HANDLE Handle);

EFI_STATUS EFIAPI
GetMode(
    IN  EFI_CONSOLE_CONTROL_PROTOCOL    *This,
    OUT EFI_CONSOLE_CONTROL_SCREEN_MODE *Mode,
    OUT BOOLEAN                         *UgaExists OPTIONAL,
    OUT BOOLEAN                         *StdInLocked OPTIONAL
);
EFI_STATUS EFIAPI
SetMode(
    IN EFI_CONSOLE_CONTROL_PROTOCOL    *This,
    IN EFI_CONSOLE_CONTROL_SCREEN_MODE Mode
);
EFI_STATUS EFIAPI
LockStdIn(
    IN EFI_CONSOLE_CONTROL_PROTOCOL *This,
    IN CHAR16 *Password
);


#if (EFI_DEBUG)
VOID DisplaySupportedModes(VOID);
#endif

/// Simple Text Out Driver binding protcol declaration
EFI_DRIVER_BINDING_PROTOCOL gConSplitterSimpleTextOutDriverBindingProtocol = {
    CSSimpleTextOutSupported,
    CSSimpleTextOutStart,
    CSSimpleTextOutStop,
    0x10,
    NULL,
    NULL
    };

/// Simple Text In driver binding protcol declaration
EFI_DRIVER_BINDING_PROTOCOL gConSplitterTextInDriverBindingProtocol = {
    CSSimpleTextInSupported,
    CSSimpleTextInStart,
    CSSimpleTextInStop,
    0x10,
    NULL,
    NULL
    };

/// Pointer protocol driver binding protocol declaration
EFI_DRIVER_BINDING_PROTOCOL gConSplitterPointerDriverBindingProtocol = {
    CSPointerSupported,
    CSPointerStart,
    CSPointerStop,
    0x10,
    NULL,
    NULL
    };

/// Multi language support protocol declaration
AMI_MULTI_LANG_SUPPORT_PROTOCOL     gMultiLangSupportProtocol = {
    KeyboardLayoutMap
};

/// Component name protocol for the Text In driver binding protocol
static EFI_COMPONENT_NAME2_PROTOCOL gComponentNameTextIn = {
  ComponentNameGetDriverName,
  ComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};

/// Component name protocol for the Text Out driver binding protocol
static EFI_COMPONENT_NAME2_PROTOCOL gComponentNameTextOut = {
  ComponentNameGetDriverName,
  ComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};

/// Component name protocol for the pointer driver binding protocol
static EFI_COMPONENT_NAME2_PROTOCOL gComponentNamePointer = {
  ComponentNameGetDriverName,
  ComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};

/// Driver name for the generic Console Splitter driver binding protocol
static CHAR16 *gDriverName=L"AMI Console Splitter Driver";

/// Driver name for the Pointer driver binding protocol
static CHAR16 *gDriverNamePointer=L"AMI Console Splitter Pointer Driver";

/// Driver name for the text out driver binding protocol
static CHAR16 *gDriverNameTextOut=L"AMI Console Splitter Text Out Driver";

/// Driver name for the text in driver binding protocol
static CHAR16 *gDriverNameTextIn=L"AMI Console Splitter Text In Driver";

/// Controller name for the Ami Console Splitter
static CHAR16 *gControllerName=L"AMI Console Splitter";

/// The console control protocol declaration
EFI_CONSOLE_CONTROL_PROTOCOL gConsoleControlProtocol = {
    GetMode,
    SetMode,
    LockStdIn
};

/**
    The GetControllerName function of the EFI_COMPONENT_NAME_PROTOCOL for the consplitter device

    @param This pointer to the EFI_COMPONENT_NAME2_PROTOCOL protocol instance
    @param Controller Parent device handle handle
    @param ChildHandle Handle of the child device child handle
    @param Language Pointer to language description
    @param ControllerName Double pointer to return the controller name

    @retval EFI_SUCCESS controller name returned
    @retval EFI_INVALID_PARAMETER language undefined
    @retval EFI_UNSUPPORTED given language not supported

**/
static EFI_STATUS EFIAPI
ComponentNameGetControllerName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN  EFI_HANDLE                   ControllerHandle,
    IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **ControllerName
)
{
    //Supports only English
    if(!Language || !ControllerName || !ControllerHandle)
        return EFI_INVALID_PARAMETER;

    if(ChildHandle!=ConSplitHandle || !LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH))
        return EFI_UNSUPPORTED;

    *ControllerName=gControllerName;
    return EFI_SUCCESS;
}

/**
    The GetDriverName function of the EFI_COMPONENT_NAME_PROTOCOL for the consplitter device.

    @param This pointer to the EFI_COMPONENT_NAME2_PROTOCOL protocol instance
    @param Language Pointer to language description
    @param DriverName Double pointer to return the driver name

    @retval EFI_SUCCESS driver name returned
    @retval EFI_INVALID_PARAMETER language undefined
    @retval EFI_UNSUPPORTED given language not supported

**/
static EFI_STATUS EFIAPI
ComponentNameGetDriverName(
    IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
)
{
    //Supports only English
    if(!Language || !DriverName)
        return EFI_INVALID_PARAMETER;

    if (!LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH))
        return EFI_UNSUPPORTED;

    if(This == &gComponentNameTextOut)
        *DriverName = gDriverNameTextOut;
    else if(This == &gComponentNameTextIn)
        *DriverName = gDriverNameTextIn;
    else if(This == &gComponentNamePointer)
        *DriverName = gDriverNamePointer;
    else
        *DriverName=gDriverName;

    return EFI_SUCCESS;
}

/**
    Function GetMode of the the EFI_CONSOLE_CONTROL_PROTOCOL. This function will return
    information about how the screen is currently being used (text or graphics) and
    will inform the caller if the UGA protocol exists and the current status of
    the ConsoleIn Devices (locked, or unlocked)

    @param This Pointer to the EFI_CONSOLE_CONTROL_PROTOCOL
    @param Mode Mode of the screen (either EfiConsoleControlScreenText or EfiConsoleControlScreenGraphics)
    @param UgaExists If not null, will return if the UGA protocol exist
    @param StdInLocked If not null, will return the Console In Lock status (TRUE = locked, FALSE = not locked)

    @retval EFI_SUCCESS function returns correct values
**/
EFI_STATUS EFIAPI
GetMode(
    IN  EFI_CONSOLE_CONTROL_PROTOCOL    *This,
    OUT EFI_CONSOLE_CONTROL_SCREEN_MODE *Mode,
    OUT BOOLEAN                         *UgaExists OPTIONAL,
    OUT BOOLEAN                         *StdInLocked OPTIONAL
)
{
    if (Mode)
        *Mode = ScreenMode;

    if (UgaExists)
        *UgaExists = TRUE;

    if (StdInLocked)
        *StdInLocked = CurrentStdInStatus;

    return EFI_SUCCESS;
}

/**
    Function SetMode of the EFI_CONSOLE_CONTROL_PROTOCOL.  This function will
    set the information about the current status of the console.

    @param This Pointer to the EFI_CONSOLE_CONTROL_PROTOCOL
    @param Mode Mode to set for the screen (either EfiConsoleControlScreenText or EfiConsoleControlScreenGraphics)

    @retval EFI_SUCCESS Mode set successfully
    @retval EFI_INVALID_PARAMETER Invalid mode given

**/
EFI_STATUS EFIAPI
SetMode(
    IN EFI_CONSOLE_CONTROL_PROTOCOL    *This,
    IN EFI_CONSOLE_CONTROL_SCREEN_MODE Mode
)
{
    if(ScreenMode != Mode)
    {
        ScreenMode = Mode;
        if (ScreenMode == EfiConsoleControlScreenText)
        {
            // Restore UGA mode when switching from graphics to text
            DLINK *ListPtr = ConOutList.pHead;
            while ( ListPtr != NULL)
            {
                CON_SPLIT_OUT *SimpleOut;
                SimpleOut = OUTTER(ListPtr, Link, CON_SPLIT_OUT);
                RestoreUgaMode(SimpleOut->Handle);
                ListPtr = ListPtr->pNext;
            }

            if (CursorVisible)
                mCSOutProtocol.EnableCursor(&mCSOutProtocol,TRUE);
                
            if(PixelBuffer != NULL) {
                pBS->FreePool(PixelBuffer);
                PixelBuffer = NULL;
            }
        }
        else if (ScreenMode == EfiConsoleControlScreenGraphics)
        {
            DLINK *ListPtr = ConOutList.pHead;
            CursorVisible = MasterMode.CursorVisible;
            if (CursorVisible)
                mCSOutProtocol.EnableCursor(&mCSOutProtocol,FALSE);

            //Save UGA mode when switching from text to graphics
            while ( ListPtr != NULL)
            {
                CON_SPLIT_OUT *SimpleOut;
                SimpleOut = OUTTER(ListPtr, Link, CON_SPLIT_OUT);
                SaveUgaMode(SimpleOut->Handle);
                ListPtr = ListPtr->pNext;
            }
        }
        else
            return EFI_INVALID_PARAMETER;
    }
    return EFI_SUCCESS;
}

/**
    Function LockStdIn of the EFI_CONSOLE_CONTROL_PROTOCOL.  This function
    will prevent the Consplitter's Virtual input from returning key input
    data.

    @param This Pointer to the EFI_CONSOLE_CONTROL_PROTOCOL
    @param Password Pointer to a password string used to prevent

    @retval EFI_SUCCESS The console was locked or unlocked successfully
    @retval EFI_ACCESS_DENIED The password value was incorrect, console is still locked
**/
EFI_STATUS EFIAPI
LockStdIn(
    IN EFI_CONSOLE_CONTROL_PROTOCOL *This,
    IN CHAR16 *Password
)
{
    EFI_STATUS Status = EFI_ACCESS_DENIED;
    UINT32 CrcValue = 0;

    pBS->CalculateCrc32(Password, Wcslen(Password), &CrcValue);

    if(CurrentStdInStatus == FALSE)
    {
        UnRegisterAllKeyNotifyHandlers();
        CurrentStdInStatus = TRUE;
        LockCrcValue = CrcValue;
        Status = EFI_SUCCESS;
    }
    else if(LockCrcValue == CrcValue)
    {
        CurrentStdInStatus = FALSE;
        ReRegisterAllKeyNotifyHandlers();
        Status = EFI_SUCCESS;
    }

    return Status;
};

/**
    Function to update the EFI_SYSTEM_TABLE's ConsoleOut pointer, ConsoleOutHandle,
    StdErr and StandardErrorHandle, and to recalculate the CRC32 for the
    EFI_SYSTEM_TABLE.
**/
VOID UpdateSystemTableConOut(VOID)
{
    UINT32 CRC32 = 0;

    if(ConOutList.Size == 0)
    {
        EFI_STATUS Status;
        //Initialize all the global variables used by
        //splitter implementation of TxtOut.
        //When physical ConOut devices are available, the initialization is performed
        //within InitModesTable routine.
        Status = ResizeSplitterBuffer(0);
        ASSERT_EFI_ERROR(Status);

        Status = pBS->AllocatePool(EfiBootServicesData, sizeof(SUPPORT_RES), (VOID**)&SupportedModes);
        ASSERT_EFI_ERROR(Status);

        SupportedModes[0].Rows =  25;
        SupportedModes[0].Columns = 80;
        SupportedModes[0].AllDevices = TRUE;
    }

    pST->ConOut = &mCSOutProtocol;
    pST->ConsoleOutHandle = ConSplitHandle;
    // We want to initialize ConOut-related fields of the systems table early
    // to workaround bugs in some of the UEFI OpROM drivers
    // that are using pST->ConOut without NULL checking.
    // We are not installing the instance of SimpleTextOut on ConSplitHandle though,
    // because it confuses the logic of TSE notification callbacks.
    // The protocol is installed once all ConOut devices are connected
    // in ReportNoConOutError.
    if (pST->StdErr == NULL)
    {
        pST->StdErr = pST->ConOut;
        pST->StandardErrorHandle = pST->ConsoleOutHandle;
    }

    // Now calculate the CRC32 value
    pST->Hdr.CRC32 = 0;
    pST->BootServices->CalculateCrc32(pST, sizeof(EFI_SYSTEM_TABLE), &CRC32);
    pST->Hdr.CRC32 = CRC32;
}

/**
    Callback function notified by the Console Available protocol installation.
    If SDL token REPORT_NO_CON_OUT_ERROR is set to 1, this function will check
    if a real console out device exists in the system, and will report an error
    otherwise.  The final portion of this function is to install the Simple Text
    Out and Console Control protocols onto the EFI_SYSTEM_TABLE's ConInHandle.

    @param Event signalled event
    @param Context pointer to event context
**/
VOID ReportNoConOutError(IN EFI_EVENT Event, IN VOID *Context)
{
#if defined(REPORT_NO_CON_OUT_ERROR)&&(REPORT_NO_CON_OUT_ERROR==1)
    DLINK       *Link;
    EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL *Dp;

    //Report error if no ConOut devices available or
    // all console devices are fake devices (without device path).
    for(Link = ConOutList.pHead; Link!=NULL; Link=Link->pNext)
    {
        CON_SPLIT_OUT *SimpleOut = OUTTER(Link, Link, CON_SPLIT_OUT);

        // Check if this device path is a real device (has a device path) or if it
        //  is only a virtual device (no device path)
        Status = pBS->HandleProtocol(SimpleOut->Handle, &gEfiDevicePathProtocolGuid, (VOID**)&Dp);
        if(!EFI_ERROR(Status))
            break;
    }

    //Report error if no real console out devices exist
    if( ConOutList.Size==0 || EFI_ERROR(Status) )
        ERROR_CODE(DXE_NO_CON_OUT, EFI_ERROR_MAJOR);
#endif
    pBS->InstallMultipleProtocolInterfaces( &ConSplitHandle,
                                            &gEfiSimpleTextOutProtocolGuid, &mCSOutProtocol,
                                            &gEfiConsoleControlProtocolGuid, &gConsoleControlProtocol,
                                            NULL);
    pBS->CloseEvent(Event);
}


/**
  Function which is called when the Ctrl-Alt-Delete key sequence is detected. This function
  will cause the system to reset by calling the runtime services table's ResetSystem.

  @param KeyData Pointer to the key data that was read and caused this function to be called

  @return EFI_STATUS The status of the function
  @retval EFI_SUCCESS This function exited, instead of causing a reset.
**/
EFI_STATUS EFIAPI HandleCtrlAltDel(IN EFI_KEY_DATA *KeyData)
{
    if(CurrentStdInStatus == FALSE)
        pRS->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
    return EFI_SUCCESS;
}

/**
    This function updates system table ConIn pointer

    @param Event signalled event
    @param Context pointer to event context
**/
VOID UpdateSystemTableConIn(IN EFI_EVENT Event, IN VOID *Context)
{
    UINT32 CRC32 = 0;
    
    UINTN i = 0;
    EFI_KEY_DATA ControlAltDelKeys[] =
    {
        {{EFI_SCAN_DEL, 0}, {EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED, 0}},
        {{EFI_SCAN_DEL, 0}, {EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED, 0}},
        {{EFI_SCAN_DEL, 0}, {EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED, 0}},
        {{EFI_SCAN_DEL, 0}, {EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED, 0}},
        {{EFI_SCAN_DEL, '.'}, {EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED, 0}},
        {{EFI_SCAN_DEL, '.'}, {EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED, 0}},
        {{EFI_SCAN_DEL, '.'}, {EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED, 0}},
        {{EFI_SCAN_DEL, '.'}, {EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED, 0}}
    };

    // The ControlAltDel will be registered onto the Console Splitter's registerkeynotify, which in turn will
    //  register the functions onto the managed child devices.  Because the ConSplitter is not installed through
    //  the driver binding sequence, we will not ever need to unregister the control alt delete keys from the
    //  console splitter, so only one Handle will be used. 
    VOID *RegisterHandle = NULL;

#if REPORT_NO_CON_IN_ERROR
    DLINK *Link;
    EFI_STATUS Status;

    EFI_HOB_HANDOFF_INFO_TABLE *pHit;
    static EFI_GUID guidHob = HOB_LIST_GUID;

    //Report error if no ConIn devices available or
    // all console devices are fake devices (without device path).
    for(Link = ConInList.pHead; Link!=NULL; Link=Link->pNext){
        CON_SPLIT_IN *SimpleIn = OUTTER(Link, Link, CON_SPLIT_IN);
        VOID *Dp;

        Status = pBS->HandleProtocol(
            SimpleIn->Handle, &gEfiDevicePathProtocolGuid, &Dp
        );
        if (!EFI_ERROR(Status)) break; // Got one device path
    }

    pHit = GetEfiConfigurationTable(pST, &guidHob);
    //Report error if no ConIn devices with device path exists
    if( (ConInList.Size == 0 || EFI_ERROR(Status)) && (pHit->BootMode == BOOT_WITH_FULL_CONFIGURATION))
        ERROR_CODE(DXE_NO_CON_IN, EFI_ERROR_MAJOR);
#endif
    pST->ConIn = &mCSSimpleInProtocol;

    pBS->InstallMultipleProtocolInterfaces (&ConSplitHandle,
                                            &gEfiSimpleTextInProtocolGuid, &mCSSimpleInProtocol,
                                            &gEfiSimpleTextInputExProtocolGuid, &mCSSimpleInExProtocol,
                                            &gAmiEfiKeycodeProtocolGuid, &mCSKeycodeInProtocol,
                                            &gEfiSimplePointerProtocolGuid, &mCSSimplePointerProtocol,
                                            &gEfiAbsolutePointerProtocolGuid, &mCSAbsolutePointerProtocol,
                                            NULL);

    pST->ConsoleInHandle = ConSplitHandle;

    // Now calculate the CRC32 value
    pST->Hdr.CRC32 = 0;
    pST->BootServices->CalculateCrc32(pST, sizeof(EFI_SYSTEM_TABLE), &CRC32);
    pST->Hdr.CRC32 = CRC32;

    pBS->CloseEvent(Event);

    // Register the ctrl-alt-delete reset function
    for(i = 0; i < sizeof(ControlAltDelKeys)/sizeof(EFI_KEY_DATA); i++)
        mCSSimpleInExProtocol.RegisterKeyNotify(&mCSSimpleInExProtocol, &ControlAltDelKeys[i], &HandleCtrlAltDel, &RegisterHandle);
}

/**
    Consplitter driver entry point.  The entrypoint installs a DriverBinding protocol
    and componentname2 protocol onto the ImageHandle.  Then it proceeds to setup the
    timer callbacks for the SimpleTextIn's WaitForKey, SimpleTextInEx's WaitForKeyEx,
    AmiKeyCode's WaitForKeyEx and the SimplePointer's WaitForInput. Callback functions
    are registered for the Con Out Started protocol installation, the Con In Started
    protocol installation and for the event signaled by the Hii Database when the
    keyboard layout is changed.

    @param ImageHandle Image handle of the Console splitter driver
    @param SystemTable Pointer to the EFI_SYSTEM_TABLE

    @retval EFI_SUCCESS driver installed successfully
    @retval EFI_ERROR error occured during execution
**/
EFI_STATUS EFIAPI CSEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    static EFI_GUID guidConInStarted = CONSOLE_IN_DEVICES_STARTED_PROTOCOL_GUID;
    static EFI_GUID guidConOutStarted = CONSOLE_OUT_DEVICES_STARTED_PROTOCOL_GUID;

    EFI_STATUS  Status;
    EFI_EVENT   Event;
    VOID *pRegistration;

    // initialize AMI library
    InitAmiLib(ImageHandle, SystemTable);

    // Initialize and install the Simple Text Out driving binding protocol
    gConSplitterSimpleTextOutDriverBindingProtocol.DriverBindingHandle = NULL;
    gConSplitterSimpleTextOutDriverBindingProtocol.ImageHandle = ImageHandle;

    Status = EfiLibInstallDriverBindingComponentName2 (
        ImageHandle,
        SystemTable,
        &gConSplitterSimpleTextOutDriverBindingProtocol,
        gConSplitterSimpleTextOutDriverBindingProtocol.DriverBindingHandle,
        NULL,
        &gComponentNameTextOut
    );
    ASSERT_EFI_ERROR (Status);
    
    // Initialize and install the simple text in driver binding protocol
    gConSplitterTextInDriverBindingProtocol.DriverBindingHandle = NULL;
    gConSplitterTextInDriverBindingProtocol.ImageHandle = ImageHandle;
    
    // Install driver binding protocol here
    Status = EfiLibInstallDriverBindingComponentName2 (
        ImageHandle,
        SystemTable,
        &gConSplitterTextInDriverBindingProtocol,
        gConSplitterTextInDriverBindingProtocol.DriverBindingHandle,
        NULL,
        &gComponentNameTextIn
    );
    ASSERT_EFI_ERROR (Status);

    // Initialize and install the simple/absolute pointer driver binding protocol
    gConSplitterPointerDriverBindingProtocol.DriverBindingHandle = NULL;
    gConSplitterPointerDriverBindingProtocol.ImageHandle = ImageHandle;
    
    // Install driver binding protocol here
    Status = EfiLibInstallDriverBindingComponentName2 (
        ImageHandle,
        SystemTable,
        &gConSplitterPointerDriverBindingProtocol,
        gConSplitterPointerDriverBindingProtocol.DriverBindingHandle,
        NULL,
        &gComponentNamePointer
    );
    ASSERT_EFI_ERROR (Status);

    // Create and event for the Simple In Interface
    Status = pBS->CreateEvent (EFI_EVENT_NOTIFY_WAIT, TPL_NOTIFY,
                CSWaitForKey, &mCSSimpleInProtocol,
                &mCSSimpleInProtocol.WaitForKey
                );
    ASSERT_EFI_ERROR (Status);

    // Create and event for the SimpleInEx Interface
    Status = pBS->CreateEvent (EFI_EVENT_NOTIFY_WAIT, TPL_NOTIFY,
                CSWaitForKey, &mCSSimpleInExProtocol,
                &mCSSimpleInExProtocol.WaitForKeyEx
                );
    ASSERT_EFI_ERROR (Status);


    // Create and event for the KeycodeIn Interface
    Status = pBS->CreateEvent (EFI_EVENT_NOTIFY_WAIT, TPL_NOTIFY,
                CSWaitForKey, &mCSKeycodeInProtocol,
                &mCSKeycodeInProtocol.WaitForKeyEx
                );
    ASSERT_EFI_ERROR (Status);

    // Create an event for the SimplePointer Interface
    Status = pBS->CreateEvent(
        EFI_EVENT_NOTIFY_WAIT,
        TPL_NOTIFY,
        ConSplitterSimplePointerWaitForInput,
        &mCSSimplePointerProtocol,
        &mCSSimplePointerProtocol.WaitForInput
    );
    ASSERT_EFI_ERROR(Status);

    Status = pBS->CreateEvent (
        EVT_NOTIFY_WAIT,
        TPL_NOTIFY,
        ConSplitterAbsolutePointerWaitForInput,
        &mCSAbsolutePointerProtocol,
        &mCSAbsolutePointerProtocol.WaitForInput
    );
    ASSERT_EFI_ERROR(Status);

    // Initialize the global lists here
    DListInit(&ConInList);
    DListInit(&ConOutList);
    DListInit(&ConPointerList);
    DListInit(&KeyNotifyList);
    DListInit(&ConAbsolutePointerList);

    // Register Protocol Notification to expose
    // Console Splitter interface only after all consoles initialized
    RegisterProtocolCallback(&guidConOutStarted, ReportNoConOutError, NULL, &Event,&pRegistration);
    RegisterProtocolCallback(&guidConInStarted, UpdateSystemTableConIn, NULL, &Event,&pRegistration);

    //We need a valid handle
    //The only way to create it is to install a protocol
    //Let's install a dummy protocol
    pBS->InstallMultipleProtocolInterfaces (
        &ConSplitHandle,
        &gAmiMultiLangSupportProtocolGuid, &gMultiLangSupportProtocol,
        NULL
    );

    //install pST->ConOut
    UpdateSystemTableConOut();

    // Callback when keyboard layout is set in Hii Database.
    Status = pBS->CreateEventEx(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    CSSetKbLayoutNotifyFn,
                    NULL,
                    &gEfiHiiKeyBoardLayoutGuid,
                    &Event);
    CSSetKbLayoutNotifyFn(NULL, NULL);

    return Status;
}

/**
    Function to check if the passed handle matches the handle that corresponds to the
    virtual console splitter that we created in UpdateSystemTableConOut. Used to prevent
    attempting this driver from attempting to bind onto of it self.

    @param Handle Device handle to check against the virtual console splitter created by this driver

    @retval EFI_SUCCESS this is not the virtual Console splitter handle
    @retval EFI_UNSUPPORTED This is the handle of the console splitter
**/
EFI_STATUS CheckHandle(IN EFI_HANDLE Handle)
{
    EFI_STATUS Status;
    EFI_LOADED_IMAGE_PROTOCOL *Image;

    // 1. Don't connect to our own handle (ConSplitHandle).
    // 2. Don't connect to pST->ConsoleOutHandle.
    // 3. Don't connect to pST->ConsoleInHandle.
    // 4. Don't connect to a handle installed by UEFI application (as oppose to a driver).
    // In a typical scenario pST->ConsoleOutHandle is ConSplitHandle (see UpdateSystemTableConOut above).
    // In a typical scenario pST->ConsoleInHandle is ConSplitHandle (see UpdateSystemTableConIn above).
    // If  ConSplitHandle has been replaced with another handle or is a handle produced by UEFI application,
    // the chances are that the TxtOut associated with this handle is special (f.i. alternative splitter)
    // and installing our splitter on top of it can cause undesired side effects.
    // For example, Shell installs an instance of TxtOut that internally calls our splitter.
    // An attempt to install our splitter on top of this TxtOut would lead to an endless dispatching loop
    // within the TxtOut member functions implemented in Out.c.

    if (Handle == ConSplitHandle || Handle == pST->ConsoleOutHandle || Handle == pST->ConsoleInHandle)
        return EFI_UNSUPPORTED;

    Status = pBS->HandleProtocol(Handle, &gEfiLoadedImageProtocolGuid, (VOID**)&Image);
    if(EFI_ERROR(Status))
        return EFI_SUCCESS;

    return (Image->ImageCodeType == EfiLoaderCode) ? EFI_UNSUPPORTED : EFI_SUCCESS;
}


//======================================================================================
//======================================================================================
//======================================================================================
//===  Begin Driver Binding Protocol


//======================================================================================
// === Begin Simple Text Out Driver Binding Protocol

/**
    Simple text out supported function for the simple text out driver binding protocol. 
    Tests if the passed controller handle contains a simple text out instance that should
    be managed by the console splitter driver

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param RemainingDevicePath Pointer to the EFI_DEVICE_PATH_PROTOCOL for the ControllerHandle

    @retval EFI_SUCCESS This controller can be managed by the consplitter driver
    @retval EFI_UNSUPPORTED This controller cannot be managed by the consplitter driver
**/
EFI_STATUS EFIAPI
CSSimpleTextOutSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
)
{
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *SimpleOut = NULL;
    EFI_STATUS Status;

    Status = CheckHandle(ControllerHandle);
    if(EFI_ERROR(Status))
        return Status;
        
    // check to see if this device has a simple text out protocol installed on it
    Status = pBS->OpenProtocol( ControllerHandle, &gEfiSimpleTextOutProtocolGuid,
                                (VOID**)&SimpleOut, This->DriverBindingHandle,
                                ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    if ( EFI_ERROR(Status) )
        return EFI_UNSUPPORTED;
    else
        pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextOutProtocolGuid,
                           This->DriverBindingHandle, ControllerHandle);

    return EFI_SUCCESS;
}

/**
    ConsoleSplitter driver binding start function for SimpleTextOut. 
    Attempts to configure the controller handle to be managed by the console splitter driver.

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param RemainingDevicePath Pointer to the EFI_DEVICE_PATH_PROTOCOL for the ControllerHandle

    @retval EFI_SUCCESS This controller is now managed by the consplitter driver
    @retval EFI_UNSUPPORTED An error was encountered in attempting to manage this device
**/
EFI_STATUS EFIAPI
CSSimpleTextOutStart(
        IN EFI_DRIVER_BINDING_PROTOCOL *This,
        IN EFI_HANDLE                  ControllerHandle,
        IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
    )
{
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *SimpleOut = NULL;
    EFI_STATUS  OutStatus;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    EFI_STATUS Status;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.

    // if it has a simple text out add the Con Out device to the list and
    OutStatus = pBS->OpenProtocol(ControllerHandle, &gEfiSimpleTextOutProtocolGuid,
                                (VOID**)&SimpleOut, This->DriverBindingHandle,
                                ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (!EFI_ERROR(OutStatus) )
    {
        OutStatus = InstallConOutDevice(SimpleOut, ControllerHandle);
        if (EFI_ERROR(OutStatus)) {
            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextOutProtocolGuid,
                        This->DriverBindingHandle, ControllerHandle);
            return OutStatus;
        } else {
            RestoreTheScreen(ControllerHandle,SimpleOut);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
            Status = pBS->OpenProtocol(ControllerHandle, &gEfiSimpleTextOutProtocolGuid,
                              (VOID**)&SimpleOut, This->DriverBindingHandle,
                              ConSplitHandle, EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER );
            ASSERT_EFI_ERROR(Status);
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        }
    }

    // If no devices were installed, then Houston we have a problem
    if ( EFI_ERROR(OutStatus))
        return EFI_UNSUPPORTED;


    return EFI_SUCCESS;
}


/**
    ConsoleSplitter driver binding stop function for Simple Text Out. Unconfigures the device from being
    managed by this this driver.

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param NumberOfChildren Number of child handles in the ChildHandleBuffer
    @param ChildHandleBuffer Pointer to array of ChildDevices of this controller

    @retval EFI_SUCCESS This controller can be managed by the consplitter driver
    @retval EFI_UNSUPPORTED This controller cannot be managed by the consplitter driver

    @retval EFI_SUCCESS driver stopped successfully
    @retval EFI_INVALID_PARAMETER invalid values passed for NumberOfChildren or ChildHandleBuffer
    @retval EFI_UNSUPPORTED The controller could not be removed from the management of this driver
**/
EFI_STATUS EFIAPI
CSSimpleTextOutStop (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN UINTN                       NumberOfChildren,
    IN EFI_HANDLE                  *ChildHandleBuffer OPTIONAL
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    DLINK *ListPtr;
    BOOLEAN Stopped = FALSE;

    if (NumberOfChildren == 0)
        return EFI_SUCCESS;

    if ( NumberOfChildren != 1 ||
         ChildHandleBuffer == NULL ||
         *ChildHandleBuffer!= ConSplitHandle )
        return EFI_INVALID_PARAMETER;

    // remove simple text out
    ListPtr = ConOutList.pHead;
    while ( ListPtr != NULL)
    {
        CON_SPLIT_OUT *SimpleOut;
        SimpleOut = OUTTER(ListPtr, Link, CON_SPLIT_OUT);
        if ( SimpleOut->Handle == ControllerHandle)
        {
            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextOutProtocolGuid,
                        This->DriverBindingHandle, ControllerHandle);

            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextOutProtocolGuid,
                        This->DriverBindingHandle, ConSplitHandle);
            DListDelete(&ConOutList, ListPtr);
            Stopped = TRUE;  // indicates that at least one device was stopped
            // added to help cover cases where an individual device was being disconnected and 
            // then reconnected.  i.e. video device to run Option ROM.
            SaveTheScreen(ControllerHandle, SimpleOut->SimpleOut); 
            Status = pBS->FreePool(SimpleOut);
            break;
        }

        ListPtr = ListPtr->pNext;
    }

    //If we already populated pST->ConOut preserve
    //screen buffer and list of supported modes
    //to keep using it when ConOut devices are connected
    if(ConOutList.Size == 0 && !pST->ConOut) //all devices stops
    {
        if(SupportedModes != NULL)
        {
            pBS->FreePool(SupportedModes);
            SupportedModes = NULL;
            InitModesTableCalled = FALSE;
        }

        if(ScreenBuffer != NULL)
        {
            pBS->FreePool(ScreenBuffer);
            ScreenBuffer = NULL;
        }

        if(AttributeBuffer != NULL)
        {
            pBS->FreePool(AttributeBuffer);
            AttributeBuffer = NULL;
        }

        MasterMode.Mode=0;
    } else {
        // re-initialize supported modes buffer if at least one child was stopped
        if(Stopped && ConOutList.Size > 0) 
            AdjustSupportedModes();
    }
       
    return Status;
}


// End Simple Text Out Driver Binding Protocol
//=========================================================================




//=========================================================================
// Begin SimpleTextIn, SimpleTextInEx, and AmiEfiKeycode Driver Binding Protocol

/**
    Simple text in supported function for the simple text in driver binding protocol. 
    Tests if the passed controller handle contains a simple in instance that should
    be managed by the console splitter driver

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param RemainingDevicePath Pointer to the EFI_DEVICE_PATH_PROTOCOL for the ControllerHandle

    @retval EFI_SUCCESS This controller can be managed by the consplitter driver
    @retval EFI_UNSUPPORTED This controller cannot be managed by the consplitter driver
**/
EFI_STATUS EFIAPI
CSSimpleTextInSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *SimpleIn = NULL;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleInEx = NULL;
    AMI_EFIKEYCODE_PROTOCOL *KeycodeIn = NULL;
    
    EFI_STATUS InStatus;
    EFI_STATUS InExStatus;
    EFI_STATUS KeycodeInStatus;
    
    Status = CheckHandle(ControllerHandle);
    if(EFI_ERROR(Status))
        return Status;

    // Check if this handle contains a supported protocol 
    InStatus = pBS->OpenProtocol(   ControllerHandle, &gEfiSimpleTextInProtocolGuid,
                                    (VOID**)&SimpleIn, This->DriverBindingHandle,
                                    ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    InExStatus = pBS->OpenProtocol( ControllerHandle, &gEfiSimpleTextInputExProtocolGuid,
                                    (VOID**)&SimpleInEx, This->DriverBindingHandle,
                                    ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    KeycodeInStatus = pBS->OpenProtocol(ControllerHandle, &gAmiEfiKeycodeProtocolGuid,
                                        (VOID**)&KeycodeIn, This->DriverBindingHandle,
                                        ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (!EFI_ERROR(InStatus))
        pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextInProtocolGuid,
                           This->DriverBindingHandle, ControllerHandle);

    if (!EFI_ERROR(InExStatus))
        pBS->CloseProtocol(ControllerHandle, &gAmiEfiKeycodeProtocolGuid,
                           This->DriverBindingHandle, ControllerHandle);

    if (!EFI_ERROR(KeycodeInStatus))
        pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextInputExProtocolGuid,
                           This->DriverBindingHandle, ControllerHandle);

    if ( EFI_ERROR(InStatus) &&
         EFI_ERROR(InExStatus) &&
         EFI_ERROR(KeycodeInStatus))
        return EFI_UNSUPPORTED;

    return EFI_SUCCESS;
}

/**
    ConsoleSplitter driver binding start function for SimpleTextIn, SimpleTextInEx, and AmiEfiKeycode. 
    Attempts to configure the controller handle to be managed by the console splitter driver.

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param RemainingDevicePath Pointer to the EFI_DEVICE_PATH_PROTOCOL for the ControllerHandle

    @retval EFI_SUCCESS This controller is now managed by the consplitter driver
    @retval EFI_UNSUPPORTED An error was encountered in attempting to manage this device
**/
EFI_STATUS EFIAPI
CSSimpleTextInStart(
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
)
{
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL          *SimpleIn = NULL;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL       *SimpleInEx = NULL;
    AMI_EFIKEYCODE_PROTOCOL                 *KeycodeIn = NULL;

    EFI_STATUS Status = EFI_UNSUPPORTED;
    EFI_STATUS  InStatus;
    EFI_STATUS  InExStatus;
    EFI_STATUS  KeycodeInStatus;
    SETUP_DATA *SetupData = NULL;
    UINTN VariableSize = 0;
    EFI_GUID SetupGuid = SETUP_GUID;

    // if Simple In, add the Con In device to the list and
    InStatus = pBS->OpenProtocol(ControllerHandle, &gEfiSimpleTextInProtocolGuid,
                                (VOID**)&SimpleIn, This->DriverBindingHandle,
                                ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    InExStatus = pBS->OpenProtocol(ControllerHandle, &gEfiSimpleTextInputExProtocolGuid,
                                (VOID**)&SimpleInEx, This->DriverBindingHandle,
                                ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    KeycodeInStatus = pBS->OpenProtocol(ControllerHandle, &gAmiEfiKeycodeProtocolGuid,
                                (VOID**)&KeycodeIn, This->DriverBindingHandle,
                                ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (!EFI_ERROR(InStatus) || !EFI_ERROR(InExStatus) || !EFI_ERROR(KeycodeInStatus))
    {
        InStatus = InstallConInDevice(SimpleIn, SimpleInEx, KeycodeIn, ControllerHandle);
        if(EFI_ERROR(InStatus)) {
            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextInProtocolGuid,
                               This->DriverBindingHandle, ControllerHandle);
            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextInputExProtocolGuid,
                               This->DriverBindingHandle, ControllerHandle);
            pBS->CloseProtocol(ControllerHandle, &gAmiEfiKeycodeProtocolGuid,
                               This->DriverBindingHandle, ControllerHandle);
            if (InStatus == EFI_OUT_OF_RESOURCES)
                return InStatus;
        } else {
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
            Status = pBS->OpenProtocol(ControllerHandle, &gEfiSimpleTextInProtocolGuid,
                              (VOID**)&SimpleIn, This->DriverBindingHandle,
                              ConSplitHandle, EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER );
            ASSERT_EFI_ERROR(Status);
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        }
    }

    // If no devices were installed, then Houston we have a problem
    if ( EFI_ERROR(InStatus) )
        return EFI_UNSUPPORTED;

    // Light up the keyboard(s) lights
    if(NumLockSet == FALSE) {
    Status = GetEfiVariable(L"Setup", &SetupGuid, NULL, &VariableSize, (VOID**)&SetupData);
    if (!EFI_ERROR(Status)) {
        if (SetupData->Numlock) {
            mCSToggleState |= NUM_LOCK_ACTIVE;
            }
        }
        NumLockSet=TRUE;
    }
    pBS->FreePool(SetupData);

    CSInSetState ( NULL, &mCSToggleState );

    return EFI_SUCCESS;
}

/**
    ConsoleSplitter driver binding stop function for SimpleTextIn, SimpleTextInEx, and AmiEfiKeycode. Unconfigures the device from being
    managed by this this driver.

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param NumberOfChildren Number of child handles in the ChildHandleBuffer
    @param ChildHandleBuffer Pointer to array of ChildDevices of this controller

    @retval EFI_SUCCESS This controller can be managed by the consplitter driver
    @retval EFI_UNSUPPORTED This controller cannot be managed by the consplitter driver

    @retval EFI_SUCCESS driver stopped successfully
    @retval EFI_INVALID_PARAMETER invalid values passed for NumberOfChildren or ChildHandleBuffer
    @retval EFI_UNSUPPORTED The controller could not be removed from the management of this driver
**/
EFI_STATUS EFIAPI
CSSimpleTextInStop (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN UINTN                       NumberOfChildren,
    IN EFI_HANDLE                  *ChildHandleBuffer OPTIONAL
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    DLINK *ListPtr;

    // Checks to see if we are trying to disconnect a device while in process of executing
    // part of the interface
    if (LockConIn != 0) {
        BlockConInStop = TRUE;
        return EFI_NOT_READY;
    } else {
        BlockConInStop = FALSE;
    }

    if (NumberOfChildren == 0)
        return EFI_SUCCESS;

    if ( NumberOfChildren != 1 ||
         ChildHandleBuffer == NULL ||
         *ChildHandleBuffer!= ConSplitHandle )
        return EFI_INVALID_PARAMETER;


    ListPtr = ConInList.pHead;
    while ( ListPtr != NULL)
    {
        CON_SPLIT_IN *SimpleIn;
        SimpleIn = OUTTER(ListPtr, Link, CON_SPLIT_IN);
        if ( SimpleIn->Handle == ControllerHandle)
        {
            KeyNotifyRemoveChild(SimpleIn);

            DListDelete(&ConInList, ListPtr);

            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextInProtocolGuid,
                        This->DriverBindingHandle, ControllerHandle);

            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextInputExProtocolGuid,
                        This->DriverBindingHandle, ControllerHandle);

            pBS->CloseProtocol(ControllerHandle, &gAmiEfiKeycodeProtocolGuid,
                        This->DriverBindingHandle, ControllerHandle);

            pBS->CloseProtocol(ControllerHandle, &gEfiSimpleTextInProtocolGuid,
                        This->DriverBindingHandle, ConSplitHandle);

            Status = pBS->FreePool(SimpleIn);
            break;
        }

        ListPtr = ListPtr->pNext;
    }

        
    return Status;
}
// === End of Simple Text In Binding Protocol
//======================================================================================

//======================================================================================
// === Begin Simple Pointer and Absolute Pointer Driver Binding Protocol

/**
    ConSplitter Pointer driver binding supported function. Verifies that the passed
    ControllerHandle contains a pointer protocol that should be managed by the 
    console splitter driver.

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param RemainingDevicePath Pointer to the EFI_DEVICE_PATH_PROTOCOL for the ControllerHandle

    @retval EFI_SUCCESS This controller can be managed by the consplitter driver
    @retval EFI_UNSUPPORTED This controller cannot be managed by the consplitter driver
**/
EFI_STATUS EFIAPI
CSPointerSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointer = NULL;
    EFI_SIMPLE_POINTER_PROTOCOL *SimplePointer = NULL;
    EFI_STATUS SimplePointerStatus;
    EFI_STATUS AbsolutePointerStatus;

    
    Status = CheckHandle(ControllerHandle);
    if(EFI_ERROR(Status))
        return Status;

    // Check if this handle contains a supported protocol 
    AbsolutePointerStatus = pBS->OpenProtocol(ControllerHandle, &gEfiAbsolutePointerProtocolGuid,
                                              (VOID**)&AbsolutePointer, This->DriverBindingHandle,
                                              ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    
    SimplePointerStatus = pBS->OpenProtocol(ControllerHandle, &gEfiSimplePointerProtocolGuid,
                                            (VOID**)&SimplePointer, This->DriverBindingHandle,
                                            ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (!EFI_ERROR(SimplePointerStatus))
        pBS->CloseProtocol(ControllerHandle, &gEfiSimplePointerProtocolGuid,
                           This->DriverBindingHandle, ControllerHandle );

    if (!EFI_ERROR(AbsolutePointerStatus))
        pBS->CloseProtocol(ControllerHandle, &gEfiAbsolutePointerProtocolGuid,
                           This->DriverBindingHandle, ControllerHandle);

    if( EFI_ERROR(SimplePointerStatus) && 
        EFI_ERROR(AbsolutePointerStatus))
        return EFI_UNSUPPORTED;

    return EFI_SUCCESS;
}


    

/**
    ConsoleSplitter driver binding start function for SimplePointer and AbsolutePointer. Attempts to 
    configure the controller handle to be managed by the console splitter driver.

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param RemainingDevicePath Pointer to the EFI_DEVICE_PATH_PROTOCOL for the ControllerHandle

    @retval EFI_SUCCESS This controller is now managed by the consplitter driver
    @retval EFI_UNSUPPORTED An error was encountered in attempting to manage this device
**/
EFI_STATUS EFIAPI
CSPointerStart(
        IN EFI_DRIVER_BINDING_PROTOCOL *This,
        IN EFI_HANDLE                  ControllerHandle,
        IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
    )
{    
    EFI_ABSOLUTE_POINTER_PROTOCOL           *AbsolutePointer = NULL;
    EFI_SIMPLE_POINTER_PROTOCOL *SimplePointer = NULL;
    EFI_STATUS  Status;

    Status = pBS->OpenProtocol(ControllerHandle, &gEfiSimplePointerProtocolGuid,
                               (VOID**)&SimplePointer, This->DriverBindingHandle,
                               ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER);
    if (!EFI_ERROR(Status)) {
        Status = InstallSimplePointerDevice( SimplePointer, ControllerHandle );
        if (!EFI_ERROR(Status)) {
            Status = pBS->OpenProtocol(ControllerHandle, &gEfiSimplePointerProtocolGuid,
                                       (VOID**)&SimplePointer, This->DriverBindingHandle,
                                       ConSplitHandle, EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
            ASSERT_EFI_ERROR(Status);
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        } else {
            pBS->CloseProtocol(ControllerHandle, &gEfiSimplePointerProtocolGuid,
                               This->DriverBindingHandle, ControllerHandle);
            if (Status == EFI_OUT_OF_RESOURCES)
                return Status;
        }
    }
    
    Status = pBS->OpenProtocol(ControllerHandle, &gEfiAbsolutePointerProtocolGuid,
                               (VOID**)&AbsolutePointer, This->DriverBindingHandle,
                               ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER);
    if (!EFI_ERROR(Status)) {
        Status = InstallAbsolutePointerDevice( AbsolutePointer, ControllerHandle );
        if (!EFI_ERROR(Status)) {
            Status = pBS->OpenProtocol(ControllerHandle, &gEfiAbsolutePointerProtocolGuid,
                                       (VOID**)&AbsolutePointer, This->DriverBindingHandle,
                                       ConSplitHandle, EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
            ASSERT_EFI_ERROR(Status);
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        } else {
            pBS->CloseProtocol(ControllerHandle, &gEfiAbsolutePointerProtocolGuid,
                               This->DriverBindingHandle, ControllerHandle);
            if (Status == EFI_OUT_OF_RESOURCES)
                return Status;
        }
    }

    // If no devices were installed, then Houston we have a problem
    if ( EFI_ERROR(Status) )
        return EFI_UNSUPPORTED;

    return EFI_SUCCESS;
}
    
    

/**
    ConsoleSplitter driver binding stop function. Unconfigures the device from being
    managed by this this driver.

    @param This Pointer to the driver binding protocol
    @param ControllerHandle Handle of the controller to check if able to be managed by consplitter
    @param NumberOfChildren Number of child handles in the ChildHandleBuffer
    @param ChildHandleBuffer Pointer to array of ChildDevices of this controller

    @retval EFI_SUCCESS This controller can be managed by the consplitter driver
    @retval EFI_UNSUPPORTED This controller cannot be managed by the consplitter driver

    @retval EFI_SUCCESS driver stopped successfully
    @retval EFI_INVALID_PARAMETER invalid values passed for NumberOfChildren or ChildHandleBuffer
    @retval EFI_UNSUPPORTED The controller could not be removed from the management of this driver
**/
EFI_STATUS EFIAPI
CSPointerStop (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN UINTN                       NumberOfChildren,
    IN EFI_HANDLE                  *ChildHandleBuffer OPTIONAL
)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    DLINK *ListPtr;

    // Checks to see if we are trying to disconnect a device while in process of executing
    // part of the interface
    if (LockPointer != 0) {
        BlockPointerStop = TRUE;
        return EFI_NOT_READY;
    } else {
        BlockPointerStop = FALSE;
    }

    if (NumberOfChildren == 0)
        return EFI_SUCCESS;

    if ( NumberOfChildren != 1 ||
         ChildHandleBuffer == NULL ||
         *ChildHandleBuffer!= ConSplitHandle )
        return EFI_INVALID_PARAMETER;

    // Remove Simple Pointer
    ListPtr = ConPointerList.pHead;
    while (ListPtr != NULL) {
        CON_SPLIT_SIMPLE_POINTER *SimplePointer;
        SimplePointer = OUTTER(ListPtr, Link, CON_SPLIT_SIMPLE_POINTER);
        if ( SimplePointer->Handle == ControllerHandle ) {
            DListDelete(&ConPointerList, ListPtr);

            pBS->CloseProtocol(ControllerHandle, &gEfiSimplePointerProtocolGuid,
                                This->DriverBindingHandle, ControllerHandle);

            pBS->CloseProtocol(ControllerHandle, &gEfiSimplePointerProtocolGuid,
                                This->DriverBindingHandle, ConSplitHandle);

            Status = pBS->FreePool(SimplePointer);
            break;
        }
        ListPtr = ListPtr->pNext;
    }
    
    // Remove Absolute Pointer
    ListPtr = ConAbsolutePointerList.pHead;
    while (ListPtr != NULL) {
        CON_SPLIT_ABSOLUTE_POINTER *AbsolutePointer;
        AbsolutePointer = OUTTER(ListPtr, Link, CON_SPLIT_ABSOLUTE_POINTER);
        if (AbsolutePointer->Handle == ControllerHandle) {
            DListDelete(&ConAbsolutePointerList, ListPtr);
            
            pBS->CloseProtocol(ControllerHandle, &gEfiAbsolutePointerProtocolGuid,
                                This->DriverBindingHandle, ControllerHandle);

            pBS->CloseProtocol(ControllerHandle, &gEfiAbsolutePointerProtocolGuid,
                                This->DriverBindingHandle, ConSplitHandle);
            Status = pBS->FreePool(AbsolutePointer);
            break;
        }
        ListPtr = ListPtr->pNext;
    }
        
    return Status;
}

// === End of Simple Pointer and Absolute Pointer Driver Binding Protocol
//======================================================================================


// ====  End of Driver Binding Protocols
//======================================================================================
//======================================================================================
//======================================================================================




/**
    Function to create or update the sVarName EFI variable with the device path
    installed on the passed Controller EFI_HANDLE. If the EFI variable already
    exists, the device path will be appended as a additional device path instance.

    @param Controller Controller which contains the device path to store
    @param sVarName Unicode name of the EFI variable to store the device path
**/
VOID UpdateConVar(IN EFI_HANDLE Controller, IN CHAR16 *sVarName)
{
    static EFI_GUID guidEfiVar = EFI_GLOBAL_VARIABLE;
    EFI_DEVICE_PATH_PROTOCOL *pDevicePath, *pConDev = NULL;
    EFI_STATUS Status;
    UINTN DataSize=0;
    UINT32 Attributes;

    Status = pBS->HandleProtocol(Controller, &gEfiDevicePathProtocolGuid, (VOID**)&pDevicePath);
    if(!EFI_ERROR(Status))
    {
        Status = GetEfiVariable(sVarName, &guidEfiVar, &Attributes, &DataSize, (VOID**)&pConDev);
        if (EFI_ERROR(Status))
        {
            if (Status!=EFI_NOT_FOUND)
                return;

            DataSize = DPLength(pDevicePath);
            pRS->SetVariable(sVarName, &guidEfiVar,
                            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                            DataSize, pDevicePath);
            return;
        }

        if (!DPIsOneOf(pConDev, pDevicePath, FALSE))
        {
            EFI_DEVICE_PATH_PROTOCOL *pNewConDev = DPAddInstance(pConDev, pDevicePath);
            DataSize = DPLength(pNewConDev);
            pRS->SetVariable(sVarName, &guidEfiVar, Attributes, DataSize, pNewConDev);
            pBS->FreePool(pNewConDev);
        }

        pBS->FreePool(pConDev);
    }
}

/**
    Function to add another Simple Pointer protocol to the list
    of protocols being managed by the ConsoleSplitter

    @param SimplePointer Pointer to the protocol that will be managed
    @param Handle Handle of the new device

    @retval EFI_SUCCESS Device was added successfully
    @retval EFI_UNSUPPORTED The device was not supported.
    @retval EFI_OUT_OF_RESOURCES Problems were encountered in finding resources
**/
EFI_STATUS InstallSimplePointerDevice(
    IN EFI_SIMPLE_POINTER_PROTOCOL *SimplePointer,
    IN EFI_HANDLE Handle )
{
    EFI_STATUS Status;
    CON_SPLIT_SIMPLE_POINTER *ConSimplePointer = NULL;

    Status = ConSimplePointerHandleCheck(Handle);
    if(!EFI_ERROR(Status))
    {
        Status = pBS->AllocatePool(EfiBootServicesData, sizeof(CON_SPLIT_SIMPLE_POINTER), (VOID**)&ConSimplePointer);
        if(!EFI_ERROR(Status))
        {
            ConSimplePointer->SimplePointer = SimplePointer;
            ConSimplePointer->Handle = Handle;
            mCSSimplePointerProtocol.Mode->LeftButton |= SimplePointer->Mode->LeftButton;
            mCSSimplePointerProtocol.Mode->RightButton |= SimplePointer->Mode->RightButton;
            DListAdd(&ConPointerList, &ConSimplePointer->Link);
        }
    }
    return Status;
}

/**
    Function to add another absolute pointer protocol device to the list
    of devices managed by the console splitter.

    @param AbsolutePointer Pointer to the device's protocol that will be managed
    @param Handle Handle of the device

    @retval EFI_SUCCESS Device was added successfully
    @retval EFI_UNSUPPORTED The device was not supported.
    @retval EFI_OUT_OF_RESOURCES Problems were encountered in finding resources
**/
EFI_STATUS InstallAbsolutePointerDevice(
    IN EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointer,
    IN EFI_HANDLE Handle )
{
    EFI_STATUS Status;
    CON_SPLIT_ABSOLUTE_POINTER *ConAbsolutePointer = NULL;

    Status = ConAbsolutePointerHandleCheck(Handle);
    if(!EFI_ERROR(Status))
    {
        Status = pBS->AllocatePool(EfiBootServicesData, sizeof(CON_SPLIT_ABSOLUTE_POINTER), (VOID**)&ConAbsolutePointer);
        if(!EFI_ERROR(Status))
        {
            ConAbsolutePointer->AbsolutePointer = AbsolutePointer;
            ConAbsolutePointer->Handle = Handle;
            UpdateAbsolutePointerInformation();
            mCSAbsolutePointerProtocol.Mode->Attributes |= AbsolutePointer->Mode->Attributes;
            
            DListAdd(&ConAbsolutePointerList, &ConAbsolutePointer->Link);
        }
    }
    return Status;
}

/**
    Function to add another Simple Text Out protocol to the list of protocols being
    managed by the ConsoleSplitter.  Function will also ensure common configuration
    for all the console out devices.

    @param SimpleOut Pointer to the protocol that will be managed
    @param Handle Handle of the new device

    @retval EFI_SUCCESS Device was added successfully
    @retval EFI_UNSUPPORTED The device was not supported.
    @retval EFI_OUT_OF_RESOURCES Problems were encountered in finding resources

**/
EFI_STATUS InstallConOutDevice(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *SimpleOut,
    IN EFI_HANDLE                      Handle
)
{
    EFI_STATUS Status;
    CON_SPLIT_OUT *ConOut = NULL;
    INT32   DeviceModeNumber = 0;
    BOOLEAN Active = TRUE;

    Status = ConOutHandleCheck(Handle);
    if(EFI_ERROR(Status))
        return EFI_UNSUPPORTED;

    // The existing console devices are already in an extended mode, 
    // verify that the extended mode is supported by the device
    if(MasterMode.Mode != 0)
    {
        Status = IsModeSupported(SimpleOut, MasterMode.Mode, &DeviceModeNumber);
        if (EFI_ERROR(Status))
        {
	        // The device does not support the extended mode, add it to the list, 
	        // but mark its entry as non active in the list;
	        Active = FALSE;
        
	        SimpleOut->OutputString(SimpleOut, L"Console Does Not Support current video mode\n\r");
        }
    }

    // The first console out device becomes the master device.
    if( ConOutList.Size == 0 && !InitModesTableCalled)
    {
        /// check to see if platform engineer has overridden the default value of the token
        /// If it has been set to FALSE, then change the value of the MasterMode.CursorVisible
        /// field.  This will only be done for the First Child.  All the rest should follow
        /// from this device
        if (PcdGetBool(PcdDefaultCursorState) == FALSE)
               MasterMode.CursorVisible = 0;

        // The first ConOut device becomes the master device, and is used to populate the modes table
        Status = InitModesTable(SimpleOut, Handle);
        if(EFI_ERROR(Status)) 
            return Status;
    }
    // All devices added after the master device are only used to update the modes table
    else
        UpdateModesTable(SimpleOut, Handle);

    // Create an entry in the Con Spliter's Simple Out device list
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(CON_SPLIT_OUT), (VOID**)&ConOut);
    if (EFI_ERROR(Status))
        return EFI_OUT_OF_RESOURCES;

    ConOut->SimpleOut = SimpleOut;
    ConOut->Handle = Handle;
    ConOut->Active = Active;

    // If this is not the first device, then call set mode and enable the cursoor based on the 
    //  master mode structure
    if ((ConOutList.Size != 0) || (MasterMode.Mode != SimpleOut->Mode->Mode))
    {
        // Set the child display to a current master mode
        SimpleOut->SetMode(SimpleOut, DeviceModeNumber);
        SimpleOut->EnableCursor(SimpleOut, MasterMode.CursorVisible);
    }

    DListAdd(&ConOutList, &(ConOut->Link));

    #if defined(FORCE_COMMON_MODE_FOR_DEVICES)&&(FORCE_COMMON_MODE_FOR_DEVICES==1)
    if( SupportedModes[MasterMode.Mode].AllDevices != TRUE)
    {
        UINTN i;
        for(i = MasterMode.Mode; i > 0; i--)
        {
            if(SupportedModes[i].AllDevices == TRUE)
                break;
        }
        
        // When reaching here, either there will be a common mode set, or the default mode 0 will be used and the
        //  devices will be enabled/disabled by  set mode calls
        mCSOutProtocol.SetMode(&mCSOutProtocol, i);
    }
    #endif

    UpdateConVar(Handle, L"ConOutDev");
    return EFI_SUCCESS;
}

/**
    Function to add another Simple Text In, Simple Text In Ex or Ami Efikeycode Protocol,
    or any combination of those, to the list of protocols being managed by the
    ConsoleSplitter.

    @param *SimpleIn Pointer to the new SimpleIn protocol
    @param *SimpleInEx Pointer to the new SimpleInEx protocol
    @param *KeycodeIn Pointer to the AmiEfiKeyCode protocol
    @param Handle Handle of the device supplying the protocols

    @retval EFI_SUCCESS Device was added successfully
    @retval EFI_UNSUPPORTED The device was not supported.
    @retval EFI_OUT_OF_RESOURCES Problems were encountered in finding resources
**/
EFI_STATUS InstallConInDevice(
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *SimpleIn,
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleInEx,
    IN AMI_EFIKEYCODE_PROTOCOL           *KeycodeIn,
    IN EFI_HANDLE                        Handle
)
{
    EFI_STATUS Status;
    CON_SPLIT_IN *ConIn;

    Status = ConInHandleCheck(Handle);
    if(!EFI_ERROR(Status))
    {
        Status = pBS->AllocatePool(EfiBootServicesData, sizeof(CON_SPLIT_IN), (VOID**)&ConIn);
        if(!EFI_ERROR(Status))
        {
            ConIn->SimpleIn = SimpleIn;
            ConIn->SimpleInEx = SimpleInEx;
            ConIn->KeycodeInEx = KeycodeIn;
            ConIn->Handle = Handle;

            // Only register the key notification functions if the standard in is not locked
            if(CurrentStdInStatus == FALSE)
                KeyNotifyAddChild(ConIn);

            DListAdd(&ConInList, &(ConIn->Link));

            UpdateConVar(Handle, L"ConInDev");
        }
    }
    return Status;
}

/**
    Function that checks if the passed handle already exists in the
    list of Console Out devices that are being managed by the
    ConsoleSplitter driver

    @param Handle Handle of the device being checked

    @retval EFI_SUCCESS Device is not already managed
    @retval EFI_UNSUPPORTED The device is already being managed
**/
EFI_STATUS ConOutHandleCheck(IN EFI_HANDLE Handle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CON_SPLIT_OUT *SimpleOut;

    SimpleOut = OUTTER(ConOutList.pHead, Link, CON_SPLIT_OUT);

    // Go through the list checking if the handle exists in the entries
    while(SimpleOut != NULL)
    {
        if (SimpleOut->Handle == Handle)
        {
            Status = EFI_UNSUPPORTED;
            break;
        }

        // Go to the next list entry
        SimpleOut = OUTTER(SimpleOut->Link.pNext, Link, CON_SPLIT_OUT);
    }
    return Status;
}

/**
    Function that checks if the passed handle already exists in the
    list of Console In devices that are being managed by the
    ConsoleSplitter driver

    @param Handle Handle of the device being checked

    @retval EFI_SUCCESS Device is not already managed
    @retval EFI_UNSUPPORTED The device is already being managed
**/
EFI_STATUS ConInHandleCheck(IN EFI_HANDLE Handle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CON_SPLIT_IN *SimpleIn;

    SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN);

    // Go through the list checking if the handle exists in the entries
    while(SimpleIn != NULL)
    {
        if (SimpleIn->Handle == Handle)
        {
            Status = EFI_UNSUPPORTED;
            break;
        }
        SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN);
    }
    return Status;
}

/**
    Function that checks if the passed handle already exists in the
    list of Simple Pointer devices that are being managed by the
    ConsoleSplitter driver

    @param Handle - handle of device to check

    @retval EFI_SUCCESS device not present
    @retval EFI_UNSUPPORTED device already present

**/
EFI_STATUS ConSimplePointerHandleCheck( IN EFI_HANDLE Handle )
{
    EFI_STATUS Status = EFI_SUCCESS;
    CON_SPLIT_SIMPLE_POINTER *SimplePointer;

    SimplePointer = OUTTER(ConPointerList.pHead, Link, CON_SPLIT_SIMPLE_POINTER);

    // Go through the list checking if the handle exists in the entries
    while(SimplePointer != NULL)
    {
        if (SimplePointer->Handle == Handle)
        {
            Status = EFI_UNSUPPORTED;
            break;
        }

        SimplePointer = OUTTER(SimplePointer->Link.pNext, Link, CON_SPLIT_SIMPLE_POINTER);
    }

    return Status;
}

/**
    Function that checks if the passed handle already exists in the
    list of Simple Pointer devices that are being managed by the
    ConsoleSplitter driver

    @param Handle - handle of device to check

    @retval EFI_SUCCESS device not present
    @retval EFI_UNSUPPORTED device already present

**/
EFI_STATUS ConAbsolutePointerHandleCheck( IN EFI_HANDLE Handle )
{
    EFI_STATUS Status = EFI_SUCCESS;
    CON_SPLIT_ABSOLUTE_POINTER *AbsolutePointer;

    AbsolutePointer = OUTTER(ConAbsolutePointerList.pHead, Link, CON_SPLIT_ABSOLUTE_POINTER);

    // Go through the list checking if the handle exists in the entries
    while(AbsolutePointer != NULL)
    {
        if (AbsolutePointer->Handle == Handle)
        {
            Status = EFI_UNSUPPORTED;
            break;
        }

        AbsolutePointer = OUTTER(AbsolutePointer->Link.pNext, Link, CON_SPLIT_ABSOLUTE_POINTER);
    }

    return Status;
}
/**
    This function fills the gSupportedModes table with modes supported by the first
    simple text out device.

    @param This Pointer to the Simple Text Out protocol for this device
    @param Handle Handle of the device that contained the Simple Text Out

    @retval EFI_SUCCESS table filled successfully
    @retval EFI_OUT_OF_RESOURCES not enough resources to perform operation
**/
EFI_STATUS InitModesTable(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN EFI_HANDLE                      Handle
)
{
    INT32 MaxMode;
    INT32 CurrentMode;
    UINTN Rows, Columns;
    INT32 i;
    EFI_STATUS Status;

    if(Handle == ConSplitHandle)
        return EFI_SUCCESS;

    InitModesTableCalled = TRUE;

    MaxMode = This->Mode->MaxMode;
    CurrentMode = This->Mode->Mode;

    //The SupportedModes structure
    //may have already been initialized in UpdateSystemTableConOut.
    //If this is the case, free the memory before reinitialization.
    if(SupportedModes != NULL)
    {
        //If SupportedModes is not NULL, ResizeSplitterBuffer(0) has already been called.
        // Free the allocated buffer so it can be recreated with the information from
        // this new device.
        pBS->FreePool(SupportedModes);
        SupportedModes = NULL;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(SUPPORT_RES)* MaxMode, (VOID**)&SupportedModes);
    if(EFI_ERROR(Status))
    {
        MasterMode.MaxMode = 1;
        return EFI_SUCCESS;
    }

    // Modify default value
    MasterMode.MaxMode = MaxMode;

    for(i = 0; i < MaxMode; i++)
    {
        Status = This->QueryMode(This, i, &Columns, &Rows);
        SupportedModes[i].Rows = (INT32)Rows;
        SupportedModes[i].Columns = (INT32)Columns;
        SupportedModes[i].AllDevices = EFI_ERROR(Status) ? FALSE : TRUE;
    }

    // Make sure MasterMode.Mode is not the CurrentMode, otherwise ResizeSplitterBuffer won't do anything
    MasterMode.Mode = CurrentMode + 1;
    Status = ResizeSplitterBuffer(CurrentMode);
    // now change MasterMode.Mode to be the CurrentMode
    MasterMode.Mode = CurrentMode;
    UpdateAbsolutePointerInformation();

#if EFI_DEBUG
    DisplaySupportedModes();
#endif    
    return Status;
}

/**
    The Console Splitter is a virtual device, and it contains a set of
    mode numbers that do not directly correlate to the mode numbers
    supplied by the actual Simple Text Out devices. This function exists
    to create the mapping from the virtual device mode number to an
    equivalent mode number for the physical device.

    @param This Pointer to the Simple Text Out prtocol function to find a mode number from
    @param CurrentSystemMode Console Splitter's Mode Numer
    @param DeviceModeNumber Pointer to a UINT32 to store the device's equivalent mode number

    @retval EFI_SUCCESS An equivalent mode was supported by the device and is contained in DeviceModeNumber
    @retval EFI_UNSUPPORTED An equivalent mode was not supported by the device
**/
EFI_STATUS IsModeSupported(
    IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN  UINTN                           CurrentSystemMode,
    OUT INT32                           *DeviceModeNumber)
{
    EFI_STATUS Status;
    UINTN Rows = 0;
    UINTN Columns = 0;
    INT32 i;

    // Go through the new device's mode list and attempt to find a mode
    //  that matches the current set of Rows and Columns the system is configured for
    for(i = 0; i < This->Mode->MaxMode; i++)
    {
        // ask device about each mode to find the Rows and columns for that mode
        Status = This->QueryMode(This, i, &Columns, &Rows);
        if(!EFI_ERROR(Status))
        {
            // compare rows and columns to the current mode's row and column
            if( SupportedModes[CurrentSystemMode].Rows == (INT32)Rows && 
                SupportedModes[CurrentSystemMode].Columns == (INT32)Columns)
            {
                // Mode i of the current device supports the Rows and column of the current system mode
                *DeviceModeNumber = i;
                return EFI_SUCCESS;
            }
        }
    }
    return EFI_UNSUPPORTED;
}

/**
    As additional physical devices are connected in the system, the modes table
    may need to be updated to incorporate the modes supported by the new devices.
    This function exists to keep track of the modes that are supported by all
    the devices connected in the system.

    @param This Pointer to the Simple Text Out protocol
    @param Handle Handle of the device the Simple Text Out was found on
**/
VOID UpdateModesTable(
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN EFI_HANDLE                      Handle
)
{
    INT32 i, ModeNumber;
    EFI_STATUS Status;

    if(Handle == ConSplitHandle)
        return;

    for(i = 0; i < MasterMode.MaxMode; i++) {
        if(SupportedModes[i].AllDevices == FALSE)
            continue;
        Status = IsModeSupported(This, i, &ModeNumber);
        SupportedModes[i].AllDevices = EFI_ERROR(Status) ? FALSE : TRUE;
    }

#if defined(FORCE_COMMON_MODE_FOR_DEVICES)&&(FORCE_COMMON_MODE_FOR_DEVICES==1)
    // Update gMasterMode.MaxMode value based on modes supported by different devices
    // lookup for the first mode above 1 not supported by all devices - this will be
    // new MaxMode value
   for(i = MasterMode.MaxMode; i > 0; i--) 
   {
        if(SupportedModes[i-1].AllDevices == TRUE)
            break;
    }
    MasterMode.MaxMode=i;
#endif

#if EFI_DEBUG    
    DisplaySupportedModes();
#endif
}

/**
    This function allocates new buffers when mode changed

    @param ModeNum new mode

    @retval EFI_SUCCESS new buffers allocated
    @retval EFI_OUT_OF_RESOURCES not enough resources to perform operation
**/
EFI_STATUS ResizeSplitterBuffer(IN INT32 ModeNum)
{
    INT32 Row, Col;
    CHAR16 *NewCharBuffer;
    INT32 *NewAttributeBuffer;
    EFI_STATUS Status;

    // Check if no mode initialization has occured
    if(ModeNum != MasterMode.Mode || SupportedModes == NULL)
    {
        if(SupportedModes == NULL)
        {
            Row = 25;
            Col = 80;
        }
        else
        {
            Row = SupportedModes[ModeNum].Rows;
            Col = SupportedModes[ModeNum].Columns;
        }

        Status = pBS->AllocatePool(EfiBootServicesData, sizeof(CHAR16) * Row * Col, (VOID**)&NewCharBuffer);
        if(EFI_ERROR(Status))
            return Status;

        Status = pBS->AllocatePool(EfiBootServicesData, sizeof(INT32) * Row * Col, (VOID**)&NewAttributeBuffer);
        if(EFI_ERROR(Status))
        {
            pBS->FreePool(NewCharBuffer);
            return Status;
        }

        if(ScreenBuffer != NULL)
            pBS->FreePool(ScreenBuffer);

        ScreenBuffer = NewCharBuffer;
        EndOfTheScreen = ScreenBuffer + (Row * Col);

        if(AttributeBuffer != NULL)
            pBS->FreePool(AttributeBuffer);

        AttributeBuffer = NewAttributeBuffer;
        Columns = Col;
    }
    MemClearScreen();
    return EFI_SUCCESS;
}

/**
    Callback function which is notified anytime that the keyboard layout being used
    by the system is changed via a call to the Hii Database's SetKeyboardLayout. The
    keyboardlayout is kept in a gloabl variable for use during the boot process.

    @param Event Event that caused the event to be notified
    @param Context Context of the event
**/
VOID EFIAPI
CSSetKbLayoutNotifyFn(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS Status;

    if(HiiDatabase == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&HiiDatabase);
        if(EFI_ERROR(Status))
            return;
    }

    Status = HiiDatabase->GetKeyboardLayout(HiiDatabase, NULL, &KeyDescriptorListSize, gKeyDescriptorList);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        if(gKeyDescriptorList != NULL)
            pBS->FreePool(gKeyDescriptorList);

        Status = pBS->AllocatePool(EfiBootServicesData, KeyDescriptorListSize, (VOID**)&gKeyDescriptorList);
        if(EFI_ERROR(Status)) {
            KeyDescriptorListSize = 0;
            gKeyDescriptorList = NULL;
        } else {
            HiiDatabase->GetKeyboardLayout(HiiDatabase, NULL, &KeyDescriptorListSize, gKeyDescriptorList);
        }
    } else if(Status == EFI_NOT_FOUND) {
        if(gKeyDescriptorList != NULL) {
            pBS->FreePool(gKeyDescriptorList);
            KeyDescriptorListSize = 0;
            gKeyDescriptorList = NULL;
        }
    }
}

/**
    Function to adjust the global list of supported modes based upon the currently
    connected SimpleTextOut devices.
    This function is only called when a device has been stopped.  This recreates the 
    Supported Modes structure based on the modes that are still installed
**/
VOID AdjustSupportedModes(VOID)
{
    DLINK *ListPtr;
    CON_SPLIT_OUT *SimpleOut;
    EFI_STATUS Status;
    INT32 i;
    UINTN Columns;
    UINTN Rows;

    ListPtr = ConOutList.pHead;
    SimpleOut = OUTTER(ListPtr, Link, CON_SPLIT_OUT);

    // Re-initialize the supported modes buffer
        if (SupportedModes != NULL)
            pBS->FreePool(SupportedModes);

        Status = pBS->AllocatePool(EfiBootServicesData, SimpleOut->SimpleOut->Mode->MaxMode * sizeof(SUPPORT_RES), (VOID**)&SupportedModes);
        if(EFI_ERROR(Status))
            return;

    MasterMode.MaxMode = SimpleOut->SimpleOut->Mode->MaxMode;
    for(i = 0; i < MasterMode.MaxMode; i++) {
        Status = SimpleOut->SimpleOut->QueryMode(SimpleOut->SimpleOut, i, &Columns, &Rows);
        SupportedModes[i].Rows = (INT32)Rows;
        SupportedModes[i].Columns = (INT32)Columns;
        SupportedModes[i].AllDevices = EFI_ERROR(Status) ? FALSE : TRUE;
    }

    // Update the supported modes buffer based upon all the connected devices
    ListPtr = ListPtr->pNext;
    while(ListPtr != NULL) {
        SimpleOut = OUTTER(ListPtr, Link, CON_SPLIT_OUT);
        UpdateModesTable(SimpleOut->SimpleOut, SimpleOut->Handle);
        ListPtr = ListPtr->pNext;
    }
}

/**
    This function maps an EFI_KEY to a Unicode character, based on the current
    keyboard layout

    @param This pointer ot the AMI_MULTI_LANG_SUPPORT_PROTOCOL instance
    @param KeyData pointer to the key data returned by a device

    @retval EFI_SUCCESS key was mapped successfully
    @retval EFI_NOT_FOUND the key was not found in the keyboard layout
    @retval EFI_INVALID_PARAMETER KeyData is NULL
**/
EFI_STATUS EFIAPI
KeyboardLayoutMap(
    IN      AMI_MULTI_LANG_SUPPORT_PROTOCOL *This,
    IN OUT  AMI_EFI_KEY_DATA                *KeyData)
{
    EFI_STATUS Status;
    EFI_KEY_DESCRIPTOR *KeyDescriptor;

    BOOLEAN AltState = FALSE;
    BOOLEAN ShiftKeyState = FALSE;
    BOOLEAN ShiftState = ShiftKeyState;

    static UINT16 ModifierIndex = 0xFFFF;
    static CHAR16 ModifierUnicodeChar = 0x0000;

    UINT16 i = 0;

    if(gKeyDescriptorList== NULL) {
        return EFI_NOT_FOUND;
    }

    KeyDescriptor = (EFI_KEY_DESCRIPTOR *)(gKeyDescriptorList + 1);

    // check alt status (left alt or right alt)
    if( ((KeyData->KeyState.KeyShiftState)&(RIGHT_ALT_PRESSED|LEFT_ALT_PRESSED)) != 0 )
        AltState = TRUE;

    if( ((KeyData->KeyState.KeyShiftState)&(RIGHT_SHIFT_PRESSED|LEFT_SHIFT_PRESSED)) != 0 )
        ShiftKeyState = TRUE;

    Status = EFI_NOT_FOUND;
    if ( (ModifierIndex != 0xFFFF) && (KeyDescriptor[ModifierIndex].Modifier == EFI_NS_KEY_MODIFIER) ) {
        // Previous key had a modifier, we need to find out what to do
        // for now, only handle EFI_NS_KEY_MODIFIER
        for (i = ModifierIndex+1; KeyDescriptor[i].Modifier == EFI_NS_KEY_DEPENDENCY_MODIFIER && i < gKeyDescriptorList->DescriptorCount; i++) {
            if(KeyDescriptor[i].Key == KeyData->EfiKey) {
                if ((KeyDescriptor[i].AffectedAttribute&EFI_AFFECTED_BY_STANDARD_SHIFT) != 0)
                    ShiftState = ShiftKeyState;
                else
                    ShiftState = FALSE;
                // account for cAPS lOCK, only applicable if the affected attribute is set
                if (!AltState && ((KeyDescriptor[i].AffectedAttribute&EFI_AFFECTED_BY_CAPS_LOCK) != 0) && ((KeyData->KeyState.KeyToggleState&CAPS_LOCK_ACTIVE) != 0))
                    ShiftState = !ShiftState;

                if (AltState && ShiftState && (KeyDescriptor[i].ShiftedAltGrUnicode != 0x0000)) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].ShiftedAltGrUnicode;
                    Status = EFI_SUCCESS;
                }
                else if (AltState && !ShiftState && (KeyDescriptor[i].AltGrUnicode != 0x0000)) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].AltGrUnicode;
                    Status = EFI_SUCCESS;
                }
                else if (!AltState && ShiftState && (KeyDescriptor[i].ShiftedUnicode != 0x0000)) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].ShiftedUnicode;
                    Status = EFI_SUCCESS;
                }
                else if (!AltState && !ShiftState && (KeyDescriptor[i].Unicode != 0x0000)) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].Unicode;
                    Status = EFI_SUCCESS;
                }
                break;
            }
        }

        if (EFI_ERROR(Status))
        {
            // No match found, just return the deadkey's character
            // return unique status to indicate to the caller that the current key should not be removed from the input buffer
            KeyData->Key.UnicodeChar = ModifierUnicodeChar;
            Status = EFI_WARN_STALE_DATA;
        }
        ModifierIndex = 0xFFFF;
        ModifierUnicodeChar = 0x0000;
        return Status;
    }

    // Search the KeyDescriptorList for a matching key
    for(i = 0; i < gKeyDescriptorList->DescriptorCount; i++)
    {
        if(KeyDescriptor[i].Key == KeyData->EfiKey || (KeyDescriptor[i].Key == 0xA5A5 && KeyData->PS2ScanCode == 0x73)) {
            if ((KeyDescriptor[i].AffectedAttribute&EFI_AFFECTED_BY_STANDARD_SHIFT) != 0)
                ShiftState = ShiftKeyState;
            else
                ShiftState = FALSE;
            // account for cAPS lOCK, only applicable if the affected attribute is set
            if (!AltState && ((KeyDescriptor[i].AffectedAttribute&EFI_AFFECTED_BY_CAPS_LOCK) != 0) && ((KeyData->KeyState.KeyToggleState&CAPS_LOCK_ACTIVE) != 0))
                ShiftState = !ShiftState;

            switch (KeyDescriptor[i].Modifier) {
            case EFI_NULL_MODIFIER:
                Status = EFI_SUCCESS;
                if (AltState && ShiftState) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].ShiftedAltGrUnicode;
                }
                else if (AltState && !ShiftState) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].AltGrUnicode;
                }
                else if (!AltState && ShiftState) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].ShiftedUnicode;
                }
                else if (!AltState && !ShiftState) {
                    KeyData->Key.UnicodeChar = KeyDescriptor[i].Unicode;
                }
                break;
            case EFI_NS_KEY_MODIFIER:
                Status = EFI_SUCCESS;
                if (AltState && ShiftState && (KeyDescriptor[i].ShiftedAltGrUnicode != 0x0000)) {
                    ModifierIndex = i;
                    ModifierUnicodeChar = KeyDescriptor[i].ShiftedAltGrUnicode;
                    KeyData->Key.UnicodeChar = 0x0000;      // don't return a character yet, the next keypress will determine the correct character
                }
                else if (AltState && !ShiftState && (KeyDescriptor[i].AltGrUnicode != 0x0000)) {
                    ModifierIndex = i;
                    ModifierUnicodeChar = KeyDescriptor[i].AltGrUnicode;
                    KeyData->Key.UnicodeChar = 0x0000;      // don't return a character yet, the next keypress will determine the correct character
                }
                else if (!AltState && ShiftState && (KeyDescriptor[i].ShiftedUnicode != 0x0000)) {
                    ModifierIndex = i;
                    ModifierUnicodeChar = KeyDescriptor[i].ShiftedUnicode;
                    KeyData->Key.UnicodeChar = 0x0000;      // don't return a character yet, the next keypress will determine the correct character
                }
                else if (!AltState && !ShiftState && (KeyDescriptor[i].Unicode != 0x0000)) {
                    ModifierIndex = i;
                    ModifierUnicodeChar = KeyDescriptor[i].Unicode;
                    KeyData->Key.UnicodeChar = 0x0000;      // don't return a character yet, the next keypress will determine the correct character
                }
            default:
            case EFI_NS_KEY_DEPENDENCY_MODIFIER:
                // skip deadkey-dependent modifiers and unknown modifiers
                break;
            }           // switch (KeyDescriptor[i].Modifier)

            if (!EFI_ERROR(Status) && (KeyData->Key.UnicodeChar != 0x0000 || ModifierUnicodeChar != 0x0000))
                break;  // successfully mapped a key, break for(...) loop
        }
    }
    return Status;
}

/**
    The EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL has a function, RegisterKeyNotify. For this
    function to work properly, anytime a notification function is registered to the
    ConsoleSplitter device's RegisterKeyNotify function, it needs to also be
    registered with to any new Simple Text In Ex protocols. This function accomplishes
    that by registering the notification to all the devices being managed.

    @param Child
**/
EFI_STATUS KeyNotifyAddChild(IN CON_SPLIT_IN *Child)
{
    KEY_NOTIFY_LINK *NotifyLink;
    UINT32 ChildIndex;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    EFI_STATUS Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    EFI_HANDLE NotifyHandle;

    if(ConInList.Size >= MAX_CON_IN_CHILDREN)
        return EFI_OUT_OF_RESOURCES;

    // No callbacks have been registered
    if(KeyNotifyList.Size == 0)
        return EFI_SUCCESS;

    NotifyLink = (KEY_NOTIFY_LINK *)KeyNotifyList.pHead;
    ChildIndex = (UINT32) ConInList.Size;
    while(NotifyLink != NULL) {
        NotifyLink->Children[ChildIndex].Child = Child;
        if(Child->SimpleInEx != NULL) {
            Status = Child->SimpleInEx->RegisterKeyNotify(
                        Child->SimpleInEx,
                        &(NotifyLink->KeyData),
                        NotifyLink->NotifyFunction,
                        &NotifyHandle);
            NotifyLink->Children[ChildIndex].NotifyHandle = (EFI_ERROR(Status)) ? (EFI_HANDLE) UNUSED_NOTIFY_HANDLE : NotifyHandle;
        } else {
            NotifyLink->Children[ChildIndex].NotifyHandle = (EFI_HANDLE) UNUSED_NOTIFY_HANDLE;
        }
        NotifyLink = (KEY_NOTIFY_LINK *)NotifyLink->Link.pNext;
    }
    return Status;
}


/**
    UnRegister the list of RegisterKeyNotify functions installed on each SimpleTextIn
    device being managed by the ConsoleSplitter.

    @param Child

    @retval EFI_SUCCESS The registration was removed from all the children devices
    @retval EFI_ERROR An error occured while removing the registration from the children devicse

**/
EFI_STATUS KeyNotifyRemoveChild(
    IN CON_SPLIT_IN *Child
)
{
    KEY_NOTIFY_LINK *NotifyLink;
    UINT32 ChildIndex;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    EFI_STATUS Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.

    // No callbacks registered
    if(KeyNotifyList.Size == 0)
        return EFI_SUCCESS;

    NotifyLink = (KEY_NOTIFY_LINK *)KeyNotifyList.pHead;
    while(NotifyLink != NULL) {
        for(ChildIndex = 0; ChildIndex < ConInList.Size; ChildIndex++) {
            if(NotifyLink->Children[ChildIndex].Child == Child &&
               Child->SimpleInEx != NULL &&
               NotifyLink->Children[ChildIndex].NotifyHandle != (EFI_HANDLE) UNUSED_NOTIFY_HANDLE) {
                Status = Child->SimpleInEx->UnregisterKeyNotify(Child->SimpleInEx, NotifyLink->Children[ChildIndex].NotifyHandle);
                NotifyLink->Children[ChildIndex].Child = NULL;
                NotifyLink->Children[ChildIndex].NotifyHandle = (EFI_HANDLE) UNUSED_NOTIFY_HANDLE;
            }
        }
        NotifyLink = (KEY_NOTIFY_LINK *)NotifyLink->Link.pNext;
    }
    return Status;
}

/**
    Go through the list of Console in devices, and call KeyNotifyRemoveChild on each instance to unregister
    all the key notification functions from each device

    @return EFI_STATUS The return status
    @retval EFI_SUCCESS The registration functions were attempted to be removed from all the managed devices
**/
EFI_STATUS UnRegisterAllKeyNotifyHandlers(VOID)
{
    CON_SPLIT_IN *SimpleIn = NULL;

    // Go through the list of console in devices and unregister the key notification functions
    for(SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN); SimpleIn != NULL; SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN))
        KeyNotifyRemoveChild(SimpleIn);

    return EFI_SUCCESS;
}

/**
    Go though the list of key notification functions and using the list of console in devices, reregister all
    the notification functions onto each device

    @return EFI_STATUS
    @retval EFI_SUCCESS The registration functions were attempted to be added to all managed devices
**/
EFI_STATUS ReRegisterAllKeyNotifyHandlers(VOID)
{
    CON_SPLIT_IN *SimpleIn = NULL;
    KEY_NOTIFY_LINK *NotifyLink = NULL;

    UINTN Index = 0;

    // Go through the set of notification links
    for(NotifyLink = (KEY_NOTIFY_LINK *)KeyNotifyList.pHead; NotifyLink != NULL; NotifyLink = (KEY_NOTIFY_LINK*)NotifyLink->Link.pNext)
    {
        // Go through the set of console in devices
        for(SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN), Index = 0; SimpleIn != NULL; SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN), Index++)
        {
            // Updated the notification list with the console in device, and register the notification functions to the console in device
            NotifyLink->Children[Index].Child = SimpleIn;
            if(SimpleIn->SimpleInEx != NULL)
                SimpleIn->SimpleInEx->RegisterKeyNotify(SimpleIn->SimpleInEx, &(NotifyLink->KeyData), NotifyLink->NotifyFunction, &(NotifyLink->Children[Index].NotifyHandle));
            else
                NotifyLink->Children[Index].NotifyHandle = (EFI_HANDLE)UNUSED_NOTIFY_HANDLE;
        }
    }
    return EFI_SUCCESS;
}

#if EFI_DEBUG
VOID DisplaySupportedModes (VOID)
{
	UINTN i;
	
	DEBUG((-1, "\r\n"));
	for(i = 0; i < MasterMode.MaxMode; i++)
    {
        DEBUG((-1, "Rows = %X  Col = %X    AllDevices = %X \n\r", SupportedModes[i].Rows, SupportedModes[i].Columns, SupportedModes[i].AllDevices));
    }

}
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
