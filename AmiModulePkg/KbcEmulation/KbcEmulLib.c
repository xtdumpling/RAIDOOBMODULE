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


/** @file KbcEmulLib.c
    KBC emulation lib file that used in other files

**/

//---------------------------------------------------------------------------

#include "KbcEmul.h"
#include "KbcEmulLib.h"

//---------------------------------------------------------------------------

UINT16  WritePCIConfig(UINT16, UINT8);

/**
    This routine reads a Byte from the specified IO address

    @param wIOAddr     I/O address to read

    @retval UINT8

**/

UINT8
ByteReadIO (
    UINT16  wIOAddr
)
{
    UINT8 value;
    gSmst1->SmmIo.Io.Read(&gSmst1->SmmIo, SMM_IO_UINT8, (UINT64)wIOAddr, 1, &value);
    return value;
}

/**
    This routine writes a byte to the specified IO address

    @param wIOAddr     I/O address to write
    @param bValue      Byte value to write

    @retval VOID

**/

VOID
ByteWriteIO (
    UINT16  wIOAddr,
    UINT8   bValue
)
{
	gSmst1->SmmIo.Io.Write(&gSmst1->SmmIo, SMM_IO_UINT8, (UINT64)wIOAddr, 1, &bValue);
}

/**
    This routine reads a Word from the specified IO address

    @param wIOAddr     I/O address to read

    @retval Value read

**/

UINT16
WordReadIO (
    UINT16  wIOAddr
)
{
    UINT16 value;
    gSmst1->SmmIo.Io.Read(&gSmst1->SmmIo, SMM_IO_UINT16, (UINT64)wIOAddr, 1, &value);
    return  value;
}

/**
    This routine writes a word to the specified IO address

    @param wIOAddr     I/O address to write
    @param wValue      Word value to write

    @retval VOID

**/

VOID
WordWriteIO (
    UINT16 wIOAddr, 
    UINT16 wValue
)
{
	gSmst1->SmmIo.Io.Write(&gSmst1->SmmIo, SMM_IO_UINT16, (UINT64)wIOAddr, 1, &wValue);
}

/**
    This routine reads a dword from the specified IO address

    @param wIOAddr     I/O address to read

    @retval Value read

**/

UINT32
DwordReadIO (
    UINT16  wIOAddr
)
{
    UINT32  value;
    gSmst1->SmmIo.Io.Read(&gSmst1->SmmIo, SMM_IO_UINT32, (UINT64)wIOAddr, 1, &value);
    return  value;
}

/**
    This routine writes a double word to the specified IO address

    @param wIOAddr     I/O address to write
    @param dValue      Double word value to write

    @retval VOID

**/

VOID
DwordWriteIO (
    UINT16 wIOAddr,
    UINT32 dValue
)
{
    gSmst1->SmmIo.Io.Write(&gSmst1->SmmIo, SMM_IO_UINT32, (UINT64)wIOAddr, 1, &dValue);
}

/**
    This routine reads from the PCI configuration space register
    the value can be typecasted to 8bits - 32bits

    @param BusDevFunc - Bus, device & function number of the PCI device
    @param Register   - Register offset to read

    @retval UINT32

**/

UINT32
ReadPCIConfig (
    UINT16  BusDevFunc, 
    UINT8   Register
)
{
    UINT32 data;
    DwordWriteIO(0xCF8, (UINT32)(0x80000000 | (BusDevFunc<<8) | (Register & 0xFC)));
    data = DwordReadIO(0xCFC);
    return (data >> ((Register & 3) << 3)); // Adjust uneven register

}

/**
    This routine writes a byte value to the PCI configuration
    register space

    @param BusDevFunc - Bus, device & function number of the PCI device
    @param Register   - Register offset to read
    @param Value      - Value to write

    @retval VOID

**/

VOID
ByteWritePCIConfig (
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT8   Value
)
{
    UINT16 wIOAddr;
    wIOAddr = WritePCIConfig(BusDevFunc, Register);
    ByteWriteIO (wIOAddr, Value);
}

/**
    This routine writes a byte value to the PCI configuration
    register space

    @param BusDevFunc - Bus, device & function number of the PCI device
    @param Register   - Register offset to read
    @param Value      - Value to write

    @retval VOID

**/

VOID
WordWritePCIConfig (
    UINT16  BusDevFunc,
    UINT8   Register, 
    UINT16  Value
)
{
    UINT16 wIOAddr;
    wIOAddr = WritePCIConfig(BusDevFunc, Register);
    WordWriteIO (wIOAddr, Value);
}

/**
    This routine writes a Dword value to the PCI configuration
    register space

    @param BusDevFunc - Bus, device & function number of the PCI device
    @param Register   - Register offset to read
    @param Value      - Value to write

    @retval VOID
**/

VOID
DwordWritePCIConfig (
    UINT16  BusDevFunc,
    UINT8   Register,
    UINT32  Value
)
{
    UINT16 wIOAddr;
    wIOAddr = WritePCIConfig(BusDevFunc, Register);
    DwordWriteIO (wIOAddr, Value);
}

/**
    This function opens PCI configuration for a given register

    @param wBDF  - Bus, device and function number
    @param bReg  - Register number to read

    @retval UINT16

**/

UINT16
WritePCIConfig (
    UINT16  wBDF, 
    UINT8   bReg
)
{
    DwordWriteIO(0xCF8, (UINT32)(0x80000000 | (wBDF<<8) | (bReg & 0xFC)));
    return (UINT16)(0xCFC+(bReg & 3));
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


