//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file AhciRecoveryAccess.c
    Provides Index Data Port Access to AhciRecovery Driver

**/

//---------------------------------------------------------------------------

#include    <Token.h>
#include    "AmiDxeLib.h"

//---------------------------------------------------------------------------

#define     INDEX_OFFSET_FROM_LBAR      0x10
#define     DATA_OFFSET_FROM_LBAR       0x14

/**
    Read the Dword Data using Index/Data access method

    @param  BaseAddress - BaseAddress of AHCI Controller
    @param  Index       - Index address to read

    @retval Value Read

**/
UINT32
ReadDataDword (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index
)
{ 
    IoWrite32(IO_PORT_ADDRESS+INDEX_OFFSET_FROM_LBAR,(UINT32)Index);
    return IoRead32(IO_PORT_ADDRESS+DATA_OFFSET_FROM_LBAR);
}

/**
    WriteRead the Dword Data using Index/Data access method

    @param  BaseAddress - BaseAddress of AHCI Controller
    @param  Index       - Index address to Write
    @param  Data        - Data to be written

    @retval Nothing

**/
VOID
WriteDataDword (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index, 
    IN  UINTN   Data
)
{ 
    IoWrite32(IO_PORT_ADDRESS+INDEX_OFFSET_FROM_LBAR,(UINT32)Index);
    IoWrite32(IO_PORT_ADDRESS+DATA_OFFSET_FROM_LBAR,(UINT32)Data);
}

/**
    Read the Word Data using Index/Data access method

    @param  BaseAddress - BaseAddress of AHCI Controller
    @param  Index       - Index address to read           

    @retval Value Read

**/
UINT16
ReadDataWord (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index
)
{ 
    IoWrite32(IO_PORT_ADDRESS+INDEX_OFFSET_FROM_LBAR,(UINT32)Index);
    return (UINT16)IoRead32(IO_PORT_ADDRESS+DATA_OFFSET_FROM_LBAR);
}

/**
    WriteRead the word Data using Index/Data access method

    @param BaseAddress - BaseAddress of AHCI Controller
    @param Index       - Index address to Write
    @param Data        - Data to be written

    @retval Nothing

**/
VOID
WriteDataWord (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index, 
    IN  UINTN   Data
)
{ 
    IoWrite32(IO_PORT_ADDRESS+INDEX_OFFSET_FROM_LBAR,(UINT32)Index);
    IoWrite32(IO_PORT_ADDRESS+DATA_OFFSET_FROM_LBAR,(UINT16)Data);
}

/**
    Read the Byte Data using Index/Data access method

    @param  BaseAddress - BaseAddress of AHCI Controller
    @param  Index       - Index address to read

    @retval Value Read

**/
UINT8
ReadDataByte (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index
)
{ 
    IoWrite32(IO_PORT_ADDRESS+INDEX_OFFSET_FROM_LBAR,(UINT32)Index);
    return (UINT8)IoRead32(IO_PORT_ADDRESS+DATA_OFFSET_FROM_LBAR);
}

/**
    WriteRead the Dword Data using Index/Data access method

    @param BaseAddress - BaseAddress of AHCI Controller
    @param Index       - Index address to Write
    @param Data        - Data to be written

    @retval Nothing

**/
VOID
WriteDataByte (
    IN  UINTN   BaseAddr,
    IN  UINTN   Index,
    IN  UINTN   Data
)
{ 
    IoWrite32(IO_PORT_ADDRESS+INDEX_OFFSET_FROM_LBAR,(UINT32)Index);
    IoWrite8(IO_PORT_ADDRESS+DATA_OFFSET_FROM_LBAR,(UINT8)Data);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
