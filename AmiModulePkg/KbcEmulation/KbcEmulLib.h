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

/** @file KbcEmulLib.h
    KBC emulation Lib header file used in other files

**/

#ifndef _EMUL6064LIB_H_INC_

UINT8       ByteReadIO(UINT16 wIOAddr);
VOID        ByteWriteIO (UINT16 wIOAddr, UINT8 bValue);
UINT16      WordReadIO(UINT16 wIOAddr);
VOID        WordWriteIO (UINT16 wIOAddr, UINT16 wValue);
UINT32      DwordReadIO(UINT16 wIOAddr);
VOID        DwordWriteIO(UINT16 wIOAddr, UINT32 dValue);
UINT32      ReadPCIConfig(UINT16 BusDevFunc, UINT8 Register);
VOID        ByteWritePCIConfig(UINT16 BusDevFunc, UINT8 Register, UINT8 Value);
VOID        WordWritePCIConfig(UINT16 BusDevFunc, UINT8 Register, UINT16 Value);
VOID        DwordWritePCIConfig(UINT16 BusDevFunc, UINT8 Register, UINT32 Value);
UINT32      ReadPCIConfig(UINT16 BusDevFunc, UINT8 Register);

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

