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
    File contains the Simple Text Output functionality for the
    Console Splitter Driver

**/
//**********************************************************************
#include "ConSplit.h"
#include "Token.h"


extern EFI_HII_KEYBOARD_LAYOUT *gKeyDescriptorList;
extern EFI_KEY_TOGGLE_STATE mCSToggleState;
extern SIMPLE_TEXT_OUTPUT_MODE  MasterMode;


/// Consplitter's simple pointer mode declaration
EFI_SIMPLE_POINTER_MODE gSimplePointerMode = {
    0x10000,
    0x10000,
    0x10000,
    FALSE,
    FALSE
};

/// Consplitter's absolute pointer mode declaration 
EFI_ABSOLUTE_POINTER_MODE gAbsolutePointerMode = {
    0,       
    0,       
    0,       
    0, 
    0, 
    0, 
    0        
};

/// Variable for tracking if an absolute pointer event is pending
BOOLEAN mAbsoluteInputEventSignalState;

/// ConSplitter's Simple In protocol declaration
EFI_SIMPLE_TEXT_INPUT_PROTOCOL mCSSimpleInProtocol = {
	CSInReset,
	CSReadKeyStroke,
	NULL
	} ;

/// ConSplitter's Simple In Ex protocol declaration
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL mCSSimpleInExProtocol = 	{
	CSInResetEx,
	CSReadKeyStrokeEx,
	NULL,
    CSInSetState,
    CSInRegisterKeyNotify,
    CSInUnRegisterKeyNotify
	} ;

/// ConSplitter's AmiEfiKeycode protocol declaration
AMI_EFIKEYCODE_PROTOCOL mCSKeycodeInProtocol = 	{
	CSInResetEx,
	CSReadEfiKey,
	NULL,
    CSInSetState,
    CSInRegisterKeyNotify,
    CSInUnRegisterKeyNotify
	} ;

/// ConSplitter's Simple Pointer protocol declaration
EFI_SIMPLE_POINTER_PROTOCOL mCSSimplePointerProtocol = {
    ConSplitterSimplePointerReset,
    ConSplitterSimplePointerGetState,
    ConSplitterSimplePointerWaitForInput,
    &gSimplePointerMode
};

/// ConSplitter's Absolute Pointer protocol declaration
EFI_ABSOLUTE_POINTER_PROTOCOL mCSAbsolutePointerProtocol = {
    ConSplitterAbsolutePointerReset,
    ConSplitterAbsolutePointerGetState,
    NULL,
    &gAbsolutePointerMode
};

VOID StallForStop(volatile UINT8 *Lock, volatile BOOLEAN *BlockStop)
{
    if ((*Lock == 0) && (*BlockStop == TRUE)) {
        UINTN i;
        for (i = 0; i < 100; i++) {
            if ((*Lock !=0) || (*BlockStop == FALSE)) {
                break;
            }
            pBS->Stall(1 * 1000); // 1ms
        }
    }
}

/**
    Function goes through all the devices containing the Simple Pointer Protocol
    and calls their reset functions. If the console control's LockStdIn has been
    called, this function will be blocked from executing

    @param  This Pointer to the ConsoleSplitter's Simple Pointer Protocol
    @param  ExtendedVerification Value to pass to the reset functino's Extended Verification

    @retval EFI_SUCCESS Devices were reset sucessfully
    @retval EFI_ERROR Some of the devices returned an error
**/
EFI_STATUS EFIAPI
ConSplitterSimplePointerReset (
    IN  EFI_SIMPLE_POINTER_PROTOCOL *This,
    IN  BOOLEAN ExtendedVerification )
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_STATUS TestStatus;
    CON_SPLIT_SIMPLE_POINTER *ConSimplePointer;

    LockPointer++;
    
    if (CurrentStdInStatus)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_ACCESS_DENIED;
    }
    
    if (ConPointerList.pHead == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_SUCCESS;
    }

    ConSimplePointer = OUTTER(ConPointerList.pHead, Link, CON_SPLIT_SIMPLE_POINTER);

    // we need to loop through all the registered simple pointer devices
    // and call each of their Reset function
    while (ConSimplePointer != NULL) {
        TestStatus = ConSimplePointer->SimplePointer->Reset(ConSimplePointer->SimplePointer, ExtendedVerification);
        ConSimplePointer = OUTTER( ConSimplePointer->Link.pNext, Link, CON_SPLIT_SIMPLE_POINTER );

        if (EFI_ERROR(TestStatus))
            Status = TestStatus;
    }

    LockPointer--;
    StallForStop(&LockPointer, &BlockPointerStop);
    return Status;
}

/**
    Retrieves the current state of a pointer device.

    @param This Pointer to the ConsoleSplitter's Simple Pointer Protcool
    @param State Pointer to the buffer to return the collective state

    @retval EFI_SUCCESS The state information was returned
    @retval EFI_NOT_READY There were no devices being managed by the Console Splitter
    @retval EFI_DEVICE_ERROR A hardware error was encountered
**/
EFI_STATUS EFIAPI
ConSplitterSimplePointerGetState(
    IN  EFI_SIMPLE_POINTER_PROTOCOL *This,
    IN OUT EFI_SIMPLE_POINTER_STATE *State )
{
    EFI_STATUS Status;
    EFI_SIMPLE_POINTER_STATE  CurrentState;
    CON_SPLIT_SIMPLE_POINTER *ConSimplePointer;
    BOOLEAN EfiSuccessDetected = FALSE;
    BOOLEAN EfiDeviceErrorDetected = FALSE;

    LockPointer++;

    if (CurrentStdInStatus)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_ACCESS_DENIED;
    }

    State->RelativeMovementX  = 0;
    State->RelativeMovementY  = 0;
    State->RelativeMovementZ  = 0;
    State->LeftButton         = FALSE;
    State->RightButton        = FALSE;

    //if no device attached return success with no movement
    if (ConPointerList.pHead == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_NOT_READY;
    }

    ConSimplePointer = OUTTER( ConPointerList.pHead, Link, CON_SPLIT_SIMPLE_POINTER );

    // we need to loop through all the registered simple pointer devices
    while (ConSimplePointer != NULL) {

        Status = ConSimplePointer->SimplePointer->GetState(ConSimplePointer->SimplePointer, &CurrentState);

        if (!EFI_ERROR(Status)) {

            EfiSuccessDetected = TRUE;

            if (CurrentState.LeftButton)
                State->LeftButton = TRUE;

            if (CurrentState.RightButton)
                State->RightButton = TRUE;

            if ( CurrentState.RelativeMovementX != 0 && ConSimplePointer->SimplePointer->Mode->ResolutionX != 0 )
                State->RelativeMovementX +=
                    (CurrentState.RelativeMovementX * (INT32)gSimplePointerMode.ResolutionX) /
                    (INT32)ConSimplePointer->SimplePointer->Mode->ResolutionX;

            if ( CurrentState.RelativeMovementY != 0 && ConSimplePointer->SimplePointer->Mode->ResolutionY != 0 )
                State->RelativeMovementY +=
                    (CurrentState.RelativeMovementY * (INT32)gSimplePointerMode.ResolutionY) /
                    (INT32)ConSimplePointer->SimplePointer->Mode->ResolutionY;

            if ( CurrentState.RelativeMovementZ != 0 && ConSimplePointer->SimplePointer->Mode->ResolutionZ != 0 )
                State->RelativeMovementZ +=
                    (CurrentState.RelativeMovementZ * (INT32)gSimplePointerMode.ResolutionZ) /
                    (INT32)ConSimplePointer->SimplePointer->Mode->ResolutionZ;

        } else if (Status == EFI_DEVICE_ERROR) {
            EfiDeviceErrorDetected = TRUE;
        }

        ConSimplePointer = OUTTER( ConSimplePointer->Link.pNext, Link, CON_SPLIT_SIMPLE_POINTER );
    }

    LockPointer--;
    StallForStop(&LockPointer, &BlockPointerStop);
    return (EfiSuccessDetected) ? EFI_SUCCESS : (EfiDeviceErrorDetected) ? EFI_DEVICE_ERROR : EFI_NOT_READY;
}


/**
    Callback function executed on the WaitForEvent timer experiation. If LockStdIn is not set,
    function will notify registered callbacks waiting for input that the event has expired.

    @param Event The Event assoicated with callback.
    @param Context Context registered when Event was created.
**/
VOID EFIAPI
ConSplitterSimplePointerWaitForInput(
    IN  EFI_EVENT Event,
    IN  VOID *Context )
{
    EFI_STATUS TestStatus;
    CON_SPLIT_SIMPLE_POINTER *ConSimplePointer;

    LockPointer++;

    if (CurrentStdInStatus)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return;
    }

    if (ConPointerList.pHead == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return;
    }

    ConSimplePointer = OUTTER( ConPointerList.pHead, Link, CON_SPLIT_SIMPLE_POINTER );

    // loop through simple pointer events and check their events
    // if one event has been signaled, signal my event and exit.
    // we need to loop through all the registered simple pointer devices
    while (ConSimplePointer != NULL) {
        TestStatus = pBS->CheckEvent(ConSimplePointer->SimplePointer->WaitForInput);
        ConSimplePointer = OUTTER( ConSimplePointer->Link.pNext, Link, CON_SPLIT_SIMPLE_POINTER );

        if (!EFI_ERROR(TestStatus))
            pBS->SignalEvent(Event);
    }

    LockPointer--;
    StallForStop(&LockPointer, &BlockPointerStop);
    return;
}


/**
  Resets the pointer device hardware.

  @param  This                     Protocol instance pointer.
  @param  ExtendedVerification     Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS              The device was reset.
  @retval EFI_DEVICE_ERROR         The device is not functioning correctly and
                                   could not be reset.

**/
EFI_STATUS
EFIAPI
ConSplitterAbsolutePointerReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL   *This,
  IN BOOLEAN                         ExtendedVerification
  )
{
    EFI_STATUS                    Status;
    EFI_STATUS                    ReturnStatus;
    CON_SPLIT_ABSOLUTE_POINTER    *ConAbsolutePointer;
    
    LockPointer++;

    if (CurrentStdInStatus)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_ACCESS_DENIED;
    }
    
    if(This == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_INVALID_PARAMETER;
    }
    
    if (ConAbsolutePointerList.pHead == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_SUCCESS;
    }
        
    mAbsoluteInputEventSignalState = FALSE;
    
    for(ConAbsolutePointer = OUTTER( ConAbsolutePointerList.pHead, Link, CON_SPLIT_ABSOLUTE_POINTER );
        ConAbsolutePointer != NULL;
        ConAbsolutePointer = OUTTER( ConAbsolutePointer->Link.pNext, Link, CON_SPLIT_ABSOLUTE_POINTER ))
    {
        ReturnStatus = ConAbsolutePointer->AbsolutePointer->Reset(ConAbsolutePointer->AbsolutePointer, ExtendedVerification);
        if (EFI_ERROR(ReturnStatus))
            Status = ReturnStatus;
    }
    
    LockPointer--;
    StallForStop(&LockPointer, &BlockPointerStop);
    return Status;
}

/**
  Retrieves the current state of a pointer device.

  @param  This                     Protocol instance pointer.
  @param  State                    A pointer to the state information on the
                                   pointer device.

  @retval EFI_SUCCESS              The state of the pointer device was returned in
                                   State..
  @retval EFI_NOT_READY            The state of the pointer device has not changed
                                   since the last call to GetState().
  @retval EFI_DEVICE_ERROR         A device error occurred while attempting to
                                   retrieve the pointer device's current state.

**/
EFI_STATUS
EFIAPI
ConSplitterAbsolutePointerGetState (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL   *This,
  IN OUT EFI_ABSOLUTE_POINTER_STATE  *State
  )
{
    EFI_STATUS Status;
    EFI_ABSOLUTE_POINTER_STATE  CurrentState;
    CON_SPLIT_ABSOLUTE_POINTER *ConAbsolutePointer;
    BOOLEAN EfiSuccessDetected = FALSE;
    BOOLEAN EfiDeviceErrorDetected = FALSE;
    UINT64 Temp = 0;

    LockPointer++;
    
    if (CurrentStdInStatus)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_ACCESS_DENIED;
    }

    if(This == NULL || State == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_INVALID_PARAMETER;
    }

    State->CurrentX     = 0;
    State->CurrentY     = 0;
    State->CurrentZ     = 0;
    State->ActiveButtons = 0;

    //if no device attached return success with no movement
    if (ConAbsolutePointerList.pHead == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return EFI_NOT_READY;
    }

    mAbsoluteInputEventSignalState = FALSE;

    for(ConAbsolutePointer = OUTTER( ConAbsolutePointerList.pHead, Link, CON_SPLIT_ABSOLUTE_POINTER );
        ConAbsolutePointer != NULL;
        ConAbsolutePointer = OUTTER( ConAbsolutePointer->Link.pNext, Link, CON_SPLIT_ABSOLUTE_POINTER ))
    {
        Status = ConAbsolutePointer->AbsolutePointer->GetState(ConAbsolutePointer->AbsolutePointer, &CurrentState);
        if (!EFI_ERROR(Status)) {
            EfiSuccessDetected = TRUE;

            if( ((CurrentState.ActiveButtons)&EFI_ABSP_TouchActive) == EFI_ABSP_TouchActive)
                State->ActiveButtons |= EFI_ABSP_TouchActive;

            if( ((CurrentState.ActiveButtons)&EFI_ABS_AltActive) == EFI_ABS_AltActive)
                State->ActiveButtons |= EFI_ABS_AltActive;

            if (!(ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinX == 0 && ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMaxX == 0)) {
                Temp = (CurrentState.CurrentX - ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinX);
                Temp = MultU64x64(Temp, gAbsolutePointerMode.AbsoluteMaxX);
                State->CurrentX = DivU64x32(Temp, (UINT32)(ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMaxX - ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinX));
            }

            if (!(ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinY == 0 && ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMaxY == 0)) {
                Temp = (CurrentState.CurrentY - ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinY);
                Temp = MultU64x64(Temp, gAbsolutePointerMode.AbsoluteMaxY);
                State->CurrentY = DivU64x32(Temp, (UINT32)(ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMaxY - ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinY));
            }

            if (!(ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinZ == 0 && ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMaxZ == 0)) {
                Temp = (CurrentState.CurrentZ - ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinZ);
                Temp = MultU64x64(Temp, gAbsolutePointerMode.AbsoluteMaxZ);
                State->CurrentZ = DivU64x32(Temp, (UINT32)(ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMaxZ - ConAbsolutePointer->AbsolutePointer->Mode->AbsoluteMinZ));
            }

        } else if (Status == EFI_DEVICE_ERROR) {
            EfiDeviceErrorDetected = TRUE;
        }
    }

    LockPointer--;
    StallForStop(&LockPointer, &BlockPointerStop);
    return (EfiSuccessDetected) ? EFI_SUCCESS : (EfiDeviceErrorDetected) ? EFI_DEVICE_ERROR : EFI_NOT_READY;
}

/**
  This event agregates all the events of the pointer devices in the splitter.
  If any events of physical pointer devices are signaled, signal the pointer
  splitter event. This will cause the calling code to call
  ConSplitterAbsolutePointerGetState ().

  @param  Event                    The Event assoicated with callback.
  @param  Context                  Context registered when Event was created.

**/
VOID
EFIAPI
ConSplitterAbsolutePointerWaitForInput (
  IN  EFI_EVENT                       Event,
  IN  VOID                            *Context
  )
{
    EFI_STATUS Status;
    CON_SPLIT_ABSOLUTE_POINTER *ConAbsolutePointer;

    LockPointer++;
    
    if (CurrentStdInStatus)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return;
    }

    //if no device attached return success with no movement
    if (ConAbsolutePointerList.pHead == NULL)
    {
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return;
    }
        
    //
    // if AbsoluteInputEventSignalState is flagged before,
    // and not cleared by Reset() or GetState(), signal it
    if(mAbsoluteInputEventSignalState) {
        pBS->SignalEvent(Event);
        LockPointer--;
        StallForStop(&LockPointer, &BlockPointerStop);
        return;
    }
    //
    // if any physical console input device has key input, signal the event.
    //
    for(ConAbsolutePointer = OUTTER( ConAbsolutePointerList.pHead, Link, CON_SPLIT_ABSOLUTE_POINTER );
        ConAbsolutePointer != NULL;
        ConAbsolutePointer = OUTTER( ConAbsolutePointer->Link.pNext, Link, CON_SPLIT_ABSOLUTE_POINTER ))
    {
        Status = pBS->CheckEvent (ConAbsolutePointer->AbsolutePointer->WaitForInput);
        if (!EFI_ERROR (Status)) {
            pBS->SignalEvent(Event);
            mAbsoluteInputEventSignalState = TRUE;
        }
    }

    LockPointer--;
    StallForStop(&LockPointer, &BlockPointerStop);
}

/**
 * Function that updates the ConSplitter's AbsolutePointer mode data with the absolute
 *  minimum and maximum addresses based upon the ConSplitter' current Text Screen mode
 */
VOID UpdateAbsolutePointerInformation(VOID)
{
    mCSAbsolutePointerProtocol.Mode->AbsoluteMinX = 0;
    mCSAbsolutePointerProtocol.Mode->AbsoluteMinY = 0;
    mCSAbsolutePointerProtocol.Mode->AbsoluteMinZ = 0;
    
    mCSAbsolutePointerProtocol.Mode->AbsoluteMaxX = MultU64x64(SupportedModes[MasterMode.Mode].Columns, EFI_GLYPH_WIDTH);
    mCSAbsolutePointerProtocol.Mode->AbsoluteMaxY = MultU64x64(SupportedModes[MasterMode.Mode].Rows, EFI_GLYPH_HEIGHT);
    mCSAbsolutePointerProtocol.Mode->AbsoluteMaxZ = 0;

}

/**
    Reset function for the Simple Text In virtual Device. This function goes
    through the list of Simple Text In devices being managed and calls the
    Reset function for each device.

    @param This Pointer to the Console Splitter's SimpleTextIn protocol
    @param EV Flag to determine if Extended Verification should be performed when resetting each device.

    @retval EFI_SUCCESS Managed devices were reset
    @retval EFI_ERROR One or more of the managed devices returned an error
    @retval EFI_ACCESS_DENIED The Console is locked via the console control protocol
**/
EFI_STATUS EFIAPI
CSInReset(
	IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
	IN BOOLEAN                        EV
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_STATUS ManagedDeviceStatus;
	CON_SPLIT_IN *SimpleIn;

    // control
    LockConIn++;

    if(CurrentStdInStatus)
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
	    return EFI_ACCESS_DENIED;
    }
    SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN);

	// we need to loop through all the registered simple text out devices
	//	and call each of their Reset function
	while( SimpleIn != NULL)
	{
		ManagedDeviceStatus = SimpleIn->SimpleIn->Reset(SimpleIn->SimpleIn, EV);

		if(EFI_ERROR(ManagedDeviceStatus))
			Status = ManagedDeviceStatus;

        SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN);
	}

    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
	return Status;
}

/**
    Function to read the next keystroke from all the Simple Text Input devices
    that are managed by the Console Splitter.

    @param This Pointer to the Console Splitter's Simple Text In protocol
    @param Key Pointer to the buffer to return keypress information

    @retval EFI_SUCCESS Keystroke data successfully retrieved
    @retval EFI_NOT_READY There was no keystroke data available, or the console is locked
    @retval EFI_DEVICE_ERROR The keystroke information was not returned due to hardware error
**/
EFI_STATUS EFIAPI
CSReadKeyStroke(
	IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
	OUT	EFI_INPUT_KEY                  *Key
)
{
	EFI_STATUS	Status = EFI_NOT_READY;
    AMI_EFI_KEY_DATA EfiKeyData;

    LockConIn++;

    if(CurrentStdInStatus)
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return Status;
    }
    Status = CSReadEfiKey( (AMI_EFIKEYCODE_PROTOCOL*) This, &EfiKeyData );
    if(!EFI_ERROR(Status)) {
        //
        // Check for the Partial Key. If found, SimpleTextIn ReadKeyStroke
        // Should not return that Key has bee found.
        //
        if(EfiKeyData.Key.ScanCode == 00 && EfiKeyData.Key.UnicodeChar == 0 &&
             (EfiKeyData.KeyState.KeyToggleState & KEY_STATE_EXPOSED )) {
            LockConIn--;
            return EFI_NOT_READY;
        }
        *Key = EfiKeyData.Key;
    }

    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
	return Status;
}

/**
    Timer callback functin called perioditically to check if a new keystroke has occured. When
    it determines that a keystroke has occured and that there is key data available, the
    function signals the passed event.  If the LockStdIn is set, new keystroke information will
    not be checked.

    @param Event Event to signal on new keystroke data being available
    @param Context Pointer to specific context
**/
VOID EFIAPI
CSWaitForKey(IN EFI_EVENT Event, IN VOID *Context)
{
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	EFI_STATUS	 TestStatus = EFI_NOT_READY;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
	CON_SPLIT_IN *SimpleIn;

    LockConIn++;
	
	if(CurrentStdInStatus)
	{    
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
	    return;
	}
	
	if(ConInList.pHead == NULL)
	{
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
	    return;
	}
	
    // loop through simple in events and check their events
    //	if one event has been signaled, signal my event and exit
    SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN);

	// we need to loop through all the registered simple text out devices
	//	and call each of their Reset function
	while ( SimpleIn != NULL)
	{
	    // Based on Input Context check WaitForKey Event for SimpleTextIn, SimpleTextInEx or KeyCodeInEx
	    if((Context == &mCSSimpleInProtocol) && (SimpleIn->SimpleIn != NULL)) {
		     TestStatus = pBS->CheckEvent(SimpleIn->SimpleIn->WaitForKey);
	    } else if((Context == &mCSSimpleInExProtocol) && (SimpleIn->SimpleInEx != NULL)) {
	        TestStatus = pBS->CheckEvent(SimpleIn->SimpleInEx->WaitForKeyEx);
	    } else if((Context == &mCSKeycodeInProtocol) && (SimpleIn->KeycodeInEx != NULL)) {
	        TestStatus = pBS->CheckEvent(SimpleIn->KeycodeInEx->WaitForKeyEx);
	    }
	    
		SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN);

		if (!EFI_ERROR(TestStatus))
     		pBS->SignalEvent (Event);
	}

    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
	return;
}

/**
    Function to reset the input device hardware.

    This function resets the input device hardware. This routine is a part
    of SimpleTextInEx protocol implementation

    @param This pointer to protocol instance
    @param ExtendedVerification flag if Extended verification has to be performed

    @retval EFI_SUCCESS device was reset successfully
    @retval EFI_ERROR some of devices returned error

**/
EFI_STATUS EFIAPI
CSInResetEx(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN BOOLEAN                           ExtendedVerification )
{
    EFI_STATUS Status;

    LockConIn++;

    if(CurrentStdInStatus)
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_SUCCESS;
    }
    Status = CSInReset(0, ExtendedVerification);
    
    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
    return Status;
}

/**
    This function reads the next keystroke from the input device. This
    routine is a part of SimpleTextInEx protocol implementation

    @param This Pointer to the simple text in ex protocol
    @param KeyData Buffer to return the keypress data in.

    @retval EFI_SUCCESS Keystroke data successfully retrieved
    @retval EFI_NOT_READY There was no keystroke data available
    @retval EFI_DEVICE_ERROR The keystroke information was not returned due to hardware error
**/
EFI_STATUS EFIAPI
CSReadKeyStrokeEx (
    IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    OUT EFI_KEY_DATA                      *KeyData
)
{
	EFI_STATUS	Status = EFI_INVALID_PARAMETER;
    AMI_EFI_KEY_DATA EfiKeyData;

    LockConIn++;

    if(CurrentStdInStatus)
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_NOT_READY;
    }
    
    if(KeyData != NULL)
    {
        Status = CSReadEfiKey( (AMI_EFIKEYCODE_PROTOCOL*) This, &EfiKeyData );
        if (Status == EFI_SUCCESS) {
            KeyData->Key = EfiKeyData.Key;
            KeyData->KeyState = EfiKeyData.KeyState;
            LockConIn--;
            return EFI_SUCCESS;
        }
    }
    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
	return Status;
}

/**
    This function reads the next keystroke from the input devices. This
    routine is a part of AmiKeyCode protocol implementation

    @param This pointer to protocol instance
    @param KeyData key pressed information

    @retval EFI_SUCCESS Keystroke data successfully retrieved
    @retval EFI_NOT_READY There was no keystroke data available
    @retval EFI_DEVICE_ERROR The keystroke information was not returned
        due to hardware error

**/
EFI_STATUS EFIAPI
CSReadEfiKey (
    IN  AMI_EFIKEYCODE_PROTOCOL *This,
    OUT AMI_EFI_KEY_DATA        *KeyData
)
{
	AMI_EFI_KEY_DATA	TempKey;
	EFI_STATUS	Status = EFI_NOT_READY;
	CON_SPLIT_IN *SimpleIn;

    LockConIn++;

	if(CurrentStdInStatus)
	{        
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
	    return EFI_NOT_READY;
	}
	if (ConInList.pHead == NULL) 
	{
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_NOT_READY;
	}

    SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN);

    pBS->SetMem(KeyData, sizeof(AMI_EFI_KEY_DATA), 0);

	// we need to loop through all the registered EfiKey, SimpleInEx and
    // SimpleIn devices and call each of their ReadKeyStroke function
	while (SimpleIn != NULL)
	{
        if (SimpleIn->KeycodeInEx) {
    		Status = SimpleIn->KeycodeInEx->ReadEfikey(SimpleIn->KeycodeInEx, &TempKey);
        } else if(SimpleIn->SimpleInEx != NULL) {
    		Status = SimpleIn->SimpleInEx->ReadKeyStrokeEx(
                        SimpleIn->SimpleInEx, (EFI_KEY_DATA*)&TempKey);
        } else if(SimpleIn->SimpleIn != NULL) {
            Status = SimpleIn->SimpleIn->ReadKeyStroke(
                        SimpleIn->SimpleIn, (EFI_INPUT_KEY*)&TempKey);
        }

        // Check for the Toggle State change
        if (!EFI_ERROR(Status) && (TempKey.KeyState.KeyToggleState & TOGGLE_STATE_VALID)) {
            if ((TempKey.KeyState.KeyToggleState & ~KEY_STATE_EXPOSED ) != mCSToggleState) {
                mCSToggleState = (TempKey.KeyState.KeyToggleState & ~KEY_STATE_EXPOSED);
                CSInSetState ( (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL*) This,
                                &mCSToggleState );
            }
        }

		if (!EFI_ERROR(Status)) {
			*KeyData = TempKey;
            break;
        }

		SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN);
	}


	
	
    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
	return Status;
}

/**
    Function to set keyboard states for all the keyboard devices.

    @param This Pointer to the ConsoleSplitter's Simple Text In Ex protocol
    @param KeyToggleState Pointer Key states to set into the keyboard devices

    @retval EFI_SUCCESS Keystroke data successfully retrieved
    @retval EFI_UNSUPPORTED Given state not supported
    @retval EFI_INVALID_PARAMETER KeyToggleState is NULL
    @retval EFI_DEVICE_ERROR input device not found
    @retval EFI_ACCESS_DENIED input device is busy
**/
EFI_STATUS EFIAPI
CSInSetState (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_TOGGLE_STATE              *KeyToggleState
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	CON_SPLIT_IN *SimpleIn;

    LockConIn++;
	if(CurrentStdInStatus)
	{        
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
	    return EFI_ACCESS_DENIED;
	}

	if (ConInList.pHead == NULL)
	{
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_UNSUPPORTED;
	}

    if(KeyToggleState == NULL ) {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_INVALID_PARAMETER;
    }

    if (!(*KeyToggleState & TOGGLE_STATE_VALID) ||
        ((*KeyToggleState & (~(TOGGLE_STATE_VALID | KEY_STATE_EXPOSED |
                            SCROLL_LOCK_ACTIVE | NUM_LOCK_ACTIVE | CAPS_LOCK_ACTIVE)))) ) {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_UNSUPPORTED;
    }

    mCSToggleState = *KeyToggleState;  // Update global toggle state

    SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN);

	// we need to loop through all the registered KeycodeInEx devices
	//	and call each of their SetState function
	while ( SimpleIn != NULL )
	{
        if (SimpleIn->SimpleInEx) {
    		SimpleIn->SimpleInEx->SetState(SimpleIn->SimpleInEx, KeyToggleState);
        }
		SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN);
	}

    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
	return Status;
}

/**
    Function to register notificaiton functions into each of the Simple Text In Ex
    devices being managed by the ConsoleSplitter.

    @param This Pointer to the Console Splitter's Simple Text In Ex protocol
    @param KeyData The key data which should trigger the notification function
    @param KeyNotificationFunction The function to call when the keydata is input
    @param NotifyHandle A handle unique to the registered event

    @retval EFI_SUCCESS Notification function registered successfully
    @retval EFI_INVALID_PARAMETER KeyData/KeyNotificationFunction/NotifyHandle is NULL
    @retval EFI_DEVICE_ERROR input device not found
    @retval EFI_ACCESS_DENIED input device is busy
**/
EFI_STATUS EFIAPI
CSInRegisterKeyNotify(
    IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN  EFI_KEY_DATA                      *KeyData,
    IN  EFI_KEY_NOTIFY_FUNCTION           KeyNotificationFunction,
    OUT EFI_HANDLE                        *NotifyHandle
)
{
    EFI_STATUS      Status = EFI_NOT_READY;
    CON_SPLIT_IN    *SimpleIn;
    KEY_NOTIFY_LINK *NotifyLink;
    UINT32          ChildIndex;

    LockConIn++;

    if (CurrentStdInStatus)
    {        
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_ACCESS_DENIED;
    }

    if(KeyData == NULL || KeyNotificationFunction == NULL || NotifyHandle == NULL )
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_INVALID_PARAMETER;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(KEY_NOTIFY_LINK), (VOID**)&NotifyLink);
    if(EFI_ERROR(Status))
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return Status;
    }

    NotifyLink->KeyData = *KeyData;
    NotifyLink->NotifyFunction = KeyNotificationFunction;

    if (ConInList.pHead != NULL)
    {

        SimpleIn = OUTTER(ConInList.pHead, Link, CON_SPLIT_IN);
        ChildIndex = 0;
    
        // we need to loop through all the registered SimpleInEx
        // and call each RegisterKeyNotify function
        while (SimpleIn != NULL)
        {
            NotifyLink->Children[ChildIndex].Child = SimpleIn;
            if(SimpleIn->SimpleInEx != NULL) {
                Status = SimpleIn->SimpleInEx->RegisterKeyNotify(
                            SimpleIn->SimpleInEx, KeyData, KeyNotificationFunction, NotifyHandle);
                NotifyLink->Children[ChildIndex].NotifyHandle = (EFI_ERROR(Status)) ? (EFI_HANDLE) UNUSED_NOTIFY_HANDLE : *NotifyHandle;
            } else {
                NotifyLink->Children[ChildIndex].NotifyHandle = (EFI_HANDLE) UNUSED_NOTIFY_HANDLE;
            }
            ChildIndex++;
            SimpleIn = OUTTER(SimpleIn->Link.pNext, Link, CON_SPLIT_IN);
        }
    }
    
    DListAdd(&KeyNotifyList, (DLINK *)NotifyLink);
    *NotifyHandle = (EFI_HANDLE) NotifyLink;

    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
    return EFI_SUCCESS;
}

/**
    Function to unregister a previously registered Notification function.

    @param This Pointer to the Console Splitter's Simple Text In Ex protocol
    @param NotificationHandle handle to unregister

    @retval EFI_SUCCESS notification function unregistered successfully
    @retval EFI_INVALID_PARAMETER NotificationHandle is NULL
    @retval EFI_DEVICE_ERROR input device not found
    @retval EFI_ACCESS_DENIED input device is busy
**/
EFI_STATUS EFIAPI
CSInUnRegisterKeyNotify(
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_HANDLE                        NotificationHandle
)
{
    EFI_STATUS      Status = EFI_NOT_READY;
    CON_SPLIT_IN    *SimpleIn;
    KEY_NOTIFY_LINK *NotifyLink;
    UINT32          ChildIndex;
    EFI_HANDLE      Handle;

    LockConIn++;
    
    if (CurrentStdInStatus)
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_ACCESS_DENIED;
    }

    if(NotificationHandle == NULL ) {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_INVALID_PARAMETER;
    }


    NotifyLink = (KEY_NOTIFY_LINK *)KeyNotifyList.pHead;
    while(NotifyLink != NULL) {
        if((EFI_HANDLE)NotifyLink == NotificationHandle)
            break;
        NotifyLink = (KEY_NOTIFY_LINK *)NotifyLink->Link.pNext;
    }

    if(NotifyLink == NULL)
    {
        LockConIn--;
        StallForStop(&LockConIn, &BlockConInStop);
        return EFI_INVALID_PARAMETER;
    }

    if (ConInList.pHead != NULL)
    {
	    for(ChildIndex = 0; ChildIndex < ConInList.Size; ChildIndex++) {
	        SimpleIn = NotifyLink->Children[ChildIndex].Child;
	        Handle = NotifyLink->Children[ChildIndex].NotifyHandle;
	        if(SimpleIn != NULL && SimpleIn->SimpleInEx != NULL && Handle != (EFI_HANDLE) UNUSED_NOTIFY_HANDLE) {
	            Status = SimpleIn->SimpleInEx->UnregisterKeyNotify(SimpleIn->SimpleInEx, Handle);
	            if(EFI_ERROR(Status))
	            {
	                LockConIn--;
	                StallForStop(&LockConIn, &BlockConInStop);
	                return Status;
                }
            }
        }
    }

    DListDelete(&KeyNotifyList, (DLINK *)NotifyLink);
    pBS->FreePool(NotifyLink);

    LockConIn--;
    StallForStop(&LockConIn, &BlockConInStop);
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
