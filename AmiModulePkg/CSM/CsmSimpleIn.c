//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CsmSimpleIn.c
    Implements the SimpleTextIn protocol for the Csm.

**/

#include <Efi.h>
#include <AmiDxeLib.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AmiKeycode.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/ConsoleControl.h>


typedef struct _CSMSIMPLEIN_DEV {
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL      SimpleTextInput;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL	SimpleTextInputEx;
    AMI_EFIKEYCODE_PROTOCOL             EfiKeycodeInput;
} CSMSIMPLEIN_DEV;

UINT8   CurrentKeyState=0;

typedef struct _KEY_WAITING_RECORD{
  DLINK                                         Link;
  EFI_KEY_DATA                                  Context;
  EFI_KEY_NOTIFY_FUNCTION                       Callback;
} KEY_WAITING_RECORD;

DLIST                               mCsmKeyboardData;

//Simple Input:

EFI_STATUS CsmSimpleInInputReset(
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    IN BOOLEAN                          ExtendedVerification
);

EFI_STATUS CsmSimpleInInputReadKey(
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    OUT EFI_INPUT_KEY                   *Key
);

//Simple Input Ex:

EFI_STATUS CsmSimpleInInputResetEx(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN BOOLEAN                              ExtendedVerification
);

EFI_STATUS CsmSimpleInInputReadKeyEx(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    OUT EFI_KEY_DATA                        *KeyData
);

EFI_STATUS CsmSimpleInInputSetState (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN EFI_KEY_TOGGLE_STATE                 *KeyToggleState
);

EFI_STATUS CsmSimpleInInputRegisterKeyNotify(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN EFI_KEY_DATA                         *KeyData,
    IN EFI_KEY_NOTIFY_FUNCTION              KeyNotificationFunction,
    OUT EFI_HANDLE                          *NotifyHandle
);

EFI_STATUS CsmSimpleInInputUnRegisterKeyNotify(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN EFI_HANDLE                           NotificationHandle
);

// AMI KeyCode:

EFI_STATUS
CsmSimpleInInputReadEfiKey(
    IN AMI_EFIKEYCODE_PROTOCOL              *This,
    OUT AMI_EFI_KEY_DATA                    *KeyData
);

VOID ConnectCsmSimpleIn(
    IN EFI_EVENT Event, 
    IN VOID *Context
);

EFI_HANDLE          gCsmSimpleInHandle=NULL;


/**
    CSM SimpleIn driver entry point

    @param Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT

    @retval The status of CSM board initalization

**/

EFI_STATUS
InitCsmSimpleIn (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

    EFI_STATUS          Status;
    CSMSIMPLEIN_DEV     *CsmSimpleInDev = NULL;
	static EFI_GUID     GuidConInStarted = CONSOLE_IN_DEVICES_STARTED_PROTOCOL_GUID;
	EFI_EVENT 	        Event;
	VOID                *pRegistration;


    Status = pBS->AllocatePool(
                    EfiBootServicesData,
                    sizeof(CSMSIMPLEIN_DEV),
                    &CsmSimpleInDev);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    pBS->SetMem(CsmSimpleInDev, sizeof(CSMSIMPLEIN_DEV), 0);

    CsmSimpleInDev->SimpleTextInput.Reset = CsmSimpleInInputReset;
    CsmSimpleInDev->SimpleTextInput.ReadKeyStroke = CsmSimpleInInputReadKey;
    CsmSimpleInDev->SimpleTextInput.WaitForKey = NULL;

    CsmSimpleInDev->SimpleTextInputEx.Reset = CsmSimpleInInputResetEx;
    CsmSimpleInDev->SimpleTextInputEx.ReadKeyStrokeEx = CsmSimpleInInputReadKeyEx;
    CsmSimpleInDev->SimpleTextInputEx.SetState = CsmSimpleInInputSetState;
    CsmSimpleInDev->SimpleTextInputEx.RegisterKeyNotify = CsmSimpleInInputRegisterKeyNotify;
    CsmSimpleInDev->SimpleTextInputEx.UnregisterKeyNotify = CsmSimpleInInputUnRegisterKeyNotify;
    CsmSimpleInDev->SimpleTextInputEx.WaitForKeyEx = NULL;

    CsmSimpleInDev->EfiKeycodeInput.Reset = CsmSimpleInInputResetEx;
    CsmSimpleInDev->EfiKeycodeInput.ReadEfikey = CsmSimpleInInputReadEfiKey;
    CsmSimpleInDev->EfiKeycodeInput.SetState = CsmSimpleInInputSetState;
    CsmSimpleInDev->EfiKeycodeInput.RegisterKeyNotify = CsmSimpleInInputRegisterKeyNotify;
    CsmSimpleInDev->EfiKeycodeInput.UnregisterKeyNotify = CsmSimpleInInputUnRegisterKeyNotify;
    CsmSimpleInDev->EfiKeycodeInput.WaitForKeyEx = NULL;

    Status = pBS->InstallMultipleProtocolInterfaces(
            &gCsmSimpleInHandle,
            &gEfiSimpleTextInProtocolGuid, &CsmSimpleInDev->SimpleTextInput,
            &gEfiSimpleTextInputExProtocolGuid, &CsmSimpleInDev->SimpleTextInputEx,
            &gAmiEfiKeycodeProtocolGuid, &CsmSimpleInDev->EfiKeycodeInput,
            NULL
            );

	RegisterProtocolCallback(
		    &GuidConInStarted, ConnectCsmSimpleIn,
		    NULL, &Event,&pRegistration
	        );


    return EFI_SUCCESS;
}

/**
    This function Connect the CsmSimpleIn handle 

           
    @param Event signalled event
    @param Context pointer to event context

**/

VOID ConnectCsmSimpleIn(
    IN EFI_EVENT Event, 
    IN VOID *Context
)
{
    // Connect the CsmSimpleIn, So that Consplitter gets notified 
    pBS->ConnectController(gCsmSimpleInHandle, NULL, NULL, TRUE);
    // Kill the Event
    pBS->CloseEvent(Event); 

    return;        
}

//**********************************************************************
/**
    In our implementation, this function does nothing.

**/
//**********************************************************************
EFI_STATUS CsmSimpleInInputReset(
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    IN BOOLEAN                          ExtendedVerification
)
{
    return EFI_SUCCESS;
}


//**********************************************************************
/**
    In our implementation, this function does nothing.

**/
//**********************************************************************
EFI_STATUS CsmSimpleInInputResetEx(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
    IN BOOLEAN                              ExtendedVerification
)
{
    return EFI_SUCCESS;
}


//**********************************************************************
/**
    Returns a EFI_INPUT_KEY Key if possible.

**/
//**********************************************************************
EFI_STATUS CsmSimpleInInputReadKey(
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *This,
    OUT EFI_INPUT_KEY                   *Key
)
{
    return EFI_NOT_READY;
}

/**
    Get the Key Toggle State from the BDA

    @param  KeyToggleState - Pointer to the EFI_KEY_TOGGLE_STATE to
            set the state for the input device.

**/
EFI_STATUS CsmSimpleInToggleState(
    OUT EFI_KEY_TOGGLE_STATE *KeyToggleState
)
{
    UINT8   KeyState;

    KeyState = *(UINT8*)(UINTN)0x417;
    if(KeyState != CurrentKeyState) {
        if(KeyState & 0x10) {
            *KeyToggleState |= SCROLL_LOCK_ACTIVE;
        }
        if(KeyState & 0x20) {
            *KeyToggleState |= NUM_LOCK_ACTIVE;
        }
        if(KeyState & 0x40) {
            *KeyToggleState |= CAPS_LOCK_ACTIVE;
        }
        *KeyToggleState |= TOGGLE_STATE_VALID;
        CurrentKeyState=*(UINT8*)(UINTN)0x417;
        return EFI_SUCCESS;
    }
    return EFI_NOT_READY;
}

/**
    Reads the next keystroke from the input device and
    returns data in the form of EFI_KEY_DATA structure.
    This routine is a part of SimpleTextInEx protocol
    implementation.

**/

EFI_STATUS
CsmSimpleInInputReadKeyEx(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    OUT EFI_KEY_DATA *KeyData
)
{
    EFI_STATUS  Status;

    if(KeyData == NULL)
        return EFI_INVALID_PARAMETER;

    pBS->SetMem (KeyData, sizeof(EFI_KEY_DATA) , 0);
    Status=CsmSimpleInToggleState(&(KeyData->KeyState.KeyToggleState));
    return Status;
}


/**
    Set certain state for the input device.

    @param This - A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
                          instance.
    @param KeyToggleState - Pointer to the EFI_KEY_TOGGLE_STATE to
                          set the state for the input device.
**/

EFI_STATUS
CsmSimpleInInputSetState (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_TOGGLE_STATE *KeyToggleState
)
{
    if(KeyToggleState == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    if (!(*KeyToggleState & TOGGLE_STATE_VALID) ||
        ((*KeyToggleState & (~(TOGGLE_STATE_VALID | KEY_STATE_EXPOSED | SCROLL_LOCK_ACTIVE | NUM_LOCK_ACTIVE | CAPS_LOCK_ACTIVE)))) ) {
        return EFI_UNSUPPORTED;
    }

    CurrentKeyState = *(UINT8*)(UINTN)0x417;

    // Clear NumLock, CapsLock, Scroll Lock status
    CurrentKeyState &= 0x8F;

    if (*KeyToggleState & SCROLL_LOCK_ACTIVE) {
        CurrentKeyState |= 0x10;
    }

    if (*KeyToggleState & NUM_LOCK_ACTIVE) {
        CurrentKeyState |= 0x20;
    }

    if (*KeyToggleState & CAPS_LOCK_ACTIVE) {
        CurrentKeyState |= 0x40;
    }

    *(UINT8*)(UINTN)0x417 = CurrentKeyState;

    return EFI_SUCCESS;
}

/**
    Register a notification function for a particular
    keystroke for the input device.

    @param This - A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
        instance.
    @param KeyData - Key value
    @param KeyNotificationFunction- Pointer to the Notification Function
    @param NotificationHandle - Handle to be unregisterd

**/

EFI_STATUS
CsmSimpleInInputRegisterKeyNotify(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_DATA *KeyData,
    IN EFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction,
    OUT EFI_HANDLE *NotifyHandle
)
{

    EFI_STATUS          Status;
    KEY_WAITING_RECORD  *CsmKeyIn;

    if(KeyData == NULL || KeyNotificationFunction == NULL || NotifyHandle == NULL ) {
        return EFI_INVALID_PARAMETER;
    }


    //
    // Create database record and add to database
    //
    Status = pBS->AllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (KEY_WAITING_RECORD),
                      &CsmKeyIn
                      );

    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Gather information about the registration request
    //

    CsmKeyIn->Context   = *KeyData;
    CsmKeyIn->Callback  = KeyNotificationFunction;

    DListAdd (&mCsmKeyboardData, &(CsmKeyIn->Link));

    //
    // Child's handle will be the address linked list link in the record
    //
    *NotifyHandle = (EFI_HANDLE) (&CsmKeyIn->Link);

    return EFI_SUCCESS;
}


/**
    Removes the notification which was previously
    registered by CsmSimpleInInputRegisterKeyNotify.

    @param This - A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
        instance.
    @param NotificationHandle - Handle to be unregisterd

**/

EFI_STATUS
CsmSimpleInInputUnRegisterKeyNotify(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_HANDLE NotificationHandle
)
{
    DLINK               *ListPtr;
    KEY_WAITING_RECORD  *CsmKeyIn;

    if(NotificationHandle == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    ListPtr = mCsmKeyboardData.pHead;
    while ( ListPtr != NULL)
    {
        CsmKeyIn = OUTTER(ListPtr, Link, KEY_WAITING_RECORD);
        if ( (&CsmKeyIn->Link) == NotificationHandle)
        {
            DListDelete(&mCsmKeyboardData, ListPtr);
            pBS->FreePool(CsmKeyIn);
            return EFI_SUCCESS;
        }

        ListPtr = ListPtr->pNext;
    }

    return EFI_INVALID_PARAMETER;
}


/**
    Reads the next keystroke from the input device and
    returns data in the form of AMI_EFI_KEY_DATA structure.
    This routine is a part of AmiEfiKeycode protocol
    implementation.

**/

EFI_STATUS
CsmSimpleInInputReadEfiKey(
    IN AMI_EFIKEYCODE_PROTOCOL *This,
    OUT AMI_EFI_KEY_DATA *KeyData
)
{
    EFI_STATUS  Status;
    pBS->SetMem (KeyData, sizeof(AMI_EFI_KEY_DATA) , 0);
    Status=CsmSimpleInToggleState(&(KeyData->KeyState.KeyToggleState));
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
