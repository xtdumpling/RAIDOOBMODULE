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
/** @file Pilot4PlatformHookSerialPortLib.c
  The file initialize the serial port for PlatformHookLib.
**/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <Library/AmiSioPeiLib.h>

extern IO_DECODE_DATA Pilot4SerialPortDebugDecodeTable[];
extern UINT8 Pilot4SerialPortDebugDecodeTableCount;
extern SIO_DEVICE_INIT_DATA Pilot4SerialPortDebugInitTable[];
extern UINT8 Pilot4SerialPortDebugInitTableCount;
/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval  RETURN_SUCCESS    The platform specific initialization succeeded.
  @retval  Others            The platform specific initialization could not be completed.
**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (VOID)
{
    //1. Decode
    UINT8 index;

    for(index=0; index<Pilot4SerialPortDebugDecodeTableCount; index++)
        AmiSioLibSetLpcDeviceDecoding(NULL, \
                                      Pilot4SerialPortDebugDecodeTable[index].BaseAdd, \
                                      Pilot4SerialPortDebugDecodeTable[index].UID, \
                                      Pilot4SerialPortDebugDecodeTable[index].Type);
    //2. Enable IO
    //3. Set Serial port address
    ProgramRtRegisterTable(0x00, Pilot4SerialPortDebugInitTable, Pilot4SerialPortDebugInitTableCount);

    return  RETURN_SUCCESS;
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

