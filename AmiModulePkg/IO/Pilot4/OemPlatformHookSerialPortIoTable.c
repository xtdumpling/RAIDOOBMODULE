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
/** @file OemPlatformHookSerialPortIoTable.c
  SIO init table in PEI phase. Just initialize the serial port for debug mode.
**/
//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <Library/AmiSioPeiLib.h>

///---------------------------------------------------------------------
///Decode table for debug serial port.
///---------------------------------------------------------------------
IO_DECODE_DATA Pilot4SerialPortDebugDecodeTable[]= {
    // -----------------------------
    //|  BaseAdd | UID  | Type |
    // -----------------------------
    {PILOT4_CONFIG_INDEX, 2, 0xFF},
    //Below decode is for DEBUG Mode
    {DEBUG_COM_PORT_ADDR, 0x01, dsUART},
};

///---------------------------------------------------------------------
///The serial port debug decode table count.
///---------------------------------------------------------------------
UINT8 Pilot4SerialPortDebugDecodeTableCount = sizeof(Pilot4SerialPortDebugDecodeTable)/sizeof(IO_DECODE_DATA);

///---------------------------------------------------------------------
///Init table for debug serial port.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA Pilot4SerialPortDebugInitTable[]= {
    // -----------------------------
    //|  Addr | DataMask  | DataValue |
    // -----------------------------
    //------------------------------------------------------------------
    // Enter Configuration Mode.
    //------------------------------------------------------------------
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_CONFIG_MODE_ENTER_VALUE},
    
    //------------------------------------------------------------------
    // Before init all logical devices, program Global register if needed.
    //------------------------------------------------------------------


    //------------------------------------------------------------------
    // Initialize the Serial Port for debug useage. Default is COMA
    //------------------------------------------------------------------
    // Select device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, DEBUG_LDN_UART},
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(DEBUG_COM_PORT_ADDR & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(DEBUG_COM_PORT_ADDR >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},

    //------------------------------------------------------------------
    // After init all logical devices,  Exit Configuration Mode.
    //------------------------------------------------------------------
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_CONFIG_MODE_EXIT_VALUE},

};

///---------------------------------------------------------------------
///The serial port debug init table count.
///---------------------------------------------------------------------
UINT8 Pilot4SerialPortDebugInitTableCount = sizeof(Pilot4SerialPortDebugInitTable)/sizeof(SIO_DEVICE_INIT_DATA);

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

