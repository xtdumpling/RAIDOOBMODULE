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

/** @file Kbc.h
    KBC Header file has the structure definition for Virtual and Legacy devices 

**/

#ifndef __INC_KBC_H__
#define __INC_KBC_H__

//---------------------------------------------------------------------------

#include "Token.h"

//---------------------------------------------------------------------------

typedef struct _KBC KBC;
typedef struct _PS2SINK PS2SINK;

typedef enum {
    PS2DEV_KBD = 0x0,
    PS2DEV_MOUSE = 0x1,
} PS2DEV_TYPE;
#define     KBC_SENDQUEUE_SIZE 15

struct _PS2SINK {
    //
    //notify about a command from application
    //
    VOID (*onCommand)(PS2SINK* dev, UINT8 cmd );

    //
    //notify that queue is empty; pull streaming data
    //
    VOID (*onQueueFree)(PS2SINK* dev);

    KBC* kbc;
    BOOLEAN present_;
    BOOLEAN presence_detected_;
};

struct _KBC{
    VOID (*kbc_write_command)(KBC* kbc, UINT8 cmd );
    VOID (*kbc_write_data)(KBC* kbc, UINT8 data );
    UINT8 (*kbc_read_status)(KBC* kbc );
    UINT8 (*kbc_read_data)(KBC* kbc );

    BOOLEAN (*send_outb1)( KBC* kbc, PS2DEV_TYPE devtype, BOOLEAN ovrd, UINT8 data );
    UINT8 (*read_ccb)(KBC* kbc);
    PS2SINK* mouse_dev;
    PS2SINK* kbd_dev;
    UINT8 send_queue[KBC_SENDQUEUE_SIZE];
    UINT8 send_queue_aux[KBC_SENDQUEUE_SIZE];
    UINT8* sqHead[2];
    UINT8* sqTail[2];

    int     nSeq;
    UINT8   aux_en; // aux interface enabled/disabled (value is a KBC command)
    UINT8   kbd_en; // kbd interface enabled/disabled (value is a KBC command)
};

#define KBC_STATUS_REG              0x64
#define KBC_COMMAND_REG             0x64
#define KBC_DATA_REG                0x60
#ifndef KBCTIMEOUT
#define KBCTIMEOUT                  0x1000
#endif
//
// KBC Status bits definition
//
#define KBC_STATUS_OBF              0x1
#define KBC_STATUS_IBF              0x2
#define KBC_STATUS_SYS              0x4
#define KBC_STATUS_A2               0x8
#define KBC_STATUS_INH              0x10
#define KBC_STATUS_AUXB             0x20
#define KBC_STATUS_TO               0x40            //Timeout detected
#define KBC_STATUS_PERR             0x80            //Parity error detected

//
// KBC CCB bits definition
//
#define KBC_CCB_INT             0x01
#define KBC_CCB_INT2            0x02
#define KBC_CCB_SYS             0x04
#define KBC_CCB_EN              0x10
#define KBC_CCB_EN2             0x20
#define KBC_CCB_XLAT            0x40

//
//KBC output bits definition
//
#define CCB_KEYBOARD_DISABLED               0x10
#define CCB_MOUSE_DISABLED                  0x20
#define CCB_TRANSLATE_SCAN_CODE_BIT_MASK    0x40

//
// KBC Commands
//
#define KBCCMD_WriteAuxBuffer           0xD3
#define KBCCMD_WriteKbdBuffer           0xD2
#define KBCCMD_WriteAuxDev              0xD4
#define KBCCMD_READ_CCB                 0x20
#define KBCCMD_WRITE_CCB                0x60
#define KBCCMD_LOCK                     USB_SEND_COMMAND_TO_KBC
#define KBCCMD_AUXENBL                  0xA8
#define KBCCMD_AUXDSBL                  0xA7
#define KBCCMD_KBDENBL                  0xAE
#define KBCCMD_KBDDSBL                  0xAD
#define KBCCMD_CheckAux                 0xA9
#define KBCCMD_CheckKbd                 0xAB
#define KBCCMD_SelfTest                 0xAA
#define KBCCMD_ReadInPort               0xC0
#define KBCCMD_ReadOutPortL             0xC1
#define KBCCMD_ReadOutPortH             0xC2
#define KBCCMD_ReadOutPort              0xD0
#define KBCCMD_WriteOutPort             0xD1
#define KBCCMD_ResetSystem              0xFE

//
// KBC Command Status Response
//
#define KBCCMD_RES_SelfTestOk           0x55
#define KBCCMD_RES_KBInterfaceOk        0

#endif // __INC_KBC_H__

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


