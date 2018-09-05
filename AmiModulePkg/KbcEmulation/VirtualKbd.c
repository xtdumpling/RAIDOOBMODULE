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

/** @file VirtualKbd.c
    Virtual keyboard functions
**/

//---------------------------------------------------------------------------

#include "KbcEmul.h"
#include "KbcDevEmul.h"
#include "KbcEmulLib.h"

//---------------------------------------------------------------------------

#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))

VOID vkbd_initParams(VIRTKBD* kbd);
VOID vkbd_read_codes(PS2SINK* ps2dev, UINT8 cmd );
VOID vkbd_read_option(PS2SINK* ps2dev, UINT8 cmd );
VOID vkbd_rw_scancode(PS2SINK* ps2dev, UINT8 cmd );
VOID vkbd_send_resp(VIRTKBD* mouse, UINT8* data, int count );


static UINT8 ackResp[] = {0xFA};
static UINT8 ResetResp[] = {0xFA,0xAA};
static UINT8 IdResp[] = {0xFA,0xAB, 0x41};
static UINT8 ScanCodePageResp[] = {0xFA, 0x00};
EFI_EMUL6064KBDINPUT_PROTOCOL   *gKbdProtocol=NULL;
static UINT8 CmdED = 0;

/**
    Handle commands sent from KBC to PS2 Keyboard. Keyboard is scanning or idle, waiting for command or key

    @param ps2dev
    @param Cmd

    @retval VOID

**/

VOID
kbd_scanning (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    VIRTKBD* kbd = _CR(ps2dev,VIRTKBD,sink);

    switch(cmd){
        //
        //Reset
        //
        case 0xFF:
            vkbd_initParams(kbd);
            vkbd_send_resp(kbd, ResetResp,COUNTOF(ResetResp));
            break;
        case 0xFD:
        case 0xFC:
        case 0xFB:
            vkbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            kbd->read_code_action = cmd;
            kbd->sink.onCommand = vkbd_read_codes;
            break;
        case 0xFA:
        case 0xF9:
        case 0xF8:
        case 0xF7:
        case 0xF6:
        case 0xF5:  
        case 0xF4:  //Enable
            kbd->Enabled=TRUE;
            vkbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            break;
        //
        //Set Typeatic Rate/Delay
        //
        case 0xF3:
            vkbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            kbd->sink.onCommand = vkbd_read_option;
            kbd->option_ptr = &kbd->typematicRateDelay;
            break;
        case 0xF2:
            vkbd_send_resp(kbd, IdResp,COUNTOF(IdResp));
            break;
        case 0xF0:
            vkbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            kbd->sink.onCommand = vkbd_rw_scancode;
            kbd->option_ptr = &kbd->scancodepage;
            break;
        //
        //Ehco
        //
        case 0xEE:
            vkbd_send_resp(kbd, &cmd, 1);
            break;

        //
        //Led
        //
        case 0xED:
            vkbd_send_resp(kbd, ackResp, COUNTOF(ackResp));
            kbd->sink.onCommand = vkbd_read_option;
            kbd->option_ptr = &kbd->typematicRateDelay;
            CmdED = 1;
            break;
    }
}

/**
    Keyboard Read Codes.

    @param ps2dev
    @param Cmd

    @retval VOID

**/

VOID
vkbd_read_codes (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    VIRTKBD* kbd = _CR(ps2dev,VIRTKBD,sink);
    if( cmd >= 0xED ){
        kbd->sink.onCommand = kbd_scanning;
        kbd_scanning(ps2dev,cmd);
    } else
        vkbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
}

/**
    Keyboard Read option.

    @param ps2dev
    @param Cmd

    @retval VOID

**/

VOID
vkbd_read_option (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    VIRTKBD* kbd = _CR(ps2dev,VIRTKBD,sink);
    UINT8*   fPtr;

    kbd->sink.onCommand = kbd_scanning;

    if (CmdED == 1) {
        fPtr = (UINT8*)(UINTN)0x417;
        *fPtr &= ~0x70;
        *fPtr |= ((cmd & 0x07) << 4);
        CmdED = 0;
        vkbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
    }
    else {
        if (cmd > 0xED) {
            kbd_scanning(ps2dev,cmd);
        } else  {
            *(kbd->option_ptr) = cmd;
            vkbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
        }
    }
}

/**
    Keyboard Read write Scan code.

    @param ps2dev
    @param Cmd

    @retval VOID

**/

VOID
vkbd_rw_scancode (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    VIRTKBD* kbd = _CR(ps2dev,VIRTKBD,sink);
    if(cmd != 0 )
        vkbd_read_option(ps2dev,cmd);
    else {
        kbd->sink.onCommand = kbd_scanning;
        ScanCodePageResp[1] = kbd->scancodepage;
        vkbd_send_resp(kbd, ScanCodePageResp,COUNTOF(ScanCodePageResp));
    }
}
/**
    Place the Responce bytes in the keyboard buffer.

    @param ps2dev
    @param Data
    @param count

    @retval VOID

**/
VOID
vkbd_send_resp (
    VIRTKBD  *kbd,
    UINT8    *data,
    int      count
)
{
    if( kbd->sink.present_ ){
        //
        // Emulation must provide the responce. Override any pervious kbd data queued to KBC
        //
        kbd->qtail = kbd->qhead = kbd->queue;
        for( ; count > 0; --count ){
            *kbd->qtail++ = *data++;
        }

        //
        // push the first byte to the KBC
        //
        if( kbd->qhead != kbd->qtail &&
            kbd->sink.kbc->send_outb1(kbd->sink.kbc,
            PS2DEV_KBD,TRUE,*kbd->qhead))
        {
            if(++kbd->qhead >=  kbd->queue+KBDQUEUE_SIZE)
                kbd->qhead = kbd->queue;
        }
    }
}

/**
    KBC has empty queue; Legacy I/O trapping is being processed.If Kbd has data to push into KBC,
    now is a good time to do it

    @param ps2dev

    @retval VOID

**/

VOID
vkbd_pumpQueue (
    PS2SINK  *ps2dev
)
{
    VIRTKBD* kbd = _CR(ps2dev,VIRTKBD,sink);
    //
    //send the Keyboard data to the KBC
    //
    if( kbd->qhead != kbd->qtail &&
        ps2dev->kbc->send_outb1(ps2dev->kbc,PS2DEV_KBD,FALSE,*kbd->qhead))
    {
        if(++kbd->qhead >=  kbd->queue+KBDQUEUE_SIZE)
            kbd->qhead = kbd->queue;
    }

}

/**
    Keyboard Input source sends data. Put the data into the buffer, attempt to send the first byte.
    The call comes outside of Trapping Leg access. Check that Trapping status will not be overriden

    @param P       - pointer to the Emulation Protocol
    @param Buffer  - Data buffer
    @param Count   - Buffer Length

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
kbdInput_Send (
    EFI_EMUL6064KBDINPUT_PROTOCOL *p,
    UINT8                         *buffer,
    UINT32                        count
)
{
    VIRTKBD* kbd = _CR(p,VIRTKBD,kbdInput_);
    UINTN c = 0;

    //
    // Check whether BIOS is processing IRQ1 or IRQ12
    //
    ByteWriteIO(0x20, 0xB);
    if (ByteReadIO(0x20) & 2) {
        return EFI_NOT_READY;
    }

    ByteWriteIO(0xA0, 0xB);
    if (ByteReadIO(0xA0) & 0x10) {
        return EFI_NOT_READY;
    }

    //
    //Keyboard is disabled by KBC command
    //
    if(!kbd->Enabled) {
        return EFI_DEVICE_ERROR;
    }

    //
    // count of free items in the queue
    //
    if( kbd->qtail >= kbd->qhead )
        c = KBDQUEUE_SIZE - (kbd->qtail - kbd->qhead);
    else
        c = kbd->qhead - kbd->qtail;

    if(c < count )
        return EFI_NOT_AVAILABLE_YET;

    for( ; count > 0; --count ){
        *kbd->qtail++ = *buffer++;
        if(kbd->qtail >= kbd->queue+KBDQUEUE_SIZE)
            kbd->qtail = kbd->queue;
    }
    //
    //Place the first data into the KBC buffer
    //
    vkbd_pumpQueue(&kbd->sink);

    return EFI_SUCCESS;
}

/**
    KBC translation

    @param P   - pointer to the Emulation Protocol
    @param outTrans

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
kbdInput_GetTranslation(
    EFI_EMUL6064KBDINPUT_PROTOCOL *p,
    OUT KBC_KBDTRANSLATION        *outTrans)
{
    *outTrans=KBC_KBDTRANS_PCXT;
    return EFI_SUCCESS;
}

/**
    Update the keyboard LED's

    @param P    - pointer to the Emulation Protocol
    @param Data - LED data

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
kbdInput_updateLED (
    EFI_EMUL6064KBDINPUT_PROTOCOL *p,
    IN UINT8                      data)
{
    return EFI_SUCCESS;
}

/**
    Initialize the KBD data

    @param kbd   - Pointer to the KBD structure

    @retval VOID

**/

VOID
vkbd_initParams (
    VIRTKBD  *Kbd
)
{
    Kbd->Enabled = TRUE;
}

/**
    Initialize the KBD data

    @param kbc     - pointer to the KBC Structure
    @param kbd     - Pointer to the KBD structure

    @retval VOID

**/

VOID
InitVirtualKbd (
    KBC     *kbc,
    VIRTKBD *kbd
)
{
    EFI_HANDLE hEmulationService = 0;
    EFI_STATUS Status;
    kbd->qhead = kbd->qtail = kbd->queue;
    kbd->sink.kbc = kbc;
    kbd->sink.onCommand = kbd_scanning;
    kbd->sink.onQueueFree = vkbd_pumpQueue;
    kbd->kbdInput_.Send = kbdInput_Send;
    kbd->kbdInput_.GetTranslation = kbdInput_GetTranslation;
    kbd->kbdInput_.UpdateLEDState = kbdInput_updateLED;
    gKbdProtocol=&kbd->kbdInput_;
    vkbd_initParams(kbd);
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    Status = gSmst1->SmmInstallProtocolInterface(&hEmulationService,
            &gEmul6064KbdInputProtocolGuid,EFI_NATIVE_INTERFACE,(VOID*)&kbd->kbdInput_);
#else
    Status = gBS->InstallProtocolInterface(&hEmulationService,
            &gEmul6064KbdInputProtocolGuid,EFI_NATIVE_INTERFACE,(VOID*)&kbd->kbdInput_);
    ASSERT_EFI_ERROR(Status);
#endif
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

