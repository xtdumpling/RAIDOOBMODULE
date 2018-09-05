//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file VirtualMouse.c
    Handles the Virtual mouse functions

**/

//---------------------------------------------------------------------------

#include "KbcEmul.h"
#include "KbcDevEmul.h"
#include "KbcEmulLib.h"
#if OHCI_EMUL_SUPPORT
#include "KbcOhci.h"
#endif
//---------------------------------------------------------------------------

#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))

static UINT8 resetResp[] = {0xFA, 0xAA, 0x00 } ;
static UINT8 statusResp[] = {0xFA, 0x20, 0x02, 0x64  } ;
static UINT8 dataResp[] = {0xFA, 0x08, 0x00, 0x0  } ;
static UINT8 ackResp[] = {0xFA};
static UINT8 idResp[] = {0xFA, 0};
static UINT8 unknResp[] = {0xFE };

VOID    mouse_reciev_option(PS2SINK* mouse, UINT8 cmd);
VOID    mouse_initParams(VIRTMOUSE* mouse);
VOID    vmouse_flushBuffer(VIRTMOUSE* mouse);
VOID    vmouse_send_resp(VIRTMOUSE* mouse, UINT8* data, int count);
VOID    mouse_echo(PS2SINK* mouse, UINT8 cmd);
BOOLEAN SendMouseBuffer(VIRTMOUSE* mouse, UINT8* packet, UINTN Count);
VOID    vmouse_pumpQueue(PS2SINK* ps2dev);
extern  GenerateIRQ12(VIRTKBC*);
#if OHCI_EMUL_SUPPORT
extern  BOOLEAN gClearCharacterPending;
#endif

/**
    Handle commands sent from KBC to PS2 Mouse. Mouse idle, 
    waiting for command or key

    @param ps2dev
    @param Cmd

    @retval VOID

**/
VOID 
mouse_idle (
    PS2SINK  *ps2dev, 
    UINT8    cmd
)
{
    VIRTMOUSE* mouse = (VIRTMOUSE*)ps2dev;

    switch(cmd){
        //reset
        case 0xFF:
            mouse_initParams(mouse);
            vmouse_send_resp(mouse, resetResp,COUNTOF(resetResp));
            break;
        //report status
        case 0xE9:
            vmouse_send_resp(mouse, statusResp,COUNTOF(statusResp));
            break;
        //read data
        case 0xEB:
            MemCpy(dataResp+1,&mouse->dataPacket,sizeof(mouse->dataPacket));
            vmouse_flushBuffer(mouse);
            vmouse_send_resp(mouse, dataResp,COUNTOF(dataResp));
            break;
        //Enable Mouse
        case 0xF4:
            mouse->Enabled = TRUE;
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            break;
        //Disable Mouse
        case 0xF5:
            mouse->Enabled = FALSE;
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            break;
        //Set Streaming
        case 0xEA:
            mouse->streaming = 1;
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            break;
        //Set Remote
        case 0xF0:
            mouse->streaming = 0;
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            break;
        //Set Standard, Set Scaling and Reset Scaling
        case 0xF6:
        case 0xE6:
        case 0xE7:
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            break;
        //Identify
        case 0xF2:
            vmouse_send_resp(mouse, idResp,COUNTOF(idResp));
            break;

        // set resolution
        case 0xE8:
            mouse->option_ptr = &mouse->resolution_;
            ps2dev->onCommand = mouse_reciev_option;
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            break;
        // set sampling rate
        case 0xF3:
            mouse->option_ptr = &mouse->samplingRate_;
            ps2dev->onCommand = mouse_reciev_option;
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            break;
        case 0xEE:
            vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
            ps2dev->onCommand = mouse_echo;
            break;
        //unknown command; send FE
        default:
            vmouse_send_resp(mouse, unknResp,COUNTOF(unknResp));
            break;
    }
}

/**
    Mouse Echo command

    @param ps2dev
    @param Cmd

    @retval VOID

**/

VOID 
mouse_echo (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    VIRTMOUSE* mouse = (VIRTMOUSE*)ps2dev;
    if( cmd == 0xEC ){
        ps2dev->onCommand = mouse_idle;
        vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
    } else
        vmouse_send_resp(mouse, &cmd,1);

}

/**
    Mouse reciev command

    @param ps2dev
    @param Cmd

    @retval VOID

**/

VOID 
mouse_reciev_option (
    PS2SINK  *ps2dev,
    UINT8    cmd
)
{
    VIRTMOUSE* mouse = (VIRTMOUSE*)ps2dev;
    *(mouse->option_ptr)  = cmd;
    ps2dev->onCommand = mouse_idle;
    vmouse_send_resp(mouse, ackResp,COUNTOF(ackResp));
}

/**
    Mouse Input source sends data. Put the data into the buffer, 
    attempt to send the first byte. The call comes outside of 
    trapping Leg access. 
    Check that Trapping status will not be overriden

    @param This
    @param Data

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI 
msInput_Send (
    IN EFI_EMUL6064MSINPUT_PROTOCOL  *This,
    IN PS2MouseData*                 data
)
{
    VIRTMOUSE* mouse = _CR(This,VIRTMOUSE,msInput_);
#if USB_MOUSE_UPDATE_EBDA_DATA
    UINT16     ebdaSeg = *(UINT16*)0x40E;
    UINT8      *ebda = (UINT8*)((UINTN)ebdaSeg<<4);

    // Return EFI NOT READY if EBDA has some data.
    // Let USB resend the data is the EBDA is not empty
    if ( 0 != ebda[0x26] ) {
        return EFI_NOT_READY;
    }
#endif
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
    

    //clear all previous flags except overflow
    mouse->dataPacket.flags &= (PS2MSFLAGS_XO|PS2MSFLAGS_YO);

    //copy overflow and button flags from new packet
    mouse->dataPacket.flags |= data->flags & ~(PS2MSFLAGS_XSIGN|PS2MSFLAGS_YSIGN);
    mouse->dataPacket.flags |= 0x8;
    if( (mouse->dataPacket.flags & PS2MSFLAGS_XO) == 0){
        if(data->flags & PS2MSFLAGS_XSIGN){
            mouse->x += (signed short)((UINT8)data->x | 0xFF00);
        } else {
            mouse->x += data->x;
        }
        mouse->dataPacket.x = (UINT8)(UINT32)(mouse->x);
        if( mouse->x < 0 ){
            mouse->dataPacket.flags |= PS2MSFLAGS_XSIGN;
            if( mouse->x < -256 )
                mouse->dataPacket.flags |= PS2MSFLAGS_XO;
        } else {
            if( mouse->x > 256 )
                mouse->dataPacket.flags |= PS2MSFLAGS_XO;
        }
    }

    if( (mouse->dataPacket.flags & PS2MSFLAGS_YO) == 0){
        if(data->flags & PS2MSFLAGS_YSIGN){
            mouse->y += (signed short)((UINT8)data->y | 0xFF00);
        } else {
            mouse->y += data->y;
        }
        mouse->dataPacket.y = (UINT8)(UINT32)(mouse->y);
        if( mouse->y < 0 ){
            mouse->dataPacket.flags |= PS2MSFLAGS_YSIGN;
            if( mouse->y < -256 )
                mouse->dataPacket.flags |= PS2MSFLAGS_YO;
        } else{
            if( mouse->y > 256 )
                mouse->dataPacket.flags |= PS2MSFLAGS_YO;
        }
    }

    mouse->fFreshPacket = 1;
    if( mouse->Enabled && mouse->streaming ){
        if( SendMouseBuffer(mouse,(UINT8*)&mouse->dataPacket,sizeof(mouse->dataPacket)))
        {
            vmouse_flushBuffer(mouse);
        }
    }
    return EFI_SUCCESS;
}

/**
    Send the mouse packet to KBC

    @param Mouse
    @param Packetr
    @param Count

    @retval BOOLEAN

**/

BOOLEAN 
SendMouseBuffer (
    VIRTMOUSE  *mouse,
    UINT8*     packet,
    UINTN      Count
)
{
    UINTN   FreeQueue = 0;
#if USB_MOUSE_UPDATE_EBDA_DATA
    UINT16  ebdaSeg = *(UINT16*)0x40E;
    UINT8   *ebda = (UINT8*)((UINTN)ebdaSeg<<4);
    UINT8   packageSize = ebda[0x27] & 7;
    UINT8   index=0;
#endif

    // count of free items in the queue
    if( mouse->qtail >= mouse->qhead )
        FreeQueue = MOUSEQUEUE_SIZE - (mouse->qtail - mouse->qhead);
    else
        FreeQueue = mouse->qhead - mouse->qtail;

    if(FreeQueue < Count )
        return TRUE;

    //place the Mouse data in mouse buffer
    for( ; Count > 0; --Count ){
        *mouse->qtail++ = *packet++;
        if(mouse->qtail >= mouse->queue+MOUSEQUEUE_SIZE)
            mouse->qtail = mouse->queue;
    }

    //Place the dummy 4th byte in mouse buffer,
//  *mouse->qtail++ = 0;
//  if(mouse->qtail >= mouse->queue+MOUSEQUEUE_SIZE)
//  mouse->qtail = mouse->queue;

#if USB_MOUSE_UPDATE_EBDA_DATA
    // Write the 1st two bytes of mouse packet to EBDA and last byte to KBC
    for (; index<=packageSize; index++) {
        if (index == 0) {
            ebda[0x28+index] = *mouse->qhead;
            ebda[0x26]++;
            if(++mouse->qhead >=  mouse->queue+MOUSEQUEUE_SIZE)
                mouse->qhead = mouse->queue;
        }
        if (index == 1) {
            ebda[0x28+index] = *mouse->qhead;
            ebda[0x26]++;
            if(++mouse->qhead >=  mouse->queue+MOUSEQUEUE_SIZE)
                mouse->qhead = mouse->queue;
        }
        if (index == 2) {
            vmouse_pumpQueue(&mouse->sink);
        }
    }
#else
    //Put the first byte in the KBC buffer
    vmouse_pumpQueue(&mouse->sink);
#if OHCI_EMUL_SUPPORT
    // If there is any pending Mouse data to send to the application software, set the
    // CharacterPending bit in the HceControl register. This causes the Host Controller 
    // to generate an emulation interrupt on the next frame boundary, after the application 
    // has read from port 60h (HceOutput.)
    if ( mouse->qhead != mouse->qtail ) {
    	SetHceControl(GetHceControl() | HCE_CNTRL_CHARACTER_PENDING);
        gClearCharacterPending = FALSE;
    }
#endif
#endif

    return TRUE;
}

/**
    KBC has empty queue; Legacy I/O trapping is being processed.
    If mouse has data to push into KBC, now is a good time to do it

    @param ps2dev - pointer to the Mouse structure

    @retval VOID

**/

VOID 
vmouse_pumpQueue (
    PS2SINK  *ps2dev
)
{
    VIRTMOUSE* mouse = (VIRTMOUSE*)ps2dev;

    //Put the data into KBC
    if((mouse->qhead != mouse->qtail) && (ps2dev->kbc->send_outb1(ps2dev->kbc,PS2DEV_MOUSE,FALSE,*mouse->qhead))) {
        if(++mouse->qhead >=  mouse->queue+MOUSEQUEUE_SIZE)
            mouse->qhead = mouse->queue;
    }
}

/**
    Place the Response bytes in the keyboard buffer.

    @param Mousev
    @param Data
    @param count

    @retval VOID

**/

VOID 
vmouse_send_resp (
    VIRTMOUSE  *mouse,
    UINT8*     data,
    int        count
)
{
    if(mouse->sink.present_ ){
        // Emulation must provide the response. Override any previos mouse data queued to KBC
        mouse->qtail = mouse->qhead = mouse->queue;
        for( ; count > 0; --count ){
            *mouse->qtail++ = *data++;
        }

        // push the first byte to the KBC
        if( mouse->qhead != mouse->qtail &&
            mouse->sink.kbc->send_outb1(mouse->sink.kbc,
            PS2DEV_MOUSE,TRUE,*mouse->qhead))
        {
            if(++mouse->qhead >=  mouse->queue+MOUSEQUEUE_SIZE)
                mouse->qhead = mouse->queue;        }
    }
}

/**
    Clear the mouse data buffer that tracks the mouse changes; 
    after the data is cleared any data read will return 
    zero (x,y) and current buttons state.

    @param mouse - pointer to the Mouse structure

    @retval VOID

**/

VOID 
vmouse_flushBuffer (
    VIRTMOUSE  *mouse
)
{
    mouse->x = mouse->y = 0;
    mouse->dataPacket.x = mouse->dataPacket.y = 0;
    mouse->dataPacket.flags &= (PS2MSFLAGS_LBUTTON|PS2MSFLAGS_RBUTTON);
    mouse->fFreshPacket = 0;
}

/**
    Initialize the mouse data

    @param mouse - Pointer to the mouse structure

    @retval VOID

**/

VOID 
mouse_initParams (
    VIRTMOUSE  *mouse
)
{
    mouse->samplingRate_ = 0x64;
    mouse->resolution_ = 2;
    mouse->option_ptr = 0;
    mouse->Enabled = FALSE;
    mouse->streaming = 1;
}

/**
    Initialize the Mouse data

    @param kbc   - pointer to the KBC Structure
    @param Mouse - Pointer to the Mouse structure

    @retval VOID

**/
VOID 
InitVirtualMouse (
    KBC       *kbc,
    VIRTMOUSE *mouse
)
{
    EFI_HANDLE hEmulationService = 0;
    EFI_STATUS Status;

    mouse->qhead = mouse->qtail = mouse->queue;
    mouse->sink.kbc = kbc;
    mouse->sink.onCommand = mouse_idle;
    mouse->sink.onQueueFree = vmouse_pumpQueue;
    mouse->msInput_.Send = msInput_Send;
    mouse->x = mouse->y = 0;
    mouse->dataPacket.flags = 0x8;
    mouse->dataPacket.x = 0;
    mouse->dataPacket.y = 0;
    mouse->fFreshPacket = 0;
    mouse_initParams(mouse);

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    Status = gSmst1->SmmInstallProtocolInterface(&hEmulationService,
            &gEmul6064MsInputProtocolGuid,EFI_NATIVE_INTERFACE,(VOID*)&mouse->msInput_);
#else
    Status = gBS->InstallProtocolInterface(&hEmulationService,
            &gEmul6064MsInputProtocolGuid,EFI_NATIVE_INTERFACE,(VOID*)&mouse->msInput_);
    ASSERT_EFI_ERROR(Status);
#endif

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

