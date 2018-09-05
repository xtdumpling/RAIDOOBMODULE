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
/** @file OemDxeIoTable.c
  SIO init table in DXE phase. Any customers have to review below tables
  for themselves platform and make sure each initialization is necessary.

  @note  In all tables, only fill with necessary setting. Don't fill with default
**/
//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <Library/AmiSioDxeLib.h>

///---------------------------------------------------------------------
///This table will be filled when COMs isBeforeActivate.
///---------------------------------------------------------------------
DXE_DEVICE_INIT_DATA   AST2500_DXE_COM_Mode_Init_Table[] = {
    //-----------------------------
    //| Reg8 | AndData8  | OrData8  |
    //-----------------------------
    //OEM_TODO: Base on OEM board.
    //Program COM clock source Registers.
    {0xF0, 0xFD, 0x00}, //BIT1: 0: 24Mhz/13, 1: 24Mhz\no effect when com1 disabled
    {0xF0, 0xFD, 0x00}, //BIT1: 0: 24Mhz/13, 1: 24Mhz\no effect when com2 disabled
    {0xF0, 0xFD, 0x00}, //BIT1: 0: 24Mhz/13, 1: 24Mhz\no effect when com3 disabled
    {0xF0, 0xFD, 0x00}, //BIT1: 0: 24Mhz/13, 1: 24Mhz\no effect when com4 disabled
};

///---------------------------------------------------------------------
///This table will be filled when GPIO isBeforeActivate.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA   DXE_GPIO_Init_Table_Before_Active[] = {
    //-----------------------------
    //| Reg16 | AndData8  | OrData8  |
    //-----------------------------
    //OEM_TODO: Base on OEM board.
    //Program Global Configuration Registers.
    //These registers ask enter LDN== first.
    {0x07, 0x00, 0x04},  //

}; //DXE_GPIO_Init_Table_Before_Active
///---------------------------------------------------------------------
///The GPIO before active init table count.
///---------------------------------------------------------------------
UINT8 DXE_GPIO_Init_Table_Before_Active_Count = sizeof(DXE_GPIO_Init_Table_Before_Active)/sizeof(SIO_DEVICE_INIT_DATA);

///---------------------------------------------------------------------
///This table will be filled when GPIO isAfterActivate.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA   DXE_GPIO_Init_Table_After_Active[] = {
    //-----------------------------
    //| Reg16 | AndData8  | OrData8  |
    //-----------------------------
    //OEM_TODO: Base on OEM board.
    //Program Global Configuration Registers.
    //These registers ask enter LDN== first.
    {0x07,0x00, 0x07},                  //Select LDN

}; //DXE_GPIO_Init_Table_After_Active
///---------------------------------------------------------------------
///The GPIO after active init table count.
///---------------------------------------------------------------------
UINT8 DXE_GPIO_Init_Table_After_Active_Count = sizeof(DXE_GPIO_Init_Table_After_Active)/sizeof(SIO_DEVICE_INIT_DATA);

#if AST2500_SWC_PRESENT
///---------------------------------------------------------------------
///This table will be filled when SWC isBeforeActivate.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA   DXE_SWC_Init_Table_Before_Active[] = {
    //-----------------------------
    //| Reg16 | AndData8  | OrData8  |
    //-----------------------------
    //OEM_TODO: Base on OEM board.
    //Program Global Configuration Registers.
    //These registers ask enter LDN== first.
    {0x07,0x00, 0x04},                  //Select LDN
//    {0x60,0x00, (UINT8)(IO1B >> 8)},    //SWC base address bit[15:8]
//    {0x61,0x00, (UINT8)(IO1B & 0xFF)},  //SWC base address bit[7:1]
//    {0x62,0x00, (UINT8)(IO2B >> 8)},    //PM1b_EVT base address bit[15:8]
//    {0x63,0x00, (UINT8)(IO2B & 0xFF)},  //PM1b_EVT base address bit[7:2]
    {0x64,0x00, (UINT8)(IO3B >> 8)},    //PM1b_CNT base address bit[15:8]
    {0x65,0x00, (UINT8)(IO3B & 0xFF)},  //PM1b_CNT base address bit[7:1]
    {0x66,0x00, (UINT8)(IO4B >> 8)},    //GPE1_BLK base address bit[15:8]
    {0x67,0x00, (UINT8)(IO4B & 0xFF)},  //GPE1_BLK base address bit[7:3]
    
}; //DXE_SWC_Init_Table_Before_Active
///---------------------------------------------------------------------
///The SWC Before active init table count.
///---------------------------------------------------------------------
UINT8 DXE_SWC_Init_Table_Before_Active_Count = sizeof(DXE_SWC_Init_Table_Before_Active)/sizeof(SIO_DEVICE_INIT_DATA);
#endif//AST2500_SWC_PRESENT

#if AST2500_ILPC2AHB_PRESENT
///---------------------------------------------------------------------
///This table will be filled when iLPC2AHB isBeforeActivate.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA   DXE_ILPC2AHB_Init_Table_Before_Active[] = {
    //-----------------------------
    //| Reg16 | AndData8  | OrData8  |
    //-----------------------------
    //OEM_TODO: Base on OEM board.
    //Program Global Configuration Registers.
    //These registers ask enter LDN== first.
    {0x07,0x00, 0x0D},                  //Select LDN

}; //DXE_ILPC2AHB_Init_Table_Before_Active
///---------------------------------------------------------------------
///The ILPC2AHB Before active init table count.
///---------------------------------------------------------------------
UINT8 DXE_ILPC2AHB_Init_Table_Before_Active_Count = sizeof(DXE_ILPC2AHB_Init_Table_Before_Active)/sizeof(SIO_DEVICE_INIT_DATA);
#endif//AST2500_ILPC2AHB_PRESENT

#if AST2500_MAILBOX_PRESENT
///---------------------------------------------------------------------
///This table will be filled when MailBox isBeforeActivate.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA   DXE_MAILBOX_Init_Table_Before_Active[] = {
    //-----------------------------
    //| Reg16 | AndData8  | OrData8  |
    //-----------------------------
    //OEM_TODO: Base on OEM board.
    //Program Global Configuration Registers.
    //These registers ask enter LDN== first.
    {0x07,0x00, 0x0E},                  //Select LDN

}; //DXE_MailBox_Init_Table_Before_Active
///---------------------------------------------------------------------
///The MailBox Before active init table count.
///---------------------------------------------------------------------
UINT8 DXE_MAILBOX_Init_Table_Before_Active_Count = sizeof(DXE_MAILBOX_Init_Table_Before_Active)/sizeof(SIO_DEVICE_INIT_DATA);
#endif//AST2500_MAILBOX_PRESENT

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

