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

/** @file KbcIoTrap.h
    Header file for IoTrap controller for Emulation

**/

#ifndef _KBC_IO_TRAP_H_
#define _KBC_IO_TRAP_H_

//
// Trap Dispatcher
//
typedef struct _IOTRAPDISPATCH {
    VOID (* trap_function)();
} IOTRAPDISPATCH;

BOOLEAN IoTrap_HasTrapStatus ();
BOOLEAN IoTrapEnable (BOOLEAN);
BOOLEAN NonSmmIoTrapEnable (BOOLEAN);

VOID ClearIoTrapStatusReg ();
VOID WriteIoTrapCfg (BOOLEAN);

VOID NonSmmClearIoTrapStatusReg ();
BOOLEAN NonSmmIoTrap_HasTrapStatus();
VOID NonSmmWriteIoTrapCfg (BOOLEAN);

VOID CheckTrappedCycleRegister(UINT32 *RegAddr, UINT8 *Type);
BOOLEAN GetIoTrapRegisterStatus();

#endif

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

