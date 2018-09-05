//*********************************************************************
//*********************************************************************
//**                                                                 **
//**       (C)Copyright 1993-2015 Supermicro Computer, Inc.          **
//**                                                                 **
//*********************************************************************
//*********************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Fixed AST2500 COM port console redirection can't work in eSPI mode.
//      Reason:     Refer information from Aspeed FAE, the Serial IRQ interrupt type need set 
//                  to 0x00 before enabling UART in eSPI mode.
//      Auditor:    Jimmy Chiu
//      Date:       Jul/20/2016
//
//**********************************************************************
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
/** @file OemPeiIoTable.c
  SIO init table in PEI phase. Any customers have to review below tables
  for themselves platform and make sure each initialization is necessary.

  @note In all tables, only fill with necessary setting. Don't fill with default
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
LPC_AHB_DATA AST2500_SUC_access_Table[] = {
    //---------------------------------------------------------------------
    // program SCU Multi-Pin if needed.
    //---------------------------------------------------------------------
    {0x00, 0x00000000 , 0x1688A8A8},//Unlock SCU Registers
    #if (AST2500_SERIAL_PORT1_PRESENT)
    {0x84, 0xFF00FFFF , 0x00FF0000},//SCU84[23:16]="FFh" Enable UART1 or video VPB Multi-Function pins
    {0x90, 0xFFFFFFCF , 0x00000000},//SCU90[5:4]="00b" Disable digital video Multi-Function pins
    #endif //#if (AST2500_SERIAL_PORT1_PRESENT)
    #if (AST2500_SERIAL_PORT2_PRESENT)
    {0x84, 0x00FFFFFF , 0xFF000000},//SCU84[31:24]="FFh" Enable UART2 or video VPB Multi-Function pins
    {0x90, 0xFFFFFFCF , 0x00000000},//SCU90[5:4]="00b" Disable digital video Multi-Function pins
    #endif //#if (AST2500_SERIAL_PORT2_PRESENT)
    #if (AST2500_SERIAL_PORT3_PRESENT)
    {0x80, 0xFF00FFFF , 0x00FF0000},//SCU80[23:16]="FFh" Enable UART3 Multi-Function pins
    #endif //#if (AST2500_SERIAL_PORT3_PRESENT)
    #if (AST2500_SERIAL_PORT4_PRESENT)
    {0x80, 0x00FFFFFF , 0xFF000000},//SCU80[31:24]="FFh" Enable UART4 Multi-Function pins
    #endif //#if (AST2500_SERIAL_PORT4_PRESENT)

    {0x00, 0x00000000 , 0x00000000},//Lock SCU Registers for BMC KCS interface not work issue.
};
///---------------------------------------------------------------------
///The AST2500 SUC Access table count.
///---------------------------------------------------------------------
UINT8 AST2500_SUC_access_Table_Count = sizeof(AST2500_SUC_access_Table)/sizeof(IO_DECODE_DATA);

///---------------------------------------------------------------------
///Decode table for PEI phase.
///---------------------------------------------------------------------
IO_DECODE_DATA AST2500PeiDecodeTable[]= {
    // -----------------------------
    //|  BaseAdd | UID  | Type |
    // -----------------------------
    {AST2500_CONFIG_INDEX, 2, 0xFF},
#if (AST2500_KEYBOARD_PRESENT)
    {0x60, 0, dsPS2K},                                  // KBC decode
#endif
    //Below decode is for recovery mode
#if (AST2500_RECOVERY_SUPPORT)
    {RECOVERY_COM_PORT_ADDR, 0x02, dsUART},
#endif
    //Below decode is for SIO generic IO decode
#if defined(AST2500_TOTAL_BASE_ADDRESS) && (AST2500_TOTAL_BASE_ADDRESS != 0)
    {AST2500_TOTAL_BASE_ADDRESS, AST2500_TOTAL_LENGTH, 0xFF},
#endif
    // !!!!Attention!!!!This is necessary
    //OEM_TODO//OEM_TODO//OEM_TODO//OEM_TODO
    // If your com port number > 2 , you'd add more table for more com ports.
    {0x3E0, 0x10, 0xFF}, // 0x3E0~0x3F0 , open a IODecode section for UART3 4
#if (DEBUG_COM_PORT_ADDR != 0x2F0 && DEBUG_COM_PORT_ADDR != 0x2E0)
    {0x2E0, 0x20, 0xFF}, // 0x2E0~0x2FF , open a IODecode section for UART5 6
#endif
    // Add more OEM IO decode below.

};
///---------------------------------------------------------------------
///The PEI decode table count.
///---------------------------------------------------------------------
UINT8 AST2500PeiDecodeTableCount = sizeof(AST2500PeiDecodeTable)/sizeof(IO_DECODE_DATA);

///---------------------------------------------------------------------
///Init table for PEI phase.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA AST2500PeiInitTable[]= {
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
    // Initialize KBC if exist
    //------------------------------------------------------------------
#if (AST2500_KEYBOARD_PRESENT)
    // Seclect device KEYBOARD
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_PS2K},
    // Program Base Addr
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE1_HI_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE1_LO_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x60},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE2_HI_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE2_LO_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x64},
    // Program Interrupt
    {AST2500_CONFIG_INDEX, 0x00, AST2500_IRQ1_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x01},
    // Activate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_PS2K},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_DEACTIVATE_VALUE},
#endif //AST2500_KEYBOARD_PRESENT


    //------------------------------------------------------------------
    // Initialize Serial Port and Floppy Controller for Recovery
    //------------------------------------------------------------------
#if (AST2500_SERIAL_PORT1_PRESENT)
    // Select device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_UART1},
    // Program Serial IRQ interrupt type to 0x00 for eSPI mode
    {AST2500_CONFIG_INDEX, 0x00, 0x71},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
#endif //AST2500_SERIAL_PORT1_PRESENT

#if (AST2500_SERIAL_PORT2_PRESENT)
    // Select device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_UART2},
    // Program Serial IRQ interrupt type to 0x00 for eSPI mode
    {AST2500_CONFIG_INDEX, 0x00, 0x71},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
#endif //AST2500_SERIAL_PORT2_PRESENT

#if (AST2500_RECOVERY_SUPPORT)
    // Select device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, RECOVERY_LDN_UART},
    // Program Base Addr
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE1_LO_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, (UINT8)(RECOVERY_COM_PORT_ADDR & 0xFF)},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_BASE1_HI_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, (UINT8)(RECOVERY_COM_PORT_ADDR >> 8)},
    // Activate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_ACTIVATE_VALUE},

#endif //AST2500_RECOVERY_SUPPORT

    //------------------------------------------------------------------
    // Program and initialize some logical device if needed.
    //------------------------------------------------------------------
        
#if (AST2500_SWC_PRESENT)
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_SWC},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_IRQ1_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
//    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
//    {AST2500_CONFIG_DATA,  0x00, AST2500_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_SWC},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_DEACTIVATE_VALUE},
#endif //#if (AST2500_SWC_PRESENT)
    
#if (AST2500_GPIO_PORT_PRESENT)
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_GPIO},
//    {AST2500_CONFIG_INDEX, 0x00, 0x38},//bit7:enable inverse polarity of port 80h GPIO, BIt[0-4]: IO band selection
//    {AST2500_CONFIG_DATA,  0x00, 0x00},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_IRQ1_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
//    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
//    {AST2500_CONFIG_DATA,  0x00, AST2500_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_GPIO},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_DEACTIVATE_VALUE},
#endif //#if (AST2500_GPIO_PORT_PRESENT)
    
#if (AST2500_ILPC2AHB_PRESENT)
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_ILPC2AHB},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_IRQ1_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
//    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
//    {AST2500_CONFIG_DATA,  0x00, AST2500_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_ILPC2AHB},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_DEACTIVATE_VALUE},
#endif //#if (AST2500_LPC2AHB_PRESENT)
    
#if (AST2500_MAILBOX_PRESENT)
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_MAILBOX},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_IRQ1_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, 0x00},
//    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
//    {AST2500_CONFIG_DATA,  0x00, AST2500_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_MAILBOX},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_DEACTIVATE_VALUE},
#endif //#if (AST2500_MAILBOX_PRESENT)

    //------------------------------------------------------------------
    // After init all logical devices, program Global register if needed.
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // After init all logical devices,  Exit Configuration Mode.
    //------------------------------------------------------------------
    {AST2500_CONFIG_INDEX, 0x00, AST2500_CONFIG_MODE_EXIT_VALUE},
};

///---------------------------------------------------------------------
///The PEI init table count.
///---------------------------------------------------------------------
UINT8 AST2500PeiInitTableCount = sizeof(AST2500PeiInitTable)/sizeof(SIO_DEVICE_INIT_DATA);

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

