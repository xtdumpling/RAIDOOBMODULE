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

/** @file VirtualKbc.c
    Handles the Virtual KBC(Without KBC system) function

**/

//---------------------------------------------------------------------------

#include "KbcEmul.h"
#include "KbcDevEmul.h"
#include "Kbc.h"

//---------------------------------------------------------------------------

VOID    vkbc_write_command( KBC* kbc, UINT8 cmd);
UINT8   vkbc_read_status( KBC* kbc);
VOID    vkbc_write_data( KBC* kbc, UINT8 cmd);
UINT8   vkbc_read_data( KBC* kbc);
BOOLEAN virtkbc_sendout(KBC* kbc, PS2DEV_TYPE devtype,BOOLEAN ovrd, UINT8 data);
VOID    initKBC(KBC* kbc, PS2SINK* kbd, PS2SINK* mouse);

//
//Depend on the KBC command generate IRQ12
//
BOOLEAN     MouseIrq= TRUE;

//
//Depend on the KBC command generate IRQ1
//
BOOLEAN     KeyboardIrq= TRUE;

//
//To hold the keyboard outport Status.
//
UINT8       OutPortStatus= 0xDF;

//
//To hold the number of Mouse byte already send
//
UINT8       PacketSize=00;

extern  EFI_EMUL6064KBDINPUT_PROTOCOL   *gKbdProtocol;

#if IRQ_EMUL_SUPPORT
extern  GenerateIRQ12(VIRTKBC*);
extern  GenerateIRQ1(VIRTKBC*);
#endif

#if OHCI_EMUL_SUPPORT
UINT8   GetHceStatus(VOID);
#endif

/**
    Handle write to command port of virtual KBC

    @param kbc     - pointer to the KBC structure
    @param cmd     - keyboard controller command

    @retval VOID

**/

VOID
vkbc_write_command (
    KBC   *kbc,
    UINT8 cmd
)
{
    VIRTKBC*    vkbc = (VIRTKBC*)kbc;
    VIRTMOUSE*  Mouse = (VIRTMOUSE*)kbc->mouse_dev;
    VIRTKBD*    Kbd = (VIRTKBD*)kbc->kbd_dev;
    EFI_RUNTIME_SERVICES 	*SmmRuntimeVar;
    EFI_GUID SmmRsTableGuid = EFI_SMM_RUNTIME_SERVICES_TABLE_GUID;
    UINT8 Index;

    SmmRuntimeVar = NULL;

    //
    //Save the KBC command
    //
    vkbc->kbc_command_ = cmd;
    vkbc->st_ |= KBC_STATUS_A2;
    vkbc->TwoByteCmd = FALSE;

    //
    //Process the KBC command
    //
    switch( vkbc->kbc_command_ ){
        case KBCCMD_READ_CCB:
                //move ccb into the ouput buffer
                virtkbc_sendout(kbc,PS2DEV_KBD,TRUE,vkbc->ccb_);
                break;
        case KBCCMD_KBDDSBL:
                vkbc->ccb_|=CCB_KEYBOARD_DISABLED;
                Kbd->Enabled= FALSE;
                break;
        case KBCCMD_KBDENBL:
                vkbc->ccb_&=(~CCB_KEYBOARD_DISABLED);
                Kbd->Enabled= TRUE;
                break;
        case KBCCMD_AUXDSBL:
                vkbc->ccb_|=CCB_MOUSE_DISABLED;
                Mouse->Enabled = FALSE;
                break;
        case KBCCMD_AUXENBL:
                vkbc->ccb_&=(~CCB_MOUSE_DISABLED);
                Mouse->Enabled = TRUE;
                break;
        case KBCCMD_ReadOutPort:
                virtkbc_sendout(kbc,PS2DEV_KBD,TRUE,OutPortStatus);
                break;
        //
        //Selft Test- Returns 0x55 if okay 
        //
        case KBCCMD_SelfTest:
                virtkbc_sendout(kbc,PS2DEV_KBD,TRUE,KBCCMD_RES_SelfTestOk);
                break;
        //
        //Returns 0x00 if okay, 0x01 if Clock line stuck low, 0x02 if clock line stuck high,
        //0x03 if data line stuck low, and 0x04 if data line stuck high
        //
        case KBCCMD_CheckKbd:
                virtkbc_sendout(kbc,PS2DEV_KBD,TRUE,KBCCMD_RES_KBInterfaceOk);
                break;
        //
        //Reset the System for KBC command 0xFE
        //
        case KBCCMD_ResetSystem:

                for (Index = 0; Index < gSmst1->NumberOfTableEntries; ++Index) {
                    if (guidcmp(&gSmst1->SmmConfigurationTable[Index].VendorGuid, \
                                        &SmmRsTableGuid) == 0) {
                        break;
                    }
                }
                if (Index != gSmst1->NumberOfTableEntries) {
                     SmmRuntimeVar =(EFI_RUNTIME_SERVICES *) gSmst1->SmmConfigurationTable[Index].VendorTable;

                }
                if(SmmRuntimeVar){
                   SmmRuntimeVar->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
                }
                break;
        //
        //Below are 2 byte commands. Next byte expected in Port 60.
        //    
        case KBCCMD_WriteAuxDev:
        case KBCCMD_WRITE_CCB:
        case KBCCMD_WriteAuxBuffer:
        case KBCCMD_WriteKbdBuffer:
        case KBCCMD_WriteOutPort:
                vkbc->TwoByteCmd = TRUE;
                break;

        default:
                break;
    }
}

/**
    Handle read from status port of virtual KBC

    @param kbc  - pointer to the KBC structure

    @retval UINT8

**/

UINT8
vkbc_read_status (
    KBC  *kbc
)
{
    VIRTKBC* vkbc = (VIRTKBC*)kbc;

    //
    //If we send 1 byte of mouse already , then try to send the remaining 2 bytes of data first and process the keyboard data.
    //

    if(PacketSize >0 && PacketSize < MOUSE_PACKET_SIZE) {
        //
        //Check the OBF, If it's empty , check Mouse queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->mouse_dev->onQueueFree)(kbc->mouse_dev);
        }

        //
        //Check the OBF, If it's empty , check KBD queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->kbd_dev->onQueueFree)(kbc->kbd_dev);
        }
    } else {
        //
        //Try to process the keyboard data first.
        //

        PacketSize=0;
        //
        //Check the OBF, If it's empty , check KBD queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->kbd_dev->onQueueFree)(kbc->kbd_dev);
        }

        //
        //Check the OBF, If it's empty , check Mouse queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->mouse_dev->onQueueFree)(kbc->mouse_dev);
        }
    }

    return vkbc->st_;
}

/**
    Handle write to data port of virtual KBC

    @param kbc     - pointer to the KBC structure
    @param Data    - keyboard controller Data

    @retval void

**/
VOID
vkbc_write_data (
    KBC   *kbc,
    UINT8 cmd
)
{
    VIRTKBC* vkbc = (VIRTKBC*)kbc;
    VIRTMOUSE* Mouse = (VIRTMOUSE*)kbc->mouse_dev;
    VIRTKBD* Kbd = (VIRTKBD*)kbc->kbd_dev;

    //
    //Check that perviously we got command in port64, if yes then we got data byte for the pervious command
    //
    if (vkbc->TwoByteCmd) {
        vkbc->TwoByteCmd = FALSE;
        switch( vkbc->kbc_command_ ){
            case KBCCMD_WriteAuxDev:
                if(kbc->mouse_dev)
                    (*kbc->mouse_dev->onCommand)(kbc->mouse_dev,cmd);
                vkbc->st_ |= KBC_STATUS_AUXB | KBC_STATUS_OBF;
                break;
            case KBCCMD_WRITE_CCB:
                KeyboardIrq = (cmd & BIT0) ? TRUE : FALSE;
                MouseIrq = (cmd & BIT1) ? TRUE : FALSE;
                Kbd->Enabled = (cmd & BIT4) ? FALSE : TRUE;
                Mouse->Enabled = (cmd & BIT5) ? FALSE : TRUE;
                vkbc->ccb_ = cmd;
                break;
            case KBCCMD_WriteAuxBuffer:
                vkbc->st_ |= KBC_STATUS_AUXB | KBC_STATUS_OBF;
                vkbc->outb_ = cmd;
                break;
            case KBCCMD_WriteKbdBuffer:
                gKbdProtocol->Send(gKbdProtocol, (UINT8 *)&cmd,1);
                break;
            case KBCCMD_WriteOutPort:
                OutPortStatus=cmd;
                break;
            default:
                break;
        }
    } else {
        //
        // We got data byte here and process the data byte
        //
        if(kbc->kbd_dev)
            (*kbc->kbd_dev->onCommand)(kbc->kbd_dev,cmd);
    }

    //
    //reset the command and status that command has been processed.
    //
    vkbc->kbc_command_ = 0;
    vkbc->st_ &= ~KBC_STATUS_A2;
}

/**
    Handle read from data port of virtual KBC

    @param kbc  - pointer to the KBC structure

    @retval UINT8

**/

UINT8
vkbc_read_data (
    KBC  *kbc
)
{
    VIRTKBC*    vkbc = (VIRTKBC*)kbc;
    UINT8       Data;

    Data = vkbc->outb_;

    if(vkbc->st_ & KBC_STATUS_OBF) {
        //
        //Reset the OBF flag
        //
        vkbc->st_ &= (~(KBC_STATUS_AUXB | KBC_STATUS_OBF));
    }

    //
    //If we send 1 byte of mouse already , then try to send the remaining 2 bytes of data first and process the keyboard data.
    //

    if(PacketSize >0 && PacketSize < MOUSE_PACKET_SIZE) {

        //
        //Check the OBF, If it's empty , check Mouse queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->mouse_dev->onQueueFree)(kbc->mouse_dev);
        }

        //
        //Check the OBF, If it's empty , check KBD queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->kbd_dev->onQueueFree)(kbc->kbd_dev);
        }
    } else {
        //
        //Try to process the keyboard data first.
        //

        PacketSize=0;
        //
        //Check the OBF, If it's empty , check KBD queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->kbd_dev->onQueueFree)(kbc->kbd_dev);
        }

        //
        //Check the OBF, If it's empty , check Mouse queue , if it's has the data push the data to KBC.
        //
        if(!(vkbc->st_ & KBC_STATUS_OBF)) {
            (*kbc->mouse_dev->onQueueFree)(kbc->mouse_dev);
        }
  }

    return Data;
}

/**
    Keep the data in the output buffer and set the OBF bit set. Based on device set the status bit also.

    @param kbc     - pointer to the KBC structure
    @param Devtype - Data from which device (KBD or Mouse)
    @param orvd
    @param Data    - Data to the kbc buffer

    @retval BOOLEAN

**/

BOOLEAN
virtkbc_sendout (
    KBC         *kbc,
    PS2DEV_TYPE devtype,
    BOOLEAN     ovrd,
    UINT8       data
)
{
    VIRTKBC* vkbc = (VIRTKBC*)kbc;

#if OHCI_EMUL_SUPPORT
    //
    // Actual OBF status is maintained by the OHCI controller.
    // Update the internal structure
    //
    vkbc->st_ &= (~(KBC_STATUS_OBF | KBC_STATUS_AUXB));
    vkbc->st_ |= GetHceStatus() & (KBC_STATUS_OBF  | KBC_STATUS_AUXB);
#endif

    if( vkbc->st_ & KBC_STATUS_OBF) {
        if(vkbc->st_ & KBC_STATUS_AUXB) {
            if(MouseIrq) {
            #if IRQ_EMUL_SUPPORT
                GenerateIRQ12(vkbc);
            #endif
            }
        } else {
            #if IRQ_EMUL_SUPPORT
            GenerateIRQ1(vkbc);
            #endif
        }
        //
        //KBC already has the data.
        //
        return FALSE;
    }
    //
    //Save the data in the KBC buffer
    //
    vkbc->outb_ = data;

    //
    //Set the OBF full
    //
    vkbc->st_ |= KBC_STATUS_OBF;

#if OHCI_EMUL_SUPPORT
    //
    // This is the write place to do. Code flow may take IRQ disabled path or
    // Trap6064_Handler won't get control because of USB SMI
    //
    SetHceOutput(data);
#endif
    //
    //Set the device type ( from which device data is out)
    //
    if( devtype == PS2DEV_MOUSE) {
        vkbc->st_ |= KBC_STATUS_AUXB;
        //
        //Generate the IRQ12, as AUX OBF is set.
        //
        if(MouseIrq) {
#if IRQ_EMUL_SUPPORT
            GenerateIRQ12(vkbc);
#endif
            //
            //If the input is Mouse data, increment the Packet size, otherwise (responce byte) initlize to 0
            //
            if(ovrd)
                PacketSize=0;
            else
                PacketSize++;
        }
    }else {
        vkbc->st_ &= ~KBC_STATUS_AUXB;
        //
        //Generate IRQ1
        //
#if IRQ_EMUL_SUPPORT
        GenerateIRQ1(vkbc);
#endif
    }

    return TRUE;
}

/**
    Return the CCB data from the KBC buffer

    @param kbc  - pointer to the KBC structure

    @retval UINT8

**/

UINT8
vkbc_readCCB (
    KBC  *kbc)
{
    VIRTKBC* vkbc = (VIRTKBC*)kbc;
    return vkbc->ccb_;
}

//
// Initialize KBC structure
//
VOID
initKBC (
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
    Initialize KBC structure. Set initial state of the emulated KBC

    @param vkbc    - pointer to the VKBC structure
    @param kbd     - Pointer to the KBD structure
    @param mouse   - Pointer to the Mouse Structure

    @retval VOID

**/

VOID
InitVirtualKBC (
    VIRTKBC  *vkbc,
    PS2SINK  *kbd,
    PS2SINK  *mouse
)
{
    initKBC(&vkbc->kbc,kbd,mouse);

    //
    //Initialize the KBC Structure
    //
    vkbc->kbc.kbc_write_command = vkbc_write_command;
    vkbc->kbc.kbc_write_data = vkbc_write_data;
    vkbc->kbc.kbc_read_status = vkbc_read_status;
    vkbc->kbc.kbc_read_data = vkbc_read_data;
    vkbc->kbc.send_outb1 = virtkbc_sendout;
    vkbc->kbc.read_ccb = vkbc_readCCB;

    //
    //Initialize the CCB
    //
    vkbc->ccb_ = 0;
    //
    //Initialize the Port64 Status
    //
    vkbc->st_ = 0x1c;
    //
    //Initialize the Port64 command regsiter
    //
    vkbc->kbc_command_ = 0;
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

