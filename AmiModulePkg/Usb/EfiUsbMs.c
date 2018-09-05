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

/** @file EfiUsbMs.c
    EFI USB Mouse Driver

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"
#include "ComponentName.h"
#include "UsbBus.h"


#define USBMS_DRIVER_VERSION 2

#define USB_MOUSE_DEV_SIGNATURE   EFI_SIGNATURE_32('u','m','o','u')

#ifndef CR
#define CR(record, TYPE, field, signature) _CR(record, TYPE, field) 
#endif
#define USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL(a,b) \
    CR(a, USB_MOUSE_DEV, b, USB_MOUSE_DEV_SIGNATURE)

typedef struct
{
    UINTN                           Signature;
    UINT8                           NumberOfButtons;
    INT32                           XLogicMax;
    INT32                           XLogicMin;
    INT32                           YLogicMax;
    INT32                           YLogicMin;
    EFI_SIMPLE_POINTER_PROTOCOL     SimplePointerProtocol;
    EFI_SIMPLE_POINTER_MODE         Mode;
    UINT8                           Endpoint;
    EFI_USB_IO_PROTOCOL             *UsbIo;
} USB_MOUSE_DEV;

VOID
EFIAPI
UsbMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

EFI_STATUS
UpdateUsbMouseData (
    EFI_SIMPLE_POINTER_PROTOCOL  *This, 
	EFI_SIMPLE_POINTER_STATE	*State
  );

//
// Mouse Protocol
//
EFI_STATUS
EFIAPI
GetMouseState(
  IN   EFI_SIMPLE_POINTER_PROTOCOL  *This,
  OUT  EFI_SIMPLE_POINTER_STATE     *MouseState
);

EFI_STATUS
EFIAPI
UsbMouseReset(
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  );

extern USB_GLOBAL_DATA *gUsbData;

BOOLEAN                         StateChanged; 
UINT8                           ButtonsState; 
EFI_SIMPLE_POINTER_STATE        MsState;



/**
    Initialize USB mouse device and all private data structures.

    @param VOID

    @retval EFI_SUCCESS or EFI_ERROR

**/

EFI_STATUS
InitUSBMouse()
{
    EfiZeroMem (&MsState, sizeof(EFI_SIMPLE_POINTER_STATE));
    ButtonsState = 0;
    StateChanged = FALSE; 
    return EFI_SUCCESS;
}  

/**
    Installs SimplePointerProtocol interface on a given controller.

    @param Controller - controller handle to install interface on.

    @retval VOID

**/
VOID
InstallUSBMouse(
    EFI_HANDLE Controller,
    EFI_USB_IO_PROTOCOL *UsbIo,
    DEV_INFO *DevInfo
)
{
    USB_MOUSE_DEV       *UsbMouse; 
    EFI_STATUS Status;

    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(USB_MOUSE_DEV),
                    &UsbMouse);
    
    ASSERT(Status == EFI_SUCCESS);

    if (EFI_ERROR(Status)) {
        return;
    }

    EfiZeroMem(UsbMouse, sizeof(USB_MOUSE_DEV));

    //
    // Initialize UsbMouseDevice
    //
    UsbMouse->Signature = USB_MOUSE_DEV_SIGNATURE;
    UsbMouse->SimplePointerProtocol.GetState = GetMouseState;
    UsbMouse->SimplePointerProtocol.Reset = UsbMouseReset;
    UsbMouse->SimplePointerProtocol.Mode = &UsbMouse->Mode;

    UsbMouse->NumberOfButtons = 2;
    UsbMouse->XLogicMax = UsbMouse->YLogicMax = 127;
    UsbMouse->XLogicMin = UsbMouse->YLogicMin = -127;

    UsbMouse->Mode.LeftButton = TRUE;
    UsbMouse->Mode.RightButton = TRUE;
    UsbMouse->Mode.ResolutionX = 8;
    UsbMouse->Mode.ResolutionY = 8;
    UsbMouse->Mode.ResolutionZ = 1; 

    UsbMouse->UsbIo = UsbIo;
    UsbMouse->Endpoint = DevInfo->IntInEndpoint;

    UsbMouseReset(NULL, FALSE);
 
    Status = gBS->CreateEvent (
        EFI_EVENT_NOTIFY_WAIT,
        EFI_TPL_NOTIFY,
        UsbMouseWaitForInput,
        UsbMouse,
        &((UsbMouse->SimplePointerProtocol).WaitForInput)
        );
    
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "Mouse event is created, status = %r\n", Status);
    
    ASSERT(Status == EFI_SUCCESS);

    //
    // Install protocol interfaces for the USB mouse device
    //
    Status = gBS->InstallProtocolInterface(
        &Controller,
        &gEfiSimplePointerProtocolGuid,
        EFI_NATIVE_INTERFACE,
        &UsbMouse->SimplePointerProtocol);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "Mouse protocol is installed, status = %r\n", Status);
    
    ASSERT(Status == EFI_SUCCESS);
    
}

/**
    Stops USB mouse device

**/

EFI_STATUS
UninstallUSBMouse (
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE Controller,
    UINTN NumberOfChildren,
    EFI_HANDLE *Children
)
{
    EFI_STATUS Status;
    EFI_SIMPLE_POINTER_PROTOCOL     *SimplePoint; 
    USB_MOUSE_DEV       *UsbMouse = 0; 
 

    Status = gBS->OpenProtocol(Controller,
                                &gEfiSimplePointerProtocolGuid,
                                (VOID **)&SimplePoint,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL); 

    UsbMouse = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL(SimplePoint,SimplePointerProtocol);
    
    Status = gBS->UninstallProtocolInterface(Controller, &gEfiSimplePointerProtocolGuid,
                        &UsbMouse->SimplePointerProtocol);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->CloseEvent(
            (UsbMouse->SimplePointerProtocol).WaitForInput);
    
    ASSERT(Status == EFI_SUCCESS);

    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    gBS->FreePool(UsbMouse);
    
    return Status;
} 


/************ SimplePointer Protocol implementation routines*************/

/**
    This routine is a part of SimplePointerProtocol implementation;
    it resets USB mouse.

    @param This - A pointer to the EFI_SIMPLE_POINTER_PROTOCOL instance.
        ExtendedVerification - Indicates that the driver may perform
        a more exhaustive verification operation of the device during
        reset.

    @retval EFI_SUCCESS or EFI_DEVICE_ERROR

**/

EFI_STATUS
EFIAPI
UsbMouseReset(
    IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
    IN BOOLEAN                        ExtendedVerification
  )
{
    EfiZeroMem (
        &MsState,
        sizeof(EFI_SIMPLE_POINTER_STATE)
        );
    StateChanged = FALSE;
    
    EfiZeroMem (&gUsbData->MouseData, sizeof(MOUSE_DATA));

    return EFI_SUCCESS;
}


/**
    This routine is a part of SimplePointerProtocol implementation;
    it retrieves the current state of a pointer device.

    @param This - A pointer to the EFI_SIMPLE_POINTER_PROTOCOL instance.
        MouseState - A pointer to the state information on the pointer
        device. Type EFI_SIMPLE_POINTER_STATE is defined as follows:
        typedef struct {
        INT32 RelativeMovementX;
        INT32 RelativeMovementY;
        INT32 RelativeMovementZ;
        BOOLEAN LeftButton;
        BOOLEAN RightButton;
        } EFI_SIMPLE_POINTER_STATE;

    @retval EFI_SUCCESS The state of the pointer device was returned
        in MouseState.
    @retval EFI_NOT_READY The state of the pointer device has not changed
        since the last call to GetMouseState().
    @retval EFI_DEVICE_ERROR A device error occurred while attempting to
        retrieve the pointer device’s current state.
**/

EFI_STATUS
EFIAPI
GetMouseState(
    EFI_SIMPLE_POINTER_PROTOCOL  *This,
    EFI_SIMPLE_POINTER_STATE     *MouseState
)
{
    EFI_TPL OldTpl;
    EFI_STATUS  Status;
    
    if (MouseState == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
    
    Status = UpdateUsbMouseData(This,MouseState);

    gBS->RestoreTPL(OldTpl);

    return Status;
}


/**
    This routine updates current mouse data.

    @param Data* - pointer to the data area to be updated.

    @retval EFI_SUCCESS

**/

EFI_STATUS
UpdateUsbMouseData (
    EFI_SIMPLE_POINTER_PROTOCOL  *This,
	EFI_SIMPLE_POINTER_STATE	*State
)
{
    BOOLEAN LeftButton, RightButton;
    INT32   rX, rY, rZ;
    UINT8   bData;
    EFI_STATUS  Status;
    UINT8   MouseData[4];
    UINTN   DataLength;
    UINT32  UsbStatus;
    INT32   Coordinates;
    USB_MOUSE_DEV       *UsbMouse = 0; 
 
    if ((gUsbData->dUSBStateFlag & USB_FLAG_EFIMS_DIRECT_ACCESS) && (This != NULL) ){
        UsbMouse = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL(This,SimplePointerProtocol); 
        // Get the data from mouse
        DataLength = 4;
    
        Status = UsbMouse->UsbIo->UsbSyncInterruptTransfer(
            UsbMouse->UsbIo,
            UsbMouse->Endpoint | 0x80,    // IN
            MouseData,
            &DataLength,
            0,  // Timeout
            &UsbStatus
        );

	    if (EFI_ERROR(Status)) {
		    return EFI_DEVICE_ERROR;
	    }
    
        gUsbData->MouseData.ButtonStatus = MouseData[0];
    
        Coordinates = (INT8)MouseData[1];
        gUsbData->MouseData.MouseX += Coordinates;
        Coordinates = (INT8)MouseData[2];
        gUsbData->MouseData.MouseY += Coordinates;
     }

    bData = gUsbData->MouseData.ButtonStatus & 7;

    //
    // Check mouse Data
    //
	LeftButton=(BOOLEAN)(bData & 0x01)?TRUE:FALSE;
	RightButton=(BOOLEAN)(bData & 0x02)?TRUE:FALSE;

    rX = gUsbData->MouseData.MouseX;
    rY = gUsbData->MouseData.MouseY;
    rZ = - (gUsbData->MouseData.MouseZ);

	if (StateChanged == FALSE) {
		if (rX == 0 && rY == 0 && rZ == 0 && 
			bData == ButtonsState) {
			return EFI_NOT_READY;
		}
        StateChanged = TRUE;
	}

    gUsbData->MouseData.MouseX=0;
    gUsbData->MouseData.MouseY=0;
    gUsbData->MouseData.MouseZ=0;

    ButtonsState = bData;
    MsState.LeftButton = LeftButton;
    MsState.RightButton = RightButton;
    MsState.RelativeMovementX += rX;
    MsState.RelativeMovementY += rY;
    MsState.RelativeMovementZ += rZ; 


	if (State != NULL) {
		EfiCopyMem(State, &MsState, sizeof(EFI_SIMPLE_POINTER_STATE));
	    //
	    // Clear previous move state
	    //
        MsState.RelativeMovementX = 0;
        MsState.RelativeMovementY = 0;
        MsState.RelativeMovementZ = 0;  
        StateChanged = FALSE;
	}

    return EFI_SUCCESS;
}


/**
    Event notification function for SIMPLE_POINTER.WaitForInput
    event. Signal the event if there is input from mouse.

    @param Event - event to signal in case of mouse activity
        Context - data to pass along with the event.

    @retval VOID

**/

VOID
EFIAPI
UsbMouseWaitForInput (
    EFI_EVENT   Event,
    VOID        *Context
    )
{
	EFI_STATUS Status;
    USB_MOUSE_DEV       *UsbMouse = (USB_MOUSE_DEV*)Context; 

    Status = UpdateUsbMouseData (&UsbMouse->SimplePointerProtocol,NULL);
	if (EFI_ERROR(Status)) {
		return;
	}

    //
    // Someone is waiting on the mouse event, if there's
    // input from mouse, signal the event
    //
    gBS->SignalEvent(Event);

}



/**
    Initialize USB Mouse driver

**/

CHAR16*
UsbMsGetControllerName(
    EFI_HANDLE Controller,
    EFI_HANDLE Child
)
{
    return 0;
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
