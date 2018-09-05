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

/** @file KbcDevEmul.h
    Devices header file

**/

#ifndef __INC_VKBC_H__
#define __INC_VKBC_H__

//---------------------------------------------------------------------------
#include "Kbc.h"

//---------------------------------------------------------------------------

#define KBD_MOUSE_INTR_DISABLE              0x75
#define MOUSE_INTR_DISABLE                  0x65
#define MOUSE_INTR_ENABLE                   0x47

//
//Virtual KBC
//
typedef struct _VIRTKBC VIRTKBC;

struct _VIRTKBC{
    KBC     kbc;
    UINT8   ccb_;
    UINT8   kbc_command_;
    UINT8   st_;
    UINT8   outb_;
    BOOLEAN TwoByteCmd;
    BOOLEAN DelaySendingDataCmd;
    BOOLEAN fCcb_loaded;
};

VOID InitVirtualKBC(VIRTKBC* vkbc, PS2SINK* kbd, PS2SINK* mouse );

//
//Legacy KBC. Keep both _VIRTKBC and _LEGACYKBC synced up till DelaySendingDataCmd
//
typedef struct _LEGACYKBC LEGACYKBC;

struct _LEGACYKBC {
    KBC         kbc_;
    UINT8       ccb_;
    UINT8       kbc_command_;
    UINT8       st_;
    UINT8       outb_;
    BOOLEAN     TwoByteCmd;
    BOOLEAN     DelaySendingDataCmd;
    BOOLEAN     fCcb_loaded;
};
VOID InitLegacyKBC (LEGACYKBC* kbc, PS2SINK* kbd, PS2SINK* mouse);

//
//Virtual KBD & Legacy KBD
//

typedef struct _VIRTKBD VIRTKBD;
typedef struct _VIRTKBD LEGACYKBD;

#define KBDQUEUE_SIZE 10

typedef struct _VIRTKBD{
    PS2SINK sink;

    EFI_EMUL6064KBDINPUT_PROTOCOL kbdInput_;

    UINT8   queue[KBDQUEUE_SIZE];
    UINT8*  qhead;
    UINT8*  qtail;


    UINT8*  option_ptr;
    UINT8   read_code_action;
    UINT8   typematicRateDelay;
    UINT8   scancodepage;
    BOOLEAN Enabled;
};

VOID InitVirtualKbd (KBC* , VIRTKBD*);
VOID InitLegacyKbd (KBC* , LEGACYKBD*);

//
//Virtual Mouse & Legacy Mouse
//

#define MOUSE_PACKET_SIZE   3

typedef struct _VIRTMOUSE VIRTMOUSE;
typedef struct _VIRTMOUSE LEGACYMOUSE;


#define MOUSEQUEUE_SIZE 12

typedef struct _VIRTMOUSE{
    PS2SINK sink;

    EFI_EMUL6064MSINPUT_PROTOCOL msInput_;

    UINT8   queue[MOUSEQUEUE_SIZE];
    UINT8*  qhead;
    UINT8*  qtail;

    UINT8*  option_ptr;
    UINT8   samplingRate_;
    UINT8   resolution_;

    int     x, y;
    int     fFreshPacket;
    BOOLEAN Enabled;
    int     streaming;
    PS2MouseData dataPacket;
};

VOID InitVirtualMouse (KBC* kbc, VIRTMOUSE* mouse);
VOID InitLegacyMouse (KBC* kbc, LEGACYMOUSE* mouse);

#endif // __INC_VKBC_H__

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

