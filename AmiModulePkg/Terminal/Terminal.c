//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file Terminal.c
    Installs EFI_SIMPLE_TEXT_INPUT_PROTOCOL and
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL for a serial device.
    Adds a vendor messaging path for the terminal type 
    (PcAnsi, VT100, etc) to its device path.

**/

#include "Terminal.h"
#if EFI_SIO_PROTOCOL_SUPPORT
#include <Protocol/SuperIo.h>
#else
#include <Protocol/AmiSio.h>
#endif
#include <Protocol/SerialIo.h>
#include <Protocol/TerminalAmiSerial.h>
#include <Library/BaseMemoryLib.h>

static ACPI_SPCR_TABLE_INFO gSpcrInfo;


EFI_EVENT   gAcpiSupportEvent;
VOID        *gAcpiSupportRegistration = NULL;
UINTN       gSpcrTableHandle = 0;
EFI_EVENT   gSetupInitEvent;
VOID        *gSetupInitRegistration = NULL;

extern EFI_COMPONENT_NAME_PROTOCOL  gComponentName;
extern UINT32 gTotalSioSerialPorts;
extern UINT32 gTotalPciSerialPorts;
extern DLIST  mTerminalKeyboardData;
extern UINT8    SPCR_OEM_ID[];
extern UINT8    SPCR_OEM_TBL_ID[];
extern CHAR8 SupportedLanguages[];

TERMINAL_DEV    **gTerminalDev; 
extern UINTN  TotalTerminalDev;
extern UINT32 Uart_Fifo_Size;
extern UINT8  SpcrInterfaceType;
extern UINT8  SpcrAddrSpcId;
extern UINT8  SpcrAddrBitWidth;
extern UINT8  SpcrAddrBitOffset;
extern UINT8  SpcrAccessSize;
extern UINT8  SpcrInterruptType;
extern UINT8  SpcrParity;
extern UINT8  SpcrStopBits;
extern UINT16 SpcrPciDeviceId;
extern UINT16 SpcrPciVendorId;
extern UINT8  SpcrPciBusNumber;
extern UINT8  SpcrPciDeviceNumber;
extern UINT8  SpcrPciFunctionNumber;
extern UINT8  SpcrPciFlags;
extern UINT8  SpcrPciSegment;
extern UINT64 AcpiOemRev;
extern UINT32 CoreRevision;
UINT8       *gStartedTerminalDev;
UINT8       gStartedTerminalDevCount=0;
extern BOOLEAN ClearTerminalKBBufferReadyToBoot;

extern STATE_DESCRIPTION EscSeqStateArray[];
extern STATE_DESCRIPTION Vtutf8EscSeqStateArray[];

EFI_STATUS 
EFIAPI
TerminalSupported(
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath);

EFI_STATUS 
EFIAPI
TerminalStart(
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath);

EFI_STATUS
EFIAPI
TerminalStop(
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN UINTN                        NumberOfChildren,
    IN EFI_HANDLE                   *ChildHandleBuffer);

//this funciton is created from InitList.c template file during build
//process
VOID
InitParts (
    IN EFI_HANDLE ImageHandle, 
    IN EFI_SYSTEM_TABLE *SystemTable
);

extern VOID
InitSerialPortsEnabledVar ();

extern UINT8
SetSerialPortsEnabledVar_Sio (
    UINT8 Port
);

extern EFI_STATUS 

ClearSerialPortsEnabledVar_Pci(
    IN UINT8 Bus,
    IN UINT8 Device, 
    IN UINT8 Function,
    IN UINT8 Port,
    IN BOOLEAN AmiPciSerialDetected );

extern UINT8
SetSerialPortsEnabledVar_Pci (
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN UINT8 Bus,
    IN UINT8 Device,
    IN UINT8 Function,
    IN UINT8 Port,
    IN BOOLEAN AmiPciSerialDetected
);

extern VOID
GetAcpiSpcrTableValues (
    IN UINT8 *AcpiSpcrTableComPort,
    IN OUT UINT8 *AcpiSpcrTableConsoleRedirectionEnable,
    IN OUT UINT8 *AcpiSpcrTableTerminalType,
    IN OUT UINT8 *AcpiSpcrTableBaudRate,
    IN OUT UINT8 *AcpiSpcrTableFlowControl
);


VENDOR_DEVICE_PATH gPcAnsiDevicePath =
{
    {
        MESSAGING_DEVICE_PATH,
        MSG_VENDOR_DP,
        sizeof(VENDOR_DEVICE_PATH),
    },
    EFI_PC_ANSI_GUID
};

VENDOR_DEVICE_PATH gVt100DevicePath = 
{
{
        MESSAGING_DEVICE_PATH,
        MSG_VENDOR_DP,
        sizeof(VENDOR_DEVICE_PATH),
    },
    EFI_VT_100_GUID
};

VENDOR_DEVICE_PATH gVt100PlusDevicePath =
{
    {
        MESSAGING_DEVICE_PATH,
        MSG_VENDOR_DP,
        sizeof(VENDOR_DEVICE_PATH),
    },
    EFI_VT_100_PLUS_GUID
};

VENDOR_DEVICE_PATH gVtUtf8DevicePath =
{
    {
        MESSAGING_DEVICE_PATH,
        MSG_VENDOR_DP,
        sizeof(VENDOR_DEVICE_PATH),
    },
    EFI_VT_UTF8_GUID
};

EFI_DRIVER_BINDING_PROTOCOL gTerminalDriverBinding = {
    TerminalSupported,
    TerminalStart,
    TerminalStop,
    0x10,   //Version
    NULL,   //Image Handle
    NULL    //DriverBindingHandle
};

/**
    Entry point for the Terminal driver.
    Installs the driver binding protocol.
    Calls InitParts which will result in the
    SerialIo driver entry point being called.
    Calls InitSerialPortsEnabledVar
    (this inits the variable which keeps track of how many
    serial ports have been enabled).

    @param      ImageHandle
    @param      SystemTable

    @retval     EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;
    UINT8   i;
    EFI_EVENT    ReadyToBootEvent;

    InitAmiLib(ImageHandle,SystemTable);

    gTerminalDriverBinding.ImageHandle          = ImageHandle;
    gTerminalDriverBinding.DriverBindingHandle  = NULL;

    gComponentName.SupportedLanguages = SupportedLanguages;

    Status = pBS->InstallMultipleProtocolInterfaces(
                    &gTerminalDriverBinding.DriverBindingHandle,
                    &gEfiDriverBindingProtocolGuid, &gTerminalDriverBinding,
#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID //old Core
                    &gEfiComponentNameProtocolGuid, &gComponentName,
#else
                    &gEfiComponentName2ProtocolGuid, &gComponentName,
#endif
                    NULL
                    );

    ASSERT_EFI_ERROR(Status);

    gTerminalDev = (TERMINAL_DEV **)MallocZ(sizeof(TERMINAL_DEV*)*TotalTerminalDev);
    gStartedTerminalDev = (UINT8 *)MallocZ(sizeof(UINT8)*TotalTerminalDev);

    //
    // Register the ReadyToBoot event function to clear the RawFIFO
    // and UnicodeFIFO buffer
    //
    if(ClearTerminalKBBufferReadyToBoot) {
        CreateReadyToBootEvent(TPL_CALLBACK,
                               ClearFiFoBuffers,
                               NULL,
                               &ReadyToBootEvent
                               );
    }

    for(i=0;i<TotalTerminalDev;i++) {
        gTerminalDev[i]=0;
        gStartedTerminalDev[i]=0xff;
    }

    InitSerialPortsEnabledVar();
    return Status;
}

/**
    Checks whether the device path and serial io protocols
    are supported.  
    Checks whether the terminal type is supported.

    @param  This
    @param  Controller
    @param  RemainingDevicePath

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath)
{
    EFI_STATUS                      Status;
    EFI_SERIAL_IO_PROTOCOL          *SerialIo;
    VENDOR_DEVICE_PATH              *TerminalPath;

    //This bus driver can only have one child device, so it can't
    //have been started already.  
    Status=pBS->OpenProtocol(
                    Controller,
                    &gEfiSerialIoProtocolGuid,
                    (VOID **)&SerialIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_BY_DRIVER 
                    );
    if(EFI_ERROR(Status)) {
        goto Error;
    }

    Status = pBS->CloseProtocol(
                    Controller,
                    &gEfiSerialIoProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    if(EFI_ERROR(Status)) {
        goto Error;
    }

    Status=pBS->OpenProtocol(
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    NULL,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

    if(EFI_ERROR(Status)) {
        pBS->CloseProtocol(Controller,
                           &gEfiSerialIoProtocolGuid,
                           This->DriverBindingHandle,
                           Controller
                           );
        goto Error;
    }

    if (RemainingDevicePath)
    {
        TerminalPath = (VENDOR_DEVICE_PATH *)RemainingDevicePath;
        if (TerminalPath->Header.Type != MESSAGING_DEVICE_PATH
            || TerminalPath->Header.SubType != MSG_VENDOR_DP)
        {
            return EFI_UNSUPPORTED;
        }
        if ((guidcmp(&TerminalPath->Guid,&gEfiVT100Guid) != 0) &&
                (guidcmp(&TerminalPath->Guid,&gEfiVT100PlusGuid) != 0) &&
                (guidcmp(&TerminalPath->Guid,&gEfiVTUTF8Guid) != 0) &&
                (guidcmp(&TerminalPath->Guid,&gEfiPcAnsiGuid) != 0))
        {
            return EFI_UNSUPPORTED;
        }
    }

    return EFI_SUCCESS;

Error:

    if( Status == EFI_ALREADY_STARTED || Status == EFI_ACCESS_DENIED ) {
        return Status;
    }
    return EFI_UNSUPPORTED;

}

/**
    Installs the SimpleTextIn and SimpleTextOut protocols.
    Adds a node to the device path for the terminal type.
    Modifies the SerialIo protocol according to Setup values.

    @param  This
    @param  Controller
    @param  RemainingDevicePath

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalStart (
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
)
{
    EFI_STATUS                      Status;
    EFI_DEVICE_PATH_PROTOCOL        *ParentDevicePath;
    EFI_SERIAL_IO_PROTOCOL          *SerialIo;
    VENDOR_DEVICE_PATH              *TerminalPath;
    EFI_HANDLE                      ChildHandle;
    TERMINAL_DEV                    *TerminalDev = NULL;
    EFI_DEVICE_PATH_PROTOCOL        *DefaultDevicePath = NULL;
    BOOLEAN                         SupportExRes;
    BOOLEAN                         RecorderMode;
    BOOLEAN                         VtUtf8;
    UINT8                           Disable_Terminal_For_SCT_Test;
    EFI_STATUS                      SerialIoStatus;
    UINT8                           ComPort = -1;
    BOOLEAN                         IsPciSerialInterface = FALSE;
    UINT8                           ConsoleRedirectionEnable = 0;
    SERIAL_IO_MODE                  SerialIoMode;
    UINT8                           TerminalType;
    UINT8                           TerminalIndex;
    UINT8                           PuttyKeyPad;

    Status = GetComPortNumber(This, Controller, &ComPort,
                                &IsPciSerialInterface);
    if (EFI_ERROR(Status)) goto Error;

    if (ComPort != (UINT8)-1) {
        //
        //Terminal Can be started many times for the same COM port.
        //But SPCR table should be created only once.
        //
        for(TerminalIndex=0;TerminalIndex<TotalTerminalDev;TerminalIndex++) {
            if(gStartedTerminalDev[TerminalIndex] == ComPort) {
                break;
            }
        }
        if(TerminalIndex >= TotalTerminalDev) {
#if ACPI_SUPPORT
            AcpiSpcrTable(ComPort, This, Controller);
#endif
            gStartedTerminalDev[gStartedTerminalDevCount]=ComPort;
            gStartedTerminalDevCount++;
        }
    }

    GetSetupValuesForTerminal(ComPort, &ConsoleRedirectionEnable,
                                &TerminalType,&SupportExRes,&VtUtf8,&RecorderMode,
                                &PuttyKeyPad,&Disable_Terminal_For_SCT_Test);

    if (!ConsoleRedirectionEnable) goto Error;

    if (Disable_Terminal_For_SCT_Test) goto Error;

    TerminalDev = MallocZ(sizeof(TERMINAL_DEV));
    if (!TerminalDev){
        return EFI_OUT_OF_RESOURCES;
    }

    //This bus driver can only have one child device, so it can't
    //have been already started.

    Status=pBS->OpenProtocol(
                    Controller,
                    &gEfiSerialIoProtocolGuid,
                    (VOID **)&SerialIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_BY_DRIVER
                    );

    if(EFI_ERROR(Status)) {
        goto Error;
    }

    DListInit(&mTerminalKeyboardData);
    TerminalDev->TerminalType = (TERMINAL_TYPE) TerminalType;

    TerminalDev->SimpleTextInput.Reset = TerminalInputReset;
    TerminalDev->SimpleTextInput.ReadKeyStroke = TerminalInputReadKey;
    TerminalDev->SimpleTextInput.WaitForKey = NULL;

    TerminalDev->SimpleTextInputEx.Reset = TerminalInputResetEx;
    TerminalDev->SimpleTextInputEx.ReadKeyStrokeEx = TerminalInputReadKeyEx;
    TerminalDev->SimpleTextInputEx.SetState = TerminalInputSetState;
    TerminalDev->SimpleTextInputEx.RegisterKeyNotify = TerminalInputRegisterKeyNotify;
    TerminalDev->SimpleTextInputEx.UnregisterKeyNotify = TerminalInputUnRegisterKeyNotify;
    TerminalDev->SimpleTextInputEx.WaitForKeyEx = NULL;

    TerminalDev->EfiKeycodeInput.Reset = TerminalInputResetEx;
    TerminalDev->EfiKeycodeInput.ReadEfikey = TerminalInputReadEfiKey;
    TerminalDev->EfiKeycodeInput.SetState = TerminalInputSetState;
    TerminalDev->EfiKeycodeInput.RegisterKeyNotify = TerminalInputRegisterKeyNotify;
    TerminalDev->EfiKeycodeInput.UnregisterKeyNotify = TerminalInputUnRegisterKeyNotify;
    TerminalDev->EfiKeycodeInput.WaitForKeyEx = NULL;

    InitTerminalKeyBuffer(&(TerminalDev->KeyboardBuffer));
    TerminalDev->KeyToggleState=TOGGLE_STATE_VALID;

    //MaxMode: Number of supported modes
    TerminalDev->TerminalTextOutputMode.MaxMode = (SupportExRes) ? 3 : 1;
    //Mode 0: 80 x 25,1:100 x 31
    TerminalDev->TerminalTextOutputMode.Mode = 0;       //default is Mode 0 80x25
    //Attribute: Foreground White, Background Black.
    TerminalDev->TerminalTextOutputMode.Attribute = 
                                    EFI_TEXT_ATTR(EFI_WHITE,EFI_BLACK);
    TerminalDev->TerminalTextOutputMode.CursorColumn = 0;
    TerminalDev->TerminalTextOutputMode.CursorRow = 0;
    TerminalDev->TerminalTextOutputMode.CursorVisible = TRUE;

    TerminalDev->SimpleTextOutput.Reset = TerminalReset;
    TerminalDev->SimpleTextOutput.OutputString = TerminalOutputString;
    TerminalDev->SimpleTextOutput.TestString = TerminalTestString;
    TerminalDev->SimpleTextOutput.QueryMode = TerminalQueryMode;
    TerminalDev->SimpleTextOutput.SetMode = TerminalSetMode;
    TerminalDev->SimpleTextOutput.SetAttribute = TerminalSetAttribute;
    TerminalDev->SimpleTextOutput.ClearScreen = TerminalClearScreen;
    TerminalDev->SimpleTextOutput.SetCursorPosition = TerminalSetCursorPosition;
    TerminalDev->SimpleTextOutput.EnableCursor = TerminalEnableCursor;
    TerminalDev->SimpleTextOutput.Mode = &TerminalDev->TerminalTextOutputMode;
    TerminalDev->RecorderMode=RecorderMode;
    TerminalDev->PuttyKeyPad=PuttyKeyPad;

    TerminalDev->SerialIo = SerialIo;

    if(VtUtf8) {
        TerminalDev->TerminalEscSeqState=Vtutf8EscSeqStateArray;
    } else {
        TerminalDev->TerminalEscSeqState=EscSeqStateArray;
    }

    Status=pBS->OpenProtocol(
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&ParentDevicePath,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if(EFI_ERROR(Status)) goto Error;

    //If Remaining Device Path, Verify and append to parent.
    //Else, use default terminal type and append default terminal path to
    //parent.
    if (RemainingDevicePath)
    {
        TerminalPath = (VENDOR_DEVICE_PATH *)RemainingDevicePath;
        if (TerminalPath->Header.Type != MESSAGING_DEVICE_PATH
            || TerminalPath->Header.SubType != MSG_VENDOR_DP) { 
            goto Error;
        }
        switch (TerminalDev->TerminalType) {
            case VT100:     TerminalPath->Guid = gEfiVT100Guid;
                            break;
            case VT100Plus: TerminalPath->Guid = gEfiVT100PlusGuid;
                            break;
            case VT_UTF8:   TerminalPath->Guid = gEfiVTUTF8Guid;
                            break;
            case ANSI:      TerminalPath->Guid = gEfiPcAnsiGuid;
                            break;
        }

        //Append one node of RemainingDevicePath.
        TerminalDev->ChildDevicePath =
                            DPAddNode(ParentDevicePath, RemainingDevicePath);
    } else {
        //Append default Device Path.
        switch (TerminalDev->TerminalType) {
            case VT100:     DefaultDevicePath = &gVt100DevicePath.Header;
                            break;
            case VT100Plus: DefaultDevicePath = &gVt100PlusDevicePath.Header;
                            break;
            case VT_UTF8:   DefaultDevicePath = &gVtUtf8DevicePath.Header;
                            break;
            case ANSI:      DefaultDevicePath = &gPcAnsiDevicePath.Header;
                            break;
        }
        TerminalDev->ChildDevicePath =
                            DPAddNode(ParentDevicePath, DefaultDevicePath);
    }

    Status = pBS->CreateEvent(
                    EVT_NOTIFY_WAIT,
                    TPL_NOTIFY,
                    WaitForKey,
                    &TerminalDev->SimpleTextInput,
                    &TerminalDev->SimpleTextInput.WaitForKey
                    );
    ASSERT_EFI_ERROR(Status);
    Status = pBS->CreateEvent(
                    EVT_NOTIFY_WAIT,
                    TPL_NOTIFY,
                    WaitForKey,
                    &TerminalDev->SimpleTextInput,
                    &TerminalDev->SimpleTextInputEx.WaitForKeyEx
                    );
    ASSERT_EFI_ERROR(Status);

    Status = pBS->CreateEvent(
                    EVT_NOTIFY_WAIT,
                    TPL_NOTIFY,
                    WaitForKey,
                    &TerminalDev->SimpleTextInput,
                    &TerminalDev->EfiKeycodeInput.WaitForKeyEx
                    );
    ASSERT_EFI_ERROR(Status);

    //Create a timeout event which will be used to distinguish between
    //escape sequences and lone escape characters.
    //
    //Certain keys (e.g. the arrow keys) produce an escape sequence,
    //that is, a sequence of characters which start with an escape character.
    //If an escape character is read, then we wait a certain amount of time
    //to see if other characters of various escape sequences follow.
    //If, after waiting a certain period of time, no escape sequence
    //characters are read, then we determine what we have is just a lone
    //escape character.

    Status = pBS->CreateEvent(
        EVT_TIMER,
        TPL_NOTIFY,
        NULL,
        NULL,
        &TerminalDev->TimeoutEvent
    );
    ASSERT_EFI_ERROR(Status);
    Status = pBS->CreateEvent(
        EVT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
        TPL_NOTIFY,
        WaitForEsc,
        &TerminalDev->TerminalEscIn,
        &TerminalDev->TimeoutEscEvent
    );
    ASSERT_EFI_ERROR(Status);
    Status = pBS->CreateEvent(
        EVT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
        TPL_NOTIFY,
        PollingTerminalKey,
        &TerminalDev->TerminalKeyIn,
        &TerminalDev->TerminalKeyEvent
    );
    ASSERT_EFI_ERROR(Status);

    ChildHandle = NULL;

    Status = pBS->InstallMultipleProtocolInterfaces(
            &ChildHandle,
            &gEfiDevicePathProtocolGuid, TerminalDev->ChildDevicePath,
            NULL
            );
    ASSERT_EFI_ERROR(Status);
    //If error remove Console Device Variable.

    Status = pBS->OpenProtocol(
                        Controller,
                        &gEfiSerialIoProtocolGuid,
                        (VOID**)&SerialIo,
                        This->DriverBindingHandle,
                        ChildHandle,
                        EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                        );

    ASSERT_EFI_ERROR(Status);

    if (ComPort != (UINT8)-1) {
        GetSetupValuesForSerialIoMode(ComPort, &SerialIoMode);

        Status = SerialIo->SetAttributes(SerialIo,
                                            SerialIoMode.BaudRate,
                                            Uart_Fifo_Size,
                                            0,
                                            SerialIoMode.Parity,
                                            SerialIoMode.DataBits,
                                            SerialIoMode.StopBits);
        if (EFI_ERROR(Status)) {
            goto SerialIoError;
        }

        Status = SerialIo->SetControl(SerialIo, SerialIoMode.ControlMask);
        if (EFI_ERROR(Status)) {
            goto SerialIoError;
        }
    }

    for(TerminalIndex=0;TerminalIndex<TotalTerminalDev;TerminalIndex++) {
        if(gTerminalDev[TerminalIndex]==NULL) {
            gTerminalDev[TerminalIndex]=TerminalDev;
            break;
        }
    }

    if (!EFI_ERROR(Status)) {
        Status = TerminalClearScreen(&TerminalDev->SimpleTextOutput);
    }
    Status = pBS->InstallMultipleProtocolInterfaces(
                &ChildHandle,
                &gEfiSimpleTextInProtocolGuid, &TerminalDev->SimpleTextInput,
                &gEfiSimpleTextInputExProtocolGuid, &TerminalDev->SimpleTextInputEx,
                &gAmiEfiKeycodeProtocolGuid, &TerminalDev->EfiKeycodeInput,
                &gEfiSimpleTextOutProtocolGuid, &TerminalDev->SimpleTextOutput,
                NULL
                );
    return EFI_SUCCESS;

SerialIoError:

    SerialIoStatus = pBS->UninstallMultipleProtocolInterfaces(
                            ChildHandle,
                            &gEfiDevicePathProtocolGuid, TerminalDev->ChildDevicePath,
                            &gEfiSimpleTextInProtocolGuid, &TerminalDev->SimpleTextInput,
                            &gEfiSimpleTextInputExProtocolGuid, &TerminalDev->SimpleTextInputEx,
                            &gAmiEfiKeycodeProtocolGuid, &TerminalDev->EfiKeycodeInput,
                            &gEfiSimpleTextOutProtocolGuid, &TerminalDev->SimpleTextOutput,
                            NULL
                        );

    if (EFI_ERROR(SerialIoStatus)) return EFI_DEVICE_ERROR;

    SerialIoStatus = pBS->CloseEvent(TerminalDev->SimpleTextInput.WaitForKey);
    if (EFI_ERROR(SerialIoStatus)) return EFI_DEVICE_ERROR;

    SerialIoStatus = pBS->CloseEvent(TerminalDev->SimpleTextInputEx.WaitForKeyEx);
    if (EFI_ERROR(SerialIoStatus)) return EFI_DEVICE_ERROR;

    SerialIoStatus = pBS->CloseEvent(TerminalDev->EfiKeycodeInput.WaitForKeyEx);
    if (EFI_ERROR(SerialIoStatus)) return EFI_DEVICE_ERROR;

    SerialIoStatus = pBS->CloseEvent(TerminalDev->TimeoutEvent);
    if (EFI_ERROR(SerialIoStatus)) return EFI_DEVICE_ERROR;

    SerialIoStatus = pBS->CloseEvent(TerminalDev->TimeoutEscEvent);
    if (EFI_ERROR(SerialIoStatus)) return EFI_DEVICE_ERROR;

    SerialIoStatus = pBS->CloseEvent(TerminalDev->TerminalKeyEvent);
    if (EFI_ERROR(SerialIoStatus)) return EFI_DEVICE_ERROR;

Error:
    pBS->CloseProtocol(
            Controller,
            &gEfiSerialIoProtocolGuid,
            This->DriverBindingHandle,
            Controller
            );

    if (TerminalDev) {
        pBS->FreePool(TerminalDev);
    }

    return EFI_DEVICE_ERROR;

}

/**
    Closes the SerialIo protocol.
    Uninstalls the SimpleTextIn and SimpleTextOut protocols.
    Closes timeout events like WaitForKey.

    @param  This
    @param  Controller
    @param  NumberOfChildren
    @param  ChildHandleBuffer

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalStop (
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN UINTN                        NumberOfChildren,
    IN EFI_HANDLE                   *ChildHandleBuffer
)
{
    EFI_STATUS                          Status;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL      *SimpleTextInput = NULL; 
    TERMINAL_DEV                        *TerminalDev = NULL; 
    UINT8                               TerminalIndex;
    ACPI_HID_DEVICE_PATH                *AcpiPrevDPNodePtr = NULL;
    EFI_DEVICE_PATH_PROTOCOL            *TerminalDevPath = NULL;
    EFI_DEVICE_PATH_PROTOCOL            *PciIoDevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL            *TruncatedTerminalDevPath = NULL;
    EFI_HANDLE                          Handle;
    EFI_PCI_IO_PROTOCOL                 *PciIo=NULL;
    UINTN                               Bus=0;
    UINTN                               Device=0;
    UINTN                               Function=0;
    UINTN                               Segment=0;
    UINTN                               Port=0;
    BOOLEAN                             AmiPciSerialDetected=FALSE;
    AMI_SERIAL_VENDOR_DEVICE_PATH       *AmiSerialDevicePath=NULL; 
    EFI_GUID                            AmiSerialVendorDevicePathGuid = AMI_SERIAL_VENDOR_DEVICE_PATH_GUID;


    if (!NumberOfChildren)
    {
        Status = pBS->CloseProtocol(
                        Controller,
                        &gEfiSerialIoProtocolGuid,
                        This->DriverBindingHandle,
                        Controller
                        );
        if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;
        return EFI_SUCCESS;
    }

    Status = pBS->CloseProtocol(
                    Controller,
                    &gEfiSerialIoProtocolGuid,
                    This->DriverBindingHandle,
                    *ChildHandleBuffer      //Only one child maximum.
                    );
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Status = pBS->OpenProtocol (
                    *ChildHandleBuffer,
                    &gEfiSimpleTextInProtocolGuid,
                    (VOID **)&SimpleTextInput,
                    This->DriverBindingHandle,
                    *ChildHandleBuffer,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    TerminalDev = OUTTER(SimpleTextInput, SimpleTextInput, TERMINAL_DEV);
    TerminalClearScreen(&TerminalDev->SimpleTextOutput);
    
    for(TerminalIndex=0;TerminalIndex<TotalTerminalDev;TerminalIndex++) {
        if(gTerminalDev[TerminalIndex]==TerminalDev) {
            gTerminalDev[TerminalIndex]=0;
            break;
        }
    }
    Status = pBS->OpenProtocol(Controller, &gEfiDevicePathProtocolGuid,
                                (VOID**)&TerminalDevPath, This->DriverBindingHandle, 
                                Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    
    if(!EFI_ERROR(Status)) {
        TruncatedTerminalDevPath=DPCopy(TerminalDevPath);
        do {
            if(TruncatedTerminalDevPath == NULL ) {
                break;
            }
            AcpiPrevDPNodePtr = (ACPI_HID_DEVICE_PATH *)DPGetLastNode(TruncatedTerminalDevPath);
            if((AcpiPrevDPNodePtr->Header.Type == ACPI_DEVICE_PATH)) {
                pBS->FreePool(TruncatedTerminalDevPath);
                goto UninstallProtocol;
            }
            
            //
            // Check whether AMI Serial Vendor device path is present.
            //    
            AmiSerialDevicePath = (AMI_SERIAL_VENDOR_DEVICE_PATH *)AcpiPrevDPNodePtr;
            if( (AmiSerialDevicePath->VendorDevicePath.Header.SubType == HW_VENDOR_DP)) {
                if( guidcmp(&(AmiSerialDevicePath->VendorDevicePath.Guid)
                            ,&AmiSerialVendorDevicePathGuid)== 0 ) {
                    AmiPciSerialDetected = TRUE;
                    break;
                }
            }

            AmiSerialDevicePath = (AMI_SERIAL_VENDOR_DEVICE_PATH *)TruncatedTerminalDevPath;
            TruncatedTerminalDevPath = DPCut(TruncatedTerminalDevPath);
            pBS->FreePool(AmiSerialDevicePath);
        }while(TruncatedTerminalDevPath != NULL);
    }
    
    if ( AmiPciSerialDetected ) {
        Bus      = AmiSerialDevicePath->Bus;
        Device   = AmiSerialDevicePath->Device;
        Function = AmiSerialDevicePath->Function;
        Port     = AmiSerialDevicePath->Port;

        Status = ClearSerialPortsEnabledVar_Pci((UINT8)Bus, (UINT8)Device, (UINT8)Function, 
                                                  (UINT8)Port, AmiPciSerialDetected);
        
        pBS->FreePool(TruncatedTerminalDevPath);

    } else {

        PciIoDevicePath = DPCut(TerminalDevPath);
        
        Status = pBS->LocateDevicePath(&gEfiPciIoProtocolGuid, 
                                       &PciIoDevicePath, 
                                       &Handle);
        if (!EFI_ERROR(Status)) {
            Status = pBS->OpenProtocol(Handle, &gEfiPciIoProtocolGuid,(VOID**) &PciIo,
                                        This->DriverBindingHandle, Controller,
                                        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
            if (!EFI_ERROR(Status)) {
                Status = PciIo->GetLocation(PciIo, &Segment, &Bus, 
                                            &Device, &Function);
                if (!EFI_ERROR(Status)) {
                    Status = ClearSerialPortsEnabledVar_Pci((UINT8)Bus, (UINT8)Device, (UINT8)Function, 
                                                            (UINT8)Port, AmiPciSerialDetected);
                }
            }
        }
        pBS->FreePool(PciIoDevicePath);
        pBS->FreePool(TruncatedTerminalDevPath);
    }
    
UninstallProtocol:
    Status = pBS->UninstallMultipleProtocolInterfaces(
            *ChildHandleBuffer,         //Only one child maximum.
            &gEfiDevicePathProtocolGuid, TerminalDev->ChildDevicePath,
            &gEfiSimpleTextInProtocolGuid, &TerminalDev->SimpleTextInput,
            &gEfiSimpleTextInputExProtocolGuid, &TerminalDev->SimpleTextInputEx,
            &gAmiEfiKeycodeProtocolGuid, &TerminalDev->EfiKeycodeInput,
            &gEfiSimpleTextOutProtocolGuid, &TerminalDev->SimpleTextOutput,
            NULL
            );
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Status = pBS->CloseEvent(TerminalDev->SimpleTextInput.WaitForKey);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Status = pBS->CloseEvent(TerminalDev->SimpleTextInputEx.WaitForKeyEx);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Status = pBS->CloseEvent(TerminalDev->EfiKeycodeInput.WaitForKeyEx);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Status = pBS->CloseEvent(TerminalDev->TimeoutEvent);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Status = pBS->CloseEvent(TerminalDev->TimeoutEscEvent);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Status = pBS->CloseEvent(TerminalDev->TerminalKeyEvent);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    pBS->FreePool(TerminalDev);
    TerminalDev = NULL;

    return EFI_SUCCESS;
}

/**
    The function analyzes the device path to determine
    if the serial port is a SuperIo serial port or a
    PCI serial port.
    If it's a SuperIo serial port, it uses the EISA Pnp ID
    in the Acpi device path node to get the com port number.
    If it's a PCI serial port, it uses the PciIo protocol to
    get the com port number.
    This is a slight modification of SerialIo.c's GetSerialIo_DP.

    @param  This
    @param  Controller
    @param  ComPort
    @param  IsPciSerialInterface

    @retval EFI_STATUS

**/

EFI_STATUS
GetComPortNumber (
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN OUT UINT8                    *ComPort,
    IN OUT BOOLEAN                  *IsPciSerialInterface
)
{
    EFI_STATUS Status;
    ACPI_HID_DEVICE_PATH     *AcpiPrevDPNodePtr = NULL;
    EFI_DEVICE_PATH_PROTOCOL *TerminalDevPath = NULL;
    EFI_DEVICE_PATH_PROTOCOL *TruncatedTerminalDevPath = NULL;

    EFI_PCI_IO_PROTOCOL *PciIo=NULL;
    UINTN Segment=0;
    UINTN Bus=0;
    UINTN Device=0;
    UINTN Function=0;
    UINT8 Port=0;
    EFI_DEVICE_PATH_PROTOCOL *DevicePathProtocol=NULL;
    EFI_DEVICE_PATH_PROTOCOL *PciIoDevicePath=NULL;
    EFI_DEVICE_PATH_PROTOCOL *PciIoDevicePathCopy=NULL;
    EFI_DEVICE_PATH_PROTOCOL *RmvdUartDevicePath=NULL;
    AMI_SERIAL_VENDOR_DEVICE_PATH *AmiSerialDevicePath=NULL; 
    VENDOR_DEVICE_PATH *VendorDPNodePtr=NULL;
    EFI_GUID AmiSerialVendorDevicePathGuid = AMI_SERIAL_VENDOR_DEVICE_PATH_GUID;
    BOOLEAN AmiPciSerialDetected=FALSE;

    EFI_HANDLE Handle=0;

    *ComPort = -1;
    *IsPciSerialInterface = FALSE; 

    // Get device path from Controller handle.
    //
    Status = pBS->OpenProtocol (
                        Controller,
                        &gEfiDevicePathProtocolGuid,
                        (void **)&TerminalDevPath,
                        This->DriverBindingHandle,
                        Controller,   
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL
                        );

    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    TruncatedTerminalDevPath=DPCopy(TerminalDevPath);
    do {
        if(TruncatedTerminalDevPath == NULL ) {
            break;
        }
        AcpiPrevDPNodePtr = (ACPI_HID_DEVICE_PATH *)DPGetLastNode(TruncatedTerminalDevPath);
        if((AcpiPrevDPNodePtr->Header.Type == ACPI_DEVICE_PATH)) {
            break;
        }
        //
        // Check whether AMI Serial Vendor device path is present.
        //
        AmiSerialDevicePath = (AMI_SERIAL_VENDOR_DEVICE_PATH *)AcpiPrevDPNodePtr;
        if( (AmiSerialDevicePath->VendorDevicePath.Header.SubType == HW_VENDOR_DP)) {
            if( guidcmp(&(AmiSerialDevicePath->VendorDevicePath.Guid)
                        ,&AmiSerialVendorDevicePathGuid)== 0 ) {
                AmiPciSerialDetected = TRUE;
                break;
            }
        }

        AmiSerialDevicePath = (AMI_SERIAL_VENDOR_DEVICE_PATH *)TruncatedTerminalDevPath;
        TruncatedTerminalDevPath = DPCut(TruncatedTerminalDevPath);
        pBS->FreePool(AmiSerialDevicePath);
    }while(TruncatedTerminalDevPath != NULL);

    VendorDPNodePtr = (VENDOR_DEVICE_PATH*)TerminalDevPath;
    if ((AcpiPrevDPNodePtr->Header.Type == ACPI_DEVICE_PATH) &&
        (AcpiPrevDPNodePtr->Header.SubType == ACPI_DP) &&
        (AcpiPrevDPNodePtr->HID == EISA_PNP_ID(0x501)))  {
        *ComPort=SetSerialPortsEnabledVar_Sio(AcpiPrevDPNodePtr->UID);
        pBS->FreePool(TruncatedTerminalDevPath);
        if (*ComPort == 0xff) {
            Status=EFI_UNSUPPORTED;
        } else {
            Status=EFI_SUCCESS;
        }
    } else if((VendorDPNodePtr->Header.Type == HARDWARE_DEVICE_PATH) &&
        (VendorDPNodePtr->Header.SubType == HW_VENDOR_DP) &&
        (guidcmp(&VendorDPNodePtr->Guid,&gEfiSerialIoProtocolGuid)== 0) ){
        //
        // This Serial IO is from the Debugger.
        //
        *ComPort=SetSerialPortsEnabledVar_Sio(0xFF);
        Status = EFI_SUCCESS;
    } else {
        *IsPciSerialInterface = TRUE;
        Status=pBS->OpenProtocol(Controller,
                                    &gEfiDevicePathProtocolGuid,
                                    (VOID**)&DevicePathProtocol,
                                    This->DriverBindingHandle,
                                    Controller,
                                    EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        PciIoDevicePath = DPCut(DevicePathProtocol);
        PciIoDevicePathCopy = DPCopy(PciIoDevicePath);
        //
        // Removing the UART device path for the device comparison,
        // as it has only uart device properties and it can be changed for same device also
        //                   
        while( PciIoDevicePathCopy->Type != END_DEVICE_PATH ){
            if( (PciIoDevicePathCopy->Type == MESSAGING_DEVICE_PATH) &&
                (PciIoDevicePathCopy->SubType == MSG_UART_DP) ){
                 PciIoDevicePathCopy= NEXT_NODE(PciIoDevicePathCopy);
                continue;
            }
            RmvdUartDevicePath=DPAddNode(RmvdUartDevicePath,PciIoDevicePathCopy);
            PciIoDevicePathCopy= NEXT_NODE(PciIoDevicePathCopy);
        }
        PciIoDevicePathCopy = DPCopy(RmvdUartDevicePath);
        pBS->FreePool(RmvdUartDevicePath);
 
        if ( AmiPciSerialDetected ) {
            Bus      = AmiSerialDevicePath->Bus;
            Device   = AmiSerialDevicePath->Device;
            Function = AmiSerialDevicePath->Function;
            Port     = AmiSerialDevicePath->Port;

            pBS->FreePool(TruncatedTerminalDevPath);

        } else {
            Status = pBS->LocateDevicePath(&gEfiPciIoProtocolGuid,
                                            &PciIoDevicePath,
                                            &Handle);
            if (EFI_ERROR(Status)) goto Error_Devicepath;
            Status = pBS->OpenProtocol(Handle, &gEfiPciIoProtocolGuid,(VOID**) &PciIo,
                                        This->DriverBindingHandle, Controller,
                                        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
            if (EFI_ERROR(Status)) goto Error_Devicepath;
            Status = PciIo->GetLocation(PciIo, &Segment, &Bus, 
                                        &Device, &Function);
            if (EFI_ERROR(Status)) goto Error_Devicepath;
        }

        *ComPort = SetSerialPortsEnabledVar_Pci(PciIoDevicePathCopy,
                                                (UINT8)Bus,
                                                (UINT8)Device, 
                                                (UINT8)Function,
                                                Port,
                                                AmiPciSerialDetected);

        if (*ComPort == 0xff) {
            Status=EFI_UNSUPPORTED;
        } else {
            Status=EFI_SUCCESS;
        }
Error_Devicepath:

        pBS->FreePool(PciIoDevicePathCopy);
    }

    return Status;
}

/**
    Gets the Spcr table values for SuperIo com ports
    (Base Address, IRQ) from the AMI_SIO_PROTOCOL installed 
    on the controller and writes them to the global table of spcr
    values gSpcrInfo.  

    @param  This
    @param  Controller

    @retval EFI_STATUS

**/

EFI_STATUS
SetComSuperIoSpcrTableValues (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE Controller
)
{
    EFI_STATUS                       Status;
#if EFI_SIO_PROTOCOL_SUPPORT
    EFI_SIO_PROTOCOL                 *EfiSioProtocol=NULL;
    ACPI_RESOURCE_HEADER_PTR         ResourceList;
    ACPI_SMALL_RESOURCE_HEADER       *Hdr=NULL;
    EFI_DEVICE_PATH_PROTOCOL         *EfiSioDevicePath=NULL;
#else
    AMI_SIO_PROTOCOL                 *AmiSioProtocol=NULL;
    T_ITEM_LIST                      *ResourcesList=NULL;
    ASLRF_S_HDR                      *Header=NULL;
    EFI_DEVICE_PATH_PROTOCOL         *AmiSioDevicePath=NULL; 
    BOOLEAN                          SetFlag=FALSE;
    UINTN                            i=0;
#endif
    UINTN                            j=0;
    UINTN                            IrqInBitPosition=0;
    EFI_DEVICE_PATH_PROTOCOL         *DevicePathProtocol=NULL;
    EFI_HANDLE                       Handle=0;
    ACPI_SPCR_TABLE_INFO             *SpcrInfo = &gSpcrInfo;
    UART_FLOW_CONTROL_DEVICE_PATH    *DevPathLastNode = NULL;
    EFI_DEVICE_PATH_PROTOCOL         *TempDevPath = NULL;
    
    Status=pBS->OpenProtocol(Controller,
                                &gEfiDevicePathProtocolGuid,
                                (VOID**)&DevicePathProtocol,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    ASSERT_EFI_ERROR(Status);

#if EFI_SIO_PROTOCOL_SUPPORT
    
    DevPathLastNode = (UART_FLOW_CONTROL_DEVICE_PATH *) DPGetLastNode(DevicePathProtocol);
    if(CompareGuid(&DevPathLastNode->Guid, &gEfiUartDevicePathGuid)) {
        EfiSioDevicePath = DPCut(DevicePathProtocol);
    } else {
        EfiSioDevicePath = DPCopy(DevicePathProtocol);
    }
    
    TempDevPath = EfiSioDevicePath;

    Status = pBS->LocateDevicePath(&gEfiDevicePathProtocolGuid,
                                    &EfiSioDevicePath,
                                    &Handle);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) {
        pBS->FreePool(TempDevPath);
        return Status;
    }

    Status = pBS->OpenProtocol(Handle,
                &gEfiSioProtocolGuid,
                (VOID**) &EfiSioProtocol,
                This->DriverBindingHandle,
                Controller,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if(EFI_ERROR(Status)) {
        pBS->FreePool(TempDevPath);
        return Status;
    }

    ResourceList.SmallHeader = NULL;
    Status = EfiSioProtocol->GetResources(EfiSioProtocol,
                                          &ResourceList);

    if(EFI_ERROR(Status)) {
        pBS->FreePool(TempDevPath);
        return Status;
    }

    if (ResourceList.SmallHeader){
        for(Hdr=ResourceList.SmallHeader; (Hdr->Byte)!=ASLV_END_TAG_HDR; Hdr+=((sizeof(ACPI_SMALL_RESOURCE_HEADER))+Hdr->Bits.Length)) {
            switch(Hdr->Bits.Name) {
                case ASLV_RT_FixedIO:
                    SpcrInfo->BaseAddress = ((ASLR_FixedIO*)Hdr)->_BAS;
                    break;
                case ASLV_RT_IO:
                    SpcrInfo->BaseAddress=((ASLR_IO*)Hdr)->_MIN;
                    break;
                case ASLV_RT_IRQ:
                    IrqInBitPosition = ((ASLR_IRQNoFlags*)Hdr)->_INT;
                    j = 1;
                    while (IrqInBitPosition != 0) {
                        IrqInBitPosition = IrqInBitPosition >> 1;
                        j++;
                    }
                    SpcrInfo->Irq = (UINT8)j;
                    break;
            }
        }
    }
#else
    
    DevPathLastNode = (UART_FLOW_CONTROL_DEVICE_PATH *) DPGetLastNode(DevicePathProtocol);
    if(CompareGuid(&DevPathLastNode->Guid, &gEfiUartDevicePathGuid)) {
        AmiSioDevicePath = DPCut(DevicePathProtocol);
    } else {
        AmiSioDevicePath = DPCopy(DevicePathProtocol);
    }
    
    TempDevPath = AmiSioDevicePath;
    
    Status = pBS->LocateDevicePath(&gEfiDevicePathProtocolGuid,
                                    &AmiSioDevicePath,
                                    &Handle);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) {
        pBS->FreePool(TempDevPath);
        return Status;
    }
    
    Status = pBS->OpenProtocol(Handle,
                                &gEfiAmiSioProtocolGuid,
                               (VOID**) &AmiSioProtocol,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if(EFI_ERROR(Status)) {
        pBS->FreePool(TempDevPath);
        return Status;
    }

    Status = AmiSioProtocol->CurrentRes(AmiSioProtocol,
                                        SetFlag,
                                        &ResourcesList);
    if (EFI_ERROR(Status)) {
        pBS->FreePool(TempDevPath);
        return Status;
    }

    if(ResourcesList){
        for(i=0; i<ResourcesList->ItemCount; i++){
            Header=(ASLRF_S_HDR*)ResourcesList->Items[i];
            switch(Header->Name) {
                case ASLV_RT_FixedIO:
                    SpcrInfo->BaseAddress=((ASLR_FixedIO*)Header)->_BAS;
                    break;
                case ASLV_RT_IO:
                    SpcrInfo->BaseAddress=((ASLR_IO*)Header)->_MIN;
                    break;
                case ASLV_RT_IRQ:
                    IrqInBitPosition = ((ASLR_IRQNoFlags*)Header)->_INT;
                    j = 1;
                    while (IrqInBitPosition != 0) {
                        IrqInBitPosition = IrqInBitPosition >> 1;
                        j++;
                    }
                SpcrInfo->Irq = (UINT8)j;
                break;
            }
        }
    }
#endif
    
    pBS->FreePool(TempDevPath);

    return Status;
}

/**
    Gets the Spcr table values for Pci com ports
    (Base Address, IRQ, device id, vendor id,
    bus number, device number, function number)
    and stores them in the global table of spcr values gSpcrInfo.

    @param  This
    @param  Controller

    @retval EFI_STATUS

**/

EFI_STATUS
SetPciSerialSpcrTableValues (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE   Controller
)
{
    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIo = NULL;
    UINT8                       i = 0;
    UINT64                      Supports = 1;
    ASLR_QWORD_ASD              *Resources = NULL;
    ACPI_SPCR_TABLE_INFO        *SpcrInfo = &gSpcrInfo;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePathProtocol = NULL;
    EFI_DEVICE_PATH_PROTOCOL    *PciIoDevicePath = NULL;
    EFI_HANDLE                  Handle = 0;

    Status=pBS->OpenProtocol(Controller, 
                             &gEfiDevicePathProtocolGuid,
                             (VOID**)&DevicePathProtocol,
                             This->DriverBindingHandle,
                             Controller,
                             EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    ASSERT_EFI_ERROR(Status);
    PciIoDevicePath = DPCut(DevicePathProtocol);

    Status = pBS->LocateDevicePath(&gEfiPciIoProtocolGuid,
                                    &PciIoDevicePath,
                                    &Handle);
    ASSERT_EFI_ERROR(Status);

        Status = pBS->OpenProtocol(Handle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo,
                                    This->DriverBindingHandle, Controller,
                                    EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(Status)) return Status;
    
        for (i = 0; i < PCI_MAX_BAR_NO; i++) {
            PciIo->GetBarAttributes (PciIo, i, &Supports, (VOID**)&Resources);
            if (Resources->Type == ASLRV_SPC_TYPE_IO) {
                SpcrInfo->BaseAddress = Resources->_MIN;
                break;
            }
        }

        Status = PciIo->GetLocation(PciIo, &SpcrInfo->Segment, &SpcrInfo->Bus,
                                    &SpcrInfo->Device, &SpcrInfo->Function);
        if (EFI_ERROR(Status)) return Status;

        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8,
                                    PCI_INTLINE, 1, &SpcrInfo->Irq);
        if (EFI_ERROR(Status)) return Status;

        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 
                                    PCI_VID, 1, &SpcrInfo->VendorId);
        if (EFI_ERROR(Status)) return Status;

        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 
                                    PCI_DID, 1, &SpcrInfo->DeviceId);
        if (EFI_ERROR(Status)) return Status;

    return Status; 
}

#if ACPI_SUPPORT

#define ACPI_REV1 1
#define SPCR_HEADER_REVISION 2

/**
    Creates an spcr table and register it with 
    AcpiSupportProtocol->SetAcpiTable().

    @param  AcpiSupportProtocol
    @param  SpcrInfo

    @retval VOID

**/

VOID
CreateSpcrAcpiTable (
    EFI_ACPI_SUPPORT_PROTOCOL *AcpiSupportProtocol,
    ACPI_SPCR_TABLE_INFO *SpcrInfo
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    PSPCR_30    spcr_tbl = NULL;
    UINT8       i;

    //get the setup variable

    spcr_tbl = MallocZ(sizeof(SPCR_30));
    ASSERT(spcr_tbl);

    spcr_tbl->Header.Signature = SPCR_SIG;
    spcr_tbl->Header.Length = sizeof(SPCR_30);
    spcr_tbl->Header.Revision = SPCR_HEADER_REVISION;
    spcr_tbl->Header.Checksum = 0;

    for (i=0;i<6;i++) spcr_tbl->Header.OemId[i]=SPCR_OEM_ID[i];
    for (i=0;i<8;i++)spcr_tbl->Header.OemTblId[i]=SPCR_OEM_TBL_ID[i];

    spcr_tbl->Header.OemRev = (UINT32)AcpiOemRev;
    spcr_tbl->Header.CreatorId = 0x2E494D41;    //"AMI."
    spcr_tbl->Header.CreatorRev = CoreRevision;

    //InterfaceType: SMC47B27x has 16C550 UARTs
    spcr_tbl->InterfaceType = SpcrInterfaceType;
    spcr_tbl->BaseAddress.AddrSpcID = SpcrAddrSpcId;
    spcr_tbl->BaseAddress.RegBitWidth = SpcrAddrBitWidth;
    spcr_tbl->BaseAddress.RegBitOffs = SpcrAddrBitOffset;
    spcr_tbl->BaseAddress.AccessSize = SpcrAccessSize;
    spcr_tbl->BaseAddress.Address = SpcrInfo->BaseAddress;

    spcr_tbl->Irq = SpcrInfo->Irq;
    spcr_tbl->GlobalSystemInt = SpcrInfo->Irq;
    //InterruptType: We've support PIC and SAPIC modes
    spcr_tbl->InterruptType = SpcrInterruptType;

    spcr_tbl->BaudRate = SpcrInfo->BaudRate;
    spcr_tbl->Parity = SpcrParity;
    spcr_tbl->StopBits = SpcrStopBits;
    spcr_tbl->FlowControl = SpcrInfo->FlowControl;
    spcr_tbl->TerminalType = SpcrInfo->TerminalType;

    if (SpcrInfo->PortNumber >= gTotalSioSerialPorts) {
        spcr_tbl->PciDeviceId = SpcrInfo->DeviceId;
        spcr_tbl->PciVendorId = SpcrInfo->VendorId;
        spcr_tbl->PciBusNumber = (UINT8) SpcrInfo->Bus;
        spcr_tbl->PciDeviceNumber = (UINT8) SpcrInfo->Device;
        spcr_tbl->PciFunctionNumber = (UINT8) SpcrInfo->Function;
        spcr_tbl->PciFlags = 1;
        spcr_tbl->PciSegment = (UINT8) SpcrInfo->Segment;
    } else {
        spcr_tbl->PciDeviceId = SpcrPciDeviceId;
        spcr_tbl->PciVendorId = SpcrPciVendorId;
        spcr_tbl->PciBusNumber = SpcrPciBusNumber;
        spcr_tbl->PciDeviceNumber = SpcrPciDeviceNumber;
        spcr_tbl->PciFunctionNumber = SpcrPciFunctionNumber;
        spcr_tbl->PciFlags = SpcrPciFlags;
        spcr_tbl->PciSegment = SpcrPciSegment;
    }

    //Add table 
    Status = AcpiSupportProtocol->SetAcpiTable(AcpiSupportProtocol,
                                                spcr_tbl,
                                                TRUE,
                                                EFI_ACPI_TABLE_VERSION_ALL,
                                                &gSpcrTableHandle);
    ASSERT_EFI_ERROR(Status);

    //free memory used for table image
    pBS->FreePool(spcr_tbl);
}

/**
    Calls CreateSpcrAcpiTable when the AcpiSupport protocol
    becomes available. 

    @param  Event
    @param  Context

    @retval EFI_STATUS

**/

VOID
EFIAPI
CreateSpcrAcpiTableWrapper (
    IN EFI_EVENT  Event,
    IN VOID       *Context
)
{
    EFI_STATUS Status;
    EFI_ACPI_SUPPORT_PROTOCOL   *AcpiSupportProtocol = NULL;

    Status=pBS->LocateProtocol(&gEfiAcpiSupportGuid,
                                NULL,
                                &AcpiSupportProtocol);
    if(EFI_ERROR(Status)) return;

    CreateSpcrAcpiTable(AcpiSupportProtocol,
                        (ACPI_SPCR_TABLE_INFO *)Context);
    //Kill the Event
    pBS->CloseEvent(Event);
}

/**
    Gets the Spcr values which are configurable in Setup
    (the port number, terminal type, etc.).
    Calls CreateSpcrAcpiTable or calls RegisterProtocolCallback
    to call CreateSpcrAcpiTableWrapper when the
    AcpiSupportProtocol is installed.

    @param  Port
    @param  This
    @param  Controller

    @retval VOID

**/

VOID
AcpiSpcrTable (
    IN UINT8 Port,
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE Controller
)
{

    EFI_STATUS                          Status = EFI_SUCCESS;
    UINT8                               AcpiSpcrTableComPort = 0;
    UINT8                               AcpiSpcrTableConsoleRedirectionEnable = 0;
    UINT8                               AcpiSpcrTableTerminalType = 0;
    UINT8                               AcpiSpcrTableBaudRate = 0;
    UINT8                               AcpiSpcrTableFlowControl = 0;
    BOOLEAN                             IsPci = FALSE;
    BOOLEAN                             IsMmIo = FALSE;
    UINT8                               MmioWidth;
    BOOLEAN                             AmiSerialDetected = FALSE;
    EFI_ACPI_SUPPORT_PROTOCOL           *AcpiSupportProtocol = NULL;
    AMI_SERIAL_PROTOCOL                 *AmiSerialProtocol = NULL;
    EFI_HANDLE                          Handle = 0;
    EFI_HANDLE                          *HandleBuffer = NULL;
    UINTN                               NumHandles;
    UINTN                               HandleIndex;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *Entries = NULL;
    UINTN                               Count;
    UINTN                               CountIndex;

    GetAcpiSpcrTableValues(&AcpiSpcrTableComPort,
                            &AcpiSpcrTableConsoleRedirectionEnable,
                            &AcpiSpcrTableTerminalType,
                            &AcpiSpcrTableBaudRate,
                            &AcpiSpcrTableFlowControl);

    if (Port != AcpiSpcrTableComPort) return;

    if (!AcpiSpcrTableConsoleRedirectionEnable) return;

    // The AmiSerial Protocol and SerialIo Protocol are installed on different Handles.
    // The relationship between SerialIo protocol and AmiSerial Protocol is set as CHILD controller.
    // We have to use this relation to get the Handle for AmiSerial Protocol with SerialIo Protocol.

    // Check if AmiSerialProtocol is installed in any Handle
    Status = pBS->LocateHandleBuffer(ByProtocol,
                                     &gAmiSerialProtocolGuid,
                                     NULL,
                                     &NumHandles,
                                     &HandleBuffer);
    if (!EFI_ERROR(Status)) {
        // AmiSerialProtocol is installed in some Handles
        for (HandleIndex = 0; HandleIndex < NumHandles; HandleIndex++) {
            // Get the Information of all the protools that have opened the
            // AmiSerialProtocol
            Status = pBS->OpenProtocolInformation(HandleBuffer[HandleIndex],
                                                  &gAmiSerialProtocolGuid,
                                                  &Entries,
                                                  &Count);
            ASSERT_EFI_ERROR(Status);

            for(CountIndex = 0; CountIndex< Count; CountIndex++) {
                // Check if AmiSerial Protocol is opened by an attribute
                // EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                if(Entries[CountIndex].Attributes != EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)
                    continue;
                // Check if the Opening Protocol is SerialIo Protocol
                // This can be known by checking if the Controller Handle is same as the
                // Handle on which SerialIo Protocol is installed
                if(Entries[CountIndex].ControllerHandle == Controller) {
                    Handle = HandleBuffer[HandleIndex];
                    AmiSerialDetected = TRUE;
                    break;
                }
            }
        }
    }

    if(AmiSerialDetected) {
        // OpenProcol For AmiSerial Protocol
        Status=pBS->OpenProtocol(Handle,
                                 &gAmiSerialProtocolGuid,
                                 (VOID **)&AmiSerialProtocol,
                                 This->DriverBindingHandle,
                                 Controller,
                                 EFI_OPEN_PROTOCOL_GET_PROTOCOL );
        ASSERT_EFI_ERROR(Status);
        // Check if it is MmIo
        AmiSerialProtocol->CheckPciMmio(AmiSerialProtocol, &IsPci, &IsMmIo, &MmioWidth);
        if (IsPci) {
            // Filling the Base Address
            AmiSerialProtocol->GetBaseAddress(AmiSerialProtocol,&gSpcrInfo.BaseAddress);
            // Filling the IRQ Number
            AmiSerialProtocol->GetSerialIRQ(AmiSerialProtocol, &gSpcrInfo.Irq);
            // Filling the Bus, Device and Function Number
            AmiSerialProtocol->GetPciLocation(AmiSerialProtocol,
                                              &gSpcrInfo.Bus,
                                              &gSpcrInfo.Device,
                                              &gSpcrInfo.Function,
                                              &gSpcrInfo.PortNumber);
            // Checking if Bus, Device and Function Number are valid
            if ((gSpcrInfo.Bus != 0xFF)&&(gSpcrInfo.Device != 0xFF)&&(gSpcrInfo.Function != 0xFF)) {
                // Filling the Vendor ID
                gSpcrInfo.VendorId = *(UINT16*)PCI_CFG_ADDR(gSpcrInfo.Bus,
                                                             gSpcrInfo.Device,
                                                             gSpcrInfo.Function,
                                                             PCI_VID);
                // Filling the Device ID
                gSpcrInfo.DeviceId = *(UINT16*)PCI_CFG_ADDR(gSpcrInfo.Bus,
                                                             gSpcrInfo.Device,
                                                             gSpcrInfo.Function,
                                                             PCI_DID);
            }
        } else {
            // Filling the Base Address
            AmiSerialProtocol->GetBaseAddress(AmiSerialProtocol,&gSpcrInfo.BaseAddress);
            // Filling the IRQ Number
            AmiSerialProtocol->GetSerialIRQ(AmiSerialProtocol, &gSpcrInfo.Irq);
        }
        if (IsMmIo) {
            SpcrAddrSpcId = GAS_SYS_MEM;
        } else {
            SpcrAddrSpcId = GAS_SYS_IO;
        }
    } else {
        if (Port >= gTotalSioSerialPorts) {
            Status = SetPciSerialSpcrTableValues(This, Controller);
        } else {
            Status = SetComSuperIoSpcrTableValues(This, Controller);
        }
    }

    gSpcrInfo.PortNumber = AcpiSpcrTableComPort;
    gSpcrInfo.TerminalType = AcpiSpcrTableTerminalType;
    gSpcrInfo.BaudRate = AcpiSpcrTableBaudRate;
    gSpcrInfo.FlowControl = AcpiSpcrTableFlowControl;

    Status=pBS->LocateProtocol(&gEfiAcpiSupportGuid,
                                NULL,
                                &AcpiSupportProtocol);
    if(EFI_ERROR(Status)) {
        Status=RegisterProtocolCallback(&gEfiAcpiSupportGuid,
                                        CreateSpcrAcpiTableWrapper,
                                        &gSpcrInfo,
                                        &gAcpiSupportEvent,
                                        &gAcpiSupportRegistration);
        ASSERT_EFI_ERROR(Status);
    } else {
        CreateSpcrAcpiTable(AcpiSupportProtocol, &gSpcrInfo);
    }

}
#endif

/**
    Function to clear the Fifo buffers

    @param  Event
    @param  Context

    @retval VOID

**/

VOID
EFIAPI
ClearFiFoBuffers (
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
    UINTN            Index;
    UINTN            i;
    TERMINAL_DEV    *TerminalDev;

    for (Index = 0; Index < TotalTerminalDev; Index++) {
        TerminalDev = gTerminalDev[Index];
        if (TerminalDev == 0) {
            continue;
        } else {
            for (i = 0; i < BUFFER_SIZE; i++) {
                TerminalDev->RawFIFO.Buffer[i] = 0;
                TerminalDev->UnicodeFIFO.Buffer[i] = 0;
            }

            TerminalDev->RawFIFO.Head = TerminalDev->RawFIFO.Tail;
            TerminalDev->UnicodeFIFO.Head = TerminalDev->UnicodeFIFO.Tail;
        }
    }

    pBS->CloseEvent(Event);
    return;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
