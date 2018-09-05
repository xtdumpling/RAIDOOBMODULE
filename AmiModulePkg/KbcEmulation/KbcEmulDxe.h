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

/** @file KbcEmulDxe.h
    KBC emulation header file for Non-Smm Mode

**/

//---------------------------------------------------------------------------
#include <Efi.h>
#include <AmiDxeLib.h>
#include "Token.h"
#include <Protocol/Emul6064Trap.h>
#include <Protocol/CpuIo2.h>

//---------------------------------------------------------------------------

typedef struct _HC_DEV_STRUC {
    UINT16          wPCIDev;
    UINT16          wHCType;
} HC_DEV_STRUC;

extern EFI_CPU_IO2_PROTOCOL *CpuIo;

//
//Checks if Trap status is set
//
BOOLEAN     NonSmmTrapEnable(BOOLEAN );
BOOLEAN     NonSmmGetTrapStatus();
UINT8       NonSmmByteReadIO(UINT16 wIOAddr);
VOID        NonSmmByteWriteIO (UINT16 wIOAddr, UINT8 bValue);
UINT16      NonSmmWordReadIO(UINT16 wIOAddr);
VOID        NonSmmWordWriteIO (UINT16 wIOAddr, UINT16 wValue);
UINT32      NonSmmDwordReadIO(UINT16 wIOAddr);
VOID        NonSmmDwordWriteIO(UINT16 wIOAddr, UINT32 dValue);
UINT32      NonSmmReadPCIConfig(UINT16 BusDevFunc, UINT8 Register);
VOID        NonSmmByteWritePCIConfig(UINT16 BusDevFunc, UINT8 Register, UINT8 Value);
VOID        NonSmmWordWritePCIConfig(UINT16 BusDevFunc, UINT8 Register, UINT16 Value);
VOID        NonSmmDwordWritePCIConfig(UINT16 BusDevFunc, UINT8 Register, UINT32 Value);
UINT32      NonSmmReadPCIConfig(UINT16 BusDevFunc, UINT8 Register);

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

