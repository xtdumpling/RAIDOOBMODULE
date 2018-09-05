//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
  AmiBeepLib instance that generates beeps using legacy PC/AT speaker.
**/

#include <Library/IoLib.h>
#include <Library/TimerLib.h>

/**
    Generates sound of beep of the specified frequency and duration using legacy PC/AT speaker.
        
    @param Frequency sound frequency in hundredth of Hertz units.
    @param Duration sound duration in microseconds.
**/
VOID AmiBeep(
    IN UINT32           Frequency,
    IN UINT32           Duration
    )
{
    /** THIS IS HW DEPENDENT. PORTING MAY BE REQUIRED. **/
    UINT16    Divider;
    
    Divider = (119318200 + Frequency/2)/Frequency;
    //
    // Set up channel 1 timer (used for delays)
    //
    IoWrite8(0x43,0x54);
    IoWrite8(0x41,0x12);
    //
    // Set up channel 2 timer (used by speaker)
    //
    IoWrite8(0x43,0xb6);
    IoWrite8(0x42,(UINT8)Divider);
    IoWrite8(0x42,(UINT8)(Divider>>8));
    //
    // Turn the speaker on
    //
    IoWrite8(0x61,IoRead8(0x61)|3);
    //
    // Delay
    //
    MicroSecondDelay(Duration);
    //
    // Turn off the speaker
    //
    IoWrite8(0x61,IoRead8(0x61)&0xfc);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
