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

/** @file KbcEmulDxeLib.c
    KBC emulation Lib header file used in other files

**/

//---------------------------------------------------------------------------
#include "KbcEmulDxe.h"
#include "KbcEmulLib.h"

//---------------------------------------------------------------------------

/**
    This routine reads a Byte from the specified IO address

    @param wIOAddr     I/O address to read

    @retval UINT8

**/

UINT8
NonSmmByteReadIO (
    UINT16  wIOAddr
)
{
    UINT8 value;
    CpuIo->Io.Read(CpuIo, SMM_IO_UINT8, wIOAddr, 1, &value);
    return value;
}

/**
    This routine writes a byte to the specified IO address

    @param wIOAddr     I/O address to write
    @param bValue      Byte value to write

    @retval VOID

**/

VOID
NonSmmByteWriteIO (
    UINT16  wIOAddr,
    UINT8   bValue
)
{
    CpuIo->Io.Write(CpuIo, SMM_IO_UINT8, wIOAddr, 1, &bValue);
}

/**
    This routine reads a Word from the specified IO address

    @param wIOAddr     I/O address to read

    @retval UINT16

**/

UINT16
NonSmmWordReadIO (
    UINT16  wIOAddr
)
{
    UINT16 value;
    CpuIo->Io.Read(CpuIo, SMM_IO_UINT16, wIOAddr, 1, &value);
    return  value;
}

/**
    This routine writes a word to the specified IO address

    @param wIOAddr     I/O address to write
    @param wValue      Word value to write

    @retval VOID

**/

VOID
NonSmmWordWriteIO (
    UINT16  wIOAddr,
    UINT16  wValue
)
{
    CpuIo->Io.Write(CpuIo, SMM_IO_UINT16, wIOAddr, 1, &wValue);
}


/**
    This routine reads a dword from the specified IO address

    @param wIOAddr     I/O address to read

    @retval Value read

**/

UINT32
NonSmmDwordReadIO (
    UINT16  wIOAddr
)
{
    UINT32  value;
    CpuIo->Io.Read(CpuIo, SMM_IO_UINT32, wIOAddr, 1, &value);
    return  value;
}


/**
    This routine writes a double word to the specified IO address

    @param wIOAddr     I/O address to write
    @param dValue      Double word value to write

    @retval VOID

**/

VOID
NonSmmDwordWriteIO (
    UINT16 wIOAddr, 
    UINT32 dValue
)
{
    CpuIo->Io.Write(CpuIo, SMM_IO_UINT32, wIOAddr, 1, &dValue);
}

/**
    This function opens PCI configuration for a given register

    @param wBDF  - Bus, device and function number
    @param bReg  - Register number to read

    @retval UINT16

**/

UINT16
NonSmmWritePCIConfig (
    UINT16  wBDF,
    UINT8 bReg
)
{
    NonSmmDwordWriteIO(0xCF8, (UINT32)(0x80000000 | (wBDF<<8) | (bReg & 0xFC)));
    return (UINT16)(0xCFC+(bReg & 3));
}

/**
    This routine reads from the PCI configuration space register
    the value can be typecasted to 8bits - 32bits

    @param BusDevFunc - Bus, device & function number of the PCI device
    @param Register   - Register offset to read

    @retval UINT32

**/

UINT32
NonSmmReadPCIConfig (
    UINT16  BusDevFunc,
    UINT8   Register
)
{
    UINT32 data;
    NonSmmDwordWriteIO(0xCF8, (UINT32)(0x80000000 | (BusDevFunc<<8) | (Register & 0xFC)));
    data = NonSmmDwordReadIO(0xCFC);
    return (data >> ((Register & 3) << 3)); // Adjust uneven register

}

/**
    This routine writes a byte value to the PCI configuration
    register space

    @param BusDevFunc - Bus, device & function number of the PCI device
        Register   - Register offset to read
        Value      - Value to write

    @retval VOID

**/

VOID
NonSmmByteWritePCIConfig (
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT8   Value
)
{
    UINT16 wIOAddr;
    wIOAddr = NonSmmWritePCIConfig(BusDevFunc, Register);
    NonSmmByteWriteIO (wIOAddr, Value);
}

/**
    This routine writes a word value to the PCI configuration
    register space

    @param BusDevFunc - Bus, device & function number of the PCI device
    @param Register   - Register offset to write
    @param Value      - Value to write

    @retval VOID

**/

VOID
NonSmmWordWritePCIConfig (
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT16  Value
)
{
    UINT16 wIOAddr;
    wIOAddr = NonSmmWritePCIConfig(BusDevFunc, Register);
    NonSmmWordWriteIO (wIOAddr, Value);
}

/**
    This routine writes a Dword value to the PCI configuration
    register space

    @param BusDevFunc - Bus, device & function number of the PCI device
        Register   - Register offset to read
        Value      - Value to write

    @retval VOID
**/

VOID
NonSmmDwordWritePCIConfig (
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT32  Value
)
{
    UINT16 wIOAddr;
    wIOAddr = NonSmmWritePCIConfig(BusDevFunc, Register);
    NonSmmDwordWriteIO (wIOAddr, Value);
}

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

