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

typedef struct _LPC_AHB_DATA{
    UINT8           Offset;
    UINT32           And;
    UINT32           Or;
} LPC_AHB_DATA;

///----------------------------------------------------------------------------
///  the table is used for LPC_AHB access function. OEM need to porting.
///----------------------------------------------------------------------------
LPC_AHB_DATA AST2500_SUC_Access_For_Debug_Table[] = {
    //---------------------------------------------------------------------
    // program SCU Multi-Pin if needed.
    //---------------------------------------------------------------------
    {0x00, 0x00000000 , 0x1688A8A8},//Unlock SCU Registers
    #if (AST2500_SERIAL_PORT1_PRESENT)
    {0x84, 0xFF00FFFF , 0x00FF0000},//SCU84[23:16]="FFh" Enable UART1 or video VPB Multi-Function pins
    {0x90, 0xFFFFFFCF , 0x00000000},//SCU90[5:4]="00b" Disable digital video Multi-Function pins
    #endif //#if (AST2500_SERIAL_PORT1_PRESENT)
    {0x00, 0x00000000 , 0x00000000},//Lock SCU Registers for BMC KCS interface not work issue.
};
///---------------------------------------------------------------------
///The AST2500 SUC Access for Debug table count.
///---------------------------------------------------------------------
UINT8 AST2500_SUC_Access_For_Debug_Table_Count = sizeof(AST2500_SUC_Access_For_Debug_Table)/sizeof(IO_DECODE_DATA);


///---------------------------------------------------------------------
///Decode table for debug serial port.
///---------------------------------------------------------------------
IO_DECODE_DATA AST2500SerialPortDebugDecodeTable[]= {
    // -----------------------------
    //|  BaseAdd | UID  | Type |
    // -----------------------------
    {AST2500_CONFIG_INDEX, 2, 0xFF},
    //Below decode is for DEBUG Mode
#if (DEBUG_COM_PORT_ADDR == 0x2F0 || DEBUG_COM_PORT_ADDR == 0x2E0)
    {0x2E0, 0x20, 0xFF}, // 0x2E0~0x2FF , open a IODecode section for UART5 6
#else
    {DEBUG_COM_PORT_ADDR, 0x01, dsUART},
#endif
};
///---------------------------------------------------------------------
///The serial port debug decode table count.
///---------------------------------------------------------------------
UINT8 AST2500SerialPortDebugDecodeTableCount = sizeof(AST2500SerialPortDebugDecodeTable)/sizeof(IO_DECODE_DATA);

///---------------------------------------------------------------------
///Init table for debug serial port.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA AST2500SerialPortDebugInitTable[]= {
    // -----------------------------
    //|  Addr | DataMask  | DataValue |
    // -----------------------------
    //------------------------------------------------------------------
    // Enter Configuration Mode.
    //------------------------------------------------------------------
    {AST2500_CONFIG_INDEX, 0x00, AST2500_CONFIG_MODE_ENTER_VALUE},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_CONFIG_MODE_ENTER_VALUE},

    //------------------------------------------------------------------
    // Before init all logical devices, program Global register if needed.
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // Initialize the Serial Port for debug useage. Default is COMA
    //------------------------------------------------------------------
    // Select device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, DEBUG_LDN_UART},
    // Program Base Addr
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE1_LO_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, (UINT8)(DEBUG_COM_PORT_ADDR & 0xFF)},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE1_HI_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, (UINT8)(DEBUG_COM_PORT_ADDR >> 8)},
    // Activate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_ACTIVATE_VALUE},

    //------------------------------------------------------------------
    // After init all logical devices,  Exit Configuration Mode.
    //------------------------------------------------------------------
    {AST2500_CONFIG_INDEX, 0x00, AST2500_CONFIG_MODE_EXIT_VALUE},

};

///---------------------------------------------------------------------
///The serial port debug init table count.
///---------------------------------------------------------------------
UINT8 AST2500SerialPortDebugInitTableCount = sizeof(AST2500SerialPortDebugInitTable)/sizeof(SIO_DEVICE_INIT_DATA);

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

