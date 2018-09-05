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

/** @file Legacykbd.c
    Legacy Keyboard functions

**/

//---------------------------------------------------------------------------

#include "KbcEmul.h"
#include "KbcDevEmul.h"
#include "KbcEmulLib.h"

//---------------------------------------------------------------------------

VOID    LegacyKbd_initParams(LEGACYKBD* kbd);
VOID    LegacyKbd_read_codes(PS2SINK* ps2dev, UINT8 cmd);
VOID    LegacyKbd_read_option(PS2SINK* ps2dev, UINT8 cmd);
VOID    LegacyKbd_rw_scancode(PS2SINK* ps2dev, UINT8 cmd);
VOID    LegacyKbd_send_resp(LEGACYKBD* mouse, UINT8* data, int count);

BOOLEAN KBC_WaitForInputBufferToBeFree ();
BOOLEAN KBC_WaitForOutputBufferToBeFilled ();
UINT8   KBC_ReadDataByte ();
VOID    KBC_WriteSubCommandByte(UINT8   bCmd);
VOID    KBC_WriteCommandByte(UINT8  bCmd);

#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))

static UINT8 ackResp[] = {0xFA};
static UINT8 ResetResp[] = {0xFA,0xAA};
static UINT8 IdResp[] = {0xFA,0xAB, 0x41};
static UINT8 ScanCodePageResp[] = {0xFA, 0x00};

/**
    Handle commands sent from KBC to PS2 Keyboard. Keyboard is scanning or idle, waiting for command 
    or key

    @param ps2dev  - pointer to the KBD structure
    @param Cmd     - Data from the kbc buffer

    @retval VOID

**/

VOID
Legacykbd_scanning (
    PS2SINK  *ps2dev,
    UINT8    cmd)
{
    LEGACYKBD* kbd = _CR(ps2dev,LEGACYKBD,sink);
    switch(cmd){
        case 0xFF: //reset
            LegacyKbd_initParams(kbd);
            LegacyKbd_send_resp(kbd, ResetResp,COUNTOF(ResetResp));
            break;
        case 0xFD:
        case 0xFC:
        case 0xFB:
            LegacyKbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            kbd->read_code_action = cmd;
            kbd->sink.onCommand = LegacyKbd_read_codes;
            break;
        case 0xFA:
        case 0xF9:
        case 0xF8:
        case 0xF7:
        case 0xF6:
        case 0xF5:  //Disable
        case 0xF4:  //Enable
            LegacyKbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            break;
        case 0xF3:  //Set Typeatic Rate/Delay
            LegacyKbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            kbd->sink.onCommand = LegacyKbd_read_option;
            kbd->option_ptr = &kbd->typematicRateDelay;
            break;
        case 0xF2:
            LegacyKbd_send_resp(kbd, IdResp,COUNTOF(IdResp));
            break;
        case 0xF0:
            LegacyKbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            kbd->sink.onCommand = LegacyKbd_rw_scancode;
            kbd->option_ptr = &kbd->scancodepage;
            break;

        case 0xEE:  //Echo
            LegacyKbd_send_resp(kbd, &cmd,1);
            break;

        case 0xED:  //LED
            LegacyKbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
            kbd->sink.onCommand = LegacyKbd_read_option;
            kbd->option_ptr = &kbd->typematicRateDelay;
            break;
    }
}
/**
    Keyboard Read Codes.

    @param ps2dev  - pointer to the KBD structure
    @param Cmd     - Data from the kbc buffer

    @retval VOID

**/

VOID
LegacyKbd_read_codes (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    LEGACYKBD* kbd = _CR(ps2dev,LEGACYKBD,sink);
    if( cmd >= 0xED ){
        kbd->sink.onCommand = Legacykbd_scanning;
        Legacykbd_scanning(ps2dev,cmd);
    } else
        LegacyKbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
}

/**
    Keyboard Read option.

    @param ps2dev  - pointer to the KBD structure
    @param Cmd     - Data from the kbc buffer

    @retval VOID

**/

VOID
LegacyKbd_read_option (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    LEGACYKBD* kbd = _CR(ps2dev,LEGACYKBD,sink);
    kbd->sink.onCommand = Legacykbd_scanning;
    if( cmd >= 0xED ){
        Legacykbd_scanning(ps2dev,cmd);
    } else  {
        *(kbd->option_ptr) = cmd;
        LegacyKbd_send_resp(kbd, ackResp,COUNTOF(ackResp));
    }
}

/**
    Keyboard Read write Scan code.

    @param ps2dev  - pointer to the KBD structure
    @param Cmd     - Data from the kbc buffer

    @retval VOID

**/

VOID
LegacyKbd_rw_scancode (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    LEGACYKBD* kbd = _CR(ps2dev,LEGACYKBD,sink);
    if(cmd != 0 )
        LegacyKbd_read_option(ps2dev,cmd);
    else {
        kbd->sink.onCommand = Legacykbd_scanning;
        ScanCodePageResp[1] = kbd->scancodepage;
        LegacyKbd_send_resp(kbd, ScanCodePageResp,COUNTOF(ScanCodePageResp));
    }
}

/**
    Place the Responce bytes in the keyboard buffer.

    @param ps2dev      - pointer to the KBD structure
    @param Data        - Pointer to the Data
    @param count       - Length of the data

    @retval VOID

**/

VOID
LegacyKbd_send_resp (
    LEGACYKBD  *kbd,
    UINT8      *data,
    int        count
)
{
    //
    //When there is no PS2 Keyboard present in the system, Emulation should send the responce byte for the keyboard command.
    //If PS2 Keyboard present, it will send the responce to the KBC. So Emulation code doesn't need to responce it.
    //
    if(!kbd->sink.present_ ){
        //
        // Emulation must provide the responce. Override any previos kbd data queued to KBC
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
                kbd->qhead = kbd->queue;        }
    }
}

/**
    KBC has empty queue; Legacy I/O trapping is being processed.If Kbd has data to push into KBC,
    now is a good time to do it

    @param ps2dev  - pointer to the KBD structure

    @retval VOID

**/

VOID
LegacyKbd_pumpQueue (
    PS2SINK  *ps2dev
)
{
    LEGACYKBD* kbd = _CR(ps2dev,LEGACYKBD,sink);
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
LegacykbdInput_Send (
    EFI_EMUL6064KBDINPUT_PROTOCOL *p,
    UINT8                         *buffer,
    UINT32                        count
)
{
    LEGACYKBD* kbd = _CR(p,LEGACYKBD,kbdInput_);
    UINTN c = 0;

    //
    // count of free items in the queue
    //
    if( kbd->qtail >= kbd->qhead )
        c = KBDQUEUE_SIZE - (kbd->qtail - kbd->qhead);
    else
        c = kbd->qhead - kbd->qtail -1;

    if(c < count )
        return EFI_NOT_AVAILABLE_YET;

    for( ; count > 0; --count ){
        *kbd->qtail++ = *buffer++;
        if(kbd->qtail >= kbd->queue+KBDQUEUE_SIZE)
            kbd->qtail = kbd->queue;
    }

    //
    // Disable trap to access real harware
    //
    TrapEnable(FALSE);
    LegacyKbd_pumpQueue(&kbd->sink);
    //
    // Enable trap back and clear statuses
    //
    TrapEnable(TRUE);

    return EFI_SUCCESS;
}

/**
    KBC translation

    @param P
    @param outTrans

    @retval EFI_STATUS

**/

EFI_STATUS 
EFIAPI
LegacykbdInput_GetTranslation (
    EFI_EMUL6064KBDINPUT_PROTOCOL *p,
    OUT KBC_KBDTRANSLATION        *outTrans
)
{
    LEGACYKBD* kbd = _CR(p,LEGACYKBD,kbdInput_);
    *outTrans = (kbd->sink.kbc->read_ccb(kbd->sink.kbc) & KBC_CCB_XLAT) != 0?
            KBC_KBDTRANS_PCXT : KBC_KBDTRANS_AT;
    return EFI_SUCCESS;
}

/**
    Update the keyboard LED's

    @param P       - pointer to the Emulation Protocol
    @param Data    - LED data

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
LegacykbdInput_updateLED (
    EFI_EMUL6064KBDINPUT_PROTOCOL  *p,
    IN UINT8                       data
)
{
    UINT8       bKBData = 0;
    UINT8       bSave;

    //
    // Disable trap to access real harware
    //
    TrapEnable(FALSE);
    //
    // Wait for input buffer to be free
    //
    KBC_WaitForInputBufferToBeFree();
    KBC_WriteCommandByte(KBCCMD_LOCK);
    bSave = ByteReadIO(KBC_STATUS_REG);

    //
    // Data is pending. Read it in AL
    //
    if(bSave & KBC_STATUS_OBF)
    {
        bKBData = ByteReadIO(KBC_DATA_REG);
    }
    KBC_WriteCommandByte(0xAE);
    KBC_WriteSubCommandByte(0xED);
    KBC_ReadDataByte();
    KBC_WriteSubCommandByte(data);
    KBC_ReadDataByte();

    if(bSave & KBC_STATUS_OBF){
        KBC_WriteCommandByte((bSave & KBC_STATUS_AUXB) ? 0xD3: 0xD2);
        KBC_WriteSubCommandByte(bKBData);
    }
    //
    // Enable trap back and clear statuses
    //
    TrapEnable(TRUE);

    return EFI_SUCCESS;
}

/**
    Initialize the KBD data

    @param kbd   - Pointer to the KBD structure

    @retval VOID

**/

VOID
LegacyKbd_initParams (
    LEGACYKBD  *kbd
)
{
}

/**
    Initialize the KBD data

    @param kbc     - pointer to the KBC Structure
    @param kbd     - Pointer to the KBD structure

    @retval VOID

**/
VOID
InitLegacyKbd (
    KBC       *kbc,
    LEGACYKBD *kbd
)
{
    EFI_HANDLE hEmulationService = 0;
    EFI_STATUS Status;

    kbd->qhead = kbd->qtail = kbd->queue;
    kbd->sink.kbc = kbc;
    kbd->sink.onCommand = Legacykbd_scanning;
    kbd->sink.onQueueFree = LegacyKbd_pumpQueue;
    kbd->kbdInput_.Send = LegacykbdInput_Send;
    kbd->kbdInput_.GetTranslation = LegacykbdInput_GetTranslation;
    kbd->kbdInput_.UpdateLEDState = LegacykbdInput_updateLED;
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

