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

/** @file KbcSb.h
    Header file for SB controller for Emulation

**/

//
// Trap Dispatcher
//
//
typedef struct _TRAPDISPATCH {
    UINT16 status_bit;
    VOID (* trap_function)();
} TRAPDISPATCH;

#define SB_SMIBYENDPS     (UINT16)(1U << 15)
#define SB_USBPIRQEN      (UINT16)(1U << 13)
#define SB_SMIBYUSB       (UINT16)(1U << 12)
#define SB_TRAPBY64W      (UINT16)(1U << 11)
#define SB_TRAPBY64R      (UINT16)(1U << 10)
#define SB_TRAPBY60W      (UINT16)(1U << 9)
#define SB_TRAPBY60R      (UINT16)(1U << 8)
#define SB_SMIATENDPS     (UINT16)(1U << 7)
#define SB_PSTATE         (UINT16)(1U << 6)
#define SB_A20PASSEN      (UINT16)(1U << 5)
#define SB_USBSMIEN       (UINT16)(1U << 4)
#define SB_64WEN          (UINT16)(1U << 3)
#define SB_64REN          (UINT16)(1U << 2)
#define SB_60WEN          (UINT16)(1U << 1)
#define SB_60REN          (UINT16)(1U )

#define SB_TRAPBY_MASK        (SB_TRAPBY64W | SB_TRAPBY64R | SB_TRAPBY60W | SB_TRAPBY60R | SB_SMIBYENDPS)

//enable bits for traps
#define SB_TRAPEN_MASK        (SB_64WEN | SB_60WEN  | SB_64REN | SB_60REN | SB_A20PASSEN )

BOOLEAN     Sb_HasTrapStatus ();
BOOLEAN     NonSmmSb_HasTrapStatus();
BOOLEAN     Sb_TrapEnable(BOOLEAN);
BOOLEAN     NonSmmSb_TrapEnable(BOOLEAN);
VOID        GenerateSbIRQ12();
VOID        GenerateSbIRQ1();

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

