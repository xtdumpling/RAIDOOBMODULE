//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/ser_regs.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
//	ser_regs.h
//
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/ser_regs.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 1     11/02/12 10:07a Sudhirv
// AMIDebugRxPkg modulepart for AMI Debug for UEFI 3.0
// 
// 2     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//*****************************************************************


#define		RX_BUF_REG           0x00
#define		TX_BUF_REG           0x00
#define		INT_ENB_REG          0x01
#define		INT_ID_REG           0x02
#define		FIFO_CTRL_REG        0x02
#define		LINE_CTRL_REG        0x03
#define		MDM_CTRL_REG         0x04
#define		LINE_STATUS_REG      0x05
#define		MDM_STATUS_REG       0x06
#define		SCRATCH_REG          0x07
#define		DL_LSB_REG           0x00
#define		DL_MSB_REG           0x01

// Interrupt enable register
#define		INT_ENB_RXRD         0x01
#define		INT_ENB_TBE          0x02
#define		INT_ENB_ERBK         0x04
#define		INT_ENB_SINP         0x08

// Interrupt identification register
#define		INT_ID_MASK          0x06
#define		INT_ID_SINP          0x00
#define		INT_ID_TBE           0x02
#define		INT_ID_RXRD          0x04
#define		INT_ID_ERBK          0x06

#define		INT_ID_NO_INT_PND	0x01

// Data format register
#define		DATA_BITS_5          0x00
#define		DATA_BITS_6          0x01
#define		DATA_BITS_7          0x02
#define		DATA_BITS_8          0x03
#define		STOP_BITS_1          0x00
#define		STOP_BITS_2          0x04
#define		NO_PARITY            0x00
#define		ODD_PARITY           0x08
#define		EVEN_PARITY          0x18
#define		MARK_PARITY          0x28
#define		SPACE_PARITY         0x38
#define		BRK_OFF              0x00
#define		BRK_ON               0x40
#define		DLAB_ENB             0x80

// Divisor latch register
#define DL_LSB(BaudRate) ((UINT8)(115200L/BaudRate) & 0xff)
#define DL_MSB(BaudRate) ((UINT8)((115200L/BaudRate) >> 0x08) & 0xff)


// Modem control register
#define		DTR_ENB              0x01
#define		RTS_ENB              0x02
#define		OUT1_ENB             0x04
#define		OUT2_ENB             0x08
#define		LOOP_ENB             0x10

// Line status register
#define		LINE_STATUS_RxRD     0x01
#define		LINE_STATUS_OVRE     0x02
#define		LINE_STATUS_PARE     0x04
#define		LINE_STATUS_FRME     0x08
#define		LINE_STATUS_BREK     0x10
#define		LINE_STATUS_TBE      0x20
#define		LINE_STATUS_TXE      0x40

// Modem status register
#define		MDM_STATUS_DCTS		 0x01
#define		MDM_STATUS_DDSR      0x02
#define		MDM_STATUS_DRI       0x04
#define		MDM_STATUS_DDCD      0x08
#define		MDM_STATUS_CTS       0x10
#define		MDM_STATUS_DSR       0x20
#define		MDM_STATUS_RI        0x40
#define		MDM_STATUS_DCD       0x80

// BASE PORT Address
#define		PIC_MASK_PORT		 0x21
#define		PIC_CTRL_PORT		 0x20
#define		EOI					 0x20

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

