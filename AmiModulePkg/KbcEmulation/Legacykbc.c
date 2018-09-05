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

/** @file Legacykbc.c
    LegacyKBC handles the with KBC model

**/

//---------------------------------------------------------------------------

#include "KbcEmul.h"
#include "KbcDevEmul.h"
#include "Kbc.h"
#include "KbcEmulLib.h"

//---------------------------------------------------------------------------

#define AUXOBF (KBC_STATUS_AUXB|KBC_STATUS_OBF)
extern BOOLEAN          gTrackPs2Mouse4ByteCheck;
extern USB_GLOBAL_DATA *gUsbDataPtr;
UINT8  MouseSequence [] = { 0x64, 0xD4, 0,
                            0x60, 0xF3, 0,
                            0x64, 0xD4, 0,
                            0x60, 0xC8, 0,
                            0x64, 0xD4, 0,
                            0x60, 0xF3, 0,
                            0x64, 0xD4, 0,
                            0x60, 0x64, 0,
                            0x64, 0xD4, 0,
                            0x60, 0xF3, 0,
                            0x64, 0xD4, 0,
                            0x60, 0x50, 0,
                            0x60, 0xFA, 1,     // Read Port 60
                            0x64, 0xD4, 0,
                            0x60, 0xF2, 0,
                            0x60, 0xFA, 1,     // Read Port 60
                            0x60, 0x03, 1};    // Read Port 60

UINT8    *pMouseSequence = MouseSequence;
BOOLEAN  TrackPort60Read = FALSE;

VOID kbc_passthrough_wdata( KBC* kbc, UINT8 data );
VOID kbc_mouse_wdata( KBC* kbc, UINT8 data );
VOID kbc_wccb( KBC* kbc, UINT8 data );
VOID on_kbc_write_command( KBC* kbc, UINT8 cmd );
UINT8 kbc_passthrough_rstatus( KBC* kbc );
VOID legkbc_updateCCB(LEGACYKBC* legkbc, UINT8 set, UINT8 mask );
VOID kbc_kbd_wdata( KBC* kbc, UINT8 data );

/**
    This routine checks the input buffer free bit and waits till
    it is set by the keyboard controller

    @param None

    @retval BOOLEAN

**/

BOOLEAN
KBC_WaitForInputBufferToBeFree ()
{
    UINT32      count ;
    UINT8       status;

    for( count = KBCTIMEOUT, status = 1; status != 0 && count != 0;
        status= ByteReadIO(KBC_STATUS_REG) & KBC_STATUS_IBF, --count );

    return ( status & KBC_STATUS_IBF)==0;
}

/**
    This routine checks the output buffer full bit and waits till
    it is set by the keyboard controller

    @param None

    @retval BOOLEAN

**/

BOOLEAN
KBC_WaitForOutputBufferToBeFilled ()
{
    UINT32  count ;
    UINT8   status;
    for( count = KBCTIMEOUT, status = 0; status == 0 && count != 0;
        status      = ByteReadIO(KBC_STATUS_REG) & KBC_STATUS_OBF,
        --count );
    return ( status & KBC_STATUS_OBF)!=0;
}

/**
    This routine till the keyboard controller sends a data byte

    @param  None

    @retval UINT8

**/

UINT8
KBC_ReadDataByte ()
{
    KBC_WaitForOutputBufferToBeFilled();
    return ByteReadIO(KBC_DATA_REG);
}

/**
    This routine writes the command to the keyboard controller

    @param bCmd

    @retval VOID

**/
VOID
KBC_WriteCommandByte (
    UINT8  bCmd
)
{
    KBC_WaitForInputBufferToBeFree();
    ByteWriteIO(KBC_COMMAND_REG, bCmd);
    KBC_WaitForInputBufferToBeFree();
}

/**
    This routine writes the command to the keyboard controller

    @param bCmd

    @retval VOID

**/
VOID
KBC_WriteSubCommandByte (
    UINT8  bCmd
)
{
    KBC_WaitForInputBufferToBeFree();
    ByteWriteIO(KBC_DATA_REG, bCmd);
    KBC_WaitForInputBufferToBeFree();
}

/**
    Return current CCB stored in KBC .Traping and tracking KBC ports I/O
    allow to cache the ccb value. Still at the initial value must be
    loaded by reading CCB from KBC (command 20). Care is taken not to
    damage whatever data is stored in KBC

    @param kbc

    @retval UINT8

**/
UINT8
kegkbc_readCCB(
    KBC  *kbc
)
{
    LEGACYKBC* legkbc = _CR(kbc,LEGACYKBC,kbc_);
    UINT8 stSave;
    UINT8 bSave;

    if( legkbc->fCcb_loaded )
        return legkbc->ccb_;

    //
    //Call can come from outside of Trap handler (EFI protocol invoked).We must disable trapping for this case
    //
    TrapEnable(FALSE);

    //
    // lock the KBC
    //
    KBC_WriteCommandByte(KBCCMD_LOCK);

    //
    //Get the Status regsiter value and Data from Data register
    //
    stSave      = ByteReadIO(KBC_STATUS_REG);
    bSave = ByteReadIO(KBC_DATA_REG);
    KBC_WriteCommandByte(KBCCMD_READ_CCB);

    //
    //Get the CCB value and store it and set CCB has been read already.
    //
    legkbc->ccb_ = KBC_ReadDataByte();
    legkbc->fCcb_loaded = TRUE;

    //
    //Push the Data to back to Keyboard Controller.
    //
    if(stSave & KBC_STATUS_OBF){
        if( stSave & KBC_STATUS_AUXB )
            KBC_WriteCommandByte(KBCCMD_WriteAuxBuffer);
        else
            KBC_WriteCommandByte(KBCCMD_WriteKbdBuffer);
        ByteWriteIO(KBC_DATA_REG, bSave);
    }

    //
    //Enable the trap again.
    //
    TrapEnable(TRUE);
    return legkbc->ccb_;
}

/**
    Handle write to port 60. Previouse command written to port 64 was "Send to auxilary device"
    (0xD4). The data is passed to the Mouse emulation machine.
    Next state is: accepting keyboard data on port 60.

    @param  kbc
    @param  data

    @retval VOID

**/

VOID
kbc_mouse_wdata (
    KBC   *kbc,
    UINT8 data
)
{
    LEGACYKBC* legkbc = _CR(kbc,LEGACYKBC,kbc_);
    
    // Tracks for Ps2 Mouse Scroll Wheel Format Support.
    if (gTrackPs2Mouse4ByteCheck) {
        if (*(pMouseSequence + 1) == data) {
            if (*(pMouseSequence + 1) == 0x50) {
                TrackPort60Read = TRUE;
                pMouseSequence+=3;
            } else {
                pMouseSequence+=3;
            }
        } else {
            // Reset the sequence
            pMouseSequence = MouseSequence;
        }
    }
    //
    //enable Aux interface
    //
    legkbc_updateCCB(legkbc,0,KBC_CCB_EN2);

    if(kbc->mouse_dev->present_) {
        KBC_WaitForInputBufferToBeFree();
        ByteWriteIO( KBC_DATA_REG, data );
    }

    //
    //Detect if real PS/2 mouse is present, Wait till responce comes
    //

    if(!kbc->mouse_dev->presence_detected_)
    {
        int c;
        UINT8 st;
        UINT8 mouseData;

        //
        //Mouse detection happens here
        //
        kbc->mouse_dev->presence_detected_= TRUE;

        KBC_WaitForInputBufferToBeFree();

        for( st = 0,c = 0x10000; c != 0 && (st & AUXOBF )!= AUXOBF;c--){
            int c1;
            st = ByteReadIO(KBC_STATUS_REG);
            for( c1 = 0x10000; c1 != 0 && (st & AUXOBF) == KBC_STATUS_OBF; --c1  )
                    st = ByteReadIO(KBC_STATUS_REG);
            if((st & AUXOBF) == KBC_STATUS_OBF  ){
                //not a mouse data; trash it (we can trash a keyboard key)
                //a better detection alg. may be needed
                //at least we can avoid trashing keyboard key: if non-mouse
                //data is found (which must not accure for a correct application)
                //get out of here assuming mouse present for the time (do not send)
                //the responce; enable read port 64 and detect a mouse at the next
                //trapped I/O
                ByteReadIO(KBC_DATA_REG);
            }
        }

        kbc->mouse_dev->present_ = FALSE;
        if( (st & AUXOBF )== AUXOBF ){
            //
            //Mouse responce reached
            //
            mouseData = ByteReadIO(KBC_DATA_REG);
            //
            //Whatever command was it, good responce can not be 0xFE
            //
            kbc->mouse_dev->present_ =  mouseData != 0xFE;

            if( kbc->mouse_dev->present_ ){
                //put the mouse responce back to KBC
                KBC_WriteCommandByte( KBCCMD_WriteAuxBuffer);
                ByteWriteIO(KBC_DATA_REG,mouseData);
                KBC_WaitForInputBufferToBeFree();
            }
        }
    }

  (*kbc->mouse_dev->onCommand)(kbc->mouse_dev, data );

    kbc->kbc_write_data = kbc_kbd_wdata;
}

/**
    Handle write to port 60. The data is passed to the Keyboard emulation machine
    Next state is: accepting keyboard data on port 60.

    @param  kbc
    @param  data

    @retval VOID

**/

VOID
kbc_kbd_wdata (
    KBC   *kbc, 
    UINT8 data
)
{
    LEGACYKBC* legkbc = _CR(kbc,LEGACYKBC,kbc_);

    legkbc_updateCCB(legkbc,0,KBC_CCB_EN); //enable KBD interface

    if(kbc->kbd_dev->present_) {
        KBC_WaitForInputBufferToBeFree();
        ByteWriteIO(KBC_DATA_REG, data);
    }

    //
    //Detect if real PS/2 Keyboard is present, Wait till responce comes
    //
    if(!kbc->kbd_dev->presence_detected_ && (data != 0xDD) && (data !=0xDF))
    {
        int c;
        UINT8 st;
        UINT8 kbdData;

        kbc->kbd_dev->presence_detected_= TRUE;
        KBC_WaitForInputBufferToBeFree();

        for( st = 0,c = 0x10000; c != 0 && (st & AUXOBF )!= KBC_STATUS_OBF;c-- ){
            st = ByteReadIO(KBC_STATUS_REG);
            if( st & KBC_STATUS_TO ){
                break;
            }
        }
        kbc->kbd_dev->present_ = FALSE;
        if( (st & AUXOBF )== KBC_STATUS_OBF ){
            //
            //Keyboard responce reached
            //
            kbdData = ByteReadIO(KBC_DATA_REG);
            //
            //Whatever command was it, good responce can not be 0xFE
            //
            kbc->kbd_dev->present_ =  kbdData != 0xFE;
            if( kbc->kbd_dev->present_ ){
                //
                //put the responce back to KBC
                //
                KBC_WriteCommandByte( KBCCMD_WriteKbdBuffer);
                ByteWriteIO(KBC_DATA_REG,kbdData);
                KBC_WaitForInputBufferToBeFree();
            }
        }
    }
  (*kbc->kbd_dev->onCommand)(kbc->kbd_dev, data );
    kbc->kbc_write_data = kbc_kbd_wdata;
}

/**
    Handle write to port 60. This function handles the data specfic the 
    KBC controller. Next state is: accepting keyboard data on port 60.

    @param  kbc
    @param  data

    @retval VOID

**/
VOID
kbc_kbd_wdata2 (
    KBC   *kbc,
    UINT8 data
)
{

    BOOLEAN     DeviceStatus;
    UINT8 ccb;

    ccb = kegkbc_readCCB(kbc);
    DeviceStatus = (ccb & KBC_CCB_EN);

    //
    //If the device disable, don't put data in KBC
    //
    if(DeviceStatus) {
        return ;
    }

    KBC_WaitForInputBufferToBeFree();
    //
    //Write the Keyboard Data.
    //
    ByteWriteIO( KBC_DATA_REG, data );

    return;
}

/**
    Update cached CCB in responce to trapped KBC command.If CCB wasn't loaded before, the new CCB cache is
    still unkknown.

    @param legkbc
    @param set
    @param mask

    @retval VOID

**/

VOID 
legkbc_updateCCB (
    LEGACYKBC  *legkbc,
    UINT8      set,
    UINT8      mask
)
{
    if(legkbc->fCcb_loaded){
        //
        //CCB value
        //
        legkbc->ccb_ = (legkbc->ccb_ & ~mask ) | (set & mask);
    }
}

/**
    Handle write to port 60. Previouse command written to port 64 was "Write CCB (60h)"
    The data is a new ccb.  Next state is: accepting keyboard data on port 60.

    @param kbc
    @param data

    @retval VOID

**/

VOID
kbc_wccb (
    KBC   *kbc,
    UINT8 data
)
{
    LEGACYKBC* legkbc = _CR(kbc,LEGACYKBC,kbc_);

    // Do we need to wait for IBF?
    // to accept a data as a new value of ccb we must be sure that
    // KBC will take it.
    // Alternative is to check that IBF is clear only once. If IBF is
    // not clear then set fCcb_loaded = FALSE. That will force next
    // legkbc_readCCB to read a value from KBC, but we will not spend
    // time in SMI# (or delay this wait until CCB is needed for emulation)
    for( ;(ByteReadIO(KBC_STATUS_REG) & KBC_STATUS_IBF ) != 0 ; );
    legkbc->ccb_ = data;
    legkbc->fCcb_loaded = TRUE;

    //
    //Send the CCB to KBC
    //
    ByteWriteIO( KBC_DATA_REG, data );
    kbc->kbc_write_data = kbc_kbd_wdata;

}

/**
    Handle write to port 64. New command always aborts previous (no state dependency). This function
    will always get called on port 64 write.Parse the incomming command and change the state accoringly

    @param kbc
    @param cmd

    @retval VOID

**/

VOID
on_kbc_write_command (
    KBC   *kbc,
    UINT8 cmd
)
{
    LEGACYKBC* legkbc = _CR(kbc,LEGACYKBC,kbc_);
    LEGACYMOUSE*  Mouse = (LEGACYMOUSE*)kbc->mouse_dev;

    ByteWriteIO(KBC_COMMAND_REG, cmd);
    //
    //If the command has data, it goes to Keyboard device
    //
    kbc->kbc_write_data = kbc_kbd_wdata;
    switch(cmd){
        case KBCCMD_WriteAuxDev:
            //
            //next data is a mouse data. So send it to mouse device
            //
            kbc->kbc_write_data = kbc_mouse_wdata;
            // Tracks for Ps2 Mouse Scroll Wheel Format Support.
            if (gTrackPs2Mouse4ByteCheck) {
                if (*(pMouseSequence + 1) == cmd) {
                    pMouseSequence+=3;
                } else  {
                    // Reset the sequence
                    pMouseSequence = MouseSequence;
                }
            }
            break;
        case KBCCMD_AUXENBL:
            Mouse->Enabled = TRUE;
            legkbc_updateCCB( legkbc, 0, KBC_CCB_EN2);
            break;
        case KBCCMD_AUXDSBL:
            Mouse->Enabled = FALSE;
            legkbc_updateCCB( legkbc, KBC_CCB_EN2, KBC_CCB_EN2);
            break;
        case KBCCMD_KBDENBL:
            legkbc_updateCCB( legkbc, 0, KBC_CCB_EN);
            break;
        case KBCCMD_KBDDSBL:
            legkbc_updateCCB( legkbc, KBC_CCB_EN, KBC_CCB_EN);
            break;
        case KBCCMD_WRITE_CCB:
            kbc->kbc_write_data = kbc_wccb;
            break;
        case KBCCMD_WriteKbdBuffer:
        case KBCCMD_WriteAuxBuffer:
            kbc->kbc_write_data = kbc_kbd_wdata2;
            break;
        default:
            break;
    }
}

/**
    Handle write to port 60.The emulation doesn't process this write and the access is passed to the KBC

    @param kbc
    @param data

    @retval VOID

**/

VOID
kbc_passthrough_wdata (
    KBC   *kbc,
    UINT8 data
)
{
    ByteWriteIO(KBC_DATA_REG, data);
}

/**
    Handle write to port 64.The emulation doesn't process this write and
    the access is passed to the KBC

    @param kbc
    @param data

    @retval VOID

**/

VOID
kbc_passthrough_wcmd (
    KBC   *kbc,
    UINT8 data
)
{
    ByteWriteIO(KBC_STATUS_REG,data);
}

/**
    Handle read from port 64.The emulation doesn't process this read and
    the access is passed to the KBC

    @param kbc

    @retval UINT8

**/

UINT8
kbc_passthrough_rstatus (
    KBC  *kbc
)
{
    return ByteReadIO (KBC_STATUS_REG);
}

/**
    Handle read from port 60.The access is passed to the KBC. Emulation tracks the read to pump the
    KBC output buffer (in case if there is more data in the queue)

    @param kbc

    @retval UINT8

**/

UINT8
kbc_passthrough_rdata (
    KBC  *kbc
)
{
    UINT8 data;

    //
    //Get the data from KBC
    //
    data = ByteReadIO (KBC_DATA_REG);

    //
    //Push the Keyboard Data to KBC if it has any
    //
    (*kbc->kbd_dev->onQueueFree)(kbc->kbd_dev);

    //
    //Push the Mouse data to KBC, if it has any
    //
    (*kbc->mouse_dev->onQueueFree)(kbc->mouse_dev);
	
	// Below code Tracks for Ps2 Mouse Scroll Wheel Format Support.
	// If the connected Ps2 mouse, support's Scroll wheel then USB driver also should send 4 bytes USB Packet data.
    // If the scroll support Mouse ID(0x03) is returned by the connected Ps2 Ms, then MOUSE_4BYTE_DATA_BIT in MouseStatusFlag
    // is set.

    if (gTrackPs2Mouse4ByteCheck) {
        if (TrackPort60Read) {
           if (*(pMouseSequence + 1) == data)  {
               pMouseSequence+=3;
               if (gUsbDataPtr && (data == SCROLL_SUPPORT_MOUSE_ID)) { 
                   gUsbDataPtr->bMouseStatusFlag |= MOUSE_4BYTE_DATA_BIT_MASK;        // 4 byte data expected MOUSE_4BYTE_DATA_BIT
                   // Reset the sequence
                   pMouseSequence = MouseSequence;
                   TrackPort60Read = FALSE;
               }
           } else {
               // Reset the sequence
               pMouseSequence = MouseSequence;
               TrackPort60Read = FALSE;
           }
       }
    }

    return data;
}

/**
    Implementation of the "send_outb1" for the case of legacy KBC present. Data is put into output buffer
    of legacy KBC which sets OBF and generates IRQ. Application gets a chance to handle the interrupt and
    read the data from legacy KBC.



    @param kbc
    @param devtype
    @param ovrd
    @param data

    @retval BOOLEAN

    @note  condition is checked: OBF is cleard (previous data was processed)

**/

BOOLEAN
legkbc_sendout (
    KBC         *kbc,
    PS2DEV_TYPE devtype,
    BOOLEAN     ovrd,
    UINT8       data
)
{
    UINT8 ccb;
    BOOLEAN DeviceStatus;

    ccb = kegkbc_readCCB(kbc);
    DeviceStatus = (ccb & (devtype == PS2DEV_MOUSE ? KBC_CCB_EN2 : KBC_CCB_EN))==0;

    //
    //If the device disable, don't put data in KBC
    //
    if(!DeviceStatus)
        return FALSE;

    if( KBC_WaitForInputBufferToBeFree()){
        UINT8 st;

        KBC_WriteCommandByte(KBCCMD_LOCK); // lock the KBC
        st = ByteReadIO(KBC_STATUS_REG);

        //
        //If the KBC , if already has data (OBF set) return, If it's free , then place data to KBC
        //
        if( ((st & KBC_STATUS_OBF) != 0) && !ovrd ){
            //Output buffer is full - it's not a time to push our data

            //unlock KBC by sending some command that doesn't trash the Output buffer
            // since we see data from device, we assume that this interface is
            //enabled in KBC. So there is no harm to enable it again. The exception to
            //the rule is a sequence of data that emulation code is pushing into the KBC:
            //to isolate sequence from asichncronose input from a real device, we disable
            //the interface in KBC but still the data will appear in buffer. aux_en and kbd_en
            //track the disabling/enabling interface by emulation code.
            KBC_WriteCommandByte( (st & KBC_STATUS_AUXB) != 0? kbc->aux_en : kbc->kbd_en );
            return FALSE;
        }
        //
        //OBF is not set and KBC is ready to get the data.\
        //
        KBC_WriteCommandByte( devtype == PS2DEV_MOUSE ? KBCCMD_WriteAuxBuffer:KBCCMD_WriteKbdBuffer );
        ByteWriteIO(KBC_DATA_REG,data);
        KBC_WaitForInputBufferToBeFree();
        return TRUE;
    }
    return FALSE;
}

/**
    Initilize the KBC structure

    @param kbc
    @param kbd
    @param mouse

    @retval VOID

**/

VOID
initLegacyKBC (
    KBC     *kbc,
    PS2SINK *kbd,
    PS2SINK *mouse
)
{
    kbc->mouse_dev = mouse;
    kbc->kbd_dev = kbd;
    kbc->sqTail[PS2DEV_KBD] = kbc->sqHead[PS2DEV_KBD] = kbc->send_queue;
    kbc->sqTail[PS2DEV_MOUSE] = kbc->sqHead[PS2DEV_MOUSE] = kbc->send_queue_aux;
    kbc->aux_en = KBCCMD_AUXENBL;
    kbc->kbd_en = KBCCMD_KBDENBL;
    kbd->present_ = TRUE;
    kbd->presence_detected_ = FALSE;
    mouse->presence_detected_ = FALSE;
    mouse->present_ = TRUE;
}

/**
    Initialize Legacy KBC object

    @param legkbc
    @param kbd
    @param mouse

    @retval VOID

**/

VOID
InitLegacyKBC (
    LEGACYKBC  *legkbc,
    PS2SINK    *kbd,
    PS2SINK    *mouse
)
{
    initLegacyKBC(&legkbc->kbc_,kbd,mouse);
    legkbc->kbc_.kbc_write_command = on_kbc_write_command;
    legkbc->kbc_.kbc_write_data = kbc_kbd_wdata;
    legkbc->kbc_.kbc_read_status = kbc_passthrough_rstatus;
    legkbc->kbc_.kbc_read_data = kbc_passthrough_rdata;
    legkbc->kbc_.send_outb1 = legkbc_sendout;
    legkbc->kbc_.read_ccb = kegkbc_readCCB;
    legkbc->ccb_ = 0;
    legkbc->fCcb_loaded = FALSE;
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

